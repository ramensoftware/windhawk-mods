// ==WindhawkMod==
// @id              language-switcher-position
// @name            Language Switcher Position
// @description     Customize the Windows 11 26H2 language switcher position with alignment and X/Y coordinates.
// @version         1.2
// @author          MaestroSky
// @include         explorer.exe
// @github          https://github.com/MaestroSky
// @homepage        https://github.com/MaestroSky/Language-Switcher-Position
// @license         MIT
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Language Switcher Position

## 🇺🇦 Українська

Мод дозволяє змінювати положення сучасного перемикача мов введення Windows 11 26H2, який з'являється після натискання на індикатор мови (РУС / ENG) у системному треї.

### Можливості

- **Left** — розміщення перемикача біля лівого краю екрана.
- **Center** — розміщення по центру екрана.
- **Right** — розміщення біля правого краю екрана.
- **Custom X/Y** — ручне налаштування координат вікна.
- **Vertical offset** — додаткове вертикальне зміщення.

### Приклади роботи

![Left position](https://raw.githubusercontent.com/MaestroSky/Language-Switcher-Position/main/images/left.png)

![Center position](https://raw.githubusercontent.com/MaestroSky/Language-Switcher-Position/main/images/center.png)

![Right position](https://raw.githubusercontent.com/MaestroSky/Language-Switcher-Position/main/images/right.png)

![Default position](https://raw.githubusercontent.com/MaestroSky/Language-Switcher-Position/main/images/default.png)

### Налаштування

**Horizontal position** — вибір горизонтального розташування перемикача: Left, Center, Right або Custom X/Y.

**Custom X coordinate** — горизонтальна координата вікна в пікселях.

**Custom Y coordinate** — вертикальна координата вікна в пікселях.

**Vertical offset** — додаткове вертикальне зміщення відносно початкової позиції Windows. Використовується в режимах Left, Center та Right.

### Сумісність

- Windows 11 26H2
- Windows Explorer (explorer.exe)
- Сучасний XAML-перемикач мов введення.

### Примітка

Позиціонування виконується відносно повного розміру монітора, включно з областю панелі завдань. Користувацькі координати задаються в екранних пікселях.

Мод змінює лише положення вікна перемикача та не впливає на системні налаштування мов введення.

---

## 🇬🇧 English

This mod allows you to change the position of the modern Windows 11 input language switcher, which appears when clicking the language indicator (РУС / ENG) in the system tray.

### Features

- **Left** — positions the switcher at the left edge of the screen.
- **Center** — positions the switcher in the center of the screen.
- **Right** — positions the switcher at the right edge of the screen.
- **Custom X/Y** — manually sets the window coordinates.
- **Vertical offset** — provides additional vertical positioning control.

### Screenshots

![Left position](https://raw.githubusercontent.com/MaestroSky/Language-Switcher-Position/main/images/left.png)

![Center position](https://raw.githubusercontent.com/MaestroSky/Language-Switcher-Position/main/images/center.png)

![Right position](https://raw.githubusercontent.com/MaestroSky/Language-Switcher-Position/main/images/right.png)

![Default position](https://raw.githubusercontent.com/MaestroSky/Language-Switcher-Position/main/images/default.png)

### Settings

**Horizontal position** — selects the horizontal alignment: Left, Center, Right, or Custom X/Y.

**Custom X coordinate** — the horizontal window coordinate in pixels.

**Custom Y coordinate** — the vertical window coordinate in pixels.

**Vertical offset** — additional vertical displacement relative to the original Windows position. Used with Left, Center, and Right modes.

### Compatibility

- Windows 11 26H2
- Windows Explorer (explorer.exe)
- Modern XAML input language switcher.

### Note

Positioning is calculated relative to the full monitor dimensions, including the taskbar area. Custom coordinates are specified in screen pixels.

The mod only changes the position of the switcher window and does not modify Windows input language settings.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- alignment: right
  $name: Horizontal position
  $description: Position relative to the monitor.
  $options:
  - left: Left
  - center: Center
  - right: Right
  - custom: Custom X/Y

- customX: 0
  $name: Custom X coordinate
  $description: Horizontal screen coordinate in pixels.

- customY: 778
  $name: Custom Y coordinate
  $description: Vertical screen coordinate in pixels.

- verticalOffset: 0
  $name: Vertical offset
  $description: Additional vertical offset for standard positions.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <wchar.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

using SetWindowPos_t = BOOL (WINAPI*)(
    HWND,
    HWND,
    int,
    int,
    int,
    int,
    UINT
);

static SetWindowPos_t SetWindowPos_Original = nullptr;

BOOL WINAPI SetWindowPos_Hook(
    HWND hWnd,
    HWND hWndInsertAfter,
    int X,
    int Y,
    int cx,
    int cy,
    UINT uFlags
) {
    wchar_t className[256] = {};

    GetClassNameW(
        hWnd,
        className,
        ARRAYSIZE(className)
    );

    if (wcscmp(
            className,
            L"Shell_InputSwitchTopLevelWindow"
        ) == 0 &&
        cx == 0 &&
        cy == 0) {

        HMODULE callerModule = nullptr;

        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCWSTR)__builtin_return_address(0),
            &callerModule
        );

        wchar_t moduleName[MAX_PATH] = {};

        if (callerModule) {
            GetModuleFileNameW(
                callerModule,
                moduleName,
                ARRAYSIZE(moduleName)
            );
        }

        if (wcsstr(moduleName, L"InputSwitch.dll")) {

            RECT windowRect = {};

            if (GetWindowRect(hWnd, &windowRect)) {

                int windowWidth =
                    windowRect.right - windowRect.left;

                POINT anchor = { X, Y };

                HMONITOR monitor = MonitorFromPoint(
                    anchor,
                    MONITOR_DEFAULTTONEAREST
                );

                MONITORINFO monitorInfo = {};
                monitorInfo.cbSize = sizeof(monitorInfo);

                if (monitor &&
                    GetMonitorInfoW(monitor, &monitorInfo)) {

                    RECT screen = monitorInfo.rcMonitor;

                    PCWSTR alignment =
                        Wh_GetStringSetting(L"alignment");

                    if (alignment) {

                        if (wcscmp(alignment, L"left") == 0) {
                            X = screen.left;
                        }
                        else if (wcscmp(alignment, L"center") == 0) {
                            X = screen.left +
                                ((screen.right - screen.left) -
                                windowWidth) / 2;
                        }
                        else if (wcscmp(alignment, L"right") == 0) {
                            X = screen.right - windowWidth;
                        }
                        else if (wcscmp(alignment, L"custom") == 0) {
                            X = Wh_GetIntSetting(L"customX");
                            Y = Wh_GetIntSetting(L"customY");
                        }

                        if (wcscmp(alignment, L"custom") != 0) {
                            Y += Wh_GetIntSetting(
                                L"verticalOffset"
                            );
                        }

                        Wh_FreeStringSetting(alignment);
                    }

                    Wh_Log(
                        L"[LanguageSwitcher] X=%d Y=%d Width=%d",
                        X,
                        Y,
                        windowWidth
                    );
                }
            }
        }
    }

    return SetWindowPos_Original(
        hWnd,
        hWndInsertAfter,
        X,
        Y,
        cx,
        cy,
        uFlags
    );
}

BOOL Wh_ModInit() {

    HMODULE user32 = GetModuleHandleW(L"user32.dll");

    if (!user32) {
        Wh_Log(L"Cannot find user32.dll");
        return FALSE;
    }

    auto function = reinterpret_cast<SetWindowPos_t>(
        GetProcAddress(user32, "SetWindowPos")
    );

    if (!function) {
        Wh_Log(L"Cannot find SetWindowPos");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            (void*)function,
            (void*)SetWindowPos_Hook,
            (void**)&SetWindowPos_Original
        )) {

        Wh_Log(L"Failed to install SetWindowPos hook");
        return FALSE;
    }

    Wh_Log(L"Language Switcher Position initialized");

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Language Switcher Position unloaded");
}
