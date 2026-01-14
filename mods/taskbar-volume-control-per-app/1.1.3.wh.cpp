// ==WindhawkMod==
// @id              taskbar-volume-control-per-app
// @name            Taskbar Volume Control Per-App
// @description     Control the per-app volume by scrolling over taskbar buttons
// @version         1.1.3
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lntdll -lole32 -loleaut32 -lruntimeobject
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
application. Ctrl+clicking on a taskbar button will toggle mute for that app. A
tooltip shows the current volume percentage, or "No audio session" if the app
has no active audio.

For controlling the overall system volume, check out the [Taskbar Volume
Control](https://windhawk.net/mods/taskbar-volume-control) mod. Note that both
mods can be used simultaneously using one of these approaches:
1. Configure Taskbar Volume Control to use a limited region, such as the system
   tray area, while this mod handles the app-specific volume on the task
   buttons.
2. Configure one of the mods to require the Ctrl key for volume changes, so that
   holding Ctrl while scrolling adjusts the app volume (this mod), and normal
   scrolling adjusts the system volume (Taskbar Volume Control), or vice versa.

Note that if both mods are configured to act simultaneously, the Taskbar Volume
Control mod takes precedence due to the way the mod works.

![Demonstration](https://i.imgur.com/56QHjUv.gif)
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
- ctrlScrollVolumeChange: false
  $name: Ctrl + Scroll to change volume
  $description: >-
    When enabled, scrolling the mouse wheel will only change the volume when
    the Ctrl key is held down.
- terseFormat: false
  $name: Terse format
  $description: >-
    When enabled, the tooltip shows a compact format with emojis instead of
    text (e.g., "🔊 64%" instead of "Volume: 64%").
- noAutomaticMuteToggle: false
  $name: No automatic mute toggle
  $description: >-
    By default, the app is muted once the volume reaches zero, and is unmuted
    on any change to a non-zero volume. Enabling this option turns off this
    functionality, such that the app mute status is not changed.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <tlhelp32.h>
#include <winternl.h>

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
    bool ctrlScrollVolumeChange;
    bool terseFormat;
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
// Returns true to continue iterating, false to stop.
using AudioSessionCallback =
    std::function<bool(ISimpleAudioVolume* simpleVolume)>;

// Iterates over all audio sessions for a given process ID.
// Calls the callback for each matching session's ISimpleAudioVolume.
// Returns true if at least one session was found and processed.
bool ForEachAudioSession(DWORD targetPID,
                         const AudioSessionCallback& callback) {
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

        DWORD sessionPID = 0;
        hr = sessionControl2->GetProcessId(&sessionPID);
        if (FAILED(hr) || sessionPID != targetPID) {
            continue;
        }

        // Skip system sounds session.
        if (sessionControl2->IsSystemSoundsSession() == S_OK) {
            continue;
        }

        winrt::com_ptr<ISimpleAudioVolume> simpleVolume;
        hr = sessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume),
                                             simpleVolume.put_void());
        if (FAILED(hr)) {
            continue;
        }

        foundSession = true;
        if (!callback(simpleVolume.get())) {
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
            for (int i = 0; i < argc; i++) {
                if (wcscmp(argv[i], L"--type=utility") == 0) {
                    isUtility = true;
                } else if (wcscmp(argv[i],
                                  L"--utility-sub-type=audio.mojom."
                                  L"AudioService") == 0) {
                    isAudioService = true;
                }
            }

            LocalFree(argv);

            if (isUtility && isAudioService) {
                audioSubprocessPID = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &pe32));
    }

    CloseHandle(snapshot);
    return audioSubprocessPID;
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

    ForEachAudioSession(targetPID, [&](ISimpleAudioVolume* vol) {
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

// Get the current volume for a process (returns 0-100 or -1 if no session).
int GetAppVolumeForPID(DWORD targetPID) {
    int volumePercent = -1;

    ForEachAudioSession(targetPID, [&](ISimpleAudioVolume* vol) {
        float currentVolume = 0.0f;
        HRESULT hr = vol->GetMasterVolume(&currentVolume);
        if (SUCCEEDED(hr)) {
            volumePercent = (int)(currentVolume * 100.0f + 0.5f);
            return false;  // Stop after finding first session.
        }
        return true;  // Continue to next session.
    });

    return volumePercent;
}

int GetAppVolume(DWORD targetPID) {
    int result = GetAppVolumeForPID(targetPID);
    if (result >= 0) {
        return result;
    }

    // Try Chromium audio subprocess if no session found.
    DWORD audioSubprocessPID = FindChromiumAudioSubprocess(targetPID);
    if (audioSubprocessPID) {
        result = GetAppVolumeForPID(audioSubprocessPID);
    }

    return result;
}

// Toggle mute state for a process. Returns new mute state, or nullopt if no
// session.
std::optional<bool> ToggleAppMuteForPID(DWORD targetPID) {
    std::optional<bool> newMuteState;

    ForEachAudioSession(targetPID, [&](ISimpleAudioVolume* vol) {
        BOOL isMuted = FALSE;
        HRESULT hr = vol->GetMute(&isMuted);
        if (FAILED(hr)) {
            return true;  // Continue to next session.
        }

        hr = vol->SetMute(!isMuted, NULL);
        if (SUCCEEDED(hr)) {
            newMuteState = !isMuted;
        }

        return true;  // Continue to process all sessions.
    });

    return newMuteState;
}

std::optional<bool> ToggleAppMute(DWORD targetPID) {
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

struct {
    Controls::Primitives::Popup popup = nullptr;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    double rotationAngle = 0.0;
    double cursorX = 0.0;
    double cursorY = 0.0;
    UINT_PTR hideTimer = 0;
} g_volumeTooltipState;

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

// Get rotation angle from element's parent RenderTransform.
// Used to detect vertical taskbar orientation.
double GetParentRotationAngle(FrameworkElement element) {
    auto parent =
        Media::VisualTreeHelper::GetParent(element).try_as<UIElement>();
    if (!parent) {
        return 0.0;
    }

    auto transform = parent.RenderTransform();
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
    double tooltipHeight = border ? border.ActualHeight() : 0;

    auto taskbarFrame = g_volumeTooltipState.taskbarFrame.get();
    if (!taskbarFrame) {
        // Overflow popup: use stored cursor coordinates.
        popup.HorizontalOffset(g_volumeTooltipState.cursorX + kTooltipOffset);
        popup.VerticalOffset(g_volumeTooltipState.cursorY - tooltipHeight / 2);
    } else {
        // Main taskbar: use element-relative coordinates.
        double cursorX = g_volumeTooltipState.cursorX;
        double cursorY = g_volumeTooltipState.cursorY;

        if (g_volumeTooltipState.rotationAngle == 0.0) {
            // Horizontal taskbar.
            double taskbarHeight = taskbarFrame.ActualHeight();
            popup.HorizontalOffset(cursorX + kTooltipOffset);
            popup.VerticalOffset(
                std::max((taskbarHeight - tooltipHeight) / 2,
                         static_cast<double>(kTooltipOffsetTaskbarEdge)));
        } else {
            // Vertical taskbar.
            popup.HorizontalOffset(kTooltipOffsetTaskbarEdge);
            popup.VerticalOffset(cursorY + kTooltipOffset);
        }
    }
}

void ShowVolumeTooltip(FrameworkElement taskbarFrame,
                       double cursorX,
                       double cursorY,
                       bool isOverflowPopup,
                       PCWSTR text) {
    if (!taskbarFrame) {
        return;
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
        if (isOverflowPopup) {
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
    g_volumeTooltipState.cursorX = cursorX;
    g_volumeTooltipState.cursorY = cursorY;

    auto popup = g_volumeTooltipState.popup;
    auto border = popup.Child().try_as<Controls::Border>();

    // Update the text.
    if (auto textBlock = border.Child().try_as<Controls::TextBlock>()) {
        textBlock.Text(text);
    }

    // Use system accent color for styling.
    winrt::Windows::UI::ViewManagement::UISettings uiSettings;
    auto accentColor = uiSettings.GetColorValue(
        winrt::Windows::UI::ViewManagement::UIColorType::Accent);
    border.Background(Media::SolidColorBrush(accentColor));

    // Set text color to white for contrast.
    if (auto textBlock = border.Child().try_as<Controls::TextBlock>()) {
        textBlock.Foreground(
            Media::SolidColorBrush(winrt::Windows::UI::Colors::White()));
    }

    // Set XamlRoot and position near cursor.
    popup.XamlRoot(taskbarFrame.XamlRoot());

    // Check if taskbar frame's parent is rotated (vertical taskbar).
    g_volumeTooltipState.rotationAngle = GetParentRotationAngle(taskbarFrame);

    UpdateTooltipPosition();

    popup.IsOpen(true);

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
    g_volumeTooltipState.rotationAngle = 0.0;
    g_volumeTooltipState.cursorX = 0.0;
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

    // Calculate volume change.
    int step = g_settings.volumeChangeStep;
    if (!step) {
        step = 2;
    }
    float volumeChange = (float)delta * step * (0.01f / WHEEL_DELTA);

    // Adjust the app's volume.
    auto volumeResult = AdjustAppVolume(processId, volumeChange);

    // Show tooltip near cursor.
    auto [taskbarFrame, isOverflowPopup] = FindTaskbarFrameAncestor(element);
    if (taskbarFrame) {
        auto point = args.GetCurrentPoint(taskbarFrame);
        // Transform from taskbarFrame-relative to root-relative coordinates.
        auto transform = taskbarFrame.TransformToVisual(nullptr);
        auto rootPoint =
            transform.TransformPoint({static_cast<float>(point.Position().X),
                                      static_cast<float>(point.Position().Y)});
        double cursorX = rootPoint.X;
        double cursorY = rootPoint.Y;

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
        ShowVolumeTooltip(taskbarFrame, cursorX, cursorY, isOverflowPopup,
                          tooltipText);
    }

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

    return original();
}

using TaskListButton_OnPointerMoved_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerMoved_t TaskListButton_OnPointerMoved_Original;
int WINAPI TaskListButton_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskListButton_OnPointerMoved_Original(pThis, pArgs);
    };

    // Only update if tooltip is currently shown.
    if (!g_volumeTooltipState.popup || !g_volumeTooltipState.popup.IsOpen()) {
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

    if (!g_settings.ctrlClickToMute) {
        return original();
    }

    // Check if Ctrl is pressed.
    if (GetKeyState(VK_CONTROL) >= 0) {
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

    // Get process ID from the taskbar button.
    DWORD processId = GetProcessIdFromTaskListButton(element);
    if (!processId) {
        Wh_Log(L"Could not get process ID from taskbar button");
        return original();
    }

    Wh_Log(L"Ctrl+click to toggle mute: PID=%u", processId);

    // Toggle mute state.
    auto newMuteState = ToggleAppMute(processId);

    // Show tooltip near cursor.
    auto [taskbarFrame, isOverflowPopup] = FindTaskbarFrameAncestor(element);
    if (taskbarFrame) {
        auto point = args.GetCurrentPoint(taskbarFrame);
        // Transform from taskbarFrame-relative to root-relative coordinates.
        auto transform = taskbarFrame.TransformToVisual(nullptr);
        auto rootPoint =
            transform.TransformPoint({static_cast<float>(point.Position().X),
                                      static_cast<float>(point.Position().Y)});
        double cursorX = rootPoint.X;
        double cursorY = rootPoint.Y;

        WCHAR tooltipText[64];
        if (!newMuteState) {
            wcscpy_s(tooltipText,
                     g_settings.terseFormat ? L"🔕" : L"No audio session");
        } else if (*newMuteState) {
            wcscpy_s(tooltipText, g_settings.terseFormat ? L"🔇" : L"Muted");
        } else {
            // Get current volume to display.
            int volume = GetAppVolume(processId);
            if (volume >= 0) {
                swprintf_s(
                    tooltipText,
                    g_settings.terseFormat ? L"🔊 %d%%" : L"Volume: %d%%",
                    volume);
            } else {
                wcscpy_s(tooltipText,
                         g_settings.terseFormat ? L"🔕" : L"No audio session");
            }
        }
        ShowVolumeTooltip(taskbarFrame, cursorX, cursorY, isOverflowPopup,
                          tooltipText);
    }

    // Mark event as handled to prevent normal click behavior.
    args.Handled(true);
    return 0;
}

void LoadSettings() {
    g_settings.volumeChangeStep = Wh_GetIntSetting(L"volumeChangeStep");
    g_settings.ctrlClickToMute = Wh_GetIntSetting(L"ctrlClickToMute");
    g_settings.ctrlScrollVolumeChange =
        Wh_GetIntSetting(L"ctrlScrollVolumeChange");
    g_settings.terseFormat = Wh_GetIntSetting(L"terseFormat");
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
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

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

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        RunFromWindowThread(
            hTaskbarWnd,
            [](void*) {
                HideVolumeTooltip();
                SndVolUninit();
            },
            nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
