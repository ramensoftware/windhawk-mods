// ==WindhawkMod==
// @id              simple-scheduler
// @name            Simple Scheduler
// @description     Schedule hibernation, sleep, shutdown, programs, notifications, screen state, and volume by time and day.
// @version         1.3.0
// @author          RadekPilich
// @github          https://github.com/RadekPilich
// @include         explorer.exe
// @compilerOptions -luser32 -lshell32 -lpowrprof -ladvapi32 -lole32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Simple Scheduler

Schedule hibernation, sleep, shutdown, programs, notifications, screen state, and volume by time and day.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Tasks: |
    # --- SYNTAX GUIDE ---
    # Format:  Time | Days | Action | Payload
    # Time:    HH:MM (24-hour format)
    # Days:    Any, Mon-Sun, or dates 1-31 (comma-separated, e.g., Mon,15,31)
    # Action:  Notify, Run, Hibernate, Sleep, Shutdown, Restart, SignOut,
    #          ScreenOff, ScreenOn, SoundOff, SoundOn
    # Disable: Add a # at the start of a line to disable/toggle that specific task
    # --- EXAMPLES ---
    # 19:40 | Mon,Tue,Wed,Thu,Fri | Sleep | 
    # 23:00 | Any | ScreenOff | 
    # 02:00 | Sun | Restart | 
    # ==============================================================================

    # ================================================================== DAILY

    # ================================================================== WEEKLY

    # ================================================================== MONTHLY

  $name: Scheduled Tasks
  $description: "Edit settings in textual mode. Syntax and actions are described in the comment box."
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

enum class Action { Hibernate, Sleep, Shutdown, Restart, SignOut, Run, Notify, ScreenOff, ScreenOn, SoundOff, SoundOn };

struct Task {
    int hour;
    int minute;
    uint8_t weekDaysMask;   // Bit 0=Sun, 1=Mon...
    uint32_t monthDaysMask; // Bit 1=1st, 2=2nd...
    Action action;
    std::wstring payload;
    time_t lastExecuted;
};

std::vector<Task> g_tasks;
HANDLE g_timer = nullptr;
HANDLE g_timerThread = nullptr;
HANDLE g_wakeEvent = nullptr;
bool g_exitFlag = false;

std::wstring Trim(const std::wstring& s) {
    size_t start = s.find_first_not_of(L" \t\r\n");
    size_t end = s.find_last_not_of(L" \t\r\n");
    return (start == std::wstring::npos) ? L"" : s.substr(start, end - start + 1);
}

void ParseDays(const std::wstring& daysStr, uint8_t& weekMask, uint32_t& monthMask) {
    weekMask = 0;
    monthMask = 0;
    std::wstringstream ss(daysStr);
    std::wstring token;
    
    while (std::getline(ss, token, L',')) {
        token = Trim(token);
        if (token.empty()) continue;
        
        if (_wcsicmp(token.c_str(), L"Any") == 0) { weekMask = 0x7F; monthMask = 0xFFFFFFFF; return; }
        else if (_wcsicmp(token.c_str(), L"Sun") == 0) weekMask |= (1 << 0);
        else if (_wcsicmp(token.c_str(), L"Mon") == 0) weekMask |= (1 << 1);
        else if (_wcsicmp(token.c_str(), L"Tue") == 0) weekMask |= (1 << 2);
        else if (_wcsicmp(token.c_str(), L"Wed") == 0) weekMask |= (1 << 3);
        else if (_wcsicmp(token.c_str(), L"Thu") == 0) weekMask |= (1 << 4);
        else if (_wcsicmp(token.c_str(), L"Fri") == 0) weekMask |= (1 << 5);
        else if (_wcsicmp(token.c_str(), L"Sat") == 0) weekMask |= (1 << 6);
        else {
            try {
                int dayNum = std::stoi(token);
                if (dayNum >= 1 && dayNum <= 31) monthMask |= (1 << dayNum);
            } catch (...) {} // Ignore invalid numbers
        }
    }
}

void LoadSettings() {
    g_tasks.clear();
    PCWSTR tasksStr = Wh_GetStringSetting(L"Tasks");
    if (!tasksStr) return;

    std::wstringstream wss(tasksStr);
    std::wstring line;
    while (std::getline(wss, line)) {
        std::wstring trimmedLine = Trim(line);
        if (trimmedLine.empty() || trimmedLine[0] == L'#') continue;

        std::wstringstream lineStream(line);
        std::wstring timeStr, daysStr, actionStr, payloadStr;
        
        std::getline(lineStream, timeStr, L'|');
        std::getline(lineStream, daysStr, L'|');
        std::getline(lineStream, actionStr, L'|');
        std::getline(lineStream, payloadStr);

        timeStr = Trim(timeStr);
        daysStr = Trim(daysStr);
        actionStr = Trim(actionStr);
        payloadStr = Trim(payloadStr);

        Task t = {};
        if (swscanf_s(timeStr.c_str(), L"%d:%d", &t.hour, &t.minute) != 2) continue;
        
        ParseDays(daysStr, t.weekDaysMask, t.monthDaysMask);
        t.payload = payloadStr;
        t.lastExecuted = 0;

        if (_wcsicmp(actionStr.c_str(), L"Hibernate") == 0) t.action = Action::Hibernate;
        else if (_wcsicmp(actionStr.c_str(), L"Sleep") == 0) t.action = Action::Sleep;
        else if (_wcsicmp(actionStr.c_str(), L"Shutdown") == 0) t.action = Action::Shutdown;
        else if (_wcsicmp(actionStr.c_str(), L"Restart") == 0) t.action = Action::Restart;
        else if (_wcsicmp(actionStr.c_str(), L"SignOut") == 0) t.action = Action::SignOut;
        else if (_wcsicmp(actionStr.c_str(), L"Run") == 0) t.action = Action::Run;
        else if (_wcsicmp(actionStr.c_str(), L"Notify") == 0) t.action = Action::Notify;
        else if (_wcsicmp(actionStr.c_str(), L"ScreenOff") == 0) t.action = Action::ScreenOff;
        else if (_wcsicmp(actionStr.c_str(), L"ScreenOn") == 0) t.action = Action::ScreenOn;
        else if (_wcsicmp(actionStr.c_str(), L"SoundOff") == 0) t.action = Action::SoundOff;
        else if (_wcsicmp(actionStr.c_str(), L"SoundOn") == 0) t.action = Action::SoundOn;
        else continue;

        g_tasks.push_back(t);
    }
    Wh_FreeStringSetting(tasksStr);
}

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
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    if (FAILED(hr)) return;

    IMMDevice *defaultDevice = nullptr;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    if (SUCCEEDED(hr)) {
        IAudioEndpointVolume *endpointVolume = nullptr;
        hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID *)&endpointVolume);
        if (SUCCEEDED(hr)) {
            endpointVolume->SetMute(mute, nullptr);
            endpointVolume->Release();
        }
        defaultDevice->Release();
    }
    deviceEnumerator->Release();
}

struct NotifyPayload { std::wstring message; };

DWORD WINAPI NotifyThread(LPVOID param) {
    NotifyPayload* p = (NotifyPayload*)param;
    MessageBoxW(nullptr, p->message.c_str(), L"Simple Scheduler", MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_SYSTEMMODAL);
    delete p;
    return 0;
}

void ExecuteAction(const Task& t) {
    switch (t.action) {
        case Action::Hibernate:
            SetSuspendState(TRUE, FALSE, FALSE);
            break;
        case Action::Sleep:
            SetSuspendState(FALSE, FALSE, FALSE);
            break;
        case Action::Shutdown:
            EnableShutdownPrivilege();
            ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
            break;
        case Action::Restart:
            EnableShutdownPrivilege();
            ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
            break;
        case Action::SignOut:
            ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
            break;
        case Action::ScreenOff:
            SendMessageTimeoutW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2, SMTO_ABORTIFHUNG, 100, nullptr);
            break;
        case Action::ScreenOn:
            // Wake monitor using broadcast and a simulated mouse movement
            SendMessageTimeoutW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, -1, SMTO_ABORTIFHUNG, 100, nullptr);
            mouse_event(MOUSEEVENTF_MOVE, 0, 0, 0, 0); 
            break;
        case Action::SoundOff:
            SetMuteState(true);
            break;
        case Action::SoundOn:
            SetMuteState(false);
            break;
        case Action::Notify: {
            NotifyPayload* p = new NotifyPayload{t.payload};
            CloseHandle(CreateThread(nullptr, 0, NotifyThread, p, 0, nullptr));
            break;
        }
        case Action::Run:
            if (!t.payload.empty()) {
                if (t.payload.size() >= 4 && _wcsicmp(t.payload.c_str() + t.payload.size() - 4, L".ps1") == 0) {
                    std::wstring args = L"-WindowStyle Hidden -ExecutionPolicy Bypass -File \"" + t.payload + L"\"";
                    ShellExecuteW(nullptr, L"open", L"powershell.exe", args.c_str(), nullptr, SW_HIDE);
                } else {
                    ShellExecuteW(nullptr, L"open", t.payload.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                }
            }
            break;
    }
}

DWORD WINAPI SchedulerThread(LPVOID) {
    // Initialize COM for the audio endpoint manipulation
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    HANDLE handles[] = { g_wakeEvent, g_timer };

    while (!g_exitFlag) {
        time_t now = time(nullptr);
        time_t next_run = 0;
        std::vector<Task*> due_tasks;

        struct tm tm_now;
        localtime_s(&tm_now, &now);

        for (auto& t : g_tasks) {
            struct tm tm_target = tm_now;
            tm_target.tm_hour = t.hour;
            tm_target.tm_min = t.minute;
            tm_target.tm_sec = 0;
            tm_target.tm_isdst = -1; 
            
            time_t target_today = mktime(&tm_target);
            
            // Validate if today matches either the weekday bitmask OR the calendar date bitmask
            bool validDay = ((t.weekDaysMask & (1 << tm_now.tm_wday)) != 0) || 
                            ((t.monthDaysMask & (1 << tm_now.tm_mday)) != 0);
            
            bool inExecutionWindow = (now >= target_today && now < target_today + 60);
            bool triggeredToday = (t.lastExecuted >= target_today);

            if (validDay && inExecutionWindow && !triggeredToday) {
                due_tasks.push_back(&t);
                t.lastExecuted = now; 
            }

            struct tm tm_next = tm_now;
            tm_next.tm_hour = t.hour;
            tm_next.tm_min = t.minute;
            tm_next.tm_sec = 0;
            tm_next.tm_isdst = -1;
            time_t next_target = mktime(&tm_next);

            if (now >= next_target + 60 || (inExecutionWindow && triggeredToday)) {
                tm_next.tm_mday += 1;
                next_target = mktime(&tm_next);
            }

            struct tm tm_check;
            localtime_s(&tm_check, &next_target);
            while (((t.weekDaysMask & (1 << tm_check.tm_wday)) == 0) &&
                   ((t.monthDaysMask & (1 << tm_check.tm_mday)) == 0)) {
                tm_next.tm_mday += 1;
                next_target = mktime(&tm_next);
                localtime_s(&tm_check, &next_target);
            }

            if (next_run == 0 || next_target < next_run) {
                next_run = next_target;
            }
        }

        for (Task* t : due_tasks) {
            ExecuteAction(*t);
        }

        if (next_run > 0) {
            LARGE_INTEGER due;
            due.QuadPart = (LONGLONG)next_run * 10000000LL + 116444736000000000LL;
            SetWaitableTimer(g_timer, &due, 0, nullptr, nullptr, FALSE);
        } else {
            CancelWaitableTimer(g_timer);
        }

        MsgWaitForMultipleObjects(2, handles, FALSE, INFINITE, QS_ALLEVENTS);
    }

    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_timer = CreateWaitableTimerW(nullptr, TRUE, nullptr);
    g_timerThread = CreateThread(nullptr, 0, SchedulerThread, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    SetEvent(g_wakeEvent);
}

void WhTool_ModUninit() {
    g_exitFlag = true;
    SetEvent(g_wakeEvent);
    if (g_timerThread) {
        WaitForSingleObject(g_timerThread, INFINITE);
        CloseHandle(g_timerThread);
    }
    if (g_timer) CloseHandle(g_timer);
    if (g_wakeEvent) CloseHandle(g_wakeEvent);
}

// ==============================================================================
// TOOL MOD LAUNCHER CODE
// ==============================================================================
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() { 
    ExitThread(0); 
}

BOOL Wh_ModInit() {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) return FALSE;

    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
        }
    }

    LocalFree(argv);

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        void* entryPoint = (BYTE*)dosHeader + ntHeaders->OptionalHeader.AddressOfEntryPoint;
        
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath));
    
    WCHAR commandLine[MAX_PATH + 50];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, 
        WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    
    auto pCreateProcessInternalW = (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");

    STARTUPINFO si{ .cb = sizeof(STARTUPINFO), .dwFlags = STARTF_FORCEOFFFEEDBACK };
    PROCESS_INFORMATION pi;
    
    pCreateProcessInternalW(
        nullptr, currentProcessPath, commandLine, nullptr, nullptr, 
        FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr);
        
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() { 
    if (!g_isToolModProcessLauncher) {
        WhTool_ModSettingsChanged(); 
    }
}

void Wh_ModUninit() { 
    if (!g_isToolModProcessLauncher) { 
        WhTool_ModUninit(); 
        ExitProcess(0); 
    } 
}
