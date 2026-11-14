// ==WindhawkMod==
// @id              adaptive-microphone-icon-visibility
// @name            Adaptive Microphone Icon Visibility
// @description     Hides the system tray microphone icon if only whitelisted applications have active access.
// @version         1.0
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
(or no applications) have active access. If an unlisted application begins using
the microphone, the icon is immediately shown to alert the user.

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
    List of allowed apps. Use executable names for desktop apps
    (e.g., obs64.exe) or the package name before the first underscore
    for Store apps (e.g., Microsoft.WindowsSoundRecorder).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
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

std::list<FrameworkElementLoadedEventRevoker> g_autoRevokerList;

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
                [notifyIconViewElementWeakRef, &shouldHide](
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

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_autoRevokerList.emplace_back();
    auto autoRevokerIt = g_autoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_autoRevokerList.erase(autoRevokerIt);

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

            g_autoRevokerList.clear();

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
                return;
            }

            if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyle failed");
            }
        },
        &param);
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
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

// Returns the module that hosts winrt::SystemTray::* in the current build.
// Order matters: SystemTray.dll is the new home (Win11 Insider 26200+);
// Taskbar.View.dll is kept as fallbacks so this still works on older builds.
HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // First known module version without SystemTray is Taskbar.View.dll
            // 2604.8002.200.6000.
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
HANDLE g_hMonitorThread = NULL;

void SetHideMicrophoneIcon(bool hide) {
    if (g_hideMicrophoneIcon.exchange(hide) != hide) {
        Wh_Log(L"Microphone icon visibility updated: hide=%d", hide);
        ApplySettings();
    }
}

ULONGLONG GetQWORDValue(HKEY hKey, const wchar_t* valueName) {
    ULONGLONG value = 0;
    DWORD cbData = sizeof(value);
    DWORD dwType = 0;
    LSTATUS status = RegQueryValueExW(hKey, valueName, NULL, &dwType,
                                      (LPBYTE)&value, &cbData);
    if (status == ERROR_SUCCESS && dwType == REG_QWORD) {
        return value;
    }
    return 0;
}

// Extract app name from path string (handles registry '#' format for
// non-packaged apps)
std::wstring GetAppNameFromPath(const std::wstring& fullPath) {
    size_t lastHash = fullPath.find_last_of(L'#');
    if (lastHash != std::wstring::npos) {
        return fullPath.substr(lastHash + 1);
    }
    size_t lastSlash = fullPath.find_last_of(L'\\');
    if (lastSlash != std::wstring::npos) {
        return fullPath.substr(lastSlash + 1);
    }
    return fullPath;
}

std::wstring GetCleanPackagedName(const std::wstring& packageName) {
    size_t firstUnderscore = packageName.find(L'_');
    if (firstUnderscore != std::wstring::npos) {
        return packageName.substr(0, firstUnderscore);
    }
    return packageName;
}

bool IsAppWhitelisted(const std::wstring& appName) {
    std::lock_guard<std::mutex> lock(g_whitelistMutex);
    for (const auto& whitelisted : g_whitelist) {
        if (_wcsicmp(appName.c_str(), whitelisted.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

void ScanMicrophoneKeys(const wchar_t* subKeyPath,
                        bool isPackaged,
                        std::vector<std::wstring>& activeApps) {
    HKEY hParentKey = NULL;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, subKeyPath, 0, KEY_READ,
                      &hParentKey) != ERROR_SUCCESS) {
        return;
    }

    wchar_t szSubKeyName[MAX_PATH];
    DWORD cbName = MAX_PATH;
    DWORD dwIndex = 0;

    while (RegEnumKeyExW(hParentKey, dwIndex, szSubKeyName, &cbName, NULL, NULL,
                         NULL, NULL) == ERROR_SUCCESS) {
        if (isPackaged && _wcsicmp(szSubKeyName, L"NonPackaged") == 0) {
            cbName = MAX_PATH;
            dwIndex++;
            continue;
        }

        HKEY hSubKey = NULL;
        if (RegOpenKeyExW(hParentKey, szSubKeyName, 0, KEY_READ, &hSubKey) ==
            ERROR_SUCCESS) {
            ULONGLONG stop = GetQWORDValue(hSubKey, L"LastUsedTimeStop");
            ULONGLONG start = GetQWORDValue(hSubKey, L"LastUsedTimeStart");

            if (stop == 0 && start != 0) {
                if (isPackaged) {
                    activeApps.push_back(GetCleanPackagedName(szSubKeyName));
                } else {
                    activeApps.push_back(GetAppNameFromPath(szSubKeyName));
                }
            }
            RegCloseKey(hSubKey);
        }

        cbName = MAX_PATH;
        dwIndex++;
    }

    RegCloseKey(hParentKey);
}

void CheckMicrophoneStatus() {
    const wchar_t* kPackagedPath =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\"
        L"CapabilityAccessManager\\ConsentStore\\microphone";

    const wchar_t* kNonPackagedPath =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\"
        L"CapabilityAccessManager\\ConsentStore\\microphone\\NonPackaged";

    std::vector<std::wstring> activeApps;
    ScanMicrophoneKeys(kPackagedPath, true, activeApps);
    ScanMicrophoneKeys(kNonPackagedPath, false, activeApps);

    if (!activeApps.empty()) {
        std::sort(activeApps.begin(), activeApps.end());
        auto last = std::unique(activeApps.begin(), activeApps.end());
        activeApps.erase(last, activeApps.end());
    }

    bool unauthorizedDetected = false;
    for (const auto& app : activeApps) {
        if (!IsAppWhitelisted(app)) {
            unauthorizedDetected = true;
            break;
        }
    }

    SetHideMicrophoneIcon(!unauthorizedDetected);
}

DWORD WINAPI RegMonitorThreadProc(LPVOID lpParam) {
    HKEY hKey = NULL;
    const wchar_t* kPath =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\"
        L"CapabilityAccessManager\\ConsentStore\\microphone";

    if (RegOpenKeyExW(HKEY_CURRENT_USER, kPath, 0, KEY_NOTIFY, &hKey) !=
        ERROR_SUCCESS) {
        Wh_Log(L"Failed to open registry key for notification");
        return 0;
    }

    HANDLE hRegEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!hRegEvent) {
        RegCloseKey(hKey);
        return 0;
    }

    HANDLE hWaitHandles[2] = {g_hShutdownEvent, hRegEvent};
    bool bRunning = true;

    while (bRunning) {
        // Watch subtree recursively to monitor NonPackaged application subkeys
        LONG status = RegNotifyChangeKeyValue(
            hKey, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,
            hRegEvent, TRUE);

        if (status != ERROR_SUCCESS) {
            Wh_Log(L"RegNotifyChangeKeyValue failed: %d", status);
            break;
        }

        CheckMicrophoneStatus();

        DWORD dwWait = WaitForMultipleObjects(2, hWaitHandles, FALSE, INFINITE);
        if (dwWait == WAIT_OBJECT_0) {
            bRunning = false;
        } else if (dwWait != WAIT_OBJECT_0 + 1) {
            break;  // Break loop on wait failure or unexpected event
        }
    }

    CloseHandle(hRegEvent);
    RegCloseKey(hKey);
    return 0;
}

void StartRegistryMonitor() {
    g_hShutdownEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_hShutdownEvent) {
        return;
    }
    g_hMonitorThread =
        CreateThread(NULL, 0, RegMonitorThreadProc, NULL, 0, NULL);
}

void StopRegistryMonitor() {
    if (g_hShutdownEvent) {
        SetEvent(g_hShutdownEvent);
    }
    if (g_hMonitorThread) {
        WaitForSingleObject(g_hMonitorThread, INFINITE);
        CloseHandle(g_hMonitorThread);
        g_hMonitorThread = NULL;
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

        g_whitelist.emplace_back(appNameSetting);
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
    StartRegistryMonitor();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"> Before Uninit");

    g_unloading = true;

    StopRegistryMonitor();
    ApplySettings();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"> Whitelist changed, reloading...");

    LoadSettings();
    CheckMicrophoneStatus();
}
