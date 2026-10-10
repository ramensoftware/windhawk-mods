// ==WindhawkMod==
// @id              app-owned-frame
// @name            App Owned Frame
// @description     Restores DWM clipping for selected custom-frame applications without changing their window styles or resizing.
// @version         1.42
// @author          appEW
// @github          https://github.com/appEW
// @license         MIT
// @include         explorer.exe
// @include         Discord.exe
// @include         Photoshop.exe
// @include         Resolve.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -ladvapi32 -lshlwapi -lgdi32 -lshcore
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

- Windows 11, build 22000 or later, with Explorer running.
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
- Explorer supplies the external frame correction. A clean mod disable restores
  the captured state, but border/caption colours which Windows cannot read back
  return to the system default. After an Explorer crash, original values held
  only by that process cannot be recovered.
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
    DWORD originalBackdrop = 0;
    bool backdropReadable = false;
    bool backdropChanged = false;
    uintptr_t resizeRequests = 0;
    CaptionFillState captionFill;
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
constexpr wchar_t kDwmCaptionIdentity[] = L"Windhawk.AppOwnedFrame.DwmCaption.1.Identity";
constexpr wchar_t kDwmCaptionPrefix[] = L"Windhawk.AppOwnedFrame.DwmCaption.1.";
struct DwmCaptionState {
    HANDLE identity = nullptr;
    uintptr_t paintCalls = 0, paintHandled = 0, activationCalls = 0, activationHandled = 0;
    LRESULT lastResult = 0;
};
std::mutex dwmCaptionMutex;
std::unordered_map<HWND, DwmCaptionState> dwmCaptionWindows;
uintptr_t nextDwmCaptionIdentity = 0;
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
using RegisterClass = ATOM(WINAPI*)(const WNDCLASSEXW*);
RegisterClass registerClass = nullptr;
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
HANDLE workerThread = nullptr;
HANDLE inputThread = nullptr;
std::atomic<DWORD> inputThreadId{0};
HANDLE instanceMutex = nullptr;
HWINEVENTHOOK objectHook = nullptr;
HWINEVENTHOOK foregroundHook = nullptr;
HWINEVENTHOOK sizingHook = nullptr;
std::unordered_set<HWND> sizingWindows;
std::unordered_map<HWND, bool> pendingWindows;
bool foregroundChanged = false;
bool sizingChanged = false;
uintptr_t nextIdentity = 0;
bool updatingWindow = false;
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

bool InspectProcess(DWORD pid, ProcessInfo* info) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    FILETIME created{}, exited{}, kernel{}, user{};
    wchar_t path[32768]{};
    DWORD chars = ARRAYSIZE(path);
    std::vector<BYTE> owner;
    const bool success = GetProcessTimes(process, &created, &exited, &kernel, &user) &&
        QueryFullProcessImageNameW(process, 0, path, &chars) &&
        ReadUserSid(process, owner);
    const bool sameUser = success && EqualSid(
        reinterpret_cast<TOKEN_USER*>(owner.data())->User.Sid,
        reinterpret_cast<TOKEN_USER*>(userSid.data())->User.Sid);
    CloseHandle(process);
    if (!sameUser) return false;
    const FrameGeometry geometry = MatchGeometry(path, geometries);
    *info = {pid, created, MatchProgram(path) && geometry != FrameGeometry::Disabled,
        MatchList(path, backdropPrograms), geometry,
        MatchList(path,legacyPrograms) ? MatchGutter(path,gutterPrograms) : kNoBorder};
    return true;
}

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
    if (!target || resizeTarget.target == target) resizeTarget = {};
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
    }
}

void UpdateMouseHook() {
    if (activeGripSize && !mouseHook) {
        mouseHook = SetWindowsHookExW(WH_MOUSE_LL, ResizeMouse, modModule, 0);
        if (!mouseHook) Wh_Log(L"Resize mouse hook registration failed: %lu", GetLastError());
    } else if (!activeGripSize && mouseHook) {
        UnhookWindowsHookEx(mouseHook);
        mouseHook = nullptr;
        CancelPendingResize();
        ClearResizeTarget(nullptr);
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

void ClearDwmCaptionState(HWND window = nullptr) {
    std::lock_guard lock(dwmCaptionMutex);
    for (auto it = dwmCaptionWindows.begin(); it != dwmCaptionWindows.end();) {
        if (!window || it->first == window) {
            if (GetPropW(it->first, kDwmCaptionIdentity) == it->second.identity) {
                for (const PCWSTR name : {L"Identity", L"PaintCalls", L"PaintHandled",
                     L"ActivationCalls", L"ActivationHandled", L"LastResult"}) {
                    const std::wstring property = std::wstring(kDwmCaptionPrefix) + name;
                    RemovePropW(it->first, property.c_str());
                }
            }
            it = dwmCaptionWindows.erase(it);
        } else ++it;
    }
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
    // Diagnostic scalar markers; no window pointers/callbacks survive unload.
    std::lock_guard lock(dwmCaptionMutex);
    if (stopping || !dwmCaptionEnabled) return handled != FALSE;
    auto found = dwmCaptionWindows.find(window);
    if (found == dwmCaptionWindows.end()) {
        if (dwmCaptionWindows.size() >= 256 || GetPropW(window, kDwmCaptionIdentity))
            return handled != FALSE;
        if (!nextDwmCaptionIdentity)
            nextDwmCaptionIdentity = static_cast<uintptr_t>(GetTickCount64()) << 16;
        const HANDLE token = reinterpret_cast<HANDLE>(++nextDwmCaptionIdentity);
        if (!SetPropW(window, kDwmCaptionIdentity, token)) return handled != FALSE;
        found = dwmCaptionWindows.emplace(window, DwmCaptionState{}).first;
        found->second.identity = token;
    }
    auto& state = found->second;
    if (GetPropW(window, kDwmCaptionIdentity) != state.identity) return handled != FALSE;
    if (message == WM_NCPAINT) { ++state.paintCalls; state.paintHandled += handled != FALSE; }
    if (message == WM_NCACTIVATE) { ++state.activationCalls; state.activationHandled += handled != FALSE; }
    state.lastResult = result;
    const std::array<std::pair<PCWSTR, uintptr_t>, 5> values{{
        {L"PaintCalls", state.paintCalls}, {L"PaintHandled", state.paintHandled},
        {L"ActivationCalls", state.activationCalls}, {L"ActivationHandled", state.activationHandled},
        {L"LastResult", static_cast<uintptr_t>(state.lastResult)}
    }};
    for (const auto& [name, value] : values) {
        const std::wstring property = std::wstring(kDwmCaptionPrefix) + name;
        SetPropW(window, property.c_str(), reinterpret_cast<HANDLE>(value));
    }
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
        if (GetPropW(window, kDwmCaptionIdentity)) ClearDwmCaptionState(window);
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
    if (!dwmCaptionEnabled || earlyDwmEnabled) ClearDwmCaptionState();
    if (!earlyDwmEnabled) ClearEarlyDwm();
    { std::lock_guard lock(nativeFrameMutex); nativeFrame = {}; }
    EnumWindows(ClearNativeAck, 0);
    if (defaultProcW) EnumWindows(RefreshLegacyWindow, 0);
}

bool WriteExile(HWND window, BOOL exile) {
    CompositionData data{kNcRenderingExiled, &exile, sizeof(exile)};
    return setComposition(window, &data);
}

void Restore(HWND window, WindowState& state) {
    UpdateScope update;
    ClearResizeTarget(window);
    if (!SameWindow(window, state)) return;
    if (state.borderChanged) {
        const COLORREF color = kDefaultBorder;
        DwmSetWindowAttribute(window, kBorderColor, &color, sizeof(color));
    }
    if (state.backdropChanged && state.backdropReadable)
        DwmSetWindowAttribute(window, kSystemBackdrop,
            &state.originalBackdrop, sizeof(state.originalBackdrop));
    ReconcileCaptionFill(window,state.captionFill,kNoBorder);
    if (state.compositionChanged) WriteExile(window, !state.originalNcRendering);
    RemovePropW(window, kIdentityProperty);
    RemovePropW(window, kResizeProperty);
    RemovePropW(window, kNativeResizeProperty);
    RemovePropW(window, kNativePressProperty);
}

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
        found = windows.end();
    }
    // Iconic and temporarily hidden windows retain their record for rollback.
    if (!IsWindowVisible(window) || IsIconic(window)) {
        if (found != windows.end()) ClearResizeTarget(window);
        return;
    }
    if (!CustomMainWindow(window, FrameGeometry::ResizeGutter) ||
        (found != windows.end() && !CustomMainWindow(window, found->second.process.geometry))) {
        if (found != windows.end()) {
            Restore(window, found->second);
            windows.erase(found);
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
        const HANDLE identity = reinterpret_cast<HANDLE>(
            (static_cast<uintptr_t>(GetCurrentProcessId()) << 32) | ++nextIdentity);
        if (!SetPropW(window, kIdentityProperty, identity)) return;
        found = windows.emplace(window, WindowState{process, enabled != FALSE, identity}).first;
        found->second.backdropReadable = SUCCEEDED(DwmGetWindowAttribute(
            window, kSystemBackdrop, &found->second.originalBackdrop, sizeof(DWORD)));
        forceBorder = true;
        Wh_Log(L"Tracking custom frame hwnd=%p pid=%lu originalNC=%d", window, pid, enabled);
    }
    auto& state = found->second;
    if (state.process.removeBackdrop && state.backdropReadable) {
        DWORD backdrop = 0;
        if (SUCCEEDED(DwmGetWindowAttribute(window, kSystemBackdrop, &backdrop, sizeof(backdrop))) &&
            backdrop != 1) {
            const DWORD none = 1; // DWMSBT_NONE: no system material behind NC.
            if (SUCCEEDED(DwmSetWindowAttribute(window, kSystemBackdrop, &none, sizeof(none))))
                state.backdropChanged = true;
        }
    }
    BOOL enabled = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_NCRENDERING_ENABLED,
                                        &enabled, sizeof(enabled))) && !enabled) {
        if (WriteExile(window, FALSE)) {
            state.compositionChanged = true;
            forceBorder = true;
            Wh_Log(L"Restored compositor clipping hwnd=%p pid=%lu", window, state.process.pid);
        }
    }
    if (forceBorder || !state.borderChanged) {
        const COLORREF color = kNoBorder;
        if (SUCCEEDED(DwmSetWindowAttribute(window, kBorderColor, &color, sizeof(color))))
            state.borderChanged = true;
    }
    // Only the opt-in color of a validated custom-frame root. Never from
    // hit testing, the LL input callback, or an active native resize loop.
    ReconcileCaptionFill(window,state.captionFill,state.process.gutterColor,forceBorder);
    RefreshResizeTarget(window, state);
}

BOOL CALLBACK Enumerate(HWND window, LPARAM) { Apply(window, true); return TRUE; }

void CALLBACK WindowEvent(HWINEVENTHOOK, DWORD event, HWND window, LONG object,
                          LONG child, DWORD, DWORD) {
    if (stopping || !window) return;
    // No DWM calls, process/token queries or helper repositioning from this
    // callback. Location events arrive for every drag step and can reenter.
    // Coalesce them; most importantly, don't touch the app during its native
    // move/size loop. The compositor state established before it stays valid.
    if (event == EVENT_SYSTEM_MOVESIZESTART) {
        sizingWindows.insert(window);
        ClearResizeTarget(window);
        sizingChanged = true;
        return;
    }
    if (event == EVENT_SYSTEM_MOVESIZEEND) {
        sizingWindows.erase(window);
        pendingWindows[window] = true;
        return;
    }
    if (event == EVENT_SYSTEM_FOREGROUND) {
        ClearResizeTarget(nullptr);
        foregroundChanged = true;
        pendingWindows[window] = true;
    } else if (object == OBJID_WINDOW && child == CHILDID_SELF) {
        if (event == EVENT_OBJECT_DESTROY) sizingWindows.erase(window);
        if (pendingWindows.size() < 4096)
            pendingWindows[window] = pendingWindows[window] || event == EVENT_OBJECT_SHOW;
    }
}

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
            Restore(it->first, it->second);
            it = windows.erase(it);
        } else {
            if (!process.removeBackdrop && it->second.backdropChanged && it->second.backdropReadable) {
                DwmSetWindowAttribute(it->first, kSystemBackdrop,
                    &it->second.originalBackdrop, sizeof(DWORD));
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

// CONTROLLER_LEASE_BEGIN: exact code tested with real Windows synchronization.
enum class ControllerAcquire { Owned, Stopped, Failed };

class ControllerLease {
    HANDLE mutex_;
    bool owned_ = false;
public:
    explicit ControllerLease(HANDLE mutex) : mutex_(mutex) {}
    ControllerLease(const ControllerLease&) = delete;
    ControllerLease& operator=(const ControllerLease&) = delete;
    ~ControllerLease() { if (owned_) ReleaseMutex(mutex_); }

    ControllerAcquire Acquire(HANDLE stop) {
        if (owned_) return ControllerAcquire::Owned;
        const HANDLE handles[]{stop, mutex_};
        const DWORD result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (result == WAIT_OBJECT_0) return ControllerAcquire::Stopped;
        if (result != WAIT_OBJECT_0 + 1 && result != WAIT_ABANDONED_0 + 1)
            return ControllerAcquire::Failed;
        owned_ = true;
        // Stop wins even if it arrives just after the mutex was acquired.
        const DWORD cancelled = WaitForSingleObject(stop, 0);
        if (cancelled != WAIT_TIMEOUT) {
            ReleaseMutex(mutex_);
            owned_ = false;
            return cancelled == WAIT_OBJECT_0 ? ControllerAcquire::Stopped
                                              : ControllerAcquire::Failed;
        }
        return ControllerAcquire::Owned;
    }
};
// CONTROLLER_LEASE_END

DWORD WINAPI Worker(void*) {
    // Init must succeed even while the preceding Explorer still has its
    // controller. Only this worker owns/releases the lease; no hooks, state
    // writes, input pump or DPI change are installed while it is waiting.
    SetEvent(readyEvent);
    ControllerLease lease(instanceMutex);
    const ControllerAcquire acquired = lease.Acquire(stopEvent);
    if (acquired != ControllerAcquire::Owned) {
        if (acquired == ControllerAcquire::Failed)
            Wh_Log(L"Controller lease wait failed: %lu", GetLastError());
        return acquired == ControllerAcquire::Stopped ? 0 : 1;
    }
    Wh_Log(L"Controller lease acquired by pid=%lu", GetCurrentProcessId());
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
    objectHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_LOCATIONCHANGE,
        nullptr, WindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        nullptr, WindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    sizingHook = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
        nullptr, WindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!objectHook || !foregroundHook || !sizingHook) {
        Wh_Log(L"WinEvent registration failed: %lu", GetLastError());
        SetEvent(stopEvent);
        if (inputThread) WaitForSingleObject(inputThread, INFINITE);
        if (objectHook) UnhookWinEvent(objectHook);
        if (foregroundHook) UnhookWinEvent(foregroundHook);
        if (sizingHook) UnhookWinEvent(sizingHook);
        workerId = 0;
        if (priorDpi) SetThreadDpiAwarenessContext(priorDpi);
        return 1;
    }
    EnumWindows(Enumerate, 0);
    ULONGLONG nextDiscovery = GetTickCount64() + 2000;
    ULONGLONG nextEvents = GetTickCount64() + 100;
    ULONGLONG nextPoll = GetTickCount64() + 500;
    while (!stopping) {
        const DWORD wait = MsgWaitForMultipleObjects(1, &stopEvent, FALSE, 100, QS_ALLINPUT);
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
        if (now >= nextEvents) {
            // Detach the batch: callbacks can safely queue the next batch.
            auto pending = std::move(pendingWindows);
            pendingWindows.clear();
            for (const auto& [window, forceBorder] : pending) Apply(window, forceBorder);
            nextEvents = now + 100;
        }
        if (now >= nextDiscovery) {
            EnumWindows(Enumerate, 0);
            nextDiscovery = now + 2000;
        } else if (now >= nextPoll) {
            std::vector<HWND> tracked;
            for (const auto& [window, state] : windows) tracked.push_back(window);
            for (HWND window : tracked) Apply(window);
            nextPoll = now + 500;
        }
    }
    UnhookWinEvent(objectHook);
    UnhookWinEvent(foregroundHook);
    UnhookWinEvent(sizingHook);
    for (auto& [window, state] : windows) Restore(window, state);
    windows.clear();
    // The old input hook must be drained before a waiting controller can
    // acquire the lease. Uninit closes these handles only after both joins.
    SetEvent(stopEvent);
    if (inputThread) WaitForSingleObject(inputThread, INFINITE);
    workerId = 0;
    if (priorDpi) SetThreadDpiAwarenessContext(priorDpi);
    return 0;
}
} // namespace

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
    LoadSettings();
    if (_wcsicmp(PathFindFileNameW(path), L"explorer.exe") != 0) {
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
    DWORD session = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &session) || !session ||
        !ReadUserSid(GetCurrentProcess(), userSid)) return FALSE;
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    setComposition = reinterpret_cast<SetComposition>(
        GetProcAddress(user32, "SetWindowCompositionAttribute"));
    if (!setComposition) return FALSE;
    instanceMutex = CreateMutexW(nullptr, FALSE, L"Local\\Windhawk.AppOwnedFrame.Controller.1");
    if (!instanceMutex) return FALSE;
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent || !readyEvent) {
        if (stopEvent) CloseHandle(stopEvent);
        if (readyEvent) CloseHandle(readyEvent);
        CloseHandle(instanceMutex);
        stopEvent = readyEvent = instanceMutex = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (legacyProcess) { RefreshLegacySettings(); return; }
    workerThread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!workerThread) { Wh_Log(L"Controller creation failed: %lu", GetLastError()); return; }
    WaitForSingleObject(readyEvent, 5000);
    Wh_Log(L"Controller candidate ready: thread=%lu", workerId.load());
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = FALSE;
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
    stopping = true;
    if (legacyProcess) {
        earlyDwmEnabled = false;
        ClearEarlyDwm();
        dwmCaptionEnabled = false;
        ClearDwmCaptionState();
        legacyPaintEnabled = false;
        legacyGutterColor = kNoBorder;
        nativeGripSize = 0;
        // Serialize with a one-time class registration hook before the engine
        // removes function detours. No subclass pointers survive DLL unload.
        { std::lock_guard lock(nativeHookMutex); }
        EnumWindows(ClearNativeAck, 0);
        return;
    }
    if (stopEvent) SetEvent(stopEvent);
    // The worker is the sole owner of events/state. Drain it before unloading
    // the DLL, so no callbacks or thread instructions can reference freed code.
    if (workerThread) WaitForSingleObject(workerThread, INFINITE);
    if (inputThread) WaitForSingleObject(inputThread, INFINITE);
}

void Wh_ModUninit() {
    if (legacyProcess) {
        if (defaultProcW) EnumWindows(RefreshLegacyWindow, 0);
        return;
    }
    if (workerThread) CloseHandle(workerThread);
    if (inputThread) CloseHandle(inputThread);
    if (readyEvent) CloseHandle(readyEvent);
    if (stopEvent) CloseHandle(stopEvent);
    if (instanceMutex) CloseHandle(instanceMutex);
}
