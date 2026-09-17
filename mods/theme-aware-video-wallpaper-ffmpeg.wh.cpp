// ==WindhawkMod==
// @id              theme-aware-video-wallpaper-ffmpeg
// @name            A Video Wallpaper
// @name:zh-CN      视频壁纸
// @description     A complete video wallpaper for Windhawk. Follows Windows light/dark theme, plays folders or single files, smooth audio-video sync, configurable hotkeys, auto-pause in fullscreen.
// @description:zh-CN  完整的视频壁纸 Windhawk 模组。跟随 Windows 深浅色主题自动切换，支持单个视频或文件夹循环播放，音画同步流畅，可配置全局快捷键，全屏时自动暂停。
// @version         3.0
// @author          wakhh@qq.com
// @github          https://github.com/wakhh
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldxgi -ld2d1 -ld3d11 -ldcomp -ldwmapi -lgdi32 -luser32 -lole32 -lmmdevapi -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 视频壁纸 | A Video Wallpaper

一个功能齐全的 Windhawk 视频壁纸模组，支持本地视频文件或文件夹循环播放，自动跟随系统明暗主题切换，流畅的影音同步，全局快捷键，全屏自动暂停，多种硬件加速模式等。

A feature-rich Windhawk video wallpaper mod that plays local video files or folders, with automatic light/dark theme switching, smooth audio-video sync, global hotkeys, fullscreen auto-pause, multiple hardware acceleration modes, and more.

---

### 使用前必读 | Before You Start

模组依赖外部 **ffmpeg.exe**，请下载后放置在系统 PATH 或模组设置指定路径中：

This mod requires **ffmpeg.exe**. Download it and place it in your system PATH or the folder specified in mod settings:

👉 https://github.com/GyanD/codexffmpeg/releases/ （推荐 ffmpeg-release-full.7z | ffmpeg-release-full.7z recommended）

---

### 特色功能 | Features

- 🎨 **主题跟随** — 明/暗模式分别设置不同视频，系统切换时自动切换  |  Separate videos for light/dark mode; auto-switches with Windows theme
- 📁 **文件夹循环** — 支持单文件或整个文件夹，8 种排序方式自由选择  |  Single file or entire folder, 8 sort orders
- 🎞️ **流畅影音** — 低延迟音频播放，音画自动同步，没有卡顿没有漂移  |  Smooth audio playback, auto AV sync, no lag or drift
- 🚀 **硬件加速** — 自动识别最佳加速方案，也可手动指定  |  Auto-selects best option, or choose manually
- ⌨️ **全局快捷键** — 暂停/播放、上一项、下一项、静音切换，按住只触发一次  |  Pause/Play, Prev, Next, Mute. Press-and-hold fires once only
- 🖥️ **全屏自动暂停** — 游戏全屏或视频播放器最大化时自动暂停并禁用快捷键  |  Auto-pauses and disables hotkeys when other apps are fullscreen/maximized
- 📺 **多显示器** — 选择在哪个屏幕显示  |  Choose which monitor
- 🎚️ **缩放与填充** — Cover / Contain / Center / Stretch，多种填充色选项  |  Multiple scaling modes with configurable padding color

### 默认快捷键 | Default Hotkeys

| 功能 Action | 默认值 Default | 说明 Notes |
|---|---|---|
| 暂停 / 播放 Pause / Play | `Ctrl+Alt+F5` |  |  |
| 上一个视频 Previous | `Ctrl+Alt+←` | 到顶自动跳到末尾  |  Wraps to last at top |
| 下一个视频 Next | `Ctrl+Alt+→` | 到底自动跳到开头  |  Wraps to first at bottom |
| 静音切换 Mute | *(未设置)* | 在静音和设置音量之间切换  |  Toggles between muted and configured volume |

---

### 支持一下作者 | Support the Author

这个模组从设计到实现花了不少心思，反复调试音画同步、修复各种边界情况，才有了现在流畅稳定的体验。如果觉得好用，欢迎打赏一杯咖啡 ☕️，你的支持就是我继续维护的动力！

This mod took a lot of care — from audio-video sync tuning to edge-case bug fixing — to feel smooth and stable. If you find it useful, a coffee is always appreciated. Thanks for your support!

![赞助码](https://raw.githubusercontent.com/wakhh/PersonalLittleImageHosting/main/%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20260917043237_6_7.jpg)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ffmpegPath: ""
  $name: ffmpeg.exe path
  $name:zh-CN: ffmpeg.exe 路径
  $description: "Full path to ffmpeg.exe. Leave empty to auto-search from system PATH"
  $description:zh-CN: "ffmpeg.exe 的完整路径。留空则自动从系统 PATH 中查找"
- lightVideoPath: ""
  $name: Light mode video / folder path
  $name:zh-CN: 浅色模式视频 / 文件夹路径
  $description: "Full path to video file OR folder containing videos (folder plays by latest modified time)"
  $description:zh-CN: "视频文件完整路径，或包含视频文件的文件夹路径（文件夹按修改时间最新排序播放）"
- darkVideoPath: ""
  $name: Dark mode video / folder path
  $name:zh-CN: 深色模式视频 / 文件夹路径
  $description: "Leave empty to reuse light mode"
  $description:zh-CN: "留空则复用浅色模式设置"
- sortMode: "0"
  $name: Folder playback sort order
  $name:zh-CN: 文件夹播放排序方式
  $description: "Sort order when playing videos from a folder"
  $description:zh-CN: "文件夹模式下视频文件的排序方式"
  $options:
    - "0": "Modified time (newest first)"
    - "1": "Modified time (oldest first)"
    - "2": "Name (A to Z)"
    - "3": "Name (Z to A)"
    - "4": "Created time (oldest first)"
    - "5": "Created time (newest first)"
    - "6": "Size (largest first)"
    - "7": "Size (smallest first)"
  $options:zh-CN:
    - "0": "修改时间（新到旧）"
    - "1": "修改时间（旧到新）"
    - "2": "文件名（A 到 Z）"
    - "3": "文件名（Z 到 A）"
    - "4": "创建时间（旧到新）"
    - "5": "创建时间（新到旧）"
    - "6": "文件大小（大到小）"
    - "7": "文件大小（小到大）"
- enableAudio: "0"
  $name: Play audio
  $name:zh-CN: 播放音频
  $description: "Play the video's audio track. Disable to mute"
  $description:zh-CN: "播放视频文件的声音。关闭则静音"
  $options:
    - "0": "Disabled (mute)"
    - "1": "Enabled"
  $options:zh-CN:
    - "0": "关闭（静音）"
    - "1": "开启"
- audioVolume: 100
  $name: Volume (%)
  $name:zh-CN: 音量（%）
  $description: "Volume of video playback. "
  $description:zh-CN: "视频文件内部音频播放音量。"
- pauseOnFullscreen: "1"
  $name: Pause when app is fullscreen
  $name:zh-CN: 有应用全屏时暂停
  $description: "Pause video playback and disable hotkeys when another window is maximized or fullscreen (e.g., playing games, watching videos)"
  $description:zh-CN: "当其他窗口最大化或全屏时（如玩游戏、看视频）暂停视频壁纸播放且禁用快捷键"
  $options:
    - "0": "Disabled"
    - "1": "Enabled"
  $options:zh-CN:
    - "0": "关闭"
    - "1": "开启"
- hotkeyPause: "Ctrl+Alt+F5"
  $name: Hotkey - Pause/Play
  $name:zh-CN: 快捷键 - 暂停/播放
  $description: "Toggle pause/play. Auto disable when Empty to disable"
  $description:zh-CN: "切换视频壁纸的暂停/播放状态。留空禁用"
- hotkeyPrev: "Ctrl+Alt+Left"
  $name: Hotkey - Previous video
  $name:zh-CN: 快捷键 - 上一个视频
  $description: "Switch to previous video (wraps to end at top). Empty to disable"
  $description:zh-CN: "切换到上一个视频文件（到顶去底部）。仅文件夹内有多个视频文件。留空禁用"
- hotkeyNext: "Ctrl+Alt+Right"
  $name: Hotkey - Next video
  $name:zh-CN: 快捷键 - 下一个视频
  $description: "Switch to next video (wraps to top at end). Empty to disable"
  $description:zh-CN: "切换到下一个视频文件（到底去顶部）。仅文件夹内有多个视频文件。留空禁用"
- hotkeyMute: ""
  $name: Hotkey - Mute toggle
  $name:zh-CN: 快捷键 - 静音切换
  $description: "Toggle mute. Empty to disable"
  $description:zh-CN: "切换静音。留空禁用"
- applyMode: "on_next"
  $name: Theme/file/settings/audio change apply timing
  $name:zh-CN: 主题/文件/声音/设置变化生效时机
  $description: "When theme switches, files change, hotkey switches video while muted (audio reload), or settings are saved — may require"
  $description:zh-CN: "当主题切换、文件变化、静音时载入的视频快捷键切换声音或保存修改一些设置时，可能要"
  $options:
    - "on_next": "Wait until current video finishes, then apply"
    - "instant": "Instant (stop current video, apply immediately)"
  $options:zh-CN:
    - "on_next": "等当前视频播完后生效"
    - "instant": "立即停止当前视频并生效"
- fps: 15
  $name: Frame rate (fps)
  $name:zh-CN: 帧率（fps）
  $description: "Output frame rate. Range 10-60. Lower values save CPU. Videos with a lower native fps will have frames duplicated"
  $description:zh-CN: "输出帧率。范围 10-60。值越低越省 CPU。视频原始帧率低于设置值时会重复帧"
- opacity: 100
  $name: Opacity (%)
  $name:zh-CN: 不透明度（%）
  $description: "Overall transparency of the video wallpaper. 100 = fully opaque, lower values let the desktop show through."
  $description:zh-CN: "视频壁纸的整体不透明度。100 = 完全不透明，值越低桌面越明显。"
- scalingMode: "0"
  $name: Scaling Mode
  $name:zh-CN: 缩放模式
  $description: "How the video is scaled to fit the screen"
  $description:zh-CN: "视频壁纸如何缩放以适应屏幕"
  $options:
    - "0": "Cover (fill screen, keep ratio, crop edges)"
    - "1": "Contain (fit screen, keep ratio, pad bars)"
    - "2": "Center (original size, centered, pad bars)"
    - "3": "Stretch (fill screen, may distort)"
  $options:zh-CN:
    - "0": "填充（保持比例，裁剪边缘）"
    - "1": "适应（保持比例，可能有空置边缘）"
    - "2": "居中（原尺寸居中，可能有空置边缘）"
    - "3": "拉伸（填满屏幕，可能变形）"
- padColorMode: "black"
  $name: Pad color (for Contain/Center modes)
  $name:zh-CN: 空置边缘颜色（适应/居中模式）
  $description: "Color of the padding area around the video"
  $description:zh-CN: "周围空置边缘的颜色"
  $options:
    - "black": "Black"
    - "white": "White"
    - "dynamic": "Dynamic (white in light mode, black in dark mode)"
    - "transparent": "Transparent (shows desktop wallpaper underneath)"
    - "custom": "Custom color (set below)"
  $options:zh-CN:
    - "black": "黑色"
    - "white": "白色"
    - "dynamic": "动态（浅色模式白色，深色模式黑色）"
    - "transparent": "透明（露出底下的桌面壁纸）"
    - "custom": "自定义颜色（在下方设置）"
- padColorCustom: "#000000"
  $name: Custom pad color (hex)
  $name:zh-CN: 自定义边缘颜色（十六进制）
  $description: "Custom color for padding when Pad color is set to Custom. 6-digit RGB or 8-digit ARGB. Examples: #FF0000 for red, #80FF0000 for semi-transparent red, #00000000 for fully transparent"
  $description:zh-CN: "空置边缘颜色设为自定义时使用。6 位 RGB 或 8 位 ARGB。例如 #FF0000 为红色，#80FF0000 为半透明红，#00000000 为全透明"
- monitor: 1
  $name: Monitor
  $name:zh-CN: 显示器
  $description: "The monitor number to display wallpaper on (1-based). Use Microsoft PowerToys to reorder monitors if needed"
  $description:zh-CN: "显示壁纸的显示器编号（从 1 开始）。如需重排序号请使用 Microsoft PowerToys"
- hwaccelMode: "1"
  $name: Hardware acceleration
  $name:zh-CN: 硬件加速
  $description: "GPU-accelerated video decoding. Auto (default) lets FFmpeg choose the best option for your system. Try others if auto fails or you prefer a specific method"
  $description:zh-CN: "GPU 加速视频解码。自动（默认）让 FFmpeg 选择最合适的方案。如果自动失败或想用特定方案可以手动切换"
  $options:
    - "0": "Disabled (software decode)"
    - "1": "Auto (let FFmpeg choose)"
    - "2": "D3D12VA (Microsoft, fastest, Win10+, all GPUs, +AV1)"
    - "3": "D3D11VA (Microsoft, compatible, Win8+, all GPUs)"
    - "4": "DXVA2 (Microsoft, legacy fallback, Vista+)"
    - "5": "QSV (Intel Quick Sync, Intel GPUs only)"
    - "6": "CUDA/NVDEC (NVIDIA GPUs only)"
    - "7": "AMF (AMD GPUs only)"
  $options:zh-CN:
    - "0": "关闭（纯 CPU 软解）"
    - "1": "自动（让 FFmpeg 选）"
    - "2": "D3D12VA（微软通用，最快，Win10+，全平台，+AV1）"
    - "3": "D3D11VA（微软通用，兼容性好，Win8+，全平台）"
    - "4": "DXVA2（微软老方案后备，Vista+）"
    - "5": "QSV（Intel Quick Sync，仅 Intel 显卡）"
    - "6": "CUDA/NVDEC（仅 NVIDIA 显卡）"
    - "7": "AMF（仅 AMD 显卡）"
*/
// ==/WindhawkModSettings==

#include <Windows.h>
#include <windhawk_utils.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_3.h>
#include <wrl/client.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <atomic>
#include <algorithm>
#include <cstdint>

static IAudioClient* g_audioClient = nullptr;
static IAudioRenderClient* g_audioRenderClient = nullptr;
static ISimpleAudioVolume* g_simpleAudioVol = nullptr;
static HANDLE g_audioRenderEvent = NULL;
static HANDLE g_audioThread = NULL;
static std::atomic<bool> g_audioRunning{false};

static HANDLE g_ffmpegProc = NULL;
static HANDLE g_ffmpegWaitReg = NULL;
static HANDLE g_pipeRead = NULL;
static HANDLE g_pipeWrite = NULL;
static HANDLE g_audioPipeRead = NULL;
static UINT32 g_audioBlockAlign = 0;
static HWND g_videoHwnd = NULL;
static int g_frameSize = 0;

constexpr int VIDEO_QUEUE_SIZE = 8;
static std::vector<BYTE> g_frameQueue[VIDEO_QUEUE_SIZE];
static std::atomic<int> g_queueWriteIdx{0};
static std::atomic<int> g_queueReadIdx{0};

static HANDLE g_videoReadyEvent = NULL;

static std::vector<BYTE> g_audioBuf;

std::atomic<uint32_t> g_videoFramesRead{0};
std::atomic<uint32_t> g_videoFramesDisplayed{0};
std::atomic<uint64_t> g_audioSamplesPlayed{0};
WCHAR g_ffmpegPath[MAX_PATH] = {0};
WCHAR g_lightPath[MAX_PATH] = {0};
WCHAR g_darkPath[MAX_PATH] = {0};
int g_opacity = 255;
int g_fps = 15;
int g_scalingMode = 0;
WCHAR g_padColorMode[32] = {L"black"};
WCHAR g_padColorCustom[32] = {0};
WCHAR g_applyMode[16] = {L"instant"};
int g_sortMode = 0;
int g_hwaccelMode = 0;
int g_enableAudio = 0;
int g_audioVolume = 100;
int g_monitor = 1;
bool g_needWindowRecreate = false;
volatile bool g_pendingReload = false;
int g_pauseOnFullscreen = 1;

WCHAR g_hotkeyPauseStr[128] = {L"Ctrl+Alt+F5"};
WCHAR g_hotkeyPrevStr[128] = {L"Ctrl+Alt+Left"};
WCHAR g_hotkeyNextStr[128] = {L"Ctrl+Alt+Right"};
WCHAR g_hotkeyMuteStr[128] = {0};

static constexpr int HK_ID_PAUSE = 1;
static constexpr int HK_ID_PREV = 2;
static constexpr int HK_ID_NEXT = 3;
static constexpr int HK_ID_MUTE = 4;

int g_hotkeyPauseState = -1;
int g_hotkeyMuteState = -1;
bool g_hotkeysRegistered = false;

HANDLE g_pipeThread = NULL;
HANDLE g_dirChangeHandle = INVALID_HANDLE_VALUE;
bool g_classRegistered = false;
std::atomic<bool> g_running{true};
std::atomic<bool> g_isPaused{false};
std::atomic<bool> g_videoSwitching{false};
bool g_lastIsDark = false;
HANDLE g_mutex = NULL;
std::vector<std::wstring> g_videoList;
size_t g_videoIndex = 0;
int g_consecutiveErrors = 0;
static const int MAX_CONSECUTIVE_ERRORS = 3;

HWND g_workerW = NULL;
HKEY g_hThemeKey = NULL;
HANDLE g_hThemeEvt = NULL;
bool g_lastDarkChecked = false;
bool g_prevFull = false;

UINT_PTR g_createVideoTimer = 0;
constexpr UINT WM_APP_CLEANUP = WM_APP + 1;
constexpr UINT WM_APP_FFMPEG_EXIT = WM_APP + 2;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

[[clang::no_destroy]] Microsoft::WRL::ComPtr<ID3D11Device> g_d3dDevice;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDXGIDevice> g_dxgiDevice;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDXGIFactory2> g_dxgiFactory;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<ID2D1Factory1> g_d2dFactory;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<ID2D1Device> g_d2dDevice;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDXGISwapChain1> g_swapChain;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<ID2D1DeviceContext> g_dc;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDCompositionDevice> g_compositionDevice;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDCompositionTarget> g_compositionTarget;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<IDCompositionVisual> g_compositionVisual;
[[clang::no_destroy]] Microsoft::WRL::ComPtr<ID2D1Bitmap> g_frameBitmap;

bool g_dxInitSucceeded = false;
bool g_dxInitialized = false;

constexpr UINT TIMER_ID_MANAGER = 1;
constexpr UINT TIMER_ID_RENDERER = 2;

bool PlayNext();
bool PlayPrev();
void ReloadWallpaper();
void RenderFrame();
void StartRendererTimer();
void StopRendererTimer();
void StartManagerTimer();
void StopManagerTimer();
void ManagerTick();
void UpdateSessionVolume();
bool CreateVideoWindow();
void Wh_ModSettingsChanged();
HWND GetWorkerW();

HMONITOR GetMonitorById(int monitorId) {
    HMONITOR result = nullptr;
    int idx = 0;
    auto proc = [&](HMONITOR hMonitor) -> BOOL {
        if (idx == monitorId) { result = hMonitor; return FALSE; }
        idx++; return TRUE;
    };
    EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData) -> BOOL {
        auto& p = *reinterpret_cast<decltype(proc)*>(dwData);
        return p(hMonitor);
    }, reinterpret_cast<LPARAM>(&proc));
    return result;
}

bool GetTargetMonitorRect(int& x, int& y, int& w, int& h) {
    HMONITOR mon = GetMonitorById(g_monitor - 1);
    if (!mon) mon = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    if (!mon) return false;
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) return false;
    int vsx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vsy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    x = mi.rcMonitor.left - vsx;
    y = mi.rcMonitor.top - vsy;
    w = mi.rcMonitor.right - mi.rcMonitor.left;
    h = mi.rcMonitor.bottom - mi.rcMonitor.top;
    return true;
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&GetCurrentModuleHandle, &module);
    return module;
}

bool IsFolderViewWnd(HWND hWnd) {
    WCHAR buffer[64];
    if (!GetClassName(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SysListView32")) return false;
    if (!GetWindowText(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"FolderView")) return false;
    HWND hParent = GetAncestor(hWnd, GA_PARENT);
    if (!hParent) return false;
    if (!GetClassName(hParent, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SHELLDLL_DefView")) return false;
    if (GetWindowTextLength(hParent) > 0) return false;
    HWND hParent2 = GetAncestor(hParent, GA_PARENT);
    if (!hParent2) return false;
    if ((!GetClassName(hParent2, buffer, ARRAYSIZE(buffer)) ||
         _wcsicmp(buffer, L"Progman")) && hParent2 != GetShellWindow()) return false;
    return true;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT registeredMsg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_VidWallpaper");
    struct PARAM { RunFromWindowThreadProc_t proc; void* param; };
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) { proc(procParam); return true; }
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == *(UINT*)&registeredMsg) {
                    PARAM* p = (PARAM*)cwp->lParam;
                    p->proc(p->param);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }, nullptr, tid);
    if (!hook) return false;
    PARAM param = { proc, procParam };
    SendMessage(hWnd, registeredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !IsFolderViewWnd(hWnd)) return hWnd;
    if (!g_createVideoTimer) {
        g_createVideoTimer = SetTimer(nullptr, 0, 1000,
            [](HWND, UINT, UINT_PTR, DWORD) {
                KillTimer(nullptr, g_createVideoTimer);
                g_createVideoTimer = 0;
                if (!CreateVideoWindow()) {
                    Wh_Log(L"CreateVideoWindow failed in hook timer");
                    return;
                }
                Wh_ModSettingsChanged();
            });
    }
    return hWnd;
}

HWND GetProgmanWnd()
{
    HWND hProgman = FindWindowW(L"Progman", nullptr);
    if (!hProgman) return nullptr;
    DWORD progmanProcessId = 0;
    GetWindowThreadProcessId(hProgman, &progmanProcessId);
    if (progmanProcessId != GetCurrentProcessId()) return nullptr;
    return hProgman;
}

HWND GetWorkerW()
{
    HWND hProgman = GetProgmanWnd();
    if (!hProgman) return nullptr;

    SendMessage(hProgman, 0x052C, 0xD, 0);
    SendMessage(hProgman, 0x052C, 0xD, 1);

    HWND hWorkerW = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            if (!FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) return TRUE;
            HWND hWorker = FindWindowExW(nullptr, hWnd, L"WorkerW", nullptr);
            if (hWorker) { *(HWND*)lParam = hWorker; return FALSE; }
            return TRUE;
        },
        (LPARAM)&hWorkerW);

    if (!hWorkerW) {
        SendMessage(hProgman, 0x052C, 0, 0);
        EnumWindows(
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                if (!FindWindowExW(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) return TRUE;
                HWND hWorker = FindWindowExW(nullptr, hWnd, L"WorkerW", nullptr);
                if (hWorker) { *(HWND*)lParam = hWorker; return FALSE; }
                return TRUE;
            },
            (LPARAM)&hWorkerW);
    }

    if (!hWorkerW) hWorkerW = FindWindowExW(hProgman, nullptr, L"WorkerW", nullptr);
    if (!hWorkerW) hWorkerW = hProgman;
    return hWorkerW;
}

void RunDxgiWorkaroundForExplorerPatcher()
{
    auto dxgiModule = GetModuleHandleW(L"dxgi.dll");
    if (!dxgiModule) return;

    WCHAR dxgiPath[MAX_PATH];
    DWORD len = GetModuleFileNameW(dxgiModule, dxgiPath, MAX_PATH);
    if (len == 0 || len == MAX_PATH) return;

    WCHAR epDxgiPath[MAX_PATH];
    GetWindowsDirectoryW(epDxgiPath, MAX_PATH);
    wcscat_s(epDxgiPath, L"\\dxgi.dll");

    if (_wcsicmp(dxgiPath, epDxgiPath) != 0) return;

    auto dxgiDeclare = (HRESULT(WINAPI*)())GetProcAddress(dxgiModule, "DXGIDeclareAdapterRemovalSupport");
    if (dxgiDeclare) dxgiDeclare();
}

bool InitDirectX()
{
    HRESULT hr;

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                           nullptr, 0, D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice HARDWARE failed: 0x%08X, trying WARP", hr);
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags,
                               nullptr, 0, D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
        if (FAILED(hr)) { Wh_Log(L"D3D11CreateDevice WARP also failed: 0x%08X", hr); return false; }
        Wh_Log(L"D3D11CreateDevice: using WARP (software)");
    }

    hr = g_d3dDevice.As(&g_dxgiDevice);
    if (FAILED(hr)) { Wh_Log(L"QueryInterface IDXGIDevice failed: 0x%08X", hr); return false; }

    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&g_dxgiFactory));
    if (FAILED(hr)) { Wh_Log(L"CreateDXGIFactory2 failed: 0x%08X", hr); return false; }

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&g_d2dFactory));
    if (FAILED(hr)) { Wh_Log(L"D2D1CreateFactory failed: 0x%08X", hr); return false; }

    hr = g_d2dFactory->CreateDevice(g_dxgiDevice.Get(), &g_d2dDevice);
    if (FAILED(hr)) { Wh_Log(L"D2D CreateDevice failed: 0x%08X", hr); return false; }

    return true;
}

void UninitDirectX()
{
    g_frameBitmap.Reset();
    g_swapChain.Reset();
    g_dc.Reset();
    g_compositionVisual.Reset();
    g_compositionTarget.Reset();
    g_compositionDevice.Reset();
    g_d2dDevice.Reset();
    g_d2dFactory.Reset();
    g_dxgiFactory.Reset();
    g_dxgiDevice.Reset();
    g_d3dDevice.Reset();
    g_dxInitialized = false;
    g_dxInitSucceeded = false;
}

bool CreateSwapChainResources(UINT width, UINT height)
{
    HRESULT hr;

    DXGI_FORMAT targetFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    D2D1_ALPHA_MODE d2dAlphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;

    DXGI_SWAP_CHAIN_DESC1 scd = {};
    scd.Width = width;
    scd.Height = height;
    scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.Scaling = DXGI_SCALING_STRETCH;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

    hr = g_dxgiFactory->CreateSwapChainForComposition(g_dxgiDevice.Get(), &scd, nullptr, &g_swapChain);
    if (FAILED(hr)) { Wh_Log(L"CreateSwapChainForComposition failed: 0x%08X", hr); return false; }

    hr = g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_dc);
    if (FAILED(hr)) { Wh_Log(L"CreateDeviceContext failed: 0x%08X", hr); return false; }

    Microsoft::WRL::ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) { Wh_Log(L"GetBuffer failed: 0x%08X", hr); return false; }

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = d2dAlphaMode;
    bitmapProperties.pixelFormat.format = targetFormat;
    bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &targetBitmap);
    if (FAILED(hr)) { Wh_Log(L"CreateBitmapFromDxgiSurface failed: 0x%08X", hr); return false; }

    g_dc->SetTarget(targetBitmap.Get());

    hr = DCompositionCreateDevice(g_dxgiDevice.Get(), IID_PPV_ARGS(&g_compositionDevice));
    if (FAILED(hr)) { Wh_Log(L"DCompositionCreateDevice failed: 0x%08X", hr); return false; }

    hr = g_compositionDevice->CreateTargetForHwnd(g_videoHwnd, TRUE, &g_compositionTarget);
    if (FAILED(hr)) { Wh_Log(L"CreateTargetForHwnd failed: 0x%08X", hr); return false; }

    hr = g_compositionDevice->CreateVisual(&g_compositionVisual);
    if (FAILED(hr)) { Wh_Log(L"CreateVisual failed: 0x%08X", hr); return false; }

    hr = g_compositionVisual->SetContent(g_swapChain.Get());
    if (FAILED(hr)) { Wh_Log(L"SetContent failed: 0x%08X", hr); return false; }

    hr = g_compositionTarget->SetRoot(g_compositionVisual.Get());
    if (FAILED(hr)) { Wh_Log(L"SetRoot failed: 0x%08X", hr); return false; }

    hr = g_compositionDevice->Commit();
    if (FAILED(hr)) { Wh_Log(L"Commit failed: 0x%08X", hr); return false; }

    D2D1_BITMAP_PROPERTIES frameBitmapProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(targetFormat, d2dAlphaMode));
    hr = g_dc->CreateBitmap(D2D1::SizeU(width, height), nullptr, width * 4, frameBitmapProps, &g_frameBitmap);
    if (FAILED(hr)) { Wh_Log(L"CreateBitmap (frame) failed: 0x%08X", hr); return false; }

    return true;
}

void ReleaseSwapChainResources()
{
    g_frameBitmap.Reset();
    g_swapChain.Reset();
    g_dc.Reset();
    g_compositionVisual.Reset();
    g_compositionTarget.Reset();
    g_compositionDevice.Reset();
}

bool ResizeSwapChain(UINT width, UINT height)
{
    if (!g_swapChain || !g_dc) return false;

    g_dc->SetTarget(nullptr);

    HRESULT hr = g_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
        Wh_Log(L"ResizeBuffers failed: 0x%08X", hr);
        return false;
    }

    Microsoft::WRL::ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) {
        Wh_Log(L"GetBuffer failed: 0x%08X", hr);
        return false;
    }

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &targetBitmap);
    if (FAILED(hr)) {
        Wh_Log(L"CreateBitmapFromDxgiSurface failed: 0x%08X", hr);
        return false;
    }

    g_dc->SetTarget(targetBitmap.Get());

    g_frameSize = (int)(width * height * 4);
    for (int i = 0; i < VIDEO_QUEUE_SIZE; i++) g_frameQueue[i].clear();

    D2D1_BITMAP_PROPERTIES frameBitmapProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    g_frameBitmap.Reset();
    hr = g_dc->CreateBitmap(D2D1::SizeU(width, height), nullptr, width * 4, frameBitmapProps, &g_frameBitmap);
    if (FAILED(hr)) {
        Wh_Log(L"CreateBitmap (frame) failed: 0x%08X", hr);
        return false;
    }

    return true;
}

bool EnsureDxInitialized()
{
    if (g_dxInitialized) return g_dxInitSucceeded;
    g_dxInitialized = true;
    RunDxgiWorkaroundForExplorerPatcher();
    g_dxInitSucceeded = InitDirectX();
    return g_dxInitSucceeded;
}

bool IsVideoExt(const WCHAR* ext)
{
    return ext && (_wcsicmp(ext, L".mp4") == 0 || _wcsicmp(ext, L".mkv") == 0 ||
        _wcsicmp(ext, L".mov") == 0 || _wcsicmp(ext, L".webm") == 0 ||
        _wcsicmp(ext, L".avi") == 0 || _wcsicmp(ext, L".m4v") == 0 ||
        _wcsicmp(ext, L".wmv") == 0);
}

struct VideoEntry {
    std::wstring path;
    std::wstring name;
    FILETIME writeTime;
    FILETIME createTime;
    ULONGLONG size;
};

std::vector<std::wstring> EnumVideos(const WCHAR* folder)
{
    std::vector<VideoEntry> entries;
    WCHAR path[MAX_PATH];
    swprintf_s(path, L"%s\\*", folder);
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(path, &fd);
    if (hFind == INVALID_HANDLE_VALUE) return {};
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            WCHAR ext[_MAX_EXT];
            _wsplitpath_s(fd.cFileName, NULL, 0, NULL, 0, NULL, 0, ext, _MAX_EXT);
            if (IsVideoExt(ext)) {
                WCHAR full[MAX_PATH];
                swprintf_s(full, L"%s\\%s", folder, fd.cFileName);
                ULONGLONG size = ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
                entries.push_back({full, fd.cFileName, fd.ftLastWriteTime, fd.ftCreationTime, size});
            }
        }
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);

    std::sort(entries.begin(), entries.end(), [](const VideoEntry& a, const VideoEntry& b) {
        switch (g_sortMode) {
            case 1:
                if (a.writeTime.dwHighDateTime != b.writeTime.dwHighDateTime)
                    return a.writeTime.dwHighDateTime < b.writeTime.dwHighDateTime;
                return a.writeTime.dwLowDateTime < b.writeTime.dwLowDateTime;
            case 2: return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
            case 3: return _wcsicmp(a.name.c_str(), b.name.c_str()) > 0;
            case 4:
                if (a.createTime.dwHighDateTime != b.createTime.dwHighDateTime)
                    return a.createTime.dwHighDateTime < b.createTime.dwHighDateTime;
                return a.createTime.dwLowDateTime < b.createTime.dwLowDateTime;
            case 5:
                if (a.createTime.dwHighDateTime != b.createTime.dwHighDateTime)
                    return a.createTime.dwHighDateTime > b.createTime.dwHighDateTime;
                return a.createTime.dwLowDateTime > b.createTime.dwLowDateTime;
            case 6: return a.size > b.size;
            case 7: return a.size < b.size;
            case 0:
            default:
                if (a.writeTime.dwHighDateTime != b.writeTime.dwHighDateTime)
                    return a.writeTime.dwHighDateTime > b.writeTime.dwHighDateTime;
                return a.writeTime.dwLowDateTime > b.writeTime.dwLowDateTime;
        }
    });

    std::vector<std::wstring> result;
    result.reserve(entries.size());
    for (auto& e : entries) result.push_back(std::move(e.path));
    return result;
}

void CALLBACK OnFfmpegExit(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    if (TimerOrWaitFired) return;
    HANDLE proc = (HANDLE)lpParam;
    DWORD code = 0;
    GetExitCodeProcess(proc, &code);
    if (g_videoHwnd) PostMessage(g_videoHwnd, WM_APP_FFMPEG_EXIT, code, 0);
}

bool InitWasapi() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"InitWasapi: CoInitialize failed 0x%08X", hr);
        return false;
    }

    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;
    WAVEFORMATEX* pwfx = nullptr;
    WAVEFORMATEX fmt = {};

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enumerator);
    if (FAILED(hr)) { Wh_Log(L"InitWasapi: CoCreateInstance enumerator failed 0x%08X", hr); goto fail; }

    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    enumerator->Release();
    enumerator = nullptr;
    if (FAILED(hr)) { Wh_Log(L"InitWasapi: GetDefaultAudioEndpoint failed 0x%08X", hr); goto fail; }

    hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&g_audioClient);
    device->Release();
    device = nullptr;
    if (FAILED(hr)) { Wh_Log(L"InitWasapi: Activate IAudioClient failed 0x%08X", hr); goto fail; }

    hr = g_audioClient->GetMixFormat(&pwfx);
    if (FAILED(hr) || !pwfx) { Wh_Log(L"InitWasapi: GetMixFormat failed 0x%08X", hr); goto fail; }

    fmt.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    fmt.nChannels = 2;
    fmt.nSamplesPerSec = 48000;
    fmt.wBitsPerSample = 32;
    fmt.nBlockAlign = fmt.nChannels * (fmt.wBitsPerSample / 8);
    fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;
    fmt.cbSize = 0;

    Wh_Log(L"InitWasapi: using f32le ch=2 rate=48000 blockAlign=%d", fmt.nBlockAlign);
    g_audioBlockAlign = fmt.nBlockAlign;

    CoTaskMemFree(pwfx);
    pwfx = nullptr;

    hr = g_audioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
        200000, 0, &fmt, nullptr);
    if (FAILED(hr)) { Wh_Log(L"InitWasapi: Initialize failed 0x%08X", hr); goto fail; }

    hr = g_audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&g_audioRenderClient);
    if (FAILED(hr)) { Wh_Log(L"InitWasapi: GetService RenderClient failed 0x%08X", hr); goto fail; }

    hr = g_audioClient->GetService(__uuidof(ISimpleAudioVolume), (void**)&g_simpleAudioVol);
    if (FAILED(hr)) { Wh_Log(L"InitWasapi: GetService SimpleAudioVol failed 0x%08X", hr); goto fail; }

    g_audioRenderEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_audioRenderEvent) { Wh_Log(L"InitWasapi: CreateEvent failed"); goto fail; }

    hr = g_audioClient->SetEventHandle(g_audioRenderEvent);
    if (FAILED(hr)) { Wh_Log(L"InitWasapi: SetEventHandle failed 0x%08X", hr); goto fail; }

    hr = g_audioClient->Start();
    if (FAILED(hr)) { Wh_Log(L"InitWasapi: Start failed 0x%08X", hr); goto fail; }

    UpdateSessionVolume();
    Wh_Log(L"InitWasapi: OK");
    return true;

fail:
    if (g_audioClient) { g_audioClient->Release(); g_audioClient = nullptr; }
    if (g_audioRenderClient) { g_audioRenderClient->Release(); g_audioRenderClient = nullptr; }
    if (g_simpleAudioVol) { g_simpleAudioVol->Release(); g_simpleAudioVol = nullptr; }
    if (g_audioRenderEvent) { CloseHandle(g_audioRenderEvent); g_audioRenderEvent = NULL; }
    if (pwfx) CoTaskMemFree(pwfx);
    CoUninitialize();
    return false;
}

void ShutdownWasapi() {
    if (g_audioClient) {
        g_audioClient->Stop();
        g_audioClient->Release();
        g_audioClient = nullptr;
    }
    if (g_audioRenderClient) { g_audioRenderClient->Release(); g_audioRenderClient = nullptr; }
    if (g_simpleAudioVol) { g_simpleAudioVol->Release(); g_simpleAudioVol = nullptr; }
    if (g_audioRenderEvent) { CloseHandle(g_audioRenderEvent); g_audioRenderEvent = NULL; }
    CoUninitialize();
}

void UpdateSessionVolume() {
    if (!g_simpleAudioVol) return;
    float vol;
    if (g_hotkeyMuteState == 1) {
        vol = 0.0f;
    } else if (g_hotkeyMuteState == 0) {
        int v = g_audioVolume > 0 ? g_audioVolume : 100;
        vol = v / 100.0f;
    } else {
        if (!g_enableAudio) {
            vol = 0.0f;
        } else {
            vol = g_audioVolume / 100.0f;
        }
    }
    g_simpleAudioVol->SetMasterVolume(vol, NULL);
}

DWORD WINAPI AudioPlayThread(LPVOID) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool coInitOk = SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE;

    g_audioSamplesPlayed.store(0);
    g_audioRunning = true;
    Wh_Log(L"AudioPlayThread: starting");

    if (g_videoReadyEvent) {
        WaitForSingleObject(g_videoReadyEvent, 5000);
    }

    while (g_audioRunning && g_running) {
        if (!g_audioPipeRead) { Sleep(10); continue; }
        if (!g_audioClient || !g_audioRenderClient) { Sleep(10); continue; }
        if (g_isPaused) { Sleep(1); continue; }

        WaitForSingleObject(g_audioRenderEvent, 100);

        UINT32 padding = 0;
        HRESULT hr2 = g_audioClient->GetCurrentPadding(&padding);
        if (FAILED(hr2)) break;

        UINT32 bufferSize = 0;
        hr2 = g_audioClient->GetBufferSize(&bufferSize);
        if (FAILED(hr2)) break;

        UINT32 available = bufferSize - padding;
        if (available == 0) continue;

        BYTE* dest = nullptr;
        hr2 = g_audioRenderClient->GetBuffer(available, &dest);
        if (FAILED(hr2)) break;

        DWORD bytesToRead = available * g_audioBlockAlign;
        DWORD totalRead = 0;
        while (totalRead < bytesToRead && g_running) {
            DWORD got = 0;
            BOOL ok = ReadFile(g_audioPipeRead, dest + totalRead, bytesToRead - totalRead, &got, NULL);
            if (!ok || got == 0) break;
            totalRead += got;
        }

        UINT32 framesRead = (g_audioBlockAlign > 0) ? (totalRead / g_audioBlockAlign) : 0;
        if (framesRead < available) {
            memset(dest + totalRead, 0, (available - framesRead) * g_audioBlockAlign);
        }
        g_audioRenderClient->ReleaseBuffer(framesRead, 0);
        g_audioSamplesPlayed.fetch_add(framesRead);
    }

    Wh_Log(L"AudioPlayThread: exiting");
    g_audioRunning = false;
    if (coInitOk) CoUninitialize();
    return 0;
}

void StopFfmpeg()
{
    if (g_ffmpegWaitReg) { UnregisterWait(g_ffmpegWaitReg); g_ffmpegWaitReg = NULL; }

    g_audioRunning = false;
    if (g_audioThread) {
        if (g_audioPipeRead) { CloseHandle(g_audioPipeRead); g_audioPipeRead = NULL; }
        WaitForSingleObject(g_audioThread, 500);
        CloseHandle(g_audioThread);
        g_audioThread = NULL;
    }
    ShutdownWasapi();

    if (g_pipeThread) {
        if (g_pipeRead) { CloseHandle(g_pipeRead); g_pipeRead = NULL; }
        DWORD waitRet = WaitForSingleObject(g_pipeThread, 500);
        if (waitRet != WAIT_OBJECT_0) {
            Wh_Log(L"StopFfmpeg: pipe thread still alive after 500ms, terminating");
            TerminateThread(g_pipeThread, 0);
        }
        CloseHandle(g_pipeThread);
        g_pipeThread = NULL;
    }
    g_videoFramesRead.store(0);
    g_videoFramesDisplayed.store(0);
    if (g_pipeWrite) { CloseHandle(g_pipeWrite); g_pipeWrite = NULL; }
    if (g_pipeRead)  { CloseHandle(g_pipeRead);  g_pipeRead = NULL; }
    if (g_ffmpegProc){ TerminateProcess(g_ffmpegProc, 0); CloseHandle(g_ffmpegProc); g_ffmpegProc = NULL; }
    if (g_videoReadyEvent) { CloseHandle(g_videoReadyEvent); g_videoReadyEvent = NULL; }
}

bool IsDark()
{
    DWORD val = 1, cb = sizeof(val); HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&val, &cb);
        RegCloseKey(hKey);
    }
    return val == 0;
}

bool IsForegroundFull()
{
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    if (fg == GetDesktopWindow() || fg == GetShellWindow()) return false;
    DWORD style = (DWORD)GetWindowLongW(fg, GWL_STYLE);
    if (!(style & WS_MAXIMIZE)) {
        RECT rc; GetWindowRect(fg, &rc);
        int w = rc.right - rc.left, h = rc.bottom - rc.top;
        if (w < GetSystemMetrics(SM_CXSCREEN) - 200 || h < GetSystemMetrics(SM_CYSCREEN) - 200) return false;
    }
    return true;
}

bool IsHotkeyTargetReady()
{
    if (g_pauseOnFullscreen && IsForegroundFull()) return false;
    return true;
}

bool PathExists(const WCHAR* p) { return GetFileAttributesW(p) != INVALID_FILE_ATTRIBUTES; }

static std::unordered_map<std::wstring, UINT> g_hkVkMap;
static std::unordered_map<std::wstring, UINT> g_hkModMap;
static bool g_hkMapsInit = false;

void InitHotkeyMaps() {
    if (g_hkMapsInit) return;
    g_hkModMap = {
        {L"ctrl", MOD_CONTROL}, {L"control", MOD_CONTROL},
        {L"alt", MOD_ALT}, {L"shift", MOD_SHIFT}, {L"win", MOD_WIN},
    };
    for (int i = 0; i < 26; i++) g_hkVkMap[std::wstring(1, L'a' + i)] = 0x41 + i;
    for (int i = 0; i <= 9; i++) g_hkVkMap[std::wstring(1, L'0' + i)] = 0x30 + i;
    for (int i = 1; i <= 12; i++) { WCHAR buf[8]; swprintf_s(buf, L"f%d", i); g_hkVkMap[buf] = 0x6F + i; }
    g_hkVkMap[L"space"] = 0x20; g_hkVkMap[L"spacebar"] = 0x20;
    g_hkVkMap[L"left"] = 0x25; g_hkVkMap[L"up"] = 0x26; g_hkVkMap[L"right"] = 0x27; g_hkVkMap[L"down"] = 0x28;
    g_hkVkMap[L"enter"] = 0x0D; g_hkVkMap[L"return"] = 0x0D;
    g_hkVkMap[L"esc"] = 0x1B; g_hkVkMap[L"escape"] = 0x1B;
    g_hkVkMap[L"tab"] = 0x09; g_hkVkMap[L"backspace"] = 0x08;
    g_hkVkMap[L"home"] = 0x24; g_hkVkMap[L"end"] = 0x23;
    g_hkVkMap[L"pageup"] = 0x21; g_hkVkMap[L"pagedown"] = 0x22;
    g_hkVkMap[L"insert"] = 0x2D; g_hkVkMap[L"delete"] = 0x2E;
    g_hkVkMap[L"prtsc"] = 0x2C; g_hkVkMap[L"printscreen"] = 0x2C;
    g_hkMapsInit = true;
}

bool ParseHotkey(const WCHAR* str, UINT& mods, UINT& vk) {
    InitHotkeyMaps();
    mods = 0; vk = 0;
    if (!str || !*str) return false;
    std::wstring s = str;
    std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c) { return towlower(c); });
    std::wstring cur;
    std::vector<std::wstring> parts;
    for (wchar_t c : s) {
        if (c == L'+') { if (!cur.empty()) parts.push_back(cur); cur.clear(); }
        else cur += c;
    }
    if (!cur.empty()) parts.push_back(cur);
    if (parts.empty()) return false;
    for (size_t i = 0; i < parts.size() - 1; i++) {
        auto it = g_hkModMap.find(parts[i]);
        if (it == g_hkModMap.end()) return false;
        mods |= it->second;
    }
    auto it = g_hkVkMap.find(parts.back());
    if (it == g_hkVkMap.end()) return false;
    vk = it->second;
    return true;
}

static bool g_hkRegistered[5] = {false};

void UnregisterAllHotkeys(HWND hwnd) {
    if (!hwnd) return;
    for (int id = HK_ID_PAUSE; id <= HK_ID_MUTE; id++) {
        if (g_hkRegistered[id]) { UnregisterHotKey(hwnd, id); g_hkRegistered[id] = false; }
    }
    g_hotkeysRegistered = false;
}

void RegisterAllHotkeys(HWND hwnd) {
    if (!hwnd) return;
    UnregisterAllHotkeys(hwnd);
    bool multi = g_videoList.size() > 1;
    struct { const WCHAR* str; int id; bool need; } entries[] = {
        {g_hotkeyPauseStr, HK_ID_PAUSE, true},
        {g_hotkeyPrevStr,  HK_ID_PREV,  multi},
        {g_hotkeyNextStr,  HK_ID_NEXT,  multi},
        {g_hotkeyMuteStr,  HK_ID_MUTE,  true},
    };
    bool any = false;
    for (auto& e : entries) {
        if (!e.need) continue;
        if (!e.str || !*e.str) continue;
        UINT mods = 0, vk = 0;
        if (ParseHotkey(e.str, mods, vk)) {
            if (RegisterHotKey(hwnd, e.id, mods | MOD_NOREPEAT, vk)) {
                g_hkRegistered[e.id] = true;
                any = true;
            }
        }
    }
    g_hotkeysRegistered = any;
}


LRESULT CALLBACK VideoWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_TIMER:
            if (wParam == TIMER_ID_MANAGER) { ManagerTick(); return 0; }
            if (wParam == TIMER_ID_RENDERER) { if (g_opacity > 0) RenderFrame(); return 0; }
            break;
        case WM_HOTKEY: {
            static DWORD s_lastHotkeyTime[5] = {0};
            int id = (int)wParam;
            if (id < HK_ID_PAUSE || id > HK_ID_MUTE) return 0;
            DWORD now = GetTickCount();
            if (now - s_lastHotkeyTime[id] < 250) return 0;
            s_lastHotkeyTime[id] = now;
            if (id == HK_ID_PAUSE) {
                g_hotkeyPauseState = (g_hotkeyPauseState == 1) ? 0 : 1;
                Wh_Log(L"Hotkey pause toggle: state=%d", g_hotkeyPauseState);
            } else if (id == HK_ID_PREV) {
                if (g_videoSwitching.load()) return 0;
                g_hotkeyPauseState = -1;
                PlayPrev();
            } else if (id == HK_ID_NEXT) {
                if (g_videoSwitching.load()) return 0;
                g_hotkeyPauseState = -1;
                PlayNext();
            } else if (id == HK_ID_MUTE) {
                g_hotkeyMuteState = (g_hotkeyMuteState == 1) ? 0 : 1;
                Wh_Log(L"Hotkey mute toggle: state=%d", g_hotkeyMuteState);
                if (!g_enableAudio && g_hotkeyMuteState == 0) {
                    bool isOnNext = (wcscmp(g_applyMode, L"on_next") == 0);
                    if (!isOnNext) {
                        ReloadWallpaper();
                    }
                } else {
                    UpdateSessionVolume();
                }
            }
            return 0;
        }
        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* wp = (const WINDOWPOS*)lParam;
            if (!(wp->flags & SWP_NOSIZE) && g_d3dDevice) {
                if (ResizeSwapChain((UINT)wp->cx, (UINT)wp->cy)) {
                    if (g_opacity > 0) RenderFrame();
                }
            }
            break;
        }
        case WM_DISPLAYCHANGE: {
            Wh_Log(L"WM_DISPLAYCHANGE: resizing to monitor %d", g_monitor);
            int nx = 0, ny = 0, nw = 0, nh = 0;
            if (GetTargetMonitorRect(nx, ny, nw, nh) && nw > 0 && nh > 0) {
                SetWindowPos(hwnd, HWND_BOTTOM, nx, ny, nw, nh, SWP_NOACTIVATE);
            }
            break;
        }
        case WM_APP_CLEANUP:
            DestroyWindow(hwnd);
            return 0;
        case WM_APP_FFMPEG_EXIT: {
            if (g_videoSwitching.load()) return 0;
            DWORD code = (DWORD)wParam;
            if (code == 0) {
                g_consecutiveErrors = 0;
                Wh_Log(L"WM_APP_FFMPEG_EXIT: normal, moving to next");
            } else {
                g_consecutiveErrors++;
                Wh_Log(L"WM_APP_FFMPEG_EXIT: error=%lu consecutive=%d", code, g_consecutiveErrors);
            }
            if (g_consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
                Wh_Log(L"Too many consecutive errors (%d), stopping auto-play", g_consecutiveErrors);
                StopFfmpeg();
            } else {
                PlayNext();
            }
            return 0;
        }
        case WM_DESTROY:
            Wh_Log(L"VideoWndProc WM_DESTROY");
            StopManagerTimer();
            StopRendererTimer();
            UnregisterAllHotkeys(hwnd);
            g_running = false;
            ReleaseSwapChainResources();
            g_videoHwnd = NULL;
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool CreateVideoWindow()
{
    if (g_videoHwnd && IsWindow(g_videoHwnd)) return true;

    if (g_opacity <= 0) {
        Wh_Log(L"CreateVideoWindow: opacity=0, skipping window creation");
        return false;
    }

    if (!EnsureDxInitialized()) { Wh_Log(L"DX init failed"); return false; }

    g_workerW = GetWorkerW();
    if (!g_workerW) { Wh_Log(L"CreateVideoWindow: GetWorkerW failed"); return false; }

    int x = 0, y = 0, sw = 0, sh = 0;
    if (!GetTargetMonitorRect(x, y, sw, sh)) {
        Wh_Log(L"CreateVideoWindow: GetTargetMonitorRect failed, using primary");
        sw = GetSystemMetrics(SM_CXSCREEN);
        sh = GetSystemMetrics(SM_CYSCREEN);
    }

    if (!g_classRegistered) {
        WNDCLASSEXW wc = {sizeof(wc)};
        wc.lpfnWndProc = VideoWndProc;
        wc.hInstance = GetCurrentModuleHandle();
        wc.lpszClassName = L"VidWallpaperWnd";
        wc.hbrBackground = NULL;
        ATOM classAtom = RegisterClassExW(&wc);
        if (!classAtom) {
            DWORD err = GetLastError();
            if (err == ERROR_CLASS_ALREADY_EXISTS) {
                UnregisterClassW(L"VidWallpaperWnd", GetCurrentModuleHandle());
                classAtom = RegisterClassExW(&wc);
            }
        }
        if (!classAtom) { Wh_Log(L"RegisterClassExW failed"); return false; }
        g_classRegistered = true;
    }

    g_videoHwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_NOACTIVATE,
        L"VidWallpaperWnd", L"",
        WS_CHILD | WS_VISIBLE,
        x, y, sw, sh,
        g_workerW, NULL, GetCurrentModuleHandle(), NULL);

    if (!g_videoHwnd) { Wh_Log(L"CreateWindowExW failed err=%lu", GetLastError()); return false; }

    SetWindowPos(g_videoHwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    if (!CreateSwapChainResources(sw, sh)) {
        Wh_Log(L"CreateVideoWindow: CreateSwapChainResources failed");
        DestroyWindow(g_videoHwnd);
        g_videoHwnd = NULL;
        return false;
    }

    g_frameSize = sw * sh * 4;

    RegisterAllHotkeys(g_videoHwnd);

    return true;
}

DWORD WINAPI PipeReaderThread(LPVOID)
{
    int sw = 0, sh = 0;
    if (g_videoHwnd) {
        RECT rc;
        GetClientRect(g_videoHwnd, &rc);
        sw = rc.right - rc.left;
        sh = rc.bottom - rc.top;
    }
    if (sw <= 0) {
        sw = GetSystemMetrics(SM_CXSCREEN);
        sh = GetSystemMetrics(SM_CYSCREEN);
    }
    g_frameSize = sw * sh * 4;
    for (int i = 0; i < VIDEO_QUEUE_SIZE; i++) {
        if (g_frameQueue[i].size() != static_cast<size_t>(g_frameSize)) {
            g_frameQueue[i].resize(g_frameSize);
        }
    }

    g_queueWriteIdx.store(0);
    g_queueReadIdx.store(0);
    g_videoFramesRead.store(0);
    g_videoFramesDisplayed.store(0);

    while (g_running && g_pipeRead)
    {
        if (!g_frameSize) { Sleep(10); continue; }

        if (g_isPaused) { Sleep(1); continue; }

        int wIdx = g_queueWriteIdx.load();
        int nextW = (wIdx + 1) % VIDEO_QUEUE_SIZE;
        if (nextW == g_queueReadIdx.load()) { Sleep(1); continue; }

        std::vector<BYTE>& writeBuf = g_frameQueue[wIdx];

        DWORD totalRead = 0;
        while (totalRead < (DWORD)g_frameSize && g_running)
        {
            DWORD got = 0;
            BOOL okR = ReadFile(g_pipeRead, writeBuf.data() + totalRead, g_frameSize - totalRead, &got, NULL);
            if (!okR || got == 0) break;
            totalRead += got;
        }
        if (totalRead != (DWORD)g_frameSize) break;

        if (g_isPaused) continue;

        g_queueWriteIdx.store(nextW);
        uint32_t cnt = g_videoFramesRead.fetch_add(1) + 1;

        if (cnt == 1 && g_videoReadyEvent) {
            SetEvent(g_videoReadyEvent);
            g_videoSwitching.store(false);
        }
    }

    return 0;
}

void RenderFrame()
{
    if (g_opacity <= 0) return;
    if (g_isPaused) return;
    if (!g_running) return;
    if (!g_videoHwnd || !IsWindow(g_videoHwnd)) return;
    if (!g_dc || !g_frameBitmap || !g_swapChain) return;
    if (!g_frameSize) return;

    int rIdx = g_queueReadIdx.load();
    int wIdx = g_queueWriteIdx.load();
    if (rIdx == wIdx) return;

    uint32_t displayed = g_videoFramesDisplayed.load();
    uint32_t expectedFrame;
    if (g_enableAudio && g_audioClient) {
        uint64_t audioSamples = g_audioSamplesPlayed.load();
        expectedFrame = (uint32_t)(audioSamples * (uint64_t)g_fps / 48000);
        if (expectedFrame == 0) expectedFrame = 1;
    } else {
        expectedFrame = displayed + 1;
    }
    if (displayed + 1 > expectedFrame) return;

    int sw = 0, sh = 0;
    RECT rc;
    if (GetClientRect(g_videoHwnd, &rc)) {
        sw = rc.right - rc.left;
        sh = rc.bottom - rc.top;
    }
    if (sw <= 0 || sh <= 0) return;

    const std::vector<BYTE>& readBuf = g_frameQueue[rIdx];

    HRESULT hr;
    hr = g_frameBitmap->CopyFromMemory(nullptr, readBuf.data(), sw * 4);
    if (FAILED(hr)) {
        Wh_Log(L"RenderFrame: CopyFromMemory failed 0x%08X", hr);
        return;
    }

    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    if (g_opacity >= 255) {
        g_dc->DrawBitmap(
            g_frameBitmap.Get(),
            D2D1::RectF(0, 0, (FLOAT)sw, (FLOAT)sh),
            1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            D2D1::RectF(0, 0, (FLOAT)sw, (FLOAT)sh));
    } else {
        g_dc->DrawBitmap(
            g_frameBitmap.Get(),
            D2D1::RectF(0, 0, (FLOAT)sw, (FLOAT)sh),
            (FLOAT)g_opacity / 255.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            D2D1::RectF(0, 0, (FLOAT)sw, (FLOAT)sh));
    }
    hr = g_dc->EndDraw();
    if (FAILED(hr)) {
        Wh_Log(L"RenderFrame: EndDraw failed 0x%08X", hr);
        return;
    }

    if (!g_running) return;
    hr = g_swapChain->Present(0, 0);
    if (FAILED(hr)) {
        Wh_Log(L"RenderFrame: Present failed 0x%08X", hr);
        return;
    }

    g_queueReadIdx.store((rIdx + 1) % VIDEO_QUEUE_SIZE);
    g_videoFramesDisplayed.fetch_add(1);
}

bool StartFfmpeg(const WCHAR* videoPath)
{
    g_videoSwitching.store(true);
    StopFfmpeg();
    if (!g_ffmpegPath[0]) {
        Wh_Log(L"StartFfmpeg: ffmpegPath not set, searching PATH...");
        wcscpy_s(g_ffmpegPath, MAX_PATH, L"ffmpeg.exe");
    }
    if (!videoPath || !*videoPath || !PathExists(videoPath)) { g_videoSwitching.store(false); return false; }

    g_videoReadyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    int sw = 0, sh = 0;
    if (g_videoHwnd) {
        RECT rc;
        GetClientRect(g_videoHwnd, &rc);
        sw = rc.right - rc.left;
        sh = rc.bottom - rc.top;
    }
    if (sw <= 0) {
        sw = GetSystemMetrics(SM_CXSCREEN);
        sh = GetSystemMetrics(SM_CYSCREEN);
    }

    SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};

    if (g_enableAudio) {
        if (!InitWasapi()) {
            Wh_Log(L"StartFfmpeg: InitWasapi failed, falling back to no audio");
            g_enableAudio = false;
        }
    }

    HANDLE hRead = NULL, hWrite = NULL;
    if (!CreatePipe(&hRead, &hWrite, &sa, 8 * 1024 * 1024)) { Wh_Log(L"CreatePipe failed"); StopFfmpeg(); g_videoSwitching.store(false); return false; }
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

    HANDLE hErrRead = NULL, hErrWrite = NULL;
    if (!CreatePipe(&hErrRead, &hErrWrite, &sa, 65536)) { Wh_Log(L"CreatePipe err failed"); StopFfmpeg(); g_videoSwitching.store(false); return false; }
    SetHandleInformation(hErrRead, HANDLE_FLAG_INHERIT, 0);

    HANDLE hAudioRead = NULL, hAudioWrite = NULL;
    if (g_enableAudio) {
        if (!CreatePipe(&hAudioRead, &hAudioWrite, &sa, 2 * 1024 * 1024)) { Wh_Log(L"CreatePipe audio failed"); StopFfmpeg(); g_videoSwitching.store(false); return false; }
        SetHandleInformation(hAudioRead, HANDLE_FLAG_INHERIT, 0);
    }

    g_pipeRead = hRead; g_pipeWrite = hWrite;
    WCHAR cmd[MAX_PATH * 6];
    const WCHAR* vfArg = nullptr;

    WCHAR padColor[64] = {0};
    bool isPadTransparent = false;
    if (wcscmp(g_padColorMode, L"white") == 0) {
        wcscpy_s(padColor, L"color=white");
    } else if (wcscmp(g_padColorMode, L"black") == 0) {
        wcscpy_s(padColor, L"color=black");
    } else if (wcscmp(g_padColorMode, L"transparent") == 0) {
        wcscpy_s(padColor, L"color=0x00000000");
        isPadTransparent = true;
    } else if (wcscmp(g_padColorMode, L"dynamic") == 0) {
        wcscpy_s(padColor, g_lastIsDark ? L"color=black" : L"color=white");
    } else if (wcscmp(g_padColorMode, L"custom") == 0 && g_padColorCustom[0]) {
        const WCHAR* src = g_padColorCustom[0] == L'#' ? g_padColorCustom + 1 : g_padColorCustom;
        size_t len = wcslen(src);
        bool valid = (len == 6 || len == 8);
        if (valid) {
            for (size_t i = 0; i < len; i++) {
                WCHAR c = src[i];
                if (!((c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F'))) { valid = false; break; }
            }
        }
        WCHAR hex[16] = {0};
        if (!valid) {
            Wh_Log(L"padColorCustom: invalid value '%s', fallback to black", g_padColorCustom);
            wcscpy_s(hex, L"0xFF000000");
        } else if (len == 6) {
            swprintf_s(hex, L"0xFF%s", src);
        } else {
            swprintf_s(hex, L"0x%s", src);
            if (src[0] == L'0' && src[1] == L'0') isPadTransparent = true;
        }
        swprintf_s(padColor, L"color=%s", hex);
    } else {
        wcscpy_s(padColor, L"color=black");
    }

    switch (g_scalingMode) {
        case 0: vfArg = L" -vf \"scale=%d:%d:force_original_aspect_ratio=increase,crop=%d:%d,fps=%d\""; break;
        case 1: vfArg = L" -vf \"scale=%d:%d:force_original_aspect_ratio=decrease,pad=%d:%d:(ow-iw)/2:(oh-ih)/2:%s,fps=%d\""; break;
        case 3: vfArg = L" -vf \"fps=%d\""; break;
        default: vfArg = L" -vf \"fps=%d\""; break;
    }
    const WCHAR* hwaccelArg = L"";
    switch (g_hwaccelMode) {
        case 1: hwaccelArg = L" -hwaccel auto"; break;
        case 2: hwaccelArg = L" -hwaccel d3d12va"; break;
        case 3: hwaccelArg = L" -hwaccel d3d11va"; break;
        case 4: hwaccelArg = L" -hwaccel dxva2"; break;
        case 5: hwaccelArg = L" -hwaccel qsv"; break;
        case 6: hwaccelArg = L" -hwaccel cuda"; break;
        case 7: hwaccelArg = L" -hwaccel amf"; break;
        default: hwaccelArg = L""; break;
    }
    const WCHAR* pixFmt = L"bgra";

    WCHAR audioArg[256] = {0};
    bool needsAudio;
    if (g_hotkeyMuteState >= 0) {
        needsAudio = (g_hotkeyMuteState == 0);
    } else {
        needsAudio = (g_enableAudio != 0);
    }
    if (needsAudio) {
        swprintf_s(audioArg, L" -map 0:a? -af volume=1.00,aformat=sample_fmts=fltp:channel_layouts=stereo:sample_rates=48000 -f f32le -ac 2 -ar 48000 pipe:2");
    } else {
        wcscpy_s(audioArg, L" -an");
    }

    bool hasVf = (vfArg != nullptr) || g_scalingMode == 2;

    if (hasVf) {
        WCHAR vfBuf[512];
        if (g_scalingMode == 2) {
            const WCHAR* colorVal = padColor;
            if (wcsncmp(padColor, L"color=", 6) == 0) colorVal = padColor + 6;
            swprintf_s(vfBuf,
                L" -vf \"color=c=%s:s=%dx%d:r=%d,format=%s[bg];[0:v]format=%s[v];[bg][v]overlay=(W-w)/2:(H-h)/2:shortest=1,format=%s,fps=%d\"",
                colorVal, sw, sh, g_fps, pixFmt, pixFmt, pixFmt, g_fps);
        } else if (g_scalingMode == 0) {
            swprintf_s(vfBuf, vfArg, sw, sh, sw, sh, g_fps);
        } else if (g_scalingMode == 1) {
            swprintf_s(vfBuf, vfArg, sw, sh, sw, sh, padColor, g_fps);
        } else {
            swprintf_s(vfBuf, vfArg, g_fps);
        }
        swprintf_s(cmd, L"\"%s\" -nostdin -hide_banner -loglevel error%s -i \"%s\" -map 0:v -f rawvideo -pix_fmt %s%s -%s",
            g_ffmpegPath, hwaccelArg, videoPath, pixFmt, vfBuf, audioArg);
    } else {
        swprintf_s(cmd, L"\"%s\" -nostdin -hide_banner -loglevel error%s -i \"%s\" -map 0:v -f rawvideo -pix_fmt %s -s %dx%d -vf \"fps=%d\" -%s",
            g_ffmpegPath, hwaccelArg, videoPath, pixFmt, sw, sh, g_fps, audioArg);
    }

    DWORD creationFlags = CREATE_NO_WINDOW;
    STARTUPINFOEXW siEx = {};
    siEx.StartupInfo.cb = sizeof(STARTUPINFOEXW);
    siEx.StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    siEx.StartupInfo.wShowWindow = SW_HIDE;
    siEx.StartupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    siEx.StartupInfo.hStdOutput = hWrite;
    siEx.StartupInfo.hStdError = g_enableAudio ? hAudioWrite : hErrWrite;

    PROCESS_INFORMATION pi = {0};
    BOOL ok = CreateProcessW(NULL, cmd, NULL, NULL, TRUE, creationFlags, NULL, NULL,
        (LPSTARTUPINFOW)&siEx, &pi);
    if (!ok) {
        Wh_Log(L"CreateProcess failed err=%lu", GetLastError());
        StopFfmpeg();
        g_videoSwitching.store(false);
        return false;
    }
    CloseHandle(pi.hThread);
    CloseHandle(hWrite);
    CloseHandle(hErrWrite);
    if (hAudioWrite) CloseHandle(hAudioWrite);
    g_pipeWrite = NULL;
    g_audioPipeRead = g_enableAudio ? hAudioRead : NULL;
    g_ffmpegProc = pi.hProcess;

    {
        char errBuf[2048] = {0};
        DWORD waited = 0;
        bool gotData = false;
        while (waited < 5000 && g_running) {
            DWORD avail = 0;
            if (PeekNamedPipe(hRead, NULL, 0, NULL, &avail, NULL) && avail > 0) {
                gotData = true;
                break;
            }
            Sleep(50); waited += 50;
        }
        DWORD errAvail = 0;
        BOOL errPeekOk = PeekNamedPipe(hErrRead, NULL, 0, NULL, &errAvail, NULL);
        if (errPeekOk && errAvail > 0) {
            DWORD got = 0;
            if (ReadFile(hErrRead, errBuf, sizeof(errBuf) - 1, &got, NULL) && got > 0) {
                errBuf[got] = 0;
                Wh_Log(L"ffmpeg stderr: %S", errBuf);
            }
        }
        CloseHandle(hErrRead);
        if (!gotData) {
            Wh_Log(L"StartFfmpeg: no data from ffmpeg after %dms, aborting", waited);
            StopFfmpeg(); g_videoSwitching.store(false); return false;
        }
    }

    if (!g_videoHwnd || !IsWindow(g_videoHwnd)) {
        Wh_Log(L"StartFfmpeg: video window is not ready");
        StopFfmpeg();
        g_videoSwitching.store(false);
        return false;
    }

    if (g_enableAudio) {
        g_audioThread = CreateThread(NULL, 0, AudioPlayThread, NULL, 0, NULL);
    }

    g_pipeThread = CreateThread(NULL, 0, PipeReaderThread, NULL, 0, NULL);
    RegisterWaitForSingleObject(&g_ffmpegWaitReg, g_ffmpegProc, OnFfmpegExit, g_ffmpegProc, INFINITE, WT_EXECUTEONLYONCE);
    StartRendererTimer();
    return true;
}

void ReloadVideoList()
{
    if (g_dirChangeHandle != INVALID_HANDLE_VALUE) { FindCloseChangeNotification(g_dirChangeHandle); g_dirChangeHandle = INVALID_HANDLE_VALUE; }

    g_videoList.clear(); g_videoIndex = 0; g_consecutiveErrors = 0;
    bool dark = IsDark();
    g_lastIsDark = dark;

    WCHAR target[MAX_PATH] = {0};
    if (dark) { if (g_darkPath[0]) wcscpy_s(target, g_darkPath); else wcscpy_s(target, g_lightPath); }
    else wcscpy_s(target, g_lightPath);

    DWORD attr = GetFileAttributesW(target);
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        g_videoList = EnumVideos(target);
        g_dirChangeHandle = FindFirstChangeNotificationW(target, FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE);
    } else if (PathExists(target)) {
        g_videoList.emplace_back(target);
    }
}

void ReloadWallpaper()
{
    if (g_videoSwitching.load()) {
        Wh_Log(L"ReloadWallpaper: video switching in progress, skipping");
        return;
    }
    StopFfmpeg();
    if (g_opacity <= 0) {
        Wh_Log(L"ReloadWallpaper: opacity=0, skipping ffmpeg start");
        return;
    }
    if (g_needWindowRecreate && g_videoHwnd && IsWindow(g_videoHwnd)) {
        Wh_Log(L"ReloadWallpaper: recreating window for monitor change");
        DestroyWindow(g_videoHwnd);
        g_videoHwnd = NULL;
        g_needWindowRecreate = false;
    }
    if (!g_videoHwnd || !IsWindow(g_videoHwnd)) {
        if (!CreateVideoWindow()) {
            Wh_Log(L"ReloadWallpaper: CreateVideoWindow failed");
            return;
        }
    }
    ReloadVideoList();
    if (!g_videoList.empty()) {
        const WCHAR* first = g_videoList[g_videoIndex].c_str();
        bool ok = StartFfmpeg(first);
        if (ok) StartManagerTimer();
    }
}

bool PlayPrev()
{
    if (g_videoSwitching.load()) {
        Wh_Log(L"PlayPrev: video switching in progress, skipping");
        return false;
    }
    if (g_opacity <= 0) return false;

    if (g_pendingReload) {
        g_pendingReload = false;
        ReloadVideoList();
    }
    if (g_needWindowRecreate && g_videoHwnd && IsWindow(g_videoHwnd)) {
        Wh_Log(L"PlayPrev: recreating window for monitor change");
        DestroyWindow(g_videoHwnd);
        g_videoHwnd = NULL;
        g_needWindowRecreate = false;
    }
    if (!g_videoHwnd || !IsWindow(g_videoHwnd)) {
        if (!CreateVideoWindow()) {
            Wh_Log(L"PlayPrev: CreateVideoWindow failed");
            return false;
        }
    }
    if (g_videoList.empty()) return false;
    if (g_consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
        Wh_Log(L"PlayPrev: too many consecutive errors (%d)", g_consecutiveErrors);
        return false;
    }
    if (g_videoIndex == 0) {
        g_videoIndex = (int)g_videoList.size() - 1;
    } else {
        g_videoIndex--;
    }
    const WCHAR* prev = g_videoList[g_videoIndex].c_str();
    bool ok = StartFfmpeg(prev);
    if (!ok) {
        g_consecutiveErrors++;
    } else {
        StartManagerTimer();
    }
    return ok;
}

bool PlayNext()
{
    if (g_videoSwitching.load()) {
        Wh_Log(L"PlayNext: video switching in progress, skipping");
        return false;
    }
    if (g_opacity <= 0) return false;

    if (g_pendingReload) {
        g_pendingReload = false;
        ReloadVideoList();
    }
    if (g_needWindowRecreate && g_videoHwnd && IsWindow(g_videoHwnd)) {
        Wh_Log(L"PlayNext: recreating window for monitor change");
        DestroyWindow(g_videoHwnd);
        g_videoHwnd = NULL;
        g_needWindowRecreate = false;
    }
    if (!g_videoHwnd || !IsWindow(g_videoHwnd)) {
        if (!CreateVideoWindow()) {
            Wh_Log(L"PlayNext: CreateVideoWindow failed");
            return false;
        }
    }
    if (g_videoList.empty()) return false;
    if (g_consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
        Wh_Log(L"PlayNext: too many consecutive errors (%d), giving up. Reload to retry.", g_consecutiveErrors);
        return false;
    }
    g_videoIndex = (g_videoIndex + 1) % g_videoList.size();
    const WCHAR* next = g_videoList[g_videoIndex].c_str();
    bool ok = StartFfmpeg(next);
    if (!ok) {
        g_consecutiveErrors++;
    } else {
        StartManagerTimer();
    }
    return ok;
}

void Wh_ModSettingsChanged()
{
    PCWSTR s = Wh_GetStringSetting(L"ffmpegPath");
    WCHAR newFfmpegPath[MAX_PATH] = {0};
    if (s) { wcscpy_s(newFfmpegPath, MAX_PATH, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"lightVideoPath");
    WCHAR newLightPath[MAX_PATH] = {0};
    if (s) { wcscpy_s(newLightPath, MAX_PATH, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"darkVideoPath");
    WCHAR newDarkPath[MAX_PATH] = {0};
    if (s) { wcscpy_s(newDarkPath, MAX_PATH, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"sortMode");
    WCHAR sortStr[8] = {0};
    if (s) { wcscpy_s(sortStr, 8, s); Wh_FreeStringSetting(s); }
    int newSortMode = _wtoi(sortStr);
    if (newSortMode < 0 || newSortMode > 7) newSortMode = 0;

    int oldOpacity = g_opacity;
    int pct = Wh_GetIntSetting(L"opacity");
    int newOpacity = (pct >= 0 && pct <= 100) ? (pct * 255 / 100) : 255;
    bool oldWasZero = (oldOpacity <= 0);
    bool newIsZero = (newOpacity <= 0);

    int fps = Wh_GetIntSetting(L"fps");
    if (fps < 10) fps = 10;
    if (fps > 60) fps = 60;
    int oldFps = g_fps;
    g_fps = fps;

    s = Wh_GetStringSetting(L"scalingMode");
    WCHAR scalingStr[8] = {0};
    if (s) { wcscpy_s(scalingStr, 8, s); Wh_FreeStringSetting(s); }
    int scaling = _wtoi(scalingStr);
    if (scaling < 0 || scaling > 3) scaling = 0;
    int oldScaling = g_scalingMode;
    g_scalingMode = scaling;

    s = Wh_GetStringSetting(L"padColorMode");
    WCHAR newPadMode[32] = {0};
    if (s) { wcscpy_s(newPadMode, 32, s); Wh_FreeStringSetting(s); }
    if (!newPadMode[0]) wcscpy_s(newPadMode, 32, L"black");

    s = Wh_GetStringSetting(L"padColorCustom");
    WCHAR newPadCustom[32] = {0};
    if (s) { wcscpy_s(newPadCustom, 32, s); Wh_FreeStringSetting(s); }

    s = Wh_GetStringSetting(L"applyMode");
    WCHAR newApplyMode[16] = {0};
    if (s) { wcscpy_s(newApplyMode, 16, s); Wh_FreeStringSetting(s); }
    if (!newApplyMode[0]) wcscpy_s(newApplyMode, 16, L"instant");

    s = Wh_GetStringSetting(L"hwaccelMode");
    WCHAR hwaccelStr[4] = {0};
    if (s) { wcscpy_s(hwaccelStr, 4, s); Wh_FreeStringSetting(s); }
    int newHwaccel = _wtoi(hwaccelStr);
    if (newHwaccel < 0 || newHwaccel > 7) newHwaccel = 1;

    s = Wh_GetStringSetting(L"enableAudio");
    WCHAR eaStr[4] = {0};
    if (s) { wcscpy_s(eaStr, 4, s); Wh_FreeStringSetting(s); }
    int newEnableAudio = _wtoi(eaStr);
    if (newEnableAudio < 0 || newEnableAudio > 1) newEnableAudio = 0;

    int newAudioVolume = Wh_GetIntSetting(L"audioVolume");
    if (newAudioVolume < 0) newAudioVolume = 0;
    if (newAudioVolume > 100) newAudioVolume = 100;

    int newMonitor = Wh_GetIntSetting(L"monitor");
    if (newMonitor < 1) newMonitor = 1;

    s = Wh_GetStringSetting(L"pauseOnFullscreen");
    WCHAR pfsStr[4] = {0};
    if (s) { wcscpy_s(pfsStr, 4, s); Wh_FreeStringSetting(s); }
    int newPauseOnFullscreen = _wtoi(pfsStr);
    if (newPauseOnFullscreen < 0 || newPauseOnFullscreen > 1) newPauseOnFullscreen = 1;

    WCHAR newHotkeyPause[128] = {0};
    WCHAR newHotkeyPrev[128] = {0};
    WCHAR newHotkeyNext[128] = {0};
    WCHAR newHotkeyMute[128] = {0};
    s = Wh_GetStringSetting(L"hotkeyPause");
    if (s) { wcscpy_s(newHotkeyPause, 128, s); Wh_FreeStringSetting(s); }
    s = Wh_GetStringSetting(L"hotkeyPrev");
    if (s) { wcscpy_s(newHotkeyPrev, 128, s); Wh_FreeStringSetting(s); }
    s = Wh_GetStringSetting(L"hotkeyNext");
    if (s) { wcscpy_s(newHotkeyNext, 128, s); Wh_FreeStringSetting(s); }
    s = Wh_GetStringSetting(L"hotkeyMute");
    if (s) { wcscpy_s(newHotkeyMute, 128, s); Wh_FreeStringSetting(s); }

    g_opacity = newOpacity;
    g_pauseOnFullscreen = newPauseOnFullscreen;

    if (!oldWasZero && newIsZero) {
        Wh_Log(L"Opacity changed >0→0, stopping ffmpeg");
        StopFfmpeg();
        StopManagerTimer();
        StopRendererTimer();
        return;
    }

    bool needReload =
        wcscmp(newFfmpegPath, g_ffmpegPath) != 0 ||
        wcscmp(newLightPath, g_lightPath) != 0 ||
        wcscmp(newDarkPath, g_darkPath) != 0 ||
        newSortMode != g_sortMode ||
        newHwaccel != g_hwaccelMode ||
        newEnableAudio != g_enableAudio ||
        newAudioVolume != g_audioVolume ||
        newMonitor != g_monitor ||
        g_fps != oldFps ||
        g_scalingMode != oldScaling ||
        wcscmp(newPadMode, g_padColorMode) != 0 ||
        wcscmp(newPadCustom, g_padColorCustom) != 0;

    int oldMonitor = g_monitor;
    g_monitor = newMonitor;
    if (oldMonitor != newMonitor) {
        g_needWindowRecreate = true;
        Wh_Log(L"Monitor changed %d→%d, will recreate window", oldMonitor, newMonitor);
    }
    wcscpy_s(g_ffmpegPath, MAX_PATH, newFfmpegPath);
    wcscpy_s(g_lightPath, MAX_PATH, newLightPath);
    wcscpy_s(g_darkPath, MAX_PATH, newDarkPath);
    g_sortMode = newSortMode;
    g_hwaccelMode = newHwaccel;
    g_enableAudio = newEnableAudio;
    g_audioVolume = newAudioVolume;
    UpdateSessionVolume();
    wcscpy_s(g_padColorMode, 32, newPadMode);
    wcscpy_s(g_padColorCustom, 32, newPadCustom);
    wcscpy_s(g_applyMode, 16, newApplyMode);

    bool hotkeysChanged =
        wcscmp(newHotkeyPause, g_hotkeyPauseStr) != 0 ||
        wcscmp(newHotkeyPrev, g_hotkeyPrevStr) != 0 ||
        wcscmp(newHotkeyNext, g_hotkeyNextStr) != 0 ||
        wcscmp(newHotkeyMute, g_hotkeyMuteStr) != 0;
    if (hotkeysChanged) {
        wcscpy_s(g_hotkeyPauseStr, 128, newHotkeyPause);
        wcscpy_s(g_hotkeyPrevStr, 128, newHotkeyPrev);
        wcscpy_s(g_hotkeyNextStr, 128, newHotkeyNext);
        wcscpy_s(g_hotkeyMuteStr, 128, newHotkeyMute);
        if (g_videoHwnd) RegisterAllHotkeys(g_videoHwnd);
    }

    if (oldWasZero && !newIsZero) {
        Wh_Log(L"Opacity changed 0→%d%, creating window and starting", pct);
        if (!g_videoHwnd || !IsWindow(g_videoHwnd)) {
            if (!CreateVideoWindow()) {
                Wh_Log(L"Wh_ModSettingsChanged: CreateVideoWindow failed");
                return;
            }
        }
        ReloadWallpaper();
        return;
    }

    if (needReload) {
        if (wcscmp(newApplyMode, L"on_next") == 0 && g_ffmpegProc != NULL) {
            g_pendingReload = true;
        } else {
            ReloadWallpaper();
        }
    }
}

void StartManagerTimer() {
    if (!g_videoHwnd) return;
    SetTimer(g_videoHwnd, TIMER_ID_MANAGER, 200, nullptr);
}

void StopManagerTimer() {
    if (g_videoHwnd) KillTimer(g_videoHwnd, TIMER_ID_MANAGER);
}

void StartRendererTimer() {
    if (!g_videoHwnd) return;
    DWORD frameInterval = g_fps > 0 ? 1000 / (DWORD)g_fps : 66;
    DWORD interval = frameInterval / 2 > 1 ? frameInterval / 2 : 1;
    SetTimer(g_videoHwnd, TIMER_ID_RENDERER, interval, nullptr);
}

void StopRendererTimer() {
    if (g_videoHwnd) KillTimer(g_videoHwnd, TIMER_ID_RENDERER);
}

void ManagerTick() {
    if (!g_hThemeKey) {
        RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_NOTIFY, &g_hThemeKey);
        if (g_hThemeKey) g_hThemeEvt = CreateEventW(NULL, TRUE, FALSE, NULL);
        g_lastDarkChecked = IsDark();
    }

    if (g_videoHwnd && IsWindow(g_videoHwnd)) {
        bool ready = IsHotkeyTargetReady();
        bool multi = g_videoList.size() > 1;
        static bool s_lastMulti = false;
        if (ready) {
            if (!g_hotkeysRegistered || s_lastMulti != multi) {
                RegisterAllHotkeys(g_videoHwnd);
                s_lastMulti = multi;
            }
        } else if (g_hotkeysRegistered) {
            UnregisterAllHotkeys(g_videoHwnd);
        }
    }

    if (g_hThemeKey && g_hThemeEvt) {
        ResetEvent(g_hThemeEvt);
        RegNotifyChangeKeyValue(g_hThemeKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, g_hThemeEvt, TRUE);
        DWORD waitRet = WaitForSingleObject(g_hThemeEvt, 0);
        if (waitRet == WAIT_OBJECT_0) {
            bool isDark = IsDark();
            if (isDark != g_lastDarkChecked) {
                g_lastDarkChecked = isDark;
                g_lastIsDark = isDark;
                bool isOnNext = (wcscmp(g_applyMode, L"on_next") == 0);
                if (isOnNext && g_ffmpegProc) { g_pendingReload = true; }
                else { ReloadWallpaper(); return; }
            }
        }
    }

    if (g_dirChangeHandle != INVALID_HANDLE_VALUE) {
        DWORD waitRet = WaitForSingleObject(g_dirChangeHandle, 0);
        if (waitRet == WAIT_OBJECT_0) {
            bool isOnNext = (wcscmp(g_applyMode, L"on_next") == 0);
            if (isOnNext && g_ffmpegProc) { g_pendingReload = true; }
            else { ReloadWallpaper(); return; }
            FindNextChangeNotification(g_dirChangeHandle);
        }
    }

    if (g_hotkeyPauseState == 1) {
        g_isPaused = true;
    } else {
        bool shouldPause = false;
        if (g_pauseOnFullscreen) {
            shouldPause = IsForegroundFull();
        }
        if (shouldPause != g_isPaused) {
            g_isPaused = shouldPause;
        }
    }
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Wh_ModInit: entering");
    g_mutex = CreateMutexW(NULL, TRUE, L"Local\\VidWallpaperOnlyOne");
    if (g_mutex && GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(g_mutex); g_mutex = NULL; return FALSE; }
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    Wh_Log(L"Wh_ModInit: OK");
    return TRUE;
}

void Wh_ModAfterInit()
{
    Wh_Log(L"Wh_ModAfterInit: entering");
    HWND hWorkerW = GetWorkerW();
    if (hWorkerW) {
        RunFromWindowThread(hWorkerW, [](void*) {
            if (!CreateVideoWindow()) {
                Wh_Log(L"CreateVideoWindow failed in AfterInit");
                return;
            }
            Wh_ModSettingsChanged();
        }, nullptr);
    } else {
        Wh_Log(L"Wh_ModAfterInit: WorkerW not found, waiting for FolderView hook");
    }
}

void Wh_ModUninit()
{
    Wh_Log(L"Wh_ModUninit: entering");
    g_running = false;
    StopFfmpeg();
    if (g_dirChangeHandle != INVALID_HANDLE_VALUE) { FindCloseChangeNotification(g_dirChangeHandle); g_dirChangeHandle = INVALID_HANDLE_VALUE; }

    if (HWND hProgman = GetProgmanWnd()) {
        RunFromWindowThread(hProgman, [](void*) {
            if (g_createVideoTimer) { KillTimer(nullptr, g_createVideoTimer); g_createVideoTimer = 0; }
            if (g_videoHwnd) { SendMessage(g_videoHwnd, WM_APP_CLEANUP, 0, 0); }
        }, nullptr);
    }

    ReleaseSwapChainResources();
    if (g_videoHwnd) { DestroyWindow(g_videoHwnd); g_videoHwnd = NULL; }
    if (g_classRegistered) {
        UnregisterClassW(L"VidWallpaperWnd", GetCurrentModuleHandle());
        g_classRegistered = false;
    }
    UninitDirectX();
    for (int i = 0; i < VIDEO_QUEUE_SIZE; i++) g_frameQueue[i].clear();
    g_frameSize = 0;
    g_workerW = NULL;
    if (g_hThemeKey) { RegCloseKey(g_hThemeKey); g_hThemeKey = NULL; }
    if (g_hThemeEvt) { CloseHandle(g_hThemeEvt); g_hThemeEvt = NULL; }
    if (g_mutex) { CloseHandle(g_mutex); g_mutex = NULL; }
    Wh_Log(L"Wh_ModUninit: done");
}