// ==WindhawkMod==
// @id              explorer-ctrlfn-newfolder
// @name            Explorer Ctrl+F+N to New Folder
// @description     Ctrl+F+N makes a new folder and names it - inline rename (native) or a popup box, one setting. Batch: <name> 1..N.
// @version         0.6.0
// @author          Ashix
// @github          https://github.com/k-ashix
// @twitter         https://x.com/k_ashix
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lshell32 -luser32 -luuid -luiautomationcore -lgdi32 -ladvapi32
// ==/WindhawkMod==

// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# 📁 Explorer Ctrl+F+N -> New Folder

⚡ Make a new folder in File Explorer without touching the mouse. 🖱️ Press the
shortcut and a new folder appears right where you are browsing, already
selected so you can type its name and press Enter.

✅ Works in any filesystem Explorer window, tab, or sub-folder - and on the
Desktop too. (Non-filesystem locations like *This PC* or *Quick access*, which
have no folder to create in, are skipped.)

💡 **Why "Ctrl + F + N"?** Ctrl, then **F** for *Folder*, then **N** for *Name*.

## ⚠️ **YOU MUST HOLD CTRL THE WHOLE TIME**
> ## 🔒 **Keep Ctrl held down for the entire shortcut.**
> **If your Ctrl hold breaks at any point, the shortcut will NOT work.**
> Hold **Ctrl**, then tap the other keys (and type any number) **while Ctrl
> stays down**. Let go of Ctrl and the gesture is abandoned.

## ⌨️ How to use
1. 📂 Open File Explorer (or click an empty spot on the Desktop).
2. 👉 Hold **Ctrl**, tap **F**, then tap **N**.
3. ✏️ Type a name and press **Enter**. That's it.

🙅 Not ready to name it? Press **Esc** to keep the default name ("New folder").
Clicking away instead **commits** whatever you have typed so far (just like
Explorer's own rename), so use **Esc** when you want the default.

## ✏️ Two ways to name the folder (pick one in Settings)
🎚️ Toggle **"Name folders with a popup box"**:
- ⌨️ **OFF (default) - inline rename:** the new folder appears already in
  Explorer's rename box - just type and press **Enter**. Feels native.
- 🪟 **ON - popup box:** a clean box appears; type the name and press **Create**.
  More reliable, behaves the same in every folder, and follows your
  **light/dark** Windows theme. (The count still comes from the number you type
  during the shortcut.)

🏷️ Either way: one folder becomes `<name>`, several become `<name> 1`,
`<name> 2`, … skipping any that already exist. Leave the name blank to fall
back to the classic `New folder` / `New folder (2)`.

## 🔢 Make several at once (multi-folder)
🎚️ Turn on **"Multiple folders in one go"** (it is **off by default** - classic
single-folder behavior) and, right after the shortcut, 🔢 type a number then
press **Enter** - e.g. Ctrl+F+N then `5` then Enter makes five folders. ⤵️ No
number (just Enter, or a short pause) makes one; ❌ **Esc** cancels. They are
named `New folder`, `New folder (2)`, `New folder (3)`... 🚫 skipping any that
already exist, and ✏️ the first one opens ready to rename. The cap is **25** by
default and can go up to **100**.

## ⚙️ Pick the shortcut that feels right (Settings)
- ⭐ **Ctrl+F+N** *(default)* - hold Ctrl, tap F, then N. 🔍 Your normal Ctrl+F
  search still works: if you don't press N, the search box just opens as usual.
- 1️⃣ **Ctrl+N** - a single press. ⚠️ Heads up: this takes over Explorer's "open a
  new window" shortcut and uses it for "new folder" instead.
- 🔁 **Ctrl+N+F** - hold Ctrl, tap N, then F. ⚠️ Also takes over the "new window"
  shortcut.

## 🧠 Before you use it - keep in mind
- 🎯 **Only the shortcut you pick above is active - just one at a time.** If you
  fire a *different* chord (e.g. you selected **Ctrl+N+F** but press **Ctrl+F+N**),
  nothing is created: it is simply not the active shortcut. Switch it in
  Settings. (**Ctrl+N** and **Ctrl+N+F** can never both be on - they both start with
  **Ctrl+N** and mean opposite things.)
- 🔒 **Hold Ctrl the whole time, then let go to finish.** Releasing Ctrl with no
  number makes exactly **one** folder; type a number first to make that many.
  **Esc** cancels and creates nothing.
- 🏷️ **"Auto-resolve name clashes" decides what happens when `New folder`
  already exists.** Leave it **on** (default) to get a folder *every* time
  (`New folder (2)`, `(3)`...). Turn it **off** and only the plain `New folder`
  name is ever used - so once that folder exists, pressing the shortcut again
  does **nothing**. If it "only works once", this setting is why - turn it on.
- 🗂️ **Works only when File Explorer or the Desktop is the active window.**
  Everywhere else your keys behave normally.

## 🏷️ Naming clashes
🔀 **Auto-resolve name clashes** (default on) numbers new folders past any that
already exist - `New folder (2)`, `(3)`... 🧹 Turn it off to only ever use the
plain `New folder` and skip a clash instead of numbering it.

🛡️ The shortcut only does something when File Explorer (or the Desktop) is the
window you are using, so it stays out of the way everywhere else.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- trigger: ctrl_f_n
  $name: Trigger shortcut
  $description: >-
    Which keystroke creates a new folder in the active Explorer location.
  $options:
  - ctrl_f_n: Ctrl+F+N - hold Ctrl, tap F, then N (keeps Ctrl+F search)
  - ctrl_n: Ctrl+N - repurposes the New window shortcut
  - ctrl_n_f: Ctrl+N+F - hold Ctrl, tap N, then F (repurposes New window)
- multiFolder: false
  $name: Multiple folders in one go
  $description: >-
    Off by default (classic single-folder behavior). When on, type a number
    right after the trigger and press Enter to create that many folders at once
    (Esc cancels; no number, or a short pause, makes one). When off, the trigger
    always creates a single folder instantly.
- maxFolders: 25
  $name: Max folders per run
  $description: >-
    Most folders one trigger can create. Clamped to the range 1-100; 100 is the
    max cap even if you enter a larger number.
- autoResolveNames: true
  $name: Auto-resolve name clashes
  $description: >-
    When on, a new folder whose name is taken is numbered automatically
    (New folder (2), (3), ...). When off, only the plain "New folder" name is
    used and a clashing folder is skipped instead of numbered.
- useDialog: false
  $name: Name folders with a popup box
  $description: >-
    OFF (default): type the name straight into Explorer's inline rename box -
    feels native. ON: a clean popup box appears - type the name and press
    Create. The popup is the more reliable option and follows your light/dark
    Windows theme. Either way, one folder is "<name>" and several are
    "<name> 1", "<name> 2", ... (the count still comes from the number you type
    during the shortcut).
*/
// ==/WindhawkModSettings==

#include <sdkddkver.h>

#include <windows.h>
#include <string>
#include <cwctype>
#include <vector>
#include <memory>
#include <new>
#include <shlwapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <exdisp.h>
#include <oleauto.h>
#include <KnownFolders.h>
#include <ShlGuid.h>
#include <UIAutomation.h>

// Libraries are linked via @compilerOptions at the top of this file. Windhawk
// compiles with clang, so MSVC-style link pragmas are intentionally omitted.

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#ifndef FOFX_SHOWELEVATIONPROMPT
#define FOFX_SHOWELEVATIONPROMPT 0x00040000
#endif

#ifndef FOFX_NOCOPYHOOKS
#define FOFX_NOCOPYHOOKS 0x00800000
#endif

template <typename T>
static void SafeRelease(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}

static IUIAutomation* g_uia = nullptr;
static volatile LONG g_actionRunning = 0;

// Trigger mode: 0 = Ctrl+F+N (default), 1 = Ctrl+N, 2 = Ctrl+N+F.
static volatile LONG g_triggerMode = 0;

// Multi-folder creation (v0.2.0). Default cap 25; the hard cap 100 can never be
// exceeded even if the user sets maxFolders higher.
static const int kFolderDefaultMax = 25;
static const int kFolderHardCap = 100;
static volatile LONG g_multiFolder = 0;                 // toggle (default off)
static volatile LONG g_maxFolders = kFolderDefaultMax;  // raw maxFolders setting
static volatile LONG g_autoResolveNames = 1;            // number clashes (default on)
static volatile LONG g_useDialog = 0;                   // 0 = inline rename, 1 = popup box

// #5: settings are written from Windhawk's settings-change callback and READ on
// the hook and worker threads. Writes already use InterlockedExchange; a bare
// `g_x` load, however, is an ordinary non-atomic read and is not a defined
// synchronisation with those writes. AtomicGet performs an interlocked (atomic,
// full-barrier) load so every read is paired with the interlocked writes and
// the settings are read consistently across threads. Used for EVERY read of the
// settings globals below -- no raw `g_multiFolder`/`g_maxFolders`/etc. loads.
static inline LONG AtomicGet(volatile LONG* p) {
    return InterlockedCompareExchange(p, 0, 0);
}

// #7: set on unload so a long-running poll (the inline rename watch) can bail
// promptly instead of blocking teardown for up to its full timeout. Read via
// AtomicGet, written via InterlockedExchange.
static volatile LONG g_unloading = 0;

// Set to 1 while a folder action is queued/in flight so only one runs at a
// time. Declared here (before the popup pump that clears it on a stale
// WM_APP+1) and used by the worker post/drain paths. Interlocked-only.
static volatile LONG g_actionPosted = 0;

// #6: deterministic Esc during inline rename. Polling GetAsyncKeyState in a
// 60 ms loop can miss a fast Esc press+release that happens entirely between
// two samples, which then gets misclassified as a focus-lost commit. The
// low-level keyboard hook (which sees EVERY keydown) LATCHES this flag the
// instant Esc is pressed while a rename watch is active, so the watcher can
// observe the cancel even if it never coincides with a poll. Both written via
// Interlocked; read via AtomicGet.
static volatile LONG g_inlineRenameActive = 0;  // a rename watch is running
static volatile LONG g_inlineEscPressed = 0;    // Esc seen during that watch

static void LoadSettings() {
    PCWSTR trigger = Wh_GetStringSetting(L"trigger");

    LONG mode = 0;  // ctrl_f_n
    if (trigger) {
        if (wcscmp(trigger, L"ctrl_n") == 0) {
            mode = 1;
        } else if (wcscmp(trigger, L"ctrl_n_f") == 0) {
            mode = 2;
        } else {
            mode = 0;
        }
        Wh_FreeStringSetting(trigger);
    }

    InterlockedExchange(&g_triggerMode, mode);

    InterlockedExchange(&g_multiFolder,
                        Wh_GetIntSetting(L"multiFolder") != 0 ? 1 : 0);
    InterlockedExchange(&g_maxFolders,
                        static_cast<LONG>(Wh_GetIntSetting(L"maxFolders")));
    InterlockedExchange(&g_autoResolveNames,
                        Wh_GetIntSetting(L"autoResolveNames") != 0 ? 1 : 0);
    InterlockedExchange(&g_useDialog,
                        Wh_GetIntSetting(L"useDialog") != 0 ? 1 : 0);

    // One concise summary per (re)load, so tuning any setting always produces a
    // single, greppable line from the mod itself -- instead of relying only on
    // Windhawk's verbose per-Wh_GetIntSetting engine trace.
    Wh_Log(L"Settings applied: trigger=%d multiFolder=%d maxFolders=%d "
           L"autoResolve=%d useDialog=%d",
           static_cast<int>(AtomicGet(&g_triggerMode)),
           AtomicGet(&g_multiFolder) != 0 ? 1 : 0,
           static_cast<int>(AtomicGet(&g_maxFolders)),
           AtomicGet(&g_autoResolveNames) != 0 ? 1 : 0,
           AtomicGet(&g_useDialog) != 0 ? 1 : 0);
}

static bool EnsureUIA() {
    if (g_uia) {
        return true;
    }

    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation,
                                  nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&g_uia));

    if (FAILED(hr) || !g_uia) {
        Wh_Log(L"CoCreateInstance(CUIAutomation) failed. hr=0x%08X", hr);
        return false;
    }

    return true;
}

// ---------------- Basic helpers ----------------
static std::wstring JoinPath(const std::wstring& a, const std::wstring& b) {
    if (a.empty()) return b;
    if (a.back() == L'\\' || a.back() == L'/') return a + b;
    return a + L'\\' + b;
}

static std::wstring GetFileNamePart(const std::wstring& fullPath) {
    const wchar_t* p = PathFindFileNameW(fullPath.c_str());
    return p ? std::wstring(p) : L"";
}

static std::wstring GetParentDir(const std::wstring& fullPath) {
    wchar_t buf[MAX_PATH * 4] = {0};

    // Guard against paths longer than the fixed buffer: wcscpy_s would fail (and
    // previously its return was ignored, so we'd operate on an empty/garbage
    // buffer). Bail out explicitly instead so an over-long path fails cleanly
    // rather than silently producing a wrong parent.
    if (fullPath.size() >= ARRAYSIZE(buf)) {
        Wh_Log(L"GetParentDir: path too long (%zu chars) for buffer; skipping.",
               fullPath.size());
        return L"";
    }

    if (wcscpy_s(buf, ARRAYSIZE(buf), fullPath.c_str()) != 0) {
        Wh_Log(L"GetParentDir: wcscpy_s failed.");
        return L"";
    }

    if (!PathRemoveFileSpecW(buf)) {
        return L"";
    }

    return buf;
}

// Mirrors tests/newfolder_logic.h::ClampFolderMax.
static int ClampFolderMax(int maxSetting) {
    if (maxSetting < 1) {
        return 1;
    }
    if (maxSetting > kFolderHardCap) {
        return kFolderHardCap;
    }
    return maxSetting;
}

// Mirrors tests/newfolder_logic.h::ResolveFolderCount. Off -> always 1; no digits
// (requested < 1) -> 1; otherwise clamp to [1, ClampFolderMax(maxSetting)].
static int ResolveFolderCount(int requested, bool multiEnabled, int maxSetting) {
    if (!multiEnabled) {
        return 1;
    }

    int effMax = ClampFolderMax(maxSetting);
    if (requested < 1) {
        return 1;
    }
    if (requested > effMax) {
        return effMax;
    }
    return requested;
}

// Mirrors tests/newfolder_logic.h::MakeUniqueFolderNames. Returns `count` paths
// using "New folder", "New folder (2)", ..., reserving each within the batch and
// skipping anything already on disk (so making more never collides or clobbers).
static std::vector<std::wstring> MakeUniqueFolderNames(const std::wstring& dir,
                                                       int count,
                                                       bool autoResolve) {
    std::vector<std::wstring> out;
    if (count < 1) {
        return out;
    }

    // Names are emitted in strictly increasing numeric order and an index we
    // pass is never revisited, so a monotonic cursor replaces the old per-slot
    // rescan-from-(2). That turns O(count * (stat + batch)) -- thousands of
    // PathFileExistsW stats at the 100-folder cap in a busy directory -- into
    // O(count + preexisting) stats, with no per-slot rescan of `out`. Output is
    // byte-identical (multi_name_test.cpp / auto_resolve_test.cpp pin it).
    int nextIndex = 1;  // 1 == bare "New folder"; 2+ == "New folder (N)".

    auto pathForIndex = [&](int i) -> std::wstring {
        if (i <= 1) {
            return JoinPath(dir, L"New folder");
        }
        wchar_t buf[64] = {0};
        swprintf_s(buf, ARRAYSIZE(buf), L"New folder (%d)", i);
        return JoinPath(dir, buf);
    };

    for (int k = 0; k < count; ++k) {
        if (!autoResolve) {
            // Numbering disabled: only the bare "New folder" is ever a
            // candidate, and it can be claimed at most once per batch.
            std::wstring base = pathForIndex(1);
            if (nextIndex <= 1 && !PathFileExistsW(base.c_str())) {
                out.push_back(base);
            }
            nextIndex = 2;
            continue;
        }

        bool placed = false;
        for (; nextIndex < 100000; ++nextIndex) {
            std::wstring candidate = pathForIndex(nextIndex);
            if (!PathFileExistsW(candidate.c_str())) {
                out.push_back(candidate);
                ++nextIndex;  // reserve it; the next slot starts past it
                placed = true;
                break;
            }
        }
        if (!placed) {
            break;
        }
    }

    return out;
}

// ---- Name-capture resolution (mirrors tests/capture_logic.h) ----
// Named batch: "<base> 1", "<base> 2", ... skipping any already on disk.
static std::vector<std::wstring> MakeNamedFolderNames(const std::wstring& dir,
                                                      const std::wstring& base,
                                                      int count) {
    std::vector<std::wstring> out;
    if (count < 1 || base.empty()) {
        return out;
    }
    int nextIndex = 1;
    for (int k = 0; k < count; ++k) {
        bool placed = false;
        for (; nextIndex < 1000000; ++nextIndex) {
            std::wstring candidate =
                JoinPath(dir, base + L" " + std::to_wstring(nextIndex));
            if (!PathFileExistsW(candidate.c_str())) {
                out.push_back(candidate);
                ++nextIndex;
                placed = true;
                break;
            }
        }
        if (!placed) {
            break;
        }
    }
    return out;
}

// Windows-forbidden filename characters.
static bool IsForbiddenNameChar(wchar_t c) {
    return c == L'\\' || c == L'/' || c == L':' || c == L'*' ||
           c == L'?'  || c == L'"' || c == L'<' || c == L'>' || c == L'|';
}

// Sanitize typed input into a folder base: strip control + forbidden chars,
// trim whitespace, strip trailing dots/spaces. May return empty.
static std::wstring SanitizeFolderBase(const std::wstring& raw) {
    std::wstring s;
    s.reserve(raw.size());
    for (wchar_t c : raw) {
        // #12: strip ALL C0 control chars (< 0x20), covering \r \n \t plus every
        // other control char that would otherwise reach CreateDirectoryW and
        // make it fail. Mirrors tests/capture_logic.h::SanitizeFolderBase.
        if (c < 0x20) {
            continue;
        }
        if (IsForbiddenNameChar(c)) {
            continue;
        }
        s.push_back(c);
    }
    size_t b = 0;
    while (b < s.size() && iswspace(s[b])) {
        ++b;
    }
    size_t e = s.size();
    while (e > b && (iswspace(s[e - 1]) || s[e - 1] == L'.')) {
        --e;
    }
    return s.substr(b, e - b);
}

// Single named folder: "<base>", then "<base> (2)", "<base> (3)", ...
// #9: when autoResolve is off, never renumber a clashing custom name -- return
// the bare "<base>" and let the create layer report the collision (matches
// autoResolveNames=off semantics for the default name too). Mirrors
// tests/capture_logic.h::MakeUniqueNamedFolder.
static std::wstring MakeUniqueNamedFolder(const std::wstring& dir,
                                          const std::wstring& base,
                                          bool autoResolve = true) {
    std::wstring first = JoinPath(dir, base);
    if (!autoResolve || !PathFileExistsW(first.c_str())) {
        return first;
    }
    for (int i = 2; i < 100000; ++i) {
        wchar_t buf[24] = {0};
        swprintf_s(buf, ARRAYSIZE(buf), L" (%d)", i);
        std::wstring candidate = JoinPath(dir, base + buf);
        if (!PathFileExistsW(candidate.c_str())) {
            return candidate;
        }
    }
    return first;
}

// Resolve captured base + count into concrete folder paths.
//   empty base        -> default "New folder"/"New folder (N)" flow
//   base + count == 1 -> single "<base>" (with " (2)" fallback)
//   base + count > 1  -> "<base> 1".."<base> N" (skips existing)
static std::vector<std::wstring> ResolveCaptureNames(const std::wstring& dir,
                                                     const std::wstring& rawBase,
                                                     int count,
                                                     bool autoResolve) {
    std::wstring base = SanitizeFolderBase(rawBase);
    if (base.empty()) {
        return MakeUniqueFolderNames(dir, count, autoResolve);
    }
    if (count <= 1) {
        return { MakeUniqueNamedFolder(dir, base, autoResolve) };
    }
    return MakeNamedFolderNames(dir, base, count);
}

static std::wstring GetDesktopDir() {
    PWSTR p = nullptr;

    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &p))) {
        std::wstring ret = p;
        CoTaskMemFree(p);
        return ret;
    }

    wchar_t buf[MAX_PATH] = {0};

    if (SUCCEEDED(SHGetFolderPathW(nullptr,
                                   CSIDL_DESKTOPDIRECTORY,
                                   nullptr,
                                   SHGFP_TYPE_CURRENT,
                                   buf))) {
        return buf;
    }

    return L"";
}

static bool IsPermissionError(DWORD err) {
    return err == ERROR_ACCESS_DENIED ||
           err == ERROR_PRIVILEGE_NOT_HELD ||
           err == ERROR_ELEVATION_REQUIRED;
}

// ---------------- Window helpers ----------------
static bool IsClassName(HWND hwnd, const wchar_t* expected) {
    if (!hwnd || !expected) return false;

    wchar_t cls[128] = {0};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    return wcscmp(cls, expected) == 0;
}

static bool IsExplorerTopLevel(HWND hwnd) {
    return IsClassName(hwnd, L"CabinetWClass") ||
           IsClassName(hwnd, L"ExploreWClass");
}

static bool IsDescendantOrSelf(HWND parent, HWND child) {
    if (!parent || !child) return false;
    if (parent == child) return true;
    return IsChild(parent, child) != FALSE;
}

static HWND FindAncestorOrSelfByClass(HWND hwnd,
                                      const wchar_t* className,
                                      HWND stopAt) {
    for (HWND cur = hwnd; cur; cur = GetParent(cur)) {
        if (IsClassName(cur, className)) {
            return cur;
        }

        if (cur == stopAt) {
            break;
        }
    }

    return nullptr;
}

static HWND HitTestExplorerByCursor() {
    POINT pt = {};
    GetCursorPos(&pt);

    HWND h = WindowFromPoint(pt);
    if (!h) return nullptr;

    HWND root = GetAncestor(h, GA_ROOT);

    if (IsExplorerTopLevel(root)) {
        return root;
    }

    for (HWND cur = h; cur; cur = GetParent(cur)) {
        if (IsExplorerTopLevel(cur)) {
            return cur;
        }
    }

    return nullptr;
}

static HWND GetTargetExplorerHWND() {
    HWND fg = GetForegroundWindow();

    if (fg && (IsExplorerTopLevel(fg) || fg == GetShellWindow())) {
        return fg;
    }

    if (HWND h = HitTestExplorerByCursor()) {
        return h;
    }

    HWND shell = GetShellWindow();

    if (shell) {
        return shell;
    }

    return nullptr;
}

static HWND GetThreadFocusWindow(HWND topLevel) {
    if (!topLevel) return nullptr;

    DWORD tid = GetWindowThreadProcessId(topLevel, nullptr);

    GUITHREADINFO gi = {};
    gi.cbSize = sizeof(gi);

    if (!GetGUIThreadInfo(tid, &gi)) {
        return nullptr;
    }

    if (gi.hwndFocus) return gi.hwndFocus;
    if (gi.hwndActive) return gi.hwndActive;

    return nullptr;
}

struct DefViewSearchCtx {
    HWND found = nullptr;
};

static BOOL CALLBACK FindDefViewEnumProc(HWND child, LPARAM lParam) {
    DefViewSearchCtx* ctx = reinterpret_cast<DefViewSearchCtx*>(lParam);

    if (IsWindowVisible(child) && IsClassName(child, L"SHELLDLL_DefView")) {
        ctx->found = child;
        return FALSE;
    }

    return TRUE;
}

struct ShellTabSearchCtx {
    HWND bestTab = nullptr;
    LONG_PTR bestArea = 0;
};

static BOOL CALLBACK FindShellTabEnumProc(HWND hwnd, LPARAM lParam) {
    ShellTabSearchCtx* ctx = reinterpret_cast<ShellTabSearchCtx*>(lParam);

    if (!IsWindowVisible(hwnd)) return TRUE;
    if (!IsClassName(hwnd, L"ShellTabWindowClass")) return TRUE;

    DefViewSearchCtx defViewCtx;
    EnumChildWindows(hwnd,
                     FindDefViewEnumProc,
                     reinterpret_cast<LPARAM>(&defViewCtx));

    if (!defViewCtx.found) return TRUE;

    RECT rc = {};
    if (!GetWindowRect(defViewCtx.found, &rc)) return TRUE;

    LONG_PTR area =
        static_cast<LONG_PTR>(rc.right - rc.left) *
        static_cast<LONG_PTR>(rc.bottom - rc.top);

    if (area > ctx->bestArea) {
        ctx->bestArea = area;
        ctx->bestTab = hwnd;
    }

    return TRUE;
}

static HWND GetActiveShellTabHwnd(HWND explorerHwnd) {
    if (!explorerHwnd || explorerHwnd == GetShellWindow()) {
        return nullptr;
    }

    HWND focus = GetThreadFocusWindow(explorerHwnd);

    if (focus && IsDescendantOrSelf(explorerHwnd, focus)) {
        HWND tab = FindAncestorOrSelfByClass(focus,
                                             L"ShellTabWindowClass",
                                             explorerHwnd);
        if (tab) {
            return tab;
        }
    }

    // If focus is on the tab bar, command bar, or address bar, it may not be in
    // the ShellTabWindowClass subtree. Fall back to the tab holding the visible
    // SHELLDLL_DefView.
    ShellTabSearchCtx ctx;
    EnumChildWindows(explorerHwnd,
                     FindShellTabEnumProc,
                     reinterpret_cast<LPARAM>(&ctx));

    return ctx.bestTab;
}

static void ForceForeground(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    DWORD targetThread = GetWindowThreadProcessId(hwnd, nullptr);
    DWORD currentThread = GetCurrentThreadId();

    // Only attach when the target lives on another thread: AttachThreadInput to
    // your own thread fails, and we must only detach if the attach succeeded
    // (an unbalanced detach on a failed attach corrupts the input-queue state).
    bool attached = false;
    if (targetThread != currentThread) {
        attached = AttachThreadInput(currentThread, targetThread, TRUE) != FALSE;
    }

    if (!SetForegroundWindow(hwnd)) {
        // Foreground can be legitimately refused (foreground-lock timeout);
        // hardening only -- BringWindowToTop below still raises the window.
        Wh_Log(L"ForceForeground: SetForegroundWindow failed (error=%lu).",
               GetLastError());
    }
    BringWindowToTop(hwnd);

    if (attached) {
        AttachThreadInput(currentThread, targetThread, FALSE);
    }
}

static void SendSimpleKey(WORD key) {
    INPUT inputs[2] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT sent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (sent != ARRAYSIZE(inputs)) {
        // Input can be blocked (e.g. UIPI / a secure desktop). Hardening only:
        // there is no safe retry from here, so log and move on.
        Wh_Log(L"SendSimpleKey: SendInput sent %u/%u events (error=%lu).",
               sent, static_cast<UINT>(ARRAYSIZE(inputs)), GetLastError());
    }
}

static void WaitForModifierKeysReleased() {
    for (int i = 0; i < 40; ++i) {
        bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool altDown = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool nDown = (GetAsyncKeyState('N') & 0x8000) != 0;
        bool fDown = (GetAsyncKeyState('F') & 0x8000) != 0;

        if (!ctrlDown && !altDown && !shiftDown && !nDown && !fDown) {
            return;
        }

        Sleep(5);
    }
}

// ---------------- ShellView resolve for rename ----------------
static bool GetPathFromShellView(IShellView* view, std::wstring& outPath) {
    outPath.clear();

    if (!view) return false;

    IFolderView* folderView = nullptr;
    HRESULT hr = view->QueryInterface(IID_PPV_ARGS(&folderView));

    if (FAILED(hr) || !folderView) {
        return false;
    }

    IPersistFolder2* persistFolder = nullptr;
    hr = folderView->GetFolder(IID_PPV_ARGS(&persistFolder));
    SafeRelease(folderView);

    if (FAILED(hr) || !persistFolder) {
        return false;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    hr = persistFolder->GetCurFolder(&pidl);
    SafeRelease(persistFolder);

    if (FAILED(hr) || !pidl) {
        return false;
    }

    wchar_t path[MAX_PATH * 4] = {0};
    bool ok = SHGetPathFromIDListEx(pidl,
                                    path,
                                    ARRAYSIZE(path),
                                    GPFIDL_DEFAULT) &&
              path[0] != L'\0';

    CoTaskMemFree(pidl);

    if (ok) {
        outPath = path;
        return true;
    }

    return false;
}

static bool ResolveActiveShellView(HWND explorerHwnd,
                                   std::wstring& outPath,
                                   IShellView** outView) {
    if (!outView) return false;
    outPath.clear();
    *outView = nullptr;

    if (!explorerHwnd || !IsWindow(explorerHwnd)) {
        return false;
    }

    HWND activeTab = GetActiveShellTabHwnd(explorerHwnd);

    IShellWindows* shellWindows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows,
                                  nullptr,
                                  CLSCTX_ALL,
                                  IID_PPV_ARGS(&shellWindows));

    if (FAILED(hr) || !shellWindows) {
        return false;
    }

    long count = 0;
    shellWindows->get_Count(&count);

    for (long i = count - 1; i >= 0; --i) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;

        IDispatch* dispatch = nullptr;

        if (FAILED(shellWindows->Item(v, &dispatch)) || !dispatch) {
            VariantClear(&v);
            continue;
        }

        IServiceProvider* serviceProvider = nullptr;
        hr = dispatch->QueryInterface(IID_PPV_ARGS(&serviceProvider));
        SafeRelease(dispatch);

        if (FAILED(hr) || !serviceProvider) {
            VariantClear(&v);
            continue;
        }

        IShellBrowser* browser = nullptr;
        hr = serviceProvider->QueryService(SID_STopLevelBrowser,
                                           IID_PPV_ARGS(&browser));
        SafeRelease(serviceProvider);

        if (FAILED(hr) || !browser) {
            VariantClear(&v);
            continue;
        }

        IShellView* view = nullptr;
        hr = browser->QueryActiveShellView(&view);
        SafeRelease(browser);

        if (FAILED(hr) || !view) {
            VariantClear(&v);
            continue;
        }

        HWND viewHwnd = nullptr;
        hr = view->GetWindow(&viewHwnd);

        if (FAILED(hr) || !viewHwnd) {
            SafeRelease(view);
            VariantClear(&v);
            continue;
        }

        HWND viewRoot = GetAncestor(viewHwnd, GA_ROOT);
        if (viewRoot != explorerHwnd) {
            SafeRelease(view);
            VariantClear(&v);
            continue;
        }

        if (activeTab) {
            HWND viewTab = FindAncestorOrSelfByClass(viewHwnd,
                                                     L"ShellTabWindowClass",
                                                     explorerHwnd);
            if (viewTab != activeTab) {
                SafeRelease(view);
                VariantClear(&v);
                continue;
            }
        }

        std::wstring viewPath;
        if (GetPathFromShellView(view, viewPath)) {
            outPath = viewPath;
            *outView = view;
            shellWindows->Release();
            VariantClear(&v);
            return true;
        }

        SafeRelease(view);
        VariantClear(&v);
    }

    shellWindows->Release();
    return false;
}

// ---------------- PIDL / rename ----------------
static bool BindParentAndChildPIDL(const std::wstring& fullPath,
                                   PIDLIST_ABSOLUTE* outFullPidl,
                                   PCUITEMID_CHILD* outChild) {
    if (!outFullPidl || !outChild) return false;

    *outFullPidl = nullptr;
    *outChild = nullptr;

    PIDLIST_ABSOLUTE pidl = nullptr;
    SFGAOF sf = 0;

    HRESULT hr = SHParseDisplayName(fullPath.c_str(),
                                    nullptr,
                                    &pidl,
                                    0,
                                    &sf);

    if (FAILED(hr) || !pidl) {
        return false;
    }

    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;

    hr = SHBindToParent(pidl, IID_PPV_ARGS(&parent), &child);

    if (FAILED(hr) || !parent || !child) {
        if (parent) parent->Release();
        CoTaskMemFree(pidl);
        return false;
    }

    parent->Release();

    *outFullPidl = pidl;
    *outChild = child;
    return true;
}

static bool RenameViaShellView(IShellView* shellView,
                               const std::wstring& fullPath) {
    if (!shellView) {
        return false;
    }

    ULONGLONG t0 = GetTickCount64();

    PIDLIST_ABSOLUTE fullPidl = nullptr;
    PCUITEMID_CHILD child = nullptr;

    if (!BindParentAndChildPIDL(fullPath, &fullPidl, &child)) {
        Wh_Log(L"BindParentAndChildPIDL failed, cost=%llums",
               GetTickCount64() - t0);
        return false;
    }

    HRESULT hr = shellView->SelectItem(child,
                                       SVSI_SELECT |
                                       SVSI_ENSUREVISIBLE |
                                       SVSI_DESELECTOTHERS |
                                       SVSI_EDIT);

    CoTaskMemFree(fullPidl);

    if (SUCCEEDED(hr)) {
        Wh_Log(L"Rename via IShellView succeeded, cost=%llums",
               GetTickCount64() - t0);
        return true;
    }

    Wh_Log(L"IShellView::SelectItem failed. hr=0x%08X cost=%llums",
           hr,
           GetTickCount64() - t0);
    return false;
}

// ---------------- Folder creation ----------------
// #5: a check-then-create (PathFileExistsW then CreateDirectoryW) is not atomic
// -- another process can win the name in between, so CreateDirectoryW fails with
// ERROR_ALREADY_EXISTS. That is a collision to retry (advance to the next
// candidate name), NOT a hard failure. Mirrors
// tests/newfolder_logic.h::ShouldRetryOnCollision.
static bool ShouldRetryOnCollision(DWORD createError) {
    return createError == ERROR_ALREADY_EXISTS;
}

static bool CreateFolderDirect(const std::wstring& fullPath,
                               DWORD* pError = nullptr) {
    if (pError) {
        *pError = ERROR_SUCCESS;
    }

    if (CreateDirectoryW(fullPath.c_str(), nullptr)) {
        return true;
    }

    if (pError) {
        *pError = GetLastError();
    }
    return false;
}

static bool CreateFolderWithShellElevation(const std::wstring& fullPath,
                                           HWND owner) {
    std::wstring dir = GetParentDir(fullPath);
    std::wstring name = GetFileNamePart(fullPath);

    if (dir.empty() || name.empty()) return false;

    IShellItem* folderItem = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(dir.c_str(),
                                             nullptr,
                                             IID_PPV_ARGS(&folderItem));

    if (FAILED(hr) || !folderItem) {
        Wh_Log(L"SHCreateItemFromParsingName failed. hr=0x%08X dir=%ls",
               hr,
               dir.c_str());
        return false;
    }

    IFileOperation* fileOp = nullptr;
    hr = CoCreateInstance(CLSID_FileOperation,
                          nullptr,
                          CLSCTX_ALL,
                          IID_PPV_ARGS(&fileOp));

    if (FAILED(hr) || !fileOp) {
        SafeRelease(folderItem);
        Wh_Log(L"CoCreateInstance(CLSID_FileOperation) failed. hr=0x%08X",
               hr);
        return false;
    }

    if (owner && IsWindow(owner)) {
        fileOp->SetOwnerWindow(owner);
    }

    DWORD flags =
        FOF_NOCONFIRMMKDIR |
        FOFX_SHOWELEVATIONPROMPT |
        FOFX_NOCOPYHOOKS;

    fileOp->SetOperationFlags(flags);

    hr = fileOp->NewItem(folderItem,
                         FILE_ATTRIBUTE_DIRECTORY,
                         name.c_str(),
                         nullptr,
                         nullptr);

    if (SUCCEEDED(hr)) {
        hr = fileOp->PerformOperations();
    }

    BOOL aborted = FALSE;
    fileOp->GetAnyOperationsAborted(&aborted);

    SafeRelease(fileOp);
    SafeRelease(folderItem);

    if (FAILED(hr) || aborted) {
        Wh_Log(L"IFileOperation::NewItem failed. hr=0x%08X aborted=%d path=%ls",
               hr,
               aborted,
               fullPath.c_str());
        return false;
    }

    return PathFileExistsW(fullPath.c_str()) != FALSE;
}

// ---------------- UIA rename fallback ----------------
static IUIAutomationCondition* UIA_MakeExactNameCondition(const std::wstring& name) {
    if (!EnsureUIA()) return nullptr;

    VARIANT vName;
    VariantInit(&vName);
    vName.vt = VT_BSTR;
    vName.bstrVal = SysAllocString(name.c_str());

    IUIAutomationCondition* nameCond = nullptr;
    g_uia->CreatePropertyCondition(UIA_NamePropertyId,
                                   vName,
                                   &nameCond);

    VariantClear(&vName);
    return nameCond;
}

static bool UIA_SelectItemByNameFallback(HWND explorerHwnd,
                                         const std::wstring& fileName) {
    if (!explorerHwnd || !IsWindow(explorerHwnd) || fileName.empty()) {
        return false;
    }

    if (!EnsureUIA()) {
        return false;
    }

    IUIAutomationElement* root = nullptr;
    HRESULT hr = g_uia->ElementFromHandle(explorerHwnd, &root);

    if (FAILED(hr) || !root) {
        return false;
    }

    IUIAutomationCondition* nameCond = UIA_MakeExactNameCondition(fileName);
    if (!nameCond) {
        SafeRelease(root);
        return false;
    }

    IUIAutomationElement* item = nullptr;
    hr = root->FindFirst(TreeScope_Descendants, nameCond, &item);

    SafeRelease(nameCond);
    SafeRelease(root);

    if (FAILED(hr) || !item) {
        return false;
    }

    bool ok = false;

    IUnknown* unkPattern = nullptr;
    hr = item->GetCurrentPattern(UIA_SelectionItemPatternId, &unkPattern);

    if (SUCCEEDED(hr) && unkPattern) {
        IUIAutomationSelectionItemPattern* sel = nullptr;

        if (SUCCEEDED(unkPattern->QueryInterface(IID_PPV_ARGS(&sel))) && sel) {
            ok = SUCCEEDED(sel->Select());
            SafeRelease(sel);
        }

        SafeRelease(unkPattern);
    }

    if (!ok) {
        ok = SUCCEEDED(item->SetFocus());
    } else {
        item->SetFocus();
    }

    SafeRelease(item);
    return ok;
}

// #6: arm the hook-thread Esc latch. MUST be called BEFORE StartRename so the
// hook is already recording Esc when the rename edit first appears -- closing
// the tiny pre-arm window where an extremely fast Esc pressed between
// StartRename returning and the watch loop starting would otherwise be missed.
// Clears any stale press, then marks the rename active.
static void ArmInlineEscLatch() {
    InterlockedExchange(&g_inlineEscPressed, 0);
    InterlockedExchange(&g_inlineRenameActive, 1);
}

static void StartRename(HWND explorerHwnd,
                        IShellView* shellView,
                        const std::wstring& fullPath,
                        const std::wstring& folderPath,
                        bool elevated) {
    std::wstring fileName = GetFileNamePart(fullPath);

    if (fileName.empty()) return;

    SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, fullPath.c_str(), nullptr);
    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, folderPath.c_str(), nullptr);

    ULONGLONG t0 = GetTickCount64();

    if (shellView) {
        bool ok = RenameViaShellView(shellView, fullPath);

        if (ok) {
            return;
        }
    } else {
        Wh_Log(L"Active ShellView for rename not available, cost=%llums",
               GetTickCount64() - t0);
    }

    ULONGLONG fallbackStart = GetTickCount64();
    const int maxWaitMs = elevated ? 900 : 250;
    const int intervalMs = 10;
    const int maxTries = maxWaitMs / intervalMs;

    bool selected = false;

    for (int i = 0; i < maxTries; ++i) {
        selected = UIA_SelectItemByNameFallback(explorerHwnd, fileName);

        if (selected) {
            break;
        }

        Sleep(intervalMs);
    }

    if (!selected) {
        Wh_Log(L"Fallback UIA failed to select item: %ls, cost=%llums",
               fileName.c_str(),
               GetTickCount64() - fallbackStart);
        return;
    }

    WaitForModifierKeysReleased();
    ForceForeground(explorerHwnd);
    SendSimpleKey(VK_F2);

    Wh_Log(L"Rename via fallback UIA+F2, cost=%llums",
           GetTickCount64() - fallbackStart);
}

// ---------------- Inline rename capture (inline) ----------------
// Read the text of the focused inline-rename EDIT box via UIA. Returns true and
// fills outText only while an EDIT control that BELONGS TO `owner` is focused.
//
// #6: the focused EDIT must be scoped to the originating Explorer window. If we
// only checked "focused control is an EDIT", then when focus drifts to another
// application mid-rename that app's EDIT text would be captured as the folder
// name. We verify the EDIT's native window is `owner` or a descendant of it and
// reject anything else.
static bool UIA_ReadFocusedEditText(std::wstring& outText, HWND owner) {
    if (!EnsureUIA()) {
        return false;
    }
    IUIAutomationElement* focused = nullptr;
    if (FAILED(g_uia->GetFocusedElement(&focused)) || !focused) {
        return false;
    }
    CONTROLTYPEID ct = 0;
    bool isEdit = SUCCEEDED(focused->get_CurrentControlType(&ct)) &&
                  ct == UIA_EditControlTypeId;
    if (!isEdit) {
        SafeRelease(focused);
        return false;
    }
    // #6: scope the EDIT to the originating Explorer window. Reject a focused
    // EDIT that lives in some other top-level window (another app that grabbed
    // focus during the rename).
    if (owner) {
        UIA_HWND editHwnd = nullptr;
        HWND edit = SUCCEEDED(focused->get_CurrentNativeWindowHandle(&editHwnd))
                        ? reinterpret_cast<HWND>(editHwnd)
                        : nullptr;
        bool belongs = edit && (GetAncestor(edit, GA_ROOT) ==
                                    GetAncestor(owner, GA_ROOT) ||
                                IsDescendantOrSelf(owner, edit));
        if (!belongs) {
            SafeRelease(focused);
            return false;
        }
    }
    bool got = false;
    IUnknown* unk = nullptr;
    if (SUCCEEDED(focused->GetCurrentPattern(UIA_ValuePatternId, &unk)) && unk) {
        IUIAutomationValuePattern* vp = nullptr;
        if (SUCCEEDED(unk->QueryInterface(IID_PPV_ARGS(&vp))) && vp) {
            BSTR bstr = nullptr;
            if (SUCCEEDED(vp->get_CurrentValue(&bstr)) && bstr) {
                outText.assign(bstr, SysStringLen(bstr));
                got = true;
                SysFreeString(bstr);
            }
            SafeRelease(vp);
        }
        SafeRelease(unk);
    }
    SafeRelease(focused);
    return got;
}

// Watch the inline rename until the EDIT box closes (Enter / focus loss) or we
// time out. outBase receives the last text seen. Returns true if an edit box
// was ever observed.
// Terminal outcome of watching the inline rename EDIT. Mirrors RenameOutcome in
// tests/capture_logic.h and is gated by ShouldUseCapturedName() (see
// tests/rename_outcome_test): only a real commit may create folders. Do NOT
// infer commit from "the EDIT disappeared" -- Esc dismisses it exactly like
// Enter, and treating that as a commit is the multi-folder "Esc still creates"
// regression.
enum class InlineRenameOutcome {
    NeverStarted,        // the rename EDIT never appeared
    Committed,           // user pressed Enter -> use the typed name
    FocusLostCommitted,  // focus left the EDIT with text intact -> commit
    Cancelled,           // user pressed Esc -> discard, cancel
    Timeout,             // watcher timed out -> discard, cancel
};

// True only when the captured name may be used to create folders (mirror of
// ShouldUseCapturedName in tests/capture_logic.h).
static bool ShouldUseCapturedName(InlineRenameOutcome outcome) {
    return outcome == InlineRenameOutcome::Committed ||
           outcome == InlineRenameOutcome::FocusLostCommitted;
}

// Watch the inline rename EDIT and report BOTH the last text seen and WHY the
// rename ended. Esc is detected explicitly (GetAsyncKeyState) so a cancel is
// never misread as a commit. Enter with text present -> Committed; the EDIT
// vanishing with text still present -> FocusLostCommitted; the whole window
// elapsing -> Timeout; the EDIT never appearing -> NeverStarted.
static InlineRenameOutcome WaitForInlineRenameResult(std::wstring& outBase,
                                                     HWND owner, int timeoutMs) {
    ULONGLONG start = GetTickCount64();
    bool sawEdit = false;
    int missAfterSeen = 0;
    std::wstring last;
    InlineRenameOutcome outcome = InlineRenameOutcome::Timeout;

    // #6: the hook-thread Esc latch is ARMED BY THE CALLER (ArmInlineEscLatch)
    // *before* StartRename, so an extremely fast Esc pressed the instant the
    // rename edit appears -- before this watch loop begins -- is still latched
    // by the hook. Do NOT clear g_inlineEscPressed here: that would erase such
    // an early press. We only (re)assert the active flag defensively, in case
    // the latch was never armed by the caller for some path.
    InterlockedExchange(&g_inlineRenameActive, 1);

    while (GetTickCount64() - start < (ULONGLONG)timeoutMs) {
        // #7: bail promptly on unload instead of blocking teardown for the full
        // timeout. Treat it as a cancel: discard the text, create nothing.
        if (AtomicGet(&g_unloading) != 0) {
            outcome = InlineRenameOutcome::Cancelled;
            last.clear();
            break;
        }

        // Esc cancels the rename: discard whatever was typed and stop. Checked
        // first so a cancel is never misclassified as a focus-lost commit.
        // #6: honour BOTH the hook's latched flag (catches a fast Esc between
        // polls) and the live key state (Esc still physically down now).
        if (AtomicGet(&g_inlineEscPressed) != 0 ||
            (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
            outcome = sawEdit ? InlineRenameOutcome::Cancelled
                              : InlineRenameOutcome::NeverStarted;
            last.clear();
            break;
        }

        std::wstring text;
        if (UIA_ReadFocusedEditText(text, owner)) {
            sawEdit = true;
            missAfterSeen = 0;
            last = text;

            // Enter commits the rename with the current text.
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                outcome = InlineRenameOutcome::Committed;
                break;
            }
        } else if (sawEdit) {
            // The EDIT went away without an explicit Esc/Enter: Explorer
            // committed the rename on focus loss (click-away). The text we
            // last read is the committed name.
            if (++missAfterSeen >= 2) {
                outcome = InlineRenameOutcome::FocusLostCommitted;
                break;
            }
        }
        Sleep(60);
    }

    // #6: also honour a latched Esc that landed after the loop's last check
    // (e.g. right as the EDIT vanished) so it is never reclassified as a commit.
    if (AtomicGet(&g_inlineEscPressed) != 0) {
        outcome = sawEdit ? InlineRenameOutcome::Cancelled
                          : InlineRenameOutcome::NeverStarted;
        last.clear();
    } else if (!sawEdit) {
        outcome = InlineRenameOutcome::NeverStarted;
    }

    // #6: disarm the latch so it can't leak into a later, unrelated watch.
    InterlockedExchange(&g_inlineRenameActive, 0);
    InterlockedExchange(&g_inlineEscPressed, 0);

    outBase = last;
    return outcome;
}

// ---------------- Popup name capture (hybrid: dialog mode) ----------------
// Detect Windows "apps" dark mode from the registry (AppsUseLightTheme == 0).
static bool IsDarkMode() {
    DWORD val = 1;
    DWORD sz = sizeof(val);
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &key) == ERROR_SUCCESS) {
        RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, nullptr,
                         reinterpret_cast<LPBYTE>(&val), &sz);
        RegCloseKey(key);
    }
    return val == 0;
}

// True on Windows 11 (build >= 22000), where the acrylic backdrop is available.
static bool IsWin11() {
    HMODULE nt = GetModuleHandleW(L"ntdll.dll");
    if (!nt) {
        return false;
    }
    typedef LONG(WINAPI * PFN_RtlGetVersion)(OSVERSIONINFOW*);
    PFN_RtlGetVersion rtl = reinterpret_cast<PFN_RtlGetVersion>(
        GetProcAddress(nt, "RtlGetVersion"));
    if (!rtl) {
        return false;
    }
    OSVERSIONINFOW vi = {0};
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (rtl(&vi) != 0) {
        return false;
    }
    return vi.dwMajorVersion > 10 ||
           (vi.dwMajorVersion == 10 && vi.dwBuildNumber >= 22000);
}

// Clean Segoe UI font (replaces the pixelated default System font).
static HFONT g_uiFont = nullptr;
static HFONT UiFont() {
    if (!g_uiFont) {
        g_uiFont = CreateFontW(-17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    }
    return g_uiFont;
}

// ---- Popup theme palette (mirrors tests/theme_logic.h) ----
// One source of truth for every popup colour in light and dark mode. The
// WndProc colour handlers (WM_CTLCOLORSTATIC/EDIT) and the owner-draw buttons
// (WM_DRAWITEM) all read from here, so re-tuning a theme means editing exactly
// one function -- and its mirror in the tests. Keeping these in lockstep is
// what "compatible with light AND dark mode" means: no hard-coded colour is
// left behind in a handler where only one theme was considered.
struct ThemePalette {
    COLORREF panelBg;           // window / panel background
    COLORREF panelText;         // label text on the panel
    COLORREF editBg;            // edit-box background
    COLORREF editText;          // edit-box text
    COLORREF createFill;        // Create button fill (accent)
    COLORREF createFillPressed; // Create button fill while pressed
    COLORREF createText;        // Create button caption
    COLORREF createBorder;      // Create button border
    COLORREF cancelFill;        // Cancel button fill
    COLORREF cancelFillPressed; // Cancel button fill while pressed
    COLORREF cancelText;        // Cancel button caption
    COLORREF cancelBorder;      // Cancel button border
};

// ---- Design colour tokens ----
// Named colour tokens so NO popup colour is a bare hard-coded RGB() at the
// point of use. ResolveThemePalette() maps each semantic slot to one of these
// tokens; the mirror in tests/theme_logic.h defines the SAME token set/values.
static const COLORREF kColWhite            = RGB(255, 255, 255);
// Accent (Create) -- shared across both themes.
static const COLORREF kColAccent           = RGB(0, 95, 184);   // #005FB8
static const COLORREF kColAccentPressed    = RGB(0, 78, 152);
// Light-theme neutrals.
static const COLORREF kColSurfaceLight     = RGB(243, 243, 243);
static const COLORREF kColTextLight        = RGB(26, 26, 26);
static const COLORREF kColInputLight       = RGB(255, 255, 255);
static const COLORREF kColInputTextLight   = RGB(20, 20, 20);
// Dark-theme neutrals.
static const COLORREF kColSurfaceDark      = RGB(32, 32, 32);
static const COLORREF kColTextDark         = RGB(240, 240, 240);
static const COLORREF kColInputDark        = RGB(84, 84, 84);    // #545454
static const COLORREF kColInputTextDark    = RGB(240, 240, 240);
// Danger (Cancel) reds -- per theme.
static const COLORREF kColDangerLight        = RGB(196, 43, 28); // #C42B1C
static const COLORREF kColDangerLightPressed = RGB(176, 34, 22);
static const COLORREF kColDangerLightBorder  = RGB(150, 32, 20);
static const COLORREF kColDangerDark         = RGB(160, 42, 34);
static const COLORREF kColDangerDarkPressed  = RGB(130, 34, 28);
static const COLORREF kColDangerDarkBorder   = RGB(110, 28, 22);

static ThemePalette ResolveThemePalette(bool dark) {
    ThemePalette p = {};
    // Create = the muted accent blue in BOTH themes (pressed darker); white
    // caption. Every assignment below is a token, not a raw colour.
    p.createFill = kColAccent;
    p.createFillPressed = kColAccentPressed;
    p.createText = kColWhite;
    p.createBorder = kColAccent;
    if (dark) {
        p.panelBg = kColSurfaceDark;
        p.panelText = kColTextDark;
        p.editBg = kColInputDark;
        p.editText = kColInputTextDark;
        p.cancelFill = kColDangerDark;
        p.cancelFillPressed = kColDangerDarkPressed;
        p.cancelText = kColWhite;
        p.cancelBorder = kColDangerDarkBorder;
    } else {
        p.panelBg = kColSurfaceLight;
        p.panelText = kColTextLight;
        p.editBg = kColInputLight;
        p.editText = kColInputTextLight;
        p.cancelFill = kColDangerLight;
        p.cancelFillPressed = kColDangerLightPressed;
        p.cancelText = kColWhite;
        p.cancelBorder = kColDangerLightBorder;
    }
    return p;
}

// Small fixed cache of solid brushes keyed by colour, so no colour handler
// leaks a brush or re-creates one per WM_CTLCOLOR message. Freed at unload.
static HBRUSH g_themeBrushes[8] = {0};
static COLORREF g_themeBrushColors[8] = {0};
static HBRUSH ThemeBrush(COLORREF color) {
    for (int i = 0; i < 8; ++i) {
        if (g_themeBrushes[i] && g_themeBrushColors[i] == color) {
            return g_themeBrushes[i];
        }
    }
    for (int i = 0; i < 8; ++i) {
        if (!g_themeBrushes[i]) {
            g_themeBrushes[i] = CreateSolidBrush(color);
            g_themeBrushColors[i] = color;
            return g_themeBrushes[i];
        }
    }
    // Cache full (only two palettes x a few roles are ever requested, so this is
    // not expected in practice). Evict slot 0 rather than returning an UNTRACKED
    // brush: an untracked CreateSolidBrush() is never recorded in
    // g_themeBrushes[] and so FreeThemeBrushes() can never DeleteObject() it --
    // a GDI handle leak for the process lifetime. Reusing a tracked slot keeps
    // every brush we hand out owned by the cache and freed on unload.
    DeleteObject(g_themeBrushes[0]);
    g_themeBrushes[0] = CreateSolidBrush(color);
    g_themeBrushColors[0] = color;
    return g_themeBrushes[0];
}

static void FreeThemeBrushes() {
    for (int i = 0; i < 8; ++i) {
        if (g_themeBrushes[i]) {
            DeleteObject(g_themeBrushes[i]);
            g_themeBrushes[i] = nullptr;
            g_themeBrushColors[i] = 0;
        }
    }
}

// Solid panel background (used when the acrylic backdrop is unavailable).
static HBRUSH PanelBrush(bool dark) {
    return ThemeBrush(ResolveThemePalette(dark).panelBg);
}

// Rounded corners + dark title bar + acrylic backdrop (all best-effort; any
// attribute the OS does not support is silently ignored).
static void ApplyWindowPolish(HWND hwnd, bool dark, bool glass) {
    HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
    if (!dwm) {
        return;
    }
    typedef HRESULT(WINAPI * PFN_Set)(HWND, DWORD, LPCVOID, DWORD);
    PFN_Set set =
        reinterpret_cast<PFN_Set>(GetProcAddress(dwm, "DwmSetWindowAttribute"));
    if (set) {
        BOOL d = dark ? TRUE : FALSE;
        set(hwnd, 20, &d, sizeof(d));  // DWMWA_USE_IMMERSIVE_DARK_MODE
        set(hwnd, 19, &d, sizeof(d));  // older build fallback
        DWORD round = 2;               // DWMWCP_ROUND
        set(hwnd, 33, &round, sizeof(round));
        if (glass) {
            DWORD backdrop = 3;        // DWMSBT_TRANSIENTWINDOW (acrylic)
            set(hwnd, 38, &backdrop, sizeof(backdrop));
        }
    }
    FreeLibrary(dwm);
}

// Force classic (non-themed) painting on a control so it honours the colours we
// hand back from WM_CTLCOLOR*. A themed EDIT in dark mode otherwise IGNORES the
// brush we return and paints its background white (#FFFFFF) -- disabling its
// theme is what makes the dark #545454 input background actually stick.
// Dynamic-loaded (like dwmapi) so there is no extra link-time dependency.
static void DisableWindowTheme(HWND h) {
    HMODULE ux = LoadLibraryW(L"uxtheme.dll");
    if (!ux) {
        return;
    }
    typedef HRESULT(WINAPI * PFN_SetWindowTheme)(HWND, LPCWSTR, LPCWSTR);
    PFN_SetWindowTheme swt = reinterpret_cast<PFN_SetWindowTheme>(
        GetProcAddress(ux, "SetWindowTheme"));
    if (swt) {
        swt(h, L"", L"");
    }
    FreeLibrary(ux);
}

struct NamePromptData {
    std::wstring base;
    bool accepted = false;
    bool glass = false;          // acrylic backdrop REQUESTED (Win11)
    bool acrylicActive = false;  // acrylic backdrop CONFIRMED compositing
    HWND edit = nullptr;
};

static LRESULT CALLBACK NamePromptWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                          LPARAM lParam) {
    NamePromptData* d = reinterpret_cast<NamePromptData*>(
        GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_CREATE: {
            CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            d = reinterpret_cast<NamePromptData*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(d));
            HINSTANCE hinst = HINST_THISCOMPONENT;

            HWND label = CreateWindowExW(0, L"STATIC", L"Enter your folder name",
                            WS_CHILD | WS_VISIBLE,
                            20, 18, 304, 22, hwnd, nullptr, hinst, nullptr);
            d->edit = CreateWindowExW(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                20, 46, 304, 30, hwnd,
                reinterpret_cast<HMENU>(101), hinst, nullptr);
            HWND ok = CreateWindowExW(0, L"BUTTON", L"Create",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                            170, 92, 74, 30, hwnd,
                            reinterpret_cast<HMENU>(IDOK), hinst, nullptr);
            HWND cancel = CreateWindowExW(0, L"BUTTON", L"Cancel",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
                            250, 92, 74, 30, hwnd,
                            reinterpret_cast<HMENU>(IDCANCEL), hinst, nullptr);

            HFONT font = UiFont();
            SendMessageW(label, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
            SendMessageW(d->edit, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
            SendMessageW(ok, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
            SendMessageW(cancel, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);

            // Rounded corners on the input field AND both buttons. Clipping
            // each control HWND to a round-rect region is what removes the
            // corner artifact: the square corners fall OUTSIDE the region, so
            // the owner-draw fill can never leave an unpainted/black corner.
            SetWindowRgn(d->edit, CreateRoundRectRgn(0, 0, 305, 31, 12, 12), TRUE);
            SetWindowRgn(ok, CreateRoundRectRgn(0, 0, 75, 31, 14, 14), TRUE);
            SetWindowRgn(cancel, CreateRoundRectRgn(0, 0, 75, 31, 14, 14), TRUE);
            // Keep the caret/text off the rounded edge of the input field.
            SendMessageW(d->edit, EM_SETMARGINS,
                         EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(10, 10));
            // Detheme the edit so the dark #545454 background actually applies
            // (a themed edit ignores WM_CTLCOLOREDIT and stays white).
            DisableWindowTheme(d->edit);

            SetFocus(d->edit);
            return 0;
        }
        case WM_ERASEBKGND: {
            // Only skip the solid fill when the acrylic backdrop is CONFIRMED to
            // be compositing. Requesting glass (d->glass) is NOT the same as it
            // working: with transparency effects off / DWM rejecting the
            // backdrop / remote sessions, nothing composited and the panel
            // showed through WHITE in dark mode. Painting the solid themed panel
            // is always safe -- real acrylic simply composites over the fill.
            bool acrylicConfirmed = d && d->glass && d->acrylicActive;
            if (acrylicConfirmed) {
                return 1;  // real acrylic will show through; skip the solid fill
            }
            HDC hdc = reinterpret_cast<HDC>(wParam);
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, PanelBrush(IsDarkMode()));
            return 1;
        }
        case WM_CTLCOLORSTATIC: {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            ThemePalette pal = ResolveThemePalette(IsDarkMode());
            SetTextColor(hdc, pal.panelText);
            SetBkMode(hdc, TRANSPARENT);
            // Match WM_ERASEBKGND: a hollow label brush only makes sense over a
            // real acrylic backdrop. When we painted a solid panel, hand back the
            // matching solid brush so the label background is the themed surface
            // (not a stale/white one).
            bool acrylicConfirmed = d && d->glass && d->acrylicActive;
            if (acrylicConfirmed) {
                return reinterpret_cast<LRESULT>(GetStockObject(HOLLOW_BRUSH));
            }
            return reinterpret_cast<LRESULT>(ThemeBrush(pal.panelBg));
        }
        case WM_CTLCOLOREDIT: {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            ThemePalette pal = ResolveThemePalette(IsDarkMode());
            SetTextColor(hdc, pal.editText);
            SetBkColor(hdc, pal.editBg);
            return reinterpret_cast<LRESULT>(ThemeBrush(pal.editBg));
        }
        case WM_DRAWITEM: {
            DRAWITEMSTRUCT* dis = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
            if (dis->CtlType == ODT_BUTTON &&
                (dis->CtlID == IDOK || dis->CtlID == IDCANCEL)) {
                bool pressed = (dis->itemState & ODS_SELECTED) != 0;
                bool isCreate = (dis->CtlID == IDOK);
                RECT rc = dis->rcItem;
                HDC hdc = dis->hDC;

                ThemePalette pal = ResolveThemePalette(IsDarkMode());
                COLORREF fill;
                COLORREF txt;
                COLORREF border;
                if (isCreate) {
                    fill = pressed ? pal.createFillPressed : pal.createFill;
                    txt = pal.createText;
                    border = pal.createBorder;
                } else {
                    fill = pressed ? pal.cancelFillPressed : pal.cancelFill;
                    txt = pal.cancelText;
                    border = pal.cancelBorder;
                }

                // The button HWND is clipped to a rounded region (set at
                // create), so fill the WHOLE item rect -- only the rounded area
                // shows and there is no unpainted corner artifact -- then trace
                // a matching rounded border with a hollow brush for definition.
                HBRUSH b = CreateSolidBrush(fill);
                FillRect(hdc, &rc, b);
                DeleteObject(b);

                HPEN pen = CreatePen(PS_SOLID, 1, border);
                HGDIOBJ op = SelectObject(hdc, pen);
                HGDIOBJ ob = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
                RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 14, 14);
                SelectObject(hdc, ob);
                SelectObject(hdc, op);
                DeleteObject(pen);

                wchar_t caption[32] = {0};
                GetWindowTextW(dis->hwndItem, caption, ARRAYSIZE(caption));
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, txt);
                HGDIOBJ of = SelectObject(hdc, UiFont());
                DrawTextW(hdc, caption, -1, &rc,
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(hdc, of);
                return TRUE;
            }
            break;
        }
        case WM_COMMAND: {
            WORD id = LOWORD(wParam);
            if (id == IDOK && d) {
                wchar_t buf[512] = {0};
                GetWindowTextW(d->edit, buf, ARRAYSIZE(buf));
                d->base = buf;
                d->accepted = true;
                DestroyWindow(hwnd);
                return 0;
            }
            if (id == IDCANCEL) {
                if (d) {
                    d->accepted = false;
                }
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// The whole trigger context posted to the worker via WM_APP+1. Declared here
// (above PromptForFolderName) because the popup's own modal loop shares the
// worker queue and can dequeue a WM_APP+1; when it does it must FREE the heap
// action carried in lParam, so it needs the full type -- not just a forward
// declaration.
struct PendingFolderAction {
    int requested = 0;          // count requested at trigger time
    HWND origin = nullptr;      // origin Explorer window (#1)
    HWND tab = nullptr;         // active shell tab at trigger time (#3)
    bool snapExpected = false;  // #2: WM_APP+2 snapshot POST succeeded for this
                                // action -> drift protection is available. If the
                                // post FAILED this stays false and the worker
                                // safe-cancels rather than running unprotected.
    ULONGLONG keypressTick = 0; // #2 (true trigger-time capture): GetTickCount64()
                                // captured on the hook thread WHEN THE SHORTCUT
                                // FIRED. Paired on the worker with the snapshot's
                                // resolve tick to prove the browse-dir snapshot
                                // was taken close enough to the keypress to be a
                                // real trigger-time capture (see
                                // kSnapshotFreshWindowMs). A late resolve (worker
                                // was busy) can't be trusted -> safe-cancel.
};

// Free a PendingFolderAction* carried in an lParam and clear the post-guard so a
// fresh trigger can queue another action. Used wherever a WM_APP+1 is CONSUMED
// (dropped) rather than acted on, so the heap allocation from
// PostFolderActionCount() is never leaked.
static void DiscardFolderAction(LPARAM lParam) {
    delete reinterpret_cast<PendingFolderAction*>(lParam);
    InterlockedExchange(&g_actionPosted, 0);
}

// Show the minimal modal prompt centered on Explorer. Returns true if the user
// pressed Create; fills outBase (the count is already known from the trigger).
// No PostQuitMessage -> the worker thread's own message loop is left intact.
static bool PromptForFolderName(HWND owner, std::wstring& outBase) {
    static const wchar_t* kClass = L"WhNewFolderHybridPrompt";
    static bool registered = false;
    if (!registered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = NamePromptWndProc;
        wc.hInstance = HINST_THISCOMPONENT;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;  // painted in WM_ERASEBKGND
        wc.lpszClassName = kClass;
        // Hardening: only latch `registered` when the class is actually usable.
        // ERROR_CLASS_ALREADY_EXISTS means a prior registration is still good.
        if (RegisterClassExW(&wc) != 0 ||
            GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
            registered = true;
        } else {
            Wh_Log(L"RegisterClassExW failed (error=%lu); popup unavailable.",
                   GetLastError());
            return false;
        }
    }

    NamePromptData data;
    data.glass = IsWin11();

    const int w = 344;
    const int h = 178;
    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    RECT rc;
    if (owner && GetWindowRect(owner, &rc)) {
        x = rc.left + ((rc.right - rc.left) - w) / 2;
        y = rc.top + ((rc.bottom - rc.top) - h) / 2;
    }

    HWND hwnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, kClass, L"New folder",
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        x, y, w, h, owner, nullptr, HINST_THISCOMPONENT, &data);
    if (!hwnd) {
        return false;
    }

    ApplyWindowPolish(hwnd, IsDarkMode(), data.glass);
    // acrylicActive stays false: requesting the backdrop does not guarantee it
    // composites (transparency effects off, remote session, DWM refusal). We
    // therefore always paint the solid themed panel in WM_ERASEBKGND -- if real
    // acrylic is active it simply composites over that fill, and if it is not we
    // still get the correct #202020 (dark) / #F3F3F3 (light) panel instead of a
    // bare white window. Flip this to true only behind a confirmed-acrylic probe.
    data.acrylicActive = false;

    if (owner) {
        EnableWindow(owner, FALSE);
    }
    ShowWindow(hwnd, SW_SHOW);
    if (!SetForegroundWindow(hwnd)) {
        // Foreground-lock can refuse this; SetFocus below still gives the edit
        // keyboard focus within our (enabled) window, so the prompt is usable.
        Wh_Log(L"PromptForFolderName: SetForegroundWindow(popup) failed "
               L"(error=%lu).", GetLastError());
    }
    SetFocus(data.edit);

    // Self-contained modal loop. Enter = Create, Esc = Cancel. No
    // PostQuitMessage, so the worker thread's outer loop is untouched.
    //
    // #1 (queue ownership): this loop shares the WORKER thread's message queue,
    // so it can dequeue the worker's own thread messages (msg.hwnd == nullptr):
    //   * WM_APP     -- Worker_Exit's shutdown signal. If we swallowed it the
    //                   worker would never quit and unload would hang. Re-post
    //                   it and CLOSE the popup so the outer loop handles quit.
    //   * WM_APP + 1 -- a second folder action posted while this popup is open.
    //                   CONSUME it: this popup owns the queue, so re-posting it
    //                   here would be re-dequeued on the very next GetMessageW
    //                   and spin the CPU (re-post -> receive -> re-post ...).
    //                   The action can't run nested anyway, so drop it and just
    //                   clear g_actionPosted, freeing the next trigger to post
    //                   again after the popup closes. WM_APP (shutdown) is
    //                   different: it must reach the outer loop, so re-post it
    //                   and break out of the modal loop.
    // We never DISPATCH a worker thread message from here -- only the outer
    // WorkerThread loop owns them.
    MSG msg;
    while (IsWindow(hwnd) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            // Shutdown requested: re-post so the outer worker loop can quit,
            // then stop the modal loop.
            PostThreadMessageW(GetCurrentThreadId(), msg.message,
                               msg.wParam, msg.lParam);
            break;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP + 2) {
            // A trigger-time dir snapshot (#2) posted while this popup is open.
            // The action it belongs to will be consumed below (WM_APP+1) and
            // never acted on, so just drop the snapshot request; leaving it to
            // fall through would dispatch a thread message to no window.
            continue;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP + 1) {
            // Stale/duplicate action while this popup is open: consume it,
            // never re-post (that busy-loops). FREE the heap PendingFolderAction
            // carried in lParam (it was new'd in PostFolderActionCount and is
            // never dequeued by the worker once we drop it here) and clear the
            // post-guard so a fresh trigger can queue an action once the popup
            // closes. Dropping without deleting leaked one allocation per
            // shortcut press while the popup was open.
            DiscardFolderAction(msg.lParam);
            continue;
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN) {
            SendMessageW(hwnd, WM_COMMAND, IDOK, 0);
            continue;
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            SendMessageW(hwnd, WM_COMMAND, IDCANCEL, 0);
            continue;
        }
        if (!IsDialogMessageW(hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (owner) {
        EnableWindow(owner, TRUE);
        SetForegroundWindow(owner);
    }

    if (data.accepted) {
        outBase = data.base;
        return true;
    }
    return false;
}

// #1: the origin Explorer target, captured on the HOOK thread at trigger time
// and carried through WM_APP+1 so the worker acts on the window that was
// actually focused when the shortcut fired -- not on whatever is foreground
// hundreds of ms later (the multi-folder count timer alone is up to 1200ms).
// Only touched on the hook thread.
static HWND g_triggerTarget = nullptr;

// #3: the active shell TAB captured alongside g_triggerTarget at trigger time.
// The worker requires the active tab to be unchanged before creating folders,
// so switching tabs in the same Explorer window during the wait cancels the
// action instead of creating folders in the wrong tab. Only touched on the hook
// thread; snapshotted into the queued PendingFolderAction at post time (#2),
// so the worker never reads this global -- it reads its own action's copy.
static HWND g_triggerTab = nullptr;

// #2 (directory race): whether the WM_APP+2 snapshot POST succeeded for the most
// recent trigger. Set in TriggerFired, copied into the queued action by
// PostFolderActionCount, and consumed by the worker: a false value means this
// action has no drift protection, so the worker safe-cancels. Hook-thread only.
static bool g_triggerSnapPosted = false;

// #2 (true trigger-time capture): GetTickCount64() captured on the hook thread at
// the instant the shortcut fires. Carried into the queued PendingFolderAction and
// compared on the worker against g_snapResolveTick, so a snapshot resolved too
// long after the keypress (worker busy -> possible same-tab drift before capture)
// is not trusted. Hook-thread only.
static ULONGLONG g_triggerTick = 0;

// #2 (directory race): the browsing directory the user was in WHEN THE SHORTCUT
// FIRED, resolved on the worker via a WM_APP+2 snapshot posted from TriggerFired
// -- BEFORE count entry runs. Navigating the same tab during count entry (which
// keeps the tab HWND unchanged, so the #3 tab guard alone can't catch it) no
// longer moves the folders: the worker creates them in this snapshot dir, not in
// wherever the tab drifted to by the time WM_APP+1 is acted on.
//
// These three are WORKER-THREAD-LOCAL: both WM_APP+2 (snapshot) and WM_APP+1
// (act) are dispatched by the same worker message loop, serially, so there is no
// cross-thread access and no lock is needed. The hook thread only POSTS the
// messages (origin/tab travel in wParam/lParam), never touching this state.
static std::wstring g_snapDir;          // browse dir at trigger time
static HWND         g_snapOrigin = nullptr;  // origin the snapshot was taken for
static HWND         g_snapTab = nullptr;     // tab the snapshot was taken for
// #2 (true trigger-time capture): GetTickCount64() at the moment SnapshotTriggerDir
// actually RESOLVED g_snapDir on the worker. Compared on the worker against the
// keypress tick carried in PendingFolderAction to reject a snapshot that resolved
// too late to reflect keypress-time reality. Worker-thread-local; paired with
// g_snapDir (cleared alongside it).
static ULONGLONG    g_snapResolveTick = 0;

// #2 (true trigger-time capture): maximum tolerated latency between the keypress
// (stamped on the hook thread) and the worker actually resolving the browse-dir
// snapshot for it. A deliberate same-tab navigation takes far longer than this,
// so a snapshot resolved within the window reliably reflects the keypress-time
// directory; one resolved later (the worker was busy, so the user had time to
// navigate BEFORE the snapshot ran) cannot be trusted as a trigger-time capture
// and the action is cancelled. Chosen well below human navigation time and
// comfortably above an idle worker's COM resolve cost.
static const ULONGLONG kSnapshotFreshWindowMs = 250;

// Resolve and cache the current browsing directory for (origin, tab). Called on
// the worker thread from the WM_APP+2 handler at trigger time, so the captured
// path reflects where the user was before any count-entry navigation.
static void SnapshotTriggerDir(HWND origin, HWND tab) {
    // #2 (true trigger-time capture): stamp WHEN the worker actually reached this
    // snapshot. If the worker was busy the stamp lands well after the keypress;
    // the WM_APP+1 handler compares the two ticks and rejects a stale snapshot.
    // Recorded on entry so every early-return path below still carries it.
    g_snapResolveTick = GetTickCount64();
    g_snapDir.clear();
    g_snapOrigin = origin;
    g_snapTab = tab;

    if (!origin || !IsWindow(origin)) {
        return;
    }

    if (origin == GetShellWindow()) {
        g_snapDir = GetDesktopDir();
        return;
    }

    std::wstring path;
    IShellView* view = nullptr;
    if (ResolveActiveShellView(origin, path, &view) && !path.empty()) {
        g_snapDir = path;
    }
    SafeRelease(view);
}

// ---------------- Main action ----------------
// `origin` is the Explorer window captured on the hook thread at trigger time
// (#1) and carried through WM_APP+1. We act on THAT window, not on whatever is
// foreground now, and we re-validate it here (mirrors ShouldActOnTarget in
// tests/target_capture_test): a null, destroyed, or no-longer-Explorer target
// means the context we were triggered in is gone -> cancel instead of guessing.
static void PerformNewFolderAction(int requested, HWND origin,
                                   HWND capturedTab,
                                   const std::wstring& snapshotDir,
                                   bool snapshotExpected,
                                   bool snapshotFresh) {
    if (InterlockedCompareExchange(&g_actionRunning, 1, 0) != 0) {
        return;
    }

    ULONGLONG totalStart = GetTickCount64();

    // #1: validate the captured origin target instead of re-resolving late.
    bool stillValid = origin && IsWindow(origin);
    bool stillExplorer = stillValid &&
        (IsExplorerTopLevel(origin) || origin == GetShellWindow());
    if (!origin || !stillValid || !stillExplorer) {
        Wh_Log(L"No explorer window detected (captured origin target is gone "
               L"or no longer Explorer); cancelling.");
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    HWND target = origin;

    std::wstring dir;

    IShellView* activeView = nullptr;

    // #3: if a tab was identified at trigger time, require it to still be the
    // active tab now (mirrors ShouldActOnTab / tests/tab_safe_test). Switching
    // tabs in the same Explorer window during the wait must CANCEL, not create
    // folders in the tab the user moved to. A zero captured tab (Desktop /
    // single-tab shell) has no tab identity to drift, so we skip the check.
    // #2: capturedTab travels in the queued action (arg), not a shared global,
    // so a later gesture's tab can't overwrite the one this action must match.
    if (capturedTab != nullptr && target != GetShellWindow()) {
        HWND currentTab = GetActiveShellTabHwnd(target);
        if (currentTab != capturedTab) {
            Wh_Log(L"Active tab changed since trigger; cancelling, "
                   L"nothing created.");
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }
    }

    if (target == GetShellWindow()) {
        dir = GetDesktopDir();
    } else if (!ResolveActiveShellView(target, dir, &activeView) || dir.empty()) {
        Wh_Log(L"Failed to resolve current active tab path.");
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #2 + #3 unified safety rule (mirrors ShouldActOnDir in
    // tests/capture_logic.h, pinned by tests/dir_safe_test). Create ONLY when we
    // have a snapshot we can FULLY trust AND it still matches the live dir:
    //     snapshot POST succeeded             (snapshotExpected)          [#3]
    //   + snapshot resolved near the keypress (snapshotFresh)            [#2]
    //   + snapshot actually resolved a dir    (snapshotDir non-empty)    [#3]
    //   + current dir resolved                (dir non-empty)
    //   + snapshot dir == current dir         (no same-tab drift)        [#2]
    // Anything else CANCELS. We never fall back to "create in the current dir
    // without a verified snapshot".

    // #3 (post-failure hole): a failed WM_APP+2 post means no drift protection.
    if (!snapshotExpected) {
        Wh_Log(L"Trigger-dir snapshot was not requested/failed to post; "
               L"cancelling to avoid an unprotected action.");
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #2 (true trigger-time capture): the snapshot is resolved on the worker
    // (WM_APP+2), not at the keypress. If it resolved too long after the keypress
    // the worker was busy, so the user may have navigated the same tab BEFORE the
    // snapshot ran -- meaning the snapshot itself could hold the drifted-to dir
    // and the compare below would wrongly match. A stale snapshot is not a
    // trigger-time capture, so cancel rather than trust it.
    if (!snapshotFresh) {
        Wh_Log(L"Trigger-dir snapshot resolved too late to be a true "
               L"keypress-time capture (worker was busy); cancelling.");
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #3 (resolution-failure hole): "post succeeded" != "snapshot succeeded".
    // ResolveActiveShellView can fail and leave the snapshot empty even though the
    // post went through. Treat an unresolved snapshot (or an unresolvable current
    // dir) as unsafe and CANCEL -- do NOT silently proceed with the current dir
    // and no drift check.
    if (snapshotDir.empty() || dir.empty()) {
        Wh_Log(L"Trigger-dir snapshot did not resolve (post ok but empty "
               L"snapshot/current dir); cancelling to avoid an unverified "
               L"action.");
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #2 (directory race): if the captured browse dir DIFFERS from where the
    // active view resolves NOW, the user navigated the same tab during the wait
    // (the #3 tab guard can't see a same-tab nav). Creating in the snapshot dir
    // while `activeView` still points at the drifted-to dir would also drive the
    // inline rename against the WRONG live view. The safe, provable behavior is
    // to CANCEL -- create nothing.
    {
        std::wstring snap = snapshotDir;
        while (!snap.empty() && (snap.back() == L'\\' || snap.back() == L'/')) {
            snap.pop_back();
        }
        if (snap.empty() || _wcsicmp(snap.c_str(), dir.c_str()) != 0) {
            Wh_Log(L"Browse dir changed since trigger (was '%ls', now '%ls'); "
                   L"cancelling, nothing created.", snap.c_str(), dir.c_str());
            SafeRelease(activeView);
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }
    }

    while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) {
        dir.pop_back();
    }

    DWORD dirAttrs = GetFileAttributesW(dir.c_str());

    if (dirAttrs == INVALID_FILE_ATTRIBUTES ||
        !(dirAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
        Wh_Log(L"Unsupported target directory: %ls", dir.c_str());
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #5: snapshot the settings ONCE (atomic reads) so this whole action uses a
    // single consistent view even if the user changes settings mid-run.
    const LONG multiFolder = AtomicGet(&g_multiFolder);
    const LONG maxFolders = AtomicGet(&g_maxFolders);
    const LONG autoResolveNames = AtomicGet(&g_autoResolveNames);
    const LONG useDialog = AtomicGet(&g_useDialog);

    int count = ResolveFolderCount(requested, multiFolder != 0, maxFolders);

    // Debug diagnostic (gated by Windhawk's own "Logging enabled" toggle):
    // shows exactly how the requested count was resolved for this run.
    Wh_Log(L"NewFolder: requested=%d multi=%d maxFolders=%d autoResolve=%d "
           L"-> count=%d dir=%ls",
           requested,
           multiFolder != 0 ? 1 : 0,
           static_cast<int>(maxFolders),
           autoResolveNames != 0 ? 1 : 0,
           count,
           dir.c_str());

    if (useDialog) {
        // Dialog mode: a small popup takes the base name (+ count when multi is
        // on), then we create the folders directly. No temp folder, no
        // rename-watch, and NO PostQuitMessage (that was the "popup works only
        // once" bug -- it killed the worker thread's message loop).
        std::wstring capturedBase;
        if (!PromptForFolderName(target, capturedBase)) {
            Wh_Log(L"Dialog cancelled; nothing created.");
            SafeRelease(activeView);
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }

        std::vector<std::wstring> dlgFolders =
            ResolveCaptureNames(dir, capturedBase, count,
                                autoResolveNames != 0);
        if (dlgFolders.empty()) {
            Wh_Log(L"No folder names available in: %ls", dir.c_str());
            SafeRelease(activeView);
            InterlockedExchange(&g_actionRunning, 0);
            return;
        }

        std::wstring dlgFirst;
        int dlgCreated = 0;
        for (const std::wstring& folder : dlgFolders) {
            std::wstring targetName = folder;
            DWORD err = ERROR_SUCCESS;
            bool created = CreateFolderDirect(targetName, &err);
            if (!created && IsPermissionError(err)) {
                created = CreateFolderWithShellElevation(targetName, target);
            }
            // #5: a lost check-then-create race is ERROR_ALREADY_EXISTS -- treat
            // it as a collision and retry the next free name (bounded), rather
            // than reporting a spurious create failure.
            // A named batch ("<base> 1", "<base> 2", ...) keeps its numbered
            // scheme on retry: the sanitized base is non-empty AND more than one
            // folder was planned. Re-running ResolveCaptureNames(count=1) here
            // would collapse to the bare "<base>" (and even "<base> (2)"),
            // breaking the sequence into "Project 1 / Project / Project 3".
            // MakeNamedFolderNames(..., 1) returns the next free "<base> N".
            const std::wstring retryBase = SanitizeFolderBase(capturedBase);
            const bool namedBatch = !retryBase.empty() && dlgFolders.size() > 1;
            int retries = 0;
            while (!created && ShouldRetryOnCollision(err) && retries < 8) {
                ++retries;
                std::vector<std::wstring> next =
                    namedBatch
                        ? MakeNamedFolderNames(dir, retryBase, 1)
                        : ResolveCaptureNames(dir, capturedBase, 1,
                                              autoResolveNames != 0);
                if (next.empty()) {
                    break;
                }
                targetName = next[0];
                err = ERROR_SUCCESS;
                created = CreateFolderDirect(targetName, &err);
                if (!created && IsPermissionError(err)) {
                    created = CreateFolderWithShellElevation(targetName, target);
                }
            }
            if (!created) {
                Wh_Log(L"Folder create failed. error=%lu path=%ls",
                       err, targetName.c_str());
                continue;
            }
            ++dlgCreated;
            if (dlgFirst.empty()) {
                dlgFirst = targetName;
            }
            SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, targetName.c_str(), nullptr);
        }
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, dir.c_str(), nullptr);
        if (!dlgFirst.empty()) {
            UIA_SelectItemByNameFallback(target, GetFileNamePart(dlgFirst));
        }

        SafeRelease(activeView);
        Wh_Log(L"Dialog capture: base='%ls' created=%d of %d",
               capturedBase.c_str(), dlgCreated, (int)dlgFolders.size());
        Wh_Log(L"Total action cost=%llums", GetTickCount64() - totalStart);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // Inline mode: create ONE temp folder, drop it into native inline
    // rename (the "temp folder as input box" idea), watch the edit box for the
    // typed name, then materialise "<base> 1".."<base> N".
    std::vector<std::wstring> seed =
        MakeUniqueFolderNames(dir, 1, autoResolveNames != 0);
    if (seed.empty()) {
        Wh_Log(L"No seed folder name available in: %ls", dir.c_str());
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    std::wstring tempPath = seed[0];
    DWORD seedErr = ERROR_SUCCESS;
    bool seedElevated = false;
    bool seedCreated = CreateFolderDirect(tempPath, &seedErr);
    if (!seedCreated && IsPermissionError(seedErr)) {
        seedElevated = true;
        seedCreated = CreateFolderWithShellElevation(tempPath, target);
    }
    // #4: the seed create is check-then-create (MakeUniqueFolderNames picked a
    // name that looked free), so it has the same TOCTOU race as the dialog/batch
    // paths -- another process can win the name and CreateDirectoryW returns
    // ERROR_ALREADY_EXISTS. Treat that as a collision and advance to the next
    // free name ("New folder (2)", ...) instead of aborting (mirrors
    // ShouldRetryOnCollision / tests/collision_retry_test).
    int seedRetries = 0;
    while (!seedCreated && ShouldRetryOnCollision(seedErr) && seedRetries < 8) {
        ++seedRetries;
        std::vector<std::wstring> nextSeed =
            MakeUniqueFolderNames(dir, 1, autoResolveNames != 0);
        if (nextSeed.empty()) {
            break;
        }
        tempPath = nextSeed[0];
        seedErr = ERROR_SUCCESS;
        seedElevated = false;
        seedCreated = CreateFolderDirect(tempPath, &seedErr);
        if (!seedCreated && IsPermissionError(seedErr)) {
            seedElevated = true;
            seedCreated = CreateFolderWithShellElevation(tempPath, target);
        }
    }
    if (!seedCreated) {
        Wh_Log(L"Temp folder create failed. error=%lu path=%ls",
               seedErr, tempPath.c_str());
        SafeRelease(activeView);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #6: arm the Esc latch BEFORE the rename edit can appear, so a fast Esc in
    // the gap between StartRename and the watch loop is still caught by the hook.
    ArmInlineEscLatch();

    // Native inline rename on the temp folder (this IS the input box).
    StartRename(target, activeView, tempPath, dir, seedElevated);
    SafeRelease(activeView);

    // Watch the edit box until the user finishes typing (Enter / click away).
    std::wstring typed;
    InlineRenameOutcome outcome =
        WaitForInlineRenameResult(typed, target, 30000);

    // Esc / timeout / never-started must CANCEL -- never create folders from a
    // cancelled rename (mirrors ShouldUseCapturedName; the multi-folder "Esc
    // still creates" regression). Explorer removed the temp folder on Esc, so
    // there is nothing to clean up here; just stop.
    if (!ShouldUseCapturedName(outcome)) {
        Wh_Log(L"Inline rename not committed (outcome=%d); cancelling, "
               L"nothing created.",
               static_cast<int>(outcome));
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    std::wstring base = SanitizeFolderBase(typed);

    // Locate the temp folder's committed path (Explorer may already have
    // renamed it to `base` for us).
    std::wstring committedPath = tempPath;
    if (!PathFileExistsW(tempPath.c_str())) {
        std::wstring renamed = JoinPath(dir, base);
        if (!base.empty() && PathFileExistsW(renamed.c_str())) {
            committedPath = renamed;
        } else {
            committedPath.clear();
        }
    }

    if (base.empty() || count <= 1) {
        // No usable name, or a single folder: the inline rename already did the
        // whole job, nothing more to create.
        Wh_Log(L"Inline capture: single/no-name base='%ls' count=%d",
               base.c_str(), count);
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // Batch of "<base> 1".."<base> N". Reuse the temp folder as slot 1 by
    // renaming it (avoids deleting anything, so no N+1 leftover). If reuse
    // fails, delete the temp folder and create slot 1 fresh -- and if that
    // delete is blocked the temp folder lingers (the N+1 edge case).
    std::vector<std::wstring> names = MakeNamedFolderNames(dir, base, count);
    if (names.empty()) {
        InterlockedExchange(&g_actionRunning, 0);
        return;
    }

    // #10: only tell Explorer about folders that ACTUALLY exist -- notifying
    // SHCNE_MKDIR for intended-but-failed names makes Explorer cache phantoms.
    std::vector<std::wstring> createdFolders;
    size_t startIdx = 0;
    if (!committedPath.empty() && committedPath == names[0]) {
        // The inline rename already produced names[0].
        createdFolders.push_back(names[0]);
        startIdx = 1;
    } else if (!committedPath.empty()) {
        // #6: try to reuse the committed temp folder as names[0] via a rename.
        // If the move fails, LEAVE the committed folder in place -- do NOT
        // delete a user-visible folder on a transient/racy move failure (the
        // old RemoveDirectoryW fallback was unsafe). We simply create names[0]
        // fresh below and the committed folder stays as-is.
        if (MoveFileW(committedPath.c_str(), names[0].c_str())) {
            createdFolders.push_back(names[0]);
            startIdx = 1;
        } else {
            Wh_Log(L"Temp folder rename to '%ls' failed; leaving it in place: %ls",
                   names[0].c_str(), committedPath.c_str());
        }
    }

    for (size_t i = startIdx; i < names.size(); ++i) {
        std::wstring targetName = names[i];
        DWORD err = ERROR_SUCCESS;
        bool created = CreateFolderDirect(targetName, &err);
        if (!created && IsPermissionError(err)) {
            created = CreateFolderWithShellElevation(targetName, target);
        }
        // #5: a lost check-then-create race shows up as ERROR_ALREADY_EXISTS.
        // Treat it as a collision: re-resolve the next free "<base> N" name and
        // retry, up to a small bounded number of attempts, instead of failing.
        int retries = 0;
        while (!created && ShouldRetryOnCollision(err) && retries < 8) {
            ++retries;
            std::vector<std::wstring> next =
                MakeNamedFolderNames(dir, base, 1);
            if (next.empty()) {
                break;
            }
            targetName = next[0];
            err = ERROR_SUCCESS;
            created = CreateFolderDirect(targetName, &err);
            if (!created && IsPermissionError(err)) {
                created = CreateFolderWithShellElevation(targetName, target);
            }
        }
        if (created) {
            createdFolders.push_back(targetName);
        } else {
            Wh_Log(L"Batch folder create failed. error=%lu path=%ls",
                   err, targetName.c_str());
        }
    }

    for (const std::wstring& f : createdFolders) {
        SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, f.c_str(), nullptr);
    }
    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATHW, dir.c_str(), nullptr);
    int createdCount = (int)createdFolders.size();
    Wh_Log(L"Inline capture: base='%ls' created=%d of %d",
           base.c_str(), createdCount, (int)names.size());

    Wh_Log(L"Total action cost=%llums",
           GetTickCount64() - totalStart);

    InterlockedExchange(&g_actionRunning, 0);
}

// ---------------- Hook thread ----------------
static volatile HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HHOOK g_lowLevelHook = nullptr;
static volatile HANDLE g_workerThread = nullptr;
static DWORD g_workerThreadId = 0;

// Chord state -- only touched on the hook thread.
static bool g_chordPending = false;
static int g_chordPendingVk = 0;
// #5 (chord snapshot): the trigger mode captured when a chord BEGINS. The
// gesture must resolve with the mode that started it, even if the user changes
// the trigger setting while the chord is pending. Hook-thread only.
static LONG g_chordMode = 0;
static ULONGLONG g_chordSinceMs = 0;
static UINT_PTR g_chordTimer = 0;
static bool g_swallowUpF = false;
static bool g_swallowUpN = false;

static const UINT_PTR kChordTimerId = 0xF0F0;
static const UINT kChordTimeoutMs = 1200;

// Count-entry state (multi-folder) -- only touched on the hook thread.
static bool g_countActive = false;
static int g_countValue = 0;
static bool g_countSawDigit = false;
static UINT_PTR g_countTimer = 0;

static const UINT_PTR kCountTimerId = 0xF0F1;
static const UINT kCountTimeoutMs = 1200;

static DWORD WINAPI WorkerThread(void* pParameter);
static DWORD WINAPI HookThread(void* pParameter);
static LRESULT CALLBACK LowLevelKeybdProc(int nCode,
                                          WPARAM wParam,
                                          LPARAM lParam);

// #2: every field the worker needs to act on the ORIGINAL trigger context is
// captured on the hook thread and carried in a heap-owned struct whose pointer
// rides in lParam. Nothing is read from a shared global at worker time, so a
// second gesture that overwrites g_triggerTarget / g_triggerTab while the first
// action is still queued can no longer make the first action act on the wrong
// window OR the wrong tab. The worker takes ownership and frees it.
// Post the (folder-creating) worker action. Non-blocking; the hook thread must
// never do COM / filesystem work inline.
//
// The whole trigger context (count + origin HWND + origin tab HWND) is snapshot
// into a heap PendingFolderAction and its pointer rides in lParam, so each
// posted action is fully self-contained -- NO shared mutable state between the
// hook thread and the worker.
static void PostFolderActionCount(int requested) {
    if (g_workerThreadId &&
        InterlockedCompareExchange(&g_actionPosted, 1, 0) == 0) {
        auto* action = new (std::nothrow) PendingFolderAction();
        if (!action) {
            InterlockedExchange(&g_actionPosted, 0);
            return;
        }
        action->requested = requested;
        action->origin = g_triggerTarget;
        action->tab = g_triggerTab;
        action->snapExpected = g_triggerSnapPosted;  // #2: carry post success
        action->keypressTick = g_triggerTick;        // #2: carry keypress time
        if (!PostThreadMessageW(
                g_workerThreadId, WM_APP + 1, 0,
                reinterpret_cast<LPARAM>(action))) {
            delete action;
            InterlockedExchange(&g_actionPosted, 0);
        }
    }
}

// Re-inject the Ctrl+F we swallowed so Explorer's search still opens. Injected
// input carries LLKHF_INJECTED, so our own hook ignores it (no feedback loop).
static void ReplayCtrlF() {
    bool ctrlHeld = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;

    INPUT in[4] = {};
    int n = 0;

    if (!ctrlHeld) {
        in[n].type = INPUT_KEYBOARD;
        in[n].ki.wVk = VK_CONTROL;
        ++n;
    }

    in[n].type = INPUT_KEYBOARD;
    in[n].ki.wVk = 'F';
    ++n;

    in[n].type = INPUT_KEYBOARD;
    in[n].ki.wVk = 'F';
    in[n].ki.dwFlags = KEYEVENTF_KEYUP;
    ++n;

    if (!ctrlHeld) {
        in[n].type = INPUT_KEYBOARD;
        in[n].ki.wVk = VK_CONTROL;
        in[n].ki.dwFlags = KEYEVENTF_KEYUP;
        ++n;
    }

    UINT sent = SendInput(n, in, sizeof(INPUT));
    if (sent != static_cast<UINT>(n)) {
        // Replay of the swallowed Ctrl+F may be blocked (UIPI / secure desktop);
        // hardening only, nothing safe to retry here.
        Wh_Log(L"ReplayCtrlF: SendInput sent %u/%d events (error=%lu).",
               sent, n, GetLastError());
    }
}

static void ResolveChordTimeout();  // fwd decl: chord-timer failure fallback

static void StartChordTimer() {
    g_chordTimer = SetTimer(nullptr, kChordTimerId, kChordTimeoutMs, nullptr);
    // Hardening: if the timer could not be created the chord would otherwise
    // stay pending forever (Ctrl+F stuck). There is no tick coming, so resolve
    // the pending chord NOW -- same path the WM_TIMER would have taken (replay
    // Ctrl+F / create). Log it so the rare failure is diagnosable.
    if (g_chordTimer == 0) {
        Wh_Log(L"StartChordTimer: SetTimer failed (error=%lu); resolving chord "
               L"immediately so it can't stay pending", GetLastError());
        ResolveChordTimeout();
    }
}

static void StopChordTimer() {
    if (g_chordTimer) {
        KillTimer(nullptr, g_chordTimer);
        g_chordTimer = 0;
    }
}

static void ResolveChordTimeout() {
    StopChordTimer();

    if (!g_chordPending) {
        return;
    }

    bool wasF = (g_chordPendingVk == 'F');
    // #5: resolve with the mode captured when the chord began, not the current
    // (possibly just-changed) setting.
    LONG mode = g_chordMode;

    g_chordPending = false;
    g_chordPendingVk = 0;

    // Only Ctrl+F+N swallowed a real Ctrl+F (search) that it now owes back.
    if (mode == 0 && wasF) {
        ReplayCtrlF();
    }
}

// ---- Multi-folder count entry (hook thread only) ----
static void StartCountTimer() {
    g_countTimer = SetTimer(nullptr, kCountTimerId, kCountTimeoutMs, nullptr);
    // Hardening: if the count-entry timer could not be created it would never
    // auto-commit. Log it; the caller (BeginCountCapture) commits immediately
    // as a fallback so the gesture still produces a folder.
    if (g_countTimer == 0) {
        Wh_Log(L"StartCountTimer: SetTimer failed (error=%lu)", GetLastError());
    }
}

static void StopCountTimer() {
    if (g_countTimer) {
        KillTimer(nullptr, g_countTimer);
        g_countTimer = 0;
    }
}

// Clear count-entry state and stop its timer.
static void EndCountCapture() {
    StopCountTimer();
    g_countActive = false;
    g_countValue = 0;
    g_countSawDigit = false;
}

// Open a count-entry window right after a trigger fired (multi-folder on).
static void BeginCountCapture() {
    g_countActive = true;
    g_countValue = 0;
    g_countSawDigit = false;
    StartCountTimer();
    // Hardening: without the auto-commit timer, count entry could never close.
    // Fall back to creating a single folder immediately so the gesture is never
    // silently stuck.
    if (g_countTimer == 0) {
        g_countActive = false;
        PostFolderActionCount(1);
    }
}

// Commit the accumulated count (no digits -> 1) and create that many folders.
static void CommitCountCapture() {
    int requested = g_countSawDigit ? g_countValue : 0;
    Wh_Log(L"CountCapture commit: sawDigit=%d value=%d -> requested=%d",
           g_countSawDigit ? 1 : 0,
           g_countValue,
           requested);
    EndCountCapture();
    PostFolderActionCount(requested);
}

// The count-entry window elapsed: commit what was typed.
static void ResolveCountTimeout() {
    if (!g_countActive) {
        return;
    }
    CommitCountCapture();
}

// A trigger fired: open count entry when multi-folder is on, else create one now.
static void TriggerFired() {
    // #2 (true trigger-time capture): stamp the keypress instant FIRST, before any
    // other work, so it is as close as possible to the physical shortcut. The
    // worker later uses it to verify the browse-dir snapshot was resolved promptly
    // (i.e. before the user could have navigated the same tab).
    g_triggerTick = GetTickCount64();
    // #1: resolve the origin Explorer window NOW, on the hook thread, while the
    // user's focus/cursor is still where the shortcut was pressed. This HWND is
    // carried through to the worker via PostFolderActionCount so the folders
    // land in the right window even if focus moves during count entry.
    g_triggerTarget = GetTargetExplorerHWND();
    // #3: also snapshot the active tab so the worker can detect a tab switch
    // during the wait and cancel rather than create in the wrong tab.
    g_triggerTab = g_triggerTarget ? GetActiveShellTabHwnd(g_triggerTarget)
                                   : nullptr;

    // #2 (directory race): ask the worker to snapshot the CURRENT browse dir for
    // this (origin, tab) NOW, before count entry can navigate the same tab. COM
    // path resolution must not run on the hook thread, so we only POST -- the
    // worker resolves it and caches it, and later reuses it when the WM_APP+1
    // action arrives. HWNDs fit in wParam/lParam.
    //
    // Record whether the post SUCCEEDED. If it failed (or there was no target),
    // this action has NO drift protection, and PostFolderActionCount carries the
    // failure through so the worker safe-cancels instead of running unprotected.
    g_triggerSnapPosted = false;
    if (g_workerThreadId && g_triggerTarget) {
        g_triggerSnapPosted =
            PostThreadMessageW(g_workerThreadId, WM_APP + 2,
                               reinterpret_cast<WPARAM>(g_triggerTarget),
                               reinterpret_cast<LPARAM>(g_triggerTab)) != FALSE;
    }

    if (AtomicGet(&g_multiFolder)) {  // atomic read, like every other settings load
        BeginCountCapture();
    } else {
        PostFolderActionCount(1);
    }
}

static BOOL Worker_Init() {
    if (g_workerThread) {
        return TRUE;
    }

    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) return FALSE;

    HANDLE hThread = CreateThread(nullptr,
                                  0,
                                  WorkerThread,
                                  readyEvent,
                                  CREATE_SUSPENDED,
                                  &g_workerThreadId);

    if (!hThread) {
        CloseHandle(readyEvent);
        return FALSE;
    }

    SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
    ResumeThread(hThread);

    WaitForSingleObject(readyEvent, INFINITE);
    CloseHandle(readyEvent);

    g_workerThread = hThread;
    return TRUE;
}

static void Worker_Exit() {
    // #7: signal any in-flight inline-rename poll to bail so this teardown does
    // not block on WaitForSingleObject(INFINITE) for the full rename timeout.
    InterlockedExchange(&g_unloading, 1);

    HANDLE hThread =
        (HANDLE)InterlockedExchangePointer((PVOID*)&g_workerThread, nullptr);

    if (!hThread) return;

    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_APP, 0, 0);
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    g_workerThreadId = 0;
}

static BOOL KeybdHook_Init() {
    if (g_hookThread) {
        return TRUE;
    }

    if (!Worker_Init()) {
        Wh_Log(L"Worker thread initialization failed.");
        return FALSE;
    }

    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        Worker_Exit();
        return FALSE;
    }

    HANDLE hThread = CreateThread(nullptr,
                                  0,
                                  HookThread,
                                  readyEvent,
                                  CREATE_SUSPENDED,
                                  &g_hookThreadId);

    if (!hThread) {
        CloseHandle(readyEvent);
        Worker_Exit();
        return FALSE;
    }

    SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
    ResumeThread(hThread);

    WaitForSingleObject(readyEvent, INFINITE);
    CloseHandle(readyEvent);

    if (!g_lowLevelHook) {
        Wh_Log(L"SetWindowsHookEx failed.");
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        g_hookThreadId = 0;
        Worker_Exit();
        return FALSE;
    }

    g_hookThread = hThread;
    return TRUE;
}

static void KeybdHook_Exit() {
    HANDLE hThread =
        (HANDLE)InterlockedExchangePointer((PVOID*)&g_hookThread, nullptr);

    if (!hThread) return;

    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_APP, 0, 0);
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    g_hookThreadId = 0;
    g_lowLevelHook = nullptr;

    Worker_Exit();
}

static DWORD WINAPI WorkerThread(void* pParameter) {
    HANDLE readyEvent = (HANDLE)pParameter;
    MSG msg;

    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HRESULT hrCom = CoInitializeEx(nullptr,
                                   COINIT_APARTMENTTHREADED |
                                   COINIT_DISABLE_OLE1DDE);
    // Hardening: COM is required for shell path/view resolution. On failure the
    // action still fails safe later (ResolveActiveShellView returns false), but
    // logging makes the root cause obvious instead of a mystery "resolve failed".
    if (FAILED(hrCom)) {
        Wh_Log(L"Worker CoInitializeEx failed (hr=0x%08lX); shell resolution "
               L"will be unavailable.",
               static_cast<unsigned long>(hrCom));
    }

    SetEvent(readyEvent);

    while (true) {
        BOOL bRet = GetMessageW(&msg, nullptr, 0, 0);

        if (bRet <= 0) {
            break;
        }

        if (msg.hwnd == nullptr) {
            if (msg.message == WM_APP) {
                PostQuitMessage(0);
                continue;
            }

            if (msg.message == WM_APP + 2) {
                // #2 (directory race): trigger-time browse-dir snapshot. Resolve
                // and cache the dir for (origin, tab) NOW, before count entry
                // navigates. Worker-thread-local; paired with the WM_APP+1 that
                // follows once the count commits.
                SnapshotTriggerDir(reinterpret_cast<HWND>(msg.wParam),
                                   reinterpret_cast<HWND>(msg.lParam));
                continue;
            }

            if (msg.message == WM_APP + 1) {
                InterlockedExchange(&g_actionPosted, 0);
                // Take ownership of the heap action posted by the hook thread
                // (#2): count + origin + tab all travel WITH the message, never
                // read from a global at worker time.
                std::unique_ptr<PendingFolderAction> action(
                    reinterpret_cast<PendingFolderAction*>(msg.lParam));
                HWND origin = action ? action->origin : nullptr;
                HWND tab = action ? action->tab : nullptr;
                int requested = action ? action->requested : 0;
                // #2 (directory race): did the WM_APP+2 snapshot POST succeed?
                // If not, this action has NO drift protection and must be
                // safe-cancelled rather than run unprotected.
                bool snapExpected = action ? action->snapExpected : false;
                // #2 (directory race): pair this action with the trigger-time
                // browse-dir snapshot taken by WM_APP+2. Both messages run on
                // this same worker loop serially, so g_snap* needs no lock. Only
                // use it (dir + resolve tick) when it was resolved for THIS exact
                // (origin, tab); if anything differs we leave both empty/zero so
                // the downstream guard safe-cancels rather than acting unverified.
                std::wstring snapDir;
                ULONGLONG snapResolveTick = 0;
                if (origin && origin == g_snapOrigin && tab == g_snapTab) {
                    snapDir = g_snapDir;
                    snapResolveTick = g_snapResolveTick;
                }
                g_snapDir.clear();
                g_snapOrigin = nullptr;
                g_snapTab = nullptr;
                g_snapResolveTick = 0;
                // #2 (true trigger-time capture): the snapshot is resolved on the
                // worker when it processes WM_APP+2, NOT at the keypress. If the
                // worker was busy the resolve can land AFTER the user navigated
                // the same tab, so the snapshot itself would hold the drifted-to
                // dir and the dir compare would wrongly match. Only trust the
                // snapshot when it was resolved within a short window of the
                // keypress -- short enough that no manual same-tab navigation
                // could have completed in between. Anything slower is not a true
                // trigger-time capture, so the action safe-cancels downstream.
                ULONGLONG keypressTick = action ? action->keypressTick : 0;
                bool snapFresh =
                    snapResolveTick != 0 && keypressTick != 0 &&
                    snapResolveTick >= keypressTick &&
                    (snapResolveTick - keypressTick) <= kSnapshotFreshWindowMs;
                // Mirrors ShouldRunQueuedAction (tests/queue_drain_test,
                // tests/unload_drain_test). DRAIN (discard) the dequeued action
                // instead of running it when:
                //   * we are UNLOADING -- Worker_Exit set g_unloading; starting
                //     now would seed a temp folder mid-teardown before the
                //     rename watch could notice the unload and cancel;
                //   * an action is already in flight (its popup is pumping this
                //     same loop) -- avoids the stale duplicate; or
                //   * the message carried no captured origin (stale/legacy).
                if (AtomicGet(&g_unloading) != 0 ||
                    g_actionRunning != 0 || origin == nullptr) {
                    continue;  // action freed by unique_ptr
                }
                PerformNewFolderAction(requested, origin, tab, snapDir,
                                       snapExpected, snapFresh);
                continue;
            }
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    SafeRelease(g_uia);

    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
    }

    return 0;
}

static DWORD WINAPI HookThread(void* pParameter) {
    HANDLE readyEvent = (HANDLE)pParameter;
    MSG msg;

    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_lowLevelHook = SetWindowsHookExW(WH_KEYBOARD_LL,
                                       LowLevelKeybdProc,
                                       HINST_THISCOMPONENT,
                                       0);

    SetEvent(readyEvent);

    if (!g_lowLevelHook) {
        return 0;
    }

    while (true) {
        BOOL bRet = GetMessageW(&msg, nullptr, 0, 0);

        if (bRet <= 0) {
            break;
        }

        if (msg.hwnd == nullptr) {
            if (msg.message == WM_APP) {
                PostQuitMessage(0);
                continue;
            }

            // SetTimer(NULL,...) IGNORES the id we pass and returns a fresh
            // system id (stored in g_chordTimer); the WM_TIMER wParam carries
            // THAT id, not our constant -- match the stored handle or the
            // timeout never resolves and the chord state gets stuck.
            if (msg.message == WM_TIMER && g_chordTimer &&
                msg.wParam == g_chordTimer) {
                ResolveChordTimeout();
                continue;
            }

            // Same as above: match the SetTimer-returned handle, not the
            // ignored constant kCountTimerId. Without this the count-entry
            // window never auto-commits, so a bare trigger creates nothing and
            // g_countActive stays stuck (the next keystroke then leaks to
            // Explorer -- search opens / Ctrl+N new window).
            if (msg.message == WM_TIMER && g_countTimer &&
                msg.wParam == g_countTimer) {
                ResolveCountTimeout();
                continue;
            }
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWindowsHookEx(g_lowLevelHook);
    g_lowLevelHook = nullptr;

    return 0;
}

static bool IsExplorerForegroundForHotkey() {
    HWND fg = GetForegroundWindow();

    if (!fg) return false;

    return IsExplorerTopLevel(fg) || fg == GetShellWindow();
}

// Mirrors tests/newfolder_logic.h::TriggerDecide. Runs on the hook thread and
// stays O(1): it only reads async key state, updates flags, arms a timer, and
// posts the worker action. No COM / filesystem / blocking work here.
static LRESULT CALLBACK LowLevelKeybdProc(int nCode,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
    }

    const KBDLLHOOKSTRUCT* info =
        reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

    if (!info) {
        return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
    }

    // Never react to our own injected input (avoids a feedback loop).
    if (info->flags & LLKHF_INJECTED) {
        return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
    }

    const bool isKeyDown =
        (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    const bool isKeyUp =
        (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
    const int vk = static_cast<int>(info->vkCode);
    const LONG mode = AtomicGet(&g_triggerMode);   // #5: atomic settings read

    // Swallow the key-up of a key whose key-down we swallowed (no stray char).
    if (isKeyUp) {
        if (vk == 'F' && g_swallowUpF) {
            g_swallowUpF = false;
            return 1;
        }
        if (vk == 'N' && g_swallowUpN) {
            g_swallowUpN = false;
            return 1;
        }

        // Hold-Ctrl-throughout: the instant Ctrl is released, finish count
        // entry NOW -- commit typed digits, or the default single folder when
        // none were typed. This is the responsive path (no 1200 ms count-timer
        // wait) and it clears g_countActive so the next gesture starts clean
        // instead of leaking the key into Explorer (search / Ctrl+N window).
        if (g_countActive &&
            (vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL) &&
            (GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0) {
            CommitCountCapture();
        }

        return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
    }

    if (!isKeyDown) {
        return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
    }

    // #6: latch an Esc pressed while the inline-rename watcher is running so a
    // fast press+release between the watcher's 60 ms polls is still seen as a
    // cancel. We only RECORD it here -- never swallow it -- because Explorer
    // itself needs the Esc to cancel its own rename EDIT.
    if (vk == VK_ESCAPE && AtomicGet(&g_inlineRenameActive) != 0) {
        InterlockedExchange(&g_inlineEscPressed, 1);
    }

    // ---- Count entry (multi-folder): a number typed right after a trigger ----
    if (g_countActive) {
        // Never hold keys hostage if focus left Explorer/desktop.
        if (!IsExplorerForegroundForHotkey()) {
            EndCountCapture();
            return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
        }

        // Ctrl-held-throughout: the count window only lives while Ctrl stays
        // down. The moment Ctrl is released, FINISH entry -- commit the digits
        // typed so far, or the default single folder when none were typed
        // (CommitCountCapture maps "no digits" -> 1). Only Esc cancels (create
        // nothing). Mirrors CountDecide(ctrlHeld=false). NOTE: the primary
        // release path is the key-up handler above; this keydown fallback
        // covers a key that arrives after Ctrl was already let go.
        const bool ctrlStillHeld = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        if (!ctrlStillHeld) {
            // A DIGIT is explicit intent: the low-level hook can deliver the
            // FIRST count digit's keydown after GetAsyncKeyState(VK_CONTROL)
            // already flipped to released (observed as "Ctrl+F+N then 9"
            // logging sawDigit=0 -> requested=0 -> count=1). Fold that first
            // digit into the accumulator BEFORE committing, so the requested
            // count is honoured. Mirrors CountDecide(ctrlHeld=false, Digit).
            if (!g_countSawDigit) {
                int firstDigit = -1;
                if (vk >= '0' && vk <= '9') {
                    firstDigit = vk - '0';
                } else if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) {
                    firstDigit = vk - VK_NUMPAD0;
                }
                if (firstDigit >= 0) {
                    g_countValue = firstDigit;
                    g_countSawDigit = true;
                    Wh_Log(L"Ctrl read released as the first count digit "
                           L"arrived; recording digit=%d before commit",
                           firstDigit);
                    CommitCountCapture();  // create the requested count
                    return 1;              // this digit is ours; swallow it
                }
            }
            if (g_countSawDigit) {
                Wh_Log(L"Ctrl released during count entry: committing typed "
                       L"count (value=%d)", g_countValue);
                CommitCountCapture();  // create the requested count
            } else {
                Wh_Log(L"Ctrl released during count entry with no digits: "
                       L"creating the default single folder");
                CommitCountCapture();  // no digits -> 1 (default single folder)
            }
            return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
        }

        // Modifier auto-repeat while Ctrl is (correctly) still held: neither
        // commit nor leak a keystroke -- just keep capturing.
        if (vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL ||
            vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT ||
            vk == VK_MENU || vk == VK_LMENU || vk == VK_RMENU ||
            vk == VK_LWIN || vk == VK_RWIN) {
            return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
        }

        int digit = -1;
        if (vk >= '0' && vk <= '9') {
            digit = vk - '0';
        } else if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) {
            digit = vk - VK_NUMPAD0;
        }

        if (digit >= 0) {
            int v = g_countValue * 10 + digit;
            if (v > kFolderHardCap) {
                v = kFolderHardCap;
            }
            g_countValue = v;
            g_countSawDigit = true;
            return 1;  // swallow the digit
        }

        if (vk == VK_RETURN) {
            CommitCountCapture();
            return 1;  // swallow Enter
        }

        if (vk == VK_ESCAPE) {
            EndCountCapture();  // cancel: create nothing
            return 1;  // swallow Esc
        }

        // Any other key WHILE CTRL IS STILL HELD (the Ctrl-release paths above
        // already returned): commit what we have (or one) and SWALLOW it.
        // Letting it through would reach Explorer as a Ctrl+<key> shortcut --
        // Ctrl+N opens a new window, Ctrl+F opens search -- the exact "a new
        // window opens when I use Ctrl+F+N <number>" bug. Keys reach Explorer
        // only after Ctrl is released (handled above). Mirrors
        // CountDecide(CountKey::Other, ctrlHeld=true) -> swallow.
        CommitCountCapture();
        return 1;
    }

    const bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    const bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    const bool cleanCtrl = ctrl && !alt && !shift;
    const bool fg = IsExplorerForegroundForHotkey();

    // ---- Complete or abandon a pending chord ----
    if (g_chordPending) {
        bool expired = (GetTickCount64() - g_chordSinceMs) > kChordTimeoutMs;

        if (!expired && ctrl) {
            if (mode == 0 && g_chordPendingVk == 'F' && vk == 'N') {
                StopChordTimer();
                g_chordPending = false;
                g_chordPendingVk = 0;
                g_swallowUpN = true;
                TriggerFired();
                return 1;
            }

            if (mode == 2 && g_chordPendingVk == 'N' && vk == 'F') {
                StopChordTimer();
                g_chordPending = false;
                g_chordPendingVk = 0;
                g_swallowUpF = true;
                TriggerFired();
                return 1;
            }

            // Auto-repeat of the first chord key: keep swallowing, stay pending.
            if (vk == g_chordPendingVk) {
                return 1;
            }
        }

        // Chord broken or expired: abandon (replay Ctrl+F only for Ctrl+F+N),
        // then let the current key be evaluated as a fresh trigger below.
        bool wasF = (g_chordPendingVk == 'F');
        StopChordTimer();
        g_chordPending = false;
        g_chordPendingVk = 0;
        if (mode == 0 && wasF) {
            ReplayCtrlF();
        }
    }

    // ---- Not pending: react to the first trigger key for this mode ----
    if (cleanCtrl && fg) {
        if (mode == 1 && vk == 'N') {                 // Ctrl+N -> create now
            g_swallowUpN = true;
            TriggerFired();
            return 1;
        }

        if (mode == 0 && vk == 'F') {                 // Ctrl+F+N -> start on F
            g_chordPending = true;
            g_chordPendingVk = 'F';
            g_chordMode = mode;                       // #5: snapshot at begin
            g_chordSinceMs = GetTickCount64();
            g_swallowUpF = true;
            StartChordTimer();
            return 1;
        }

        if (mode == 2 && vk == 'N') {                 // Ctrl+N+F -> start on N
            g_chordPending = true;
            g_chordPendingVk = 'N';
            g_chordMode = mode;                       // #5: snapshot at begin
            g_chordSinceMs = GetTickCount64();
            g_swallowUpN = true;
            StartChordTimer();
            return 1;
        }
    }

    return CallNextHookEx(g_lowLevelHook, nCode, wParam, lParam);
}

// ---------------- Tool mod entry points ----------------
BOOL WhTool_ModInit() {
    // Startup diagnostic. The version here MUST match the @version metadata
    // above (enforced by scripts\check_version_init.ps1).
    Wh_Log(L"INIT: v0.6.0 explorer-ctrlfn-newfolder loaded");
    LoadSettings();
    return KeybdHook_Init();
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    KeybdHook_Exit();
    FreeThemeBrushes();
    if (g_uiFont) {
        DeleteObject(g_uiFont);
        g_uiFont = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod boilerplate

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; ++i) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; ++i) {
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
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
    switch (GetModuleFileNameW(nullptr,
                               currentProcessPath,
                               ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 256];
    swprintf_s(commandLine,
               L"\"%s\" -tool-mod \"%s\"",
               currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE,
        LPCWSTR,
        LPWSTR,
        LPSECURITY_ATTRIBUTES,
        LPSECURITY_ATTRIBUTES,
        WINBOOL,
        DWORD,
        LPVOID,
        LPCWSTR,
        LPSTARTUPINFOW,
        LPPROCESS_INFORMATION,
        PHANDLE);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;

    PROCESS_INFORMATION pi = {};
    if (!pCreateProcessInternalW(nullptr,
                                 currentProcessPath,
                                 commandLine,
                                 nullptr,
                                 nullptr,
                                 FALSE,
                                 NORMAL_PRIORITY_CLASS,
                                 nullptr,
                                 nullptr,
                                 &si,
                                 &pi,
                                 nullptr)) {
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

    LoadSettings();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
