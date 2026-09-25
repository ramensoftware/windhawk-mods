// ==WindhawkMod==
// @id              taskbar-disk-space-label
// @name            Taskbar Disk Space Label
// @description     A simple disk space label integrated into the Windows taskbar
// @version         0.59
// @author          allelimo
// @github          https://github.com/allelimo
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Taskbar Disk Space Label

A lightweight free/available disk space label integrated directly into the
Windows 11 taskbar. This is kind of a "remix" of the Taskbar Countdown Timer
module by Richi (https://github.com/richilp) 
The label is showed on primary taskbar only, taskbars on secondary monitors don't show it.
Windows 11 only.

## Features

- Choose the disk to be checked via module settings (default to C)
- Choose the font size (Default to 12)
- Choose between user available free space or total free available space (Default to total free space)
- Choose the update interval (default to 60 seconds)
- Choose the description text on the first line of the label (default to "Free Space")
- Leave the description text empty to have a single-line label with no info text
- When the label gets clipped, please lower the font size, or to clear the description
- Choose the alignment of the label text
## Screenshot

![Screenshot](https://i.imgur.com/Nwuevwf.png)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- diskLetter: "C"
  $name: Disk
  $description: Drive letter, e.g. C
- userFreeSpace: false
  $name: Current user available space
  $description: Select for the current user available free space, unselect for the total free space.  
- showUnit: true
  $name: Show unit 
  $description: Show the unit (GB) for the disk space.
- updateInterval: 60
  $name: Update interval
  $description: Update interval time in seconds. [Default 60]
- fontSize: 12
  $name: Font size
  $description: Font size of the disk label. [Default 12]  
- labelInfoText: "Free Space"
  $name: Label description text. 
  $description: Text on the first line of the label. Leave empty for a single-line label [Default "Free Space"] 
- labelAlignment: left
  $name: Alignment
  $description: Aligns the label text. [Default "Left"]
  $options:
    - left: Left
    - center: Center
    - right: Right  
*/
// ==/WindhawkModSettings==

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <atomic>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <list>
#include <windhawk_utils.h>
#include <cwchar>
#include <string>
#include <cwctype>
#include <optional>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

[[clang::no_destroy]] static TextBlock g_labelText{nullptr};

static std::atomic<HWND> g_taskbarWnd{nullptr};
static std::atomic_bool g_labelInjected{false};
static std::atomic_bool g_systemTrayModuleHooked{false};

[[clang::no_destroy]] static ColumnDefinition g_labelColumn{nullptr};
[[clang::no_destroy]] static std::optional<std::list<FrameworkElement::Loaded_revoker>>
    g_loadedRevokers;

static std::atomic_bool g_unloading{false};

static int myspacefree = 10;
static int myspacetot = 100;

struct {
    std::wstring diskLetter;
    std::wstring labelInfoText;
    std::wstring labelAlignment;
    int fontSize;
    int updateInterval;
    bool userFreeSpace;
    bool showUnit;
} g_settings;

static void ApplyDiskSpaceLabelIfAvailable();

[[clang::no_destroy]] static DispatcherTimer g_refreshTimer{nullptr};
static winrt::event_token g_refreshTickToken{};

// -----------------------------------------------------------------------------
// XAML helpers
// -----------------------------------------------------------------------------

static FrameworkElement FindChildRecursive(
    FrameworkElement element,
    const std::function<bool(FrameworkElement)>& callback,
    int maxDepth = 20) {
    if (!element || maxDepth <= 0) {
        return nullptr;
    }

    int count = VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < count; i++) {
        auto child =
            VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();

        if (!child) {
            continue;
        }

        if (callback(child)) {
            return child;
        }

        auto found = FindChildRecursive(child, callback, maxDepth - 1);

        if (found) {
            return found;
        }
    }

    return nullptr;
}

// -----------------------------------------------------------------------------
// Taskbar window
// -----------------------------------------------------------------------------

static HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            WCHAR className[32];

            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;

                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));

    return result;
}

// -----------------------------------------------------------------------------
// taskbar.dll symbols
// -----------------------------------------------------------------------------

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void* result);

static CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);

static TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);

static std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original =
    nullptr;

static void* CTaskBand_ITaskListWndSite_vftable = nullptr;

static bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!module) {
        Wh_Log(L"ERROR: Could not load taskbar.dll");

        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &std__Ref_count_base__Decref_Original},
    };

    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

// -----------------------------------------------------------------------------
// Get taskbar XAML root
// -----------------------------------------------------------------------------

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original ||
        !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original ||
        !CTaskBand_ITaskListWndSite_vftable) {
        Wh_Log(L"ERROR: Required symbols are missing");

        return nullptr;
    }

    HWND hTaskSwWnd =
        reinterpret_cast<HWND>(GetPropW(hTaskbarWnd, L"TaskbandHWND"));

    if (!hTaskSwWnd) {
        Wh_Log(L"ERROR: TaskbandHWND not found");

        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(hTaskSwWnd, 0));

    if (!taskBand) {
        Wh_Log(L"ERROR: CTaskBand object not found");

        return nullptr;
    }

    void* taskBandForSite = taskBand;

    for (int i = 0; *reinterpret_cast<void**>(taskBandForSite) !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            Wh_Log(L"ERROR: ITaskListWndSite vftable not found");

            return nullptr;
        }

        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};

    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);

    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) {
        if (taskbarHostSharedPtr[1]) {
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        }

        Wh_Log(L"ERROR: TaskbarHost not obtained");

        return nullptr;
    }

    size_t offset = 0x48;

#if defined(_M_X64)
    const BYTE* code =
        reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);

    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] <= 0x7F) {
        offset = code[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
#elif defined(_M_ARM64)
    const DWORD* p =
        reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);

    if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
        p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
        offset = (p[3] >> 12) & 0xFF;
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }
#else
#error "Unsupported architecture"
#endif

    auto* unknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + offset);

    if (!unknown) {
        Wh_Log(L"ERROR: Taskbar XAML object not found");

        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;

    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                            winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}


// -----------------------------------------------------------------------------
// Execute code on taskbar XAML thread
// -----------------------------------------------------------------------------

using RunFromWindowThreadProc_t = void (*)(void*);

static void RunGuarded(RunFromWindowThreadProc_t proc, void* param) {
    try {
        proc(param);
    } catch (...) {
        Wh_Log(L"Taskbar XAML work failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
}


static bool RunFromWindowThread(HWND hWnd,
                                RunFromWindowThreadProc_t proc,
                                void* procParam) {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct Param {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);

    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        RunGuarded(proc, procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);

                if (cwp->message == message) {
                    auto* param = reinterpret_cast<Param*>(cwp->lParam);

                    RunGuarded(param->proc, param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);

    if (!hook) {
        return false;
    }

    Param param{proc, procParam};

    SendMessageW(hWnd, message, 0, reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);

    return true;
}

// -----------------------------------------------------------------------------
// Runs on the taskbar thread. Stops the timer, detaches the label and its
// column from whatever tree they are in, and drops the references.
// -----------------------------------------------------------------------------

static void ReleaseOwnedXaml() {
    if (g_refreshTimer) {
        g_refreshTimer.Stop();
        g_refreshTimer.Tick(g_refreshTickToken);
        g_refreshTimer = nullptr;
    }

    if (g_labelText) {
        auto parentElement =
            VisualTreeHelper::GetParent(g_labelText).try_as<FrameworkElement>();

        auto parent = parentElement.try_as<Panel>();

        if (parent) {
            auto children = parent.Children();

            uint32_t index = 0;

            if (children.IndexOf(g_labelText, index)) {
                children.RemoveAt(index);
            }

            auto parentGrid = parentElement.try_as<Grid>();

            if (parentGrid && g_labelColumn) {
                auto columns = parentGrid.ColumnDefinitions();

                uint32_t columnIndex = 0;
                if (columns.IndexOf(g_labelColumn, columnIndex)) {
                    for (uint32_t i = 0; i < children.Size(); i++) {
                        auto child =
                            children.GetAt(i).try_as<FrameworkElement>();

                        if (!child) {
                            continue;
                        }

                        int currentColumn = Grid::GetColumn(child);

                        if (currentColumn > static_cast<int>(columnIndex)) {
                            Grid::SetColumn(child, currentColumn - 1);
                        }
                    }

                    columns.RemoveAt(columnIndex);
                }
            }
        }

        g_labelColumn = nullptr;
        g_labelText = nullptr;
    }
}

// -----------------------------------------------------------------------------
// get the disk root path
// Accepts "C", "C:" or "C:\" and returns "C:\" (empty if unset).
// -----------------------------------------------------------------------------

static std::wstring GetDiskRootPath() {
    std::wstring root = g_settings.diskLetter;
    if (root.empty()) {
        return root;
    }
    if (root.size() == 1) {
        root += L':';
    }
    if (root.back() != L'\\') {
        root += L'\\';
    }

    std::transform(root.begin(), root.end(), root.begin(), ::towupper);

    return root;
}


// -----------------------------------------------------------------------------
// Get disk information: available free space, total space, total free space
// -----------------------------------------------------------------------------

static void GetDiskInfo() {
    ULARGE_INTEGER freeAvailable, totalBytes, totalFree;

    
    if (GetDiskFreeSpaceExW(GetDiskRootPath().c_str(), &freeAvailable,
                            &totalBytes, &totalFree)) {

        if(g_settings.userFreeSpace){
            myspacefree = freeAvailable.QuadPart / (1024 * 1024 * 1024);
        }
        else{
            myspacefree = totalFree.QuadPart / (1024 * 1024 * 1024);
        }                                
    
        myspacetot = totalBytes.QuadPart / (1024 * 1024 * 1024);

    } else {
        myspacefree = 0;
        myspacetot = 0;
    }
}


// -----------------------------------------------------------------------------
// Format space information to be displayed on the taskbar label
// -----------------------------------------------------------------------------

// static std::wstring FormatSpace(int spacefree,
//                                 int spacetot) {
    
//     std::wstring label = GetDiskRootPath().substr(0, 2);
        
//     if (spacefree == 0 && spacetot == 0) {

//         if (g_settings.labelInfoText.empty()) {
//             return label + L" n/a";
//         } else {
//             return g_settings.labelInfoText + L"\n" + label + L" n/a";
//         }
//     }

//     if (g_settings.showUnit) {

//         if (g_settings.labelInfoText.empty()) {
//             return label + L" " + std::to_wstring(spacefree) + L"/" + std::to_wstring(spacetot) + L" GB";

//         } else {
//             return g_settings.labelInfoText + L"\n" + label + L" " + std::to_wstring(spacefree) + L"/" + std::to_wstring(spacetot) + L" GB";
//         }  

//     } else {

//         if (g_settings.labelInfoText.empty()) {
//             return label + L" " + std::to_wstring(spacefree) + L"/" + std::to_wstring(spacetot);

//         } else {
//             return g_settings.labelInfoText + L"\n" + label + L" " + std::to_wstring(spacefree) + L"/" + std::to_wstring(spacetot);
//         }

//     }
// }

static std::wstring FormatSpace(int spacefree, int spacetot) {
    std::wstring text = GetDiskRootPath().substr(0, 2);
    if (spacefree == 0 && spacetot == 0) {
        text += L" n/a";
    } else {
        text += L" " + std::to_wstring(spacefree) + L"/" + std::to_wstring(spacetot);
        if (g_settings.showUnit) {
            text += L" GB";
        }
    }
    if (!g_settings.labelInfoText.empty()) {
        text = g_settings.labelInfoText + L"\n" + text;
    }
    return text;
}


// -----------------------------------------------------------------------------
// Load settings
// -----------------------------------------------------------------------------

static void LoadSettings() {
    
    g_settings.fontSize = Wh_GetIntSetting(L"fontSize");
    g_settings.diskLetter = WindhawkUtils::StringSetting::make(L"diskLetter").get();
    g_settings.labelInfoText = WindhawkUtils::StringSetting::make(L"labelInfoText").get();

    g_settings.labelAlignment = WindhawkUtils::StringSetting::make(L"labelAlignment").get();

    g_settings.userFreeSpace =  Wh_GetIntSetting(L"userFreeSpace");
    g_settings.showUnit = Wh_GetIntSetting(L"showUnit");
    g_settings.updateInterval = Wh_GetIntSetting(L"updateInterval");

    if (g_settings.fontSize <= 0)
        g_settings.fontSize = 12;

    if (g_settings.updateInterval <= 0)
        g_settings.updateInterval = 60;
}


static void RefreshDiskSpaceLabel(void*) {
    
    if (g_labelText) {
        GetDiskInfo();
        g_labelText.Text(FormatSpace(myspacefree, myspacetot));
        g_labelText.FontSize(g_settings.fontSize);

    // horizontal alignment
    std::wstring align = g_settings.labelAlignment;
    if (align == L"center") {
        g_labelText.TextAlignment(TextAlignment::Center);
    } else if (align == L"right") {
        g_labelText.TextAlignment(TextAlignment::Right);
    } else {
        g_labelText.TextAlignment(TextAlignment::Left);    
    }
     
    }
}


static void ReloadSettingsAndRefresh(void*) {
    LoadSettings();
    if (g_refreshTimer) {
        g_refreshTimer.Interval(std::chrono::seconds(g_settings.updateInterval));
    }    
    RefreshDiskSpaceLabel(nullptr);
}


// -----------------------------------------------------------------------------
// Add/remove taskbar label
// -----------------------------------------------------------------------------

static void AddDiskSpaceLabel(void* param) {
    if (g_unloading.load()) {
        return;
    }

    HWND taskbar = reinterpret_cast<HWND>(param);

    auto xamlRoot = GetTaskbarXamlRoot(taskbar);

    if (!xamlRoot) {
        return;
    }

    auto content = xamlRoot.Content().try_as<FrameworkElement>();

    if (!content) {
        return;
    }

    auto tray = FindChildRecursive(content, [](FrameworkElement element) {
        return element.Name() == L"SystemTrayFrameGrid";
    });

    if (!tray) {
        return;
    }

    auto panel = tray.try_as<Panel>();

    if (!panel) {
        return;
    }

    auto existing = FindChildRecursive(tray, [](FrameworkElement element) {
        return element.Name() == L"TaskbarDiskSpaceLabel";
    });

    // The current XAML tree already has our label.
    if (existing) {
        g_taskbarWnd.store(taskbar);
        g_labelInjected.store(true);
        return;
    }
     
    ReleaseOwnedXaml();
    
    g_labelText = TextBlock();

    g_labelText.Name(L"TaskbarDiskSpaceLabel");

    // get disk iformation
    GetDiskInfo();

    // format the info to be displayed
    g_labelText.Text(
        FormatSpace(myspacefree, myspacetot));
    // vertical alignment, padding and font size
    g_labelText.VerticalAlignment(VerticalAlignment::Center);
    g_labelText.Padding(Thickness{4, 0, 4, 0});
    g_labelText.FontSize(g_settings.fontSize); 

    // horizontal alignment
    std::wstring align = g_settings.labelAlignment;
    if (align == L"center") {
        g_labelText.TextAlignment(TextAlignment::Center);
    } else if (align == L"right") {
        g_labelText.TextAlignment(TextAlignment::Right);
    } else {
        g_labelText.TextAlignment(TextAlignment::Left);    
    }

    auto children = panel.Children();
    auto trayClass = winrt::get_class_name(tray);

    if (trayClass == L"Windows.UI.Xaml.Controls.StackPanel") {
        children.InsertAt(0, g_labelText);
    } else {
        auto trayGrid = tray.try_as<Grid>();

        if (!trayGrid) {
            Wh_Log(L"ERROR: Unsupported SystemTrayFrameGrid layout class: %s",
                   trayClass.c_str());

            ReleaseOwnedXaml();
            return;
        }

        auto columns = trayGrid.ColumnDefinitions(); 

        g_labelColumn = ColumnDefinition();

        g_labelColumn.Width(GridLengthHelper::Auto());

        columns.InsertAt(0, g_labelColumn);

        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i).try_as<FrameworkElement>();

            if (!child) {
                continue;
            }

            Grid::SetColumn(child, Grid::GetColumn(child) + 1);
        }

        Grid::SetColumn(g_labelText, 0);


        children.InsertAt(0, g_labelText);
    }
    
    g_refreshTimer = DispatcherTimer();
    g_refreshTimer.Interval(std::chrono::seconds(g_settings.updateInterval));
    g_refreshTickToken = g_refreshTimer.Tick(
        [](auto&&, auto&&) { RefreshDiskSpaceLabel(nullptr); });
    g_refreshTimer.Start();

    g_taskbarWnd.store(taskbar);
    g_labelInjected.store(true);

    Wh_Log(L"Label added to taskbar"); 
}

static void RemoveDiskSpaceLabel(void*) {

    ReleaseOwnedXaml();
    g_loadedRevokers.reset();
    g_labelInjected.store(false);
}

static void ApplyDiskSpaceLabelIfAvailable() {
    if (g_unloading.load()) {
        return;
    }

    HWND taskbar = FindCurrentProcessTaskbarWnd();

    if (!taskbar) {
        return;
    }

    g_taskbarWnd.store(taskbar);

    if (!RunFromWindowThread(taskbar, AddDiskSpaceLabel, taskbar)) {
        Wh_Log(L"Could not access taskbar UI thread");
    }
}


// -----------------------------------------------------------------------------
// System tray rebuild hook
// -----------------------------------------------------------------------------

using IconView_IconView_t = void*(WINAPI*)(void* pThis);

static IconView_IconView_t IconView_IconView_Original = nullptr;

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);

static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;


static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module) {
    HRSRC resource =
        FindResourceW(module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (!resource) {
        return nullptr;
    }
    HGLOBAL loaded = LoadResource(module, resource);
    void* data = loaded ? LockResource(loaded) : nullptr;
    void* fixedInfo = nullptr;
    UINT fixedInfoSize = 0;
    if (!data || !VerQueryValueW(data, L"\\", &fixedInfo, &fixedInfoSize) ||
        !fixedInfoSize) {
        return nullptr;
    }
    return static_cast<VS_FIXEDFILEINFO*>(fixedInfo);
}

static HMODULE GetSystemTrayModuleHandle() {
    if (HMODULE module = GetModuleHandleW(L"SystemTray.dll")) {
        return module;
    }

    if (HMODULE module = GetModuleHandleW(L"Taskbar.View.dll")) {
         // Taskbar.View.dll 2604+ no longer contains the SystemTray symbols;
        // those builds load SystemTray.dll instead.
        VS_FIXEDFILEINFO* fixedInfo = GetModuleVersionInfo(module);
        WORD major = fixedInfo ? HIWORD(fixedInfo->dwFileVersionMS) : 0;
        if (major && major < 2604) {
            return module;
        }
    }

    return GetModuleHandleW(L"ExplorerExtensions.dll");
}

static void* WINAPI IconView_IconView_Hook(void* pThis) {
    auto result = IconView_IconView_Original(pThis);

    if (g_unloading.load()) {
        return result;
    }

    try {
        FrameworkElement iconView = nullptr;

        reinterpret_cast<IUnknown**>(pThis)[1]->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(iconView));

        if (!iconView) {
            return result;
        }

        g_loadedRevokers->emplace_back();
        auto it = std::prev(g_loadedRevokers->end());

        *it = iconView.Loaded(winrt::auto_revoke_t{},
                        [it](winrt::Windows::Foundation::IInspectable const&,
                        RoutedEventArgs const&) {
                            g_loadedRevokers->erase(it);

                            if (g_unloading.load()) {
                                return;
                            }

                            // A new IconView means the tray may have rebuilt
                            // its XAML tree.
                            g_labelInjected.store(false);
                            ApplyDiskSpaceLabelIfAvailable();
                        });
    } catch (...) {
        Wh_Log(L"IconView hook failed: %08X",
            static_cast<unsigned>(winrt::to_hresult()));        
    }

    return result;
}

static bool HookSystemTraySymbols(HMODULE module) {

    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {{
        {
            LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"
        },
        &IconView_IconView_Original,
        IconView_IconView_Hook,
    }};

    return WindhawkUtils::HookSymbols(
        module,
        systemTrayHooks,
        ARRAYSIZE(systemTrayHooks)
    );
}


static void HandleLoadedModuleIfSystemTray(HMODULE module) {
    if (GetSystemTrayModuleHandle() != module) {
        return;
    }

    if (!g_systemTrayModuleHooked.exchange(true)) {
        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        } else {
            g_systemTrayModuleHooked.store(false);
        }
    }
}

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                          HANDLE file,
                                          DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);

    if (module) {
        HandleLoadedModuleIfSystemTray(module);
    }

    return module;
}


// -----------------------------------------------------------------------------
// Windhawk
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Taskbar Disk Space Label loading");

    g_unloading.store(false);
    g_loadedRevokers.emplace();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"ERROR: Failed to resolve taskbar.dll symbols");

        return FALSE;
    }

    if (HMODULE systemTray = GetSystemTrayModuleHandle()) {
        if (HookSystemTraySymbols(systemTray)) {
            g_systemTrayModuleHooked.store(true);
        }
    } else {
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");

        auto loadLibraryExW =
            kernelbase ? reinterpret_cast<LoadLibraryExW_t>(
                             GetProcAddress(kernelbase, "LoadLibraryExW"))
                       : nullptr;

        if (loadLibraryExW) {
            WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
        }
    }

    LoadSettings();

    return TRUE;
}

void Wh_ModAfterInit() {
    if (HMODULE systemTray = GetSystemTrayModuleHandle()) {
        HandleLoadedModuleIfSystemTray(systemTray);
    }

    ApplyDiskSpaceLabelIfAvailable();

}


void Wh_ModBeforeUninit() {
    g_unloading.store(true);
}

void Wh_ModUninit() {
    g_unloading.store(true);

    bool removed = false;

    for (int attempt = 0; attempt < 5 && !removed; attempt++) {
        HWND taskbar = g_taskbarWnd.load();
        if (!taskbar || !IsWindow(taskbar)) {
            taskbar = FindCurrentProcessTaskbarWnd();
        }

        if (taskbar) {
            removed =
                RunFromWindowThread(taskbar, RemoveDiskSpaceLabel, nullptr);
        }

        if (!removed) {
            Sleep(50);
        }
    }

    if (!removed && g_labelInjected.load()) {
        Wh_Log(L"ERROR: Could not remove label XAML objects before unload");
    }

    Wh_Log(L"Taskbar Disk Space Label unloaded");
}

void Wh_ModSettingsChanged() {
    HWND taskbar = g_taskbarWnd.load();

    if (!taskbar || !IsWindow(taskbar)) {
        taskbar = FindCurrentProcessTaskbarWnd();
    }
    
    if (taskbar) {
        RunFromWindowThread(taskbar, ReloadSettingsAndRefresh, nullptr);
    }

    if (!g_labelInjected.load()) {
        ApplyDiskSpaceLabelIfAvailable();
    }
}
