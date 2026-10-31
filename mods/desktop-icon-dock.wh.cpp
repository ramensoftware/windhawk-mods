// ==WindhawkMod==
// @id              desktop-icon-dock
// @name            Desktop Icon Dock
// @name:zh-CN      桌面图标 Dock
// @description     Turns desktop icons into a centered macOS-style dock with hover magnification and an optional glass panel
// @description:zh-CN 将桌面图标变成居中的 macOS 风格 Dock，支持悬停放大，可选液态玻璃面板
// @version         1.0.11
// @author          yizqq0721
// @github          https://github.com/yizqq0721
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lole32 -loleaut32 -luuid -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icon Dock

Turns desktop icons into a centered macOS-style dock. A fully transparent
background is used by default, showing only the icons; an optional Liquid Glass
panel is available.

![Desktop Icon Dock preview](https://raw.githubusercontent.com/yizqq0721/desktop-icon-dock/main/images/dock-preview-transparent.jpg)

*Example: desktop icons arranged as a centered macOS-style dock; the hovered
icon is magnified with its label shown.*

## Features

- **macOS-style hover magnification**: icons grow smoothly as the pointer
  approaches, with cosine falloff and neighboring icons spreading aside.
- **Transparent background (default)**: only the icons and a soft drop shadow
  are drawn, so the dock blends into the wallpaper; transparent areas of the
  dock window are click-through, so desktop right-click and clicks keep
  working.
- **Optional Liquid Glass panel**: enabled by turning off "Transparent
  background". Blurred wallpaper backdrop, milky glass tint, specular rim and
  a hover glow. The backdrop is sampled from the screen above the dock and
  works with Wallpaper Engine and other animated wallpapers.
- **Name labels**: hovering an icon shows a name pill above it; in transparent
  mode the pill stays at a fixed position instead of jumping while icons
  scale.
- **Native interaction**: left click opens an item; right click opens that
  item's native Explorer context menu (open, send to, create shortcut,
  properties, delete, ...).
- **Automatic desktop sync**: items added, removed, renamed or moved on the
  desktop appear in the dock automatically through shell change notifications,
  ordered by their desktop grid position.
- **Original desktop icons hidden in memory**: while the dock is active, the
  desktop's list view is hidden; no Explorer setting or registry value is
  changed, and the icons are shown again when the mod is unloaded.
- **Icon cache with cleanup**: icons are cached per item path, evicted when an
  item disappears, and refreshed when the shell reports an icon update.

## How it works

The dock does not rearrange the system desktop icons. It reads the desktop
folder (Shell's IFolderView), draws the items itself, and hides the original
icons by hiding the desktop's list view window in memory.

- **Sync**: the dock listens for shell change notifications for the desktop
  folders, so most changes appear immediately. A slow 30-second check covers
  changes that do not raise a notification, such as a new sort order.
- **Ordering**: items are sorted by desktop grid position (top to bottom, then
  left to right), so the dock order matches the desktop layout and a new
  shortcut appears at its corresponding position.
- **Layout**: centered horizontally at the bottom of the primary monitor,
  above the taskbar; the panel widens as items are added, and icons are
  vertically centered at rest.
- **Performance**: in transparent mode the pointer position is polled every
  50 ms so hover magnification also works in the gaps between icons; idle CPU
  usage is negligible.

## Settings

| Setting | Default | Description |
| --- | --- | --- |
| Icon size | 48 | Base size in pixels at 100% DPI, scaled with system DPI (about 72 px at 150%) |
| Icon spacing | 10 | Pixel gap between icons at rest |
| Maximum scale (%) | 175 | Largest magnification of the hovered icon |
| Effect radius | 160 | Distance in pixels over which magnification fades out from the cursor |
| Neighbor spread (%) | 60 | How much neighboring icons move aside while magnifying |
| Smoothing speed | 16 | Higher is snappier, lower is softer |
| Bottom offset | 10 | Gap between the dock and the taskbar, in pixels at 100% DPI |
| Transparent background | On | Show only the icons; turn off to use the Liquid Glass panel |
| Glass opacity (%) | 24 | Glass mode only: strength of the milky tint |
| Glass blur | 20 | Glass mode only: backdrop blur strength |
| Show labels | On | Show a name pill for the hovered item |
| Hide original desktop icons | On | Hide the system desktop icons while the dock is active by hiding the desktop list view in memory; they are shown again when the mod is unloaded |
| Excluded items | empty | Comma-separated display names that should not appear in the dock |
| Backdrop refresh interval (s) | 20 | How often the blurred backdrop (used by the glass panel and the label pill) is re-sampled; 0 disables refresh |

## Compatibility and limitations

- Windows 10 / 11 on x86-64 only, injected into explorer.exe by Windhawk.
- Primary monitor only; the dock is anchored to the bottom of the primary
  monitor, above the taskbar.
- The dock is a layered window placed below normal windows; full-screen apps
  and other windows cover it, which is expected.
- In transparent mode, fully transparent pixels are click-through; the icons
  themselves remain clickable.

## Troubleshooting

- **Dock missing after an Explorer restart**: the mod reloads together with
  explorer.exe; the dock comes back within a second.
- **A new shortcut does not appear immediately**: desktop changes are picked
  up through shell notifications, so the item should appear within a second;
  the 30-second fallback covers unusual changes such as a new sort order.
- **Settings changes seem to have no effect**: saving settings reloads and
  rebuilds the dock automatically; no Explorer restart is needed.
- **Add an item**: put a shortcut or file on the desktop, or press Win+R and
  enter `shell:Desktop` to open the desktop folder; the dock picks it up
  within a second.
- **Hide a specific item**: put its display name in "Excluded items"
  (comma-separated).

## 中文说明

本模组把桌面图标变成居中的 macOS 风格 Dock，默认使用全透明背景；关闭“透明背景”后会显示液态玻璃面板。

- 图标支持悬停放大、相邻图标让位和名称标签。
- Dock 通过 shell 变更通知自动同步桌面上新增、删除、重命名和移动的项目，并按桌面网格位置排序。
- 原始桌面图标只在 Dock 运行期间通过隐藏桌面列表视图在内存中隐藏，不会修改 Explorer 的任何设置或注册表项；卸载模组后图标自动恢复。
- 左键打开项目，右键打开与桌面一致的资源管理器上下文菜单。
- 仅支持 x86-64 的 Windows 10/11，且只在主显示器底部显示。

各设置项的说明见上方英文表格。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- IconSize: 48
  $name: Icon size
  $name:zh-CN: 图标大小
  $description: Base icon size in pixels (at 100% DPI scaling)
  $description:zh-CN: 图标的基础像素大小（100% DPI 缩放时）
- Spacing: 10
  $name: Icon spacing
  $name:zh-CN: 图标间距
  $description: Gap between icons at rest, in pixels
  $description:zh-CN: 静止状态下图标之间的像素间距
- MaxScale: 175
  $name: Maximum scale (%)
  $name:zh-CN: 最大放大比例（%）
  $description: How large the hovered icon grows (175 = 1.75x)
  $description:zh-CN: 鼠标悬停时图标的最大放大倍数（175 = 1.75 倍）
- EffectRadius: 160
  $name: Effect radius
  $name:zh-CN: 效果半径
  $description: Distance from the cursor where magnification fades out, in pixels
  $description:zh-CN: 放大效果从光标向外衰减的距离（像素）
- SpacingFactor: 60
  $name: Neighbor spread (%)
  $name:zh-CN: 相邻扩散（%）
  $description: How much neighboring icons move aside while magnifying
  $description:zh-CN: 放大时相邻图标向外让位的程度
- Smoothing: 16
  $name: Smoothing speed
  $name:zh-CN: 平滑速度
  $description: Higher is snappier, lower is softer
  $description:zh-CN: 数值越大越跟手，越小越柔和
- BottomOffset: 10
  $name: Bottom offset
  $name:zh-CN: 底部偏移
  $description: Gap between the dock and the taskbar, in pixels
  $description:zh-CN: Dock 与任务栏之间的间距（像素）
- TransparentBackground: true
  $name: Transparent background
  $name:zh-CN: 透明背景
  $description: Show only the icons and skip the glass panel
  $description:zh-CN: 只显示图标，不使用玻璃背景
- GlassOpacity: 24
  $name: Glass opacity (%)
  $name:zh-CN: 玻璃不透明度（%）
  $description: Glass mode only (Transparent background off) - strength of the milky tint
  $description:zh-CN: 仅玻璃模式（关闭透明背景）时生效：乳白色调的强度
- GlassBlur: 20
  $name: Glass blur
  $name:zh-CN: 玻璃模糊
  $description: Glass mode only (Transparent background off) - backdrop blur strength
  $description:zh-CN: 仅玻璃模式（关闭透明背景）时生效：背景模糊强度
- ShowLabels: true
  $name: Show labels
  $name:zh-CN: 显示名称
  $description: Show the name of the hovered item in a floating glass pill
  $description:zh-CN: 悬停时在浮动玻璃标签中显示名称
- HideDesktopIcons: true
  $name: Hide original desktop icons
  $name:zh-CN: 隐藏原始桌面图标
  $description: Hide the original desktop icons while the dock is active by hiding the desktop list view in memory; restored on unload
  $description:zh-CN: 启用 Dock 时在内存中隐藏桌面列表视图，不修改系统设置；卸载时恢复
- ExcludeItems: ""
  $name: Excluded items
  $name:zh-CN: 排除项目
  $description: Comma separated display names to keep out of the dock
  $description:zh-CN: 用逗号分隔的显示名称，匹配的项目不会出现在 Dock 中
- BackdropRefreshSeconds: 20
  $name: Backdrop refresh interval (s)
  $name:zh-CN: 背景刷新间隔（秒）
  $description: How often the blurred backdrop used by the glass panel and the label pill is re-sampled; 0 disables refresh
  $description:zh-CN: 多久重新采样一次玻璃面板和名称标签所使用的模糊背景；0 表示不自动刷新
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <windhawk_utils.h>
#include <oleauto.h>
#include <shlobj.h>
#include <shlguid.h>
#include <exdisp.h>
#include <shellapi.h>
#include <gdiplus.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace Gdiplus;

namespace {

constexpr wchar_t kWindowClass[] = L"WindhawkDesktopIconDock";
constexpr UINT kMsgReloadSettings = WM_APP + 1;
constexpr UINT kMsgShellNotify = WM_APP + 2;
constexpr UINT kMsgQuit = WM_APP + 3;
constexpr UINT_PTR kTimerAnimation = 1;
constexpr UINT_PTR kTimerMaintenance = 2;
constexpr UINT_PTR kTimerRefresh = 3;
constexpr UINT_PTR kTimerHoverPoll = 5;
constexpr UINT_PTR kTimerShellNotify = 6;
constexpr int kHoverPollMs = 50;
constexpr int kIconRequestPx = 256;
constexpr int kFullRebuildSeconds = 30;
constexpr int kNotifyRebuildDelayMs = 300;
constexpr double kPi = 3.14159265358979323846;

struct Settings {
    int iconSize = 48;
    int spacing = 10;
    int maxScalePercent = 175;
    int effectRadius = 160;
    int spacingFactor = 60;
    int smoothing = 16;
    int bottomOffset = 10;
    int transparentBackground = 1;
    int glassOpacity = 24;
    int glassBlur = 20;
    int showLabels = 1;
    int hideDesktopIcons = 1;
    int backdropRefreshSeconds = 20;
    std::wstring excludeItems;
};

Settings g_settings;

std::atomic<bool> g_stopRequested{false};
std::atomic<HWND> g_hwnd{nullptr};
std::atomic<HANDLE> g_thread{nullptr};
HINSTANCE g_instance = nullptr;

HDC g_screenDc = nullptr;
double g_dpiScale = 1.0;

struct FrameBuffer {
    int width = 0;
    int height = 0;
    HDC memDc = nullptr;
    HBITMAP dib = nullptr;
    HBITMAP oldBitmap = nullptr;
    std::unique_ptr<Bitmap> bitmap;
};

FrameBuffer g_frame;

std::unique_ptr<Bitmap> g_glowTexture;
std::unique_ptr<Bitmap> g_shadowTexture;
std::unique_ptr<Bitmap> g_noiseTexture;
std::unique_ptr<Bitmap> g_backdrop;
bool g_backdropValid = false;
std::chrono::steady_clock::time_point g_backdropCaptureTime;

struct LayoutMetrics {
    int iconPx = 48;
    int spacingPx = 10;
    double maxScale = 1.75;
    double spacingFactor = 0.6;
    int panelPadX = 14;
    int panelPadY = 8;
    int panelHeight = 0;
    int panelHeightRest = 0;
    int panelHeightMax = 0;
    int panelTop = 0;
    int panelBottom = 0;
    int iconBottom = 0;
    int panelBaseWidth = 0;
    int panelMaxWidth = 0;
    int windowWidth = 0;
    int windowHeight = 0;
    int glowMargin = 0;
    int labelFontPx = 20;
    int labelPillHeight = 30;
    int labelGap = 12;
    int labelAreaHeight = 0;
};

LayoutMetrics g_layout;

struct DockItem {
    std::wstring name;
    std::wstring key;
    std::shared_ptr<void> pidlOwner;
    PIDLIST_ABSOLUTE pidl = nullptr;
    std::shared_ptr<Bitmap> icon;
    double baseCenterX = 0.0;
    double centerX = 0.0;
    double scale = 1.0;
    double labelAlpha = 0.0;
};

std::vector<DockItem> g_items;
std::map<std::wstring, std::shared_ptr<Bitmap>> g_iconCache;
std::wstring g_lastSignature;

int g_windowX = 0;
int g_windowY = 0;
int g_windowWidth = 0;
int g_windowHeight = 0;
RECT g_lastWorkArea{};

double g_panelWidth = 0.0;
double g_intensity = 0.0;
double g_lastMouseX = 0.0;
bool g_mouseXValid = false;
int g_hoverIndex = -1;
bool g_animationTimerRunning = false;
bool g_hoverPollRunning = false;
bool g_trackingMouse = false;
std::chrono::steady_clock::time_point g_lastTickTime;
std::chrono::steady_clock::time_point g_lastItemCheckTime;
DWORD g_lastLaunchTick = 0;

bool g_dockShown = false;
bool g_iconsHiddenByUs = false;
bool g_menuOpen = false;
bool g_gdiplusStarted = false;
ULONG_PTR g_gdiplusToken = 0;
bool g_classRegistered = false;
HWND g_desktopListView = nullptr;
ULONG g_changeNotifyRegister = 0;
bool g_notifyForceRebuild = false;

template <typename T>
class ComPtr {
  public:
    ComPtr() = default;
    ~ComPtr() { Reset(); }
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ComPtr(ComPtr&& other) noexcept : p_(other.p_) { other.p_ = nullptr; }
    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) {
            Reset();
            p_ = other.p_;
            other.p_ = nullptr;
        }
        return *this;
    }
    T* Get() const { return p_; }
    T** put() {
        Reset();
        return &p_;
    }
    T* operator->() const { return p_; }
    explicit operator bool() const { return p_ != nullptr; }
    void Reset() {
        if (p_) {
            p_->Release();
            p_ = nullptr;
        }
    }

  private:
    T* p_ = nullptr;
};

ComPtr<IContextMenu2> g_activeMenu2;
ComPtr<IContextMenu3> g_activeMenu3;
std::unique_ptr<FontFamily> g_labelFontFamily;
std::unique_ptr<Font> g_labelFont;

int ClampInt(int value, int lo, int hi) {
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

double ClampDouble(double value, double lo, double hi) {
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

std::wstring ToLowerString(const std::wstring& value) {
    std::wstring out = value;
    for (auto& c : out) c = (wchar_t)towlower(c);
    return out;
}

std::wstring TrimString(const std::wstring& value) {
    size_t start = value.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return L"";
    size_t end = value.find_last_not_of(L" \t\r\n");
    return value.substr(start, end - start + 1);
}

std::vector<std::wstring> ParseExcludeList(const std::wstring& value) {
    std::vector<std::wstring> out;
    size_t start = 0;
    while (start <= value.size()) {
        size_t comma = value.find(L',', start);
        std::wstring part =
            value.substr(start, comma == std::wstring::npos ? std::wstring::npos : comma - start);
        part = ToLowerString(TrimString(part));
        if (!part.empty()) out.push_back(part);
        if (comma == std::wstring::npos) break;
        start = comma + 1;
    }
    return out;
}

double Falloff(double distance) {
    double radius = std::max(1.0, g_settings.effectRadius * g_dpiScale);
    if (distance >= radius) return 0.0;
    double t = distance / radius;
    return (std::cos(t * kPi) + 1.0) * 0.5;
}

void AddRoundedRect(GraphicsPath& path, const RectF& rect, float radius) {
    if (radius < 0.5f) {
        path.AddRectangle(rect);
        return;
    }
    float diameter = radius * 2.0f;
    if (diameter > rect.Width) diameter = rect.Width;
    if (diameter > rect.Height) diameter = rect.Height;
    if (diameter < 2.0f) {
        path.AddRectangle(rect);
        return;
    }
    path.AddArc(rect.X, rect.Y, diameter, diameter, 180, 90);
    path.AddArc(rect.X + rect.Width - diameter, rect.Y, diameter, diameter, 270, 90);
    path.AddArc(rect.X + rect.Width - diameter, rect.Y + rect.Height - diameter,
                diameter, diameter, 0, 90);
    path.AddArc(rect.X, rect.Y + rect.Height - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();
}

void LoadSettings() {
    Settings s;
    s.iconSize = ClampInt(Wh_GetIntSetting(L"IconSize"), 24, 128);
    s.spacing = ClampInt(Wh_GetIntSetting(L"Spacing"), 0, 64);
    s.maxScalePercent = ClampInt(Wh_GetIntSetting(L"MaxScale"), 100, 300);
    s.effectRadius = ClampInt(Wh_GetIntSetting(L"EffectRadius"), 20, 900);
    s.spacingFactor = ClampInt(Wh_GetIntSetting(L"SpacingFactor"), 0, 100);
    s.smoothing = ClampInt(Wh_GetIntSetting(L"Smoothing"), 1, 60);
    s.bottomOffset = ClampInt(Wh_GetIntSetting(L"BottomOffset"), 0, 300);
    s.transparentBackground = Wh_GetIntSetting(L"TransparentBackground") ? 1 : 0;
    s.glassOpacity = ClampInt(Wh_GetIntSetting(L"GlassOpacity"), 0, 100);
    s.glassBlur = ClampInt(Wh_GetIntSetting(L"GlassBlur"), 0, 80);
    s.showLabels = Wh_GetIntSetting(L"ShowLabels") ? 1 : 0;
    s.hideDesktopIcons = Wh_GetIntSetting(L"HideDesktopIcons") ? 1 : 0;
    s.backdropRefreshSeconds =
        ClampInt(Wh_GetIntSetting(L"BackdropRefreshSeconds"), 0, 3600);
    s.excludeItems = WindhawkUtils::StringSetting::make(L"ExcludeItems").get();
    g_settings = std::move(s);
}

std::wstring GetShellItemName(IShellItem* item, SIGDN kind) {
    if (!item) return L"";
    PWSTR value = nullptr;
    std::wstring out;
    if (SUCCEEDED(item->GetDisplayName(kind, &value)) && value) {
        out = value;
        CoTaskMemFree(value);
    }
    return out;
}

bool GetDesktopView(ComPtr<IFolderView>& folderView, HWND* defViewOut) {
    if (defViewOut) *defViewOut = nullptr;
    ComPtr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(shellWindows.put())))) {
        return false;
    }
    VARIANT loc;
    VARIANT root;
    VariantInit(&loc);
    VariantInit(&root);
    long shellHwnd = 0;
    ComPtr<IDispatch> dispatch;
    HRESULT hr = shellWindows->FindWindowSW(&loc, &root, SWC_DESKTOP, &shellHwnd,
                                           SWFO_NEEDDISPATCH, dispatch.put());
    if (FAILED(hr) || !dispatch) return false;

    ComPtr<IServiceProvider> provider;
    if (FAILED(dispatch->QueryInterface(IID_PPV_ARGS(provider.put())))) return false;

    ComPtr<IShellBrowser> browser;
    if (FAILED(provider->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(browser.put())))) {
        return false;
    }

    ComPtr<IShellView> view;
    if (FAILED(browser->QueryActiveShellView(view.put()))) return false;

    if (defViewOut) view->GetWindow(defViewOut);

    if (FAILED(view->QueryInterface(IID_PPV_ARGS(folderView.put())))) return false;
    return true;
}

struct EnumeratedItem {
    std::wstring name;
    std::wstring key;
    std::shared_ptr<void> pidlOwner;
    PIDLIST_ABSOLUTE pidl = nullptr;
    POINT position{};
    bool isFilesystem = false;
    ComPtr<IShellItem> shellItem;
};

bool EnumerateDesktopItems(std::vector<EnumeratedItem>& out) {
    out.clear();
    ComPtr<IFolderView> folderView;
    if (!GetDesktopView(folderView, nullptr)) return false;

    ComPtr<IShellFolder> folder;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(folder.put())))) return false;

    PIDLIST_ABSOLUTE root = nullptr;
    ComPtr<IPersistFolder2> persist;
    if (SUCCEEDED(folder->QueryInterface(IID_PPV_ARGS(persist.put())))) {
        persist->GetCurFolder(&root);
    }

    int count = 0;
    if (FAILED(folderView->ItemCount(SVGIO_ALLVIEW, &count))) {
        if (root) CoTaskMemFree(root);
        return false;
    }

    for (int i = 0; i < count; ++i) {
        if (g_stopRequested) break;
        PITEMID_CHILD child = nullptr;
        if (FAILED(folderView->Item(i, &child)) || !child) continue;

        PIDLIST_ABSOLUTE pidl = root ? ILCombine(root, child) : nullptr;
        POINT position{};
        folderView->GetItemPosition(child, &position);

        ComPtr<IShellItem> item;
        if (pidl) {
            SHCreateItemFromIDList(pidl, IID_PPV_ARGS(item.put()));
        }

        std::wstring name = GetShellItemName(item.Get(), SIGDN_NORMALDISPLAY);
        std::wstring key = GetShellItemName(item.Get(), SIGDN_DESKTOPABSOLUTEPARSING);
        if (key.empty()) key = name + L"|" + std::to_wstring(i);
        if (name.empty()) name = key;

        EnumeratedItem entry;
        entry.name = std::move(name);
        entry.key = std::move(key);
        entry.pidl = pidl;
        if (pidl) {
            entry.pidlOwner = std::shared_ptr<void>(
                pidl, [](void* p) { if (p) ILFree((PIDLIST_ABSOLUTE)p); });
        }
        entry.position = position;
        if (item) {
            SFGAOF attributes = 0;
            if (SUCCEEDED(item->GetAttributes(SFGAO_FILESYSTEM, &attributes))) {
                entry.isFilesystem = (attributes & SFGAO_FILESYSTEM) != 0;
            }
        }
        entry.shellItem = std::move(item);
        out.push_back(std::move(entry));

        CoTaskMemFree(child);
    }

    if (root) CoTaskMemFree(root);
    return !g_stopRequested;
}

std::shared_ptr<Bitmap> CopyToOwnedBitmap(Bitmap* source) {
    if (!source) return nullptr;
    INT width = source->GetWidth();
    INT height = source->GetHeight();
    if (width <= 0 || height <= 0) return nullptr;
    auto owned = std::make_unique<Bitmap>(width, height, PixelFormat32bppPARGB);
    if (owned->GetLastStatus() != Ok) return nullptr;
    {
        Graphics graphics(owned.get());
        graphics.SetCompositingMode(CompositingModeSourceCopy);
        graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
        graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
        graphics.DrawImage(source, 0, 0, width, height);
    }
    if (owned->GetLastStatus() != Ok) return nullptr;
    return std::shared_ptr<Bitmap>(owned.release());
}

std::shared_ptr<Bitmap> CopyHBitmapToBitmap(HBITMAP hbmp) {
    BITMAP bm{};
    if (!GetObjectW(hbmp, sizeof(bm), &bm) || bm.bmWidth <= 0 || bm.bmHeight <= 0) {
        return nullptr;
    }
    std::unique_ptr<Bitmap> owned;
    if (bm.bmBits && bm.bmBitsPixel == 32 && bm.bmWidthBytes >= bm.bmWidth * 4) {
        // Bitmaps returned by IShellItemImageFactory are bottom-up DIBs, so
        // copy the rows into a top-down GDI+ bitmap instead of wrapping the
        // scanlines directly (which would render the icon upside down).
        owned = std::make_unique<Bitmap>(bm.bmWidth, bm.bmHeight,
                                         PixelFormat32bppPARGB);
        if (!owned || owned->GetLastStatus() != Ok) return nullptr;
        BitmapData data{};
        Rect rect(0, 0, bm.bmWidth, bm.bmHeight);
        if (owned->LockBits(&rect, ImageLockModeWrite, PixelFormat32bppPARGB,
                            &data) != Ok) {
            return nullptr;
        }
        bool bottomUp = true;
        DIBSECTION dib{};
        if (GetObjectW(hbmp, sizeof(dib), &dib) == sizeof(dib) &&
            dib.dsBmih.biSize == sizeof(BITMAPINFOHEADER)) {
            bottomUp = dib.dsBmih.biHeight > 0;
        }
        size_t rowBytes = (size_t)bm.bmWidth * 4;
        for (INT y = 0; y < bm.bmHeight; ++y) {
            const BYTE* srcRow = (const BYTE*)bm.bmBits +
                                 (size_t)(bottomUp ? bm.bmHeight - 1 - y : y) *
                                     bm.bmWidthBytes;
            memcpy((BYTE*)data.Scan0 + (size_t)y * data.Stride, srcRow, rowBytes);
        }
        owned->UnlockBits(&data);
    } else {
        std::unique_ptr<Bitmap> source(Bitmap::FromHBITMAP(hbmp, nullptr));
        if (!source || source->GetLastStatus() != Ok) return nullptr;
        owned.reset(source.release());
    }
    if (owned->GetLastStatus() != Ok) return nullptr;
    return std::shared_ptr<Bitmap>(owned.release());
}

int IconRequestSize() {
    double px = (double)g_settings.iconSize * (g_settings.maxScalePercent / 100.0) *
                g_dpiScale;
    return ClampInt((int)std::ceil(px), 32, kIconRequestPx);
}

std::shared_ptr<Bitmap> LoadShellIcon(IShellItem* item, const std::wstring& key) {
    auto cached = g_iconCache.find(key);
    if (cached != g_iconCache.end()) return cached->second;

    std::shared_ptr<Bitmap> result;
    if (item) {
        ComPtr<IShellItemImageFactory> factory;
        if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(factory.put())))) {
            HBITMAP hbmp = nullptr;
            int px = IconRequestSize();
            SIZE size{px, px};
            HRESULT hr = factory->GetImage(
                size, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbmp);
            if (SUCCEEDED(hr) && hbmp) {
                result = CopyHBitmapToBitmap(hbmp);
                DeleteObject(hbmp);
            }
        }
        if (!result) {
            PWSTR path = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &path)) && path) {
                SHFILEINFOW info{};
                if (SHGetFileInfoW(path, 0, &info, sizeof(info),
                                   SHGFI_ICON | SHGFI_LARGEICON) &&
                    info.hIcon) {
                    Bitmap* raw = Bitmap::FromHICON(info.hIcon);
                    if (raw) {
                        result = CopyToOwnedBitmap(raw);
                        delete raw;
                    }
                    DestroyIcon(info.hIcon);
                }
                CoTaskMemFree(path);
            }
        }
    }

    if (!result) Wh_Log(L"Failed to load icon for %ls", key.c_str());
    g_iconCache[key] = result;
    return result;
}

HWND DesktopListView() {
    if (g_desktopListView && IsWindow(g_desktopListView)) return g_desktopListView;
    g_desktopListView = nullptr;
    ComPtr<IFolderView> folderView;
    HWND defView = nullptr;
    if (!GetDesktopView(folderView, &defView) || !defView) return nullptr;
    g_desktopListView =
        FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
    return g_desktopListView;
}

bool AreDesktopIconsHidden() {
    SHELLSTATE state{};
    SHGetSetSettings(&state, SSF_HIDEICONS, FALSE);
    return state.fHideIcons != FALSE;
}

// The list view belongs to Explorer's UI thread. A synchronous ShowWindow
// reenters comctl32 in the middle of the shell's own desktop refresh and can
// crash Explorer, so visibility changes are always posted from here.
void SetDesktopIconsHidden(bool hide) {
    HWND listView = DesktopListView();
    if (!listView) return;
    if (hide) {
        if (!AreDesktopIconsHidden() && IsWindowVisible(listView)) {
            ShowWindowAsync(listView, SW_HIDE);
            g_iconsHiddenByUs = true;
        }
    } else {
        if (g_iconsHiddenByUs && !AreDesktopIconsHidden() &&
            !IsWindowVisible(listView)) {
            ShowWindowAsync(listView, SW_SHOW);
        }
        g_iconsHiddenByUs = false;
    }
}

void EnsureDesktopIconsHidden() {
    if (!g_iconsHiddenByUs || !g_settings.hideDesktopIcons ||
        AreDesktopIconsHidden()) {
        return;
    }
    HWND listView = DesktopListView();
    if (listView && IsWindowVisible(listView)) {
        ShowWindowAsync(listView, SW_HIDE);
    }
}

void EnsureFrameBuffer(int width, int height) {
    if (width <= 0 || height <= 0) return;
    if (g_frame.width == width && g_frame.height == height && g_frame.memDc) return;

    if (g_frame.memDc) {
        g_frame.bitmap.reset();
        if (g_frame.oldBitmap) SelectObject(g_frame.memDc, g_frame.oldBitmap);
        if (g_frame.dib) DeleteObject(g_frame.dib);
        DeleteDC(g_frame.memDc);
        g_frame = FrameBuffer{};
    }

    if (!g_screenDc) g_screenDc = GetDC(nullptr);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(g_screenDc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        if (dib) DeleteObject(dib);
        return;
    }
    HDC mem = CreateCompatibleDC(g_screenDc);
    if (!mem) {
        DeleteObject(dib);
        return;
    }
    HBITMAP old = (HBITMAP)SelectObject(mem, dib);
    memset(bits, 0, (size_t)width * height * 4);

    g_frame.width = width;
    g_frame.height = height;
    g_frame.memDc = mem;
    g_frame.dib = dib;
    g_frame.oldBitmap = old;
    g_frame.bitmap = std::make_unique<Bitmap>(width, height, width * 4,
                                              PixelFormat32bppPARGB, (BYTE*)bits);
}

void PresentFrame() {
    if (!g_hwnd || !g_frame.memDc || !g_frame.bitmap) return;
    POINT src{0, 0};
    SIZE size{g_frame.width, g_frame.height};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwnd, g_screenDc, nullptr, &size, g_frame.memDc, &src, 0,
                        &blend, ULW_ALPHA);
}

void CreateTextures() {
    if (!g_glowTexture) {
        auto texture = std::make_unique<Bitmap>(256, 256, PixelFormat32bppPARGB);
        Graphics graphics(texture.get());
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.Clear(Color(0, 0, 0, 0));
        GraphicsPath path;
        path.AddEllipse(2.0f, 2.0f, 252.0f, 252.0f);
        PathGradientBrush brush(&path);
        brush.SetCenterColor(Color(255, 255, 255, 255));
        Color surround(0, 255, 255, 255);
        int count = 1;
        brush.SetSurroundColors(&surround, &count);
        graphics.FillPath(&brush, &path);
        g_glowTexture = std::move(texture);
    }

    if (!g_shadowTexture) {
        auto texture = std::make_unique<Bitmap>(256, 256, PixelFormat32bppPARGB);
        Graphics graphics(texture.get());
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.Clear(Color(0, 0, 0, 0));
        GraphicsPath path;
        path.AddEllipse(2.0f, 2.0f, 252.0f, 252.0f);
        PathGradientBrush brush(&path);
        brush.SetCenterColor(Color(170, 0, 0, 0));
        Color surround(0, 0, 0, 0);
        int count = 1;
        brush.SetSurroundColors(&surround, &count);
        graphics.FillPath(&brush, &path);
        g_shadowTexture = std::move(texture);
    }

    if (!g_noiseTexture) {
        auto texture = std::make_unique<Bitmap>(128, 128, PixelFormat32bppPARGB);
        BitmapData data{};
        Rect rect(0, 0, 128, 128);
        if (texture->LockBits(&rect, ImageLockModeWrite, PixelFormat32bppPARGB, &data) ==
            Ok) {
            uint32_t seed = 0x9e3779b9u;
            BYTE* base = (BYTE*)data.Scan0;
            for (int y = 0; y < 128; ++y) {
                BYTE* row = base + y * data.Stride;
                for (int x = 0; x < 128; ++x) {
                    seed = seed * 1664525u + 1013904223u;
                    BYTE alpha = (BYTE)((seed >> 24) % 14);
                    row[x * 4 + 0] = alpha;
                    row[x * 4 + 1] = alpha;
                    row[x * 4 + 2] = alpha;
                    row[x * 4 + 3] = alpha;
                }
            }
            texture->UnlockBits(&data);
        }
        g_noiseTexture = std::move(texture);
    }
}

void DrawTextureAlpha(Graphics& graphics, Image* image, const RectF& dest, double alpha) {
    if (!image || alpha <= 0.001) return;
    REAL scale = (REAL)ClampDouble(alpha, 0.0, 1.0);
    ColorMatrix matrix{};
    matrix.m[0][0] = scale;
    matrix.m[1][1] = scale;
    matrix.m[2][2] = scale;
    matrix.m[3][3] = scale;
    matrix.m[4][4] = 1.0f;
    ImageAttributes attributes;
    attributes.SetColorMatrix(&matrix);
    graphics.DrawImage(image, dest, 0, 0, (REAL)image->GetWidth(),
                       (REAL)image->GetHeight(), UnitPixel, &attributes);
}

void BoxBlurBitmap(Bitmap& bitmap, int radius) {
    if (radius < 1) return;
    int width = (int)bitmap.GetWidth();
    int height = (int)bitmap.GetHeight();
    if (width < 2 || height < 2) return;

    BitmapData data{};
    Rect rect(0, 0, width, height);
    if (bitmap.LockBits(&rect, ImageLockModeRead | ImageLockModeWrite,
                        PixelFormat32bppPARGB, &data) != Ok) {
        return;
    }
    int stride = data.Stride;
    BYTE* base = (BYTE*)data.Scan0;
    std::vector<BYTE> temp((size_t)stride * height);

    for (int y = 0; y < height; ++y) {
        BYTE* row = base + (size_t)y * stride;
        BYTE* out = temp.data() + (size_t)y * stride;
        for (int c = 0; c < 3; ++c) {
            int sum = 0;
            int window = 0;
            for (int dx = -radius; dx <= radius; ++dx) {
                int x = ClampInt(dx, 0, width - 1);
                sum += row[x * 4 + c];
                ++window;
            }
            for (int x = 0; x < width; ++x) {
                out[x * 4 + c] = (BYTE)(sum / window);
                int addX = ClampInt(x + radius + 1, 0, width - 1);
                int subX = ClampInt(x - radius, 0, width - 1);
                sum += row[addX * 4 + c] - row[subX * 4 + c];
            }
        }
        for (int x = 0; x < width; ++x) out[x * 4 + 3] = row[x * 4 + 3];
    }

    for (int x = 0; x < width; ++x) {
        for (int c = 0; c < 3; ++c) {
            int sum = 0;
            int window = 0;
            for (int dy = -radius; dy <= radius; ++dy) {
                int y = ClampInt(dy, 0, height - 1);
                sum += temp[(size_t)y * stride + x * 4 + c];
                ++window;
            }
            for (int y = 0; y < height; ++y) {
                base[(size_t)y * stride + x * 4 + c] = (BYTE)(sum / window);
                int addY = ClampInt(y + radius + 1, 0, height - 1);
                int subY = ClampInt(y - radius, 0, height - 1);
                sum += temp[(size_t)addY * stride + x * 4 + c] -
                       temp[(size_t)subY * stride + x * 4 + c];
            }
        }
    }

    bitmap.UnlockBits(&data);
}

void ComputeLayout() {
    int count = (int)g_items.size();
    g_labelFont.reset();
    g_labelFontFamily.reset();
    UINT dpi = g_hwnd.load() ? GetDpiForWindow(g_hwnd.load()) : 96;
    if (dpi == 0) dpi = 96;
    g_dpiScale = dpi / 96.0;

    g_layout.iconPx = (int)lround(g_settings.iconSize * g_dpiScale);
    g_layout.spacingPx = (int)lround(g_settings.spacing * g_dpiScale);
    g_layout.maxScale = std::max(1.0, g_settings.maxScalePercent / 100.0);
    g_layout.spacingFactor = ClampDouble(g_settings.spacingFactor / 100.0, 0.0, 1.0);
    g_layout.panelPadX = std::max(8, (int)lround(g_layout.iconPx * 0.30));
    g_layout.panelPadY = std::max(4, (int)lround(g_layout.iconPx * 0.07));
    int iconArea = (int)ceil(g_layout.iconPx * g_layout.maxScale);
    int liftMax = (int)ceil(4.0 * g_dpiScale);
    g_layout.panelHeightRest = g_layout.iconPx + 2 * g_layout.panelPadY;
    g_layout.panelHeightMax = iconArea + liftMax + 2 * g_layout.panelPadY;
    g_layout.panelHeight = g_layout.panelHeightMax;

    int baseContent = 0;
    if (count > 0) {
        baseContent = count * g_layout.iconPx + (count - 1) * g_layout.spacingPx;
    }
    g_layout.panelBaseWidth = baseContent + 2 * g_layout.panelPadX;

    double maxExpansion = 0.0;
    if (count > 0) {
        int affected = std::min(count, 5);
        maxExpansion = (g_layout.maxScale - 1.0) * g_layout.iconPx *
                       g_layout.spacingFactor * affected;
    }
    g_layout.panelMaxWidth =
        (int)ceil(g_layout.panelBaseWidth + maxExpansion + 10 * g_dpiScale);

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    HMONITOR monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    GetMonitorInfoW(monitor, &monitorInfo);
    int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    int widthCap = monitorWidth - (int)lround(48 * g_dpiScale);
    if (g_layout.panelMaxWidth > widthCap) {
        g_layout.panelMaxWidth = std::max(g_layout.panelBaseWidth, widthCap);
    }

    g_layout.glowMargin = (int)lround(10 * g_dpiScale);
    g_layout.labelFontPx = std::max(12, (int)lround(13 * g_dpiScale));
    g_layout.labelPillHeight = (int)lround(g_layout.labelFontPx * 1.9);
    g_layout.labelGap = (int)lround(8 * g_dpiScale);
    g_layout.labelAreaHeight =
        g_settings.showLabels ? g_layout.labelPillHeight + g_layout.labelGap : 0;

    g_layout.windowWidth = g_layout.panelMaxWidth + 2 * g_layout.glowMargin;
    g_layout.windowHeight = g_layout.glowMargin + g_layout.labelAreaHeight +
                            g_layout.panelHeight + g_layout.glowMargin;
    g_layout.panelTop = g_layout.glowMargin + g_layout.labelAreaHeight;
    g_layout.panelBottom = g_layout.panelTop + g_layout.panelHeight;
    g_layout.iconBottom = g_layout.panelBottom - g_layout.panelPadY;

    double pitch = g_layout.iconPx + g_layout.spacingPx;
    for (int i = 0; i < count; ++i) {
        g_items[i].baseCenterX =
            g_layout.windowWidth / 2.0 + (i - (count - 1) / 2.0) * pitch;
    }
}

RECT PanelClientRect() {
    RECT rect{};
    int width = (int)lround(g_panelWidth);
    rect.left = (g_layout.windowWidth - width) / 2;
    rect.right = rect.left + width;
    rect.top = g_layout.panelTop;
    rect.bottom = g_layout.panelBottom;
    return rect;
}

RectF IconRectFor(const DockItem& item) {
    double size = g_layout.iconPx * item.scale;
    double lift = 0.0;
    if (g_layout.maxScale > 1.0) {
        lift = (item.scale - 1.0) / (g_layout.maxScale - 1.0) * 4.0 * g_dpiScale;
    }
    double bottom = g_layout.iconBottom - lift;
    return RectF((REAL)(item.centerX - size * 0.5), (REAL)(bottom - size),
                 (REAL)size, (REAL)size);
}

double CurrentPanelProgress() {
    if (g_layout.maxScale <= 1.0) return 0.0;
    double largest = 1.0;
    for (const auto& item : g_items) {
        if (item.scale > largest) largest = item.scale;
    }
    return ClampDouble((largest - 1.0) / (g_layout.maxScale - 1.0), 0.0, 1.0);
}

double CurrentPanelHeight() {
    return g_layout.panelHeightRest +
           (g_layout.panelHeightMax - g_layout.panelHeightRest) *
               CurrentPanelProgress();
}

double CurrentPanelTop() {
    return g_layout.panelBottom - CurrentPanelHeight();
}

void PositionWindow() {
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    HMONITOR monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    GetMonitorInfoW(monitor, &monitorInfo);

    g_windowWidth = g_layout.windowWidth;
    g_windowHeight = g_layout.windowHeight;
    int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    g_windowX = monitorInfo.rcMonitor.left + (monitorWidth - g_windowWidth) / 2;
    int bottom = monitorInfo.rcWork.bottom -
                 (int)lround(g_settings.bottomOffset * g_dpiScale);
    g_windowY = bottom - g_windowHeight;
    if (g_windowY < monitorInfo.rcMonitor.top) g_windowY = monitorInfo.rcMonitor.top;
    g_lastWorkArea = monitorInfo.rcWork;

    SetWindowPos(g_hwnd, HWND_BOTTOM, g_windowX, g_windowY, g_windowWidth,
                 g_windowHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void CaptureBackdrop() {
    if (!g_hwnd || !g_screenDc || g_windowWidth <= 0 || g_windowHeight <= 0 ||
        g_layout.panelHeight <= 0) {
        return;
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    HMONITOR monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    GetMonitorInfoW(monitor, &monitorInfo);

    int capW = std::min(g_layout.panelMaxWidth,
                        (int)(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left));
    int capH = g_layout.panelHeight;
    int capX = g_windowX + (g_windowWidth - capW) / 2;
    int capY = g_windowY - capH;
    if (capX < monitorInfo.rcMonitor.left) capX = monitorInfo.rcMonitor.left;
    if (capX + capW > monitorInfo.rcMonitor.right)
        capX = monitorInfo.rcMonitor.right - capW;
    if (capY < monitorInfo.rcMonitor.top) capY = monitorInfo.rcMonitor.top;
    if (capY + capH > g_windowY) capH = g_windowY - capY;
    if (capW < 16 || capH < 16) {
        g_backdropValid = false;
        return;
    }

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = capW;
    info.bmiHeader.biHeight = -capH;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(g_screenDc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        if (dib) DeleteObject(dib);
        g_backdropValid = false;
        return;
    }
    HDC mem = CreateCompatibleDC(g_screenDc);
    HBITMAP old = mem ? (HBITMAP)SelectObject(mem, dib) : nullptr;
    BOOL copied = FALSE;
    if (mem) {
        copied = BitBlt(mem, 0, 0, capW, capH, g_screenDc, capX, capY, SRCCOPY);
        SelectObject(mem, old);
        DeleteDC(mem);
    }

    if (!copied) {
        DeleteObject(dib);
        g_backdropValid = false;
        return;
    }

    BYTE* bytes = (BYTE*)bits;
    for (int i = 0; i < capW * capH; ++i) bytes[i * 4 + 3] = 255;

    int smallW = std::max(4, capW / 4);
    int smallH = std::max(4, capH / 4);
    Bitmap source(capW, capH, capW * 4, PixelFormat32bppPARGB, bytes);
    auto small = std::make_unique<Bitmap>(smallW, smallH, PixelFormat32bppPARGB);
    if (small->GetLastStatus() == Ok) {
        Graphics graphics(small.get());
        graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
        graphics.DrawImage(&source, Rect(0, 0, smallW, smallH), 0, 0, capW, capH,
                           UnitPixel);
        int blurRadius = (int)lround(g_settings.glassBlur / 4.0);
        if (blurRadius > 0) {
            BoxBlurBitmap(*small, blurRadius);
            BoxBlurBitmap(*small, blurRadius);
        }
        g_backdrop = std::move(small);
        g_backdropValid = true;
        g_backdropCaptureTime = std::chrono::steady_clock::now();
    } else {
        g_backdropValid = false;
    }
    DeleteObject(dib);
}

void DrawPanelShadow(Graphics& graphics, const RectF& panel, float radius) {
    for (int i = 5; i >= 1; --i) {
        double expand = i * 1.6 * g_dpiScale;
        RectF shadowRect(panel.X - (REAL)expand, panel.Y - (REAL)(expand * 0.5) + 2.0f,
                         panel.Width + (REAL)(expand * 2.0f),
                         panel.Height + (REAL)(expand * 2.0f));
        GraphicsPath path;
        AddRoundedRect(path, shadowRect, radius + (float)expand);
        SolidBrush brush(Color(7, 0, 0, 0));
        graphics.FillPath(&brush, &path);
    }
}

void DrawPanelRim(Graphics& graphics, GraphicsPath& panelPath, const RectF& panel,
                  float radius) {
    Pen darkPen(Color(40, 18, 24, 34), 1.0f);
    graphics.DrawPath(&darkPen, &panelPath);

    LinearGradientBrush rimBrush(
        RectF(panel.X, panel.Y, panel.Width, panel.Height),
        Color(235, 255, 255, 255), Color(55, 255, 255, 255),
        LinearGradientModeVertical);
    Pen rimPen(&rimBrush, (REAL)(1.4 * g_dpiScale));
    graphics.DrawPath(&rimPen, &panelPath);

    RectF innerRect(panel.X + 1.5f, panel.Y + 1.5f, panel.Width - 3.0f,
                    panel.Height - 3.0f);
    GraphicsPath innerPath;
    AddRoundedRect(innerPath, innerRect, std::max(1.0f, radius - 1.5f));

    Color iridColors[] = {Color(0, 255, 255, 255), Color(60, 120, 220, 255),
                          Color(45, 255, 190, 255), Color(60, 255, 150, 210),
                          Color(0, 255, 255, 255)};
    REAL iridPositions[] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};
    LinearGradientBrush iridBrush(
        RectF(panel.X, panel.Y, panel.Width, panel.Height), Color(0, 255, 255, 255),
        Color(0, 255, 255, 255), LinearGradientModeHorizontal);
    iridBrush.SetInterpolationColors(iridColors, iridPositions, 5);
    Pen iridPen(&iridBrush, 1.0f);
    graphics.DrawPath(&iridPen, &innerPath);

    Color glossColors[] = {Color(0, 255, 255, 255), Color(180, 255, 255, 255),
                           Color(0, 255, 255, 255)};
    REAL glossPositions[] = {0.0f, 0.5f, 1.0f};
    LinearGradientBrush glossBrush(
        RectF(panel.X, panel.Y, panel.Width, 2.0f), Color(0, 255, 255, 255),
        Color(0, 255, 255, 255), LinearGradientModeHorizontal);
    glossBrush.SetInterpolationColors(glossColors, glossPositions, 3);
    Pen glossPen(&glossBrush, (REAL)(1.3 * g_dpiScale));
    GraphicsPath glossLine;
    glossLine.AddLine(panel.X + radius * 0.8f, panel.Y + 1.0f,
                      panel.X + panel.Width - radius * 0.8f, panel.Y + 1.0f);
    graphics.DrawPath(&glossPen, &glossLine);
}

void DrawGlassPanel(Graphics& graphics, const RectF& panel, float radius) {
    GraphicsPath panelPath;
    AddRoundedRect(panelPath, panel, radius);

    DrawPanelShadow(graphics, panel, radius);

    GraphicsState state = graphics.Save();
    graphics.SetClip(&panelPath, CombineModeReplace);

    if (g_backdropValid && g_backdrop) {
        graphics.DrawImage(g_backdrop.get(), panel);
        SolidBrush tint(Color((BYTE)lround(255.0 * g_settings.glassOpacity / 100.0),
                              247, 250, 255));
        graphics.FillRectangle(&tint, panel);
    } else {
        LinearGradientBrush fallback(
            panel, Color(205, 228, 234, 242), Color(165, 196, 205, 218),
            LinearGradientModeVertical);
        graphics.FillRectangle(&fallback, panel);
    }

    SolidBrush shadeTint(Color(26, 6, 10, 18));
    graphics.FillRectangle(&shadeTint, panel);

    {
        RectF shadeRect(panel.X, panel.Y + panel.Height * 0.52f, panel.Width,
                        panel.Height * 0.48f);
        LinearGradientBrush shade(shadeRect, Color(0, 0, 0, 0), Color(70, 6, 10, 16),
                                  LinearGradientModeVertical);
        graphics.FillRectangle(&shade, shadeRect);
    }
    {
        RectF glossRect(panel.X, panel.Y, panel.Width, panel.Height * 0.40f);
        LinearGradientBrush gloss(glossRect, Color(92, 255, 255, 255),
                                  Color(0, 255, 255, 255), LinearGradientModeVertical);
        graphics.FillRectangle(&gloss, glossRect);
    }
    {
        float lightHeight = panel.Height * 0.18f;
        RectF lightRect(panel.X, panel.Y + panel.Height - lightHeight, panel.Width,
                        lightHeight);
        LinearGradientBrush light(lightRect, Color(0, 255, 255, 255),
                                  Color(30, 255, 255, 255),
                                  LinearGradientModeVertical);
        graphics.FillRectangle(&light, lightRect);
    }

    if (g_noiseTexture) {
        TextureBrush noise(g_noiseTexture.get(), WrapModeTile);
        graphics.FillPath(&noise, &panelPath);
    }

    if (g_layout.maxScale > 1.0) {
        for (const auto& item : g_items) {
            if (item.scale <= 1.002) continue;
            double strength = (item.scale - 1.0) / (g_layout.maxScale - 1.0);
            double glowSize = g_layout.iconPx * (1.9 + 0.9 * item.scale);
            RectF glowRect((REAL)(item.centerX - glowSize * 0.5),
                           (REAL)(g_layout.iconBottom - glowSize * 0.62),
                           (REAL)glowSize, (REAL)glowSize);
            DrawTextureAlpha(graphics, g_glowTexture.get(), glowRect,
                             0.10 + 0.30 * strength);
        }
    }

    if (g_intensity > 0.01 && g_mouseXValid) {
        double glowSize = panel.Height * 2.4;
        double centerY = (panel.Y + panel.Height) - panel.Height * 0.45;
        RectF glowRect((REAL)(g_lastMouseX - glowSize * 0.5),
                       (REAL)(centerY - glowSize * 0.5), (REAL)glowSize,
                       (REAL)glowSize);
        DrawTextureAlpha(graphics, g_glowTexture.get(), glowRect,
                         0.10 * g_intensity);
    }

    graphics.Restore(state);

    DrawPanelRim(graphics, panelPath, panel, radius);
}

void DrawItemLabel(Graphics& graphics, int index, double panelTopNow) {
    if (index < 0 || index >= (int)g_items.size()) return;
    const DockItem& item = g_items[index];
    double alpha = item.labelAlpha;
    if (alpha <= 0.01) return;

    if (!g_labelFont) {
        auto family = std::make_unique<FontFamily>(L"Segoe UI Variable Text");
        if (family->GetLastStatus() != Ok || !family->IsAvailable()) {
            family = std::make_unique<FontFamily>(L"Segoe UI");
        }
        g_labelFont = std::make_unique<Font>(family.get(), (REAL)g_layout.labelFontPx,
                                             FontStyleRegular, UnitPixel);
        if (g_labelFont->GetLastStatus() != Ok) {
            g_labelFont.reset();
            return;
        }
        g_labelFontFamily = std::move(family);
    }

    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    format.SetFormatFlags(StringFormatFlagsNoWrap);
    format.SetTrimming(StringTrimmingEllipsisCharacter);

    double padX = 12.0 * g_dpiScale;
    double maxWidth =
        std::max(80.0, (double)g_layout.panelBaseWidth - 24.0 * g_dpiScale);
    RectF measureRect(0, 0, (REAL)maxWidth, (REAL)g_layout.labelPillHeight);
    RectF bounds;
    graphics.MeasureString(item.name.c_str(), -1, g_labelFont.get(), measureRect,
                           &format, &bounds);

    double pillWidth = ClampDouble(bounds.Width + padX * 2.0, 40.0, maxWidth);
    double pillHeight = g_layout.labelPillHeight;
    double slide = (1.0 - alpha) * 6.0 * g_dpiScale;
    double centerX = ClampDouble(item.centerX, pillWidth * 0.5 + 6.0 * g_dpiScale,
                                 g_layout.windowWidth - pillWidth * 0.5 -
                                     6.0 * g_dpiScale);
    RectF pill((REAL)(centerX - pillWidth * 0.5),
               (REAL)(panelTopNow - g_layout.labelGap - pillHeight + slide),
               (REAL)pillWidth, (REAL)pillHeight);

    {
        for (int i = 3; i >= 1; --i) {
            double expand = i * 1.4 * g_dpiScale;
            RectF shadowRect(pill.X - (REAL)expand, pill.Y - (REAL)(expand * 0.4) + 2,
                             pill.Width + (REAL)expand * 2,
                             pill.Height + (REAL)expand * 2);
            GraphicsPath path;
            AddRoundedRect(path, shadowRect, (float)(pill.Height * 0.5 + expand));
            SolidBrush brush(Color((BYTE)(10 * alpha), 0, 0, 0));
            graphics.FillPath(&brush, &path);
        }
    }

    GraphicsPath pillPath;
    AddRoundedRect(pillPath, pill, (float)(pill.Height * 0.5));
    GraphicsState state = graphics.Save();
    graphics.SetClip(&pillPath, CombineModeReplace);

    if (g_backdropValid && g_backdrop) {
        REAL fade = (REAL)alpha;
        ColorMatrix matrix{};
        matrix.m[0][0] = fade;
        matrix.m[1][1] = fade;
        matrix.m[2][2] = fade;
        matrix.m[3][3] = fade;
        matrix.m[4][4] = 1.0f;
        ImageAttributes attributes;
        attributes.SetColorMatrix(&matrix);
        graphics.DrawImage(g_backdrop.get(), pill, 0, 0, (REAL)g_backdrop->GetWidth(),
                           (REAL)g_backdrop->GetHeight(), UnitPixel, &attributes);
        SolidBrush milk(Color((BYTE)(lround(170 * alpha)), 246, 249, 252));
        graphics.FillPath(&milk, &pillPath);
    } else {
        SolidBrush milk(Color((BYTE)(lround(220 * alpha)), 240, 244, 248));
        graphics.FillPath(&milk, &pillPath);
    }

    {
        RectF glossRect(pill.X, pill.Y, pill.Width, pill.Height * 0.55f);
        LinearGradientBrush gloss(glossRect,
                                  Color((BYTE)(lround(120 * alpha)), 255, 255, 255),
                                  Color(0, 255, 255, 255),
                                  LinearGradientModeVertical);
        graphics.FillRectangle(&gloss, glossRect);
    }
    graphics.Restore(state);

    Pen rim(Color((BYTE)(lround(180 * alpha)), 255, 255, 255),
            (REAL)(1.0 * g_dpiScale));
    graphics.DrawPath(&rim, &pillPath);

    SolidBrush textBrush(Color((BYTE)(lround(235 * alpha)), 30, 32, 38));
    RectF textRect(pill.X + (REAL)padX, pill.Y, pill.Width - (REAL)(padX * 2.0),
                   pill.Height);
    graphics.DrawString(item.name.c_str(), -1, g_labelFont.get(), textRect, &format,
                        &textBrush);
}

void DrawIcons(Graphics& graphics) {
    for (const auto& item : g_items) {
        RectF rect = IconRectFor(item);
        if (item.scale <= 0.002) continue;
        if (g_shadowTexture) {
            double shadowWidth = rect.Width * 0.92;
            double shadowHeight = rect.Height * 0.30;
            RectF shadowRect(rect.X + (rect.Width - (REAL)shadowWidth) * 0.5f,
                             rect.Y + rect.Height - (REAL)(shadowHeight * 0.42),
                             (REAL)shadowWidth, (REAL)shadowHeight);
            DrawTextureAlpha(graphics, g_shadowTexture.get(), shadowRect, 0.42);
        }
        if (item.icon) {
            graphics.DrawImage(item.icon.get(), rect);
        } else {
            SolidBrush placeholder(Color(90, 255, 255, 255));
            GraphicsPath path;
            AddRoundedRect(path, rect, rect.Width * 0.22f);
            graphics.FillPath(&placeholder, &path);
        }
    }
}

void RenderFrame() {
    if (!g_frame.bitmap) return;
    Graphics graphics(g_frame.bitmap.get());
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    graphics.Clear(Color(0, 0, 0, 0));

    if (g_items.empty()) {
        PresentFrame();
        return;
    }

    double width = ClampDouble(g_panelWidth, (double)g_layout.panelBaseWidth,
                               (double)g_layout.panelMaxWidth);
    double x = (g_layout.windowWidth - width) * 0.5;
    double labelAnchor = 0.0;
    if (g_settings.transparentBackground) {
        labelAnchor = g_layout.panelBottom - g_layout.panelHeightMax;
    } else {
        double panelTopNow = CurrentPanelTop();
        double panelHeightNow = g_layout.panelBottom - panelTopNow;
        RectF panel((REAL)x, (REAL)panelTopNow, (REAL)width,
                    (REAL)panelHeightNow);
        float radius = (float)(panelHeightNow * 0.36);
        DrawGlassPanel(graphics, panel, radius);
        labelAnchor = panelTopNow;
    }
    DrawIcons(graphics);
    if (g_settings.showLabels) {
        DrawItemLabel(graphics, g_hoverIndex, labelAnchor);
    }

    PresentFrame();
}

void EnsureAnimationTimer() {
    if (!g_hwnd) return;
    if (!g_animationTimerRunning) {
        g_lastTickTime = std::chrono::steady_clock::now();
        SetTimer(g_hwnd, kTimerAnimation, 15, nullptr);
        g_animationTimerRunning = true;
    }
}

bool IsPointerOverDock(POINT screenPoint) {
    HWND under = WindowFromPoint(screenPoint);
    if (!under) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(under, &pid);
    if (pid != GetCurrentProcessId()) return false;
    bool overDock = (under == g_hwnd);
    if (!overDock && g_settings.transparentBackground) {
        // Fully transparent pixels are click-through, so accept the desktop
        // windows underneath as hovering the dock area.
        wchar_t className[64] = L"";
        if (GetClassNameW(under, className, ARRAYSIZE(className)) > 0) {
            overDock = !_wcsicmp(className, L"Progman") ||
                       !_wcsicmp(className, L"WorkerW") ||
                       !_wcsicmp(className, L"SHELLDLL_DefView") ||
                       !_wcsicmp(className, L"SysListView32");
        }
    }
    if (!overDock) return false;
    POINT clientPoint = screenPoint;
    ScreenToClient(g_hwnd, &clientPoint);
    RECT hoverRect = PanelClientRect();
    InflateRect(&hoverRect, (int)lround(10 * g_dpiScale),
                (int)lround(10 * g_dpiScale) + g_layout.labelAreaHeight);
    return PtInRect(&hoverRect, clientPoint) != FALSE;
}

void UpdateHoverPollTimer() {
    if (!g_hwnd) return;
    bool wantPoll = g_settings.transparentBackground && g_dockShown;
    if (wantPoll && !g_hoverPollRunning) {
        SetTimer(g_hwnd, kTimerHoverPoll, kHoverPollMs, nullptr);
        g_hoverPollRunning = true;
    } else if (!wantPoll && g_hoverPollRunning) {
        KillTimer(g_hwnd, kTimerHoverPoll);
        g_hoverPollRunning = false;
    }
}

bool UpdateAnimation() {
    if (g_items.empty() || !g_hwnd) return false;

    auto now = std::chrono::steady_clock::now();
    double dt = std::chrono::duration<double>(now - g_lastTickTime).count();
    g_lastTickTime = now;
    if (dt <= 0.0 || dt > 0.08) dt = 0.016;

    POINT screenPoint{};
    GetCursorPos(&screenPoint);
    POINT clientPoint = screenPoint;
    ScreenToClient(g_hwnd, &clientPoint);
    bool inside = IsPointerOverDock(screenPoint);

    if (inside) {
        g_lastMouseX = clientPoint.x;
        g_mouseXValid = true;
    }

    double targetIntensity = inside ? 1.0 : 0.0;
    double intensityAlpha = 1.0 - std::exp(-12.0 * dt);
    g_intensity += (targetIntensity - g_intensity) * intensityAlpha;
    if (std::fabs(g_intensity - targetIntensity) < 0.001) g_intensity = targetIntensity;

    int count = (int)g_items.size();
    std::vector<double> targetScale(count, 1.0);
    std::vector<double> extraWidth(count, 0.0);
    double totalExpansion = 0.0;

    if (g_intensity > 0.0001 && g_mouseXValid) {
        for (int i = 0; i < count; ++i) {
            double distance = std::fabs(g_lastMouseX - g_items[i].baseCenterX);
            double amount = Falloff(distance) * g_intensity;
            targetScale[i] = 1.0 + (g_layout.maxScale - 1.0) * amount;
            extraWidth[i] =
                (targetScale[i] - 1.0) * g_layout.iconPx * g_layout.spacingFactor;
            totalExpansion += extraWidth[i];
        }
    }

    int nearest = -1;
    if (inside) {
        double best = 1e18;
        for (int i = 0; i < count; ++i) {
            double distance = std::fabs(g_lastMouseX - g_items[i].baseCenterX);
            if (distance < best) {
                best = distance;
                nearest = i;
            }
        }
    }
    if (nearest >= 0) g_hoverIndex = nearest;

    double lerp = 1.0 - std::exp(-(double)g_settings.smoothing * dt);
    lerp = ClampDouble(lerp, 0.0, 1.0);
    bool changed = false;
    double cumulative = 0.0;
    double currentExpansion = 0.0;

    for (int i = 0; i < count; ++i) {
        double targetCenter =
            g_items[i].baseCenterX + cumulative + extraWidth[i] * 0.5 -
            totalExpansion * 0.5;
        cumulative += extraWidth[i];

        double newScale = g_items[i].scale + (targetScale[i] - g_items[i].scale) * lerp;
        double newCenter =
            g_items[i].centerX + (targetCenter - g_items[i].centerX) * lerp;
        if (std::fabs(newScale - g_items[i].scale) > 0.0002 ||
            std::fabs(newCenter - g_items[i].centerX) > 0.02) {
            changed = true;
        }
        g_items[i].scale = newScale;
        g_items[i].centerX = newCenter;
        currentExpansion +=
            (newScale - 1.0) * g_layout.iconPx * g_layout.spacingFactor;

        double labelTarget = (g_settings.showLabels && inside && i == g_hoverIndex &&
                              newScale > 1.01)
                                 ? 1.0
                                 : 0.0;
        double labelAlpha =
            g_items[i].labelAlpha + (labelTarget - g_items[i].labelAlpha) *
                                        (1.0 - std::exp(-15.0 * dt));
        if (std::fabs(labelAlpha - g_items[i].labelAlpha) > 0.002) changed = true;
        g_items[i].labelAlpha = labelAlpha;
    }

    double targetPanelWidth =
        g_layout.panelBaseWidth + currentExpansion;
    double newPanelWidth =
        g_panelWidth + (targetPanelWidth - g_panelWidth) *
                           (1.0 - std::exp(-10.0 * dt));
    if (std::fabs(newPanelWidth - g_panelWidth) > 0.05) changed = true;
    g_panelWidth = newPanelWidth;

    if (changed) RenderFrame();

    return inside || g_intensity > 0.002 ||
           std::fabs(g_panelWidth - targetPanelWidth) > 0.2;
}

int ItemAtCursor(POINT screenPoint) {
    POINT point = screenPoint;
    ScreenToClient(g_hwnd, &point);
    int best = -1;
    double bestDistance = 1e18;
    for (int i = 0; i < (int)g_items.size(); ++i) {
        RectF rect = IconRectFor(g_items[i]);
        REAL pad = (REAL)(4.0 * g_dpiScale);
        rect.X -= pad;
        rect.Y -= pad;
        rect.Width += pad * 2;
        rect.Height += pad * 2;
        if (point.x >= rect.X && point.x <= rect.X + rect.Width &&
            point.y >= rect.Y && point.y <= rect.Y + rect.Height) {
            double distance = std::fabs(point.x - (rect.X + rect.Width * 0.5));
            if (distance < bestDistance) {
                bestDistance = distance;
                best = i;
            }
        }
    }
    return best;
}

void LaunchItem(DockItem& item) {
    if (!item.pidl) return;
    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    info.lpVerb = L"open";
    info.lpIDList = item.pidl;
    info.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&info)) {
        Wh_Log(L"ShellExecuteEx failed for %ls: %lu", item.name.c_str(), GetLastError());
    }
}

void KeepWindowAtBottom() {
    if (!g_hwnd || g_menuOpen) return;
    SetWindowPos(g_hwnd, HWND_BOTTOM, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void ShowItemContextMenu(int index, POINT screenPoint) {
    if (index < 0 || index >= (int)g_items.size()) return;
    DockItem& item = g_items[index];
    if (!item.pidl) return;

    ComPtr<IShellFolder> parent;
    PCUITEMID_CHILD child = nullptr;
    if (FAILED(SHBindToParent(item.pidl, IID_IShellFolder,
                              (void**)parent.put(), &child)) ||
        !parent || !child) {
        Wh_Log(L"SHBindToParent failed");
        return;
    }

    ComPtr<IContextMenu> menu;
    if (FAILED(parent->GetUIObjectOf(g_hwnd, 1, &child, IID_IContextMenu, nullptr,
                                     (void**)menu.put())) ||
        !menu) {
        Wh_Log(L"GetUIObjectOf failed");
        return;
    }

    HMENU popup = CreatePopupMenu();
    if (!popup) return;

    const UINT firstCommand = 1;
    UINT flags = CMF_NORMAL;
    if (GetKeyState(VK_SHIFT) & 0x8000) flags |= CMF_EXTENDEDVERBS;
    HRESULT query = menu->QueryContextMenu(popup, 0, firstCommand, 0x6FFF, flags);
    if (query < 0) {
        DestroyMenu(popup);
        return;
    }

    ComPtr<IContextMenu2> menu2;
    ComPtr<IContextMenu3> menu3;
    menu->QueryInterface(IID_IContextMenu2, (void**)menu2.put());
    menu->QueryInterface(IID_IContextMenu3, (void**)menu3.put());

    g_activeMenu2 = std::move(menu2);
    g_activeMenu3 = std::move(menu3);
    g_menuOpen = true;

    HWND previousForeground = GetForegroundWindow();
    LONG_PTR exStyle = GetWindowLongPtrW(g_hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(g_hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_NOACTIVATE);
    SetForegroundWindow(g_hwnd);

    UINT command = TrackPopupMenuEx(popup, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                    screenPoint.x, screenPoint.y, g_hwnd, nullptr);
    PostMessageW(g_hwnd, WM_NULL, 0, 0);

    SetWindowLongPtrW(g_hwnd, GWL_EXSTYLE, exStyle);
    if (previousForeground && IsWindow(previousForeground) &&
        previousForeground != g_hwnd) {
        SetForegroundWindow(previousForeground);
    }
    g_menuOpen = false;
    g_activeMenu2.Reset();
    g_activeMenu3.Reset();
    KeepWindowAtBottom();

    if (command >= firstCommand) {
        CMINVOKECOMMANDINFOEX invoke{};
        invoke.cbSize = sizeof(invoke);
        invoke.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE;
        invoke.hwnd = g_hwnd;
        invoke.lpVerb = MAKEINTRESOURCEA(command - firstCommand);
        invoke.lpVerbW = MAKEINTRESOURCEW(command - firstCommand);
        invoke.nShow = SW_SHOWNORMAL;
        invoke.ptInvoke = screenPoint;
        menu->InvokeCommand(reinterpret_cast<LPCMINVOKECOMMANDINFO>(&invoke));
        SetTimer(g_hwnd, kTimerRefresh, 700, nullptr);
    }

    DestroyMenu(popup);
}

void PruneIconCache() {
    for (auto it = g_iconCache.begin(); it != g_iconCache.end();) {
        bool used = false;
        for (const auto& item : g_items) {
            if (item.key == it->first) {
                used = true;
                break;
            }
        }
        if (used) {
            ++it;
        } else {
            it = g_iconCache.erase(it);
        }
    }
}

// Virtual items (Recycle Bin, This PC, ...) have no file-system events, so
// their state changes (e.g. an emptied Recycle Bin) are picked up by the
// slow fallback check instead.
void RefreshVirtualIcons(const std::vector<EnumeratedItem>& found) {
    bool updated = false;
    for (const auto& entry : found) {
        if (entry.isFilesystem) continue;
        if (g_iconCache.find(entry.key) == g_iconCache.end()) continue;
        g_iconCache.erase(entry.key);
        auto icon = LoadShellIcon(entry.shellItem.Get(), entry.key);
        if (!icon) continue;
        for (auto& item : g_items) {
            if (item.key == entry.key) {
                item.icon = icon;
                updated = true;
                break;
            }
        }
    }
    if (updated && !g_animationTimerRunning) RenderFrame();
}

void HideDock() {
    if (g_dockShown) {
        ShowWindow(g_hwnd, SW_HIDE);
        g_dockShown = false;
    }
    SetDesktopIconsHidden(false);
    UpdateHoverPollTimer();
}

bool RebuildFromDesktop(bool force) {
    if (g_stopRequested) return false;
    std::vector<EnumeratedItem> found;
    if (!EnumerateDesktopItems(found)) {
        Wh_Log(L"Desktop item enumeration failed");
        return false;
    }

    auto excludes = ParseExcludeList(g_settings.excludeItems);
    if (!excludes.empty()) {
        found.erase(std::remove_if(found.begin(), found.end(),
                                   [&](const EnumeratedItem& entry) {
                                       return std::find(excludes.begin(), excludes.end(),
                                                        ToLowerString(entry.name)) !=
                                              excludes.end();
                                   }),
                    found.end());
    }

    std::stable_sort(found.begin(), found.end(),
                     [](const EnumeratedItem& a, const EnumeratedItem& b) {
                         if (a.position.y != b.position.y)
                             return a.position.y < b.position.y;
                         if (a.position.x != b.position.x)
                             return a.position.x < b.position.x;
                         return a.name < b.name;
                     });

    if (found.empty()) {
        Wh_Log(L"No desktop items to show");
        g_items.clear();
        g_lastSignature.clear();
        HideDock();
        return true;
    }

    std::wstring signature;
    for (const auto& entry : found) {
        signature += entry.key;
        signature += L'|';
        signature += std::to_wstring(entry.position.x);
        signature += L'|';
        signature += std::to_wstring(entry.position.y);
        signature += L'\n';
    }
    if (!force && !g_items.empty() && signature == g_lastSignature) {
        RefreshVirtualIcons(found);
        g_lastItemCheckTime = std::chrono::steady_clock::now();
        return true;
    }

    std::vector<DockItem> items;
    items.reserve(found.size());
    for (auto& entry : found) {
        if (g_stopRequested) return false;
        DockItem item;
        item.name = entry.name;
        item.key = entry.key;
        item.pidlOwner = entry.pidlOwner;
        item.pidl = entry.pidl;
        item.icon = LoadShellIcon(entry.shellItem.Get(), entry.key);
        items.push_back(std::move(item));
    }

    g_items = std::move(items);
    g_lastSignature = std::move(signature);
    PruneIconCache();
    g_intensity = 0.0;
    g_hoverIndex = -1;
    g_mouseXValid = false;

    ComputeLayout();
    g_panelWidth = g_layout.panelBaseWidth;
    for (auto& item : g_items) item.centerX = item.baseCenterX;
    EnsureFrameBuffer(g_layout.windowWidth, g_layout.windowHeight);
    PositionWindow();

    if (!g_dockShown) {
        if (g_settings.hideDesktopIcons) {
            SetDesktopIconsHidden(true);
        }
        CaptureBackdrop();
        RenderFrame();
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
        g_dockShown = true;
        EnsureAnimationTimer();
    } else {
        CaptureBackdrop();
        RenderFrame();
    }
    UpdateHoverPollTimer();

    g_lastItemCheckTime = std::chrono::steady_clock::now();
    return true;
}

void ReloadSettings() {
    LoadSettings();
    g_iconCache.clear();
    RebuildFromDesktop(true);
    SetDesktopIconsHidden(g_settings.hideDesktopIcons != 0 && g_dockShown);
    UpdateHoverPollTimer();
}

void RegisterShellNotifications() {
    if (!g_hwnd || g_changeNotifyRegister) return;

    PIDLIST_ABSOLUTE desktop = nullptr;
    PIDLIST_ABSOLUTE publicDesktop = nullptr;
    SHGetKnownFolderIDList(FOLDERID_Desktop, 0, nullptr, &desktop);
    SHGetKnownFolderIDList(FOLDERID_PublicDesktop, 0, nullptr, &publicDesktop);

    // Non-recursive: only the items directly on the desktop appear in the
    // dock, so file writes inside desktop folders do not wake us up.
    SHChangeNotifyEntry entries[2] = {};
    int entryCount = 0;
    if (desktop) entries[entryCount++] = {desktop, FALSE};
    if (publicDesktop) entries[entryCount++] = {publicDesktop, FALSE};

    if (entryCount > 0) {
        LONG events = SHCNE_CREATE | SHCNE_DELETE | SHCNE_MKDIR | SHCNE_RMDIR |
                      SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER |
                      SHCNE_UPDATEITEM | SHCNE_UPDATEIMAGE | SHCNE_UPDATEDIR;
        // SHCNRF_ShellLevel delivers no messages on recent Windows 11 builds;
        // interrupt-level notifications for these two folders are reliable.
        g_changeNotifyRegister = SHChangeNotifyRegister(
            g_hwnd, SHCNRF_InterruptLevel | SHCNRF_NewDelivery, events,
            kMsgShellNotify, entryCount, entries);
        if (!g_changeNotifyRegister) {
            Wh_Log(L"SHChangeNotifyRegister failed");
        }
    }

    if (desktop) CoTaskMemFree(desktop);
    if (publicDesktop) CoTaskMemFree(publicDesktop);
}

void UnregisterShellNotifications() {
    if (g_changeNotifyRegister) {
        SHChangeNotifyDeregister(g_changeNotifyRegister);
        g_changeNotifyRegister = 0;
    }
}

void HandleShellNotification(WPARAM wParam, LPARAM lParam) {
    PIDLIST_ABSOLUTE* pidls = nullptr;
    LONG event = 0;
    HANDLE lock =
        SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam, &pidls, &event);
    bool needsRebuild = true;
    if (lock) {
        if (event & SHCNE_UPDATEIMAGE) {
            g_iconCache.clear();
            g_notifyForceRebuild = true;
        } else if (event & SHCNE_UPDATEITEM) {
            // A content update only affects the icon of that one item;
            // everything else stays cached.
            needsRebuild = false;
            if (pidls && pidls[0]) {
                ComPtr<IShellItem> item;
                if (SUCCEEDED(SHCreateItemFromIDList(pidls[0],
                                                     IID_PPV_ARGS(item.put())))) {
                    std::wstring key = GetShellItemName(
                        item.Get(), SIGDN_DESKTOPABSOLUTEPARSING);
                    if (!key.empty() && g_iconCache.erase(key) > 0) {
                        g_notifyForceRebuild = true;
                        needsRebuild = true;
                    }
                }
            }
        }
        SHChangeNotification_Unlock(lock);
    }
    if (needsRebuild && g_hwnd) {
        SetTimer(g_hwnd, kTimerShellNotify, kNotifyRebuildDelayMs, nullptr);
    }
}

void RefreshWorkAreaIfNeeded() {
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    HMONITOR monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    GetMonitorInfoW(monitor, &monitorInfo);
    if (memcmp(&monitorInfo.rcWork, &g_lastWorkArea, sizeof(RECT)) != 0) {
        PositionWindow();
        CaptureBackdrop();
        RenderFrame();
    }
}

LRESULT CALLBACK DockWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_TIMER:
            switch (wParam) {
                case kTimerAnimation:
                    if (!UpdateAnimation()) {
                        KillTimer(hwnd, kTimerAnimation);
                        g_animationTimerRunning = false;
                        RenderFrame();
                    }
                    return 0;
                case kTimerHoverPoll:
                    if (!g_animationTimerRunning && g_dockShown) {
                        POINT screenPoint{};
                        GetCursorPos(&screenPoint);
                        if (IsPointerOverDock(screenPoint)) {
                            EnsureAnimationTimer();
                        }
                    }
                    return 0;
                case kTimerMaintenance: {
                    auto now = std::chrono::steady_clock::now();
                    if (now - g_lastItemCheckTime >
                        std::chrono::seconds(kFullRebuildSeconds)) {
                        g_lastItemCheckTime = now;
                        RebuildFromDesktop(false);
                    }
                    EnsureDesktopIconsHidden();
                    if (g_settings.backdropRefreshSeconds > 0 && !g_menuOpen &&
                        now - g_backdropCaptureTime >
                            std::chrono::seconds(g_settings.backdropRefreshSeconds)) {
                        CaptureBackdrop();
                        if (!g_animationTimerRunning) RenderFrame();
                    }
                    RefreshWorkAreaIfNeeded();
                    return 0;
                }
                case kTimerShellNotify:
                    KillTimer(hwnd, kTimerShellNotify);
                    if (g_menuOpen) {
                        SetTimer(hwnd, kTimerShellNotify, 500, nullptr);
                        return 0;
                    }
                    if (RebuildFromDesktop(g_notifyForceRebuild)) {
                        g_notifyForceRebuild = false;
                    }
                    return 0;
                case kTimerRefresh:
                    KillTimer(hwnd, kTimerRefresh);
                    if (!RebuildFromDesktop(true)) {
                        SetTimer(hwnd, kTimerRefresh, 2000, nullptr);
                    }
                    return 0;
            }
            break;

        case WM_MOUSEMOVE:
            if (!g_trackingMouse) {
                TRACKMOUSEEVENT track{};
                track.cbSize = sizeof(track);
                track.dwFlags = TME_LEAVE;
                track.hwndTrack = hwnd;
                if (TrackMouseEvent(&track)) g_trackingMouse = true;
            }
            EnsureAnimationTimer();
            return 0;

        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            EnsureAnimationTimer();
            return 0;

        case WM_LBUTTONUP: {
            DWORD now = GetTickCount();
            POINT point{};
            GetCursorPos(&point);
            int index = ItemAtCursor(point);
            if (now - g_lastLaunchTick < 450) return 0;
            if (index >= 0) {
                g_lastLaunchTick = now;
                LaunchItem(g_items[index]);
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            POINT point{};
            GetCursorPos(&point);
            int index = ItemAtCursor(point);
            if (index >= 0) {
                ShowItemContextMenu(index, point);
            }
            return 0;
        }

        case WM_SETCURSOR: {
            if (LOWORD(lParam) == HTCLIENT) {
                POINT point{};
                GetCursorPos(&point);
                int index = ItemAtCursor(point);
                SetCursor(LoadCursorW(nullptr, index >= 0 ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            break;
        }

        case WM_INITMENUPOPUP:
            if (g_activeMenu2) {
                g_activeMenu2->HandleMenuMsg(message, wParam, lParam);
                return 0;
            }
            break;

        case WM_DRAWITEM:
        case WM_MEASUREITEM:
            if (g_activeMenu2) {
                g_activeMenu2->HandleMenuMsg(message, wParam, lParam);
                return 0;
            }
            break;

        case WM_MENUCHAR:
            if (g_activeMenu3) {
                LRESULT result = 0;
                if (SUCCEEDED(
                        g_activeMenu3->HandleMenuMsg2(message, wParam, lParam, &result))) {
                    return result;
                }
            }
            break;

        case WM_DISPLAYCHANGE:
            PositionWindow();
            CaptureBackdrop();
            RenderFrame();
            return 0;

        case WM_SETTINGCHANGE:
            if (wParam == SPI_SETWORKAREA) {
                PositionWindow();
                CaptureBackdrop();
                RenderFrame();
            }
            return 0;

        case WM_DPICHANGED: {
            // Icons are extracted at a DPI-scaled size, so the scale has to
            // be updated before the rebuild re-extracts them.
            UINT dpi = GetDpiForWindow(hwnd);
            g_dpiScale = (dpi ? dpi : 96) / 96.0;
            g_iconCache.clear();
            if (!RebuildFromDesktop(true)) {
                ComputeLayout();
                g_panelWidth = g_layout.panelBaseWidth;
                for (auto& item : g_items) {
                    item.centerX = item.baseCenterX;
                    item.scale = 1.0;
                    item.labelAlpha = 0.0;
                }
                EnsureFrameBuffer(g_layout.windowWidth, g_layout.windowHeight);
                PositionWindow();
                CaptureBackdrop();
                RenderFrame();
            }
            return 0;
        }

        case WM_POWERBROADCAST:
            if (wParam == PBT_APMRESUMEAUTOMATIC) {
                CaptureBackdrop();
                RenderFrame();
            }
            return 0;

        case kMsgShellNotify:
            HandleShellNotification(wParam, lParam);
            return 0;

        case kMsgReloadSettings:
            ReloadSettings();
            return 0;

        case kMsgQuit:
            PostQuitMessage(0);
            return 0;

        case WM_NCDESTROY:
            g_hwnd = nullptr;
            break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void DestroyFrameBuffer() {
    if (g_frame.memDc) {
        g_frame.bitmap.reset();
        if (g_frame.oldBitmap) SelectObject(g_frame.memDc, g_frame.oldBitmap);
        if (g_frame.dib) DeleteObject(g_frame.dib);
        DeleteDC(g_frame.memDc);
    }
    g_frame = FrameBuffer{};
}

int RunDockThread() {
    while (!g_stopRequested) {
        DWORD shellPid = 0;
        HWND shellWindow = GetShellWindow();
        if (shellWindow) GetWindowThreadProcessId(shellWindow, &shellPid);
        if (shellPid == GetCurrentProcessId()) break;
        Sleep(500);
    }
    if (g_stopRequested) return 0;

    HRESULT oleInit = OleInitialize(nullptr);
    if (FAILED(oleInit)) {
        Wh_Log(L"OleInitialize failed: 0x%08X", oleInit);
        return 1;
    }

    GdiplusStartupInput gdiplusInput;
    if (GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr) != Ok) {
        Wh_Log(L"GdiplusStartup failed");
        OleUninitialize();
        return 2;
    }
    g_gdiplusStarted = true;

    g_instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = DockWndProc;
    windowClass.hInstance = g_instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = kWindowClass;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassEx failed: %lu", GetLastError());
        GdiplusShutdown(g_gdiplusToken);
        OleUninitialize();
        return 3;
    }
    g_classRegistered = true;

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWindowClass, L"",
        WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, g_instance, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed: %lu", GetLastError());
        UnregisterClassW(kWindowClass, g_instance);
        g_classRegistered = false;
        GdiplusShutdown(g_gdiplusToken);
        OleUninitialize();
        return 4;
    }
    g_hwnd = hwnd;
    g_screenDc = GetDC(nullptr);
    UINT dpi = GetDpiForWindow(hwnd);
    g_dpiScale = dpi ? dpi / 96.0 : 1.0;
    CreateTextures();

    LoadSettings();
    g_lastItemCheckTime = std::chrono::steady_clock::now();
    RegisterShellNotifications();
    if (!RebuildFromDesktop(true)) {
        SetTimer(hwnd, kTimerRefresh, 1000, nullptr);
    }

    if (g_stopRequested) PostQuitMessage(0);

    SetTimer(hwnd, kTimerMaintenance, 1000, nullptr);

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (g_dockShown) {
        ShowWindow(hwnd, SW_HIDE);
    }
    KillTimer(hwnd, kTimerAnimation);
    KillTimer(hwnd, kTimerMaintenance);
    KillTimer(hwnd, kTimerRefresh);
    KillTimer(hwnd, kTimerHoverPoll);
    KillTimer(hwnd, kTimerShellNotify);
    g_hoverPollRunning = false;

    UnregisterShellNotifications();
    SetDesktopIconsHidden(false);
    g_desktopListView = nullptr;

    g_activeMenu2.Reset();
    g_activeMenu3.Reset();
    DestroyFrameBuffer();
    g_backdrop.reset();
    g_backdropValid = false;
    g_iconCache.clear();
    g_items.clear();
    g_labelFont.reset();
    g_labelFontFamily.reset();
    g_glowTexture.reset();
    g_shadowTexture.reset();
    g_noiseTexture.reset();

    if (g_screenDc) {
        ReleaseDC(nullptr, g_screenDc);
        g_screenDc = nullptr;
    }

    if (hwnd && IsWindow(hwnd)) DestroyWindow(hwnd);
    if (g_classRegistered) {
        UnregisterClassW(kWindowClass, g_instance);
        g_classRegistered = false;
    }

    if (g_gdiplusStarted) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusStarted = false;
    }
    OleUninitialize();
    return 0;
}

DWORD WINAPI DockThreadProc(LPVOID) {
    return (DWORD)RunDockThread();
}

}  // namespace

BOOL Wh_ModInit() {
    g_stopRequested = false;
    HANDLE thread = CreateThread(nullptr, 0, DockThreadProc, nullptr, 0, nullptr);
    if (!thread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        return FALSE;
    }
    g_thread = thread;
    return TRUE;
}

void Wh_ModUninit() {
    g_stopRequested = true;
    HWND hwnd = g_hwnd.load();
    if (hwnd) PostMessageW(hwnd, kMsgQuit, 0, 0);
    HANDLE thread = g_thread.exchange(nullptr);
    if (thread) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

void Wh_ModSettingsChanged() {
    HWND hwnd = g_hwnd.load();
    if (hwnd) PostMessageW(hwnd, kMsgReloadSettings, 0, 0);
}
