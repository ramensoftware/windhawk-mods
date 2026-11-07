// ==WindhawkMod==
// @id              unlock-folder-in-use
// @name            Unlock "Folder In Use"
// @description     When Explorer says "Folder/File In Use", shows what is blocking it and lets you choose: close the processes and continue, or only close them
// @version         1.0.0
// @author          LyricsPortal
// @github          https://github.com/LyricsPortal
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -lrstrtmgr -luser32 -lshlwapi -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Unlock "Folder In Use"

When Explorer refuses to delete, move or rename something because it is in use ("Folder In Use" / "File In Use"), this mod finds out which processes are blocking it and lets you decide what to do.

When the warning appears, a small dialog lists the blocking processes and offers:

* **Close and continue** - closes the processes and presses *Try Again* for you, so Explorer completes the original operation.
* **Only close processes** - closes the processes and leaves Explorer's warning open so you can retry yourself.
* **Cancel** - does nothing.

If Explorer windows are open inside the folder being processed, they are moved up one level.

## How it works

* The warning is detected through its internal UI Automation identifiers instead of its text, so it works with any display language.
* Blockers are found with the Restart Manager, the current directory of each process, and the system handle table.

## Notes

* Closing a process terminates it immediately. Unsaved work in that process is lost.
* Elevated or protected processes can't be closed from Explorer.
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

static std::atomic<bool> g_stop{false};
static HANDLE g_monitor = nullptr;
static std::atomic<int> g_active{0};
static std::set<HWND> g_handled;
static std::map<HWND, std::wstring> g_seen;
static std::vector<HWND> g_candidates;

template <class T> struct Com {
    T* p = nullptr;
    ~Com() { if (p) p->Release(); }
    T** operator&() { return &p; }
    T* operator->() { return p; }
    operator bool() const { return p != nullptr; }
};

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
        Sleep(100);
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
    if (msg == TDN_CREATED)
        SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    return S_OK;
}

enum Choice { CHOICE_CANCEL = 0, CHOICE_CLOSE_CONTINUE = 1, CHOICE_CLOSE_ONLY = 2 };

static void FillCommon(TASKDIALOGCONFIG& cfg, HWND parent, const std::wstring& instruction,
                       const std::wstring& content, const std::wstring& details) {
    cfg.cbSize = sizeof(cfg);
    cfg.hwndParent = parent;
    cfg.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_POSITION_RELATIVE_TO_WINDOW;
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

static void ShowInfo(HWND parent, const std::wstring& instruction, const std::wstring& content,
                     const std::wstring& details, bool error) {
    if (GetTaskDialog() && Wh_GetIntSetting(L"modernDialogs")) {
        TASKDIALOGCONFIG cfg = {};
        FillCommon(cfg, parent, instruction, content, details);
        cfg.dwCommonButtons = TDCBF_OK_BUTTON;
        if (error) cfg.pszMainIcon = TD_ERROR_ICON;
        int pressed = 0;
        if (SUCCEEDED(GetTaskDialog()(&cfg, &pressed, nullptr, nullptr))) return;
    }
    std::wstring text = instruction;
    if (!content.empty()) text += L"\n\n" + content;
    if (!details.empty()) text += L"\n\n" + details;
    MessageBoxW(parent, text.c_str(), L"Unlock",
                MB_OK | MB_TOPMOST | MB_SETFOREGROUND | (error ? MB_ICONERROR : MB_ICONINFORMATION));
}

static Choice ShowChoice(HWND parent, const std::wstring& instruction, const std::wstring& content,
                         const std::wstring& details, const std::wstring& primary,
                         const std::wstring& secondary) {
    const int kPrimary = 1001, kSecondary = 1002;
    const bool hasSecondary = !secondary.empty();

    if (GetTaskDialog() && Wh_GetIntSetting(L"modernDialogs")) {
        TASKDIALOG_BUTTON btns[2] = {{kPrimary, primary.c_str()}, {kSecondary, secondary.c_str()}};
        TASKDIALOGCONFIG cfg = {};
        FillCommon(cfg, parent, instruction, content, details);
        cfg.dwFlags |= TDF_USE_COMMAND_LINKS_NO_ICON;
        cfg.dwCommonButtons = TDCBF_CANCEL_BUTTON;
        cfg.pButtons = btns;
        cfg.cButtons = hasSecondary ? 2 : 1;
        cfg.nDefaultButton = kPrimary;
        int pressed = 0;
        if (SUCCEEDED(GetTaskDialog()(&cfg, &pressed, nullptr, nullptr))) {
            if (pressed == kPrimary) return CHOICE_CLOSE_CONTINUE;
            if (pressed == kSecondary) return CHOICE_CLOSE_ONLY;
            return CHOICE_CANCEL;
        }
    }

    std::wstring text = instruction;
    if (!content.empty()) text += L"\n\n" + content;
    if (hasSecondary) {
        text += L"\n\nYes: close processes and continue\nNo: only close processes\nCancel: do nothing";
        int r = MessageBoxW(parent, text.c_str(), L"Unlock",
                            MB_YESNOCANCEL | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND);
        if (r == IDYES) return CHOICE_CLOSE_CONTINUE;
        if (r == IDNO) return CHOICE_CLOSE_ONLY;
        return CHOICE_CANCEL;
    }
    text += L"\n\nContinue?";
    int r = MessageBoxW(parent, text.c_str(), L"Unlock",
                        MB_YESNO | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND);
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

static void CollectFiles(const std::wstring& path, std::vector<std::wstring>& files, size_t limit = 20000) {
    DWORD attr = GetFileAttributesW(path.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) return;
    files.push_back(path);
    if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) return;

    std::vector<std::wstring> stack{path};
    while (!stack.empty() && files.size() < limit) {
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
        } while (FindNextFileW(h, &fd) && files.size() < limit);
        FindClose(h);
    }
}

static void FindLockersRM(const std::vector<std::wstring>& files, std::map<DWORD, std::wstring>& result) {
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
            for (UINT i = 0; i < count; i++)
                result[info[i].Process.dwProcessId] = info[i].strAppName;
        }
    }
    RmEndSession(session);
}

typedef LONG (NTAPI *NtQueryInformationProcess_t)(HANDLE, ULONG, PVOID, ULONG, PULONG);

static bool GetProcessCwd(HANDLE hProc, std::wstring& cwd) {
#ifdef _WIN64
    static auto NtQIP = (NtQueryInformationProcess_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationProcess");
    if (!NtQIP) return false;
    BOOL wow = FALSE;
    IsWow64Process(hProc, &wow);
    if (wow) return false;

    struct { PVOID r1; PVOID peb; PVOID r2[2]; ULONG_PTR pid; PVOID r3; } pbi = {};
    if (NtQIP(hProc, 0, &pbi, sizeof(pbi), nullptr) != 0 || !pbi.peb) return false;

    PVOID params = nullptr;
    if (!ReadProcessMemory(hProc, (BYTE*)pbi.peb + 0x20, &params, sizeof(params), nullptr) || !params) return false;

    struct { USHORT len; USHORT max; PWSTR buf; } us = {};
    if (!ReadProcessMemory(hProc, (BYTE*)params + 0x38, &us, sizeof(us), nullptr) || !us.buf || !us.len) return false;

    std::wstring s(us.len / sizeof(WCHAR), L'\0');
    if (!ReadProcessMemory(hProc, us.buf, &s[0], us.len, nullptr)) return false;
    cwd = s;
    return true;
#else
    return false;
#endif
}

static void FindLockersCwd(const std::wstring& root, std::map<DWORD, std::wstring>& result) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32W pe = {sizeof(pe)};
    if (Process32FirstW(snap, &pe)) {
        do {
            if (pe.th32ProcessID <= 4) continue;
            HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
            if (!h) continue;
            std::wstring cwd;
            if (GetProcessCwd(h, cwd) && PathStartsWith(cwd, root))
                result[pe.th32ProcessID] = pe.szExeFile;
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
    std::vector<std::wstring> roots;
    std::map<DWORD, std::wstring> found;
    size_t fileHandles = 0;
    size_t inaccessibleProcs = 0;
    HANDLE done = nullptr;
    ~ScanJob() { if (done) CloseHandle(done); }
};

static DWORD HandleScanMain(LPVOID param) {
    std::shared_ptr<ScanJob> job = *(std::shared_ptr<ScanJob>*)param;
    delete (std::shared_ptr<ScanJob>*)param;

    auto NtQSI = (NtQuerySystemInformation_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQuerySystemInformation");
    if (!NtQSI) { SetEvent(job->done); return 0; }

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
        SetEvent(job->done);
        return 0;
    }

    ULONG_PTR n = *(ULONG_PTR*)buf.data();
    auto* entries = (HandleEntryEx*)(buf.data() + 2 * sizeof(ULONG_PTR));

    int fileType = -1;
    for (ULONG_PTR i = 0; i < n; i++) {
        if (entries[i].Pid == myPid && entries[i].Handle == (ULONG_PTR)self) { fileType = entries[i].Type; break; }
    }
    CloseHandle(self);
    if (fileType < 0) { SetEvent(job->done); return 0; }

    std::map<DWORD, HANDLE> procs;
    std::set<DWORD> bad;

    for (ULONG_PTR i = 0; i < n; i++) {
        const HandleEntryEx& e = entries[i];
        if (e.Type != fileType) continue;
        DWORD pid = (DWORD)e.Pid;
        if (pid == myPid || pid <= 4) continue;
        job->fileHandles++;

        if (e.Access == 0x0012019f || e.Access == 0x001a019f || e.Access == 0x00120189 || e.Access == 0x00100000)
            continue;
        if (job->found.count(pid)) continue;

        HANDLE hp = nullptr;
        auto it = procs.find(pid);
        if (it != procs.end()) hp = it->second;
        else if (!bad.count(pid)) {
            hp = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pid);
            if (hp) procs[pid] = hp; else { bad.insert(pid); job->inaccessibleProcs++; }
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
                for (auto& root : job->roots) {
                    if (PathStartsWith(p, root)) {
                        auto nit = names.find(pid);
                        job->found[pid] = nit != names.end() ? nit->second : L"(unknown)";
                        break;
                    }
                }
            }
        }
        CloseHandle(dup);
    }

    for (auto& kv : procs) CloseHandle(kv.second);
    SetEvent(job->done);
    return 0;
}

static DWORD WINAPI HandleScanThread(LPVOID param) {
    DWORD r = HandleScanMain(param);
    g_active--;
    return r;
}

static void FindLockersHandles(const std::vector<std::wstring>& roots, std::map<DWORD, std::wstring>& result,
                               size_t& fileHandles, size_t& inaccessible, bool& timedOut) {
    auto job = std::make_shared<ScanJob>();
    job->roots = roots;
    job->done = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    auto* arg = new std::shared_ptr<ScanJob>(job);
    g_active++;
    HANDLE th = CreateThread(nullptr, 0, HandleScanThread, arg, 0, nullptr);
    if (!th) { g_active--; delete arg; return; }
    CloseHandle(th);

    timedOut = WaitForSingleObject(job->done, 12000) != WAIT_OBJECT_0;
    if (!timedOut) {
        for (auto& kv : job->found) result[kv.first] = kv.second;
        fileHandles = job->fileHandles;
        inaccessible = job->inaccessibleProcs;
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

static bool KillPid(DWORD pid) {
    HANDLE h = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pid);
    if (!h) return false;
    bool ok = TerminateProcess(h, 1) != 0;
    if (ok) WaitForSingleObject(h, 3000);
    CloseHandle(h);
    return ok;
}

static std::wstring JoinNames(const std::map<DWORD, std::wstring>& m) {
    std::set<std::wstring> uniq;
    for (auto& kv : m) uniq.insert(kv.second);
    std::wstring s;
    for (auto& n : uniq) {
        if (!s.empty()) s += L", ";
        s += n;
    }
    return s;
}

static DWORD WorkerMain(LPVOID param) {
    HWND dlg = (HWND)param;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    Wh_Log(L"Worker started");

    std::vector<std::wstring> sel;
    std::vector<ExpWin> wins;
    GetExplorerState(sel, wins);
    Wh_Log(L"Explorer state: %d selected, %d windows", (int)sel.size(), (int)wins.size());

    std::map<DWORD, std::wstring> lockers;
    size_t scanned = 0;
    for (auto& p : sel) {
        std::vector<std::wstring> files;
        CollectFiles(p, files);
        scanned += files.size();
        FindLockersRM(files, lockers);
        FindLockersCwd(p, lockers);
    }

    size_t fileHandles = 0, inaccessible = 0;
    bool timedOut = false;
    if (!sel.empty()) FindLockersHandles(sel, lockers, fileHandles, inaccessible, timedOut);

    lockers.erase(GetCurrentProcessId());
    for (auto it = lockers.begin(); it != lockers.end();) {
        if (it->first <= 4) it = lockers.erase(it); else ++it;
    }
    Wh_Log(L"Lockers found: %d", (int)lockers.size());

    std::vector<std::pair<IWebBrowser2*, std::wstring>> inside;
    std::vector<std::wstring> insidePaths;
    for (auto& w : wins) {
        for (auto& p : sel) {
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

    auto releaseAll = [&]() { for (auto& w : wins) w.wb->Release(); };

    if (lockers.empty() && inside.empty()) {
        if (Wh_GetIntSetting(L"showNoResultDialog")) {
            std::wstring content;
            if (sel.empty())
                content = L"The selection couldn't be read. Select the item in Explorer and try again.";
            else if (inaccessible > 0)
                content = L"It is most likely held by an elevated or protected process.";
            else
                content = L"It may be held by Explorer itself (preview pane, thumbnails) or by the system.";

            std::wstring details;
            details += L"Selected items: " + std::to_wstring(sel.size()) + L"\n";
            for (size_t i = 0; i < sel.size() && i < 5; i++) details += L"    " + sel[i] + L"\n";
            details += L"Files scanned: " + std::to_wstring(scanned) + L"\n";
            details += L"Explorer windows: " + std::to_wstring(wins.size()) + L"\n";
            details += L"Handles checked: " + std::to_wstring(fileHandles) + L"\n";
            details += L"Inaccessible processes: " + std::to_wstring(inaccessible);
            if (timedOut) details += L"\nThe handle scan timed out and was cancelled.";

            ShowInfo(dlg, L"Couldn't find what's blocking this item", content, details, false);
        }
        releaseAll();
        CoUninitialize();
        Wh_Log(L"Worker finished (nothing found)");
        return 0;
    }

    std::wstring content;
    if (!lockers.empty()) content = L"In use by: " + JoinNames(lockers);
    if (!inside.empty()) {
        if (!content.empty()) content += L"\n\n";
        content += L"Explorer windows open inside will move up one level.";
    }

    std::wstring details;
    if (!lockers.empty()) {
        details += L"Processes:\n";
        for (auto& kv : lockers)
            details += L"    " + kv.second + L"  (PID " + std::to_wstring(kv.first) + L")\n";
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
        primary = L"Close and continue\nCloses the processes and completes the operation";
        secondary = L"Only close processes\nYou retry it yourself";
    }

    Choice choice = ShowChoice(dlg, L"Item in use", content, details, primary, secondary);
    Wh_Log(L"User choice: %d", (int)choice);

    if (choice != CHOICE_CANCEL) {
        std::wstring failed;
        for (auto& kv : lockers)
            if (!KillPid(kv.first))
                failed += L"    " + kv.second + L"  (PID " + std::to_wstring(kv.first) + L")\n";

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
            ShowInfo(dlg, L"Some processes couldn't be closed",
                     L"They probably need administrator rights.", failed, true);
        } else if (choice == CHOICE_CLOSE_CONTINUE) {
            Sleep(600);
            PressRetry(dlg);
        }
    }
    releaseAll();
    CoUninitialize();
    Wh_Log(L"Worker finished");
    return 0;
}

static DWORD WINAPI WorkerThread(LPVOID param) {
    DWORD r = WorkerMain(param);
    g_active--;
    return r;
}

static BOOL CALLBACK CollectCandidate(HWND h, LPARAM) {
    if (!IsWindowVisible(h)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    if (IsHungAppWindow(h)) return TRUE;

    WCHAR cls[64] = {};
    GetClassNameW(h, cls, 64);
    if (wcscmp(cls, kDialogClass)) return TRUE;

    g_candidates.push_back(h);
    return TRUE;
}

static DWORD WINAPI MonitorThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    Com<IUIAutomation> ua;
    if (!CreateUia(&ua)) {
        Wh_Log(L"UI Automation is not available");
        CoUninitialize();
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
            g_active++;
            HANDLE th = CreateThread(nullptr, 0, WorkerThread, h, 0, nullptr);
            if (th) CloseHandle(th); else g_active--;
        }
        Sleep(250);
    }

    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    g_stop = false;
    g_handled.clear();
    g_seen.clear();
    g_monitor = CreateThread(nullptr, 0, MonitorThread, nullptr, 0, nullptr);
    return g_monitor != nullptr;
}

void Wh_ModUninit() {
    g_stop = true;
    if (g_monitor) {
        WaitForSingleObject(g_monitor, 3000);
        CloseHandle(g_monitor);
        g_monitor = nullptr;
    }
    for (int i = 0; i < 100 && g_active > 0; i++) Sleep(100);
}
