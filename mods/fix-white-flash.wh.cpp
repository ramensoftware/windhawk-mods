// ==WindhawkMod==
// @id                fix-white-flash
// @name            Fix white flashes for all windows
// @description     Fixes white flashes when opening new window.
// @version         0.1
// @author          Rafaello
// @github          https://github.com/JoyHak
// @include *
// @exclude csrss.exe
// @exclude dwm.exe
// @exclude winlogon.exe
// @exclude services.exe
// @exclude svchost.exe
// @exclude lsass.exe
// @exclude smss.exe
// @exclude wininit.exe
// @exclude conhost.exe
// @exclude fontdrvhost.exe
// @exclude audiodg.exe
// @exclude wmic.exe
// @exclude wmiapsrv.exe
// @exclude wmiprvse.exe
// @exclude alg.exe
// @exclude nvcplui.exe
// @exclude nvcontainer.exe
// @exclude git.exe
// @exclude windhawk.exe
// @exclude windhawk-cli.exe
// @exclude VSCodium.exe
// @exclude clang++.exe
// @exclude clang-20.exe
// @exclude ld.lld.exe
// @exclude TextInputHost.exe
// @compilerOptions -lGdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Fixes white flashes when opening new windows.

### Before
![Before](https://raw.githubusercontent.com/MGGSK/MGGSK/refs/heads/main/WindhawkModReadmeImages/fix-explorer-white-flash-before.png)

### After
![After](https://raw.githubusercontent.com/MGGSK/MGGSK/refs/heads/main/WindhawkModReadmeImages/fix-explorer-white-flash-after.png)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Global:
    - backgroundColor: "0x191919"
      $name: Background Color
      $description: Enter hex (#RRGGBB or 0xRRGGBB) or RGB(r,g,b)

    - aggressivePaint: true
      $name: Aggressive Painting
      $description: >-
        Aggressively search for white regions and paint them in with the chosen color.
        May affect the appearance and rendering of windows!
        Fixes white flickering while changing the window size.

    - longerPaint: true
      $name: Longer Painting
      $description: >-
        Paint the white regions for a longer period.
        Window elements will appear more slowly.
        Guaranteed to paint all windows and child elements.

  $name: Global Settings
  $description: These settings affect all processes and their windows.

- Process:
  - - name: ""
      $name: Process Name
      $description: base name + .exe (explorer.exe, notepad++.exe)

    - backgroundColor: "0x191919"
      $name: Background Color
      $description: Enter hex (#RRGGBB or 0xRRGGBB) or RGB(r,g,b)

    - aggressivePaint: true
      $name: Aggressive Painting
      $description: >-
        Aggressively search for white regions and fill them in with the chosen color.
        May affect the appearance and rendering of windows!
        Fixes white flickering while changing the window size.

    - longerPaint: true
      $name: Longer Painting
      $description: >-
        Paint the white regions for a longer period.
        Window elements will appear more slowly.
        Guaranteed to paint all windows and child elements.

  $name: Settings per Process
  $description: >-
    You can set individual parameters for each process.
    Click "Add new item" below to add a new process.

- verbose: false
  $name: Verbose Logging
  $description: >-
    Output additional messages to the "output" tab and
    `user-data\logs\......\*-Windhawk Log.log`
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <concepts>     // for logging overloads

#define DCX_USESTYLE  0x00010000L

using std::wstring;
using std::unordered_map;
using Lock = std::lock_guard<std::mutex>;
using DefProcCallback = WNDPROC;

// == Verbose Logging ==

/**
 * @brief Formats a narrow (char) printf-style string and append its converted UTF-16
 * representation to a wide output string using `MultiByteToWideChar`
 * with the specified @p codePage.
 *
 * @remark It is used to convert narrow ANSI string from
 * `__builtin_dump_struct` intrinsic (see `Dump` below) to wide UTF-16 string because
 * `Wh_Log` (macro around `InternalWh_Log_Wrapper`) expects wide string.
 * https://clang.llvm.org/docs/LanguageExtensions.html#builtin-dump-struct
 * @see Dump
 *
 * @param[out] out
 *     `std::wstring` to which the converted wide text will be appended.
 *
 * @param[in] codePage
 *     Win32 code page identifier passed to `MultiByteToWideChar` (CP_UTF8, CP_ACP, ...)
 *     The chosen code page determines how the narrow bytes are interpreted
 *     when converting to UTF-16.
 *
 * @param[in] format
 *     printf-style format string. Must be non-null.
 *
 * @param[in] args
 *     Arguments corresponding to @p format. They are forwarded to `std::snprintf`.
 *
 * @warning Performs no modifications to @p out if any step fails (null @p format,
 * formatting error, or conversion failure); performs no any locale-aware normalization
 * beyond the specified code page conversion.
 */
template<typename... Args>
void ToWide(wstring& out, const UINT codePage, const char* format, Args&& ...args) {
    if (!format)
        return;

    // determine required size for narrow formatted string
    int narrowLen = std::snprintf(
        nullptr, 0,
        format,
        std::forward<Args>(args)...
    );
    if (narrowLen < 0)
        return;

    // allocate narrow buffer and format into it
    // (include space for terminating NUL)
    std::string narrow{};
    narrow.resize(static_cast<size_t>(narrowLen) + 1);
    std::snprintf(
        narrow.data(),
        narrow.size(),
        format,
        std::forward<Args>(args)...
    );

    // convert narrow to wide
    int wideLen = MultiByteToWideChar(
        codePage, 0,
        narrow.c_str(),
        -1, nullptr, 0
    );
    if (wideLen <= 0)
        return;

    // wideLen includes terminating NUL;
    // resize to exclude the trailing null when appending
    wstring wide{};
    wide.resize(static_cast<size_t>(wideLen) - 1);

    MultiByteToWideChar(
        codePage, 0,
        narrow.c_str(),
        -1, wide.data(),
        wideLen
    );

    out += wide;
}

namespace dbg {
/**
* @brief Dumps @p obj contents into the string.
* Supports primitives, strings and objects.
* Dumps public and private fields, their type, name and value.
*
* @param[out] out
*     Target string to which the @p obj contents will be appended.
* @param[in] obj
*     Any object, struct, string or primitive.
*/
template<typename T>
void Dump(wstring& out, const T& obj) {
    if constexpr (std::is_same_v<std::remove_cv_t<T>, wstring>) {
        out += L"\"" + obj + L"\"";
        return;
    }

    constexpr UINT codePage = CP_ACP;  // ANSI string
    if constexpr (std::is_same_v<std::remove_cv_t<T>, std::string>) {
        ToWide(out, codePage, "\"%s\"", obj.data());
        return;
    }

    wstring tmp{};
    size_t start{}, end{};

    if constexpr (std::is_class_v<T> || std::is_union_v<T>) {
        __builtin_dump_struct(&obj, &ToWide, tmp, codePage);

        // Trim class/struct type
        start = tmp.find(L'{');
        end   = tmp.rfind(L'}');

        if (start != wstring::npos) {
            start += 3;
            out += L'{';
        }
        if (end != wstring::npos) {
            end += 1;
        }
    } else {
        struct { T value; } v { obj };
        __builtin_dump_struct(&v, &ToWide, tmp, codePage);

        // Trim struct wrapper
        start = tmp.find(L'=');
        end   = tmp.rfind(L'}');

        if (start != wstring::npos)
            start += 8;
        if (end != wstring::npos)
            end -= 1;
    }

    if (start == wstring::npos && end == wstring::npos) {
        out += tmp;
        return;
    }

    if (start == wstring::npos)
        start = 0;

    if (end == wstring::npos)
        out += tmp.substr(start);
    else
        out += tmp.substr(start, end - start);
}

template<typename T>
concept pair = requires (T t) {
    typename T::first_type;
    typename T::second_type;
    { t.first }  -> std::same_as<typename T::first_type&>;
    { t.second } -> std::same_as<typename T::second_type&>;
};

template<typename Cont>
concept container = requires (Cont t) {
    { std::begin(t) } -> std::input_or_output_iterator;
    { std::end(t)   } -> std::input_or_output_iterator;
    t.size();
    t.empty();
};

template<typename Cont>
concept container_pairs = container<Cont> && pair<typename Cont::value_type>;

template<typename Cont>
concept container_linear = container<Cont>;
// concept container_linear = container<Cont> && (!pair<typename Cont::value_type>);

// Helpers to create readable dump string

template<container_pairs Cont>
void Fmt(wstring& out, Cont& cont) {
    if (cont.empty()) {
        out += L"{};";
        return;
    }

    out.reserve(out.size() + cont.size() * 256); // heuristic reserve to reduce reallocations
    out += L"{ ";

    for (const auto& kv : cont) {
        Dump(out, kv.first);
        out += L" -> ";
        Dump(out, kv.second);
        out += L"; ";
    }

    out.erase(out.length() - 2);
    out += L" };";
}

template<container_linear Cont>
void Fmt(wstring& out, Cont& cont) {
    if (cont.empty()) {
        out += L"[];";
        return;
    }

    out.reserve(out.size() + cont.size() * 256); // heuristic reserve to reduce reallocations
    out += L"[ ";

    for (const auto& val : cont) {
        Dump(out, val);
        out += L", ";
    }
    out.erase(out.length() - 2);
    out += L" ];";
}

template<typename T>
void Fmt(wstring& out, const T& obj) {
    Dump(out, obj);
    out += L";";
}

bool g_verbose = false;

} // dbg

#define WIDE(x) L##x

/**
* @brief Outputs variable name and it's value.
*/
#define Log(obj)                                          \
    do {                                                  \
        if (dbg::g_verbose) {                             \
            std::wstring _out(WIDE(#obj) L" = ");         \
            dbg::Fmt(_out, (obj));                        \
            Wh_Log(L"%s", _out.c_str());                  \
        }                                                 \
    } while (0)

// == Helpers ==

template <typename T>
struct Deferrer {
	T f;
	Deferrer(T f) : f(f) { };
	Deferrer(const Deferrer&) = delete;
	~Deferrer() { f(); }
};

#define defer Deferrer _ =

// == Data ==

class Process {
  public:
    /**
     * @brief Queries name of the process by window handle
     * and stores it in the cache to avoid multiple syscalls.
     */
    static wstring getName(HWND hWnd) {
        if (!hWnd) {
            return {};
        }

        DWORD ownerPid = 0;
        const DWORD ownerTid = GetWindowThreadProcessId(hWnd, &ownerPid);
        {
            Lock lock(s_mutex);
            auto it = s_windows.find(hWnd);

            if (it != s_windows.end()
            && it->second.processId == ownerPid
            && it->second.threadId  == ownerTid) {
                return it->second.name;
            }

            if (it != s_windows.end())
                s_windows.erase(it);
        }

        if (!ownerPid) {
            return {};
        }

        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ownerPid);
        if (!hProc) {
            return {};
        }

        wstring procName{};
        WCHAR exePath[MAX_PATH] = {0};
        DWORD exePathLen = MAX_PATH;

        if (QueryFullProcessImageNameW(hProc, 0, exePath, &exePathLen)) {
            WCHAR* name = wcsrchr(exePath, L'\\');
            if (name) {
                procName = (name + 1);
                std::transform(
                    procName.begin(),
                    procName.end(),
                    procName.begin(),
                    std::towlower
                );
            }
        }

        CloseHandle(hProc);

        if (!procName.empty()) {
            Lock lock(s_mutex);
            s_windows[hWnd] = { ownerPid, ownerTid, procName };
            // return pointer stored in the map to ensure stable lifetime
            return s_windows[hWnd].name;
        }

        return {};
    }

    static void clear() {
        Lock lock(s_mutex);
        s_windows.clear();
    }

    Process() = delete;
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
    ~Process() = delete;

  private:
    struct Cache {
        DWORD processId = 0;
        DWORD threadId  = 0;
        wstring name;
    };

    // inline = declaration also a definition
    // https://stackoverflow.com/a/46874207
    inline static std::mutex s_mutex;
    inline static unordered_map<HWND, Cache> s_windows;

};

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
        Lock lock(s_mutex);

        Log(s_windows);
        for (auto& win : s_windows) {
            if (IsWindow(win.first)) {
                RedrawWindow(
                    win.first, NULL, NULL,
                    RDW_FRAME | RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN
                );
            }
        }
        s_windows.clear();
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
 * @brief RAII wrapper for settings and brushes.
 * Stores user settings in the single instance (fully static).
 */
class Cfg {
  public:
    /**
    * @brief Available user settings.
    */
    struct Values {
        HBRUSH brush;
        bool aggressivePaint;
        bool longerPaint;
    };

    /**
    * @brief Loads all settings. Returns true on success.
    */
    static bool load() {
        dbg::g_verbose = Wh_GetIntSetting(L"verbose");

        unload();  // safe cleanup
        Lock lock(s_mutex);

        s_global.aggressivePaint = Wh_GetIntSetting(L"Global.aggressivePaint");
        s_global.longerPaint     = Wh_GetIntSetting(L"Global.longerPaint");
        {
            UINT   backColor = parseColor(L"Global.backgroundColor");
            HBRUSH brush     = tryCreateBrush(backColor);

            if (!brush)
                return false;

            s_global.brush = brush;
        }

        for (int i = 0;; ++i) {
            auto name = Cfg::getProcessName(L"Process[%d].name", i);
            if (name.empty())
                break;
            if (name == kInvalidProcessName)
                continue;

            s_processes[name].aggressivePaint =
                Wh_GetIntSetting(L"Process[%d].aggressivePaint", i);
            s_processes[name].longerPaint =
                Wh_GetIntSetting(L"Process[%d].longerPaint", i);

            UINT   backColor = parseColor(L"Process[%d].backgroundColor", i);
            HBRUSH brush     = tryCreateBrush(backColor);

            if (brush)
                s_processes[name].brush = brush;
        }

        Log(s_global);
        Log(s_processes);

        return true;
    }

    /**
    * @brief Frees brushes and clears maps.
    */
    static void unload() {
        Lock lock(s_mutex);

        if (s_global.brush) {
            DeleteObject(s_global.brush);
        }

        for (auto &kv : s_processes) {
            if (kv.second.brush) {
                DeleteObject(kv.second.brush);
            }
        }

        s_processes.clear();
    }

    static Values get() { return s_global; }

    /**
    * @brief Returns values for specific process
    * or default (global) values.
    */
    static Values get(HWND hWnd) {
        wstring name = Process::getName(hWnd);
        // Wh_Log(L"\"%s\" (%x)", name.c_str(), hWnd);

        if (!name.empty()) {
            auto it = s_processes.find(name);
            if (it != s_processes.end())
                return it->second;
        }

        return s_global;
    }

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
        if (*value == L'\0')
            return {};

        defer [&] { Wh_FreeStringSetting(value); };  // empty value cannot be freed

        wstring name{ value };
        name.erase(0, name.find_first_not_of(L" \t\v\n"));  // left trim
        name.erase(name.find_last_not_of(L" \t\v\n") + 1);  // right trim

        if (name.empty())
            return kInvalidProcessName;

        // Process name should be in lower case
        std::transform(name.begin(), name.end(), name.begin(), std::towlower);
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
    * @brief Creates solid brush with fall back to `kDefaultColor`
    */
    static HBRUSH tryCreateBrush(UINT rgb) {
        HBRUSH brush = CreateSolidBrush(toColor(rgb));
        if (!brush) {
            Wh_Log(L"Failed to create 0x%06x brush!", rgb);
            brush = CreateSolidBrush(toColor(kDefaultColor));
        }
        if (!brush) {
            Wh_Log(L"Failed to create default brush!");
            return NULL;
        }
        return brush;
    }

    // inline = declaration also a definition
    inline static std::mutex s_mutex;
    inline static Values s_global{};
    inline static unordered_map<wstring, Values> s_processes{};
    static constexpr UINT kDefaultColor = 0x191919;
    static constexpr UINT kInvalidColor = UINT_MAX;
    static constexpr PCWSTR kInvalidProcessName{ L"<ipn>" };
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

        auto cfg = Cfg::get(hWnd);
        if (!cfg.longerPaint && ShouldSkip(hWnd))
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

        FillRect(hdc, &rect, cfg.brush);
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

        auto cfg = Cfg::get(hWnd);
        if (ShouldSkip(hWnd))   // prevent any visual issues
            break;

        RECT rect{};
        if (!GetClientRect(hWnd, &rect))
            break;

        HDC hdc = (HDC)wParam;
        if (!hdc) {
            break;
        }

        FillRect(hdc, &rect, cfg.brush);
        return TRUE;  // don't let the original erase it again
    }
    case WM_PAINT: {
        // This message appears very frequently.
        // Set "painted" state temporary to reduce painting
        auto cfg = Cfg::get(hWnd);
        if (cfg.aggressivePaint)
            break;

        PAINTSTRUCT paint{};
        HDC hdc = BeginPaint(hWnd, &paint);
        if (!hdc) {
            break;
        }

        Win::setPainted(hWnd);
        LRESULT result = original(hWnd, Msg, wParam, lParam);

        Win::unsetPainted(hWnd);
        EndPaint(hWnd, &paint);

        return result;
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
    Process::clear();
}
