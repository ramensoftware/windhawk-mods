// ==WindhawkMod==
// @id              fix-white-flash
// @name            Fix white flashes for all windows
// @description     No more white flashes when opening new windows and dialogs!
// @version         0.1
// @author          Rafaello
// @github          https://github.com/JoyHak
// @homepage        https://github.com/JoyHak/windhawk
// @license         MIT
// @include *
// @exclude conhost.exe
// @exclude TextInputHost.exe
// @exclude audiodg.exe
// @exclude wmic.exe
// @exclude alg.exe
// @exclude nvcplui.exe
// @exclude nvcontainer.exe
// @exclude git.exe
// @exclude windhawk.exe
// @exclude windhawk-cli.exe
// @exclude VSCodium.exe
// @exclude clang++.exe
// @exclude clang-20.exe
// @exclude DbgViewMini.exe
// @exclude ld.lld.exe
// @compilerOptions -lGdi32
// ==/WindhawkMod==

// Source code is published under the MIT license.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/JoyHak/windhawk

// ==WindhawkModReadme==
/*
### Before
![Before](https://raw.githubusercontent.com/JoyHak/windhawk/main/images/before.gif)

### After
![After](https://raw.githubusercontent.com/JoyHak/windhawk/main/images/after.gif)

Even a custom dark theme can't fix the long-standing issue with white areas in Win32 applications.
This mod automatically detects and paints all white regions before you can even see them.

### Colors
On the "Settings" tab, you can specify a color for each process.
For other processes not listed here, the global settings apply.
The color can be in one of the following formats:
- `RGB(r, g, b)` or `rgb(r, g, b)` (e.g. `RGB(25, 25, 25)` or `rgb(17,0,0)`)
- `#RRGGBB` or `0xRRGGBB` (e.g. `#191919` - default).
- Decimal number (e.g. `1644825` = `0x191919` as hexadecimal)

Use color picker from ShareX or PowerToys to quickly select color from screen.

---

Works on Windows 10 and 11 (starting from 21h1).

Also you can install the [UxTheme mod](https://windhawk.net/mods/uxtheme-hook) and set up the [CakeOS theme](https://www.deviantart.com/niivu/art/cakeOS-2-0-for-Windows-11-953541433)
by niivu to get best dark theme experience (works on Windows 10 too).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Global:
    - backgroundColor: "0x191919"
      $name: Background Color
      $description: Enter hex (#RRGGBB or 0xRRGGBB) or RGB(r,g,b)

  $name: Global Settings
  $description: These settings affect all processes and their windows.

- Process:
  - - name: ""
      $name: Process Name
      $description: base name + .exe (explorer.exe, notepad++.exe)

    - backgroundColor: "0x191919"
      $name: Background Color
      $description: Enter hex (#RRGGBB or 0xRRGGBB) or RGB(r,g,b)

  $name: Settings per Process
  $description: >-
    You can set individual parameters for each process.
    Click "Add new item" below to add a new process.
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>
#include <mutex>
#include <string>
#include <unordered_map>

#define DCX_USESTYLE  0x00010000L

using std::wstring;
using std::unordered_map;
using Lock = std::lock_guard<std::mutex>;
using DefProcCallback = WNDPROC;  // for clarity

// == Helpers ==

template <typename T>
struct Defer {
	T f;
	Defer(T f) : f(f) { };
	Defer(const Defer&) = delete;
	~Defer() { f(); }
};

#define CONCAT(a, b) a##b
#define defer Defer CONCAT(_defer, __COUNTER__) =

// == Data ==

/**
 * @brief Holds marks that window and its root ancestor
 * are rendered (skip painting).
 */
class Win {
    static constexpr uint8_t kPainted = 1;  // window has at least one paint
    static constexpr uint8_t kCreated = 2;  // window and it's root is created

  public:
    inline static void setPainted(HWND hWnd, HWND root = NULL) {
        set(hWnd, root, kPainted);
    }
    inline static void unsetPainted(HWND hWnd, HWND root = NULL) {
        unset(hWnd, root, kPainted);
    }
    inline static bool painted(HWND hWnd, HWND root = NULL) {
        return test(hWnd, root, kPainted);
    }

    inline static void setCreated(HWND hWnd, HWND root = NULL) {
        set(hWnd, root, kCreated);
    }
    inline static void unsetCreated(HWND hWnd, HWND root = NULL) {
        unset(hWnd, root, kCreated);
    }
    inline static bool created(HWND hWnd, HWND root = NULL) {
        return test(hWnd, root, kCreated);
    }

    /**
     * @brief Erases states for a window and it's root (optionally)
     */
    static void erase(HWND hWnd, HWND root = NULL) {
        Lock lock(s_mutex);
        s_windows.erase(hWnd);

        if (root)
            s_windows.erase(root);
    }

    /**
     * @brief Erases all states and redraws stored windows
     */
    static void clear() {
        decltype(s_windows) windows;
        {
            Lock lock(s_mutex);
            windows.swap(s_windows);
        }

        for (auto& win : windows) {
            if (IsWindow(win.first)) {
                RedrawWindow(
                    win.first, NULL, NULL,
                    RDW_FRAME | RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN
                );
            }
        }
    }

    Win() = delete;
    Win(const Win&) = delete;
    Win& operator=(const Win&) = delete;
    ~Win() = delete;

  private:
    inline static void set(HWND hWnd, HWND root, uint8_t value) {
        Lock lock(s_mutex);
        s_windows[hWnd] |= value;

        if (root)
            s_windows[root] |= value;
    }

    inline static void unset(HWND hWnd, HWND root, uint8_t value) {
        Lock lock(s_mutex);
        s_windows[hWnd] &= ~value;

        if (root)
            s_windows[root] &= ~value;
    }

    inline static bool test(HWND hWnd, HWND root, uint8_t value) {
        Lock lock(s_mutex);

        auto i = s_windows.find(hWnd);
        if (i != s_windows.end()
        && (i->second & value)) {
            return true;
        }

        if (!root) {
            return false;
        }

        auto j = s_windows.find(root);
        if (j != s_windows.end()
        && (j->second & value)) {
            return true;
        }

        return false;
    }

    inline static std::mutex s_mutex;
    inline static unordered_map<HWND, uint8_t> s_windows{};
};

/**
 * @brief Returns trimmed name of the current process in lower case.
 */
wstring GetCurrentProcessName() {
    WCHAR pathBuffer[1024] {0};  // path can be longer than 260
    DWORD pathLength =
        GetModuleFileNameW(NULL, pathBuffer, ARRAYSIZE(pathBuffer));

    if (!pathLength)
        return {};

    wstring path{ pathBuffer, pathLength };

    // Find the file name part
    size_t lastSlash = path.find_last_of(L"\\/");
    if (lastSlash == wstring::npos)
        return {};

    path.erase(0, lastSlash + 1);

    // Process name should be lower case
    std::transform(
        path.begin(), path.end(),
        path.begin(),
        std::towlower
    );

    return path;
}

class Cfg {
  public:
    static bool load() {
        unload();  // safe cleanup

        if (loadProcessValues()
         || loadGlobalValues()) {
            return true;
        }

        return false;
    }

    static void unload() {
        if (s_brush) {
            DeleteObject(s_brush);
        }
    }

    inline static HBRUSH get() { return s_brush; }

    Cfg() = delete;
    Cfg(const Cfg&) = delete;
    Cfg& operator=(const Cfg&) = delete;
    ~Cfg() = delete;

  private:
    /**
    * @brief Returns trimmed process name in lower case.
    */
    template <typename... Args>
    static wstring getProcessName(PCWSTR valueName, Args... args) {
        PCWSTR value = Wh_GetStringSetting(valueName, args...);
        defer [&] { Wh_FreeStringSetting(value); };

        if (*value == L'\0')
            return {};

        wstring name{ value };
        name.erase(0, name.find_first_not_of(L" \t\v\n"));  // left trim
        name.erase(name.find_last_not_of(L" \t\v\n") + 1);  // right trim

        if (name.empty())
            return {};

        // Process name should be in lower case
        std::transform(
            name.begin(), name.end(),
            name.begin(),
            std::towlower
        );
        return name;
    }

    // Helpers for color parsing

    static UINT clamp(UINT value, UINT low, UINT high) {
        if (value < low)
            return low;
        if (value > high)
            return high;

        return value;
    }

    static COLORREF toColor(UINT rgb) {
        UINT r = (rgb >> 16) & 0xFF;
        UINT g = (rgb >> 8)  & 0xFF;
        UINT b = rgb & 0xFF;

        return RGB(r, g, b);
    }

    /**
     * @brief Parses color from user: #RRGGBB, 0xRRGGBB, RGB(r,g,b)
     * @returns Integer that represents RGB (not COLORREF!).
     */
    template <typename... Args>
    static UINT parseColor(PCWSTR valueName, Args... args) {
        UINT color = kInvalidColor;
        PCWSTR value = Wh_GetStringSetting(valueName, args...);

        defer [&] {
            if (color == kInvalidColor) {
                Wh_Log(L"Color \"%s\" is incorrect! Fall back to 0x%06x", value, kDefaultColor);
            } else {
                Wh_Log(L"Color \"%s\" -> 0x%06x", value, color);
            }
            Wh_FreeStringSetting(value);
        };

        if (*value == L'\0') {
            return kDefaultColor;
        }

        const wchar_t* p = value;
        while (*p && iswspace(*p)) {
            ++p;
        }
        if (*p == L'\0') {
            return kDefaultColor;
        }

        // Parse RGB(r,g,b).
        const bool isRgb =
            (p[0] == L'r' || p[0] == L'R')
         && (p[1] == L'g' || p[1] == L'G')
         && (p[2] == L'b' || p[2] == L'B');

        if (isRgb) {
            p += 3;
            while (*p && iswspace(*p)) {
                ++p;
            }
            if (*p != L'(') {
                return kDefaultColor;
            }
            ++p;

            UINT rgb[3] { 0, 0, 0 };

            for (int i = 0; i < 3; ++i) {
                while (*p && iswspace(*p)) {
                    ++p;
                }

                bool hasDigit = false;
                UINT number   = 0;
                while (*p >= L'0' && *p <= L'9') {
                    hasDigit = true;
                    number   = number * 10 + (*p - L'0');
                    ++p;
                }

                if (!hasDigit)
                    return kDefaultColor;

                rgb[i] = number;

                while (*p == L',') {
                    ++p;
                }
            }

            const UINT r = clamp(rgb[0], 0, 255);
            const UINT g = clamp(rgb[1], 0, 255);
            const UINT b = clamp(rgb[2], 0, 255);

            color = (r << 16) | (g << 8) | b;
            return color;
        }

        // Determine number base (decimal/hexadecimal)
        ULONG base = 10;
        if (p[0] == L'#') {
            // #RRGGBB
            base = 16;
            p += 1;
        } else if (p[0] == L'0'
        && (p[1] == L'x' || p[1] == L'X')) {
            // 0xRRGGBB
            base = 16;
            p += 2;
        } else {
            // A number
            const wchar_t* t = p;
            while (*t && !iswspace(*t)) {
                if ((*t >= L'A' && *t <= L'F')
                 || (*t >= L'a' && *t <= L'f')) {
                    base = 16;
                    break;
                }
                ++t;
            }
        }

        // Convert string to number
        bool hasDigit = false;
        ULONG number = 0;

        while (*p != L'\0') {
            ULONG digit = 0;

            if (*p >= L'0' && *p <= L'9')
                digit = *p - L'0';
            else if (*p >= L'A' && *p <= L'F')
                digit = *p - L'A' + 10;
            else if (*p >= L'a' && *p <= L'f')
                digit = *p - L'a' + 10;
            else
                break;

            if (digit >= base)
                return kDefaultColor;

            hasDigit = true;
            number   = number * base + digit;
            ++p;
        }

        if (!hasDigit)
            return kDefaultColor;

        color = static_cast<UINT>(number & 0xFFFFFFUL);
        return color;
    }

    /**
     * @brief Creates solid brush and returns true on success.
     */
    static bool tryCreateBrush(UINT rgb, PCWSTR valueType = L"process") {
        HBRUSH brush = CreateSolidBrush(toColor(rgb));
        if (brush) {
            s_brush = brush;
            return true;
        }

        Wh_Log(
            L"Failed to create %s brush 0x%06x!",
            valueType, rgb
        );
        return false;
    }

    static bool loadGlobalValues() {
        UINT backColor = parseColor(L"Global.backgroundColor");
        if (tryCreateBrush(backColor, L"global"))
            return true;

        if (tryCreateBrush(kDefaultColor, L"default"))
            return true;

        return false;
    }

    static bool loadProcessValues() {
        wstring currentName = ::GetCurrentProcessName();
        if (currentName.empty()) {
            Wh_Log(L"Failed to retrieve current process name!");
            return false;
        }

        for (int i = 0;; ++i) {
            wstring name = Cfg::getProcessName(L"Process[%d].name", i);
            if (name.empty())
                break;
            if (name != currentName)
                continue;

            UINT backColor = parseColor(L"Process[%d].backgroundColor", i);
            return tryCreateBrush(backColor, L"process");
        }

        Wh_Log(L"\"%s\" not found in the settings. Use global values", currentName.c_str());
        return false;
    }

    // inline = declaration also a definition
    inline static HBRUSH s_brush = NULL;
    static constexpr UINT kDefaultColor = 0x191919;
    static constexpr UINT kInvalidColor = UINT_MAX;
};

// == Main ==

inline HWND GetRoot(HWND hWnd) {
    return GetAncestor(hWnd, GA_ROOT);
}

/**
 * @brief Checks if element is child, small, border or not main window.
 */
static bool ShouldSkip(HWND hWnd) {
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if ((style & WS_CHILD)
    || !(style & WS_CAPTION)
    || !(style & WS_THICKFRAME))
        return true;

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED)
        return true;

    return false;
}

/**
 * @brief Covers the white background with a colored rectangle
 * while the window is rendering.
 */
LRESULT FillWindow(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, DefProcCallback original) {
    if (!hWnd) {
        return original(hWnd, Msg, wParam, lParam);
    }

    switch (Msg) {
    case WM_NCPAINT: {
        // This message handling is responsible for most regions.
        // We're painting the non-client area first and
        // then original() draws chrome elements
        // (caption buttons, borders) on top of it.
        if (Win::painted(hWnd, GetRoot(hWnd)))
            break;

        HRGN hrgn = (HRGN)wParam;
        HDC  hdc{};

        // paint the NC area
        if (hrgn == (HRGN)1) {
            hdc = GetDCEx(hWnd, NULL, DCX_WINDOW | DCX_USESTYLE | 0);
        } else {
            hdc = GetDCEx(hWnd, hrgn, DCX_WINDOW | DCX_USESTYLE | DCX_INTERSECTRGN);
        }
        if (!hdc) {
            break;
        }

        RECT rect{};
        if (!GetWindowRect(hWnd, &rect)) {
            ReleaseDC(hWnd, hdc);
            break;
        }

        rect = {
            0, 0,  // left upper corner
            rect.right  - rect.left,
            rect.bottom - rect.top
        };

        FillRect(hdc, &rect, Cfg::get());
        ReleaseDC(hWnd, hdc);
        Win::setPainted(hWnd);  // prevent any flicks

        break;
    }
    case WM_ERASEBKGND: {
        // This message is common for dialogs.
        // Erase white background and replace it
        // with colorful rectangle.
        // It will be removed later and
        // window elements will become visible.
        if (Win::painted(hWnd, GetRoot(hWnd)))
            break;
        if (ShouldSkip(hWnd))  // prevent any visual issues
            break;

        RECT rect{};
        if (!GetClientRect(hWnd, &rect))
            break;

        HDC hdc = (HDC)wParam;
        if (!hdc)
            break;

        FillRect(hdc, &rect, Cfg::get());
        return TRUE;  // don't let the original erase it again
    }
    case WM_THEMECHANGED:
    case WM_SYSCOLORCHANGE:
    case WM_DPICHANGED:
    case WM_SETTINGCHANGE:
    case WM_ENABLE:
    case WM_SETFOCUS:
    case WM_KILLFOCUS: {
        // Window is ready for painting
        // after switch back to the window
        HWND root = GetRoot(hWnd);
        if (Win::created(hWnd, root)) {
            Win::erase(hWnd, root);
        }
        break;
    }
    case WM_CREATE: {
        // Prepare for above
        Win::setCreated(hWnd, GetRoot(hWnd));
        break;
    }
    case WM_NCDESTROY: {
        // Window is destroyed.
        // Forget all states to paint again later.
        Win::erase(hWnd, GetRoot(hWnd));
        break;
    }
    } // switch

    // Draw all required elements and regions
    return original(hWnd, Msg, wParam, lParam);
}

// == Hook rendering procedures ==

decltype(&DefWindowProcA) DefWindowProcA_Original = nullptr;
decltype(&DefWindowProcW) DefWindowProcW_Original = nullptr;
decltype(&DefDlgProcA)    DefDlgProcA_Original    = nullptr;
decltype(&DefDlgProcW)    DefDlgProcW_Original    = nullptr;

LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    return FillWindow(hWnd, Msg, wParam, lParam, DefWindowProcA_Original);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    return FillWindow(hWnd, Msg, wParam, lParam, DefWindowProcW_Original);
}

LRESULT WINAPI DefDlgProcA_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam){
    return FillWindow(hWnd, Msg, wParam, lParam, DefDlgProcA_Original);
}

LRESULT WINAPI DefDlgProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam){
    return FillWindow(hWnd, Msg, wParam, lParam, DefDlgProcW_Original);
}

BOOL Wh_ModInit() {
    if (Cfg::load()) {
        Wh_Log(L"Settings loaded");
    } else {
        Wh_Log(L"Failed load settings!");
        return FALSE;
    }

    using WindhawkUtils::SetFunctionHook;

    if (!SetFunctionHook(DefWindowProcA, DefWindowProcA_Hook, &DefWindowProcA_Original))
        Wh_Log(L"Failed to hook DefWindowProcA!");
    if (!SetFunctionHook(DefWindowProcW, DefWindowProcW_Hook, &DefWindowProcW_Original))
        Wh_Log(L"Failed to hook DefWindowProcW!");
    if (!SetFunctionHook(DefDlgProcA, DefDlgProcA_Hook, &DefDlgProcA_Original))
        Wh_Log(L"Failed to hook DefDlgProcA!");
    if (!SetFunctionHook(DefDlgProcW, DefDlgProcW_Hook, &DefDlgProcW_Original))
        Wh_Log(L"Failed to hook DefDlgProcW!");

    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL*) {
    if (Cfg::load()) {
        Wh_Log(L"Settings reloaded");
        return TRUE;
    } else {
        Wh_Log(L"Failed to reload settings - unloading...");
        return FALSE;
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    Cfg::unload();
    Win::clear();
<<<<<<< New base: Fix doxygen
    Process::clear();
}
||||||| Common ancestor
    Process::clear();
}
=======
}
>>>>>>> Current commit: Fixed leaks and reduced mod size
