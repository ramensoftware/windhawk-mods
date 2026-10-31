// ==WindhawkMod==
// @id              copy-file-contents
// @name            Copy File Contents
// @description     Copies a selected file's text or its SHA-256 hash without opening it, from a keyboard shortcut or the right-click menu.
// @version         1.1
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

Adds two actions to File Explorer, so a file can be read without opening it:

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

**Right-click.** The two entries are also added to the classic menu, reached
with **Show more options** or Shift+F10. They are added at the bottom of that
menu, below Properties, and removed again once the menu closes, so the menu is
never left modified. The new Windows 11 menu belongs to a different component and
cannot be extended by injected code without registering a handler in the
registry, which a mod must not do. To get the entries on a plain right-click, use
the separate mod **Classic context menu on Windows 11**. If the classic menu
cannot be reached on your build, the keyboard shortcuts are the whole of the mod
and they do not need the menu at all.

## What it will and will not copy

A file is copied only if it looks like text. Files that look binary are refused
rather than pasted as garbage, and files larger than the size limit are refused
the same way. When either happens the mod offers to open the file instead, which
is the honest answer: this is not a text file, so read it in an editor.

The size limit is 8 MB by default, with the unit chosen next to it, megabytes or
kilobytes. A large selection is copied file by file, so one file over the limit is
skipped and the rest are still copied. That partial copy is silent, because a
dialog in the middle of copying five files is worse than a missing one. When
nothing at all can be copied, you are asked instead. The whole selection is also
held to a 32 MB budget, so selecting a whole folder of logs cannot make File
Explorer allocate without bound.

The ceilings are 64 MB per file and 32 MB per selection whatever the settings
say. A text file that big is already beyond what a clipboard is good for, and
the mod refuses rather than trying.

When nothing in the selection can be copied, **Copy contents** is left out of the
menu instead of being offered and then refusing. To decide that, the first 4 KiB
of each file are read when the menu opens, stopping at the first file that
passes, and looking at no more than 16 files. Files that are offline or stored
online only are not read at all, because reading one would start a download; such
a file is assumed copyable and the copy itself decides. Files on a network share
are treated the same way. A mixed selection still shows the entry and copies the
files that can be copied.

Encoding is detected rather than assumed: a byte order mark is honored, then the
file is checked for UTF-16 without a mark, then for strict UTF-8, and finally read
as the system code page. UTF-16 is checked before UTF-8 on purpose: the zero bytes
of a UTF-16 file are valid UTF-8, so the other order would decode such a file as
text with invisible gaps. Line endings are copied as they are.

## Large hashes

Hashing a file reads all of it, and the work runs on the same thread that owns the
File Explorer window, so that window does not repaint until the hash is done. For
a normal file that is instant. Above the size set in the settings, 1 GB by
default, the mod asks first, and says how big the file is. Answer no and nothing
happens.

## Known limits

* Only files. A selection of folders, or the desktop, does nothing.
* The shortcuts act in File Explorer windows only. They do nothing in other
  programs and nothing on the desktop.
* The contents are put on the clipboard as text. Nothing is written to disk, and
  no temporary file is created.
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
  $description: Files bigger than this are refused, with an offer to open them instead. The unit is the setting below it, and 64 MB is the most this can ever be. A megabyte here is 1,048,576 bytes, the same as Windows shows in the file listing.
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
  $description: When a file is too big or not text, ask whether to open it instead of failing silently.
- hideWhenUncopyable: true
  $name: Hide "Copy contents" when nothing can be copied
  $description: When every selected file is binary or over the size limit, the entry is left out of the menu instead of being offered and then refusing. The first few KiB of each file are read to decide, except files that are offline or on a network share, which are not read.
- separateEntries: false
  $name: Put each file on the clipboard as its own item
  $description: When several files are copied, each one goes on the clipboard in turn instead of being joined into one text. Windows clipboard history (Win+V) then lists them separately and you can paste any of them. Ctrl+V pastes only the last file. Needs clipboard history turned on, otherwise only the last file is reachable, and history keeps 25 entries of about 4 MB each, so a file over that limit is written last and can only be pasted with Ctrl+V.
- separateGapMs: 250
  $name: Wait between separate items, in milliseconds
  $description: Windows clipboard history ignores copies that arrive too close together, so the files are written with a short wait between them. Raise this if some files still go missing from the history, lower it if the copy feels slow. 0 turns the wait off.
- askHashMB: 1024
  $name: Ask before hashing a file bigger than this, in MB
  $description: Hashing reads the whole file and the File Explorer window waits until it is done. Above this size the mod asks first. 0 asks for every file.
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
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

struct Settings {
    int maxSize;
    bool maxSizeInMb;
    bool copyContents;
    bool copyHash;
    bool offerEditor;
    bool hideWhenUncopyable;
    bool separateEntries;
    int separateGapMs;
    int askHashMB;
    bool hotkeys;
    UINT contentsModifiers;
    UINT contentsKey;
    UINT hashModifiers;
    UINT hashKey;
};

// Written one per line so that adding a field cannot silently shift the rest.
Settings g_settings = {
    8,                        // maxSize
    true,                     // maxSizeInMb
    true,                     // copyContents
    true,                     // copyHash
    true,                     // offerEditor
    true,                     // hideWhenUncopyable
    false,                    // separateEntries
    250,                      // separateGapMs
    1024,                     // askHashMB
    true,                     // hotkeys
    MOD_CONTROL | MOD_SHIFT,  // contentsModifiers
    'T',                      // contentsKey
    MOD_CONTROL | MOD_SHIFT,  // hashModifiers
    'H',                      // hashKey
};

// A string setting. Wh_GetStringSetting always returns a string, never null, but
// the buffer belongs to the mod SDK and is copied out here.
std::wstring GetStringSetting(const wchar_t* name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

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
    g_settings.maxSize = Wh_GetIntSetting(L"maxSize");
    if (g_settings.maxSize <= 0) {
        g_settings.maxSize = 8;
    }
    // Two levels of cap: this one keeps the number sane, and SizeLimitBytes
    // holds the bytes at 64 MB whatever is entered here.
    if (g_settings.maxSize > 65536) {
        g_settings.maxSize = 65536;
    }
    // Only the two units are offered, and anything unreadable means megabytes,
    // which is the default.
    g_settings.maxSizeInMb =
        wcscmp(GetStringSetting(L"maxSizeUnit").c_str(), L"kb") != 0;
    g_settings.copyContents = Wh_GetIntSetting(L"copyContents") != 0;
    g_settings.copyHash = Wh_GetIntSetting(L"copyHash") != 0;
    g_settings.offerEditor = Wh_GetIntSetting(L"offerEditor") != 0;
    g_settings.hideWhenUncopyable =
        Wh_GetIntSetting(L"hideWhenUncopyable") != 0;
    g_settings.separateEntries = Wh_GetIntSetting(L"separateEntries") != 0;
    g_settings.separateGapMs = Wh_GetIntSetting(L"separateGapMs");
    if (g_settings.separateGapMs < 0) {
        g_settings.separateGapMs = 0;
    }
    if (g_settings.separateGapMs > 2000) {
        g_settings.separateGapMs = 2000;
    }
    g_settings.askHashMB = Wh_GetIntSetting(L"askHashMB");
    if (g_settings.askHashMB < 0) {
        g_settings.askHashMB = 0;
    }
    g_settings.hotkeys = Wh_GetIntSetting(L"hotkeys") != 0;
    g_settings.contentsModifiers = ParseModifiers(
        GetStringSetting(L"contentsModifiers").c_str(), MOD_CONTROL | MOD_SHIFT);
    g_settings.contentsKey =
        ParseKey(GetStringSetting(L"contentsKey").c_str(), 'T');
    g_settings.hashModifiers = ParseModifiers(
        GetStringSetting(L"hashModifiers").c_str(), MOD_CONTROL | MOD_SHIFT);
    g_settings.hashKey = ParseKey(GetStringSetting(L"hashKey").c_str(), 'H');
}

// ---------------------------------------------------------------------------
// Work in progress, and unloading
//
// Reading, hashing and the prompts all run on File Explorer's own thread, and
// the mod's code is on that thread's stack the whole time. If the mod is
// disabled or updated in the middle, it must not be unloaded while that stack
// is still in the mod. So every such stretch is counted, and
// Wh_ModBeforeUninit closes any open menu and waits for the count to reach zero.
// ---------------------------------------------------------------------------

std::atomic<bool> g_unloading{false};
// Open menus carrying the entries, and copies in progress.
std::atomic<int> g_busy{0};
// The window that owns the menu or the shortcut, for prompts and the clipboard.
std::atomic<HWND> g_busyOwner{nullptr};
// Only the outer popup call adds entries, and only one copy runs at a time per
// window thread.
thread_local bool t_inMenuHook = false;
thread_local bool t_copying = false;

struct BusyGuard {
    explicit BusyGuard(HWND owner) {
        g_busyOwner = owner;
        g_busy++;
    }
    ~BusyGuard() { g_busy--; }
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
    const long long value = g_settings.maxSize > 0 ? g_settings.maxSize : 8;
    const long long bytes = value * unit;
    if (bytes <= 0) {
        return 8LL * 1024LL * 1024LL;
    }
    return bytes > kHardMaxBytes ? kHardMaxBytes : bytes;
}

bool TooBigForHistory(const std::wstring& text) {
    return text.size() > kHistoryMaxChars;
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

// The shell browser behind a window in the file view, or null. The caller
// releases it.
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
            return reinterpret_cast<IShellBrowser*>(
                SendMessageW(h, kMsgGetShellBrowser, 0, 0));
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
    const DWORD attributes = GetFileAttributesW(path.c_str());
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

bool LooksLikeUtf8(const unsigned char* data, size_t size) {
    size_t i = 0;
    while (i < size) {
        const unsigned char lead = data[i];
        size_t extra = 0;
        unsigned int code = 0;
        if (lead < 0x80) {
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
        const unsigned char c = data[i];
        if (c == 0) {
            return false;
        }
        if (c < 0x20 && c != '\t' && c != '\r' && c != '\n') {
            return false;
        }
    }
    return true;
}

// A mark is the file saying what it is, and it is never overruled.
ReadResult ReadFileAsText(const std::wstring& path, std::wstring* out) {
    const HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE |
                                        FILE_SHARE_DELETE,
                                    nullptr, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return ReadResult::Unreadable;
    }

    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(file, &size) || size.QuadPart < 0) {
        CloseHandle(file);
        return ReadResult::Unreadable;
    }
    if (size.QuadPart > SizeLimitBytes()) {
        CloseHandle(file);
        return ReadResult::TooBig;
    }

    std::vector<unsigned char> bytes(static_cast<size_t>(size.QuadPart) + 1);
    size_t read = 0;
    while (read < static_cast<size_t>(size.QuadPart)) {
        if (g_unloading) {
            CloseHandle(file);
            return ReadResult::Unreadable;
        }
        DWORD chunk = 0;
        const DWORD want = static_cast<DWORD>(
            (static_cast<size_t>(size.QuadPart) - read) > 0x10000
                ? 0x10000
                : (static_cast<size_t>(size.QuadPart) - read));
        if (!ReadFile(file, bytes.data() + read, want, &chunk, nullptr) ||
            chunk == 0) {
            break;
        }
        read += chunk;
    }
    CloseHandle(file);
    if (read != static_cast<size_t>(size.QuadPart)) {
        return ReadResult::Unreadable;
    }
    bytes.resize(read);

    const unsigned char* data = bytes.data();
    const size_t length = bytes.size();

    if (length >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) {
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
        *out = std::wstring(reinterpret_cast<const wchar_t*>(data + 2),
                            (length - 2) / sizeof(wchar_t));
        return ReadResult::Ok;
    }
    if (length >= 2 && data[0] == 0xFE && data[1] == 0xFF) {
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
// is offline, stored online only, or on a network share is not read at all:
// reading one would start a download or wait for the network, and the menu would
// wait with it. Such a file is assumed copyable and the copy decides.
// ---------------------------------------------------------------------------

constexpr int kSniffFiles = 16;
constexpr size_t kSniffBytes = 4096;

bool IsRemoteOrSparse(const std::wstring& path) {
    const DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    if (attributes & (FILE_ATTRIBUTE_OFFLINE |
                      FILE_ATTRIBUTE_RECALL_ON_OPEN |
                      FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS)) {
        return true;
    }
    // A UNC path, which reading would fetch over the network.
    return path.size() >= 2 && path[0] == L'\\' && path[1] == L'\\';
}

// The same order the real read uses: a mark first, then UTF-16 without a mark,
// then strict UTF-8, then the system code page.
bool SniffIsText(const unsigned char* data, size_t size) {
    if (size == 0) {
        return true;
    }
    if (size >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) {
        return true;
    }
    if (size >= 2 && ((data[0] == 0xFF && data[1] == 0xFE) ||
                      (data[0] == 0xFE && data[1] == 0xFF))) {
        return true;
    }
    bool bigEndian = false;
    if (LooksLikeUtf16NoBom(data, size, &bigEndian)) {
        return true;
    }
    if (LooksLikeUtf8(data, size)) {
        return true;
    }
    return LooksLikeAnsiText(data, size);
}

bool LooksCopyable(const std::wstring& path) {
    LARGE_INTEGER size = {};
    const HANDLE probe = CreateFileW(path.c_str(), GENERIC_READ,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE |
                                         FILE_SHARE_DELETE,
                                     nullptr, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL, nullptr);
    if (probe == INVALID_HANDLE_VALUE) {
        return false;
    }
    if (!GetFileSizeEx(probe, &size) || size.QuadPart < 0 ||
        size.QuadPart > SizeLimitBytes()) {
        CloseHandle(probe);
        return false;
    }
    unsigned char buffer[kSniffBytes];
    DWORD read = 0;
    const bool ok = ReadFile(probe, buffer, sizeof(buffer), &read, nullptr) != 0;
    CloseHandle(probe);
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
    bool opened = false;
    for (int attempt = 0; attempt < 20; attempt++) {
        if (OpenClipboard(owner)) {
            opened = true;
            break;
        }
        if (g_unloading) {
            return false;
        }
        Sleep(25);
    }
    if (!opened) {
        return false;
    }
    EmptyClipboard();
    const size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL block = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!block) {
        CloseClipboard();
        return false;
    }
    void* target = GlobalLock(block);
    if (!target) {
        GlobalFree(block);
        CloseClipboard();
        return false;
    }
    memcpy(target, text.c_str(), bytes);
    GlobalUnlock(block);
    // The clipboard owns the block from here. Freeing it on failure is the only
    // case where the handle stays ours.
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
    const HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE |
                                        FILE_SHARE_DELETE,
                                    nullptr, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return std::wstring();
    }

    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    std::vector<unsigned char> object;
    std::vector<unsigned char> digest(32);
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
                if (!ReadFile(file, buffer.data(),
                              static_cast<DWORD>(buffer.size()), &read,
                              nullptr) ||
                    read == 0) {
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
            BCryptDestroyHash(hash);
        }
    }
    if (algorithm) {
        BCryptCloseAlgorithmProvider(algorithm, 0);
    }
    CloseHandle(file);
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
    const HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE |
                                        FILE_SHARE_DELETE,
                                    nullptr, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return -1;
    }
    LARGE_INTEGER size = {};
    const bool ok = GetFileSizeEx(file, &size) != 0;
    CloseHandle(file);
    return ok ? size.QuadPart : -1;
}

// ---------------------------------------------------------------------------
// The two commands
// ---------------------------------------------------------------------------

void OfferEditor(HWND owner, const std::wstring& path, const wchar_t* reason) {
    if (!g_settings.offerEditor || g_unloading) {
        return;
    }
    std::wstring text = reason;
    text += L"\n\nOpen it instead?";
    if (MessageBoxW(owner, text.c_str(), L"Copy File Contents",
                    MB_YESNO | MB_ICONINFORMATION) == IDYES) {
        ShellExecuteW(owner, L"open", L"notepad.exe", path.c_str(), nullptr,
                      SW_SHOWNORMAL);
    }
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
    size_t budget = kTotalTextBytes;

    try {
        for (const std::wstring& path : paths) {
            if (g_unloading) {
                return;
            }
            if (IsDirectory(path)) {
                continue;
            }
            if (FileSizeBytes(path) > SizeLimitBytes()) {
                // Checked before reading, so an oversized file costs nothing.
                lastRefused = path;
                lastReason = L"This file is larger than the size limit.";
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
                continue;
            }
            // The whole selection is held to a budget, so Ctrl+A in a folder of
            // logs cannot make File Explorer allocate without bound.
            const size_t bytes = text.size() * sizeof(wchar_t);
            if (bytes > budget) {
                lastRefused = path;
                lastReason = L"That selection contains more text than the mod "
                             L"will copy at once.";
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
                OfferEditor(owner, lastRefused, lastReason);
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
                // immediately and it only ever sees the last one.
                if (n + 1 < order.size() && gapMs > 0) {
                    Sleep(static_cast<DWORD>(gapMs));
                }
            }
            if (written != static_cast<int>(parts.size())) {
                Wh_Log(L"only %d of %d items reached the clipboard", written,
                       static_cast<int>(parts.size()));
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
            // Several files are marked with their names, so a paste into an
            // editor says where each part came from. One file is copied bare,
            // because that is what "copy the contents" means.
            if (parts.size() > 1) {
                combined += L"=== ";
                combined += part.name;
                combined += L" ======\r\n";
            }
            combined += part.text;
        }
        if (!SetClipboardText(owner, combined)) {
            Wh_Log(L"the clipboard was busy, nothing was copied");
        }
    } catch (...) {
        // An allocation failure inside a hook would otherwise end Explorer.
        Wh_Log(L"ran out of memory while copying, nothing was copied");
    }
}

bool ConfirmLargeHash(HWND owner, const std::wstring& path) {
    const long long size = FileSizeBytes(path);
    if (size < 0 || g_settings.askHashMB <= 0) {
        return true;
    }
    const long long limit = static_cast<long long>(g_settings.askHashMB) *
                            1024LL * 1024LL;
    if (size <= limit) {
        return true;
    }
    wchar_t text[512];
    swprintf_s(text,
               L"This file is %.1f GB. Hashing reads all of it, and this window "
               L"will not respond until it is done.\n\nHash it anyway?",
               static_cast<double>(size) / (1024.0 * 1024.0 * 1024.0));
    return MessageBoxW(owner, text, L"Copy SHA-256",
                       MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES;
}

void DoCopyHash(HWND owner, const std::vector<std::wstring>& paths) {
    try {
        // One prompt for the whole operation, about the largest file in it.
        if (paths.size() == 1 && !ConfirmLargeHash(owner, paths[0])) {
            return;
        }
        std::wstring combined;
        int copied = 0;
        for (const std::wstring& path : paths) {
            if (g_unloading) {
                return;
            }
            if (IsDirectory(path)) {
                continue;
            }
            bool ok = false;
            const std::wstring hex = Sha256OfFile(path, &ok);
            if (!ok) {
                continue;
            }
            if (!combined.empty()) {
                combined += L"\r\n";
            }
            combined += hex;
            if (paths.size() > 1) {
                combined += L"  ";
                combined += FileNameOf(path);
            }
            copied++;
        }
        if (copied > 0 && !combined.empty()) {
            if (!SetClipboardText(owner, combined)) {
                Wh_Log(L"the clipboard was busy, nothing was copied");
            }
        }
    } catch (...) {
        Wh_Log(L"ran out of memory while hashing, nothing was copied");
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
                    BusyGuard busy(root);
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

bool IdsAreFree(HMENU menu, UINT base) {
    return GetMenuState(menu, base, MF_BYCOMMAND) == (UINT)-1 &&
           GetMenuState(menu, base + 1, MF_BYCOMMAND) == (UINT)-1;
}

UINT PickMenuIdBase(HMENU menu) {
    for (int i = 0; i < kMenuIdTries; i++) {
        const UINT base = kMenuIdBase + static_cast<UINT>(i) * 2;
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
}

// Adds the entries when this popup is a File Explorer file menu, and reports
// the id base it used, or 0.
UINT AddEntries(HMENU menu, UINT uFlags, HWND hWnd,
                std::vector<std::wstring>* paths) {
    if (!(uFlags & TPM_RETURNCMD) || !menu || !hWnd) {
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

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    if (addContents) {
        AppendMenuW(menu, MF_STRING, base, L"Copy contents");
    }
    if (addHash) {
        AppendMenuW(menu, MF_STRING, base + 1, L"Copy SHA-256");
    }
    return base;
}

template <typename Track>
BOOL TrackWithEntries(HMENU menu, UINT uFlags, HWND owner, Track track) {
    // TrackPopupMenu may call TrackPopupMenuEx. Only the outer call adds.
    if (g_unloading || t_inMenuHook) {
        return track();
    }

    std::vector<std::wstring> paths;
    const UINT base = AddEntries(menu, uFlags, owner, &paths);
    if (!base) {
        return track();
    }

    // Counted from here to the end of the scope, so unloading waits for both
    // the open menu and the copy that may follow it, and so nothing is left
    // behind if the copy throws.
    BusyGuard busy(owner);
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
    return TrackWithEntries(hMenu, uFlags, hWnd, [&] {
        return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, params);
    });
}

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                int reserved, HWND hWnd, const RECT* rect) {
    return TrackWithEntries(hMenu, uFlags, hWnd, [&] {
        return TrackPopupMenu_Original(hMenu, uFlags, x, y, reserved, hWnd, rect);
    });
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Copy File Contents 1.1");
    LoadSettings();
    Wh_Log(L"settings: max %d %s (%lld bytes), contents=%d hash=%d",
           g_settings.maxSize, g_settings.maxSizeInMb ? L"MB" : L"KB",
           SizeLimitBytes(), g_settings.copyContents, g_settings.copyHash);

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
        return FALSE;
    }
    Wh_Log(L"ready, %d of 3 hooks installed", hooked);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"settings changed: max %d %s (%lld bytes)",
           g_settings.maxSize, g_settings.maxSizeInMb ? L"MB" : L"KB",
           SizeLimitBytes());
}

void Wh_ModBeforeUninit() {
    // No new work from here on: the hooks pass everything straight through.
    g_unloading = true;

    // A menu carrying the entries may still be open, and a copy or a prompt may
    // still be running with this mod on the stack. Close the menu and give the
    // work two seconds to finish, so nothing of this mod is on any thread when
    // it is unloaded.
    for (int i = 0; i < 100 && g_busy > 0; i++) {
        HWND owner = g_busyOwner;
        if (owner) {
            PostMessageW(owner, WM_CANCELMODE, 0, 0);
        }
        Sleep(20);
    }
}
