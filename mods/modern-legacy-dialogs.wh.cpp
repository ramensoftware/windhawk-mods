// ==WindhawkMod==
// @id              modern-legacy-dialogs
// @name            Modern Legacy Dialogs
// @description     Renders MessageBox as a task dialog and routes GetOpenFileName/GetSaveFileName through the modern file picker
// @version         2.0.0
// @author          emirerkul991-1yssssss
// @github          https://github.com/emirerkul991-1yssssss
// @license         MIT
// @include         *
// @include         WerFault.exe
// @compilerOptions -lcomdlg32 -lole32 -luuid -luser32 -lgdi32 -lgdiplus -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Modern Legacy Dialogs

Two of the most visible "this app is from 2001" tells on Windows are the raw
`MessageBox` and the ancient common file dialog. Both are pure Win32 API calls,
which makes them interceptable.

* **`MessageBox` -> `TaskDialogIndirect`** — same buttons, same return values,
  modern typography, spacing, and dark-mode-aware chrome. Covers `MessageBoxW/A`,
  `MessageBoxExW/A` and `MessageBoxIndirectW/A`. That last pair matters more than
  it looks: it is a separate user32 export that does not route through
  `MessageBoxW`, and it is what VBScript's `MsgBox` and several older frameworks
  actually call. A `MessageBoxIndirect` call carrying a custom icon
  (`MB_USERICON`) falls back to the original dialog, as does one showing a Help
  button (`MB_HELP`). A help *callback* alone does not: VBScript installs one on
  every `MsgBox` in case a helpfile argument was given, so treating the callback
  itself as disqualifying would exclude all of VBScript. Without `MB_HELP` there
  is no Help button, and the only thing lost is F1 opening help - which a box
  with no helpfile does not have anyway.
* **`GetOpenFileNameW` / `GetSaveFileNameW` -> `IFileOpenDialog` /
  `IFileSaveDialog`** — the Explorer-based picker with the navigation pane,
  search, and Quick Access, for apps that still call the Win2000-era API.

## Two renderers

**Custom (default)** — the dialog is drawn by the mod: DWM-rounded corners, a
title row with a close button, the system icon, and right-aligned buttons with
the accent colour on the default one. It never touches comctl32, so none of the
activation-context machinery below applies to it, and it is immune to other mods
that remap process-wide UI fonts. Colours follow the app/dark-mode setting and
the system accent colour.

**Task dialog** — `TaskDialogIndirect` from comctl32 v6. Closer to a stock
Windows 11 dialog, and the automatic fallback for right-to-left layouts, which
the custom renderer does not mirror.

`messageSplit` only affects the task-dialog renderer. The custom one puts the
caption in the title row and the whole message in the body, which is what the
heading/body split was approximating anyway.

## The comctl32 v6 problem

`TaskDialogIndirect` only exists in comctl32 version 6, and the apps that most
need this mod are exactly the ones with no v6 manifest. The mod solves this by
building an activation context at runtime (a generated manifest in `%TEMP%`,
deleted immediately after `CreateActCtx` reads it) and activating it around both
the module load and the dialog call.

## When it deliberately gets out of the way

Fidelity matters more than coverage here, so the mod falls back to the original
API whenever it cannot reproduce the exact behaviour:

**MessageBox falls back on:** `MB_HELP` (the Help button needs `WM_HELP` routed
back to the owner), `MB_SERVICE_NOTIFICATION`, and `MB_DEFAULT_DESKTOP_ONLY`.

`MB_SYSTEMMODAL` / `MB_TASKMODAL` fall back **only on the task dialog renderer**,
which cannot disable a whole thread's windows. The custom renderer implements
`MB_TASKMODAL` directly by disabling every top-level window on the calling
thread, and treats `MB_SYSTEMMODAL` as topmost. This matters more than it
sounds: Windows Script Host shows its `MsgBox` task-modal, so a blanket modal
fallback silently excluded every VBScript dialog.

**File dialogs fall back on:** `OFN_ENABLEHOOK`, `OFN_ENABLETEMPLATE`,
`OFN_ENABLETEMPLATEHANDLE`, `OFN_ENABLEINCLUDENOTIFY` (all of these let the app
inject its own controls or hook proc into the dialog, which `IFileDialog` has no
equivalent for), multi-select without `OFN_EXPLORER` (different result encoding),
and any thread that is already in an MTA apartment.

## Known limitations

* **ANSI file dialogs are not hooked.** `GetOpenFileNameA` still shows the old
  picker. Converting `OPENFILENAMEA` round-trip is doable but the result buffer
  semantics get genuinely hairy; left out on purpose rather than done badly.
* **`CommDlgExtendedError` is not settable** from outside comdlg32, so an app
  that distinguishes "cancelled" from "buffer too small" will see both as
  cancelled. The mod avoids this by falling back when the buffer is too small.
* The `MB_ABORTRETRYIGNORE` and `MB_CANCELTRYCONTINUE` button sets have no
  common-button equivalents, so their labels are pulled from `user32`'s string
  table. If those resource IDs ever shift, the mod uses English fallbacks —
  turn off `handleRareButtonSets` to route those two sets to the original
  `MessageBox` instead.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modernizeMessageBox: true
  $name: Modernize MessageBox
  $name: Dialog style
  $options:
  - custom: WinUI-style dialog drawn by the mod (rounded, accent default button)
  - taskdialog: Windows task dialog (comctl32), closer to a stock Windows 11 dialog
- messageSplit: auto
  $name: Message layout
  $description: >-
    A task dialog has a large "main instruction" and a smaller body; MessageBox
    has one blob of text.
  $options:
  - auto: First line as the heading when the message has several lines
  - instruction: Always use the large heading style
  - content: Always use the small body style
- handleRareButtonSets: true
  $name: Handle Abort/Retry/Ignore and Cancel/Try Again/Continue
  $description: >-
    These need custom buttons labelled from user32's string table. Turn off to
    send those two sets to the original MessageBox.
- fallbackOnModal: true
  $name: Use the original MessageBox for system/task modal dialogs
  $description: >-
    Only affects the task dialog renderer, which cannot block a whole thread the
    way MB_TASKMODAL requires. The custom renderer implements it directly, so
    this setting does not restrict it.
- modernizeFileDialogs: true
  $name: Modernize the common file dialogs
- emulateChangeDir: true
  $name: Emulate the old working-directory change
  $description: >-
    The legacy dialog changes the process working directory unless OFN_NOCHANGEDIR
    is set. Some old apps depend on it.
- verboseLog: false
  $name: Log each intercepted dialog
  $description: >-
    Also writes a trace of the renderer decision path to
    %TEMP%\wh-mld-diag.log, which is useful when a dialog unexpectedly stays
    unmodernized.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <commdlg.h>
#include <dwmapi.h>
// For walking loaded modules: two comctl32 versions can be loaded at once and
// only the v6 one exports TaskDialogIndirect. The K32* entry points live in
// kernel32, so no extra import library is needed.
#include <psapi.h>
#include <shobjidl.h>
#include <shlwapi.h>

#include <algorithm>
#include <objidl.h>
#include <gdiplus.h>

#include <stdarg.h>
#include <stdio.h>

#include <mutex>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

enum class MessageSplit { Auto, Instruction, Content };
enum class Renderer { Custom, TaskDialog };

struct Settings {
    bool modernizeMessageBox = true;
    Renderer renderer = Renderer::Custom;
    MessageSplit messageSplit = MessageSplit::Auto;
    bool handleRareButtonSets = true;
    bool fallbackOnModal = true;
    bool modernizeFileDialogs = true;
    bool emulateChangeDir = true;
    bool verboseLog = false;
};

Settings g_settings;

// -----------------------------------------------------------------------------
// comctl32 v6 activation context
// -----------------------------------------------------------------------------

constexpr char kManifest[] =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n"
    "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">\r\n"
    "  <dependency>\r\n"
    "    <dependentAssembly>\r\n"
    "      <assemblyIdentity type=\"win32\""
    " name=\"Microsoft.Windows.Common-Controls\""
    " version=\"6.0.0.0\""
    " processorArchitecture=\"*\""
    " publicKeyToken=\"6595b64144ccf1df\""
    " language=\"*\" />\r\n"
    "    </dependentAssembly>\r\n"
    "  </dependency>\r\n"
    "</assembly>\r\n";

HANDLE g_actCtx = INVALID_HANDLE_VALUE;

using TaskDialogIndirect_t = HRESULT(WINAPI*)(const TASKDIALOGCONFIG*, int*, int*,
                                              BOOL*);
TaskDialogIndirect_t g_taskDialogIndirect;

std::once_flag g_taskDialogOnce;

struct ActCtxScope {
    ULONG_PTR cookie = 0;
    bool active = false;

    explicit ActCtxScope(HANDLE actCtx) {
        if (actCtx && actCtx != INVALID_HANDLE_VALUE) {
            active = ActivateActCtx(actCtx, &cookie) != FALSE;
        }
    }
    ~ActCtxScope() {
        if (active) {
            DeactivateActCtx(0, cookie);
        }
    }
    ActCtxScope(const ActCtxScope&) = delete;
    ActCtxScope& operator=(const ActCtxScope&) = delete;
};

HANDLE CreateComctlV6ActCtx() {
    WCHAR tempDir[MAX_PATH];
    DWORD length = GetTempPathW(MAX_PATH, tempDir);
    if (!length || length >= MAX_PATH) {
        return INVALID_HANDLE_VALUE;
    }

    std::wstring path = tempDir;
    path += L"windhawk-modern-legacy-dialogs-";
    path += std::to_wstring(GetCurrentProcessId());
    path += L".manifest";

    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return INVALID_HANDLE_VALUE;
    }

    DWORD written = 0;
    BOOL ok = WriteFile(file, kManifest, sizeof(kManifest) - 1, &written, nullptr);
    CloseHandle(file);
    if (!ok || written != sizeof(kManifest) - 1) {
        DeleteFileW(path.c_str());
        return INVALID_HANDLE_VALUE;
    }

    ACTCTXW actCtx{};
    actCtx.cbSize = sizeof(actCtx);
    actCtx.lpSource = path.c_str();
    HANDLE handle = CreateActCtxW(&actCtx);

    // CreateActCtx parses the manifest eagerly, so the file is dead weight now.
    DeleteFileW(path.c_str());
    return handle;
}

bool EnsureTaskDialog() {
    std::call_once(g_taskDialogOnce, [] {
        g_actCtx = CreateComctlV6ActCtx();
        ActCtxScope scope(g_actCtx);

        HMODULE comctl = LoadLibraryExW(L"comctl32.dll", nullptr, 0);
        if (!comctl) {
            return;
        }
        g_taskDialogIndirect = reinterpret_cast<TaskDialogIndirect_t>(
            GetProcAddress(comctl, "TaskDialogIndirect"));
        if (!g_taskDialogIndirect) {
            Wh_Log(L"comctl32 v6 unavailable, MessageBox is left alone");
        }
    });
    return g_taskDialogIndirect != nullptr;
}

// -----------------------------------------------------------------------------
// MessageBox -> TaskDialogIndirect
// -----------------------------------------------------------------------------

thread_local bool tls_inTaskDialog = false;

// user32's message-box button strings. The IDs are stable in practice but the
// English fallbacks make a mismatch cosmetic rather than fatal.
std::wstring LoadUserStringOr(UINT id, const WCHAR* fallback) {
    WCHAR buffer[128];
    int length = LoadStringW(GetModuleHandleW(L"user32.dll"), id, buffer,
                             ARRAYSIZE(buffer));
    if (length <= 0 || length >= static_cast<int>(ARRAYSIZE(buffer))) {
        return fallback;
    }
    return std::wstring(buffer, length);
}

struct ButtonPlan {
    TASKDIALOG_COMMON_BUTTON_FLAGS common = 0;
    std::vector<std::wstring> customText;
    std::vector<int> customIds;
    std::vector<int> order;  // for MB_DEFBUTTONn
    bool allowCancellation = false;
    int cancelMapsTo = IDCANCEL;
    bool supported = true;
};

ButtonPlan PlanButtons(UINT type) {
    ButtonPlan plan;

    switch (type & MB_TYPEMASK) {
        case MB_OK:
            plan.common = TDCBF_OK_BUTTON;
            plan.order = {IDOK};
            plan.allowCancellation = true;
            plan.cancelMapsTo = IDOK;  // MessageBox returns IDOK when Esc closes it
            break;

        case MB_OKCANCEL:
            plan.common = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
            plan.order = {IDOK, IDCANCEL};
            plan.allowCancellation = true;
            break;

        case MB_YESNO:
            plan.common = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;
            plan.order = {IDYES, IDNO};
            break;

        case MB_YESNOCANCEL:
            plan.common = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON;
            plan.order = {IDYES, IDNO, IDCANCEL};
            plan.allowCancellation = true;
            break;

        case MB_RETRYCANCEL:
            plan.common = TDCBF_RETRY_BUTTON | TDCBF_CANCEL_BUTTON;
            plan.order = {IDRETRY, IDCANCEL};
            plan.allowCancellation = true;
            break;

        case MB_ABORTRETRYIGNORE:
            if (!g_settings.handleRareButtonSets) {
                plan.supported = false;
                break;
            }
            plan.customText = {LoadUserStringOr(802, L"&Abort"),
                               LoadUserStringOr(803, L"&Retry"),
                               LoadUserStringOr(804, L"&Ignore")};
            plan.customIds = {IDABORT, IDRETRY, IDIGNORE};
            plan.order = {IDABORT, IDRETRY, IDIGNORE};
            break;

        case MB_CANCELTRYCONTINUE:
            if (!g_settings.handleRareButtonSets) {
                plan.supported = false;
                break;
            }
            plan.customText = {LoadUserStringOr(801, L"Cancel"),
                               LoadUserStringOr(809, L"&Try Again"),
                               LoadUserStringOr(810, L"&Continue")};
            plan.customIds = {IDCANCEL, IDTRYAGAIN, IDCONTINUE};
            plan.order = {IDCANCEL, IDTRYAGAIN, IDCONTINUE};
            plan.allowCancellation = true;
            break;

        default:
            plan.supported = false;
            break;
    }

    return plan;
}

void SplitMessage(const WCHAR* text, std::wstring* instruction,
                  std::wstring* content) {
    std::wstring all = text ? text : L"";

    if (g_settings.messageSplit == MessageSplit::Content) {
        *content = std::move(all);
        return;
    }
    if (g_settings.messageSplit == MessageSplit::Instruction) {
        *instruction = std::move(all);
        return;
    }

    size_t breakPos = all.find_first_of(L"\r\n");
    // A long first line is a paragraph, not a heading.
    if (breakPos == std::wstring::npos || breakPos == 0 || breakPos > 120) {
        *instruction = std::move(all);
        return;
    }

    *instruction = all.substr(0, breakPos);
    size_t rest = all.find_first_not_of(L"\r\n", breakPos);
    if (rest != std::wstring::npos) {
        *content = all.substr(rest);
    }
}

HRESULT CALLBACK TaskDialogCallback(HWND hwnd, UINT msg, WPARAM, LPARAM,
                                    LONG_PTR refData) {
    if (msg == TDN_CREATED) {
        UINT type = static_cast<UINT>(refData);
        if (type & MB_TOPMOST) {
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        if (type & MB_SETFOREGROUND) {
            SetForegroundWindow(hwnd);
        }
    }
    return S_OK;
}

// -----------------------------------------------------------------------------
// Custom-drawn dialog
//
// TaskDialogIndirect cannot be restyled - comctl32 owns its layout - so the
// WinUI-style look is drawn from scratch here: a WS_POPUP window painted by the
// mod, with real owner-drawn BUTTON children so focus, Tab, mnemonics and space
// activation keep working for free.
// -----------------------------------------------------------------------------

namespace customdlg {

// Not in mingw's dwmapi.h at the time of writing.
constexpr DWORD kDwmUseImmersiveDarkMode = 20;
constexpr DWORD kDwmWindowCornerPreference = 33;
constexpr DWORD kDwmBorderColor = 34;
constexpr DWORD kDwmCornerRound = 2;

constexpr int kCloseButtonId = 0x7FF0;
constexpr PCWSTR kClassName = L"WindhawkModernLegacyDialog";
constexpr PCWSTR kHotProp = L"WhMldHot";
constexpr PCWSTR kOldProcProp = L"WhMldOldProc";

// Traces the renderer decision path to %TEMP%\wh-mld-diag.log when verbose
// logging is on. Off by default; Windhawk's own log is not readable from
// outside its UI, which makes a file the only practical way to debug a mod
// running inside somebody else's process.
void Diag(PCWSTR format, ...) {
    if (!g_settings.verboseLog) {
        return;
    }

    WCHAR line[512];
    va_list args;
    va_start(args, format);
    int written = _vsnwprintf(line, ARRAYSIZE(line) - 1, format, args);
    va_end(args);
    if (written < 0) {
        return;
    }
    line[written] = L'\0';

    Wh_Log(L"%s", line);

    WCHAR tempDir[MAX_PATH];
    DWORD length = GetTempPathW(MAX_PATH, tempDir);
    if (!length || length >= MAX_PATH) {
        return;
    }

    WCHAR exePath[MAX_PATH] = L"?";
    GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
    PCWSTR exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? exeName + 1 : exePath;

    std::wstring record = exeName;
    record += L" [";
    record += std::to_wstring(GetCurrentProcessId());
    record += L"] ";
    record += line;
    record += L"\r\n";

    int bytes = WideCharToMultiByte(CP_UTF8, 0, record.c_str(), -1, nullptr, 0,
                                    nullptr, nullptr);
    if (bytes <= 1) {
        return;
    }
    std::string utf8(static_cast<size_t>(bytes - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, record.c_str(), -1, utf8.data(), bytes,
                        nullptr, nullptr);

    std::wstring path = tempDir;
    path += L"wh-mld-diag.log";
    HANDLE file =
        CreateFileW(path.c_str(), FILE_APPEND_DATA,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }
    DWORD ignored = 0;
    WriteFile(file, utf8.data(), static_cast<DWORD>(utf8.size()), &ignored,
              nullptr);
    CloseHandle(file);
}

ULONG_PTR g_gdiplusToken = 0;
bool g_gdiplusReady = false;
std::once_flag g_gdiplusOnce;

ATOM g_classAtom = 0;
std::once_flag g_classOnce;

bool EnsureGdiplus() {
    std::call_once(g_gdiplusOnce, [] {
        Gdiplus::GdiplusStartupInput input;
        g_gdiplusReady =
            Gdiplus::GdiplusStartup(&g_gdiplusToken, &input, nullptr) == Gdiplus::Ok;
    });
    return g_gdiplusReady;
}

HINSTANCE ModuleInstance() {
    static HMODULE module = [] {
        HMODULE result = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<PCWSTR>(&EnsureGdiplus), &result);
        return result;
    }();
    return reinterpret_cast<HINSTANCE>(module);
}

UINT GetDpiForWindowSafe(HWND hwnd) {
    static auto proc = reinterpret_cast<UINT(WINAPI*)(HWND)>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    if (proc) {
        UINT dpi = proc(hwnd);
        if (dpi) {
            return dpi;
        }
    }
    HDC hdc = GetDC(nullptr);
    UINT dpi = hdc ? static_cast<UINT>(GetDeviceCaps(hdc, LOGPIXELSX)) : 96;
    if (hdc) {
        ReleaseDC(nullptr, hdc);
    }
    return dpi ? dpi : 96;
}

// -------------------------------- palette ------------------------------------

struct Palette {
    COLORREF body;
    COLORREF footer;
    COLORREF border;
    COLORREF text;
    COLORREF buttonFace;
    COLORREF buttonHot;
    COLORREF buttonDown;
    COLORREF buttonBorder;
    COLORREF accent;
    COLORREF accentHot;
    COLORREF accentDown;
    COLORREF accentText;
    bool dark;
};

DWORD ReadDword(HKEY root, PCWSTR subKey, PCWSTR value, DWORD fallback) {
    DWORD data = 0;
    DWORD size = sizeof(data);
    if (RegGetValueW(root, subKey, value, RRF_RT_REG_DWORD, nullptr, &data, &size) ==
        ERROR_SUCCESS) {
        return data;
    }
    return fallback;
}

COLORREF Shade(COLORREF color, int delta) {
    auto clamp = [](int v) { return v < 0 ? 0 : (v > 255 ? 255 : v); };
    return RGB(clamp(GetRValue(color) + delta), clamp(GetGValue(color) + delta),
               clamp(GetBValue(color) + delta));
}

bool IsLight(COLORREF color) {
    // Rec. 601 luma - only used to pick black vs white text on the accent fill.
    int luma = (GetRValue(color) * 299 + GetGValue(color) * 587 +
                GetBValue(color) * 114) /
               1000;
    return luma > 150;
}

Palette GetPalette() {
    Palette p{};
    p.dark = ReadDword(HKEY_CURRENT_USER,
                       L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                       L"Personalize",
                       L"AppsUseLightTheme", 1) == 0;

    // AccentColor is stored as 0xAABBGGRR, whose low 24 bits already match
    // COLORREF's 0x00BBGGRR layout.
    DWORD accent =
        ReadDword(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                  L"AccentColor", 0);
    p.accent = accent ? static_cast<COLORREF>(accent & 0x00FFFFFF) : RGB(0, 120, 212);
    p.accentHot = Shade(p.accent, p.dark ? 16 : -14);
    p.accentDown = Shade(p.accent, p.dark ? -18 : -28);
    p.accentText = IsLight(p.accent) ? RGB(0, 0, 0) : RGB(255, 255, 255);

    if (p.dark) {
        p.body = RGB(32, 32, 32);
        p.footer = RGB(27, 27, 27);
        p.border = RGB(58, 58, 58);
        p.text = RGB(255, 255, 255);
        p.buttonFace = RGB(45, 45, 45);
        p.buttonHot = RGB(53, 53, 53);
        p.buttonDown = RGB(39, 39, 39);
        p.buttonBorder = RGB(66, 66, 66);
    } else {
        p.body = RGB(243, 243, 243);
        p.footer = RGB(238, 238, 238);
        p.border = RGB(219, 219, 219);
        p.text = RGB(26, 26, 26);
        p.buttonFace = RGB(253, 253, 253);
        p.buttonHot = RGB(246, 246, 246);
        p.buttonDown = RGB(249, 249, 249);
        p.buttonBorder = RGB(216, 216, 216);
    }
    return p;
}

Gdiplus::Color ToGdi(COLORREF color, BYTE alpha = 255) {
    return Gdiplus::Color(alpha, GetRValue(color), GetGValue(color),
                          GetBValue(color));
}

// --------------------------------- fonts -------------------------------------

void CopyFaceName(WCHAR* destination, PCWSTR source) {
    size_t length = wcsnlen(source, LF_FACESIZE - 1);
    memcpy(destination, source, length * sizeof(WCHAR));
    destination[length] = L'\0';
}

// Must be a real __stdcall function rather than a lambda: on 32-bit builds a
// captureless lambda converts to a __cdecl pointer, which FONTENUMPROCW rejects.
// (On x64 there is only one calling convention, so this compiles either way.)
int CALLBACK FontEnumProc(const LOGFONTW*, const TEXTMETRICW*, DWORD,
                          LPARAM param) {
    *reinterpret_cast<bool*>(param) = true;
    return 0;
}

bool FontExists(PCWSTR name) {
    LOGFONTW lf{};
    lf.lfCharSet = DEFAULT_CHARSET;
    CopyFaceName(lf.lfFaceName, name);

    bool found = false;
    HDC hdc = GetDC(nullptr);
    if (!hdc) {
        return false;
    }
    EnumFontFamiliesExW(hdc, &lf, FontEnumProc, reinterpret_cast<LPARAM>(&found),
                        0);
    ReleaseDC(nullptr, hdc);
    return found;
}

PCWSTR PickFont(PCWSTR preferred, PCWSTR fallback) {
    return FontExists(preferred) ? preferred : fallback;
}

HFONT MakeFont(PCWSTR face, int tenthsOfPoint, int weight, UINT dpi) {
    LOGFONTW lf{};
    lf.lfHeight = -MulDiv(tenthsOfPoint, static_cast<int>(dpi), 720);
    lf.lfWeight = weight;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = CLEARTYPE_QUALITY;
    CopyFaceName(lf.lfFaceName, face);
    return CreateFontIndirectW(&lf);
}

struct Fonts {
    HFONT title = nullptr;
    HFONT body = nullptr;
    HFONT button = nullptr;
    HFONT glyph = nullptr;

    void Create(UINT dpi) {
        PCWSTR display = PickFont(L"Segoe UI Variable Display", L"Segoe UI");
        PCWSTR textFace = PickFont(L"Segoe UI Variable Text", L"Segoe UI");
        PCWSTR glyphFace = PickFont(L"Segoe Fluent Icons", L"Segoe MDL2 Assets");
        title = MakeFont(display, 135, FW_SEMIBOLD, dpi);
        body = MakeFont(textFace, 105, FW_NORMAL, dpi);
        button = MakeFont(textFace, 105, FW_NORMAL, dpi);
        glyph = MakeFont(glyphFace, 80, FW_NORMAL, dpi);
    }

    ~Fonts() {
        for (HFONT font : {title, body, button, glyph}) {
            if (font) {
                DeleteObject(font);
            }
        }
    }
};

// -------------------------------- metrics ------------------------------------

struct Metrics {
    int padding;
    int iconSize;
    int iconGap;
    int titleGap;
    int footerPadding;
    int buttonHeight;
    int buttonMinWidth;
    int buttonPadX;
    int buttonGap;
    int closeSize;
    int maxTextWidth;
    int minWidth;
    int buttonRadius;
};

Metrics ScaleMetrics(UINT dpi) {
    auto s = [dpi](int dip) { return MulDiv(dip, static_cast<int>(dpi), 96); };
    Metrics m{};
    m.padding = s(20);
    m.iconSize = s(32);
    m.iconGap = s(16);
    m.titleGap = s(14);
    m.footerPadding = s(16);
    m.buttonHeight = s(32);
    m.buttonMinWidth = s(96);
    m.buttonPadX = s(16);
    m.buttonGap = s(8);
    m.closeSize = s(30);
    m.maxTextWidth = s(400);
    m.minWidth = s(360);
    m.buttonRadius = s(4);
    return m;
}

// --------------------------------- state -------------------------------------

struct Button {
    int id = 0;
    std::wstring text;
    bool isDefault = false;
    int width = 0;
    int x = 0;
    HWND hwnd = nullptr;
};

struct State {
    std::wstring title;
    std::wstring text;
    HICON icon = nullptr;
    std::vector<Button> buttons;
    int defaultId = 0;
    int cancelId = 0;
    int result = 0;
    bool done = false;
    bool showClose = false;
    bool showPrefix = false;
    Palette palette{};
    Metrics metrics{};
    Fonts fonts;
    UINT dpi = 96;
    HWND hwnd = nullptr;
    HWND closeButton = nullptr;
    RECT titleRect{};
    RECT iconRect{};
    RECT textRect{};
    RECT closeRect{};
    int footerTop = 0;
};

// ------------------------------- rendering -----------------------------------

void AddRoundRect(Gdiplus::GraphicsPath* path, float x, float y, float w, float h,
                  float radius) {
    float d = radius * 2;
    d = (std::min)(d, (std::min)(w, h));
    if (d <= 0.0f) {
        path->AddRectangle(Gdiplus::RectF(x, y, w, h));
        return;
    }
    path->AddArc(x, y, d, d, 180.0f, 90.0f);
    path->AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
    path->AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
    path->AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
    path->CloseFigure();
}

void PaintPill(Gdiplus::Graphics* graphics, const RECT& rect, int radius,
               Gdiplus::Color fill, bool hasBorder, Gdiplus::Color border) {
    float w = static_cast<float>(rect.right - rect.left);
    float h = static_cast<float>(rect.bottom - rect.top);

    Gdiplus::GraphicsPath fillPath;
    AddRoundRect(&fillPath, static_cast<float>(rect.left),
                 static_cast<float>(rect.top), w, h, static_cast<float>(radius));
    Gdiplus::SolidBrush brush(fill);
    graphics->FillPath(&brush, &fillPath);

    if (hasBorder) {
        Gdiplus::GraphicsPath borderPath;
        AddRoundRect(&borderPath, rect.left + 0.5f, rect.top + 0.5f, w - 1.0f,
                     h - 1.0f, static_cast<float>(radius));
        Gdiplus::Pen pen(border, 1.0f);
        graphics->DrawPath(&pen, &borderPath);
    }
}

void DrawButtonItem(State* state, DRAWITEMSTRUCT* dis) {
    const Button* button = nullptr;
    for (const Button& candidate : state->buttons) {
        if (static_cast<int>(dis->CtlID) == candidate.id) {
            button = &candidate;
            break;
        }
    }
    bool isClose = dis->CtlID == kCloseButtonId;
    if (!button && !isClose) {
        return;
    }

    RECT rect = dis->rcItem;
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    bool hot = GetPropW(dis->hwndItem, kHotProp) != nullptr;
    bool pressed = (dis->itemState & ODS_SELECTED) != 0;
    bool focused = (dis->itemState & ODS_FOCUS) != 0;

    HDC mem = CreateCompatibleDC(dis->hDC);
    HBITMAP bitmap = CreateCompatibleBitmap(dis->hDC, width, height);
    HGDIOBJ oldBitmap = SelectObject(mem, bitmap);

    const Palette& p = state->palette;
    RECT local{0, 0, width, height};
    HBRUSH backdrop = CreateSolidBrush(isClose ? p.body : p.footer);
    FillRect(mem, &local, backdrop);
    DeleteObject(backdrop);

    COLORREF textColor = p.text;
    if (!g_gdiplusReady) {
        // GDI fallback: aliased corners, but the dialog still renders.
        COLORREF fill = p.buttonFace;
        bool hasBorder = true;
        if (isClose) {
            hasBorder = false;
            fill = (hot || pressed) ? Shade(p.body, p.dark ? 30 : -20) : p.body;
        } else if (button->isDefault) {
            hasBorder = false;
            fill = pressed ? p.accentDown : (hot ? p.accentHot : p.accent);
            textColor = p.accentText;
        } else {
            fill = pressed ? p.buttonDown : (hot ? p.buttonHot : p.buttonFace);
        }

        HBRUSH brush = CreateSolidBrush(fill);
        HPEN pen = hasBorder ? CreatePen(PS_SOLID, 1, p.buttonBorder)
                             : CreatePen(PS_SOLID, 1, fill);
        HGDIOBJ oldBrush = SelectObject(mem, brush);
        HGDIOBJ oldPen = SelectObject(mem, pen);
        int diameter = state->metrics.buttonRadius * 2;
        RoundRect(mem, local.left, local.top, local.right, local.bottom, diameter,
                  diameter);
        SelectObject(mem, oldBrush);
        SelectObject(mem, oldPen);
        DeleteObject(brush);
        DeleteObject(pen);
    } else {
        Gdiplus::Graphics graphics(mem);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        if (isClose) {
            if (hot || pressed) {
                COLORREF fill = pressed ? Shade(p.body, p.dark ? 24 : -24)
                                        : Shade(p.body, p.dark ? 34 : -16);
                PaintPill(&graphics, local, state->metrics.buttonRadius,
                          ToGdi(fill), false, ToGdi(fill));
            }
        } else if (button->isDefault) {
            COLORREF fill =
                pressed ? p.accentDown : (hot ? p.accentHot : p.accent);
            PaintPill(&graphics, local, state->metrics.buttonRadius, ToGdi(fill),
                      false, ToGdi(fill));
            textColor = p.accentText;
        } else {
            COLORREF fill =
                pressed ? p.buttonDown : (hot ? p.buttonHot : p.buttonFace);
            PaintPill(&graphics, local, state->metrics.buttonRadius, ToGdi(fill),
                      true, ToGdi(p.buttonBorder));
        }

        if (focused) {
            RECT ring = local;
            InflateRect(&ring, -2, -2);
            Gdiplus::GraphicsPath path;
            AddRoundRect(&path, ring.left + 0.5f, ring.top + 0.5f,
                         static_cast<float>(ring.right - ring.left) - 1.0f,
                         static_cast<float>(ring.bottom - ring.top) - 1.0f,
                         static_cast<float>(state->metrics.buttonRadius));
            Gdiplus::Pen pen(ToGdi(textColor, 170), 1.0f);
            graphics.DrawPath(&pen, &path);
        }
    }

    SetBkMode(mem, TRANSPARENT);
    SetTextColor(mem, textColor);
    SelectObject(mem, isClose ? state->fonts.glyph : state->fonts.button);

    UINT flags = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
    if (!state->showPrefix) {
        flags |= DT_HIDEPREFIX;
    }
    PCWSTR label = isClose ? L"✕" : button->text.c_str();
    DrawTextW(mem, label, -1, &local, isClose ? (DT_CENTER | DT_VCENTER |
                                                 DT_SINGLELINE | DT_NOPREFIX)
                                             : flags);

    BitBlt(dis->hDC, rect.left, rect.top, width, height, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(mem);
}

void OnPaint(State* state, HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT client;
    GetClientRect(hwnd, &client);

    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bitmap = CreateCompatibleBitmap(hdc, client.right, client.bottom);
    HGDIOBJ oldBitmap = SelectObject(mem, bitmap);

    HBRUSH bodyBrush = CreateSolidBrush(state->palette.body);
    FillRect(mem, &client, bodyBrush);
    DeleteObject(bodyBrush);

    RECT footer{0, state->footerTop, client.right, client.bottom};
    HBRUSH footerBrush = CreateSolidBrush(state->palette.footer);
    FillRect(mem, &footer, footerBrush);
    DeleteObject(footerBrush);

    SetBkMode(mem, TRANSPARENT);
    SetTextColor(mem, state->palette.text);

    SelectObject(mem, state->fonts.title);
    RECT titleRect = state->titleRect;
    DrawTextW(mem, state->title.c_str(), -1, &titleRect,
              DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER);

    if (state->icon) {
        DrawIconEx(mem, state->iconRect.left, state->iconRect.top, state->icon,
                   state->iconRect.right - state->iconRect.left,
                   state->iconRect.bottom - state->iconRect.top, 0, nullptr,
                   DI_NORMAL);
    }

    SelectObject(mem, state->fonts.body);
    RECT textRect = state->textRect;
    DrawTextW(mem, state->text.c_str(), -1, &textRect,
              DT_WORDBREAK | DT_NOPREFIX | DT_EDITCONTROL);

    BitBlt(hdc, 0, 0, client.right, client.bottom, mem, 0, 0, SRCCOPY);
    SelectObject(mem, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(mem);
    EndPaint(hwnd, &ps);
}

// -------------------------------- plumbing -----------------------------------

struct DisableContext {
    std::vector<HWND>* disabled;
    HWND except;
};

// Real __stdcall function rather than a lambda, for the same 32-bit reason as
// FontEnumProc.
BOOL CALLBACK DisableThreadWindowProc(HWND hwnd, LPARAM param) {
    auto* context = reinterpret_cast<DisableContext*>(param);
    if (hwnd != context->except && IsWindowVisible(hwnd) &&
        IsWindowEnabled(hwnd)) {
        EnableWindow(hwnd, FALSE);
        context->disabled->push_back(hwnd);
    }
    return TRUE;
}

LRESULT CALLBACK ButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto original = reinterpret_cast<WNDPROC>(GetPropW(hwnd, kOldProcProp));
    auto forward = [&]() -> LRESULT {
        return original ? CallWindowProcW(original, hwnd, msg, wParam, lParam)
                        : DefWindowProcW(hwnd, msg, wParam, lParam);
    };

    switch (msg) {
        case WM_MOUSEMOVE:
            if (!GetPropW(hwnd, kHotProp)) {
                SetPropW(hwnd, kHotProp, reinterpret_cast<HANDLE>(1));
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&track);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            break;
        case WM_MOUSELEAVE:
            RemovePropW(hwnd, kHotProp);
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        case WM_NCDESTROY: {
            LRESULT result = forward();
            RemovePropW(hwnd, kHotProp);
            RemovePropW(hwnd, kOldProcProp);
            return result;
        }
    }
    return forward();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* state =
        reinterpret_cast<State*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            if (state) {
                OnPaint(state, hwnd);
                return 0;
            }
            break;

        case WM_DRAWITEM:
            if (state) {
                DrawButtonItem(state, reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
                return TRUE;
            }
            break;

        case WM_COMMAND: {
            if (!state) {
                break;
            }
            int id = LOWORD(wParam);
            HWND control = reinterpret_cast<HWND>(lParam);
            // Esc arrives as IDCANCEL with no control handle; the X button has
            // its own id. Both resolve to the mapped cancel result.
            if (id == kCloseButtonId || (id == IDCANCEL && !control)) {
                if (state->cancelId) {
                    state->result = state->cancelId;
                    state->done = true;
                }
                return 0;
            }
            if (control && HIWORD(wParam) == BN_CLICKED) {
                state->result = id;
                state->done = true;
            }
            return 0;
        }

        case WM_CLOSE:
            if (state && state->cancelId) {
                state->result = state->cancelId;
                state->done = true;
            }
            return 0;

        case WM_NCHITTEST: {
            LRESULT hit = DefWindowProcW(hwnd, msg, wParam, lParam);
            if (hit == HTCLIENT && state) {
                POINT pt{static_cast<short>(LOWORD(lParam)),
                         static_cast<short>(HIWORD(lParam))};
                ScreenToClient(hwnd, &pt);
                if (pt.y < state->titleRect.bottom &&
                    pt.x < state->closeRect.left) {
                    return HTCAPTION;
                }
            }
            return hit;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool EnsureClass() {
    std::call_once(g_classOnce, [] {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = ModuleInstance();
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kClassName;
        g_classAtom = RegisterClassExW(&wc);
    });
    return g_classAtom != 0;
}

void Cleanup() {
    if (g_classAtom) {
        UnregisterClassW(kClassName, ModuleInstance());
        g_classAtom = 0;
    }
    if (g_gdiplusReady) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusReady = false;
    }
}

// --------------------------------- layout ------------------------------------

void ComputeLayout(State* state, HDC hdc, int* clientWidth, int* clientHeight) {
    const Metrics& m = state->metrics;

    HGDIOBJ oldFont = SelectObject(hdc, state->fonts.title);
    RECT measure{0, 0, 0, 0};
    DrawTextW(hdc, state->title.c_str(), -1, &measure,
              DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
    int titleWidth = measure.right;
    int titleHeight = measure.bottom;

    SelectObject(hdc, state->fonts.body);
    RECT textMeasure{0, 0, m.maxTextWidth, 0};
    DrawTextW(hdc, state->text.c_str(), -1, &textMeasure,
              DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX | DT_EDITCONTROL);
    int textWidth = textMeasure.right;
    int textHeight = textMeasure.bottom;

    SelectObject(hdc, state->fonts.button);
    int buttonsWidth = 0;
    for (Button& button : state->buttons) {
        RECT buttonMeasure{0, 0, 0, 0};
        DrawTextW(hdc, button.text.c_str(), -1, &buttonMeasure,
                  DT_CALCRECT | DT_SINGLELINE);
        button.width = (std::max)(
            m.buttonMinWidth,
            static_cast<int>(buttonMeasure.right) + m.buttonPadX * 2);
        buttonsWidth += button.width;
    }
    if (!state->buttons.empty()) {
        buttonsWidth +=
            static_cast<int>(state->buttons.size() - 1) * m.buttonGap;
    }
    SelectObject(hdc, oldFont);

    int closeWidth = state->showClose ? m.closeSize + m.iconGap : 0;
    int contentWidth = textWidth + (state->icon ? m.iconSize + m.iconGap : 0);
    int width = (std::max)(
        {m.minWidth, m.padding * 2 + contentWidth, m.padding * 2 + buttonsWidth,
         m.padding * 2 + titleWidth + closeWidth});

    int y = m.padding;
    state->closeRect = {width - m.padding / 2 - m.closeSize, m.padding / 2,
                        width - m.padding / 2, m.padding / 2 + m.closeSize};
    if (!state->showClose) {
        state->closeRect.left = width;
    }
    state->titleRect = {m.padding, y, state->closeRect.left - m.iconGap,
                        y + titleHeight};
    y += titleHeight + m.titleGap;

    int contentHeight =
        (std::max)(textHeight, state->icon ? m.iconSize : 0);
    if (state->icon) {
        int iconTop = y + (contentHeight - m.iconSize) / 2;
        state->iconRect = {m.padding, iconTop, m.padding + m.iconSize,
                           iconTop + m.iconSize};
    }
    int textLeft = m.padding + (state->icon ? m.iconSize + m.iconGap : 0);
    int textTop = y + (contentHeight - textHeight) / 2;
    state->textRect = {textLeft, textTop, textLeft + textWidth,
                       textTop + textHeight};

    y += contentHeight + m.padding;
    state->footerTop = y;

    int buttonTop = y + m.footerPadding;
    int x = width - m.padding - buttonsWidth;
    for (Button& button : state->buttons) {
        button.x = x;
        x += button.width + m.buttonGap;
    }

    *clientWidth = width;
    *clientHeight = buttonTop + m.buttonHeight + m.footerPadding;
}

// ---------------------------------- show -------------------------------------

void SubclassButton(HWND button) {
    auto original = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(button, GWLP_WNDPROC,
                          reinterpret_cast<LONG_PTR>(ButtonProc)));
    SetPropW(button, kOldProcProp, reinterpret_cast<HANDLE>(original));
}

void CenterOnOwner(HWND hwnd, HWND owner, int width, int height) {
    RECT anchor{};
    if (owner && IsWindow(owner) && GetWindowRect(owner, &anchor) &&
        (anchor.right - anchor.left) > 0) {
        // keep it
    } else {
        HMONITOR monitor = MonitorFromWindow(owner ? owner : hwnd,
                                             MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO info{sizeof(info)};
        GetMonitorInfoW(monitor, &info);
        anchor = info.rcWork;
    }

    int x = anchor.left + ((anchor.right - anchor.left) - width) / 2;
    int y = anchor.top + ((anchor.bottom - anchor.top) - height) / 2;

    HMONITOR monitor = MonitorFromPoint(POINT{x + width / 2, y + height / 2},
                                        MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if (GetMonitorInfoW(monitor, &info)) {
        x = (std::min)((std::max)(x, static_cast<int>(info.rcWork.left)),
                       static_cast<int>(info.rcWork.right) - width);
        y = (std::min)((std::max)(y, static_cast<int>(info.rcWork.top)),
                       static_cast<int>(info.rcWork.bottom) - height);
    }
    SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
}

void ApplyWindowChrome(HWND hwnd, const Palette& palette) {
    BOOL dark = palette.dark ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, kDwmUseImmersiveDarkMode, &dark, sizeof(dark));

    DWORD corner = kDwmCornerRound;
    DwmSetWindowAttribute(hwnd, kDwmWindowCornerPreference, &corner,
                          sizeof(corner));

    COLORREF border = palette.border;
    DwmSetWindowAttribute(hwnd, kDwmBorderColor, &border, sizeof(border));
}

int Show(HWND owner, PCWSTR title, PCWSTR text, UINT type, int defaultId,
         int cancelId, std::vector<Button> buttons) {
    // GDI+ is best-effort now; DrawButtonItem falls back to GDI without it.
    bool gdiplus = EnsureGdiplus();
    bool registered = EnsureClass();
    Diag(L"Show: gdiplus=%d class=%d(atom=%u,err=%lu) buttons=%d", (int)gdiplus,
         (int)registered, (unsigned)g_classAtom, GetLastError(),
         (int)buttons.size());

    if (!registered || buttons.empty()) {
        Diag(L"Show: bailing out before window creation");
        return 0;
    }

    State state;
    state.title = title && *title ? title : L"";
    state.text = text ? text : L"";
    state.buttons = std::move(buttons);
    state.defaultId = defaultId;
    state.cancelId = cancelId;
    state.showClose = cancelId != 0;
    state.palette = GetPalette();

    BOOL cues = TRUE;
    SystemParametersInfoW(SPI_GETKEYBOARDCUES, 0, &cues, 0);
    state.showPrefix = cues != FALSE;

    UINT beep = MB_OK;
    switch (type & MB_ICONMASK) {
        case MB_ICONERROR:
            state.icon = LoadIconW(nullptr, IDI_ERROR);
            beep = MB_ICONERROR;
            break;
        case MB_ICONWARNING:
            state.icon = LoadIconW(nullptr, IDI_WARNING);
            beep = MB_ICONWARNING;
            break;
        case MB_ICONINFORMATION:
            state.icon = LoadIconW(nullptr, IDI_INFORMATION);
            beep = MB_ICONINFORMATION;
            break;
        case MB_ICONQUESTION:
            state.icon = LoadIconW(nullptr, IDI_QUESTION);
            beep = MB_ICONQUESTION;
            break;
        default:
            break;
    }

    HWND validOwner = (owner && IsWindow(owner)) ? owner : nullptr;

    HWND hwnd = CreateWindowExW(WS_EX_CONTROLPARENT, kClassName, state.title.c_str(),
                                WS_POPUP | WS_CLIPCHILDREN, 0, 0, 10, 10,
                                validOwner, nullptr, ModuleInstance(), nullptr);
    if (!hwnd) {
        Diag(L"Show: CreateWindowExW failed, err=%lu", GetLastError());
        return 0;
    }
    Diag(L"Show: window created %p", (void*)hwnd);
    state.hwnd = hwnd;
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&state));

    state.dpi = GetDpiForWindowSafe(hwnd);
    state.metrics = ScaleMetrics(state.dpi);
    state.fonts.Create(state.dpi);

    int width = 0;
    int height = 0;
    HDC hdc = GetDC(hwnd);
    ComputeLayout(&state, hdc, &width, &height);
    ReleaseDC(hwnd, hdc);

    int buttonTop = state.footerTop + state.metrics.footerPadding;
    for (Button& button : state.buttons) {
        button.hwnd = CreateWindowExW(
            0, L"BUTTON", button.text.c_str(),
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, button.x, buttonTop,
            button.width, state.metrics.buttonHeight, hwnd,
            reinterpret_cast<HMENU>(static_cast<UINT_PTR>(button.id)),
            ModuleInstance(), nullptr);
        if (button.hwnd) {
            SubclassButton(button.hwnd);
        }
    }
    if (state.showClose) {
        state.closeButton = CreateWindowExW(
            0, L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            state.closeRect.left, state.closeRect.top,
            state.closeRect.right - state.closeRect.left,
            state.closeRect.bottom - state.closeRect.top, hwnd,
            reinterpret_cast<HMENU>(static_cast<UINT_PTR>(kCloseButtonId)),
            ModuleInstance(), nullptr);
        if (state.closeButton) {
            SubclassButton(state.closeButton);
        }
    }

    ApplyWindowChrome(hwnd, state.palette);
    CenterOnOwner(hwnd, validOwner, width, height);

    bool ownerWasEnabled = validOwner && IsWindowEnabled(validOwner);
    if (ownerWasEnabled) {
        EnableWindow(validOwner, FALSE);
    }

    // MB_TASKMODAL disables every top-level window on the calling thread. The
    // owner is disabled just above, so it is already skipped here.
    std::vector<HWND> disabledWindows;
    if (type & (MB_TASKMODAL | MB_SYSTEMMODAL)) {
        DisableContext context{&disabledWindows, hwnd};
        EnumThreadWindows(GetCurrentThreadId(), DisableThreadWindowProc,
                          reinterpret_cast<LPARAM>(&context));
    }

    MessageBeep(beep);
    ShowWindow(hwnd, SW_SHOW);
    // MB_SYSTEMMODAL means topmost in modern Windows.
    if (type & (MB_TOPMOST | MB_SYSTEMMODAL)) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    SetForegroundWindow(hwnd);

    for (const Button& button : state.buttons) {
        if (button.id == defaultId && button.hwnd) {
            SetFocus(button.hwnd);
            break;
        }
    }

    MSG msg;
    while (!state.done) {
        BOOL got = GetMessageW(&msg, nullptr, 0, 0);
        if (got <= 0) {
            if (got == 0) {
                // WM_QUIT belongs to the app's own loop; put it back.
                PostQuitMessage(static_cast<int>(msg.wParam));
            }
            break;
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN) {
            HWND focus = GetFocus();
            int id = 0;
            if (focus && GetParent(focus) == hwnd) {
                id = GetDlgCtrlID(focus);
            }
            if (!id || id == kCloseButtonId) {
                id = state.defaultId;
            }
            if (id) {
                state.result = id;
                state.done = true;
                continue;
            }
        }
        if (!IsDialogMessageW(hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    for (HWND window : disabledWindows) {
        EnableWindow(window, TRUE);
    }
    if (ownerWasEnabled) {
        EnableWindow(validOwner, TRUE);
    }
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
    DestroyWindow(hwnd);
    if (validOwner && ownerWasEnabled) {
        SetActiveWindow(validOwner);
    }

    return state.result;
}

}  // namespace customdlg

// user32 string table ids for the standard message box button captions. The
// Abort/Retry/Ignore trio was verified on-screen; the rest of the block follows
// the same layout, and a miss only costs an English fallback.
std::wstring LabelForButtonId(int id) {
    switch (id) {
        case IDOK:
            return LoadUserStringOr(800, L"OK");
        case IDCANCEL:
            return LoadUserStringOr(801, L"Cancel");
        case IDABORT:
            return LoadUserStringOr(802, L"&Abort");
        case IDRETRY:
            return LoadUserStringOr(803, L"&Retry");
        case IDIGNORE:
            return LoadUserStringOr(804, L"&Ignore");
        case IDYES:
            return LoadUserStringOr(805, L"&Yes");
        case IDNO:
            return LoadUserStringOr(806, L"&No");
        case IDTRYAGAIN:
            return LoadUserStringOr(809, L"&Try Again");
        case IDCONTINUE:
            return LoadUserStringOr(810, L"&Continue");
        default:
            return L"OK";
    }
}

bool TryCustomDialog(HWND owner, const WCHAR* text, const WCHAR* caption,
                     UINT type, const ButtonPlan& plan, int* result) {
    customdlg::Diag(L"TryCustomDialog: entered, type=0x%08X, order=%d", type,
                    (int)plan.order.size());

    // The custom renderer does not mirror itself, so RTL keeps the task dialog.
    if (type & MB_RTLREADING) {
        customdlg::Diag(L"TryCustomDialog: RTL, deferring to task dialog");
        return false;
    }

    std::vector<customdlg::Button> buttons;
    buttons.reserve(plan.order.size());

    size_t defaultIndex = 0;
    switch (type & MB_DEFMASK) {
        case MB_DEFBUTTON2:
            defaultIndex = 1;
            break;
        case MB_DEFBUTTON3:
            defaultIndex = 2;
            break;
        case MB_DEFBUTTON4:
            defaultIndex = 3;
            break;
        default:
            break;
    }
    if (defaultIndex >= plan.order.size()) {
        defaultIndex = 0;
    }
    int defaultId = plan.order.empty() ? 0 : plan.order[defaultIndex];

    for (size_t i = 0; i < plan.order.size(); i++) {
        customdlg::Button button;
        button.id = plan.order[i];
        button.text = LabelForButtonId(button.id);
        button.isDefault = button.id == defaultId;
        buttons.push_back(std::move(button));
    }

    int cancelId = plan.allowCancellation ? plan.cancelMapsTo : 0;

    int pressed = customdlg::Show(owner, caption ? caption : L"Error",
                                 text ? text : L"", type, defaultId, cancelId,
                                 std::move(buttons));
    if (!pressed) {
        return false;
    }

    if (g_settings.verboseLog) {
        Wh_Log(L"MessageBox(type=0x%08X) -> custom dialog, result %d", type,
               pressed);
    }
    *result = pressed;
    return true;
}

bool TryTaskDialog(HWND owner, const WCHAR* text, const WCHAR* caption, UINT type,
                   int* result) {
    // Logged before any guard: a dialog that stays unmodernized is nearly always
    // rejected by one of these, and knowing which one is the whole diagnosis.
    customdlg::Diag(L"entry: type=0x%08X owner=%p modernize=%d renderer=%s", type,
                    (void*)owner, (int)g_settings.modernizeMessageBox,
                    g_settings.renderer == Renderer::Custom ? L"custom"
                                                            : L"taskdialog");

    if (!g_settings.modernizeMessageBox || tls_inTaskDialog) {
        customdlg::Diag(L"  reject: disabled or re-entrant");
        return false;
    }

    constexpr UINT kUnsupported =
        MB_HELP | MB_SERVICE_NOTIFICATION | MB_DEFAULT_DESKTOP_ONLY;
    if (type & kUnsupported) {
        customdlg::Diag(L"  reject: unsupported flags (0x%08X)",
                        type & kUnsupported);
        return false;
    }

    ButtonPlan plan = PlanButtons(type);
    if (!plan.supported) {
        customdlg::Diag(L"  reject: button set 0x%X unsupported",
                        type & MB_TYPEMASK);
        return false;
    }

    if (g_settings.renderer == Renderer::Custom &&
        TryCustomDialog(owner, text, caption, type, plan, result)) {
        return true;
    }

    // Only the task dialog has to give up here. It cannot disable a thread's
    // windows the way MB_TASKMODAL requires, whereas the custom renderer
    // implements that itself. Windows Script Host shows its MsgBox task-modal,
    // so checking this before the custom renderer made VBScript unreachable.
    if (g_settings.fallbackOnModal && (type & (MB_SYSTEMMODAL | MB_TASKMODAL))) {
        customdlg::Diag(L"TryTaskDialog: modal flags, deferring to MessageBox");
        return false;
    }

    if (!EnsureTaskDialog()) {
        return false;
    }

    std::wstring instruction;
    std::wstring content;
    SplitMessage(text, &instruction, &content);

    TASKDIALOGCONFIG config{};
    config.cbSize = sizeof(config);
    config.hwndParent = IsWindow(owner) ? owner : nullptr;
    config.dwFlags = TDF_SIZE_TO_CONTENT;
    if (config.hwndParent) {
        config.dwFlags |= TDF_POSITION_RELATIVE_TO_WINDOW;
    }
    if (plan.allowCancellation) {
        config.dwFlags |= TDF_ALLOW_DIALOG_CANCELLATION;
    }
    if (type & MB_RTLREADING) {
        config.dwFlags |= TDF_RTL_LAYOUT;
    }

    config.pszWindowTitle = caption ? caption : L"Error";
    config.pszMainInstruction = instruction.empty() ? nullptr : instruction.c_str();
    config.pszContent = content.empty() ? nullptr : content.c_str();
    config.dwCommonButtons = plan.common;
    config.pfCallback = TaskDialogCallback;
    config.lpCallbackData = static_cast<LONG_PTR>(type);

    // Task dialog plays its own sound for the standard icons only.
    UINT beep = 0xFFFFFFFF;
    switch (type & MB_ICONMASK) {
        case MB_ICONERROR:
            config.pszMainIcon = TD_ERROR_ICON;
            break;
        case MB_ICONWARNING:
            config.pszMainIcon = TD_WARNING_ICON;
            break;
        case MB_ICONINFORMATION:
            config.pszMainIcon = TD_INFORMATION_ICON;
            break;
        case MB_ICONQUESTION:
            // Task dialog has no question icon; borrow the system one.
            config.hMainIcon = LoadIconW(nullptr, IDI_QUESTION);
            config.dwFlags |= TDF_USE_HICON_MAIN;
            beep = MB_ICONQUESTION;
            break;
        default:
            beep = MB_OK;
            break;
    }

    std::vector<TASKDIALOG_BUTTON> buttons;
    if (!plan.customText.empty()) {
        buttons.reserve(plan.customText.size());
        for (size_t i = 0; i < plan.customText.size(); i++) {
            TASKDIALOG_BUTTON button{};
            button.nButtonID = plan.customIds[i];
            button.pszButtonText = plan.customText[i].c_str();
            buttons.push_back(button);
        }
        config.pButtons = buttons.data();
        config.cButtons = static_cast<UINT>(buttons.size());
    }

    size_t defaultIndex = 0;
    switch (type & MB_DEFMASK) {
        case MB_DEFBUTTON2:
            defaultIndex = 1;
            break;
        case MB_DEFBUTTON3:
            defaultIndex = 2;
            break;
        case MB_DEFBUTTON4:
            defaultIndex = 3;
            break;
        default:
            defaultIndex = 0;
            break;
    }
    if (defaultIndex < plan.order.size()) {
        config.nDefaultButton = plan.order[defaultIndex];
    }

    if (beep != 0xFFFFFFFF) {
        MessageBeep(beep);
    }

    int pressed = 0;
    HRESULT hr;
    {
        tls_inTaskDialog = true;
        ActCtxScope scope(g_actCtx);
        hr = g_taskDialogIndirect(&config, &pressed, nullptr, nullptr);
        tls_inTaskDialog = false;
    }

    if (FAILED(hr)) {
        Wh_Log(L"TaskDialogIndirect failed: 0x%08X", static_cast<unsigned>(hr));
        return false;
    }

    if (pressed == 0) {
        pressed = IDCANCEL;
    }
    if (pressed == IDCANCEL && plan.cancelMapsTo != IDCANCEL) {
        pressed = plan.cancelMapsTo;
    }

    if (g_settings.verboseLog) {
        Wh_Log(L"MessageBox(type=0x%08X) -> task dialog, result %d", type, pressed);
    }

    *result = pressed;
    return true;
}

using MessageBoxW_t = decltype(&MessageBoxW);
MessageBoxW_t MessageBoxW_Original;

using MessageBoxA_t = decltype(&MessageBoxA);
MessageBoxA_t MessageBoxA_Original;

using MessageBoxExW_t = decltype(&MessageBoxExW);
MessageBoxExW_t MessageBoxExW_Original;

using MessageBoxExA_t = decltype(&MessageBoxExA);
MessageBoxExA_t MessageBoxExA_Original;

int WINAPI MessageBoxW_Hook(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption,
                            UINT uType) {
    int result = 0;
    if (TryTaskDialog(hWnd, lpText, lpCaption, uType, &result)) {
        return result;
    }
    return MessageBoxW_Original(hWnd, lpText, lpCaption, uType);
}

int WINAPI MessageBoxExW_Hook(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption,
                              UINT uType, WORD wLanguageId) {
    int result = 0;
    if (TryTaskDialog(hWnd, lpText, lpCaption, uType, &result)) {
        return result;
    }
    return MessageBoxExW_Original(hWnd, lpText, lpCaption, uType, wLanguageId);
}

bool AnsiToWide(const char* ansi, std::wstring* wide) {
    if (!ansi) {
        wide->clear();
        return true;
    }
    int needed = MultiByteToWideChar(CP_ACP, 0, ansi, -1, nullptr, 0);
    if (needed <= 0) {
        return false;
    }
    wide->resize(needed - 1);
    return MultiByteToWideChar(CP_ACP, 0, ansi, -1, wide->data(), needed) > 0;
}

int WINAPI MessageBoxA_Hook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption,
                            UINT uType) {
    std::wstring text;
    std::wstring caption;
    if (AnsiToWide(lpText, &text) && AnsiToWide(lpCaption, &caption)) {
        int result = 0;
        if (TryTaskDialog(hWnd, lpText ? text.c_str() : nullptr,
                          lpCaption ? caption.c_str() : nullptr, uType, &result)) {
            return result;
        }
    }
    return MessageBoxA_Original(hWnd, lpText, lpCaption, uType);
}

int WINAPI MessageBoxExA_Hook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption,
                              UINT uType, WORD wLanguageId) {
    std::wstring text;
    std::wstring caption;
    if (AnsiToWide(lpText, &text) && AnsiToWide(lpCaption, &caption)) {
        int result = 0;
        if (TryTaskDialog(hWnd, lpText ? text.c_str() : nullptr,
                          lpCaption ? caption.c_str() : nullptr, uType, &result)) {
            return result;
        }
    }
    return MessageBoxExA_Original(hWnd, lpText, lpCaption, uType, wLanguageId);
}

using MessageBoxIndirectW_t = decltype(&MessageBoxIndirectW);
MessageBoxIndirectW_t MessageBoxIndirectW_Original;

using MessageBoxIndirectA_t = decltype(&MessageBoxIndirectA);
MessageBoxIndirectA_t MessageBoxIndirectA_Original;

// MSGBOXPARAMS strings are either pointers or string-table identifiers. Note
// that IS_INTRESOURCE(nullptr) is true, so the null check has to come first.
bool ResolveMsgBoxStringW(HINSTANCE instance, LPCWSTR value, std::wstring* storage,
                          LPCWSTR* resolved) {
    if (!value) {
        *resolved = nullptr;
        return true;
    }
    if (!IS_INTRESOURCE(value)) {
        *resolved = value;
        return true;
    }
    WCHAR buffer[1024];
    int length =
        LoadStringW(instance, static_cast<UINT>(reinterpret_cast<ULONG_PTR>(value)),
                    buffer, ARRAYSIZE(buffer));
    if (length <= 0) {
        return false;
    }
    storage->assign(buffer, length);
    *resolved = storage->c_str();
    return true;
}

bool ResolveMsgBoxStringA(LPCSTR value, std::wstring* storage, LPCWSTR* resolved) {
    if (!value) {
        *resolved = nullptr;
        return true;
    }
    if (IS_INTRESOURCE(value)) {
        return false;  // resource id in an ANSI module; not worth emulating
    }
    if (!AnsiToWide(value, storage)) {
        return false;
    }
    *resolved = storage->c_str();
    return true;
}

int WINAPI MessageBoxIndirectW_Hook(const MSGBOXPARAMSW* lpmbp) {
    // A custom icon has no task dialog equivalent. A help callback by itself is
    // not disqualifying: VBScript installs one on every MsgBox whether or not a
    // helpfile was given, and without MB_HELP (rejected in TryTaskDialog) it can
    // only ever fire for F1 - a binding the classic dialog only makes meaningful
    // when there is a helpfile, which implies MB_HELP.
    if (lpmbp && lpmbp->cbSize >= sizeof(MSGBOXPARAMSW) &&
        (lpmbp->dwStyle & MB_ICONMASK) != MB_USERICON) {
        std::wstring textStorage;
        std::wstring captionStorage;
        LPCWSTR text = nullptr;
        LPCWSTR caption = nullptr;
        if (ResolveMsgBoxStringW(lpmbp->hInstance, lpmbp->lpszText, &textStorage,
                                 &text) &&
            ResolveMsgBoxStringW(lpmbp->hInstance, lpmbp->lpszCaption,
                                 &captionStorage, &caption)) {
            int result = 0;
            if (TryTaskDialog(lpmbp->hwndOwner, text, caption, lpmbp->dwStyle,
                              &result)) {
                return result;
            }
        }
    }
    return MessageBoxIndirectW_Original(lpmbp);
}

int WINAPI MessageBoxIndirectA_Hook(const MSGBOXPARAMSA* lpmbp) {
    // Same callback reasoning as the W hook above.
    if (lpmbp && lpmbp->cbSize >= sizeof(MSGBOXPARAMSA) &&
        (lpmbp->dwStyle & MB_ICONMASK) != MB_USERICON) {
        std::wstring textStorage;
        std::wstring captionStorage;
        LPCWSTR text = nullptr;
        LPCWSTR caption = nullptr;
        if (ResolveMsgBoxStringA(lpmbp->lpszText, &textStorage, &text) &&
            ResolveMsgBoxStringA(lpmbp->lpszCaption, &captionStorage, &caption)) {
            int result = 0;
            if (TryTaskDialog(lpmbp->hwndOwner, text, caption, lpmbp->dwStyle,
                              &result)) {
                return result;
            }
        }
    }
    return MessageBoxIndirectA_Original(lpmbp);
}

// -----------------------------------------------------------------------------
// Common file dialogs -> IFileDialog
// -----------------------------------------------------------------------------

template <typename T>
struct ComPtr {
    T* p = nullptr;

    ComPtr() = default;
    ~ComPtr() {
        if (p) {
            p->Release();
        }
    }
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;

    T** operator&() { return &p; }
    T* operator->() const { return p; }
    explicit operator bool() const { return p != nullptr; }
};

struct ComScope {
    bool initialized = false;
    bool ok = false;

    ComScope() {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED |
                                                 COINIT_DISABLE_OLE1DDE);
        if (hr == RPC_E_CHANGED_MODE) {
            // Thread is in an MTA; IFileDialog needs an STA.
            ok = false;
            return;
        }
        initialized = SUCCEEDED(hr);
        ok = true;
    }
    ~ComScope() {
        if (initialized) {
            CoUninitialize();
        }
    }
    ComScope(const ComScope&) = delete;
    ComScope& operator=(const ComScope&) = delete;
};

FILEOPENDIALOGOPTIONS MapOfnFlags(DWORD flags, bool save) {
    FILEOPENDIALOGOPTIONS options = 0;
    if (flags & OFN_ALLOWMULTISELECT) {
        options |= FOS_ALLOWMULTISELECT;
    }
    if (flags & OFN_FILEMUSTEXIST) {
        options |= FOS_FILEMUSTEXIST;
    }
    if (flags & OFN_PATHMUSTEXIST) {
        options |= FOS_PATHMUSTEXIST;
    }
    if (flags & OFN_CREATEPROMPT) {
        options |= FOS_CREATEPROMPT;
    }
    if (flags & OFN_NODEREFERENCELINKS) {
        options |= FOS_NODEREFERENCELINKS;
    }
    if (flags & OFN_DONTADDTORECENT) {
        options |= FOS_DONTADDTORECENT;
    }
    if (flags & OFN_FORCESHOWHIDDEN) {
        options |= FOS_FORCESHOWHIDDEN;
    }
    if (flags & OFN_NOVALIDATE) {
        options |= FOS_NOVALIDATE;
    }
    if (save && (flags & OFN_OVERWRITEPROMPT)) {
        options |= FOS_OVERWRITEPROMPT;
    }
    return options;
}

void SplitPath(const std::wstring& full, std::wstring* directory,
               std::wstring* name) {
    size_t slash = full.find_last_of(L"\\/");
    if (slash == std::wstring::npos) {
        directory->clear();
        *name = full;
        return;
    }
    *directory = full.substr(0, slash);
    *name = full.substr(slash + 1);
}

// Builds the double-null-terminated result buffer the legacy API promises.
bool WriteResultBuffer(OPENFILENAMEW* ofn, const std::vector<std::wstring>& paths) {
    std::wstring buffer;

    if (paths.size() == 1) {
        buffer = paths[0];
        buffer.push_back(L'\0');
    } else {
        std::wstring directory;
        std::wstring name;
        SplitPath(paths[0], &directory, &name);

        buffer = directory;
        buffer.push_back(L'\0');
        for (const std::wstring& path : paths) {
            std::wstring itemDirectory;
            std::wstring itemName;
            SplitPath(path, &itemDirectory, &itemName);
            buffer += itemName;
            buffer.push_back(L'\0');
        }
    }
    buffer.push_back(L'\0');  // terminating empty string

    if (!ofn->lpstrFile || buffer.size() > ofn->nMaxFile) {
        return false;
    }
    memcpy(ofn->lpstrFile, buffer.data(), buffer.size() * sizeof(WCHAR));

    // Offsets are only meaningful for the single-selection form; Windows reports
    // the position of the first name in the multi-selection form.
    std::wstring directory;
    std::wstring name;
    SplitPath(paths[0], &directory, &name);

    if (paths.size() == 1) {
        ofn->nFileOffset = static_cast<WORD>(directory.empty() ? 0
                                                               : directory.size() + 1);
    } else {
        ofn->nFileOffset = static_cast<WORD>(directory.size() + 1);
    }

    size_t dot = name.find_last_of(L'.');
    ofn->nFileExtension =
        dot == std::wstring::npos
            ? 0
            : static_cast<WORD>(ofn->nFileOffset + dot + 1);

    if (ofn->lpstrFileTitle && ofn->nMaxFileTitle > name.size()) {
        memcpy(ofn->lpstrFileTitle, name.c_str(),
               (name.size() + 1) * sizeof(WCHAR));
    }

    if (g_settings.emulateChangeDir && !(ofn->Flags & OFN_NOCHANGEDIR) &&
        !directory.empty()) {
        SetCurrentDirectoryW(directory.c_str());
    }

    return true;
}

bool TryModernFileDialog(OPENFILENAMEW* ofn, bool save, BOOL* result) {
    if (!g_settings.modernizeFileDialogs || !ofn) {
        return false;
    }
    if (ofn->lStructSize < OPENFILENAME_SIZE_VERSION_400W) {
        return false;
    }

    // Anything that injects app-owned UI into the dialog has no IFileDialog
    // equivalent worth faking.
    constexpr DWORD kUnsupported = OFN_ENABLEHOOK | OFN_ENABLETEMPLATE |
                                   OFN_ENABLETEMPLATEHANDLE |
                                   OFN_ENABLEINCLUDENOTIFY;
    if (ofn->Flags & kUnsupported) {
        return false;
    }
    if ((ofn->Flags & OFN_ALLOWMULTISELECT) && !(ofn->Flags & OFN_EXPLORER)) {
        return false;
    }
    if (save && (ofn->Flags & OFN_ALLOWMULTISELECT)) {
        return false;
    }

    ComScope com;
    if (!com.ok) {
        return false;
    }

    ComPtr<IFileDialog> dialog;
    HRESULT hr = CoCreateInstance(save ? CLSID_FileSaveDialog : CLSID_FileOpenDialog,
                                  nullptr, CLSCTX_INPROC_SERVER, IID_IFileDialog,
                                  reinterpret_cast<void**>(&dialog));
    if (FAILED(hr) || !dialog) {
        return false;
    }

    FILEOPENDIALOGOPTIONS options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | MapOfnFlags(ofn->Flags, save) |
                       FOS_FORCEFILESYSTEM);

    std::vector<COMDLG_FILTERSPEC> filters;
    if (ofn->lpstrFilter) {
        const WCHAR* cursor = ofn->lpstrFilter;
        while (*cursor) {
            const WCHAR* label = cursor;
            cursor += wcslen(cursor) + 1;
            if (!*cursor) {
                break;
            }
            const WCHAR* pattern = cursor;
            cursor += wcslen(cursor) + 1;
            filters.push_back({label, pattern});
        }
    }
    if (!filters.empty()) {
        dialog->SetFileTypes(static_cast<UINT>(filters.size()), filters.data());
        if (ofn->nFilterIndex >= 1 &&
            ofn->nFilterIndex <= filters.size()) {
            dialog->SetFileTypeIndex(ofn->nFilterIndex);
        }
    }

    if (ofn->lpstrTitle && *ofn->lpstrTitle) {
        dialog->SetTitle(ofn->lpstrTitle);
    }
    if (ofn->lpstrDefExt && *ofn->lpstrDefExt) {
        dialog->SetDefaultExtension(ofn->lpstrDefExt);
    }

    // The incoming lpstrFile may hold a bare name, a full path, or nothing.
    std::wstring initialDirectory;
    if (ofn->lpstrFile && *ofn->lpstrFile) {
        std::wstring name;
        SplitPath(ofn->lpstrFile, &initialDirectory, &name);
        if (!name.empty()) {
            dialog->SetFileName(name.c_str());
        }
    }
    if (initialDirectory.empty() && ofn->lpstrInitialDir &&
        *ofn->lpstrInitialDir) {
        initialDirectory = ofn->lpstrInitialDir;
    }
    if (!initialDirectory.empty()) {
        ComPtr<IShellItem> folder;
        if (SUCCEEDED(SHCreateItemFromParsingName(
                initialDirectory.c_str(), nullptr, IID_IShellItem,
                reinterpret_cast<void**>(&folder)))) {
            dialog->SetFolder(folder.p);
        }
    }

    hr = dialog->Show(IsWindow(ofn->hwndOwner) ? ofn->hwndOwner : nullptr);

    if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        *result = FALSE;
        return true;
    }
    if (FAILED(hr)) {
        return false;
    }

    std::vector<std::wstring> paths;

    if (!save && (ofn->Flags & OFN_ALLOWMULTISELECT)) {
        ComPtr<IFileOpenDialog> openDialog;
        if (FAILED(dialog->QueryInterface(IID_IFileOpenDialog,
                                          reinterpret_cast<void**>(&openDialog)))) {
            return false;
        }
        ComPtr<IShellItemArray> items;
        if (FAILED(openDialog->GetResults(&items))) {
            return false;
        }
        DWORD count = 0;
        items->GetCount(&count);
        for (DWORD i = 0; i < count; i++) {
            ComPtr<IShellItem> item;
            if (FAILED(items->GetItemAt(i, &item))) {
                continue;
            }
            PWSTR path = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) && path) {
                paths.emplace_back(path);
                CoTaskMemFree(path);
            }
        }
    } else {
        ComPtr<IShellItem> item;
        if (FAILED(dialog->GetResult(&item))) {
            return false;
        }
        PWSTR path = nullptr;
        if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) || !path) {
            return false;
        }
        paths.emplace_back(path);
        CoTaskMemFree(path);
    }

    if (paths.empty()) {
        return false;
    }

    // If the caller's buffer is too small, hand the call back to comdlg32 rather
    // than reporting a cancellation the app would misread.
    if (!WriteResultBuffer(ofn, paths)) {
        return false;
    }

    UINT typeIndex = 0;
    if (!filters.empty() && SUCCEEDED(dialog->GetFileTypeIndex(&typeIndex)) &&
        typeIndex >= 1) {
        ofn->nFilterIndex = typeIndex;
    }

    if (g_settings.verboseLog) {
        Wh_Log(L"%s -> modern picker, %d item(s)",
               save ? L"GetSaveFileNameW" : L"GetOpenFileNameW",
               static_cast<int>(paths.size()));
    }

    *result = TRUE;
    return true;
}

using GetOpenFileNameW_t = decltype(&GetOpenFileNameW);
GetOpenFileNameW_t GetOpenFileNameW_Original;

using GetSaveFileNameW_t = decltype(&GetSaveFileNameW);
GetSaveFileNameW_t GetSaveFileNameW_Original;

BOOL WINAPI GetOpenFileNameW_Hook(LPOPENFILENAMEW ofn) {
    BOOL result = FALSE;
    if (TryModernFileDialog(ofn, false, &result)) {
        return result;
    }
    return GetOpenFileNameW_Original(ofn);
}

BOOL WINAPI GetSaveFileNameW_Hook(LPOPENFILENAMEW ofn) {
    BOOL result = FALSE;
    if (TryModernFileDialog(ofn, true, &result)) {
        return result;
    }
    return GetSaveFileNameW_Original(ofn);
}

// -----------------------------------------------------------------------------
// Deferred hooking of comdlg32
// -----------------------------------------------------------------------------

LONG g_comdlgHooked = 0;
bool g_initFinished = false;

void TryHookComdlg32() {
    if (InterlockedCompareExchange(&g_comdlgHooked, 1, 0) != 0) {
        return;
    }

    HMODULE comdlg = GetModuleHandleW(L"comdlg32.dll");
    if (!comdlg) {
        InterlockedExchange(&g_comdlgHooked, 0);
        return;
    }

    FARPROC openProc = GetProcAddress(comdlg, "GetOpenFileNameW");
    FARPROC saveProc = GetProcAddress(comdlg, "GetSaveFileNameW");
    if (openProc) {
        Wh_SetFunctionHook(reinterpret_cast<void*>(openProc),
                           reinterpret_cast<void*>(GetOpenFileNameW_Hook),
                           reinterpret_cast<void**>(&GetOpenFileNameW_Original));
    }
    if (saveProc) {
        Wh_SetFunctionHook(reinterpret_cast<void*>(saveProc),
                           reinterpret_cast<void*>(GetSaveFileNameW_Hook),
                           reinterpret_cast<void**>(&GetSaveFileNameW_Original));
    }

    if (g_initFinished) {
        Wh_ApplyHookOperations();
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

using LoadLibraryW_t = decltype(&LoadLibraryW);
LoadLibraryW_t LoadLibraryW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    constexpr DWORD kDataOnly = LOAD_LIBRARY_AS_DATAFILE |
                                LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                                LOAD_LIBRARY_AS_IMAGE_RESOURCE;
    if (module && !(dwFlags & kDataOnly)) {
        TryHookComdlg32();
    }
    return module;
}

HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR lpLibFileName) {
    HMODULE module = LoadLibraryW_Original(lpLibFileName);
    if (module) {
        TryHookComdlg32();
    }
    return module;
}

// -----------------------------------------------------------------------------
// Mod entry points
// -----------------------------------------------------------------------------

MessageSplit ParseMessageSplit(PCWSTR value) {
    if (value && wcscmp(value, L"instruction") == 0) {
        return MessageSplit::Instruction;
    }
    if (value && wcscmp(value, L"content") == 0) {
        return MessageSplit::Content;
    }
    return MessageSplit::Auto;
}

void LoadSettings() {
    g_settings.modernizeMessageBox = Wh_GetIntSetting(L"modernizeMessageBox") != 0;
    g_settings.handleRareButtonSets =
        Wh_GetIntSetting(L"handleRareButtonSets") != 0;
    g_settings.fallbackOnModal = Wh_GetIntSetting(L"fallbackOnModal") != 0;
    g_settings.modernizeFileDialogs =
        Wh_GetIntSetting(L"modernizeFileDialogs") != 0;
    g_settings.emulateChangeDir = Wh_GetIntSetting(L"emulateChangeDir") != 0;
    g_settings.verboseLog = Wh_GetIntSetting(L"verboseLog") != 0;

    auto split = WindhawkUtils::StringSetting::make(L"messageSplit");
    g_settings.messageSplit = ParseMessageSplit(split.get());

    auto renderer = WindhawkUtils::StringSetting::make(L"renderer");
    g_settings.renderer =
        renderer.get() && wcscmp(renderer.get(), L"taskdialog") == 0
            ? Renderer::TaskDialog
            : Renderer::Custom;
}
BOOL Wh_ModInit() {
    LoadSettings();

    WindhawkUtils::SetFunctionHook(MessageBoxW, MessageBoxW_Hook,
                                   &MessageBoxW_Original);
    WindhawkUtils::SetFunctionHook(MessageBoxA, MessageBoxA_Hook,
                                   &MessageBoxA_Original);
    WindhawkUtils::SetFunctionHook(MessageBoxExW, MessageBoxExW_Hook,
                                   &MessageBoxExW_Original);
    WindhawkUtils::SetFunctionHook(MessageBoxExA, MessageBoxExA_Hook,
                                   &MessageBoxExA_Original);
    WindhawkUtils::SetFunctionHook(MessageBoxIndirectW, MessageBoxIndirectW_Hook,
                                   &MessageBoxIndirectW_Original);
    WindhawkUtils::SetFunctionHook(MessageBoxIndirectA, MessageBoxIndirectA_Hook,
                                   &MessageBoxIndirectA_Original);

    WindhawkUtils::SetFunctionHook(LoadLibraryExW, LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);
    WindhawkUtils::SetFunctionHook(LoadLibraryW, LoadLibraryW_Hook,
                                   &LoadLibraryW_Original);

    TryHookComdlg32();
    return TRUE;
}

void Wh_ModAfterInit() {
    g_initFinished = true;
    // Covers a comdlg32 that arrived while init was running.
    TryHookComdlg32();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // The window class points at code in this module, so it has to go before the
    // module does.
    customdlg::Cleanup();

    if (g_actCtx != INVALID_HANDLE_VALUE) {
        ReleaseActCtx(g_actCtx);
        g_actCtx = INVALID_HANDLE_VALUE;
    }
}
