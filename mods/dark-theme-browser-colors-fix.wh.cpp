// ==WindhawkMod==
// @id              dark-theme-browser-colors-fix
// @name            Fix browser and Teams text colors in dark mode
// @description     For dark theme users, fixes unreadable web sites with bright text on white background or black text on dark background. Likewise fixes Teams document viewer's text colors.
// @version         1.0
// @author          Roland Pihlakas
// @github          https://github.com/levitation
// @homepage        https://www.simplify.ee/
// @compilerOptions -lgdi32 -luser32 -lshlwapi
// @include         brave.exe
// @include         chrome.exe
// @include         chromium.exe
// @include         firefox.exe
// @include         msedge.exe
// @include         msedgewebview*.exe
// @include         opera.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/levitation-opensource/my-windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/levitation-opensource/my-windhawk-mods/

// ==WindhawkModReadme==
/*
# Fix browser text colors as well as Teams document viewer text colors under dark theme

Install this mod when you use dark theme in Windows and have occasionally problems with unreadable text color in your browser or Teams document viewer, such as bright text on white background or black text on dark background. 

This mod does not force color change on websites that specify their own colors. Only badly implemented web sites that accidentally (and inconsistently) try to use Windows colors will be adjusted. 


## How it works

The odd-looking web sites that are unreadable under dark theme are that way because they try to use Windows colors, but do so inconsistently. Additionally, these sites assume that the Windows colors use bright theme. These sites are not dark theme aware.

The mod works by emulating bright Windows 10 default color scheme for these web sites, to match their assumptions.

The color adjustments made by this mod generally do not affect the browsers' or Teams main frame appearance. The mod might affect the Firefox menu highlight color in some computers. Additionally, the mod detects calls from file picker and does not replace the colors there.


## Installation

You may need to restart your browser after installing this mod for the adjustments to take effect, though some web sites in some browsers will be fixed immediately.

Currently supported browsers are: Brave, Chrome, Chromium, Edge, Firefox, and Opera. And additionally Teams document viewer.

Under mod's settings there is an option to apply color adjustments to all `msedgewebview*.exe` processes, not only to the ones related to **Teams**. This option is off by default. Browser colors are always adjusted regardless of this setting.


## Examples of where it is useful

You can test the following web sites before and after installing the mod: 
* You can test Chromium-based browsers (Brave, Chrome, Edge, Opera) by opening Word documents under Sharepoint folders while "View -> Dark Mode" button is off in the Online Word app toolbar.
* You can test Firefox by opening for example [https://www.hexamail.com/about](https://www.hexamail.com/about) or [https://transformer-circuits.pub/2025/april-update/index.html](https://transformer-circuits.pub/2025/april-update/index.html)
* You can test Teams by opening Sharepoint office documents inside Teams document viewer.

For some reason, these problems do not manifest in all computers with dark color scheme. I have multiple computers with similar setups, and in a few computer-and-browser combinations the issue did not manifest. But all targeted browsers have the problem in general.


## Notice

Unloading this mod while a browser is running is not recommended. Unloading the mod may crash these browser tabs that used default Windows colors. Though rest of the tabs will be probably fine.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- applyToAllMsEdgeWebView: false
  $name: Apply color adjustments to all programs using msedgewebview
  $description: Applies the color adjustment to all msedgewebview*.exe processes. If turned off, only Teams-related msedgewebview processes are adjusted (for the purposes of the embedded document viewer). Browser colors are always adjusted regardless of this setting.
*/
// ==/WindhawkModSettings==


#pragma region Includes

#include <windowsx.h>
#include <intrin.h>
#include <shlwapi.h>

#include <atomic>
#include <mutex>
#include <map>
#include <unordered_set>

#pragma endregion Includes



#pragma region Reusable general helper functions

template <typename T>
BOOL Wh_SetFunctionHookT(
    FARPROC targetFunction,
    T hookFunction,
    T* originalFunction
) {
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}

#pragma endregion Reusable general helper functions



#pragma region Global variables

const int nMaxPathLength = (MAX_PATH * 16);
const int nMaxDllPathLength = nMaxPathLength;  //there is no function for retrieving the dll module name length, so need to use custom limit
#define MAX_COLOR_INDEX     COLOR_MENUBAR

const wchar_t* filePickerDlls[] = {
    L"\\Shell32.dll",       //folder tree
    L"\\ComCtl32.dll",      //top folder location bar and save/cancel buttons
    L"\\DUI70.dll",         //file list
    L"\\ExplorerFrame.dll", //file search box (at top right)
    L"\\uxtheme.dll",       //a dropdown opening from the file search box
    L"\\ACLUI.dll",         //file permissions dialog
    L"\\gdi32full.dll",     //file picker selected text
    L"\\comdlg32.dll"       //left panel in classic file picker (in case the Classic File Picker dialog mod is installed)    
};

const COLORREF defaultWindowsColors[] = {   //Default Windows10 colors
    0x00C8C8C8, //0
    0x00FF8000, //1
    0x00D1B499, //2
    0x00DBCDBF, //3

    0x00F0F0F0, //4
    0x00FFFFFF, //5
    0x00646464, //6
    0x00000000, //7

    0x00000000, //8
    0x00000000, //9
    0x00B4B4B4, //10
    0x00FCF7F4, //11

    0x00ABABAB, //12
    0x00D77800, //13
    0x00000000, //14
    0x00F0F0F0, //15

    0x00A0A0A0, //16
    0x006D6D6D, //17
    0x00000000, //18
    0x00000000, //19

    0x00FFFFFF, //20
    0x00696969, //21
    0x00E3E3E3, //22
    0x00000000, //23

    0x00E1FFFF, //24
    0x00000000, //25 - unused in GetSysColor, GetSysColorBrush, and GetThemeSysColor, but used for TMT_BUTTONALTERNATEFACE in GetThemeSysColorBrush
    0x00CC6600, //26
    0x00EAD1B9, //27

    0x00F2E4D7, //28
    0x00D77800, //29
    0x00F0F0F0, //30
};

DWORD g_processId = -1;

std::atomic<size_t> g_hookRefCount;

std::mutex g_filePickerDetectionMutex;
std::map<void*, bool> g_filePickerDetectionMap;

HMODULE hGdi32 = NULL;
HMODULE hUser32 = NULL;

HBRUSH g_preallocatedBrushes[MAX_COLOR_INDEX + 1] = {};
std::unordered_set<HBRUSH> g_preallocatedBrushesSet;

bool g_isNonTeamsWebView = false;
bool g_applyToAllWebView = false;

bool g_unloading = false;
bool g_unloaded = false;

#pragma endregion Global variables



#pragma region Originals of hooked functions

using GetSysColor_t = decltype(&GetSysColor); 
GetSysColor_t pOriginalGetSysColor; 

using GetSysColorBrush_t = decltype(&GetSysColorBrush); 
GetSysColorBrush_t pOriginalGetSysColorBrush; 

using DeleteObject_t = decltype(&DeleteObject); 
DeleteObject_t pOriginalDeleteObject;

#pragma endregion Originals of hooked functions



#pragma region Reusable scope and reference counting functions

void IncrementHookRefCountA(LPCSTR tag) {

    g_hookRefCount++;
}

void DecrementHookRefCountA(LPCSTR tag) {

    g_hookRefCount--;
}

auto HookRefCountScopeA(LPCSTR tag) {

    IncrementHookRefCountA(tag);

    return std::unique_ptr<
        const CHAR,
        void(*)(LPCSTR)
    >{
        tag, [](LPCSTR tag) {
            DecrementHookRefCountA(tag);
        }
    };
}

#define AUTO_HOOK_COUNT_SCOPE           auto hookScope = HookRefCountScopeA(__func__);   //deny mod unload until this function exits

#pragma endregion Reusable scope and reference counting functions



#pragma region Caller module name getter

#ifdef _MSC_VER
#define ReturnAddress()     _ReturnAddress()
#else   //clang compiler
#define ReturnAddress()     __builtin_return_address(0)
#endif

HMODULE GetCallerModule(void *address) {

    if (!address)
        return NULL;

    SetLastError(0);    //some Windows API-s do not clear earlier errors in case of success, so lets clear it here manually just in case, so we can check for error after the call

    HMODULE hModule;
    if (!GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCTSTR)address, 
        &hModule
    )) {    //usually GetModuleHandleExW fails in case of .NET executables
        int errorCode = GetLastError();
        Wh_Log(L"Getting caller module failed for address 0x%llX error %i", (long long)address, errorCode);
        return NULL;
    }
    return hModule;
}

LPCWSTR GetModuleName(
    HMODULE hModule,
    LPWSTR  stackBuffer,
    int     stackBufferSize
) {
    if (!hModule)
        return NULL;

    SetLastError(0);    //some Windows API-s do not clear earlier errors in case of success, so lets clear it here manually just in case, so we can check for error after the call

    if (!GetModuleFileNameW(
        hModule,
        stackBuffer,
        stackBufferSize
    )) {
        Wh_Log(L"GetModuleFileNameW failed. Module 0x%llX", (long long)hModule);

        SetLastError(0);
        return NULL;
    }
    //If the buffer is too small to hold the module name, the string is truncated to nSize characters including the terminating null character, the function returns nSize, and the function sets the last error to ERROR_INSUFFICIENT_BUFFER.
    else if (GetLastError()) {
        Wh_Log(L"GetModuleFileNameW failed, probably the module path is too long. Module 0x%llX", (long long)hModule);

        SetLastError(0);
        return NULL;
    }
    else {
        return stackBuffer;
    }
}

bool IsCallerFilePicker(void* returnAddress) {

    bool result = false;

    std::lock_guard<std::mutex> guard(g_filePickerDetectionMutex);
    auto it = g_filePickerDetectionMap.find(returnAddress);
    if (it != g_filePickerDetectionMap.end()) {
        result = it->second;
    }
    else {
        HMODULE callerModule = GetCallerModule(returnAddress);

        WCHAR stackBuffer[nMaxDllPathLength];
        PCWSTR callerDllPath = GetModuleName(
            callerModule,
            stackBuffer,
            nMaxDllPathLength
        );

        if (callerDllPath) {

            size_t callerDllPathLen = wcslen(callerDllPath);

            for (unsigned int i = 0; i < ARRAYSIZE(filePickerDlls); i++) {

                LPCWSTR checkPath = filePickerDlls[i];
                size_t checkPathLen = wcslen(checkPath);
                if (
                    callerDllPathLen >= checkPathLen
                    && wcsicmp(&callerDllPath[callerDllPathLen - checkPathLen], checkPath) == 0     //match end of path
                ) {
                    result = true;
                    break;
                }
            }
        }

        if (result) {
            Wh_Log(L"A file picker caller detected: %ls", callerDllPath ? callerDllPath : L"");
        }
        else {
            Wh_Log(L"A non file picker caller detected: %ls", callerDllPath ? callerDllPath : L"");
        }

        g_filePickerDetectionMap.insert({ 
            returnAddress, 
            result
        });
    }

    return result;
}

#pragma endregion Caller module name getter



#pragma region Hooks

DWORD WINAPI GetSysColorHook(IN int nIndex) {

    AUTO_HOOK_COUNT_SCOPE;

    if (
        nIndex >= 0
        && nIndex <= MAX_COLOR_INDEX
        && !g_unloading
        && (!g_isNonTeamsWebView || g_applyToAllWebView)
        && !IsCallerFilePicker(ReturnAddress())    //file picker detection
    ) {
        return defaultWindowsColors[nIndex];
    }
    else {
        return pOriginalGetSysColor(nIndex);
    }
}

HBRUSH WINAPI GetSysColorBrushHook(IN int nIndex) {

    AUTO_HOOK_COUNT_SCOPE;

    if (
        nIndex >= 0 
        && nIndex <= MAX_COLOR_INDEX
        && !g_unloading
        && (!g_isNonTeamsWebView || g_applyToAllWebView)
        && !IsCallerFilePicker(ReturnAddress())    //file picker detection
    ) {
        //Need to use preallocated system color brushes since the program does not have to free them and allocating a new brush upon each CreateSolidBrushHook call would result in a resource leak.
        return g_preallocatedBrushes[nIndex];
    }
    else {
        return pOriginalGetSysColorBrush(nIndex);
    }
}

BOOL WINAPI DeleteObjectHook(IN HGDIOBJ ho) {

    AUTO_HOOK_COUNT_SCOPE;

    if (
        !g_unloaded
        && g_preallocatedBrushesSet.find((HBRUSH)ho) != g_preallocatedBrushesSet.end()
    ) {
        //While deleting system color brushes is not mandatory, it is still allowed. We need to catch these calls and ignore them so the custom brush continues to be available elsewhere.
        return TRUE;
    }
    else {
        return pOriginalDeleteObject(ho);
    }
}

#pragma endregion Hooks



#pragma region Mod entrypoints - Init, AfterInit, BeforeUninit, Uninit

bool DetectNonTeamsWebView() {

    bool hookAllMsEdgeWebView = Wh_GetIntSetting(L"hookAllMsEdgeWebView");
    if (hookAllMsEdgeWebView)
        return false;


    WCHAR programPath[nMaxPathLength];
    DWORD dwSize = ARRAYSIZE(programPath);
    if (!QueryFullProcessImageNameW(GetCurrentProcess(), 0, programPath, &dwSize)) {
        *programPath = L'\0';
    }

    //is the current process msedgewebview?
    PCWSTR msedgewebviewProcessName = L"\\msedgewebview";
    if (StrStrIW(programPath, msedgewebviewProcessName)) {

        //if the current process is msedgewebview then check whether it is serving ms-teams.exe, else do not hook
        LPCWSTR commandLine = GetCommandLineW();    //the lifetime of the returned value is managed by the system, applications should not free or modify this value
        if (!StrStrIW(commandLine, L"--webview-exe-name=ms-teams.exe")) {
            Wh_Log(L"Teams MsEdgeWebView process detected");
            return true;
        }
        else {
            Wh_Log(L"Non-Teams MsEdgeWebView process detected");
        }
    }

    return false;
}

void LoadSettings() {

    g_applyToAllWebView = Wh_GetIntSetting(L"applyToAllMsEdgeWebView");
}

BOOL Wh_ModInit() {

    g_processId = GetCurrentProcessId();


    LoadSettings();
    g_isNonTeamsWebView = DetectNonTeamsWebView();


    Wh_Log(L"Initialising hooks...");


    hGdi32 = LoadLibraryW(L"Gdi32.dll");
    if (!hGdi32) {
        Wh_Log(L"Error: Loading Gdi32.dll failed");
        return FALSE;
    }

    hUser32 = LoadLibraryW(L"User32.dll");
    if (!hUser32) {
        Wh_Log(L"Error: Loading User32.dll failed");
        return FALSE;
    }


    FARPROC pGetSysColor = GetProcAddress(hUser32, "GetSysColor"); 
    FARPROC pGetSysColorBrush = GetProcAddress(hUser32, "GetSysColorBrush");
    FARPROC pDeleteObject = GetProcAddress(hGdi32, "DeleteObject");

    if (!pGetSysColor) {
        Wh_Log(L"Finding hookable function GetSysColor from User32.dll failed");
        return FALSE;
    }     
    else if (!pGetSysColorBrush) {
        Wh_Log(L"Finding hookable function GetSysColorBrush from User32.dll failed");
        return FALSE;
    } 
    else if (!pDeleteObject) {
        Wh_Log(L"Finding hookable function DeleteObject from Gdi32.dll failed");
        return FALSE;
    }


    pOriginalGetSysColor = (GetSysColor_t)pGetSysColor;    
    for (int i = 0; i <= MAX_COLOR_INDEX; i++)
    {
        COLORREF color = pOriginalGetSysColor(i);
        HBRUSH brush = CreateSolidBrush(color);

        g_preallocatedBrushes[i] = brush;
        g_preallocatedBrushesSet.insert(brush);
    }


    Wh_SetFunctionHookT(pGetSysColor, GetSysColorHook, &pOriginalGetSysColor);
    Wh_SetFunctionHookT(pGetSysColorBrush, GetSysColorBrushHook, &pOriginalGetSysColorBrush);
    Wh_SetFunctionHookT(pDeleteObject, DeleteObjectHook, &pOriginalDeleteObject);


    return TRUE;
}

BOOL CALLBACK EnumBrowserWindowsFunc(HWND hWnd, LPARAM doNotWait) {

    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (
        !dwThreadId 
        || dwProcessId != g_processId
    ) {
        return TRUE;
    }

    if (doNotWait) {
        //Sends the specified message to a window or windows. If the window was created by the calling thread, SendNotifyMessage calls the window procedure for the window and does not return until the window procedure has processed the message. If the window was created by a different thread, SendNotifyMessage passes the message to the window procedure and returns immediately; it does not wait for the window procedure to finish processing the message.
        SendNotifyMessageW(hWnd, WM_SYSCOLORCHANGE, NULL, NULL);
    }
    else {
        //Unfortunately, cannot use SendMessageCallbackW here since some windows never call the callback for some reason.

        //Calls the window procedure for the specified window and, if the specified window belongs to a different thread, does not return until the window procedure has processed the message or the specified time-out period has elapsed. If the window receiving the message belongs to the same queue as the current thread, the window procedure is called directly - the time-out value is ignored
        SendMessageTimeoutW(
            hWnd, 
            WM_SYSCOLORCHANGE, 
            NULL, 
            NULL, 
            SMTO_NOTIMEOUTIFNOTHUNG,    //The function does not enforce the time-out period as long as the receiving thread is processing messages. 
            1000,   //uTimeout
            NULL    //lpdwResult
        );
    }

    return TRUE;
}

void Wh_ModAfterInit(void) {

    Wh_Log(L"Initialising hooks done");

    //try to apply the updated colors to the previously running browser process immediately
    EnumWindows(EnumBrowserWindowsFunc, /*doNotWait*/true);
}

void Wh_ModSettingsChanged() {

    Wh_Log(L"SettingsChanged");

    LoadSettings();

    if (g_isNonTeamsWebView) {
        //try to apply the updated colors to the previously running MsEdgeWebView process immediately
        EnumWindows(EnumBrowserWindowsFunc, /*doNotWait*/true);
    }
}

void Wh_ModBeforeUninit() {

    Wh_Log(L"Restoring initial color scheme");

    g_unloading = true; //use original color scheme while processing WM_SYSCOLORCHANGE and onwards

    //try to apply the original colors
    EnumWindows(EnumBrowserWindowsFunc, /*doNotWait*/false);
}

void Wh_ModUninit() {

    Wh_Log(L"Uninit");

    g_unloaded = true;  //do not try to access g_preallocatedBrushesSet in the DeleteObjectHook any more

    //Wait for hooks to exit. I have seen programs crashing during mod unload without this.
    do {    //first sleep, then check g_hookRefCount since some hooked function might have a) entered, but not increased g_hookRefCount yet, or b) has decremented g_hookRefCount but not returned to the caller yet
        if (g_hookRefCount)
            Wh_Log(L"g_hookRefCount: %lli", (long long)g_hookRefCount);

        Sleep(1000);    //NB! Sleep always at least once. See the comment at the "do" keyword.

    } while (g_hookRefCount > 0);


    for (int i = 0; i <= MAX_COLOR_INDEX; i++) {
        pOriginalDeleteObject(g_preallocatedBrushes[i]);
        g_preallocatedBrushes[i] = NULL;
    }


    if (hGdi32) {
        FreeLibrary(hGdi32);
        hGdi32 = NULL;
    }

    if (hUser32) {
        FreeLibrary(hUser32);
        hUser32 = NULL;
    }
}

#pragma endregion Mod entrypoints - Init, AfterInit, SettingsChanged, Uninit
