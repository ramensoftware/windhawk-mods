// ==WindhawkMod==
// @id              charging-sound
// @name            Charging Sound
// @description     Plays a custom sound when the laptop is plugged in
// @version         1.0
// @author          khonloi
// @github          https://github.com/khonloi
// @include         explorer.exe
// @compilerOptions -luser32 -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Charging Sound
Plays a custom `.wav` sound whenever you connect your laptop to a charger (AC
power). By default it plays the Windows Notify Messaging sound.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- soundPath: "C:\\Windows\\Media\\Windows Notify Messaging.wav"
  $name: Sound File Path
  $description: The absolute path to a .wav file to play when plugged in.
*/
// ==/WindhawkModSettings==

#include <mmsystem.h>
#include <windows.h>
#include <string>

const GUID MY_GUID_ACDC_POWER_SOURCE = {
    0x5d3e9a59,
    0xe9d5,
    0x4b00,
    {0xa6, 0xbd, 0xff, 0x34, 0xfa, 0xc0, 0xbc, 0x27}};

struct {
    std::wstring soundPath;
} settings;

HWND g_hwnd = NULL;
HANDLE g_hThread = NULL;
DWORD g_threadId = 0;
int g_previousACStatus = -1;

LRESULT CALLBACK PowerWindowProc(HWND hwnd,
                                 UINT uMsg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    if (uMsg == WM_POWERBROADCAST) {
        if (wParam == PBT_APMPOWERSTATUSCHANGE) {
            SYSTEM_POWER_STATUS sps;
            if (GetSystemPowerStatus(&sps)) {
                int currentACStatus = sps.ACLineStatus;
                Wh_Log(
                    L"Power status change detected. Current ACLineStatus: %d",
                    currentACStatus);

                // Only trigger if we have a valid previous state and it
                // actually changed
                if (g_previousACStatus != -1 &&
                    currentACStatus != g_previousACStatus) {
                    if (currentACStatus == 1) {  // Plugged in
                        Wh_Log(L"Laptop plugged in. Playing sound: %s",
                               settings.soundPath.c_str());
                        PlaySoundW(settings.soundPath.c_str(), NULL,
                                   SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
                    } else if (currentACStatus == 0) {  // Unplugged
                        Wh_Log(L"Laptop unplugged.");
                    }
                }
                g_previousACStatus = currentACStatus;
            }
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI PowerNotifyThread(LPVOID lpParam) {
    Wh_Log(L"PowerNotifyThread started.");

    // Initialize the previous status to the current status so it doesn't play
    // immediately on launch
    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps)) {
        g_previousACStatus = sps.ACLineStatus;
    }

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = PowerWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkChargingSoundHiddenWindow";

    RegisterClassExW(&wc);

    g_hwnd = CreateWindowExW(0, wc.lpszClassName, L"WindhawkChargingSound",
                             WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL,
                             wc.hInstance, NULL);

    if (g_hwnd) {
        Wh_Log(L"Hidden window created successfully.");

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        DestroyWindow(g_hwnd);
    } else {
        Wh_Log(L"CreateWindowExW failed with error: %d", GetLastError());
    }

    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    Wh_Log(L"PowerNotifyThread exited.");
    return 0;
}

void LoadSettings() {
    PCWSTR soundPath = Wh_GetStringSetting(L"soundPath");
    if (soundPath) {
        settings.soundPath = soundPath;
        Wh_FreeStringSetting(soundPath);
    } else {
        settings.soundPath =
            L"C:\\Windows\\Media\\Windows Notify Messaging.wav";
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    g_hThread = CreateThread(NULL, 0, PowerNotifyThread, NULL, 0, &g_threadId);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_threadId != 0) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        if (g_hThread) {
            WaitForSingleObject(g_hThread, INFINITE);
            CloseHandle(g_hThread);
            g_hThread = NULL;
        }
        g_threadId = 0;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
