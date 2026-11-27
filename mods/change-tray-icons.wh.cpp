// ==WindhawkMod==
// @id              change-tray-icons
// @name            Change Tray Icons
// @description     Change all tray icons for an application to a specific other icon
// @version         1.0.1
// @author          aubymori
// @github          https://github.com/aubymori
// @compilerOptions -lshell32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Change Tray Icons - Sharp

Changes notification-area icons created by selected applications.

Compared with the original mod, replacement icons are loaded at twice the
small-icon dimensions. This avoids supplying the Windows notification area
with an icon that has already been rasterized directly to 16x16 pixels.

## Configuration

For every application, specify:

- **Application:** The executable name, such as `steam.exe`, or its full path.
- **Icon:** The full path to a valid `.ico` file.

For best results, use a multi-resolution ICO containing at least:

- 16x16
- 20x20
- 24x24
- 32x32
- 40x40
- 48x48
- 64x64
- 256x256

Restart the affected application after enabling the mod or changing its icon.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- icons:
  - - path: steam.exe
      $name: Application
      $description: Executable name, such as steam.exe, or its complete path
    - icon: C:\Icons\steam.ico
      $name: Icon
      $description: Complete path to the replacement .ico file
  $name: Applications
  $description: Applications and their replacement notification-area icons
*/
// ==/WindhawkModSettings==

#include <stddef.h>
#include <shellapi.h>
#include <windows.h>

HICON g_hIcon = nullptr;

using Shell_NotifyIconW_t = decltype(&Shell_NotifyIconW);
Shell_NotifyIconW_t Shell_NotifyIconW_orig = nullptr;

BOOL WINAPI Shell_NotifyIconW_hook(
    DWORD dwMessage,
    PNOTIFYICONDATAW lpData
)
{
    if (g_hIcon &&
        lpData &&
        lpData->cbSize >=
            offsetof(NOTIFYICONDATAW, hIcon) + sizeof(lpData->hIcon) &&
        (lpData->uFlags & NIF_ICON))
    {
        lpData->hIcon = g_hIcon;
    }

    return Shell_NotifyIconW_orig(dwMessage, lpData);
}

static void DestroyReplacementIcon()
{
    if (g_hIcon)
    {
        DestroyIcon(g_hIcon);
        g_hIcon = nullptr;
    }
}

static bool ApplicationMatches(
    const WCHAR* applicationPath,
    const WCHAR* configuredPath
)
{
    if (!applicationPath || !configuredPath || !*configuredPath)
    {
        return false;
    }

    // Match the complete executable path.
    if (_wcsicmp(applicationPath, configuredPath) == 0)
    {
        return true;
    }

    // Also allow matching only the executable name, for example steam.exe.
    const WCHAR* fileName = wcsrchr(applicationPath, L'\\');

    if (fileName)
    {
        fileName++;
    }
    else
    {
        fileName = applicationPath;
    }

    return _wcsicmp(fileName, configuredPath) == 0;
}

static UINT GetCurrentSystemDpi()
{
    using GetDpiForSystem_t = UINT(WINAPI*)();

    static GetDpiForSystem_t GetDpiForSystem_func =
        reinterpret_cast<GetDpiForSystem_t>(
            GetProcAddress(
                GetModuleHandleW(L"user32.dll"),
                "GetDpiForSystem"
            )
        );

    if (GetDpiForSystem_func)
    {
        UINT dpi = GetDpiForSystem_func();

        if (dpi)
        {
            return dpi;
        }
    }

    HDC hdc = GetDC(nullptr);

    if (!hdc)
    {
        return 96;
    }

    int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(nullptr, hdc);

    return dpi > 0 ? static_cast<UINT>(dpi) : 96;
}

static void GetSourceIconSize(int* width, int* height)
{
    UINT dpi = GetCurrentSystemDpi();

    using GetSystemMetricsForDpi_t =
        int(WINAPI*)(int nIndex, UINT dpi);

    static GetSystemMetricsForDpi_t GetSystemMetricsForDpi_func =
        reinterpret_cast<GetSystemMetricsForDpi_t>(
            GetProcAddress(
                GetModuleHandleW(L"user32.dll"),
                "GetSystemMetricsForDpi"
            )
        );

    int smallWidth;
    int smallHeight;

    if (GetSystemMetricsForDpi_func)
    {
        smallWidth = GetSystemMetricsForDpi_func(
            SM_CXSMICON,
            dpi
        );

        smallHeight = GetSystemMetricsForDpi_func(
            SM_CYSMICON,
            dpi
        );
    }
    else
    {
        smallWidth = GetSystemMetrics(SM_CXSMICON);
        smallHeight = GetSystemMetrics(SM_CYSMICON);
    }

    if (smallWidth <= 0)
    {
        smallWidth = 16;
    }

    if (smallHeight <= 0)
    {
        smallHeight = 16;
    }

    // Do not give the tray an icon already rasterized directly to 16x16.
    // At 100% scaling this loads 32x32; at 150%, typically 48x48.
    *width = smallWidth * 2;
    *height = smallHeight * 2;
}

static HICON LoadReplacementIcon(const WCHAR* iconPath)
{
    int sourceWidth;
    int sourceHeight;

    GetSourceIconSize(&sourceWidth, &sourceHeight);

    SetLastError(ERROR_SUCCESS);

    HICON icon = reinterpret_cast<HICON>(
        LoadImageW(
            nullptr,
            iconPath,
            IMAGE_ICON,
            sourceWidth,
            sourceHeight,
            LR_LOADFROMFILE | LR_DEFAULTCOLOR
        )
    );

    if (!icon)
    {
        Wh_Log(
            L"Failed to load icon \"%s\" at %dx%d, error: %lu",
            iconPath,
            sourceWidth,
            sourceHeight,
            GetLastError()
        );

        return nullptr;
    }

    Wh_Log(
        L"Loaded replacement icon \"%s\" at %dx%d",
        iconPath,
        sourceWidth,
        sourceHeight
    );

    return icon;
}

void Wh_ModSettingsChanged()
{
    DestroyReplacementIcon();

    WCHAR applicationPath[MAX_PATH] = {};

    DWORD pathLength = GetModuleFileNameW(
        nullptr,
        applicationPath,
        ARRAYSIZE(applicationPath)
    );

    if (pathLength == 0 || pathLength >= ARRAYSIZE(applicationPath))
    {
        Wh_Log(
            L"GetModuleFileNameW failed, error: %lu",
            GetLastError()
        );

        return;
    }

    Wh_Log(L"Current application: %s", applicationPath);

    for (int i = 0;; i++)
    {
        PCWSTR configuredApplication =
            Wh_GetStringSetting(L"icons[%d].path", i);

        if (!configuredApplication)
        {
            break;
        }

        // An empty path marks the end of the settings array.
        if (!*configuredApplication)
        {
            Wh_FreeStringSetting(configuredApplication);
            break;
        }

        bool matches = ApplicationMatches(
            applicationPath,
            configuredApplication
        );

        if (matches)
        {
            PCWSTR configuredIcon =
                Wh_GetStringSetting(L"icons[%d].icon", i);

            if (configuredIcon && *configuredIcon)
            {
                g_hIcon = LoadReplacementIcon(configuredIcon);
            }
            else
            {
                Wh_Log(
                    L"Application matched, but its icon path is empty"
                );
            }

            if (configuredIcon)
            {
                Wh_FreeStringSetting(configuredIcon);
            }

            Wh_FreeStringSetting(configuredApplication);

            // Only one replacement is needed for the current process.
            break;
        }

        Wh_FreeStringSetting(configuredApplication);
    }
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Initializing Change Tray Icons - Sharp");

    Wh_ModSettingsChanged();

    return Wh_SetFunctionHook(
        reinterpret_cast<void*>(Shell_NotifyIconW),
        reinterpret_cast<void*>(Shell_NotifyIconW_hook),
        reinterpret_cast<void**>(&Shell_NotifyIconW_orig)
    );
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninitializing Change Tray Icons - Sharp");

    DestroyReplacementIcon();
}
