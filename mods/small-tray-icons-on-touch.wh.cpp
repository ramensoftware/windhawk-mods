// ==WindhawkMod==
// @id              small-tray-icons-on-touch
// @name            Small Tray Icons on Touch
// @description     Reduces the size of tray icons on touch screens
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Small Tray Icons on Touch
The Windows 10 taskbar has increased tray icon size on touch screens.
This mod disables that functionality.

# IMPORTANT: READ!
Windhawk needs to hook into `winlogon.exe` to successfully capture Explorer starting. Please
navigate to Windhawk's Settings, Advanced settings, More advanced settings, and make sure that
`winlogon.exe` is in the Process inclusion list.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/small-tray-icons-on-touch-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/small-tray-icons-on-touch-after.png)
*/
// ==/WindhawkModReadme==

BOOL (WINAPI *GetPointerDevices_orig)(UINT32 *, POINTER_DEVICE_INFO *);
BOOL WINAPI GetPointerDevices_hook(UINT32 *deviceCount, POINTER_DEVICE_INFO *pointerDevices)
{
    if (!deviceCount)
        return FALSE;

    UINT32 realDeviceCount;
    if (!GetPointerDevices_orig(&realDeviceCount, nullptr))
        return FALSE;

    if (!realDeviceCount)
    {
        *deviceCount = 0;
        return TRUE;
    }

    POINTER_DEVICE_INFO *devices = new POINTER_DEVICE_INFO[realDeviceCount];
    UINT32 reportedDeviceCount = 0;
    if (!GetPointerDevices_orig(&realDeviceCount, devices))
        return FALSE;

    for (UINT32 i = 0; i < realDeviceCount; i++)
    {
        if (devices[i].pointerDeviceType != POINTER_DEVICE_TYPE_TOUCH)
        {
            if (pointerDevices)
                pointerDevices[reportedDeviceCount] = devices[i];
            reportedDeviceCount++;
        }
    }

    *deviceCount = reportedDeviceCount;
    return TRUE;
}

BOOL Wh_ModInit(void)
{
    Wh_SetFunctionHook(
        (void *)GetPointerDevices,
        (void *)GetPointerDevices_hook,
        (void **)&GetPointerDevices_orig
    );
    return TRUE;
}