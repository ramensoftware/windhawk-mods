// ==WindhawkMod==
// @id              taskbar-audio-device-toggle
// @name            Taskbar Audio Device Toggle
// @description     Toggles an audio device by SoundVolumeView and synchronizes its tray icon with the actual Windows device state.
// @version         1.6.0
// @author          Jed
// @github          https://github.com/jed84
// @license         MIT
// @homepage        https://github.com/jed84/windhawk-taskbar-audio-device-toggle
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- SoundVolumeViewPath: C:\Files\x86\Portable\Windows\SoundVolumeView.exe
  $name: SoundVolumeView path
  $description: Full path to SoundVolumeView.exe.
- DeviceId: Realtek(R) Audio
  $name: SoundVolumeView device ID
  $description: Command-line friendly ID used to toggle the device.
- EndpointId: ""
  $name: Windows audio endpoint ID
  $description: Optional exact Windows endpoint ID. Leave empty for automatic matching.
- SyncIntervalSeconds: 300
  $name: State synchronization interval
  $description: Time between actual device state checks, in seconds. Any value of 1 or more. Default is 300 (5 minutes).
- EnabledIconGlyph: E7F5
  $name: Enabled icon glyph
  $description: Hexadecimal Segoe Fluent Icons glyph code shown when the device is enabled.
- DisabledIconGlyph: E720
  $name: Disabled icon glyph
  $description: Hexadecimal Segoe Fluent Icons glyph code shown when the device is disabled.
- IconColor: FFFFFF
  $name: Icon color
  $description: Enter a color in RRGGBB format, without #. For example, FFFFFF for white or 000000 for black.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <mmdeviceapi.h>
#include <propsys.h>
#include <strsafe.h>
#include <string>
#include <vector>
#include <cwchar>

constexpr UINT WM_TRAY_CALLBACK = WM_APP + 1;
constexpr UINT_PTR SYNC_TIMER = 1;
constexpr UINT MENU_WINDHAWK = 1001;
constexpr UINT MENU_AUDIO = 1002;
constexpr ULONGLONG POLL_AFTER_TOGGLE_MS = 250;

static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_toggleProcess = nullptr;
static HMODULE g_gdi = nullptr;
static HMODULE g_ole = nullptr;
static IMMDeviceEnumerator* g_enumerator = nullptr;
static NOTIFYICONDATAW g_nid;
static UINT g_taskbarCreated = 0;
static bool g_iconAdded = false;
static bool g_stateKnown = false;
static DWORD g_state = 0;
static ULONGLONG g_syncInterval = 300000;  // internal, milliseconds
static ULONGLONG g_nextCheck = 0;
static bool g_pendingVerification = false;
static DWORD g_stateBeforeToggle = 0;
static ULONGLONG g_verificationDeadline = 0;
static HICON g_enabledIcon = nullptr;
static HICON g_disabledIcon = nullptr;
static HICON g_unknownIcon = nullptr;
static std::wstring g_deviceId;
static std::wstring g_endpointOverride;
static std::wstring g_cachedEndpoint;
static std::wstring g_displayName;
static std::wstring g_statusText;

#define DECLARE_GDI(name) static decltype(&name) p##name = nullptr
DECLARE_GDI(CreateCompatibleDC);
DECLARE_GDI(CreateDIBSection);
DECLARE_GDI(CreateBitmap);
DECLARE_GDI(SelectObject);
DECLARE_GDI(DeleteObject);
DECLARE_GDI(CreateFontW);
DECLARE_GDI(SetBkMode);
DECLARE_GDI(SetTextColor);
DECLARE_GDI(DeleteDC);
DECLARE_GDI(GdiFlush);
#undef DECLARE_GDI

static decltype(&CoInitializeEx) pCoInitializeEx = nullptr;
static decltype(&CoUninitialize) pCoUninitialize = nullptr;
static decltype(&CoCreateInstance) pCoCreateInstance = nullptr;
static decltype(&CoTaskMemFree) pCoTaskMemFree = nullptr;
static decltype(&PropVariantClear) pPropVariantClear = nullptr;

// Local constants avoid external GUID linkage dependencies.
static const CLSID kDeviceEnumerator = {0xbcde0395, 0xe52f, 0x467c, {0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e}};
static const PROPERTYKEY kDeviceDescription = {{0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}}, 2};
static const PROPERTYKEY kDeviceFriendlyName = {{0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}}, 14};
static const PROPERTYKEY kAdapterFriendlyName = {{0x026e516e, 0xb814, 0x414b, {0x83, 0xcd, 0x85, 0x6d, 0x6f, 0xef, 0x48, 0x22}}, 2};

static std::wstring ReadSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    if (value) Wh_FreeStringSetting(value);
    return result;
}

static bool EqualText(const std::wstring& first, const std::wstring& second) {
    return _wcsicmp(first.c_str(), second.c_str()) == 0;
}

static bool LoadFunctions() {
    g_gdi = LoadLibraryW(L"gdi32.dll");
    g_ole = LoadLibraryW(L"ole32.dll");
    if (!g_gdi || !g_ole) return false;
    bool loaded = true;

#define LOAD_GDI(name) \
    p##name = reinterpret_cast<decltype(p##name)>(GetProcAddress(g_gdi, #name)); \
    if (!p##name) loaded = false
    LOAD_GDI(CreateCompatibleDC);
    LOAD_GDI(CreateDIBSection);
    LOAD_GDI(CreateBitmap);
    LOAD_GDI(SelectObject);
    LOAD_GDI(DeleteObject);
    LOAD_GDI(CreateFontW);
    LOAD_GDI(SetBkMode);
    LOAD_GDI(SetTextColor);
    LOAD_GDI(DeleteDC);
    LOAD_GDI(GdiFlush);
#undef LOAD_GDI

#define LOAD_OLE(name) \
    p##name = reinterpret_cast<decltype(p##name)>(GetProcAddress(g_ole, #name)); \
    if (!p##name) loaded = false
    LOAD_OLE(CoInitializeEx);
    LOAD_OLE(CoUninitialize);
    LOAD_OLE(CoCreateInstance);
    LOAD_OLE(CoTaskMemFree);
    LOAD_OLE(PropVariantClear);
#undef LOAD_OLE

    return loaded;
}

static WCHAR ParseGlyph(const std::wstring& value, WCHAR fallback) {
    const wchar_t* start = value.c_str();
    if (value.size() > 2 && start[0] == L'U' && start[1] == L'+') {
        start += 2;
    } else if (value.size() > 2 && start[0] == L'0' && (start[1] == L'x' || start[1] == L'X')) {
        start += 2;
    }
    wchar_t* end = nullptr;
    unsigned long code = std::wcstoul(start, &end, 16);
    if (end == start || !end || *end != 0 || code == 0 || code > 0xFFFF ||
        (code >= 0xD800 && code <= 0xDFFF)) {
        Wh_Log(L"Invalid glyph code '%s'. Using %04X.", value.c_str(), static_cast<unsigned int>(fallback));
        return fallback;
    }
    return static_cast<WCHAR>(code);
}

static int HexDigit(wchar_t character) {
    if (character >= L'0' && character <= L'9') return character - L'0';
    if (character >= L'a' && character <= L'f') return character - L'a' + 10;
    if (character >= L'A' && character <= L'F') return character - L'A' + 10;
    return -1;
}

static COLORREF GetIconColor() {
    std::wstring value = ReadSetting(L"IconColor");
    if (!value.empty() && value[0] == L'#') value.erase(0, 1);
    bool valid = value.size() == 6;
    unsigned int rgb = 0;
    if (valid) {
        for (size_t i = 0; i < value.size(); i++) {
            int digit = HexDigit(value[i]);
            if (digit < 0) {
                valid = false;
                break;
            }
            rgb = (rgb << 4) | static_cast<unsigned int>(digit);
        }
    }
    if (!valid) {
        Wh_Log(L"Invalid icon color '%s'. Expected RRGGBB. Using FFFFFF.", value.c_str());
        return RGB(255, 255, 255);
    }
    return RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
}

static HICON CreateGlyphIcon(WCHAR glyph, COLORREF color) {
    int size = GetSystemMetrics(SM_CXSMICON);
    if (size < 16) size = 16;

    HDC dc = pCreateCompatibleDC(nullptr);
    if (!dc) return nullptr;

    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = size;
    info.bmiHeader.biHeight = -size;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = pCreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    int stride = ((size + 15) / 16) * 2;
    std::vector<BYTE> maskBits(stride * size, 0);
    HBITMAP mask = pCreateBitmap(size, size, 1, 1, maskBits.data());
    if (!bitmap || !mask || !bits) {
        if (bitmap) pDeleteObject(bitmap);
        if (mask) pDeleteObject(mask);
        pDeleteDC(dc);
        return nullptr;
    }

    ZeroMemory(bits, size * size * 4);
    HGDIOBJ oldBitmap = pSelectObject(dc, bitmap);
    HFONT font = pCreateFontW(-size + 2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe Fluent Icons");
    if (!font) {
        pSelectObject(dc, oldBitmap);
        pDeleteObject(bitmap);
        pDeleteObject(mask);
        pDeleteDC(dc);
        return nullptr;
    }

    HGDIOBJ oldFont = pSelectObject(dc, font);

    // Render coverage in white before applying the selected color.
    pSetBkMode(dc, TRANSPARENT);
    pSetTextColor(dc, RGB(255, 255, 255));
    RECT rect = {0, 0, size, size};
    DrawTextW(dc, &glyph, 1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    pGdiFlush();

    // Convert the coverage mask to premultiplied BGRA.
    BYTE* pixels = static_cast<BYTE*>(bits);
    for (int i = 0; i < size * size; i++) {
        BYTE* pixel = pixels + i * 4;
        BYTE alpha = pixel[0];
        if (pixel[1] > alpha) alpha = pixel[1];
        if (pixel[2] > alpha) alpha = pixel[2];
        pixel[0] = static_cast<BYTE>((GetBValue(color) * alpha) / 255);
        pixel[1] = static_cast<BYTE>((GetGValue(color) * alpha) / 255);
        pixel[2] = static_cast<BYTE>((GetRValue(color) * alpha) / 255);
        pixel[3] = alpha;
    }

    pSelectObject(dc, oldFont);
    pSelectObject(dc, oldBitmap);
    pDeleteObject(font);
    pDeleteDC(dc);

    ICONINFO iconInfo = {};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmColor = bitmap;
    iconInfo.hbmMask = mask;
    HICON icon = CreateIconIndirect(&iconInfo);
    pDeleteObject(bitmap);
    pDeleteObject(mask);
    return icon;
}

static std::wstring GetProperty(IPropertyStore* store, const PROPERTYKEY& key) {
    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring result;
    if (SUCCEEDED(store->GetValue(key, &value)) && value.vt == VT_LPWSTR && value.pwszVal) {
        result = value.pwszVal;
    }
    pPropVariantClear(&value);
    return result;
}

static bool EnsureEnumerator() {
    if (g_enumerator) return true;
    HRESULT result = pCoCreateInstance(kDeviceEnumerator, nullptr, CLSCTX_INPROC_SERVER,
                                       __uuidof(IMMDeviceEnumerator),
                                       reinterpret_cast<void**>(&g_enumerator));
    return SUCCEEDED(result);
}

static bool FindEndpoint() {
    if (!EnsureEnumerator()) return false;

    if (!g_endpointOverride.empty()) {
        g_cachedEndpoint = g_endpointOverride;
        return true;
    }

    // Accept an exact Windows endpoint ID in the main device field.
    if (g_deviceId.rfind(L"{0.0.", 0) == 0) {
        g_cachedEndpoint = g_deviceId;
        return true;
    }

    std::wstring adapter;
    std::wstring description;
    EDataFlow flow = eAll;
    bool structured = false;

    size_t marker = g_deviceId.find(L"\\Device\\");
    if (marker != std::wstring::npos) {
        size_t lastSlash = g_deviceId.rfind(L'\\');
        if (lastSlash > marker + 8) {
            adapter = g_deviceId.substr(0, marker);
            description = g_deviceId.substr(marker + 8, lastSlash - marker - 8);
            std::wstring direction = g_deviceId.substr(lastSlash + 1);
            if (EqualText(direction, L"Capture")) {
                flow = eCapture;
                structured = true;
            } else if (EqualText(direction, L"Render")) {
                flow = eRender;
                structured = true;
            }
        }
    }

    IMMDeviceCollection* collection = nullptr;
    if (FAILED(g_enumerator->EnumAudioEndpoints(flow, DEVICE_STATEMASK_ALL, &collection))) {
        return false;
    }

    UINT count = 0;
    collection->GetCount(&count);
    UINT matches = 0;
    std::wstring matchedId;
    std::wstring matchedName;

    for (UINT i = 0; i < count; i++) {
        IMMDevice* device = nullptr;
        if (FAILED(collection->Item(i, &device))) continue;

        IPropertyStore* store = nullptr;
        if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, &store))) {
            std::wstring name = GetProperty(store, kDeviceFriendlyName);
            std::wstring desc = GetProperty(store, kDeviceDescription);
            std::wstring adapterName = GetProperty(store, kAdapterFriendlyName);

            bool match;
            if (structured) {
                match = (EqualText(adapterName, adapter) && EqualText(desc, description)) ||
                        EqualText(name, description + L" (" + adapter + L")");
            } else {
                match = EqualText(name, g_deviceId) || EqualText(desc, g_deviceId);
            }

            if (match) {
                LPWSTR id = nullptr;
                if (SUCCEEDED(device->GetId(&id))) {
                    matches++;
                    matchedId = id;
                    matchedName = name;
                    pCoTaskMemFree(id);
                }
            }
            store->Release();
        }
        device->Release();
    }
    collection->Release();

    // Do not guess when multiple devices match.
    if (matches != 1) return false;

    g_cachedEndpoint = matchedId;
    g_displayName = matchedName;
    Wh_Log(L"Matched audio endpoint: %s (%s)", g_displayName.c_str(), g_cachedEndpoint.c_str());
    return true;
}

static void RefreshTray() {
    HICON icon = g_unknownIcon;
    if (g_stateKnown) {
        icon = (g_state == DEVICE_STATE_DISABLED) ? g_disabledIcon : g_enabledIcon;
    }

    std::wstring name = g_displayName.empty() ? L"Audio device" : g_displayName;
    std::wstring tip = name + L" - " + g_statusText;
    if (tip.size() >= ARRAYSIZE(g_nid.szTip)) tip.resize(ARRAYSIZE(g_nid.szTip) - 1);

    g_nid.hIcon = icon;
    StringCchCopyW(g_nid.szTip, ARRAYSIZE(g_nid.szTip), tip.c_str());

    if (g_iconAdded) {
        if (!Shell_NotifyIconW(NIM_MODIFY, &g_nid)) g_iconAdded = false;
    }
    if (!g_iconAdded) {
        if (Shell_NotifyIconW(NIM_ADD, &g_nid)) {
            g_iconAdded = true;
            Wh_Log(L"The tray icon was added successfully.");
        }
    }
}

static void SynchronizeState() {
    bool known = false;
    DWORD state = 0;

    if (EnsureEnumerator()) {
        if (g_cachedEndpoint.empty()) FindEndpoint();
        if (!g_cachedEndpoint.empty()) {
            IMMDevice* device = nullptr;
            if (SUCCEEDED(g_enumerator->GetDevice(g_cachedEndpoint.c_str(), &device))) {
                known = SUCCEEDED(device->GetState(&state));
                device->Release();
            }
        }
    }

    if (!known) {
        g_cachedEndpoint.clear();
        // Recreate the enumerator on the next attempt.
        if (g_enumerator) {
            g_enumerator->Release();
            g_enumerator = nullptr;
        }
    }

    bool changed = (known != g_stateKnown) || (known && state != g_state);
    g_stateKnown = known;
    g_state = state;

    if (!known) {
        g_statusText = L"State unknown. Check the device ID or endpoint ID.";
    } else if (state == DEVICE_STATE_DISABLED) {
        g_statusText = L"Disabled";
    } else if (state == DEVICE_STATE_ACTIVE) {
        g_statusText = L"Enabled";
    } else if (state == DEVICE_STATE_UNPLUGGED) {
        g_statusText = L"Enabled, but unplugged";
    } else if (state == DEVICE_STATE_NOTPRESENT) {
        g_statusText = L"Enabled, but not present";
    } else {
        g_statusText = L"Unknown device state";
    }

    if (changed) {
        Wh_Log(L"Verified device state: %s (0x%08lX).", g_statusText.c_str(), state);
    }

    if (g_pendingVerification) {
        if (known && ((state == DEVICE_STATE_DISABLED) != (g_stateBeforeToggle == DEVICE_STATE_DISABLED))) {
            g_pendingVerification = false;
            Wh_Log(L"Device toggle verified successfully.");
        } else if (GetTickCount64() > g_verificationDeadline) {
            g_pendingVerification = false;
            Wh_Log(L"Toggle verification timed out. The icon remains synchronized with the actual state.");
        } else {
            g_statusText += L" - waiting for device change";
        }
    }

    RefreshTray();
}

static bool OpenTarget(HWND owner, const std::wstring& target, PCWSTR arguments = nullptr) {
    return reinterpret_cast<INT_PTR>(
               ShellExecuteW(owner, L"open", target.c_str(), arguments, nullptr, SW_SHOWNORMAL)) > 32;
}

static void OpenWindhawk(HWND owner) {
    // 1. Path of the running Windhawk process.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W entry = {};
        entry.dwSize = sizeof(entry);
        if (Process32FirstW(snapshot, &entry)) {
            do {
                if (_wcsicmp(entry.szExeFile, L"windhawk.exe") != 0) continue;
                HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID);
                if (!process) continue;
                WCHAR path[32768];
                DWORD length = ARRAYSIZE(path);
                BOOL found = QueryFullProcessImageNameW(process, 0, path, &length);
                CloseHandle(process);
                if (found && OpenTarget(owner, path)) {
                    CloseHandle(snapshot);
                    return;
                }
            } while (Process32NextW(snapshot, &entry));
        }
        CloseHandle(snapshot);
    }

    // 2. Default installation folders.
    const wchar_t* candidates[] = {
        L"%ProgramFiles%\\Windhawk\\windhawk.exe",
        L"%ProgramFiles(x86)%\\Windhawk\\windhawk.exe",
    };
    for (const wchar_t* candidate : candidates) {
        WCHAR path[32768];
        DWORD length = ExpandEnvironmentStringsW(candidate, path, ARRAYSIZE(path));
        if (length > 0 && length <= ARRAYSIZE(path) &&
            GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES) {
            if (OpenTarget(owner, path)) return;
        }
    }

    MessageBoxW(owner, L"Could not find or open Windhawk.",
                L"Audio Device Toggle", MB_OK | MB_ICONERROR);
}

static void OpenAudioSettings(HWND owner) {
    WCHAR directory[MAX_PATH];
    GetSystemDirectoryW(directory, ARRAYSIZE(directory));
    OpenTarget(owner, std::wstring(directory) + L"\\control.exe", L"mmsys.cpl,,1");
}

static void ShowContextMenu(HWND owner) {
    HMENU menu = CreatePopupMenu();
    if (!menu) return;

    AppendMenuW(menu, MF_STRING, MENU_WINDHAWK, L"Windhawk Settings");
    AppendMenuW(menu, MF_STRING, MENU_AUDIO, L"Audio Settings");

    POINT position;
    GetCursorPos(&position);
    SetForegroundWindow(owner);
    UINT command = TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON,
                                    position.x, position.y, owner, nullptr);
    DestroyMenu(menu);
    PostMessageW(owner, WM_NULL, 0, 0);

    if (command == MENU_WINDHAWK) OpenWindhawk(owner);
    if (command == MENU_AUDIO) OpenAudioSettings(owner);
}

static void ToggleDevice(HWND owner) {
    if (g_toggleProcess || g_pendingVerification) return;

    SynchronizeState();
    if (!g_stateKnown) {
        MessageBoxW(owner,
                    L"The actual device state could not be verified. "
                    L"Check the SoundVolumeView device ID, or enter the "
                    L"exact Windows audio endpoint ID in the mod settings.",
                    L"Audio Device Toggle", MB_OK | MB_ICONWARNING);
        return;
    }

    std::wstring executable = ReadSetting(L"SoundVolumeViewPath");
    if (executable.empty() || executable.find(L'"') != std::wstring::npos ||
        g_deviceId.find(L'"') != std::wstring::npos) {
        Wh_Log(L"Invalid SoundVolumeView path or device ID.");
        return;
    }

    std::wstring command = L"\"" + executable + L"\" /DisableEnable \"" + g_deviceId + L"\"";

    STARTUPINFOW startup = {};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process = {};
    if (!CreateProcessW(executable.c_str(), command.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &startup, &process)) {
        Wh_Log(L"Could not launch SoundVolumeView. Error: %lu.", GetLastError());
        return;
    }

    CloseHandle(process.hThread);
    g_toggleProcess = process.hProcess;

    // Only verified state changes update the icon.
    g_stateBeforeToggle = g_state;
    g_pendingVerification = true;
    g_verificationDeadline = GetTickCount64() + 8000;
    g_nextCheck = 0;
}

static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (g_taskbarCreated && message == g_taskbarCreated) {
        g_iconAdded = false;
        g_nextCheck = 0;
        return 0;
    }

    switch (message) {
        case WM_TRAY_CALLBACK:
            if (lParam == WM_LBUTTONUP) {
                ToggleDevice(window);
            } else if (lParam == WM_RBUTTONUP || lParam == WM_CONTEXTMENU) {
                ShowContextMenu(window);
            }
            return 0;

        case WM_TIMER:
            if (wParam == SYNC_TIMER) {
                if (g_toggleProcess && WaitForSingleObject(g_toggleProcess, 0) == WAIT_OBJECT_0) {
                    CloseHandle(g_toggleProcess);
                    g_toggleProcess = nullptr;
                    g_nextCheck = 0;
                }
                ULONGLONG now = GetTickCount64();
                if (now >= g_nextCheck) {
                    SynchronizeState();
                    g_nextCheck = now + (g_pendingVerification ? POLL_AFTER_TOGGLE_MS : g_syncInterval);
                }
            }
            return 0;

        case WM_CLOSE:
            DestroyWindow(window);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static DWORD WINAPI WorkerThread(LPVOID) {
    bool comInitialized = false;
    HWND window = nullptr;
    ATOM atom = 0;
    HINSTANCE instance = GetModuleHandleW(nullptr);
    WCHAR className[160];

    if (LoadFunctions()) {
        HRESULT result = pCoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        comInitialized = SUCCEEDED(result);
        if (comInitialized) {
            g_deviceId = ReadSetting(L"DeviceId");
            g_endpointOverride = ReadSetting(L"EndpointId");
            g_cachedEndpoint.clear();
            g_displayName.clear();
            g_statusText = L"Checking device state";
            g_stateKnown = false;
            g_state = 0;
            g_iconAdded = false;
            g_pendingVerification = false;
            g_nextCheck = 0;

            int intervalSeconds = Wh_GetIntSetting(L"SyncIntervalSeconds");
            if (intervalSeconds < 1) intervalSeconds = 300;
            g_syncInterval = static_cast<ULONGLONG>(intervalSeconds) * 1000ULL;

            COLORREF color = GetIconColor();
            g_enabledIcon = CreateGlyphIcon(ParseGlyph(ReadSetting(L"EnabledIconGlyph"), 0xE7F5), color);
            g_disabledIcon = CreateGlyphIcon(ParseGlyph(ReadSetting(L"DisabledIconGlyph"), 0xE720), color);
            // Show a question mark when verification is unavailable.
            g_unknownIcon = CreateGlyphIcon(L'?', color);

            StringCchPrintfW(className, ARRAYSIZE(className), L"WindhawkAudioToggle_%lu_%lu_%llu",
                             GetCurrentProcessId(), GetCurrentThreadId(), GetTickCount64());

            WNDCLASSW wc = {};
            wc.lpfnWndProc = WindowProc;
            wc.hInstance = instance;
            wc.lpszClassName = className;

            if (g_enabledIcon && g_disabledIcon && g_unknownIcon) {
                atom = RegisterClassW(&wc);
                if (atom) {
                    window = CreateWindowExW(WS_EX_TOOLWINDOW, className, L"Audio Device Toggle",
                                             WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance, nullptr);
                }
            }

            if (window) {
                g_taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

                ZeroMemory(&g_nid, sizeof(g_nid));
                g_nid.cbSize = sizeof(g_nid);
                g_nid.hWnd = window;
                g_nid.uID = 0x4A45;
                g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
                g_nid.uCallbackMessage = WM_TRAY_CALLBACK;

                SetTimer(window, SYNC_TIMER, 250, nullptr);
                SynchronizeState();

                bool running = true;
                while (running) {
                    DWORD wait = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
                    if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;

                    MSG msg;
                    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                        if (msg.message == WM_QUIT) {
                            running = false;
                            break;
                        }
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                }
            } else {
                Wh_Log(L"Could not create the tray window or icons.");
            }
        } else {
            Wh_Log(L"Could not initialize COM.");
        }
    } else {
        Wh_Log(L"Could not load the required Windows functions.");
    }

    if (window) KillTimer(window, SYNC_TIMER);
    if (g_iconAdded) {
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        g_iconAdded = false;
    }
    if (window && IsWindow(window)) DestroyWindow(window);
    if (atom) UnregisterClassW(className, instance);
    if (g_toggleProcess) {
        CloseHandle(g_toggleProcess);
        g_toggleProcess = nullptr;
    }
    if (g_enumerator) {
        g_enumerator->Release();
        g_enumerator = nullptr;
    }
    if (g_enabledIcon) { DestroyIcon(g_enabledIcon); g_enabledIcon = nullptr; }
    if (g_disabledIcon) { DestroyIcon(g_disabledIcon); g_disabledIcon = nullptr; }
    if (g_unknownIcon) { DestroyIcon(g_unknownIcon); g_unknownIcon = nullptr; }
    if (comInitialized) pCoUninitialize();
    if (g_gdi) { FreeLibrary(g_gdi); g_gdi = nullptr; }
    if (g_ole) { FreeLibrary(g_ole); g_ole = nullptr; }
    return 0;
}

BOOL Wh_ModInit() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return FALSE;

    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
}
