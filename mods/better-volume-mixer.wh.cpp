// ==WindhawkMod==
// @id              better-volume-mixer
// @name            Better Volume Mixer
// @description     Quickly control master and per-app volume from the system tray
// @version         1.0
// @author          0Allu
// @github          https://github.com/0Allu
// @homepage        https://github.com/0Allu/better-volume-mixer
// @include         windhawk.exe
// @compilerOptions -lole32 -lshell32 -lgdi32 -luser32 -ldwmapi -ladvapi32 -lmsimg32 -loleaut32 -lgdiplus
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Better Volume Mixer

A simple volume mixer for Windows 11. Control your overall volume and individual
apps from one small window.

![Better Volume Mixer demo](https://raw.githubusercontent.com/0Allu/better-volume-mixer/main/assets/better-volume-mixer-demo.png)

Click the speaker icon in the system tray to open or close the mixer. If it is
hidden, open the tray overflow menu and drag the icon onto the taskbar.

* Adjust each app's volume with its slider or mute button.
* Scroll over a slider to change the volume with your mouse wheel.
* Click the pin beside an app to keep it at the top.
* Use Previous and Next when there are more apps than fit on one page.

Choose your theme, background transparency, animations, and apps per page in
the mod settings. If an app is missing, keep the mixer open and play some audio
in it. Apps on other output devices are remembered after playing while the mixer
is open, until their audio session ends.

## Hide the Windows volume icon

To avoid having two volume icons, install
[Taskbar tray system icon tweaks](https://windhawk.net/mods/taskbar-tray-system-icon-tweaks)
and enable **Hide volume icon** in its settings. This hides the Windows 11
volume icon while keeping Better Volume Mixer's icon visible.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- closeWhenFocusIsLost: true
  $name: Close when focus is lost
  $description: Automatically close the mixer when you click somewhere else.
- maxVisibleApps: "7"
  $name: Applications per page
  $description: Maximum app rows per page, limited by available display space. Master volume does not count toward this limit.
  $options:
  - "1": "1"
  - "2": "2"
  - "3": "3"
  - "4": "4"
  - "5": "5"
  - "6": "6"
  - "7": "7"
  - "8": "8"
  - "9": "9"
  - "10": "10"
  - "11": "11"
  - "12": "12"
  - "13": "13"
  - "14": "14"
  - "15": "15"
- volumeStep: 2
  $name: Mouse-wheel volume step
  $description: Percentage points per mouse-wheel step over a slider. Valid range is 1 to 20; default is 2.
- theme: system
  $name: Theme
  $description: Choose the mixer appearance or follow the current Windows theme.
  $options:
  - system: Follow Windows
  - dark: Dark
  - light: Light
- backgroundOpacity: 85
  $name: Background opacity
  $description: Background opacity from 0 to 100 percent. Text and controls remain opaque. Blur is applied where supported by Windows.
- animations: true
  $name: Enable animations
  $description: Animate opening, hover effects, and volume changes. The Windows animation preference is also respected.
- showAllSessions: false
  $name: Show all audio sessions
  $description: Also show unused or idle audio sessions on other playback devices. Most users can leave this off.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <dwmapi.h>
#include <propsys.h>
#include <objidl.h>
#include <gdiplus.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Some Windhawk/MinGW headers only forward-declare this Core Audio interface.
// Define the standard COM method table, in ABI order, when it is missing.
// Keep this at global scope so it completes the header's forward declaration.
#ifndef __IAudioMeterInformation_INTERFACE_DEFINED__
#define __IAudioMeterInformation_INTERFACE_DEFINED__
MIDL_INTERFACE("c02216f6-8c67-4b5b-9d00-d008e73e0064")
IAudioMeterInformation : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(
        UINT count, float* peaks) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IAudioMeterInformation, 0xc02216f6, 0x8c67, 0x4b5b,
                0x9d, 0x00, 0xd0, 0x08, 0xe7, 0x3e, 0x00, 0x64)
#endif
#endif

namespace {

// Device friendly-name property, defined locally to avoid an external SDK
// PKEY_Device_FriendlyName symbol dependency in Windhawk's toolchain.
constexpr PROPERTYKEY kDeviceFriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd,
     {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};

#ifndef NOTIFYICON_VERSION_4
#define NOTIFYICON_VERSION_4 4
#endif
#ifndef NIF_GUID
#define NIF_GUID 0x00000020
#endif
#ifndef NIF_SHOWTIP
#define NIF_SHOWTIP 0x00000080
#endif
#ifndef NIN_SELECT
#define NIN_SELECT (WM_USER + 0)
#endif
#ifndef NINF_KEY
#define NINF_KEY 0x1
#endif
#ifndef NIN_KEYSELECT
#define NIN_KEYSELECT (NIN_SELECT | NINF_KEY)
#endif

constexpr UINT WM_APP_TRAY = WM_APP + 1;
constexpr UINT WM_APP_RELOAD_SETTINGS = WM_APP + 2;
constexpr UINT WM_APP_SHOW_MIXER = WM_APP + 3;
constexpr UINT_PTR TIMER_METERS = 1;
constexpr UINT_PTR TIMER_REFRESH = 2;
constexpr UINT_PTR TIMER_CHECK_FOCUS = 3;
// Also use a distinct 16-bit numeric ID for callback/legacy compatibility.
constexpr UINT TRAY_ICON_ID = 0x4D58;
// Generated once for this mod. Never regenerate it on startup or on updates.
constexpr GUID MIXER_TRAY_GUID = {
    0x6bb7d04a, 0xc78a, 0x4a7a,
    {0xac, 0x7a, 0x81, 0x7b, 0x68, 0x0f, 0xfd, 0x51}};

constexpr UINT MENU_OPEN = 1;
constexpr UINT MENU_REFRESH = 2;
constexpr UINT MENU_SOUND_SETTINGS = 3;
constexpr UINT MENU_WINDOWS_MIXER = 4;

constexpr int DRAG_NONE = -2;
constexpr int DRAG_MASTER = -1;

struct Settings {
    std::wstring theme = L"system";
    bool showAllSessions = false;
    bool closeWhenFocusIsLost = true;
    bool animations = true;
    int backgroundOpacity = 85;
    int maxVisibleApps = 7;
    int volumeStep = 2;
};

struct Theme {
    COLORREF background;
    COLORREF panel;
    COLORREF hover;
    COLORREF text;
    COLORREF secondaryText;
    COLORREF track;
    COLORREF accent;
    COLORREF muted;
    COLORREF divider;
};

struct AppSession {
    DWORD processId = 0;
    std::wstring name;
    std::wstring executablePath;
    std::wstring pinKey;
    bool pinned = false;
    bool active = false;
    float volume = 1.0f;
    bool muted = false;
    HICON icon = nullptr;
    std::vector<ISimpleAudioVolume*> volumeControls;
    std::vector<IAudioMeterInformation*> meters;

    AppSession() = default;
    AppSession(const AppSession&) = delete;
    AppSession& operator=(const AppSession&) = delete;

    AppSession(AppSession&& other) noexcept {
        *this = std::move(other);
    }

    AppSession& operator=(AppSession&& other) noexcept {
        if (this != &other) {
            Reset();
            processId = other.processId;
            name = std::move(other.name);
            executablePath = std::move(other.executablePath);
            pinKey = std::move(other.pinKey);
            pinned = other.pinned;
            active = other.active;
            volume = other.volume;
            muted = other.muted;
            icon = other.icon;
            volumeControls = std::move(other.volumeControls);
            meters = std::move(other.meters);
            other.icon = nullptr;
            other.volumeControls.clear();
            other.meters.clear();
        }
        return *this;
    }

    ~AppSession() {
        Reset();
    }

    void Reset() {
        for (auto* control : volumeControls) {
            control->Release();
        }
        volumeControls.clear();
        for (auto* meter : meters) {
            meter->Release();
        }
        meters.clear();
        if (icon) {
            DestroyIcon(icon);
            icon = nullptr;
        }
    }
};

Settings g_settings;
Theme g_theme;

HANDLE g_thread;
HANDLE g_windowReadyEvent;
std::atomic<HWND> g_hWnd;
UINT g_taskbarCreatedMessage;

HICON g_trayIcon;
HICON g_masterIcon;
// Cache source icons, but hand each row an owned CopyIcon so refresh/cleanup
// cannot invalidate an icon that is still being drawn.
std::unordered_map<std::wstring, HICON> g_iconCache;
HFONT g_titleFont;
HFONT g_bodyFont;
HFONT g_smallFont;
HFONT g_symbolFont;
ULONG_PTR g_gdiplusToken;
Gdiplus::Font* g_titleTypography;
Gdiplus::Font* g_bodyTypography;
Gdiplus::Font* g_smallTypography;

struct RowVisual {
    float hover = 0.0f;
    float volume = 0.0f;
    float peak = 0.0f;
    bool initialized = false;
};
std::unordered_map<std::wstring, RowVisual> g_rowVisuals;
bool g_motionEnabled;
bool g_closeHovered;
int g_hoverPageButton = -1;
float g_closeHoverAmount;
float g_frameSeconds = 0.016f;
ULONGLONG g_lastPaintTime;
ULONGLONG g_openTime;
int g_paintMatte = -1; // -1: opaque, 0/1: black/white alpha-recovery pass.
bool g_transparencyFailed;
float g_masterPaintVolume;
bool g_masterPaintMuted;
bool g_frameAnimating;
bool g_frameHasAudio;
UINT g_paintTimerInterval;

IAudioEndpointVolume* g_endpointVolume;
std::wstring g_endpointName;
// COM interfaces and icons must never be released by a process-exit destructor.
// Refresh clears the contents; the UI thread resets storage on controlled exit.
[[clang::no_destroy]] std::optional<std::vector<AppSession>> g_apps{std::in_place};
// Session instance IDs are unique across app instances. Do not remember PIDs:
// they can be reused, and one process can have sessions on multiple outputs.
std::unordered_set<std::wstring> g_activatedSessions;
std::unordered_set<std::wstring> g_liveSessions;

int g_dpi = 96;
int g_scrollRow;
int g_availableAppRows = 15;
int g_hoverRow = DRAG_NONE;
int g_dragRow = DRAG_NONE;
bool g_mouseTracking;
bool g_audioAvailable;
bool g_trayVersion4;
bool g_trayUsesGuid = true;
bool g_showRequestPending;
bool g_closeRequestPending;
bool g_trayPressKnown;
bool g_trayPressWasVisible;
bool g_showingMixer;

int Scale(int value) {
    return MulDiv(value, g_dpi, 96);
}

int ClampInt(int value, int minimum, int maximum) {
    return value < minimum ? minimum : (value > maximum ? maximum : value);
}

float ClampVolume(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

void LoadSettings() {
    WindhawkUtils::StringSetting theme =
        WindhawkUtils::StringSetting::make(L"theme");
    g_settings.theme = theme.get();
    if (g_settings.theme != L"dark" && g_settings.theme != L"light") {
        g_settings.theme = L"system";
    }

    g_settings.showAllSessions =
        Wh_GetIntSetting(L"showAllSessions") != 0;
    g_settings.closeWhenFocusIsLost =
        Wh_GetIntSetting(L"closeWhenFocusIsLost") != 0;
    g_settings.animations = Wh_GetIntSetting(L"animations") != 0;
    g_settings.backgroundOpacity = ClampInt(Wh_GetIntSetting(L"backgroundOpacity"), 0, 100);
    WindhawkUtils::StringSetting pageSize =
        WindhawkUtils::StringSetting::make(L"maxVisibleApps");
    g_settings.maxVisibleApps = ClampInt(_wtoi(pageSize.get()), 1, 15);
    g_settings.volumeStep =
        ClampInt(Wh_GetIntSetting(L"volumeStep"), 1, 20);
}

enum class AccentState : int {
    Disabled = 0,
    BlurBehind = 3,
};

struct AccentPolicy {
    AccentState state;
    DWORD flags;
    DWORD gradientColor;
    DWORD animationId;
};

struct WindowCompositionAttributeData {
    int attribute;
    void* data;
    SIZE_T size;
};

void ApplyBackdropBlur(HWND hWnd) {
    using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(
        HWND, const WindowCompositionAttributeData*);

    static auto setWindowCompositionAttribute =
        reinterpret_cast<SetWindowCompositionAttribute_t>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"),
                           "SetWindowCompositionAttribute"));

    if (!setWindowCompositionAttribute) {
        return;
    }

    bool enable = g_settings.backgroundOpacity < 100 && !g_transparencyFailed;
    AccentPolicy policy{};
    policy.state = enable ? AccentState::BlurBehind : AccentState::Disabled;

    // WCA_ACCENT_POLICY. Keep the backdrop untinted here because the mixer
    // already paints its own theme-aware translucent background on top.
    WindowCompositionAttributeData data{19, &policy, sizeof(policy)};
    if (!setWindowCompositionAttribute(hWnd, &data) && enable) {
        Wh_Log(L"Mixer: backdrop blur unavailable; using transparency without blur");
    }
}

void ApplyTransparencyStyle(HWND hWnd) {
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    bool transparent = g_settings.backgroundOpacity < 100 && !g_transparencyFailed;
    LONG_PTR next = transparent ? style | WS_EX_LAYERED : style & ~WS_EX_LAYERED;
    if (style != next) {
        SetLastError(0);
        if (!SetWindowLongPtrW(hWnd, GWL_EXSTYLE, next) && GetLastError()) {
            g_transparencyFailed = true;
            Wh_Log(L"Mixer: could not enable background transparency");
        }
        SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
    ApplyBackdropBlur(hWnd);
}

void SetPaintTimer(HWND hWnd, UINT interval) {
    if (g_paintTimerInterval != interval &&
        SetTimer(hWnd, TIMER_METERS, interval, nullptr)) {
        g_paintTimerInterval = interval;
    }
}

UINT ChoosePaintInterval(bool motion, bool animating, bool audio) {
    if (motion && animating) return 16;
    if (audio) return motion ? 33 : 80;
    return 200;
}

void UpdatePaintTimer(HWND hWnd) {
    if (!IsWindowVisible(hWnd)) return;
    SetPaintTimer(hWnd, ChoosePaintInterval(g_motionEnabled,
        g_frameAnimating || g_openTime || g_dragRow != DRAG_NONE, g_frameHasAudio));
}

void UpdateMotionPreference(HWND hWnd) {
    BOOL enabled = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0);
    g_motionEnabled = g_settings.animations && enabled;
    if (!g_motionEnabled) {
        g_openTime = 0;
    }
    if (IsWindowVisible(hWnd)) {
        SetPaintTimer(hWnd, g_motionEnabled ? 16 : 80);
    }
}

bool WindowsUsesLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    LSTATUS result = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return result == ERROR_SUCCESS && value != 0;
}

void ClearTextCache();

void UpdateTheme() {
    ClearTextCache();
    bool light = g_settings.theme == L"light" ||
                 (g_settings.theme == L"system" && WindowsUsesLightTheme());

    DWORD colorization = 0;
    BOOL opaque = FALSE;
    COLORREF accent = RGB(0, 120, 212);
    if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) {
        accent = RGB((colorization >> 16) & 0xFF,
                     (colorization >> 8) & 0xFF,
                     colorization & 0xFF);
    }

    if (light) {
        g_theme = {RGB(247, 248, 250), RGB(255, 255, 255), RGB(235, 238, 242),
                   RGB(27, 31, 38), RGB(98, 105, 117), RGB(218, 223, 230),
                   accent, RGB(115, 122, 134), RGB(228, 231, 236)};
    } else {
        g_theme = {RGB(27, 29, 34), RGB(38, 41, 48), RGB(45, 49, 57),
                   RGB(242, 244, 248), RGB(159, 168, 183), RGB(65, 72, 84),
                   accent, RGB(138, 146, 159), RGB(48, 53, 62)};
    }
}

void DeleteFonts() {
    ClearTextCache();
    delete g_titleTypography;
    delete g_bodyTypography;
    delete g_smallTypography;
    g_titleTypography = g_bodyTypography = g_smallTypography = nullptr;
    if (g_symbolFont) {
        DeleteObject(g_symbolFont);
        g_symbolFont = nullptr;
    }
    if (g_titleFont) {
        DeleteObject(g_titleFont);
        g_titleFont = nullptr;
    }
    if (g_bodyFont) {
        DeleteObject(g_bodyFont);
        g_bodyFont = nullptr;
    }
    if (g_smallFont) {
        DeleteObject(g_smallFont);
        g_smallFont = nullptr;
    }
}

Gdiplus::Font* CreateTypographyFont(PCWSTR preferredFamily, float pixels) {
    Gdiplus::FontFamily family(preferredFamily);
    Gdiplus::Font* font = nullptr;
    if (family.GetLastStatus() == Gdiplus::Ok &&
        family.IsStyleAvailable(Gdiplus::FontStyleRegular)) {
        font = new Gdiplus::Font(&family, pixels, Gdiplus::FontStyleRegular,
                                 Gdiplus::UnitPixel);
        if (font->GetLastStatus() == Gdiplus::Ok) return font;
        delete font;
    }
    // Some Windows/GDI+ combinations cannot resolve the variable font family.
    font = new Gdiplus::Font(L"Segoe UI", pixels, Gdiplus::FontStyleRegular,
                             Gdiplus::UnitPixel);
    if (font->GetLastStatus() == Gdiplus::Ok) return font;
    delete font;
    return nullptr; // DrawLabel retains a GDI fallback.
}

void CreateFonts(HWND hWnd) {
    DeleteFonts();
    g_dpi = static_cast<int>(GetDpiForWindow(hWnd));
    if (!g_dpi) {
        g_dpi = 96;
    }

    g_titleFont = CreateFontW(
        -MulDiv(15, g_dpi, 72), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_bodyFont = CreateFontW(
        -MulDiv(10, g_dpi, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_smallFont = CreateFontW(
        -MulDiv(9, g_dpi, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_symbolFont = CreateFontW(
        -MulDiv(13, g_dpi, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe Fluent Icons");
    if (g_gdiplusToken) {
        // Fractional device-pixel sizes and typographic advances avoid rounding
        // each character's spacing to GDI's integer-width font metrics.
        float density = g_dpi / 96.0f;
        g_titleTypography = CreateTypographyFont(L"Segoe UI Semibold", 20.0f * density);
        g_bodyTypography = CreateTypographyFont(L"Segoe UI Variable Text", 14.0f * density);
        g_smallTypography = CreateTypographyFont(L"Segoe UI Variable Text", 12.0f * density);
    }
}

std::wstring BaseNameWithoutExtension(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    std::wstring name = slash == std::wstring::npos ? path : path.substr(slash + 1);
    size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        name.resize(dot);
    }
    return name;
}

std::wstring GetProcessPath(DWORD processId) {
    if (!processId) {
        return {};
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                 processId);
    if (!process) {
        return {};
    }

    std::vector<WCHAR> buffer(32768);
    DWORD size = static_cast<DWORD>(buffer.size());
    std::wstring result;
    if (QueryFullProcessImageNameW(process, 0, buffer.data(), &size)) {
        result.assign(buffer.data(), size);
    }
    CloseHandle(process);
    return result;
}

bool IsAudioEngineSession(const std::wstring& processPath, bool systemSounds) {
    if (systemSounds || processPath.empty()) {
        return false;
    }
    size_t slash = processPath.find_last_of(L"\\/");
    const wchar_t* filename = processPath.c_str() +
        (slash == std::wstring::npos ? 0 : slash + 1);
    return _wcsicmp(filename, L"audiodg.exe") == 0;
}

void ClearIconCache() {
    for (const auto& entry : g_iconCache) {
        if (entry.second) {
            DestroyIcon(entry.second);
        }
    }
    g_iconCache.clear();
}

HICON ExtractSizedIcon(const std::wstring& path, int pixels) {
    if (path.empty()) {
        return nullptr;
    }

    HICON icon = nullptr;
    HRESULT hr = SHDefExtractIconW(path.c_str(), 0, 0, &icon, nullptr,
                                   MAKELONG(pixels, 0));
    if (SUCCEEDED(hr) && icon) {
        return icon;
    }
    if (icon) {
        DestroyIcon(icon);
    }

    // Shell associations can provide an icon even when a file has no embedded
    // icon resources. Prefer the large version, never the 16px small version.
    SHFILEINFOW info = {};
    if (SHGetFileInfoW(path.c_str(), FILE_ATTRIBUTE_NORMAL, &info, sizeof(info),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        return info.hIcon;
    }
    return nullptr;
}

HICON GetExecutableIcon(const std::wstring& path) {
    if (path.empty()) {
        return nullptr;
    }
    int pixels = Scale(24);
    std::wstring key = std::to_wstring(pixels) + L":" + path;
    auto found = g_iconCache.find(key);
    if (found != g_iconCache.end()) {
        return found->second ? CopyIcon(found->second) : nullptr;
    }
    if (g_iconCache.size() >= 256) {
        ClearIconCache();
    }
    HICON icon = ExtractSizedIcon(path, pixels);
    g_iconCache.emplace(std::move(key), icon);
    return icon ? CopyIcon(icon) : nullptr;
}

std::wstring GetSessionDisplayName(IAudioSessionControl* control,
                                   DWORD processId,
                                   bool systemSounds,
                                   std::wstring* processPath) {
    if (systemSounds) {
        return L"System sounds";
    }

    LPWSTR displayName = nullptr;
    if (SUCCEEDED(control->GetDisplayName(&displayName)) && displayName) {
        std::wstring value = displayName;
        CoTaskMemFree(displayName);
        if (!value.empty() && value.front() != L'@') {
            return value;
        }
    }

    if (processPath->empty()) {
        *processPath = GetProcessPath(processId);
    }
    std::wstring name = BaseNameWithoutExtension(*processPath);
    return name.empty() ? L"Unknown application" : name;
}

std::wstring ReadEndpointName(IMMDevice* device) {
    IPropertyStore* store = nullptr;
    if (FAILED(device->OpenPropertyStore(STGM_READ, &store)) || !store) {
        return L"Default output";
    }

    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring result = L"Default output";
    if (SUCCEEDED(store->GetValue(kDeviceFriendlyName, &value)) &&
        value.vt == VT_LPWSTR && value.pwszVal) {
        result = value.pwszVal;
    }
    PropVariantClear(&value);
    store->Release();
    return result;
}

void ReleaseAudioData() {
    g_apps->clear();
    if (g_endpointVolume) {
        g_endpointVolume->Release();
        g_endpointVolume = nullptr;
    }
    g_endpointName.clear();
    g_audioAvailable = false;
}

bool ShouldIncludeSession(AudioSessionState state, const std::wstring& id,
                          bool isDefault) {
    if (state == AudioSessionStateExpired) {
        return false;
    }
    if (!id.empty()) {
        g_liveSessions.insert(id);
        if (state == AudioSessionStateActive) {
            g_activatedSessions.insert(id);
        }
    }
    // Include the full default-output list. Only additional outputs need
    // playback history, preventing their unused idle sessions from flooding it.
    // Playback state also works for muted apps, allowing users to unmute them.
    // If the ID query failed, show active sessions without remembering them.
    return isDefault || g_settings.showAllSessions ||
           state == AudioSessionStateActive ||
           (!id.empty() && g_activatedSessions.count(id) != 0);
}

void PrunePlaybackHistory() {
    for (auto it = g_activatedSessions.begin(); it != g_activatedSessions.end();) {
        if (!g_liveSessions.count(*it)) {
            it = g_activatedSessions.erase(it);
        } else {
            ++it;
        }
    }
    g_liveSessions.clear();
}

std::wstring SourcePinKey(std::wstring path, const std::wstring& name,
                          const std::wstring& endpoint, bool systemSounds) {
    for (auto& c : path) c = static_cast<wchar_t>(std::towlower(c));
    std::wstring identity = systemSounds ? L"system" :
        (path.empty() ? L"name:" + name : L"exe:" + path);
    identity += L"|output:" + endpoint;
    unsigned long long hash = 14695981039346656037ULL;
    for (wchar_t c : identity) {
        hash ^= static_cast<unsigned short>(c);
        hash *= 1099511628211ULL;
    }
    return L"sourcePin_v1_" + std::to_wstring(hash);
}

bool AppSortOrder(const AppSession& left, const AppSession& right) {
    if (left.pinned != right.pinned) return left.pinned > right.pinned;
    // Playback changes should not shuffle pinned sources around.
    if (!left.pinned && left.active != right.active) return left.active > right.active;
    int nameOrder = _wcsicmp(left.name.c_str(), right.name.c_str());
    return nameOrder ? nameOrder < 0 : left.pinKey < right.pinKey;
}

void ToggleSourcePin(HWND window, int row) {
    if (row < 0 || row >= static_cast<int>(g_apps->size())) return;
    std::wstring key = (*g_apps)[row].pinKey;
    bool pinned = !(*g_apps)[row].pinned;
    BOOL saved = pinned ? Wh_SetIntValue(key.c_str(), 1)
                        : Wh_DeleteValue(key.c_str());
    if (!saved) {
        Wh_Log(L"Mixer: could not save source pin");
        return;
    }
    for (auto& app : *g_apps) if (app.pinKey == key) app.pinned = pinned;
    std::stable_sort(g_apps->begin(), g_apps->end(), AppSortOrder);
    g_scrollRow = 0;
    g_hoverRow = DRAG_NONE;
    InvalidateRect(window, nullptr, FALSE);
}

void AppendDeviceSessions(IMMDevice* device, bool isDefault, bool diagnostics) {
    std::wstring endpointName = ReadEndpointName(device);
    std::wstring endpointId = endpointName;
    LPWSTR rawEndpointId = nullptr;
    if (SUCCEEDED(device->GetId(&rawEndpointId)) && rawEndpointId) endpointId = rawEndpointId;
    if (rawEndpointId) CoTaskMemFree(rawEndpointId);
    IAudioSessionManager2* manager = nullptr;
    HRESULT hr = device->Activate(__uuidof(IAudioSessionManager2),
                                  CLSCTX_INPROC_SERVER, nullptr,
                                  reinterpret_cast<void**>(&manager));
    if (FAILED(hr) || !manager) {
        if (diagnostics) {
            Wh_Log(L"Mixer: session manager failed for '%s': 0x%08lX",
                   endpointName.c_str(), static_cast<unsigned long>(hr));
        }
        return;
    }

    IAudioSessionEnumerator* sessions = nullptr;
    hr = manager->GetSessionEnumerator(&sessions);
    manager->Release();
    if (FAILED(hr) || !sessions) {
        if (diagnostics) {
            Wh_Log(L"Mixer: session enumeration failed for '%s': 0x%08lX",
                   endpointName.c_str(), static_cast<unsigned long>(hr));
        }
        return;
    }

    // This map is scoped to a single device. Do not merge volume controls
    // across outputs, or merge unknown-PID sessions into System sounds.
    std::unordered_map<std::wstring, size_t> groupByProcess;
    int count = 0;
    hr = sessions->GetCount(&count);
    if (diagnostics) {
        Wh_Log(L"Mixer: output '%s', default=%d, sessions=%d, result=0x%08lX",
               endpointName.c_str(), isDefault, count,
               static_cast<unsigned long>(hr));
    }
    for (int i = 0; i < count; i++) {
        IAudioSessionControl* control = nullptr;
        if (FAILED(sessions->GetSession(i, &control)) || !control) {
            continue;
        }

        AudioSessionState state = AudioSessionStateExpired;
        control->GetState(&state);
        if (state == AudioSessionStateExpired) {
            control->Release();
            continue;
        }

        IAudioSessionControl2* control2 = nullptr;
        ISimpleAudioVolume* volume = nullptr;
        if (FAILED(control->QueryInterface(__uuidof(IAudioSessionControl2),
                                           reinterpret_cast<void**>(&control2))) ||
            FAILED(control->QueryInterface(__uuidof(ISimpleAudioVolume),
                                           reinterpret_cast<void**>(&volume))) ||
            !control2 || !volume) {
            if (control2) {
                control2->Release();
            }
            if (volume) {
                volume->Release();
            }
            control->Release();
            continue;
        }

        DWORD processId = 0;
        HRESULT processResult = control2->GetProcessId(&processId);
        bool systemSounds = control2->IsSystemSoundsSession() == S_OK;
        std::wstring processPath = SUCCEEDED(processResult)
            ? GetProcessPath(processId) : std::wstring();
        if (IsAudioEngineSession(processPath, systemSounds)) {
            if (diagnostics) {
                Wh_Log(L"Mixer: hiding audio engine session, PID=%lu, output='%s'",
                       processId, endpointName.c_str());
            }
            volume->Release();
            control2->Release();
            control->Release();
            continue;
        }

        std::wstring displayName = GetSessionDisplayName(control, processId,
                                                         systemSounds, &processPath);
        std::wstring pinKey = SourcePinKey(processPath, displayName, endpointId, systemSounds);
        bool pinned = Wh_GetIntValue(pinKey.c_str(), 0) != 0;
        LPWSTR rawInstanceId = nullptr;
        std::wstring instanceId;
        if (SUCCEEDED(control2->GetSessionInstanceIdentifier(&rawInstanceId)) &&
            rawInstanceId) {
            instanceId = rawInstanceId;
        }
        if (rawInstanceId) {
            CoTaskMemFree(rawInstanceId);
        }
        if (!ShouldIncludeSession(state, instanceId, isDefault) && !pinned) {
            volume->Release();
            control2->Release();
            control->Release();
            continue;
        }

        std::wstring groupKey;
        if (systemSounds) {
            groupKey = L"system";
        } else if (SUCCEEDED(processResult) && processId) {
            groupKey = L"pid:" + std::to_wstring(processId);
        } else {
            groupKey = L"session:" + std::to_wstring(i);
        }

        size_t groupIndex;
        auto existing = groupByProcess.find(groupKey);
        if (existing == groupByProcess.end()) {
            AppSession app;
            app.processId = processId;
            app.executablePath = std::move(processPath);
            app.pinKey = std::move(pinKey);
            app.pinned = pinned;
            app.active = state == AudioSessionStateActive;
            app.name = std::move(displayName);
            if (diagnostics) {
                Wh_Log(L"Mixer: app '%s', PID=%lu, active=%d, output='%s'",
                       app.name.c_str(), processId, app.active,
                       endpointName.c_str());
            }
            if (!isDefault) {
                app.name += L" [" + endpointName + L"]";
            }
            app.icon = GetExecutableIcon(app.executablePath);
            volume->GetMasterVolume(&app.volume);
            BOOL muted = FALSE;
            volume->GetMute(&muted);
            app.muted = muted != FALSE;
            app.volumeControls.push_back(volume);
            groupIndex = g_apps->size();
            groupByProcess[groupKey] = groupIndex;
            g_apps->push_back(std::move(app));
        } else {
            groupIndex = existing->second;
            (*g_apps)[groupIndex].active |= state == AudioSessionStateActive;
            (*g_apps)[groupIndex].volumeControls.push_back(volume);
        }

        IAudioMeterInformation* meter = nullptr;
        if (SUCCEEDED(control->QueryInterface(__uuidof(IAudioMeterInformation),
                                              reinterpret_cast<void**>(&meter))) &&
            meter) {
            (*g_apps)[groupIndex].meters.push_back(meter);
        }

        control2->Release();
        control->Release();
    }
    sessions->Release();
}

void ClampPageOffset();

void RefreshAudioSessions(HWND hWnd, bool diagnostics = false) {
    if (g_dragRow != DRAG_NONE) {
        return;
    }

    g_liveSessions.clear();
    ReleaseAudioData();
    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  __uuidof(IMMDeviceEnumerator),
                                  reinterpret_cast<void**>(&enumerator));
    if (FAILED(hr) || !enumerator) {
        if (diagnostics) {
            Wh_Log(L"Mixer: device enumeration failed: 0x%08lX",
                   static_cast<unsigned long>(hr));
        }
        ClampPageOffset();
        InvalidateRect(hWnd, nullptr, FALSE);
        return;
    }

    std::wstring defaultId;
    IMMDevice* defaultDevice = nullptr;
    if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(
            eRender, eMultimedia, &defaultDevice)) && defaultDevice) {
        LPWSTR id = nullptr;
        if (SUCCEEDED(defaultDevice->GetId(&id)) && id) {
            defaultId = id;
            CoTaskMemFree(id);
        }
        g_endpointName = ReadEndpointName(defaultDevice);
        defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
                                 CLSCTX_INPROC_SERVER, nullptr,
                                 reinterpret_cast<void**>(&g_endpointVolume));
    }

    IMMDeviceCollection* devices = nullptr;
    hr = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices);
    if (SUCCEEDED(hr) && devices) {
        UINT count = 0;
        devices->GetCount(&count);
        if (diagnostics) {
            Wh_Log(L"Mixer: scanning %u active playback outputs", count);
        }
        for (UINT i = 0; i < count; ++i) {
            IMMDevice* device = nullptr;
            if (FAILED(devices->Item(i, &device)) || !device) {
                continue;
            }
            LPWSTR id = nullptr;
            bool isDefault = false;
            if (SUCCEEDED(device->GetId(&id)) && id) {
                isDefault = !defaultId.empty() && defaultId == id;
                CoTaskMemFree(id);
            }
            AppendDeviceSessions(device, isDefault, diagnostics);
            device->Release();
        }
        devices->Release();
    } else {
        if (diagnostics) {
            Wh_Log(L"Mixer: playback collection failed: 0x%08lX",
                   static_cast<unsigned long>(hr));
        }
        // Preserve default-device functionality if collection enumeration fails.
        if (defaultDevice) {
            AppendDeviceSessions(defaultDevice, true, diagnostics);
        }
    }
    if (defaultDevice) {
        defaultDevice->Release();
    }
    enumerator->Release();

    PrunePlaybackHistory();

    std::stable_sort(g_apps->begin(), g_apps->end(), AppSortOrder);

    g_audioAvailable = g_endpointVolume != nullptr || !g_apps->empty();
    ClampPageOffset();
    InvalidateRect(hWnd, nullptr, FALSE);
}

float GetMasterVolume() {
    float value = 0.0f;
    if (g_endpointVolume) {
        g_endpointVolume->GetMasterVolumeLevelScalar(&value);
    }
    return value;
}

bool GetMasterMuted() {
    BOOL muted = FALSE;
    if (g_endpointVolume) {
        g_endpointVolume->GetMute(&muted);
    }
    return muted != FALSE;
}

float GetAppPeak(const AppSession& app) {
    float peak = 0.0f;
    for (auto* meter : app.meters) {
        float value = 0.0f;
        if (SUCCEEDED(meter->GetPeakValue(&value))) {
            peak = std::max(peak, value);
        }
    }
    return peak;
}

void SetRowVolume(int row, float value) {
    value = ClampVolume(value);
    if (row == DRAG_MASTER) {
        if (g_endpointVolume) {
            g_endpointVolume->SetMasterVolumeLevelScalar(value, nullptr);
            if (value > 0.0f) {
                g_endpointVolume->SetMute(FALSE, nullptr);
            }
        }
        return;
    }

    if (row < 0 || row >= static_cast<int>(g_apps->size())) {
        return;
    }
    AppSession& app = (*g_apps)[row];
    for (auto* control : app.volumeControls) {
        control->SetMasterVolume(value, nullptr);
        if (value > 0.0f) {
            control->SetMute(FALSE, nullptr);
        }
    }
    app.volume = value;
    if (value > 0.0f) {
        app.muted = false;
    }
}

void ToggleRowMute(int row) {
    if (row == DRAG_MASTER) {
        if (g_endpointVolume) {
            g_endpointVolume->SetMute(!GetMasterMuted(), nullptr);
        }
        return;
    }

    if (row < 0 || row >= static_cast<int>(g_apps->size())) {
        return;
    }
    AppSession& app = (*g_apps)[row];
    bool newMuted = !app.muted;
    for (auto* control : app.volumeControls) {
        control->SetMute(newMuted, nullptr);
    }
    app.muted = newMuted;
}

int HeaderHeight() {
    return Scale(76);
}

int RowHeight() {
    return Scale(68);
}

int RowTop(int visibleRow) {
    return HeaderHeight() + visibleRow * RowHeight() +
           (visibleRow > 0 ? Scale(30) : 0);
}

int PopupWidth() {
    return Scale(420);
}

int PageSize() {
    return std::min(g_settings.maxVisibleApps, g_availableAppRows);
}

int AppRowsForHeight(int availableHeight) {
    int fixedHeight = HeaderHeight() + RowHeight() + Scale(30 + 52);
    return ClampInt((availableHeight - fixedHeight) / RowHeight(), 1, 15);
}

int PageCount() {
    return std::max(1, (static_cast<int>(g_apps->size()) +
                       PageSize() - 1) / PageSize());
}

void ClampPageOffset() {
    int page = ClampInt(g_scrollRow / PageSize(), 0, PageCount() - 1);
    g_scrollRow = page * PageSize();
}

void ChangePage(HWND window, int delta) {
    // A captured slider always continues to refer to the same source.
    if (g_dragRow != DRAG_NONE || PageCount() <= 1) return;
    int pages = PageCount();
    int page = (g_scrollRow / PageSize() + delta % pages + pages) % pages;
    g_scrollRow = page * PageSize();
    g_hoverRow = DRAG_NONE;
    InvalidateRect(window, nullptr, FALSE);
}

int VisibleAppCount() {
    return std::min(std::max(0, static_cast<int>(g_apps->size()) - g_scrollRow),
                    PageSize());
}

int PopupHeight() {
    // Keep navigation stationary on the last, partially filled page.
    int slots = std::max(1, std::min(static_cast<int>(g_apps->size()),
                                   PageSize()));
    return HeaderHeight() + RowHeight() * (1 + slots) + Scale(30) +
           Scale(PageCount() > 1 ? 52 : 12);
}

RECT PageButtonRect(bool next, const RECT& client) {
    int left = next ? client.right - Scale(120) : Scale(20);
    return {left, client.bottom - Scale(44), left + Scale(100),
            client.bottom - Scale(12)};
}

RECT SliderRectForRow(int visibleRow, int clientWidth) {
    int top = RowTop(visibleRow);
    return {Scale(68), top + Scale(44), clientWidth - Scale(78),
            top + Scale(48)};
}

RECT MuteRectForRow(int visibleRow, int clientWidth) {
    int top = RowTop(visibleRow);
    return {clientWidth - Scale(58), top + Scale(visibleRow ? 32 : 22),
            clientWidth - Scale(22), top + Scale(visibleRow ? 64 : 58)};
}

RECT PinRectForRow(int visibleRow, int clientWidth) {
    int top = RowTop(visibleRow);
    return {clientWidth - Scale(56), top + Scale(1),
            clientWidth - Scale(24), top + Scale(29)};
}

int RowFromPoint(POINT point) {
    if (point.y < HeaderHeight()) {
        return DRAG_NONE;
    }
    int relative = point.y - HeaderHeight();
    int visibleRow = 0;
    if (relative >= RowHeight()) {
        if (relative < RowHeight() + Scale(30)) {
            return DRAG_NONE;
        }
        visibleRow = 1 + (relative - RowHeight() - Scale(30)) / RowHeight();
    }
    if (visibleRow > VisibleAppCount()) {
        return DRAG_NONE;
    }
    if (visibleRow == 0) {
        return DRAG_MASTER;
    }
    int appIndex = g_scrollRow + visibleRow - 1;
    return appIndex >= 0 && appIndex < static_cast<int>(g_apps->size())
               ? appIndex
               : DRAG_NONE;
}

int VisibleRowForDataRow(int row) {
    return row == DRAG_MASTER ? 0 : row - g_scrollRow + 1;
}

void ApplyVolumeFromPoint(HWND hWnd, int row, int x) {
    RECT client;
    GetClientRect(hWnd, &client);
    RECT slider = SliderRectForRow(VisibleRowForDataRow(row), client.right);
    float value = static_cast<float>(x - slider.left) /
                  static_cast<float>(slider.right - slider.left);
    SetRowVolume(row, value);
    InvalidateRect(hWnd, nullptr, FALSE);
}

void FillSolidRect(HDC dc, const RECT& rect, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);
}

void DrawRoundedRect(HDC dc, const RECT& rect, int radius, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_NULL, 0, color);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

struct CachedLabel {
    std::wstring text;
    HFONT font = nullptr;
    UINT format = 0;
    COLORREF color = 0;
    int width = 0, height = 0;
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    ~CachedLabel() {
        if (dc && previous) SelectObject(dc, previous);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
    }
};
struct LabelCache {
    std::vector<std::unique_ptr<CachedLabel>> labels;
    size_t bytes = 0;
};
struct TextCaches {
    LabelCache general;
    LabelCache percentages;
};
// Explicitly reset on the UI thread before fonts/GDI+ are destroyed.
std::optional<TextCaches> g_textCache;
constexpr int TEXT_SUPERSAMPLE = 6;

void ClearTextCache() {
    GdiFlush();
    g_textCache.reset();
}

// Average the supersampled coverage mask, then construct premultiplied BGRA.
// The same coverage is used against both mattes, without colored ClearType fringes.
DWORD TextCoveragePixel(const BYTE* scan, int stride, int x, int y, COLORREF color) {
    unsigned sum = 0;
    for (int dy = 0; dy < TEXT_SUPERSAMPLE; ++dy) {
        const BYTE* row = scan + static_cast<std::ptrdiff_t>(
            y * TEXT_SUPERSAMPLE + dy) * stride;
        for (int dx = 0; dx < TEXT_SUPERSAMPLE; ++dx) {
            sum += row[(x * TEXT_SUPERSAMPLE + dx) * 4];
        }
    }
    constexpr unsigned samples = TEXT_SUPERSAMPLE * TEXT_SUPERSAMPLE;
    DWORD alpha = (sum + samples / 2) / samples;
    return (alpha << 24) |
        (((GetRValue(color) * alpha + 127) / 255) << 16) |
        (((GetGValue(color) * alpha + 127) / 255) << 8) |
        ((GetBValue(color) * alpha + 127) / 255);
}

bool DrawSmoothLabel(HDC dc, const std::wstring& text, const RECT& rect,
                     HFONT font, Gdiplus::Font* typography, COLORREF color, UINT format,
                     bool percentage) {
    int width = rect.right - rect.left, height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0 || text.empty()) return true;
    if (width > 4096 || height > 1024 || static_cast<size_t>(width) * height > 262144)
        return false;
    auto blend = [&](const CachedLabel& label) {
        BLENDFUNCTION operation{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        return AlphaBlend(dc, rect.left, rect.top, width, height,
                          label.dc, 0, 0, width, height, operation) != FALSE;
    };
    if (!g_textCache) g_textCache.emplace();
    // A volume drag must not evict application names from the general cache.
    LabelCache& cache = percentage ? g_textCache->percentages : g_textCache->general;
    for (const auto& label : cache.labels) {
        if (label->font == font && label->format == format && label->color == color &&
            label->width == width && label->height == height && label->text == text)
            return blend(*label);
    }

    // Layout stays in physical screen pixels. Only rasterization is enlarged;
    // app names, percentages and ellipses therefore retain their existing layout.
    Gdiplus::Bitmap mask(width * TEXT_SUPERSAMPLE, height * TEXT_SUPERSAMPLE,
                         PixelFormat32bppARGB);
    if (mask.GetLastStatus() != Gdiplus::Ok) return false;
    {
        Gdiplus::Graphics graphics(&mask);
        graphics.SetPageUnit(Gdiplus::UnitPixel);
        graphics.Clear(Gdiplus::Color(255, 0, 0, 0));
        graphics.ScaleTransform(static_cast<float>(TEXT_SUPERSAMPLE),
                                static_cast<float>(TEXT_SUPERSAMPLE));
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        Gdiplus::StringFormat layout(Gdiplus::StringFormat::GenericTypographic());
        layout.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox |
            ((format & DT_SINGLELINE) ? Gdiplus::StringFormatFlagsNoWrap : 0));
        layout.SetAlignment((format & DT_CENTER) ? Gdiplus::StringAlignmentCenter :
            (format & DT_RIGHT) ? Gdiplus::StringAlignmentFar : Gdiplus::StringAlignmentNear);
        layout.SetLineAlignment((format & DT_VCENTER) ? Gdiplus::StringAlignmentCenter :
            Gdiplus::StringAlignmentNear);
        layout.SetTrimming((format & DT_END_ELLIPSIS) ? Gdiplus::StringTrimmingEllipsisCharacter :
            Gdiplus::StringTrimmingNone);
        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
        Gdiplus::RectF bounds(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
        if (graphics.DrawString(text.c_str(), static_cast<int>(text.size()), typography,
                                bounds, &layout, &brush) != Gdiplus::Ok) return false;
        graphics.Flush(Gdiplus::FlushIntentionSync);
    }

    auto label = std::make_unique<CachedLabel>();
    label->text = text; label->font = font; label->format = format; label->color = color;
    label->width = width; label->height = height;
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    label->bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    label->dc = CreateCompatibleDC(nullptr);
    if (!label->bitmap || !bits || !label->dc) return false;
    HGDIOBJ previous = SelectObject(label->dc, label->bitmap);
    if (!previous || previous == HGDI_ERROR) return false;
    label->previous = previous;

    Gdiplus::Rect area(0, 0, width * TEXT_SUPERSAMPLE,
                       height * TEXT_SUPERSAMPLE);
    Gdiplus::BitmapData data{};
    if (mask.LockBits(&area, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB,
                      &data) != Gdiplus::Ok) return false;
    auto* pixels = static_cast<DWORD*>(bits);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            pixels[static_cast<size_t>(y) * width + x] = TextCoveragePixel(
                static_cast<const BYTE*>(data.Scan0), data.Stride, x, y, color);
        }
    }
    mask.UnlockBits(&data);

    size_t bytes = static_cast<size_t>(width) * height * sizeof(DWORD);
    GdiFlush();
    while (!cache.labels.empty() &&
           (cache.labels.size() >= 128 || cache.bytes + bytes > 8 * 1024 * 1024)) {
        const auto& oldest = cache.labels.front();
        cache.bytes -= static_cast<size_t>(oldest->width) * oldest->height * sizeof(DWORD);
        cache.labels.erase(cache.labels.begin());
    }
    cache.bytes += bytes;
    cache.labels.push_back(std::move(label));
    return blend(*cache.labels.back());
}

void DrawLabel(HDC dc, const std::wstring& text, RECT rect, HFONT font,
               COLORREF color, UINT format, bool percentage = false) {
    Gdiplus::Font* typography = font == g_titleFont ? g_titleTypography :
        font == g_bodyFont ? g_bodyTypography :
        font == g_smallFont ? g_smallTypography : nullptr;
    if (typography && typography->GetLastStatus() == Gdiplus::Ok &&
        DrawSmoothLabel(dc, text, rect, font, typography, color, format, percentage)) return;
    // Keep Fluent icon glyphs and a fallback path independent of GDI+.
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetTextColor(dc, color);
    SetBkMode(dc, TRANSPARENT);
    DrawTextW(dc, text.c_str(), -1, &rect, format | DT_NOPREFIX);
    SelectObject(dc, oldFont);
}

COLORREF MixColor(COLORREF from, COLORREF to, float amount) {
    amount = ClampVolume(amount);
    return RGB(
        static_cast<int>(GetRValue(from) + (GetRValue(to) - GetRValue(from)) * amount),
        static_cast<int>(GetGValue(from) + (GetGValue(to) - GetGValue(from)) * amount),
        static_cast<int>(GetBValue(from) + (GetBValue(to) - GetBValue(from)) * amount));
}

void DrawSurface(HDC dc, const RECT& rect, int radius, COLORREF color) {
    if (g_paintMatte >= 0) {
        color = MixColor(g_paintMatte ? RGB(255, 255, 255) : RGB(0, 0, 0),
                         color, g_settings.backgroundOpacity / 100.0f);
    }
    DrawRoundedRect(dc, rect, radius, color);
}

float EaseTowards(float current, float target, float speed) {
    if (!g_motionEnabled) {
        return target;
    }
    float next = current + (target - current) *
        (1.0f - std::exp(-speed * g_frameSeconds));
    return std::abs(target - next) < 0.001f ? target : next;
}

void DrawSlider(HDC dc, const RECT& rect, float volume, float peak,
                bool muted, float hover) {
    float shownVolume = ClampVolume(volume);

    int centerY = (rect.top + rect.bottom) / 2;
    int trackHeight = std::max(Scale(4), 2);
    int meterHeight = std::max(Scale(2), 1);
    int trackRadius = trackHeight / 2;
    int meterRadius = meterHeight / 2;

    RECT track = {
        rect.left,
        centerY - trackHeight / 2,
        rect.right,
        centerY + (trackHeight + 1) / 2
    };

    COLORREF inactiveTrack = MixColor(g_theme.track, g_theme.panel, 0.30f);
    COLORREF activeTrack = muted
        ? MixColor(g_theme.muted, g_theme.track, 0.70f)
        : g_theme.accent;

    DrawRoundedRect(dc, track, trackRadius, inactiveTrack);

    int thumbX = track.left + static_cast<int>(
        (track.right - track.left) * shownVolume);
    thumbX = ClampInt(thumbX, track.left, track.right);

    RECT fill = track;
    fill.right = thumbX;
    if (fill.right > fill.left) {
        DrawRoundedRect(dc, fill, trackRadius, activeTrack);
    }

    if (!muted && peak > 0.01f) {
        int peakX = track.left + static_cast<int>(
            (track.right - track.left) * ClampVolume(peak));
        RECT meter = {
            track.left,
            track.bottom + Scale(6),
            peakX,
            track.bottom + Scale(6) + meterHeight
        };
        if (meter.right > meter.left) {
            DrawRoundedRect(dc, meter, meterRadius,
                            MixColor(g_theme.accent, g_theme.track, 0.55f));
        }
    }

    int thumbRadius = Scale(5) + static_cast<int>(Scale(1) * hover);
    RECT thumb = {
        thumbX - thumbRadius,
        centerY - thumbRadius,
        thumbX + thumbRadius,
        centerY + thumbRadius
    };

    COLORREF thumbColor = muted
        ? MixColor(g_theme.muted, g_theme.track, 0.55f)
        : g_theme.accent;

    DrawRoundedRect(dc, thumb, thumbRadius * 2, thumbColor);
}

void DrawMuteButton(HDC dc, int visibleRow, int clientWidth, bool muted) {
    RECT button = MuteRectForRow(visibleRow, clientWidth);
    if (muted) {
        DrawSurface(dc, button, Scale(8), g_theme.hover);
    }
    DrawLabel(dc, muted ? L"\uE74F" : L"\uE767", button, g_symbolFont,
              muted ? g_theme.accent : g_theme.secondaryText,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DrawAudioRow(HDC dc, int visibleRow, int dataRow, int clientWidth) {
    int top = RowTop(visibleRow);
    bool master = dataRow == DRAG_MASTER;
    std::wstring name;
    float volume;
    float peak = 0.0f;
    bool muted;
    HICON icon = nullptr;

    if (master) {
        name = g_endpointVolume ? L"Master volume" : L"Output unavailable";
        if (g_paintMatte != 1) {
            g_masterPaintVolume = GetMasterVolume();
            g_masterPaintMuted = GetMasterMuted();
        }
        volume = g_masterPaintVolume;
        muted = g_masterPaintMuted;
        icon = g_masterIcon;
    } else {
        const AppSession& app = (*g_apps)[dataRow];
        name = app.name;
        volume = app.volume;
        muted = app.muted;
        if (g_paintMatte != 1) peak = GetAppPeak(app);
        icon = app.icon;
    }

    std::wstring visualKey = master ? L"master" :
        L"app:" + std::to_wstring((*g_apps)[dataRow].processId) + L":" + name;
    RowVisual& visual = g_rowVisuals[visualKey];
    if (!visual.initialized) {
        visual.volume = volume;
        visual.initialized = true;
    }
    if (g_paintMatte != 1) {
        float targetHover = g_hoverRow == dataRow ? 1.0f : 0.0f;
        g_frameAnimating |= std::abs(visual.hover - targetHover) >= 0.001f ||
                            std::abs(visual.volume - volume) >= 0.001f;
        g_frameHasAudio |= peak > 0.005f || visual.peak > 0.005f;
        visual.hover = EaseTowards(visual.hover, g_hoverRow == dataRow ? 1.0f : 0.0f, 18.0f);
        visual.volume = g_dragRow == dataRow ? volume : EaseTowards(visual.volume, volume, 22.0f);
        visual.peak = EaseTowards(visual.peak, peak, peak > visual.peak ? 35.0f : 8.0f);
    }
    RECT rowRect = {Scale(12), top, clientWidth - Scale(12), top + RowHeight() - Scale(2)};
    COLORREF base = master ? g_theme.panel : g_theme.background;
    DrawSurface(dc, rowRect, Scale(16), MixColor(base, g_theme.hover, visual.hover));

    RECT iconTile = {Scale(22), top + Scale(13), Scale(58), top + Scale(49)};
    DrawSurface(dc, iconTile, Scale(10),
                    master ? g_theme.background : g_theme.panel);
    if (icon) {
        DrawIconEx(dc, Scale(28), top + Scale(19), icon, Scale(24), Scale(24),
                   0, nullptr, DI_NORMAL);
    } else {
        DrawLabel(dc, L"\uE8D6", iconTile, g_symbolFont, g_theme.secondaryText,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    RECT nameRect = {Scale(68), top + Scale(9), clientWidth - Scale(120),
                     top + Scale(33)};
    DrawLabel(dc, name, nameRect, g_bodyFont, g_theme.text,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    WCHAR percent[16];
    swprintf_s(percent, L"%d%%",
               static_cast<int>(volume * 100.0f + 0.5f));
    RECT percentRect = {clientWidth - Scale(116), top + Scale(9),
                        clientWidth - Scale(78), top + Scale(33)};
    DrawLabel(dc, percent, percentRect, g_smallFont, g_theme.secondaryText,
              DT_RIGHT | DT_VCENTER | DT_SINGLELINE, true);

    DrawSlider(dc, SliderRectForRow(visibleRow, clientWidth), visual.volume,
               visual.peak, muted, visual.hover);
    DrawMuteButton(dc, visibleRow, clientWidth, muted);
    if (!master) {
        RECT pin = PinRectForRow(visibleRow, clientWidth);
        bool pinned = (*g_apps)[dataRow].pinned;
        DrawLabel(dc, pinned ? L"\uE840" : L"\uE718", pin, g_symbolFont,
                  pinned ? g_theme.accent : g_theme.secondaryText,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void RenderMixerContents(HDC dc, const RECT& client) {
    if (g_paintMatte >= 0) {
        FillSolidRect(dc, client, g_paintMatte ? RGB(255, 255, 255) : RGB(0, 0, 0));
        DrawSurface(dc, client, Scale(20), g_theme.background);
    } else {
        FillSolidRect(dc, client, g_theme.background);
    }

    RECT titleRect = {Scale(24), Scale(16), client.right - Scale(60), Scale(43)};
    DrawLabel(dc, L"Volume mixer", titleRect, g_titleFont, g_theme.text,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT subtitleRect = {Scale(24), Scale(43), client.right - Scale(60), Scale(64)};
    DrawLabel(dc, g_endpointName.empty() ? L"No output device" : g_endpointName,
              subtitleRect, g_smallFont, g_theme.secondaryText,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    RECT closeRect = {client.right - Scale(52), Scale(17),
                      client.right - Scale(20), Scale(49)};
    if (g_paintMatte != 1) {
        g_frameAnimating |= std::abs(g_closeHoverAmount -
                                     (g_closeHovered ? 1.0f : 0.0f)) >= 0.001f;
        g_closeHoverAmount = EaseTowards(g_closeHoverAmount, g_closeHovered ? 1.0f : 0.0f, 18.0f);
    }
    DrawSurface(dc, closeRect, Scale(8),
                    MixColor(g_theme.background, g_theme.hover, g_closeHoverAmount));
    DrawLabel(dc, L"\uE8BB", closeRect, g_symbolFont, g_theme.secondaryText,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    if (!g_audioAvailable) {
        RECT message = {Scale(24), HeaderHeight() + Scale(30),
                        client.right - Scale(24), client.bottom - Scale(20)};
        DrawLabel(dc, L"No controllable playback sessions or default output.", message,
                  g_bodyFont, g_theme.secondaryText,
                  DT_CENTER | DT_VCENTER | DT_WORDBREAK);
    } else {
        DrawAudioRow(dc, 0, DRAG_MASTER, client.right);

        RECT section = {Scale(24), HeaderHeight() + RowHeight() + Scale(5),
                        client.right - Scale(24), RowTop(1) - Scale(3)};
        DrawLabel(dc, L"Applications", section, g_smallFont, g_theme.secondaryText,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        DrawLabel(dc, std::to_wstring(g_apps->size()), section, g_smallFont,
                  g_theme.secondaryText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        if (g_apps->empty()) {
            RECT empty = {Scale(24), RowTop(1) + Scale(8),
                          client.right - Scale(24), client.bottom - Scale(12)};
            DrawLabel(dc, g_settings.showAllSessions
                              ? L"No applications have a playback session."
                              : L"No app sessions found. Start playback to add one.",
                      empty, g_bodyFont, g_theme.secondaryText,
                      DT_CENTER | DT_VCENTER | DT_WORDBREAK);
        } else {
            int count = VisibleAppCount();
            for (int i = 0; i < count; i++) {
                DrawAudioRow(dc, i + 1, g_scrollRow + i, client.right);
            }
        }
    }

    if (PageCount() > 1) {
        for (bool next : {false, true}) {
            RECT button = PageButtonRect(next, client);
            bool hovered = g_hoverPageButton == (next ? 1 : 0);
            // Opaque button faces remain clickable even at 0% background opacity.
            DrawRoundedRect(dc, button, Scale(8), hovered ? g_theme.hover : g_theme.panel);
            DrawLabel(dc, next ? L"Next" : L"Previous", button, g_smallFont,
                      g_theme.text, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        RECT counter = {Scale(128), client.bottom - Scale(44),
                        client.right - Scale(128), client.bottom - Scale(12)};
        std::wstring text = L"Page " + std::to_wstring(g_scrollRow / PageSize() + 1) +
                            L" of " + std::to_wstring(PageCount());
        DrawLabel(dc, text, counter, g_smallFont, g_theme.secondaryText,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

struct PixelBuffer {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    DWORD* pixels = nullptr;
    int width = 0, height = 0;
    void Reset() {
        if (dc && previous) SelectObject(dc, previous);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
        dc = nullptr; bitmap = nullptr; previous = nullptr; pixels = nullptr;
        width = height = 0;
    }
    bool Ensure(int w, int h) {
        if (dc && width == w && height == h) return true;
        Reset();
        if (w <= 0 || h <= 0) return false;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = w;
        info.bmiHeader.biHeight = -h;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        dc = CreateCompatibleDC(nullptr);
        if (!bitmap || !bits || !dc) { Reset(); return false; }
        previous = SelectObject(dc, bitmap);
        if (!previous || previous == HGDI_ERROR) { previous = nullptr; Reset(); return false; }
        pixels = static_cast<DWORD*>(bits);
        width = w; height = h;
        return true;
    }
};
PixelBuffer g_blackFrame, g_whiteFrame;

DWORD RecoverPixelAlpha(DWORD black, DWORD white) {
    int transparency = 0;
    for (int shift : {0, 8, 16}) {
        int difference = static_cast<int>((white >> shift) & 255) -
                         static_cast<int>((black >> shift) & 255);
        transparency = std::max(transparency, difference);
    }
    DWORD alpha = 255 - ClampInt(transparency, 0, 255);
    DWORD result = alpha << 24;
    for (int shift : {0, 8, 16}) {
        result |= std::min<DWORD>((black >> shift) & 255UL, alpha) << shift;
    }
    return result;
}

bool PaintTransparentMixer(HWND window, const RECT& client, ULONGLONG now) {
    if (!g_blackFrame.Ensure(client.right, client.bottom) ||
        !g_whiteFrame.Ensure(client.right, client.bottom)) return false;
    // GDI does not preserve alpha. Draw the same scene against black and white
    // mattes to recover per-pixel coverage, including antialiased text/icons.
    // Only surfaces use backgroundOpacity; foreground primitives stay opaque.
    g_paintMatte = 0;
    RenderMixerContents(g_blackFrame.dc, client);
    g_paintMatte = 1;
    RenderMixerContents(g_whiteFrame.dc, client);
    g_paintMatte = -1;
    GdiFlush();
    float reveal = g_openTime && g_motionEnabled
        ? ClampVolume(static_cast<float>(now - g_openTime) / 160.0f) : 1.0f;
    reveal = 1.0f - std::pow(1.0f - reveal, 3.0f);
    int offset = static_cast<int>(Scale(8) * (1.0f - reveal));
    // Work bottom-up so the reveal can shift rows in place without overwriting
    // unread source pixels. The actual window and its hit boxes do not move.
    for (int y = client.bottom - 1; y >= 0; --y) {
        for (int x = 0; x < client.right; ++x) {
            size_t destination = static_cast<size_t>(y) * client.right + x;
            if (y < offset) {
                g_blackFrame.pixels[destination] = 0;
            } else {
                size_t source = static_cast<size_t>(y - offset) * client.right + x;
                g_blackFrame.pixels[destination] = RecoverPixelAlpha(
                    g_blackFrame.pixels[source], g_whiteFrame.pixels[source]);
            }
        }
    }
    POINT origin{};
    SIZE size{client.right, client.bottom};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, static_cast<BYTE>(255 * reveal), AC_SRC_ALPHA};
    BOOL result = UpdateLayeredWindow(window, nullptr, nullptr, &size,
                                     g_blackFrame.dc, &origin, 0, &blend, ULW_ALPHA);
    if (reveal >= 1.0f) g_openTime = 0;
    return result != FALSE;
}

void PaintMixer(HWND hWnd) {
    g_frameAnimating = false;
    g_frameHasAudio = false;
    ULONGLONG now = GetTickCount64();
    g_frameSeconds = g_lastPaintTime
        ? std::min(0.05f, static_cast<float>(now - g_lastPaintTime) / 1000.0f) : 0.016f;
    g_lastPaintTime = now;
    if (g_rowVisuals.size() > 256) g_rowVisuals.clear();
    PAINTSTRUCT paint;
    HDC windowDc = BeginPaint(hWnd, &paint);
    RECT client;
    GetClientRect(hWnd, &client);
    if (client.right <= 0 || client.bottom <= 0) { EndPaint(hWnd, &paint); return; }
    if (g_settings.backgroundOpacity < 100 && !g_transparencyFailed) {
        if (PaintTransparentMixer(hWnd, client, now)) {
            UpdatePaintTimer(hWnd);
            EndPaint(hWnd, &paint);
            return;
        }
        Wh_Log(L"Mixer: transparent rendering failed, using opaque background, error %lu", GetLastError());
        g_transparencyFailed = true;
        ApplyTransparencyStyle(hWnd);
    }
    g_paintMatte = -1;
    HDC dc = CreateCompatibleDC(windowDc);
    HBITMAP bitmap = CreateCompatibleBitmap(windowDc, client.right, client.bottom);
    if (!dc || !bitmap) {
        if (dc) DeleteDC(dc);
        if (bitmap) DeleteObject(bitmap);
        EndPaint(hWnd, &paint);
        return;
    }
    HGDIOBJ oldBitmap = SelectObject(dc, bitmap);
    RenderMixerContents(dc, client);

    float reveal = g_openTime && g_motionEnabled
        ? ClampVolume(static_cast<float>(now - g_openTime) / 160.0f) : 1.0f;
    reveal = 1.0f - std::pow(1.0f - reveal, 3.0f);
    if (reveal < 1.0f) {
        // Composite into a second buffer. The actual window stays stationary
        // and opaque, preserving normal DWM corners, focus, and tray behavior.
        HDC composite = CreateCompatibleDC(windowDc);
        HBITMAP frame = CreateCompatibleBitmap(windowDc, client.right, client.bottom);
        if (composite && frame) {
            HGDIOBJ oldFrame = SelectObject(composite, frame);
            FillSolidRect(composite, client, g_theme.background);
            int offset = static_cast<int>(Scale(8) * (1.0f - reveal));
            BLENDFUNCTION blend = {AC_SRC_OVER, 0,
                static_cast<BYTE>(255.0f * reveal), 0};
            if (AlphaBlend(composite, 0, offset, client.right, client.bottom - offset,
                           dc, 0, 0, client.right, client.bottom - offset, blend)) {
                BitBlt(windowDc, 0, 0, client.right, client.bottom, composite, 0, 0, SRCCOPY);
            } else {
                BitBlt(windowDc, 0, 0, client.right, client.bottom, dc, 0, 0, SRCCOPY);
            }
            SelectObject(composite, oldFrame);
        } else {
            BitBlt(windowDc, 0, 0, client.right, client.bottom, dc, 0, 0, SRCCOPY);
        }
        if (frame) DeleteObject(frame);
        if (composite) DeleteDC(composite);
    } else {
        g_openTime = 0;
        BitBlt(windowDc, 0, 0, client.right, client.bottom, dc, 0, 0, SRCCOPY);
    }
    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
    UpdatePaintTimer(hWnd);
    EndPaint(hWnd, &paint);
}

void ResizeMixer(HWND hWnd) {
    RECT windowRect;
    GetWindowRect(hWnd, &windowRect);
    SetWindowPos(hWnd, nullptr, windowRect.left, windowRect.top, PopupWidth(),
                 PopupHeight(), SWP_NOZORDER | SWP_NOACTIVATE);
}

bool GetTrayIconRect(RECT* rect) {
    NOTIFYICONIDENTIFIER identifier = {};
    identifier.cbSize = sizeof(identifier);
    identifier.hWnd = g_hWnd.load();
    identifier.uID = TRAY_ICON_ID;
    if (g_trayUsesGuid) identifier.guidItem = MIXER_TRAY_GUID;
    return SUCCEEDED(Shell_NotifyIconGetRect(&identifier, rect));
}

void PositionMixer(HWND hWnd) {
    RECT anchor = {};
    if (!GetTrayIconRect(&anchor)) {
        POINT cursor = {};
        GetCursorPos(&cursor);
        anchor = {cursor.x, cursor.y, cursor.x + 1, cursor.y + 1};
    }

    HMONITOR monitor = MonitorFromRect(&anchor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info = {sizeof(info)};
    GetMonitorInfoW(monitor, &info);

    int gap = Scale(8);
    g_availableAppRows = AppRowsForHeight(info.rcWork.bottom - info.rcWork.top - 2 * gap);
    ClampPageOffset();
    int width = PopupWidth();
    int height = PopupHeight();
    int x = (anchor.left + anchor.right - width) / 2;
    int y = anchor.top - height - gap;

    if (y < info.rcWork.top) {
        y = anchor.bottom + gap;
    }
    x = ClampInt(x, info.rcWork.left, std::max(info.rcWork.left, info.rcWork.right - width));
    y = ClampInt(y, info.rcWork.top, std::max(info.rcWork.top, info.rcWork.bottom - height));

    RECT current = {};
    if (!GetWindowRect(hWnd, &current) || current.left != x || current.top != y ||
        current.right - current.left != width || current.bottom - current.top != height) {
        SetWindowPos(hWnd, HWND_TOPMOST, x, y, width, height,
                     SWP_NOOWNERZORDER | SWP_NOACTIVATE);
    }
}

void ShowMixer(HWND hWnd) {
    if (g_showingMixer) {
        return;
    }
    g_showingMixer = true;
    KillTimer(hWnd, TIMER_CHECK_FOCUS);
    // This function always opens; toggle intent is resolved before queuing.
    bool opening = !IsWindowVisible(hWnd);
    if (opening) {
        RefreshAudioSessions(hWnd, true);
        UpdateMotionPreference(hWnd);
        g_openTime = g_motionEnabled ? GetTickCount64() : 0;
        g_lastPaintTime = 0;
        g_hoverRow = DRAG_NONE;
        g_closeHovered = false;
        g_closeHoverAmount = 0;
        g_rowVisuals.clear();
    }
    PositionMixer(hWnd);
    ShowWindow(hWnd, SW_SHOWNORMAL);
    SetTimer(hWnd, TIMER_REFRESH, 1000, nullptr);
    SetPaintTimer(hWnd, g_motionEnabled ? 16 : 80);
    BOOL foreground = SetForegroundWindow(hWnd);
    if (foreground) {
        SetFocus(hWnd);
    } else if (g_settings.closeWhenFocusIsLost) {
        SetTimer(hWnd, TIMER_CHECK_FOCUS, 150, nullptr);
    }
    InvalidateRect(hWnd, nullptr, FALSE);
    UpdateWindow(hWnd);
    g_showingMixer = false;
    Wh_Log(L"Mixer: popup shown, foreground request=%d", foreground);
}

void QueueMixerAction(HWND hWnd, bool close) {
    KillTimer(hWnd, TIMER_CHECK_FOCUS);
    if (!g_showRequestPending) {
        g_showRequestPending = true;
        g_closeRequestPending = close;
        // Return to the shell callback before showing/activating the popup.
        if (!PostMessageW(hWnd, WM_APP_SHOW_MIXER, 0, 0)) {
            g_showRequestPending = false;
            Wh_Log(L"Mixer: could not queue popup, error %lu", GetLastError());
        }
    }
}

void QueueShowMixer(HWND hWnd) {
    QueueMixerAction(hWnd, false);
}

void RememberTrayPress(HWND hWnd) {
    g_trayPressKnown = true;
    g_trayPressWasVisible = IsWindowVisible(hWnd) != FALSE;
}

void QueueTrayToggle(HWND hWnd, bool keyboard) {
    // The taskbar can take focus and dismiss the popup before mouse-up.
    // Use its visibility at mouse-down, so a closing click cannot reopen it.
    bool close = !keyboard && g_trayPressKnown
        ? g_trayPressWasVisible : IsWindowVisible(hWnd) != FALSE;
    g_trayPressKnown = false;
    QueueMixerAction(hWnd, close);
}

void HideMixer(HWND hWnd) {
    g_showRequestPending = false;
    KillTimer(hWnd, TIMER_CHECK_FOCUS);
    KillTimer(hWnd, TIMER_METERS);
    KillTimer(hWnd, TIMER_REFRESH);
    g_paintTimerInterval = 0;
    g_openTime = 0;
    g_dragRow = DRAG_NONE;
    if (GetCapture() == hWnd) {
        ReleaseCapture();
    }
    ShowWindow(hWnd, SW_HIDE);
    g_blackFrame.Reset();
    g_whiteFrame.Reset();
    ClearTextCache();
}

enum class TrayAction { None, Open, Menu };

TrayAction GetTrayAction(UINT event, bool version4) {
    if (version4) {
        if (event == NIN_SELECT || event == NIN_KEYSELECT) {
            return TrayAction::Open;
        }
        if (event == WM_CONTEXTMENU) {
            return TrayAction::Menu;
        }
    } else {
        if (event == WM_LBUTTONUP) {
            return TrayAction::Open;
        }
        if (event == WM_RBUTTONUP) {
            return TrayAction::Menu;
        }
    }
    return TrayAction::None;
}

void AddTrayIcon(HWND hWnd) {
    g_trayVersion4 = false;
    g_trayUsesGuid = true;
    NOTIFYICONDATAW data = {};
    data.cbSize = sizeof(data);
    data.hWnd = hWnd;
    data.uID = TRAY_ICON_ID;
    data.guidItem = MIXER_TRAY_GUID;
    data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID | NIF_SHOWTIP;
    data.uCallbackMessage = WM_APP_TRAY;
    data.hIcon = g_trayIcon;
    wcscpy_s(data.szTip, L"Better Volume Mixer");
    if (!Shell_NotifyIconW(NIM_ADD, &data)) {
        // A GUID can remain associated with an earlier Windhawk executable path.
        // Keep the same identity mode for all later operations on this icon.
        data.uFlags &= ~NIF_GUID;
        data.guidItem = {};
        g_trayUsesGuid = false;
        if (!Shell_NotifyIconW(NIM_ADD, &data)) {
            Wh_Log(L"Mixer: tray icon registration failed");
            return;
        }
        Wh_Log(L"Mixer: tray icon registered using window/ID fallback");
    }
    Wh_Log(L"Mixer: tray icon registered");

    data.uVersion = NOTIFYICON_VERSION_4;
    g_trayVersion4 = Shell_NotifyIconW(NIM_SETVERSION, &data) != FALSE;
    if (!g_trayVersion4) {
        Wh_Log(L"Mixer: tray icon version negotiation failed");
    }
    Wh_Log(L"Mixer: tray callback mode=%s", g_trayVersion4 ? L"v4" : L"legacy");
}

void RemoveTrayIcon(HWND hWnd) {
    NOTIFYICONDATAW data = {};
    data.cbSize = sizeof(data);
    data.hWnd = hWnd;
    data.uID = TRAY_ICON_ID;
    data.uFlags = g_trayUsesGuid ? NIF_GUID : 0;
    if (g_trayUsesGuid) data.guidItem = MIXER_TRAY_GUID;
    Shell_NotifyIconW(NIM_DELETE, &data);
}

bool SpeakerShapeContains(float x, float y) {
    // A solid speaker silhouette with two separated sound-wave arcs, in a
    // 16x16 design grid. Rasterized from geometry, not a scaled app icon.
    constexpr float points[][2] = {
        {1.5f, 5.5f}, {4.5f, 5.5f}, {8.0f, 2.5f},
        {8.0f, 13.5f}, {4.5f, 10.5f}, {1.5f, 10.5f}};
    bool inside = false;
    for (int i = 0, j = 5; i < 6; j = i++) {
        if ((points[i][1] > y) != (points[j][1] > y) &&
            x < (points[j][0] - points[i][0]) * (y - points[i][1]) /
                    (points[j][1] - points[i][1]) + points[i][0]) {
            inside = !inside;
        }
    }
    if (inside) return true;
    float dx = x - 6.8f, dy = y - 8.0f;
    float radius = std::sqrt(dx * dx + dy * dy);
    bool inner = dx > 2.2f && std::abs(dy) < 2.8f &&
                 std::abs(radius - 3.8f) < 0.65f;
    bool outer = dx > 3.8f && std::abs(dy) < 5.6f &&
                 std::abs(radius - 6.6f) < 0.65f;
    return inner || outer;
}

HICON CreateSpeakerIcon(int pixels, COLORREF color) {
    pixels = ClampInt(pixels, 16, 256);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = pixels;
    info.bmiHeader.biHeight = -pixels;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) DeleteObject(bitmap);
        return nullptr;
    }
    auto output = static_cast<DWORD*>(bits);
    constexpr int samples = 4;
    for (int y = 0; y < pixels; ++y) {
        for (int x = 0; x < pixels; ++x) {
            int covered = 0;
            for (int sy = 0; sy < samples; ++sy) {
                for (int sx = 0; sx < samples; ++sx) {
                    covered += SpeakerShapeContains(
                        (x + (sx + 0.5f) / samples) * 16.0f / pixels,
                        (y + (sy + 0.5f) / samples) * 16.0f / pixels);
                }
            }
            DWORD alpha = covered * 255 / (samples * samples);
            // Icons use premultiplied BGRA, including partially covered edges.
            output[y * pixels + x] = (alpha << 24) |
                ((GetRValue(color) * alpha / 255) << 16) |
                ((GetGValue(color) * alpha / 255) << 8) |
                (GetBValue(color) * alpha / 255);
        }
    }
    std::vector<BYTE> maskBits(((pixels + 15) / 16) * 2 * pixels, 0);
    HBITMAP mask = CreateBitmap(pixels, pixels, 1, 1, maskBits.data());
    HICON icon = nullptr;
    if (mask) {
        ICONINFO iconInfo{};
        iconInfo.fIcon = TRUE;
        iconInfo.hbmColor = bitmap;
        iconInfo.hbmMask = mask;
        icon = CreateIconIndirect(&iconInfo);
        DeleteObject(mask);
    }
    DeleteObject(bitmap);
    return icon;
}

HICON LoadMixerIcon(int pixels, bool tray = false) {
    COLORREF color = g_theme.text;
    if (tray) {
        DWORD light = 0, size = sizeof(light);
        RegGetValueW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);
        color = light ? RGB(30, 32, 36) : RGB(248, 249, 252);
    }
    if (HICON icon = CreateSpeakerIcon(pixels, color)) return icon;
    WCHAR systemDirectory[MAX_PATH];
    UINT length = GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
    if (length && length < ARRAYSIZE(systemDirectory)) {
        std::wstring path(systemDirectory, length);
        path += L"\\SndVol.exe";
        if (HICON icon = ExtractSizedIcon(path, pixels)) {
            return icon;
        }
    }
    return static_cast<HICON>(LoadImageW(nullptr, IDI_APPLICATION, IMAGE_ICON,
                                         pixels, pixels, 0));
}

void RefreshIconSizes(HWND hWnd) {
    // The popup and taskbar may be on monitors with different scale factors.
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    UINT trayDpi = taskbar ? GetDpiForWindow(taskbar) : 96;
    if (!trayDpi) {
        trayDpi = 96;
    }
    int trayPixels = GetSystemMetricsForDpi(SM_CXSMICON, trayDpi);
    HICON tray = LoadMixerIcon(std::max(16, trayPixels), true);
    if (tray) {
        if (g_hWnd.load() == hWnd) {
            NOTIFYICONDATAW data = {};
            data.cbSize = sizeof(data);
            data.hWnd = hWnd;
            data.uID = TRAY_ICON_ID;
            if (g_trayUsesGuid) data.guidItem = MIXER_TRAY_GUID;
            data.uFlags = NIF_ICON | (g_trayUsesGuid ? NIF_GUID : 0);
            data.hIcon = tray;
            if (!Shell_NotifyIconW(NIM_MODIFY, &data)) {
                Wh_Log(L"Mixer: tray DPI icon update failed");
            }
        }
        if (g_trayIcon) {
            DestroyIcon(g_trayIcon);
        }
        g_trayIcon = tray;
    }
    HICON master = LoadMixerIcon(Scale(24));
    if (master) {
        if (g_masterIcon) {
            DestroyIcon(g_masterIcon);
        }
        g_masterIcon = master;
    }
    for (auto& app : *g_apps) {
        if (app.icon) {
            DestroyIcon(app.icon);
        }
        app.icon = GetExecutableIcon(app.executablePath);
    }
}

void OpenSystemPage(PCWSTR uri) {
    ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL);
}

void ShowTrayMenu(HWND hWnd, int x, int y) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }
    AppendMenuW(menu, MF_STRING | MF_DEFAULT, MENU_OPEN, L"Open volume mixer");
    AppendMenuW(menu, MF_STRING, MENU_REFRESH, L"Refresh applications");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, MENU_SOUND_SETTINGS, L"Sound settings");
    AppendMenuW(menu, MF_STRING, MENU_WINDOWS_MIXER, L"Windows volume mixer");

    SetForegroundWindow(hWnd);
    UINT command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY |
                                           TPM_RIGHTBUTTON,
                                  x, y, 0, hWnd, nullptr);
    PostMessageW(hWnd, WM_NULL, 0, 0);
    DestroyMenu(menu);

    switch (command) {
        case MENU_OPEN:
            QueueShowMixer(hWnd);
            break;
        case MENU_REFRESH:
            RefreshAudioSessions(hWnd, true);
            if (IsWindowVisible(hWnd)) {
                ResizeMixer(hWnd);
                PositionMixer(hWnd);
            }
            break;
        case MENU_SOUND_SETTINGS:
            OpenSystemPage(L"ms-settings:sound");
            break;
        case MENU_WINDOWS_MIXER:
            OpenSystemPage(L"ms-settings:apps-volume");
            break;
    }
}

void HandlePointerMove(HWND hWnd, POINT point) {
    RECT client;
    GetClientRect(hWnd, &client);
    RECT closeRect = {client.right - Scale(52), Scale(17),
                      client.right - Scale(20), Scale(49)};
    bool closeHovered = PtInRect(&closeRect, point) != FALSE;
    int pageHovered = -1;
    if (PageCount() > 1) {
        for (bool next : {false, true}) {
            RECT button = PageButtonRect(next, client);
            if (PtInRect(&button, point)) pageHovered = next ? 1 : 0;
        }
    }
    if (pageHovered != g_hoverPageButton) {
        g_hoverPageButton = pageHovered;
        InvalidateRect(hWnd, nullptr, FALSE);
    }
    if (closeHovered != g_closeHovered) {
        g_closeHovered = closeHovered;
        InvalidateRect(hWnd, nullptr, FALSE);
    }
    int row = RowFromPoint(point);
    if (row != g_hoverRow) {
        g_hoverRow = row;
        InvalidateRect(hWnd, nullptr, FALSE);
    }

    if (!g_mouseTracking) {
        TRACKMOUSEEVENT tracking = {sizeof(tracking), TME_LEAVE, hWnd, 0};
        TrackMouseEvent(&tracking);
        g_mouseTracking = true;
    }

    if (g_dragRow != DRAG_NONE && (GetKeyState(VK_LBUTTON) & 0x8000)) {
        ApplyVolumeFromPoint(hWnd, g_dragRow, point.x);
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
                            LPARAM lParam) {
    if (message == g_taskbarCreatedMessage && g_taskbarCreatedMessage) {
        AddTrayIcon(hWnd);
        return 0;
    }

    switch (message) {
        case WM_CREATE: {
            Gdiplus::GdiplusStartupInput startup;
            if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &startup, nullptr) != Gdiplus::Ok) {
                g_gdiplusToken = 0;
                Wh_Log(L"Mixer: typography initialization failed; using GDI text");
            }
            CreateFonts(hWnd);
            UpdateTheme();
            RefreshIconSizes(hWnd);
            ApplyTransparencyStyle(hWnd);
            DWORD corner = 2;  // DWMWCP_ROUND.
            DwmSetWindowAttribute(hWnd, 33, &corner, sizeof(corner));
            return 0;
        }

        case WM_APP_TRAY: {
            UINT event = g_trayVersion4 ? LOWORD(lParam)
                                        : static_cast<UINT>(lParam);
            if (event == WM_LBUTTONDOWN || event == WM_LBUTTONDBLCLK) {
                RememberTrayPress(hWnd);
            }
            TrayAction action = GetTrayAction(event, g_trayVersion4);
            if (action != TrayAction::None) {
                Wh_Log(L"Mixer: tray action event=0x%X", event);
            }
            if (action == TrayAction::Open) {
                QueueTrayToggle(hWnd, event == NIN_KEYSELECT);
            } else if (action == TrayAction::Menu) {
                g_trayPressKnown = false;
                // WM_CONTEXTMENU coordinates in the tray callback are not
                // guaranteed. Anchor keyboard menus at the icon, others at
                // the cursor, rather than decoding an undefined wParam.
                POINT cursor = {};
                GetCursorPos(&cursor);
                RECT iconRect = {};
                if (GetKeyState(VK_RBUTTON) >= 0 && GetTrayIconRect(&iconRect)) {
                    cursor = {iconRect.left, iconRect.bottom};
                }
                ShowTrayMenu(hWnd, cursor.x, cursor.y);
            }
            return 0;
        }

        case WM_APP_SHOW_MIXER:
            if (g_showRequestPending) {
                g_showRequestPending = false;
                if (g_closeRequestPending) {
                    HideMixer(hWnd);
                } else {
                    ShowMixer(hWnd);
                }
            }
            return 0;

        case WM_APP_RELOAD_SETTINGS:
            LoadSettings();
            g_transparencyFailed = false;
            ApplyTransparencyStyle(hWnd);
            UpdateMotionPreference(hWnd);
            UpdateTheme();
            RefreshIconSizes(hWnd);
            RefreshAudioSessions(hWnd);
            ResizeMixer(hWnd);
            if (IsWindowVisible(hWnd)) {
                PositionMixer(hWnd);
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;

        case WM_TIMER:
            if (wParam == TIMER_CHECK_FOCUS) {
                KillTimer(hWnd, TIMER_CHECK_FOCUS);
                HWND foreground = GetForegroundWindow();
                bool mixerOwnsFocus = foreground == hWnd ||
                    (foreground && GetAncestor(foreground, GA_ROOTOWNER) == hWnd);
                if (!g_showingMixer && !g_showRequestPending &&
                    g_settings.closeWhenFocusIsLost && IsWindowVisible(hWnd) &&
                    !mixerOwnsFocus && GetCapture() != hWnd) {
                    Wh_Log(L"Mixer: closing after confirmed focus loss");
                    HideMixer(hWnd);
                }
                return 0;
            }
            if (wParam == TIMER_METERS && IsWindowVisible(hWnd)) {
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (wParam == TIMER_REFRESH) {
                // KillTimer doesn't remove a timer message already in the queue.
                if (IsWindowVisible(hWnd)) {
                    RefreshAudioSessions(hWnd);
                    PositionMixer(hWnd);
                }
                return 0;
            }
            break;

        case WM_PAINT:
            if (IsWindowVisible(hWnd)) {
                PaintMixer(hWnd);
            } else {
                ValidateRect(hWnd, nullptr);
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            HandlePointerMove(hWnd, point);
            return 0;
        }

        case WM_MOUSELEAVE:
            g_mouseTracking = false;
            g_hoverRow = DRAG_NONE;
            g_closeHovered = false;
            g_hoverPageButton = -1;
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;

        case WM_LBUTTONDOWN: {
            // Finish the visual reveal before handling exact slider hit boxes.
            g_openTime = 0;
            SetFocus(hWnd);
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            RECT client;
            GetClientRect(hWnd, &client);

            RECT closeRect = {client.right - Scale(52), Scale(17),
                              client.right - Scale(20), Scale(49)};
            if (PtInRect(&closeRect, point)) {
                HideMixer(hWnd);
                return 0;
            }

            if (PageCount() > 1) {
                for (bool next : {false, true}) {
                    RECT button = PageButtonRect(next, client);
                    if (PtInRect(&button, point)) {
                        ChangePage(hWnd, next ? 1 : -1);
                        return 0;
                    }
                }
            }

            int row = RowFromPoint(point);
            if (row == DRAG_NONE) {
                return 0;
            }
            int visibleRow = VisibleRowForDataRow(row);
            RECT pinRect = PinRectForRow(visibleRow, client.right);
            if (row >= 0 && PtInRect(&pinRect, point)) {
                ToggleSourcePin(hWnd, row);
                return 0;
            }
            RECT muteRect = MuteRectForRow(visibleRow, client.right);
            if (PtInRect(&muteRect, point)) {
                ToggleRowMute(row);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }

            RECT sliderRect = SliderRectForRow(visibleRow, client.right);
            InflateRect(&sliderRect, 0, Scale(12));
            if (PtInRect(&sliderRect, point)) {
                g_dragRow = row;
                SetCapture(hWnd);
                ApplyVolumeFromPoint(hWnd, row, point.x);
            }
            return 0;
        }

        case WM_LBUTTONUP:
            if (g_dragRow != DRAG_NONE) {
                g_dragRow = DRAG_NONE;
                ReleaseCapture();
            }
            return 0;

        case WM_CAPTURECHANGED:
            g_dragRow = DRAG_NONE;
            return 0;

        case WM_MOUSEWHEEL: {
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hWnd, &point);
            int row = RowFromPoint(point);
            RECT client;
            GetClientRect(hWnd, &client);

            bool overSlider = false;
            if (row != DRAG_NONE) {
                RECT slider = SliderRectForRow(VisibleRowForDataRow(row),
                                                client.right);
                InflateRect(&slider, 0, Scale(14));
                overSlider = PtInRect(&slider, point) != FALSE;
            }

            int notches = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            if (overSlider && notches) {
                float current = row == DRAG_MASTER ? GetMasterVolume()
                                                   : (*g_apps)[row].volume;
                SetRowVolume(row, current + notches *
                                              g_settings.volumeStep / 100.0f);
                InvalidateRect(hWnd, nullptr, FALSE);
            } else if (!g_apps->empty() && notches) {
                ChangePage(hWnd, -notches);
            }
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                HideMixer(hWnd);
                return 0;
            }
            if (wParam == VK_PRIOR || wParam == VK_NEXT) {
                ChangePage(hWnd, wParam == VK_NEXT ? 1 : -1);
                return 0;
            }
            break;

        case WM_ACTIVATE:
            if (LOWORD(wParam) != WA_INACTIVE) {
                KillTimer(hWnd, TIMER_CHECK_FOCUS);
            } else if (!g_showingMixer && !g_showRequestPending &&
                g_settings.closeWhenFocusIsLost && IsWindowVisible(hWnd)) {
                // Capture intent even if activation arrives before the tray's
                // mouse-down callback. Focus dismissal remains independent.
                POINT cursor = {};
                RECT iconRect = {};
                if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) &&
                    GetCursorPos(&cursor) && GetTrayIconRect(&iconRect) &&
                    PtInRect(&iconRect, cursor)) {
                    RememberTrayPress(hWnd);
                }
                // Do not hide inside a synchronous activation transition.
                // Recheck the actual foreground window after the shell settles.
                SetTimer(hWnd, TIMER_CHECK_FOCUS, 150, nullptr);
            }
            break;

        case WM_SETTINGCHANGE:
            UpdateMotionPreference(hWnd);
            if (g_settings.theme == L"system") {
                UpdateTheme();
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            if (lParam && wcscmp(reinterpret_cast<PCWSTR>(lParam),
                                  L"ImmersiveColorSet") == 0) {
                RefreshIconSizes(hWnd);
            }
            break;

        case WM_DISPLAYCHANGE:
            RefreshIconSizes(hWnd);
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case WM_DPICHANGED: {
            g_dpi = HIWORD(wParam);
            CreateFonts(hWnd);
            RefreshIconSizes(hWnd);
            RECT* suggested = reinterpret_cast<RECT*>(lParam);
            SetWindowPos(hWnd, nullptr, suggested->left, suggested->top,
                         PopupWidth(), PopupHeight(),
                         SWP_NOZORDER | SWP_NOACTIVATE);
            PositionMixer(hWnd);
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }

        case WM_DESTROY:
            g_blackFrame.Reset();
            g_whiteFrame.Reset();
            KillTimer(hWnd, TIMER_METERS);
            KillTimer(hWnd, TIMER_REFRESH);
            KillTimer(hWnd, TIMER_CHECK_FOCUS);
            g_showRequestPending = false;
            RemoveTrayIcon(hWnd);
            ReleaseAudioData();
            g_activatedSessions.clear();
            g_liveSessions.clear();
            g_rowVisuals.clear();
            ClearIconCache();
            if (g_masterIcon) {
                DestroyIcon(g_masterIcon);
                g_masterIcon = nullptr;
            }
            DeleteFonts();
            if (g_gdiplusToken) {
                Gdiplus::GdiplusShutdown(g_gdiplusToken);
                g_gdiplusToken = 0;
            }
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

constexpr PCWSTR WINDOW_CLASS_NAME = L"WindhawkBetterTaskbarVolumeMixer";

DWORD WINAPI ThreadProc(LPVOID) {
    Wh_Log(L"Mixer: UI thread started, PID=%lu", GetCurrentProcessId());
    // Limit the DPI override to our own UI thread, not the Windhawk launcher.
    if (!SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
        Wh_Log(L"Mixer: per-monitor DPI setup failed, error %lu", GetLastError());
    }
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    Wh_Log(L"Mixer: COM initialization result=0x%08lX",
           static_cast<unsigned long>(comResult));
    if (FAILED(comResult)) {
        SetEvent(g_windowReadyEvent);
        return 1;
    }

    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_DBLCLKS | CS_DROPSHADOW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW failed, error %u", GetLastError());
        SetEvent(g_windowReadyEvent);
        if (g_trayIcon) {
            DestroyIcon(g_trayIcon);
            g_trayIcon = nullptr;
        }
        if (SUCCEEDED(comResult)) {
            CoUninitialize();
        }
        return 1;
    }

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST, WINDOW_CLASS_NAME, L"Volume mixer",
        WS_POPUP, 0, 0, 420, 400, nullptr, nullptr, windowClass.hInstance,
        nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowExW failed, error %u", GetLastError());
        UnregisterClassW(WINDOW_CLASS_NAME, windowClass.hInstance);
        SetEvent(g_windowReadyEvent);
        if (g_trayIcon) {
            DestroyIcon(g_trayIcon);
            g_trayIcon = nullptr;
        }
        if (SUCCEEDED(comResult)) {
            CoUninitialize();
        }
        return 1;
    }

    g_hWnd.store(hWnd);
    Wh_Log(L"Mixer: window created, HWND=%p", static_cast<void*>(hWnd));
    AddTrayIcon(hWnd);
    SetEvent(g_windowReadyEvent);

    MSG message;
    BOOL result;
    while ((result = GetMessageW(&message, nullptr, 0, 0)) != 0) {
        if (result == -1) {
            Wh_Log(L"Mixer: GetMessage failed, error %lu", GetLastError());
            break;
        }
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (IsWindow(hWnd)) DestroyWindow(hWnd);
    g_hWnd.store(nullptr);
    g_apps.reset();
    UnregisterClassW(WINDOW_CLASS_NAME, windowClass.hInstance);
    if (g_trayIcon) {
        DestroyIcon(g_trayIcon);
        g_trayIcon = nullptr;
    }

    if (SUCCEEDED(comResult)) {
        CoUninitialize();
    }
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L"Mixer: initializing tool");
    LoadSettings();

    g_windowReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_windowReadyEvent) {
        Wh_Log(L"Mixer: CreateEvent failed, error %lu", GetLastError());
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"Mixer: CreateThread failed, error %lu", GetLastError());
        CloseHandle(g_windowReadyEvent);
        g_windowReadyEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_windowReadyEvent) {
        WaitForSingleObject(g_windowReadyEvent, 5000);
    }
    if (HWND hWnd = g_hWnd.load()) {
        PostMessageW(hWnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_windowReadyEvent) {
        WaitForSingleObject(g_windowReadyEvent, 5000);
    }
    if (HWND hWnd = g_hWnd.load()) {
        PostMessageW(hWnd, WM_CLOSE, 0, 0);
    }
    if (g_thread) {
        DWORD result = WaitForSingleObject(g_thread, 5000);
        if (result != WAIT_OBJECT_0) {
            // The caller immediately exits this dedicated tool process. Keep
            // handles alive until then: the UI thread might still use them.
            Wh_Log(L"Mixer: UI shutdown did not complete; exiting the dedicated tool process");
            return;
        }
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_windowReadyEvent) {
        CloseHandle(g_windowReadyEvent);
        g_windowReadyEvent = nullptr;
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
