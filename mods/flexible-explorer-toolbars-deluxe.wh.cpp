// ==WindhawkMod==
// @id              flexible-explorer-toolbars-deluxe
// @name            Flexible Explorer Toolbars Deluxe
// @description     Makes Search Bar, Broeadcrumb Bar and others into movable toolbars
// @version         1.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -luxtheme -lgdi32 -ladvapi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- MoveSearchBand: true
  $name: Show Search Bar
- MoveBreadcrumb: false
  $name: Show Breadcrumb Bar
- MoveUpButton: true
  $name: Show Up Button
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*

# Flexible Explorer Toolbars Deluxe

**!Important!** This mod curently only supports Windows 10 or Windows 11 versions 23H2 or below. Suport for later versions is planed.

**!Important!** To use this mod, you shoud disable any other mods that hide the clasic Navigation bar, such as the `Disable Navigation Bar` mod by ItsProfessional.
This mod hides the Navication Bar by itself. It is recommended to install the `Windows 7 Comand Bar` mod.

This mod hides the Navigation Bar and instead creates the following optional toolbars, which could be freely moved and ordered together with the Menu Bar, if it is enabled:

* The Search bar

* The Breadcrumbs Bar

* The Up Buton

The toolbars can be locked and unlocked.

# Further adjustments

* To make the toolbars to have the 3D borders, install this mod: [Separators around File Explorer toolbars](https://windhawk.net/mods/explorer-toolbars-separators).

* To fix appearance of the default text in the search bar under dark Classic theme, install this mod: [Classic Theme Explorer Search Fix](https://windhawk.net/mods/classic-theme-explorer-search-fix).

![screnshot](https://i.imgur.com/1YbTzZt.png)

![screnshot](https://i.imgur.com/OV8NRKJ.png)

![screnshot](https://i.imgur.com/OEthKme.png)

![screnshot](https://i.imgur.com/JXKEXL1.png)

![screnshot](https://i.imgur.com/QFFmczo.png)

*/

// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <winternl.h>
#include <commctrl.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

constexpr int UP_BUTTON_ICON_SIZE  = 16;
constexpr UINT WM_APP_DO_MOVE      = WM_APP + 0x501;
constexpr LPCWSTR REG_KEY          = L"Software\\WindhawkMods\\FlexibleExplorer";

struct Settings {
    bool moveSearchBand    = true;
    bool moveBreadcrumb    = true;
    bool moveUpButton      = true;
} g_settings;

void LoadSettings() {
    g_settings.moveSearchBand = Wh_GetIntSetting(L"MoveSearchBand");
    g_settings.moveBreadcrumb = Wh_GetIntSetting(L"MoveBreadcrumb");
    g_settings.moveUpButton   = Wh_GetIntSetting(L"MoveUpButton");
}
void Wh_ModSettingsChanged() { LoadSettings(); }

CRITICAL_SECTION g_mutex;

std::unordered_map<HWND,bool>    g_alreadyMoved;
std::unordered_map<HWND,bool>    g_forceHiddenWorkers;
std::unordered_set<HWND>         g_neuteredAddressRoots;
std::unordered_map<HWND,HWND>    g_rebarToCabinet;
std::unordered_map<HWND,HWND>    g_cabinetToMenuRebar;
std::unordered_set<HWND>         g_pendingApply;
std::unordered_map<HWND,int>     g_applyAttempts;

enum ChildFlag { CF_MOVED=1, CF_UPBUTTON=2, CF_BREADCRUMB=4 };
std::unordered_map<HWND,int> g_childFlags;

struct ToolbarGuard{int x=0,y=0,cx=0,cy=0;bool hasGood=false;};
std::unordered_map<HWND,ToolbarGuard> g_toolbarGuards;

thread_local bool g_insideApply = false;
thread_local int  g_rebarLayoutDepth = 0;
thread_local bool g_insideGripperSync = false;
bool IsInsideRebarLayout() { return g_rebarLayoutDepth > 0; }

void HookWindow(HWND hwnd, WindhawkUtils::WH_SUBCLASSPROC subclassProc);

int GetDesiredBandHeight() {
    return GetSystemMetrics(SM_CYSIZE) + GetSystemMetrics(SM_CYBORDER) * 2 + 2;
}

// =====================================================================
// NTDLL registry hooks
// =====================================================================

using NtSetValueKey_t = NTSTATUS(NTAPI*)(
    HANDLE KeyHandle,
    PUNICODE_STRING ValueName,
    ULONG TitleIndex,
    ULONG Type,
    PVOID Data,
    ULONG DataSize);

NtSetValueKey_t   NtSetValueKey_Original   = nullptr;

bool UnicodeStringEqualsIgnoreCase(PUNICODE_STRING vn, const wchar_t* target) {
    if (!vn || !vn->Buffer || vn->Length == 0) return false;
    size_t lenChars = vn->Length / sizeof(WCHAR);
    size_t tlen = wcslen(target);
    if (lenChars != tlen) return false;
    for (size_t i = 0; i < tlen; i++) {
        if (towlower(vn->Buffer[i]) != towlower(target[i])) return false;
    }
    return true;
}

NTSTATUS NTAPI NtSetValueKey_Hook(
    HANDLE KeyHandle,
    PUNICODE_STRING ValueName,
    ULONG TitleIndex,
    ULONG Type,
    PVOID Data,
    ULONG DataSize)
{
    if (!ValueName||UnicodeStringEqualsIgnoreCase(ValueName, L"ITBar7Layout")) {return 0;}
    return NtSetValueKey_Original(KeyHandle, ValueName, TitleIndex, Type, Data, DataSize);
}

// =====================================================================
// Accessors
// =====================================================================
bool WasAlreadyMoved(HWND c){EnterCriticalSection(&g_mutex);bool r=g_alreadyMoved.count(c)&&g_alreadyMoved[c];LeaveCriticalSection(&g_mutex);return r;}
void MarkMoved(HWND c){EnterCriticalSection(&g_mutex);g_alreadyMoved[c]=true;LeaveCriticalSection(&g_mutex);}
bool IsForceHidden(HWND w){EnterCriticalSection(&g_mutex);bool r=g_forceHiddenWorkers.count(w)&&g_forceHiddenWorkers[w];LeaveCriticalSection(&g_mutex);return r;}
void MarkForceHidden(HWND w){EnterCriticalSection(&g_mutex);g_forceHiddenWorkers[w]=true;LeaveCriticalSection(&g_mutex);}
void MarkNeutered(HWND h){EnterCriticalSection(&g_mutex);g_neuteredAddressRoots.insert(h);LeaveCriticalSection(&g_mutex);}
bool IsNeutered(HWND h){EnterCriticalSection(&g_mutex);bool r=g_neuteredAddressRoots.count(h)!=0;LeaveCriticalSection(&g_mutex);return r;}
void RegisterRebarCabinet(HWND r,HWND c){EnterCriticalSection(&g_mutex);g_rebarToCabinet[r]=c;LeaveCriticalSection(&g_mutex);}
HWND GetRebarCabinet(HWND r){EnterCriticalSection(&g_mutex);auto it=g_rebarToCabinet.find(r);HWND c=it!=g_rebarToCabinet.end()?it->second:NULL;LeaveCriticalSection(&g_mutex);return c;}
void RegisterCabinetMenuRebar(HWND cab,HWND r){EnterCriticalSection(&g_mutex);g_cabinetToMenuRebar[cab]=r;LeaveCriticalSection(&g_mutex);}
HWND GetCabinetMenuRebar(HWND cab){EnterCriticalSection(&g_mutex);auto it=g_cabinetToMenuRebar.find(cab);HWND r=it!=g_cabinetToMenuRebar.end()?it->second:NULL;LeaveCriticalSection(&g_mutex);return r;}
void MarkPendingApply(HWND r){EnterCriticalSection(&g_mutex);g_pendingApply.insert(r);LeaveCriticalSection(&g_mutex);}
bool IsPendingApply(HWND r){EnterCriticalSection(&g_mutex);bool v=g_pendingApply.count(r)!=0;LeaveCriticalSection(&g_mutex);return v;}
void ClearPendingApply(HWND r){EnterCriticalSection(&g_mutex);g_pendingApply.erase(r);LeaveCriticalSection(&g_mutex);}

void SetChildFlag(HWND h,int f){EnterCriticalSection(&g_mutex);g_childFlags[h]|=f;LeaveCriticalSection(&g_mutex);}
void ClearChildFlag(HWND h,int f){
    EnterCriticalSection(&g_mutex);
    auto it=g_childFlags.find(h);
    if(it!=g_childFlags.end()){it->second&=~f;if(!it->second)g_childFlags.erase(it);}
    LeaveCriticalSection(&g_mutex);
}
bool HasChildFlag(HWND h,int f){EnterCriticalSection(&g_mutex);auto it=g_childFlags.find(h);bool r=it!=g_childFlags.end()&&(it->second&f);LeaveCriticalSection(&g_mutex);return r;}
void ClearAllChildFlags(HWND h){EnterCriticalSection(&g_mutex);g_childFlags.erase(h);LeaveCriticalSection(&g_mutex);}

void CleanupCabinetMovedChildren(HWND cab){
    HWND mr=GetCabinetMenuRebar(cab);
    if(!mr||!IsWindow(mr))return;
    int cnt=(int)SendMessage(mr,RB_GETBANDCOUNT,0,0);
    for(int i=0;i<cnt;i++){
        REBARBANDINFO rbi={sizeof(rbi)};rbi.fMask=RBBIM_CHILD;
        if(!SendMessage(mr,RB_GETBANDINFO,i,(LPARAM)&rbi)||!rbi.hwndChild)continue;
        ClearAllChildFlags(rbi.hwndChild);
    }
}

void GetEffectiveClassName(HWND child, wchar_t* out, size_t outCount) {
    if (child && HasChildFlag(child, CF_UPBUTTON)) {
        wcsncpy_s(out, outCount, L"UpButtonToolbar", _TRUNCATE);
    } else if (child && HasChildFlag(child, CF_BREADCRUMB)) {
        wcsncpy_s(out, outCount, L"BreadcrumbToolbar", _TRUNCATE);
    } else if (child) {
        GetClassName(child, out, (int)outCount);
    } else {
        out[0] = L'\0';
    }
}

// =====================================================================
// Registry
// =====================================================================
struct BandState{UINT cx;bool brk;};

void SaveBandPositions(HWND rebar){
    if(!rebar||!IsWindow(rebar))return;
    HKEY k;
    if(RegCreateKeyExW(HKEY_CURRENT_USER,REG_KEY,0,NULL,0,KEY_WRITE,NULL,&k,NULL)!=ERROR_SUCCESS)return;
    int cnt=(int)SendMessage(rebar,RB_GETBANDCOUNT,0,0);
    DWORD dc=(DWORD)cnt;
    RegSetValueExW(k,L"BandCount",0,REG_DWORD,(BYTE*)&dc,sizeof(dc));
    for(int i=0;i<cnt;i++){
        REBARBANDINFO rbi={sizeof(rbi)};rbi.fMask=RBBIM_SIZE|RBBIM_STYLE|RBBIM_CHILD;
        if(!SendMessage(rebar,RB_GETBANDINFO,i,(LPARAM)&rbi))continue;
        WCHAR cls[256]=L"";
        if(rbi.hwndChild&&IsWindow(rbi.hwndChild))GetEffectiveClassName(rbi.hwndChild,cls,ARRAYSIZE(cls));
        WCHAR ok[64];wsprintf(ok,L"Order_%d",i);
        RegSetValueExW(k,ok,0,REG_SZ,(BYTE*)cls,(DWORD)((wcslen(cls)+1)*sizeof(WCHAR)));
        WCHAR ck[128];wsprintf(ck,L"Cx_%s",cls);
        DWORD cx=rbi.cx;RegSetValueExW(k,ck,0,REG_DWORD,(BYTE*)&cx,sizeof(cx));
        WCHAR bk[128];wsprintf(bk,L"Break_%s",cls);
        DWORD brk=(rbi.fStyle&RBBS_BREAK)?1u:0u;
        RegSetValueExW(k,bk,0,REG_DWORD,(BYTE*)&brk,sizeof(brk));
    }
    RegCloseKey(k);
}

bool LoadBandState(const wchar_t* cls,BandState& out){
    HKEY k;
    if(RegOpenKeyExW(HKEY_CURRENT_USER,REG_KEY,0,KEY_READ,&k)!=ERROR_SUCCESS)return false;
    WCHAR ck[128];wsprintf(ck,L"Cx_%s",cls);
    WCHAR bk[128];wsprintf(bk,L"Break_%s",cls);
    DWORD cx=0,brk=0,sz=sizeof(DWORD),t;
    bool ok=RegQueryValueExW(k,ck,NULL,&t,(BYTE*)&cx,&sz)==ERROR_SUCCESS;
    sz=sizeof(DWORD);RegQueryValueExW(k,bk,NULL,&t,(BYTE*)&brk,&sz);
    RegCloseKey(k);
    if(ok&&cx>=20&&cx<=8000){out.cx=cx;out.brk=brk!=0;return true;}
    return false;
}

std::vector<std::wstring> LoadBandOrder(){
    std::vector<std::wstring> order;
    HKEY k;
    if(RegOpenKeyExW(HKEY_CURRENT_USER,REG_KEY,0,KEY_READ,&k)!=ERROR_SUCCESS)return order;
    DWORD cnt=0,sz=sizeof(cnt),t;
    if(RegQueryValueExW(k,L"BandCount",NULL,&t,(BYTE*)&cnt,&sz)!=ERROR_SUCCESS){RegCloseKey(k);return order;}
    for(DWORD i=0;i<cnt;i++){
        WCHAR ok[64];wsprintf(ok,L"Order_%u",i);
        WCHAR val[256]=L"";DWORD vsz=sizeof(val);
        if(RegQueryValueExW(k,ok,NULL,&t,(BYTE*)val,&vsz)==ERROR_SUCCESS)order.push_back(val);
    }
    RegCloseKey(k);return order;
}

void ResizeUpButtonToolbar(HWND toolbar) {
    if (!toolbar || !IsWindow(toolbar)) return;
    SendMessage(toolbar, TB_SETBITMAPSIZE, 0, MAKELONG(UP_BUTTON_ICON_SIZE, UP_BUTTON_ICON_SIZE));
    SendMessage(toolbar, TB_SETPADDING, 0, MAKELONG(4, 4));
    SendMessage(toolbar, TB_AUTOSIZE, 0, 0);
}

// =====================================================================
// ApplySavedLayout + ReapplyCx
// =====================================================================
void ReapplyCx(HWND rebar){
    ClearPendingApply(rebar);
    int cnt=(int)SendMessage(rebar,RB_GETBANDCOUNT,0,0);
    g_insideApply=true;
    bool anyChanged=false;
    for(int i=0;i<cnt;i++){
        REBARBANDINFO rbi={sizeof(rbi)};rbi.fMask=RBBIM_CHILD|RBBIM_SIZE|RBBIM_STYLE;
        if(!SendMessage(rebar,RB_GETBANDINFO,i,(LPARAM)&rbi))continue;
        WCHAR cls[256]=L"";
        if(rbi.hwndChild)GetEffectiveClassName(rbi.hwndChild,cls,ARRAYSIZE(cls));
        BandState bs;
        if(!LoadBandState(cls,bs))continue;
        bool cxOk =rbi.cx==bs.cx;
        bool brkOk=((rbi.fStyle&RBBS_BREAK)!=0)==bs.brk;
        if(!cxOk||!brkOk){
            REBARBANDINFO set={sizeof(set)};
            set.fMask=RBBIM_SIZE|RBBIM_IDEALSIZE|RBBIM_STYLE;
            set.cx=bs.cx;set.cxIdeal=bs.cx;
            set.fStyle=rbi.fStyle;
            if(bs.brk)set.fStyle|=RBBS_BREAK;
            else set.fStyle&=~RBBS_BREAK;
            SendMessage(rebar,RB_SETBANDINFO,i,(LPARAM)&set);
            anyChanged=true;
        }
    }
    g_insideApply=false;
    if(anyChanged)MarkPendingApply(rebar);
}

void ApplySavedLayout(HWND rebar){
    if(!rebar||!IsWindow(rebar))return;
    int cnt=(int)SendMessage(rebar,RB_GETBANDCOUNT,0,0);
    if(cnt<=0)return;
    g_insideApply=true;
    std::vector<std::wstring> wantedOrder=LoadBandOrder();
    for(int target=0;target<(int)wantedOrder.size();target++){
        const std::wstring& wantCls=wantedOrder[target];
        int curCnt=(int)SendMessage(rebar,RB_GETBANDCOUNT,0,0);
        for(int j=target;j<curCnt;j++){
            REBARBANDINFO rbi={sizeof(rbi)};rbi.fMask=RBBIM_CHILD;
            if(!SendMessage(rebar,RB_GETBANDINFO,j,(LPARAM)&rbi))continue;
            WCHAR cls[256]=L"";
            if(rbi.hwndChild)GetEffectiveClassName(rbi.hwndChild,cls,ARRAYSIZE(cls));
            if(wantCls==cls){
                if(j!=target)SendMessage(rebar,RB_MOVEBAND,j,target);
                break;
            }
        }
    }
    cnt=(int)SendMessage(rebar,RB_GETBANDCOUNT,0,0);
    for(int i=0;i<cnt;i++){
        REBARBANDINFO rbi={sizeof(rbi)};rbi.fMask=RBBIM_CHILD|RBBIM_STYLE;
        if(!SendMessage(rebar,RB_GETBANDINFO,i,(LPARAM)&rbi))continue;
        WCHAR cls[256]=L"";
        if(rbi.hwndChild)GetEffectiveClassName(rbi.hwndChild,cls,ARRAYSIZE(cls));
        BandState bs;
        if(!LoadBandState(cls,bs))continue;
        REBARBANDINFO set={sizeof(set)};
        set.fMask=RBBIM_SIZE|RBBIM_IDEALSIZE|RBBIM_STYLE;
        set.cx=bs.cx;set.cxIdeal=bs.cx;
        set.fStyle=rbi.fStyle;
        if(bs.brk)set.fStyle|=RBBS_BREAK;
        else set.fStyle&=~RBBS_BREAK;
        SendMessage(rebar,RB_SETBANDINFO,i,(LPARAM)&set);
    }
    g_insideApply=false;
    EnterCriticalSection(&g_mutex);
    g_applyAttempts[rebar]=0;
    LeaveCriticalSection(&g_mutex);
    MarkPendingApply(rebar);
}

// =====================================================================
// Window helpers
// =====================================================================
HWND GetCabinetAncestor(HWND hwnd){
    for(HWND c=hwnd;c;c=GetParent(c)){
        WCHAR cls[64];
        if(GetClassName(c,cls,ARRAYSIZE(cls))&&wcscmp(cls,L"CabinetWClass")==0)return c;
    }return NULL;
}
HWND FindMenuBarRebar(HWND c){
    HWND s=FindWindowEx(c,NULL,L"ShellTabWindowClass",NULL);if(!s)return NULL;
    HWND w=FindWindowEx(s,NULL,L"WorkerW",NULL);if(!w)return NULL;
    return FindWindowEx(w,NULL,L"ReBarWindow32",NULL);
}
HWND FindNavbarRebar(HWND c){
    HWND w=FindWindowEx(c,NULL,L"WorkerW",NULL);
    while(w){HWND r=FindWindowEx(w,NULL,L"ReBarWindow32",NULL);if(r)return r;w=FindWindowEx(c,w,L"WorkerW",NULL);}
    return NULL;
}

HWND FindUpButton(HWND cab) {
    HWND navRebar = FindNavbarRebar(cab);
    if (!navRebar) return NULL;
    
    int cnt = (int)SendMessage(navRebar, RB_GETBANDCOUNT, 0, 0);
    for (int i = 0; i < cnt; i++) {
        REBARBANDINFO rbi = {sizeof(rbi)};
        rbi.fMask = RBBIM_CHILD;
        if (!SendMessage(navRebar, RB_GETBANDINFO, i, (LPARAM)&rbi)) continue;
        if (!rbi.hwndChild) continue;
        
        WCHAR cls[256];
        if (GetClassName(rbi.hwndChild, cls, ARRAYSIZE(cls))) {
            if (wcscmp(cls, L"UpBand") == 0) {
                HWND toolbar = FindWindowEx(rbi.hwndChild, NULL, L"ToolbarWindow32", NULL);
                if (toolbar) return toolbar;
            }
        }
    }
    return NULL;
}

HWND FindShellTab(HWND c){return FindWindowEx(c,NULL,L"ShellTabWindowClass",NULL);}
bool IsRebarChildOfDirectWorkerW(HWND r,HWND cab){
    HWND p=GetParent(r);if(!p)return false;
    WCHAR cls[64];if(!GetClassName(p,cls,ARRAYSIZE(cls)))return false;
    if(wcscmp(cls,L"WorkerW")!=0)return false;
    return GetParent(p)==cab;
}
bool IsNavbarWorkerW(HWND h){
    WCHAR cls[64];if(!GetClassName(h,cls,ARRAYSIZE(cls)))return false;
    if(wcscmp(cls,L"WorkerW")!=0)return false;
    HWND p=GetParent(h);if(!p)return false;
    if(!GetClassName(p,cls,ARRAYSIZE(cls))||wcscmp(cls,L"CabinetWClass")!=0)return false;
    return FindWindowEx(h,NULL,L"ReBarWindow32",NULL)!=NULL;
}
bool ContainsClass(HWND root,const wchar_t* target){
    WCHAR cls[256];if(!GetClassName(root,cls,ARRAYSIZE(cls)))return false;
    if(wcscmp(cls,target)==0)return true;
    struct L{static BOOL CALLBACK cb(HWND h,LPARAM l){
        auto*p=(std::pair<const wchar_t*,bool>*)l;
        WCHAR c[256];if(GetClassName(h,c,ARRAYSIZE(c))&&wcscmp(c,p->first)==0){p->second=true;return FALSE;}
        return TRUE;}};
    std::pair<const wchar_t*,bool> ctx{target,false};
    EnumChildWindows(root,L::cb,(LPARAM)&ctx);return ctx.second;
}
bool ContainsSearchBand(HWND h){
    return ContainsClass(h,L"UniversalSearchBand")||ContainsClass(h,L"Search Box")||ContainsClass(h,L"SearchEditBoxWrapperClass");
}
bool ContainsAddressBand(HWND h){return ContainsClass(h,L"Address Band Root");}
HWND FindBreadcrumbParent(HWND root){
    WCHAR cls[256];if(GetClassName(root,cls,ARRAYSIZE(cls))&&wcscmp(cls,L"Breadcrumb Parent")==0)return root;
    struct L{static BOOL CALLBACK cb(HWND h,LPARAM l){
        WCHAR c[256];if(GetClassName(h,c,ARRAYSIZE(c))&&wcscmp(c,L"Breadcrumb Parent")==0){*(HWND*)l=h;return FALSE;}
        return TRUE;}};
    HWND f=NULL;EnumChildWindows(root,L::cb,(LPARAM)&f);return f;
}
void ExpandShellTabToFillCabinet(HWND cab){
    HWND s=FindShellTab(cab);if(!s||!IsWindow(s))return;
    RECT rc;GetClientRect(cab,&rc);
    SetWindowPos(s,NULL,0,0,rc.right,rc.bottom,SWP_NOZORDER|SWP_NOACTIVATE);
}

DWORD GetReferenceGripperStyle(HWND rebar){
    int cnt=(int)SendMessage(rebar,RB_GETBANDCOUNT,0,0);
    for(int i=0;i<cnt;i++){
        REBARBANDINFO rbi={sizeof(rbi)};
        rbi.fMask=RBBIM_STYLE|RBBIM_CHILD;
        if(!SendMessage(rebar,RB_GETBANDINFO,i,(LPARAM)&rbi))continue;
        if(!rbi.hwndChild)continue;
        if(HasChildFlag(rbi.hwndChild, CF_MOVED))continue;
        return rbi.fStyle & (RBBS_GRIPPERALWAYS|RBBS_NOGRIPPER);
    }
    return RBBS_GRIPPERALWAYS;
}

void SyncMovedBandGrippers(HWND rebar){
    if(g_insideGripperSync) return;
    if(!rebar||!IsWindow(rebar))return;
    g_insideGripperSync=true;
    DWORD gripperBits=GetReferenceGripperStyle(rebar);
    int cnt=(int)SendMessage(rebar,RB_GETBANDCOUNT,0,0);
    for(int i=0;i<cnt;i++){
        REBARBANDINFO q={sizeof(q)};
        q.fMask=RBBIM_CHILD|RBBIM_STYLE;
        if(!SendMessage(rebar,RB_GETBANDINFO,i,(LPARAM)&q))continue;
        if(!q.hwndChild||!HasChildFlag(q.hwndChild, CF_MOVED))continue;
        DWORD curBits=q.fStyle&(RBBS_GRIPPERALWAYS|RBBS_NOGRIPPER);
        if(curBits!=gripperBits){
            REBARBANDINFO set={sizeof(set)};
            set.fMask=RBBIM_STYLE;
            set.fStyle=(q.fStyle&~(RBBS_GRIPPERALWAYS|RBBS_NOGRIPPER))|gripperBits;
            SendMessage(rebar,RB_SETBANDINFO,i,(LPARAM)&set);
        }
    }
    g_insideGripperSync=false;
}

LRESULT CALLBACK UpButton_SubclassProc(HWND hwnd, UINT msg, WPARAM wP, LPARAM lP, DWORD_PTR /*data*/) {
    if (msg == WM_SIZE) {
        ResizeUpButtonToolbar(hwnd);
    }
    return DefSubclassProc(hwnd, msg, wP, lP);
}

LRESULT CALLBACK BreadcrumbToolbar_SubclassProc(HWND hwnd,UINT msg,WPARAM wP,LPARAM lP,DWORD_PTR){
    if(msg==WM_WINDOWPOSCHANGING){
        auto*pos=(WINDOWPOS*)lP;
        EnterCriticalSection(&g_mutex);
        ToolbarGuard&g=g_toolbarGuards[hwnd];
        if(IsInsideRebarLayout()){
            if(!(pos->flags&SWP_NOMOVE)){g.x=pos->x;g.y=pos->y;}
            if(!(pos->flags&SWP_NOSIZE)){g.cx=pos->cx;g.cy=pos->cy;}
            g.hasGood=true;pos->flags&=~SWP_HIDEWINDOW;
        }else if(g.hasGood){
            pos->x=g.x;pos->y=g.y;pos->cx=g.cx;pos->cy=g.cy;
            pos->flags&=~(SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE);pos->flags|=SWP_SHOWWINDOW;
        }
        LeaveCriticalSection(&g_mutex);
    }
    if(msg==WM_SHOWWINDOW&&!wP&&!IsInsideRebarLayout())return 0;
    return DefSubclassProc(hwnd,msg,wP,lP);
}

LRESULT CALLBACK AddressBandRoot_SubclassProc(HWND hwnd,UINT msg,WPARAM wP,LPARAM lP,DWORD_PTR){
    if(IsNeutered(hwnd))return DefWindowProc(hwnd,msg,wP,lP);
    return DefSubclassProc(hwnd,msg,wP,lP);
}

LRESULT CALLBACK NavWorkerW_SubclassProc(HWND hwnd,UINT msg,WPARAM wP,LPARAM lP,DWORD_PTR){
    if(IsForceHidden(hwnd)){
        if(msg==WM_WINDOWPOSCHANGING){
            auto*p=(WINDOWPOS*)lP;p->x=p->y=p->cx=p->cy=0;
            p->flags=(p->flags&~SWP_SHOWWINDOW)|SWP_HIDEWINDOW|SWP_NOACTIVATE|SWP_NOZORDER;
        }
        if(msg==WM_SHOWWINDOW&&wP)return 0;
    }
    return DefSubclassProc(hwnd,msg,wP,lP);
}

LRESULT CALLBACK ShellTab_SubclassProc(HWND hwnd,UINT msg,WPARAM wP,LPARAM lP,DWORD_PTR){
    if(msg==WM_WINDOWPOSCHANGING){
        HWND cab=GetParent(hwnd);
        if(cab&&WasAlreadyMoved(cab)){
            auto*p=(WINDOWPOS*)lP;RECT rc;GetClientRect(cab,&rc);
            p->x=0;p->y=0;p->cx=rc.right;p->cy=rc.bottom;
            p->flags=(p->flags&~(SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW))|SWP_NOZORDER|SWP_NOACTIVATE;
        }
    }
    return DefSubclassProc(hwnd,msg,wP,lP);
}

LRESULT CALLBACK MenuReBarParent_SubclassProc(HWND hwnd,UINT msg,WPARAM wP,LPARAM lP,DWORD_PTR){
    if(msg==WM_NOTIFY){
        auto*hdr=(NMHDR*)lP;
        if(hdr->code==RBN_LAYOUTCHANGED||hdr->code==RBN_ENDDRAG){
            HWND rebar=hdr->hwndFrom;
            HWND cab=GetRebarCabinet(rebar);
            if(!cab)cab=GetCabinetAncestor(rebar);
            if(cab&&WasAlreadyMoved(cab)&&!g_insideApply)
                SaveBandPositions(rebar);
        }
    }
    return DefSubclassProc(hwnd,msg,wP,lP);
}

LRESULT CALLBACK ReBar_SubclassProc(HWND hwnd,UINT msg,WPARAM wP,LPARAM lP,DWORD_PTR){
    if(msg==RB_SETBANDINFO){
        auto*inf=(REBARBANDINFO*)lP;
        if(inf&&(inf->fMask&RBBIM_CHILDSIZE)){
            HWND ch=NULL;
            if(inf->fMask&RBBIM_CHILD)ch=inf->hwndChild;
            else{REBARBANDINFO q={sizeof(q)};q.fMask=RBBIM_CHILD;if(SendMessage(hwnd,RB_GETBANDINFO,wP,(LPARAM)&q))ch=q.hwndChild;}
            if(ch&&HasChildFlag(ch, CF_MOVED)){
                int bandHeight = GetDesiredBandHeight();
                inf->cyMinChild=inf->cyChild=inf->cyMaxChild=bandHeight;
                inf->cyIntegral=1;
            }
        }
    }
    g_rebarLayoutDepth++;
    LRESULT r=DefSubclassProc(hwnd,msg,wP,lP);
    g_rebarLayoutDepth--;
    
    if(msg==WM_SIZE&&IsPendingApply(hwnd)&&!g_insideApply){
        EnterCriticalSection(&g_mutex);
        int&attempts=g_applyAttempts[hwnd];
        bool canRetry=(attempts<5);
        if(canRetry)attempts++;
        LeaveCriticalSection(&g_mutex);
        if(canRetry)ReapplyCx(hwnd);
        else ClearPendingApply(hwnd);
    }
    if(msg==RB_INSERTBAND){
        HWND cab=GetCabinetAncestor(hwnd);
        if(cab&&!WasAlreadyMoved(cab)&&IsRebarChildOfDirectWorkerW(hwnd,cab))
            PostMessage(cab,WM_APP_DO_MOVE,0,0);
    }
    
    if(msg==RB_SETBANDINFO && !g_insideGripperSync){
        HWND cab=GetCabinetAncestor(hwnd);
        if(cab && WasAlreadyMoved(cab)){
            SyncMovedBandGrippers(hwnd);
        }
    }
    
    if(msg==WM_MOUSEMOVE || msg==WM_LBUTTONUP) {
        HWND cab=GetRebarCabinet(hwnd);
        if(!cab)cab=GetCabinetAncestor(hwnd);
        if(cab&&WasAlreadyMoved(cab)&&!g_insideApply) {
            static DWORD lastSave = 0;
            DWORD now = GetTickCount();
            if(msg==WM_LBUTTONUP || (now - lastSave > 1000)) {
                SaveBandPositions(hwnd);
                lastSave = now;
            }
        }
    }
    
    return r;
}

bool DoMoveSearchBandToMenuBar(HWND cabinetWnd);

LRESULT CALLBACK Cabinet_SubclassProc(HWND hwnd,UINT msg,WPARAM wP,LPARAM lP,DWORD_PTR){
    if(msg==WM_CLOSE){
        if(WasAlreadyMoved(hwnd)){
            HWND mr=GetCabinetMenuRebar(hwnd);
            if(mr&&IsWindow(mr))SaveBandPositions(mr);
        }
    }
    if(msg==WM_APP_DO_MOVE){DoMoveSearchBandToMenuBar(hwnd);return 0;}
    if((msg==WM_SIZE||msg==WM_WINDOWPOSCHANGED)&&WasAlreadyMoved(hwnd)){
        LRESULT r=DefSubclassProc(hwnd,msg,wP,lP);
        ExpandShellTabToFillCabinet(hwnd);
        return r;
    }
    if((msg==WM_ACTIVATE||msg==WM_SETFOCUS)&&!WasAlreadyMoved(hwnd))
        PostMessage(hwnd,WM_APP_DO_MOVE,0,0);
    return DefSubclassProc(hwnd,msg,wP,lP);
}

bool DoMoveSearchBandToMenuBar(HWND cabinetWnd){
    if(WasAlreadyMoved(cabinetWnd)){ExpandShellTabToFillCabinet(cabinetWnd);return true;}
    HWND menuRebar=FindMenuBarRebar(cabinetWnd);
    HWND navRebar=FindNavbarRebar(cabinetWnd);
    if(!menuRebar||!navRebar)return false;
    
    int bandHeight = GetDesiredBandHeight();
    
    struct BandToMove{
        int navIdx;HWND child;HWND leftover;int width;
        bool isUpButton;bool isBreadcrumb;
    };
    std::vector<BandToMove> toMove;
    int navCnt=(int)SendMessage(navRebar,RB_GETBANDCOUNT,0,0);
    
    for(int i=0;i<navCnt;i++){
        REBARBANDINFO rbi={sizeof(rbi)};rbi.fMask=RBBIM_CHILD;
        if(!SendMessage(navRebar,RB_GETBANDINFO,i,(LPARAM)&rbi)||!rbi.hwndChild)continue;
        
        if(g_settings.moveSearchBand&&ContainsSearchBand(rbi.hwndChild)){
            RECT rc;GetWindowRect(rbi.hwndChild,&rc);int w=rc.right-rc.left;if(w<50)w=200;
            toMove.push_back({i,rbi.hwndChild,NULL,w,false,false});
        }else if(g_settings.moveBreadcrumb&&ContainsAddressBand(rbi.hwndChild)){
            HWND bc=FindBreadcrumbParent(rbi.hwndChild);
            if(bc){
                HWND bcToolbar=FindWindowEx(bc,NULL,L"ToolbarWindow32",NULL);
                if(bcToolbar){
                    RECT rc;GetWindowRect(bcToolbar,&rc);int w=rc.right-rc.left;if(w<50)w=250;
                    toMove.push_back({i,bcToolbar,rbi.hwndChild,w,false,true});
                }
            }
        }
    }
    
    if(g_settings.moveUpButton) {
        HWND upButton = FindUpButton(cabinetWnd);
        if(upButton && IsWindow(upButton)) {
            for(int i = 0; i < navCnt; i++) {
                REBARBANDINFO rbi = {sizeof(rbi)};
                rbi.fMask = RBBIM_CHILD;
                if(!SendMessage(navRebar, RB_GETBANDINFO, i, (LPARAM)&rbi)) continue;
                
                WCHAR cls[256];
                if(rbi.hwndChild && GetClassName(rbi.hwndChild, cls, ARRAYSIZE(cls))) {
                    if(wcscmp(cls, L"UpBand") == 0) {
                        RECT rc;
                        GetWindowRect(upButton, &rc);
                        int w = rc.right - rc.left;
                        if(w < 20) w = 30;
                        toMove.push_back({i, upButton, rbi.hwndChild, w, true, false});
                        break;
                    }
                }
            }
        }
    }
    
    LONG_PTR st=GetWindowLongPtr(menuRebar,GWL_STYLE);
    st=(st&~RBS_FIXEDORDER)|RBS_VARHEIGHT;
    SetWindowLongPtr(menuRebar,GWL_STYLE,st);
    HWND navWorker=GetParent(navRebar);
    
    if(!toMove.empty()){
        DWORD gripperStyle = GetReferenceGripperStyle(menuRebar);
        
        for(auto it=toMove.rbegin();it!=toMove.rend();++it){
            SendMessage(navRebar,RB_DELETEBAND,it->navIdx,0);
            if(it->leftover&&IsWindow(it->leftover)){MarkNeutered(it->leftover);ShowWindow(it->leftover,SW_HIDE);}
        }
        
        for(auto&b:toMove){
            SetParent(b.child,menuRebar);
            
            WCHAR cls[256]=L"";
            if(b.isUpButton){
                wcsncpy_s(cls,ARRAYSIZE(cls),L"UpButtonToolbar",_TRUNCATE);
            }else if(b.isBreadcrumb){
                wcsncpy_s(cls,ARRAYSIZE(cls),L"BreadcrumbToolbar",_TRUNCATE);
            }else{
                GetClassName(b.child,cls,ARRAYSIZE(cls));
            }
            
            BandState bs;bool hasSaved=LoadBandState(cls,bs);
            UINT useCx=hasSaved?bs.cx:(UINT)b.width;
            
            if(b.isUpButton) {
                ResizeUpButtonToolbar(b.child);
            }else if(b.isBreadcrumb){
                SendMessage(b.child, TB_AUTOSIZE, 0, 0);
            }
            
            REBARBANDINFO rbi={sizeof(rbi)};
            rbi.fMask=RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_SIZE|RBBIM_IDEALSIZE;
            rbi.fStyle=gripperStyle;
            
            if(hasSaved&&bs.brk)rbi.fStyle|=RBBS_BREAK;
            rbi.hwndChild=b.child;
            rbi.cxMinChild = 80;
            rbi.cyMinChild = bandHeight;
            rbi.cyMaxChild = bandHeight;
            rbi.cyChild = bandHeight;
            rbi.cx = useCx;
            rbi.cxIdeal = useCx;
            rbi.cyIntegral=1;
            
            BOOL ins=(BOOL)SendMessage(menuRebar,RB_INSERTBAND,(WPARAM)-1,(LPARAM)&rbi);
            if(ins){
                int flags=CF_MOVED;
                if(b.isUpButton)flags|=CF_UPBUTTON;
                else if(b.isBreadcrumb)flags|=CF_BREADCRUMB;
                SetChildFlag(b.child,flags);
                if(b.isUpButton){
                    HookWindow(b.child, UpButton_SubclassProc);
                    ResizeUpButtonToolbar(b.child);
                }else if(b.isBreadcrumb){
                    HookWindow(b.child, BreadcrumbToolbar_SubclassProc);
                }
                ShowWindow(b.child,SW_SHOW);
            }else{
                SetParent(b.child,b.leftover?b.leftover:navRebar);
            }
        }
        
        {
            HWND mrParent=GetParent(menuRebar);
            if(mrParent){
                HookWindow(mrParent, MenuReBarParent_SubclassProc);
            }
        }
        RegisterRebarCabinet(menuRebar,cabinetWnd);
        RegisterCabinetMenuRebar(cabinetWnd,menuRebar);
    }
    
    if(navWorker)MarkForceHidden(navWorker);
    MarkMoved(cabinetWnd);
    
    if(!toMove.empty()){
        ApplySavedLayout(menuRebar);
        SyncMovedBandGrippers(menuRebar);
    }
    
    ExpandShellTabToFillCabinet(cabinetWnd);
    RedrawWindow(cabinetWnd,NULL,NULL,
        RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ALLCHILDREN|RDW_ERASENOW|RDW_FRAME);
    return true;
}

void HookWindow(HWND hwnd, WindhawkUtils::WH_SUBCLASSPROC subclassProc){
    if(!hwnd||!IsWindow(hwnd))return;
    WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, subclassProc, 0);
}

void ProcessWindow(HWND hwnd){
    if(!hwnd||!IsWindow(hwnd))return;
    WCHAR cls[256];if(!GetClassName(hwnd,cls,ARRAYSIZE(cls)))return;
    if(!wcscmp(cls,L"CabinetWClass")){HookWindow(hwnd,Cabinet_SubclassProc);return;}
    if(!wcscmp(cls,L"ShellTabWindowClass")){
        HWND p=GetParent(hwnd);WCHAR pc[64];
        if(p&&GetClassName(p,pc,ARRAYSIZE(pc))&&wcscmp(pc,L"CabinetWClass")==0)
            HookWindow(hwnd,ShellTab_SubclassProc);
        return;
    }
    if(!wcscmp(cls,L"WorkerW")){
        if(IsNavbarWorkerW(hwnd)){
            HookWindow(hwnd,NavWorkerW_SubclassProc);
            SetWindowPos(hwnd,NULL,0,0,0,0,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOREDRAW|SWP_HIDEWINDOW);
            MarkForceHidden(hwnd);
            return;
        }
    }
    if(!wcscmp(cls,L"ReBarWindow32")){HookWindow(hwnd,ReBar_SubclassProc);return;}
    if(!wcscmp(cls,L"Address Band Root")){HookWindow(hwnd,AddressBandRoot_SubclassProc);return;}
}
BOOL CALLBACK EnumChildHook(HWND hwnd,LPARAM){ProcessWindow(hwnd);return TRUE;}

using CreateWindowExW_t=decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD s,LPCWSTR c,LPCWSTR wn,DWORD st,int X,int Y,int W,int H,HWND p,HMENU m,HINSTANCE h,LPVOID lp){
    HWND hwnd=CreateWindowExW_Original(s,c,wn,st,X,Y,W,H,p,m,h,lp);
    if(hwnd&&c&&!IS_INTRESOURCE(c)){
        ProcessWindow(hwnd);EnumChildWindows(hwnd,EnumChildHook,0);
        if(wcscmp(c,L"UniversalSearchBand")&&wcscmp(c,L"Address Band Root")&&wcscmp(c,L"Breadcrumb Parent")&&wcscmp(c,L"UpBand")){
            HWND cab=GetCabinetAncestor(hwnd);
            if(cab&&!WasAlreadyMoved(cab))PostMessage(cab,WM_APP_DO_MOVE,0,0);
        }
    }
    return hwnd;
}

BOOL Wh_ModInit(){
    Wh_Log(L"FlexibleExplorer init");
    LoadSettings();
    InitializeCriticalSection(&g_mutex);

    Wh_SetFunctionHook((void*)CreateWindowExW,(void*)CreateWindowExW_Hook,(void**)&CreateWindowExW_Original);

    Wh_SetFunctionHook(
            (void*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtSetValueKey"),
            (void*)NtSetValueKey_Hook,
            (void**)&NtSetValueKey_Original);

    for(HWND w=GetTopWindow(NULL);w;w=GetNextWindow(w,GW_HWNDNEXT)){
        WCHAR cls[64];
        if(GetClassName(w,cls,ARRAYSIZE(cls))&&!wcscmp(cls,L"CabinetWClass")){
            ProcessWindow(w);EnumChildWindows(w,EnumChildHook,0);PostMessage(w,WM_APP_DO_MOVE,0,0);
        }
    }

    return TRUE;
}

void Wh_ModUninit(){
    Wh_Log(L"FlexibleExplorer uninit");
    DeleteCriticalSection(&g_mutex);
}
