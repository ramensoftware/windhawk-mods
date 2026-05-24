// ==WindhawkMod==
// @id              desktop-icons-toggle-fork
// @name            Toggle iconos del escritorio - Fork
// @description     Muestra/oculta los iconos del escritorio al instante con un boton flotante arrastrable, un atajo de teclado y opacidad ajustable.
// @version         2.1
// @author          Aaron
// @github          https://github.com/
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Toggle iconos del escritorio

Hace lo que la opcion de Windows "Ver -> Mostrar iconos del escritorio", pero
**sin** el menu contextual. Actua directamente sobre la ventana `SysListView32`
del escritorio (igual que la funcion nativa).

## Formas de alternar

- **Boton flotante** en pantalla: un clic alterna mostrar/ocultar. **Mantén
  pulsado y arrastra** para moverlo donde quieras; su posicion se recuerda.
- **Atajo de teclado global** (por defecto `Ctrl+Alt+D`).
- **Casilla** "Mostrar iconos del escritorio" en los ajustes del mod.

## Extra

- **Opacidad** de los iconos de 0 a 100 (transparencia), algo que la funcion
  nativa no permite.

## Uso

1. Compila (`Ctrl+B`) y activa el mod.
2. Veras el boton flotante. Clic para alternar, arrastra para reubicar.
3. Puedes ajustar la opacidad del botón desde los ajustes para esconderlo completamente o hacerlo visible para moverlo.

Al desactivar el mod, los iconos vuelven visibles y opacos y el boton desaparece.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showIcons: true
  $name: Mostrar iconos del escritorio
  $description: Activa o desactiva la visibilidad de los iconos del escritorio.
- opacity: 100
  $name: Opacidad de los iconos
  $description: De 0 (invisible) a 100 (totalmente opaco).
- showButton: true
  $name: Mostrar boton flotante
  $description: Boton en pantalla para alternar (clic) y arrastrable (mantener y mover).
- buttonOpacity: 60
  $name: Opacidad del boton flotante
  $description: De 1 a 100. (Controla que tan transparente es el boton base).
- buttonSize: 64
  $name: Tamaño del boton flotante
  $description: Tamaño en pixeles (ej. 32, 46, 64).
- hotkey: "Ctrl+Alt+D"
  $name: Atajo para alternar
  $description: >-
    Combinacion global. Ejemplos: Ctrl+Alt+D, Win+I, Ctrl+Shift+F8. Dejalo vacio
    para desactivar el atajo.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <cwctype>
#include <algorithm>

// ---------------------------------------------------------------------------
// Estado global
// ---------------------------------------------------------------------------
static bool   g_settingShow = true;   // valor de la casilla
static int    g_opacity     = 100;    // 0..100
static bool   g_showButton  = true;   // mostrar boton flotante
static int    g_btnOpacity  = 60;
static int    g_btnSize     = 64;
static bool   g_hidden      = false;  // estado en ejecucion
static UINT   g_mods        = 0;      // modificadores del atajo
static UINT   g_vk          = 0;      // tecla del atajo (0 = sin atajo)

static HANDLE g_thread      = nullptr;
static DWORD  g_threadId    = 0;
static HANDLE g_threadReady = nullptr;

// Boton flotante
static HWND   g_btnWnd          = nullptr;
static bool   g_classRegistered = false;
static bool   g_dragging        = false;
static bool   g_moved           = false;
static POINT  g_dragStartCur    = {0, 0};
static POINT  g_winStart        = {0, 0};
static const wchar_t* kBtnClass = L"WhDesktopIconsToggleButton";

// ---------------------------------------------------------------------------
// Localizar la ventana de iconos del escritorio (SysListView32)
// ---------------------------------------------------------------------------
static BOOL CALLBACK FindDefViewEnumProc(HWND top, LPARAM lp) {
    WCHAR cls[64];
    if (!GetClassNameW(top, cls, ARRAYSIZE(cls)))
        return TRUE;
    if (wcscmp(cls, L"WorkerW") == 0) {
        HWND dv = FindWindowExW(top, nullptr, L"SHELLDLL_DefView", nullptr);
        if (dv) {
            *reinterpret_cast<HWND*>(lp) = dv;
            return FALSE;  // encontrado, detener enumeracion
        }
    }
    return TRUE;
}

static HWND GetDesktopListView() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND defview = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);

    // En Win10/11 con fondo activo el DefView puede colgar de un WorkerW.
    if (!defview) {
        EnumWindows(FindDefViewEnumProc, reinterpret_cast<LPARAM>(&defview));
    }
    if (!defview)
        return nullptr;

    return FindWindowExW(defview, nullptr, L"SysListView32", nullptr);
}

// ---------------------------------------------------------------------------
// Aplicar el estado actual (visibilidad + opacidad) a los iconos
// ---------------------------------------------------------------------------
static void ApplyState() {
    HWND lv = GetDesktopListView();
    if (!lv) {
        Wh_Log(L"No se encontro la lista de iconos del escritorio");
        return;
    }

    if (g_hidden) {
        ShowWindow(lv, SW_HIDE);
        return;
    }

    ShowWindow(lv, SW_SHOWNA);

    LONG_PTR ex = GetWindowLongPtrW(lv, GWL_EXSTYLE);
    if (g_opacity >= 100) {
        if (ex & WS_EX_LAYERED) {
            SetWindowLongPtrW(lv, GWL_EXSTYLE, ex & ~static_cast<LONG_PTR>(WS_EX_LAYERED));
            RedrawWindow(lv, nullptr, nullptr,
                         RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
    } else {
        SetWindowLongPtrW(lv, GWL_EXSTYLE, ex | WS_EX_LAYERED);
        int a = g_opacity;
        if (a < 0) a = 0;
        BYTE alpha = static_cast<BYTE>(a * 255 / 100);
        SetLayeredWindowAttributes(lv, 0, alpha, LWA_ALPHA);
        RedrawWindow(lv, nullptr, nullptr,
                     RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
    }
}

static void ToggleNow() {
    g_hidden = !g_hidden;
    Wh_SetIntValue(L"hidden", g_hidden ? 1 : 0);
    ApplyState();
    if (g_btnWnd)
        InvalidateRect(g_btnWnd, nullptr, TRUE);
}

// ---------------------------------------------------------------------------
// Boton flotante: dibujo y comportamiento (Minimalista)
// ---------------------------------------------------------------------------
static void PaintButton(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc;
    GetClientRect(hwnd, &rc);

    // Fondo magenta transparente
    HBRUSH keyBrush = CreateSolidBrush(RGB(255, 0, 255));
    FillRect(hdc, &rc, keyBrush);
    DeleteObject(keyBrush);

    // Botón redondeado simple
    COLORREF bgColor = g_hidden ? RGB(60, 60, 60) : RGB(0, 120, 215);
    HBRUSH body = CreateSolidBrush(bgColor);
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    
    HGDIOBJ oldBody = SelectObject(hdc, body);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    
    // Dibuja la caja
    RoundRect(hdc, rc.left + 2, rc.top + 2, rc.right - 2, rc.bottom - 2, 12, 12);
    
    // Indicador rojo cuando los iconos están ocultos
    if (g_hidden) {
        int padding = g_btnSize / 4;
        HPEN redPen = CreatePen(PS_SOLID, 3, RGB(230, 60, 60));
        SelectObject(hdc, redPen);
        MoveToEx(hdc, rc.left + padding, rc.bottom - padding, nullptr);
        LineTo(hdc, rc.right - padding, rc.top + padding);
        DeleteObject(redPen);
    }

    SelectObject(hdc, oldBody);
    SelectObject(hdc, oldPen);
    DeleteObject(body);
    DeleteObject(pen);

    EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK BtnWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_LBUTTONDOWN: {
            g_dragging = true;
            g_moved = false;
            GetCursorPos(&g_dragStartCur);
            RECT rc;
            GetWindowRect(hwnd, &rc);
            g_winStart.x = rc.left;
            g_winStart.y = rc.top;
            SetCapture(hwnd);
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (g_dragging) {
                POINT p;
                GetCursorPos(&p);
                int dx = p.x - g_dragStartCur.x;
                int dy = p.y - g_dragStartCur.y;
                if (dx > 3 || dx < -3 || dy > 3 || dy < -3)
                    g_moved = true;
                    
                int newX = g_winStart.x + dx;
                int newY = g_winStart.y + dy;
                
                int sx = GetSystemMetrics(SM_CXSCREEN);
                int sy = GetSystemMetrics(SM_CYSCREEN);
                if (newX < 0) newX = 0;
                if (newY < 0) newY = 0;
                if (newX > sx - g_btnSize) newX = sx - g_btnSize;
                if (newY > sy - g_btnSize) newY = sy - g_btnSize;

                SetWindowPos(hwnd, nullptr, newX, newY,
                             0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (g_dragging) {
                g_dragging = false;
                ReleaseCapture();
                
                if (!g_moved) {
                    ToggleNow();  // fue un clic: alternar
                } else {
                    RECT rc;
                    GetWindowRect(hwnd, &rc);
                    Wh_SetIntValue(L"btnX", rc.left);
                    Wh_SetIntValue(L"btnY", rc.top);
                }
            }
            return 0;
        }
        case WM_PAINT:
            PaintButton(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void CreateButtonWindow() {
    if (!g_showButton || g_btnWnd)
        return;

    HINSTANCE hInst = GetModuleHandleW(nullptr);

    if (!g_classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize        = sizeof(wc);
        wc.lpfnWndProc   = BtnWndProc;
        wc.hInstance     = hInst;
        wc.hCursor       = LoadCursorW(nullptr, IDC_SIZEALL);
        wc.lpszClassName = kBtnClass;
        RegisterClassExW(&wc);  // si ya existe, se ignora
        g_classRegistered = true;
    }

    int sx = GetSystemMetrics(SM_CXSCREEN);
    int defX = sx - g_btnSize - 40;
    int defY = 120;
    int x = Wh_GetIntValue(L"btnX", defX);
    int y = Wh_GetIntValue(L"btnY", defY);

    int sy = GetSystemMetrics(SM_CYSCREEN);
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > sx - g_btnSize) x = sx - g_btnSize;
    if (y > sy - g_btnSize) y = sy - g_btnSize;

    g_btnWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE,
        kBtnClass, L"", WS_POPUP,
        x, y, g_btnSize, g_btnSize,
        nullptr, nullptr, hInst, nullptr);

    if (g_btnWnd) {
        BYTE alpha = static_cast<BYTE>(g_btnOpacity * 255 / 100);
        SetLayeredWindowAttributes(g_btnWnd, RGB(255, 0, 255), alpha,
                                   LWA_COLORKEY | LWA_ALPHA);
        ShowWindow(g_btnWnd, SW_SHOWNA);
        UpdateWindow(g_btnWnd);
    }
}

static void DestroyButtonWindow() {
    if (g_btnWnd) {
        DestroyWindow(g_btnWnd);
        g_btnWnd = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Parseo del atajo de teclado
// ---------------------------------------------------------------------------
static UINT KeyNameToVk(const std::wstring& t) {
    if (t.size() == 1) {
        wchar_t c = t[0];
        if (c >= L'a' && c <= L'z') return static_cast<UINT>(L'A' + (c - L'a'));
        if (c >= L'0' && c <= L'9') return static_cast<UINT>(c);
    }
    if (t.size() >= 2 && t[0] == L'f') {
        int n = 0;
        bool digits = true;
        for (size_t i = 1; i < t.size(); ++i) {
            wchar_t d = t[i];
            if (d < L'0' || d > L'9') { digits = false; break; }
            n = n * 10 + (d - L'0');
        }
        if (digits && n >= 1 && n <= 24) return static_cast<UINT>(VK_F1 + (n - 1));
    }
    if (t == L"space")                 return VK_SPACE;
    if (t == L"insert" || t == L"ins") return VK_INSERT;
    if (t == L"delete" || t == L"del") return VK_DELETE;
    if (t == L"home")                  return VK_HOME;
    if (t == L"end")                   return VK_END;
    if (t == L"pgup" || t == L"prior") return VK_PRIOR;
    if (t == L"pgdn" || t == L"next")  return VK_NEXT;
    return 0;
}

static bool ParseHotkey(PCWSTR s, UINT* mods, UINT* vk) {
    *mods = MOD_NOREPEAT;
    *vk = 0;
    if (!s) return false;

    std::wstring str = s;
    size_t start = 0;
    while (start <= str.size()) {
        size_t plus = str.find(L'+', start);
        std::wstring tok = (plus == std::wstring::npos)
                                ? str.substr(start)
                                : str.substr(start, plus - start);

        size_t a = tok.find_first_not_of(L" \t");
        if (a == std::wstring::npos) {
            tok.clear();
        } else {
            size_t b = tok.find_last_not_of(L" \t");
            tok = tok.substr(a, b - a + 1);
        }
        for (auto& c : tok) c = towlower(c);

        if (!tok.empty()) {
            if (tok == L"ctrl" || tok == L"control")               *mods |= MOD_CONTROL;
            else if (tok == L"alt")                                *mods |= MOD_ALT;
            else if (tok == L"shift")                              *mods |= MOD_SHIFT;
            else if (tok == L"win" || tok == L"windows" || tok == L"meta") *mods |= MOD_WIN;
            else {
                UINT k = KeyNameToVk(tok);
                if (k) *vk = k;
            }
        }

        if (plus == std::wstring::npos) break;
        start = plus + 1;
    }

    return *vk != 0;
}

// ---------------------------------------------------------------------------
// Hilo de interfaz: atajo de teclado + bombeo de mensajes del boton
// ---------------------------------------------------------------------------
static DWORD WINAPI UiThreadProc(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    BOOL registered = FALSE;
    if (g_vk)
        registered = RegisterHotKey(nullptr, 1, g_mods, g_vk);
    if (g_vk && !registered)
        Wh_Log(L"No se pudo registrar el atajo (puede estar en uso)");

    CreateButtonWindow();

    if (g_threadReady)
        SetEvent(g_threadReady);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY) {
            ToggleNow();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    DestroyButtonWindow();
    if (registered)
        UnregisterHotKey(nullptr, 1);
    return 0;
}

static void StartUiThread() {
    if (!g_vk && !g_showButton)
        return;  // nada que hacer
    g_threadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_threadId);
    if (g_threadReady) {
        if (g_thread)
            WaitForSingleObject(g_threadReady, 3000);
        CloseHandle(g_threadReady);
        g_threadReady = nullptr;
    }
}

static void StopUiThread() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
        g_threadId = 0;
    }
}

// ---------------------------------------------------------------------------
// Ajustes
// ---------------------------------------------------------------------------
static void LoadSettings() {
    g_settingShow = Wh_GetIntSetting(L"showIcons") != 0;
    g_showButton  = Wh_GetIntSetting(L"showButton") != 0;

    g_opacity = Wh_GetIntSetting(L"opacity");
    if (g_opacity < 0)   g_opacity = 0;
    if (g_opacity > 100) g_opacity = 100;

    g_btnOpacity = Wh_GetIntSetting(L"buttonOpacity");
    if (g_btnOpacity < 1)   g_btnOpacity = 1;
    if (g_btnOpacity > 100) g_btnOpacity = 100;

    g_btnSize = Wh_GetIntSetting(L"buttonSize");
    if (g_btnSize < 16)  g_btnSize = 16;
    if (g_btnSize > 256) g_btnSize = 256;

    PCWSTR hk = Wh_GetStringSetting(L"hotkey");
    if (!ParseHotkey(hk, &g_mods, &g_vk)) {
        g_mods = 0;
        g_vk = 0;
    }
    Wh_FreeStringSetting(hk);
}

// ---------------------------------------------------------------------------
// Ciclo de vida del mod
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    int defaultHidden = g_settingShow ? 0 : 1;
    g_hidden = Wh_GetIntValue(L"hidden", defaultHidden) != 0;

    ApplyState();
    StartUiThread();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    StopUiThread();

    // Restaurar iconos visibles y opacos al desactivar el mod.
    g_hidden = false;
    g_opacity = 100;
    ApplyState();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    StopUiThread();
    LoadSettings();

    // La casilla manda al editar ajustes.
    g_hidden = !g_settingShow;
    Wh_SetIntValue(L"hidden", g_hidden ? 1 : 0);

    ApplyState();
    StartUiThread();
}