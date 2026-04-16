// ==WindhawkMod==
// @id              audioswap
// @name            AudioSwap
// @description     Adds a tray icon to instantly toggle between two preferred audio outputs.
// @version         1.1.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -luser32 -lole32 -luuid -loleaut32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AudioSwap
Instantly toggle between two audio output devices from your system tray — no diving into Sound settings.

---

## How to Use

1. **Choose Icons** — Open the **Settings** tab and pick an icon for each of your two devices.
2. **Select Your Devices** — Right-click the tray icon. Use **Set as Device 1** and **Set as Device 2** to assign your outputs from the live device list.
3. **Toggle** — Left-click the tray icon at any time to swap between the two devices instantly.

> The tray tooltip always shows the active device. On first run it will read *"Right-click to configure"* until both devices are assigned.

---

## Changelog

### v1.1.0
- **New:** Right-click context menu — auto-detects all active audio outputs and lets you assign Device 1 and Device 2 directly from a live list. No more typing device names manually.
- **New:** Device selections persist across restarts.
- **Improved:** Toggle now matches devices by their unique system ID instead of a name substring search — works correctly regardless of how Windows names your device.
- **Improved:** Tray tooltip prompts you to configure on first run instead of showing "Unknown Device".
- **Removed:** Device name text fields from the Settings tab (replaced by the right-click menu).

### v1.0.1
- Initial release.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- icon1: headphones1
  $name: First Device Icon
  $options:
    - headphones1: Headphones (normal)
    - headphones2: Modern Headset (white)
    - speaker1: Basic Speaker (normal)
    - speaker2: Modern Speaker (white)
- icon2: speaker1
  $name: Second Device Icon
  $options:
    - headphones1: Headphones (normal)
    - headphones2: Modern Headset (white)
    - speaker1: Basic Speaker (normal)
    - speaker2: Modern Speaker (white)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <shlwapi.h>
#include <shobjidl.h>

#include <propkey.h>
#include <mmdeviceapi.h>
#include <propidl.h>
#include <functiondiscoverykeys_devpkey.h>

#define TRAY_ICON_ID 1
#define WM_TRAY_CALLBACK (WM_USER + 1)
#define WM_UPDATE_TRAY_STATE (WM_USER + 2)
#define MENU_DEVICE1_BASE 1000
#define MENU_DEVICE2_BASE 2000
#define MENU_MAX_DEVICES  32

const DWORD CLICK_DEBOUNCE_MS = 500;

static volatile LONG g_isProcessingClick = 0;
static HANDLE g_trayThread = nullptr;
static HANDLE g_workerThread = nullptr;
static HWND g_trayHwnd = nullptr;
static HINSTANCE g_hInstance = nullptr;
static HICON g_iconDev1 = nullptr;
static HICON g_iconDev2 = nullptr;
static DWORD g_lastClickTime = 0;
static UINT g_taskbarCreatedMsg = 0;

// Cached device selections (loaded from Windhawk storage)
static WCHAR g_cachedDev1Id[512]   = {0};
static WCHAR g_cachedDev1Name[256] = {0};
static WCHAR g_cachedDev2Id[512]   = {0};
static WCHAR g_cachedDev2Name[256] = {0};

const CLSID CLSID_CPolicyConfigClient = {
    0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}
};
const IID IID_IPolicyConfig_Win10_11 = {
    0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}
};

MIDL_INTERFACE("f8679f50-850a-41cf-9c72-430f290290c8")
IPolicyConfig : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, void*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT, PINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR wszDeviceId, ERole eRole) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};

int GetIconIndex(PCWSTR iconSetting) {
    if (iconSetting) {
        if (wcscmp(iconSetting, L"headphones1") == 0) return 2;
        if (wcscmp(iconSetting, L"headphones2") == 0) return 91;
        if (wcscmp(iconSetting, L"speaker1") == 0) return 4;
        if (wcscmp(iconSetting, L"speaker2") == 0) return 93;
    }
    return 4;
}

void LoadDeviceSelections() {
    g_cachedDev1Id[0] = g_cachedDev1Name[0] = L'\0';
    g_cachedDev2Id[0] = g_cachedDev2Name[0] = L'\0';

    Wh_GetStringValue(L"Device1Id",   g_cachedDev1Id,   ARRAYSIZE(g_cachedDev1Id));
    Wh_GetStringValue(L"Device1Name", g_cachedDev1Name, ARRAYSIZE(g_cachedDev1Name));
    Wh_GetStringValue(L"Device2Id",   g_cachedDev2Id,   ARRAYSIZE(g_cachedDev2Id));
    Wh_GetStringValue(L"Device2Name", g_cachedDev2Name, ARRAYSIZE(g_cachedDev2Name));
}

void SaveDeviceSelection(int slot, PCWSTR deviceId, PCWSTR friendlyName) {
    if (slot == 1) {
        Wh_SetStringValue(L"Device1Id",   deviceId);
        Wh_SetStringValue(L"Device1Name", friendlyName);
        lstrcpynW(g_cachedDev1Id,   deviceId,     512);
        lstrcpynW(g_cachedDev1Name, friendlyName, 256);
    } else {
        Wh_SetStringValue(L"Device2Id",   deviceId);
        Wh_SetStringValue(L"Device2Name", friendlyName);
        lstrcpynW(g_cachedDev2Id,   deviceId,     512);
        lstrcpynW(g_cachedDev2Name, friendlyName, 256);
    }
}

void LoadUserIconsAndSettings() {
    if (g_iconDev1) DestroyIcon(g_iconDev1);
    if (g_iconDev2) DestroyIcon(g_iconDev2);

    PCWSTR s1 = Wh_GetStringSetting(L"icon1");
    PCWSTR s2 = Wh_GetStringSetting(L"icon2");

    ExtractIconExW(L"ddores.dll", GetIconIndex(s1), nullptr, &g_iconDev1, 1);
    ExtractIconExW(L"ddores.dll", GetIconIndex(s2), nullptr, &g_iconDev2, 1);

    if (s1) Wh_FreeStringSetting(s1);
    if (s2) Wh_FreeStringSetting(s2);
}

void UpdateTrayTip(HWND hWnd, BOOL isAdd) {
    WCHAR currentDev[256] = L"Unknown Device";
    WCHAR currentId[512]  = {0};
    HICON currentIcon = g_iconDev1;

    IMMDeviceEnumerator* pEnum = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        IMMDevice* pDefaultDevice = nullptr;
        if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice))) {
            LPWSTR pId = nullptr;
            if (SUCCEEDED(pDefaultDevice->GetId(&pId))) {
                lstrcpynW(currentId, pId, 512);
                CoTaskMemFree(pId);
            }
            IPropertyStore* pStore = nullptr;
            if (SUCCEEDED(pDefaultDevice->OpenPropertyStore(STGM_READ, &pStore))) {
                PROPVARIANT varName;
                PropVariantInit(&varName);
                if (SUCCEEDED(pStore->GetValue(PKEY_Device_FriendlyName, &varName)) && varName.pwszVal)
                    lstrcpynW(currentDev, varName.pwszVal, 256);
                PropVariantClear(&varName);
                pStore->Release();
            }
            pDefaultDevice->Release();
        }
        pEnum->Release();
    }

    // ID-based icon selection — exact match, no fuzzy strings
    if (g_cachedDev1Id[0] != L'\0' && wcscmp(currentId, g_cachedDev1Id) == 0)
        currentIcon = g_iconDev1;
    else if (g_cachedDev2Id[0] != L'\0' && wcscmp(currentId, g_cachedDev2Id) == 0)
        currentIcon = g_iconDev2;

    // CPU opt: skip redraw if nothing changed
    static WCHAR s_lastDev[256] = {0};
    static HICON s_lastIcon = nullptr;
    if (!isAdd && wcscmp(currentDev, s_lastDev) == 0 && currentIcon == s_lastIcon)
        return;
    lstrcpyW(s_lastDev, currentDev);
    s_lastIcon = currentIcon;

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd = hWnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;

    if (g_cachedDev1Id[0] == L'\0' || g_cachedDev2Id[0] == L'\0')
        swprintf_s(nid.szTip, L"AudioSwap: Right-click to configure");
    else
        swprintf_s(nid.szTip, L"Audio: %s", currentDev);

    nid.hIcon = currentIcon;
    Shell_NotifyIconW(isAdd ? NIM_ADD : NIM_MODIFY, &nid);
}

BOOL ToggleAudioDevice() {
    // Guard: no-op if devices not configured yet
    if (g_cachedDev1Id[0] == L'\0' || g_cachedDev2Id[0] == L'\0') {
        return FALSE;
    }

    CoInitialize(nullptr);
    IMMDeviceEnumerator* pEnum = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        CoUninitialize(); return FALSE;
    }

    IMMDevice* pDefaultDevice = nullptr;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice))) {
        pEnum->Release(); CoUninitialize(); return FALSE;
    }

    LPWSTR currentId = nullptr;
    pDefaultDevice->GetId(&currentId);
    pDefaultDevice->Release();

    // Pick the other device by comparing IDs directly — no fuzzy name search
    PCWSTR targetId = (currentId && wcscmp(currentId, g_cachedDev1Id) == 0)
                      ? g_cachedDev2Id
                      : g_cachedDev1Id;

    if (currentId) CoTaskMemFree(currentId);

    IPolicyConfig* pPolicyConfig = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_CPolicyConfigClient, nullptr, CLSCTX_ALL,
                                   IID_IPolicyConfig_Win10_11, (void**)&pPolicyConfig))) {
        pPolicyConfig->SetDefaultEndpoint(targetId, eConsole);
        pPolicyConfig->SetDefaultEndpoint(targetId, eMultimedia);
        pPolicyConfig->SetDefaultEndpoint(targetId, eCommunications);
        pPolicyConfig->Release();
    }

    pEnum->Release();
    CoUninitialize();
    return TRUE;
}

struct AudioDevice {
    WCHAR id[512];
    WCHAR name[256];
};

void BuildAndShowContextMenu(HWND hWnd) {
    AudioDevice devices[MENU_MAX_DEVICES];
    int deviceCount = 0;

    // Enumerate active render endpoints (COM already init on tray thread)
    IMMDeviceEnumerator* pEnum = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                   __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        IMMDeviceCollection* pDevices = nullptr;
        if (SUCCEEDED(pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices))) {
            UINT count = 0;
            pDevices->GetCount(&count);
            if (count > MENU_MAX_DEVICES) count = MENU_MAX_DEVICES;

            for (UINT i = 0; i < count; i++) {
                IMMDevice* pDevice = nullptr;
                if (FAILED(pDevices->Item(i, &pDevice))) continue;

                LPWSTR pId = nullptr;
                if (SUCCEEDED(pDevice->GetId(&pId))) {
                    lstrcpynW(devices[deviceCount].id, pId, 512);
                    CoTaskMemFree(pId);

                    IPropertyStore* pStore = nullptr;
                    if (SUCCEEDED(pDevice->OpenPropertyStore(STGM_READ, &pStore))) {
                        PROPVARIANT varName;
                        PropVariantInit(&varName);
                        if (SUCCEEDED(pStore->GetValue(PKEY_Device_FriendlyName, &varName)) && varName.pwszVal) {
                            lstrcpynW(devices[deviceCount].name, varName.pwszVal, 256);
                            deviceCount++;
                        }
                        PropVariantClear(&varName);
                        pStore->Release();
                    }
                }
                pDevice->Release();
            }
            pDevices->Release();
        }
        pEnum->Release();
    }

    // Build submenus
    HMENU hSub1 = CreatePopupMenu();
    HMENU hSub2 = CreatePopupMenu();

    for (int i = 0; i < deviceCount; i++) {
        UINT flags1 = MF_STRING | (wcscmp(devices[i].id, g_cachedDev1Id) == 0 ? MF_CHECKED : 0);
        UINT flags2 = MF_STRING | (wcscmp(devices[i].id, g_cachedDev2Id) == 0 ? MF_CHECKED : 0);
        AppendMenuW(hSub1, flags1, MENU_DEVICE1_BASE + i, devices[i].name);
        AppendMenuW(hSub2, flags2, MENU_DEVICE2_BASE + i, devices[i].name);
    }

    // Assemble root menu
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSub1, L"Set as Device 1");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSub2, L"Set as Device 2");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

    // Status line — get current active device name
    WCHAR statusText[300];
    if (g_cachedDev1Id[0] == L'\0' || g_cachedDev2Id[0] == L'\0') {
        lstrcpyW(statusText, L"Right-click to configure devices");
    } else {
        WCHAR activeName[256] = L"Unknown";
        IMMDeviceEnumerator* pEnum2 = nullptr;
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                       __uuidof(IMMDeviceEnumerator), (void**)&pEnum2))) {
            IMMDevice* pDefault = nullptr;
            if (SUCCEEDED(pEnum2->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefault))) {
                IPropertyStore* pStore = nullptr;
                if (SUCCEEDED(pDefault->OpenPropertyStore(STGM_READ, &pStore))) {
                    PROPVARIANT v; PropVariantInit(&v);
                    if (SUCCEEDED(pStore->GetValue(PKEY_Device_FriendlyName, &v)) && v.pwszVal)
                        lstrcpynW(activeName, v.pwszVal, 256);
                    PropVariantClear(&v);
                    pStore->Release();
                }
                pDefault->Release();
            }
            pEnum2->Release();
        }
        swprintf_s(statusText, L"Active: %s", activeName);
    }
    AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, statusText);

    // Show menu at cursor
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hWnd);
    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
                             pt.x, pt.y, 0, hWnd, nullptr);
    PostMessageW(hWnd, WM_NULL, 0, 0); // flush menu

    DestroyMenu(hMenu); // also destroys attached submenus

    // Handle selection
    if (cmd >= MENU_DEVICE1_BASE && cmd < MENU_DEVICE1_BASE + deviceCount) {
        int idx = cmd - MENU_DEVICE1_BASE;
        SaveDeviceSelection(1, devices[idx].id, devices[idx].name);
        PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
    } else if (cmd >= MENU_DEVICE2_BASE && cmd < MENU_DEVICE2_BASE + deviceCount) {
        int idx = cmd - MENU_DEVICE2_BASE;
        SaveDeviceSelection(2, devices[idx].id, devices[idx].name);
        PostMessageW(hWnd, WM_UPDATE_TRAY_STATE, 0, 0);
    }
}

DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    if (ToggleAudioDevice() && g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, 0, 0);
    }
    InterlockedExchange(&g_isProcessingClick, 0);
    return 0;
}

LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TRAY_CALLBACK && lParam == WM_RBUTTONUP) {
        BuildAndShowContextMenu(hWnd);
    } else if (msg == WM_TRAY_CALLBACK && lParam == WM_LBUTTONUP) {
        if (InterlockedExchange(&g_isProcessingClick, 1) == 0) {
            DWORD now = GetTickCount();
            if (now - g_lastClickTime > CLICK_DEBOUNCE_MS) { // Corrected check
                g_lastClickTime = now;
                if (g_workerThread) { CloseHandle(g_workerThread); g_workerThread = nullptr; }
                g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
            } else InterlockedExchange(&g_isProcessingClick, 0);
        }
    } else if (msg == WM_UPDATE_TRAY_STATE || (msg == WM_TIMER && wParam == 1)) {
        UpdateTrayTip(hWnd, FALSE);
    } else if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        UpdateTrayTip(hWnd, TRUE);
    } else if (msg == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

DWORD WINAPI TrayThreadProc(LPVOID) {
    CoInitialize(nullptr);
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"AudioSwitcherWindowClass";
    RegisterClassW(&wc);
    g_trayHwnd = CreateWindowExW(0, wc.lpszClassName, L"Audio Switcher", WS_OVERLAPPED, 0,0,1,1, nullptr, nullptr, g_hInstance, nullptr);
    
    IPropertyStore* pps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(g_trayHwnd, IID_PPV_ARGS(&pps)))) {
        PROPVARIANT var; var.vt = VT_LPWSTR; var.pwszVal = (LPWSTR)CoTaskMemAlloc(MAX_PATH);
        if (var.pwszVal) {
            lstrcpyW(var.pwszVal, L"BlackPaw.AudioSwitcher");
            pps->SetValue(PKEY_AppUserModel_ID, var);
            CoTaskMemFree(var.pwszVal);
        }
        pps->Commit(); pps->Release();
    }

    SetTimer(g_trayHwnd, 1, 1500, nullptr);
    UpdateTrayTip(g_trayHwnd, TRUE);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) { DispatchMessageW(&msg); }
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"AudioSwap Mod Init");
    g_hInstance = GetModuleHandle(nullptr);
    LoadUserIconsAndSettings();
    LoadDeviceSelections();
    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadUserIconsAndSettings();
    LoadDeviceSelections();
    if (g_trayHwnd) PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L"AudioSwap Mod Uninit");
    if (g_trayHwnd) PostMessageW(g_trayHwnd, WM_DESTROY, 0, 0);
    if (g_trayThread) { WaitForSingleObject(g_trayThread, 2000); CloseHandle(g_trayThread); g_trayThread = nullptr; }
    if (g_workerThread) { WaitForSingleObject(g_workerThread, 2000); CloseHandle(g_workerThread); g_workerThread = nullptr; }
    if (g_iconDev1) { DestroyIcon(g_iconDev1); g_iconDev1 = nullptr; }
    if (g_iconDev2) { DestroyIcon(g_iconDev2); g_iconDev2 = nullptr; }
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
