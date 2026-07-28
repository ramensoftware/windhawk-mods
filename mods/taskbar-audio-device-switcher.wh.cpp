// ==WindhawkMod==
// @id              taskbar-audio-device-switcher
// @name            Taskbar audio device switcher
// @description     Shows a tray icon for every connected audio device and switches the default device with a click
// @version         1.1.0
// @author          Maksim Chingin
// @github          https://github.com/umnik1
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar audio device switcher

Adds one tray (notification area) icon per connected audio device. The icon of
the device that is currently the default one is drawn normally, all the other
ones are dimmed and grayed out. **Left click** on an icon makes that device the
default playback/recording device, **right click** opens a menu with additional
options.

Each device uses its own icon, the one Windows shows in the sound settings, so
headphones, speakers, HDMI output and a microphone are easy to tell apart.

## Choosing which devices are shown

The right click menu has a **Shown devices** submenu which lists every device,
including the ones which are currently hidden, with a check mark next to the
ones that have an icon. Clicking an entry shows or hides that device, and the
choice is remembered across restarts. There is also **Hide this icon** for the
device that was clicked, and **Show all devices** to undo everything.

The icon of the last remaining device can't be hidden, otherwise there would be
no menu left to bring the other ones back.

## Windows 11 note

New tray icons are hidden inside the overflow ("^") flyout by default. To make
the audio device icons always visible, either drag them out of the flyout onto
the taskbar, or go to
*Settings → Personalization → Taskbar → Other system tray icons* and turn the
relevant `explorer.exe` entries on.

## Notes

* Devices are refreshed automatically when a device is plugged in/out, enabled,
  disabled, or when the default device changes from somewhere else.
* Switching the default device uses the undocumented `IPolicyConfig` interface,
  the same one used by nircmd, SoundSwitch and similar tools.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showPlayback: true
  $name: Playback devices
  $description: Show an icon for every active playback (output) device
- showRecording: false
  $name: Recording devices
  $description: Show an icon for every active recording (input) device
- dimInactive: true
  $name: Dim inactive devices
  $description: >-
    Draw the icons of the devices which are not the default one faded and
    grayscale
- setCommunications: true
  $name: Switch the communications device too
  $description: >-
    On click, also set the device as the default communications device, and not
    only as the default console/multimedia device
- excludedDevices: [""]
  $name: Always hidden devices
  $description: >-
    Devices whose name contains one of these strings are never shown, for
    example: HDMI. Individual devices can also be hidden from the tray icon
    right click menu, without touching this list.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <propsys.h>
#include <mmdeviceapi.h>

#include <cstring>
#include <cwchar>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef NOTIFYICON_VERSION_4
#define NOTIFYICON_VERSION_4 4
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

// ---------------------------------------------------------------------------
// GUIDs and interfaces (declared locally so that no extra import libs and no
// SDK-only headers are needed).
// ---------------------------------------------------------------------------

static const IID kIID_IUnknown = {
    0x00000000, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const CLSID kCLSID_MMDeviceEnumerator = {
    0xbcde0395, 0xe52f, 0x467c, {0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e}};
static const IID kIID_IMMDeviceEnumerator = {
    0xa95664d2, 0x9614, 0x4f35, {0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6}};
static const IID kIID_IMMNotificationClient = {
    0x7991eec9, 0x7e89, 0x4d85, {0x83, 0x90, 0x6c, 0x70, 0x3c, 0xec, 0x60, 0xc0}};
static const CLSID kCLSID_PolicyConfigClient = {
    0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}};
static const IID kIID_IPolicyConfig = {
    0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}};
static const IID kIID_IPolicyConfigVista = {
    0x568b9108, 0x44bf, 0x40b4, {0x90, 0x06, 0x86, 0xaf, 0xe5, 0xb5, 0xa6, 0x20}};

static const PROPERTYKEY kPKEY_Device_FriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}}, 14};
static const PROPERTYKEY kPKEY_DeviceClass_IconPath = {
    {0x259abffc, 0x50a7, 0x47ce, {0xaf, 0x08, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66}}, 12};

struct DeviceShareModeOpaque;

// The undocumented interface used by the sound applet to change the default
// endpoint. Only SetDefaultEndpoint is called, the other methods are declared
// just to keep the vtable layout correct.
struct IPolicyConfig : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, WAVEFORMATEX*, WAVEFORMATEX*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT64, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, DeviceShareModeOpaque*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, DeviceShareModeOpaque*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR deviceId, ERole role) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

// The Vista variant, kept as a fallback. It has one method less before
// SetDefaultEndpoint (no ResetDeviceFormat).
struct IPolicyConfigVista : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, WAVEFORMATEX*, WAVEFORMATEX*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT64, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, DeviceShareModeOpaque*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, DeviceShareModeOpaque*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR deviceId, ERole role) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

// ---------------------------------------------------------------------------
// Mod state
// ---------------------------------------------------------------------------

namespace {

constexpr UINT WM_APP_TRAY = WM_APP + 1;
constexpr UINT WM_APP_REFRESH = WM_APP + 2;
constexpr UINT WM_APP_RELOAD_SETTINGS = WM_APP + 3;
constexpr UINT_PTR kRefreshTimerId = 1;

constexpr UINT IDM_SET_DEFAULT = 1;
constexpr UINT IDM_SET_COMMUNICATIONS = 2;
constexpr UINT IDM_SOUND_SETTINGS = 3;
constexpr UINT IDM_VOLUME_MIXER = 4;
constexpr UINT IDM_LEGACY_APPLET = 5;
constexpr UINT IDM_HIDE_THIS = 6;
constexpr UINT IDM_SHOW_ALL = 7;
// One command per device, for the "Shown devices" submenu.
constexpr UINT IDM_TOGGLE_FIRST = 100;

// The name of the mod storage value holding the manually hidden device ids.
constexpr PCWSTR kHiddenDevicesValueName = L"hiddenDevices";

struct AudioDevice {
    std::wstring id;
    std::wstring name;
    std::wstring iconPath;
    EDataFlow flow = eRender;
    bool isDefault = false;
    bool isDefaultComm = false;
    // Hidden through the tray menu, can be toggled back from there.
    bool hidden = false;
    // Hidden through the "Hidden devices" setting, can only be brought back by
    // changing the setting.
    bool hiddenBySetting = false;
    UINT trayId = 0;
};

struct TrayIcon {
    UINT trayId = 0;
    HICON icon = nullptr;
    std::wstring tip;
};

struct Settings {
    bool showPlayback = true;
    bool showRecording = false;
    bool dimInactive = true;
    bool setCommunications = true;
    std::vector<std::wstring> excluded;
};

Settings g_settings;

HANDLE g_thread;
HANDLE g_windowReadyEvent;
HWND g_hWnd;
UINT g_taskbarCreatedMessage;

std::vector<AudioDevice> g_devices;  // all devices, hidden ones included
std::unordered_map<std::wstring, TrayIcon> g_trayIcons;  // device id -> icon
std::vector<std::wstring> g_hiddenIds;

using PrivateExtractIconsW_t = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
PrivateExtractIconsW_t g_pPrivateExtractIcons;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

void LoadSettings() {
    g_settings.showPlayback = Wh_GetIntSetting(L"showPlayback") != 0;
    g_settings.showRecording = Wh_GetIntSetting(L"showRecording") != 0;
    g_settings.dimInactive = Wh_GetIntSetting(L"dimInactive") != 0;
    g_settings.setCommunications = Wh_GetIntSetting(L"setCommunications") != 0;

    g_settings.excluded.clear();
    for (int i = 0; i < 64; i++) {
        PCWSTR value = Wh_GetStringSetting(L"excludedDevices[%d]", i);
        bool empty = !value || !*value;
        if (!empty) {
            g_settings.excluded.emplace_back(value);
        }
        Wh_FreeStringSetting(value);
        if (empty) {
            break;
        }
    }
}

std::wstring ToLower(std::wstring str) {
    if (!str.empty()) {
        CharLowerBuffW(&str[0], static_cast<DWORD>(str.size()));
    }
    return str;
}

bool IsExcluded(const std::wstring& name) {
    if (g_settings.excluded.empty()) {
        return false;
    }
    std::wstring lowerName = ToLower(name);
    for (const auto& pattern : g_settings.excluded) {
        if (lowerName.find(ToLower(pattern)) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// The list of devices hidden through the tray menu. It lives in the mod
// storage and not in the settings, because a mod can read its settings but not
// write them.
// ---------------------------------------------------------------------------

void LoadHiddenIds() {
    g_hiddenIds.clear();

    // Device ids are about 55 characters long, so this is enough for hundreds
    // of them.
    std::vector<WCHAR> buffer(32768);
    size_t length = Wh_GetStringValue(kHiddenDevicesValueName, buffer.data(),
                                      buffer.size());
    if (length == 0) {
        return;
    }

    std::wstring stored(buffer.data(), length);
    size_t start = 0;
    while (start <= stored.size()) {
        size_t end = stored.find(L'\n', start);
        if (end == std::wstring::npos) {
            end = stored.size();
        }
        if (end > start) {
            g_hiddenIds.emplace_back(stored, start, end - start);
        }
        start = end + 1;
    }
}

void SaveHiddenIds() {
    if (g_hiddenIds.empty()) {
        Wh_DeleteValue(kHiddenDevicesValueName);
        return;
    }

    std::wstring joined;
    for (const auto& id : g_hiddenIds) {
        if (!joined.empty()) {
            joined += L'\n';
        }
        joined += id;
    }
    Wh_SetStringValue(kHiddenDevicesValueName, joined.c_str());
}

bool IsHiddenById(const std::wstring& deviceId) {
    for (const auto& id : g_hiddenIds) {
        if (id == deviceId) {
            return true;
        }
    }
    return false;
}

void SetDeviceHidden(const std::wstring& deviceId, bool hidden) {
    bool changed = false;
    if (hidden) {
        if (!IsHiddenById(deviceId)) {
            g_hiddenIds.push_back(deviceId);
            changed = true;
        }
    } else {
        for (auto it = g_hiddenIds.begin(); it != g_hiddenIds.end(); ++it) {
            if (*it == deviceId) {
                g_hiddenIds.erase(it);
                changed = true;
                break;
            }
        }
    }
    if (changed) {
        SaveHiddenIds();
    }
}

// ---------------------------------------------------------------------------
// Icons
// ---------------------------------------------------------------------------

HICON ExtractIconFromSpec(std::wstring spec, int size) {
    if (spec.empty()) {
        return nullptr;
    }

    int index = 0;
    size_t comma = spec.find_last_of(L',');
    if (comma != std::wstring::npos) {
        index = static_cast<int>(wcstol(spec.c_str() + comma + 1, nullptr, 10));
        spec.resize(comma);
    }

    WCHAR expanded[MAX_PATH * 2];
    if (ExpandEnvironmentStringsW(spec.c_str(), expanded, ARRAYSIZE(expanded))) {
        spec = expanded;
    }

    HICON icon = nullptr;

    if (g_pPrivateExtractIcons) {
        UINT id = 0;
        if (g_pPrivateExtractIcons(spec.c_str(), index, size, size, &icon, &id, 1,
                                   LR_DEFAULTCOLOR) != 1) {
            icon = nullptr;
        }
    }

    if (!icon) {
        HICON large = nullptr;
        HICON small = nullptr;
        if (ExtractIconExW(spec.c_str(), index, &large, &small, 1) != UINT_MAX) {
            HICON source = small ? small : large;
            if (source) {
                icon = static_cast<HICON>(
                    CopyImage(source, IMAGE_ICON, size, size, LR_DEFAULTCOLOR));
            }
        }
        if (large) {
            DestroyIcon(large);
        }
        if (small) {
            DestroyIcon(small);
        }
    }

    return icon;
}

HICON ExtractFallbackIcon(EDataFlow flow, int size) {
    PCWSTR candidates[] = {
        flow == eCapture ? L"%windir%\\system32\\mmres.dll,-3020"
                         : L"%windir%\\system32\\mmres.dll,-3010",
        L"%windir%\\system32\\SndVol.exe,0",
        L"%windir%\\system32\\mmres.dll,0",
    };
    for (PCWSTR candidate : candidates) {
        if (HICON icon = ExtractIconFromSpec(candidate, size)) {
            return icon;
        }
    }
    return nullptr;
}

// Returns a faded, mostly grayscale copy of the icon, or nullptr on failure.
HICON MakeDimmedIcon(HICON source) {
    ICONINFO info = {};
    if (!GetIconInfo(source, &info)) {
        return nullptr;
    }

    HICON result = nullptr;
    HDC hdc = GetDC(nullptr);
    BITMAP bitmap = {};

    if (hdc && info.hbmColor && GetObjectW(info.hbmColor, sizeof(bitmap), &bitmap) &&
        bitmap.bmWidth > 0 && bitmap.bmHeight > 0) {
        const int width = bitmap.bmWidth;
        const int height = bitmap.bmHeight;

        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;  // top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        std::vector<DWORD> pixels(static_cast<size_t>(width) * height, 0);
        if (GetDIBits(hdc, info.hbmColor, 0, height, pixels.data(), &bi, DIB_RGB_COLORS)) {
            bool hasAlpha = false;
            for (DWORD pixel : pixels) {
                if ((pixel >> 24) != 0) {
                    hasAlpha = true;
                    break;
                }
            }

            // Icons without an alpha channel: build one from the AND mask.
            if (!hasAlpha && info.hbmMask) {
                std::vector<DWORD> mask(pixels.size(), 0);
                if (GetDIBits(hdc, info.hbmMask, 0, height, mask.data(), &bi,
                              DIB_RGB_COLORS)) {
                    for (size_t i = 0; i < pixels.size(); i++) {
                        DWORD alpha = (mask[i] & 0x00FFFFFF) ? 0u : 0xFFu;
                        pixels[i] = (pixels[i] & 0x00FFFFFF) | (alpha << 24);
                    }
                    hasAlpha = true;
                }
            }

            if (hasAlpha) {
                for (DWORD& pixel : pixels) {
                    BYTE a = static_cast<BYTE>(pixel >> 24);
                    BYTE r = static_cast<BYTE>(pixel >> 16);
                    BYTE g = static_cast<BYTE>(pixel >> 8);
                    BYTE b = static_cast<BYTE>(pixel);
                    BYTE gray = static_cast<BYTE>((r * 77 + g * 151 + b * 28) >> 8);
                    r = static_cast<BYTE>((r + gray * 3) / 4);
                    g = static_cast<BYTE>((g + gray * 3) / 4);
                    b = static_cast<BYTE>((b + gray * 3) / 4);
                    a = static_cast<BYTE>(a * 45 / 100);
                    pixel = (static_cast<DWORD>(a) << 24) | (static_cast<DWORD>(r) << 16) |
                            (static_cast<DWORD>(g) << 8) | b;
                }

                void* bits = nullptr;
                HBITMAP color = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
                if (color && bits) {
                    memcpy(bits, pixels.data(), pixels.size() * sizeof(DWORD));

                    // An all-zero AND mask, the alpha channel does the masking.
                    std::vector<BYTE> maskBits(
                        static_cast<size_t>(((width + 31) / 32) * 4) * height, 0);
                    HBITMAP maskBitmap = CreateBitmap(width, height, 1, 1, maskBits.data());
                    if (maskBitmap) {
                        ICONINFO out = {};
                        out.fIcon = TRUE;
                        out.hbmColor = color;
                        out.hbmMask = maskBitmap;
                        result = CreateIconIndirect(&out);
                        DeleteObject(maskBitmap);
                    }
                }
                if (color) {
                    DeleteObject(color);
                }
            }
        }
    }

    if (hdc) {
        ReleaseDC(nullptr, hdc);
    }
    if (info.hbmColor) {
        DeleteObject(info.hbmColor);
    }
    if (info.hbmMask) {
        DeleteObject(info.hbmMask);
    }
    return result;
}

HICON BuildDeviceIcon(const AudioDevice& device) {
    int size = GetSystemMetrics(SM_CXSMICON);
    if (size <= 0) {
        size = 16;
    }

    HICON icon = ExtractIconFromSpec(device.iconPath, size);
    if (!icon) {
        icon = ExtractFallbackIcon(device.flow, size);
    }
    if (!icon) {
        return nullptr;
    }

    if (g_settings.dimInactive && !device.isDefault) {
        if (HICON dimmed = MakeDimmedIcon(icon)) {
            DestroyIcon(icon);
            icon = dimmed;
        }
    }

    return icon;
}

// ---------------------------------------------------------------------------
// Device enumeration
// ---------------------------------------------------------------------------

std::wstring GetDeviceId(IMMDevice* device) {
    std::wstring result;
    LPWSTR id = nullptr;
    if (SUCCEEDED(device->GetId(&id)) && id) {
        result = id;
    }
    if (id) {
        CoTaskMemFree(id);
    }
    return result;
}

std::wstring GetStringProperty(IMMDevice* device, const PROPERTYKEY& key) {
    std::wstring result;
    IPropertyStore* store = nullptr;
    if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, &store)) && store) {
        PROPVARIANT value;
        PropVariantInit(&value);
        if (SUCCEEDED(store->GetValue(key, &value)) && value.vt == VT_LPWSTR &&
            value.pwszVal) {
            result = value.pwszVal;
        }
        PropVariantClear(&value);
        store->Release();
    }
    return result;
}

// A stable id derived from the device id, so that Windows remembers the "show
// this icon on the taskbar" choice across sessions and reconnects. The tray
// callback message of NOTIFYICON_VERSION_4 carries the id in the high word, so
// it has to fit in 16 bits.
UINT TrayIdForDevice(const std::wstring& deviceId) {
    UINT hash = 2166136261u;
    for (wchar_t c : deviceId) {
        hash = (hash ^ static_cast<UINT>(c & 0xFF)) * 16777619u;
        hash = (hash ^ static_cast<UINT>((c >> 8) & 0xFF)) * 16777619u;
    }
    hash = (hash ^ (hash >> 16)) & 0xFFFF;
    return hash ? hash : 1;
}

void EnumerateDevices(std::vector<AudioDevice>& devices) {
    IMMDeviceEnumerator* enumerator = nullptr;
    if (FAILED(CoCreateInstance(kCLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL,
                                kIID_IMMDeviceEnumerator,
                                reinterpret_cast<void**>(&enumerator))) ||
        !enumerator) {
        Wh_Log(L"Failed to create the device enumerator");
        return;
    }

    const struct {
        EDataFlow flow;
        bool enabled;
    } flows[] = {
        {eRender, g_settings.showPlayback},
        {eCapture, g_settings.showRecording},
    };

    for (const auto& entry : flows) {
        if (!entry.enabled) {
            continue;
        }

        std::wstring defaultId;
        std::wstring defaultCommId;
        IMMDevice* defaultDevice = nullptr;
        if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(entry.flow, eConsole,
                                                          &defaultDevice)) &&
            defaultDevice) {
            defaultId = GetDeviceId(defaultDevice);
            defaultDevice->Release();
            defaultDevice = nullptr;
        }
        if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(entry.flow, eCommunications,
                                                          &defaultDevice)) &&
            defaultDevice) {
            defaultCommId = GetDeviceId(defaultDevice);
            defaultDevice->Release();
            defaultDevice = nullptr;
        }

        IMMDeviceCollection* collection = nullptr;
        if (FAILED(enumerator->EnumAudioEndpoints(entry.flow, DEVICE_STATE_ACTIVE,
                                                  &collection)) ||
            !collection) {
            continue;
        }

        UINT count = 0;
        collection->GetCount(&count);
        for (UINT i = 0; i < count; i++) {
            IMMDevice* device = nullptr;
            if (FAILED(collection->Item(i, &device)) || !device) {
                continue;
            }

            AudioDevice info;
            info.id = GetDeviceId(device);
            info.name = GetStringProperty(device, kPKEY_Device_FriendlyName);
            info.iconPath = GetStringProperty(device, kPKEY_DeviceClass_IconPath);
            info.flow = entry.flow;
            info.isDefault = !info.id.empty() && info.id == defaultId;
            info.isDefaultComm = !info.id.empty() && info.id == defaultCommId;
            device->Release();

            if (info.id.empty()) {
                continue;
            }
            if (info.name.empty()) {
                info.name = entry.flow == eCapture ? L"Recording device" : L"Playback device";
            }

            // Hidden devices stay in the list, they are only skipped when the
            // tray icons are created. This way they can still be listed in the
            // "Shown devices" submenu.
            info.hiddenBySetting = IsExcluded(info.name);
            info.hidden = info.hiddenBySetting || IsHiddenById(info.id);

            info.trayId = TrayIdForDevice(info.id);
            // 16 bits are not a lot, make sure two devices never share an id.
            bool taken;
            do {
                taken = false;
                for (const auto& other : devices) {
                    if (other.trayId == info.trayId) {
                        taken = true;
                        info.trayId = (info.trayId % 0xFFFF) + 1;
                        break;
                    }
                }
            } while (taken);

            devices.push_back(std::move(info));
        }
        collection->Release();
    }

    enumerator->Release();
}

// ---------------------------------------------------------------------------
// Tray icons
// ---------------------------------------------------------------------------

void RemoveTrayIcon(UINT trayId) {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hWnd;
    nid.uID = trayId;
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

void RemoveAllTrayIcons(bool notifyShell) {
    for (auto& entry : g_trayIcons) {
        if (notifyShell) {
            RemoveTrayIcon(entry.second.trayId);
        }
        if (entry.second.icon) {
            DestroyIcon(entry.second.icon);
        }
    }
    g_trayIcons.clear();
}

std::wstring MakeTooltip(const AudioDevice& device) {
    std::wstring tip;
    if (device.isDefault) {
        tip += L"✔ ";  // heavy check mark
    }
    tip += device.name;
    if (device.isDefault) {
        tip += device.flow == eCapture ? L"\n(default recording device)"
                                       : L"\n(default playback device)";
    }
    if (tip.size() > 127) {
        tip.resize(127);
    }
    return tip;
}

void RefreshDevices() {
    std::vector<AudioDevice> devices;
    EnumerateDevices(devices);

    // Remove the icons of devices which are gone or which were hidden.
    for (auto it = g_trayIcons.begin(); it != g_trayIcons.end();) {
        bool stillShown = false;
        for (const auto& device : devices) {
            if (device.id == it->first && !device.hidden) {
                stillShown = true;
                break;
            }
        }
        if (stillShown) {
            ++it;
            continue;
        }
        RemoveTrayIcon(it->second.trayId);
        if (it->second.icon) {
            DestroyIcon(it->second.icon);
        }
        it = g_trayIcons.erase(it);
    }

    for (const auto& device : devices) {
        if (device.hidden) {
            continue;
        }

        HICON icon = BuildDeviceIcon(device);
        std::wstring tip = MakeTooltip(device);

        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = g_hWnd;
        nid.uID = device.trayId;
        nid.uFlags = NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
        nid.uCallbackMessage = WM_APP_TRAY;
        lstrcpynW(nid.szTip, tip.c_str(), ARRAYSIZE(nid.szTip));
        if (icon) {
            nid.uFlags |= NIF_ICON;
            nid.hIcon = icon;
        }

        auto it = g_trayIcons.find(device.id);
        if (it == g_trayIcons.end()) {
            if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
                // A leftover icon from a previous session of the mod.
                RemoveTrayIcon(device.trayId);
                if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
                    Wh_Log(L"Failed to add a tray icon for %s", device.name.c_str());
                    if (icon) {
                        DestroyIcon(icon);
                    }
                    continue;
                }
            }

            NOTIFYICONDATAW version = {};
            version.cbSize = sizeof(version);
            version.hWnd = g_hWnd;
            version.uID = device.trayId;
            version.uVersion = NOTIFYICON_VERSION_4;
            Shell_NotifyIconW(NIM_SETVERSION, &version);

            TrayIcon state;
            state.trayId = device.trayId;
            state.icon = icon;
            state.tip = std::move(tip);
            g_trayIcons[device.id] = std::move(state);
        } else {
            Shell_NotifyIconW(NIM_MODIFY, &nid);
            if (it->second.icon) {
                DestroyIcon(it->second.icon);
            }
            it->second.icon = icon;
            it->second.tip = std::move(tip);
        }
    }

    g_devices = std::move(devices);
}

const AudioDevice* FindDeviceByTrayId(UINT trayId) {
    for (const auto& device : g_devices) {
        if (device.trayId == trayId) {
            return &device;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Switching the default device
// ---------------------------------------------------------------------------

void SetDefaultDevice(const std::wstring& deviceId, bool communicationsOnly) {
    IPolicyConfig* config = nullptr;
    HRESULT hr = CoCreateInstance(kCLSID_PolicyConfigClient, nullptr, CLSCTX_ALL,
                                  kIID_IPolicyConfig, reinterpret_cast<void**>(&config));
    if (SUCCEEDED(hr) && config) {
        if (communicationsOnly) {
            config->SetDefaultEndpoint(deviceId.c_str(), eCommunications);
        } else {
            config->SetDefaultEndpoint(deviceId.c_str(), eConsole);
            config->SetDefaultEndpoint(deviceId.c_str(), eMultimedia);
            if (g_settings.setCommunications) {
                config->SetDefaultEndpoint(deviceId.c_str(), eCommunications);
            }
        }
        config->Release();
        return;
    }

    IPolicyConfigVista* vista = nullptr;
    hr = CoCreateInstance(kCLSID_PolicyConfigClient, nullptr, CLSCTX_ALL,
                          kIID_IPolicyConfigVista, reinterpret_cast<void**>(&vista));
    if (SUCCEEDED(hr) && vista) {
        if (communicationsOnly) {
            vista->SetDefaultEndpoint(deviceId.c_str(), eCommunications);
        } else {
            vista->SetDefaultEndpoint(deviceId.c_str(), eConsole);
            vista->SetDefaultEndpoint(deviceId.c_str(), eMultimedia);
            if (g_settings.setCommunications) {
                vista->SetDefaultEndpoint(deviceId.c_str(), eCommunications);
            }
        }
        vista->Release();
        return;
    }

    Wh_Log(L"Failed to create the policy config client, hr=0x%08X",
           static_cast<unsigned>(hr));
}

// ---------------------------------------------------------------------------
// Device change notifications
// ---------------------------------------------------------------------------

class NotificationClient final : public IMMNotificationClient {
   public:
    virtual ~NotificationClient() = default;

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (IsEqualGUID(riid, kIID_IUnknown) ||
            IsEqualGUID(riid, kIID_IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        LONG count = InterlockedDecrement(&m_refCount);
        if (count == 0) {
            delete this;
        }
        return count;
    }

    // IMMNotificationClient
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override {
        NotifyChanged();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override {
        NotifyChanged();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override {
        NotifyChanged();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow, ERole, LPCWSTR) override {
        NotifyChanged();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override {
        NotifyChanged();
        return S_OK;
    }

   private:
    void NotifyChanged() {
        if (g_hWnd) {
            PostMessageW(g_hWnd, WM_APP_REFRESH, 0, 0);
        }
    }

    LONG m_refCount = 1;
};

// ---------------------------------------------------------------------------
// Window
// ---------------------------------------------------------------------------

// The submenu which lets the user pick which devices get a tray icon. It lists
// every device, the hidden ones included, since a hidden device has no icon of
// its own to right click on.
HMENU BuildShownDevicesMenu(const std::vector<AudioDevice>& devices,
                            size_t visibleCount) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return nullptr;
    }

    bool anyHidden = false;
    for (size_t i = 0; i < devices.size(); i++) {
        const AudioDevice& device = devices[i];
        UINT flags = MF_STRING;
        if (!device.hidden) {
            flags |= MF_CHECKED;
            // Never let the user hide the last icon, it would leave no way
            // back to this menu.
            if (visibleCount <= 1) {
                flags |= MF_GRAYED;
            }
        } else {
            anyHidden = true;
            // Devices filtered out by the "Hidden devices" setting can't be
            // brought back from here.
            if (device.hiddenBySetting) {
                flags |= MF_GRAYED;
            }
        }

        std::wstring text = device.name;
        if (device.flow == eCapture) {
            text += L" — microphone";
        }
        if (device.hiddenBySetting) {
            text += L" — hidden by the settings";
        }

        AppendMenuW(menu, flags, IDM_TOGGLE_FIRST + static_cast<UINT_PTR>(i),
                    text.c_str());
    }

    if (anyHidden) {
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING, IDM_SHOW_ALL, L"Show all devices");
    }

    return menu;
}

void ShowContextMenu(HWND hWnd,
                     const std::vector<AudioDevice>& devices,
                     size_t clickedIndex,
                     int x,
                     int y) {
    if (clickedIndex >= devices.size()) {
        return;
    }
    const AudioDevice& device = devices[clickedIndex];

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    size_t visibleCount = 0;
    for (const auto& other : devices) {
        if (!other.hidden) {
            visibleCount++;
        }
    }

    AppendMenuW(menu, MF_STRING | MF_GRAYED, 0, device.name.c_str());
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (device.isDefault ? MF_CHECKED : 0), IDM_SET_DEFAULT,
                L"Use as the default device");
    AppendMenuW(menu, MF_STRING | (device.isDefaultComm ? MF_CHECKED : 0),
                IDM_SET_COMMUNICATIONS, L"Use as the communications device");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

    HMENU shownDevicesMenu = BuildShownDevicesMenu(devices, visibleCount);
    if (shownDevicesMenu) {
        AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(shownDevicesMenu),
                    L"Shown devices");
    }
    AppendMenuW(menu, MF_STRING | (visibleCount <= 1 ? MF_GRAYED : 0), IDM_HIDE_THIS,
                L"Hide this icon");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_SOUND_SETTINGS, L"Sound settings");
    AppendMenuW(menu, MF_STRING, IDM_VOLUME_MIXER, L"Volume mixer");
    AppendMenuW(menu, MF_STRING, IDM_LEGACY_APPLET, L"Sound control panel");

    SetForegroundWindow(hWnd);
    UINT command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, x,
                                  y, 0, hWnd, nullptr);
    PostMessageW(hWnd, WM_NULL, 0, 0);
    DestroyMenu(menu);  // destroys the submenu as well

    if (command >= IDM_TOGGLE_FIRST &&
        command < IDM_TOGGLE_FIRST + devices.size()) {
        const AudioDevice& toggled = devices[command - IDM_TOGGLE_FIRST];
        SetDeviceHidden(toggled.id, !toggled.hidden);
        RefreshDevices();
        return;
    }

    switch (command) {
        case IDM_SET_DEFAULT:
            SetDefaultDevice(device.id, false);
            break;
        case IDM_SET_COMMUNICATIONS:
            SetDefaultDevice(device.id, true);
            break;
        case IDM_HIDE_THIS:
            SetDeviceHidden(device.id, true);
            RefreshDevices();
            break;
        case IDM_SHOW_ALL:
            g_hiddenIds.clear();
            SaveHiddenIds();
            RefreshDevices();
            break;
        case IDM_SOUND_SETTINGS:
            ShellExecuteW(nullptr, L"open", L"ms-settings:sound", nullptr, nullptr,
                          SW_SHOWNORMAL);
            break;
        case IDM_VOLUME_MIXER:
            ShellExecuteW(nullptr, L"open", L"ms-settings:apps-volume", nullptr, nullptr,
                          SW_SHOWNORMAL);
            break;
        case IDM_LEGACY_APPLET:
            ShellExecuteW(nullptr, L"open", L"rundll32.exe",
                          L"shell32.dll,Control_RunDLL mmsys.cpl", nullptr, SW_SHOWNORMAL);
            break;
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == g_taskbarCreatedMessage && g_taskbarCreatedMessage) {
        // The tray was re-created, our icons are gone with it.
        RemoveAllTrayIcons(false);
        RefreshDevices();
        return 0;
    }

    switch (message) {
        case WM_APP_TRAY: {
            UINT event = LOWORD(lParam);
            UINT trayId = HIWORD(lParam);
            const AudioDevice* device = FindDeviceByTrayId(trayId);
            if (!device) {
                return 0;
            }
            switch (event) {
                case NIN_SELECT:
                case NIN_KEYSELECT:
                case WM_LBUTTONUP:
                    if (!device->isDefault) {
                        SetDefaultDevice(device->id, false);
                    }
                    break;
                case WM_MBUTTONUP:
                    SetDefaultDevice(device->id, true);
                    break;
                case WM_CONTEXTMENU:
                case WM_RBUTTONUP: {
                    // The menu runs its own message loop, during which
                    // g_devices can be rebuilt, so work on a snapshot.
                    std::vector<AudioDevice> snapshot = g_devices;
                    size_t clickedIndex = snapshot.size();
                    for (size_t i = 0; i < snapshot.size(); i++) {
                        if (snapshot[i].trayId == trayId) {
                            clickedIndex = i;
                            break;
                        }
                    }
                    ShowContextMenu(hWnd, snapshot, clickedIndex,
                                    static_cast<short>(LOWORD(wParam)),
                                    static_cast<short>(HIWORD(wParam)));
                    break;
                }
            }
            return 0;
        }

        case WM_APP_REFRESH:
            // Coalesce the bursts of notifications a device change produces.
            SetTimer(hWnd, kRefreshTimerId, 250, nullptr);
            return 0;

        case WM_APP_RELOAD_SETTINGS:
            LoadSettings();
            RemoveAllTrayIcons(true);
            RefreshDevices();
            return 0;

        case WM_TIMER:
            if (wParam == kRefreshTimerId) {
                KillTimer(hWnd, kRefreshTimerId);
                RefreshDevices();
                return 0;
            }
            break;

        case WM_SETTINGCHANGE:
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            SetTimer(hWnd, kRefreshTimerId, 500, nullptr);
            break;

        case WM_DESTROY:
            KillTimer(hWnd, kRefreshTimerId);
            RemoveAllTrayIcons(true);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

constexpr PCWSTR kWindowClassName = L"WindhawkTaskbarAudioDeviceSwitcher";

DWORD WINAPI ThreadProc(LPVOID) {
    HRESULT comInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        g_pPrivateExtractIcons = reinterpret_cast<PrivateExtractIconsW_t>(
            GetProcAddress(user32, "PrivateExtractIconsW"));
    }

    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    LoadHiddenIds();

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kWindowClassName;
    ATOM atom = RegisterClassExW(&wc);
    if (!atom && GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
        // Left over from a previous instance of the mod, its window procedure
        // points into an unloaded module.
        UnregisterClassW(kWindowClassName, wc.hInstance);
        atom = RegisterClassExW(&wc);
    }
    if (!atom) {
        Wh_Log(L"RegisterClassExW failed, error %u", GetLastError());
        SetEvent(g_windowReadyEvent);
        if (SUCCEEDED(comInit)) {
            CoUninitialize();
        }
        return 1;
    }

    HWND hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, kWindowClassName, L"", WS_POPUP, 0, 0, 0,
                                0, nullptr, nullptr, wc.hInstance, nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowExW failed, error %u", GetLastError());
        UnregisterClassW(kWindowClassName, wc.hInstance);
        SetEvent(g_windowReadyEvent);
        if (SUCCEEDED(comInit)) {
            CoUninitialize();
        }
        return 1;
    }

    g_hWnd = hWnd;
    SetEvent(g_windowReadyEvent);

    IMMDeviceEnumerator* enumerator = nullptr;
    NotificationClient* client = nullptr;
    if (SUCCEEDED(CoCreateInstance(kCLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL,
                                   kIID_IMMDeviceEnumerator,
                                   reinterpret_cast<void**>(&enumerator))) &&
        enumerator) {
        client = new NotificationClient();
        if (FAILED(enumerator->RegisterEndpointNotificationCallback(client))) {
            client->Release();
            client = nullptr;
            Wh_Log(L"Failed to register the endpoint notification callback");
        }
    }

    RefreshDevices();

    MSG msg;
    BOOL result;
    while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
        if (result == -1) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (enumerator) {
        if (client) {
            enumerator->UnregisterEndpointNotificationCallback(client);
            client->Release();
        }
        enumerator->Release();
    }

    g_hWnd = nullptr;
    DestroyWindow(hWnd);
    UnregisterClassW(kWindowClassName, wc.hInstance);

    if (SUCCEEDED(comInit)) {
        CoUninitialize();
    }
    return 0;
}

}  // namespace

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_windowReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_windowReadyEvent) {
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_windowReadyEvent);
        g_windowReadyEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (g_hWnd) {
        PostMessageW(g_hWnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_windowReadyEvent) {
        WaitForSingleObject(g_windowReadyEvent, 5000);
    }

    HWND hWnd = g_hWnd;
    if (hWnd) {
        PostMessageW(hWnd, WM_CLOSE, 0, 0);
    }

    if (g_thread) {
        if (WaitForSingleObject(g_thread, 5000) == WAIT_TIMEOUT) {
            Wh_Log(L"The mod thread did not exit in time");
        }
        CloseHandle(g_thread);
        g_thread = nullptr;
    }

    if (g_windowReadyEvent) {
        CloseHandle(g_windowReadyEvent);
        g_windowReadyEvent = nullptr;
    }
}
