// ==WindhawkMod==
// @id              cef-titlebar-enabler-universal
// @name            CEF/Spotify Tweaks
// @description     Various tweaks for Spotify, including native frames, transparent windows, and more
// @version         0.6
// @author          Ingan121
// @github          https://github.com/Ingan121
// @twitter         https://twitter.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         spotify.exe
// @include         cefclient.exe
// @compilerOptions -lcomctl32 -luxtheme -ldwmapi -DWINVER=0x0A00
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Spotify Tweaks
* Formerly known as CEF/Spotify Titlebar Enabler
* Force native frames and title bars for CEF apps, such as Spotify
* Only works on apps using native CEF top-level windows
    * Steam uses SDL for its top-level windows (except DevTools), so this mod doesn't work with Steam
* Electron apps are NOT supported! Just patch asar to override `frame: false` to true in BrowserWindow creation
## Features for Spotify
* Enable native frames and title bars on the main window
* Enable native frames and title bars on other windows, including Miniplayer, DevTools, etc.
* Hide the menu button or Spotify's custom window controls
* Make Spotify's custom window controls transparent
* Ignore the minimum window size set by Spotify
* Enable transparent rendering to make the transparent parts of the web contents transparent
* Disable forced dark mode to prevent Spotify from forcing dark mode on the CEF UI & web contents
* Force enable Chrome extension support
* Use the settings tab on the mod details page to configure the features
## Notes
* Supported CEF versions: 90.4 to 131
    * This mod won't work with versions before 90.4
    * Versions after 132 may work but are not tested
    * Variant of this mod using copy-pasted CEF structs instead of hardcoded offsets is available at [here](https://github.com/Ingan121/files/tree/master/cte)
    * Copy required structs/definitions from your wanted CEF version (available [here](https://cef-builds.spotifycdn.com/index.html)) and paste them to the above variant to calculate the offsets
    * Testing with cefclient: `cefclient.exe --use-views --hide-frame --hide-controls`
* Supported Spotify versions: 1.1.60 to 1.2.56 (newer versions may work)
* Spotify notes:
    * Old releases are available [here](https://docs.google.com/spreadsheets/d/1wztO1L4zvNykBRw7X4jxP8pvo11oQjT0O5DvZ_-S4Ok/edit?pli=1&gid=803394557#gid=803394557)
    * 1.1.60-1.1.67: Use [SpotifyNoControl](https://github.com/JulienMaille/SpotifyNoControl) to remove the window controls
    * 1.1.68-1.1.70: Window control hiding doesn't work
    * 1.2.7: First version to use Library X UI by default
    * 1.2.13: Last version to have the old UI
    * 1.2.28: First version to support Chrome runtime (disabled by default)
    * 1.2.45: Last version to support disabling the global navbar
    * 1.2.47: Chrome runtime is always enabled since this version
    * Try the [noControls](https://github.com/ohitstom/spicetify-extensions/tree/main/noControls) Spicetify extension to remove the space left by the custom window controls
    * Or try my [WMPotify](https://github.com/Ingan121/WMPotify) theme for Windows Media Player 11-like look
    * Enable Chrome runtime to get a proper window icon. Use `--enable-chrome-runtime` flag or put `app.enable-chrome-runtime=true` in `%appdata%\Spotify\prefs`
* Notes for Spicetify extension/theme developers
    * Use `window.outerHeight - window.innerHeight > 0` to detect if the window has a native title bar
    * This mod exposes a JavaScript API that can be used to interact with the main window and this mod
    * The API is available with `window.cancelEsperantoCall('ctewh')`
    * Use `cancelEsperantoCall('ctewh').query()` to get various information about the window and the mod
    * Various functions are available in the object returned by `cancelEsperantoCall('ctewh')`
    * See [here](https://github.com/Ingan121/WMPotify/blob/master/theme/src/js/WindhawkComm.js) for a simple example of how to use the functions
    * This API is only available on Spotify 1.2.4 and above, and only if the mod is enabled before Spotify starts
    * The API is disabled by default on untested CEF versions
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showframe: true
  $name: Enable native frames and title bars on the main window*
  $description: "(*): Requires a restart to take effect"
- showframeonothers: false
  $name: Enable native frames and title bars on other windows
  $description: Includes Miniplayer, DevTools, etc.
- showmenu: true
  $name: Show the menu button*
  $description: Disabling this also prevents opening the Spotify menu with the Alt key
- showcontrols: false
  $name: Show Spotify's custom window controls*
- transparentcontrols: false
  $name: Make Spotify's custom window controls transparent
- transparentrendering: true
  $name: Enable transparent rendering*
  $description: "Make the transparent parts of the web contents transparent\nWill use the ButtonFace color instead if the classic theme is being used and native frames are enabled\nChrome runtime is required for this to work"
- noforceddarkmode: false
  $name: Disable forced dark mode*
  $description: Prevents Spotify from forcing dark mode on the CEF UI & web contents
- forceextensions: true
  $name: Force enable Chrome extensions*
  $description: Always enable Chrome extension support, regardless of the DevTools status
- ignoreminsize: false
  $name: Ignore minimum window size
  $description: Allows resizing the window below the minimum size set by Spotify
- allowuntested: false
  $name: (Advanced) Use unsafe methods on untested CEF versions*
  $description: Allows calling unsafe functions on untested CEF versions. May cause crashes or other issues. If disabled, an inefficient alternative method will be used on untested versions. JS API will also be disabled on untested versions
*/
// ==/WindhawkModSettings==

/* Spotify CEF version map
90.6: 1.1.60-1.1.62
91.1: 1.1.63-1.1.67
91.3: 1.1.68-1.1.70
94: 1.1.71
95: 1.1.74-1.1.75
96: 1.1.76
98: 1.1.81
100: 1.1.85
101: 1.1.86-1.1.88
102: 1.1.89
104: 1.1.94
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
131: 1.2.53, 1.2.55-1.2.56
*/

#include <libloaderapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <condition_variable>
#include <cstdint>
#include <thread>
#include <mutex>
#include <regex>
#include <string_view>
#include <vector>
#include <aclapi.h>
#include <dwmapi.h>
#include <sddl.h>
#include <uxtheme.h>
#include <windows.h>

using namespace std::string_view_literals;

#define CEF_CALLBACK __stdcall
#define CEF_EXPORT __cdecl
#define cef_window_handle_t HWND
#define ANY_MINOR -1
#define PIPE_NAME L"\\\\.\\pipe\\CTEWH-IPC"
#define LAST_TESTED_CEF_VERSION 131

// Win11 only DWM attributes for Windhawk 1.4
#define DWMWA_USE_HOSTBACKDROPBRUSH 17
#define DWMWA_SYSTEMBACKDROP_TYPE 38

#define DWMSBT_MAINWINDOW 2
#define DWMSBT_TRANSIENTWINDOW 3
#define DWMSBT_TABBEDWINDOW 4

struct cte_settings {
    BOOL showframe;
    BOOL showframeonothers;
    BOOL showmenu;
    BOOL showcontrols;
    BOOL transparentcontrols;
    BOOL ignoreminsize;
    BOOL transparentrendering;
    BOOL noforceddarkmode;
    BOOL forceextensions;
    BOOL allowuntested;
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
    {91, 3, 0x50, 0xa0},
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
    {124, ANY_MINOR, 0x68, 0xd0}
};

cte_offset_t add_child_view_offsets[] = {
    {94, ANY_MINOR, 0xf0, 0x1e0},
    {122, ANY_MINOR, 0xf0, 0x1e0},
    {124, ANY_MINOR, 0xf4, 0x1e8},
    {130, ANY_MINOR, 0xf4, 0x1e8},
    {131, ANY_MINOR, 0xf8, 0x1f0}
};

cte_offset_t get_window_handle_offsets[] = {
    {94, ANY_MINOR, 0x184, 0x308},
    {114, ANY_MINOR, 0x184, 0x308},
    {115, ANY_MINOR, 0x188, 0x310},
    {123, ANY_MINOR, 0x188, 0x310},
    {124, ANY_MINOR, 0x18c, 0x318},
    {130, ANY_MINOR, 0x18c, 0x318},
    {131, ANY_MINOR, 0x194, 0x328},
    {132, ANY_MINOR, 0x194, 0x328}
};

cte_offset_t set_background_color_offsets[] = {
    {94, ANY_MINOR, 0xbc, 0x178},
    {130, ANY_MINOR, 0xbc, 0x178},
    {131, ANY_MINOR, 0xc0, 0x180}
};

int is_frameless_offset = NULL;
int add_child_view_offset = NULL;
int get_window_handle_offset = NULL;
int set_background_color_offset = NULL;

BOOL g_isSpotifyRenderer = FALSE;

HWND g_mainHwnd = NULL;
int g_minWidth = -1;
int g_minHeight = -1;
BOOL g_hwAccelerated = FALSE;
BOOL g_dwmBackdropEnabled = FALSE;
BOOL g_titleLocked = FALSE;

double g_playbackSpeed = 1.0;
int64_t g_currentTrackPlayer = NULL;

HANDLE g_hPipe = INVALID_HANDLE_VALUE;
BOOL g_shouldClosePipe = FALSE;
std::thread g_pipeThread;

std::condition_variable g_queryResponseCv;
std::mutex g_ipcMutex;
bool g_queryResponseReceived = false;

struct cte_queryResponse_t {
    BOOL success;
    BOOL isMaximized;
    BOOL isTopMost;
    BOOL isLayered;
    BOOL isThemingEnabled;
    BOOL isDwmEnabled;
    BOOL dwmBackdropEnabled;
    BOOL hwAccelerated;
    int minWidth;
    int minHeight;
    BOOL titleLocked;
    int dpi;
    BOOL speedModSupported;
    double playbackSpeed;
    BOOL immediateSpeedChange;
} g_queryResponse;

#pragma region CEF structs (as minimal and cross-version compatible as possible)
// Copyright (c) 2024 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

typedef struct _cef_string_utf16_t {
  char16_t* str;
  size_t length;
  void (*dtor)(char16_t* str);
} cef_string_utf16_t;

typedef cef_string_utf16_t cef_string_t;
typedef cef_string_utf16_t* cef_string_userfree_utf16_t;
typedef cef_string_userfree_utf16_t cef_string_userfree_t;

typedef struct _cef_string_list_t* cef_string_list_t;

typedef uint32_t cef_color_t;

typedef struct _cef_base_ref_counted_t {
  size_t size;
  void(CEF_CALLBACK* add_ref)(struct _cef_base_ref_counted_t* self);
  int(CEF_CALLBACK* release)(struct _cef_base_ref_counted_t* self);
  int(CEF_CALLBACK* has_one_ref)(struct _cef_base_ref_counted_t* self);
  int(CEF_CALLBACK* has_at_least_one_ref)(struct _cef_base_ref_counted_t* self);
} cef_base_ref_counted_t;

typedef struct _cef_size_t {
  int width;
  int height;
} cef_size_t;

typedef struct _cef_view_delegate_t {
  cef_base_ref_counted_t base;
  cef_size_t(CEF_CALLBACK* get_preferred_size)(struct _cef_view_delegate_t* self);
} cef_view_delegate_t;

typedef struct _cef_panel_delegate_t {
  cef_view_delegate_t base;
} cef_panel_delegate_t;

typedef struct _cef_basetime_t {
  int64_t val;
} cef_basetime_t;

typedef enum {
  V8_PROPERTY_ATTRIBUTE_NONE = 0,
  V8_PROPERTY_ATTRIBUTE_READONLY = 1 << 0,
  V8_PROPERTY_ATTRIBUTE_DONTENUM = 1 << 1,
  V8_PROPERTY_ATTRIBUTE_DONTDELETE = 1 << 2
} cef_v8_propertyattribute_t;

typedef struct _cef_v8handler_t {
  cef_base_ref_counted_t base;
  int(CEF_CALLBACK* execute)(struct _cef_v8handler_t* self,
                             const cef_string_t* name,
                             struct _cef_v8value_t* object,
                             size_t argumentsCount,
                             struct _cef_v8value_t* const* arguments,
                             struct _cef_v8value_t** retval,
                             cef_string_t* exception);
} cef_v8handler_t;

// CEF 108+
typedef struct _cef_v8value_t {
    cef_base_ref_counted_t base;

    int(CEF_CALLBACK* is_valid)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_undefined)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_null)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_bool)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_int)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_uint)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_double)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_date)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_string)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_object)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_array)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_array_buffer)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_function)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_promise)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_same)(struct _cef_v8value_t* self, struct _cef_v8value_t* that);
    int(CEF_CALLBACK* get_bool_value)(struct _cef_v8value_t* self);
    int32_t(CEF_CALLBACK* get_int_value)(struct _cef_v8value_t* self);
    uint32_t(CEF_CALLBACK* get_uint_value)(struct _cef_v8value_t* self);
    double(CEF_CALLBACK* get_double_value)(struct _cef_v8value_t* self);
    cef_basetime_t(CEF_CALLBACK* get_date_value)(struct _cef_v8value_t* self);
    cef_string_userfree_t(CEF_CALLBACK* get_string_value)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* is_user_created)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* has_exception)(struct _cef_v8value_t* self);
    struct _cef_v8exception_t*(CEF_CALLBACK* get_exception)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* clear_exception)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* will_rethrow_exceptions)(struct _cef_v8value_t* self);
    int(CEF_CALLBACK* set_rethrow_exceptions)(struct _cef_v8value_t* self, int rethrow);
    int(CEF_CALLBACK* has_value_bykey)(struct _cef_v8value_t* self, const cef_string_t* key);
    int(CEF_CALLBACK* has_value_byindex)(struct _cef_v8value_t* self, int index);
    int(CEF_CALLBACK* delete_value_bykey)(struct _cef_v8value_t* self, const cef_string_t* key);
    int(CEF_CALLBACK* delete_value_byindex)(struct _cef_v8value_t* self, int index);
    struct _cef_v8value_t*(CEF_CALLBACK* get_value_bykey)(struct _cef_v8value_t* self, const cef_string_t* key);
    struct _cef_v8value_t*(CEF_CALLBACK* get_value_byindex)(struct _cef_v8value_t* self, int index);
    int(CEF_CALLBACK* set_value_bykey)(struct _cef_v8value_t* self, const cef_string_t* key, struct _cef_v8value_t* value, cef_v8_propertyattribute_t attribute);
    int(CEF_CALLBACK* set_value_byindex)(struct _cef_v8value_t* self, int index, struct _cef_v8value_t* value);
    // below here is updated quite recently (CEF 126), so it's avoided
} cef_v8value_t;
#pragma endregion

#pragma region CEF V8 functions + helpers
typedef cef_v8value_t* CEF_EXPORT (*cef_v8value_create_function_t)(const cef_string_t* name, cef_v8handler_t* handler);
cef_v8value_create_function_t CEF_EXPORT cef_v8value_create_function_original;

typedef cef_v8value_t* CEF_EXPORT (*cef_v8value_create_bool_t)(int value);
cef_v8value_create_bool_t cef_v8value_create_bool;

typedef cef_v8value_t* CEF_EXPORT (*cef_v8value_create_int_t)(int value);
cef_v8value_create_int_t cef_v8value_create_int;

typedef cef_v8value_t* CEF_EXPORT (*cef_v8value_create_double_t)(double value);
cef_v8value_create_double_t cef_v8value_create_double;

typedef cef_v8value_t* CEF_EXPORT (*cef_v8value_create_string_t)(const cef_string_t* value);
cef_v8value_create_string_t cef_v8value_create_string;

typedef cef_v8value_t* CEF_EXPORT (*cef_v8value_create_object_t)(void* accessor, void* interceptor);
cef_v8value_create_object_t cef_v8value_create_object;

typedef cef_v8value_t* CEF_EXPORT (*cef_v8value_create_array_t)(int length);
cef_v8value_create_array_t cef_v8value_create_array;

typedef int (*cef_version_info_t)(int entry);

cef_string_t* GenerateCefString(std::u16string str) {
    cef_string_t* cefStr = (cef_string_t*)calloc(1, sizeof(cef_string_t));
    cefStr->str = (char16_t*)calloc(str.size() + 1, sizeof(char16_t));
    cefStr->length = str.size();
    memcpy(cefStr->str, str.c_str(), str.size() * sizeof(char16_t));
    return cefStr;
}

cef_string_t* GenerateCefString(std::wstring str) {
    return GenerateCefString(std::u16string(str.begin(), str.end()));
}

cef_string_t* FormatCefString(const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);
    int len = _vscwprintf(format, args);
    std::wstring str(len + 1, L'\0');
    vswprintf_s(&str[0], len + 1, format, args);
    va_end(args);
    return GenerateCefString(std::u16string(str.begin(), str.end()));
}

int AddFunctionToObj(cef_v8value_t* obj, std::u16string name, cef_v8handler_t* handler) {
    cef_string_t* cefName = GenerateCefString(name);
    cef_v8value_t* func = cef_v8value_create_function_original(cefName, handler);
    int result = obj->set_value_bykey(obj, cefName, func, V8_PROPERTY_ATTRIBUTE_NONE);
    free(cefName->str);
    free(cefName);
    return result;
}

int AddValueToObj(cef_v8value_t* obj, std::u16string name, cef_v8value_t* value) {
    cef_string_t* cefName = GenerateCefString(name);
    int result = obj->set_value_bykey(obj, cefName, value, V8_PROPERTY_ATTRIBUTE_NONE);
    free(cefName->str);
    free(cefName);
    return result;
}

int AddValueToObj(cef_v8value_t* obj, std::u16string name, std::u16string value) {
    cef_string_t* cefValue = GenerateCefString(value);
    int result = AddValueToObj(obj, name, cef_v8value_create_string(cefValue));
    free(cefValue->str);
    free(cefValue);
    return result;
}
#pragma endregion

void CreateNamedPipeServer();

// Whether DwmExtendFrameIntoClientArea should be called
// False if DWM is disabled, visual styles are disabled, or some kind of basic themer is used
BOOL IsDwmEnabled() {
    if (!IsAppThemed() && !IsThemeActive()) {
        return FALSE;
    }
    BOOL dwmEnabled = FALSE;
    DwmIsCompositionEnabled(&dwmEnabled);
    BOOL dwmFrameEnabled = FALSE;
    if (dwmEnabled && g_mainHwnd != NULL) {
        HRESULT hr = DwmGetWindowAttribute(g_mainHwnd, DWMWA_NCRENDERING_ENABLED, &dwmFrameEnabled, sizeof(dwmFrameEnabled));
        if (!SUCCEEDED(hr)) {
            return dwmEnabled;
        }
    } else {
        return dwmEnabled;
    }
    return dwmEnabled && dwmFrameEnabled;
}

#pragma region Subclassing
LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    // dwRefData is 1 if the window is created by cef_window_create_top_level
    // Assumed 1 if this mod is loaded after the window is created
    // dwRefData is 2 if the window is created by cef_window_create_top_level and is_frameless is hooked
    switch (uMsg) {
        case WM_NCACTIVATE:
            if (g_dwmBackdropEnabled) {
                // Fix MicaForEveryone not working well with native frames
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
            break;
        case WM_NCPAINT:
            if (hWnd == g_mainHwnd && g_hwAccelerated && cte_settings.transparentrendering && !cte_settings.showframe && !IsDwmEnabled()) {
                // Do not draw anything
                return 0;
            }
        case WM_NCHITTEST:
        case WM_NCLBUTTONDOWN:
        case WM_NCCALCSIZE:
            // Unhook Spotify's custom window control event handling
            // Also unhook WM_NCPAINT to fix non-DWM frames randomly going black
            // WM_NCCALCSIZE is only for windows with Chrome's custom frame (DevTools, Miniplayer, full browser UI, etc.)
            if (dwRefData) {
                if (cte_settings.showframe == TRUE || dwRefData == 2) {
                    return DefWindowProc(hWnd, uMsg, wParam, lParam);
                }
            } else if (cte_settings.showframeonothers == TRUE) {
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
            break;
        case WM_GETMINMAXINFO:
            if (g_minWidth != -1 && g_minHeight != -1) {
                RECT windowRect, clientRect;
                GetWindowRect(hWnd, &windowRect);
                GetClientRect(hWnd, &clientRect);
                MINMAXINFO* mmi = (MINMAXINFO*)lParam;
                mmi->ptMinTrackSize.x = g_minWidth + (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
                mmi->ptMinTrackSize.y = g_minHeight + (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);
                return 0;
            } else if (cte_settings.ignoreminsize == TRUE) {
                MINMAXINFO* mmi = (MINMAXINFO*)lParam;
                mmi->ptMinTrackSize.x = 0;
                mmi->ptMinTrackSize.y = 0;
                return 0;
            }
            break;
        case WM_PAINT:
            if (hWnd == g_mainHwnd && g_hwAccelerated && cte_settings.transparentrendering && !cte_settings.showframe) {
                // Do not draw anything
                ValidateRect(hWnd, NULL);
                return 0;
            }
            break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK UpdateEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        // Update NonClient size
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        if (wcsncmp(className, L"Chrome_WidgetWin_", 17) == 0) {
            if (lParam == 1) {
                // Really move the window a bit to make Spotify update window control colors
                RECT rect;
                GetWindowRect(hWnd, &rect);
                SetWindowPos(hWnd, NULL, rect.left, rect.top + 1, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
                SetWindowPos(hWnd, NULL, rect.left, rect.top, 0, 0, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
            } else {
                SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
            }
        }
    }
    return TRUE;
}

BOOL CALLBACK InitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    // Subclass all relevant windows belonging to this process
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        if (wcsncmp(className, L"Chrome_WidgetWin_", 17) == 0) {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 1)) {
                Wh_Log(L"Subclassed %p", hWnd);
                if (lParam == 1) {
                    UpdateEnumWindowsProc(hWnd, 0);
                }
            }
        }
    }
    return TRUE;
}

BOOL CALLBACK UninitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    // Unsubclass all windows belonging to this process
    if (pid == GetCurrentProcessId()) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
        if (lParam == 1) {
            UpdateEnumWindowsProc(hWnd, 0);
        }
    }
    return TRUE;
}
#pragma endregion

#pragma region Memory patches
// From https://windhawk.net/mods/visual-studio-anti-rich-header
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

// Pass an empty targetPatch to use it as a regex search
int64_t PatchMemory(char* pbExecutable, const std::string& targetRegex, const std::vector<uint8_t>& targetPatch, int expectedSection = -1, int maxMatch = -1) {
    IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)pbExecutable;
    IMAGE_NT_HEADERS* pNtHeader = (IMAGE_NT_HEADERS*)((char*)pDosHeader + pDosHeader->e_lfanew);
    IMAGE_SECTION_HEADER* pSectionHeader = (IMAGE_SECTION_HEADER*)((char*)&pNtHeader->OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);

    std::regex regex(ReplaceAll(targetRegex, ".", R"([\s\S])"));
    std::match_results<std::string_view::const_iterator> match;
    bool foundAnyMatch = false;

    for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; ++i) {
        if (expectedSection != -1 && i != expectedSection) {
            continue;
        }

        char* from = pbExecutable + pSectionHeader[i].VirtualAddress;
        char* to = from + pSectionHeader[i].SizeOfRawData;

        std::string_view search(from, to - from);

        int matchCount = 0;

        while (std::regex_search(search.begin(), search.end(), match, regex)) {
            auto pos = from + match.position(0);

            Wh_Log(L"Match found in section %d at position: %p", i, pos);

            if (targetPatch.size() == 0) {
                // Just return the address of the first match
                return (int64_t)pos;
            }

            // #include <iomanip>
            // #include <sstream>
            // // Log the bytes before patching
            // std::string beforePatch(pos, targetPatch.size());
            // std::ostringstream hexStreamBefore;
            // for (size_t i = 0; i < targetPatch.size(); ++i) {
            //     hexStreamBefore << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)beforePatch[i] << " ";
            // }
            // std::string hexStreamStrBefore = hexStreamBefore.str();
            // std::wstring hexStreamWStrBefore(hexStreamStrBefore.begin(), hexStreamStrBefore.end());
            // Wh_Log(L"Hex bytes before patch: %s", hexStreamWStrBefore.c_str());

            DWORD dwOldProtect;
            if (VirtualProtect(pos, targetPatch.size(), PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
                memcpy(pos, targetPatch.data(), targetPatch.size());
                VirtualProtect(pos, targetPatch.size(), dwOldProtect, &dwOldProtect);
                Wh_Log(L"Patch applied successfully.");

                // // Log the bytes after patching
                // std::string afterPatch(pos, targetPatch.size());
                // std::ostringstream hexStreamAfter;
                // for (size_t i = 0; i < targetPatch.size(); ++i) {
                //     hexStreamAfter << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)afterPatch[i] << " ";
                // }
                // std::string hexStreamStrAfter = hexStreamAfter.str();
                // std::wstring hexStreamWStrAfter(hexStreamStrAfter.begin(), hexStreamStrAfter.end());
                // Wh_Log(L"Hex bytes after patch: %s", hexStreamWStrAfter.c_str());

                foundAnyMatch = true;
            } else {
                Wh_Log(L"Failed to change memory protection.");
            }

            if (maxMatch != -1 && ++matchCount >= maxMatch) {
                break;
            }

            // Move the search start position to after the current match
            from = pos + targetPatch.size();
            search = std::string_view(from, to - from);
        }
    }

    if (!foundAnyMatch) {
        Wh_Log(L"No match found for the regex pattern.");
    }

    return foundAnyMatch;
}

BOOL EnableTransparentRendering(char* pbExecutable) {
    std::vector<uint8_t> targetPatch = {0xba, 0x00, 0x00, 0x00, 0x00, 0xff};

    // Use ButtonFace color if classic theme is used and frames are enabled
    if (!IsAppThemed() && !IsThemeActive() && cte_settings.showframe == TRUE) {
        DWORD btnFace = GetSysColor(COLOR_BTNFACE); // ButtonFace color
        targetPatch[1] = ((btnFace >> 16) & 0xFF);
        targetPatch[2] = ((btnFace >> 8) & 0xFF);
        targetPatch[3] = (btnFace & 0xFF);
        targetPatch[4] = 0xFF; // Opaque
    }

    #ifdef _WIN64
        std::string targetRegex = R"(\xba\x12\x12\x12\xff\xff)"; // mov edx, 0xff121212 (default background color)
    #else
        std::string targetRegex = R"(\x68\x12\x12\x12\xff\x8b)"; // push 0xff121212
        targetPatch[0] = 0x68;
        targetPatch[5] = 0x8b;
    #endif

    return PatchMemory(pbExecutable, targetRegex, targetPatch, 0, 4);
}

BOOL DisableForcedDarkMode(char* pbExecutable) {
    std::string targetRegex = R"(force-dark-mode)";
    std::string targetPatch = "some-invalidarg";
    std::vector<uint8_t> targetPatchBytes(targetPatch.begin(), targetPatch.end());
    return PatchMemory(pbExecutable, targetRegex, targetPatchBytes, 1, 1);
}

BOOL ForceEnableExtensions(char* pbExecutable) {
    std::string targetRegex = R"(disable-extensions)";
    std::string targetPatch = "enable-extensions!";
    std::vector<uint8_t> targetPatchBytes(targetPatch.begin(), targetPatch.end());
    return PatchMemory(pbExecutable, targetRegex, targetPatchBytes, 1, 1);
}
#pragma endregion

#pragma region Spotify.exe hooks (non-exported functions)
#ifdef _WIN64
// From https://windhawk.net/mods/chrome-ui-tweaks
typedef struct {
    std::string_view search; // instructions to search for
    std::string_view prologue; // prologue of the function to be hooked (instructions at entry point)
    const size_t instr_offset; // estimated location of the searched instructions relative to the entry point
} function_search;

// Wrapper for string_view::find, that checks the needle is unique within the haystack.
const char* unique_search(std::string_view haystack, std::string_view needle, LPCWSTR symbol_name) {
    size_t index1 = haystack.find(needle);
    if (index1 == std::string_view::npos) {
        Wh_Log(L"Error: Couldn't find instructions for symbol %s", symbol_name);
        return NULL;
    }
    // Can we find the same sequence again in the rest of the haystack?
    size_t index2 = haystack.find(needle, index1 + 1);
    if (index2 != std::string_view::npos) {
        Wh_Log(L"Error: Found multiple matches for %s: at %p and at %p", symbol_name, haystack.begin()+index1, haystack.begin()+index2);
        // log_hexdump((unsigned char*)haystack.begin() + index1 - 0x50, 6);
        // Wh_Log(L"----------------");
        // log_hexdump((unsigned char*)haystack.begin() + index2 - 0x50, 6);
        return NULL;
    }
    return haystack.begin() + index1;
}

// get address and size of code section via PE header info  (expect around 200 MB)
// TODO is this actually correct? (does it include superfluous sections of the DLL?)
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format
// https://learn.microsoft.com/en-us/archive/msdn-magazine/2002/february/inside-windows-win32-portable-executable-file-format-in-detail
std::string_view getCodeSection(HMODULE chromeModule) {
    if (chromeModule == NULL) return ""sv;
    IMAGE_DOS_HEADER* dos_header = (IMAGE_DOS_HEADER*) chromeModule;
    IMAGE_NT_HEADERS* pe_header = (IMAGE_NT_HEADERS*)(((char*)dos_header) + dos_header->e_lfanew);
    if (pe_header->FileHeader.Machine != 0x8664 || pe_header->OptionalHeader.Magic != 0x20b) {
        Wh_Log(L"Mod only implemented for 64-bit windows/chrome - machine was 0x%04x and magic was 0x%04x",
        pe_header->FileHeader.Machine, pe_header->OptionalHeader.Magic);
        return ""sv;
    }
    return std::string_view{
        ((char*)dos_header) + pe_header->OptionalHeader.BaseOfCode,
        pe_header->OptionalHeader.SizeOfCode
    };
}

// Find a function address by scanning for specific instruction patterns.
// We search the entire code section once per function,
// to ensure we aren't hooking the wrong location.
// Better safe than sorry, and it shouldn't cause noticeable delay on startup.
const char* search_function_instructions(std::string_view code_section, function_search fsearch, LPCWSTR symbol_name) {
    if (code_section.size()==0) return 0;
    Wh_Log(L"Searching for function %s", symbol_name);
    const char* addr = unique_search(code_section, fsearch.search, symbol_name);
    if (addr == NULL) {
        Wh_Log(L"Could not find function %s; is the mod up to date?", symbol_name);
        return NULL;
    }
    Wh_Log(L"Instructions were found at address: %p", addr);
    int offset = fsearch.instr_offset;
    const char* entry = addr - offset;
    // verify the prologue is what we expect; otherwise search for it
    // and verify it is preceded by 0xcc INT3 or 0xc3 RET (or ?? JMP)
    auto prologue = fsearch.prologue;
    if (prologue != std::string_view{entry, prologue.size()}) {
        Wh_Log(L"Prologue not found where expected, searching...");
        // maybe function length changed due to different compilation
        auto search_space = std::string_view{entry - 0x40, addr};
        size_t new_offset = search_space.rfind(prologue);
        if (new_offset != std::string_view::npos) {
            entry = (char*)search_space.begin() + new_offset;
        } else {
            entry = NULL;
        }
    }
    if (entry) {
        Wh_Log(L"Found entrypoint for function %s at addr %p", symbol_name, entry);
        if (entry[-1]!=(char)0xcc && entry[-1]!=(char)0xc3) {
            Wh_Log(L"Warn: prologue not preceded by INT3 or RET");
        }
        return entry;
    } else {
        Wh_Log(L"Err: Couldn't locate function entry point for symbol %s", symbol_name);
        // log_hexdump(addr - 0x40, 0x5);
        return NULL;
    }
}

typedef uint64_t* __fastcall (*CreateTrackPlayer_t)(
    int64_t trackPlayer,
    void* a2,
    void* a3,
    double speed,
    unsigned int a5,
    unsigned int a6,
    int a7,
    int64_t a8,
    unsigned int a9,
    int64_t a10,
    int64_t a11,
    int64_t a12
);
CreateTrackPlayer_t CreateTrackPlayer_original;
uint64_t* __fastcall CreateTrackPlayer_hook(
    int64_t trackPlayer,
    uint64_t* a2,
    uint64_t* a3,
    double speed,
    unsigned int a5,
    unsigned int a6,
    int a7,
    int64_t a8,
    unsigned int a9,
    int64_t a10,
    int64_t a11,
    int64_t a12
) {
    Wh_Log(L"CreateTrackPlayer_hook");
    g_currentTrackPlayer = trackPlayer;
    return CreateTrackPlayer_original(trackPlayer, a2, a3, g_playbackSpeed, a5, a6, a7, a8, a9, a10, a11, a12);
}

// Find this function with xref of string "    Creating track player for track (playback_id %s)"
const std::string_view CreateTrackPlayer_instructions =
    "\x01"sv         // just a single byte before the instructions below, to distinguish from another match
    "\x49\x8B\x0F"sv // mov rcx, [r15]
    "\x48\x8B\x01"sv // mov rax, [rcx]
    "\xFF\x50\x38"sv // call qword ptr [rax+38h]
    "\x48\x8D"sv;    // lea rdx, (followed by address of "yes" in .rdata)
const std::string_view CreateTrackPlayer_prologue = "\x48\x8B\xC4USVWATAUAVAWH"sv;

typedef char __fastcall (*SetPlaybackSpeed_t)(int64_t trackPlayer, double speed);
SetPlaybackSpeed_t SetPlaybackSpeed;

// Find this function with xref of string "Setting playback speed to %d percent (playback_id %s) from %d percent"
// This function's various numbers are different in every version, so we need to perform a regex search
const std::string SetPlaybackSpeed_instructions =
    R"(\x48\x8B\xC4)"                // mov rax, rsp (beginning of function)
    R"(\x48\x89\x58\x18)"            // mov [rax+18h], rbx
    R"(\x48\x89\x70\x20)"            // mov [rax+20h], rsi
    R"(\x55\x57\x41\x56)"            // push rbp, push rdi, push r14
    R"(\x48\x8D\xA8.?\xFD\xFF\xFF)"; // lea rbp, [rax-??h]

// Only works on Spotify x64 1.2.36 and newer
// No plans to support x86 or older versions
BOOL HookCreateTrackPlayer(char* pbExecutable, BOOL shouldFindSetPlaybackSpeed) {
    std::string_view code_section = getCodeSection((HMODULE)pbExecutable);
    if (code_section.size() == 0) return FALSE;
    const char* addr = search_function_instructions(
        code_section,
        {
            .search = CreateTrackPlayer_instructions,
            .prologue = CreateTrackPlayer_prologue,
            .instr_offset = 0xBA0
        },
        L"CreateTrackPlayer"
    );
    if (addr == NULL) return FALSE;
    Wh_Log(L"Hooking CreateTrackPlayer at %p", addr);
    Wh_SetFunctionHook((void*)addr, (void*)CreateTrackPlayer_hook, (void**)&CreateTrackPlayer_original);

    // This only works on Spotify x64 1.2.45 and newer
    // Don't find SetPlaybackSpeed on a known unsupported version, as finding non-existent instructions will delay startup
    if (shouldFindSetPlaybackSpeed) {
        SetPlaybackSpeed = (SetPlaybackSpeed_t)PatchMemory(pbExecutable, SetPlaybackSpeed_instructions, {}, 0, 1);
        Wh_Log(L"SetPlaybackSpeed at %p", SetPlaybackSpeed);
    }
    return TRUE;
}
#else
#define CreateTrackPlayer_original NULL
#define SetPlaybackSpeed NULL
#endif
#pragma endregion

#pragma region CEF hooks
typedef int CEF_CALLBACK (*is_frameless_t)(struct _cef_window_delegate_t* self, struct _cef_window_t* window);
int CEF_CALLBACK is_frameless_hook(struct _cef_window_delegate_t* self, struct _cef_window_t* window) {
    Wh_Log(L"is_frameless_hook");
    return 0;
}

typedef cef_window_handle_t CEF_CALLBACK (*get_window_handle_t)(struct _cef_window_t* self);

typedef _cef_window_t* CEF_EXPORT (*cef_window_create_top_level_t)(void* delegate);
cef_window_create_top_level_t CEF_EXPORT cef_window_create_top_level_original;
_cef_window_t* CEF_EXPORT cef_window_create_top_level_hook(void* delegate) {
    Wh_Log(L"cef_window_create_top_level_hook");

    BOOL is_frameless_hooked = FALSE;
    if (is_frameless_offset != NULL && cte_settings.showframe == TRUE) {
        *((is_frameless_t*)((char*)delegate + is_frameless_offset)) = is_frameless_hook;
        is_frameless_hooked = TRUE;
    }
    _cef_window_t* window = cef_window_create_top_level_original(delegate);
    if (get_window_handle_offset != NULL) {
        get_window_handle_t get_window_handle = *((get_window_handle_t*)((char*)window + get_window_handle_offset));
        HWND hWnd = get_window_handle(window);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, is_frameless_hooked ? 2 : 1)) {
            Wh_Log(L"Subclassed %p", hWnd);
            if (g_hwAccelerated && cte_settings.transparentrendering) {
                InvalidateRect(hWnd, NULL, TRUE);
                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
            }
        }
        if (g_mainHwnd == NULL) {
            g_pipeThread = std::thread([=]() {
                CreateNamedPipeServer();
            });
            g_pipeThread.detach();
            g_mainHwnd = hWnd;
        }
    } else {
        // Just subclass everything again if get_window_handle is not available
        // Calling functions from invalid offsets will crash the app for sure
        EnumWindows(UninitEnumWindowsProc, 0);
        EnumWindows(InitEnumWindowsProc, 1);
        Wh_Log(L"Avoided calling get_window_handle on an untested version");
    }
    return window;
}

typedef void CEF_CALLBACK (*set_background_color_t)(struct _cef_view_t* self, cef_color_t color);
set_background_color_t CEF_CALLBACK set_background_color_original;
void CEF_CALLBACK set_background_color_hook(struct _cef_view_t* self, cef_color_t color) {
    //Wh_Log(L"set_background_color_hook: %#x", color);
    // 0x87000000: normal, 0x3fffffff: hover, 0x33ffffff: active, 0xffc42b1c: close button hover, 0xff941320: close button active
    if (color == 0x87000000 && cte_settings.transparentcontrols == TRUE) {
        color = 0x00000000;
    }
    set_background_color_original(self, color);
    return;
}

struct cte_control_container {
    set_background_color_t CEF_CALLBACK set_background_color_original;
    set_background_color_t* CEF_CALLBACK set_background_color_addr;
} cte_controls[3];

int cnt = -1;

typedef void CEF_CALLBACK (*add_child_view_t)(struct _cef_panel_t* self, struct _cef_view_t* view);
add_child_view_t CEF_CALLBACK add_child_view_original;
void CEF_CALLBACK add_child_view_hook(struct _cef_panel_t* self, struct _cef_view_t* view) {
    cnt++;
    Wh_Log(L"add_child_view_hook: %d", cnt);
    // 0: Minimize, 1: Maximize, 2: Close, 3: Menu (removing this also prevents alt key from working)
    if (cnt < 3) {
      if (cte_settings.showcontrols == FALSE) {
        return;
      }
    } else if (cte_settings.showmenu == FALSE) {
      return;
    }
    if (cnt < 3 && set_background_color_offset != NULL) {
        set_background_color_original = *((set_background_color_t*)((char*)view + set_background_color_offset));
        *((set_background_color_t*)((char*)view + set_background_color_offset)) = set_background_color_hook;
        cte_controls[cnt].set_background_color_original = set_background_color_original;
        cte_controls[cnt].set_background_color_addr = (set_background_color_t*)((char*)view + set_background_color_offset);
    }
    add_child_view_original(self, view);
    return;
}

typedef _cef_panel_t* CEF_EXPORT (*cef_panel_create_t)(cef_panel_delegate_t* delegate);
cef_panel_create_t CEF_EXPORT cef_panel_create_original;
_cef_panel_t* CEF_EXPORT cef_panel_create_hook(cef_panel_delegate_t* delegate) {
    Wh_Log(L"cef_panel_create_hook");
    if ((cnt != 2 || cte_settings.showmenu == FALSE) && // left panel
        (cnt != -1 || cte_settings.showcontrols == FALSE) // right panel
    ) {
        // Nullify get_preferred_size to make the leftover space from hiding the window controls clickable
        // This has side effect of making the menu button ignore the height set by cosmos endpoint (used by noControls Spicetify extension)
        // So only nullify get_preferred_size for the left panel if menu button is hidden
        delegate->base.get_preferred_size = NULL;
    }
    _cef_panel_t* panel = cef_panel_create_original(delegate);
    if (add_child_view_offset != NULL) {
        add_child_view_original = *((add_child_view_t*)((char*)panel + add_child_view_offset));
        *((add_child_view_t*)((char*)panel + add_child_view_offset)) = add_child_view_hook;
    }
    return panel;
}
#pragma endregion

#pragma region Win32 API hooks
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_original;
HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    Wh_Log(L"CreateWindowExW_hook");
    HWND hWnd = CreateWindowExW_original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd != NULL) {
        wchar_t className[256];
        GetClassName(hWnd, className, 256);
        if (wcsncmp(className, L"Chrome_WidgetWin_", 17) == 0) { // Chrome_WidgetWin_1: with Chrome runtime, Chrome_WidgetWin_0: without Chrome runtime (Alloy) + some hidden windows
            if (dwStyle & WS_CAPTION) {
                // Subclass other Chromium/CEF windows, including those not created by cef_window_create_top_level (e.g. DevTools, Miniplayer (DocumentPictureInPicture), full Chromium browser UI that somehow can be opened, etc.)
                // But exclude windows without WS_CAPTION to prevent subclassing dropdowns, tooltips, etc.
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 0)) {
                    Wh_Log(L"Subclassed %p", hWnd);
                }
            }
        }
    }
    return hWnd;
}

using SetWindowThemeAttribute_t = decltype(&SetWindowThemeAttribute);
SetWindowThemeAttribute_t SetWindowThemeAttribute_original;
HRESULT WINAPI SetWindowThemeAttribute_hook(HWND hwnd, enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute) {
    Wh_Log(L"SetWindowThemeAttribute_hook");
    if (eAttribute == WTA_NONCLIENT && is_frameless_offset != NULL && cte_settings.showframe == TRUE) {
        // Ignore this to make sure DWM window controls are visible
        return S_OK;
    } else {
        return SetWindowThemeAttribute_original(hwnd, eAttribute, pvAttribute, cbAttribute);
    }
}

using SetWindowTextW_t = decltype(&SetWindowTextW);
SetWindowTextW_t SetWindowTextW_original;
BOOL WINAPI SetWindowTextW_hook(HWND hWnd, LPCWSTR lpString) {
    Wh_Log(L"SetWindowTextW_hook");
    if (g_titleLocked && hWnd == g_mainHwnd) {
        return TRUE;
    }
    return SetWindowTextW_original(hWnd, lpString);
}

// renderer and gpu process spawn with this when --no-sandbox is used
using CreateProcessW_t = decltype(&CreateProcessW);
CreateProcessW_t CreateProcessW_original;
BOOL WINAPI CreateProcessW_hook(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    Wh_Log(L"CreateProcessW_hook");

    BOOL result = CreateProcessW_original(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation
    );

    if (result && lpCommandLine && wcsstr(lpCommandLine, L"--type=gpu-process")) {
        g_hwAccelerated = TRUE;
        Wh_Log(L"GPU process detected, hardware acceleration enabled");
    }
    Wh_Log(L"lpCommandLine: %s", lpCommandLine);

    return result;
}

// renderer and gpu process spawn with this when sandbox is enabled
using CreateProcessAsUserW_t = decltype(&CreateProcessAsUserW);
CreateProcessAsUserW_t CreateProcessAsUserW_original;
BOOL WINAPI CreateProcessAsUserW_hook(
    HANDLE hToken,
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    Wh_Log(L"CreateProcessAsUserW_hook");

    // Inject %PROGRAMDATA% to get Windhawk 1.5.1 and below to work with sandboxed renderers
    const wchar_t* lpEnvironmentCopy = (wchar_t*)lpEnvironment;
    if (lpCommandLine && wcsstr(lpCommandLine, L"--type=renderer")) {
        if (lpEnvironment) {
            const wchar_t* env = (const wchar_t*)lpEnvironment;
            BOOL found = FALSE;
            while (*env) {
                if (wcsstr(env, L"PROGRAMDATA") != NULL) {
                    found = TRUE;
                    break;
                }
                env += wcslen(env) + 1;
            }

            if (!found) {
                size_t envLen = 0;
                while (((wchar_t*)lpEnvironment)[envLen] != L'\0' || ((wchar_t*)lpEnvironment)[envLen + 1] != L'\0') {
                    envLen++;
                }
                envLen += 1; // Only include one null terminator at the end

                std::wstring append1 = L"PROGRAMDATA=";
                std::wstring append2(MAX_PATH, L'\0');
                GetEnvironmentVariableW(L"PROGRAMDATA", append2.data(), MAX_PATH);
                std::wstring append3 = L"\0\0";
                std::wstring append = append1 + append2 + append3;
                size_t appendLen = append.size();

                wchar_t* newEnv = new wchar_t[envLen + appendLen];
                memcpy(newEnv, lpEnvironment, envLen * sizeof(wchar_t));
                memcpy(newEnv + envLen, append.c_str(), appendLen * sizeof(wchar_t));

                lpEnvironmentCopy = newEnv;
            }
        }
    }

    BOOL result = CreateProcessAsUserW_original(
        hToken,
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        (LPVOID)lpEnvironmentCopy,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation
    );

    if (result && lpCommandLine && wcsstr(lpCommandLine, L"--type=gpu-process")) {
        g_hwAccelerated = TRUE;
        Wh_Log(L"GPU process detected, hardware acceleration enabled");
    }
    Wh_Log(L"lpCommandLine: %s", lpCommandLine);

    return result;
}
#pragma endregion

#pragma region Renderer JS API injection + IPC
void HandleWindhawkComm(LPCWSTR command) {
    if (g_mainHwnd == NULL) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_ipcMutex); // Protect shared resources

    int len = wcslen(command);
    Wh_Log(L"HandleWindhawkComm: %s, len: %d, size: %d", command, len, sizeof(command));

    // Validate command length
    if (len >= 269) { // "/WH:SetTitle:" + max title length (255) + null terminator
        Wh_Log(L"Command too long");
        return;
    }

    // /WH:ExtendFrame:<left>:<right>:<top>:<bottom>
    // Set DWM margins to extend frame into client area
    if (wcsncmp(command, L"/WH:ExtendFrame:", 16) == 0) {
        if (!IsDwmEnabled()) {
            return;
        }
        int left, right, top, bottom;
        if (swscanf(command + 16, L"%d:%d:%d:%d", &left, &right, &top, &bottom) == 4) {
            MARGINS margins = {left, right, top, bottom};
            DwmExtendFrameIntoClientArea(g_mainHwnd, &margins);
        }
    // /WH:Minimize, /WH:MaximizeRestore, /WH:Close
    // Send respective window messages to the main window
    } else if (wcscmp(command, L"/WH:Minimize") == 0) {
        ShowWindow(g_mainHwnd, SW_MINIMIZE);
    } else if (wcscmp(command, L"/WH:MaximizeRestore") == 0) {
        if (IsIconic(g_mainHwnd)) {
            ShowWindow(g_mainHwnd, SW_RESTORE);
        } else {
            ShowWindow(g_mainHwnd, IsZoomed(g_mainHwnd) ? SW_RESTORE : SW_MAXIMIZE);
        }
    } else if (wcscmp(command, L"/WH:Close") == 0) {
        PostMessage(g_mainHwnd, WM_CLOSE, 0, 0);
    // /WH:Focus
    // Set focus to the main window
    } else if (wcscmp(command, L"/WH:Focus") == 0) {
        SetForegroundWindow(g_mainHwnd);
    // /WH:SetLayered:<layered (1/0)>:<alpha>:<optional-transparentColor>
    // Make the window layered with optional transparent color key
    } else if (wcsncmp(command, L"/WH:SetLayered:", 15) == 0) {
        int layered, alpha, color;
        if (swscanf(command + 15, L"%d:%d:%x", &layered, &alpha, &color) == 3) {
            if (layered) {
                SetWindowLong(g_mainHwnd, GWL_EXSTYLE, GetWindowLong(g_mainHwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes(g_mainHwnd, color, alpha, LWA_COLORKEY | LWA_ALPHA);
            } else {
                SetWindowLong(g_mainHwnd, GWL_EXSTYLE, GetWindowLong(g_mainHwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
            }
        } else if (swscanf(command + 15, L"%d:%d", &layered, &alpha) == 2) {
            if (layered) {
                SetWindowLong(g_mainHwnd, GWL_EXSTYLE, GetWindowLong(g_mainHwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes(g_mainHwnd, 0, alpha, LWA_ALPHA);
            } else {
                SetWindowLong(g_mainHwnd, GWL_EXSTYLE, GetWindowLong(g_mainHwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
            }
        } else if (swscanf(command + 15, L"%d", &layered) == 1) {
            if (layered) {
                SetWindowLong(g_mainHwnd, GWL_EXSTYLE, GetWindowLong(g_mainHwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            } else {
                SetWindowLong(g_mainHwnd, GWL_EXSTYLE, GetWindowLong(g_mainHwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
            }
        }
    // /WH:SetBackdrop:<mica|acrylic|tabbed>
    // Set the window backdrop type (Windows 11 only)
    } else if (wcsncmp(command, L"/WH:SetBackdrop:", 16) == 0) {
        if (wcscmp(command + 16, L"mica") == 0) {
            BOOL value = TRUE;
            DwmSetWindowAttribute(g_mainHwnd, DWMWA_USE_HOSTBACKDROPBRUSH, &value, sizeof(value));
            int backdrop_type = DWMSBT_MAINWINDOW;
            DwmSetWindowAttribute(g_mainHwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop_type, sizeof(backdrop_type));
            g_dwmBackdropEnabled = TRUE;
        } else if (wcscmp(command + 16, L"acrylic") == 0) {
            BOOL value = TRUE;
            DwmSetWindowAttribute(g_mainHwnd, DWMWA_USE_HOSTBACKDROPBRUSH, &value, sizeof(value));
            int backdrop_type = DWMSBT_TRANSIENTWINDOW;
            DwmSetWindowAttribute(g_mainHwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop_type, sizeof(backdrop_type));
            g_dwmBackdropEnabled = TRUE;
        } else if (wcscmp(command + 16, L"tabbed") == 0) {
            BOOL value = TRUE;
            DwmSetWindowAttribute(g_mainHwnd, DWMWA_USE_HOSTBACKDROPBRUSH, &value, sizeof(value));
            int backdrop_type = DWMSBT_TABBEDWINDOW;
            DwmSetWindowAttribute(g_mainHwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop_type, sizeof(backdrop_type));
            g_dwmBackdropEnabled = TRUE;
        }
    // /WH:ResizeTo:<width>:<height>
    } else if (wcsncmp(command, L"/WH:ResizeTo:", 13) == 0) {
        int width, height;
        if (swscanf(command + 13, L"%d:%d", &width, &height) == 2) {
            SetWindowPos(g_mainHwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
        }
    // /WH:SetMinSize:<width>:<height>
    } else if (wcsncmp(command, L"/WH:SetMinSize:", 15) == 0) {
        int width, height;
        if (swscanf(command + 15, L"%d:%d", &width, &height) == 2) {
            g_minWidth = width;
            g_minHeight = height;
        }
    // /WH:SetTopMost:<topmost (1/0), toggle if absent>
    } else if (wcsncmp(command, L"/WH:SetTopMost:", 15) == 0) {
        int topmost;
        if (swscanf(command + 15, L"%d", &topmost) == 1) {
            SetWindowPos(g_mainHwnd, topmost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        } else {
            SetWindowPos(g_mainHwnd, GetWindowLong(g_mainHwnd, GWL_EXSTYLE) & WS_EX_TOPMOST ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    // /WH:SetTitle:<title>
    // Set the window title (ignores title lock)
    } else if (wcsncmp(command, L"/WH:SetTitle:", 13) == 0) {
        SetWindowTextW_original(g_mainHwnd, command + 13);
    // /WH:LockTitle:<lock (1/0), toggle if absent>
    // Prevent Spotify from changing the window title
    } else if (wcsncmp(command, L"/WH:LockTitle:", 14) == 0) {
        int lock;
        if (swscanf(command + 14, L"%d", &lock) == 1) {
            g_titleLocked = lock;
        } else {
            g_titleLocked = !g_titleLocked;
        }
    // /WH:OpenSpotifyMenu
    // Open the Spotify menu (Alt, won't work if the menu button is hidden in the mod options)
    } else if (wcscmp(command, L"/WH:OpenSpotifyMenu") == 0) {
        PostMessage(g_mainHwnd, WM_SYSCOMMAND, SC_KEYMENU, 0);
    // /WH:SetPlaybackSpeed:<speed> (64-bit only)
    #ifdef _WIN64
    } else if (wcsncmp(command, L"/WH:SetPlaybackSpeed:", 21) == 0) {
        double speed;
        if (swscanf(command + 21, L"%lf", &speed) == 1) {
            g_playbackSpeed = speed;
            if (SetPlaybackSpeed != NULL && g_currentTrackPlayer != NULL) {
                SetPlaybackSpeed(g_currentTrackPlayer, speed);
            }
        }
    #endif
    // /WH:Query
    } else if (wcscmp(command, L"/WH:Query") == 0) {
        if (g_hPipe == INVALID_HANDLE_VALUE) {
            return;
        }
        wchar_t queryResponse[256];
        // <showframe:showframeonothers:showmenu:showcontrols:transparentcontrols:transparentrendering:ignoreminsize:isMaximized:isTopMost:isLayered:isThemingEnabled:isDwmEnabled:dwmBackdropEnabled:hwAccelerated:minWidth:minHeight:titleLocked:dpi:speedModSupported:playbackSpeed:immediateSpeedChange>
        swprintf(queryResponse, 256, L"/WH:QueryResponse:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%lf:%d",
            cte_settings.showframe,
            cte_settings.showframeonothers,
            cte_settings.showmenu,
            cte_settings.showcontrols,
            cte_settings.transparentcontrols,
            cte_settings.transparentrendering,
            cte_settings.ignoreminsize,
            IsZoomed(g_mainHwnd),
            GetWindowLong(g_mainHwnd, GWL_EXSTYLE) & WS_EX_TOPMOST,
            GetWindowLong(g_mainHwnd, GWL_EXSTYLE) & WS_EX_LAYERED,
            IsAppThemed() && IsThemeActive(),
            IsDwmEnabled(),
            g_dwmBackdropEnabled,
            g_hwAccelerated,
            g_minWidth,
            g_minHeight,
            g_titleLocked,
            GetDpiForWindow(g_mainHwnd),
            CreateTrackPlayer_original != NULL,
            g_playbackSpeed,
            SetPlaybackSpeed != NULL
        );
        DWORD bytesWritten;
        WriteFile(g_hPipe, queryResponse, wcslen(queryResponse) * sizeof(wchar_t), &bytesWritten, NULL);
    }
}

// Copy-pasted from https://source.chromium.org/chromium/chromium/src/+/main:third_party/crashpad/crashpad/util/win/registration_protocol_win.cc;drc=f39c57f31413abcb41d3068cfb2c7a1718003cc5;l=253
// Same logic as in crashpad to allow processes with untrusted integrity level to connect to the named pipe
void* GetSecurityDescriptorWithUser(const wchar_t* sddl_string, size_t* size) {
    if (size)
        *size = 0;

    PSECURITY_DESCRIPTOR base_sec_desc;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptor(
            sddl_string, SDDL_REVISION_1, &base_sec_desc, nullptr)) {
        Wh_Log(L"ConvertStringSecurityDescriptorToSecurityDescriptor failed");
        return nullptr;
    }

    EXPLICIT_ACCESS access;
    wchar_t username[] = L"CURRENT_USER";
    BuildExplicitAccessWithName(
        &access, username, GENERIC_ALL, GRANT_ACCESS, NO_INHERITANCE);

    PSECURITY_DESCRIPTOR user_sec_desc;
    ULONG user_sec_desc_size;
    DWORD error = BuildSecurityDescriptor(nullptr,
                                          nullptr,
                                          1,
                                          &access,
                                          0,
                                          nullptr,
                                          base_sec_desc,
                                          &user_sec_desc_size,
                                          &user_sec_desc);
    if (error != ERROR_SUCCESS) {
        SetLastError(error);
        Wh_Log(L"BuildSecurityDescriptor failed");
        return nullptr;
    }

    *size = user_sec_desc_size;
    return user_sec_desc;
}

const void* GetSecurityDescriptorForNamedPipeInstance(size_t* size) {
    // Get a security descriptor which grants the current user and SYSTEM full
    // access to the named pipe. Also grant AppContainer RW access through the ALL
    // APPLICATION PACKAGES SID (S-1-15-2-1). Finally add an Untrusted Mandatory
    // Label for non-AppContainer sandboxed users.
    static size_t sd_size;
    static void* sec_desc = GetSecurityDescriptorWithUser(
        L"D:(A;;GA;;;SY)(A;;GWGR;;;S-1-15-2-1)(A;;GA;;;LW)S:(ML;;;;;S-1-16-0)", &sd_size);

    if (size)
        *size = sd_size;
    return sec_desc;
}

void CreateNamedPipeServer() {
    void* pSecurityDescriptor = const_cast<void*>(GetSecurityDescriptorForNamedPipeInstance(NULL));
    if (!pSecurityDescriptor) {
        Wh_Log(L"GetSecurityDescriptorForNamedPipeInstance failed. Pipe won't be accessible to sandboxed CEF renderers.");
    }

    SECURITY_ATTRIBUTES securityAttributes = {};
    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    securityAttributes.lpSecurityDescriptor = pSecurityDescriptor;
    securityAttributes.bInheritHandle = TRUE;

    while (!g_shouldClosePipe) {
        g_hPipe = CreateNamedPipe(
            PIPE_NAME,                                       // Pipe name
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,       // Read/Write access with overlapped I/O
            PIPE_TYPE_MESSAGE |                              // Message type pipe
            PIPE_READMODE_MESSAGE |                          // Message-read mode
            PIPE_WAIT,                                       // Blocking mode
            PIPE_UNLIMITED_INSTANCES,                        // Max instances
            512,                                             // Output buffer size
            512,                                             // Input buffer size
            0,                                               // Client time-out
            pSecurityDescriptor ? &securityAttributes : NULL // Security attributes
        );

        if (g_hPipe == INVALID_HANDLE_VALUE) {
            Wh_Log(L"CreateNamedPipe failed, GLE=%d", GetLastError());
            return;
        }

        Wh_Log(L"Waiting for client to connect...");
        BOOL connected = ConnectNamedPipe(g_hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (connected) {
            Wh_Log(L"Client connected, waiting for message...");
            wchar_t buffer[512];
            DWORD bytesRead;
            while (!g_shouldClosePipe) {
                BOOL result = ReadFile(g_hPipe, buffer, sizeof(buffer) - sizeof(wchar_t), &bytesRead, NULL);
                if (result) {
                    if (bytesRead >= sizeof(buffer) - sizeof(wchar_t)) {
                        Wh_Log(L"Buffer overflow detected");
                        continue;
                    }
                    buffer[bytesRead / sizeof(wchar_t)] = L'\0';
                    Wh_Log(L"Received message: %s", buffer);
                    HandleWindhawkComm(buffer);
                } else {
                    DWORD error = GetLastError();
                    if (error == ERROR_BROKEN_PIPE || error == ERROR_PIPE_NOT_CONNECTED) {
                        Wh_Log(L"Client disconnected, GLE=%d", error);
                        break;
                    }
                }
            }
        } else {
            Wh_Log(L"ConnectNamedPipe failed, GLE=%d", GetLastError());
        }

        Wh_Log(L"Closing pipe...");
        CloseHandle(g_hPipe);
        g_hPipe = INVALID_HANDLE_VALUE;
    }

    LocalFree(pSecurityDescriptor);
}

int ConnectToNamedPipe() {
    g_hPipe = CreateFile(
        PIPE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (g_hPipe == INVALID_HANDLE_VALUE) {
        int gle = GetLastError();
        Wh_Log(L"CreateFile failed, GLE=%d", gle);
        return gle;
    }

    g_pipeThread = std::thread([]() {
        wchar_t buffer[512];
        DWORD bytesRead;
        OVERLAPPED overlapped = {};
        overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (!overlapped.hEvent) {
            Wh_Log(L"CreateEvent failed, GLE=%d", GetLastError());
            return;
        }

        while (!g_shouldClosePipe) {
            BOOL result = ReadFile(g_hPipe, buffer, sizeof(buffer) - sizeof(wchar_t), &bytesRead, &overlapped);
            if (!result && GetLastError() == ERROR_IO_PENDING) {
                DWORD waitResult = WaitForSingleObject(overlapped.hEvent, INFINITE);
                if (waitResult == WAIT_OBJECT_0) {
                    if (GetOverlappedResult(g_hPipe, &overlapped, &bytesRead, FALSE)) {
                        buffer[bytesRead / sizeof(wchar_t)] = L'\0';
                        Wh_Log(L"Received message: %s", buffer);
                        if (wcsncmp(buffer, L"/WH:QueryResponse:", 18) == 0) {
                            if (swscanf(buffer + 18, L"%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%lf:%d",
                                &cte_settings.showframe,
                                &cte_settings.showframeonothers,
                                &cte_settings.showmenu,
                                &cte_settings.showcontrols,
                                &cte_settings.transparentcontrols,
                                &cte_settings.transparentrendering,
                                &cte_settings.ignoreminsize,
                                &g_queryResponse.isMaximized,
                                &g_queryResponse.isTopMost,
                                &g_queryResponse.isLayered,
                                &g_queryResponse.isThemingEnabled,
                                &g_queryResponse.isDwmEnabled,
                                &g_queryResponse.dwmBackdropEnabled,
                                &g_queryResponse.hwAccelerated,
                                &g_queryResponse.minWidth,
                                &g_queryResponse.minHeight,
                                &g_queryResponse.titleLocked,
                                &g_queryResponse.dpi,
                                &g_queryResponse.speedModSupported,
                                &g_queryResponse.playbackSpeed,
                                &g_queryResponse.immediateSpeedChange) == 21
                            ) {
                                g_queryResponse.success = TRUE;
                            }
                            // Notify the condition variable
                            {
                                std::lock_guard<std::mutex> lock(g_ipcMutex);
                                g_queryResponseReceived = true;
                            }
                            g_queryResponseCv.notify_one();
                        }
                    } else {
                        DWORD error = GetLastError();
                        if (error == ERROR_BROKEN_PIPE || error == ERROR_PIPE_NOT_CONNECTED) {
                            Wh_Log(L"Server disconnected, GLE=%d", error);
                            break;
                        }
                    }
                }
            }
        }
        CloseHandle(overlapped.hEvent);
        CloseHandle(g_hPipe);
        g_hPipe = INVALID_HANDLE_VALUE;
    });
    g_pipeThread.detach();

    return 0;
}

int SendNamedPipeMessage(LPCWSTR message) {
    if (g_hPipe == INVALID_HANDLE_VALUE) {
        Wh_Log(L"SendNamedPipeMessage failed: pipe is not connected");
        return ERROR_PIPE_NOT_CONNECTED;
    }

    DWORD bytesWritten;
    size_t messageLength = wcslen(message) * sizeof(wchar_t);
    Wh_Log(L"Sending message: %s", message);

    OVERLAPPED overlapped = {};
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!overlapped.hEvent) {
        int gle = GetLastError();
        Wh_Log(L"CreateEvent failed, GLE=%d", gle);
        return gle;
    }

    BOOL result = WriteFile(g_hPipe, message, messageLength, &bytesWritten, &overlapped);
    if (!result && GetLastError() != ERROR_IO_PENDING) {
        int gle = GetLastError();
        Wh_Log(L"WriteFile failed, GLE=%d", gle);
        CloseHandle(overlapped.hEvent);
        return gle;
    }

    // Wait for the write operation to complete
    DWORD waitResult = WaitForSingleObject(overlapped.hEvent, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        int gle = GetLastError();
        Wh_Log(L"WaitForSingleObject failed, GLE=%d", gle);
        CloseHandle(overlapped.hEvent);
        return gle;
    }

    // Check the result of the write operation
    if (!GetOverlappedResult(g_hPipe, &overlapped, &bytesWritten, FALSE)) {
        int gle = GetLastError();
        Wh_Log(L"GetOverlappedResult failed, GLE=%d", gle);
        CloseHandle(overlapped.hEvent);
        return gle;
    }

    Wh_Log(L"Message sent successfully");
    CloseHandle(overlapped.hEvent);

    if (wcsncmp(message, L"/WH:Query", 9) == 0) {
        // Wait for the query response
        std::unique_lock<std::mutex> lock(g_ipcMutex);
        g_queryResponseCv.wait(lock, [] { return g_queryResponseReceived; });
        g_queryResponseReceived = false;
    }

    return 0;
}

int CEF_CALLBACK WindhawkCommV8Handler(cef_v8handler_t* self, const cef_string_t* name, cef_v8value_t* object, size_t argumentsCount, cef_v8value_t* const* arguments, cef_v8value_t** retval, cef_string_t* exception) {
    Wh_Log(L"WindhawkCommV8Handler called with name: %s", name->str);
    std::u16string nameStr(name->str, name->length);
    if (g_hPipe == INVALID_HANDLE_VALUE) {
        cef_string_t* msg = GenerateCefString(u"Disconnected from the Windhawk mod running in the main process. Is the mod unloaded?");
        *exception = *msg;
        free(msg->str);
        free(msg);
        return TRUE;
    }

    int ipcRes = -1;
    if (nameStr == u"extendFrame") {
        if (argumentsCount == 4 && arguments[0]->is_int(arguments[0]) && arguments[1]->is_int(arguments[1]) && arguments[2]->is_int(arguments[2]) && arguments[3]->is_int(arguments[3])) {
            int left = arguments[0]->get_int_value(arguments[0]);
            int right = arguments[1]->get_int_value(arguments[1]);
            int top = arguments[2]->get_int_value(arguments[2]);
            int bottom = arguments[3]->get_int_value(arguments[3]);
            ipcRes = SendNamedPipeMessage((L"/WH:ExtendFrame:" + std::to_wstring(left) + L":" + std::to_wstring(right) + L":" + std::to_wstring(top) + L":" + std::to_wstring(bottom)).c_str());
        } else {
            cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (int, int, int, int)");
            *exception = *msg;
            free(msg->str);
            free(msg);
            return TRUE;
        }
    } else if (nameStr == u"minimize") {
        ipcRes = SendNamedPipeMessage(L"/WH:Minimize");
    } else if (nameStr == u"maximizeRestore") {
        ipcRes = SendNamedPipeMessage(L"/WH:MaximizeRestore");
    } else if (nameStr == u"close") {
        ipcRes = SendNamedPipeMessage(L"/WH:Close");
    } else if (nameStr == u"focus") {
        ipcRes = SendNamedPipeMessage(L"/WH:Focus");
    } else if (nameStr == u"setLayered") {
        if (argumentsCount >= 1 && arguments[0]->is_bool(arguments[0])) {
            bool layered = arguments[0]->get_bool_value(arguments[0]);
            if (argumentsCount >= 2) {
                if (!arguments[1]->is_int(arguments[1])) {
                    cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (bool, optional int, optional string)");
                    *exception = *msg;
                    free(msg->str);
                    free(msg);
                    return TRUE;
                }
                int alpha = arguments[1]->get_int_value(arguments[1]);
                if (argumentsCount >= 3) {
                    if (!arguments[2]->is_string(arguments[2])) {
                        cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (bool, optional int, optional string)");
                        *exception = *msg;
                        free(msg->str);
                        free(msg);
                        return TRUE;
                    }
                    cef_string_t* colorArg = arguments[2]->get_string_value(arguments[2]);
                    std::wstring colorStr(colorArg->str, colorArg->str + colorArg->length);
                    int color = 0; // only for format validation
                    if (swscanf(colorStr.c_str(), L"%x", &color) == 1) {
                        ipcRes = SendNamedPipeMessage((L"/WH:SetLayered:" + std::to_wstring(layered) + L":" + std::to_wstring(alpha) + L":" + colorStr).c_str());
                    } else {
                        cef_string_t* msg = GenerateCefString(u"Invalid color format, expected six-digit hex string");
                        *exception = *msg;
                        free(msg->str);
                        free(msg);
                        return TRUE;
                    }
                } else {
                    ipcRes = SendNamedPipeMessage((L"/WH:SetLayered:" + std::to_wstring(layered) + L":" + std::to_wstring(alpha)).c_str());
                }
            } else {
                ipcRes = SendNamedPipeMessage((L"/WH:SetLayered:" + std::to_wstring(layered)).c_str());
            }
        } else {
            cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (bool, optional int, optional string)");
            *exception = *msg;
            free(msg->str);
            free(msg);
            return TRUE;
        }
    } else if (nameStr == u"setBackdrop") {
        if (argumentsCount == 1 && arguments[0]->is_string(arguments[0])) {
            cef_string_t* backdropArg = arguments[0]->get_string_value(arguments[0]);
            std::wstring backdropStr(backdropArg->str, backdropArg->str + backdropArg->length);
            if (backdropStr == L"mica" || backdropStr == L"acrylic" || backdropStr == L"tabbed") {
                ipcRes = SendNamedPipeMessage((L"/WH:SetBackdrop:" + backdropStr).c_str());
            } else {
                cef_string_t* msg = GenerateCefString(u"Invalid backdrop type, expected 'mica', 'acrylic', or 'tabbed'");
                *exception = *msg;
                free(msg->str);
                free(msg);
                return TRUE;
            }
        } else {
            cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (string)");
            *exception = *msg;
            free(msg->str);
            free(msg);
            return TRUE;
        }
    } else if (nameStr == u"resizeTo") {
        if (argumentsCount == 2 && arguments[0]->is_int(arguments[0]) && arguments[1]->is_int(arguments[1])) {
            int width = arguments[0]->get_int_value(arguments[0]);
            int height = arguments[1]->get_int_value(arguments[1]);
            ipcRes = SendNamedPipeMessage((L"/WH:ResizeTo:" + std::to_wstring(width) + L":" + std::to_wstring(height)).c_str());
        } else {
            cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (int, int)");
            *exception = *msg;
            free(msg->str);
            free(msg);
            return TRUE;
        }
    } else if (nameStr == u"setMinSize") {
        if (argumentsCount == 2 && arguments[0]->is_int(arguments[0]) && arguments[1]->is_int(arguments[1])) {
            int width = arguments[0]->get_int_value(arguments[0]);
            int height = arguments[1]->get_int_value(arguments[1]);
            ipcRes = SendNamedPipeMessage((L"/WH:SetMinSize:" + std::to_wstring(width) + L":" + std::to_wstring(height)).c_str());
        } else {
            cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (int, int)");
            *exception = *msg;
            free(msg->str);
            free(msg);
            return TRUE;
        }
    } else if (nameStr == u"setTopMost") {
        if (argumentsCount >= 1 && arguments[0]->is_bool(arguments[0])) {
            bool topmost = arguments[0]->get_bool_value(arguments[0]);
            ipcRes = SendNamedPipeMessage((L"/WH:SetTopMost:" + std::to_wstring(topmost)).c_str());
        } else {
            cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (bool)");
            *exception = *msg;
            free(msg->str);
            free(msg);
            return TRUE;
        }
    } else if (nameStr == u"setTitle") {
        if (argumentsCount == 1 && arguments[0]->is_string(arguments[0])) {
            cef_string_t* titleArg = arguments[0]->get_string_value(arguments[0]);
            std::wstring titleStr(titleArg->str, titleArg->str + titleArg->length);
            if (titleStr.length() > 255) {
                cef_string_t* msg = GenerateCefString(u"Title is too long, max is 255 characters");
                *exception = *msg;
                free(msg->str);
                free(msg);
                return TRUE;
            }
            ipcRes = SendNamedPipeMessage((L"/WH:SetTitle:" + titleStr).c_str());
        } else {
            cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (string)");
            *exception = *msg;
            free(msg->str);
            free(msg);
            return TRUE;
        }
    } else if (nameStr == u"lockTitle") {
        if (argumentsCount >= 1 && arguments[0]->is_bool(arguments[0])) {
            bool lock = arguments[0]->get_bool_value(arguments[0]);
            ipcRes = SendNamedPipeMessage((L"/WH:LockTitle:" + std::to_wstring(lock)).c_str());
        } else {
            cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (bool)");
            *exception = *msg;
            free(msg->str);
            free(msg);
            return TRUE;
        }
    } else if (nameStr == u"openSpotifyMenu") {
        ipcRes = SendNamedPipeMessage(L"/WH:OpenSpotifyMenu");
    #ifdef _WIN64
    } else if (nameStr == u"setPlaybackSpeed") {
        if (argumentsCount == 1 && arguments[0]->is_double(arguments[0])) {
            double speed = arguments[0]->get_double_value(arguments[0]);
            if (speed <= 0 || speed > 5.0) {
                // Prevent potential ban risk by limiting the playback speed
                // Note: setting to zero or negative values will be ignored by SetPlaybackSpeed and causes crashes in CreateTrackPlayer
                cef_string_t* msg = GenerateCefString(u"Playback speed must be faster than 0 and less than or equal to 5.0");
                *exception = *msg;
                free(msg->str);
                free(msg);
                return TRUE;
            }
            ipcRes = SendNamedPipeMessage((L"/WH:SetPlaybackSpeed:" + std::to_wstring(speed)).c_str());
        } else {
            cef_string_t* msg = GenerateCefString(u"Invalid argument types, expected (double)");
            *exception = *msg;
            free(msg->str);
            free(msg);
            return TRUE;
        }
    #endif
    } else if (nameStr == u"query") {
        ipcRes = SendNamedPipeMessage(L"/WH:Query");
        if (g_queryResponse.success) {
            cef_v8value_t* retobj = cef_v8value_create_object(NULL, NULL);
            cef_v8value_t* configObj = cef_v8value_create_object(NULL, NULL);
            AddValueToObj(configObj, u"showframe", cef_v8value_create_bool(cte_settings.showframe));
            AddValueToObj(configObj, u"showframeonothers", cef_v8value_create_bool(cte_settings.showframeonothers));
            AddValueToObj(configObj, u"showmenu", cef_v8value_create_bool(cte_settings.showmenu));
            AddValueToObj(configObj, u"showcontrols", cef_v8value_create_bool(cte_settings.showcontrols));
            AddValueToObj(configObj, u"transparentcontrols", cef_v8value_create_bool(cte_settings.transparentcontrols));
            AddValueToObj(configObj, u"transparentrendering", cef_v8value_create_bool(cte_settings.transparentrendering));
            AddValueToObj(configObj, u"ignoreminsize", cef_v8value_create_bool(cte_settings.ignoreminsize));
            AddValueToObj(configObj, u"noforceddarkmode", cef_v8value_create_bool(cte_settings.noforceddarkmode));
            AddValueToObj(configObj, u"forceextensions", cef_v8value_create_bool(cte_settings.forceextensions));
            AddValueToObj(configObj, u"allowuntested", cef_v8value_create_bool(cte_settings.allowuntested));
            AddValueToObj(retobj, u"options", configObj);
            AddValueToObj(retobj, u"isMaximized", cef_v8value_create_bool(g_queryResponse.isMaximized));
            AddValueToObj(retobj, u"isTopMost", cef_v8value_create_bool(g_queryResponse.isTopMost));
            AddValueToObj(retobj, u"isLayered", cef_v8value_create_bool(g_queryResponse.isLayered));
            AddValueToObj(retobj, u"isThemingEnabled", cef_v8value_create_bool(g_queryResponse.isThemingEnabled));
            AddValueToObj(retobj, u"isDwmEnabled", cef_v8value_create_bool(g_queryResponse.isDwmEnabled));
            AddValueToObj(retobj, u"dwmBackdropEnabled", cef_v8value_create_bool(g_queryResponse.dwmBackdropEnabled));
            AddValueToObj(retobj, u"hwAccelerated", cef_v8value_create_bool(g_queryResponse.hwAccelerated));
            AddValueToObj(retobj, u"minWidth", cef_v8value_create_int(g_queryResponse.minWidth));
            AddValueToObj(retobj, u"minHeight", cef_v8value_create_int(g_queryResponse.minHeight));
            AddValueToObj(retobj, u"titleLocked", cef_v8value_create_bool(g_queryResponse.titleLocked));
            AddValueToObj(retobj, u"dpi", cef_v8value_create_int(g_queryResponse.dpi));
            AddValueToObj(retobj, u"speedModSupported", cef_v8value_create_bool(g_queryResponse.speedModSupported));
            #ifdef _WIN64
            AddValueToObj(retobj, u"playbackSpeed", cef_v8value_create_double(g_queryResponse.playbackSpeed));
            AddValueToObj(retobj, u"immediateSpeedChange", cef_v8value_create_bool(g_queryResponse.immediateSpeedChange));
            #endif
            *retval = retobj;
            g_queryResponse.success = FALSE;
        } else {
            cef_string_t* msg = GenerateCefString(u"Error: Query response not received");
            *exception = *msg;
            free(msg->str);
            free(msg);
            return TRUE;
        }
    }

    if (ipcRes != 0) {
        cef_string_t* msg = FormatCefString(L"IPC Error: %d", ipcRes);
        *exception = *msg;
        free(msg->str);
        free(msg);
    }
    return TRUE;
}

typedef int CEF_CALLBACK (*v8func_exec_t)(cef_v8handler_t* self, const cef_string_t* name, cef_v8value_t* object, size_t argumentsCount, cef_v8value_t* const* arguments, cef_v8value_t** retval, cef_string_t* exception);
v8func_exec_t CEF_CALLBACK cancelEsperantoCall_original;
int CEF_CALLBACK cancelEsperantoCall_hook(cef_v8handler_t* self, const cef_string_t* name, cef_v8value_t* object, size_t argumentsCount, cef_v8value_t* const* arguments, cef_v8value_t** retval, cef_string_t* exception) {
    Wh_Log(L"cancelEsperantoCall_hook called with name: %s", name->str);
    if (argumentsCount == 1) {
        cef_string_t* arg = arguments[0]->get_string_value(arguments[0]); // NULL when it's an empty string
        if (arg != NULL && u"ctewh" == std::u16string(arg->str, arg->length)) {
            Wh_Log(L"CTEWH is being requested");
            cef_v8handler_t* ctewh = (cef_v8handler_t*)calloc(1, sizeof(cef_v8handler_t));
            ctewh->base.size = sizeof(cef_v8handler_t);
            ctewh->execute = WindhawkCommV8Handler;
            cef_v8value_t* retobj = cef_v8value_create_object(NULL, NULL);
            AddFunctionToObj(retobj, u"query", ctewh);
            AddFunctionToObj(retobj, u"extendFrame", ctewh);
            AddFunctionToObj(retobj, u"minimize", ctewh);
            AddFunctionToObj(retobj, u"maximizeRestore", ctewh);
            AddFunctionToObj(retobj, u"close", ctewh);
            AddFunctionToObj(retobj, u"focus", ctewh);
            AddFunctionToObj(retobj, u"setLayered", ctewh);
            AddFunctionToObj(retobj, u"setBackdrop", ctewh);
            AddFunctionToObj(retobj, u"resizeTo", ctewh);
            AddFunctionToObj(retobj, u"setMinSize", ctewh);
            AddFunctionToObj(retobj, u"setTopMost", ctewh);
            AddFunctionToObj(retobj, u"setTitle", ctewh);
            AddFunctionToObj(retobj, u"lockTitle", ctewh);
            AddFunctionToObj(retobj, u"openSpotifyMenu", ctewh);
            #ifdef _WIN64
            AddFunctionToObj(retobj, u"setPlaybackSpeed", ctewh);
            #endif
            cef_v8value_t* initialConfigObj = cef_v8value_create_object(NULL, NULL);
            AddValueToObj(initialConfigObj, u"showframe", cef_v8value_create_bool(cte_settings.showframe));
            AddValueToObj(initialConfigObj, u"showframeonothers", cef_v8value_create_bool(cte_settings.showframeonothers));
            AddValueToObj(initialConfigObj, u"showmenu", cef_v8value_create_bool(cte_settings.showmenu));
            AddValueToObj(initialConfigObj, u"showcontrols", cef_v8value_create_bool(cte_settings.showcontrols));
            AddValueToObj(initialConfigObj, u"transparentcontrols", cef_v8value_create_bool(cte_settings.transparentcontrols));
            AddValueToObj(initialConfigObj, u"transparentrendering", cef_v8value_create_bool(cte_settings.transparentrendering));
            AddValueToObj(initialConfigObj, u"ignoreminsize", cef_v8value_create_bool(cte_settings.ignoreminsize));
            AddValueToObj(initialConfigObj, u"noforceddarkmode", cef_v8value_create_bool(cte_settings.noforceddarkmode));
            AddValueToObj(initialConfigObj, u"forceextensions", cef_v8value_create_bool(cte_settings.forceextensions));
            AddValueToObj(initialConfigObj, u"allowuntested", cef_v8value_create_bool(cte_settings.allowuntested));
            AddValueToObj(retobj, u"initialOptions", initialConfigObj);
            AddValueToObj(retobj, u"version", u"0.6");

            *retval = retobj;
            return TRUE;
        }
    }
    return cancelEsperantoCall_original(self, name, object, argumentsCount, arguments, retval, exception);
}

cef_v8value_create_function_t CEF_EXPORT cef_v8value_create_function_hook = [](const cef_string_t* name, cef_v8handler_t* handler) -> cef_v8value_t* {
    Wh_Log(L"cef_v8value_create_function called with name: %s", name->str);
    // Originally _getSpotifyModule was hooked but that function was removed in Spotify 1.2.56, which was released while this mod was in development
    // So, we're hooking cancelEsperantoCall instead. The function choice and arguments are odd, as it was originally intended for _getSpotifyModule. Deal with it.
    if (u"cancelEsperantoCall" == std::u16string(name->str, name->length)) {
        Wh_Log(L"cancelEsperantoCall is being created");
        if (g_hPipe == INVALID_HANDLE_VALUE) {
            // API won't be available if the pipe is not connected
            return cef_v8value_create_function_original(name, handler);
        }
        cancelEsperantoCall_original = handler->execute;
        handler->execute = cancelEsperantoCall_hook;
        return cef_v8value_create_function_original(name, handler);
    }
    return cef_v8value_create_function_original(name, handler);
};

BOOL InitSpotifyRendererHooks(HMODULE cefModule) {
    g_isSpotifyRenderer = TRUE;
    Wh_Log(L"Initializing Spotify renderer hooks");

    if (ConnectToNamedPipe() == 0) {
        Wh_Log(L"Connected to named pipe");
    } else {
        Wh_Log(L"Unable to connect to named pipe");
        return FALSE;
    }

    cef_v8value_create_function_t cef_v8value_create_function = (cef_v8value_create_function_t)GetProcAddress(cefModule, "cef_v8value_create_function");
    cef_v8value_create_bool = (cef_v8value_create_bool_t)GetProcAddress(cefModule, "cef_v8value_create_bool");
    cef_v8value_create_int = (cef_v8value_create_int_t)GetProcAddress(cefModule, "cef_v8value_create_int");
    cef_v8value_create_double = (cef_v8value_create_double_t)GetProcAddress(cefModule, "cef_v8value_create_double");
    cef_v8value_create_string = (cef_v8value_create_string_t)GetProcAddress(cefModule, "cef_v8value_create_string");
    cef_v8value_create_object = (cef_v8value_create_object_t)GetProcAddress(cefModule, "cef_v8value_create_object");
    cef_v8value_create_array = (cef_v8value_create_array_t)GetProcAddress(cefModule, "cef_v8value_create_array");

    if (!cef_v8value_create_function || !cef_v8value_create_bool || !cef_v8value_create_int || !cef_v8value_create_double || !cef_v8value_create_string || !cef_v8value_create_object || !cef_v8value_create_array) {
        Wh_Log(L"Failed to get CEF functions");
        return FALSE;
    }
    Wh_SetFunctionHook((void*)cef_v8value_create_function, (void*)cef_v8value_create_function_hook,
                       (void**)&cef_v8value_create_function_original);

    return TRUE;
}
#pragma endregion

void LoadSettings() {
    cte_settings.showframe = Wh_GetIntSetting(L"showframe");
    cte_settings.showframeonothers = Wh_GetIntSetting(L"showframeonothers");
    cte_settings.showmenu = Wh_GetIntSetting(L"showmenu");
    cte_settings.showcontrols = Wh_GetIntSetting(L"showcontrols");
    cte_settings.transparentcontrols = Wh_GetIntSetting(L"transparentcontrols");
    cte_settings.ignoreminsize = Wh_GetIntSetting(L"ignoreminsize");
    cte_settings.transparentrendering = Wh_GetIntSetting(L"transparentrendering");
    cte_settings.noforceddarkmode = Wh_GetIntSetting(L"noforceddarkmode");
    cte_settings.forceextensions = Wh_GetIntSetting(L"forceextensions");
    cte_settings.allowuntested = Wh_GetIntSetting(L"allowuntested");
}

int FindOffset(int major, int minor, cte_offset_t offsets[], int offsets_size, BOOL allow_untested = TRUE) {
    int prev_major = offsets[0].ver_major;
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
    if (allow_untested && major >= offsets[offsets_size - 1].ver_major) {
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

    char* pbExecutable = (char*)GetModuleHandle(NULL);

    #ifdef _WIN64
        const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
    #else
        const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
    #endif
    BOOL isInitialThread = *(USHORT*)((BYTE*)NtCurrentTeb() + OFFSET_SAME_TEB_FLAGS) & 0x0400;

    HMODULE cefModule = LoadLibrary(L"libcef.dll");
    if (!cefModule) {
        Wh_Log(L"Failed to load CEF!");
        return FALSE;
    }

    // Check if the app is Spotify
    wchar_t exeName[MAX_PATH];
    GetModuleFileName(NULL, exeName, MAX_PATH);
    BOOL isSpotify = wcsstr(_wcsupr(exeName), L"SPOTIFY.EXE") != NULL;
    if (isSpotify) {
        Wh_Log(L"Spotify detected");
    }

    cef_version_info_t cef_version_info = (cef_version_info_t)GetProcAddress(cefModule, "cef_version_info");

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

    if (major <= 89 || (major == 90 && minor <= 3)) {
        Wh_Log(L"Unsupported CEF version!");
        return FALSE;
    }

    BOOL isTestedVersion = cte_settings.allowuntested || major <= LAST_TESTED_CEF_VERSION;

    // Check if this process is auxilliary process by checking if the arguments contain --type=
    LPWSTR args = GetCommandLineW();
    if (wcsstr(args, L"--type=") != NULL) {
        if (isSpotify && isInitialThread &&
            major >= 108 && isTestedVersion &&
            wcsstr(args, L"--type=renderer") != NULL &&
            wcsstr(args, L"--extension-process") == NULL
        ) {
            return InitSpotifyRendererHooks(cefModule);
        }
        Wh_Log(L"Auxilliary process detected, skipping.");
        return FALSE;
    }

    // Get appropriate offsets for current CEF version
    is_frameless_offset = FindOffset(major, minor, is_frameless_offsets, ARRAYSIZE(is_frameless_offsets));
    Wh_Log(L"is_frameless offset: %#x", is_frameless_offset);
    get_window_handle_offset = FindOffset(major, minor, get_window_handle_offsets, ARRAYSIZE(get_window_handle_offsets), cte_settings.allowuntested);
    Wh_Log(L"get_window_handle offset: %#x", get_window_handle_offset);
    if (isSpotify) {
        add_child_view_offset = FindOffset(major, minor, add_child_view_offsets, ARRAYSIZE(add_child_view_offsets));
        Wh_Log(L"add_child_view offset: %#x", add_child_view_offset);
        set_background_color_offset = FindOffset(major, minor, set_background_color_offsets, ARRAYSIZE(set_background_color_offsets));
        Wh_Log(L"set_background_color offset: %#x", set_background_color_offset);
    }

    cef_window_create_top_level_t cef_window_create_top_level = (cef_window_create_top_level_t)GetProcAddress(cefModule, "cef_window_create_top_level");
    cef_panel_create_t cef_panel_create = (cef_panel_create_t)GetProcAddress(cefModule, "cef_panel_create");

    Wh_SetFunctionHook((void*)cef_window_create_top_level,
                       (void*)cef_window_create_top_level_hook,
                       (void**)&cef_window_create_top_level_original);
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_hook,
                       (void**)&CreateWindowExW_original);
    if (isSpotify) {
        Wh_SetFunctionHook((void*)cef_panel_create, (void*)cef_panel_create_hook,
                           (void**)&cef_panel_create_original);
        Wh_SetFunctionHook((void*)SetWindowThemeAttribute, (void*)SetWindowThemeAttribute_hook,
                           (void**)&SetWindowThemeAttribute_original);
        Wh_SetFunctionHook((void*)SetWindowTextW, (void*)SetWindowTextW_hook,
                           (void**)&SetWindowTextW_original);
        Wh_SetFunctionHook((void*)CreateProcessW, (void*)CreateProcessW_hook,
                           (void**)&CreateProcessW_original);
        Wh_SetFunctionHook((void*)CreateProcessAsUserW, (void*)CreateProcessAsUserW_hook,
                           (void**)&CreateProcessAsUserW_original);

        // Patch the executable in memory to enable transparent rendering, disable forced dark mode, or force enable extensions
        // (Only do this on process startup as patching after CEF initialization is pointless)
        if (isInitialThread) {
            if (cte_settings.transparentrendering) {
                if (EnableTransparentRendering(pbExecutable)) {
                    Wh_Log(L"Enabled transparent rendering");
                }
            }
            if (cte_settings.noforceddarkmode) {
                if (DisableForcedDarkMode(pbExecutable)) {
                    Wh_Log(L"Disabled forced dark mode");
                }
            }
            if (cte_settings.forceextensions) {
                if (ForceEnableExtensions(pbExecutable)) {
                    Wh_Log(L"Enabled extensions");
                }
            }

            #ifdef _WIN64
            if (major >= 122 && isTestedVersion) {
                HookCreateTrackPlayer(pbExecutable, major >= 127);
            }
            #endif
        }
    }

    EnumWindows(InitEnumWindowsProc, 1);
    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    g_shouldClosePipe = TRUE;

    if (g_isSpotifyRenderer) {
        // Note: sandboxed renderers won't even respond to the uninit request and keep loaded until the renderer exits
        if (g_hPipe != INVALID_HANDLE_VALUE) {
            CloseHandle(g_hPipe);
            g_hPipe = INVALID_HANDLE_VALUE;
        }
        if (g_pipeThread.joinable()) {
            g_pipeThread.join();
        }
        return;
    }

    if (g_hPipe != INVALID_HANDLE_VALUE) {
        CancelIoEx(g_hPipe, NULL);
        CloseHandle(g_hPipe);
        g_hPipe = INVALID_HANDLE_VALUE;
    }
    if (g_pipeThread.joinable()) {
        g_pipeThread.join();
    }

    EnumWindows(UninitEnumWindowsProc, 1);

    // Restore the original set_background_color functions to prevent crashes
    // (Control colors hooks won't work till the app is restarted)
    for (int i = 0; i < 3; i++) {
        if (cte_controls[i].set_background_color_addr != NULL) {
            *((set_background_color_t*)cte_controls[i].set_background_color_addr) = cte_controls[i].set_background_color_original;
        }
    }
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    if (g_isSpotifyRenderer) {
        // Won't work in a sandboxed renderer process
        // Up-to-date settings will be fetched through this mod's custom IPC
        return;
    }
    BOOL prev_transparentcontrols = cte_settings.transparentcontrols;
    LoadSettings();
    EnumWindows(UpdateEnumWindowsProc, prev_transparentcontrols != cte_settings.transparentcontrols);
}
