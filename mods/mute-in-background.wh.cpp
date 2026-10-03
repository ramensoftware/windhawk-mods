// ==WindhawkMod==
// @id              mute-in-background
// @name            Mute in Background
// @name:zh-CN      后台自动静音
// @description     Automatically mutes the program when it goes to the background and restores the volume when it returns to the foreground.
// @description:zh-CN 当程序切换到后台时自动静音，切回前台时恢复音量
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// @include         mspaint.exe
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mute in Background

Mutes the target program automatically while it is in the background, and
restores its volume when it comes back to the foreground.

## Choosing the target program

The target program is set by the mod's `@include` metadata field, which
Windhawk uses to decide which processes to inject into. It is set to
`mspaint.exe` (Paint) as a placeholder, so nothing happens to your programs
until you change it.

You do not need to edit the source code to change it. Open the mod in Windhawk,
go to **Details** → **Advanced settings**, and put the executable name of your
target in the **process inclusion list** there. The change takes effect the next
time the program starts.

## Notes

- The volume is restored when the mod is unloaded or the program exits.
- Only the audio session of the target program itself is affected.
*/
// ==/WindhawkModReadme==

#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

static HWINEVENTHOOK g_hook = nullptr;
static float g_savedVolume = 1.0f;

static void SetProcessVolume(float vol) {
    ComPtr<IMMDeviceEnumerator> enumerator;
    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr,
        CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
    if (FAILED(hr)) return;

    ComPtr<IMMDevice> device;
    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    if (FAILED(hr)) return;

    ComPtr<IAudioSessionManager2> mgr;
    hr = device->Activate(__uuidof(IAudioSessionManager2),
        CLSCTX_ALL, nullptr, &mgr);
    if (FAILED(hr)) return;

    ComPtr<IAudioSessionEnumerator> sessEnum;
    hr = mgr->GetSessionEnumerator(&sessEnum);
    if (FAILED(hr)) return;

    int count = 0;
    sessEnum->GetCount(&count);

    DWORD selfPid = GetCurrentProcessId();

    for (int i = 0; i < count; i++) {
        ComPtr<IAudioSessionControl> ctrl;
        if (FAILED(sessEnum->GetSession(i, &ctrl))) continue;

        ComPtr<IAudioSessionControl2> ctrl2;
        if (FAILED(ctrl.As(&ctrl2))) continue;

        DWORD pid = 0;
        if (FAILED(ctrl2->GetProcessId(&pid))) continue;
        if (pid != selfPid) continue;

        ComPtr<ISimpleAudioVolume> volCtrl;
        if (FAILED(ctrl.As(&volCtrl))) continue;

        volCtrl->SetMasterVolume(vol, nullptr);
    }
}

static void CALLBACK WinEventProc(
    HWINEVENTHOOK, DWORD event, HWND hwnd,
    LONG idObject, LONG, DWORD, DWORD)
{
    if (idObject != OBJID_WINDOW) return;
    if (event != EVENT_SYSTEM_FOREGROUND) return;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    bool isForeground = (pid == GetCurrentProcessId());
    SetProcessVolume(isForeground ? g_savedVolume : 0.0f);
}

BOOL Wh_ModInit() {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    g_hook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        nullptr, WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT
    );

    if (!g_hook) {
        Wh_Log(L"SetWinEventHook failed: %d", GetLastError());
        return FALSE;
    }

    Wh_Log(L"mute-in-background initialized");
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hook) {
        UnhookWinEvent(g_hook);
        g_hook = nullptr;
    }
    // Make sure the volume is restored on exit
    SetProcessVolume(g_savedVolume);
    CoUninitialize();
}