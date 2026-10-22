// ==WindhawkMod==
// @id              explorer-column-view
// @name            Explorer Column View
// @description     Browse folders in Finder-style columns with keyboard navigation, file previews, and inline audio and video.
// @version         0.43
// @author          ItsMikesch
// @github          https://github.com/ItsMikesch
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luuid -lshlwapi -lshell32 -lgdi32 -lstrmiids -lmfplat -lmfreadwrite -lmfuuid -lcomctl32 -ladvapi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Column View

Bring Finder-style column browsing to Windows 11 File Explorer. Follow a folder
path across resizable columns, revisit earlier selections, and inspect files
beside your folders.

## In action

Native Windows styling, with folder columns and the audio Inspector:

![Folder columns and audio waveform in Windows 11 light mode](https://raw.githubusercontent.com/ItsMikesch/windhawk-mods/a133451a82c0a5123db47025382704d27a8cbca4/explorer-column-view-light.png)

Columns also follow your existing Explorer styling. See the
[customized Explorer screenshot](https://raw.githubusercontent.com/ItsMikesch/windhawk-mods/a133451a82c0a5123db47025382704d27a8cbca4/explorer-column-view-styled.png);
its glass appearance comes from the user's other customization tools.

## Browse with a clear trail

- Select a folder to reveal its contents in the next column. Choose a different
  folder earlier in the trail to replace the columns that follow it.
- Start with one column and add only the columns you use. Drag a thin divider to
  resize a column live; the layout scrolls horizontally as your path grows.
- Press **Right Arrow** to enter the selected folder's column and **Left Arrow**
  to focus the previous column. Folder double-click and Enter extend the trail.
- Each Explorer tab keeps its own folders, selections, column widths and Inspector.
- On supported Shell versions, **Open in new tab** targets the owning Explorer window; ordinary folder opening
  continues along the column trail. **Open in new window** keeps its native behavior.
- Browse **Home** and **This PC**, local and network folders, and archives exposed
  by your Windows Shell providers, including supported ZIP, RAR and 7z folders.

## Look closer with the Inspector

Selecting a file opens its Inspector after the folder columns, filling the
remaining space. Prefer a sidebar? Enable **Use fixed-width Inspector** to dock
it on the right and show folder details too.

- File information, icons and registered **Open with** applications.
- Image previews and installed Windows preview handlers for supported documents.
- Inline audio and video playback for supported formats and installed codecs.
- Scrubbable audio waveforms, with duration, sample rate, bit depth and channel
  information when available.
- Playback starts when you press Play and pauses when its tab is hidden.

## Turn columns on or off

With Explorer focused, press **Ctrl+Alt+Shift+V** (the default) to switch column
browsing across open and new Explorer windows and tabs. Change **Toggle shortcut**
in the mod settings, for example to `Ctrl+Shift+C` or `F8`. Use `Ctrl`, `Alt`,
`Shift` and/or `Win` separated by `+`, followed by a letter, digit, `F1`–`F24`,
or a named key: `Space`, `Tab`, `Enter`, `Esc`, `Backspace`, `Delete`, `Insert`,
`Home`, `End`, `PageUp`, `PageDown`, `Left`, `Right`, `Up`, or `Down`.
Other keyboard virtual-key codes can be written in hexadecimal, e.g. `Ctrl+0xBA`.
Names are case-insensitive; leave the setting empty to disable the shortcut.
Invalid shortcuts fall back to **Ctrl+Alt+Shift+V** and are logged.

The shortcut only runs in Explorer, not while typing in an address/search/rename
field or using Right Alt (AltGr). Extra modifiers prevent a match. Choose a
combination that does not conflict with another Explorer command: the configured
shortcut takes precedence there, but cannot override Windows-reserved shortcuts.
It remains available to other apps. **Enable columns by default** controls the
initial mode.
Use Explorer's main sidebar or address bar to change the first column's location.
Click **Save settings** in Windhawk to apply configuration changes.

## Compatibility and limitations

- Designed for Windows 11. The mod hosts native Shell views and follows existing
  Explorer styling; it does not include a glass theme.
- Windows 11 context menus are available on supported x64 builds in the 26100
  and 26200 families. Other builds and architectures use classic menus.
  **Use classic context menus** forces that fallback inside the columns.
- Use a column's context menu or keyboard shortcuts for file operations. The
  hidden native view's selection is cleared before columns cover it. The outer
  command bar's **New** and **Paste** still target the original tab folder;
  its status bar can reflect that folder rather than the focused column.
  Selecting or renaming a new item there temporarily reveals native Explorer.
  Deselect it to return to the existing column trail and Inspector.
- Home contents depend on Windows' Shell provider. Unavailable recent entries
  cannot be previewed. Unsupported virtual roots return to the regular native view.
- Previews depend on installed handlers/codecs. Some archive providers expose
  folders but no file streams. Encrypted archives and playback inside archives
  require an external application.
- Some metadata and document-preview calls run on Explorer's UI thread. A slow
  handler or unavailable network file can pause the window. Installed media
  codecs run inside Explorer; **Enable inline audio and video** can be turned
  off for setups with incompatible codecs. Slow providers can delay unload.
- Other mods that rearrange Explorer's file area may need compatibility testing.

## Troubleshooting

**Refresh views after navigation and resizing** keeps additional redraw recovery
enabled. If another styling mod causes excessive flashing, compare with it off.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- EnableByDefault: true
  $name: Enable columns by default
  $description: On starts a fresh Explorer/mod session with columns enabled. Off starts with regular Explorer until the toggle shortcut is pressed. The shortcut switches all open and new windows on or off. Changing this setting applies immediately.
- ToggleShortcut: Ctrl+Alt+Shift+V
  $name: Toggle shortcut
  $description: Explorer-only shortcut. Combine Ctrl, Alt, Shift or Win with a letter, digit, F1-F24 or named key (see Details), e.g. Ctrl+Shift+C or F8. Empty disables it. Invalid values use Ctrl+Alt+Shift+V. Does not run in text fields or with Right Alt (AltGr). Save settings to apply.
- FixedInspector: false
  $name: Use fixed-width Inspector
  $description: Off places file previews after the folder columns and fills the remaining window space. On keeps the Inspector fixed on the far right and also shows folder details.
- DividerWidth: 4
  $name: Divider width
  $description: Width of the draggable divider area, from 2 to 12 logical pixels. Default 4. The visible separator stays one physical pixel; the rest matches the folder background. Save settings to apply.
- ClassicContextMenus: false
  $name: Use classic context menus
  $description: On uses the old right-click menu in folder columns only. Off requests the Windows 11 menu when available, with the classic menu as a fallback. Applies to the next menu opened.
- DelayedPaintRepairs: true
  $name: Refresh views after navigation and resizing
  $description: Perform additional redraws after folders load and columns resize. Leave enabled for drawing recovery; disable to compare if another styling mod causes excessive flashing.
- InlineMediaPlayback: true
  $name: Enable inline audio and video
  $description: Off keeps file information and Open with, without starting the inline media player or decoding waveforms. Applies to the current preview after saving.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <exdisp.h>
#include <exdispid.h>
#include <shdispid.h>
#include <ocidl.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <propkey.h>
#include <dshow.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mfmediaengine.h>
#include <new>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <cstring>
#include <atomic>
#include <memory>
#include <cassert>
#include <cwctype>
#include <windhawk_utils.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT reinterpret_cast<HINSTANCE>(&__ImageBase)

namespace {
std::atomic<bool> g_unloading{false};
std::atomic<unsigned> g_activeCallbacks{0};
std::atomic<unsigned> g_liveComObjects{0};
std::atomic<unsigned> g_livePaneBrowserSites{0};
std::atomic<unsigned> g_liveNavigationSinks{0};
std::atomic<unsigned> g_livePaneNavigationSinks{0};
std::atomic<unsigned> g_liveMediaEngineNotifies{0};
struct CallbackScope {
    CallbackScope() { g_activeCallbacks.fetch_add(1, std::memory_order_acq_rel); }
    ~CallbackScope() { g_activeCallbacks.fetch_sub(1, std::memory_order_acq_rel); }
    CallbackScope(const CallbackScope&) = delete;
    CallbackScope& operator=(const CallbackScope&) = delete;
};
// Declared first in each COM object, so its count survives the complete
// destructor and member teardown. Release's callback scope also covers delete.
struct ComObjectLifetime {
    std::atomic<unsigned>& classCount;
    explicit ComObjectLifetime(std::atomic<unsigned>& count) : classCount(count) {
        classCount.fetch_add(1, std::memory_order_acq_rel);
        g_liveComObjects.fetch_add(1, std::memory_order_acq_rel);
    }
    ~ComObjectLifetime() {
        classCount.fetch_sub(1, std::memory_order_acq_rel);
        g_liveComObjects.fetch_sub(1, std::memory_order_acq_rel);
    }
    ComObjectLifetime(const ComObjectLifetime&) = delete;
    ComObjectLifetime& operator=(const ComObjectLifetime&) = delete;
};
constexpr UINT_PTR kVisualRepairTimerId = 3;
constexpr UINT_PTR kSettledPaintTimerId = 4;
constexpr UINT_PTR kDividerPaintTimerId = 5;
constexpr UINT kVisualRepairDelayMs = 180;
constexpr UINT kSettledPaintDelayMs = 600;
constexpr UINT kNavigationEventMessage = WM_APP + 40;
constexpr UINT kPaneNavigationCompleteMessage = WM_APP + 41;
constexpr UINT kImageReadyMessage = WM_APP + 43;
constexpr UINT kWaveReadyMessage = WM_APP + 44;
constexpr UINT kMediaEventMessage = WM_APP + 45;
constexpr UINT kLayoutSettingsChangedMessage = WM_APP + 46;
constexpr UINT kReconcileModeMessage = WM_APP + 47;
constexpr UINT kCommandDoneMessage = WM_APP + 48;
constexpr UINT kSelectionUpdateMessage = WM_APP + 49;
constexpr UINT kRootNavigationFailedMessage = WM_APP + 50;
constexpr UINT kPaletteRefreshMessage = WM_APP + 51;
constexpr UINT kOverlayBoundsMessage = WM_APP + 52;
constexpr UINT kExplorerWindowMessage = WM_APP + 53;
constexpr UINT kToggleColumnsMessage = WM_APP + 54;
constexpr UINT kNativeStructureMessage = WM_APP + 55;
constexpr UINT_PTR kPaletteRefreshTimerId = 10;
constexpr UINT kPaletteSettleDelayMs = 150;
constexpr UINT_PTR kSelectionRetryTimerId = 9;
constexpr UINT_PTR kPointerReleaseTimerId = 11;
constexpr UINT kPointerReleaseDelayMs = 16;
constexpr UINT kSelectionRetryDelayMs = 100;
constexpr unsigned kSelectionRetryLimit = 4;
constexpr UINT_PTR kStateRetryTimerId = 8;
constexpr UINT_PTR kFocusTimerId = 6;
constexpr UINT_PTR kAudioTimerId = 7;
constexpr UINT kAudioTimerIntervalMs = 30;
constexpr int kSeparatorWidth = 4; // Logical resize hit area; the visible line is one physical pixel.
constexpr UINT_PTR kLiveResizeTimerId = 1; // Splitter-window timer, separate from overlay timers.
constexpr DWORD kLiveResizeIntervalMs = 16;
constexpr int kMaxColumns = 64;
constexpr int kDefaultColumnWidth = 300;
constexpr int kInspectorWidth = 360;
constexpr wchar_t kOverlayClass[] = L"WindhawkExplorerColumnViewOverlay";
constexpr wchar_t kPaneClass[] = L"WindhawkExplorerColumnViewPane";
constexpr wchar_t kViewHostClass[] = L"WindhawkExplorerColumnViewHost";
constexpr wchar_t kSplitterClass[] = L"WindhawkExplorerColumnViewSplitter";
constexpr wchar_t kPreviewClass[] = L"WindhawkExplorerColumnViewPreview";
constexpr wchar_t kVideoClass[] = L"WindhawkExplorerColumnViewVideo";
constexpr wchar_t kThemeObserverClass[] = L"WindhawkExplorerColumnViewObserver";

struct Session;
class PaneNavigationSink;
class MediaEngineNotify;

struct Pane {
    Session* session = nullptr;
    HWND window = nullptr;
    HWND viewHost = nullptr;
    IExplorerBrowser* browser = nullptr;
    IShellBrowser* navigationBrowser = nullptr;
    IServiceProvider* browserSite = nullptr;
    PIDLIST_ABSOLUTE navigationTarget = nullptr;
    IUnknown* styledView = nullptr;
    PaneNavigationSink* navigationSink = nullptr;
    DWORD navigationCookie = 0;
    bool awaitingNavigationRepair = false;
    DWORD navigationStarted = 0;
    bool initialized = false;
    bool styleUnavailableLogged = false;
    DWORD lastSetRect = 0;
    std::wstring path;
    int index = 0;
    ULONG_PTR generation = 0;
    ULONG_PTR navigationSerial = 0;
    bool selectionDirty = true;
    unsigned menuRequests = 0;
    HRESULT lastMenuResult = E_PENDING;
};

void ClearNavigationTarget(Pane& pane) {
    CoTaskMemFree(pane.navigationTarget);
    pane.navigationTarget = nullptr;
}

struct WavePreview {
    DWORD generation = 0;
    bool supported = false;
    DWORD sampleRate = 0;
    WORD channels = 0;
    WORD bits = 0;
    WORD format = 0;
    std::wstring codec;
    ULONGLONG frames = 0;
    std::vector<float> peaks;
};

std::atomic<int> g_dividerWidthSetting{kSeparatorWidth};
int ReadDividerWidthSetting() {
    int width = Wh_GetIntSetting(L"DividerWidth");
    // An existing installation can have no stored value for a newly added setting.
    return width ? std::clamp(width, 2, 12) : kSeparatorWidth;
}
// Workers own their inputs and results independently of the tab. A posted
// notification carries only a unique token: it never transfers a raw owning
// pointer into a window queue, which could outlive the window or reuse its HWND.
struct PreviewJob {
    UINT_PTR token = 0;
    DWORD generation = 0;
    std::wstring path;
    HWND target = nullptr; // Protected by g_previewJobsLock.
    UINT message = 0;
    HANDLE thread = nullptr;
    HANDLE cancel = nullptr;
    bool ready = false;
    HBITMAP bitmap = nullptr;
    std::unique_ptr<WavePreview> wave;
    ~PreviewJob() {
        if (bitmap) DeleteObject(bitmap);
        if (thread) CloseHandle(thread);
        if (cancel) CloseHandle(cancel);
    }
};

SRWLOCK g_previewJobsLock = SRWLOCK_INIT;
std::atomic<UINT_PTR> g_previewJobSequence{0};
// Keep every thread handle until it is signaled, including the interval before
// its entry point starts. CallbackScope alone cannot cover that startup gap.
std::vector<std::shared_ptr<PreviewJob>> g_previewJobs;

bool PreviewCanceled(HANDLE cancel) {
    return g_unloading.load(std::memory_order_acquire) ||
        (cancel && WaitForSingleObject(cancel, 0) == WAIT_OBJECT_0);
}

void RetirePreviewJob(std::shared_ptr<PreviewJob>& job) {
    if (!job) return;
    AcquireSRWLockExclusive(&g_previewJobsLock);
    job->target = nullptr;
    SetEvent(job->cancel);
    if (job->bitmap) DeleteObject(job->bitmap);
    job->bitmap = nullptr;
    job->wave.reset();
    ReleaseSRWLockExclusive(&g_previewJobsLock);
    job.reset();
}

void CompletePreviewJob(const std::shared_ptr<PreviewJob>& job,
                        HBITMAP bitmap, std::unique_ptr<WavePreview> wave) {
    AcquireSRWLockExclusive(&g_previewJobsLock);
    job->ready = true;
    if (job->target && !g_unloading.load(std::memory_order_acquire)) {
        if (PreviewCanceled(job->cancel)) {
            if (bitmap) DeleteObject(bitmap);
            bitmap = nullptr;
            wave.reset();
        }
        job->bitmap = bitmap;
        bitmap = nullptr;
        job->wave = std::move(wave);
        if (!PostMessageW(job->target, job->message, job->token, 0)) {
            job->target = nullptr;
            if (job->bitmap) DeleteObject(job->bitmap);
            job->bitmap = nullptr;
            job->wave.reset();
        }
    }
    ReleaseSRWLockExclusive(&g_previewJobsLock);
    if (bitmap) DeleteObject(bitmap);
}

bool TakePreviewResult(std::shared_ptr<PreviewJob>& current, UINT_PTR token,
                       HWND target, DWORD* generation, HBITMAP* bitmap,
                       std::unique_ptr<WavePreview>* wave) {
    if (!current || current->token != token) return false;
    bool accepted = false;
    AcquireSRWLockExclusive(&g_previewJobsLock);
    if (current->target == target && current->ready) {
        *generation = current->generation;
        if (bitmap) { *bitmap = current->bitmap; current->bitmap = nullptr; }
        if (wave) *wave = std::move(current->wave);
        current->target = nullptr;
        accepted = true;
    }
    ReleaseSRWLockExclusive(&g_previewJobsLock);
    if (accepted) current.reset();
    return accepted;
}

bool ReapPreviewJobs() {
    AcquireSRWLockExclusive(&g_previewJobsLock);
    g_previewJobs.erase(std::remove_if(g_previewJobs.begin(), g_previewJobs.end(),
        [](const std::shared_ptr<PreviewJob>& job) {
            return !job->target && job->thread &&
                WaitForSingleObject(job->thread, 0) == WAIT_OBJECT_0;
        }), g_previewJobs.end());
    bool pending = !g_previewJobs.empty();
    ReleaseSRWLockExclusive(&g_previewJobsLock);
    return pending;
}

std::shared_ptr<PreviewJob> StartPreviewJob(const std::wstring& path,
        HWND target, DWORD generation, UINT message, LPTHREAD_START_ROUTINE entry) {
    if (g_unloading.load(std::memory_order_acquire)) return {};
    // Retired workers are also collected when new selection work starts;
    // preview cleanup does not need a recurring discovery timer.
    ReapPreviewJobs();
    std::shared_ptr<PreviewJob> job;
    try {
        job = std::make_shared<PreviewJob>();
        job->path = path;
    } catch (const std::bad_alloc&) {
        return {};
    }
    job->target = target;
    job->generation = generation;
    job->message = message;
    job->token = g_previewJobSequence.fetch_add(1, std::memory_order_relaxed) + 1;
    job->cancel = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!job->cancel) return {};
    auto* argument = new (std::nothrow) std::shared_ptr<PreviewJob>(job);
    if (!argument) return {};
    AcquireSRWLockExclusive(&g_previewJobsLock);
    if (!g_unloading.load(std::memory_order_acquire)) {
        try {
            g_previewJobs.push_back(job);
        } catch (const std::bad_alloc&) {
            ReleaseSRWLockExclusive(&g_previewJobsLock);
            delete argument;
            return {};
        }
        job->thread = CreateThread(nullptr, 0, entry, argument, 0, nullptr);
        if (!job->thread) g_previewJobs.pop_back();
    }
    ReleaseSRWLockExclusive(&g_previewJobsLock);
    if (!job->thread) { delete argument; return {}; }
    return job;
}

// Pack the key and modifiers together so UI threads never observe a partially
// updated shortcut when settings are saved on another thread.
constexpr UINT kShortcutCtrl = 0x100;
constexpr UINT kShortcutAlt = 0x200;
constexpr UINT kShortcutShift = 0x400;
constexpr UINT kShortcutWin = 0x800;
constexpr UINT kDefaultToggleShortcut = kShortcutCtrl | kShortcutAlt | kShortcutShift | 'V';
std::atomic<UINT> g_toggleShortcut{kDefaultToggleShortcut};

bool ParseToggleShortcut(const wchar_t* value, UINT* shortcut) {
    *shortcut = 0;
    std::wstring text = value ? value : L"";
    UINT modifiers = 0, key = 0;
    size_t start = 0;
    if (text.find_first_not_of(L" \t\r\n") == std::wstring::npos) return true;
    while (start <= text.size()) {
        size_t end = text.find(L'+', start);
        std::wstring token = text.substr(start, end == std::wstring::npos ? end : end - start);
        size_t first = token.find_first_not_of(L" \t\r\n");
        if (first == std::wstring::npos) return false;
        token = token.substr(first, token.find_last_not_of(L" \t\r\n") - first + 1);
        for (auto& ch : token) ch = std::towupper(ch);
        UINT modifier = token == L"CTRL" ? kShortcutCtrl :
                        token == L"ALT" ? kShortcutAlt :
                        token == L"SHIFT" ? kShortcutShift :
                        token == L"WIN" ? kShortcutWin : 0;
        if (modifier) {
            if (key || (modifiers & modifier)) return false;
            modifiers |= modifier;
        } else {
            if (key) return false;
            if (token.size() == 1 && ((token[0] >= L'A' && token[0] <= L'Z') ||
                                      (token[0] >= L'0' && token[0] <= L'9'))) {
                key = token[0];
            } else if (token[0] == L'F' && token.size() >= 2 && token.size() <= 3) {
                UINT number = 0;
                for (size_t i = 1; i < token.size(); ++i) {
                    if (token[i] < L'0' || token[i] > L'9') return false;
                    number = number * 10 + token[i] - L'0';
                }
                if (number < 1 || number > 24) return false;
                key = VK_F1 + number - 1;
            } else if (token.size() >= 3 && token.size() <= 4 && token.substr(0, 2) == L"0X") {
                for (size_t i = 2; i < token.size(); ++i) {
                    wchar_t ch = token[i];
                    if (ch >= L'0' && ch <= L'9') key = key * 16 + ch - L'0';
                    else if (ch >= L'A' && ch <= L'F') key = key * 16 + ch - L'A' + 10;
                    else return false;
                }
                // Exclude mouse buttons, modifiers and undefined sentinel values.
                if (key <= VK_XBUTTON2 || key >= 0xFF ||
                    (key >= VK_SHIFT && key <= VK_MENU) ||
                    key == VK_LWIN || key == VK_RWIN ||
                    (key >= VK_LSHIFT && key <= VK_RMENU)) return false;
            } else {
                static const struct { PCWSTR name; UINT key; } names[] = {
                    {L"SPACE", VK_SPACE}, {L"TAB", VK_TAB}, {L"ENTER", VK_RETURN},
                    {L"ESC", VK_ESCAPE}, {L"BACKSPACE", VK_BACK}, {L"DELETE", VK_DELETE},
                    {L"INSERT", VK_INSERT}, {L"HOME", VK_HOME}, {L"END", VK_END},
                    {L"PAGEUP", VK_PRIOR}, {L"PAGEDOWN", VK_NEXT}, {L"LEFT", VK_LEFT},
                    {L"RIGHT", VK_RIGHT}, {L"UP", VK_UP}, {L"DOWN", VK_DOWN},
                };
                for (const auto& entry : names) if (token == entry.name) key = entry.key;
                if (!key) return false;
            }
        }
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    if (!key) return false;
    *shortcut = modifiers | key;
    return true;
}

void LoadToggleShortcut() {
    PCWSTR value = Wh_GetStringSetting(L"ToggleShortcut");
    UINT shortcut;
    if (!ParseToggleShortcut(value, &shortcut)) {
        Wh_Log(L"[Columns settings] Invalid ToggleShortcut; using Ctrl+Alt+Shift+V");
        shortcut = kDefaultToggleShortcut;
    }
    Wh_FreeStringSetting(value);
    g_toggleShortcut.store(shortcut);
}

std::atomic<bool> g_fixedInspectorSetting{false};
std::atomic<bool> g_classicContextMenus{false};
std::atomic<bool> g_delayedPaintRepairs{true};
std::atomic<bool> g_inlineMediaPlayback{true};
std::atomic<bool> g_enableByDefault{true};
std::atomic<bool> g_columnsEnabled{true};
std::atomic<bool> g_preferenceChangePending{false};
HANDLE g_thread = nullptr;
std::atomic<DWORD> g_threadId{0};
UINT g_toggleMessage = 0;
UINT g_modeChangedMessage = 0;
struct SharedColumnsMode { LONG magic; LONG enabled; };
constexpr LONG kSharedModeMagic = 0x4E435039;
HANDLE g_modeMapping = nullptr; // Helper-thread-owned, never used by UI callbacks.
SharedColumnsMode* g_sharedMode = nullptr;
// Version the mapping and notification together when changing its layout.
constexpr wchar_t kColumnsModeName[] = L"Local\\Windhawk.NativeColumns.Mode.V039";
SRWLOCK g_windowEventsLock = SRWLOCK_INIT;
std::vector<HWND> g_windowEvents;
bool g_windowEventPosted = false; // Protected by g_windowEventsLock.
std::atomic<bool> g_helperQueueReady{false};

std::atomic<ULONG_PTR> g_sessionSequence{0};
std::atomic<ULONG_PTR> g_paneSequence{0};
std::atomic<ULONG_PTR> g_navigationSequence{0};

enum class NativeViewReason { None, SafetyCheck, Navigation, Selection, UnsupportedLocation, RootFailure };

struct Session final : std::enable_shared_from_this<Session> {
    Pane panes[kMaxColumns];
    std::wstring columnSelection[kMaxColumns];
    HWND overlay = nullptr;
    HWND splitters[kMaxColumns] = {};
    bool fixedInspector = false; // UI-thread copy of the saved setting.
    int dividerWidth = kSeparatorWidth; // Snapshot after canceling any active drag.
    bool inlineMediaPlayback = true;
    HWND previewWindow = nullptr;
    IPreviewHandler* previewHandler = nullptr;
    int previewIndex = -1;
    std::wstring previewPath;
    std::wstring previewDisplayName;
    std::wstring previewMessage;
    HBITMAP previewBitmap = nullptr;
    ULONGLONG previewFileSize = 0;
    FILETIME previewModified = {};
    FILETIME previewCreated = {};
    std::wstring previewType;
    bool previewIsFolder = false;
    bool previewIsVirtualFolder = false;
    bool previewMedia = false;
    bool previewVideo = false;
    HWND videoWindow = nullptr;
    DWORD videoWidth = 0, videoHeight = 0, videoFrameRate = 0;
    IMFMediaEngine* mediaEngine = nullptr;
    IMFMediaEngineEx* mediaEngineEx = nullptr;
    MediaEngineNotify* mediaNotify = nullptr; // Owns one ref until playback retirement.
    bool mediaFoundationStarted = false;
    bool mediaReady = false;
    HICON previewIcon = nullptr;
    HICON openWithIcon = nullptr;
    std::wstring openWithName;
    ULONG audioSampleRate = 0;
    ULONG audioBits = 0;
    ULONG audioChannels = 0;
    std::wstring audioFormat;
    int inspectorScrollY = 0;
    int inspectorDrag = 0; // 1: waveform scrub, 2: volume.
    int inspectorHover = 0; // 1: play, 2: back, 3: forward, 4: speaker, 5: volume, 6: open.
    COLORREF inspectorBackground = GetSysColor(COLOR_WINDOW);
    COLORREF inspectorText = GetSysColor(COLOR_WINDOWTEXT);
    HFONT inspectorHeadingFont = nullptr;
    HFONT inspectorNameFont = nullptr;
    HFONT inspectorBodyFont = nullptr;
    HFONT inspectorCaptionFont = nullptr;
    UINT inspectorFontDpi = 0;
    WavePreview* wavePreview = nullptr;
    std::shared_ptr<PreviewJob> waveJob;
    bool waveWanted = false;
    IGraphBuilder* audioGraph = nullptr;
    IMediaControl* audioControl = nullptr;
    IMediaSeeking* audioSeeking = nullptr;
    IBasicAudio* audioVolumeControl = nullptr;
    IVideoWindow* directShowVideo = nullptr;
    int audioVolume = 100;
    int audioUnmutedVolume = 100;
    bool audioPlaying = false;
    bool audioTimerRunning = false;
    LONGLONG audioDuration = 0;
    std::shared_ptr<PreviewJob> imageJob;
    DWORD previewGeneration = 0;
    bool imageWanted = false;
    int focusRequest = -1;
    int focusAttempts = 0;
    unsigned focusEpoch = 0;
    int columnWidths[kMaxColumns] = {};
    int columnCount = 0;
    int scrollX = 0;
    bool inLayout = false;
    HWND tab = nullptr;
    HWND explorer = nullptr;
    HWND originalView = nullptr;
    HWND boundsWindows[16] = {};
    unsigned boundsWindowCount = 0;
    bool boundsUpdatePosted = false;
    bool syncingOverlayBounds = false;
    HWND originalDrawingSurface = nullptr;
    bool originalWasVisible = false;
    bool nativeSelectionGuarded = false;
    bool nativeSelectionChecking = false;
    bool nativeSelectionClearing = false;
    bool nativeNavigationInProgress = false;
    ULONG_PTR nativeNavigationGeneration = 0;
    IConnectionPoint* nativeSelectionConnection = nullptr;
    DWORD nativeSelectionCookie = 0;
    IUnknown* nativeSelectionIdentity = nullptr;
    HWND nativeSelectionWindow = nullptr;
    PIDLIST_ABSOLUTE nativeRestorationExpected = nullptr;
    PIDLIST_ABSOLUTE nativeRestorationRoot = nullptr;
    ULONG_PTR nativeRestorationRootSerial = 0;
    ULONG_PTR nativeRestorationNavigation = 0;
    ULONG_PTR nativeRestorationConnection = 0;
    ULONG_PTR nativeRestorationInputEpoch = 0;
    ULONG_PTR nativeRestorationObserver = 0;
    ULONG_PTR nativeRestorationBlockedSerial = 0;
    ULONGLONG nativeRestorationStarted = 0;
    // Borrowed from the observer, which retires its bound permit before release.
    IUnknown* nativeRestorationViewIdentity = nullptr;
    HWND nativeRestorationWindow = nullptr;
    ULONG_PTR nativeSelectionGeneration = 0;
    ULONG_PTR nativeSelectionChangeSerial = 0;
    std::wstring tabPath;
    int activePane = 0;
    bool uiComInitialized = false;
    DWORD lastSplitterLayout = 0;
    UINT layoutDpi = 0;
    HWND resizeSplitter = nullptr;
    int resizeColumn = -1;
    int resizeStartX = 0;
    int resizeStartWidth = 0;
    int resizeStartScrollX = 0;
    int resizeProposedWidth = 0;
    bool applyingColumnResize = false;
    // These COM interfaces are created, queried and released only on this
    // tab's UI thread. ShellWindows enumeration is discovery/recovery only.
    IWebBrowser2* nativeAutomation = nullptr;
    IShellBrowser* nativeBrowser = nullptr;
    DWORD nativeBrowserThread = 0;
    ULONG_PTR nativeConnectionGeneration = 0;
    PIDLIST_ABSOLUTE nativeLocationIdentity = nullptr;
    ULONG_PTR identity = ++g_sessionSequence;
    bool suspended = false;
    NativeViewReason nativeViewReason = NativeViewReason::None;
    // Native New/Paste can start rename input. A retry must wait for a real
    // deselection instead of clearing the command's selection itself.
    bool nativeSelectionYielded = false;
    // A failed column load restores the good native view, while keeping the
    // native location identity/path as the source of truth for a later retry.
    bool rootNavigationPending = false;
    bool rootNavigationRetry = false;
    unsigned selectionRetryAttempts = 0;
    bool navigationUpdatePosted = false;
    bool selectionUpdatePosted = false;
    bool pointerSelectionPending = false;
    bool nativePointerPending = false;
    ULONG_PTR pointerRootCompletionSerial = 0;
    unsigned stateRetryAttempts = 0;
    unsigned nativeViewRetryAttempts = 0;
    unsigned navigationConnectAttempts = 0;
    IConnectionPoint* navigationConnection = nullptr;
    DWORD navigationCookie = 0;
    bool mouseHookAcquired = false;
    bool tabWasVisible = true;
    bool closed = false;
};

thread_local Session* g_currentSession = nullptr;
thread_local HHOOK g_uiMouseHook = nullptr;
// Frame and tab input can run on different UI threads. Any intervening input
// conservatively cancels restoration eligibility in every Explorer session.
std::atomic<ULONG_PTR> g_nativeInputEpoch{0};
thread_local unsigned g_uiMouseUsers = 0;
std::atomic<unsigned> g_creatingSessions{0};
std::atomic<DWORD> g_previewSequence{0};
constexpr wchar_t kSessionProperty[] = L"Windhawk.ExplorerColumnView.Session";

struct SessionScope {
    std::shared_ptr<Session> owner;
    Session* previous;
    explicit SessionScope(std::shared_ptr<Session> session)
        : owner(std::move(session)), previous(g_currentSession) {
        g_currentSession = owner.get();
    }
    explicit SessionScope(Session* session)
        : SessionScope(session->shared_from_this()) {}
    ~SessionScope() { g_currentSession = previous; }
};

Session& Context() {
    assert(g_currentSession);
    return *g_currentSession;
}

struct SessionRecord {
    HWND tab;
    HWND overlay;
    std::shared_ptr<Session> owner;
};
SRWLOCK g_sessionsLock = SRWLOCK_INIT;
// Process exit happens under the loader lock. Normal unload explicitly drains
// all windows and this registry on their owning UI threads.
std::vector<SessionRecord> g_sessions;

void PublishSession(HWND tab, HWND overlay, std::shared_ptr<Session> owner) {
    AcquireSRWLockExclusive(&g_sessionsLock);
    g_sessions.push_back({tab, overlay, std::move(owner)});
    ReleaseSRWLockExclusive(&g_sessionsLock);
}

void ForgetSession(HWND overlay) {
    AcquireSRWLockExclusive(&g_sessionsLock);
    g_sessions.erase(std::remove_if(g_sessions.begin(), g_sessions.end(),
        [overlay](const SessionRecord& entry) { return entry.overlay == overlay; }),
        g_sessions.end());
    ReleaseSRWLockExclusive(&g_sessionsLock);
}

std::shared_ptr<Session> SessionForTab(HWND tab) {
    std::shared_ptr<Session> result;
    AcquireSRWLockShared(&g_sessionsLock);
    for (const auto& entry : g_sessions)
        if (entry.tab == tab) { result = entry.owner; break; }
    ReleaseSRWLockShared(&g_sessionsLock);
    return result;
}

std::vector<HWND> SessionWindows() {
    std::vector<HWND> result;
    AcquireSRWLockShared(&g_sessionsLock);
    for (const auto& entry : g_sessions) result.push_back(entry.overlay);
    ReleaseSRWLockShared(&g_sessionsLock);
    return result;
}

Session* SessionForWindow(HWND window) {
    for (HWND parent = window; parent; parent = GetParent(parent))
        if (auto* session = reinterpret_cast<Session*>(GetPropW(parent, kSessionProperty)))
            return session;
    return nullptr;
}

struct DispatchTarget { DWORD thread; HHOOK hook; HANDLE ownerThread; };
std::vector<DispatchTarget> g_dispatchHooks; // Helper-thread-owned.
struct TabTarget { HWND explorer; HWND tab; };
std::vector<TabTarget> g_knownTabs; // Discovered once at load, then by window creation.
struct TabCommand { HWND tab; bool pending; LPARAM action; };
std::vector<TabCommand> g_tabCommands; // Helper-thread-owned; UI callbacks post acknowledgments.
constexpr LPARAM kOpenColumnsCommand = 4;
constexpr LPARAM kCloseColumnsCommand = 5;

// Shell DefView asks its browser host for this service before showing the
// Windows 11 menu. These private identifiers are also documented in the
// Windhawk explorer-context-menu-classic mod. Ask Windows for the exact IID
// requested; never reuse another view's presenter or command callback.
constexpr GUID kContextMenuPresenterService = {
    0xb306c5b1, 0xb4f2, 0x473c, {0xb6,0xff,0x70,0x1b,0x24,0x6c,0xe2,0xd2}};
constexpr CLSID kContextMenuPresenterClass = {
    0x86ca1aa0, 0x34aa, 0x4e8b, {0xa5,0x09,0x50,0xc9,0x05,0xba,0xe2,0xa2}};
constexpr IID kContextMenuPresenterV2 = {
    0x37a472f7, 0x63cf, 0x4ccf, {0xa8,0x8b,0x52,0x31,0xa3,0xc7,0xd8,0xb6}};
constexpr IID kInvokeContextMenuCommand = {
    0x9a19ddcf, 0x9ed4, 0x4a18, {0x89,0xde,0xc3,0xb4,0xdd,0x1d,0x7a,0xe3}};

// The V2 contract through Invoke and the callback IID were verified against the
// Microsoft symbols for Windows.UI.FileExplorer and Shell32. Use only this
// known interface version. Older/new unknown versions retain the classic menu.
// DefView prepares the menu. Windows sends selected commands to its view HWND;
// embedded views need to route that message back to their own presenter.
struct MenuPlatform {
    DWORD major = 0, minor = 0, build = 0, revision = 0;
    WORD architecture = PROCESSOR_ARCHITECTURE_UNKNOWN;
    bool versionAvailable = false;
};

const MenuPlatform& GetMenuPlatform() {
    static const MenuPlatform platform = [] {
        MenuPlatform result;
        using RtlGetVersionType = LONG (WINAPI*)(OSVERSIONINFOW*);
        auto module = GetModuleHandleW(L"ntdll.dll");
        auto getVersion = reinterpret_cast<RtlGetVersionType>(
            module ? GetProcAddress(module, "RtlGetVersion") : nullptr);
        OSVERSIONINFOW version = {};
        version.dwOSVersionInfoSize = sizeof(version);
        SYSTEM_INFO system = {};
        GetNativeSystemInfo(&system);
        result.architecture = system.wProcessorArchitecture;
        if (getVersion && getVersion(&version) == 0) {
            result.versionAvailable = true;
            result.major = version.dwMajorVersion;
            result.minor = version.dwMinorVersion;
            result.build = version.dwBuildNumber;
        }
        DWORD length = sizeof(result.revision);
        RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            L"UBR", RRF_RT_REG_DWORD, nullptr, &result.revision, &length);
        return result;
    }();
    return platform;
}

constexpr bool IsSupportedMenuPlatform(DWORD major, DWORD build, WORD architecture) {
    // 24H2 received the exact V2 IID in KB5052093 (26100.3323). 24H2/25H2
    // share servicing/system files (KB5054156). This is a scoped family gate,
    // not a cast of the older interface or a promise about later Windows ABIs.
    return architecture == PROCESSOR_ARCHITECTURE_AMD64 && major == 10 &&
        (build == 26100 || build == 26200);
}

bool IsSupportedMenuPlatform() {
    const auto& platform = GetMenuPlatform();
    return platform.versionAvailable && IsSupportedMenuPlatform(
        platform.major, platform.build, platform.architecture);
}

struct ContextMenuPresenterV2 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Initialize(BOOL modern, IUnknown* invokeCommand,
        HWND ownerWindow, int host) = 0;
    virtual HRESULT STDMETHODCALLTYPE PrepareForContextMenu(UINT locationFlags, IUnknown* site,
        POINT point, IContextMenu* menu, UINT first, UINT last, BOOL canRename,
        const void* verbMap, GUID context, IShellItem* target) = 0;
    virtual void STDMETHODCALLTYPE DoContextMenu(HMENU menu, UINT flags, GUID context) = 0;
    virtual void STDMETHODCALLTYPE DismissContextMenu(PCWSTR reason) = 0;
    virtual BOOL STDMETHODCALLTYPE IsContextMenuOpen() = 0;
    virtual void STDMETHODCALLTYPE Invoke(UINT command) = 0;
};

// Complete native callback contract, verified against Shell32 and
// ExplorerFrame symbols. The bridge also exposes IOleWindow, which embedded
// DefView's callback does not expose on all Windows builds.
struct NativeMenuCommandCallback : IUnknown {
    virtual void STDMETHODCALLTYPE Invoke(IContextMenu* menu, IShellItemArray* selection,
        HMENU popup, UINT command, POINT point) = 0;
    virtual void STDMETHODCALLTYPE OnContextMenuDismiss() = 0;
    virtual void STDMETHODCALLTYPE DoExpandedContextMenu(POINT point, int flags,
        GUID* context, POINT* originalPoint, int keyboard) = 0;
    virtual void STDMETHODCALLTYPE SetFocus() = 0;
};

UINT NativeMenuInvokeMessage() {
    // This named message is used by both Windows.UI.FileExplorer presenters
    // and the native Shell host. No Windows-build-specific offsets or IDs.
    static const UINT message = RegisterWindowMessageW(L"FILE_EXPLORER_CONTEXTMENU_INVOKEMENUITEM");
    return message;
}

const char* ClipboardVerb(DWORD command) {
    switch (command) {
        case OLECMDID_COPY: return "copy";
        case OLECMDID_CUT: return "cut";
        case OLECMDID_PASTE: return "paste";
        default: return nullptr;
    }
}

HRESULT ColumnClipboardCommand(Pane& pane, const char* verb, bool execute, DWORD* status = nullptr) {
    if (status) *status = OLECMDF_SUPPORTED;
    if (!execute && !strcmp(verb, "paste")) {
        // DefView's background menu accepts the canonical Paste verb but does
        // not enumerate it through GetCommandString on all Windows builds.
        // Ask the folder's native drop target whether it accepts the clipboard.
        IDataObject* data = nullptr;
        IFolderView* view = nullptr;
        IShellFolder* folder = nullptr;
        IDropTarget* drop = nullptr;
        HRESULT hr = OleGetClipboard(&data);
        if (SUCCEEDED(hr)) hr = pane.browser ? pane.browser->GetCurrentView(IID_IFolderView,
            reinterpret_cast<void**>(&view)) : E_NOINTERFACE;
        if (SUCCEEDED(hr)) hr = view->GetFolder(IID_IShellFolder, reinterpret_cast<void**>(&folder));
        if (SUCCEEDED(hr)) hr = folder->CreateViewObject(pane.viewHost, IID_IDropTarget,
            reinterpret_cast<void**>(&drop));
        if (SUCCEEDED(hr)) {
            DWORD effect = DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK;
            hr = drop->DragEnter(data, 0, POINTL{}, &effect);
            if (SUCCEEDED(hr)) {
                drop->DragLeave();
                if (effect & (DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK)) {
                    if (status) *status |= OLECMDF_ENABLED;
                } else hr = OLECMDERR_E_DISABLED;
            }
        }
        if (drop) drop->Release();
        if (folder) folder->Release();
        if (view) view->Release();
        if (data) data->Release();
        return hr;
    }
    IShellView* shell = nullptr;
    HRESULT hr = pane.browser ? pane.browser->GetCurrentView(IID_IShellView,
        reinterpret_cast<void**>(&shell)) : E_NOINTERFACE;
    if (FAILED(hr)) return hr;
    HWND commandWindow = nullptr;
    shell->GetWindow(&commandWindow);
    IContextMenu* contextMenu = nullptr;
    hr = shell->GetItemObject(!strcmp(verb, "paste") ? SVGIO_BACKGROUND : SVGIO_SELECTION,
        IID_IContextMenu, reinterpret_cast<void**>(&contextMenu));
    shell->Release();
    if (FAILED(hr)) return hr;
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        DWORD error = GetLastError();
        contextMenu->Release();
        return error ? HRESULT_FROM_WIN32(error) : E_OUTOFMEMORY;
    }
    // Initialize the native menu and invoke a canonical Shell verb. No
    // localized labels, private command IDs, or custom file operation engine.
    constexpr UINT first = 1;
    hr = contextMenu->QueryContextMenu(menu, 0, first, 0x7fff, CMF_NORMAL | CMF_EXPLORE);
    if (SUCCEEDED(hr) && execute) {
        CMINVOKECOMMANDINFOEX info = {};
        info.cbSize = sizeof(info);
        info.fMask = CMIC_MASK_UNICODE;
        info.hwnd = commandWindow;
        info.lpVerb = verb;
        info.lpVerbW = !strcmp(verb, "paste") ? L"paste" : !strcmp(verb, "cut") ? L"cut" : L"copy";
        info.nShow = SW_SHOWNORMAL;
        hr = contextMenu->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&info));
    } else if (SUCCEEDED(hr)) {
        UINT count = HRESULT_CODE(hr);
        hr = OLECMDERR_E_DISABLED;
        for (UINT offset = 0; offset < count; ++offset) {
            char name[80] = {};
            if (FAILED(contextMenu->GetCommandString(offset, GCS_VERBA, nullptr, name, ARRAYSIZE(name))) ||
                strcmp(name, verb)) continue;
            UINT state = GetMenuState(menu, first + offset, MF_BYCOMMAND);
            if (state == static_cast<UINT>(-1) || (state & (MF_DISABLED | MF_GRAYED))) break;
            if (status) *status |= OLECMDF_ENABLED;
            hr = S_OK;
            break;
        }
    }
    DestroyMenu(menu);
    contextMenu->Release();
    return hr;
}

void CancelPendingFocus();

bool NativePointerInteractionActive() {
    // Async state catches a held button even inside Shell's nested drag pump.
    // Both sides are checked so swapped mouse-button settings work as well.
    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
        (GetAsyncKeyState(VK_RBUTTON) & 0x8000) || GetCapture()) return true;
    // Physical release can precede Shell consuming WM_*BUTTONUP. Respect that
    // queued release without latching stale queue state after an outside release.
    bool queuedDown = (GetKeyState(VK_LBUTTON) & 0x8000) ||
                      (GetKeyState(VK_RBUTTON) & 0x8000);
    return queuedDown && (HIWORD(GetQueueStatus(QS_MOUSEBUTTON)) & QS_MOUSEBUTTON);
}

void SchedulePointerRelease(bool selection, bool nativeNavigation = false) {
    Session& session = Context();
    if (session.closed || !IsWindow(session.overlay)) return;
    session.pointerSelectionPending |= selection;
    session.nativePointerPending |= nativeNavigation;
    SetTimer(session.overlay, kPointerReleaseTimerId, kPointerReleaseDelayMs, nullptr);
}

void QueueSelectionUpdate(Pane& pane) {
    Session& session = Context();
    if (session.closed || g_unloading.load()) return;
    pane.selectionDirty = true;
    session.selectionRetryAttempts = 0;
    if (!session.selectionUpdatePosted && IsWindow(session.overlay)) {
        session.selectionUpdatePosted = PostMessageW(session.overlay,
            kSelectionUpdateMessage, session.identity, 0) != FALSE;
    }
}

void QueueNativeNavigationUpdate() {
    Session& session = Context();
    if (!session.closed && !session.navigationUpdatePosted && IsWindow(session.overlay))
        session.navigationUpdatePosted = PostMessageW(session.overlay,
            kNavigationEventMessage, session.identity, 0) != FALSE;
}

void QueueOverlayBoundsUpdate(Session& session);
void QueueThreadBoundsUpdates();

class PaneBrowserSite final : public IServiceProvider, public IOleWindow,
    public IOleCommandTarget, public NativeMenuCommandCallback, public ICommDlgBrowser2 {
    ComObjectLifetime lifetime_{g_livePaneBrowserSites};
    LONG refs_ = 1;
    std::weak_ptr<Session> owner_;
    int index_;
    ULONG_PTR generation_;
    IUnknown* presenter_ = nullptr;
    IUnknown* presenterView_ = nullptr;
    IUnknown* invokeCommand_ = nullptr;
    bool forwardingCommand_ = false;
    HWND presenterWindow_ = nullptr;
    bool invokingMenu_ = false;
    bool resettingPresenter_ = false;
    ULONGLONG menuInputAt_ = 0;
    bool menuOpenReported_ = false;

    static LRESULT CALLBACK PresenterViewProc(HWND window, UINT message, WPARAM wp,
        LPARAM lp, UINT_PTR subclass, DWORD_PTR reference) {
        CallbackScope callbackScope;
        auto* self = reinterpret_cast<PaneBrowserSite*>(reference);
        // Invocation can close columns or navigate to a replacement view.
        self->AddRef();
        LRESULT result;
        if (message == WM_NCDESTROY) {
            if (self->presenterWindow_ == window) self->presenterWindow_ = nullptr;
            RemoveWindowSubclass(window, PresenterViewProc, subclass);
            result = DefSubclassProc(window, message, wp, lp);
            self->Release(); // The installed subclass's reference.
        } else if (message == WM_CONTEXTMENU && !g_classicContextMenus.load()) {

            result = DefSubclassProc(window, message, wp, lp);
        } else if (message == NativeMenuInvokeMessage() && !lp &&
                   wp <= MAXUINT && self->InvokeMenuCommand(window, static_cast<UINT>(wp))) {
            // The original DefView would resolve its host's presenter here.
            // The native tab's presenter belongs to the hidden original view.
            result = 0;
        } else result = DefSubclassProc(window, message, wp, lp);
        self->Release();
        return result;
    }
    bool InvokeMenuCommand(HWND window, UINT command) {
        auto owner = owner_.lock();
        if (!owner || owner->closed || g_unloading.load() || !presenter_ ||
            window != presenterWindow_ || invokingMenu_) return false;
        SessionScope scope(std::move(owner));

        auto* native = static_cast<ContextMenuPresenterV2*>(presenter_);
        native->AddRef();
        invokingMenu_ = true;
        Wh_Log(L"[Columns menu] Tab=%p pane=%d invoke=%u", Context().tab, index_, command);
        native->Invoke(command);
        invokingMenu_ = false;
        native->Release();
        return true;
    }
    HRESULT ConnectPresenterRoute() {
        if (!NativeMenuInvokeMessage()) return E_FAIL;
        IOleWindow* view = nullptr;
        // Use the same IOleWindow query as the native presenter's command
        // event. Its input/activation host is still ShellTabWindowClass.
        HRESULT hr = QueryInterface(IID_IOleWindow, reinterpret_cast<void**>(&view));
        HWND window = nullptr;
        if (SUCCEEDED(hr)) hr = view->GetWindow(&window);
        if (view) view->Release();
        if (FAILED(hr)) return hr;
        if (!IsWindow(window) || GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId())
            return E_FAIL;
        AddRef(); // SetWindowSubclass does not retain the reference data.
        if (!SetWindowSubclass(window, PresenterViewProc, reinterpret_cast<UINT_PTR>(this),
                reinterpret_cast<DWORD_PTR>(this))) {
            Release();
            return E_FAIL;
        }
        presenterWindow_ = window;
        Wh_Log(L"[Columns menu] Pane %d command route connected to %p", index_, window);
        return S_OK;
    }
public:
    explicit PaneBrowserSite(int index)
        : owner_(Context().weak_from_this()), index_(index),
          generation_(Context().panes[index].generation) {}
    ~PaneBrowserSite() { ResetPresenter(); }
    void Retire() {
        ResetPresenter(); // Break presenter -> callback and subclass references first.
        owner_.reset();
    }
    void RecordMenuInput() {
        menuInputAt_ = GetTickCount64();
        menuOpenReported_ = false;
    }
    bool IsMenuOpen() {
        bool open = presenter_ && static_cast<ContextMenuPresenterV2*>(presenter_)->IsContextMenuOpen();
        if (open && menuInputAt_ && !menuOpenReported_) {
            menuOpenReported_ = true;
            Wh_Log(L"[Columns menu timing] Pane %d open observed after mouse release=%llu ms",
                index_, GetTickCount64() - menuInputAt_);
        } else if (!open && menuOpenReported_) {
            menuInputAt_ = 0;
            menuOpenReported_ = false;
        }
        return open;
    }
    void ResetPresenter() {
        if (resettingPresenter_) return;
        resettingPresenter_ = true;
        auto* native = static_cast<ContextMenuPresenterV2*>(presenter_);
        presenter_ = nullptr;
        if (native) {
            // Dismiss while the original view/callback still exists. Clear
            // the cached presenter first because dismissal can re-enter us.
            if (native->IsContextMenuOpen())
                native->DismissContextMenu(L"Columns view retired");
            native->Release();
        }
        HWND window = presenterWindow_;
        if (window && RemoveWindowSubclass(window, PresenterViewProc, reinterpret_cast<UINT_PTR>(this))) {
            presenterWindow_ = nullptr;
            Release(); // The installed subclass's reference.
        }
        if (invokeCommand_) invokeCommand_->Release();
        invokeCommand_ = nullptr;
        if (presenterView_) presenterView_->Release();
        presenterView_ = nullptr;
        resettingPresenter_ = false;
    }
    HRESULT EnsurePresenter(Pane& pane) {
        if (resettingPresenter_) return E_NOINTERFACE;
        if (!IsSupportedMenuPlatform()) return E_NOINTERFACE;
        if (!pane.browser || !pane.initialized) return E_NOINTERFACE;
        IUnknown* current = nullptr;
        HRESULT hr = pane.browser->GetCurrentView(IID_IUnknown,
            reinterpret_cast<void**>(&current));
        if (FAILED(hr)) return hr;
        if (current != presenterView_) ResetPresenter();
        if (!presenter_) {

            hr = current->QueryInterface(kInvokeContextMenuCommand,
                reinterpret_cast<void**>(&invokeCommand_));
            ContextMenuPresenterV2* native = nullptr;
            if (SUCCEEDED(hr))
                hr = CoCreateInstance(kContextMenuPresenterClass, nullptr, CLSCTX_INPROC_SERVER,
                    kContextMenuPresenterV2, reinterpret_cast<void**>(&native));
            // Match Explorer's native presenter: its input/activation host is
            // ShellTabWindowClass. Its command callback forwards to this column's
            // DefView and exposes its command HWND through IOleWindow. A column
            // HWND cannot substitute for the tab's XAML input host.
            HWND inputHost = Context().tab;
            if (SUCCEEDED(hr) && (!IsWindow(inputHost) ||
                GetWindowThreadProcessId(inputHost, nullptr) != GetCurrentThreadId())) hr = E_FAIL;
            // 2 is Explorer's host kind, verified in CShellBrowser's own initialization.
            if (SUCCEEDED(hr)) hr = native->Initialize(TRUE, static_cast<NativeMenuCommandCallback*>(this), inputHost, 2);
            if (SUCCEEDED(hr)) hr = ConnectPresenterRoute();
            Wh_Log(L"[Columns menu] Pane %d presenter initialization=%08X",
                index_, static_cast<unsigned>(hr));
            if (SUCCEEDED(hr)) {
                presenter_ = native;
                presenterView_ = current;
                current = nullptr;
            } else {
                if (native) native->Release();
                ResetPresenter();
            }
        } else hr = S_OK;
        if (current) current->Release();
        return hr;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        CallbackScope callbackScope;
        if (!out) return E_POINTER;
        *out = nullptr;
        if (IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_IServiceProvider))
            *out = static_cast<IServiceProvider*>(this);
        else if (IsEqualIID(iid, IID_IOleWindow))
            *out = static_cast<IOleWindow*>(this);
        else if (IsEqualIID(iid, IID_IOleCommandTarget))
            *out = static_cast<IOleCommandTarget*>(this);
        else if (IsEqualIID(iid, kInvokeContextMenuCommand))
            *out = static_cast<NativeMenuCommandCallback*>(this);
        else if (IsEqualIID(iid, IID_ICommDlgBrowser) || IsEqualIID(iid, IID_ICommDlgBrowser2))
            *out = static_cast<ICommDlgBrowser2*>(this);
        else return E_NOINTERFACE;
        AddRef();
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        CallbackScope callbackScope;
        return InterlockedIncrement(&refs_);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        CallbackScope callbackScope;
        ULONG refs = InterlockedDecrement(&refs_);
        if (!refs) delete this;
        return refs;
    }
    template<class Action> void ForwardMenuCallback(Action action, bool retiring = false) {
        CallbackScope callbackScope;
        auto owner = owner_.lock();
        if (!owner || !invokeCommand_ || (!retiring && (owner->closed || g_unloading.load()))) return;
        SessionScope scope(std::move(owner));
        if (!retiring && (Context().panes[index_].generation != generation_ ||
            Context().panes[index_].browserSite != static_cast<IServiceProvider*>(this))) return;
        auto* native = static_cast<NativeMenuCommandCallback*>(invokeCommand_);
        native->AddRef();
        action(native);
        native->Release();
    }
    void STDMETHODCALLTYPE Invoke(IContextMenu* menu, IShellItemArray* selection,
        HMENU popup, UINT command, POINT point) override {
        CallbackScope callbackScope;
        ForwardMenuCallback([&](NativeMenuCommandCallback* native) {
            native->Invoke(menu, selection, popup, command, point);
        });
    }
    void STDMETHODCALLTYPE OnContextMenuDismiss() override {
        CallbackScope callbackScope;

        ForwardMenuCallback([](NativeMenuCommandCallback* native) {
            native->OnContextMenuDismiss();
        }, true);
        QueueThreadBoundsUpdates();
    }
    void STDMETHODCALLTYPE DoExpandedContextMenu(POINT point, int flags, GUID* context,
        POINT* originalPoint, int keyboard) override {
        CallbackScope callbackScope;
        ForwardMenuCallback([&](NativeMenuCommandCallback* native) {
            native->DoExpandedContextMenu(point, flags, context, originalPoint, keyboard);
        });
    }
    void STDMETHODCALLTYPE SetFocus() override {
        CallbackScope callbackScope;

        ForwardMenuCallback([](NativeMenuCommandCallback* native) { native->SetFocus(); });
    }
    HRESULT STDMETHODCALLTYPE QueryService(REFGUID service, REFIID iid, void** out) override {
        CallbackScope callbackScope;
        if (!out) return E_POINTER;
        *out = nullptr;
        auto owner = owner_.lock();
        if (!owner || owner->closed || g_unloading.load()) return E_NOINTERFACE;
        SessionScope scope(std::move(owner));
        Pane& pane = Context().panes[index_];
        if (pane.browserSite != static_cast<IServiceProvider*>(this) ||
            pane.generation != generation_) return E_NOINTERFACE;
        // ExplorerBrowser queries this service during Initialize, before the
        // embedded view exists. Do not gate this host interface on initialized.
        if (IsEqualGUID(service, SID_SExplorerBrowserFrame))
            return QueryInterface(iid, out);
        if (!pane.browser || !pane.initialized) return E_NOINTERFACE;
        // Native Explorer commands resolve their target through the site chain.
        // Keep that chain in this column, rather than the hidden original view.
        if (IsEqualGUID(service, IID_IOleCommandTarget) ||
            ((IsEqualGUID(service, SID_STopLevelBrowser) ||
              IsEqualGUID(service, SID_SShellBrowser)) && IsEqualIID(iid, IID_IOleCommandTarget)))
            return QueryInterface(iid, out);
        if (IsEqualGUID(service, SID_STopLevelBrowser) || IsEqualGUID(service, SID_SShellBrowser))
            return pane.browser->QueryInterface(iid, out);
        if (IsEqualGUID(service, SID_SFolderView))
            return pane.browser->GetCurrentView(iid, out);
        if (!IsEqualGUID(service, kContextMenuPresenterService)) return E_NOINTERFACE;

        bool classic = g_classicContextMenus.load();
        HRESULT hr = E_NOINTERFACE;
        PCWSTR reason = classic ? L"classic-setting" :
            !IsEqualIID(iid, kContextMenuPresenterV2) ? L"unrecognized-presenter-IID" :
            !IsSupportedMenuPlatform() ? L"unsupported-Windows-build-or-architecture" : L"none";
        if (!classic && IsEqualIID(iid, kContextMenuPresenterV2)) {
            // DefView requests this service several times while preparing and
            // displaying one menu. Keep one presenter identity for this view.
            hr = EnsurePresenter(pane);
            if (SUCCEEDED(hr)) hr = presenter_->QueryInterface(iid, out);
            if (FAILED(hr) && !wcscmp(reason, L"none")) reason = L"presenter-initialization-failed";
        }
        ++pane.menuRequests;
        pane.lastMenuResult = hr;
        const auto& platform = GetMenuPlatform();
        wchar_t requested[40] = {};
        StringFromGUID2(iid, requested, ARRAYSIZE(requested));
        Wh_Log(L"[Columns menu] v%s pid=%u Tab=%p pane=%d request=%u mode=%s result=%08X "
               L"Windows=%u.%u.%u.%u arch=%u requested=%s reason=%s",
            WH_MOD_VERSION, GetCurrentProcessId(), Context().tab, index_, pane.menuRequests,
            classic ? L"classic" : L"Windows 11", static_cast<unsigned>(hr),
            platform.major, platform.minor, platform.build, platform.revision,
            static_cast<unsigned>(platform.architecture), requested, reason);
        if (FAILED(hr)) {
            Wh_Log(L"[Columns menu fallback] v%s Windows=%u.%u.%u.%u arch=%u requested=%s reason=%s",
                WH_MOD_VERSION, platform.major, platform.minor, platform.build, platform.revision,
                static_cast<unsigned>(platform.architecture), requested, reason);
        }
        return hr;
    }
    HRESULT STDMETHODCALLTYPE GetWindow(HWND* window) override {
        CallbackScope callbackScope;
        if (!window) return E_POINTER;
        *window = nullptr;
        auto owner = owner_.lock();
        if (!owner || owner->closed || g_unloading.load()) return E_FAIL;
        IShellView* shell = nullptr;
        Pane& pane = owner->panes[index_];
        HRESULT hr = pane.browser ? pane.browser->GetCurrentView(IID_IShellView,
            reinterpret_cast<void**>(&shell)) : E_FAIL;
        if (SUCCEEDED(hr)) hr = shell->GetWindow(window);
        if (shell) shell->Release();
        return SUCCEEDED(hr) && IsWindow(*window) ? S_OK : E_FAIL;
    }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override {
        CallbackScope callbackScope;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultCommand(IShellView*) override {
        CallbackScope callbackScope;
        // S_FALSE explicitly preserves DefView's file-open/default verb. Folder
        // activation is still intercepted by PaneNavigationSink's existing path.
        return S_FALSE;
    }
    HRESULT STDMETHODCALLTYPE OnStateChange(IShellView*, ULONG change) override {
        CallbackScope callbackScope;
        auto owner = owner_.lock();
        if (!owner || owner->closed || g_unloading.load()) return S_OK;
        SessionScope scope(std::move(owner));
        Pane& pane = Context().panes[index_];
        if (pane.browserSite != static_cast<IServiceProvider*>(this) ||
            pane.generation != generation_) return S_OK;
        if (change == CDBOSC_SELCHANGE || change == CDBOSC_RENAME ||
            change == CDBOSC_STATECHANGE || change == CDBOSC_SETFOCUS)
            QueueSelectionUpdate(pane);
        // Never replace views or call navigation from a Shell callback.
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE IncludeObject(IShellView*, PCUITEMID_CHILD) override {
        CallbackScope callbackScope;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Notify(IShellView*, DWORD) override {
        CallbackScope callbackScope;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetDefaultMenuText(IShellView*, LPWSTR text, int length) override {
        CallbackScope callbackScope;
        if (text && length > 0) *text = L'\0';
        return S_FALSE;
    }
    HRESULT STDMETHODCALLTYPE GetViewFlags(DWORD* flags) override {
        CallbackScope callbackScope;
        if (!flags) return E_POINTER;
        // Keep Explorer's normal hidden-file policy. Avoid the expensive
        // IncludeObject callback for every item: this host does no filtering.
        *flags = CDB2GVF_NOINCLUDEITEM | CDB2GVF_NOSELECTVERB;
        return S_OK;
    }
    HRESULT ForwardCommand(const GUID* group, ULONG count, OLECMD* commands, OLECMDTEXT* text,
                           DWORD id, DWORD options, VARIANT* in, VARIANT* out, bool execute) {
        auto owner = owner_.lock();
        if (!owner || owner->closed || g_unloading.load() || forwardingCommand_)
            return OLECMDERR_E_NOTSUPPORTED;
        SessionScope scope(std::move(owner));
        IExplorerBrowser* browser = Context().panes[index_].browser;
        IOleCommandTarget* target = nullptr;
        forwardingCommand_ = true;
        HRESULT hr = browser ? browser->GetCurrentView(IID_IOleCommandTarget,
            reinterpret_cast<void**>(&target)) : E_NOINTERFACE;
        if (SUCCEEDED(hr)) {
            hr = execute ? target->Exec(group, id, options, in, out) :
                           target->QueryStatus(group, count, commands, text);
            target->Release();
        }
        if (!group && (hr == OLECMDERR_E_UNKNOWNGROUP || hr == OLECMDERR_E_NOTSUPPORTED)) {
            Pane& pane = Context().panes[index_];
            if (execute) {
                if (const char* verb = ClipboardVerb(id)) hr = ColumnClipboardCommand(pane, verb, true);
            } else if (count && commands) {
                bool supported = false;
                for (ULONG i = 0; i < count; ++i) {
                    commands[i].cmdf = 0;
                    if (const char* verb = ClipboardVerb(commands[i].cmdID)) {
                        ColumnClipboardCommand(pane, verb, false, &commands[i].cmdf);
                        supported = true;
                    }
                }
                if (supported) hr = S_OK;
            }
        }
        forwardingCommand_ = false;
        if (execute)
            Wh_Log(L"[Columns command] Tab=%p pane=%d id=%u result=%08X",
                Context().tab, index_, id, static_cast<unsigned>(hr));
        return hr;
    }
    HRESULT STDMETHODCALLTYPE QueryStatus(const GUID* group, ULONG count,
        OLECMD* commands, OLECMDTEXT* text) override {
        CallbackScope callbackScope;
        if (count && !commands) return E_POINTER;
        return ForwardCommand(group, count, commands, text, 0, 0, nullptr, nullptr, false);
    }
    HRESULT STDMETHODCALLTYPE Exec(const GUID* group, DWORD id, DWORD options,
        VARIANT* in, VARIANT* out) override {
        CallbackScope callbackScope;
        return ForwardCommand(group, 0, nullptr, nullptr, id, options, in, out, true);
    }
};

std::shared_ptr<Session> NativeMenuSessionOnThread() {
    // Flyout input HWNDs can be descendants of DefView, or separate owned
    // popups. Look at the native presenters on this UI thread, not class names.
    for (HWND overlay : SessionWindows()) {
        if (GetWindowThreadProcessId(overlay, nullptr) != GetCurrentThreadId()) continue;
        Session* owner = SessionForWindow(overlay);
        if (!owner || owner->closed) continue;
        for (int i = 0; i < owner->columnCount; ++i) {
            auto* site = static_cast<PaneBrowserSite*>(owner->panes[i].browserSite);
            if (site && site->IsMenuOpen()) return owner->shared_from_this();
        }
    }
    return {};
}

void QueueThreadBoundsUpdates() {
    if (g_unloading.load()) return;
    // Bounds changes are deferred while any asynchronous flyout owns this UI
    // thread. Its dismissal supplies the event that resumes those updates.
    for (HWND overlay : SessionWindows()) {
        if (GetWindowThreadProcessId(overlay, nullptr) != GetCurrentThreadId()) continue;
        if (auto* session = SessionForWindow(overlay)) QueueOverlayBoundsUpdate(*session);
    }
}


void NativeRootNavigationStarting();
void NativeSelectionChanged(DISPID event = DISPID_SELECTIONCHANGED);

class NavigationSink final : public IDispatch {
    ComObjectLifetime lifetime_{g_liveNavigationSinks};
    LONG refs_ = 1;
    std::weak_ptr<Session> owner_;
    bool selectionEvents_;
    ULONG_PTR generation_;
public:
    explicit NavigationSink(std::weak_ptr<Session> owner, bool selectionEvents = false)
        : owner_(std::move(owner)), selectionEvents_(selectionEvents),
          generation_(selectionEvents ? Context().nativeSelectionGeneration :
              Context().nativeConnectionGeneration) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        CallbackScope callbackScope;
        if (!out) return E_POINTER;
        *out = nullptr;
        if (IsEqualIID(iid, IID_IUnknown) ||
            IsEqualIID(iid, IID_IDispatch) ||
            IsEqualIID(iid, selectionEvents_ ? DIID_DShellFolderViewEvents :
                DIID_DWebBrowserEvents2)) {
            *out = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        CallbackScope callbackScope;
        return InterlockedIncrement(&refs_);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        CallbackScope callbackScope;
        ULONG refs = InterlockedDecrement(&refs_);
        if (!refs) delete this;
        return refs;
    }
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* count) override {
        CallbackScope callbackScope;
        if (count) *count = 0;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**) override {
        CallbackScope callbackScope;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID,
                                            DISPID*) override {
        CallbackScope callbackScope;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE Invoke(DISPID id, REFIID, LCID, WORD,
                                     DISPPARAMS*, VARIANT*, EXCEPINFO*,
                                     UINT*) override {
        CallbackScope callbackScope;
        auto owner = owner_.lock();
        if (g_unloading.load() || !owner || owner->closed) return S_OK;
        SessionScope scope(std::move(owner));
        if (selectionEvents_) {
            if (generation_ == Context().nativeSelectionGeneration &&
                (id == DISPID_SELECTIONCHANGED || id == DISPID_FILELISTENUMDONE))
                NativeSelectionChanged(id);
            return S_OK;
        }
        if (generation_ != Context().nativeConnectionGeneration) return S_OK;
        if (id == DISPID_BEFORENAVIGATE2) NativeRootNavigationStarting();
        if (id == DISPID_BEFORENAVIGATE2)
            Wh_Log(L"[Columns] Explorer BeforeNavigate2");
        if (id == DISPID_DOCUMENTCOMPLETE)
            Wh_Log(L"[Columns] Explorer DocumentComplete");
        if (id == DISPID_DOWNLOADCOMPLETE)
            Wh_Log(L"[Columns] Explorer DownloadComplete");
        if (id == DISPID_NAVIGATECOMPLETE2 || id == DISPID_DOCUMENTCOMPLETE ||
            id == DISPID_DOWNLOADCOMPLETE || id == DISPID_NAVIGATEERROR) {
            Context().nativeNavigationInProgress = false;
            Context().stateRetryAttempts = 0;
            QueueNativeNavigationUpdate();
        }
        return S_OK;
    }
};

class PaneNavigationSink final : public IExplorerBrowserEvents {
    ComObjectLifetime lifetime_{g_livePaneNavigationSinks};
    LONG refs_ = 1;
    int index_;
    ULONG_PTR generation_;
    std::weak_ptr<Session> owner_;
public:
    explicit PaneNavigationSink(int index)
        : index_(index), generation_(Context().panes[index].generation),
          owner_(Context().weak_from_this()) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        CallbackScope callbackScope;
        if (!out) return E_POINTER;
        *out = nullptr;
        if (IsEqualIID(iid, IID_IUnknown) ||
            IsEqualIID(iid, IID_IExplorerBrowserEvents)) {
            *out = static_cast<IExplorerBrowserEvents*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        CallbackScope callbackScope;
        return InterlockedIncrement(&refs_);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        CallbackScope callbackScope;
        ULONG refs = InterlockedDecrement(&refs_);
        if (!refs) delete this;
        return refs;
    }
    HRESULT STDMETHODCALLTYPE OnNavigationPending(PCIDLIST_ABSOLUTE target) override {
        CallbackScope callbackScope;
        auto owner = owner_.lock();
        if (!owner || owner->closed) return E_ABORT;
        SessionScope scope(std::move(owner));
        if (g_unloading.load() || Context().panes[index_].generation != generation_ ||
            Context().panes[index_].navigationSink != this) return E_ABORT;
        Pane& pane = Context().panes[index_];
        // Only explicit tab/sidebar navigation may replace this column.
        // Shell folder activation (double-click, Enter, Open) stays in the
        // column chain, processed by the usual selection handler after return.
        if (target && pane.navigationTarget &&
            ILIsEqual(target, pane.navigationTarget)) return S_OK;
        Wh_Log(L"[Columns] Pane %d folder activation kept in columns", index_);
        QueueSelectionUpdate(pane);
        return E_ABORT;
    }
    HRESULT STDMETHODCALLTYPE OnViewCreated(IShellView*) override {
        CallbackScope callbackScope;
        auto owner = owner_.lock();
        if (!owner || owner->closed) return S_OK;
        SessionScope scope(std::move(owner));
        if (g_unloading.load() || Context().panes[index_].generation != generation_ ||
            Context().panes[index_].navigationSink != this) return S_OK;
        Wh_Log(L"[Columns] Pane %d view created", index_);
        QueueSelectionUpdate(Context().panes[index_]);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnNavigationComplete(PCIDLIST_ABSOLUTE target) override {
        CallbackScope callbackScope;
        auto owner = owner_.lock();
        if (!owner || owner->closed) return S_OK;
        SessionScope scope(std::move(owner));
        if (g_unloading.load() || Context().panes[index_].generation != generation_ ||
            Context().panes[index_].navigationSink != this) return S_OK;
        Pane& pane = Context().panes[index_];
        if (pane.navigationSink != this) return S_OK;
        if (pane.navigationTarget) {
            if (!target || !ILIsEqual(target, pane.navigationTarget)) return S_OK;
            ClearNavigationTarget(pane);
        }
        QueueSelectionUpdate(pane);
        DWORD elapsed = pane.awaitingNavigationRepair ?
                        GetTickCount() - pane.navigationStarted : 0;
        Wh_Log(L"[Columns] Pane %d navigation complete; repair=%d "
               L"elapsed=%u ms", index_,
               pane.awaitingNavigationRepair ? 1 : 0, elapsed);
        if (pane.awaitingNavigationRepair) {
            pane.awaitingNavigationRepair = false;
            if (IsWindow(Context().overlay)) {
                PostMessageW(Context().overlay, kPaneNavigationCompleteMessage,
                             static_cast<WPARAM>(index_), static_cast<LPARAM>(pane.navigationSerial));
            }
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnNavigationFailed(PCIDLIST_ABSOLUTE target) override {
        CallbackScope callbackScope;
        auto owner = owner_.lock();
        if (!owner || owner->closed) return S_OK;
        SessionScope scope(std::move(owner));
        if (g_unloading.load() || Context().panes[index_].generation != generation_ ||
            Context().panes[index_].navigationSink != this) return S_OK;
        Pane& pane = Context().panes[index_];
        if (!pane.navigationTarget ||
            (target && !ILIsEqual(target, pane.navigationTarget))) return S_OK;
        ClearNavigationTarget(pane);
        pane.awaitingNavigationRepair = false;
        Wh_Log(L"[Columns] Pane %d navigation failed", index_);
        if (index_ == 0 && Context().rootNavigationPending && IsWindow(Context().overlay))
            PostMessageW(Context().overlay, kRootNavigationFailedMessage,
                Context().identity, static_cast<LPARAM>(pane.navigationSerial));
        return S_OK;
    }
};

void PaintColumnBackground(HWND window, HDC dc, const RECT* area = nullptr);
void PaintSplitterContents(HWND window, HDC dc, const RECT* area = nullptr);
void CancelPendingFocus();
void CancelColumnResize();

void CancelFocusForUserInput(HWND target) {
    HWND root = GetAncestor(target, GA_ROOT);
    if (!root) return;
    // A sidebar/address-bar click is outside the overlay. Cancel pending
    // requests in all tabs of this frame on this UI thread before dispatch.
    for (HWND overlay : SessionWindows()) {
        if (GetWindowThreadProcessId(overlay, nullptr) != GetCurrentThreadId()) continue;
        Session* owner = SessionForWindow(overlay);
        if (owner && owner->explorer == root) {
            SessionScope scope(owner);
            CancelPendingFocus();
        }
    }
}

void SetActivePane(int index) {
    if (index < 0 || index >= Context().columnCount || Context().activePane == index) return;
    Context().activePane = index;
    for (auto& pane : Context().panes) {
        if (pane.window) InvalidateRect(pane.window, nullptr, FALSE);
    }
}

bool IsNativeRestorationInput(UINT message, WPARAM wp = 0) {
    switch (message) {
        case WM_KEYDOWN: case WM_SYSKEYDOWN: case WM_CHAR: case WM_SYSCHAR:
        case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN: case WM_XBUTTONDOWN:
        case WM_LBUTTONDBLCLK: case WM_RBUTTONDBLCLK: case WM_MBUTTONDBLCLK: case WM_XBUTTONDBLCLK:
        case WM_NCLBUTTONDOWN: case WM_NCRBUTTONDOWN: case WM_NCMBUTTONDOWN: case WM_NCXBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK: case WM_NCRBUTTONDBLCLK: case WM_NCMBUTTONDBLCLK: case WM_NCXBUTTONDBLCLK:
        case WM_MOUSEWHEEL: case WM_MOUSEHWHEEL: case WM_POINTERDOWN: case WM_TOUCH:
        case WM_COMMAND: case WM_SYSCOMMAND: case WM_APPCOMMAND:
            return true;
        case WM_POINTERUPDATE:
            return (HIWORD(wp) & POINTER_MESSAGE_FLAG_INCONTACT) != 0;
    }
    return false;
}

void ObserveNativeRestorationInput(UINT message, WPARAM wp = 0) {
    if (IsNativeRestorationInput(message, wp)) g_nativeInputEpoch.fetch_add(1);
}

LRESULT CALLBACK MouseHookProc(int code, WPARAM wp, LPARAM lp) {
    CallbackScope callbackScope;
    if (g_unloading.load()) return CallNextHookEx(nullptr, code, wp, lp);
    if (code >= 0) ObserveNativeRestorationInput(static_cast<UINT>(wp));
    if (code >= 0 && (wp == WM_LBUTTONDOWN || wp == WM_RBUTTONDOWN || wp == WM_RBUTTONUP)) {
        if (NativeMenuSessionOnThread()) return CallNextHookEx(nullptr, code, wp, lp);
        auto* mouse = reinterpret_cast<MOUSEHOOKSTRUCT*>(lp);
        if (wp != WM_RBUTTONUP) CancelFocusForUserInput(mouse->hwnd);
        Session* owner = SessionForWindow(mouse->hwnd);
        if (!owner) return CallNextHookEx(nullptr, code, wp, lp);
        SessionScope scope(owner);
        for (int i = 0; i < Context().columnCount; ++i) {
            if (Context().panes[i].window &&
                (mouse->hwnd == Context().panes[i].window ||
                 IsChild(Context().panes[i].window, mouse->hwnd))) {
                if (wp == WM_RBUTTONUP) {
                    if (Context().panes[i].browserSite)
                        static_cast<PaneBrowserSite*>(Context().panes[i].browserSite)->RecordMenuInput();
                } else SetActivePane(i);
                break;
            }
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

void LogHr(const wchar_t* step, HRESULT hr) {
    Wh_Log(L"[Columns] %s: HRESULT 0x%08X", step,
           static_cast<unsigned>(hr));
}

void LogWin32(const wchar_t* step) {
    Wh_Log(L"[Columns] %s: Win32 error %u", step, GetLastError());
}

bool IsExplorerFrame(HWND hwnd) {
    wchar_t name[64] = {};
    if (!hwnd || !GetClassNameW(hwnd, name, ARRAYSIZE(name)) ||
        wcscmp(name, L"CabinetWClass") != 0) return false;
    return true;
}

bool IsExplorerWindow(HWND hwnd) {
    if (!IsExplorerFrame(hwnd)) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid == GetCurrentProcessId();
}

void QueueExplorerWindow(HWND window) {
    if (g_unloading.load()) return;
    AcquireSRWLockExclusive(&g_windowEventsLock);
    if (std::find(g_windowEvents.begin(), g_windowEvents.end(), window) == g_windowEvents.end())
        g_windowEvents.push_back(window);
    if (!g_windowEventPosted && g_helperQueueReady.load())
        g_windowEventPosted = PostThreadMessageW(g_threadId, kExplorerWindowMessage, 0, 0) != FALSE;
    ReleaseSRWLockExclusive(&g_windowEventsLock);
}

using CreateWindowExWFn = decltype(&CreateWindowExW);
CreateWindowExWFn g_createWindowExWOriginal = nullptr;

HWND WINAPI CreateWindowExWHook(DWORD exStyle, LPCWSTR className, LPCWSTR title,
    DWORD style, int x, int y, int width, int height, HWND parent, HMENU menu,
    HINSTANCE instance, LPVOID parameter) {
    CallbackScope callbackScope;
    HWND window = g_createWindowExWOriginal(exStyle, className, title, style,
        x, y, width, height, parent, menu, instance, parameter);
    if (!window || g_unloading.load() || SessionForWindow(window)) return window;
    wchar_t name[80] = {};
    if (GetClassNameW(window, name, ARRAYSIZE(name)) &&
        (!wcscmp(name, L"CabinetWClass") || !wcscmp(name, L"ShellTabWindowClass") ||
         !wcscmp(name, L"SHELLDLL_DefView") || !wcscmp(name, L"DirectUIHWND")))
        QueueExplorerWindow(window);
    return window;
}

HWND FindTabWindow(HWND fileView) {
    for (HWND parent = GetParent(fileView); parent; parent = GetParent(parent)) {
        wchar_t name[64] = {};
        if (GetClassNameW(parent, name, ARRAYSIZE(name)) &&
            wcscmp(name, L"ShellTabWindowClass") == 0) return parent;
    }
    return nullptr;
}

HWND BrowserTab(IDispatch* dispatch) {
    IServiceProvider* provider = nullptr;
    IShellBrowser* browser = nullptr;
    HWND tab = nullptr;
    if (SUCCEEDED(dispatch->QueryInterface(IID_IServiceProvider,
            reinterpret_cast<void**>(&provider)))) {
        if (SUCCEEDED(provider->QueryService(SID_STopLevelBrowser,
                IID_IShellBrowser, reinterpret_cast<void**>(&browser)))) {
            HWND window = nullptr;
            browser->GetWindow(&window);
            wchar_t name[80] = {};
            if (window && GetClassNameW(window, name, ARRAYSIZE(name)) &&
                !wcscmp(name, L"ShellTabWindowClass")) tab = window;
            if (!tab) {
                IShellView* view = nullptr;
                if (SUCCEEDED(browser->QueryActiveShellView(&view))) {
                    view->GetWindow(&window);
                    tab = FindTabWindow(window);
                    view->Release();
                }
            }
            browser->Release();
        }
        provider->Release();
    }
    return tab;
}

void RetireNativeRestorationPermit() {
    auto& session = Context();
    CoTaskMemFree(std::exchange(session.nativeRestorationExpected, nullptr));
    CoTaskMemFree(std::exchange(session.nativeRestorationRoot, nullptr));
    session.nativeRestorationRootSerial = 0;
    session.nativeRestorationNavigation = 0;
    session.nativeRestorationConnection = 0;
    session.nativeRestorationInputEpoch = 0;
    session.nativeRestorationObserver = 0;
    session.nativeRestorationStarted = 0;
    session.nativeRestorationViewIdentity = nullptr;
    session.nativeRestorationWindow = nullptr;
}

PIDLIST_ABSOLUTE NativeRestorationChild(PCIDLIST_ABSOLUTE root, PCIDLIST_ABSOLUTE previous) {
    if (!root || !previous || !ILIsParent(root, previous, FALSE)) return nullptr;
    PIDLIST_ABSOLUTE computer = nullptr;
    HRESULT hr = SHParseDisplayName(L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}",
        nullptr, &computer, 0, nullptr);
    bool isComputer = SUCCEEDED(hr) && computer && ILIsEqual(root, computer);
    CoTaskMemFree(computer);
    if (!isComputer) return nullptr;
    PCUIDLIST_RELATIVE child = ILFindChild(const_cast<PIDLIST_ABSOLUTE>(root), previous);
    PITEMID_CHILD first = child ? ILCloneFirst(child) : nullptr;
    PIDLIST_ABSOLUTE expected = first ? ILCombine(root, first) : nullptr;
    CoTaskMemFree(first);
    if (expected && !ILIsParent(root, expected, TRUE)) {
        CoTaskMemFree(expected);
        expected = nullptr;
    }
    return expected;
}

void ArmNativeRestorationPermit(PIDLIST_ABSOLUTE expected,
        ULONG_PTR inputEpoch = g_nativeInputEpoch.load(), ULONGLONG started = GetTickCount64()) {
    RetireNativeRestorationPermit();
    auto& session = Context();
    if (!expected || session.closed || session.nativeNavigationInProgress ||
            !session.columnCount || !session.nativeLocationIdentity) {
        CoTaskMemFree(expected);
        return;
    }
    if (g_nativeInputEpoch.load() != inputEpoch) {
        session.nativeRestorationBlockedSerial = session.panes[0].navigationSerial;
        session.nativeSelectionYielded = true;
        CoTaskMemFree(expected);
        return;
    }
    session.nativeRestorationRoot = ILCloneFull(session.nativeLocationIdentity);
    if (!session.nativeRestorationRoot || !ILIsParent(session.nativeLocationIdentity, expected, TRUE)) {
        session.nativeRestorationBlockedSerial = session.panes[0].navigationSerial;
        session.nativeSelectionYielded = true;
        CoTaskMemFree(expected);
        CoTaskMemFree(std::exchange(session.nativeRestorationRoot, nullptr));
        return;
    }
    session.nativeRestorationExpected = expected;
    session.nativeRestorationRootSerial = session.panes[0].navigationSerial;
    session.nativeRestorationNavigation = session.nativeNavigationGeneration;
    session.nativeRestorationConnection = session.nativeConnectionGeneration;
    session.nativeRestorationInputEpoch = inputEpoch;
    session.nativeRestorationStarted = started;
    Wh_Log(L"[Columns] Parent selection restoration armed; root serial=%llu",
        static_cast<unsigned long long>(session.nativeRestorationRootSerial));
}

void BindNativeRestorationPermit(IUnknown* identity, HWND window) {
    auto& session = Context();
    if (!session.nativeRestorationExpected) return;
    if (session.closed || !session.columnCount ||
            session.panes[0].navigationSerial != session.nativeRestorationRootSerial ||
            session.nativeNavigationGeneration != session.nativeRestorationNavigation ||
            session.nativeConnectionGeneration != session.nativeRestorationConnection ||
            g_nativeInputEpoch.load() != session.nativeRestorationInputEpoch ||
            GetTickCount64() - session.nativeRestorationStarted > 2000 ||
            !session.nativeLocationIdentity || !session.nativeRestorationRoot ||
            !ILIsEqual(session.nativeLocationIdentity, session.nativeRestorationRoot) ||
            (session.nativeRestorationViewIdentity &&
                (session.nativeRestorationViewIdentity != identity || session.nativeRestorationWindow != window))) {
        session.nativeRestorationBlockedSerial = session.nativeRestorationRootSerial;
        if (session.columnCount && session.panes[0].navigationSerial == session.nativeRestorationRootSerial)
            session.nativeSelectionYielded = true;
        RetireNativeRestorationPermit();
        return;
    }
    session.nativeRestorationViewIdentity = identity;
    session.nativeRestorationWindow = window;
    session.nativeRestorationObserver = session.nativeSelectionGeneration;
}

bool ConsumeNativeRestorationPermit(IShellView* view, IUnknown* identity, HWND window,
    ULONG_PTR* inputEpoch, PIDLIST_ABSOLUTE* verifiedRoot);
bool NativeRestorationRootMatches(PCIDLIST_ABSOLUTE root);

HRESULT NativeViewWindow(IShellView* view, HWND* window) {
    *window = nullptr;
    HRESULT hr = view->GetWindow(window);
    if (SUCCEEDED(hr) && (!IsWindow(*window) || !IsChild(Context().tab, *window) ||
            SessionForWindow(*window))) hr = HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);
    return hr;
}

HWND FindOriginalView(HWND root) {
    // During a Shell view replacement, the departing DefView can still be the
    // largest child. Once the browser is cached, only its active view is valid.
    if (g_currentSession && root == Context().tab && Context().nativeBrowser) {
        auto& session = Context();
        if (session.closed || session.nativeBrowserThread != GetCurrentThreadId()) return nullptr;
        IShellBrowser* browser = session.nativeBrowser;
        browser->AddRef();
        ULONG_PTR connection = session.nativeConnectionGeneration;
        ULONG_PTR navigation = session.nativeNavigationGeneration;
        IShellView* view = nullptr;
        HWND window = nullptr;
        HRESULT hr = browser->QueryActiveShellView(&view);
        if (SUCCEEDED(hr)) hr = view ? NativeViewWindow(view, &window) : E_NOINTERFACE;
        if (view) view->Release();
        browser->Release();
        if (FAILED(hr) || session.closed || session.nativeBrowser != browser ||
                session.nativeConnectionGeneration != connection ||
                session.nativeNavigationGeneration != navigation) return nullptr;
        return window;
    }
    // Bootstrap discovery before the tab's native browser has been cached.
    struct Search {
        HWND view = nullptr;
        LONG_PTR area = 0;
    } search;
    EnumChildWindows(root, [](HWND hwnd, LPARAM value) -> BOOL {
        auto* search = reinterpret_cast<Search*>(value);
        if (SessionForWindow(hwnd))
            return TRUE;
        wchar_t name[64] = {};
        if (!GetClassNameW(hwnd, name, ARRAYSIZE(name)) ||
            wcscmp(name, L"SHELLDLL_DefView") != 0) return TRUE;
        RECT rect = {};
        if (!GetClientRect(hwnd, &rect)) return TRUE;
        LONG_PTR area = static_cast<LONG_PTR>(rect.right - rect.left) *
                        (rect.bottom - rect.top);
        if (area > search->area) {
            search->area = area;
            search->view = hwnd;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&search));
    return search.view;
}

bool ViewRectInTab(HWND fileView, HWND tab, RECT* result, bool allowEmpty = false) {
    if (!IsWindow(fileView) || !IsWindow(tab) ||
        !IsChild(tab, fileView) || SessionForWindow(fileView) ||
        !GetWindowRect(fileView, result)) return false;
    MapWindowPoints(nullptr, tab, reinterpret_cast<POINT*>(result), 2);
    RECT client = {}, clipped = {};
    if (!GetClientRect(tab, &client)) return false;
    bool nonempty = IntersectRect(&clipped, result, &client) != FALSE;
    *result = clipped;
    return allowEmpty || nonempty;
}

void DisconnectNativeSelectionEvents() {
    // The first binding belongs to the new root; a later disconnect retires it.
    if (Context().nativeRestorationViewIdentity) {
        Context().nativeRestorationBlockedSerial = Context().nativeRestorationRootSerial;
        RetireNativeRestorationPermit();
    }
    Context().nativeSelectionGuarded = false;
    ++Context().nativeSelectionGeneration;
    auto* connection = std::exchange(Context().nativeSelectionConnection, nullptr);
    DWORD cookie = std::exchange(Context().nativeSelectionCookie, 0);
    auto* identity = std::exchange(Context().nativeSelectionIdentity, nullptr);
    Context().nativeSelectionWindow = nullptr;
    if (connection) {
        if (cookie) connection->Unadvise(cookie);
        connection->Release();
    }
    if (identity) identity->Release();
}

HRESULT ConnectNativeSelectionEvents(IShellView* view, IUnknown* identity, HWND window) {
    if (Context().nativeSelectionIdentity == identity && Context().nativeSelectionWindow == window &&
        Context().nativeSelectionConnection && Context().nativeSelectionCookie) {
        BindNativeRestorationPermit(identity, window);
        return S_OK;
    }
    bool replaced = Context().nativeSelectionIdentity &&
        (Context().nativeSelectionIdentity != identity || Context().nativeSelectionWindow != window);
    DisconnectNativeSelectionEvents();
    // Protection belongs to the observed view. Retire it only for a confirmed
    // replacement, before Advise can deliver genuine events from the new view.
    if (replaced) {
        Wh_Log(L"[Columns] Native selection observer replaced; prior yield=%d root pending=%d",
            Context().nativeSelectionYielded, Context().rootNavigationPending);
        Context().nativeSelectionYielded = false;
    }
    ULONG_PTR generation = Context().nativeSelectionGeneration;
    IDispatch* automation = nullptr;
    IConnectionPointContainer* container = nullptr;
    IConnectionPoint* connection = nullptr;
    DWORD cookie = 0;
    HRESULT hr = view->GetItemObject(SVGIO_BACKGROUND, IID_IDispatch,
        reinterpret_cast<void**>(&automation));
    if (SUCCEEDED(hr) && automation)
        hr = automation->QueryInterface(IID_IConnectionPointContainer,
            reinterpret_cast<void**>(&container));
    if (SUCCEEDED(hr) && container)
        hr = container->FindConnectionPoint(DIID_DShellFolderViewEvents, &connection);
    if (automation) automation->Release();
    if (container) container->Release();
    if (SUCCEEDED(hr) && connection) {
        auto* sink = new (std::nothrow) NavigationSink(Context().weak_from_this(), true);
        hr = sink ? connection->Advise(static_cast<IDispatch*>(sink), &cookie) : E_OUTOFMEMORY;
        if (sink) sink->Release();
    }
    if (SUCCEEDED(hr) && (!connection || !cookie || Context().closed ||
            generation != Context().nativeSelectionGeneration)) hr = E_ABORT;
    if (SUCCEEDED(hr)) {
        Context().nativeSelectionConnection = connection;
        Context().nativeSelectionCookie = cookie;
        identity->AddRef();
        Context().nativeSelectionIdentity = identity;
        Context().nativeSelectionWindow = window;
        BindNativeRestorationPermit(identity, window);
    } else if (connection) {
        if (cookie) connection->Unadvise(cookie);
        connection->Release();
    }
    return hr;
}

HRESULT NativeViewSelectionIsEmpty(IShellView* view, HWND expectedView, int* selectionCount = nullptr) {
    if (selectionCount) *selectionCount = -1;
    HWND window = nullptr;
    HRESULT hr = NativeViewWindow(view, &window);
    if (SUCCEEDED(hr) && window != expectedView)
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);
    IFolderView* folderView = nullptr;
    if (SUCCEEDED(hr)) hr = view->QueryInterface(IID_IFolderView,
        reinterpret_cast<void**>(&folderView));
    int selected = -1;
    if (SUCCEEDED(hr) && folderView) hr = folderView->ItemCount(SVGIO_SELECTION, &selected);
    if (folderView) folderView->Release();
    if (SUCCEEDED(hr) && selectionCount) *selectionCount = selected;
    if (SUCCEEDED(hr) && selected != 0) {
        Wh_Log(L"[Columns] Native view has selection; count=%d root pending=%d navigation=%d",
            selected, Context().rootNavigationPending, Context().nativeNavigationInProgress);
        hr = E_ABORT;
    }
    return hr;
}

bool IsColumnTextInput(HWND window);

bool ClearNativeViewSelection(HWND fileView) {
    auto& session = Context();
    session.nativeSelectionGuarded = false;
    if (session.closed || session.nativeSelectionChecking || session.nativeNavigationInProgress ||
        !session.nativeBrowser || session.nativeBrowserThread != GetCurrentThreadId()) return false;
    session.nativeSelectionChecking = true;
    struct SelectionCheckScope {
        Session& session;
        ~SelectionCheckScope() { session.nativeSelectionChecking = false; }
    } checking{session};
    IShellBrowser* browser = session.nativeBrowser;
    browser->AddRef(); // Shell calls can pump messages and replace the cached browser.
    ULONG_PTR generation = session.nativeConnectionGeneration;
    ULONG_PTR navigationGeneration = session.nativeNavigationGeneration;
    IShellView* view = nullptr;
    IUnknown* identity = nullptr;
    HRESULT hr = browser->QueryActiveShellView(&view);
    if (SUCCEEDED(hr) && view) hr = view->QueryInterface(IID_IUnknown,
        reinterpret_cast<void**>(&identity));
    if (SUCCEEDED(hr) && (!view || !identity)) hr = E_NOINTERFACE;
    HWND window = nullptr;
    if (SUCCEEDED(hr)) hr = NativeViewWindow(view, &window);
    // Verify the target before mutating selection, not only after deselection.
    if (SUCCEEDED(hr) && window != fileView) {
        Wh_Log(L"[Columns] Native takeover deferred for view replacement; expected=%p active=%p",
            fileView, window);
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);
    }
    if (SUCCEEDED(hr) && (session.closed || session.nativeNavigationInProgress ||
            session.nativeNavigationGeneration != navigationGeneration ||
            session.nativeConnectionGeneration != generation || session.nativeBrowser != browser))
        hr = E_ABORT;
    if (SUCCEEDED(hr)) hr = ConnectNativeSelectionEvents(view, identity, window);
    if (SUCCEEDED(hr) && (session.closed || session.nativeNavigationInProgress ||
            session.nativeNavigationGeneration != navigationGeneration ||
            session.nativeConnectionGeneration != generation || session.nativeBrowser != browser ||
            session.nativeSelectionIdentity != identity || session.nativeSelectionWindow != window))
        hr = E_ABORT;
    HWND focused = GetFocus();
    if (SUCCEEDED(hr) && focused && IsChild(fileView, focused) && IsColumnTextInput(focused)) {
        session.nativeSelectionYielded = true;
        hr = E_ABORT;
    }
    // Qualifying This PC roots must also protect a deliberate selection whose
    // asynchronous notification has not yet reached this observer.
    bool skipEmptyRestorationClear = false;
    bool permitHasSelection = false;
    if (session.columnCount && session.nativeRestorationBlockedSerial == session.panes[0].navigationSerial)
        session.nativeSelectionYielded = true;
    if (SUCCEEDED(hr) && session.nativeRestorationExpected) {
        int selected = -1;
        HRESULT selectionHr = NativeViewSelectionIsEmpty(view, fileView, &selected);
        if (selectionHr == E_ABORT && selected > 0) {
            session.nativeSelectionYielded = true;
            permitHasSelection = selected == 1;
            if (!permitHasSelection) hr = selectionHr;
        } else if (FAILED(selectionHr)) hr = selectionHr;
        else skipEmptyRestorationClear = true;
    }
    ULONG_PTR restorationEpoch = 0;
    ULONG_PTR restorationSerial = session.nativeRestorationRootSerial;
    PIDLIST_ABSOLUTE verifiedRestorationRoot = nullptr;
    bool restoredParent = SUCCEEDED(hr) && session.nativeSelectionYielded && permitHasSelection &&
        ConsumeNativeRestorationPermit(view, identity, window, &restorationEpoch, &verifiedRestorationRoot);
    auto restorationRoot = std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)>(
        verifiedRestorationRoot, CoTaskMemFree);
    if (restoredParent) session.nativeSelectionYielded = false;
    // Initial takeover may clear history selection. A yielded command remains
    // protected, except for the one exact ancestor child just proved above.
    focused = GetFocus();
    if (SUCCEEDED(hr) && ((focused && IsChild(fileView, focused) && IsColumnTextInput(focused)) ||
            (restoredParent && (g_nativeInputEpoch.load() != restorationEpoch ||
                !session.columnCount || session.panes[0].navigationSerial != restorationSerial)))) {
        session.nativeSelectionYielded = true;
        hr = E_ABORT;
    }
    if (SUCCEEDED(hr) && !session.nativeSelectionYielded && !skipEmptyRestorationClear) {
        session.nativeSelectionClearing = true;
        hr = view->SelectItem(nullptr, SVSI_DESELECTOTHERS);
        session.nativeSelectionClearing = false;
    }
    ULONG_PTR selectionSerial = session.nativeSelectionChangeSerial;
    if (SUCCEEDED(hr)) {
        hr = NativeViewSelectionIsEmpty(view, fileView);
        if (hr == E_ABORT) session.nativeSelectionYielded = true;
    }
    IShellView* current = nullptr;
    IUnknown* currentIdentity = nullptr;
    if (SUCCEEDED(hr)) hr = browser->QueryActiveShellView(&current);
    if (SUCCEEDED(hr) && current) hr = current->QueryInterface(IID_IUnknown,
        reinterpret_cast<void**>(&currentIdentity));
    if (SUCCEEDED(hr) && (!currentIdentity || currentIdentity != identity ||
            session.closed || session.nativeNavigationInProgress ||
            session.nativeNavigationGeneration != navigationGeneration ||
            session.nativeConnectionGeneration != generation || session.nativeBrowser != browser))
        hr = E_ABORT;
    if (currentIdentity) currentIdentity->Release();
    if (current) current->Release();
    if (identity) identity->Release();
    if (view) view->Release();
    browser->Release();
    if (restoredParent && !NativeRestorationRootMatches(restorationRoot.get())) {
        session.nativeSelectionYielded = true;
        hr = E_ABORT;
    }
    // COM queries/releases may pump Shell notifications. A restored selection
    // after the count query invalidates the result even if the view is the same.
    if (SUCCEEDED(hr) && (session.nativeSelectionChangeSerial != selectionSerial ||
            session.nativeConnectionGeneration != generation || session.nativeBrowser != browser ||
            session.nativeNavigationGeneration != navigationGeneration ||
            session.closed || session.nativeNavigationInProgress)) hr = E_ABORT;
    if (restoredParent && (g_nativeInputEpoch.load() != restorationEpoch ||
            !session.columnCount || session.panes[0].navigationSerial != restorationSerial)) {
        session.nativeSelectionYielded = true;
        hr = E_ABORT;
    }
    // A native command can start rename while the final COM queries pump.
    // Even an empty selection must not let the overlay cover its new editor.
    focused = GetFocus();
    if (SUCCEEDED(hr) && focused && IsChild(fileView, focused) && IsColumnTextInput(focused)) {
        session.nativeSelectionYielded = true;
        hr = E_ABORT;
    }
    if (FAILED(hr)) {
        if (restoredParent) session.nativeSelectionYielded = true;
        if (session.nativeRestorationExpected) {
            session.nativeRestorationBlockedSerial = session.nativeRestorationRootSerial;
            session.nativeSelectionYielded = true;
        }
        RetireNativeRestorationPermit();
        LogHr(L"Native selection safety check; retaining native view", hr);
    } else {
        session.nativeViewRetryAttempts = 0;
        session.nativeSelectionYielded = false;
    }
    return SUCCEEDED(hr);
}

void RestoreOriginalView() {
    // Disable DirectUI show suppression before restoring it (ShowWindow can reenter).
    Context().nativeSelectionGuarded = false;
    if (Context().originalDrawingSurface && Context().originalWasVisible &&
        IsWindow(Context().originalDrawingSurface)) {
        ShowWindow(Context().originalDrawingSurface, SW_SHOWNA);
    }
    Context().originalDrawingSurface = nullptr;
    Context().originalWasVisible = false;
}

bool HideOriginalView(HWND fileView) {
    HWND drawingSurface = IsWindow(fileView) ?
        FindWindowExW(fileView, nullptr, L"DirectUIHWND", nullptr) : nullptr;
    if (drawingSurface != Context().originalDrawingSurface) {
        RestoreOriginalView();
        Context().originalDrawingSurface = drawingSurface;
        // Preserve this surface's own visibility, even if its tab/ancestor is
        // currently hidden. Effective visibility would lose the restore state
        // when a background tab finishes its root navigation.
        Context().originalWasVisible = drawingSurface &&
            (GetWindowLongPtrW(drawingSurface, GWL_STYLE) & WS_VISIBLE) != 0;
    }
    // The frame command bar still belongs to this native view. Never cover a
    // selection that its Cut/Delete/Rename commands could act on invisibly.
    // Observe late selection restoration as well as checking this instant.
    if (!drawingSurface || !ClearNativeViewSelection(fileView)) return false;
    Context().nativeSelectionGuarded = true;
    if (drawingSurface && (GetWindowLongPtrW(drawingSurface, GWL_STYLE) & WS_VISIBLE))
        ShowWindow(drawingSurface, SW_HIDE);
    // Native window processing can restore a selection or start navigation.
    return Context().nativeSelectionGuarded && !Context().suspended &&
        !Context().closed && Context().originalDrawingSurface == drawingSurface;
}

bool IsShellFolderPath(const std::wstring& path) {
    IShellItem* item = nullptr;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr,
            IID_IShellItem, reinterpret_cast<void**>(&item)))) return false;
    SFGAOF attributes = 0;
    HRESULT hr = item->GetAttributes(SFGAO_FOLDER, &attributes);
    item->Release();
    return SUCCEEDED(hr) && (attributes & SFGAO_FOLDER);
}

enum class NativeLocationKind { Supported, Unsupported, TransientFailure };
struct NativeLocation {
    NativeLocationKind kind = NativeLocationKind::TransientFailure;
    HRESULT result = E_FAIL;
    bool inputSnapshotValid = false;
    ULONG_PTR inputEpoch = 0;
    ULONGLONG inputObservedAt = 0;
    std::wstring path;
    std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)> identity{nullptr, CoTaskMemFree};
};

void DisconnectNavigationEvents() {
    auto* connection = std::exchange(Context().navigationConnection, nullptr);
    DWORD cookie = std::exchange(Context().navigationCookie, 0);
    Context().navigationUpdatePosted = false;
    if (connection) {
        if (cookie) connection->Unadvise(cookie);
        connection->Release();
    }
}

void ResetNativeTabConnection() {
    RetireNativeRestorationPermit();
    ++Context().nativeConnectionGeneration;
    auto* browser = std::exchange(Context().nativeBrowser, nullptr);
    auto* automation = std::exchange(Context().nativeAutomation, nullptr);
    Context().nativeBrowserThread = 0;
    DisconnectNativeSelectionEvents();
    Context().nativeNavigationInProgress = false;
    DisconnectNavigationEvents();
    if (browser) browser->Release();
    if (automation) automation->Release();
}

bool CacheNativeTabBrowser(HWND explorer, HWND tab) {
    if (!IsWindow(tab) || GetWindowThreadProcessId(tab, nullptr) != GetCurrentThreadId()) return false;
    if (Context().nativeBrowser && Context().nativeAutomation &&
        Context().nativeBrowserThread == GetCurrentThreadId() &&
        BrowserTab(Context().nativeAutomation) == tab) return true;
    ResetNativeTabConnection();
    IShellWindows* windows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER,
        IID_IShellWindows, reinterpret_cast<void**>(&windows));
    if (FAILED(hr)) { LogHr(L"Native tab discovery: ShellWindows", hr); return false; }
    long count = 0;
    hr = windows->get_Count(&count);
    for (long i = 0; SUCCEEDED(hr) && i < count; ++i) {
        VARIANT index = {};
        index.vt = VT_I4;
        index.lVal = i;
        IDispatch* dispatch = nullptr;
        if (FAILED(windows->Item(index, &dispatch)) || !dispatch) continue;
        IWebBrowser2* automation = nullptr;
        IServiceProvider* provider = nullptr;
        IShellBrowser* browser = nullptr;
        SHANDLE_PTR frame = 0;
        bool matching = SUCCEEDED(dispatch->QueryInterface(IID_IWebBrowser2,
            reinterpret_cast<void**>(&automation))) &&
            SUCCEEDED(automation->get_HWND(&frame)) &&
            reinterpret_cast<HWND>(static_cast<LONG_PTR>(frame)) == explorer &&
            BrowserTab(dispatch) == tab &&
            SUCCEEDED(dispatch->QueryInterface(IID_IServiceProvider,
                reinterpret_cast<void**>(&provider))) &&
            SUCCEEDED(provider->QueryService(SID_STopLevelBrowser, IID_IShellBrowser,
                reinterpret_cast<void**>(&browser)));
        if (provider) provider->Release();
        dispatch->Release();
        if (matching) {
            Context().nativeAutomation = automation;
            Context().nativeBrowser = browser;
            Context().nativeBrowserThread = GetCurrentThreadId();
            break;
        }
        if (browser) browser->Release();
        if (automation) automation->Release();
    }
    windows->Release();
    return Context().nativeBrowser != nullptr;
}

// Explorer's native opennewtab verb asks the embedded browser to BrowseObject
// with this tab-disposition bit. Forward the native request to its real frame;
// OnNavigationPending has only a PIDL and cannot recover this disposition.
// This native bit is restricted to the same verified Windows build families
// as the modern menu contract. No private method RVA or vtable replacement.
constexpr UINT kNativeNewTabBrowse = 0x00000400;
using ColumnBrowseObjectFn = HRESULT (STDMETHODCALLTYPE*)(IShellBrowser*, PCUIDLIST_RELATIVE, UINT);
ColumnBrowseObjectFn g_columnBrowseOriginal = nullptr;
HMODULE g_columnBrowseModule = nullptr;

HRESULT STDMETHODCALLTYPE ColumnBrowseObjectHook(IShellBrowser* browser,
    PCUIDLIST_RELATIVE target, UINT flags) {
    CallbackScope callbackScope;
    if (g_unloading.load() || !IsSupportedMenuPlatform() ||
        !(flags & kNativeNewTabBrowse) || !target ||
        (flags & (SBSP_RELATIVE | SBSP_PARENT | SBSP_NAVIGATEBACK | SBSP_NAVIGATEFORWARD)))
        return g_columnBrowseOriginal(browser, target, flags);

    std::shared_ptr<Session> owner;
    int index = -1;
    DWORD thread = GetCurrentThreadId();
    AcquireSRWLockShared(&g_sessionsLock);
    for (const auto& entry : g_sessions) {
        // Plain Session/Pane state belongs to this UI thread. Other Explorer
        // windows or other embedded browsers must retain their original route.
        if (GetWindowThreadProcessId(entry.tab, nullptr) != thread || entry.owner->closed)
            continue;
        for (int i = 0; i < entry.owner->columnCount; ++i) {
            const Pane& pane = entry.owner->panes[i];
            if (pane.generation && pane.initialized && pane.navigationBrowser == browser) {
                owner = entry.owner;
                index = i;
                break;
            }
        }
        if (owner) break;
    }
    ReleaseSRWLockShared(&g_sessionsLock);
    if (!owner) return g_columnBrowseOriginal(browser, target, flags);

    SessionScope scope(owner);
    IShellBrowser* native = owner->nativeBrowser;
    IWebBrowser2* automation = owner->nativeAutomation;
    if (!native || native == browser || !automation || owner->nativeBrowserThread != thread)
        return g_columnBrowseOriginal(browser, target, flags);
    browser->AddRef();
    native->AddRef();
    automation->AddRef();
    struct BrowserRefs {
        IShellBrowser* source;
        IShellBrowser* destination;
        IWebBrowser2* automation;
        ~BrowserRefs() { automation->Release(); destination->Release(); source->Release(); }
    } refs{browser, native, automation};
    ULONG_PTR generation = owner->panes[index].generation;
    ULONG_PTR connection = owner->nativeConnectionGeneration;
    HWND tab = owner->tab;
    PIDLIST_ABSOLUTE absolute = ILCloneFull(target);
    if (!absolute) return E_OUTOFMEMORY;
    std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)> keepTarget(absolute, CoTaskMemFree);
    bool matchingTab = BrowserTab(automation) == tab;
    // BrowserTab can call COM and pump messages. Re-check the logical owners
    // after it returns, retaining every COM reference across that validation.
    if (!matchingTab || g_unloading.load() || owner->closed ||
        !IsWindow(tab) || owner->nativeConnectionGeneration != connection ||
        owner->nativeBrowser != native || owner->nativeAutomation != automation ||
        owner->panes[index].generation != generation ||
        owner->panes[index].navigationBrowser != browser ||
        SessionForTab(tab).get() != owner.get())
        return g_columnBrowseOriginal(browser, target, flags);
    HRESULT hr = native->BrowseObject(absolute, flags);
    Wh_Log(L"[Columns new tab] v%s tab=%p pane=%d flags=%08X result=%08X",
        WH_MOD_VERSION, tab, index, flags, static_cast<unsigned>(hr));
    // Never retry in the embedded browser after native dispatch; its caller
    // must receive the exact native HRESULT and choose any fallback itself.
    return hr;
}

struct ColumnBrowseProbe {
    void* method = nullptr;
    HMODULE module = nullptr;
    HRESULT result = E_FAIL;
};

DWORD WINAPI ResolveColumnBrowseEntry(void* argument) {
    auto& probe = *static_cast<ColumnBrowseProbe*>(argument);
    HRESULT apartment = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    probe.result = apartment;
    if (FAILED(apartment)) return 0;
    IExplorerBrowser* browser = nullptr;
    IShellBrowser* navigation = nullptr;
    probe.result = CoCreateInstance(CLSID_ExplorerBrowser, nullptr, CLSCTX_INPROC_SERVER,
        IID_IExplorerBrowser, reinterpret_cast<void**>(&browser));
    if (SUCCEEDED(probe.result))
        probe.result = browser->QueryInterface(IID_IShellBrowser,
            reinterpret_cast<void**>(&navigation));
    if (SUCCEEDED(probe.result)) {
        // Slot 11 is BrowseObject in the public IShellBrowser COM ABI.
        // Discover it from a real class instance; do not patch its vtable.
        probe.method = (*reinterpret_cast<void***>(navigation))[11];
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                reinterpret_cast<PCWSTR>(probe.method), &probe.module)) {
            probe.result = HRESULT_FROM_WIN32(GetLastError());
            probe.method = nullptr;
        }
    }
    if (navigation) navigation->Release();
    if (browser) browser->Release();
    CoUninitialize();
    return 0;
}

bool RegisterColumnBrowseHook() {
    if (!IsSupportedMenuPlatform()) return true;
    ColumnBrowseProbe probe;
    // Discover the COM entry in a private STA, without creating a view or
    // enumerating a user folder. Join it fully before registering from ModInit.
    HANDLE thread = CreateThread(nullptr, 0, ResolveColumnBrowseEntry, &probe, 0, nullptr);
    if (!thread) return false;
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    bool registered = SUCCEEDED(probe.result) && probe.method && probe.module &&
        WindhawkUtils::SetFunctionHook(reinterpret_cast<ColumnBrowseObjectFn>(probe.method),
            ColumnBrowseObjectHook, &g_columnBrowseOriginal);
    if (!registered) {
        if (probe.module) FreeLibrary(probe.module);
        Wh_Log(L"[Columns new tab] Hook initialization failed: %08X", static_cast<unsigned>(probe.result));
        return false;
    }
    g_columnBrowseModule = probe.module; // Retain until engine hook removal and Uninit drain.
    return true;
}

bool IsAbsoluteColumnPath(const std::wstring& path) {
    return (path.size() >= 3 && iswalpha(path[0]) && path[1] == L':' && path[2] == L'\\') ||
        (path.size() >= 2 && path[0] == L'\\' && path[1] == L'\\');
}

bool IsColumnNamespaceRoot(PCIDLIST_ABSOLUTE pidl) {
    // Keep the supported root scope explicit. These are namespace identities,
    // never localized display names. Older systems may expose Quick Access.
    const wchar_t* roots[] = {
        L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}", // This PC
        L"::{F874310E-B6B7-47DC-BC84-B9E6B38F5903}", // Windows 11 Home
        L"::{679F85CB-0220-4080-B29B-5540CC05AAB6}"  // Quick Access, where available
    };
    for (const auto* root : roots) {
        PIDLIST_ABSOLUTE candidate = nullptr;
        HRESULT hr = SHParseDisplayName(root, nullptr, &candidate, 0, nullptr);
        bool equal = SUCCEEDED(hr) && candidate && ILIsEqual(pidl, candidate);
        CoTaskMemFree(candidate);
        if (equal) return true;
    }
    return false;
}

NativeLocation ClassifyNativeLocation(PIDLIST_ABSOLUTE pidl) {
    NativeLocation location;
    location.identity.reset(pidl); // Takes the active folder's caller-owned PIDL.
    if (!pidl) return location;
    IShellItem* item = nullptr;
    location.result = SHCreateItemFromIDList(pidl, IID_IShellItem,
        reinterpret_cast<void**>(&item));
    if (FAILED(location.result) || !item) return location;
    SFGAOF attributes = 0;
    location.result = item->GetAttributes(SFGAO_FOLDER, &attributes);
    PWSTR parsing = nullptr;
    if (SUCCEEDED(location.result)) {
        location.result = item->GetDisplayName(SIGDN_FILESYSPATH, &parsing);
        if (FAILED(location.result)) {
            CoTaskMemFree(parsing);
            parsing = nullptr;
            location.result = item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &parsing);
        }
        if (SUCCEEDED(location.result) && parsing) {
            if ((attributes & SFGAO_FOLDER) &&
                (IsAbsoluteColumnPath(parsing) || IsColumnNamespaceRoot(pidl))) {
                location.kind = NativeLocationKind::Supported;
                location.path = parsing;
            } else location.kind = NativeLocationKind::Unsupported;
        }
    }
    CoTaskMemFree(parsing);
    item->Release();
    return location;
}

NativeLocation ReadNativeLocation() {
    NativeLocation location;
    ULONG_PTR inputEpoch = g_nativeInputEpoch.load();
    ULONGLONG inputObservedAt = GetTickCount64();
    if (!Context().nativeBrowser || Context().nativeBrowserThread != GetCurrentThreadId())
        return location;
    IShellView* shell = nullptr;
    location.result = Context().nativeBrowser->QueryActiveShellView(&shell);
    IFolderView* view = nullptr;
    if (SUCCEEDED(location.result) && shell)
        location.result = shell->QueryInterface(IID_IFolderView, reinterpret_cast<void**>(&view));
    if (shell) shell->Release();
    IPersistFolder2* folder = nullptr;
    if (SUCCEEDED(location.result) && view)
        location.result = view->GetFolder(IID_IPersistFolder2, reinterpret_cast<void**>(&folder));
    if (view) view->Release();
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(location.result) && folder) location.result = folder->GetCurFolder(&pidl);
    if (folder) folder->Release();
    if (SUCCEEDED(location.result) && pidl) {
        NativeLocation classified = ClassifyNativeLocation(pidl);
        classified.inputSnapshotValid = true;
        classified.inputEpoch = inputEpoch;
        classified.inputObservedAt = inputObservedAt;
        return classified;
    }
    CoTaskMemFree(pidl);
    // A failed query is not a virtual location or a deselection. Keep the last
    // valid UI while the native navigation settles, then retry asynchronously.
    return location;
}

bool ConsumeNativeRestorationPermit(IShellView* view, IUnknown* identity, HWND window,
        ULONG_PTR* inputEpoch, PIDLIST_ABSOLUTE* verifiedRoot) {
    auto& session = Context();
    if (!session.nativeRestorationExpected) return false;
    // Consume before COM: nested selection notifications cannot spend it twice.
    auto expected = std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)>(
        std::exchange(session.nativeRestorationExpected, nullptr), CoTaskMemFree);
    auto root = std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)>(
        std::exchange(session.nativeRestorationRoot, nullptr), CoTaskMemFree);
    ULONG_PTR serial = session.nativeRestorationRootSerial;
    ULONG_PTR navigation = session.nativeRestorationNavigation;
    ULONG_PTR connection = session.nativeRestorationConnection;
    ULONG_PTR observer = session.nativeRestorationObserver;
    ULONG_PTR epoch = session.nativeRestorationInputEpoch;
    ULONGLONG started = session.nativeRestorationStarted;
    IUnknown* boundIdentity = session.nativeRestorationViewIdentity;
    HWND boundWindow = session.nativeRestorationWindow;
    IShellBrowser* browser = session.nativeBrowser;
    ULONG_PTR selectionSerial = session.nativeSelectionChangeSerial;
    session.nativeRestorationBlockedSerial = serial;
    RetireNativeRestorationPermit();
    auto current = [&] {
        return !session.closed && !session.nativeNavigationInProgress && session.columnCount &&
            session.panes[0].navigationSerial == serial &&
            session.nativeNavigationGeneration == navigation &&
            session.nativeConnectionGeneration == connection && session.nativeBrowser == browser &&
            session.nativeBrowserThread == GetCurrentThreadId() &&
            session.nativeSelectionGeneration == observer &&
            session.nativeSelectionChangeSerial == selectionSerial &&
            identity == boundIdentity && window == boundWindow &&
            session.nativeSelectionIdentity == identity && session.nativeSelectionWindow == window &&
            IsWindow(window) && IsChild(session.tab, window) && !SessionForWindow(window) &&
            root && session.nativeLocationIdentity && ILIsEqual(root.get(), session.nativeLocationIdentity) &&
            g_nativeInputEpoch.load() == epoch && GetTickCount64() - started <= 2000;
    };
    HWND focused = GetFocus();
    if (!current() || (focused && IsChild(window, focused) && IsColumnTextInput(focused))) {
        Wh_Log(L"[Columns] Parent selection restoration retired before query; input changed=%d expired=%d root serial changed=%d",
            g_nativeInputEpoch.load() != epoch, GetTickCount64() - started > 2000,
            !session.columnCount || session.panes[0].navigationSerial != serial);
        return false;
    }
    IFolderView2* folder = nullptr;
    IShellItemArray* selection = nullptr;
    IShellItem* item = nullptr;
    PIDLIST_ABSOLUTE selected = nullptr;
    HRESULT hr = view->QueryInterface(IID_IFolderView2, reinterpret_cast<void**>(&folder));
    if (SUCCEEDED(hr) && folder) hr = folder->GetSelection(FALSE, &selection);
    else if (SUCCEEDED(hr)) hr = E_NOINTERFACE;
    DWORD count = 0;
    if (SUCCEEDED(hr) && selection) hr = selection->GetCount(&count);
    else if (SUCCEEDED(hr)) hr = E_NOINTERFACE;
    if (SUCCEEDED(hr) && count == 1) hr = selection->GetItemAt(0, &item);
    else if (SUCCEEDED(hr)) hr = E_ABORT;
    if (SUCCEEDED(hr) && item) hr = SHGetIDListFromObject(item, &selected);
    else if (SUCCEEDED(hr)) hr = E_NOINTERFACE;
    bool matches = SUCCEEDED(hr) && selected && ILIsEqual(selected, expected.get());
    if (item) item->Release();
    if (selection) selection->Release();
    if (folder) folder->Release();
    // Re-read the actual root and active view after all selected-item queries.
    IShellView* active = nullptr;
    IUnknown* activeIdentity = nullptr;
    HWND activeWindow = nullptr;
    if (SUCCEEDED(hr)) hr = browser ? browser->QueryActiveShellView(&active) : E_NOINTERFACE;
    if (SUCCEEDED(hr) && active) hr = active->QueryInterface(IID_IUnknown,
        reinterpret_cast<void**>(&activeIdentity));
    else if (SUCCEEDED(hr)) hr = E_NOINTERFACE;
    if (SUCCEEDED(hr)) hr = NativeViewWindow(active, &activeWindow);
    bool sameView = SUCCEEDED(hr) && activeIdentity == identity && activeWindow == window;
    if (activeIdentity) activeIdentity->Release();
    if (active) active->Release();
    NativeLocation actual = ReadNativeLocation();
    focused = GetFocus();
    bool allowed = matches && sameView && actual.kind == NativeLocationKind::Supported &&
        actual.identity && ILIsEqual(actual.identity.get(), root.get()) && current() &&
        !(focused && IsChild(window, focused) && IsColumnTextInput(focused));
    Wh_Log(L"[Columns] Parent selection restoration %s; count=%u same item=%d same view=%d",
        allowed ? L"approved once" : L"retired", count, matches, sameView);
    if (allowed) {
        *inputEpoch = epoch;
        *verifiedRoot = root.release();
    } else {
        // Diagnostic names never participate in matching. Query them only after
        // denial, when this consumed attempt cannot mutate or cover the view.
        PWSTR selectedName = nullptr;
        PWSTR expectedName = nullptr;
        if (selected) SHGetNameFromIDList(selected, SIGDN_DESKTOPABSOLUTEPARSING, &selectedName);
        SHGetNameFromIDList(expected.get(), SIGDN_DESKTOPABSOLUTEPARSING, &expectedName);
        Wh_Log(L"[Columns] Parent restoration denied; selected=%s expected=%s",
            selectedName ? selectedName : L"(unavailable)", expectedName ? expectedName : L"(unavailable)");
        CoTaskMemFree(selectedName);
        CoTaskMemFree(expectedName);
    }
    CoTaskMemFree(selected);
    return allowed;
}

bool NativeRestorationRootMatches(PCIDLIST_ABSOLUTE root) {
    NativeLocation actual = ReadNativeLocation();
    return root && actual.kind == NativeLocationKind::Supported && actual.identity &&
        ILIsEqual(actual.identity.get(), root);
}

void ConnectNavigationEvents(HWND explorer) {
    if (!CacheNativeTabBrowser(explorer, Context().tab) || Context().navigationCookie) return;
    IConnectionPointContainer* container = nullptr;
    HRESULT hr = Context().nativeAutomation->QueryInterface(IID_IConnectionPointContainer,
        reinterpret_cast<void**>(&container));
    if (SUCCEEDED(hr)) {
        hr = container->FindConnectionPoint(DIID_DWebBrowserEvents2, &Context().navigationConnection);
        container->Release();
    }
    if (SUCCEEDED(hr)) {
        auto* sink = new (std::nothrow) NavigationSink(Context().weak_from_this());
        hr = sink ? Context().navigationConnection->Advise(static_cast<IDispatch*>(sink),
            &Context().navigationCookie) : E_OUTOFMEMORY;
        if (sink) sink->Release();
    }
    if (SUCCEEDED(hr)) Wh_Log(L"[Columns] Native tab location/events cached on UI thread %u",
        GetCurrentThreadId());
    else { LogHr(L"Navigation events: connect", hr); DisconnectNavigationEvents(); }
}

void ReleasePane(Pane& pane) {
    pane.generation = 0;
    pane.navigationSerial = 0;
    pane.selectionDirty = false;
    IShellBrowser* navigation = std::exchange(pane.navigationBrowser, nullptr);
    if (navigation) navigation->Release();
    if (pane.browserSite)
        static_cast<PaneBrowserSite*>(pane.browserSite)->Retire();
    if (pane.styledView) pane.styledView->Release();
    pane.styledView = nullptr;
    if (pane.browser) {
        if (pane.navigationCookie) {
            HRESULT hr = pane.browser->Unadvise(pane.navigationCookie);
            if (FAILED(hr)) LogHr(L"ExplorerBrowser::Unadvise", hr);
            pane.navigationCookie = 0;
        }
        if (pane.initialized) {
            HRESULT hr = pane.browser->Destroy();
            if (FAILED(hr)) LogHr(L"ExplorerBrowser::Destroy", hr);
        }
        IObjectWithSite* withSite = nullptr;
        if (SUCCEEDED(pane.browser->QueryInterface(IID_IObjectWithSite,
                reinterpret_cast<void**>(&withSite)))) {
            withSite->SetSite(nullptr);
            withSite->Release();
        }
        pane.browser->Release();
    }
    if (pane.browserSite) pane.browserSite->Release();
    pane.browserSite = nullptr;
    if (pane.navigationSink) pane.navigationSink->Release();
    ClearNavigationTarget(pane);
    pane.navigationSink = nullptr;
    pane.browser = nullptr;
    pane.initialized = false;
    pane.awaitingNavigationRepair = false;
    pane.lastSetRect = 0;
    pane.generation = 0;
    pane.selectionDirty = false;
    pane.path.clear();
    pane.window = nullptr;
    pane.viewHost = nullptr;
}

HRESULT BrowsePidl(Pane& pane, PCIDLIST_ABSOLUTE pidl, const std::wstring& path,
        ULONG_PTR* startedSerial = nullptr) {
    pane.navigationSerial = ++g_navigationSequence;
    if (startedSerial) *startedSerial = pane.navigationSerial;
    ClearNavigationTarget(pane);
    if (!pidl || !pane.browser) return E_INVALIDARG;
    pane.navigationTarget = ILCloneFull(pidl);
    HRESULT hr = pane.navigationTarget ?
        pane.browser->BrowseToIDList(pidl, SBSP_ABSOLUTE) : E_OUTOFMEMORY;
    if (FAILED(hr)) {
        ClearNavigationTarget(pane);
        LogHr(L"BrowseToIDList", hr);
    } else {
        pane.path = path;
        if (pane.window) InvalidateRect(pane.window, nullptr, FALSE);
    }
    return hr;
}

HRESULT BrowsePath(Pane& pane, const std::wstring& path) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    HRESULT hr = SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr);
    if (SUCCEEDED(hr)) hr = BrowsePidl(pane, pidl, path);
    else LogHr(L"BrowsePath", hr);
    CoTaskMemFree(pidl);
    return hr;
}

void ReadInspectorColors();
void RefreshPane(Pane& pane) {
    if (!pane.browser || !pane.initialized) return;
    IFolderView* view = nullptr;
    if (SUCCEEDED(pane.browser->GetCurrentView(IID_IFolderView,
            reinterpret_cast<void**>(&view)))) {
        IPersistFolder2* folder = nullptr;
        if (SUCCEEDED(view->GetFolder(IID_IPersistFolder2,
                reinterpret_cast<void**>(&folder)))) {
            PIDLIST_ABSOLUTE pidl = nullptr;
            if (SUCCEEDED(folder->GetCurFolder(&pidl)) && pidl) {
                PWSTR path = nullptr;
                HRESULT pathHr = SHGetNameFromIDList(pidl, SIGDN_FILESYSPATH, &path);
                if (FAILED(pathHr)) {
                    CoTaskMemFree(path);
                    path = nullptr;
                    pathHr = SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEPARSING, &path);
                }
                if (SUCCEEDED(pathHr) && path && pane.path != path) {
                    pane.path = path;
                    InvalidateRect(pane.window, nullptr, FALSE);
                }
                CoTaskMemFree(path);
                CoTaskMemFree(pidl);
            }
            folder->Release();
        }
        view->Release();
    }

    IVisualProperties* visuals = nullptr;
    HRESULT visualHr = pane.browser->GetCurrentView(IID_IVisualProperties,
            reinterpret_cast<void**>(&visuals));
    if (SUCCEEDED(visualHr)) {
        IUnknown* identity = nullptr;
        if (SUCCEEDED(visuals->QueryInterface(IID_IUnknown,
                reinterpret_cast<void**>(&identity)))) {
            if (identity != pane.styledView) {
                if (pane.styledView) pane.styledView->Release();
                pane.styledView = identity;
                COLORREF actual = 0;
                HRESULT readHr = visuals->GetColor(VPCF_BACKGROUND, &actual);
                Wh_Log(L"[Columns] Pane %d native color=%08X "
                       L"readHr=%08X", pane.index,
                       static_cast<unsigned>(actual),
                       static_cast<unsigned>(readHr));
            } else {
                identity->Release();
            }
        }
        visuals->Release();
    } else if (!pane.styleUnavailableLogged) {
        pane.styleUnavailableLogged = true;
        LogHr(L"GetCurrentView(IVisualProperties)", visualHr);
    }
    ReadInspectorColors();
}

void ApplyBrowserRect(HWND host, Pane& pane) {
    if (!pane.browser || !pane.initialized) return;
    RECT client = {};
    GetClientRect(host, &client);
    DWORD start = GetTickCount();
    HRESULT hr = pane.browser->SetRect(nullptr, client);
    pane.lastSetRect = GetTickCount();
    if (FAILED(hr)) LogHr(L"SetRect", hr);
    DWORD elapsed = pane.lastSetRect - start;
    if (elapsed >= 24) {
        Wh_Log(L"[Columns] Pane %d SetRect took %u ms",
               pane.index, elapsed);
    }
}

LRESULT CALLBACK ViewHostProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    CallbackScope callbackScope;
    Pane* pane = reinterpret_cast<Pane*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lp);
        pane = static_cast<Pane*>(create->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pane));
    }
    if (!pane || !pane->session) return DefWindowProcW(hwnd, message, wp, lp);
    SessionScope scope(pane->session);
    if (message == WM_SIZE && pane->browser && pane->initialized &&
        wp != SIZE_MINIMIZED) {
        // The parent and its native view share one resize transaction. A second
        // 48 ms schedule left the old DefView width exposed inside the new host.
        ApplyBrowserRect(hwnd, *pane);
        return 0;
    }
    if (message == WM_ERASEBKGND) {
        PaintColumnBackground(hwnd, reinterpret_cast<HDC>(wp));
        return 1;
    }
    if (message == WM_PAINT) {
        PAINTSTRUCT ps = {};
        HDC dc = BeginPaint(hwnd, &ps);
        PaintColumnBackground(hwnd, dc, &ps.rcPaint);
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

LRESULT CALLBACK PaneProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    CallbackScope callbackScope;
    Pane* pane = reinterpret_cast<Pane*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lp);
        pane = static_cast<Pane*>(create->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pane));
    }
    if (!pane || !pane->session) return DefWindowProcW(hwnd, message, wp, lp);
    SessionScope scope(pane->session);
    switch (message) {
        case WM_SIZE:
            if (pane && pane->viewHost) {
                SetWindowPos(pane->viewHost, nullptr, 0, 0,
                             LOWORD(lp), HIWORD(lp),
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return 0;
        case WM_ERASEBKGND:
            PaintColumnBackground(hwnd, reinterpret_cast<HDC>(wp));
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps = {};
            HDC dc = BeginPaint(hwnd, &ps);
            PaintColumnBackground(hwnd, dc, &ps.rcPaint);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

int SlotCount() {
    return Context().columnCount;
}

int InspectorWidth(HWND overlay) {
    if (!IsWindow(overlay) || Context().previewIndex < 0) return 0;
    RECT rect = {};
    GetClientRect(overlay, &rect);
    return std::min(MulDiv(kInspectorWidth, GetDpiForWindow(overlay), 96),
                    static_cast<int>(std::max(0L, rect.right / 2)));
}

int DefaultColumnWidth(HWND overlay) {
    return MulDiv(kDefaultColumnWidth, GetDpiForWindow(overlay), 96);
}

int SeparatorWidth() {
    UINT dpi = Context().layoutDpi;
    if (!dpi && IsWindow(Context().overlay)) dpi = GetDpiForWindow(Context().overlay);
    return std::max(2, MulDiv(Context().dividerWidth, dpi ? dpi : 96, 96));
}

void RefreshLayoutDpi(UINT dpi) {
    if (!dpi) dpi = 96;
    UINT previous = Context().layoutDpi;
    if (previous == dpi) return;
    CancelColumnResize();
    if (previous) {
        for (int i = 0; i < Context().columnCount; ++i)
            Context().columnWidths[i] = std::max(1,
                MulDiv(Context().columnWidths[i], dpi, previous));
        Context().scrollX = std::max(0, MulDiv(Context().scrollX, dpi, previous));
    }
    Context().layoutDpi = dpi;
}

int TotalColumnWidth() {
    int total = SlotCount() * SeparatorWidth();
    for (int i = 0; i < SlotCount(); ++i) total += Context().columnWidths[i];
    return total;
}

void LayoutPanes(HWND overlay) {
    if (Context().inLayout || !IsWindow(overlay)) return;
    if (Context().resizeSplitter && !Context().applyingColumnResize) {
        CancelColumnResize();
        return;
    }
    Context().inLayout = true;
    RECT rect = {};
    GetClientRect(overlay, &rect);
    int inspectorWidth = InspectorWidth(overlay);
    int viewport = std::max(1, static_cast<int>(rect.right) - inspectorWidth);
    int total = TotalColumnWidth();
    Context().scrollX = std::clamp(Context().scrollX, 0, std::max(0, total - viewport));
    SCROLLINFO scroll = {};
    scroll.cbSize = sizeof(scroll);
    scroll.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    scroll.nMin = 0;
    scroll.nMax = std::max(0, total - 1);
    scroll.nPage = viewport;
    scroll.nPos = Context().scrollX;
    SetScrollInfo(overlay, SB_HORZ, &scroll, TRUE);
    GetClientRect(overlay, &rect);
    int height = rect.bottom;
    int x = -Context().scrollX;
    constexpr UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
    for (int i = 0; i < SlotCount(); ++i) {
        HWND slot = Context().panes[i].window;
        if (IsWindow(slot))
            SetWindowPos(slot, nullptr, x, 0,
                         Context().columnWidths[i], height, flags);
        x += Context().columnWidths[i];
        if (IsWindow(Context().splitters[i])) {
            SetWindowPos(Context().splitters[i], nullptr, x, 0,
                         SeparatorWidth(), height, flags);
            x += SeparatorWidth();
        }
    }
    if (IsWindow(Context().previewWindow)) {
        int inspectorLeft = Context().fixedInspector ? rect.right - inspectorWidth : x;
        int expandedWidth = std::max(inspectorWidth,
                                    static_cast<int>(rect.right) - inspectorLeft);
        SetWindowPos(Context().previewWindow, HWND_TOP,
                     inspectorLeft, 0, expandedWidth, height,
                     SWP_NOACTIVATE);
    }
    InvalidateRect(overlay, nullptr, FALSE);
    Context().inLayout = false;
}

void ScrollToNewestColumn() {
    if (!Context().overlay) return;
    RECT rect = {};
    GetClientRect(Context().overlay, &rect);
    Context().scrollX = std::max(0, TotalColumnWidth() -
        (static_cast<int>(rect.right) - InspectorWidth(Context().overlay)));
    LayoutPanes(Context().overlay);
}

void ResetColumnResizeState() {
    if (Context().resizeSplitter)
        KillTimer(Context().resizeSplitter, kLiveResizeTimerId);
    Context().resizeSplitter = nullptr;
    Context().resizeColumn = -1;
    Context().resizeProposedWidth = 0;
    Context().applyingColumnResize = false;
}

void CancelColumnResize() {
    HWND splitter = Context().resizeSplitter;
    if (!splitter) return;
    int index = Context().resizeColumn;
    int width = Context().resizeStartWidth;
    int scroll = Context().resizeStartScrollX;
    // Clear first: ReleaseCapture synchronously sends WM_CAPTURECHANGED.
    ResetColumnResizeState();
    if (GetCapture() == splitter) ReleaseCapture();
    if (index >= 0 && index < Context().columnCount)
        Context().columnWidths[index] = width;
    Context().scrollX = scroll;
    if (!Context().closed && IsWindow(Context().overlay))
        LayoutPanes(Context().overlay);
}

void BeginColumnResize(HWND splitter, int index, POINT point) {
    CancelPendingFocus();
    CancelColumnResize();
    if (index < 0 || index >= Context().columnCount) return;
    Context().resizeSplitter = splitter;
    Context().resizeColumn = index;
    Context().resizeStartX = point.x;
    Context().resizeStartWidth = Context().columnWidths[index];
    Context().resizeStartScrollX = Context().scrollX;
    Context().resizeProposedWidth = Context().columnWidths[index];
    Context().lastSplitterLayout = 0;
    SetCapture(splitter);
    if (GetCapture() != splitter) CancelColumnResize();
}

void ApplyLiveColumnResize(bool force = false) {
    HWND splitter = Context().resizeSplitter;
    int index = Context().resizeColumn;
    if (!splitter || index < 0 ||
        index >= Context().columnCount || Context().closed) return;
    DWORD elapsed = GetTickCount() - Context().lastSplitterLayout;
    if (!force && elapsed < kLiveResizeIntervalMs) {
        // Preserve the latest requested width even if the pointer stops before
        // the next frame. The old throttle dropped that final mouse movement.
        SetTimer(splitter, kLiveResizeTimerId, kLiveResizeIntervalMs - elapsed, nullptr);
        return;
    }
    KillTimer(splitter, kLiveResizeTimerId);
    int width = Context().resizeProposedWidth;
    if (Context().columnWidths[index] == width) return;
    Context().lastSplitterLayout = GetTickCount();
    Context().columnWidths[index] = width;
    Context().applyingColumnResize = true;
    LayoutPanes(Context().overlay);
    Context().applyingColumnResize = false;
}

void UpdateColumnResizeAtPoint(POINT point, bool finalUpdate = false) {
    HWND splitter = Context().resizeSplitter;
    int index = Context().resizeColumn;
    if (!splitter || index < 0 || index >= Context().columnCount) return;
    int dpi = GetDpiForWindow(Context().overlay);
    int width = std::clamp(Context().resizeStartWidth +
                          static_cast<int>(point.x) - Context().resizeStartX,
                          MulDiv(180, dpi, 96), MulDiv(1200, dpi, 96));
    Context().resizeProposedWidth = width;
    ApplyLiveColumnResize(finalUpdate);
}

void FinishColumnResize(POINT point) {
    if (!Context().resizeSplitter) return;
    UpdateColumnResizeAtPoint(point, true);
    HWND splitter = Context().resizeSplitter;
    int index = Context().resizeColumn;
    int width = Context().resizeProposedWidth;
    ResetColumnResizeState();
    if (GetCapture() == splitter) ReleaseCapture();
    if (index < 0 || index >= Context().columnCount || Context().closed) return;
    Context().columnWidths[index] = width;
    // Preserve the user's current viewport. LayoutPanes only clamps it when
    // shrinking leaves fewer columns than the visible width.
    LayoutPanes(Context().overlay);
    RedrawWindow(Context().overlay, nullptr, nullptr,
        RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    if (g_delayedPaintRepairs.load())
        SetTimer(Context().overlay, kDividerPaintTimerId, 120, nullptr);
    Wh_Log(L"[Columns] Divider resize committed; width=%d scroll=%d", width, Context().scrollX);
}

POINT SplitterMessagePoint(HWND splitter, LPARAM lp) {
    POINT point{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
    MapWindowPoints(splitter, Context().overlay, &point, 1);
    return point;
}

LRESULT CALLBACK SplitterProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    CallbackScope callbackScope;
    if (message == WM_NCCREATE)
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(
            reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams));
    auto* pane = reinterpret_cast<Pane*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (!pane || !pane->session) return DefWindowProcW(hwnd, message, wp, lp);
    SessionScope scope(pane->session);
    switch (message) {
        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_SIZEWE));
            return TRUE;
        case WM_LBUTTONDOWN:
            BeginColumnResize(hwnd, pane->index, SplitterMessagePoint(hwnd, lp));
            return 0;
        case WM_MOUSEMOVE:
            if (GetCapture() == hwnd && Context().resizeSplitter == hwnd)
                UpdateColumnResizeAtPoint(SplitterMessagePoint(hwnd, lp));
            return 0;
        case WM_LBUTTONUP:
            if (GetCapture() == hwnd && Context().resizeSplitter == hwnd)
                FinishColumnResize(SplitterMessagePoint(hwnd, lp));
            return 0;
        case WM_TIMER:
            if (wp == kLiveResizeTimerId) {
                KillTimer(hwnd, kLiveResizeTimerId);
                if (GetCapture() == hwnd && Context().resizeSplitter == hwnd)
                    ApplyLiveColumnResize(true);
                return 0;
            }
            break;
        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
            if (Context().resizeSplitter == hwnd) CancelColumnResize();
            return 0;
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE && Context().resizeSplitter == hwnd) {
                CancelColumnResize();
                return 0;
            }
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps = {};
            HDC dc = BeginPaint(hwnd, &ps);
            PaintSplitterContents(hwnd, dc, &ps.rcPaint);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            if (Context().resizeSplitter == hwnd) CancelColumnResize();
            for (HWND& part : Context().splitters)
                if (part == hwnd) part = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

bool CreateSplitter(int index) {
    if (index < 0 || index >= kMaxColumns) return false;
    Context().panes[index].session = &Context();
    Context().panes[index].index = index;
    Context().splitters[index] = CreateWindowExW(0, kSplitterClass, L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0, 0, SeparatorWidth(), 100,
        Context().overlay, nullptr, HINST_THISCOMPONENT,
        &Context().panes[index]);
    if (!Context().splitters[index]) LogWin32(L"CreateWindowEx(splitter)");
    return Context().splitters[index] != nullptr;
}

struct SelectedItem {
    std::wstring path;
    bool folder = false;
    bool querySucceeded = false; // A query failure never means deselection.
    std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)> identity{nullptr, CoTaskMemFree};
};

SelectedItem SelectedShellItem(IShellItem* item) {
    SelectedItem result;
    // A failed provider query is transient; a successfully queried Home item
    // with no absolute target is unavailable and clears unrelated descendants.
    if (!item) return result;
    item->AddRef();
    SFGAOF attributes = 0;
    HRESULT hr = item->GetAttributes(SFGAO_FOLDER | SFGAO_LINK | SFGAO_FILESYSTEM, &attributes);
    if (SUCCEEDED(hr) && (attributes & SFGAO_LINK)) {
        IShellItem* target = nullptr;
        item->BindToHandler(nullptr, BHID_LinkTargetItem, IID_IShellItem,
            reinterpret_cast<void**>(&target));
        if (!target) {
            IShellLinkW* link = nullptr;
            if (SUCCEEDED(item->BindToHandler(nullptr, BHID_SFUIObject, IID_IShellLinkW,
                    reinterpret_cast<void**>(&link))) && link) {
                PIDLIST_ABSOLUTE destination = nullptr;
                if (SUCCEEDED(link->GetIDList(&destination)) && destination)
                    SHCreateItemFromIDList(destination, IID_IShellItem,
                        reinterpret_cast<void**>(&target));
                CoTaskMemFree(destination);
                link->Release();
            }
        }
        if (target) {
            item->Release();
            item = target;
            attributes = 0;
            hr = item->GetAttributes(SFGAO_FOLDER | SFGAO_FILESYSTEM, &attributes);
        }
    }
    PWSTR path = nullptr;
    if (SUCCEEDED(hr)) {
        hr = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
        if (FAILED(hr)) {
            CoTaskMemFree(path);
            path = nullptr;
            hr = item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &path);
        }
        // Home can return stale recent objects with only a basename. Treat
        // those as unavailable, never relative to Explorer's working directory.
        bool absolute = path && (IsAbsoluteColumnPath(path) || wcsncmp(path, L"::{", 3) == 0);
        if (SUCCEEDED(hr) && path && *path) result.querySucceeded = true;
        if (SUCCEEDED(hr) && absolute) {
            result.path = path;
            result.folder = (attributes & SFGAO_FOLDER) != 0;
            if (result.folder) {
                PIDLIST_ABSOLUTE destination = nullptr;
                // Home wrappers are not the actual destination PIDLs. Normalize
                // filesystem targets before extending into them or an archive.
                hr = SHParseDisplayName(path, nullptr, &destination, 0, nullptr);
                if (SUCCEEDED(hr) && destination) result.identity.reset(destination);
                else {
                    CoTaskMemFree(destination);
                    result.folder = false;
                    result.path.clear();
                    result.querySucceeded = false; // Retry a failed target bind.
                }
            }
        }
    }
    CoTaskMemFree(path);
    item->Release();
    return result;
}

SelectedItem SelectedPath(Pane& pane) {
    SelectedItem result;
    if (!pane.browser || !pane.initialized) return result;
    IFolderView2* view = nullptr;
    HRESULT hr = pane.browser->GetCurrentView(IID_IFolderView2,
        reinterpret_cast<void**>(&view));
    if (FAILED(hr) || !view) return result;
    int selectionCount = 0;
    hr = view->ItemCount(SVGIO_SELECTION, &selectionCount);
    if (FAILED(hr)) { view->Release(); return result; }
    if (selectionCount != 1) {
        view->Release();
        result.querySucceeded = true;
        return result;
    }
    IShellItemArray* selection = nullptr;
    hr = view->GetSelection(FALSE, &selection);
    view->Release();
    if (FAILED(hr)) { if (selection) selection->Release(); return result; }
    // GetSelection documents S_FALSE as a successfully queried zero-item array.
    if (!selection) { result.querySucceeded = hr == S_FALSE; return result; }
    DWORD count = 0;
    hr = selection->GetCount(&count);
    IShellItem* item = nullptr;
    if (SUCCEEDED(hr) && count == 1) hr = selection->GetItemAt(0, &item);
    selection->Release();
    if (FAILED(hr)) return result;
    if (count != 1) { result.querySucceeded = true; return result; }
    if (!item) return result;
    result = SelectedShellItem(item);
    item->Release();
    return result;
}

bool FocusColumn(int index) {
    if (NativePointerInteractionActive()) return false;
    if (index < 0 || index >= Context().columnCount ||
        !Context().panes[index].browser || Context().panes[index].navigationTarget)
        return false;
    IExplorerBrowser* browser = Context().panes[index].browser;
    browser->AddRef();
    struct BrowserRef {
        IExplorerBrowser* value;
        ~BrowserRef() { value->Release(); }
    } keep{browser};
    unsigned epoch = Context().focusEpoch;
    auto stillCurrent = [&] {
        return !Context().closed && Context().focusEpoch == epoch &&
            !NativePointerInteractionActive() &&
            index < Context().columnCount && Context().panes[index].browser == browser;
    };
    IShellView* shellView = nullptr;
    HRESULT hr = browser->GetCurrentView(IID_IShellView,
        reinterpret_cast<void**>(&shellView));
    if (FAILED(hr) || !shellView) return false;
    if (stillCurrent()) hr = shellView->UIActivate(SVUIA_ACTIVATE_FOCUS);
    else hr = E_ABORT;
    shellView->Release();
    if (FAILED(hr) || !stillCurrent()) return false;
    // Activating an empty folder is valid. Selection is an optional convenience,
    // not a reason to keep stealing focus with the retry timer.
    SetActivePane(index);
    IFolderView2* folderView = nullptr;
    hr = browser->GetCurrentView(IID_IFolderView2,
        reinterpret_cast<void**>(&folderView));
    if (FAILED(hr) || !folderView) return stillCurrent();
    int itemCount = 0;
    hr = folderView->ItemCount(SVGIO_ALLVIEW, &itemCount);
    if (SUCCEEDED(hr) && itemCount > 0 && stillCurrent()) {
        IShellItemArray* selection = nullptr;
        DWORD selectedCount = 0;
        if (SUCCEEDED(folderView->GetSelection(FALSE, &selection)) &&
            selection) {
            selection->GetCount(&selectedCount);
            selection->Release();
        }
        if (selectedCount == 0 && stillCurrent()) {
            int focusedItem = 0;
            if (FAILED(folderView->GetFocusedItem(&focusedItem)) ||
                focusedItem < 0 || focusedItem >= itemCount)
                focusedItem = 0;
            if (stillCurrent()) {
                hr = folderView->SelectItem(focusedItem,
                    SVSI_SELECT | SVSI_FOCUSED | SVSI_ENSUREVISIBLE);
                if (FAILED(hr))
                    Wh_Log(L"[Columns] SelectItem on enter failed: %08X", hr);
            }
        }
    }
    folderView->Release();
    return stillCurrent();
}

void CancelPendingFocus() {
    ++Context().focusEpoch;
    if (IsWindow(Context().overlay)) KillTimer(Context().overlay, kFocusTimerId);
    Context().focusRequest = -1;
    Context().focusAttempts = 0;
}

void RequestFocusColumn(int index) {
    CancelPendingFocus();
    if (index < 0 || index >= Context().columnCount) return;
    SetActivePane(index);
    // Ready views respond immediately. Retry only while a newly created native
    // view is not yet available for activation.
    unsigned epoch = Context().focusEpoch;
    IExplorerBrowser* browser = Context().panes[index].browser;
    if (FocusColumn(index) || Context().closed || Context().focusEpoch != epoch ||
        index >= Context().columnCount || Context().panes[index].browser != browser) return;
    Context().focusRequest = index;
    SetTimer(Context().overlay, kFocusTimerId, 80, nullptr);
}

class MediaEngineNotify final : public IMFMediaEngineNotify {
    ComObjectLifetime lifetime_{g_liveMediaEngineNotifies};
    LONG references_ = 1;
    std::atomic<HWND> target_;
    DWORD generation_;
public:
    MediaEngineNotify(HWND target, DWORD generation) : target_(target), generation_(generation) {}
    void Detach() { target_.store(nullptr, std::memory_order_release); }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        CallbackScope callbackScope;
        if (!out) return E_POINTER;
        *out = nullptr;
        if (IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_IMFMediaEngineNotify)) {
            *out = static_cast<IMFMediaEngineNotify*>(this);
            AddRef(); return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        CallbackScope callbackScope;
        return InterlockedIncrement(&references_);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        CallbackScope callbackScope;
        ULONG count = InterlockedDecrement(&references_);
        if (!count) delete this;
        return count;
    }
    HRESULT STDMETHODCALLTYPE EventNotify(DWORD event, DWORD_PTR param1, DWORD param2) override {
        CallbackScope callbackScope;
        if (event == MF_MEDIA_ENGINE_EVENT_NOTIFYSTABLESTATE) {
            SetEvent(reinterpret_cast<HANDLE>(param1));
            return S_OK;
        }
        if (g_unloading.load()) return S_OK;
        // No UI or global player state is touched from the engine callback thread.
        LPARAM packed = static_cast<LPARAM>((static_cast<ULONGLONG>(param2) << 32) | event);
        HWND target = target_.load(std::memory_order_acquire);
        if (target) PostMessageW(target, kMediaEventMessage, generation_, packed);
        return S_OK;
    }
};

bool MediaCanPlay() { return (Context().mediaEngine && Context().mediaReady) || Context().audioControl; }
bool MediaCanSeek() { return (Context().mediaEngine && Context().mediaReady) || Context().audioSeeking; }
bool MediaCanSetVolume() {
    return (Context().mediaEngine && Context().mediaReady && Context().mediaEngine->HasAudio()) || Context().audioVolumeControl;
}
void HandleMediaEvent(DWORD event, HRESULT error);
void PositionVideoPreview();

void UpdateAudioPlaybackTimer() {
    bool active = Context().audioPlaying && (Context().audioGraph || Context().mediaEngine) &&
        IsWindowVisible(Context().previewWindow) && !g_unloading.load();
    if (active) {
        // PLAY and PLAYING can both arrive; preserve an already running timer.
        if (!Context().audioTimerRunning)
            Context().audioTimerRunning = SetTimer(Context().previewWindow,
                kAudioTimerId, kAudioTimerIntervalMs, nullptr) != 0;
    } else {
        if (IsWindow(Context().previewWindow)) KillTimer(Context().previewWindow, kAudioTimerId);
        Context().audioTimerRunning = false;
    }
}

void PauseMediaPreview() {
    Context().audioPlaying = false;
    UpdateAudioPlaybackTimer();
    if (Context().mediaEngine) Context().mediaEngine->Pause();
    if (Context().audioControl) Context().audioControl->Pause();
    Context().audioPlaying = false;
    UpdateAudioPlaybackTimer();
}

void StopAudioPreview() {
    if (IsWindow(Context().previewWindow)) KillTimer(Context().previewWindow, kAudioTimerId);
    Context().audioTimerRunning = false;
    Context().audioPlaying = false;
    Context().mediaReady = false;
    auto* callback = std::exchange(Context().mediaNotify, nullptr);
    if (callback) callback->Detach();
    auto* engine = std::exchange(Context().mediaEngine, nullptr);
    auto* engineEx = std::exchange(Context().mediaEngineEx, nullptr);
    if (engine) engine->Shutdown();
    if (engineEx) engineEx->Release();
    if (engine) engine->Release();
    if (callback) callback->Release();
    if (Context().mediaFoundationStarted) { MFShutdown(); Context().mediaFoundationStarted = false; }
    if (Context().audioControl) Context().audioControl->Stop();
    if (Context().directShowVideo) {
        Context().directShowVideo->put_Visible(OAFALSE);
        Context().directShowVideo->put_Owner(0);
        Context().directShowVideo->Release(); Context().directShowVideo = nullptr;
    }
    if (Context().audioSeeking) { Context().audioSeeking->Release(); Context().audioSeeking = nullptr; }
    if (Context().audioVolumeControl) { Context().audioVolumeControl->Release(); Context().audioVolumeControl = nullptr; }
    if (Context().audioControl) { Context().audioControl->Release(); Context().audioControl = nullptr; }
    if (Context().audioGraph) { Context().audioGraph->Release(); Context().audioGraph = nullptr; }
    if (IsWindow(Context().videoWindow)) DestroyWindow(Context().videoWindow);
    Context().videoWindow = nullptr;
    Context().audioPlaying = false;
    Context().audioDuration = 0;
}

std::wstring AudioTime(LONGLONG units) {
    unsigned long long seconds = units > 0 ?
        static_cast<unsigned long long>(units / 10000000) : 0;
    wchar_t result[32] = {};
    swprintf(result, ARRAYSIZE(result), L"%llu:%02llu",
             seconds / 60, seconds % 60);
    return result;
}

LONGLONG AudioPosition() {
    if (Context().mediaEngine) {
        double seconds = Context().mediaEngine->GetCurrentTime();
        return std::isfinite(seconds) && seconds > 0 ?
            static_cast<LONGLONG>(seconds * 10000000.0) : 0;
    }
    LONGLONG position = 0;
    if (Context().audioSeeking) Context().audioSeeking->GetCurrentPosition(&position);
    return position;
}

int InspectorPx(int logical) {
    UINT dpi = IsWindow(Context().previewWindow) ? GetDpiForWindow(Context().previewWindow) : 96;
    return MulDiv(logical, dpi ? dpi : 96, 96);
}

COLORREF InspectorBlend(COLORREF front, COLORREF back, int opacity) {
    return RGB((GetRValue(front) * opacity + GetRValue(back) * (100 - opacity)) / 100,
               (GetGValue(front) * opacity + GetGValue(back) * (100 - opacity)) / 100,
               (GetBValue(front) * opacity + GetBValue(back) * (100 - opacity)) / 100);
}

double InspectorLuminance(COLORREF color) {
    auto linear = [](BYTE channel) {
        double value = channel / 255.0;
        return value <= 0.04045 ? value / 12.92 : std::pow((value + 0.055) / 1.055, 2.4);
    };
    return 0.2126 * linear(GetRValue(color)) + 0.7152 * linear(GetGValue(color)) +
           0.0722 * linear(GetBValue(color));
}

COLORREF InspectorReadableText(COLORREF background, COLORREF requested) {
    double back = InspectorLuminance(background);
    double front = InspectorLuminance(requested);
    double contrast = (std::max(back, front) + 0.05) / (std::min(back, front) + 0.05);
    // Explorer themes can recolor DrawText while leaving GDI pens unchanged.
    // A reported black foreground on a dark canvas must not produce black controls.
    if (contrast >= 4.5) return requested;
    return (1.05 / (back + 0.05)) >= ((back + 0.05) / 0.05) ?
        RGB(238, 238, 238) : RGB(28, 28, 28);
}

struct ColumnPalette {
    COLORREF background;
    COLORREF text;
    PCWSTR source;
};

ColumnPalette ResolveInheritedColumnPalette(COLORREF background, COLORREF text,
    bool highContrast, bool appDark, bool haveTheme, COLORREF themeBackground,
    COLORREF themeText) {
    ColumnPalette palette{background, text,
        highContrast ? L"high-contrast" : L"system-window"};
    if (!highContrast && appDark && background == RGB(255, 255, 255)) {
        // Visual-style SysMetrics can still be light even when apps use dark
        // mode. Accept a genuinely dark theme canvas, not a second white value.
        if (haveTheme && InspectorLuminance(themeBackground) < 0.18) {
            palette = {themeBackground, themeText, L"native-theme"};
        } else {
            // Last resort for stock Win32 white only. Customized system colors
            // (including transparent black) never enter this branch.
            palette = {RGB(25, 25, 25), RGB(238, 238, 238), L"app-dark-fallback"};
        }
    }
    return palette;
}

void ReadInspectorColors() {
    // Read the Shell's palette; never impose a dark canvas on a light view.
    // Cache it outside WM_PAINT so painting exposed host strips does no COM work.
    COLORREF background = GetSysColor(COLOR_WINDOW);
    COLORREF text = GetSysColor(COLOR_WINDOWTEXT);
    int index = std::clamp(Context().activePane, 0, std::max(0, Context().columnCount - 1));
    HIGHCONTRASTW contrast = {};
    contrast.cbSize = sizeof(contrast);
    bool highContrast = SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON);
    DWORD light = 1, length = sizeof(light);
    bool appDark = RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &length) == ERROR_SUCCESS && !light;
    PCWSTR source = highContrast ? L"high-contrast" : L"system-window";
    if (!highContrast) {
        bool haveNativeBackground = false;
        HWND nativeWindow = Context().originalView;
        for (int attempt = 0; attempt < Context().columnCount; ++attempt) {
            int candidate = (index + attempt) % Context().columnCount;
            IVisualProperties* visuals = nullptr;
            auto& pane = Context().panes[candidate];
            if (!pane.browser || !pane.initialized || FAILED(pane.browser->GetCurrentView(
                    IID_IVisualProperties, reinterpret_cast<void**>(&visuals)))) continue;
            COLORREF color = CLR_NONE;
            bool haveBackground = SUCCEEDED(visuals->GetColor(VPCF_BACKGROUND, &color)) &&
                !(color & 0xFF000000) && !(appDark && color == RGB(255, 255, 255));
            if (haveBackground) {
                background = color;
                haveNativeBackground = true;
                source = L"native-view";
            }
            if (SUCCEEDED(visuals->GetColor(VPCF_TEXT, &color)) && !(color & 0xFF000000)) text = color;
            visuals->Release();
            if (!nativeWindow) {
                IShellView* shell = nullptr;
                if (SUCCEEDED(pane.browser->GetCurrentView(IID_IShellView,
                        reinterpret_cast<void**>(&shell)))) {
                    shell->GetWindow(&nativeWindow);
                    shell->Release();
                }
            }
            if (haveBackground) break;
        }
        // An inherited-color sentinel means use the window palette. Styling
        // mods can deliberately make COLOR_WINDOW black/transparent. Replacing
        // it with a guessed dark RGB created the gray unused-area rectangle.
        // Stock white reported during a dark-mode transition is not a custom
        // canvas override. Nonwhite native colors remain authoritative.
        if (!haveNativeBackground) {
            COLORREF themeBackground = background, themeText = text;
            bool haveTheme = false;
            if (!nativeWindow && Context().tab) nativeWindow = FindOriginalView(Context().tab);
            if (appDark && background == RGB(255, 255, 255)) {
                HTHEME theme = IsWindow(nativeWindow) ? GetWindowTheme(nativeWindow) : nullptr;
                if (theme) {
                    themeBackground = GetThemeSysColor(theme, COLOR_WINDOW);
                    themeText = GetThemeSysColor(theme, COLOR_WINDOWTEXT);
                    haveTheme = true;
                }
            }
            auto palette = ResolveInheritedColumnPalette(background, text,
                highContrast, appDark, haveTheme, themeBackground, themeText);
            background = palette.background;
            text = palette.text;
            source = palette.source;
        }
    }
    text = InspectorReadableText(background, text);
    if (background == Context().inspectorBackground && text == Context().inspectorText) return;
    Context().inspectorBackground = background;
    Context().inspectorText = text;
    Wh_Log(L"[Columns palette] v%s background=%08X text=%08X source=%s appDark=%d highContrast=%d systemWindow=%08X",
        WH_MOD_VERSION, static_cast<unsigned>(background), static_cast<unsigned>(text), source,
        appDark, highContrast, static_cast<unsigned>(GetSysColor(COLOR_WINDOW)));
    if (IsWindow(Context().overlay)) InvalidateRect(Context().overlay, nullptr, FALSE);
    if (IsWindow(Context().previewWindow)) InvalidateRect(Context().previewWindow, nullptr, FALSE);
    for (int i = 0; i < Context().columnCount; ++i) {
        for (HWND window : {Context().panes[i].window, Context().panes[i].viewHost, Context().splitters[i]})
            if (IsWindow(window)) InvalidateRect(window, nullptr, FALSE);
    }
}

void PaintColumnBackground(HWND window, HDC dc, const RECT* area) {
    if (!dc) return;
    RECT client = {};
    if (area) client = *area;
    else GetClientRect(window, &client);
    if (Context().inspectorBackground == GetSysColor(COLOR_WINDOW)) {
        // Keep the semantic system brush, so an Explorer styling mod can apply
        // the same transparency treatment as it does to the native file lists.
        FillRect(dc, &client, GetSysColorBrush(COLOR_WINDOW));
        return;
    }
    COLORREF previous = SetDCBrushColor(dc, Context().inspectorBackground);
    FillRect(dc, &client, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
    SetDCBrushColor(dc, previous);
}

void PaintSplitterContents(HWND window, HDC dc, const RECT* area) {
    if (!dc) return;
    PaintColumnBackground(window, dc, area);
    RECT client = {};
    GetClientRect(window, &client);
    // Keep a hairline at every DPI. The larger, background-matched window is
    // only the hit target: it must not become a second broad gray separator.
    int center = (client.right - client.left) / 2;
    RECT line = {center, client.top, center + 1, client.bottom};
    COLORREF color = InspectorBlend(Context().inspectorText,
        Context().inspectorBackground, GetCapture() == window ? 75 : 45);
    COLORREF previous = SetDCBrushColor(dc, color);
    FillRect(dc, &line, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
    SetDCBrushColor(dc, previous);
}

struct InspectorLayout {
    bool showOpenWith = true;
    int height = 0;
    int infoLine = 0;
    int openLine = 0;
    RECT preview = {};
    RECT wave = {};
    RECT seek = {};
    RECT time = {};
    RECT back = {};
    RECT play = {};
    RECT forward = {};
    RECT speaker = {};
    RECT volume = {};
    RECT open = {};
};

InspectorLayout GetInspectorLayout(const RECT& client) {
    InspectorLayout layout;
    layout.showOpenWith = !Context().previewIsVirtualFolder;
    int margin = InspectorPx(20);
    int width = client.right;
    int cx = width / 2;
    layout.height = std::max(static_cast<int>(client.bottom),
                            InspectorPx(!layout.showOpenWith ? 260 :
                                Context().previewVideo ? 714 : Context().previewMedia ? 634 : 420));
    layout.infoLine = InspectorPx(Context().previewMedia ? 290 : 188);
    if (Context().previewMedia) {
        int waveTop = InspectorPx(330);
        int waveHeight = std::clamp(layout.height - InspectorPx(550),
                                    InspectorPx(Context().previewVideo ? 120 : 80),
                                    InspectorPx(Context().previewVideo ? 220 : 160));
        layout.wave = {margin, waveTop, width - margin, waveTop + waveHeight};
        int extra = Context().previewVideo ? 18 : 0;
        layout.seek = {margin, layout.wave.bottom + InspectorPx(6),
                       width - margin, layout.wave.bottom + InspectorPx(9)};
        layout.time = {margin, layout.wave.bottom + InspectorPx(4 + extra),
                       width - margin, layout.wave.bottom + InspectorPx(24 + extra)};
        int cy = layout.wave.bottom + InspectorPx(44 + extra);
        int radius = InspectorPx(20);
        layout.play = {cx - radius, cy - radius, cx + radius, cy + radius};
        layout.back = {cx - InspectorPx(77), cy - InspectorPx(18),
                       cx - InspectorPx(41), cy + InspectorPx(18)};
        layout.forward = {cx + InspectorPx(41), cy - InspectorPx(18),
                          cx + InspectorPx(77), cy + InspectorPx(18)};
        int volumeY = cy + InspectorPx(46);
        layout.speaker = {margin, volumeY - InspectorPx(14),
                          margin + InspectorPx(30), volumeY + InspectorPx(14)};
        layout.volume = {margin + InspectorPx(42), volumeY - InspectorPx(2),
                         width - margin - InspectorPx(8), volumeY + InspectorPx(2)};
        layout.openLine = volumeY + InspectorPx(28);
    } else {
        layout.openLine = layout.showOpenWith ? layout.height - InspectorPx(100) : layout.height;
        layout.preview = {margin, layout.infoLine + InspectorPx(16),
                          width - margin, layout.openLine - InspectorPx(16)};
    }
    if (layout.showOpenWith)
        layout.open = {margin, layout.openLine + InspectorPx(42),
                       width - margin, layout.openLine + InspectorPx(78)};
    return layout;
}

RECT InspectorClientRect() {
    RECT client = {};
    if (IsWindow(Context().previewWindow)) GetClientRect(Context().previewWindow, &client);
    return client;
}

void InvalidateAudioPlayback() {
    RECT client = InspectorClientRect();
    InspectorLayout layout = GetInspectorLayout(client);
    RECT area = {}, visible = {};
    UnionRect(&area, Context().previewVideo ? &layout.seek : &layout.wave, &layout.time);
    InflateRect(&area, InspectorPx(3), InspectorPx(3));
    OffsetRect(&area, 0, -Context().inspectorScrollY);
    if (IntersectRect(&visible, &area, &client))
        InvalidateRect(Context().previewWindow, &visible, FALSE);
}

void UpdateVideoFrame() {
    if (!IsWindow(Context().videoWindow)) return;
    RECT destination = {};
    GetClientRect(Context().videoWindow, &destination);
    if (Context().mediaEngineEx) {
        MFARGB border = {};
        border.rgbAlpha = 255;
        Context().mediaEngineEx->UpdateVideoStream(nullptr, &destination, &border);
    }
    if (Context().directShowVideo)
        Context().directShowVideo->SetWindowPosition(destination.left, destination.top,
            destination.right - destination.left, destination.bottom - destination.top);
}

void PositionVideoPreview() {
    if (!IsWindow(Context().videoWindow)) return;
    if (!Context().previewVideo) { ShowWindow(Context().videoWindow, SW_HIDE); return; }
    RECT area = GetInspectorLayout(InspectorClientRect()).wave;
    DWORD width = Context().videoWidth, height = Context().videoHeight;
    if (Context().mediaEngine) Context().mediaEngine->GetVideoAspectRatio(&width, &height);
    int availableWidth = area.right - area.left;
    int availableHeight = area.bottom - area.top;
    if (width && height && availableWidth > 0 && availableHeight > 0) {
        double scale = std::min(static_cast<double>(availableWidth) / width,
                                static_cast<double>(availableHeight) / height);
        int w = std::max(1, static_cast<int>(width * scale));
        int h = std::max(1, static_cast<int>(height * scale));
        // Center the actual rendering window, so native Media Engine drawing
        // starts at its origin while the picture stays above the play button.
        area = {area.left + (availableWidth - w) / 2,
                area.top + (availableHeight - h) / 2,
                area.left + (availableWidth + w) / 2,
                area.top + (availableHeight + h) / 2};
    }
    OffsetRect(&area, 0, -Context().inspectorScrollY);
    RECT previous = {};
    GetWindowRect(Context().videoWindow, &previous);
    MapWindowPoints(nullptr, Context().previewWindow, reinterpret_cast<POINT*>(&previous), 2);
    if (!EqualRect(&previous, &area)) {
        SetWindowPos(Context().videoWindow, nullptr, area.left, area.top,
            std::max(1L, area.right - area.left), std::max(1L, area.bottom - area.top),
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    } else if (!(GetWindowLongPtrW(Context().videoWindow, GWL_STYLE) & WS_VISIBLE)) {
        ShowWindow(Context().videoWindow, SW_SHOWNOACTIVATE);
    }
    // A newly attached renderer needs its aspect-fit rectangle even when the
    // preview host already has the right size and position.
    UpdateVideoFrame();
}

LRESULT CALLBACK VideoPreviewProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    CallbackScope callbackScope;
    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lp);
        if (!create->lpCreateParams ||
            !SetPropW(hwnd, kSessionProperty, create->lpCreateParams)) return FALSE;
    }
    auto* owner = reinterpret_cast<Session*>(GetPropW(hwnd, kSessionProperty));
    if (!owner) return DefWindowProcW(hwnd, message, wp, lp);
    SessionScope scope(owner);
    if (message == WM_NCDESTROY) {
        RemovePropW(hwnd, kSessionProperty);
        return DefWindowProcW(hwnd, message, wp, lp);
    }
    switch (message) {
        case WM_SIZE:
            UpdateVideoFrame();
            return 0;
        case WM_ERASEBKGND: return 1;
        case WM_LBUTTONDOWN:
            SetFocus(Context().previewWindow);
            SendMessageW(Context().previewWindow, WM_KEYDOWN, VK_SPACE, 0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps = {};
            HDC dc = BeginPaint(hwnd, &ps);
            if (!Context().mediaReady && !Context().directShowVideo) {
                RECT rect = {};
                GetClientRect(hwnd, &rect);
                FillRect(dc, &rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
                if (!Context().previewMessage.empty()) {
                    SetBkMode(dc, TRANSPARENT);
                    SetTextColor(dc, RGB(190, 190, 190));
                    HGDIOBJ old = Context().inspectorCaptionFont ? SelectObject(dc, Context().inspectorCaptionFont) : nullptr;
                    DrawTextW(dc, Context().previewMessage.c_str(), -1, &rect,
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
                    if (old) SelectObject(dc, old);
                }
            } else if (Context().mediaEngineEx) {
                Context().mediaEngineEx->UpdateVideoStream(nullptr, nullptr, nullptr);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

bool CreateVideoWindow() {
    if (IsWindow(Context().videoWindow)) return true;
    Context().videoWindow = CreateWindowExW(0, kVideoClass, L"",
        WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 10, 10, Context().previewWindow,
        nullptr, HINST_THISCOMPONENT, &Context());
    PositionVideoPreview();
    return Context().videoWindow != nullptr;
}

void PositionNativePreview() {
    PositionVideoPreview();
    if (!Context().previewHandler || !IsWindow(Context().previewWindow)) return;
    RECT area = GetInspectorLayout(InspectorClientRect()).preview;
    OffsetRect(&area, 0, -Context().inspectorScrollY);
    Context().previewHandler->SetRect(&area);
}

void UpdateInspectorScroll() {
    if (!IsWindow(Context().previewWindow)) return;
    RECT client = InspectorClientRect();
    InspectorLayout layout = GetInspectorLayout(client);
    SCROLLINFO info = {};
    info.cbSize = sizeof(info);
    info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    info.nMin = 0;
    info.nMax = layout.height - 1;
    info.nPage = std::max(0L, client.bottom);
    Context().inspectorScrollY = std::clamp(Context().inspectorScrollY, 0,
        std::max(0, layout.height - static_cast<int>(client.bottom)));
    info.nPos = Context().inspectorScrollY;
    SetScrollInfo(Context().previewWindow, SB_VERT, &info, TRUE);
    PositionNativePreview();
}

void ScrollInspector(int position) {
    RECT client = InspectorClientRect();
    int maximum = std::max(0, GetInspectorLayout(client).height -
                              static_cast<int>(client.bottom));
    int previous = Context().inspectorScrollY;
    Context().inspectorScrollY = std::clamp(position, 0, maximum);
    if (previous == Context().inspectorScrollY) return;
    SetScrollPos(Context().previewWindow, SB_VERT, Context().inspectorScrollY, TRUE);
    PositionNativePreview();
    InvalidateRect(Context().previewWindow, nullptr, FALSE);
}

std::wstring FormatFileSize(ULONGLONG bytes) {
    wchar_t text[64] = {};
    if (bytes >= 1048576)
        swprintf(text, ARRAYSIZE(text), L"%.2f MB", bytes / 1048576.0);
    else if (bytes >= 1024)
        swprintf(text, ARRAYSIZE(text), L"%.1f KB", bytes / 1024.0);
    else
        swprintf(text, ARRAYSIZE(text), L"%llu bytes", bytes);
    return text;
}

std::wstring FormatFileDate(FILETIME fileTime) {
    FILETIME local = {};
    SYSTEMTIME time = {};
    if ((!fileTime.dwLowDateTime && !fileTime.dwHighDateTime) ||
        !FileTimeToLocalFileTime(&fileTime, &local) ||
        !FileTimeToSystemTime(&local, &time)) return L"—";
    wchar_t date[96] = {}, clock[64] = {};
    if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE,
            &time, nullptr, date, ARRAYSIZE(date), nullptr) ||
        !GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS,
            &time, nullptr, clock, ARRAYSIZE(clock))) return L"—";
    return std::wstring(date) + L" " + clock;
}

std::wstring AudioDurationText() {
    if (Context().audioDuration <= 0) return L"—";
    wchar_t text[48] = {};
    unsigned long long ms = Context().audioDuration / 10000;
    swprintf(text, ARRAYSIZE(text), L"%llu:%02llu.%03llu",
             ms / 60000, (ms / 1000) % 60, ms % 1000);
    return text;
}

std::wstring AudioFormatText() {
    if (Context().previewVideo) {
        std::wstring extension = PathFindExtensionW(Context().previewPath.c_str());
        if (!extension.empty() && extension[0] == L'.') extension.erase(0, 1);
        CharUpperBuffW(extension.data(), static_cast<DWORD>(extension.size()));
        return extension.empty() ? L"Video" : extension;
    }
    if (Context().wavePreview && !Context().wavePreview->codec.empty()) return Context().wavePreview->codec;
    if (Context().wavePreview && Context().wavePreview->supported &&
        (Context().wavePreview->format == 1 || Context().wavePreview->format == 3))
        return Context().wavePreview->format == 3 ? L"IEEE float (WAV)" : L"PCM (WAV)";
    if (!Context().audioFormat.empty()) return Context().audioFormat;
    std::wstring extension = PathFindExtensionW(Context().previewPath.c_str());
    if (!extension.empty() && extension[0] == L'.') extension.erase(0, 1);
    if (extension.empty()) return L"—";
    CharUpperBuffW(extension.data(), static_cast<DWORD>(extension.size()));
    return extension;
}

void DeleteInspectorFonts() {
    if (Context().inspectorHeadingFont) DeleteObject(Context().inspectorHeadingFont);
    if (Context().inspectorNameFont) DeleteObject(Context().inspectorNameFont);
    if (Context().inspectorBodyFont) DeleteObject(Context().inspectorBodyFont);
    if (Context().inspectorCaptionFont) DeleteObject(Context().inspectorCaptionFont);
    Context().inspectorHeadingFont = nullptr;
    Context().inspectorNameFont = nullptr;
    Context().inspectorBodyFont = nullptr;
    Context().inspectorCaptionFont = nullptr;
    Context().inspectorFontDpi = 0;
}

HFONT MakeInspectorFont(int points, int weight, UINT dpi) {
    return CreateFontW(-MulDiv(points, dpi, 72), 0, 0, 0, weight,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
        L"Segoe UI Variable Text");
}

void EnsureInspectorFonts(HWND window) {
    UINT dpi = GetDpiForWindow(window);
    if (!dpi) dpi = 96;
    if (dpi == Context().inspectorFontDpi && Context().inspectorBodyFont) return;
    DeleteInspectorFonts();
    Context().inspectorHeadingFont = MakeInspectorFont(9, FW_NORMAL, dpi);
    Context().inspectorNameFont = MakeInspectorFont(11, FW_SEMIBOLD, dpi);
    Context().inspectorBodyFont = MakeInspectorFont(10, FW_NORMAL, dpi);
    Context().inspectorCaptionFont = MakeInspectorFont(9, FW_NORMAL, dpi);
    Context().inspectorFontDpi = dpi;
}

void InspectorText(HDC dc, HFONT font, COLORREF color, RECT area,
                   const std::wstring& text, UINT flags =
                   DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS) {
    HGDIOBJ old = SelectObject(dc, font);
    SetTextColor(dc, color);
    DrawTextW(dc, text.c_str(), -1, &area, flags | DT_NOPREFIX);
    SelectObject(dc, old);
}

void PaintInfoRow(HDC dc, int width, int row,
                  const wchar_t* label, const std::wstring& value) {
    int y = InspectorPx(106 + row * 21);
    int valueX = std::max(InspectorPx(132), width * 47 / 100);
    RECT left = {InspectorPx(20), y, valueX - InspectorPx(8), y + InspectorPx(21)};
    RECT right = {valueX, y, width - InspectorPx(20), y + InspectorPx(21)};
    InspectorText(dc, Context().inspectorBodyFont,
        InspectorBlend(Context().inspectorText, Context().inspectorBackground, 76), left, label);
    InspectorText(dc, Context().inspectorBodyFont, Context().inspectorText, right, value);
}

void ApplyAudioVolume() {
    if (Context().mediaEngine) {
        HRESULT hr = Context().mediaEngine->SetVolume(Context().audioVolume / 100.0);
        if (FAILED(hr)) Wh_Log(L"[Columns] Media volume failed: %08X", hr);
        return;
    }
    if (!Context().audioVolumeControl) return;
    long level = Context().audioVolume <= 0 ? -10000 :
        static_cast<long>(2000.0 * std::log10(Context().audioVolume / 100.0));
    HRESULT hr = Context().audioVolumeControl->put_Volume(std::clamp(level, -10000L, 0L));
    if (FAILED(hr)) Wh_Log(L"[Columns] Audio volume failed: %08X", hr);
}

void SeekAudio(LONGLONG position) {
    if (!MediaCanSeek() || Context().audioDuration <= 0) return;
    position = std::clamp(position, 0LL, Context().audioDuration);
    HRESULT hr = Context().mediaEngine ? Context().mediaEngine->SetCurrentTime(position / 10000000.0) :
        Context().audioSeeking->SetPositions(&position, AM_SEEKING_AbsolutePositioning,
            nullptr, AM_SEEKING_NoPositioning);
    if (FAILED(hr)) Wh_Log(L"[Columns] Audio seek failed: %08X", hr);
    InvalidateRect(Context().previewWindow, nullptr, FALSE);
}

void ToggleAudioPlayback() {
    if (!MediaCanPlay()) return;
    bool play = !Context().audioPlaying;
    if (play && Context().audioDuration > 0 &&
        AudioPosition() >= Context().audioDuration - 10000) SeekAudio(0);
    HRESULT hr = Context().mediaEngine ? (play ? Context().mediaEngine->Play() : Context().mediaEngine->Pause()) :
        (play ? Context().audioControl->Run() : Context().audioControl->Pause());
    if (SUCCEEDED(hr)) {
        Context().audioPlaying = play;
        UpdateAudioPlaybackTimer();
    }
    else Wh_Log(L"[Columns] Audio playback failed: %08X", hr);
    InvalidateRect(Context().previewWindow, nullptr, FALSE);
}

void InspectorDragTo(POINT point) {
    InspectorLayout layout = GetInspectorLayout(InspectorClientRect());
    RECT timeline = Context().previewVideo ? layout.seek : layout.wave;
    if (Context().inspectorDrag == 1 && timeline.right > timeline.left) {
        double fraction = std::clamp(static_cast<double>(point.x - timeline.left) /
            (timeline.right - timeline.left), 0.0, 1.0);
        SeekAudio(static_cast<LONGLONG>(Context().audioDuration * fraction));
    } else if (Context().inspectorDrag == 2 && layout.volume.right > layout.volume.left) {
        int percent = std::clamp(MulDiv(point.x - layout.volume.left, 100,
            layout.volume.right - layout.volume.left), 0, 100);
        Context().audioVolume = percent;
        if (percent > 0) Context().audioUnmutedVolume = percent;
        ApplyAudioVolume();
        InvalidateRect(Context().previewWindow, nullptr, FALSE);
    }
}

void ReadOpenWithDefault() {
    Context().openWithName.clear();
    if (Context().openWithIcon) DestroyIcon(Context().openWithIcon);
    Context().openWithIcon = nullptr;
    if (Context().previewIsVirtualFolder) return;
    Context().openWithName = Context().previewIsFolder ? L"File Explorer" : L"Choose an app";
    if (Context().previewIsFolder) {
        if (Context().previewIcon) Context().openWithIcon = CopyIcon(Context().previewIcon);
        return;
    }
    const wchar_t* extension = PathFindExtensionW(Context().previewPath.c_str());
    wchar_t name[512] = {};
    DWORD count = ARRAYSIZE(name);
    if (SUCCEEDED(AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_FRIENDLYAPPNAME,
            extension, L"open", name, &count)) && name[0]) Context().openWithName = name;
    std::wstring executable(MAX_PATH, L'\0');
    count = static_cast<DWORD>(executable.size());
    HRESULT executableResult = AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_EXECUTABLE,
        extension, L"open", executable.data(), &count);
    if (executableResult == E_POINTER && count > executable.size()) {
        executable.resize(count);
        executableResult = AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_EXECUTABLE,
            extension, L"open", executable.data(), &count);
    }
    if (SUCCEEDED(executableResult)) {
        SHFILEINFOW info = {};
        if (SHGetFileInfoW(executable.c_str(), 0, &info, sizeof(info), SHGFI_ICON | SHGFI_SMALLICON))
            Context().openWithIcon = info.hIcon;
    }
}

void ShowOpenWithMenu(HWND window) {
    const std::wstring path = Context().previewPath;
    if (path.empty() || Context().previewIsVirtualFolder) return;
    const bool folder = Context().previewIsFolder;
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    std::vector<IAssocHandler*> handlers;
    if (!folder) {
        IEnumAssocHandlers* enumeration = nullptr;
        if (SUCCEEDED(SHAssocEnumHandlers(PathFindExtensionW(path.c_str()),
                ASSOC_FILTER_NONE, &enumeration)) && enumeration) {
            IAssocHandler* handler = nullptr;
            ULONG fetched = 0;
            while (handlers.size() < 80 &&
                enumeration->Next(1, &handler, &fetched) == S_OK && fetched == 1) {
                PWSTR name = nullptr;
                if (SUCCEEDED(handler->GetUIName(&name)) && name && *name) {
                    handlers.push_back(handler);
                    std::wstring label = name;
                    // Menu ampersands are mnemonic markers, not literal text.
                    for (size_t i = 0; (i = label.find(L'&', i)) != std::wstring::npos; i += 2)
                        label.insert(i, 1, L'&');
                    AppendMenuW(menu, MF_STRING, 100 + handlers.size(), label.c_str());
                } else {
                    handler->Release();
                }
                CoTaskMemFree(name);
                handler = nullptr;
            }
            enumeration->Release();
        }
    }
    if (!handlers.empty()) AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 1, folder ? L"File Explorer" : L"Default app");
    if (!folder) AppendMenuW(menu, MF_STRING, 2, L"Choose another app…");
    RECT button = GetInspectorLayout(InspectorClientRect()).open;
    POINT anchor = {button.left, button.bottom - Context().inspectorScrollY};
    ClientToScreen(window, &anchor);
    UINT command = TrackPopupMenuEx(menu,
        TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
        anchor.x, anchor.y, window, nullptr);
    DestroyMenu(menu);
    HRESULT hr = S_OK;
    if (command) {
        if (Context().audioPlaying) PauseMediaPreview();
        if (command == 1) {
            SHELLEXECUTEINFOW execute = {};
            execute.cbSize = sizeof(execute);
            execute.fMask = SEE_MASK_FLAG_NO_UI;
            execute.hwnd = window;
            execute.lpVerb = L"open";
            execute.lpFile = path.c_str();
            execute.nShow = SW_SHOWNORMAL;
            if (!ShellExecuteExW(&execute)) hr = HRESULT_FROM_WIN32(GetLastError());
        } else if (command == 2) {
            OPENASINFO open = {};
            open.pcszFile = path.c_str();
            open.oaifInFlags = static_cast<OPEN_AS_INFO_FLAGS>(OAIF_EXEC | OAIF_HIDE_REGISTRATION);
            hr = SHOpenWithDialog(window, &open);
        } else if (command > 100 && command - 101 < handlers.size()) {
            IShellItem* item = nullptr;
            IDataObject* data = nullptr;
            hr = SHCreateItemFromParsingName(path.c_str(), nullptr,
                IID_IShellItem, reinterpret_cast<void**>(&item));
            if (SUCCEEDED(hr)) hr = item->BindToHandler(nullptr, BHID_DataObject,
                IID_IDataObject, reinterpret_cast<void**>(&data));
            if (SUCCEEDED(hr)) hr = handlers[command - 101]->Invoke(data);
            if (data) data->Release();
            if (item) item->Release();
        }
    }
    for (IAssocHandler* handler : handlers) handler->Release();
    if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        Wh_Log(L"[Columns] Open with failed: %08X", hr);
        MessageBoxW(window, L"Windows could not open this file with the selected app.",
                    L"Open with", MB_OK | MB_ICONINFORMATION);
    }
    if (IsWindow(window)) InvalidateRect(window, nullptr, FALSE);
}

void InspectorLine(HDC dc, int left, int right, int y, COLORREF color,
                   int thickness = 1) {
    HPEN pen = CreatePen(PS_SOLID, thickness, color);
    HGDIOBJ old = SelectObject(dc, pen);
    MoveToEx(dc, left, y, nullptr);
    LineTo(dc, right, y);
    SelectObject(dc, old);
    DeleteObject(pen);
}

void PaintAudioGlyph(HDC dc, const RECT& rect, int kind, COLORREF color) {
    // kind: play, pause, rewind, forward, speaker, muted speaker.
    int x = (rect.left + rect.right) / 2;
    int y = (rect.top + rect.bottom) / 2;
    int u = InspectorPx(7);
    HPEN pen = CreatePen(PS_SOLID, std::max(1, InspectorPx(1)), color);
    HBRUSH brush = CreateSolidBrush(color);
    HGDIOBJ oldPen = SelectObject(dc, pen), oldBrush = SelectObject(dc, brush);
    if (kind == 0) {
        POINT points[] = {{x - u / 2, y - u}, {x - u / 2, y + u}, {x + u, y}};
        Polygon(dc, points, 3);
    } else if (kind == 1) {
        RECT a = {x - u, y - u, x - u / 3, y + u};
        RECT b = {x + u / 3, y - u, x + u, y + u};
        FillRect(dc, &a, brush); FillRect(dc, &b, brush);
    } else if (kind == 2 || kind == 3) {
        int direction = kind == 2 ? -1 : 1;
        for (int offset : {-4, 4}) {
            int center = x + InspectorPx(offset);
            POINT points[] = {{center - direction * u / 2, y - u},
                              {center - direction * u / 2, y + u},
                              {center + direction * u / 2, y}};
            Polygon(dc, points, 3);
        }
    } else {
        POINT points[] = {{x - u, y - u / 3}, {x - u / 2, y - u / 3},
                          {x + u / 3, y - u}, {x + u / 3, y + u},
                          {x - u / 2, y + u / 3}, {x - u, y + u / 3}};
        Polygon(dc, points, 6);
        if (kind == 5) {
            MoveToEx(dc, x + u / 2, y - u / 2, nullptr);
            LineTo(dc, x + u + u / 2, y + u / 2);
            MoveToEx(dc, x + u / 2, y + u / 2, nullptr);
            LineTo(dc, x + u + u / 2, y - u / 2);
        } else {
            SelectObject(dc, GetStockObject(NULL_BRUSH));
            Arc(dc, x - u / 2, y - u, x + u + u / 2, y + u,
                x + u, y + u, x + u, y - u);
        }
    }
    SelectObject(dc, oldBrush); SelectObject(dc, oldPen);
    DeleteObject(brush); DeleteObject(pen);
}

void PaintInspector(HDC dc, RECT client) {
    InspectorLayout layout = GetInspectorLayout(client);
    int margin = InspectorPx(20);
    COLORREF muted = InspectorBlend(Context().inspectorText, Context().inspectorBackground, 76);
    COLORREF line = InspectorBlend(Context().inspectorText, Context().inspectorBackground, 18);
    COLORREF waveform = InspectorBlend(Context().inspectorText, Context().inspectorBackground, 54);
    COLORREF control = InspectorBlend(Context().inspectorText, Context().inspectorBackground, 80);
    COLORREF accent = InspectorBlend(Context().inspectorText, Context().inspectorBackground, 88);
    RECT canvas = {0, 0, client.right, layout.height};
    PaintColumnBackground(Context().previewWindow, dc, &canvas);
    SetBkMode(dc, TRANSPARENT);
    RECT heading = {margin, InspectorPx(5), client.right - margin, InspectorPx(29)};
    InspectorText(dc, Context().inspectorHeadingFont, muted, heading, L"File Info");
    int iconSize = InspectorPx(34);
    if (Context().previewIcon)
        DrawIconEx(dc, InspectorPx(26), InspectorPx(45), Context().previewIcon,
                   iconSize, iconSize, 0, nullptr, DI_NORMAL);
    RECT name = {InspectorPx(78), InspectorPx(39), client.right - margin, InspectorPx(79)};
    InspectorText(dc, Context().inspectorNameFont, Context().inspectorText, name,
        Context().previewDisplayName, DT_LEFT | DT_WORDBREAK | DT_END_ELLIPSIS);
    RECT type = {InspectorPx(78), InspectorPx(81), client.right - margin, InspectorPx(101)};
    InspectorText(dc, Context().inspectorCaptionFont, muted, type, Context().previewType);
    if (Context().previewMedia) {
        PaintInfoRow(dc, client.right, 0, L"Size", FormatFileSize(Context().previewFileSize));
        PaintInfoRow(dc, client.right, 1, L"Duration", AudioDurationText());
        ULONG rate = Context().wavePreview && Context().wavePreview->supported ? Context().wavePreview->sampleRate : Context().audioSampleRate;
        ULONG bits = Context().wavePreview && Context().wavePreview->supported ? Context().wavePreview->bits : Context().audioBits;
        ULONG channels = Context().wavePreview && Context().wavePreview->supported ? Context().wavePreview->channels : Context().audioChannels;
        wchar_t rateText[64] = L"—";
        if (rate) {
            wchar_t raw[32] = {};
            swprintf(raw, ARRAYSIZE(raw), L"%lu", rate);
            NUMBERFMTW format = {0, 0, 3, const_cast<PWSTR>(L"."), const_cast<PWSTR>(L","), 1};
            if (!GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, raw, &format, rateText, ARRAYSIZE(rateText)))
                wcscpy_s(rateText, raw);
            wcscat_s(rateText, L" Hz");
        }
        if (Context().previewVideo) {
            PaintInfoRow(dc, client.right, 2, L"Dimensions", Context().videoWidth && Context().videoHeight ?
                std::to_wstring(Context().videoWidth) + L" × " + std::to_wstring(Context().videoHeight) : L"—");
            wchar_t frameRate[64] = L"—";
            if (Context().videoFrameRate) swprintf(frameRate, ARRAYSIZE(frameRate), L"%.2f fps", Context().videoFrameRate / 1000.0);
            PaintInfoRow(dc, client.right, 3, L"Frame Rate", frameRate);
            PaintInfoRow(dc, client.right, 4, L"Audio", channels == 1 ? L"Mono" : channels == 2 ? L"Stereo" :
                channels ? std::to_wstring(channels) + L" channels" : L"—");
        } else {
            PaintInfoRow(dc, client.right, 2, L"Sample Rate", rateText);
            PaintInfoRow(dc, client.right, 3, L"Bit Depth", bits ? std::to_wstring(bits) + L" bit" : L"—");
            PaintInfoRow(dc, client.right, 4, L"Channels", channels == 1 ? L"Mono" : channels == 2 ? L"Stereo" :
                         channels ? std::to_wstring(channels) + L" channels" : L"—");
        }
        PaintInfoRow(dc, client.right, 5, L"Format", AudioFormatText());
        PaintInfoRow(dc, client.right, 6, L"Modified", FormatFileDate(Context().previewModified));
        PaintInfoRow(dc, client.right, 7, L"Created", FormatFileDate(Context().previewCreated));
    } else {
        int row = 0;
        if (!Context().previewIsFolder) PaintInfoRow(dc, client.right, row++, L"Size", FormatFileSize(Context().previewFileSize));
        PaintInfoRow(dc, client.right, row++, L"Modified", FormatFileDate(Context().previewModified));
        PaintInfoRow(dc, client.right, row, L"Created", FormatFileDate(Context().previewCreated));
    }
    InspectorLine(dc, margin, client.right - margin, layout.infoLine, line);
    if (Context().previewBitmap && !Context().previewMedia) {
        BITMAP bitmap = {};
        RECT area = layout.preview;
        if (GetObjectW(Context().previewBitmap, sizeof(bitmap), &bitmap) && bitmap.bmWidth > 0 && bitmap.bmHeight > 0) {
            double scale = std::min(static_cast<double>(area.right - area.left) / bitmap.bmWidth,
                                    static_cast<double>(area.bottom - area.top) / bitmap.bmHeight);
            int w = std::max(1, static_cast<int>(bitmap.bmWidth * scale));
            int h = std::max(1, static_cast<int>(bitmap.bmHeight * scale));
            HDC source = CreateCompatibleDC(dc);
            HGDIOBJ old = SelectObject(source, Context().previewBitmap);
            SetStretchBltMode(dc, HALFTONE);
            SetBrushOrgEx(dc, 0, 0, nullptr);
            StretchBlt(dc, area.left + (area.right - area.left - w) / 2,
                area.top + (area.bottom - area.top - h) / 2, w, h,
                source, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
            SelectObject(source, old); DeleteDC(source);
        }
    }
    if (Context().previewMedia) {
        RECT title = {margin, layout.infoLine + InspectorPx(10),
                      client.right - margin, layout.infoLine + InspectorPx(34)};
        InspectorText(dc, Context().inspectorHeadingFont, muted, title, L"Playback");
        RECT wave = layout.wave;
        if (!Context().previewVideo) {
        int mid = (wave.top + wave.bottom) / 2;
        InspectorLine(dc, wave.left, wave.right, mid, line);
        if (Context().wavePreview && Context().wavePreview->supported && !Context().wavePreview->peaks.empty()) {
            HPEN bars = CreatePen(PS_SOLID, 1, waveform);
            HGDIOBJ old = SelectObject(dc, bars);
            int width = wave.right - wave.left;
            int count = static_cast<int>(Context().wavePreview->peaks.size());
            for (int x = 0; x < width; ++x) {
                int first = x * count / width;
                int last = std::max(first + 1, (x + 1) * count / width);
                float peak = 0;
                for (int bin = first; bin < last && bin < count; ++bin)
                    peak = std::max(peak, Context().wavePreview->peaks[bin]);
                int amplitude = std::max(1, static_cast<int>(peak * (wave.bottom - wave.top - InspectorPx(6)) / 2));
                MoveToEx(dc, wave.left + x, mid - amplitude, nullptr);
                LineTo(dc, wave.left + x, mid + amplitude + 1);
            }
            SelectObject(dc, old); DeleteObject(bars);
        } else {
            InspectorText(dc, Context().inspectorCaptionFont, muted, wave,
                Context().waveWanted ? L"Loading waveform…" : L"Waveform unavailable",
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        LONGLONG position = AudioPosition();
        if (Context().audioDuration > 0) {
            int playX = wave.left + static_cast<int>((wave.right - wave.left) *
                static_cast<double>(std::clamp(position, 0LL, Context().audioDuration)) / Context().audioDuration);
            HPEN pen = CreatePen(PS_SOLID, std::max(1, InspectorPx(1)), accent);
            HGDIOBJ old = SelectObject(dc, pen);
            MoveToEx(dc, playX, wave.top, nullptr); LineTo(dc, playX, wave.bottom);
            SelectObject(dc, old); DeleteObject(pen);
        }
        } else {
            int y = (layout.seek.top + layout.seek.bottom) / 2;
            InspectorLine(dc, layout.seek.left, layout.seek.right, y, line, InspectorPx(2));
            if (Context().audioDuration > 0) {
                int x = layout.seek.left + static_cast<int>((layout.seek.right - layout.seek.left) *
                    static_cast<double>(std::clamp(AudioPosition(), 0LL, Context().audioDuration)) / Context().audioDuration);
                InspectorLine(dc, layout.seek.left, x, y, control, InspectorPx(2));
                HBRUSH dot = CreateSolidBrush(control);
                RECT marker = {x - InspectorPx(2), y - InspectorPx(3),
                               x + InspectorPx(2), y + InspectorPx(3)};
                FillRect(dc, &marker, dot); DeleteObject(dot);
            }
        }
        LONGLONG position = AudioPosition();
        InspectorText(dc, Context().inspectorCaptionFont, muted, layout.time,
            AudioTime(position) + L" / " + AudioTime(Context().audioDuration),
            DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        COLORREF disabled = InspectorBlend(Context().inspectorText, Context().inspectorBackground, 35);
        PaintAudioGlyph(dc, layout.back, 2, MediaCanPlay() ?
            (Context().inspectorHover == 2 ? Context().inspectorText : control) : disabled);
        PaintAudioGlyph(dc, layout.forward, 3, MediaCanPlay() ?
            (Context().inspectorHover == 3 ? Context().inspectorText : control) : disabled);
        bool playHover = Context().inspectorHover == 1 && MediaCanPlay();
        HBRUSH button = playHover ? CreateSolidBrush(
            InspectorBlend(Context().inspectorText, Context().inspectorBackground, 9)) : nullptr;
        HPEN ring = CreatePen(PS_SOLID, std::max(1, InspectorPx(1)),
            InspectorBlend(Context().inspectorText, Context().inspectorBackground,
                MediaCanPlay() ? (playHover ? 68 : 40) : 20));
        HGDIOBJ oldBrush = SelectObject(dc, button ? button : GetStockObject(NULL_BRUSH));
        HGDIOBJ oldPen = SelectObject(dc, ring);
        Ellipse(dc, layout.play.left, layout.play.top, layout.play.right, layout.play.bottom);
        SelectObject(dc, oldBrush); SelectObject(dc, oldPen);
        if (button) DeleteObject(button);
        DeleteObject(ring);
        PaintAudioGlyph(dc, layout.play, Context().audioPlaying ? 1 : 0,
            MediaCanPlay() ? (playHover ? Context().inspectorText : accent) : disabled);
        PaintAudioGlyph(dc, layout.speaker, Context().audioVolume ? 4 : 5,
            MediaCanSetVolume() ? (Context().inspectorHover == 4 ? Context().inspectorText : control) : disabled);
        int cy = (layout.volume.top + layout.volume.bottom) / 2;
        InspectorLine(dc, layout.volume.left, layout.volume.right, cy, line, InspectorPx(2));
        int volumeX = layout.volume.left + MulDiv(layout.volume.right - layout.volume.left, Context().audioVolume, 100);
        InspectorLine(dc, layout.volume.left, volumeX, cy, control, InspectorPx(1));
        HBRUSH knob = CreateSolidBrush(Context().inspectorHover == 5 ? Context().inspectorText : control);
        oldBrush = SelectObject(dc, knob); oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
        int radius = InspectorPx(4);
        Ellipse(dc, volumeX - radius, cy - radius, volumeX + radius, cy + radius);
        SelectObject(dc, oldBrush); SelectObject(dc, oldPen); DeleteObject(knob);
    }
    if (!Context().previewMessage.empty()) {
        RECT area = Context().previewMedia ? layout.wave : layout.preview;
        if (Context().previewMedia) {
            PaintColumnBackground(Context().previewWindow, dc, &area);
        }
        InspectorText(dc, Context().inspectorCaptionFont, muted, area, Context().previewMessage,
                       DT_LEFT | DT_TOP | DT_WORDBREAK);
    }
    if (!layout.showOpenWith) return;
    InspectorLine(dc, margin, client.right - margin, layout.openLine, line);
    RECT openTitle = {margin, layout.openLine + InspectorPx(10),
                      client.right - margin, layout.openLine + InspectorPx(34)};
    InspectorText(dc, Context().inspectorHeadingFont, muted, openTitle, L"Open with");
    HPEN border = CreatePen(PS_SOLID, 1, Context().inspectorHover == 6 ? muted : line);
    HGDIOBJ oldPen = SelectObject(dc, border), oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    RoundRect(dc, layout.open.left, layout.open.top, layout.open.right, layout.open.bottom,
              InspectorPx(6), InspectorPx(6));
    SelectObject(dc, oldBrush); SelectObject(dc, oldPen); DeleteObject(border);
    int icon = InspectorPx(20);
    int iconY = (layout.open.top + layout.open.bottom - icon) / 2;
    if (Context().openWithIcon) DrawIconEx(dc, layout.open.left + InspectorPx(9), iconY,
        Context().openWithIcon, icon, icon, 0, nullptr, DI_NORMAL);
    RECT appName = layout.open;
    appName.left += InspectorPx(Context().openWithIcon ? 38 : 12);
    appName.right -= InspectorPx(30);
    InspectorText(dc, Context().inspectorBodyFont, Context().inspectorText, appName, Context().openWithName);
    int arrowX = layout.open.right - InspectorPx(16);
    int arrowY = (layout.open.top + layout.open.bottom) / 2;
    HPEN chevron = CreatePen(PS_SOLID, std::max(1, InspectorPx(1)), control);
    oldPen = SelectObject(dc, chevron);
    MoveToEx(dc, arrowX - InspectorPx(4), arrowY - InspectorPx(2), nullptr);
    LineTo(dc, arrowX, arrowY + InspectorPx(2));
    LineTo(dc, arrowX + InspectorPx(4), arrowY - InspectorPx(2));
    SelectObject(dc, oldPen); DeleteObject(chevron);
}

int InspectorHoverAt(POINT point) {
    point.y += Context().inspectorScrollY;
    InspectorLayout layout = GetInspectorLayout(InspectorClientRect());
    if (layout.showOpenWith && PtInRect(&layout.open, point)) return 6;
    if (!Context().previewMedia) return 0;
    if (MediaCanPlay()) {
        if (PtInRect(&layout.play, point)) return 1;
        if (PtInRect(&layout.back, point)) return 2;
        if (PtInRect(&layout.forward, point)) return 3;
    }
    if (MediaCanSetVolume()) {
        if (PtInRect(&layout.speaker, point)) return 4;
        RECT volume = layout.volume;
        InflateRect(&volume, InspectorPx(8), InspectorPx(12));
        if (PtInRect(&volume, point)) return 5;
    }
    RECT timeline = Context().previewVideo ? layout.seek : layout.wave;
    if (Context().previewVideo) InflateRect(&timeline, 0, InspectorPx(8));
    if (MediaCanSeek() && PtInRect(&timeline, point)) return 7;
    return 0;
}

LRESULT CALLBACK PreviewSlotProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    CallbackScope callbackScope;
    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lp);
        if (!create->lpCreateParams ||
            !SetPropW(hwnd, kSessionProperty, create->lpCreateParams)) return FALSE;
    }
    auto* owner = reinterpret_cast<Session*>(GetPropW(hwnd, kSessionProperty));
    if (!owner) return DefWindowProcW(hwnd, message, wp, lp);
    SessionScope scope(owner);
    if (message == WM_NCDESTROY) {
        KillTimer(hwnd, kAudioTimerId);
        Context().audioTimerRunning = false;
        RemovePropW(hwnd, kSessionProperty);
        return DefWindowProcW(hwnd, message, wp, lp);
    }
    switch (message) {
        case kMediaEventMessage:
            if (wp == Context().previewGeneration)
                HandleMediaEvent(static_cast<DWORD>(lp), static_cast<HRESULT>(static_cast<ULONGLONG>(lp) >> 32));
            return 0;
        case WM_TIMER:
            if (wp == kAudioTimerId) {
                if (!IsWindowVisible(hwnd)) PauseMediaPreview();
                if (!Context().audioPlaying || !(Context().audioGraph || Context().mediaEngine)) {
                    UpdateAudioPlaybackTimer();
                    return 0;
                }
                if (Context().audioPlaying && Context().audioDuration > 0 && AudioPosition() >= Context().audioDuration - 100000) {
                    PauseMediaPreview();
                    SeekAudio(0);
                }
                if (Context().audioPlaying) InvalidateAudioPlayback();
                return 0;
            }
            break;
        case WM_SHOWWINDOW:
            if (!wp) PauseMediaPreview();
            return 0;
        case WM_DESTROY:
            PauseMediaPreview();
            return 0;
        case WM_SIZE:
            UpdateInspectorScroll();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            if (IsWindow(Context().overlay))
                PostMessageW(Context().overlay, kPaletteRefreshMessage, Context().identity, 0);
            return 0;
        case WM_VSCROLL: {
            SCROLLINFO info = {};
            info.cbSize = sizeof(info);
            info.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &info);
            int y = Context().inspectorScrollY;
            switch (LOWORD(wp)) {
                case SB_LINEUP: y -= InspectorPx(24); break;
                case SB_LINEDOWN: y += InspectorPx(24); break;
                case SB_PAGEUP: y -= info.nPage; break;
                case SB_PAGEDOWN: y += info.nPage; break;
                case SB_THUMBTRACK: case SB_THUMBPOSITION: y = info.nTrackPos; break;
                case SB_TOP: y = 0; break;
                case SB_BOTTOM: y = info.nMax; break;
            }
            ScrollInspector(y);
            return 0;
        }
        case WM_MOUSEWHEEL:
            ScrollInspector(Context().inspectorScrollY - MulDiv(GET_WHEEL_DELTA_WPARAM(wp), InspectorPx(48), WHEEL_DELTA));
            return 0;
        case WM_LBUTTONDOWN: {
            SetFocus(hwnd);
            POINT point = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp) + Context().inspectorScrollY};
            InspectorLayout layout = GetInspectorLayout(InspectorClientRect());
            RECT volume = layout.volume;
            InflateRect(&volume, InspectorPx(8), InspectorPx(12));
            RECT timeline = Context().previewVideo ? layout.seek : layout.wave;
            if (Context().previewVideo) InflateRect(&timeline, 0, InspectorPx(8));
            if (layout.showOpenWith && PtInRect(&layout.open, point)) { ShowOpenWithMenu(hwnd); return 0; }
            if (!Context().previewMedia) return 0;
            if (PtInRect(&layout.play, point)) ToggleAudioPlayback();
            else if (PtInRect(&layout.back, point)) SeekAudio(AudioPosition() - 50000000LL);
            else if (PtInRect(&layout.forward, point)) SeekAudio(AudioPosition() + 50000000LL);
            else if (PtInRect(&layout.speaker, point) && MediaCanSetVolume()) {
                if (Context().audioVolume) { Context().audioUnmutedVolume = Context().audioVolume; Context().audioVolume = 0; }
                else Context().audioVolume = std::max(1, Context().audioUnmutedVolume);
                ApplyAudioVolume(); InvalidateRect(hwnd, nullptr, FALSE);
            } else if (PtInRect(&timeline, point) && MediaCanSeek()) {
                Context().inspectorDrag = 1; SetCapture(hwnd); InspectorDragTo(point);
            } else if (PtInRect(&volume, point) && MediaCanSetVolume()) {
                Context().inspectorDrag = 2; SetCapture(hwnd); InspectorDragTo(point);
            }
            return 0;
        }
        case WM_MOUSEMOVE:
            if (GetCapture() == hwnd && Context().inspectorDrag) {
                POINT point = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp) + Context().inspectorScrollY};
                InspectorDragTo(point);
                return 0;
            }
            {
                POINT point = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                int hover = InspectorHoverAt(point);
                if (hover != Context().inspectorHover) {
                    Context().inspectorHover = hover;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                TRACKMOUSEEVENT tracking = {};
                tracking.cbSize = sizeof(tracking);
                tracking.dwFlags = TME_LEAVE;
                tracking.hwndTrack = hwnd;
                TrackMouseEvent(&tracking);
                return 0;
            }
        case WM_MOUSELEAVE:
            if (Context().inspectorHover) {
                Context().inspectorHover = 0;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_SETCURSOR:
            if (LOWORD(lp) == HTCLIENT) {
                POINT point = {};
                GetCursorPos(&point);
                ScreenToClient(hwnd, &point);
                if (InspectorHoverAt(point)) {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }
            }
            break;
        case WM_LBUTTONUP:
            if (GetCapture() == hwnd) {
                POINT point = {GET_X_LPARAM(lp), GET_Y_LPARAM(lp) + Context().inspectorScrollY};
                InspectorDragTo(point); Context().inspectorDrag = 0; ReleaseCapture();
            }
            return 0;
        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
            Context().inspectorDrag = 0;
            return 0;
        case WM_KEYDOWN:
            if (wp == VK_SPACE) ToggleAudioPlayback();
            else if (wp == VK_LEFT) SeekAudio(AudioPosition() - 50000000LL);
            else if (wp == VK_RIGHT) SeekAudio(AudioPosition() + 50000000LL);
            else if ((wp == VK_RETURN || wp == VK_APPS) && !Context().previewIsVirtualFolder)
                ShowOpenWithMenu(hwnd);
            else break;
            return 0;
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps = {};
            HDC target = BeginPaint(hwnd, &ps);
            RECT client = {};
            GetClientRect(hwnd, &client);
            EnsureInspectorFonts(hwnd);
            HDC buffer = CreateCompatibleDC(target);
            HBITMAP bitmap = CreateCompatibleBitmap(target, std::max(1L, client.right), std::max(1L, client.bottom));
            if (buffer && bitmap) {
                HGDIOBJ old = SelectObject(buffer, bitmap);
                IntersectClipRect(buffer, ps.rcPaint.left, ps.rcPaint.top,
                    ps.rcPaint.right, ps.rcPaint.bottom);
                SetViewportOrgEx(buffer, 0, -Context().inspectorScrollY, nullptr);
                PaintInspector(buffer, client);
                SetViewportOrgEx(buffer, 0, 0, nullptr);
                BitBlt(target, 0, 0, client.right, client.bottom, buffer, 0, 0, SRCCOPY);
                SelectObject(buffer, old);
            } else {
                int saved = SaveDC(target);
                SetViewportOrgEx(target, 0, -Context().inspectorScrollY, nullptr);
                PaintInspector(target, client);
                RestoreDC(target, saved);
            }
            if (bitmap) DeleteObject(bitmap);
            if (buffer) DeleteDC(buffer);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

void ClearPreview() {
    if (Context().previewIndex < 0) return;
    Context().previewGeneration = g_previewSequence.fetch_add(1) + 1;
    Context().imageWanted = false;
    Context().waveWanted = false;
    if (Context().imageJob) SetEvent(Context().imageJob->cancel);
    if (Context().waveJob) SetEvent(Context().waveJob->cancel);
    delete Context().wavePreview;
    Context().wavePreview = nullptr;
    StopAudioPreview();
    if (Context().previewBitmap) DeleteObject(Context().previewBitmap);
    Context().previewBitmap = nullptr;
    if (Context().previewHandler) {
        Context().previewHandler->Unload();
        Context().previewHandler->Release();
        Context().previewHandler = nullptr;
    }
    if (IsWindow(Context().previewWindow)) DestroyWindow(Context().previewWindow);
    Context().previewWindow = nullptr;
    Context().previewPath.clear();
    Context().previewDisplayName.clear();
    Context().previewMessage.clear();
    Context().previewFileSize = 0;
    Context().previewModified = {};
    Context().previewCreated = {};
    if (Context().previewIcon) DestroyIcon(Context().previewIcon);
    if (Context().openWithIcon) DestroyIcon(Context().openWithIcon);
    Context().previewIcon = nullptr;
    Context().openWithIcon = nullptr;
    Context().openWithName.clear();
    Context().previewType.clear();
    Context().previewIsFolder = false;
    Context().previewIsVirtualFolder = false;
    Context().previewMedia = false;
    Context().previewVideo = false;
    Context().videoWidth = Context().videoHeight = Context().videoFrameRate = 0;
    Context().audioSampleRate = Context().audioBits = Context().audioChannels = 0;
    Context().audioFormat.clear();
    Context().inspectorScrollY = 0;
    Context().inspectorDrag = 0;
    Context().inspectorHover = 0;
    Context().previewIndex = -1;
}

DWORD WINAPI ImageWorker(void* data) {
    CallbackScope callbackScope;
    std::unique_ptr<std::shared_ptr<PreviewJob>> argument(
        static_cast<std::shared_ptr<PreviewJob>*>(data));
    auto job = std::move(*argument);
    HRESULT init = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    HBITMAP bitmap = nullptr;
    if (SUCCEEDED(init)) {
        IShellItem* item = nullptr;
        HRESULT hr = PreviewCanceled(job->cancel) ? E_ABORT :
            SHCreateItemFromParsingName(job->path.c_str(), nullptr,
                IID_IShellItem, reinterpret_cast<void**>(&item));
        if (SUCCEEDED(hr) && item) {
            IShellItemImageFactory* factory = nullptr;
            hr = item->QueryInterface(IID_IShellItemImageFactory,
                reinterpret_cast<void**>(&factory));
            if (SUCCEEDED(hr) && factory) {
                SIZE size = {1200, 1200};
                // GetImage has no cancellation argument. Closing the tab retires
                // this job immediately; unloading still waits for provider exit.
                if (!PreviewCanceled(job->cancel))
                    hr = factory->GetImage(size, static_cast<SIIGBF>(
                        SIIGBF_THUMBNAILONLY | SIIGBF_BIGGERSIZEOK), &bitmap);
                factory->Release();
            }
            item->Release();
        }
        CoUninitialize();
    }
    CompletePreviewJob(job, bitmap, {});
    return 0;
}

bool ReadWaveBytes(HANDLE file, ULONGLONG offset, void* buffer, DWORD count,
                   HANDLE cancel = nullptr) {
    if (PreviewCanceled(cancel)) return false;
    LARGE_INTEGER position = {};
    position.QuadPart = static_cast<LONGLONG>(offset);
    DWORD read = 0;
    return SetFilePointerEx(file, position, nullptr, FILE_BEGIN) &&
           ReadFile(file, buffer, count, &read, nullptr) && read == count &&
           !PreviewCanceled(cancel);
}

WORD WaveWord(const BYTE* bytes) {
    return static_cast<WORD>(bytes[0] | (bytes[1] << 8));
}

DWORD WaveDword(const BYTE* bytes) {
    return static_cast<DWORD>(bytes[0]) |
           (static_cast<DWORD>(bytes[1]) << 8) |
           (static_cast<DWORD>(bytes[2]) << 16) |
           (static_cast<DWORD>(bytes[3]) << 24);
}

float WaveSample(const BYTE* bytes, WORD format, WORD bits) {
    if (format == 3 && bits == 32) {
        float value = 0;
        std::memcpy(&value, bytes, sizeof(value));
        return std::isfinite(value) ? std::clamp(value, -1.0f, 1.0f) : 0;
    }
    if (format == 3 && bits == 64) {
        double value = 0;
        std::memcpy(&value, bytes, sizeof(value));
        return std::isfinite(value) ? static_cast<float>(
            std::clamp(value, -1.0, 1.0)) : 0;
    }
    if (bits == 8) return (static_cast<int>(bytes[0]) - 128) / 128.0f;
    if (bits == 16)
        return static_cast<short>(WaveWord(bytes)) / 32768.0f;
    if (bits == 24) {
        int value = static_cast<int>(bytes[0]) |
                    (static_cast<int>(bytes[1]) << 8) |
                    (static_cast<int>(bytes[2]) << 16);
        if (value & 0x800000) value |= ~0xffffff;
        return value / 8388608.0f;
    }
    int value = static_cast<int>(WaveDword(bytes));
    return static_cast<float>(value / 2147483648.0);
}

void BuildWavePreview(const std::wstring& path, WavePreview* result,
                      HANDLE cancel = nullptr) {
    if (PreviewCanceled(cancel)) return;
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    LARGE_INTEGER length = {};
    BYTE riff[12] = {};
    if (!GetFileSizeEx(file, &length) || length.QuadPart < 44 ||
        !ReadWaveBytes(file, 0, riff, sizeof(riff), cancel) ||
        std::memcmp(riff, "RIFF", 4) || std::memcmp(riff + 8, "WAVE", 4)) {
        CloseHandle(file);
        return;
    }
    ULONGLONG end = std::min<ULONGLONG>(length.QuadPart,
        static_cast<ULONGLONG>(WaveDword(riff + 4)) + 8);
    WORD format = 0, channels = 0, bits = 0, displayBits = 0,
         blockAlign = 0;
    DWORD sampleRate = 0;
    ULONGLONG dataOffset = 0, dataSize = 0;
    for (ULONGLONG offset = 12; offset + 8 <= end;) {
        BYTE chunk[8] = {};
        if (!ReadWaveBytes(file, offset, chunk, sizeof(chunk), cancel)) break;
        ULONGLONG size = WaveDword(chunk + 4);
        ULONGLONG start = offset + 8;
        if (size > end - start) break;
        if (!std::memcmp(chunk, "fmt ", 4) && size >= 16) {
            BYTE info[40] = {};
            DWORD amount = static_cast<DWORD>(std::min<ULONGLONG>(size, 40));
            if (!ReadWaveBytes(file, start, info, amount, cancel)) break;
            format = WaveWord(info);
            channels = WaveWord(info + 2);
            sampleRate = WaveDword(info + 4);
            blockAlign = WaveWord(info + 12);
            bits = WaveWord(info + 14);
            displayBits = bits;
            if (format == 0xfffe && size >= 40 &&
                WaveWord(info + 16) >= 22) {
                WORD validBits = WaveWord(info + 18);
                if (validBits && validBits <= bits) displayBits = validBits;
                format = WaveWord(info + 24); // WAVEFORMATEXTENSIBLE subtype.
            }
        } else if (!std::memcmp(chunk, "data", 4) && !dataSize) {
            dataOffset = start;
            dataSize = size;
        }
        offset = start + size + (size & 1);
        if (format && dataSize) break;
    }
    int bytesPerSample = bits / 8;
    bool supported = (format == 1 &&
        (bits == 8 || bits == 16 || bits == 24 || bits == 32)) ||
        (format == 3 && (bits == 32 || bits == 64));
    if (!supported || !channels || channels > 32 || !sampleRate ||
        !blockAlign || blockAlign < channels * bytesPerSample ||
        !dataSize) {
        CloseHandle(file);
        return;
    }
    result->channels = channels;
    result->sampleRate = sampleRate;
    result->bits = displayBits;
    result->format = format;
    result->frames = dataSize / blockAlign;
    result->supported = result->frames > 0;
    constexpr int bins = 512;
    result->peaks.resize(bins);
    BYTE sampleBytes[8192] = {};
    ULONGLONG maxFrames = sizeof(sampleBytes) / blockAlign;
    for (int i = 0; result->supported && i < bins; ++i) {
        if (PreviewCanceled(cancel)) { result->supported = false; break; }
        ULONGLONG first = result->frames * i / bins;
        ULONGLONG last = result->frames * (i + 1) / bins;
        if (last <= first) last = std::min(result->frames, first + 1);
        ULONGLONG count = std::min(last - first, maxFrames);
        if (!count) continue;
        first += (last - first - count) / 2;
        DWORD byteCount = static_cast<DWORD>(count * blockAlign);
        if (!ReadWaveBytes(file, dataOffset + first * blockAlign,
                           sampleBytes, byteCount, cancel)) {
            result->supported = false;
            break;
        }
        float peak = 0;
        for (ULONGLONG frame = 0; frame < count; ++frame) {
            const BYTE* samples = sampleBytes + frame * blockAlign;
            for (WORD channel = 0; channel < channels; ++channel)
                peak = std::max(peak, std::abs(WaveSample(
                    samples + channel * bytesPerSample, format, bits)));
        }
        result->peaks[i] = std::min(peak, 1.0f);
    }
    CloseHandle(file);
}

HRESULT BuildDecodedWavePreview(const std::wstring& path, WavePreview* result, HANDLE cancel) {
    if (PreviewCanceled(cancel)) return E_ABORT;
    HRESULT hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) return hr;
    IMFSourceReader* reader = nullptr;
    IMFMediaType* native = nullptr;
    IMFMediaType* requested = nullptr;
    IMFMediaType* decoded = nullptr;
    PROPVARIANT duration = {};
    UINT32 rate = 0, channels = 0, bits = 0;
    GUID subtype = {};
    ULONGLONG units = 0, frames = 0;
    bool ended = false;
    ULONGLONG started = GetTickCount64();
    do {
        hr = MFCreateSourceReaderFromURL(path.c_str(), nullptr, &reader);
        if (FAILED(hr)) break;
        hr = reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE);
        if (SUCCEEDED(hr)) hr = reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
        if (FAILED(hr)) break;
        if (SUCCEEDED(reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &native))) {
            native->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bits);
            native->GetGUID(MF_MT_SUBTYPE, &subtype);
        }
        if (IsEqualGUID(subtype, MFAudioFormat_FLAC)) result->codec = L"FLAC";
        else if (IsEqualGUID(subtype, MFAudioFormat_ALAC)) result->codec = L"ALAC";
        else if (IsEqualGUID(subtype, MFAudioFormat_AAC)) { result->codec = L"AAC"; bits = 0; }
        else if (IsEqualGUID(subtype, MFAudioFormat_MP3)) { result->codec = L"MP3"; bits = 0; }
        else if (IsEqualGUID(subtype, MFAudioFormat_Opus)) { result->codec = L"Opus"; bits = 0; }
        else if (!IsEqualGUID(subtype, MFAudioFormat_PCM) && !IsEqualGUID(subtype, MFAudioFormat_Float)) bits = 0;
        hr = reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &duration);
        if (FAILED(hr) || duration.vt != VT_UI8 || !duration.uhVal.QuadPart) { hr = E_FAIL; break; }
        units = duration.uhVal.QuadPart;
        hr = MFCreateMediaType(&requested);
        if (SUCCEEDED(hr)) hr = requested->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
        if (SUCCEEDED(hr)) hr = requested->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
        if (SUCCEEDED(hr)) hr = reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, requested);
        if (SUCCEEDED(hr)) hr = reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &decoded);
        if (SUCCEEDED(hr)) hr = decoded->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &rate);
        if (SUCCEEDED(hr)) hr = decoded->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &channels);
        if (FAILED(hr) || !rate || !channels || channels > 32) { hr = E_FAIL; break; }
        result->sampleRate = rate;
        result->channels = static_cast<WORD>(channels);
        result->bits = static_cast<WORD>(bits); // Original precision, never the float decoder's 32 bits.
        constexpr int bins = 512;
        result->peaks.assign(bins, 0.0f);
        while (!ended) {
            if (PreviewCanceled(cancel) ||
                GetTickCount64() - started > 15000) { hr = E_ABORT; break; }
            DWORD flags = 0;
            LONGLONG timestamp = 0;
            IMFSample* sample = nullptr;
            hr = reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0,
                nullptr, &flags, &timestamp, &sample);
            if (PreviewCanceled(cancel)) {
                if (sample) sample->Release();
                hr = E_ABORT;
                break;
            }
            ended = (flags & MF_SOURCE_READERF_ENDOFSTREAM) != 0;
            if (FAILED(hr) || (flags & (MF_SOURCE_READERF_ERROR | MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED))) {
                if (sample) sample->Release();
                if (SUCCEEDED(hr)) hr = E_FAIL;
                break;
            }
            if (!sample) continue;
            IMFMediaBuffer* buffer = nullptr;
            BYTE* bytes = nullptr;
            DWORD length = 0;
            hr = sample->ConvertToContiguousBuffer(&buffer);
            if (SUCCEEDED(hr)) hr = buffer->Lock(&bytes, nullptr, &length);
            if (SUCCEEDED(hr)) {
                DWORD count = length / (channels * sizeof(float));
                for (DWORD frame = 0; frame < count; ++frame) {
                    if ((frame & 4095) == 0 && PreviewCanceled(cancel)) {
                        hr = E_ABORT;
                        break;
                    }
                    double time = std::max(0.0, static_cast<double>(timestamp)) +
                                  frame * 10000000.0 / rate;
                    int bin = std::clamp(static_cast<int>(time * bins / units), 0, bins - 1);
                    float peak = 0;
                    for (UINT32 channel = 0; channel < channels; ++channel) {
                        float value = 0;
                        std::memcpy(&value, bytes + (frame * channels + channel) * sizeof(float), sizeof(float));
                        if (std::isfinite(value)) peak = std::max(peak, std::abs(value));
                    }
                    result->peaks[bin] = std::max(result->peaks[bin], std::min(1.0f, peak));
                }
                frames += count;
                buffer->Unlock();
            }
            if (buffer) buffer->Release();
            sample->Release();
            if (FAILED(hr)) break;
        }
        result->frames = static_cast<ULONGLONG>(units * static_cast<double>(rate) / 10000000.0);
        result->supported = SUCCEEDED(hr) && ended && frames > 0;
    } while (false);
    PropVariantClear(&duration);
    if (decoded) decoded->Release();
    if (requested) requested->Release();
    if (native) native->Release();
    if (reader) reader->Release();
    MFShutdown();
    return hr;
}

DWORD WINAPI WaveWorker(void* data) {
    CallbackScope callbackScope;
    std::unique_ptr<std::shared_ptr<PreviewJob>> argument(
        static_cast<std::shared_ptr<PreviewJob>*>(data));
    auto job = std::move(*argument);
    std::unique_ptr<WavePreview> result(new (std::nothrow) WavePreview);
    if (result) {
        result->generation = job->generation;
        if (!_wcsicmp(PathFindExtensionW(job->path.c_str()), L".wav"))
            BuildWavePreview(job->path, result.get(), job->cancel);
        if (!result->supported && !PreviewCanceled(job->cancel)) {
            HRESULT init = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
            if (SUCCEEDED(init)) {
                HRESULT hr = BuildDecodedWavePreview(job->path, result.get(), job->cancel);
                Wh_Log(L"[Columns] Decoded waveform: %08X supported=%d", hr, result->supported ? 1 : 0);
                CoUninitialize();
            }
        }
    }
    CompletePreviewJob(job, nullptr, std::move(result));
    return 0;
}

void LaunchWaveWorker() {
    if (!Context().waveWanted || !Context().previewWindow || !Context().overlay || Context().waveJob) return;
    Context().waveJob = StartPreviewJob(Context().previewPath, Context().overlay,
        Context().previewGeneration, kWaveReadyMessage, WaveWorker);
}

void LaunchImageWorker() {
    if (!Context().imageWanted || !Context().previewWindow || !Context().overlay) return;
    // Keep at most one provider call per kind and tab. A new selection cancels
    // the old work, whose completion starts the latest requested preview.
    if (Context().imageJob) return;
    Context().imageJob = StartPreviewJob(Context().previewPath, Context().overlay,
        Context().previewGeneration, kImageReadyMessage, ImageWorker);
    if (!Context().imageJob) {
        Context().previewMessage = L"Image preview could not start.";
    }
}

bool StartImageFallback(const std::wstring& path) {
    const wchar_t* extension = PathFindExtensionW(path.c_str());
    PERCEIVED type = PERCEIVED_TYPE_UNSPECIFIED;
    PERCEIVEDFLAG flags = PERCEIVEDFLAG_UNDEFINED;
    if (!extension || FAILED(AssocGetPerceivedType(extension, &type,
            &flags, nullptr)) || type != PERCEIVED_TYPE_IMAGE)
        return false;
    Context().previewMessage = L"Loading image preview...";
    Context().imageWanted = true;
    LaunchImageWorker();
    return true;
}

HRESULT StartDirectShowPreview(const std::wstring& path) {
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER,
        IID_IGraphBuilder, reinterpret_cast<void**>(&Context().audioGraph));
    if (SUCCEEDED(hr)) hr = Context().audioGraph->RenderFile(path.c_str(), nullptr);
    if (SUCCEEDED(hr)) hr = Context().audioGraph->QueryInterface(IID_IMediaControl,
        reinterpret_cast<void**>(&Context().audioControl));
    if (SUCCEEDED(hr)) hr = Context().audioGraph->QueryInterface(IID_IMediaSeeking,
        reinterpret_cast<void**>(&Context().audioSeeking));
    if (SUCCEEDED(hr)) hr = Context().audioSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
    if (SUCCEEDED(hr)) hr = Context().audioSeeking->GetDuration(&Context().audioDuration);
    if (SUCCEEDED(hr) && Context().previewVideo) {
        if (!CreateVideoWindow()) hr = HRESULT_FROM_WIN32(GetLastError());
        if (SUCCEEDED(hr)) hr = Context().audioGraph->QueryInterface(IID_IVideoWindow,
            reinterpret_cast<void**>(&Context().directShowVideo));
        if (SUCCEEDED(hr)) hr = Context().directShowVideo->put_AutoShow(OAFALSE);
        if (SUCCEEDED(hr)) hr = Context().directShowVideo->put_Owner(reinterpret_cast<OAHWND>(Context().videoWindow));
        if (SUCCEEDED(hr)) hr = Context().directShowVideo->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
        if (SUCCEEDED(hr)) hr = Context().directShowVideo->put_MessageDrain(reinterpret_cast<OAHWND>(Context().previewWindow));
        if (SUCCEEDED(hr)) hr = Context().directShowVideo->put_Visible(OATRUE);
        IBasicVideo* video = nullptr;
        if (SUCCEEDED(hr) && SUCCEEDED(Context().audioGraph->QueryInterface(IID_IBasicVideo,
                reinterpret_cast<void**>(&video)))) {
            long width = 0, height = 0;
            if (SUCCEEDED(video->GetVideoSize(&width, &height))) {
                Context().videoWidth = std::max(0L, width); Context().videoHeight = std::max(0L, height);
            }
            video->Release();
        }
    }
    if (FAILED(hr)) { StopAudioPreview(); return hr; }
    Context().audioGraph->QueryInterface(IID_IBasicAudio, reinterpret_cast<void**>(&Context().audioVolumeControl));
    ApplyAudioVolume();
    Context().audioPlaying = false;
    Context().previewMessage.clear();
    if (Context().previewVideo) {
        PositionVideoPreview();
        Context().audioControl->Pause(); // Display the first frame without starting the audio.
    }
    Wh_Log(L"[Columns] DirectShow fallback ready: %s", path.c_str());
    return S_OK;
}

HRESULT StartMediaEngine(const std::wstring& path) {
    HRESULT hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) return hr;
    Context().mediaFoundationStarted = true;
    IMFMediaEngineClassFactory* factory = nullptr;
    IMFAttributes* attributes = nullptr;
    MediaEngineNotify* callback = new (std::nothrow) MediaEngineNotify(Context().previewWindow, Context().previewGeneration);
    if (!callback) hr = E_OUTOFMEMORY;
    if (SUCCEEDED(hr) && !CreateVideoWindow()) hr = HRESULT_FROM_WIN32(GetLastError());
    if (SUCCEEDED(hr)) hr = MFCreateAttributes(&attributes, 3);
    if (SUCCEEDED(hr)) hr = attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, callback);
    if (SUCCEEDED(hr)) hr = attributes->SetUINT64(MF_MEDIA_ENGINE_PLAYBACK_HWND,
        reinterpret_cast<UINT64>(Context().videoWindow));
    if (SUCCEEDED(hr)) hr = CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr,
        CLSCTX_INPROC_SERVER, IID_IMFMediaEngineClassFactory, reinterpret_cast<void**>(&factory));
    if (SUCCEEDED(hr)) hr = factory->CreateInstance(0, attributes, &Context().mediaEngine);
    if (SUCCEEDED(hr)) hr = Context().mediaEngine->QueryInterface(IID_IMFMediaEngineEx,
        reinterpret_cast<void**>(&Context().mediaEngineEx));
    if (SUCCEEDED(hr)) hr = Context().mediaEngine->SetAutoPlay(FALSE);
    if (SUCCEEDED(hr)) hr = Context().mediaEngine->SetVolume(Context().audioVolume / 100.0);
    if (SUCCEEDED(hr)) hr = Context().mediaEngine->SetPreload(MF_MEDIA_ENGINE_PRELOAD_AUTOMATIC);
    if (SUCCEEDED(hr)) {
        std::wstring url(MAX_PATH, L'\0');
        DWORD length = static_cast<DWORD>(url.size());
        hr = UrlCreateFromPathW(path.c_str(), url.data(), &length, 0);
        if (hr == E_POINTER && length > url.size()) {
            url.resize(length);
            hr = UrlCreateFromPathW(path.c_str(), url.data(), &length, 0);
        }
        if (SUCCEEDED(hr)) {
            BSTR source = SysAllocString(url.c_str());
            hr = source ? Context().mediaEngine->SetSource(source) : E_OUTOFMEMORY;
            SysFreeString(source);
        }
    }
    if (attributes) attributes->Release();
    if (factory) factory->Release();
    Context().mediaNotify = callback; // Retain a detachable callback through Shutdown.
    if (FAILED(hr)) { StopAudioPreview(); return hr; }
    Context().previewMessage = L"Loading media…";
    Wh_Log(L"[Columns] Media Engine loading: %s", path.c_str());
    return S_OK;
}

void HandleMediaEvent(DWORD event, HRESULT error) {
    if (!Context().mediaEngine) return;
    bool updateVideo = event == MF_MEDIA_ENGINE_EVENT_ERROR ||
        event == MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA || event == MF_MEDIA_ENGINE_EVENT_LOADEDDATA ||
        event == MF_MEDIA_ENGINE_EVENT_CANPLAY || event == MF_MEDIA_ENGINE_EVENT_FORMATCHANGE ||
        event == MF_MEDIA_ENGINE_EVENT_FIRSTFRAMEREADY || event == MF_MEDIA_ENGINE_EVENT_SEEKED;
    if (event == MF_MEDIA_ENGINE_EVENT_ERROR) {
        Wh_Log(L"[Columns] Media Engine failed: %08X for %s; trying DirectShow", error, Context().previewPath.c_str());
        StopAudioPreview();
        HRESULT hr = StartDirectShowPreview(Context().previewPath);
        if (FAILED(hr)) {
            Context().previewMessage = L"Windows could not decode this file. Use Open with to play it in another app.";
            Wh_Log(L"[Columns] DirectShow fallback also failed: %08X", hr);
        }
    } else {
        if (event == MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA ||
            event == MF_MEDIA_ENGINE_EVENT_DURATIONCHANGE || event == MF_MEDIA_ENGINE_EVENT_CANPLAY) {
            double duration = Context().mediaEngine->GetDuration();
            if (std::isfinite(duration) && duration > 0)
                Context().audioDuration = static_cast<LONGLONG>(duration * 10000000.0);
            if (Context().mediaEngine->HasVideo()) {
                Context().previewVideo = true;
                Context().mediaEngine->GetNativeVideoSize(&Context().videoWidth, &Context().videoHeight);
            }
        }
        if (event == MF_MEDIA_ENGINE_EVENT_CANPLAY || event == MF_MEDIA_ENGINE_EVENT_CANPLAYTHROUGH) {
            if (!Context().mediaReady) Wh_Log(L"[Columns] Media Engine ready: duration=%lld video=%d size=%ux%u",
                Context().audioDuration, Context().previewVideo ? 1 : 0, Context().videoWidth, Context().videoHeight);
            Context().mediaReady = true;
            Context().previewMessage.clear();
        }
        if (event == MF_MEDIA_ENGINE_EVENT_PLAY || event == MF_MEDIA_ENGINE_EVENT_PLAYING ||
            event == MF_MEDIA_ENGINE_EVENT_PAUSE) {
            // Queued events can outlive a later toggle; use the engine's current state.
            if (IsWindowVisible(Context().previewWindow))
                Context().audioPlaying = !Context().mediaEngine->IsPaused();
            else PauseMediaPreview();
        }
        if (event == MF_MEDIA_ENGINE_EVENT_ENDED) { PauseMediaPreview(); SeekAudio(0); }
        UpdateAudioPlaybackTimer();
    }
    if (updateVideo) {
        UpdateInspectorScroll();
        UpdateVideoFrame();
        if (IsWindow(Context().videoWindow)) InvalidateRect(Context().videoWindow, nullptr, FALSE);
    }
    InvalidateRect(Context().previewWindow, nullptr, FALSE);
}

bool StartMediaPreview(const std::wstring& path) {
    const wchar_t* extension = PathFindExtensionW(path.c_str());
    if (!extension || !*extension) return false;
    PERCEIVED type = PERCEIVED_TYPE_UNSPECIFIED;
    PERCEIVEDFLAG flags = PERCEIVEDFLAG_UNDEFINED;
    AssocGetPerceivedType(extension, &type, &flags, nullptr);
    bool audio = type == PERCEIVED_TYPE_AUDIO;
    bool video = type == PERCEIVED_TYPE_VIDEO;
    const wchar_t* audioTypes[] = {L".mp3", L".wav", L".wma", L".m4a", L".m4b", L".aac",
        L".flac", L".ogg", L".oga", L".opus", L".aif", L".aiff", L".aifc", L".alac", L".ape", L".caf", L".ac3", L".mka", L".au", L".snd"};
    const wchar_t* videoTypes[] = {L".mp4", L".m4v", L".mov", L".mkv", L".avi", L".wmv",
        L".webm", L".mpg", L".mpeg", L".mts", L".m2ts", L".ts", L".3gp", L".3g2", L".asf"};
    for (const wchar_t* known : audioTypes) if (!_wcsicmp(extension, known)) audio = true;
    for (const wchar_t* known : videoTypes) if (!_wcsicmp(extension, known)) video = true;
    if (!audio && !video) return false;
    Context().previewMedia = true;
    Context().previewVideo = video;
    IShellItem2* metadata = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
            IID_IShellItem2, reinterpret_cast<void**>(&metadata)))) {
        metadata->GetUInt32(PKEY_Audio_SampleRate, &Context().audioSampleRate);
        metadata->GetUInt32(PKEY_Audio_SampleSize, &Context().audioBits);
        metadata->GetUInt32(PKEY_Audio_ChannelCount, &Context().audioChannels);
        metadata->GetUInt32(PKEY_Video_FrameWidth, &Context().videoWidth);
        metadata->GetUInt32(PKEY_Video_FrameHeight, &Context().videoHeight);
        metadata->GetUInt32(PKEY_Video_FrameRate, &Context().videoFrameRate);
        ULONGLONG duration = 0;
        if (SUCCEEDED(metadata->GetUInt64(PKEY_Media_Duration, &duration)))
            Context().audioDuration = static_cast<LONGLONG>(duration);
        PWSTR format = nullptr;
        if (SUCCEEDED(metadata->GetString(PKEY_Audio_Format, &format)) && format) Context().audioFormat = format;
        CoTaskMemFree(format);
        metadata->Release();
    }
    if (!Context().inlineMediaPlayback) {
        Context().previewMessage = L"Inline audio and video are disabled. Use Open with to play this file.";
        return true;
    }
    if (!video) { Context().waveWanted = true; LaunchWaveWorker(); }
    HRESULT hr = StartMediaEngine(path);
    if (FAILED(hr)) {
        Wh_Log(L"[Columns] Media Engine setup failed: %08X; trying DirectShow", hr);
        hr = StartDirectShowPreview(path);
        if (FAILED(hr)) {
            Context().previewMessage = L"Windows could not decode this file. Use Open with to play it in another app.";
            Wh_Log(L"[Columns] DirectShow fallback failed: %08X", hr);
        }
    }
    return true;
}

HRESULT OpenPreviewStream(const std::wstring& path, IStream** stream) {
    if (!stream) return E_POINTER;
    *stream = nullptr;
    HRESULT hr = SHCreateStreamOnFileEx(path.c_str(),
        STGM_READ | STGM_SHARE_DENY_NONE, FILE_ATTRIBUTE_NORMAL,
        FALSE, nullptr, stream);
    if (SUCCEEDED(hr)) return hr;
    IShellItem* item = nullptr;
    hr = SHCreateItemFromParsingName(path.c_str(), nullptr,
        IID_IShellItem, reinterpret_cast<void**>(&item));
    if (SUCCEEDED(hr)) {
        hr = item->BindToHandler(nullptr, BHID_Stream, IID_IStream,
                                 reinterpret_cast<void**>(stream));
        item->Release();
    }
    return hr;
}

void StartNativePreview(const std::wstring& path) {
    if (Context().previewIsFolder) {
        Context().previewMessage = L"Select a file to see its preview here.";
        return;
    }
    const wchar_t* extension = PathFindExtensionW(path.c_str());
    if (!extension || !*extension) {
        Context().previewMessage = L"No preview is available for this file.";
        return;
    }
    // The built-in player supplies consistent controls for audio and video,
    // including file types that have no registered Explorer preview handler.
    // Media decoders need an on-disk file; archive members use Shell previews.
    if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES &&
        StartMediaPreview(path)) return;
    constexpr wchar_t previewKey[] =
        L"{8895B1C6-B41F-4C1C-A562-0D564250836F}";
    wchar_t clsidText[80] = {};
    DWORD length = ARRAYSIZE(clsidText);
    HRESULT hr = AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_SHELLEXTENSION,
                                   extension, previewKey, clsidText,
                                   &length);
    CLSID clsid = {};
    if (FAILED(hr) || FAILED(CLSIDFromString(clsidText, &clsid))) {
        if (!StartImageFallback(path))
            Context().previewMessage = L"No Windows preview handler is installed for this file type.";
        Wh_Log(L"[Columns] No preview handler for %s", extension);
        return;
    }
    // Request the registered out-of-process host. Never load an arbitrary
    // third-party preview DLL directly into Explorer.
    hr = CoCreateInstance(clsid, nullptr, CLSCTX_LOCAL_SERVER,
        IID_IPreviewHandler, reinterpret_cast<void**>(&Context().previewHandler));
    if (FAILED(hr) || !Context().previewHandler) {
        if (!StartImageFallback(path))
            Context().previewMessage = L"The Windows preview handler could not start.";
        Wh_Log(L"[Columns] Preview activation failed: %08X", hr);
        return;
    }
    bool initialized = false;
    IInitializeWithStream* withStream = nullptr;
    if (SUCCEEDED(Context().previewHandler->QueryInterface(IID_IInitializeWithStream,
            reinterpret_cast<void**>(&withStream))) && withStream) {
        IStream* stream = nullptr;
        if (SUCCEEDED(OpenPreviewStream(path, &stream)) && stream) {
            initialized = SUCCEEDED(withStream->Initialize(stream,
                STGM_READ | STGM_SHARE_DENY_NONE));
            stream->Release();
        }
        withStream->Release();
    }
    if (!initialized) {
        IInitializeWithFile* withFile = nullptr;
        if (SUCCEEDED(Context().previewHandler->QueryInterface(IID_IInitializeWithFile,
                reinterpret_cast<void**>(&withFile))) && withFile) {
            initialized = SUCCEEDED(withFile->Initialize(path.c_str(), STGM_READ));
            withFile->Release();
        }
    }
    if (!initialized) {
        IInitializeWithItem* withItem = nullptr;
        if (SUCCEEDED(Context().previewHandler->QueryInterface(IID_IInitializeWithItem,
                reinterpret_cast<void**>(&withItem))) && withItem) {
            IShellItem* item = nullptr;
            if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                    IID_IShellItem, reinterpret_cast<void**>(&item))) && item) {
                initialized = SUCCEEDED(withItem->Initialize(item, STGM_READ));
                item->Release();
            }
            withItem->Release();
        }
    }
    if (!initialized) {
        Context().previewHandler->Release();
        Context().previewHandler = nullptr;
        if (!StartImageFallback(path))
            Context().previewMessage = L"The preview handler could not open this file.";
        return;
    }
    RECT area = {};
    GetClientRect(Context().previewWindow, &area);
    area = GetInspectorLayout(area).preview;
    hr = Context().previewHandler->SetWindow(Context().previewWindow, &area);
    if (SUCCEEDED(hr)) hr = Context().previewHandler->DoPreview();
    if (FAILED(hr)) {
        Wh_Log(L"[Columns] DoPreview failed: %08X", hr);
        Context().previewHandler->Unload();
        Context().previewHandler->Release();
        Context().previewHandler = nullptr;
        if (!StartImageFallback(path))
            Context().previewMessage = L"The preview handler could not display this file.";
    } else {
        Wh_Log(L"[Columns] Preview displayed: %s", path.c_str());
    }
}

bool ShowPreview(const std::wstring& path) {
    if (!Context().overlay) return false;
    ClearPreview();
    Context().previewGeneration = g_previewSequence.fetch_add(1) + 1;
    WIN32_FILE_ATTRIBUTE_DATA attributes = {};
    bool haveAttributes = GetFileAttributesExW(path.c_str(),
        GetFileExInfoStandard, &attributes) != FALSE;
    bool isFolder = (haveAttributes &&
        (attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
        IsShellFolderPath(path);
    if (isFolder && !Context().fixedInspector) {
        ScrollToNewestColumn();
        return true;
    }
    int index = Context().columnCount;
    Context().previewWindow = CreateWindowExW(0, kPreviewClass, L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL,
        0, 0, 100, 100, Context().overlay, nullptr, HINST_THISCOMPONENT,
        &Context());
    if (!Context().previewWindow) {
        LogWin32(L"CreateWindowEx(preview)");
        return false;
    }
    Context().previewIndex = index;
    Context().previewPath = path;
    Context().previewDisplayName = PathFindFileNameW(path.c_str());
    Context().previewMessage.clear();
    Context().previewIsFolder = isFolder;
    Context().previewIsVirtualFolder = isFolder && !haveAttributes;
    if (haveAttributes) {
        Context().previewFileSize =
            (static_cast<ULONGLONG>(attributes.nFileSizeHigh) << 32) |
            attributes.nFileSizeLow;
        Context().previewModified = attributes.ftLastWriteTime;
        Context().previewCreated = attributes.ftCreationTime;
    }
    IShellItem2* metadata = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
            IID_IShellItem2, reinterpret_cast<void**>(&metadata))) && metadata) {
        PWSTR displayName = nullptr;
        if (SUCCEEDED(metadata->GetDisplayName(SIGDN_NORMALDISPLAY, &displayName)) &&
                displayName && *displayName)
            Context().previewDisplayName = displayName;
        CoTaskMemFree(displayName);
        SFGAOF shellAttributes = 0;
        if (SUCCEEDED(metadata->GetAttributes(SFGAO_FILESYSTEM, &shellAttributes)))
            Context().previewIsVirtualFolder = isFolder && !(shellAttributes & SFGAO_FILESYSTEM);
        if (!haveAttributes) {
            metadata->GetUInt64(PKEY_Size, &Context().previewFileSize);
            metadata->GetFileTime(PKEY_DateModified, &Context().previewModified);
            metadata->GetFileTime(PKEY_DateCreated, &Context().previewCreated);
        }
    }
    SHFILEINFOW shellInfo = {};
    DWORD_PTR haveShellInfo = 0;
    if (Context().previewIsVirtualFolder) {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (metadata && SUCCEEDED(SHGetIDListFromObject(metadata, &pidl)) && pidl)
            haveShellInfo = SHGetFileInfoW(reinterpret_cast<PCWSTR>(pidl), 0,
                &shellInfo, sizeof(shellInfo), SHGFI_PIDL | SHGFI_TYPENAME | SHGFI_ICON | SHGFI_LARGEICON);
        CoTaskMemFree(pidl);
        // Parsing names can be GUIDs; a failed display-name query must not expose them as a title.
        if (Context().previewDisplayName.empty() || Context().previewDisplayName == path ||
                Context().previewDisplayName.find(L"::{") != std::wstring::npos)
            Context().previewDisplayName = L"Folder";
    } else {
        haveShellInfo = SHGetFileInfoW(path.c_str(), Context().previewIsFolder ? FILE_ATTRIBUTE_DIRECTORY :
            FILE_ATTRIBUTE_NORMAL, &shellInfo, sizeof(shellInfo),
            SHGFI_TYPENAME | SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);
    }
    if (metadata) metadata->Release();
    if (haveShellInfo) {
        Context().previewType = shellInfo.szTypeName;
        Context().previewIcon = shellInfo.hIcon;
    }
    if (Context().previewType.empty())
        Context().previewType = Context().previewIsFolder ? L"Folder" : L"File";
    ScrollToNewestColumn();
    ReadInspectorColors();
    ReadOpenWithDefault();
    StartNativePreview(path);
    UpdateInspectorScroll();
    InvalidateRect(Context().previewWindow, nullptr, FALSE);
    return true;
}

void RemoveColumnsAfter(int index);
bool AppendColumn(const std::wstring& path, PCIDLIST_ABSOLUTE identity = nullptr);
bool CreatePane(HWND overlay, int index, const std::wstring& path, PCIDLIST_ABSOLUTE identity,
    ULONG_PTR* startedSerial = nullptr);
void TestPaintOnly(const wchar_t* reason);

void ProcessSelectionUpdates(bool recovery) {
    if (Context().suspended || Context().closed) return;
    if (NativePointerInteractionActive()) {
        if (recovery)
            for (int i = 0; i < Context().columnCount; ++i)
                Context().panes[i].selectionDirty = true;
        SchedulePointerRelease(true);
        return;
    }
    bool queryFailed = false;
    for (int i = 0; i < Context().columnCount; ++i) {
        Pane& pane = Context().panes[i];
        if (pane.awaitingNavigationRepair || pane.navigationTarget) break;
        if (!recovery && !pane.selectionDirty) continue;
        ULONG_PTR generation = pane.generation;
        SelectedItem selected = SelectedPath(pane);
        // COM providers can pump input while answering. Never apply a snapshot
        // after a new gesture starts or the originating pane has been replaced.
        if (Context().closed || i >= Context().columnCount ||
            Context().panes[i].generation != generation) return;
        if (NativePointerInteractionActive()) {
            pane.selectionDirty = true;
            SchedulePointerRelease(true);
            return;
        }
        if (!selected.querySucceeded) {
            pane.selectionDirty = true; // Query failure never removes descendants.
            queryFailed = true;
            break;
        }
        pane.selectionDirty = false;
        if (selected.path == Context().columnSelection[i]) continue;
        Context().columnSelection[i] = selected.path;
        RemoveColumnsAfter(i);
        if (!selected.path.empty()) {
            if (selected.folder) {
                Wh_Log(L"[Columns] Extending from %d: %s", i, selected.path.c_str());
                AppendColumn(selected.path, selected.identity.get());
            } else Wh_Log(L"[Columns] Preview from %d: %s", i, selected.path.c_str());
            ShowPreview(selected.path);
        }
        break; // One update can destroy every later column and create a new one.
    }
    if (queryFailed && Context().selectionRetryAttempts < kSelectionRetryLimit) {
        ++Context().selectionRetryAttempts;
        SetTimer(Context().overlay, kSelectionRetryTimerId, kSelectionRetryDelayMs, nullptr);
    } else {
        KillTimer(Context().overlay, kSelectionRetryTimerId);
        if (!queryFailed) Context().selectionRetryAttempts = 0;
    }

}

void HideColumnsForNativeView(HWND hwnd, NativeViewReason reason = NativeViewReason::SafetyCheck) {
    if (reason == NativeViewReason::SafetyCheck && Context().nativeSelectionYielded)
        reason = NativeViewReason::Selection;
    CancelPendingFocus();
    CancelColumnResize();
    KillTimer(hwnd, kSelectionRetryTimerId);
    Context().selectionRetryAttempts = 0;
    KillTimer(hwnd, kVisualRepairTimerId);
    KillTimer(hwnd, kSettledPaintTimerId);
    if (!Context().suspended && reason != NativeViewReason::Selection) ClearPreview();
    Context().suspended = true;
    Context().nativeViewReason = reason;
    ShowWindow(hwnd, SW_HIDE);
    RestoreOriginalView();
}

void RetryNativeViewTransition() {
    // A transient active-view lookup must not consume the last navigation event.
    // Stop after a short recovery window; genuine selections wait for their own
    // deselection event and never enter this retry path.
    if (!Context().nativeSelectionYielded && ++Context().nativeViewRetryAttempts <= 10)
        SetTimer(Context().overlay, kStateRetryTimerId, 100, nullptr);
}

void NativeRootNavigationStarting() {
    RetireNativeRestorationPermit();
    Context().nativeRestorationBlockedSerial = 0;
    ++Context().nativeNavigationGeneration;
    Context().nativeNavigationInProgress = true;
    // A column completion for the preceding root must not cover a view that
    // is being replaced, including a navigation back to the same folder.
    Context().rootNavigationPending = false;
    Context().pointerRootCompletionSerial = 0;
    HideColumnsForNativeView(Context().overlay, NativeViewReason::Navigation);
}

void NativeSelectionChanged(DISPID event) {
    ++Context().nativeSelectionChangeSerial;
    if (Context().nativeSelectionChecking) {
        if (!Context().nativeSelectionClearing) Context().nativeSelectionYielded = true;
        if (Context().nativeSelectionYielded) QueueNativeNavigationUpdate();
        return;
    }
    if (Context().closed) return;
    IShellBrowser* browser = Context().nativeBrowser;
    ULONG_PTR connection = Context().nativeConnectionGeneration;
    ULONG_PTR navigation = Context().nativeNavigationGeneration;
    ULONG_PTR observer = Context().nativeSelectionGeneration;
    ULONG_PTR serial = Context().nativeSelectionChangeSerial;
    IShellView* view = nullptr;
    IUnknown* identity = nullptr;
    HWND window = nullptr;
    bool replaced = false;
    int selectionCount = -1;
    HRESULT hr = E_ABORT;
    if (browser && Context().nativeBrowserThread == GetCurrentThreadId()) {
        browser->AddRef();
        hr = browser->QueryActiveShellView(&view);
        if (SUCCEEDED(hr) && view) hr = view->QueryInterface(IID_IUnknown,
            reinterpret_cast<void**>(&identity));
        if (SUCCEEDED(hr) && (!view || !identity)) hr = E_NOINTERFACE;
        if (SUCCEEDED(hr)) hr = NativeViewWindow(view, &window);
        if (SUCCEEDED(hr)) replaced = identity != Context().nativeSelectionIdentity ||
            window != Context().nativeSelectionWindow;
        if (SUCCEEDED(hr) && !replaced) hr = NativeViewSelectionIsEmpty(view, window, &selectionCount);
        if (identity) identity->Release();
        if (view) view->Release();
        browser->Release();
    }
    // These queries/releases can pump a newer navigation or selection event.
    // Its state wins; an older observer must not protect or resume that view.
    if (Context().closed || Context().nativeBrowser != browser ||
            Context().nativeConnectionGeneration != connection ||
            Context().nativeNavigationGeneration != navigation ||
            Context().nativeSelectionGeneration != observer ||
            Context().nativeSelectionChangeSerial != serial) return;
    if (replaced) {
        if (Context().nativeRestorationViewIdentity) {
            Context().nativeRestorationBlockedSerial = Context().nativeRestorationRootSerial;
            RetireNativeRestorationPermit();
        }
        HideColumnsForNativeView(Context().overlay, NativeViewReason::Navigation);
        Context().stateRetryAttempts = 0;
        QueueNativeNavigationUpdate();
        Wh_Log(L"[Columns] Stale native selection observer; active view changed, root pending=%d",
            Context().rootNavigationPending);
        return;
    }
    if (SUCCEEDED(hr)) {
        if (Context().nativeSelectionYielded) {
            Context().stateRetryAttempts = 0;
            QueueNativeNavigationUpdate();
        }
        return;
    }
    if (hr != E_ABORT || selectionCount != 1) {
        if (Context().nativeRestorationExpected)
            Context().nativeRestorationBlockedSerial = Context().nativeRestorationRootSerial;
        RetireNativeRestorationPermit();
    }
    Wh_Log(L"[Columns] Native selection event=%d count=%d restoration pending=%d",
        event, selectionCount, Context().nativeRestorationExpected != nullptr);
    // Enumeration/history restoration can select an item after the root
    // navigation event. Reveal it synchronously; never leave stale selection
    // hidden until the queued root check gets a chance to run.
    Context().nativeSelectionYielded = true;
    HideColumnsForNativeView(Context().overlay, NativeViewReason::Selection);
    Context().stateRetryAttempts = 0;
    QueueNativeNavigationUpdate();
    LogHr(L"Native selection changed; native view restored", hr);
}

void SuspendColumnsForNativeLocation(HWND hwnd) {
    RetireNativeRestorationPermit();
    bool changed = !Context().suspended || !Context().tabPath.empty();
    Context().rootNavigationPending = false;
    Context().rootNavigationRetry = false;
    Context().tabPath.clear();
    HideColumnsForNativeView(hwnd, NativeViewReason::UnsupportedLocation);
    if (changed) Wh_Log(L"[Columns] Confirmed unsupported native location; columns suspended");
}

void SuspendColumnsForRootFailure(HWND hwnd) {
    RetireNativeRestorationPermit();
    Context().rootNavigationPending = false;
    Context().rootNavigationRetry = true;
    HideColumnsForNativeView(hwnd, NativeViewReason::RootFailure);
    Wh_Log(L"[Columns] Root column navigation failed; native view restored, retry pending");
}

void QueueOverlayBoundsUpdate(Session& session) {
    if (!session.closed && !session.boundsUpdatePosted && IsWindow(session.overlay))
        session.boundsUpdatePosted = PostMessageW(session.overlay,
            kOverlayBoundsMessage, session.identity, 0) != FALSE;
}

LRESULT CALLBACK NativeBoundsProc(HWND window, UINT message, WPARAM wp,
    LPARAM lp, UINT_PTR subclass, DWORD_PTR reference) {
    CallbackScope callbackScope;
    auto* session = reinterpret_cast<Session*>(reference);
    // Cleanup removes every subclass before the registry releases its owner.
    // Retain it through native processing, which can run a nested message loop.
    auto owner = session->shared_from_this();
    if (!g_unloading.load() && !owner->closed && !owner->suspended &&
        owner->nativeSelectionGuarded && window == owner->originalDrawingSurface) {
        if (message == WM_WINDOWPOSCHANGING && lp) {
            auto* position = reinterpret_cast<WINDOWPOS*>(lp);
            if (position->flags & SWP_SHOWWINDOW) {
                owner->originalWasVisible = true;
                position->flags = (position->flags & ~SWP_SHOWWINDOW) | SWP_HIDEWINDOW;
            }
        } else if (message == WM_SHOWWINDOW && wp) {
            // ShowWindow still sends WINDOWPOSCHANGING, where the show is
            // converted into a hide before the native surface becomes visible.
            owner->originalWasVisible = true;
        }
    }
    if (message == WM_NCDESTROY) {
        if (!owner->closed && (owner->originalView == window ||
            owner->originalDrawingSurface == window)) {
            SessionScope scope(owner);
            // A replacement native view must stay uncovered until its own
            // selection has been verified. Disarm suppression synchronously.
            owner->originalWasVisible = false;
            owner->nativeSelectionGuarded = false;
            owner->rootNavigationPending = false;
            HideColumnsForNativeView(owner->overlay);
            QueueNativeNavigationUpdate();
        }
        RemoveWindowSubclass(window, NativeBoundsProc, subclass);
        for (HWND& tracked : owner->boundsWindows)
            if (tracked == window) tracked = nullptr;
        if (owner->originalView == window) {
            owner->originalView = nullptr;
            owner->originalDrawingSurface = nullptr;
            owner->originalWasVisible = false;
            owner->nativeSelectionGuarded = false;
        }
        if (owner->originalDrawingSurface == window) {
            owner->originalDrawingSurface = nullptr;
            owner->originalWasVisible = false;
            owner->nativeSelectionGuarded = false;
        }
        if (owner->tab == window && IsWindow(owner->overlay))
            PostMessageW(owner->overlay, WM_CLOSE, 0, 0);
    }
    LRESULT result = DefSubclassProc(window, message, wp, lp);
    if (!g_unloading.load() && !owner->closed &&
        (message == WM_WINDOWPOSCHANGED || message == WM_SIZE || message == WM_STYLECHANGED ||
         message == WM_DPICHANGED_AFTERPARENT ||
         message == WM_SHOWWINDOW || message == WM_NCDESTROY ||
         (message == WM_PARENTNOTIFY &&
          (LOWORD(wp) == WM_CREATE || LOWORD(wp) == WM_DESTROY))))
        QueueOverlayBoundsUpdate(*owner);
    return result;
}

void DetachNativeBoundsTracking() {
    auto& session = Context();
    for (HWND& window : session.boundsWindows) {
        if (IsWindow(window))
            RemoveWindowSubclass(window, NativeBoundsProc,
                reinterpret_cast<UINT_PTR>(&session));
        window = nullptr;
    }
    session.boundsWindowCount = 0;
    session.boundsUpdatePosted = false;
}

void TrackNativeBounds(HWND source) {
    HWND chain[ARRAYSIZE(Context().boundsWindows)] = {};
    unsigned count = 0;
    HWND drawingSurface = FindWindowExW(source, nullptr, L"DirectUIHWND", nullptr);
    if (drawingSurface) chain[count++] = drawingSurface;
    for (HWND window = source; window && count < ARRAYSIZE(chain);
         window = GetParent(window)) {
        if (GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId()) break;
        chain[count++] = window;
        if (window == Context().tab) break;
    }
    if (count == Context().boundsWindowCount &&
        std::equal(chain, chain + count, Context().boundsWindows)) return;
    DetachNativeBoundsTracking();
    for (unsigned i = 0; i < count; ++i) {
        if (SetWindowSubclass(chain[i], NativeBoundsProc,
                reinterpret_cast<UINT_PTR>(&Context()), reinterpret_cast<DWORD_PTR>(&Context())))
            Context().boundsWindows[Context().boundsWindowCount++] = chain[i];
        else LogWin32(L"SetWindowSubclass(native bounds)");
    }
}

void RedrawVacatedOverlayArea(HWND parent, const RECT& previous, const RECT& current) {
    HRGN exposed = CreateRectRgnIndirect(&previous);
    HRGN covered = CreateRectRgnIndirect(&current);
    if (exposed && covered) {
        int type = CombineRgn(exposed, exposed, covered, RGN_DIFF);
        if (type != ERROR && type != NULLREGION)
            RedrawWindow(parent, nullptr, exposed, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    } else {
        RedrawWindow(parent, &previous, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }
    if (exposed) DeleteObject(exposed);
    if (covered) DeleteObject(covered);
}

void SyncOverlayBounds(HWND overlay, HWND originalView, bool nativeLayout = false) {
    if (Context().suspended) return;
    if (NativeMenuSessionOnThread()) return;
    if (Context().closed || Context().syncingOverlayBounds || !IsWindow(overlay)) return;
    auto owner = Context().shared_from_this();
    Context().syncingOverlayBounds = true;
    struct SyncScope {
        Session& session;
        ~SyncScope() { session.syncingOverlayBounds = false; }
    } syncScope{Context()};
    if (!IsWindow(originalView) || !IsChild(Context().tab, originalView) || SessionForWindow(originalView))
        originalView = FindOriginalView(Context().tab);
    if (!originalView) {
        // A replacement native view may not exist until its current layout ends.
        ShowWindow(overlay, SW_HIDE);
        return;
    }
    Context().originalView = originalView;
    TrackNativeBounds(originalView);
    if (!HideOriginalView(originalView)) {
        HideColumnsForNativeView(overlay);
        return;
    }
    RECT rect = {};
    if (!ViewRectInTab(originalView, Context().tab, &rect, true)) return;
    RECT now = {};
    GetWindowRect(overlay, &now);
    MapWindowPoints(nullptr, Context().tab, reinterpret_cast<POINT*>(&now), 2);
    bool moved = now.left != rect.left || now.top != rect.top ||
                 now.right != rect.right || now.bottom != rect.bottom;
    HWND originalBranch = originalView;
    HWND parent = GetParent(overlay);
    while (originalBranch && GetParent(originalBranch) != parent)
        originalBranch = GetParent(originalBranch);
    bool covered = false;
    if (originalBranch && originalBranch != overlay)
        for (HWND above = GetWindow(overlay, GW_HWNDPREV); above; above = GetWindow(above, GW_HWNDPREV))
            if (above == originalBranch) { covered = true; break; }
    bool hidden = !(GetWindowLongPtrW(overlay, GWL_STYLE) & WS_VISIBLE);
    if (moved || covered || hidden) {
        // Only correct the original file view's stacking. Other siblings can
        // be native flyouts; moving above every sibling steals their input.
        HWND after = covered ? GetWindow(originalBranch, GW_HWNDPREV) : nullptr;
        UINT flags = SWP_NOACTIVATE;
        if (moved) flags |= SWP_NOCOPYBITS;
        if (!covered) flags |= SWP_NOZORDER;
        if (!moved) flags |= SWP_NOMOVE | SWP_NOSIZE;
        if (hidden) flags |= SWP_SHOWWINDOW;
        if (!SetWindowPos(overlay, after, rect.left, rect.top,
                rect.right - rect.left, rect.bottom - rect.top, flags)) return;
        if (Context().closed) return;
        if (Context().suspended || !Context().nativeSelectionGuarded) {
            HideColumnsForNativeView(overlay);
            return;
        }
        if (moved) RedrawVacatedOverlayArea(Context().tab, now, rect);
    }
    // The native file views can keep their own size while their entire parent
    // moves with Explorer's navigation pane. Repaint those descendants too;
    // invalidating the WS_CLIPCHILDREN wrapper alone leaves copied sidebar pixels.
    if (moved || covered || hidden || nativeLayout)
        RedrawWindow(overlay, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}

bool ApplyNativeLocation(NativeLocation location) {
    HWND hwnd = Context().overlay;
    if (Context().nativeNavigationInProgress) return false;
    if (location.kind == NativeLocationKind::TransientFailure) return false;
    if (NativePointerInteractionActive()) {
        SchedulePointerRelease(false, true);
        return false; // Re-query the latest native root after release.
    }
    Context().nativePointerPending = false; // A successful query confirms the root.
    ULONG_PTR restorationEpoch = location.inputSnapshotValid ? location.inputEpoch : g_nativeInputEpoch.load();
    ULONGLONG restorationStarted = location.inputSnapshotValid ? location.inputObservedAt : GetTickCount64();
    ULONG_PTR connection = Context().nativeConnectionGeneration;
    ULONG_PTR navigation = Context().nativeNavigationGeneration;
    HWND current = FindOriginalView(Context().tab);
    if (Context().closed || Context().nativeNavigationInProgress ||
            Context().nativeConnectionGeneration != connection ||
            Context().nativeNavigationGeneration != navigation) return false;
    if (location.kind == NativeLocationKind::Supported && !current) {
        HideColumnsForNativeView(hwnd, NativeViewReason::Navigation);
        RetryNativeViewTransition();
        return false;
    }
    if (current) Context().originalView = current;
    bool navigated = Context().tabPath.empty() || !Context().nativeLocationIdentity ||
        !ILIsEqual(location.identity.get(), Context().nativeLocationIdentity);
    if (location.kind == NativeLocationKind::Unsupported) {
        Context().pointerRootCompletionSerial = 0;
        CoTaskMemFree(Context().nativeLocationIdentity);
        Context().nativeLocationIdentity = location.identity.release();
        SuspendColumnsForNativeLocation(hwnd);
        return false;
    }
    if (navigated || Context().rootNavigationRetry ||
        (Context().suspended && Context().nativeViewReason != NativeViewReason::Selection &&
            !Context().rootNavigationPending)) {
        auto previous = std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)>(
            Context().nativeLocationIdentity ? ILCloneFull(Context().nativeLocationIdentity) : nullptr, CoTaskMemFree);
        auto intendedRoot = std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)>(
            ILCloneFull(location.identity.get()), CoTaskMemFree);
        auto expected = std::unique_ptr<ITEMIDLIST, decltype(&CoTaskMemFree)>(
            NativeRestorationChild(location.identity.get(), previous.get()), CoTaskMemFree);
        if (Context().closed || Context().nativeNavigationInProgress ||
                Context().nativeConnectionGeneration != connection ||
                Context().nativeNavigationGeneration != navigation) return false;
        RetireNativeRestorationPermit();
        Context().nativeRestorationBlockedSerial = 0;
        Context().pointerRootCompletionSerial = 0;
        HideColumnsForNativeView(hwnd, NativeViewReason::Navigation);
        Context().tabPath = location.path;
        CoTaskMemFree(Context().nativeLocationIdentity);
        Context().nativeLocationIdentity = location.identity.release();
        Context().rootNavigationPending = true;
        Context().rootNavigationRetry = false;
        RemoveColumnsAfter(0);
        Context().columnSelection[0].clear();
        Pane& active = Context().panes[0];
        active.awaitingNavigationRepair = true;
        active.navigationStarted = GetTickCount();
        active.selectionDirty = true;
        bool loaded = false;
        ULONG_PTR rootSerial = 0;
        if (!Context().columnCount) {
            // Unsupported initial roots retain only their navigation observer.
            // Build the first column on the first supported navigation event.
            Context().columnCount = 1;
            Context().columnWidths[0] = DefaultColumnWidth(hwnd);
            loaded = CreatePane(hwnd, 0, Context().tabPath, Context().nativeLocationIdentity, &rootSerial) &&
                CreateSplitter(0);
            if (!loaded) {
                HWND paneWindow = active.window;
                ReleasePane(active);
                if (IsWindow(paneWindow)) DestroyWindow(paneWindow);
                if (IsWindow(Context().splitters[0])) DestroyWindow(Context().splitters[0]);
                Context().splitters[0] = nullptr;
                active = Pane{};
                Context().columnCount = 0;
            }
        } else {
            loaded = SUCCEEDED(BrowsePidl(active, Context().nativeLocationIdentity, Context().tabPath, &rootSerial));
        }
        SetActivePane(0);
        if (!loaded) {
            active.awaitingNavigationRepair = false;
            SuspendColumnsForRootFailure(hwnd);
        } else {
            LayoutPanes(hwnd);
            if (!Context().closed && !Context().nativeNavigationInProgress &&
                    Context().nativeConnectionGeneration == connection &&
                    Context().nativeNavigationGeneration == navigation && Context().columnCount &&
                    rootSerial && Context().panes[0].navigationSerial == rootSerial &&
                    intendedRoot && Context().nativeLocationIdentity &&
                    ILIsEqual(intendedRoot.get(), Context().nativeLocationIdentity))
                ArmNativeRestorationPermit(expected.release(), restorationEpoch, restorationStarted);
        }
        Wh_Log(L"[Columns] Native location changed; root=%s", Context().tabPath.c_str());
        // Keep the good native view until a matching completion message arrives.
        return false;
    }
    if (Context().rootNavigationPending) {
        // A deferred native-root query now confirms this is still the same
        // location. Only now may its saved completion reveal the overlay.
        ULONG_PTR complete = std::exchange(Context().pointerRootCompletionSerial, 0);
        if (complete && Context().rootNavigationPending &&
            complete == Context().panes[0].navigationSerial)
            PostMessageW(hwnd, kPaneNavigationCompleteMessage, 0, complete);
        return false;
    }
    // Same-root native selections temporarily reveal the native view while
    // retaining the trail and Inspector. Only its own deselection resumes us.
    if (Context().nativeViewReason == NativeViewReason::Selection)
        Context().suspended = false;
    if (!HideOriginalView(Context().originalView)) {
        HideColumnsForNativeView(hwnd);
        RetryNativeViewTransition();
        return false;
    }
    Context().nativeSelectionYielded = false;
    Context().nativeViewReason = NativeViewReason::None;
    SyncOverlayBounds(hwnd, Context().originalView);
    return !Context().suspended && Context().nativeSelectionGuarded;
}

void CleanupSession(HWND hwnd) {
    if (Context().closed) return;
    // Retire the cross-thread handles before releasing UI-owned state.
    Context().closed = true;
    DetachNativeBoundsTracking();
    CancelPendingFocus();
    CancelColumnResize();
    KillTimer(hwnd, kVisualRepairTimerId);
    KillTimer(hwnd, kSettledPaintTimerId);
    KillTimer(hwnd, kDividerPaintTimerId);
    KillTimer(hwnd, kFocusTimerId);
    KillTimer(hwnd, kStateRetryTimerId);
    KillTimer(hwnd, kSelectionRetryTimerId);
    KillTimer(hwnd, kPaletteRefreshTimerId);
    KillTimer(hwnd, kPointerReleaseTimerId);
    Context().pointerSelectionPending = false;
    Context().nativePointerPending = false;
    Context().pointerRootCompletionSerial = 0;
    // UI closure never joins provider or decoder calls. The helper owns retired
    // jobs until their thread handles signal; actual DLL unload drains them.
    RetirePreviewJob(Context().imageJob);
    RetirePreviewJob(Context().waveJob);
    MSG imageMessage = {};
    while (PeekMessageW(&imageMessage, hwnd, kImageReadyMessage,
                        kImageReadyMessage, PM_REMOVE)) {}
    MSG waveMessage = {};
    while (PeekMessageW(&waveMessage, hwnd, kWaveReadyMessage,
                        kWaveReadyMessage, PM_REMOVE)) {}
    if (Context().mouseHookAcquired) {
        Context().mouseHookAcquired = false;
        if (--g_uiMouseUsers == 0 && g_uiMouseHook) {
            UnhookWindowsHookEx(g_uiMouseHook);
            g_uiMouseHook = nullptr;
        }
    }
    ResetNativeTabConnection();
    CoTaskMemFree(Context().nativeLocationIdentity);
    Context().nativeLocationIdentity = nullptr;
    RestoreOriginalView();
    ClearPreview();
    DeleteInspectorFonts();
    for (int i = 0; i < Context().columnCount; ++i)
        ReleasePane(Context().panes[i]);
    Context().columnCount = 0;
    Context().scrollX = 0;
    for (auto& splitter : Context().splitters) splitter = nullptr;
    Context().overlay = nullptr;
    Context().tab = nullptr;
    Context().explorer = nullptr;
    Context().originalView = nullptr;
    Context().tabPath.clear();
    if (Context().uiComInitialized) {
        Context().uiComInitialized = false;
        CoUninitialize();
    }
    Wh_Log(L"[Columns] Closed");
}

LRESULT CALLBACK OverlayProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    CallbackScope callbackScope;
    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lp);
        if (!create->lpCreateParams ||
            !SetPropW(hwnd, kSessionProperty, create->lpCreateParams)) return FALSE;
    }
    auto* owner = reinterpret_cast<Session*>(GetPropW(hwnd, kSessionProperty));
    if (!owner) return DefWindowProcW(hwnd, message, wp, lp);
    SessionScope scope(owner);
    if (message == WM_NCDESTROY) {
        RemovePropW(hwnd, kSessionProperty);
        ForgetSession(hwnd);
        return DefWindowProcW(hwnd, message, wp, lp);
    }
    // COM release can pump messages during teardown. Ignore queued layout and
    // worker notifications after CleanupSession has retired their resources.
    if (Context().closed && message != WM_CLOSE && message != WM_DESTROY)
        return DefWindowProcW(hwnd, message, wp, lp);
    switch (message) {
        case kOverlayBoundsMessage:
            if (wp != Context().identity) return 0;
            Context().boundsUpdatePosted = false;
            if (!IsWindow(Context().tab)) {
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
                return 0;
            }
            {
                bool visible = IsWindowVisible(Context().tab) != FALSE;
                if (!visible && Context().tabWasVisible) PauseMediaPreview();
                if (visible && !Context().tabWasVisible) {
                    Context().stateRetryAttempts = 0;
                    Context().navigationConnectAttempts = 0;
                    QueueNativeNavigationUpdate();
                }
                Context().tabWasVisible = visible;
                HWND root = GetAncestor(Context().tab, GA_ROOT);
                if (root != Context().explorer && IsExplorerWindow(root)) {
                    Context().explorer = root;
                    Context().stateRetryAttempts = 0;
                    Context().navigationConnectAttempts = 0;
                    QueueNativeNavigationUpdate();
                }
                HWND current = FindOriginalView(Context().tab);
                if (current && current != Context().originalView) {
                    Context().originalView = current;
                    Context().stateRetryAttempts = 0;
                    Context().navigationConnectAttempts = 0;
                    QueueNativeNavigationUpdate();
                }
                TrackNativeBounds(current ? current : Context().tab);
            }
            SyncOverlayBounds(hwnd, Context().originalView, true);
            return 0;
        case kNativeStructureMessage:
            if (wp != Context().identity) return 0;
            Context().stateRetryAttempts = 0;
            Context().navigationConnectAttempts = 0;
            QueueOverlayBoundsUpdate(Context());
            QueueNativeNavigationUpdate();
            return 0;
        case kPaletteRefreshMessage:
            if (wp != Context().identity) return 0;
            SetTimer(hwnd, kPaletteRefreshTimerId, kPaletteSettleDelayMs, nullptr);
            return 0;
        case kLayoutSettingsChangedMessage:
        {
            CancelColumnResize();
            Context().dividerWidth = g_dividerWidthSetting.load();
            ReadInspectorColors();
            bool previous = Context().fixedInspector;
            Context().fixedInspector = g_fixedInspectorSetting.load();
            bool mediaChanged = Context().inlineMediaPlayback != g_inlineMediaPlayback.load();
            Context().inlineMediaPlayback = g_inlineMediaPlayback.load();
            if (!g_delayedPaintRepairs.load()) {
                KillTimer(hwnd, kVisualRepairTimerId);
                KillTimer(hwnd, kSettledPaintTimerId);
                KillTimer(hwnd, kDividerPaintTimerId);
            }
            if (mediaChanged && Context().previewMedia && !Context().previewPath.empty()) {
                std::wstring path = Context().previewPath;
                ShowPreview(path);
            }
            if (!Context().fixedInspector && Context().previewIsFolder) ClearPreview();
            if (Context().fixedInspector && !previous && Context().previewIndex < 0) {
                SelectedItem selected = SelectedPath(Context().panes[Context().activePane]);
                if (!selected.path.empty()) ShowPreview(selected.path);
            }
            if (previous != Context().fixedInspector) ScrollToNewestColumn();
            else LayoutPanes(hwnd);
            RedrawWindow(hwnd, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
            Wh_Log(L"[Columns settings] Applied v%s tab=%p fixedInspector=%d classicMenus=%d divider=%d",
                WH_MOD_VERSION, Context().tab, Context().fixedInspector,
                g_classicContextMenus.load(), Context().dividerWidth);
            return 0;
        }
        case WM_DPICHANGED:
        case WM_DPICHANGED_AFTERPARENT:
            RefreshLayoutDpi(GetDpiForWindow(hwnd));
            DeleteInspectorFonts();
            LayoutPanes(hwnd);
            return 0;
        case WM_SIZE:
            if (!Context().inLayout) {
                RefreshLayoutDpi(GetDpiForWindow(hwnd));
                LayoutPanes(hwnd);
            }
            return 0;
        case kImageReadyMessage: {
            HBITMAP bitmap = nullptr;
            DWORD generation = 0;
            if (!TakePreviewResult(Context().imageJob, wp, hwnd,
                    &generation, &bitmap, nullptr)) return 0;
            if (generation == Context().previewGeneration && Context().previewWindow &&
                Context().imageWanted) {
                Context().imageWanted = false;
                if (bitmap) {
                    Context().previewBitmap = bitmap;
                    Context().previewMessage.clear();
                    InvalidateRect(Context().previewWindow, nullptr, FALSE);
                } else {
                    Context().previewMessage = L"Image preview is unavailable.";
                    InvalidateRect(Context().previewWindow, nullptr, FALSE);
                }
            } else {
                if (bitmap) DeleteObject(bitmap);
                if (Context().imageWanted) LaunchImageWorker();
            }
            return 0;
        }
        case kWaveReadyMessage: {
            std::unique_ptr<WavePreview> result;
            DWORD generation = 0;
            if (!TakePreviewResult(Context().waveJob, wp, hwnd,
                    &generation, nullptr, &result)) return 0;
            if (generation == Context().previewGeneration &&
                Context().previewWindow && Context().waveWanted) {
                Context().waveWanted = false;
                delete Context().wavePreview;
                Context().wavePreview = result.release();
                auto* ready = Context().wavePreview;
                if (ready && (ready->codec == L"AAC" || ready->codec == L"MP3" || ready->codec == L"Opus"))
                    Context().audioBits = 0;
                if (ready && ready->supported && ready->sampleRate) {
                    LONGLONG wavDuration = static_cast<LONGLONG>(
                        ready->frames * 10000000ULL / ready->sampleRate);
                    if (Context().audioDuration <= 0) Context().audioDuration = wavDuration;
                }
                InvalidateRect(Context().previewWindow, nullptr, FALSE);
            } else {
                if (Context().waveWanted) LaunchWaveWorker();
            }
            return 0;
        }
        case WM_HSCROLL: {
            SCROLLINFO info = {};
            info.cbSize = sizeof(info);
            info.fMask = SIF_TRACKPOS;
            GetScrollInfo(hwnd, SB_HORZ, &info);
            RECT area = {};
            GetClientRect(hwnd, &area);
            int code = LOWORD(wp);
            if (code == SB_LINELEFT) Context().scrollX -= 48;
            else if (code == SB_LINERIGHT) Context().scrollX += 48;
            else if (code == SB_PAGELEFT)
                Context().scrollX -= area.right - InspectorWidth(hwnd);
            else if (code == SB_PAGERIGHT)
                Context().scrollX += area.right - InspectorWidth(hwnd);
            else if (code == SB_THUMBTRACK || code == SB_THUMBPOSITION)
                Context().scrollX = info.nTrackPos;
            LayoutPanes(hwnd);
            return 0;
        }
        case WM_MOUSEHWHEEL:
            Context().scrollX += -GET_WHEEL_DELTA_WPARAM(wp) / 2;
            LayoutPanes(hwnd);
            return 0;
        case kPaneNavigationCompleteMessage:
            if (wp < static_cast<WPARAM>(Context().columnCount) &&
                static_cast<ULONG_PTR>(lp) == Context().panes[wp].navigationSerial) {
                if (wp == 0 && Context().rootNavigationPending) {
                    if (NativePointerInteractionActive() || Context().nativePointerPending) {
                        Context().pointerRootCompletionSerial = static_cast<ULONG_PTR>(lp);
                        SchedulePointerRelease(false);
                        return 0;
                    }
                    // Explorer can change roots or begin New/Paste rename
                    // while this column's asynchronous navigation finishes.
                    // Revalidate the actual root before allowing takeover.
                    ULONG_PTR navigation = Context().nativeNavigationGeneration;
                    ULONG_PTR connection = Context().nativeConnectionGeneration;
                    NativeLocation native = ReadNativeLocation();
                    if (Context().closed || !Context().columnCount ||
                        !Context().rootNavigationPending ||
                        static_cast<ULONG_PTR>(lp) != Context().panes[0].navigationSerial) return 0;
                    if (Context().nativeNavigationInProgress ||
                        native.kind == NativeLocationKind::TransientFailure) {
                        Context().pointerRootCompletionSerial = static_cast<ULONG_PTR>(lp);
                        QueueNativeNavigationUpdate();
                        return 0;
                    }
                    if (native.kind != NativeLocationKind::Supported ||
                        !Context().nativeLocationIdentity ||
                        !ILIsEqual(native.identity.get(), Context().nativeLocationIdentity)) {
                        ApplyNativeLocation(std::move(native));
                        return 0;
                    }
                    HWND current = FindOriginalView(Context().tab);
                    if (Context().closed || !Context().columnCount ||
                            !Context().rootNavigationPending || Context().nativeNavigationInProgress ||
                            Context().nativeNavigationGeneration != navigation ||
                            Context().nativeConnectionGeneration != connection ||
                            static_cast<ULONG_PTR>(lp) != Context().panes[0].navigationSerial) return 0;
                    if (!current) {
                        Context().pointerRootCompletionSerial = static_cast<ULONG_PTR>(lp);
                        HideColumnsForNativeView(hwnd, NativeViewReason::Navigation);
                        RetryNativeViewTransition();
                        return 0;
                    }
                    Context().suspended = false;
                    Context().originalView = current;
                    if (!HideOriginalView(Context().originalView)) {
                        Context().pointerRootCompletionSerial = static_cast<ULONG_PTR>(lp);
                        HideColumnsForNativeView(hwnd);
                        RetryNativeViewTransition();
                        return 0;
                    }
                    Context().rootNavigationPending = false;
                    Context().rootNavigationRetry = false;
                    Context().nativeSelectionYielded = false;
                    Context().nativeViewReason = NativeViewReason::None;
                    SyncOverlayBounds(hwnd, Context().originalView);
                    if (Context().suspended || Context().closed) return 0;
                    QueueSelectionUpdate(Context().panes[0]);
                }
                RefreshPane(Context().panes[wp]);
                KillTimer(hwnd, kVisualRepairTimerId);
                KillTimer(hwnd, kSettledPaintTimerId);
                if (g_delayedPaintRepairs.load()) {
                    SetTimer(hwnd, kVisualRepairTimerId, kVisualRepairDelayMs, nullptr);
                    SetTimer(hwnd, kSettledPaintTimerId, kSettledPaintDelayMs, nullptr);
                } else {
                    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN);
                }
            }
            return 0;
        case kRootNavigationFailedMessage:
            if (wp == Context().identity && Context().rootNavigationPending &&
                static_cast<ULONG_PTR>(lp) == Context().panes[0].navigationSerial)
                SuspendColumnsForRootFailure(hwnd);
            return 0;
        case kNavigationEventMessage:
        case kSelectionUpdateMessage:
        case WM_TIMER: {
            if (message == WM_TIMER && wp == kPaletteRefreshTimerId) {
                if (NativeMenuSessionOnThread()) return 0;
                KillTimer(hwnd, kPaletteRefreshTimerId);
                ReadInspectorColors();
                RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN);
                return 0;
            }
            if (message == kNavigationEventMessage) {
                if (wp != Context().identity) return 0;
                Context().navigationUpdatePosted = false;
            } else if (message == kSelectionUpdateMessage) {
                if (wp != Context().identity) return 0;
                Context().selectionUpdatePosted = false;
            }
            if (!IsWindow(Context().tab) || !IsWindow(Context().explorer)) {
                SendMessageW(hwnd, WM_CLOSE, 0, 0);
                return 0;
            }
            // Native flyouts are asynchronous. Their input site can live
            // inside a column, while another tab still has a pending focus
            // request. Pause every session on this UI thread until it closes.
            if (NativeMenuSessionOnThread()) {
                // Keep dirty state and requeue after the asynchronous flyout
                // releases input; do not mutate views inside its nested pump.
                if (message == kSelectionUpdateMessage)
                    SetTimer(hwnd, kSelectionRetryTimerId, kSelectionRetryDelayMs, nullptr);
                else if (message == kNavigationEventMessage)
                    SetTimer(hwnd, kStateRetryTimerId, 100, nullptr);
                return 0;
            }
            if (message == WM_TIMER && wp == kPointerReleaseTimerId) {
                if (NativePointerInteractionActive()) return 0;
                KillTimer(hwnd, kPointerReleaseTimerId);
                bool selection = std::exchange(Context().pointerSelectionPending, false);
                // Keep this obligation across transient queries and later
                // clicks; only a confirmed native location may clear it.
                bool nativeNavigation = Context().nativePointerPending;
                ULONG_PTR complete = Context().pointerRootCompletionSerial;
                // A transient root query must not let a previously completed
                // root cover the native view. Keep its completion until a good
                // native query confirms the location in ApplyNativeLocation.
                if (nativeNavigation) QueueNativeNavigationUpdate();
                else {
                    Context().pointerRootCompletionSerial = 0;
                    if (complete)
                        PostMessageW(hwnd, kPaneNavigationCompleteMessage, 0, complete);
                }
                if (selection && !nativeNavigation && !complete &&
                    IsWindowVisible(Context().tab) && !Context().suspended)
                    ProcessSelectionUpdates(false);
                return 0;
            }
            if (message == WM_TIMER && wp == kSelectionRetryTimerId) {
                KillTimer(hwnd, kSelectionRetryTimerId);
                if (IsWindowVisible(Context().tab) && !Context().suspended)
                    ProcessSelectionUpdates(false);
                return 0;
            }
            if (message == WM_TIMER && wp == kFocusTimerId) {
                if (!IsWindowVisible(Context().tab) || Context().suspended) {
                    KillTimer(hwnd, kFocusTimerId);
                    Context().focusRequest = -1;
                    return 0;
                }
                if (NativePointerInteractionActive()) return 0;
                if (Context().focusRequest < 0 ||
                    FocusColumn(Context().focusRequest) || ++Context().focusAttempts >= 20) {
                    KillTimer(hwnd, kFocusTimerId);
                    Context().focusRequest = -1;
                }
                return 0;
            }
            if (message == WM_TIMER && wp == kVisualRepairTimerId) {
                KillTimer(hwnd, kVisualRepairTimerId);
                TestPaintOnly(L"early navigation");
                return 0;
            }
            if (message == WM_TIMER && wp == kSettledPaintTimerId) {
                KillTimer(hwnd, kSettledPaintTimerId);
                TestPaintOnly(L"settled navigation");
                return 0;
            }
            if (message == WM_TIMER && wp == kDividerPaintTimerId) {
                KillTimer(hwnd, kDividerPaintTimerId);
                TestPaintOnly(L"settled divider drag");
                return 0;
            }
            if (message == WM_TIMER && wp != kStateRetryTimerId) return 0;
            if (message == WM_TIMER && wp == kStateRetryTimerId) KillTimer(hwnd, kStateRetryTimerId);
            if (!IsWindowVisible(Context().tab)) return 0;
            HWND focused = GetFocus();
            for (int i = 0; i < Context().columnCount; ++i) {
                if (focused && Context().panes[i].window &&
                    (focused == Context().panes[i].window ||
                     IsChild(Context().panes[i].window, focused))) SetActivePane(i);
            }
            // Selection notifications take the direct path. Native events and
            // their bounded transient-failure retries consult the cached browser.
            if (message != kSelectionUpdateMessage) {
                ConnectNavigationEvents(Context().explorer); // Retries failed connections.
                NativeLocation location = ReadNativeLocation();
                if (location.kind == NativeLocationKind::TransientFailure) {
                    if (++Context().stateRetryAttempts <= 10)
                        SetTimer(hwnd, kStateRetryTimerId, 100, nullptr);
                    if (Context().stateRetryAttempts == 3) ResetNativeTabConnection();
                    return 0;
                }
                Context().stateRetryAttempts = 0;
                KillTimer(hwnd, kStateRetryTimerId);
                // A readable location does not imply Advise succeeded. Keep a
                // short, bounded connection-recovery window even for dormant
                // unsupported roots; successful subscriptions need no polling.
                if (Context().navigationCookie) Context().navigationConnectAttempts = 0;
                else if (++Context().navigationConnectAttempts <= 10)
                    SetTimer(hwnd, kStateRetryTimerId, 100, nullptr);
                if (!ApplyNativeLocation(std::move(location))) return 0;
            }
            ProcessSelectionUpdates(false);
            return 0;
        }
        case WM_ERASEBKGND: {
            PaintColumnBackground(hwnd, reinterpret_cast<HDC>(wp));
            return 1;
        }
        case WM_PAINT: {
            PAINTSTRUCT paint = {};
            HDC dc = BeginPaint(hwnd, &paint);
            PaintColumnBackground(hwnd, dc, &paint.rcPaint);
            EndPaint(hwnd, &paint);
            return 0;
        }
        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            SetTimer(hwnd, kPaletteRefreshTimerId, kPaletteSettleDelayMs, nullptr);
            return 0;
        case WM_CLOSE:
            CleanupSession(hwnd);
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            CleanupSession(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

bool CreatePane(HWND overlay, int index, const std::wstring& path, PCIDLIST_ABSOLUTE identity,
        ULONG_PTR* startedSerial) {
    Pane& pane = Context().panes[index];
    pane.index = index;
    pane.generation = ++g_paneSequence;
    pane.selectionDirty = true;
    pane.session = &Context();
    pane.window = CreateWindowExW(0, kPaneClass, L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0, 0, 100, 100, overlay, nullptr, HINST_THISCOMPONENT, &pane);
    if (!pane.window) {
        LogWin32(L"CreateWindowEx(pane)");
        return false;
    }
    pane.viewHost = CreateWindowExW(0, kViewHostClass, L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0, 0, 100, 100, pane.window, nullptr,
        HINST_THISCOMPONENT, &pane);
    if (!pane.viewHost) {
        LogWin32(L"CreateWindowEx(view host)");
        return false;
    }
    HRESULT hr = CoCreateInstance(CLSID_ExplorerBrowser, nullptr,
        CLSCTX_INPROC_SERVER, IID_IExplorerBrowser,
        reinterpret_cast<void**>(&pane.browser));
    if (FAILED(hr)) {
        LogHr(L"CoCreateInstance(ExplorerBrowser)", hr);
        return false;
    }
    hr = pane.browser->QueryInterface(IID_IShellBrowser,
        reinterpret_cast<void**>(&pane.navigationBrowser));
    if (FAILED(hr)) {
        LogHr(L"ExplorerBrowser navigation interface", hr);
        if (IsSupportedMenuPlatform()) return false;
    }
    pane.menuRequests = 0;
    pane.lastMenuResult = E_PENDING;
    pane.browserSite = new (std::nothrow) PaneBrowserSite(index);
    if (!pane.browserSite) return false;
    IObjectWithSite* withSite = nullptr;
    hr = pane.browser->QueryInterface(IID_IObjectWithSite,
        reinterpret_cast<void**>(&withSite));
    if (SUCCEEDED(hr)) {
        hr = withSite->SetSite(pane.browserSite);
        withSite->Release();
    }
    Wh_Log(L"[Columns menu] Pane %d browser site=%08X", index, static_cast<unsigned>(hr));
    // Selection updates depend on this site's ICommDlgBrowser2 callbacks.
    // If it cannot be installed, preserve native Explorer instead of opening
    // a column that would require a permanent selection-polling fallback.
    if (FAILED(hr)) {
        pane.browserSite->Release();
        pane.browserSite = nullptr;
        return false;
    }
    hr = pane.browser->SetOptions(static_cast<EXPLORER_BROWSER_OPTIONS>(
        EBO_NOBORDER | EBO_NOPERSISTVIEWSTATE));
    if (FAILED(hr)) LogHr(L"SetOptions", hr);
    RECT inside = {0, 0, 100, 100};
    FOLDERSETTINGS settings = {FVM_DETAILS, FWF_AUTOARRANGE};
    hr = pane.browser->Initialize(pane.viewHost, &inside, &settings);
    if (FAILED(hr)) {
        LogHr(L"Initialize", hr);
        return false;
    }
    pane.initialized = true;
    pane.navigationSink = new (std::nothrow) PaneNavigationSink(index);
    if (!pane.navigationSink) return false;
    hr = pane.browser->Advise(pane.navigationSink, &pane.navigationCookie);
    if (FAILED(hr)) {
        LogHr(L"ExplorerBrowser::Advise", hr);
        return false;
    }
    if (FAILED(identity ? BrowsePidl(pane, identity, path, startedSerial) : BrowsePath(pane, path))) return false;
    RefreshPane(pane);
    return true;
}

void RemoveColumnsAfter(int index) {
    if (!Context().overlay || index < 0) return;
    if (Context().focusRequest > index) CancelPendingFocus();
    CancelColumnResize();
    if (Context().previewIndex > index) ClearPreview();
    while (Context().columnCount > index + 1) {
        int last = Context().columnCount - 1;
        HWND paneWindow = Context().panes[last].window;
        ReleasePane(Context().panes[last]);
        if (IsWindow(paneWindow)) DestroyWindow(paneWindow);
        if (IsWindow(Context().splitters[last]))
            DestroyWindow(Context().splitters[last]);
        Context().splitters[last] = nullptr;
        Context().columnWidths[last] = 0;
        Context().columnSelection[last].clear();
        --Context().columnCount;
    }
    Context().activePane = std::min(Context().activePane, Context().columnCount - 1);
    if (Context().columnCount > 0 && Context().panes[Context().columnCount - 1].window)
        InvalidateRect(Context().panes[Context().columnCount - 1].window, nullptr, FALSE);
    ScrollToNewestColumn();
}

bool AppendColumn(const std::wstring& path, PCIDLIST_ABSOLUTE identity) {
    ClearPreview();
    if (!Context().overlay || Context().columnCount >= kMaxColumns) {
        Wh_Log(L"[Columns] Maximum column depth reached");
        return false;
    }
    int index = Context().columnCount;
    if (!CreateSplitter(index))
        return false;
    if (!CreatePane(Context().overlay, index, path, identity)) {
        HWND paneWindow = Context().panes[index].window;
        ReleasePane(Context().panes[index]);
        if (IsWindow(paneWindow)) DestroyWindow(paneWindow);
        DestroyWindow(Context().splitters[index]);
        Context().splitters[index] = nullptr;
        return false;
    }
    Context().columnWidths[index] = DefaultColumnWidth(Context().overlay);
    Context().columnSelection[index].clear();
    ++Context().columnCount;
    if (index > 0)
        InvalidateRect(Context().panes[index - 1].window, nullptr, FALSE);
    InvalidateRect(Context().panes[index].window, nullptr, FALSE);
    ScrollToNewestColumn();
    return true;
}

void SetTabColumns(HWND explorer, HWND tab, bool enabled) {
    if (g_unloading.load() || !IsExplorerWindow(explorer) || !IsWindow(tab)) return;
    if (auto existing = SessionForTab(tab)) {
        if (!enabled) {
            SessionScope scope(existing);
            SendMessageW(Context().overlay, WM_CLOSE, 0, 0);
        }
        return;
    }
    if (!enabled) return;
    auto owner = std::make_shared<Session>();
    SessionScope scope(owner);
    struct CreationScope {
        CreationScope() { ++g_creatingSessions; }
        ~CreationScope() { --g_creatingSessions; }
    } creating;
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(coHr)) {
        LogHr(L"Explorer UI thread CoInitializeEx", coHr);
        return;
    }
    Context().uiComInitialized = true;
    Context().tab = tab;
    Context().explorer = explorer;
    // Early creation failure must release cached COM objects before CoUninitialize.
    struct CreationConnectionGuard {
        bool committed = false;
        ~CreationConnectionGuard() { if (!committed) ResetNativeTabConnection(); }
    } connectionGuard;
    HWND original = FindOriginalView(tab);
    RECT rect = {};
    if (!ViewRectInTab(original, tab, &rect, true) && !GetClientRect(tab, &rect)) {
        Context().uiComInitialized = false;
        CoUninitialize();
        return;
    }
    CacheNativeTabBrowser(explorer, tab);
    NativeLocation location = ReadNativeLocation();
    if (!g_columnsEnabled.load() || g_unloading.load()) {
        ResetNativeTabConnection();
        Context().uiComInitialized = false;
        CoUninitialize();
        return;
    }
    Context().inlineMediaPlayback = g_inlineMediaPlayback.load();
    Context().dividerWidth = g_dividerWidthSetting.load();
    ReadInspectorColors();
    Context().overlay = CreateWindowExW(0, kOverlayClass, L"Explorer Column View",
        WS_CHILD | WS_HSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        rect.left, rect.top, rect.right - rect.left,
        rect.bottom - rect.top, tab, nullptr, HINST_THISCOMPONENT, owner.get());
    if (!Context().overlay) {
        LogWin32(L"CreateWindowEx(overlay)");
        ResetNativeTabConnection();
        Context().uiComInitialized = false;
        CoUninitialize();
        return;
    }
    PublishSession(tab, Context().overlay, owner);
    connectionGuard.committed = true;
    Context().tab = tab;
    Context().explorer = explorer;
    Context().originalView = original;
    Context().fixedInspector = g_fixedInspectorSetting.load();
    Context().activePane = 0;
    Context().columnCount = 0;
    Context().scrollX = 0;
    for (auto& selection : Context().columnSelection) selection.clear();
    for (auto& width : Context().columnWidths) width = 0;
    Context().layoutDpi = GetDpiForWindow(Context().overlay);
    Context().columnWidths[0] = DefaultColumnWidth(Context().overlay);
    // A dormant session keeps navigation events for unsupported/transient roots.
    // A confirmed supported location creates pane zero through the same path
    // as subsequent sidebar navigation.
    Context().suspended = true;
    // A global off request can arrive while a slow Shell folder is opening.
    if (!g_columnsEnabled.load() || g_unloading.load()) {
        SendMessageW(Context().overlay, WM_CLOSE, 0, 0);
        return;
    }
    TrackNativeBounds(original ? original : tab);
    ConnectNavigationEvents(explorer);
    if (location.kind == NativeLocationKind::TransientFailure || !Context().navigationCookie)
        SetTimer(Context().overlay, kStateRetryTimerId, 100, nullptr);
    if (location.kind != NativeLocationKind::TransientFailure) ApplyNativeLocation(std::move(location));
    if (!g_uiMouseHook)
        g_uiMouseHook = SetWindowsHookExW(WH_MOUSE, MouseHookProc,
                                         nullptr, GetCurrentThreadId());
    if (g_uiMouseHook) {
        ++g_uiMouseUsers;
        Context().mouseHookAcquired = true;
    } else {
        LogWin32(L"SetWindowsHookEx(WH_MOUSE)");
    }
    DWORD tabThread = GetWindowThreadProcessId(tab, nullptr);
    Wh_Log(L"[Columns] UI thread: tab=%u overlay=%u",
           tabThread, GetWindowThreadProcessId(Context().overlay, nullptr));
    Wh_Log(L"[Columns] Navigation observer attached; root=%s", Context().tabPath.c_str());

}

void TestPaintOnly(const wchar_t* reason) {
    if (!IsWindow(Context().overlay)) return;
    DWORD start = GetTickCount();
    BOOL result = RedrawWindow(Context().overlay, nullptr, nullptr,
                               RDW_INVALIDATE | RDW_ERASE |
                               RDW_ALLCHILDREN | RDW_UPDATENOW);
    Wh_Log(L"[Columns] Paint-only redraw (%s)=%d elapsed=%u ms",
           reason,
           result ? 1 : 0, GetTickCount() - start);
}

bool IsColumnTextInput(HWND window) {
    // Input may be addressed to a child inside an edit/search/address host.
    // Walk its parents as well as recognizing the classic rename controls.
    for (HWND current = window; current; current = GetParent(current)) {
        wchar_t name[100] = {};
        GetClassNameW(current, name, ARRAYSIZE(name));
        if (!lstrcmpiW(name, L"Edit") || !_wcsnicmp(name, L"RichEdit", 8) ||
            !lstrcmpiW(name, L"SearchBox") ||
            !lstrcmpiW(name, L"SearchEditBoxWrapperClass") ||
            !lstrcmpiW(name, L"Address Band Root") ||
            !lstrcmpiW(name, L"AddressBandRoot") ||
            !lstrcmpiW(name, L"Windows.UI.Input.InputSite.WindowClass")) return true;
    }
    return false;
}

bool TranslateColumnAccelerator(MSG* message) {
    static thread_local bool translating = false;
    if (translating || IsColumnTextInput(message->hwnd)) return false;
    for (int i = 0; i < Context().columnCount; ++i) {
        Pane& pane = Context().panes[i];
        if (!pane.browser || !pane.viewHost ||
            (message->hwnd != pane.viewHost && !IsChild(pane.viewHost, message->hwnd))) continue;
        IShellView* shell = nullptr;
        HRESULT hr = pane.browser->GetCurrentView(IID_IShellView,
            reinterpret_cast<void**>(&shell));
        if (FAILED(hr)) return false;
        SetActivePane(i);
        translating = true;
        // Explorer must offer the focused Shell view its accelerators before
        // its frame translates them. The frame still owns its original view.
        hr = shell->TranslateAccelerator(message);
        shell->Release();
        bool handled = hr == S_OK;
        const char* verb = nullptr;
        if (!(GetKeyState(VK_MENU) & 0x8000)) {
            bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            if (ctrl && !shift) {
                if (message->wParam == 'C' || message->wParam == VK_INSERT) verb = "copy";
                else if (message->wParam == 'X') verb = "cut";
                else if (message->wParam == 'V') verb = "paste";
            } else if (shift && !ctrl && message->wParam == VK_INSERT) verb = "paste";
        }
        if (!handled && verb && !Context().closed) {
            // DefView leaves some frame commands, notably Paste, to its host.
            // Invoke this column's native Shell verb instead of the hidden view.
            hr = ColumnClipboardCommand(pane, verb, true);
            handled = true; // Even a disabled clipboard action must not reach the hidden original view.
        }
        translating = false;
        if ((GetKeyState(VK_CONTROL) & 0x8000) &&
            (message->wParam == 'C' || message->wParam == 'X' || message->wParam == 'V'))
            Wh_Log(L"[Columns keyboard] Tab=%p pane=%d Ctrl+%c result=%08X",
                Context().tab, i, static_cast<wchar_t>(message->wParam), static_cast<unsigned>(hr));
        return handled;
    }
    return false;
}

bool HandleToggleShortcut(MSG* msg) {
    const bool down = msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN;
    const bool up = msg->message == WM_KEYUP || msg->message == WM_SYSKEYUP;
    if (!down && !up) return false;
    static thread_local UINT consumedKey = 0;
    const bool repeat = down && (msg->lParam & (1LL << 30));
    if (msg->wParam == consumedKey) {
        // Finish a consumed press even if settings/modifiers/focus changed.
        // A fresh down also recovers from a keyup that never reached this thread.
        if (up || repeat) {
            if (up) consumedKey = 0;
            msg->message = WM_NULL;
            return true;
        }
        consumedKey = 0;
    }
    if (!down || repeat) return false;
    UINT shortcut = g_toggleShortcut.load();
    if (!shortcut || msg->wParam != (shortcut & 0xFF)) return false;
    UINT modifiers = 0;
    if (GetKeyState(VK_CONTROL) & 0x8000) modifiers |= kShortcutCtrl;
    if (GetKeyState(VK_MENU) & 0x8000) modifiers |= kShortcutAlt;
    if (GetKeyState(VK_SHIFT) & 0x8000) modifiers |= kShortcutShift;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) modifiers |= kShortcutWin;
    HWND foreground = GetForegroundWindow();
    if (modifiers != (shortcut & ~0xFFu) || (GetKeyState(VK_RMENU) & 0x8000) ||
        !IsExplorerWindow(foreground) || IsColumnTextInput(msg->hwnd) ||
        GetWindowThreadProcessId(msg->hwnd, nullptr) != GetCurrentThreadId() ||
        GetAncestor(msg->hwnd, GA_ROOT) != foreground) return false;
    if (!PostThreadMessageW(g_threadId, kToggleColumnsMessage, 0, 0)) return false;
    consumedKey = static_cast<UINT>(msg->wParam);
    msg->message = WM_NULL;
    return true;
}

LRESULT CALLBACK DispatchHookProc(int code, WPARAM wp, LPARAM lp) {
    CallbackScope callbackScope;
    if (g_unloading.load())
        return CallNextHookEx(nullptr, code, wp, lp);
    if (code >= 0 && wp == PM_REMOVE && lp) {
        MSG* msg = reinterpret_cast<MSG*>(lp);
        ObserveNativeRestorationInput(msg->message, msg->wParam);
        if (HandleToggleShortcut(msg)) return CallNextHookEx(nullptr, code, wp, lp);
        if (msg->message == g_toggleMessage) {
            HWND explorer = reinterpret_cast<HWND>(msg->wParam);
            LPARAM command = msg->lParam;
            msg->message = WM_NULL;
            if (command == kOpenColumnsCommand || command == kCloseColumnsCommand) {
                bool enabled = command == kOpenColumnsCommand;
                if (enabled == g_columnsEnabled.load()) SetTabColumns(explorer, msg->hwnd, enabled);
            }
            PostThreadMessageW(g_threadId, kCommandDoneMessage,
                reinterpret_cast<WPARAM>(msg->hwnd), command);
        } else if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN) {
            if (auto menuOwner = NativeMenuSessionOnThread()) {
                wchar_t name[100] = {};
                GetClassNameW(msg->hwnd, name, ARRAYSIZE(name));
                Wh_Log(L"[Columns menu input] Native menu owns key=%u target=%p (%s)",
                    static_cast<unsigned>(msg->wParam), msg->hwnd, name);
                return CallNextHookEx(nullptr, code, wp, lp);
            }
            CancelFocusForUserInput(msg->hwnd);
            Session* owner = SessionForWindow(msg->hwnd);
            if (!owner) return CallNextHookEx(nullptr, code, wp, lp);
            SessionScope scope(owner);
            if (msg->wParam == VK_ESCAPE && Context().resizeSplitter) {
                CancelColumnResize();
                msg->message = WM_NULL;
            }
            if (msg->message == WM_KEYDOWN && !IsColumnTextInput(msg->hwnd) &&
                Context().overlay && (msg->wParam == VK_LEFT ||
                              msg->wParam == VK_RIGHT) &&
                !(GetKeyState(VK_CONTROL) & 0x8000) &&
                !(GetKeyState(VK_MENU) & 0x8000) &&
                !(GetKeyState(VK_SHIFT) & 0x8000)) {
                for (int i = 0; i < Context().columnCount; ++i) {
                    if (!Context().panes[i].viewHost ||
                        (msg->hwnd != Context().panes[i].viewHost &&
                         !IsChild(Context().panes[i].viewHost, msg->hwnd)))
                        continue;
                    if (msg->wParam == VK_LEFT && i > 0) {
                        msg->message = WM_NULL;
                        RequestFocusColumn(i - 1);
                    } else if (msg->wParam == VK_RIGHT) {
                        if (NativePointerInteractionActive()) {
                            msg->message = WM_NULL;
                            QueueSelectionUpdate(Context().panes[i]);
                            break;
                        }
                        ULONG_PTR generation = Context().panes[i].generation;
                        SelectedItem selected = SelectedPath(Context().panes[i]);
                        if (Context().closed || i >= Context().columnCount ||
                            Context().panes[i].generation != generation) {
                            msg->message = WM_NULL;
                            break;
                        }
                        if (NativePointerInteractionActive()) {
                            msg->message = WM_NULL;
                            QueueSelectionUpdate(Context().panes[i]);
                            break;
                        }
                        if (selected.folder && !selected.path.empty()) {
                            if (i + 1 >= Context().columnCount ||
                                Context().panes[i + 1].path != selected.path) {
                                Context().columnSelection[i] = selected.path;
                                RemoveColumnsAfter(i);
                                AppendColumn(selected.path, selected.identity.get());
                                ShowPreview(selected.path);
                            }
                            if (i + 1 < Context().columnCount) {
                                msg->message = WM_NULL;
                                RequestFocusColumn(i + 1);
                            }
                        }
                    }
                    break;
                }
            }
            if (msg->message != WM_NULL && TranslateColumnAccelerator(msg))
                msg->message = WM_NULL; // S_OK means the column has already handled it.
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

void InitializeModeStore() {
    std::wstring gateName = kColumnsModeName;
    gateName += L".Initialize";
    HANDLE gate = CreateMutexW(nullptr, FALSE, gateName.c_str());
    if (!gate) { LogWin32(L"CreateMutex(columns mode)"); return; }
    DWORD wait = WaitForSingleObject(gate, 1000);
    if (wait != WAIT_OBJECT_0 && wait != WAIT_ABANDONED) {
        Wh_Log(L"[Columns] Shared mode initialization unavailable: %u", wait);
        CloseHandle(gate);
        return;
    }
    g_modeMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
        0, sizeof(SharedColumnsMode), kColumnsModeName);
    if (g_modeMapping) {
        g_sharedMode = static_cast<SharedColumnsMode*>(MapViewOfFile(g_modeMapping,
            FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(SharedColumnsMode)));
        if (!g_sharedMode) {
            LogWin32(L"MapViewOfFile(columns mode)");
            CloseHandle(g_modeMapping);
            g_modeMapping = nullptr;
        } else if (InterlockedCompareExchange(&g_sharedMode->magic, 0, 0) != kSharedModeMagic) {
            InterlockedExchange(&g_sharedMode->enabled, g_enableByDefault.load() ? 1 : 0);
            InterlockedExchange(&g_sharedMode->magic, kSharedModeMagic);
        }
    } else LogWin32(L"CreateFileMapping(columns mode)");
    ReleaseMutex(gate);
    CloseHandle(gate);
}

void CloseModeStore() {
    if (g_sharedMode) UnmapViewOfFile(g_sharedMode);
    g_sharedMode = nullptr;
    if (g_modeMapping) CloseHandle(g_modeMapping);
    g_modeMapping = nullptr;
}

void RefreshLocalColumnsMode(bool enabled) {
    if (g_columnsEnabled.exchange(enabled) != enabled) {
        Wh_Log(L"[Columns] Global mode %s", enabled ? L"enabled" : L"disabled");
    }
}

void ApplyColumnsMode(bool enabled) {
    if (g_sharedMode) {
        InterlockedExchange(&g_sharedMode->enabled, enabled ? 1 : 0);
        // Each Explorer helper has a hidden top-level observer window. Find
        // every process's observer by class; receivers reread the mapping.
        for (HWND observer = nullptr;
             (observer = FindWindowExW(nullptr, observer, kThemeObserverClass, nullptr));) {
            if (!PostMessageW(observer, g_modeChangedMessage, 0, 0))
                LogWin32(L"PostMessage(columns mode observer)");
        }
    }
    RefreshLocalColumnsMode(enabled);
}

void SynchronizeColumnsMode() {
    if (g_preferenceChangePending.exchange(false)) ApplyColumnsMode(g_enableByDefault.load());
    if (g_sharedMode) {
        RefreshLocalColumnsMode(InterlockedCompareExchange(&g_sharedMode->enabled, 0, 0) != 0);
    }
}

bool EnsureDispatchThread(HWND window);

std::vector<TabTarget> DiscoverExplorerTabs() {
    std::vector<TabTarget> result;
    EnumWindows([](HWND root, LPARAM value) -> BOOL {
        if (!IsExplorerWindow(root)) return TRUE;
        EnsureDispatchThread(root);
        struct Search { HWND root; std::vector<TabTarget>* result; } search{
            root, reinterpret_cast<std::vector<TabTarget>*>(value)};
        EnumChildWindows(root, [](HWND child, LPARAM data) -> BOOL {
            auto* search = reinterpret_cast<Search*>(data);
            wchar_t name[80] = {};
            if (GetClassNameW(child, name, ARRAYSIZE(name)) &&
                !wcscmp(name, L"ShellTabWindowClass"))
                search->result->push_back({search->root, child});
            return TRUE;
        }, reinterpret_cast<LPARAM>(&search));
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

void PruneDispatchTargets() {
    g_dispatchHooks.erase(std::remove_if(g_dispatchHooks.begin(), g_dispatchHooks.end(),
        [](const DispatchTarget& entry) {
            if (WaitForSingleObject(entry.ownerThread, 0) != WAIT_OBJECT_0) return false;
            UnhookWindowsHookEx(entry.hook);
            CloseHandle(entry.ownerThread);
            return true;
        }), g_dispatchHooks.end());
    g_tabCommands.erase(std::remove_if(g_tabCommands.begin(), g_tabCommands.end(),
        [](const TabCommand& entry) { return !IsWindow(entry.tab); }), g_tabCommands.end());
    g_knownTabs.erase(std::remove_if(g_knownTabs.begin(), g_knownTabs.end(),
        [](const TabTarget& entry) { return !IsWindow(entry.tab); }), g_knownTabs.end());
}

bool EnsureDispatchThread(HWND window) {
    DWORD pid = 0;
    DWORD thread = GetWindowThreadProcessId(window, &pid);
    if (!thread || pid != GetCurrentProcessId()) return false;
    auto hook = std::find_if(g_dispatchHooks.begin(), g_dispatchHooks.end(),
        [thread](const DispatchTarget& entry) { return entry.thread == thread; });
    if (hook == g_dispatchHooks.end()) {
        HANDLE ownerThread = OpenThread(SYNCHRONIZE, FALSE, thread);
        if (!ownerThread) { LogWin32(L"OpenThread(columns dispatch)"); return false; }
        HHOOK installed = SetWindowsHookExW(WH_GETMESSAGE, DispatchHookProc, nullptr, thread);
        if (!installed) {
            LogWin32(L"SetWindowsHookEx(WH_GETMESSAGE)");
            CloseHandle(ownerThread);
            return false;
        }
        g_dispatchHooks.push_back({thread, installed, ownerThread});
    }
    return true;
}

void QueueTabCommand(HWND explorer, HWND tab, LPARAM action) {
    if (g_unloading.load() || !IsExplorerWindow(explorer) ||
        GetAncestor(tab, GA_ROOT) != explorer || !EnsureDispatchThread(tab)) return;
    auto pending = std::find_if(g_tabCommands.begin(), g_tabCommands.end(),
        [tab](const TabCommand& entry) { return entry.tab == tab; });
    if (pending != g_tabCommands.end() && pending->pending) return;
    if (PostMessageW(tab, g_toggleMessage, reinterpret_cast<WPARAM>(explorer), action)) {
        if (pending == g_tabCommands.end())
            g_tabCommands.push_back({tab, true, action});
        else *pending = {tab, true, action};
    } else LogWin32(L"PostMessage(columns command)");
}

void RememberExplorerTab(HWND explorer, HWND tab) {
    if (!IsExplorerWindow(explorer) || GetAncestor(tab, GA_ROOT) != explorer) return;
    // The toggle must work before any columns have been opened, including
    // when EnableByDefault is off. Also cover the frame's own input thread.
    EnsureDispatchThread(explorer);
    EnsureDispatchThread(tab);
    auto known = std::find_if(g_knownTabs.begin(), g_knownTabs.end(),
        [tab](const TabTarget& entry) { return entry.tab == tab; });
    if (known == g_knownTabs.end()) g_knownTabs.push_back({explorer, tab});
    else known->explorer = explorer;
}

void ProcessExplorerWindowEvents() {
    std::vector<HWND> windows;
    AcquireSRWLockExclusive(&g_windowEventsLock);
    windows.swap(g_windowEvents);
    g_windowEventPosted = false;
    ReleaseSRWLockExclusive(&g_windowEventsLock);
    PruneDispatchTargets();
    for (HWND window : windows) {
        if (!IsWindow(window) || SessionForWindow(window)) continue;
        HWND explorer = GetAncestor(window, GA_ROOT);
        if (!IsExplorerWindow(explorer)) continue;
        EnsureDispatchThread(explorer);
        EnsureDispatchThread(window);
        HWND tab = window;
        for (; tab && tab != explorer; tab = GetParent(tab)) {
            wchar_t name[80] = {};
            if (GetClassNameW(tab, name, ARRAYSIZE(name)) &&
                !wcscmp(name, L"ShellTabWindowClass")) break;
        }
        if (!tab || tab == explorer) continue;
        RememberExplorerTab(explorer, tab);
        if (auto owner = SessionForTab(tab)) {
            // Only native structure creation reaches here; custom column
            // descendants were excluded by the CreateWindowExW hook.
            PostMessageW(owner->overlay, kNativeStructureMessage, owner->identity, 0);
        } else if (g_columnsEnabled.load())
            QueueTabCommand(explorer, tab, kOpenColumnsCommand);
    }
}

void ReconcileColumnsMode() {
    if (g_unloading.load()) return;
    SynchronizeColumnsMode();
    PruneDispatchTargets();
    bool enabled = g_columnsEnabled.load();
    for (auto& target : g_knownTabs) {
        target.explorer = GetAncestor(target.tab, GA_ROOT);
        if (!IsExplorerWindow(target.explorer)) continue;
        EnsureDispatchThread(target.explorer);
        EnsureDispatchThread(target.tab);
        bool open = SessionForTab(target.tab) != nullptr;
        if (open != enabled)
            QueueTabCommand(target.explorer, target.tab,
                enabled ? kOpenColumnsCommand : kCloseColumnsCommand);
    }
}

LRESULT CALLBACK ThemeObserverProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    CallbackScope callbackScope;
    if (!g_unloading.load() && message == g_modeChangedMessage) {
        PostThreadMessageW(g_threadId, kReconcileModeMessage, 0, 0);
        return 0;
    }
    if (!g_unloading.load() && (message == WM_SETTINGCHANGE ||
        message == WM_SYSCOLORCHANGE || message == WM_THEMECHANGED)) {
        // This hidden top-level window receives broadcasts that child overlays
        // can miss. Queue identity-only notices; do all COM/painting on each
        // session's owning UI thread after the native theme transition settles.
        AcquireSRWLockShared(&g_sessionsLock);
        for (const auto& entry : g_sessions)
            PostMessageW(entry.overlay, kPaletteRefreshMessage, entry.owner->identity, 0);
        ReleaseSRWLockShared(&g_sessionsLock);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wp, lp);
}

DWORD WINAPI UiThread(void*) {
    g_threadId.store(GetCurrentThreadId());
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        LogHr(L"CoInitializeEx", hr);
        g_unloading.store(true);
        return 1;
    }
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
    if (g_unloading.load()) {
        CoUninitialize();
        return 0;
    }
    g_toggleMessage = RegisterWindowMessageW(
        L"Windhawk.NativeColumns.Toggle.7CCDD1D4-57CA-47D5-AE3B-67A6A15AC0C2");
    g_modeChangedMessage = RegisterWindowMessageW(
        L"Windhawk.NativeColumns.ModeChanged.V039");
    if (!g_toggleMessage || !g_modeChangedMessage) {
        LogWin32(L"RegisterWindowMessage(toggle)");
        g_unloading.store(true);
        CoUninitialize();
        return 1;
    }
    const wchar_t* classes[] = {kOverlayClass, kPaneClass,
                                kViewHostClass, kSplitterClass,
                                kPreviewClass, kVideoClass, kThemeObserverClass};
    WNDPROC procs[] = {OverlayProc, PaneProc, ViewHostProc,
                       SplitterProc, PreviewSlotProc, VideoPreviewProc, ThemeObserverProc};
    constexpr int classCount = ARRAYSIZE(classes);
    int registered = 0;
    for (int i = 0; i < classCount; ++i) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = procs[i];
        wc.hInstance = HINST_THISCOMPONENT;
        wc.lpszClassName = classes[i];
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        if (!RegisterClassW(&wc)) {
            Wh_Log(L"[Columns] RegisterClass failed for %s", classes[i]);
            LogWin32(L"RegisterClass");
            break;
        }
        ++registered;
    }
    if (registered != classCount) {
        g_unloading.store(true);
        for (int i = registered - 1; i >= 0; --i)
            UnregisterClassW(classes[i], HINST_THISCOMPONENT);
        CoUninitialize();
        return 1;
    }
    HWND themeObserver = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kThemeObserverClass, L"", WS_POPUP, 0, 0, 0, 0,
        nullptr, nullptr, HINST_THISCOMPONENT, nullptr);
    if (!themeObserver) {
        LogWin32(L"CreateWindowEx(theme observer)");
        g_unloading.store(true);
        for (int i = classCount - 1; i >= 0; --i)
            UnregisterClassW(classes[i], HINST_THISCOMPONENT);
        CoUninitialize();
        return 1;
    }
    // Create the observer before reading shared state: a concurrent change is
    // then represented either by that read or by a queued notification.
    InitializeModeStore();
    SynchronizeColumnsMode();
    g_helperQueueReady.store(true);
    Wh_Log(L"[Columns] Ready v%s: mode=%s; configured shortcut in Explorer toggles columns",
           WH_MOD_VERSION, g_columnsEnabled.load() ? L"on" : L"off");
    // The only full enumeration. Later tabs/native views arrive through the
    // CreateWindowExW hook, registered during Wh_ModInit before hooks activate.
    for (const auto& target : DiscoverExplorerTabs())
        RememberExplorerTab(target.explorer, target.tab);
    ProcessExplorerWindowEvents();
    ReconcileColumnsMode();
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (g_unloading.load()) break;
        if (msg.message == kReconcileModeMessage) {
            ReapPreviewJobs();
            ReconcileColumnsMode();
        } else if (msg.message == kExplorerWindowMessage) {
            ReapPreviewJobs();
            ProcessExplorerWindowEvents();
        } else if (msg.message == kToggleColumnsMessage) {
            SynchronizeColumnsMode();
            ApplyColumnsMode(!g_columnsEnabled.load());
            ReconcileColumnsMode();
        } else if (msg.message == kCommandDoneMessage) {
            HWND tab = reinterpret_cast<HWND>(msg.wParam);
            auto pending = std::find_if(g_tabCommands.begin(), g_tabCommands.end(),
                [tab](const TabCommand& entry) { return entry.tab == tab; });
            if (pending != g_tabCommands.end() && pending->action == msg.lParam) {
                pending->pending = false;
                // A mode change can race a queued UI command. Reconcile only
                // that race; a failed open waits for a native window event.
                if ((msg.lParam == kOpenColumnsCommand) != g_columnsEnabled.load())
                    ReconcileColumnsMode();
            }
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    g_helperQueueReady.store(false);
    if (themeObserver) DestroyWindow(themeObserver);
    for (const auto& entry : g_dispatchHooks) {
        UnhookWindowsHookEx(entry.hook);
        CloseHandle(entry.ownerThread);
    }
    g_dispatchHooks.clear();
    std::vector<DispatchTarget>().swap(g_dispatchHooks);
    std::vector<TabCommand>().swap(g_tabCommands);
    std::vector<TabTarget>().swap(g_knownTabs);
    AcquireSRWLockExclusive(&g_windowEventsLock);
    std::vector<HWND>().swap(g_windowEvents);
    g_windowEventPosted = false;
    ReleaseSRWLockExclusive(&g_windowEventsLock);
    CloseModeStore();
    for (;;) {
        if (g_creatingSessions.load()) { Sleep(1); continue; }
        auto windows = SessionWindows();
        for (HWND overlay : windows) {
            if (IsWindow(overlay)) SendMessageW(overlay, WM_CLOSE, 0, 0);
            else ForgetSession(overlay);
        }
        bool workersPending = ReapPreviewJobs();
        if (SessionWindows().empty() && !workersPending &&
            g_activeCallbacks.load(std::memory_order_acquire) == 0) break;
        Sleep(1);
    }
    AcquireSRWLockExclusive(&g_sessionsLock);
    std::vector<SessionRecord>().swap(g_sessions);
    ReleaseSRWLockExclusive(&g_sessionsLock);
    for (int i = classCount - 1; i >= 0; --i) {
        if (!UnregisterClassW(classes[i], HINST_THISCOMPONENT)) {
            Wh_Log(L"[Columns] UnregisterClass failed for %s: %u",
                   classes[i], GetLastError());
        }
    }
    CoUninitialize();
    return 0;
}
void DrainComObjectsAndCallbacks(DWORD warningAfter = 5000) {
    const ULONGLONG started = GetTickCount64();
    auto logState = [started](PCWSTR state) {
        Wh_Log(L"[Columns unload] %s after %llu ms: COM=%u "
               L"PaneBrowserSite=%u NavigationSink=%u PaneNavigationSink=%u "
               L"MediaEngineNotify=%u callbacks=%u",
            state, GetTickCount64() - started,
            g_liveComObjects.load(std::memory_order_acquire),
            g_livePaneBrowserSites.load(std::memory_order_acquire),
            g_liveNavigationSinks.load(std::memory_order_acquire),
            g_livePaneNavigationSinks.load(std::memory_order_acquire),
            g_liveMediaEngineNotifies.load(std::memory_order_acquire),
            g_activeCallbacks.load(std::memory_order_acquire));
    };
    logState(L"Drain started");
    ULONGLONG nextWarning = started + warningAfter;
    while (g_liveComObjects.load(std::memory_order_acquire) != 0 ||
           g_activeCallbacks.load(std::memory_order_acquire) != 0) {
        if (GetTickCount64() >= nextWarning) {
            logState(L"Still waiting for owners to release callable objects");
            // A diagnostic timeout cannot authorize unloading callable vtables.
            nextWarning = GetTickCount64() + 30000;
        }
        Sleep(1);
    }
    logState(L"Drain completed");
}
}  // namespace

BOOL Wh_ModInit() {
    g_unloading.store(false);
    g_helperQueueReady.store(false);
    g_threadId.store(0);
    g_preferenceChangePending.store(false);
    g_enableByDefault.store(Wh_GetIntSetting(L"EnableByDefault") != 0);
    g_columnsEnabled.store(g_enableByDefault.load());
    g_fixedInspectorSetting.store(Wh_GetIntSetting(L"FixedInspector") != 0);
    g_classicContextMenus.store(Wh_GetIntSetting(L"ClassicContextMenus") != 0);
    g_dividerWidthSetting.store(ReadDividerWidthSetting());
    g_delayedPaintRepairs.store(Wh_GetIntSetting(L"DelayedPaintRepairs") != 0);
    g_inlineMediaPlayback.store(Wh_GetIntSetting(L"InlineMediaPlayback") != 0);
    LoadToggleShortcut();
    Wh_Log(L"[Columns settings] Loaded v%s default=%d fixedInspector=%d classicMenus=%d",
        WH_MOD_VERSION, g_enableByDefault.load(), g_fixedInspectorSetting.load(), g_classicContextMenus.load());
    const auto& platform = GetMenuPlatform();
    Wh_Log(L"[Columns platform] Windows=%u.%u.%u.%u arch=%u knownV2Family=%d",
        platform.major, platform.minor, platform.build, platform.revision,
        static_cast<unsigned>(platform.architecture), IsSupportedMenuPlatform());
    // Windhawk applies these hooks after ModInit returns. Discovery must be
    // registered here, while its queue/one initial enumeration start AfterInit.
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExWHook,
            &g_createWindowExWOriginal)) {
        return FALSE;
    }
    if (!RegisterColumnBrowseHook())
        Wh_Log(L"[Columns new tab] Optional redirect unavailable; continuing without it");
    return TRUE;
}

void Wh_ModAfterInit() {
    // Start the event/discovery coordinator after function hooks are active.
    g_thread = CreateThread(nullptr, 0, UiThread, nullptr, 0, nullptr);
    if (!g_thread) {
        LogWin32(L"CreateThread(columns coordinator)");
        g_unloading.store(true);
    }
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true);
    if (g_thread) PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
}

void Wh_ModSettingsChanged() {
    bool preference = Wh_GetIntSetting(L"EnableByDefault") != 0;
    g_fixedInspectorSetting.store(Wh_GetIntSetting(L"FixedInspector") != 0);
    g_classicContextMenus.store(Wh_GetIntSetting(L"ClassicContextMenus") != 0);
    g_dividerWidthSetting.store(ReadDividerWidthSetting());
    g_delayedPaintRepairs.store(Wh_GetIntSetting(L"DelayedPaintRepairs") != 0);
    g_inlineMediaPlayback.store(Wh_GetIntSetting(L"InlineMediaPlayback") != 0);
    LoadToggleShortcut();
    // Publish the layout/menu preferences before the helper can open new tabs.
    if (g_enableByDefault.exchange(preference) != preference) {
        g_preferenceChangePending.store(true);
        if (g_threadId) PostThreadMessageW(g_threadId, kReconcileModeMessage, 0, 0);
    }
    Wh_Log(L"[Columns settings] Saved v%s default=%d fixedInspector=%d classicMenus=%d modeChange=%d",
        WH_MOD_VERSION, preference, g_fixedInspectorSetting.load(), g_classicContextMenus.load(),
        g_preferenceChangePending.load());
    if (!g_unloading.load())
        for (HWND overlay : SessionWindows())
            if (IsWindow(overlay)) PostMessageW(overlay, kLayoutSettingsChangedMessage, 0, 0);
}

void Wh_ModUninit() {
    const ULONGLONG started = GetTickCount64();
    Wh_Log(L"[Columns unload] Uninit started");
    g_unloading.store(true);
    if (g_thread) {
        // An immediate disable can arrive before the helper creates its queue.
        // Retry until that queue exists or the helper has already exited.
        while (WaitForSingleObject(g_thread, 0) == WAIT_TIMEOUT) {
            PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
            WaitForSingleObject(g_thread, 50);
        }
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    // Also cover any registered worker whose entry point had not begun when
    // shutdown started. Never unload this code while a worker can still run it.
    while (ReapPreviewJobs()) Sleep(1);
    // A closed tab does not imply that Windows has dropped its last site,
    // event-sink or Media Foundation callback reference. In particular,
    // Release must remain callable after shutdown returns on another thread.
    DrainComObjectsAndCallbacks();
    AcquireSRWLockExclusive(&g_previewJobsLock);
    std::vector<std::shared_ptr<PreviewJob>>().swap(g_previewJobs);
    ReleaseSRWLockExclusive(&g_previewJobsLock);
    // Windhawk disables function hooks before Wh_ModUninit. All callbacks
    // have drained above, so their implementation module can now be released.
    if (g_columnBrowseModule) { FreeLibrary(g_columnBrowseModule); g_columnBrowseModule = nullptr; }
    Wh_Log(L"[Columns unload] Uninit completed after %llu ms", GetTickCount64() - started);
}



