// ==WindhawkMod==
// @id              taskbar-volume-control-per-app
// @name            Taskbar Volume Control Per-App
// @description     Control the per-app volume by scrolling over taskbar buttons
// @version         1.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lhid -lntdll -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar Volume Control Per-App

Control the per-app volume by scrolling over taskbar buttons on Windows 11.

Scrolling over a taskbar button will adjust the volume of that specific
application. Ctrl+clicking or middle-clicking (clicking the mouse wheel) on a
taskbar button will toggle mute for that app. A tooltip shows the current
volume percentage, or "No audio session" if the app has no active audio.

**Note:** Some laptop touchpads might not support scrolling over the taskbar.
For precision touchpads, the "Touchpad scrolling" option can be enabled to read
two-finger scrolling directly from the touchpad. Another workaround is to use
the "pinch to zoom" gesture. For details, check out [a relevant
issue](https://tweaker.userecho.com/topics/826-scroll-on-trackpadtouchpad-doesnt-trigger-mouse-wheel-options).

![Demonstration](https://i.imgur.com/56QHjUv.gif)

## Controlling the overall system volume

For controlling the overall system volume, check out the [Taskbar Volume
Control](https://windhawk.net/mods/taskbar-volume-control) mod. Note that both
mods can be used simultaneously using one of these approaches:
1. Configure Taskbar Volume Control to use a limited region, such as the system
   tray area, while this mod handles the app-specific volume on the task
   buttons.
2. Configure Taskbar Volume Control to require the Ctrl key for volume changes,
   so that holding Ctrl while scrolling adjusts the system volume, and normal
   scrolling adjusts per-app volume.

Note that if both mods are configured to act simultaneously, the Taskbar Volume
Control mod takes precedence due to the way the mod works.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- volumeChangeStep: 10
  $name: Volume change step
  $description: >-
    Allows to configure the volume change that will occur with each notch of
    mouse wheel movement.
- ctrlClickToMute: true
  $name: Ctrl + Click to mute
  $description: >-
    When enabled, Ctrl+clicking on a taskbar button will toggle the mute state
    of the application.
- middleClickToMute: false
  $name: Middle Click to mute
  $description: >-
    When enabled, middle-clicking (clicking the mouse wheel) on a taskbar
    button will toggle the mute state of the application.
- ctrlScrollVolumeChange: false
  $name: Ctrl + Scroll to change volume
  $description: >-
    When enabled, scrolling with the mouse wheel or the touchpad will only
    change the volume when the Ctrl key is held down.
- touchpadScroll: false
  $name: Touchpad scrolling
  $description: >-
    Change the volume with two-finger scrolling on a precision touchpad, for
    touchpads whose scrolling doesn't work over the taskbar. Moving two fingers
    up raises the volume.
- terseFormat: false
  $name: Terse format
  $description: >-
    When enabled, the tooltip shows a compact format with emojis instead of
    text (e.g., "🔊 64%" instead of "Volume: 64%").
- tooltipColor: ""
  $name: Tooltip color
  $description: >-
    Custom tooltip background color in #RRGGBB or #AARRGGBB format (e.g.,
    #FF0000 for red). Leave empty to use the system accent color.
- noAutomaticMuteToggle: false
  $name: No automatic mute toggle
  $description: >-
    By default, the app is muted once the volume reaches zero, and is unmuted
    on any change to a non-zero volume. Enabling this option turns off this
    functionality, such that the app mute status is not changed.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <audiopolicy.h>
#include <endpointvolume.h>
#include <hidusage.h>
#include <mmdeviceapi.h>
#include <tlhelp32.h>
#include <winternl.h>

// Depends on hidusage.h.
#include <hidpi.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    int volumeChangeStep;
    bool ctrlClickToMute;
    bool middleClickToMute;
    bool ctrlScrollVolumeChange;
    bool touchpadScroll;
    bool terseFormat;
    std::optional<winrt::Windows::UI::Color> tooltipColor;
    bool noAutomaticMuteToggle;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;

IMMDeviceEnumerator* g_pDeviceEnumerator;

// Per-app volume: taskbar button integration.
thread_local bool g_captureTaskGroup;
thread_local void* g_capturedTaskGroup;

// Click sentinel pattern for getting native task item/group from WindowsUdk.
WCHAR g_clickSentinel[] = L"click-sentinel";
void* g_clickSentinel_TaskGroup = nullptr;
void* g_clickSentinel_TaskItem = nullptr;

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

const GUID XIID_IMMDeviceEnumerator = {
    0xA95664D2,
    0x9614,
    0x4F35,
    {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const GUID XIID_MMDeviceEnumerator = {
    0xBCDE0395,
    0xE52F,
    0x467C,
    {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const GUID XIID_IAudioSessionManager2 = {
    0x77AA99A0,
    0x1BD6,
    0x484F,
    {0x8B, 0xC7, 0x2C, 0x65, 0x4C, 0x9A, 0x9B, 0x6F}};

void SndVolInit() {
    HRESULT hr = CoCreateInstance(
        XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
        XIID_IMMDeviceEnumerator, (LPVOID*)&g_pDeviceEnumerator);
    if (FAILED(hr)) {
        g_pDeviceEnumerator = NULL;
    }
}

void SndVolUninit() {
    if (g_pDeviceEnumerator) {
        g_pDeviceEnumerator->Release();
        g_pDeviceEnumerator = NULL;
    }
}

// Callback type for processing audio sessions.
// Receives the session's process ID and session control interface.
// Returns true to continue iterating, false to stop.
using AudioSessionCallback =
    std::function<bool(DWORD pid, IAudioSessionControl2* sessionControl)>;

// Iterates over all audio sessions.
// Calls the callback for each session with its PID and IAudioSessionControl2.
// Returns true if at least one session was processed.
bool ForEachAudioSession(const AudioSessionCallback& callback) {
    if (!g_pDeviceEnumerator) {
        SndVolInit();
        if (!g_pDeviceEnumerator) {
            return false;
        }
    }

    winrt::com_ptr<IMMDevice> defaultDevice;
    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, defaultDevice.put());
    if (FAILED(hr)) {
        return false;
    }

    winrt::com_ptr<IAudioSessionManager2> sessionManager;
    hr = defaultDevice->Activate(XIID_IAudioSessionManager2, CLSCTX_ALL, NULL,
                                 sessionManager.put_void());
    if (FAILED(hr)) {
        return false;
    }

    winrt::com_ptr<IAudioSessionEnumerator> sessionEnumerator;
    hr = sessionManager->GetSessionEnumerator(sessionEnumerator.put());
    if (FAILED(hr)) {
        return false;
    }

    int sessionCount = 0;
    hr = sessionEnumerator->GetCount(&sessionCount);
    if (FAILED(hr)) {
        return false;
    }

    bool foundSession = false;

    for (int i = 0; i < sessionCount; i++) {
        winrt::com_ptr<IAudioSessionControl> sessionControl;
        hr = sessionEnumerator->GetSession(i, sessionControl.put());
        if (FAILED(hr)) {
            continue;
        }

        winrt::com_ptr<IAudioSessionControl2> sessionControl2;
        hr = sessionControl->QueryInterface(__uuidof(IAudioSessionControl2),
                                            sessionControl2.put_void());
        if (FAILED(hr)) {
            continue;
        }

        // Skip system sounds session.
        if (sessionControl2->IsSystemSoundsSession() == S_OK) {
            continue;
        }

        DWORD sessionPID = 0;
        hr = sessionControl2->GetProcessId(&sessionPID);
        if (FAILED(hr)) {
            continue;
        }

        foundSession = true;
        if (!callback(sessionPID, sessionControl2.get())) {
            break;
        }
    }

    return foundSession;
}

// Get the command line of a process from its handle.
// Returns empty string on failure.
std::wstring GetProcessCommandLine(HANDLE hProcess) {
    PROCESS_BASIC_INFORMATION pbi;
    ULONG returnLength;
    NTSTATUS status = NtQueryInformationProcess(
        hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &returnLength);

    if (!NT_SUCCESS(status) || !pbi.PebBaseAddress) {
        return {};
    }

    PEB peb;
    if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb),
                           nullptr)) {
        return {};
    }

    RTL_USER_PROCESS_PARAMETERS params;
    if (!ReadProcessMemory(hProcess, peb.ProcessParameters, &params,
                           sizeof(params), nullptr)) {
        return {};
    }

    std::wstring cmdLine(params.CommandLine.Length / sizeof(WCHAR), L'\0');
    if (!ReadProcessMemory(hProcess, params.CommandLine.Buffer, cmdLine.data(),
                           params.CommandLine.Length, nullptr)) {
        return {};
    }

    return cmdLine;
}

// Find Chromium's audio subprocess for a given parent process.
// Chromium-based browsers run audio in a separate utility process with
// specific command line flags. Returns the audio subprocess PID or 0 if not
// found.
DWORD FindChromiumAudioSubprocessUncached(DWORD parentPID) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    DWORD audioSubprocessPID = 0;
    std::vector<DWORD> rendererPIDs;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    if (Process32First(snapshot, &pe32)) {
        do {
            if (pe32.th32ParentProcessID != parentPID) {
                continue;
            }

            HANDLE hProcess =
                OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,
                            FALSE, pe32.th32ProcessID);
            if (!hProcess) {
                continue;
            }

            std::wstring cmdLine = GetProcessCommandLine(hProcess);
            CloseHandle(hProcess);

            if (cmdLine.empty()) {
                continue;
            }

            // Parse command line and check for Chromium audio service flags.
            int argc = 0;
            LPWSTR* argv = CommandLineToArgvW(cmdLine.c_str(), &argc);
            if (!argv) {
                continue;
            }

            bool isUtility = false;
            bool isAudioService = false;
            bool isRenderer = false;
            for (int i = 0; i < argc; i++) {
                if (wcscmp(argv[i], L"--type=utility") == 0) {
                    isUtility = true;
                } else if (wcscmp(argv[i],
                                  L"--utility-sub-type=audio.mojom."
                                  L"AudioService") == 0) {
                    isAudioService = true;
                } else if (wcscmp(argv[i], L"--type=renderer") == 0) {
                    isRenderer = true;
                }
            }

            LocalFree(argv);

            if (isUtility && isAudioService) {
                audioSubprocessPID = pe32.th32ProcessID;
                break;
            } else if (isRenderer) {
                rendererPIDs.push_back(pe32.th32ProcessID);
            }
        } while (Process32Next(snapshot, &pe32));
    }

    CloseHandle(snapshot);

    if (audioSubprocessPID) {
        return audioSubprocessPID;
    }

    // Fallback: return first renderer process with audio session.
    if (rendererPIDs.empty()) {
        return 0;
    }

    DWORD rendererAudioPID = 0;
    ForEachAudioSession([&](DWORD sessionPID, IAudioSessionControl2*) {
        for (DWORD pid : rendererPIDs) {
            if (sessionPID == pid) {
                rendererAudioPID = pid;
                return false;  // Stop iterating.
            }
        }
        return true;  // Continue iterating.
    });
    return rendererAudioPID;
}

// Cache for Chromium audio subprocess lookups to avoid expensive repeated
// process enumeration.
struct ChromiumAudioSubprocessCache {
    DWORD audioSubprocessPID;
    ULONGLONG timestamp;
};
std::unordered_map<DWORD, ChromiumAudioSubprocessCache>
    g_chromiumAudioSubprocessCache;
constexpr ULONGLONG kChromiumCacheTTL = 5000;  // 5 seconds.

DWORD FindChromiumAudioSubprocess(DWORD parentPID) {
    ULONGLONG now = GetTickCount64();

    // Check cache first.
    auto it = g_chromiumAudioSubprocessCache.find(parentPID);
    if (it != g_chromiumAudioSubprocessCache.end()) {
        if (now - it->second.timestamp < kChromiumCacheTTL) {
            return it->second.audioSubprocessPID;
        }
    }

    // Cache miss or expired, do the lookup.
    DWORD audioSubprocessPID = FindChromiumAudioSubprocessUncached(parentPID);

    // Clean up expired entries.
    for (auto cacheIt = g_chromiumAudioSubprocessCache.begin();
         cacheIt != g_chromiumAudioSubprocessCache.end();) {
        if (now - cacheIt->second.timestamp >= kChromiumCacheTTL) {
            cacheIt = g_chromiumAudioSubprocessCache.erase(cacheIt);
        } else {
            ++cacheIt;
        }
    }

    // Update cache.
    g_chromiumAudioSubprocessCache[parentPID] = {audioSubprocessPID, now};

    return audioSubprocessPID;
}

struct AppVolumeResult {
    int volume;  // 0-100.
    bool muted;
};

// Adjust volume for all audio sessions matching the given process ID.
std::optional<AppVolumeResult> AdjustAppVolumeForPID(DWORD targetPID,
                                                     float fVolumeAdd) {
    std::optional<AppVolumeResult> result;

    ForEachAudioSession([&](DWORD pid, IAudioSessionControl2* sessionControl) {
        if (pid != targetPID) {
            return true;  // Continue to next session.
        }

        winrt::com_ptr<ISimpleAudioVolume> vol;
        if (FAILED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                  vol.put_void()))) {
            return true;  // Continue to next session.
        }

        float currentVolume = 0.0f;
        HRESULT hr = vol->GetMasterVolume(&currentVolume);
        if (FAILED(hr)) {
            return true;  // Continue to next session.
        }

        float newVolume = currentVolume + fVolumeAdd;
        if (newVolume < 0.0f) {
            newVolume = 0.0f;
        } else if (newVolume > 1.0f) {
            newVolume = 1.0f;
        }

        hr = vol->SetMasterVolume(newVolume, NULL);
        if (SUCCEEDED(hr)) {
            AppVolumeResult r;
            r.volume = (int)(newVolume * 100.0f + 0.5f);

            // Handle mute state based on volume.
            if (!g_settings.noAutomaticMuteToggle) {
                if (newVolume < 0.005f) {
                    vol->SetMute(TRUE, NULL);
                    r.muted = true;
                } else {
                    vol->SetMute(FALSE, NULL);
                    r.muted = false;
                }
            } else {
                // Read actual mute state.
                BOOL isMuted = FALSE;
                r.muted = SUCCEEDED(vol->GetMute(&isMuted)) && isMuted;
            }

            result = r;
        }

        return true;  // Continue to process all sessions.
    });

    return result;
}

std::optional<AppVolumeResult> AdjustAppVolume(DWORD targetPID,
                                               float fVolumeAdd) {
    auto result = AdjustAppVolumeForPID(targetPID, fVolumeAdd);
    if (result) {
        return result;
    }

    // Try Chromium audio subprocess if no session found.
    DWORD audioSubprocessPID = FindChromiumAudioSubprocess(targetPID);
    if (audioSubprocessPID) {
        result = AdjustAppVolumeForPID(audioSubprocessPID, fVolumeAdd);
    }

    return result;
}

// Toggle mute state for a process. Returns new state with volume, or nullopt
// if no session.
std::optional<AppVolumeResult> ToggleAppMuteForPID(DWORD targetPID) {
    std::optional<AppVolumeResult> result;

    ForEachAudioSession([&](DWORD pid, IAudioSessionControl2* sessionControl) {
        if (pid != targetPID) {
            return true;  // Continue to next session.
        }

        winrt::com_ptr<ISimpleAudioVolume> vol;
        if (FAILED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                  vol.put_void()))) {
            return true;  // Continue to next session.
        }

        BOOL isMuted = FALSE;
        HRESULT hr = vol->GetMute(&isMuted);
        if (FAILED(hr)) {
            return true;  // Continue to next session.
        }

        hr = vol->SetMute(!isMuted, NULL);
        if (SUCCEEDED(hr)) {
            AppVolumeResult r;
            r.muted = !isMuted;

            float currentVolume = 0.0f;
            if (SUCCEEDED(vol->GetMasterVolume(&currentVolume))) {
                r.volume = (int)(currentVolume * 100.0f + 0.5f);
            } else {
                r.volume = 0;
            }

            result = r;
        }

        return true;  // Continue to process all sessions.
    });

    return result;
}

std::optional<AppVolumeResult> ToggleAppMute(DWORD targetPID) {
    auto result = ToggleAppMuteForPID(targetPID);
    if (result) {
        return result;
    }

    // Try Chromium audio subprocess if no session found.
    DWORD audioSubprocessPID = FindChromiumAudioSubprocess(targetPID);
    if (audioSubprocessPID) {
        result = ToggleAppMuteForPID(audioSubprocessPID);
    }

    return result;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{
        .proc = proc,
        .procParam = procParam,
    };
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow;

using CImmersiveTaskItem_GetAppWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetAppWindow_t CImmersiveTaskItem_GetAppWindow;

void* CImmersiveTaskItem_vftable;
void* CImmersiveTaskItem_vftable_ITaskItem;

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                            void** taskItem);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original;

using TaskListGroupViewModel_IsMultiWindow_t = bool(WINAPI*)(void* pThis);
TaskListGroupViewModel_IsMultiWindow_t
    TaskListGroupViewModel_IsMultiWindow_Original;

using ITaskGroup_IsRunning_t = bool(WINAPI*)(void* pThis);
ITaskGroup_IsRunning_t ITaskGroup_IsRunning_Original;
bool WINAPI ITaskGroup_IsRunning_Hook(void* pThis) {
    if (g_captureTaskGroup) {
        Wh_Log(L">");
        g_capturedTaskGroup = *(void**)pThis;
        return false;
    }

    return ITaskGroup_IsRunning_Original(pThis);
}

using CTaskGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems;

HDPA GetTaskItemsArray(void* taskGroup) {
    // This is a horrible hack, but it's the best way I found to get the array
    // of task items from a task group. It relies on the implementation of
    // CTaskGroup::GetNumItems being just this:
    //
    // return DPA_GetPtrCount(this->taskItemsArray);
    //
    // Or in other words:
    //
    // return *(int*)this[taskItemsArrayOffset];
    //
    // Instead of calling it with a real taskGroup object, we call it with an
    // array of pointers to ints. The returned int value is actually the offset
    // to the array member.

    static size_t offset = []() {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }

        return CTaskGroup_GetNumItems(arrayOfIntPtrs);
    }();

    return (HDPA)((void**)taskGroup)[offset];
}

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                    void* taskGroup,
                                                    void* taskItem,
                                                    void** launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;
HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                             void* taskGroup,
                                             void* taskItem,
                                             void** launcherOptions) {
    if (*launcherOptions == &g_clickSentinel) {
        Wh_Log(L"Click triggered by sentinel, taskGroup=%p, taskItem=%p",
               taskGroup, taskItem);
        g_clickSentinel_TaskGroup = taskGroup;
        g_clickSentinel_TaskItem = taskItem;
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                             launcherOptions);
}

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

using TaskGroup_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskGroup_ReportClicked_t TaskGroup_ReportClicked_Original;

// Triggers the sentinel to capture real native task item from WindowsUdk task
// item.
void* GetNativeTaskItemFromWindowsUdkTaskItem(void* windowsUdkTaskItem) {
    g_clickSentinel_TaskItem = nullptr;
    TaskItem_ReportClicked_Original(windowsUdkTaskItem, &g_clickSentinel);
    return g_clickSentinel_TaskItem;
}

// Triggers the sentinel to capture real native task group from WindowsUdk task
// group.
void* GetNativeTaskGroupFromWindowsUdkTaskGroup(void* windowsUdkTaskGroup) {
    g_clickSentinel_TaskGroup = nullptr;
    TaskGroup_ReportClicked_Original(windowsUdkTaskGroup, &g_clickSentinel);
    return g_clickSentinel_TaskGroup;
}

winrt::com_ptr<IUnknown> GetWindowsUdkTaskItemFromTaskListButton(
    UIElement element) {
    winrt::com_ptr<IUnknown> windowViewModel;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &element);
    if (!windowViewModel) {
        return nullptr;
    }

    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    TaskListWindowViewModel_get_TaskItem_Original(
        windowViewModel.get(), windowsUdkTaskItem.put_void());
    return windowsUdkTaskItem;
}

void* GetWindowsUdkTaskGroupFromTaskListButton(UIElement element) {
    winrt::com_ptr<IUnknown> groupViewModel = nullptr;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &element);
    if (!groupViewModel) {
        return nullptr;
    }

    g_capturedTaskGroup = nullptr;
    g_captureTaskGroup = true;
    TaskListGroupViewModel_IsMultiWindow_Original((void**)groupViewModel.get() -
                                                  1);
    g_captureTaskGroup = false;
    return g_capturedTaskGroup;
}

HWND GetWindowFromTaskItem(void* taskItem) {
    if (!taskItem) {
        return nullptr;
    }

    if (*(void**)taskItem == CImmersiveTaskItem_vftable_ITaskItem) {
        void* immersiveTaskItem =
            QueryViaVtable(taskItem, CImmersiveTaskItem_vftable);
        return CImmersiveTaskItem_GetAppWindow(immersiveTaskItem);
    }

    return CWindowTaskItem_GetWindow(taskItem);
}

// Get process ID from a taskbar button element.
// Tries to get from individual task item first, then from task group.
DWORD GetProcessIdFromTaskListButton(UIElement element) {
    // First try to get from individual task item using sentinel pattern.
    auto windowsUdkTaskItem = GetWindowsUdkTaskItemFromTaskListButton(element);
    if (windowsUdkTaskItem) {
        void* nativeTaskItem =
            GetNativeTaskItemFromWindowsUdkTaskItem(windowsUdkTaskItem.get());
        if (nativeTaskItem) {
            HWND hWnd = GetWindowFromTaskItem(nativeTaskItem);
            if (hWnd) {
                DWORD processId = 0;
                GetWindowThreadProcessId(hWnd, &processId);
                return processId;
            }
        }
    }

    // Try to get from task group (for grouped items).
    void* windowsUdkTaskGroup =
        GetWindowsUdkTaskGroupFromTaskListButton(element);
    if (windowsUdkTaskGroup) {
        void* nativeTaskGroup =
            GetNativeTaskGroupFromWindowsUdkTaskGroup(windowsUdkTaskGroup);
        if (nativeTaskGroup) {
            HDPA taskItemsArray = GetTaskItemsArray(nativeTaskGroup);
            if (taskItemsArray && DPA_GetPtrCount(taskItemsArray) > 0) {
                void* taskItem = DPA_GetPtr(taskItemsArray, 0);
                HWND hWnd = GetWindowFromTaskItem(taskItem);
                if (hWnd) {
                    DWORD processId = 0;
                    GetWindowThreadProcessId(hWnd, &processId);
                    return processId;
                }
            }
        }
    }

    return 0;
}

struct VolumeTooltipState {
    Controls::Primitives::Popup popup = nullptr;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    bool vertical = false;
    // The cursor's monitor in root coordinates.
    winrt::Windows::Foundation::Rect monitorRect{};
    double cursorX = 0.0;
    double cursorY = 0.0;
    UINT_PTR hideTimer = 0;
};

// Released on the UI thread by HideVolumeTooltip.
[[clang::no_destroy]] VolumeTooltipState g_volumeTooltipState;

struct TaskbarFrameAncestorResult {
    FrameworkElement element = nullptr;
    bool isOverflowPopup = false;
};

TaskbarFrameAncestorResult FindTaskbarFrameAncestor(UIElement element) {
    auto parent = Media::VisualTreeHelper::GetParent(element);
    while (parent) {
        auto className = winrt::get_class_name(parent);
        if (className == L"Taskbar.TaskbarFrame") {
            return {parent.try_as<FrameworkElement>(), false};
        }
        if (className == L"Taskbar.FlyoutFrame") {
            return {parent.try_as<FrameworkElement>(), true};
        }
        parent = Media::VisualTreeHelper::GetParent(parent);
    }
    return {};
}

void CALLBACK HideVolumeTooltipTimerProc(HWND hwnd,
                                         UINT message,
                                         UINT_PTR idTimer,
                                         DWORD dwTime) {
    KillTimer(nullptr, g_volumeTooltipState.hideTimer);
    g_volumeTooltipState.hideTimer = 0;

    if (g_volumeTooltipState.popup) {
        g_volumeTooltipState.popup.IsOpen(false);
    }
}

// Tooltip positioning offset from cursor in pixels.
constexpr int kTooltipOffset = 12;
constexpr int kTooltipOffsetTaskbarEdge = 4;

double GetRotationAngle(UIElement element) {
    auto transform = element.RenderTransform();
    if (!transform) {
        return 0.0;
    }

    if (auto rotateTransform = transform.try_as<Media::RotateTransform>()) {
        return rotateTransform.Angle();
    }

    if (auto transformGroup = transform.try_as<Media::TransformGroup>()) {
        for (auto child : transformGroup.Children()) {
            if (auto rotateTransform = child.try_as<Media::RotateTransform>()) {
                return rotateTransform.Angle();
            }
        }
    }

    return 0.0;
}

void UpdateTooltipPosition() {
    if (!g_volumeTooltipState.popup) {
        return;
    }

    auto popup = g_volumeTooltipState.popup;
    auto border = popup.Child().try_as<FrameworkElement>();
    double tooltipWidth = border ? border.ActualWidth() : 0;
    double tooltipHeight = border ? border.ActualHeight() : 0;

    double cursorX = g_volumeTooltipState.cursorX;
    double cursorY = g_volumeTooltipState.cursorY;
    const auto& monitorRect = g_volumeTooltipState.monitorRect;
    double monitorRight = monitorRect.X + monitorRect.Width;

    auto taskbarFrame = g_volumeTooltipState.taskbarFrame.get();
    if (!taskbarFrame) {
        // Overflow popup: to the right of the cursor, or to the left if
        // there's no room, as with a vertical taskbar docked to the right.
        double x = cursorX + kTooltipOffset;
        if (x + tooltipWidth > monitorRight) {
            x = cursorX - kTooltipOffset - tooltipWidth;
        }
        popup.HorizontalOffset(x);
        popup.VerticalOffset(cursorY - tooltipHeight / 2);
        return;
    }

    if (!g_volumeTooltipState.vertical) {
        // Get TaskbarFrame's position in root coordinates. In some cases,
        // taskbarFrame may be contained in an element with a larger height,
        // with a region above that's invisible.
        auto transform = taskbarFrame.TransformToVisual(nullptr);
        auto frameOrigin = transform.TransformPoint({0, 0});

        double taskbarHeight = taskbarFrame.ActualHeight();
        popup.HorizontalOffset(cursorX + kTooltipOffset);
        popup.VerticalOffset(
            frameOrigin.Y +
            std::fmax((taskbarHeight - tooltipHeight) / 2,
                      static_cast<double>(kTooltipOffsetTaskbarEdge)));
    } else {
        // At the screen edge which the taskbar is docked to, extending over
        // the taskbar towards the screen center.
        bool dockedRight = cursorX > monitorRect.X + monitorRect.Width / 2;
        popup.HorizontalOffset(
            dockedRight ? monitorRight - kTooltipOffsetTaskbarEdge - tooltipWidth
                        : monitorRect.X + kTooltipOffsetTaskbarEdge);
        popup.VerticalOffset(cursorY + kTooltipOffset);
    }
}

// Set while the tooltip popup opens, which creates the popup's window if it's a
// windowed popup.
thread_local bool g_openingTooltipPopup;

// ZBID_SYSTEM_TOOLS, an undocumented window band above the band of the taskbar
// thumbnail previews.
constexpr DWORD kZbidSystemTools = 16;

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           LPVOID lpParam,
                                           DWORD dwBand);

bool IsWindowedPopupClass(LPCWSTR className, HINSTANCE hInstance) {
    constexpr WCHAR kWindowedPopupClassName[] = L"Xaml_WindowedPopupClass";
    if (!IS_INTRESOURCE(className)) {
        return _wcsicmp(className, kWindowedPopupClassName) == 0;
    }

    // GetClassInfoEx returns the class atom.
    WNDCLASSEX wc{
        .cbSize = sizeof(wc),
    };
    ATOM atom =
        (ATOM)GetClassInfoEx(hInstance, kWindowedPopupClassName, &wc);
    return atom && atom == (ATOM)(ULONG_PTR)className;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    if (!g_openingTooltipPopup ||
        !IsWindowedPopupClass(lpClassName, hInstance)) {
        return CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                        dwStyle, X, Y, nWidth, nHeight,
                                        hWndParent, hMenu, hInstance, lpParam);
    }

    // The window takes mouse input regardless of the popup's IsHitTestVisible,
    // and the tooltip follows the cursor, so the window is made transparent to
    // mouse input to keep the pointer on the taskbar.
    dwExStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;

    static auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
        GetModuleHandle(L"user32.dll"), "CreateWindowInBand");

    HWND hWnd = nullptr;
    if (pCreateWindowInBand) {
        // A window can't be owned by a window of another band. Without an
        // owner, it's a tool window to stay off the taskbar and Alt+Tab.
        hWnd = pCreateWindowInBand(dwExStyle | WS_EX_TOOLWINDOW, lpClassName,
                                   lpWindowName, dwStyle, X, Y, nWidth, nHeight,
                                   nullptr, hMenu, hInstance, lpParam,
                                   kZbidSystemTools);
        if (!hWnd) {
            Wh_Log(L"CreateWindowInBand failed: %u", GetLastError());
        }
    }

    if (!hWnd) {
        hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                        dwStyle, X, Y, nWidth, nHeight,
                                        hWndParent, hMenu, hInstance, lpParam);
    }

    if (hWnd) {
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
    }

    return hWnd;
}

// Returns the cursor's monitor in root coordinates, given the cursor's root
// coordinates.
winrt::Windows::Foundation::Rect GetCursorMonitorRect(
    double cursorX,
    double cursorY,
    double rasterizationScale) {
    POINT pt;
    GetCursorPos(&pt);

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST),
                   &monitorInfo);
    const RECT& rc = monitorInfo.rcMonitor;

    return {
        static_cast<float>(cursorX + (rc.left - pt.x) / rasterizationScale),
        static_cast<float>(cursorY + (rc.top - pt.y) / rasterizationScale),
        static_cast<float>((rc.right - rc.left) / rasterizationScale),
        static_cast<float>((rc.bottom - rc.top) / rasterizationScale),
    };
}

void ShowVolumeTooltip(FrameworkElement taskbarFrame,
                       double cursorX,
                       double cursorY,
                       bool isOverflowPopup,
                       PCWSTR text) {
    auto xamlRoot = taskbarFrame ? taskbarFrame.XamlRoot() : nullptr;
    if (!xamlRoot) {
        return;
    }

    // The XAML root of a vertical taskbar is taller than it's wide, unless a
    // mod rotates the XAML root content to make the taskbar vertical.
    bool vertical = false;
    if (!isOverflowPopup) {
        auto xamlRootSize = xamlRoot.Size();
        auto xamlRootContent = xamlRoot.Content();
        vertical = xamlRootSize.Height > xamlRootSize.Width ||
                   (xamlRootContent &&
                    GetRotationAngle(xamlRootContent) != 0.0);
    }

    // Create the popup if it doesn't exist.
    if (!g_volumeTooltipState.popup) {
        Controls::Border border;
        border.IsHitTestVisible(false);
        border.Padding(ThicknessHelper::FromLengths(12, 6, 12, 6));
        border.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));

        Controls::TextBlock textBlock;
        textBlock.FontSize(12);
        border.Child(textBlock);

        Controls::Primitives::Popup popup;
        popup.IsHitTestVisible(false);
        // A vertical taskbar is usually narrower than the tooltip.
        if (isOverflowPopup || vertical) {
            popup.ShouldConstrainToRootBounds(false);
        }
        popup.Child(border);

        // Update position when the border size changes.
        border.SizeChanged([](auto&&, auto&&) { UpdateTooltipPosition(); });

        g_volumeTooltipState.popup = popup;
    }

    // Store taskbarFrame only for main taskbar (element-relative coords).
    // null taskbarFrame indicates screen coordinates mode.
    g_volumeTooltipState.taskbarFrame =
        isOverflowPopup ? nullptr : taskbarFrame;
    g_volumeTooltipState.vertical = vertical;
    g_volumeTooltipState.monitorRect = GetCursorMonitorRect(
        cursorX, cursorY, xamlRoot.RasterizationScale());
    g_volumeTooltipState.cursorX = cursorX;
    g_volumeTooltipState.cursorY = cursorY;

    auto popup = g_volumeTooltipState.popup;
    auto border = popup.Child().try_as<Controls::Border>();

    // Update the text.
    if (auto textBlock = border.Child().try_as<Controls::TextBlock>()) {
        textBlock.Text(text);
    }

    // Use custom color or system accent color for styling.
    winrt::Windows::UI::Color bgColor;
    if (g_settings.tooltipColor) {
        bgColor = *g_settings.tooltipColor;
    } else {
        winrt::Windows::UI::ViewManagement::UISettings uiSettings;
        bgColor = uiSettings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Accent);
    }
    border.Background(Media::SolidColorBrush(bgColor));

    // Set text color to white for contrast.
    if (auto textBlock = border.Child().try_as<Controls::TextBlock>()) {
        textBlock.Foreground(
            Media::SolidColorBrush(winrt::Windows::UI::Colors::White()));
    }

    // Set XamlRoot and position near cursor.
    popup.XamlRoot(xamlRoot);

    UpdateTooltipPosition();

    g_openingTooltipPopup = true;
    popup.IsOpen(true);
    g_openingTooltipPopup = false;

    // Reset the hide timer.
    if (g_volumeTooltipState.hideTimer) {
        KillTimer(nullptr, g_volumeTooltipState.hideTimer);
    }
    g_volumeTooltipState.hideTimer =
        SetTimer(nullptr, 0, 1000, HideVolumeTooltipTimerProc);
}

void HideVolumeTooltip() {
    if (g_volumeTooltipState.hideTimer) {
        KillTimer(nullptr, g_volumeTooltipState.hideTimer);
        g_volumeTooltipState.hideTimer = 0;
    }

    if (g_volumeTooltipState.popup) {
        g_volumeTooltipState.popup.IsOpen(false);
    }

    // Destroy popup to disassociate from XamlRoot. A new one will be
    // created next time.
    g_volumeTooltipState.popup = nullptr;
    g_volumeTooltipState.taskbarFrame = nullptr;
    g_volumeTooltipState.vertical = false;
    g_volumeTooltipState.monitorRect = {};
    g_volumeTooltipState.cursorX = 0.0;
}

void ShowVolumeResultTooltip(UIElement element,
                             winrt::Windows::Foundation::Point rootPoint,
                             std::optional<AppVolumeResult> volumeResult) {
    auto [taskbarFrame, isOverflowPopup] = FindTaskbarFrameAncestor(element);
    if (!taskbarFrame) {
        return;
    }

    WCHAR tooltipText[64];
    if (!volumeResult) {
        wcscpy_s(tooltipText,
                 g_settings.terseFormat ? L"🔕" : L"No audio session");
    } else if (volumeResult->muted) {
        wcscpy_s(tooltipText, g_settings.terseFormat ? L"🔇" : L"Muted");
    } else {
        swprintf_s(tooltipText,
                   g_settings.terseFormat ? L"🔊 %d%%" : L"Volume: %d%%",
                   volumeResult->volume);
    }
    ShowVolumeTooltip(taskbarFrame, rootPoint.X, rootPoint.Y, isOverflowPopup,
                      tooltipText);
}

void ShowVolumeResultTooltip(UIElement element,
                             Input::PointerRoutedEventArgs args,
                             std::optional<AppVolumeResult> volumeResult) {
    // Transform from element-relative to root-relative coordinates.
    auto rootPoint = element.TransformToVisual(nullptr).TransformPoint(
        args.GetCurrentPoint(element).Position());
    ShowVolumeResultTooltip(element, rootPoint, volumeResult);
}

float VolumeChangeFromWheelDelta(double delta) {
    int step = g_settings.volumeChangeStep;
    if (!step) {
        step = 2;
    }
    return (float)delta * step * (0.01f / WHEEL_DELTA);
}

// Touchpad scrolling. Two-finger scrolling on some precision touchpads doesn't
// produce wheel input over the taskbar, so the touchpad is read as raw HID
// input instead, only while the cursor is over a taskbar button. A dedicated
// thread receives the input, and the volume is changed on the taskbar thread.

constexpr USAGE kHidUsageTouchPad = 0x05;
constexpr USAGE kHidUsageContactId = 0x51;
constexpr USAGE kHidUsageContactCount = 0x54;

// The finger movement of one volume step, relative to the touchpad height.
constexpr double kTouchpadStepHeight = 1.0 / 16;

// Wheel input is ignored for this long after two fingers were on the touchpad,
// for touchpads which produce wheel input too.
constexpr ULONGLONG kTouchpadWheelSuppressMs = 1000;

constexpr WCHAR kTouchpadWndClassName[] = L"Windhawk_TouchpadScroll_" WH_MOD_ID;

// wParam: whether the cursor is over a taskbar button.
constexpr UINT kTouchpadMsgHover = WM_APP + 1;
// Applies g_touchpadScrollDelta.
constexpr UINT kTouchpadMsgScroll = WM_APP + 2;

bool g_touchpadWndClassRegistered;
HANDLE g_touchpadThread;
// The window of the touchpad thread.
std::atomic<HWND> g_touchpadWnd;
// The window of the taskbar thread, created on the first hover.
std::atomic<HWND> g_touchpadUiWnd;
std::atomic<ULONGLONG> g_touchpadTwoFingersTick;
// The wheel delta which the taskbar thread didn't apply yet, WHEEL_DELTA per
// volume step, positive to raise the volume. A scroll message is posted when it
// becomes non-zero.
std::atomic<int> g_touchpadScrollDelta;

// Belongs to the taskbar thread.
struct {
    winrt::weak_ref<UIElement> element;
    // The touchpad thread window which was notified about the hover.
    HWND touchpadWnd;
    // The button's screen rectangle.
    RECT rect;
    // Map XAML root coordinates to screen coordinates.
    double originX;
    double originY;
    double scale;
} g_touchpadHover;

void TouchpadHoverClear() {
    HWND hTouchpadWnd = g_touchpadHover.touchpadWnd;
    g_touchpadHover.element = nullptr;
    g_touchpadHover.touchpadWnd = nullptr;
    if (hTouchpadWnd && hTouchpadWnd == g_touchpadWnd) {
        PostMessage(hTouchpadWnd, kTouchpadMsgHover, FALSE, 0);
    }
}

void ApplyTouchpadScroll(int delta) {
    auto element = g_touchpadHover.element.get();
    if (!element) {
        return;
    }

    // The hover isn't ended by exits with the pointer still over the button,
    // see TouchpadHoverEnd.
    POINT pt;
    GetCursorPos(&pt);
    if (!PtInRect(&g_touchpadHover.rect, pt)) {
        TouchpadHoverClear();
        return;
    }

    DWORD processId = GetProcessIdFromTaskListButton(element);
    if (!processId) {
        Wh_Log(L"Could not get process ID from taskbar button");
        return;
    }

    Wh_Log(L"Per-app volume (touchpad): PID=%u, delta=%d", processId, delta);

    auto volumeResult =
        AdjustAppVolume(processId, VolumeChangeFromWheelDelta(delta));

    ShowVolumeResultTooltip(
        element,
        winrt::Windows::Foundation::Point{
            static_cast<float>((pt.x - g_touchpadHover.originX) /
                               g_touchpadHover.scale),
            static_cast<float>((pt.y - g_touchpadHover.originY) /
                               g_touchpadHover.scale),
        },
        volumeResult);
}

// Everything below up to TouchpadWndProc belongs to the touchpad thread.

struct TouchpadDevice {
    std::vector<BYTE> preparsed;
    // The link collections of the contact slots.
    std::vector<USHORT> fingerCollections;
    USHORT contactCountCollection;
    double rangeY;
};

// Devices which can't be used are kept as nullopt.
std::unordered_map<HANDLE, std::optional<TouchpadDevice>> g_touchpadDevices;

struct TouchpadContact {
    ULONG id;
    ULONG y;
};

struct {
    bool registered;
    std::vector<BYTE> inputBuffer;
    // A frame's contacts can be spread over several reports, the first of
    // which has the contact count.
    std::vector<TouchpadContact> frame;
    ULONG frameExpected;
    ULONG frameReceived;
    // Two contacts are tracked while they move.
    bool tracking;
    ULONG trackedIds[2];
    double lastY;
    double accumulatedY;
} g_touchpadState;

const TouchpadDevice* GetTouchpadDevice(HANDLE hDevice) {
    auto [it, inserted] = g_touchpadDevices.try_emplace(hDevice);
    if (!inserted) {
        return it->second ? &*it->second : nullptr;
    }

    TouchpadDevice dev{};

    UINT size = 0;
    if (GetRawInputDeviceInfo(hDevice, RIDI_PREPARSEDDATA, nullptr, &size) !=
            0 ||
        !size) {
        return nullptr;
    }

    dev.preparsed.resize(size);
    if (GetRawInputDeviceInfo(hDevice, RIDI_PREPARSEDDATA, dev.preparsed.data(),
                              &size) == (UINT)-1) {
        return nullptr;
    }

    auto preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();
    HIDP_CAPS caps;
    if (HidP_GetCaps(preparsed, &caps) != HIDP_STATUS_SUCCESS) {
        return nullptr;
    }

    USHORT valueCapsCount = caps.NumberInputValueCaps;
    std::vector<HIDP_VALUE_CAPS> valueCaps(valueCapsCount);
    if (!valueCapsCount ||
        HidP_GetValueCaps(HidP_Input, valueCaps.data(), &valueCapsCount,
                          preparsed) != HIDP_STATUS_SUCCESS) {
        return nullptr;
    }

    bool hasContactCount = false;
    for (USHORT i = 0; i < valueCapsCount; i++) {
        const HIDP_VALUE_CAPS& vc = valueCaps[i];
        if (vc.IsRange) {
            continue;
        }

        if (vc.UsagePage == HID_USAGE_PAGE_DIGITIZER &&
            vc.NotRange.Usage == kHidUsageContactCount) {
            hasContactCount = true;
            dev.contactCountCollection = vc.LinkCollection;
        } else if (vc.UsagePage == HID_USAGE_PAGE_GENERIC &&
                   vc.NotRange.Usage == HID_USAGE_GENERIC_Y) {
            dev.fingerCollections.push_back(vc.LinkCollection);
            if (!dev.rangeY) {
                LONGLONG range = (LONGLONG)vc.LogicalMax - vc.LogicalMin;
                // Some devices declare a maximum which only fits unsigned.
                if (range <= 0 && vc.BitSize > 0 && vc.BitSize <= 32) {
                    range = (1LL << vc.BitSize) - 1;
                }
                dev.rangeY = (double)range;
            }
        }
    }

    bool valid =
        hasContactCount && !dev.fingerCollections.empty() && dev.rangeY > 0;

    Wh_Log(L"Touchpad %p: valid=%d, slots=%u, range=%.0f", hDevice, valid,
           (UINT)dev.fingerCollections.size(), dev.rangeY);

    if (!valid) {
        return nullptr;
    }

    it->second = std::move(dev);
    return &*it->second;
}

void OnTouchpadFrame(const TouchpadDevice& dev) {
    auto& state = g_touchpadState;

    if (state.frame.size() != 2) {
        state.tracking = false;
        return;
    }

    g_touchpadTwoFingersTick = GetTickCount64();

    const TouchpadContact& a = state.frame[0];
    const TouchpadContact& b = state.frame[1];
    double y = ((double)a.y + b.y) / 2;

    if (!state.tracking ||
        !((a.id == state.trackedIds[0] && b.id == state.trackedIds[1]) ||
          (a.id == state.trackedIds[1] && b.id == state.trackedIds[0]))) {
        state.tracking = true;
        state.trackedIds[0] = a.id;
        state.trackedIds[1] = b.id;
        state.lastY = y;
        state.accumulatedY = 0;
        return;
    }

    // Y grows downwards, and moving up raises the volume.
    state.accumulatedY += state.lastY - y;
    state.lastY = y;

    double deltaHeight = dev.rangeY * kTouchpadStepHeight / WHEEL_DELTA;
    int delta = (int)(state.accumulatedY / deltaHeight);
    if (!delta) {
        return;
    }

    state.accumulatedY -= delta * deltaHeight;

    // GetKeyState doesn't reflect the keyboard on this thread.
    if (g_settings.ctrlScrollVolumeChange &&
        GetAsyncKeyState(VK_CONTROL) >= 0) {
        return;
    }

    if (HWND hUiWnd = g_touchpadUiWnd) {
        if (g_touchpadScrollDelta.fetch_add(delta) == 0) {
            PostMessage(hUiWnd, kTouchpadMsgScroll, 0, 0);
        }
    }
}

void OnTouchpadReport(const TouchpadDevice& dev, PCHAR report, ULONG length) {
    auto& state = g_touchpadState;
    auto preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();

    ULONG count = 0;
    if (HidP_GetUsageValue(HidP_Input, HID_USAGE_PAGE_DIGITIZER,
                           dev.contactCountCollection, kHidUsageContactCount,
                           &count, preparsed, report,
                           length) == HIDP_STATUS_SUCCESS &&
        count > 0) {
        state.frame.clear();
        state.frameExpected = count;
        state.frameReceived = 0;
    }

    // The rest of a frame whose start was missed.
    if (!state.frameExpected) {
        return;
    }

    for (USHORT collection : dev.fingerCollections) {
        if (state.frameReceived == state.frameExpected) {
            break;
        }

        ULONG y;
        if (HidP_GetUsageValue(HidP_Input, HID_USAGE_PAGE_GENERIC, collection,
                               HID_USAGE_GENERIC_Y, &y, preparsed, report,
                               length) != HIDP_STATUS_SUCCESS) {
            continue;
        }

        state.frameReceived++;

        USAGE usages[16];
        ULONG usageCount = ARRAYSIZE(usages);
        if (HidP_GetUsages(HidP_Input, HID_USAGE_PAGE_DIGITIZER, collection,
                           usages, &usageCount, preparsed, report,
                           length) != HIDP_STATUS_SUCCESS ||
            std::find(usages, usages + usageCount,
                      HID_USAGE_DIGITIZER_TIP_SWITCH) == usages + usageCount) {
            continue;
        }

        ULONG id = collection;
        HidP_GetUsageValue(HidP_Input, HID_USAGE_PAGE_DIGITIZER, collection,
                           kHidUsageContactId, &id, preparsed, report, length);

        state.frame.push_back({id, y});
    }

    if (state.frameReceived == state.frameExpected) {
        OnTouchpadFrame(dev);
        state.frameExpected = 0;
    }
}

void OnTouchpadRawInput(HRAWINPUT hRawInput) {
    auto& state = g_touchpadState;

    // Input which was queued before the registration was removed.
    if (!state.registered) {
        return;
    }

    UINT size = 0;
    if (GetRawInputData(hRawInput, RID_INPUT, nullptr, &size,
                        sizeof(RAWINPUTHEADER)) != 0 ||
        !size) {
        return;
    }

    state.inputBuffer.resize(size);
    if (GetRawInputData(hRawInput, RID_INPUT, state.inputBuffer.data(), &size,
                        sizeof(RAWINPUTHEADER)) == (UINT)-1) {
        return;
    }

    const RAWINPUT* raw = (const RAWINPUT*)state.inputBuffer.data();
    if (raw->header.dwType != RIM_TYPEHID) {
        return;
    }

    const TouchpadDevice* dev = GetTouchpadDevice(raw->header.hDevice);
    if (!dev) {
        return;
    }

    const RAWHID& hid = raw->data.hid;
    for (DWORD i = 0; i < hid.dwCount; i++) {
        OnTouchpadReport(*dev, (PCHAR)hid.bRawData + i * hid.dwSizeHid,
                         hid.dwSizeHid);
    }
}

// Returns whether the touchpad raw input of the process is registered, and to
// which window.
bool GetTouchpadRawInputTarget(HWND* hTargetWnd) {
    UINT count = 0;
    GetRegisteredRawInputDevices(nullptr, &count, sizeof(RAWINPUTDEVICE));
    if (!count) {
        return false;
    }

    std::vector<RAWINPUTDEVICE> devices(count);
    if (GetRegisteredRawInputDevices(devices.data(), &count,
                                     sizeof(RAWINPUTDEVICE)) == (UINT)-1) {
        return false;
    }

    for (const auto& device : devices) {
        if (device.usUsagePage == HID_USAGE_PAGE_DIGITIZER &&
            device.usUsage == kHidUsageTouchPad) {
            *hTargetWnd = device.hwndTarget;
            return true;
        }
    }

    return false;
}

// Raw input of a device type goes to a single window in a process, so a
// registration of someone else is neither taken over nor removed.
void SetTouchpadHovered(HWND hWnd, bool hovered) {
    auto& state = g_touchpadState;
    if (hovered == state.registered) {
        return;
    }

    HWND hTargetWnd = nullptr;
    bool hasTarget = GetTouchpadRawInputTarget(&hTargetWnd);

    if (hovered) {
        if (hasTarget) {
            Wh_Log(L"Touchpad raw input is already registered to %p",
                   hTargetWnd);
            return;
        }

        RAWINPUTDEVICE rid{
            .usUsagePage = HID_USAGE_PAGE_DIGITIZER,
            .usUsage = kHidUsageTouchPad,
            .dwFlags = RIDEV_INPUTSINK,
            .hwndTarget = hWnd,
        };
        if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
            Wh_Log(L"RegisterRawInputDevices failed: %u", GetLastError());
            return;
        }
    } else {
        if (hasTarget && hTargetWnd == hWnd) {
            RAWINPUTDEVICE rid{
                .usUsagePage = HID_USAGE_PAGE_DIGITIZER,
                .usUsage = kHidUsageTouchPad,
                .dwFlags = RIDEV_REMOVE,
            };
            RegisterRawInputDevices(&rid, 1, sizeof(rid));
        }

        // A device handle can be reused by another device.
        g_touchpadDevices.clear();
        state.frameExpected = 0;
        state.tracking = false;
    }

    state.registered = hovered;
}

// Serves both the touchpad thread window and the taskbar thread window.
LRESULT CALLBACK TouchpadWndProc(HWND hWnd,
                                 UINT uMsg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    switch (uMsg) {
        case WM_INPUT:
            OnTouchpadRawInput((HRAWINPUT)lParam);
            break;

        case kTouchpadMsgHover:
            SetTouchpadHovered(hWnd, wParam);
            return 0;

        case kTouchpadMsgScroll:
            if (int delta = g_touchpadScrollDelta.exchange(0)) {
                ApplyTouchpadScroll(delta);
            }
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HINSTANCE GetTouchpadWndInstance() {
    HINSTANCE hInstance = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&TouchpadWndProc, &hInstance);
    return hInstance;
}

HWND CreateTouchpadWnd() {
    HWND hWnd = CreateWindowEx(0, kTouchpadWndClassName, nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, GetTouchpadWndInstance(),
                               nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
    }

    return hWnd;
}

// Called on the taskbar thread on pointer moves over a taskbar button.
void TouchpadHoverUpdate(UIElement element,
                         Input::PointerRoutedEventArgs args) {
    HWND hTouchpadWnd = g_touchpadWnd;
    if (!hTouchpadWnd) {
        return;
    }

    auto frameworkElement = element.try_as<FrameworkElement>();
    auto xamlRoot = element.XamlRoot();
    if (!frameworkElement || !xamlRoot) {
        return;
    }

    if (g_touchpadHover.element.get() != element ||
        g_touchpadHover.touchpadWnd != hTouchpadWnd) {
        HWND hUiWnd = g_touchpadUiWnd;
        if (!hUiWnd) {
            hUiWnd = CreateTouchpadWnd();
            g_touchpadUiWnd = hUiWnd;
        }

        // The scrolling is applied on the thread of this window.
        if (!hUiWnd ||
            GetWindowThreadProcessId(hUiWnd, nullptr) != GetCurrentThreadId()) {
            return;
        }

        g_touchpadHover.element = element;
        g_touchpadHover.touchpadWnd = hTouchpadWnd;
        PostMessage(hTouchpadWnd, kTouchpadMsgHover, TRUE, 0);
    }

    // The cursor position is read later than the event's position, so the
    // geometry is refreshed on each move, until the cursor rests.
    POINT pt;
    GetCursorPos(&pt);

    auto transform = element.TransformToVisual(nullptr);
    auto rootPoint =
        transform.TransformPoint(args.GetCurrentPoint(element).Position());
    auto bounds = transform.TransformBounds(winrt::Windows::Foundation::Rect{
        0,
        0,
        static_cast<float>(frameworkElement.ActualWidth()),
        static_cast<float>(frameworkElement.ActualHeight()),
    });

    double scale = xamlRoot.RasterizationScale();
    double originX = pt.x - rootPoint.X * scale;
    double originY = pt.y - rootPoint.Y * scale;

    g_touchpadHover.rect = {
        .left = (LONG)std::lround(originX + bounds.X * scale),
        .top = (LONG)std::lround(originY + bounds.Y * scale),
        .right = (LONG)std::lround(originX + (bounds.X + bounds.Width) * scale),
        .bottom =
            (LONG)std::lround(originY + (bounds.Y + bounds.Height) * scale),
    };
    g_touchpadHover.originX = originX;
    g_touchpadHover.originY = originY;
    g_touchpadHover.scale = scale;
}

// Called on the taskbar thread when the pointer exits a taskbar button.
void TouchpadHoverEnd(UIElement element, Input::PointerRoutedEventArgs args) {
    if (g_touchpadHover.element.get() != element) {
        return;
    }

    // An exit with the pointer still over the button might be due to something
    // else taking the pointer, such as a touchpad gesture.
    auto frameworkElement = element.try_as<FrameworkElement>();
    auto position = args.GetCurrentPoint(element).Position();
    if (frameworkElement && position.X >= 0 && position.Y >= 0 &&
        position.X < frameworkElement.ActualWidth() &&
        position.Y < frameworkElement.ActualHeight()) {
        return;
    }

    TouchpadHoverClear();
}

DWORD WINAPI TouchpadThread(LPVOID lpParameter) {
    HANDLE hReadyEvent = (HANDLE)lpParameter;

    HWND hWnd = CreateTouchpadWnd();
    g_touchpadWnd = hWnd;

    SetEvent(hReadyEvent);

    if (!hWnd) {
        return 1;
    }

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            break;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SetTouchpadHovered(hWnd, false);
    DestroyWindow(hWnd);
    return 0;
}

void TouchpadThreadInit() {
    if (g_touchpadThread) {
        return;
    }

    // Kept registered until the mod is unloaded, since the taskbar thread
    // window outlives the thread.
    if (!g_touchpadWndClassRegistered) {
        WNDCLASS wc{
            .lpfnWndProc = TouchpadWndProc,
            .hInstance = GetTouchpadWndInstance(),
            .lpszClassName = kTouchpadWndClassName,
        };
        if (!RegisterClass(&wc)) {
            Wh_Log(L"RegisterClass failed: %u", GetLastError());
            return;
        }

        g_touchpadWndClassRegistered = true;
    }

    HANDLE hReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hReadyEvent) {
        return;
    }

    g_touchpadThread =
        CreateThread(nullptr, 0, TouchpadThread, hReadyEvent, 0, nullptr);
    if (g_touchpadThread) {
        WaitForSingleObject(hReadyEvent, INFINITE);
    }

    CloseHandle(hReadyEvent);
}

void TouchpadThreadUninit() {
    if (g_touchpadThread) {
        g_touchpadWnd = nullptr;
        PostThreadMessage(GetThreadId(g_touchpadThread), WM_APP, 0, 0);
        WaitForSingleObject(g_touchpadThread, INFINITE);
        CloseHandle(g_touchpadThread);
        g_touchpadThread = nullptr;
    }
}

// Per-app volume wheel scroll handling.
using TaskListButton_OnPointerWheelChanged_t = int(WINAPI*)(void* pThis,
                                                            void* pArgs);
TaskListButton_OnPointerWheelChanged_t
    TaskListButton_OnPointerWheelChanged_Original;
int WINAPI TaskListButton_OnPointerWheelChanged_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskListButton_OnPointerWheelChanged_Original(pThis, pArgs);
    };

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    // If Ctrl + Scroll is required, skip if Ctrl is not pressed.
    if (g_settings.ctrlScrollVolumeChange && GetKeyState(VK_CONTROL) >= 0) {
        return original();
    }

    if (g_settings.touchpadScroll &&
        GetTickCount64() - g_touchpadTwoFingersTick <
            kTouchpadWheelSuppressMs) {
        return original();
    }

    double delta = args.GetCurrentPoint(element).Properties().MouseWheelDelta();
    if (!delta) {
        return original();
    }

    // Get process ID from the taskbar button.
    DWORD processId = GetProcessIdFromTaskListButton(element);
    if (!processId) {
        Wh_Log(L"Could not get process ID from taskbar button");
        return original();
    }

    Wh_Log(L"Per-app volume: PID=%u, delta=%f", processId, delta);

    // Adjust the app's volume.
    auto volumeResult =
        AdjustAppVolume(processId, VolumeChangeFromWheelDelta(delta));

    ShowVolumeResultTooltip(element, args, volumeResult);

    // Mark event as handled.
    args.Handled(true);
    return 0;
}

using TaskListButton_OnPointerExited_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerExited_t TaskListButton_OnPointerExited_Original;
int WINAPI TaskListButton_OnPointerExited_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskListButton_OnPointerExited_Original(pThis, pArgs);
    };

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    HideVolumeTooltip();

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (args) {
        TouchpadHoverEnd(element, args);
    }

    return original();
}

using TaskListButton_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerMoved_t TaskListButton_OnPointerMoved_Original;
int WINAPI TaskListButton_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskListButton_OnPointerMoved_Original(pThis, pArgs);
    };

    bool tooltipShown =
        g_volumeTooltipState.popup && g_volumeTooltipState.popup.IsOpen();
    if (!tooltipShown && !g_touchpadWnd) {
        return original();
    }

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    TouchpadHoverUpdate(element, args);

    // Only update if tooltip is currently shown.
    if (!tooltipShown) {
        return original();
    }

    auto ancestorFrame = FindTaskbarFrameAncestor(element).element;
    if (ancestorFrame) {
        auto point = args.GetCurrentPoint(ancestorFrame);
        // Transform from ancestorFrame-relative to root-relative coordinates.
        auto transform = ancestorFrame.TransformToVisual(nullptr);
        auto rootPoint =
            transform.TransformPoint({static_cast<float>(point.Position().X),
                                      static_cast<float>(point.Position().Y)});
        g_volumeTooltipState.cursorX = rootPoint.X;
        g_volumeTooltipState.cursorY = rootPoint.Y;
    }

    UpdateTooltipPosition();

    return original();
}

using TaskListButton_OnPointerPressed_t = int(WINAPI*)(void* pThis,
                                                       void* pArgs);
TaskListButton_OnPointerPressed_t TaskListButton_OnPointerPressed_Original;
int WINAPI TaskListButton_OnPointerPressed_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskListButton_OnPointerPressed_Original(pThis, pArgs);
    };

    if (!g_settings.ctrlClickToMute && !g_settings.middleClickToMute) {
        return original();
    }

    // Skip if Ctrl is not pressed and middle click to mute is not enabled.
    if (GetKeyState(VK_CONTROL) >= 0 && !g_settings.middleClickToMute) {
        return original();
    }

    UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<UIElement>(), winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.TaskListButton") {
        return original();
    }

    Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    if (!args) {
        return original();
    }

    // Check if Ctrl is pressed or middle button is pressed.
    bool isCtrlPressed = GetKeyState(VK_CONTROL) < 0;
    auto point = args.GetCurrentPoint(element);
    bool isMiddleButton = point.Properties().IsMiddleButtonPressed();

    if (!(g_settings.ctrlClickToMute && isCtrlPressed) &&
        !(g_settings.middleClickToMute && isMiddleButton)) {
        return original();
    }

    // Get process ID from the taskbar button.
    DWORD processId = GetProcessIdFromTaskListButton(element);
    if (!processId) {
        Wh_Log(L"Could not get process ID from taskbar button");
        return original();
    }

    Wh_Log(L"Click to toggle mute: PID=%u", processId);

    // Toggle mute state.
    auto muteResult = ToggleAppMute(processId);

    ShowVolumeResultTooltip(element, args, muteResult);

    // Mark event as handled to prevent normal click behavior.
    args.Handled(true);
    return 0;
}

void LoadSettings() {
    g_settings.volumeChangeStep = Wh_GetIntSetting(L"volumeChangeStep");
    g_settings.ctrlClickToMute = Wh_GetIntSetting(L"ctrlClickToMute");
    g_settings.middleClickToMute = Wh_GetIntSetting(L"middleClickToMute");
    g_settings.ctrlScrollVolumeChange =
        Wh_GetIntSetting(L"ctrlScrollVolumeChange");
    g_settings.touchpadScroll = Wh_GetIntSetting(L"touchpadScroll");
    g_settings.terseFormat = Wh_GetIntSetting(L"terseFormat");

    g_settings.tooltipColor.reset();
    PCWSTR tooltipColorStr = Wh_GetStringSetting(L"tooltipColor");
    size_t len = wcslen(tooltipColorStr);
    if (tooltipColorStr[0] == L'#' && (len == 7 || len == 9)) {
        unsigned long hex = wcstoul(tooltipColorStr + 1, nullptr, 16);
        if (len == 7) {
            hex |= 0xFF000000;
        }
        g_settings.tooltipColor = winrt::Windows::UI::Color{
            static_cast<uint8_t>((hex >> 24) & 0xFF),
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF),
        };
    }
    Wh_FreeStringSetting(tooltipColorStr);

    g_settings.noAutomaticMuteToggle =
        Wh_GetIntSetting(L"noAutomaticMuteToggle");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))"},
            &TaskListButton_OnPointerWheelChanged_Original,
            TaskListButton_OnPointerWheelChanged_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &TaskListButton_OnPointerExited_Original,
            TaskListButton_OnPointerExited_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskListButton_OnPointerMoved_Original,
            TaskListButton_OnPointerMoved_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"},
            &TaskListButton_OnPointerPressed_Original,
            TaskListButton_OnPointerPressed_Hook,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::IsMultiWindow(void)const )"},
            &TaskListGroupViewModel_IsMultiWindow_Original,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskGroup<struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::IsRunning(void)const )"},
            &ITaskGroup_IsRunning_Original,
            ITaskGroup_IsRunning_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll for per-app volume");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetAppWindow(void))"},
            &CImmersiveTaskItem_GetAppWindow,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable')"},
            &CImmersiveTaskItem_vftable,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable_ITaskItem,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskGroup,struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::ReportClicked(void *))"},
            &TaskGroup_ReportClicked_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    if (g_settings.touchpadScroll) {
        TouchpadThreadInit();
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    TouchpadThreadUninit();

    if (HWND hTouchpadUiWnd = g_touchpadUiWnd.exchange(nullptr)) {
        SendMessage(hTouchpadUiWnd, WM_CLOSE, 0, 0);
    }

    if (g_touchpadWndClassRegistered) {
        UnregisterClass(kTouchpadWndClassName, GetTouchpadWndInstance());
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        RunFromWindowThread(
            hTaskbarWnd,
            [](void*) {
                HideVolumeTooltip();
                SndVolUninit();
                g_touchpadHover.element = nullptr;
            },
            nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    if (g_settings.touchpadScroll) {
        TouchpadThreadInit();
    } else {
        TouchpadThreadUninit();
    }
}
