// ==WindhawkMod==
// @id              taskbar-idle-clean-desktop
// @name            Clean Desktop on Idle
// @description     Hides the taskbar and minimizes all windows after a period of user inactivity. Includes smart detection.
// @version         1.9
// @author          Breiq
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32
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
        PostMessageW(tray, WM_COMMAND, 419, 0);
    }
}

// PASO 1 y 2: En "Mods as tools", el punto de entrada es Wh_Main
// No se usa Wh_ModInit ni hilos separados (CreateThread), porque el proceso ya es tuyo.
void Wh_Main() {
    int minutosConfig = Wh_GetIntSetting(L"minutesIdle");
    bool oculto = false;

    // Reset inicial
    SetTaskbarHidden(false);

    // Bucle principal del proceso dedicado
    while (TRUE) {
        // Actualizar configuración en cada ciclo o podrías usar una notificación
        minutosConfig = Wh_GetIntSetting(L"minutesIdle");

        LASTINPUTINFO lii;
        lii.cbSize = sizeof(LASTINPUTINFO);

        if (GetLastInputInfo(&lii)) {
            ULONGLONG idleMs = GetTickCount64() - lii.dwTime;
            ULONGLONG limitMs = (ULONGLONG)minutosConfig * 60 * 1000;

            if (idleMs >= limitMs && !oculto) {
                if (!IsUserBusy()) {
                    SetTaskbarHidden(true);
                    MinimizeAll();
                    oculto = true;
                }
            } 
            else if (idleMs < limitMs && oculto) {
                SetTaskbarHidden(false);
                oculto = false;
            }
        }
        Sleep(1000);
    }
}

// Al ser un proceso dedicado, ya no necesitamos Wh_ModUninit para limpiar el hilo, 
// pero Windhawk matará el proceso cuando se desactive el mod.
