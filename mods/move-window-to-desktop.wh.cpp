// ==WindhawkMod==
// @id              move-window-to-desktop
// @name            Move Window to Virtual Desktop
// @name:zh-CN       移动窗口到虚拟桌面
// @description     Adds a "Move to desktop" submenu to the window title bar right-click menu, supporting instant window migration and new desktop creation.
// @description:zh-CN 在窗口标题栏右键系统菜单中添加“移动到桌面”子菜单，支持窗口在桌面间快速转移与一键新建桌面迁移。
// @version         1.0.0
// @author          heartacker
// @github          https://github.com/heartacker
// @homepage        https://github.com/heartacker/move-window-to-desktop
// @include         *
// @exclude         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Move Window to Virtual Desktop (移动窗口到虚拟桌面)

在 Windows 任意常规应用程序窗口的标题栏右键（或按快捷键 `Alt + Space` 调出的系统上下文菜单）中，添加 **“移动到桌面” (Move to Desktop)** 功能菜单。

## 功能特性 (Features)
- 📌 **当前桌面状态标识**：自动检测当前活动窗口所属的虚拟桌面，并在菜单对应项上打勾显示（✔ 且置灰禁用重复点击）。
- 🚀 **即时桌面转移**：列出所有已开启的虚拟桌面（支持读取用户自定义桌面名，如“工作”、“娱乐”或系统默认的“桌面 1”），点击目标桌面即可将当前窗口无缝移入。
- ➕ **新建桌面并移入**：子菜单底部提供 **“+ 新建桌面并移动”** 选项，点击后由系统自动创建一个全新的虚拟桌面，并将当前窗口直接移至该新建桌面上。
- ⚙️ **广泛兼容**：完整适配 Windows 10 与 Windows 11（包括 21H2, 22H2, 23H2, 以及 24H2 Build 26100+ 架构）。

## 配置项 (Settings)
- **子菜单名称 (Submenu Label)**：支持自定义一级子菜单的显示文本（默认: 移动到桌面 (&M)）。
- **显示新建桌面选项 (Show New Desktop Option)**：可自由开启或关闭底部的“+ 新建桌面并移动”条目。

## 编译与安装 (Installation)
在 Windhawk 内置的 Mod 代码编辑器中：
1. 将本代码全部复制替换到编辑器。
2. 点击左侧工具栏的 **Compile Mod**（或按下快捷键 `Ctrl + B`）。
3. 编译成功后立即全局注入生效，无需重启系统。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showNewDesktopOption: true
  $name: Show "New Desktop" option
  $name:zh-CN: 显示“新建桌面并移动”选项
  $description: Add an option to create a new virtual desktop and move the window to it immediately.
  $description:zh-CN: 在子菜单末尾提供创建新虚拟桌面并立即移入当前窗口的选项。
- customSubmenuText: "移动到桌面 (&M)"
  $name: Submenu title text
  $name:zh-CN: 菜单项显示文本
  $description: Label of the submenu added into the system title bar menu.
  $description:zh-CN: 注入到窗口系统右键菜单中的子菜单标题文本。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shobjidl.h>
#include <inspectable.h>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// 用户设置结构
// ---------------------------------------------------------------------------

inline struct {
    bool showNewDesktopOption = true;
    std::wstring submenuText = L"移动到桌面 (&M)";
} g_settings;

// ---------------------------------------------------------------------------
// 自定义内联 GUID 定义（彻底避免 MinGW/Clang 链接找不到外部符号的问题）
// ---------------------------------------------------------------------------

namespace {

inline constexpr GUID My_GUID_NULL = {
    0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

inline constexpr GUID My_IID_IUnknown = {
    0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};

inline constexpr GUID My_IID_IServiceProvider = {
    0x6D5140C1, 0x7436, 0x11CE, {0x80, 0x34, 0x00, 0xAA, 0x00, 0x60, 0x09, 0xFA}
};

inline constexpr GUID My_CLSID_ImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}
};

inline constexpr GUID My_CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}
};

inline constexpr GUID My_CLSID_VirtualDesktopManager = {
    0xAA509086, 0x5CA9, 0x4C25, {0x8F, 0x95, 0x58, 0x9D, 0x3C, 0x07, 0xB4, 0x8A}
};

inline constexpr GUID My_IID_IVirtualDesktopManager = {
    0xA5CD92FF, 0x29BE, 0x454C, {0x8D, 0x04, 0xD8, 0x28, 0x79, 0xFB, 0x3F, 0x1B}
};

inline constexpr GUID My_IID_IApplicationViewCollection = {
    0x1841C6D7, 0x4F9D, 0x42C0, {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}
};

inline constexpr GUID My_IID_IVirtualDesktop_Win11_24H2 = {
    0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}
};

inline constexpr GUID My_IID_IVirtualDesktop_Legacy = {
    0x53603423, 0x6DA5, 0x4A69, {0x85, 0xC8, 0x19, 0xA2, 0x60, 0x41, 0x60, 0x9D}
};

inline constexpr GUID My_IID_IVirtualDesktop_Win10 = {
    0xFF72FFDD, 0xBE7E, 0x4360, {0x82, 0xB0, 0x29, 0x30, 0xEE, 0xC0, 0xC3, 0x22}
};

inline constexpr GUID My_IID_IVirtualDesktopManagerInternal24H2 = {
    0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}
};

inline constexpr GUID My_IID_IVirtualDesktopManagerInternalLegacy = {
    0xB2F925B9, 0x5A0F, 0x4D2E, {0x9F, 0x4D, 0x2B, 0x15, 0x07, 0x59, 0x3C, 0x10}
};

inline constexpr GUID My_IID_IVirtualDesktopManagerInternalWin10 = {
    0xF31574D6, 0xB682, 0x4CDC, {0xBD, 0x56, 0x18, 0x27, 0x86, 0x0A, 0xBE, 0xC6}
};

// ---------------------------------------------------------------------------
// 动态加载 WinString 函数，避免引入 combase.dll 显式依赖
// ---------------------------------------------------------------------------

inline PCWSTR SafeWindowsGetStringRawBuffer(HSTRING hstring) {
    using PFN_WindowsGetStringRawBuffer = PCWSTR(WINAPI*)(HSTRING, UINT32*);
    static PFN_WindowsGetStringRawBuffer pfn = []() -> PFN_WindowsGetStringRawBuffer {
        HMODULE hMod = LoadLibraryW(L"combase.dll");
        if (!hMod) hMod = LoadLibraryW(L"api-ms-win-core-winrt-string-l1-1-0.dll");
        if (hMod) {
            return (PFN_WindowsGetStringRawBuffer)GetProcAddress(hMod, "WindowsGetStringRawBuffer");
        }
        return nullptr;
    }();
    if (pfn) return pfn(hstring, nullptr);
    return nullptr;
}

inline void SafeWindowsDeleteString(HSTRING hstring) {
    using PFN_WindowsDeleteString = HRESULT(WINAPI*)(HSTRING);
    static PFN_WindowsDeleteString pfn = []() -> PFN_WindowsDeleteString {
        HMODULE hMod = LoadLibraryW(L"combase.dll");
        if (!hMod) hMod = LoadLibraryW(L"api-ms-win-core-winrt-string-l1-1-0.dll");
        if (hMod) {
            return (PFN_WindowsDeleteString)GetProcAddress(hMod, "WindowsDeleteString");
        }
        return nullptr;
    }();
    if (pfn && hstring) pfn(hstring);
}

// ---------------------------------------------------------------------------
// 虚拟桌面相关 COM 接口纯虚定义
// ---------------------------------------------------------------------------

MIDL_INTERFACE("372E1D3B-38D3-42E4-A15B-8AB2B178F513")
IApplicationView : public IInspectable {
public:
    virtual HRESULT STDMETHODCALLTYPE SetFocus(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchTo(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryInvokeBack(void* callback) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetThumbnailWindow(HWND* hwnd) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMonitor(void** monitor) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVisibility(int* visibility) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCloak(int cloakType, int unknown) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPosition(REFIID riid, void** position) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPosition(void* position) = 0;
    virtual HRESULT STDMETHODCALLTYPE InsertAfterWindow(HWND hwnd) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetExtendedFramePosition(RECT* rect) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAppUserModelId(LPWSTR* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetAppUserModelId(LPCWSTR id) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsEqualByAppUserModelId(LPCWSTR id, int* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetViewState(UINT* state) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetViewState(UINT state) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNeediness(int* neediness) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastActivationTimestamp(ULONGLONG* timestamp) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetLastActivationTimestamp(ULONGLONG timestamp) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVirtualDesktopId(GUID* guid) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVirtualDesktopId(const GUID* guid) = 0;
};

MIDL_INTERFACE("1841C6D7-4F9D-42C0-AF41-8747538F10E5")
IApplicationViewCollection : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetViews(IObjectArray** array) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetViewsByZOrder(IObjectArray** array) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetViewsByAppUserModelId(LPCWSTR id, IObjectArray** array) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetViewForHwnd(HWND hwnd, IApplicationView** view) = 0;
};

MIDL_INTERFACE("3F07F4BE-B107-441A-AF0F-39D82529072C")
IVirtualDesktop24H2 : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IApplicationView* view, int* visible) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetId(GUID* pGuid) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetName(HSTRING* pName) = 0;
};

MIDL_INTERFACE("53F5CA0B-158F-4124-900C-057158060B27")
IVirtualDesktopManagerInternal24H2 : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetCount(int* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveViewToDesktop(IApplicationView* view, IUnknown* desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE CanViewMoveDesktops(IApplicationView* view, int* canMove) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentDesktop(IUnknown** desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesktops(IObjectArray** desktops) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAdjacentDesktop(IUnknown* from, int direction, IUnknown** desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktop(IUnknown* desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktopAndMoveForegroundView(IUnknown* desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDesktop(IUnknown** desktop) = 0;
};

MIDL_INTERFACE("B2F925B9-5A0F-4D2E-9F4D-2B1507593C10")
IVirtualDesktopManagerInternalLegacy : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetCount(int* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveViewToDesktop(IApplicationView* view, IUnknown* desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE CanViewMoveDesktops(IApplicationView* view, int* canMove) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentDesktop(IUnknown** desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesktops(IObjectArray** desktops) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAdjacentDesktop(IUnknown* from, int direction, IUnknown** desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktop(IUnknown* desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDesktop(IUnknown** desktop) = 0;
};

MIDL_INTERFACE("F31574D6-B682-4CDC-BD56-1827860ABEC6")
IVirtualDesktopManagerInternalWin10 : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetCount(int* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveViewToDesktop(IApplicationView* view, IUnknown* desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE CanViewMoveDesktops(IApplicationView* view, int* canMove) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentDesktop(IUnknown** desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesktops(IObjectArray** desktops) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAdjacentDesktop(IUnknown* from, int direction, IUnknown** desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktop(IUnknown* desktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDesktop(IUnknown** desktop) = 0;
};

// 官方公开的 IVirtualDesktopManager 自行声明
MIDL_INTERFACE("a5cd92ff-29be-454c-8d04-d82879fb3f1b")
IMyVirtualDesktopManager : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentVirtualDesktop(
        HWND topLevelWindow,
        BOOL *onCurrentDesktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(
        HWND topLevelWindow,
        GUID *desktopId) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveWindowToDesktop(
        HWND topLevelWindow,
        REFGUID desktopId) = 0;
};

// 桌面项数据结构
struct DesktopItem {
    GUID id = My_GUID_NULL;
    std::wstring name;
    IUnknown* pDesktop = nullptr;
};

constexpr UINT_PTR IDM_VIRTUAL_DESKTOP_BASE = 0x9E00;
constexpr UINT_PTR IDM_VIRTUAL_DESKTOP_MAX = 0x9E80;
constexpr UINT_PTR IDM_VIRTUAL_DESKTOP_NEW = 0x9E99;

} // namespace

// ---------------------------------------------------------------------------
// 虚拟桌面包装管理器
// ---------------------------------------------------------------------------

class VirtualDesktopHelper {
public:
    static VirtualDesktopHelper& Instance() {
        static VirtualDesktopHelper helper;
        return helper;
    }

    bool Initialize() {
        if (m_initialized) return true;

        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
            return false;
        }

        IServiceProvider* pServiceProvider = nullptr;
        hr = CoCreateInstance(My_CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
                              My_IID_IServiceProvider, (void**)&pServiceProvider);
        if (FAILED(hr) || !pServiceProvider) {
            return false;
        }

        // 尝试获取 ManagerInternal (按照 24H2 -> Legacy Win11 -> Win10 顺序尝试)
        hr = pServiceProvider->QueryService(My_CLSID_VirtualDesktopManagerInternal,
                                            My_IID_IVirtualDesktopManagerInternal24H2,
                                            (void**)&m_pManager24H2);
        if (SUCCEEDED(hr)) {
            m_desktopIID = My_IID_IVirtualDesktop_Win11_24H2;
        } else {
            hr = pServiceProvider->QueryService(My_CLSID_VirtualDesktopManagerInternal,
                                                My_IID_IVirtualDesktopManagerInternalLegacy,
                                                (void**)&m_pManagerLegacy);
            if (SUCCEEDED(hr)) {
                m_desktopIID = My_IID_IVirtualDesktop_Legacy;
            } else {
                hr = pServiceProvider->QueryService(My_CLSID_VirtualDesktopManagerInternal,
                                                    My_IID_IVirtualDesktopManagerInternalWin10,
                                                    (void**)&m_pManagerWin10);
                if (SUCCEEDED(hr)) {
                    m_desktopIID = My_IID_IVirtualDesktop_Win10;
                }
            }
        }

        // 获取 ApplicationViewCollection
        pServiceProvider->QueryService(My_IID_IApplicationViewCollection,
                                       My_IID_IApplicationViewCollection,
                                       (void**)&m_pViewCollection);

        pServiceProvider->Release();

        // 获取公开接口作为保底
        CoCreateInstance(My_CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                         My_IID_IVirtualDesktopManager, (void**)&m_pPublicManager);

        m_initialized = (m_pManager24H2 != nullptr || m_pManagerLegacy != nullptr || m_pManagerWin10 != nullptr);
        return m_initialized;
    }

    std::vector<DesktopItem> GetDesktops() {
        std::vector<DesktopItem> list;
        if (!Initialize()) return list;

        IObjectArray* pArray = nullptr;
        HRESULT hr = E_FAIL;
        if (m_pManager24H2) {
            hr = m_pManager24H2->GetDesktops(&pArray);
        } else if (m_pManagerLegacy) {
            hr = m_pManagerLegacy->GetDesktops(&pArray);
        } else if (m_pManagerWin10) {
            hr = m_pManagerWin10->GetDesktops(&pArray);
        }

        if (FAILED(hr) || !pArray) return list;

        UINT count = 0;
        pArray->GetCount(&count);

        for (UINT i = 0; i < count; i++) {
            IUnknown* pUnk = nullptr;
            if (SUCCEEDED(pArray->GetAt(i, My_IID_IUnknown, (void**)&pUnk)) && pUnk) {
                DesktopItem item;
                item.pDesktop = pUnk;
                item.name = L"桌面 " + std::to_wstring(i + 1);

                IVirtualDesktop24H2* pVd24H2 = nullptr;
                if (SUCCEEDED(pUnk->QueryInterface(My_IID_IVirtualDesktop_Win11_24H2, (void**)&pVd24H2))) {
                    pVd24H2->GetId(&item.id);
                    HSTRING hName = nullptr;
                    if (SUCCEEDED(pVd24H2->GetName(&hName)) && hName) {
                        PCWSTR str = SafeWindowsGetStringRawBuffer(hName);
                        if (str && wcslen(str) > 0) {
                            item.name = str;
                        }
                        SafeWindowsDeleteString(hName);
                    }
                    pVd24H2->Release();
                }
                list.push_back(item);
            }
        }

        pArray->Release();
        return list;
    }

    bool GetWindowDesktopId(HWND hWnd, GUID* pGuid) {
        if (!m_pPublicManager && !Initialize()) return false;
        if (m_pPublicManager && SUCCEEDED(m_pPublicManager->GetWindowDesktopId(hWnd, pGuid))) {
            return true;
        }
        return false;
    }

    bool MoveWindow(HWND hWnd, IUnknown* pTargetDesktop, const GUID& desktopId) {
        if (!Initialize()) return false;

        // 1. 优先尝试官方公开接口
        if (m_pPublicManager) {
            if (SUCCEEDED(m_pPublicManager->MoveWindowToDesktop(hWnd, desktopId))) {
                return true;
            }
        }

        // 2. 尝试使用 Internal MoveViewToDesktop
        if (m_pViewCollection && pTargetDesktop) {
            IApplicationView* pView = nullptr;
            if (SUCCEEDED(m_pViewCollection->GetViewForHwnd(hWnd, &pView)) && pView) {
                HRESULT hr = E_FAIL;
                if (m_pManager24H2) {
                    hr = m_pManager24H2->MoveViewToDesktop(pView, pTargetDesktop);
                } else if (m_pManagerLegacy) {
                    hr = m_pManagerLegacy->MoveViewToDesktop(pView, pTargetDesktop);
                } else if (m_pManagerWin10) {
                    hr = m_pManagerWin10->MoveViewToDesktop(pView, pTargetDesktop);
                }
                pView->Release();
                if (SUCCEEDED(hr)) return true;
            }
        }

        return false;
    }

    IUnknown* CreateNewDesktop() {
        if (!Initialize()) return nullptr;
        IUnknown* pNew = nullptr;
        if (m_pManager24H2) {
            m_pManager24H2->CreateDesktop(&pNew);
        } else if (m_pManagerLegacy) {
            m_pManagerLegacy->CreateDesktop(&pNew);
        } else if (m_pManagerWin10) {
            m_pManagerWin10->CreateDesktop(&pNew);
        }
        return pNew;
    }

    void ReleaseDesktops(std::vector<DesktopItem>& list) {
        for (auto& it : list) {
            if (it.pDesktop) {
                it.pDesktop->Release();
                it.pDesktop = nullptr;
            }
        }
    }

    void Cleanup() {
        if (m_pManager24H2) {
            m_pManager24H2->Release();
            m_pManager24H2 = nullptr;
        }
        if (m_pManagerLegacy) {
            m_pManagerLegacy->Release();
            m_pManagerLegacy = nullptr;
        }
        if (m_pManagerWin10) {
            m_pManagerWin10->Release();
            m_pManagerWin10 = nullptr;
        }
        if (m_pViewCollection) {
            m_pViewCollection->Release();
            m_pViewCollection = nullptr;
        }
        if (m_pPublicManager) {
            m_pPublicManager->Release();
            m_pPublicManager = nullptr;
        }
        m_initialized = false;
    }

private:
    VirtualDesktopHelper() = default;
    ~VirtualDesktopHelper() {
        Cleanup();
    }

    bool m_initialized = false;
    GUID m_desktopIID = My_GUID_NULL;
    IVirtualDesktopManagerInternal24H2* m_pManager24H2 = nullptr;
    IVirtualDesktopManagerInternalLegacy* m_pManagerLegacy = nullptr;
    IVirtualDesktopManagerInternalWin10* m_pManagerWin10 = nullptr;
    IApplicationViewCollection* m_pViewCollection = nullptr;
    IMyVirtualDesktopManager* m_pPublicManager = nullptr;
};

// ---------------------------------------------------------------------------
// 菜单逻辑与消息捕获
// ---------------------------------------------------------------------------

namespace {

HWND g_currentMenuWnd = nullptr;
std::vector<DesktopItem> g_cachedDesktops;

void CleanupCachedDesktops() {
    VirtualDesktopHelper::Instance().ReleaseDesktops(g_cachedDesktops);
    g_cachedDesktops.clear();
}

void PopulateMoveToDesktopMenu(HWND hWnd, HMENU hSysMenu) {
    if (!hSysMenu || !IsWindow(hWnd)) return;

    int count = GetMenuItemCount(hSysMenu);
    for (int i = count - 1; i >= 0; i--) {
        MENUITEMINFOW mii = { sizeof(mii) };
        mii.fMask = MIIM_ID | MIIM_SUBMENU;
        if (GetMenuItemInfoW(hSysMenu, i, TRUE, &mii)) {
            if (mii.wID == IDM_VIRTUAL_DESKTOP_BASE) {
                RemoveMenu(hSysMenu, i, MF_BYPOSITION);
            }
        }
    }

    CleanupCachedDesktops();
    g_cachedDesktops = VirtualDesktopHelper::Instance().GetDesktops();
    g_currentMenuWnd = hWnd;

    HMENU hSubMenu = CreatePopupMenu();
    if (!hSubMenu) return;

    GUID currentDesktopId = My_GUID_NULL;
    bool hasCurrentId = VirtualDesktopHelper::Instance().GetWindowDesktopId(hWnd, &currentDesktopId);

    for (size_t i = 0; i < g_cachedDesktops.size() && i < (IDM_VIRTUAL_DESKTOP_MAX - IDM_VIRTUAL_DESKTOP_BASE); i++) {
        UINT_PTR cmdId = IDM_VIRTUAL_DESKTOP_BASE + 1 + i;
        bool isCurrent = false;

        if (hasCurrentId && IsEqualGUID(g_cachedDesktops[i].id, currentDesktopId)) {
            isCurrent = true;
        }

        UINT flags = MF_STRING;
        if (isCurrent) {
            flags |= MF_CHECKED | MF_GRAYED;
        }

        AppendMenuW(hSubMenu, flags, cmdId, g_cachedDesktops[i].name.c_str());
    }

    if (g_settings.showNewDesktopOption) {
        AppendMenuW(hSubMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hSubMenu, MF_STRING, IDM_VIRTUAL_DESKTOP_NEW, L"+ 新建桌面并移动");
    }

    AppendMenuW(hSysMenu, MF_POPUP, (UINT_PTR)hSubMenu, g_settings.submenuText.c_str());
}

void HandleMenuCommand(HWND hWnd, WPARAM wParam) {
    UINT_PTR cmd = wParam & 0xFFFF;

    if (cmd == IDM_VIRTUAL_DESKTOP_NEW) {
        Wh_Log(L"New desktop requested for window %p", hWnd);
        IUnknown* pNew = VirtualDesktopHelper::Instance().CreateNewDesktop();
        if (pNew) {
            GUID newId = My_GUID_NULL;
            IVirtualDesktop24H2* pVd = nullptr;
            if (SUCCEEDED(pNew->QueryInterface(My_IID_IVirtualDesktop_Win11_24H2, (void**)&pVd))) {
                pVd->GetId(&newId);
                pVd->Release();
            }
            VirtualDesktopHelper::Instance().MoveWindow(hWnd, pNew, newId);
            pNew->Release();
        }
        CleanupCachedDesktops();
    } else if (cmd > IDM_VIRTUAL_DESKTOP_BASE && cmd <= IDM_VIRTUAL_DESKTOP_MAX) {
        size_t index = cmd - (IDM_VIRTUAL_DESKTOP_BASE + 1);
        if (index < g_cachedDesktops.size()) {
            Wh_Log(L"Move window %p to desktop %zu (%ls)", hWnd, index, g_cachedDesktops[index].name.c_str());
            VirtualDesktopHelper::Instance().MoveWindow(hWnd, g_cachedDesktops[index].pDesktop, g_cachedDesktops[index].id);
        }
        CleanupCachedDesktops();
    }
}

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;
BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT* prcRect) {
    if (hWnd) {
        PopulateMoveToDesktopMenu(hWnd, hMenu);
    }
    return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;
BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) {
    if (hWnd) {
        PopulateMoveToDesktopMenu(hWnd, hMenu);
    }
    return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
}

using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_Original;
LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITMENUPOPUP) {
        HMENU hMenu = (HMENU)wParam;
        if (HIWORD(lParam) != 0) {
            PopulateMoveToDesktopMenu(hWnd, hMenu);
        }
    } else if (uMsg == WM_SYSCOMMAND) {
        WPARAM cmd = wParam & 0xFFFF;
        if (cmd >= IDM_VIRTUAL_DESKTOP_BASE && cmd <= IDM_VIRTUAL_DESKTOP_NEW) {
            HandleMenuCommand(hWnd, wParam);
            return 0;
        }
    }
    return DefWindowProcW_Original(hWnd, uMsg, wParam, lParam);
}

using DefWindowProcA_t = decltype(&DefWindowProcA);
DefWindowProcA_t DefWindowProcA_Original;
LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITMENUPOPUP) {
        HMENU hMenu = (HMENU)wParam;
        if (HIWORD(lParam) != 0) {
            PopulateMoveToDesktopMenu(hWnd, hMenu);
        }
    } else if (uMsg == WM_SYSCOMMAND) {
        WPARAM cmd = wParam & 0xFFFF;
        if (cmd >= IDM_VIRTUAL_DESKTOP_BASE && cmd <= IDM_VIRTUAL_DESKTOP_NEW) {
            HandleMenuCommand(hWnd, wParam);
            return 0;
        }
    }
    return DefWindowProcA_Original(hWnd, uMsg, wParam, lParam);
}

} // namespace

// ---------------------------------------------------------------------------
// 设置加载与生命周期管理
// ---------------------------------------------------------------------------

void LoadSettings() {
    g_settings.showNewDesktopOption = Wh_GetIntSetting(L"showNewDesktopOption");
    PCWSTR label = Wh_GetStringSetting(L"customSubmenuText");
    if (label && wcslen(label) > 0) {
        g_settings.submenuText = label;
    } else {
        g_settings.submenuText = L"移动到桌面 (&M)";
    }
    Wh_FreeStringSetting(label);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Move Window to Virtual Desktop mod initializing...");

    LoadSettings();

    Wh_SetFunctionHook((void*)TrackPopupMenu, (void*)TrackPopupMenu_Hook, (void**)&TrackPopupMenu_Original);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&TrackPopupMenuEx_Original);
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)DefWindowProcA, (void*)DefWindowProcA_Hook, (void**)&DefWindowProcA_Original);

    Wh_Log(L"Move Window to Virtual Desktop mod initialized successfully.");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Move Window to Virtual Desktop mod uninitializing...");

    CleanupCachedDesktops();
    VirtualDesktopHelper::Instance().Cleanup();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
}
