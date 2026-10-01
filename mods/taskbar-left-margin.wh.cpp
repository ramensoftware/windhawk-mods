// ==WindhawkMod==
// @id              taskbar-left-margin
// @name            Taskbar Left Margin
// @name:zh-CN      任务栏左边距
// @description     Adds a configurable left margin to the taskbar content
// @description:zh-CN 为任务栏内容添加可调节的左边距
// @version         1.0
// @author          loliri
// @github          https://github.com/loliri
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshlwapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The XAML diagnostics and visual tree watching code is based on the
// Windows 11 Taskbar Styler mod by m417z, which is also licensed under the
// GNU General Public License v3.0:
// https://github.com/m417z/my-windhawk-mods
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/loliri/windhawk-taskbar-left-margin/issues

// ==WindhawkModReadme==
/*
# Taskbar Left Margin

Shifts the taskbar **content** to the right by a configurable number of pixels,
leaving an empty margin on the left. The taskbar background stays full width.

Only the taskbar itself is affected.

## Notes

Requires Windows 11.

## Compatibility

- **TranslucentTB** is confirmed compatible and can be used alongside this mod.
- Tested on Windows 11 26H2.

This mod uses XAML diagnostics to modify taskbar elements. There can only be one
XAML diagnostics consumer at a time, so it may conflict with other tools which
use it.

## Suggested use

Together with [FluentFlyout](https://github.com/unchihugo/FluentFlyout): enable
the taskbar widget there, set its position to the bottom left corner, and turn
on the fixed widget width. The taskbar elements then tile linearly instead of
overlapping each other.

## Implementation notes

The VisualTreeWatcher implementation is based on the
[ExplorerTAP](https://github.com/TranslucentTB/TranslucentTB/tree/develop/ExplorerTAP)
code from the **TranslucentTB** project.

### How the context menu is positioned

The taskbar context menu (the jump list) is not laid out by XAML. Its anchor
point is computed in `explorer.exe` by `CTaskListWnd::_ComputeJumpViewPosition`
in `taskbar.dll`, and handed to `ShellExperienceHost.exe`, which draws the menu
there. The point is in physical screen pixels.

The consequences below were all verified on a 150% display:

* Shifting the taskbar's XAML content does not move the menu, because the menu
  is not placed relative to it. The mod adjusts the anchor point instead, which
  is why it hooks that function.
* The menu's content is its own visual tree. The chain stops at
  `JumpViewUI.TaskbarJumpListFrame > Border > ScrollContentPresenter >
  ScrollViewer`, with no visual or logical parent beyond it, and the popup
  carrying it is never reported to the XAML diagnostics callback. Moving the
  content directly is not possible.
* The menu's `XamlRoot` is exactly the size of the menu, so a render transform
  on the content is clipped by it rather than moving the menu.
* The menu is drawn into a `Windows.UI.Core.CoreWindow`, but that window's
  rectangle does not correspond to the menu's visible area, so moving the window
  has no effect either.

Because the anchor is in physical pixels, the setting is converted according to
the display scale when **Follow display DPI** is enabled.

## Feedback

Bug reports and feature requests are welcome at
[loliri/windhawk-taskbar-left-margin](https://github.com/loliri/windhawk-taskbar-left-margin/issues).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- leftMargin: 220
  $name: Left margin (pixels)
  $name:zh-CN: 左边距（像素）
  $description: >-
    How many pixels of empty space to leave on the left of the taskbar content.
    A larger value shifts the content further right.
  $description:zh-CN: >-
    任务栏内容左侧留空的像素数，数值越大整体越靠右。
- followDpi: true
  $name: Follow display DPI
  $name:zh-CN: 跟随显示器 DPI
  $description: >-
    Scale the margin together with the display DPI, so it keeps the same visual
    size on high-DPI displays. Turn off to keep the margin at a constant
    physical pixel size.
  $description:zh-CN: >-
    让边距随显示器 DPI 一同缩放，在高 DPI 显示器上保持相同的视觉大小。关闭后边距将固定为恒定的物理像素数。
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <xamlom.h>

#include <atomic>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.h>

#include <Unknwn.h>
#include <combaseapi.h>
#include <ocidl.h>
#include <weakreference.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <chrono>
#include <utility>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

using namespace winrt::Windows::UI::Xaml;

namespace wuxc = winrt::Windows::UI::Xaml::Controls;

int g_leftMargin;
bool g_followDpi;

// The jump list (taskbar context menu) is positioned by taskbar.dll, which
// computes a screen-space anchor point and hands it to the process that draws
// the menu. That anchor is computed from the taskbar's own layout, not from the
// XAML positions of the buttons, so shifting the taskbar content does not move
// the menu. The anchor is adjusted here instead, which moves the menu itself.
//
// The anchor is in physical screen pixels, while the margin setting is in DIPs,
// hence the DPI conversion.
std::atomic<int> g_anchorOffsetPx{0};

std::atomic<bool> g_initialized;
thread_local bool g_initializedForThread;

struct AppliedElement
{
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> element;
    winrt::Windows::UI::Xaml::DependencyProperty property;
};

thread_local std::vector<AppliedElement> g_appliedElements;

HMODULE GetCurrentModuleHandle()
{
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module))
    {
        return nullptr;
    }

    return module;
}

// XamlDiagnostics implements this interface too, and xamlom.h does not declare
// it. UnregisterInstance closes the runtime object cached for a handle, the
// only reference the diagnostics keep to an element once it was reported.
static constexpr GUID IID_IXamlDiagnosticsTestHooks =
    {0x735941a2, 0x3ee3, 0x495a, {0x8d, 0xa9, 0x97, 0x26, 0x27, 0x00, 0x30, 0x75}};

struct IXamlDiagnosticsTestHooks : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE UnregisterInstance(
        InstanceHandle handle) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetDispatcherQueueForObject(
        InstanceHandle handle,
        void **dispatcherQueue) = 0;
};

// The diagnostics create a runtime object which holds every element they
// report, and drop it only once the element is reported as removed. Removals
// are reported for elements taken out of their parent, but a tree which is
// discarded whole is never taken apart that way. Elements nothing is keyed by
// are handed back from here so the teardown can happen.
//
// Reporting an element recreates the runtime object of its parent, which is why
// each report queues the parent as well, and why the queue is drained only once
// the burst of reports has stopped: releasing mid-burst would just be undone by
// the next child of whatever was released.
thread_local std::vector<InstanceHandle> g_pendingDiagnosticsRelease;
thread_local ULONGLONG g_lastDiagnosticsReleaseQueueTick;
thread_local bool g_diagnosticsReleaseDrainQueued;
thread_local winrt::Windows::System::DispatcherQueueTimer
    g_diagnosticsReleaseDrainTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    g_diagnosticsReleaseDrainTimerTickRevoker;

// Long enough to sit out a tree being built.
constexpr ULONGLONG kDiagnosticsReleaseDelay = 200;

// The drain waits on a one-shot timer, which the thread teardown can stop,
// rather than on a dispatcher item, which it cannot: the module is freed once
// the mod is uninitialized, and an item still on the dispatcher would call into
// it. The interval only has to carry the drain out of the report which arms it.
constexpr ULONGLONG kDiagnosticsReleaseDrainDelay = 1;

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile>
{
public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site);

    VisualTreeWatcher(const VisualTreeWatcher &) = delete;
    VisualTreeWatcher &operator=(const VisualTreeWatcher &) = delete;

    VisualTreeWatcher(VisualTreeWatcher &&) = delete;
    VisualTreeWatcher &operator=(VisualTreeWatcher &&) = delete;

    ~VisualTreeWatcher();

    void UnadviseVisualTreeChange();

    bool ReleaseDiagnosticsReference(InstanceHandle handle);

    winrt::Windows::Foundation::IInspectable FromHandle(
        InstanceHandle handle)
    {
        winrt::Windows::Foundation::IInspectable obj{nullptr};
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(
            handle,
            reinterpret_cast<::IInspectable **>(winrt::put_abi(obj))));
        return obj;
    }

private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation relation,
        VisualElement element,
        VisualMutationType mutationType) override;
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle element,
                                                    VisualElementState elementState,
                                                    LPCWSTR context) noexcept override;

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
    winrt::com_ptr<IXamlDiagnosticsTestHooks> m_XamlDiagnosticsTestHooks =
        nullptr;
};

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// {C85D8CC7-5463-40E8-A432-F5916B6427E5}
static constexpr CLSID CLSID_WindhawkTAP = {
    0xc85d8cc7, 0x5463, 0x40e8, {0xa4, 0x32, 0xf5, 0x91, 0x6b, 0x64, 0x27, 0xe5}};

class WindhawkTAP : public winrt::implements<WindhawkTAP,
                                             IObjectWithSite,
                                             winrt::non_agile>
{
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown *pUnkSite) override;
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void **ppvSite) noexcept override;

private:
    winrt::com_ptr<IUnknown> site;
};

template <class T>
struct SimpleFactory
    : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile>
{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown *pUnkOuter,
                                             REFIID riid,
                                             void **ppvObject) override
    try
    {
        if (!pUnkOuter)
        {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        }
        else
        {
            return CLASS_E_NOAGGREGATION;
        }
    }
    catch (...)
    {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override
    {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) _Use_decl_annotations_ STDAPI
DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
try
{
    if (rclsid == CLSID_WindhawkTAP)
    {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    }
    else
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

__declspec(dllexport) _Use_decl_annotations_ STDAPI DllCanUnloadNow()
{
    if (winrt::get_module_lock())
    {
        return S_FALSE;
    }
    else
    {
        return S_OK;
    }
}

#pragma clang diagnostic pop

HRESULT WindhawkTAP::SetSite(IUnknown *pUnkSite)
try
{
    // Only ever 1 VTW at once.
    if (g_visualTreeWatcher)
    {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    site.copy_from(pUnkSite);

    if (site)
    {
        // Decrease refcount increased by InitializeXamlDiagnosticsEx.
        FreeLibrary(GetCurrentModuleHandle());

        g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

HRESULT WindhawkTAP::GetSite(REFIID riid, void **ppvSite) noexcept
{
    return site.as(riid, ppvSite);
}

bool g_inInjectWindhawkTAP = false;

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX =
    decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkTAP() noexcept
{
    HMODULE module = GetCurrentModuleHandle();
    if (!module)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location)))
    {
    case 0:
    case ARRAYSIZE(location):
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux = LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr,
                                      LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!wux) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // I didn't find a better way than trying many connections until one works.
    g_inInjectWindhawkTAP = true;

    HRESULT hr;
    for (int i = 0; i < 10000; i++)
    {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"VisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), L"", location,
                  CLSID_WindhawkTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            break;
        }
    }

    g_inInjectWindhawkTAP = false;

    return hr;
}

// The XAML composition diagnostics rebuild a process-wide visual tree walker
// without any locking whenever a DirectComposition visual is added, so any UI
// thread which adds one corrupts the heap while another thread is in the same
// code. Only element mutations are needed here, and those are reported by an
// unrelated code path, so the composition diagnostics are kept from being
// created at all: XamlDiagnostics::CreateCompVisualDiag skips them when the
// HKLM\Software\Microsoft\XAML\Debug\DisableCompositionDiag value is 1.
// Windows.UI.Xaml.dll reads and caches the value once, from within
// AdviseVisualTreeChange, so it is faked for exactly that window.
thread_local bool g_reportCompositionDiagAsDisabled;

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExW_Original;
LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey,
                                  LPCWSTR lpSubKey,
                                  DWORD ulOptions,
                                  REGSAM samDesired,
                                  PHKEY phkResult)
{
    LSTATUS result = RegOpenKeyExW_Original(hKey, lpSubKey, ulOptions,
                                            samDesired, phkResult);
    if (result == ERROR_SUCCESS || !g_reportCompositionDiagAsDisabled ||
        hKey != HKEY_LOCAL_MACHINE || !lpSubKey ||
        _wcsicmp(lpSubKey, L"Software\\Microsoft\\XAML\\Debug") != 0)
    {
        return result;
    }

    // The key usually doesn't exist, and the value isn't queried unless the key
    // could be opened, so hand out a key which does exist.
    Wh_Log(L"Substituting the XAML debug key");
    return RegOpenKeyExW_Original(HKEY_LOCAL_MACHINE, L"Software\\Microsoft",
                                  ulOptions, samDesired, phkResult);
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey,
                                     LPCWSTR lpValueName,
                                     LPDWORD lpReserved,
                                     LPDWORD lpType,
                                     LPBYTE lpData,
                                     LPDWORD lpcbData)
{
    if (!g_reportCompositionDiagAsDisabled || !lpValueName ||
        _wcsicmp(lpValueName, L"DisableCompositionDiag") != 0)
    {
        return RegQueryValueExW_Original(hKey, lpValueName, lpReserved, lpType,
                                         lpData, lpcbData);
    }

    Wh_Log(L"Reporting DisableCompositionDiag as set");

    if (lpType)
    {
        *lpType = REG_DWORD;
    }

    if (lpData && (!lpcbData || *lpcbData < sizeof(DWORD)))
    {
        if (lpcbData)
        {
            *lpcbData = sizeof(DWORD);
        }
        return ERROR_MORE_DATA;
    }

    if (lpData)
    {
        *reinterpret_cast<DWORD *>(lpData) = 1;
    }

    if (lpcbData)
    {
        *lpcbData = sizeof(DWORD);
    }

    return ERROR_SUCCESS;
}

void FlushDiagnosticsReleases()
{
    auto pending = std::move(g_pendingDiagnosticsRelease);
    g_pendingDiagnosticsRelease.clear();

    if (!g_visualTreeWatcher)
    {
        return;
    }

    // A handle is queued once per report naming it, so a parent appears once
    // per child.
    std::sort(pending.begin(), pending.end());
    pending.erase(std::unique(pending.begin(), pending.end()), pending.end());

    for (InstanceHandle handle : pending)
    {
        g_visualTreeWatcher->ReleaseDiagnosticsReference(handle);
    }
}

void QueueDiagnosticsRelease(InstanceHandle handle)
{
    if (!handle)
    {
        return;
    }

    g_pendingDiagnosticsRelease.push_back(handle);
    g_lastDiagnosticsReleaseQueueTick = GetTickCount64();
}

// Whether the burst has stopped is decided when this is scheduled: the report
// which schedules it queues its own handles right afterwards, so the time since
// the last queue is short again by the time this runs.
void DrainDiagnosticsReleases()
{
    g_diagnosticsReleaseDrainQueued = false;
    FlushDiagnosticsReleases();
}

// Reports arrive from inside XAML's own Enter and Leave walks, and a release
// there re-enters the diagnostics while the tree is being mutated: dropping the
// last reference to an element the walk is still visiting destroys it mid-walk.
// The drain is therefore armed on the thread's dispatcher, which runs it once
// the walk has finished.
void FlushDiagnosticsReleasesIfQuiet()
{
    if (g_pendingDiagnosticsRelease.empty() ||
        g_diagnosticsReleaseDrainQueued ||
        GetTickCount64() - g_lastDiagnosticsReleaseQueueTick <
            kDiagnosticsReleaseDelay)
    {
        return;
    }

    try
    {
        if (!g_diagnosticsReleaseDrainTimer)
        {
            auto dispatcherQueue =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (!dispatcherQueue)
            {
                // Releasing from here is the one thing that isn't safe, so the
                // elements stay held instead.
                Wh_Log(L"No dispatcher queue, elements will be held");
                return;
            }

            g_diagnosticsReleaseDrainTimer = dispatcherQueue.CreateTimer();
            g_diagnosticsReleaseDrainTimer.IsRepeating(false);
            g_diagnosticsReleaseDrainTimer.Interval(
                std::chrono::milliseconds{kDiagnosticsReleaseDrainDelay});
            g_diagnosticsReleaseDrainTimerTickRevoker =
                g_diagnosticsReleaseDrainTimer.Tick(
                    winrt::auto_revoke,
                    [](winrt::Windows::System::DispatcherQueueTimer const &,
                       winrt::Windows::Foundation::IInspectable const &)
                    {
                        DrainDiagnosticsReleases();
                    });
        }

        g_diagnosticsReleaseDrainTimer.Start();
        g_diagnosticsReleaseDrainQueued = true;
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

void StopDiagnosticsReleases()
{
    g_pendingDiagnosticsRelease.clear();

    if (g_diagnosticsReleaseDrainTimer)
    {
        try
        {
            g_diagnosticsReleaseDrainTimer.Stop();
        }
        catch (winrt::hresult_error const &ex)
        {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    g_diagnosticsReleaseDrainTimerTickRevoker.revoke();
    g_diagnosticsReleaseDrainTimer = nullptr;
    g_diagnosticsReleaseDrainQueued = false;
}

bool IsElementOfType(const VisualElement &element, PCWSTR type)
{
    return element.Type && _wcsicmp(element.Type, type) == 0;
}

bool ParentIsOfType(VisualTreeWatcher *watcher,
                    InstanceHandle parentHandle,
                    PCWSTR type)
{
    if (!parentHandle)
    {
        return false;
    }

    try
    {
        auto parent = watcher->FromHandle(parentHandle);
        return parent &&
               _wcsicmp(winrt::get_class_name(parent).c_str(), type) == 0;
    }
    catch (...)
    {
        return false;
    }
}

bool ParentIsNamed(VisualTreeWatcher *watcher,
                   InstanceHandle parentHandle,
                   PCWSTR name)
{
    if (!parentHandle)
    {
        return false;
    }

    try
    {
        auto parent = watcher->FromHandle(parentHandle);
        auto parentElement =
            parent ? parent.try_as<winrt::Windows::UI::Xaml::FrameworkElement>()
                   : nullptr;
        return parentElement && _wcsicmp(parentElement.Name().c_str(), name) == 0;
    }
    catch (...)
    {
        return false;
    }
}

void RecordApplied(winrt::Windows::UI::Xaml::FrameworkElement const &element,
                   winrt::Windows::UI::Xaml::DependencyProperty property)
{
    g_appliedElements.push_back({winrt::make_weak(element), property});
}

// The margin in DIPs, which is what XAML expects. With DPI following on the
// setting is taken as DIPs; with it off the setting is taken as physical pixels
// and converted, so the margin keeps a constant physical size.
double GetMarginInDips(
    winrt::Windows::UI::Xaml::FrameworkElement const &element)
{
    if (g_followDpi)
    {
        return static_cast<double>(g_leftMargin);
    }

    try
    {
        auto xamlRoot = element.XamlRoot();
        if (xamlRoot)
        {
            double scale = xamlRoot.RasterizationScale();
            if (scale > 0)
            {
                return static_cast<double>(g_leftMargin) / scale;
            }
        }
    }
    catch (...)
    {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }

    return static_cast<double>(g_leftMargin);
}

// The margin in physical screen pixels, which is the unit the jump list anchor
// uses.
int GetMarginInPixels(
    winrt::Windows::UI::Xaml::FrameworkElement const &element)
{
    if (!g_followDpi)
    {
        return g_leftMargin;
    }

    try
    {
        auto xamlRoot = element.XamlRoot();
        if (xamlRoot)
        {
            double scale = xamlRoot.RasterizationScale();
            if (scale > 0)
            {
                return static_cast<int>(g_leftMargin * scale);
            }
        }
    }
    catch (...)
    {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }

    return g_leftMargin;
}

void ApplyLeftMargin(VisualTreeWatcher *watcher,
                     const ParentChildRelation &relation,
                     const VisualElement &element,
                     winrt::Windows::UI::Xaml::FrameworkElement const &frameworkElement)
{
    double margin = GetMarginInDips(frameworkElement);

    if (IsElementOfType(element, L"Windows.UI.Xaml.Controls.Grid") &&
        element.Name && _wcsicmp(element.Name, L"RootGrid") == 0)
    {
        if (!ParentIsOfType(watcher, relation.Parent,
                            L"Taskbar.TaskbarFrame"))
        {
            return;
        }

        auto grid = frameworkElement.try_as<wuxc::Grid>();
        if (!grid)
        {
            Wh_Log(L"RootGrid is not a Grid, skipping");
            return;
        }

        grid.Padding(winrt::Windows::UI::Xaml::Thickness{margin, 0, 0, 0});
        RecordApplied(frameworkElement, wuxc::Grid::PaddingProperty());

        // The jump list anchor is computed in physical pixels, so the offset
        // the hook adds is kept in the same unit.
        g_anchorOffsetPx.store(GetMarginInPixels(frameworkElement));
    }
    else if (IsElementOfType(element, L"Taskbar.TaskbarBackground"))
    {
        if (!ParentIsNamed(watcher, relation.Parent, L"RootGrid"))
        {
            return;
        }

        frameworkElement.Margin(
            winrt::Windows::UI::Xaml::Thickness{-margin, 0, 0, 0});
        RecordApplied(frameworkElement,
                      winrt::Windows::UI::Xaml::FrameworkElement::MarginProperty());
    }
}

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site) : m_XamlDiagnostics(site.as<IXamlDiagnostics>())
{
    Wh_Log(L"Constructing VisualTreeWatcher");

    HRESULT hr = m_XamlDiagnostics->QueryInterface(
        IID_IXamlDiagnosticsTestHooks, m_XamlDiagnosticsTestHooks.put_void());
    if (FAILED(hr))
    {
        Wh_Log(L"IXamlDiagnosticsTestHooks is unavailable, elements will be leaked: %08X",
               hr);
    }

    // Calling AdviseVisualTreeChange from the current thread causes the app to
    // hang in Advising::RunOnUIThread sometimes. Creating a new thread and
    // calling it from there fixes it.
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD
        {
            auto watcher = reinterpret_cast<VisualTreeWatcher *>(lpParam);
            auto service = watcher->m_XamlDiagnostics.as<IVisualTreeService3>();
            g_reportCompositionDiagAsDisabled = true;
            HRESULT hr = service->AdviseVisualTreeChange(watcher);
            g_reportCompositionDiagAsDisabled = false;
            watcher->Release();
            if (FAILED(hr))
            {
                Wh_Log(L"AdviseVisualTreeChange failed with error %08X", hr);
            }
            return 0;
        },
        this, 0, nullptr);
    if (thread)
    {
        AddRef();
        CloseHandle(thread);
    }
}

VisualTreeWatcher::~VisualTreeWatcher()
{
    Wh_Log(L"Destructing VisualTreeWatcher");
}

void VisualTreeWatcher::UnadviseVisualTreeChange()
{
    Wh_Log(L"UnadviseVisualTreeChange VisualTreeWatcher");
    HRESULT hr =
        m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(
            this);
    if (FAILED(hr))
    {
        Wh_Log(L"UnadviseVisualTreeChange failed with error %08X", hr);
    }
}

bool VisualTreeWatcher::ReleaseDiagnosticsReference(InstanceHandle handle)
{
    if (!m_XamlDiagnosticsTestHooks)
    {
        return false;
    }

    HRESULT hr = m_XamlDiagnosticsTestHooks->UnregisterInstance(handle);
    if (FAILED(hr))
    {
        Wh_Log(L"UnregisterInstance failed with error %08X", hr);
        return false;
    }

    return true;
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation relation,
                                              VisualElement element,
                                              VisualMutationType mutationType)
try
{
    if (!g_initializedForThread)
    {
        return S_OK;
    }

    if (mutationType == Add)
    {
        try
        {
            auto inspectable = FromHandle(element.Handle);
            auto frameworkElement =
                inspectable
                    ? inspectable
                          .try_as<winrt::Windows::UI::Xaml::FrameworkElement>()
                    : nullptr;
            if (frameworkElement)
            {
                ApplyLeftMargin(this, relation, element, frameworkElement);
            }
        }
        catch (...)
        {
            Wh_Log(L"Error %08X", winrt::to_hresult());
        }
    }

    FlushDiagnosticsReleasesIfQuiet();

    if (mutationType == Add)
    {
        QueueDiagnosticsRelease(element.Handle);
        QueueDiagnosticsRelease(relation.Parent);
    }
    else if (mutationType == Remove)
    {
        QueueDiagnosticsRelease(element.Handle);
    }

    return S_OK;
}
catch (...)
{
    Wh_Log(L"Error %08X", winrt::to_hresult());

    // Returning an error prevents (some?) further messages, always return
    // success.
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle,
                                                 VisualElementState,
                                                 LPCWSTR) noexcept
{
    return S_OK;
}

void UninitializeForCurrentThread()
{
    if (!g_initializedForThread)
    {
        return;
    }

    for (const auto &applied : g_appliedElements)
    {
        if (auto element = applied.element.get())
        {
            try
            {
                element.ClearValue(applied.property);
            }
            catch (winrt::hresult_error const &ex)
            {
                Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }
        }
    }

    g_appliedElements.clear();

    g_anchorOffsetPx.store(0);

    StopDiagnosticsReleases();

    g_initializedForThread = false;
}

void InitializeForCurrentThread()
{
    if (g_initializedForThread)
    {
        return;
    }

    g_initializedForThread = true;
}

void InitializeSettingsAndTap()
{
    if (g_initialized.exchange(true))
    {
        return;
    }

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr))
    {
        Wh_Log(L"Error %08X", hr);
    }
}

void UninitializeSettingsAndTap()
{
    if (g_visualTreeWatcher)
    {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    g_initialized = false;
}

using RunFromWindowThreadProc_t = void(WINAPI *)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam)
{
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM
    {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0)
    {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId())
    {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            if (nCode == HC_ACTION)
            {
                const CWPSTRUCT *cwp = (const CWPSTRUCT *)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg)
                {
                    RUN_FROM_WINDOW_THREAD_PARAM *param =
                        (RUN_FROM_WINDOW_THREAD_PARAM *)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook)
    {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

void OnWindowCreated(HWND hWnd,
                     HWND hWndParent,
                     LPCWSTR lpClassName,
                     PCSTR funcName)
{
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    WCHAR className[64];
    if (hWndParent && GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 &&
        GetClassName(hWndParent, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0)
    {
        Wh_Log(L"Initializing - Created DesktopWindowContentBridge window");
        InitializeForCurrentThread();
        InitializeSettingsAndTap();
        return;
    }

    if (bTextualClassName &&
        (_wcsicmp(lpClassName, L"XamlExplorerHostIslandWindow") == 0 ||
         _wcsicmp(lpClassName, L"Shell_InputSwitchTopLevelWindow") == 0))
    {
        Wh_Log(L"Initializing - Created XAML host window: %08X via %S",
               (DWORD)(ULONG_PTR)hWnd, funcName);
        InitializeForCurrentThread();
        InitializeSettingsAndTap();
        return;
    }
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
                                 PVOID lpParam)
{
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd)
    {
        return hWnd;
    }

    OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI *)(DWORD dwExStyle,
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
                                            PVOID lpParam,
                                            DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
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
                                    PVOID lpParam,
                                    DWORD dwBand)
{
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd)
    {
        return hWnd;
    }

    OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);

    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI *)(DWORD dwExStyle,
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
                                              PVOID lpParam,
                                              DWORD dwBand,
                                              DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
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
                                      PVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlags)
{
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd)
    {
        return hWnd;
    }

    OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);

    return hWnd;
}

PFN_INITIALIZE_XAML_DIAGNOSTICS_EX InitializeXamlDiagnosticsEx_Original;
HRESULT WINAPI InitializeXamlDiagnosticsEx_Hook(
    _In_ PCWSTR endPointName,
    _In_ DWORD pid,
    _In_ PCWSTR wszDllXamlDiagnostics,
    _In_ PCWSTR wszTAPDllName,
    _In_ CLSID tapClsid,
    _In_opt_ PCWSTR wszInitializationData)
{
    if (g_inInjectWindhawkTAP)
    {
        return InitializeXamlDiagnosticsEx_Original(
            endPointName, pid, wszDllXamlDiagnostics, wszTAPDllName, tapClsid,
            wszInitializationData);
    }

    // Another consumer is asking for the diagnostics. There can only be one at
    // a time; let the other one have it rather than breaking it.
    Wh_Log(L"Allowing InitializeXamlDiagnosticsEx call");
    return InitializeXamlDiagnosticsEx_Original(
        endPointName, pid, wszDllXamlDiagnostics, wszTAPDllName, tapClsid,
        wszInitializationData);
}

bool HookInitializeXamlDiagnosticsExIfNeeded()
{
    if (InitializeXamlDiagnosticsEx_Original)
    {
        return false; // Already hooked
    }

    const HMODULE wux = GetModuleHandle(L"Windows.UI.Xaml.dll");
    if (!wux)
    {
        return false; // DLL not loaded yet
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde)
    {
        return false;
    }

    Wh_Log(L"Hooking InitializeXamlDiagnosticsEx");
    return WindhawkUtils::SetFunctionHook(ixde,
                                          InitializeXamlDiagnosticsEx_Hook,
                                          &InitializeXamlDiagnosticsEx_Original);
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags)
{
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module && !InitializeXamlDiagnosticsEx_Original && lpLibFileName)
    {
        PCWSTR fileName = wcsrchr(lpLibFileName, L'\\');
        fileName = fileName ? fileName + 1 : lpLibFileName;
        if (_wcsicmp(fileName, L"Windows.UI.Xaml.dll") == 0 &&
            HookInitializeXamlDiagnosticsExIfNeeded())
        {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

std::vector<HWND> GetXamlHostWnds()
{
    struct ENUM_WINDOWS_PARAM
    {
        std::vector<HWND> *hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            ENUM_WINDOWS_PARAM &param = *(ENUM_WINDOWS_PARAM *)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
            {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
            {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"XamlExplorerHostIslandWindow") == 0 ||
                _wcsicmp(szClassName, L"Shell_InputSwitchTopLevelWindow") == 0)
            {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

HWND FindCurrentProcessTaskbarWnd()
{
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0)
            {
                *reinterpret_cast<HWND *>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

HWND GetTaskbarUiWnd()
{
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd)
    {
        return nullptr;
    }

    return FindWindowEx(hTaskbarWnd, nullptr,
                        L"Windows.UI.Composition.DesktopWindowContentBridge",
                        nullptr);
}

void LoadSettings()
{
    g_leftMargin = Wh_GetIntSetting(L"leftMargin");
    g_followDpi = Wh_GetIntSetting(L"followDpi") != 0;
}

// taskbar.dll computes the jump list's anchor point in physical screen pixels
// and hands it to the process that draws the menu, so the menu is positioned
// from here rather than from the XAML layout of the taskbar buttons. Adjusting
// the point is what moves the menu, and it does so for every way the menu can be
// opened: mouse, touch and keyboard alike.
using ComputeJumpViewPosition_t = HRESULT(WINAPI*)(
    void* pThis,
    void* pTaskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point* point,
    winrt::Windows::UI::Xaml::HorizontalAlignment* hAlign,
    winrt::Windows::UI::Xaml::VerticalAlignment* vAlign);

ComputeJumpViewPosition_t ComputeJumpViewPosition_Original;

HRESULT WINAPI ComputeJumpViewPosition_Hook(
    void* pThis,
    void* pTaskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point* point,
    winrt::Windows::UI::Xaml::HorizontalAlignment* hAlign,
    winrt::Windows::UI::Xaml::VerticalAlignment* vAlign)
{
    HRESULT hr = ComputeJumpViewPosition_Original(pThis, pTaskBtnGroup, param2,
                                                  point, hAlign, vAlign);

    int offset = g_anchorOffsetPx.load();
    if (SUCCEEDED(hr) && point && offset)
    {
        float originalX = point->X;
        point->X += static_cast<float>(offset);
        Wh_Log(L"Jump list anchor x: %d -> %d", (int)originalX, (int)point->X);
    }

    return hr;
}

bool HookTaskbarDllSymbols()
{
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module)
    {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
            &ComputeJumpViewPosition_Original,
            ComputeJumpViewPosition_Hook,
            // Optional: if the symbol is unavailable the mod still loads, and
            // the menu simply keeps its original position.
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks)))
    {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    Wh_Log(L"Hooked taskbar.dll symbols");
    return true;
}

BOOL Wh_ModInit()
{
    Wh_Log(L">");

    LoadSettings();

    HookTaskbarDllSymbols();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module)
    {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand)
        {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }

        auto pCreateWindowInBandEx = (CreateWindowInBandEx_t)GetProcAddress(
            user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx)
        {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBandEx,
                                           CreateWindowInBandEx_Hook,
                                           &CreateWindowInBandEx_Original);
        }
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    auto pKernelBaseRegOpenKeyExW =
        (RegOpenKeyExW_t)GetProcAddress(kernelBaseModule, "RegOpenKeyExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseRegOpenKeyExW, RegOpenKeyExW_Hook,
                                   &RegOpenKeyExW_Original);

    auto pKernelBaseRegQueryValueExW = (RegQueryValueExW_t)GetProcAddress(
        kernelBaseModule, "RegQueryValueExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseRegQueryValueExW,
                                   RegQueryValueExW_Hook,
                                   &RegQueryValueExW_Original);

    // Hook immediately if DLL is already loaded.
    HookInitializeXamlDiagnosticsExIfNeeded();

    return TRUE;
}

void Wh_ModAfterInit()
{
    Wh_Log(L">");

    bool initialize = false;

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd)
    {
        Wh_Log(L"Initializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID)
            { InitializeForCurrentThread(); },
            nullptr);
        initialize = true;
    }

    for (auto hXamlHostWnd : GetXamlHostWnds())
    {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd, [](PVOID)
            { InitializeForCurrentThread(); }, nullptr);
        initialize = true;
    }

    if (initialize)
    {
        InitializeSettingsAndTap();
    }
}

void Wh_ModUninit()
{
    Wh_Log(L">");

    UninitializeSettingsAndTap();

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd)
    {
        Wh_Log(L"Uninitializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID)
            { UninitializeForCurrentThread(); },
            nullptr);
    }

    for (auto hXamlHostWnd : GetXamlHostWnds())
    {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd, [](PVOID)
            { UninitializeForCurrentThread(); },
            nullptr);
    }
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L">");

    UninitializeSettingsAndTap();

    LoadSettings();

    bool initialize = false;

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd)
    {
        Wh_Log(L"Reinitializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd,
            [](PVOID)
            {
                UninitializeForCurrentThread();
                InitializeForCurrentThread();
            },
            nullptr);
        initialize = true;
    }

    for (auto hXamlHostWnd : GetXamlHostWnds())
    {
        Wh_Log(L"Reinitializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd,
            [](PVOID)
            {
                UninitializeForCurrentThread();
                InitializeForCurrentThread();
            },
            nullptr);
        initialize = true;
    }

    if (initialize)
    {
        InitializeSettingsAndTap();
    }
}
