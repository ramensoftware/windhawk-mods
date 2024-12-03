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
// @compilerOptions -lkernel32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Virtual Desktop Transition Animation

This mod disables the animation seen when switching between virtual desktops in Windows 10.

In order to use this mod, you have to hook into `dwm.exe`. In order to do this, you must go into Windhawk advanced settings
and remove the exclusion entry for `<critical_system_processes>`.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windhawk_utils.h>

LPCWSTR g_kMsgBoxTitle = L"Disable Virtual Desktop Transition Animation (Windhawk mod)";

LPCWSTR g_kNoInjectMsg =
    L"DWM was unexpected restarted. The mod will not be reinjected to avoid a potential "
    L"crash loop. Please disable and re-eanble the mod to force reinjection.";

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

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    bool fAlreadyRunning = !(*(USHORT*)(((long long)NtCurrentTeb()) + 0x17EE) & 0x0400);

    if (!fAlreadyRunning)
    {
        Wh_Log(L"%s", g_kNoInjectMsg);
        MessageBoxW(
            nullptr,
            g_kNoInjectMsg,
            g_kMsgBoxTitle,
            MB_OK
        );
        return FALSE;
    }

    if (CVersionHelper::IsWindows11OrGreater())
    {
        MessageBoxW(
            nullptr,
            L"Windows 11 is not currently supported by this mod.",
            g_kMsgBoxTitle,
            MB_OK
        );
        return FALSE;
    }
    else if (!CVersionHelper::IsWindows10OrGreater())
    {
        MessageBoxW(
            nullptr,
            L"This mod does not support versions of Windows prior to Windows 10.",
            g_kMsgBoxTitle,
            MB_OK
        );
        return FALSE;
    }

    Wh_Log(L"Init");

    // uDWM.dll
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

        MessageBoxW(
            nullptr,
            L"Failed to install symbol hooks for uDWM.dll. Symbols may not be available "
            L"for your version of Windows yet, or there was likely an issue connecting to "
            L"the internet.",
            g_kMsgBoxTitle,
            MB_OK
        );

        return FALSE;
    };

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}
