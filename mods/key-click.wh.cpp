// ==WindhawkMod==
// @id              key-click
// @name            Key Click
// @description     Produces click sound on keypress, supports autorepeat
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @compilerOptions -lwinmm
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*Produces click sound on keypress, supports autorepeat*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <mmsystem.h>

HWND g_hwnd = NULL;
HANDLE g_thread = NULL;
DWORD g_threadId = 0;
HINSTANCE g_hInst = NULL;

static const wchar_t* CLASS_NAME = L"KeyClickWndClass";
static const unsigned char clickWav[] = {
    // RIFF Header
    'R','I','F','F', 38,0,0,0, 'W','A','V','E',
    // Sub-chunk 1 (fmt)
    'f','m','t',' ', 16,0,0,0, 1,0, 1,0,
    0x40,0x1F,0,0, // Sample Rate (8000 Hz)
    0x40,0x1F,0,0, // Byte Rate
    1,0, 8,0,      // Block Align, Bits per sample (8)
    // Sub-chunk 2 (data)
    'd','a','t','a', 8,0,0,0, 
    // Сами байты звука:
    128, 255, 0, 255, 0, 200, 80, 128
};

void PlayClick() {
    PlaySoundA((LPCSTR)clickWav, NULL,
        SND_MEMORY | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_INPUT) {
        UINT size = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

        BYTE buffer[64];
        if (size <= sizeof(buffer)) {
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER)) == size) {
                RAWINPUT* ri = (RAWINPUT*)buffer;

                if (ri->header.dwType == RIM_TYPEKEYBOARD) {
                    if (!(ri->data.keyboard.Flags & RI_KEY_BREAK)) {
                        PlayClick();
                    }
                }
            }
        }
        return 0;
    }

    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI MsgThread(LPVOID) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInst;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    g_hwnd = CreateWindowEx(
        0, CLASS_NAME, L"",
        0, 0,0,0,0,
        HWND_MESSAGE, NULL, g_hInst, NULL
    );

    // регистрация Raw Input
    RAWINPUTDEVICE rid = {};
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x06;
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = g_hwnd;

    RegisterRawInputDevices(&rid, 1, sizeof(rid));

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}

BOOL Wh_ModInit() {
    g_hInst = GetModuleHandle(NULL);

    g_thread = CreateThread(NULL, 0, MsgThread, NULL, 0, &g_threadId);
    return g_thread != NULL;
}

void Wh_ModUninit() {
    if (g_hwnd) {
        // отписка от Raw Input
        RAWINPUTDEVICE rid = {};
        rid.usUsagePage = 0x01;
        rid.usUsage = 0x06;
        rid.dwFlags = RIDEV_REMOVE;
        rid.hwndTarget = NULL;
        RegisterRawInputDevices(&rid, 1, sizeof(rid));

        DestroyWindow(g_hwnd);
        g_hwnd = NULL;
    }

    if (g_threadId) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    }

    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = NULL;
    }

    UnregisterClass(CLASS_NAME, g_hInst);
}
