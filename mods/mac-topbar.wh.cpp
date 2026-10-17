// ==WindhawkMod==
// @id              mac-topbar
// @name            MacTopBar (prototype)
// @description     Forces the Windows 11 taskbar to the top while this mod is enabled.
// @version         1.0
// @author          C0mputer M4n
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32
// ==/WindhawkMod==

#include <windows.h>

// Windows 11's native top-taskbar support stores the taskbar edge here.
// ABE_TOP = 1, ABE_BOTTOM = 3.
static constexpr wchar_t kExplorerAdvancedKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
static constexpr wchar_t kTaskbarLocationValue[] = L"TaskbarLocation";
static constexpr DWORD kAbeTop = 1;
static constexpr DWORD kAbeBottom = 3;

// Windows 11 shell-private message used for taskbar setting changes.
// This is undocumented and may change in future Windows builds.
static constexpr UINT kTaskbarSettingChangedMessage = 0x5CA;
static constexpr WPARAM kTaskbarLocationSettingId = 6;

static DWORD g_previousLocation = kAbeBottom;
static bool g_hadPreviousValue = false;
static bool g_valueWasChanged = false;

static void NotifyShellOfLocation(DWORD edge) {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        DWORD_PTR ignored = 0;
        SendMessageTimeoutW(taskbar,
                            kTaskbarSettingChangedMessage,
                            kTaskbarLocationSettingId,
                            static_cast<LPARAM>(edge),
                            SMTO_ABORTIFHUNG,
                            1500,
                            &ignored);
        SendMessageTimeoutW(taskbar,
                            WM_SETTINGCHANGE,
                            0,
                            reinterpret_cast<LPARAM>(L"TraySettings"),
                            SMTO_ABORTIFHUNG,
                            1500,
                            &ignored);
    }

    // Notify shell components that may re-read the taskbar settings.
    DWORD_PTR ignored = 0;
    SendMessageTimeoutW(HWND_BROADCAST,
                        WM_SETTINGCHANGE,
                        0,
                        reinterpret_cast<LPARAM>(L"TraySettings"),
                        SMTO_ABORTIFHUNG,
                        1500,
                        &ignored);
}

static bool WriteTaskbarLocation(DWORD edge) {
    HKEY key = nullptr;
    LONG status = RegCreateKeyExW(HKEY_CURRENT_USER,
                                  kExplorerAdvancedKey,
                                  0,
                                  nullptr,
                                  0,
                                  KEY_QUERY_VALUE | KEY_SET_VALUE,
                                  nullptr,
                                  &key,
                                  nullptr);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"MacTopBar: couldn't open Explorer\\Advanced (error %ld)", status);
        return false;
    }

    status = RegSetValueExW(key,
                            kTaskbarLocationValue,
                            0,
                            REG_DWORD,
                            reinterpret_cast<const BYTE*>(&edge),
                            sizeof(edge));
    RegCloseKey(key);

    if (status != ERROR_SUCCESS) {
        Wh_Log(L"MacTopBar: couldn't set TaskbarLocation (error %ld)", status);
        return false;
    }

    return true;
}

BOOL Wh_ModInit() {
    HKEY key = nullptr;
    LONG status = RegOpenKeyExW(HKEY_CURRENT_USER,
                                kExplorerAdvancedKey,
                                0,
                                KEY_QUERY_VALUE,
                                &key);
    if (status == ERROR_SUCCESS) {
        DWORD type = 0;
        DWORD bytes = sizeof(g_previousLocation);
        status = RegQueryValueExW(key,
                                  kTaskbarLocationValue,
                                  nullptr,
                                  &type,
                                  reinterpret_cast<BYTE*>(&g_previousLocation),
                                  &bytes);
        if (status == ERROR_SUCCESS && type == REG_DWORD && bytes == sizeof(DWORD)) {
            g_hadPreviousValue = true;
        }
        RegCloseKey(key);
    }

    if (!g_hadPreviousValue) {
        // A missing value uses the normal Windows default (bottom).
        g_previousLocation = kAbeBottom;
    }

    if (!WriteTaskbarLocation(kAbeTop)) {
        return FALSE;
    }

    g_valueWasChanged = true;
    NotifyShellOfLocation(kAbeTop);
    Wh_Log(L"MacTopBar: requested taskbar location = Top");
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_valueWasChanged) {
        return;
    }

    if (g_hadPreviousValue) {
        if (WriteTaskbarLocation(g_previousLocation)) {
            NotifyShellOfLocation(g_previousLocation);
        }
    } else {
        HKEY key = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          kExplorerAdvancedKey,
                          0,
                          KEY_SET_VALUE,
                          &key) == ERROR_SUCCESS) {
            RegDeleteValueW(key, kTaskbarLocationValue);
            RegCloseKey(key);
            NotifyShellOfLocation(kAbeBottom);
        }
    }

    Wh_Log(L"MacTopBar: restored the previous taskbar location preference");
}
