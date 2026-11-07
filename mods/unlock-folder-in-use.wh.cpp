// ==WindhawkMod==
// @id              unlock-folder-in-use
// @name            Unlock "Folder In Use"
// @description     When Explorer says "Folder/File In Use", shows what is blocking it and lets you choose: end the processes and continue, or only end them
// @version         1.0.0
// @author          LyricsPortal
// @github          https://github.com/LyricsPortal
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -lrstrtmgr -luser32 -lshlwapi -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Unlock "Folder In Use"

When Explorer refuses to delete, move or rename something because it is in use ("Folder In Use" / "File In Use"), this mod finds out which processes are blocking it and lets you decide what to do.

When the warning appears, a small dialog lists the blocking processes and offers:

* **End processes and continue** - terminates the processes and presses *Try Again* for you, so Explorer completes the original operation.
* **Only end processes** - terminates the processes and leaves Explorer's warning open so you can retry yourself.
* **Cancel** - does nothing.

If Explorer windows are open inside the item being processed, they are moved up one level.

## How it works

* The mod runs as a tool in its own dedicated process, not inside Explorer, so a problem in it can't affect the taskbar or the desktop.
* The warning is detected through its internal UI Automation identifiers instead of its text, so it works with any display language.
* The blocked item is identified from the name shown in the warning. Only the text elements of the interrupt panel itself are read (starting from the *Try Again* button), never the rest of the progress window, and names are compared in full. Only the selected items (or entries inside selected folders) whose name matches exactly are scanned.
* The matched paths are listed in the details of the prompt, so you can see what was scanned before ending anything.
* Blockers are found with the Restart Manager, the current directory of each process, and the system handle table. Every Explorer process is excluded.
* Processes are opened while scanning and terminated through those same handles, so a process that exits and whose PID is reused is never terminated by mistake.

## Notes

* Ending a process terminates it immediately. Unsaved work in that process is lost.
* Elevated or protected processes can't be ended from this mod.
* If the item named in the warning can't be matched with the selection, the mod says so and does nothing.
* The mod relies on internal details of Explorer's operation dialog, which may change in future Windows versions.

## Settings

* **Modern dialogs** - use task-dialog style windows instead of classic message boxes.
* **Show message when nothing is found** - show a dialog when no blocking process could be identified.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modernDialogs: true
  $name: Modern dialogs
  $description: Use task-dialog style windows instead of classic message boxes
- showNoResultDialog: false
  $name: Show message when nothing is found
  $description: Show a dialog when no blocking process could be identified
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <shldisp.h>
#include <exdisp.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <restartmanager.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <atomic>
#include <algorithm>
#include <uiautomation.h>

static const wchar_t* kRetryId = L"eltInterruptRetryBtn";

static const wchar_t* kDialogClass = L"OperationStatusWindow";

struct ThreadInfo {
    HANDLE h;
    DWORD tid;
    bool isScan;
};

struct Locker {
    std::wstring name;
    HANDLE h = nullptr;
    bool canKill = false;
};

struct SrwLock {
    SRWLOCK* l;
    explicit SrwLock(SRWLOCK* lock) : l(lock) { AcquireSRWLockExclusive(l); }
    ~SrwLock() { ReleaseSRWLockExclusive(l); }
};

struct CoInit {
    HRESULT hr;
    explicit CoInit(DWORD mode) : hr(CoInitializeEx(nullptr, mode)) {}
    ~CoInit() { if (SUCCEEDED(hr)) CoUninitialize(); }
};

template <class T> struct Com {
    T* p = nullptr;
    ~Com() { if (p) p->Release(); }
    T** operator&() { return &p; }
    T* operator->() { return p; }
    operator bool() const { return p != nullptr; }
};

static std::atomic<bool> g_stop{false};
static HANDLE g_stopEvent = nullptr;
static HANDLE g_monitor = nullptr;
static std::wstring g_explorerPath;
static SRWLOCK g_threadsLock = SRWLOCK_INIT;
static std::vector<ThreadInfo> g_threads;
static std::set<HWND> g_handled;
static std::map<HWND, std::wstring> g_seen;
static std::vector<HWND> g_candidates;

static void CloseLockers(std::map<DWORD, Locker>& m) {
    for (auto& kv : m)
        if (kv.second.h) CloseHandle(kv.second.h);
    m.clear();
}

static void AddLocker(std::map<DWORD, Locker>& m, DWORD pid, const std::wstring& name, HANDLE h, bool canKill) {
    auto it = m.find(pid);
    if (it == m.end()) {
        Locker l;
        l.name = name;
        l.h = h;
        l.canKill = canKill;
        m[pid] = l;
        return;
    }
    if (!it->second.h && h) {
        it->second.h = h;
        it->second.canKill = canKill;
    } else if (h) {
        CloseHandle(h);
    }
}

static HANDLE OpenLockerProcess(DWORD pid, DWORD extra, bool& canKill) {
    canKill = true;
    HANDLE h = OpenProcess(extra | PROCESS_TERMINATE | SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (h) return h;
    canKill = false;
    return OpenProcess(extra | SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
}

static bool KillLocker(const Locker& l) {
    if (!l.h || !l.canKill) return false;
    if (WaitForSingleObject(l.h, 0) == WAIT_OBJECT_0) return true;
    bool ok = TerminateProcess(l.h, 1) != 0;
    if (ok) WaitForSingleObject(l.h, 3000);
    return ok;
}

static bool SafeWindowText(HWND h, WCHAR* buf, int n) {
    buf[0] = 0;
    DWORD_PTR r = 0;
    if (!SendMessageTimeoutW(h, WM_GETTEXT, (WPARAM)n, (LPARAM)buf,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 150, &r)) {
        buf[0] = 0;
        return false;
    }
    return true;
}

static bool CreateUia(IUIAutomation** out) {
    Com<IUIAutomation> ua;
    if (FAILED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ua))) || !ua)
        return false;
    Com<IUIAutomation2> ua2;
    if (SUCCEEDED(ua->QueryInterface(IID_PPV_ARGS(&ua2))) && ua2) {
        ua2->put_ConnectionTimeout(400);
        ua2->put_TransactionTimeout(800);
    }
    ua->AddRef();
    *out = ua.p;
    return true;
}

static bool FindRetry(IUIAutomation* ua, HWND h, IUIAutomationElement** out) {
    Com<IUIAutomationElement> root;
    if (FAILED(ua->ElementFromHandle(h, &root)) || !root) return false;

    VARIANT v;
    VariantInit(&v);
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(kRetryId);
    Com<IUIAutomationCondition> cond;
    HRESULT hr = ua->CreatePropertyCondition(UIA_AutomationIdPropertyId, v, &cond);
    VariantClear(&v);
    if (FAILED(hr) || !cond) return false;

    Com<IUIAutomationElement> el;
    if (FAILED(root->FindFirst(TreeScope_Descendants, cond.p, &el)) || !el) return false;

    BOOL offscreen = FALSE;
    el->get_CurrentIsOffscreen(&offscreen);
    if (offscreen) return false;

    if (out) {
        el->AddRef();
        *out = el.p;
    }
    return true;
}

static bool InterruptActive(IUIAutomation* ua, HWND h) {
    if (!IsWindow(h) || !IsWindowVisible(h)) return false;
    return FindRetry(ua, h, nullptr);
}

// Reads only the text elements of the interrupt panel. It starts from the
// Try Again button and walks up to the first container that holds text, never
// reaching the window root (which also holds the other operations).
static void CollectDialogTexts(IUIAutomation* ua, HWND h, std::vector<std::wstring>& out) {
    Com<IUIAutomationElement> btn;
    if (!FindRetry(ua, h, &btn) || !btn) return;

    Com<IUIAutomationElement> root;
    if (FAILED(ua->ElementFromHandle(h, &root)) || !root) return;

    Com<IUIAutomationTreeWalker> walker;
    if (FAILED(ua->get_ControlViewWalker(&walker)) || !walker) return;

    VARIANT v;
    VariantInit(&v);
    v.vt = VT_I4;
    v.lVal = UIA_TextControlTypeId;
    Com<IUIAutomationCondition> textCond;
    HRESULT hr = ua->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &textCond);
    VariantClear(&v);
    if (FAILED(hr) || !textCond) return;

    IUIAutomationElement* cur = btn.p;
    cur->AddRef();

    for (int depth = 0; depth < 8; depth++) {
        IUIAutomationElement* parent = nullptr;
        if (FAILED(walker->GetParentElement(cur, &parent)) || !parent) break;
        cur->Release();
        cur = parent;

        BOOL isRoot = FALSE;
        if (SUCCEEDED(ua->CompareElements(cur, root.p, &isRoot)) && isRoot) break;

        Com<IUIAutomationElementArray> arr;
        if (FAILED(cur->FindAll(TreeScope_Descendants, textCond.p, &arr)) || !arr) continue;

        int n = 0;
        arr->get_Length(&n);
        if (n <= 0) continue;

        for (int i = 0; i < n && i < 50; i++) {
            Com<IUIAutomationElement> el;
            if (FAILED(arr->GetElement(i, &el)) || !el) continue;
            BSTR name = nullptr;
            if (SUCCEEDED(el->get_CurrentName(&name)) && name) {
                if (name[0]) out.push_back(name);
                SysFreeString(name);
            }
        }
        break;
    }
    cur->Release();
}

static bool InvokeElement(IUIAutomationElement* el) {
    Com<IUnknown> unk;
    if (FAILED(el->GetCurrentPattern(UIA_InvokePatternId, &unk)) || !unk) return false;
    Com<IUIAutomationInvokePattern> inv;
    if (FAILED(unk->QueryInterface(IID_PPV_ARGS(&inv))) || !inv) return false;
    return SUCCEEDED(inv->Invoke());
}

static bool DefaultActionElement(IUIAutomationElement* el) {
    Com<IUnknown> unk;
    if (FAILED(el->GetCurrentPattern(UIA_LegacyIAccessiblePatternId, &unk)) || !unk) return false;
    Com<IUIAutomationLegacyIAccessiblePattern> pat;
    if (FAILED(unk->QueryInterface(IID_PPV_ARGS(&pat))) || !pat) return false;
    return SUCCEEDED(pat->DoDefaultAction());
}

static bool WaitInterruptGone(IUIAutomation* ua, HWND dlg, int ms) {
    for (int waited = 0; waited < ms; waited += 100) {
        if (WaitForSingleObject(g_stopEvent, 100) == WAIT_OBJECT_0) return true;
        if (!InterruptActive(ua, dlg)) return true;
    }
    return false;
}

static void PressRetry(HWND dlg) {
    if (!IsWindow(dlg)) return;
    EnableWindow(dlg, TRUE);

    Com<IUIAutomation> ua;
    if (CreateUia(&ua)) {
        Com<IUIAutomationElement> btn;
        if (FindRetry(ua.p, dlg, &btn) && btn) {
            if (InvokeElement(btn.p)) {
                Wh_Log(L"Retry pressed via Invoke");
                if (WaitInterruptGone(ua.p, dlg, 1500)) return;
            }
            if (DefaultActionElement(btn.p)) {
                Wh_Log(L"Retry pressed via default action");
                if (WaitInterruptGone(ua.p, dlg, 1500)) return;
            }
        }
    }

    PostMessageW(dlg, WM_COMMAND, MAKEWPARAM(IDRETRY, BN_CLICKED), 0);
    Wh_Log(L"Retry pressed via WM_COMMAND");
}

typedef HRESULT (WINAPI *TaskDialogIndirect_t)(const TASKDIALOGCONFIG*, int*, int*, BOOL*);

static TaskDialogIndirect_t GetTaskDialog() {
    static TaskDialogIndirect_t fn = []() -> TaskDialogIndirect_t {
        HMODULE m = LoadLibraryW(L"comctl32.dll");
        return m ? (TaskDialogIndirect_t)GetProcAddress(m, "TaskDialogIndirect") : nullptr;
    }();
    return fn;
}

static HRESULT CALLBACK TdCallback(HWND h, UINT msg, WPARAM, LPARAM, LONG_PTR) {
    if (msg == TDN_CREATED) {
        SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        if (g_stop) PostMessageW(h, TDM_CLICK_BUTTON, IDCANCEL, 0);
    }
    return S_OK;
}

enum Choice { CHOICE_CANCEL = 0, CHOICE_CLOSE_CONTINUE = 1, CHOICE_CLOSE_ONLY = 2 };

static void FillCommon(TASKDIALOGCONFIG& cfg, const std::wstring& instruction,
                       const std::wstring& content, const std::wstring& details) {
    cfg.cbSize = sizeof(cfg);
    cfg.hwndParent = nullptr;
    cfg.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
    cfg.pszWindowTitle = L"Unlock";
    cfg.pszMainInstruction = instruction.c_str();
    cfg.pszContent = content.empty() ? nullptr : content.c_str();
    cfg.pfCallback = TdCallback;
    if (!details.empty()) {
        cfg.pszExpandedInformation = details.c_str();
        cfg.pszCollapsedControlText = L"Details";
        cfg.pszExpandedControlText = L"Hide details";
    }
}

static void ShowInfo(const std::wstring& instruction, const std::wstring& content,
                     const std::wstring& details, bool error) {
    if (g_stop) return;
    if (GetTaskDialog() && Wh_GetIntSetting(L"modernDialogs")) {
        TASKDIALOGCONFIG cfg = {};
        FillCommon(cfg, instruction, content, details);
        cfg.dwCommonButtons = TDCBF_OK_BUTTON;
        if (error) cfg.pszMainIcon = TD_ERROR_ICON;
        int pressed = 0;
        if (SUCCEEDED(GetTaskDialog()(&cfg, &pressed, nullptr, nullptr))) return;
    }
    std::wstring text = instruction;
    if (!content.empty()) text += L"\n\n" + content;
    if (!details.empty()) text += L"\n\n" + details;
    MessageBoxW(nullptr, text.c_str(), L"Unlock",
                MB_OK | MB_TOPMOST | MB_SETFOREGROUND | (error ? MB_ICONERROR : MB_ICONINFORMATION));
}

static Choice ShowChoice(const std::wstring& instruction, const std::wstring& content,
                         const std::wstring& details, const std::wstring& primary,
                         const std::wstring& secondary) {
    const int kPrimary = 1001, kSecondary = 1002;
    const bool hasSecondary = !secondary.empty();
    if (g_stop) return CHOICE_CANCEL;

    if (GetTaskDialog() && Wh_GetIntSetting(L"modernDialogs")) {
        TASKDIALOG_BUTTON btns[2] = {{kPrimary, primary.c_str()}, {kSecondary, secondary.c_str()}};
        TASKDIALOGCONFIG cfg = {};
        FillCommon(cfg, instruction, content, details);
        cfg.dwFlags |= TDF_USE_COMMAND_LINKS_NO_ICON;
        cfg.dwCommonButtons = TDCBF_CANCEL_BUTTON;
        cfg.pButtons = btns;
        cfg.cButtons = hasSecondary ? 2 : 1;
        cfg.nDefaultButton = IDCANCEL;
        int pressed = 0;
        if (SUCCEEDED(GetTaskDialog()(&cfg, &pressed, nullptr, nullptr))) {
            if (pressed == kPrimary) return CHOICE_CLOSE_CONTINUE;
            if (pressed == kSecondary) return CHOICE_CLOSE_ONLY;
            return CHOICE_CANCEL;
        }
    }

    std::wstring text = instruction;
    if (!content.empty()) text += L"\n\n" + content;
    if (!details.empty()) text += L"\n\n" + details;
    if (hasSecondary) {
        text += L"\n\nYes: end processes and continue\nNo: only end processes\nCancel: do nothing";
        int r = MessageBoxW(nullptr, text.c_str(), L"Unlock",
                            MB_YESNOCANCEL | MB_DEFBUTTON3 | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND);
        if (r == IDYES) return CHOICE_CLOSE_CONTINUE;
        if (r == IDNO) return CHOICE_CLOSE_ONLY;
        return CHOICE_CANCEL;
    }
    text += L"\n\nContinue?";
    int r = MessageBoxW(nullptr, text.c_str(), L"Unlock",
                        MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND);
    return r == IDYES ? CHOICE_CLOSE_CONTINUE : CHOICE_CANCEL;
}

static bool PathStartsWith(const std::wstring& cwd, const std::wstring& root) {
    std::wstring a = cwd, b = root;
    while (a.size() > 3 && a.back() == L'\\') a.pop_back();
    while (b.size() > 3 && b.back() == L'\\') b.pop_back();
    if (a.size() < b.size()) return false;
    if (_wcsnicmp(a.c_str(), b.c_str(), b.size()) != 0) return false;
    return a.size() == b.size() || a[b.size()] == L'\\' || b.back() == L'\\';
}

static std::map<DWORD, std::wstring> ProcessNames() {
    std::map<DWORD, std::wstring> names;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return names;
    PROCESSENTRY32W pe = {sizeof(pe)};
    if (Process32FirstW(snap, &pe)) {
        do { names[pe.th32ProcessID] = pe.szExeFile; } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return names;
}

static bool IsExplorerExe(DWORD pid, bool assumeIfUnknown) {
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return assumeIfUnknown;
    WCHAR path[MAX_PATH * 2];
    DWORD n = ARRAYSIZE(path);
    BOOL ok = QueryFullProcessImageNameW(h, 0, path, &n);
    CloseHandle(h);
    if (!ok) return assumeIfUnknown;
    return _wcsicmp(path, g_explorerPath.c_str()) == 0;
}

static std::set<DWORD> BuildExcluded() {
    std::set<DWORD> ex;
    ex.insert(GetCurrentProcessId());
    for (auto& kv : ProcessNames()) {
        if (_wcsicmp(kv.second.c_str(), L"explorer.exe") == 0 && IsExplorerExe(kv.first, true))
            ex.insert(kv.first);
    }
    return ex;
}

static void CollectFiles(const std::wstring& path, std::vector<std::wstring>& files, size_t limit = 20000) {
    DWORD attr = GetFileAttributesW(path.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) return;
    files.push_back(path);
    if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) return;

    std::vector<std::wstring> stack{path};
    while (!stack.empty() && files.size() < limit && !g_stop) {
        std::wstring dir = stack.back(); stack.pop_back();
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW((dir + L"\\*").c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) continue;
        do {
            if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..")) continue;
            std::wstring full = dir + L"\\" + fd.cFileName;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                    files.push_back(full);
                    stack.push_back(full);
                }
            } else {
                files.push_back(full);
            }
        } while (FindNextFileW(h, &fd) && files.size() < limit && !g_stop);
        FindClose(h);
    }
}

// Whole-name comparison only: the text must be exactly the item name.
static bool TextMatchesName(const std::wstring& text, const std::wstring& name) {
    if (name.empty() || text.empty()) return false;
    return _wcsicmp(text.c_str(), name.c_str()) == 0;
}

static std::vector<std::wstring> ResolveTargets(const std::vector<std::wstring>& sel,
                                                const std::vector<std::wstring>& texts) {
    auto matches = [&](const std::wstring& path) {
        const WCHAR* n = PathFindFileNameW(path.c_str());
        for (auto& t : texts)
            if (TextMatchesName(t, n)) return true;
        return false;
    };

    std::vector<std::wstring> out;
    for (auto& p : sel) {
        if (g_stop) break;
        if (matches(p)) {
            out.push_back(p);
            continue;
        }
        DWORD attr = GetFileAttributesW(p.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_DIRECTORY)) continue;
        std::vector<std::wstring> files;
        CollectFiles(p, files);
        for (auto& f : files)
            if (f != p && matches(f)) out.push_back(f);
    }
    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}

static void FindLockersRM(const std::vector<std::wstring>& files, const std::set<DWORD>& excluded,
                          std::map<DWORD, Locker>& result) {
    DWORD session = 0;
    WCHAR key[CCH_RM_SESSION_KEY + 1] = {};
    if (RmStartSession(&session, 0, key) != ERROR_SUCCESS) return;

    for (size_t i = 0; i < files.size(); i += 500) {
        size_t end = std::min(files.size(), i + 500);
        std::vector<LPCWSTR> batch;
        for (size_t j = i; j < end; j++) batch.push_back(files[j].c_str());
        RmRegisterResources(session, (UINT)batch.size(), batch.data(), 0, nullptr, 0, nullptr);
    }

    UINT needed = 0, count = 0;
    DWORD reason = 0;
    DWORD r = RmGetList(session, &needed, &count, nullptr, &reason);
    if (r == ERROR_MORE_DATA && needed > 0) {
        std::vector<RM_PROCESS_INFO> info(needed);
        count = needed;
        if (RmGetList(session, &needed, &count, info.data(), &reason) == ERROR_SUCCESS) {
            for (UINT i = 0; i < count; i++) {
                DWORD pid = info[i].Process.dwProcessId;
                if (pid <= 4 || excluded.count(pid)) continue;
                bool canKill = false;
                HANDLE h = OpenLockerProcess(pid, 0, canKill);
                if (h) {
                    FILETIME c, e, k, u;
                    if (!GetProcessTimes(h, &c, &e, &k, &u) ||
                        CompareFileTime(&c, &info[i].Process.ProcessStartTime) != 0) {
                        CloseHandle(h);
                        continue;
                    }
                }
                AddLocker(result, pid, info[i].strAppName, h, canKill);
            }
        }
    }
    RmEndSession(session);
}

typedef LONG (NTAPI *NtQueryInformationProcess_t)(HANDLE, ULONG, PVOID, ULONG, PULONG);
typedef LONG (NTAPI *NtWow64ReadVirtualMemory64_t)(HANDLE, ULONG64, PVOID, ULONG64, PULONG64);

struct ProcBasicInfo64 {
    ULONG64 ExitStatus;
    ULONG64 PebBaseAddress;
    ULONG64 AffinityMask;
    ULONG64 BasePriority;
    ULONG64 UniqueProcessId;
    ULONG64 InheritedFromUniqueProcessId;
};

struct UnicodeString64 {
    USHORT Length;
    USHORT MaximumLength;
    ULONG Pad;
    ULONG64 Buffer;
};

struct UnicodeString32 {
    USHORT Length;
    USHORT MaximumLength;
    ULONG Buffer;
};

static bool ReadCwd64(HANDLE h, std::wstring& cwd) {
    HMODULE nt = GetModuleHandleW(L"ntdll.dll");
    if (!nt) return false;
    ProcBasicInfo64 pbi = {};
#ifdef _WIN64
    auto query = (NtQueryInformationProcess_t)GetProcAddress(nt, "NtQueryInformationProcess");
    auto read = [&](ULONG64 a, void* d, SIZE_T n) {
        return ReadProcessMemory(h, (LPCVOID)a, d, n, nullptr) != 0;
    };
#else
    auto query = (NtQueryInformationProcess_t)GetProcAddress(nt, "NtWow64QueryInformationProcess64");
    auto readFn = (NtWow64ReadVirtualMemory64_t)GetProcAddress(nt, "NtWow64ReadVirtualMemory64");
    if (!readFn) return false;
    auto read = [&](ULONG64 a, void* d, SIZE_T n) {
        return readFn(h, a, d, (ULONG64)n, nullptr) >= 0;
    };
#endif
    if (!query) return false;
    if (query(h, 0, &pbi, sizeof(pbi), nullptr) < 0 || !pbi.PebBaseAddress) return false;

    ULONG64 params = 0;
    if (!read(pbi.PebBaseAddress + 0x20, &params, sizeof(params)) || !params) return false;

    UnicodeString64 us = {};
    if (!read(params + 0x38, &us, sizeof(us)) || !us.Buffer || !us.Length) return false;

    std::wstring s(us.Length / sizeof(WCHAR), L'\0');
    if (!read(us.Buffer, &s[0], us.Length)) return false;
    cwd = s;
    return true;
}

static bool ReadCwd32(HANDLE h, std::wstring& cwd) {
    HMODULE nt = GetModuleHandleW(L"ntdll.dll");
    if (!nt) return false;
    auto query = (NtQueryInformationProcess_t)GetProcAddress(nt, "NtQueryInformationProcess");
    if (!query) return false;

    ULONG64 peb = 0;
#ifdef _WIN64
    ULONG_PTR p = 0;
    if (query(h, 26, &p, sizeof(p), nullptr) < 0 || !p) return false;
    peb = p;
#else
    struct { PVOID r1; PVOID peb; PVOID r2[2]; ULONG_PTR pid; PVOID r3; } pbi = {};
    if (query(h, 0, &pbi, sizeof(pbi), nullptr) < 0 || !pbi.peb) return false;
    peb = (ULONG_PTR)pbi.peb;
#endif

    ULONG params = 0;
    if (!ReadProcessMemory(h, (LPCVOID)(ULONG_PTR)(peb + 0x10), &params, sizeof(params), nullptr) || !params)
        return false;

    UnicodeString32 us = {};
    if (!ReadProcessMemory(h, (LPCVOID)(ULONG_PTR)(params + 0x24), &us, sizeof(us), nullptr) || !us.Buffer || !us.Length)
        return false;

    std::wstring s(us.Length / sizeof(WCHAR), L'\0');
    if (!ReadProcessMemory(h, (LPCVOID)(ULONG_PTR)us.Buffer, &s[0], us.Length, nullptr)) return false;
    cwd = s;
    return true;
}

static bool GetProcessCwd(HANDLE h, std::wstring& cwd) {
    BOOL selfWow = FALSE, targetWow = FALSE;
    IsWow64Process(GetCurrentProcess(), &selfWow);
    IsWow64Process(h, &targetWow);
#ifdef _WIN64
    bool target64 = !targetWow;
#else
    bool target64 = selfWow && !targetWow;
#endif
    return target64 ? ReadCwd64(h, cwd) : ReadCwd32(h, cwd);
}

static void FindLockersCwd(const std::wstring& root, const std::set<DWORD>& excluded,
                           std::map<DWORD, Locker>& result) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32W pe = {sizeof(pe)};
    if (Process32FirstW(snap, &pe)) {
        do {
            if (g_stop) break;
            DWORD pid = pe.th32ProcessID;
            if (pid <= 4 || excluded.count(pid)) continue;
            bool canKill = false;
            HANDLE h = OpenLockerProcess(pid, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, canKill);
            if (!h) continue;
            std::wstring cwd;
            if (GetProcessCwd(h, cwd) && PathStartsWith(cwd, root))
                AddLocker(result, pid, pe.szExeFile, h, canKill);
            else
                CloseHandle(h);
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
}

typedef LONG (NTAPI *NtQuerySystemInformation_t)(ULONG, PVOID, ULONG, PULONG);

struct HandleEntryEx {
    PVOID Object;
    ULONG_PTR Pid;
    ULONG_PTR Handle;
    ULONG Access;
    USHORT BackTrace;
    USHORT Type;
    ULONG Attr;
    ULONG Reserved;
};

struct ScanJob {
    SRWLOCK lock = SRWLOCK_INIT;
    std::vector<std::wstring> roots;
    std::set<DWORD> excluded;
    std::map<DWORD, Locker> found;
    size_t fileHandles = 0;
    size_t inaccessibleProcs = 0;
    bool finished = false;
    bool abandoned = false;
    std::atomic<bool> cancel{false};
    HANDLE done = nullptr;
    ~ScanJob() {
        CloseLockers(found);
        if (done) CloseHandle(done);
    }
};

static void HandleScanMain(ScanJob& job) {
    auto NtQSI = (NtQuerySystemInformation_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQuerySystemInformation");
    if (!NtQSI) return;

    DWORD myPid = GetCurrentProcessId();
    auto names = ProcessNames();

    WCHAR exe[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    HANDLE self = CreateFileW(exe, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, 0, nullptr);

    std::vector<BYTE> buf;
    ULONG size = 1 << 20;
    bool ok = false;
    for (int i = 0; i < 12; i++) {
        buf.resize(size);
        ULONG ret = 0;
        LONG st = NtQSI(64, buf.data(), size, &ret);
        if (st == (LONG)0xC0000004) { size = std::max<ULONG>(size * 2, ret + (1 << 16)); continue; }
        ok = st >= 0;
        break;
    }
    if (!ok || self == INVALID_HANDLE_VALUE) {
        if (self != INVALID_HANDLE_VALUE) CloseHandle(self);
        return;
    }

    ULONG_PTR n = *(ULONG_PTR*)buf.data();
    auto* entries = (HandleEntryEx*)(buf.data() + 2 * sizeof(ULONG_PTR));

    int fileType = -1;
    for (ULONG_PTR i = 0; i < n; i++) {
        if (entries[i].Pid == myPid && entries[i].Handle == (ULONG_PTR)self) { fileType = entries[i].Type; break; }
    }
    CloseHandle(self);
    if (fileType < 0) return;

    struct Proc { HANDLE h; bool canKill; };
    std::map<DWORD, Proc> procs;
    std::set<DWORD> bad;

    for (ULONG_PTR i = 0; i < n; i++) {
        if (job.cancel) break;
        const HandleEntryEx& e = entries[i];
        if (e.Type != fileType) continue;
        DWORD pid = (DWORD)e.Pid;
        if (pid <= 4 || job.excluded.count(pid)) continue;
        job.fileHandles++;

        if (e.Access == 0x0012019f || e.Access == 0x001a019f || e.Access == 0x00120189 || e.Access == 0x00100000)
            continue;
        if (job.found.count(pid)) continue;

        HANDLE hp = nullptr;
        bool canKill = false;
        auto it = procs.find(pid);
        if (it != procs.end()) {
            hp = it->second.h;
            canKill = it->second.canKill;
        } else if (!bad.count(pid)) {
            hp = OpenLockerProcess(pid, PROCESS_DUP_HANDLE, canKill);
            if (hp) procs[pid] = {hp, canKill};
            else { bad.insert(pid); job.inaccessibleProcs++; }
        }
        if (!hp) continue;

        HANDLE dup = nullptr;
        if (!DuplicateHandle(hp, (HANDLE)e.Handle, GetCurrentProcess(), &dup, 0, FALSE, DUPLICATE_SAME_ACCESS) || !dup)
            continue;

        if (GetFileType(dup) == FILE_TYPE_DISK) {
            WCHAR path[1024] = {};
            DWORD len = GetFinalPathNameByHandleW(dup, path, 1024, VOLUME_NAME_DOS);
            if (len > 0 && len < 1024) {
                std::wstring p = path;
                if (p.rfind(L"\\\\?\\UNC\\", 0) == 0) p = L"\\\\" + p.substr(8);
                else if (p.rfind(L"\\\\?\\", 0) == 0) p = p.substr(4);
                for (auto& root : job.roots) {
                    if (PathStartsWith(p, root)) {
                        auto nit = names.find(pid);
                        Locker l;
                        l.name = nit != names.end() ? nit->second : L"(unknown)";
                        l.h = hp;
                        l.canKill = canKill;
                        job.found[pid] = l;
                        procs.erase(pid);
                        break;
                    }
                }
            }
        }
        CloseHandle(dup);
    }

    for (auto& kv : procs) CloseHandle(kv.second.h);
}

static DWORD WINAPI HandleScanThread(LPVOID param) {
    std::shared_ptr<ScanJob> job = *(std::shared_ptr<ScanJob>*)param;
    delete (std::shared_ptr<ScanJob>*)param;

    HandleScanMain(*job);

    {
        SrwLock lock(&job->lock);
        job->finished = true;
        if (job->abandoned) CloseLockers(job->found);
    }
    SetEvent(job->done);
    return 0;
}

static void FindLockersHandles(const std::vector<std::wstring>& roots, const std::set<DWORD>& excluded,
                               std::map<DWORD, Locker>& result, size_t& fileHandles,
                               size_t& inaccessible, bool& timedOut) {
    if (g_stop) return;
    auto job = std::make_shared<ScanJob>();
    job->roots = roots;
    job->excluded = excluded;
    job->done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!job->done) return;

    auto* arg = new std::shared_ptr<ScanJob>(job);
    DWORD tid = 0;
    HANDLE th = CreateThread(nullptr, 0, HandleScanThread, arg, 0, &tid);
    if (!th) { delete arg; return; }
    {
        SrwLock lock(&g_threadsLock);
        g_threads.push_back({th, tid, true});
    }

    HANDLE waits[2] = {job->done, g_stopEvent};
    WaitForMultipleObjects(2, waits, FALSE, 12000);

    SrwLock lock(&job->lock);
    if (job->finished) {
        for (auto& kv : job->found) result[kv.first] = kv.second;
        job->found.clear();
        fileHandles = job->fileHandles;
        inaccessible = job->inaccessibleProcs;
        timedOut = false;
    } else {
        job->cancel = true;
        job->abandoned = true;
        timedOut = true;
    }
}

struct ExpWin { std::wstring path; IWebBrowser2* wb; };

static void AddFromDispatch(IDispatch* disp, std::vector<std::wstring>& sel, std::vector<ExpWin>& wins) {
    Com<IWebBrowser2> wb;
    if (FAILED(disp->QueryInterface(IID_PPV_ARGS(&wb)))) return;

    BSTR url = nullptr;
    if (SUCCEEDED(wb->get_LocationURL(&url)) && url) {
        WCHAR path[MAX_PATH * 2];
        DWORD len = ARRAYSIZE(path);
        if (SUCCEEDED(PathCreateFromUrlW(url, path, &len, 0))) {
            wb->AddRef();
            wins.push_back({path, wb.p});
        }
        SysFreeString(url);
    }

    Com<IDispatch> doc;
    if (FAILED(wb->get_Document(&doc)) || !doc) return;
    Com<IShellFolderViewDual> fv;
    if (FAILED(doc->QueryInterface(IID_PPV_ARGS(&fv)))) return;
    Com<FolderItems> items;
    if (FAILED(fv->SelectedItems(&items)) || !items) return;
    long n = 0;
    items->get_Count(&n);
    for (long i = 0; i < n; i++) {
        VARIANT v; VariantInit(&v); v.vt = VT_I4; v.lVal = i;
        Com<FolderItem> it;
        if (FAILED(items->Item(v, &it)) || !it) continue;
        BSTR p = nullptr;
        if (SUCCEEDED(it->get_Path(&p)) && p) {
            sel.push_back(p);
            SysFreeString(p);
        }
    }
}

static void GetExplorerState(std::vector<std::wstring>& sel, std::vector<ExpWin>& wins) {
    Com<IShellWindows> psw;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&psw)))) return;

    long count = 0;
    psw->get_Count(&count);
    for (long i = 0; i < count; i++) {
        VARIANT v; VariantInit(&v); v.vt = VT_I4; v.lVal = i;
        Com<IDispatch> disp;
        if (SUCCEEDED(psw->Item(v, &disp)) && disp) AddFromDispatch(disp.p, sel, wins);
    }
    VARIANT vEmpty; VariantInit(&vEmpty);
    long hwnd = 0;
    Com<IDispatch> deskDisp;
    if (SUCCEEDED(psw->FindWindowSW(&vEmpty, &vEmpty, SWC_DESKTOP, &hwnd, SWFO_NEEDDISPATCH, &deskDisp)) && deskDisp)
        AddFromDispatch(deskDisp.p, sel, wins);

    std::sort(sel.begin(), sel.end());
    sel.erase(std::unique(sel.begin(), sel.end()), sel.end());
}

static std::wstring JoinNames(const std::map<DWORD, Locker>& m) {
    std::set<std::wstring> uniq;
    for (auto& kv : m) uniq.insert(kv.second.name);
    std::wstring s;
    for (auto& n : uniq) {
        if (!s.empty()) s += L", ";
        s += n;
    }
    return s;
}

static DWORD WorkerMain(LPVOID param) {
    HWND dlg = (HWND)param;
    CoInit com(COINIT_APARTMENTTHREADED);
    Wh_Log(L"Worker started");

    std::set<DWORD> excluded = BuildExcluded();

    std::vector<std::wstring> texts;
    {
        Com<IUIAutomation> ua;
        if (CreateUia(&ua)) CollectDialogTexts(ua.p, dlg, texts);
    }
    for (size_t i = 0; i < texts.size() && i < 20; i++)
        Wh_Log(L"Dialog text: %s", texts[i].c_str());

    std::vector<std::wstring> sel;
    std::vector<ExpWin> wins;
    GetExplorerState(sel, wins);
    auto releaseAll = [&]() { for (auto& w : wins) w.wb->Release(); wins.clear(); };
    Wh_Log(L"Explorer state: %d selected, %d windows", (int)sel.size(), (int)wins.size());

    std::vector<std::wstring> targets = ResolveTargets(sel, texts);
    Wh_Log(L"Matched targets: %d", (int)targets.size());

    if (g_stop) {
        releaseAll();
        return 0;
    }

    if (targets.empty()) {
        std::wstring content = sel.empty()
            ? L"Nothing is selected in Explorer, so the blocked item couldn't be identified."
            : L"None of the selected items matches the item named in the warning.";
        std::wstring details;
        details += L"Selected items: " + std::to_wstring(sel.size()) + L"\n";
        for (size_t i = 0; i < sel.size() && i < 5; i++) details += L"    " + sel[i] + L"\n";
        details += L"Texts read from the warning: " + std::to_wstring(texts.size());
        for (size_t i = 0; i < texts.size() && i < 6; i++) details += L"\n    " + texts[i];
        ShowInfo(L"Couldn't identify the blocked item", content, details, false);
        releaseAll();
        Wh_Log(L"Worker finished (no matching target)");
        return 0;
    }

    std::map<DWORD, Locker> lockers;
    struct LockerGuard {
        std::map<DWORD, Locker>& m;
        ~LockerGuard() { CloseLockers(m); }
    } guard{lockers};

    size_t scanned = 0;
    for (auto& p : targets) {
        if (g_stop) break;
        std::vector<std::wstring> files;
        CollectFiles(p, files);
        scanned += files.size();
        FindLockersRM(files, excluded, lockers);
        FindLockersCwd(p, excluded, lockers);
    }

    size_t fileHandles = 0, inaccessible = 0;
    bool timedOut = false;
    FindLockersHandles(targets, excluded, lockers, fileHandles, inaccessible, timedOut);
    Wh_Log(L"Lockers found: %d", (int)lockers.size());

    if (g_stop) {
        releaseAll();
        return 0;
    }

    std::vector<std::pair<IWebBrowser2*, std::wstring>> inside;
    std::vector<std::wstring> insidePaths;
    for (auto& w : wins) {
        for (auto& p : targets) {
            if (PathStartsWith(w.path, p)) {
                std::wstring parent = p;
                PathRemoveFileSpecW(&parent[0]);
                parent.resize(wcslen(parent.c_str()));
                inside.push_back({w.wb, parent});
                insidePaths.push_back(w.path);
                break;
            }
        }
    }

    if (lockers.empty() && inside.empty()) {
        if (Wh_GetIntSetting(L"showNoResultDialog")) {
            std::wstring content;
            if (inaccessible > 0)
                content = L"It is most likely held by an elevated or protected process.";
            else
                content = L"It may be held by Explorer itself (preview pane, thumbnails) or by the system.";

            std::wstring details;
            details += L"Blocked items matched: " + std::to_wstring(targets.size()) + L"\n";
            for (size_t i = 0; i < targets.size() && i < 5; i++) details += L"    " + targets[i] + L"\n";
            details += L"Files scanned: " + std::to_wstring(scanned) + L"\n";
            details += L"Explorer windows: " + std::to_wstring(wins.size()) + L"\n";
            details += L"Handles checked: " + std::to_wstring(fileHandles) + L"\n";
            details += L"Inaccessible processes: " + std::to_wstring(inaccessible);
            if (timedOut) details += L"\nThe handle scan timed out and was cancelled.";

            ShowInfo(L"Couldn't find what's blocking this item", content, details, false);
        }
        releaseAll();
        Wh_Log(L"Worker finished (nothing found)");
        return 0;
    }

    std::wstring content;
    if (!lockers.empty()) {
        content = L"In use by: " + JoinNames(lockers);
        content += L"\n\nEnding a process terminates it immediately. Unsaved work in it will be lost.";
    }
    if (!inside.empty()) {
        if (!content.empty()) content += L"\n\n";
        content += L"Explorer windows open inside will move up one level.";
    }

    std::wstring details;
    details += L"Matched items (" + std::to_wstring(targets.size()) + L"):\n";
    for (size_t i = 0; i < targets.size() && i < 10; i++)
        details += L"    " + targets[i] + L"\n";
    if (targets.size() > 10)
        details += L"    ... and " + std::to_wstring(targets.size() - 10) + L" more\n";
    details += L"\n";
    if (!lockers.empty()) {
        details += L"Processes:\n";
        for (auto& kv : lockers) {
            details += L"    " + kv.second.name + L"  (PID " + std::to_wstring(kv.first) + L")";
            if (!kv.second.h || !kv.second.canKill) details += L"  - needs administrator rights";
            details += L"\n";
        }
    }
    if (!insidePaths.empty()) {
        details += L"Windows:\n";
        for (auto& p : insidePaths) details += L"    " + p + L"\n";
    }
    while (!details.empty() && details.back() == L'\n') details.pop_back();

    std::wstring primary, secondary;
    if (lockers.empty()) {
        primary = L"Move windows and continue\nCompletes the operation";
    } else {
        primary = L"End processes and continue\nTerminates the processes and completes the operation";
        secondary = L"Only end processes\nYou retry it yourself";
    }

    Choice choice = ShowChoice(L"Item in use", content, details, primary, secondary);
    if (g_stop) choice = CHOICE_CANCEL;
    Wh_Log(L"User choice: %d", (int)choice);

    if (choice != CHOICE_CANCEL) {
        std::wstring failed;
        for (auto& kv : lockers)
            if (!KillLocker(kv.second))
                failed += L"    " + kv.second.name + L"  (PID " + std::to_wstring(kv.first) + L")\n";

        for (auto& w : inside) {
            VARIANT target; VariantInit(&target);
            target.vt = VT_BSTR;
            target.bstrVal = SysAllocString(w.second.c_str());
            VARIANT e; VariantInit(&e);
            w.first->Navigate2(&target, &e, &e, &e, &e);
            VariantClear(&target);
        }

        if (!failed.empty()) {
            while (!failed.empty() && failed.back() == L'\n') failed.pop_back();
            ShowInfo(L"Some processes couldn't be ended",
                     L"They probably need administrator rights.", failed, true);
        } else if (choice == CHOICE_CLOSE_CONTINUE) {
            if (WaitForSingleObject(g_stopEvent, 600) != WAIT_OBJECT_0)
                PressRetry(dlg);
        }
    }
    releaseAll();
    Wh_Log(L"Worker finished");
    return 0;
}

static DWORD WINAPI WorkerThread(LPVOID param) {
    return WorkerMain(param);
}

static BOOL CALLBACK CollectCandidate(HWND h, LPARAM) {
    if (!IsWindowVisible(h)) return TRUE;

    WCHAR cls[64] = {};
    GetClassNameW(h, cls, 64);
    if (wcscmp(cls, kDialogClass)) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid || !IsExplorerExe(pid, false)) return TRUE;
    if (IsHungAppWindow(h)) return TRUE;

    g_candidates.push_back(h);
    return TRUE;
}

static void ReapThreads() {
    SrwLock lock(&g_threadsLock);
    for (auto it = g_threads.begin(); it != g_threads.end();) {
        if (WaitForSingleObject(it->h, 0) == WAIT_OBJECT_0) {
            CloseHandle(it->h);
            it = g_threads.erase(it);
        } else {
            ++it;
        }
    }
}

static DWORD WINAPI MonitorThread(LPVOID) {
    CoInit com(COINIT_MULTITHREADED);

    Com<IUIAutomation> ua;
    if (!CreateUia(&ua)) {
        Wh_Log(L"UI Automation is not available");
        return 0;
    }

    while (!g_stop) {
        g_candidates.clear();
        EnumWindows(CollectCandidate, 0);

        std::set<HWND> present(g_candidates.begin(), g_candidates.end());
        for (auto it = g_seen.begin(); it != g_seen.end();)
            it = present.count(it->first) ? std::next(it) : g_seen.erase(it);

        for (auto it = g_handled.begin(); it != g_handled.end();)
            it = InterruptActive(ua.p, *it) ? std::next(it) : g_handled.erase(it);

        for (HWND h : g_candidates) {
            if (g_stop) break;
            if (g_handled.count(h)) continue;

            WCHAR title[128] = {};
            if (!SafeWindowText(h, title, 128)) continue;

            auto seen = g_seen.find(h);
            if (seen != g_seen.end() && seen->second == title) continue;
            g_seen[h] = title;

            if (!InterruptActive(ua.p, h)) continue;

            WCHAR cls[128] = {};
            GetClassNameW(h, cls, 128);
            Wh_Log(L"Interrupt dialog detected: class=%s title=%s", cls, title);
            g_handled.insert(h);

            DWORD tid = 0;
            HANDLE th = CreateThread(nullptr, 0, WorkerThread, h, 0, &tid);
            if (th) {
                SrwLock lock(&g_threadsLock);
                g_threads.push_back({th, tid, false});
            }
        }

        ReapThreads();
        if (WaitForSingleObject(g_stopEvent, 250) == WAIT_OBJECT_0) break;
    }
    return 0;
}

static BOOL CALLBACK CancelWindowProc(HWND h, LPARAM) {
    WCHAR cls[32] = {};
    GetClassNameW(h, cls, 32);
    if (wcscmp(cls, L"#32770") != 0) return TRUE;
    PostMessageW(h, TDM_CLICK_BUTTON, IDCANCEL, 0);
    PostMessageW(h, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), 0);
    PostMessageW(h, WM_COMMAND, MAKEWPARAM(IDNO, BN_CLICKED), 0);
    PostMessageW(h, WM_CLOSE, 0, 0);
    return TRUE;
}

static void WaitWorkers() {
    std::vector<ThreadInfo> workers;
    {
        SrwLock lock(&g_threadsLock);
        for (auto& t : g_threads)
            if (!t.isScan) workers.push_back(t);
    }
    DWORD start = GetTickCount();
    for (auto& t : workers) {
        while (WaitForSingleObject(t.h, 100) != WAIT_OBJECT_0) {
            EnumThreadWindows(t.tid, CancelWindowProc, 0);
            if (GetTickCount() - start > 10000) break;
        }
    }
}

static void CloseAllThreads() {
    std::vector<ThreadInfo> all;
    {
        SrwLock lock(&g_threadsLock);
        all = g_threads;
        g_threads.clear();
    }
    for (auto& t : all)
        if (t.isScan) WaitForSingleObject(t.h, 3000);
    for (auto& t : all) CloseHandle(t.h);
}

BOOL WhTool_ModInit() {
    g_stop = false;
    g_handled.clear();
    g_seen.clear();

    WCHAR win[MAX_PATH] = {};
    GetWindowsDirectoryW(win, MAX_PATH);
    g_explorerPath = std::wstring(win) + L"\\explorer.exe";

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return FALSE;

    g_monitor = CreateThread(nullptr, 0, MonitorThread, nullptr, 0, nullptr);
    return g_monitor != nullptr;
}

void WhTool_ModSettingsChanged() {
}

void WhTool_ModUninit() {
    g_stop = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_monitor) {
        WaitForSingleObject(g_monitor, INFINITE);
        CloseHandle(g_monitor);
        g_monitor = nullptr;
    }
    WaitWorkers();
    CloseAllThreads();
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
