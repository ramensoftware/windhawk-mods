// ==WindhawkMod==
// @id              taskbar-jump-list-volume-control
// @name            Taskbar Jump List Volume Control
// @description     Adds a per-app volume slider to the taskbar jump list
// @version         1.0
// @author          Ara
// @github          https://github.com/ara-hwang
// @homepage        https://github.com/ara-hwang/windhawk-taskbar-jump-list-volume-control
// @license         GPL-3.0-only
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The task group/audio session code is based on the "Taskbar Volume Control
// Per-App" mod by m417z.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ara-hwang/windhawk-taskbar-jump-list-volume-control/issues

// ==WindhawkModReadme==
/*
# Taskbar Jump List Volume Control

Adds a volume slider and a mute button for the app to the jump list that opens
when you right-click a taskbar button on Windows 11.

![Demonstration](https://i.imgur.com/eZOdWZv.gif)

* The slider only appears for apps that have an audio session.
* Scrolling the mouse wheel or a precision touchpad over the slider also
  changes the volume.
* Apps that play audio from child processes (Chrome, Firefox, Discord, etc.)
  are supported.

## How it works

The jump list is rendered by `ShellExperienceHost.exe`, not by `explorer.exe`,
so this mod is loaded into both processes. In `explorer.exe` it finds the app of
the right-clicked button and its audio sessions, and in `ShellExperienceHost.exe`
it inserts the slider into the jump list.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wheelStep: 2
  $name: Mouse wheel step
  $description: Volume change (in %) per mouse wheel notch over the slider
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <audiopolicy.h>
#include <commctrl.h>
#include <mmdeviceapi.h>
#include <tlhelp32.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

////////////////////////////////////////////////////////////////////////////////
// Definitions shared by both processes.

// Window in explorer.exe. ShellExperienceHost sends volume changes to it.
constexpr WCHAR kHostWndClass[] = L"WhJumpListVolume_Host";
// Window in ShellExperienceHost.exe. explorer sends the target app info to it.
constexpr WCHAR kShellWndClass[] = L"WhJumpListVolume_Shell";

constexpr UINT kMsgSetVolume = WM_APP + 0x51;  // wParam: 0-100.
constexpr UINT kMsgSetMute = WM_APP + 0x52;    // wParam: 0/1.

constexpr ULONG_PTR kCopyDataMagic = 0x4A4C5643;  // 'JLVC'

struct JumpListVolumeInfo {
    ULONGLONG hostWnd;
    DWORD hasAudio;
    DWORD volume;  // 0-100.
    DWORD muted;
};

struct {
    // Written on the Windhawk engine thread, read on the jump list UI thread.
    std::atomic<int> wheelStep;
} g_settings;

bool g_isShellExperienceHost;

void LoadSettings() {
    int wheelStep = Wh_GetIntSetting(L"wheelStep");
    g_settings.wheelStep = wheelStep < 1 ? 1 : wheelStep;
}

// The module handle of the mod itself, as opposed to the host exe. Window
// classes are registered with it, so that they're owned by the mod.
HINSTANCE GetModInstance() {
    static HINSTANCE instance = []() {
        HMODULE module = nullptr;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCWSTR)&GetModInstance, &module);
        return (HINSTANCE)module;
    }();
    return instance;
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

////////////////////////////////////////////////////////////////////////////////
// explorer.exe: audio sessions.

IMMDeviceEnumerator* g_pDeviceEnumerator;

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

// Receives the session's process ID and its volume interface.
using AudioSessionCallback =
    std::function<void(DWORD pid, ISimpleAudioVolume* volume)>;

// Iterates over all audio sessions of the default render device.
void ForEachAudioSession(const AudioSessionCallback& callback) {
    if (!g_pDeviceEnumerator) {
        SndVolInit();
        if (!g_pDeviceEnumerator) {
            return;
        }
    }

    winrt::com_ptr<IMMDevice> defaultDevice;
    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, defaultDevice.put());
    if (FAILED(hr)) {
        return;
    }

    winrt::com_ptr<IAudioSessionManager2> sessionManager;
    hr = defaultDevice->Activate(XIID_IAudioSessionManager2, CLSCTX_ALL, NULL,
                                 sessionManager.put_void());
    if (FAILED(hr)) {
        return;
    }

    winrt::com_ptr<IAudioSessionEnumerator> sessionEnumerator;
    hr = sessionManager->GetSessionEnumerator(sessionEnumerator.put());
    if (FAILED(hr)) {
        return;
    }

    int sessionCount = 0;
    hr = sessionEnumerator->GetCount(&sessionCount);
    if (FAILED(hr)) {
        return;
    }

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
        if (FAILED(hr) || !sessionPID) {
            continue;
        }

        winrt::com_ptr<ISimpleAudioVolume> volume;
        hr = sessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume),
                                             volume.put_void());
        if (FAILED(hr)) {
            continue;
        }

        callback(sessionPID, volume.get());
    }
}

ULONGLONG GetProcessCreationTime(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return 0;
    }

    ULONGLONG result = 0;
    FILETIME creation, exit, kernel, user;
    if (GetProcessTimes(process, &creation, &exit, &kernel, &user)) {
        result = ((ULONGLONG)creation.dwHighDateTime << 32) |
                 creation.dwLowDateTime;
    }

    CloseHandle(process);
    return result;
}

std::unordered_map<DWORD, DWORD> GetProcessParents() {
    std::unordered_map<DWORD, DWORD> parents;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return parents;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    if (Process32First(snapshot, &pe32)) {
        do {
            parents[pe32.th32ProcessID] = pe32.th32ParentProcessID;
        } while (Process32Next(snapshot, &pe32));
    }

    CloseHandle(snapshot);
    return parents;
}

// Processes which own a visible top-level window, i.e. apps of their own.
std::unordered_set<DWORD> GetProcessesWithWindows() {
    std::unordered_set<DWORD> result;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            if (IsWindowVisible(hWnd) && !GetWindow(hWnd, GW_OWNER) &&
                !(GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)) {
                DWORD pid = 0;
                if (GetWindowThreadProcessId(hWnd, &pid) && pid) {
                    ((std::unordered_set<DWORD>*)lParam)->insert(pid);
                }
            }
            return TRUE;
        },
        (LPARAM)&result);

    return result;
}

// Returns the PIDs of audio sessions which belong to the given app processes.
// A session belongs to an app if its process is one of the app processes, or
// a windowless descendant of one (Chromium's audio service, Firefox's content
// processes, etc.).
std::vector<DWORD> FindAudioSessionPids(const std::vector<DWORD>& appPids) {
    std::vector<DWORD> result;
    if (appPids.empty()) {
        return result;
    }

    std::unordered_set<DWORD> appPidSet(appPids.begin(), appPids.end());
    std::unordered_set<DWORD> sessionPids;
    ForEachAudioSession(
        [&](DWORD pid, ISimpleAudioVolume*) { sessionPids.insert(pid); });

    std::unordered_map<DWORD, DWORD> parents;
    std::unordered_set<DWORD> processesWithWindows;
    bool processInfoLoaded = false;

    for (DWORD sessionPid : sessionPids) {
        if (appPidSet.contains(sessionPid)) {
            result.push_back(sessionPid);
            continue;
        }

        if (!processInfoLoaded) {
            parents = GetProcessParents();
            processesWithWindows = GetProcessesWithWindows();
            processInfoLoaded = true;
        }

        DWORD pid = sessionPid;
        for (int depth = 0; depth < 8; depth++) {
            // A process with its own window is a different app.
            if (processesWithWindows.contains(pid)) {
                break;
            }

            auto it = parents.find(pid);
            if (it == parents.end() || !it->second) {
                break;
            }

            // Everything is launched by explorer, don't attribute to it.
            DWORD parentPid = it->second;
            if (parentPid == GetCurrentProcessId()) {
                break;
            }

            // The parent PID might have been reused by a newer process.
            ULONGLONG parentTime = GetProcessCreationTime(parentPid);
            ULONGLONG childTime = GetProcessCreationTime(pid);
            if (parentTime && childTime && parentTime > childTime) {
                break;
            }

            if (appPidSet.contains(parentPid)) {
                result.push_back(sessionPid);
                break;
            }

            pid = parentPid;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// explorer.exe: taskbar integration.

// Audio session PIDs of the app whose jump list was opened last. Only accessed
// from the taskbar thread.
std::vector<DWORD> g_targetSessionPids;

HWND g_hostWnd;

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow;

using CImmersiveTaskItem_GetAppWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetAppWindow_t CImmersiveTaskItem_GetAppWindow;

void* CImmersiveTaskItem_vftable;
void* CImmersiveTaskItem_vftable_ITaskItem;

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

std::vector<DWORD> GetProcessIdsFromTaskGroup(void* taskGroup) {
    std::vector<DWORD> pids;
    if (!taskGroup) {
        return pids;
    }

    HDPA taskItemsArray = GetTaskItemsArray(taskGroup);
    if (!taskItemsArray) {
        return pids;
    }

    int count = DPA_GetPtrCount(taskItemsArray);
    for (int i = 0; i < count; i++) {
        HWND hWnd = GetWindowFromTaskItem(DPA_GetPtr(taskItemsArray, i));
        DWORD pid = 0;
        if (hWnd && GetWindowThreadProcessId(hWnd, &pid) && pid &&
            std::find(pids.begin(), pids.end(), pid) == pids.end()) {
            pids.push_back(pid);
        }
    }

    return pids;
}

bool IsTargetSession(DWORD pid) {
    return std::find(g_targetSessionPids.begin(), g_targetSessionPids.end(),
                     pid) != g_targetSessionPids.end();
}

LRESULT CALLBACK HostWndProc(HWND hWnd,
                             UINT uMsg,
                             WPARAM wParam,
                             LPARAM lParam) {
    switch (uMsg) {
        case kMsgSetVolume: {
            float volume = (float)(wParam > 100 ? 100 : wParam) / 100.0f;
            ForEachAudioSession([&](DWORD pid, ISimpleAudioVolume* vol) {
                if (IsTargetSession(pid)) {
                    vol->SetMasterVolume(volume, nullptr);
                    if (volume > 0) {
                        vol->SetMute(FALSE, nullptr);
                    }
                }
            });
            return 0;
        }

        case kMsgSetMute: {
            BOOL mute = wParam != 0;
            ForEachAudioSession([&](DWORD pid, ISimpleAudioVolume* vol) {
                if (IsTargetSession(pid)) {
                    vol->SetMute(mute, nullptr);
                }
            });
            return 0;
        }

        case WM_DESTROY:
            SndVolUninit();
            break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool g_hostWndClassRegistered;

void EnsureHostWindow() {
    if (g_hostWnd) {
        return;
    }

    if (!g_hostWndClassRegistered) {
        WNDCLASS wc{};
        wc.lpfnWndProc = HostWndProc;
        wc.hInstance = GetModInstance();
        wc.lpszClassName = kHostWndClass;
        if (!RegisterClass(&wc)) {
            Wh_Log(L"RegisterClass failed: %u", GetLastError());
            return;
        }

        g_hostWndClassRegistered = true;
    }

    g_hostWnd = CreateWindowEx(0, kHostWndClass, nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, GetModInstance(),
                               nullptr);
    if (!g_hostWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return;
    }

    // ShellExperienceHost runs in an AppContainer, allow its messages.
    ChangeWindowMessageFilterEx(g_hostWnd, kMsgSetVolume, MSGFLT_ALLOW,
                                nullptr);
    ChangeWindowMessageFilterEx(g_hostWnd, kMsgSetMute, MSGFLT_ALLOW, nullptr);
}

void OnShowJumpView(void* taskGroup) {
    EnsureHostWindow();
    if (!g_hostWnd) {
        return;
    }

    g_targetSessionPids =
        FindAudioSessionPids(GetProcessIdsFromTaskGroup(taskGroup));

    JumpListVolumeInfo info{};
    info.hostWnd = (ULONGLONG)(ULONG_PTR)g_hostWnd;
    info.hasAudio = !g_targetSessionPids.empty();

    if (info.hasAudio) {
        float maxVolume = 0;
        bool allMuted = true;
        ForEachAudioSession([&](DWORD pid, ISimpleAudioVolume* vol) {
            if (!IsTargetSession(pid)) {
                return;
            }

            float volume = 0;
            if (SUCCEEDED(vol->GetMasterVolume(&volume)) &&
                volume > maxVolume) {
                maxVolume = volume;
            }

            BOOL muted = FALSE;
            if (FAILED(vol->GetMute(&muted)) || !muted) {
                allMuted = false;
            }
        });

        info.volume = (DWORD)(maxVolume * 100.0f + 0.5f);
        info.muted = allMuted;
    }

    Wh_Log(L"Jump list: sessions=%d, volume=%u, muted=%u",
           (int)g_targetSessionPids.size(), info.volume, info.muted);

    HWND shellWnd =
        FindWindowEx(HWND_MESSAGE, nullptr, kShellWndClass, nullptr);
    if (!shellWnd) {
        Wh_Log(L"ShellExperienceHost window not found");
        return;
    }

    // Sent synchronously so that the slider is in place before the jump list
    // is measured and shown.
    COPYDATASTRUCT cds{};
    cds.dwData = kCopyDataMagic;
    cds.cbData = sizeof(info);
    cds.lpData = &info;
    DWORD_PTR result = 0;
    if (!SendMessageTimeout(shellWnd, WM_COPYDATA, (WPARAM)g_hostWnd,
                            (LPARAM)&cds, SMTO_ABORTIFHUNG, 500, &result)) {
        Wh_Log(L"SendMessageTimeout failed: %u", GetLastError());
    }
}

using CTaskListWnd_ShowJumpView_t = HRESULT(WINAPI*)(void* pThis,
                                                     void* taskGroup,
                                                     void* taskItem,
                                                     bool param3);
CTaskListWnd_ShowJumpView_t CTaskListWnd_ShowJumpView_Original;
HRESULT WINAPI CTaskListWnd_ShowJumpView_Hook(void* pThis,
                                              void* taskGroup,
                                              void* taskItem,
                                              bool param3) {
    Wh_Log(L">");

    OnShowJumpView(taskGroup);

    return CTaskListWnd_ShowJumpView_Original(pThis, taskGroup, taskItem,
                                              param3);
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::ShowJumpView(struct ITaskGroup *,struct ITaskItem *,bool))"},
            &CTaskListWnd_ShowJumpView_Original,
            CTaskListWnd_ShowJumpView_Hook,
        },
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
    };

    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

////////////////////////////////////////////////////////////////////////////////
// ShellExperienceHost.exe: jump list UI.

constexpr WCHAR kGlyphVolume[] = L"";
constexpr WCHAR kGlyphMute[] = L"";

constexpr UINT_PTR kPollTimerId = 1;
constexpr UINT kPollIntervalMs = 50;
constexpr int kPollCount = 40;

HANDLE g_shellThread;
std::atomic<HWND> g_shellWnd;
std::atomic<HWND> g_volumeHostWnd;

// Accessed from the mod's window thread, and from the jump list UI thread
// while the former is blocked waiting for it.
struct {
    bool hasAudio = false;
    int volume = 0;
    bool muted = false;
    DWORD seq = 0;
} g_info;

// Only accessed from the mod's window thread.
HWND g_jumpListWnd;
int g_pollsLeft;
bool g_appliedForSeq;

enum class AttachMode {
    None,
    Header,  // Set as the header of the system items list.
    Panel,   // Inserted next to the list in its parent panel.
};

// Only accessed from the jump list UI thread. Holds strong XAML references,
// which are released on the UI thread by RemoveVolumeUi(), never by the
// automatic destructor at process exit.
struct UiState {
    AttachMode mode = AttachMode::None;
    winrt::weak_ref<Controls::ListViewBase> list;
    winrt::event_token layoutUpdatedToken;
    int layoutCount = 0;
    Controls::Grid root = nullptr;
    Controls::Slider slider = nullptr;
    Controls::Button muteButton = nullptr;
    Controls::TextBlock percentText = nullptr;
    Controls::FontIcon muteIcon = nullptr;
    winrt::event_token sliderValueChangedToken;
    winrt::event_token muteButtonClickToken;
    winrt::event_token wheelChangedToken;
    // Panel mode.
    Controls::Panel parentPanel = nullptr;
    Controls::StackPanel wrapper = nullptr;
    bool muted = false;
    bool updating = false;
    int wheelDeltaAccumulator = 0;
    DWORD appliedSeq = 0;
};
[[clang::no_destroy]] UiState g_ui;

// The thread which created the elements of g_ui, 0 if there are none. Only
// written on that thread, but read by the mod's window thread on unload.
std::atomic<DWORD> g_uiThreadId;

bool g_headerUnsupported;

void UpdateVolumeTexts() {
    int value = (int)std::lround(g_ui.slider.Value());
    g_ui.percentText.Text(winrt::to_hstring(value) + L"%");
    g_ui.muteIcon.Glyph(g_ui.muted || value == 0 ? kGlyphMute : kGlyphVolume);
}

void CreateVolumeUi() {
    Controls::FontIcon muteIcon;
    muteIcon.FontFamily(
        Media::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    muteIcon.FontSize(16);

    Controls::Button muteButton;
    muteButton.Content(muteIcon);
    muteButton.Width(32);
    muteButton.Height(32);
    muteButton.Padding(ThicknessHelper::FromUniformLength(0));
    muteButton.BorderThickness(ThicknessHelper::FromUniformLength(0));
    muteButton.Background(
        Media::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
    muteButton.VerticalAlignment(VerticalAlignment::Center);

    Controls::Slider slider;
    slider.Minimum(0);
    slider.Maximum(100);
    slider.StepFrequency(1);
    slider.MinWidth(150);
    slider.IsThumbToolTipEnabled(false);
    slider.VerticalAlignment(VerticalAlignment::Center);
    slider.HorizontalAlignment(HorizontalAlignment::Stretch);

    Controls::TextBlock percentText;
    percentText.FontSize(12);
    percentText.MinWidth(36);
    percentText.TextAlignment(TextAlignment::Right);
    percentText.VerticalAlignment(VerticalAlignment::Center);

    Controls::Grid root;
    root.Padding(ThicknessHelper::FromLengths(8, 4, 12, 4));
    root.ColumnSpacing(8);
    root.HorizontalAlignment(HorizontalAlignment::Stretch);
    // Lets the whole row receive mouse wheel events.
    root.Background(
        Media::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));

    Controls::ColumnDefinition column0;
    column0.Width(GridLengthHelper::Auto());
    Controls::ColumnDefinition column1;
    column1.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
    Controls::ColumnDefinition column2;
    column2.Width(GridLengthHelper::Auto());
    root.ColumnDefinitions().Append(column0);
    root.ColumnDefinitions().Append(column1);
    root.ColumnDefinitions().Append(column2);

    Controls::Grid::SetColumn(muteButton, 0);
    Controls::Grid::SetColumn(slider, 1);
    Controls::Grid::SetColumn(percentText, 2);
    root.Children().Append(muteButton);
    root.Children().Append(slider);
    root.Children().Append(percentText);

    g_ui.sliderValueChangedToken = slider.ValueChanged(
        [](auto&&,
           Controls::Primitives::RangeBaseValueChangedEventArgs const& e) {
            if (!g_ui.slider) {
                return;
            }

            int value = (int)std::lround(e.NewValue());
            if (!g_ui.updating) {
                if (value > 0) {
                    g_ui.muted = false;
                }

                if (!PostMessage(g_volumeHostWnd, kMsgSetVolume, value, 0)) {
                    Wh_Log(L"PostMessage failed: %u", GetLastError());
                }
            }

            UpdateVolumeTexts();
        });

    g_ui.muteButtonClickToken = muteButton.Click([](auto&&, auto&&) {
        if (!g_ui.slider) {
            return;
        }

        g_ui.muted = !g_ui.muted;
        if (!PostMessage(g_volumeHostWnd, kMsgSetMute, g_ui.muted, 0)) {
            Wh_Log(L"PostMessage failed: %u", GetLastError());
        }

        UpdateVolumeTexts();
    });

    g_ui.wheelChangedToken = root.PointerWheelChanged(
        [](auto&&, Input::PointerRoutedEventArgs const& e) {
            if (!g_ui.slider) {
                return;
            }

            int delta =
                e.GetCurrentPoint(nullptr).Properties().MouseWheelDelta();
            if (delta) {
                // Precision touchpads send many deltas smaller than
                // WHEEL_DELTA. Accumulate them, and step once per notch.
                if ((g_ui.wheelDeltaAccumulator > 0) != (delta > 0)) {
                    g_ui.wheelDeltaAccumulator = 0;
                }

                g_ui.wheelDeltaAccumulator += delta;
                int notches = g_ui.wheelDeltaAccumulator / WHEEL_DELTA;
                g_ui.wheelDeltaAccumulator -= notches * WHEEL_DELTA;

                if (notches) {
                    int wheelStep = g_settings.wheelStep;
                    double value = g_ui.slider.Value() + notches * wheelStep;
                    g_ui.slider.Value(value < 0     ? 0
                                      : value > 100 ? 100
                                                    : value);
                }
            }

            e.Handled(true);
        });

    g_ui.root = root;
    g_ui.slider = slider;
    g_ui.muteButton = muteButton;
    g_ui.percentText = percentText;
    g_ui.muteIcon = muteIcon;
    g_uiThreadId = GetCurrentThreadId();
}

void DetachVolumeUi() {
    auto list = g_ui.list.get();

    if (list && g_ui.layoutUpdatedToken) {
        list.LayoutUpdated(g_ui.layoutUpdatedToken);
    }

    if (g_ui.mode == AttachMode::Header) {
        if (list && list.Header() == g_ui.root) {
            list.Header(nullptr);
        }
    } else if (g_ui.mode == AttachMode::Panel && g_ui.parentPanel) {
        auto children = g_ui.parentPanel.Children();
        uint32_t index = 0;
        if (g_ui.wrapper) {
            // Put the list back in place of the wrapper.
            if (list && children.IndexOf(g_ui.wrapper, index)) {
                g_ui.wrapper.Children().Clear();
                children.RemoveAt(index);
                Controls::Grid::SetRow(list,
                                       Controls::Grid::GetRow(g_ui.wrapper));
                Controls::Grid::SetColumn(
                    list, Controls::Grid::GetColumn(g_ui.wrapper));
                Controls::Grid::SetRowSpan(
                    list, Controls::Grid::GetRowSpan(g_ui.wrapper));
                Controls::Grid::SetColumnSpan(
                    list, Controls::Grid::GetColumnSpan(g_ui.wrapper));
                children.InsertAt(index, list);
            }
        } else if (children.IndexOf(g_ui.root, index)) {
            children.RemoveAt(index);
        }
    }

    g_ui.mode = AttachMode::None;
    g_ui.list = nullptr;
    g_ui.layoutUpdatedToken = {};
    g_ui.layoutCount = 0;
    g_ui.parentPanel = nullptr;
    g_ui.wrapper = nullptr;
}

// Must be called on the thread that created the elements, see g_uiThreadId.
void RemoveVolumeUi() {
    DetachVolumeUi();

    // XAML can keep a detached element alive for a while (focus, pointer
    // capture during a drag), the handlers must not outlive the mod.
    if (g_ui.slider && g_ui.sliderValueChangedToken) {
        g_ui.slider.ValueChanged(g_ui.sliderValueChangedToken);
    }
    if (g_ui.muteButton && g_ui.muteButtonClickToken) {
        g_ui.muteButton.Click(g_ui.muteButtonClickToken);
    }
    if (g_ui.root && g_ui.wheelChangedToken) {
        g_ui.root.PointerWheelChanged(g_ui.wheelChangedToken);
    }

    g_ui.sliderValueChangedToken = {};
    g_ui.muteButtonClickToken = {};
    g_ui.wheelChangedToken = {};
    g_ui.root = nullptr;
    g_ui.slider = nullptr;
    g_ui.muteButton = nullptr;
    g_ui.percentText = nullptr;
    g_ui.muteIcon = nullptr;
    g_ui.wheelDeltaAccumulator = 0;
    g_ui.appliedSeq = 0;
    g_uiThreadId = 0;
}

bool AttachVolumeUiToPanel(Controls::ListViewBase list) {
    auto parentPanel =
        Media::VisualTreeHelper::GetParent(list).try_as<Controls::Panel>();
    if (!parentPanel) {
        return false;
    }

    auto children = parentPanel.Children();
    uint32_t index = 0;
    if (!children.IndexOf(list, index)) {
        return false;
    }

    auto stackPanel = parentPanel.try_as<Controls::StackPanel>();
    if (stackPanel &&
        stackPanel.Orientation() == Controls::Orientation::Vertical) {
        children.InsertAt(index, g_ui.root);
    } else {
        // Wrap the list and the slider with a panel which takes the place of
        // the list.
        Controls::StackPanel wrapper;
        Controls::Grid::SetRow(wrapper, Controls::Grid::GetRow(list));
        Controls::Grid::SetColumn(wrapper, Controls::Grid::GetColumn(list));
        Controls::Grid::SetRowSpan(wrapper, Controls::Grid::GetRowSpan(list));
        Controls::Grid::SetColumnSpan(wrapper,
                                      Controls::Grid::GetColumnSpan(list));

        children.RemoveAt(index);
        wrapper.Children().Append(g_ui.root);
        wrapper.Children().Append(list);
        children.InsertAt(index, wrapper);

        g_ui.wrapper = wrapper;
    }

    g_ui.parentPanel = parentPanel;
    g_ui.mode = AttachMode::Panel;
    return true;
}

void AttachVolumeUi(Controls::ListViewBase list) {
    g_ui.list = list;

    if (!g_headerUnsupported && !list.Header()) {
        list.Header(g_ui.root);
        g_ui.mode = AttachMode::Header;
        g_ui.layoutCount = 0;
        g_ui.layoutUpdatedToken =
            list.LayoutUpdated([](auto&&, auto&&) { g_ui.layoutCount++; });
        Wh_Log(L"Attached as list header");
        return;
    }

    if (AttachVolumeUiToPanel(list)) {
        Wh_Log(L"Attached to the parent panel (%s)",
               winrt::get_class_name(g_ui.parentPanel).c_str());
    } else {
        Wh_Log(L"Couldn't attach, parent is %s",
               winrt::get_class_name(Media::VisualTreeHelper::GetParent(list))
                   .c_str());
    }
}

Controls::ListViewBase FindSystemItemList(DependencyObject object, int depth) {
    if (!object || depth > 24) {
        return nullptr;
    }

    if (winrt::get_class_name(object) == L"JumpViewUI.SystemItemListView") {
        return object.try_as<Controls::ListViewBase>();
    }

    int count = Media::VisualTreeHelper::GetChildrenCount(object);
    for (int i = 0; i < count; i++) {
        if (auto found = FindSystemItemList(
                Media::VisualTreeHelper::GetChild(object, i), depth + 1)) {
            return found;
        }
    }

    return nullptr;
}

Controls::ListViewBase FindSystemItemListInCurrentWindow() {
    auto window = Window::Current();
    if (!window) {
        return nullptr;
    }

    if (auto found = FindSystemItemList(window.Content(), 0)) {
        return found;
    }

    for (auto popup : Media::VisualTreeHelper::GetOpenPopups(window)) {
        if (auto found = FindSystemItemList(popup.Child(), 0)) {
            return found;
        }
    }

    return nullptr;
}

// Returns true if the current thread hosts the jump list.
bool ApplyOnUiThread() {
    auto list = FindSystemItemListInCurrentWindow();
    if (!list) {
        return false;
    }

    // The elements can't be touched from another thread, and can't be released
    // from it either. Leave them as they are, rather than crash.
    DWORD uiThreadId = g_uiThreadId;
    if (uiThreadId && uiThreadId != GetCurrentThreadId()) {
        Wh_Log(L"Jump list is on thread %u, the volume UI is on thread %u",
               GetCurrentThreadId(), uiThreadId);
        return false;
    }

    // The header is not shown by this list's template, use the other way.
    if (g_ui.mode == AttachMode::Header && g_ui.layoutCount >= 3 &&
        !Media::VisualTreeHelper::GetParent(g_ui.root)) {
        Wh_Log(L"List header isn't displayed, switching to panel mode");
        g_headerUnsupported = true;
        DetachVolumeUi();
    }

    bool attached = g_ui.mode != AttachMode::None && g_ui.list.get() == list &&
                    (g_ui.mode != AttachMode::Header ||
                     list.Header() == g_ui.root);
    if (!attached) {
        if (g_ui.root) {
            DetachVolumeUi();
        } else {
            CreateVolumeUi();
        }

        AttachVolumeUi(list);
        g_ui.appliedSeq = 0;
    }

    // Applied once per jump list, the user might be dragging the slider.
    if (g_ui.appliedSeq != g_info.seq) {
        g_ui.appliedSeq = g_info.seq;
        g_ui.muted = g_info.muted;
        g_ui.updating = true;
        g_ui.slider.Value(g_info.volume);
        g_ui.updating = false;
        UpdateVolumeTexts();
        g_ui.root.Visibility(g_info.hasAudio ? Visibility::Visible
                                              : Visibility::Collapsed);
    }

    return true;
}

bool ApplyOnWindowThread(HWND hWnd) {
    bool applied = false;
    RunFromWindowThread(
        hWnd,
        [](void* param) {
            try {
                *(bool*)param = ApplyOnUiThread();
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Error %08X: %s", (UINT)e.code(), e.message().c_str());
            } catch (...) {
                Wh_Log(L"Unknown error");
            }
        },
        &applied);
    return applied;
}

std::vector<HWND> GetCoreWindows() {
    std::vector<HWND> windows;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& windows = *(std::vector<HWND>*)lParam;

            DWORD pid = 0;
            WCHAR className[64];
            DWORD threadId = GetWindowThreadProcessId(hWnd, &pid);
            if (!threadId || pid != GetCurrentProcessId() ||
                !GetClassName(hWnd, className, ARRAYSIZE(className)) ||
                _wcsicmp(className, L"Windows.UI.Core.CoreWindow") != 0) {
                return TRUE;
            }

            // One window per UI thread is enough.
            for (HWND other : windows) {
                if (GetWindowThreadProcessId(other, nullptr) == threadId) {
                    return TRUE;
                }
            }

            windows.push_back(hWnd);
            return TRUE;
        },
        (LPARAM)&windows);

    return windows;
}

bool ApplyToJumpList() {
    if (g_jumpListWnd && IsWindow(g_jumpListWnd) &&
        ApplyOnWindowThread(g_jumpListWnd)) {
        return true;
    }

    g_jumpListWnd = nullptr;

    for (HWND hWnd : GetCoreWindows()) {
        if (ApplyOnWindowThread(hWnd)) {
            g_jumpListWnd = hWnd;
            return true;
        }
    }

    return false;
}

// For troubleshooting, in case the jump list wasn't found.
void LogCoreWindows() {
    for (HWND hWnd : GetCoreWindows()) {
        WCHAR title[128] = L"";
        GetWindowText(hWnd, title, ARRAYSIZE(title));
        Wh_Log(L"CoreWindow %p: %s", hWnd, title);

        RunFromWindowThread(
            hWnd,
            [](void*) {
                try {
                    auto window = Window::Current();
                    auto content = window ? window.Content() : nullptr;
                    Wh_Log(L"  Content: %s",
                           content ? winrt::get_class_name(content).c_str()
                                   : L"(none)");
                } catch (...) {
                    Wh_Log(L"  No XAML window");
                }
            },
            nullptr);
    }
}

LRESULT CALLBACK ShellWndProc(HWND hWnd,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam) {
    switch (uMsg) {
        case WM_COPYDATA: {
            auto* cds = (const COPYDATASTRUCT*)lParam;
            if (cds->dwData != kCopyDataMagic ||
                cds->cbData != sizeof(JumpListVolumeInfo)) {
                return FALSE;
            }

            auto* info = (const JumpListVolumeInfo*)cds->lpData;
            g_volumeHostWnd = (HWND)(ULONG_PTR)info->hostWnd;
            g_info.hasAudio = info->hasAudio;
            g_info.volume = info->volume > 100 ? 100 : info->volume;
            g_info.muted = info->muted;
            g_info.seq++;

            // The jump list might be recreated when shown, keep applying for
            // a while.
            g_appliedForSeq = ApplyToJumpList();
            g_pollsLeft = kPollCount;
            SetTimer(hWnd, kPollTimerId, kPollIntervalMs, nullptr);
            return TRUE;
        }

        case WM_TIMER:
            if (wParam == kPollTimerId) {
                if (ApplyToJumpList()) {
                    g_appliedForSeq = true;
                }

                if (--g_pollsLeft <= 0) {
                    KillTimer(hWnd, kPollTimerId);
                    if (!g_appliedForSeq) {
                        Wh_Log(L"Jump list not found");
                        LogCoreWindows();
                    }
                }
            }
            return 0;

        case WM_CLOSE:
            KillTimer(hWnd, kPollTimerId);

            // Clean up on the thread which owns the elements. It's not
            // necessarily the thread of the last known jump list window, which
            // is reset whenever the jump list isn't found.
            if (DWORD uiThreadId = g_uiThreadId) {
                for (HWND coreWnd : GetCoreWindows()) {
                    if (GetWindowThreadProcessId(coreWnd, nullptr) ==
                        uiThreadId) {
                        RunFromWindowThread(
                            coreWnd,
                            [](void*) {
                                try {
                                    RemoveVolumeUi();
                                } catch (...) {
                                    Wh_Log(L"Removing the volume UI failed");
                                }
                            },
                            nullptr);
                        break;
                    }
                }
            }

            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// The parameter is an event which is signaled once the window is created, or
// once it's known that it can't be.
DWORD WINAPI ShellThreadProc(LPVOID readyEvent) {
    WNDCLASS wc{};
    wc.lpfnWndProc = ShellWndProc;
    wc.hInstance = GetModInstance();
    wc.lpszClassName = kShellWndClass;
    if (!RegisterClass(&wc)) {
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
        SetEvent((HANDLE)readyEvent);
        return 1;
    }

    HWND hWnd = CreateWindowEx(0, kShellWndClass, nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        UnregisterClass(kShellWndClass, wc.hInstance);
        SetEvent((HANDLE)readyEvent);
        return 1;
    }

    g_shellWnd = hWnd;
    SetEvent((HANDLE)readyEvent);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_shellWnd = nullptr;
    UnregisterClass(kShellWndClass, wc.hInstance);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    WCHAR path[MAX_PATH];
    if (GetModuleFileName(nullptr, path, ARRAYSIZE(path))) {
        PCWSTR name = wcsrchr(path, L'\\');
        name = name ? name + 1 : path;
        g_isShellExperienceHost =
            _wcsicmp(name, L"ShellExperienceHost.exe") == 0;
    }

    if (g_isShellExperienceHost) {
        HANDLE readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (!readyEvent) {
            return FALSE;
        }

        g_shellThread =
            CreateThread(nullptr, 0, ShellThreadProc, readyEvent, 0, nullptr);
        if (g_shellThread) {
            // Signaled by the thread after creating its window, or on failure.
            // This guarantees that Wh_ModUninit can always reach the window.
            WaitForSingleObject(readyEvent, INFINITE);
        }

        CloseHandle(readyEvent);

        if (g_shellThread && !g_shellWnd) {
            // The thread failed to create its window and is exiting.
            WaitForSingleObject(g_shellThread, INFINITE);
            CloseHandle(g_shellThread);
            g_shellThread = nullptr;
        }

        return g_shellThread != nullptr;
    }

    return HookTaskbarDllSymbols();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_isShellExperienceHost) {
        if (HWND shellWnd = g_shellWnd) {
            PostMessage(shellWnd, WM_CLOSE, 0, 0);
        }

        if (g_shellThread) {
            // No timeout: the mod's code must not be unloaded while the thread
            // is still running.
            WaitForSingleObject(g_shellThread, INFINITE);
            CloseHandle(g_shellThread);
            g_shellThread = nullptr;
        }

        return;
    }

    if (g_hostWnd) {
        // Destroys the window from its own thread.
        SendMessage(g_hostWnd, WM_CLOSE, 0, 0);
        g_hostWnd = nullptr;
    }

    if (g_hostWndClassRegistered) {
        UnregisterClass(kHostWndClass, GetModInstance());
        g_hostWndClassRegistered = false;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
