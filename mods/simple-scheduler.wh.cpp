// ==WindhawkMod==
// @id              simple-scheduler
// @name            Simple Scheduler
// @description     Schedule hibernation, sleep, shutdown, program runs, notifications, screen state, and volume by time and day/date.
// @version         1.4.0
// @author          RadekPilich
// @github          https://github.com/RadekPilich
// @include         explorer.exe
// @compilerOptions -luser32 -lshell32 -lpowrprof -ladvapi32 -lole32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Simple Scheduler

A simple text based scheduler for scheduling system actions, program execution, and notifications based on time, day of the week, or specific calendar dates.

### --- SYNTAX GUIDE ---
Format: `Time | Days | Action | Payload`

* **Time:** `HH:MM` (24-hour format).
* **Days:** * `Any`: Runs every day.
    * `Mon-Sun`: Specific days (e.g., `Mon,Wed,Fri`).
    * `1-31`: Specific calendar dates (e.g., `1,15,30`).
    * **Ranges:** Supports wrap-around ranges for days and dates (e.g., `Fri-Tue` or `28-5`).
* **Toggle:** Add a `#` at the start of a line to disable that specific task.

### --- AVAILABLE ACTIONS ---
* **Notify:** Displays a system message box. Payload: Your message.
* **Run:** Launches a file or script. Supports `.ps1` files (runs hidden with bypass). Payload: Path.
* **Hibernate / Sleep:** Puts the system into the respective power state.
* **Shutdown / Restart:** Safely powers down or reboots the PC.
* **SignOut:** Logs out the current user session.
* **ScreenOff / ScreenOn:** Toggles monitor power state.
* **SoundOff / SoundOn:** Mutes or unmutes the default system audio endpoint.

### --- EXAMPLES ---
* `12:30 | Mon-Fri | Notify | Lunch time!`
* `19:25 | Fri-Sun | Notify | Family time! Computer going to sleep in 5 mins.`
* `19:30 | Fri-Sun | Sleep |`
* `23:00 | Any | SoundOff |`
* `10:45 | 15 | Notify | Mid-month backup reminder.`
* `# 02:00 | Sun | Restart |` (This task is disabled)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Tasks: |
    # ==================================================================
    # Format: HH:MM | Days | Action | Payload
    # ==================================================================
    15:30 | Any | Notify | Test notification from Windhawk Simple Scheduler mod
  $name: Scheduled Tasks
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <powrprof.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <regex>

enum class Action { Hibernate, Sleep, Shutdown, Restart, SignOut, Run, Notify, ScreenOff, ScreenOn, SoundOff, SoundOn };

struct Task {
    int hour;
    int minute;
    uint8_t weekDaysMask;   
    uint32_t monthDaysMask; 
    Action action;
    std::wstring payload;
    time_t lastExecuted;
};

struct NotifyPayload { 
    std::wstring message; 
};

// --- GLOBAL STATE ---
std::vector<Task> g_tasks;
HANDLE g_timer = nullptr;
HANDLE g_timerThread = nullptr;
HANDLE g_wakeEvent = nullptr;
bool g_exitFlag = false;

// --- UTILITIES ---
std::wstring Trim(const std::wstring& s) {
    const wchar_t* ws = L" \t\r\n\xA0";
    size_t start = s.find_first_not_of(ws);
    size_t end = s.find_last_not_of(ws);
    return (start == std::wstring::npos) ? L"" : s.substr(start, end - start + 1);
}

int GetDayIndex(const std::wstring& day) {
    std::wstring d = Trim(day);
    if (_wcsicmp(d.c_str(), L"Sun") == 0) return 0;
    if (_wcsicmp(d.c_str(), L"Mon") == 0) return 1;
    if (_wcsicmp(d.c_str(), L"Tue") == 0) return 2;
    if (_wcsicmp(d.c_str(), L"Wed") == 0) return 3;
    if (_wcsicmp(d.c_str(), L"Thu") == 0) return 4;
    if (_wcsicmp(d.c_str(), L"Fri") == 0) return 5;
    if (_wcsicmp(d.c_str(), L"Sat") == 0) return 6;
    return -1;
}

// --- PRIVILEGES & AUDIO ---
bool EnableShutdownPrivilege() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return false;
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    bool result = (GetLastError() == ERROR_SUCCESS);
    CloseHandle(hToken);
    return result;
}

void SetMuteState(bool mute) {
    IMMDeviceEnumerator *deviceEnumerator = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator))) {
        IMMDevice *defaultDevice = nullptr;
        if (SUCCEEDED(deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice))) {
            IAudioEndpointVolume *endpointVolume = nullptr;
            if (SUCCEEDED(defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID *)&endpointVolume))) {
                endpointVolume->SetMute(mute, nullptr);
                endpointVolume->Release();
            }
            defaultDevice->Release();
        }
        deviceEnumerator->Release();
    }
}

DWORD WINAPI NotifyThread(LPVOID param) {
    NotifyPayload* p = (NotifyPayload*)param;
    MessageBoxW(nullptr, p->message.c_str(), L"Simple Scheduler", MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_SYSTEMMODAL);
    delete p;
    return 0;
}

// --- EXECUTION LOGIC ---
void ExecuteAction(const Task& t) {
    Wh_Log(L"Executing task: %02d:%02d", t.hour, t.minute);
    switch (t.action) {
        case Action::Hibernate: SetSuspendState(TRUE, FALSE, FALSE); break;
        case Action::Sleep: SetSuspendState(FALSE, FALSE, FALSE); break;
        case Action::Shutdown: EnableShutdownPrivilege(); ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER); break;
        case Action::Restart: EnableShutdownPrivilege(); ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER); break;
        case Action::SignOut: ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER); break;
        case Action::ScreenOff: SendMessageTimeoutW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2, SMTO_ABORTIFHUNG, 100, nullptr); break;
        case Action::ScreenOn: SendMessageTimeoutW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, -1, SMTO_ABORTIFHUNG, 100, nullptr); mouse_event(MOUSEEVENTF_MOVE, 0, 0, 0, 0); break;
        case Action::SoundOff: SetMuteState(true); break;
        case Action::SoundOn: SetMuteState(false); break;
        case Action::Notify: {
            NotifyPayload* p = new NotifyPayload{t.payload};
            CloseHandle(CreateThread(nullptr, 0, NotifyThread, p, 0, nullptr));
            break;
        }
        case Action::Run:
            if (t.payload.size() >= 4 && _wcsicmp(t.payload.c_str() + t.payload.size() - 4, L".ps1") == 0) {
                std::wstring args = L"-WindowStyle Hidden -ExecutionPolicy Bypass -File \"" + t.payload + L"\"";
                ShellExecuteW(nullptr, L"open", L"powershell.exe", args.c_str(), nullptr, SW_HIDE);
            } else ShellExecuteW(nullptr, L"open", t.payload.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            break;
    }
}

// --- PARSING LOGIC ---
void ParseDays(const std::wstring& daysStr, uint8_t& weekMask, uint32_t& monthMask) {
    weekMask = 0; monthMask = 0;
    std::wstringstream ss(daysStr);
    std::wstring token;
    while (std::getline(ss, token, L',')) {
        token = Trim(token);
        if (token.empty()) continue;
        if (_wcsicmp(token.c_str(), L"Any") == 0) { weekMask = 0x7F; monthMask = 0xFFFFFFFF; return; }
        
        size_t hyphen = token.find(L'-');
        if (hyphen != std::wstring::npos) {
            std::wstring sPart = token.substr(0, hyphen), ePart = token.substr(hyphen + 1);
            int sW = GetDayIndex(sPart), eW = GetDayIndex(ePart);
            if (sW != -1 && eW != -1) {
                int i = sW;
                while (true) { weekMask |= (1 << (uint8_t)i); if (i == eW) break; i = (i + 1) % 7; }
            } else {
                try {
                    int sM = std::stoi(Trim(sPart)), eM = std::stoi(Trim(ePart));
                    int i = sM;
                    while (true) { if (i >= 1 && i <= 31) monthMask |= (1 << (uint32_t)i); if (i == eM) break; i = (i % 31) + 1; }
                } catch (...) {}
            }
        } else {
            int wIdx = GetDayIndex(token);
            if (wIdx != -1) weekMask |= (1 << (uint8_t)wIdx);
            else {
                try {
                    int dayNum = std::stoi(token);
                    if (dayNum >= 1 && dayNum <= 31) monthMask |= (1 << (uint32_t)dayNum);
                } catch (...) {}
            }
        }
    }
}

void LoadSettings() {
    g_tasks.clear();
    PCWSTR tasksStrRaw = Wh_GetStringSetting(L"Tasks");
    if (!tasksStrRaw) return;

    std::wstring raw(tasksStrRaw);
    Wh_FreeStringSetting(tasksStrRaw);

    // Regex pattern to recover potentially flattened strings
    std::wregex taskStartPattern(L"(\\d{1,2}:\\d{2}\\s*\\|)");
    std::wsregex_iterator it(raw.begin(), raw.end(), taskStartPattern);
    std::wsregex_iterator end;

    std::vector<size_t> splitPoints;
    for (; it != end; ++it) {
        size_t pos = it->position();
        size_t lineStart = pos;
        while (lineStart > 0 && raw[lineStart-1] != L'\n' && raw[lineStart-1] != L'\r') {
            lineStart--;
        }
        splitPoints.push_back(lineStart);
    }
    splitPoints.push_back(raw.length());

    for (size_t i = 0; i < splitPoints.size() - 1; ++i) {
        std::wstring line = raw.substr(splitPoints[i], splitPoints[i+1] - splitPoints[i]);
        std::wstring trimmed = Trim(line);
        
        if (trimmed.empty() || trimmed[0] == L'#') continue;

        std::wstringstream ls(trimmed);
        std::wstring tP, dP, aP, pP;
        if (!std::getline(ls, tP, L'|')) continue;
        if (!std::getline(ls, dP, L'|')) continue;
        if (!std::getline(ls, aP, L'|')) continue;
        std::getline(ls, pP);

        Task t = {};
        if (swscanf_s(Trim(tP).c_str(), L"%d:%d", &t.hour, &t.minute) != 2) continue;
        ParseDays(dP, t.weekDaysMask, t.monthDaysMask);
        t.payload = Trim(pP);
        t.lastExecuted = 0;

        std::wstring act = Trim(aP);
        if (_wcsicmp(act.c_str(), L"Hibernate") == 0) t.action = Action::Hibernate;
        else if (_wcsicmp(act.c_str(), L"Sleep") == 0) t.action = Action::Sleep;
        else if (_wcsicmp(act.c_str(), L"Shutdown") == 0) t.action = Action::Shutdown;
        else if (_wcsicmp(act.c_str(), L"Restart") == 0) t.action = Action::Restart;
        else if (_wcsicmp(act.c_str(), L"SignOut") == 0) t.action = Action::SignOut;
        else if (_wcsicmp(act.c_str(), L"Run") == 0) t.action = Action::Run;
        else if (_wcsicmp(act.c_str(), L"Notify") == 0) t.action = Action::Notify;
        else if (_wcsicmp(act.c_str(), L"ScreenOff") == 0) t.action = Action::ScreenOff;
        else if (_wcsicmp(act.c_str(), L"ScreenOn") == 0) t.action = Action::ScreenOn;
        else if (_wcsicmp(act.c_str(), L"SoundOff") == 0) t.action = Action::SoundOff;
        else if (_wcsicmp(act.c_str(), L"SoundOn") == 0) t.action = Action::SoundOn;
        else continue;

        g_tasks.push_back(t);
    }
    Wh_Log(L"Parser: Loaded %zu active tasks.", g_tasks.size());
}

// --- THREADING ---
DWORD WINAPI SchedulerThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    HANDLE handles[] = { g_wakeEvent, g_timer };
    while (!g_exitFlag) {
        time_t now = time(nullptr);
        time_t next_run = 0;
        std::vector<Task*> due_tasks;
        struct tm tm_now; localtime_s(&tm_now, &now);

        for (auto& t : g_tasks) {
            struct tm tm_t = tm_now;
            tm_t.tm_hour = t.hour; tm_t.tm_min = t.minute; tm_t.tm_sec = 0; tm_t.tm_isdst = -1;
            time_t target_today = mktime(&tm_t);
            bool validDay = ((t.weekDaysMask & (1 << tm_now.tm_wday)) != 0) || ((t.monthDaysMask & (1 << tm_now.tm_mday)) != 0);
            bool inWindow = (now >= target_today && now < target_today + 60);
            if (validDay && inWindow && (t.lastExecuted < target_today)) {
                due_tasks.push_back(&t);
                t.lastExecuted = now;
            }
            time_t next_target = target_today;
            if (now >= target_today + 60 || (inWindow && t.lastExecuted >= target_today)) {
                tm_t.tm_mday += 1; next_target = mktime(&tm_t);
            }
            struct tm tm_c; localtime_s(&tm_c, &next_target);
            while (!(((t.weekDaysMask & (1 << tm_c.tm_wday)) != 0) || ((t.monthDaysMask & (1 << tm_c.tm_mday)) != 0))) {
                tm_t.tm_mday += 1; next_target = mktime(&tm_t); localtime_s(&tm_c, &next_target);
            }
            if (next_run == 0 || next_target < next_run) next_run = next_target;
        }

        for (Task* t : due_tasks) ExecuteAction(*t);

        if (next_run > 0) {
            LARGE_INTEGER due; due.QuadPart = (LONGLONG)next_run * 10000000LL + 116444736000000000LL;
            SetWaitableTimer(g_timer, &due, 0, nullptr, nullptr, FALSE);
        }
        MsgWaitForMultipleObjects(2, handles, FALSE, INFINITE, QS_ALLEVENTS);
    }
    CoUninitialize();
    return 0;
}

static ULONG WINAPI PowerCallback(PVOID Context, ULONG Type, PVOID Setting) {
    if (Type == 7) SetEvent(g_wakeEvent);
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Tool process started.");
    LoadSettings();
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_timer = CreateWaitableTimerW(nullptr, TRUE, nullptr);
    
    HMODULE hP = GetModuleHandle(L"powrprof.dll");
    if (hP) {
        typedef HPOWERNOTIFY (WINAPI *Reg_t)(DWORD, PVOID, PHPOWERNOTIFY);
        auto Reg = (Reg_t)GetProcAddress(hP, "PowerRegisterSuspendResumeNotification");
        if (Reg) {
            static struct { PVOID c; PVOID ctx; } p = { (PVOID)PowerCallback, g_wakeEvent };
            HPOWERNOTIFY hn; Reg(2, &p, &hn);
        }
    }
    g_timerThread = CreateThread(nullptr, 0, SchedulerThread, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() { LoadSettings(); SetEvent(g_wakeEvent); }
void WhTool_ModUninit() { g_exitFlag = true; SetEvent(g_wakeEvent); if (g_timerThread) { WaitForSingleObject(g_timerThread, INFINITE); CloseHandle(g_timerThread); } CloseHandle(g_timer); CloseHandle(g_wakeEvent); }

// --- BOILERPLATE ---
bool g_isToolModProcessLauncher;
HANDLE g_childProcessHandle;
void WINAPI EntryPoint_Hook() { ExitThread(0); }

BOOL Wh_ModInit() {
    int argc; LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    bool isToolMod = false, isCurr = false;
    for (int i = 0; i < argc; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0 && i + 1 < argc) {
            isToolMod = true;
            if (wcscmp(argv[i+1], WH_MOD_ID) == 0) isCurr = true;
        }
    }
    LocalFree(argv);

    if (isCurr) {
        HANDLE mutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (GetLastError() == ERROR_ALREADY_EXISTS) { if (mutex) CloseHandle(mutex); ExitProcess(1); }
        if (!WhTool_ModInit()) { if (mutex) CloseHandle(mutex); ExitProcess(1); }
        IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)((BYTE*)dos + dos->e_lfanew);
        Wh_SetFunctionHook((BYTE*)dos + nt->OptionalHeader.AddressOfEntryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }
    if (isToolMod) return FALSE;

    HWND hwndShell = GetShellWindow();
    if (hwndShell) {
        DWORD dwShellPid; GetWindowThreadProcessId(hwndShell, &dwShellPid);
        if (GetCurrentProcessId() != dwShellPid) return FALSE;
    }
    g_isToolModProcessLauncher = true; 
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;
    WCHAR path[MAX_PATH]; GetModuleFileName(nullptr, path, MAX_PATH);
    WCHAR cmd[MAX_PATH + 128]; swprintf_s(cmd, L"\"%s\" -tool-mod \"%s\"", path, WH_MOD_ID);
    STARTUPINFO si = { sizeof(si) };
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    HMODULE hK = GetModuleHandle(L"kernelbase.dll"); if (!hK) hK = GetModuleHandle(L"kernel32.dll");
    using CP_t = BOOL(WINAPI*)(HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    auto pCP = (CP_t)GetProcAddress(hK, "CreateProcessInternalW");

    HANDLE hJob = CreateJobObject(nullptr, nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { { 0 } };
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));

    if (pCP(nullptr, path, cmd, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, nullptr, nullptr, &si, &pi, nullptr)) {
        AssignProcessToJobObject(hJob, pi.hProcess);
        ResumeThread(pi.hThread);
        g_childProcessHandle = pi.hProcess;
        CloseHandle(pi.hThread);
    }
}

void Wh_ModSettingsChanged() { if (!g_isToolModProcessLauncher) WhTool_ModSettingsChanged(); }
void Wh_ModUninit() { 
    if (!g_isToolModProcessLauncher) { 
        WhTool_ModUninit(); 
        ExitProcess(0); 
    } else {
        if (g_childProcessHandle) { TerminateProcess(g_childProcessHandle, 0); CloseHandle(g_childProcessHandle); }
    }
}
