// ==WindhawkMod==
// @id              recycle-bin-original-names
// @name            Real Names in Pre-Vista Recycle Bin Prompts
// @description     Companion to Pre-Vista File Operation Dialogs: its XP-style prompts for deleting from the Recycle Bin show the file's real name instead of the internal $R name the Recycle Bin keeps it under
// @name:ru         Настоящие имена в запросах Корзины в стиле XP
// @description:ru  Дополнение к Pre-Vista File Operation Dialogs: его запросы в стиле XP на удаление из Корзины показывают настоящее имя файла вместо служебного $R-имени, под которым Корзина его хранит
// @version         1.3
// @author          appEW
// @github          https://github.com/appEW
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshlwapi -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Real Names in Pre-Vista Recycle Bin Prompts

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

Delete something out of the Recycle Bin with the XP-style prompts of
[Pre-Vista File Operation Dialogs](https://windhawk.net/mods/prevista-file-copy)
enabled, and the confirmation asks about a file you never had:

> Are you sure you want to delete "$RVAZ2WZ.b"?

This mod puts the real name back:

![Before: the prompt names the $R stub](https://raw.githubusercontent.com/appEW/images/main/recycle-bin-original-names/before.png)

![After: the prompt names the file that was deleted](https://raw.githubusercontent.com/appEW/images/main/recycle-bin-original-names/after.png)

The stock Windows 11 prompt already shows the right name; the mod is for prompts
that look the name up the way Pre-Vista File Operation Dialogs does.

## Why the name is wrong

The Recycle Bin does not keep a deleted file under its own name. It renames it
to `$R` plus a few random characters, keeping only the extension, and writes the
real path into a companion `$I` file next to it. Everything that asks the
Recycle Bin folder for a display name gets the original name back; anything that
only looks at the file on disk sees the stub.

The XP-style prompt looks at the file on disk: it asks `SHGetFileInfo` for the
display name of `C:\$Recycle.Bin\<SID>\$RVAZ2WZ.b`, and for a plain file path
that is just the file name - the stub.

## What the mod does

When `SHGetFileInfo` is asked for the display name of a `$R` file inside a
`$Recycle.Bin` folder, the mod answers with the name recorded in the `$I` file
next to it - one small file read, no searching. With *Show the full original
path* on, it answers with the whole path the file was deleted from.

Anything else is passed straight through, and a `$R` file with no `$I` file next
to it keeps its own name.

Nothing about the Recycle Bin's contents, its folder view, or the files
themselves changes. This only changes the name a prompt is about to display.

---

## По-русски

Если включён мод
[Pre-Vista File Operation Dialogs](https://windhawk.net/mods/prevista-file-copy)
с запросами в стиле XP, то при удалении чего-нибудь из Корзины подтверждение
спрашивает об удалении файла, которого у вас никогда не было, - например,
«$RVAZ2WZ.b». Мод возвращает в такой запрос настоящее имя (скриншоты - выше, в
английской части). Стандартный запрос Windows 11 и так показывает правильное
имя; мод нужен для запросов, которые узнают имя так же, как Pre-Vista File
Operation Dialogs.

Корзина не хранит удалённый файл под его настоящим именем. Она переименовывает
его в `$R` и несколько случайных символов, сохраняя только расширение, а
настоящий путь записывает в парный файл `$I` рядом. Всё, что спрашивает имя у
папки Корзины, получает настоящее имя; всё, что смотрит на сам файл на диске,
видит служебное.

Запрос в стиле XP смотрит на файл на диске: он спрашивает у `SHGetFileInfo`
отображаемое имя пути `C:\$Recycle.Bin\<SID>\$RVAZ2WZ.b`, а для обычного пути к
файлу это просто имя файла, то есть служебное.

Когда у `SHGetFileInfo` спрашивают отображаемое имя `$R`-файла в папке
`$Recycle.Bin`, мод отвечает именем, записанным в соседнем файле `$I`, - это
одно чтение маленького файла, без всякого поиска. Если включено «Показывать
полный исходный путь», ответом будет весь путь, откуда файл был удалён.
Всё остальное проходит без изменений, а `$R`-файл без соседнего `$I` остаётся
со своим именем.

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
#include <string>
#include <vector>

static std::atomic<bool> g_fullPath{false};

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

// A prompt that asks for the display name of the stub's path gets the stub's
// own name back, since for a plain file path that is all there is. This
// answers with the name recorded in the stub's $I file instead.
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

static void LoadSettings() {
    g_fullPath = Wh_GetIntSetting(L"fullPath") != 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    WindhawkUtils::SetFunctionHook(SHGetFileInfoW, SHGetFileInfoW_Hook,
                                   &SHGetFileInfoW_Original);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
