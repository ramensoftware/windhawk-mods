// ==WindhawkMod==
// @id flexible-explorer-toolbars-deluxe
// @name Flexible Explorer Toolbars Deluxe
// @description Makes Search Bar, Address Bar, Breadcrumb Bar and others into movable toolbars
// @version 1.5
// @author Anixx
// @github https://github.com/Anixx
// @include explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Flexible Explorer Toolbars Deluxe
**!Important!** This mod curently only supports Windows 10 or Windows 11 versions up to 23H2 and 24H2/25H2 builds up to 8037.
On 24H2 and 25H2 you may have to use vivetool to enable toolbars in Explorer: `vivetool /disable /id:55063786`.
On later, unsuported builds you may need to replace the Explorerframe.dll from an earlier version.

**!Important!** To use this mod, you shoud disable any other mods that hide the classic Navigation bar, such as the `Disable Navigation Bar` mod by ItsProfessional.
This mod hides the Navigation Bar by itself.

For this mod to work you should enable a mod that restores the Navigation bar, it is recommended to install the `Windows 7 Comand Bar` mod, altough, `Classic Explorer navigation bar`
also would work if you want to retain elements of Windows 11 fluent interface, as well or any modification that restores ribbon.

This mod hides the Navigation Bar and instead creates the following optional toolbars, which could be freely moved and ordered together with the Menu Bar, if it is enabled:
* The Search Bar
* The Address Bar
* The Breadcrumbs Bar
* The Up Buton

The toolbars can be locked and unlocked.
If you are using this mod together with Classic Explorer toolbar (Open Shell), enable that toolbar before enabling this mod, otherwise its enabled state will not be remembered.

**Toolbar visibility** is controlled from the right-click context menu of any of the movable toolbars (or of the Menu Bar itself) — the same menu where "Lock the Toolbars" is located.
Four checkable items are added there ("Search Bar", "Address Bar", "Location" and "Up Band") that let you show/hide the corresponding toolbar on the fly. The choice is stored via the Windhawk Storage API.

By default, only the Search Bar is shown; the Location (breadcrumb) bar and the Up button are hidden until explicitly enabled from that context menu.

# Further adjustments
* It is recommended to install mod [Explorer Unlocked Toolbars Fix (WINAPI)](https://windhawk.net/mods/explorer-no-toolbars-bottom-gripper) to make the unlocked toolbars to appear better.
* To make the toolbars to have the 3D borders, install this mod: [Separators around File Explorer toolbars](https://windhawk.net/mods/explorer-toolbars-separators).
* To fix the appearance of the default text in the search bar under dark Classic theme, install this mod: [Classic Theme Explorer Search Fix](https://windhawk.net/mods/classic-theme-explorer-search-fix).

![screenshot](https://i.imgur.com/7lJxsAT.png)

![screenshot](https://i.imgur.com/OV8NRKJ.png)

![screenshot](https://i.imgur.com/OEthKme.png)

![screenshot](https://i.imgur.com/JXKEXL1.png)

![screenshot](https://i.imgur.com/QFFmczo.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <winternl.h>
#include <commctrl.h>
#include <exdisp.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shtypes.h>
#include <shlguid.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <windowsx.h>

constexpr UINT CMD_TOGGLE_SEARCHBAND = 0xF101;
constexpr UINT CMD_TOGGLE_BREADCRUMB = 0xF102;
constexpr UINT CMD_TOGGLE_UPBUTTON = 0xF103;
constexpr UINT CMD_TOGGLE_ADDRESSBAND = 0xF104;

constexpr UINT STR_ID_SEARCHBAR = 34304;
constexpr UINT STR_ID_BREADCRUMB = 49952;
constexpr UINT STR_ID_UPBUTTON = 14347;
constexpr UINT STR_ID_ADDRESSBAR = 12352;

enum class BandType { Search, Breadcrumb, UpButton, AddressBar };
enum ChildFlag { CF_MOVED = 1, CF_UPBUTTON = 2, CF_BREADCRUMB = 4, CF_SEARCH = 8, CF_ADDRESSBAR = 16 };

UINT g_msgDoMove = 0, g_msgFixContent = 0, g_msgSyncSettings = 0, g_msgSyncLock = 0, g_msgFixBreadcrumb = 0, g_msgTryCapture = 0, g_msgUpdateAddress = 0;

struct {
    bool s = true, b = true, u = true, a = false;
} g_set;

void LoadSettings() {
    g_set.s = Wh_GetIntValue(L"MoveSearchBand", 1);
    g_set.b = Wh_GetIntValue(L"MoveBreadcrumb", 0);
    g_set.u = Wh_GetIntValue(L"MoveUpButton", 0);
    g_set.a = Wh_GetIntValue(L"MoveAddressBar", 0);
}

CRITICAL_SECTION g_cs;
struct Lock {
    Lock() { EnterCriticalSection(&g_cs); }
    ~Lock() { LeaveCriticalSection(&g_cs); }
};

struct TbGuard {
    int x = 0, y = 0, cx = 0, cy = 0;
    bool good = false;
};

std::unordered_map<HWND, TbGuard> g_guards;
std::unordered_map<HWND, WindhawkUtils::WH_SUBCLASSPROC> g_hooks;
std::unordered_map<HWND, std::wstring> g_lastPathCache;

thread_local bool g_inApply = false;
thread_local bool g_inSync = false;
thread_local bool g_inRecalc = false;
thread_local int g_rbLayoutDepth = 0;
thread_local bool g_inAddressUpdate = false;

using SW_t = BOOL(WINAPI*)(HWND, int);
SW_t origSW = nullptr;

BOOL WINAPI Hook_SW(HWND hWnd, int nCmdShow) {
    if (hWnd && GetPropW(hWnd, L"FlexTbForceHidden")) {
        if (nCmdShow!= SW_HIDE && nCmdShow!= SW_MINIMIZE)
            return origSW(hWnd, SW_HIDE);
    }
    return origSW(hWnd, nCmdShow);
}

void HookWindow(HWND hwnd, WindhawkUtils::WH_SUBCLASSPROC proc) {
    if (hwnd && IsWindow(hwnd)) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, proc, 0);
        Lock L;
        g_hooks[hwnd] = proc;
    }
}

template<typename F>
void EnumBands(HWND rb, UINT mask, F f) {
    for (int i = 0, c = (int)SendMessage(rb, RB_GETBANDCOUNT, 0, 0); i < c; i++) {
        REBARBANDINFO rbi = { sizeof(rbi) };
        rbi.fMask = mask;
        if (SendMessage(rb, RB_GETBANDINFO, i, (LPARAM)&rbi))
            f(i, rbi);
    }
}

int GetSavedRank(const wchar_t* cls) {
    WCHAR ok[160];
    swprintf(ok, 160, L"OrderRank_%s", cls);
    return Wh_GetIntValue(ok, INT_MAX);
}

void SetSavedRank(const wchar_t* cls, int r) {
    WCHAR ok[160];
    swprintf(ok, 160, L"OrderRank_%s", cls);
    Wh_SetIntValue(ok, r);
}

struct BandState {
    UINT cx = 0;
    bool brk = false;
};

bool LoadBandState(const wchar_t* cls, BandState& out) {
    WCHAR ck[160], bk[160];
    swprintf(ck, 160, L"Cx_%s", cls);
    swprintf(bk, 160, L"Break_%s", cls);
    int cx = Wh_GetIntValue(ck, -1);
    if (cx < 20 || cx > 8000) return false;
    out.cx = (UINT)cx;
    out.brk = Wh_GetIntValue(bk, 0)!= 0;
    return true;
}

void GetEffClass(HWND ch, wchar_t* out, size_t max) {
    int f = (int)(INT_PTR)GetPropW(ch, L"FlexTbFlag");
    if (f & CF_UPBUTTON) wcsncpy(out, L"UpButtonToolbar", max);
    else if (f & CF_BREADCRUMB) wcsncpy(out, L"BreadcrumbToolbar", max);
    else if (f & CF_ADDRESSBAR) wcsncpy(out, L"AddressBarToolbar", max);
    else if (ch) GetClassName(ch, out, (int)max);
    else out[0] = 0;
}

void SaveBandPositions(HWND rb, bool saveWidth = true) {
    if (!rb ||!IsWindow(rb) || g_inApply || g_inRecalc) return;
    EnumBands(rb, RBBIM_SIZE | RBBIM_STYLE | RBBIM_CHILD, [&](int i, REBARBANDINFO& rbi) {
        WCHAR cls[256] = L"";
        if (rbi.hwndChild && IsWindow(rbi.hwndChild))
            GetEffClass(rbi.hwndChild, cls, 256);
        if (cls[0]) {
            SetSavedRank(cls, i);
            WCHAR k[160];
            if (saveWidth) {
                swprintf(k, 160, L"Cx_%s", cls);
                Wh_SetIntValue(k, (int)rbi.cx);
            }
            swprintf(k, 160, L"Break_%s", cls);
            Wh_SetIntValue(k, (rbi.fStyle & RBBS_BREAK)? 1 : 0);
        }
    });
}

void ReapplyCx(HWND rb) {
    if (g_rbLayoutDepth > 0 || g_inRecalc) return;
    RemovePropW(rb, L"FlexTbPendApply");
    g_inApply = true;
    EnumBands(rb, RBBIM_CHILD | RBBIM_SIZE | RBBIM_STYLE, [&](int i, REBARBANDINFO& rbi) {
        WCHAR cls[256] = L"";
        if (rbi.hwndChild) GetEffClass(rbi.hwndChild, cls, 256);
        BandState bs;
        if (LoadBandState(cls, bs)) {
            bool wantBreak = bs.brk;
            if (i == 0) wantBreak = false;
            if (rbi.cx!= bs.cx || ((rbi.fStyle & RBBS_BREAK)!= 0)!= wantBreak) {
                REBARBANDINFO s = { sizeof(s) };
                s.fMask = RBBIM_SIZE | RBBIM_IDEALSIZE | RBBIM_STYLE;
                s.cx = s.cxIdeal = bs.cx;
                s.fStyle = rbi.fStyle;
                if (wantBreak) s.fStyle |= RBBS_BREAK;
                else s.fStyle &= ~RBBS_BREAK;
                SendMessage(rb, RB_SETBANDINFO, i, (LPARAM)&s);
            }
        }
    });
    g_inApply = false;
}

void ApplySavedLayout(HWND rb) {
    if (!rb ||!IsWindow(rb) || g_inRecalc) return;
    g_inApply = true;
    std::vector<std::pair<int, HWND>> infos;
    EnumBands(rb, RBBIM_CHILD, [&](int i, REBARBANDINFO& rbi) {
        WCHAR cls[256] = L"";
        if (rbi.hwndChild) GetEffClass(rbi.hwndChild, cls, 256);
        infos.push_back({ GetSavedRank(cls) == INT_MAX? i : GetSavedRank(cls), rbi.hwndChild });
    });
    std::stable_sort(infos.begin(), infos.end(), [](auto& a, auto& b) { return a.first < b.first; });
    for (size_t t = 0; t < infos.size(); t++) {
        EnumBands(rb, RBBIM_CHILD, [&](int j, REBARBANDINFO& rbi) {
            if (rbi.hwndChild == infos[t].second && j!= (int)t)
                SendMessage(rb, RB_MOVEBAND, j, t);
        });
    }
    REBARBANDINFO rbi = { sizeof(rbi) };
    rbi.fMask = RBBIM_STYLE;
    if (SendMessage(rb, RB_GETBANDINFO, 0, (LPARAM)&rbi)) {
        if (rbi.fStyle & RBBS_BREAK) {
            rbi.fStyle &= ~RBBS_BREAK;
            SendMessage(rb, RB_SETBANDINFO, 0, (LPARAM)&rbi);
        }
    }
    g_inApply = false;
    SetPropW(rb, L"FlexTbApplyAtm", (HANDLE)0);
    SetPropW(rb, L"FlexTbPendApply", (HANDLE)1);
    ReapplyCx(rb);
}

HWND FindByClass(HWND p, LPCWSTR c) { return FindWindowEx(p, NULL, c, NULL); }

HWND GetCabinet(HWND h) {
    while (h) {
        WCHAR c[64];
        if (GetClassName(h, c, 64) &&!wcscmp(c, L"CabinetWClass")) return h;
        h = GetParent(h);
    }
    return NULL;
}

HWND FindNavRb(HWND c) {
    for (HWND w = FindByClass(c, L"WorkerW"); w; w = FindWindowEx(c, w, L"WorkerW", NULL))
        if (HWND r = FindByClass(w, L"ReBarWindow32")) return r;
    return NULL;
}

void RefreshBreadcrumb(HWND cab, HWND breadcrumb) {
    if (!cab ||!IsWindow(cab) ||!breadcrumb ||!IsWindow(breadcrumb)) return;
    BOOL active = (GetForegroundWindow() == cab);
    SendMessage(cab, WM_NCACTIVATE,!active, 0);
    SendMessage(cab, WM_NCACTIVATE, active, 0);
    SetWindowPos(breadcrumb, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

BOOL CALLBACK EnumSyncCabinets_Proc(HWND w, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    if (pid == GetCurrentProcessId()) {
        WCHAR c[64];
        if (GetClassName(w, c, 64) &&!wcscmp(c, L"CabinetWClass"))
            PostMessage(w, g_msgSyncSettings, 0, 0);
    }
    return TRUE;
}

void PostSyncToAllCabinets() { EnumWindows(EnumSyncCabinets_Proc, 0); }

BOOL CALLBACK EnumFindClass_Proc(HWND h, LPARAM l) {
    auto* p = (std::pair<LPCWSTR, bool*>*)l;
    WCHAR c[256];
    if (GetClassName(h, c, 256) &&!wcscmp(c, p->first)) {
        *p->second = true;
        return FALSE;
    }
    return TRUE;
}

bool ContainsClass(HWND root, LPCWSTR t) {
    WCHAR c[256];
    if (GetClassName(root, c, 256) &&!wcscmp(c, t)) return true;
    bool f = false;
    std::pair<LPCWSTR, bool*> ctx{ t, &f };
    EnumChildWindows(root, EnumFindClass_Proc, (LPARAM)&ctx);
    return f;
}

void ForceHideWorker(HWND w) {
    if (!w ||!IsWindow(w)) return;
    SetPropW(w, L"FlexTbForceHidden", (HANDLE)1);
    LONG_PTR style = GetWindowLongPtr(w, GWL_STYLE);
    if (style & WS_VISIBLE) SetWindowLongPtr(w, GWL_STYLE, style & ~WS_VISIBLE);
    ShowWindow(w, SW_HIDE);
    SetWindowPos(w, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_FRAMECHANGED);
}

void ForceCabinetRelayout(HWND cab) {
    if (!cab ||!IsWindow(cab) || g_inRecalc) return;
    HWND s = FindByClass(cab, L"ShellTabWindowClass");
    if (s) {
        RECT rc;
        GetClientRect(cab, &rc);
        SetWindowPos(s, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER | SWP_NOACTIVATE);
    }
    for (HWND w = FindByClass(cab, L"WorkerW"); w; w = FindWindowEx(cab, w, L"WorkerW", NULL))
        if (FindByClass(w, L"ReBarWindow32")) ForceHideWorker(w);
}

DWORD GetRefGripper(HWND rb) {
    DWORD style = RBBS_GRIPPERALWAYS;
    EnumBands(rb, RBBIM_STYLE | RBBIM_CHILD, [&](int, REBARBANDINFO& rbi) {
        if (rbi.hwndChild &&!((int)(INT_PTR)GetPropW(rbi.hwndChild, L"FlexTbFlag") & CF_MOVED))
            style = rbi.fStyle & (RBBS_GRIPPERALWAYS | RBBS_NOGRIPPER);
    });
    return style;
}

void SyncGrippers(HWND rb) {
    if (g_inSync ||!rb ||!IsWindow(rb) || g_inRecalc) return;
    g_inSync = true;
    DWORD grp = GetRefGripper(rb);
    EnumBands(rb, RBBIM_CHILD | RBBIM_STYLE, [&](int i, REBARBANDINFO& q) {
        if (q.hwndChild && ((int)(INT_PTR)GetPropW(q.hwndChild, L"FlexTbFlag") & CF_MOVED)
            && (q.fStyle & (RBBS_GRIPPERALWAYS | RBBS_NOGRIPPER))!= grp) {
            q.fStyle = (q.fStyle & ~(RBBS_GRIPPERALWAYS | RBBS_NOGRIPPER)) | grp;
            SendMessage(rb, RB_SETBANDINFO, i, (LPARAM)&q);
        }
    });
    g_inSync = false;
}

bool GetLockToolbarsState(HWND rb) {
    if (!rb ||!IsWindow(rb)) return false;
    REBARBANDINFO rbi = { sizeof(rbi) };
    rbi.fMask = RBBIM_STYLE;
    if (SendMessage(rb, RB_GETBANDINFO, 0, (LPARAM)&rbi))
        return!(rbi.fStyle & RBBS_GRIPPERALWAYS);
    return false;
}

LRESULT CALLBACK Tbar_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR);
LRESULT CALLBACK InnerCombo_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR);
LRESULT CALLBACK AddressEdit_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR);
LRESULT CALLBACK Rb_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR);
LRESULT CALLBACK Cab_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR);
LRESULT CALLBACK ShellTab_Proc(HWND hh, UINT mm, WPARAM ww, LPARAM ll, DWORD_PTR);
LRESULT CALLBACK WorkerW_Proc(HWND hh, UINT mm, WPARAM ww, LPARAM ll, DWORD_PTR);
LRESULT CALLBACK AddrBand_Proc(HWND hh, UINT mm, WPARAM ww, LPARAM ll, DWORD_PTR);

HWND GetBandChild(HWND rb, int flag);
HWND GetHiddenBand(HWND cab, BandType type);
void SetHiddenBand(HWND cab, BandType type, HWND ch);
void ToggleBand(HWND cab, BandType type, bool enable);
void BeginAddressBarCapture(HWND cab);
bool TryCaptureFromRoot(HWND cab);
void ForceRebarRecalc(HWND rb);

// === адресная строка: текст + иконка ===

void ReleaseBrowserCache(HWND cab) {
    if (!cab) return;
    Lock L;
    g_lastPathCache.erase(cab);
}

IWebBrowser2* GetBrowserForCab(HWND cab) {
    if (!cab) return nullptr;
    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_IShellWindows, (void**)&pSW)) ||!pSW)
        return nullptr;
    IWebBrowser2* result = nullptr;
    long cnt = 0;
    pSW->get_Count(&cnt);
    for (long i = 0; i < cnt; i++) {
        VARIANT vi; VariantInit(&vi); vi.vt = VT_I4; vi.lVal = i;
        IDispatch* pDisp = nullptr;
        if (FAILED(pSW->Item(vi, &pDisp)) ||!pDisp) { VariantClear(&vi); continue; }
        VariantClear(&vi);
        IWebBrowser2* pWB = nullptr;
        if (FAILED(pDisp->QueryInterface(IID_IWebBrowser2, (void**)&pWB)) ||!pWB) { pDisp->Release(); continue; }
        pDisp->Release();
        SHANDLE_PTR hWnd = 0;
        pWB->get_HWND(&hWnd);
        if ((HWND)hWnd!= cab) { pWB->Release(); continue; }
        result = pWB; // already AddRef'd from QI
        break;
    }
    pSW->Release();
    return result; // caller must Release() on same thread
}

LPITEMIDLIST GetPidlForCab(HWND cab)
{
    if (!cab) return nullptr;
    IWebBrowser2* pWB = GetBrowserForCab(cab);
    if (!pWB) return nullptr;
    LPITEMIDLIST result = nullptr;
    IServiceProvider* pSP = nullptr;
    if (SUCCEEDED(pWB->QueryInterface(IID_IServiceProvider, (void**)&pSP)) && pSP) {
        IShellBrowser* pSB = nullptr;
        if (SUCCEEDED(pSP->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&pSB)) && pSB) {
            IShellView* pSV = nullptr;
            if (SUCCEEDED(pSB->QueryActiveShellView(&pSV)) && pSV) {
                IFolderView* pFV = nullptr;
                if (SUCCEEDED(pSV->QueryInterface(IID_IFolderView, (void**)&pFV)) && pFV) {
                    IPersistFolder2* pPF2 = nullptr;
                    if (SUCCEEDED(pFV->GetFolder(IID_PPV_ARGS(&pPF2))) && pPF2) {
                        LPITEMIDLIST pidl = nullptr;
                        if (SUCCEEDED(pPF2->GetCurFolder(&pidl)) && pidl) {
                            result = ILClone(pidl);
                            CoTaskMemFree(pidl);
                        }
                        pPF2->Release();
                    }
                    pFV->Release();
                }
                pSV->Release();
            }
            pSB->Release();
        }
        pSP->Release();
    }
    pWB->Release();
    return result;
}

std::wstring GetFriendlyPathFromPidl(LPITEMIDLIST pidl)
{
    std::wstring out;
    if (!pidl) return out;
    PWSTR psz = nullptr;
    if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEEDITING, &psz)) && psz) {
        out = psz;
        CoTaskMemFree(psz);
        return out;
    }
    if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_FILESYSPATH, &psz)) && psz) {
        out = psz;
        CoTaskMemFree(psz);
        return out;
    }
    if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEPARSING, &psz)) && psz) {
        out = psz;
        CoTaskMemFree(psz);
    }
    return out;
}

HWND FindEditInAddressBand(HWND addrBand)
{
    if (!addrBand) return NULL;
    HWND innerCombo = FindWindowExW(addrBand, NULL, L"ComboBox", NULL);
    if (!innerCombo) {
        HWND comboEx = FindWindowExW(addrBand, NULL, L"ComboBoxEx32", NULL);
        if (comboEx) innerCombo = FindWindowExW(comboEx, NULL, L"ComboBox", NULL);
        else {
            innerCombo = addrBand;
            innerCombo = FindWindowExW(innerCombo, NULL, L"ComboBox", NULL);
        }
    }
    if (!innerCombo) return NULL;
    return FindWindowExW(innerCombo, NULL, L"Edit", NULL);
}

void UpdateAddressBandForCab(HWND cab)
{
    if (!cab ||!IsWindow(cab) || g_inAddressUpdate) return;
    HWND mr = (HWND)GetPropW(cab, L"FlexTbRb");
    if (!mr ||!IsWindow(mr)) return;
    HWND addr = GetBandChild(mr, CF_ADDRESSBAR);
    if (!addr ||!IsWindow(addr)) {
        addr = GetHiddenBand(cab, BandType::AddressBar);
        if (!addr ||!IsWindow(addr)) return;
    }
    HWND edit = FindEditInAddressBand(addr);
    if (!edit ||!IsWindow(edit)) return;
    if (GetFocus() == edit) return;

    LPITEMIDLIST pidl = GetPidlForCab(cab);
    if (!pidl) return;
    std::wstring curPath = GetFriendlyPathFromPidl(pidl);
    if (curPath.empty()) { ILFree(pidl); return; }

    {
        Lock L;
        auto it = g_lastPathCache.find(cab);
        if (it!= g_lastPathCache.end() && it->second == curPath) {
            ILFree(pidl);
            return;
        }
    }

    int curTxtLen = GetWindowTextLengthW(edit);
    std::wstring curTxt;
    curTxt.resize(curTxtLen + 1);
    GetWindowTextW(edit, curTxt.data(), curTxtLen + 1);
    curTxt.resize(curTxtLen);
    bool needText = wcscmp(curTxt.c_str(), curPath.c_str())!= 0;
    if (!needText) {
        Lock L;
        g_lastPathCache[cab] = curPath;
        ILFree(pidl);
        return;
    }

    SHFILEINFO sfiSys = {};
    HIMAGELIST hSysSmall = (HIMAGELIST)SHGetFileInfoW(L"C:\\", FILE_ATTRIBUTE_DIRECTORY, &sfiSys, sizeof(sfiSys), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
    SHFILEINFO sfiPidl = {};
    BOOL gotPidlIcon = SHGetFileInfoW((LPCWSTR)pidl, 0, &sfiPidl, sizeof(sfiPidl), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

    g_inAddressUpdate = true;

    if (hSysSmall) {
        HIMAGELIST hCur = (HIMAGELIST)SendMessageW(addr, CBEM_GETIMAGELIST, 0, 0);
        if (hCur!= hSysSmall) {
            SendMessageW(addr, CBEM_SETIMAGELIST, 0, (LPARAM)hSysSmall);
        }
    }

    COMBOBOXEXITEMW cbei = {0};
    cbei.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
    cbei.mask |= CBEIF_TEXT;
    cbei.iItem = -1;
    cbei.pszText = (LPWSTR)curPath.c_str();
    cbei.cchTextMax = (int)curPath.size();
    cbei.iImage = sfiPidl.iIcon;
    cbei.iSelectedImage = sfiPidl.iIcon;

    if (!gotPidlIcon) {
        cbei.mask &= ~(CBEIF_IMAGE | CBEIF_SELECTEDIMAGE);
    }

    SendMessageW(addr, CBEM_SETITEMW, 0, (LPARAM)&cbei);

    {
        Lock L;
        g_lastPathCache[cab] = curPath;
    }

    g_inAddressUpdate = false;
    ILFree(pidl);
}

int GetIdealToolbarHeight(HWND hwndRef) {
    UINT dpi = GetDpiForWindow(hwndRef);
    return GetSystemMetricsForDpi(SM_CYSIZE, dpi) + GetSystemMetricsForDpi(SM_CYBORDER, dpi) * 2 + 2;
}

void CacheGoodSize(HWND ch) {
    if (!ch ||!IsWindow(ch)) return;
    RECT wr;
    GetWindowRect(ch, &wr);
    int cx = wr.right - wr.left;
    int cy = wr.bottom - wr.top;
    if (cy > 0 && cy < 60) SetPropW(ch, L"FlexTbLastCY", (HANDLE)(INT_PTR)cy);
    if (cx > 0) SetPropW(ch, L"FlexTbLastCX", (HANDLE)(INT_PTR)cx);
}

void SaveTypedAddress(LPCWSTR rawPath) {
    if (!rawPath ||!*rawPath) return;
    std::wstring tmpStr(rawPath);
    size_t start = 0;
    while (start < tmpStr.size() && (tmpStr[start] == L' ' || tmpStr[start] == L'\t' || tmpStr[start] == L'\r' || tmpStr[start] == L'\n')) start++;
    size_t end = tmpStr.size();
    while (end > start && (tmpStr[end-1] == L' ' || tmpStr[end-1] == L'\t' || tmpStr[end-1] == L'\r' || tmpStr[end-1] == L'\n')) end--;
    if (end <= start) return;
    std::wstring s = tmpStr.substr(start, end-start);
    if (s.length() < 2) return;
    if (s.length() < 3 && s[1]!= L':') return;

    HKEY hKey = NULL;
    LONG lr = RegCreateKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\TypedPaths",
        0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &hKey, NULL);
    if (lr!= ERROR_SUCCESS ||!hKey) return;

    std::vector<std::wstring> existing;
    existing.reserve(25);
    std::vector<WCHAR> dataBuf(4096);
    for (int i = 1; i <= 50; i++) {
        WCHAR name[32];
        swprintf(name, 32, L"url%d", i);
        DWORD type = 0;
        DWORD cb = (DWORD)(dataBuf.size() * sizeof(WCHAR));
        LONG q = RegQueryValueExW(hKey, name, NULL, &type, (BYTE*)dataBuf.data(), &cb);
        if (q == ERROR_SUCCESS) {
            if (type == REG_SZ || type == REG_EXPAND_SZ) {
                dataBuf[4095] = 0;
                existing.emplace_back(dataBuf.data());
            } else break;
        } else break;
    }

    auto it = std::find_if(existing.begin(), existing.end(), [&](const std::wstring& v){ return _wcsicmp(v.c_str(), s.c_str())==0; });
    if (it!= existing.end()) {
        std::wstring moved = *it;
        existing.erase(it);
        existing.insert(existing.begin(), std::move(moved));
    } else {
        if ((int)existing.size() >= 25) existing.resize(24);
        existing.insert(existing.begin(), s);
    }

    int writeCount = std::min((int)existing.size(), 25);
    for (int i = 0; i < writeCount; i++) {
        WCHAR name[32];
        swprintf(name, 32, L"url%d", i+1);
        RegSetValueExW(hKey, name, 0, REG_SZ, (BYTE*)existing[i].c_str(), (DWORD)((existing[i].length() + 1) * sizeof(WCHAR)));
    }
    for (int i = writeCount + 1; i <= 50; i++) {
        WCHAR name[32];
        swprintf(name, 32, L"url%d", i);
        RegDeleteValueW(hKey, name);
    }
    RegCloseKey(hKey);
}

void NavigateCabinet(HWND cab, LPCWSTR rawPath) {
    if (!cab ||!IsWindow(cab) ||!rawPath ||!*rawPath) return;
    std::wstring pathStr(rawPath);
    for (auto& ch : pathStr) if (ch == L'\r' || ch == L'\n') ch = 0;
    pathStr = std::wstring(pathStr.c_str());
    size_t start = 0;
    while (start < pathStr.size() && (pathStr[start] == L' ' || pathStr[start] == L'\t')) start++;
    size_t end = pathStr.size();
    while (end > start && (pathStr[end-1] == L' ' || pathStr[end-1] == L'\t')) end--;
    if (end <= start) return;
    std::wstring s = pathStr.substr(start, end-start);
    if (s.empty()) return;
    IWebBrowser2* pWB = GetBrowserForCab(cab);
    if (!pWB) return;
    VARIANT vPath, vEmpty; VariantInit(&vPath); VariantInit(&vEmpty);
    vPath.vt = VT_BSTR; vPath.bstrVal = SysAllocString(s.c_str());
    HRESULT hr = pWB->Navigate2(&vPath, &vEmpty, &vEmpty, &vEmpty, &vEmpty);
    VariantClear(&vPath);
    pWB->Release();
    if (SUCCEEDED(hr)) {
        SaveTypedAddress(s.c_str());
    }
    PostMessage(cab, g_msgUpdateAddress, 0, 0);
}

LRESULT CALLBACK AddressEdit_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if (m == WM_NCDESTROY) { Lock L; g_hooks.erase(h); return DefSubclassProc(h, m, w, l); }
    if (m == WM_KEYDOWN && w == VK_RETURN) {
        int txtLen = GetWindowTextLengthW(h);
        std::wstring txt;
        txt.resize(txtLen + 1);
        GetWindowTextW(h, txt.data(), txtLen + 1);
        txt.resize(txtLen);
        HWND comboBox = GetParent(h);
        HWND comboEx = GetParent(comboBox);
        HWND rebar = GetParent(comboEx);
        if (!rebar) rebar = (HWND)GetPropW(comboEx, L"FlexTbReBar");
        HWND cab = GetCabinet(rebar? rebar : comboEx);
        NavigateCabinet(cab, txt.c_str());
        return 0;
    }
    if (m == WM_CHAR && w == 13) return 0;
    return DefSubclassProc(h, m, w, l);
}

LRESULT CALLBACK InnerCombo_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if (m == WM_NCDESTROY) { Lock L; g_hooks.erase(h); return DefSubclassProc(h, m, w, l); }
    if (m == WM_WINDOWPOSCHANGED) {
        HWND parentEx = GetParent(h);
        if (parentEx && IsWindow(parentEx)) {
            int outerH = GetIdealToolbarHeight(parentEx);
            int newEditH = outerH - 8;
            if (newEditH < 12) newEditH = 12;
            if (SendMessageW(h, CB_GETITEMHEIGHT, (WPARAM)-1, 0)!= newEditH)
                SendMessageW(h, CB_SETITEMHEIGHT, (WPARAM)-1, (LPARAM)newEditH);
            SendMessageW(h, CB_SETMINVISIBLE, 8, 0);
        }
    }
    if (m == WM_KEYDOWN && w == VK_RETURN) {
        HWND edit = FindWindowExW(h, NULL, L"Edit", NULL);
        if (edit && IsWindow(edit)) {
            int txtLen = GetWindowTextLengthW(edit);
            std::wstring txt;
            txt.resize(txtLen + 1);
            GetWindowTextW(edit, txt.data(), txtLen + 1);
            txt.resize(txtLen);
            HWND comboEx = GetParent(h);
            HWND rebar = GetParent(comboEx);
            if (!rebar) rebar = (HWND)GetPropW(comboEx, L"FlexTbReBar");
            HWND cab = GetCabinet(rebar? rebar : comboEx);
            NavigateCabinet(cab, txt.c_str());
            return 0;
        }
    }
    return DefSubclassProc(h, m, w, l);
}

LRESULT CALLBACK Tbar_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if (m == WM_NCDESTROY) {
        RemovePropW(h, L"FlexTbReBar");
        { Lock L; g_guards.erase(h); g_hooks.erase(h); }
        return DefSubclassProc(h, m, w, l);
    }
    if (m == WM_CONTEXTMENU) {
        if (HWND rb = GetParent(h)) {
            POINT pt = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
            if (pt.x == -1 && pt.y == -1) { RECT rc; GetWindowRect(h, &rc); pt.x = rc.left; pt.y = rc.bottom; }
            if (HWND cab = GetCabinet(rb)) {
                HWND st = FindByClass(cab, L"ShellTabWindowClass");
                if (st) if (HWND ww = FindByClass(st, L"WorkerW")) {
                    if (HMENU hM = LoadMenuW(GetModuleHandleW(L"explorerframe.dll"), MAKEINTRESOURCEW(264))) {
                        if (HMENU sub = GetSubMenu(hM, 0)) {
                            TrackPopupMenuEx(sub, TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, ww, NULL);
                            PostMessage(ww, WM_NULL, 0, 0);
                        }
                        DestroyMenu(hM);
                    }
                }
            }
            return 0;
        }
    }
    if (GetPropW(h, L"FlexTbIsHidden")) return DefSubclassProc(h, m, w, l);
    int flag = (int)(INT_PTR)GetPropW(h, L"FlexTbFlag");
    if (m == WM_WINDOWPOSCHANGING && (flag & (CF_BREADCRUMB | CF_ADDRESSBAR | CF_SEARCH | CF_UPBUTTON))) {
        auto* p = (WINDOWPOS*)l;
        Lock L;
        TbGuard& g = g_guards[h];
        if (g_rbLayoutDepth > 0) {
            if (!(p->flags & SWP_NOMOVE)) { g.x = p->x; g.y = p->y; }
            if (!(p->flags & SWP_NOSIZE)) { g.cx = p->cx; g.cy = GetIdealToolbarHeight(h); p->cy = g.cy; }
            g.good = true;
            p->flags &= ~SWP_HIDEWINDOW;
        } else if (g.good) {
            p->x = g.x; p->y = g.y; p->cx = g.cx; p->cy = g.cy;
            p->flags = (p->flags & ~(SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE)) | SWP_SHOWWINDOW;
        }
    }
    if (m == WM_WINDOWPOSCHANGED && (flag & CF_ADDRESSBAR)) {
        HWND innerCombo = FindWindowExW(h, NULL, L"ComboBox", NULL);
        if (innerCombo && IsWindow(innerCombo)) {
            RECT rc;
            GetClientRect(h, &rc);
            int outerH = rc.bottom - rc.top;
            if (outerH < 20) outerH = GetIdealToolbarHeight(h);
            int newEditH = outerH - 8;
            if (newEditH < 12) newEditH = 12;
            SendMessageW(innerCombo, CB_SETITEMHEIGHT, (WPARAM)-1, (LPARAM)newEditH);
            SendMessageW(innerCombo, CB_SETMINVISIBLE, 8, 0);
            SetWindowPos(innerCombo, NULL, 0, 0, rc.right, 200, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSENDCHANGING);
        }
    }
    if (m == WM_SHOWWINDOW &&!w &&!g_rbLayoutDepth && (flag & (CF_BREADCRUMB | CF_ADDRESSBAR))) return 0;
    return DefSubclassProc(h, m, w, l);
}

bool HasId(HMENU m, UINT id, int d = 0) {
    for (int i = 0, c = GetMenuItemCount(m); i < c; i++) {
        UINT cid = GetMenuItemID(m, i);
        if (cid == id) return true;
        if (cid == (UINT)-1 && d < 2) if (HasId(GetSubMenu(m, i), id, d + 1)) return true;
    }
    return false;
}

std::wstring GetLocalStr(UINT id, LPCWSTR def) {
    WCHAR buf[256];
    return LoadStringW(GetModuleHandleW(L"explorerframe.dll"), id, buf, 256) > 0? buf : def;
}

void SyncMenu(HMENU m, HWND rb) {
    LoadSettings();
    if (!HasId(m, CMD_TOGGLE_SEARCHBAND)) {
        InsertMenuW(m, 0, MF_BYPOSITION | MF_STRING, CMD_TOGGLE_UPBUTTON, GetLocalStr(STR_ID_UPBUTTON, L"Up Button").c_str());
        InsertMenuW(m, 0, MF_BYPOSITION | MF_STRING, CMD_TOGGLE_ADDRESSBAND, GetLocalStr(STR_ID_ADDRESSBAR, L"Address Bar").c_str());
        InsertMenuW(m, 0, MF_BYPOSITION | MF_STRING, CMD_TOGGLE_BREADCRUMB, GetLocalStr(STR_ID_BREADCRUMB, L"Location").c_str());
        InsertMenuW(m, 0, MF_BYPOSITION | MF_STRING, CMD_TOGGLE_SEARCHBAND, GetLocalStr(STR_ID_SEARCHBAR, L"Search Bar").c_str());
    }
    CheckMenuItem(m, CMD_TOGGLE_SEARCHBAND, MF_BYCOMMAND | (g_set.s? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(m, CMD_TOGGLE_BREADCRUMB, MF_BYCOMMAND | (g_set.b? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(m, CMD_TOGGLE_UPBUTTON, MF_BYCOMMAND | (g_set.u? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(m, CMD_TOGGLE_ADDRESSBAND, MF_BYCOMMAND | (g_set.a? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(m, 41484, MF_BYCOMMAND | (GetLockToolbarsState(rb)? MF_CHECKED : MF_UNCHECKED));
}

BOOL CALLBACK EnumForLock_Proc(HWND w, LPARAM lp) {
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    if (pid!= GetCurrentProcessId()) return TRUE;
    WCHAR c[64];
    if (GetClassName(w, c, 64) &&!wcscmp(c, L"CabinetWClass")) PostMessage(w, g_msgSyncLock, (WPARAM)lp, 0);
    return TRUE;
}

BOOL HandleMenuResult(UINT cmd, bool isRet, HWND hw) {
    if (cmd >= CMD_TOGGLE_SEARCHBAND && cmd <= CMD_TOGGLE_ADDRESSBAND) {
        bool& s = (cmd == CMD_TOGGLE_SEARCHBAND)? g_set.s : (cmd == CMD_TOGGLE_BREADCRUMB)? g_set.b : (cmd == CMD_TOGGLE_UPBUTTON)? g_set.u : g_set.a;
        s =!s;
        LPCWSTR key = (cmd == CMD_TOGGLE_SEARCHBAND)? L"MoveSearchBand" : (cmd == CMD_TOGGLE_BREADCRUMB)? L"MoveBreadcrumb" : (cmd == CMD_TOGGLE_UPBUTTON)? L"MoveUpButton" : L"MoveAddressBar";
        Wh_SetIntValue(key, s? 1 : 0);
        PostSyncToAllCabinets();
        return isRet? 0 : TRUE;
    }
    if (cmd == 41484) {
        HWND cab = GetCabinet(hw);
        HWND rb = cab? (HWND)GetPropW(cab, L"FlexTbRb") : NULL;
        if (!rb ||!IsWindow(rb)) { if (!isRet && hw) PostMessage(hw, WM_COMMAND, MAKEWPARAM(LOWORD(cmd), 0), 0); return isRet? cmd : TRUE; }
        EnumWindows(EnumForLock_Proc, (LPARAM)!GetLockToolbarsState(rb));
        return isRet? 0 : TRUE;
    }
    if (!isRet && cmd && hw) PostMessage(hw, WM_COMMAND, MAKEWPARAM(LOWORD(cmd), 0), 0);
    return isRet? cmd : TRUE;
}

using TPM_t = BOOL(WINAPI*)(HMENU, UINT, int, int, int, HWND, CONST RECT*);
TPM_t origTPM;
BOOL WINAPI Hook_TPM(HMENU m, UINT f, int x, int y, int r, HWND hw, CONST RECT* pr) {
    HWND cab = GetCabinet(hw);
    if (!cab ||!HasId(m, 41484)) return origTPM(m, f, x, y, r, hw, pr);
    HWND rb = (HWND)GetPropW(cab, L"FlexTbRb");
    SyncMenu(m, rb);
    bool isRet = f & TPM_RETURNCMD;
    UINT cmd = (UINT)origTPM(m, f | TPM_RETURNCMD, x, y, r, hw, pr);
    return HandleMenuResult(cmd, isRet, hw);
}

using TPMEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
TPMEx_t origTPMEx;
BOOL WINAPI Hook_TPMEx(HMENU m, UINT f, int x, int y, HWND hw, LPTPMPARAMS p) {
    HWND cab = GetCabinet(hw);
    if (!cab ||!HasId(m, 41484)) return origTPMEx(m, f, x, y, hw, p);
    HWND rb = (HWND)GetPropW(cab, L"FlexTbRb");
    SyncMenu(m, rb);
    bool isRet = f & TPM_RETURNCMD;
    UINT cmd = (UINT)origTPMEx(m, f | TPM_RETURNCMD, x, y, hw, p);
    return HandleMenuResult(cmd, isRet, hw);
}

HWND GetBandChild(HWND rb, int flag) {
    HWND res = NULL;
    EnumBands(rb, RBBIM_CHILD, [&](int, REBARBANDINFO& rbi) {
        if (rbi.hwndChild &&!res && (((int)(INT_PTR)GetPropW(rbi.hwndChild, L"FlexTbFlag")) & flag)) res = rbi.hwndChild;
    });
    return res;
}

HWND GetHiddenBand(HWND cab, BandType type) {
    LPCWSTR prop = (type == BandType::Search)? L"FlexTbRmSearch" : (type == BandType::Breadcrumb)? L"FlexTbRmBread" : (type == BandType::AddressBar)? L"FlexTbRmAddr" : L"FlexTbRmUp";
    return (HWND)GetPropW(cab, prop);
}

void SetHiddenBand(HWND cab, BandType type, HWND ch) {
    LPCWSTR prop = (type == BandType::Search)? L"FlexTbRmSearch" : (type == BandType::Breadcrumb)? L"FlexTbRmBread" : (type == BandType::AddressBar)? L"FlexTbRmAddr" : L"FlexTbRmUp";
    if (ch) SetPropW(cab, prop, (HANDLE)ch);
    else RemovePropW(cab, prop);
}

BOOL CALLBACK EnumBreadcrumb_Proc(HWND ch, LPARAM lp) {
    WCHAR c[64];
    if (GetClassName(ch, c, 64) &&!wcscmp(c, L"Breadcrumb Parent")) { *(HWND*)lp = ch; return FALSE; }
    return TRUE;
}

struct FindComboCtx { HWND found; };
BOOL CALLBACK FindCombo_Proc(HWND h, LPARAM lp) {
    FindComboCtx* ctx = (FindComboCtx*)lp;
    WCHAR cls[64];
    if (!GetClassName(h, cls, 64)) return TRUE;
    if (!wcscmp(cls, L"ComboBoxEx32")) { ctx->found = h; return FALSE; }
    if (!wcscmp(cls, L"msctls_progress32")) {
        HWND combo = FindWindowEx(h, NULL, L"ComboBoxEx32", NULL);
        if (combo) { ctx->found = combo; return FALSE; }
    }
    return TRUE;
}

HWND FindComboInAddressBand(HWND addrRoot) {
    if (!addrRoot ||!IsWindow(addrRoot)) return NULL;
    FindComboCtx ctx = { NULL };
    EnumChildWindows(addrRoot, FindCombo_Proc, (LPARAM)&ctx);
    return ctx.found;
}

void ClickBreadcrumbToolbar(HWND tb) {
    if (!tb ||!IsWindow(tb)) return;
    RECT rc;
    GetClientRect(tb, &rc);
    if (rc.right <= rc.left) return;
    int x = 4;
    int count = (int)SendMessage(tb, TB_BUTTONCOUNT, 0, 0);
    if (count > 0) {
        RECT ir = {};
        if (SendMessage(tb, TB_GETITEMRECT, count - 1, (LPARAM)&ir)) x = ir.right + 6;
    }
    if (x >= rc.right) x = rc.right > 8? rc.right - 4 : 1;
    int y = (rc.bottom - rc.top) / 2;
    LPARAM lp = MAKELPARAM(x, y);
    SendMessage(tb, WM_MOUSEMOVE, 0, lp);
    SendMessage(tb, WM_LBUTTONDOWN, MK_LBUTTON, lp);
    SendMessage(tb, WM_LBUTTONUP, 0, lp);
}

void ShowOffscreenForClick(HWND ch) {
    if (!ch ||!IsWindow(ch)) return;
    int cx = (int)(INT_PTR)GetPropW(ch, L"FlexTbLastCX");
    int cy = (int)(INT_PTR)GetPropW(ch, L"FlexTbLastCY");
    if (cx <= 0 || cy <= 0) {
        RECT wr;
        GetWindowRect(ch, &wr);
        if (cx <= 0) cx = wr.right - wr.left;
        if (cy <= 0) cy = wr.bottom - wr.top;
    }
    if (cx <= 0) cx = 250;
    if (cy <= 0) cy = 24;
    SetWindowPos(ch, NULL, -10000, -10000, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_SHOWWINDOW);
}

void HideAfterOffscreenClick(HWND ch) { if (ch && IsWindow(ch)) ShowWindow(ch, SW_HIDE); }

void CaptureAddressCombo(HWND cab, HWND mr, HWND combo) {
    ShowWindow(combo, SW_HIDE);
    SetParent(combo, mr);
    SetPropW(combo, L"FlexTbFlag", (HANDLE)(INT_PTR)(CF_MOVED | CF_ADDRESSBAR));
    SetPropW(combo, L"FlexTbIsHidden", (HANDLE)1);
    SetPropW(combo, L"FlexTbReBar", (HANDLE)mr);
    HookWindow(combo, Tbar_Proc);
    SetHiddenBand(cab, BandType::AddressBar, combo);
    CacheGoodSize(combo);
    HWND innerCombo = FindWindowExW(combo, NULL, L"ComboBox", NULL);
    if (innerCombo && IsWindow(innerCombo)) {
        HookWindow(innerCombo, InnerCombo_Proc);
        HWND edit = FindWindowExW(innerCombo, NULL, L"Edit", NULL);
        if (edit && IsWindow(edit)) HookWindow(edit, AddressEdit_Proc);
        int outerH = GetIdealToolbarHeight(combo);
        int newEditH = outerH - 8;
        if (newEditH < 12) newEditH = 12;
        SendMessageW(innerCombo, CB_SETITEMHEIGHT, (WPARAM)-1, (LPARAM)newEditH);
        SendMessageW(innerCombo, CB_SETMINVISIBLE, 8, 0);
    }
}

void ForceRebarRecalc(HWND rb) {
    if (!rb ||!IsWindow(rb) || g_inRecalc || g_inApply) return;
    g_inRecalc = true;
    RECT rc;
    GetClientRect(rb, &rc);
    SendMessage(rb, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
    SetWindowPos(rb, NULL, 0, 0, rc.right, rc.bottom, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_NOSENDCHANGING);
    g_inRecalc = false;
}

bool TryCaptureFromRoot(HWND cab) {
    if (!cab ||!IsWindow(cab)) return false;
    if (!GetPropW(cab, L"FlexTbAddrCapturing")) return false;
    HWND mr = (HWND)GetPropW(cab, L"FlexTbRb");
    HWND root = (HWND)GetPropW(cab, L"FlexTbAddrRoot");
    if (!mr ||!IsWindow(mr) ||!root ||!IsWindow(root)) { RemovePropW(cab, L"FlexTbAddrCapturing"); return false; }
    HWND combo = FindComboInAddressBand(root);
    if (!combo) return false;
    RemovePropW(cab, L"FlexTbAddrCapturing");
    HWND tempBC = (HWND)GetPropW(cab, L"FlexTbAddrTempBC");
    RemovePropW(cab, L"FlexTbAddrTempBC");
    CaptureAddressCombo(cab, mr, combo);
    if (tempBC) HideAfterOffscreenClick(tempBC);
    if (g_set.a) ToggleBand(cab, BandType::AddressBar, true);
    PostMessage(cab, g_msgUpdateAddress, 0, 0);
    return true;
}

void ToggleBand(HWND cab, BandType type, bool enable) {
    int f = (type == BandType::Search)? CF_SEARCH : (type == BandType::Breadcrumb)? CF_BREADCRUMB : (type == BandType::AddressBar)? CF_ADDRESSBAR : CF_UPBUTTON;
    HWND mr = (HWND)GetPropW(cab, L"FlexTbRb");
    if (!mr ||!IsWindow(mr)) return;
    HWND ch = GetBandChild(mr, f);
    if (!enable && ch) {
        SaveBandPositions(mr, false);
        CacheGoodSize(ch);
        SetPropW(ch, L"FlexTbIsHidden", (HANDLE)1);
        int foundIdx = -1;
        EnumBands(mr, RBBIM_CHILD, [&](int i, REBARBANDINFO& rbi) { if (rbi.hwndChild == ch) foundIdx = i; });
        if (foundIdx!= -1) SendMessage(mr, RB_DELETEBAND, foundIdx, 0);
        ShowWindow(ch, SW_HIDE);
        SetHiddenBand(cab, type, ch);
        ForceCabinetRelayout(cab);
        SaveBandPositions(mr, false);
    } else if (enable &&!ch) {
        if (type == BandType::AddressBar &&!GetHiddenBand(cab, type)) { BeginAddressBarCapture(cab); return; }
        ch = GetHiddenBand(cab, type);
        if (!ch ||!IsWindow(ch)) return;
        SetHiddenBand(cab, type, NULL);
        RemovePropW(ch, L"FlexTbIsHidden");
        SetParent(ch, mr);
        WCHAR c[256];
        GetEffClass(ch, c, 256);
        BandState bs;
        bool hasSv = LoadBandState(c, bs);
        int minW = (type == BandType::Search)? 200 : (type == BandType::Breadcrumb)? 250 : (type == BandType::AddressBar)? 220 : 30;
        if (!hasSv) { bs.cx = (UINT)minW; bs.brk = false; }
        else { if (bs.cx < (UINT)minW) bs.cx = (UINT)minW; if (bs.cx > 2000) bs.cx = (UINT)minW; }
        if (type == BandType::UpButton) {
            SendMessage(ch, TB_SETBITMAPSIZE, 0, MAKELONG(16, 16));
            SendMessage(ch, TB_SETPADDING, 0, MAKELONG(4, 4));
            SendMessage(ch, TB_AUTOSIZE, 0, 0);
        }
        int idealH = GetIdealToolbarHeight(cab);
        int savedRank = GetSavedRank(c);
        int bandCount = (int)SendMessage(mr, RB_GETBANDCOUNT, 0, 0);
        int insertAt = -1;
        if (savedRank!= INT_MAX && savedRank >= 0 && savedRank < bandCount) insertAt = savedRank;
        REBARBANDINFO rbi = { sizeof(rbi) };
        rbi.fMask = RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_IDEALSIZE;
        rbi.fStyle = GetRefGripper(mr) | (bs.brk? RBBS_BREAK : 0);
        rbi.hwndChild = ch;
        rbi.cyMinChild = rbi.cyMaxChild = rbi.cyChild = idealH;
        rbi.cx = rbi.cxIdeal = bs.cx;
        rbi.cyIntegral = 1;
        if (SendMessage(mr, RB_INSERTBAND, (WPARAM)insertAt, (LPARAM)&rbi)) {
            SetPropW(ch, L"FlexTbFlag", (HANDLE)(INT_PTR)(CF_MOVED | f));
            if (f!= CF_SEARCH) HookWindow(ch, Tbar_Proc);
            if (f == CF_ADDRESSBAR) {
                HWND inner = FindWindowExW(ch, NULL, L"ComboBox", NULL);
                if (inner) {
                    HookWindow(inner, InnerCombo_Proc);
                    HWND edit = FindWindowExW(inner, NULL, L"Edit", NULL);
                    if (edit) HookWindow(edit, AddressEdit_Proc);
                }
            }
            ShowWindow(ch, SW_SHOW);
            SetWindowPos(ch, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
            bool isFirst = (insertAt == 0 || savedRank == 0);
            ApplySavedLayout(mr);
            if (isFirst || f == CF_ADDRESSBAR) ForceRebarRecalc(mr);
            else { SyncGrippers(mr); ForceCabinetRelayout(cab); }
            PostMessage(cab, g_msgFixContent, (WPARAM)ch, 0);
            if (type == BandType::Breadcrumb || type == BandType::AddressBar) RefreshBreadcrumb(cab, ch);
            SaveBandPositions(mr, false);
            if (type == BandType::AddressBar) PostMessage(cab, g_msgUpdateAddress, 0, 0);
        }
    }
}

LRESULT CALLBACK Rb_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if (m == WM_NCDESTROY) { { Lock L; g_hooks.erase(h); } return DefSubclassProc(h, m, w, l); }
    if (m == WM_CONTEXTMENU) {
        HWND cab = GetCabinet(h);
        if (!cab) return DefSubclassProc(h, m, w, l);
        POINT ptScreen = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
        if (ptScreen.x == -1 && ptScreen.y == -1) { RECT rc; GetWindowRect(h, &rc); ptScreen.x = rc.left; ptScreen.y = rc.bottom; }
        POINT ptClient = ptScreen;
        ScreenToClient(h, &ptClient);
        RBHITTESTINFO rbht = {};
        rbht.pt = ptClient;
        int idx = (int)SendMessage(h, RB_HITTEST, 0, (LPARAM)&rbht);
        bool takeOver = true;
        if (idx >= 0) {
            REBARBANDINFO rbi = { sizeof(rbi) };
            rbi.fMask = RBBIM_CHILD;
            if (SendMessage(h, RB_GETBANDINFO, idx, (LPARAM)&rbi) && rbi.hwndChild)
                takeOver = ((int)(INT_PTR)GetPropW(rbi.hwndChild, L"FlexTbFlag") & CF_MOVED)!= 0;
            else takeOver = false;
        }
        if (!takeOver) return DefSubclassProc(h, m, w, l);
        bool shown = false;
        HWND st = FindByClass(cab, L"ShellTabWindowClass");
        if (st) if (HWND ww = FindByClass(st, L"WorkerW")) {
            if (HMENU hM = LoadMenuW(GetModuleHandleW(L"explorerframe.dll"), MAKEINTRESOURCEW(264))) {
                if (HMENU sub = GetSubMenu(hM, 0)) {
                    TrackPopupMenuEx(sub, TPM_RIGHTBUTTON | TPM_LEFTBUTTON, ptScreen.x, ptScreen.y, ww, NULL);
                    PostMessage(ww, WM_NULL, 0, 0);
                    shown = true;
                }
                DestroyMenu(hM);
            }
        }
        return shown? 0 : DefSubclassProc(h, m, w, l);
    }
    if (m == RB_SETBANDINFO) {
        auto* inf = (REBARBANDINFO*)l;
        if (inf && (inf->fMask & RBBIM_CHILDSIZE)) {
            HWND ch = (inf->fMask & RBBIM_CHILD)? inf->hwndChild : [&]() {
                REBARBANDINFO q = { sizeof(q) };
                q.fMask = RBBIM_CHILD;
                return SendMessage(h, RB_GETBANDINFO, w, (LPARAM)&q)? q.hwndChild : NULL;
            }();
            if (ch && ((int)(INT_PTR)GetPropW(ch, L"FlexTbFlag") & CF_MOVED)) {
                if (inf->cbSize < sizeof(REBARBANDINFO)) return DefSubclassProc(h, m, w, l);
                REBARBANDINFO local = *inf;
                int idealH = GetIdealToolbarHeight(h);
                local.cyMinChild = local.cyChild = local.cyMaxChild = idealH;
                local.cyIntegral = 1;
                g_rbLayoutDepth++;
                LRESULT r = DefSubclassProc(h, m, w, (LPARAM)&local);
                g_rbLayoutDepth--;
                if (!g_inSync) if (HWND cab = GetCabinet(h)) if (GetPropW(cab, L"FlexTbMoved")) SyncGrippers(h);
                return r;
            }
        }
    }
    g_rbLayoutDepth++;
    LRESULT r = DefSubclassProc(h, m, w, l);
    g_rbLayoutDepth--;
    if (m == WM_SIZE && GetPropW(h, L"FlexTbPendApply") &&!g_inApply &&!g_inRecalc) {
        int a = (int)(INT_PTR)GetPropW(h, L"FlexTbApplyAtm");
        if (a < 5) { SetPropW(h, L"FlexTbApplyAtm", (HANDLE)(INT_PTR)(a + 1)); ReapplyCx(h); }
        else RemovePropW(h, L"FlexTbPendApply");
    }
    if (m == RB_INSERTBAND) if (HWND cab = GetCabinet(h)) if (!GetPropW(cab, L"FlexTbMoved")) PostMessage(cab, g_msgDoMove, 0, 0);
    if (m == RB_SETBANDINFO &&!g_inSync) if (HWND cab = GetCabinet(h)) if (GetPropW(cab, L"FlexTbMoved")) SyncGrippers(h);
    return r;
}

LRESULT CALLBACK ParentRb_Proc(HWND hh, UINT mm, WPARAM ww, LPARAM ll, DWORD_PTR) {
    if (mm == WM_NCDESTROY) { { Lock L; g_hooks.erase(hh); } return DefSubclassProc(hh, mm, ww, ll); }
    if (mm == WM_NOTIFY && (((NMHDR*)ll)->code == RBN_LAYOUTCHANGED || ((NMHDR*)ll)->code == RBN_ENDDRAG)) {
        HWND c = (HWND)GetPropW(((NMHDR*)ll)->hwndFrom, L"FlexTbCab");
        if (!c) c = GetCabinet(((NMHDR*)ll)->hwndFrom);
        if (c && GetPropW(c, L"FlexTbMoved") &&!g_inApply &&!g_inRecalc) SaveBandPositions(((NMHDR*)ll)->hwndFrom);
    }
    return DefSubclassProc(hh, mm, ww, ll);
}

LRESULT CALLBACK Cab_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if (m == WM_NCDESTROY) {
        ReleaseBrowserCache(h);
        { Lock L; g_hooks.erase(h); }
        return DefSubclassProc(h, m, w, l);
    }
    if (m == g_msgUpdateAddress) {
        UpdateAddressBandForCab(h);
        return 0;
    }
    if (m == WM_SETTEXT && GetPropW(h, L"FlexTbMoved")) {
        LRESULT res = DefSubclassProc(h, m, w, l);
        PostMessage(h, g_msgUpdateAddress, 0, 0);
        BOOL active = (GetForegroundWindow() == h);
        SendMessage(h, WM_NCACTIVATE,!active, 0);
        SendMessage(h, WM_NCACTIVATE, active, 0);
        return res;
    }
    if (m == WM_CLOSE || m == WM_DESTROY) {
        if (GetPropW(h, L"FlexTbMoved")) { HWND mr = (HWND)GetPropW(h, L"FlexTbRb"); if (mr) SaveBandPositions(mr); }
    }
    if (m == g_msgTryCapture) { TryCaptureFromRoot(h); return 0; }
    if (m == g_msgSyncLock) {
        HWND mr = (HWND)GetPropW(h, L"FlexTbRb");
        if (mr && IsWindow(mr) && GetLockToolbarsState(mr)!= (bool)w) {
            HWND st = FindByClass(h, L"ShellTabWindowClass");
            if (st) if (HWND ww = FindByClass(st, L"WorkerW")) PostMessage(ww, WM_COMMAND, MAKEWPARAM(41484, 0), 0);
        }
        return 0;
    }
    if (m == g_msgFixBreadcrumb) {
        if (GetPropW(h, L"FlexTbMoved")) {
            HWND mr = (HWND)GetPropW(h, L"FlexTbRb");
            if (mr && IsWindow(mr)) {
                HWND bc = GetBandChild(mr, CF_BREADCRUMB);
                if (bc && IsWindow(bc) &&!GetPropW(bc, L"FlexTbIsHidden")) RefreshBreadcrumb(h, bc);
            }
        }
        return 0;
    }
    if (m == WM_SIZE && w == SIZE_MINIMIZED) SetPropW(h, L"FlexTbWasMinimized", (HANDLE)1);
    if (m == WM_SIZE && (w == SIZE_RESTORED || w == SIZE_MAXIMIZED) &&!IsIconic(h)) {
        LRESULT res = DefSubclassProc(h, m, w, l);
        if (GetPropW(h, L"FlexTbWasMinimized")) { RemovePropW(h, L"FlexTbWasMinimized"); PostMessage(h, g_msgFixBreadcrumb, 0, 0); }
        return res;
    }
    if (m == g_msgSyncSettings) {
        LoadSettings();
        if (GetPropW(h, L"FlexTbMoved")) {
            ToggleBand(h, BandType::Search, g_set.s);
            ToggleBand(h, BandType::Breadcrumb, g_set.b);
            ToggleBand(h, BandType::UpButton, g_set.u);
            ToggleBand(h, BandType::AddressBar, g_set.a);
        }
        return 0;
    }
    if (m == g_msgFixContent) {
        HWND ch = (HWND)w;
        if (!ch ||!IsWindow(ch)) return 0;
        int flag = (int)(INT_PTR)GetPropW(ch, L"FlexTbFlag");
        if (flag & CF_ADDRESSBAR) {
            SetWindowPos(ch, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            HWND inner = FindWindowExW(ch, NULL, L"ComboBox", NULL);
            if (inner) {
                RECT rc;
                GetClientRect(ch, &rc);
                if (rc.right > 0) {
                    int newEditH = (rc.bottom - 8);
                    if (newEditH < 12) newEditH = 12;
                    SendMessageW(inner, CB_SETITEMHEIGHT, (WPARAM)-1, (LPARAM)newEditH);
                    SendMessageW(inner, CB_SETMINVISIBLE, 8, 0);
                    SetWindowPos(inner, NULL, 0, 0, rc.right, 200, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
                }
            }
        } else SendMessage(ch, TB_AUTOSIZE, 0, 0);
        InvalidateRect(ch, NULL, TRUE);
        return 0;
    }
    if (m == g_msgDoMove) {
        if (GetPropW(h, L"FlexTbMoved")) {
            return 1;
        }
        HWND st = FindByClass(h, L"ShellTabWindowClass");
        if (!st) return 0;
        HWND ww = FindByClass(st, L"WorkerW");
        if (!ww) return 0;
        HWND mRb = FindByClass(ww, L"ReBarWindow32");
        HWND nRb = FindNavRb(h);
        if (!mRb ||!nRb) return 0;
        SetWindowLongPtr(mRb, GWL_STYLE, (GetWindowLongPtr(mRb, GWL_STYLE) & ~RBS_FIXEDORDER) | RBS_VARHEIGHT);
        HookWindow(GetParent(mRb), ParentRb_Proc);
        SetPropW(mRb, L"FlexTbCab", (HANDLE)h);
        SetPropW(h, L"FlexTbRb", (HANDLE)mRb);

        struct BTM { int idx; HWND c; HWND left; int w; int type; };
        std::vector<BTM> mv;
        EnumBands(nRb, RBBIM_CHILD, [&](int i, REBARBANDINFO& rbi) {
            if (!rbi.hwndChild) return;
            if (ContainsClass(rbi.hwndChild, L"UniversalSearchBand") || ContainsClass(rbi.hwndChild, L"Search Box")) {
                RECT rc; GetWindowRect(rbi.hwndChild, &rc); int w2 = rc.right - rc.left;
                mv.push_back({ i, rbi.hwndChild, NULL, w2 < 200? 200 : w2, CF_SEARCH });
            } else if (ContainsClass(rbi.hwndChild, L"Address Band Root")) {
                HWND root = rbi.hwndChild;
                SetPropW(h, L"FlexTbAddrRoot", (HANDLE)root);
                HWND b = root;
                WCHAR c2[64];
                if (!GetClassName(b, c2, 64) || wcscmp(c2, L"Breadcrumb Parent")!= 0) b = NULL;
                EnumChildWindows(root, EnumBreadcrumb_Proc, (LPARAM)&b);
                HWND bcTb = NULL;
                if (b) bcTb = FindByClass(b, L"ToolbarWindow32");
                if (bcTb) { RECT rc; GetWindowRect(bcTb, &rc); int w2 = rc.right - rc.left; mv.push_back({ i, bcTb, b, w2 < 250? 250 : w2, CF_BREADCRUMB }); }
            } else if (ContainsClass(rbi.hwndChild, L"UpBand")) {
                if (HWND t = FindByClass(rbi.hwndChild, L"ToolbarWindow32")) {
                    RECT rc; GetWindowRect(t, &rc); int w2 = rc.right - rc.left;
                    mv.push_back({ i, t, rbi.hwndChild, w2 < 30? 30 : w2, CF_UPBUTTON });
                }
            }
        });

        if (!mv.empty()) {
            std::vector<int> delDone;
            for (auto it = mv.rbegin(); it!= mv.rend(); ++it) {
                if (std::find(delDone.begin(), delDone.end(), it->idx) == delDone.end()) {
                    SendMessage(nRb, RB_DELETEBAND, it->idx, 0);
                    delDone.push_back(it->idx);
                }
                if (it->left && IsWindow(it->left)) { SetPropW(it->left, L"FlexTbNeutered", (HANDLE)1); ShowWindow(it->left, SW_HIDE); }
            }
            for (auto& b : mv) {
                SetParent(b.c, mRb);
                WCHAR c2[256] = L"";
                if (b.type == CF_UPBUTTON) wcsncpy(c2, L"UpButtonToolbar", 256);
                else if (b.type == CF_BREADCRUMB) wcsncpy(c2, L"BreadcrumbToolbar", 256);
                else GetClassName(b.c, c2, 256);
                BandState bs;
                bool hasSv = LoadBandState(c2, bs);
                if (b.type == CF_UPBUTTON) { SendMessage(b.c, TB_SETBITMAPSIZE, 0, MAKELONG(16, 16)); SendMessage(b.c, TB_SETPADDING, 0, MAKELONG(4, 4)); SendMessage(b.c, TB_AUTOSIZE, 0, 0); }
                int idealH = GetIdealToolbarHeight(h);
                REBARBANDINFO ri = { sizeof(ri) };
                ri.fMask = RBBIM_STYLE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_IDEALSIZE;
                ri.fStyle = GetRefGripper(mRb) | ((hasSv? bs.brk : true)? RBBS_BREAK : 0);
                ri.hwndChild = b.c;
                ri.cyMinChild = ri.cyMaxChild = ri.cyChild = idealH;
                ri.cx = ri.cxIdeal = hasSv? bs.cx : (UINT)b.w;
                ri.cyIntegral = 1;
                if (!hasSv) ri.fStyle &= ~RBBS_BREAK;
                if (SendMessage(mRb, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&ri)) {
                    SetPropW(b.c, L"FlexTbFlag", (HANDLE)(INT_PTR)(CF_MOVED | b.type));
                    ShowWindow(b.c, SW_SHOW);
                    if (b.type!= CF_SEARCH) HookWindow(b.c, Tbar_Proc);
                }
            }
            ApplySavedLayout(mRb);
            SyncGrippers(mRb);
            ForceRebarRecalc(mRb);
        }
        ForceHideWorker(GetParent(nRb));
        SetPropW(h, L"FlexTbMoved", (HANDLE)1);
        ForceCabinetRelayout(h);
        for (auto& b : mv) PostMessage(h, g_msgFixContent, (WPARAM)b.c, 0);
        if (!g_set.s) ToggleBand(h, BandType::Search, false);
        if (!g_set.b) ToggleBand(h, BandType::Breadcrumb, false);
        if (!g_set.u) ToggleBand(h, BandType::UpButton, false);
        if (g_set.a) ToggleBand(h, BandType::AddressBar, true);
        SaveBandPositions(mRb, false);
        PostMessage(h, g_msgUpdateAddress, 0, 0);
        return 0;
    }
    if (m == WM_ACTIVATE || m == WM_SETFOCUS) {
        if (!GetPropW(h, L"FlexTbMoved")) PostMessage(h, g_msgDoMove, 0, 0);
        else {
            if (g_set.a) {
                HWND mr = (HWND)GetPropW(h, L"FlexTbRb");
                if (mr && IsWindow(mr) &&!GetBandChild(mr, CF_ADDRESSBAR) &&!GetHiddenBand(h, BandType::AddressBar) &&!GetPropW(h, L"FlexTbAddrCapturing"))
                    BeginAddressBarCapture(h);
            }
        }
    }
    return DefSubclassProc(h, m, w, l);
}

LRESULT CALLBACK ShellTab_Proc(HWND hh, UINT mm, WPARAM ww, LPARAM ll, DWORD_PTR) {
    if (mm == WM_NCDESTROY) { { Lock L; g_hooks.erase(hh); } return DefSubclassProc(hh, mm, ww, ll); }
    if (mm == WM_WINDOWPOSCHANGING && GetPropW(GetParent(hh), L"FlexTbMoved")) {
        auto* p = (WINDOWPOS*)ll;
        RECT rc;
        GetClientRect(GetParent(hh), &rc);
        p->x = p->y = 0;
        p->cx = rc.right;
        p->cy = rc.bottom;
        p->flags = (p->flags & ~(SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW)) | SWP_NOZORDER | SWP_NOACTIVATE;
    }
    return DefSubclassProc(hh, mm, ww, ll);
}

LRESULT CALLBACK WorkerW_Proc(HWND hh, UINT mm, WPARAM ww, LPARAM ll, DWORD_PTR) {
    if (mm == WM_NCDESTROY) { RemovePropW(hh, L"FlexTbForceHidden"); { Lock L; g_hooks.erase(hh); } return DefSubclassProc(hh, mm, ww, ll); }
    if (GetPropW(hh, L"FlexTbForceHidden")) {
        if (mm == WM_WINDOWPOSCHANGING) {
            auto* p = (WINDOWPOS*)ll;
            p->flags &= ~SWP_SHOWWINDOW;
            p->cx = 0; p->cy = 0;
            p->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
        } else if (mm == WM_STYLECHANGING && ww == GWL_STYLE) ((STYLESTRUCT*)ll)->styleNew &= ~WS_VISIBLE;
        else if (mm == WM_PAINT || mm == WM_NCPAINT) { PAINTSTRUCT ps; BeginPaint(hh, &ps); EndPaint(hh, &ps); return 0; }
        else if (mm == WM_ERASEBKGND) return 1;
        else if (mm == WM_NCHITTEST) return HTTRANSPARENT;
    }
    return DefSubclassProc(hh, mm, ww, ll);
}

LRESULT CALLBACK AddrBand_Proc(HWND hh, UINT mm, WPARAM ww, LPARAM ll, DWORD_PTR) {
    if (mm == WM_NCDESTROY) { { Lock L; g_hooks.erase(hh); } return DefSubclassProc(hh, mm, ww, ll); }
    if (GetPropW(hh, L"FlexTbNeutered")) return DefWindowProc(hh, mm, ww, ll);
    if ((mm == WM_WINDOWPOSCHANGED || mm == WM_PARENTNOTIFY) && GetPropW(GetCabinet(hh), L"FlexTbAddrCapturing")) {
        if (HWND cab = GetCabinet(hh)) PostMessage(cab, g_msgTryCapture, 0, 0);
    }
    return DefSubclassProc(hh, mm, ww, ll);
}

void BeginAddressBarCapture(HWND cab) {
    if (!cab ||!IsWindow(cab)) return;
    if (GetPropW(cab, L"FlexTbAddrCapturing")) return;
    HWND mr = (HWND)GetPropW(cab, L"FlexTbRb");
    if (!mr ||!IsWindow(mr)) return;
    if (GetBandChild(mr, CF_ADDRESSBAR) || GetHiddenBand(cab, BandType::AddressBar)) return;
    HWND root = (HWND)GetPropW(cab, L"FlexTbAddrRoot");
    if (!root ||!IsWindow(root)) return;
    if (HWND combo = FindComboInAddressBand(root)) { CaptureAddressCombo(cab, mr, combo); if (g_set.a) ToggleBand(cab, BandType::AddressBar, true); PostMessage(cab, g_msgUpdateAddress, 0, 0); return; }
    SetPropW(cab, L"FlexTbAddrCapturing", (HANDLE)1);
    HWND bcToolbar = GetBandChild(mr, CF_BREADCRUMB);
    HWND tempBC = NULL;
    if (!bcToolbar) {
        bcToolbar = GetHiddenBand(cab, BandType::Breadcrumb);
        if (bcToolbar && IsWindow(bcToolbar)) {
            ShowOffscreenForClick(bcToolbar);
            tempBC = bcToolbar;
            SetPropW(cab, L"FlexTbAddrTempBC", (HANDLE)tempBC);
        }
    }
    if (!bcToolbar ||!IsWindow(bcToolbar)) { RemovePropW(cab, L"FlexTbAddrCapturing"); if (tempBC) HideAfterOffscreenClick(tempBC); return; }
    ClickBreadcrumbToolbar(bcToolbar);
}

void ProcessWnd(HWND h) {
    if (!h ||!IsWindow(h)) return;
    WCHAR c[256];
    if (!GetClassName(h, c, 256)) return;
    if (!wcscmp(c, L"CabinetWClass")) HookWindow(h, Cab_Proc);
    else if (!wcscmp(c, L"ShellTabWindowClass") && GetCabinet(h)) HookWindow(h, ShellTab_Proc);
    else if (!wcscmp(c, L"WorkerW")) {
        HWND p = GetParent(h);
        WCHAR pc[64];
        if (p && GetClassName(p, pc, 64) &&!wcscmp(pc, L"CabinetWClass")) { HookWindow(h, WorkerW_Proc); ForceHideWorker(h); }
    } else if (!wcscmp(c, L"ReBarWindow32") && GetCabinet(h)) HookWindow(h, Rb_Proc);
    else if (!wcscmp(c, L"Address Band Root") && GetCabinet(h)) HookWindow(h, AddrBand_Proc);
}

BOOL CALLBACK EnumProcessWnd_Proc(HWND ch, LPARAM) { ProcessWnd(ch); return TRUE; }

using CWExW_t = decltype(&CreateWindowExW);
CWExW_t origCWExW;

HWND WINAPI Hook_CWExW(DWORD s, LPCWSTR c, LPCWSTR wn, DWORD st, int X, int Y, int W, int H, HWND p, HMENU mi, HINSTANCE hi, LPVOID lp) {
    HWND hw = origCWExW(s, c, wn, st, X, Y, W, H, p, mi, hi, lp);
    if (hw && c &&!IS_INTRESOURCE(c)) {
        ProcessWnd(hw);
        EnumChildWindows(hw, EnumProcessWnd_Proc, 0);
        if (!wcscmp(c, L"ComboBoxEx32")) {
            if (HWND cab = GetCabinet(hw)) {
                if (GetPropW(cab, L"FlexTbAddrCapturing")) PostMessage(cab, g_msgTryCapture, 0, 0);
            }
        }
    }
    return hw;
}

using NtSet_t = NTSTATUS(NTAPI*)(HANDLE, PUNICODE_STRING, ULONG, ULONG, PVOID, ULONG);
NtSet_t origNtSet;

NTSTATUS NTAPI Hook_NtSet(HANDLE k, PUNICODE_STRING v, ULONG ti, ULONG t, PVOID d, ULONG ds) {
    if (v && v->Buffer && v->Length && (v->Length / 2) == 12) {
        bool match = true;
        LPCWSTR tgt = L"ITBar7Layout";
        for (int i = 0; i < 12; i++) if (towlower(v->Buffer[i])!= towlower(tgt[i])) match = false;
        if (match) return 0;
    }
    return origNtSet(k, v, ti, t, d, ds);
}

BOOL Wh_ModInit() {
    LoadSettings();
    InitializeCriticalSection(&g_cs);
    g_msgDoMove = RegisterWindowMessage(L"FlexExpTb_DoMove");
    g_msgFixContent = RegisterWindowMessage(L"FlexExpTb_Fix");
    g_msgSyncSettings = RegisterWindowMessage(L"FlexExpTb_SyncSettings");
    g_msgSyncLock = RegisterWindowMessage(L"FlexExpTb_SyncLock");
    g_msgFixBreadcrumb = RegisterWindowMessage(L"FlexExpTb_FixBreadcrumb");
    g_msgTryCapture = RegisterWindowMessage(L"FlexExpTb_TryCapture");
    g_msgUpdateAddress = RegisterWindowMessage(L"FlexExpTb_UpdateAddr");
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)Hook_CWExW, (void**)&origCWExW);
    Wh_SetFunctionHook((void*)TrackPopupMenu, (void*)Hook_TPM, (void**)&origTPM);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)Hook_TPMEx, (void**)&origTPMEx);
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtSetValueKey"), (void*)Hook_NtSet, (void**)&origNtSet);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)Hook_SW, (void**)&origSW);
    for (HWND w = GetTopWindow(NULL); w; w = GetNextWindow(w, GW_HWNDNEXT)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (pid == GetCurrentProcessId() && GetCabinet(w)) {
            ProcessWnd(w);
            EnumChildWindows(w, EnumProcessWnd_Proc, 0);
            PostMessage(w, g_msgDoMove, 0, 0);
        }
    }
    return TRUE;
}

void Wh_ModUninit() {
    {
        Lock L;
        g_lastPathCache.clear();
    }
    std::vector<std::pair<HWND, WindhawkUtils::WH_SUBCLASSPROC>> hooksToClean;
    { Lock l; for (auto& pair : g_hooks) hooksToClean.push_back(pair); g_hooks.clear(); }
    for (auto& pair : hooksToClean) if (IsWindow(pair.first)) WindhawkUtils::RemoveWindowSubclassFromAnyThread(pair.first, pair.second);
    DeleteCriticalSection(&g_cs);
}
