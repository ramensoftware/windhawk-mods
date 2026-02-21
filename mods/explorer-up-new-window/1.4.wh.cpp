// ==WindhawkMod==
// @id              explorer-up-new-window
// @name            Explorer Up → New Window/Tab (Ctrl-click or Middle-click)
// @description     Ctrl or middle-click on explorer up-button opens parent in a new window or tab.
// @version         1.4
// @author          Tobias Lind
// @github          https://github.com/TobbeLino
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore -lshell32 -luser32 -luuid -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Up → New Window/Tab

Opens the parent folder in a **new** Explorer window or tab when you:
- **Middle-click** on the Up button
- **Ctrl + Left-click** on the Up button
- Press **Ctrl + Alt + Up** keyboard shortcut

Useful when you want to keep your current folder open while exploring the parent.


*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- openIn: window
  $name: Open parent folder in
  $description: Choose whether to open in a new window or a new tab
  $options:
  - window: New Window
  - tab: New Tab
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <UIAutomation.h>
#include <OleAuto.h>
#include <string>
#include <vector>
#include <atomic>

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

// ---------- Settings ----------
enum class OpenMode { Window, Tab };

struct {
    OpenMode openIn = OpenMode::Window;
} g_settings;

void LoadSettings() {
    PCWSTR val = Wh_GetStringSetting(L"openIn");
    if (val && _wcsicmp(val, L"tab") == 0) {
        g_settings.openIn = OpenMode::Tab;
    } else {
        g_settings.openIn = OpenMode::Window;
    }
    Wh_FreeStringSetting(val);
}

// ---------- Globals ----------
static std::atomic_bool g_inProgress{false};

static thread_local bool t_comInit = false;
static thread_local IUIAutomation* t_uia = nullptr;
static thread_local IUIAutomationTreeWalker* t_walker = nullptr;

static HHOOK g_mouseHook = nullptr;
static HHOOK g_kbdHook   = nullptr;

// Hook thread (LL hooks require a message pump)
static volatile HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;

enum class SuppressBtn : int { None = 0, Left = 1, Middle = 2, Key = 3 };
SuppressBtn g_suppress = SuppressBtn::None;

// Timing constants
const int maxKeyWaitTimeMs = 5000;
const int kMouseDebounceMs = 200;  // Ignore clicks too close together
const int kNewWindowWaitMs = 700;  // Time to wait for new Explorer window
const int kFocusDelayMs = 120;     // Delay before sending keyboard commands

std::atomic<ULONGLONG> lastMouseUpEventTime{0};

static ULONGLONG g_midCooldownUntil = 0;
static bool g_midCooldownSwallowNextUp = false;
static constexpr ULONGLONG kMidCooldownMs = 1000; // ~1s cooldown

// ---------- Helpers ----------
static inline bool IsInProgress() {
    return g_inProgress.load(std::memory_order_acquire);
}

// Returns true iff we *successfully* took the latch (i.e., it was idle)
static inline bool SetInProgressIfIdle() {
    // exchange() sets true and returns the *previous* value
    // We succeed when previous was false.
    return !g_inProgress.exchange(true, std::memory_order_acq_rel);
}

static inline void ResetInProgress() {
    g_inProgress.store(false, std::memory_order_release);
}

static void CleanupThreadUia() {
    if (t_walker) {
        t_walker->Release();
        t_walker = nullptr;
    }
    if (t_uia) {
        t_uia->Release();
        t_uia = nullptr;
    }
    if (t_comInit) {
        CoUninitialize();
        t_comInit = false;
    }
}

static void EnsureUiaForThisThread() {
    if (!t_comInit) {
        // UIA client prefers MTA
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE) {
            t_comInit = true;
        }
    }
    if (!t_uia) {
        if (SUCCEEDED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                       IID_PPV_ARGS(&t_uia))) && t_uia) {
            t_uia->get_ControlViewWalker(&t_walker); // ignore failure, we check null later
        }
    }
}

static bool IsExplorerTop(HWND h) {
    if (!h) return false;
    HWND top = GetAncestor(h, GA_ROOT);
    if (!top) return false;
    wchar_t cls[64]{};
    if (!GetClassNameW(top, cls, (int)ARRAYSIZE(cls))) return false;
    return wcscmp(cls, L"CabinetWClass") == 0;
}

static void ExtractNameAutoId(IUIAutomationElement* el, std::wstring& name, std::wstring& autoId) {
    name.clear();
    autoId.clear();
    if (!el) return;
    VARIANT v; VariantInit(&v);
    if (SUCCEEDED(el->GetCurrentPropertyValue(UIA_NamePropertyId, &v)) && v.vt == VT_BSTR && v.bstrVal)
        name.assign(v.bstrVal, SysStringLen(v.bstrVal));
    VariantClear(&v);
    if (SUCCEEDED(el->GetCurrentPropertyValue(UIA_AutomationIdPropertyId, &v)) && v.vt == VT_BSTR && v.bstrVal)
        autoId.assign(v.bstrVal, SysStringLen(v.bstrVal));
    VariantClear(&v);
}

static bool StartsWithI(const std::wstring& s, const wchar_t* prefix) {
    size_t n = wcslen(prefix);
    if (s.size() < n) return false;
    return _wcsnicmp(s.c_str(), prefix, (unsigned)n) == 0;
}

static bool LooksLikeUp(const wchar_t* name, const wchar_t* autoId) {
    // Check AutomationId first (language-independent, case-insensitive)
    if (autoId && _wcsicmp(autoId, L"UpButton") == 0)
        return true;

    // Fallback: check localized Name property
    // "Up" prefix covers English ("Up"), Swedish ("Upp"), etc.
    if (name && StartsWithI(name, L"Up"))
        return true;

    return false;
}

static bool CheckElementAndAncestorsForUp(
    IUIAutomationTreeWalker* walker,
    IUIAutomationElement* el,
    std::wstring& hitName,
    std::wstring& hitAutoId
) {
    hitName.clear();
    hitAutoId.clear();
    if (!el || !walker) return false;
    IUIAutomationElement* cur = el; cur->AddRef();
    bool isUp = false;
    for (int i = 0; i < 6 && cur && !isUp; ++i) {
        std::wstring nm, id;
        ExtractNameAutoId(cur, nm, id);
        if (LooksLikeUp(nm.c_str(), id.c_str())) {
            hitName = nm; hitAutoId = id; isUp = true; break;
        }
        IUIAutomationElement* parent = nullptr;
        if (FAILED(walker->GetParentElement(cur, &parent)) || !parent) break;
        cur->Release(); cur = parent;
    }
    if (cur) cur->Release();
    return isUp;
}

static void ForceForeground(HWND h) {
    if (!IsWindow(h)) return;
    HWND fg = GetForegroundWindow();
    DWORD fgTid = fg ? GetWindowThreadProcessId(fg, nullptr) : 0;
    DWORD curTid = GetCurrentThreadId();
    if (fgTid) AttachThreadInput(fgTid, curTid, TRUE);
    ShowWindow(h, SW_SHOW);
    SetForegroundWindow(h);
    BringWindowToTop(h);
    SetActiveWindow(h);
    if (fgTid) AttachThreadInput(fgTid, curTid, FALSE);
}

struct FindNewWinCtx { HWND exclude; DWORD pid; HWND found; };
static BOOL CALLBACK EnumNewExplorerProc(HWND hwnd, LPARAM lp) {
    FindNewWinCtx* ctx = (FindNewWinCtx*)lp;
    if (!IsWindowVisible(hwnd) || hwnd == ctx->exclude) return TRUE;
    wchar_t cls[64]{};
    if (!GetClassNameW(hwnd, cls, (int)ARRAYSIZE(cls))) return TRUE;
    if (wcscmp(cls, L"CabinetWClass") != 0) return TRUE;
    DWORD pid = 0; GetWindowThreadProcessId(hwnd, &pid);
    if (pid != ctx->pid) return TRUE;
    ctx->found = hwnd; return FALSE;
}

static void SendKeysSequence(const INPUT* inputs, UINT count) {
    if (!count) return;
    SendInput(count, const_cast<INPUT*>(inputs), sizeof(INPUT));
}

static bool isKeyDown(int key) {
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

static bool awaitKeyRelease(int key) {
    DWORD t0 = GetTickCount();
    while (isKeyDown(key) && GetTickCount() - t0 < maxKeyWaitTimeMs) {
        // Wh_Log(L"Waiting for key (%d) release...", key);
        Sleep(30);
    }
    return !isKeyDown(key);
}

static void AltUp() {
    // Alt + Up
    // If Alt is already down, just 'Up'; else Alt+Up
    Wh_Log(L"AltUp");

    // If Ctrl is down: Wait for user to release it! (or else "Alt + up" will become "Ctrl + Alt + up" and fail)
    awaitKeyRelease(VK_CONTROL);

    bool altDown = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    INPUT inps[4]; ZeroMemory(inps, sizeof(inps));
    UINT idx = 0;
    if (!altDown) {
        inps[idx].type = INPUT_KEYBOARD; inps[idx].ki.wVk = VK_MENU; idx++;
    }
    inps[idx].type = INPUT_KEYBOARD; inps[idx].ki.wVk = VK_UP;   idx++;
    inps[idx].type = INPUT_KEYBOARD; inps[idx].ki.wVk = VK_UP;   inps[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    if (!altDown) {
        inps[idx].type = INPUT_KEYBOARD; inps[idx].ki.wVk = VK_MENU; inps[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    }
    SendKeysSequence(inps, idx);
}

static void CtrlN() {
    // Ctrl + N
    // If Ctrl is already down, just 'N'; else Ctrl+N
    Wh_Log(L"CtrlN");

    // If Alt is down: Wait for user to release it! (or else "Ctrl + N" will become "Ctrl + Alt + N" and fail)
    awaitKeyRelease(VK_MENU);

    bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    INPUT inps[4]; ZeroMemory(inps, sizeof(inps));
    UINT idx = 0;
    if (!ctrlDown) {
        inps[idx].type = INPUT_KEYBOARD; inps[idx].ki.wVk = VK_CONTROL; idx++;
    }
    inps[idx].type = INPUT_KEYBOARD; inps[idx].ki.wVk = 'N'; idx++;
    inps[idx].type = INPUT_KEYBOARD; inps[idx].ki.wVk = 'N'; inps[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    if (!ctrlDown) {
        inps[idx].type = INPUT_KEYBOARD; inps[idx].ki.wVk = VK_CONTROL; inps[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    }
    SendKeysSequence(inps, idx);
}

// ---------- Clipboard helpers for New Tab mode ----------
struct ClipboardFormatData {
    UINT format;
    HANDLE hData;
};

struct ClipboardBackup {
    std::vector<ClipboardFormatData> formats;
};

static ClipboardBackup BackupClipboard() {
    ClipboardBackup backup;
    if (!OpenClipboard(nullptr)) {
        return backup;
    }

    UINT format = 0;
    while ((format = EnumClipboardFormats(format)) != 0) {
        HANDLE hRaw = GetClipboardData(format);
        if (!hRaw) continue;

        SIZE_T size = GlobalSize(hRaw);
        if (size == 0) continue;

        HANDLE hCopy = GlobalAlloc(GMEM_MOVEABLE, size);
        if (!hCopy) continue;

        void* pSrc = GlobalLock(hRaw);
        void* pDst = GlobalLock(hCopy);
        if (pSrc && pDst) {
            memcpy(pDst, pSrc, size);
            GlobalUnlock(hCopy);
            GlobalUnlock(hRaw);
            backup.formats.push_back({ format, hCopy });
        } else {
            GlobalUnlock(hCopy);
            GlobalUnlock(hRaw);
            GlobalFree(hCopy);
        }
    }

    CloseClipboard();
    return backup;
}

static void RestoreClipboard(ClipboardBackup& backup) {
    if (backup.formats.empty()) return;

    if (OpenClipboard(nullptr)) {
        EmptyClipboard();
        for (auto& item : backup.formats) {
            SetClipboardData(item.format, item.hData);
            item.hData = nullptr;
        }
        CloseClipboard();
    } else {
        for (auto& item : backup.formats) {
            if (item.hData) {
                GlobalFree(item.hData);
                item.hData = nullptr;
            }
        }
    }
    backup.formats.clear();
}

static std::wstring ProbeAddressBarWithClipboard(HWND top) {
    std::wstring foundPath = L"";

    SetForegroundWindow(top);
    Sleep(50);

    INPUT inputs[8] = {};
    int idx = 0;

    // Alt+D (Focus & Select address bar)
    memset(inputs, 0, sizeof(inputs)); idx = 0;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    Sleep(80);

    // Ctrl+C (Copy)
    memset(inputs, 0, sizeof(inputs)); idx = 0;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'C'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'C'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    Sleep(80);

    // Esc (Restore view)
    memset(inputs, 0, sizeof(inputs)); idx = 0;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_ESCAPE; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_ESCAPE; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    Sleep(50);

    // Read from clipboard
    if (OpenClipboard(nullptr)) {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData) {
            WCHAR* pszText = (WCHAR*)GlobalLock(hData);
            if (pszText) {
                foundPath = std::wstring(pszText);
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }

    return foundPath;
}

static std::wstring GetParentPath(const std::wstring& path) {
    if (path.empty()) return L"";
    WCHAR buffer[MAX_PATH];
    wcsncpy(buffer, path.c_str(), MAX_PATH);
    buffer[MAX_PATH - 1] = 0;
    PathRemoveFileSpecW(buffer);
    return std::wstring(buffer);
}

static void NavigateNewTab(HWND top, const std::wstring& targetPath) {
    if (targetPath.length() < 2) return;

    Wh_Log(L"NavigateNewTab: %s", targetPath.c_str());

    SetForegroundWindow(top);
    Sleep(50);

    // Put path in clipboard (with exclusion markers for clipboard history)
    static UINT cfExclude = RegisterClipboardFormat(L"ExcludeClipboardContentFromMonitorProcessing");
    static UINT cfCanInclude = RegisterClipboardFormat(L"CanIncludeInClipboardHistory");

    if (OpenClipboard(nullptr)) {
        EmptyClipboard();
        size_t size = (targetPath.length() + 1) * sizeof(WCHAR);
        HGLOBAL hGlobalPath = GlobalAlloc(GMEM_MOVEABLE, size);
        if (hGlobalPath) {
            void* pData = GlobalLock(hGlobalPath);
            memcpy(pData, targetPath.c_str(), size);
            GlobalUnlock(hGlobalPath);
            SetClipboardData(CF_UNICODETEXT, hGlobalPath);
        }
        // Mark as excluded from clipboard history
        HGLOBAL hGlobalExclude = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hGlobalExclude) {
            void* pData = GlobalLock(hGlobalExclude);
            *(DWORD*)pData = 0;
            GlobalUnlock(hGlobalExclude);
            SetClipboardData(cfExclude, hGlobalExclude);
        }
        HGLOBAL hGlobalCanInclude = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hGlobalCanInclude) {
            void* pData = GlobalLock(hGlobalCanInclude);
            *(DWORD*)pData = 0;
            GlobalUnlock(hGlobalCanInclude);
            SetClipboardData(cfCanInclude, hGlobalCanInclude);
        }
        CloseClipboard();
    }

    INPUT inputs[4] = {};
    int idx = 0;

    // Ctrl+T (New tab)
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'T'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'T'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    Sleep(450);

    // Alt+D (Focus address bar)
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'D'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_MENU; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    Sleep(100);

    // Ctrl+V (Paste path)
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'V'; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = 'V'; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_CONTROL; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
    Sleep(50);

    // Enter (Navigate)
    idx = 0; memset(inputs, 0, sizeof(inputs));
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_RETURN; idx++;
    inputs[idx].type = INPUT_KEYBOARD; inputs[idx].ki.wVk = VK_RETURN; inputs[idx].ki.dwFlags = KEYEVENTF_KEYUP; idx++;
    SendInput(idx, inputs, sizeof(INPUT));
}

static void OpenParentInNewTab(HWND top) {
    if (!IsWindow(top)) return;

    Wh_Log(L"OpenParentInNewTab");

    // Backup clipboard before we start using it
    ClipboardBackup backup = BackupClipboard();

    // Get current path via clipboard probe (sends Alt+D, Ctrl+C, then reads clipboard)
    std::wstring currentPath = ProbeAddressBarWithClipboard(top);
    if (currentPath.empty()) {
        Wh_Log(L"[err] Failed to get current path");
        RestoreClipboard(backup);
        return;
    }

    Wh_Log(L"Current path: %s", currentPath.c_str());

    // Compute parent path
    std::wstring parentPath = GetParentPath(currentPath);
    if (parentPath.empty() || parentPath == currentPath) {
        Wh_Log(L"[err] Already at root or failed to get parent");
        RestoreClipboard(backup);
        return;
    }

    Wh_Log(L"Parent path: %s", parentPath.c_str());

    // Navigate to parent in new tab
    NavigateNewTab(top, parentPath);

    // Give time for paste to complete, then restore clipboard
    Sleep(200);
    RestoreClipboard(backup);
}

static bool InvokeUpInWindow(HWND top) {
    Wh_Log(L"InvokeUpInWindow");
    if (!IsWindow(top)) return false;

    SetForegroundWindow(top);
    Sleep(kFocusDelayMs);
    AltUp();
    return true;
}

// ---------- Action: Ctrl+N → focus new → UIA Up ----------
static void DuplicateAndInvokeUp(HWND top) {
    if (!IsWindow(top)) return;

    // Ensure keystrokes go to the source window
    SetForegroundWindow(top);

    // Duplicate window
    Sleep(15);
    CtrlN();

    // Find the new window (same process)
    DWORD srcPid = 0; GetWindowThreadProcessId(top, &srcPid);
    HWND target = nullptr;

    DWORD t0 = GetTickCount();
    while (!target && GetTickCount() - t0 < kNewWindowWaitMs) {
        // Wait for newly opened window...
        Sleep(30);
        HWND fg = GetForegroundWindow();
        if (fg && fg != top) {
            wchar_t cls[64]{};
            if (GetClassNameW(fg, cls, (int)ARRAYSIZE(cls)) && wcscmp(cls, L"CabinetWClass") == 0) {
                DWORD pid = 0; GetWindowThreadProcessId(fg, &pid);
                if (pid == srcPid) target = fg;
            }
        }
    }
    if (!target) {
        FindNewWinCtx ctx{ top, srcPid, nullptr };
        EnumWindows(EnumNewExplorerProc, (LPARAM)&ctx);
        if (ctx.found) target = ctx.found;
    }

    if (target) {
        // Wh_Log(L"New window found, navigating up");
        ForceForeground(target);
        InvokeUpInWindow(target);
    } else {
        // Fallback: try current foreground window
        // Wh_Log(L"New window not found, trying foreground");
        HWND fg = GetForegroundWindow();
        if (IsExplorerTop(fg)) {
            ForceForeground(fg);
            Sleep(kFocusDelayMs);
            InvokeUpInWindow(fg);
        }
    }
}

// ---------- Worker ----------
struct OpenParentTask { HWND top; OpenMode mode; };

static DWORD WINAPI OpenParentWorker(LPVOID param) {
    OpenParentTask* t = (OpenParentTask*)param;
    if (t) {
        HWND top = t->top;
        OpenMode mode = t->mode;
        delete t;

        if (mode == OpenMode::Tab) {
            Wh_Log(L"[Act] Opening parent in new tab");
            OpenParentInNewTab(top);
        } else {
            Wh_Log(L"[Act] Ctrl+N → focus new → Alt+Up");
            DuplicateAndInvokeUp(top);
        }
    }

    ResetInProgress();
    return 0;
}

static void OpenParent(HWND top) {
    Wh_Log(L"OpenParent: mode=%s", g_settings.openIn == OpenMode::Tab ? L"tab" : L"window");
    OpenParentTask* t = new OpenParentTask{ top, g_settings.openIn };
    HANDLE h = CreateThread(nullptr, 0, OpenParentWorker, t, 0, nullptr);
    if (h) {
        CloseHandle(h);
    } else {
        Wh_Log(L"[err] CreateThread failed: %lu", GetLastError());
        delete t;
        ResetInProgress();
    }
}

// ---------- Mouse hook (Middle button up, Ctrl + left button up) ----------
static LRESULT CALLBACK MouseLLProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        ULONGLONG now = GetTickCount64();
        const MSLLHOOKSTRUCT* ms = (const MSLLHOOKSTRUCT*)lp;
        bool ctrlDown = isKeyDown(VK_CONTROL);

        // Ignore our own injected events (avoid recursion)
        // Also ignore mouse events we're not interested in
        if (ms->flags & LLMHF_INJECTED ||
            (
                (wp != WM_LBUTTONDOWN || !ctrlDown) &&
                (wp != WM_LBUTTONUP) &&
                (wp != WM_MBUTTONDOWN) &&
                (wp != WM_MBUTTONUP) &&
                (wp != WM_MBUTTONDBLCLK)
            )) {
            return CallNextHookEx(g_mouseHook, code, wp, lp);
        }

        // Swallow matching UP if we already handled DOWN
        if (g_suppress == SuppressBtn::Middle) {
            if (wp == WM_MBUTTONUP) {
                g_suppress = SuppressBtn::None;
                // start cooldown after the matching UP
                g_midCooldownUntil = now + kMidCooldownMs;
                g_midCooldownSwallowNextUp = false;
                return 1; // matching UP
            }
        } else if (g_suppress == SuppressBtn::Left) {
            if (wp == WM_LBUTTONUP) {
                g_suppress = SuppressBtn::None;
                g_midCooldownUntil = now + kMidCooldownMs;
                g_midCooldownSwallowNextUp = false;
                return 1;
            }
        }

        // cooldown right after a handled middle-click
        if (now < g_midCooldownUntil) {
            if (wp == WM_MBUTTONDBLCLK || wp == WM_MBUTTONDOWN) {
                // if the system synthesized a DBLCLK, remember to eat its following UP
                g_midCooldownSwallowNextUp = true;
                return 1;
            } else if (wp == WM_MBUTTONDOWN || wp == WM_LBUTTONDOWN) {
                return 1;
            }
        }
        if ((wp == WM_MBUTTONDOWN || wp == WM_LBUTTONDOWN) && g_midCooldownSwallowNextUp) {
            g_midCooldownSwallowNextUp = false; // balanced pair swallowed
            return 1;
        }

        if (IsInProgress()) {
            // Wh_Log(L"inProgress(Mouse): Ignore");
            return CallNextHookEx(g_mouseHook, code, wp, lp);
        }

        if (wp == WM_LBUTTONUP || wp == WM_MBUTTONUP) {
            lastMouseUpEventTime.store(now, std::memory_order_release);
            // Wh_Log(L"Mouse up: Skip");
            return CallNextHookEx(g_mouseHook, code, wp, lp);
        } else if (now - lastMouseUpEventTime.load(std::memory_order_acquire) < kMouseDebounceMs) {
            // Wh_Log(L"Mouse down too close to mouse up (%d ms): Skip", now - lastMouseUpEventTime);
            return CallNextHookEx(g_mouseHook, code, wp, lp);
        }

        POINT pt = ms->pt;
        HWND hwndAtPt = WindowFromPoint(pt);
        HWND top = GetAncestor(hwndAtPt, GA_ROOT);

        // Wh_Log(L"hwndAtPt: %p", hwndAtPt);

        if ((wp == WM_LBUTTONDOWN || wp == WM_MBUTTONDOWN) && IsExplorerTop(hwndAtPt)) {
            // Optimization: Avoid expensive UIA calls if not clicking near the top (toolbar)
            RECT rc;
            if (GetWindowRect(top, &rc)) {
                // Address bar is typically at the top. 200px is a safe generous margin.
                if ((long)ms->pt.y - rc.top > 200) {
                     return CallNextHookEx(g_mouseHook, code, wp, lp);
                }
            }

            EnsureUiaForThisThread();
            if (t_uia) {
                IUIAutomationElement* el = nullptr;
                if (SUCCEEDED(t_uia->ElementFromPoint(pt, &el)) && el) {
                    std::wstring nm, id;
                    bool isUp = CheckElementAndAncestorsForUp(t_walker, el, nm, id);
                    if (isUp) {
                        SuppressBtn which = SuppressBtn::None;
                        bool shouldHandle = false;
                        if (wp == WM_MBUTTONDOWN) {
                            Wh_Log(L"Mbutton down");
                            shouldHandle = true;
                            which = SuppressBtn::Middle;
                        } else if (wp == WM_LBUTTONDOWN && ctrlDown) {
                            Wh_Log(L"Ctrl + Lbutton down");
                            shouldHandle = true;
                            which = SuppressBtn::Left;
                        }

                        if (shouldHandle) {
                            Wh_Log(L"[Up+] %s on Up → open parent",
                                (which == SuppressBtn::Middle) ? L"Middle-down" : L"Ctrl+Left-down");
                            g_suppress = which;
                            if (!SetInProgressIfIdle()) {
                                el->Release();
                                return CallNextHookEx(g_mouseHook, code, wp, lp);
                            }
                            OpenParent(top);
                            return 1; // swallow DOWN; matching UP swallowed above
                        }
                    }
                    el->Release();
                }
            }
        }
    }
    return CallNextHookEx(g_mouseHook, code, wp, lp);
}

// ---------- Keyboard hook (Ctrl+Alt+Up) ----------
static LRESULT CALLBACK KbdLLProc(int code, WPARAM wp, LPARAM lp);

// ---------- Hook thread (LL hooks require a message pump) ----------
static DWORD WINAPI HookThreadProc(LPVOID pParameter) {
    HANDLE hThreadReadyEvent = (HANDLE)pParameter;

    // Create message queue before signaling ready
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Install hooks from this thread
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseLLProc, nullptr, 0);
    if (!g_mouseHook) {
        Wh_Log(L"[err] SetWindowsHookEx WH_MOUSE_LL failed: %lu", GetLastError());
    }

    g_kbdHook = SetWindowsHookExW(WH_KEYBOARD_LL, KbdLLProc, nullptr, 0);
    if (!g_kbdHook) {
        Wh_Log(L"[err] SetWindowsHookEx WH_KEYBOARD_LL failed: %lu", GetLastError());
    }

    // Signal that we're ready
    SetEvent(hThreadReadyEvent);

    // Message loop - required for LL hooks to work
    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            break;
        }

        // WM_APP signals us to exit
        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup hooks
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    if (g_kbdHook) {
        UnhookWindowsHookEx(g_kbdHook);
        g_kbdHook = nullptr;
    }

    // Cleanup thread-local UIA resources
    CleanupThreadUia();

    return 0;
}

static LRESULT CALLBACK KbdLLProc(int code, WPARAM wp, LPARAM lp) {
    if (!IsInProgress() && code == HC_ACTION && (wp == WM_KEYDOWN || wp == WM_KEYUP || wp == WM_SYSKEYDOWN)) {
        const KBDLLHOOKSTRUCT* ks = (const KBDLLHOOKSTRUCT*)lp;

        // Swallow matching UP if we already handled DOWN
        if (wp == WM_KEYUP) {
            if (g_suppress == SuppressBtn::Key) {
                g_suppress = SuppressBtn::None;
                return 1;
            } else {
                return CallNextHookEx(g_kbdHook, code, wp, lp);
            }
        }

        if (ks->vkCode == VK_UP) {
            if (isKeyDown(VK_MENU) && isKeyDown(VK_CONTROL)) {
                HWND fg = GetForegroundWindow();
                if (IsExplorerTop(fg)) {
                    Wh_Log(L"[Up+] Ctrl+Alt+Up → open parent");
                    g_suppress = SuppressBtn::Key;
                    if (!SetInProgressIfIdle()) {
                        return CallNextHookEx(g_kbdHook, code, wp, lp);
                    }
                    OpenParent(fg);
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(g_kbdHook, code, wp, lp);
}

// ---------- Tool mod entry points ----------
BOOL WhTool_ModInit() {
    Wh_Log(L"Init (Up → New Window/Tab, LL hooks in dedicated thread)");

    LoadSettings();
    ResetInProgress();

    // Create event to signal when hook thread is ready
    HANDLE hThreadReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hThreadReadyEvent) {
        Wh_Log(L"[err] CreateEvent failed: %lu", GetLastError());
        return FALSE;
    }

    // Create hook thread (LL hooks require a message pump)
    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, hThreadReadyEvent, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"[err] CreateThread failed: %lu", GetLastError());
        CloseHandle(hThreadReadyEvent);
        return FALSE;
    }

    // Wait for hook thread to initialize
    WaitForSingleObject(hThreadReadyEvent, INFINITE);
    CloseHandle(hThreadReadyEvent);

    return TRUE;
}

void WhTool_ModUninit() {
    // Signal hook thread to exit and wait for it
    HANDLE hThread = InterlockedExchangePointer(&g_hookThread, nullptr);
    if (hThread) {
        PostThreadMessage(g_hookThreadId, WM_APP, 0, 0);
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    g_suppress = SuppressBtn::None;
    ResetInProgress();
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
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
