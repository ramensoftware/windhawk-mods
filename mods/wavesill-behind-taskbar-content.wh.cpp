// ==WindhawkMod==
// @id              wavesill-behind-taskbar-content
// @name            Wavesill Behind Taskbar Content
// @name:zh-CN      栏声 (Wavesill) 置于任务栏内容之下
// @description     Moves the Wavesill audio spectrum behind everything on the Windows 11 taskbar — icons, search, tray and clock — so it never covers them
// @description:zh-CN 把栏声 (Wavesill) 的音频频谱放到 Windows 11 任务栏上所有内容 (图标、搜索、托盘、时钟) 的后面，不再遮挡它们
// @version         0.6.3
// @author          GenuineHorace
// @github          https://github.com/GenuineHorace
// @include         explorer.exe
// @architecture    x86-64
// @architecture    arm64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Wavesill Behind Taskbar Content

Companion mod for [Wavesill (栏声)](https://github.com/GenuineHorace/Wavesill), the faint
audio spectrum for the Windows taskbar.

On its own, Wavesill draws its spectrum in a click-through window *on top of* the
taskbar. With this mod the spectrum sits *inside* the taskbar instead: beneath the Start
button, app icons, tray and clock, and above the acrylic backdrop, so nothing is ever
drawn over an icon.

## How It Works

* Wavesill keeps doing all the work: audio capture, analysis, styles, colours, opacity and
  per-monitor settings. Every frame it hands the finished picture to this mod through a
  small shared-memory block (`Local\Wavesill.Bridge`).
* The mod inserts one `Image` element into each taskbar's XAML tree, right after the
  `TaskbarBackground` element, and copies Wavesill's frames into it.
* Heartbeats run both ways. While the mod is displaying a taskbar, Wavesill hides its own
  overlay there; if the mod is disabled, Windhawk is closed or a Windows update breaks it,
  Wavesill's overlay comes back within a second. Nothing is ever half-drawn.

## Requirements

* Windows 11 (the XAML taskbar), x64 or ARM64. Windows 10 keeps using Wavesill's overlay.
* Wavesill 0.6.2 or later running. Its About window shows "Windhawk Mod: Connected", a
  "Mod Stage" line (9/9 when frames are flowing), and `[Mod]` on each taskbar line.

## Safety

* The mod hooks no functions. It uses the public XAML diagnostics API, the same mechanism
  that TranslucentTB and the Taskbar Styler use.
* A crash guard marks the moment the mod asks XAML to load it and clears the mark 20 s
  later. If Explorer dies in between, the next run skips the XAML part and Wavesill's
  About window says so; re-enable the mod to try again. There is no restart loop.
* The "Attach to the Taskbar" setting turns the XAML part off entirely.

---

# 栏声 (Wavesill) 置于任务栏内容之下

[栏声 (Wavesill)](https://github.com/GenuineHorace/Wavesill) 的配套模组。栏声是一个显示在
Windows 任务栏上的淡淡音频频谱。

栏声自己只能把频谱画在一个点击穿透的窗口里、浮在任务栏**上面**。装上这个模组后，频谱会显示在任务栏
**内部**：在开始按钮、程序图标、托盘和时钟之下、亚克力背景之上，永远不会盖住任何图标。

## 工作方式

* 所有工作仍由栏声完成：采集、分析、样式、颜色、浓度、每显示器设置。它每帧把画好的图片通过一小块
  共享内存 (`Local\Wavesill.Bridge`) 交给本模组。
* 模组在每条任务栏的 XAML 树里、`TaskbarBackground` 元素之后插入一个 `Image`，把栏声的帧拷进去。
* 双向心跳。模组显示某条任务栏时，栏声隐藏自己在那里的覆盖层；模组被禁用、Windhawk 关闭或
  Windows 更新把它弄坏时，栏声的覆盖层在一秒内自动回来。不会出现画一半的情况。

## 要求

* Windows 11 (XAML 任务栏)，x64 或 ARM64。Windows 10 继续使用栏声的覆盖层。
* 栏声 0.6.2 或更新版本正在运行。它的"关于"窗口会显示"Windhawk 模组：已连接"、"模组阶段"
  (帧在流动时为 9/9)，每条任务栏后面标 `[模组]`。

## 安全性

* 不 hook 任何函数，只使用公开的 XAML 诊断接口 (TranslucentTB 与 Taskbar Styler 用的同一机制)。
* 崩溃保护：模组请求 XAML 加载自己时留下标记，20 秒后清除。若 Explorer 在此期间崩溃，下一次运行会
  跳过 XAML 部分，栏声的"关于"窗口会说明原因；重新启用模组即可再试一次。不会出现重启循环。
* 设置项"接入任务栏"可以把 XAML 部分整个关掉。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- attach: true
  $name: Attach to the Taskbar
  $name:zh-CN: 接入任务栏
  $description: >-
    Turn this off to keep the mod installed but inert; Wavesill then uses its own overlay.
    Takes effect the next time the mod is enabled.
  $description:zh-CN: >-
    关闭后模组保持安装但不做任何事，栏声改用自己的覆盖层。下次启用模组时生效。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <xamlom.h>

// winbase.h defines GetCurrentTime() as a macro; C++/WinRT's XAML animation headers
// declare a method with that name.
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wuc = winrt::Windows::UI::Core;
namespace wsys = winrt::Windows::System;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxmi = winrt::Windows::UI::Xaml::Media::Imaging;

// =====================================================================================
//  Bridge protocol. This is a verbatim copy of wavesill_bridge.h from the Wavesill
//  repository (a Windhawk mod is a single file). Keep the two in sync; WS_BRIDGE_PROTOCOL
//  is the only number that has to agree.
// =====================================================================================
#define WS_BRIDGE_MAPPING_NAME   L"Local\\Wavesill.Bridge"
#define WS_BRIDGE_MAGIC          0x4C495357u          /* "WSIL" */
#define WS_BRIDGE_PROTOCOL       1u
#define WS_BRIDGE_FRESH_MS       500u
#define WS_BRIDGE_MAX_SLOTS      8u
#define WS_BRIDGE_MIN_SLOT_BYTES (2u * 1024u * 1024u)
enum { WS_EDGE_LEFT = 0, WS_EDGE_TOP = 1, WS_EDGE_RIGHT = 2, WS_EDGE_BOTTOM = 3 };
#pragma pack(push, 8)
typedef struct WsBridgeHeader {
    uint32_t magic, protocol, headerSize, slotSize, slotCount, slotBytes, totalBytes, modVersion;
    uint64_t modHeartbeatMs;
    uint32_t appProtocol, appVersion;
    uint64_t appHeartbeatMs;
    uint32_t flags;
    uint8_t  reserved[68];
} WsBridgeHeader;
typedef struct WsBridgeSlot {
    uint64_t taskbarHwnd;
    int32_t  x, y, width, height;
    uint32_t stride, dpi, edge;
    uint32_t bufferOffset[2], bufferBytes, front, frameSeq, visible;
    uint64_t appHeartbeatMs;
    uint32_t flags;
    uint64_t modHeartbeatMs;
    uint32_t modStatus;
    uint8_t  reserved[52];
} WsBridgeSlot;
#pragma pack(pop)
enum { WS_SLOT_PRIMARY = 1u };
/* header.flags: progress bits so that Wavesill's About window can say where the mod stopped. */
enum {
    WS_STAGE_XAML = 1u,        /* Taskbar.View.dll and Windows.UI.Xaml.dll are present   */
    WS_STAGE_DIAG = 2u,        /* InitializeXamlDiagnosticsEx succeeded                  */
    WS_STAGE_ADVISED = 4u,     /* AdviseVisualTreeChange succeeded                       */
    WS_STAGE_BACKGROUND = 8u,  /* A Taskbar.TaskbarBackground element was seen           */
    WS_STAGE_INSERTED = 16u,   /* The mod's Image is in the tree                         */
    WS_STAGE_TIMER = 32u,      /* DispatcherTimer created                                */
    WS_STAGE_TICKING = 64u,    /* First tick ran (timer or Rendering fallback)           */
    WS_STAGE_MATCHED = 128u,   /* A taskbar was paired with a slot                       */
    WS_STAGE_BLITTED = 256u,   /* At least one frame was copied into the taskbar         */
    WS_STAGE_OFF = 0x4000u,    /* Attach disabled in the settings                        */
    WS_STAGE_GUARD = 0x8000u,  /* Attach skipped because the previous run crashed        */
};
static_assert(sizeof(WsBridgeHeader) == 128, "Header layout");
static_assert(sizeof(WsBridgeSlot) == 144, "Slot layout");

#define MOD_VERSION_U32 ((0u << 16) | (6u << 8) | 3u)

// {7B1C0A3E-5F2D-4E8A-9C61-2A7F3D9E4B10}: this mod's TAP class id. It is never
// registered; XAML loads it straight from this DLL through the exported DllGetClassObject.
static const CLSID CLSID_WavesillTAP = {0x7B1C0A3E, 0x5F2D, 0x4E8A, {0x9C, 0x61, 0x2A, 0x7F, 0x3D, 0x9E, 0x4B, 0x10}};
static const IID WS_IID_IUnknown        = {0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const IID WS_IID_IClassFactory   = {0x00000001, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const IID WS_IID_IObjectWithSite = {0xFC4801A3, 0x2BA9, 0x11CF, {0xA2, 0x29, 0x00, 0xAA, 0x00, 0x3D, 0x73, 0x52}};
static const IID WS_IID_IXamlDiagnostics         = {0x18C9E2B6, 0x3F43, 0x4116, {0x9F, 0x2B, 0xFF, 0x93, 0x5D, 0x77, 0x70, 0xD2}};
static const IID WS_IID_IVisualTreeService3      = {0x0E79C6E0, 0x85A0, 0x4BE8, {0xB4, 0x1A, 0x65, 0x5C, 0xF1, 0xFD, 0x19, 0xBD}};
static const IID WS_IID_IVisualTreeServiceCallback  = {0xAA7A8931, 0x80E4, 0x4FEC, {0x8F, 0x3B, 0x55, 0x3F, 0x87, 0xB4, 0x96, 0x6E}};
static const IID WS_IID_IVisualTreeServiceCallback2 = {0xBAD9EB88, 0xAE77, 0x4397, {0xB9, 0x48, 0x5F, 0xA2, 0xDB, 0x0A, 0x19, 0xEA}};
// Windows.Storage.Streams.IBufferByteAccess, declared by hand so that no extra header is needed.
static const IID IID_IBufferByteAccessMin = {0x905A0FEF, 0xBC53, 0x11DF, {0x8C, 0x49, 0x00, 0x1E, 0x4F, 0xC6, 0x86, 0xDA}};
struct IBufferByteAccessMin : IUnknown { virtual HRESULT STDMETHODCALLTYPE Buffer(uint8_t** value) = 0; };

// --------------------------------------------------------------------------- globals
// Once XAML has loaded this DLL as a TAP it stays loaded, so disabling the mod cannot
// really unload it: disabling puts it to sleep (timer stopped, elements collapsed, bridge
// closed) and re-enabling runs Wh_ModInit again on the same globals and wakes it up. The
// XAML attach itself is kept across sessions; only the per-session state is reset.
static HMODULE g_hModule = nullptr;
static std::atomic<bool> g_unloading{false};
static std::atomic<uint32_t> g_stage{0};
static std::atomic<bool> g_renderFallback{false};
static bool g_attach = true;
static bool g_guardTripped = false;
static std::wstring g_guardPath;
static HANDLE g_bridgeThread = nullptr;
static void stage(uint32_t bit) { g_stage.fetch_or(bit); }

// Bridge (created by this mod, read and written by Wavesill).
static HANDLE g_map = nullptr;
static uint8_t* g_base = nullptr;
static WsBridgeHeader* g_hdr = nullptr;

// XAML diagnostics.
static std::mutex g_diagMutex;
static winrt::com_ptr<IXamlDiagnostics> g_diag;
static winrt::com_ptr<IVisualTreeService3> g_service;
static IVisualTreeServiceCallback2* g_watcher = nullptr;

// UI-thread state. Touched only on the XAML UI thread, except through the dispatcher.
struct Host {
    InstanceHandle handle = 0;
    wux::FrameworkElement bg{nullptr};
    wuxc::Panel parent{nullptr};
    wuxc::Image image{nullptr};
    wuxmi::WriteableBitmap bmp{nullptr};
    int bmpW = 0, bmpH = 0;
    int slot = -1;
    uint32_t lastSeq = 0;
    int ticksSinceMatch = 999;
    bool inserted = false;
    bool broken = false;
    ULONGLONG nextTryMs = 0;
    int insertTries = 0;
};
static std::vector<Host> g_hosts;
static wux::DispatcherTimer g_timer{nullptr};
static int g_timerMs = 0;
static winrt::event_token g_renderToken{};
static ULONGLONG g_lastMatchLog = 0;
static std::mutex g_dispMutex;
static wuc::CoreDispatcher g_dispatcher{nullptr};   // null inside XAML islands
static wsys::DispatcherQueue g_queue{nullptr};      // the island thread's queue

// --------------------------------------------------------------------------- crash guard
// A marker file exists from the moment the mod asks XAML to load it until the attach has
// been stable for 20 s. Finding it at start-up means the previous run died in between, so
// the XAML part is skipped and the marker cleared: Explorer stays up, Wavesill shows the
// reason, and re-enabling the mod tries once more.
static void guardInit() {
    wchar_t buf[MAX_PATH] = L"";
    if (!GetEnvironmentVariableW(L"LOCALAPPDATA", buf, MAX_PATH) || !buf[0]) return;
    std::wstring dir = std::wstring(buf) + L"\\Wavesill";
    CreateDirectoryW(dir.c_str(), nullptr);
    g_guardPath = dir + L"\\mod.crashguard";
    if (GetFileAttributesW(g_guardPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        g_guardTripped = true;
        DeleteFileW(g_guardPath.c_str());
        Wh_Log(L"Crash guard: the previous run crashed while attaching; the XAML attach is skipped this time");
    }
}
static void guardArm() {
    if (g_guardPath.empty()) return;
    HANDLE f = CreateFileW(g_guardPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f != INVALID_HANDLE_VALUE) CloseHandle(f);
}
static void guardDisarm() { if (!g_guardPath.empty()) DeleteFileW(g_guardPath.c_str()); }

// --------------------------------------------------------------------------- bridge
static WsBridgeSlot* slotAt(uint32_t i) { return (WsBridgeSlot*)(g_base + g_hdr->headerSize + (size_t)i * g_hdr->slotSize); }

static bool bridgeCreate() {
    const uint32_t slotCount = 4, bufferBytes = 3u * 1024u * 1024u;   // 3 MB per buffer: up to 5120 × 150 px
    const uint32_t headerSize = sizeof(WsBridgeHeader), slotSize = sizeof(WsBridgeSlot);
    const uint32_t pixelBase = (headerSize + slotCount * slotSize + 4095u) & ~4095u;
    const uint32_t total = pixelBase + slotCount * 2u * bufferBytes;
    g_map = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, total, WS_BRIDGE_MAPPING_NAME);
    if (!g_map) { Wh_Log(L"CreateFileMapping failed: %u", GetLastError()); return false; }
    const bool existed = GetLastError() == ERROR_ALREADY_EXISTS;
    g_base = (uint8_t*)MapViewOfFile(g_map, FILE_MAP_ALL_ACCESS, 0, 0, total);
    if (!g_base) { Wh_Log(L"MapViewOfFile failed: %u", GetLastError()); CloseHandle(g_map); g_map = nullptr; return false; }
    g_hdr = (WsBridgeHeader*)g_base;
    if (!existed || g_hdr->magic != WS_BRIDGE_MAGIC || g_hdr->headerSize != headerSize || g_hdr->slotSize != slotSize ||
        g_hdr->slotCount != slotCount || g_hdr->totalBytes != total) {
        memset(g_base, 0, pixelBase);   // fresh (or foreign) mapping: lay it out from scratch
    }
    g_hdr->magic = WS_BRIDGE_MAGIC; g_hdr->protocol = WS_BRIDGE_PROTOCOL;
    g_hdr->headerSize = headerSize; g_hdr->slotSize = slotSize; g_hdr->slotCount = slotCount;
    g_hdr->slotBytes = 2u * bufferBytes; g_hdr->totalBytes = total; g_hdr->modVersion = MOD_VERSION_U32;
    for (uint32_t i = 0; i < slotCount; ++i) {
        WsBridgeSlot* s = slotAt(i);
        s->bufferOffset[0] = pixelBase + i * 2u * bufferBytes;
        s->bufferOffset[1] = s->bufferOffset[0] + bufferBytes;
        s->bufferBytes = bufferBytes;
        s->modHeartbeatMs = 0; s->modStatus = 0;
    }
    g_hdr->modHeartbeatMs = GetTickCount64();
    Wh_Log(L"Bridge ready: %u slots, %u bytes%s", slotCount, total, existed ? L" (mapping already existed)" : L"");
    return true;
}
static void bridgeClose() {
    if (g_hdr) {
        for (uint32_t i = 0; i < g_hdr->slotCount; ++i) { slotAt(i)->modStatus = 0; slotAt(i)->modHeartbeatMs = 0; }
        g_hdr->modHeartbeatMs = 0;
    }
    if (g_base) UnmapViewOfFile(g_base);
    if (g_map) CloseHandle(g_map);
    g_base = nullptr; g_hdr = nullptr; g_map = nullptr;
}

// --------------------------------------------------------------------------- UI-thread work
static void logHr(const wchar_t* what, const winrt::hresult_error& e) {
    Wh_Log(L"%s: 0x%08X %s", what, (unsigned)e.code().value, e.message().c_str());
}

static void removeImage(Host& h) {
    try {
        if (h.parent && h.image) { uint32_t i = 0; auto ch = h.parent.Children(); if (ch.IndexOf(h.image, i)) ch.RemoveAt(i); }
    } catch (...) {}
    h.parent = nullptr; h.image = nullptr; h.bmp = nullptr; h.inserted = false; h.slot = -1;
}

static bool tryInsert(Host& h) {
    try {
        // Explorer briefly shows other XAML islands that also contain a TaskbarBackground
        // (a full-screen one appears for a fraction of a second on some actions). Only a
        // taskbar-shaped island gets the element: one side no taller than 256 DIP.
        auto root = h.bg.XamlRoot();
        if (!root) return false;                                   // not in a tree yet; retry later
        const auto sz = root.Size();
        if (std::min(sz.Width, sz.Height) > 256.f) {
            h.broken = true;
            Wh_Log(L"Ignored: an island of %.0f × %.0f DIP is not a taskbar", sz.Width, sz.Height);
            return false;
        }
        wuxc::Panel panel{nullptr};
        uint32_t index = 0;
        if (auto parentObj = h.bg.Parent()) panel = parentObj.try_as<wuxc::Panel>();
        if (panel) {
            auto children = panel.Children();
            uint32_t idx = 0;
            index = children.IndexOf(h.bg, idx) ? idx + 1 : children.Size();
        } else {
            // Fallback: inside TaskbarBackground's own panel, on top of the fill.
            if (wuxm::VisualTreeHelper::GetChildrenCount(h.bg) > 0) panel = wuxm::VisualTreeHelper::GetChild(h.bg, 0).try_as<wuxc::Panel>();
            if (!panel) return false;
            index = panel.Children().Size();
        }
        wuxc::Image img;
        img.Name(L"WavesillSpectrum");
        img.Stretch(wuxm::Stretch::Fill);
        img.HorizontalAlignment(wux::HorizontalAlignment::Left);
        img.VerticalAlignment(wux::VerticalAlignment::Top);
        img.IsHitTestVisible(false);
        img.Visibility(wux::Visibility::Collapsed);
        img.UseLayoutRounding(false);
        try { wuxc::Grid::SetColumnSpan(img, 64); wuxc::Grid::SetRowSpan(img, 64); } catch (...) {}
        panel.Children().InsertAt(index, img);
        h.parent = panel; h.image = img; h.inserted = true;
        stage(WS_STAGE_INSERTED);
        Wh_Log(L"Spectrum element inserted at index %u of %s (%u children)", index, winrt::get_class_name(panel).c_str(), panel.Children().Size());
        return true;
    } catch (const winrt::hresult_error& e) { logHr(L"Insert", e); return false; }
}

// Pairs a taskbar island with a slot by pixel size and DPI. Identical taskbars are told
// apart by WS_SLOT_PRIMARY: the first island seen is the primary taskbar.
static int matchSlot(int W, int H, int dpi, size_t hostIndex, ULONGLONG now) {
    int best = -1; bool bestPrimary = false;
    int live = 0, lastLive = -1;
    for (uint32_t i = 0; i < g_hdr->slotCount; ++i) {
        WsBridgeSlot* s = slotAt(i);
        if (!s->taskbarHwnd || now - s->appHeartbeatMs > 3000) continue;
        ++live; lastLive = (int)i;
        if (std::abs(s->width - W) > 2 || std::abs(s->height - H) > 2 || std::abs((int)s->dpi - dpi) > 2) continue;
        bool taken = false;
        for (size_t k = 0; k < g_hosts.size(); ++k) if (k != hostIndex && g_hosts[k].slot == (int)i) taken = true;
        if (taken) continue;
        const bool primary = (s->flags & WS_SLOT_PRIMARY) != 0;
        if (best < 0 || (hostIndex == 0 ? (primary && !bestPrimary) : (!primary && bestPrimary))) { best = (int)i; bestPrimary = primary; }
    }
    if (best < 0 && live == 1 && g_hosts.size() == 1) {
        // One taskbar and one slot whose sizes disagree (island versus window rounding):
        // there is nothing else it could be.
        WsBridgeSlot* s = slotAt(lastLive);
        if (now - g_lastMatchLog > 5000) { g_lastMatchLog = now; Wh_Log(L"Size mismatch tolerated: island %d × %d @ %d DPI, slot %d × %d @ %u DPI", W, H, dpi, s->width, s->height, s->dpi); }
        best = lastLive;
    } else if (best < 0 && live > 0 && now - g_lastMatchLog > 5000) {
        g_lastMatchLog = now;
        Wh_Log(L"No slot matches island %d × %d @ %d DPI (host %u of %u); live slots:", W, H, dpi, (unsigned)hostIndex, (unsigned)g_hosts.size());
        for (uint32_t i = 0; i < g_hdr->slotCount; ++i) { WsBridgeSlot* s = slotAt(i); if (s->taskbarHwnd && now - s->appHeartbeatMs <= 3000) Wh_Log(L"  Slot %u: %d × %d @ %u DPI, flags %u, visible %u, seq %u", i, s->width, s->height, s->dpi, s->flags, s->visible, s->frameSeq); }
    }
    if (best >= 0) stage(WS_STAGE_MATCHED);
    return best;
}

static void blit(Host& h, WsBridgeSlot* s, double scale) {
    const int w = s->width, hgt = s->height;
    if (w <= 0 || hgt <= 0 || (uint32_t)w * 4u * (uint32_t)hgt > s->bufferBytes) return;
    if (!h.bmp || h.bmpW != w || h.bmpH != hgt) {
        h.bmp = wuxmi::WriteableBitmap(w, hgt);
        h.bmpW = w; h.bmpH = hgt;
        h.image.Source(h.bmp);
        h.image.Width(w / scale);
        h.image.Height(hgt / scale);
    }
    auto buffer = h.bmp.PixelBuffer();
    IUnknown* unk = static_cast<IUnknown*>(winrt::get_abi(buffer));
    IBufferByteAccessMin* bba = nullptr;
    if (!unk || FAILED(unk->QueryInterface(IID_IBufferByteAccessMin, (void**)&bba)) || !bba) return;
    uint8_t* dst = nullptr;
    if (SUCCEEDED(bba->Buffer(&dst)) && dst) {
        const uint32_t front = s->front;
        MemoryBarrier();
        const uint8_t* src = g_base + s->bufferOffset[front & 1];
        const size_t rowBytes = (size_t)w * 4;
        if (s->stride == rowBytes) memcpy(dst, src, rowBytes * hgt);
        else for (int y = 0; y < hgt; ++y) memcpy(dst + y * rowBytes, src + (size_t)y * s->stride, rowBytes);
    }
    bba->Release();
    h.bmp.Invalidate();
    if (!(g_stage.load() & WS_STAGE_BLITTED)) { stage(WS_STAGE_BLITTED); Wh_Log(L"First frame copied: %d × %d px, image %.1f × %.1f DIP", w, hgt, w / scale, hgt / scale); }
}

static void setTimerInterval(int ms) {
    if (g_timerMs == ms || !g_timer) return;
    g_timerMs = ms;
    g_timer.Interval(std::chrono::milliseconds(ms));
}

static void tick() {
    if (g_unloading.load() || !g_hdr) return;
    const ULONGLONG now = GetTickCount64();
    if (!(g_stage.load() & WS_STAGE_TICKING)) { stage(WS_STAGE_TICKING); Wh_Log(L"First tick"); }
    bool anyLive = false;
    for (size_t i = 0; i < g_hosts.size(); ++i) {
        Host& h = g_hosts[i];
        if (h.broken) continue;
        try {
            if (!h.inserted) {
                if (now >= h.nextTryMs && h.insertTries < 600) { ++h.insertTries; h.nextTryMs = now + 500; tryInsert(h); }   // every 0.5 s, up to 5 min
                if (!h.inserted) continue;
            }
            auto root = h.bg.XamlRoot();
            if (!root) {   // detached from any tree (taskbar rebuilt); a fresh Add will follow
                if (h.slot >= 0) slotAt(h.slot)->modStatus = 0;
                removeImage(h); h.broken = true;
                continue;
            }
            const auto sz = root.Size();
            const double scale = root.RasterizationScale();
            const int W = (int)std::lround(sz.Width * scale), H = (int)std::lround(sz.Height * scale), dpi = (int)std::lround(96.0 * scale);
            if (h.slot < 0 || ++h.ticksSinceMatch >= 30) {
                h.ticksSinceMatch = 0;
                int m = matchSlot(W, H, dpi, i, now);
                if (m != h.slot) { if (h.slot >= 0) slotAt(h.slot)->modStatus = 0; h.slot = m; h.lastSeq = 0; }
            }
            if (h.slot < 0) { h.image.Visibility(wux::Visibility::Collapsed); continue; }
            WsBridgeSlot* s = slotAt(h.slot);
            anyLive = true;
            const bool appFresh = now - s->appHeartbeatMs < 3000;
            if (!appFresh || !s->visible) {
                if (h.image.Visibility() != wux::Visibility::Collapsed) h.image.Visibility(wux::Visibility::Collapsed);
            } else {
                if (s->frameSeq != h.lastSeq) { blit(h, s, scale); h.lastSeq = s->frameSeq; }
                if (h.image.Visibility() != wux::Visibility::Visible) h.image.Visibility(wux::Visibility::Visible);
            }
            s->modStatus = 1;
            MemoryBarrier();
            s->modHeartbeatMs = now;
        } catch (const winrt::hresult_error& e) { logHr(L"Tick", e); h.broken = true; }
          catch (...) { Wh_Log(L"Tick: unknown exception"); h.broken = true; }
    }
    if (!g_renderFallback.load()) setTimerInterval(anyLive ? 16 : 500);
}

// Fallback driver for the case where DispatcherTimer never ticks inside the island.
static void ensureRenderFallback() {
    if (g_renderFallback.load()) return;
    try {
        g_renderToken = wuxm::CompositionTarget::Rendering([](wf::IInspectable const&, wf::IInspectable const&) { tick(); });
        g_renderFallback.store(true);
        Wh_Log(L"DispatcherTimer never fired; switched to CompositionTarget.Rendering");
    } catch (const winrt::hresult_error& e) { logHr(L"Rendering fallback", e); }
}

static void ensureTimer() {
    if (g_timer) return;
    try {
        g_timer = wux::DispatcherTimer();
        g_timerMs = 16;
        g_timer.Interval(std::chrono::milliseconds(16));
        g_timer.Tick([](wf::IInspectable const&, wf::IInspectable const&) { tick(); });
        g_timer.Start();
        stage(WS_STAGE_TIMER);
        std::lock_guard<std::mutex> lk(g_dispMutex);
        try { g_queue = wsys::DispatcherQueue::GetForCurrentThread(); } catch (...) {}
        try { if (!g_hosts.empty()) g_dispatcher = g_hosts[0].bg.Dispatcher(); } catch (...) {}
        Wh_Log(L"Timer started (queue %s, dispatcher %s)", g_queue ? L"yes" : L"no", g_dispatcher ? L"yes" : L"no");
    } catch (const winrt::hresult_error& e) { logHr(L"Timer", e); }
}

static void onBackgroundAdded(InstanceHandle handle) {   // UI thread
    for (auto it = g_hosts.begin(); it != g_hosts.end();) { if (it->broken && !it->inserted) it = g_hosts.erase(it); else ++it; }
    for (auto& h : g_hosts) if (h.handle == handle) return;
    winrt::com_ptr<IXamlDiagnostics> diag;
    { std::lock_guard<std::mutex> lk(g_diagMutex); diag = g_diag; }
    if (!diag) return;
    IInspectable* raw = nullptr;
    if (FAILED(diag->GetIInspectableFromHandle(handle, &raw)) || !raw) return;
    winrt::com_ptr<IInspectable> insp; insp.attach(raw);
    wux::FrameworkElement fe{nullptr};
    try { fe = insp.try_as<wux::FrameworkElement>(); } catch (...) {}
    if (!fe) return;
    Host h; h.handle = handle; h.bg = fe;
    g_hosts.push_back(std::move(h));
    Wh_Log(L"TaskbarBackground found (%u so far)", (unsigned)g_hosts.size());
    if (g_unloading.load()) return;   // asleep: remembered now, inserted after the next wake-up
    stage(WS_STAGE_BACKGROUND);
    ensureTimer();
    tryInsert(g_hosts.back());
}
// Sleep / wake-up, both on the UI thread.
static void uiSuspend() {
    try { if (g_timer) { g_timer.Stop(); g_timer = nullptr; } } catch (...) {}
    try { if (g_renderFallback.load()) { wuxm::CompositionTarget::Rendering(g_renderToken); g_renderFallback.store(false); } } catch (...) {}
    g_timerMs = 0;
    for (auto& h : g_hosts) {
        try { if (h.image) h.image.Visibility(wux::Visibility::Collapsed); } catch (...) {}
        h.slot = -1; h.lastSeq = 0; h.ticksSinceMatch = 999;
    }
}
static void uiResume() {
    bool any = false;
    for (auto& h : g_hosts) {
        h.slot = -1; h.lastSeq = 0; h.ticksSinceMatch = 999; h.nextTryMs = 0; h.insertTries = 0;
        if (h.inserted) { stage(WS_STAGE_BACKGROUND | WS_STAGE_INSERTED); any = true; }
        else if (!h.broken) { stage(WS_STAGE_BACKGROUND); any = true; }
    }
    if (any) ensureTimer();
    Wh_Log(L"Resumed with %u known taskbar(s)", (unsigned)g_hosts.size());
}
static void onRemoved(InstanceHandle handle) {   // UI thread
    for (auto it = g_hosts.begin(); it != g_hosts.end(); ++it) {
        if (it->handle == handle) { if (it->slot >= 0 && g_hdr) slotAt(it->slot)->modStatus = 0; removeImage(*it); g_hosts.erase(it); Wh_Log(L"TaskbarBackground removed"); return; }
    }
}

// --------------------------------------------------------------------------- XAML diagnostics TAP
class VisualTreeWatcher final : public IVisualTreeServiceCallback2 {
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** pp) override {
        if (!pp) return E_POINTER;
        if (IsEqualIID(riid, WS_IID_IUnknown) || IsEqualIID(riid, WS_IID_IVisualTreeServiceCallback) || IsEqualIID(riid, WS_IID_IVisualTreeServiceCallback2)) { *pp = static_cast<IVisualTreeServiceCallback2*>(this); AddRef(); return S_OK; }
        *pp = nullptr; return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return (ULONG)InterlockedIncrement(&ref_); }
    STDMETHODIMP_(ULONG) Release() override { ULONG r = (ULONG)InterlockedDecrement(&ref_); if (!r) delete this; return r; }
    STDMETHODIMP OnVisualTreeChange(ParentChildRelation, VisualElement element, VisualMutationType mutation) override {
        if (mutation == Add) {
            const bool byType = element.Type && wcscmp(element.Type, L"Taskbar.TaskbarBackground") == 0;
            const bool byName = element.Name && wcscmp(element.Name, L"BackgroundControl") == 0;
            if (byType || byName) onBackgroundAdded(element.Handle);
        } else if (mutation == Remove) {
            onRemoved(element.Handle);
        }
        return S_OK;
    }
    STDMETHODIMP OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) override { return S_OK; }
private:
    LONG ref_ = 1;
};

class TapSite final : public IObjectWithSite {
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** pp) override {
        if (!pp) return E_POINTER;
        if (IsEqualIID(riid, WS_IID_IUnknown) || IsEqualIID(riid, WS_IID_IObjectWithSite)) { *pp = static_cast<IObjectWithSite*>(this); AddRef(); return S_OK; }
        *pp = nullptr; return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return (ULONG)InterlockedIncrement(&ref_); }
    STDMETHODIMP_(ULONG) Release() override { ULONG r = (ULONG)InterlockedDecrement(&ref_); if (!r) delete this; return r; }
    STDMETHODIMP SetSite(IUnknown* site) override {
        if (!site) {
            winrt::com_ptr<IVisualTreeService3> svc; IVisualTreeServiceCallback2* w = nullptr;
            { std::lock_guard<std::mutex> lk(g_diagMutex); svc = g_service; w = g_watcher; g_service = nullptr; g_diag = nullptr; g_watcher = nullptr; }
            if (svc && w) svc->UnadviseVisualTreeChange(w);
            if (w) w->Release();
            return S_OK;
        }
        winrt::com_ptr<IXamlDiagnostics> diag;
        if (FAILED(site->QueryInterface(WS_IID_IXamlDiagnostics, diag.put_void())) || !diag) { Wh_Log(L"The site is not IXamlDiagnostics"); return E_NOINTERFACE; }
        winrt::com_ptr<IVisualTreeService3> svc;
        if (FAILED(diag->QueryInterface(WS_IID_IVisualTreeService3, svc.put_void())) || !svc) { Wh_Log(L"IVisualTreeService3 is not available"); return E_NOINTERFACE; }
        IVisualTreeServiceCallback2* w = nullptr;
        {
            std::lock_guard<std::mutex> lk(g_diagMutex);
            g_diag = diag; g_service = svc;
            if (!g_watcher) g_watcher = new VisualTreeWatcher();
            w = g_watcher; w->AddRef();
        }
        // Advise replays the whole existing tree through OnVisualTreeChange synchronously,
        // so it must run without g_diagMutex held.
        HRESULT hr = svc->AdviseVisualTreeChange(w);
        w->Release();
        if (SUCCEEDED(hr)) stage(WS_STAGE_ADVISED);
        Wh_Log(L"AdviseVisualTreeChange: 0x%08X", (unsigned)hr);
        return hr;
    }
    STDMETHODIMP GetSite(REFIID riid, void** pp) override {
        std::lock_guard<std::mutex> lk(g_diagMutex);
        if (!g_diag) { *pp = nullptr; return E_FAIL; }
        return g_diag->QueryInterface(riid, pp);
    }
private:
    LONG ref_ = 1;
};

class TapFactory final : public IClassFactory {
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** pp) override {
        if (!pp) return E_POINTER;
        if (IsEqualIID(riid, WS_IID_IUnknown) || IsEqualIID(riid, WS_IID_IClassFactory)) { *pp = static_cast<IClassFactory*>(this); AddRef(); return S_OK; }
        *pp = nullptr; return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return (ULONG)InterlockedIncrement(&ref_); }
    STDMETHODIMP_(ULONG) Release() override { ULONG r = (ULONG)InterlockedDecrement(&ref_); if (!r) delete this; return r; }
    STDMETHODIMP CreateInstance(IUnknown* outer, REFIID riid, void** pp) override {
        if (outer) return CLASS_E_NOAGGREGATION;
        TapSite* s = new TapSite();
        HRESULT hr = s->QueryInterface(riid, pp);
        s->Release();
        return hr;
    }
    STDMETHODIMP LockServer(BOOL) override { return S_OK; }
private:
    LONG ref_ = 1;
};

// XAML looks these two up by name in this DLL. They must really be exported; without the
// export XAML fails fast and takes Explorer down with no message.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
#endif
extern "C" __declspec(dllexport) HRESULT WINAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    if (!IsEqualCLSID(rclsid, CLSID_WavesillTAP)) return CLASS_E_CLASSNOTAVAILABLE;
    TapFactory* f = new TapFactory();
    HRESULT hr = f->QueryInterface(riid, ppv);
    f->Release();
    return hr;
}
extern "C" __declspec(dllexport) HRESULT WINAPI DllCanUnloadNow() { return S_FALSE; }
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// --------------------------------------------------------------------------- bridge thread
// Keeps the header heartbeat alive independently of XAML, attaches the diagnostics session
// once the XAML taskbar exists, disarms the crash guard after 20 stable seconds, and arms
// the Rendering fallback if the timer never ticks.
static DWORD WINAPI bridgeThread(LPVOID) {
    bool diagDone = false; int waitTicks = 0; ULONGLONG timerSeenAt = 0, diagOkAt = 0;
    if (!g_attach) { stage(WS_STAGE_OFF); diagDone = true; Wh_Log(L"Attach is disabled in the settings; staying inert"); }
    else if (g_guardTripped) { stage(WS_STAGE_GUARD); diagDone = true; }
    else {
        // Already attached by a previous session of this DLL: just wake the UI side up.
        bool attached; wsys::DispatcherQueue q{nullptr};
        { std::lock_guard<std::mutex> lk(g_diagMutex); attached = g_service != nullptr; }
        { std::lock_guard<std::mutex> lk(g_dispMutex); q = g_queue; }
        if (attached) {
            stage(WS_STAGE_XAML | WS_STAGE_DIAG | WS_STAGE_ADVISED);
            bool posted = false;
            try { if (q) posted = q.TryEnqueue(wsys::DispatcherQueueHandler([]() { uiResume(); })); } catch (...) {}
            if (posted) { diagDone = true; Wh_Log(L"XAML already attached; resuming"); }
            else Wh_Log(L"XAML already attached but the UI thread cannot be reached; attaching again");
        }
    }
    while (!g_unloading.load()) {
        const ULONGLONG now = GetTickCount64();
        if (g_hdr) { g_hdr->modHeartbeatMs = now; g_hdr->flags = g_stage.load(); }
        if (diagOkAt && now - diagOkAt > 20000) { guardDisarm(); diagOkAt = 0; Wh_Log(L"Attach stable for 20 s; crash guard disarmed"); }
        if (!diagDone && waitTicks < 1200) {   // up to 5 minutes
            ++waitTicks;
            if (GetModuleHandleW(L"Taskbar.View.dll") && GetModuleHandleW(L"Windows.UI.Xaml.dll")) {
                stage(WS_STAGE_XAML);
                if (waitTicks < 6) { Sleep(250); continue; }   // give a freshly loaded taskbar a moment
                typedef HRESULT (WINAPI* InitFn)(PCWSTR, DWORD, PCWSTR, PCWSTR, CLSID, PCWSTR);
                HMODULE xaml = GetModuleHandleW(L"Windows.UI.Xaml.dll");
                InitFn fn = xaml ? (InitFn)GetProcAddress(xaml, "InitializeXamlDiagnosticsEx") : nullptr;
                if (!fn) { Wh_Log(L"InitializeXamlDiagnosticsEx not found"); diagDone = true; }
                else if (waitTicks % 12 == 6) {   // one attempt every 3 s until it succeeds
                    wchar_t path[MAX_PATH]; GetModuleFileNameW(g_hModule, path, MAX_PATH);
                    guardArm();
                    // "VisualDiagConnection1" is the endpoint name Visual Studio, TranslucentTB
                    // and the Taskbar Styler use; other names are rejected with ERROR_NOT_FOUND.
                    HRESULT hr = fn(L"VisualDiagConnection1", GetCurrentProcessId(), nullptr, path, CLSID_WavesillTAP, nullptr);
                    Wh_Log(L"InitializeXamlDiagnosticsEx: 0x%08X", (unsigned)hr);
                    if (SUCCEEDED(hr)) { stage(WS_STAGE_DIAG); diagDone = true; diagOkAt = now; }
                    else if (waitTicks >= 6 + 12 * 20) { Wh_Log(L"Giving up on XAML diagnostics after 20 attempts"); diagDone = true; guardDisarm(); }
                }
            }
        }
        // Timer created but never ticked for 3 s: drive ticks from CompositionTarget.Rendering.
        const uint32_t st = g_stage.load();
        if ((st & WS_STAGE_TIMER) && !(st & WS_STAGE_TICKING) && !g_renderFallback.load()) {
            if (!timerSeenAt) timerSeenAt = now;
            else if (now - timerSeenAt > 3000) {
                wsys::DispatcherQueue q{nullptr};
                { std::lock_guard<std::mutex> lk(g_dispMutex); q = g_queue; }
                bool posted = false;
                try { if (q) posted = q.TryEnqueue(wsys::DispatcherQueueHandler([]() { ensureRenderFallback(); })); } catch (...) {}
                if (!posted) Wh_Log(L"The UI thread cannot be reached for the Rendering fallback");
                timerSeenAt = now + 10000;   // retry in 10 s if it still does not tick
            }
        }
        Sleep(250);
    }
    return 0;
}

// --------------------------------------------------------------------------- init / uninit
BOOL Wh_ModInit() {
    Wh_Log(L"Wavesill Behind Taskbar Content 0.6.3: init");
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&Wh_ModInit, &g_hModule);
    // Per-session reset (see the note above the globals).
    g_unloading.store(false);
    g_stage.store(0);
    g_renderFallback.store(false);
    g_guardTripped = false;
    g_lastMatchLog = 0;
    g_attach = Wh_GetIntSetting(L"attach") != 0;
    guardInit();
    void* gco = g_hModule ? (void*)GetProcAddress(g_hModule, "DllGetClassObject") : nullptr;
    if (!gco) Wh_Log(L"DllGetClassObject is not exported; the XAML attach would crash Explorer, so it is disabled");
    if (!gco) g_attach = false;
    if (!bridgeCreate()) return FALSE;
    g_bridgeThread = CreateThread(nullptr, 0, bridgeThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    g_attach = Wh_GetIntSetting(L"attach") != 0;
    Wh_Log(L"Settings changed: attach = %d (takes effect when the mod is re-enabled)", g_attach ? 1 : 0);
}

void Wh_ModUninit() {
    Wh_Log(L"Wavesill Behind Taskbar Content: uninit");
    g_unloading.store(true);
    guardDisarm();
    if (g_bridgeThread) { WaitForSingleObject(g_bridgeThread, 3000); CloseHandle(g_bridgeThread); g_bridgeThread = nullptr; }

    // Put the UI side to sleep (timer off, elements collapsed) and wait for it. The XAML
    // attach itself stays in place for the next session; Explorer's restart clears it.
    wuc::CoreDispatcher disp{nullptr}; wsys::DispatcherQueue queue{nullptr};
    { std::lock_guard<std::mutex> lk(g_dispMutex); disp = g_dispatcher; queue = g_queue; }
    if (disp || queue) {
        HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        auto sleep = [done]() { uiSuspend(); SetEvent(done); };
        bool posted = false;
        try { if (queue) posted = queue.TryEnqueue(wsys::DispatcherQueueHandler(sleep)); } catch (...) {}
        if (!posted && disp) { try { disp.RunAsync(wuc::CoreDispatcherPriority::Normal, wuc::DispatchedHandler(sleep)); posted = true; } catch (...) {} }
        if (posted) WaitForSingleObject(done, 3000);
        CloseHandle(done);
    }
    bridgeClose();
}
