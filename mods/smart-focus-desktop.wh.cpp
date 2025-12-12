// ==WindhawkMod==
// @id              smart-focus-desktop
// @name            Smart Focus Desktop
// @description     Auto-hides desktop icons when windows are active. Click on the empty wallpaper to toggle "Show Desktop" (Win+D) and reveal icons.
// @version         1.0.0
// @author          yetzu

// @compilerOptions -lcomctl32 -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// --- INCLUDES ---
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <exdisp.h>
#include <shldisp.h>
#include <atomic>
#include <stdio.h>

// --- CONFIGURACION DE TIEMPOS (UX) ---
// Modifica estos valores si sientes lag o rebotes
#define CLICK_POLL_MS      10    // 10ms: Detección de clic instantánea.
#define FOCUS_POLL_MS      50    // 50ms: Reacción visual rápida (antes 150ms).
#define GRACE_PERIOD_MS    600   // 600ms: Tiempo justo para la animación de Win+D (antes 1500ms).

// --- ESTADO GLOBAL ---
struct {
    HWND hDefView = NULL;
    HWND hListView = NULL;
    HANDLE hThread = NULL;
    std::atomic<bool> bRunning = false;
    bool bIconsHidden = false;
    
    // Control de Tiempos
    ULONGLONG nextFocusCheckTime = 0;
    ULONGLONG suspendAutoHideUntil = 0; 
} g_ctx;

// --- LOGGING ---
void LogDebug(const wchar_t* fmt, ...) {
    wchar_t buffer[1024];
    va_list args;
    va_start(args, fmt);
    vswprintf(buffer, 1024, fmt, args);
    va_end(args);
    Wh_Log(L"%s", buffer);
}

// --- HELPERS ---

void ToggleDesktop() {
    IShellDispatch4* pShellDispatch = NULL;
    HRESULT hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch4, (void**)&pShellDispatch);
    
    if (SUCCEEDED(hr) && pShellDispatch) {
        LogDebug(L"[ACCION] ToggleDesktop (COM) ejecutado.");
        pShellDispatch->ToggleDesktop();
        pShellDispatch->Release();
    } else {
        LogDebug(L"[WARN] Fallback a MinimizeAll.");
        IShellDispatch* pShellDispatchBase = NULL;
        hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void**)&pShellDispatchBase);
        if (SUCCEEDED(hr) && pShellDispatchBase) {
             pShellDispatchBase->MinimizeAll();
             pShellDispatchBase->Release();
        }
    }
}

void SetIconsVisible(bool show) {
    if (!g_ctx.hListView || !IsWindow(g_ctx.hListView)) return;

    if (show && g_ctx.bIconsHidden) {
        ShowWindow(g_ctx.hListView, SW_SHOW);
        InvalidateRect(g_ctx.hListView, NULL, TRUE);
        g_ctx.bIconsHidden = false;
        // LogDebug(L"[VISUAL] Iconos MOSTRADOS");
    } 
    else if (!show && !g_ctx.bIconsHidden) {
        ShowWindow(g_ctx.hListView, SW_HIDE);
        g_ctx.bIconsHidden = true;
        // LogDebug(L"[VISUAL] Iconos OCULTOS");
    }
}

bool RefreshDesktopHandles() {
    HWND hProgman = FindWindow(L"Progman", NULL);
    HWND hDefView = FindWindowEx(hProgman, NULL, L"SHELLDLL_DefView", NULL);

    if (!hDefView) {
        HWND hWorkerW = NULL;
        while ((hWorkerW = FindWindowEx(NULL, hWorkerW, L"WorkerW", NULL)) != NULL) {
            hDefView = FindWindowEx(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hDefView) break;
        }
    }

    if (hDefView) {
        g_ctx.hDefView = hDefView;
        g_ctx.hListView = FindWindowEx(hDefView, NULL, L"SysListView32", NULL);
    }
    return (g_ctx.hDefView && g_ctx.hListView);
}

// Determina si una ventana es "Normal" (Debe ocultar iconos) o "Sistema" (Debe mostrar iconos)
bool IsNormalWindow(HWND hwnd) {
    if (!hwnd || !IsWindowVisible(hwnd)) return false;
    if (hwnd == g_ctx.hListView || hwnd == g_ctx.hDefView) return false;
    
    WCHAR className[256];
    if (GetClassName(hwnd, className, 256) == 0) return false;

    // Lista Blanca: Si el foco está aquí, los iconos deben verse.
    if (wcscmp(className, L"Progman") == 0 || 
        wcscmp(className, L"WorkerW") == 0 || 
        wcscmp(className, L"Shell_TrayWnd") == 0 || 
        wcscmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
        wcscmp(className, L"SysListView32") == 0) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    if (exStyle & WS_EX_NOACTIVATE) return false;

    return true;
}

// --- HILO PRINCIPAL ---

DWORD WINAPI MonitorThreadProc(LPVOID lpParam) {
    CoInitialize(NULL);
    LogDebug(L"[THREAD] Iniciado. Poll Input: %dms | Poll Foco: %dms", CLICK_POLL_MS, FOCUS_POLL_MS);

    bool bMouseWasDown = false;

    while (g_ctx.bRunning) {
        // 1. Recuperación de errores (Handles)
        if (!IsWindow(g_ctx.hListView)) {
            if (!RefreshDesktopHandles()) {
                Sleep(500); continue;
            }
            LogDebug(L"[SYSTEM] Handles recuperados.");
        }

        ULONGLONG now = GetTickCount64();

        // ---------------------------------------------------------
        // A. DETECCIÓN DE CLIC (ALTA VELOCIDAD)
        // ---------------------------------------------------------
        bool bMouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        if (bMouseDown && !bMouseWasDown) {
            POINT pt;
            GetCursorPos(&pt);
            HWND hWindowUnderMouse = WindowFromPoint(pt);
            
            // Verificar si clicamos en el escritorio
            if (hWindowUnderMouse == g_ctx.hListView || hWindowUnderMouse == g_ctx.hDefView) {
                bool bIsEmpty = true;
                
                // HitTest para asegurar que no sea un icono
                if (hWindowUnderMouse == g_ctx.hListView) {
                    POINT ptClient = pt;
                    ScreenToClient(g_ctx.hListView, &ptClient);
                    LVHITTESTINFO hitInfo = {};
                    hitInfo.pt = ptClient;
                    ListView_HitTest(g_ctx.hListView, &hitInfo);
                    if (!(hitInfo.flags & LVHT_NOWHERE)) {
                        bIsEmpty = false;
                    }
                }

                if (bIsEmpty) {
                    LogDebug(L"[INPUT] Clic Válido.");
                    
                    // 1. Pausar Auto-Hide brevemente (Grace Period)
                    g_ctx.suspendAutoHideUntil = now + GRACE_PERIOD_MS;
                    
                    // 2. Feedback visual inmediato
                    SetIconsVisible(true);

                    // 3. Acción
                    ToggleDesktop();

                    // 4. Esperar a que se suelte el clic
                    while ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && g_ctx.bRunning) {
                        Sleep(10);
                    }
                }
            }
        }
        bMouseWasDown = bMouseDown;

        // ---------------------------------------------------------
        // B. GESTION DE FOCO (VISIBILIDAD)
        // ---------------------------------------------------------
        if (now > g_ctx.nextFocusCheckTime) {
            g_ctx.nextFocusCheckTime = now + FOCUS_POLL_MS;

            // Solo gestionar visibilidad si pasó el periodo de gracia
            if (now > g_ctx.suspendAutoHideUntil) {
                HWND hForeground = GetForegroundWindow();
                
                if (IsNormalWindow(hForeground)) {
                    // Hay una ventana normal -> Ocultar
                    SetIconsVisible(false);
                } else {
                    // Estamos en escritorio/barra tareas -> Mostrar
                    SetIconsVisible(true);

                    // DEBUG: Si los iconos se muestran pero hay ventanas visibles, 
                    // queremos saber qué ventana tiene el foco.
                    /* WCHAR cls[100];
                    GetClassName(hForeground, cls, 100);
                    if (!g_ctx.bIconsHidden) { // Solo logear si estamos mostrando iconos
                        // LogDebug(L"[FOCO] Manteniendo visible. Foco en: %s", cls);
                    }
                    */
                }
            } else {
                // Durante el periodo de gracia, forzamos visible para la animación
                SetIconsVisible(true);
            }
        }

        Sleep(CLICK_POLL_MS);
    }
    
    CoUninitialize();
    LogDebug(L"[THREAD] Finalizado.");
    return 0;
}

// --- WINDHAWK LIFECYCLE ---

BOOL Wh_ModInit() {
    Wh_Log(L"INIT: Mod V3.4 (Tuned UX)");
    
    if (!RefreshDesktopHandles()) {
        Wh_Log(L"[WARN] Handles pendientes...");
    }

    g_ctx.bRunning = true;
    g_ctx.hThread = CreateThread(NULL, 0, MonitorThreadProc, NULL, 0, NULL);
    
    // Prioridad Alta para asegurar respuesta al clic
    if (g_ctx.hThread) SetThreadPriority(g_ctx.hThread, THREAD_PRIORITY_ABOVE_NORMAL);

    return (g_ctx.hThread != NULL);
}

void Wh_ModUninit() {
    Wh_Log(L"UNINIT: Limpiando...");
    g_ctx.bRunning = false;
    
    if (g_ctx.hThread) {
        WaitForSingleObject(g_ctx.hThread, 2000);
        CloseHandle(g_ctx.hThread);
    }

    if (g_ctx.hListView && IsWindow(g_ctx.hListView)) {
        ShowWindow(g_ctx.hListView, SW_SHOW);
    }
}
