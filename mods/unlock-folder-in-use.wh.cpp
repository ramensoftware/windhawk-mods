// ==WindhawkMod==
// @id              unlock-folder-in-use
// @name            Unlock "Folder In Use"
// @description     When Explorer says "Folder/File In Use", shows what is blocking it and lets you choose: end the processes and continue, or only end them
// @version         1.1.0
// @author          LyricsPortal
// @github          https://github.com/LyricsPortal
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -lrstrtmgr -luser32 -lshlwapi -luuid -lgdi32 -ldwmapi -luxtheme -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Unlock "Folder In Use"

When Explorer refuses to delete, move or rename something because it is in use ("Folder In Use" / "File In Use"), this mod finds out which processes are blocking it and lets you decide what to do.

| Before | After |
|:------:|:-----:|
| ![Before](https://i.imgur.com/9EMduqU.png) | ![After](https://i.imgur.com/vGv3Znc.png) |

When the warning appears, a small dialog lists the blocking processes and offers:

* **End processes and continue** - terminates the processes and presses *Try Again* for you, so Explorer completes the original operation.
* **Only end processes** - terminates the processes and leaves Explorer's warning open so you can retry yourself.
* **Cancel** - does nothing.

If Explorer windows are open inside the item being processed, they are moved up one level.

The dialog follows the Windows app theme: if apps are set to dark mode, the dialog is dark too.

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

* **Expand details by default** - show the list of matched items, processes and windows already expanded. If disabled, the *Details* row starts collapsed.
* **Show message when nothing is found** - show a dialog when the blocked item or its blocking process could not be identified (enabled by default).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- expandDetails: true
  $name: Expand details by default
  $description: Show the list of matched items and processes already expanded in the prompt
- showNoResultDialog: true
  $name: Show message when nothing is found
  $description: Show a dialog when the blocked item or its blocking process could not be identified
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <shldisp.h>
#include <exdisp.h>
#include <shlwapi.h>
#include <restartmanager.h>
#include <tlhelp32.h>
#include <dwmapi.h>
#include <uxtheme.h>
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

static const wchar_t* kPromptClass = L"UnlockFolderInUsePrompt";

static const wchar_t* kTitle = L"Unlock \"Folder In Use\"";

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
static ATOM g_promptClass = 0;

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

static void PressRetry(HWND dlg) {
    if (!IsWindow(dlg)) return;

    Com<IUIAutomation> ua;
    if (CreateUia(&ua)) {
        Com<IUIAutomationElement> btn;
        if (FindRetry(ua.p, dlg, &btn) && btn) {
            // One press only: if the retry fails again (or the next item is
            // also locked), the new warning must not be pressed a second time.
            if (InvokeElement(btn.p)) {
                Wh_Log(L"Retry pressed via Invoke");
                return;
            }
            if (DefaultActionElement(btn.p)) {
                Wh_Log(L"Retry pressed via default action");
                return;
            }
        }
    }

    PostMessageW(dlg, WM_COMMAND, MAKEWPARAM(IDRETRY, BN_CLICKED), 0);
    Wh_Log(L"Retry pressed via WM_COMMAND");
}

////////////////////////////////////////////////////////////////////////////////
// Prompt window
//
// A small custom window instead of a TaskDialog, because TaskDialog can't be
// drawn in dark mode.

enum Choice { CHOICE_CANCEL = 0, CHOICE_CLOSE_CONTINUE = 1, CHOICE_CLOSE_ONLY = 2 };

enum { ID_TOGGLE = 2001, ID_PRIMARY = 2002, ID_SECONDARY = 2003, ID_CANCEL = 2005 };

struct Theme {
    bool dark = false;
    COLORREF bg = 0, editBg = 0, text = 0, sub = 0;
    COLORREF btn = 0, btnHot = 0, btnDown = 0, border = 0;
    COLORREF accent = 0, accentHot = 0, accentDown = 0, accentText = 0;
};

struct Fonts {
    HFONT base = nullptr;
    HFONT semi = nullptr;
    HFONT head = nullptr;
};

struct PromptBtn {
    int id;
    std::wstring title;
    std::wstring note;
    bool primary;
    RECT rc;
};

struct PromptState {
    std::wstring instruction, content, details;
    std::vector<PromptBtn> btns;
    bool expanded = false;
    Choice result = CHOICE_CANCEL;
    bool done = false;
    bool tracking = false;
    bool kbdActive = false;
    int hot = -1, down = -1, kbd = -1;
    UINT dpi = 96;
    Theme th;
    Fonts fonts;
    HWND edit = nullptr;
    HBRUSH bgBrush = nullptr;
    HBRUSH editBrush = nullptr;
    RECT rInstr = {}, rContent = {}, rEdit = {};
    int clientW = 0, clientH = 0;
};

static bool SystemUsesDarkMode() {
    DWORD v = 1, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz) != ERROR_SUCCESS)
        return false;
    return v == 0;
}

static COLORREF MixColor(COLORREF a, COLORREF b, int pct) {
    return RGB(GetRValue(a) + (GetRValue(b) - GetRValue(a)) * pct / 100,
               GetGValue(a) + (GetGValue(b) - GetGValue(a)) * pct / 100,
               GetBValue(a) + (GetBValue(b) - GetBValue(a)) * pct / 100);
}

static Theme MakeTheme() {
    Theme t;
    t.dark = SystemUsesDarkMode();
    if (t.dark) {
        t.bg = RGB(32, 32, 32);
        t.editBg = RGB(43, 43, 43);
        t.text = RGB(255, 255, 255);
        t.sub = RGB(190, 190, 190);
        t.btn = RGB(52, 52, 52);
        t.btnHot = RGB(62, 62, 62);
        t.btnDown = RGB(45, 45, 45);
        t.border = RGB(72, 72, 72);
    } else {
        t.bg = RGB(243, 243, 243);
        t.editBg = RGB(255, 255, 255);
        t.text = RGB(26, 26, 26);
        t.sub = RGB(96, 96, 96);
        t.btn = RGB(253, 253, 253);
        t.btnHot = RGB(249, 249, 249);
        t.btnDown = RGB(245, 245, 245);
        t.border = RGB(219, 219, 219);
    }

    COLORREF accent = RGB(0, 120, 212);
    DWORD c = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&c, &opaque)))
        accent = RGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
    int lum = (GetRValue(accent) * 299 + GetGValue(accent) * 587 + GetBValue(accent) * 114) / 1000;
    COLORREF towards = lum < 128 ? RGB(255, 255, 255) : RGB(0, 0, 0);
    t.accent = accent;
    t.accentHot = MixColor(accent, towards, 12);
    t.accentDown = MixColor(accent, towards, 24);
    t.accentText = lum < 150 ? RGB(255, 255, 255) : RGB(0, 0, 0);
    return t;
}

typedef BOOL (WINAPI *SpiForDpi_t)(UINT, UINT, PVOID, UINT, UINT);
typedef BOOL (WINAPI *AdjustRectDpi_t)(LPRECT, DWORD, BOOL, DWORD, UINT);
typedef HANDLE (WINAPI *SetThreadDpiCtx_t)(HANDLE);
typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);

static UINT DpiForPoint(POINT pt) {
    static HMODULE shcore = LoadLibraryW(L"shcore.dll");
    auto fn = shcore ? (GetDpiForMonitor_t)GetProcAddress(shcore, "GetDpiForMonitor") : nullptr;
    UINT x = 96, y = 96;
    if (fn && SUCCEEDED(fn(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), 0, &x, &y)) && x) return x;
    return 96;
}

static void MakeFonts(Fonts& f, UINT dpi) {
    NONCLIENTMETRICSW ncm = {};
    ncm.cbSize = sizeof(ncm);
    bool ok = false;
    auto spi = (SpiForDpi_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SystemParametersInfoForDpi");
    if (spi) ok = spi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi) != 0;
    if (!ok) ok = SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0) != 0;

    LOGFONTW lf = ncm.lfMessageFont;
    if (!ok || !lf.lfHeight) {
        lf = {};
        wcscpy_s(lf.lfFaceName, L"Segoe UI");
        lf.lfHeight = -MulDiv(9, (int)dpi, 72);
    }
    lf.lfQuality = CLEARTYPE_QUALITY;
    lf.lfWeight = FW_NORMAL;
    f.base = CreateFontIndirectW(&lf);

    LOGFONTW semi = lf;
    semi.lfWeight = FW_SEMIBOLD;
    f.semi = CreateFontIndirectW(&semi);

    LOGFONTW head = lf;
    head.lfHeight = lf.lfHeight * 6 / 5;
    head.lfWeight = FW_SEMIBOLD;
    f.head = CreateFontIndirectW(&head);
}

static void DeleteFonts(Fonts& f) {
    if (f.base) DeleteObject(f.base);
    if (f.semi) DeleteObject(f.semi);
    if (f.head) DeleteObject(f.head);
    f = Fonts();
}

static PromptBtn* FindBtn(PromptState& s, int id) {
    for (auto& b : s.btns)
        if (b.id == id) return &b;
    return nullptr;
}

static int MeasureText(HDC dc, HFONT f, const std::wstring& t, int w) {
    HGDIOBJ old = SelectObject(dc, f);
    RECT r = {0, 0, w, 0};
    DrawTextW(dc, t.c_str(), -1, &r, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
    SelectObject(dc, old);
    return r.bottom - r.top;
}

static void LayoutPrompt(HWND hwnd, PromptState& s) {
    HDC dc = GetDC(hwnd);
    auto S = [&](int v) { return MulDiv(v, (int)s.dpi, 96); };
    const int m = S(20);
    const int W = S(420);
    const int inner = W - 2 * m;
    int y = S(18);

    int h = MeasureText(dc, s.fonts.head, s.instruction, inner);
    s.rInstr = {m, y, m + inner, y + h};
    y += h + S(6);

    if (!s.content.empty()) {
        h = MeasureText(dc, s.fonts.base, s.content, inner);
        s.rContent = {m, y, m + inner, y + h};
        y += h + S(10);
    } else {
        s.rContent = {0, 0, 0, 0};
        y += S(4);
    }

    if (PromptBtn* t = FindBtn(s, ID_TOGGLE)) {
        t->title = s.expanded ? L"Hide details" : L"Details";
        t->rc = {m, y, m + inner, y + S(26)};
        y += S(26) + S(4);
        if (s.expanded) {
            s.rEdit = {m, y, m + inner, y + S(84)};
            if (s.edit) {
                MoveWindow(s.edit, s.rEdit.left + 1, s.rEdit.top + 1,
                           (s.rEdit.right - s.rEdit.left) - 2, (s.rEdit.bottom - s.rEdit.top) - 2, TRUE);
                ShowWindow(s.edit, SW_SHOW);
            }
            y += S(84) + S(10);
        } else {
            if (s.edit) ShowWindow(s.edit, SW_HIDE);
            s.rEdit = {0, 0, 0, 0};
            y += S(4);
        }
    } else {
        y += S(4);
    }

    for (int id : {ID_PRIMARY, ID_SECONDARY}) {
        if (PromptBtn* b = FindBtn(s, id)) {
            int bh = b->note.empty() ? S(32) : S(50);
            b->rc = {m, y, m + inner, y + bh};
            y += bh + S(6);
        }
    }

    y += S(4);
    int cw = S(100), bh = S(30);
    if (PromptBtn* c = FindBtn(s, ID_CANCEL)) {
        c->rc = {m + inner - cw, y, m + inner, y + bh};
    }
    y += bh + S(16);

    s.clientW = W;
    s.clientH = y;
    ReleaseDC(hwnd, dc);
}

static void ResizePrompt(HWND hwnd, PromptState& s, bool center) {
    RECT rc = {0, 0, s.clientW, s.clientH};
    DWORD style = (DWORD)GetWindowLongW(hwnd, GWL_STYLE);
    DWORD ex = (DWORD)GetWindowLongW(hwnd, GWL_EXSTYLE);
    auto adj = (AdjustRectDpi_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "AdjustWindowRectExForDpi");
    if (adj) adj(&rc, style, FALSE, ex, s.dpi);
    else AdjustWindowRectEx(&rc, style, FALSE, ex);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;

    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
    int x = 0, y = 0;
    if (center) {
        POINT pt = {};
        GetCursorPos(&pt);
        MONITORINFO mi = {sizeof(mi)};
        if (GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi)) {
            x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - w) / 2;
            y = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - h) / 2;
            if (y < mi.rcWork.top) y = mi.rcWork.top;
        }
    } else {
        flags |= SWP_NOMOVE;
    }
    SetWindowPos(hwnd, nullptr, x, y, w, h, flags);
}

static void FillRound(HDC dc, const RECT& rc, COLORREF fill, COLORREF edge, int radius) {
    HBRUSH br = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, edge);
    HGDIOBJ ob = SelectObject(dc, br);
    HGDIOBJ op = SelectObject(dc, pen);
    RoundRect(dc, rc.left, rc.top, rc.right, rc.bottom, radius * 2, radius * 2);
    SelectObject(dc, ob);
    SelectObject(dc, op);
    DeleteObject(br);
    DeleteObject(pen);
}

static void DrawPromptButton(HDC dc, const PromptState& s, size_t idx) {
    const PromptBtn& b = s.btns[idx];
    const Theme& th = s.th;
    auto S = [&](int v) { return MulDiv(v, (int)s.dpi, 96); };
    bool hot = (int)idx == s.hot;
    bool down = hot && (int)idx == s.down;
    bool ring = s.kbdActive && (int)idx == s.kbd;
    RECT rc = b.rc;

    if (b.id == ID_TOGGLE) {
        if (hot) FillRound(dc, rc, th.btnHot, th.btnHot, S(4));

        int cx = rc.left + S(14), cy = (rc.top + rc.bottom) / 2;
        POINT pts[3];
        if (s.expanded) {
            pts[0] = {cx - S(4), cy - S(2)};
            pts[1] = {cx, cy + S(2)};
            pts[2] = {cx + S(4), cy - S(2)};
        } else {
            pts[0] = {cx - S(2), cy - S(4)};
            pts[1] = {cx + S(2), cy};
            pts[2] = {cx - S(2), cy + S(4)};
        }
        HPEN pen = CreatePen(PS_SOLID, std::max(1, S(1)), th.text);
        HGDIOBJ op = SelectObject(dc, pen);
        Polyline(dc, pts, 3);
        SelectObject(dc, op);
        DeleteObject(pen);

        HGDIOBJ of = SelectObject(dc, s.fonts.base);
        SetTextColor(dc, th.text);
        RECT tr = {rc.left + S(30), rc.top, rc.right, rc.bottom};
        DrawTextW(dc, b.title.c_str(), -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
        SelectObject(dc, of);

        if (ring) {
            RECT fr = rc;
            InflateRect(&fr, -S(1), -S(1));
            DrawFocusRect(dc, &fr);
        }
        return;
    }

    COLORREF fill, edge, tcol, scol;
    if (b.primary) {
        fill = down ? th.accentDown : hot ? th.accentHot : th.accent;
        edge = fill;
        tcol = scol = th.accentText;
    } else {
        fill = down ? th.btnDown : hot ? th.btnHot : th.btn;
        edge = th.border;
        tcol = th.text;
        scol = th.sub;
    }
    FillRound(dc, rc, fill, edge, S(4));

    int h = rc.bottom - rc.top;
    if (b.note.empty()) {
        HGDIOBJ of = SelectObject(dc, s.fonts.base);
        SetTextColor(dc, tcol);
        RECT tr = {rc.left + S(8), rc.top, rc.right - S(8), rc.bottom};
        DrawTextW(dc, b.title.c_str(), -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
        SelectObject(dc, of);
    } else {
        TEXTMETRICW tm1 = {}, tm2 = {};
        HGDIOBJ of = SelectObject(dc, s.fonts.semi);
        GetTextMetricsW(dc, &tm1);
        SelectObject(dc, s.fonts.base);
        GetTextMetricsW(dc, &tm2);
        int total = tm1.tmHeight + S(2) + tm2.tmHeight;
        int top = rc.top + (h - total) / 2;

        RECT r1 = {rc.left + S(16), top, rc.right - S(12), top + tm1.tmHeight};
        RECT r2 = {rc.left + S(16), top + tm1.tmHeight + S(2), rc.right - S(12), top + total};

        SelectObject(dc, s.fonts.semi);
        SetTextColor(dc, tcol);
        DrawTextW(dc, b.title.c_str(), -1, &r1, DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
        SelectObject(dc, s.fonts.base);
        SetTextColor(dc, scol);
        DrawTextW(dc, b.note.c_str(), -1, &r2, DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
        SelectObject(dc, of);
    }

    if (ring) {
        RECT fr = rc;
        InflateRect(&fr, -S(3), -S(3));
        HPEN pen = CreatePen(PS_SOLID, std::max(1, S(2)), b.primary ? th.accentText : th.text);
        HGDIOBJ op = SelectObject(dc, pen);
        HGDIOBJ ob = SelectObject(dc, GetStockObject(NULL_BRUSH));
        RoundRect(dc, fr.left, fr.top, fr.right, fr.bottom, S(6), S(6));
        SelectObject(dc, ob);
        SelectObject(dc, op);
        DeleteObject(pen);
    }
}

static std::wstring ToCrLf(const std::wstring& t) {
    std::wstring out;
    for (wchar_t c : t) {
        if (c == L'\n') out += L"\r\n";
        else out += c;
    }
    return out;
}

static void ActivatePrompt(HWND hwnd, PromptState& s, int idx) {
    if (idx < 0 || idx >= (int)s.btns.size()) return;
    switch (s.btns[idx].id) {
        case ID_TOGGLE:
            s.expanded = !s.expanded;
            LayoutPrompt(hwnd, s);
            ResizePrompt(hwnd, s, false);
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        case ID_PRIMARY:
            s.result = CHOICE_CLOSE_CONTINUE;
            DestroyWindow(hwnd);
            break;
        case ID_SECONDARY:
            s.result = CHOICE_CLOSE_ONLY;
            DestroyWindow(hwnd);
            break;
        case ID_CANCEL:
            s.result = CHOICE_CANCEL;
            DestroyWindow(hwnd);
            break;
    }
}

static int PromptHitTest(const PromptState& s, int x, int y) {
    POINT p = {x, y};
    for (size_t i = 0; i < s.btns.size(); i++)
        if (PtInRect(&s.btns[i].rc, p)) return (int)i;
    return -1;
}

static LRESULT CALLBACK PromptProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCCREATE) {
        auto* cs = (CREATESTRUCTW*)lp;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    PromptState* s = (PromptState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (!s) return DefWindowProcW(hwnd, msg, wp, lp);

    switch (msg) {
        case WM_CREATE: {
            if (!s->details.empty()) {
                std::wstring text = ToCrLf(s->details);
                s->edit = CreateWindowExW(0, L"EDIT", text.c_str(),
                                          WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                                          0, 0, 10, 10, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
                if (s->edit) {
                    SendMessageW(s->edit, WM_SETFONT, (WPARAM)s->fonts.base, FALSE);
                    int pad = MulDiv(8, (int)s->dpi, 96);
                    SendMessageW(s->edit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(pad, pad));
                    SetWindowTheme(s->edit, s->th.dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
                }
            }
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wp;
            SetTextColor(hdc, s->th.text);
            SetBkColor(hdc, s->th.editBg);
            return (LRESULT)s->editBrush;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT cr;
            GetClientRect(hwnd, &cr);
            HDC mdc = CreateCompatibleDC(hdc);
            HBITMAP bmp = CreateCompatibleBitmap(hdc, cr.right, cr.bottom);
            HGDIOBJ ob = SelectObject(mdc, bmp);

            FillRect(mdc, &cr, s->bgBrush);
            SetBkMode(mdc, TRANSPARENT);
            SetTextColor(mdc, s->th.text);

            HGDIOBJ of = SelectObject(mdc, s->fonts.head);
            DrawTextW(mdc, s->instruction.c_str(), -1, &s->rInstr, DT_WORDBREAK | DT_NOPREFIX);
            if (!s->content.empty()) {
                SelectObject(mdc, s->fonts.base);
                DrawTextW(mdc, s->content.c_str(), -1, &s->rContent, DT_WORDBREAK | DT_NOPREFIX);
            }
            SelectObject(mdc, of);

            if (s->expanded && !s->details.empty()) {
                HBRUSH fb = CreateSolidBrush(s->th.border);
                FrameRect(mdc, &s->rEdit, fb);
                DeleteObject(fb);
            }
            for (size_t i = 0; i < s->btns.size(); i++) DrawPromptButton(mdc, *s, i);

            BitBlt(hdc, 0, 0, cr.right, cr.bottom, mdc, 0, 0, SRCCOPY);
            SelectObject(mdc, ob);
            DeleteObject(bmp);
            DeleteDC(mdc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!s->tracking) {
                TRACKMOUSEEVENT t = {sizeof(t), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&t);
                s->tracking = true;
            }
            int h = PromptHitTest(*s, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            if (h != s->hot) {
                s->hot = h;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            s->tracking = false;
            s->hot = -1;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_SETCURSOR:
            if (LOWORD(lp) == HTCLIENT && s->hot >= 0) {
                SetCursor(LoadCursorW(nullptr, IDC_HAND));
                return TRUE;
            }
            break;

        case WM_LBUTTONDOWN:
            SetFocus(hwnd);
            s->down = PromptHitTest(*s, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            s->kbdActive = false;
            if (s->down >= 0) SetCapture(hwnd);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_LBUTTONUP: {
            if (GetCapture() == hwnd) ReleaseCapture();
            int hit = PromptHitTest(*s, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
            int pressed = s->down;
            s->down = -1;
            InvalidateRect(hwnd, nullptr, FALSE);
            if (pressed >= 0 && pressed == hit) ActivatePrompt(hwnd, *s, hit);
            return 0;
        }

        case WM_KEYDOWN: {
            if (wp == VK_ESCAPE) {
                s->result = CHOICE_CANCEL;
                DestroyWindow(hwnd);
                return 0;
            }
            if (wp == VK_TAB) {
                SetFocus(hwnd);
                int n = (int)s->btns.size();
                if (n > 0) {
                    bool back = GetKeyState(VK_SHIFT) < 0;
                    s->kbdActive = true;
                    s->kbd = ((s->kbd < 0 ? 0 : s->kbd) + (back ? n - 1 : 1)) % n;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }
            if (wp == VK_RETURN || (wp == VK_SPACE && s->kbdActive)) {
                ActivatePrompt(hwnd, *s, s->kbd);
                return 0;
            }
            break;
        }

        case WM_DPICHANGED: {
            s->dpi = HIWORD(wp);
            Fonts nf;
            MakeFonts(nf, s->dpi);
            if (s->edit) {
                SendMessageW(s->edit, WM_SETFONT, (WPARAM)nf.base, TRUE);
                int pad = MulDiv(8, (int)s->dpi, 96);
                SendMessageW(s->edit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(pad, pad));
            }
            DeleteFonts(s->fonts);
            s->fonts = nf;
            LayoutPrompt(hwnd, *s);
            ResizePrompt(hwnd, *s, false);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case WM_CLOSE:
            s->result = CHOICE_CANCEL;
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            s->done = true;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static Choice RunPrompt(PromptState& s) {
    if (g_stop || !g_promptClass) return CHOICE_CANCEL;

    // Per-monitor DPI awareness for this worker thread only, so the window is crisp.
    auto setCtx = (SetThreadDpiCtx_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext");
    HANDLE oldCtx = setCtx ? setCtx((HANDLE)(LONG_PTR)-4 /*PER_MONITOR_AWARE_V2*/) : nullptr;

    POINT pt = {};
    GetCursorPos(&pt);
    s.dpi = DpiForPoint(pt);
    s.th = MakeTheme();
    MakeFonts(s.fonts, s.dpi);
    s.bgBrush = CreateSolidBrush(s.th.bg);
    s.editBrush = CreateSolidBrush(s.th.editBg);

    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST, kPromptClass, kTitle,
                                WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN,
                                0, 0, 100, 100, nullptr, nullptr, GetModuleHandleW(nullptr), &s);
    if (hwnd) {
        BOOL dark = s.th.dark;
        DwmSetWindowAttribute(hwnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark, sizeof(dark));
        COLORREF cap = s.th.bg, txt = s.th.text;
        DwmSetWindowAttribute(hwnd, 35 /*DWMWA_CAPTION_COLOR*/, &cap, sizeof(cap));
        DwmSetWindowAttribute(hwnd, 36 /*DWMWA_TEXT_COLOR*/, &txt, sizeof(txt));

        LayoutPrompt(hwnd, s);
        ResizePrompt(hwnd, s, true);
        ShowWindow(hwnd, SW_SHOWNORMAL);
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);

        MSG msg;
        while (!s.done) {
            DWORD r = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
            if (r == WAIT_OBJECT_0) {
                s.result = CHOICE_CANCEL;
                break;
            }
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    s.done = true;
                    break;
                }
                // Esc / Tab must work even when the details box has the focus.
                if (msg.message == WM_KEYDOWN && (msg.wParam == VK_ESCAPE || msg.wParam == VK_TAB) &&
                    msg.hwnd != hwnd && IsChild(hwnd, msg.hwnd))
                    msg.hwnd = hwnd;
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        if (IsWindow(hwnd)) DestroyWindow(hwnd);
    }

    DeleteFonts(s.fonts);
    if (s.bgBrush) DeleteObject(s.bgBrush);
    if (s.editBrush) DeleteObject(s.editBrush);
    s.bgBrush = s.editBrush = nullptr;
    if (setCtx && oldCtx) setCtx(oldCtx);
    return g_stop ? CHOICE_CANCEL : s.result;
}

static void AddToggle(PromptState& s) {
    if (!s.details.empty()) s.btns.push_back({ID_TOGGLE, L"Details", L"", false, {0, 0, 0, 0}});
}

static void AddCancel(PromptState& s, const wchar_t* text) {
    s.btns.push_back({ID_CANCEL, text, L"", false, {0, 0, 0, 0}});
    s.kbd = (int)s.btns.size() - 1;
}

static void SplitTitleNote(const std::wstring& t, std::wstring& title, std::wstring& note) {
    size_t p = t.find(L'\n');
    title = t.substr(0, p);
    note = p == std::wstring::npos ? L"" : t.substr(p + 1);
}

static void ShowInfo(const std::wstring& instruction, const std::wstring& content,
                     const std::wstring& details, bool /*error*/) {
    if (g_stop) return;
    PromptState s;
    s.instruction = instruction;
    s.content = content;
    s.details = details;
    s.expanded = !details.empty() && Wh_GetIntSetting(L"expandDetails");
    AddToggle(s);
    AddCancel(s, L"OK");
    RunPrompt(s);
}

static Choice ShowChoice(const std::wstring& instruction, const std::wstring& content,
                         const std::wstring& details, const std::wstring& primary,
                         const std::wstring& secondary) {
    if (g_stop) return CHOICE_CANCEL;
    PromptState s;
    s.instruction = instruction;
    s.content = content;
    s.details = details;
    s.expanded = !details.empty() && Wh_GetIntSetting(L"expandDetails");

    AddToggle(s);
    std::wstring title, note;
    SplitTitleNote(primary, title, note);
    s.btns.push_back({ID_PRIMARY, title, note, true, {0, 0, 0, 0}});
    if (!secondary.empty()) {
        SplitTitleNote(secondary, title, note);
        s.btns.push_back({ID_SECONDARY, title, note, false, {0, 0, 0, 0}});
    }
    AddCancel(s, L"Cancel");
    return RunPrompt(s);
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
        // With "Hide extensions for known file types" (the Windows default)
        // the warning shows the name without its extension.
        const WCHAR* ext = PathFindExtensionW(n);
        std::wstring noExt(n, (size_t)(ext - n));
        for (auto& t : texts)
            if (TextMatchesName(t, n) || TextMatchesName(t, noExt)) return true;
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
        Wh_Log(L"Handle table unavailable");
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
    Wh_Log(L"Handle table: %d entries, file type index %d", (int)n, fileType);
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
        for (auto& kv : job->found)
            AddLocker(result, kv.first, kv.second.name, kv.second.h, kv.second.canKill);
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
        if (Wh_GetIntSetting(L"showNoResultDialog"))
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
    Wh_Log(L"Lockers found: %d (handles checked: %d, inaccessible: %d, timed out: %d)",
           (int)lockers.size(), (int)fileHandles, (int)inaccessible, (int)timedOut);

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

        for (auto it = g_handled.begin(); it != g_handled.end();) {
            if (InterruptActive(ua.p, *it)) {
                ++it;
            } else {
                g_seen.erase(*it);
                it = g_handled.erase(it);
            }
        }

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
    WCHAR cls[64] = {};
    GetClassNameW(h, cls, 64);
    if (wcscmp(cls, kPromptClass) == 0) PostMessageW(h, WM_CLOSE, 0, 0);
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

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = PromptProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kPromptClass;
    g_promptClass = RegisterClassExW(&wc);
    if (!g_promptClass) {
        Wh_Log(L"RegisterClassEx failed");
        return FALSE;
    }

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
    if (g_promptClass) {
        UnregisterClassW(kPromptClass, GetModuleHandleW(nullptr));
        g_promptClass = 0;
    }
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
