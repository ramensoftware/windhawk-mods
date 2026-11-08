    // ==WindhawkMod==
    // @id              window-centering-suite
    // @name            Window Centering Suite
    // @description     Smooth, refresh-rate-aware window centering with a customizable hotkey. Requires Windhawk 2.0 Alpha 4+.
    // @version         1.0.0
    // @author          Barfunkle999
    // @github          https://github.com/Barfunkle999
    // @include         windhawk.exe
    // @compilerOptions -luser32 -lkernel32
    // ==/WindhawkMod==

    // ==WindhawkModReadme==
    /*
    # Window Centering Suite

    > **Requires Windhawk 2.0 Alpha 4 or newer.** Windhawk 1.x is not supported.

    Window Centering Suite automatically centers supported windows on the monitor
    where they open, using smooth animations that adapt to the monitor's refresh
    rate. It also provides a customizable shortcut for centering the active window.

    ## Features

    - **Automatic centering:** Centers eligible application windows when they open.
    - **Explorer file operations:** Centers Windows file copy, move, and delete
    progress dialogs, including after expanding or collapsing their details.
    - **Manual centering:** Press **Alt+X** by default to center the active window.
    - **Multi-monitor support:** Centers each window in its monitor's work area.
    - **Exclusions:** Skip selected window titles or executable names, with the
    option to pause centering while an excluded application is running.

    ## Shortcut configuration

    In **Settings**, enter a combination in **Center hotkey** and save. Examples:
    `Alt+X`, `F8`, `Ctrl+Shift+C`, or `Ctrl+Alt+Shift+F9`.

    Shortcuts support one primary key, optionally combined with Ctrl, Alt, Shift,
    and/or Win. Enter `Off` to disable manual centering.

    ## Compatibility

    This mod runs as a dedicated Windhawk tool mod; no companion application or
    AutoHotkey installation is needed. It requires Windhawk 2.0 Alpha 4 or newer.

    The mod uses out-of-context Windows event notifications and does not itself
    inject code into the applications it centers. Windhawk's separate injection
    settings still apply. Some elevated or protected applications may restrict
    external window positioning. For anti-cheat-protected games, configure
    Windhawk's process exclusions as appropriate; compatibility is not guaranteed.

    Source contributions: ChatGPT (OpenAI).
    */
    // ==/WindhawkModReadme==

    // ==WindhawkModSettings==
    /*
    - AutoCenter: true
    $name: Automatic centering
    $description: >-
        Automatically centers new windows on the monitor where they open, including
        Explorer's file copy, move, and delete progress windows.
    - CenterHotkey: "Alt+X"
    $name: Center hotkey
    $description: >-
        Type a key combination to center the active window, then save Settings.
        Examples: Alt+X, F8, Ctrl+Shift+C, Ctrl+Alt+Shift+F9.
        Type Off to disable manual centering. Default: Alt+X.
    - ExcludedWindowTitles: ""
    $name: Excluded window titles
    $description: >-
        Window titles containing any of these words won't be centered. Separate
        multiple entries with semicolons. Matching is not case-sensitive.
    - ExcludedApplications: ""
    $name: Excluded applications
    $description: >-
        Applications whose windows shouldn't be centered. Enter executable names
        (such as game.exe), separated with semicolons. Leave empty for no exclusions.
    - PauseWhileExcludedAppsRun: false
    $name: Pause while excluded applications run
    $description: >-
        Stops all automatic and manual centering while any application listed above
        is running. Turn off to exclude only those applications' own windows.
    */
    // ==/WindhawkModSettings==

    #ifndef UNICODE
    #define UNICODE
    #endif
    #ifndef _UNICODE
    #define _UNICODE
    #endif

    #include <windows.h>
    #include <tlhelp32.h>
    #include <windhawk_tool_mod.h>

    #include <algorithm>
    #include <cmath>
    #include <cwchar>
    #include <cwctype>
    #include <string>
    #include <unordered_map>
    #include <unordered_set>
    #include <vector>
    #include <mutex>
    #include <utility>
    #include <atomic>
    #include <limits>

    struct Options {
        bool autoCenter = true;
        std::wstring manualCenterHotkey = L"Alt+X";
        bool pauseWhileExcludedRuns = false;
        int animationMs = 180;  // Constant wall-clock duration on every monitor.
        bool originalEaseOut = false;
        int foregroundFallbackMs = 300;  // Internal missed-event recovery.
        std::vector<std::wstring> excludeTitles;
        std::vector<std::wstring> excludeProcesses;
    };

    struct Pending {
        ULONGLONG due = 0;
        SIZE lastSize{};
        int resizes = 0;
        bool transfer = false;
    };

    static std::mutex g_optionsMutex;
    static Options g_options;
    static std::unordered_map<HWND, Pending> g_pending;
    static std::unordered_set<HWND> g_seenRegular;
    static std::unordered_map<HWND, SIZE> g_transferSizes;
    static std::vector<HWINEVENTHOOK> g_hooks;
    static std::atomic<DWORD> g_eventThreadId{0};
    static UINT_PTR g_timer = 0;
    static bool g_hotkeyRegistered = false;
    static UINT g_registeredModifiers = 0;
    static UINT g_registeredVk = 0;
    static bool g_pauseForProcess = false;
    static ULONGLONG g_lastForegroundScan = 0;
    static ULONGLONG g_lastTransferScan = 0;
    static ULONGLONG g_lastProcessScan = 0;
    static constexpr int kHotkeyId = 991;
    // Explorer transfers are included in automatic centering. The targeted
    // fallback stays internal so users need only configure centering itself.
    static constexpr ULONGLONG kTransferScanMs = 120;

    // Animations run on their own thread. The WinEvent/message thread remains free
    // to discover newly created windows while another window is being animated.
    struct Animation {
        HWND hwnd{};
        int startX{}, startY{}, targetX{}, targetY{};
        int durationMs{};
        bool originalEaseOut{};
        LONGLONG startedQpc{};
        LONGLONG deadlineQpc{};
        LONGLONG nextFrameQpc{};
        LONGLONG framePeriodQpc{};
    };

    static std::mutex g_animationMutex;
    static std::unordered_map<HWND, Animation> g_animations;
    static HANDLE g_animationWake = nullptr;
    static HANDLE g_animationThread = nullptr;
    static std::atomic<bool> g_animationStop{false};
    static LARGE_INTEGER g_qpcFrequency{};

    #ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
    #define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
    #endif

    static std::wstring Lower(std::wstring s) {
        std::transform(s.begin(), s.end(), s.begin(),
                    [](wchar_t c) { return (wchar_t)towlower(c); });
        return s;
    }

    static std::vector<std::wstring> ParseNames(const std::wstring& source) {
        std::vector<std::wstring> out;
        size_t pos = 0;
        while (pos < source.size()) {
            size_t end = source.find_first_of(L";,\n", pos);
            if (end == std::wstring::npos) end = source.size();
            std::wstring item = source.substr(pos, end - pos);
            size_t first = item.find_first_not_of(L" \t\r");
            size_t last = item.find_last_not_of(L" \t\r");
            if (first != std::wstring::npos) {
                item = Lower(item.substr(first, last - first + 1));
                if (!item.empty()) out.push_back(item);
            }
            pos = end + 1;
        }
        return out;
    }

    static std::wstring ReadSetting(PCWSTR name) {
        PCWSTR value = Wh_GetStringSetting(name);
        std::wstring result = value ? value : L"";
        Wh_FreeStringSetting(value);
        return result;
    }

    static Options LoadOptions() {
        Options o;
        o.autoCenter = Wh_GetIntSetting(L"AutoCenter") != 0;
        o.manualCenterHotkey = ReadSetting(L"CenterHotkey");
        o.pauseWhileExcludedRuns = Wh_GetIntSetting(L"PauseWhileExcludedAppsRun") != 0;
        o.excludeTitles = ParseNames(ReadSetting(L"ExcludedWindowTitles"));
        o.excludeProcesses = ParseNames(ReadSetting(L"ExcludedApplications"));
        return o;
    }

    static Options GetOptions() {
        std::lock_guard<std::mutex> lock(g_optionsMutex);
        return g_options;
    }

    static std::wstring ExecutableNameForWindow(HWND hwnd) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (!pid) return L"";
        HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!process) return L"";
        WCHAR path[32768];
        DWORD size = ARRAYSIZE(path);
        std::wstring result;
        if (QueryFullProcessImageNameW(process, 0, path, &size)) {
            const wchar_t* last = wcsrchr(path, L'\\');
            result = Lower(last ? last + 1 : path);
        }
        CloseHandle(process);
        return result;
    }

    static bool IsExcludedName(const std::wstring& name,
                            const std::vector<std::wstring>& excluded) {
        for (const auto& candidate : excluded) {
            if (name == candidate) return true;
            // Accept "explorer" as well as "explorer.exe".
            if (candidate.size() > 4 && candidate.substr(candidate.size() - 4) == L".exe" &&
                name == candidate.substr(0, candidate.size() - 4)) return true;
            if (name.size() > 4 && name.substr(name.size() - 4) == L".exe" &&
                name.substr(0, name.size() - 4) == candidate) return true;
        }
        return false;
    }

    static bool Excluded(HWND hwnd, const Options& o) {
        WCHAR title[2048]{};
        GetWindowTextW(hwnd, title, ARRAYSIZE(title));
        std::wstring lowerTitle = Lower(title);
        for (const auto& fragment : o.excludeTitles) {
            if (!fragment.empty() && lowerTitle.find(fragment) != std::wstring::npos)
                return true;
        }
        if (!o.excludeProcesses.empty()) {
            std::wstring executable = ExecutableNameForWindow(hwnd);
            if (IsExcludedName(executable, o.excludeProcesses)) return true;
        }
        return false;
    }

    static bool IsTransfer(HWND hwnd) {
        if (!IsWindow(hwnd)) return false;
        WCHAR className[128]{};
        return GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
            wcscmp(className, L"OperationStatusWindow") == 0;
    }

    static bool IsTopLevelVisible(HWND hwnd) {
        return hwnd && IsWindow(hwnd) && GetAncestor(hwnd, GA_ROOT) == hwnd &&
            IsWindowVisible(hwnd) && !IsIconic(hwnd) && !IsZoomed(hwnd);
    }

    static bool IsRegularEligible(HWND hwnd) {
        if (!IsTopLevelVisible(hwnd)) return false;
        LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        // Normal application windows require a system menu. Transfer dialogs
        // have their own eligibility path.
        return (style & WS_SYSMENU) && !(exStyle & WS_EX_TOOLWINDOW);
    }

    static bool HasExcludedProcess(const Options& o) {
        if (!o.pauseWhileExcludedRuns || o.excludeProcesses.empty()) return false;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) return false;
        PROCESSENTRY32W entry{};
        entry.dwSize = sizeof(entry);
        bool found = false;
        if (Process32FirstW(snapshot, &entry)) {
            do {
                if (IsExcludedName(Lower(entry.szExeFile), o.excludeProcesses)) {
                    found = true;
                    break;
                }
            } while (Process32NextW(snapshot, &entry));
        }
        CloseHandle(snapshot);
        return found;
    }

    static void QueueWindow(HWND hwnd, bool transfer) {
        if (!hwnd || !IsWindow(hwnd)) return;
        if (!transfer && g_seenRegular.count(hwnd)) return;
        if (g_pending.find(hwnd) != g_pending.end()) return;
        RECT r{};
        if (!GetWindowRect(hwnd, &r)) return;
        Pending p;
        p.transfer = transfer;
        p.due = GetTickCount64() + 80;
        p.lastSize = {r.right - r.left, r.bottom - r.top};
        g_pending.emplace(hwnd, p);
    }

    // Obtain the actual current refresh rate of the monitor containing the window.
    // Timing follows the monitor instead of assuming 60 Hz or sleeping a fixed 6 ms.
    static int GetMonitorRefreshRate(HMONITOR monitor) {
        MONITORINFOEXW info{};
        info.cbSize = sizeof(info);
        if (GetMonitorInfoW(monitor, &info)) {
            DEVMODEW dm{};
            dm.dmSize = sizeof(dm);
            if (EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &dm) &&
                dm.dmDisplayFrequency >= 30 && dm.dmDisplayFrequency <= 1000) {
                return static_cast<int>(dm.dmDisplayFrequency);
            }
        }
        return 60;
    }

    static double AnimationEase(double p, bool originalEaseOut) {
        p = std::clamp(p, 0.0, 1.0);
        if (originalEaseOut) {
            // Optional legacy easing curve (internal).
            return 1.0 - std::pow(1.0 - p, 3.0);
        }
        // Smooth acceleration and landing at every monitor refresh rate.
        constexpr double kPi = 3.14159265358979323846;
        return (1.0 - std::cos(kPi * p)) * 0.5;
    }

    // Animation worker: one timer is only active while there are animations.
    // A high-resolution waitable timer provides short, low-CPU frame waits;
    // QueryPerformanceCounter measures the actual elapsed time, eliminating
    // GetTickCount64's coarse 10-16 ms quantization.
    static DWORD WINAPI AnimationThreadProc(LPVOID) {
        // Match the coordinate space of the event thread (physical screen pixels).
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        HANDLE timer = CreateWaitableTimerExW(
            nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
            TIMER_MODIFY_STATE | SYNCHRONIZE);
        if (!timer) timer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
        HANDLE handles[2] = {g_animationWake, timer};

        struct Move {
            HWND hwnd;
            int x, y;
        };

        while (!g_animationStop.load()) {
            LARGE_INTEGER now{};
            QueryPerformanceCounter(&now);
            LONGLONG nextFrame = std::numeric_limits<LONGLONG>::max();
            std::vector<Move> moves;
            {
                std::lock_guard<std::mutex> lock(g_animationMutex);
                for (auto it = g_animations.begin(); it != g_animations.end();) {
                    Animation& a = it->second;
                    if (!IsWindow(a.hwnd)) {
                        it = g_animations.erase(it);
                        continue;
                    }
                    if (now.QuadPart >= a.nextFrameQpc ||
                        now.QuadPart >= a.deadlineQpc) {
                        double progress = static_cast<double>(now.QuadPart - a.startedQpc) /
                            (static_cast<double>(g_qpcFrequency.QuadPart) *
                            a.durationMs / 1000.0);
                        double easing = AnimationEase(progress, a.originalEaseOut);
                        int x = a.startX + static_cast<int>(std::lround((a.targetX - a.startX) * easing));
                        int y = a.startY + static_cast<int>(std::lround((a.targetY - a.startY) * easing));
                        bool complete = now.QuadPart >= a.deadlineQpc;
                        if (complete) { x = a.targetX; y = a.targetY; }
                        moves.push_back({a.hwnd, x, y});
                        if (complete) {
                            it = g_animations.erase(it);
                            continue;
                        }
                        // Skip overdue frame slots rather than queueing a burst of
                        // stale positions when Windows schedules our thread late.
                        LONGLONG slots = (now.QuadPart - a.startedQpc) /
                                                a.framePeriodQpc + 1;
                        a.nextFrameQpc = std::min(a.deadlineQpc,
                            a.startedQpc + slots * a.framePeriodQpc);
                    }
                    nextFrame = std::min(nextFrame, a.nextFrameQpc);
                    ++it;
                }
            }
            const UINT flags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE;
            for (const Move& move : moves) {
                if (g_animationStop.load()) break;
                if (IsWindow(move.hwnd)) {
                    SetWindowPos(move.hwnd, nullptr, move.x, move.y, 0, 0, flags);
                }
            }
            if (g_animationStop.load()) break;

            if (nextFrame == std::numeric_limits<LONGLONG>::max()) {
                // No animations: fully asleep until a new window needs centering.
                WaitForSingleObject(g_animationWake, INFINITE);
                continue;
            }

            QueryPerformanceCounter(&now);
            LONGLONG remaining = nextFrame - now.QuadPart;
            if (remaining <= 0) continue;
            if (timer) {
                LARGE_INTEGER due{};
                // Negative due times represent relative time in 100 ns units.
                due.QuadPart = -std::max<LONGLONG>(1,
                    remaining * 10000000LL / g_qpcFrequency.QuadPart);
                if (SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE)) {
                    WaitForMultipleObjects(2, handles, FALSE, INFINITE);
                    continue;
                }
            }
            // Normal timer fallback on platforms that don't have high-res timers.
            DWORD ms = static_cast<DWORD>(std::clamp<LONGLONG>(
                remaining * 1000 / g_qpcFrequency.QuadPart, 1, 100));
            WaitForSingleObject(g_animationWake, ms);
        }
        if (timer) CloseHandle(timer);
        return 0;
    }

    static void CancelAnimation(HWND hwnd) {
        std::lock_guard<std::mutex> lock(g_animationMutex);
        g_animations.erase(hwnd);
    }

    static void CenterWindow(HWND hwnd, int animationMs, bool originalEaseOut) {
        RECT r{};
        if (!GetWindowRect(hwnd, &r)) return;
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        if (!GetMonitorInfoW(monitor, &mi)) return;
        int w = r.right - r.left;
        int h = r.bottom - r.top;
        if (w <= 0 || h <= 0) return;
        int targetX = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - w) / 2;
        int targetY = mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top - h) / 2;
        const UINT flags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE;
        if (r.left == targetX && r.top == targetY) return;

        if (animationMs == 0 || !g_animationThread ||
            g_qpcFrequency.QuadPart <= 0) {
            CancelAnimation(hwnd);
            SetWindowPos(hwnd, nullptr, targetX, targetY, 0, 0, flags);
        } else {
            LARGE_INTEGER start{};
            QueryPerformanceCounter(&start);
            int hz = GetMonitorRefreshRate(monitor);
            Animation a{};
            a.hwnd = hwnd;
            a.startX = r.left;
            a.startY = r.top;
            a.targetX = targetX;
            a.targetY = targetY;
            a.durationMs = animationMs;
            a.originalEaseOut = originalEaseOut;
            a.startedQpc = start.QuadPart;
            a.deadlineQpc = start.QuadPart +
                g_qpcFrequency.QuadPart * animationMs / 1000;
            a.framePeriodQpc = std::max<LONGLONG>(1,
                g_qpcFrequency.QuadPart / hz);
            a.nextFrameQpc = start.QuadPart + a.framePeriodQpc;
            {
                std::lock_guard<std::mutex> lock(g_animationMutex);
                // Replace any in-progress animation for this particular window.
                g_animations[hwnd] = a;
            }
            SetEvent(g_animationWake);
        }
    }

    static void ProcessPending(const Options& o) {
        ULONGLONG now = GetTickCount64();
        for (auto it = g_pending.begin(); it != g_pending.end();) {
            HWND hwnd = it->first;
            Pending& p = it->second;
            if (!IsWindow(hwnd)) {
                it = g_pending.erase(it);
                continue;
            }
            if (now < p.due) { ++it; continue; }
            if (g_pauseForProcess || !o.autoCenter || Excluded(hwnd, o)) {
                it = g_pending.erase(it);
                continue;
            }
            if (p.transfer ? !IsTopLevelVisible(hwnd) || !IsTransfer(hwnd)
                        : !IsRegularEligible(hwnd) || GetForegroundWindow() != hwnd) {
                it = g_pending.erase(it);
                continue;
            }
            RECT rect{};
            if (!GetWindowRect(hwnd, &rect) || rect.right <= rect.left || rect.bottom <= rect.top) {
                it = g_pending.erase(it);
                continue;
            }
            SIZE size{rect.right - rect.left, rect.bottom - rect.top};
            if ((size.cx != p.lastSize.cx || size.cy != p.lastSize.cy) && p.resizes++ < 5) {
                p.lastSize = size;
                p.due = now + 80;
                ++it;
                continue;
            }
            bool transfer = p.transfer;
            it = g_pending.erase(it);
            if (transfer) g_transferSizes[hwnd] = size;
            else g_seenRegular.insert(hwnd);
            CenterWindow(hwnd, o.animationMs, o.originalEaseOut);
            now = GetTickCount64();
        }
    }

    static BOOL CALLBACK ScanTransferCallback(HWND hwnd, LPARAM) {
        if (!IsTransfer(hwnd) || !IsTopLevelVisible(hwnd)) return TRUE;
        RECT r{};
        if (!GetWindowRect(hwnd, &r)) return TRUE;
        SIZE size{r.right - r.left, r.bottom - r.top};
        if (size.cx <= 0 || size.cy <= 0) return TRUE;
        auto it = g_transferSizes.find(hwnd);
        if (it == g_transferSizes.end() || it->second.cx != size.cx || it->second.cy != size.cy)
            QueueWindow(hwnd, true);
        return TRUE;
    }

    static void ScanTransfers() {
        EnumWindows(ScanTransferCallback, 0);
        for (auto it = g_transferSizes.begin(); it != g_transferSizes.end();) {
            if (!IsWindow(it->first) || !IsTransfer(it->first) || IsIconic(it->first) || IsZoomed(it->first))
                it = g_transferSizes.erase(it);
            else ++it;
        }
    }

    static void Tick() {
        Options o = GetOptions();
        ULONGLONG now = GetTickCount64();
        if (now - g_lastProcessScan >= 1500) {
            g_lastProcessScan = now;
            g_pauseForProcess = HasExcludedProcess(o);
        }
        if (now - g_lastForegroundScan >= (ULONGLONG)o.foregroundFallbackMs) {
            g_lastForegroundScan = now;
            if (o.autoCenter && !g_pauseForProcess)
                QueueWindow(GetForegroundWindow(), false);
        }
        if (now - g_lastTransferScan >= kTransferScanMs) {
            g_lastTransferScan = now;
            if (o.autoCenter && !g_pauseForProcess) ScanTransfers();
        }
        ProcessPending(o);
    }

    static void CALLBACK WinEventCallback(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                        LONG objectId, LONG childId, DWORD, DWORD) {
        if (!hwnd) return;
        if (event == EVENT_OBJECT_DESTROY && objectId == OBJID_WINDOW && childId == CHILDID_SELF) {
            g_pending.erase(hwnd);
            g_seenRegular.erase(hwnd);
            g_transferSizes.erase(hwnd);
            CancelAnimation(hwnd);
            return;
        }
        if (objectId != OBJID_WINDOW || childId != CHILDID_SELF) return;
        if (g_pauseForProcess) return;
        Options o = GetOptions();
        if (IsTransfer(hwnd)) {
            if (o.autoCenter) {
                RECT r{};
                if (GetWindowRect(hwnd, &r)) {
                    SIZE size{r.right - r.left, r.bottom - r.top};
                    auto it = g_transferSizes.find(hwnd);
                    if (it == g_transferSizes.end() || it->second.cx != size.cx || it->second.cy != size.cy)
                        QueueWindow(hwnd, true);
                }
            }
        } else if (o.autoCenter && hwnd == GetForegroundWindow()) {
            QueueWindow(hwnd, false);
        }
    }

    static void AddHook(DWORD event) {
        HWINEVENTHOOK hook = SetWinEventHook(event, event, nullptr, WinEventCallback,
                                            0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        if (hook) g_hooks.push_back(hook);
        else Wh_Log(L"Window Centering Suite: SetWinEventHook failed for %lu (%lu)", event, GetLastError());
    }

    // Windows global hotkeys consist of one primary (non-modifier) key plus
    // optional Ctrl, Alt, Shift and Win modifiers. All 1-5 key combinations of
    // that form are supported; arbitrary chords like "A+B" are not supported by
    // RegisterHotKey. Handle these on the tool's message-loop thread only.
    struct HotkeyBinding {
        UINT modifiers = 0;
        UINT vk = 0;
    };

    static std::wstring Trim(const std::wstring& str) {
        size_t first = str.find_first_not_of(L" \t\r\n");
        if (first == std::wstring::npos) return L"";
        size_t last = str.find_last_not_of(L" \t\r\n");
        return str.substr(first, last - first + 1);
    }

    static UINT KeyNameToVirtualKey(const std::wstring& key) {
        // Accept decimal VK values from older settings as well as human-readable
        // hotkey strings such as "Alt+X", "F8", and "Ctrl+Shift+C".
        if (key.size() > 1 &&
            std::all_of(key.begin(), key.end(), [](wchar_t c) {
                return c >= L'0' && c <= L'9';
            })) {
            UINT value = 0;
            for (wchar_t c : key) {
                value = value * 10 + static_cast<UINT>(c - L'0');
                if (value > 255) return 0;
            }
            // MOD_* keys cannot be used as the single primary key.
            if (value == 0 || value == VK_SHIFT || value == VK_CONTROL ||
                value == VK_MENU || value == VK_LWIN || value == VK_RWIN) return 0;
            return value;
        }
        if (key.size() == 1) {
            wchar_t c = key[0];
            if ((c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9'))
                return static_cast<UINT>(c >= L'a' ? c - L'a' + L'A' : c);
        }
        if (key.size() >= 2 && key[0] == L'f') {
            unsigned number = 0;
            bool digitsOnly = true;
            for (size_t i = 1; i < key.size(); ++i) {
                if (key[i] < L'0' || key[i] > L'9') {
                    digitsOnly = false;
                    break;
                }
                number = number * 10 + static_cast<unsigned>(key[i] - L'0');
                if (number > 24) { digitsOnly = false; break; }
            }
            if (digitsOnly && number >= 1 && number <= 24)
                return VK_F1 + number - 1;
        }
        struct KeyAlias { const wchar_t* name; UINT vk; };
        static constexpr KeyAlias aliases[] = {
            {L"escape", VK_ESCAPE}, {L"esc", VK_ESCAPE},
            {L"tab", VK_TAB}, {L"space", VK_SPACE},
            {L"enter", VK_RETURN}, {L"return", VK_RETURN},
            {L"backspace", VK_BACK}, {L"back", VK_BACK},
            {L"delete", VK_DELETE}, {L"del", VK_DELETE},
            {L"insert", VK_INSERT}, {L"ins", VK_INSERT},
            {L"home", VK_HOME}, {L"end", VK_END},
            {L"pageup", VK_PRIOR}, {L"pgup", VK_PRIOR},
            {L"pagedown", VK_NEXT}, {L"pgdn", VK_NEXT},
            {L"up", VK_UP}, {L"down", VK_DOWN},
            {L"left", VK_LEFT}, {L"right", VK_RIGHT},
            {L"capslock", VK_CAPITAL}, {L"numlock", VK_NUMLOCK},
            {L"scrolllock", VK_SCROLL}, {L"pause", VK_PAUSE},
            {L"printscreen", VK_SNAPSHOT}, {L"prtsc", VK_SNAPSHOT},
            {L"apps", VK_APPS}, {L"menu", VK_APPS},
            {L"numpad0", VK_NUMPAD0}, {L"numpad1", VK_NUMPAD1},
            {L"numpad2", VK_NUMPAD2}, {L"numpad3", VK_NUMPAD3},
            {L"numpad4", VK_NUMPAD4}, {L"numpad5", VK_NUMPAD5},
            {L"numpad6", VK_NUMPAD6}, {L"numpad7", VK_NUMPAD7},
            {L"numpad8", VK_NUMPAD8}, {L"numpad9", VK_NUMPAD9},
            {L"numpadadd", VK_ADD}, {L"numpadplus", VK_ADD},
            {L"numpadsubtract", VK_SUBTRACT}, {L"numpadminus", VK_SUBTRACT},
            {L"numpadmultiply", VK_MULTIPLY}, {L"numpaddivide", VK_DIVIDE},
            {L"numpaddecimal", VK_DECIMAL},
            {L"plus", VK_OEM_PLUS}, {L"minus", VK_OEM_MINUS},
            {L"comma", VK_OEM_COMMA}, {L"period", VK_OEM_PERIOD},
            {L"semicolon", VK_OEM_1}, {L"slash", VK_OEM_2},
            {L"backtick", VK_OEM_3}, {L"lbracket", VK_OEM_4},
            {L"backslash", VK_OEM_5}, {L"rbracket", VK_OEM_6},
            {L"apostrophe", VK_OEM_7},
        };
        for (const auto& alias : aliases) {
            if (key == alias.name) return alias.vk;
        }
        return 0;
    }

    static bool ParseManualHotkey(const std::wstring& text, HotkeyBinding* binding) {
        HotkeyBinding parsed;
        std::wstring input = Lower(Trim(text));
        if (input.empty() || input == L"off" || input == L"none" ||
            input == L"disabled") {
            *binding = parsed;  // No hotkey requested.
            return true;
        }
        size_t begin = 0;
        while (begin < input.size()) {
            size_t end = input.find(L'+', begin);
            if (end == std::wstring::npos) end = input.size();
            std::wstring token = Trim(input.substr(begin, end - begin));
            if (token.empty()) return false;  // e.g. "Alt++X"
            UINT modifier = 0;
            if (token == L"ctrl" || token == L"control") modifier = MOD_CONTROL;
            else if (token == L"alt" || token == L"option") modifier = MOD_ALT;
            else if (token == L"shift") modifier = MOD_SHIFT;
            else if (token == L"win" || token == L"windows" || token == L"super")
                modifier = MOD_WIN;
            if (modifier) {
                if (parsed.modifiers & modifier) return false; // Duplicate modifier.
                parsed.modifiers |= modifier;
            } else {
                UINT vk = KeyNameToVirtualKey(token);
                if (!vk || parsed.vk) return false; // Unsupported or two primary keys.
                parsed.vk = vk;
            }
            if (end == input.size()) break;
            begin = end + 1;
            if (begin == input.size()) return false; // Trailing '+'.
        }
        if (!parsed.vk) return false; // Modifiers alone aren't valid hotkeys.
        *binding = parsed;
        return true;
    }

    static void ApplyManualHotkey(const std::wstring& setting) {
        HotkeyBinding binding{};
        if (!ParseManualHotkey(setting, &binding)) {
            Wh_Log(L"Window Centering Suite: Invalid manual hotkey '%s'. Examples: Alt+X, F8, Ctrl+Shift+C, Off. Keeping prior hotkey.", setting.c_str());
            return;
        }
        if (g_hotkeyRegistered && binding.vk == g_registeredVk &&
            binding.modifiers == g_registeredModifiers) return;

        // Unregister before replacing: the same (thread, id) must be unique.
        UINT priorMods = g_registeredModifiers;
        UINT priorVk = g_registeredVk;
        bool hadPrevious = g_hotkeyRegistered;
        if (g_hotkeyRegistered) UnregisterHotKey(nullptr, kHotkeyId);
        g_hotkeyRegistered = false;
        g_registeredModifiers = 0;
        g_registeredVk = 0;

        if (!binding.vk) return; // "Off"/"None" turns the shortcut off.
        if (RegisterHotKey(nullptr, kHotkeyId,
                        binding.modifiers | MOD_NOREPEAT, binding.vk)) {
            g_hotkeyRegistered = true;
            g_registeredModifiers = binding.modifiers;
            g_registeredVk = binding.vk;
            return;
        }
        DWORD error = GetLastError();
        Wh_Log(L"Window Centering Suite: Could not register manual hotkey '%s' (error %lu). A program or Windows may already use it.",
            setting.c_str(), error);
        // A bad or occupied new shortcut must not silently lose the old one.
        if (hadPrevious && RegisterHotKey(nullptr, kHotkeyId,
                                        priorMods | MOD_NOREPEAT, priorVk)) {
            g_hotkeyRegistered = true;
            g_registeredModifiers = priorMods;
            g_registeredVk = priorVk;
        }
    }

    BOOL WhTool_ModInit() {
        std::lock_guard<std::mutex> lock(g_optionsMutex);
        g_options = LoadOptions();
        return TRUE;
    }

    void WhTool_ModSettingsChanged() {
        Options refreshed = LoadOptions();
        {
            std::lock_guard<std::mutex> lock(g_optionsMutex);
            g_options = std::move(refreshed);
        }
        if (g_eventThreadId) PostThreadMessageW(g_eventThreadId, WM_APP + 1, 0, 0);
    }

    void WhTool_ModUninit() {
        if (g_eventThreadId) PostThreadMessageW(g_eventThreadId, WM_QUIT, 0, 0);
    }

    void WhTool_ModEntryPoint() {
        // Ensure GetWindowRect and SetWindowPos use the same physical-pixel
        // coordinate space even on mixed-DPI Windows 10/11 desktops.
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        g_eventThreadId = GetCurrentThreadId();
        QueryPerformanceFrequency(&g_qpcFrequency);
        g_animationStop.store(false);
        g_animationWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (g_animationWake && g_qpcFrequency.QuadPart > 0)
            g_animationThread = CreateThread(nullptr, 0, AnimationThreadProc,
                                            nullptr, 0, nullptr);
        if (!g_animationThread)
            Wh_Log(L"Window Centering Suite: Animation worker failed to start; using immediate positioning");
        MSG msg{};
        PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

        Options o = GetOptions();
        ApplyManualHotkey(o.manualCenterHotkey);

        AddHook(EVENT_OBJECT_CREATE);
        AddHook(EVENT_OBJECT_SHOW);
        AddHook(EVENT_OBJECT_LOCATIONCHANGE);
        AddHook(EVENT_OBJECT_DESTROY);
        AddHook(EVENT_SYSTEM_FOREGROUND);
        AddHook(EVENT_SYSTEM_MINIMIZEEND);

        // For a thread timer with a null HWND, Windows allocates the timer ID.
        g_timer = SetTimer(nullptr, 0, 30, nullptr);
        if (!g_timer) Wh_Log(L"Window Centering Suite: SetTimer failed (%lu)", GetLastError());
        g_lastProcessScan = 0;
        Tick();

        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            if (msg.message == WM_TIMER && msg.wParam == g_timer) {
                Tick();
            } else if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyId) {
                Options s = GetOptions();
                if (g_hotkeyRegistered && !g_pauseForProcess) {
                    HWND hwnd = GetForegroundWindow();
                    if (IsTopLevelVisible(hwnd) && !Excluded(hwnd, s))
                        CenterWindow(hwnd, s.animationMs, s.originalEaseOut);
                }
            } else if (msg.message == WM_APP + 1) {
                Options s = GetOptions();
                ApplyManualHotkey(s.manualCenterHotkey);
                g_lastProcessScan = 0;
            } else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        if (g_timer) KillTimer(nullptr, g_timer);
        if (g_hotkeyRegistered) UnregisterHotKey(nullptr, kHotkeyId);
        for (auto hook : g_hooks) UnhookWinEvent(hook);
        g_hooks.clear();
        g_animationStop.store(true);
        if (g_animationWake) SetEvent(g_animationWake);
        if (g_animationThread) {
            WaitForSingleObject(g_animationThread, INFINITE);
            CloseHandle(g_animationThread);
            g_animationThread = nullptr;
        }
        if (g_animationWake) {
            CloseHandle(g_animationWake);
            g_animationWake = nullptr;
        }
        {
            std::lock_guard<std::mutex> lock(g_animationMutex);
            g_animations.clear();
        }
        g_eventThreadId = 0;
    }
