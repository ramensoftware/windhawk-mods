// ==WindhawkMod==
// @id            windows-11-start-menu-buttons
// @name          Windows 11 Start Menu Buttons
// @description         Customizable buttons for the Windows 11 Start menu.
// @description:ru-RU   Настраиваемые кнопки для меню «Пуск» Windows 11.
// @version       2.1
// @author        Salyts
// @license       MIT
// @github        https://github.com/Salyts
// @include       StartMenuExperienceHost.exe
// @include       explorer.exe
// @architecture  x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshlwapi -lshell32 -luuid -luser32 -lwtsapi32 -lpowrprof -lgdi32 -lgdiplus -lshcore -lcrypt32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Start Menu Buttons 2.1

Replaces the default bottom row of the Windows 11 Start menu with fully customizable buttons.

❗**There may be issues with mods:** Windows 11 Start Menu Styler, Windows 11 Start Menu Power Buttons.

![img](https://i.imgur.com/J5CC8XP.png)

![img](https://i.imgur.com/dm7SVQj.png)

---

## Quick Start

1. Open Windhawk settings for this mod.
2. Add buttons using the **Buttons** list.
3. For each button, pick a **Preset** or set **Preset = Custom** and fill in Name, Icon, Action.
4. Save — the Start menu updates immediately.

---

## Button fields

| Field    | Description |
|----------|-------------|
| **Preset** | Ready-made button. Set to `Custom` to define your own. |
| **Name** | Tooltip shown on hover. Leave empty on presets to use their default name. |
| **Icon** | [Segoe Fluent Icons](https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-ui-symbol-font) glyph (e.g. `\uE7E8`) **or** full path to an image file (PNG, ICO, JPG, BMP). |
| **Action** | Used only when Preset = `Custom`. See action formats below. |
| **Submenu** | Optional list of child items shown in a flyout. If any submenu items are defined, the button opens the flyout instead of running a direct action. |

---

## Action formats (Custom preset only)

| Prefix | Example | Description |
|--------|---------|-------------|
| `" "` | `"C:\Program Files\Windhawk\windhawk.exe"` | Opens a file or folder by absolute path. |
| `~` | `~Downloads` and `~windhawk.exe` | Opens a folder or file by name. |
| `cmd:` | `cmd:control` | Runs a command through `cmd.exe`. |
| `shell:` | `shell:shutdown /r /f /t 0` | Runs through `powershell.exe`. |
| `press:` | `press:Win+E` or `press:0x5B;0x45` | Keyboard key press using a [Win32 key code](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes). |
| `web:` | `web:https://windhawk.net/` | Opens a URL in the default browser. |
| `ms-settings:` | `ms-settings:bluetooth` | Opens a Windows Settings page. |

### Modifier signs (prepend to any action)

| Sign | Example | Description |
|--------|---------|-------------|
| `-` | `-"C:\Program Files\app.exe"` or `-shell:shutdown /r /f /t 0` | Runs as administrator. |
| `*` | `*cmd:tasklist` or `*shell:Get-Process` | Execution with a terminal window. (only for `cmd:` and `shell:` prefixes). |

Signs can be combined: `-*cmd:tasklist` runs cmd in a visible window as admin.

---

## Icon field

| Type | Example | Description |
|---|---|---|
| **Glyph** | `E774` or `\uE774` | Hex code of a [Segoe Fluent Icons](https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-ui-symbol-font) glyph. 4-digit hex, `\u` prefix is optional. |
| **Image file** | `C:\Icons\name.png` | Full path to an image. Supported: `.png` `.ico` `.jpg` `.bmp` `.webp`. Recommended: 32x32 px, transparent background. |
| **App path** | `C:\Program Files\Windhawk\windhawk.exe` | Full path to an executable file (`.exe`, `.dll`). The icon will be extracted from the application. |

---

## Presets

| # | Name | Icon | Description |
|---|---|---|---|
| 1 | `Settings` | \uE713 | Opens Windows Settings. |
| 2 | `Explorer` | \uEC50 | Opens File Explorer. |
| 3 | `Documents` | \uE8A5 | Opens the Documents folder. |
| 4 | `Music` | \uEC4F | Opens the Music folder. |
| 5 | `Downloads` | \uE896 | Opens the Downloads folder. |
| 6 | `Pictures` | \uE91B | Opens the Pictures folder. |
| 7 | `Videos` | \uE714 | Opens the Videos folder. |
| 8 | `Network` | \uEC27 | Opens Network places. |
| 9 | `Personal Folder` | \uEC25 | Opens the user's profile folder. |
| 10 | `Shut down` | \uE7E8 | Shuts down the PC. |
| 11 | `Restart` | \uE777 | Restarts the PC. |
| 12 | `Sign out`| \uF3B1 | Logs out of the current account. |
| 13 | `Sleep` | \uE708 | Puts the PC to sleep. |
| 14 | `Hibernate` | \uE823 | Hibernates the PC. |
| 15 | `Lock` | \uE72E | Locks the PC session. |
| 16 | `Power Menu` | \uE7E8 | Opens the power options menu. |

---

## Submenus

Fill in the **Submenu** entries for a button. Each submenu item has its own Name, Icon, and Action.
When at least one submenu item exists, the button opens a flyout menu instead of executing a direct action.

---

Thank you so much, [@SharkIT-sys](https://github.com/SharkIT-sys), for helping to improve the mod!
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- preset_language: en
  $name: Preset language
  $description: Language for the tooltip names of preset buttons.
  $options:
    - en: English
    - ru: Russian (Русский)

- alignment: right
  $name: Button alignment
  $description: Horizontal alignment of the custom button container within the Start menu bottom bar.
  $options:
    - left: Left
    - center: Center
    - right: Right

- account_button: left
  $name: Account button
  $description: Position or visibility of the user account avatar button.
  $options:
    - left: Left
    - center: Center
    - right: Right
    - hide: Hide

- invert_buttons: true
  $name: Invert button order
  $description: Reverses the order in which custom buttons appear. Useful when alignment is set to Right.

- invert_icons_submenus: false
  $name: Invert icons in submenus
  $description: Reverses the order in which icons are displayed in submenus.

- close_start_menu: true
  $name: Close Start menu after click
  $description: Automatically closes the Start menu after clicking any custom button (except buttons that open a submenu), does not work for the “press:” prefix.

- button_spacing: 0
  $name: Spacing between buttons (px)
  $description: Horizontal margin between each button.

- container_margin_left: -16
  $name: Container left margin (px)
  $description: Left margin of the custom button container. Negative values let it overlap the default padding.

- container_margin_right: -16
  $name: Container right margin (px)
  $description: Right margin of the custom button container. Negative values let it overlap the default padding.

- buttons:
    - - Preset: menu_shutdown
        $name: Preset
        $description: "Choose a built-in preset button, or select Custom to define your own Name / Icon / Action."
        $options: 
          - custom: Custom
          - settings: Settings
          - explorer: Explorer
          - documents: Documents
          - downloads: Downloads
          - music: Music
          - pictures: Pictures
          - videos: Videos
          - network: Network
          - personal_folder: Personal Folder
          - shutdown: Shut down
          - restart: Restart
          - sign_out: Sign out
          - sleep: Sleep
          - hibernate: Hibernate
          - lock: Lock
          - menu_shutdown: Power Menu
      - Name: ""
        $name: Name
        $description: "Tooltip shown on hover. Leave empty on preset buttons to use the preset default."
      - Icon: ""
        $name: Icon
        $description: "Segoe Fluent glyph (e.g. E7E8 or \\uE7E8), image path (e.g. C:\\Icons\\app.png), or app path (e.g. C:\\Program Files\\app.exe). Leave empty for preset default."
      - Action: ""
        $name: Action
        $description: "Only used when Preset = Custom. See mod description for supported formats."
      - submenu:
          - - name: ""
              $name: Item name
            - icon: ""
              $name: Item icon
            - action: ""
              $name: Item action
        $name: Submenu items
        $description: "If any items are added here, the button will open a flyout instead of running the direct action."
    - - Preset: settings
    - - Preset: explorer
  $name: Buttons
  $description: "Only used when Preset = Custom. See mod description for supported formats."
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include <windows.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <powrprof.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shcore.h>
#include <wincrypt.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

#pragma comment(lib, "gdiplus.lib")
#include <gdiplus.h>

namespace wu   = winrt::Windows::UI;
namespace wux  = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxmi = winrt::Windows::UI::Xaml::Media::Imaging;
namespace wuc  = winrt::Windows::UI::Core;
namespace wss  = winrt::Windows::Storage::Streams;

static const wchar_t* CONTAINER_TAG      = L"WH_SMB_Container";
static const wchar_t* PROXY_WINDOW_CLASS = L"WH_SMB_Proxy_Class";
static const wchar_t* PROXY_WINDOW_NAME  = L"WH_SMB_Proxy_Window";
static const wchar_t* THREAD_CALL_MSG    = L"WH_SMB_ThreadCall";
static constexpr ULONG_PTR kCopyDataMagic = 0x534D4231;
static constexpr UINT_PTR  kRetryTimerId  = 0x574831AA;
static const wchar_t* FALLBACK_ICON      = L"\uE783";

static std::atomic<bool> g_initialized{false};
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_buttonsInjected{false};
static std::atomic<bool> g_monitoringActive{false};
static std::atomic<bool> g_forceRebuild{false};
static std::atomic<bool> g_closeStartMenu{true};
static std::atomic<bool> g_invertIconsSubmenus{false};

static std::mutex g_settingsMutex;

static ULONG_PTR g_gdiplusToken = 0;
static std::mutex g_gdipMutex;

enum class AccountButtonMode { Left, Center, Right, Hide };

struct Settings {
    wux::HorizontalAlignment alignment           = wux::HorizontalAlignment::Right;
    AccountButtonMode        accountButton       = AccountButtonMode::Left;
    bool                     invertButtons       = true;
    bool                     invertIconsSubmenus = false;
    bool                     closeStartMenu      = true;
    int                      buttonSpacing       = 0;
    int                      containerMarginLeft  = -16;
    int                      containerMarginRight = -16;
    bool                     langRussian         = false;
};

struct ActionItem {
    std::wstring name;
    std::wstring icon;
    std::wstring action;
    std::vector<ActionItem> submenu;
};

static Settings              g_settings;
static std::vector<ActionItem> g_buttons;

static winrt::weak_ref<wuxc::Panel>           g_parentPanel{nullptr};
static winrt::weak_ref<wux::FrameworkElement>  g_originalPowerButton{nullptr};
static winrt::weak_ref<wuxc::StackPanel>       g_buttonContainer{nullptr};

static HWND   g_proxyWindow   = NULL;
static HANDLE g_proxyThread   = NULL;
static DWORD  g_proxyThreadId = 0;
static HWND   g_retryHwnd     = NULL;

static winrt::event_token g_visibilityToken{};
static winrt::event_token g_activatedToken{};

static std::wstring Trim(std::wstring s) {
    auto isSpace = [](wchar_t c) { return !!iswspace(c); };
    while (!s.empty() && isSpace(s.front())) s.erase(s.begin());
    while (!s.empty() && isSpace(s.back()))  s.pop_back();
    return s;
}

static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) c = static_cast<wchar_t>(towlower(c));
    return s;
}

static bool StartsWithCI(const std::wstring& s, const std::wstring& prefix) {
    return s.size() >= prefix.size() &&
           _wcsnicmp(s.c_str(), prefix.c_str(), prefix.size()) == 0;
}

static std::wstring StripOuterQuotes(const std::wstring& s) {
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"')
        return s.substr(1, s.size() - 2);
    return s;
}

static void SafeFreeString(PCWSTR s) {
    if (s) Wh_FreeStringSetting(s);
}

static bool IsImagePath(const std::wstring& s) {
    if (s.size() < 4) return false;
    if (s.size() >= 2 && s[1] == L':')                    return true;
    if (s.size() >= 2 && s[0] == L'\\' && s[1] == L'\\') return true;
    std::wstring lo = ToLower(s);
    for (const wchar_t* ext : { L".png", L".ico", L".jpg", L".jpeg", L".bmp", L".webp" }) {
        size_t elen = wcslen(ext);
        if (lo.size() >= elen && lo.compare(lo.size() - elen, elen, ext) == 0)
            return true;
    }
    return false;
}

static bool IsExePath(const std::wstring& s) {
    if (s.size() < 5) return false;
    std::wstring lo = ToLower(s);
    for (const wchar_t* ext : { L".exe", L".dll" }) {
        size_t elen = wcslen(ext);
        if (lo.size() >= elen && lo.compare(lo.size() - elen, elen, ext) == 0)
            return true;
    }
    return false;
}

static std::wstring MakeFileUri(const std::wstring& path) {
    std::wstring result;
    result.reserve(path.size() + 8);
    result = L"file:///";
    for (wchar_t c : path) {
        if (c == L'\\') {
            result += L'/';
        } else if (c == L' ') {
            result += L"%20";
        } else {
            result += c;
        }
    }
    return result;
}

static std::wstring DecodeEscapes(const std::wstring& in) {
    std::wstring out;
    out.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i) {
        if (in[i] == L'\\' && i + 1 < in.size()) {
            wchar_t next = in[i + 1];
            if (next == L'u' && i + 5 < in.size()) {
                unsigned v = 0; bool ok = true;
                for (size_t j = i + 2; j < i + 6; ++j) {
                    wchar_t c = in[j]; v <<= 4;
                    if      (c >= L'0' && c <= L'9') v |= static_cast<unsigned>(c - L'0');
                    else if (c >= L'a' && c <= L'f') v |= static_cast<unsigned>(10 + c - L'a');
                    else if (c >= L'A' && c <= L'F') v |= static_cast<unsigned>(10 + c - L'A');
                    else { ok = false; break; }
                }
                if (ok) { out.push_back(static_cast<wchar_t>(v)); i += 5; continue; }
            }
            switch (next) {
                case L'\\': out.push_back(L'\\'); ++i; continue;
                case L'"':  out.push_back(L'"');  ++i; continue;
                case L'n':  out.push_back(L'\n'); ++i; continue;
                case L'r':  out.push_back(L'\r'); ++i; continue;
                case L't':  out.push_back(L'\t'); ++i; continue;
                default: break;
            }
        }
        out.push_back(in[i]);
    }
    return out;
}

static std::wstring NormalizeIconString(const std::wstring& raw) {
    std::wstring s = Trim(raw);
    if (s.empty()) return s;
    if (IsImagePath(s) || IsExePath(s)) return s;

    if (s.size() >= 2 && s[0] == L'/' && s[1] == L'u')
        return L"\\" + s.substr(1);

    if (s.size() == 4 && std::all_of(s.begin(), s.end(), [](wchar_t c) {
        return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F');
    }))
        return L"\\u" + s;

    if (s.size() == 6 && s[0] == L'\\' && s[1] == L'u' &&
        std::all_of(s.begin() + 2, s.end(), [](wchar_t c) {
            return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F');
        }))
        return s;

    return s;
}

static void EnsureGdiplus() {
    std::lock_guard<std::mutex> lk(g_gdipMutex);
    if (g_gdiplusToken == 0) {
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::GdiplusStartup(&g_gdiplusToken, &input, nullptr);
    }
}

static HICON LoadIconFromExe(const std::wstring& path, int size) {
    HICON hLarge = nullptr, hSmall = nullptr;
    if (ExtractIconExW(path.c_str(), 0, &hLarge, &hSmall, 1) == 0)
        return nullptr;

    if (size > 16) {
        if (hSmall) DestroyIcon(hSmall);
        return hLarge;
    } else {
        if (hLarge) DestroyIcon(hLarge);
        return hSmall;
    }
}

static std::wstring CreateDataUriFromIcon(HICON hIcon) {
    if (!hIcon) return L"";

    ICONINFO info{};
    if (!GetIconInfo(hIcon, &info)) return L"";

    struct BitmapGuard {
        HBITMAP color, mask;
        ~BitmapGuard() {
            if (color) DeleteObject(color);
            if (mask)  DeleteObject(mask);
        }
    } bmpGuard{ info.hbmColor, info.hbmMask };

    BITMAP bm{};
    GetObject(info.hbmColor ? info.hbmColor : info.hbmMask, sizeof(bm), &bm);

    HDC screenDC = GetDC(nullptr);
    HDC memDC    = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = bm.bmWidth;
    bmi.bmiHeader.biHeight      = -bm.bmHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

    std::wstring result;
    if (dib && bits) {
        HBITMAP oldBmp = static_cast<HBITMAP>(SelectObject(memDC, dib));
        DrawIconEx(memDC, 0, 0, hIcon, bm.bmWidth, bm.bmHeight, 0, nullptr, DI_NORMAL);
        SelectObject(memDC, oldBmp);

        try {
            wss::InMemoryRandomAccessStream stream;
            EnsureGdiplus();

            Gdiplus::Bitmap srcBmp(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4,
                                   PixelFormat32bppARGB, static_cast<BYTE*>(bits));

            if (srcBmp.GetLastStatus() == Gdiplus::Ok) {
                IStream* pStream = nullptr;
                if (SUCCEEDED(CreateStreamOverRandomAccessStream(
                    winrt::get_unknown(stream), IID_PPV_ARGS(&pStream)))) {

                    CLSID pngClsid;
                    CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid);

                    if (srcBmp.Save(pStream, &pngClsid, nullptr) == Gdiplus::Ok) {
                        stream.Seek(0);

                        uint64_t size = stream.Size();
                        if (size > 0 && size < 10 * 1024 * 1024) {
                            wss::Buffer buffer(static_cast<uint32_t>(size));
                            stream.ReadAsync(buffer, static_cast<uint32_t>(size), wss::InputStreamOptions::None).get();

                            auto dataReader = wss::DataReader::FromBuffer(buffer);
                            std::vector<uint8_t> pngData(static_cast<uint32_t>(size));
                            dataReader.ReadBytes(pngData);

                            DWORD base64Len = 0;
                            CryptBinaryToStringW(pngData.data(), static_cast<DWORD>(pngData.size()),
                                                 CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                                                 nullptr, &base64Len);

                            if (base64Len > 0) {
                                std::vector<wchar_t> base64(base64Len);
                                if (CryptBinaryToStringW(pngData.data(), static_cast<DWORD>(pngData.size()),
                                                         CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                                                         base64.data(), &base64Len)) {
                                    result = L"data:image/png;base64," + std::wstring(base64.data(), base64Len - 1);
                                }
                            }
                        }
                    }
                    pStream->Release();
                }
            }
        } catch (...) {
            Wh_Log(L"Exception creating data URI from icon");
        }

        DeleteObject(dib);
    }

    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
    return result;
}

static wuxmi::BitmapSource CreateBitmapSourceFromIcon(HICON hIcon) {
    if (!hIcon) return nullptr;

    ICONINFO info{};
    if (!GetIconInfo(hIcon, &info)) return nullptr;

    struct BitmapGuard {
        HBITMAP color, mask;
        ~BitmapGuard() {
            if (color) DeleteObject(color);
            if (mask)  DeleteObject(mask);
        }
    } bmpGuard{ info.hbmColor, info.hbmMask };

    BITMAP bm{};
    GetObject(info.hbmColor ? info.hbmColor : info.hbmMask, sizeof(bm), &bm);

    HDC screenDC = GetDC(nullptr);
    HDC memDC    = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = bm.bmWidth;
    bmi.bmiHeader.biHeight      = -bm.bmHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

    wuxmi::BitmapSource result = nullptr;
    if (dib && bits) {
        HBITMAP oldBmp = static_cast<HBITMAP>(SelectObject(memDC, dib));
        DrawIconEx(memDC, 0, 0, hIcon, bm.bmWidth, bm.bmHeight, 0, nullptr, DI_NORMAL);
        SelectObject(memDC, oldBmp);

        try {
            wss::InMemoryRandomAccessStream stream;

            EnsureGdiplus();

            Gdiplus::Bitmap srcBmp(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4,
                                   PixelFormat32bppARGB, static_cast<BYTE*>(bits));

            if (srcBmp.GetLastStatus() == Gdiplus::Ok) {
                IStream* pStream = nullptr;
                if (SUCCEEDED(CreateStreamOverRandomAccessStream(
                    winrt::get_unknown(stream), IID_PPV_ARGS(&pStream)))) {

                    CLSID pngClsid;
                    CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid);

                    if (srcBmp.Save(pStream, &pngClsid, nullptr) == Gdiplus::Ok) {
                        stream.Seek(0);

                        wuxmi::BitmapImage bmpImage;
                        bmpImage.SetSource(stream);
                        result = bmpImage;
                    }
                    pStream->Release();
                }
            }
        } catch (...) {
            Wh_Log(L"Exception creating in-memory bitmap from icon");
        }

        DeleteObject(dib);
    }

    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
    return result;
}

static std::wstring SaveIconToPng(HICON hIcon) {
    if (!hIcon) return L"";

    wchar_t tempDir[MAX_PATH], placeholder[MAX_PATH];
    if (!GetTempPathW(MAX_PATH, tempDir)) return L"";
    if (!GetTempFileNameW(tempDir, L"ICO", 0, placeholder)) return L"";

    struct AutoDelete {
        wchar_t path[MAX_PATH];
        ~AutoDelete() { DeleteFileW(path); }
    } guard;
    wcscpy_s(guard.path, placeholder);

    std::wstring pngPath = std::wstring(placeholder) + L".png";

    ICONINFO info{};
    if (!GetIconInfo(hIcon, &info)) return L"";

    struct BitmapGuard {
        HBITMAP color, mask;
        ~BitmapGuard() {
            if (color) DeleteObject(color);
            if (mask)  DeleteObject(mask);
        }
    } bmpGuard{ info.hbmColor, info.hbmMask };

    BITMAP bm{};
    GetObject(info.hbmColor ? info.hbmColor : info.hbmMask, sizeof(bm), &bm);

    HDC screenDC = GetDC(nullptr);
    HDC memDC    = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = bm.bmWidth;
    bmi.bmiHeader.biHeight      = -bm.bmHeight;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

    std::wstring result;
    if (dib && bits) {
        HBITMAP oldBmp = static_cast<HBITMAP>(SelectObject(memDC, dib));
        DrawIconEx(memDC, 0, 0, hIcon, bm.bmWidth, bm.bmHeight, 0, nullptr, DI_NORMAL);
        SelectObject(memDC, oldBmp);

        EnsureGdiplus();

        Gdiplus::Bitmap srcBmp(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4,
                               PixelFormat32bppARGB, static_cast<BYTE*>(bits));
        if (srcBmp.GetLastStatus() == Gdiplus::Ok) {
            CLSID pngClsid;
            CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid);
            if (srcBmp.Save(pngPath.c_str(), &pngClsid, nullptr) == Gdiplus::Ok)
                result = pngPath;
        }
        DeleteObject(dib);
    }

    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);

    return result;
}

static wuxmi::BitmapSource ResolveExeIconInMemory(const std::wstring& iconStr, int size) {
    if (iconStr.empty() || !IsExePath(iconStr)) return nullptr;
    if (GetFileAttributesW(iconStr.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Executable not found for icon: %s", iconStr.c_str());
        return nullptr;
    }
    HICON hIcon = LoadIconFromExe(iconStr, size);
    if (!hIcon) return nullptr;
    auto bmpSource = CreateBitmapSourceFromIcon(hIcon);
    DestroyIcon(hIcon);
    return bmpSource;
}

static std::wstring ResolveExeIcon(const std::wstring& iconStr, int size) {
    if (iconStr.empty() || !IsExePath(iconStr)) return iconStr;
    if (GetFileAttributesW(iconStr.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Executable not found for icon: %s", iconStr.c_str());
        return L"";
    }
    HICON hIcon = LoadIconFromExe(iconStr, size);
    if (!hIcon) return L"";
    std::wstring png = SaveIconToPng(hIcon);
    DestroyIcon(hIcon);
    return png;
}

static std::wstring GlyphOrEmpty(const std::wstring& s) {
    return (!s.empty() && !IsImagePath(s) && !IsExePath(s)) ? s : std::wstring{};
}

static wux::UIElement MakeButtonIcon(const std::wstring& iconStr) {
    if (IsExePath(iconStr)) {
        auto bmpSource = ResolveExeIconInMemory(iconStr, 32);
        if (bmpSource) {
            try {
                wuxc::Image img;
                img.Width(16); img.Height(16);
                img.Stretch(wuxm::Stretch::Uniform);
                img.Source(bmpSource);
                return img;
            } catch (...) {
                Wh_Log(L"Exception creating button icon from in-memory bitmap: %s", iconStr.c_str());
            }
        }
    }

    std::wstring resolved = ResolveExeIcon(iconStr, 32);

    if (!resolved.empty() && IsImagePath(resolved)) {
        if (GetFileAttributesW(resolved.c_str()) != INVALID_FILE_ATTRIBUTES) {
            try {
                wuxmi::BitmapImage bmp;
                bmp.DecodePixelWidth(32);
                bmp.DecodePixelHeight(32);
                bmp.UriSource(winrt::Windows::Foundation::Uri(MakeFileUri(resolved)));
                bmp.ImageFailed([resolved](auto&&, auto&&) {
                    Wh_Log(L"Button icon load failed: %s", resolved.c_str());
                });
                wuxc::Image img;
                img.Width(16); img.Height(16);
                img.Stretch(wuxm::Stretch::Uniform);
                img.Source(bmp);
                return img;
            } catch (...) {
                Wh_Log(L"Exception creating button image icon: %s", resolved.c_str());
            }
        } else {
            Wh_Log(L"Icon file not found: %s", resolved.c_str());
        }
    }

    wuxc::FontIcon fi;
    fi.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    fi.FontSize(16);
    std::wstring glyph = GlyphOrEmpty(resolved.empty() ? iconStr : resolved);
    fi.Glyph(!glyph.empty() ? glyph : FALLBACK_ICON);
    return fi;
}

static wuxc::IconElement MakeMenuIcon(const std::wstring& iconStr) {
    if (IsExePath(iconStr)) {
        if (GetFileAttributesW(iconStr.c_str()) != INVALID_FILE_ATTRIBUTES) {
            HICON hIcon = LoadIconFromExe(iconStr, 16);
            if (hIcon) {
                std::wstring dataUri = CreateDataUriFromIcon(hIcon);
                DestroyIcon(hIcon);

                if (!dataUri.empty()) {
                    try {
                        wuxc::BitmapIcon bi;
                        bi.UriSource(winrt::Windows::Foundation::Uri(dataUri));
                        bi.ShowAsMonochrome(false);
                        bi.Width(16); bi.Height(16);
                        return bi;
                    } catch (...) {
                        Wh_Log(L"Exception creating menu icon from data URI: %s", iconStr.c_str());
                    }
                }
            }
        }
    }

    std::wstring resolved = iconStr;
    if (!IsExePath(iconStr)) {
        resolved = ResolveExeIcon(iconStr, 16);
    }

    if (!resolved.empty() && IsImagePath(resolved)) {
        if (GetFileAttributesW(resolved.c_str()) != INVALID_FILE_ATTRIBUTES) {
            try {
                wuxc::BitmapIcon bi;
                bi.UriSource(winrt::Windows::Foundation::Uri(MakeFileUri(resolved)));
                bi.ShowAsMonochrome(false);
                bi.Width(16); bi.Height(16);
                return bi;
            } catch (...) {
                Wh_Log(L"Exception creating menu image icon: %s", resolved.c_str());
            }
        } else {
            Wh_Log(L"Menu icon file not found: %s", resolved.c_str());
        }
    }

    wuxc::FontIcon fi;
    fi.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    fi.FontSize(16);
    std::wstring glyph = GlyphOrEmpty(resolved.empty() ? iconStr : resolved);
    fi.Glyph(!glyph.empty() ? glyph : FALLBACK_ICON);
    return fi;
}

struct PresetDef {
    const wchar_t* key;
    const wchar_t* nameEn;
    const wchar_t* nameRu;
    const wchar_t* icon;
    const wchar_t* action;
};

static const PresetDef kPresets[] = {
    { L"settings",        L"Settings",        L"Параметры",         L"\uE713", L"__preset:settings"        },
    { L"explorer",        L"Explorer",        L"Проводник",         L"\uEC50", L"__preset:explorer"        },
    { L"documents",       L"Documents",       L"Документы",         L"\uE8A5", L"__preset:documents"       },
    { L"downloads",       L"Downloads",       L"Загрузки",          L"\uE896", L"__preset:downloads"       },
    { L"music",           L"Music",           L"Музыка",            L"\uEC4F", L"__preset:music"           },
    { L"pictures",        L"Pictures",        L"Изображения",       L"\uE91B", L"__preset:pictures"        },
    { L"videos",          L"Videos",          L"Видео",             L"\uE714", L"__preset:videos"          },
    { L"network",         L"Network",         L"Сеть",              L"\uEC27", L"__preset:network"         },
    { L"personal_folder", L"Personal Folder", L"Личная папка",      L"\uEC25", L"__preset:personal_folder" },
    { L"shutdown",        L"Shut down",       L"Завершение работы", L"\uE7E8", L"__preset:shutdown"        },
    { L"restart",         L"Restart",         L"Перезагрузка",      L"\uE777", L"__preset:restart"         },
    { L"sign_out",        L"Sign out",        L"Выйти",             L"\uF3B1", L"__preset:sign_out"        },
    { L"sleep",           L"Sleep",           L"Спящий режим",      L"\uE708", L"__preset:sleep"           },
    { L"hibernate",       L"Hibernate",       L"Гибернация",        L"\uE823", L"__preset:hibernate"       },
    { L"lock",            L"Lock",            L"Блокировка",        L"\uE72E", L"__preset:lock"            },
    { L"menu_shutdown",   L"Power",           L"Выключение",        L"\uE7E8", nullptr                     },
};
static constexpr int kPresetCount = static_cast<int>(std::size(kPresets));

static const PresetDef* FindPreset(const std::wstring& key) {
    std::wstring k = ToLower(Trim(key));
    if (k.empty() || k == L"custom") return nullptr;
    for (int i = 0; i < kPresetCount; ++i)
        if (k == kPresets[i].key) return &kPresets[i];
    Wh_Log(L"Unknown preset key: '%s'", key.c_str());
    return nullptr;
}

static std::wstring PresetName(const PresetDef& pd, bool russian) {
    return russian ? pd.nameRu : pd.nameEn;
}

static bool GetKnownFolderPath(const wchar_t* id, std::wstring& out) {
    std::wstring n = ToLower(id);
    KNOWNFOLDERID fid{};
    if      (n == L"downloads")                     fid = FOLDERID_Downloads;
    else if (n == L"documents" || n == L"personal") fid = FOLDERID_Documents;
    else if (n == L"music")                         fid = FOLDERID_Music;
    else if (n == L"pictures")                      fid = FOLDERID_Pictures;
    else if (n == L"videos")                        fid = FOLDERID_Videos;
    else if (n == L"desktop")                       fid = FOLDERID_Desktop;
    else if (n == L"profile" || n == L"home")       fid = FOLDERID_Profile;
    else return false;

    PWSTR raw = nullptr;
    bool ok = SUCCEEDED(SHGetKnownFolderPath(fid, 0, nullptr, &raw)) && raw;
    if (ok) out = raw;
    CoTaskMemFree(raw);
    return ok;
}

static void OpenKnownFolder(const wchar_t* name) {
    std::wstring path;
    if (GetKnownFolderPath(name, path))
        ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static bool EnableShutdownPrivilege() {
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;
    TOKEN_PRIVILEGES tkp{};
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    bool ok = LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME,
                                    &tkp.Privileges[0].Luid) &&
              AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, nullptr, nullptr);
    CloseHandle(hToken);
    return ok;
}

static bool PerformPresetAction(const std::wstring& action) {
    if (!StartsWithCI(action, L"__preset:")) return false;
    std::wstring key = ToLower(action.substr(9));

    if      (key == L"settings")        ShellExecuteW(nullptr, L"open", L"ms-settings:", nullptr, nullptr, SW_SHOWNORMAL);
    else if (key == L"explorer")        ShellExecuteW(nullptr, L"open", L"explorer.exe", nullptr, nullptr, SW_SHOWNORMAL);
    else if (key == L"documents")       OpenKnownFolder(L"documents");
    else if (key == L"downloads")       OpenKnownFolder(L"downloads");
    else if (key == L"music")           OpenKnownFolder(L"music");
    else if (key == L"pictures")        OpenKnownFolder(L"pictures");
    else if (key == L"videos")          OpenKnownFolder(L"videos");
    else if (key == L"network")         ShellExecuteW(nullptr, L"open", L"shell:NetworkPlacesFolder", nullptr, nullptr, SW_SHOWNORMAL);
    else if (key == L"personal_folder") OpenKnownFolder(L"profile");
    else if (key == L"shutdown")        { EnableShutdownPrivilege(); ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER); }
    else if (key == L"restart")         { EnableShutdownPrivilege(); ExitWindowsEx(EWX_REBOOT  | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER); }
    else if (key == L"sign_out")        { EnableShutdownPrivilege(); ExitWindowsEx(EWX_LOGOFF  | EWX_FORCE, 0); }
    else if (key == L"sleep")           SetSuspendState(FALSE, FALSE, FALSE);
    else if (key == L"hibernate")       SetSuspendState(TRUE,  FALSE, FALSE);
    else if (key == L"lock")            LockWorkStation();
    else return false;

    return true;
}

static bool SearchRecursive(const std::wstring& root, const std::wstring& name,
                             int depth, std::wstring& out) {
    if (depth < 0) return false;
    std::wstring mask = root;
    if (!mask.empty() && mask.back() != L'\\') mask += L'\\';
    mask += L'*';

    WIN32_FIND_DATAW fd{};
    HANDLE h = FindFirstFileW(mask.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return false;

    bool found = false;
    do {
        if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..")) continue;

        std::wstring full = root;
        if (!full.empty() && full.back() != L'\\') full += L'\\';
        full += fd.cFileName;

        if (!_wcsicmp(fd.cFileName, name.c_str())) {
            out = full; found = true; break;
        }
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
            if (SearchRecursive(full, name, depth - 1, out)) { found = true; break; }
        }
    } while (FindNextFileW(h, &fd));

    FindClose(h);
    return found;
}

static bool SearchByName(const std::wstring& name, std::wstring& out) {
    wchar_t buf[MAX_PATH * 4]{};
    if (SearchPathW(nullptr, name.c_str(), nullptr, ARRAYSIZE(buf), buf, nullptr)) {
        out = buf; return true;
    }

    std::vector<std::wstring> roots;
    auto addEnvDir = [&](const wchar_t* var) {
        wchar_t tmp[MAX_PATH * 2]{};
        if (GetEnvironmentVariableW(var, tmp, ARRAYSIZE(tmp))) roots.emplace_back(tmp);
    };
    addEnvDir(L"ProgramFiles"); addEnvDir(L"ProgramFiles(x86)"); addEnvDir(L"ProgramW6432");
    addEnvDir(L"SystemRoot");   addEnvDir(L"LOCALAPPDATA");       addEnvDir(L"APPDATA");
    addEnvDir(L"USERPROFILE");
    for (const wchar_t* folder : { L"desktop", L"documents", L"downloads",
                                    L"music",   L"pictures",  L"videos" }) {
        std::wstring p;
        if (GetKnownFolderPath(folder, p)) roots.push_back(std::move(p));
    }

    for (const auto& r : roots)
        if (SearchRecursive(r, name, 5, out)) return true;
    return false;
}

static WORD ResolveKeyToken(const std::wstring& tok) {
    if (tok.empty()) return 0;
    std::wstring lo = ToLower(tok);

    struct { const wchar_t* name; WORD vk; } named[] = {
        { L"ctrl",       VK_CONTROL  }, { L"alt",        VK_MENU    },
        { L"shift",      VK_SHIFT    }, { L"win",        VK_LWIN    },
        { L"enter",      VK_RETURN   }, { L"esc",        VK_ESCAPE  },
        { L"escape",     VK_ESCAPE   }, { L"tab",        VK_TAB     },
        { L"space",      VK_SPACE    }, { L"backspace",  VK_BACK    },
        { L"delete",     VK_DELETE   }, { L"del",        VK_DELETE  },
        { L"insert",     VK_INSERT   }, { L"ins",        VK_INSERT  },
        { L"home",       VK_HOME     }, { L"end",        VK_END     },
        { L"pageup",     VK_PRIOR    }, { L"pgup",       VK_PRIOR   },
        { L"pagedown",   VK_NEXT     }, { L"pgdn",       VK_NEXT    },
        { L"left",       VK_LEFT     }, { L"right",      VK_RIGHT   },
        { L"up",         VK_UP       }, { L"down",       VK_DOWN    },
        { L"printscreen",VK_SNAPSHOT }, { L"pause",      VK_PAUSE   },
        { L"capslock",   VK_CAPITAL  }, { L"numlock",    VK_NUMLOCK },
        { L"scrolllock", VK_SCROLL   }, { L"sleep",      VK_SLEEP   },
    };
    for (auto& e : named)
        if (lo == e.name) return e.vk;

    if (lo.size() >= 2 && lo[0] == L'f') {
        wchar_t* end = nullptr;
        long n = wcstol(lo.c_str() + 1, &end, 10);
        if (end && *end == L'\0' && n >= 1 && n <= 24)
            return static_cast<WORD>(VK_F1 + n - 1);
    }

    if (tok.size() >= 2 && tok[0] == L'0' && (tok[1] == L'x' || tok[1] == L'X')) {
        long v = wcstol(tok.c_str(), nullptr, 16);
        if (v > 0 && v <= 0xFF) return static_cast<WORD>(v);
        Wh_Log(L"press: hex VK out of range: %s", tok.c_str());
        return 0;
    }

    if (iswdigit(tok[0])) {
        long v = wcstol(tok.c_str(), nullptr, 10);
        if (v > 0 && v <= 0xFF) return static_cast<WORD>(v);
        Wh_Log(L"press: decimal VK out of range: %s", tok.c_str());
        return 0;
    }

    if (tok.size() == 1) {
        wchar_t c = towupper(tok[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9'))
            return static_cast<WORD>(c);
    }

    Wh_Log(L"press: unrecognised token '%s'", tok.c_str());
    return 0;
}

static void SendVirtualKeypress(const std::wstring& keysStr) {
    std::vector<WORD> keys;
    std::wstring token;

    auto flush = [&]() {
        token = Trim(token);
        if (!token.empty()) {
            WORD vk = ResolveKeyToken(token);
            if (vk) keys.push_back(vk);
            token.clear();
        }
    };

    for (size_t i = 0; i < keysStr.size(); ++i) {
        wchar_t c = keysStr[i];
        if (c == L'+' || c == L';') {
            flush();
        } else {
            token += c;
        }
    }
    flush();

    if (keys.empty()) {
        Wh_Log(L"press: no valid codes in '%s'", keysStr.c_str());
        return;
    }

    for (WORD vk : keys)      { keybd_event(static_cast<BYTE>(vk), 0, 0, 0); Sleep(10); }
    Sleep(100);
    for (size_t i = keys.size(); i > 0; --i)
        { keybd_event(static_cast<BYTE>(keys[i - 1]), 0, KEYEVENTF_KEYUP, 0); Sleep(10); }
}

struct ParsedAction {
    std::wstring body;
    bool         runas;
    bool         showWindow;
};

static ParsedAction ParseActionSigns(const std::wstring& raw) {
    ParsedAction r{ raw, false, false };
    size_t i = 0;
    while (i < raw.size()) {
        if      (raw[i] == L'-') { r.runas      = true; ++i; }
        else if (raw[i] == L'*') { r.showWindow = true; ++i; }
        else break;
    }
    r.body = raw.substr(i);
    return r;
}

static bool ExecuteProcess(const std::wstring& cmd, bool useCmdExe, bool showWindow) {
    std::wstring line = useCmdExe ? (L"cmd.exe /C " + cmd) : cmd;
    std::vector<wchar_t> buf(line.begin(), line.end());
    buf.push_back(L'\0');

    STARTUPINFOW si{}; si.cb = sizeof(si);
    si.dwFlags     = STARTF_USESHOWWINDOW;
    si.wShowWindow = showWindow ? SW_NORMAL : SW_HIDE;

    PROCESS_INFORMATION pi{};
    DWORD flags = showWindow ? 0 : CREATE_NO_WINDOW;
    if (!CreateProcessW(nullptr, buf.data(), nullptr, nullptr,
                        FALSE, flags, nullptr, nullptr, &si, &pi))
        return false;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

static void ExecuteActionText(const std::wstring& raw) {
    std::wstring a = Trim(raw);
    if (a.empty()) return;

    if (PerformPresetAction(a)) return;

    std::thread([a]() {
        auto [action, runas, showWindow] = ParseActionSigns(a);
        const LPCWSTR verb = runas ? L"runas" : L"open";

        if (StartsWithCI(action, L"press:")) {
            Sleep(150);
            SendVirtualKeypress(action.substr(6));
            return;
        }

        if (StartsWithCI(action, L"cmd:")) {
            std::wstring cmd = Trim(action.substr(4));
            if (showWindow)
                ShellExecuteW(nullptr, verb, L"cmd.exe",
                              (L"/K " + cmd).c_str(), nullptr, SW_NORMAL);
            else
                ExecuteProcess(cmd, true, false);
            return;
        }

        if (StartsWithCI(action, L"shell:")) {
            std::wstring ps = Trim(action.substr(6));
            std::wstring args = L"-NoProfile -ExecutionPolicy Bypass -Command " + ps;
            if (showWindow) args = L"-NoExit " + args;
            ShellExecuteW(nullptr, verb, L"powershell.exe",
                          args.c_str(), nullptr, showWindow ? SW_NORMAL : SW_HIDE);
            return;
        }

        if (StartsWithCI(action, L"ms-settings:")) {
            ShellExecuteW(nullptr, verb, action.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }

        if (StartsWithCI(action, L"web:")) {
            ShellExecuteW(nullptr, verb, Trim(action.substr(4)).c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }

        if (!action.empty() && action.front() == L'"') {
            ShellExecuteW(nullptr, verb, StripOuterQuotes(action).c_str(),
                          nullptr, nullptr, SW_SHOWNORMAL);
            return;
        }

        if (!action.empty() && action.front() == L'~') {
            std::wstring tgt = Trim(action.substr(1));
            if (tgt.empty()) { Wh_Log(L"~search: empty target"); return; }
            std::wstring resolved;
            if (GetKnownFolderPath(tgt.c_str(), resolved) ||
                SearchByName(tgt, resolved))
                ShellExecuteW(nullptr, verb, resolved.c_str(),
                              nullptr, nullptr, SW_SHOWNORMAL);
            else
                Wh_Log(L"~search: '%s' not found", tgt.c_str());
            return;
        }

        ExecuteProcess(action, false, showWindow);
    }).detach();
}

static void SendEscapeKey() {
    INPUT in[2]{};
    in[0].type = in[1].type = INPUT_KEYBOARD;
    in[0].ki.wVk = in[1].ki.wVk = VK_ESCAPE;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

static void ScheduleEscapeKeypress(DWORD delayMs = 100) {
    auto fire = [delayMs]() {
        std::thread([delayMs] { Sleep(delayMs); SendEscapeKey(); }).detach();
    };
    try {
        auto dq = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (dq) { dq.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, fire); return; }
    } catch (...) {}
    fire();
}

static void CloseStartMenuAfterClick() {
    if (g_closeStartMenu.load(std::memory_order_relaxed))
        ScheduleEscapeKeypress(100);
}

static void CloseStartMenuForPress() {
    ScheduleEscapeKeypress(100);
}

static LRESULT CALLBACK ProxyWndProc(HWND hWnd, UINT msg,
                                     WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COPYDATA) {
        const auto* cds = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
        if (cds && cds->dwData == kCopyDataMagic && cds->cbData == sizeof(int)) {
            int idx = *static_cast<const int*>(cds->lpData);
            std::lock_guard<std::mutex> lk(g_settingsMutex);
            if (idx >= 0 && idx < static_cast<int>(g_buttons.size()))
                ExecuteActionText(g_buttons[idx].action);
        }
        return 0;
    }
    switch (msg) {
        case WM_CLOSE:   DestroyWindow(hWnd); return 0;
        case WM_DESTROY: g_proxyWindow = NULL; PostQuitMessage(0); return 0;
        default: return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}

static DWORD WINAPI ProxyWindowThread(LPVOID) {
    WNDCLASSEXW wcex{};
    wcex.cbSize        = sizeof(wcex);
    wcex.lpfnWndProc   = ProxyWndProc;
    wcex.hInstance     = GetModuleHandleW(nullptr);
    wcex.lpszClassName = PROXY_WINDOW_CLASS;

    if (!RegisterClassExW(&wcex)) {
        DWORD err = GetLastError();
        if (err != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"Proxy: RegisterClassEx failed %lu", err);
            return 1;
        }
    }

    g_proxyWindow = CreateWindowExW(
        0, PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME, 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!g_proxyWindow) {
        Wh_Log(L"Proxy: CreateWindowEx failed %lu", GetLastError());
        UnregisterClassW(PROXY_WINDOW_CLASS, GetModuleHandleW(nullptr));
        return 1;
    }

    MSG m{};
    while (GetMessageW(&m, nullptr, 0, 0)) {
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }

    UnregisterClassW(PROXY_WINDOW_CLASS, GetModuleHandleW(nullptr));
    g_proxyWindow = NULL;
    return 0;
}

static void StartProxyThread() {
    if (g_proxyThread) return;
    g_proxyThread = CreateThread(nullptr, 0, ProxyWindowThread,
                                 nullptr, 0, &g_proxyThreadId);
    if (!g_proxyThread)
        Wh_Log(L"Proxy: CreateThread failed %lu", GetLastError());
}

static bool SendActionToProxy(int idx) {
    HWND hw = FindWindowW(PROXY_WINDOW_CLASS, PROXY_WINDOW_NAME);
    if (!hw) {
        std::lock_guard<std::mutex> lk(g_settingsMutex);
        if (idx >= 0 && idx < static_cast<int>(g_buttons.size()))
            ExecuteActionText(g_buttons[idx].action);
        return false;
    }
    COPYDATASTRUCT cds{ kCopyDataMagic, sizeof(int), &idx };
    return SendMessageW(hw, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&cds)) != 0;
}

static void LoadSettings() {
    Settings s;

    PCWSTR lang = Wh_GetStringSetting(L"preset_language");
    s.langRussian = lang && _wcsicmp(lang, L"ru") == 0;
    SafeFreeString(lang);

    PCWSTR align = Wh_GetStringSetting(L"alignment");
    if      (align && _wcsicmp(align, L"left")   == 0) s.alignment = wux::HorizontalAlignment::Left;
    else if (align && _wcsicmp(align, L"center") == 0) s.alignment = wux::HorizontalAlignment::Center;
    else                                                s.alignment = wux::HorizontalAlignment::Right;
    SafeFreeString(align);

    PCWSTR acct = Wh_GetStringSetting(L"account_button");
    if      (acct && _wcsicmp(acct, L"center") == 0) s.accountButton = AccountButtonMode::Center;
    else if (acct && _wcsicmp(acct, L"right")  == 0) s.accountButton = AccountButtonMode::Right;
    else if (acct && _wcsicmp(acct, L"hide")   == 0) s.accountButton = AccountButtonMode::Hide;
    else                                              s.accountButton = AccountButtonMode::Left;
    SafeFreeString(acct);

    s.invertButtons          = Wh_GetIntSetting(L"invert_buttons")        != 0;
    s.invertIconsSubmenus    = Wh_GetIntSetting(L"invert_icons_submenus") != 0;
    s.closeStartMenu         = Wh_GetIntSetting(L"close_start_menu")      != 0;
    s.buttonSpacing          = Wh_GetIntSetting(L"button_spacing");
    s.containerMarginLeft    = Wh_GetIntSetting(L"container_margin_left");
    s.containerMarginRight   = Wh_GetIntSetting(L"container_margin_right");

    g_closeStartMenu.store(s.closeStartMenu,         std::memory_order_relaxed);
    g_invertIconsSubmenus.store(s.invertIconsSubmenus, std::memory_order_relaxed);

    std::lock_guard<std::mutex> lk(g_settingsMutex);
    g_settings = s;

    Wh_Log(L"Settings loaded: lang=%s align=%d acct=%d invertIcons=%d",
           s.langRussian ? L"ru" : L"en",
           static_cast<int>(s.alignment),
           static_cast<int>(s.accountButton),
           static_cast<int>(s.invertIconsSubmenus));
}

static std::vector<ActionItem> LoadSubmenu(int btnIdx) {
    std::vector<ActionItem> items;
    for (int j = 0; j < 64; ++j) {
        PCWSTR n  = Wh_GetStringSetting(L"buttons[%d].submenu[%d].name",   btnIdx, j);
        PCWSTR ic = Wh_GetStringSetting(L"buttons[%d].submenu[%d].icon",   btnIdx, j);
        PCWSTR ac = Wh_GetStringSetting(L"buttons[%d].submenu[%d].action", btnIdx, j);

        std::wstring name   = n ? n : L"";
        std::wstring icon   = DecodeEscapes(NormalizeIconString(ic ? ic : L""));
        std::wstring action = ac ? ac : L"";

        SafeFreeString(n); SafeFreeString(ic); SafeFreeString(ac);

        if (Trim(name).empty()) break;

        ActionItem item;
        item.name   = Trim(name);
        item.icon   = Trim(icon);
        item.action = Trim(action);
        items.push_back(std::move(item));
    }
    return items;
}

static void BuildButtons() {
    bool ruLang;
    { std::lock_guard<std::mutex> lk(g_settingsMutex); ruLang = g_settings.langRussian; }

    std::vector<ActionItem> newButtons;

    for (int i = 0; i < 128; ++i) {
        PCWSTR presetRaw = Wh_GetStringSetting(L"buttons[%d].Preset", i);
        PCWSTR nRaw      = Wh_GetStringSetting(L"buttons[%d].Name",   i);
        PCWSTR iRaw      = Wh_GetStringSetting(L"buttons[%d].Icon",   i);
        PCWSTR aRaw      = Wh_GetStringSetting(L"buttons[%d].Action", i);

        std::wstring preset    = presetRaw ? presetRaw : L"";
        std::wstring rawName   = nRaw ? nRaw : L"";
        std::wstring rawIcon   = DecodeEscapes(NormalizeIconString(iRaw ? iRaw : L""));
        std::wstring rawAction = aRaw ? aRaw : L"";

        SafeFreeString(presetRaw); SafeFreeString(nRaw);
        SafeFreeString(iRaw);      SafeFreeString(aRaw);

        std::wstring presetKey = ToLower(Trim(preset));
        bool isCustom = (presetKey == L"custom" || presetKey.empty());

        if (isCustom) {
            bool allEmpty = Trim(rawName).empty() &&
                            Trim(rawIcon).empty() &&
                            Trim(rawAction).empty();
            if (presetKey.empty() && allEmpty) break;
            if (allEmpty) continue;
        }

        const PresetDef* pd = FindPreset(preset);
        auto userSub = LoadSubmenu(i);

        ActionItem item;

        if (pd) {
            item.name = Trim(rawName).empty() ? PresetName(*pd, ruLang) : Trim(rawName);
            item.icon = Trim(rawIcon).empty()  ? pd->icon               : Trim(rawIcon);

            if (!userSub.empty()) {
                item.submenu = std::move(userSub);
            } else if (pd->action) {
                item.action = pd->action;
            } else {
                for (const wchar_t* key : { L"lock", L"sleep", L"shutdown", L"restart" }) {
                    const PresetDef* sp = FindPreset(key);
                    if (!sp) continue;
                    ActionItem si;
                    si.name   = PresetName(*sp, ruLang);
                    si.icon   = sp->icon;
                    si.action = sp->action;
                    item.submenu.push_back(std::move(si));
                }
            }
        } else {
            item.name   = Trim(rawName);
            item.icon   = Trim(rawIcon).empty() ? FALLBACK_ICON : Trim(rawIcon);
            item.action = Trim(rawAction);
            if (!userSub.empty()) item.submenu = std::move(userSub);
        }

        newButtons.push_back(std::move(item));
    }

    std::lock_guard<std::mutex> lk(g_settingsMutex);
    g_buttons = std::move(newButtons);
}

static bool IsExplorerProcess() {
    WCHAR path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    return ToLower(path).find(L"explorer.exe") != std::wstring::npos;
}

static wux::FrameworkElement FindByName(wux::DependencyObject root,
                                        const wchar_t* name) {
    if (!root) return nullptr;
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto fe = child.try_as<wux::FrameworkElement>())
            if (fe.Name() == name) return fe;
        auto found = FindByName(child, name);
        if (found) return found;
    }
    return nullptr;
}

static void AddMenuItems(wuxc::MenuFlyout flyout,
                         const std::vector<ActionItem>& items,
                         bool invertIcons) {
    for (const auto& item : items) {
        wuxc::MenuFlyoutItem mi;
        mi.Text(item.name);
        mi.Icon(MakeMenuIcon(item.icon.empty() ? std::wstring(FALLBACK_ICON) : item.icon));
        if (invertIcons)
            mi.FlowDirection(wux::FlowDirection::RightToLeft);

        std::wstring act = item.action;
        mi.Click([act](auto&&, auto&&) {
            if (StartsWithCI(act, L"press:"))
                CloseStartMenuForPress();
            else
                CloseStartMenuAfterClick();
            ExecuteActionText(act);
        });
        flyout.Items().Append(mi);
    }
}

static void InjectButtons(wuxc::Panel parentPanel,
                           wux::FrameworkElement) {
    try {
        Settings cur;
        std::vector<ActionItem> btns;
        {
            std::lock_guard<std::mutex> lk(g_settingsMutex);
            cur  = g_settings;
            btns = g_buttons;
        }

        std::vector<int> origIdx(btns.size());
        std::iota(origIdx.begin(), origIdx.end(), 0);
        if (cur.invertButtons) {
            std::reverse(btns.begin(), btns.end());
            std::reverse(origIdx.begin(), origIdx.end());
        }

        auto children = parentPanel.Children();
        for (uint32_t i = 0; i < children.Size(); ++i) {
            auto fe = children.GetAt(i).try_as<wux::FrameworkElement>();
            if (!fe) continue;

            if (winrt::unbox_value_or<winrt::hstring>(fe.Tag(), L"") == CONTAINER_TAG)
                continue;

            const winrt::hstring& n = fe.Name();
            bool isUserTile    = (n == L"UserTileButton" || n == L"UserTile" || n == L"ProfileButton");
            bool isPowerButton = (n == L"PowerButton");

            if (isPowerButton) {
                fe.Visibility(wux::Visibility::Collapsed);
                continue;
            }
            if (isUserTile) {
                switch (cur.accountButton) {
                    case AccountButtonMode::Hide:
                        fe.Visibility(wux::Visibility::Collapsed);
                        break;
                    case AccountButtonMode::Left:
                        fe.HorizontalAlignment(wux::HorizontalAlignment::Left);
                        fe.Visibility(wux::Visibility::Visible);
                        break;
                    case AccountButtonMode::Center:
                        fe.HorizontalAlignment(wux::HorizontalAlignment::Center);
                        fe.Visibility(wux::Visibility::Visible);
                        break;
                    case AccountButtonMode::Right:
                        fe.HorizontalAlignment(wux::HorizontalAlignment::Right);
                        fe.Visibility(wux::Visibility::Visible);
                        break;
                }
                continue;
            }
            fe.Visibility(wux::Visibility::Collapsed);
        }

        wuxc::StackPanel container = g_buttonContainer.get();
        if (container) {
            try {
                if (wuxm::VisualTreeHelper::GetParent(container) != parentPanel) {
                    container = nullptr;
                    g_buttonContainer = nullptr;
                }
            } catch (...) {
                container = nullptr;
                g_buttonContainer = nullptr;
            }
        }
        if (!container) {
            for (uint32_t i = 0; i < children.Size(); ++i) {
                if (auto sp = children.GetAt(i).try_as<wuxc::StackPanel>()) {
                    if (winrt::unbox_value_or<winrt::hstring>(sp.Tag(), L"") == CONTAINER_TAG) {
                        container = sp;
                        g_buttonContainer = sp;
                        break;
                    }
                }
            }
        }
        if (!container) {
            container = wuxc::StackPanel();
            container.Tag(winrt::box_value(CONTAINER_TAG));
            container.Orientation(wuxc::Orientation::Horizontal);
            container.VerticalAlignment(wux::VerticalAlignment::Center);
            parentPanel.Children().Append(container);
            g_buttonContainer = container;
        }

        container.Visibility(wux::Visibility::Visible);
        container.HorizontalAlignment(cur.alignment);
        container.Margin({ static_cast<double>(cur.containerMarginLeft),  0,
                           static_cast<double>(cur.containerMarginRight), 0 });

        bool needRebuild = g_forceRebuild.exchange(false) ||
                           container.Children().Size() != static_cast<uint32_t>(btns.size());
        if (!needRebuild) return;

        container.Children().Clear();

        for (size_t i = 0; i < btns.size(); ++i) {
            const auto& def   = btns[i];
            int          btnIdx = origIdx[i];

            wuxc::Button btn;
            btn.Width(40); btn.Height(40);
            double rightMargin = (i + 1 < btns.size()) ? static_cast<double>(cur.buttonSpacing) : 0.0;
            btn.Margin({ 0, 0, rightMargin, 0 });
            btn.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
            btn.BorderThickness({ 0, 0, 0, 0 });
            btn.CornerRadius({ 4, 4, 4, 4 });
            btn.Padding({ 0, 0, 0, 0 });
            btn.Content(MakeButtonIcon(def.icon.empty() ? std::wstring(FALLBACK_ICON) : def.icon));
            wuxc::ToolTipService::SetToolTip(btn, winrt::box_value(def.name));

            if (!def.submenu.empty()) {
                auto sub        = def.submenu;
                bool invertIcons = cur.invertIconsSubmenus;
                btn.Click([sub, btn, invertIcons](auto&&, auto&&) mutable {
                    wuxc::MenuFlyout flyout;
                    AddMenuItems(flyout, sub, invertIcons);
                    flyout.ShowAt(btn);
                });
            } else {
                std::wstring act = def.action;
                btn.Click([act, btnIdx](auto&&, auto&&) {
                    if (StartsWithCI(act, L"press:"))
                        CloseStartMenuForPress();
                    else
                        CloseStartMenuAfterClick();
                    SendActionToProxy(btnIdx);
                });
            }

            container.Children().Append(btn);
        }

    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"InjectButtons: WinRT 0x%08X %s",
               static_cast<unsigned>(ex.code()), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"InjectButtons: unknown exception");
    }
}

static bool TryInjectButtons() {
    if (g_unloading) return false;
    try {
        auto window = wux::Window::Current();
        if (!window) return false;
        auto content = window.Content();
        if (!content) return false;

        auto powerBtn = FindByName(content, L"PowerButton");
        if (!powerBtn) return false;

        auto parentObj = wuxm::VisualTreeHelper::GetParent(powerBtn);
        if (!parentObj) return false;
        auto parentPanel = parentObj.try_as<wuxc::Panel>();
        if (!parentPanel) return false;

        g_parentPanel         = parentPanel;
        g_originalPowerButton = powerBtn;
        InjectButtons(parentPanel, powerBtn);
        g_buttonsInjected = true;
        return true;
    } catch (...) {
        Wh_Log(L"TryInjectButtons: exception");
        return false;
    }
}

static void OnWindowVisible() {
    if (g_unloading) return;
    if (g_buttonsInjected && !g_forceRebuild) return;

    try {
        auto dq = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (dq) {
            dq.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, []() {
                if (!g_unloading) TryInjectButtons();
            });
        }
    } catch (...) {}
}

static bool StartVisibilityMonitoring() {
    if (g_monitoringActive) return true;
    try {
        wuc::CoreWindow cw = wuc::CoreWindow::GetForCurrentThread();
        if (!cw) return false;

        g_visibilityToken = cw.VisibilityChanged([](auto&&, auto&& a) {
            if (a.Visible()) OnWindowVisible();
        });
        g_activatedToken = cw.Activated([](auto&&, auto&& a) {
            if (a.WindowActivationState() != wuc::CoreWindowActivationState::Deactivated)
                OnWindowVisible();
        });

        g_monitoringActive = true;
        if (cw.Visible()) OnWindowVisible();
        return true;
    } catch (...) { return false; }
}

static void StopVisibilityMonitoring() {
    try {
        wuc::CoreWindow cw = wuc::CoreWindow::GetForCurrentThread();
        if (cw) {
            if (g_visibilityToken) cw.VisibilityChanged(std::exchange(g_visibilityToken, {}));
            if (g_activatedToken)  cw.Activated(std::exchange(g_activatedToken, {}));
        }
    } catch (...) {}

    try {
        auto panel     = g_parentPanel.get();
        auto container = g_buttonContainer.get();
        auto origBtn   = g_originalPowerButton.get();

        if (panel && container) {
            uint32_t idx;
            if (panel.Children().IndexOf(container, idx))
                panel.Children().RemoveAt(idx);
            auto ch = panel.Children();
            for (uint32_t i = 0; i < ch.Size(); ++i) {
                if (auto fe = ch.GetAt(i).try_as<wux::FrameworkElement>())
                    if (fe.Visibility() == wux::Visibility::Collapsed)
                        fe.Visibility(wux::Visibility::Visible);
            }
        }
        if (origBtn) origBtn.Visibility(wux::Visibility::Visible);
    } catch (...) {}

    g_parentPanel         = nullptr;
    g_originalPowerButton = nullptr;
    g_buttonContainer     = nullptr;
    g_monitoringActive    = false;
}

static void CALLBACK RetryTimerProc(HWND hwnd, UINT, UINT_PTR id, DWORD) {
    KillTimer(hwnd, id);
    if (g_unloading) return;
    g_buttonsInjected = false;
    HWND h = g_retryHwnd ? g_retryHwnd : hwnd;
    if (!StartVisibilityMonitoring())
        SetTimer(h, kRetryTimerId, 100, RetryTimerProc);
}

static void InitWithRetry(HWND hwnd) {
    if (!g_initialized.exchange(true)) {
        LoadSettings();
        BuildButtons();
    }
    if (hwnd) g_retryHwnd = hwnd;
    if (!StartVisibilityMonitoring())
        SetTimer(hwnd, kRetryTimerId, 100, RetryTimerProc);
}

using CreateWindowInBand_t = HWND(WINAPI*)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, PVOID, DWORD);
static CreateWindowInBand_t CreateWindowInBand_Original;

static HWND WINAPI CreateWindowInBand_Hook(
    DWORD exStyle, LPCWSTR cls, LPCWSTR name, DWORD style,
    int x, int y, int w, int h,
    HWND parent, HMENU menu, HINSTANCE inst, PVOID param, DWORD band)
{
    HWND hWnd = CreateWindowInBand_Original(
        exStyle, cls, name, style, x, y, w, h,
        parent, menu, inst, param, band);
    if (hWnd && cls && _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0)
        InitWithRetry(hWnd);
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, PVOID, DWORD, DWORD);
static CreateWindowInBandEx_t CreateWindowInBandEx_Original;

static HWND WINAPI CreateWindowInBandEx_Hook(
    DWORD exStyle, LPCWSTR cls, LPCWSTR name, DWORD style,
    int x, int y, int w, int h,
    HWND parent, HMENU menu, HINSTANCE inst, PVOID param,
    DWORD band, DWORD typeFlags)
{
    HWND hWnd = CreateWindowInBandEx_Original(
        exStyle, cls, name, style, x, y, w, h,
        parent, menu, inst, param, band, typeFlags);
    if (hWnd && cls && _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0)
        InitWithRetry(hWnd);
    return hWnd;
}

using ThreadCallProc_t = void(WINAPI*)(PVOID);

static bool CallOnWindowThread(HWND hWnd, ThreadCallProc_t proc, PVOID param) {
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) { proc(param); return true; }

    struct CallPacket { ThreadCallProc_t proc; PVOID param; };
    static const UINT msg = RegisterWindowMessageW(THREAD_CALL_MSG);

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int code, WPARAM wp, LPARAM lp) -> LRESULT {
            if (code == HC_ACTION) {
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lp);
                if (cwp->message == RegisterWindowMessageW(THREAD_CALL_MSG)) {
                    auto* pkt = reinterpret_cast<CallPacket*>(cwp->lParam);
                    pkt->proc(pkt->param);
                }
            }
            return CallNextHookEx(nullptr, code, wp, lp);
        },
        nullptr, tid);
    if (!hook) return false;

    CallPacket pkt{ proc, param };
    SendMessageW(hWnd, msg, 0, reinterpret_cast<LPARAM>(&pkt));
    UnhookWindowsHookEx(hook);
    return true;
}

static HWND FindCoreWindow() {
    HWND result = nullptr;
    EnumWindows([](HWND w, LPARAM lp) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (pid != GetCurrentProcessId()) return TRUE;
        WCHAR cls[128]{};
        if (!GetClassNameW(w, cls, ARRAYSIZE(cls))) return TRUE;
        if (_wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
            *reinterpret_cast<HWND*>(lp) = w;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Wh_ModInit");
    LoadSettings();
    BuildButtons();

    if (IsExplorerProcess()) {
        StartProxyThread();
        return TRUE;
    }

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (auto p = GetProcAddress(hUser32, "CreateWindowInBand"))
        Wh_SetFunctionHook(reinterpret_cast<void*>(p),
                           reinterpret_cast<void*>(CreateWindowInBand_Hook),
                           reinterpret_cast<void**>(&CreateWindowInBand_Original));

    if (auto p = GetProcAddress(hUser32, "CreateWindowInBandEx"))
        Wh_SetFunctionHook(reinterpret_cast<void*>(p),
                           reinterpret_cast<void*>(CreateWindowInBandEx_Hook),
                           reinterpret_cast<void**>(&CreateWindowInBandEx_Original));

    if (HWND cw = FindCoreWindow())
        CallOnWindowThread(cw, [](PVOID hw) { InitWithRetry(static_cast<HWND>(hw)); },
                           reinterpret_cast<PVOID>(cw));

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Wh_ModUninit");
    g_unloading = true;

    if (IsExplorerProcess()) {
        if (g_proxyWindow)
            PostMessageW(g_proxyWindow, WM_CLOSE, 0, 0);
        if (g_proxyThread) {
            WaitForSingleObject(g_proxyThread, 2000);
            CloseHandle(g_proxyThread);
            g_proxyThread = nullptr;
        }
        return;
    }

    if (HWND cw = FindCoreWindow())
        CallOnWindowThread(cw, [](PVOID) { StopVisibilityMonitoring(); }, nullptr);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Wh_ModSettingsChanged");
    LoadSettings();
    BuildButtons();
    g_forceRebuild    = true;
    g_buttonsInjected = false;

    if (HWND cw = FindCoreWindow())
        CallOnWindowThread(cw, [](PVOID) {
            try {
                if (auto w = wuc::CoreWindow::GetForCurrentThread())
                    if (w.Visible()) OnWindowVisible();
            } catch (...) {}
        }, nullptr);
}
