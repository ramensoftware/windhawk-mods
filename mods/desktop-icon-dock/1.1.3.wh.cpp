// ==WindhawkMod==
// @id              desktop-icon-dock
// @name            Desktop Icon Dock
// @name:zh-CN      桌面图标 Dock
// @description     A standalone macOS-style dock: drag shortcuts, files and folders onto it, reorder them, drag them out to put them back on the desktop; shortcuts are copied into a dock-owned folder so desktop cleanups do not affect the dock; optional glass panel
// @description:zh-CN 独立的 macOS 风格 Dock：把快捷方式、文件和文件夹拖进来，拖动排序，拖出放回桌面；快捷方式会复制到 Dock 自己的目录，清理桌面不影响 Dock；可选液态玻璃面板
// @version         1.1.3
// @author          yizqq0721
// @github          https://github.com/yizqq0721
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdiplus -lgdi32 -luser32 -lole32 -loleaut32 -luuid -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icon Dock

Turns shortcuts, files and folders that you drag onto it into a centered
macOS-style dock. The dock is an independent list, not a mirror of the
desktop. A fully transparent background is used by default, showing only the
icons; an optional Liquid Glass panel is available.

![Desktop Icon Dock preview](https://raw.githubusercontent.com/yizqq0721/desktop-icon-dock/main/images/dock-preview-transparent.jpg)

*Example: icons arranged as a centered macOS-style dock; the hovered icon is
magnified with its label shown.*

## Features

- **Drag to add**: drag a shortcut, file or folder from the desktop, File
  Explorer or a `shell:Desktop` window onto the dock. It is inserted at the
  drop position; items that are already in the dock are ignored. `.lnk` and
  `.url` shortcuts are copied into a dock-owned folder, so deleting the
  original shortcut from the desktop leaves the dock item intact; files,
  folders and virtual items are referenced by path.
- **Drag to reorder**: hold the left mouse button on an icon and drag it
  within the dock. Icons rearrange live and the new order is saved when you
  release the button.
- **Drag out to put back on the desktop**: drag an item outside the dock and
  release it over the desktop. The icon is placed at the release point. If the
  item already lives on the desktop, its original icon is moved there;
  otherwise a shortcut is created on the desktop and the original file or
  folder stays where it is. For a shortcut that was copied into the dock, the
  original shortcut is used, so dragging it out does not create a second
  `(2)` copy. Releasing over any other window leaves the item in the dock.
  Items that have no filesystem location, such as the Recycle Bin, are only
  removed from the dock. A dock-owned shortcut copy is removed after it has
  been placed back. Removing the last item brings back the empty-state hint.
- **Empty state**: a visible drop target is shown when the list is empty, so
  the dock can always receive the first item. The window is never hidden just
  because the list is empty.
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
- **Independent from the desktop**: adding, removing, renaming or moving
  desktop icons does not change the dock. Shortcuts added to the dock are
  copied into its own folder, so even deleting them from the desktop does not
  affect the dock; files, folders and virtual items are referenced by path.
  The list is stored per user in Windhawk's mod storage and survives Explorer
  restarts.
- **Original desktop icons hidden in memory (optional)**: when enabled, the
  desktop's list view is hidden while the dock is active; no Explorer setting
  or registry value is changed, and the icons are shown again when the mod is
  unloaded. Off by default, so the desktop remains available as a drag source.
- **Icon cache with cleanup**: icons are cached per item path and reused;
  virtual items such as the Recycle Bin refresh their icons periodically.

## How it works

The dock keeps its own ordered list. It does not read or rearrange the system
desktop icons.

- **Storage**: the ordered list is saved as `items.tsv` in Windhawk's mod
  storage, in a subfolder named after the current user's security identifier
  (SID), so each Windows user keeps a separate list. Each entry is a
  desktop-absolute parsing name, such as a full file path or a shell GUID for
  virtual items, followed by the original path for copied shortcuts. Shortcut
  copies live in the `Items` subfolder of the same directory. Every add,
  reorder and removal is written immediately. Windhawk removes the whole
  storage folder when the mod is uninstalled.
- **Adding**: external drags are received through an OLE drop target
  (`IDropTarget`), supporting file drops and shell item lists. The insertion
  position follows the pointer; `.lnk` and `.url` shortcuts are copied into
  the dock folder first. Duplicates are skipped by parsing name. The drop is
  processed after the drop has been accepted, so a slow or unreachable path
  does not freeze the drag source.
- **Reordering and removing**: these are handled inside the dock window with
  mouse capture, so they also work in the transparent gaps between icons.
- **Putting items back on the desktop**: releasing outside the panel over the
  desktop places the item at the release point; releasing over any other
  window snaps it back into the dock. An item whose parent folder is already
  the user or public desktop has its original icon repositioned through the
  desktop's folder view; for a copied shortcut the original shortcut path is
  used. Any other file or folder gets a uniquely named `.lnk` shortcut on the
  user desktop (a source `.lnk` or `.url` is copied, other items get a new
  shortcut). The original file is never moved or deleted, and a dock-owned
  shortcut copy is removed after it has been placed back. Positioning follows
  the system "align icons to grid" setting.
- **Empty state**: the centered hint pill is drawn with a non-transparent
  background and is registered as part of the same drop target.
- **Layout**: centered horizontally at the bottom of the primary monitor,
  above the taskbar; the panel widens as items are added, and icons are
  vertically centered at rest.
- **Maintenance**: every 30 seconds, entries that failed to resolve earlier
  are retried and the icons of virtual items are refreshed. Items that still
  cannot be resolved are skipped on screen but kept in storage.
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
| Hide original desktop icons | Off | Hide the system desktop icons while the dock is active by hiding the desktop list view in memory; they are shown again when the mod is unloaded |
| Backdrop refresh interval (s) | 20 | How often the blurred backdrop (used by the glass panel and the label pill) is re-sampled; 0 disables refresh |

## How it differs from existing mods

This mod overlaps with Quick Launch Dock in the basic launcher behavior:
drag in, reorder, click to launch, a persistent list. It exists separately
because it targets a different use case: a dock that is part of the desktop,
not a launcher that stays on top of other windows.

- **Desktop level**: the window sits at the bottom of the Z-order, below all
  normal windows, and never takes focus. It is visible when the desktop is
  visible, like the desktop icons themselves.
- **Drag out to the desktop**: items can be dragged from the dock back to the
  desktop, reusing the original shortcut when it is still there instead of
  creating a duplicate.
- **Desktop integration**: hiding the system desktop icons in memory and
  opening the native Explorer context menu both work with the shell's desktop
  view from the mod's own process, without injecting into the running shell.
- **Screen-sampled glass**: the backdrop and the name pills sample the screen
  area behind the dock, which requires a desktop-level window.

## Compatibility and limitations

- Windows 10 / 11 on x86-64 only. The mod runs as a Windhawk tool mod in a
  dedicated explorer.exe process, separate from the shell's own explorer.exe;
  the shell is only accessed through its published shell interfaces. (The
  windhawk.exe host cannot be used here: Windhawk is a 32-bit app, while this
  mod is x86-64.)
- Primary monitor only; the dock is anchored to the bottom of the primary
  monitor, above the taskbar.
- The dock is a layered window placed below normal windows; full-screen apps
  and other windows cover it, which is expected.
- In transparent mode, fully transparent pixels are click-through; the icons
  themselves remain clickable.

## Troubleshooting

- **After an Explorer restart**: the dock runs in its own dedicated
  explorer.exe process, so a shell restart does not unload it; it reconnects to
  the new desktop view and reapplies the icon hiding.
- **An item disappeared**: its stored parsing name could not be resolved (for
  example, the referenced file or folder was deleted or renamed). Shortcuts
  are copied into the dock folder and are not affected by deleting the
  desktop shortcut. The entry is kept in storage and retried; it reappears if
  the path is available again.
- **After dragging an item out, a shortcut appeared on the desktop**: this is
  expected. Items that are not on the desktop are not moved; a shortcut is
  created at the release point instead. The original file or folder is not
  affected, and the shortcut can be deleted at any time.
- **Settings changes seem to have no effect**: saving settings reloads and
  rebuilds the dock automatically; no Explorer restart is needed.
- **Add an item**: drag it from the desktop or any File Explorer window onto
  the dock. You can also press Win+R and enter `shell:Desktop` to open a
  desktop folder and drag from there.
- **Reset the list**: drag the items out, or uninstall the mod. Windhawk
  deletes the mod's storage, including the list and the shortcut copies, when
  the mod is removed.

## 中文说明

本模组把快捷方式、文件和文件夹变成一个居中的 macOS 风格 Dock，默认使用全透明背景；关闭“透明背景”后会显示液态玻璃面板。

- 从桌面、资源管理器或 `shell:Desktop` 窗口把项目拖到 Dock 上即可添加，落点决定插入位置，重复项目会被忽略。快捷方式（`.lnk` / `.url`）会复制到 Dock 自己的目录，删除桌面上的原快捷方式不影响 Dock；文件和文件夹按路径引用。
- 按住图标在 Dock 内拖动可实时排序；拖到 Dock 外、在桌面上松手会把项目放回桌面，图标落在松手位置。项目原本就在桌面上时移动原图标；否则在桌面新建指向它的快捷方式，原文件或文件夹保持原位；对于复制进来的快捷方式会优先使用原快捷方式，不会产生多余的 `(2)` 副本；在其他窗口上松手则项目留在 Dock。回收站等虚拟项目仅从 Dock 移除；Dock 副本在放回桌面后删除。移除最后一项后显示空状态提示。
- Dock 是独立列表，不同步桌面；列表保存在 Windhawk 模组存储的 `items.tsv` 中，并按当前用户 SID 分目录，顺序即显示顺序，重启 Explorer 后保持；快捷方式副本位于同一目录的 `Items` 子目录。卸载模组时 Windhawk 会删除整个存储目录。
- 图标支持悬停放大、相邻图标让位和名称标签；左键打开，右键打开资源管理器原生菜单。
- “隐藏原始桌面图标”默认关闭；开启后仅在内存中隐藏桌面列表视图，卸载模组后自动恢复。
- 解析失败的项目会跳过显示但保留在存储中，每 30 秒只重试此前失败的项目；回收站等虚拟项目的图标会定期刷新。
- 与 Quick Launch Dock 等启动器 Dock 的区别：本模组位于桌面层级（所有普通窗口之下）、不抢焦点，支持把项目拖回桌面并复用原快捷方式，还提供隐藏系统桌面图标和屏幕采样玻璃背景；这些行为通过 Shell 桌面视图完成，模组本身运行在独立的 explorer.exe tool-mod 进程中，与正在运行的 Shell 进程分离（Windhawk 主程序为 32 位，无法承载本 x86-64 模组）。
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
- HideDesktopIcons: false
  $name: Hide original desktop icons
  $name:zh-CN: 隐藏原始桌面图标
  $description: Hide the original desktop icons while the dock is active by hiding the desktop list view in memory; restored on unload. Off by default so the desktop stays available as a drag source
  $description:zh-CN: 启用 Dock 时在内存中隐藏桌面列表视图，不修改系统设置；卸载时恢复。默认关闭，便于随时从桌面拖入项目
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
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <map>
#include <memory>
#include <new>
#include <set>
#include <string>
#include <vector>
#include <windhawk_utils.h>
#include <oleauto.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <exdisp.h>
#include <shellapi.h>
#include <sddl.h>
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
constexpr wchar_t kItemsFileName[] = L"items.tsv";
constexpr wchar_t kItemsHeader[] = L"DesktopIconDock/2";
constexpr UINT kMsgReloadSettings = WM_APP + 1;
constexpr UINT kMsgQuit = WM_APP + 3;
constexpr UINT kMsgAddDropped = WM_APP + 4;
constexpr UINT_PTR kTimerAnimation = 1;
constexpr UINT_PTR kTimerMaintenance = 2;
constexpr UINT_PTR kTimerRefresh = 3;
constexpr UINT_PTR kTimerHoverPoll = 5;
constexpr int kHoverPollMs = 50;
constexpr int kIconRequestPx = 256;
constexpr int kMaintenanceSeconds = 30;
constexpr int kDragThresholdPx = 5;
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
    int hideDesktopIcons = 0;
    int backdropRefreshSeconds = 20;
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
    std::wstring filePath;
    std::shared_ptr<void> pidlOwner;
    PIDLIST_ABSOLUTE pidl = nullptr;
    std::shared_ptr<Bitmap> icon;
    size_t storedIndex = 0;
    bool isFilesystem = false;
    double baseCenterX = 0.0;
    double centerX = 0.0;
    double scale = 1.0;
    double labelAlpha = 0.0;
};

std::vector<DockItem> g_items;
std::vector<std::wstring> g_storedKeys;
std::map<std::wstring, std::shared_ptr<Bitmap>> g_iconCache;

// Windhawk's storage is shared between users, so the list and the shortcut
// copies live in a per-user subfolder.
std::wstring g_userStorageRoot;
std::wstring g_itemsFolderPath;
std::wstring g_itemsFilePath;
// Original shortcut path of each dock copy, keyed by the lowercased copy path.
std::map<std::wstring, std::wstring> g_shortcutSources;
std::set<std::wstring> g_unresolvedKeys;

struct PendingDrop {
    std::vector<std::wstring> keys;
    int index = 0;
};
std::vector<PendingDrop> g_pendingDrops;

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

bool g_leftDown = false;
bool g_dragCandidate = false;
bool g_internalDragActive = false;
bool g_dragOutside = false;
int g_dragIndex = -1;
std::wstring g_dragKey;
POINT g_leftDownPoint{};

bool g_externalDragActive = false;
int g_dropInsertIndex = 0;

bool g_dockShown = false;
bool g_hideIconsRequested = false;
bool g_iconsHiddenByUs = false;
bool g_menuOpen = false;
bool g_gdiplusStarted = false;
ULONG_PTR g_gdiplusToken = 0;
bool g_classRegistered = false;
HWND g_desktopListView = nullptr;

class DockDropTarget;
DockDropTarget* g_dropTarget = nullptr;

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
std::unique_ptr<FontFamily> g_emptyPrimaryFamily;
std::unique_ptr<Font> g_emptyPrimaryFont;
std::unique_ptr<FontFamily> g_emptySecondaryFamily;
std::unique_ptr<Font> g_emptySecondaryFont;

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

bool KeyEqualsNoCase(const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

bool ContainsKeyNoCase(const std::vector<std::wstring>& keys,
                       const std::wstring& key) {
    for (const auto& existing : keys) {
        if (KeyEqualsNoCase(existing, key)) return true;
    }
    return false;
}

std::wstring GetCurrentUserSidString() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        Wh_Log(L"OpenProcessToken failed: %lu", GetLastError());
        return L"";
    }

    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    std::vector<BYTE> buffer(size);
    std::wstring sid;
    if (size > 0 &&
        GetTokenInformation(token, TokenUser, buffer.data(), size, &size)) {
        auto* user = reinterpret_cast<TOKEN_USER*>(buffer.data());
        LPWSTR text = nullptr;
        if (ConvertSidToStringSidW(user->User.Sid, &text)) {
            sid = text;
            LocalFree(text);
        }
    }
    CloseHandle(token);
    if (sid.empty()) Wh_Log(L"Reading the user SID failed: %lu", GetLastError());
    return sid;
}

// All mod data lives in Windhawk's per-mod storage, which is removed together
// with the mod. Shortcut copies and the list are separated per user because a
// regular Windhawk install shares the storage folder between all users.
void EnsureStoragePaths() {
    if (!g_itemsFolderPath.empty()) return;

    wchar_t storage[MAX_PATH * 4] = L"";
    size_t length = Wh_GetModStoragePath(storage, ARRAYSIZE(storage));
    if (length == 0 || length >= ARRAYSIZE(storage)) {
        Wh_Log(L"Getting the mod storage path failed");
        return;
    }

    std::wstring sid = GetCurrentUserSidString();
    if (sid.empty()) sid = L"default";
    g_userStorageRoot = std::wstring(storage) + L"\\" + sid;
    int result =
        SHCreateDirectoryExW(nullptr, g_userStorageRoot.c_str(), nullptr);
    if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS) {
        Wh_Log(L"Creating the mod storage folder failed: %d", result);
        g_userStorageRoot.clear();
        return;
    }
    g_itemsFolderPath = g_userStorageRoot + L"\\Items";
    g_itemsFilePath = g_userStorageRoot + L"\\" + kItemsFileName;
    Wh_Log(L"Mod storage folder: %ls", g_userStorageRoot.c_str());
}

bool ReadFileBytes(const std::wstring& path, std::vector<BYTE>& data) {
    data.clear();
    HANDLE file = CreateFileW(
        path.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file, &size) || size.QuadPart < 0 ||
        size.QuadPart > 32 * 1024 * 1024) {
        CloseHandle(file);
        return false;
    }
    if (size.QuadPart > 0) {
        data.resize((size_t)size.QuadPart);
        DWORD read = 0;
        BOOL ok =
            ReadFile(file, data.data(), (DWORD)data.size(), &read, nullptr);
        if (!ok || read != data.size()) {
            data.clear();
            CloseHandle(file);
            return false;
        }
    }
    CloseHandle(file);
    return true;
}

bool WriteFileBytesAtomic(const std::wstring& path,
                          const std::vector<BYTE>& data) {
    std::wstring temp = path + L".tmp";
    HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Creating dock storage file failed: %lu", GetLastError());
        return false;
    }
    DWORD written = 0;
    BOOL ok = WriteFile(file, data.data(), (DWORD)data.size(), &written,
                        nullptr) &&
              written == data.size();
    if (ok) FlushFileBuffers(file);
    CloseHandle(file);
    if (!ok) {
        Wh_Log(L"Writing dock storage file failed: %lu", GetLastError());
        DeleteFileW(temp.c_str());
        return false;
    }
    if (!MoveFileExW(temp.c_str(), path.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"Replacing dock storage file failed: %lu", GetLastError());
        DeleteFileW(temp.c_str());
        return false;
    }
    return true;
}

// Each line is the parsing name, a tab and the original shortcut path. The
// second field is empty for plain references and virtual items.
std::vector<BYTE> SerializeDockList(const std::vector<std::wstring>& keys) {
    std::wstring text = kItemsHeader;
    text += L"\r\n";
    for (const auto& key : keys) {
        text += key;
        text += L'\t';
        auto it = g_shortcutSources.find(ToLowerString(key));
        if (it != g_shortcutSources.end()) text += it->second;
        text += L"\r\n";
    }

    std::vector<BYTE> bytes;
    bytes.reserve(2 + text.size() * sizeof(wchar_t));
    bytes.push_back(0xFF);
    bytes.push_back(0xFE);
    const BYTE* raw = reinterpret_cast<const BYTE*>(text.data());
    bytes.insert(bytes.end(), raw, raw + text.size() * sizeof(wchar_t));
    return bytes;
}

bool ParseDockList(const std::vector<BYTE>& bytes,
                   std::vector<std::wstring>& keys,
                   std::map<std::wstring, std::wstring>& sources) {
    keys.clear();
    sources.clear();
    if (bytes.size() < 2 || (bytes.size() % 2) != 0) return false;

    size_t offset = 0;
    if (bytes[0] == 0xFF && bytes[1] == 0xFE) offset = 2;
    size_t count = (bytes.size() - offset) / sizeof(wchar_t);
    const wchar_t* raw =
        reinterpret_cast<const wchar_t*>(bytes.data() + offset);
    std::wstring content(raw, count);
    if (!content.empty() && content[0] == 0xFEFF) content.erase(0, 1);

    size_t pos = 0;
    bool first = true;
    while (pos <= content.size()) {
        size_t end = content.find(L'\n', pos);
        std::wstring line =
            content.substr(pos, end == std::wstring::npos ? std::wstring::npos
                                                          : end - pos);
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (first) {
            if (line != kItemsHeader) return false;
            first = false;
        } else if (!line.empty()) {
            size_t tab = line.find(L'\t');
            std::wstring key =
                tab == std::wstring::npos ? line : line.substr(0, tab);
            std::wstring source =
                tab == std::wstring::npos ? L"" : line.substr(tab + 1);
            if (!key.empty()) {
                keys.push_back(key);
                if (!source.empty()) {
                    sources[ToLowerString(key)] = source;
                }
            }
        }
        if (end == std::wstring::npos) break;
        pos = end + 1;
    }
    return true;
}

std::vector<std::wstring> LoadDockKeys() {
    EnsureStoragePaths();
    std::vector<std::wstring> keys;
    if (g_itemsFilePath.empty() ||
        GetFileAttributesW(g_itemsFilePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return keys;
    }

    std::vector<BYTE> bytes;
    if (!ReadFileBytes(g_itemsFilePath, bytes)) {
        Wh_Log(L"Reading dock storage failed: %lu", GetLastError());
        return keys;
    }
    std::map<std::wstring, std::wstring> sources;
    if (!ParseDockList(bytes, keys, sources)) {
        Wh_Log(L"Invalid dock storage file: %ls", g_itemsFilePath.c_str());
        return {};
    }
    g_shortcutSources = std::move(sources);
    return keys;
}

bool SaveDockKeys(const std::vector<std::wstring>& keys) {
    EnsureStoragePaths();
    if (g_itemsFilePath.empty()) return false;
    return WriteFileBytesAtomic(g_itemsFilePath, SerializeDockList(keys));
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
    // Keep the request even if the desktop view is not available yet. The
    // maintenance timer retries until the shell has created it.
    g_hideIconsRequested = hide;

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
    if (!g_hideIconsRequested || !g_settings.hideDesktopIcons ||
        AreDesktopIconsHidden()) {
        return;
    }
    HWND listView = DesktopListView();
    if (listView && IsWindowVisible(listView)) {
        ShowWindowAsync(listView, SW_HIDE);
        g_iconsHiddenByUs = true;
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
    g_layout.glowMargin = (int)lround(10 * g_dpiScale);
    g_layout.labelFontPx = std::max(12, (int)lround(13 * g_dpiScale));
    g_layout.labelPillHeight = (int)lround(g_layout.labelFontPx * 1.9);
    g_layout.labelGap = (int)lround(8 * g_dpiScale);

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    HMONITOR monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    GetMonitorInfoW(monitor, &monitorInfo);
    int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    int widthCap = monitorWidth - (int)lround(48 * g_dpiScale);

    if (count == 0) {
        g_layout.panelHeightRest = (int)lround(84 * g_dpiScale);
        g_layout.panelHeightMax = g_layout.panelHeightRest;
        g_layout.panelHeight = g_layout.panelHeightRest;
        g_layout.panelBaseWidth =
            ClampInt((int)lround(340 * g_dpiScale), 200, std::max(200, widthCap));
        g_layout.panelMaxWidth = g_layout.panelBaseWidth;
        g_layout.labelAreaHeight = 0;
        g_layout.windowWidth = g_layout.panelMaxWidth + 2 * g_layout.glowMargin;
        g_layout.windowHeight = g_layout.glowMargin + g_layout.panelHeight +
                                g_layout.glowMargin;
        g_layout.panelTop = g_layout.glowMargin;
        g_layout.panelBottom = g_layout.panelTop + g_layout.panelHeight;
        g_layout.iconBottom = g_layout.panelBottom - g_layout.panelPadY;
        return;
    }

    int iconArea = (int)ceil(g_layout.iconPx * g_layout.maxScale);
    int liftMax = (int)ceil(4.0 * g_dpiScale);
    g_layout.panelHeightRest = g_layout.iconPx + 2 * g_layout.panelPadY;
    g_layout.panelHeightMax = iconArea + liftMax + 2 * g_layout.panelPadY;
    g_layout.panelHeight = g_layout.panelHeightMax;

    int baseContent = count * g_layout.iconPx + (count - 1) * g_layout.spacingPx;
    g_layout.panelBaseWidth = baseContent + 2 * g_layout.panelPadX;

    int affected = std::min(count, 5);
    double maxExpansion = (g_layout.maxScale - 1.0) * g_layout.iconPx *
                          g_layout.spacingFactor * affected;
    g_layout.panelMaxWidth =
        (int)ceil(g_layout.panelBaseWidth + maxExpansion + 10 * g_dpiScale);
    if (g_layout.panelMaxWidth > widthCap) {
        g_layout.panelMaxWidth = std::max(g_layout.panelBaseWidth, widthCap);
    }

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

bool g_emptyHintChinese = false;

void EnsureEmptyFonts() {
    if (g_emptyPrimaryFont) return;
    g_emptyHintChinese =
        PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_CHINESE;
    auto primaryFamily = std::make_unique<FontFamily>(
        g_emptyHintChinese ? L"Microsoft YaHei UI" : L"Segoe UI");
    if (primaryFamily->GetLastStatus() != Ok || !primaryFamily->IsAvailable()) {
        primaryFamily = std::make_unique<FontFamily>(L"Segoe UI");
    }
    g_emptyPrimaryFont = std::make_unique<Font>(
        primaryFamily.get(), (REAL)lround(16 * g_dpiScale), FontStyleRegular,
        UnitPixel);
    if (g_emptyPrimaryFont->GetLastStatus() != Ok) {
        g_emptyPrimaryFont.reset();
        return;
    }
    g_emptyPrimaryFamily = std::move(primaryFamily);

    auto secondaryFamily = std::make_unique<FontFamily>(L"Segoe UI Variable Text");
    if (secondaryFamily->GetLastStatus() != Ok || !secondaryFamily->IsAvailable()) {
        secondaryFamily = std::make_unique<FontFamily>(L"Segoe UI");
    }
    g_emptySecondaryFont = std::make_unique<Font>(
        secondaryFamily.get(), (REAL)lround(10.5 * g_dpiScale), FontStyleRegular,
        UnitPixel);
    if (g_emptySecondaryFont->GetLastStatus() != Ok) {
        g_emptySecondaryFont.reset();
        g_emptyPrimaryFont.reset();
        g_emptyPrimaryFamily.reset();
        return;
    }
    g_emptySecondaryFamily = std::move(secondaryFamily);
}

void DrawEmptyState(Graphics& graphics, const RECT& panelRect) {
    RectF panel((REAL)panelRect.left, (REAL)panelRect.top,
                (REAL)(panelRect.right - panelRect.left),
                (REAL)(panelRect.bottom - panelRect.top));
    if (panel.Width < 8.0f || panel.Height < 8.0f) return;

    EnsureEmptyFonts();
    GraphicsPath panelPath;
    AddRoundedRect(panelPath, panel, panel.Height * 0.30f);

    for (int i = 3; i >= 1; --i) {
        double expand = i * 1.4 * g_dpiScale;
        RectF shadowRect(panel.X - (REAL)expand, panel.Y - (REAL)(expand * 0.3) + 2,
                         panel.Width + (REAL)expand * 2,
                         panel.Height + (REAL)expand * 2);
        GraphicsPath shadowPath;
        AddRoundedRect(shadowPath, shadowRect,
                       (float)(panel.Height * 0.30 + expand));
        SolidBrush shadow(Color((BYTE)(9 * i), 0, 0, 0));
        graphics.FillPath(&shadow, &shadowPath);
    }

    bool glass = !g_settings.transparentBackground;
    bool active = g_externalDragActive;
    Color fill = glass
                     ? Color(active ? 165 : 120, 248, 250, 254)
                     : Color(active ? 82 : 46, 18, 20, 26);
    SolidBrush fillBrush(fill);
    graphics.FillPath(&fillBrush, &panelPath);

    Pen borderPen(glass ? Color((BYTE)(active ? 170 : 130), 35, 40, 50)
                        : Color((BYTE)(active ? 235 : 155), 255, 255, 255),
                 (REAL)std::max(1.0, 1.4 * g_dpiScale));
    borderPen.SetDashStyle(DashStyleDash);
    graphics.DrawPath(&borderPen, &panelPath);

    if (!g_emptyPrimaryFont || !g_emptySecondaryFont) return;

    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    format.SetFormatFlags(StringFormatFlagsNoWrap);
    format.SetTrimming(StringTrimmingEllipsisCharacter);

    double padX = 16.0 * g_dpiScale;
    RectF primaryRect(panel.X + (REAL)padX,
                      panel.Y + panel.Height * 0.16f,
                      panel.Width - (REAL)(padX * 2.0),
                      panel.Height * 0.38f);
    RectF secondaryRect(panel.X + (REAL)padX,
                        panel.Y + panel.Height * 0.50f,
                        panel.Width - (REAL)(padX * 2.0),
                        panel.Height * 0.30f);

    Color primaryColor = glass ? Color(232, 30, 32, 38)
                               : Color(238, 255, 255, 255);
    Color secondaryColor = glass ? Color(165, 42, 46, 54)
                                 : Color(172, 255, 255, 255);
    SolidBrush primaryBrush(primaryColor);
    SolidBrush secondaryBrush(secondaryColor);
    const wchar_t* primaryText =
        g_emptyHintChinese ? L"将图标拖到这里" : L"Drag items here";
    const wchar_t* secondaryText = g_emptyHintChinese
                                       ? L"拖动快捷方式、文件或文件夹到这里"
                                       : L"Shortcuts, files and folders";
    graphics.DrawString(primaryText, -1, g_emptyPrimaryFont.get(), primaryRect,
                        &format, &primaryBrush);
    graphics.DrawString(secondaryText, -1, g_emptySecondaryFont.get(),
                        secondaryRect, &format, &secondaryBrush);
}

void DrawDropBackdrop(Graphics& graphics, const RECT& panelRect) {
    if (!g_externalDragActive) return;
    RectF panel((REAL)panelRect.left, (REAL)panelRect.top,
                (REAL)(panelRect.right - panelRect.left),
                (REAL)(panelRect.bottom - panelRect.top));
    if (panel.Width < 8.0f || panel.Height < 8.0f) return;
    float radius = (float)(g_layout.panelHeightRest * 0.36);
    GraphicsPath path;
    AddRoundedRect(path, panel, radius);
    if (g_settings.transparentBackground) {
        // Any non-zero alpha makes the whole panel a valid OLE drop target,
        // including the otherwise click-through gaps between icons.
        SolidBrush fill(Color(24, 255, 255, 255));
        graphics.FillPath(&fill, &path);
    }
    Pen border(Color(150, 120, 190, 255),
               (REAL)std::max(1.0, 1.4 * g_dpiScale));
    border.SetDashStyle(DashStyleDash);
    graphics.DrawPath(&border, &path);
}

void DrawDropIndicator(Graphics& graphics) {
    if (!g_externalDragActive || g_items.empty()) return;
    int count = (int)g_items.size();
    int index = ClampInt(g_dropInsertIndex, 0, count);
    double pitch = g_layout.iconPx + g_layout.spacingPx;
    double x = 0.0;
    if (index <= 0) {
        x = g_items.front().baseCenterX - pitch * 0.5;
    } else if (index >= count) {
        x = g_items.back().baseCenterX + pitch * 0.5;
    } else {
        x = (g_items[index - 1].baseCenterX + g_items[index].baseCenterX) * 0.5;
    }

    double width = std::max(2.0, 3.0 * g_dpiScale);
    double top = g_layout.iconBottom - g_layout.iconPx - 4.0 * g_dpiScale;
    double height = g_layout.iconPx + 8.0 * g_dpiScale;
    RectF bar((REAL)(x - width * 0.5), (REAL)top, (REAL)width, (REAL)height);
    GraphicsPath path;
    AddRoundedRect(path, bar, (float)(width * 0.5));
    SolidBrush glow(Color(55, 120, 190, 255));
    RectF glowRect(bar.X - 3.0f * (REAL)g_dpiScale, bar.Y,
                   bar.Width + 6.0f * (REAL)g_dpiScale, bar.Height);
    GraphicsPath glowPath;
    AddRoundedRect(glowPath, glowRect, (float)(glowRect.Width * 0.5));
    graphics.FillPath(&glow, &glowPath);
    SolidBrush brush(Color(245, 130, 200, 255));
    graphics.FillPath(&brush, &path);
}

void DrawIcons(Graphics& graphics) {
    int index = 0;
    for (const auto& item : g_items) {
        RectF rect = IconRectFor(item);
        bool dragging = g_internalDragActive && index == g_dragIndex;
        if (dragging) {
            double size = g_layout.iconPx * 1.10;
            double lift = 6.0 * g_dpiScale;
            rect = RectF((REAL)(item.centerX - size * 0.5),
                         (REAL)(g_layout.iconBottom - size - lift), (REAL)size,
                         (REAL)size);
        }
        if (item.scale <= 0.002) {
            ++index;
            continue;
        }
        if (g_shadowTexture) {
            double shadowWidth = rect.Width * 0.92;
            double shadowHeight = rect.Height * (dragging ? 0.36 : 0.30);
            RectF shadowRect(rect.X + (rect.Width - (REAL)shadowWidth) * 0.5f,
                             rect.Y + rect.Height - (REAL)(shadowHeight * 0.42),
                             (REAL)shadowWidth, (REAL)shadowHeight);
            DrawTextureAlpha(graphics, g_shadowTexture.get(), shadowRect,
                             dragging ? 0.55 : 0.42);
        }
        if (item.icon) {
            if (dragging && g_dragOutside) {
                DrawTextureAlpha(graphics, item.icon.get(), rect, 0.35);
            } else {
                graphics.DrawImage(item.icon.get(), rect);
            }
        } else {
            SolidBrush placeholder(Color((BYTE)(dragging && g_dragOutside ? 35 : 90),
                                         255, 255, 255));
            GraphicsPath path;
            AddRoundedRect(path, rect, rect.Width * 0.22f);
            graphics.FillPath(&placeholder, &path);
        }
        ++index;
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
        RECT panelRect = PanelClientRect();
        DrawEmptyState(graphics, panelRect);
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
    if (g_externalDragActive) {
        RECT panelRect = PanelClientRect();
        DrawDropBackdrop(graphics, panelRect);
    }
    DrawIcons(graphics);
    if (g_externalDragActive) {
        DrawDropIndicator(graphics);
    }
    if (g_settings.showLabels && !g_externalDragActive && !g_internalDragActive) {
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
    bool wantPoll =
        g_settings.transparentBackground && g_dockShown && !g_items.empty();
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
    bool dragging = g_internalDragActive || g_externalDragActive;
    bool inside = !dragging && IsPointerOverDock(screenPoint);

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
    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    info.lpVerb = L"open";
    info.lpIDList = item.pidl;
    info.lpFile = item.pidl ? nullptr : item.key.c_str();
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
            if (KeyEqualsNoCase(item.key, it->first)) {
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

struct ResolvedDockItem {
    std::wstring name;
    std::wstring key;
    std::wstring filePath;
    std::shared_ptr<void> pidlOwner;
    PIDLIST_ABSOLUTE pidl = nullptr;
    ComPtr<IShellItem> shellItem;
    bool isFilesystem = false;
    size_t storedIndex = 0;
};

bool ResolveDockItem(const std::wstring& storedKey, size_t storedIndex,
                     ResolvedDockItem& out) {
    ComPtr<IShellItem> item;
    HRESULT hr = SHCreateItemFromParsingName(storedKey.c_str(), nullptr,
                                             IID_PPV_ARGS(item.put()));
    if (FAILED(hr) || !item) {
        Wh_Log(L"Resolving dock item failed: %ls (0x%08X)", storedKey.c_str(), hr);
        return false;
    }

    std::wstring name = GetShellItemName(item.Get(), SIGDN_NORMALDISPLAY);
    std::wstring key = GetShellItemName(item.Get(), SIGDN_DESKTOPABSOLUTEPARSING);
    if (key.empty()) key = storedKey;
    if (name.empty()) name = key;

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHGetIDListFromObject(item.Get(), &pidl))) pidl = nullptr;

    SFGAOF attributes = 0;
    bool isFilesystem = false;
    if (SUCCEEDED(item->GetAttributes(SFGAO_FILESYSTEM, &attributes))) {
        isFilesystem = (attributes & SFGAO_FILESYSTEM) != 0;
    }

    out.name = std::move(name);
    out.key = std::move(key);
    out.filePath = GetShellItemName(item.Get(), SIGDN_FILESYSPATH);
    out.pidl = pidl;
    if (pidl) {
        out.pidlOwner = std::shared_ptr<void>(
            pidl, [](void* p) { if (p) ILFree((PIDLIST_ABSOLUTE)p); });
    }
    out.shellItem = std::move(item);
    out.isFilesystem = isFilesystem;
    out.storedIndex = storedIndex;
    return true;
}

// Virtual items (Recycle Bin, This PC, ...) have no file-system events, so
// their state changes (e.g. an emptied Recycle Bin) are picked up here.
// Parsing names of virtual items are local and never touch the network.
void RefreshVirtualIconCache() {
    bool updated = false;
    for (auto& item : g_items) {
        if (item.isFilesystem) continue;
        ComPtr<IShellItem> shellItem;
        if (FAILED(SHCreateItemFromParsingName(item.key.c_str(), nullptr,
                                               IID_PPV_ARGS(shellItem.put()))) ||
            !shellItem) {
            continue;
        }
        g_iconCache.erase(item.key);
        auto icon = LoadShellIcon(shellItem.Get(), item.key);
        if (!icon) continue;
        item.icon = std::move(icon);
        updated = true;
    }
    if (updated && !g_animationTimerRunning) RenderFrame();
}

void ResetInternalDragState() {
    g_leftDown = false;
    g_dragCandidate = false;
    g_internalDragActive = false;
    g_dragOutside = false;
    g_dragIndex = -1;
    g_dragKey.clear();
    if (GetCapture() == g_hwnd) ReleaseCapture();
}

bool RebuildFromList(bool retryFailed = true) {
    if (g_stopRequested) return false;

    std::vector<ResolvedDockItem> resolved;
    resolved.reserve(g_storedKeys.size());
    if (retryFailed) g_unresolvedKeys.clear();
    for (size_t i = 0; i < g_storedKeys.size(); ++i) {
        if (g_stopRequested) return false;
        if (!retryFailed && g_unresolvedKeys.count(g_storedKeys[i])) {
            continue;
        }
        ResolvedDockItem entry;
        if (!ResolveDockItem(g_storedKeys[i], i, entry)) {
            g_unresolvedKeys.insert(g_storedKeys[i]);
            continue;
        }
        bool duplicate = false;
        for (const auto& existing : resolved) {
            if (KeyEqualsNoCase(existing.key, entry.key)) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) resolved.push_back(std::move(entry));
    }

    std::vector<DockItem> items;
    items.reserve(resolved.size());
    for (auto& entry : resolved) {
        if (g_stopRequested) return false;
        DockItem item;
        item.name = entry.name;
        item.key = entry.key;
        item.filePath = entry.filePath;
        item.pidlOwner = entry.pidlOwner;
        item.pidl = entry.pidl;
        item.icon = LoadShellIcon(entry.shellItem.Get(), entry.key);
        item.storedIndex = entry.storedIndex;
        item.isFilesystem = entry.isFilesystem;
        items.push_back(std::move(item));
    }

    ResetInternalDragState();
    g_items = std::move(items);
    PruneIconCache();
    g_intensity = 0.0;
    g_hoverIndex = -1;
    g_mouseXValid = false;

    ComputeLayout();
    g_panelWidth = g_layout.panelBaseWidth;
    for (auto& item : g_items) item.centerX = item.baseCenterX;
    EnsureFrameBuffer(g_layout.windowWidth, g_layout.windowHeight);
    PositionWindow();

    if (g_settings.hideDesktopIcons) {
        SetDesktopIconsHidden(true);
    } else {
        SetDesktopIconsHidden(false);
    }
    CaptureBackdrop();
    RenderFrame();

    if (!g_dockShown) {
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
        g_dockShown = true;
    }
    UpdateHoverPollTimer();

    g_lastItemCheckTime = std::chrono::steady_clock::now();
    return true;
}

// Failed entries are retried on their own, so the periodic refresh cannot
// stall on every stored path. A successful retry rebuilds the dock while the
// entries that are still unresolved stay skipped.
void RetryUnresolvedItems() {
    if (g_unresolvedKeys.empty() || g_stopRequested) return;

    std::vector<std::wstring> pending(g_unresolvedKeys.begin(),
                                      g_unresolvedKeys.end());
    bool resolvedAny = false;
    for (const auto& key : pending) {
        if (g_stopRequested) return;
        size_t index = 0;
        bool found = false;
        for (size_t i = 0; i < g_storedKeys.size(); ++i) {
            if (KeyEqualsNoCase(g_storedKeys[i], key)) {
                index = i;
                found = true;
                break;
            }
        }
        if (!found) {
            g_unresolvedKeys.erase(key);
            continue;
        }
        ResolvedDockItem entry;
        if (ResolveDockItem(key, index, entry)) {
            g_unresolvedKeys.erase(key);
            resolvedAny = true;
        }
    }
    if (resolvedAny) RebuildFromList(false);
}

void ReloadSettings() {
    LoadSettings();
    g_iconCache.clear();
    g_emptyPrimaryFont.reset();
    g_emptyPrimaryFamily.reset();
    g_emptySecondaryFont.reset();
    g_emptySecondaryFamily.reset();
    RebuildFromList(true);
    SetDesktopIconsHidden(g_settings.hideDesktopIcons != 0 && g_dockShown);
    UpdateHoverPollTimer();
}

int DropIndexAtScreenPoint(POINT screenPoint) {
    POINT point = screenPoint;
    ScreenToClient(g_hwnd, &point);
    for (int i = 0; i < (int)g_items.size(); ++i) {
        if (point.x < g_items[i].baseCenterX) return i;
    }
    return (int)g_items.size();
}

bool IsPointOutsideDock(POINT screenPoint) {
    POINT point = screenPoint;
    ScreenToClient(g_hwnd, &point);
    RECT rect = PanelClientRect();
    InflateRect(&rect, (int)lround(6 * g_dpiScale),
                (int)lround(6 * g_dpiScale));
    return PtInRect(&rect, point) == FALSE;
}

// Dragging an item out only removes it when the pointer is released over the
// desktop itself. Releasing over another window keeps the item in the dock.
bool IsPointOverDesktop(POINT screenPoint) {
    HWND window = WindowFromPoint(screenPoint);
    if (!window) return false;
    // The dock runs in its own process, so the desktop windows belong to the
    // shell process rather than to this one.
    DWORD shellPid = 0;
    HWND shellWindow = GetShellWindow();
    if (shellWindow) GetWindowThreadProcessId(shellWindow, &shellPid);
    if (!shellPid) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid != shellPid) return false;
    wchar_t className[64] = L"";
    if (GetClassNameW(window, className, ARRAYSIZE(className)) == 0) {
        return false;
    }
    return _wcsicmp(className, L"SysListView32") == 0 ||
           _wcsicmp(className, L"WorkerW") == 0 ||
           _wcsicmp(className, L"Progman") == 0 ||
           _wcsicmp(className, L"SHELLDLL_DefView") == 0;
}

int DragTargetIndex(double x) {
    int count = (int)g_items.size();
    if (count <= 1) return g_dragIndex;
    int desired = 0;
    for (int i = 0; i < count; ++i) {
        if (i == g_dragIndex) continue;
        if (x > g_items[i].baseCenterX) ++desired;
    }
    return ClampInt(desired, 0, count - 1);
}

// Keeps unresolved entries in place and writes the current visible order into
// the slots they occupy. This way a failed parsing name is never dropped just
// because the user reordered or removed another item.
void PersistVisibleOrder(int removedStoredIndex) {
    std::vector<std::wstring> stored = g_storedKeys;
    if (removedStoredIndex >= 0 &&
        removedStoredIndex < (int)stored.size()) {
        stored.erase(stored.begin() + removedStoredIndex);
    }

    std::vector<size_t> slots;
    slots.reserve(g_items.size());
    for (const auto& item : g_items) {
        size_t slot = item.storedIndex;
        if (removedStoredIndex >= 0 && item.storedIndex > (size_t)removedStoredIndex) {
            --slot;
        }
        if (slot < stored.size()) slots.push_back(slot);
    }
    std::sort(slots.begin(), slots.end());
    for (size_t i = 0; i < slots.size() && i < g_items.size(); ++i) {
        stored[slots[i]] = g_items[i].key;
    }

    g_storedKeys = std::move(stored);
    SaveDockKeys(g_storedKeys);
}

bool EndsWithNoCase(const std::wstring& value, const wchar_t* suffix) {
    size_t length = wcslen(suffix);
    return value.size() >= length &&
           _wcsicmp(value.c_str() + value.size() - length, suffix) == 0;
}

std::wstring GetParentPath(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    if (pos == std::wstring::npos) return L"";
    if (pos == 2 && path[1] == L':') return path.substr(0, 3);
    return path.substr(0, pos);
}

std::wstring GetKnownFolder(REFKNOWNFOLDERID folderId) {
    PWSTR raw = nullptr;
    if (FAILED(SHGetKnownFolderPath(folderId, KF_FLAG_DEFAULT, nullptr, &raw)) ||
        !raw) {
        return L"";
    }
    std::wstring path = raw;
    CoTaskMemFree(raw);
    return path;
}

// Shortcuts are copied into the mod's storage folder, so deleting the original
// shortcut from the desktop does not remove the dock item.
const std::wstring& GetDockItemsFolder() {
    EnsureStoragePaths();
    return g_itemsFolderPath;
}

bool IsUnderFolder(const std::wstring& path, const std::wstring& folder) {
    if (folder.empty() || path.size() <= folder.size()) return false;
    if (_wcsnicmp(path.c_str(), folder.c_str(), folder.size()) != 0) return false;
    return path[folder.size()] == L'\\';
}

bool IsUnderDockItemsFolder(const std::wstring& path) {
    return IsUnderFolder(path, GetDockItemsFolder());
}

uint64_t HashShortcutSource(const std::wstring& path) {
    uint64_t hash = 1469598103934665603ULL;
    for (wchar_t ch : path) {
        hash ^= (uint16_t)towlower(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::wstring BuildShortcutCopyPath(const std::wstring& sourcePath) {
    const std::wstring& folder = GetDockItemsFolder();
    if (folder.empty()) return L"";
    size_t slash = sourcePath.find_last_of(L"\\/");
    if (slash == std::wstring::npos || slash + 1 >= sourcePath.size()) {
        return L"";
    }
    std::wstring fileName = sourcePath.substr(slash + 1);

    uint64_t hash = HashShortcutSource(sourcePath);
    wchar_t hashText[17];
    for (int i = 15; i >= 0; --i) {
        uint64_t digit = hash & 0xF;
        hashText[i] =
            (wchar_t)(digit < 10 ? L'0' + digit : L'A' + (digit - 10));
        hash >>= 4;
    }
    hashText[16] = L'\0';

    std::wstring itemFolder = folder + L"\\" + hashText;
    int result = SHCreateDirectoryExW(nullptr, itemFolder.c_str(), nullptr);
    if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS) {
        Wh_Log(L"Creating dock copy folder failed: %d", result);
        return L"";
    }
    return itemFolder + L"\\" + fileName;
}

// Returns the dock-owned copy path when the source should be copied, or an
// empty string when it has to stay a plain reference (virtual items, folders,
// regular files and files that already live in the dock folder).
std::wstring ShortcutCopyTarget(const std::wstring& sourcePath) {
    if (!EndsWithNoCase(sourcePath, L".lnk") &&
        !EndsWithNoCase(sourcePath, L".url")) {
        return L"";
    }
    if (IsUnderDockItemsFolder(sourcePath)) return L"";
    DWORD attributes = GetFileAttributesW(sourcePath.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return L"";
    }
    return BuildShortcutCopyPath(sourcePath);
}

bool EnsureShortcutCopy(const std::wstring& sourcePath,
                        const std::wstring& targetPath) {
    if (CopyFileW(sourcePath.c_str(), targetPath.c_str(), FALSE)) return true;
    Wh_Log(L"Copying shortcut failed: %ls -> %ls (%lu)", sourcePath.c_str(),
           targetPath.c_str(), GetLastError());
    return false;
}

void RemoveShortcutCopy(const std::wstring& copyPath) {
    if (!IsUnderDockItemsFolder(copyPath)) return;
    if (!DeleteFileW(copyPath.c_str())) {
        Wh_Log(L"Removing dock copy failed: %ls (%lu)", copyPath.c_str(),
               GetLastError());
        return;
    }
    std::wstring itemFolder = GetParentPath(copyPath);
    const std::wstring& folder = GetDockItemsFolder();
    if (!itemFolder.empty() &&
        _wcsicmp(itemFolder.c_str(), folder.c_str()) != 0) {
        RemoveDirectoryW(itemFolder.c_str());
    }
}

void SetShortcutSource(const std::wstring& copyPath,
                       const std::wstring& sourcePath) {
    if (copyPath.empty() || sourcePath.empty()) return;
    g_shortcutSources[ToLowerString(copyPath)] = sourcePath;
}

bool FindShortcutSource(const std::wstring& copyPath,
                        std::wstring& sourcePath) {
    auto it = g_shortcutSources.find(ToLowerString(copyPath));
    if (it == g_shortcutSources.end() || it->second.empty()) return false;
    sourcePath = it->second;
    return true;
}

void RemoveShortcutSource(const std::wstring& copyPath) {
    g_shortcutSources.erase(ToLowerString(copyPath));
}

bool AddKeysAt(int insertIndex, const std::vector<std::wstring>& newKeys) {
    std::vector<std::wstring> stored = g_storedKeys;
    std::vector<std::wstring> accepted;
    bool restored = false;
    bool sourcesChanged = false;
    accepted.reserve(newKeys.size());
    for (const auto& sourceKey : newKeys) {
        if (sourceKey.empty()) continue;
        std::wstring copyPath = ShortcutCopyTarget(sourceKey);
        std::wstring key = copyPath.empty() ? sourceKey : copyPath;
        bool duplicate = ContainsKeyNoCase(stored, key) ||
                         ContainsKeyNoCase(accepted, key);
        if (copyPath.empty()) {
            if (!duplicate) accepted.push_back(std::move(key));
            continue;
        }
        if (duplicate) {
            if (GetFileAttributesW(copyPath.c_str()) ==
                INVALID_FILE_ATTRIBUTES) {
                // The stored item lost its copy; recreate it so the add works
                // again even though the entry is already in the list.
                if (EnsureShortcutCopy(sourceKey, copyPath)) {
                    restored = true;
                    SetShortcutSource(copyPath, sourceKey);
                    sourcesChanged = true;
                }
            } else {
                std::wstring knownSource;
                if (!FindShortcutSource(copyPath, knownSource)) {
                    SetShortcutSource(copyPath, sourceKey);
                    sourcesChanged = true;
                }
            }
            continue;
        }
        if (!EnsureShortcutCopy(sourceKey, copyPath)) continue;
        SetShortcutSource(copyPath, sourceKey);
        sourcesChanged = true;
        accepted.push_back(std::move(key));
    }
    if (accepted.empty()) {
        if (sourcesChanged) SaveDockKeys(g_storedKeys);
        return restored ? RebuildFromList(true) : false;
    }

    size_t target = stored.size();
    if (!g_items.empty()) {
        int index = ClampInt(insertIndex, 0, (int)g_items.size());
        if (index < (int)g_items.size()) {
            target = g_items[index].storedIndex;
        } else {
            target = g_items.back().storedIndex + 1;
        }
        if (target > stored.size()) target = stored.size();
    }
    stored.insert(stored.begin() + target, accepted.begin(), accepted.end());
    g_storedKeys = std::move(stored);
    SaveDockKeys(g_storedKeys);
    return RebuildFromList(true);
}

std::wstring MakeUniqueShortcutPath(const std::wstring& desktopPath,
                                    const std::wstring& displayName,
                                    const wchar_t* extension) {
    std::wstring stem = displayName;
    size_t extensionLength = wcslen(extension);
    if (EndsWithNoCase(stem, extension)) {
        stem.resize(stem.size() - extensionLength);
    }
    for (auto& c : stem) {
        if (c < 32 || wcschr(L"\\/:*?\"<>|", c)) c = L'_';
    }
    while (!stem.empty() && (stem.back() == L'.' || stem.back() == L' ')) {
        stem.pop_back();
    }
    if (stem.empty()) stem = L"Shortcut";

    std::wstring candidate = desktopPath + L"\\" + stem + extension;
    for (int i = 2; i < 1000; ++i) {
        if (GetFileAttributesW(candidate.c_str()) == INVALID_FILE_ATTRIBUTES) {
            return candidate;
        }
        candidate = desktopPath + L"\\" + stem + L" (" + std::to_wstring(i) +
                    L")" + extension;
    }
    return desktopPath + L"\\" + stem + extension;
}

bool PositionDesktopItemAt(const std::wstring& itemPath, POINT screenPoint) {
    ComPtr<IFolderView> view;
    HWND defView = nullptr;
    if (!GetDesktopView(view, &defView) || !view) {
        Wh_Log(L"Positioning desktop item: desktop view unavailable");
        return false;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(itemPath.c_str(), nullptr, &pidl, 0, nullptr)) ||
        !pidl) {
        Wh_Log(L"Positioning desktop item: %ls not found", itemPath.c_str());
        return false;
    }

    POINT point = screenPoint;
    if (defView) ScreenToClient(defView, &point);
    point.x -= GetSystemMetrics(SM_CXICON) / 2;
    point.y -= GetSystemMetrics(SM_CYICON) / 2;

    PCUITEMID_CHILD child = ILFindLastID(pidl);
    HRESULT hr =
        view->SelectAndPositionItems(1, &child, &point, SVSI_POSITIONITEM);
    CoTaskMemFree(pidl);
    if (FAILED(hr)) {
        Wh_Log(L"SelectAndPositionItems failed: 0x%08X", hr);
        return false;
    }
    return true;
}

bool CreateDesktopShortcut(const std::wstring& sourcePath,
                           const std::wstring& displayName,
                           const std::wstring& desktopPath,
                           std::wstring& outPath) {
    bool sourceIsShortcutFile = EndsWithNoCase(sourcePath, L".lnk") ||
                                EndsWithNoCase(sourcePath, L".url");
    const wchar_t* extension =
        EndsWithNoCase(sourcePath, L".url") ? L".url" : L".lnk";
    std::wstring destination =
        MakeUniqueShortcutPath(desktopPath, displayName, extension);
    if (sourceIsShortcutFile) {
        if (!CopyFileW(sourcePath.c_str(), destination.c_str(), TRUE)) {
            Wh_Log(L"Copying shortcut failed: %ls (%lu)", sourcePath.c_str(),
                   GetLastError());
            return false;
        }
    } else {
        ComPtr<IShellLinkW> link;
        if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(link.put())))) {
            return false;
        }
        link->SetPath(sourcePath.c_str());
        link->SetDescription(displayName.c_str());
        std::wstring parent = GetParentPath(sourcePath);
        if (!parent.empty()) link->SetWorkingDirectory(parent.c_str());

        ComPtr<IPersistFile> persist;
        if (FAILED(link->QueryInterface(IID_PPV_ARGS(persist.put()))) ||
            FAILED(persist->Save(destination.c_str(), TRUE))) {
            Wh_Log(L"Creating desktop shortcut failed: %ls", destination.c_str());
            return false;
        }
    }
    outPath = std::move(destination);
    return true;
}

// Returns false only when the item should stay in the dock because it could
// not be placed on the desktop.
bool PlaceOnDesktopAt(const DockItem& item, POINT screenPoint) {
    if (!item.isFilesystem || item.filePath.empty()) return true;

    std::wstring desktopPath = GetKnownFolder(FOLDERID_Desktop);
    if (desktopPath.empty()) {
        Wh_Log(L"Desktop folder unavailable");
        return true;
    }
    std::wstring publicDesktop = GetKnownFolder(FOLDERID_PublicDesktop);

    // A dock copy keeps the path of the shortcut it was copied from. Prefer
    // that original, so dragging the item out moves the original shortcut
    // back instead of creating a second one next to it.
    std::wstring sourcePath;
    std::wstring placementPath = item.filePath;
    if (FindShortcutSource(item.key, sourcePath) &&
        GetFileAttributesW(sourcePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        placementPath = sourcePath;
    }

    std::wstring parent = GetParentPath(placementPath);
    bool alreadyOnDesktop =
        _wcsicmp(parent.c_str(), desktopPath.c_str()) == 0 ||
        (!publicDesktop.empty() &&
         _wcsicmp(parent.c_str(), publicDesktop.c_str()) == 0);
    if (alreadyOnDesktop) {
        PositionDesktopItemAt(placementPath, screenPoint);
        return true;
    }

    if (GetFileAttributesW(placementPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Desktop placement skipped, source missing: %ls",
               placementPath.c_str());
        return true;
    }

    std::wstring shortcutPath;
    if (!CreateDesktopShortcut(placementPath, item.name, desktopPath,
                               shortcutPath)) {
        return false;
    }
    SHChangeNotify(SHCNE_CREATE, SHCNF_PATHW, shortcutPath.c_str(), nullptr);
    PositionDesktopItemAt(shortcutPath, screenPoint);
    return true;
}

void FinishInternalDrag(bool removeItem, POINT screenPoint) {
    if (!g_internalDragActive || g_dragIndex < 0 ||
        g_dragIndex >= (int)g_items.size()) {
        ResetInternalDragState();
        return;
    }

    int removedStoredIndex = -1;
    if (removeItem) {
        if (!PlaceOnDesktopAt(g_items[g_dragIndex], screenPoint)) {
            ResetInternalDragState();
            RebuildFromList(true);
            return;
        }
        removedStoredIndex = (int)g_items[g_dragIndex].storedIndex;
        RemoveShortcutCopy(g_items[g_dragIndex].filePath);
        RemoveShortcutSource(g_items[g_dragIndex].key);
        g_items.erase(g_items.begin() + g_dragIndex);
    }
    PersistVisibleOrder(removedStoredIndex);
    RebuildFromList(true);
}

void ClearExternalDrag() {
    if (!g_externalDragActive) return;
    g_externalDragActive = false;
    g_dropInsertIndex = 0;
    RenderFrame();
}

class DockDropTarget final : public IDropTarget {
  public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropTarget)) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return (ULONG)InterlockedIncrement(&ref_);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        LONG count = InterlockedDecrement(&ref_);
        if (count == 0) delete this;
        return (ULONG)count;
    }

    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* data, DWORD, POINTL pt,
                                        DWORD* effect) override {
        if (!effect) return E_POINTER;
        DWORD chosen = EffectFromAllowed(*effect);
        if (!CanAcceptData(data) || chosen == DROPEFFECT_NONE) {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }
        g_externalDragActive = true;
        g_hoverIndex = -1;
        g_mouseXValid = false;
        g_dropInsertIndex =
            DropIndexAtScreenPoint(POINT{(LONG)pt.x, (LONG)pt.y});
        EnsureAnimationTimer();
        RenderFrame();
        *effect = chosen;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* effect) override {
        if (!effect) return E_POINTER;
        DWORD chosen = EffectFromAllowed(*effect);
        if (!g_externalDragActive || chosen == DROPEFFECT_NONE) {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }
        int index = DropIndexAtScreenPoint(POINT{(LONG)pt.x, (LONG)pt.y});
        if (index != g_dropInsertIndex) {
            g_dropInsertIndex = index;
            RenderFrame();
        }
        *effect = chosen;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override {
        ClearExternalDrag();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* data, DWORD, POINTL,
                                   DWORD* effect) override {
        if (!effect) return E_POINTER;
        DWORD chosen = EffectFromAllowed(*effect);
        int index = g_dropInsertIndex;
        std::vector<std::wstring> keys;
        ExtractKeys(data, keys);
        ClearExternalDrag();
        if (chosen == DROPEFFECT_NONE || keys.empty()) {
            *effect = DROPEFFECT_NONE;
            return S_OK;
        }
        // Adding an item copies shortcuts and resolves parsing names, which
        // can block on a slow path. Post the work, so the drag source is not
        // frozen while the drop is being processed.
        PendingDrop pending;
        pending.index = index;
        pending.keys = std::move(keys);
        g_pendingDrops.push_back(std::move(pending));
        PostMessageW(g_hwnd, kMsgAddDropped, 0, 0);
        *effect = chosen;
        return S_OK;
    }

  private:
    ~DockDropTarget() = default;

    static DWORD EffectFromAllowed(DWORD allowed) {
        if (allowed & DROPEFFECT_COPY) return DROPEFFECT_COPY;
        if (allowed & DROPEFFECT_LINK) return DROPEFFECT_LINK;
        return DROPEFFECT_NONE;
    }

    static UINT ShellIdListFormat() {
        static UINT format = RegisterClipboardFormatW(L"Shell IDList Array");
        return format;
    }

    static bool CanAcceptData(IDataObject* data) {
        if (!data) return false;
        FORMATETC format{};
        format.dwAspect = DVASPECT_CONTENT;
        format.lindex = -1;
        format.tymed = TYMED_HGLOBAL;
        format.cfFormat = CF_HDROP;
        if (data->QueryGetData(&format) == S_OK) return true;
        format.cfFormat = (CLIPFORMAT)ShellIdListFormat();
        return format.cfFormat && data->QueryGetData(&format) == S_OK;
    }

    static void AddKeyFromShellItem(IShellItem* item,
                                    std::vector<std::wstring>& keys) {
        if (!item) return;
        std::wstring key = GetShellItemName(item, SIGDN_DESKTOPABSOLUTEPARSING);
        if (key.empty()) key = GetShellItemName(item, SIGDN_FILESYSPATH);
        if (!key.empty()) keys.push_back(std::move(key));
    }

    static void ExtractKeys(IDataObject* data, std::vector<std::wstring>& keys) {
        keys.clear();
        if (!data) return;

        ComPtr<IShellItemArray> array;
        if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(
                data, IID_PPV_ARGS(array.put()))) &&
            array) {
            DWORD count = 0;
            if (SUCCEEDED(array->GetCount(&count))) {
                for (DWORD i = 0; i < count; ++i) {
                    ComPtr<IShellItem> item;
                    if (SUCCEEDED(array->GetItemAt(i, item.put()))) {
                        AddKeyFromShellItem(item.Get(), keys);
                    }
                }
            }
        }
        if (!keys.empty()) return;

        FORMATETC format{};
        format.cfFormat = CF_HDROP;
        format.dwAspect = DVASPECT_CONTENT;
        format.lindex = -1;
        format.tymed = TYMED_HGLOBAL;
        STGMEDIUM medium{};
        if (data->GetData(&format, &medium) != S_OK) return;
        if (medium.tymed == TYMED_HGLOBAL && medium.hGlobal) {
            HDROP drop = (HDROP)medium.hGlobal;
            UINT count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
            for (UINT i = 0; i < count; ++i) {
                UINT length = DragQueryFileW(drop, i, nullptr, 0);
                if (length == 0) continue;
                std::vector<wchar_t> buffer((size_t)length + 1, 0);
                if (DragQueryFileW(drop, i, buffer.data(), length + 1) > 0) {
                    keys.emplace_back(buffer.data());
                }
            }
        }
        ReleaseStgMedium(&medium);
    }

    LONG ref_ = 1;
};

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
                    if (!g_menuOpen && !g_internalDragActive &&
                        !g_externalDragActive &&
                        now - g_lastItemCheckTime >
                            std::chrono::seconds(kMaintenanceSeconds)) {
                        g_lastItemCheckTime = now;
                        RetryUnresolvedItems();
                        RefreshVirtualIconCache();
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
                case kTimerRefresh:
                    KillTimer(hwnd, kTimerRefresh);
                    if (!RebuildFromList(true)) {
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
            if (g_leftDown && !g_internalDragActive) {
                POINT point{};
                GetCursorPos(&point);
                int threshold = (int)lround(kDragThresholdPx * g_dpiScale);
                if (abs(point.x - g_leftDownPoint.x) > threshold ||
                    abs(point.y - g_leftDownPoint.y) > threshold) {
                    int index = -1;
                    for (int i = 0; i < (int)g_items.size(); ++i) {
                        if (KeyEqualsNoCase(g_items[i].key, g_dragKey)) {
                            index = i;
                            break;
                        }
                    }
                    if (index < 0) {
                        ResetInternalDragState();
                        return 0;
                    }
                    g_internalDragActive = true;
                    g_dragCandidate = false;
                    g_dragIndex = index;
                    g_hoverIndex = -1;
                    g_mouseXValid = false;
                    g_intensity = 0.0;
                    for (auto& item : g_items) {
                        item.scale = 1.0;
                        item.centerX = item.baseCenterX;
                        item.labelAlpha = 0.0;
                    }
                    g_panelWidth = g_layout.panelBaseWidth;
                    g_dragOutside = IsPointOutsideDock(point);
                    SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
                    RenderFrame();
                }
            }
            if (g_internalDragActive) {
                POINT point{};
                GetCursorPos(&point);
                bool changed = false;
                bool outside = IsPointOutsideDock(point);
                if (outside != g_dragOutside) {
                    g_dragOutside = outside;
                    changed = true;
                }
                if (!outside) {
                    POINT client = point;
                    ScreenToClient(hwnd, &client);
                    int target = DragTargetIndex(client.x);
                    if (target != g_dragIndex && target >= 0 &&
                        target < (int)g_items.size()) {
                        DockItem moved = std::move(g_items[g_dragIndex]);
                        g_items.erase(g_items.begin() + g_dragIndex);
                        g_items.insert(g_items.begin() + target, std::move(moved));
                        g_dragIndex = target;
                        ComputeLayout();
                        g_panelWidth = g_layout.panelBaseWidth;
                        for (auto& item : g_items) {
                            item.centerX = item.baseCenterX;
                            item.scale = 1.0;
                            item.labelAlpha = 0.0;
                        }
                        EnsureFrameBuffer(g_layout.windowWidth,
                                          g_layout.windowHeight);
                        changed = true;
                    }
                }
                if (changed) RenderFrame();
                return 0;
            }
            if (!g_items.empty()) EnsureAnimationTimer();
            return 0;

        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            if (!g_items.empty()) EnsureAnimationTimer();
            return 0;

        case WM_LBUTTONDOWN: {
            if (g_externalDragActive || g_menuOpen) return 0;
            POINT point{};
            GetCursorPos(&point);
            int index = ItemAtCursor(point);
            if (index < 0) return 0;
            g_leftDown = true;
            g_dragCandidate = true;
            g_internalDragActive = false;
            g_dragOutside = false;
            g_leftDownPoint = point;
            g_dragIndex = index;
            g_dragKey = g_items[index].key;
            SetCapture(hwnd);
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_internalDragActive) {
                POINT point{};
                GetCursorPos(&point);
                bool onDesktop = IsPointOutsideDock(point) &&
                                 IsPointOverDesktop(point);
                FinishInternalDrag(onDesktop, point);
                return 0;
            }
            if (!g_leftDown) return 0;
            ResetInternalDragState();

            DWORD now = GetTickCount();
            if (now - g_lastLaunchTick < 450) return 0;
            POINT point{};
            GetCursorPos(&point);
            int index = ItemAtCursor(point);
            if (index >= 0) {
                g_lastLaunchTick = now;
                LaunchItem(g_items[index]);
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            if (g_internalDragActive) {
                PersistVisibleOrder(-1);
                ResetInternalDragState();
                RebuildFromList(true);
            } else {
                ResetInternalDragState();
            }
            return 0;

        case WM_RBUTTONUP: {
            if (g_internalDragActive || g_leftDown || g_externalDragActive) {
                return 0;
            }
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
                if (g_internalDragActive) {
                    SetCursor(LoadCursorW(
                        nullptr, g_dragOutside ? IDC_NO : IDC_SIZEALL));
                    return TRUE;
                }
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
            g_emptyPrimaryFont.reset();
            g_emptyPrimaryFamily.reset();
            g_emptySecondaryFont.reset();
            g_emptySecondaryFamily.reset();
            if (!RebuildFromList(true)) {
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

        case kMsgReloadSettings:
            ReloadSettings();
            return 0;

        case kMsgAddDropped:
            if (!g_pendingDrops.empty()) {
                std::vector<PendingDrop> drops = std::move(g_pendingDrops);
                g_pendingDrops.clear();
                for (auto& drop : drops) {
                    AddKeysAt(drop.index, drop.keys);
                }
            }
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
        if (GetShellWindow()) break;
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
    g_storedKeys = LoadDockKeys();
    g_lastItemCheckTime = std::chrono::steady_clock::now();
    if (auto* dropTarget = new (std::nothrow) DockDropTarget()) {
        if (RegisterDragDrop(hwnd, dropTarget) == S_OK) {
            g_dropTarget = dropTarget;
        } else {
            Wh_Log(L"RegisterDragDrop failed: %lu", GetLastError());
            dropTarget->Release();
        }
    }
    RebuildFromList(true);

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
    if (g_dropTarget) {
        RevokeDragDrop(hwnd);
        g_dropTarget->Release();
        g_dropTarget = nullptr;
    }
    ResetInternalDragState();
    g_externalDragActive = false;
    KillTimer(hwnd, kTimerAnimation);
    KillTimer(hwnd, kTimerMaintenance);
    KillTimer(hwnd, kTimerRefresh);
    KillTimer(hwnd, kTimerHoverPoll);
    g_hoverPollRunning = false;

    SetDesktopIconsHidden(false);
    g_desktopListView = nullptr;

    g_activeMenu2.Reset();
    g_activeMenu3.Reset();
    DestroyFrameBuffer();
    g_backdrop.reset();
    g_backdropValid = false;
    g_iconCache.clear();
    g_items.clear();
    g_storedKeys.clear();
    g_unresolvedKeys.clear();
    g_pendingDrops.clear();
    g_labelFont.reset();
    g_labelFontFamily.reset();
    g_emptyPrimaryFont.reset();
    g_emptyPrimaryFamily.reset();
    g_emptySecondaryFont.reset();
    g_emptySecondaryFamily.reset();
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

BOOL WhTool_ModInit() {
    g_stopRequested = false;
    HANDLE thread = CreateThread(nullptr, 0, DockThreadProc, nullptr, 0, nullptr);
    if (!thread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        return FALSE;
    }
    g_thread = thread;
    return TRUE;
}

void WhTool_ModUninit() {
    g_stopRequested = true;
    HWND hwnd = g_hwnd.load();
    if (hwnd) PostMessageW(hwnd, kMsgQuit, 0, 0);
    HANDLE thread = g_thread.exchange(nullptr);
    if (thread) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

void WhTool_ModSettingsChanged() {
    HWND hwnd = g_hwnd.load();
    if (hwnd) PostMessageW(hwnd, kMsgReloadSettings, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to
// other processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod loads and runs in a dedicated explorer.exe process, not in the
// shell's own explorer.exe. explorer.exe is used as the host because it
// provides the shell interfaces this mod needs, and unlike windhawk.exe it is
// x86-64 on x86-64 Windows.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
