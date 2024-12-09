// ==WindhawkMod==
// @id              disable-virtual-desktop-transition
// @name            Disable Virtual Desktop Transition Animation
// @description     Disables the animation when switching between virtual desktops on Windows 10.
// @version         1.0
// @author          Isabella Lulamoon (kawapure)
// @github          https://github.com/kawapure
// @twitter         https://twitter.com/kawaipure
// @homepage        https://kawapure.github.io
// @include         dwm.exe
// @architecture    x86-64
// @compilerOptions -lkernel32 -lwevtapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Virtual Desktop Transition Animation

This mod disables the animation seen when switching between virtual desktops in Windows 10.

## ⚠ Important usage note ⚠ 
  
In order to use this mod, you must allow Windhawk to inject into the **dwm.exe** 
system process. To do so, add it to the process inclusion list in the advanced 
settings. If you do not do this, it will silently fail to inject.
  
![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png) 
*/
// ==/WindhawkModReadme==

#include <libloaderapi.h>
#include <minwindef.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winevt.h>
#include <processthreadsapi.h>

LPCWSTR g_kMsgBoxTitle = L"Disable Virtual Desktop Transition Animation (Windhawk mod)";

LPCWSTR g_kNoInjectMsg =
    L"DWM was unexpectedly restarted. The mod will not be reinjected to avoid a potential "
    L"crash loop. Please disable and re-enable the mod to force reinjection.";

class CVersionHelper
{
public:
	// Data structure returned by GetVersionInfo().
	struct VersionStruct
	{
		bool isInitialized = false;
		DWORD dwMajorVersion = 0;
		DWORD dwMinorVersion = 0;
		DWORD dwBuildNumber = 0;
		DWORD dwPlatformId = 0;
	};

	typedef void (WINAPI *RtlGetVersion_t)(OSVERSIONINFOEXW *);

	// Gets the precise OS version.
	static const VersionStruct *GetVersionInfo()
	{
		static VersionStruct s_versionStruct = { 0 };

		// Skip if cached.
		if (!s_versionStruct.isInitialized)
		{
			HMODULE hMod = LoadLibraryW(L"ntdll.dll");

			if (hMod)
			{
				RtlGetVersion_t func = (RtlGetVersion_t)GetProcAddress(hMod, "RtlGetVersion");

				if (!func)
				{
					FreeLibrary(hMod);

					// TODO: error handling.
					return &s_versionStruct;
				}

				OSVERSIONINFOEXW osVersionInfo = { 0 };
				osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

				func(&osVersionInfo);

				s_versionStruct.dwBuildNumber = osVersionInfo.dwBuildNumber;
				s_versionStruct.dwMajorVersion = osVersionInfo.dwMajorVersion;
				s_versionStruct.dwMinorVersion = osVersionInfo.dwMinorVersion;
				s_versionStruct.dwPlatformId = osVersionInfo.dwPlatformId;

				s_versionStruct.isInitialized = true;

				FreeLibrary(hMod);
			}
		}

		return &s_versionStruct;
	}

	// Specific version helpers.
	inline static bool IsWindows10OrGreater()
	{
		return GetVersionInfo()->dwMajorVersion >= 10 &&
			   GetVersionInfo()->dwBuildNumber >= 10240;
	}

    inline static bool IsWindows11OrGreater()
    {
        return GetVersionInfo()->dwMajorVersion >= 10 &&
               GetVersionInfo()->dwMinorVersion >= 22000;
    }
};

__declspec(noinline) HRESULT CVirtualDesktop__Initialize(void *pThis, void *a2)
{
    // Failure result makes DWM reject the animation and move on like normal.
    return E_FAIL;
}

size_t g_curVftableVirtualDesktopSwitch[18];

typedef void *(__cdecl *CVirtualDesktopSwitch__CVirtualDesktopSwitch_t)(void *pThis);
CVirtualDesktopSwitch__CVirtualDesktopSwitch_t CVirtualDesktopSwitch__CVirtualDesktopSwitch_orig;
void *__cdecl CVirtualDesktopSwitch__CVirtualDesktopSwitch_hook(void *pThis)
{
    void *pAnim = CVirtualDesktopSwitch__CVirtualDesktopSwitch_orig(pThis);

    /*
     * CVirtualDesktop inherits the Initialize() method from CStoryboard, so we need to
     * create our own copy of the virtual function table to replace this with our own
     * method.
     */

    memcpy(g_curVftableVirtualDesktopSwitch, *(void **)pAnim, 18 * sizeof(size_t));
    g_curVftableVirtualDesktopSwitch[1] = (size_t)CVirtualDesktop__Initialize;
    *(void **)pAnim = (void *)g_curVftableVirtualDesktopSwitch;

    return pAnim;
}

bool HasDwminitWarningRecently()
{
    const WCHAR* queryPath = L"Application";
    const WCHAR* query =
        L"*[System[Provider[@Name='Dwminit'] and (Level=3) and "
        L"TimeCreated[timediff(@SystemTime) <= 10000]]]";

    EVT_HANDLE queryHandle = EvtQuery(nullptr,    // Local machine
                                      queryPath,  // Log path (Application log)
                                      query,      // Query
                                      EvtQueryChannelPath  // Query flags
    );

    if (!queryHandle)
    {
        Wh_Log(L"EvtQuery failed with error: %u", GetLastError());
        return false;
    }

    bool found = false;

    EVT_HANDLE eventHandle = nullptr;
    DWORD dwReturned = 0;
    constexpr DWORD kTimeout = 1000;

    if (EvtNext(queryHandle, 1, &eventHandle, kTimeout, 0, &dwReturned))
    {
        found = true;
        EvtClose(eventHandle);
    }
    else if (GetLastError() != ERROR_NO_MORE_ITEMS)
    {
        Wh_Log(L"EvtNext failed with error: %u", GetLastError());
    }

    EvtClose(queryHandle);
    return found;
}

EXTERN_C void CALLBACK MsgDwmStartupReject()
{
    MessageBoxW(
        nullptr,
        g_kNoInjectMsg,
        g_kMsgBoxTitle,
        MB_OK
    );
}

EXTERN_C void CALLBACK MsgWindows11Unsupported()
{
    MessageBoxW(
        nullptr,
        L"Windows 11 is not currently supported by this mod.",
        g_kMsgBoxTitle,
        MB_OK
    );
}

EXTERN_C void CALLBACK MsgWindowsVersionUnsupported()
{
    MessageBoxW(
        nullptr,
        L"This mod does not support versions of Windows prior to Windows 10.",
        g_kMsgBoxTitle,
        MB_OK
    );
}

EXTERN_C void CALLBACK MsgSymbolDownloadFailure()
{
    MessageBoxW(
        nullptr,
        L"Failed to install symbol hooks for uDWM.dll. Symbols may not be available "
        L"for your version of Windows yet, or there was likely an issue connecting to "
        L"the internet.",
        g_kMsgBoxTitle,
        MB_OK
    );
}

BOOL Wh_ModInit();

/**
 * Calling MessageBox during DWM startup can cause an infinite crash loop, so we have
 * to defer execution into a separate process.
 *
 * Additionally, rundll32 can fail to work for a couple seconds after DWM starts up.
 * I am not sure why this is, but in these cases, we need to additionally defer execution.
 *
 * @param szProcedureName  Name of the procedure in this module to execute.
 * @param fDeferExec       Defer execution via CMD
 */
void OpenSafeStartupMessageBox(LPCWSTR szProcedureName, bool fDeferExec = false)
{
    WCHAR szParameters[1024];
    WCHAR szPathSelf[MAX_PATH];
    HMODULE hModuleSelf = nullptr;

    if (!GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)Wh_ModInit,
        &hModuleSelf
    ))
    {
        return;
    }

    if (!GetModuleFileNameW(hModuleSelf, szPathSelf, sizeof(szPathSelf)))
    {
        return;
    }

    if (fDeferExec)
    {
        swprintf(szParameters, L"/k \"timeout 2 && rundll32.exe %s, %s && exit\"", szPathSelf, szProcedureName);
    }
    else
    {
        swprintf(szParameters, L"%s, %s", szPathSelf, szProcedureName);
    }

    ShellExecuteW(
        nullptr,
        L"open",
        fDeferExec ? L"cmd.exe" : L"rundll32.exe",
        szParameters,
        nullptr,
        SW_HIDE
    );
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    if (HasDwminitWarningRecently())
    {
        Wh_Log(L"%s", g_kNoInjectMsg);
        OpenSafeStartupMessageBox(L"MsgDwmStartupReject", true);
        return FALSE;
    }

    if (CVersionHelper::IsWindows11OrGreater())
    {
        OpenSafeStartupMessageBox(L"MsgWindows11Unsupported");
        return FALSE;
    }
    else if (!CVersionHelper::IsWindows10OrGreater())
    {
        OpenSafeStartupMessageBox(L"MsgWindowsVersionUnsupported");
        return FALSE;
    }

    Wh_Log(L"Init");

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        {
            {
                L"public: __cdecl CVirtualDesktopSwitch::CVirtualDesktopSwitch(void)"
            },
            (void **)&CVirtualDesktopSwitch__CVirtualDesktopSwitch_orig,
            (void *)CVirtualDesktopSwitch__CVirtualDesktopSwitch_hook
        }
    };

    if (!WindhawkUtils::HookSymbols(GetModuleHandleW(L"udwm.dll"), udwmDllHooks, ARRAYSIZE(udwmDllHooks)))
    {
        Wh_Log(L"Failed to successfully install all symbol hooks for uDWM.dll");
        
        OpenSafeStartupMessageBox(L"MsgSymbolDownloadFailure");
        return FALSE;
    };

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}
