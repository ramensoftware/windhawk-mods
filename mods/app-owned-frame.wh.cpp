// ==WindhawkMod==
// @id              app-owned-frame
// @name            App Owned Frame
// @description     Restores DWM clipping for selected custom-frame applications without changing their window styles or resizing.
// @version         1.44
// @author          appEW
// @github          https://github.com/appEW
// @license         MIT
// @include         windhawk.exe
// @include         Discord.exe
// @include         Photoshop.exe
// @include         Resolve.exe
// @compilerOptions -ldwmapi -ladvapi32 -lshlwapi -lgdi32 -lshcore -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# App Owned Frame

Some applications draw their own title bar. With the Windows classic theme,
they can show an extra white border or a second system title bar. When maximized,
their hidden frame edges can also become visible on the neighbouring monitor.

This mod keeps the application's own frame and restores normal maximized-window
clipping. It does not remove resizable-window styles, replace the application's
buttons, change the client layout or run its own resize loop.

## Requirements

- Windows 11, build 22000 or later, and Windhawk 1.7.3 or later.
- An already enabled classic theme, for example
  [Classic Theme](https://windhawk.net/mods/classic-theme-enable) or
  [Classic Theme Enable with extended compatibility](https://windhawk.net/mods/classic-theme-enable-with-extended-compatibility).
  This mod is a compatibility fix, not a classic-theme enabler.

## Default application profiles

| Application | Default treatment |
| --- | --- |
| Discord | Remove the extra white frame; use the normal resize cursor with wider grips. |
| ChatGPT and Claude | Fix the outer frame and maximized clipping without loading this mod into the app. |
| Photoshop | Keep its own title bar and prepare it during startup. |
| DaVinci Resolve | Keep its own title bar and colour a remaining resize gutter to match the dark UI. |

These profiles were tested on the submitter's setup. App updates, Windows
versions and other frame/DPI mods can affect the result.

The dedicated helper and the in-process application hooks share one mod source.
Windhawk 1.7.3 is the runtime tested on this setup; Windhawk 2.0 alpha has not
yet been runtime-tested with this combined configuration.

## Adding or excluding an application

1. Open this mod's **Settings** tab and add the executable filename or full path
   under **Applications with their own frame**. Wildcards `*` and `?` are supported.
2. Leave **Frame geometry** on **Automatic** initially. Use **Off** to exclude an
   entry without deleting it. Only main resizable windows with an app-drawn title
   bar are handled; ordinary Windows captions, dialogs and tool windows are skipped.
3. For an extra classic title bar like Photoshop or Resolve, enable
   **Use the application's own title bar**. Open this mod's **Advanced** tab,
   add the executable to the **Custom process inclusion list**, and save.
4. For Chromium/Electron apps, **Native resize hit testing** is optional and also
   requires that Advanced inclusion entry. Enable it only after testing the app.
   Discord is included by default. Do not add Claude or ChatGPT to that list
   merely to fix their border; their default profiles intentionally stay external.

The application list chooses which windows to treat. The Advanced inclusion
list is needed only for features which load code into the application.
When an enabled feature needs different hooks, saving settings reloads this
mod in that process. Changing the grip size or colour does not require an app restart.

## Easier resizing

**Easier resizing from all corners and edges** widens the grab areas without
drawing a frame. All four corners use the same size; edges use half that size.
The size is in pixels at 100% scale and is adjusted for the window's monitor.

Discord's native mode provides the usual hover resize cursor. For external-only
apps the cursor changes when the drag starts, not while hovering. Larger grips
extend into the window and can overlap the edge of a scrollbar or caption button;
reduce the size or disable wider grips if this makes an app control hard to click.

## Other options and limitations

- **Residual resize gutter color** paints only the remaining frame gutter, not
  client content. Resolve defaults to `#17181a`; leave it empty to disable.
- **Prepare the custom title bar at startup** is enabled for Photoshop. Other
  apps must opt in after testing. It takes priority over the experimental
  **Delegate caption painting to DWM** option.
- **Remove system backdrop from the frame** is for a remaining bright material
  edge and requires Windows 11 build 22621 or later.
- The external correction runs in a dedicated Windhawk process, not Explorer.
  A clean disable restores captured values. Scalar rollback data stays on each
  window if that helper crashes; reloading the mod recovers it after validating
  the application and former helper process identities. Unreadable original
  border/caption colours return to the system default.
- This does not fix unrelated application rendering bugs, DPI-projection bugs
  or conflicts with another mod which keeps rewriting the same frame state.
  No application/system light or dark setting is changed.

## Screenshots

Before: without the mod. After: with the mod. Provided by the submitter.

### Discord

| Before | After |
| --- | --- |
| ![Discord without the mod](https://raw.githubusercontent.com/appEW/windhawk-mods/1e9ac8a344e1c10d659226d0cb77f9eca7134042/beforeDS.png) | ![Discord with the mod](https://raw.githubusercontent.com/appEW/windhawk-mods/1e9ac8a344e1c10d659226d0cb77f9eca7134042/afterDS.png) |

### Photoshop

| Before | After |
| --- | --- |
| ![Photoshop without the mod](https://raw.githubusercontent.com/appEW/windhawk-mods/1e9ac8a344e1c10d659226d0cb77f9eca7134042/beforePH.png) | ![Photoshop with the mod](https://raw.githubusercontent.com/appEW/windhawk-mods/1e9ac8a344e1c10d659226d0cb77f9eca7134042/afterPH.png) |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- programs:
  - - executable: Discord.exe
      $name: Executable filename or full path
      $name:ru: Имя программы или полный путь
    - frameGeometry: auto
      $name: Frame geometry
      $name:ru: Тип собственной рамки
      $description: Automatic detects main windows with an app-drawn title bar. Ordinary Windows title bars, dialogs and pop-ups are ignored. Off excludes this app without deleting its entry.
      $description:ru: Авто — собственная клиентская рамка или симметричный отступ растягивания, в том числе Qt. Обычные заголовки, диалоги и служебные окна исключены. «Не обрабатывать» отключает запись без удаления.
      $options:
      - auto: Automatic (custom client or resize-only gutter)
      - compact: Compact custom-client frame
      - resizeGutter: Custom caption with a wider resize gutter
      - off: Do not process
    - legacyPaint: false
      $name: Use the application's own title bar
      $name:ru: Использовать собственный заголовок программы
      $description: Removes an extra classic title bar in apps like Photoshop and Resolve. Also add the executable to this mod's Advanced tab, Custom process inclusion list. Not needed for Electron.
      $description:ru: Убирает лишний классический заголовок в программах типа Photoshop и Resolve. Также добавьте exe на вкладке Advanced этого мода в Custom process inclusion list. Для Electron не нужно.
    - legacyGutterColor: ""
      $name: Residual resize gutter color (empty disables)
      $name:ru: Цвет остаточного системного отступа (пусто — выключено)
      $description: Optional #RRGGBB flat fill for the allocated non-client gutter, including the same DWM caption color for a residual top strip. Requires legacyPaint and Advanced inclusion. Never paints client/buttons/ordinary native captions. No geometry changes. Off restores queried original caption color or system default.
      $description:ru: Необязательный цвет #RRGGBB для оставшегося системного отступа, включая верхнюю полосу DWM. Нужны legacyPaint и Advanced. Клиент, кнопки и обычные заголовки не закрашиваются; геометрия не меняется. При отключении восстанавливается прочитанный цвет заголовка или системный по умолчанию.
    - dwmCaptionPaint: false
      $name: Delegate caption painting to DWM, not classic GDI
      $name:ru: Передавать отрисовку заголовка DWM, не классическому GDI
      $description: Experimental for legacy custom captions; requires legacyPaint and Advanced inclusion. Uses real messages only. No geometry, style or maximize changes.
      $description:ru: Эксперимент для собственных legacy-заголовков; нужны legacyPaint и включение Advanced. Только реальные сообщения. Не меняет размер, стиль или максимизацию.
    - earlyDwm: false
      $name: Prepare the custom title bar at startup
      $name:ru: Подготавливать собственный заголовок при запуске
      $description: Enabled for Photoshop; test before enabling for another app. Requires Use the application's own title bar and Advanced inclusion. Takes priority over Delegate caption painting to DWM. Does not resize or maximize the window.
      $description:ru: Включено для Photoshop; для других программ сначала проверьте результат. Нужны собственный заголовок и список Advanced. Приоритетнее отрисовки заголовка DWM; не растягивает и не разворачивает окно.
    - removeBackdrop: true
      $name: Remove system backdrop from the frame
      $name:ru: Убирать системный фон DWM в рамке
      $description: For the remaining bright Mica edge. Keeps client geometry; restores the original backdrop on disable. Requires Windows 11 build 22621 or later.
      $description:ru: Устраняет светлый край Mica без изменения клиентской области. Исходный фон возвращается при отключении. Windows 11 build 22621+.
    - nativeResize: true
      $name: Native resize hit testing
      $name:ru: Штатный захват углов и краёв
      $description: For Chrome_WidgetWin_1 windows; also requires this executable in Advanced inclusion settings. Enabled only for Discord by default; not legacy painting.
      $description:ru: Для окон Chrome_WidgetWin_1; exe также нужен в списке включения Advanced. По умолчанию только Discord. Меняет только hit testing, не рисует рамку.
  - - executable: ChatGPT.exe
    - frameGeometry: auto
    - legacyPaint: false
    - legacyGutterColor: ""
    - dwmCaptionPaint: false
    - earlyDwm: false
    - removeBackdrop: false
    - nativeResize: false
  - - executable: claude.exe
    - frameGeometry: auto
    - legacyPaint: false
    - legacyGutterColor: ""
    - dwmCaptionPaint: false
    - earlyDwm: false
    - removeBackdrop: false
    - nativeResize: false
  - - executable: Photoshop.exe
    - frameGeometry: auto
    - legacyPaint: true
    - legacyGutterColor: ""
    - dwmCaptionPaint: true
    - earlyDwm: true
    - removeBackdrop: false
    - nativeResize: false
  - - executable: Resolve.exe
    - frameGeometry: auto
    - legacyPaint: true
    - legacyGutterColor: "#17181a"
    - dwmCaptionPaint: false
    - earlyDwm: false
    - removeBackdrop: false
    - nativeResize: false
  $name: Applications with their own frame
  $name:ru: Программы с собственной рамкой
  $description: Only matching main custom-frame windows are modified. Wildcards * and ? are supported.
  $description:ru: Обрабатываются только главные окна с собственной рамкой. Поддерживаются * и ?.
- resizeCorners: true
  $name: Easier resizing from all corners and edges
  $name:ru: Удобное растягивание за все углы и края
  $description: Equal corner squares and edge strips (half the corner size). Native mode supplies native cursor and input; external mode has no hover cursor override. No helper windows.
  $description:ru: Одинаковые углы и полосы краёв (половина размера угла). Штатный режим даёт обычный курсор и ввод; во внешнем режиме курсор меняется после начала жеста. Без окон-накладок.
- resizeGripSize: 16
  $name: Corner size at 100% scale (edges use half)
  $name:ru: Размер угла при 100% (края — половина)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <shellscalingapi.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <climits>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
struct CompositionData {
    int attribute;
    void* value;
    SIZE_T size;
};
using SetComposition = BOOL(WINAPI*)(HWND, CompositionData*);
SetComposition setComposition = nullptr;
constexpr int kNcRenderingExiled = 11;
constexpr DWORD kBorderColor = 34;
constexpr DWORD kSystemBackdrop = 38;
constexpr COLORREF kNoBorder = 0xFFFFFFFE;
constexpr COLORREF kDefaultBorder = 0xFFFFFFFF;
constexpr UINT kReconfigure = WM_APP + 33;
constexpr UINT kResizeRequest = WM_APP + 34;
constexpr UINT kBeginResize = WM_APP + 35;
constexpr wchar_t kIdentityProperty[] = L"Windhawk.AppOwnedFrame.Identity.1";
constexpr wchar_t kResizeProperty[] = L"Windhawk.AppOwnedFrame.ResizeRequests.1";
constexpr wchar_t kNativeResizeProperty[] = L"Windhawk.AppOwnedFrame.NativeResize.1";
constexpr wchar_t kNativePressProperty[] = L"Windhawk.AppOwnedFrame.NativePresses.1";

// FRAME_GEOMETRY_TYPES_BEGIN
enum class FrameGeometry { Automatic, Compact, ResizeGutter, Disabled };
// FRAME_GEOMETRY_TYPES_END
struct ProgramGeometry {
    std::wstring pattern;
    FrameGeometry mode;
};
struct ProgramGutter { std::wstring pattern; COLORREF color; };
// CAPTION_FILL_STATE_BEGIN: exact production state tested without app windows.
struct CaptionFillState {
    COLORREF original = 0xFFFFFFFF;
    COLORREF applied = 0xFFFFFFFE;
    bool captured = false, readable = false, changed = false;
};

bool ReconcileCaptionFill(HWND window, CaptionFillState& state, COLORREF color,
                          bool force = false) {
    constexpr DWORD captionAttribute = 35; // DWMWA_CAPTION_COLOR, Win11 22000+.
    if (color == 0xFFFFFFFE && !state.changed) return true;
    if (color != 0xFFFFFFFE && state.changed && state.applied == color && !force)
        return true;
    if (color != 0xFFFFFFFE && !state.captured) {
        COLORREF original = 0xFFFFFFFF;
        state.readable = SUCCEEDED(DwmGetWindowAttribute(
            window,captionAttribute,&original,sizeof(original)));
        state.original = state.readable ? original : 0xFFFFFFFF;
        state.captured = true;
    }
    const COLORREF value = color == 0xFFFFFFFE ? state.original : color;
    if (FAILED(DwmSetWindowAttribute(window,captionAttribute,&value,sizeof(value))))
        return false;
    if (color == 0xFFFFFFFE) state = {};
    else { state.applied = color; state.changed = true; }
    return true;
}
// CAPTION_FILL_STATE_END

struct ProcessInfo {
    DWORD pid;
    FILETIME created;
    bool eligible;
    bool removeBackdrop;
    FrameGeometry geometry = FrameGeometry::Automatic;
    COLORREF gutterColor = kNoBorder;
};
struct WindowState {
    ProcessInfo process;
    bool originalNcRendering;
    HANDLE identity = nullptr;
    bool compositionChanged = false;
    bool borderChanged = false;
    COLORREF originalBorder = kDefaultBorder;
    bool borderReadable = false;
    DWORD originalBackdrop = 0;
    bool backdropReadable = false;
    bool backdropChanged = false;
    uintptr_t resizeRequests = 0;
    CaptionFillState captionFill;
    ULONGLONG nextUnreadableColorCheck = 0;
    bool retiring = false;
};
struct ResizeTarget {
    HWND target;
    HANDLE identity;
    RECT window;
    RECT frame;
    int cornerSize;
    int edgeSize;
};
ResizeTarget resizeTarget{};
struct PendingResize {
    ResizeTarget target;
    POINT physical;
    WPARAM direction;
};
// Used only by the input thread (including its low-level callback).
PendingResize pendingResize{};
UINT_PTR resizeTimer = 0;
std::mutex resizeMutex;
std::atomic<int> activeGripSize{0};
std::atomic<bool> externalResizeNeeded{false};
HHOOK mouseHook = nullptr;
HMODULE modModule = nullptr;
int configuredGripSize = 0;
int gripSize = 0;
std::unordered_map<HWND, WindowState> windows;
std::vector<BYTE> userSid;
std::mutex settingsMutex;
std::vector<std::wstring> configuredPrograms;
std::vector<ProgramGeometry> configuredGeometries;
std::vector<ProgramGeometry> geometries;
std::atomic<FrameGeometry> localFrameGeometry{FrameGeometry::Automatic};
std::vector<std::wstring> configuredLegacyPrograms;
std::vector<ProgramGutter> configuredGutters;
std::vector<std::wstring> configuredDwmCaptionPrograms;
std::vector<std::wstring> configuredEarlyDwmPrograms;
std::vector<std::wstring> configuredBackdropPrograms;
std::vector<std::wstring> configuredNativeResizePrograms;
std::vector<std::wstring> programs;
std::vector<std::wstring> backdropPrograms;
std::vector<std::wstring> legacyPrograms;
std::vector<ProgramGutter> gutterPrograms;
std::atomic<bool> legacyPaintEnabled{false};
std::atomic<COLORREF> legacyGutterColor{kNoBorder};
thread_local bool paintingLegacyGutter = false;
bool legacyProcess = false;
// Hook availability is fixed at init; runtime feature flags may change later.
bool legacyHooksInstalled = false;
std::atomic<bool> dwmCaptionEnabled{false};
thread_local bool inDwmCaptionPaint = false;
std::atomic<bool> earlyDwmEnabled{false};
SetComposition earlyCompositionOriginal = nullptr;
decltype(&DwmExtendFrameIntoClientArea) earlyMarginsOriginal = nullptr;
thread_local bool inEarlyDwm = false;
constexpr wchar_t kEarlyDwmIdentity[] = L"Windhawk.AppOwnedFrame.EarlyDwm.1.Identity";
constexpr wchar_t kEarlyDwmPrefix[] = L"Windhawk.AppOwnedFrame.EarlyDwm.1.";
struct EarlyDwmState {
    HANDLE identity = nullptr;
    BOOL desiredExile = FALSE;
    bool changed = false;
    uintptr_t requests = 0, intercepted = 0, prepared = 0;
    unsigned activeCalls = 0;
    bool clearRequested = false, restoreRequested = false, destroyed = false;
};
std::mutex earlyDwmMutex;
std::unordered_map<HWND, EarlyDwmState> earlyDwmWindows;
uintptr_t nextEarlyDwmIdentity = 0;
std::wstring currentPath;
using DefaultProc = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
DefaultProc defaultProcW = nullptr;
DefaultProc defaultProcA = nullptr;
DefaultProc nativeWindowProc = nullptr;
DefaultProc nativeRendererProc = nullptr;
void* nativeWindowTarget = nullptr;
void* nativeRendererTarget = nullptr;
using RegisterClassExW_t = ATOM(WINAPI*)(const WNDCLASSEXW*);
RegisterClassExW_t registerClass = nullptr;
std::mutex nativeHookMutex;
std::atomic<bool> nativeClassHooked{false};
std::atomic<bool> nativeRendererHooked{false};
std::atomic<int> nativeGripSize{0};
bool nativeResizeProcess = false;
struct NativeFrameCache {
    HWND window = nullptr;
    HANDLE identity = nullptr;
    HMONITOR monitor = nullptr;
    LONG left = 0, top = 0, right = 0, bottom = 0;
    int corner = 0, edge = 0, grip = 0;
};
std::mutex nativeFrameMutex;
NativeFrameCache nativeFrame{};
std::atomic<DWORD> workerId{0};
std::atomic<bool> stopping{false};
HANDLE stopEvent = nullptr;
HANDLE readyEvent = nullptr;
std::atomic<bool> workerReady{false};
HANDLE workerThread = nullptr;
HANDLE inputThread = nullptr;
std::atomic<DWORD> inputThreadId{0};
std::array<HWINEVENTHOOK,4> objectHooks{};
HWINEVENTHOOK foregroundHook = nullptr;
HWINEVENTHOOK sizingHook = nullptr;
std::unordered_set<HWND> sizingWindows;
std::unordered_map<HWND, bool> pendingWindows;
ULONGLONG pendingDue = 0;
bool foregroundChanged = false;
bool sizingChanged = false;
struct LocationHookState {
    HWINEVENTHOOK hook;
    FILETIME created;
};
std::unordered_map<DWORD,LocationHookState> locationHooks;
bool locationHooksDirty = true;
ULONGLONG locationHooksRetryAfter = 0;
uintptr_t nextIdentity = 0;
bool updatingWindow = false;
bool toolContext = false;
struct UpdateScope {
    bool prior = updatingWindow;
    UpdateScope() { updatingWindow = true; }
    ~UpdateScope() { updatingWindow = prior; }
};

// LEGACY_GUTTER_GEOMETRY_BEGIN: exact code tested by Test-LegacyGutter.ps1.
COLORREF ParseLegacyGutterColor(const std::wstring& value) {
    if (value.size() != 7 || value.front() != L'#') return kNoBorder;
    unsigned color = 0;
    for (size_t i=1; i<value.size(); ++i) {
        const wchar_t c = value[i];
        const int nibble = c >= L'0' && c <= L'9' ? c-L'0' :
            c >= L'a' && c <= L'f' ? c-L'a'+10 :
            c >= L'A' && c <= L'F' ? c-L'A'+10 : -1;
        if (nibble < 0) return kNoBorder;
        color = (color << 4) | static_cast<unsigned>(nibble);
    }
    return RGB((color>>16)&255,(color>>8)&255,color&255);
}

bool LegacyGutterRects(const RECT& window, const RECT& client, int limit,
                       std::array<RECT,4>& strips, RECT& localClient) {
    const int64_t width = int64_t(window.right)-window.left;
    const int64_t height = int64_t(window.bottom)-window.top;
    const int64_t left = int64_t(client.left)-window.left;
    const int64_t top = int64_t(client.top)-window.top;
    const int64_t right = int64_t(window.right)-client.right;
    const int64_t bottom = int64_t(window.bottom)-client.bottom;
    if (limit < 0 || width <= 0 || height <= 0 || width > LONG_MAX ||
        height > LONG_MAX || client.right <= client.left || client.bottom <= client.top ||
        left < 0 || top < 0 || right < 0 || bottom < 0 ||
        left > limit || top > limit || right > limit || bottom > limit) return false;
    const LONG w=static_cast<LONG>(width), h=static_cast<LONG>(height);
    localClient={static_cast<LONG>(left),static_cast<LONG>(top),
        static_cast<LONG>(width-right),static_cast<LONG>(height-bottom)};
    strips={RECT{0,0,localClient.left,h},RECT{localClient.right,0,w,h},
        RECT{localClient.left,0,localClient.right,localClient.top},
        RECT{localClient.left,localClient.bottom,localClient.right,h}};
    return true;
}
// LEGACY_GUTTER_GEOMETRY_END

// LEGACY_GUTTER_PAINT_BEGIN: exact GDI drawing tested offscreen, no real windows.
bool FillLegacyGutter(HDC dc, HBRUSH brush, const std::array<RECT,4>& strips,
                     const RECT& client) {
    const int saved = SaveDC(dc);
    if (!saved) return false;
    bool success = ExcludeClipRect(dc,client.left,client.top,client.right,client.bottom) != ERROR;
    if (success) {
        for (const RECT& strip : strips)
            if (strip.right > strip.left && strip.bottom > strip.top &&
                !FillRect(dc,&strip,brush)) success=false;
    }
    RestoreDC(dc,saved);
    return success;
}
// LEGACY_GUTTER_PAINT_END

void LoadSettings() {
    std::vector<std::wstring> result;
    std::vector<ProgramGeometry> geometryRules;
    std::vector<std::wstring> legacy;
    std::vector<ProgramGutter> gutterRules;
    std::vector<std::wstring> dwmCaptions;
    std::vector<std::wstring> earlyDwms;
    std::vector<std::wstring> backdrops;
    std::vector<std::wstring> nativeResize;
    for (int i = 0; i < 256; ++i) {
        const std::wstring key = L"programs[" + std::to_wstring(i) + L"].executable";
        PCWSTR setting = Wh_GetStringSetting(key.c_str());
        std::wstring value = setting ? setting : L"";
        if (setting) Wh_FreeStringSetting(setting);
        if (value.empty()) break;
        const size_t first = value.find_first_not_of(L" \t\r\n");
        const size_t last = value.find_last_not_of(L" \t\r\n");
        if (first != std::wstring::npos) {
            result.push_back(value.substr(first, last-first+1));
            const std::wstring geometryKey = L"programs[" + std::to_wstring(i) + L"].frameGeometry";
            PCWSTR geometrySetting = Wh_GetStringSetting(geometryKey.c_str());
            const std::wstring geometryValue = geometrySetting ? geometrySetting : L"";
            if (geometrySetting) Wh_FreeStringSetting(geometrySetting);
            const FrameGeometry geometry = geometryValue == L"off" ? FrameGeometry::Disabled :
                geometryValue == L"compact" ? FrameGeometry::Compact :
                geometryValue == L"resizeGutter" ? FrameGeometry::ResizeGutter : FrameGeometry::Automatic;
            geometryRules.push_back({result.back(), geometry});
            const std::wstring paintKey = L"programs[" + std::to_wstring(i) + L"].legacyPaint";
            if (Wh_GetIntSetting(paintKey.c_str())) legacy.push_back(result.back());
            const std::wstring gutterKey = L"programs[" + std::to_wstring(i) + L"].legacyGutterColor";
            PCWSTR gutterSetting = Wh_GetStringSetting(gutterKey.c_str());
            const std::wstring gutterValue = gutterSetting ? gutterSetting : L"";
            if (gutterSetting) Wh_FreeStringSetting(gutterSetting);
            gutterRules.push_back({result.back(),ParseLegacyGutterColor(gutterValue)});
            const std::wstring dwmCaptionKey = L"programs[" + std::to_wstring(i) + L"].dwmCaptionPaint";
            if (Wh_GetIntSetting(dwmCaptionKey.c_str())) dwmCaptions.push_back(result.back());
            const std::wstring earlyDwmKey = L"programs[" + std::to_wstring(i) + L"].earlyDwm";
            if (Wh_GetIntSetting(earlyDwmKey.c_str())) earlyDwms.push_back(result.back());
            const std::wstring backdropKey = L"programs[" + std::to_wstring(i) + L"].removeBackdrop";
            if (Wh_GetIntSetting(backdropKey.c_str())) backdrops.push_back(result.back());
            const std::wstring nativeKey = L"programs[" + std::to_wstring(i) + L"].nativeResize";
            if (Wh_GetIntSetting(nativeKey.c_str())) nativeResize.push_back(result.back());
        }
    }
    std::lock_guard lock(settingsMutex);
    configuredPrograms = std::move(result);
    configuredGeometries = std::move(geometryRules);
    configuredLegacyPrograms = std::move(legacy);
    configuredGutters = std::move(gutterRules);
    configuredDwmCaptionPrograms = std::move(dwmCaptions);
    configuredEarlyDwmPrograms = std::move(earlyDwms);
    configuredBackdropPrograms = std::move(backdrops);
    configuredNativeResizePrograms = std::move(nativeResize);
    configuredGripSize = Wh_GetIntSetting(L"resizeCorners")
        ? std::clamp(Wh_GetIntSetting(L"resizeGripSize"), 6, 24) : 0;
}

bool ReadUserSid(HANDLE process, std::vector<BYTE>& output) {
    HANDLE token = nullptr;
    if (!OpenProcessToken(process, TOKEN_QUERY, &token)) return false;
    DWORD bytes = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &bytes);
    output.resize(bytes);
    const bool success = bytes &&
        GetTokenInformation(token, TokenUser, output.data(), bytes, &bytes);
    CloseHandle(token);
    return success;
}

bool MatchList(const wchar_t* path, const std::vector<std::wstring>& patterns) {
    const wchar_t* basename = PathFindFileNameW(path);
    for (const auto& pattern : patterns) {
        const wchar_t* candidate = pattern.find_first_of(L"\\/") == std::wstring::npos
            ? basename : path;
        if (PathMatchSpecW(candidate, pattern.c_str())) return true;
    }
    return false;
}

bool MatchProgram(const wchar_t* path) { return MatchList(path, programs); }

COLORREF MatchGutter(const wchar_t* path,
                     const std::vector<ProgramGutter>& rules = configuredGutters) {
    const wchar_t* basename = PathFindFileNameW(path);
    for (const auto& rule : rules) {
        const wchar_t* candidate = rule.pattern.find_first_of(L"\\\\/") == std::wstring::npos ? basename : path;
        if (PathMatchSpecW(candidate,rule.pattern.c_str())) return rule.color;
    }
    return kNoBorder;
}

FrameGeometry MatchGeometry(const wchar_t* path, const std::vector<ProgramGeometry>& rules) {
    const wchar_t* basename = PathFindFileNameW(path);
    for (const auto& rule : rules) {
        const wchar_t* candidate = rule.pattern.find_first_of(L"\\/") == std::wstring::npos
            ? basename : path;
        if (PathMatchSpecW(candidate, rule.pattern.c_str())) return rule.mode;
    }
    return FrameGeometry::Automatic;
}

// PROCESS_CACHE_BEGIN
struct CachedProcess {
    HANDLE handle = nullptr;
    ProcessInfo info{};
    bool sameUser = false;
};
std::unordered_map<DWORD,CachedProcess> processCache;

void ClearProcessCache() {
    for (const auto& [pid,entry] : processCache) CloseHandle(entry.handle);
    processCache.clear();
}

void PruneProcessCache() {
    for (auto it=processCache.begin();it!=processCache.end();) {
        if (WaitForSingleObject(it->second.handle,0) != WAIT_TIMEOUT) {
            CloseHandle(it->second.handle);
            it=processCache.erase(it);
        } else ++it;
    }
}

bool InspectProcess(DWORD pid, ProcessInfo* info) {
    auto cached = processCache.find(pid);
    if (cached != processCache.end()) {
        // The retained process handle identifies the old process even after
        // its PID is reused. No image/path/token query on the ordinary poll.
        if (WaitForSingleObject(cached->second.handle,0) == WAIT_TIMEOUT) {
            *info = cached->second.info;
            return cached->second.sameUser;
        }
        CloseHandle(cached->second.handle);
        processCache.erase(cached);
    }
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, pid);
    if (!process) return false;
    FILETIME created{}, exited{}, kernel{}, user{};
    wchar_t path[32768];
    DWORD chars = ARRAYSIZE(path);
    std::vector<BYTE> owner;
    const bool success = GetProcessTimes(process, &created, &exited, &kernel, &user) &&
        QueryFullProcessImageNameW(process, 0, path, &chars) &&
        ReadUserSid(process, owner);
    const bool sameUser = success && EqualSid(
        reinterpret_cast<TOKEN_USER*>(owner.data())->User.Sid,
        reinterpret_cast<TOKEN_USER*>(userSid.data())->User.Sid);
    if (!sameUser) {
        if (success && processCache.size() < 512)
            processCache.emplace(pid,CachedProcess{process,{},false});
        else CloseHandle(process);
        return false;
    }
    const FrameGeometry geometry = MatchGeometry(path, geometries);
    *info = {pid, created, MatchProgram(path) && geometry != FrameGeometry::Disabled,
        MatchList(path, backdropPrograms), geometry,
        MatchList(path,legacyPrograms) ? MatchGutter(path,gutterPrograms) : kNoBorder};
    if (processCache.size() < 512)
        processCache.emplace(pid,CachedProcess{process,*info,true});
    else CloseHandle(process);
    return true;
}
// PROCESS_CACHE_END

bool SameWindow(HWND window, const WindowState& state) {
    DWORD pid = 0;
    if (!IsWindow(window) || !GetWindowThreadProcessId(window, &pid) ||
        pid != state.process.pid || GetPropW(window, kIdentityProperty) != state.identity) return false;
    ProcessInfo current{};
    return InspectProcess(pid, &current) &&
        CompareFileTime(&current.created, &state.process.created) == 0;
}

UINT PhysicalFrameDpi(HWND window) {
    // GetWindowInfo and DWM bounds are physical on our PMv2 worker, but a
    // bitmap-scaled/DPI-unaware target still reports GetDpiForWindow=96.
    // Compare geometry with the monitor's physical scale, not that logical
    // DPI. Otherwise the 4px gutter becomes 9px at 225% and is rejected.
    DEVICE_SCALE_FACTOR scale = SCALE_100_PERCENT;
    if (SUCCEEDED(GetScaleFactorForMonitor(
            MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &scale)) &&
        scale >= SCALE_100_PERCENT && static_cast<int>(scale) <= 500)
        return MulDiv(USER_DEFAULT_SCREEN_DPI, static_cast<int>(scale), 100);
    const UINT dpi = GetDpiForWindow(window);
    return dpi ? dpi : USER_DEFAULT_SCREEN_DPI;
}

// FRAME_GEOMETRY_BEGIN: exact production predicate compiled by Test-FrameGeometry.ps1.
bool HasCustomFrameGeometry(const WINDOWINFO& info, int compactGutter,
                            int resizeGutter, int tolerance, FrameGeometry mode) {
    constexpr DWORD requiredStyle = WS_CAPTION | WS_THICKFRAME;
    if (mode == FrameGeometry::Disabled || compactGutter < 0 ||
        resizeGutter < compactGutter || tolerance < 0 ||
        (info.dwStyle & requiredStyle) != requiredStyle ||
        (info.dwStyle & (WS_CHILD | WS_DISABLED)) ||
        (info.dwExStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) ||
        info.rcWindow.right-info.rcWindow.left < 200 ||
        info.rcWindow.bottom-info.rcWindow.top < 150) return false;
    const LONG top = info.rcClient.top-info.rcWindow.top;
    const LONG left = info.rcClient.left-info.rcWindow.left;
    const LONG right = info.rcWindow.right-info.rcClient.right;
    const LONG bottom = info.rcWindow.bottom-info.rcClient.bottom;
    const auto within = [&](int gutter) {
        return top >= 0 && top <= gutter && left >= 0 && left <= gutter &&
            right >= 0 && right <= gutter && bottom >= 0 && bottom <= gutter;
    };
    if (within(compactGutter)) return true;
    if (mode == FrameGeometry::Compact || !within(resizeGutter)) return false;
    if (mode == FrameGeometry::ResizeGutter) return true;
    // Wider auto frames must retain only symmetric resize insets, not a
    // separately allocated system caption. No executable/framework check.
    return left > 0 && right > 0 && bottom > 0 &&
        std::abs(left-right) <= tolerance && std::abs(left-bottom) <= tolerance;
}
// FRAME_GEOMETRY_END

bool CustomMainWindow(HWND window, FrameGeometry mode = FrameGeometry::Automatic,
                      bool allowHidden = false) {
    if ((!allowHidden && !IsWindowVisible(window)) || IsIconic(window) ||
        GetAncestor(window, GA_ROOT) != window || GetWindow(window, GW_OWNER)) return false;
    WINDOWINFO info{sizeof(info)};
    if (!GetWindowInfo(window, &info)) return false;
    const UINT dpi = PhysicalFrameDpi(window);
    const int nativeGutter = GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi) +
        GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
    const int referenceGutter = GetSystemMetricsForDpi(SM_CYSIZEFRAME, USER_DEFAULT_SCREEN_DPI) +
        GetSystemMetricsForDpi(SM_CXPADDEDBORDER, USER_DEFAULT_SCREEN_DPI);
    const int compact = std::max(nativeGutter,
        MulDiv(referenceGutter, dpi, USER_DEFAULT_SCREEN_DPI)) + 2;
    // Some custom-caption apps retain 8px resize insets despite the
    // classic system's reduced metrics. This is not a native title bar.
    const int wider = std::max(compact, MulDiv(8, dpi, USER_DEFAULT_SCREEN_DPI) + 2);
    return HasCustomFrameGeometry(info, compact, wider,
        std::max(1, MulDiv(2, dpi, USER_DEFAULT_SCREEN_DPI)), mode);
}

// RESIZE_GEOMETRY_BEGIN: this exact function is exercised by Test-ResizeZones.ps1.
WPARAM ResizeDirection(const RECT& window, const RECT& frame, POINT point,
                       int cornerSize, int edgeSize) {
    if (cornerSize <= 0 || edgeSize <= 0 ||
        window.right <= window.left || window.bottom <= window.top ||
        frame.right <= frame.left || frame.bottom <= frame.top ||
        !PtInRect(&window, point)) return 0;
    const LONG cornerX = std::min<LONG>(cornerSize, (frame.right-frame.left)/2);
    const LONG cornerY = std::min<LONG>(cornerSize, (frame.bottom-frame.top)/2);
    const LONG edgeX = std::min<LONG>(edgeSize, cornerX);
    const LONG edgeY = std::min<LONG>(edgeSize, cornerY);
    const bool leftCorner = point.x < frame.left + cornerX;
    const bool rightCorner = point.x >= frame.right - cornerX;
    const bool topCorner = point.y < frame.top + cornerY;
    const bool bottomCorner = point.y >= frame.bottom - cornerY;
    // Corners take priority, symmetrically, including the native outer gutter.
    if (leftCorner && topCorner) return WMSZ_TOPLEFT;
    if (rightCorner && topCorner) return WMSZ_TOPRIGHT;
    if (leftCorner && bottomCorner) return WMSZ_BOTTOMLEFT;
    if (rightCorner && bottomCorner) return WMSZ_BOTTOMRIGHT;
    if (point.x < frame.left + edgeX) return WMSZ_LEFT;
    if (point.x >= frame.right - edgeX) return WMSZ_RIGHT;
    if (point.y < frame.top + edgeY) return WMSZ_TOP;
    if (point.y >= frame.bottom - edgeY) return WMSZ_BOTTOM;
    return 0;
}
// RESIZE_GEOMETRY_END

LRESULT HandleNativeHitTest(HWND window, UINT message, WPARAM wp, LPARAM lp,
                           LRESULT original) {
    if (message == WM_DPICHANGED || message == WM_SETTINGCHANGE ||
        message == WM_DWMNCRENDERINGCHANGED || message == WM_NCDESTROY ||
        (message == WM_SIZE && (wp == SIZE_MAXIMIZED || wp == SIZE_MINIMIZED))) {
        std::unique_lock lock(nativeFrameMutex, std::try_to_lock);
        if (lock.owns_lock() && nativeFrame.window == window) nativeFrame = {};
    }
    const int grip = nativeGripSize.load();
    if (message != WM_NCHITTEST || stopping || !grip || !nativeClassHooked ||
        (original != HTCLIENT && original != HTCAPTION &&
         (original < HTLEFT || original > HTBOTTOMRIGHT))) return original;
    const HWND root = GetAncestor(window, GA_ROOT);
    const bool contentWindow = root != window;
    if (!root) return original;
    if (contentWindow) {
        wchar_t name[64]{};
        if (!GetClassNameW(window, name, ARRAYSIZE(name)) ||
            wcscmp(name, L"Chrome_RenderWidgetHostHWND") != 0 ||
            GetWindowThreadProcessId(root, nullptr) !=
                GetWindowThreadProcessId(window, nullptr) ||
            !AreDpiAwarenessContextsEqual(GetWindowDpiAwarenessContext(root),
                                          GetWindowDpiAwarenessContext(window))) return original;
    }
    const HANDLE identity = GetPropW(root, kIdentityProperty);
    if (!identity) return original; // Controller validates custom main windows.
    POINT point{static_cast<short>(LOWORD(lp)), static_cast<short>(HIWORD(lp))};
    if (!LogicalToPhysicalPointForPerMonitorDPI(window, &point)) return original;
    // Windows enters a window procedure in that window's own DPI context.
    // Do not request PMv2 here: a scaling mod may translate that request and
    // even its restoration to another context. Normalize geometry explicitly.
    if (!AreDpiAwarenessContextsEqual(GetThreadDpiAwarenessContext(),
                                      GetWindowDpiAwarenessContext(window))) return original;
    WINDOWINFO info{sizeof(info)};
    if (!GetWindowInfo(root, &info) ||
        (info.dwStyle & (WS_VISIBLE | WS_THICKFRAME)) != (WS_VISIBLE | WS_THICKFRAME) ||
        (info.dwStyle & (WS_MAXIMIZE | WS_MINIMIZE | WS_DISABLED))) return original;
    POINT first{info.rcWindow.left,info.rcWindow.top};
    POINT last{info.rcWindow.right,info.rcWindow.bottom};
    // These vertices belong to the main window, not the content child. The
    // conversion API rejects points outside the HWND supplied to it.
    if (!LogicalToPhysicalPointForPerMonitorDPI(root,&first) ||
        !LogicalToPhysicalPointForPerMonitorDPI(root,&last)) return original;
    info.rcWindow = {first.x,first.y,last.x,last.y};
    const HMONITOR monitor = MonitorFromWindow(root, MONITOR_DEFAULTTONEAREST);
    std::unique_lock lock(nativeFrameMutex, std::try_to_lock);
    if (!lock.owns_lock()) return original;
    if (nativeFrame.window != root || nativeFrame.identity != identity ||
        nativeFrame.monitor != monitor || nativeFrame.grip != grip) {
        RECT frame{};
        if (FAILED(DwmGetWindowAttribute(root, DWMWA_EXTENDED_FRAME_BOUNDS,
                                        &frame, sizeof(frame)))) return original;
        const UINT dpi = PhysicalFrameDpi(root);
        nativeFrame = {root, identity, monitor, frame.left-info.rcWindow.left,
            frame.top-info.rcWindow.top, info.rcWindow.right-frame.right,
            info.rcWindow.bottom-frame.bottom, MulDiv(grip,dpi,96),
            MulDiv(std::max(3,grip/2),dpi,96), grip};
    }
    const RECT frame{info.rcWindow.left+nativeFrame.left, info.rcWindow.top+nativeFrame.top,
        info.rcWindow.right-nativeFrame.right, info.rcWindow.bottom-nativeFrame.bottom};
    const WPARAM direction = ResizeDirection(info.rcWindow, frame, point,
        nativeFrame.corner, nativeFrame.edge);
    // Once per controller identity, not once per movement. The controller
    // then leaves the physical press entirely to native Windows hit testing.
    // Do not acknowledge root-only coverage while the content class is still
    // unhooked: a physical press can hit that child before reaching the root.
    if (nativeRendererHooked && GetPropW(root, kNativeResizeProperty) != identity)
        SetPropW(root, kNativeResizeProperty, identity);
    constexpr std::array<LRESULT,9> hitTests{HTCLIENT,HTLEFT,HTRIGHT,HTTOP,
        HTTOPLEFT,HTTOPRIGHT,HTBOTTOM,HTBOTTOMLEFT,HTBOTTOMRIGHT};
    if (!direction) return original;
    // Chromium's render host already forwards non-client mouse messages to the
    // parent's normal handler. Mirror its native upper-corner result instead
    // of HTTRANSPARENT, whose routing did not start an actual resize here.
    return hitTests[direction];
}

LRESULT DispatchNative(DefaultProc procedure, HWND window, UINT message, WPARAM wp, LPARAM lp) {
    if (message == WM_NCLBUTTONDOWN && wp >= HTLEFT && wp <= HTBOTTOMRIGHT) {
        const LRESULT direction = HandleNativeHitTest(window,WM_NCHITTEST,0,lp,HTCLIENT);
        if (direction == static_cast<LRESULT>(wp)) {
            const HWND root = GetAncestor(window,GA_ROOT);
            const uintptr_t presses = reinterpret_cast<uintptr_t>(GetPropW(root,kNativePressProperty));
            SetPropW(root,kNativePressProperty,reinterpret_cast<HANDLE>(presses+1));
            // Synchronous and on the app's own UI thread. The real mouse down
            // has already been delivered; Windows owns capture and all moves.
            // Chromium can otherwise consume an expanded NC press as a client
            // view press before its normal DefWindowProc resize fallback.
            return DefWindowProcW(root,message,wp,lp);
        }
    }
    if (message == WM_SETCURSOR && nativeGripSize && !stopping &&
        LOWORD(lp) >= HTLEFT && LOWORD(lp) <= HTBOTTOMRIGHT) {
        const HWND root = GetAncestor(window,GA_ROOT);
        if (root && GetPropW(root,kIdentityProperty) &&
            GetWindowThreadProcessId(root,nullptr) == GetWindowThreadProcessId(window,nullptr))
            return DefWindowProcW(root,message,reinterpret_cast<WPARAM>(root),lp);
    }
    // Snapshot the candidate in the incoming window context, before application
    // code or another mod can temporarily change it. The original procedure
    // still decides buttons, scrollbars and every point outside our zones.
    const LRESULT candidate = message == WM_NCHITTEST
        ? HandleNativeHitTest(window,message,wp,lp,HTCLIENT) : HTCLIENT;
    const LRESULT original = procedure(window,message,wp,lp);
    LRESULT result = original;
    if (message == WM_NCHITTEST) {
        if (candidate >= HTLEFT && candidate <= HTBOTTOMRIGHT &&
            (original == HTCLIENT || original == HTCAPTION ||
             (original >= HTLEFT && original <= HTBOTTOMRIGHT))) result = candidate;
    } else HandleNativeHitTest(window,message,wp,lp,original);
    return result;
}

LRESULT CALLBACK NativeWindowProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    return DispatchNative(nativeWindowProc, window, message, wp, lp);
}

LRESULT CALLBACK NativeRendererProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    return DispatchNative(nativeRendererProc, window, message, wp, lp);
}

int NativeClassKind(PCWSTR name) {
    if (!name || IS_INTRESOURCE(name)) return -1;
    if (wcscmp(name, L"Chrome_WidgetWin_1") == 0) return 0;
    if (wcscmp(name, L"Chrome_RenderWidgetHostHWND") == 0) return 1;
    return -1;
}

bool HookNativeClass(void* procedure, bool renderer, bool applyNow) {
    auto& hooked = renderer ? nativeRendererHooked : nativeClassHooked;
    if (!procedure || stopping || hooked) return hooked.load();
    std::lock_guard lock(nativeHookMutex);
    if (stopping || hooked) return hooked.load();
    auto& target = renderer ? nativeRendererTarget : nativeWindowTarget;
    auto& trampoline = renderer ? nativeRendererProc : nativeWindowProc;
    const auto otherTarget = renderer ? nativeWindowTarget : nativeRendererTarget;
    // Chromium versions can register both classes with one shared procedure.
    // Its one detour handles both kinds; never detour a trampoline a second time.
    if (procedure == otherTarget) {
        target = procedure;
        trampoline = renderer ? nativeWindowProc : nativeRendererProc;
        hooked = true;
        return true;
    }
    const DefaultProc proxy = renderer ? NativeRendererProc : NativeWindowProc;
    if (!Wh_SetFunctionHook(procedure, reinterpret_cast<void*>(proxy),
            reinterpret_cast<void**>(&trampoline))) return false;
    target = procedure;
    if (applyNow && !Wh_ApplyHookOperations()) return false;
    hooked = true;
    return true;
}

ATOM WINAPI NativeRegisterClass(const WNDCLASSEXW* definition) {
    const ATOM result = registerClass(definition);
    const int kind = definition ? NativeClassKind(definition->lpszClassName) : -1;
    if (result && !stopping && kind >= 0)
        HookNativeClass(reinterpret_cast<void*>(definition->lpfnWndProc), kind == 1, true);
    return result;
}

BOOL CALLBACK InspectNativeClass(HWND window, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    wchar_t name[128]{};
    if (GetClassNameW(window, name, ARRAYSIZE(name))) {
        const int kind = NativeClassKind(name);
        if (kind >= 0)
            HookNativeClass(reinterpret_cast<void*>(GetClassLongPtrW(window, GCLP_WNDPROC)),
                kind == 1, false);
    }
    return TRUE;
}

BOOL CALLBACK FindNativeClass(HWND window, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid == GetCurrentProcessId()) {
        InspectNativeClass(window, 0);
        EnumChildWindows(window, InspectNativeClass, 0);
    }
    return !(nativeClassHooked && nativeRendererHooked);
}

BOOL CALLBACK ClearNativeAck(HWND window, LPARAM) {
    DWORD pid = 0;
    if (GetWindowThreadProcessId(window, &pid) && pid == GetCurrentProcessId()) {
        RemovePropW(window, kNativeResizeProperty);
        RemovePropW(window, kNativePressProperty);
    }
    return TRUE;
}

LRESULT CALLBACK ResizeMouse(int code, WPARAM message, LPARAM lp) {
    // This hook never handles movement, waits for another process, calls DWM
    // or logs. Do only cached geometry + cheap Win32 validation for a down.
    if (code != HC_ACTION || message != WM_LBUTTONDOWN || stopping || !activeGripSize)
        return CallNextHookEx(mouseHook, code, message, lp);
    ResizeTarget target{};
    {
        std::unique_lock lock(resizeMutex, std::try_to_lock);
        if (!lock.owns_lock()) return CallNextHookEx(mouseHook, code, message, lp);
        target = resizeTarget;
    }
    const POINT physical = reinterpret_cast<const MSLLHOOKSTRUCT*>(lp)->pt;
    if (!target.target || GetForegroundWindow() != target.target ||
        GetPropW(target.target, kNativeResizeProperty) == target.identity)
        return CallNextHookEx(mouseHook, code, message, lp);
    const WPARAM direction = ResizeDirection(target.window, target.frame, physical,
        target.cornerSize, target.edgeSize);
    if (!direction ||
        GetPropW(target.target, kIdentityProperty) != target.identity ||
        IsZoomed(target.target) || IsIconic(target.target) ||
        GetAncestor(WindowFromPoint(physical), GA_ROOT) != target.target)
        return CallNextHookEx(mouseHook, code, message, lp);
    WINDOWINFO info{sizeof(info)};
    if (!GetWindowInfo(target.target, &info) ||
        !EqualRect(&info.rcWindow, &target.window) || !(info.dwStyle & WS_VISIBLE) ||
        (info.dwStyle & WS_DISABLED))
        return CallNextHookEx(mouseHook, code, message, lp);
    pendingResize = {target, physical, direction};
    if (!PostThreadMessageW(inputThreadId.load(), kBeginResize, 0, 0)) pendingResize = {};
    // Do not swallow the down: native mouse sizing needs the real button state.
    // Also don't wake the target from inside the pre-input LL callback.
    return CallNextHookEx(mouseHook, code, message, lp);
}

void CancelPendingResize() {
    if (resizeTimer) { KillTimer(nullptr, resizeTimer); resizeTimer = 0; }
    pendingResize = {};
}

void BeginPendingResize() {
    const PendingResize request = pendingResize;
    CancelPendingResize();
    const auto& target = request.target;
    if (stopping || !activeGripSize || !target.target ||
        !(GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
        GetForegroundWindow() != target.target || IsZoomed(target.target) ||
        IsIconic(target.target) || GetPropW(target.target, kIdentityProperty) != target.identity ||
        GetAncestor(WindowFromPoint(request.physical), GA_ROOT) != target.target) return;
    WINDOWINFO info{sizeof(info)};
    if (!GetWindowInfo(target.target, &info) || !EqualRect(&info.rcWindow, &target.window) ||
        !(info.dwStyle & WS_VISIBLE) || (info.dwStyle & WS_DISABLED)) return;
    POINT logical = request.physical;
    if (!PhysicalToLogicalPointForPerMonitorDPI(target.target, &logical)) return;
    if (GetPropW(target.target, kNativeResizeProperty) == target.identity) return;
    // Keep the user-validated 1.9 delivery for external-only applications.
    if (!PostMessageW(target.target, WM_SYSCOMMAND, SC_SIZE | request.direction,
                      MAKELPARAM(logical.x, logical.y))) return;
    PostThreadMessageW(workerId.load(), kResizeRequest,
        reinterpret_cast<WPARAM>(target.target), reinterpret_cast<LPARAM>(target.identity));
}

void ClearResizeTarget(HWND target) {
    std::lock_guard lock(resizeMutex);
    if (!target || resizeTarget.target == target) {
        resizeTarget = {};
        if (externalResizeNeeded.exchange(false)) {
            const DWORD thread = inputThreadId.load();
            if (thread) PostThreadMessageW(thread,kReconfigure,0,0);
        }
    }
}

void RefreshResizeTarget(HWND target, WindowState& state) {
    if (!gripSize || IsZoomed(target) || IsIconic(target) ||
        !IsWindowVisible(target) || GetForegroundWindow() != target) {
        ClearResizeTarget(target);
        return;
    }
    RECT frame{};
    if (FAILED(DwmGetWindowAttribute(target, DWMWA_EXTENDED_FRAME_BOUNDS,
                                     &frame, sizeof(frame))) ||
        frame.right-frame.left < 200 || frame.bottom-frame.top < 150) {
        ClearResizeTarget(target);
        return;
    }
    const UINT dpi = PhysicalFrameDpi(target);
    const int cornerSize = MulDiv(gripSize, dpi, USER_DEFAULT_SCREEN_DPI);
    const int edgeSize = MulDiv(std::max(3, gripSize/2), dpi, USER_DEFAULT_SCREEN_DPI);
    WINDOWINFO info{sizeof(info)};
    if (GetWindowInfo(target, &info)) {
        std::lock_guard lock(resizeMutex);
        resizeTarget = {target, state.identity, info.rcWindow, frame, cornerSize, edgeSize};
        const bool needed = GetPropW(target,kNativeResizeProperty) != state.identity;
        if (externalResizeNeeded.exchange(needed) != needed) {
            const DWORD thread = inputThreadId.load();
            if (thread) PostThreadMessageW(thread,kReconfigure,0,0);
        }
    }
}

void UpdateMouseHook() {
    if (activeGripSize && externalResizeNeeded && !mouseHook) {
        mouseHook = SetWindowsHookExW(WH_MOUSE_LL, ResizeMouse, modModule, 0);
        if (!mouseHook) Wh_Log(L"Resize mouse hook registration failed: %lu", GetLastError());
    } else if ((!activeGripSize || !externalResizeNeeded) && mouseHook) {
        UnhookWindowsHookEx(mouseHook);
        mouseHook = nullptr;
        CancelPendingResize();
    }
}

DWORD WINAPI InputWorker(void*) {
    const auto priorDpi = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    MSG message{};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    inputThreadId = GetCurrentThreadId();
    UpdateMouseHook();
    while (!stopping) {
        const DWORD wait = MsgWaitForMultipleObjects(1, &stopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;
        while (!stopping && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == kReconfigure) UpdateMouseHook();
            else if (message.message == kBeginResize) {
                if (resizeTimer) KillTimer(nullptr, resizeTimer);
                resizeTimer = SetTimer(nullptr, 0, USER_TIMER_MINIMUM, nullptr);
                if (!resizeTimer) pendingResize = {};
            } else if (message.message == WM_TIMER && resizeTimer &&
                       message.wParam == resizeTimer) BeginPendingResize();
            else DispatchMessageW(&message);
        }
    }
    if (mouseHook) { UnhookWindowsHookEx(mouseHook); mouseHook = nullptr; }
    CancelPendingResize();
    inputThreadId = 0;
    if (priorDpi) SetThreadDpiAwarenessContext(priorDpi);
    return 0;
}

void ConfigureInput() {
    activeGripSize = gripSize;
    if (!gripSize) ClearResizeTarget(nullptr);
    const DWORD thread = inputThreadId.load();
    if (thread) PostThreadMessageW(thread, kReconfigure, 0, 0);
}

bool EarlyDwmCandidate(HWND window) {
    DWORD pid = 0;
    return !stopping && earlyDwmEnabled &&
        GetWindowThreadProcessId(window,&pid) == GetCurrentThreadId() &&
        pid == GetCurrentProcessId() &&
        CustomMainWindow(window,localFrameGeometry.load(),true);
}

HANDLE ReserveEarlyDwm(HWND window) {
    std::lock_guard lock(earlyDwmMutex);
    if (stopping || !earlyDwmEnabled) return nullptr;
    auto found = earlyDwmWindows.find(window);
    if (found != earlyDwmWindows.end() &&
        GetPropW(window,kEarlyDwmIdentity) != found->second.identity) {
        earlyDwmWindows.erase(found);
        found = earlyDwmWindows.end();
    }
    if (found == earlyDwmWindows.end()) {
        if (earlyDwmWindows.size() >= 64 || GetPropW(window,kEarlyDwmIdentity)) return nullptr;
        if (!nextEarlyDwmIdentity) nextEarlyDwmIdentity = uintptr_t(GetTickCount64()) << 16;
        const HANDLE identity = reinterpret_cast<HANDLE>(++nextEarlyDwmIdentity);
        found = earlyDwmWindows.emplace(window,EarlyDwmState{}).first;
        found->second.identity = identity;
        if (!SetPropW(window,kEarlyDwmIdentity,identity)) {
            earlyDwmWindows.erase(found);
            return nullptr;
        }
    }
    if (found->second.clearRequested) return nullptr;
    ++found->second.activeCalls;
    return found->second.identity;
}

void ReleaseEarlyDwm(HWND target, const EarlyDwmState& state, bool restore) {
    DWORD pid=0;
    if (GetPropW(target,kEarlyDwmIdentity)!=state.identity ||
        !GetWindowThreadProcessId(target,&pid) || pid!=GetCurrentProcessId()) return;
    if (restore && !state.destroyed && state.changed && earlyCompositionOriginal) {
        BOOL desired=state.desiredExile;
        CompositionData data{kNcRenderingExiled,&desired,sizeof(desired)};
        const bool prior=inEarlyDwm;
        inEarlyDwm=true;
        earlyCompositionOriginal(target,&data);
        inEarlyDwm=prior;
    }
    for (const PCWSTR name : {L"Identity",L"Requests",L"Intercepted",
            L"Prepared",L"DesiredExile",L"Changed"}) {
        const std::wstring property=std::wstring(kEarlyDwmPrefix)+name;
        RemovePropW(target,property.c_str());
    }
}

void RecordEarlyDwm(HWND window, HANDLE identity, BOOL desired,
                    bool changed, bool prepared, bool succeeded) {
    EarlyDwmState detached{};
    bool release=false;
    {
        std::lock_guard lock(earlyDwmMutex);
        const auto found=earlyDwmWindows.find(window);
        if (found==earlyDwmWindows.end() || found->second.identity!=identity) return;
        auto& state = found->second;
        if (succeeded && GetPropW(window,kEarlyDwmIdentity)==identity && !state.destroyed) {
            state.desiredExile = desired;
            state.changed = changed;
            if (prepared) ++state.prepared;
            else { ++state.requests; state.intercepted += changed; }
            for (const auto& [name,value] : std::array<std::pair<PCWSTR,uintptr_t>,5>{{
                {L"Requests",state.requests}, {L"Intercepted",state.intercepted},
                {L"Prepared",state.prepared}, {L"DesiredExile",uintptr_t(state.desiredExile)},
                {L"Changed",uintptr_t(state.changed)}}}) {
                const std::wstring property = std::wstring(kEarlyDwmPrefix)+name;
                SetPropW(window,property.c_str(),reinterpret_cast<HANDLE>(value));
            }
        }
        if (state.activeCalls) --state.activeCalls;
        if (!state.activeCalls && (state.clearRequested || (!state.requests && !state.prepared))) {
            detached=state;
            earlyDwmWindows.erase(found);
            release=true;
        }
    }
    // A settings/unload callback can overlap the real call. Its reservation
    // survives until this commit, so the last in-flight change is restored too.
    if (release) ReleaseEarlyDwm(window,detached,detached.restoreRequested);
}

void ClearEarlyDwm(HWND window = nullptr, bool restore = true) {
    std::vector<std::pair<HWND,EarlyDwmState>> detached;
    {
        std::lock_guard lock(earlyDwmMutex);
        for (auto it=earlyDwmWindows.begin();it!=earlyDwmWindows.end();) {
            if (!window || window==it->first) {
                if (it->second.activeCalls) {
                    auto& state=it->second;
                    state.clearRequested=true;
                    state.restoreRequested |= restore;
                    state.destroyed |= !restore;
                    ++it;
                    continue;
                }
                detached.push_back(*it);
                it=earlyDwmWindows.erase(it);
            } else ++it;
        }
    }
    // Composition can dispatch NC notifications. Never call it under the map
    // mutex, never retain a subclass/callback or a pointer in a window property.
    for (const auto& [target,state] : detached) {
        ReleaseEarlyDwm(target,state,restore);
    }
}

BOOL WINAPI EarlyComposition(HWND window, CompositionData* data) {
    const DWORD savedError=GetLastError();
    if (inEarlyDwm || !EarlyDwmCandidate(window)) {
        SetLastError(savedError);
        return earlyCompositionOriginal(window,data);
    }
    CompositionData snapshot{};
    BOOL desired=FALSE;
    SIZE_T read=0;
    if (!ReadProcessMemory(GetCurrentProcess(),data,&snapshot,sizeof(snapshot),&read) ||
        read!=sizeof(snapshot) || snapshot.attribute!=kNcRenderingExiled ||
        !snapshot.value || snapshot.size!=sizeof(desired) ||
        !ReadProcessMemory(GetCurrentProcess(),snapshot.value,&desired,sizeof(desired),&read) ||
        read!=sizeof(desired)) {
        SetLastError(savedError);
        return earlyCompositionOriginal(window,data);
    }
    BOOL enabled=FALSE;
    CompositionData replacement{kNcRenderingExiled,&enabled,sizeof(enabled)};
    const HANDLE identity=ReserveEarlyDwm(window);
    if (!identity) {
        SetLastError(savedError);
        return earlyCompositionOriginal(window,data);
    }
    inEarlyDwm=true;
    SetLastError(savedError);
    const BOOL result=earlyCompositionOriginal(window,desired ? &replacement : data);
    const DWORD error=GetLastError();
    inEarlyDwm=false;
    RecordEarlyDwm(window,identity,desired,desired!=FALSE,false,result!=FALSE);
    SetLastError(error);
    return result;
}

HRESULT WINAPI EarlyMargins(HWND window,const MARGINS* margins) {
    const DWORD savedError=GetLastError();
    MARGINS value{};
    SIZE_T read=0;
    if (!inEarlyDwm && EarlyDwmCandidate(window) && margins &&
        ReadProcessMemory(GetCurrentProcess(),margins,&value,sizeof(value),&read) &&
        read==sizeof(value) && value.cxLeftWidth>=0 && value.cxRightWidth>=0 &&
        value.cyBottomHeight>=0 && value.cyTopHeight>0 && value.cyTopHeight<=96) {
        BOOL nc=TRUE;
        if (SUCCEEDED(DwmGetWindowAttribute(window,DWMWA_NCRENDERING_ENABLED,&nc,sizeof(nc))) && !nc) {
            // Save the effective NC state for rollback, then establish DWM
            // before the real application's initialization call, not after.
            BOOL exile=FALSE;
            CompositionData data{kNcRenderingExiled,&exile,sizeof(exile)};
            const HANDLE identity=ReserveEarlyDwm(window);
            if (!identity) {
                SetLastError(savedError);
                return earlyMarginsOriginal(window,margins);
            }
            inEarlyDwm=true;
            const BOOL changed=earlyCompositionOriginal(window,&data);
            inEarlyDwm=false;
            RecordEarlyDwm(window,identity,TRUE,true,true,changed!=FALSE);
        }
    }
    SetLastError(savedError);
    return earlyMarginsOriginal(window,margins); // exactly one real app call
}

bool DelegateDwmCaption(HWND window, UINT message, WPARAM wp, LPARAM lp, LRESULT& result) {
    if (stopping || !dwmCaptionEnabled || earlyDwmEnabled || inDwmCaptionPaint ||
        GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId()) return false;
    struct Scope {
        Scope() { inDwmCaptionPaint = true; }
        ~Scope() { inDwmCaptionPaint = false; }
    } scope;
    // Public DWM message handler only. Never run GDI default paint to make
    // glyphs appear; when DWM declines, the normal legacy filter remains.
    const BOOL handled = DwmDefWindowProc(window, message, wp, lp, &result);
    return handled != FALSE;
}

void PaintLegacyGutter(HWND window) {
    const COLORREF color = legacyGutterColor.load();
    if (color == kNoBorder || stopping || !legacyPaintEnabled || paintingLegacyGutter) return;
    struct ErrorScope { DWORD error=GetLastError(); ~ErrorScope() { SetLastError(error); } } error;
    DWORD pid=0;
    if (!GetWindowThreadProcessId(window,&pid) || pid != GetCurrentProcessId() ||
        !CustomMainWindow(window,localFrameGeometry.load())) return;
    // Use the app's existing coordinate context for its own window DC. Never
    // mix physical WINDOWINFO coordinates with a potentially virtualized DC.
    RECT outer{}, client{};
    POINT first{}, last{};
    if (!GetWindowRect(window,&outer) || !GetClientRect(window,&client)) return;
    first={client.left,client.top}; last={client.right,client.bottom};
    if (!ClientToScreen(window,&first) || !ClientToScreen(window,&last)) return;
    client={first.x,first.y,last.x,last.y};
    const UINT dpi=GetDpiForWindow(window);
    const int limit=MulDiv(12,dpi ? dpi : USER_DEFAULT_SCREEN_DPI,USER_DEFAULT_SCREEN_DPI);
    std::array<RECT,4> strips{};
    RECT localClient{};
    if (!LegacyGutterRects(outer,client,limit,strips,localClient)) return;
    if (std::none_of(strips.begin(),strips.end(),[](const RECT& r) {
            return r.right > r.left && r.bottom > r.top; })) return;
    struct PaintScope {
        PaintScope() { paintingLegacyGutter=true; }
        ~PaintScope() { paintingLegacyGutter=false; }
    } paint;
    HDC dc=GetWindowDC(window);
    if (!dc) return;
    HBRUSH brush=CreateSolidBrush(color);
    if (brush) {
        FillLegacyGutter(dc,brush,strips,localClient);
        DeleteObject(brush);
    }
    ReleaseDC(window,dc);
}

// No general-purpose subclass or geometry hooks. In particular,
// retaining WS_VISIBLE avoids dropping Photoshop from taskbar replacements.
LRESULT HandleLegacyPaint(DefaultProc original, HWND window, UINT message,
                          WPARAM wp, LPARAM lp) {
    if (message == WM_NCDESTROY) {
        if (GetPropW(window,kEarlyDwmIdentity)) ClearEarlyDwm(window,false);
        return original(window, message, wp, lp);
    }
    switch (message) {
        case WM_NCPAINT:
        case WM_NCACTIVATE:
        case 0x00AE: // WM_NCUAHDRAWCAPTION
        case 0x00AF: // WM_NCUAHDRAWFRAME
        case WM_SETTEXT:
        case WM_SETICON:
            break;
        default: {
            const LRESULT result=original(window,message,wp,lp);
            if (message == WM_WINDOWPOSCHANGED || message == WM_SYNCPAINT ||
                message == WM_DWMNCRENDERINGCHANGED || message == WM_DWMCOMPOSITIONCHANGED)
                PaintLegacyGutter(window);
            return result;
        }
    }
    DWORD pid = 0;
    if (!legacyPaintEnabled || !GetWindowThreadProcessId(window, &pid) ||
        pid != GetCurrentProcessId() || !CustomMainWindow(window, localFrameGeometry.load()))
        return original(window, message, wp, lp);
    if (message == WM_NCPAINT || message == WM_NCACTIVATE) {
        LRESULT dwmResult = 0;
        const bool handled = DelegateDwmCaption(window, message, wp, lp, dwmResult);
        if (message == WM_NCPAINT) {
            PaintLegacyGutter(window);
            return handled ? dwmResult : 0;
        }
        // Preserve activation bookkeeping without allowing classic GDI paint,
        // regardless of whether the independent DWM handler accepts it.
        const LRESULT result = original(window, message, wp, -1);
        PaintLegacyGutter(window);
        return handled ? dwmResult : result;
    }
    if (message == WM_SETTEXT || message == WM_SETICON) {
        const LRESULT result = original(window, message, wp, lp);
        // The title/icon must remain correct for the shell. Request a client
        // repaint without ever toggling WS_VISIBLE, CAPTION or THICKFRAME.
        RedrawWindow(window, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN);
        PaintLegacyGutter(window);
        return result;
    }
    PaintLegacyGutter(window);
    return 0;
}

LRESULT WINAPI LegacyDefaultW(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    return HandleLegacyPaint(defaultProcW, window, message, wp, lp);
}
LRESULT WINAPI LegacyDefaultA(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    return HandleLegacyPaint(defaultProcA, window, message, wp, lp);
}

BOOL CALLBACK RefreshLegacyWindow(HWND window, LPARAM) {
    DWORD pid = 0;
    if (GetWindowThreadProcessId(window, &pid) && pid == GetCurrentProcessId() &&
        CustomMainWindow(window, localFrameGeometry.load()))
        RedrawWindow(window, nullptr, nullptr,
            RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_FRAME);
    return TRUE;
}

void RefreshLegacySettings() {
    {
        std::lock_guard lock(settingsMutex);
        legacyPaintEnabled = MatchList(currentPath.c_str(), configuredLegacyPrograms);
        localFrameGeometry = MatchGeometry(currentPath.c_str(), configuredGeometries);
        legacyGutterColor = legacyPaintEnabled && localFrameGeometry != FrameGeometry::Disabled
            ? MatchGutter(currentPath.c_str()) : kNoBorder;
        dwmCaptionEnabled = legacyPaintEnabled &&
            MatchList(currentPath.c_str(), configuredDwmCaptionPrograms);
        earlyDwmEnabled = legacyPaintEnabled && localFrameGeometry != FrameGeometry::Disabled &&
            MatchList(currentPath.c_str(),configuredEarlyDwmPrograms);
        nativeGripSize = localFrameGeometry != FrameGeometry::Disabled &&
            MatchList(currentPath.c_str(), configuredNativeResizePrograms)
            ? configuredGripSize : 0;
    }
    if (!earlyDwmEnabled) ClearEarlyDwm();
    { std::lock_guard lock(nativeFrameMutex); nativeFrame = {}; }
    EnumWindows(ClearNativeAck, 0);
    if (defaultProcW) EnumWindows(RefreshLegacyWindow, 0);
}

bool WriteExile(HWND window, BOOL exile) {
    CompositionData data{kNcRenderingExiled, &exile, sizeof(exile)};
    return setComposition(window, &data);
}

// ROLLBACK_PACKET_BEGIN
constexpr std::array<PCWSTR,13> kRollbackProperties{
    L"Windhawk.AppOwnedFrame.Rollback.1.Valid",
    L"Windhawk.AppOwnedFrame.Rollback.1.Pid",
    L"Windhawk.AppOwnedFrame.Rollback.1.CreatedLow",
    L"Windhawk.AppOwnedFrame.Rollback.1.CreatedHigh",
    L"Windhawk.AppOwnedFrame.Rollback.1.HostPid",
    L"Windhawk.AppOwnedFrame.Rollback.1.HostLow",
    L"Windhawk.AppOwnedFrame.Rollback.1.HostHigh",
    L"Windhawk.AppOwnedFrame.Rollback.1.Nc",
    L"Windhawk.AppOwnedFrame.Rollback.1.Border",
    L"Windhawk.AppOwnedFrame.Rollback.1.Backdrop",
    L"Windhawk.AppOwnedFrame.Rollback.1.Caption",
    L"Windhawk.AppOwnedFrame.Rollback.1.Readable",
    L"Windhawk.AppOwnedFrame.Rollback.1.Changed"
};
FILETIME controllerCreated{};

DWORD PacketValue(HWND window,size_t field) {
    return static_cast<DWORD>(reinterpret_cast<uintptr_t>(GetPropW(window,kRollbackProperties[field])));
}

bool SetPacketValue(HWND window,size_t field,DWORD value) {
    return SetPropW(window,kRollbackProperties[field],reinterpret_cast<HANDLE>(uintptr_t(value)));
}

void ClearRollbackPacket(HWND window) {
    RemovePropW(window,kRollbackProperties[0]);
    for (size_t i=1; i<kRollbackProperties.size(); ++i)
        RemovePropW(window,kRollbackProperties[i]);
}

enum class PacketRecovery { None, Recovered, OtherController };
PacketRecovery RecoverRollbackPacket(HWND window,WindowState& state) {
    if (PacketValue(window,0) != 1)
        return GetPropW(window,kIdentityProperty) ? PacketRecovery::OtherController : PacketRecovery::None;
    if (PacketValue(window,1) != state.process.pid ||
        PacketValue(window,2) != state.process.created.dwLowDateTime ||
        PacketValue(window,3) != state.process.created.dwHighDateTime)
        return PacketRecovery::OtherController;
    const HANDLE host = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                                    FALSE,PacketValue(window,4));
    if (host) {
        FILETIME created{},exited{},kernel{},user{};
        const DWORD wait = WaitForSingleObject(host,0);
        const bool times = GetProcessTimes(host,&created,&exited,&kernel,&user);
        const bool alive = wait != WAIT_OBJECT_0 &&
            (!times || (created.dwLowDateTime == PacketValue(window,5) &&
                        created.dwHighDateTime == PacketValue(window,6)));
        CloseHandle(host);
        if (alive) return PacketRecovery::OtherController;
    } else if (GetLastError() != ERROR_INVALID_PARAMETER)
        return PacketRecovery::OtherController; // Access denied isn't proof of death.
    state.originalNcRendering = PacketValue(window,7) != 0;
    state.originalBorder = PacketValue(window,8);
    state.originalBackdrop = PacketValue(window,9);
    state.captionFill.original = PacketValue(window,10);
    const DWORD readable = PacketValue(window,11), changed = PacketValue(window,12);
    state.borderReadable = (readable & 1) != 0;
    state.backdropReadable = (readable & 2) != 0;
    state.captionFill.readable = (readable & 4) != 0;
    state.captionFill.captured = true;
    state.compositionChanged = (changed & 1) != 0;
    state.borderChanged = (changed & 2) != 0;
    state.backdropChanged = (changed & 4) != 0;
    state.captionFill.changed = (changed & 8) != 0;
    return PacketRecovery::Recovered;
}

bool SaveRollbackPacket(HWND window,const WindowState& state,
                        PacketRecovery recovery = PacketRecovery::None) {
    if (recovery == PacketRecovery::Recovered) {
        // The original attributes and valid marker remain intact throughout
        // adoption. Commit the new host PID after its creation time. A failed
        // claim still leaves the old rollback values available for a retry.
        return SetPacketValue(window,5,controllerCreated.dwLowDateTime) &&
            SetPacketValue(window,6,controllerCreated.dwHighDateTime) &&
            SetPacketValue(window,4,GetCurrentProcessId());
    }
    // Commit the marker last. A partial property packet is never adopted.
    RemovePropW(window,kRollbackProperties[0]);
    const std::array<DWORD,12> values{
        state.process.pid,state.process.created.dwLowDateTime,state.process.created.dwHighDateTime,
        GetCurrentProcessId(),controllerCreated.dwLowDateTime,controllerCreated.dwHighDateTime,
        DWORD(state.originalNcRendering),state.originalBorder,state.originalBackdrop,
        state.captionFill.original,
        DWORD(state.borderReadable) | (DWORD(state.backdropReadable)<<1) |
            (DWORD(state.captionFill.readable)<<2),
        DWORD(state.compositionChanged) | (DWORD(state.borderChanged)<<1) |
            (DWORD(state.backdropChanged)<<2) | (DWORD(state.captionFill.changed)<<3)
    };
    for (size_t i=0;i<values.size();++i)
        if (!SetPacketValue(window,i+1,values[i])) {
            ClearRollbackPacket(window);
            return false;
        }
    if (SetPacketValue(window,0,1)) return true;
    ClearRollbackPacket(window);
    return false;
}

bool RecordRollbackIntent(HWND window,const WindowState& state,DWORD attribute) {
    if (PacketValue(window,0) != 1 || PacketValue(window,4) != GetCurrentProcessId() ||
        PacketValue(window,5) != controllerCreated.dwLowDateTime ||
        PacketValue(window,6) != controllerCreated.dwHighDateTime ||
        GetPropW(window,kIdentityProperty) != state.identity) return false;
    const DWORD changed = PacketValue(window,12);
    return (changed & attribute) == attribute || SetPacketValue(window,12,changed | attribute);
}
// ROLLBACK_PACKET_END

bool Restore(HWND window, WindowState& state) {
    UpdateScope update;
    ClearResizeTarget(window);
    if (!SameWindow(window, state)) return true;
    state.retiring = true;
    // An intent is written before the compositor call. Even if the helper
    // dies before recording success, recovery still restores that attribute.
    const DWORD changed = PacketValue(window,12);
    bool restored = true;
    if (state.borderChanged || (changed & 2)) {
        const COLORREF color = state.borderReadable ? state.originalBorder : kDefaultBorder;
        restored &= SUCCEEDED(DwmSetWindowAttribute(window,kBorderColor,&color,sizeof(color)));
    }
    if ((state.backdropChanged || (changed & 4)) && state.backdropReadable)
        restored &= SUCCEEDED(DwmSetWindowAttribute(window,kSystemBackdrop,
            &state.originalBackdrop,sizeof(state.originalBackdrop)));
    if (changed & 8) {
        state.captionFill.readable = (PacketValue(window,11) & 4) != 0;
        state.captionFill.original = state.captionFill.readable
            ? PacketValue(window,10) : kDefaultBorder;
        state.captionFill.captured = state.captionFill.changed = true;
    }
    restored &= ReconcileCaptionFill(window,state.captionFill,kNoBorder);
    if (state.compositionChanged || (changed & 1))
        restored &= WriteExile(window,!state.originalNcRendering);
    if (!restored) return false; // Retain the packet for the next recovery/retry.
    ClearRollbackPacket(window);
    RemovePropW(window, kIdentityProperty);
    RemovePropW(window, kResizeProperty);
    RemovePropW(window, kNativeResizeProperty);
    RemovePropW(window, kNativePressProperty);
    return true;
}

// CAPTURE_WINDOW_BEGIN
bool CaptureWindow(HWND window,const ProcessInfo& process,BOOL enabled) {
    WindowState initial{process, enabled != FALSE};
    const PacketRecovery recovered = RecoverRollbackPacket(window,initial);
    if (recovered == PacketRecovery::OtherController) return false;
    if (recovered == PacketRecovery::None) {
        initial.borderReadable = SUCCEEDED(DwmGetWindowAttribute(
            window,kBorderColor,&initial.originalBorder,sizeof(COLORREF)));
        initial.backdropReadable = SUCCEEDED(DwmGetWindowAttribute(
            window,kSystemBackdrop,&initial.originalBackdrop,sizeof(DWORD)));
        initial.captionFill.readable = SUCCEEDED(DwmGetWindowAttribute(
            window,DWMWA_CAPTION_COLOR,&initial.captionFill.original,sizeof(COLORREF)));
            if (!initial.captionFill.readable) initial.captionFill.original = kDefaultBorder;
        initial.captionFill.captured = true;
    }
    // The same scalar token is seen from x86 tool and x64 app processes.
    const auto token = static_cast<uintptr_t>(
        (uint64_t(GetCurrentProcessId()) << 32) | static_cast<DWORD>(++nextIdentity));
    const HANDLE identity = reinterpret_cast<HANDLE>(token ? token : ++nextIdentity);
    initial.identity = identity;
    auto found = windows.emplace(window,initial).first;
    if (!SetPropW(window,kIdentityProperty,identity) ||
        !SaveRollbackPacket(window,initial,recovered)) {
        if (GetPropW(window,kIdentityProperty) == identity)
            RemovePropW(window,kIdentityProperty);
        windows.erase(found);
        return false;
    }
    Wh_Log(L"Tracking custom frame hwnd=%p pid=%lu originalNC=%d", window, process.pid, initial.originalNcRendering);
    locationHooksDirty = true;
    return true;
}
// CAPTURE_WINDOW_END

void Apply(HWND window, bool forceBorder = false) {
    // Out-of-context callbacks may reenter while a compositor call is being
    // serviced. Let the next queued event/poll reconcile instead of touching
    // the same map entry recursively.
    if (stopping || !window || updatingWindow || sizingWindows.contains(window)) return;
    UpdateScope update;
    auto found = windows.find(window);
    if (found != windows.end() && !SameWindow(window, found->second)) {
        ClearResizeTarget(window);
        windows.erase(found);
        locationHooksDirty = true;
        found = windows.end();
    }
    if (found == windows.end() && PacketValue(window,0) == 1) {
        // Recover orphaned rollback data even when this app was excluded
        // while the previous helper was down. Hidden/iconic windows count too.
        DWORD pid = 0;
        GetWindowThreadProcessId(window,&pid);
        ProcessInfo process{};
        if (!InspectProcess(pid,&process) || !CaptureWindow(window,process,FALSE)) return;
        found = windows.find(window);
        found->second.retiring = !process.eligible;
    }
    if (found != windows.end() && found->second.retiring) {
        if (Restore(window,found->second)) {
            windows.erase(found);
            locationHooksDirty = true;
        }
        return;
    }
    // Iconic and temporarily hidden windows retain their record for rollback.
    if (!IsWindowVisible(window) || IsIconic(window)) {
        if (found != windows.end()) ClearResizeTarget(window);
        return;
    }
    if (!CustomMainWindow(window, FrameGeometry::ResizeGutter) ||
        (found != windows.end() && !CustomMainWindow(window, found->second.process.geometry))) {
        if (found != windows.end()) {
            if (Restore(window,found->second)) {
                windows.erase(found);
                locationHooksDirty = true;
            }
        }
        return;
    }
    if (found == windows.end()) {
        DWORD pid = 0;
        GetWindowThreadProcessId(window, &pid);
        ProcessInfo process{};
        if (!InspectProcess(pid, &process) || !process.eligible ||
            !CustomMainWindow(window, process.geometry)) return;
        BOOL enabled = FALSE;
        if (FAILED(DwmGetWindowAttribute(window, DWMWA_NCRENDERING_ENABLED,
                                         &enabled, sizeof(enabled)))) return;
        if (!CaptureWindow(window,process,enabled)) return;
        found = windows.find(window);
        forceBorder = true;
    }
    auto& state = found->second;
    if (state.process.removeBackdrop && state.backdropReadable) {
        DWORD backdrop = 0;
        if (SUCCEEDED(DwmGetWindowAttribute(window, kSystemBackdrop, &backdrop, sizeof(backdrop))) &&
            backdrop != 1) {
            const DWORD none = 1; // DWMSBT_NONE: no system material behind NC.
            if (RecordRollbackIntent(window,state,4) &&
                SUCCEEDED(DwmSetWindowAttribute(window, kSystemBackdrop, &none, sizeof(none))))
                state.backdropChanged = true;
        }
    }
    BOOL enabled = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_NCRENDERING_ENABLED,
                                        &enabled, sizeof(enabled))) && !enabled) {
        if (RecordRollbackIntent(window,state,1) && WriteExile(window, FALSE)) {
            state.compositionChanged = true;
            forceBorder = true;
            Wh_Log(L"Restored compositor clipping hwnd=%p pid=%lu", window, state.process.pid);
        }
    }
    const ULONGLONG now = GetTickCount64();
    COLORREF currentBorder = 0;
    const bool borderReadable = SUCCEEDED(DwmGetWindowAttribute(
        window,kBorderColor,&currentBorder,sizeof(currentBorder)));
    // Some Windows builds don't expose these colors to Get. Retain a limited
    // foreground-only reconciliation for them; never force every discovery.
    const bool unreadableCheck = now >= state.nextUnreadableColorCheck &&
        GetForegroundWindow() == window;
    if (forceBorder || !state.borderChanged ||
        (borderReadable ? currentBorder != kNoBorder : unreadableCheck)) {
        const COLORREF color = kNoBorder;
        if (RecordRollbackIntent(window,state,2) &&
            SUCCEEDED(DwmSetWindowAttribute(window, kBorderColor, &color, sizeof(color))))
            state.borderChanged = true;
    }
    // Only the opt-in color of a validated custom-frame root. Never from
    // hit testing, the LL input callback, or an active native resize loop.
    bool forceCaption = forceBorder;
    if (state.process.gutterColor != kNoBorder) {
        COLORREF current = 0;
        const bool readable = SUCCEEDED(DwmGetWindowAttribute(
            window,DWMWA_CAPTION_COLOR,&current,sizeof(current)));
        forceCaption |= readable ? current != state.process.gutterColor : unreadableCheck;
    }
    if (state.process.gutterColor == kNoBorder || RecordRollbackIntent(window,state,8))
        ReconcileCaptionFill(window,state.captionFill,state.process.gutterColor,forceCaption);
    if (unreadableCheck || forceBorder) state.nextUnreadableColorCheck = now + 2000;
    RefreshResizeTarget(window, state);
}

BOOL CALLBACK Enumerate(HWND window, LPARAM) { Apply(window); return TRUE; }

void CALLBACK WindowEvent(HWINEVENTHOOK, DWORD event, HWND window, LONG object,
                          LONG child, DWORD, DWORD) {
    if (stopping || !window) return;
    // No DWM calls, process/token queries or helper repositioning from this
    // callback. Location events arrive for every drag step and can reenter.
    // Coalesce them; most importantly, don't touch the app during its native
    // move/size loop. The compositor state established before it stays valid.
    const bool tracked = windows.contains(window);
    if (event == EVENT_SYSTEM_MOVESIZESTART && tracked) {
        sizingWindows.insert(window);
        ClearResizeTarget(window);
        sizingChanged = true;
        return;
    }
    if (event == EVENT_SYSTEM_MOVESIZEEND && tracked) {
        sizingWindows.erase(window);
    } else if (event == EVENT_SYSTEM_FOREGROUND) {
        ClearResizeTarget(nullptr);
        foregroundChanged = true;
    } else if (object == OBJID_WINDOW && child == CHILDID_SELF) {
        if (event == EVENT_OBJECT_DESTROY) sizingWindows.erase(window);
        if (!tracked && event != EVENT_OBJECT_CREATE && event != EVENT_OBJECT_SHOW) return;
        if (!tracked && GetAncestor(window,GA_ROOT) != window) return;
    } else return;
    if (pendingWindows.size() >= 4096 && !pendingWindows.contains(window)) return;
    if (pendingWindows.empty()) pendingDue = GetTickCount64() + 100;
    pendingWindows[window] = pendingWindows[window] || event == EVENT_OBJECT_SHOW ||
        event == EVENT_SYSTEM_FOREGROUND || event == EVENT_SYSTEM_MOVESIZEEND;
}

// LOCATION_HOOKS_BEGIN
void SyncLocationHooks() {
    const ULONGLONG now = GetTickCount64();
    if (!locationHooksDirty && now < locationHooksRetryAfter) return;
    locationHooksDirty = false;
    bool retry = false;
    std::unordered_map<DWORD,FILETIME> wanted;
    for (const auto& [window,state] : windows) {
        if (!state.process.pid) continue; // Zero would subscribe globally.
        auto [found,inserted] = wanted.emplace(state.process.pid,state.process.created);
        if (!inserted && CompareFileTime(&state.process.created,&found->second) > 0)
            found->second = state.process.created;
    }
    // Runs only on the controller's message-pump thread, never in a WinEvent
    // callback. Two windows in one process share a hook. A reused PID must
    // retire the old generation before the new process gets a subscription.
    for (auto it=locationHooks.begin();it!=locationHooks.end();) {
        const auto current = wanted.find(it->first);
        if (current == wanted.end() ||
            CompareFileTime(&current->second,&it->second.created) != 0) {
            if (UnhookWinEvent(it->second.hook)) it=locationHooks.erase(it);
            else { retry = true; ++it; }
        } else ++it;
    }
    for (const auto& [pid,created] : wanted) {
        if (locationHooks.contains(pid)) continue;
        const HWINEVENTHOOK hook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE,
            EVENT_OBJECT_LOCATIONCHANGE,nullptr,WindowEvent,pid,0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        if (hook) locationHooks.emplace(pid,LocationHookState{hook,created});
        else retry = true; // Tracked polling remains a safe fallback.
    }
    // Failed registrations/removals are retried without spinning on each
    // unrelated event. A later track/untrack transition bypasses this delay.
    locationHooksRetryAfter = retry ? now + 1000 : ULLONG_MAX;
}

void ClearLocationHooks() {
    for (const auto& [pid,state] : locationHooks) UnhookWinEvent(state.hook);
    locationHooks.clear();
}
// LOCATION_HOOKS_END

void ReconcileEvents() {
    UpdateScope update;
    // Snapshot handles before calling Win32: out-of-context delivery can
    // occur while an API pumps messages, but never invalidate these loops.
    if (sizingChanged) {
        sizingChanged = false;
        const std::vector<HWND> sizing(sizingWindows.begin(), sizingWindows.end());
        for (HWND target : sizing) {
            ClearResizeTarget(target);
        }
    }
    if (foregroundChanged) {
        foregroundChanged = false;
        ClearResizeTarget(nullptr);
    }
}

void Reconfigure() {
    ClearProcessCache(); // Paths/settings/eligibility are one cache generation.
    {
        std::lock_guard lock(settingsMutex);
        programs = configuredPrograms;
        geometries = configuredGeometries;
        backdropPrograms = configuredBackdropPrograms;
        legacyPrograms = configuredLegacyPrograms;
        gutterPrograms = configuredGutters;
        gripSize = configuredGripSize;
    }
    ConfigureInput();
    // Keep unchanged targets corrected throughout a settings update; do not
    // briefly restore their white frame just to reapply the same setting.
    for (auto it = windows.begin(); it != windows.end();) {
        ProcessInfo process{};
        if (!SameWindow(it->first, it->second) ||
            !InspectProcess(it->second.process.pid, &process) || !process.eligible) {
            if (Restore(it->first,it->second)) {
                it=windows.erase(it);
                locationHooksDirty = true;
            }
            else ++it;
        } else {
            if (!process.removeBackdrop && it->second.backdropChanged && it->second.backdropReadable) {
                if (SUCCEEDED(DwmSetWindowAttribute(it->first,kSystemBackdrop,
                    &it->second.originalBackdrop,sizeof(DWORD))))
                    it->second.backdropChanged = false;
            }
            it->second.process.removeBackdrop = process.removeBackdrop;
            it->second.process.geometry = process.geometry;
            it->second.process.gutterColor = process.gutterColor;
            ++it;
        }
    }
    EnumWindows(Enumerate, 0);
}

// CONTROLLER_WAIT_BEGIN
DWORD ControllerWait(ULONGLONG now, ULONGLONG discovery, ULONGLONG poll,
                     ULONGLONG pending, bool tracked, bool queued,
                     ULONGLONG hookRetry = ULLONG_MAX) {
    ULONGLONG next = std::min(discovery,hookRetry);
    if (tracked) next = std::min(next,poll);
    if (queued) next = std::min(next,pending);
    return next <= now ? 0 : static_cast<DWORD>(std::min<ULONGLONG>(next-now,15000));
}
// CONTROLLER_WAIT_END

DWORD WINAPI Worker(void*) {
    // The official tool host's process mutex isolates this controller from
    // Explorer and excludes a second active controller in the same session.
    const auto priorDpi = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    MSG message{};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    workerId = GetCurrentThreadId();
    {
        std::lock_guard lock(settingsMutex);
        programs = configuredPrograms;
        geometries = configuredGeometries;
        backdropPrograms = configuredBackdropPrograms;
        legacyPrograms = configuredLegacyPrograms;
        gutterPrograms = configuredGutters;
        gripSize = configuredGripSize;
    }
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<PCWSTR>(ResizeMouse), &modModule);
    ConfigureInput();
    // A separate input-only message pump is essential: compositor/token
    // queries in the controller must never hold up the global LL callback.
    inputThread = CreateThread(nullptr, 0, InputWorker, nullptr, 0, nullptr);
    if (!inputThread) Wh_Log(L"Input thread creation failed: %lu", GetLastError());
    constexpr std::array<DWORD,4> events{EVENT_OBJECT_CREATE,EVENT_OBJECT_DESTROY,
        EVENT_OBJECT_SHOW,EVENT_OBJECT_HIDE};
    for (size_t i=0;i<events.size();++i)
        objectHooks[i] = SetWinEventHook(events[i],events[i],nullptr,WindowEvent,
            0,0,WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        nullptr, WindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    sizingHook = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
        nullptr, WindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!inputThread || !foregroundHook || !sizingHook ||
        std::any_of(objectHooks.begin(),objectHooks.end(),[](HWINEVENTHOOK hook) {return !hook;})) {
        Wh_Log(L"WinEvent registration failed: %lu", GetLastError());
        SetEvent(stopEvent);
        if (inputThread) WaitForSingleObject(inputThread, INFINITE);
        for (HWINEVENTHOOK hook : objectHooks) if (hook) UnhookWinEvent(hook);
        if (foregroundHook) UnhookWinEvent(foregroundHook);
        if (sizingHook) UnhookWinEvent(sizingHook);
        workerId = 0;
        SetEvent(readyEvent);
        if (priorDpi) SetThreadDpiAwarenessContext(priorDpi);
        return 1;
    }
    workerReady = true;
    SetEvent(readyEvent);
    EnumWindows(Enumerate, 0);
    SyncLocationHooks();
    ULONGLONG nextDiscovery = GetTickCount64() + 15000;
    ULONGLONG nextPoll = GetTickCount64() + 1000;
    while (!stopping) {
        const DWORD timeout = ControllerWait(GetTickCount64(),nextDiscovery,nextPoll,
            pendingDue,!windows.empty(),!pendingWindows.empty(),locationHooksRetryAfter);
        const DWORD wait = MsgWaitForMultipleObjects(1,&stopEvent,FALSE,timeout,QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;
        unsigned drained = 0;
        while (drained++ < 128 && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == kReconfigure) Reconfigure();
            else if (message.message == kResizeRequest) {
                const HWND target = reinterpret_cast<HWND>(message.wParam);
                auto found = windows.find(target);
                if (found != windows.end() &&
                    found->second.identity == reinterpret_cast<HANDLE>(message.lParam) &&
                    GetPropW(target, kIdentityProperty) == found->second.identity)
                    SetPropW(target, kResizeProperty,
                        reinterpret_cast<HANDLE>(++found->second.resizeRequests));
            }
            else DispatchMessageW(&message);
        }
        const ULONGLONG now = GetTickCount64();
        ReconcileEvents();
        if (!pendingWindows.empty() && now >= pendingDue) {
            // Detach the batch: callbacks can safely queue the next batch.
            auto pending = std::move(pendingWindows);
            pendingWindows.clear();
            for (const auto& [window, forceBorder] : pending) Apply(window, forceBorder);
        }
        if (now >= nextDiscovery) {
            PruneProcessCache();
            EnumWindows(Enumerate, 0);
            nextDiscovery = now + 15000;
            nextPoll = now + 1000;
        } else if (now >= nextPoll) {
            std::vector<HWND> tracked;
            for (const auto& [window, state] : windows) tracked.push_back(window);
            for (HWND window : tracked) Apply(window);
            nextPoll = now + 1000;
        }
        SyncLocationHooks();
    }
    ClearLocationHooks();
    for (HWINEVENTHOOK hook : objectHooks) UnhookWinEvent(hook);
    UnhookWinEvent(foregroundHook);
    UnhookWinEvent(sizingHook);
    for (auto& [window, state] : windows) Restore(window, state);
    windows.clear();
    ClearProcessCache();
    // Both threads are drained before the host exits or unloads its DLL.
    SetEvent(stopEvent);
    if (inputThread) WaitForSingleObject(inputThread, INFINITE);
    workerId = 0;
    if (priorDpi) SetThreadDpiAwarenessContext(priorDpi);
    return 0;
}
} // namespace

BOOL WhTool_ModInit() {
    DWORD session = 0;
    FILETIME exited{},kernel{},user{};
    if (!ProcessIdToSessionId(GetCurrentProcessId(),&session) || !session ||
        !ReadUserSid(GetCurrentProcess(),userSid) ||
        !GetProcessTimes(GetCurrentProcess(),&controllerCreated,&exited,&kernel,&user))
        return FALSE;
    setComposition = reinterpret_cast<SetComposition>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"),"SetWindowCompositionAttribute"));
    if (!setComposition) return FALSE;
    nextIdentity = static_cast<DWORD>(GetTickCount64()) ^ controllerCreated.dwLowDateTime;
    LoadSettings();
    stopEvent = CreateEventW(nullptr,TRUE,FALSE,nullptr);
    readyEvent = CreateEventW(nullptr,TRUE,FALSE,nullptr);
    if (!stopEvent || !readyEvent) return FALSE;
    workerThread = CreateThread(nullptr,0,Worker,nullptr,0,nullptr);
    if (!workerThread) return FALSE;
    if (WaitForSingleObject(readyEvent,5000) != WAIT_OBJECT_0 || !workerReady) {
        SetEvent(stopEvent);
        WaitForSingleObject(workerThread,INFINITE);
        return FALSE;
    }
    Wh_Log(L"Dedicated frame controller ready: pid=%lu thread=%lu",
        GetCurrentProcessId(),workerId.load());
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    const DWORD thread = workerId.load();
    if (thread) PostThreadMessageW(thread,kReconfigure,0,0);
}

void StopController() {
    stopping = true;
    if (stopEvent) SetEvent(stopEvent);
    if (workerThread) WaitForSingleObject(workerThread,INFINITE);
    if (inputThread) WaitForSingleObject(inputThread,INFINITE);
}

void WhTool_ModUninit() {
    StopController();
    for (HANDLE handle : {workerThread,inputThread,readyEvent,stopEvent})
        if (handle) CloseHandle(handle);
    workerThread = inputThread = readyEvent = stopEvent = nullptr;
}

// Official 1.7.3 compatibility implementation, scoped to retain app hooks.
// Intentional body difference: fail closed if entry-point hooking fails,
// shutting down the already-started controller before the host can proceed.
// Global WhTool_* names let Windhawk 2.0 recognize the legacy tool host.
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
namespace ToolHost {
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

        if (!Wh_SetFunctionHook(entryPoint,(void*)EntryPoint_Hook,nullptr)) {
            WhTool_ModUninit();
            ExitProcess(1);
        }
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
} // namespace ToolHost


BOOL Wh_ModInit() {
    using RtlVersion = LONG(WINAPI*)(OSVERSIONINFOW*);
    const auto rtlVersion = reinterpret_cast<RtlVersion>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion"));
    OSVERSIONINFOW version{sizeof(version)};
    if (!rtlVersion || rtlVersion(&version) != 0 || version.dwBuildNumber < 22000)
        return FALSE;
    wchar_t path[32768]{};
    const DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length == ARRAYSIZE(path)) return FALSE;
    currentPath = path;
    const wchar_t* basename = PathFindFileNameW(path);
    toolContext = _wcsicmp(basename,L"windhawk.exe") == 0 ||
        _wcsicmp(basename,L"windhawk-mod.exe") == 0;
    if (toolContext) return ToolHost::Wh_ModInit();
    if (_wcsicmp(basename,L"explorer.exe") == 0) return FALSE;
    LoadSettings();
    {
        legacyProcess = true;
        {
            std::lock_guard lock(settingsMutex);
            legacyPaintEnabled = MatchList(path, configuredLegacyPrograms);
            localFrameGeometry = MatchGeometry(path, configuredGeometries);
            legacyGutterColor = legacyPaintEnabled && localFrameGeometry != FrameGeometry::Disabled
                ? MatchGutter(path) : kNoBorder;
            dwmCaptionEnabled = legacyPaintEnabled &&
                MatchList(path, configuredDwmCaptionPrograms);
            earlyDwmEnabled = legacyPaintEnabled && localFrameGeometry != FrameGeometry::Disabled &&
                MatchList(path,configuredEarlyDwmPrograms);
            nativeResizeProcess = MatchList(path, configuredNativeResizePrograms);
        }
        if (!legacyPaintEnabled && !nativeResizeProcess) return FALSE;
        if (legacyPaintEnabled &&
            !(Wh_SetFunctionHook(reinterpret_cast<void*>(DefWindowProcW),
                    reinterpret_cast<void*>(LegacyDefaultW), reinterpret_cast<void**>(&defaultProcW)) &&
               Wh_SetFunctionHook(reinterpret_cast<void*>(DefWindowProcA),
                    reinterpret_cast<void*>(LegacyDefaultA), reinterpret_cast<void**>(&defaultProcA))))
            return FALSE;
        if (legacyPaintEnabled) {
            const auto composition=reinterpret_cast<SetComposition>(
                GetProcAddress(GetModuleHandleW(L"user32.dll"),"SetWindowCompositionAttribute"));
            if (!composition ||
                !Wh_SetFunctionHook(reinterpret_cast<void*>(composition),
                    reinterpret_cast<void*>(EarlyComposition),reinterpret_cast<void**>(&earlyCompositionOriginal)) ||
                !Wh_SetFunctionHook(reinterpret_cast<void*>(DwmExtendFrameIntoClientArea),
                    reinterpret_cast<void*>(EarlyMargins),reinterpret_cast<void**>(&earlyMarginsOriginal)))
                return FALSE;
            legacyHooksInstalled = true;
        }
        if (nativeResizeProcess) {
            // Existing classes are hooked during init. Registration handles
            // future launches once, never from a mouse/hit-test callback.
            if (!Wh_SetFunctionHook(reinterpret_cast<void*>(RegisterClassExW),
                    reinterpret_cast<void*>(NativeRegisterClass), reinterpret_cast<void**>(&registerClass)))
                return FALSE;
            EnumWindows(FindNativeClass, 0);
        }
        return TRUE;
    }
}

void Wh_ModAfterInit() {
    if (toolContext) { ToolHost::Wh_ModAfterInit(); return; }
    if (legacyProcess) { RefreshLegacySettings(); return; }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = FALSE;
    if (toolContext) { ToolHost::Wh_ModSettingsChanged(); return TRUE; }
    LoadSettings();
    if (legacyProcess) {
        bool legacyRequested, nativeRequested;
        {
            std::lock_guard lock(settingsMutex);
            legacyRequested = MatchList(currentPath.c_str(), configuredLegacyPrograms);
            nativeRequested = MatchList(currentPath.c_str(), configuredNativeResizePrograms);
        }
        // Changing flags alone can't install a missing hook. Let the engine
        // drain/remove the old hooks and rerun init with the requested set.
        if (legacyRequested != legacyHooksInstalled ||
            nativeRequested != nativeResizeProcess) {
            *bReload = TRUE;
            return TRUE;
        }
        RefreshLegacySettings();
        return TRUE;
    }
    const DWORD thread = workerId.load();
    if (thread) PostThreadMessageW(thread, kReconfigure, 0, 0);
    return TRUE;
}

void Wh_ModBeforeUninit() {
    if (toolContext) {
        if (!ToolHost::g_isToolModProcessLauncher) StopController();
        return;
    }
    stopping = true;
    if (legacyProcess) {
        earlyDwmEnabled = false;
        ClearEarlyDwm();
        dwmCaptionEnabled = false;
        legacyPaintEnabled = false;
        legacyGutterColor = kNoBorder;
        nativeGripSize = 0;
        // Serialize with a one-time class registration hook before the engine
        // removes function detours. No subclass pointers survive DLL unload.
        { std::lock_guard lock(nativeHookMutex); }
        EnumWindows(ClearNativeAck, 0);
        return;
    }
}

void Wh_ModUninit() {
    if (toolContext) { ToolHost::Wh_ModUninit(); return; }
    if (legacyProcess) {
        if (defaultProcW) EnumWindows(RefreshLegacyWindow, 0);
        return;
    }
}
