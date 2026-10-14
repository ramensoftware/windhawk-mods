// ==WindhawkMod==
// @id              wavesill-behind-taskbar-content
// @name            Wavesill Behind Taskbar Content
// @name:zh-CN      栏声 (Wavesill) 置于任务栏内容之下
// @description     Moves the Wavesill audio spectrum behind everything on the Windows 11 taskbar — icons, search, tray and clock — so it never covers them
// @description:zh-CN 把栏声 (Wavesill) 的音频频谱放到 Windows 11 任务栏上所有内容 (图标、搜索、托盘、时钟) 的后面，不再遮挡它们
// @version         0.7.0
// @author          GenuineHorace
// @github          https://github.com/GenuineHorace
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Wavesill Behind Taskbar Content

Companion mod for [Wavesill](https://github.com/GenuineHorace/Wavesill), the faint
audio spectrum for the Windows taskbar.

On its own, Wavesill draws its spectrum in a click-through window *on top of* the
taskbar. With this mod the spectrum sits *inside* the taskbar instead: beneath the Start
button, app icons, tray and clock, and above the acrylic backdrop, so nothing is ever
drawn over an icon.

## How It Works

* Wavesill keeps doing all the work: audio capture, analysis, styles, colours, opacity and
  per-monitor settings. Every frame it hands the finished picture to this mod through a
  small shared-memory block (`Local\Wavesill.Bridge`).
* The mod reaches each taskbar's XAML tree from its window, on the taskbar's own thread,
  inserts one `Image` element right after the `TaskbarBackground` element, and copies
  Wavesill's frames into it. Each taskbar is paired with its frames by window handle, so
  several monitors are never mixed up.
* Heartbeats run both ways. While the mod is displaying a taskbar, Wavesill hides its own
  overlay there; if the mod is disabled, Windhawk is closed or a Windows update breaks it,
  Wavesill's overlay comes back within a second. Nothing is ever half-drawn.

## Requirements

* Windows 11 (the XAML taskbar), x64 or ARM64. Windows 10 keeps using Wavesill's overlay.
* Wavesill 0.6.2 or later running. Its About window shows "Windhawk Mod: Connected", a
  "Mod Stage" line (9/9 when frames are flowing), and `[Mod]` on each taskbar line.
* The first time the mod is enabled, Windhawk downloads a few MB of Windows symbols; that
  can take a minute and needs an internet connection.

## Notes

* The mod hooks no functions. It looks up a few `taskbar.dll` symbols to find each
  taskbar's XAML root, the same way the other taskbar mods do.
* Disabling or removing the mod takes the element out of the taskbar and unloads the mod
  completely; nothing is left behind in Explorer or on disk.

---

# 栏声 (Wavesill) 置于任务栏内容之下

[栏声 (Wavesill)](https://github.com/GenuineHorace/Wavesill) 的配套模组。栏声是一个显示在
Windows 任务栏上的淡淡音频频谱。

栏声自己只能把频谱画在一个点击穿透的窗口里、浮在任务栏**上面**。装上这个模组后，频谱会显示在任务栏
**内部**：在开始按钮、程序图标、托盘和时钟之下、亚克力背景之上，永远不会盖住任何图标。

## 工作方式

* 所有工作仍由栏声完成：采集、分析、样式、颜色、浓度、每显示器设置。它每帧把画好的图片通过一小块
  共享内存 (`Local\Wavesill.Bridge`) 交给本模组。
* 模组从每条任务栏的窗口找到它的 XAML 树，在任务栏自己的线程上、`TaskbarBackground` 元素之后插入一个
  `Image`，把栏声的帧拷进去。任务栏和帧按窗口句柄配对，多显示器不会串。
* 双向心跳。模组显示某条任务栏时，栏声隐藏自己在那里的覆盖层；模组被禁用、Windhawk 关闭或
  Windows 更新把它弄坏时，栏声的覆盖层在一秒内自动回来。不会出现画一半的情况。

## 要求

* Windows 11 (XAML 任务栏)，x64 或 ARM64。Windows 10 继续使用栏声的覆盖层。
* 栏声 0.6.2 或更新版本正在运行。它的"关于"窗口会显示"Windhawk 模组：已连接"、"模组阶段"
  (帧在流动时为 9/9)，每条任务栏后面标 `[模组]`。
* 第一次启用时 Windhawk 会下载几 MB 的 Windows 符号，可能需要一分钟，并且需要联网。

## 说明

* 不 hook 任何函数。只查找几个 `taskbar.dll` 符号来定位每条任务栏的 XAML 根，和其它任务栏模组的做法一样。
* 禁用或移除模组会把元素从任务栏里取出并完全卸载模组，Explorer 里和磁盘上都不留任何东西。
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

// winbase.h defines GetCurrentTime() as a macro; C++/WinRT's XAML animation headers
// declare a method with that name.
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <vector>

namespace wf = winrt::Windows::Foundation;
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
    WS_STAGE_SYMBOLS = 1u,     /* taskbar.dll symbols resolved                           */
    WS_STAGE_THREAD = 2u,      /* the taskbar's UI thread was reached                    */
    WS_STAGE_TIMER = 4u,       /* DispatcherTimer created on that thread                 */
    WS_STAGE_TICKING = 8u,     /* first tick ran                                         */
    WS_STAGE_ROOT = 16u,       /* a taskbar's XamlRoot was obtained                      */
    WS_STAGE_BACKGROUND = 32u, /* a Taskbar.TaskbarBackground element was found          */
    WS_STAGE_INSERTED = 64u,   /* the mod's Image is in the tree                         */
    WS_STAGE_MATCHED = 128u,   /* a taskbar was paired with a slot                       */
    WS_STAGE_BLITTED = 256u,   /* at least one frame was copied into the taskbar         */
    WS_STAGE_COUNT = 9u,
    WS_STAGE_OFF = 0x4000u,    /* mods before 0.7.0: attach disabled in the settings     */
    WS_STAGE_GUARD = 0x8000u,  /* mods before 0.7.0: attach skipped after a crash        */
};
static_assert(sizeof(WsBridgeHeader) == 128, "Header layout");
static_assert(sizeof(WsBridgeSlot) == 144, "Slot layout");

// --------------------------------------------------------------------------- globals
// Everything that holds a XAML object lives on the taskbar's UI thread and is released
// there in Wh_ModUninit; the no_destroy attribute keeps the CRT from releasing it on the
// wrong thread if Explorer exits without unloading the mod.
static std::atomic<bool> g_unloading{false};
static std::atomic<uint32_t> g_stage{0};
static std::atomic<bool> g_uiStarted{false};
static uint32_t g_modVersion = 0;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_bridgeThread = nullptr;
static void stage(uint32_t bit) { g_stage.fetch_or(bit); }

// Bridge (created by this mod, read and written by Wavesill). The layout is fixed here and
// never read back from the mapping, which Wavesill (or any process of the same user) can
// write to.
static constexpr uint32_t kSlotCount = 4;
static constexpr uint32_t kBufferBytes = 3u * 1024u * 1024u;   // 3 MB per buffer: up to 5120 × 150 px
static constexpr uint32_t kHeaderSize = sizeof(WsBridgeHeader);
static constexpr uint32_t kSlotSize = sizeof(WsBridgeSlot);
static constexpr uint32_t kPixelBase = (kHeaderSize + kSlotCount * kSlotSize + 4095u) & ~4095u;
static constexpr uint32_t kTotalBytes = kPixelBase + kSlotCount * 2u * kBufferBytes;
static constexpr uint32_t bufferOffset(uint32_t slot, uint32_t front) { return kPixelBase + (slot * 2u + front) * kBufferBytes; }
static HANDLE g_map = nullptr;
static uint8_t* g_base = nullptr;
static WsBridgeHeader* g_hdr = nullptr;
static WsBridgeSlot* slotAt(uint32_t i) { return (WsBridgeSlot*)(g_base + kHeaderSize + (size_t)i * kSlotSize); }

// UI-thread state: touched only on the taskbar's UI thread.
struct Host {
    HWND hwnd = nullptr;
    bool secondary = false;
    wuxc::Panel parent{nullptr};
    wuxc::Image image{nullptr};
    wuxmi::WriteableBitmap bmp{nullptr};
    int bmpW = 0, bmpH = 0;
    int slot = -1;
    uint32_t lastSeq = 0;
};
[[clang::no_destroy]] static std::vector<Host> g_hosts;
[[clang::no_destroy]] static wux::DispatcherTimer g_timer{nullptr};
static int g_timerMs = 0;
static int g_ticksSinceScan = 0;
static ULONGLONG g_lastAttachLog = 0, g_lastMatchLog = 0;

// --------------------------------------------------------------------------- bridge
static bool bridgeCreate() {
    g_map = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, kTotalBytes, WS_BRIDGE_MAPPING_NAME);
    if (!g_map) { Wh_Log(L"CreateFileMapping failed: %u", GetLastError()); return false; }
    const bool existed = GetLastError() == ERROR_ALREADY_EXISTS;
    g_base = (uint8_t*)MapViewOfFile(g_map, FILE_MAP_ALL_ACCESS, 0, 0, kTotalBytes);
    if (!g_base) { Wh_Log(L"MapViewOfFile failed: %u", GetLastError()); CloseHandle(g_map); g_map = nullptr; return false; }
    g_hdr = (WsBridgeHeader*)g_base;
    if (!existed || g_hdr->magic != WS_BRIDGE_MAGIC || g_hdr->headerSize != kHeaderSize || g_hdr->slotSize != kSlotSize ||
        g_hdr->slotCount != kSlotCount || g_hdr->totalBytes != kTotalBytes) {
        memset(g_base, 0, kPixelBase);   // fresh (or foreign) mapping: lay it out from scratch
    }
    g_hdr->magic = WS_BRIDGE_MAGIC; g_hdr->protocol = WS_BRIDGE_PROTOCOL;
    g_hdr->headerSize = kHeaderSize; g_hdr->slotSize = kSlotSize; g_hdr->slotCount = kSlotCount;
    g_hdr->slotBytes = 2u * kBufferBytes; g_hdr->totalBytes = kTotalBytes; g_hdr->modVersion = g_modVersion;
    for (uint32_t i = 0; i < kSlotCount; ++i) {
        WsBridgeSlot* s = slotAt(i);
        s->bufferOffset[0] = bufferOffset(i, 0);
        s->bufferOffset[1] = bufferOffset(i, 1);
        s->bufferBytes = kBufferBytes;
        s->modHeartbeatMs = 0; s->modStatus = 0;
    }
    g_hdr->modHeartbeatMs = GetTickCount64();
    Wh_Log(L"Bridge ready: %u slots, %u bytes%s", kSlotCount, kTotalBytes, existed ? L" (mapping already existed)" : L"");
    return true;
}
static void bridgeClose() {
    if (g_hdr) {
        for (uint32_t i = 0; i < kSlotCount; ++i) { slotAt(i)->modStatus = 0; slotAt(i)->modHeartbeatMs = 0; }
        g_hdr->modHeartbeatMs = 0;
    }
    if (g_base) UnmapViewOfFile(g_base);
    if (g_map) CloseHandle(g_map);
    g_base = nullptr; g_hdr = nullptr; g_map = nullptr;
}

// --------------------------------------------------------------------------- taskbar XAML root
// The taskbar's XAML tree is reached from its window through a few taskbar.dll symbols,
// the way the other taskbar mods do it (after the Taskbar Notification Icon Spacing and
// Hide Start Button mods, MIT). No function is hooked; the symbols are only looked up.
static void* CTaskBand_ITaskListWndSite_vftable;
static void* CSecondaryTaskBand_ITaskListWndSite_vftable;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
static CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;
using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
static CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;
static void* TaskbarHost_FrameHeight_Original;
using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
static std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

static bool resolveTaskbarSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) { Wh_Log(L"taskbar.dll is not available (Windows 10?)"); return false; }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"}, &CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"}, &CSecondaryTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &std__Ref_count_base__Decref_Original},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// The offset of the taskbar's root element inside TaskbarHost is read from the first
// instructions of TaskbarHost::FrameHeight, which loads that very member.
static size_t taskbarElementOffset() {
#if defined(_M_X64)
    // 48:83EC 28 | sub rsp,28
    // 48:83C1 48 | add rcx,48
    const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
    if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) return b[7];
    Wh_Log(L"Unsupported TaskbarHost::FrameHeight; using the usual offset");
    return 0x48;
#elif defined(_M_ARM64)
    // 7f2303d5 pacibsp
    // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
    // fd030091 mov     fp, sp
    // 080c41f8 ldr     x8, [x0, #0x10]!
    const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
    if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD && p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) return (p[3] >> 12) & 0xFF;
    Wh_Log(L"Unsupported TaskbarHost::FrameHeight; using the usual offset");
    return 0x10;
#else
#error "Unsupported architecture"
#endif
}

static wux::XamlRoot taskbarXamlRoot(HWND hTaskbarWnd, bool secondary) {
    HWND hTaskSwWnd = secondary ? FindWindowEx(hTaskbarWnd, nullptr, L"WorkerW", nullptr) : (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;
    void* vftable = secondary ? CSecondaryTaskBand_ITaskListWndSite_vftable : CTaskBand_ITaskListWndSite_vftable;
    void* p = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!p) return nullptr;
    for (int i = 0; *(void**)p != vftable; ++i) {
        if (i == 20) return nullptr;
        p = (void**)p + 1;
    }
    void* sharedPtr[2]{};
    if (secondary) CSecondaryTaskBand_GetTaskbarHost_Original(p, sharedPtr);
    else CTaskBand_GetTaskbarHost_Original(p, sharedPtr);
    if (!sharedPtr[0] && !sharedPtr[1]) return nullptr;
    auto* unk = *(IUnknown**)((BYTE*)sharedPtr[0] + taskbarElementOffset());
    wux::FrameworkElement element{nullptr};
    if (unk) unk->QueryInterface(winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(element));
    wux::XamlRoot root = element ? element.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(sharedPtr[1]);
    return root;
}

// Runs proc on the thread that owns hWnd and waits for it. A WH_CALLWNDPROC hook on that
// thread catches a registered message sent to the window.
using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);
static bool runFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT msg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param { RunFromWindowThreadProc_t proc; void* procParam; };
    const DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) return false;
    if (threadId == GetCurrentThreadId()) { proc(procParam); return true; }
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
        if (nCode == HC_ACTION) {
            const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
            if (cwp->message == msg) { Param* param = (Param*)cwp->lParam; param->proc(param->procParam); }
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }, nullptr, threadId);
    if (!hook) return false;
    Param param{proc, procParam};
    SendMessage(hWnd, msg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

static HWND findTaskbarWnd() {
    HWND hWnd = nullptr;
    while ((hWnd = FindWindowEx(nullptr, hWnd, L"Shell_TrayWnd", nullptr))) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid == GetCurrentProcessId()) return hWnd;
    }
    return nullptr;
}

// --------------------------------------------------------------------------- UI-thread work
static void logHr(const wchar_t* what, const winrt::hresult_error& e) {
    Wh_Log(L"%s: 0x%08X %s", what, (unsigned)e.code().value, e.message().c_str());
}

static wux::FrameworkElement findBackground(wux::DependencyObject const& node, int depth) {
    if (!node || depth < 0) return nullptr;
    if (auto fe = node.try_as<wux::FrameworkElement>()) {
        if (winrt::get_class_name(fe) == L"Taskbar.TaskbarBackground" || fe.Name() == L"BackgroundControl") return fe;
    }
    const int n = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; ++i) {
        if (auto found = findBackground(wuxm::VisualTreeHelper::GetChild(node, i), depth - 1)) return found;
    }
    return nullptr;
}

static void removeImage(Host& h) {
    try {
        if (h.parent && h.image) { uint32_t i = 0; auto ch = h.parent.Children(); if (ch.IndexOf(h.image, i)) ch.RemoveAt(i); }
    } catch (...) {}
    if (h.slot >= 0 && g_hdr) slotAt(h.slot)->modStatus = 0;
    h.parent = nullptr; h.image = nullptr; h.bmp = nullptr; h.slot = -1;
}

static void attach(HWND hwnd, bool secondary, ULONGLONG now) {
    try {
        auto root = taskbarXamlRoot(hwnd, secondary);
        if (!root) return;   // the XAML frame is not up yet; the next scan tries again
        stage(WS_STAGE_ROOT);
        auto bg = findBackground(root.Content(), 8);
        if (!bg) {
            if (now - g_lastAttachLog > 5000) { g_lastAttachLog = now; Wh_Log(L"No TaskbarBackground under the root of taskbar %p yet", hwnd); }
            return;
        }
        stage(WS_STAGE_BACKGROUND);
        wuxc::Panel panel{nullptr};
        uint32_t index = 0;
        if (auto parentObj = bg.Parent()) panel = parentObj.try_as<wuxc::Panel>();
        if (panel) {
            uint32_t idx = 0;
            index = panel.Children().IndexOf(bg, idx) ? idx + 1 : panel.Children().Size();
        } else {
            // Fallback: inside TaskbarBackground's own panel, on top of the fill.
            if (wuxm::VisualTreeHelper::GetChildrenCount(bg) > 0) panel = wuxm::VisualTreeHelper::GetChild(bg, 0).try_as<wuxc::Panel>();
            if (!panel) { Wh_Log(L"TaskbarBackground has no panel to insert into"); return; }
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
        Host h; h.hwnd = hwnd; h.secondary = secondary; h.parent = panel; h.image = img;
        g_hosts.push_back(std::move(h));
        stage(WS_STAGE_INSERTED);
        Wh_Log(L"Spectrum element inserted at index %u of %s in %s taskbar %p (%u taskbars)", index, winrt::get_class_name(panel).c_str(), secondary ? L"secondary" : L"primary", hwnd, (unsigned)g_hosts.size());
    } catch (const winrt::hresult_error& e) { logHr(L"Attach", e); }
}

// Drops taskbars that are gone or whose tree was rebuilt, and attaches new ones.
static void scanTaskbars(ULONGLONG now) {
    for (size_t i = 0; i < g_hosts.size();) {
        Host& h = g_hosts[i];
        bool alive = IsWindow(h.hwnd) != FALSE;
        if (alive) { try { alive = h.image.XamlRoot() != nullptr; } catch (...) { alive = false; } }
        if (alive) { ++i; continue; }
        Wh_Log(L"Taskbar %p is gone or was rebuilt; the element will be inserted again", h.hwnd);
        removeImage(h);
        g_hosts.erase(g_hosts.begin() + i);
    }
    struct Found { HWND hwnd[16]; bool secondary[16]; int count = 0; } found;
    EnumThreadWindows(GetCurrentThreadId(), [](HWND hWnd, LPARAM lParam) -> BOOL {
        Found& f = *(Found*)lParam;
        if (f.count >= 16) return FALSE;
        WCHAR cls[32];
        if (!GetClassName(hWnd, cls, ARRAYSIZE(cls))) return TRUE;
        const bool primary = _wcsicmp(cls, L"Shell_TrayWnd") == 0, secondary = _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
        if (primary || secondary) { f.hwnd[f.count] = hWnd; f.secondary[f.count] = secondary; ++f.count; }
        return TRUE;
    }, (LPARAM)&found);
    for (int i = 0; i < found.count; ++i) {
        bool known = false;
        for (auto& h : g_hosts) if (h.hwnd == found.hwnd[i]) known = true;
        if (!known) attach(found.hwnd[i], found.secondary[i], now);
    }
}

static int findSlot(HWND hwnd, ULONGLONG now) {
    for (uint32_t i = 0; i < kSlotCount; ++i) {
        const WsBridgeSlot* s = slotAt(i);
        if (s->taskbarHwnd == (uint64_t)(uintptr_t)hwnd && now - s->appHeartbeatMs < 3000) { stage(WS_STAGE_MATCHED); return (int)i; }
    }
    return -1;
}

static void blit(Host& h, const WsBridgeSlot* s, double scale) {
    // One snapshot of the per-frame fields; Wavesill may change them at any time.
    const int32_t w = s->width, hgt = s->height;
    const uint32_t stride = s->stride, front = s->front & 1u;
    if (w <= 0 || hgt <= 0 || w > 16384 || hgt > 16384) return;
    const uint64_t rowBytes = (uint64_t)w * 4u;
    if (stride < rowBytes || (uint64_t)stride * (uint64_t)(hgt - 1) + rowBytes > kBufferBytes) return;
    if (!h.bmp || h.bmpW != w || h.bmpH != hgt) {
        h.bmp = wuxmi::WriteableBitmap(w, hgt);
        h.bmpW = w; h.bmpH = hgt;
        h.image.Source(h.bmp);
        h.image.Width(w / scale);
        h.image.Height(hgt / scale);
    }
    auto buffer = h.bmp.PixelBuffer();
    uint8_t* dst = buffer.data();
    if (!dst || buffer.Capacity() < rowBytes * (uint64_t)hgt) return;
    MemoryBarrier();
    const uint8_t* src = g_base + bufferOffset((uint32_t)h.slot, front);
    if (stride == rowBytes) memcpy(dst, src, (size_t)(rowBytes * hgt));
    else for (int32_t y = 0; y < hgt; ++y) memcpy(dst + (size_t)y * rowBytes, src + (size_t)y * stride, (size_t)rowBytes);
    h.bmp.Invalidate();
    if (!(g_stage.load() & WS_STAGE_BLITTED)) { stage(WS_STAGE_BLITTED); Wh_Log(L"First frame copied: %d × %d px, image %.1f × %.1f DIP", w, hgt, w / scale, hgt / scale); }
}

static void setTimerInterval(int ms) {
    if (g_timerMs == ms || !g_timer) return;
    g_timerMs = ms;
    g_timer.Interval(std::chrono::milliseconds(ms));
}

static void tick() {
    try {
        if (g_unloading.load() || !g_hdr) return;
        const ULONGLONG now = GetTickCount64();
        if (!(g_stage.load() & WS_STAGE_TICKING)) { stage(WS_STAGE_TICKING); Wh_Log(L"First tick"); }
        // Look for new, gone or rebuilt taskbars about twice a second.
        if (++g_ticksSinceScan >= (g_timerMs == 16 ? 30 : 1)) { g_ticksSinceScan = 0; scanTaskbars(now); }
        bool anyLive = false;
        for (Host& h : g_hosts) {
            if (h.slot < 0 || slotAt(h.slot)->taskbarHwnd != (uint64_t)(uintptr_t)h.hwnd || now - slotAt(h.slot)->appHeartbeatMs >= 3000) {
                const int m = findSlot(h.hwnd, now);
                if (m != h.slot) {
                    if (h.slot >= 0) slotAt(h.slot)->modStatus = 0;
                    h.slot = m; h.lastSeq = 0;
                    if (m < 0 && now - g_lastMatchLog > 5000) { g_lastMatchLog = now; Wh_Log(L"No live slot for taskbar %p (Wavesill not running, or older than 0.6.2)", h.hwnd); }
                }
            }
            if (h.slot < 0) {
                if (h.image.Visibility() != wux::Visibility::Collapsed) h.image.Visibility(wux::Visibility::Collapsed);
                continue;
            }
            WsBridgeSlot* s = slotAt(h.slot);
            anyLive = true;
            if (!s->visible) {
                if (h.image.Visibility() != wux::Visibility::Collapsed) h.image.Visibility(wux::Visibility::Collapsed);
            } else {
                const uint32_t seq = s->frameSeq;
                if (seq != h.lastSeq) {
                    auto root = h.image.XamlRoot();
                    if (root) blit(h, s, root.RasterizationScale());
                    h.lastSeq = seq;
                }
                if (h.image.Visibility() != wux::Visibility::Visible) h.image.Visibility(wux::Visibility::Visible);
            }
            s->modStatus = 1;
            MemoryBarrier();
            s->modHeartbeatMs = now;
        }
        setTimerInterval(anyLive ? 16 : 500);
    } catch (const winrt::hresult_error& e) { logHr(L"Tick", e); }
      catch (...) { Wh_Log(L"Tick: unknown exception"); }
}

// Runs on the taskbar thread once, from the bridge thread.
static void WINAPI uiStart(void*) {
    try {
        if (g_timer) return;
        g_timer = wux::DispatcherTimer();
        g_timerMs = 500;
        g_timer.Interval(std::chrono::milliseconds(500));
        g_timer.Tick([](wf::IInspectable const&, wf::IInspectable const&) { tick(); });
        g_timer.Start();
        g_uiStarted.store(true);
        stage(WS_STAGE_TIMER);
        Wh_Log(L"Timer started on the taskbar thread");
    } catch (const winrt::hresult_error& e) { logHr(L"Timer", e); }
      catch (...) { Wh_Log(L"Timer: unknown exception"); }
}

// Runs on the taskbar thread once, from Wh_ModUninit: takes everything out of the tree and
// releases every XAML object on the thread that owns it.
static void WINAPI uiStop(void*) {
    try { if (g_timer) g_timer.Stop(); } catch (...) {}
    g_timer = nullptr;
    g_timerMs = 0;
    for (Host& h : g_hosts) removeImage(h);
    g_hosts.clear();
    g_hosts.shrink_to_fit();
    g_uiStarted.store(false);
    Wh_Log(L"Elements removed; UI side stopped");
}

// --------------------------------------------------------------------------- bridge thread
// Keeps the header heartbeat and the progress bits fresh, and starts the UI side once the
// taskbar window exists.
static DWORD WINAPI bridgeThread(LPVOID) {
    ULONGLONG lastStartTry = 0;
    while (WaitForSingleObject(g_stopEvent, 250) == WAIT_TIMEOUT) {
        const ULONGLONG now = GetTickCount64();
        if (g_hdr) { g_hdr->modHeartbeatMs = now; g_hdr->flags = g_stage.load(); }
        if (!g_uiStarted.load() && now - lastStartTry >= 1000) {
            lastStartTry = now;
            if (HWND taskbar = findTaskbarWnd()) {
                if (runFromWindowThread(taskbar, uiStart, nullptr)) stage(WS_STAGE_THREAD);
                else Wh_Log(L"The taskbar thread could not be reached; retrying");
            }
        }
    }
    return 0;
}

// --------------------------------------------------------------------------- init / uninit
BOOL Wh_ModInit() {
    Wh_Log(L"Wavesill Behind Taskbar Content %s: init", WH_MOD_VERSION);
    unsigned major = 0, minor = 0, patch = 0;
    swscanf(WH_MOD_VERSION, L"%u.%u.%u", &major, &minor, &patch);
    g_modVersion = (major << 16) | (minor << 8) | patch;
    g_unloading.store(false);
    g_stage.store(0);
    if (!resolveTaskbarSymbols()) { Wh_Log(L"taskbar.dll symbols could not be resolved"); return FALSE; }
    stage(WS_STAGE_SYMBOLS);
    if (!bridgeCreate()) return FALSE;
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) { bridgeClose(); return FALSE; }
    g_bridgeThread = CreateThread(nullptr, 0, bridgeThread, nullptr, 0, nullptr);
    if (!g_bridgeThread) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; bridgeClose(); return FALSE; }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Wavesill Behind Taskbar Content: uninit");
    g_unloading.store(true);
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_bridgeThread) { WaitForSingleObject(g_bridgeThread, INFINITE); CloseHandle(g_bridgeThread); g_bridgeThread = nullptr; }
    if (g_uiStarted.load()) {
        HWND taskbar = findTaskbarWnd();
        if (!taskbar || !runFromWindowThread(taskbar, uiStop, nullptr)) Wh_Log(L"The taskbar thread is gone; nothing to take out of the tree");
    }
    bridgeClose();
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
}
