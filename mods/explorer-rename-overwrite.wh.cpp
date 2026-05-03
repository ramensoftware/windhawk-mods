// ==WindhawkMod==
// @id              explorer-rename-overwrite
// @name            Explorer Rename Overwrite
// @description     When renaming a file in Explorer to a name that already exists, overwrite the existing file instead of being blocked by the "file already exists" prompt.
// @version         1.1.2
// @author          tria
// @github          https://github.com/triatomic
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Rename Overwrite

Changes Explorer's rename behavior so that renaming a file to a name that
already exists overwrites the existing file, instead of producing the
"file already exists" prompt or appending " (2)".

Works by hooking `IFileOperation::RenameItem` / `IFileOperation::RenameItems`.
When a rename target collides with an existing file, the existing file is
removed first (to the Recycle Bin by default, or permanently if configured).

## Settings

- **disposition**: Where the overwritten file goes — `recycle` (safer, default),
  `keep` (renamed in place, see *keepInPlaceSuffix*), or `permanent`
  (irreversible).
- **onlyFiles**: If enabled, never overwrite directories, only regular files.
- **renameBeforeRecycle**: When recycling, append a suffix to the overwritten
  file's name before sending it to the Recycle Bin. This guarantees that
  restoring the file from the bin will not clobber the new file you just
  renamed into its place. No effect when *disposition* is `permanent`.
- **recycleSuffix**: The suffix to insert before the extension
  (`report.txt` → `report_old.txt`). If a file with the suffixed name
  already exists, a number is appended (`report_old (2).txt`), matching
  Explorer's own collision behavior. Defaults to `_old`.
- **keepInPlaceSuffix**: Used when *disposition* is `keep`. The suffix is
  appended to the **whole filename including its extension**, so
  `report.txt` becomes `report.txt_old`. This intentionally breaks the
  file association so the old file is visually distinct and won't be
  double-clicked into its old app. Collisions get the same `(2)`, `(3)`
  treatment as the recycle path. Defaults to `_old`.

## Changelog

### 1.1.2
- Remove the `enabled` master switch — disable the mod via Windhawk's own
  toggle instead.
- Preserve the existing target's filename casing when renaming it with the
  recycle/keep suffix (e.g. renaming `Src.txt` over `DEST.txt` produces
  `DEST.txt_old`, not `Dest.txt_old`).
- Don't pass `MOVEFILE_REPLACE_EXISTING` when moving the collision target
  to its suffixed name — the suffixed path is chosen to not exist, and
  silently overwriting would defeat the safety of the suffix.

### 1.1.1
- Fix `keep` disposition: notify the shell of the in-place rename so
  Explorer's view doesn't transiently show both files with the suffix
  until the window is refreshed.

### 1.1.0
- New `keep` disposition: rename the overwritten file in place (default
  `<name>_old`) instead of deleting/recycling it.
- New `keepInPlaceSuffix` setting controlling the in-place suffix.

### 1.0.0
- Initial release: hook `IFileOperation::RenameItem` / `RenameItems` and
  clear the destination on collision.
- Settings: `disposition` (`recycle` / `permanent`), `onlyFiles`,
  `renameBeforeRecycle`, `recycleSuffix`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- disposition: recycle
  $name: Overwritten file disposition
  $description: What to do with the file being overwritten.
  $options:
  - recycle: Move to Recycle Bin
  - keep: Keep in place, renamed with suffix
  - permanent: Delete permanently
- onlyFiles: true
  $name: Only overwrite files
  $description: If enabled, never overwrite a directory — only regular files.
- renameBeforeRecycle: true
  $name: Append suffix before recycling
  $description: When sending the overwritten file to the Recycle Bin, append a suffix to its name first so that restoring it from the bin will not clobber the new file. Has no effect when disposition is "permanent".
- recycleSuffix: "_old"
  $name: Recycle suffix
  $description: Suffix to append to the filename's stem before it is sent to the Recycle Bin. Inserted before the extension (e.g. "report.txt" → "report_old.txt"). If a name with the suffix already exists, a number is appended ("report_old (2).txt").
- keepInPlaceSuffix: "_old"
  $name: Keep-in-place suffix
  $description: Used when disposition is "keep". Appended to the whole filename, extension included (e.g. "report.txt" → "report.txt_old"). Breaks the file association so the old file is visually distinct. If the target name already exists, a number is appended.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <objbase.h>
#include <string>

namespace {

enum class Disposition { Recycle, Keep, Permanent };

struct Settings {
    Disposition disposition = Disposition::Recycle;
    bool onlyFiles = true;
    bool renameBeforeRecycle = true;
    std::wstring recycleSuffix = L"_old";
    std::wstring keepInPlaceSuffix = L"_old";
};

Settings g_settings;

void LoadSettings() {
    PCWSTR disp = Wh_GetStringSetting(L"disposition");
    if (disp && wcscmp(disp, L"permanent") == 0) {
        g_settings.disposition = Disposition::Permanent;
    } else if (disp && wcscmp(disp, L"keep") == 0) {
        g_settings.disposition = Disposition::Keep;
    } else {
        g_settings.disposition = Disposition::Recycle;
    }
    Wh_FreeStringSetting(disp);
    g_settings.onlyFiles = Wh_GetIntSetting(L"onlyFiles") != 0;
    g_settings.renameBeforeRecycle =
        Wh_GetIntSetting(L"renameBeforeRecycle") != 0;

    PCWSTR suffix = Wh_GetStringSetting(L"recycleSuffix");
    g_settings.recycleSuffix = (suffix && *suffix) ? suffix : L"_old";
    Wh_FreeStringSetting(suffix);

    PCWSTR keepSuffix = Wh_GetStringSetting(L"keepInPlaceSuffix");
    g_settings.keepInPlaceSuffix = (keepSuffix && *keepSuffix) ? keepSuffix : L"_old";
    Wh_FreeStringSetting(keepSuffix);
}

// Get the absolute filesystem path from an IShellItem.
bool GetItemPath(IShellItem* item, std::wstring& out) {
    if (!item) return false;
    PWSTR p = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &p)) || !p) return false;
    out.assign(p);
    CoTaskMemFree(p);
    return true;
}

// Compute the destination path that RenameItem will produce.
bool ComputeRenameTarget(IShellItem* source, PCWSTR newName, std::wstring& out) {
    std::wstring src;
    if (!GetItemPath(source, src)) return false;
    size_t slash = src.find_last_of(L"\\/");
    if (slash == std::wstring::npos) return false;
    out.assign(src, 0, slash + 1);
    out.append(newName);
    return true;
}

// True if `target` refers to the same file as `source` (case-insensitive,
// long-path normalized). We must not delete the source when only the case
// is changing.
bool SamePath(const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

// Split a full path into directory (with trailing slash), stem, and extension.
// "C:\a\b\report.tar.gz" → dir="C:\a\b\", stem="report.tar", ext=".gz"
// Files starting with '.' (e.g. ".gitignore") are treated as all-stem.
void SplitPath(const std::wstring& path, std::wstring& dir,
               std::wstring& stem, std::wstring& ext) {
    size_t slash = path.find_last_of(L"\\/");
    std::wstring name;
    if (slash == std::wstring::npos) {
        dir.clear();
        name = path;
    } else {
        dir.assign(path, 0, slash + 1);
        name.assign(path, slash + 1, std::wstring::npos);
    }
    size_t dot = name.find_last_of(L'.');
    if (dot == std::wstring::npos || dot == 0) {
        stem = name;
        ext.clear();
    } else {
        stem.assign(name, 0, dot);
        ext.assign(name, dot, std::wstring::npos);
    }
}

// Resolve `path` to the on-disk casing of its final component. Earlier
// directory components keep whatever casing the caller supplied — only the
// leaf name is corrected, which is enough for the suffix-rename to preserve
// the existing file's name (e.g. when renaming "Src.txt" over an existing
// "DEST.txt", the suffixed name should be "DEST.txt_old", not
// "Dest.txt_old").
std::wstring ResolveLeafCase(const std::wstring& path) {
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(path.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return path;
    FindClose(h);
    size_t slash = path.find_last_of(L"\\/");
    if (slash == std::wstring::npos) return fd.cFileName;
    std::wstring out(path, 0, slash + 1);
    out.append(fd.cFileName);
    return out;
}

// Pick a path of the form "<dir><stem><suffix><ext>" that does not exist.
// If it already exists, try " (2)", " (3)", … like Explorer does.
std::wstring PickSuffixedPath(const std::wstring& original) {
    std::wstring dir, stem, ext;
    SplitPath(original, dir, stem, ext);
    std::wstring base = dir + stem + g_settings.recycleSuffix;
    std::wstring candidate = base + ext;
    if (GetFileAttributesW(candidate.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return candidate;
    }
    for (int i = 2; i < 1000; ++i) {
        wchar_t num[32];
        swprintf_s(num, L" (%d)", i);
        candidate = base + num + ext;
        if (GetFileAttributesW(candidate.c_str()) == INVALID_FILE_ATTRIBUTES) {
            return candidate;
        }
    }
    return std::wstring();  // give up
}

// Pick a path of the form "<original><suffix>" (suffix appended after the
// extension) that does not exist. Used by the "keep in place" disposition.
std::wstring PickAppendedPath(const std::wstring& original) {
    std::wstring base = original + g_settings.keepInPlaceSuffix;
    if (GetFileAttributesW(base.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return base;
    }
    for (int i = 2; i < 1000; ++i) {
        wchar_t num[32];
        swprintf_s(num, L" (%d)", i);
        std::wstring candidate = base + num;
        if (GetFileAttributesW(candidate.c_str()) == INVALID_FILE_ATTRIBUTES) {
            return candidate;
        }
    }
    return std::wstring();
}

bool KeepInPlace(const std::wstring& path) {
    if (g_settings.keepInPlaceSuffix.empty()) return false;
    std::wstring dst = PickAppendedPath(path);
    if (dst.empty()) return false;
    if (MoveFileExW(path.c_str(), dst.c_str(), 0) == 0) return false;
    // Tell the shell about the rename so Explorer's view doesn't end up with
    // a stale entry pointing at the old name (which makes the new file
    // appear with the "_old" suffix until the user refreshes).
    SHChangeNotify(SHCNE_RENAMEITEM, SHCNF_PATHW, path.c_str(), dst.c_str());
    return true;
}

bool DeleteToRecycleBin(const std::wstring& path) {
    // SHFileOperationW requires a double-null-terminated buffer.
    std::wstring buf = path;
    buf.push_back(L'\0');
    buf.push_back(L'\0');

    SHFILEOPSTRUCTW op = {};
    op.wFunc = FO_DELETE;
    op.pFrom = buf.c_str();
    op.fFlags = FOF_ALLOWUNDO | FOF_NO_UI | FOF_NOCONFIRMATION |
                FOF_NOERRORUI | FOF_SILENT;
    return SHFileOperationW(&op) == 0 && !op.fAnyOperationsAborted;
}

bool DeletePermanently(const std::wstring& path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) return true;  // already gone
    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        // RemoveDirectoryW only succeeds on empty directories. Use the shell
        // file op for recursive deletion, just without FOF_ALLOWUNDO so it
        // bypasses the Recycle Bin.
        std::wstring buf = path;
        buf.push_back(L'\0');
        buf.push_back(L'\0');
        SHFILEOPSTRUCTW op = {};
        op.wFunc = FO_DELETE;
        op.pFrom = buf.c_str();
        op.fFlags = FOF_NO_UI | FOF_NOCONFIRMATION |
                    FOF_NOERRORUI | FOF_SILENT;
        return SHFileOperationW(&op) == 0 && !op.fAnyOperationsAborted;
    }
    if (attr & FILE_ATTRIBUTE_READONLY) {
        SetFileAttributesW(path.c_str(), attr & ~FILE_ATTRIBUTE_READONLY);
    }
    return DeleteFileW(path.c_str()) != 0;
}

// Remove the existing file at `target` so a subsequent rename will succeed.
// Returns true if the path is now clear (or never existed).
bool ClearTarget(const std::wstring& target) {
    DWORD attr = GetFileAttributesW(target.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) return true;  // nothing there

    if ((attr & FILE_ATTRIBUTE_DIRECTORY) && g_settings.onlyFiles) {
        Wh_Log(L"Target is a directory and onlyFiles=true; skipping: %s",
               target.c_str());
        return false;
    }

    if (g_settings.disposition == Disposition::Permanent) {
        return DeletePermanently(target);
    }

    if (g_settings.disposition == Disposition::Keep) {
        return KeepInPlace(target);
    }

    std::wstring toRecycle = target;
    if (g_settings.renameBeforeRecycle &&
        !g_settings.recycleSuffix.empty() &&
        !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
        std::wstring suffixed = PickSuffixedPath(target);
        // No MOVEFILE_REPLACE_EXISTING: PickSuffixedPath already returned a
        // path that does not exist, and silently overwriting on a TOCTOU race
        // would defeat the safety the suffix is meant to provide.
        if (!suffixed.empty() &&
            MoveFileExW(target.c_str(), suffixed.c_str(), 0) != 0) {
            toRecycle = suffixed;
        } else {
            Wh_Log(L"Suffix rename failed, recycling original name");
        }
    }
    return DeleteToRecycleBin(toRecycle);
}

// Pre-rename hook body shared by RenameItem / RenameItems.
void HandleRename(IShellItem* source, PCWSTR newName) {
    if (!source || !newName || !*newName) return;
    std::wstring target;
    if (!ComputeRenameTarget(source, newName, target)) return;

    std::wstring src;
    if (GetItemPath(source, src) && SamePath(src, target)) {
        // Same path (case-only change or identity) — leave it to Explorer.
        return;
    }

    DWORD attr = GetFileAttributesW(target.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) return;  // no collision

    // The existing file may have different casing than `newName`. Use the
    // on-disk name so the suffixed rename preserves it.
    target = ResolveLeafCase(target);

    Wh_Log(L"Rename collision, clearing target: %s", target.c_str());
    ClearTarget(target);
}

// --- Hooks ----------------------------------------------------------------

using RenameItem_t = HRESULT(STDMETHODCALLTYPE*)(IFileOperation*, IShellItem*,
                                                 LPCWSTR, IFileOperationProgressSink*);
RenameItem_t RenameItem_Original;

HRESULT STDMETHODCALLTYPE RenameItem_Hook(IFileOperation* self,
                                          IShellItem* item,
                                          LPCWSTR newName,
                                          IFileOperationProgressSink* sink) {
    HandleRename(item, newName);
    return RenameItem_Original(self, item, newName, sink);
}

using RenameItems_t = HRESULT(STDMETHODCALLTYPE*)(IFileOperation*, IUnknown*,
                                                  LPCWSTR);
RenameItems_t RenameItems_Original;

HRESULT STDMETHODCALLTYPE RenameItems_Hook(IFileOperation* self,
                                           IUnknown* items,
                                           LPCWSTR newName) {
    // RenameItems applies the same name to every item, so collisions only
    // really make sense for a single item — but we still process whatever
    // we can enumerate.
    if (items && newName && *newName) {
        IShellItemArray* arr = nullptr;
        if (SUCCEEDED(items->QueryInterface(IID_PPV_ARGS(&arr))) && arr) {
            DWORD count = 0;
            arr->GetCount(&count);
            for (DWORD i = 0; i < count; ++i) {
                IShellItem* it = nullptr;
                if (SUCCEEDED(arr->GetItemAt(i, &it)) && it) {
                    HandleRename(it, newName);
                    it->Release();
                }
            }
            arr->Release();
        }
    }
    return RenameItems_Original(self, items, newName);
}

// Resolve the IFileOperation vtable slots by creating a transient instance.
bool InstallVtableHooks() {
    IFileOperation* op = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOperation, nullptr,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&op));
    if (FAILED(hr) || !op) {
        Wh_Log(L"CoCreateInstance(FileOperation) failed: 0x%08X", hr);
        return false;
    }

    void** vtbl = *reinterpret_cast<void***>(op);
    // IFileOperation vtable layout (after IUnknown's 3 slots):
    //   3:  Advise               4:  Unadvise
    //   5:  SetOperationFlags    6:  SetProgressMessage
    //   7:  SetProgressDialog    8:  SetProperties
    //   9:  SetOwnerWindow       10: ApplyPropertiesToItem
    //   11: ApplyPropertiesToItems
    //   12: RenameItem           13: RenameItems
    //   14: MoveItem             ...
    void* renameItem  = vtbl[12];
    void* renameItems = vtbl[13];
    op->Release();

    bool ok = true;
    if (!Wh_SetFunctionHook(renameItem,
                            (void*)RenameItem_Hook,
                            (void**)&RenameItem_Original)) {
        Wh_Log(L"Failed to hook RenameItem");
        ok = false;
    }
    if (!Wh_SetFunctionHook(renameItems,
                            (void*)RenameItems_Hook,
                            (void**)&RenameItems_Original)) {
        Wh_Log(L"Failed to hook RenameItems");
        ok = false;
    }
    return ok;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED |
                                             COINIT_DISABLE_OLE1DDE);
    bool needUninit = SUCCEEDED(hr);
    bool ok = InstallVtableHooks();
    if (needUninit) CoUninitialize();
    return ok ? TRUE : FALSE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
