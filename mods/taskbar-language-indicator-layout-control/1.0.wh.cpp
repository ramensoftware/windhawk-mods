// ==WindhawkMod==
// @id              taskbar-language-indicator-layout-control
// @name            Taskbar language indicator layout control
// @description     Prevents the Tray area from jumping around when the language indicator is hidden while RDP client window is active. There are multiple mitigations you can choose from.
// @version         1.0
// @author          Roland Pihlakas
// @github          https://github.com/levitation
// @homepage        https://www.simplify.ee/
// @compilerOptions -lkernel32 -luser32
// @include         explorer.exe
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
# Taskbar language indicator layout control

By default, Windows has a distracting feature: When language indicator is enabled and the user opens a Remote Desktop client window, the language bar is hidden, causing a jump in the taskbar layout. When the user switches to another program, the taskbar jumps again as the language indicator is reshown.

Moreover, the language indicator configuration in Windows Settings is buggy even in a number of ways: Disabling the language indicator via Windows Settings does not "stick", after a reboot the indicator is visible again and needs to be turned off once more. As a smaller additional bug, the language indicator checkbox in acts weird, requiring the user to click the checkbox twice before it has effect. To confuse things even further, the checkbox description is reversed from what it should be.

This mod is targeted at solving the language indicator problems once and for all.

Under current mod's Settings you find four options to choose from for the language indicator layout:
1) Make it visible except when Windows wants to hide it temporarily. At the same time, the layout is fixed by the mod. This means that when Windows hides the language indicator temporarily while a Remote Desktop window is in foreground, then the space for the indicator is preserved by the mod, thereby avoiding the tray area from jumping around - there will be an empty space preserving the place of the language indicator.
2) Hide the language indicator persistently. This option is useful considering that after a reboot, Windows itself normally "forgets" the system setting to hide the language indicator. With this option, the mod overrides whatever Windows is trying to do to the language indicator.
3) Make it always visible, including when a Remote Desktop window is in foreground. This option again prevents the tray area from jumping around. Note that when Remote Desktop window is in foreground, then the language choice visible in the language indicator has no real effect - it is there just to avoid visual distractions.
4) Let Windows manage it. This option is conceptually equivalent to disabling this mod. But compared to mod disabling, choosing this option for temporary purposes might work faster and provides better Taskbar stability guarantees. If you do not need this mod anymore at all, then of course disable it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Config: keepLayoutOnly
  $name: How do you want the language indicator in the Tray area to be shown or hidden?
  $description: Please be patient, if you have many programs running then there may be a delay before the setting change is applied.
  $options:
  - keepLayoutOnly: Make it visible except when Windows wants to hide it temporarily, and keep the layout fixed when the indicator is only temporarily hidden
  - hide: Hide at all times
  - show: Show at all times
  - windowsDefault: Let Windows manage it in whatever way it wants to
*/
// ==/WindhawkModSettings==


#include <windowsx.h>

#include <atomic>


template <typename T>
BOOL Wh_SetFunctionHookT(
    FARPROC targetFunction,
    T hookFunction,
    T* originalFunction
) {
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}


enum class Config {
    hide,
    show,
    windowsDefault,
    keepLayoutOnly
};


std::atomic<size_t> g_hookRefCount;

PCWSTR g_languageIndicatorFrameClass = L"TrayInputIndicatorWClass";
PCWSTR g_languageIndicatorTextClass = L"InputIndicatorButton";

HANDLE g_initThread = NULL;
HANDLE g_initThreadStopSignal = NULL;
HWND g_hwndTaskbar = NULL;

HWND g_hwndLanguageIndicatorText = NULL;

bool g_windowsShowConfig;
bool g_doNotReadWindowsConfigDuringShowWindow = false;
bool g_showWindowWasOverriddenDuringLastCall = false;

Config g_config = Config::show;


using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t pOriginalShowWindow;


Config ConfigFromString(PCWSTR string) {
    if (wcscmp(string, L"hide") == 0) {
        return Config::hide;
    }
    else if (wcscmp(string, L"show") == 0) {
        return Config::show;
    }
    else if (wcscmp(string, L"windowsDefault") == 0) {
        return Config::windowsDefault;
    }
    else {
        return Config::keepLayoutOnly;
    }
}

void GetWindowsConfig() {

    bool show;

    BOOL value;
    if (!SystemParametersInfoW(
        SPI_GETSYSTEMLANGUAGEBAR,
        0,
        &value,     //The pvParam parameter must point to a BOOL variable
        0
    )) {
        Wh_Log(L"SystemParametersInfoW failed");
        return;
    }
    else {
        show = (value == FALSE);
    }

    g_windowsShowConfig = show;

    Wh_Log(L"Default Windows configuration: %ls", (show ? L"Show" : L"Hide"));

    return;
}

void SetWindowsConfig(bool show) {

    g_doNotReadWindowsConfigDuringShowWindow = true;


    BOOL value;

    value = show ? FALSE : TRUE;
    if (!SystemParametersInfoW(
        SPI_SETSYSTEMLANGUAGEBAR,
        0,
        //The documentation is wrong, it says - "The pvParam parameter is a pointer to a BOOL variable." - https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-systemparametersinfow
        //Instead, it should be a BOOL casted to PVOID, else it does not have effect during show.
        // 
        //Additionally, the documentation says "Set pvParam to TRUE to enable the legacy language bar, or FALSE to disable it." - If that is so, then the "legacy language bar" should be something else than the language input indicator on Taskbar, because for controlling the indicator on Taskbar, the opposite boolean values need to be used here. 
        //Also in reality, I personally never see that "legacy language bar" during logged in session, the only thing that actually changes is the language indicator on Taskbar. I do see a floating language bar in the login screen, but that is not relevant here.
        (PVOID)value,
        SPIF_SENDCHANGE | SPIF_UPDATEINIFILE
    )) {
        Wh_Log(L"SystemParametersInfoW failed");
    }


    g_doNotReadWindowsConfigDuringShowWindow = false;
}

bool IsLanguageIndicatorFrameWindow(HWND hWnd) {

    if (hWnd) {

        wchar_t buf[32];
        if (!GetClassNameW(
            hWnd,
            buf,
            ARRAYSIZE(buf)
        )) {
            Wh_Log(L"GetClassNameW failed");
        }
        else {
            if (wcscmp(buf, g_languageIndicatorFrameClass) == 0) {

                return true;
            }
        }
    }

    return false;
}

bool IsLanguageIndicatorTextWindow(HWND hWnd) {

    if (hWnd) {

        wchar_t buf[32];
        if (!GetClassNameW(
            hWnd,
            buf,
            ARRAYSIZE(buf)
        )) {
            Wh_Log(L"GetClassNameW failed");
        }
        else {
            if (wcscmp(buf, g_languageIndicatorTextClass) == 0) {

                return true;
            }
        }
    }

    return false;
}

BOOL CALLBACK FindLanguageIndicatorTextWindowEnumProc(HWND hWnd, LPARAM lParam) {

    if (IsLanguageIndicatorTextWindow(hWnd)) {

        g_hwndLanguageIndicatorText = hWnd;

        return FALSE;   //to stop enumeration, it must return FALSE.
    }

    return TRUE; //To continue enumeration, the callback function must return TRUE
}

HWND FindLanguageIndicatorTextHwnd() {

    if (g_hwndLanguageIndicatorText)    //we can cache that value, the language indicator text is created only once
        return g_hwndLanguageIndicatorText;

    //FindWindowEx would not work here since we need a recursive search
    EnumChildWindows(g_hwndTaskbar, FindLanguageIndicatorTextWindowEnumProc, NULL);     //NB! cannot check for return value here, accoding to documentation, "The return value is not used." - https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumchildwindows

    if (!g_hwndLanguageIndicatorText) {
        Wh_Log(L"Finding language indicator text window failed");
    }

    return g_hwndLanguageIndicatorText;
}

BOOL WINAPI ShowWindowHook(
    IN HWND hWnd,
    IN int  nCmdShow
) {
    g_hookRefCount++;

    if (g_hwndTaskbar) {               //is the current process the taskbar process?

        int originalError = GetLastError();

        if (IsLanguageIndicatorFrameWindow(hWnd)) {

            if (!g_doNotReadWindowsConfigDuringShowWindow) {

                GetWindowsConfig();     //update mod's internal copy of Windows config
            }

            if (g_config == Config::keepLayoutOnly) {

                if (g_windowsShowConfig == false) {

                    //allow windows to hide the language indicator entirely
                }
                else {  //Generally Windows wants to show the indicator, but right now it wants to hide it temporarily. Lets preserve the layout by hiding only the text but not the frame.

                    //Windows will not hide the text on its own, we need to do it manually here
                    HWND languageIndicatorTextHwnd = FindLanguageIndicatorTextHwnd();

                    SetLastError(0);    //ShowWindow() does not clear earlier errors in case of success, so lets clear it here manually, so we can check for error after the call

                    if (languageIndicatorTextHwnd) {
                        pOriginalShowWindow(
                            languageIndicatorTextHwnd,
                            nCmdShow
                        );
                        if (GetLastError())
                        {
                            Wh_Log(L"ShowWindow failed");
                        }
                    }

                    if (nCmdShow == SW_HIDE) {
                        //hide the language indicator text instead, but keep the language indicator frame in place

                        Wh_Log(L"redirecting hWnd 0x%llX to 0x%llX nCmdShow %u", (long long)hWnd, (long long)languageIndicatorTextHwnd, nCmdShow);

                        g_showWindowWasOverriddenDuringLastCall = true;
                        SetLastError(0);    //override silently
                        g_hookRefCount--;
                        return TRUE;       //If the window was previously visible, the return value is nonzero.
                    }
                    else {  //show both the frame and the indicator text

                        //NB! show the frame first and only then show the text
                        Wh_Log(L"allowing hWnd 0x%llX nCmdShow %u", (long long)hWnd, nCmdShow);

                        //NB! do not return here, call the function on the frame as well
                    }
                }
            }
            else if (
                (
                    g_config == Config::show
                    && nCmdShow == SW_HIDE  //ignore hide commands
                )
                || (
                    g_config == Config::hide
                    && nCmdShow != SW_HIDE  //ignore show commands
                )
            ) {
                Wh_Log(L"blocking hWnd 0x%llX nCmdShow %u", (long long)hWnd, nCmdShow);

                g_showWindowWasOverriddenDuringLastCall = true;
                SetLastError(0);    //override silently
                BOOL result = nCmdShow == SW_HIDE;
                g_hookRefCount--;
                return result;       //If the window was previously hidden, the return value is zero. If the window was previously visible, the return value is nonzero.
            }
            else {
                Wh_Log(L"allowing hWnd 0x%llX nCmdShow %u", (long long)hWnd, nCmdShow);
            }


            g_showWindowWasOverriddenDuringLastCall = false;    //NB! update this flag only if the ShowWindow was called for the language indicator

        }   //if (IsLanguageIndicatorFrameWindow(hWnd)) {

        SetLastError(originalError);
    }


    BOOL result = pOriginalShowWindow(
        hWnd,
        nCmdShow
    );
    g_hookRefCount--;
    return result;
}

void ApplyWindowsDefaultConfig() {

    bool show = g_windowsShowConfig;

    if (g_showWindowWasOverriddenDuringLastCall) {
        //NB! just in case, flip the config temporarily in order to get the language indicator visual to be refreshed
        //This does not seem actually to be currently necessary, but it does not harm to do it and it makes the mod more future proof
        SetWindowsConfig(!show);      //trigger the Tray area update 
        g_showWindowWasOverriddenDuringLastCall = false;
    }

    SetWindowsConfig(show);     //trigger the Tray area update
}

void ApplyHideOrShowSettings() {

    Config config = g_config;

    if (config != Config::windowsDefault) {

        bool show = (
            config == Config::show      //NB! No need to change Windows config. The language bar will show updates to the input language even if Windows thinks the language bar is hidden.
            || config == Config::keepLayoutOnly     //Let Windows allocate space for the language indicator as if it was shown, regardless whether it will be actually temporarily hidden when Remote Desktop window is in foreground.
        );

        GetWindowsConfig();     //store copy of Windows config locally
        SetWindowsConfig(show);     //set Windows config to match mod's config - this will trigger the Tray area update so we can override what needs to be overridden in the hook
        ApplyWindowsDefaultConfig();        //NB! Immediately restore Windows original config in the registry. The Windows registry config was changed above only temporarily in order to trigger Taskbar's visual adjustment.
    }
}

bool TryInit(bool* abort) {

    HWND hwndTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    Wh_Log(L"hwndTaskbar: 0x%llX", (long long)hwndTaskbar);
    if (!hwndTaskbar)
        return false;   //retry

    DWORD taskbarProcessId;
    if (
		!GetWindowThreadProcessId(hwndTaskbar, &taskbarProcessId) 
		|| !taskbarProcessId
	) {
        return false;   //retry
	}

    Wh_Log(L"taskbarProcessId: %u", taskbarProcessId);
    if (taskbarProcessId != GetCurrentProcessId()) {
        Wh_Log(L"Not a taskbar process, hooks will not be active");
        *abort = true;
        return false;
    }
    else {
        g_hwndTaskbar = hwndTaskbar;

        ApplyHideOrShowSettings();

        Wh_Log(L"Initialising taskbar hwnd done");

        return true;
    }
}

DWORD WINAPI InitThreadFunc(LPVOID param) {

    Wh_Log(L"InitThreadFunc enter");

    bool isRetry = false;
retry:  //wait until taskbar has properly initialised in order to detect whether current process will become taskbar or not
    if (isRetry) {
        if (WaitForSingleObject(g_initThreadStopSignal, 1000) != WAIT_TIMEOUT) {
            Wh_Log(L"Shutting down InitThreadFunc before success");
            return FALSE;
        }
    }
    isRetry = true;

    bool abort = false;
    if (TryInit(&abort)) {
        return TRUE;
    }
    else if (abort) {
        return FALSE;   //if the taskbar process is already running then subsequent non-taskbar related explorer.exe instances will not be hooked
    }
    else {      //taskbar was not yet found, so we need to retry later
        goto retry;
    }
}

void Wh_ModAfterInit(void) {

    Wh_Log(L"Initialising hooks done");
}

void LoadSettings() {

    PCWSTR configString = Wh_GetStringSetting(L"Config");
    g_config = ConfigFromString(configString);
    Wh_FreeStringSetting(configString);
}

BOOL Wh_ModInit() {

    LoadSettings();

    Wh_Log(L"Init");


    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        Wh_Log(L"Loading user32.dll failed");
        return FALSE;
    }
    
    FARPROC pShowWindow = GetProcAddress(hUser32, "ShowWindow");
    if (!pShowWindow) {
        Wh_Log(L"Finding hookable functions from user32.dll failed");
        return FALSE;
    }


    bool abort = false;
    if (TryInit(&abort)) { 
        //set hooks at the end and then exit the function
    }
    else if (abort) {
        return FALSE;   //if the taskbar process is already running then subsequent non-taskbar related explorer.exe instances will not be hooked
    }
    else {
        //Taskbar was not yet found, maybe it is still initialising in the current process, so we need to retry later.
        //Hooking CreateWindowExW would cause potential instabilities during mod unload therefore using polling thread instead.

        g_initThreadStopSignal = CreateEventW(
            /*lpEventAttributes = */NULL,           // default security attributes
            /*bManualReset = */TRUE,				// manual-reset event
            /*bInitialState = */FALSE,              // initial state is nonsignaled
            /*lpName = */NULL						// object name
        );

        if (!g_initThreadStopSignal) {
            Wh_Log(L"CreateEvent failed");
            return FALSE;
        }

        g_initThread = CreateThread(
            /*lpThreadAttributes = */NULL,
            /*dwStackSize = */0,    //in this mod we can start the init thread immediately, before hooks are activated
            InitThreadFunc,
            /*lpParameter = */NULL,
            /*dwCreationFlags = */0,
            /*lpThreadId = */NULL
        );

        if (g_initThread) {
            Wh_Log(L"InitThread created");
            //set hooks at the end and then exit the function
        }
        else {
            Wh_Log(L"CreateThread failed");
            CloseHandle(g_initThreadStopSignal);
            g_initThreadStopSignal = NULL;
            return FALSE;
        }
    }


    Wh_Log(L"Initialising hooks...");

    Wh_SetFunctionHookT(pShowWindow, ShowWindowHook, &pOriginalShowWindow);
    
    return TRUE;
}

void Wh_ModSettingsChanged() {

    Wh_Log(L"Wh_ModSettingsChanged");

    LoadSettings();

    if (g_config == Config::windowsDefault) {

        GetWindowsConfig();
        ApplyWindowsDefaultConfig();
    }
    else {  //Config::show or Config::hide or Config::keepLayoutOnly

        ApplyHideOrShowSettings();
    }
}

void Wh_ModUninit() {

    Wh_Log(L"Uniniting...");

    if (g_initThread) {
        SetEvent(g_initThreadStopSignal);
        WaitForSingleObject(g_initThread, INFINITE);
        CloseHandle(g_initThread);
        g_initThread = NULL;
    }

    if (g_initThreadStopSignal) {
        CloseHandle(g_initThreadStopSignal);
        g_initThreadStopSignal = NULL;
    }


    //Wait for the hooked calls to exit. I have seen Taskbar crashing during this mod's unload without this.
    do {    //first sleep, then check g_hookRefCount since some hooked function might have a) entered, but not increased g_hookRefCount yet, or b) has decremented g_hookRefCount but not returned to the caller yet
        if (g_hookRefCount)
            Wh_Log(L"g_hookRefCount: %lli", (long long)g_hookRefCount);

        Sleep(1000);    //NB! Sleep always at least once. See the comment at the "do" keyword.

    } while (g_hookRefCount > 0);


    //Apply original Windows config only after the hooked calls have exited
    GetWindowsConfig();
    ApplyWindowsDefaultConfig();


    Wh_Log(L"Uninit complete");
}
