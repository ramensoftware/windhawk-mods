// ==WindhawkMod==
// @id              taskbar-user-tile
// @name            Taskbar User Tile
// @description     Recreation of Early Windows 8 style User tile flyout 
// @version         1.0.0
// @author          AdmXP8
// @github          https://github.com/AdmXP8
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32 -lshell32 -lwtsapi32 -ladvapi32 -lsecur32 -lnetapi32 -lgdiplus -lcomctl32 -luxtheme -lole32 -lruntimeobject -luuid -lwindowsapp -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- taskbarMode: auto
  $name: Taskbar type
  $description: Select the actual taskbar implementation. if you are using windows 11 with the explorer patcher taskbar or if you are using the Win10 taskbar on Win11 24H2 mod(or similar mods) you need to select this option yourself depending on the type of taskbar you have
  $options:
  - auto: Detect automatically
  - win10: Original Windows 10 classic taskbar
  - explorerpatcher: ExplorerPatcher classic taskbar 
  - win11: Windows 11 native XAML taskbar 
  - off: No taskbar button (only appear with hotkey)
- tileWidth: 44
  $name: Account button width (96-DPI pixels)
  $description: Range 28 to 80. Height follows the taskbar. The square picture is centered inside the button
- pictureSize: 36
  $name: Account picture size (96-DPI pixels)
  $description: Range 20 to 64. Automatically limited to the available button size
- enableHotkey: true
  $name: Enable keyboard shortcut
  $description: Toggle the flyout with Ctrl+Alt+U. Turn this off to open the flyout only from the taskbar button (and to free the key combination for other apps)
- language: auto
  $name: Language
  $description: Language of the flyout text
  $options:
  - auto: Detect automatically
  - en: English
  - it: Italiano
  - es: Español
  - de: Deutsch
  - fr: Français
  - pt-BR: Português (Brasil)
  - ru: Русский
  - zh-CN: 简体中文
  - ja: 日本語
  - pl: Polski
  - tr: Türkçe
  - fa: فارسی
- theme: auto
  $name: Flyout theme
  $description: Color theme of the flyout. Auto follows the Windows taskbar theme
  $options:
  - auto: Follow Windows (Auto)
  - dark: Dark
  - light: Light
- borderColor: auto
  $name: Flyout border
  $description: Native uses the real window border drawn by Windows (accent color, shadow, rounded corners on Windows 11) like the system flyouts. The other options draw the border inside the flyout. Follow Windows uses the accent color only when "Show accent color on Start and taskbar" is turned on in Windows settings
  $options:
  - native: Native Windows border
  - auto: Follow Windows (drawn border)
  - accent: Always use the accent color (drawn border)
  - neutral: Always neutral gray (drawn border)
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Taskbar User Tile
This mod recreates early windows 8 leaked builds usertile on the taskbar of windows 10 and 11

## Screenshots:

### Widows 11 taskbar
![win11tb](https://raw.githubusercontent.com/AdmXP8/assets/main/Tbuserwin11.png)

### Windows 10 taskbar (Explorer patcher)
![win10EPtb](https://raw.githubusercontent.com/AdmXP8/assets/main/TbuserEP.png)

### windows 10 taskbar
![win10tb](https://raw.githubusercontent.com/AdmXP8/assets/main/Tbuserwin10.png)


## Note
Native XAML taskbar integration contains portions adapted from Taskbar Fluent
Media Player mod https://windhawk.net/mods/taskbar-fluent-media-player

## Limitation:
Usertile button does not appers when taskbar sized verticaly!
*/
// ==/WindhawkModReadme==

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define SECURITY_WIN32
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <wtsapi32.h>
#include <security.h>
#include <lm.h>
#include <sddl.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <commctrl.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <vsstyle.h>
#include <cstddef>
#include <cwctype>
using std::nullptr_t;
#include <windhawk_utils.h>
#include <cmath>
#include <coroutine>
#undef GetCurrentTime
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <robuffer.h>

static std::recursive_mutex g_accountMutex;
static std::atomic<bool> g_stopping{false};
static bool g_win10Mode = true, g_useExplorerPatcher = false, g_nativeWin11Mode = false;
static std::atomic<bool> g_symbolsReady{false};
static std::atomic<bool> g_hooksApplied{false};
static int g_tileWidth = 44, g_pictureSize = 36;
static bool g_hotkeyEnabled = true;
static std::atomic<HWND> g_clock{nullptr}, g_tile{nullptr}, g_taskbar{nullptr}, g_tray{nullptr};
static std::atomic<int> g_nativeClockWidth{0}, g_reservedSlot{0};
static bool g_slotAllocated = false, g_tileHover = false, g_tilePressed = false;
static bool g_tileFlyoutVisibleOnPress = false;
static int g_lastActionCenterState = -1;
static const wchar_t* kTileClass = L"WhUserTileTaskbarV04";
static UINT g_setupMsg = 0, g_cleanupMsg = 0;
static constexpr UINT_PTR kDiscoveryTimer = 300;
static void DiscoverTaskbar();
static void DetachTaskbar();
static void RelayoutTaskbar();
static void DiscoverWin11Taskbar();
static void RemoveWin11Tile();
static void RefreshWin11TileImage();
static LRESULT CALLBACK TileProc(HWND, UINT, WPARAM, LPARAM);

static HWND g_flyout = nullptr;
static HANDLE g_thread = nullptr, g_ready = nullptr;
static HFONT g_font = nullptr, g_fontUnderline = nullptr;
static ULONG_PTR g_gdip = 0;
static std::unique_ptr<Gdiplus::Bitmap> g_picture;
static std::wstring g_displayName;
static UINT g_dpi = 96;
static bool g_initialized = false;
static int g_hover = -1, g_pressed = -1;
static RECT g_flyoutAnchorScreen = {};
static bool g_flyoutAnchorValid = false;

#define HOTKEY_ID 1
#define HOTKEY_MODS (MOD_CONTROL | MOD_ALT | MOD_NOREPEAT)
#define HOTKEY_VK 'U'
#define WM_SHOWFLYOUT (WM_APP + 1)
#define WM_SHOWFLYOUT_AT (WM_APP + 2)
#ifndef WM_DWMCOLORIZATIONCOLORCHANGED
#define WM_DWMCOLORIZATIONCOLORCHANGED 0x0320
#endif

enum Action { ACT_NONE = -1, ACT_SETTINGS, ACT_LOOK, ACT_SWITCH,
              ACT_LOGOFF, ACT_LOCK, ACT_COUNT };
struct Item { const wchar_t* text; RECT rc; };
static constexpr int W = 250, H = 216;
static const RECT kPicture = {133, 23, 231, 121};
static const Item g_items[ACT_COUNT] = {
    {L"My Settings", {22, 43, 128, 69}},
    {L"My Look",     {22, 72, 128, 98}},
    {L"Switch user", {0, 130, W, 158}},
    {L"Log off",     {0, 158, W, 187}},
    {L"Lock PC",     {0, 187, W, H}},
};
enum Lang {
    LANG_EN, LANG_IT, LANG_ES,
    LANG_DE, LANG_FR, LANG_PT_BR, LANG_RU,
    LANG_ZH_CN, LANG_JA, LANG_PL, LANG_TR, LANG_FA,
    LANG_COUNT
};
static Lang g_lang = LANG_EN;

// order: My Settings, My Look, Switch user, Log off, Lock PC
static const wchar_t* kTexts[LANG_COUNT][ACT_COUNT] = {
    { L"My Settings", L"My Look", L"Switch user", L"Log off", L"Lock PC" },
    { L"Impostazioni", L"Aspetto", L"Cambia utente", L"Disconnetti", L"Blocca PC" },
    { L"Mi configuración", L"Mi apariencia", L"Cambiar de usuario", L"Cerrar sesión", L"Bloquear PC" },
    { L"Einstellungen", L"Aussehen", L"Benutzer wechseln", L"Abmelden", L"PC sperren" },
    { L"Mes paramètres", L"Mon apparence", L"Changer d’utilisateur", L"Fermer la session", L"Verrouiller le PC" },
    { L"Configurações", L"Aparência", L"Trocar usuário", L"Sair", L"Bloquear PC" },
    { L"Параметры", L"Вид", L"Сменить пользователя", L"Выйти", L"Заблокировать ПК" },
    { L"我的设置", L"我的外观", L"切换用户", L"注销", L"锁定电脑" },
    { L"マイ設定", L"マイ外観", L"ユーザーの切り替え", L"サインアウト", L"PC をロック" },
    { L"Ustawienia", L"Wygląd", L"Przełącz użytkownika", L"Wyloguj", L"Zablokuj komputer" },
    { L"Ayarlar", L"Görünüm", L"Kullanıcı değiştir", L"Oturumu kapat", L"Bilgisayarı kilitle" },
    { L"تنظیمات من", L"ظاهر من", L"تعویض کاربر", L"خروج از سیستم", L"قفل کردن رایانه" },
};

static const wchar_t* kUserFallback[LANG_COUNT] = {
    L"User", L"Utente", L"Usuario", L"Benutzer", L"Utilisateur", L"Usuário",
    L"Пользователь", L"用户", L"ユーザー", L"Użytkownik", L"Kullanıcı", L"کاربر"
};

static const wchar_t* kLangCodes[LANG_COUNT] = {
    L"en", L"it", L"es", L"de", L"fr", L"pt-BR",
    L"ru", L"zh-CN", L"ja", L"pl", L"tr", L"fa"
};

static int DetectSystemLanguage() {
    //get default UI language
    LANGID langid = GetUserDefaultUILanguage();
    WORD primary = PRIMARYLANGID(langid);

    switch (primary) {
        case LANG_ENGLISH:    return LANG_EN;
        case LANG_ITALIAN:    return LANG_IT;
        case LANG_SPANISH:    return LANG_ES;
        case LANG_GERMAN:     return LANG_DE;
        case LANG_FRENCH:     return LANG_FR;
        case LANG_PORTUGUESE: return LANG_PT_BR; 
        case LANG_RUSSIAN:    return LANG_RU;
        case LANG_CHINESE:    return LANG_ZH_CN; 
        case LANG_JAPANESE:   return LANG_JA;
        case LANG_POLISH:     return LANG_PL;
        case LANG_TURKISH:    return LANG_TR;
        case LANG_FARSI:      return LANG_FA;
        default:              return LANG_EN;
    }
}

static void LoadLanguageSetting() {
    PCWSTR value = Wh_GetStringSetting(L"language");
    if (value) {
        if (wcscmp(value, L"auto") == 0) {
            g_lang = (Lang)DetectSystemLanguage();
        } else {
            g_lang = LANG_EN;
            for (int i = 0; i < LANG_COUNT; i++)
                if (wcscmp(value, kLangCodes[i]) == 0) { g_lang = (Lang)i; break; }
        }
        Wh_FreeStringSetting(value);
    }
}
static bool StartsWithRtl(const std::wstring& text) {
    for (wchar_t c : text) {
        if (c >= 0x0590 && c <= 0x08FF) return true;
        if (c >= 0xFB1D && c <= 0xFDFF) return true;
        if (c >= 0xFE70 && c <= 0xFEFF) return true;
        if (iswalpha(c)) return false;
    }
    return false;
}
static int S(int value) { return MulDiv(value, g_dpi, 96); }
static RECT Scaled(RECT r) { return {S(r.left), S(r.top), S(r.right), S(r.bottom)}; }
static void Fill(HDC dc, RECT r, COLORREF c) {
    HBRUSH b = CreateSolidBrush(c); FillRect(dc, &r, b); DeleteObject(b);
}

static std::wstring AccountDisplayName() {
    ULONG count = 0;
    GetUserNameExW(NameDisplay, nullptr, &count);
    if (count) {
        std::vector<wchar_t> name(count + 1);
        if (GetUserNameExW(NameDisplay, name.data(), &count) && name[0])
            return name.data();
    }
    wchar_t login[UNLEN + 1] = {};
    DWORD size = ARRAYSIZE(login);
    if (GetUserNameW(login, &size)) {
        LPBYTE data = nullptr;
        if (NetUserGetInfo(nullptr, login, 2, &data) == NERR_Success) {
            auto info = reinterpret_cast<USER_INFO_2*>(data);
            std::wstring full = info->usri2_full_name ? info->usri2_full_name : L"";
            NetApiBufferFree(data);
            if (!full.empty()) return full;
        }
        return login;
    }
    return kUserFallback[g_lang];
}
static std::wstring CurrentSid() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return {};
    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    std::vector<BYTE> data(size);
    std::wstring result;
    if (size && GetTokenInformation(token, TokenUser, data.data(), size, &size)) {
        LPWSTR sid = nullptr;
        if (ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(data.data())->User.Sid, &sid)) {
            result = sid; LocalFree(sid);
        }
    }
    CloseHandle(token);
    return result;
}
static std::wstring Expand(const std::wstring& path) {
    DWORD n = ExpandEnvironmentStringsW(path.c_str(), nullptr, 0);
    if (!n) return path;
    std::vector<wchar_t> out(n);
    if (!ExpandEnvironmentStringsW(path.c_str(), out.data(), n)) return path;
    return out.data();
}
static bool LoadPictureFile(const std::wstring& rawPath) {
    if (!g_gdip || rawPath.empty()) return false;
    std::wstring path = Expand(rawPath);
    if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) return false;
    //copy the image so we dont depend on the source bitmap
    Gdiplus::Bitmap source(path.c_str());
    if (source.GetLastStatus() != Gdiplus::Ok || !source.GetWidth() || !source.GetHeight())
        return false;
    std::unique_ptr<Gdiplus::Bitmap> copy(source.Clone(0, 0,
        static_cast<INT>(source.GetWidth()), static_cast<INT>(source.GetHeight()),
        PixelFormat32bppPARGB));
    if (!copy || copy->GetLastStatus() != Gdiplus::Ok) return false;
    g_picture = std::move(copy);
    Wh_Log(L"User Tile: account picture loaded from %s", path.c_str());
    return true;
}
static bool LoadRegistryPicture(HKEY root, const std::wstring& key, REGSAM view) {
    HKEY h = nullptr;
    if (RegOpenKeyExW(root, key.c_str(), 0, KEY_QUERY_VALUE | view, &h) != ERROR_SUCCESS)
        return false;
    //sized as original one
    int wanted = S(98);
    const int sizes[] = {32, 40, 48, 64, 96, 192, 208, 240, 424, 448, 1080};
    std::vector<int> order(std::begin(sizes), std::end(sizes));
    std::stable_sort(order.begin(), order.end(), [wanted](int a, int b) {
        bool aEnough = a >= wanted, bEnough = b >= wanted;
        if (aEnough != bEnough) return aEnough;
        return aEnough ? a < b : a > b;
    });
    bool found = false;
    for (int pixels : order) {
        std::wstring value = L"Image" + std::to_wstring(pixels);
        DWORD type = 0, bytes = 0;
        if (RegQueryValueExW(h, value.c_str(), nullptr, &type, nullptr, &bytes) != ERROR_SUCCESS ||
            (type != REG_SZ && type != REG_EXPAND_SZ) || bytes > 1024 * 1024) continue;
        std::vector<wchar_t> path(bytes / sizeof(wchar_t) + 2, 0);
        if (RegQueryValueExW(h, value.c_str(), nullptr, &type,
            reinterpret_cast<BYTE*>(path.data()), &bytes) == ERROR_SUCCESS &&
            LoadPictureFile(path.data())) { found = true; break; }
    }
    RegCloseKey(h);
    return found;
}
static void RefreshAccount() {
    std::lock_guard<std::recursive_mutex> lock(g_accountMutex);
    g_displayName = AccountDisplayName();
    g_picture.reset();
    std::wstring sid = CurrentSid();
    if (!sid.empty()) {
        std::wstring key = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AccountPicture\\Users\\" + sid;
        if (LoadRegistryPicture(HKEY_LOCAL_MACHINE, key, KEY_WOW64_64KEY) ||
            LoadRegistryPicture(HKEY_CURRENT_USER, key, KEY_WOW64_64KEY) ||
            LoadRegistryPicture(HKEY_LOCAL_MACHINE, key, 0)) return;
    }
    //older picture location
    wchar_t login[UNLEN + 1] = {};
    DWORD length = ARRAYSIZE(login);
    if (GetUserNameW(login, &length) && LoadPictureFile(
        L"%PUBLIC%\\AccountPictures\\" + std::wstring(login) + L".bmp")) return;
    if (GetUserNameW(login, &length) && LoadPictureFile(
        L"%ProgramData%\\Microsoft\\User Account Pictures\\" + std::wstring(login) + L".bmp")) return;
    //fall back to the default pfp
    const wchar_t* defaults[] = {
        L"%ProgramData%\\Microsoft\\User Account Pictures\\user.png",
        L"%ProgramData%\\Microsoft\\User Account Pictures\\user.bmp"
    };
    for (auto path : defaults) if (LoadPictureFile(path)) return;
    Wh_Log(L"User Tile: no readable account picture; using a clipped fallback icon.");
}

static void SetDpi(UINT dpi) {
    g_dpi = dpi ? dpi : 96;
    if (g_font) DeleteObject(g_font);
    g_font = CreateFontW(-S(12), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    //same font with underline, used for the hovered "My Settings"/"My Look" links
    if (g_fontUnderline) DeleteObject(g_fontUnderline);
    g_fontUnderline = CreateFontW(-S(12), 0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
}
static UINT MonitorDpi(HMONITOR monitor) {
    UINT x = 96, y = 96;
    HMODULE shcore = LoadLibraryW(L"shcore.dll");
    if (shcore) {
        using Fn = HRESULT (WINAPI*)(HMONITOR, int, UINT*, UINT*);
        auto get = reinterpret_cast<Fn>(GetProcAddress(shcore, "GetDpiForMonitor"));
        if (!get || FAILED(get(monitor, 0, &x, &y))) x = 96;
        FreeLibrary(shcore);
    } else {
        HDC dc = GetDC(nullptr);
        if (dc) { x = GetDeviceCaps(dc, LOGPIXELSX); ReleaseDC(nullptr, dc); }
    }
    return x;
}
static void DoAction(int action) {
    switch (action) {
    case ACT_SETTINGS: ShellExecuteW(nullptr, L"open", L"ms-settings:yourinfo", nullptr, nullptr, SW_SHOWNORMAL); break;
    case ACT_LOOK: ShellExecuteW(nullptr, L"open", L"ms-settings:personalization", nullptr, nullptr, SW_SHOWNORMAL); break;
    case ACT_SWITCH: WTSDisconnectSession(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, FALSE); break;
    case ACT_LOGOFF: ExitWindowsEx(EWX_LOGOFF, 0); break;
    case ACT_LOCK: LockWorkStation(); break;
    }
}
static int HitTest(int x, int y) {
    POINT p = {x, y};
    for (int i = 0; i < ACT_COUNT; ++i) {
        RECT r = Scaled(g_items[i].rc);
        if (PtInRect(&r, p)) return i;
    }
    return ACT_NONE;
}
struct Theme {
    COLORREF bg, titleBar, titleText, pictureBg, separator,
             hoverBg, link, linkHover;
};
static const Theme kDark = {
    RGB(32, 32, 32), RGB(160, 160, 160), RGB(245, 245, 245), RGB(247, 247, 247),
    RGB(60, 60, 60), RGB(50, 50, 50), RGB(110, 205, 235), RGB(255, 255, 255)
};
static const Theme kLight = {
    RGB(255, 255, 255), RGB(130, 130, 130), RGB(0, 0, 0), RGB(247, 247, 247),
    RGB(200, 200, 200), RGB(229, 241, 251), RGB(30, 100, 200), RGB(0, 60, 140)
};
static int g_themeSetting = 0;      // 0 = auto 1 = dark 2 = light
static bool g_isLight = false;

static bool WindowsUsesLightTheme() {
    DWORD value = 0, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) != ERROR_SUCCESS)
        return true;
    return value != 0;
}
static void UpdateTheme() {
    g_isLight = g_themeSetting == 2 || (g_themeSetting == 0 && WindowsUsesLightTheme());
}
static const Theme& T() { return g_isLight ? kLight : kDark; }
static void LoadThemeSetting() {
    PCWSTR v = Wh_GetStringSetting(L"theme");
    g_themeSetting = 0;
    if (v) {
        if (wcscmp(v, L"dark") == 0) g_themeSetting = 1;
        else if (wcscmp(v, L"light") == 0) g_themeSetting = 2;
        Wh_FreeStringSetting(v);
    }
    UpdateTheme();
}
static int g_borderSetting = 0;
static bool g_accentBorder = false;
static COLORREF g_accentColor = 0;

static bool ReadUserDword(const wchar_t* key, const wchar_t* name, DWORD* out) {
    DWORD size = sizeof(*out);
    return RegGetValueW(HKEY_CURRENT_USER, key, name, RRF_RT_REG_DWORD,
                        nullptr, out, &size) == ERROR_SUCCESS;
}
static void UpdateBorder() {
    DWORD accent = 0;
    bool haveAccent =
        ReadUserDword(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
                      L"AccentColorMenu", &accent) ||
        ReadUserDword(L"Software\\Microsoft\\Windows\\DWM", L"AccentColor", &accent);
    DWORD prevalence = 0;
    ReadUserDword(L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                  L"ColorPrevalence", &prevalence);
    g_accentColor = accent & 0x00FFFFFF; 
    g_accentBorder = haveAccent && (g_borderSetting == 1 || (g_borderSetting == 0 && prevalence != 0));
}
static void LoadBorderSetting() {
    PCWSTR v = Wh_GetStringSetting(L"borderColor");
    g_borderSetting = 0;
    if (v) {
        if (wcscmp(v, L"auto") == 0) g_borderSetting = 0;
        else if (wcscmp(v, L"accent") == 0) g_borderSetting = 1;
        else if (wcscmp(v, L"neutral") == 0) g_borderSetting = 2;
        else if (wcscmp(v, L"native") == 0) g_borderSetting = 3;
        Wh_FreeStringSetting(v);
    }
    UpdateBorder();
}
static bool g_nativeFrame = false;
static constexpr int kNativeOffsetX = 6; 
static constexpr int kNativeOffsetY = 6; 
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
static DWORD FlyoutStyle() { return WS_POPUP | (g_nativeFrame ? (WS_BORDER | WS_THICKFRAME) : 0); }
static DWORD FlyoutExStyle() { return WS_EX_TOOLWINDOW | WS_EX_TOPMOST; }
static void OuterSize(int* width, int* height) {
    RECT r = {0, 0, S(W), S(H)};
    if (g_nativeFrame) {
        using Fn = BOOL (WINAPI*)(LPRECT, DWORD, BOOL, DWORD, UINT);
        auto fn = reinterpret_cast<Fn>(GetProcAddress(GetModuleHandleW(L"user32.dll"),
                                                     "AdjustWindowRectExForDpi"));
        if (fn) fn(&r, FlyoutStyle(), FALSE, FlyoutExStyle(), g_dpi);
        else AdjustWindowRectEx(&r, FlyoutStyle(), FALSE, FlyoutExStyle());
    }
    *width = r.right - r.left; *height = r.bottom - r.top;
}
static void UpdateNativeFrameTheme(HWND hwnd) {
    if (!g_nativeFrame || !hwnd) return;
    BOOL dark = !g_isLight;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}
static void ApplyNativeFrame(HWND hwnd) {
    if (!g_nativeFrame || !hwnd) return;
    DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));
    int corner = 2; 
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
    MARGINS margins = {0, 0, 0, 1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    UpdateNativeFrameTheme(hwnd);
}
static void SnapToVisible(HWND hwnd, int wantedRight, int wantedEdgeY, bool useBottom) {
    RECT visible = {}, window = {};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &visible, sizeof(visible))) ||
        !GetWindowRect(hwnd, &window)) return;
    int dx = wantedRight - visible.right;
    int dy = (useBottom ? wantedEdgeY - visible.bottom : wantedEdgeY - visible.top);
    if (dx || dy)
        SetWindowPos(hwnd, nullptr, window.left + dx, window.top + dy, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
static bool DrawThemeHover(HWND hwnd, HDC dc, const RECT& r) {
    HTHEME theme = OpenThemeData(hwnd, g_isLight ? L"Explorer::ListView" : L"DarkMode_Explorer::ListView");
    if (!theme) return false;
    HRESULT hr = DrawThemeBackground(theme, dc, LVP_LISTITEM, LISS_HOT, &r, nullptr);
    CloseThemeData(theme);
    return SUCCEEDED(hr);
}
static void Paint(HWND hwnd) {
    std::lock_guard<std::recursive_mutex> lock(g_accountMutex);
    PAINTSTRUCT ps;
    HDC target = BeginPaint(hwnd, &ps);
    RECT client; GetClientRect(hwnd, &client);
    HDC dc = CreateCompatibleDC(target);
    HBITMAP buffer = CreateCompatibleBitmap(target, client.right, client.bottom);
    HGDIOBJ oldBitmap = SelectObject(dc, buffer);
    const Theme& theme = T();  
    Fill(dc, client, theme.bg);
    Fill(dc, Scaled({0, 0, W, 16}), theme.titleBar);
    HGDIOBJ oldFont = SelectObject(dc, g_font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, theme.titleText);
    RECT title = Scaled({2, 0, W - 3, 16});
    UINT titleFlags = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX;
    if (StartsWithRtl(g_displayName)) titleFlags |= DT_RTLREADING;
    DrawTextW(dc, g_displayName.c_str(), -1, &title, titleFlags);
    RECT picture = Scaled(kPicture);
    Fill(dc, picture, theme.pictureBg);
    if (g_gdip) {
        Gdiplus::Graphics graphics(dc);
        graphics.SetClip(Gdiplus::Rect(picture.left, picture.top,
            picture.right - picture.left, picture.bottom - picture.top));
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        if (g_picture) {
            int width = static_cast<int>(g_picture->GetWidth());
            int height = static_cast<int>(g_picture->GetHeight());
            int side = (std::min)(width, height);
            graphics.DrawImage(g_picture.get(), Gdiplus::Rect(picture.left, picture.top,
                picture.right - picture.left, picture.bottom - picture.top),
                (width - side) / 2, (height - side) / 2, side, side, Gdiplus::UnitPixel);
        } else {
            //no pfp found
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            Gdiplus::SolidBrush ink(Gdiplus::Color(255, 145, 145, 145));
            graphics.FillEllipse(&ink, S(167), S(43), S(30), S(30));
            Gdiplus::GraphicsPath body;
            body.AddArc(S(159), S(77), S(46), S(18), 180, 180);
            body.AddArc(S(159), S(77), S(46), S(34), 0, 180);
            body.CloseFigure(); graphics.FillPath(&ink, &body);
        }
    }
    Fill(dc, {0, S(129), client.right, S(129) + 1}, theme.separator);
    for (int i = 0; i < ACT_COUNT; ++i) {
        RECT r = Scaled(g_items[i].rc);
        if (i == g_hover && i >= ACT_SWITCH && !DrawThemeHover(hwnd, dc, r)) Fill(dc, r, theme.hoverBg);
        SetTextColor(dc, i == g_hover ? theme.linkHover : theme.link);
        if (i >= ACT_SWITCH) r.left += S(22);
        UINT flags = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS;
        if (g_lang == LANG_FA) flags |= DT_RTLREADING;
        //the two links at the top get underlined while hovered
        SelectObject(dc, (i == g_hover && i < ACT_SWITCH && g_fontUnderline) ? g_fontUnderline : g_font);
        DrawTextW(dc, kTexts[g_lang][i], -1, &r, flags);
    }
    if (!g_nativeFrame) { //in native mode DWM draws the border
        HBRUSH border = CreateSolidBrush(g_accentBorder ? g_accentColor : theme.separator);
        FrameRect(dc, &client, border);
        DeleteObject(border);
    }
    SelectObject(dc, oldFont);
    BitBlt(target, 0, 0, client.right, client.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, oldBitmap); DeleteObject(buffer); DeleteDC(dc);
    EndPaint(hwnd, &ps);
}
static void PositionAndShow(HWND hwnd, HWND anchor = nullptr, const RECT* screenAnchor = nullptr) {
    POINT pt = {}; GetCursorPos(&pt);
    RECT anchorRect = {};
    g_flyoutAnchorValid = false;
    if (screenAnchor) {
        anchorRect = *screenAnchor;
        g_flyoutAnchorScreen = anchorRect;
        g_flyoutAnchorValid = true; //exact bounds of the Win11 tile
        pt = {anchorRect.left, anchorRect.top};
        anchor = reinterpret_cast<HWND>(static_cast<INT_PTR>(1));
    } else if (anchor && GetWindowRect(anchor, &anchorRect)) {
        if (anchor == g_tile.load()) {
            g_flyoutAnchorScreen = anchorRect;
            g_flyoutAnchorValid = true;
        }
        pt = {anchorRect.left, anchorRect.top};
    } else anchor = nullptr;
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) return;
    SetDpi(MonitorDpi(monitor));
    RefreshAccount();
    RefreshWin11TileImage();
    if (IsWindow(g_tile.load())) InvalidateRect(g_tile.load(), nullptr, FALSE);
    g_hover = g_pressed = ACT_NONE;
    int outerW = S(W), outerH = S(H);
    if (g_nativeFrame) OuterSize(&outerW, &outerH);

    int x = (std::max)(mi.rcWork.left, mi.rcWork.right - outerW - S(8));
    int y = (std::max)(mi.rcWork.top, mi.rcWork.bottom - outerH - S(8));
    if (anchor) {
        x = anchorRect.right - outerW;
        if (anchorRect.top >= mi.rcWork.bottom)
            y = mi.rcWork.bottom - outerH - S(8);
        else
            y = mi.rcWork.top + S(8); //taskbar on top
        x = (std::max)(static_cast<int>(mi.rcWork.left),
            (std::min)(x, static_cast<int>(mi.rcWork.right) - outerW));
    }
    bool fromBottom = !anchor || anchorRect.top >= mi.rcWork.bottom;

    if (g_nativeFrame) {
        int frameOffsetX = S(kNativeOffsetX);
        int frameOffsetY = S(kNativeOffsetY);
        x -= frameOffsetX;
        y += fromBottom ? -frameOffsetY : frameOffsetY;
        x = (std::max)(static_cast<int>(mi.rcWork.left),
            (std::min)(x, static_cast<int>(mi.rcWork.right) - outerW));
        y = (std::max)(static_cast<int>(mi.rcWork.top),
            (std::min)(y, static_cast<int>(mi.rcWork.bottom) - outerH));
    }

    SetWindowPos(hwnd, HWND_TOPMOST, x, y, outerW, outerH, SWP_SHOWWINDOW);
    if (g_nativeFrame)
        SnapToVisible(hwnd, x + outerW, fromBottom ? y + outerH : y, fromBottom);
    SetForegroundWindow(hwnd);
    InvalidateRect(hwnd, nullptr, FALSE);
}
static bool CursorOverTileAnchor() {
    POINT point = {};
    if (!GetCursorPos(&point)) return false;
    //use the tile's current position
    HWND tile = g_tile.load();
    if (tile && IsWindow(tile)) {
        RECT rect = {};
        if (GetWindowRect(tile, &rect) && PtInRect(&rect, point)) return true;
    }
    return g_flyoutAnchorValid && PtInRect(&g_flyoutAnchorScreen, point);
}
static void Toggle(HWND hwnd, HWND anchor = nullptr, const RECT* screenAnchor = nullptr,
                   int visibleOnPress = -1) {
    bool wasVisible = visibleOnPress >= 0 ? visibleOnPress != 0 : IsWindowVisible(hwnd);
    if (wasVisible) ShowWindow(hwnd, SW_HIDE);
    else PositionAndShow(hwnd, anchor, screenAnchor);
}
static LRESULT CALLBACK FlyoutProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_HOTKEY: if (wp == HOTKEY_ID) Toggle(hwnd); return 0;
    case WM_SHOWFLYOUT:
        Toggle(hwnd, reinterpret_cast<HWND>(lp), nullptr, wp ? 1 : 0); return 0;
    case WM_SHOWFLYOUT_AT: {
        std::unique_ptr<RECT> anchor(reinterpret_cast<RECT*>(lp));
        if (anchor) Toggle(hwnd, nullptr, anchor.get(), wp ? 1 : 0);
        return 0;
    }
    case WM_TIMER:
        if (wp == kDiscoveryTimer && !g_stopping) DiscoverTaskbar();
        return 0;
    case WM_SETTINGCHANGE:
    if (lp && wcscmp(reinterpret_cast<LPCWSTR>(lp), L"ImmersiveColorSet") == 0) {
        UpdateTheme();
        UpdateBorder();
        UpdateNativeFrameTheme(hwnd);
        InvalidateRect(hwnd, nullptr, FALSE);
    }
    return 0;
    case WM_DWMCOLORIZATIONCOLORCHANGED:
        UpdateBorder();
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    case WM_SETCURSOR:
        if (LOWORD(lp) == HTCLIENT) {
            POINT p = {}; GetCursorPos(&p); ScreenToClient(hwnd, &p);
            int over = HitTest(p.x, p.y);
            if (over == ACT_SETTINGS || over == ACT_LOOK) {
                SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(32649))); //IDC_HAND
                return TRUE;
            }
        }
        break;
    case WM_NCHITTEST: {
        LRESULT hit = DefWindowProcW(hwnd, msg, wp, lp);
        if (g_nativeFrame && (hit == HTTOP || hit == HTTOPLEFT || hit == HTTOPRIGHT ||
            hit == HTBOTTOM || hit == HTBOTTOMLEFT || hit == HTBOTTOMRIGHT ||
            hit == HTLEFT || hit == HTRIGHT))
            return HTBORDER; //keep the thick frame from letting the flyout be resized
        return hit;
    }
    case WM_PAINT: Paint(hwnd); return 0;
    case WM_ERASEBKGND: return 1;
    case WM_DPICHANGED: {
        SetDpi(HIWORD(wp));
        RECT* r = reinterpret_cast<RECT*>(lp);
        int outerW = S(W), outerH = S(H);
        if (g_nativeFrame) OuterSize(&outerW, &outerH);
        SetWindowPos(hwnd, nullptr, r->left, r->top, outerW, outerH, SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(hwnd, nullptr, FALSE); return 0;
    }
    case WM_MOUSEMOVE: {
        int hover = HitTest(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        if (hover != g_hover) { g_hover = hover; InvalidateRect(hwnd, nullptr, FALSE); }
        TRACKMOUSEEVENT t = {sizeof(t), TME_LEAVE, hwnd, 0}; TrackMouseEvent(&t); return 0;
    }
    case WM_MOUSELEAVE: g_hover = ACT_NONE; InvalidateRect(hwnd, nullptr, FALSE); return 0;
    case WM_LBUTTONDOWN:
        g_pressed = HitTest(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        if (g_pressed != ACT_NONE) SetCapture(hwnd);
        return 0;
    case WM_LBUTTONUP: {
        int hit = HitTest(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
        int pressed = g_pressed; g_pressed = ACT_NONE;
        if (GetCapture() == hwnd) ReleaseCapture();
        if (hit != ACT_NONE && hit == pressed) { ShowWindow(hwnd, SW_HIDE); DoAction(hit); }
        return 0;
    }
    case WM_CAPTURECHANGED: g_pressed = ACT_NONE; return 0;
    case WM_ACTIVATE:
        if (LOWORD(wp) == WA_INACTIVE) {
            if (!CursorOverTileAnchor()) ShowWindow(hwnd, SW_HIDE);
        }
        return 0;
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) ShowWindow(hwnd, SW_HIDE);
        else if (wp == VK_DOWN || wp == VK_UP || wp == VK_TAB) {
            bool back = wp == VK_UP || (wp == VK_TAB && (GetKeyState(VK_SHIFT) & 0x8000));
            g_hover = g_hover == ACT_NONE ? (back ? ACT_COUNT - 1 : 0) :
                (g_hover + (back ? ACT_COUNT - 1 : 1)) % ACT_COUNT;
            InvalidateRect(hwnd, nullptr, FALSE);
        } else if ((wp == VK_RETURN || wp == VK_SPACE) && g_hover != ACT_NONE) {
            int action = g_hover; ShowWindow(hwnd, SW_HIDE); DoAction(action);
        }
        return 0;
    case WM_MOUSEACTIVATE: return MA_ACTIVATE;
    case WM_CLOSE: DestroyWindow(hwnd); return 0;
    case WM_DESTROY: UnregisterHotKey(hwnd, HOTKEY_ID); PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
//Win10 and EP
static std::mutex g_attachMutex;
static std::mutex g_epSymbolMutex;
static HMODULE g_epAttemptedModule = nullptr;
static bool g_epWaitingLogged = false;
using CalcMinimum = LPSIZE (WINAPI*)(void*, LPSIZE, SIZE);
using UpdateClockState = void (WINAPI*)(void*, bool);
static CalcMinimum g_calcMinimum = nullptr;
static UpdateClockState g_updateClockState = nullptr;

static UINT TileDpi(HWND hwnd) {
    using Fn = UINT (WINAPI*)(HWND);
    auto fn = reinterpret_cast<Fn>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    UINT dpi = fn ? fn(hwnd) : 0;
    return dpi ? dpi : MonitorDpi(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY));
}
static bool EligibleTaskbar(HWND taskbar) {
    RECT r = {};
    if (!IsWindow(taskbar) || !GetWindowRect(taskbar, &r)) return false;
    if (r.right - r.left <= r.bottom - r.top) return false; //no you don't have to shown in vertical taskbar
    HWND clock = g_clock.load();
    HWND parent = clock ? GetParent(clock) : nullptr;
    return !(GetWindowLongPtrW(taskbar, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) &&
           !(parent && (GetWindowLongPtrW(parent, GWL_EXSTYLE) & WS_EX_LAYOUTRTL));
}
static LPSIZE WINAPI CalcMinimumHook(void* object, LPSIZE result, SIZE constraint) {
    LPSIZE answer = g_calcMinimum(object, result, constraint);
    if (g_stopping || !answer || !g_win10Mode) return answer;
    HWND clock = g_clock.load();
    if (!clock || !EligibleTaskbar(g_taskbar.load())) return answer;
    if (reinterpret_cast<void*>(GetWindowLongPtrW(clock, 0)) != object) return answer;
    int slot = MulDiv(g_tileWidth, TileDpi(clock), 96);
    if (answer->cx > 0 && slot > 0) {
        if (g_nativeClockWidth.load() != answer->cx || g_reservedSlot.load() != slot)
            Wh_Log(L"User Tile: clock width %d, requested extra tile width %d.", answer->cx, slot);
        g_nativeClockWidth = answer->cx;
        g_reservedSlot = slot;
        answer->cx += slot;
    }
    return answer;
}
static bool InitTaskbarSymbols() {
    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {{L"public: struct tagSIZE __cdecl ClockButton::CalculateMinimumSize(struct tagSIZE)"},
            &g_calcMinimum, CalcMinimumHook},
        {{L"protected: virtual void __cdecl ClockButton::v_OnDisplayStateChange(bool)"},
            &g_updateClockState},
    };
    g_symbolsReady = WindhawkUtils::HookSymbols(GetModuleHandleW(nullptr), explorerExeHooks, ARRAYSIZE(explorerExeHooks));
    if (!g_symbolsReady)
        Wh_Log(L"User Tile: Windows 10 clock symbols unavailable. Keeping flyout/hotkey only.");
    return g_symbolsReady;
}
using EnumProcessModulesFn = BOOL (WINAPI*)(HANDLE, HMODULE*, DWORD, LPDWORD);
static EnumProcessModulesFn GetEnumProcessModulesFn() {
    static EnumProcessModulesFn fn = []() -> EnumProcessModulesFn {
        if (HMODULE kernel = GetModuleHandleW(L"kernel32.dll")) {
            if (auto address = GetProcAddress(kernel, "K32EnumProcessModules"))
                return reinterpret_cast<EnumProcessModulesFn>(address);
        }
        if (HMODULE psapi = LoadLibraryW(L"psapi.dll")) {
            if (auto address = GetProcAddress(psapi, "K32EnumProcessModules"))
                return reinterpret_cast<EnumProcessModulesFn>(address);
            if (auto address = GetProcAddress(psapi, "EnumProcessModules"))
                return reinterpret_cast<EnumProcessModulesFn>(address);
        }
        return nullptr;
    }();
    return fn;
}
static HMODULE FindExplorerPatcherTaskbarModule() {
    HMODULE modules[1024] = {};
    DWORD bytesNeeded = 0;
    auto enumModules = GetEnumProcessModulesFn();
    if (!enumModules ||
        !enumModules(GetCurrentProcess(), modules, sizeof(modules), &bytesNeeded))
        return nullptr;
    size_t count = (std::min)(static_cast<size_t>(bytesNeeded / sizeof(HMODULE)),
                             static_cast<size_t>(ARRAYSIZE(modules)));
    for (size_t i = 0; i < count; ++i) {
        WCHAR path[MAX_PATH] = {};
        DWORD length = GetModuleFileNameW(modules[i], path, ARRAYSIZE(path));
        if (!length || length >= ARRAYSIZE(path)) continue;
        PCWSTR name = wcsrchr(path, L'\\');
        name = name ? name + 1 : path;
        if (_wcsnicmp(name, L"ep_taskbar.", 11) == 0) return modules[i];
    }
    return nullptr;
}
static bool EnsureExplorerPatcherSymbols() {
    std::lock_guard<std::mutex> lock(g_epSymbolMutex);
    if (g_symbolsReady || g_stopping || !g_useExplorerPatcher) return g_symbolsReady;
    HMODULE module = FindExplorerPatcherTaskbarModule();
    if (!module) {
        if (!g_epWaitingLogged) {
            Wh_Log(L"User Tile: ExplorerPatcher ep_taskbar module not loaded yet; will retry.");
            g_epWaitingLogged = true;
        }
        return false;
    }
    if (module == g_epAttemptedModule) return false;
    g_epAttemptedModule = module;
    Wh_Log(L"User Tile: ExplorerPatcher ep_taskbar module detected.");

    //EP exports these functions directly(and it does not need symbol download) 
    void* calculate = reinterpret_cast<void*>(GetProcAddress(module,
        "?CalculateMinimumSize@ClockButton@@QEAA?AUtagSIZE@@U2@@Z"));
    void* updateState = reinterpret_cast<void*>(GetProcAddress(module,
        "?v_OnDisplayStateChange@ClockButton@@MEAAX_N@Z"));
    if (!calculate || !updateState) {
        Wh_Log(L"User Tile: ep_taskbar module found, but required ClockButton exports are missing.");
        return false;
    }
    if (!Wh_SetFunctionHook(calculate, reinterpret_cast<void*>(CalcMinimumHook),
                            reinterpret_cast<void**>(&g_calcMinimum))) {
        Wh_Log(L"User Tile: failed to register ExplorerPatcher CalculateMinimumSize hook.");
        return false;
    }
    g_updateClockState = reinterpret_cast<UpdateClockState>(updateState);
    //hooks added after init must be applied manually
    if (g_hooksApplied && !Wh_ApplyHookOperations()) {
        Wh_Log(L"User Tile: failed to apply ExplorerPatcher hook operations.");
        g_updateClockState = nullptr;
        return false;
    }
    g_symbolsReady = true;
    Wh_Log(L"User Tile: ExplorerPatcher ClockButton backend hooked successfully.");
    return true;
}
static bool EnsureTaskbarSymbols() {
    if (g_symbolsReady) return true;
    if (g_useExplorerPatcher) return EnsureExplorerPatcherSymbols();
    return false; //native Explorer symbols are loaded once in Wh_ModInit
}
static bool ChildRect(HWND child, HWND parent, RECT* rect) {
    if (!child || !GetWindowRect(child, rect)) return false;
    MapWindowPoints(nullptr, parent, reinterpret_cast<POINT*>(rect), 2);
    return true;
}
static HWND VisibleActionCenter(HWND parent) {
    //on classic Win10 Action Center is the TrayButton child
    HWND button = FindWindowExW(parent, nullptr, L"TrayButton", nullptr);
    if (!button || !IsWindowVisible(button)) return nullptr;
    RECT r = {};
    if (!ChildRect(button, parent, &r) || r.right <= r.left) return nullptr;
    return button;
}
static void PositionTile() {
    HWND tile = g_tile.load(), clock = g_clock.load();
    if (!IsWindow(tile) || !IsWindow(clock)) return;
    if (g_stopping || !g_slotAllocated || !EligibleTaskbar(g_taskbar.load()) || !IsWindowVisible(clock)) {
        ShowWindow(tile, SW_HIDE); return;
    }
    HWND parent = GetParent(clock);
    RECT clockRect = {};
    if (!ChildRect(clock, parent, &clockRect)) return;
    int slot = g_reservedSlot.load();
    RECT parentRect = {}; GetClientRect(parent, &parentRect);
    if (slot <= 0 || clockRect.right + slot > parentRect.right + 1) {
        ShowWindow(tile, SW_HIDE); return;
    }

    //explorer placed Action Center after the widened clock Move it back(like original one i tested)
    HWND actionCenter = VisibleActionCenter(parent);
    RECT actionRect = {};
    int actionWidth = 0;
    if (actionCenter && ChildRect(actionCenter, parent, &actionRect))
        actionWidth = actionRect.right - actionRect.left;
    else actionCenter = nullptr;

    RECT newAction = {clockRect.right, actionCenter ? actionRect.top : clockRect.top,
                      clockRect.right + actionWidth,
                      actionCenter ? actionRect.bottom : clockRect.bottom};
    RECT proposed = {newAction.right, clockRect.top,
                     newAction.right + slot, clockRect.bottom};
    if (proposed.right > parentRect.right + 1) {
        ShowWindow(tile, SW_HIDE); return;
    }

    //make sure the new positions don't overlap other controls
    bool collision = false;
    for (HWND child = GetWindow(parent, GW_CHILD); child; child = GetWindow(child, GW_HWNDNEXT)) {
        if (child == tile || child == clock || child == actionCenter || !IsWindowVisible(child)) continue;
        RECT childRect = {}, overlap = {};
        if (!ChildRect(child, parent, &childRect)) continue;
        bool tileCollision = IntersectRect(&overlap, &proposed, &childRect) &&
            overlap.right - overlap.left > 1 && overlap.bottom - overlap.top > 1;
        bool actionCollision = actionCenter &&
            IntersectRect(&overlap, &newAction, &childRect) &&
            overlap.right - overlap.left > 1 && overlap.bottom - overlap.top > 1;
        if (tileCollision || actionCollision) { collision = true; break; }
    }
    if (collision) {
        ShowWindow(tile, SW_HIDE);
        return;
    }

    int actionState = actionCenter ? 1 : 0;
    if (g_lastActionCenterState != actionState) {
        if (actionCenter)
            Wh_Log(L"User Tile: order Clock -> Action Center -> User Tile -> Show Desktop.");
        else
            Wh_Log(L"User Tile: Action Center hidden/off; order Clock -> User Tile -> Show Desktop.");
        g_lastActionCenterState = actionState;
    }
    if (actionCenter && actionRect.left != newAction.left) {
        SetWindowPos(actionCenter, nullptr, newAction.left, newAction.top, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    SetWindowPos(tile, HWND_TOP, proposed.left, proposed.top,
        proposed.right - proposed.left, proposed.bottom - proposed.top,
        SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(tile, nullptr, FALSE);
}
static HWND TileThemeSource(HWND hwnd) {
    HWND tray = g_tray.load();
    if (IsWindow(tray)) {
        HWND pager = FindWindowExW(tray, nullptr, L"SysPager", nullptr);
        HWND toolbar = FindWindowExW(pager, nullptr, L"ToolbarWindow32", nullptr);
        if (!toolbar) toolbar = FindWindowExW(tray, nullptr, L"ToolbarWindow32", nullptr);
        if (toolbar) return toolbar;
    }
    HWND taskbar = g_taskbar.load();
    return IsWindow(taskbar) ? taskbar : GetParent(hwnd);
}
static void DrawNativeTileState(HWND hwnd, HDC dc, const RECT& r) {
    if (!g_tileHover && !g_tilePressed) return;
    int state = g_tilePressed ? TS_PRESSED : TS_HOT;
    HWND source = TileThemeSource(hwnd);
    HTHEME theme = source ? OpenThemeData(source, L"Toolbar") : nullptr;
    if (theme) {
        HRESULT result = DrawThemeBackground(theme, dc, TP_BUTTON, state, &r, &r);
        CloseThemeData(theme);
        if (SUCCEEDED(result)) return;
    }
    //fallback
    RECT edge = r;
    HBRUSH brush = CreateSolidBrush(RGB(130, 130, 130));
    FrameRect(dc, &edge, brush);
    DeleteObject(brush);
}
static void PaintTile(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC target = BeginPaint(hwnd, &ps);
    RECT r = {}; GetClientRect(hwnd, &r);
    if (r.right <= 0 || r.bottom <= 0) { EndPaint(hwnd, &ps); return; }
    HDC dc = CreateCompatibleDC(target);
    HBITMAP buffer = CreateCompatibleBitmap(target, r.right, r.bottom);
    HGDIOBJ old = SelectObject(dc, buffer);
    //draw the taskbars own background behind the tile
    Fill(dc, r, RGB(30, 30, 30));
    DrawThemeParentBackground(hwnd, dc, &r);
    DrawNativeTileState(hwnd, dc, r);
    int pad = (std::max)(2, MulDiv(3, TileDpi(hwnd), 96));
    int desired = MulDiv(g_pictureSize, TileDpi(hwnd), 96);
    int side = (std::max)(1, (std::min)(desired, static_cast<int>((std::min)(r.right, r.bottom)) - 2 * pad));
    int x = (r.right - side) / 2, y = (r.bottom - side) / 2;
    RECT pic = {x, y, x + side, y + side};
    Fill(dc, pic, RGB(247, 247, 247));
    {
        std::lock_guard<std::recursive_mutex> lock(g_accountMutex);
        if (g_gdip) {
            Gdiplus::Graphics graphics(dc);
            graphics.SetClip(Gdiplus::Rect(x, y, side, side));
            graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
            if (g_picture) {
                int w = static_cast<int>(g_picture->GetWidth());
                int h = static_cast<int>(g_picture->GetHeight());
                int crop = (std::min)(w, h);
                graphics.DrawImage(g_picture.get(), Gdiplus::Rect(x, y, side, side),
                    (w - crop) / 2, (h - crop) / 2, crop, crop, Gdiplus::UnitPixel);
            } else {
                graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                Gdiplus::SolidBrush ink(Gdiplus::Color(255, 145, 145, 145));
                float unit = side / 100.0f;
                graphics.FillEllipse(&ink, x + 35 * unit, y + 20 * unit, 30 * unit, 30 * unit);
                graphics.FillEllipse(&ink, x + 25 * unit, y + 55 * unit, 50 * unit, 30 * unit);
            }
        }
    }
    BitBlt(target, 0, 0, r.right, r.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, old); DeleteObject(buffer); DeleteDC(dc);
    EndPaint(hwnd, &ps);
}
static LRESULT CALLBACK TileProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT: PaintTile(hwnd); return 0;
    case WM_ERASEBKGND: return 1;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_MOUSEMOVE: {
        if (!g_tileHover) { g_tileHover = true; InvalidateRect(hwnd, nullptr, FALSE); }
        TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, hwnd, 0};
        TrackMouseEvent(&track); return 0;
    }
    case WM_MOUSELEAVE:
        g_tileHover = false; InvalidateRect(hwnd, nullptr, FALSE); return 0;
    case WM_LBUTTONDOWN:
        g_tileFlyoutVisibleOnPress = g_flyout && IsWindowVisible(g_flyout);
        g_tilePressed = true; SetCapture(hwnd);
        InvalidateRect(hwnd, nullptr, FALSE); return 0;
    case WM_LBUTTONUP: {
        bool pressed = g_tilePressed; g_tilePressed = false;
        InvalidateRect(hwnd, nullptr, FALSE);
        if (GetCapture() == hwnd) ReleaseCapture();
        RECT r = {}; GetClientRect(hwnd, &r);
        POINT point = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
        if (pressed && PtInRect(&r, point) && !g_stopping && g_flyout)
            PostMessageW(g_flyout, WM_SHOWFLYOUT, g_tileFlyoutVisibleOnPress ? 1 : 0,
                         reinterpret_cast<LPARAM>(hwnd));
        return 0;
    }
    case WM_CAPTURECHANGED:
        g_tilePressed = false; InvalidateRect(hwnd, nullptr, FALSE); return 0;
    case WM_THEMECHANGED:
        InvalidateRect(hwnd, nullptr, FALSE); return 0;
    case WM_DESTROY:
        if (g_tile.load() == hwnd) g_tile = nullptr;
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
static void RelayoutTaskbar() {
    HWND taskbar = g_taskbar.load();
    if (!IsWindow(taskbar)) return;
    RECT r = {}; GetClientRect(taskbar, &r);
    SendMessageW(taskbar, WM_SIZE, SIZE_RESTORED, MAKELPARAM(r.right, r.bottom));
    InvalidateRect(taskbar, nullptr, TRUE);
}
static LRESULT CALLBACK ClockSubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    if (msg == g_setupMsg) {
        if (g_stopping) return 0;
        if (wp == 1) { PositionTile(); return 0; }
        if (!IsWindow(g_tile.load())) {
            g_slotAllocated = false;
            g_tile = CreateWindowExW(WS_EX_NOPARENTNOTIFY, kTileClass, L"User account",
                WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 1, 1, GetParent(hwnd), nullptr,
                GetModuleHandleW(nullptr), nullptr);
        }
        if (!g_tile.load()) { Wh_Log(L"User Tile: child button creation failed (%lu).", GetLastError()); return 0; }
        void* object = reinterpret_cast<void*>(GetWindowLongPtrW(hwnd, 0));
        if (object && g_updateClockState) g_updateClockState(object, true);
        RelayoutTaskbar();
        PositionTile();
        return 0;
    }
    if (msg == g_cleanupMsg) {
        g_slotAllocated = false;
        HWND tile = g_tile.exchange(nullptr);
        if (IsWindow(tile)) DestroyWindow(tile);
        void* object = reinterpret_cast<void*>(GetWindowLongPtrW(hwnd, 0));
        if (object && g_updateClockState) g_updateClockState(object, true);
        RelayoutTaskbar();
        return 0;
    }
    if (msg == WM_WINDOWPOSCHANGING) {
        LRESULT answer = DefSubclassProc(hwnd, msg, wp, lp);
        auto pos = reinterpret_cast<WINDOWPOS*>(lp);
        if (!g_stopping && EligibleTaskbar(g_taskbar.load()) && !(pos->flags & SWP_NOSIZE)) {
            int nativeWidth = g_nativeClockWidth.load(), slot = g_reservedSlot.load();
            //give the extra width back to the tile but only if hook added it
            if (nativeWidth > 0 && slot > 0 && pos->cx == nativeWidth + slot) {
                pos->cx = nativeWidth;
                if (!g_slotAllocated) Wh_Log(L"User Tile: reserved clock width split successfully.");
                g_slotAllocated = true;
            } else if (pos->cx != nativeWidth) {
                g_slotAllocated = false;
            }
        }
        return answer;
    }
    LRESULT answer = DefSubclassProc(hwnd, msg, wp, lp);
    if (msg == WM_WINDOWPOSCHANGED || msg == WM_SHOWWINDOW || msg == WM_THEMECHANGED || msg == WM_DPICHANGED)
        PositionTile();
    if (msg == WM_NCDESTROY) {
        HWND tile = g_tile.exchange(nullptr);
        if (IsWindow(tile)) DestroyWindow(tile);
        g_clock = nullptr; g_slotAllocated = false;
        g_nativeClockWidth = 0; g_reservedSlot = 0;
    }
    return answer;
}
static LRESULT CALLBACK TraySubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR) {
    LRESULT answer = DefSubclassProc(hwnd, msg, wp, lp);
    //explorer has finished its own layout so replace the tile now to make sure nothing overwrites our position
    if (msg == WM_SIZE || msg == WM_WINDOWPOSCHANGED || msg == WM_SETTINGCHANGE ||
        msg == WM_THEMECHANGED || msg == WM_DPICHANGED)
        PositionTile();
    if (msg == WM_NCDESTROY) g_tray = nullptr;
    return answer;
}
static void DiscoverTaskbar() {
    std::lock_guard<std::mutex> attachLock(g_attachMutex);
    if (g_nativeWin11Mode) { DiscoverWin11Taskbar(); return; }
    if (g_stopping || !g_hooksApplied || !g_win10Mode) return;
    if (!EnsureTaskbarSymbols()) return;
    if (IsWindow(g_clock.load())) {
        SendMessageW(g_clock.load(), g_setupMsg, 1, 0);
        return;
    }
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD process = 0; GetWindowThreadProcessId(taskbar, &process);
    if (!taskbar || process != GetCurrentProcessId()) return;
    HWND tray = FindWindowExW(taskbar, nullptr, L"TrayNotifyWnd", nullptr);
    HWND clock = tray ? FindWindowExW(tray, nullptr, L"TrayClockWClass", nullptr) : nullptr;
    if (!clock) return;
    g_taskbar = taskbar; g_clock = clock;
    if (!EligibleTaskbar(taskbar)) {
        g_clock = nullptr; g_taskbar = nullptr; return;
    }
    if (!IsWindow(g_tray.load())) {
        g_tray = tray;
        if (!WindhawkUtils::SetWindowSubclassFromAnyThread(tray, TraySubclass, 0)) {
            Wh_Log(L"User Tile: tray parent subclass failed; using timer/clock updates.");
            g_tray = nullptr;
        }
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(clock, ClockSubclass, 0)) {
        g_clock = nullptr; g_taskbar = nullptr;
        HWND attachedTray = g_tray.exchange(nullptr);
        if (IsWindow(attachedTray))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(attachedTray, TraySubclass);
        Wh_Log(L"User Tile: failed to attach clock subclass."); return;
    }
    if (g_useExplorerPatcher)
        Wh_Log(L"User Tile: attaching to ExplorerPatcher primary clock %p.", clock);
    else
        Wh_Log(L"User Tile: attaching to original Windows 10 primary clock %p.", clock);
    SendMessageW(clock, g_setupMsg, 0, 0);
}
static void DetachTaskbar() {
    std::lock_guard<std::mutex> attachLock(g_attachMutex);
    HWND clock = g_clock.load();
    if (IsWindow(clock)) {
        SendMessageW(clock, g_cleanupMsg, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(clock, ClockSubclass);
    }
    HWND tray = g_tray.exchange(nullptr);
    if (IsWindow(tray)) WindhawkUtils::RemoveWindowSubclassFromAnyThread(tray, TraySubclass);
    g_clock = nullptr; g_taskbar = nullptr;
    g_lastActionCenterState = -1;
    g_nativeClockWidth = 0; g_reservedSlot = 0;
}

//thanks to taskbar fluent media player mod for this

namespace wx = winrt::Windows::UI::Xaml;
namespace wxc = winrt::Windows::UI::Xaml::Controls;
namespace wxm = winrt::Windows::UI::Xaml::Media;
namespace wxmi = winrt::Windows::UI::Xaml::Media::Imaging;
namespace wui = winrt::Windows::UI;

static std::atomic<bool> g_xamlSymbolsReady{false}, g_xamlReady{false};
static std::atomic<bool> g_xamlCompatibilityFailed{false};
static std::atomic<bool> g_xamlRetryLogged{false};
static std::atomic<HWND> g_xamlTaskbar{nullptr};
static std::atomic<ULONGLONG> g_xamlNextAttempt{0};
[[clang::no_destroy]] static wxc::Panel g_xamlParent{nullptr};
[[clang::no_destroy]] static wxc::Button g_xamlButton{nullptr};
[[clang::no_destroy]] static wxc::Image g_xamlImage{nullptr};
static winrt::event_token g_xamlClickToken{};
[[clang::no_destroy]] static winrt::Windows::Foundation::IInspectable g_xamlPointerPressedHandler{nullptr};
static bool g_xamlFlyoutVisibleOnPress = false;
static bool g_xamlPointerPressActive = false;
static int g_xamlColumn = -1;
static bool g_xamlUsesGrid = false;
static constexpr wchar_t kXamlTileName[] = L"WindhawkUserTile";

using CTaskBandGetHost = void* (WINAPI*)(void*, void*);
using TaskbarHostFrameHeight = int (WINAPI*)(void*);
using StdRefDecref = void (WINAPI*)(void*);
using TrayUIStartTaskbar = void (WINAPI*)(void*);
static CTaskBandGetHost g_ctaskBandGetHost = nullptr;
static TaskbarHostFrameHeight g_taskbarHostFrameHeight = nullptr;
static StdRefDecref g_stdRefDecref = nullptr;
static TrayUIStartTaskbar g_trayUIStartTaskbar = nullptr;
static void* g_ctaskBandSiteVftable = nullptr;

using WindowThreadProc = void (*)(void*);
//runs proc on the taskbars own thread 
static bool RunOnTaskbarThread(HWND hwnd, WindowThreadProc proc, void* param) {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Payload { WindowThreadProc proc; void* param; };
    DWORD thread = GetWindowThreadProcessId(hwnd, nullptr);
    if (!thread || !proc) return false;
    if (thread == GetCurrentThreadId()) { proc(param); return true; }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC,
        [](int code, WPARAM wp, LPARAM lp) -> LRESULT {
            if (code == HC_ACTION) {
                auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lp);
                static const UINT msg = RegisterWindowMessageW(
                    L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == msg) {
                    auto* payload = reinterpret_cast<Payload*>(cwp->lParam);
                    payload->proc(payload->param);
                }
            }
            return CallNextHookEx(nullptr, code, wp, lp);
        }, nullptr, thread);
    if (!hook) return false;
    Payload payload{proc, param};
    SendMessageW(hwnd, message, 0, reinterpret_cast<LPARAM>(&payload));
    UnhookWindowsHookEx(hook);
    return true;
}
static HWND FindNativeTaskbarWindow() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD process = 0;
    GetWindowThreadProcessId(taskbar, &process);
    return taskbar && process == GetCurrentProcessId() ? taskbar : nullptr;
}
static bool ReadableXamlMemory(const void* address, size_t size) {
    if (!address || !size) return false;
    MEMORY_BASIC_INFORMATION memory = {};
    if (!VirtualQuery(address, &memory, sizeof(memory)) ||
        memory.State != MEM_COMMIT ||
        (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) return false;
    auto start = reinterpret_cast<uintptr_t>(address);
    auto base = reinterpret_cast<uintptr_t>(memory.BaseAddress);
    return start >= base && start - base <= memory.RegionSize &&
           size <= memory.RegionSize - (start - base);
}
static wx::XamlRoot GetNativeTaskbarXamlRoot(HWND taskbar) {
    HWND taskbandWindow = static_cast<HWND>(GetPropW(taskbar, L"TaskbandHWND"));
    if (!taskbandWindow) return nullptr;
    void* taskband = reinterpret_cast<void*>(GetWindowLongPtrW(taskbandWindow, 0));
    if (!taskband || !g_ctaskBandGetHost || !g_ctaskBandSiteVftable ||
        !g_taskbarHostFrameHeight || !g_stdRefDecref) return nullptr;
    void* site = taskband;
    int slot = 0;
    for (;; ++slot) {
        if (!ReadableXamlMemory(site, sizeof(void*))) return nullptr;
        if (*reinterpret_cast<void**>(site) == g_ctaskBandSiteVftable) break;
        if (slot >= 20) {
            Wh_Log(L"User Tile: taskband ITaskListWndSite vftable not found.");
            return nullptr;
        }
        site = static_cast<void**>(site) + 1;
    }
    void* hostSharedPtr[2] = {};
    g_ctaskBandGetHost(site, hostSharedPtr);
    auto releaseHost = [&]() {
        if (hostSharedPtr[1]) g_stdRefDecref(hostSharedPtr[1]);
    };
    if (!hostSharedPtr[0]) { releaseHost(); return nullptr; }

    //Xaml
    const BYTE* code = reinterpret_cast<const BYTE*>(g_taskbarHostFrameHeight);
    size_t offset = 0;
#if defined(_M_X64)
    // 48:83EC 28 | sub rsp,28
    // 48:83C1 48 | add rcx,48
    if (ReadableXamlMemory(code, 8) && code[0] == 0x48 && code[1] == 0x83 &&
        code[2] == 0xEC && code[4] == 0x48 && code[5] == 0x83 &&
        code[6] == 0xC1 && code[7] <= 0x7F) {
        offset = code[7];
    }
#elif defined(_M_ARM64)
    // 7f2303d5 pacibsp
    // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
    // fd030091 mov     fp, sp
    // 080c41f8 ldr     x8, [x0, #0x10]!
    const DWORD* p = reinterpret_cast<const DWORD*>(code);
    if (ReadableXamlMemory(p, 16) && p[0] == 0xD503237F &&
        (p[1] & 0xFFC07FFF) == 0xA9807BFD && p[2] == 0x910003FD &&
        (p[3] & 0xFFF00FE0) == 0xF8400C00) {
        offset = (p[3] >> 12) & 0xFF;
    }
#else
#error "Unsupported architecture"
#endif
    if (!offset) {
        g_xamlCompatibilityFailed = true;
        Wh_Log(L"User Tile: unsupported TaskbarHost::FrameHeight; XAML backend disabled for this build.");
        releaseHost();
        return nullptr;
    }
    auto hostBytes = static_cast<BYTE*>(hostSharedPtr[0]);
    if (!ReadableXamlMemory(hostBytes + offset, sizeof(IUnknown*))) {
        releaseHost(); return nullptr;
    }
    IUnknown* elementUnknown = *reinterpret_cast<IUnknown**>(hostBytes + offset);
    if (!elementUnknown) { releaseHost(); return nullptr; }
    wx::FrameworkElement element{nullptr};
    HRESULT hr = elementUnknown->QueryInterface(
        winrt::guid_of<wx::FrameworkElement>(), winrt::put_abi(element));
    wx::XamlRoot result = element ? element.XamlRoot() : nullptr;
    releaseHost();
    return SUCCEEDED(hr) ? result : nullptr;
}
static wx::FrameworkElement FindXamlChildByName(
    wx::FrameworkElement const& parent, const wchar_t* name, int depth = 32) {
    if (!parent || depth <= 0) return nullptr;
    int count = wxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = wxm::VisualTreeHelper::GetChild(parent, i).try_as<wx::FrameworkElement>();
        if (!child) continue;
        if (child.Name() == name) return child;
        if (auto found = FindXamlChildByName(child, name, depth - 1)) return found;
    }
    return nullptr;
}
static wxmi::WriteableBitmap MakeXamlAccountBitmap(int pixelSize) {
    if (!g_gdip || pixelSize <= 0) return nullptr;
    wxmi::WriteableBitmap bitmap(pixelSize, pixelSize);
    std::lock_guard<std::recursive_mutex> lock(g_accountMutex);
    Gdiplus::Bitmap canvas(pixelSize, pixelSize, PixelFormat32bppPARGB);
    Gdiplus::Graphics graphics(&canvas);
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
    graphics.Clear(Gdiplus::Color(255, 247, 247, 247));
    if (g_picture) {
        int width = static_cast<int>(g_picture->GetWidth());
        int height = static_cast<int>(g_picture->GetHeight());
        int side = (std::min)(width, height);
        graphics.DrawImage(g_picture.get(), Gdiplus::Rect(0, 0, pixelSize, pixelSize),
            (width - side) / 2, (height - side) / 2, side, side, Gdiplus::UnitPixel);
    } else {
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::SolidBrush ink(Gdiplus::Color(255, 145, 145, 145));
        float unit = pixelSize / 100.0f;
        graphics.FillEllipse(&ink, 35 * unit, 20 * unit, 30 * unit, 30 * unit);
        graphics.FillEllipse(&ink, 25 * unit, 55 * unit, 50 * unit, 30 * unit);
    }
    Gdiplus::BitmapData data = {};
    Gdiplus::Rect rect(0, 0, pixelSize, pixelSize);
    if (canvas.LockBits(&rect, Gdiplus::ImageLockModeRead,
                        PixelFormat32bppPARGB, &data) == Gdiplus::Ok) {
        auto buffer = bitmap.PixelBuffer();
        auto access = buffer.as<Windows::Storage::Streams::IBufferByteAccess>();
        BYTE* pixels = nullptr;
        if (SUCCEEDED(access->Buffer(&pixels)) && pixels) {
            for (int y = 0; y < pixelSize; ++y) {
                memcpy(pixels + static_cast<size_t>(y) * pixelSize * 4,
                       static_cast<BYTE*>(data.Scan0) + static_cast<size_t>(y) * data.Stride,
                       static_cast<size_t>(pixelSize) * 4);
            }
        }
        buffer.Length(static_cast<uint32_t>(static_cast<size_t>(pixelSize) * pixelSize * 4));
        canvas.UnlockBits(&data);
    }
    bitmap.Invalidate();
    return bitmap;
}
static void PostFlyoutFromXaml(wx::FrameworkElement const& element, bool visibleOnPress) {
    if (g_stopping || !g_flyout) return;
    auto anchor = std::make_unique<RECT>();
    HWND taskbar = g_xamlTaskbar.load();
    bool valid = taskbar && GetClientRect(taskbar, anchor.get());
    if (valid) {
        POINT origin = {};
        ClientToScreen(taskbar, &origin);
        try {
            auto root = element.XamlRoot().Content().try_as<wx::FrameworkElement>();
            double scale = element.XamlRoot().RasterizationScale();
            auto transform = element.TransformToVisual(root);
            auto topLeft = transform.TransformPoint({0, 0});
            auto bottomRight = transform.TransformPoint(
                {static_cast<float>(element.ActualWidth()),
                 static_cast<float>(element.ActualHeight())});
            anchor->left = origin.x + static_cast<LONG>(std::lround(topLeft.X * scale));
            anchor->top = origin.y + static_cast<LONG>(std::lround(topLeft.Y * scale));
            anchor->right = origin.x + static_cast<LONG>(std::lround(bottomRight.X * scale));
            anchor->bottom = origin.y + static_cast<LONG>(std::lround(bottomRight.Y * scale));
        } catch (...) { valid = false; }
    }
    if (!valid) {
        if (!PostMessageW(g_flyout, WM_SHOWFLYOUT, visibleOnPress ? 1 : 0,
                          reinterpret_cast<LPARAM>(taskbar))) return;
        return;
    }
    if (!PostMessageW(g_flyout, WM_SHOWFLYOUT_AT, visibleOnPress ? 1 : 0,
                      reinterpret_cast<LPARAM>(anchor.get()))) return;
    anchor.release();
}
static wxc::Button BuildXamlUserTile(wx::XamlRoot const& root) {
    wxc::Button button;
    button.Name(kXamlTileName);
    button.Width(g_tileWidth);
    button.MinWidth(0);
    button.MinHeight(0);
    button.Padding({0, 0, 0, 0});
    button.BorderThickness({0, 0, 0, 0});
    wxm::SolidColorBrush transparent(wui::Colors::Transparent());
    button.Background(transparent);
    button.BorderBrush(transparent);
    button.HorizontalAlignment(wx::HorizontalAlignment::Center);
    button.VerticalAlignment(wx::VerticalAlignment::Stretch);
    button.HorizontalContentAlignment(wx::HorizontalAlignment::Center);
    button.VerticalContentAlignment(wx::VerticalAlignment::Center);

    wxc::Image image;
    image.Width(g_pictureSize);
    image.Height(g_pictureSize);
    image.Stretch(wxm::Stretch::Uniform);
    double scale = root ? root.RasterizationScale() : 1.0;
    if (scale <= 0) scale = 1.0;
    int pixels = (std::max)(16, (std::min)(512,
        static_cast<int>(std::lround(g_pictureSize * scale))));
    image.Source(MakeXamlAccountBitmap(pixels));
    button.Content(image);
    g_xamlImage = image;
    g_xamlPointerPressedHandler = winrt::box_value(wx::Input::PointerEventHandler(
        [](winrt::Windows::Foundation::IInspectable const&,
           wx::Input::PointerRoutedEventArgs const&) {
            g_xamlFlyoutVisibleOnPress = g_flyout && IsWindowVisible(g_flyout);
            g_xamlPointerPressActive = true;
        }));
    button.AddHandler(wx::UIElement::PointerPressedEvent(),
                      g_xamlPointerPressedHandler, true);
    g_xamlClickToken = button.Click(
        [](winrt::Windows::Foundation::IInspectable const& sender,
           wx::RoutedEventArgs const&) {
            if (auto element = sender.try_as<wx::FrameworkElement>()) {
                bool visibleOnPress = g_xamlPointerPressActive
                    ? g_xamlFlyoutVisibleOnPress
                    : (g_flyout && IsWindowVisible(g_flyout));
                g_xamlPointerPressActive = false;
                PostFlyoutFromXaml(element, visibleOnPress);
            }
        });
    return button;
}
static void RemoveWin11TileOnTaskbarThread() {
    try {
        if (g_xamlButton && g_xamlPointerPressedHandler) {
            g_xamlButton.RemoveHandler(wx::UIElement::PointerPressedEvent(),
                                       g_xamlPointerPressedHandler);
        }
    } catch (...) {}
    g_xamlPointerPressedHandler = nullptr;
    g_xamlPointerPressActive = false;
    try {
        if (g_xamlButton && g_xamlClickToken.value) {
            g_xamlButton.Click(g_xamlClickToken);
        }
    } catch (...) {}
    g_xamlClickToken = {};
    try {
        if (g_xamlParent) {
            auto children = g_xamlParent.Children();
            for (int i = static_cast<int>(children.Size()) - 1; i >= 0; --i) {
                auto element = children.GetAt(i).try_as<wx::FrameworkElement>();
                if (element && element.Name() == kXamlTileName) children.RemoveAt(i);
            }
            if (g_xamlUsesGrid && g_xamlColumn >= 0) {
                auto grid = g_xamlParent.as<wxc::Grid>();
                for (auto const& child : children) {
                    auto element = child.try_as<wx::FrameworkElement>();
                    if (element) {
                        int column = wxc::Grid::GetColumn(element);
                        if (column > g_xamlColumn) wxc::Grid::SetColumn(element, column - 1);
                    }
                }
                if (g_xamlColumn < static_cast<int>(grid.ColumnDefinitions().Size()))
                    grid.ColumnDefinitions().RemoveAt(g_xamlColumn);
            }
        }
    } catch (...) {}
    g_xamlReady = false;
    g_xamlParent = nullptr;
    g_xamlButton = nullptr;
    g_xamlImage = nullptr;
    g_xamlColumn = -1;
    g_xamlUsesGrid = false;
}
static bool InjectWin11TileOnTaskbarThread() {
    if (g_xamlReady || g_stopping || !g_nativeWin11Mode) return g_xamlReady;
    HWND taskbar = FindNativeTaskbarWindow();
    if (!taskbar) return false;
    g_xamlTaskbar = taskbar;
    try {
        auto root = GetNativeTaskbarXamlRoot(taskbar);
        if (!root || !root.Content()) return false;
        auto content = root.Content().try_as<wx::FrameworkElement>();
        auto tray = content ? FindXamlChildByName(content, L"SystemTrayFrameGrid") : nullptr;
        auto showDesktop = content ? FindXamlChildByName(content, L"ShowDesktopStack") : nullptr;
        if (!tray || !showDesktop) return false;
        auto button = BuildXamlUserTile(root);
        g_xamlButton = button;

        if (auto grid = tray.try_as<wxc::Grid>()) {
            int column = wxc::Grid::GetColumn(showDesktop);
            if (column < 0 || column >= static_cast<int>(grid.ColumnDefinitions().Size())) {
                RemoveWin11TileOnTaskbarThread(); return false;
            }
            g_xamlParent = grid;
            g_xamlUsesGrid = true;
            wxc::ColumnDefinition definition;
            definition.Width({1.0, wx::GridUnitType::Auto});
            grid.ColumnDefinitions().InsertAt(column, definition);
            g_xamlColumn = column;
            auto children = grid.Children();
            for (auto const& child : children) {
                auto element = child.try_as<wx::FrameworkElement>();
                if (element) {
                    int childColumn = wxc::Grid::GetColumn(element);
                    if (childColumn >= column) wxc::Grid::SetColumn(element, childColumn + 1);
                }
            }
            wxc::Grid::SetColumn(button, column);
            children.Append(button);
        } else if (auto stack = tray.try_as<wxc::StackPanel>()) {
            if (stack.Orientation() != wxc::Orientation::Horizontal) {
                RemoveWin11TileOnTaskbarThread(); return false;
            }
            g_xamlParent = stack;
            g_xamlUsesGrid = false;
            auto children = stack.Children();
            uint32_t index = children.Size();
            for (uint32_t i = 0; i < children.Size(); ++i) {
                auto element = children.GetAt(i).try_as<wx::FrameworkElement>();
                if (element && element.Name() == L"ShowDesktopStack") { index = i; break; }
            }
            if (index == children.Size()) {
                Wh_Log(L"User Tile: native StackPanel has no direct ShowDesktopStack child.");
                RemoveWin11TileOnTaskbarThread();
                return false;
            }
            children.InsertAt(index, button);
        } else {
            RemoveWin11TileOnTaskbarThread();
            return false;
        }
        g_xamlReady = true;
        g_xamlNextAttempt = 0;
        Wh_Log(L"User Tile: native Windows 11 XAML user tile inserted before Show Desktop.");
        return true;
    } catch (winrt::hresult_error const& error) {
        Wh_Log(L"User Tile: XAML injection failed (0x%08X): %ls",
               static_cast<unsigned>(error.code()), error.message().c_str());
    } catch (...) {
        Wh_Log(L"User Tile: XAML injection failed with an unknown exception.");
    }
    RemoveWin11TileOnTaskbarThread();
    return false;
}
static void RemoveWin11Tile() {
    HWND taskbar = g_xamlTaskbar.load();
    if (taskbar && IsWindow(taskbar)) {
        RunOnTaskbarThread(taskbar, [](void*) { RemoveWin11TileOnTaskbarThread(); }, nullptr);
    }
    g_xamlReady = false;
}
static void RefreshWin11TileImage() {
    if (!g_nativeWin11Mode || !g_xamlReady) return;
    HWND taskbar = g_xamlTaskbar.load();
    if (!taskbar || !IsWindow(taskbar)) return;
    RunOnTaskbarThread(taskbar, [](void*) {
        if (!g_xamlImage || g_stopping) return;
        try {
            auto root = g_xamlImage.XamlRoot();
            double scale = root ? root.RasterizationScale() : 1.0;
            if (scale <= 0) scale = 1.0;
            int pixels = (std::max)(16, (std::min)(512,
                static_cast<int>(std::lround(g_pictureSize * scale))));
            g_xamlImage.Source(MakeXamlAccountBitmap(pixels));
        } catch (...) {}
    }, nullptr);
}
static void DiscoverWin11Taskbar() {
    if (g_stopping || !g_nativeWin11Mode || !g_hooksApplied ||
        !g_xamlSymbolsReady || g_xamlReady || g_xamlCompatibilityFailed) return;
    ULONGLONG now = GetTickCount64();
    if (now < g_xamlNextAttempt) return;
    HWND taskbar = FindNativeTaskbarWindow();
    if (!taskbar) return;
    RunOnTaskbarThread(taskbar, [](void*) { InjectWin11TileOnTaskbarThread(); }, nullptr);
    if (g_xamlReady) {
        g_xamlRetryLogged = false;
    } else {
        g_xamlNextAttempt = now + 5000;
        if (!g_xamlCompatibilityFailed && !g_xamlRetryLogged.exchange(true))
            Wh_Log(L"User Tile: native XAML tray not ready; retrying every 5 seconds.");
    }
}
static void WINAPI TrayUIStartTaskbarHook(void* object) {
    if (g_nativeWin11Mode && !g_stopping && g_xamlReady)
        RemoveWin11TileOnTaskbarThread();
    g_trayUIStartTaskbar(object);
    if (g_nativeWin11Mode && !g_stopping) {
        g_xamlNextAttempt = 0;
        if (g_flyout) PostMessageW(g_flyout, WM_TIMER, kDiscoveryTimer, 0);
    }
}
static bool InitXamlTaskbarSymbols() {
    HMODULE taskbarModule = LoadLibraryExW(
        L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbarModule) {
        Wh_Log(L"User Tile: taskbar.dll could not be loaded for native XAML backend.");
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{L"const CTaskBand::`vftable'{for `ITaskListWndSite'}"},
            &g_ctaskBandSiteVftable},
        {{L"public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const "},
            &g_ctaskBandGetHost},
        {{L"public: int __cdecl TaskbarHost::FrameHeight(void)const "},
            &g_taskbarHostFrameHeight},
        {{L"public: void __cdecl std::_Ref_count_base::_Decref(void)"},
            &g_stdRefDecref},
        {{L"public: virtual void __cdecl TrayUI::StartTaskbar(void)"},
            &g_trayUIStartTaskbar, TrayUIStartTaskbarHook},
    };
    g_xamlCompatibilityFailed = false;
    g_xamlSymbolsReady = WindhawkUtils::HookSymbols(
    taskbarModule, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
    if (!g_xamlSymbolsReady)
        Wh_Log(L"User Tile: native XAML taskbar symbols unavailable; keeping flyout/hotkey only.");
    return g_xamlSymbolsReady;
}

static DWORD WINAPI UiThread(LPVOID) {
    //Make only this thread DPI aware not all of Explorer
    using SetAwareness = HANDLE (WINAPI*)(HANDLE);
    auto setAwareness = reinterpret_cast<SetAwareness>(GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext"));
    HANDLE oldAwareness = nullptr;
    if (setAwareness) {
        oldAwareness = setAwareness(reinterpret_cast<HANDLE>(static_cast<INT_PTR>(-4)));
        if (!oldAwareness)
            oldAwareness = setAwareness(reinterpret_cast<HANDLE>(static_cast<INT_PTR>(-3)));
    }
    Gdiplus::GdiplusStartupInput startup;
    if (Gdiplus::GdiplusStartup(&g_gdip, &startup, nullptr) != Gdiplus::Ok) g_gdip = 0;
    const wchar_t* cls = L"WhUserTileFlyoutV04";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = FlyoutProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = cls;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    ATOM atom = RegisterClassW(&wc);
    WNDCLASSW tileClass = {};
    tileClass.lpfnWndProc = TileProc;
    tileClass.hInstance = wc.hInstance;
    tileClass.lpszClassName = kTileClass;
    tileClass.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    ATOM tileAtom = RegisterClassW(&tileClass);
    SetDpi(96);
    g_nativeFrame = g_borderSetting == 3;
    if (atom) g_flyout = CreateWindowExW(FlyoutExStyle(),
        cls, L"User Tile", FlyoutStyle(), 0, 0, W, H, nullptr, nullptr, wc.hInstance, nullptr);
    if (g_flyout) {
        ApplyNativeFrame(g_flyout);
        g_initialized = true;
        if (!g_hotkeyEnabled) {
            Wh_Log(L"User Tile: keyboard shortcut disabled in settings.");
        } else if (!RegisterHotKey(g_flyout, HOTKEY_ID, HOTKEY_MODS, HOTKEY_VK)) {
            Wh_Log(L"User Tile: RegisterHotKey failed (%lu); continuing without the shortcut.", GetLastError());
        }
    }
    if (g_initialized) {
        RefreshAccount();
        SetTimer(g_flyout, kDiscoveryTimer, 2000, nullptr);
    }
    SetEvent(g_ready);
    if (g_initialized) {
        MSG m;
        while (GetMessageW(&m, nullptr, 0, 0) > 0) {
            TranslateMessage(&m); DispatchMessageW(&m);
        }
        if (IsWindow(g_flyout)) DestroyWindow(g_flyout);
    }
    g_flyout = nullptr;
    g_picture.reset();
    if (tileAtom) UnregisterClassW(kTileClass, wc.hInstance);
    if (g_gdip) { Gdiplus::GdiplusShutdown(g_gdip); g_gdip = 0; }
    if (g_font) { DeleteObject(g_font); g_font = nullptr; }
    if (g_fontUnderline) { DeleteObject(g_fontUnderline); g_fontUnderline = nullptr; }
    if (atom) UnregisterClassW(cls, wc.hInstance);
    if (setAwareness && oldAwareness) setAwareness(oldAwareness);
    return 0;
}
static DWORD GetOsBuildNumber() {
    typedef LONG(WINAPI* RtlGetVersion_t)(PRTL_OSVERSIONINFOW);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto fn = ntdll ? reinterpret_cast<RtlGetVersion_t>(
                          GetProcAddress(ntdll, "RtlGetVersion")) : nullptr;
    RTL_OSVERSIONINFOW vi = {};
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (fn && fn(&vi) == 0) return vi.dwBuildNumber;
    return 0;
}
BOOL Wh_ModInit() {
    g_initialized = false;
    g_stopping = false;
    PCWSTR mode = Wh_GetStringSetting(L"taskbarMode");
    bool autoMode = !mode || !*mode || wcscmp(mode, L"auto") == 0;
    g_useExplorerPatcher = mode && wcscmp(mode, L"explorerpatcher") == 0;
    g_nativeWin11Mode = (mode && wcscmp(mode, L"win11") == 0) ||
                        (autoMode && GetOsBuildNumber() >= 22000);
    g_win10Mode = !g_nativeWin11Mode &&
                  (autoMode || (mode && wcscmp(mode, L"win10") == 0) || g_useExplorerPatcher);
    if (mode) Wh_FreeStringSetting(mode);
    g_tileWidth = (std::clamp)(Wh_GetIntSetting(L"tileWidth"), 28, 80);
    g_pictureSize = (std::clamp)(Wh_GetIntSetting(L"pictureSize"), 20, 64);
    g_hotkeyEnabled = Wh_GetIntSetting(L"enableHotkey") != 0;
    LoadLanguageSetting();
    LoadThemeSetting();
    LoadBorderSetting();
    g_setupMsg = RegisterWindowMessageW(L"WhUserTileV04.Setup");
    g_cleanupMsg = RegisterWindowMessageW(L"WhUserTileV04.Cleanup");
    if (!g_setupMsg || !g_cleanupMsg) return FALSE;
    if (g_nativeWin11Mode) InitXamlTaskbarSymbols();
    else if (g_useExplorerPatcher) EnsureExplorerPatcherSymbols();
    else if (g_win10Mode) InitTaskbarSymbols();
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_ready) return FALSE;
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    if (!g_thread) { CloseHandle(g_ready); g_ready = nullptr; return FALSE; }
    // Wait until the UI thread finishes its setup
    WaitForSingleObject(g_ready, INFINITE);
    if (!g_initialized) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread); g_thread = nullptr;
        CloseHandle(g_ready); g_ready = nullptr;
        return FALSE;
    }
    return TRUE;
}
void Wh_ModAfterInit() {
    g_hooksApplied = true;
    if (g_flyout) PostMessageW(g_flyout, WM_TIMER, kDiscoveryTimer, 0);
}
BOOL Wh_ModSettingsChanged(BOOL* reload) { *reload = TRUE; return TRUE; }
void Wh_ModBeforeUninit() {
    g_stopping = true;
    if (g_nativeWin11Mode) RemoveWin11Tile();
    // Stop discovery first, then remove the subclass and tile window
    DetachTaskbar();
}
void Wh_ModUninit() {
    g_stopping = true;
    if (g_nativeWin11Mode) RemoveWin11Tile();
    DetachTaskbar();
    if (g_flyout) PostMessageW(g_flyout, WM_CLOSE, 0, 0);
    if (g_thread) { WaitForSingleObject(g_thread, INFINITE); CloseHandle(g_thread); g_thread = nullptr; }
    if (g_ready) { CloseHandle(g_ready); g_ready = nullptr; }
}
