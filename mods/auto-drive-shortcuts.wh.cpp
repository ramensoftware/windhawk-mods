// ==WindhawkMod==
// @id              auto-drive-shortcuts
// @name            Auto Drive Shortcuts (macOS/Linux Style)
// @description     Automatically creates desktop shortcuts for inserted USB drives and deletes them when ejected.
// @version         1.0.0
// @author          ChadX
// @github          https://github.com/ritambiswas4567         
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -lshell32
// ==/WindhawkMod==


// ==WindhawkModReadme==
// # Auto Drive Shortcuts
//
// Automatically creates desktop shortcuts for connected USB drives and deletes them when ejected, bringing macOS/Linux desktop behavior to Windows.
//
// ## Features
// - **Automatic Creation:** Generates a shortcut on the Desktop pointing to the drive letter when a removable drive is attached.
// - **Automatic Cleanup:** Removes the shortcut automatically upon drive disconnection or safe ejection.
// - **Mounting Delay:** Includes a small initialization buffer to ensure the drive volume is fully mounted by Windows before shortcut creation.
// ==/WindhawkModReadme==


#include <windows.h>
#include <dbt.h>
#include <shlobj.h>
#include <string>
#include <windhawk_api.h>

HWND g_hwnd = NULL;
HDEVNOTIFY g_hDevNotify = NULL;

std::wstring GetDesktopPath() {
    wchar_t path[MAX_PATH];
    if (SHGetSpecialFolderPathW(NULL, path, CSIDL_DESKTOP, FALSE)) {
        return std::wstring(path);
    }
    return L"";
}

void CreateDriveShortcut(wchar_t driveLetter) {
    Wh_Log(L"Creating shortcut for drive %c", driveLetter);
    
    // Wait 1.5 seconds for the OS to fully mount the drive before making the shortcut
    Sleep(1500); 

    IShellLinkW* psl;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*)&psl);
    if (SUCCEEDED(hr)) {
        std::wstring target = std::wstring(1, driveLetter) + L":\\";
        psl->SetPath(target.c_str());
        psl->SetDescription(L"Removable Drive");

        IPersistFile* ppf;
        hr = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
        if (SUCCEEDED(hr)) {
            std::wstring shortcutPath = GetDesktopPath() + L"\\Drive " + driveLetter + L".lnk";
            hr = ppf->Save(shortcutPath.c_str(), TRUE);
            if (SUCCEEDED(hr)) {
                Wh_Log(L"Successfully saved shortcut to %s", shortcutPath.c_str());
            } else {
                Wh_Log(L"Failed to save shortcut, HRESULT: 0x%X", hr);
            }
            ppf->Release();
        } else {
            Wh_Log(L"QueryInterface for IPersistFile failed");
        }
        psl->Release();
    } else {
        Wh_Log(L"CoCreateInstance for IShellLink failed, HRESULT: 0x%X", hr);
    }
}

void DeleteDriveShortcut(wchar_t driveLetter) {
    Wh_Log(L"Deleting shortcut for drive %c", driveLetter);
    std::wstring shortcutPath = GetDesktopPath() + L"\\Drive " + driveLetter + L".lnk";
    if (DeleteFileW(shortcutPath.c_str())) {
        Wh_Log(L"Successfully deleted shortcut");
    } else {
        Wh_Log(L"Failed to delete shortcut, error: %lu", GetLastError());
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DEVICECHANGE) {
        if (wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE) {
            PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
            if (pHdr && pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                PDEV_BROADCAST_VOLUME pVol = (PDEV_BROADCAST_VOLUME)pHdr;
                for (int i = 0; i < 26; ++i) {
                    if (pVol->dbcv_unitmask & (1 << i)) {
                        wchar_t driveLetter = L'A' + i;
                        Wh_Log(L"Volume event for drive %c (Arrival/Remove: %X)", driveLetter, wParam);
                        if (wParam == DBT_DEVICEARRIVAL) {
                            CreateDriveShortcut(driveLetter);
                        } else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
                            DeleteDriveShortcut(driveLetter);
                        }
                    }
                }
            }
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI DeviceListenerThread(LPVOID lpParam) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkDriveListenerClass";
    RegisterClass(&wc);

    g_hwnd = CreateWindowEx(0, wc.lpszClassName, L"WindhawkDriveListener", 0, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);

    if (!g_hwnd) {
        Wh_Log(L"Failed to create window, error: %lu", GetLastError());
        return 1;
    }

    DEV_BROADCAST_VOLUME filter = { 0 };
    filter.dbcv_size = sizeof(filter);
    filter.dbcv_devicetype = DBT_DEVTYP_VOLUME;
    g_hDevNotify = RegisterDeviceNotification(g_hwnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
    
    if (g_hDevNotify) {
        Wh_Log(L"Successfully registered for device notifications");
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hDevNotify) UnregisterDeviceNotification(g_hDevNotify);
    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Mod Init Triggered!");
    CreateThread(NULL, 0, DeviceListenerThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Mod uninitialized");
    if (g_hwnd) {
        PostMessage(g_hwnd, WM_QUIT, 0, 0);
    }
}
