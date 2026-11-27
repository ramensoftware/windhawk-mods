// ==WindhawkMod==
// @id              adaptive-microphone-icon-visibility
// @name            Adaptive Microphone Icon Visibility
// @description     Hides the system tray microphone icon if only whitelisted applications have an active capture stream.
// @version         2.0
// @author          Ikkecode
// @github          https://github.com/ikkecode
// @twitter         https://twitter.com/ikkenand
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Adaptive Microphone Icon Visibility

This mod hides the system tray microphone icon if only whitelisted applications
(or no applications) are using the microphone. If an unlisted application starts
using it, the icon is shown right away.

Whitelist apps by executable name, e.g. `obs64.exe`.

Only Windows 11 is supported.

## Credits

Icon manipulation logic in this mod is based on the
[Taskbar tray system icon tweaks] mod by **m417z**.

[Taskbar tray system icon tweaks]:
https://windhawk.net/mods/taskbar-tray-system-icon-tweaks

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- WhitelistedApps: [""]
  $name: Whitelisted Apps
  $description: >-
    List of allowed apps, by executable name (e.g., obs64.exe). The app's
    name is shown in the mod log when it starts capturing audio.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

#include <audiopolicy.h>
#include <mmdeviceapi.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

std::atomic<bool> g_hideMicrophoneIcon{true};

// --- Part 1: Icon Hide Logic ---
// Based on "Taskbar tray system icon tweaks" by m417z
// https://windhawk.net/mods/taskbar-tray-system-icon-tweaks

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_unloading;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

[[clang::no_destroy]] std::optional<
    std::list<FrameworkElementLoadedEventRevoker>> g_autoRevokerList{
    std::in_place};

winrt::weak_ref<Controls::TextBlock> g_mainStackInnerTextBlock;
int64_t g_mainStackTextChangedToken;

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

FrameworkElement EnumParentElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (enumCallback(parent)) {
            return parent;
        }
    }
}

FrameworkElement GetParentElementByName(FrameworkElement element, PCWSTR name) {
    return EnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!GetParentElementByName(element, name);
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// https://stackoverflow.com/a/3382894
std::wstring StringToHex(std::wstring_view input) {
    static const WCHAR kHexDigits[] = L"0123456789ABCDEF";

    std::wstring output;
    output.reserve(input.length() * 5);
    for (WCHAR c : input) {
        output.push_back(kHexDigits[(c >> 12) & 15]);
        output.push_back(kHexDigits[(c >> 8) & 15]);
        output.push_back(kHexDigits[(c >> 4) & 15]);
        output.push_back(kHexDigits[c & 15]);
        output.push_back(L' ');
    }

    if (!output.empty() && output.back() == L' ') {
        output.resize(output.length() - 1);
    }

    return output;
}

enum class SystemTrayIconIdent {
    kUnknown,
    kNone,
    kMicrophone,
};

SystemTrayIconIdent IdentifySystemTrayIconFromText(std::wstring_view text) {
    switch (text.length()) {
        case 0:
            return SystemTrayIconIdent::kNone;

        case 1:
            break;

        default:
            return SystemTrayIconIdent::kUnknown;
    }

    switch (text[0]) {
        case L'\uE361':  // Private Use
        case L'\uE720':  // Microphone
        case L'\uEC71':  // MicOn
            return SystemTrayIconIdent::kMicrophone;
    }

    return SystemTrayIconIdent::kUnknown;
}

void ApplyMainStackIconViewStyle(FrameworkElement notifyIconViewElement) {
    FrameworkElement systemTrayTextIconContent = nullptr;

    FrameworkElement child = notifyIconViewElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        systemTrayTextIconContent = child;
    } else {
        Wh_Log(L"Failed to get SystemTray.TextIconContent");
        return;
    }

    Controls::TextBlock innerTextBlock = nullptr;

    child = systemTrayTextIconContent;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"Base")) &&
        (child = FindChildByName(child, L"InnerTextBlock"))) {
        innerTextBlock = child.as<Controls::TextBlock>();
    } else {
        Wh_Log(L"Failed to get InnerTextBlock");
        return;
    }

    auto shouldHide = [](Controls::TextBlock innerTextBlock) {
        auto text = innerTextBlock.Text();
        auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

        bool hide = false;
        if (!g_unloading) {
            switch (systemTrayIconIdent) {
                case SystemTrayIconIdent::kMicrophone:
                    hide = g_hideMicrophoneIcon.load();
                    break;

                case SystemTrayIconIdent::kNone:
                    // Happens when the icon is about to disappear.
                    break;

                default:
                    break;
            }
        }

        Wh_Log(L"Main stack icon %d (%s), hide=%d", (int)systemTrayIconIdent,
               StringToHex(text).c_str(), hide);

        return hide;
    };

    bool hide = shouldHide(innerTextBlock);

    notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                          : Visibility::Visible);

    if (!g_unloading && !g_mainStackInnerTextBlock.get()) {
        auto notifyIconViewElementWeakRef =
            winrt::make_weak(notifyIconViewElement);
        g_mainStackInnerTextBlock = innerTextBlock;
        g_mainStackTextChangedToken =
            innerTextBlock.RegisterPropertyChangedCallback(
                Controls::TextBlock::TextProperty(),
                [notifyIconViewElementWeakRef, shouldHide](
                    DependencyObject sender, DependencyProperty property) {
                    auto innerTextBlock = sender.try_as<Controls::TextBlock>();
                    if (!innerTextBlock) {
                        return;
                    }

                    auto notifyIconViewElement =
                        notifyIconViewElementWeakRef.get();
                    if (!notifyIconViewElement) {
                        return;
                    }

                    bool hide = shouldHide(innerTextBlock);

                    Wh_Log(L"Main stack icon, hide=%d", hide);

                    notifyIconViewElement.Visibility(
                        hide ? Visibility::Collapsed : Visibility::Visible);
                });
    }
}

bool ApplyMainStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyMainStackIconViewStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyStyle(XamlRoot xamlRoot) {
    FrameworkElement systemTrayFrameGrid = nullptr;

    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        systemTrayFrameGrid = child;
    }

    if (!systemTrayFrameGrid) {
        return false;
    }

    bool somethingSucceeded = false;

    FrameworkElement mainStack =
        FindChildByName(systemTrayFrameGrid, L"MainStack");
    if (mainStack) {
        somethingSucceeded |= ApplyMainStackStyle(mainStack);
    }

    return somethingSucceeded;
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = IconView_IconView_Original(pThis);

    // A new icon view has nothing to restore, and the revoker list is released
    // while unloading.
    if (g_unloading) {
        return ret;
    }

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_autoRevokerList->emplace_back();
    auto autoRevokerIt = g_autoRevokerList->end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_autoRevokerList->erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            auto className = winrt::get_class_name(iconView);
            Wh_Log(L"className: %s", className.c_str());

            if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon") {
                    if (IsChildOfElementByName(iconView, L"MainStack")) {
                        ApplyMainStackIconViewStyle(iconView);
                    }
                }
            }
        });

    return ret;
}

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
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

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

void ApplySettings() {
    struct ApplySettingsParam {
        HWND hTaskbarWnd;
    };

    Wh_Log(L"Applying settings");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    ApplySettingsParam param{
        .hTaskbarWnd = hTaskbarWnd,
    };

    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) {
            ApplySettingsParam& param = *(ApplySettingsParam*)pParam;

            if (g_autoRevokerList) {
                g_autoRevokerList->clear();
            }

            if (auto mainStackInnerTextBlock =
                    g_mainStackInnerTextBlock.get()) {
                mainStackInnerTextBlock.UnregisterPropertyChangedCallback(
                    Controls::TextBlock::TextProperty(),
                    g_mainStackTextChangedToken);
                g_mainStackInnerTextBlock = nullptr;
                g_mainStackTextChangedToken = 0;
            }

            auto xamlRoot = GetTaskbarXamlRoot(param.hTaskbarWnd);
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
            } else if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyle failed");
            }

            if (g_unloading) {
                g_autoRevokerList.reset();
            }
        },
        &param);
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // hook Taskbar.View.dll at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
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
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// --- Part 2: Microphone Monitoring ---

std::mutex g_whitelistMutex;
std::vector<std::wstring> g_whitelist;
HANDLE g_hShutdownEvent = NULL;
HANDLE g_hChangeEvent = NULL;
HANDLE g_hMonitorThread = NULL;

constexpr DWORD kFallbackRescanMs = 30000;

#ifndef AUDCLNT_S_NO_SINGLE_PROCESS
#define AUDCLNT_S_NO_SINGLE_PROCESS ((HRESULT)0x0889000D)
#endif

void SetHideMicrophoneIcon(bool hide) {
    if (g_unloading) {
        return;
    }

    if (g_hideMicrophoneIcon.exchange(hide) != hide) {
        Wh_Log(L"Microphone icon visibility updated: hide=%d", hide);
        ApplySettings();
    }
}

std::wstring StripExeSuffix(std::wstring name) {
    if (name.size() > 4 &&
        _wcsicmp(name.c_str() + name.size() - 4, L".exe") == 0) {
        name.resize(name.size() - 4);
    }
    return name;
}

bool IsAppWhitelisted(const std::wstring& exeName) {
    std::wstring name = StripExeSuffix(exeName);

    std::lock_guard<std::mutex> lock(g_whitelistMutex);
    for (const auto& whitelisted : g_whitelist) {
        if (_wcsicmp(name.c_str(), whitelisted.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

std::wstring GetProcessExeName(DWORD pid) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess) {
        return L"";
    }

    WCHAR path[MAX_PATH * 2];
    DWORD len = ARRAYSIZE(path);
    std::wstring name;
    if (QueryFullProcessImageNameW(hProcess, 0, path, &len)) {
        const WCHAR* slash = wcsrchr(path, L'\\');
        name = slash ? slash + 1 : path;
    }

    CloseHandle(hProcess);
    return name;
}

// Callbacks must not (un)register notifications, so they only wake the
// monitor thread.
struct AudioChangeListener : winrt::implements<AudioChangeListener,
                                               IMMNotificationClient,
                                               IAudioSessionNotification,
                                               IAudioSessionEvents> {
    // Changes that can affect which apps are capturing.
    STDMETHODIMP OnDeviceAdded(LPCWSTR) { return Signal(); }
    STDMETHODIMP OnDeviceRemoved(LPCWSTR) { return Signal(); }
    STDMETHODIMP OnDeviceStateChanged(LPCWSTR, DWORD) { return Signal(); }
    STDMETHODIMP OnSessionCreated(IAudioSessionControl*) { return Signal(); }
    STDMETHODIMP OnStateChanged(AudioSessionState) { return Signal(); }
    STDMETHODIMP OnSessionDisconnected(AudioSessionDisconnectReason) {
        return Signal();
    }

    // Everything else is ignored.
    STDMETHODIMP OnDefaultDeviceChanged(EDataFlow, ERole, LPCWSTR) {
        return S_OK;
    }
    STDMETHODIMP OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) {
        return S_OK;
    }
    STDMETHODIMP OnDisplayNameChanged(LPCWSTR, LPCGUID) { return S_OK; }
    STDMETHODIMP OnIconPathChanged(LPCWSTR, LPCGUID) { return S_OK; }
    STDMETHODIMP OnSimpleVolumeChanged(float, BOOL, LPCGUID) { return S_OK; }
    STDMETHODIMP OnChannelVolumeChanged(DWORD, float[], DWORD, LPCGUID) {
        return S_OK;
    }
    STDMETHODIMP OnGroupingParamChanged(LPCGUID, LPCGUID) { return S_OK; }

   private:
    static HRESULT Signal() {
        SetEvent(g_hChangeEvent);
        return S_OK;
    }
};

class CaptureSessionWatcher {
   public:
    bool Init() {
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                    CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                    m_enumerator.put_void()))) {
            Wh_Log(L"Failed to create IMMDeviceEnumerator");
            return false;
        }

        m_listener = winrt::make_self<AudioChangeListener>();

        if (FAILED(m_enumerator->RegisterEndpointNotificationCallback(
                m_listener.get()))) {
            Wh_Log(L"Failed to register device notifications");
            m_enumerator = nullptr;  // Nothing to unregister.
            return false;
        }

        return true;
    }

    ~CaptureSessionWatcher() {
        ClearRegistrations();
        if (m_enumerator) {
            m_enumerator->UnregisterEndpointNotificationCallback(
                m_listener.get());
        }
    }

    // Register before reading state, so no change in between is missed.
    // Returns false if the capture sessions couldn't be read.
    bool Rescan(std::vector<std::wstring>& activeApps) {
        ClearRegistrations();

        winrt::com_ptr<IMMDeviceCollection> devices;
        if (FAILED(m_enumerator->EnumAudioEndpoints(
                eCapture, DEVICE_STATE_ACTIVE, devices.put()))) {
            return false;
        }

        UINT deviceCount = 0;
        devices->GetCount(&deviceCount);

        for (UINT i = 0; i < deviceCount; i++) {
            winrt::com_ptr<IMMDevice> device;
            if (FAILED(devices->Item(i, device.put()))) {
                return false;
            }

            winrt::com_ptr<IAudioSessionManager2> manager;
            if (FAILED(device->Activate(__uuidof(IAudioSessionManager2),
                                        CLSCTX_ALL, nullptr,
                                        manager.put_void()))) {
                return false;
            }

            if (SUCCEEDED(
                    manager->RegisterSessionNotification(m_listener.get()))) {
                m_managers.push_back(manager);
            }

            // Also required for session-created notifications to start.
            winrt::com_ptr<IAudioSessionEnumerator> sessions;
            if (FAILED(manager->GetSessionEnumerator(sessions.put()))) {
                return false;
            }

            int sessionCount = 0;
            sessions->GetCount(&sessionCount);

            for (int j = 0; j < sessionCount; j++) {
                winrt::com_ptr<IAudioSessionControl> control;
                if (FAILED(sessions->GetSession(j, control.put()))) {
                    return false;
                }

                if (SUCCEEDED(control->RegisterAudioSessionNotification(
                        m_listener.get()))) {
                    m_sessions.push_back(control);
                }

                AudioSessionState state;
                if (FAILED(control->GetState(&state))) {
                    return false;
                }
                if (state != AudioSessionStateActive) {
                    continue;
                }

                auto control2 = control.try_as<IAudioSessionControl2>();
                if (control2 && control2->IsSystemSoundsSession() == S_OK) {
                    continue;
                }

                activeApps.push_back(GetSessionAppName(control2));
            }
        }

        return true;
    }

   private:
    // A session that can't be attributed to a single process is reported as
    // "unknown", which is never whitelisted.
    static std::wstring GetSessionAppName(
        const winrt::com_ptr<IAudioSessionControl2>& control2) {
        if (!control2) {
            return L"unknown";
        }

        DWORD pid = 0;
        HRESULT hr = control2->GetProcessId(&pid);
        if (FAILED(hr) || hr == AUDCLNT_S_NO_SINGLE_PROCESS || pid == 0) {
            return L"unknown";
        }

        std::wstring name = GetProcessExeName(pid);
        if (name.empty()) {
            name = L"pid:" + std::to_wstring(pid);
        }
        return name;
    }

    void ClearRegistrations() {
        for (auto& session : m_sessions) {
            session->UnregisterAudioSessionNotification(m_listener.get());
        }
        m_sessions.clear();

        for (auto& manager : m_managers) {
            manager->UnregisterSessionNotification(m_listener.get());
        }
        m_managers.clear();
    }

    winrt::com_ptr<IMMDeviceEnumerator> m_enumerator;
    winrt::com_ptr<AudioChangeListener> m_listener;
    std::vector<winrt::com_ptr<IAudioSessionManager2>> m_managers;
    std::vector<winrt::com_ptr<IAudioSessionControl>> m_sessions;
};

void UpdateMicrophoneIcon(std::vector<std::wstring> activeApps,
                          std::vector<std::wstring>& lastLogged) {
    std::sort(activeApps.begin(), activeApps.end());
    activeApps.erase(std::unique(activeApps.begin(), activeApps.end()),
                     activeApps.end());

    if (activeApps != lastLogged) {
        lastLogged = activeApps;
        Wh_Log(L"Active capture apps: %zu", activeApps.size());
        for (const auto& app : activeApps) {
            Wh_Log(L"  %s (whitelisted=%d)", app.c_str(),
                   IsAppWhitelisted(app));
        }
    }

    bool allWhitelisted =
        std::all_of(activeApps.begin(), activeApps.end(), IsAppWhitelisted);
    SetHideMicrophoneIcon(allWhitelisted);
}

DWORD WINAPI MonitorThreadProc(LPVOID lpParam) {
    HRESULT hrInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    {
        CaptureSessionWatcher watcher;
        if (!watcher.Init()) {
            Wh_Log(L"Microphone monitoring failed, showing the icon");
            SetHideMicrophoneIcon(false);
        } else {
            std::vector<std::wstring> lastLogged;
            HANDLE waitHandles[] = {g_hShutdownEvent, g_hChangeEvent};

            while (true) {
                std::vector<std::wstring> activeApps;
                if (watcher.Rescan(activeApps)) {
                    UpdateMicrophoneIcon(std::move(activeApps), lastLogged);
                } else {
                    Wh_Log(
                        L"Failed to read capture sessions, showing the icon");
                    SetHideMicrophoneIcon(false);
                }

                DWORD result =
                    WaitForMultipleObjects(ARRAYSIZE(waitHandles), waitHandles,
                                           FALSE, kFallbackRescanMs);
                bool changed = result == WAIT_OBJECT_0 + 1;
                bool timedOut = result == WAIT_TIMEOUT;
                if (!changed && !timedOut) {
                    break;  // Shutdown, or the wait failed.
                }
            }
        }
    }

    if (SUCCEEDED(hrInit)) {
        CoUninitialize();
    }

    return 0;
}

void StartMonitor() {
    g_hShutdownEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_hChangeEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (g_hShutdownEvent && g_hChangeEvent) {
        g_hMonitorThread =
            CreateThread(NULL, 0, MonitorThreadProc, NULL, 0, NULL);
    }

    if (!g_hMonitorThread) {
        Wh_Log(L"Failed to start microphone monitoring, showing the icon");
        SetHideMicrophoneIcon(false);
    }
}

void StopMonitor() {
    if (g_hShutdownEvent) {
        SetEvent(g_hShutdownEvent);
    }
    if (g_hMonitorThread) {
        WaitForSingleObject(g_hMonitorThread, INFINITE);
        CloseHandle(g_hMonitorThread);
        g_hMonitorThread = NULL;
    }
    if (g_hChangeEvent) {
        CloseHandle(g_hChangeEvent);
        g_hChangeEvent = NULL;
    }
    if (g_hShutdownEvent) {
        CloseHandle(g_hShutdownEvent);
        g_hShutdownEvent = NULL;
    }
}

void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_whitelistMutex);
    g_whitelist.clear();

    using WindhawkUtils::StringSetting;

    for (int i = 0;; i++) {
        auto appNameSetting = StringSetting::make(L"WhitelistedApps[%d]", i);

        if (wcslen(appNameSetting) == 0) {
            break;
        }

        g_whitelist.push_back(StripExeSuffix(appNameSetting.get()));
    }

    Wh_Log(L"Loaded %zu whitelisted apps total", g_whitelist.size());
}

BOOL Wh_ModInit() {
    Wh_Log(L"> Initializing mod");

    LoadSettings();

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"HookTaskbarDllSymbols failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"> After Init");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings();
    StartMonitor();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"> Before Uninit");

    g_unloading = true;

    StopMonitor();
    ApplySettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"> Whitelist changed, reloading...");

    LoadSettings();

    if (g_hChangeEvent) {
        SetEvent(g_hChangeEvent);
    }
}
