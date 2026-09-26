// ==WindhawkMod==
// @id              recycle-bin-original-names
// @name            Real Names in Recycle Bin Prompts
// @description     Shows the file's real name in the prompts for deleting it from the Recycle Bin, instead of the internal $R name the Recycle Bin keeps it under
// @name:ru         Настоящие имена файлов в запросах Корзины
// @description:ru  Показывает в запросах на удаление из Корзины настоящее имя файла вместо служебного $R-имени, под которым Корзина его хранит
// @version         1.0
// @author          appEW
// @github          https://github.com/appEW
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshlwapi
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

This mod reads the `$I` file and puts the real name back. Any `$R` stub name on
its way into a dialog is looked up across the `$Recycle.Bin` folders of the
local drives; if a matching `$I` file is there, the name it records is
substituted. Text with no `$R` in it is passed straight through, and a `$R` that
no `$I` file accounts for is left exactly as it was - a file of your own called
`$Report.xlsx` is never touched.

Where the text carries a full `C:\$Recycle.Bin\<SID>\$R...` path, the whole path
is replaced by the original one, so the prompt names the place the file actually
came from.

## What it does not do

Nothing about the Recycle Bin's contents, its folder view, or the files
themselves. This only rewrites what a dialog is about to display.

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
возвращает настоящее имя: любое `$R`-имя, которое попадает в текст окна,
ищется в папках `$Recycle.Bin` локальных дисков, и если для него есть файл
`$I`, подставляется записанное там имя. Если в тексте указан полный путь
`C:\$Recycle.Bin\<SID>\$R...`, он заменяется исходным путём, так что в запросе
видно, откуда файл был удалён. Текст без `$R` проходит без изменений, а `$R`,
для которого нет файла `$I`, остаётся как есть, - ваш собственный файл
`$Report.xlsx` мод не тронет.

Мод ничего не меняет в содержимом Корзины, в её окне и в самих файлах - только
текст, который окно собирается показать.

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
- log: false
  $name: Write to the Windhawk log
  $name:ru: Писать в журнал Windhawk
  $description: >-
    Records every name that was substituted. Only needed when looking into
    a problem.
  $description:ru: >-
    Записывает каждую подставленную замену. Нужно только для разбора неполадок.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>

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
static ULONGLONG g_stubsTick = 0;

static bool g_fullPath = false;
static bool g_log = false;

// Rebuilt rather than watched: a prompt appears rarely enough that listing the
// Recycle Bin folders on the spot is cheaper than keeping a live view of them.
static void BuildStubIndex() {
    g_stubs.clear();

    DWORD driveMask = GetLogicalDrives();
    for (int i = 0; i < 26; i++) {
        if (!(driveMask & (1u << i))) {
            continue;
        }

        WCHAR root[4] = {(WCHAR)(L'A' + i), L':', L'\\', L'\0'};
        UINT driveType = GetDriveTypeW(root);
        if (driveType != DRIVE_FIXED && driveType != DRIVE_REMOVABLE) {
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

    g_stubsTick = GetTickCount64();
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
// drop it: the stub's whole path is about to be replaced by the original one.
static bool TrimRecycleBinPrefix(std::wstring* out) {
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

    out->erase(start);
    return true;
}

static bool TransformText(const WCHAR* text, std::wstring* out) {
    if (!text || !wcsstr(text, L"$R")) {
        return false;
    }

    std::lock_guard<std::mutex> guard(g_indexMutex);

    bool rebuilt = false;
    if (g_stubs.empty() || GetTickCount64() - g_stubsTick > 1000) {
        BuildStubIndex();
        rebuilt = true;
    }

    bool changed = false;
    out->clear();

    for (const WCHAR* p = text; *p;) {
        if (p[0] != L'$' || (p[1] != L'R' && p[1] != L'r')) {
            out->push_back(*p++);
            continue;
        }

        size_t matched = 0;
        std::wstring original;
        bool found = MatchStub(p, &matched, &original);
        if (!found && !rebuilt) {
            // The item may have reached the Recycle Bin since the last listing.
            BuildStubIndex();
            rebuilt = true;
            found = MatchStub(p, &matched, &original);
        }

        if (!found) {
            out->push_back(*p++);
            continue;
        }

        // Text that named a path keeps naming one, whatever the setting says.
        bool wasPath = TrimRecycleBinPrefix(out);
        out->append((g_fullPath || wasPath)
                        ? original.c_str()
                        : PathFindFileNameW(original.c_str()));
        changed = true;
        p += matched;
    }

    return changed;
}

using SetDlgItemTextW_t = decltype(&SetDlgItemTextW);
SetDlgItemTextW_t SetDlgItemTextW_Original;

BOOL WINAPI SetDlgItemTextW_Hook(HWND hDlg, int nIDDlgItem, LPCWSTR lpString) {
    std::wstring fixed;
    if (TransformText(lpString, &fixed)) {
        if (g_log) {
            Wh_Log(L"control %d: [%s] -> [%s]", nIDDlgItem, lpString,
                   fixed.c_str());
        }
        return SetDlgItemTextW_Original(hDlg, nIDDlgItem, fixed.c_str());
    }

    return SetDlgItemTextW_Original(hDlg, nIDDlgItem, lpString);
}

using SetWindowTextW_t = decltype(&SetWindowTextW);
SetWindowTextW_t SetWindowTextW_Original;

BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    std::wstring fixed;
    if (TransformText(lpString, &fixed)) {
        if (g_log) {
            Wh_Log(L"window: [%s] -> [%s]", lpString, fixed.c_str());
        }
        return SetWindowTextW_Original(hWnd, fixed.c_str());
    }

    return SetWindowTextW_Original(hWnd, lpString);
}

static void LoadSettings() {
    g_fullPath = Wh_GetIntSetting(L"fullPath") != 0;
    g_log = Wh_GetIntSetting(L"log") != 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    Wh_SetFunctionHook((void*)SetDlgItemTextW, (void*)SetDlgItemTextW_Hook,
                       (void**)&SetDlgItemTextW_Original);
    Wh_SetFunctionHook((void*)SetWindowTextW, (void*)SetWindowTextW_Hook,
                       (void**)&SetWindowTextW_Original);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> guard(g_indexMutex);
    g_stubs.clear();
}
