// ==WindhawkMod==
// @id              snap-sentry
// @name            SnapSentry
// @description     Copy saved screenshots, delete them automatically, or choose what to do from a notification.
// @version         0.7.0
// @author          mario0318
// @github          https://github.com/mario0318
// @include         windhawk.exe
// @license         GPL-3.0
// @compilerOptions -lole32 -lshell32 -lcomctl32 -lwindowscodecs -lruntimeobject -ladvapi32 -luuid
// ==/WindhawkMod==

// Source code is published under the GNU General Public License v3.0.
// https://github.com/mario0318/SnapSentry

// ==WindhawkModReadme==
/*
# SnapSentry

Watches **Pictures\\Screenshots** and handles new screenshots as they are saved.
Copy the image, delete the file after a delay, or choose what to do from a
notification.

![The SnapSentry notification](https://raw.githubusercontent.com/mario0318/SnapSentry/32558d49a40d02fb03661abad0c2d9d313ae00e9/assets/notification.png)

## Clipboard modes

* **Image** copies the picture so it stays pasteable even after the file is deleted.
* **File** copies the file for pasting into File Explorer. Deletion is disabled.
* **Path** copies the full path as text. You can pick plain text, a quoted path, a
  file link, or a Markdown image link, which is handy for pasting a screenshot
  straight into notes or a bug report. Deletion is disabled.
* **None** leaves the clipboard unchanged.

## Naming

You can have SnapSentry rename each new screenshot from the window that was in
front when it was taken, together with a timestamp, so a file ends up named for
what it shows instead of Screenshot (1). The file stays in the same folder.

## The notification

The popup is a real Windows notification, so it matches your light or dark theme.
While the popup is turned on, SnapSentry registers itself with Windows so the
notification buttons work, and turning the popup off or disabling the mod removes
that registration again. If notifications are unavailable, SnapSentry shows a
standard dialog instead.

## Privacy

SnapSentry only handles files created after it starts. By default a deleted
screenshot goes to the Recycle Bin so it can be restored; if you turn that off,
deletion is permanent. Deleting a screenshot does not remove copies already
stored in clipboard history, cloud sync, backups, or other programs.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable processing
- delaySeconds: 5
  $name: Seconds before deletion
  $description: With the popup on, this is the countdown before the automatic action, and 0 waits for you to choose, keeping the file if the popup is never answered. With the popup off, 0 deletes as soon as clipboard copying finishes.
- deleteFile: true
  $name: Delete the saved screenshot
  $description: Only applies to Image and None clipboard modes. File and Path modes reference the file, so deletion is always suppressed for them.
- recycle: true
  $name: Send deletions to the Recycle Bin
  $description: Deleted screenshots go to the Recycle Bin so they can be restored. Turn off to delete permanently.
- renameFromWindow: false
  $name: Rename new screenshots by active window
  $description: Renames each new screenshot using the title of the window that is in front, plus a timestamp, so files are easier to find later. The file stays in the same folder.
- showActionPopup: true
  $name: Show companion action popup
  $description: A notification offering Delete now, Copy image and delete, or Keep (falls back to a dialog if notifications are unavailable). The configured automatic action runs when the countdown expires.
- clipboardMode: image
  $name: Clipboard content
  $options:
  - image: Image (recommended with deletion)
  - file: File object (deletion suppressed)
  - path: Full path text (deletion suppressed)
  - none: Don't change clipboard
- pathFormat: plain
  $name: Path text format
  $description: Only used when Clipboard content is set to Full path text.
  $options:
  - plain: Plain path
  - quoted: Quoted path
  - uri: File link
  - markdown: Markdown image link
- folder: ""
  $name: Folder override
  $description: Leave empty to watch your Windows Screenshots folder, wherever it is located.
- logDetails: false
  $name: Verbose logging (may include file paths)
  $description: When off, log lines omit file paths. Turn on only while debugging.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <initguid.h>  // Defines the GUIDs pulled in by the headers below in-TU.
#include <knownfolders.h>
#include <shlobj.h>
#include <propkey.h>   // PKEY_AppUserModel_ID, PKEY_AppUserModel_ToastActivatorCLSID
#include <commctrl.h>
#include <wincodec.h>
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

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <deque>
#include <set>
#include <string>
#include <utility>

// ============================================================================
// Settings and shared state
// ============================================================================

struct Settings {
    bool enabled;
    int delaySeconds;
    bool deleteFile;
    bool popup;
    std::wstring mode;
    std::wstring pathFormat;
    std::wstring folder;
    bool recycle;
    bool renameFromWindow;
    bool logDetails;
};

static CRITICAL_SECTION g_lock;       // Guards g_settings, g_queue, g_inflight, g_recent, g_preexisting.
static Settings g_settings;
static std::deque<std::wstring> g_queue;   // Full paths waiting to be processed.
static std::set<std::wstring> g_inflight;  // Names queued or in progress (dedup).

// Names already present when the watched folder was opened. They are never acted
// on, which is the literal safety invariant "never touch files that were already
// in the folder." This is what stops a flood when OneDrive Files On-Demand
// hydrates a folder full of old screenshots: each download surfaces as an ADDED
// or RENAMED event for a name in this set, so it is ignored. Refreshed every time
// the folder is (re)opened.
static std::set<std::wstring> g_preexisting;

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

// Pre-seed the recent-name set so a file event we cause ourselves (the rename
// below) is swallowed by the watcher instead of being handled as a brand-new
// screenshot, which would rename it again in a loop.
static void MarkNameRecent(const std::wstring& name) {
    ULONGLONG now = GetTickCount64();
    EnterCriticalSection(&g_lock);
    g_recent.push_back({name, now + kDuplicateEventWindowMs});
    LeaveCriticalSection(&g_lock);
}

static HANDLE g_stopEvent;   // Manual-reset: set once at shutdown.
static HANDLE g_reloadEvent; // Auto-reset: settings changed, re-open the folder.
static HANDLE g_workEvent;   // Auto-reset: queue has work.
static HANDLE g_watchThread;
static HANDLE g_workerThread;
static std::atomic<HWND> g_dialog{nullptr};  // Open action dialog, for shutdown.

enum {
    ACTION_AUTO = 100,
    ACTION_DELETE = 101,
    ACTION_COPY_DELETE = 102,
    ACTION_KEEP = 103,
};

static CRITICAL_SECTION g_toastLock;   // Guards g_toastAction, g_toastAnswered, g_activeToastId.
static int g_toastAction = ACTION_AUTO;
// First answer wins. A button click and a dismissal can both arrive for the same
// toast (clicking a button dismisses it), and the click is the real answer, so a
// later dismissal must not overwrite it with Keep.
static bool g_toastAnswered = false;
static ULONGLONG g_activeToastId;
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
    s.enabled = Wh_GetIntSetting(L"enabled") != 0;
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
    s.logDetails = Wh_GetIntSetting(L"logDetails") != 0;

    PCWSTR mode = Wh_GetStringSetting(L"clipboardMode");
    s.mode = mode;
    Wh_FreeStringSetting(mode);

    PCWSTR pathFormat = Wh_GetStringSetting(L"pathFormat");
    s.pathFormat = pathFormat;
    Wh_FreeStringSetting(pathFormat);

    PCWSTR folder = Wh_GetStringSetting(L"folder");
    s.folder = folder;
    Wh_FreeStringSetting(folder);
    if (s.folder.empty()) {
        s.folder = DefaultScreenshotsFolder();
    }

    EnterCriticalSection(&g_lock);
    g_settings = std::move(s);
    LeaveCriticalSection(&g_lock);
}

static Settings SnapshotSettings() {
    EnterCriticalSection(&g_lock);
    Settings s = g_settings;
    LeaveCriticalSection(&g_lock);
    return s;
}

// True if the stop event became signalled within the wait.
static bool WaitStop(DWORD ms) {
    return WaitForSingleObject(g_stopEvent, ms) == WAIT_OBJECT_0;
}

// ============================================================================
// File name helpers
// ============================================================================

static bool IsSupportedImage(const std::wstring& name) {
    auto dot = name.find_last_of(L'.');
    if (dot == std::wstring::npos) {
        return false;
    }
    std::wstring ext = name.substr(dot);
    CharLowerBuffW(ext.data(), (DWORD)ext.size());
    return ext == L".png" || ext == L".jpg" || ext == L".jpeg" ||
           ext == L".bmp" || ext == L".gif" || ext == L".webp";
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

// Renames a just-created screenshot to "<timestamp> <window>.<ext>" in the same
// folder. Returns the new full path, or L"" if it didn't rename (no usable title,
// or the move failed) so the caller keeps using the original. The result stays a
// plain file directly inside the watched folder, preserving the safety invariant.
static std::wstring RenameByActiveWindow(const std::wstring& path) {
    std::wstring label = ActiveWindowLabel();
    if (label.empty()) return L"";

    size_t slash = path.find_last_of(L"\\/");
    if (slash == std::wstring::npos) return L"";
    std::wstring dir = path.substr(0, slash + 1);
    std::wstring name = path.substr(slash + 1);
    size_t dot = name.find_last_of(L'.');
    std::wstring ext = (dot == std::wstring::npos) ? L"" : name.substr(dot);

    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t stamp[32];
    swprintf_s(stamp, L"%04u-%02u-%02u %02u-%02u-%02u", st.wYear, st.wMonth,
               st.wDay, st.wHour, st.wMinute, st.wSecond);
    std::wstring base = std::wstring(stamp) + L" " + label;

    for (int n = 1; n <= 50; n++) {
        std::wstring suffix =
            (n == 1) ? std::wstring() : (L" (" + std::to_wstring(n) + L")");
        std::wstring childName = base + suffix + ext;
        std::wstring candidate = dir + childName;
        if (GetFileAttributesW(candidate.c_str()) != INVALID_FILE_ATTRIBUTES) {
            continue;  // name already taken
        }
        // Register the target before moving so the watcher ignores the event our
        // own rename fires, instead of re-processing (and re-renaming) the file.
        MarkNameRecent(childName);
        if (MoveFileW(path.c_str(), candidate.c_str())) {
            return candidate;
        }
        return L"";  // move failed; keep the original name
    }
    return L"";
}

// ============================================================================
// Path text formatting (Path clipboard mode)
// ============================================================================

// Minimal file URI: forward slashes and %20 for spaces, enough for a link or a
// Markdown image to paste cleanly.
static std::wstring PathToFileUri(const std::wstring& path) {
    std::wstring uri = L"file:///";
    for (wchar_t c : path) {
        if (c == L'\\') {
            uri += L'/';
        } else if (c == L' ') {
            uri += L"%20";
        } else {
            uri += c;
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

    if (!OpenClipboard(nullptr)) {
        GlobalFree(h);
        return false;
    }
    EmptyClipboard();
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

    if (!OpenClipboard(nullptr)) {
        GlobalFree(h);
        return false;
    }
    EmptyClipboard();
    bool ok = SetClipboardData(CF_HDROP, h) != nullptr;
    if (!ok) {
        GlobalFree(h);
    }
    CloseClipboard();
    return ok;
}

// Fills a DIB pixel region from a top-down 32bpp BGRA source, writing the rows
// bottom-up as DIB clipboard formats expect.
static void WriteBottomUp(BYTE* dest, const BYTE* topDown, UINT width,
                          UINT height) {
    size_t stride = (size_t)width * 4;
    for (UINT y = 0; y < height; y++) {
        memcpy(dest + (size_t)y * stride,
               topDown + (size_t)(height - 1 - y) * stride, stride);
    }
}

// Decodes any WIC-supported image into self-contained CF_DIBV5 + CF_DIB payloads.
// This is what makes the copied image survive deletion of the source file.
static bool ClipboardImage(const std::wstring& path) {
    IWICImagingFactory* factory = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)))) {
        return false;
    }

    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;
    BYTE* topDown = nullptr;
    HGLOBAL hV5 = nullptr;
    HGLOBAL hDib = nullptr;
    bool ok = false;

    do {
        if (FAILED(factory->CreateDecoderFromFilename(
                path.c_str(), nullptr, GENERIC_READ,
                WICDecodeMetadataCacheOnDemand, &decoder))) {
            break;
        }
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
        topDown = (BYTE*)malloc(pixels);
        if (!topDown ||
            FAILED(converter->CopyPixels(nullptr, (UINT)stride, (UINT)pixels,
                                         topDown))) {
            break;
        }

        hV5 = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPV5HEADER) + pixels);
        hDib = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + pixels);
        if (!hV5 || !hDib) {
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
        WriteBottomUp(v5 + sizeof(BITMAPV5HEADER), topDown, width, height);
        GlobalUnlock(hV5);

        BYTE* dib = (BYTE*)GlobalLock(hDib);
        if (!dib) {
            break;
        }
        auto* bih = (BITMAPINFOHEADER*)dib;
        ZeroMemory(bih, sizeof(*bih));
        bih->biSize = sizeof(BITMAPINFOHEADER);
        bih->biWidth = (LONG)width;
        bih->biHeight = (LONG)height;
        bih->biPlanes = 1;
        bih->biBitCount = 32;
        bih->biCompression = BI_RGB;
        bih->biSizeImage = (DWORD)pixels;
        WriteBottomUp(dib + sizeof(BITMAPINFOHEADER), topDown, width, height);
        GlobalUnlock(hDib);

        if (!OpenClipboard(nullptr)) {
            break;
        }
        EmptyClipboard();
        bool setV5 = SetClipboardData(CF_DIBV5, hV5) != nullptr;
        if (setV5) {
            hV5 = nullptr;  // Clipboard owns it now.
        }
        bool setDib = SetClipboardData(CF_DIB, hDib) != nullptr;
        if (setDib) {
            hDib = nullptr;
        }
        CloseClipboard();
        ok = setV5 || setDib;
    } while (false);

    if (hV5) {
        GlobalFree(hV5);
    }
    if (hDib) {
        GlobalFree(hDib);
    }
    free(topDown);
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
    return ok;
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
    if (!GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath))) {
        Wh_Log(L"AUMID: GetModuleFileName failed %lu", GetLastError());
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
    if (!GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath))) {
        return false;
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
    Invoke(ABI::Windows::UI::Notifications::IToastNotification*,
           ABI::Windows::UI::Notifications::IToastDismissedEventArgs*) override {
        EnterCriticalSection(&g_toastLock);
        // Only the toast currently being waited on, and only if a button click
        // hasn't already answered it (clicking one also dismisses the toast).
        bool mine = g_activeToastId != 0 && !g_toastAnswered;
        if (mine) {
            g_toastAction = ACTION_KEEP;
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

// Builds and shows the toast, then waits up to s.delaySeconds for a button
// click (via ToastActivator::Activate, dispatched by CoWaitForMultipleHandles)
// or an explicit dismissal. Returns false -- meaning "use the dialog instead"
// -- if registration hasn't succeeded or any WinRT step fails, so the mod stays
// usable even where toast notifications don't work.
static bool ShowToast(const std::wstring& path, const Settings& s, int& action) {
    using namespace ABI::Windows::UI::Notifications;
    using namespace ABI::Windows::Data::Xml::Dom;
    using namespace ABI::Windows::Foundation;
    using Microsoft::WRL::ComPtr;
    using Microsoft::WRL::Wrappers::HStringReference;

    if (!g_toastRegistered.load()) {
        return false;
    }

    std::wstring name = path.substr(path.find_last_of(L"\\/") + 1);
    ULONGLONG toastId = GetTickCount64();
    std::wstring id = std::to_wstring(toastId);
    std::wstring xml =
        L"<toast scenario=\"reminder\" duration=\"long\">"
        L"<visual><binding template=\"ToastGeneric\">"
        L"<text>Screenshot saved</text><text>" +
        XmlEscape(name) +
        L"</text><text placement=\"attribution\">SnapSentry</text>"
        L"</binding></visual><actions>"
        L"<action content=\"Delete\" arguments=\"delete|" + id + L"\" activationType=\"background\"/>"
        L"<action content=\"Copy + delete\" arguments=\"copydelete|" + id + L"\" activationType=\"background\"/>"
        L"<action content=\"Keep\" arguments=\"keep|" + id + L"\" activationType=\"background\"/>"
        L"</actions></toast>";

    ComPtr<IToastNotificationManagerStatics> toastStatics;
    if (FAILED(RoGetActivationFactory(
            HStringReference(
                RuntimeClass_Windows_UI_Notifications_ToastNotificationManager)
                .Get(),
            IID_PPV_ARGS(&toastStatics)))) {
        return false;
    }
    ComPtr<IToastNotifier> notifier;
    if (FAILED(toastStatics->CreateToastNotifierWithId(
            HStringReference(kAppUserModelId).Get(), &notifier))) {
        return false;
    }

    // Show() reports success even when notifications are switched off for us, and
    // then nothing is ever delivered and no answer can arrive. Ask first and use
    // the dialog on the machines where the toast would go nowhere.
    NotificationSetting setting = NotificationSetting_Enabled;
    if (FAILED(notifier->get_Setting(&setting)) ||
        setting != NotificationSetting_Enabled) {
        Wh_Log(L"Notifications unavailable (setting=%d); using the dialog instead",
               (int)setting);
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
    ComPtr<IToastNotification> toast;
    if (FAILED(toastFactory->CreateToastNotification(doc.Get(), &toast))) {
        return false;
    }

    ResetEvent(g_toastActionEvent);
    EnterCriticalSection(&g_toastLock);
    g_activeToastId = toastId;
    g_toastAction = ACTION_AUTO;
    g_toastAnswered = false;
    LeaveCriticalSection(&g_toastLock);

    EventRegistrationToken dismissToken{};
    bool dismissHooked =
        SUCCEEDED(toast->add_Dismissed(&g_toastDismissListener, &dismissToken));

    if (FAILED(notifier->Show(toast.Get()))) {
        EnterCriticalSection(&g_toastLock);
        g_activeToastId = 0;  // Don't accept a late Activate for a toast that never showed.
        LeaveCriticalSection(&g_toastLock);
        if (dismissHooked) {
            toast->remove_Dismissed(dismissToken);
        }
        return false;
    }

    // Our own timer is authoritative for the automatic action. Remove the toast
    // once the action is settled so stale buttons can't affect a later screenshot.
    //
    // With no countdown the wait used to be INFINITE, which meant a toast that was
    // never delivered (Focus assist, a delivery failure, anything that swallows it
    // quietly) parked this thread forever and every later screenshot queued behind
    // it. Wait a long time instead of forever, and treat running out as Keep, since
    // nobody saw the toast to ask for a deletion.
    static constexpr DWORD kNoAnswerCapMs = 10 * 60 * 1000;
    bool noCountdown = s.delaySeconds <= 0;
    DWORD timeoutMs =
        noCountdown ? kNoAnswerCapMs : (DWORD)s.delaySeconds * 1000;
    HANDLE waits[] = {g_stopEvent, g_toastActionEvent};
    DWORD start = GetTickCount();
    bool removeToast = false;
    for (;;) {
        DWORD elapsed = GetTickCount() - start;
        DWORD remaining = elapsed >= timeoutMs ? 0 : timeoutMs - elapsed;
        DWORD result = MsgWaitForMultipleObjectsEx(
            ARRAYSIZE(waits), waits, remaining, QS_ALLINPUT,
            MWMO_INPUTAVAILABLE | MWMO_ALERTABLE);
        if (result == WAIT_IO_COMPLETION) {
            continue;  // MWMO_ALERTABLE: an APC ran, keep waiting.
        }
        if (result == WAIT_TIMEOUT) {
            if (noCountdown) {
                Wh_Log(L"No answer to the notification; keeping the file");
            }
            action = noCountdown ? ACTION_KEEP : ACTION_AUTO;
            removeToast = true;
            break;
        }
        if (result == WAIT_OBJECT_0) {  // Stop requested: never delete on shutdown.
            action = ACTION_KEEP;
            removeToast = true;
            break;
        }
        if (result == WAIT_OBJECT_0 + 1) {
            EnterCriticalSection(&g_toastLock);
            action = g_toastAction;
            LeaveCriticalSection(&g_toastLock);
            removeToast = true;
            break;
        }
        if (result == WAIT_OBJECT_0 + ARRAYSIZE(waits)) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            continue;
        }
        Wh_Log(L"Toast wait failed %lu", GetLastError());
        action = ACTION_KEEP;
        removeToast = true;  // Its buttons are dead to us now, so don't leave it up.
        break;
    }

    EnterCriticalSection(&g_toastLock);
    g_activeToastId = 0;
    LeaveCriticalSection(&g_toastLock);
    if (dismissHooked) {
        toast->remove_Dismissed(dismissToken);
    }
    if (removeToast) notifier->Hide(toast.Get());
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
    DWORD started;
    DWORD timeoutMs;  // INFINITE when there is no countdown.
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
            if (state->timeoutMs != INFINITE) {
                DWORD elapsed = GetTickCount() - state->started;
                if (elapsed >= state->timeoutMs) {
                    SendMessageW(hwnd, TDM_CLICK_BUTTON, ACTION_AUTO, 0);
                } else {
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

static int AskAction(const std::wstring& path, const Settings& s) {
    if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
        return ACTION_KEEP;  // Don't put up UI during teardown.
    }
    std::wstring name = path.substr(path.find_last_of(L"\\/") + 1);
    DialogState state;
    state.started = GetTickCount();
    state.timeoutMs = s.delaySeconds > 0 ? (DWORD)s.delaySeconds * 1000 : INFINITE;
    state.baseText = name + L"\n\nChoose an action, or wait for the configured "
                            L"automatic action.";

    // Reserve the countdown line up front so TDF_SIZE_TO_CONTENT sizes the dialog
    // once instead of growing after the first timer tick.
    std::wstring initialText = state.baseText;
    if (state.timeoutMs != INFINITE) {
        initialText += L"\n\nAutomatic action in " +
                       std::to_wstring((state.timeoutMs + 999) / 1000) + L" s…";
    }

    TASKDIALOG_BUTTON buttons[] = {
        {ACTION_DELETE, L"Delete now"},
        {ACTION_COPY_DELETE, L"Copy image && delete"},
        {ACTION_KEEP, L"Keep file"},
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
    c.cButtons = ARRAYSIZE(buttons);
    c.pButtons = buttons;
    c.nDefaultButton = ACTION_COPY_DELETE;
    c.pfCallback = DialogCallback;
    c.lpCallbackData = (LONG_PTR)&state;

    int result = ACTION_AUTO;
    // Cancellation (Esc / shutdown-driven close) is treated as Keep: never delete.
    // TDF_ALLOW_DIALOG_CANCELLATION makes that a successful call returning IDCANCEL
    // rather than a failure, so map anything that isn't one of our buttons to Keep.
    if (FAILED(TaskDialogIndirect(&c, &result, nullptr, nullptr))) {
        return ACTION_KEEP;
    }
    switch (result) {
        case ACTION_AUTO:
        case ACTION_DELETE:
        case ACTION_COPY_DELETE:
        case ACTION_KEEP:
            return result;
        default:
            return ACTION_KEEP;
    }
}

// Tries the toast notification first; falls back to the dialog box if toast
// registration didn't succeed on this machine or the WinRT call chain fails.
static int ChooseAction(const std::wstring& path, const Settings& s) {
    int action = ACTION_AUTO;
    if (ShowToast(path, s, action)) {
        return action;
    }
    return AskAction(path, s);
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

// Deletes a watched-folder file, refusing to follow a reparse point so we never
// act on something that redirects outside the folder. In Recycle Bin mode a file
// that can't be recycled is kept rather than permanently deleted, so choosing
// "recoverable" never silently turns into a permanent delete.
static void DeleteWatched(const std::wstring& path, const Settings& s) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return;  // Already gone.
    }
    if (attrs & FILE_ATTRIBUTE_REPARSE_POINT) {
        Wh_Log(L"Refusing to delete reparse point%s",
               s.logDetails ? (L": " + path).c_str() : L"");
        return;
    }
    if (s.recycle) {
        if (!RecycleFile(path)) {
            Wh_Log(L"Recycle failed, keeping file%s",
                   s.logDetails ? (L": " + path).c_str() : L"");
        }
        return;
    }
    if (!DeleteFileW(path.c_str())) {
        Wh_Log(L"Delete failed (%lu)%s", GetLastError(),
               s.logDetails ? (L": " + path).c_str() : L"");
    }
}

// Waits until the screenshot has finished being written, or the mod stops.
// Rather than waiting for Snipping Tool (or an antivirus scan) to release the
// file, it watches the size stop changing and confirms the file can be opened
// with full sharing. This reacts in about 50-100ms instead of stalling for
// seconds behind another process's handle. A half-written file simply fails to
// decode later, so acting slightly early is safe.
static bool WaitForStableFile(const std::wstring& path) {
    LONGLONG lastSize = -1;
    for (int i = 0; i < 40; i++) {  // ~2s cap at 50ms steps.
        WIN32_FILE_ATTRIBUTE_DATA info;
        if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &info)) {
            LONGLONG size =
                ((LONGLONG)info.nFileSizeHigh << 32) | info.nFileSizeLow;
            if (size > 0 && size == lastSize) {
                HANDLE h = CreateFileW(
                    path.c_str(), GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    nullptr, OPEN_EXISTING, 0, nullptr);
                if (h != INVALID_HANDLE_VALUE) {
                    CloseHandle(h);
                    return true;
                }
            }
            lastSize = size;
        } else {
            DWORD err = GetLastError();
            if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
                return false;  // Deleted before we got to it.
            }
        }
        if (WaitStop(50)) {
            return false;
        }
    }
    // Size never settled; proceed only if the file still exists.
    return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

static void ProcessOne(std::wstring path) {
    Settings s = SnapshotSettings();

    DWORD t0 = GetTickCount();
    if (!WaitForStableFile(path)) {
        return;
    }
    if (s.logDetails) {
        Wh_Log(L"stable in %lu ms: %s", GetTickCount() - t0, path.c_str());
    }

    // Optional: rename to match the window in front, before anything downstream
    // (clipboard payloads and deletion) uses the path.
    if (s.renameFromWindow) {
        std::wstring renamed = RenameByActiveWindow(path);
        if (!renamed.empty()) {
            path = std::move(renamed);
            if (s.logDetails) {
                Wh_Log(L"renamed to %s", path.c_str());
            }
        }
    }

    DWORD t1 = GetTickCount();
    int action = s.popup ? ChooseAction(path, s) : ACTION_AUTO;
    if (s.logDetails) {
        Wh_Log(L"popup returned %d in %lu ms", action, GetTickCount() - t1);
    }
    // Keep, or anything we don't recognize, leaves the file alone. "Unknown means
    // delete" is the wrong way round for this mod.
    if (action != ACTION_AUTO && action != ACTION_DELETE &&
        action != ACTION_COPY_DELETE) {
        return;
    }
    if (action == ACTION_DELETE) {
        DeleteWatched(path, s);
        return;
    }

    // ACTION_COPY_DELETE forces the durable image path + deletion regardless of
    // the configured clipboard mode. ACTION_AUTO / timeout uses the settings.
    bool forceImage = (action == ACTION_COPY_DELETE);

    bool copied;
    if (forceImage || s.mode == L"image") {
        copied = ClipboardImage(path);
    } else if (s.mode == L"file") {
        copied = ClipboardFile(path);
    } else if (s.mode == L"path") {
        copied = ClipboardText(FormatPathText(path, s.pathFormat));
    } else {  // "none"
        copied = true;
    }

    if (!copied) {
        Wh_Log(L"Clipboard copy failed%s",
               s.logDetails ? (L": " + path).c_str() : L"");
        return;  // Invariant: never delete when a requested copy failed.
    }

    // Deletion only makes sense for self-contained payloads (image / none).
    // File and Path payloads reference the file, so deleting would break them.
    bool payloadReferencesFile =
        !forceImage && (s.mode == L"file" || s.mode == L"path");
    bool wantsDelete = forceImage || (s.deleteFile && !payloadReferencesFile);
    if (!wantsDelete) {
        return;
    }

    // With the popup, the countdown already elapsed, so delete immediately.
    DWORD delay = (s.popup || forceImage) ? 0 : (DWORD)s.delaySeconds * 1000;
    if (!WaitStop(delay)) {
        DeleteWatched(path, s);
    }
}

// ============================================================================
// Worker: drains the queue on a dedicated COM (STA) thread
// ============================================================================

static bool DequeueOne(std::wstring& path) {
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
        if (entry.first == name) {
            return true;
        }
    }
    return false;
}

static void ReleaseInflight(const std::wstring& path) {
    std::wstring name = path.substr(path.find_last_of(L"\\/") + 1);
    ULONGLONG now = GetTickCount64();
    EnterCriticalSection(&g_lock);
    g_inflight.erase(name);
    g_recent.push_back({name, now + kDuplicateEventWindowMs});
    LeaveCriticalSection(&g_lock);
}

// Whether the Start Menu shortcut and the CLSID key are currently in place.
// Worker thread only: the shortcut work reads and writes COM objects and needs
// this thread's apartment.
static bool g_toastRegApplied = false;

// The shortcut and the registry key exist for one reason, to let a toast button
// activate us, so they follow the popup setting instead of being written
// unconditionally at startup. With the popup off there is no notification, and a
// SnapSentry entry sitting in Start and in Search would be there for nothing.
static void SyncToastRegistration(bool wantPopup) {
    if (wantPopup == g_toastRegApplied) {
        return;
    }
    if (wantPopup) {
        g_toastRegApplied = true;  // Even on a partial failure, so teardown cleans up.
        bool aumidOk = EnsureAumidRegistered();
        bool clsidOk = EnsureClsidRegistered();
        HRESULT regHr = E_FAIL;
        if (aumidOk && clsidOk) {
            regHr = CoRegisterClassObject(
                CLSID_SnapSentryToastActivator, &g_toastActivatorFactory,
                CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &g_toastActivatorCookie);
        }
        g_toastRegistered = aumidOk && clsidOk && SUCCEEDED(regHr);
        if (g_toastRegistered.load()) {
            Wh_Log(L"Toast notifications ready");
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
    RemoveAumidRegistration();
    RemoveClsidRegistration();
    g_toastRegApplied = false;
}

static DWORD WINAPI WorkerThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    RoInitialize(RO_INIT_SINGLETHREADED);

    SyncToastRegistration(SnapshotSettings().popup);

    // CoWaitForMultipleHandles (rather than plain WaitForMultipleObjects) pumps
    // incoming COM calls -- needed so a toast button click can be dispatched to
    // ToastActivator::Activate even while idling between screenshots.
    HANDLE waits[] = {g_stopEvent, g_workEvent};
    while (true) {
        DWORD idx = 0;
        HRESULT hr = CoWaitForMultipleHandles(
            COWAIT_DISPATCH_CALLS | COWAIT_DISPATCH_WINDOW_MESSAGES, INFINITE,
            ARRAYSIZE(waits), waits, &idx);
        if (FAILED(hr) || idx == 0) {
            break;  // Stop (or an unexpected error -- treat as shutdown).
        }
        // Settings changes wake us here too, so the popup being switched on or off
        // takes effect without a reload.
        SyncToastRegistration(SnapshotSettings().popup);
        std::wstring path;
        while (!WaitStop(0) && DequeueOne(path)) {
            ProcessOne(path);
            ReleaseInflight(path);
        }
    }

    // Leave nothing behind. Done here, while this thread's COM apartment is still
    // live, because removing the shortcut has to read its properties through COM;
    // the uninit thread has no apartment. Everything is recreated on the next load.
    SyncToastRegistration(false);

    RoUninitialize();
    CoUninitialize();
    return 0;
}

static void Enqueue(const std::wstring& folder, const std::wstring& name) {
    if (!IsSafeChildName(name) || !IsSupportedImage(name)) {
        return;
    }
    ULONGLONG now = GetTickCount64();
    EnterCriticalSection(&g_lock);
    // Skip names present when watching started (g_preexisting), then dedup rapid
    // duplicate events (g_inflight) and the delayed follow-up event for a name we
    // just finished handling (g_recent), such as OneDrive rewriting the file as a
    // placeholder after the first notification closed.
    bool added = !SeenRecently(name, now) && g_preexisting.count(name) == 0 &&
                 g_inflight.insert(name).second;
    if (added) {
        g_queue.push_back(folder + L"\\" + name);
    }
    bool logDetails = g_settings.logDetails;
    LeaveCriticalSection(&g_lock);
    if (added) {
        SetEvent(g_workEvent);
        if (logDetails) {
            Wh_Log(L"detected %s", name.c_str());
        }
    }
}

// ============================================================================
// Watcher: overlapped ReadDirectoryChangesW with a clean stop/reload path
// ============================================================================

static void ParseNotifications(const std::wstring& folder, const BYTE* buffer) {
    for (auto* info = (const FILE_NOTIFY_INFORMATION*)buffer;;) {
        if (info->Action == FILE_ACTION_ADDED ||
            info->Action == FILE_ACTION_RENAMED_NEW_NAME) {
            std::wstring name(info->FileName,
                              info->FileNameLength / sizeof(wchar_t));
            Enqueue(folder, name);
        }
        if (!info->NextEntryOffset) {
            break;
        }
        info = (const FILE_NOTIFY_INFORMATION*)((const BYTE*)info +
                                                info->NextEntryOffset);
    }
}

// Records the folder's current file names so hydration or sync churn on files
// that predate this watch session is never treated as a new screenshot. Called
// each time the directory is opened, before the first change notification is
// armed. Placeholders are enumerated like any other entry, so dehydrated old
// screenshots are captured too.
static void SnapshotExistingNames(const std::wstring& folder) {
    std::set<std::wstring> names;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((folder + L"\\*").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                names.insert(fd.cFileName);
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    EnterCriticalSection(&g_lock);
    g_preexisting = std::move(names);
    LeaveCriticalSection(&g_lock);
}

static DWORD WINAPI WatchThread(LPVOID) {
    OVERLAPPED ov{};
    ov.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ov.hEvent) {
        return 0;
    }
    HANDLE readyWaits[] = {g_stopEvent, g_reloadEvent, ov.hEvent};

    while (!WaitStop(0)) {
        Settings s = SnapshotSettings();
        if (!s.enabled || s.folder.empty()) {
            HANDLE idle[] = {g_stopEvent, g_reloadEvent};
            WaitForMultipleObjects(2, idle, FALSE, 500);
            continue;
        }

        HANDLE dir = CreateFileW(
            s.folder.c_str(), FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            nullptr);
        if (dir == INVALID_HANDLE_VALUE) {
            HANDLE idle[] = {g_stopEvent, g_reloadEvent};
            WaitForMultipleObjects(2, idle, FALSE, 2000);  // Folder may appear later.
            continue;
        }

        // Snapshot existing names before arming notifications so downloads or
        // sync churn on pre-existing files never look like new screenshots.
        SnapshotExistingNames(s.folder);

        // ReadDirectoryChangesW writes FILE_NOTIFY_INFORMATION records that must
        // be DWORD aligned; alignas guarantees it (a plain BYTE array need not be).
        alignas(DWORD) BYTE buffer[16384];
        bool reopen = false;
        while (!reopen) {
            ResetEvent(ov.hEvent);
            DWORD bytes = 0;
            if (!ReadDirectoryChangesW(
                    dir, buffer, sizeof(buffer), FALSE,
                    FILE_NOTIFY_CHANGE_FILE_NAME,
                    &bytes, &ov, nullptr)) {
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
                // Buffer overflow: notifications were dropped. We can't tell which
                // files are new, and rescanning could delete pre-existing files,
                // so we deliberately skip rather than risk the safety invariant.
                Wh_Log(L"Change buffer overflow; some screenshots may be skipped");
                continue;
            }
            ParseNotifications(s.folder, buffer);
        }
        CloseHandle(dir);
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
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);   // Manual reset.
    g_reloadEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);  // Auto reset.
    g_workEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);    // Auto reset.
    g_toastActionEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);  // Auto reset.
    if (!g_stopEvent || !g_reloadEvent || !g_workEvent || !g_toastActionEvent) {
        if (g_stopEvent) CloseHandle(g_stopEvent);
        if (g_reloadEvent) CloseHandle(g_reloadEvent);
        if (g_workEvent) CloseHandle(g_workEvent);
        if (g_toastActionEvent) CloseHandle(g_toastActionEvent);
        g_stopEvent = g_reloadEvent = g_workEvent = g_toastActionEvent = nullptr;
        DeleteCriticalSection(&g_lock);
        DeleteCriticalSection(&g_toastLock);
        return FALSE;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    g_watchThread = CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
    if (!g_workerThread || !g_watchThread) {
        SetEvent(g_stopEvent);
        SetEvent(g_workEvent);
        if (g_watchThread) WaitForSingleObject(g_watchThread, INFINITE);
        if (g_workerThread) WaitForSingleObject(g_workerThread, INFINITE);
        if (g_watchThread) CloseHandle(g_watchThread);
        if (g_workerThread) CloseHandle(g_workerThread);
        CloseHandle(g_stopEvent);
        CloseHandle(g_reloadEvent);
        CloseHandle(g_workEvent);
        CloseHandle(g_toastActionEvent);
        g_watchThread = g_workerThread = nullptr;
        g_stopEvent = g_reloadEvent = g_workEvent = g_toastActionEvent = nullptr;
        DeleteCriticalSection(&g_lock);
        DeleteCriticalSection(&g_toastLock);
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    SetEvent(g_reloadEvent);  // Re-open the folder in case the path changed.
    SetEvent(g_workEvent);    // Wake the worker so it can follow the popup setting.
}

void WhTool_ModUninit() {
    SetEvent(g_stopEvent);

    // Dismiss an open action dialog as "Keep" so shutdown never triggers a delete.
    HWND dlg = g_dialog.load();
    if (dlg) {
        SendMessageW(dlg, TDM_CLICK_BUTTON, ACTION_KEEP, 0);
    }
    SetEvent(g_workEvent);

    HANDLE threads[] = {g_watchThread, g_workerThread};
    // Both threads observe g_stopEvent and must be gone before their events and
    // critical sections are destroyed.
    WaitForMultipleObjects(2, threads, TRUE, INFINITE);
    if (g_watchThread) {
        CloseHandle(g_watchThread);
    }
    if (g_workerThread) {
        CloseHandle(g_workerThread);
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
    }
    if (g_reloadEvent) {
        CloseHandle(g_reloadEvent);
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
// Windhawk tool mod boilerplate. Runs the mod in a dedicated windhawk.exe
// process instead of injecting into other processes. See:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process

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
