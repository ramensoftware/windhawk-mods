// ==WindhawkMod==
// @id              snap-sentry
// @name            SnapSentry
// @description     Watch your Screenshots folder or any folder you pick, then copy, rename, or delete each new screenshot, or choose from a notification.
// @version         0.22.0
// @author          mario0318
// @github          https://github.com/mario0318
// @include         windhawk.exe
// @license         GPL-3.0
// @compilerOptions -lole32 -lshell32 -lcomctl32 -lwindowscodecs -lruntimeobject -ladvapi32 -lbcrypt -luuid -lgdi32
// ==/WindhawkMod==

// Source code is published under the GNU General Public License v3.0.
// https://github.com/mario0318/SnapSentry
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/mario0318/SnapSentry/issues

// ==WindhawkModReadme==
/*
# SnapSentry

Watches your Windows **Screenshots** folder, wherever it is, or any other folder
you point it at, and handles new screenshots as they are saved. Copy the image,
rename the file, delete it after a delay, or choose what to do from a
notification.

![The SnapSentry notification](https://raw.githubusercontent.com/mario0318/SnapSentry/9f28c82bb5b901c399d6773ea8a2e09195bc3f1a/assets/notification-rename.png)

## Clipboard modes

* **Image** copies the picture so it stays pasteable even after the file is deleted.
  For a multi-page or animated image only the first frame is copied, so the file is
  kept rather than deleted.
* **File** copies the file for pasting into File Explorer. Deletion is disabled.
* **Path** copies the full path as text. You can pick plain text, a quoted path, a
  file link, or a Markdown image link, which is handy for pasting a screenshot
  straight into notes or a bug report. Deletion is disabled.
* **None** leaves the clipboard unchanged.

![A Markdown image link pasted into a note](https://raw.githubusercontent.com/mario0318/SnapSentry/70157e72808d7788583e09480e4011e95e8e8868/assets/demo-markdown.png)

## Naming

You can have SnapSentry rename each new screenshot from the window that was in
front when the new file was observed, together with a timestamp, so a file ends up named for
what it shows instead of Screenshot (1). The file stays in the same folder.

![Screenshots renamed after the window they were taken from](https://raw.githubusercontent.com/mario0318/SnapSentry/70157e72808d7788583e09480e4011e95e8e8868/assets/demo-rename.png)

## Identical recent screenshots

When **Remove identical recent screenshots** is enabled, SnapSentry compares a
new image with images handled recently by this running instance, within a short
window of about ten minutes. It works only with the **Image** clipboard mode,
because that is the mode that makes a durable copy before cleanup. An exact
byte-for-byte match is always sent to the Recycle Bin on the automatic path,
even when **Delete the screenshot after copying** is off or ordinary deletion
is set to permanent. If recycling fails, the duplicate stays in place. The older
copy must still contain the same bytes when cleanup runs. If normal deletion is
on, it usually removes each earlier copy before a later identical image arrives,
so there may be no keeper for duplicate cleanup to find. The comparison is
limited to 64 recent entries, resets when settings change or folder watching
restarts, and does not scan or touch files that were already in the folder when
watching began. Images with different embedded metadata are kept even if they
look the same.

![An identical screenshot in the Recycle Bin after it was removed](https://raw.githubusercontent.com/mario0318/SnapSentry/70157e72808d7788583e09480e4011e95e8e8868/assets/demo-duplicate.png)

## Which folder it watches

By default this is your Windows Screenshots folder, wherever Windows keeps it. In
the settings you can type the full path to any folder instead, and shortcuts like
%USERPROFILE% are filled in for you. Every new image that arrives in the folder you
choose is treated the same way, so pick one where screenshots land rather than one
that collects downloads. The image types treated as new screenshots are .png, .jpg,
.jpeg, .jfif, .bmp, .gif, .webp, .tif, and .tiff; with deletion on, only freshly
created files of these types are ever removed, so it is worth knowing when you pick a
folder.

## The notification

The popup is a real Windows notification, so it matches your light or dark theme.
While the popup is turned on, SnapSentry leaves two things on your machine so the
notification buttons work, a SnapSentry shortcut in your Start Menu programs
folder and one registry entry. Turning the popup off or disabling the mod removes
both again. If the notification can't be shown, SnapSentry falls back
to a standard dialog. If you have turned its notifications off, it takes
that as a cue to stay quiet: it still copies to the clipboard but shows no dialog
and never auto deletes. Windows forgets that choice when SnapSentry is disabled,
updated or turned off, because its notification entry is removed then, so turn
its notifications off again in Windows settings if you want it to stay quiet.
A multi-page or animated image is kept rather than
deleted, because only its first frame can go on the clipboard, and afterwards a
short notice says so, when the notification is available.

## Capture sessions

Turn on **Capture sessions** to add a tray icon. Its menu or **Ctrl+Alt+F8** (you
can pick another F key in the settings) starts
a session. Take screenshots as usual, then finish the session to review them.
Check the images to include, edit captions, and use **Move up** or **Move down**
to order them. The contact sheet preview follows that order.

![Reviewing a capture session before exporting it](https://raw.githubusercontent.com/mario0318/SnapSentry/6fd426c877bbb3f0a806dc97bb092aaff18b2a8f/assets/demo-session.png)

**Export packet** asks for a folder outside the watched folder and creates a new
packet there: numbered copies of the included screenshots, a contact sheet, and
a Markdown page that links to them. Export never removes the originals, and
leaving an image unchecked only leaves it out of the packet. A screenshot that
was edited or replaced after it was taken is not exported.

Closing the review stops recording but keeps the list, so the shortcut reopens
it. Use **Start session** or the tray menu to begin a new one. A session holds up
to 64 screenshots; any taken after that stay on disk, outside the session. A
packet can hold up to 1 GB of screenshots. The list is forgotten when SnapSentry
stops, but the screenshots are not.

Only ordinary image files saved directly in the watched folder are handled.
Very large images are left alone. The window name used for renaming is the one
in front when the file appears.

Before deleting anything, SnapSentry checks that the file is still the one it
copied. If it changed during the countdown, it is kept. If it can't be moved
to the Recycle Bin, it is kept too.

## Privacy

SnapSentry treats any supported image written into the watched folder in about the
last half minute as a new screenshot. Files that were already there, and copies of
older images dragged in or synced from another device, are left alone. A brand new
file saved or downloaded straight into the folder cannot be told apart from a
capture, so avoid pointing the folder at a place where downloads land. By default a
deleted screenshot goes to the Recycle Bin so it can be restored; if you turn that
off, deletion is permanent. Deleting a screenshot does not remove copies already
stored in clipboard history, cloud sync, backups, or other programs. Duplicate
detection keeps only short-lived hashes, not image data.

## Issues

Bugs and feature requests go on the [SnapSentry issues
page](https://github.com/mario0318/SnapSentry/issues).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# ---- What to copy ----
- clipboardMode: image
  $name: What to copy to the clipboard
  $options:
  - image: Image, the picture itself (best if you also delete)
  - file: File, for pasting into File Explorer (never deleted)
  - path: Path, the file's location as text (never deleted)
  - none: None, leave the clipboard alone
- pathFormat: plain
  $name: Path format
  $description: Only used when What to copy is Path. File link is clickable, and Markdown shows the picture when pasted into notes or a bug report.
  $options:
  - plain: Plain path
  - quoted: Quoted path
  - uri: File link
  - markdown: Markdown image

# ---- Deleting the screenshot ----
- deleteFile: false
  $name: Delete the screenshot after copying
  $description: Off by default, since deleting is the part you cannot undo. Removes the file once the copy has succeeded. Only applies when copying the picture or nothing, because copying the file or its location has to leave it in place. When copying the picture, a multi-frame image, such as a multi-page TIFF or animated GIF, is kept rather than deleted, since only its first frame can go on the clipboard.
- recycle: true
  $name: Delete to the Recycle Bin
  $description: Ordinary automatic deletion and the popup buttons send files to the Recycle Bin so you can get them back. Turn this off for permanent deletion. Identical recent screenshots always use the Recycle Bin regardless of this choice.
- removeExactDuplicates: false
  $name: Remove identical recent screenshots
  $description: In Image clipboard mode, recycles an exact byte-for-byte repeat among up to 64 images handled in the last ten minutes, only on the automatic action and after its copy succeeds. Duplicates always go to the Recycle Bin, even if ordinary deletion is set to permanent or Delete the screenshot after copying is off. A failed recycle keeps the file. The earlier copy must still have identical contents when cleanup runs. Normal auto-delete usually removes earlier copies before a duplicate arrives. The comparison resets on settings changes or watcher restarts and never scans older files.

# ---- The popup ----
- showActionPopup: false
  $name: Show a popup with buttons after each screenshot
  $description: Each screenshot brings up a notification with Delete, Copy and delete, and Keep buttons, or a plain dialog if the notification cannot be set up on this machine. If you have turned SnapSentry's notifications off in Windows, it stays quiet instead, still copying but never prompting and never deleting. A multi-page or animated image is copied and kept rather than deleted, since only its first frame can go on the clipboard, and afterwards a short notice says so when the notification is available. This is the only setting that leaves anything outside the mod, namely a Start Menu entry and a registry entry so the buttons work, both removed when you turn it off or disable the mod. Left off, SnapSentry copies and leaves nothing behind.
- delaySeconds: 5
  $name: Seconds to wait before the automatic action
  $description: The countdown before the automatic action runs. With the popup on, 0 waits for you to click instead, giving up after 10 minutes so it cannot wait forever. With the popup off, 0 acts as soon as the copy is done. A countdown no longer holds up the next screenshot, which is copied straight away. Popups appear one at a time. Maximum 3600.

# ---- Renaming ----
- renameFromWindow: false
  $name: Rename screenshots after the active window
  $description: Names each screenshot after the window that was in front when the file appeared, plus a timestamp, so you can find it later without opening it. The file stays in the same folder.
- captureSessions: false
  $name: Capture sessions
  $description: Adds a SnapSentry tray icon. Start a session, take screenshots, then review their order and captions and export a contact sheet and Markdown packet. Session screenshots are always kept, including when ordinary deletion or duplicate removal is enabled. Sessions are limited to 64 images. Disabling the mod discards only the review list, never the images.
- sessionHotkey: F8
  $name: Session hotkey
  $options:
  - F1: Ctrl+Alt+F1
  - F2: Ctrl+Alt+F2
  - F3: Ctrl+Alt+F3
  - F4: Ctrl+Alt+F4
  - F5: Ctrl+Alt+F5
  - F6: Ctrl+Alt+F6
  - F7: Ctrl+Alt+F7
  - F8: Ctrl+Alt+F8
  - F9: Ctrl+Alt+F9
  - F10: Ctrl+Alt+F10
  - F11: Ctrl+Alt+F11
  $description: Starts the first session, finishes recording, or reopens the existing review. Use Start session or the tray menu for a new session. The tray menu is available if the shortcut is already in use.

# ---- Advanced ----
- folder: ""
  $name: Folder to watch (leave empty for Screenshots)
  $description: Empty watches your Windows Screenshots folder, wherever it is. To watch somewhere else, paste the full path to that folder here, for example C:\Users\You\Pictures\Captures or %USERPROFILE%\Pictures\ShareX. Anything arriving in the folder you choose is handled the same way, so with deletion on, other images saved there are deleted too.
- logDetails: false
  $name: Verbose logging (may include file paths)
  $description: Off keeps file paths out of the log. Turn this on only while troubleshooting.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <initguid.h>  // Defines the GUIDs pulled in by the headers below in-TU.
#include <knownfolders.h>
#include <shlobj.h>
#include <shellapi.h>  // SHQueryUserNotificationState
#include <propkey.h>   // PKEY_AppUserModel_ID, PKEY_AppUserModel_ToastActivatorCLSID
#include <commctrl.h>
#include <wincodec.h>
#include <bcrypt.h>
#include <roapi.h>
#include <winstring.h>
#include <windows.data.xml.dom.h>
#include <windows.ui.notifications.h>

// The MinGW SDK bundled with Windhawk doesn't ship
// notificationactivationcallback.h, so declare its stable Win32 ABI locally.
struct NOTIFICATION_USER_INPUT_DATA {
    LPCWSTR Key;
    LPCWSTR Value;
};

MIDL_INTERFACE("53E31837-6600-4A81-9395-75CFFE746F94")
INotificationActivationCallback : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Activate(
        LPCWSTR appUserModelId,
        LPCWSTR invokedArgs,
        const NOTIFICATION_USER_INPUT_DATA* data,
        ULONG dataCount) = 0;
};

DEFINE_GUID(IID_INotificationActivationCallback,
            0x53e31837, 0x6600, 0x4a81, 0x93, 0x95, 0x75, 0xcf, 0xfe, 0x74,
            0x6f, 0x94);

#include <windhawk_utils.h>  // WindhawkUtils::StringSetting (RAII settings strings)

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>

#include <atomic>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <deque>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <memory>
#include <cstddef>

// ============================================================================
// Settings and shared state
// ============================================================================

struct Settings {
    int delaySeconds;
    bool deleteFile;
    bool popup;
    std::wstring mode;
    std::wstring pathFormat;
    std::wstring folder;
    bool recycle;
    bool renameFromWindow;
    bool removeExactDuplicates;
    bool logDetails;
    bool sessions;
    int sessionHotkey;
};

static CRITICAL_SECTION g_lock;       // Guards g_settings and watcher/session state below.
static Settings g_settings;
// Bumped only when settings are reloaded, so a prompt on screen is cancelled
// by an actual settings change and never by a watcher restart.
static std::atomic<ULONGLONG> g_generation{0};
struct CaptureRecord {
    std::wstring path;
    std::wstring label;
    FILETIME observed{};
    BY_HANDLE_FILE_INFORMATION identity{};
    bool identified = false;
    ULONGLONG generation = 0;
    ULONGLONG session = 0;
    int action = -1;
    bool prompted = false;
    unsigned retries = 0;
    unsigned deleteRetries = 0;
};

struct NameLess {
    bool operator()(const std::wstring& a, const std::wstring& b) const {
        return CompareStringOrdinal(a.c_str(), -1, b.c_str(), -1, TRUE) == CSTR_LESS_THAN;
    }
};
static constexpr size_t kMaxPendingCaptures = 128;
static constexpr size_t kMaxDecodedBytes = 128 * 1024 * 1024;
static HANDLE g_promptEvent = nullptr;
static HANDLE g_promptThread = nullptr;
static HWND g_clipboardOwner = nullptr;
static ULONGLONG g_clipboardGeneration = 0;
static std::atomic<ULONGLONG> g_activeSession{0};
static std::atomic<unsigned> g_sessionPending{0};
static std::deque<CaptureRecord> g_queue;   // Full paths waiting to be processed.
static std::set<std::wstring, NameLess> g_inflight;  // Names queued or in progress (dedup).

// Names already present when the watched folder was opened. They are never acted
// on, which is the literal safety invariant "never touch files that were already
// in the folder." This is what stops a flood when OneDrive Files On-Demand
// hydrates a folder full of old screenshots: each download surfaces as an ADDED
// or RENAMED event for a name in this set, so it is ignored. Refreshed every time
// the folder is (re)opened.
static std::set<std::wstring, NameLess> g_preexisting;

// Names finished processing recently, each with an expiry tick. Swallows a
// duplicate filesystem event for an already-handled screenshot that arrives
// after the name left g_inflight. The common case is OneDrive Files On-Demand
// rewriting the saved file into a placeholder, which fires a second ADDED or
// RENAMED event for the same name seconds after the first notification closed;
// an antivirus or a screenshot tool's temp-then-rename can do the same. A
// screenshot name is unique per capture, so a same-name event inside the window
// is always such a duplicate, never a distinct new shot.
static std::deque<std::pair<std::wstring, ULONGLONG>> g_recent;
static constexpr ULONGLONG kDuplicateEventWindowMs = 60000;  // Measured from when handling finished.

struct RecentContent {
    std::array<BYTE, 32> hash;
    std::wstring keeper;
    ULONGLONG expires;
    BY_HANDLE_FILE_INFORMATION identity{};
};
static std::deque<RecentContent> g_recentContent;
static constexpr ULONGLONG kContentDuplicateWindowMs = 10 * 60 * 1000;
static constexpr size_t kMaxRecentContent = 64;
// Opened and closed by the worker, which joins the export thread (also a user)
// before closing it. Algorithm handles may be shared between threads.
static BCRYPT_ALG_HANDLE g_hashAlgorithm = nullptr;

// Pre-seed the recent-name set so a file event we cause ourselves (the rename
// below) is swallowed by the watcher instead of being handled as a brand-new
// screenshot, which would rename it again in a loop.
static void MarkNameRecent(const std::wstring& name) {
    ULONGLONG now = GetTickCount64();
    EnterCriticalSection(&g_lock);
    g_recent.push_back({name, now + kDuplicateEventWindowMs});
    while (g_recent.size() > 1024) g_recent.pop_front();
    LeaveCriticalSection(&g_lock);
}

static HANDLE g_stopEvent;   // Manual-reset: set once at shutdown.
static HANDLE g_reloadEvent; // Auto-reset: settings changed, re-open the folder.
static HANDLE g_workEvent;   // Auto-reset: queue has work.
static HANDLE g_settingsEvent;  // Manual reset: settings reloaded while a prompt is up.
static HANDLE g_watchThread;
static HANDLE g_workerThread;
static std::atomic<HWND> g_dialog{nullptr};  // Open action dialog, for shutdown.

// A popup with no countdown (delaySeconds 0) waits for an answer, but not
// forever: popups are shown one at a time on the prompt thread, so an
// unanswered one holds back every later popup. Running out is treated as Keep,
// since nobody chose to delete anything.
static constexpr DWORD kNoAnswerCapMs = 10 * 60 * 1000;

enum {
    ACTION_AUTO = 100,
    ACTION_DELETE = 101,
    ACTION_COPY_DELETE = 102,
    ACTION_KEEP = 103,
    ACTION_COPY_ONLY = 104,  // Couldn't ask the user: do the configured copy, never delete.
};

// Audit outcomes describe the final state, not an attempted operation. In
// particular, a failed recycle is Kept, so the audit never claims a destructive
// action that did not actually happen. Deleted is separate because the user can
// explicitly disable Recycle Bin use.
enum class AuditOutcome {
    Copied,
    Kept,
    Recycled,
    RecycledDuplicate,
    Skipped,
    Deleted,
};

static CRITICAL_SECTION g_toastLock;   // Guards g_toastAction, g_toastAnswered, g_activeToastId.
static int g_toastAction = ACTION_AUTO;
// First answer wins. A button click and a dismissal can both arrive for the same
// toast (clicking a button dismisses it), and the click is the real answer, so a
// later dismissal must not overwrite it with Keep.
static bool g_toastAnswered = false;
static ULONGLONG g_activeToastId;
// The toast currently being waited on. A Dismissed event for an earlier toast can
// be pumped after we've moved on to the next one, so the handler matches on this
// pointer before treating a dismissal as the current toast's answer.
static std::atomic<void*> g_activeToast{nullptr};
static HANDLE g_toastActionEvent;      // Auto-reset: a toast was activated or dismissed.
static std::atomic<bool> g_toastRegistered{false};  // AUMID+CLSID registration ok.

// ============================================================================
// Settings
// ============================================================================

// FOLDERID_Screenshots ({B7BEDE81-DF94-4682-A7D8-57A52620B86F}). Some SDK headers
// gate the constant behind NTDDI_VERSION, so define it locally. Resolving it
// directly follows the folder if the user has relocated it (Properties ->
// Location, some OneDrive setups), which Pictures + "\Screenshots" would miss.
static const GUID kFolderIdScreenshots = {
    0xb7bede81, 0xdf94, 0x4682, {0xa7, 0xd8, 0x57, 0xa5, 0x26, 0x20, 0xb8, 0x6f}};

static std::wstring DefaultScreenshotsFolder() {
    std::wstring result;
    PWSTR path = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(kFolderIdScreenshots, 0, nullptr, &path))) {
        result.assign(path);
        CoTaskMemFree(path);
        return result;
    }
    // Fallback for systems without the Screenshots known folder.
    PWSTR pictures = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Pictures, 0, nullptr, &pictures))) {
        result.assign(pictures);
        result += L"\\Screenshots";
        CoTaskMemFree(pictures);
    }
    return result;
}

static void LoadSettings() {
    Settings s{};
    s.delaySeconds = Wh_GetIntSetting(L"delaySeconds");
    if (s.delaySeconds < 0) {
        s.delaySeconds = 0;
    }
    if (s.delaySeconds > 3600) {
        s.delaySeconds = 3600;  // Cap so delaySeconds * 1000 can't overflow a DWORD.
    }
    s.deleteFile = Wh_GetIntSetting(L"deleteFile") != 0;
    s.popup = Wh_GetIntSetting(L"showActionPopup") != 0;
    s.recycle = Wh_GetIntSetting(L"recycle") != 0;
    s.renameFromWindow = Wh_GetIntSetting(L"renameFromWindow") != 0;
    s.removeExactDuplicates = Wh_GetIntSetting(L"removeExactDuplicates") != 0;
    // Kept as its own toggle so file paths can be withheld from a log the user
    // might share; Windhawk's own logging switch is all-or-nothing.
    s.logDetails = Wh_GetIntSetting(L"logDetails") != 0;
    s.sessions = Wh_GetIntSetting(L"captureSessions") != 0;
    // F1 to F11 map to consecutive key codes. F12 is left out because Windows
    // reserves it for the debugger. Anything else falls back to F8.
    {
        auto key = WindhawkUtils::StringSetting::make(L"sessionHotkey");
        PCWSTR text = key.get();
        int number = 0;
        if ((text[0] == L'F' || text[0] == L'f') && text[1]) {
            number = _wtoi(text + 1);
        }
        s.sessionHotkey =
            number >= 1 && number <= 11 ? VK_F1 + number - 1 : VK_F8;
    }

    s.mode = WindhawkUtils::StringSetting::make(L"clipboardMode").get();
    if (s.mode != L"image" && s.mode != L"file" && s.mode != L"path" && s.mode != L"none") {
        s.mode = L"image";
    }
    s.pathFormat = WindhawkUtils::StringSetting::make(L"pathFormat").get();
    s.folder = WindhawkUtils::StringSetting::make(L"folder").get();
    const auto firstNonWhitespace = s.folder.find_first_not_of(L" \t\r\n");
    const auto lastNonWhitespace = s.folder.find_last_not_of(L" \t\r\n");
    s.folder = firstNonWhitespace == std::wstring::npos
                   ? std::wstring()
                   : s.folder.substr(firstNonWhitespace,
                                     lastNonWhitespace - firstNonWhitespace + 1);
    if (s.removeExactDuplicates && s.mode != L"image") {
        Wh_Log(L"Exact duplicate removal requires Image clipboard mode; disabled for the current mode");
    }
    if (s.folder.empty()) {
        s.folder = DefaultScreenshotsFolder();
    } else {
        // Expand things like %USERPROFILE% so a pasted path with env vars
        // resolves. The two-call form avoids a length cliff: the first call
        // with no buffer returns the size actually needed, so a long expansion
        // isn't silently left unexpanded.
        DWORD need = ExpandEnvironmentStringsW(s.folder.c_str(), nullptr, 0);
        if (need > 0) {
            std::wstring expanded(need, L'\0');
            DWORD got = ExpandEnvironmentStringsW(s.folder.c_str(),
                                                  expanded.data(), need);
            if (got > 0 && got <= need) {
                expanded.resize(got - 1);  // Drop the terminator counted in got.
                s.folder = std::move(expanded);
            }
        }
    }
    // Canonicalize once: this also turns '/' into '\' and resolves '.' and '..',
    // so nothing downstream has to cope with a path the shell parser rejects.
    // Skipped when empty (DefaultScreenshotsFolder() can return "" when neither
    // known folder resolves): GetFullPathNameW treats "" as a relative path and
    // resolves it against the current directory instead of leaving it empty,
    // which would defeat WatchThread's empty-folder idle check. Two-call form
    // for the same reason as above: a long path shouldn't silently skip
    // normalization.
    if (!s.folder.empty()) {
        DWORD need = GetFullPathNameW(s.folder.c_str(), 0, nullptr, nullptr);
        if (need > 0) {
            std::wstring full(need, L'\0');
            DWORD got =
                GetFullPathNameW(s.folder.c_str(), need, full.data(), nullptr);
            if (got > 0 && got < need) {
                full.resize(got);
                s.folder = std::move(full);
            }
        }
    }

    EnterCriticalSection(&g_lock);
    g_recentContent.clear();
    // An unrelated change, such as turning on logging, keeps a recording
    // session going. Turning sessions off or watching another folder ends it.
    const bool keepSession =
        s.sessions && CompareStringOrdinal(s.folder.c_str(), -1,
                                           g_settings.folder.c_str(), -1,
                                           TRUE) == CSTR_EQUAL;
    g_settings = std::move(s);
    ++g_generation;
    if (!keepSession) g_activeSession.store(0);
    SetEvent(g_settingsEvent);
    LeaveCriticalSection(&g_lock);
}

static Settings SnapshotSettings(ULONGLONG* generation = nullptr) {
    EnterCriticalSection(&g_lock);
    Settings s = g_settings;
    if (generation) *generation = g_generation.load();
    LeaveCriticalSection(&g_lock);
    return s;
}

// True if the stop event became signalled within the wait.
static bool WaitStop(DWORD ms) {
    return WaitForSingleObject(g_stopEvent, ms) == WAIT_OBJECT_0;
}

// Cleanup is scheduled rather than waited for, so this only checks. It must not
// reset g_settingsEvent: the prompt thread waits on that event to dismiss a
// popup when settings change, and a reset here could swallow the signal.
static bool CleanupCancelled(DWORD, ULONGLONG generation) {
    return WaitStop(0) || g_generation.load() != generation;
}

// ============================================================================
// File name helpers
// ============================================================================

static bool IsSupportedImage(const std::wstring& name) {
    auto dot = name.find_last_of(L'.');
    if (dot == std::wstring::npos) {
        return false;
    }
    // CompareStringOrdinal with bIgnoreCase is a true ordinal, case-insensitive
    // match, so no locale-sensitive case folding is involved: a Turkish locale
    // can't fold the 'i' in .tif/.tiff/.jfif/.gif to a dotless form matching nothing.
    static constexpr const wchar_t* kExtensions[] = {
        L".png", L".jpg",  L".jpeg", L".jfif", L".bmp",
        L".gif", L".webp", L".tif",  L".tiff",
    };
    const wchar_t* ext = name.c_str() + dot;
    for (const wchar_t* candidate : kExtensions) {
        if (CompareStringOrdinal(ext, -1, candidate, -1, TRUE) == CSTR_EQUAL) {
            return true;
        }
    }
    return false;
}

// A change-notification name must be a plain child file name. Reject anything
// with a path separator or a relative component so we can never act outside the
// watched folder.
static bool IsSafeChildName(const std::wstring& name) {
    if (name.empty() || name == L"." || name == L"..") {
        return false;
    }
    return name.find_first_of(L"\\/") == std::wstring::npos;
}

// ============================================================================
// Renaming (optional): name a new screenshot from the active window
// ============================================================================

// Strips characters Windows disallows in a file name and trims to a sane length
// so a window title can be reused as a name safely.
static std::wstring SanitizeForFilename(const std::wstring& in) {
    std::wstring out;
    bool prevSpace = false;
    for (wchar_t c : in) {
        if (c < 0x20) continue;                   // control characters
        if (wcschr(L"\\/:*?\"<>|", c)) continue;  // reserved on Windows
        bool sp = (c == L' ');
        if (sp && prevSpace) continue;            // collapse runs of spaces
        out += c;
        prevSpace = sp;
    }
    size_t b = out.find_first_not_of(L' ');
    if (b == std::wstring::npos) return L"";
    size_t e = out.find_last_not_of(L" .");       // no trailing space or dot
    out = out.substr(b, e - b + 1);
    if (out.size() > 80) {
        out.resize(80);
        // Don't end on half of a surrogate pair, which would leave a lone
        // surrogate (an emoji cut in two) in the name.
        if (!out.empty() && out.back() >= 0xD800 && out.back() <= 0xDBFF) {
            out.pop_back();
        }
    }
    e = out.find_last_not_of(L" .");
    if (e == std::wstring::npos) return L"";
    out.resize(e + 1);
    return out;
}

// Title of the window currently in front, sanitized for use in a file name.
static std::wstring ActiveWindowLabel() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return L"";
    int len = GetWindowTextLengthW(hwnd);
    if (len <= 0) return L"";
    std::wstring title((size_t)len + 1, L'\0');
    int got = GetWindowTextW(hwnd, title.data(), len + 1);
    title.resize(got < 0 ? 0 : (size_t)got);
    return SanitizeForFilename(title);
}

// ============================================================================
// Path text formatting (Path clipboard mode)
// ============================================================================

// File URI with proper percent-encoding, so a name with #, %, ( or ) (which window
// titles are full of, and which SanitizeForFilename keeps) still makes a link and a
// Markdown image that paste cleanly rather than a broken one.
static std::wstring PathToFileUri(const std::wstring& path) {
    std::wstring uri = L"file:///";
    for (wchar_t c : path) {
        if (c == L'\\' || c == L'/') {
            uri += L'/';
        } else if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') ||
                   (c >= L'0' && c <= L'9') || wcschr(L"-._~:", c)) {
            uri += c;  // unreserved, plus the drive colon
        } else if (c < 0x80) {
            wchar_t esc[4];
            swprintf_s(esc, L"%%%02X", (unsigned)c);
            uri += esc;  // ' ' -> %20, '#' -> %23, '(' -> %28, '%' -> %25
        } else {
            uri += c;  // non-ASCII passes through; file: consumers accept it
        }
    }
    return uri;
}

static std::wstring FormatPathText(const std::wstring& path,
                                   const std::wstring& fmt) {
    if (fmt == L"quoted") return L"\"" + path + L"\"";
    if (fmt == L"uri") return PathToFileUri(path);
    if (fmt == L"markdown") return L"![](" + PathToFileUri(path) + L")";
    return path;  // "plain"
}

// ============================================================================
// Clipboard payloads
// ============================================================================

static bool OpenOwnedClipboard() {
    for (unsigned i = 0; i < 12; ++i) {
        if (WaitStop(0) || g_generation.load() != g_clipboardGeneration) return false;
        if (g_clipboardOwner && OpenClipboard(g_clipboardOwner)) {
            if (EmptyClipboard()) return true;
            CloseClipboard();
            return false;
        }
        if (WaitStop(25)) return false;
    }
    return false;
}

static bool ClipboardText(const std::wstring& text) {
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!h) {
        return false;
    }
    void* p = GlobalLock(h);
    if (!p) {
        GlobalFree(h);
        return false;
    }
    memcpy(p, text.c_str(), bytes);
    GlobalUnlock(h);

    if (!OpenOwnedClipboard()) {
        GlobalFree(h);
        return false;
    }
    bool ok = SetClipboardData(CF_UNICODETEXT, h) != nullptr;
    if (!ok) {
        GlobalFree(h);  // Ownership stays with us on failure.
    }
    CloseClipboard();
    return ok;
}

static bool ClipboardFile(const std::wstring& path) {
    size_t bytes = sizeof(DROPFILES) + (path.size() + 2) * sizeof(wchar_t);
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, bytes);
    if (!h) {
        return false;
    }
    auto* drop = (DROPFILES*)GlobalLock(h);
    if (!drop) {
        GlobalFree(h);
        return false;
    }
    drop->pFiles = sizeof(DROPFILES);
    drop->fWide = TRUE;
    memcpy((BYTE*)drop + sizeof(DROPFILES), path.c_str(),
           (path.size() + 1) * sizeof(wchar_t));
    GlobalUnlock(h);

    if (!OpenOwnedClipboard()) {
        GlobalFree(h);
        return false;
    }
    bool ok = SetClipboardData(CF_HDROP, h) != nullptr;
    if (!ok) {
        GlobalFree(h);
    }
    CloseClipboard();
    return ok;
}

// Result of an image copy. CopiedFirstFrameOnly means a multi-page or animated
// image was copied but only its first frame reached the clipboard, so the caller
// must keep the source file (the rest isn't on the clipboard).
enum class ImageCopy { Failed, Copied, CopiedFirstFrameOnly };

// Decodes any WIC-supported image into a self-contained CF_DIBV5 payload, which is
// what makes the copied image survive deletion of the source file. Windows
// synthesizes CF_DIB and CF_BITMAP from CF_DIBV5 on demand, so publishing those too
// would just be another full-size copy of the bitmap (it runs in a 32-bit process).
static ImageCopy ClipboardImage(const std::wstring& path,
                                HANDLE lockedFile = INVALID_HANDLE_VALUE) {
    IWICImagingFactory* factory = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)))) {
        return ImageCopy::Failed;
    }

    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;
    IWICBitmapFlipRotator* flipper = nullptr;
    HGLOBAL hV5 = nullptr;
    bool ok = false;
    bool multiFrame = false;

    do {
        HRESULT decode = lockedFile != INVALID_HANDLE_VALUE
            ? factory->CreateDecoderFromFileHandle(
                  (ULONG_PTR)lockedFile, nullptr,
                  WICDecodeMetadataCacheOnDemand, &decoder)
            : factory->CreateDecoderFromFilename(
                  path.c_str(), nullptr, GENERIC_READ,
                  WICDecodeMetadataCacheOnDemand, &decoder);
        if (FAILED(decode)) {
            break;
        }
        // A multi-page file (routinely a scanned TIFF) decodes only frame 0 onto
        // the clipboard. Report that so the caller keeps the source file: pages
        // 2..N aren't on the clipboard and would be lost if it were deleted. A
        // count that can't be read (a third-party codec returning E_NOTIMPL) is
        // treated as multi-frame, so frame 0 still copies but the file is kept.
        UINT frames = 0;
        multiFrame = FAILED(decoder->GetFrameCount(&frames)) || frames != 1;
        if (FAILED(decoder->GetFrame(0, &frame)) ||
            FAILED(factory->CreateFormatConverter(&converter)) ||
            FAILED(converter->Initialize(
                frame, GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone,
                nullptr, 0.0, WICBitmapPaletteTypeCustom))) {
            break;
        }

        UINT width = 0, height = 0;
        if (FAILED(converter->GetSize(&width, &height)) || width == 0 ||
            height == 0 || width > 30000 || height > 30000) {
            break;
        }

        size_t stride = (size_t)width * 4;
        size_t pixels = stride * height;
        if (pixels > kMaxDecodedBytes || WaitStop(0)) break;

        hV5 = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPV5HEADER) + pixels);
        if (!hV5) {
            break;
        }
        BYTE* v5 = (BYTE*)GlobalLock(hV5);
        if (!v5) {
            break;
        }
        auto* bv5 = (BITMAPV5HEADER*)v5;
        ZeroMemory(bv5, sizeof(*bv5));
        bv5->bV5Size = sizeof(BITMAPV5HEADER);
        bv5->bV5Width = (LONG)width;
        bv5->bV5Height = (LONG)height;
        bv5->bV5Planes = 1;
        bv5->bV5BitCount = 32;
        bv5->bV5Compression = BI_BITFIELDS;
        bv5->bV5SizeImage = (DWORD)pixels;
        bv5->bV5RedMask = 0x00FF0000;
        bv5->bV5GreenMask = 0x0000FF00;
        bv5->bV5BlueMask = 0x000000FF;
        bv5->bV5AlphaMask = 0xFF000000;
        bv5->bV5CSType = LCS_WINDOWS_COLOR_SPACE;

        // A DIB stores rows bottom-up, but the converter yields them top-down.
        // Flipping vertically and copying straight into the DIB buffer avoids a
        // second full-size scratch copy (peak stays 1x the bitmap, which matters
        // for a large TIFF in a 32-bit process) and drops the manual row copy.
        if (FAILED(factory->CreateBitmapFlipRotator(&flipper)) ||
            FAILED(flipper->Initialize(converter,
                                       WICBitmapTransformFlipVertical)) ||
            FAILED(flipper->CopyPixels(nullptr, (UINT)stride, (UINT)pixels,
                                       v5 + sizeof(BITMAPV5HEADER)))) {
            GlobalUnlock(hV5);
            break;
        }
        GlobalUnlock(hV5);

        if (!OpenOwnedClipboard()) {
            break;
        }
        bool setV5 = SetClipboardData(CF_DIBV5, hV5) != nullptr;
        if (setV5) {
            hV5 = nullptr;  // Clipboard owns it now.
        }
        CloseClipboard();
        ok = setV5;
    } while (false);

    if (hV5) {
        GlobalFree(hV5);
    }
    if (flipper) {
        flipper->Release();
    }
    if (converter) {
        converter->Release();
    }
    if (frame) {
        frame->Release();
    }
    if (decoder) {
        decoder->Release();
    }
    factory->Release();
    if (!ok) {
        return ImageCopy::Failed;
    }
    return multiFrame ? ImageCopy::CopiedFirstFrameOnly : ImageCopy::Copied;
}

// ============================================================================
// Toast notification identity (AUMID + COM activator CLSID)
//
// Windows requires three things to agree before an unpackaged Win32 app gets
// interactive toast notifications: a Start Menu shortcut carrying the AUMID
// (PKEY_AppUserModel_ID) and the activator CLSID (PKEY_AppUserModel_ToastActivatorCLSID),
// a registry LocalServer32 entry for that CLSID, and a live COM registration
// (CoRegisterClassObject) for the process that wants to handle button clicks.
// ============================================================================

static constexpr wchar_t kAppUserModelId[] = L"mario0318.SnapSentry";
static constexpr wchar_t kShortcutName[] = L"SnapSentry.lnk";

// Assigned to this mod; do not reuse or change this value --
// Windows ties the shortcut and registry state below to this exact value.
// {304BD1DF-F3CE-414D-A33B-3BA70D2CE081}
DEFINE_GUID(CLSID_SnapSentryToastActivator, 0x304bd1df, 0xf3ce, 0x414d, 0xa3,
            0x3b, 0x3b, 0xa7, 0x0d, 0x2c, 0xe0, 0x81);

static bool SetStringProp(IPropertyStore* store, REFPROPERTYKEY key,
                          const wchar_t* value) {
    PROPVARIANT pv{};
    pv.vt = VT_LPWSTR;
    pv.pwszVal = (PWSTR)CoTaskMemAlloc((wcslen(value) + 1) * sizeof(wchar_t));
    if (!pv.pwszVal) {
        return false;
    }
    wcscpy_s(pv.pwszVal, wcslen(value) + 1, value);
    bool ok = SUCCEEDED(store->SetValue(key, pv));
    PropVariantClear(&pv);  // Also frees pwszVal (CoTaskMemAlloc-compatible).
    return ok;
}

static bool SetClsidProp(IPropertyStore* store, REFPROPERTYKEY key,
                         REFCLSID clsid) {
    PROPVARIANT pv{};
    pv.vt = VT_CLSID;
    pv.puuid = (CLSID*)CoTaskMemAlloc(sizeof(CLSID));
    if (!pv.puuid) {
        return false;
    }
    *pv.puuid = clsid;
    bool ok = SUCCEEDED(store->SetValue(key, pv));
    PropVariantClear(&pv);
    return ok;
}

static bool ShortcutHasCorrectProperties(IShellLinkW* link) {
    IPropertyStore* store = nullptr;
    if (FAILED(link->QueryInterface(IID_PPV_ARGS(&store)))) {
        return false;
    }
    PROPVARIANT aumid{};
    PROPVARIANT clsid{};
    bool ok = SUCCEEDED(store->GetValue(PKEY_AppUserModel_ID, &aumid)) &&
              aumid.vt == VT_LPWSTR && aumid.pwszVal &&
              wcscmp(aumid.pwszVal, kAppUserModelId) == 0 &&
              SUCCEEDED(store->GetValue(PKEY_AppUserModel_ToastActivatorCLSID,
                                        &clsid)) &&
              clsid.vt == VT_CLSID && clsid.puuid &&
              IsEqualCLSID(*clsid.puuid, CLSID_SnapSentryToastActivator);
    PropVariantClear(&aumid);
    PropVariantClear(&clsid);
    store->Release();
    return ok;
}

// Creates (or repairs) %APPDATA%\Microsoft\Windows\Start Menu\Programs\SnapSentry.lnk
// pointing at the current windhawk.exe, tagged with our AUMID and activator CLSID.
// Idempotent: does nothing if a correctly-tagged shortcut already exists.
// True only if a shortcut already exists at the path and carries our exact AUMID
// and activator CLSID. Uses its own throwaway object so a failed load never
// touches the object we create the shortcut with.
static bool ShortcutExistsAndValid(const std::wstring& path) {
    if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link)))) {
        return false;
    }
    bool ok = false;
    IPersistFile* file = nullptr;
    if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&file)))) {
        if (SUCCEEDED(file->Load(path.c_str(), STGM_READ))) {
            ok = ShortcutHasCorrectProperties(link);
        }
        file->Release();
    }
    link->Release();
    return ok;
}

static bool EnsureAumidRegistered() {
    PWSTR programs = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &programs);
    if (FAILED(hr)) {
        Wh_Log(L"AUMID: SHGetKnownFolderPath failed 0x%08lx", hr);
        return false;
    }
    std::wstring path = std::wstring(programs) + L"\\" + kShortcutName;
    CoTaskMemFree(programs);

    if (ShortcutExistsAndValid(path)) {
        return true;  // Already registered correctly.
    }

    WCHAR exePath[MAX_PATH];
    DWORD exeLen = GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
    if (exeLen == 0 || exeLen >= ARRAYSIZE(exePath)) {
        // At the buffer size it truncates with no null guarantee, so reject it
        // rather than write a shortcut that points somewhere wrong.
        Wh_Log(L"AUMID: GetModuleFileName failed/truncated %lu", GetLastError());
        return false;
    }

    // Create the shortcut on a fresh object (the documented pattern).
    IShellLinkW* link = nullptr;
    hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&link));
    if (FAILED(hr)) {
        Wh_Log(L"AUMID: CoCreateInstance(ShellLink) failed 0x%08lx", hr);
        return false;
    }

    bool ok = false;
    do {
        hr = link->SetPath(exePath);
        if (FAILED(hr)) {
            Wh_Log(L"AUMID: IShellLink::SetPath failed 0x%08lx", hr);
            break;
        }
        hr = link->SetArguments(L"");
        if (FAILED(hr)) {
            Wh_Log(L"AUMID: IShellLink::SetArguments failed 0x%08lx", hr);
            break;
        }
        hr = link->SetDescription(L"SnapSentry (Windhawk)");
        if (FAILED(hr)) {
            Wh_Log(L"AUMID: IShellLink::SetDescription failed 0x%08lx", hr);
            break;
        }

        IPropertyStore* store = nullptr;
        hr = link->QueryInterface(IID_PPV_ARGS(&store));
        if (FAILED(hr)) {
            Wh_Log(L"AUMID: QI IPropertyStore failed 0x%08lx", hr);
            break;
        }
        bool aumidProp = SetStringProp(store, PKEY_AppUserModel_ID, kAppUserModelId);
        bool clsidProp = SetClsidProp(store, PKEY_AppUserModel_ToastActivatorCLSID,
                                      CLSID_SnapSentryToastActivator);
        hr = store->Commit();
        store->Release();
        if (!aumidProp || !clsidProp || FAILED(hr)) {
            Wh_Log(L"AUMID: set props failed (aumid=%d clsid=%d commit=0x%08lx)",
                   aumidProp, clsidProp, hr);
            break;
        }

        IPersistFile* file = nullptr;
        hr = link->QueryInterface(IID_PPV_ARGS(&file));
        if (FAILED(hr)) {
            Wh_Log(L"AUMID: QI IPersistFile failed 0x%08lx", hr);
            break;
        }
        hr = file->Save(path.c_str(), TRUE);
        file->Release();
        if (FAILED(hr)) {
            Wh_Log(L"AUMID: IPersistFile::Save failed 0x%08lx", hr);
            break;
        }
        ok = true;
    } while (false);

    link->Release();
    return ok;
}

// Writes HKCU\Software\Classes\CLSID\{...}\LocalServer32 so Windows treats our
// CLSID as a valid, launchable COM server for the AUMID above. While SnapSentry
// is already running, CoRegisterClassObject (below) intercepts activation before
// Windows would ever need to launch a process via this key; a cold launch (the
// mod's process not already running when a toast button is clicked) is not
// specially handled and is a known limitation. It is rare in practice because the
// watcher runs continuously while the mod is enabled, and the key is removed on
// unload so nothing is left to launch once the mod is gone.
static bool EnsureClsidRegistered() {
    WCHAR exePath[MAX_PATH];
    DWORD exeLen = GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
    if (exeLen == 0 || exeLen >= ARRAYSIZE(exePath)) {
        return false;  // Truncated path would register a wrong LocalServer32 command.
    }
    WCHAR command[MAX_PATH + 64];
    swprintf_s(command, L"\"%s\" -tool-mod \"%s\"", exePath, WH_MOD_ID);

    WCHAR clsidStr[64];
    StringFromGUID2(CLSID_SnapSentryToastActivator, clsidStr, ARRAYSIZE(clsidStr));
    std::wstring keyPath = std::wstring(L"Software\\Classes\\CLSID\\") + clsidStr +
                           L"\\LocalServer32";

    // Windhawk currently hosts tool mods in an x86 process. Register both views
    // so the 64-bit notification shell and a 32-bit caller resolve the same
    // out-of-process activator.
    bool ok = true;
    const REGSAM views[] = {KEY_WOW64_64KEY, KEY_WOW64_32KEY};
    for (REGSAM view : views) {
        HKEY key = nullptr;
        LSTATUS status = RegCreateKeyExW(
            HKEY_CURRENT_USER, keyPath.c_str(), 0, nullptr, 0,
            KEY_SET_VALUE | view, nullptr, &key, nullptr);
        if (status == ERROR_SUCCESS) {
            status = RegSetValueExW(
                key, nullptr, 0, REG_SZ, (const BYTE*)command,
                (DWORD)((wcslen(command) + 1) * sizeof(wchar_t)));
            RegCloseKey(key);
        }
        if (status != ERROR_SUCCESS) {
            Wh_Log(L"Toast CLSID registration failed (view=0x%lx error=%ld)",
                   view, status);
            ok = false;
        }
    }
    return ok;
}

// Removes the Start Menu shortcut we created, but only if it's still ours (same
// AUMID and activator CLSID). A same-named file we didn't write is left alone so
// unload never deletes an unrelated shortcut.
static void RemoveAumidRegistration() {
    PWSTR programs = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &programs))) {
        return;
    }
    std::wstring path = std::wstring(programs) + L"\\" + kShortcutName;
    CoTaskMemFree(programs);
    if (ShortcutExistsAndValid(path)) {
        DeleteFileW(path.c_str());
    }
}

// Removes the CLSID key we wrote under both registry views. Deletes the whole
// {clsid} subtree since the mod owns that GUID; a missing key is not an error.
static void RemoveClsidRegistration() {
    WCHAR clsidStr[64];
    StringFromGUID2(CLSID_SnapSentryToastActivator, clsidStr, ARRAYSIZE(clsidStr));
    const REGSAM views[] = {KEY_WOW64_64KEY, KEY_WOW64_32KEY};
    for (REGSAM view : views) {
        HKEY parent = nullptr;
        LSTATUS status = RegOpenKeyExW(
            HKEY_CURRENT_USER, L"Software\\Classes\\CLSID", 0,
            DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | view, &parent);
        if (status == ERROR_SUCCESS) {
            RegDeleteTreeW(parent, clsidStr);
            RegCloseKey(parent);
        }
    }
}

// Windows creates a per-AUMID key the first time a toast is delivered, which is
// what keeps "SnapSentry" listed under Settings > System > Notifications. It is
// our own AUMID, so remove it too, otherwise the entry outlives the mod. A
// missing key is not an error.
static void RemoveNotificationSettings() {
    HKEY parent = nullptr;
    LSTATUS status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Notifications\\Settings", 0,
        DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &parent);
    if (status == ERROR_SUCCESS) {
        RegDeleteTreeW(parent, kAppUserModelId);
        RegCloseKey(parent);
    }
}

// ============================================================================
// COM activator: invoked when a toast button is clicked (routed via the CLSID
// registered above). Parses which button and hands the result to whichever
// ShowToast() call is currently waiting.
// ============================================================================

class ToastActivator : public INotificationActivationCallback {
public:
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }  // Static lifetime.
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** obj) override {
        if (riid == IID_IUnknown ||
            riid == IID_INotificationActivationCallback) {
            *obj = static_cast<INotificationActivationCallback*>(this);
            return S_OK;
        }
        *obj = nullptr;
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE
    Activate(LPCWSTR, LPCWSTR invokedArgs, const NOTIFICATION_USER_INPUT_DATA*,
            ULONG) override {
        int action = ACTION_AUTO;
        ULONGLONG toastId = 0;
        if (invokedArgs) {
            const wchar_t* separator = wcschr(invokedArgs, L'|');
            if (separator) toastId = _wcstoui64(separator + 1, nullptr, 10);
            std::wstring command(invokedArgs,
                                 separator ? separator - invokedArgs
                                           : wcslen(invokedArgs));
            if (command == L"delete") {
                action = ACTION_DELETE;
            } else if (command == L"copydelete") {
                action = ACTION_COPY_DELETE;
            } else if (command == L"keep") {
                action = ACTION_KEEP;
            } else if (command == L"body") {
                action = ACTION_COPY_ONLY;  // Toast clicked away, not answered.
            }
        }
        EnterCriticalSection(&g_toastLock);
        if (!toastId || toastId != g_activeToastId || g_toastAnswered) {
            LeaveCriticalSection(&g_toastLock);
            return S_OK;
        }
        g_toastAction = action;
        g_toastAnswered = true;
        LeaveCriticalSection(&g_toastLock);
        SetEvent(g_toastActionEvent);
        return S_OK;
    }
};

static ToastActivator g_toastActivator;

class ToastActivatorFactory : public IClassFactory {
public:
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** obj) override {
        if (riid == IID_IUnknown || riid == IID_IClassFactory) {
            *obj = static_cast<IClassFactory*>(this);
            return S_OK;
        }
        *obj = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID riid,
                                             void** obj) override {
        if (outer) {
            return CLASS_E_NOAGGREGATION;
        }
        return g_toastActivator.QueryInterface(riid, obj);
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) override { return S_OK; }
};

static ToastActivatorFactory g_toastActivatorFactory;
static DWORD g_toastActivatorCookie;

// Getting rid of a toast (swiping it away, clearing it from the notification
// center, letting the system drop it) never reaches Activate, which only fires
// for the three buttons, so the waiter would otherwise sit there as if nothing
// had happened. Treat a dismissal as Keep.
//
// The bundled SDK has no wrl/event.h, so there is no Microsoft::WRL::Callback to
// wrap a lambda in. This is a plain static implementation of the generated
// handler interface instead, in the same shape as the activator above.
using ToastDismissedHandler = ABI::Windows::Foundation::ITypedEventHandler<
    ABI::Windows::UI::Notifications::ToastNotification*,
    ABI::Windows::UI::Notifications::ToastDismissedEventArgs*>;

class ToastDismissListener : public ToastDismissedHandler {
public:
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }  // Static lifetime.
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** obj) override {
        if (riid == IID_IUnknown || riid == __uuidof(ToastDismissedHandler)) {
            *obj = static_cast<ToastDismissedHandler*>(this);
            return S_OK;
        }
        *obj = nullptr;
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE
    Invoke(ABI::Windows::UI::Notifications::IToastNotification* sender,
           ABI::Windows::UI::Notifications::IToastDismissedEventArgs* args) override {
        // A dismissal raised for an earlier toast can be dispatched by a later
        // toast's message pump; ignore anything that isn't the one we're waiting on.
        if (sender != g_activeToast.load()) {
            return S_OK;
        }
        // Only a user dismissal is an answer. TimedOut (the platform retired the
        // toast despite scenario=reminder) and ApplicationHidden (our own Hide) are
        // not, so leave the wait running for those rather than cutting it short.
        ABI::Windows::UI::Notifications::ToastDismissalReason reason =
            ABI::Windows::UI::Notifications::ToastDismissalReason_UserCanceled;
        if (args && SUCCEEDED(args->get_Reason(&reason)) &&
            reason != ABI::Windows::UI::Notifications::ToastDismissalReason_UserCanceled) {
            return S_OK;
        }
        EnterCriticalSection(&g_toastLock);
        // Only the toast currently being waited on, and only if a button click
        // hasn't already answered it (clicking one also dismisses the toast).
        // Swiping it away means "off my screen", not "delete", so copy, don't delete.
        bool mine = g_activeToastId != 0 && !g_toastAnswered;
        if (mine) {
            g_toastAction = ACTION_COPY_ONLY;
            g_toastAnswered = true;
        }
        LeaveCriticalSection(&g_toastLock);
        if (mine) {
            SetEvent(g_toastActionEvent);
        }
        return S_OK;
    }
};

static ToastDismissListener g_toastDismissListener;

using ToastFailedHandler = ABI::Windows::Foundation::ITypedEventHandler<
    ABI::Windows::UI::Notifications::ToastNotification*,
    ABI::Windows::UI::Notifications::ToastFailedEventArgs*>;

// Windows raises this when it could not deliver the notification. That is the
// platform's own answer to "was it shown", which notification-center history is
// not: a banner can appear with center history switched off, and Do Not Disturb
// files a toast into the center without ever showing a banner.

class ToastFailListener : public ToastFailedHandler {
public:
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }  // Static lifetime.
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** obj) override {
        if (riid == IID_IUnknown || riid == __uuidof(ToastFailedHandler)) {
            *obj = static_cast<ToastFailedHandler*>(this);
            return S_OK;
        }
        *obj = nullptr;
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE
    Invoke(ABI::Windows::UI::Notifications::IToastNotification* sender,
           ABI::Windows::UI::Notifications::IToastFailedEventArgs* args) override {
        if (sender != g_activeToast.load()) {
            return S_OK;
        }
        HRESULT code = E_FAIL;
        if (args) {
            args->get_ErrorCode(&code);
        }
        Wh_Log(L"Notification was not delivered (0x%08lx); copy only", (unsigned long)code);
        EnterCriticalSection(&g_toastLock);
        // Same shape as a dismissal: it is an answer, so it must be recorded,
        // not just signalled, or the wait settles it as Keep and skips the copy.
        bool mine = g_activeToastId != 0 && !g_toastAnswered;
        if (mine) {
            g_toastAction = ACTION_COPY_ONLY;
            g_toastAnswered = true;
        }
        LeaveCriticalSection(&g_toastLock);
        if (mine) {
            SetEvent(g_toastActionEvent);
        }
        return S_OK;
    }
};

static ToastFailListener g_toastFailListener;

// ============================================================================
// Toast notification display and wait
// ============================================================================

static std::wstring XmlEscape(const std::wstring& in) {
    std::wstring out;
    out.reserve(in.size());
    for (wchar_t c : in) {
        switch (c) {
            case L'&': out += L"&amp;"; break;
            case L'<': out += L"&lt;"; break;
            case L'>': out += L"&gt;"; break;
            case L'"': out += L"&quot;"; break;
            default: out += c;
        }
    }
    return out;
}

// Builds the notifier and the toast object for a given XML payload: the
// activation factory, the per-AUMID notifier, the current notification
// setting, and the constructed toast document. Both toast paths need this
// exact chain. Returns false if any step fails or notifications are off;
// settingReadable reports whether get_Setting itself succeeded, so a caller
// that needs to tell "notifications are off" apart from "couldn't find out"
// doesn't have to infer it from setting alone.
static bool BuildToast(const std::wstring& xml,
                       ABI::Windows::UI::Notifications::NotificationSetting& setting,
                       bool& settingReadable,
                       Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotifier>& notifier,
                       Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotification>& toast) {
    using namespace ABI::Windows::UI::Notifications;
    using namespace ABI::Windows::Data::Xml::Dom;
    using namespace ABI::Windows::Foundation;
    using Microsoft::WRL::ComPtr;
    using Microsoft::WRL::Wrappers::HStringReference;

    settingReadable = false;
    ComPtr<IToastNotificationManagerStatics> toastStatics;
    if (FAILED(RoGetActivationFactory(
            HStringReference(
                RuntimeClass_Windows_UI_Notifications_ToastNotificationManager)
                .Get(),
            IID_PPV_ARGS(&toastStatics)))) {
        return false;
    }
    if (FAILED(toastStatics->CreateToastNotifierWithId(
            HStringReference(kAppUserModelId).Get(), &notifier))) {
        return false;
    }

    setting = NotificationSetting_Enabled;
    HRESULT settingResult = notifier->get_Setting(&setting);
    settingReadable = SUCCEEDED(settingResult);
    // A newly registered desktop identity can have no settings record yet.
    // Try its first notification, but never infer permission to auto-delete
    // from this missing record. Other lookup errors still use the dialog.
    const bool settingsNotCreated = settingResult == HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    if ((!settingReadable && !settingsNotCreated) ||
        (settingReadable && setting != NotificationSetting_Enabled)) {
        return false;
    }

    ComPtr<IInspectable> docInspectable;
    if (FAILED(RoActivateInstance(
            HStringReference(RuntimeClass_Windows_Data_Xml_Dom_XmlDocument).Get(),
            &docInspectable))) {
        return false;
    }
    ComPtr<IXmlDocument> doc;
    ComPtr<IXmlDocumentIO> docIO;
    if (FAILED(docInspectable.As(&doc)) || FAILED(docInspectable.As(&docIO)) ||
        FAILED(docIO->LoadXml(HStringReference(xml.c_str()).Get()))) {
        return false;
    }

    ComPtr<IToastNotificationFactory> toastFactory;
    if (FAILED(RoGetActivationFactory(
            HStringReference(RuntimeClass_Windows_UI_Notifications_ToastNotification)
                .Get(),
            IID_PPV_ARGS(&toastFactory)))) {
        return false;
    }
    return SUCCEEDED(toastFactory->CreateToastNotification(doc.Get(), &toast));
}

// Returns the final path segment: what should appear on screen instead of the
// full path.
static std::wstring BaseName(const std::wstring& path) {
    return path.substr(path.find_last_of(L"\\/") + 1);
}

// Builds and shows the toast, then waits up to s.delaySeconds for a button
// click (via ToastActivator::Activate, dispatched by the prompt thread's message
// pump in the wait below)
// or an explicit dismissal. Returns false, meaning "use the dialog instead",
// if registration hasn't succeeded or any WinRT step fails, so the mod stays
// usable even where toast notifications don't work.
static bool ShowToast(const std::wstring& path, const Settings& s, int& action,
                      ULONGLONG generation) {
    using namespace ABI::Windows::UI::Notifications;
    using namespace ABI::Windows::Foundation;
    using Microsoft::WRL::ComPtr;

    if (!g_toastRegistered.load()) {
        return false;
    }

    std::wstring name = BaseName(path);
    ULONGLONG toastId = GetTickCount64();
    std::wstring id = std::to_wstring(toastId);
    const std::wstring deleteAction = (s.mode == L"file" || s.mode == L"path") ? L"" :
        L"<action content=\"Delete\" arguments=\"delete|" + id + L"\" activationType=\"background\"/>";
    std::wstring xml =
        L"<toast scenario=\"reminder\" duration=\"long\" "
        L"activationType=\"background\" launch=\"body|" + id + L"\">"
        L"<visual><binding template=\"ToastGeneric\">"
        L"<text>Screenshot saved</text><text>" +
        XmlEscape(name) +
        L"</text><text placement=\"attribution\">SnapSentry</text>"
        L"</binding></visual><actions>" + deleteAction +
        L"<action content=\"Copy + delete\" arguments=\"copydelete|" + id + L"\" activationType=\"background\"/>"
        L"<action content=\"Keep, don't copy\" arguments=\"keep|" + id + L"\" activationType=\"background\"/>"
        L"</actions></toast>";

    // Show() reports success even when notifications are switched off for us, and
    // then nothing is ever delivered and no answer can arrive. BuildToast asks
    // first, so we can tell the two failure shapes apart. If we genuinely can't
    // tell why it failed, fall back to the dialog; but if notifications are
    // switched off (system-wide, by policy, or for this app), the user has said
    // "stop popping things up", so don't substitute a modal dialog on every
    // screenshot. Do the configured clipboard copy and never delete, since the
    // user was never actually prompted.
    NotificationSetting setting = NotificationSetting_Enabled;
    bool settingReadable = false;
    ComPtr<IToastNotifier> notifier;
    ComPtr<IToastNotification> toast;
    if (!BuildToast(xml, setting, settingReadable, notifier, toast)) {
        if (settingReadable && setting != NotificationSetting_Enabled) {
            Wh_Log(L"Notifications off (setting=%d); copy only, no dialog",
                   (int)setting);
            action = ACTION_COPY_ONLY;
            return true;
        }
        return false;  // Can't tell: fall back to the dialog.
    }

    ResetEvent(g_toastActionEvent);
    ResetEvent(g_settingsEvent);  // Only one prompt is ever up at a time.
    EnterCriticalSection(&g_toastLock);
    g_activeToastId = toastId;
    g_toastAction = ACTION_AUTO;
    g_toastAnswered = false;
    LeaveCriticalSection(&g_toastLock);
    g_activeToast.store(toast.Get());

    EventRegistrationToken dismissToken{};
    bool dismissHooked =
        SUCCEEDED(toast->add_Dismissed(&g_toastDismissListener, &dismissToken));
    EventRegistrationToken failedToken{};
    bool failedHooked =
        SUCCEEDED(toast->add_Failed(&g_toastFailListener, &failedToken));

    if (FAILED(notifier->Show(toast.Get()))) {
        g_activeToast.store(nullptr);
        EnterCriticalSection(&g_toastLock);
        g_activeToastId = 0;  // Don't accept a late Activate for a toast that never showed.
        LeaveCriticalSection(&g_toastLock);
        if (dismissHooked) {
            toast->remove_Dismissed(dismissToken);
        }
        if (failedHooked) {
            toast->remove_Failed(failedToken);
        }
        return false;
    }

    // Ask whether the shell was accepting notifications when this toast was
    // submitted. Checking only at expiry can reverse the answer while the
    // countdown is running, for example when a full-screen app opens or closes.
    QUERY_USER_NOTIFICATION_STATE showState = QUNS_ACCEPTS_NOTIFICATIONS;
    bool notificationsAcceptedAtShow =
        SUCCEEDED(SHQueryUserNotificationState(&showState)) &&
        showState == QUNS_ACCEPTS_NOTIFICATIONS;
    if (!notificationsAcceptedAtShow) {
        Wh_Log(L"Notifications were held back at show time (state=%d); copy only",
               (int)showState);
    }

    // Our own timer is authoritative for the automatic action. Remove the toast
    // once the action is settled so stale buttons can't affect a later screenshot.
    //
    // With no countdown the wait used to be INFINITE, which meant a toast that was
    // never delivered (Focus assist, a delivery failure, anything that swallows it
    // quietly) parked this thread forever and every later screenshot queued behind
    // it. Wait a long time instead of forever, and treat running out as Keep, since
    // nobody saw the toast to ask for a deletion.
    bool noCountdown = s.delaySeconds <= 0;
    DWORD timeoutMs =
        noCountdown ? kNoAnswerCapMs : (DWORD)s.delaySeconds * 1000;
    HANDLE waits[] = {g_stopEvent, g_toastActionEvent, g_settingsEvent};
    DWORD start = GetTickCount();
    // An activation callback can be dispatched reentrantly from the message
    // pump below, so settle under the same lock the callbacks take: an answer
    // already accepted is never overwritten by a timeout.
    auto settle = [&](int proposed) {
        bool cancelled = WaitStop(0);
        EnterCriticalSection(&g_toastLock);
        if (cancelled) {
            action = ACTION_KEEP;
        } else if (g_toastAnswered) {
            action = g_toastAction;
        } else {
            action = proposed;
        }
        g_toastAction = action;
        g_toastAnswered = true;
        LeaveCriticalSection(&g_toastLock);
    };
    for (;;) {
        DWORD elapsed = GetTickCount() - start;
        if (WaitStop(0)) {  // Unload: never copy, never delete.
            settle(ACTION_KEEP);
            break;
        }
        // Stale settings are a reason not to run the automatic action, not a
        // reason to drop the screenshot.
        if (g_generation.load() != generation) {
            settle(ACTION_COPY_ONLY);
            break;
        }
        if (WaitForSingleObject(g_toastActionEvent, 0) == WAIT_OBJECT_0) {
            settle(ACTION_KEEP);
            break;  // A real answer always wins over any timeout.
        }
        if (elapsed >= timeoutMs) {
            NotificationSetting currentSetting = NotificationSetting_Enabled;
            bool enabled = SUCCEEDED(notifier->get_Setting(&currentSetting)) &&
                           currentSetting == NotificationSetting_Enabled;
            // Focus assist, presentation mode, a full-screen app or the secure
            // desktop all suppress the banner without failing delivery, so the
            // countdown would be deleting for a prompt nobody was shown.
            settle(noCountdown || !settingReadable || !enabled || !notificationsAcceptedAtShow
                       ? ACTION_COPY_ONLY
                                                     : ACTION_AUTO);
            break;
        }
        // The answer arrives on an event, so wait out the deadline in one go.
        DWORD result = MsgWaitForMultipleObjectsEx(
            ARRAYSIZE(waits), waits, timeoutMs - elapsed, QS_ALLINPUT,
            MWMO_INPUTAVAILABLE | MWMO_ALERTABLE);
        if (result == WAIT_IO_COMPLETION) {
            continue;  // MWMO_ALERTABLE: an APC ran, keep waiting.
        }
        if (result == WAIT_TIMEOUT) {
            continue;  // Deadlines are checked above, not inferred from a slice.
        }
        if (result == WAIT_OBJECT_0) {  // Stop requested: never delete on shutdown.
            settle(ACTION_KEEP);
            break;
        }
        if (result == WAIT_OBJECT_0 + 1) {
            settle(ACTION_KEEP);
            break;
        }
        if (result == WAIT_OBJECT_0 + 2) {
            continue;  // Settings reloaded: handled at the top of the loop.
        }
        if (result == WAIT_OBJECT_0 + ARRAYSIZE(waits)) {
            MSG msg;
            for (unsigned count = 0; count < 32 &&
                 PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE); ++count) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            continue;
        }
        Wh_Log(L"Toast wait failed %lu", GetLastError());
        settle(ACTION_KEEP);
        break;
    }

    g_activeToast.store(nullptr);
    EnterCriticalSection(&g_toastLock);
    g_activeToastId = 0;
    LeaveCriticalSection(&g_toastLock);
    if (dismissHooked) {
        toast->remove_Dismissed(dismissToken);
    }
    if (failedHooked) {
        toast->remove_Failed(failedToken);
    }
    notifier->Hide(toast.Get());
    return true;
}

// Loads the notification stack once at startup so the first real screenshot
// doesn't pay the cold-start cost while the user is waiting for the popup.
static void PrewarmToast() {
    using namespace ABI::Windows::UI::Notifications;
    using namespace ABI::Windows::Foundation;
    using Microsoft::WRL::ComPtr;
    using Microsoft::WRL::Wrappers::HStringReference;

    ComPtr<IToastNotificationManagerStatics> statics;
    if (SUCCEEDED(RoGetActivationFactory(
            HStringReference(
                RuntimeClass_Windows_UI_Notifications_ToastNotificationManager)
                .Get(),
            IID_PPV_ARGS(&statics)))) {
        ComPtr<IToastNotifier> notifier;
        statics->CreateToastNotifierWithId(HStringReference(kAppUserModelId).Get(),
                                           &notifier);
    }
    ComPtr<IInspectable> doc;
    RoActivateInstance(
        HStringReference(RuntimeClass_Windows_Data_Xml_Dom_XmlDocument).Get(),
        &doc);
}

// ============================================================================
// Action dialog (fallback, used when toast notification registration failed)
// ============================================================================

struct DialogState {
    ULONGLONG generation;
    DWORD started;
    DWORD timeoutMs;
    bool showCountdown;   // False when there is no countdown, only the backstop.
    int expiryAction;     // What running out of time means (a real button id).
    bool expired;         // Set when the backstop closed it, not the user.
    bool reloaded;    // Settings changed while the dialog was up.
    std::wstring baseText;
};

static HRESULT CALLBACK DialogCallback(HWND hwnd, UINT msg, WPARAM, LPARAM,
                                       LONG_PTR ref) {
    auto* state = reinterpret_cast<DialogState*>(ref);
    switch (msg) {
        case TDN_CREATED:
            g_dialog.store(hwnd);
            // If stop was signaled before the handle was stored, close now as Keep.
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
                SendMessageW(hwnd, TDM_CLICK_BUTTON, ACTION_KEEP, 0);
                break;
            }
            if (g_generation.load() != state->generation) {
                state->reloaded = true;
                SendMessageW(hwnd, TDM_CLICK_BUTTON, ACTION_KEEP, 0);
                break;
            }
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            break;
        case TDN_DESTROYED:
            g_dialog.store(nullptr);
            break;
        case TDN_TIMER:
            // Backstop for unload: if the stop event fired in the window before
            // TDN_CREATED stored our handle, WhTool_ModUninit couldn't dismiss us,
            // so close ourselves as Keep here rather than let the join hang.
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
                SendMessageW(hwnd, TDM_CLICK_BUTTON, ACTION_KEEP, 0);
                break;
            }
            if (g_generation.load() != state->generation) {
                state->reloaded = true;
                SendMessageW(hwnd, TDM_CLICK_BUTTON, ACTION_KEEP, 0);
                break;
            }
            {
                DWORD elapsed = GetTickCount() - state->started;
                if (elapsed >= state->timeoutMs) {
                    state->expired = true;  // Backstop, not a user choice.
                    SendMessageW(hwnd, TDM_CLICK_BUTTON, state->expiryAction, 0);
                } else if (state->showCountdown) {
                    DWORD left = (state->timeoutMs - elapsed + 999) / 1000;
                    std::wstring text = state->baseText + L"\n\nAutomatic action in " +
                                        std::to_wstring(left) + L" s…";
                    SendMessageW(hwnd, TDM_SET_ELEMENT_TEXT, TDE_CONTENT,
                                 (LPARAM)text.c_str());
                }
            }
            break;
    }
    return S_OK;
}

static int AskAction(const std::wstring& path, const Settings& s,
                     ULONGLONG generation) {
    if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
        return ACTION_KEEP;  // Don't put up UI during teardown.
    }
    std::wstring name = BaseName(path);
    DialogState state;
    state.generation = generation;
    state.started = GetTickCount();
    // Same backstop as the toast: without it, a dialog nobody answers holds the
    // prompt thread and no later popup appears until someone does.
    state.showCountdown = s.delaySeconds > 0;
    state.timeoutMs =
        state.showCountdown ? (DWORD)s.delaySeconds * 1000 : kNoAnswerCapMs;
    state.expiryAction = state.showCountdown ? ACTION_AUTO : ACTION_KEEP;
    state.expired = false;
    state.reloaded = false;
    state.baseText = name + L"\n\nChoose an action, or wait for the configured "
                            L"automatic action.";

    // Reserve the countdown line up front so TDF_SIZE_TO_CONTENT sizes the dialog
    // once instead of growing after the first timer tick.
    std::wstring initialText = state.baseText;
    if (state.showCountdown) {
        initialText += L"\n\nAutomatic action in " +
                       std::to_wstring((state.timeoutMs + 999) / 1000) + L" s…";
    }

    TASKDIALOG_BUTTON buttons[] = {
        {ACTION_DELETE, L"Delete now"},
        {ACTION_COPY_DELETE, L"Copy image && delete"},
        {ACTION_KEEP, L"Keep, don't copy"},
        {ACTION_AUTO, L"Use automatic action"},
    };

    TASKDIALOGCONFIG c{};
    c.cbSize = sizeof(c);
    c.dwFlags = TDF_SIZE_TO_CONTENT | TDF_CALLBACK_TIMER |
                TDF_ALLOW_DIALOG_CANCELLATION;
    c.pszWindowTitle = L"SnapSentry";
    c.pszMainIcon = TD_INFORMATION_ICON;
    c.pszMainInstruction = L"Screenshot saved";
    c.pszContent = initialText.c_str();
    const bool allowDelete = s.mode != L"file" && s.mode != L"path";
    c.cButtons = ARRAYSIZE(buttons) - (allowDelete ? 0 : 1);
    c.pButtons = buttons + (allowDelete ? 0 : 1);
    c.nDefaultButton = ACTION_KEEP;  // Enter should never be the destructive choice.
    c.pfCallback = DialogCallback;
    c.lpCallbackData = (LONG_PTR)&state;

    int result = ACTION_AUTO;
    // Cancellation (Esc / shutdown-driven close) is treated as Keep: never delete.
    // TDF_ALLOW_DIALOG_CANCELLATION makes that a successful call returning IDCANCEL
    // rather than a failure, so map anything that isn't one of our buttons to Keep.
    if (FAILED(TaskDialogIndirect(&c, &result, nullptr, nullptr))) {
        return ACTION_KEEP;
    }
    // The no-countdown backstop clicks Keep (a real button) on expiry; translate
    // that to copy-only, since nobody actually answered. A real Keep click, or a
    // countdown expiry (which uses ACTION_AUTO), is left as-is.
    if (state.reloaded) {
        return ACTION_COPY_ONLY;  // Stale settings, but the copy still happens.
    }
    if (state.expired && !state.showCountdown) {
        return ACTION_COPY_ONLY;
    }
    switch (result) {
        case ACTION_AUTO:
        case ACTION_DELETE:
        case ACTION_COPY_DELETE:
        case ACTION_KEEP:
            return result;
        default:
            // Esc / the X close the dialog with IDCANCEL. Teardown clicks the real
            // Keep button, and entry re-checks the stop event, so a cancel here can
            // only be a live user dismissing it: copy, never delete, matching the
            // toast-swipe path.
            return ACTION_COPY_ONLY;
    }
}

// Tries the toast notification first; falls back to the dialog box if toast
// registration didn't succeed on this machine or the WinRT call chain fails.
static int ChooseAction(const std::wstring& path, const Settings& s,
                        ULONGLONG generation) {
    int action = ACTION_AUTO;
    if (ShowToast(path, s, action, generation)) {
        return action;
    }
    return AskAction(path, s, generation);
}

// Fire-and-forget informational toast, shown when a multi-frame or animated image
// was copied (first frame only) and therefore kept instead of deleted, so an
// explicit "Copy + delete" or a countdown auto-action isn't a silent no-op. Unlike
// the action toast this adds no listeners and does not wait, so it never parks the
// worker. Purely informational, so there is nothing to fall back to: if it can't be
// shown (registration gone, notifications off), the caller's log line stands alone.
static void ShowKeptToast(const std::wstring& name) {
    using namespace ABI::Windows::UI::Notifications;
    using Microsoft::WRL::ComPtr;

    if (!g_toastRegistered.load()) {
        return;
    }
    // Don't put up UI during teardown: the registration this toast needs is about
    // to be removed, and a notification that outlives the mod would leave exactly
    // the residue the mod exists to avoid.
    if (WaitStop(0)) {
        return;
    }
    std::wstring xml =
        L"<toast duration=\"short\" activationType=\"background\">"
        L"<visual><binding template=\"ToastGeneric\">"
        L"<text>Kept the file</text><text>" +
        XmlEscape(name) +
        L" is a multi-page or animated image, so only its first frame was copied "
        L"and the file was kept.</text>"
        L"<text placement=\"attribution\">SnapSentry</text>"
        L"</binding></visual></toast>";

    // Respect the user's notification choice; nothing to substitute for an
    // informational message if they've turned notifications off. BuildToast
    // covers that check along with the rest of the chain.
    NotificationSetting setting = NotificationSetting_Enabled;
    bool settingReadable = false;
    ComPtr<IToastNotifier> notifier;
    ComPtr<IToastNotification> toast;
    if (!BuildToast(xml, setting, settingReadable, notifier, toast)) {
        return;
    }
    notifier->Show(toast.Get());
}

static const wchar_t* AuditOutcomeName(AuditOutcome outcome) {
    switch (outcome) {
        case AuditOutcome::Copied: return L"copied";
        case AuditOutcome::Kept: return L"kept";
        case AuditOutcome::Recycled: return L"recycled";
        case AuditOutcome::RecycledDuplicate: return L"recycled-as-duplicate";
        case AuditOutcome::Skipped: return L"skipped";
        case AuditOutcome::Deleted: return L"deleted";
    }
    return L"skipped";
}

// Reporting is deliberately a sink: it does not decide whether a copy or
// cleanup is allowed, and it never changes the ordering of those operations.
// Every outcome is log-only; popup-driven outcomes therefore do not produce a
// second notification, and popup-off keeps the mod's no-footprint contract.
static void AuditResult(const Settings& s, AuditOutcome outcome,
                        const wchar_t* reason, const std::wstring& path) {
    if (s.logDetails) {
        Wh_Log(L"Audit: %s; %s; %s", AuditOutcomeName(outcome), reason,
               path.c_str());
    } else {
        Wh_Log(L"Audit: %s; %s", AuditOutcomeName(outcome), reason);
    }
}

// ============================================================================
// Processing
// ============================================================================

// Windows 8 and later. Not declared in every SDK header, so define it locally.
#ifndef FOFX_RECYCLEONDELETE
#define FOFX_RECYCLEONDELETE 0x00080000
#endif

// Sends a file to the Recycle Bin via IFileOperation so a deletion is
// recoverable. Runs on the worker's STA COM thread. Returns false if the
// operation could not be carried out.
//
// FOF_ALLOWUNDO on its own is best effort: where undo information can't be kept
// (the bin turned off for the volume, a file over the bin's quota, a network or
// removable location) the item is deleted permanently instead, and FOF_NO_UI
// suppresses the confirmation that would normally warn about that. FOFX_RECYCLEONDELETE
// makes it recycle or fail. GetAnyOperationsAborted is checked too, because
// PerformOperations returns S_OK for "carried out or aborted" alike, so success
// on its own doesn't mean the file actually moved.
static bool RecycleFile(const std::wstring& path) {
    IFileOperation* op = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&op)))) {
        return false;
    }
    bool ok = false;
    if (SUCCEEDED(op->SetOperationFlags(FOF_ALLOWUNDO | FOF_NO_UI |
                                        FOFX_RECYCLEONDELETE))) {
        IShellItem* item = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                                  IID_PPV_ARGS(&item)))) {
            BOOL aborted = FALSE;
            if (SUCCEEDED(op->DeleteItem(item, nullptr)) &&
                SUCCEEDED(op->PerformOperations()) &&
                SUCCEEDED(op->GetAnyOperationsAborted(&aborted)) && !aborted) {
                ok = true;
            }
            item->Release();
        }
    }
    op->Release();
    return ok;
}

static bool HashFile(HANDLE file, std::array<BYTE, 32>& digest) {
    BCRYPT_HASH_HANDLE hash = nullptr;
    bool ok = false;
    do {
        if (!g_hashAlgorithm) break;
        // Windows 7 and later allocate and release the hash object when the
        // caller supplies a null buffer and zero length.
        if (BCryptCreateHash(g_hashAlgorithm, &hash, nullptr, 0, nullptr, 0, 0) != 0)
            break;
        LARGE_INTEGER start{};
        if (file == INVALID_HANDLE_VALUE ||
            !SetFilePointerEx(file, start, nullptr, FILE_BEGIN)) break;
        BY_HANDLE_FILE_INFORMATION info{};
        if (!GetFileInformationByHandle(file, &info) ||
            (info.dwFileAttributes & (FILE_ATTRIBUTE_REPARSE_POINT |
                                      FILE_ATTRIBUTE_DIRECTORY)) ||
            (info.nFileSizeHigh == 0 && info.nFileSizeLow == 0)) break;
        std::array<BYTE, 64 * 1024> buffer;
        for (;;) {
            if (WaitStop(0)) break;
            DWORD read = 0;
            if (!ReadFile(file, buffer.data(), (DWORD)buffer.size(), &read,
                          nullptr)) break;
            if (read == 0) {
                ok = BCryptFinishHash(hash, digest.data(), (ULONG)digest.size(),
                                      0) == 0;
                break;
            }
            if (BCryptHashData(hash, buffer.data(), read, 0) != 0) break;
        }
    } while (false);
    if (hash) BCryptDestroyHash(hash);
    return ok;
}

// Caller holds g_lock.
static void RememberRecentContent(const std::array<BYTE, 32>& hash,
                                  const std::wstring& path,
                                  ULONGLONG now, const BY_HANDLE_FILE_INFORMATION& identity) {
    g_recentContent.push_back({hash, path, now + kContentDuplicateWindowMs, identity});
    while (g_recentContent.size() > kMaxRecentContent)
        g_recentContent.pop_front();
}

static bool SeenRecentContentAndRemember(const std::array<BYTE, 32>& hash,
                                         const std::wstring& path,
                                         ULONGLONG now,
                                         std::wstring& keeper,
                                         BY_HANDLE_FILE_INFORMATION& keeperIdentity,
                                         const BY_HANDLE_FILE_INFORMATION& identity) {
    while (!g_recentContent.empty() && g_recentContent.front().expires <= now)
        g_recentContent.pop_front();
    for (auto it = g_recentContent.begin(); it != g_recentContent.end();) {
        if (it->hash != hash) {
            ++it;
            continue;
        }
        if (CompareStringOrdinal(it->keeper.c_str(), -1, path.c_str(), -1, TRUE) != CSTR_EQUAL) {
            keeper = it->keeper;
            keeperIdentity = it->identity;
            return true;
        }
        it = g_recentContent.erase(it);
    }
    RememberRecentContent(hash, path, now, identity);
    return false;
}

// True when the path is directly inside the folder being watched.
static bool IsDirectChildOf(const std::wstring& path, const std::wstring& folder) {
    std::wstring parent = folder;
    if (parent.size() > 1 && (parent.back() == L'\\' || parent.back() == L'/')) {
        parent.pop_back();
    }
    auto split = path.find_last_of(L"\\/");
    return split != std::wstring::npos &&
           _wcsicmp(path.substr(0, split).c_str(), parent.c_str()) == 0;
}

static bool SameIdentity(const BY_HANDLE_FILE_INFORMATION& a,
                         const BY_HANDLE_FILE_INFORMATION& b) {
    return a.dwVolumeSerialNumber == b.dwVolumeSerialNumber &&
        a.nFileIndexHigh == b.nFileIndexHigh && a.nFileIndexLow == b.nFileIndexLow;
}
static bool SameRevision(const BY_HANDLE_FILE_INFORMATION& a,
                         const BY_HANDLE_FILE_INFORMATION& b) {
    return SameIdentity(a, b) && a.nFileSizeHigh == b.nFileSizeHigh &&
        a.nFileSizeLow == b.nFileSizeLow &&
        CompareFileTime(&a.ftLastWriteTime, &b.ftLastWriteTime) == 0;
}
static bool PlainFileInfo(HANDLE file, BY_HANDLE_FILE_INFORMATION& info) {
    return GetFileInformationByHandle(file, &info) &&
        !(info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) &&
        info.nNumberOfLinks == 1;
}
static bool ObserveIdentity(const std::wstring& path, BY_HANDLE_FILE_INFORMATION& info) {
    HANDLE file = CreateFileW(path.c_str(), FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    const bool ok = PlainFileInfo(file, info);
    CloseHandle(file);
    return ok;
}

class LockedCapture {
public:
    HANDLE file = INVALID_HANDLE_VALUE;
    HANDLE folder = INVALID_HANDLE_VALUE;
    BY_HANDLE_FILE_INFORMATION info{};
    bool canDelete = false;
    LockedCapture() = default;
    LockedCapture(const LockedCapture&) = delete;
    LockedCapture& operator=(const LockedCapture&) = delete;
    ~LockedCapture() { Close(); }
    void CloseFile() {
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        file = INVALID_HANDLE_VALUE;
    }
    void Close() {
        CloseFile();
        if (folder != INVALID_HANDLE_VALUE) CloseHandle(folder);
        folder = INVALID_HANDLE_VALUE;
    }
    bool Open(const std::wstring& path, const std::wstring& parent,
               bool deletion = false) {
        Close();
        if (!IsDirectChildOf(path, parent) ||
            path.find(L':', path.find_last_of(L"\\/")) != std::wstring::npos) return false;
        folder = CreateFileW(parent.c_str(), FILE_READ_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
        BY_HANDLE_FILE_INFORMATION directory{};
        if (folder == INVALID_HANDLE_VALUE ||
            !GetFileInformationByHandle(folder, &directory) ||
            !(directory.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
            (directory.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
            Close(); return false;
        }
        canDelete = deletion;
        file = CreateFileW(path.c_str(), GENERIC_READ | (deletion ? DELETE : 0),
            FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
        if (file == INVALID_HANDLE_VALUE || !PlainFileInfo(file, info)) {
            Close(); return false;
        }
        return true;
    }
};

// FileRenameInfo with ReplaceIfExists=false acts on the held object and never
// overwrites a collision. Keep the parent pinned until any shell operation ends.
static bool RenameLocked(HANDLE file, const std::wstring& target) {
    const DWORD bytes = (DWORD)(target.size() * sizeof(wchar_t));
    // FileNameLength excludes the terminator, but the Win32 wrapper also needs
    // a terminated FileName while translating the path to its NT form.
    std::vector<BYTE> storage(offsetof(FILE_RENAME_INFO, FileName) + bytes + sizeof(wchar_t));
    auto* rename = reinterpret_cast<FILE_RENAME_INFO*>(storage.data());
    rename->ReplaceIfExists = FALSE;
    rename->RootDirectory = nullptr;
    rename->FileNameLength = bytes;
    memcpy(rename->FileName, target.data(), bytes);
    return SetFileInformationByHandle(file, FileRenameInfo, rename,
                                       (DWORD)storage.size()) != FALSE;
}

static AuditOutcome DeleteLocked(CaptureRecord& record, LockedCapture& capture,
                                 const Settings& settings, ULONGLONG generation) {
    if (!capture.canDelete || CleanupCancelled(0, generation)) return AuditOutcome::Kept;
    if (!settings.recycle) {
        FILE_DISPOSITION_INFO disposition{TRUE};
        if (!SetFileInformationByHandle(capture.file, FileDispositionInfo,
                                        &disposition, sizeof(disposition)))
            return AuditOutcome::Kept;
        capture.CloseFile();
        return AuditOutcome::Deleted;
    }
    // The shell recycles by path, and the Recycle Bin keeps the name the file had
    // at that moment, so the original path is recycled. ReOpenFile swaps the held
    // handle for one on the same file object that still denies writes but lets
    // the shell move it, with no gap where another file could take its place.
    HANDLE held = ReOpenFile(capture.file, GENERIC_READ,
                             FILE_SHARE_READ | FILE_SHARE_DELETE, 0);
    if (held == INVALID_HANDLE_VALUE) return AuditOutcome::Kept;
    capture.CloseFile();
    capture.file = held;
    if (CleanupCancelled(0, generation) || !RecycleFile(record.path)) {
        return AuditOutcome::Kept;
    }
    // Another program could still rename the checked file away and put a new
    // one at the same path just before the shell acts. That file would only go
    // to the Recycle Bin, where it can be restored, so this is a check and a log
    // line rather than a guarantee. The checked file is in the bin if its final
    // path now runs through the volume's $Recycle.Bin folder.
    DWORD length = GetFinalPathNameByHandleW(held, nullptr, 0, FILE_NAME_NORMALIZED);
    std::wstring finalPath(length ? length : 1, L'\0');
    DWORD got = length ? GetFinalPathNameByHandleW(held, finalPath.data(),
                                                   length, FILE_NAME_NORMALIZED)
                       : 0;
    finalPath.resize(got < length ? got : 0);
    static constexpr wchar_t kBin[] = L"\\$Recycle.Bin\\";
    bool inBin = false;
    for (size_t i = 0; !inBin && i + ARRAYSIZE(kBin) - 1 <= finalPath.size(); ++i) {
        inBin = CompareStringOrdinal(finalPath.c_str() + i, ARRAYSIZE(kBin) - 1,
                                     kBin, ARRAYSIZE(kBin) - 1,
                                     TRUE) == CSTR_EQUAL;
    }
    if (!inBin) {
        Wh_Log(L"The file at the screenshot's path was replaced while it was "
               L"being recycled; the replacement went to the Recycle Bin%s",
               settings.logDetails ? (L": " + record.path).c_str() : L"");
        return AuditOutcome::Kept;
    }
    return AuditOutcome::Recycled;
}

// Streaming comparison avoids retaining screenshot bytes in the cache. Both
// handles deny writes; the keeper also denies deletion until recycling finishes.
static bool SameFileContents(HANDLE a, HANDLE b) {
    LARGE_INTEGER start{};
    if (!SetFilePointerEx(a, start, nullptr, FILE_BEGIN) ||
        !SetFilePointerEx(b, start, nullptr, FILE_BEGIN)) return false;
    std::array<BYTE, 64 * 1024> left, right;
    for (;;) {
        if (WaitStop(0)) return false;
        DWORD n = 0, m = 0;
        if (!ReadFile(a, left.data(), (DWORD)left.size(), &n, nullptr) ||
            !ReadFile(b, right.data(), (DWORD)right.size(), &m, nullptr) ||
            n != m || memcmp(left.data(), right.data(), n) != 0) return false;
        if (!n) return true;
    }
}

struct PendingCleanup {
    CaptureRecord record;
    BY_HANDLE_FILE_INFORMATION revision{};
    std::array<BYTE, 32> digest{};
    std::wstring keeper;
    BY_HANDLE_FILE_INFORMATION keeperIdentity{};
    ULONGLONG due = 0;
    bool duplicate = false;
    bool recycle = true;
    Settings settings;
};
static std::deque<PendingCleanup> g_pendingCleanup;  // Processing thread only.
struct RetryCapture { CaptureRecord record; ULONGLONG due; };
static std::deque<RetryCapture> g_retryCaptures; // Bounded, processing thread only.
static constexpr size_t kMaxPendingPrompts = 64;
static std::deque<CaptureRecord> g_promptQueue;  // g_lock.

static void ProcessCleanup(PendingCleanup& job) {
    const auto& settings = job.settings;
    if (CleanupCancelled(0, job.record.generation)) {
        AuditResult(settings, AuditOutcome::Kept, L"cleanup cancelled", job.record.path);
        return;
    }
    LockedCapture incoming;
    std::array<BYTE, 32> digest;
    if (!incoming.Open(job.record.path, settings.folder, true) ||
        !SameRevision(job.revision, incoming.info) ||
        !HashFile(incoming.file, digest) || digest != job.digest) {
        AuditResult(settings, AuditOutcome::Kept, L"capture changed before cleanup", job.record.path);
        return;
    }
    LockedCapture keeper;
    if (job.duplicate && (!keeper.Open(job.keeper, settings.folder) ||
                         !SameIdentity(job.keeperIdentity, keeper.info) ||
                         !SameFileContents(incoming.file, keeper.file))) {
        // The incoming image is intact. Remember it as a possible future keeper.
        EnterCriticalSection(&g_lock);
        if (job.record.generation == g_generation.load()) {
            std::erase_if(g_recentContent, [&](const RecentContent& entry) {
                return CompareStringOrdinal(entry.keeper.c_str(), -1, job.keeper.c_str(), -1, TRUE) == CSTR_EQUAL;
            });
            RememberRecentContent(job.digest, job.record.path, GetTickCount64(), incoming.info);
        }
        LeaveCriticalSection(&g_lock);
        AuditResult(settings, AuditOutcome::Kept, L"earlier copy changed or disappeared", job.record.path);
        return;
    }
    Settings deletionSettings = settings;
    deletionSettings.recycle = job.recycle;
    AuditOutcome outcome = DeleteLocked(job.record, incoming, deletionSettings,
                                        job.record.generation);
    if (job.duplicate && outcome == AuditOutcome::Recycled)
        outcome = AuditOutcome::RecycledDuplicate;
    if (outcome == AuditOutcome::Kept && job.duplicate) {
        EnterCriticalSection(&g_lock);
        if (job.record.generation == g_generation.load())
            RememberRecentContent(job.digest, job.record.path, GetTickCount64(), job.revision);
        LeaveCriticalSection(&g_lock);
    }
    AuditResult(settings, outcome, job.duplicate ? L"exact duplicate cleanup" : L"capture cleanup",
                job.record.path);
}

static void RunDueCleanup() {
    const ULONGLONG now = GetTickCount64();
    // Jobs are not necessarily in due order: a popup action may overtake an
    // automatic countdown. Inspect the bounded queue without holding g_lock.
    for (auto it = g_pendingCleanup.begin(); it != g_pendingCleanup.end();) {
        if (WaitStop(0)) return;
        if (it->record.generation != g_generation.load() || it->due <= now) {
            PendingCleanup job = std::move(*it);
            it = g_pendingCleanup.erase(it);
            ProcessCleanup(job);
        } else ++it;
    }
}

static void QueuePrompt(const CaptureRecord& record, const Settings& settings) {
    bool queued = false;
    EnterCriticalSection(&g_lock);
    if (g_promptQueue.size() < kMaxPendingPrompts &&
        record.generation == g_generation.load()) {
        CaptureRecord prompt = record;
        prompt.prompted = true;
        g_promptQueue.push_back(std::move(prompt));
        queued = true;
    }
    LeaveCriticalSection(&g_lock);
    if (queued) SetEvent(g_promptEvent);
    else AuditResult(settings, AuditOutcome::Kept, L"prompt queue full or settings changed", record.path);
}

static std::wstring RenameObserved(CaptureRecord& record, LockedCapture& capture) {
    if (record.label.empty() || !capture.canDelete) return {};
    FILETIME local;
    SYSTEMTIME time;
    // Observation time was captured before any earlier prompt/countdown.
    if (!FileTimeToLocalFileTime(&record.observed, &local) ||
        !FileTimeToSystemTime(&local, &time)) return {};
    wchar_t stamp[32];
    swprintf_s(stamp, L"%04u-%02u-%02u %02u-%02u-%02u", time.wYear, time.wMonth,
               time.wDay, time.wHour, time.wMinute, time.wSecond);
    const auto slash = record.path.find_last_of(L"\\/");
    const auto dot = record.path.find_last_of(L'.');
    const auto extension = dot != std::wstring::npos && dot > slash ? record.path.substr(dot) : L"";
    const auto prefix = record.path.substr(0, slash + 1) + stamp + L" " + record.label;
    for (int n = 1; n <= 50; ++n) {
        const auto target = prefix + (n == 1 ? L"" : L" (" + std::to_wstring(n) + L")") + extension;
        if (target == record.path) return {};
        MarkNameRecent(target);
        if (RenameLocked(capture.file, target)) return target;
        if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != ERROR_FILE_EXISTS) break;
    }
    return {};
}

// Session functions are defined below; they never remove source captures.
static bool AddSessionCapture(const CaptureRecord&, const Settings&, LockedCapture&,
                               const std::array<BYTE, 32>&);
static void SessionCaptureFinished(ULONGLONG);

static void ProcessOne(CaptureRecord record) {
    struct Completion {
        ULONGLONG session;
        bool deferred = false;
        ~Completion() { if (session && !deferred) SessionCaptureFinished(session); }
    } completion{record.session};
    ULONGLONG generation;
    Settings settings = SnapshotSettings(&generation);
    if (record.generation != generation && record.action == -1 && !record.prompted &&
        IsDirectChildOf(record.path, settings.folder)) {
        // Nothing has been shown, copied or renamed. Preserve the existing
        // update behavior: apply the new settings to this unhandled capture.
        // Session captures carry over too, since they are never deleted.
        record.generation = generation;
    }
    if (record.generation != generation || !IsDirectChildOf(record.path, settings.folder) ||
        !record.identified || WaitStop(0)) {
        AuditResult(settings, AuditOutcome::Kept, L"capture cancelled or folder changed", record.path);
        return;
    }
    if (settings.popup && record.action == -1 && !record.session) {
        QueuePrompt(record, settings);
        return;
    }
    const int action = record.action == -1 ? ACTION_AUTO : record.action;
    if (action != ACTION_AUTO && action != ACTION_DELETE &&
        action != ACTION_COPY_DELETE && action != ACTION_COPY_ONLY) {
        AuditResult(settings, AuditOutcome::Kept, L"capture kept", record.path);
        return;
    }
    LockedCapture capture;
    // Read-only files must remain copyable even if cleanup/rename rights fail.
    bool needsDeleteAccess = settings.renameFromWindow || action == ACTION_DELETE ||
        action == ACTION_COPY_DELETE || settings.deleteFile || settings.removeExactDuplicates;
    const bool deleteOpened =
        needsDeleteAccess && capture.Open(record.path, settings.folder, true);
    // A thumbnailer or sync client can hold a new file briefly without letting
    // it be renamed or deleted. Wait about a second for it to let go, rather
    // than silently skipping the rename and cleanup. A file that never allows
    // it, such as a read-only one, is then still copied.
    if (!deleteOpened && needsDeleteAccess && record.deleteRetries < 10 &&
        g_retryCaptures.size() < kMaxPendingCaptures &&
        record.generation == g_generation.load() && !WaitStop(0)) {
        ++record.deleteRetries;
        completion.deferred = true;
        g_retryCaptures.push_back({std::move(record), GetTickCount64() + 100});
        return;
    }
    if (!deleteOpened && !capture.Open(record.path, settings.folder)) {
        if (record.retries < 40 && g_retryCaptures.size() < kMaxPendingCaptures &&
            record.generation == g_generation.load() && !WaitStop(0)) {
            ++record.retries;
            completion.deferred = true;
            g_retryCaptures.push_back({std::move(record), GetTickCount64() + 100});
            return;
        }
        AuditResult(settings, AuditOutcome::Kept, L"capture busy or unavailable", record.path);
        return;
    }
    if (!SameIdentity(record.identity, capture.info)) {
        AuditResult(settings, AuditOutcome::Kept, L"observed file was replaced", record.path);
        return;
    }
    const ULONGLONG fileBytes = ((ULONGLONG)capture.info.nFileSizeHigh << 32) | capture.info.nFileSizeLow;
    if (fileBytes > 512ULL * 1024 * 1024) {
        AuditResult(settings, AuditOutcome::Kept, L"capture exceeds processing size limit", record.path);
        return;
    }
    if (CleanupCancelled(0, generation)) return;
    if (settings.renameFromWindow && !record.session) {
        auto renamed = RenameObserved(record, capture);
        if (!renamed.empty()) record.path = std::move(renamed);
    }
    std::array<BYTE, 32> digest{};
    const bool fingerprinted = HashFile(capture.file, digest);
    const bool forceImage = action == ACTION_COPY_DELETE;
    bool multiFrame = false;
    bool copied = action == ACTION_DELETE; // Explicit Delete does not request a copy.
    g_clipboardGeneration = generation;
    if (action != ACTION_DELETE) {
        if (forceImage || settings.mode == L"image") {
            LARGE_INTEGER beginning{};
            SetFilePointerEx(capture.file, beginning, nullptr, FILE_BEGIN);
            const auto result = ClipboardImage(record.path, capture.file);
            copied = result != ImageCopy::Failed;
            multiFrame = result == ImageCopy::CopiedFirstFrameOnly;
        } else if (settings.mode == L"file") copied = ClipboardFile(record.path);
        else if (settings.mode == L"path") copied = ClipboardText(FormatPathText(record.path, settings.pathFormat));
        else copied = true;
    }
    if (record.session) {
        // Session ownership suppresses ordinary prompts and every deletion path,
        // including duplicate removal. Export always leaves originals in place.
        if (fingerprinted) AddSessionCapture(record, settings, capture, digest);
        AuditResult(settings, AuditOutcome::Kept, L"capture session keeps originals", record.path);
        return;
    }
    if (!copied || multiFrame || CleanupCancelled(0, generation)) {
        if (multiFrame && settings.popup) ShowKeptToast(BaseName(record.path));
        AuditResult(settings, AuditOutcome::Kept,
                    multiFrame ? L"only the first frame copied" : L"copy failed or action cancelled", record.path);
        return;
    }
    std::wstring keeper;
    BY_HANDLE_FILE_INFORMATION keeperIdentity{};
    bool duplicate = false;
    if (fingerprinted && action == ACTION_AUTO && settings.mode == L"image" &&
        settings.removeExactDuplicates) {
        EnterCriticalSection(&g_lock);
        if (generation == g_generation.load())
            duplicate = SeenRecentContentAndRemember(digest, record.path, GetTickCount64(), keeper, keeperIdentity, capture.info);
        LeaveCriticalSection(&g_lock);
    }
    const bool referencesFile = !forceImage && (settings.mode == L"file" || settings.mode == L"path");
    const bool requested = action != ACTION_COPY_ONLY && !referencesFile &&
        (action == ACTION_DELETE || forceImage || duplicate || (settings.deleteFile && !referencesFile));
    if (!requested || !fingerprinted ||
        g_pendingCleanup.size() >= kMaxPendingCaptures) {
        AuditResult(settings, AuditOutcome::Kept, L"copy complete; source kept", record.path);
        return;
    }
    PendingCleanup job;
    job.record = record;
    job.revision = capture.info;
    job.digest = digest;
    job.keeper = keeper;
    job.keeperIdentity = keeperIdentity;
    job.duplicate = duplicate;
    job.recycle = duplicate || settings.recycle;
    job.settings = settings;
    const DWORD delay = (record.action != -1 || action == ACTION_DELETE || forceImage)
        ? 0 : (DWORD)settings.delaySeconds * 1000;
    job.due = GetTickCount64() + delay;
    // Release the copy handle before scheduling. Cleanup reacquires and validates
    // identity AND bytes, then acts on that held object, not the observed name.
    capture.Close();
    g_pendingCleanup.push_back(std::move(job));
}

// ============================================================================
// Worker: drains the queue on a dedicated COM (STA) thread
// ============================================================================

static bool DequeueOne(CaptureRecord& path) {
    EnterCriticalSection(&g_lock);
    bool has = !g_queue.empty();
    if (has) {
        path = std::move(g_queue.front());
        g_queue.pop_front();
    }
    LeaveCriticalSection(&g_lock);
    return has;
}

// Caller must hold g_lock. Entries are appended in time order, so expired ones
// are always at the front; drop them, then report whether the name is still
// inside its dedup window.
static bool SeenRecently(const std::wstring& name, ULONGLONG now) {
    while (!g_recent.empty() && g_recent.front().second <= now) {
        g_recent.pop_front();
    }
    for (const auto& entry : g_recent) {
        if (CompareStringOrdinal(entry.first.c_str(), -1, name.c_str(), -1, TRUE) == CSTR_EQUAL) {
            return true;
        }
    }
    return false;
}

static void ReleaseInflight(const std::wstring& path) {
    ULONGLONG now = GetTickCount64();
    EnterCriticalSection(&g_lock);
    g_inflight.erase(path);
    g_recent.push_back({path, now + kDuplicateEventWindowMs});
    while (g_recent.size() > 1024) g_recent.pop_front();
    LeaveCriticalSection(&g_lock);
}

// Whether the Start Menu shortcut and the CLSID key are currently in place.
// Prompt thread only: the shortcut work reads and writes COM objects and needs
// that thread's apartment, which also hosts the toast activator.
static bool g_toastRegApplied = false;
// The first pass always runs, even when there is nothing to register. Windows can
// be shut down or the tool process killed without the mod ever unloading cleanly,
// which leaves the shortcut and the key behind; starting with the popup off is the
// one moment nothing would otherwise reclaim them.
static bool g_toastRegSynced = false;

// Clears anything SnapSentry left in Action Center, such as a kept-file
// notice. Must run while the AUMID is still registered: ClearWithId needs it
// to resolve which app's history to clear. Idempotent, so it is safe to call
// on both the unload path and the popup-off path.
static void ClearToastHistory() {
    using namespace ABI::Windows::UI::Notifications;
    using Microsoft::WRL::ComPtr;
    using Microsoft::WRL::Wrappers::HStringReference;
    ComPtr<IToastNotificationManagerStatics> statics;
    ComPtr<IToastNotificationManagerStatics2> statics2;
    ComPtr<IToastNotificationHistory> history;
    HRESULT hr = E_FAIL;
    if (SUCCEEDED(RoGetActivationFactory(
            HStringReference(
                RuntimeClass_Windows_UI_Notifications_ToastNotificationManager)
                .Get(),
            IID_PPV_ARGS(&statics))) &&
        SUCCEEDED(statics.As(&statics2)) &&
        SUCCEEDED(statics2->get_History(&history))) {
        hr = history->ClearWithId(HStringReference(kAppUserModelId).Get());
    }
    if (FAILED(hr)) {
        Wh_Log(L"ClearToastHistory failed (0x%08lx); a kept-file notice may "
               L"outlive the mod",
               hr);
    }
}

// The shortcut and the registry key exist for one reason, to let a toast button
// activate us, so they follow the settings instead of being written
// unconditionally at startup. With processing or the popup off there is no
// notification, and a SnapSentry entry sitting in Start and in Search would be
// there for nothing.
static void SyncToastRegistration(bool wantPopup, bool unloading) {
    // Clear Action Center and reclaim the per-AUMID notification key on unload,
    // unconditionally and before the early return below: with the popup already
    // off at unload the requested state matches, and we would otherwise return
    // without ever running either. The AUMID registration is still intact here,
    // which ClearToastHistory needs; RemoveNotificationSettings only touches the
    // separate per-app notification-settings key, not the AUMID itself.
    if (unloading) {
        ClearToastHistory();
        RemoveNotificationSettings();
    }
    // Skip only when nothing needs to change: the state matches and, when the popup
    // is wanted, registration already fully succeeded. A prior attempt that only
    // partially succeeded is retried on a later call rather than being stuck on the
    // dialog fallback for the whole session.
    if (g_toastRegSynced && wantPopup == g_toastRegApplied &&
        (!wantPopup || g_toastRegistered.load())) {
        return;
    }
    g_toastRegSynced = true;
    if (wantPopup) {
        g_toastRegApplied = true;  // Track intent, so teardown always cleans up.
        // Both helpers are idempotent, so retrying after a partial failure is safe.
        bool aumidOk = EnsureAumidRegistered();
        bool clsidOk = EnsureClsidRegistered();
        // Don't re-register the class object if a previous attempt already did.
        HRESULT regHr = g_toastActivatorCookie ? S_OK : E_FAIL;
        if (aumidOk && clsidOk && !g_toastActivatorCookie) {
            regHr = CoRegisterClassObject(
                CLSID_SnapSentryToastActivator, &g_toastActivatorFactory,
                CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &g_toastActivatorCookie);
        }
        g_toastRegistered = aumidOk && clsidOk && SUCCEEDED(regHr);
        if (g_toastRegistered.load()) {
            Wh_Log(L"Toast notifications ready");
            // Reclaim anything an unclean exit left in Action Center: Windows
            // persists it across reboots, and the popup-off first pass below is
            // the only other place that clears it, so with the popup on nothing
            // would. The AUMID is registered by this point, so the clear
            // resolves. Once a session is enough.
            static bool clearedStaleHistory = false;
            if (!clearedStaleHistory) {
                clearedStaleHistory = true;
                ClearToastHistory();
            }
            PrewarmToast();  // Avoid a slow first popup.
        } else {
            Wh_Log(
                L"Toast notification registration incomplete (aumid=%d clsid=%d "
                L"hr=0x%08lx); using the dialog instead",
                aumidOk, clsidOk, regHr);
        }
        return;
    }

    // Popup turned off, or the mod is unloading: hand everything back.
    g_toastRegistered = false;
    if (g_toastActivatorCookie) {
        CoRevokeClassObject(g_toastActivatorCookie);
        g_toastActivatorCookie = 0;
    }
    // Unload already cleared it above, while the notification key was still
    // there; repeating it here would run after RemoveNotificationSettings and
    // could fail for that reason alone. Skip it when nothing was ever
    // registered (the default popup-off first pass), since there is then no
    // AUMID for ClearWithId to resolve and the failure means nothing.
    if (!unloading && g_toastRegApplied) {
        ClearToastHistory();
    }
    RemoveAumidRegistration();
    RemoveClsidRegistration();
    // The per-AUMID Notifications\Settings key also holds the user's own per-app
    // choices (enabled, banner, sound), so it is reclaimed only on unload (handled
    // at the top of this function), never on an ordinary popup-off toggle, which
    // would silently reset a preference the user set in Windows Settings.
    g_toastRegApplied = false;
}

struct BitmapBuffer {
    UINT width = 0, height = 0;
    std::vector<BYTE> pixels;  // Top-down BGRA, at most kMaxDecodedBytes.
};
struct SessionItem {
    CaptureRecord record;
    BY_HANDLE_FILE_INFORMATION revision{};
    std::array<BYTE, 32> digest{};
    std::shared_ptr<const BitmapBuffer> thumbnail;
    std::wstring caption;
    bool included = true;
};
static constexpr size_t kMaxSessionItems = 64;
static std::vector<SessionItem> g_sessionItems;  // g_lock, worker + UI STA.
static ULONGLONG g_sessionSerial = 0;
static std::atomic<HWND> g_sessionWindow{nullptr};
static HWND g_sessionList, g_captionEdit, g_sessionStatus, g_sessionPreview;
static bool g_refreshingSession = false;
static bool g_previewRequested = false;
static std::atomic<bool> g_exportBusy{false};
static IFileDialog* g_openFolderPicker = nullptr;  // UI STA only.
static ULONGLONG g_pickerGeneration = 0;
static HFONT g_sessionFont = nullptr;
static UINT WindowDpi(HWND window) {
    using GetDpi_t = UINT(WINAPI*)(HWND);
    static auto getDpi = (GetDpi_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
    if (getDpi) {
        UINT dpi = getDpi(window);
        if (dpi)
            return dpi;
    }
    HDC dc = GetDC(window);
    UINT dpi = dc ? (UINT)GetDeviceCaps(dc, LOGPIXELSX) : 96;
    if (dc)
        ReleaseDC(window, dc);
    return dpi ? dpi : 96;
}
static BOOL CALLBACK SetSessionChildFont(HWND child, LPARAM font) {
    SendMessageW(child, WM_SETFONT, (WPARAM)font, TRUE);
    return TRUE;
}

static HRESULT CALLBACK
SessionResetCallback(HWND dialog, UINT message, WPARAM, LPARAM, LONG_PTR ref) {
    if (message == TDN_TIMER &&
        (WaitStop(0) || *(ULONGLONG*)ref != g_generation.load()))
        SendMessageW(dialog, TDM_CLICK_BUTTON, IDCANCEL, 0);
    return S_OK;
}

static void UpdateSessionFont(HWND window) {
    HFONT next = CreateFontW(-MulDiv(13, WindowDpi(window), 96), 0, 0, 0,
                             FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    if (!next)
        return;
    EnumChildWindows(window, SetSessionChildFont, (LPARAM)next);
    if (g_sessionFont)
        DeleteObject(g_sessionFont);
    g_sessionFont = next;
}
static std::wstring g_exportParent, g_exportStatus;
static std::vector<SessionItem> g_exportItems;
static ULONGLONG g_exportGeneration = 0;
static std::shared_ptr<const BitmapBuffer> g_readyPreview;
static constexpr UINT kSessionChanged = WM_APP + 20;
static constexpr UINT kSessionPreviewReady = WM_APP + 21;
static constexpr UINT kSessionExportFinished = WM_APP + 22;
static constexpr UINT kTrayMessage = WM_APP + 23;
enum {
    SESSION_LIST = 2001,
    SESSION_CAPTION,
    SESSION_UP,
    SESSION_DOWN,
    SESSION_EXPORT,
    SESSION_FINISH,
    SESSION_START,
    SESSION_CLOSE,
    SESSION_CAPTION_LABEL,
    SESSION_PREVIEW,
    SESSION_STATUS
};

static void SessionCaptureFinished(ULONGLONG session) {
    EnterCriticalSection(&g_lock);
    if (session == g_sessionSerial && g_sessionPending.load())
        --g_sessionPending;
    LeaveCriticalSection(&g_lock);
    PostMessageW(g_sessionWindow.load(), kSessionChanged, 0, 0);
}

static std::vector<SessionItem> SnapshotSession() {
    EnterCriticalSection(&g_lock);
    auto items = g_sessionItems;
    LeaveCriticalSection(&g_lock);
    return items;
}

static std::shared_ptr<BitmapBuffer> DecodeThumbnail(HANDLE file) {
    using Microsoft::WRL::ComPtr;
    ComPtr<IWICImagingFactory> factory;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICBitmapScaler> scaler;
    ComPtr<IWICFormatConverter> converter;
    LARGE_INTEGER zero{};
    if (!SetFilePointerEx(file, zero, nullptr, FILE_BEGIN) || WaitStop(0) ||
        FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&factory))) ||
        FAILED(factory->CreateDecoderFromFileHandle(
            (ULONG_PTR)file, nullptr, WICDecodeMetadataCacheOnDemand,
            &decoder)) ||
        FAILED(decoder->GetFrame(0, &frame)))
        return {};
    UINT width = 0, height = 0;
    if (FAILED(frame->GetSize(&width, &height)) || !width || !height ||
        width > 30000 || height > 30000 ||
        (ULONGLONG)width * height * 4 > kMaxDecodedBytes)
        return {};
    const double ratio = std::min(1.0, std::min(320.0 / width, 180.0 / height));
    auto result = std::make_shared<BitmapBuffer>();
    result->width = std::max(1u, (UINT)(width * ratio));
    result->height = std::max(1u, (UINT)(height * ratio));
    result->pixels.resize((size_t)result->width * result->height * 4);
    if (FAILED(factory->CreateBitmapScaler(&scaler)) ||
        FAILED(scaler->Initialize(frame.Get(), result->width, result->height,
                                  WICBitmapInterpolationModeFant)) ||
        FAILED(factory->CreateFormatConverter(&converter)) ||
        FAILED(converter->Initialize(scaler.Get(), GUID_WICPixelFormat32bppBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0,
                                     WICBitmapPaletteTypeCustom)) ||
        FAILED(converter->CopyPixels(nullptr, result->width * 4,
                                     (UINT)result->pixels.size(),
                                     result->pixels.data())))
        return {};
    return result;
}

static void RequestSessionPreview() {
    EnterCriticalSection(&g_lock);
    g_previewRequested = true;
    LeaveCriticalSection(&g_lock);
    SetEvent(g_workEvent);
}

static bool AddSessionCapture(const CaptureRecord& record,
                              const Settings&,
                              LockedCapture& capture,
                              const std::array<BYTE, 32>& digest) {
    auto thumb = DecodeThumbnail(capture.file);
    if (!thumb)
        return false;
    bool added = false;
    EnterCriticalSection(&g_lock);
    if (record.generation == g_generation.load() &&
        record.session == g_sessionSerial &&
        g_sessionItems.size() < kMaxSessionItems) {
        g_sessionItems.push_back({record, capture.info, digest,
                                  std::move(thumb), record.label, true});
        g_previewRequested = true;
        added = true;
    }
    LeaveCriticalSection(&g_lock);
    if (added) {
        PostMessageW(g_sessionWindow.load(), kSessionChanged, 0, 0);
        SetEvent(g_workEvent);
    }
    return added;
}

static std::shared_ptr<BitmapBuffer> BuildContactSheet(
    const std::vector<SessionItem>& items) {
    size_t count =
        std::count_if(items.begin(), items.end(),
                      [](const auto& item) { return item.included; });
    if (!count)
        return {};
    const UINT columns = (UINT)std::min<size_t>(3, count);
    constexpr UINT cellW = 360, cellH = 248;
    auto result = std::make_shared<BitmapBuffer>();
    result->width = columns * cellW;
    result->height = (UINT)((count + columns - 1) / columns) * cellH;
    const size_t bytes = (size_t)result->width * result->height * 4;
    if (bytes > kMaxDecodedBytes)
        return {};
    result->pixels.resize(bytes);
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER),
                      (LONG)result->width,
                      -(LONG)result->height,
                      1,
                      32,
                      BI_RGB,
                      (DWORD)bytes,
                      0,
                      0,
                      0,
                      0};
    void* pixels = nullptr;
    HDC dc = CreateCompatibleDC(nullptr);
    HBITMAP bitmap =
        dc ? CreateDIBSection(dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0)
           : nullptr;
    if (!bitmap) {
        if (dc)
            DeleteDC(dc);
        return {};
    }
    auto oldBitmap = SelectObject(dc, bitmap);
    RECT canvas{0, 0, (LONG)result->width, (LONG)result->height};
    HBRUSH background = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(dc, &canvas, background);
    DeleteObject(background);
    HFONT font =
        CreateFontW(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    auto oldFont = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(32, 32, 32));
    size_t index = 0;
    for (const auto& item : items) {
        if (!item.included)
            continue;
        if (WaitStop(0)) {
            SelectObject(dc, oldFont);
            DeleteObject(font);
            SelectObject(dc, oldBitmap);
            DeleteObject(bitmap);
            DeleteDC(dc);
            return {};
        }
        const LONG x = (LONG)(index % columns) * cellW;
        const LONG y = (LONG)(index / columns) * cellH;
        if (item.thumbnail) {
            const auto& image = *item.thumbnail;
            BITMAPINFO source{};
            source.bmiHeader = {sizeof(BITMAPINFOHEADER),
                                (LONG)image.width,
                                -(LONG)image.height,
                                1,
                                32,
                                BI_RGB,
                                0,
                                0,
                                0,
                                0,
                                0};
            StretchDIBits(dc, x + 20 + (320 - image.width) / 2,
                          y + 12 + (180 - image.height) / 2, image.width,
                          image.height, 0, 0, image.width, image.height,
                          image.pixels.data(), &source, DIB_RGB_COLORS,
                          SRCCOPY);
        }
        RECT label{x + 20, y + 200, x + 340, y + 240};
        const auto text =
            std::to_wstring(index + 1) + L". " +
            (item.caption.empty() ? BaseName(item.record.path) : item.caption);
        DrawTextW(dc, text.c_str(), (int)text.size(), &label,
                  DT_LEFT | DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX);
        ++index;
    }
    GdiFlush();
    memcpy(result->pixels.data(), pixels, bytes);
    for (size_t i = 3; i < bytes; i += 4)
        result->pixels[i] = 255;
    SelectObject(dc, oldFont);
    DeleteObject(font);
    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
    return result;
}

static bool WriteNewFile(const std::wstring&, const void*, size_t);
static bool EncodeContactSheet(const std::wstring& path,
                               const BitmapBuffer& image) {
    using Microsoft::WRL::ComPtr;
    ComPtr<IWICImagingFactory> factory;
    ComPtr<IStream> stream;
    ComPtr<IWICBitmapEncoder> encoder;
    ComPtr<IWICBitmapFrameEncode> frame;
    ComPtr<IPropertyBag2> options;
    WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
    bool ok =
        !WaitStop(0) &&
        SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&factory))) &&
        SUCCEEDED(CreateStreamOnHGlobal(nullptr, TRUE, &stream)) &&
        SUCCEEDED(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr,
                                         &encoder)) &&
        SUCCEEDED(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache)) &&
        SUCCEEDED(encoder->CreateNewFrame(&frame, &options)) &&
        SUCCEEDED(frame->Initialize(options.Get())) &&
        SUCCEEDED(frame->SetSize(image.width, image.height)) &&
        SUCCEEDED(frame->SetPixelFormat(&format)) &&
        IsEqualGUID(format, GUID_WICPixelFormat32bppBGRA) &&
        SUCCEEDED(frame->WritePixels(image.height, image.width * 4,
                                     (UINT)image.pixels.size(),
                                     (BYTE*)image.pixels.data())) &&
        SUCCEEDED(frame->Commit()) && SUCCEEDED(encoder->Commit());
    if (!ok || WaitStop(0))
        return false;
    STATSTG stat{};
    HGLOBAL memory = nullptr;
    if (FAILED(stream->Stat(&stat, STATFLAG_NONAME)) || stat.cbSize.HighPart ||
        !stat.cbSize.LowPart || stat.cbSize.LowPart > kMaxDecodedBytes ||
        FAILED(GetHGlobalFromStream(stream.Get(), &memory)))
        return false;
    void* bytes = GlobalLock(memory);
    if (!bytes)
        return false;
    ok = WriteNewFile(path, bytes, stat.cbSize.LowPart);
    GlobalUnlock(memory);
    return ok;
}

static std::wstring FinalFolderName(HANDLE handle) {
    DWORD length =
        GetFinalPathNameByHandleW(handle, nullptr, 0, FILE_NAME_NORMALIZED);
    if (!length)
        return {};
    std::wstring name(length + 1, L'\0');
    DWORD got = GetFinalPathNameByHandleW(
        handle, name.data(), (DWORD)name.size(), FILE_NAME_NORMALIZED);
    if (!got || got >= name.size())
        return {};
    name.resize(got);
    while (!name.empty() && name.back() == L'\\')
        name.pop_back();
    return name;
}
static bool NestedFolder(const std::wstring& child,
                         const std::wstring& parent) {
    return child.size() >= parent.size() &&
           CompareStringOrdinal(child.c_str(), (int)parent.size(),
                                parent.c_str(), (int)parent.size(),
                                TRUE) == CSTR_EQUAL &&
           (child.size() == parent.size() || child[parent.size()] == L'\\');
}
static std::string Utf8(const std::wstring& value) {
    int size = WideCharToMultiByte(CP_UTF8, 0, value.c_str(), (int)value.size(),
                                   nullptr, 0, nullptr, nullptr);
    std::string result(size, '\0');
    if (size)
        WideCharToMultiByte(CP_UTF8, 0, value.c_str(), (int)value.size(),
                            result.data(), size, nullptr, nullptr);
    return result;
}
static std::wstring MarkdownCaption(const std::wstring& value) {
    std::wstring result;
    for (wchar_t c : value) {
        if (c == L'\r' || c == L'\n') {
            result += L' ';
            continue;
        }
        if (wcschr(L"\\`*_{}[]<>()!#|", c))
            result += L'\\';
        if (c >= 32)
            result += c;
    }
    return result;
}
static bool WriteNewFile(const std::wstring& path,
                         const void* data,
                         size_t bytes) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return false;
    DWORD written = 0;
    const bool ok = bytes <= MAXDWORD &&
                    WriteFile(file, data, (DWORD)bytes, &written, nullptr) &&
                    written == bytes && FlushFileBuffers(file);
    CloseHandle(file);
    return ok;
}
// An export is cancelled by unloading or by watching a different folder, not by
// any settings change: toggling logging mid-export should not leave a packet
// marked incomplete. Same rule as a recording session (see LoadSettings).
struct ExportGuard {
    ULONGLONG generation;
    std::wstring folder;
    bool Cancelled() {
        if (WaitStop(0))
            return true;
        const ULONGLONG current = g_generation.load();
        if (current == generation)
            return false;
        EnterCriticalSection(&g_lock);
        const bool sameFolder =
            CompareStringOrdinal(g_settings.folder.c_str(), -1, folder.c_str(),
                                 -1, TRUE) == CSTR_EQUAL;
        LeaveCriticalSection(&g_lock);
        if (sameFolder)
            generation = current;
        return !sameFolder;
    }
};

static bool CopyLockedFile(HANDLE source,
                           const std::wstring& destination,
                           ExportGuard& guard) {
    HANDLE target = CreateFileW(destination.c_str(), GENERIC_WRITE, 0, nullptr,
                                CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (target == INVALID_HANDLE_VALUE)
        return false;
    LARGE_INTEGER zero{};
    bool ok = SetFilePointerEx(source, zero, nullptr, FILE_BEGIN) != FALSE;
    std::array<BYTE, 64 * 1024> buffer;
    while (ok && !guard.Cancelled()) {
        DWORD read = 0, written = 0;
        if (!ReadFile(source, buffer.data(), (DWORD)buffer.size(), &read,
                      nullptr)) {
            ok = false;
            break;
        }
        if (!read)
            break;
        if (!WriteFile(target, buffer.data(), read, &written, nullptr) ||
            written != read) {
            ok = false;
            break;
        }
    }
    ok = ok && !guard.Cancelled() && FlushFileBuffers(target);
    CloseHandle(target);
    return ok;
}

static bool ExportSession(const std::vector<SessionItem>& items,
                          const std::wstring& destination,
                          const Settings& settings,
                          ULONGLONG generation,
                          std::wstring& result) {
    HANDLE parent = CreateFileW(
        destination.c_str(), FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    HANDLE watched = CreateFileW(
        settings.folder.c_str(), FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    BY_HANDLE_FILE_INFORMATION info{};
    bool valid = parent != INVALID_HANDLE_VALUE &&
                 watched != INVALID_HANDLE_VALUE &&
                 GetFileInformationByHandle(parent, &info) &&
                 (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                 !(info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
    const auto parentName = valid ? FinalFolderName(parent) : L"";
    const auto watchedName = valid ? FinalFolderName(watched) : L"";
    valid = valid && !parentName.empty() && !watchedName.empty() &&
            !NestedFolder(parentName, watchedName);
    if (watched != INVALID_HANDLE_VALUE)
        CloseHandle(watched);
    if (!valid) {
        if (parent != INVALID_HANDLE_VALUE)
            CloseHandle(parent);
        result =
            L"Choose an accessible folder outside the watched folder. "
            L"Originals were kept.";
        return false;
    }
    GUID id;
    wchar_t text[40];
    if (FAILED(CoCreateGuid(&id)) ||
        !StringFromGUID2(id, text, ARRAYSIZE(text))) {
        CloseHandle(parent);
        result = L"Could not create an export name. Originals were kept.";
        return false;
    }
    const auto packet = destination + L"\\SnapSentry-" + text;
    if (!CreateDirectoryW(packet.c_str(), nullptr)) {
        CloseHandle(parent);
        result =
            L"Could not create the packet folder. Check permissions and free "
            L"space.";
        return false;
    }
    HANDLE packetHandle =
        CreateFileW(packet.c_str(), FILE_READ_ATTRIBUTES,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                    FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    bool ok = packetHandle != INVALID_HANDLE_VALUE;
    ExportGuard guard{generation, settings.folder};
    std::wstring markdown =
        L"# Screenshot session\n\n![Contact sheet](contact-sheet.png)\n\n";
    size_t number = 0;
    ULONGLONG total = 0;
    for (const auto& item : items) {
        if (!item.included || !ok)
            continue;
        if (guard.Cancelled()) {
            ok = false;
            break;
        }
        LockedCapture capture;
        std::array<BYTE, 32> digest;
        if (!capture.Open(item.record.path, settings.folder) ||
            !SameRevision(item.revision, capture.info) ||
            !HashFile(capture.file, digest) || digest != item.digest) {
            ok = false;
            break;
        }
        total += ((ULONGLONG)capture.info.nFileSizeHigh << 32) |
                 capture.info.nFileSizeLow;
        if (total > 1024ULL * 1024 * 1024) {
            ok = false;
            break;
        }
        const auto dot = item.record.path.find_last_of(L'.');
        std::wstring extension =
            dot == std::wstring::npos ? L".png" : item.record.path.substr(dot);
        // Only supported image suffixes from a plain observed filename enter
        // here.
        if (!IsSupportedImage(L"image" + extension) ||
            extension.find_first_of(L"\\/: ") != std::wstring::npos) {
            ok = false;
            break;
        }
        wchar_t filename[32];
        swprintf_s(filename, L"%02u%s", (unsigned)++number, extension.c_str());
        if (!CopyLockedFile(capture.file, packet + L"\\" + filename, guard)) {
            ok = false;
            break;
        }
        markdown +=
            L"## " + std::to_wstring(number) + L". " +
            MarkdownCaption(item.caption.empty() ? BaseName(item.record.path)
                                                 : item.caption) +
            L"\n\n![Screenshot " + std::to_wstring(number) + L"](" + filename +
            L")\n\n";
    }
    auto sheet = ok && number ? BuildContactSheet(items) : nullptr;
    ok = ok && sheet && !guard.Cancelled() &&
         EncodeContactSheet(packet + L"\\contact-sheet.png", *sheet);
    if (ok) {
        const auto utf8 = Utf8(markdown);
        ok = !guard.Cancelled() &&
             WriteNewFile(packet + L"\\index.md", utf8.data(), utf8.size());
    }
    if (!ok) {
        static const char message[] =
            "Export incomplete. Original screenshots were not changed. Retry "
            "into a new packet.\r\n";
        WriteNewFile(packet + L"\\INCOMPLETE.txt", message,
                     sizeof(message) - 1);
    }
    if (packetHandle != INVALID_HANDLE_VALUE)
        CloseHandle(packetHandle);
    CloseHandle(parent);
    result =
        ok ? L"Exported " + std::to_wstring(number) + L" images to " + packet
           : L"Export incomplete. A source changed, access failed, or export "
             L"was cancelled. Originals were kept. Check the packet folder and "
             L"retry.";
    return ok;
}

// Preview rendering is small and stays on the worker.
static void RunSessionWork() {
    EnterCriticalSection(&g_lock);
    const bool preview = std::exchange(g_previewRequested, false);
    LeaveCriticalSection(&g_lock);
    if (preview) {
        auto sheet = BuildContactSheet(SnapshotSession());
        EnterCriticalSection(&g_lock);
        g_readyPreview = std::move(sheet);
        LeaveCriticalSection(&g_lock);
        PostMessageW(g_sessionWindow.load(), kSessionPreviewReady, 0, 0);
    }
}

// Export copies and hashes up to 1 GB, so it runs on its own thread and new
// screenshots keep being copied meanwhile. It stops on unload like the other
// work (ExportSession checks the stop event), and the worker joins it before
// closing the hash provider they share.
static std::atomic<HANDLE> g_exportThread{nullptr};

static void JoinExportThread() {
    if (HANDLE thread = g_exportThread.exchange(nullptr)) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

static DWORD WINAPI ExportThread(LPVOID) {
    const HRESULT com = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    EnterCriticalSection(&g_lock);
    std::vector<SessionItem> items = std::move(g_exportItems);
    g_exportItems.clear();
    const std::wstring parent = g_exportParent;
    const ULONGLONG generation = g_exportGeneration;
    LeaveCriticalSection(&g_lock);
    std::wstring status = L"Export could not start. Originals were kept.";
    if (SUCCEEDED(com)) {
        ExportSession(items, parent, SnapshotSettings(), generation, status);
    }
    EnterCriticalSection(&g_lock);
    g_exportStatus = std::move(status);
    g_exportBusy = false;
    LeaveCriticalSection(&g_lock);
    if (HWND window = g_sessionWindow.load()) {
        PostMessageW(window, kSessionExportFinished, 0, 0);
    }
    if (SUCCEEDED(com))
        CoUninitialize();
    return 0;
}

static int SelectedSessionItem() {
    return ListView_GetNextItem(g_sessionList, -1, LVNI_SELECTED);
}

static void RefreshSessionList() {
    const auto items = SnapshotSession();
    const int selected = SelectedSessionItem();
    HWND window = g_sessionWindow.load();
    g_refreshingSession = true;
    ListView_DeleteAllItems(g_sessionList);
    for (size_t i = 0; i < items.size(); ++i) {
        auto number = std::to_wstring(i + 1);
        LVITEMW item{};
        item.mask = LVIF_TEXT;
        item.iItem = (int)i;
        item.pszText = number.data();
        ListView_InsertItem(g_sessionList, &item);
        auto caption = items[i].caption.empty() ? BaseName(items[i].record.path)
                                                : items[i].caption;
        ListView_SetItemText(g_sessionList, (int)i, 1, caption.data());
        ListView_SetCheckState(g_sessionList, (int)i, items[i].included);
    }
    const int row =
        items.empty() ? -1 : std::clamp(selected, 0, (int)items.size() - 1);
    if (row >= 0) {
        ListView_SetItemState(g_sessionList, row, LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);
    }
    SetWindowTextW(g_captionEdit, row >= 0 ? items[row].caption.c_str() : L"");
    g_refreshingSession = false;

    const bool busy = g_exportBusy;
    const bool recording = g_activeSession.load() != 0;
    EnableWindow(g_captionEdit, row >= 0 && !busy);
    EnableWindow(g_sessionList, !busy);
    EnableWindow(GetDlgItem(window, SESSION_UP), row > 0 && !busy);
    EnableWindow(GetDlgItem(window, SESSION_DOWN),
                 row >= 0 && row + 1 < (int)items.size() && !busy);
    SetWindowTextW(window, recording ? L"SnapSentry - Recording session"
                                     : L"SnapSentry - Review session");
    const std::wstring status =
        items.size() == kMaxSessionItems && recording
            ? L"Session full at 64 images. Extra screenshots are kept outside "
              L"the packet. Finish and review to export."
            : std::to_wstring(items.size()) +
                  L" / 64 captures. Check images to include. Originals are "
                  L"always kept.";
    SetWindowTextW(g_sessionStatus, status.c_str());
    EnableWindow(GetDlgItem(window, SESSION_FINISH), recording);
    EnableWindow(
        GetDlgItem(window, SESSION_EXPORT),
        !items.empty() && !busy && !recording && !g_sessionPending.load());
}

static void LayoutSession(HWND window) {
    RECT bounds;
    GetClientRect(window, &bounds);
    const UINT dpi = WindowDpi(window);
    auto px = [=](int value) { return MulDiv(value, dpi, 96); };
    auto move = [](HWND child, int x, int y, int w, int h) {
        MoveWindow(child, x, y, std::max(1, w), std::max(1, h), TRUE);
    };
    const int width = bounds.right;
    const int height = bounds.bottom;
    const int margin = px(16);
    const int gap = px(8);
    const int buttonH = px(30);

    move(GetDlgItem(window, SESSION_START), margin, margin, px(130), buttonH);
    move(GetDlgItem(window, SESSION_FINISH), margin + px(138), margin, px(140),
         buttonH);

    // Side by side on a wide window, list above preview on a narrow one.
    const bool stacked = width < px(820);
    const int top = margin + buttonH + gap;
    const int bottom = height - margin - px(118);
    const int listW =
        stacked ? width - 2 * margin : (width - 3 * margin) * 52 / 100;
    const int listH =
        stacked ? std::max(px(100), (bottom - top) * 50 / 100) : bottom - top;
    move(g_sessionList, margin, top, listW, listH);
    if (stacked) {
        move(g_sessionPreview, margin, top + listH + gap, listW,
             bottom - top - listH - gap);
    } else {
        move(g_sessionPreview, 2 * margin + listW, top,
             width - 3 * margin - listW, bottom - top);
    }
    ListView_SetColumnWidth(g_sessionList, 0, px(64));
    ListView_SetColumnWidth(g_sessionList, 1,
                            std::max(px(100), listW - px(90)));

    move(GetDlgItem(window, SESSION_CAPTION_LABEL), margin, bottom + gap,
         px(70), px(24));
    move(g_captionEdit, margin + px(76), bottom + gap,
         width - 2 * margin - px(76), px(26));
    move(GetDlgItem(window, SESSION_UP), margin, bottom + px(40), px(95),
         buttonH);
    move(GetDlgItem(window, SESSION_DOWN), margin + px(103), bottom + px(40),
         px(105), buttonH);
    move(GetDlgItem(window, SESSION_EXPORT), width - margin - px(224),
         bottom + px(40), px(130), buttonH);
    move(GetDlgItem(window, SESSION_CLOSE), width - margin - px(86),
         bottom + px(40), px(86), buttonH);
    move(g_sessionStatus, margin, height - margin - px(38), width - 2 * margin,
         px(38));
}

static void StartSession(HWND owner) {
    const Settings settings = SnapshotSettings();
    if (!settings.sessions || g_exportBusy) {
        return;
    }
    if (!SnapshotSession().empty()) {
        TASKDIALOGCONFIG prompt{sizeof(prompt)};
        prompt.hwndParent = owner;
        const TASKDIALOG_BUTTON buttons[] = {
            {SESSION_START, L"Start session"},
            {IDCANCEL, L"Keep current session"},
        };
        prompt.cButtons = ARRAYSIZE(buttons);
        prompt.pButtons = buttons;
        prompt.nDefaultButton = IDCANCEL;
        prompt.dwFlags = TDF_CALLBACK_TIMER | TDF_ALLOW_DIALOG_CANCELLATION;
        ULONGLONG generation = g_generation.load();
        prompt.lpCallbackData = (LONG_PTR)&generation;
        prompt.pfCallback = SessionResetCallback;
        prompt.pszWindowTitle = L"SnapSentry";
        prompt.pszMainInstruction = L"Start a new session?";
        prompt.pszContent =
            L"The current review list and captions will be cleared. All "
            L"original screenshots stay on disk.";
        int result = IDCANCEL;
        if (FAILED(TaskDialogIndirect(&prompt, &result, nullptr, nullptr)) ||
            result != SESSION_START) {
            return;
        }
    }
    EnterCriticalSection(&g_lock);
    g_sessionItems.clear();
    g_readyPreview.reset();
    ++g_sessionSerial;
    g_sessionPending.store(0);
    g_activeSession.store(g_sessionSerial);
    LeaveCriticalSection(&g_lock);
    RefreshSessionList();
    InvalidateRect(g_sessionPreview, nullptr, TRUE);
}

static void ChooseExportFolder(HWND owner) {
    if (g_exportBusy || g_activeSession.load() || g_sessionPending.load()) {
        return;
    }
    const auto current = SnapshotSession();
    if (std::none_of(current.begin(), current.end(),
                     [](const auto& item) { return item.included; })) {
        SetWindowTextW(g_sessionStatus,
                       L"Check at least one image to export. Originals are "
                       L"kept.");
        return;
    }
    using Microsoft::WRL::ComPtr;
    ComPtr<IFileOpenDialog> picker;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&picker)))) {
        return;
    }
    picker->SetOptions(FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM |
                       FOS_PATHMUSTEXIST | FOS_NOCHANGEDIR);
    picker->SetTitle(L"Choose a folder outside the watched screenshots folder");
    g_openFolderPicker = picker.Get();
    g_pickerGeneration = g_generation.load();
    // Timer 2 closes the picker if the mod unloads or settings change.
    SetTimer(owner, 2, 200, nullptr);
    const HRESULT picked = picker->Show(owner);
    KillTimer(owner, 2);
    g_openFolderPicker = nullptr;
    if (FAILED(picked) || g_pickerGeneration != g_generation.load() ||
        WaitStop(0)) {
        return;
    }
    ComPtr<IShellItem> folder;
    PWSTR name = nullptr;
    if (FAILED(picker->GetResult(&folder)) ||
        FAILED(folder->GetDisplayName(SIGDN_FILESYSPATH, &name))) {
        return;
    }
    bool start = false;
    EnterCriticalSection(&g_lock);
    if (!g_exportBusy && !g_activeSession.load() && !g_sessionPending.load()) {
        g_exportParent = name;
        g_exportItems = g_sessionItems;
        g_exportGeneration = g_generation.load();
        g_exportBusy = true;
        start = true;
    }
    LeaveCriticalSection(&g_lock);
    CoTaskMemFree(name);
    if (!start) {
        return;
    }
    // The previous export has finished (g_exportBusy was clear), so this only
    // releases its handle.
    JoinExportThread();
    // Created suspended so its handle is stored before it can run. Otherwise an
    // unload in between would find nothing to join and close the hash provider
    // while the export is already hashing.
    HANDLE thread = CreateThread(nullptr, 0, ExportThread, nullptr,
                                 CREATE_SUSPENDED, nullptr);
    if (!thread) {
        EnterCriticalSection(&g_lock);
        g_exportBusy = false;
        g_exportItems.clear();
        LeaveCriticalSection(&g_lock);
        SetWindowTextW(g_sessionStatus,
                       L"Export could not start. Originals were kept.");
        return;
    }
    g_exportThread.store(thread);
    ResumeThread(thread);
    SetWindowTextW(g_sessionStatus,
                   L"Exporting copies. Originals are kept. You can close this "
                   L"window.");
    EnableWindow(GetDlgItem(owner, SESSION_EXPORT), FALSE);
    EnableWindow(GetDlgItem(owner, SESSION_START), FALSE);
    EnableWindow(g_sessionList, FALSE);
    EnableWindow(g_captionEdit, FALSE);
    EnableWindow(GetDlgItem(owner, SESSION_UP), FALSE);
    EnableWindow(GetDlgItem(owner, SESSION_DOWN), FALSE);
}

static LRESULT CALLBACK PreviewWindowProc(HWND window,
                                          UINT message,
                                          WPARAM wp,
                                          LPARAM lp) {
    if (message != WM_PAINT && message != WM_PRINTCLIENT) {
        return DefWindowProcW(window, message, wp, lp);
    }
    PAINTSTRUCT paint{};
    HDC dc = message == WM_PAINT ? BeginPaint(window, &paint) : (HDC)wp;
    RECT area;
    GetClientRect(window, &area);
    FillRect(dc, &area, GetSysColorBrush(COLOR_WINDOW));
    EnterCriticalSection(&g_lock);
    auto image = g_readyPreview;
    LeaveCriticalSection(&g_lock);
    if (image) {
        // Fit the contact sheet inside the pane, keeping its proportions.
        const double scale = std::min((double)area.right / image->width,
                                      (double)area.bottom / image->height);
        const int width = (int)(image->width * scale);
        const int height = (int)(image->height * scale);
        BITMAPINFO info{};
        info.bmiHeader = {sizeof(BITMAPINFOHEADER),
                          (LONG)image->width,
                          -(LONG)image->height,
                          1,
                          32,
                          BI_RGB,
                          0,
                          0,
                          0,
                          0,
                          0};
        SetStretchBltMode(dc, HALFTONE);
        StretchDIBits(dc, (area.right - width) / 2, (area.bottom - height) / 2,
                      width, height, 0, 0, image->width, image->height,
                      image->pixels.data(), &info, DIB_RGB_COLORS, SRCCOPY);
    } else {
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));
        DrawTextW(dc,
                  L"Contact sheet preview\nStart a session and take a "
                  L"screenshot.",
                  -1, &area, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);
    }
    if (message == WM_PAINT) {
        EndPaint(window, &paint);
    }
    return 0;
}

static void CreateSessionControls(HWND window) {
    auto control = [&](PCWSTR type, PCWSTR text, DWORD style, int id,
                       DWORD exStyle = 0) {
        HWND child = CreateWindowExW(
            exStyle, type, text, WS_CHILD | WS_VISIBLE | style, 0, 0, 1, 1,
            window, (HMENU)(INT_PTR)id, GetModuleHandleW(nullptr), nullptr);
        SendMessageW(child, WM_SETFONT,
                     (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        return child;
    };
    control(L"BUTTON", L"Start session", WS_TABSTOP, SESSION_START);
    control(L"BUTTON", L"Finish and review", WS_TABSTOP, SESSION_FINISH);
    g_sessionList =
        control(WC_LISTVIEWW, L"Screenshots",
                WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
                SESSION_LIST, WS_EX_CLIENTEDGE);
    ListView_SetExtendedListViewStyle(g_sessionList, LVS_EX_CHECKBOXES |
                                                         LVS_EX_FULLROWSELECT |
                                                         LVS_EX_DOUBLEBUFFER);
    LVCOLUMNW column{};
    column.mask = LVCF_TEXT | LVCF_WIDTH;
    column.pszText = (PWSTR)L"Include";
    column.cx = 64;
    ListView_InsertColumn(g_sessionList, 0, &column);
    column.pszText = (PWSTR)L"Caption";
    column.cx = 320;
    ListView_InsertColumn(g_sessionList, 1, &column);
    g_sessionPreview = control(L"SnapSentryPreview", L"Contact sheet", 0,
                               SESSION_PREVIEW, WS_EX_CLIENTEDGE);
    control(L"STATIC", L"&Caption", 0, SESSION_CAPTION_LABEL);
    g_captionEdit = control(L"EDIT", L"", WS_TABSTOP | ES_AUTOHSCROLL,
                            SESSION_CAPTION, WS_EX_CLIENTEDGE);
    SendMessageW(g_captionEdit, EM_SETLIMITTEXT, 240, 0);
    control(L"BUTTON", L"Move &up", WS_TABSTOP, SESSION_UP);
    control(L"BUTTON", L"Move &down", WS_TABSTOP, SESSION_DOWN);
    control(L"BUTTON", L"&Export packet...", WS_TABSTOP | BS_DEFPUSHBUTTON,
            SESSION_EXPORT);
    control(L"BUTTON", L"Close", WS_TABSTOP, SESSION_CLOSE);
    g_sessionStatus = control(L"STATIC", L"", SS_LEFT, SESSION_STATUS);
    UpdateSessionFont(window);
}

// The list's check boxes include or exclude an item, and selecting a row
// shows its caption for editing.
static void OnSessionListChanged(HWND window, const NMLISTVIEW* change) {
    const int selected = SelectedSessionItem();
    EnterCriticalSection(&g_lock);
    if (change->iItem >= 0 && (size_t)change->iItem < g_sessionItems.size()) {
        g_sessionItems[change->iItem].included =
            ListView_GetCheckState(g_sessionList, change->iItem) != 0;
    }
    std::wstring caption =
        selected >= 0 && (size_t)selected < g_sessionItems.size()
            ? g_sessionItems[selected].caption
            : L"";
    LeaveCriticalSection(&g_lock);
    g_refreshingSession = true;
    SetWindowTextW(g_captionEdit, caption.c_str());
    g_refreshingSession = false;
    const int count = ListView_GetItemCount(g_sessionList);
    EnableWindow(g_captionEdit, selected >= 0 && !g_exportBusy);
    EnableWindow(GetDlgItem(window, SESSION_UP), selected > 0 && !g_exportBusy);
    EnableWindow(GetDlgItem(window, SESSION_DOWN),
                 selected >= 0 && selected + 1 < count && !g_exportBusy);
    RequestSessionPreview();
}

static void MoveSessionItem(int offset) {
    const int from = SelectedSessionItem();
    const int to = from + offset;
    EnterCriticalSection(&g_lock);
    const bool valid =
        from >= 0 && to >= 0 && (size_t)to < g_sessionItems.size();
    if (valid) {
        std::swap(g_sessionItems[from], g_sessionItems[to]);
    }
    LeaveCriticalSection(&g_lock);
    if (!valid) {
        return;
    }
    RefreshSessionList();
    ListView_SetItemState(g_sessionList, from, 0, LVIS_SELECTED);
    ListView_SetItemState(g_sessionList, to, LVIS_SELECTED | LVIS_FOCUSED,
                          LVIS_SELECTED | LVIS_FOCUSED);
    ListView_EnsureVisible(g_sessionList, to, FALSE);
    RequestSessionPreview();
}

static void OnCaptionEdited(HWND window) {
    wchar_t caption[241];
    GetWindowTextW(g_captionEdit, caption, ARRAYSIZE(caption));
    const int index = SelectedSessionItem();
    EnterCriticalSection(&g_lock);
    if (index >= 0 && (size_t)index < g_sessionItems.size()) {
        g_sessionItems[index].caption = caption;
    }
    LeaveCriticalSection(&g_lock);
    if (index >= 0) {
        ListView_SetItemText(g_sessionList, index, 1, caption);
    }
    // Timer 1 redraws the preview once typing pauses.
    SetTimer(window, 1, 300, nullptr);
}

static void HideSessionWindow(HWND window) {
    g_activeSession.store(0);
    RefreshSessionList();
    ShowWindow(window, SW_HIDE);
}

static LRESULT CALLBACK SessionWindowProc(HWND window,
                                          UINT message,
                                          WPARAM wp,
                                          LPARAM lp) {
    switch (message) {
        case WM_CREATE:
            CreateSessionControls(window);
            return 0;

        case WM_SIZE:
            LayoutSession(window);
            return 0;

        case WM_DPICHANGED: {
            const RECT* rect = (const RECT*)lp;
            SetWindowPos(window, nullptr, rect->left, rect->top,
                         rect->right - rect->left, rect->bottom - rect->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            UpdateSessionFont(window);
            LayoutSession(window);
            return 0;
        }

        case WM_GETMINMAXINFO: {
            const UINT dpi = WindowDpi(window);
            auto* info = (MINMAXINFO*)lp;
            info->ptMinTrackSize = {MulDiv(640, dpi, 96), MulDiv(520, dpi, 96)};
            return 0;
        }

        case WM_NOTIFY: {
            const auto* header = (const NMHDR*)lp;
            if (header->idFrom == SESSION_LIST &&
                header->code == LVN_ITEMCHANGED && !g_refreshingSession) {
                OnSessionListChanged(window, (const NMLISTVIEW*)lp);
            }
            return 0;
        }

        case WM_COMMAND: {
            const int command = LOWORD(wp);
            if (command == SESSION_START) {
                StartSession(window);
            } else if (command == SESSION_FINISH) {
                g_activeSession.store(0);
                RefreshSessionList();
            } else if (command == SESSION_CLOSE || command == IDCANCEL) {
                HideSessionWindow(window);
            } else if (command == SESSION_EXPORT) {
                ChooseExportFolder(window);
            } else if (command == SESSION_UP) {
                MoveSessionItem(-1);
            } else if (command == SESSION_DOWN) {
                MoveSessionItem(1);
            } else if (command == SESSION_CAPTION && HIWORD(wp) == EN_CHANGE &&
                       !g_refreshingSession) {
                OnCaptionEdited(window);
            }
            return 0;
        }

        case WM_TIMER:
            if (wp == 2) {
                if (g_openFolderPicker &&
                    (WaitStop(0) ||
                     g_pickerGeneration != g_generation.load())) {
                    g_openFolderPicker->Close(
                        HRESULT_FROM_WIN32(ERROR_CANCELLED));
                }
            } else {
                KillTimer(window, 1);
                RequestSessionPreview();
            }
            return 0;

        case kSessionChanged:
            RefreshSessionList();
            return 0;

        case kSessionPreviewReady:
            InvalidateRect(g_sessionPreview, nullptr, TRUE);
            return 0;

        case kSessionExportFinished: {
            EnterCriticalSection(&g_lock);
            std::wstring status = g_exportStatus;
            LeaveCriticalSection(&g_lock);
            RefreshSessionList();
            SetWindowTextW(g_sessionStatus, status.c_str());
            EnableWindow(GetDlgItem(window, SESSION_START), TRUE);
            EnableWindow(GetDlgItem(window, SESSION_EXPORT),
                         !g_activeSession.load());
            return 0;
        }

        case WM_CLOSE:
            HideSessionWindow(window);
            return 0;

        case WM_DESTROY:
            if (g_sessionFont) {
                DeleteObject(g_sessionFont);
            }
            g_sessionFont = nullptr;
            g_sessionWindow.store(nullptr);
            return 0;
    }
    return DefWindowProcW(window, message, wp, lp);
}

static HICON AppIcon(bool small);  // Defined with the tray icon below.

static void ShowSessionWindow(bool start) {
    HWND window = g_sessionWindow.load();
    if (!window) {
        WNDCLASSW preview{};
        preview.lpfnWndProc = PreviewWindowProc;
        preview.hInstance = GetModuleHandleW(nullptr);
        preview.lpszClassName = L"SnapSentryPreview";
        preview.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        RegisterClassW(&preview);

        WNDCLASSW cls{};
        cls.lpfnWndProc = SessionWindowProc;
        cls.hInstance = GetModuleHandleW(nullptr);
        cls.lpszClassName = L"SnapSentrySession";
        cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        cls.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
        cls.hIcon = AppIcon(false);
        RegisterClassW(&cls);

        window =
            CreateWindowExW(WS_EX_CONTROLPARENT, L"SnapSentrySession",
                            L"SnapSentry - Review session", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, 980, 680, nullptr,
                            nullptr, GetModuleHandleW(nullptr), nullptr);
        g_sessionWindow.store(window);
        if (window) {
            // Scale the default size for the monitor, but keep it on screen.
            const UINT dpi = WindowDpi(window);
            MONITORINFO monitor{sizeof(monitor)};
            GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST),
                            &monitor);
            const int width =
                std::min<int>(MulDiv(980, dpi, 96),
                              monitor.rcWork.right - monitor.rcWork.left);
            const int height =
                std::min<int>(MulDiv(680, dpi, 96),
                              monitor.rcWork.bottom - monitor.rcWork.top);
            if (width > 0 && height > 0) {
                SetWindowPos(window, nullptr, 0, 0, width, height,
                             SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
    }
    if (!window) {
        return;
    }
    if (start) {
        StartSession(window);
    }
    RefreshSessionList();
    LayoutSession(window);
    ShowWindow(window, SW_SHOWNORMAL);
    SetForegroundWindow(window);
}

static std::atomic<HWND> g_promptWindow{nullptr};
static constexpr UINT kPromptStop = WM_APP + 30;
static NOTIFYICONDATAW g_trayIcon{};  // UI STA only.
static int g_registeredSessionKey = 0;
static HICON g_appIconLarge = nullptr;  // UI STA only, destroyed on exit.
static HICON g_appIconSmall = nullptr;

// The shell's own "image files" icon, so the tray entry reads as a screenshot
// tool and isn't mistaken for Windhawk's own tray icon. Falls back to the
// stock application icon.
static HICON AppIcon(bool small) {
    HICON& icon = small ? g_appIconSmall : g_appIconLarge;
    if (!icon) {
        SHSTOCKICONINFO info{};
        info.cbSize = sizeof(info);
        if (SUCCEEDED(SHGetStockIconInfo(
                SIID_IMAGEFILES,
                SHGSI_ICON | (small ? SHGSI_SMALLICON : SHGSI_LARGEICON),
                &info))) {
            icon = info.hIcon;
        }
    }
    return icon ? icon : LoadIconW(nullptr, IDI_APPLICATION);
}

static void SyncSessionControls(const Settings& settings) {
    HWND window = g_promptWindow.load();
    if (!window)
        return;
    if (g_registeredSessionKey &&
        (!settings.sessions ||
         settings.sessionHotkey != g_registeredSessionKey)) {
        UnregisterHotKey(window, 1);
        g_registeredSessionKey = 0;
    }
    if (settings.sessions && !g_registeredSessionKey) {
        if (RegisterHotKey(window, 1, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT,
                           settings.sessionHotkey))
            g_registeredSessionKey = settings.sessionHotkey;
        else
            Wh_Log(L"Session shortcut is in use; use the SnapSentry tray menu");
    }
    if (settings.sessions && !g_trayIcon.cbSize) {
        g_trayIcon = {};
        g_trayIcon.cbSize = sizeof(g_trayIcon);
        g_trayIcon.hWnd = window;
        g_trayIcon.uID = 1;
        g_trayIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        g_trayIcon.uCallbackMessage = kTrayMessage;
        g_trayIcon.hIcon = AppIcon(true);
        wcscpy_s(g_trayIcon.szTip, L"SnapSentry capture sessions");
        if (!Shell_NotifyIconW(NIM_ADD, &g_trayIcon))
            g_trayIcon = {};
    } else if (!settings.sessions && g_trayIcon.cbSize) {
        Shell_NotifyIconW(NIM_DELETE, &g_trayIcon);
        g_trayIcon = {};
    }
    if (!settings.sessions) {
        g_activeSession.store(0);
        HWND review = g_sessionWindow.load();
        if (review)
            ShowWindow(review, SW_HIDE);
    }
}

static LRESULT CALLBACK PromptWindowProc(HWND window,
                                         UINT message,
                                         WPARAM wp,
                                         LPARAM lp) {
    static const UINT taskbarCreated =
        RegisterWindowMessageW(L"TaskbarCreated");
    if (message == taskbarCreated) {
        g_trayIcon = {};
        SyncSessionControls(SnapshotSettings());
        return 0;
    }
    if (message == kPromptStop) {
        if (g_openFolderPicker)
            g_openFolderPicker->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
        if (HWND dialog = g_dialog.load())
            PostMessageW(dialog, TDM_CLICK_BUTTON, ACTION_KEEP, 0);
        return 0;
    }
    if (message == WM_HOTKEY && SnapshotSettings().sessions) {
        if (g_activeSession.load()) {
            g_activeSession.store(0);
            ShowSessionWindow(false);
        } else
            ShowSessionWindow(SnapshotSession().empty());
        return 0;
    }
    if (message == kTrayMessage && SnapshotSettings().sessions) {
        if (lp == WM_LBUTTONDBLCLK) {
            ShowSessionWindow(false);
            return 0;
        }
        if (lp == WM_RBUTTONUP || lp == WM_CONTEXTMENU) {
            HMENU menu = CreatePopupMenu();
            if (!menu)
                return 0;
            AppendMenuW(menu, MF_STRING | (g_exportBusy ? MF_GRAYED : 0),
                        SESSION_START, L"Start new session");
            AppendMenuW(menu, MF_STRING, SESSION_FINISH, L"Finish and review");
            POINT point;
            GetCursorPos(&point);
            SetForegroundWindow(window);
            const UINT choice =
                TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, point.x,
                               point.y, 0, window, nullptr);
            DestroyMenu(menu);
            if (choice == SESSION_START)
                ShowSessionWindow(true);
            else if (choice == SESSION_FINISH) {
                g_activeSession.store(0);
                ShowSessionWindow(false);
            }
            PostMessageW(window, WM_NULL, 0, 0);
        }
        return 0;
    }
    return DefWindowProcW(window, message, wp, lp);
}

static DWORD WINAPI PromptThread(LPVOID) {
    HRESULT apartment = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    HRESULT runtime = RoInitialize(RO_INIT_SINGLETHREADED);
    if (FAILED(apartment) || FAILED(runtime)) {
        Wh_Log(L"Notification apartment could not be initialized");
        if (SUCCEEDED(runtime))
            RoUninitialize();
        if (SUCCEEDED(apartment))
            CoUninitialize();
        SetEvent(g_stopEvent);
        return 0;
    }
    // windhawk.exe's manifest already makes the process per-monitor DPI aware
    // (PerMonitorV2), so the review window scales without a per-thread change.
    INITCOMMONCONTROLSEX controls{sizeof(controls),
                                  ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&controls);
    WNDCLASSW cls{};
    cls.lpfnWndProc = PromptWindowProc;
    cls.hInstance = GetModuleHandleW(nullptr);
    cls.lpszClassName = L"SnapSentryControl";
    RegisterClassW(&cls);
    // Hidden top-level window receives Explorer restart broadcasts for its tray
    // icon.
    HWND window = CreateWindowExW(0, cls.lpszClassName, L"SnapSentry control",
                                  0, 0, 0, 0, 0, nullptr, nullptr,
                                  GetModuleHandleW(nullptr), nullptr);
    g_promptWindow.store(window);
    HANDLE events[] = {g_stopEvent, g_promptEvent};
    // The loop wakes for every batch of window messages (tray mouse moves,
    // review window input, COM), so registration and the hotkey are synced only
    // when the settings change. TaskbarCreated re-adds the tray icon by itself.
    ULONGLONG syncedGeneration = 0;
    while (!WaitStop(0)) {
        ULONGLONG generation;
        Settings settings = SnapshotSettings(&generation);
        if (generation != syncedGeneration) {
            syncedGeneration = generation;
            SyncToastRegistration(settings.popup, false);
            SyncSessionControls(settings);
        }
        CaptureRecord record;
        bool pending = false;
        EnterCriticalSection(&g_lock);
        if (!g_promptQueue.empty()) {
            record = std::move(g_promptQueue.front());
            g_promptQueue.pop_front();
            pending = true;
        }
        LeaveCriticalSection(&g_lock);
        if (pending) {
            if (record.generation != g_generation.load())
                continue;
            // Retry a registration that failed earlier before each prompt.
            SyncToastRegistration(settings.popup, false);
            record.action = settings.popup ? ChooseAction(record.path, settings,
                                                          record.generation)
                                           : ACTION_KEEP;
            EnterCriticalSection(&g_lock);
            const bool accepted = record.generation == g_generation.load() &&
                                  g_queue.size() < kMaxPendingCaptures;
            if (accepted)
                g_queue.push_back(std::move(record));
            LeaveCriticalSection(&g_lock);
            if (accepted)
                SetEvent(g_workEvent);
            else
                AuditResult(settings, AuditOutcome::Kept,
                            L"prompt action cancelled or queue full",
                            record.path);
            continue;
        }
        DWORD result = MsgWaitForMultipleObjectsEx(
            ARRAYSIZE(events), events, INFINITE, QS_ALLINPUT,
            MWMO_INPUTAVAILABLE | MWMO_ALERTABLE);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED)
            break;
        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            HWND review = g_sessionWindow.load();
            if (review && IsDialogMessageW(review, &message))
                continue;
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    SyncToastRegistration(false, true);
    Settings disabled{};
    SyncSessionControls(disabled);
    if (HWND review = g_sessionWindow.load())
        DestroyWindow(review);
    if (window)
        DestroyWindow(window);
    g_promptWindow.store(nullptr);
    UnregisterClassW(L"SnapSentryControl", GetModuleHandleW(nullptr));
    UnregisterClassW(L"SnapSentrySession", GetModuleHandleW(nullptr));
    UnregisterClassW(L"SnapSentryPreview", GetModuleHandleW(nullptr));
    // The tray icon and the window class that used these are gone by now.
    if (g_appIconLarge)
        DestroyIcon(g_appIconLarge);
    if (g_appIconSmall)
        DestroyIcon(g_appIconSmall);
    g_appIconLarge = g_appIconSmall = nullptr;
    RoUninitialize();
    CoUninitialize();
    return 0;
}

static DWORD WINAPI WorkerThread(LPVOID) {
    HRESULT apartment = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    HRESULT runtime = RoInitialize(RO_INIT_SINGLETHREADED);
    if (FAILED(apartment) || FAILED(runtime)) {
        if (SUCCEEDED(runtime))
            RoUninitialize();
        if (SUCCEEDED(apartment))
            CoUninitialize();
        SetEvent(g_stopEvent);
        return 0;
    }
    g_clipboardOwner = CreateWindowExW(0, L"STATIC", L"SnapSentry clipboard", 0,
                                       0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                       GetModuleHandleW(nullptr), nullptr);
    if (!g_clipboardOwner) {
        RoUninitialize();
        CoUninitialize();
        SetEvent(g_stopEvent);
        return 0;
    }
    if (BCryptOpenAlgorithmProvider(&g_hashAlgorithm, BCRYPT_SHA256_ALGORITHM,
                                    nullptr, 0) != 0) {
        g_hashAlgorithm = nullptr;
        Wh_Log(
            L"SHA-256 unavailable; cleanup and session export stay disabled");
    }
    HANDLE events[] = {g_stopEvent, g_workEvent};
    while (!WaitStop(0)) {
        RunDueCleanup();
        RunSessionWork();
        CaptureRecord record;
        if (DequeueOne(record)) {
            ProcessOne(record);
            ReleaseInflight(record.path);
            continue;
        }
        const ULONGLONG now = GetTickCount64();
        auto ready = std::find_if(
            g_retryCaptures.begin(), g_retryCaptures.end(),
            [&](const RetryCapture& retry) {
                return retry.due <= now ||
                       retry.record.generation != g_generation.load();
            });
        if (ready != g_retryCaptures.end()) {
            CaptureRecord retry = std::move(ready->record);
            g_retryCaptures.erase(ready);
            ProcessOne(std::move(retry));
            continue;
        }
        DWORD timeout = INFINITE;
        for (const auto& job : g_pendingCleanup) {
            ULONGLONG now = GetTickCount64();
            DWORD remaining = job.due <= now ? 0
                                             : (DWORD)std::min<ULONGLONG>(
                                                   job.due - now, MAXDWORD - 1);
            timeout = std::min(timeout, remaining);
        }
        for (const auto& retry : g_retryCaptures) {
            ULONGLONG tick = GetTickCount64();
            timeout =
                std::min(timeout, retry.due <= tick
                                      ? 0u
                                      : (DWORD)std::min<ULONGLONG>(
                                            retry.due - tick, MAXDWORD - 1));
        }
        DWORD index = 0;
        HRESULT result = CoWaitForMultipleHandles(
            COWAIT_DISPATCH_CALLS | COWAIT_DISPATCH_WINDOW_MESSAGES, timeout,
            ARRAYSIZE(events), events, &index);
        if (result == RPC_S_CALLPENDING)
            continue;
        if (FAILED(result) || index == 0)
            break;
    }
    // No cleanup is left half done here. Pending jobs never acquire a
    // destructive handle until they run, so dropping them on unload keeps
    // files.
    g_pendingCleanup.clear();
    g_retryCaptures.clear();
    JoinExportThread();  // It hashes with g_hashAlgorithm.
    if (g_hashAlgorithm)
        BCryptCloseAlgorithmProvider(g_hashAlgorithm, 0);
    g_hashAlgorithm = nullptr;
    DestroyWindow(g_clipboardOwner);
    g_clipboardOwner = nullptr;
    RoUninitialize();
    CoUninitialize();
    return 0;
}

// A file turning up in the folder is not the same thing as a screenshot being
// taken. A sync client delivering a capture from another device, an image
// dragged in, a download saved there, all of them look identical to a new
// capture, and with deletion on they would be recycled a few seconds later. Two
// timestamps together are the closest signal available: a real capture has both
// its creation time and its last-write time within the last few seconds. A
// same-volume move keeps the original creation time so it is caught there; a
// copy (drag in, save as, a sync client that rewrites the file) gets a fresh
// creation time but keeps the old content's last-write time, so it is caught by
// the write-time check. A brand new download can't be told apart by timestamps
// at all. Checked on the watch thread, at the moment the event arrives, rather
// than at processing time, since a burst of screenshots or a large file can
// keep the worker busy for longer than the window.
static constexpr ULONGLONG kFreshCaptureWindowMs = 30000;

static bool JustCreated(const std::wstring& path) {
    WIN32_FILE_ATTRIBUTE_DATA info;
    if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &info)) {
        return false;  // Can't tell: like every other unknown, don't act on it.
    }
    FILETIME nowFt;
    GetSystemTimeAsFileTime(&nowFt);
    ULONGLONG now =
        ((ULONGLONG)nowFt.dwHighDateTime << 32) | nowFt.dwLowDateTime;
    const ULONGLONG window = kFreshCaptureWindowMs * 10000ull;  // 100ns units.
    ULONGLONG created = ((ULONGLONG)info.ftCreationTime.dwHighDateTime << 32) |
                        info.ftCreationTime.dwLowDateTime;
    if (now > created && (now - created) > window) {
        return false;  // Not created just now: an older file that appeared
                       // here.
    }
    ULONGLONG written = ((ULONGLONG)info.ftLastWriteTime.dwHighDateTime << 32) |
                        info.ftLastWriteTime.dwLowDateTime;
    if (now > written && (now - written) > window) {
        return false;  // Content predates this creation: a copy of an older
                       // image.
    }
    return true;
}

// Joins a child name to a folder without doubling the separator, which also
// keeps a watched drive root such as D:\ correct. A doubled separator is not a
// form the shell parser accepts, and the composed path feeds the clipboard
// payloads and the delete alike.
static std::wstring JoinChild(const std::wstring& dir,
                              const std::wstring& name) {
    if (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) {
        return dir + name;
    }
    return dir + L'\\' + name;
}

static void Enqueue(const std::wstring& folder, const std::wstring& name) {
    if (!IsSafeChildName(name) || !IsSupportedImage(name))
        return;
    CaptureRecord record;
    record.path = JoinChild(folder, name);
    if (!JustCreated(record.path))
        return;
    record.identified = ObserveIdentity(record.path, record.identity);
    if (!record.identified)
        return;
    GetSystemTimeAsFileTime(&record.observed);
    record.label = ActiveWindowLabel();
    const ULONGLONG now = GetTickCount64();
    static ULONGLONG lastOverflow = 0;  // Watcher thread only.
    EnterCriticalSection(&g_lock);
    record.generation = g_generation.load();
    record.session = g_activeSession.load();
    bool added = g_queue.size() < kMaxPendingCaptures &&
                 !SeenRecently(record.path, now) &&
                 !g_preexisting.count(name) &&
                 g_inflight.insert(record.path).second;
    if (added) {
        if (record.session)
            ++g_sessionPending;
        g_queue.push_back(std::move(record));
    }
    bool full = g_queue.size() >= kMaxPendingCaptures;
    LeaveCriticalSection(&g_lock);
    if (added)
        SetEvent(g_workEvent);
    else if (full && now - lastOverflow >= 5000) {
        lastOverflow = now;
        Wh_Log(L"Capture queue full; additional files are kept untouched");
    }
}

// ============================================================================
// Watcher: overlapped ReadDirectoryChangesW with a clean stop/reload path
// ============================================================================

static void ParseNotifications(const std::wstring& folder,
                               const BYTE* buffer,
                               DWORD bytes) {
    const size_t header = offsetof(FILE_NOTIFY_INFORMATION, FileName);
    size_t offset = 0;
    while (offset <= bytes && bytes - offset >= header) {
        const auto* info =
            reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(buffer + offset);
        const size_t available = bytes - offset;
        if (info->FileNameLength % sizeof(wchar_t) ||
            info->FileNameLength > available - header)
            break;
        if (info->Action == FILE_ACTION_ADDED ||
            info->Action == FILE_ACTION_RENAMED_NEW_NAME)
            Enqueue(folder, std::wstring(info->FileName, info->FileNameLength /
                                                             sizeof(wchar_t)));
        const size_t next = info->NextEntryOffset;
        if (!next)
            break;
        if (next % alignof(DWORD) || next < header + info->FileNameLength ||
            next > available)
            break;
        offset += next;
    }
}

// Records the folder's current file names so hydration or sync churn on files
// that predate this watch session is never treated as a new screenshot. Called
// each time the directory is opened, before the first change notification is
// armed. Placeholders are enumerated like any other entry, so dehydrated old
// screenshots are captured too.
static bool SnapshotExistingNames(const std::wstring& folder) {
    std::set<std::wstring, NameLess> names;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(JoinChild(folder, L"*").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                names.insert(fd.cFileName);
            }
        } while (FindNextFileW(h, &fd));
        const DWORD result = GetLastError();
        FindClose(h);
        if (result != ERROR_NO_MORE_FILES)
            return false;
    } else if (GetLastError() != ERROR_FILE_NOT_FOUND)
        return false;
    EnterCriticalSection(&g_lock);
    g_recentContent.clear();
    g_preexisting = std::move(names);
    LeaveCriticalSection(&g_lock);
    return true;
}

static DWORD WINAPI WatchThread(LPVOID) {
    OVERLAPPED ov{};
    ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ov.hEvent) {
        return 0;
    }
    HANDLE readyWaits[] = {g_stopEvent, g_reloadEvent, ov.hEvent};

    int watchFailures =
        0;  // consecutive open OR notification failures, for backoff
    while (!WaitStop(0)) {
        Settings s = SnapshotSettings();
        if (s.folder.empty()) {
            // Wait indefinitely: a settings change signals g_reloadEvent, so
            // there is nothing to poll for here.
            HANDLE idle[] = {g_stopEvent, g_reloadEvent};
            WaitForMultipleObjects(2, idle, FALSE, INFINITE);
            continue;
        }

        HANDLE dir = CreateFileW(
            s.folder.c_str(), FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            nullptr);
        if (dir == INVALID_HANDLE_VALUE) {
            // The folder may appear later (Pictures\Screenshots isn't created
            // until the first Win+PrintScreen). Back off from 2s to 30s after a
            // few tries so a Snipping-Tool-only machine isn't polled twice a
            // second forever.
            DWORD wait = watchFailures < 5 ? 2000 : 30000;
            if (watchFailures < 1000000) {
                watchFailures++;
            }
            HANDLE idle[] = {g_stopEvent, g_reloadEvent};
            WaitForMultipleObjects(2, idle, FALSE, wait);
            continue;
        }

        // Opening the handle isn't proof the watch works, so watchFailures is
        // not reset here. It resets only when a notification actually arrives
        // (below), so a path where the handle opens but ReadDirectoryChangesW
        // never delivers still escalates its backoff instead of respinning
        // every 2s.

        // Snapshot existing names before arming notifications so downloads or
        // sync churn on pre-existing files never look like new screenshots.
        if (!SnapshotExistingNames(s.folder)) {
            CloseHandle(dir);
            Wh_Log(
                L"Could not snapshot existing files; folder is left untouched");
            HANDLE idle[] = {g_stopEvent, g_reloadEvent};
            WaitForMultipleObjects(2, idle, FALSE, 2000);
            continue;
        }

        // ReadDirectoryChangesW writes FILE_NOTIFY_INFORMATION records that
        // must be DWORD aligned; alignas guarantees it (a plain BYTE array need
        // not be).
        alignas(DWORD) BYTE buffer[16384];
        bool reopen = false;
        while (!reopen) {
            ResetEvent(ov.hEvent);
            DWORD bytes = 0;
            if (!ReadDirectoryChangesW(dir, buffer, sizeof(buffer), FALSE,
                                       FILE_NOTIFY_CHANGE_FILE_NAME, &bytes,
                                       &ov, nullptr)) {
                break;  // Re-open the directory.
            }

            DWORD w = WaitForMultipleObjects(3, readyWaits, FALSE, INFINITE);
            if (w == WAIT_OBJECT_0) {  // Stop.
                CancelIoEx(dir, &ov);
                GetOverlappedResult(dir, &ov, &bytes, TRUE);
                CloseHandle(dir);
                CloseHandle(ov.hEvent);
                return 0;
            }
            if (w == WAIT_OBJECT_0 + 1) {  // Settings changed.
                CancelIoEx(dir, &ov);
                GetOverlappedResult(dir, &ov, &bytes, TRUE);
                reopen = true;
                break;
            }
            // I/O completed.
            if (!GetOverlappedResult(dir, &ov, &bytes, FALSE)) {
                break;
            }
            if (bytes == 0) {
                // Buffer overflow: notifications were dropped. We can't tell
                // which files are new, and rescanning could delete pre-existing
                // files, so we deliberately skip rather than risk the safety
                // invariant.
                Wh_Log(
                    L"Change buffer overflow; some screenshots may be skipped");
                continue;
            }
            watchFailures =
                0;  // A notification arrived: the watch works; clear backoff.
            ParseNotifications(s.folder, buffer, bytes);
        }
        CloseHandle(dir);
        if (!reopen) {
            // The inner loop ended on an error, not a reload: the handle opened
            // but change notifications don't work on this path (some network
            // redirectors and virtual/sync filesystems return
            // ERROR_INVALID_FUNCTION), or the volume went away. Back off with
            // the same 2s->30s escalation as the open-failure path
            // (watchFailures counts both), so a persistently failing watch
            // can't peg a core re-opening and re-enumerating in a spin.
            DWORD wait = watchFailures < 5 ? 2000 : 30000;
            if (watchFailures < 1000000) {
                watchFailures++;
            }
            HANDLE idle[] = {g_stopEvent, g_reloadEvent};
            WaitForMultipleObjects(2, idle, FALSE, wait);
        }
    }

    CloseHandle(ov.hEvent);
    return 0;
}

// ============================================================================
// Tool-mod entry points
// ============================================================================

BOOL WhTool_ModInit() {
    InitializeCriticalSection(&g_lock);
    InitializeCriticalSection(&g_toastLock);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);  // Manual reset.
    g_reloadEvent =
        CreateEventW(nullptr, FALSE, FALSE, nullptr);  // Auto reset.
    g_settingsEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);  // Manual reset.
    g_workEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);  // Auto reset.
    g_promptEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_toastActionEvent =
        CreateEventW(nullptr, FALSE, FALSE, nullptr);  // Auto reset.
    if (!g_stopEvent || !g_reloadEvent || !g_settingsEvent || !g_workEvent ||
        !g_toastActionEvent || !g_promptEvent) {
        if (g_promptEvent)
            CloseHandle(g_promptEvent);
        if (g_stopEvent)
            CloseHandle(g_stopEvent);
        if (g_reloadEvent)
            CloseHandle(g_reloadEvent);
        if (g_settingsEvent)
            CloseHandle(g_settingsEvent);
        if (g_workEvent)
            CloseHandle(g_workEvent);
        if (g_toastActionEvent)
            CloseHandle(g_toastActionEvent);
        g_stopEvent = g_reloadEvent = g_settingsEvent = g_workEvent =
            g_toastActionEvent = nullptr;
        DeleteCriticalSection(&g_lock);
        DeleteCriticalSection(&g_toastLock);
        return FALSE;
    }

    LoadSettings();
    g_promptThread =
        CreateThread(nullptr, 0, PromptThread, nullptr, 0, nullptr);
    g_workerThread =
        CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    g_watchThread = CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
    if (!g_workerThread || !g_watchThread || !g_promptThread) {
        SetEvent(g_stopEvent);
        SetEvent(g_workEvent);
        if (g_promptThread)
            WaitForSingleObject(g_promptThread, INFINITE);
        if (g_promptThread)
            CloseHandle(g_promptThread);
        if (g_watchThread)
            WaitForSingleObject(g_watchThread, INFINITE);
        if (g_workerThread)
            WaitForSingleObject(g_workerThread, INFINITE);
        if (g_watchThread)
            CloseHandle(g_watchThread);
        if (g_workerThread)
            CloseHandle(g_workerThread);
        CloseHandle(g_promptEvent);
        CloseHandle(g_stopEvent);
        CloseHandle(g_reloadEvent);
        CloseHandle(g_settingsEvent);
        CloseHandle(g_workEvent);
        CloseHandle(g_toastActionEvent);
        g_watchThread = g_workerThread = nullptr;
        g_stopEvent = g_reloadEvent = g_settingsEvent = g_workEvent =
            g_toastActionEvent = nullptr;
        DeleteCriticalSection(&g_lock);
        DeleteCriticalSection(&g_toastLock);
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    SetEvent(g_promptEvent);
    SetEvent(g_reloadEvent);  // Re-open the folder in case the path changed.
    SetEvent(g_workEvent);    // Wake the worker to apply the updated processing
                              // settings.
}

void WhTool_ModUninit() {
    SetEvent(g_stopEvent);
    PostMessageW(g_promptWindow.load(), kPromptStop, 0, 0);

    // Dismiss an open action dialog as "Keep" so shutdown never triggers a
    // delete.
    HWND dlg = g_dialog.load();
    if (dlg) {
        PostMessageW(dlg, TDM_CLICK_BUTTON, ACTION_KEEP, 0);
    }
    SetEvent(g_workEvent);

    HANDLE threads[] = {g_watchThread, g_workerThread, g_promptThread};
    // Both threads observe g_stopEvent and must be gone before their events and
    // critical sections are destroyed.
    WaitForMultipleObjects(ARRAYSIZE(threads), threads, TRUE, INFINITE);
    // An export started just as the stop event was set sees it straight away
    // and exits; it must still be gone before the lock and events below are.
    JoinExportThread();
    if (g_promptThread)
        CloseHandle(g_promptThread);
    if (g_watchThread) {
        CloseHandle(g_watchThread);
    }
    if (g_workerThread) {
        CloseHandle(g_workerThread);
    }
    if (g_promptEvent)
        CloseHandle(g_promptEvent);
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
    }
    if (g_reloadEvent) {
        CloseHandle(g_reloadEvent);
    }
    if (g_settingsEvent) {
        CloseHandle(g_settingsEvent);
    }
    if (g_workEvent) {
        CloseHandle(g_workEvent);
    }
    if (g_toastActionEvent) {
        CloseHandle(g_toastActionEvent);
    }
    DeleteCriticalSection(&g_lock);
    DeleteCriticalSection(&g_toastLock);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

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
