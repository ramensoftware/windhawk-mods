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
* Supported CEF versions: 90.4 to 132
    * This mod won't work with versions before 90.4
    * Versions after 132 may work but are not tested
    * Variant of this mod using copy-pasted CEF structs instead of hardcoded offsets is available at [here](https://github.com/Ingan121/files/tree/master/cte)
    * Copy required structs/definitions from your wanted CEF version (available [here](https://cef-builds.spotifycdn.com/index.html)) and paste them to the above variant to calculate the offsets
    * Testing with cefclient: `cefclient.exe --use-views --hide-frame --hide-controls`
* Supported Spotify versions: 1.1.60 to 1.2.52 (newer versions may work)
* Spotify notes:
    * Old releases are available [here](https://docs.google.com/spreadsheets/d/1wztO1L4zvNykBRw7X4jxP8pvo11oQjT0O5DvZ_-S4Ok/edit?pli=1&gid=803394557#gid=803394557)
    * 1.1.60-1.1.67: Use [SpotifyNoControl](https://github.com/JulienMaille/SpotifyNoControl) to remove the window controls
    * 1.1.68-1.1.70: Window control hiding doesn't work yet
    * 1.1.74: Last version to support proper DWM window controls
    * Native DWM window controls are invisible since 1.1.75 as it began hooking window messages to support Windows 11 maximize button hover menu
    * 1.1.85: Last version to support proper non-DWM window controls
    * They are still visible after 1.1.85, but they don't respond to clicks
    * 1.2.7: First version to use Library X UI by default
    * 1.2.13: Last version to have the old UI
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

/* Spotify CEF version map
90.6: 1.1.60-1.1.62
91.1: 1.1.63-1.1.67
91.3: 1.1.68-1.1.70
94: 1.1.71
102: 1.1.89
106: 1.1.97-1.2.3
109: 1.2.4-1.2.6
110: 1.2.7
111: 1.2.8-1.2.10
112: 1.2.11-1.2.12
113: 1.2.13-1.2.19
115: 1.2.20
116: 1.2.21-1.2.22
117: 1.2.23-1.2.24
118: 1.2.25
119: 1.2.26
120: 1.2.28-1.2.30
121: 1.2.31-1.2.32
122: 1.2.33-1.2.37
124: 1.2.38-1.2.39
125: 1.2.40-1.2.44
127: 1.2.45-1.2.46
128: 1.2.47-1.2.48
129: 1.2.49-1.2.50
130: 1.2.51-1.2.52
*/

#include <libloaderapi.h>
#include <windhawk_utils.h>

#define CEF_CALLBACK __stdcall
#define CEF_EXPORT __cdecl
#define ANY_MINOR -1

struct cte_settings {
    BOOL showframe;
    BOOL showmenu;
    BOOL showcontrols;
} cte_settings;

typedef struct cte_offset {
  int ver_major;
  int ver_minor; // -1 for any
  int offset_x86;
  int offset_x64;
} cte_offset_t;

cte_offset_t is_frameless_offsets[] = {
    {90, 4, 0x48, 0x90},
    {90, 5, 0x48, 0x90},
    {90, 6, 0x48, 0x90},
    {91, 0, 0x48, 0x90},
    {91, 1, 0x48, 0x90},
    // (91.2 is found nowhere)
    {91, 3, 0x50, 0x90},
    {92, ANY_MINOR, 0x50, 0xa0},
    {101, ANY_MINOR, 0x50, 0xa0},
    {102, ANY_MINOR, 0x54, 0xa8},
    {107, ANY_MINOR, 0x54, 0xa8},
    {108, ANY_MINOR, 0x5c, 0xb8},
    {114, ANY_MINOR, 0x5c, 0xb8},
    {115, ANY_MINOR, 0x60, 0xc0},
    {116, ANY_MINOR, 0x60, 0xc0},
    {117, ANY_MINOR, 0x64, 0xc8},
    {123, ANY_MINOR, 0x64, 0xc8},
    {124, ANY_MINOR, 0x68, 0xd0},
    {130, ANY_MINOR, 0x68, 0xd0}
};

cte_offset_t add_child_view_offsets[] = {
    {94, ANY_MINOR, 0xf0, 0x1e0},
    {122, ANY_MINOR, 0xf0, 0x1e0},
    {124, ANY_MINOR, 0xf4, 0x1e8},
    {130, ANY_MINOR, 0xf4, 0x1e8}
};

int is_frameless_offset = NULL;
int add_child_view_offset = NULL;

typedef int CEF_CALLBACK (*is_frameless_t)(struct _cef_window_delegate_t* self, struct _cef_window_t* window);
int CEF_CALLBACK is_frameless_hook(struct _cef_window_delegate_t* self, struct _cef_window_t* window) {
    Wh_Log(L"is_frameless_hook");
    return 0;
}

typedef _cef_window_t* CEF_EXPORT (*cef_window_create_top_level_t)(void* delegate);
cef_window_create_top_level_t CEF_EXPORT cef_window_create_top_level_original;
_cef_window_t* CEF_EXPORT cef_window_create_top_level_hook(void* delegate) {
    Wh_Log(L"cef_window_create_top_level_hook");

    if (is_frameless_offset != NULL && cte_settings.showframe == TRUE) {
        ((is_frameless_t*)((char*)delegate + is_frameless_offset))[0] = is_frameless_hook;
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

typedef _cef_panel_t* CEF_EXPORT (*cef_panel_create_t)(void* delegate);
cef_panel_create_t CEF_EXPORT cef_panel_create_original;
_cef_panel_t* CEF_EXPORT cef_panel_create_hook(void* delegate) {
    Wh_Log(L"cef_panel_create_hook");
    _cef_panel_t* panel = cef_panel_create_original(delegate);
    if (add_child_view_offset != NULL) {
        add_child_view_original = ((add_child_view_t*)((char*)panel + add_child_view_offset))[0];
        ((add_child_view_t*)((char*)panel + add_child_view_offset))[0] = add_child_view_hook;
    }
    return panel;
}

typedef int (*cef_version_info_t)(int entry);

void LoadSettings() {
    cte_settings.showframe = Wh_GetIntSetting(L"showframe");
    cte_settings.showmenu = Wh_GetIntSetting(L"showmenu");
    cte_settings.showcontrols = Wh_GetIntSetting(L"showcontrols");
}

int FindOffset(int major, int minor, cte_offset_t offsets[], int offsets_size) {
    int prev_major = 90;
    for (int i = 0; i < offsets_size; i++) {
        if (major <= offsets[i].ver_major && major >= prev_major) {
            if (offsets[i].ver_minor == ANY_MINOR ||
                (minor == offsets[i].ver_minor && major == offsets[i].ver_major) // mandate exact major match here
            ) {
                #ifdef _WIN64
                    return offsets[i].offset_x64;
                #else
                    return offsets[i].offset_x86;
                #endif
            }
        }
        prev_major = offsets[i].ver_major;
    }
    if (major >= offsets[offsets_size - 1].ver_major) {
        #ifdef _WIN64
            return offsets[offsets_size - 1].offset_x64;
        #else
            return offsets[offsets_size - 1].offset_x86;
        #endif
    }
    return NULL;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    #ifdef _WIN64
        Wh_Log(L"Init - x86_64");
    #else
        Wh_Log(L"Init - x86");
    #endif

    LoadSettings();

    // Check if this process is auxilliary process by checking if the arguments contain --type=
    LPWSTR args = GetCommandLineW();
    if (wcsstr(args, L"--type=") != NULL) {
        Wh_Log(L"Auxilliary process detected, skipping");
        return FALSE;
    }

    HMODULE cefModule = LoadLibrary(L"libcef.dll");
    if (!cefModule) {
        Wh_Log(L"Failed to load CEF!");
        return FALSE;
    }
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
    is_frameless_offset = FindOffset(major, minor, is_frameless_offsets, ARRAYSIZE(is_frameless_offsets));
    Wh_Log(L"is_frameless offset: %#x", is_frameless_offset);

    if (isSpotify) {
        add_child_view_offset = FindOffset(major, minor, add_child_view_offsets, ARRAYSIZE(add_child_view_offsets));
        Wh_Log(L"add_child_view offset: %#x", add_child_view_offset);
    }

    if ((is_frameless_offset == NULL || !cte_settings.showframe) &&
        (!isSpotify || add_child_view_offset == NULL || (cte_settings.showmenu && cte_settings.showcontrols))
    ) {
        Wh_Log(L"Nothing to hook, exiting");
        if (is_frameless_offset == NULL) {
            Wh_Log(L"This version of CEF is not supported!");
        }
        return FALSE;
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
