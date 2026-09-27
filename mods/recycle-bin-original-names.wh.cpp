// ==WindhawkMod==
// @id              recycle-bin-original-names
// @name            Real Names in Recycle Bin Prompts
// @description     Shows the file's real name in the prompts for deleting it from the Recycle Bin, instead of the internal $R name the Recycle Bin keeps it under
// @name:ru         Настоящие имена файлов в запросах Корзины
// @description:ru  Показывает в запросах на удаление из Корзины настоящее имя файла вместо служебного $R-имени, под которым Корзина его хранит
// @version         1.1
// @author          appEW
// @github          https://github.com/appEW
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshlwapi -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Real Names in Recycle Bin Prompts

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

Delete something out of the Recycle Bin and the confirmation asks about a file
you never had:

> Are you sure you want to delete "$RVAZ2WZ.b"?

The Recycle Bin does not keep a deleted file under its own name. It renames it
to `$R` plus a few random characters, keeping only the extension, and writes the
real path into a companion `$I` file next to it. Everything that asks the
Recycle Bin folder for a display name gets the original name back; anything that
only looks at the file on disk sees the stub.

That is what the prompt does. The shell hands it both the item and its path, and
the path wins: `SHGetFileInfo` is asked for a display name for
`C:\$Recycle.Bin\<SID>\$RVAZ2WZ.b`, and for a plain file path that is just the
file name - the stub. The progress dialog asks the item instead, which is why it
shows the right name where the prompt does not.

This mod reads the `$I` file and puts the real name back. When `SHGetFileInfo`
is asked for the display name of a `$R` file inside `$Recycle.Bin`, it answers
with the name recorded in the `$I` file next to it - one small file read, no
searching.

As a fallback for prompts that put the stub's name straight into their text,
the labels of dialogs (never edit boxes or other controls that hold data) are
checked for `$R` names as well:

* a full `C:\$Recycle.Bin\<SID>\$R...` path is replaced by the original path,
  read from the `$I` file next to it;
* a bare `$R` name is looked up in the `$Recycle.Bin` folders of the fixed
  drives. That listing is taken at most once a second.

Text with no `$R` in it is passed straight through, and a `$R` that no `$I`
file accounts for is left exactly as it was - a file of your own called
`$Report.xlsx` is never touched.

## What it does not do

Nothing about the Recycle Bin's contents, its folder view, or the files
themselves. This only changes the name a prompt is about to display.

---

## По-русски

Удалите что-нибудь из Корзины, и подтверждение спросит об удалении файла,
которого у вас никогда не было, - например, «$RVAZ2WZ.b».

Корзина не хранит удалённый файл под его настоящим именем. Она переименовывает
его в `$R` и несколько случайных символов, сохраняя только расширение, а
настоящий путь записывает в парный файл `$I` рядом. Всё, что спрашивает имя у
папки Корзины, получает настоящее имя; всё, что смотрит на сам файл на диске,
видит служебное.

Запрос на удаление как раз смотрит на файл на диске. Мод читает файл `$I` и
возвращает настоящее имя: когда у `SHGetFileInfo` спрашивают отображаемое имя
`$R`-файла в `$Recycle.Bin`, мод отвечает именем, записанным в соседнем файле
`$I`, - это одно чтение маленького файла, без всякого поиска.

Запасной путь - для запросов, которые вписывают служебное имя прямо в текст:
надписи в окнах (но никогда не поля ввода и другие элементы с данными)
проверяются на `$R`-имена:

* полный путь `C:\$Recycle.Bin\<SID>\$R...` заменяется исходным путём из
  соседнего файла `$I`;
* одиночное `$R`-имя ищется в папках `$Recycle.Bin` несъёмных дисков, причём
  список этих папок составляется не чаще раза в секунду.

Текст без `$R` проходит без изменений, а `$R`, для которого нет файла `$I`,
остаётся как есть, - ваш собственный файл `$Report.xlsx` мод не тронет.

Мод ничего не меняет в содержимом Корзины, в её окне и в самих файлах - только
имя, которое запрос собирается показать.

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fullPath: false
  $name: Show the full original path
  $name:ru: Показывать полный исходный путь
  $description: >-
    Name the file by where it was deleted from ("C:\Users\Me\Notes\a.b")
    rather than by its name alone ("a.b").
  $description:ru: >-
    Называть файл местом, откуда он был удалён ("C:\Users\Me\Notes\a.b"),
    а не одним именем ("a.b").
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <windhawk_utils.h>

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

// One deleted item: the folder it sits in, and its name with the leading "$I"
// cut off - which is exactly what follows "$R" in the stub's own name.
struct StubEntry {
    std::wstring dir;
    std::wstring suffix;
};

static std::mutex g_indexMutex;
static std::vector<StubEntry> g_stubs;
static bool g_stubsBuilt = false;
static ULONGLONG g_stubsTick = 0;

// A listing is at most this old before it is taken again. It also caps how
// often the drives are listed at all, whether a lookup found something or not.
constexpr ULONGLONG kIndexLifetimeMs = 1000;

static std::atomic<bool> g_fullPath{false};

// Rebuilt rather than watched: a prompt appears rarely enough that listing the
// Recycle Bin folders on the spot is cheaper than keeping a live view of them.
// Only fixed drives are listed; Windows keeps no Recycle Bin on removable ones.
static void BuildStubIndex() {
    g_stubs.clear();

    DWORD driveMask = GetLogicalDrives();
    for (int i = 0; i < 26; i++) {
        if (!(driveMask & (1u << i))) {
            continue;
        }

        WCHAR root[4] = {(WCHAR)(L'A' + i), L':', L'\\', L'\0'};
        if (GetDriveTypeW(root) != DRIVE_FIXED) {
            continue;
        }

        std::wstring bin = std::wstring(root) + L"$Recycle.Bin\\";

        WIN32_FIND_DATAW findUser;
        HANDLE hUsers =
            FindFirstFileExW((bin + L"*").c_str(), FindExInfoBasic, &findUser,
                             FindExSearchLimitToDirectories, NULL, 0);
        if (hUsers == INVALID_HANDLE_VALUE) {
            continue;
        }

        do {
            if (!(findUser.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                findUser.cFileName[0] == L'.') {
                continue;
            }

            // One folder per user, named after their SID. Other users' folders
            // are unreadable, and FindFirstFile simply fails on them.
            std::wstring userDir = bin + findUser.cFileName + L"\\";

            WIN32_FIND_DATAW findItem;
            HANDLE hItems =
                FindFirstFileExW((userDir + L"$I*").c_str(), FindExInfoBasic,
                                 &findItem, FindExSearchNameMatch, NULL, 0);
            if (hItems == INVALID_HANDLE_VALUE) {
                continue;
            }

            do {
                if (findItem.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    continue;
                }
                if (findItem.cFileName[0] != L'$' ||
                    (findItem.cFileName[1] != L'I' &&
                     findItem.cFileName[1] != L'i') ||
                    !findItem.cFileName[2]) {
                    continue;
                }

                g_stubs.push_back({userDir, findItem.cFileName + 2});
            } while (FindNextFileW(hItems, &findItem));

            FindClose(hItems);
        } while (FindNextFileW(hUsers, &findUser));

        FindClose(hUsers);
    }

    g_stubsBuilt = true;
    g_stubsTick = GetTickCount64();
}

// Lists the Recycle Bin folders again, unless that was done less than
// kIndexLifetimeMs ago.
static void RefreshStubIndex() {
    if (g_stubsBuilt && GetTickCount64() - g_stubsTick < kIndexLifetimeMs) {
        return;
    }
    BuildStubIndex();
}

// The $I file: an eight byte version, the size, the deletion time, and then the
// path the file was deleted from - fixed at 260 characters in version 1, and
// preceded by its own length in version 2.
static bool ReadOriginalPath(const std::wstring& metaPath, std::wstring* path) {
    HANDLE hFile =
        CreateFileW(metaPath.c_str(), GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
                    OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    BYTE header[28];
    DWORD headerRead = 0;
    if (!ReadFile(hFile, header, sizeof(header), &headerRead, NULL) ||
        headerRead < 24) {
        CloseHandle(hFile);
        return false;
    }

    INT64 version = 0;
    memcpy(&version, header, sizeof(version));

    DWORD cchPath = 0;
    if (version == 1) {
        cchPath = 260;

        // The header read overshot the fixed field's start; step back to it.
        LARGE_INTEGER pos;
        pos.QuadPart = 24;
        if (!SetFilePointerEx(hFile, pos, NULL, FILE_BEGIN)) {
            CloseHandle(hFile);
            return false;
        }
    } else if (version == 2) {
        if (headerRead < sizeof(header)) {
            CloseHandle(hFile);
            return false;
        }
        memcpy(&cchPath, header + 24, sizeof(cchPath));
        if (cchPath == 0 || cchPath > 32768) {
            CloseHandle(hFile);
            return false;
        }
    } else {
        CloseHandle(hFile);
        return false;
    }

    std::vector<WCHAR> buf(cchPath + 1, L'\0');
    DWORD pathRead = 0;
    BOOL ok = ReadFile(hFile, buf.data(), cchPath * sizeof(WCHAR), &pathRead, NULL);
    CloseHandle(hFile);

    if (!ok || pathRead < sizeof(WCHAR)) {
        return false;
    }

    buf[pathRead / sizeof(WCHAR)] = L'\0';
    buf[cchPath] = L'\0';
    if (!buf[0]) {
        return false;
    }

    path->assign(buf.data());
    return true;
}

// For "...\$Recycle.Bin\<SID>\$R<suffix>", reads the original path out of the
// "$I<suffix>" file next to it. Nothing is listed: the path says where to look.
static bool OriginalPathForStubPath(LPCWSTR stubPath, std::wstring* original) {
    if (!stubPath || !StrStrIW(stubPath, L"\\$Recycle.Bin\\")) {
        return false;
    }

    LPCWSTR name = PathFindFileNameW(stubPath);
    if (name[0] != L'$' || (name[1] != L'R' && name[1] != L'r') || !name[2]) {
        return false;
    }

    std::wstring metaPath(stubPath);
    metaPath[(name - stubPath) + 1] = L'I';
    return ReadOriginalPath(metaPath, original);
}

// `text` points at a "$R". Longest suffix wins, so a stub name that happens to
// begin with another one cannot be cut short.
static bool MatchStub(const WCHAR* text, size_t* matched, std::wstring* path) {
    const StubEntry* best = NULL;
    size_t bestLen = 0;

    for (const StubEntry& entry : g_stubs) {
        size_t len = entry.suffix.size();
        if (len <= bestLen) {
            continue;
        }
        if (_wcsnicmp(text + 2, entry.suffix.c_str(), len) == 0) {
            best = &entry;
            bestLen = len;
        }
    }

    if (!best || !ReadOriginalPath(best->dir + L"$I" + best->suffix, path)) {
        return false;
    }

    *matched = 2 + bestLen;
    return true;
}

// If what has been copied out so far ends in "<drive>:\$Recycle.Bin\<SID>\",
// move that part into `prefix`: the stub's whole path is about to be replaced
// by the original one.
static bool TrimRecycleBinPrefix(std::wstring* out, std::wstring* prefix) {
    static const WCHAR kBin[] = L"$Recycle.Bin\\";
    const size_t kBinLen = ARRAYSIZE(kBin) - 1;

    if (out->size() <= kBinLen || out->back() != L'\\') {
        return false;
    }

    const WCHAR* begin = out->c_str();
    const WCHAR* at = StrRStrIW(begin, NULL, kBin);
    if (!at) {
        return false;
    }

    // Exactly one path segment - the user's SID folder - may follow.
    const WCHAR* segment = at + kBinLen;
    const WCHAR* slash = wcschr(segment, L'\\');
    if (!slash || slash == segment || slash != begin + out->size() - 1) {
        return false;
    }

    size_t start = (size_t)(at - begin);
    if (start >= 3 && (*out)[start - 1] == L'\\' && (*out)[start - 2] == L':') {
        start -= 3;
    } else if (start >= 1 && (*out)[start - 1] == L'\\') {
        start -= 1;
    }

    prefix->assign(*out, start, std::wstring::npos);
    out->erase(start);
    return true;
}

// Length of the file name that starts at `p`: up to the first character that
// cannot be part of one, a closing quotation mark or the end of the line.
static size_t FileNameLength(const WCHAR* p) {
    size_t n = 0;
    while (p[n] && !wcschr(L"\\/:*?\"<>|\r\n\x201D\x00BB", p[n])) {
        n++;
    }
    return n;
}

static bool TransformText(const WCHAR* text, std::wstring* out) {
    if (!text || !StrStrIW(text, L"$R")) {
        return false;
    }

    std::lock_guard<std::mutex> guard(g_indexMutex);

    bool listed = false;
    bool changed = false;
    out->clear();

    for (const WCHAR* p = text; *p;) {
        if (p[0] != L'$' || (p[1] != L'R' && p[1] != L'r')) {
            out->push_back(*p++);
            continue;
        }

        size_t matched = 0;
        std::wstring original;
        std::wstring prefix;
        bool wasPath = TrimRecycleBinPrefix(out, &prefix);
        bool found = false;

        if (wasPath) {
            // A full path names its own $I file; no listing is needed.
            size_t nameLen = FileNameLength(p);
            found = OriginalPathForStubPath(
                (prefix + std::wstring(p, nameLen)).c_str(), &original);
            matched = nameLen;
        } else {
            if (!listed) {
                RefreshStubIndex();
                listed = true;
            }
            found = MatchStub(p, &matched, &original);
        }

        if (!found) {
            out->append(prefix);
            out->push_back(*p++);
            continue;
        }

        // Text that named a path keeps naming one, whatever the setting says.
        out->append((g_fullPath || wasPath)
                        ? original.c_str()
                        : PathFindFileNameW(original.c_str()));
        changed = true;
        p += matched;
    }

    return changed;
}

// Only labels are rewritten. The text of an edit box or a combo box is data the
// user may act on, not a caption, so it is left exactly as it was.
static bool IsLabel(HWND hWnd) {
    WCHAR className[32];
    if (!hWnd || !GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return _wcsicmp(className, L"Static") == 0 ||
           _wcsicmp(className, L"#32770") == 0;
}

// The delete prompt asks for the display name of the stub's path, and for a
// plain file path that is just the stub's own name. This answers with the
// name recorded in the stub's $I file instead.
using SHGetFileInfoW_t = decltype(&SHGetFileInfoW);
SHGetFileInfoW_t SHGetFileInfoW_Original;

DWORD_PTR WINAPI SHGetFileInfoW_Hook(LPCWSTR pszPath, DWORD dwFileAttributes,
                                     SHFILEINFOW* psfi, UINT cbFileInfo,
                                     UINT uFlags) {
    DWORD_PTR ret = SHGetFileInfoW_Original(pszPath, dwFileAttributes, psfi,
                                            cbFileInfo, uFlags);
    if (!ret || !psfi || !(uFlags & SHGFI_DISPLAYNAME) || (uFlags & SHGFI_PIDL)) {
        return ret;
    }

    std::wstring original;
    if (OriginalPathForStubPath(pszPath, &original)) {
        LPCWSTR name =
            g_fullPath ? original.c_str() : PathFindFileNameW(original.c_str());
        Wh_Log(L"display name: [%s] -> [%s]", pszPath, name);
        wcsncpy_s(psfi->szDisplayName, name, _TRUNCATE);
    }
    return ret;
}

using SetDlgItemTextW_t = decltype(&SetDlgItemTextW);
SetDlgItemTextW_t SetDlgItemTextW_Original;

BOOL WINAPI SetDlgItemTextW_Hook(HWND hDlg, int nIDDlgItem, LPCWSTR lpString) {
    std::wstring fixed;
    if (IsLabel(GetDlgItem(hDlg, nIDDlgItem)) &&
        TransformText(lpString, &fixed)) {
        Wh_Log(L"control %d: [%s] -> [%s]", nIDDlgItem, lpString,
               fixed.c_str());
        return SetDlgItemTextW_Original(hDlg, nIDDlgItem, fixed.c_str());
    }

    return SetDlgItemTextW_Original(hDlg, nIDDlgItem, lpString);
}

using SetWindowTextW_t = decltype(&SetWindowTextW);
SetWindowTextW_t SetWindowTextW_Original;

BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    std::wstring fixed;
    if (IsLabel(hWnd) && TransformText(lpString, &fixed)) {
        Wh_Log(L"window: [%s] -> [%s]", lpString, fixed.c_str());
        return SetWindowTextW_Original(hWnd, fixed.c_str());
    }

    return SetWindowTextW_Original(hWnd, lpString);
}

static void LoadSettings() {
    g_fullPath = Wh_GetIntSetting(L"fullPath") != 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    WindhawkUtils::SetFunctionHook(SHGetFileInfoW, SHGetFileInfoW_Hook,
                                   &SHGetFileInfoW_Original);
    WindhawkUtils::SetFunctionHook(SetDlgItemTextW, SetDlgItemTextW_Hook,
                                   &SetDlgItemTextW_Original);
    WindhawkUtils::SetFunctionHook(SetWindowTextW, SetWindowTextW_Hook,
                                   &SetWindowTextW_Original);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> guard(g_indexMutex);
    g_stubs.clear();
    g_stubsBuilt = false;
}
