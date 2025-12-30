// ==WindhawkMod==
// @id              taskbar-idle-clean-desktop
// @name            Clean Desktop on Idle
// @description     Hides the taskbar and minimizes all windows after a period of user inactivity. Includes smart detection.
// @version         1.7
// @author          Breiq
// @include         explorer.exe
// @architecture    x86-64
// @github          https://github.com/BREIQ
// @donateUrl       https://www.paypal.com/paypalme/breiq
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Clean Desktop on Idle
Automatically hides the taskbar and minimizes windows when idle.
*Does not interrupt movies or games.*
[Donate via PayPal](https://www.paypal.com/paypalme/breiq)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- minutesIdle: 5
  $name: Minutes of inactivity
  $description: Time in minutes without activity before cleaning
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <atomic>

std::atomic<bool> g_running{false};
std::atomic<bool> g_oculto{false};
std::atomic<int> g_minutosConfig{5};
HANDLE g_thread = nullptr;

// Función de detección de pantalla completa/juegos
bool IsUserBusy() {
    QUERY_USER_NOTIFICATION_STATE state;
    if (SHQueryUserNotificationState(&state) == S_OK) {
        if (state == QUNS_BUSY || 
            state == QUNS_RUNNING_D3D_FULL_SCREEN || 
            state == QUNS_PRESENTATION_MODE) {
            return true;
        }
    }
    return false;
}

void SetTaskbarHidden(bool hide) {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!abd.hWnd) return;

    abd.lParam = hide ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
    SHAppBarMessage(ABM_SETSTATE, &abd);
}

void MinimizeAll() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        // Comando para minimizar todas las ventanas
        PostMessageW(tray, WM_COMMAND, 419, 0);
    }
}

DWORD WINAPI IdleThread(LPVOID) {
    while (g_running) {
        LASTINPUTINFO lii;
        lii.cbSize = sizeof(LASTINPUTINFO);

        if (GetLastInputInfo(&lii)) {
            ULONGLONG idleMs = GetTickCount64() - lii.dwTime;
            ULONGLONG limitMs = (ULONGLONG)g_minutosConfig * 60 * 1000;

            if (idleMs >= limitMs && !g_oculto) {
                if (!IsUserBusy()) {
                    SetTaskbarHidden(true);
                    MinimizeAll();
                    g_oculto = true;
                    Wh_Log(L"Inactividad detectada: Limpiando escritorio.");
                }
            } 
            else if (idleMs < limitMs && g_oculto) {
                SetTaskbarHidden(false);
                g_oculto = false;
                Wh_Log(L"Actividad detectada: Restaurando barra.");
            }
        }
        Sleep(1000); 
    }
    return 0;
}

BOOL Wh_ModInit() {
    g_minutosConfig = Wh_GetIntSetting(L"minutesIdle");

    // RESET DE ARRANQUE: Forzamos la barra visible al iniciar el mod
    SetTaskbarHidden(false);
    g_oculto = false;

    g_running = true;
    g_thread = CreateThread(nullptr, 0, IdleThread, nullptr, 0, nullptr);
    
    Wh_Log(L"Mod iniciado correctamente y barra reseteada.");
    return g_thread != nullptr;
}

void Wh_ModSettingsChanged() {
    g_minutosConfig = Wh_GetIntSetting(L"minutesIdle");
    Wh_Log(L"Configuración actualizada.");
}

void Wh_ModUninit() {
    g_running = false;
    if (g_thread) {
        WaitForSingleObject(g_thread, 1000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    // Restauramos la barra al desactivar el mod
    SetTaskbarHidden(false);
    Wh_Log(L"Mod detenido.");
}
