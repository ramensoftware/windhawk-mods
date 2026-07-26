// ==WindhawkMod==
// @id              flexible-explorer-toolbars-deluxe
// @name            Flexible Explorer Toolbars Deluxe
// @description     Makes Search Bar, Breadcrumb Bar and others into movable toolbars
// @version         1.3.2
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lcomctl32
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
* The Search bar
* The Breadcrumbs Bar
* The Up Buton

The toolbars can be enabled and disabled via context menus.

The toolbars can be locked and unlocked.
If you are using this mod together with Classic Explorer toolbar (Open Shell), enable that toolbar before enabling this mod, otherwise its enabled state will not be remembered.

**Toolbar visibility** is controlled from the right-click context menu of any of the movable toolbars (or of the Menu Bar itself) — the same menu where "Lock the Toolbars" is located.
Three checkable items are added there ("Search Bar", "Address" and "Up") that let you show/hide the corresponding toolbar on the fly. The choice is stored via the Windhawk Storage API.

By default, only the Search Bar is shown; the Address (breadcrumb) bar and the Up button are hidden until explicitly enabled from that context menu.

# Further adjustments
* It is recommended to install mod [Explorer Unlocked Toolbars Fix (WINAPI)](https://windhawk.net/mods/explorer-no-toolbars-bottom-gripper) to make the unlocked toolbars to appear better.
* To make the toolbars to have the 3D borders, install this mod: [Separators around File Explorer toolbars](https://windhawk.net/mods/explorer-toolbars-separators).
* To fix the appearance of the default text in the search bar under dark Classic theme, install this mod: [Classic Theme Explorer Search Fix](https://windhawk.net/mods/classic-theme-explorer-search-fix).
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <winternl.h>
#include <commctrl.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <windowsx.h>

constexpr UINT CMD_TOGGLE_SEARCHBAND = 0xF101, CMD_TOGGLE_BREADCRUMB = 0xF102, CMD_TOGGLE_UPBUTTON = 0xF103;
constexpr UINT STR_ID_SEARCHBAR = 34304, STR_ID_BREADCRUMB = 49952, STR_ID_UPBUTTON = 14347;
enum class BandType { Search, Breadcrumb, UpButton };
enum ChildFlag { CF_MOVED=1, CF_UPBUTTON=2, CF_BREADCRUMB=4, CF_SEARCH=8 };

UINT g_msgDoMove = 0, g_msgFixContent = 0, g_msgSyncSettings = 0;
struct { bool s=true, b=true, u=true; } g_set;

void LoadSettings() {
    g_set.s = Wh_GetIntValue(L"MoveSearchBand", 1);
    g_set.b = Wh_GetIntValue(L"MoveBreadcrumb", 0);
    g_set.u = Wh_GetIntValue(L"MoveUpButton", 0);
}

CRITICAL_SECTION g_cs;
struct Lock { Lock(){EnterCriticalSection(&g_cs);} ~Lock(){LeaveCriticalSection(&g_cs);} };

struct TbGuard{int x=0,y=0,cx=0,cy=0;bool good=false;};
std::unordered_map<HWND, TbGuard> g_guards;
std::unordered_map<HWND, WindhawkUtils::WH_SUBCLASSPROC> g_hooks;

thread_local bool g_inApply=false, g_inSync=false;
thread_local int g_rbLayoutDepth=0;

void HookWindow(HWND hwnd, WindhawkUtils::WH_SUBCLASSPROC proc) {
    if(hwnd && IsWindow(hwnd)) { WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, proc, 0); Lock L; g_hooks[hwnd] = proc; }
}

template<typename F> void EnumBands(HWND rb, UINT mask, F f) {
    for(int i=0, c=(int)SendMessage(rb,RB_GETBANDCOUNT,0,0); i<c; i++) {
        REBARBANDINFO rbi={sizeof(rbi)}; rbi.fMask=mask;
        if(SendMessage(rb,RB_GETBANDINFO,i,(LPARAM)&rbi)) f(i, rbi);
    }
}

int GetSavedRank(const wchar_t* cls) { WCHAR ok[160]; swprintf(ok,160,L"OrderRank_%s",cls); return Wh_GetIntValue(ok, INT_MAX); }
void SetSavedRank(const wchar_t* cls, int r) { WCHAR ok[160]; swprintf(ok,160,L"OrderRank_%s",cls); Wh_SetIntValue(ok, r); }
struct BandState{UINT cx; bool brk;};
bool LoadBandState(const wchar_t* cls, BandState& out) {
    WCHAR ck[160], bk[160]; swprintf(ck,160,L"Cx_%s",cls); swprintf(bk,160,L"Break_%s",cls);
    int cx = Wh_GetIntValue(ck, -1); if(cx<20 || cx>8000) return false;
    out.cx = (UINT)cx; out.brk = Wh_GetIntValue(bk, 0) != 0; return true;
}

void GetEffClass(HWND ch, wchar_t* out, size_t max) {
    int f = (int)(INT_PTR)GetPropW(ch, L"FlexTbFlag");
    if(f & CF_UPBUTTON) wcsncpy(out, L"UpButtonToolbar", max);
    else if(f & CF_BREADCRUMB) wcsncpy(out, L"BreadcrumbToolbar", max);
    else if(ch) GetClassName(ch, out, (int)max);
    else out[0]=0;
}

void SaveBandPositions(HWND rb, bool saveWidth = true) {
    if(!rb || !IsWindow(rb)) return;
    EnumBands(rb, RBBIM_SIZE|RBBIM_STYLE|RBBIM_CHILD, [&](int i, REBARBANDINFO& rbi){
        WCHAR cls[256]=L""; if(rbi.hwndChild && IsWindow(rbi.hwndChild)) GetEffClass(rbi.hwndChild, cls, 256);
        if(cls[0]) {
            SetSavedRank(cls, i); 
            if(saveWidth) {
                WCHAR k[160];
                swprintf(k,160,L"Cx_%s",cls); Wh_SetIntValue(k, (int)rbi.cx);
                swprintf(k,160,L"Break_%s",cls); Wh_SetIntValue(k, (rbi.fStyle&RBBS_BREAK)?1:0);
            }
        }
    });
}

void ReapplyCx(HWND rb) {
    RemovePropW(rb, L"FlexTbPendApply"); g_inApply=true; bool changed=false;
    EnumBands(rb, RBBIM_CHILD|RBBIM_SIZE|RBBIM_STYLE, [&](int i, REBARBANDINFO& rbi){
        WCHAR cls[256]=L""; if(rbi.hwndChild) GetEffClass(rbi.hwndChild, cls, 256);
        BandState bs;
        if(LoadBandState(cls, bs) && (rbi.cx!=bs.cx || ((rbi.fStyle&RBBS_BREAK)!=0)!=bs.brk)) {
            REBARBANDINFO s={sizeof(s)}; s.fMask=RBBIM_SIZE|RBBIM_IDEALSIZE|RBBIM_STYLE;
            s.cx=s.cxIdeal=bs.cx; s.fStyle=rbi.fStyle;
            if(bs.brk) s.fStyle|=RBBS_BREAK; else s.fStyle&=~RBBS_BREAK;
            SendMessage(rb,RB_SETBANDINFO,i,(LPARAM)&s); changed=true;
        }
    });
    g_inApply=false; if(changed) SetPropW(rb, L"FlexTbPendApply", (HANDLE)1);
}

void ApplySavedLayout(HWND rb) {
    if(!rb || !IsWindow(rb)) return;
    g_inApply=true; std::vector<std::pair<int, HWND>> infos;
    EnumBands(rb, RBBIM_CHILD, [&](int i, REBARBANDINFO& rbi){
        WCHAR cls[256]=L""; if(rbi.hwndChild) GetEffClass(rbi.hwndChild, cls, 256);
        infos.push_back({GetSavedRank(cls)==INT_MAX ? i : GetSavedRank(cls), rbi.hwndChild});
    });
    std::stable_sort(infos.begin(), infos.end(), [](auto& a, auto& b){ return a.first < b.first; });
    
    for(size_t t=0; t<infos.size(); t++) {
        EnumBands(rb, RBBIM_CHILD, [&](int j, REBARBANDINFO& rbi){
            if(rbi.hwndChild==infos[t].second && j!=(int)t) SendMessage(rb, RB_MOVEBAND, j, t);
        });
    }
    g_inApply=false; SetPropW(rb, L"FlexTbApplyAtm", (HANDLE)0); SetPropW(rb, L"FlexTbPendApply", (HANDLE)1); ReapplyCx(rb);
}

// Window finding & visibility
HWND FindByClass(HWND p, LPCWSTR c) { return FindWindowEx(p, NULL, c, NULL); }
HWND GetCabinet(HWND h) { while(h){ WCHAR c[64]; if(GetClassName(h,c,64) && !wcscmp(c,L"CabinetWClass")) return h; h=GetParent(h); } return NULL; }
HWND FindNavRb(HWND c) { for(HWND w=FindByClass(c,L"WorkerW"); w; w=FindWindowEx(c,w,L"WorkerW",NULL)) if(HWND r=FindByClass(w,L"ReBarWindow32")) return r; return NULL; }

BOOL CALLBACK EnumFindClass_Proc(HWND h, LPARAM l) {
    auto* p = (std::pair<LPCWSTR, bool*>*)l; WCHAR c[256];
    if(GetClassName(h,c,256) && !wcscmp(c, p->first)) { *p->second = true; return FALSE; }
    return TRUE;
}

bool ContainsClass(HWND root, LPCWSTR t) {
    WCHAR c[256]; if(GetClassName(root,c,256) && !wcscmp(c,t)) return true;
    bool f = false; std::pair<LPCWSTR, bool*> ctx{t, &f};
    EnumChildWindows(root, EnumFindClass_Proc, (LPARAM)&ctx);
    return f;
}

void ForceHideWorker(HWND w) {
    if(!w || !IsWindow(w)) return;
    
    // КРИТИЧНО: не дёргаем SetWindowLongPtr/SWP_FRAMECHANGED повторно!
    bool alreadyHidden = GetPropW(w, L"FlexTbForceHidden") && 
                         !(GetWindowLongPtr(w, GWL_STYLE) & WS_VISIBLE);
    
    SetPropW(w, L"FlexTbForceHidden", (HANDLE)1);
    
    if(alreadyHidden) return;
    
    LONG_PTR style = GetWindowLongPtr(w, GWL_STYLE);
    if(style & WS_VISIBLE)
        SetWindowLongPtr(w, GWL_STYLE, style & ~WS_VISIBLE);
    
    ShowWindow(w, SW_HIDE);
    SetWindowPos(w, NULL, 0, 0, 0, 0,
        SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_FRAMECHANGED);
}

void ForceCabinetRelayout(HWND cab) {
    if(!cab || !IsWindow(cab)) return;
    
    HWND s = FindByClass(cab, L"ShellTabWindowClass");
    if(s) { 
        RECT rc; 
        GetClientRect(cab, &rc); 
        SetWindowPos(s, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER | SWP_NOACTIVATE); 
    }
    
    for(HWND w = FindByClass(cab, L"WorkerW"); w; w = FindWindowEx(cab, w, L"WorkerW", NULL))
        if(FindByClass(w, L"ReBarWindow32")) 
            ForceHideWorker(w);
    
    RECT rc; 
    GetClientRect(cab, &rc); 
    SendMessage(cab, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
    RedrawWindow(cab, NULL, NULL,
        RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_ERASENOW | RDW_FRAME);
}

DWORD GetRefGripper(HWND rb) {
    DWORD style = RBBS_GRIPPERALWAYS;
    EnumBands(rb, RBBIM_STYLE|RBBIM_CHILD, [&](int i, REBARBANDINFO& rbi){
        if(rbi.hwndChild && !((int)(INT_PTR)GetPropW(rbi.hwndChild, L"FlexTbFlag") & CF_MOVED)) 
            style = rbi.fStyle & (RBBS_GRIPPERALWAYS|RBBS_NOGRIPPER);
    }); return style;
}

void SyncGrippers(HWND rb) {
    if(g_inSync || !rb || !IsWindow(rb)) return; g_inSync=true;
    DWORD grp = GetRefGripper(rb);
    EnumBands(rb, RBBIM_CHILD|RBBIM_STYLE, [&](int i, REBARBANDINFO& q){
        if(q.hwndChild && ((int)(INT_PTR)GetPropW(q.hwndChild, L"FlexTbFlag") & CF_MOVED) && (q.fStyle&(RBBS_GRIPPERALWAYS|RBBS_NOGRIPPER))!=grp) {
            q.fStyle=(q.fStyle&~(RBBS_GRIPPERALWAYS|RBBS_NOGRIPPER))|grp;
            SendMessage(rb,RB_SETBANDINFO,i,(LPARAM)&q);
        }
    }); g_inSync=false;
}

// Subclasses
LRESULT CALLBACK Tbar_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if(m == WM_NCDESTROY) {
        { Lock L; g_guards.erase(h); g_hooks.erase(h); }
        return DefSubclassProc(h, m, w, l);
    }
    if(m == WM_CONTEXTMENU) {
        if(HWND rb = GetParent(h)) {
            POINT pt = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
            if(pt.x==-1 && pt.y==-1) { RECT rc; GetWindowRect(h, &rc); pt.x=rc.left; pt.y=rc.bottom; }
            if(HWND cab = GetCabinet(rb))
                if(HWND ww = FindByClass(FindByClass(cab,L"ShellTabWindowClass"),L"WorkerW")) {
                    if(HMENU hM = LoadMenuW(GetModuleHandleW(L"explorerframe.dll"), MAKEINTRESOURCEW(264))) {
                        if(HMENU sub = GetSubMenu(hM, 0)) {
                            TrackPopupMenuEx(sub, TPM_RIGHTBUTTON|TPM_LEFTBUTTON, pt.x, pt.y, ww, NULL);
                            PostMessage(ww, WM_NULL, 0, 0);
                        } DestroyMenu(hM);
                    }
                }
            return 0;
        }
    }
    
    // Отключаем защиту видимости, если мы целенаправленно скрываем панель из меню
    if(GetPropW(h, L"FlexTbIsHidden")) return DefSubclassProc(h,m,w,l);

    int flag = (int)(INT_PTR)GetPropW(h, L"FlexTbFlag");
    if(m==WM_WINDOWPOSCHANGING && (flag & CF_BREADCRUMB)) {
        auto* p = (WINDOWPOS*)l; Lock L; TbGuard& g = g_guards[h];
        if(g_rbLayoutDepth>0) {
            if(!(p->flags&SWP_NOMOVE)){g.x=p->x; g.y=p->y;} if(!(p->flags&SWP_NOSIZE)){g.cx=p->cx; g.cy=p->cy;}
            g.good=true; p->flags&=~SWP_HIDEWINDOW;
        } else if(g.good) {
            p->x=g.x; p->y=g.y; p->cx=g.cx; p->cy=g.cy;
            p->flags = (p->flags & ~(SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE)) | SWP_SHOWWINDOW;
        }
    }
    if(m==WM_SHOWWINDOW && !w && !g_rbLayoutDepth && (flag & CF_BREADCRUMB)) return 0;
    return DefSubclassProc(h,m,w,l);
}

// Menu handling
bool HasId(HMENU m, UINT id, int d=0) {
    for(int i=0, c=GetMenuItemCount(m); i<c; i++) {
        UINT cid=GetMenuItemID(m,i); if(cid==id) return true;
        if(cid==(UINT)-1 && d<2) if(HasId(GetSubMenu(m,i), id, d+1)) return true;
    } return false;
}

std::wstring GetLocalStr(UINT id, LPCWSTR def) {
    WCHAR buf[256];
    return LoadStringW(GetModuleHandleW(L"explorerframe.dll"), id, buf, 256) > 0 ? buf : def;
}

void SyncMenu(HMENU m) {
    LoadSettings(); 
    if(!HasId(m, CMD_TOGGLE_SEARCHBAND)) {
        InsertMenuW(m, 0, MF_BYPOSITION|MF_STRING, CMD_TOGGLE_UPBUTTON, GetLocalStr(STR_ID_UPBUTTON, L"Up").c_str());
        InsertMenuW(m, 0, MF_BYPOSITION|MF_STRING, CMD_TOGGLE_BREADCRUMB, GetLocalStr(STR_ID_BREADCRUMB, L"Address").c_str());
        InsertMenuW(m, 0, MF_BYPOSITION|MF_STRING, CMD_TOGGLE_SEARCHBAND, GetLocalStr(STR_ID_SEARCHBAR, L"Search Bar").c_str());
    }
    CheckMenuItem(m, CMD_TOGGLE_SEARCHBAND, MF_BYCOMMAND | (g_set.s ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(m, CMD_TOGGLE_BREADCRUMB, MF_BYCOMMAND | (g_set.b ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(m, CMD_TOGGLE_UPBUTTON, MF_BYCOMMAND | (g_set.u ? MF_CHECKED : MF_UNCHECKED));
}

BOOL HandleMenuResult(UINT cmd, bool isRet, HWND hw) {
    if(cmd>=CMD_TOGGLE_SEARCHBAND && cmd<=CMD_TOGGLE_UPBUTTON) {
        bool& s = (cmd==CMD_TOGGLE_SEARCHBAND)?g_set.s:(cmd==CMD_TOGGLE_BREADCRUMB)?g_set.b:g_set.u;
        s = !s; 
        Wh_SetIntValue((cmd==CMD_TOGGLE_SEARCHBAND)?L"MoveSearchBand":(cmd==CMD_TOGGLE_BREADCRUMB)?L"MoveBreadcrumb":L"MoveUpButton", s?1:0);
        PostMessage(HWND_BROADCAST, g_msgSyncSettings, 0, 0); // Синхронизируем все окна мгновенно
        return isRet ? 0 : TRUE;
    }
    if(!isRet && cmd && hw) PostMessage(hw, WM_COMMAND, MAKEWPARAM(LOWORD(cmd), 0), 0);
    return isRet ? cmd : TRUE;
}

using TPM_t = BOOL(WINAPI*)(HMENU,UINT,int,int,int,HWND,CONST RECT*); TPM_t origTPM;
BOOL WINAPI Hook_TPM(HMENU m,UINT f,int x,int y,int r,HWND hw,CONST RECT* pr) {
    if(!HasId(m, 41484)) return origTPM(m, f, x, y, r, hw, pr);
    SyncMenu(m); bool isRet = f & TPM_RETURNCMD;
    UINT cmd = (UINT)origTPM(m, f | TPM_RETURNCMD, x, y, r, hw, pr);
    return HandleMenuResult(cmd, isRet, hw);
}

using TPMEx_t = BOOL(WINAPI*)(HMENU,UINT,int,int,HWND,LPTPMPARAMS); TPMEx_t origTPMEx;
BOOL WINAPI Hook_TPMEx(HMENU m,UINT f,int x,int y,HWND hw,LPTPMPARAMS p) {
    if(!HasId(m, 41484)) return origTPMEx(m, f, x, y, hw, p);
    SyncMenu(m); bool isRet = f & TPM_RETURNCMD;
    UINT cmd = (UINT)origTPMEx(m, f | TPM_RETURNCMD, x, y, hw, p);
    return HandleMenuResult(cmd, isRet, hw);
}

// Core Band Logic
HWND GetBandChild(HWND rb, int flag) {
    HWND res=NULL; 
    EnumBands(rb, RBBIM_CHILD, [&](int, REBARBANDINFO& rbi){ 
        if(rbi.hwndChild && !res && (((int)(INT_PTR)GetPropW(rbi.hwndChild, L"FlexTbFlag")) & flag)) res=rbi.hwndChild; 
    });
    return res;
}

HWND GetHiddenBand(HWND cab, BandType type) {
    LPCWSTR prop = (type==BandType::Search)?L"FlexTbRmSearch":(type==BandType::Breadcrumb)?L"FlexTbRmBread":L"FlexTbRmUp";
    return (HWND)GetPropW(cab, prop);
}
void SetHiddenBand(HWND cab, BandType type, HWND ch) {
    LPCWSTR prop = (type==BandType::Search)?L"FlexTbRmSearch":(type==BandType::Breadcrumb)?L"FlexTbRmBread":L"FlexTbRmUp";
    if(ch) SetPropW(cab, prop, (HANDLE)ch); else RemovePropW(cab, prop);
}

void ToggleBand(HWND cab, BandType type, bool enable) {
    int f = (type==BandType::Search)?CF_SEARCH:(type==BandType::Breadcrumb)?CF_BREADCRUMB:CF_UPBUTTON;
    HWND mr = (HWND)GetPropW(cab, L"FlexTbRb"); if(!mr || !IsWindow(mr)) return;
    HWND ch = GetBandChild(mr, f);
    
    if(!enable && ch) {
        SaveBandPositions(mr, false); // Сохраняем текущий порядок ДО удаления
        int foundIdx = -1;
        EnumBands(mr, RBBIM_CHILD, [&](int i, REBARBANDINFO& rbi){ if(rbi.hwndChild==ch) foundIdx=i; });
        if(foundIdx != -1) SendMessage(mr, RB_DELETEBAND, foundIdx, 0);
        
        SetPropW(ch, L"FlexTbIsHidden", (HANDLE)1);
        ShowWindow(ch, SW_HIDE); 
        SetHiddenBand(cab, type, ch); 
        ForceCabinetRelayout(cab);
        SaveBandPositions(mr, false); // Обновляем порядок оставшихся панелей
    } else if(enable && !ch) {
        ch = GetHiddenBand(cab, type); if(!ch || !IsWindow(ch)) return;
        SetHiddenBand(cab, type, NULL);
        SetParent(ch, mr);
        
        WCHAR c[256]; GetEffClass(ch, c, 256); BandState bs; LoadBandState(c, bs);
        if(type==BandType::UpButton) { SendMessage(ch, TB_SETBITMAPSIZE, 0, MAKELONG(16,16)); SendMessage(ch, TB_SETPADDING, 0, MAKELONG(4,4)); SendMessage(ch, TB_AUTOSIZE, 0, 0); }
        int h = GetSystemMetrics(SM_CYSIZE) + GetSystemMetrics(SM_CYBORDER)*2 + 2;
        
        REBARBANDINFO rbi={sizeof(rbi)}; rbi.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_SIZE|RBBIM_IDEALSIZE;
        rbi.fStyle = GetRefGripper(mr) | (bs.brk ? RBBS_BREAK : 0);
        rbi.hwndChild=ch; rbi.cyMinChild=rbi.cyMaxChild=rbi.cyChild=h; rbi.cx=rbi.cxIdeal=bs.cx; rbi.cyIntegral=1;
        
        if(SendMessage(mr, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbi)) {
            SetPropW(ch, L"FlexTbFlag", (HANDLE)(INT_PTR)(CF_MOVED | f));
            if(f != CF_SEARCH) HookWindow(ch, Tbar_Proc); 
            RemovePropW(ch, L"FlexTbIsHidden");
            ShowWindow(ch, SW_SHOW);
            ApplySavedLayout(mr); 
            SyncGrippers(mr); 
            ForceCabinetRelayout(cab); 
            PostMessage(cab, g_msgFixContent, (WPARAM)ch, 0);
            SaveBandPositions(mr, false); // Сохраняем НОВУЮ позицию после добавления! (только позицию, не ширину)
        }
    }
}

// Subclasses main UI
LRESULT CALLBACK Rb_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if(m==WM_NCDESTROY) { { Lock L; g_hooks.erase(h); } return DefSubclassProc(h,m,w,l); }
    
    // ДОБАВЛЕНО: Контекстное меню самого ReBar'а (решает проблему с полем поиска и открытыми окнами)
    if(m == WM_CONTEXTMENU) {
        POINT pt = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
        if(pt.x==-1 && pt.y==-1) { RECT rc; GetWindowRect(h, &rc); pt.x=rc.left; pt.y=rc.bottom; }
        if(HWND cab = GetCabinet(h)) {
            if(HWND ww = FindByClass(FindByClass(cab,L"ShellTabWindowClass"),L"WorkerW")) {
                if(HMENU hM = LoadMenuW(GetModuleHandleW(L"explorerframe.dll"), MAKEINTRESOURCEW(264))) {
                    if(HMENU sub = GetSubMenu(hM, 0)) {
                        TrackPopupMenuEx(sub, TPM_RIGHTBUTTON|TPM_LEFTBUTTON, pt.x, pt.y, ww, NULL);
                        PostMessage(ww, WM_NULL, 0, 0);
                    } DestroyMenu(hM);
                }
            }
        }
        return 0;
    }

    if(m==RB_SETBANDINFO) {
        auto* inf=(REBARBANDINFO*)l;
        if(inf && (inf->fMask&RBBIM_CHILDSIZE)) {
            HWND ch = (inf->fMask&RBBIM_CHILD) ? inf->hwndChild : [&](){ REBARBANDINFO q={sizeof(q)}; q.fMask=RBBIM_CHILD; return SendMessage(h,RB_GETBANDINFO,w,(LPARAM)&q)?q.hwndChild:NULL; }();
            if(ch && ((int)(INT_PTR)GetPropW(ch, L"FlexTbFlag") & CF_MOVED)) { inf->cyMinChild=inf->cyChild=inf->cyMaxChild=GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYBORDER)*2+2; inf->cyIntegral=1; }
        }
    }
    g_rbLayoutDepth++; LRESULT r=DefSubclassProc(h,m,w,l); g_rbLayoutDepth--;
    
    if(m==WM_SIZE && GetPropW(h, L"FlexTbPendApply") && !g_inApply) {
        int a = (int)(INT_PTR)GetPropW(h, L"FlexTbApplyAtm"); 
        if(a<5) { SetPropW(h, L"FlexTbApplyAtm", (HANDLE)(INT_PTR)(a+1)); ReapplyCx(h); } 
        else RemovePropW(h, L"FlexTbPendApply");
    }
    if(m==RB_INSERTBAND) if(HWND cab=GetCabinet(h)) if(!GetPropW(cab, L"FlexTbMoved")) PostMessage(cab,g_msgDoMove,0,0);
    if(m==RB_SETBANDINFO && !g_inSync) if(HWND cab=GetCabinet(h)) if(GetPropW(cab, L"FlexTbMoved")) SyncGrippers(h);
    if(m==WM_MOUSEMOVE || m==WM_LBUTTONUP) {
        HWND cab=(HWND)GetPropW(h, L"FlexTbCab"); if(!cab) cab = GetCabinet(h);
        if(cab && GetPropW(cab, L"FlexTbMoved") && !g_inApply) {
            static DWORD ls=0; DWORD n=GetTickCount(); if(m==WM_LBUTTONUP || (n-ls>1000)) { SaveBandPositions(h); ls=n; }
        }
    }
    return r;
}

LRESULT CALLBACK ParentRb_Proc(HWND hh,UINT mm,WPARAM ww,LPARAM ll,DWORD_PTR) {
    if(mm==WM_NCDESTROY) { { Lock L; g_hooks.erase(hh); } return DefSubclassProc(hh,mm,ww,ll); }
    if(mm==WM_NOTIFY && (((NMHDR*)ll)->code==RBN_LAYOUTCHANGED || ((NMHDR*)ll)->code==RBN_ENDDRAG)) {
        HWND c = (HWND)GetPropW(((NMHDR*)ll)->hwndFrom, L"FlexTbCab"); if(!c) c = GetCabinet(((NMHDR*)ll)->hwndFrom);
        if(c && GetPropW(c, L"FlexTbMoved") && !g_inApply) SaveBandPositions(((NMHDR*)ll)->hwndFrom);
    }
    return DefSubclassProc(hh,mm,ww,ll);
}

BOOL CALLBACK EnumBreadcrumb_Proc(HWND ch,LPARAM lp) { 
    WCHAR c[64]; if(GetClassName(ch,c,64)&&!wcscmp(c,L"Breadcrumb Parent")){*(HWND*)lp=ch; return FALSE;} return TRUE; 
}

LRESULT CALLBACK Cab_Proc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if(m==WM_NCDESTROY) { { Lock L; g_hooks.erase(h); } return DefSubclassProc(h,m,w,l); }
    if(m==WM_CLOSE||m==WM_DESTROY) {
        if(GetPropW(h, L"FlexTbMoved")) {
            HWND mr = (HWND)GetPropW(h, L"FlexTbRb");
            if(mr) SaveBandPositions(mr);
        }
    }
    if(m==g_msgSyncSettings) {
        LoadSettings();
        if(GetPropW(h, L"FlexTbMoved")) {
            ToggleBand(h, BandType::Search, g_set.s);
            ToggleBand(h, BandType::Breadcrumb, g_set.b);
            ToggleBand(h, BandType::UpButton, g_set.u);
        }
        return 0;
    }
    if(m==g_msgFixContent) {
        HWND ch=(HWND)w; RECT rc; 
        if(GetClientRect(ch,&rc)) {
            int cw = rc.right-rc.left, ch_h = rc.bottom-rc.top;
            if(cw > 0 && ch_h > 0) { 
                SendMessage(ch,WM_SIZE,SIZE_RESTORED,MAKELPARAM(cw, ch_h>4 ? ch_h-2 : ch_h+2));
                SendMessage(ch,TB_AUTOSIZE,0,0);
                SendMessage(ch,WM_SIZE,SIZE_RESTORED,MAKELPARAM(cw, ch_h));
                SendMessage(ch,TB_AUTOSIZE,0,0);
                InvalidateRect(ch,NULL,TRUE); RedrawWindow(ch,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
            }
        }
        return 0;
    }
    if(m==g_msgDoMove) {
        if(GetPropW(h, L"FlexTbMoved")) return 1;
        HWND mRb = FindByClass(FindByClass(FindByClass(h,L"ShellTabWindowClass"),L"WorkerW"),L"ReBarWindow32");
        HWND nRb = FindNavRb(h); if(!mRb || !nRb) return 0;
        
        SetWindowLongPtr(mRb,GWL_STYLE, (GetWindowLongPtr(mRb,GWL_STYLE)&~RBS_FIXEDORDER)|RBS_VARHEIGHT);
        HookWindow(GetParent(mRb), ParentRb_Proc);
        SetPropW(mRb, L"FlexTbCab", (HANDLE)h); SetPropW(h, L"FlexTbRb", (HANDLE)mRb);
        
        struct BTM{int idx; HWND c; HWND left; int w; int type;}; std::vector<BTM> mv;
        EnumBands(nRb, RBBIM_CHILD, [&](int i, REBARBANDINFO& rbi){
            if(!rbi.hwndChild) return;
            if(ContainsClass(rbi.hwndChild, L"UniversalSearchBand")||ContainsClass(rbi.hwndChild,L"Search Box")) {
                RECT rc; GetWindowRect(rbi.hwndChild,&rc); int w = rc.right-rc.left; mv.push_back({i, rbi.hwndChild, NULL, w<200?200:w, CF_SEARCH});
            } else if(ContainsClass(rbi.hwndChild, L"Address Band Root")) {
                HWND b = rbi.hwndChild; WCHAR c[64];
                if(!GetClassName(b,c,64) || wcscmp(c,L"Breadcrumb Parent")!=0) b = NULL; EnumChildWindows(rbi.hwndChild, EnumBreadcrumb_Proc, (LPARAM)&b);
                if(b) if(HWND t = FindByClass(b, L"ToolbarWindow32")) { RECT rc; GetWindowRect(t,&rc); int w = rc.right-rc.left; mv.push_back({i, t, b, w<250?250:w, CF_BREADCRUMB}); }
            } else if(ContainsClass(rbi.hwndChild, L"UpBand")) {
                if(HWND t = FindByClass(rbi.hwndChild, L"ToolbarWindow32")) { RECT rc; GetWindowRect(t,&rc); int w = rc.right-rc.left; mv.push_back({i, t, rbi.hwndChild, w<30?30:w, CF_UPBUTTON}); }
            }
        });

        if(!mv.empty()) {
            for(auto it=mv.rbegin(); it!=mv.rend(); ++it) {
                SendMessage(nRb,RB_DELETEBAND,it->idx,0);
                if(it->left && IsWindow(it->left)) { SetPropW(it->left, L"FlexTbNeutered", (HANDLE)1); ShowWindow(it->left,SW_HIDE); }
            }
            int h2 = GetSystemMetrics(SM_CYSIZE) + GetSystemMetrics(SM_CYBORDER)*2 + 2;
            for(auto& b : mv) {
                SetParent(b.c, mRb);
                WCHAR c[256]=L""; 
                if(b.type==CF_UPBUTTON) wcsncpy(c,L"UpButtonToolbar",256); else if(b.type==CF_BREADCRUMB) wcsncpy(c,L"BreadcrumbToolbar",256); else GetClassName(b.c,c,256);
                
                BandState bs; bool hasSv = LoadBandState(c, bs);
                if(b.type==CF_UPBUTTON) { SendMessage(b.c, TB_SETBITMAPSIZE, 0, MAKELONG(16,16)); SendMessage(b.c, TB_SETPADDING, 0, MAKELONG(4,4)); SendMessage(b.c, TB_AUTOSIZE, 0, 0); }
                
                REBARBANDINFO r={sizeof(r)}; r.fMask=RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_SIZE|RBBIM_IDEALSIZE;
                r.fStyle = GetRefGripper(mRb) | ((hasSv?bs.brk:true)?RBBS_BREAK:0); r.hwndChild=b.c;
                r.cyMinChild=r.cyMaxChild=r.cyChild=h2; r.cx=r.cxIdeal=hasSv?bs.cx:(UINT)b.w; r.cyIntegral=1;
                
                if(SendMessage(mRb, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&r)) {
                    SetPropW(b.c, L"FlexTbFlag", (HANDLE)(INT_PTR)(CF_MOVED | b.type)); ShowWindow(b.c, SW_SHOW);
                    if(b.type!=CF_SEARCH) HookWindow(b.c, Tbar_Proc);
                }
            }
            ApplySavedLayout(mRb); SyncGrippers(mRb);
        }
        ForceHideWorker(GetParent(nRb)); SetPropW(h, L"FlexTbMoved", (HANDLE)1); ForceCabinetRelayout(h);
        for(auto& b : mv) PostMessage(h, g_msgFixContent, (WPARAM)b.c, 0);
        
        if(!g_set.s) ToggleBand(h, BandType::Search, false);
        if(!g_set.b) ToggleBand(h, BandType::Breadcrumb, false);
        if(!g_set.u) ToggleBand(h, BandType::UpButton, false);
        
        SaveBandPositions(mRb, false); // Сохраняем исходные позиции загруженных панелей
        return 0;
    }
    if((m==WM_ACTIVATE||m==WM_SETFOCUS) && !GetPropW(h, L"FlexTbMoved")) PostMessage(h,g_msgDoMove,0,0);
    return DefSubclassProc(h,m,w,l);
}

LRESULT CALLBACK ShellTab_Proc(HWND hh,UINT mm,WPARAM ww,LPARAM ll,DWORD_PTR) {
    if(mm==WM_NCDESTROY) { { Lock L; g_hooks.erase(hh); } return DefSubclassProc(hh,mm,ww,ll); }
    if(mm==WM_WINDOWPOSCHANGING && GetPropW(GetParent(hh), L"FlexTbMoved")) {
        auto*p=(WINDOWPOS*)ll; RECT rc; GetClientRect(GetParent(hh),&rc);
        p->x=p->y=0; p->cx=rc.right; p->cy=rc.bottom; p->flags=(p->flags&~(SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW))|SWP_NOZORDER|SWP_NOACTIVATE;
    } return DefSubclassProc(hh,mm,ww,ll);
}

LRESULT CALLBACK WorkerW_Proc(HWND hh,UINT mm,WPARAM ww,LPARAM ll,DWORD_PTR) {
    if(mm==WM_NCDESTROY) { RemovePropW(hh, L"FlexTbForceHidden"); { Lock L; g_hooks.erase(hh); } return DefSubclassProc(hh,mm,ww,ll); }
    if(GetPropW(hh, L"FlexTbForceHidden")) {
        if(mm==WM_STYLECHANGING && ww==GWL_STYLE) ((STYLESTRUCT*)ll)->styleNew &= ~WS_VISIBLE;
        else if(mm==WM_WINDOWPOSCHANGING) { auto*p=(WINDOWPOS*)ll; p->flags=(p->flags&~SWP_SHOWWINDOW)|SWP_HIDEWINDOW; p->cx=p->cy=0; }
        else if(mm==WM_SHOWWINDOW && ww) ShowWindow(hh, SW_HIDE);
        else if(mm==WM_SIZE||mm==WM_MOVE) return 0;
    } return DefSubclassProc(hh,mm,ww,ll);
}

LRESULT CALLBACK AddrBand_Proc(HWND hh,UINT mm,WPARAM ww,LPARAM ll,DWORD_PTR) { 
    if(mm==WM_NCDESTROY) { { Lock L; g_hooks.erase(hh); } return DefSubclassProc(hh,mm,ww,ll); }
    return GetPropW(hh, L"FlexTbNeutered") ? DefWindowProc(hh,mm,ww,ll) : DefSubclassProc(hh,mm,ww,ll); 
}

void ProcessWnd(HWND h) {
    if(!h || !IsWindow(h)) return; WCHAR c[256]; if(!GetClassName(h,c,256)) return;
    if(!wcscmp(c,L"CabinetWClass")) HookWindow(h, Cab_Proc);
    else if(!wcscmp(c,L"ShellTabWindowClass") && GetCabinet(h)) HookWindow(h, ShellTab_Proc);
    else if(!wcscmp(c,L"WorkerW") && GetCabinet(h)) { HookWindow(h, WorkerW_Proc); ForceHideWorker(h); }
    else if(!wcscmp(c,L"ReBarWindow32")) HookWindow(h, Rb_Proc);
    else if(!wcscmp(c,L"Address Band Root")) HookWindow(h, AddrBand_Proc);
}

BOOL CALLBACK EnumProcessWnd_Proc(HWND ch,LPARAM) { ProcessWnd(ch); return TRUE; }

using CWExW_t=decltype(&CreateWindowExW); CWExW_t origCWExW;
HWND WINAPI Hook_CWExW(DWORD s,LPCWSTR c,LPCWSTR wn,DWORD st,int X,int Y,int W,int H,HWND p,HMENU m,HINSTANCE h,LPVOID lp) {
    HWND hw=origCWExW(s,c,wn,st,X,Y,W,H,p,m,h,lp);
    if(hw&&c&&!IS_INTRESOURCE(c)) { ProcessWnd(hw); EnumChildWindows(hw, EnumProcessWnd_Proc, 0); }
    return hw;
}

using NtSet_t=NTSTATUS(NTAPI*)(HANDLE,PUNICODE_STRING,ULONG,ULONG,PVOID,ULONG); NtSet_t origNtSet;
NTSTATUS NTAPI Hook_NtSet(HANDLE k,PUNICODE_STRING v,ULONG ti,ULONG t,PVOID d,ULONG ds) {
    if(v && v->Buffer && v->Length && (v->Length/2)==12) {
        bool match=true; LPCWSTR tgt=L"ITBar7Layout";
        for(int i=0;i<12;i++) if(towlower(v->Buffer[i])!=towlower(tgt[i])) match=false;
        if(match) return 0;
    } return origNtSet(k,v,ti,t,d,ds);
}

BOOL Wh_ModInit() {
    LoadSettings(); InitializeCriticalSection(&g_cs);
    g_msgDoMove = RegisterWindowMessage(L"FlexExpTb_DoMove"); 
    g_msgFixContent = RegisterWindowMessage(L"FlexExpTb_Fix");
    g_msgSyncSettings = RegisterWindowMessage(L"FlexExpTb_SyncSettings");
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)Hook_CWExW, (void**)&origCWExW);
    Wh_SetFunctionHook((void*)TrackPopupMenu, (void*)Hook_TPM, (void**)&origTPM);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)Hook_TPMEx, (void**)&origTPMEx);
    Wh_SetFunctionHook((void*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtSetValueKey"), (void*)Hook_NtSet, (void**)&origNtSet);
    for(HWND w=GetTopWindow(NULL); w; w=GetNextWindow(w,GW_HWNDNEXT)) {
        DWORD p=0; GetWindowThreadProcessId(w,&p); if(p==GetCurrentProcessId() && GetCabinet(w)) {
            ProcessWnd(w); EnumChildWindows(w, EnumProcessWnd_Proc, 0); PostMessage(w,g_msgDoMove,0,0);
        }
    } return TRUE;
}

void Wh_ModUninit() {
    std::vector<std::pair<HWND, WindhawkUtils::WH_SUBCLASSPROC>> hooksToClean;
    { Lock l; for (auto& pair : g_hooks) hooksToClean.push_back(pair); g_hooks.clear(); }
    for (auto& pair : hooksToClean) if (IsWindow(pair.first)) WindhawkUtils::RemoveWindowSubclassFromAnyThread(pair.first, pair.second);
    DeleteCriticalSection(&g_cs);
}
