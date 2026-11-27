// ==WindhawkMod==
// @id              modern-folder-picker-everywhere
// @name            Modern Folder Picker Everywhere
// @description     Replaces the classic "Browse For Folder" dialog with the modern picker, including in NSIS and Inno Setup installers
// @version         1.0.0
// @author          SteelArchadian
// @github          https://github.com/SteelArchadian
// @include         *
// @compilerOptions -lole32 -loleaut32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Modern Folder Picker Everywhere

Replaces the outdated "Browse For Folder" dialog with the modern folder picker,
and keeps installers working while doing it.

Based on [Modernize Folder Picker Dialog](https://windhawk.net/mods/modernize-folder-picker-dialog)
by aubymori. That mod shows the modern picker but never calls the program's
BrowseCallbackProc, so installers that read the chosen folder from their
callback (NSIS among them) ignore the folder you pick. This mod drives the
callback the way the classic dialog does:

- BFFM_INITIALIZED when the dialog opens, BFFM_SELCHANGED whenever the current
  folder or selection changes, and once more with the final choice on OK.
- Messages the callback sends back (BFFM_SETSELECTION, BFFM_SETOKTEXT,
  BFFM_ENABLEOK, BFFM_SETSTATUSTEXT, BFFM_SETEXPANDED) are translated or
  absorbed instead of reaching the modern dialog's own window procedure.
- pszDisplayName and iImage are filled in, the working directory is left alone
  (FOS_NOCHANGEDIR), and SHBrowseForFolderA is handled too.
- Inno Setup installers, which draw their own folder form instead of calling
  SHBrowseForFolder, get the modern picker too. The choice is written back into
  Inno's form, so Inno still appends the app folder name as usual.
- Modes the modern picker can't do (computers, printers, files) still use the
  classic dialog.

Not covered yet: MSI installers, whose Browse window is drawn by Windows
Installer from the package's own dialog tables.

Disable "Modernize Folder Picker Dialog" before enabling this mod; both hook
the same function.

Made with AI assistance.
*/
// ==/WindhawkModReadme==

#define E_CANCELLED HRESULT_FROM_WIN32(ERROR_CANCELLED)

#include <initguid.h>
#include <shlobj.h>
#include <commctrl.h>
#include <windhawk_utils.h>

using SHBrowseForFolderW_t = decltype(&SHBrowseForFolderW);
SHBrowseForFolderW_t SHBrowseForFolderW_orig = nullptr;
using SHBrowseForFolderA_t = decltype(&SHBrowseForFolderA);
SHBrowseForFolderA_t SHBrowseForFolderA_orig = nullptr;

/* Set while the classic dialog runs, in case the A export calls the W one */
thread_local bool g_inClassic = false;

/* Posted to the dialog to navigate outside of the callback that asked for it */
UINT g_navigateMsg = 0;

struct BrowseState
{
    LPBROWSEINFOW bi = nullptr;
    IFileDialog *dlg = nullptr;
    HWND hwnd = nullptr;
    bool initialized = false;
    int callbackDepth = 0;
    IShellItem *pendingFolder = nullptr;

    void Notify(UINT msg, LPARAM lParam)
    {
        if (!bi->lpfn) return;
        callbackDepth++;
        bi->lpfn(hwnd, msg, lParam, bi->lParam);
        callbackDepth--;
    }

    void NotifySelChanged(IShellItem *item)
    {
        if (!item) return;
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHGetIDListFromObject(item, &pidl)))
        {
            Notify(BFFM_SELCHANGED, (LPARAM)pidl);
            CoTaskMemFree(pidl);
        }
    }

    void NotifyCurrent()
    {
        IShellItem *item = nullptr;
        if (FAILED(dlg->GetCurrentSelection(&item)) || !item)
            dlg->GetFolder(&item);
        if (item)
        {
            NotifySelChanged(item);
            item->Release();
        }
    }
};

/* The nearest existing folder for a path, since installers often preselect a
   folder they haven't created yet */
IShellItem *ItemFromPath(LPCWSTR path)
{
    WCHAR buf[MAX_PATH * 4];
    lstrcpynW(buf, path, ARRAYSIZE(buf));
    while (buf[0])
    {
        IShellItem *item = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(buf, nullptr, IID_PPV_ARGS(&item))))
            return item;
        WCHAR *slash = wcsrchr(buf, L'\\');
        if (!slash) break;
        if (slash > buf && slash[-1] == L':')
        {
            /* Keep the root's backslash, then stop at the drive */
            if (slash[1] == L'\0') break;
            slash[1] = L'\0';
        }
        else
        {
            *slash = L'\0';
        }
    }
    return nullptr;
}

IShellItem *ItemFromSetSelection(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!lParam) return nullptr;
    if (!wParam)
    {
        IShellItem *item = nullptr;
        SHCreateItemFromIDList((PCIDLIST_ABSOLUTE)lParam, IID_PPV_ARGS(&item));
        return item;
    }
    if (msg == BFFM_SETSELECTIONW)
        return ItemFromPath((LPCWSTR)lParam);

    WCHAR wide[MAX_PATH * 4];
    if (!MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lParam, -1, wide, ARRAYSIZE(wide)))
        return nullptr;
    return ItemFromPath(wide);
}

LRESULT CALLBACK DialogSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData
)
{
    BrowseState *state = (BrowseState *)dwRefData;

    if (uMsg == g_navigateMsg && g_navigateMsg)
    {
        if (state->pendingFolder)
        {
            state->dlg->SetFolder(state->pendingFolder);
            state->pendingFolder->Release();
            state->pendingFolder = nullptr;
        }
        return 0;
    }

    /* Only BFFM_* messages the callback sends belong to us; the modern dialog
       may use the same WM_USER numbers for itself */
    if (state->callbackDepth > 0)
    {
        switch (uMsg)
        {
            case BFFM_SETSELECTIONA:
            case BFFM_SETSELECTIONW:
            {
                IShellItem *item = ItemFromSetSelection(uMsg, wParam, lParam);
                if (!item) return FALSE;
                if (state->pendingFolder) state->pendingFolder->Release();
                state->pendingFolder = item;
                PostMessageW(hWnd, g_navigateMsg, 0, 0);
                return TRUE;
            }
            case BFFM_SETOKTEXT:
                if (lParam) state->dlg->SetOkButtonLabel((LPCWSTR)lParam);
                return 0;
            case BFFM_ENABLEOK:
            case BFFM_SETSTATUSTEXTA:
            case BFFM_SETSTATUSTEXTW:
            case BFFM_SETEXPANDED:
                return 0;
        }
    }

    if (uMsg == WM_NCDESTROY)
        RemoveWindowSubclass(hWnd, DialogSubclassProc, uIdSubclass);

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

class DialogEvents : public IFileDialogEvents
{
public:
    explicit DialogEvents(BrowseState *state) : m_state(state) {}

    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv) override
    {
        if (!ppv) return E_POINTER;
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IFileDialogEvents))
        {
            *ppv = static_cast<IFileDialogEvents *>(this);
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    /* Lives on SHBrowseForFolder's stack and is unadvised before it returns */
    IFACEMETHODIMP_(ULONG) AddRef() override { return 2; }
    IFACEMETHODIMP_(ULONG) Release() override { return 1; }

    IFACEMETHODIMP OnFolderChange(IFileDialog *) override
    {
        if (!m_state->initialized)
        {
            m_state->initialized = true;
            IOleWindow *window = nullptr;
            if (SUCCEEDED(m_state->dlg->QueryInterface(IID_PPV_ARGS(&window))))
            {
                window->GetWindow(&m_state->hwnd);
                window->Release();
            }
            if (m_state->hwnd)
                SetWindowSubclass(m_state->hwnd, DialogSubclassProc, 0, (DWORD_PTR)m_state);
            m_state->Notify(BFFM_INITIALIZED, 0);
        }
        m_state->NotifyCurrent();
        return S_OK;
    }

    IFACEMETHODIMP OnSelectionChange(IFileDialog *) override
    {
        m_state->NotifyCurrent();
        return S_OK;
    }

    /* The final choice, sent while the dialog still exists, which is where
       callbacks such as NSIS's copy the path they later use */
    IFACEMETHODIMP OnFileOk(IFileDialog *) override
    {
        IShellItem *result = nullptr;
        if (SUCCEEDED(m_state->dlg->GetResult(&result)) && result)
        {
            m_state->NotifySelChanged(result);
            result->Release();
        }
        return S_OK;
    }

    IFACEMETHODIMP OnFolderChanging(IFileDialog *, IShellItem *) override { return S_OK; }
    IFACEMETHODIMP OnShareViolation(IFileDialog *, IShellItem *, FDE_SHAREVIOLATION_RESPONSE *) override { return E_NOTIMPL; }
    IFACEMETHODIMP OnTypeChange(IFileDialog *) override { return S_OK; }
    IFACEMETHODIMP OnOverwrite(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *) override { return E_NOTIMPL; }

private:
    BrowseState *m_state;
};

/* Returns false when the classic dialog should be used instead */
bool ModernBrowse(LPBROWSEINFOW lpbi, PIDLIST_ABSOLUTE *out)
{
    *out = nullptr;

    if (lpbi->ulFlags & (BIF_BROWSEFORCOMPUTER | BIF_BROWSEFORPRINTER |
                         BIF_BROWSEINCLUDEFILES | BIF_BROWSEFILEJUNCTIONS))
        return false;

    IFileOpenDialog *dlg = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg))))
        return false;

    BrowseState state;
    state.bi = lpbi;
    state.dlg = dlg;

    FILEOPENDIALOGOPTIONS options = 0;
    dlg->GetOptions(&options);
    options |= FOS_PICKFOLDERS | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST | FOS_NOCHANGEDIR;
    if (lpbi->ulFlags & BIF_RETURNONLYFSDIRS)
        options |= FOS_FORCEFILESYSTEM;

    bool ok = SUCCEEDED(dlg->SetOptions(options));

    if (ok && lpbi->lpszTitle && *lpbi->lpszTitle)
        dlg->SetTitle(lpbi->lpszTitle);

    if (ok && lpbi->pidlRoot)
    {
        IShellItem *root = nullptr;
        if (SUCCEEDED(SHCreateItemFromIDList(lpbi->pidlRoot, IID_PPV_ARGS(&root))))
        {
            dlg->SetFolder(root);
            root->Release();
        }
    }

    DialogEvents events(&state);
    DWORD cookie = 0;
    ok = ok && SUCCEEDED(dlg->Advise(&events, &cookie));
    if (!ok)
    {
        dlg->Release();
        return false;
    }

    HRESULT hr = dlg->Show(lpbi->hwndOwner);
    dlg->Unadvise(cookie);

    if (state.pendingFolder) state.pendingFolder->Release();

    if (FAILED(hr))
    {
        dlg->Release();
        /* Cancelled is a normal answer; anything else means the dialog never
           worked, so the classic one gets a chance */
        return hr == E_CANCELLED;
    }

    IShellItem *result = nullptr;
    if (SUCCEEDED(dlg->GetResult(&result)) && result)
    {
        SHGetIDListFromObject(result, out);

        if (lpbi->pszDisplayName)
        {
            lpbi->pszDisplayName[0] = L'\0';
            LPWSTR name = nullptr;
            if (SUCCEEDED(result->GetDisplayName(SIGDN_NORMALDISPLAY, &name)))
            {
                lstrcpynW(lpbi->pszDisplayName, name, MAX_PATH);
                CoTaskMemFree(name);
            }
        }
        result->Release();
    }

    if (*out)
    {
        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW((LPCWSTR)*out, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_SYSICONINDEX))
            lpbi->iImage = sfi.iIcon;
    }

    dlg->Release();
    return true;
}

PIDLIST_ABSOLUTE WINAPI SHBrowseForFolderW_hook(LPBROWSEINFOW lpbi)
{
    if (!lpbi || g_inClassic) return SHBrowseForFolderW_orig(lpbi);

    PIDLIST_ABSOLUTE result = nullptr;
    if (ModernBrowse(lpbi, &result))
        return result;

    g_inClassic = true;
    result = SHBrowseForFolderW_orig(lpbi);
    g_inClassic = false;
    return result;
}

PIDLIST_ABSOLUTE WINAPI SHBrowseForFolderA_hook(LPBROWSEINFOA lpbi)
{
    if (!lpbi || g_inClassic) return SHBrowseForFolderA_orig(lpbi);

    WCHAR title[1024] = L"";
    WCHAR name[MAX_PATH] = L"";

    BROWSEINFOW biw = {};
    biw.hwndOwner = lpbi->hwndOwner;
    biw.pidlRoot = lpbi->pidlRoot;
    biw.pszDisplayName = lpbi->pszDisplayName ? name : nullptr;
    if (lpbi->lpszTitle)
    {
        MultiByteToWideChar(CP_ACP, 0, lpbi->lpszTitle, -1, title, ARRAYSIZE(title));
        biw.lpszTitle = title;
    }
    biw.ulFlags = lpbi->ulFlags;
    biw.lpfn = lpbi->lpfn;
    biw.lParam = lpbi->lParam;
    biw.iImage = lpbi->iImage;

    PIDLIST_ABSOLUTE result = nullptr;
    if (ModernBrowse(&biw, &result))
    {
        if (result && lpbi->pszDisplayName)
            WideCharToMultiByte(CP_ACP, 0, name, -1, lpbi->pszDisplayName, MAX_PATH, nullptr, nullptr);
        lpbi->iImage = biw.iImage;
        return result;
    }

    g_inClassic = true;
    result = SHBrowseForFolderA_orig(lpbi);
    g_inClassic = false;
    return result;
}

/* ---- Inno Setup ----
   Inno's wizard doesn't call SHBrowseForFolder; its Browse button opens its own
   form (a TFolderTreeView, a path edit, OK/Cancel/New Folder). We let that form
   be created as usual, keep it invisible, show the modern picker in its place,
   then write the choice into its path edit and press its OK button, so Inno
   reads the folder exactly as if the user had used its form. */

UINT g_innoPickMsg = 0;

struct InnoChildren
{
    HWND tree = nullptr, edit = nullptr, ok = nullptr;
};

BOOL CALLBACK FindInnoChildren(HWND hwnd, LPARAM lParam)
{
    InnoChildren *c = (InnoChildren *)lParam;
    WCHAR cls[64];
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (!lstrcmpW(cls, L"TFolderTreeView"))
        c->tree = hwnd;
    else if (!c->edit && wcsstr(cls, L"Edit"))
        c->edit = hwnd;
    else if (wcsstr(cls, L"Button") && (GetWindowLongW(hwnd, GWL_STYLE) & BS_DEFPUSHBUTTON))
        c->ok = hwnd;
    return TRUE;
}

/* Picks a folder starting from the form's current path. Returns false when
   the user cancels. */
bool InnoPick(HWND owner, LPCWSTR current, WCHAR *out, int outLen)
{
    IFileOpenDialog *dlg = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg))))
        return false;

    FILEOPENDIALOGOPTIONS options = 0;
    dlg->GetOptions(&options);
    dlg->SetOptions(options | FOS_PICKFOLDERS | FOS_PATHMUSTEXIST |
                    FOS_FORCEFILESYSTEM | FOS_NOCHANGEDIR);

    WCHAR title[256];
    if (GetWindowTextW(owner, title, ARRAYSIZE(title)))
        dlg->SetTitle(title);

    if (IShellItem *start = ItemFromPath(current))
    {
        dlg->SetFolder(start);
        start->Release();
    }

    bool picked = false;
    if (SUCCEEDED(dlg->Show(owner)))
    {
        IShellItem *result = nullptr;
        if (SUCCEEDED(dlg->GetResult(&result)) && result)
        {
            LPWSTR path = nullptr;
            if (SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &path)))
            {
                lstrcpynW(out, path, outLen);
                CoTaskMemFree(path);
                picked = true;
            }
            result->Release();
        }
    }
    dlg->Release();
    return picked;
}

void InnoReplaceForm(HWND form, const InnoChildren &c)
{
    WCHAR current[MAX_PATH * 4] = L"";
    GetWindowTextW(c.edit, current, ARRAYSIZE(current));

    WCHAR picked[MAX_PATH * 4];
    if (!InnoPick(form, current, picked, ARRAYSIZE(picked)))
    {
        /* Same as the form's Cancel button */
        PostMessageW(form, WM_CLOSE, 0, 0);
        return;
    }

    /* Inno appends the app's folder name to whatever folder is clicked in its
       tree (AppendDefaultDirName). Keep doing that when the preset path ends in
       a folder that doesn't exist yet, unless the user picked that folder. */
    WCHAR result[MAX_PATH * 4];
    lstrcpynW(result, picked, ARRAYSIZE(result));
    const WCHAR *leaf = wcsrchr(current, L'\\');
    if (leaf && leaf[1] && GetFileAttributesW(current) == INVALID_FILE_ATTRIBUTES)
    {
        const WCHAR *pickedLeaf = wcsrchr(picked, L'\\');
        if (!pickedLeaf || lstrcmpiW(pickedLeaf, leaf))
        {
            size_t len = wcslen(result);
            if (len && result[len - 1] == L'\\') result[--len] = L'\0';
            if (len + wcslen(leaf) < ARRAYSIZE(result))
                wcscat(result, leaf);
        }
    }

    SetWindowTextW(c.edit, result);
    SendMessageW(c.ok, BM_CLICK, 0, 0);
}

LRESULT CALLBACK InnoFormSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData
)
{
    if (uMsg == WM_SHOWWINDOW && wParam)
    {
        InnoChildren c;
        EnumChildWindows(hWnd, FindInnoChildren, (LPARAM)&c);
        if (c.tree && c.edit && c.ok)
        {
            /* Keep Inno's form invisible; it stays alive as the picker's owner */
            SetWindowLongW(hWnd, GWL_EXSTYLE, GetWindowLongW(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            PostMessageW(hWnd, g_innoPickMsg, 0, 0);
        }
        else
        {
            RemoveWindowSubclass(hWnd, InnoFormSubclassProc, uIdSubclass);
        }
    }
    else if (uMsg == g_innoPickMsg && g_innoPickMsg)
    {
        RemoveWindowSubclass(hWnd, InnoFormSubclassProc, uIdSubclass);
        InnoChildren c;
        EnumChildWindows(hWnd, FindInnoChildren, (LPARAM)&c);
        if (c.edit && c.ok) InnoReplaceForm(hWnd, c);
        return 0;
    }
    else if (uMsg == WM_NCDESTROY)
    {
        RemoveWindowSubclass(hWnd, InnoFormSubclassProc, uIdSubclass);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void InnoWatchWindow(HWND hwnd, HWND parent, DWORD style)
{
    if (!hwnd || (style & WS_CHILD)) return;
    WCHAR cls[64];
    /* Delphi forms are named after their class (TSelectFolderForm); the child
       check on first show decides whether it is really the folder form */
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && cls[0] == L'T')
        SetWindowSubclass(hwnd, InnoFormSubclassProc, 0, 0);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_orig = nullptr;
HWND WINAPI CreateWindowExW_hook(DWORD exStyle, LPCWSTR cls, LPCWSTR name, DWORD style,
                                 int x, int y, int w, int h, HWND parent, HMENU menu,
                                 HINSTANCE inst, LPVOID param)
{
    HWND hwnd = CreateWindowExW_orig(exStyle, cls, name, style, x, y, w, h, parent, menu, inst, param);
    InnoWatchWindow(hwnd, parent, style);
    return hwnd;
}

using CreateWindowExA_t = decltype(&CreateWindowExA);
CreateWindowExA_t CreateWindowExA_orig = nullptr;
HWND WINAPI CreateWindowExA_hook(DWORD exStyle, LPCSTR cls, LPCSTR name, DWORD style,
                                 int x, int y, int w, int h, HWND parent, HMENU menu,
                                 HINSTANCE inst, LPVOID param)
{
    HWND hwnd = CreateWindowExA_orig(exStyle, cls, name, style, x, y, w, h, parent, menu, inst, param);
    InnoWatchWindow(hwnd, parent, style);
    return hwnd;
}

/* Inno runs its wizard from %TEMP%\is-XXXXX.tmp\<name>.tmp */
bool IsInnoSetupProcess()
{
    WCHAR path[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!len || len >= ARRAYSIZE(path)) return false;
    CharLowerW(path);
    return len > 4 && !lstrcmpW(path + len - 4, L".tmp") && wcsstr(path, L"\\is-");
}

BOOL Wh_ModInit(void)
{
    g_navigateMsg = RegisterWindowMessageW(L"Windhawk_ModernizeFolderPicker_Navigate");

    if (IsInnoSetupProcess())
    {
        g_innoPickMsg = RegisterWindowMessageW(L"Windhawk_ModernizeFolderPicker_Inno");
        WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_hook, &CreateWindowExW_orig);
        WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateWindowExA_hook, &CreateWindowExA_orig);
    }

    if (!WindhawkUtils::SetFunctionHook(
        SHBrowseForFolderW,
        SHBrowseForFolderW_hook,
        &SHBrowseForFolderW_orig
    ))
    {
        Wh_Log(L"Failed to hook SHBrowseForFolderW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(
        SHBrowseForFolderA,
        SHBrowseForFolderA_hook,
        &SHBrowseForFolderA_orig
    ))
    {
        Wh_Log(L"Failed to hook SHBrowseForFolderA");
    }

    return TRUE;
}
