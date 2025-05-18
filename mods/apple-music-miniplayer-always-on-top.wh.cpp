// ==WindhawkMod==
// @id              apple-music-miniplayer-always-on-top
// @name            Apple Music Miniplayer Always On Top
// @description     Keeps the Apple Music MiniPlayer on top
// @version         1.0
// @author          tinodin
// @github          https://github.com/tinodin
// @include         AppleMusic.exe
// ==/WindhawkMod==

#include <Windows.h>
#include <string>

volatile bool g_Running = false;
HWINEVENTHOOK g_hWinEventHook = nullptr;

bool IsMiniplayerWindow(HWND hwnd) {
    wchar_t title[256] = {0};
    GetWindowTextW(hwnd, title, 256);
    std::wstring wtitle(title);
    for (auto& c : wtitle) c = towlower(c);
    return wtitle.find(L"miniplayer") != std::wstring::npos;
}

void SetMiniplayerAlwaysOnTop(HWND hwnd) {
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    Wh_Log(L"[MiniplayerMod] SetMiniplayerAlwaysOnTop called for HWND=%p", hwnd);
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (!g_Running || !hwnd)
        return;
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF)
        return;

    if (event == EVENT_OBJECT_CREATE || event == EVENT_OBJECT_SHOW) {
        if (IsMiniplayerWindow(hwnd)) {
            Wh_Log(L"[MiniplayerMod] MiniPlayer window detected: HWND=%p", hwnd);
            SetMiniplayerAlwaysOnTop(hwnd);
        }
    }
}

BOOL SetupWinEventHook(DWORD pid) {
    g_hWinEventHook = SetWinEventHook(
        EVENT_OBJECT_CREATE,
        EVENT_OBJECT_SHOW,
        nullptr,
        WinEventProc,
        pid,
        0,
        WINEVENT_OUTOFCONTEXT
    );

    return g_hWinEventHook != nullptr;
}

BOOL Wh_ModInit() {
    g_Running = true;
    DWORD pid = GetCurrentProcessId();

    if (!SetupWinEventHook(pid)) {
        Wh_Log(L"[MiniplayerMod] Failed to install WinEvent hook");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    g_Running = false;
    if (g_hWinEventHook) {
        UnhookWinEvent(g_hWinEventHook);
        g_hWinEventHook = nullptr;
    }
}