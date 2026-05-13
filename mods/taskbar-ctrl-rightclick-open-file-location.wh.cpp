// ==WindhawkMod==
// @id              taskbar-ctrl-rightclick-open-file-location
// @name            Ctrl+Right-click taskbar to Open file location
// @description     Holding Ctrl while right-clicking a taskbar button opens the running program's file location in Explorer (instead of showing the jump list). Works with the classic / StartAllBack / ExplorerPatcher taskbar (MSTaskListWClass).
// @version         1.2.0
// @author          tria
// @github          https://github.com/triatomic
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lpsapi -lshell32 -loleacc -luser32 -lole32 -loleaut32 -lversion -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Ctrl+Right-click taskbar to Open file location

Hold **Ctrl** and **right-click** a running app's taskbar button to open
its executable in File Explorer (the file is selected — same result as the
jump list's "Open file location" option). Without Ctrl, right-click behaves
normally and the jump list / context menu appears.

This version targets the **classic taskbar** (`MSTaskListWClass`) used by:

- Stock Windows 10
- Windows 11 with **StartAllBack**
- Windows 11 with **ExplorerPatcher** (legacy taskbar mode)

It subclasses `MSTaskListWClass` directly (so the right-click never
reaches the taskbar's own context-menu handler) and uses Microsoft Active
Accessibility (MSAA) to identify the button under the cursor — no
dependency on private taskbar.dll symbols, resilient to Windows updates.

The mod runs entirely inside `explorer.exe`, so the hook is removed
automatically when Windhawk unloads it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifier: ctrl
  $name: Modifier key
  $description: Which key must be held while clicking the taskbar button.
  $options:
  - ctrl: Ctrl
  - shift: Shift
  - alt: Alt
  - win: Win
  - none: None (just the click)
- mouseButton: right
  $name: Mouse button
  $description: Which mouse button triggers the action.
  $options:
  - right: Right-click (suppresses the jump list / context menu)
  - middle: Middle-click
- action: openLocation
  $name: Action
  $description: What to do when the trigger fires.
  $options:
  - openLocation: Open file location in Explorer
  - copyPath: Copy executable path to clipboard
  - runAsAdmin: Launch a new elevated instance (UAC prompt)
- holdMs: 0
  $name: Hold time (ms)
  $description: >-
    If non-zero, the trigger must be held down for at least this many
    milliseconds before firing. A shorter press does nothing. 0 = fire
    instantly on press.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>
#include <shlobj.h>
#include <oleacc.h>
#include <windowsx.h>

#include <string>
#include <vector>
#include <atomic>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

enum class Modifier { None, Ctrl, Shift, Alt, Win };
enum class MouseBtn { Right, Middle };
enum class Action { OpenLocation, CopyPath, RunAsAdmin };

struct Settings {
    Modifier modifier = Modifier::Ctrl;
    MouseBtn mouseButton = MouseBtn::Right;
    Action action = Action::OpenLocation;
    int holdMs = 0;
};

static Settings g_settings;

static int VkForModifier(Modifier m) {
    switch (m) {
        case Modifier::Ctrl:  return VK_CONTROL;
        case Modifier::Shift: return VK_SHIFT;
        case Modifier::Alt:   return VK_MENU;
        case Modifier::Win:   return VK_LWIN;  // also accept RWIN below
        default:              return 0;
    }
}

static bool IsModifierHeld(Modifier m) {
    if (m == Modifier::None) return true;
    int vk = VkForModifier(m);
    if (vk == VK_LWIN) {
        return (GetKeyState(VK_LWIN) & 0x8000) ||
               (GetKeyState(VK_RWIN) & 0x8000);
    }
    return (GetKeyState(vk) & 0x8000) != 0;
}

static void LoadSettings() {
    PCWSTR mod = Wh_GetStringSetting(L"modifier");
    g_settings.modifier = Modifier::Ctrl;
    if (mod) {
        if (!wcscmp(mod, L"shift")) g_settings.modifier = Modifier::Shift;
        else if (!wcscmp(mod, L"alt")) g_settings.modifier = Modifier::Alt;
        else if (!wcscmp(mod, L"win")) g_settings.modifier = Modifier::Win;
        else if (!wcscmp(mod, L"none")) g_settings.modifier = Modifier::None;
        Wh_FreeStringSetting(mod);
    }

    PCWSTR btn = Wh_GetStringSetting(L"mouseButton");
    g_settings.mouseButton = MouseBtn::Right;
    if (btn) {
        if (!wcscmp(btn, L"middle")) g_settings.mouseButton = MouseBtn::Middle;
        Wh_FreeStringSetting(btn);
    }

    PCWSTR act = Wh_GetStringSetting(L"action");
    g_settings.action = Action::OpenLocation;
    if (act) {
        if (!wcscmp(act, L"copyPath")) g_settings.action = Action::CopyPath;
        else if (!wcscmp(act, L"runAsAdmin"))
            g_settings.action = Action::RunAsAdmin;
        Wh_FreeStringSetting(act);
    }

    int hold = Wh_GetIntSetting(L"holdMs");
    if (hold < 0) hold = 0;
    if (hold > 5000) hold = 5000;
    g_settings.holdMs = hold;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::wstring GetExePathOfHwnd(HWND hWnd) {
    if (!hWnd) return {};
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return {};
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return {};
    WCHAR buf[MAX_PATH * 2];
    DWORD sz = ARRAYSIZE(buf);
    BOOL ok = QueryFullProcessImageNameW(h, 0, buf, &sz);
    CloseHandle(h);
    if (!ok) return {};
    return std::wstring(buf, sz);
}

static std::wstring StripExtFromBasename(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    std::wstring base =
        (slash == std::wstring::npos) ? path : path.substr(slash + 1);
    size_t dot = base.find_last_of(L'.');
    if (dot != std::wstring::npos) base.resize(dot);
    return base;
}

static void OpenFileLocation(const std::wstring& path) {
    PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(path.c_str());
    if (pidl) {
        SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
        ILFree(pidl);
        return;
    }
    std::wstring args = L"/select,\"" + path + L"\"";
    ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(),
                  nullptr, SW_SHOWNORMAL);
}

static void CopyPathToClipboard(const std::wstring& path) {
    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();
    size_t bytes = (path.size() + 1) * sizeof(wchar_t);
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (h) {
        void* p = GlobalLock(h);
        if (p) {
            memcpy(p, path.c_str(), bytes);
            GlobalUnlock(h);
            if (!SetClipboardData(CF_UNICODETEXT, h)) {
                GlobalFree(h);
            }
        } else {
            GlobalFree(h);
        }
    }
    CloseClipboard();
}

static void LaunchElevated(const std::wstring& path) {
    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.lpVerb = L"runas";
    info.lpFile = path.c_str();
    info.nShow = SW_SHOWNORMAL;
    info.fMask = SEE_MASK_NOASYNC;
    ShellExecuteExW(&info);
}

static void RunAction(const std::wstring& exePath) {
    switch (g_settings.action) {
        case Action::OpenLocation: OpenFileLocation(exePath); break;
        case Action::CopyPath:     CopyPathToClipboard(exePath); break;
        case Action::RunAsAdmin:   LaunchElevated(exePath); break;
    }
}

// Lowercase, ASCII-only — enough for filename / app-name comparisons.
static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        if (c >= L'A' && c <= L'Z') c = c + (L'a' - L'A');
    }
    return s;
}

// Strip everything except alphanumerics, then lowercase. Lets us compare
// "MSI Afterburner" with "MSIAfterburner.exe" -> both -> "msiafterburner".
static std::wstring NormalizeAlnum(const std::wstring& s) {
    std::wstring out;
    out.reserve(s.size());
    for (wchar_t c : s) {
        if ((c >= L'0' && c <= L'9') ||
            (c >= L'A' && c <= L'Z') ||
            (c >= L'a' && c <= L'z')) {
            out.push_back(c >= L'A' && c <= L'Z' ? c + (L'a' - L'A') : c);
        }
    }
    return out;
}

// Read the FileDescription string from the exe's version-info resource.
// This is what app launchers and the classic taskbar use as the friendly
// app label (e.g. ConEmu64.exe -> "ConEmu", MSIAfterburner.exe -> "MSI
// Afterburner").
static std::wstring GetFileDescription(const std::wstring& exePath) {
    DWORD dummy = 0;
    DWORD sz = GetFileVersionInfoSizeW(exePath.c_str(), &dummy);
    if (!sz) return {};
    std::vector<BYTE> buf(sz);
    if (!GetFileVersionInfoW(exePath.c_str(), 0, sz, buf.data())) return {};

    struct LangCp { WORD lang; WORD cp; };
    LangCp* tr = nullptr;
    UINT trLen = 0;
    if (!VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation",
                        (LPVOID*)&tr, &trLen) || !tr || trLen < sizeof(LangCp)) {
        return {};
    }

    // Try each translation; return the first FileDescription we find.
    UINT nTr = trLen / sizeof(LangCp);
    for (UINT i = 0; i < nTr; ++i) {
        wchar_t sub[64];
        swprintf_s(sub, L"\\StringFileInfo\\%04x%04x\\FileDescription",
                   tr[i].lang, tr[i].cp);
        wchar_t* val = nullptr;
        UINT valLen = 0;
        if (VerQueryValueW(buf.data(), sub, (LPVOID*)&val, &valLen) &&
            val && valLen > 1) {
            std::wstring out(val, valLen);
            // Trim trailing NUL chars.
            while (!out.empty() && out.back() == L'\0') out.pop_back();
            if (!out.empty()) return out;
        }
    }
    return {};
}

// Classic / StartAllBack taskbar button labels look like
// "<title> - N running window(s)". Returns just the title segment.
static bool ParseButtonLabel(const std::wstring& label,
                             std::wstring& prefix) {
    size_t pos = label.rfind(L" - ");
    if (pos == std::wstring::npos) return false;
    if (pos + 3 >= label.size()) return false;
    wchar_t first = label[pos + 3];
    if (first < L'0' || first > L'9') return false;
    prefix = label.substr(0, pos);
    return !prefix.empty();
}

struct FindByLabelCtx {
    std::wstring needleLower;   // lowercased label text
    std::wstring needleAlnum;   // alnum-normalized label text
    HWND found;
};

static BOOL CALLBACK FindByLabelProc(HWND hWnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<FindByLabelCtx*>(lParam);
    if (!IsWindowVisible(hWnd)) return TRUE;
    if (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
    if (GetWindow(hWnd, GW_OWNER) != nullptr) return TRUE;

    std::wstring exePath = GetExePathOfHwnd(hWnd);
    if (exePath.empty()) return TRUE;

    // Skip windows owned by explorer.exe. The shell exposes accessibility-
    // friendly phantom windows (tooltips, thumbnail previews, taskbar
    // helpers) whose titles mirror running app names, which would otherwise
    // produce false matches like "foobar2000" -> C:\Windows\explorer.exe.
    std::wstring exeBase = ToLower(StripExtFromBasename(exePath));
    if (exeBase == L"explorer") return TRUE;

    int len = GetWindowTextLengthW(hWnd);
    if (len > 0) {
        std::wstring title(len + 1, L'\0');
        int got = GetWindowTextW(hWnd, title.data(), len + 1);
        if (got > 0) {
            title.resize(got);
            if (ToLower(title) == ctx->needleLower) {
                ctx->found = hWnd;
                return FALSE;
            }
        }
    }

    std::wstring desc = GetFileDescription(exePath);
    if (!desc.empty() && ToLower(desc) == ctx->needleLower) {
        ctx->found = hWnd;
        return FALSE;
    }

    // Handles "MSI Afterburner" <-> "MSIAfterburner.exe", "ConEmu (x64)" <->
    // "ConEmu64.exe". Bidirectional so labels with extra suffixes match too.
    if (!ctx->needleAlnum.empty()) {
        auto bidirContains = [&](const std::wstring& a) {
            if (a.empty()) return false;
            return a.find(ctx->needleAlnum) != std::wstring::npos ||
                   ctx->needleAlnum.find(a) != std::wstring::npos;
        };
        if (bidirContains(NormalizeAlnum(desc)) ||
            bidirContains(NormalizeAlnum(StripExtFromBasename(exePath)))) {
            ctx->found = hWnd;
            return FALSE;
        }
    }

    return TRUE;
}

// Find any visible top-level window that matches the given taskbar-button
// label. Tries title, then exe FileDescription, then alphanumeric-normalized
// comparison with both.
static HWND FindWindowByLabel(const std::wstring& label) {
    FindByLabelCtx ctx{ToLower(label), NormalizeAlnum(label), nullptr};
    EnumWindows(FindByLabelProc, reinterpret_cast<LPARAM>(&ctx));
    return ctx.found;
}

// Read accValue from an IAccessible child; on classic taskbar buttons this
// is the running window's HWND as a decimal string. Returns nullptr if the
// value is missing, not a number, or doesn't reference a valid window.
static HWND ReadHwndFromAccValue(IAccessible* pAcc, VARIANT& vChild) {
    HWND hwnd = nullptr;
    BSTR bstrVal = nullptr;
    if (SUCCEEDED(pAcc->get_accValue(vChild, &bstrVal)) && bstrVal &&
        SysStringLen(bstrVal) > 0) {
        wchar_t* end = nullptr;
        unsigned long long n = wcstoull(bstrVal, &end, 10);
        if (end && *end == L'\0' && n != 0) {
            HWND h = reinterpret_cast<HWND>(static_cast<uintptr_t>(n));
            if (IsWindow(h)) hwnd = h;
        }
    }
    if (bstrVal) SysFreeString(bstrVal);
    return hwnd;
}

static std::wstring ReadAccName(IAccessible* pAcc, VARIANT& vChild) {
    std::wstring out;
    BSTR b = nullptr;
    if (SUCCEEDED(pAcc->get_accName(vChild, &b)) && b && SysStringLen(b) > 0) {
        out.assign(b, SysStringLen(b));
    }
    if (b) SysFreeString(b);
    return out;
}

// Walk children of pAcc looking for one whose accValue exposes a valid HWND.
// Used as a fallback for grouped buttons whose top-level entry has no HWND
// but whose sub-children (one per running window) do.
static HWND FindHwndInAccChildren(IAccessible* pAcc, int depth) {
    if (!pAcc || depth > 3) return nullptr;

    long count = 0;
    if (FAILED(pAcc->get_accChildCount(&count)) || count <= 0) {
        return nullptr;
    }

    // AccessibleChildren returns a mix of VT_DISPATCH (real sub-IAccessible)
    // and VT_I4 (numeric child id within pAcc itself).
    std::vector<VARIANT> children(count);
    for (auto& v : children) VariantInit(&v);
    long obtained = 0;
    HRESULT hr = AccessibleChildren(pAcc, 0, count, children.data(),
                                    &obtained);
    HWND found = nullptr;
    if (SUCCEEDED(hr)) {
        for (long i = 0; i < obtained && !found; ++i) {
            VARIANT& v = children[i];
            if (v.vt == VT_DISPATCH && v.pdispVal) {
                IAccessible* childAcc = nullptr;
                if (SUCCEEDED(v.pdispVal->QueryInterface(
                        IID_IAccessible, (void**)&childAcc)) && childAcc) {
                    VARIANT self;
                    VariantInit(&self);
                    self.vt = VT_I4;
                    self.lVal = CHILDID_SELF;
                    found = ReadHwndFromAccValue(childAcc, self);
                    VariantClear(&self);
                    if (!found) {
                        found = FindHwndInAccChildren(childAcc, depth + 1);
                    }
                    childAcc->Release();
                }
            } else if (v.vt == VT_I4) {
                found = ReadHwndFromAccValue(pAcc, v);
            }
        }
    }
    for (auto& v : children) VariantClear(&v);
    return found;
}

// Query the taskbar button under the cursor for the running window's HWND.
// First tries accValue on the exact MSAA child at the point (works for
// single-window buttons). For grouped buttons that have no own HWND, walks
// the children to grab any one window's HWND from the group — for the
// "open file location" use case that's fine because all windows in a group
// share the same .exe.
static bool GetTaskbarButtonInfoAtPoint(POINT pt, HWND& outHwnd,
                                        std::wstring& outName) {
    outHwnd = nullptr;
    outName.clear();

    IAccessible* pAcc = nullptr;
    VARIANT vChild;
    VariantInit(&vChild);
    HRESULT hr = AccessibleObjectFromPoint(pt, &pAcc, &vChild);
    if (FAILED(hr) || !pAcc) {
        return false;
    }

    outHwnd = ReadHwndFromAccValue(pAcc, vChild);
    outName = ReadAccName(pAcc, vChild);

    // Grouped button: the entry at the cursor has no HWND of its own. Get
    // the sub-IAccessible for the child and walk its children.
    if (!outHwnd) {
        IAccessible* childAcc = nullptr;
        if (vChild.vt == VT_I4 && vChild.lVal != CHILDID_SELF) {
            // Try to resolve the numeric child id to a real IAccessible.
            IDispatch* pDisp = nullptr;
            if (SUCCEEDED(pAcc->get_accChild(vChild, &pDisp)) && pDisp) {
                pDisp->QueryInterface(IID_IAccessible, (void**)&childAcc);
                pDisp->Release();
            }
        } else if (vChild.vt == VT_DISPATCH && vChild.pdispVal) {
            vChild.pdispVal->QueryInterface(IID_IAccessible,
                                            (void**)&childAcc);
        }

        if (childAcc) {
            outHwnd = FindHwndInAccChildren(childAcc, 0);
            childAcc->Release();
        }
        if (!outHwnd) {
            outHwnd = FindHwndInAccChildren(pAcc, 0);
        }
    }

    VariantClear(&vChild);
    pAcc->Release();

    return outHwnd != nullptr || !outName.empty();
}

// ---------------------------------------------------------------------------
// Click interception (window subclass on MSTaskListWClass)
// ---------------------------------------------------------------------------

static DWORD g_hookThreadId = 0;
static std::vector<HWND> g_subclassedWindows;

// Posted to the worker thread via PostThreadMessage. wParam = packed POINT.
constexpr UINT WM_APP_OPEN_FILE_LOCATION = WM_APP + 1;

static void RunOpenFileLocationForPoint(POINT pt) {
    HWND target = nullptr;
    std::wstring name;
    if (!GetTaskbarButtonInfoAtPoint(pt, target, name)) {
        Wh_Log(L"Could not get accessible info at %ld,%ld", pt.x, pt.y);
        return;
    }
    Wh_Log(L"Taskbar button hwnd=%p name='%s'", target, name.c_str());

    if (!target) {
        std::wstring prefix;
        std::wstring label =
            ParseButtonLabel(name, prefix) ? prefix : name;
        Wh_Log(L"Searching by label='%s'", label.c_str());
        target = FindWindowByLabel(label);
    }

    if (!target) {
        Wh_Log(L"No HWND resolved for button '%s'", name.c_str());
        return;
    }

    std::wstring exe = GetExePathOfHwnd(target);
    if (exe.empty()) {
        Wh_Log(L"Could not resolve exe for HWND %p", target);
        return;
    }
    Wh_Log(L"Running action on: %s", exe.c_str());
    RunAction(exe);
}

// Pending hold-time gesture: which subclassed window armed it, and the
// screen point of the press. Cleared by WM_xBUTTONUP or by the timer firing.
static std::atomic<UINT_PTR> g_pendingTimerId{0};
static POINT g_pendingPoint{};

static UINT DownMsgForButton(MouseBtn b) {
    return b == MouseBtn::Middle ? WM_MBUTTONDOWN : WM_RBUTTONDOWN;
}
static UINT UpMsgForButton(MouseBtn b) {
    return b == MouseBtn::Middle ? WM_MBUTTONUP : WM_RBUTTONUP;
}

static void CALLBACK HoldTimerProc(HWND hWnd, UINT, UINT_PTR id, DWORD) {
    KillTimer(hWnd, id);
    if (g_pendingTimerId.exchange(0) != id) return;
    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_APP_OPEN_FILE_LOCATION,
                           MAKELONG(g_pendingPoint.x, g_pendingPoint.y), 0);
    }
}

// Subclass procedure attached to MSTaskListWClass. Running before SAB's own
// handler lets us suppress the configured button entirely.
static LRESULT CALLBACK TaskListSubclassProc(HWND hWnd, UINT uMsg,
                                             WPARAM wParam, LPARAM lParam,
                                             DWORD_PTR /*dwRefData*/) {
    UINT downMsg = DownMsgForButton(g_settings.mouseButton);
    UINT upMsg = UpMsgForButton(g_settings.mouseButton);
    bool isCtxMenu = (uMsg == WM_CONTEXTMENU) &&
                     (g_settings.mouseButton == MouseBtn::Right);

    if (uMsg == downMsg || uMsg == upMsg || isCtxMenu) {
        if (!IsModifierHeld(g_settings.modifier)) {
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        POINT pt;
        if (isCtxMenu) {
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            if (pt.x == -1 && pt.y == -1) GetCursorPos(&pt);
        } else {
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ClientToScreen(hWnd, &pt);
        }

        if (uMsg == downMsg) {
            if (g_settings.holdMs > 0) {
                g_pendingPoint = pt;
                UINT_PTR id = SetTimer(hWnd, 0xC771,
                                       (UINT)g_settings.holdMs,
                                       HoldTimerProc);
                g_pendingTimerId.store(id);
                Wh_Log(L"Armed hold-timer at %ld,%ld", pt.x, pt.y);
            } else {
                Wh_Log(L"Trigger at %ld,%ld", pt.x, pt.y);
                if (g_hookThreadId) {
                    PostThreadMessageW(g_hookThreadId,
                                       WM_APP_OPEN_FILE_LOCATION,
                                       MAKELONG(pt.x, pt.y), 0);
                }
            }
        } else if (uMsg == upMsg) {
            // Click released — cancel any pending hold-timer (short press).
            UINT_PTR id = g_pendingTimerId.exchange(0);
            if (id) {
                KillTimer(hWnd, id);
                Wh_Log(L"Hold released early; not firing");
            }
        }
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hWnd, TaskListSubclassProc);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static BOOL CALLBACK EnumChildSubclassProc(HWND hWnd, LPARAM /*lParam*/) {
    WCHAR cls[64];
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) > 0 &&
        wcscmp(cls, L"MSTaskListWClass") == 0) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                hWnd, TaskListSubclassProc, 0)) {
            g_subclassedWindows.push_back(hWnd);
            Wh_Log(L"Subclassed MSTaskListWClass %p", hWnd);
        }
    }
    return TRUE;
}

static BOOL CALLBACK EnumTrayWndProc(HWND hWnd, LPARAM lParam) {
    WCHAR cls[64];
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) > 0 &&
        (wcscmp(cls, L"Shell_TrayWnd") == 0 ||
         wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0)) {
        EnumChildWindows(hWnd, EnumChildSubclassProc, lParam);
    }
    return TRUE;
}

static void AttachTaskbarSubclasses() {
    EnumWindows(EnumTrayWndProc, 0);
}

// ---------------------------------------------------------------------------
// Worker thread: owns the COM-apartment and message pump that runs the
// "open file location" action posted from the subclass proc.
// ---------------------------------------------------------------------------

static HANDLE g_workerThread = nullptr;

static DWORD WINAPI WorkerThreadProc(LPVOID) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr &&
            msg.message == WM_APP_OPEN_FILE_LOCATION) {
            POINT pt{static_cast<LONG>(static_cast<SHORT>(LOWORD(msg.wParam))),
                     static_cast<LONG>(static_cast<SHORT>(HIWORD(msg.wParam)))};
            RunOpenFileLocationForPoint(pt);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  &g_hookThreadId);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}

void Wh_ModAfterInit() {
    AttachTaskbarSubclasses();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    UINT_PTR pendingId = g_pendingTimerId.exchange(0);
    for (HWND hWnd : g_subclassedWindows) {
        if (IsWindow(hWnd)) {
            if (pendingId) KillTimer(hWnd, pendingId);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, TaskListSubclassProc);
        }
    }
    g_subclassedWindows.clear();

    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    g_hookThreadId = 0;
}
