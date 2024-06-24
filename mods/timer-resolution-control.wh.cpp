// ==WindhawkMod==
// @id           timer-resolution-control
// @name         Timer Resolution Control
// @description  Prevent programs from changing the Windows timer resolution and increasing power consumption
// @version      1.1
// @author       m417z, levitation
// @github       https://github.com/m417z
// @twitter      https://twitter.com/m417z
// @homepage     https://m417z.com/
// @include      *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Timer Resolution Control

The default timer resolution on Windows is 15.6 milliseconds. When programs increase the timer
frequency they increase power consumption and harm battery life. This mod provides configuration
to determine which programs are allowed to change the timer resolution.

More details:
[Windows Timer Resolution: Megawatts Wasted](https://randomascii.wordpress.com/2013/07/08/windows-timer-resolution-megawatts-wasted/)

The changes will apply immediately when the mod loads or when settings are changed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DefaultConfig: allow
  $name: Default configuration
  $description: Configuration for all programs, can be overridden for specific programs below
  $options:
  - allow: Allow changing the timer resolution
  - block: Disallow changing the timer resolution
  - limit: Limit changing the timer resolution
- DefaultLimit: 10
  $name: Default timer resolution limit (for the limit configuration)
  $description: The lowest possible delay between timer events, in milliseconds
- PerProgramConfig:
  - - Name: notepad.exe
      $name: Program name or path
    - Config: allow
      $name: Program configuration
      $options:
      - allow: Allow changing the timer resolution
      - block: Disallow changing the timer resolution
      - limit: Limit changing the timer resolution
    - Limit: 10
      $name: Program timer resolution limit (for the limit configuration)
  $name: Per-program configuration
*/
// ==/WindhawkModSettings==

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif


enum class Config {
    allow,
    block,
    limit
};

ULONG g_minimumResolution;
ULONG g_maximumResolution;
ULONG g_limitResolution;
ULONG g_lastDesiredResolution;

Config ConfigFromString(PCWSTR string) {
    if (wcscmp(string, L"block") == 0) {
        return Config::block;
    }

    if (wcscmp(string, L"limit") == 0) {
        return Config::limit;
    }

    return Config::allow;
}

typedef NTSTATUS (WINAPI *NtSetTimerResolution_t)(ULONG, BOOLEAN, PULONG);
NtSetTimerResolution_t pOriginalNtSetTimerResolution;

NTSTATUS WINAPI NtSetTimerResolutionHook(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution)
{
    if (!SetResolution) {
        Wh_Log(L"< SetResolution is FALSE");
        return pOriginalNtSetTimerResolution(DesiredResolution, SetResolution, CurrentResolution);
    }

    Wh_Log(L"> DesiredResolution: %f milliseconds", (double)DesiredResolution / 10000.0);

    g_lastDesiredResolution = DesiredResolution;

    ULONG limitResolution = g_limitResolution;
    if (DesiredResolution < limitResolution) {
        Wh_Log(L"* Overriding resolution: %f milliseconds", (double)limitResolution / 10000.0);
        DesiredResolution = limitResolution;
    }

    return pOriginalNtSetTimerResolution(DesiredResolution, SetResolution, CurrentResolution);
}

void LoadSettings(void)
{
    WCHAR programPath[1024];
    DWORD dwSize = ARRAYSIZE(programPath);
    if (!QueryFullProcessImageName(GetCurrentProcess(), 0, programPath, &dwSize)) {
        *programPath = L'\0';
    }

    PCWSTR programFileName = wcsrchr(programPath, L'\\');
    if (programFileName) {
        programFileName++;
        if (!*programFileName) {
            programFileName = nullptr;
        }
    }

    bool matched = false;
    Config config = Config::allow;
    int limit = 0;

    for (int i = 0; ; i++) {
        PCWSTR name = Wh_GetStringSetting(L"PerProgramConfig[%d].Name", i);
        bool hasName = *name;
        if (hasName) {
            if (programFileName && wcsicmp(programFileName, name) == 0) {
                matched = true;
            }
            else if (wcsicmp(programPath, name) == 0) {
                matched = true;
            }
        }

        Wh_FreeStringSetting(name);

        if (!hasName) {
            break;
        }

        if (matched) {
            PCWSTR configString = Wh_GetStringSetting(L"PerProgramConfig[%d].Config", i);
            config = ConfigFromString(configString);
            Wh_FreeStringSetting(configString);

            if (config == Config::limit) {
                limit = Wh_GetIntSetting(L"PerProgramConfig[%d].Limit", i);
            }

            break;
        }
    }

    if (!matched) {
        PCWSTR configString = Wh_GetStringSetting(L"DefaultConfig");
        config = ConfigFromString(configString);
        Wh_FreeStringSetting(configString);

        if (config == Config::limit) {
            limit = Wh_GetIntSetting(L"DefaultLimit");
        }
    }

    if (config == Config::block) {
        Wh_Log(L"Config loaded: Disallowing changes");
        g_limitResolution = g_minimumResolution;
    }
    else if (config == Config::limit) {
        ULONG limitResolution = limit * 10000;
        if (limitResolution > g_minimumResolution) {
            limitResolution = g_minimumResolution;
        }
        else if (limitResolution < g_maximumResolution) {
            limitResolution = g_maximumResolution;
        }

        Wh_Log(L"Config loaded: Limiting to %f milliseconds", (double)limitResolution / 10000.0);
        g_limitResolution = limitResolution;
    }
    else {
        Wh_Log(L"Config loaded: Allowing changes");
        g_limitResolution = g_maximumResolution;
    }
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
    if (!hNtdll) {
        return FALSE;
    }

    FARPROC pNtQueryTimerResolution = GetProcAddress(hNtdll, "NtQueryTimerResolution");
    if (!pNtQueryTimerResolution) {
        return FALSE;
    }

    FARPROC pNtSetTimerResolution = GetProcAddress(hNtdll, "NtSetTimerResolution");
    if (!pNtSetTimerResolution) {
        return FALSE;
    }

    ULONG MinimumResolution;
    ULONG MaximumResolution;
    ULONG CurrentResolution;
    NTSTATUS status = ((NTSTATUS(WINAPI*)(PULONG, PULONG, PULONG))pNtQueryTimerResolution)(
        &MinimumResolution, &MaximumResolution, &CurrentResolution);
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }

    Wh_Log(L"NtQueryTimerResolution: min=%f, max=%f, current=%f",
        (double)MinimumResolution / 10000.0,
        (double)MaximumResolution / 10000.0,
        (double)CurrentResolution / 10000.0);
    g_minimumResolution = MinimumResolution;
    g_maximumResolution = MaximumResolution;
    g_lastDesiredResolution = CurrentResolution;    //TODO: During mod load there is a small race condition where the hook is not yet set, the g_lastDesiredResolution is already set, and then the program tries to set a new desired resolution which will not be saved into g_lastDesiredResolution.

    LoadSettings();

    Wh_SetFunctionHook((void*)pNtSetTimerResolution, (void*)NtSetTimerResolutionHook, (void**)&pOriginalNtSetTimerResolution);

    return TRUE;
}

void EnforceLimits() {

    ULONG CurrentResolution;
    NTSTATUS status = pOriginalNtSetTimerResolution(0, FALSE, &CurrentResolution);
    if (NT_SUCCESS(status)) {
        Wh_Log(L"NtGetTimerResolution: current=%f", (double)CurrentResolution / 10000.0);

        ULONG limitResolution = g_limitResolution;
        ULONG lastDesiredResolution = g_lastDesiredResolution;
        if (CurrentResolution < limitResolution
            || CurrentResolution < lastDesiredResolution) {

            Wh_Log(L"> CurrentResolution: %f milliseconds", (double)CurrentResolution / 10000.0);

            if (lastDesiredResolution < limitResolution) {
                Wh_Log(L"* Overriding resolution: %f milliseconds", (double)limitResolution / 10000.0);
                pOriginalNtSetTimerResolution(limitResolution, TRUE, &CurrentResolution);
            }
            else {
                Wh_Log(L"* Restoring resolution: %f milliseconds", (double)lastDesiredResolution / 10000.0);
                pOriginalNtSetTimerResolution(lastDesiredResolution, TRUE, &CurrentResolution);
            }
        }
    }

    //TODO: for background programs set timer resolution to 100ms
}

void Wh_ModAfterInit(void) {  

    //Force timer resolution if it was already changed. NB! In order to avoid any race conditions, do this only after the hook is activated. Else there might be a situation that the mod overrides the current resolution while hook is not yet set. And then the program changes it again before the hook is finally set.

    EnforceLimits();
}

void Wh_ModSettingsChanged(void) {

    Wh_Log(L"SettingsChanged");

    LoadSettings();

    EnforceLimits();
}

void Wh_ModUninit() {

    Wh_Log(L"Uniniting...");

    //Lift all limits and restore original resolution set by the program
    ULONG lastDesiredResolution = g_lastDesiredResolution;
    Wh_Log(L"Restoring desired resolution: %f milliseconds", (double)lastDesiredResolution / 10000.0);
    ULONG CurrentResolution;
    pOriginalNtSetTimerResolution(g_lastDesiredResolution, TRUE, &CurrentResolution);
}

