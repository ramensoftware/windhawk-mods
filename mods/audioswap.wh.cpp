// ==WindhawkMod==
// @id              audioswap
// @name            AudioSwap
// @description     Adds a tray icon to instantly toggle between two preferred audio outputs.
// @version         1.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -luser32 -lole32 -luuid -loleaut32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Audio Output Switcher
A lightning-fast audio toggle right in your taskbar! 🎧🔊

## 📖 How to Use It
1. ⚙️ **Configure Your Devices:** Go to the **Settings** tab. 
   - Type the Name of your device names (e.g., "Headphones" and "Speakers").
   - Select an icon for each device from the dropdown menus.
2. 🔍 **Find the Icon:** Look in your system tray. The icon will change based on which device is currently active!
3. 🎯 **Click to Toggle:** A single click swaps your audio and updates the icon instantly.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- device1: "Headphones"
  $name: First Audio Device Name
  $description: Name of the first device's name.
- icon1: headphones1
  $name: First Device Icon
  $options:
    - headphones1: Headphones (normal)
    - headphones2: Modern Headset (white)
    - speaker1: Basic Speaker (normal)
    - speaker2: Modern Speaker (white)
- device2: "Speakers"
  $name: Second Audio Device Name
  $description: Name of the second device's name.
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

const DWORD CLICK_DEBOUNCE_MS = 500;

static volatile LONG g_isProcessingClick = 0;
static HANDLE g_trayThread = nullptr;
static HANDLE g_workerThread = nullptr; // Track worker thread
static HWND g_trayHwnd = nullptr;
static HINSTANCE g_hInstance = nullptr;
static HICON g_iconDev1 = nullptr; 
static HICON g_iconDev2 = nullptr; 
static DWORD g_lastClickTime = 0;
static UINT g_taskbarCreatedMsg = 0;

// Cached settings
static WCHAR g_cachedDev1[256] = {0};
static WCHAR g_cachedDev2[256] = {0};

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

void LoadUserIconsAndSettings() {
    if (g_iconDev1) DestroyIcon(g_iconDev1);
    if (g_iconDev2) DestroyIcon(g_iconDev2);

    PCWSTR s1 = Wh_GetStringSetting(L"icon1");
    PCWSTR s2 = Wh_GetStringSetting(L"icon2");

    ExtractIconExW(L"ddores.dll", GetIconIndex(s1), nullptr, &g_iconDev1, 1);
    ExtractIconExW(L"ddores.dll", GetIconIndex(s2), nullptr, &g_iconDev2, 1);

    if (s1) Wh_FreeStringSetting(s1);
    if (s2) Wh_FreeStringSetting(s2);

    PCWSTR rawDev1 = Wh_GetStringSetting(L"device1");
    PCWSTR rawDev2 = Wh_GetStringSetting(L"device2");
    if (rawDev1) {
        lstrcpynW(g_cachedDev1, rawDev1, 256);
        Wh_FreeStringSetting(rawDev1);
    }
    if (rawDev2) {
        lstrcpynW(g_cachedDev2, rawDev2, 256);
        Wh_FreeStringSetting(rawDev2);
    }
}

void UpdateTrayTip(HWND hWnd, BOOL isAdd) {
    WCHAR currentDev[256] = L"Unknown Device";
    HICON currentIcon = g_iconDev1; 
    
    IMMDeviceEnumerator *pEnum = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        IMMDevice *pDefaultDevice = nullptr;
        if (SUCCEEDED(pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice))) {
            IPropertyStore *pStore = nullptr;
            if (SUCCEEDED(pDefaultDevice->OpenPropertyStore(STGM_READ, &pStore))) {
                PROPVARIANT varName;
                PropVariantInit(&varName);
                if (SUCCEEDED(pStore->GetValue(PKEY_Device_FriendlyName, &varName)) && varName.pwszVal) {
                    lstrcpynW(currentDev, varName.pwszVal, 256);
                }
                PropVariantClear(&varName);
                pStore->Release();
            }
            pDefaultDevice->Release();
        }
        pEnum->Release();
    }
    
    if (StrStrIW(currentDev, g_cachedDev1)) currentIcon = g_iconDev1;
    else if (StrStrIW(currentDev, g_cachedDev2)) currentIcon = g_iconDev2;

    // FIX: CPU Optimization - Only redraw the shell icon if data actually changed
    static WCHAR s_lastDev[256] = {0};
    static HICON s_lastIcon = nullptr;
    if (!isAdd && wcscmp(currentDev, s_lastDev) == 0 && currentIcon == s_lastIcon) {
        return; 
    }
    lstrcpyW(s_lastDev, currentDev);
    s_lastIcon = currentIcon;

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd = hWnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;
    swprintf_s(nid.szTip, L"Audio: %s", currentDev);
    nid.hIcon = currentIcon;
    
    Shell_NotifyIconW(isAdd ? NIM_ADD : NIM_MODIFY, &nid);
}

BOOL ToggleAudioDevice() {
    CoInitialize(nullptr);
    IMMDeviceEnumerator *pEnum = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum))) {
        CoUninitialize(); return FALSE;
    }

    IMMDevice *pDefaultDevice = nullptr;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice))) {
        pEnum->Release(); CoUninitialize(); return FALSE;
    }

    IPropertyStore *pStore = nullptr;
    pDefaultDevice->OpenPropertyStore(STGM_READ, &pStore);
    PROPVARIANT varName; PropVariantInit(&varName);
    pStore->GetValue(PKEY_Device_FriendlyName, &varName);

    BOOL isDev1Active = (varName.pwszVal && StrStrIW(varName.pwszVal, g_cachedDev1));
    PropVariantClear(&varName); pStore->Release(); pDefaultDevice->Release();

    PCWSTR targetStr = isDev1Active ? g_cachedDev2 : g_cachedDev1;
    IMMDeviceCollection *pDevices = nullptr;
    pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
    UINT count = 0; pDevices->GetCount(&count);

    LPWSTR targetId = nullptr;
    for (UINT i = 0; i < count; i++) {
        IMMDevice *pDevice = nullptr;
        pDevices->Item(i, &pDevice);
        pDevice->OpenPropertyStore(STGM_READ, &pStore);
        PropVariantInit(&varName);
        pStore->GetValue(PKEY_Device_FriendlyName, &varName);
        if (varName.pwszVal && StrStrIW(varName.pwszVal, targetStr)) {
            pDevice->GetId(&targetId);
            PropVariantClear(&varName); pStore->Release(); pDevice->Release();
            break;
        }
        PropVariantClear(&varName); pStore->Release(); pDevice->Release();
    }

    if (targetId) {
        IPolicyConfig *pPolicyConfig = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_CPolicyConfigClient, nullptr, CLSCTX_ALL, IID_IPolicyConfig_Win10_11, (void**)&pPolicyConfig))) {
            pPolicyConfig->SetDefaultEndpoint(targetId, eConsole);
            pPolicyConfig->SetDefaultEndpoint(targetId, eMultimedia);
            pPolicyConfig->SetDefaultEndpoint(targetId, eCommunications);
            pPolicyConfig->Release();
        }
        CoTaskMemFree(targetId);
    }
    pDevices->Release(); pEnum->Release(); CoUninitialize();
    return TRUE;
}

DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    if (ToggleAudioDevice() && g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, 0, 0);
    }
    InterlockedExchange(&g_isProcessingClick, 0);
    return 0;
}

LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_TRAY_CALLBACK && lParam == WM_LBUTTONUP) {
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
    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadUserIconsAndSettings();
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
