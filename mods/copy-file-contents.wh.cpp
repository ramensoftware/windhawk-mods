// ==WindhawkMod==
// @id              copy-file-contents
// @name            Copy File Contents
// @description     Copies a selected file's text or its SHA-256 hash without opening it, from a keyboard shortcut or the right-click menu.
// @version         1.0
// @author          metanoid18
// @github          https://github.com/Metanoid18
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lshell32 -lbcrypt
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
Explorer window has the focus, and it is ignored while the rename box, the
address bar or the search box has it, so it never eats a letter you are typing.
Every key combination here can be changed in the settings. Do not reuse
Ctrl+Shift+C or Ctrl+Shift+N, which are File Explorer's own Copy as path and New
folder, because the mod would take the key from Explorer.

**Right-click.** The two entries are also added to the classic menu, reached
with **Show more options** or Shift+F10. They are added at the bottom of that
menu. The new Windows 11 menu belongs to a different component and cannot be
extended by injected code without registering a handler in the registry, which a
mod must not do. To get the entries on a plain right-click, use the separate mod
**Classic context menu on Windows 11**. If the classic menu cannot be reached on
your build, the keyboard shortcuts are the whole of the mod and they do not need
the menu at all.

## What it will and will not copy

A file is copied only if it looks like text. Files that look binary are refused
rather than pasted as garbage, and files larger than the size limit are refused
the same way. When either happens the mod offers to open the file instead, which
is the honest answer: this is not a text file, so read it in an editor.

The size limit is 8 MB by default, with the unit chosen next to it, megabytes or
kilobytes. A large selection is copied file by file, so one file over the limit is
skipped and the rest are still copied. That partial copy is silent, because a
dialog in the middle of copying five files is worse than a missing one. When
nothing at all can be copied, you are asked instead.

The ceiling is 64 MB whatever the setting says: a text file that big is already
beyond what a clipboard is good for, and the mod refuses rather than trying.

When nothing in the selection can be copied, **Copy contents** is left out of the
menu instead of being offered and then refusing. The first 4 KiB of each file are
read to decide that, stopping at the first file that passes and looking at no
more than 16 files, so the menu opens at the same speed as without the mod. A
mixed selection still shows the entry and copies the files that can be copied.

Encoding is detected rather than assumed: a byte order mark is honored, otherwise
the file is checked for valid UTF-8, then for UTF-16 without a mark, and finally
read as the system code page. Line endings are copied as they are, so a file with
Windows line endings lands on the clipboard the same way.

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
  $description: When every selected file is binary or over the size limit, the entry is left out of the menu instead of being offered and then refusing. The first few KiB of each file are read to decide, so the menu opens without waiting on the whole file.
- separateEntries: false
  $name: Put each file on the clipboard as its own item
  $description: When several files are copied, each one goes on the clipboard in turn instead of being joined into one text. Windows clipboard history (Win+V) then lists them separately and you can paste any of them. Ctrl+V pastes only the last file. Needs clipboard history turned on, otherwise only the last file is reachable, and history keeps 25 entries of about 4 MB each, so a file over that limit is written last and can only be pasted with Ctrl+V.
- separateGapMs: 250
  $name: Wait between separate items, in milliseconds
  $description: Windows clipboard history ignores copies that arrive too close together, so the files are written with a short wait between them. Raise this if some files still go missing from the history, lower it if the copy feels slow. 0 turns the wait off.
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
#include <shobjidl.h>
#include <bcrypt.h>

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
    bool hotkeys;
    UINT contentsModifiers;
    UINT contentsKey;
    UINT hashModifiers;
    UINT hashKey;
};

// The same defaults as the settings block, written one per line so that adding a
// field cannot silently shift the ones after it.
Settings g_settings = {
    8,                          // maxSize
    true,                       // maxSizeInMb
    true,                       // copyContents
    true,                       // copyHash
    true,                       // offerEditor
    true,                       // hideWhenUncopyable
    false,                      // separateEntries
    250,                        // separateGapMs
    true,                       // hotkeys
    MOD_CONTROL | MOD_SHIFT,    // contentsModifiers
    'T',                        // contentsKey
    MOD_CONTROL | MOD_SHIFT,    // hashModifiers
    'H',                        // hashKey
};

// A string setting, copied so the caller never holds the mod SDK's buffer.
std::wstring GetStringSetting(const wchar_t* name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    if (value) {
        Wh_FreeStringSetting(value);
    }
    return result;
}

UINT ParseModifiers(const wchar_t* text, UINT fallback) {
    if (!text) {
        return fallback;
    }
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
// The selection
//
// Explorer keeps the selected items on the shell view that owns the window the
// menu was opened on. WM_USER + 7 is the documented-by-practice way to ask a
// shell view window for its IShellBrowser; the same route the repository's
// existing context menu mods use.
// ---------------------------------------------------------------------------

std::vector<std::wstring> GetSelectedPaths(HWND hWnd) {
    std::vector<std::wstring> paths;

    IShellBrowser* browser = nullptr;
    for (HWND hWndWalk = hWnd; hWndWalk && !browser;
         hWndWalk = GetParent(hWndWalk)) {
        browser = reinterpret_cast<IShellBrowser*>(
            SendMessageW(hWndWalk, WM_USER + 7, 0, 0));
    }
    if (!browser) {
        return paths;
    }

    IShellView* view = nullptr;
    if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view) {
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

// The size limit in bytes. One place computes it, so the menu check and the real
// copy can never disagree about which files are too big.
constexpr long long kHardMaxBytes = 64LL * 1024LL * 1024LL;

long long SizeLimitBytes() {
    const long long unit = g_settings.maxSizeInMb ? 1024LL * 1024LL : 1024LL;
    const long long value = g_settings.maxSize > 0 ? g_settings.maxSize : 8;
    const long long bytes = value * unit;
    if (bytes <= 0) {
        return 8LL * 1024LL * 1024LL;
    }
    return bytes > kHardMaxBytes ? kHardMaxBytes : bytes;
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

// UTF-8 that a strict decoder accepts. Rejects overlong forms and surrogates,
// which is what separates a text file from a binary that happens to have no
// control bytes in its first few kilobytes.
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

// Text that is not UTF-8 and has no mark: no control bytes other than the three
// a text file is allowed to contain.
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

// UTF-16 without a mark: every other byte is zero in the ASCII range, which is
// what these files look like in practice.
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

    // A mark is the file saying what it is, and it is never overruled.
    if (length >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) {
        *out = std::wstring();
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
// What Windows clipboard history accepts
//
// History is a separate feature with its own limits, documented by Microsoft: 25
// entries, and roughly 4 MB per entry. Text is stored as UTF-16, so that is
// about two million characters. An entry over the cap is not stored at all, and
// nothing tells the program that put it there, so the mod has to know the limit
// itself and arrange the writes around it.
// ---------------------------------------------------------------------------

constexpr size_t kHistoryMaxBytes = 4 * 1024 * 1024;
constexpr size_t kHistoryMaxChars = kHistoryMaxBytes / sizeof(wchar_t);

bool TooBigForHistory(const std::wstring& text) {
    return text.size() > kHistoryMaxChars;
}

// ---------------------------------------------------------------------------
// Can this be copied at all?
//
// The menu should not offer an entry that is certain to refuse, so the files are
// checked when the menu opens. Only the first few KiB of each file are read: the
// question is "text or not", and a first block answers it without touching the
// rest. The check stops at the first file that passes, which is the common case,
// and gives up after a fixed number of files so a huge selection cannot slow the
// menu down.
// ---------------------------------------------------------------------------

constexpr int kSniffFiles = 16;
constexpr size_t kSniffBytes = 4096;

// The same order the real read uses: a mark first, then strict UTF-8, then
// UTF-16 without a mark, then the system code page. Kept next to the real read
// so the two cannot drift apart unnoticed.
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
    if (LooksLikeUtf8(data, size)) {
        return true;
    }
    bool bigEndian = false;
    if (LooksLikeUtf16NoBom(data, size, &bigEndian)) {
        return true;
    }
    return LooksLikeAnsiText(data, size);
}

// Reads a bit of the file and asks whether it could be copied. A file over the
// size limit is not copyable, and neither is one that cannot be opened.
bool LooksCopyable(const std::wstring& path) {
    const HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE |
                                        FILE_SHARE_DELETE,
                                    nullptr, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    bool result = false;
    LARGE_INTEGER size = {};
    if (GetFileSizeEx(file, &size) && size.QuadPart >= 0 &&
        size.QuadPart <= SizeLimitBytes()) {
        unsigned char buffer[kSniffBytes];
        DWORD read = 0;
        if (ReadFile(file, buffer, sizeof(buffer), &read, nullptr)) {
            result = SniffIsText(buffer, read);
        }
    }
    CloseHandle(file);
    return result;
}

// True when at least one selected file could be copied. Folders are skipped.
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
// open is retried for a short while before giving up.
bool SetClipboardText(const std::wstring& text) {
    bool opened = false;
    for (int attempt = 0; attempt < 20; attempt++) {
        if (OpenClipboard(nullptr)) {
            opened = true;
            break;
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

// ---------------------------------------------------------------------------
// The two commands
// ---------------------------------------------------------------------------

void OfferEditor(const std::wstring& path, const wchar_t* reason) {
    if (!g_settings.offerEditor) {
        return;
    }
    std::wstring text = reason;
    text += L"\n\nOpen it instead?";
    if (MessageBoxW(nullptr, text.c_str(), L"Copy File Contents",
                    MB_YESNO | MB_ICONINFORMATION) == IDYES) {
        ShellExecuteW(nullptr, L"open", L"notepad.exe", path.c_str(), nullptr,
                      SW_SHOWNORMAL);
    }
}

void DoCopyContents(const std::vector<std::wstring>& paths) {
    const bool separate = g_settings.separateEntries;
    const int gapMs = g_settings.separateGapMs;
    // Everything that could be copied, in the order it was selected.
    struct Part {
        std::wstring name;
        std::wstring text;
    };
    std::vector<Part> parts;
    std::wstring lastRefused;
    const wchar_t* lastReason = nullptr;

    for (const std::wstring& path : paths) {
        if (IsDirectory(path)) {
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
        Part part;
        part.name = FileNameOf(path);
        part.text.swap(text);
        parts.push_back(std::move(part));
    }

    if (parts.empty()) {
        if (!lastRefused.empty()) {
            OfferEditor(lastRefused, lastReason);
        }
        return;
    }

    // One file on the clipboard per file, so Windows clipboard history keeps
    // them apart. Each write replaces the one before, which is why the last
    // file is the one a plain paste gives.
    if (separate && parts.size() > 1) {
        // Anything over the history cap cannot be kept as an entry. Writing
        // those first would spend the clipboard on the last small file, so they
        // go last instead: history keeps the small ones and Ctrl+V gives the
        // big one, which is the only copy of it the user can get.
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
            const Part& part = parts[order[n]];
            const bool placed = SetClipboardText(part.text);
            if (placed) {
                written++;
            }
            // Windows clipboard history is a separate service that reads the
            // clipboard when it is told it changed. Write the next file
            // immediately and it only ever sees the last one, which is exactly
            // what a run of this mod showed. The wait gives it its turn.
            if (n + 1 < order.size() && gapMs > 0) {
                Sleep((DWORD)gapMs);
            }
        }
        if (written != (int)parts.size()) {
            Wh_Log(L"only %d of %d items reached the clipboard", written,
                   (int)parts.size());
        }
        return;
    }

    std::wstring combined;
    for (const Part& part : parts) {
        if (!combined.empty()) {
            combined += L"\r\n";
        }
        // Several files are marked with their names, so a paste into an editor
        // says where each part came from. One file is copied bare, because that
        // is what "copy the contents" means.
        if (paths.size() > 1) {
            combined += L"=== ";
            combined += part.name;
            combined += L" ======\r\n";
        }
        combined += part.text;
    }
    if (!SetClipboardText(combined)) {
        Wh_Log(L"the clipboard was busy, nothing was copied");
    }
}

void DoCopyHash(const std::vector<std::wstring>& paths) {
    std::wstring combined;
    int copied = 0;
    for (const std::wstring& path : paths) {
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
    if (copied > 0) {
        if (!SetClipboardText(combined)) {
            Wh_Log(L"the clipboard was busy, nothing was copied");
        }
    }
}

// ---------------------------------------------------------------------------
// The shortcut
//
// A key press reaches this mod through TranslateAcceleratorW, which File
// Explorer calls on the window's own thread for every key message it handles.
// Matching there means the shortcut only exists while a File Explorer window
// has the focus. Nothing is registered system wide, so no other program can
// lose a key to this mod.
// ---------------------------------------------------------------------------

bool ModifiersHeld(UINT wanted) {
    UINT held = 0;
    if (GetKeyState(VK_CONTROL) & 0x8000) held |= MOD_CONTROL;
    if (GetKeyState(VK_SHIFT) & 0x8000) held |= MOD_SHIFT;
    if (GetKeyState(VK_MENU) & 0x8000) held |= MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) held |= MOD_WIN;
    return held == wanted;
}

bool IsExplorerFrame(HWND hWnd) {
    wchar_t name[64];
    if (!hWnd || !GetClassNameW(hWnd, name, ARRAYSIZE(name))) {
        return false;
    }
    return wcscmp(name, L"CabinetWClass") == 0 ||
           wcscmp(name, L"ExploreWClass") == 0;
}

// True while the user is typing in this window: the rename box, the address
// bar, the search box, or any other text field. The key belongs to that field
// then, not to the mod.
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
    // Whatever its class, a window with a caret takes typed text.
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

using TranslateAcceleratorW_t = int(WINAPI*)(HWND, HACCEL, LPMSG);
TranslateAcceleratorW_t TranslateAcceleratorW_Original;

// Raised while a copy runs. The refusal prompt pumps messages, so a second
// press while it is up would otherwise open a second prompt on top of it.
bool g_copying = false;

int WINAPI TranslateAcceleratorW_Hook(HWND hWnd, HACCEL hAccTable,
                                      LPMSG msg) {
    if (!g_copying && msg &&
        (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN) &&
        // Bit 30 marks an auto repeat. One press is one copy.
        !(msg->lParam & (1u << 30))) {
        const int which = MatchHotkey(static_cast<UINT>(msg->wParam));
        if (which) {
            HWND root = GetAncestor(msg->hwnd, GA_ROOT);
            if (IsExplorerFrame(root) && !IsTextFieldFocused(root, msg->hwnd)) {
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
                    g_copying = true;
                    if (which == 1) {
                        DoCopyContents(paths);
                    } else {
                        DoCopyHash(paths);
                    }
                    g_copying = false;
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
// of the build, not of the menu. On the Windows 11 build this was measured on,
// the classic menu came through the plain TrackPopupMenu, with TPM_RETURNCMD
// set, owned by the shell view itself. So both functions are hooked and both
// share this code. Either of them reports the chosen entry, which is what makes
// an entry added here work; a popup without TPM_RETURNCMD is left alone,
// because an entry that cannot report its own click is a dead button.
// ---------------------------------------------------------------------------

constexpr UINT kCopyContentsId = 0x0C01;
constexpr UINT kCopyHashId = 0x0C02;

struct MenuScope {
    std::vector<std::wstring> paths;
};

// True when the window sits inside the file view of a File Explorer window.
// The navigation pane, the address bar and the rest of the frame are outside
// it, so a menu opened there is never given the two entries.
bool InFileView(HWND hWnd) {
    for (HWND h = hWnd; h; h = GetAncestor(h, GA_PARENT)) {
        wchar_t name[64];
        if (!GetClassNameW(h, name, ARRAYSIZE(name))) {
            return false;
        }
        if (wcscmp(name, L"SHELLDLL_DefView") == 0) {
            return true;
        }
    }
    return false;
}

// Decides whether this popup is a File Explorer file menu, and if so puts the
// two entries on it and keeps the selection that goes with them.
bool PrepareMenu(HMENU hMenu, UINT uFlags, HWND hWnd, MenuScope* scope) {
    if (!hWnd || !IsExplorerFrame(GetAncestor(hWnd, GA_ROOT))) {
        return false;
    }
    if (!(uFlags & TPM_RETURNCMD) || !hMenu) {
        return false;
    }
    if (!InFileView(hWnd)) {
        return false;
    }

    scope->paths = GetSelectedPaths(hWnd);
    bool anyFile = false;
    for (const std::wstring& path : scope->paths) {
        if (!IsDirectory(path)) {
            anyFile = true;
            break;
        }
    }
    if (!anyFile) {
        return false;
    }

    // Both ids must be unused. A menu that already carries one of them belongs
    // to another program's extension, and adding a second item with the same id
    // would make one of the two fire the other's command.
    if (GetMenuState(hMenu, kCopyContentsId, MF_BYCOMMAND) != (UINT)-1 ||
        GetMenuState(hMenu, kCopyHashId, MF_BYCOMMAND) != (UINT)-1) {
        Wh_Log(L"menu already carries one of the two ids, leaving it alone");
        return false;
    }

    bool addContents = g_settings.copyContents;
    const bool addHash = g_settings.copyHash;
    if (addContents && g_settings.hideWhenUncopyable &&
        !AnyCopyableText(scope->paths)) {
        addContents = false;
    }
    if (!addContents && !addHash) {
        return false;
    }

    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    if (addContents) {
        AppendMenuW(hMenu, MF_STRING, kCopyContentsId, L"Copy contents");
    }
    if (addHash) {
        AppendMenuW(hMenu, MF_STRING, kCopyHashId, L"Copy SHA-256");
    }
    return true;
}

// Runs the entry the user picked, and reports what the popup call should
// return to File Explorer. Any other choice is File Explorer's own, and the
// value is passed back untouched.
BOOL FinishMenu(UINT chosen, const MenuScope& scope) {
    if (chosen == kCopyContentsId || chosen == kCopyHashId) {
        if (g_copying) {
            // Another copy is running, which means a prompt is up. Reporting it
            // as handled stops a second prompt from stacking on top of it.
            return TRUE;
        }
        g_copying = true;
        if (chosen == kCopyContentsId) {
            DoCopyContents(scope.paths);
        } else {
            DoCopyHash(scope.paths);
        }
        g_copying = false;
        return TRUE;
    }
    return chosen != 0;
}

using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                  HWND hWnd, LPTPMPARAMS params) {
    MenuScope scope;
    if (!PrepareMenu(hMenu, uFlags, hWnd, &scope)) {
        return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, params);
    }
    const UINT chosen =
        TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, params);
    return FinishMenu(chosen, scope);
}

// The plain TrackPopupMenu passes one more reserved parameter and no TPMPARAMS.
using TrackPopupMenu_t = BOOL(WINAPI*)(HMENU, UINT, int, int, int, HWND,
                                       const RECT*);
TrackPopupMenu_t TrackPopupMenu_Original;

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                int reserved, HWND hWnd, const RECT* rect) {
    MenuScope scope;
    if (!PrepareMenu(hMenu, uFlags, hWnd, &scope)) {
        return TrackPopupMenu_Original(hMenu, uFlags, x, y, reserved, hWnd, rect);
    }
    const UINT chosen =
        TrackPopupMenu_Original(hMenu, uFlags, x, y, reserved, hWnd, rect);
    return FinishMenu(chosen, scope);
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

// Installs one hook, and says in the log when it could not. Returns 1 on
// success so the caller can count what actually took.
int HookOne(HMODULE user32, const wchar_t* name, void* hook, void** original) {
    // GetProcAddress wants a narrow name; every name here is plain ASCII.
    char narrow[64];
    size_t i = 0;
    for (; name[i] && i + 1 < ARRAYSIZE(narrow); i++) {
        narrow[i] = static_cast<char>(name[i]);
    }
    narrow[i] = 0;

    void* target = reinterpret_cast<void*>(GetProcAddress(user32, narrow));
    if (!target) {
        Wh_Log(L"%s not found, that path is off", name);
        return 0;
    }
    if (!Wh_SetFunctionHook(target, hook, original)) {
        Wh_Log(L"could not hook %s, that path is off", name);
        return 0;
    }
    return 1;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Copy File Contents 1.0");
    LoadSettings();
    Wh_Log(L"settings: max %d %s (%lld bytes), contents=%d hash=%d",
           g_settings.maxSize, g_settings.maxSizeInMb ? L"MB" : L"KB",
           SizeLimitBytes(), g_settings.copyContents, g_settings.copyHash);

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        Wh_Log(L"user32 is not loaded, nothing to hook");
        return FALSE;
    }

    // The paths are independent. Either one working leaves the mod useful, so a
    // failure on one is logged and the others are still installed.
    int hooked = 0;
    hooked += HookOne(user32, L"TrackPopupMenuEx",
                      reinterpret_cast<void*>(TrackPopupMenuEx_Hook),
                      reinterpret_cast<void**>(&TrackPopupMenuEx_Original));
    hooked += HookOne(user32, L"TrackPopupMenu",
                      reinterpret_cast<void*>(TrackPopupMenu_Hook),
                      reinterpret_cast<void**>(&TrackPopupMenu_Original));
    hooked += HookOne(user32, L"TranslateAcceleratorW",
                      reinterpret_cast<void*>(TranslateAcceleratorW_Hook),
                      reinterpret_cast<void**>(&TranslateAcceleratorW_Original));

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
