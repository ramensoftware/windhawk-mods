// ==WindhawkMod==
// @id              cef-titlebar-enabler-universal
// @name            CEF/Spotify Titlebar Enabler
// @description     Force native frames and title bars for CEF apps
// @version         0.1
// @author          Ingan121
// @github          https://github.com/Ingan121
// @twitter         https://twitter.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         spotify.exe
// @include         cefclient.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# CEF Titlebar Enabler
* Force native frames and title bars for CEF apps, such as Spotify
* Only works on apps using native CEF top-level windows
    * Steam uses SDL for its top-level windows (except DevTools), so this mod doesn't work with Steam
* Electron apps are NOT supported! Just patch asar to override `frame: false` to true in BrowserWindow creation
* Supported CEF versions: 90, 91, 94-101, 102-106, 120, 124-132
    * Versions between the listed versions, and versions before 90, are likely not to work
    * Versions after 132 may work but are not tested
    * Only x86-64 is supported on CEF 120 and newer
    * This mod relies on hardcoded offsets, so it may not work on versions not listed here (yet)
    * Variant of this mod using copy-pasted CEF structs instead of hardcoded offsets is available at [here](https://github.com/Ingan121/files/tree/master/cte)
    * Copy required structs/definitions from your wanted CEF version (available [here](https://cef-builds.spotifycdn.com/index.html)) and paste them to the above variant to calculate the offsets
* Supported Spotify versions: 1.1.60-1.1.97, 1.2.30, 1.2.38-1.2.52
* Spotify notes:
    * 1.1.60-1.1.67: Use SpotifyNoControls mod to remove the window controls
    * 1.1.68-1.1.70: Window control hiding doesn't work yet
    * 1.1.74: Last version to support proper DWM window controls
    * Native DWM window controls are invisible since 1.1.75 as it began hooking window messages to support Windows 11 maximize button hover menu
    * 1.1.85: Last version to support proper non-DWM window controls
    * They are still visible after 1.1.85, but they don't respond to clicks
    * 1.2.30: Only x86-64 is supported from this version
    * 1.2.45: Last version to support disabling the global navbar
    * Try toggling hardware acceleration multiple times (both in window menu and preferences) if you want a proper window icon. It may work on recent-ish versions
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showframe: true
  $name: Enable native frames and title bars
- showmenu: true
  $name: Show the menu button
  $description: Disabling this also prevents opening the Spotify menu with the Alt key
- showcontrols: false
  $name: Show Spotify's custom window controls
*/
// ==/WindhawkModSettings==

#include <libloaderapi.h>
#include <windhawk_utils.h>

#define CEF_CALLBACK __stdcall

struct cte_settings {
    BOOL showframe;
    BOOL showmenu;
    BOOL showcontrols;
} cte_settings;

typedef struct cte_offset {
  int ver_major;
  int ver_minor;
  int offset;
} cte_offset_t;

cte_offset_t is_frameless_offsets[] = {
    {90, 6, 0x48}, // Spotify 1.1.60 to 1.1.62
    {91, 1, 0x48}, // Spotify 1.1.63 to 1.1.67
    {91, 3, 0x50}, // Spotify 1.1.68 to 1.1.70
    {94, 0, 0x50}, // Spotify 1.1.71
    {101, 0, 0x50}, // Spotify 1.1.88
    {102, 0, 0x54}, // Spotify 1.1.89
    {106, 0, 0x54}, // Spotify 1.1.97
    {120, 0, 0xc8}, // Spotify 1.2.30
    {124, 0, 0xd0}, // Spotify 1.2.38
    {130, 0, 0xd0} // Spotify 1.2.52
};

cte_offset_t add_child_view_offsets[] = {
    {90, 6, NULL}, // Spotify 1.1.60 to 1.1.62, not needed (use SpotifyNoControls)
    {91, 1, NULL}, // Spotify 1.1.63 to 1.1.67, not needed (use SpotifyNoControls)
    {91, 3, NULL}, // Spotify 1.1.68 to 1.1.70, not working
    {94, 0, 0xf0}, // Spotify 1.1.71
    {102, 0, 0xf0}, // Spotify 1.1.89
    {106, 0, 0xf0}, // Spotify 1.1.97
    {120, 0, 0x1e0}, // Spotify 1.2.30
    {124, 0, 0x1e8}, // Spotify 1.2.38
    {130, 0, 0x1e8} // Spotify 1.2.52
};

cte_offset_t is_frameless_offset = {0, 0, NULL};
cte_offset_t add_child_view_offset = {0, 0, NULL};

typedef int CEF_CALLBACK (*is_frameless_t)(struct _cef_window_delegate_t* self, struct _cef_window_t* window);
int CEF_CALLBACK is_frameless_hook(struct _cef_window_delegate_t* self, struct _cef_window_t* window) {
    Wh_Log(L"is_frameless_hook");
    return 0;
}

typedef _cef_window_t* (*cef_window_create_top_level_t)(void* delegate);
cef_window_create_top_level_t cef_window_create_top_level_original;
_cef_window_t* cef_window_create_top_level_hook(void* delegate) {
    Wh_Log(L"cef_window_create_top_level_hook");

    if (is_frameless_offset.offset != NULL && cte_settings.showframe == TRUE) {
        ((is_frameless_t*)((char*)delegate + is_frameless_offset.offset))[0] = is_frameless_hook;
    }
    return cef_window_create_top_level_original(delegate);
}

int cnt = -1;

typedef void CEF_CALLBACK (*add_child_view_t)(struct _cef_panel_t* self, struct _cef_view_t* view);
add_child_view_t CEF_CALLBACK add_child_view_original;
void CEF_CALLBACK add_child_view_hook(struct _cef_panel_t* self, struct _cef_view_t* view) {
    Wh_Log(L"add_child_view_hook: %d", ++cnt);
    // 0: Minimize, 1: Maximize, 2: Close, 3: Menu (removing this also prevents alt key from working)
    if (cnt < 3) {
      if (cte_settings.showcontrols == FALSE) {
        return;
      }
    } else if (cte_settings.showmenu == FALSE) {
      return;
    }
    add_child_view_original(self, view);
    return;
}

typedef _cef_panel_t* (*cef_panel_create_t)(void* delegate);
cef_panel_create_t cef_panel_create_original;
_cef_panel_t* cef_panel_create_hook(void* delegate) {
    Wh_Log(L"cef_panel_create_hook");
    _cef_panel_t* panel = cef_panel_create_original(delegate);
    if (add_child_view_offset.offset != NULL) {
        add_child_view_original = ((add_child_view_t*)((char*)panel + add_child_view_offset.offset))[0];
        ((add_child_view_t*)((char*)panel + add_child_view_offset.offset))[0] = add_child_view_hook;
    }
    return panel;
}

typedef int (*cef_version_info_t)(int entry);

void LoadSettings() {
    cte_settings.showframe = Wh_GetIntSetting(L"showframe");
    cte_settings.showmenu = Wh_GetIntSetting(L"showmenu");
    cte_settings.showcontrols = Wh_GetIntSetting(L"showcontrols");
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    // Check if this process is auxilliary process by checking if the arguments contain --type=
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    BOOL isAuxProcess = FALSE;
    for (int i = 1; i < argc; i++) {
        if (wcsstr(argv[i], L"--type=") != NULL) {
            isAuxProcess = TRUE;
            break;
        }
    }
    if (isAuxProcess) {
        Wh_Log(L"Auxilliary process detected, skipping");
        return TRUE;
    }

    HMODULE cefModule = LoadLibrary(L"libcef.dll");
    cef_window_create_top_level_t cef_window_create_top_level =
        (cef_window_create_top_level_t)GetProcAddress(cefModule,
                                                "cef_window_create_top_level");
    cef_panel_create_t cef_panel_create =
        (cef_panel_create_t)GetProcAddress(cefModule, "cef_panel_create");
    cef_version_info_t cef_version_info =
        (cef_version_info_t)GetProcAddress(cefModule, "cef_version_info");

    // Get CEF version
    int major = cef_version_info(0);
    int minor = cef_version_info(1);
    Wh_Log(L"CEF v%d.%d.%d.%d (Chromium v%d.%d.%d.%d) Loaded",
        major,
        minor,
        cef_version_info(2),
        cef_version_info(3),
        cef_version_info(4),
        cef_version_info(5),
        cef_version_info(6),
        cef_version_info(7)
    );

    // Check if the app is Spotify
    wchar_t exeName[MAX_PATH];
    GetModuleFileName(NULL, exeName, MAX_PATH);
    BOOL isSpotify = wcsstr(_wcsupr(exeName), L"SPOTIFY.EXE") != NULL;
    if (isSpotify) {
        Wh_Log(L"Spotify detected");
    }

    // Get appropriate offsets for current CEF version
    int prev_major = 0;
    int offsets_size = sizeof(is_frameless_offsets) / sizeof(cte_offset_t);
    for (int i = 0; i < offsets_size; i++) {
        if (major <= is_frameless_offsets[i].ver_major && major >= prev_major) {
            if (is_frameless_offsets[i].ver_minor == 0 || minor == is_frameless_offsets[i].ver_minor) {
                is_frameless_offset = is_frameless_offsets[i];
                break;
            }
        }
        prev_major = is_frameless_offsets[i].ver_major;
    }
    if (major >= is_frameless_offsets[offsets_size - 1].ver_major) {
        is_frameless_offset = is_frameless_offsets[offsets_size - 1];
    }
    Wh_Log(L"is_frameless offset: %#x", is_frameless_offset.offset);

    if (isSpotify) {
      prev_major = 0;
      offsets_size = sizeof(add_child_view_offsets) / sizeof(cte_offset_t);
      for (int i = 0; i < offsets_size; i++) {
          if (major <= add_child_view_offsets[i].ver_major && major >= prev_major) {
              if (add_child_view_offsets[i].ver_minor == 0 || minor == add_child_view_offsets[i].ver_minor) {
                  add_child_view_offset = add_child_view_offsets[i];
                  break;
              }
              add_child_view_offset = add_child_view_offsets[i];
              break;
          }
          prev_major = add_child_view_offsets[i].ver_major;
      }
      if (major >= add_child_view_offsets[offsets_size - 1].ver_major) {
          add_child_view_offset = add_child_view_offsets[offsets_size - 1];
      }
      Wh_Log(L"add_child_view offset: %#x", add_child_view_offset.offset);
    }

    Wh_SetFunctionHook((void*)cef_window_create_top_level,
                       (void*)cef_window_create_top_level_hook,
                       (void**)&cef_window_create_top_level_original);
    Wh_SetFunctionHook((void*)cef_panel_create, (void*)cef_panel_create_hook,
                      (void**)&cef_panel_create_original);
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    LoadSettings();
}
