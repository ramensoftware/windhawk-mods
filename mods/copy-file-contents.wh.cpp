// ==WindhawkMod==
// @id              copy-file-contents
// @name            Copy File Contents
// @description     Copies a selected file's text or its SHA-256 hash without opening it in an app, from a keyboard shortcut or the right-click menu.
// @version         1.4.1
// @author          metanoid18
// @github          https://github.com/Metanoid18
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -lshell32 -lbcrypt
// ==/WindhawkMod==

// Source code is published under the MIT license.

// ==WindhawkModReadme==
/*
# Copy File Contents

Adds two actions to File Explorer, so a file can be read without opening it in an app:

* **Copy contents** puts the text of the selected file on the clipboard.
* **Copy SHA-256** puts the hash of the selected file on the clipboard.

Several files can be selected at once. Contents are copied one after another with
the file name above each, and hashes are copied one per line. A setting can put
each file on the clipboard as its own item instead, which keeps them apart in
Windows clipboard history, so Win+V offers each file on its own.

## Two ways to reach it

**Keyboard.** Select files, then press Ctrl+Shift+T for the contents or
Ctrl+Shift+H for the hash. This works with the new Windows 11 menu, because it
does not go through the menu at all. The shortcut is only live while a File
Explorer window has the focus and the file view is the one being pointed at, and
it is ignored while the rename box, the address bar or the search box has the
focus, so it never eats a letter you are typing. Every key combination here can
be changed in the settings. Do not reuse Ctrl+Shift+C or Ctrl+Shift+N, which are
File Explorer's own Copy as path and New folder, because the mod would take the
key from Explorer.

![Classic File Explorer menu with both entries](https://raw.githubusercontent.com/Metanoid18/windhawk-mods/copy-file-contents-shots/screenshots/copy-file-contents-menu.png)

**Right-click.** The two entries are also added to the classic menu, reached
with **Show more options** or Shift+F10. They are added at the bottom of that
menu, below Properties, and removed again once the menu closes, so the menu is
never left modified. The mod does not add entries to the new Windows 11 menu.
To get the entries on a plain right-click, use
the separate mod **Classic context menu on Windows 11**. If the classic menu
cannot be reached on your build, the keyboard shortcuts are the whole of the mod
and they do not need the menu at all.

## What it will and will not copy

A file is copied only if it looks like text. Files that look binary are refused
rather than pasted as garbage, and files larger than the size limit are refused
the same way. When no file contents can be read, an optional prompt shows the
reason. A clipboard write failure with no successful item is reported separately.
For an oversized local
file up to 32 MB whose prefix looks like text, the optional prompt offers
to open it in Notepad instead. This prefix check is only a heuristic. Network and
offline files are not opened for the offer. The path is passed quoted so a path
with a space in it still opens.

The size limit is 8 MB by default, with the unit chosen next to it, megabytes or
kilobytes. A large selection is copied file by file, so one file over the limit is
skipped and the rest are still copied. That partial copy is silent, because a
dialog in the middle of copying five files is worse than a missing one. In joined
mode, a multi-file selection identifies each result even if only one file succeeds.
When nothing at all can be read, the optional prompt explains why instead.

The decoded text in one selection is held to a 32 MB budget. A file bigger on
disk than what is left of that budget is skipped without being opened, even if
its decoded text might have fit. Once the remaining budget is very small, the rest of the
selection is not read at all. Smaller files are read and their decoded text is
measured: depending on the encoding it may be smaller or larger than the bytes on
disk. File names and temporary buffers are outside the budget, so total memory use
can be higher.

The ceilings are 64 MB per file and 32 MB per selection whatever the settings
say. In practice the per selection budget is the one that binds: no single file is
copied beyond 32 MB of text, which is the same as saying that a 20 MB UTF-16 file
is copied and a 20 MB ASCII file is not. A text file that big is already beyond
what a clipboard is good for, and the mod refuses rather than trying.

When the selection appears uncopyable, **Copy contents** is usually left out of
the menu instead of being offered and then refusing. To decide that, the first 4 KiB
of each file are read when the menu opens, stopping at the first file that
passes, and looking at no more than 16 files. Files that are offline or stored
online only are not read at all, because reading one would start a download; such
a file is assumed copyable and the copy itself decides. Files on a network share
are treated the same way, whether they are reached by a `\\server\share` path or
through a drive letter mapped to one, because reading from a share that has
stopped answering would hang the menu. A mixed selection still shows the entry
and copies the files that can be copied.

Encoding is detected rather than assumed: a byte order mark is honored, then the
file is checked for UTF-16 without a mark, then for strict UTF-8, and finally read
as the system code page. UTF-16 is checked before UTF-8 on purpose: the zero bytes
of a UTF-16 file are valid UTF-8, so the other order would decode such a file as
text with invisible gaps. Line endings are copied as they are.

## Large hashes

Hashing reads every byte, and the work runs on the same thread that owns the File
Explorer window, so that window does not repaint until it is done. Large files
and slow drives or shares can take a long time. Above the size set in the
settings, 256 MB by default, the mod asks first, and says how many
files and how much data are involved, using the sizes it could read across the
selection rather than one file at a time. Answer no and nothing happens. The
setting can be raised to where hashing a whole disk in one go is on you, or set
to 0 to be asked every time.
If no selected file can be hashed, a message warns that no hash was copied and
the clipboard may contain earlier data. If some files fail, successful hashes
are copied without a dialog, with file names when more than one file (not a folder)
was selected.

## Known limits

* Only files. A selection of folders, or the desktop, does nothing.
* The shortcuts act in File Explorer windows only. They do nothing in other
  programs and nothing on the desktop.
* The contents are put on the clipboard as text. Nothing is written to disk, and
  no temporary file is created.
* When the mod is turned off, it asks open menus and its prompts to close and
  waits for tracked hook calls to finish. A menu or prompt that does not close
  may need to be closed manually. The mod attempts to cancel its pending file
  operations when unloading, but cancellation is not guaranteed. A stalled
  operation may still leave Windhawk uninitializing indefinitely; the Explorer
  window can remain unresponsive. Cancellation targets a thread, so unrelated
  I/O reentered on that thread during a mod file call could also be canceled.
  A dialog opened by another Explorer shortcut, or another mod's hook, may also
  delay uninitialization until it closes or returns. Other mod
  updates in the process may have to wait. The delay between separate clipboard
  items ends early when unloading starts.
* With separate clipboard items turned on, Ctrl+V pastes only the last file. The
  earlier ones are reachable through Windows clipboard history (Win+V), and only
  there, and clipboard history has to be turned on for them to be kept.
* Clipboard history keeps 25 entries and about 4 MB per entry, which for text is
  roughly two million characters. A file bigger than that cannot be a history
  entry however it is copied; Windows drops it and tells no one. So in separate
  items mode those files are written last: the history keeps the files it can
  hold, and Ctrl+V gives the big one.
* Windows clipboard history is a separate service and it needs a moment between
  copies, so the files are written with a short wait, 250 ms by default and
  adjustable. The wait runs on File Explorer's thread, so a large selection takes
  that much longer to finish copying, and the window is briefly unresponsive
  while it does.
* A file being written by another program while it is read may be copied partially.
  That is a race no reader can win, and it is not worth a lock for this.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxSize: 8
  $name: Largest file to copy
  $description: Files bigger than this are skipped. If nothing is copied, an oversized local file up to 32 MB may be offered in Notepad if its prefix looks like text. The unit is the setting below it, and 64 MB is the most this can ever be. A megabyte here is 1,048,576 bytes, the same as Windows shows in the file listing. One copy holds at most 32 MB of text in all, so no single file is copied beyond that, whatever this is set to.
- maxSizeUnit: mb
  $name: Size unit
  $description: Megabytes or kilobytes. Whole numbers only; for one and a half megabytes, pick kilobytes and enter 1536.
  $options:
  - mb: Megabytes
  - kb: Kilobytes
- copyContents: true
  $name: Show "Copy contents"
  $description: Adds the entry that puts the file's text on the clipboard.
- copyHash: true
  $name: Show "Copy SHA-256"
  $description: Adds the entry that puts the file's SHA-256 hash on the clipboard.
- offerEditor: true
  $name: Offer to open a file that cannot be copied
  $description: When no file contents can be read, report the reason. Offer Notepad only for an oversized local file up to 32 MB whose prefix looks like text. This does not control warnings for a failed clipboard write or a hash operation with no result.
- hideWhenUncopyable: true
  $name: Hide "Copy contents" when nothing can be copied
  $description: When every selected file is binary or over the size limit, the entry is left out of the menu instead of being offered and then refusing. The first few KiB of each file are read to decide, except files that are offline, stored online only, or on a network share or a mapped network drive, which are not read.
- separateEntries: false
  $name: Put each file on the clipboard as its own item
  $description: When several files are copied, each one goes on the clipboard in turn instead of being joined into one text. Windows clipboard history (Win+V) then lists them separately and you can paste any of them. Ctrl+V pastes only the last file. Needs clipboard history turned on, otherwise only the last file is reachable, and history keeps 25 entries of about 4 MB each, so a file over that limit is written last and can only be pasted with Ctrl+V.
- separateGapMs: 250
  $name: Wait between separate items, in milliseconds
  $description: Windows clipboard history ignores copies that arrive too close together, so the files are written with a short wait between them. Raise this if some files still go missing from the history, lower it if the copy feels slow. 0 turns the wait off.
- askHashMB: 256
  $name: Ask before hashing files bigger than this in all, in MB
  $description: Hashing reads every byte and the File Explorer window waits until it is done. Above this much in total the mod asks first, using the file sizes it could read. 0 asks for every file whose size can be read.
- hotkeys: true
  $name: Keyboard shortcuts
  $description: Turn the two shortcuts on. They only act in a File Explorer window with files selected, and never while you type in a rename box, the address bar or the search box.
- contentsModifiers: ctrl_shift
  $name: Modifiers for the contents shortcut
  $description: Ctrl+Shift is the default. Combinations with Alt are not offered, because File Explorer then waits for the rest of a menu shortcut and gets stuck in that state.
  $options:
  - ctrl_shift: Ctrl+Shift
  - win_shift: Win+Shift
- contentsKey: T
  $name: Key for the contents shortcut
  $description: A letter, a digit, or F1 to F24
- hashModifiers: ctrl_shift
  $name: Modifiers for the hash shortcut
  $description: Ctrl+Shift is the default. Combinations with Alt are not offered, because File Explorer then waits for the rest of a menu shortcut and gets stuck in that state.
  $options:
  - ctrl_shift: Ctrl+Shift
  - win_shift: Win+Shift
- hashKey: H
  $name: Key for the hash shortcut
  $description: A letter, a digit, or F1 to F24
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <shobjidl.h>
#include <bcrypt.h>

#include <atomic>
#include <climits>
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

// Settings are updated on Windhawk's engine thread and read by Explorer hooks.
// Atomic fields avoid data races; one operation may still see a mix of old and
// new settings while an update is in progress.
struct Settings {
    std::atomic<int> maxSize{8};
    std::atomic<bool> maxSizeInMb{true};
    std::atomic<bool> copyContents{true};
    std::atomic<bool> copyHash{true};
    std::atomic<bool> offerEditor{true};
    std::atomic<bool> hideWhenUncopyable{true};
    std::atomic<bool> separateEntries{false};
    std::atomic<int> separateGapMs{250};
    std::atomic<int> askHashMB{256};
    std::atomic<bool> hotkeys{true};
    std::atomic<UINT> contentsModifiers{MOD_CONTROL | MOD_SHIFT};
    std::atomic<UINT> contentsKey{'T'};
    std::atomic<UINT> hashModifiers{MOD_CONTROL | MOD_SHIFT};
    std::atomic<UINT> hashKey{'H'};
};

Settings g_settings;

UINT ParseModifiers(const wchar_t* text, UINT fallback) {
    if (wcscmp(text, L"ctrl_shift") == 0) return MOD_CONTROL | MOD_SHIFT;
    if (wcscmp(text, L"win_shift") == 0) return MOD_WIN | MOD_SHIFT;
    return fallback;
}

// A letter, a digit, or F1 to F24. Anything else falls back to the default,
// because a setting the mod cannot read must not leave the mod without a key.
UINT ParseKey(const wchar_t* text, UINT fallback) {
    if (!text || !*text) {
        return fallback;
    }
    if (!text[1]) {
        wchar_t c = text[0];
        if (c >= L'a' && c <= L'z') {
            c = static_cast<wchar_t>(c - L'a' + L'A');
        }
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            return static_cast<UINT>(c);
        }
        return fallback;
    }
    if (text[0] == L'F' || text[0] == L'f') {
        int number = 0;
        for (const wchar_t* p = text + 1; *p; p++) {
            if (*p < L'0' || *p > L'9') {
                return fallback;
            }
            number = number * 10 + (*p - L'0');
            if (number > 24) {
                return fallback;
            }
        }
        if (number >= 1) {
            return static_cast<UINT>(VK_F1 + number - 1);
        }
    }
    return fallback;
}

void LoadSettings() {
    int maxSize = Wh_GetIntSetting(L"maxSize");
    if (maxSize <= 0) {
        maxSize = 8;
    }
    // Clamp before publishing so a concurrent hook cannot see an invalid size.
    if (maxSize > 65536) {
        maxSize = 65536;
    }
    g_settings.maxSize = maxSize;
    // Only the two units are offered, and anything unreadable means megabytes,
    // which is the default.
    WindhawkUtils::StringSetting unit =
        WindhawkUtils::StringSetting::make(L"maxSizeUnit");
    g_settings.maxSizeInMb = wcscmp(unit, L"kb") != 0;
    g_settings.copyContents = Wh_GetIntSetting(L"copyContents") != 0;
    g_settings.copyHash = Wh_GetIntSetting(L"copyHash") != 0;
    g_settings.offerEditor = Wh_GetIntSetting(L"offerEditor") != 0;
    g_settings.hideWhenUncopyable =
        Wh_GetIntSetting(L"hideWhenUncopyable") != 0;
    g_settings.separateEntries = Wh_GetIntSetting(L"separateEntries") != 0;
    int gapMs = Wh_GetIntSetting(L"separateGapMs");
    if (gapMs < 0) {
        gapMs = 0;
    }
    if (gapMs > 2000) {
        gapMs = 2000;
    }
    g_settings.separateGapMs = gapMs;
    int askHashMB = Wh_GetIntSetting(L"askHashMB");
    if (askHashMB < 0) {
        askHashMB = 0;
    }
    g_settings.askHashMB = askHashMB;
    g_settings.hotkeys = Wh_GetIntSetting(L"hotkeys") != 0;
    WindhawkUtils::StringSetting contentsMods =
        WindhawkUtils::StringSetting::make(L"contentsModifiers");
    WindhawkUtils::StringSetting contentsKey =
        WindhawkUtils::StringSetting::make(L"contentsKey");
    WindhawkUtils::StringSetting hashMods =
        WindhawkUtils::StringSetting::make(L"hashModifiers");
    WindhawkUtils::StringSetting hashKey =
        WindhawkUtils::StringSetting::make(L"hashKey");
    g_settings.contentsModifiers =
        ParseModifiers(contentsMods, MOD_CONTROL | MOD_SHIFT);
    g_settings.contentsKey = ParseKey(contentsKey, 'T');
    g_settings.hashModifiers = ParseModifiers(hashMods, MOD_CONTROL | MOD_SHIFT);
    g_settings.hashKey = ParseKey(hashKey, 'H');
}

// ---------------------------------------------------------------------------
// Work in progress, and unloading
//
// Reading, hashing and the prompts all run on File Explorer's own thread, and
// the mod's code is on that thread's stack the whole time. If the mod is
// disabled or updated in the middle, it must not be unloaded while that stack
// is still in the mod. Such stretches are counted, and Wh_ModBeforeUninit tries
// to close open menus and prompts and waits briefly for the count to reach zero.
// ---------------------------------------------------------------------------

std::atomic<bool> g_unloading{false};
// Signalled by Wh_ModBeforeUninit. The wait between separate clipboard items
// waits on this instead of sleeping, so a 2000 ms gap does not hold up an
// unload for two seconds.
HANDLE g_unloadEvent = nullptr;
// Popup calls and copies in progress, and active hook calls of all three kinds.
std::atomic<int> g_busy{0};
std::atomic<int> g_activeHooks{0};
struct HookGuard {
    HookGuard() { g_activeHooks++; }
    ~HookGuard() { g_activeHooks--; }
};
// Only the outer popup call adds entries, and only one copy runs at a time per
// window thread.
thread_local bool t_inMenuHook = false;
thread_local bool t_copying = false;

// Several windows can hold an open menu at once, so the owners are kept in a
// small list rather than one slot: unloading has to cancel every one of them.
constexpr int kMaxOwners = 8;
std::atomic<HWND> g_ownerSlots[kMaxOwners];
std::atomic<DWORD> g_ownerThreads[kMaxOwners];
SRWLOCK g_ownerLock = SRWLOCK_INIT;

// Which prompts belong to this mod, so unloading can close them.
const wchar_t kContentsTitle[] = L"Copy File Contents";
const wchar_t kHashTitle[] = L"Copy SHA-256";

struct BusyGuard {
    explicit BusyGuard(HWND owner) : slot(-1) {
        g_busy++;
        RegisterOwner(owner);
    }
    void RegisterOwner(HWND owner) {
        if (!owner || slot >= 0) {
            return;
        }
        AcquireSRWLockExclusive(&g_ownerLock);
        for (int i = 0; i < kMaxOwners; i++) {
            if (!g_ownerSlots[i]) {
                slot = i;
                g_ownerThreads[i] = GetCurrentThreadId();
                g_ownerSlots[i] = owner;
                break;
            }
        }
        ReleaseSRWLockExclusive(&g_ownerLock);
    }
    void UnregisterOwner() {
        if (slot >= 0) {
            AcquireSRWLockExclusive(&g_ownerLock);
            g_ownerSlots[slot] = nullptr;
            g_ownerThreads[slot] = 0;
            ReleaseSRWLockExclusive(&g_ownerLock);
            slot = -1;
        }
    }
    ~BusyGuard() {
        UnregisterOwner();
        g_busy--;
    }
    int slot;
};

// ---------------------------------------------------------------------------
// Limits
// ---------------------------------------------------------------------------

constexpr long long kHardMaxBytes = 64LL * 1024LL * 1024LL;
constexpr size_t kTotalTextBytes = 32 * 1024 * 1024;
// Clipboard history keeps about 4 MB per entry. Text is UTF-16, so that is
// roughly two million characters.
constexpr size_t kHistoryMaxChars = (4 * 1024 * 1024) / sizeof(wchar_t);

long long SizeLimitBytes() {
    const long long unit = g_settings.maxSizeInMb ? 1024LL * 1024LL : 1024LL;
    const int maxSize = g_settings.maxSize.load();
    const long long value = maxSize > 0 ? maxSize : 8;
    const long long bytes = value * unit;
    if (bytes <= 0) {
        return 8LL * 1024LL * 1024LL;
    }
    return bytes > kHardMaxBytes ? kHardMaxBytes : bytes;
}

bool TooBigForHistory(const std::wstring& text) {
    return text.size() > kHistoryMaxChars;
}

// Each file operation is registered only around the system call itself. The
// unload thread holds this lock while it requests cancellation; a returning
// operation must acquire it before continuing, so a stale registration cannot
// cancel a later, unrelated Explorer operation after this call returns.
struct IoOperation;
SRWLOCK g_ioLock = SRWLOCK_INIT;
IoOperation* g_ioOperations = nullptr;

struct IoOperation {
    HANDLE thread = nullptr;
    IoOperation* next = nullptr;
    bool allowed = false;

    IoOperation() {
        HANDLE copy = nullptr;
        DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                        GetCurrentProcess(), &copy, THREAD_TERMINATE, FALSE, 0);
        AcquireSRWLockExclusive(&g_ioLock);
        if (!g_unloading) {
            allowed = true;
            thread = copy;
            if (thread) {
                next = g_ioOperations;
                g_ioOperations = this;
                copy = nullptr;
            }
        }
        ReleaseSRWLockExclusive(&g_ioLock);
        if (copy) CloseHandle(copy);
    }

    ~IoOperation() {
        // Neither lock/handle cleanup nor a cancellation request should change
        // the result a caller sees from the underlying Windows API.
        DWORD error = GetLastError();
        if (thread) {
            AcquireSRWLockExclusive(&g_ioLock);
            IoOperation** link = &g_ioOperations;
            while (*link && *link != this) link = &(*link)->next;
            if (*link == this) *link = next;
            CloseHandle(thread);
            ReleaseSRWLockExclusive(&g_ioLock);
        }
        SetLastError(error);
    }

    IoOperation(const IoOperation&) = delete;
    IoOperation& operator=(const IoOperation&) = delete;
};

template <typename Function, typename Result>
Result CancelableFileCall(Function function, Result aborted) {
    IoOperation operation;
    if (!operation.allowed || g_unloading) {
        SetLastError(ERROR_OPERATION_ABORTED);
        return aborted;
    }
    return function();
}

HANDLE OpenFileForRead(const std::wstring& path) {
    return CancelableFileCall([&] {
        return CreateFileW(path.c_str(), GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    }, INVALID_HANDLE_VALUE);
}
DWORD ReadFileAttributes(const std::wstring& path) {
    return CancelableFileCall([&] { return GetFileAttributesW(path.c_str()); },
                              static_cast<DWORD>(INVALID_FILE_ATTRIBUTES));
}
BOOL QueryFileSize(HANDLE file, LARGE_INTEGER* size) {
    return CancelableFileCall([&] { return GetFileSizeEx(file, size); }, FALSE);
}
BOOL ReadFileChunk(HANDLE file, void* buffer, DWORD want, DWORD* read) {
    return CancelableFileCall(
        [&] { return ReadFile(file, buffer, want, read, nullptr); }, FALSE);
}
UINT QueryDriveType(const wchar_t* root) {
    return CancelableFileCall([&] { return GetDriveTypeW(root); },
                              static_cast<UINT>(DRIVE_UNKNOWN));
}

void CancelActiveIo() {
    AcquireSRWLockExclusive(&g_ioLock);
    for (IoOperation* operation = g_ioOperations; operation;
         operation = operation->next) {
        CancelSynchronousIo(operation->thread);
    }
    ReleaseSRWLockExclusive(&g_ioLock);
}

// ---------------------------------------------------------------------------
// The selection
//
// The shell view answers WM_USER + 7 with its IShellBrowser. That message is a
// WM_USER message, private to one window class, so it is sent to exactly one
// window: the ShellTabWindowClass ancestor of the menu owner. Sending it up the
// whole parent chain would also hit ReBarWindow32, whose WM_USER + 7 is
// RB_SETPARENT, with an HWND as the answer that this code would then call as a
// COM object.
// ---------------------------------------------------------------------------

constexpr UINT kMsgGetShellBrowser = WM_USER + 7;

bool HasClass(HWND hWnd, const wchar_t* name) {
    wchar_t cls[64];
    return hWnd && GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
           wcscmp(cls, name) == 0;
}

bool IsExplorerFrame(HWND hWnd) {
    return HasClass(hWnd, L"CabinetWClass") || HasClass(hWnd, L"ExploreWClass");
}

// True when the window sits inside the file view of a File Explorer window. The
// navigation pane, the address bar and the rest of the frame are outside it.
bool InFileView(HWND hWnd) {
    for (HWND h = hWnd; h; h = GetAncestor(h, GA_PARENT)) {
        if (HasClass(h, L"SHELLDLL_DefView")) {
            return true;
        }
    }
    return false;
}

// The shell browser behind a window in the file view, or null. WM_USER + 7
// returns a borrowed pointer, so a reference is taken here and the caller
// releases exactly that one. Releasing without this AddRef would take a
// reference away from the shell, which owns the object, and explorer.exe would
// eventually free it while still using it.
IShellBrowser* BrowserForFileView(HWND hWnd) {
    if (!hWnd || !IsExplorerFrame(GetAncestor(hWnd, GA_ROOT))) {
        return nullptr;
    }
    bool inFileView = false;
    for (HWND h = hWnd; h; h = GetAncestor(h, GA_PARENT)) {
        if (!inFileView) {
            if (HasClass(h, L"SHELLDLL_DefView")) {
                inFileView = true;
            }
            continue;
        }
        if (HasClass(h, L"ShellTabWindowClass")) {
            IShellBrowser* browser = reinterpret_cast<IShellBrowser*>(
                SendMessageW(h, kMsgGetShellBrowser, 0, 0));
            if (browser) {
                browser->AddRef();
            }
            return browser;
        }
        if (HasClass(h, L"CabinetWClass")) {
            break;
        }
    }
    return nullptr;
}

std::vector<std::wstring> PsFromBrowser(IShellBrowser* browser) {
    std::vector<std::wstring> paths;
    if (!browser) {
        return paths;
    }
    IShellView* view = nullptr;
    if (FAILED(browser->QueryActiveShellView(&view)) || !view) {
        return paths;
    }
    IDataObject* data = nullptr;
    if (SUCCEEDED(view->GetItemObject(SVGIO_SELECTION, IID_IDataObject,
                                      reinterpret_cast<void**>(&data))) &&
        data) {
        FORMATETC format = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1,
                            TYMED_HGLOBAL};
        STGMEDIUM medium = {};
        if (SUCCEEDED(data->GetData(&format, &medium))) {
            HDROP drop = static_cast<HDROP>(GlobalLock(medium.hGlobal));
            if (drop) {
                const UINT count = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
                for (UINT i = 0; i < count; i++) {
                    const UINT length = DragQueryFileW(drop, i, nullptr, 0);
                    if (!length) {
                        continue;
                    }
                    std::wstring path(length + 1, L'\0');
                    DragQueryFileW(drop, i, &path[0], length + 1);
                    path.resize(length);
                    paths.push_back(path);
                }
                GlobalUnlock(medium.hGlobal);
            }
            ReleaseStgMedium(&medium);
        }
        data->Release();
    }
    view->Release();
    return paths;
}

std::vector<std::wstring> GetSelectedPaths(HWND hWnd) {
    IShellBrowser* browser = BrowserForFileView(hWnd);
    std::vector<std::wstring> paths = PsFromBrowser(browser);
    if (browser) {
        browser->Release();
    }
    return paths;
}

bool IsDirectory(const std::wstring& path) {
    const DWORD attributes = ReadFileAttributes(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

std::wstring FileNameOf(const std::wstring& path) {
    const size_t slash = path.find_last_of(L"\\/");
    return slash == std::wstring::npos ? path : path.substr(slash + 1);
}

// ---------------------------------------------------------------------------
// Reading a file as text
// ---------------------------------------------------------------------------

enum class ReadResult {
    Ok,
    TooBig,
    Binary,
    Unreadable,
};

// Reject NUL and uncommon control bytes, but allow form feeds and ANSI escapes.
bool IsTextControl(unsigned char c) {
    return c == 0 || (c < 0x20 && c != '\t' && c != '\r' && c != '\n' && c != '\f' && c != 0x1B) ||
           c == 0x7F;
}

bool LooksLikeUtf8(const unsigned char* data, size_t size) {
    size_t i = 0;
    while (i < size) {
        const unsigned char lead = data[i];
        size_t extra = 0;
        unsigned int code = 0;
        if (lead < 0x80) {
            if (IsTextControl(lead)) {
                return false;
            }
            i++;
            continue;
        } else if ((lead & 0xE0) == 0xC0) {
            extra = 1;
            code = lead & 0x1F;
        } else if ((lead & 0xF0) == 0xE0) {
            extra = 2;
            code = lead & 0x0F;
        } else if ((lead & 0xF8) == 0xF0) {
            extra = 3;
            code = lead & 0x07;
        } else {
            return false;
        }
        if (i + extra >= size) {
            return false;
        }
        for (size_t k = 1; k <= extra; k++) {
            const unsigned char next = data[i + k];
            if ((next & 0xC0) != 0x80) {
                return false;
            }
            code = (code << 6) | (next & 0x3F);
        }
        if ((extra == 1 && code < 0x80) || (extra == 2 && code < 0x800) ||
            (extra == 3 && code < 0x10000)) {
            return false;
        }
        if (code >= 0xD800 && code <= 0xDFFF) {
            return false;
        }
        if (code > 0x10FFFF) {
            return false;
        }
        i += extra + 1;
    }
    return true;
}

bool LooksLikeUtf16NoBom(const unsigned char* data, size_t size, bool* bigEndian) {
    if (size < 4) {
        return false;
    }
    size_t zeroAtOdd = 0;
    size_t zeroAtEven = 0;
    const size_t samples = size < 512 ? size : 512;
    for (size_t i = 0; i + 1 < samples; i += 2) {
        if (data[i + 1] == 0) {
            zeroAtOdd++;
        }
        if (data[i] == 0) {
            zeroAtEven++;
        }
    }
    const size_t pairs = samples / 2;
    if (pairs == 0) {
        return false;
    }
    if (zeroAtOdd * 10 > pairs * 8) {
        *bigEndian = false;
        return true;
    }
    if (zeroAtEven * 10 > pairs * 8) {
        *bigEndian = true;
        return true;
    }
    return false;
}

bool LooksLikeAnsiText(const unsigned char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (IsTextControl(data[i])) {
            return false;
        }
    }
    return true;
}

// In UTF-16, zero bytes are normal; a zero code unit truncates CF_UNICODETEXT.
bool HasUtf16Null(const unsigned char* data, size_t size) {
    for (size_t i = 0; i + 1 < size; i += 2) {
        if (data[i] == 0 && data[i + 1] == 0) {
            return true;
        }
    }
    return false;
}

// A mark is the file saying what it is, and it is never overruled.
ReadResult ReadFileAsText(const std::wstring& path, std::wstring* out) {
    const HANDLE file = OpenFileForRead(path);
    if (file == INVALID_HANDLE_VALUE) {
        return ReadResult::Unreadable;
    }
    struct FileGuard {
        HANDLE file;
        void Close() {
            if (file != INVALID_HANDLE_VALUE) {
                CloseHandle(file);
                file = INVALID_HANDLE_VALUE;
            }
        }
        ~FileGuard() { Close(); }
    } guard{file};

    LARGE_INTEGER size = {};
    if (!QueryFileSize(file, &size) || size.QuadPart < 0) {
        guard.Close();
        return ReadResult::Unreadable;
    }
    if (size.QuadPart > SizeLimitBytes()) {
        guard.Close();
        return ReadResult::TooBig;
    }

    std::vector<unsigned char> bytes(static_cast<size_t>(size.QuadPart) + 1);
    size_t read = 0;
    while (read < static_cast<size_t>(size.QuadPart)) {
        if (g_unloading) {
            guard.Close();
            return ReadResult::Unreadable;
        }
        DWORD chunk = 0;
        const DWORD want = static_cast<DWORD>(
            (static_cast<size_t>(size.QuadPart) - read) > 0x10000
                ? 0x10000
                : (static_cast<size_t>(size.QuadPart) - read));
        if (!ReadFileChunk(file, bytes.data() + read, want, &chunk) ||
            chunk == 0) {
            break;
        }
        read += chunk;
    }
    guard.Close();
    if (read != static_cast<size_t>(size.QuadPart)) {
        return ReadResult::Unreadable;
    }
    bytes.resize(read);

    const unsigned char* data = bytes.data();
    const size_t length = bytes.size();

    if (length >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) {
        // A UTF-8 mark does not make embedded control bytes safe to paste.
        if (!LooksLikeAnsiText(data + 3, length - 3)) {
            return ReadResult::Binary;
        }
        out->clear();
        if (length > 3) {
            const int wide = MultiByteToWideChar(
                CP_UTF8, 0, reinterpret_cast<const char*>(data + 3),
                static_cast<int>(length - 3), nullptr, 0);
            if (wide > 0) {
                out->resize(static_cast<size_t>(wide));
                MultiByteToWideChar(CP_UTF8, 0,
                                    reinterpret_cast<const char*>(data + 3),
                                    static_cast<int>(length - 3), &(*out)[0],
                                    wide);
            }
        }
        return ReadResult::Ok;
    }
    if (length >= 2 && data[0] == 0xFF && data[1] == 0xFE) {
        if ((length - 2) % 2 != 0 || HasUtf16Null(data + 2, length - 2)) {
            return ReadResult::Binary;
        }
        *out = std::wstring(reinterpret_cast<const wchar_t*>(data + 2),
                            (length - 2) / sizeof(wchar_t));
        return ReadResult::Ok;
    }
    if (length >= 2 && data[0] == 0xFE && data[1] == 0xFF) {
        if ((length - 2) % 2 != 0 || HasUtf16Null(data + 2, length - 2)) {
            return ReadResult::Binary;
        }
        out->clear();
        out->reserve((length - 2) / 2);
        for (size_t i = 2; i + 1 < length; i += 2) {
            out->push_back(static_cast<wchar_t>((data[i] << 8) | data[i + 1]));
        }
        return ReadResult::Ok;
    }

    if (length == 0) {
        out->clear();
        return ReadResult::Ok;
    }

    // UTF-16 without a mark is checked before UTF-8 on purpose: the zero bytes
    // of such a file are valid UTF-8, so the other order would keep them and
    // the text would arrive full of invisible gaps.
    bool bigEndian = false;
    if (LooksLikeUtf16NoBom(data, length, &bigEndian)) {
        if (length % 2 != 0 || HasUtf16Null(data, length)) {
            return ReadResult::Binary;
        }
        out->clear();
        out->reserve(length / 2);
        for (size_t i = 0; i + 1 < length; i += 2) {
            const unsigned char first = bigEndian ? data[i + 1] : data[i];
            const unsigned char second = bigEndian ? data[i] : data[i + 1];
            out->push_back(static_cast<wchar_t>((second << 8) | first));
        }
        return ReadResult::Ok;
    }

    const unsigned int codePage = LooksLikeUtf8(data, length) ? CP_UTF8 : CP_ACP;
    if (codePage == CP_ACP && !LooksLikeAnsiText(data, length)) {
        return ReadResult::Binary;
    }
    const int wide = MultiByteToWideChar(
        codePage, 0, reinterpret_cast<const char*>(data),
        static_cast<int>(length), nullptr, 0);
    if (wide <= 0) {
        return ReadResult::Binary;
    }
    out->resize(static_cast<size_t>(wide));
    MultiByteToWideChar(codePage, 0, reinterpret_cast<const char*>(data),
                        static_cast<int>(length), &(*out)[0], wide);
    return ReadResult::Ok;
}

// ---------------------------------------------------------------------------
// Can this be copied at all?
//
// The menu should not offer an entry that is certain to refuse, so the files are
// checked when the menu opens. Only the first few KiB are read, and a file that
// is offline, stored online only, or on a network share, by UNC path or mapped
// drive, is not read at all:
// reading one would start a download or wait for the network, and the menu would
// wait with it. Such a file is assumed copyable and the copy decides.
// ---------------------------------------------------------------------------

constexpr int kSniffFiles = 16;
constexpr size_t kSniffBytes = 4096;

bool IsRemoteOrSparse(const std::wstring& path) {
    const DWORD attributes = ReadFileAttributes(path);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    if (attributes & (FILE_ATTRIBUTE_OFFLINE |
                      FILE_ATTRIBUTE_RECALL_ON_OPEN |
                      FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS)) {
        return true;
    }
    // A UNC path, which reading would fetch over the network.
    if (path.size() >= 2 && path[0] == L'\\' && path[1] == L'\\') {
        return true;
    }
    // A drive letter that is mapped to a share is just as much on the network
    // as a UNC path, and reading from a share that has gone away blocks.
    if (path.size() >= 3 && path[1] == L':' && (path[2] == L'\\' || path[2] == L'/')) {
        const wchar_t root[4] = {path[0], L':', L'\\', 0};
        return QueryDriveType(root) == DRIVE_REMOTE;
    }
    return false;
}

// The same order the real read uses: a mark first, then UTF-16 without a mark,
// then strict UTF-8, then the system code page.
bool SniffIsText(const unsigned char* data, size_t size) {
    if (size == 0) {
        return true;
    }
    if (size >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) {
        // Only check controls in the prefix: it may end mid UTF-8 character.
        return LooksLikeAnsiText(data + 3, size - 3);
    }
    if (size >= 2 && ((data[0] == 0xFF && data[1] == 0xFE) ||
                      (data[0] == 0xFE && data[1] == 0xFF))) {
        return !HasUtf16Null(data + 2, size - 2);
    }
    bool bigEndian = false;
    if (LooksLikeUtf16NoBom(data, size, &bigEndian)) {
        return !HasUtf16Null(data, size);
    }
    if (LooksLikeUtf8(data, size)) {
        return true;
    }
    return LooksLikeAnsiText(data, size);
}

bool LooksCopyable(const std::wstring& path) {
    LARGE_INTEGER size = {};
    const HANDLE probe = OpenFileForRead(path);
    if (probe == INVALID_HANDLE_VALUE) {
        return false;
    }
    if (!QueryFileSize(probe, &size) || size.QuadPart < 0 ||
        size.QuadPart > SizeLimitBytes()) {
        CloseHandle(probe);
        return false;
    }
    unsigned char buffer[kSniffBytes];
    DWORD read = 0;
    const bool ok = ReadFileChunk(probe, buffer, sizeof(buffer), &read) != 0;
    CloseHandle(probe);
    return ok && SniffIsText(buffer, read);
}

// Check a bounded local prefix before offering Notepad for a size refusal.
// The remaining file may still be binary; this is a heuristic, not a guarantee.
bool LooksLikeTextPrefix(const std::wstring& path) {
    const HANDLE file = OpenFileForRead(path);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    unsigned char buffer[kSniffBytes];
    DWORD read = 0;
    const bool ok = ReadFileChunk(file, buffer, sizeof(buffer), &read) != 0;
    CloseHandle(file);
    return ok && SniffIsText(buffer, read);
}

bool AnyCopyableText(const std::vector<std::wstring>& paths) {
    int looked = 0;
    for (const std::wstring& path : paths) {
        if (IsDirectory(path)) {
            continue;
        }
        if (looked >= kSniffFiles) {
            // Too many to check. Show the entry rather than hide a file the
            // user can see on screen.
            return true;
        }
        looked++;
        if (IsRemoteOrSparse(path)) {
            return true;
        }
        if (LooksCopyable(path)) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Clipboard
// ---------------------------------------------------------------------------

// A clipboard another program is holding must not cost a file its item, so the
// open is retried for a short while before giving up. The owner is the Explorer
// window, because the documented behaviour of SetClipboardData after
// OpenClipboard(NULL) is that it fails.
bool SetClipboardText(HWND owner, const std::wstring& text) {
    if (g_unloading) {
        return false;
    }

    // Prepare the new item before emptying the clipboard, so allocation and
    // lock failures cannot discard its previous contents.
    const size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL block = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!block) {
        return false;
    }
    void* target = GlobalLock(block);
    if (!target) {
        GlobalFree(block);
        return false;
    }
    memcpy(target, text.c_str(), bytes);
    GlobalUnlock(block);

    bool opened = false;
    for (int attempt = 0; attempt < 20; attempt++) {
        if (g_unloading) {
            break;
        }
        if (OpenClipboard(owner)) {
            opened = true;
            break;
        }
        Sleep(25);
    }
    if (!opened) {
        GlobalFree(block);
        return false;
    }
    if (g_unloading || !EmptyClipboard()) {
        CloseClipboard();
        GlobalFree(block);
        return false;
    }
    // After EmptyClipboard succeeds, a SetClipboardData failure can still leave
    // the clipboard empty. Ownership transfers only when SetClipboardData works.
    if (!SetClipboardData(CF_UNICODETEXT, block)) {
        GlobalFree(block);
        CloseClipboard();
        return false;
    }
    CloseClipboard();
    return true;
}

// ---------------------------------------------------------------------------
// SHA-256
// ---------------------------------------------------------------------------

std::wstring Sha256OfFile(const std::wstring& path, bool* ok) {
    *ok = false;
    // The hash object can reference this buffer until it is destroyed, so the
    // cleanup guard below must run before the vectors are destroyed.
    std::vector<unsigned char> object;
    std::vector<unsigned char> digest;
    const HANDLE file = OpenFileForRead(path);
    if (file == INVALID_HANDLE_VALUE) {
        return std::wstring();
    }

    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    struct Cleanup {
        HANDLE file;
        BCRYPT_ALG_HANDLE& algorithm;
        BCRYPT_HASH_HANDLE& hash;
        ~Cleanup() {
            if (hash) BCryptDestroyHash(hash);
            if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
            CloseHandle(file);
        }
    } cleanup{file, algorithm, hash};
    digest.resize(32);
    ULONG objectSize = 0;
    ULONG produced = 0;
    bool success = false;

    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr,
                                    0) == 0 &&
        BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH,
                          reinterpret_cast<PUCHAR>(&objectSize),
                          sizeof(objectSize), &produced, 0) == 0) {
        object.resize(objectSize);
        if (BCryptCreateHash(algorithm, &hash, object.data(), objectSize, nullptr,
                             0, 0) == 0) {
            success = true;
            std::vector<unsigned char> buffer(0x10000);
            for (;;) {
                // The mod may be disabled while this runs. Stopping between
                // chunks makes that wait short instead of a whole file.
                if (g_unloading) {
                    success = false;
                    break;
                }
                DWORD read = 0;
                if (!ReadFileChunk(file, buffer.data(),
                                   static_cast<DWORD>(buffer.size()), &read)) {
                    success = false;
                    break;
                }
                if (read == 0) {
                    break;
                }
                if (BCryptHashData(hash, buffer.data(), read, 0) != 0) {
                    success = false;
                    break;
                }
            }
            if (success &&
                BCryptFinishHash(hash, digest.data(),
                                 static_cast<ULONG>(digest.size()), 0) != 0) {
                success = false;
            }
        }
    }
    if (!success) {
        return std::wstring();
    }

    static const wchar_t* digits = L"0123456789abcdef";
    std::wstring hex;
    hex.reserve(digest.size() * 2);
    for (unsigned char byte : digest) {
        hex.push_back(digits[byte >> 4]);
        hex.push_back(digits[byte & 0x0F]);
    }
    *ok = true;
    return hex;
}

// The size without reading anything, so an oversized file costs nothing.
long long FileSizeBytes(const std::wstring& path) {
    const HANDLE file = OpenFileForRead(path);
    if (file == INVALID_HANDLE_VALUE) {
        return -1;
    }
    LARGE_INTEGER size = {};
    const bool ok = QueryFileSize(file, &size) != 0;
    CloseHandle(file);
    return ok ? size.QuadPart : -1;
}

// ---------------------------------------------------------------------------
// The two commands
// ---------------------------------------------------------------------------

// Nothing was copied, so say why. Notepad is only offered for a file that is
// small enough and looks like text in a local prefix; otherwise show the reason.
void ReportRefusal(HWND owner, const std::wstring& path, const wchar_t* reason,
                   bool mayOfferEditor) {
    if (!g_settings.offerEditor || g_unloading || path.empty()) {
        return;
    }
    const long long size = mayOfferEditor ? FileSizeBytes(path) : -1;
    const bool canOpen = size > 0 && size <= static_cast<long long>(kTotalTextBytes) &&
                         !IsRemoteOrSparse(path) && LooksLikeTextPrefix(path);
    std::wstring text = reason;
    if (canOpen) {
        text += L"\n\nOpen it instead?";
    }
    const UINT flags = canOpen ? (MB_YESNO | MB_ICONINFORMATION)
                               : (MB_OK | MB_ICONINFORMATION);
    if (g_unloading) {
        return;
    }
    const int answer = MessageBoxW(owner, text.c_str(), kContentsTitle, flags);
    if (g_unloading || answer != IDYES || !canOpen) {
        return;
    }
    // Quoted, because Notepad takes its file name as a command line, and a path
    // with a space in it would otherwise open the wrong file or none.
    const std::wstring quoted = L"\"" + path + L"\"";
    ShellExecuteW(owner, L"open", L"notepad.exe", quoted.c_str(), nullptr,
                  SW_SHOWNORMAL);
}

void DoCopyContents(HWND owner, const std::vector<std::wstring>& paths) {
    const bool separate = g_settings.separateEntries;
    const int gapMs = g_settings.separateGapMs;
    struct Part {
        std::wstring name;
        std::wstring text;
    };
    std::vector<Part> parts;
    std::wstring lastRefused;
    const wchar_t* lastReason = nullptr;
    // Whether a size refusal is eligible for the Notepad prefix check.
    bool lastMayOpen = false;
    size_t budget = kTotalTextBytes;
    size_t files = 0;

    try {
        for (const std::wstring& path : paths) {
            if (g_unloading) {
                return;
            }
            if (IsDirectory(path)) {
                continue;
            }
            files++;
            if (budget < 2 * sizeof(wchar_t)) {
                // The budget is gone. Reading more files would only throw the
                // text away again, so the loop stops instead.
                lastReason = L"That selection contains more text than the mod "
                             L"will copy at once.";
                break;
            }
            const long long onDisk = FileSizeBytes(path);
            if (onDisk < 0) {
                lastRefused = path;
                lastReason = L"This file could not be read.";
                lastMayOpen = false;
                continue;
            }
            if (onDisk > SizeLimitBytes()) {
                // Checked before reading, so an oversized file costs nothing.
                lastRefused = path;
                lastReason = L"This file is larger than the size limit.";
                lastMayOpen = true;
                continue;
            }
            // Skip files larger than the remaining budget without opening them.
            // This conservative check can also skip text that would decode to
            // fewer bytes than its on-disk encoding.
            if (static_cast<size_t>(onDisk) > budget) {
                lastRefused = path;
                lastReason = L"That selection contains more text than the mod "
                             L"will copy at once.";
                lastMayOpen = true;
                continue;
            }
            std::wstring text;
            const ReadResult result = ReadFileAsText(path, &text);
            if (result != ReadResult::Ok) {
                lastRefused = path;
                lastReason = result == ReadResult::TooBig
                                 ? L"This file is larger than the size limit."
                                 : result == ReadResult::Binary
                                       ? L"This file does not look like text."
                                       : L"This file could not be read.";
                lastMayOpen = result == ReadResult::TooBig;
                continue;
            }
            const size_t bytes = text.size() * sizeof(wchar_t);
            if (bytes > budget) {
                lastRefused = path;
                lastReason = L"That selection contains more text than the mod "
                             L"will copy at once.";
                lastMayOpen = true;
                continue;
            }
            budget -= bytes;
            Part part;
            part.name = FileNameOf(path);
            part.text.swap(text);
            parts.push_back(std::move(part));
        }

        if (parts.empty()) {
            if (!lastRefused.empty()) {
                ReportRefusal(owner, lastRefused, lastReason, lastMayOpen);
            }
            return;
        }

        // One clipboard item per file, so Windows clipboard history keeps them
        // apart. Files too big to be a history entry go last, so the history
        // keeps the small ones and Ctrl+V gives the big one.
        if (separate && parts.size() > 1) {
            std::vector<size_t> order;
            order.reserve(parts.size());
            for (size_t i = 0; i < parts.size(); i++) {
                if (!TooBigForHistory(parts[i].text)) {
                    order.push_back(i);
                }
            }
            for (size_t i = 0; i < parts.size(); i++) {
                if (TooBigForHistory(parts[i].text)) {
                    order.push_back(i);
                }
            }
            int written = 0;
            for (size_t n = 0; n < order.size(); n++) {
                if (g_unloading) {
                    return;
                }
                if (SetClipboardText(owner, parts[order[n]].text)) {
                    written++;
                }
                // Clipboard history is a separate service that reads the
                // clipboard when it is told it changed. Write the next file
                // immediately and it only ever sees the last one. The wait ends
                // early if the mod is being unloaded.
                if (n + 1 < order.size() && gapMs > 0) {
                    if (g_unloadEvent) {
                        WaitForSingleObject(g_unloadEvent,
                                            static_cast<DWORD>(gapMs));
                    } else {
                        Sleep(static_cast<DWORD>(gapMs));
                    }
                }
            }
            if (written != static_cast<int>(parts.size())) {
                Wh_Log(L"only %d of %d items reached the clipboard", written,
                       static_cast<int>(parts.size()));
                if (written == 0 && !g_unloading) {
                    MessageBoxW(owner,
                                L"The contents could not be copied. Check the "
                                L"clipboard before pasting.",
                                kContentsTitle, MB_OK | MB_ICONINFORMATION);
                }
            }
            return;
        }

        std::wstring combined;
        for (const Part& part : parts) {
            if (g_unloading) {
                return;
            }
            if (!combined.empty()) {
                combined += L"\r\n";
            }
            // Joined multi-file selections keep names even if only one file
            // could be read. Folders do not count; separate items stay bare.
            if (files > 1 && !separate) {
                combined += L"=== ";
                combined += part.name;
                combined += L" ======\r\n";
            }
            combined += part.text;
        }
        if (!SetClipboardText(owner, combined)) {
            Wh_Log(L"the clipboard write failed, nothing was copied");
            if (!g_unloading) {
                MessageBoxW(owner,
                            L"The contents could not be copied. Check the "
                            L"clipboard before pasting.",
                            kContentsTitle, MB_OK | MB_ICONINFORMATION);
            }
        }
    } catch (...) {
        // An allocation failure inside a hook would otherwise end Explorer.
        Wh_Log(L"the copy operation could not finish");
    }
}

// One prompt for the whole operation. The sizes of every selected file are
// added up, because three ISO images hash for as long as one file of their
// combined size.
bool ConfirmLargeHash(HWND owner, const std::vector<std::wstring>& paths) {
    long long total = 0;
    int files = 0;
    for (const std::wstring& path : paths) {
        if (g_unloading) {
            return false;
        }
        if (IsDirectory(path)) {
            continue;
        }
        const long long size = FileSizeBytes(path);
        if (g_unloading) {
            return false;
        }
        if (size >= 0) {
            if (size > LLONG_MAX - total) {
                total = LLONG_MAX;
            } else {
                total += size;
            }
            files++;
        }
    }
    const long long limit = static_cast<long long>(g_settings.askHashMB) *
                            1024LL * 1024LL;
    if (files == 0 || (g_settings.askHashMB > 0 && total <= limit)) {
        return true;
    }
    // Megabytes when the total is under a gigabyte, because a lowered
    // askHashMB would otherwise produce "about 0.0 GB", which says nothing.
    wchar_t sizeText[64];
    if (total >= 1024LL * 1024LL * 1024LL) {
        swprintf_s(sizeText, L"%.1f GB",
                   static_cast<double>(total) / (1024.0 * 1024.0 * 1024.0));
    } else {
        swprintf_s(sizeText, L"%lld MB",
                   (total + 1024LL * 1024LL - 1) / (1024LL * 1024LL));
    }
    wchar_t text[512];
    swprintf_s(text,
               L"%d file%s, about %s in all. Hashing reads every byte, and this "
               L"window will not respond until it is done.\n\nHash anyway?",
               files, files == 1 ? L"" : L"s", sizeText);
    if (g_unloading) {
        return false;
    }
    const int answer = MessageBoxW(owner, text, kHashTitle,
                                   MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
    return !g_unloading && answer == IDYES;
}

void DoCopyHash(HWND owner, const std::vector<std::wstring>& paths) {
    try {
        if (!ConfirmLargeHash(owner, paths)) {
            return;
        }
        struct Hashed {
            std::wstring hex;
            std::wstring name;
        };
        std::vector<Hashed> hashed;
        size_t files = 0;
        for (const std::wstring& path : paths) {
            if (g_unloading) {
                return;
            }
            if (IsDirectory(path)) {
                continue;
            }
            files++;
            bool ok = false;
            const std::wstring hex = Sha256OfFile(path, &ok);
            if (!ok) {
                continue;
            }
            Hashed line;
            line.hex = hex;
            line.name = FileNameOf(path);
            hashed.push_back(std::move(line));
        }
        if (g_unloading) {
            return;
        }
        if (hashed.empty()) {
            if (files > 0) {
                MessageBoxW(owner,
                            L"The selected file or files could not be hashed. "
                            L"No hash was copied; the clipboard may contain "
                            L"earlier data.",
                            kHashTitle, MB_OK | MB_ICONINFORMATION);
            }
            return;
        }
        // Count selected files, not just successful hashes, so a partial result
        // still identifies its file. Folders do not count.
        std::wstring combined;
        for (const Hashed& line : hashed) {
            if (!combined.empty()) {
                combined += L"\r\n";
            }
            combined += line.hex;
            if (files > 1) {
                combined += L"  ";
                combined += line.name;
            }
        }
        if (!SetClipboardText(owner, combined)) {
            Wh_Log(L"the clipboard write failed, nothing was copied");
            if (!g_unloading) {
                MessageBoxW(owner,
                            L"The hash could not be copied. Check the "
                            L"clipboard and try again.",
                            kHashTitle, MB_OK | MB_ICONINFORMATION);
            }
        }
    } catch (...) {
        Wh_Log(L"ran out of memory while hashing, nothing was copied");
        if (!g_unloading) {
            MessageBoxW(owner,
                        L"The hash operation could not finish. No new hash "
                        L"was copied; check the clipboard before using it.",
                        kHashTitle, MB_OK | MB_ICONINFORMATION);
        }
    }
}

// ---------------------------------------------------------------------------
// The shortcut
//
// A key press reaches this mod through TranslateAcceleratorW, which File
// Explorer calls on the window's own thread for every key message it handles.
// Nothing is registered system wide, so no other program can lose a key.
// ---------------------------------------------------------------------------

bool ModifiersHeld(UINT wanted) {
    UINT held = 0;
    if (GetKeyState(VK_CONTROL) & 0x8000) held |= MOD_CONTROL;
    if (GetKeyState(VK_SHIFT) & 0x8000) held |= MOD_SHIFT;
    if (GetKeyState(VK_MENU) & 0x8000) held |= MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) held |= MOD_WIN;
    return held == wanted;
}

// True while the user is typing in this window: the rename box, the address
// bar, the search box, or any other text field.
bool IsTextFieldFocused(HWND root, HWND focus) {
    if (!focus) {
        return false;
    }
    for (HWND h = focus; h && h != root; h = GetAncestor(h, GA_PARENT)) {
        wchar_t name[64];
        if (!GetClassNameW(h, name, ARRAYSIZE(name))) {
            break;
        }
        if (wcsstr(name, L"Edit") || wcsstr(name, L"Address Band")) {
            return true;
        }
    }
    GUITHREADINFO info;
    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    if (GetGUIThreadInfo(GetCurrentThreadId(), &info) && info.hwndCaret) {
        return true;
    }
    return false;
}

// 1 for the contents key, 2 for the hash key, 0 for anything else.
int MatchHotkey(UINT key) {
    if (!g_settings.hotkeys) {
        return 0;
    }
    if (g_settings.copyContents && key == g_settings.contentsKey &&
        ModifiersHeld(g_settings.contentsModifiers)) {
        return 1;
    }
    if (g_settings.copyHash && key == g_settings.hashKey &&
        ModifiersHeld(g_settings.hashModifiers)) {
        return 2;
    }
    return 0;
}

using TranslateAcceleratorW_t = decltype(&TranslateAcceleratorW);
TranslateAcceleratorW_t TranslateAcceleratorW_Original;

int WINAPI TranslateAcceleratorW_Hook(HWND hWnd, HACCEL hAccTable,
                                      LPMSG msg) {
    HookGuard hookGuard;
    if (!g_unloading && !t_copying && msg &&
        (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN) &&
        // Bit 30 marks an auto repeat. One press is one copy.
        !(msg->lParam & (1u << 30))) {
        const int which = MatchHotkey(static_cast<UINT>(msg->wParam));
        if (which) {
            HWND root = GetAncestor(msg->hwnd, GA_ROOT);
            // The file view has to be what the user is pointing at. A key
            // pressed while the navigation pane has the focus would otherwise
            // copy the file list's selection, which is not what is on screen.
            if (IsExplorerFrame(root) && InFileView(msg->hwnd) &&
                !IsTextFieldFocused(root, msg->hwnd)) {
                // Raised before the selection is read, and checked again with
                // it raised, for the same reason as in the menu hook.
                BusyGuard busy(root);
                if (g_unloading) {
                    return TranslateAcceleratorW_Original(hWnd, hAccTable, msg);
                }
                const std::vector<std::wstring> paths =
                    GetSelectedPaths(msg->hwnd);
                bool anyFile = false;
                for (const std::wstring& path : paths) {
                    if (!IsDirectory(path)) {
                        anyFile = true;
                        break;
                    }
                }
                if (anyFile) {
                    t_copying = true;
                    if (which == 1) {
                        DoCopyContents(root, paths);
                    } else {
                        DoCopyHash(root, paths);
                    }
                    t_copying = false;
                    // Swallowed, so File Explorer does not also act on the key.
                    return 1;
                }
            }
        }
    }
    return TranslateAcceleratorW_Original(hWnd, hAccTable, msg);
}

// ---------------------------------------------------------------------------
// The menu entries
//
// File Explorer uses both popup functions, and which one it uses is a property
// of the build. On the Windows 11 build this was measured on, the classic menu
// came through the plain TrackPopupMenu, with TPM_RETURNCMD set, owned by the
// shell view itself. Both are hooked and both share this code.
//
// With TPM_RETURNCMD the value the popup returns is the command id, and File
// Explorer acts on it. So every command that is not ours is returned exactly as
// it came, and a pick of ours returns 0, which is what a dismissed menu returns.
// ---------------------------------------------------------------------------

// Above the shell's own command range, which ends at 0x7FFF, and below the
// system range, which starts at 0xF000. Each menu is checked for a clash first,
// because a submenu filled later can still take an id from anywhere.
constexpr UINT kMenuIdBase = 0xE7C1;
constexpr int kMenuIdTries = 8;

// Three ids are taken: the two entries and the separator. All three are
// checked, and candidates are three apart, so the separator of one candidate is
// never the first entry of the next.
bool IdsAreFree(HMENU menu, UINT base) {
    return GetMenuState(menu, base, MF_BYCOMMAND) == (UINT)-1 &&
           GetMenuState(menu, base + 1, MF_BYCOMMAND) == (UINT)-1 &&
           GetMenuState(menu, base + 2, MF_BYCOMMAND) == (UINT)-1;
}

UINT PickMenuIdBase(HMENU menu) {
    for (int i = 0; i < kMenuIdTries; i++) {
        const UINT base = kMenuIdBase + static_cast<UINT>(i) * 3;
        if (IdsAreFree(menu, base)) {
            return base;
        }
    }
    return 0;
}

void RemoveEntries(HMENU menu, UINT base) {
    if (!IsMenu(menu)) {
        return;
    }
    DeleteMenu(menu, base, MF_BYCOMMAND);
    DeleteMenu(menu, base + 1, MF_BYCOMMAND);
    DeleteMenu(menu, base + 2, MF_BYCOMMAND);
}

bool HasCheckedItem(HMENU menu, int count) {
    for (int i = 0; i < count; i++) {
        const UINT state = GetMenuState(menu, static_cast<UINT>(i), MF_BYPOSITION);
        if (state != (UINT)-1 && (state & MF_CHECKED)) {
            return true;
        }
    }
    return false;
}

bool IsOnColumnHeader(int x, int y) {
    POINT pt = {x, y};
    return HasClass(WindowFromPoint(pt), L"SysHeader32");
}

// Adds the entries when this popup is a File Explorer file menu, and reports
// the id base it used, or 0.
UINT AddEntries(HMENU menu, UINT uFlags, HWND hWnd, int x, int y,
                std::vector<std::wstring>* paths) {
    if (!(uFlags & TPM_RETURNCMD) || !menu || !hWnd) {
        return 0;
    }
    // The column header sits inside the file view, so its own menu looks like a
    // file menu and the entries were added to it. That menu has no default
    // action, and it either has check marks on the columns being sorted by, or
    // was opened right over the header. A file menu also loses its default
    // action when more than fifteen files are selected, so the check marks and
    // the pointer are what settles it there.
    const int count = GetMenuItemCount(menu);
    if (GetMenuDefaultItem(menu, FALSE, GMDI_USEDISABLED) == (UINT)-1 &&
        (HasCheckedItem(menu, count) || IsOnColumnHeader(x, y))) {
        return 0;
    }
    IShellBrowser* browser = BrowserForFileView(hWnd);
    if (!browser) {
        return 0;
    }
    *paths = PsFromBrowser(browser);
    browser->Release();

    bool anyFile = false;
    for (const std::wstring& path : *paths) {
        if (!IsDirectory(path)) {
            anyFile = true;
            break;
        }
    }
    if (!anyFile) {
        return 0;
    }

    bool addContents = g_settings.copyContents;
    const bool addHash = g_settings.copyHash;
    if (addContents && g_settings.hideWhenUncopyable &&
        !AnyCopyableText(*paths)) {
        addContents = false;
    }
    if (!addContents && !addHash) {
        return 0;
    }

    const UINT base = PickMenuIdBase(menu);
    if (!base) {
        return 0;
    }

    // The separator is given an id of its own, because MF_SEPARATOR with id 0
    // cannot be removed by command id, and a menu that keeps the line after its
    // entries are gone is a menu left modified.
    MENUITEMINFO info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_FTYPE | MIIM_ID;
    info.fType = MFT_SEPARATOR;
    info.wID = base + 2;
    InsertMenuItemW(menu, GetMenuItemCount(menu), TRUE, &info);
    if (addContents) {
        AppendMenuW(menu, MF_STRING, base, L"Copy contents");
    }
    if (addHash) {
        AppendMenuW(menu, MF_STRING, base + 1, L"Copy SHA-256");
    }
    return base;
}

template <typename Track>
BOOL TrackWithEntries(HMENU menu, UINT uFlags, HWND owner, int x, int y,
                      Track track) {
    // TrackPopupMenu may call TrackPopupMenuEx. Only the outer call adds.
    if (t_inMenuHook) {
        return track();
    }

    // Count the selection read, popup and any copy that follows, and check the
    // unload flag again after entering. HookGuard also tracks the outer hook.
    // An open popup can keep the hook active even if it has none of our
    // entries, so ask its owner to close it when the mod is turned off.
    BusyGuard busy(owner);
    if (g_unloading) {
        return track();
    }

    std::vector<std::wstring> paths;
    const UINT base = AddEntries(menu, uFlags, owner, x, y, &paths);
    if (!base) {
        return track();
    }
    if (g_unloading) {
        RemoveEntries(menu, base);
        return track();
    }

    t_inMenuHook = true;
    const BOOL result = track();
    t_inMenuHook = false;

    // Put the menu back the way the shell built it.
    RemoveEntries(menu, base);

    const UINT picked = static_cast<UINT>(result);
    if (picked != base && picked != base + 1) {
        // File Explorer's own command, untouched.
        return result;
    }
    if (!t_copying) {
        t_copying = true;
        if (picked == base) {
            DoCopyContents(owner, paths);
        } else {
            DoCopyHash(owner, paths);
        }
        t_copying = false;
    }
    // Handled here, so File Explorer gets no command, as if it was dismissed.
    return 0;
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                  HWND hWnd, LPTPMPARAMS params) {
    HookGuard hookGuard;
    return TrackWithEntries(hMenu, uFlags, hWnd, x, y, [&] {
        return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, params);
    });
}

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                int reserved, HWND hWnd, const RECT* rect) {
    HookGuard hookGuard;
    return TrackWithEntries(hMenu, uFlags, hWnd, x, y, [&] {
        return TrackPopupMenu_Original(hMenu, uFlags, x, y, reserved, hWnd, rect);
    });
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Copy File Contents %s", WH_MOD_VERSION);
    // Signalled on unload, to cut the wait between separate clipboard items
    // short. A missing event only costs that: the code falls back to sleeping.
    g_unloadEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    LoadSettings();
    Wh_Log(L"settings: max %d %s (%lld bytes), contents=%d hash=%d",
           g_settings.maxSize.load(), g_settings.maxSizeInMb ? L"MB" : L"KB",
           SizeLimitBytes(), g_settings.copyContents.load(), g_settings.copyHash.load());

    // The paths are independent. Either one working leaves the mod useful, so a
    // failure on one is logged and the others are still installed.
    int hooked = 0;
    if (WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook,
                                       &TrackPopupMenuEx_Original)) {
        hooked++;
    } else {
        Wh_Log(L"could not hook TrackPopupMenuEx, half the menu path is off");
    }
    if (WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenu_Hook,
                                       &TrackPopupMenu_Original)) {
        hooked++;
    } else {
        Wh_Log(L"could not hook TrackPopupMenu, half the menu path is off");
    }
    if (WindhawkUtils::SetFunctionHook(TranslateAcceleratorW,
                                       TranslateAcceleratorW_Hook,
                                       &TranslateAcceleratorW_Original)) {
        hooked++;
    } else {
        Wh_Log(L"could not hook TranslateAcceleratorW, the shortcuts are off");
    }

    if (!hooked) {
        Wh_Log(L"nothing was hooked, giving up");
        if (g_unloadEvent) {
            CloseHandle(g_unloadEvent);
            g_unloadEvent = nullptr;
        }
        return FALSE;
    }
    Wh_Log(L"ready, %d of 3 hooks installed", hooked);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"settings changed: max %d %s (%lld bytes)",
           g_settings.maxSize.load(), g_settings.maxSizeInMb ? L"MB" : L"KB",
           SizeLimitBytes());
}

// A prompt of this mod is a dialog with one of two captions, on the thread
// doing the copy. WM_CANCELMODE closes a menu but not a
// message box. Unloading answers No on yes/no prompts and OK on notices.
// Some custom dialogs have no standard button handles; request closure instead.
// Posting rather than sending avoids blocking the unload thread.
BOOL CALLBACK CancelPromptProc(HWND hWnd, LPARAM) {
    if (!HasClass(hWnd, L"#32770")) {
        return TRUE;
    }
    wchar_t title[128] = {};
    DWORD_PTR length = 0;
    if (!SendMessageTimeoutW(hWnd, WM_GETTEXT, ARRAYSIZE(title),
                             reinterpret_cast<LPARAM>(title),
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &length) ||
        !length) {
        return TRUE;
    }
    if (wcscmp(title, kContentsTitle) == 0 || wcscmp(title, kHashTitle) == 0) {
        if (GetDlgItem(hWnd, IDNO)) {
            PostMessageW(hWnd, WM_COMMAND, IDNO, 0);
        } else if (GetDlgItem(hWnd, IDOK)) {
            PostMessageW(hWnd, WM_COMMAND, IDOK, 0);
        } else {
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        }
    }
    return TRUE;
}

void CancelActiveWork() {
    CancelActiveIo();
    HWND owners[kMaxOwners];
    DWORD threads[kMaxOwners];
    // Take a paired snapshot; do not hold the lock while calling window APIs.
    AcquireSRWLockShared(&g_ownerLock);
    for (int slot = 0; slot < kMaxOwners; slot++) {
        owners[slot] = g_ownerSlots[slot];
        threads[slot] = g_ownerThreads[slot];
    }
    ReleaseSRWLockShared(&g_ownerLock);
    for (int slot = 0; slot < kMaxOwners; slot++) {
        if (!owners[slot]) {
            continue;
        }
        PostMessageW(owners[slot], WM_CANCELMODE, 0, 0);
        if (threads[slot]) {
            EnumThreadWindows(threads[slot], CancelPromptProc, 0);
        }
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    if (g_unloadEvent) {
        SetEvent(g_unloadEvent);
    }

    // Give the hooks time to cancel their menus and prompts before Windhawk
    // removes them. Wh_ModUninit finishes draining after hooks are removed.
    DWORD start = GetTickCount();
    while (g_busy > 0 || g_activeHooks > 0) {
        CancelActiveWork();
        if (GetTickCount() - start >= 2000) {
            break;
        }
        Sleep(20);
    }
}

void Wh_ModUninit() {
    // Windhawk has removed the hooks. Keep the module and its event alive until
    // tracked calls leave, even if an unresponsive prompt or read takes longer.
    while (g_busy > 0 || g_activeHooks > 0) {
        CancelActiveWork();
        Sleep(250);
    }
    if (g_unloadEvent) {
        CloseHandle(g_unloadEvent);
        g_unloadEvent = nullptr;
    }
}
