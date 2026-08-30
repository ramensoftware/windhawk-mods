// ==WindhawkMod==
// @id              win7-window-animations-restorer
// @name            Windows 7 Window Animations Restorer
// @description     This mod restores the Windows 7 Aero minimize and restore animation on classic Win32 windows without hooking DWM. 
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         MIT
// @include         explorer.exe
// @include         notepad.exe
// @include         wordpad.exe
// @include         ApplicationFrameHost.exe
// @include         mspaint.exe
// @include         SnippingTool.exe
// @include         iexplore.exe
// @include         regedit.exe
// @include         diskmgmt.msc
// @include         cleanmgr.exe
// @include         rundll32.exe
// @include         calc.exe
// @include         charmap.exe
// @include         taskmgr.exe
// @include         winver.exe
// @include         msconfig.exe
// @include         mstsc.exe
// @include         wmplayer.exe
// @include         sol.exe
// @include         spider.exe
// @include         winmine.exe
// @include         freecell.exe
// @include         Chess.exe
// @include         Mahjong.exe
// @include         stikynot.exe
// @include         outlook.exe
// @include         winword.exe
// @include         excel.exe
// @include         powerpnt.exe
// @include         thunderbird.exe
// @include         chrome.exe
// @include         msedge.exe
// @include         firefox.exe
// @include         code.exe
// @include         devenv.exe
// @include         notepad++.exe
// @include         7zFM.exe
// @include         powershell.exe
// @include         pwsh.exe
// @compilerOptions -lgdi32 -lmsimg32 -lshcore -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

## Windows 7 Window Animations Restorer

## About

This mod tries to restore Windows 7 window animations on Windows 10 and 11 for classic Win32 programs, **without modifying the DWM**.

## Features

- **Minimize / Restore**: the window shrinks toward the taskbar button with a 3D tilt (5° pitch, 8° yaw, 250 ms). Restore plays the same movement in reverse.

- **Open**: handled by Windows by default. An experimental option is available.

- **Close**: The mod captures a screenshot of the window and simulates the Windows 7 animation.

**Known Limitations**

- UWP / WinUI windows show the normal Windows animation.
- During restore, the window stays minimized while the overlay grows over it.
- If a window refuses to minimize, the animation is canceled.
- Dialogs without a minimize button are not animated.

## How to add applications

The mod is injected only into a predefined list. To add a program, add its `.exe` name to the `@include` lines or use Windhawk's advanced settings.

## Notes

- The mod has been tested on Windows 10 21H2.
- The mod does not modify system files.
- The mod does not replace parts of Windows.
- The mod tries to replicate the exact timing and motion from Windows 7 (`uDWM.dll` 6.1.7600.16385).
- The mod is injected only into a curated list of applications to limit potential issues.

## Credits

- **DWM 3D Transforms** by [xalejandro](https://github.com/tetawaves).
- [OpenGlass](https://github.com/ALTaleX531/OpenGlass) by ALTaleX
- **3D Aero Transforms mod** by [kieldbg](https://github.com/kieldbg).
- Overlay / `user32` architecture:
  [Classic Minimize/Maximize Animations](https://windhawk.net/mods/classic-min-max-animations)
  by [aubymori](https://github.com/aubymori).

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- animateMinimize: true
  $name: Animate minimizing and restoring windows
  $description: This setting enables the 250 ms shrink/grow toward the taskbar button with the Win7 3D tilt when a window minimizes or restores. Turn it off to let Windows animate (or not) on its own.
- animateOpen: false
  $name: Animate opening windows (EXPERIMENTAL)
  $description: This setting plays the Windows 7 open animation when a window is shown for the first time. The mod waits for the desktop window manager to composite the real window, captures that exact image (Aero border and OpenGlass / DWMBlurGlass glass included) and animates it, so no border appears at the end. Turn it off to let Windows show the window instantly.
- openDiagnostics: false
  $name: This setting writes open-animation diagnostics to the Windhawk log
  $description: Logs every open-animation decision (why an animation was skipped, or that the composed frame was captured), instead of only the first one per process. Useful to understand why a specific program is not animated.
- animateClose: true
  $name: Animate closing windows 
  $description: This setting fades the window out over a short scale-down animation before it is actually closed, instead of the normal Windows close. It briefly blocks the closing thread for the duration of the fade, which is a bit riskier than minimize/restore, so it stays off by default. Enable it if you want the closer-to-Win7 look and are fine with that tradeoff.
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <deque>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#ifndef DWMWA_TRANSITIONS_FORCEDISABLED
#define DWMWA_TRANSITIONS_FORCEDISABLED 3
#endif
#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif
#ifndef DWM_BB_ENABLE
#define DWM_BB_ENABLE 1
#define DWM_BB_BLURREGION 2
#endif

#define RECTW(rc) ((rc).right - (rc).left)
#define RECTH(rc) ((rc).bottom - (rc).top)

static wchar_t g_exeName[MAX_PATH] = L"?";
// Diagnostica del percorso di apertura: registra SOLO i motivi per cui l'animazione
// non parte (evento raro nell'uso normale), cosi' il log di Windhawk resta pulito ma
// permette di capire perche' una data applicazione non viene animata.
static bool g_openLog=true;
static void OpenLogLine(bool important, LPCWSTR fmt, va_list ap){
    static std::atomic<bool> firstWhy{false};
    if(!g_openLog){
        if(!important) return;
        if(firstWhy.exchange(true)) return;
    }
    wchar_t buf[224]; 
    _vsnwprintf_s(buf, _countof(buf), _TRUNCATE, fmt, ap);
    
    // Use a string literal format for Wh_Log
    Wh_Log(L"[win7-anim/open] %s [%s]", buf, g_exeName);
}
static void OpenLog(LPCWSTR fmt, ...){ va_list ap; va_start(ap,fmt); OpenLogLine(false,fmt,ap); va_end(ap); }
static void OpenLogWhy(LPCWSTR fmt, ...){ va_list ap; va_start(ap,fmt); OpenLogLine(true,fmt,ap); va_end(ap); }
static void InitExeName() {
    wchar_t path[MAX_PATH] = {};
    if (!GetModuleFileNameW(nullptr, path, MAX_PATH)) return;
    const wchar_t* slash = path;
    for (const wchar_t* p = path; *p; ++p) if (*p == L'\\' || *p == L'/') slash = p + 1;
    wcsncpy_s(g_exeName, slash, _TRUNCATE);
}
static bool IsSnippingTool(){ return _wcsicmp(g_exeName, L"SnippingTool.exe")==0; }

// RAII – sicurezza extra
class ScopedDc { public: ScopedDc()=default; explicit ScopedDc(HDC hdc) noexcept : m_hdc(hdc){} ~ScopedDc(){reset();} ScopedDc(const ScopedDc&)=delete; ScopedDc& operator=(const ScopedDc&)=delete; ScopedDc(ScopedDc&& o) noexcept : m_hdc(o.m_hdc){o.m_hdc=nullptr;} ScopedDc& operator=(ScopedDc&& o) noexcept { if(this!=&o){reset(o.m_hdc); o.m_hdc=nullptr;} return *this; } void reset(HDC hdc=nullptr) noexcept { if(m_hdc) DeleteDC(m_hdc); m_hdc=hdc; } HDC get() const noexcept { return m_hdc; } explicit operator bool() const noexcept { return m_hdc!=nullptr; } private: HDC m_hdc=nullptr; };
class ScopedWindowDc { public: ScopedWindowDc()=default; ScopedWindowDc(HWND hwnd,HDC hdc) noexcept : m_hwnd(hwnd),m_hdc(hdc){} ~ScopedWindowDc(){reset();} ScopedWindowDc(const ScopedWindowDc&)=delete; ScopedWindowDc& operator=(const ScopedWindowDc&)=delete; void reset() noexcept { if(m_hdc){ReleaseDC(m_hwnd,m_hdc); m_hdc=nullptr; m_hwnd=nullptr;} } HDC get() const noexcept { return m_hdc; } explicit operator bool() const noexcept { return m_hdc!=nullptr; } private: HWND m_hwnd=nullptr; HDC m_hdc=nullptr; };
class ScopedGdiObj { public: ScopedGdiObj()=default; explicit ScopedGdiObj(HGDIOBJ obj) noexcept : m_obj(obj){} ~ScopedGdiObj(){reset();} ScopedGdiObj(const ScopedGdiObj&)=delete; ScopedGdiObj& operator=(const ScopedGdiObj&)=delete; ScopedGdiObj(ScopedGdiObj&& o) noexcept : m_obj(o.m_obj){o.m_obj=nullptr;} ScopedGdiObj& operator=(ScopedGdiObj&& o) noexcept { if(this!=&o){reset(o.m_obj); o.m_obj=nullptr;} return *this; } void reset(HGDIOBJ obj=nullptr) noexcept { if(m_obj) DeleteObject(m_obj); m_obj=obj; } HGDIOBJ get() const noexcept { return m_obj; } explicit operator bool() const noexcept { return m_obj!=nullptr; } private: HGDIOBJ m_obj=nullptr; };
class ScopedSelect { public: ScopedSelect(HDC hdc,HGDIOBJ obj) noexcept : m_hdc(hdc),m_prev(SelectObject(hdc,obj)){} ~ScopedSelect(){ if(m_hdc&&m_prev) SelectObject(m_hdc,m_prev); } ScopedSelect(const ScopedSelect&)=delete; ScopedSelect& operator=(const ScopedSelect&)=delete; private: HDC m_hdc; HGDIOBJ m_prev; };
class ScopedDpiAware { public: ScopedDpiAware() noexcept : m_prev(SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)){} ~ScopedDpiAware(){ if(m_prev) SetThreadDpiAwarenessContext(m_prev); } ScopedDpiAware(const ScopedDpiAware&)=delete; ScopedDpiAware& operator=(const ScopedDpiAware&)=delete; private: DPI_AWARENESS_CONTEXT m_prev; };
class ScopedHandle { public: ScopedHandle()=default; explicit ScopedHandle(HANDLE h) noexcept : m_h(h){} ~ScopedHandle(){reset();} ScopedHandle(const ScopedHandle&)=delete; ScopedHandle& operator=(const ScopedHandle&)=delete; ScopedHandle(ScopedHandle&& o) noexcept : m_h(o.m_h){o.m_h=nullptr;} ScopedHandle& operator=(ScopedHandle&& o) noexcept { if(this!=&o){reset(); m_h=o.m_h; o.m_h=nullptr;} return *this; } void reset(HANDLE h=nullptr) noexcept { if(m_h&&m_h!=INVALID_HANDLE_VALUE) CloseHandle(m_h); m_h=h; } HANDLE get() const noexcept { return m_h; } explicit operator bool() const noexcept { return m_h&&m_h!=INVALID_HANDLE_VALUE; } private: HANDLE m_h=nullptr; };
class ScopedHIcon { public: ScopedHIcon()=default; explicit ScopedHIcon(HICON h) noexcept : m_h(h){} ~ScopedHIcon(){reset();} ScopedHIcon(const ScopedHIcon&)=delete; ScopedHIcon& operator=(const ScopedHIcon&)=delete; void reset(HICON h=nullptr) noexcept { if(m_h) DestroyIcon(m_h); m_h=h; } HICON get() const noexcept { return m_h; } private: HICON m_h=nullptr; };
class ScopedDwmTransitions { public: explicit ScopedDwmTransitions(HWND hwnd) noexcept : m_hwnd(hwnd), m_disabled(false){} void Disable(){ if(!m_hwnd||!IsWindow(m_hwnd)) return; BOOL dis=TRUE; if(SUCCEEDED(DwmSetWindowAttribute(m_hwnd,DWMWA_TRANSITIONS_FORCEDISABLED,&dis,sizeof(dis)))) m_disabled=true; } void Restore(){ if(m_disabled&&m_hwnd&&IsWindow(m_hwnd)){ BOOL dis=FALSE; DwmSetWindowAttribute(m_hwnd,DWMWA_TRANSITIONS_FORCEDISABLED,&dis,sizeof(dis)); } m_disabled=false; } ~ScopedDwmTransitions(){ Restore(); } void Dismiss(){ m_disabled=false; } private: HWND m_hwnd; bool m_disabled; };
class ScopedHrgn { public: ScopedHrgn()=default; explicit ScopedHrgn(HRGN h) noexcept : m_h(h){} ~ScopedHrgn(){reset();} ScopedHrgn(const ScopedHrgn&)=delete; ScopedHrgn& operator=(const ScopedHrgn&)=delete; void reset(HRGN h=nullptr) noexcept { if(m_h) DeleteObject(m_h); m_h=h; } HRGN get() const noexcept { return m_h; } HRGN release() noexcept { HRGN t=m_h; m_h=nullptr; return t; } private: HRGN m_h=nullptr; };
class ScopedCoInit { public: ScopedCoInit(){ m_hr=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED); } ~ScopedCoInit(){ if(SUCCEEDED(m_hr)) CoUninitialize(); } bool succeeded() const { return SUCCEEDED(m_hr); } private: HRESULT m_hr; };
class ScopedInterlockedFlag { public: explicit ScopedInterlockedFlag(LONG* p) noexcept : m_p(p){ InterlockedExchange(m_p,1); } ~ScopedInterlockedFlag(){ InterlockedExchange(m_p,0); } private: LONG* m_p; };

namespace Mat {
struct Matrix4x4F {
    FLOAT _11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44;
    Matrix4x4F() noexcept { _11=1;_12=0;_13=0;_14=0; _21=0;_22=1;_23=0;_24=0; _31=0;_32=0;_33=1;_34=0; _41=0;_42=0;_43=0;_44=1; }
    static Matrix4x4F Translation(FLOAT x,FLOAT y,FLOAT z) noexcept { Matrix4x4F m; m._41=x; m._42=y; m._43=z; return m; }
    static Matrix4x4F Scale(FLOAT x,FLOAT y,FLOAT z) noexcept { Matrix4x4F m; m._11=x; m._22=y; m._33=z; return m; }
    static Matrix4x4F RotationX(FLOAT d) noexcept { FLOAT a=d*3.141592654f/180.f; FLOAT s=std::sin(a),c=std::cos(a); Matrix4x4F m; m._22=c; m._23=s; m._32=-s; m._33=c; return m; }
    static Matrix4x4F RotationY(FLOAT d) noexcept { FLOAT a=d*3.141592654f/180.f; FLOAT s=std::sin(a),c=std::cos(a); Matrix4x4F m; m._11=c; m._13=-s; m._31=s; m._33=c; return m; }
    static Matrix4x4F PerspectiveProjection(FLOAT depth) noexcept { Matrix4x4F m; if(depth>0) m._34=-1.f/depth; return m; }
    Matrix4x4F operator*(const Matrix4x4F& b) const noexcept {
        const Matrix4x4F& a=*this; Matrix4x4F r;
        r._11=a._11*b._11+a._12*b._21+a._13*b._31+a._14*b._41; r._12=a._11*b._12+a._12*b._22+a._13*b._32+a._14*b._42;
        r._13=a._11*b._13+a._12*b._23+a._13*b._33+a._14*b._43; r._14=a._11*b._14+a._12*b._24+a._13*b._34+a._14*b._44;
        r._21=a._21*b._11+a._22*b._21+a._23*b._31+a._24*b._41; r._22=a._21*b._12+a._22*b._22+a._23*b._32+a._24*b._42;
        r._23=a._21*b._13+a._22*b._23+a._23*b._33+a._24*b._43; r._24=a._21*b._14+a._22*b._24+a._23*b._34+a._24*b._44;
        r._31=a._31*b._11+a._32*b._21+a._33*b._31+a._34*b._41; r._32=a._31*b._12+a._32*b._22+a._33*b._32+a._34*b._42;
        r._33=a._31*b._13+a._32*b._23+a._33*b._33+a._34*b._43; r._34=a._31*b._14+a._32*b._24+a._33*b._34+a._34*b._44;
        r._41=a._41*b._11+a._42*b._21+a._43*b._31+a._44*b._41; r._42=a._41*b._12+a._42*b._22+a._43*b._32+a._44*b._42;
        r._43=a._41*b._13+a._42*b._23+a._43*b._33+a._44*b._43; r._44=a._41*b._14+a._42*b._24+a._43*b._34+a._44*b._44;
        return r;
    }
    void TransformPoint(float x,float y,float z,float& ox,float& oy) const noexcept {
        float rx=x*_11+y*_21+z*_31+_41; float ry=x*_12+y*_22+z*_32+_42; float rw=x*_14+y*_24+z*_34+_44;
        if(rw>0.0001f||rw<-0.0001f){ox=rx/rw; oy=ry/rw;} else {ox=rx; oy=ry;}
    }
};
}
using Mat::Matrix4x4F;
using ShowWindow_t = decltype(&ShowWindow);
using ShowWindowAsync_t = decltype(&ShowWindowAsync);
using DestroyWindow_t = decltype(&DestroyWindow);

constexpr double kShowHideDurationSec = 0.25;
constexpr double kCloseDurationSec = 0.20;
constexpr double kOpenDurationSec = 0.25;

// Finestra di apertura: quanto attendere la composizione reale del DWM.
// Nessuno sleep "a caso": il loop e' cadenzato da DwmFlush(), che blocca fino a
// che il DWM non ha completato un giro di composizione, quindi ogni iterazione
// vale (al piu') un frame. Il timeout e' solo un tetto di sicurezza.
constexpr UINT  kOpenSettleTimeoutMs   = 220;   // attesa massima del primo frame composto
constexpr UINT  kOpenMinStableFrames   = 2;     // geometria invariata per N composizioni
constexpr UINT  kOpenStableNoPaint     = 4;     // dopo N composizioni rinuncia ad aspettare la pittura
constexpr UINT  kOpenMaxProbes         = 3;     // tentativi di lettura del frame composto
constexpr UINT  kOpenHandoffWaitMs     = 60;    // attesa della ri-pittura al handoff (overlay ancora su)
constexpr UINT  kOpenWatchdogMs        = 900;   // dead-man switch: settle+animazione+handoff < 900 ms
constexpr UINT_PTR kOpenWatchdogTimer  = 0x57A0;
constexpr BYTE  kOpenHiddenAlpha       = 0;     // finestra "coperta" = opaca 0, non nascosta
constexpr int   kOpenMaxCaptureSide    = 16384;

enum class AnimationType { None=0, Open, Close, Minimize, RestoreFromMinimized };

static float Lerp(float a,float b,float t){return a+(b-a)*t;}
static bool IsRectUsable(const RECT& rc){return rc.right>rc.left&&rc.bottom>rc.top;}
static RECT LerpRect(const RECT& a,const RECT& b,float t){
    RECT r; r.left=LONG(std::lround(Lerp(float(a.left),float(b.left),t))); r.top=LONG(std::lround(Lerp(float(a.top),float(b.top),t)));
    r.right=LONG(std::lround(Lerp(float(a.right),float(b.right),t))); r.bottom=LONG(std::lround(Lerp(float(a.bottom),float(b.bottom),t))); return r;
}
static RECT AspectCorrectedMinimizeTarget(const RECT& button){
    float bw=float(RECTW(button)), bh=float(RECTH(button)); if(bw<1||bh<1) return button;
    float ar=bh/bw; RECT t=button; t.right=t.left+LONG(bw*ar); t.bottom=t.top+LONG(bh*ar); if(!IsRectUsable(t)) return button; return t;
}
struct Win7TransformParams{ float rotX=0, rotY=0, transZ=0, opacity=1, ease=0; float yTrans=0, zTrans=0, pivotY=0; };

static Win7TransformParams ParamsFor(AnimationType type,float t,float h=0){
    Win7TransformParams p; t=std::clamp(t,0.f,1.f);
    switch(type){
        case AnimationType::Minimize: p.rotX=5.f*t; p.rotY=8.f*t; p.transZ=-4.f*t; p.opacity=1.f-0.35f*t; p.ease=t; p.pivotY=h*0.5f; break;
        case AnimationType::RestoreFromMinimized:{ float away=1.f-t; p.rotX=5.f*away; p.rotY=8.f*away; p.transZ=-4.f*away; p.opacity=0.65f+0.35f*t; p.ease=t; p.pivotY=h*0.5f; break; }
        case AnimationType::Close:{ float ease=1.f-std::sqrt(1.f-t); p.ease=ease; p.rotX=-5.f*ease; p.rotY=-2.f*ease; p.pivotY=h; p.opacity=1.f-t; break; }
        case AnimationType::Open:{ float ease=std::sqrt(t); float inv=1.f-ease; p.ease=ease; p.rotX=5.f*inv; p.rotY=0; p.pivotY=h*0.5f; p.yTrans=inv*h*-0.017f; p.zTrans=inv*h*0.1f; p.opacity=ease; break; }
        default: break;
    }
    return p;
}
static RECT RectFor(AnimationType type,float t,const RECT& win,const RECT& dest){
    t=std::clamp(t,0.f,1.f);
    switch(type){
        case AnimationType::Minimize: return LerpRect(win,dest,t);
        case AnimationType::RestoreFromMinimized: return LerpRect(dest,win,t);
        default: return win;
    }
}
static UINT DurationMsFor(AnimationType type){
    double ms = kShowHideDurationSec * 1000.0;
    if(type==AnimationType::Close){
        ms = kCloseDurationSec * 1000.0;
    } else if(type==AnimationType::Open){
        ms = kOpenDurationSec * 1000.0;
    }
    if(ms<16) ms=16;
    return UINT(std::lround(ms));
}
static Matrix4x4F BuildCameraMatrix(float w,float h,float df=0.8f){
    float depth=std::fmax(h,1.f)*df;
    return Matrix4x4F::Translation(-w*0.5f,-h*0.5f,0)*Matrix4x4F::Scale(1,1,-1)*Matrix4x4F::PerspectiveProjection(depth)*Matrix4x4F::Translation(w*0.5f,h*0.5f,0);
}
static Matrix4x4F BuildCornerMatrix(const Win7TransformParams& p,const RECT& rcCurrent,float ow,float oh,AnimationType type=AnimationType::Minimize){
    float w=ow, h=oh;
    float pivotY = (type==AnimationType::Close)? h : h*0.5f;
    if(p.pivotY!=0) pivotY=p.pivotY;
    if(type==AnimationType::Close){
        Matrix4x4F model=Matrix4x4F::Translation(0,-pivotY,0)*Matrix4x4F::RotationY(p.rotY)*Matrix4x4F::RotationX(p.rotX)*Matrix4x4F::Translation(0,pivotY,0);
        Matrix4x4F camera=BuildCameraMatrix(w,h,0.8f);
        Matrix4x4F place=Matrix4x4F::Translation(float(rcCurrent.left),float(rcCurrent.top),0);
        return model*camera*place;
    }
    if(type==AnimationType::Open){
        Matrix4x4F model=Matrix4x4F::Translation(0,-pivotY,0)*Matrix4x4F::RotationX(p.rotX)*Matrix4x4F::Translation(0,pivotY+p.yTrans,p.zTrans);
        Matrix4x4F camera=BuildCameraMatrix(w,h,0.8f);
        Matrix4x4F place=Matrix4x4F::Translation(float(rcCurrent.left),float(rcCurrent.top),0);
        return model*camera*place;
    }
    float width=float(RECTW(rcCurrent)), height=float(RECTH(rcCurrent));
    float sx=ow>1?width/ow:1, sy=oh>1?height/oh:1;
    float cx=ow*0.5f, cy=oh*0.5f;
    Matrix4x4F m=Matrix4x4F::Translation(-cx,-cy,0)*(Matrix4x4F::RotationX(-p.rotX)*Matrix4x4F::RotationY(-p.rotY))*Matrix4x4F::Translation(cx,cy,0)*Matrix4x4F::Scale(sx,sy,1)*Matrix4x4F::Translation(float(rcCurrent.left),float(rcCurrent.top),0);
    float invH=1.f/std::fmax(oh,1.f); m._43+=p.transZ; m._44+=-p.transZ*invH; return m;
}

bool g_animateMinimize=true; bool g_animateClose=false; bool g_animateOpen=true;
static void LoadSettings(){ g_animateMinimize=Wh_GetIntSetting(L"animateMinimize")!=0; g_animateClose=Wh_GetIntSetting(L"animateClose")!=0; g_animateOpen=Wh_GetIntSetting(L"animateOpen")!=0; g_openLog=Wh_GetIntSetting(L"openDiagnostics")!=0; }

typedef BOOL(WINAPI* GetWindowMinimizeRect_t)(HWND,LPRECT);
GetWindowMinimizeRect_t pGetWindowMinimizeRect=nullptr;
ShowWindow_t ShowWindow_orig=nullptr; ShowWindowAsync_t ShowWindowAsync_orig=nullptr; DestroyWindow_t DestroyWindow_orig=nullptr;

struct CaptureBits{ std::vector<uint32_t> pixels; int width=0,height=0; bool empty() const {return pixels.empty()||width<=0||height<=0;} };
static const size_t kMaxCachedCaptures=4; static std::mutex g_cacheMutex;
struct CacheEntry{ CaptureBits bits; std::list<HWND>::iterator lruIt; };
static std::unordered_map<HWND,CacheEntry> g_captureCache; static std::list<HWND> g_captureLru;
static void CacheCapture(HWND hwnd,const CaptureBits& bits){
    if(!hwnd||bits.empty()) return; try{ std::lock_guard<std::mutex> lock(g_cacheMutex); auto it=g_captureCache.find(hwnd);
        if(it!=g_captureCache.end()){ it->second.bits=bits; g_captureLru.splice(g_captureLru.begin(),g_captureLru,it->second.lruIt); return; }
        g_captureLru.push_front(hwnd); g_captureCache.emplace(hwnd,CacheEntry{bits,g_captureLru.begin()});
        while(g_captureCache.size()>kMaxCachedCaptures){ HWND v=g_captureLru.back(); g_captureLru.pop_back(); g_captureCache.erase(v); }
    }catch(...){}
}
static bool HasCachedCapture(HWND hwnd){ if(!hwnd) return false; try{ std::lock_guard<std::mutex> lock(g_cacheMutex); return g_captureCache.find(hwnd)!=g_captureCache.end(); }catch(...){return false;} }
static bool TakeCachedCapture(HWND hwnd,int ew,int eh,CaptureBits& out){
    try{ std::lock_guard<std::mutex> lock(g_cacheMutex); auto it=g_captureCache.find(hwnd); if(it==g_captureCache.end()) return false;
        CaptureBits bits=std::move(it->second.bits); g_captureLru.erase(it->second.lruIt); g_captureCache.erase(it); if(bits.empty()) return false;
        if(ew>0&&eh>0&&(std::abs(bits.width-ew)>128||std::abs(bits.height-eh)>128)) return false;
        out=std::move(bits); return true;
    }catch(...){return false;}
}
static void ForgetCapture(HWND hwnd){ try{ std::lock_guard<std::mutex> lock(g_cacheMutex); auto it=g_captureCache.find(hwnd); if(it!=g_captureCache.end()){ g_captureLru.erase(it->second.lruIt); g_captureCache.erase(it);} }catch(...){} }
static void ForceOpaqueAlpha(uint32_t* p,size_t c){ for(size_t i=0;i<c;++i) p[i]|=0xFF000000u; }

static bool GetWindowRectPhysical(HWND hwnd, RECT* rc){
    if(!hwnd||!rc) return false;
    if(GetWindowRect(hwnd, rc)) return IsRectUsable(*rc);
    return false;
}
static bool GetVisibleWindowRectForMinimize(HWND hwnd, RECT* rc){
    if(hwnd&&rc){
        RECT wr{}; if(GetWindowRect(hwnd,&wr)&&IsRectUsable(wr)){ *rc=wr; return true; }
    } return false;
}
static bool RectEquals(const RECT& a,const RECT& b){
    return a.left==b.left&&a.top==b.top&&a.right==b.right&&a.bottom==b.bottom;
}
// Il bordo del frame composto dal DWM non coincide con GetWindowRect(): su Win10/11
// il window rect contiene le margini invisibili di ridimensionamento (e l'ombra),
// quindi catturare GetWindowRect significherebbe prendere parte del desktop.
// DWMWA_EXTENDED_FRAME_BOUNDS e' il rettangolo che il DWM compone davvero; se non
// disponibile (DWM spento, errore, valori assurdi) si ripiega su GetWindowRect.
static bool GetFrameBoundsPhysical(HWND hwnd, RECT* rc){
    if(!hwnd||!rc) return false;
    RECT ext{};
    HRESULT hr = DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &ext, sizeof(ext));
    if(SUCCEEDED(hr)&&IsRectUsable(ext)&&RECTW(ext)>=8&&RECTH(ext)>=8
        &&RECTW(ext)<=kOpenMaxCaptureSide&&RECTH(ext)<=kOpenMaxCaptureSide){ *rc=ext; return true; }
    return GetWindowRectPhysical(hwnd,rc);
}
// ---------------------------------------------------------------------------
// Tendina di opacita'.
// Per animare l'apertura serve che la finestra reale venga composta e renderizzata
// davvero (altrimenti il bordo Aero non esiste in nessun buffer), ma non deve
// vedersi mentre l'overlay recita l'animazione. Le strade classiche sono peggiori:
//   * DWMWA_CLOAK: vuota il client di Explorer/Pannello di controllo/Task Manager;
//   * ShowWindow(SW_HIDE): perde il foreground, cambia Z order e fa perdere la
//     superficie di reindirizzamento (dopo serve un nuovo paint completo);
//   * SetWindowRgn vuoto: con una regione non nulla uxtheme smette di tematizzare
//     la finestra, quindi il frame catturato verrebbe "classic".
// Qui si usa solo l'attributo di alpha delle finestre layered: lo stato della
// finestra (WS_VISIBLE, focus, Z order, stile) non cambia, la superficie resta
// valida e la ri-consegna e' pixel-per-pixel perche' cambia solo un moltiplicatore.
// ---------------------------------------------------------------------------
struct CurtainState{ DWORD prevEx=0; BYTE appAlpha=255; bool ownsStyle=false; };
static std::mutex g_curtainMutex; static std::unordered_map<HWND,CurtainState> g_curtains;
static bool CurtainIsApplied(HWND hwnd){
    try{ std::lock_guard<std::mutex> lock(g_curtainMutex); return g_curtains.find(hwnd)!=g_curtains.end(); }
    catch(...){ return false; }
}
static void CurtainGet(HWND hwnd, CurtainState* out){
    if(!out) return;
    try{ std::lock_guard<std::mutex> lock(g_curtainMutex); auto it=g_curtains.find(hwnd); if(it!=g_curtains.end()) *out=it->second; }
    catch(...){}
}
// Restituisce true se la finestra e' adesso coperta (invisibile ma viva e renderizzata).
static bool CurtainApply(HWND hwnd){
    if(!hwnd||!IsWindow(hwnd)) return false;
    if(CurtainIsApplied(hwnd)) return false;
    // Nessuna guardia su SetWindowRgn: con una tendina di OPACITA' la regione della
    // finestra non crea problemi (alpha e regione si combinano senza effetti), e
    // GetWindowRgnBox non e' affidabile ovunque (alcune implementazioni restituiscono
    // errore anche senza regione: usarlo come guardia spegnerebbe la feature in silenzio).
    CurtainState st; st.ownsStyle=true;
    LONG ex=LONG(GetWindowLongPtrW(hwnd,GWL_EXSTYLE));
    st.prevEx=DWORD(ex);
    if(ex&WS_EX_LAYERED){
        // L'app usa gia' il layering (tipico di Electron/Chromium). Si puo' intervenire
        // solo nel caso "alpha unica per tutta la finestra" (LWA_ALPHA): in quel caso si
        // salva il suo alpha e glielo si ridà. Con color key o bitmap per-pixel
        // (UpdateLayeredWindow) non si tocca niente: sovrascriverli romperebbe l'aspetto.
        COLORREF key=0; BYTE alpha=255; DWORD flags=0; st.ownsStyle=false;
        if(!GetLayeredWindowAttributes(hwnd,&key,&alpha,&flags)) return false;
        if(flags!=LWA_ALPHA) return false;
        st.appAlpha=alpha;
    } else {
        SetLastError(0);
        SetWindowLongPtrW(hwnd,GWL_EXSTYLE,LONG_PTR(LONG(ex|WS_EX_LAYERED)));
        if(!(LONG(GetWindowLongPtrW(hwnd,GWL_EXSTYLE))&WS_EX_LAYERED)){ OpenLogWhy(L"curtain: ex-style non applicato"); return false; }
    }
    if(!SetLayeredWindowAttributes(hwnd,0,kOpenHiddenAlpha,LWA_ALPHA)){
        if(st.ownsStyle) SetWindowLongPtrW(hwnd,GWL_EXSTYLE,LONG_PTR(LONG(st.prevEx)));
        OpenLogWhy(L"curtain: SetLayeredWindowAttributes rifiutata");
        return false;
    }
    bool stored=false;
    try{ std::lock_guard<std::mutex> lock(g_curtainMutex); stored=g_curtains.emplace(hwnd,st).second; }
    catch(...){ stored=false; }
    if(!stored){
        SetLayeredWindowAttributes(hwnd,0,st.appAlpha,LWA_ALPHA);
        if(st.ownsStyle) SetWindowLongPtrW(hwnd,GWL_EXSTYLE,LONG_PTR(LONG(st.prevEx)));
        return false;
    }
    return true;
}
static void CurtainSetOpacity(HWND hwnd, BYTE alpha){
    if(!hwnd||!IsWindow(hwnd)) return;
    if(!CurtainIsApplied(hwnd)) return;
    SetLayeredWindowAttributes(hwnd,0,alpha,LWA_ALPHA);
}
// Alpha da usare mentre la finestra deve essere realmente visibile (la cattura).
// Per le finestre layered gestite dall'app si usa il LORO alpha: la bitmap catturata
// deve essere identica a come la finestra si vede davvero, altrimenti al handoff si
// vedrebbe un cambio di opacita'.
static void CurtainShowForCapture(HWND hwnd){
    CurtainState st; CurtainGet(hwnd,&st);
    CurtainSetOpacity(hwnd, st.ownsStyle?BYTE(255):st.appAlpha);
}
// Idempotente: ripristina l'alpha dell'app e toglie WS_EX_LAYERED solo se l'avevamo
// aggiunto noi e se nel frattempo lo stile non e' cambiato. Non lascia mai una finestra
// coperta per sempre.
static void CurtainRelease(HWND hwnd){
    if(!hwnd) return;
    CurtainState st; bool found=false;
    try{ std::lock_guard<std::mutex> lock(g_curtainMutex); auto it=g_curtains.find(hwnd);
        if(it!=g_curtains.end()){ st=it->second; g_curtains.erase(it); found=true; } }
    catch(...){ return; }
    if(!found||!IsWindow(hwnd)) return;
    SetLayeredWindowAttributes(hwnd,0,st.appAlpha,LWA_ALPHA);
    if(st.ownsStyle){
        LONG cur=LONG(GetWindowLongPtrW(hwnd,GWL_EXSTYLE));
        if(cur==LONG(st.prevEx|DWORD(WS_EX_LAYERED))) SetWindowLongPtrW(hwnd,GWL_EXSTYLE,LONG_PTR(LONG(st.prevEx)));
    }
}
static void ReleaseAllCurtains(){
    std::vector<HWND> v;
    try{ std::lock_guard<std::mutex> lock(g_curtainMutex); v.reserve(g_curtains.size()); for(auto& kv:g_curtains) v.push_back(kv.first); }
    catch(...){ return; }
    for(HWND h: v) CurtainRelease(h);
}

static void ForceAeroForCapture(HWND hwnd){
    if(!hwnd||!IsWindow(hwnd)) return;
    // FIX contrasto: non usare MARGINS{-1} che rende finestra full glass trasparente
    // e causa desktop screenshot con strani contrasti. Lasciamo rendering naturale
    // di OpenGlass/DWMBlurGlass. Solo assicuriamo che transizioni DWM siano off.
    // Nessuna ExtendFrame, nessun blur forzato.
}

// Chiusura: screenshot preciso preserva Aero di OpenGlass/DwmBlurGlass
static bool CaptureWindowForClose(HWND hwnd, CaptureBits& out){
    if(!hwnd||!IsWindow(hwnd)) return false;
    RECT rc{}; if(!GetWindowRectPhysical(hwnd,&rc)) return false;
    int w=RECTW(rc), h=RECTH(rc); if(w<1||h<1||w>16384||h>16384) return false;
    ForceAeroForCapture(hwnd);
    BOOL dis=TRUE; DwmSetWindowAttribute(hwnd,DWMWA_TRANSITIONS_FORCEDISABLED,&dis,sizeof(dis));
    // 1) screen scrape con CAPTUREBLT – preserva vetro
    {
        ScopedWindowDc screenDc(nullptr, GetDC(nullptr));
        if(screenDc){
            ScopedDc memDc(CreateCompatibleDC(screenDc.get()));
            if(memDc){
                ScopedGdiObj hBmp(CreateCompatibleBitmap(screenDc.get(), w, h));
                if(hBmp){
                    ScopedSelect sel(memDc.get(), hBmp.get());
                    if(BitBlt(memDc.get(),0,0,w,h,screenDc.get(),rc.left,rc.top,SRCCOPY|0x40000000)){
                        BITMAPINFO bmi{}; bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth=w; bmi.bmiHeader.biHeight=-h; bmi.bmiHeader.biPlanes=1; bmi.bmiHeader.biBitCount=32; bmi.bmiHeader.biCompression=BI_RGB;
                        try{
                            out.width=w; out.height=h; out.pixels.resize(size_t(w)*size_t(h));
                            if(GetDIBits(memDc.get(), (HBITMAP)hBmp.get(), 0, h, out.pixels.data(), &bmi, DIB_RGB_COLORS)){
                                size_t nonBlack=0; for(size_t i=0;i<out.pixels.size()&&nonBlack<100;++i) if((out.pixels[i]&0x00FFFFFF)!=0) ++nonBlack;
                                if(nonBlack>=10){
                                    if(!IsSnippingTool()) ForceOpaqueAlpha(out.pixels.data(), out.pixels.size());
                                    return true;
                                }
                            }
                        }catch(...){ out={}; }
                    }
                }
            }
        }
    }
    // 2) fallback PrintWindow preciso (senza ombra)
    {
        BITMAPINFO bmi{}; bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth=w; bmi.bmiHeader.biHeight=-h; bmi.bmiHeader.biPlanes=1; bmi.bmiHeader.biBitCount=32; bmi.bmiHeader.biCompression=BI_RGB;
        void* bits=nullptr; ScopedWindowDc screenDc(nullptr, GetDC(nullptr)); if(!screenDc) return false;
        ScopedGdiObj dib(CreateDIBSection(screenDc.get(), &bmi, DIB_RGB_COLORS, &bits, nullptr, 0)); if(!dib||!bits) return false;
        ScopedDc memDc(CreateCompatibleDC(screenDc.get())); if(!memDc) return false; ScopedSelect sel(memDc.get(), dib.get());
        bool painted = PrintWindow(hwnd, memDc.get(), PW_RENDERFULLCONTENT)!=FALSE;
        if(!painted) painted = PrintWindow(hwnd, memDc.get(), 0)!=FALSE;
        if(!painted) return false; GdiFlush();
        try{
            out.width=w; out.height=h; out.pixels.resize(size_t(w)*size_t(h));
            std::memcpy(out.pixels.data(), bits, out.pixels.size()*4);
            if(!IsSnippingTool()) ForceOpaqueAlpha(out.pixels.data(), out.pixels.size());
            return true;
        }catch(...){ out={}; return false; }
    }
}
// ---------------------------------------------------------------------------
// Apertura: cattura del RISULTATO COMPOSTO, non del rendering della finestra.
// PrintWindow(PW_RENDERFULLCONTENT) rilegge la superficie della finestra: il
// bordo Aero, l'accento, gli angoli arrotondati e il vetro di OpenGlass /
// DWMBlurGlass sono pero' prodotti nella passata di rendering del DWM, quindi
// non ci sono. E' esattamente il motivo per cui l'overlay "apriva" senza bordo
// e il bordo compariva di colpo alla fine. Si cattura quindi la regione di
// schermo gia' composta, con DWMWA_EXTENDED_FRAME_BOUNDS come bounds.
// ---------------------------------------------------------------------------
// GetDC(NULL) e' ancorato al monitor primario; per finestre su altri monitor si
// usa CreateDC("DISPLAY"), che copre l'intero desktop virtuale.
static HDC AcquireScreenDcForRect(const RECT& rc, bool* owned){
    *owned=false;
    const RECT prim{0,0,(LONG)GetSystemMetrics(SM_CXSCREEN),(LONG)GetSystemMetrics(SM_CYSCREEN)};
    if(rc.left>=prim.left&&rc.top>=prim.top&&rc.right<=prim.right&&rc.bottom<=prim.bottom)
        return GetDC(nullptr);
    HDC disp=CreateDCW(L"DISPLAY",nullptr,nullptr,nullptr);
    if(disp){ *owned=true; return disp; }
    return GetDC(nullptr);
}
static void ReleaseScreenDc(HDC dc, bool owned){ if(!dc) return; if(owned) DeleteDC(dc); else ReleaseDC(nullptr,dc); }

static bool ScrapeScreenRegion(const RECT& rc, CaptureBits& out){
    const int w=RECTW(rc), h=RECTH(rc);
    if(w<8||h<8||w>kOpenMaxCaptureSide||h>kOpenMaxCaptureSide) return false;
    bool owned=false; HDC screen=AcquireScreenDcForRect(rc,&owned);
    if(!screen) return false;
    bool ok=false;
    {
        ScopedDc memDc(CreateCompatibleDC(screen));
        ScopedGdiObj hBmp(memDc.get()?CreateCompatibleBitmap(screen,w,h):nullptr);
        if(memDc&&hBmp){
            ScopedSelect sel(memDc.get(),hBmp.get());
            // CAPTUREBLT serve anche qui: la finestra da leggere e' temporaneamente
            // layered (tendina di opacita') e le layered entrano solo con questo flag.
            if(BitBlt(memDc.get(),0,0,w,h,screen,rc.left,rc.top,SRCCOPY|CAPTUREBLT)){
                GdiFlush();
                BITMAPINFO bmi{}; bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth=w; bmi.bmiHeader.biHeight=-h; bmi.bmiHeader.biPlanes=1; bmi.bmiHeader.biBitCount=32; bmi.bmiHeader.biCompression=BI_RGB;
                try{
                    out.width=w; out.height=h; out.pixels.resize(size_t(w)*size_t(h));
                    if(GetDIBits(memDc.get(),(HBITMAP)hBmp.get(),0,h,out.pixels.data(),&bmi,DIB_RGB_COLORS)==h) ok=true;
                    else out={};
                }catch(...){ out={}; ok=false; }
            }
        }
    }
    ReleaseScreenDc(screen,owned);
    return ok;
}
// Il frame "composto" ha struttura: una finestra non ancora renderizzata e' una
// superficie vuota (nera o di un unico colore). La fascia alta deve contenere il
// bordo/title bar, non solo il client.
static bool CaptureHasStructure(const CaptureBits& c){
    if(c.empty()) return false;
    const int w=c.width,h=c.height; const uint32_t* p=c.pixels.data();
    const int stepX=std::max(1,w/48), stepY=std::max(1,h/48);
    size_t samples=0, nonBlack=0, distinct=1; uint32_t prev=p[0]&0x00FFFFFFu;
    for(int y=0;y<h;y+=stepY){ const uint32_t* row=p+size_t(y)*size_t(w);
        for(int x=0;x<w;x+=stepX){ const uint32_t v=row[x]&0x00FFFFFFu;
            if(v) ++nonBlack;
            if(v!=prev){ ++distinct; prev=v; }
            ++samples; } }
    if(samples<8) return nonBlack>0;
    if(distinct<3) return false;
    if(nonBlack*100<samples*2) return false;
    size_t band=0, bandLit=0;
    const int topRows=std::min(h,std::max(2,h/40));
    for(int y=0;y<topRows;++y){ const uint32_t* row=p+size_t(y)*size_t(w);
        for(int x=0;x<w;x+=stepX){ ++band; if(row[x]&0x00FFFFFFu) ++bandLit; } }
    if(band&&bandLit*4<band) return false;
    return true;
}
// Campione ridotto della regione: serve a confrontare lo stesso rettangolo di
// schermo in due stati diversi senza allocare due bitmap a risoluzione piena.
constexpr int kOpenSigSide = 32;
static bool ScrapeSignature(const RECT& rc, uint32_t* out){
    const int side=kOpenSigSide;
    // Nota: rc puo' avere coordinate negative (monitor a sinistra/sopra il primario).
    if(RECTW(rc)<1||RECTH(rc)<1) return false;
    bool owned=false; HDC screen=AcquireScreenDcForRect(rc,&owned);
    if(!screen||!out){ if(screen) ReleaseScreenDc(screen,owned); return false; }
    bool ok=false;
    {
        ScopedDc memDc(CreateCompatibleDC(screen));
        BITMAPINFO bmi{}; bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth=side; bmi.bmiHeader.biHeight=-side; bmi.bmiHeader.biPlanes=1; bmi.bmiHeader.biBitCount=32; bmi.bmiHeader.biCompression=BI_RGB;
        void* bits=nullptr;
        ScopedGdiObj hBmp(memDc.get()?CreateDIBSection(screen,&bmi,DIB_RGB_COLORS,&bits,nullptr,0):nullptr);
        if(memDc&&hBmp&&bits){
            ScopedSelect sel(memDc.get(),hBmp.get());
            SetStretchBltMode(memDc.get(),COLORONCOLOR);
            if(StretchBlt(memDc.get(),0,0,side,side,screen,rc.left,rc.top,RECTW(rc),RECTH(rc),SRCCOPY|CAPTUREBLT)){
                GdiFlush(); std::memcpy(out,bits,size_t(side)*size_t(side)*4); ok=true;
            }
        }
    }
    ReleaseScreenDc(screen,owned);
    return ok;
}
static size_t SignaturesDiffer(const uint32_t* a, const uint32_t* b){
    const size_t n=size_t(kOpenSigSide)*size_t(kOpenSigSide); size_t diff=0;
    for(size_t i=0;i<n;++i){
        const uint32_t x=a[i]&0x00FFFFFFu, y=b[i]&0x00FFFFFFu;
        const int db=std::abs(int(x&0xFF)-int(y&0xFF)), dg=std::abs(int((x>>8)&0xFF)-int((y>>8)&0xFF)), dr=std::abs(int((x>>16)&0xFF)-int((y>>16)&0xFF));
        if(db>10||dg>10||dr>10) ++diff;
    }
    return diff;
}
// Sfondo della regione dove la finestra sta per aprirsi, campito PRIMA di ShowWindow:
// serve a dimostrare che il frame letto dopo contiene la finestra e non il desktop.
// E' un confronto indipendente da alpha e da hit-testing (una finestra coperta a
// alpha 0 NON viene restituita da WindowFromPoint: l'hit testing delle finestre
// layered ignora le aree con alpha zero, quindi qualunque controllo basato sul
// "point from window" fallirebbe sistematicamente durante l'attesa).
struct OpenBackground{ HWND hwnd=nullptr; RECT rc{}; std::vector<uint32_t> sig; bool valid=false; };
static std::mutex g_openPreMutex; static OpenBackground g_openPre;
static void OpenBackgroundCapture(HWND hwnd, const RECT& rc){
    std::vector<uint32_t> sig;
    try{ sig.assign(size_t(kOpenSigSide)*size_t(kOpenSigSide),0u); }catch(...){ return; }
    bool have = ScrapeSignature(rc,sig.data());
    try{ std::lock_guard<std::mutex> lock(g_openPreMutex);
        g_openPre=OpenBackground{};
        if(have){ g_openPre.hwnd=hwnd; g_openPre.rc=rc; g_openPre.sig=std::move(sig); g_openPre.valid=true; } }
    catch(...){}
}
static void OpenBackgroundClear(HWND hwnd){
    try{ std::lock_guard<std::mutex> lock(g_openPreMutex); if(g_openPre.hwnd==hwnd||hwnd==nullptr) g_openPre=OpenBackground{}; }
    catch(...){}
}
// Copia il fondo registrato (se della stessa finestra e stessi bound).
static bool OpenBackgroundTake(HWND hwnd, const RECT& rc, std::vector<uint32_t>& out){
    bool ok=false;
    try{ std::lock_guard<std::mutex> lock(g_openPreMutex);
        if(g_openPre.valid&&g_openPre.hwnd==hwnd&&RectEquals(g_openPre.rc,rc)){ out=g_openPre.sig; ok=true; } }
    catch(...){}
    return ok;
}
// Cattura il frame composto e ne verifica la consistenza: struttura dell'immagine e
// differenza rispetto allo sfondo precendente la finestra. Se il confronto non e'
// possibile (rect cambiato tra l'arm e la cattura) ci si ferma alla struttura: meglio
// un quadro accettato con un check in meno che l'animazione disattivata in silenzio.
static bool GrabComposedFrame(HWND hwnd, const RECT& rc, CaptureBits& cap){
    if(!hwnd||!IsWindow(hwnd)) return false;
    bool ok=false;
    CurtainShowForCapture(hwnd);
    DwmFlush();                                        // il DWM deve comporre lo stato "finestra visibile"
    if(ScrapeScreenRegion(rc,cap)&&CaptureHasStructure(cap)){
        std::vector<uint32_t> pre;
        if(OpenBackgroundTake(hwnd,rc,pre)){
            std::vector<uint32_t> now;
            try{ now.assign(pre.size(),0u); }catch(...){ now.clear(); }
            if(!now.empty()&&ScrapeSignature(rc,now.data())){
                const size_t n=pre.size();
                if(SignaturesDiffer(pre.data(),now.data())>=(n/16)) ok=true;
                else { cap={}; OpenLogWhy(L"capture identica allo sfondo: la finestra non era nel frame"); }
            } else ok=true;                            // firma non ottenibile: non si blocca l'animazione per questo
        } else ok=true;
    } else {
        cap={};
    }
    CurtainSetOpacity(hwnd,kOpenHiddenAlpha);          // ricoperta subito, prima di qualunque altra cosa
    if(ok&&!IsSnippingTool()) ForceOpaqueAlpha(cap.pixels.data(),cap.pixels.size());  // l'alpha del DC non e' definito
    if(!ok) cap={};
    return ok;
}
static bool CaptureWindowForOpen(HWND hwnd, CaptureBits& out, RECT* rcOut){
    if(!hwnd||!IsWindow(hwnd)) return false;
    RECT rc{}; if(!GetFrameBoundsPhysical(hwnd,&rc)) return false;
    if(!GrabComposedFrame(hwnd,rc,out)) return false;
    if(rcOut) *rcOut=rc;
    return true;
}
// Nessun fallback PrintWindow per l'apertura, di proposito: una bitmap senza il
// frame ricomporrebbe esattamente il difetto che si vuole eliminare (bordo che
// compare alla fine). Se lo scrape non da' un frame valido l'animazione si
// annulla e Windows mostra la finestra nel modo nativo.

static bool GetMinimizeRectPhysical(HWND hwnd,RECT* rc){
    if(pGetWindowMinimizeRect&&pGetWindowMinimizeRect(hwnd,rc)&&IsRectUsable(*rc)) return true;
    HMONITOR hmon=MonitorFromWindow(hwnd,MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if(!GetMonitorInfoW(hmon,&mi)) return false;
    rc->left=mi.rcWork.left+8;
    rc->bottom=mi.rcWork.bottom-8;
    rc->right=rc->left+24;
    rc->top=rc->bottom-24;
    return IsRectUsable(*rc);
}
static bool GetMaximizeRectPhysical(HWND hwnd,RECT* rc){
    MINMAXINFO mmi{};
    HMONITOR hmon=MonitorFromWindow(hwnd,MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if(!GetMonitorInfoW(hmon,&mi)) return false;
    RECT work=mi.rcWork;
    mmi.ptMaxPosition.x=work.left;
    mmi.ptMaxPosition.y=work.top;
    mmi.ptMaxSize.x=RECTW(work);
    mmi.ptMaxSize.y=RECTH(work);
    if(!SendMessageTimeoutW(hwnd,WM_GETMINMAXINFO,0,LPARAM(&mmi),SMTO_ABORTIFHUNG,100,nullptr)) return false;
    if(mmi.ptMaxSize.x<=0||mmi.ptMaxSize.y<=0) return false;
    rc->left=mmi.ptMaxPosition.x;
    rc->top=mmi.ptMaxPosition.y;
    rc->right=rc->left+mmi.ptMaxSize.x;
    rc->bottom=rc->top+mmi.ptMaxSize.y;
    return IsRectUsable(*rc);
}
static bool GetRestoreRectPhysical(HWND hwnd,RECT* rc){
    if(!rc) return false;
    WINDOWPLACEMENT wp{sizeof(wp)};
    if(!GetWindowPlacement(hwnd,&wp)) return false;
    *rc=wp.rcNormalPosition;
    LONG style=LONG(GetWindowLongPtrW(hwnd,GWL_STYLE));
    if(!(style & WS_CHILD)){
        HMONITOR hmon=MonitorFromWindow(hwnd,MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{sizeof(mi)};
        if(hmon&&GetMonitorInfoW(hmon,&mi)){
            OffsetRect(rc,-(mi.rcMonitor.left-mi.rcWork.left),-(mi.rcMonitor.top-mi.rcWork.top));
        }
    } else {
        if(HWND parent=GetParent(hwnd)){
            MapWindowPoints(parent,HWND_DESKTOP,LPPOINT(rc),2);
        }
    }
    return IsRectUsable(*rc);
}
static void DisableTransitions(HWND hwnd,BOOL dis){ if(!hwnd||!IsWindow(hwnd)) return; DwmSetWindowAttribute(hwnd,DWMWA_TRANSITIONS_FORCEDISABLED,&dis,sizeof(dis)); }

struct Vertex{ float x,y,u,v; };
static float Edge(const Vertex& a,const Vertex& b,const Vertex& c){ return (c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x); }
static uint32_t SampleBilinear(const uint32_t* src,int sw,int sh,float u,float v){
    u=std::clamp(u,0.f,1.f)*float(sw-1); v=std::clamp(v,0.f,1.f)*float(sh-1);
    int x0=std::clamp(int(u),0,sw-1), y0=std::clamp(int(v),0,sh-1), x1=std::min(x0+1,sw-1), y1=std::min(y0+1,sh-1);
    float fx=u-float(x0), fy=v-float(y0);
    auto unpack=[](uint32_t p,float& b,float& g,float& r){ b=float(p&0xFF); g=float((p>>8)&0xFF); r=float((p>>16)&0xFF); };
    float b00,g00,r00,b10,g10,r10,b01,g01,r01,b11,g11,r11;
    unpack(src[y0*sw+x0],b00,g00,r00); unpack(src[y0*sw+x1],b10,g10,r10); unpack(src[y1*sw+x0],b01,g01,r01); unpack(src[y1*sw+x1],b11,g11,r11);
    float b0=b00+(b10-b00)*fx, g0=g00+(g10-g00)*fx, r0=r00+(r10-r00)*fx, b1=b01+(b11-b01)*fx, g1=g01+(g11-g01)*fx, r1=r01+(r11-r01)*fx;
    BYTE b=BYTE(b0+(b1-b0)*fy+0.5f), g=BYTE(g0+(g1-g0)*fy+0.5f), r=BYTE(r0+(r1-r0)*fy+0.5f);
    return uint32_t(b)|(uint32_t(g)<<8)|(uint32_t(r)<<16);
}
static void RasterTriangle(uint32_t* dst,int stride,int dw,int dh,const uint32_t* src,int sw,int sh,Vertex v0,Vertex v1,Vertex v2,BYTE alpha){
    float area=Edge(v0,v1,v2); if(std::fabs(area)<0.5f) return; float inv=1.f/area;
    int minX=int(std::floor(std::min({v0.x,v1.x,v2.x}))), maxX=int(std::ceil(std::max({v0.x,v1.x,v2.x})));
    int minY=int(std::floor(std::min({v0.y,v1.y,v2.y}))), maxY=int(std::ceil(std::max({v0.y,v1.y,v2.y})));
    minX=std::clamp(minX,0,dw-1); maxX=std::clamp(maxX,0,dw-1); minY=std::clamp(minY,0,dh-1); maxY=std::clamp(maxY,0,dh-1);
    float af=float(alpha)/255.f;
    for(int y=minY;y<=maxY;++y){ uint32_t* row=dst+size_t(y)*size_t(stride);
        for(int x=minX;x<=maxX;++x){ Vertex p{float(x)+0.5f,float(y)+0.5f,0,0}; float w0=Edge(v1,v2,p)*inv,w1=Edge(v2,v0,p)*inv,w2=Edge(v0,v1,p)*inv; if(w0<0||w1<0||w2<0) continue;
            float u=w0*v0.u+w1*v1.u+w2*v2.u, v=w0*v0.v+w1*v1.v+w2*v2.v; uint32_t s=SampleBilinear(src,sw,sh,u,v);
            float sb=float(s&0xFF), sg=float((s>>8)&0xFF), sr=float((s>>16)&0xFF);
            BYTE b=BYTE(sb*af+0.5f), g=BYTE(sg*af+0.5f), r=BYTE(sr*af+0.5f);
            row[x]=uint32_t(b)|(uint32_t(g)<<8)|(uint32_t(r)<<16)|(uint32_t(alpha)<<24);
        }
    }
}
static void RasterQuad(uint32_t* dst,int stride,int dw,int dh,const uint32_t* src,int sw,int sh,const Vertex c[4],BYTE alpha){
    if(dw<=0||dh<=0||sw<=0||sh<=0||!dst||!src||alpha==0) return;
    RasterTriangle(dst,stride,dw,dh,src,sw,sh,c[0],c[1],c[2],alpha); RasterTriangle(dst,stride,dw,dh,src,sw,sh,c[0],c[2],c[3],alpha);
}
class PresentGdi{
   public:
    PresentGdi()=default; PresentGdi(const PresentGdi&)=delete; PresentGdi& operator=(const PresentGdi&)=delete; ~PresentGdi(){Release();}
    bool EnsureSource(const CaptureBits& cap){
        if(m_hdcSrc) return true; if(!EnsureScreenDc()) return false;
        BITMAPINFO bmi{}; bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth=cap.width; bmi.bmiHeader.biHeight=-cap.height; bmi.bmiHeader.biPlanes=1; bmi.bmiHeader.biBitCount=32; bmi.bmiHeader.biCompression=BI_RGB;
        m_hbmSrc=CreateDIBSection(m_hdcScreen,&bmi,DIB_RGB_COLORS,&m_pvSrc,nullptr,0); if(!m_hbmSrc||!m_pvSrc){Release(); return false;}
        std::memcpy(m_pvSrc,cap.pixels.data(),cap.pixels.size()*4); GdiFlush(); m_hdcSrc=CreateCompatibleDC(m_hdcScreen); if(!m_hdcSrc){Release(); return false;} SelectObject(m_hdcSrc,m_hbmSrc); return true;
    }
    bool EnsureDest(int dw,int dh){
        if(m_hdcDst&&dw<=m_dstW&&dh<=m_dstH) return true;
        if(m_hdcDst){DeleteDC(m_hdcDst); m_hdcDst=nullptr;} if(m_hbmDst){DeleteObject(m_hbmDst); m_hbmDst=nullptr; m_pvDst=nullptr;}
        if(!EnsureScreenDc()) return false;
        BITMAPINFO bmi{}; bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth=dw; bmi.bmiHeader.biHeight=-dh; bmi.bmiHeader.biPlanes=1; bmi.bmiHeader.biBitCount=32; bmi.bmiHeader.biCompression=BI_RGB;
        m_hbmDst=CreateDIBSection(m_hdcScreen,&bmi,DIB_RGB_COLORS,&m_pvDst,nullptr,0); if(!m_hbmDst||!m_pvDst){Release(); return false;}
        m_hdcDst=CreateCompatibleDC(m_hdcScreen); if(!m_hdcDst){Release(); return false;} m_dstW=dw; m_dstH=dh; SelectObject(m_hdcDst,m_hbmDst); SetStretchBltMode(m_hdcDst,HALFTONE); SetBrushOrgEx(m_hdcDst,0,0,nullptr); return true;
    }
    void Release(){ if(m_hdcDst){DeleteDC(m_hdcDst); m_hdcDst=nullptr;} if(m_hbmDst){DeleteObject(m_hbmDst); m_hbmDst=nullptr; m_pvDst=nullptr;} if(m_hdcSrc){DeleteDC(m_hdcSrc); m_hdcSrc=nullptr;} if(m_hbmSrc){DeleteObject(m_hbmSrc); m_hbmSrc=nullptr; m_pvSrc=nullptr;} if(m_hdcScreen){ReleaseDC(nullptr,m_hdcScreen); m_hdcScreen=nullptr;} }
    HDC hdcScreen() const {return m_hdcScreen;} HDC hdcDst() const {return m_hdcDst;} HDC hdcSrc() const {return m_hdcSrc;} void* dstBits() const {return m_pvDst;} int dstStride() const {return m_dstW;}
   private:
    bool EnsureScreenDc(){ if(m_hdcScreen) return true; m_hdcScreen=GetDC(nullptr); return m_hdcScreen!=nullptr; }
    HDC m_hdcScreen=nullptr,m_hdcSrc=nullptr,m_hdcDst=nullptr; HGDIOBJ m_hbmSrc=nullptr,m_hbmDst=nullptr; void* m_pvSrc=nullptr,*m_pvDst=nullptr; int m_dstW=0,m_dstH=0;
};
struct AnimRequest{ HWND hwnd=nullptr; AnimationType type=AnimationType::None; RECT rcWindow{}; RECT rcDest{}; CaptureBits capture; UINT durationMs=250; bool deferOrig=false; int origShowCmd=0; bool needsCapture=false; std::unique_ptr<PresentGdi> gdi; };

static bool PresentOverlay(HWND hwndOverlay, PresentGdi& gdi, const AnimRequest& req, const RECT& rcCurrent, const Win7TransformParams& params) {
    BYTE alpha = BYTE(std::clamp(params.opacity, 0.f, 1.f) * 255.f + 0.5f);
    if (alpha == 0 || !IsRectUsable(rcCurrent)) {
        POINT pt{rcCurrent.left, rcCurrent.top}; SIZE sz{1,1}; BLENDFUNCTION bf{AC_SRC_OVER,0,0,AC_SRC_ALPHA};
        UpdateLayeredWindow(hwndOverlay,nullptr,&pt,&sz,nullptr,nullptr,0,&bf,ULW_ALPHA); return true;
    }
    const CaptureBits& cap=req.capture; float ow=float(cap.width), oh=float(cap.height);
    bool isClose=(req.type==AnimationType::Close), isOpen=(req.type==AnimationType::Open);
    bool tiny3d=!isClose&&!isOpen&&std::fabs(params.rotX)<0.35f&&std::fabs(params.rotY)<0.35f&&std::fabs(params.transZ)<0.25f;
    RECT bbox=rcCurrent; Vertex corners[4]{};
    if(!tiny3d){
        Matrix4x4F m=BuildCornerMatrix(params,rcCurrent,ow,oh,req.type);
        float xs[4]={0,ow,ow,0}, ys[4]={0,0,oh,oh}, us[4]={0,1,1,0}, vs[4]={0,0,1,1};
        float minX=1e9f,minY=1e9f,maxX=-1e9f,maxY=-1e9f,sx[4],sy[4];
        for(int i=0;i<4;++i){ m.TransformPoint(xs[i],ys[i],0,sx[i],sy[i]); minX=std::min(minX,sx[i]); minY=std::min(minY,sy[i]); maxX=std::max(maxX,sx[i]); maxY=std::max(maxY,sy[i]); }
        bbox.left=LONG(std::floor(minX))-3; bbox.top=LONG(std::floor(minY))-3; bbox.right=LONG(std::ceil(maxX))+3; bbox.bottom=LONG(std::ceil(maxY))+3;
        for(int i=0;i<4;++i) corners[i]={sx[i]-bbox.left,sy[i]-bbox.top,us[i],vs[i]};
    }
    if(!IsRectUsable(bbox)) return false; int dw=RECTW(bbox), dh=RECTH(bbox); if(dw>16384||dh>16384) return false;
    if(!gdi.EnsureSource(cap)) return false; if(!gdi.EnsureDest(dw,dh)) return false;
    void* bits=gdi.dstBits(); int stride=gdi.dstStride(); std::memset(bits,0,size_t(stride)*size_t(dh)*4);
    if(tiny3d){
        StretchBlt(gdi.hdcDst(),0,0,dw,dh,gdi.hdcSrc(),0,0,cap.width,cap.height,SRCCOPY); GdiFlush();
        auto* px=static_cast<uint32_t*>(bits);
        for(int y=0;y<dh;++y){ uint32_t* row=px+size_t(y)*size_t(stride); for(int x=0;x<dw;++x){ uint32_t p=row[x]; BYTE b=BYTE(((p&0xFF)*alpha)/255), g=BYTE((((p>>8)&0xFF)*alpha)/255), r=BYTE((((p>>16)&0xFF)*alpha)/255); row[x]=uint32_t(b)|(uint32_t(g)<<8)|(uint32_t(r)<<16)|(uint32_t(alpha)<<24); } }
    } else {
        RasterQuad(static_cast<uint32_t*>(bits),stride,dw,dh,cap.pixels.data(),cap.width,cap.height,corners,alpha);
    }
    POINT pt{bbox.left,bbox.top}; SIZE sz{dw,dh}; POINT srcPt{0,0}; BLENDFUNCTION bf{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
    if(!UpdateLayeredWindow(hwndOverlay,gdi.hdcScreen(),&pt,&sz,gdi.hdcDst(),&srcPt,0,&bf,ULW_ALPHA)) return false; return true;
}
static void ShowOverlayWindow(HWND hwndOverlay){ if(ShowWindow_orig) ShowWindow_orig(hwndOverlay,SW_SHOWNA); else ::ShowWindow(hwndOverlay,SW_SHOWNA); SetWindowPos(hwndOverlay,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE); }
static void HideOverlayWindow(HWND hwndOverlay){
    POINT pt{0,0}; SIZE sz{1,1}; BLENDFUNCTION bf{AC_SRC_OVER,0,0,AC_SRC_ALPHA};
    UpdateLayeredWindow(hwndOverlay,nullptr,&pt,&sz,nullptr,nullptr,0,&bf,ULW_ALPHA);
    SetWindowPos(hwndOverlay,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_HIDEWINDOW);
}

enum class AnimMsg:UINT{ FirstFrame=1,Drain=2,Hide=3 };
static std::atomic<UINT> g_msgAnim{0}; static std::atomic<bool> g_fAnimating{false}; static std::atomic<bool> g_fDisabled{false};
static std::atomic<HWND> g_hwndAnim{nullptr}, g_hwndCurrent{nullptr}; static std::atomic<int> g_typeCurrent{0};
static std::atomic<bool> g_stopping{false};
static LONG g_isUninitializing = 0; // non-atomic for InterlockedExchange
// Stato dell'apertura: hwnd "armato" (gia' coperto dalla tendina, in attesa che il
// DWM componga) e timestamp dell'arm per il dead-man switch.
static std::atomic<HWND> g_hwndOpenArmed{nullptr}; static std::atomic<ULONGLONG> g_ulOpenArmedTick{0};
static HINSTANCE g_hinst=nullptr; static std::mutex g_animThreadMutex; static HANDLE g_hAnimWndThread=nullptr; static DWORD g_dwAnimThreadId=0;
static std::mutex g_queueMutex; static std::deque<AnimRequest*> g_queue;
static void PresentTime(HWND hwndOverlay,AnimRequest& req,float t){ auto p=ParamsFor(req.type,t,float(RECTH(req.rcWindow))); auto rc=RectFor(req.type,t,req.rcWindow,req.rcDest); PresentOverlay(hwndOverlay,*req.gdi,req,rc,p); }
static void PresentFirstFrame(HWND hwndOverlay,AnimRequest& req){ if(req.capture.empty()||!IsRectUsable(req.rcWindow)) return; ShowOverlayWindow(hwndOverlay); PresentTime(hwndOverlay,req,0.f); }
static void RunAnimation(HWND hwndOverlay,AnimRequest& req){
    if(req.capture.empty()||!IsRectUsable(req.rcWindow)) return; if(g_stopping.load()) return;
    UINT dur=req.durationMs?req.durationMs:250; ShowOverlayWindow(hwndOverlay); const DWORD kFrame=16; ULONGLONG start=GetTickCount64(), elapsed=0; float lastT=-1;
    while(!g_stopping.load()&&g_hwndCurrent.load()==req.hwnd&&(elapsed=GetTickCount64()-start)<=dur){
        float t=dur==0?1.f:std::clamp(float(elapsed)/float(dur),0.f,1.f); if(t-lastT<0.001f){ ULONGLONG nf=start+ULONGLONG((lastT+0.001f)*dur); ULONGLONG now=GetTickCount64(); if(now<nf) Sleep(DWORD(nf-now)); continue; } lastT=t; PresentTime(hwndOverlay,req,t);
        ULONGLONG fe=GetTickCount64(); ULONGLONG tt=start+ULONGLONG(t*dur)+kFrame; if(fe<tt) Sleep(DWORD(tt-fe));
    }
    if(!g_stopping.load()) PresentTime(hwndOverlay,req,1.f);
}
// La finestra deve essere leggibile per intero: se un'altra finestra la copre si
// catturerebbe l'altra finestra al posto del suo bordo. Si cammina la catena
// Z-order (GW_HWNDPREV) perche' e' l'unico metodo che non dipende da cosa il DWM
// mostra in questo momento: la finestra da animare, coperta dalla tendina, ha
// alpha 0 e quindi e' trasparentemente "click-through" per l'hit testing.
static bool OpenRegionIsOnTop(HWND hwnd, const RECT& rc){
    if(!hwnd||!IsWindow(hwnd)) return false;
    const HWND overlay=g_hwndAnim.load();
    for(HWND cur=GetWindow(hwnd,GW_HWNDPREV); cur&&cur!=overlay; cur=GetWindow(cur,GW_HWNDPREV)){
        if(cur==overlay) continue;                        // il nostro overlay e' topmost per progetto
        if(!IsWindowVisible(cur)) continue;
        if(GetAncestor(cur,GA_ROOT)==hwnd) continue;       // figli/owned della stessa finestra: fa parte del frame
        LONG ex=LONG(GetWindowLongPtrW(cur,GWL_EXSTYLE));
        if(ex&WS_EX_LAYERED){
            COLORREF key=0; BYTE al=255; DWORD fl=0;
            if(GetLayeredWindowAttributes(cur,&key,&al,&fl)&&al==0) continue;   // gia' invisibile: non occlude
        }
        RECT r{}; if(!GetWindowRectPhysical(cur,&r)||!IsRectUsable(r)) continue;
        if(r.left>=rc.right||r.right<=rc.left||r.top>=rc.bottom||r.bottom<=rc.top) continue;
        const LONG ix=std::min(r.right,rc.right)-std::max(r.left,rc.left);
        const LONG iy=std::min(r.bottom,rc.bottom)-std::max(r.top,rc.top);
        if(ix<=8||iy<=8) continue;                        // sfioramento: il bordo invisibile non conta
        return false;
    }
    return true;
}
// Attende che il DWM abbia composto la finestra reale e ne legge il frame finale.
// Viene eseguito sull'overlay thread, mai sul thread UI dell'applicazione: se il
// thread UI si fermasse qui dentro (come faceva il vecchio codice inline) la
// finestra non riceverebbe mai WM_PAINT, e la cattura sarebbe un client vuoto.
// La finestra sta coperta dalla tendina (alpha 0) per tutta l'attesa: diventa
// opaca solo nello strettissimo intervallo tra due compositioni necessario alla
// lettura, e torna coperta prima che l'overlay mostri qualcosa.
static bool ProbeComposedWindow(HWND hwnd, CaptureBits& cap, RECT& rcOut){
    if(!hwnd||!IsWindow(hwnd)) return false;
    const ULONGLONG start=GetTickCount64();
    RECT last{}; bool haveRect=false; UINT stable=0, probes=0; bool ok=false; bool pendingPaint=false;
    while(!g_stopping.load()&&g_hwndCurrent.load()==hwnd&&(GetTickCount64()-start)<kOpenSettleTimeoutMs){
        if(!IsWindow(hwnd)||!IsWindowVisible(hwnd)||IsIconic(hwnd)) break;
        RECT rc{}; if(!GetFrameBoundsPhysical(hwnd,&rc)) break;
        if(haveRect&&RectEquals(rc,last)){ if(stable<0xFFFF) ++stable; } else stable=0;
        haveRect=true; last=rc;
        RECT upd{}; pendingPaint = GetUpdateRect(hwnd,&upd,FALSE)!=FALSE;
        // Condizione normale: geometria ferma per qualche composizione E coda di
        // pittura smaltita, quindi l'app ha renderizzato davvero. All'ultimo tentativo
        // si accetta anche una finestra che invalida di continuo (UI animate, video).
        const bool ready = !pendingPaint && stable>=kOpenMinStableFrames;
        // Dopo il primo tentativo andato a vuoto (contenuto non valido o non leggibile)
        // si accetta anche una finestra che si ridipinge di continuo: attendere che la
        // sua coda di pittura si svuoti non ha senso, e un frame "in movimento" della
        // finestra giusta e' comunque meglio di nessuna animazione.
        const bool desperate = stable>=kOpenStableNoPaint && probes>=1;
        if((ready||desperate)&&probes<kOpenMaxProbes){
            if(!OpenRegionIsOnTop(hwnd,rc)){ OpenLogWhy(L"annullata: altra finestra sopra (z-order)"); break; }
            ++probes;
            RECT got{};
            ok = CaptureWindowForOpen(hwnd,cap,&got) && RectEquals(got,rc);   // (ri)copre la finestra da se'
            if(ok) break;
            RedrawWindow(hwnd,nullptr,nullptr,RDW_INVALIDATE|RDW_FRAME|RDW_ERASE|RDW_ALLCHILDREN);
        }
        DwmFlush();                                       // un giro di DWM = un frame, niente sleep a occhio
        Sleep(1);                                         // lascia girare il message loop dell'app
    }
    if(!ok) cap={};
    if(!ok) OpenLogWhy(L"annullata: nessun frame composto valido (stable=%u probe=%u paint=%d)",
                    stable, probes, int(pendingPaint));
    return ok;
}
// La richiesta non sara' mai animata: si toglie la tendina e si lasciano a Windows
// sia la finestra sia le sue transizioni.
static void OpenCancelCleanup(HWND hwnd){
    OpenBackgroundClear(hwnd);
    if(!hwnd) return;
    if(IsWindow(hwnd)){ CurtainRelease(hwnd); DisableTransitions(hwnd,FALSE); }
}
static bool PrepareOpenAnimation(HWND hwndOverlay, AnimRequest& req){
    if(!req.hwnd||!IsWindow(req.hwnd)) return false;
    CaptureBits cap; RECT rc{};
    const bool got = ProbeComposedWindow(req.hwnd,cap,rc);
    OpenBackgroundClear(req.hwnd);
    if(!got) return false;
    if(cap.empty()||!IsRectUsable(rc)) return false;
    req.capture=std::move(cap); req.rcWindow=rc; req.rcDest=rc;
    OpenLog(L"frame composto catturato: %dx%d a [%d,%d]", RECTW(rc), RECTH(rc), rc.left, rc.top);
    // Il primo frame dell'overlay parte subito, mentre la finestra reale e' gia'
    // di nuovo coperta: non esiste mai un frame con entrambe visibili.
    PresentFirstFrame(hwndOverlay,req);
    return true;
}
// Annullamento: nessuna animazione forzata, la finestra torna visibile e Windows
// si comporta in modo nativo. Usato solo dal percorso Open.
static void AbortRequest(AnimRequest* req){
    if(!req) return;
    OpenCancelCleanup(req->hwnd);
    HWND ha=g_hwndAnim.load(); if(ha&&IsWindow(ha)) HideOverlayWindow(ha);
    HWND cur=g_hwndCurrent.load(); if(cur==req->hwnd){ g_hwndCurrent.store(nullptr); g_typeCurrent.store(0); }
    g_fAnimating.store(false);
    delete req;
}
// Consegna finale: l'overlay e' gia' alla geometria finale e opaco, la finestra
// reale viene resa visibile SOTTO di esso e lasciata comporre dal DWM prima che
// l'overlay sparisca. Se nel frattempo la finestra si e' mossa, l'ultimo frame
// viene riallineato ai suoi bound reali per non far vedere uno scatto.
static void HandoffOpen(AnimRequest* req){
    if(!req) return;
    if(!req->hwnd||!IsWindow(req->hwnd)){ return; }
    { ScopedDpiAware dpi;
        if(!req->capture.empty()&&req->gdi){
            HWND ha=g_hwndAnim.load(); RECT now{};
            if(ha&&IsWindow(ha)&&GetFrameBoundsPhysical(req->hwnd,&now)&&!RectEquals(now,req->rcWindow)){
                Win7TransformParams p; p.opacity=1.f; p.ease=1.f; p.rotX=0.f; p.rotY=0.f;
                PresentOverlay(ha,*req->gdi,*req,now,p);
                req->rcWindow=now; req->rcDest=now;
            }
        }
        CurtainRelease(req->hwnd);
        const bool iconic = IsIconic(req->hwnd)!=FALSE;
        if(!iconic) RedrawWindow(req->hwnd,nullptr,nullptr,RDW_INVALIDATE|RDW_FRAME|RDW_ERASE|RDW_ALLCHILDREN);
        // L'overlay resta su' finche' la finestra reale non ha finito di ridipingersi:
        // la rimozione avviene solo a finestra pronta, e con l'overlay che la copre.
        const ULONGLONG hstart=GetTickCount64();
        for(;;){
            RECT upd{}; const bool pending = !iconic && GetUpdateRect(req->hwnd,&upd,FALSE)!=FALSE;
            DwmFlush();
            if(!pending) break;
            if(GetTickCount64()-hstart>kOpenHandoffWaitMs) break;
            Sleep(1);
        }
    }
}
static void FinishQueued(AnimRequest* req){
    if(!req) return; if(req->deferOrig&&req->hwnd&&IsWindow(req->hwnd)&&ShowWindowAsync_orig){ int cmd=req->origShowCmd?req->origShowCmd:SW_RESTORE; ShowWindowAsync_orig(req->hwnd,cmd);
        if(!g_stopping.load()){ ULONGLONG ws=GetTickCount64(); const ULONGLONG kMax=120; while(!g_stopping.load()&&IsWindow(req->hwnd)&&IsIconic(req->hwnd)&&(GetTickCount64()-ws)<kMax) Sleep(1);
            HWND ha=g_hwndAnim.load(); RECT rcNow{}; if(ha&&IsWindow(ha)&&GetWindowRectPhysical(req->hwnd,&rcNow)&&IsRectUsable(rcNow)&&req->gdi){ Win7TransformParams p; p.opacity=1; PresentOverlay(ha,*req->gdi,*req,rcNow,p);} if(!g_stopping.load()) Sleep(1); } }
    if(req->type==AnimationType::Open) HandoffOpen(req);
    HWND ha=g_hwndAnim.load(); if(ha&&IsWindow(ha)) HideOverlayWindow(ha); if(req->hwnd&&IsWindow(req->hwnd)) DisableTransitions(req->hwnd,FALSE);
    HWND cur=g_hwndCurrent.load(); if(cur==req->hwnd){ g_hwndCurrent.store(nullptr); g_typeCurrent.store(0); g_fAnimating.store(false);} delete req;
}
static void DrainQueue(HWND hwndOverlay){ for(;;){ if(g_stopping.load()) break; AnimRequest* req=nullptr; { std::lock_guard<std::mutex> lock(g_queueMutex); if(g_queue.empty()) break; req=g_queue.front(); g_queue.pop_front(); } if(!req) continue;
    if(req->needsCapture){ if(!PrepareOpenAnimation(hwndOverlay,*req)){ AbortRequest(req); continue; } }
    RunAnimation(hwndOverlay,*req); FinishQueued(req); } }
static const wchar_t kAnimClassName[]=L"Windhawk_Win7AeroAnim";
// Rilascia la tendina di un'apertura "armata" che non ha mai ottenuto la cattura.
static void WatchdogOpenCurtain(){
    HWND hwnd=g_hwndOpenArmed.load();
    if(!hwnd) return;
    const ULONGLONG tick=g_ulOpenArmedTick.load();
    if(tick&&GetTickCount64()-tick<kOpenWatchdogMs) return;
    g_hwndOpenArmed.store(nullptr);
    OpenBackgroundClear(hwnd);
    if(IsWindow(hwnd)){ CurtainRelease(hwnd); DisableTransitions(hwnd,FALSE); }
    if(g_hwndCurrent.load()!=hwnd) g_fAnimating.store(false);   // nessuna animazione in corso su di essa
}
static LRESULT CALLBACK AnimWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
    UINT msg=g_msgAnim.load(); if(msg&&uMsg==msg){ switch(AnimMsg(wParam)){ case AnimMsg::FirstFrame: if(lParam) PresentFirstFrame(hwnd,*reinterpret_cast<AnimRequest*>(lParam)); break; case AnimMsg::Drain: DrainQueue(hwnd); break; case AnimMsg::Hide: HideOverlayWindow(hwnd); break; } return 0; }
    switch(uMsg){ case WM_PAINT: case WM_ERASEBKGND: return 0; case WM_NCHITTEST: return HTNOWHERE;
    case WM_TIMER: if(wParam==WPARAM(kOpenWatchdogTimer)){ WatchdogOpenCurtain(); if(!g_hwndOpenArmed.load()) KillTimer(hwnd,kOpenWatchdogTimer); } return 0;
    case WM_CLOSE: DestroyWindow(hwnd); return 0; case WM_DESTROY: PostQuitMessage(0); return 0; default: return DefWindowProcW(hwnd,uMsg,wParam,lParam); }
}
static DWORD CALLBACK AnimWndThreadProc(HANDLE hEvent){
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    WNDCLASSW wc{}; wc.lpfnWndProc=AnimWndProc; wc.hInstance=g_hinst; wc.lpszClassName=kAnimClassName;
    if(!RegisterClassW(&wc)){ SetEvent(hEvent); return 0; }
    HWND hwnd=CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,kAnimClassName,nullptr,WS_POPUP,0,0,0,0,nullptr,nullptr,g_hinst,nullptr);
    if(!hwnd){ UnregisterClassW(kAnimClassName,g_hinst); SetEvent(hEvent); return 0; }
    LONG_PTR ex=GetWindowLongPtrW(hwnd,GWL_EXSTYLE); SetWindowLongPtrW(hwnd,GWL_EXSTYLE,ex|WS_EX_LAYERED|WS_EX_TRANSPARENT);
    g_hwndAnim.store(hwnd); g_msgAnim.store(RegisterWindowMessageW(L"Windhawk_Win7AeroAnim_Run")); SetEvent(hEvent);
    MSG msg; while(GetMessageW(&msg,nullptr,0,0)){ TranslateMessage(&msg); DispatchMessageW(&msg); }
    g_hwndAnim.store(nullptr); UnregisterClassW(kAnimClassName,g_hinst); return 0;
}
static bool WaitForAnimWndThread(){
    if(g_stopping.load()) return false; std::lock_guard<std::mutex> lock(g_animThreadMutex); if(g_stopping.load()) return false;
    if(g_hAnimWndThread){ HWND ha=g_hwndAnim.load(); if(ha&&IsWindow(ha)) return true; if(g_dwAnimThreadId) PostThreadMessageW(g_dwAnimThreadId,WM_QUIT,0,0);
        DWORD r=WaitForSingleObject(g_hAnimWndThread,2000); if(r==WAIT_TIMEOUT){ g_fDisabled.store(true); CloseHandle(g_hAnimWndThread); g_hAnimWndThread=nullptr; g_dwAnimThreadId=0; return false; }
        CloseHandle(g_hAnimWndThread); g_hAnimWndThread=nullptr; g_dwAnimThreadId=0;
    }
    HANDLE hEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr); if(!hEvent) return false;
    g_hAnimWndThread=CreateThread(nullptr,0,AnimWndThreadProc,hEvent,0,nullptr); if(!g_hAnimWndThread){CloseHandle(hEvent); return false;}
    g_dwAnimThreadId=GetThreadId(g_hAnimWndThread); DWORD w=WaitForSingleObject(hEvent,1000); if(w==WAIT_OBJECT_0) CloseHandle(hEvent); else { g_fDisabled.store(true); return false; }
    if(!g_hwndAnim.load()){ g_fDisabled.store(true); return false; } return true;
}
static bool QueueRun(AnimRequest&& req){
    AnimRequest* heap=nullptr; try{ heap=new AnimRequest(std::move(req)); }catch(...){ return false; }
    { std::lock_guard<std::mutex> lock(g_queueMutex); g_queue.push_back(heap); }
    HWND ha=g_hwndAnim.load(); UINT msg=g_msgAnim.load();
    if(!ha||!msg||!IsWindow(ha)||!PostMessageW(ha,msg,WPARAM(AnimMsg::Drain),0)){
        std::lock_guard<std::mutex> lock(g_queueMutex); if(!g_queue.empty()&&g_queue.back()==heap) g_queue.pop_back();
        if(heap->type!=AnimationType::Close&&heap->type!=AnimationType::Open&&heap->deferOrig&&heap->hwnd&&IsWindow(heap->hwnd)&&ShowWindow_orig) ShowWindow_orig(heap->hwnd,heap->origShowCmd?heap->origShowCmd:SW_RESTORE);
        delete heap; return false;
    } return true;
}
static bool IsAnimateCandidate(HWND hwnd){
    if(!hwnd||!IsWindow(hwnd)) return false;
    if(GetAncestor(hwnd,GA_ROOT)!=hwnd){ LONG s=LONG(GetWindowLongPtrW(hwnd,GWL_STYLE)); if(!(s&WS_CHILD)||!(s&WS_CAPTION)) return false; }
    LONG style=LONG(GetWindowLongPtrW(hwnd,GWL_STYLE)), ex=LONG(GetWindowLongPtrW(hwnd,GWL_EXSTYLE));
    if(!(style&WS_CAPTION)) return false; if(ex&WS_EX_TOOLWINDOW) return false; if(ex&WS_EX_NOACTIVATE) return false;
    if(hwnd==g_hwndAnim.load()) return false; return true;
}
static bool IsTopLevelCloseCandidate(HWND hwnd){
    if(!IsAnimateCandidate(hwnd)) return false;
    LONG style=LONG(GetWindowLongPtrW(hwnd,GWL_STYLE));
    if(style&WS_CHILD) return false;
    if(!(style&WS_SYSMENU)) return false;
    if(!(style&WS_MINIMIZEBOX)) return false;
    if(GetWindow(hwnd,GW_OWNER)) return false;
    if(GetParent(hwnd)) return false;
    if(GetAncestor(hwnd,GA_ROOT)!=hwnd) return false;
    DWORD tid=GetWindowThreadProcessId(hwnd,nullptr);
    if(tid!=GetCurrentThreadId()) return false;
    return true;
}
// Diagnostica: stessa logica di IsTopLevelCloseCandidate ma con un log per ogni
// singola condizione che fallisce. Usata SOLO dal percorso open (OpenAnimationAllowed),
// mai da quello close, per non aggiungere rumore al log del minimize/close che gia' funziona.
static bool IsTopLevelCloseCandidateVerbose(HWND hwnd){
    if(!hwnd||!IsWindow(hwnd)){ OpenLogWhy(L"idoneita': hwnd nullo/non valido"); return false; }
    LONG style=LONG(GetWindowLongPtrW(hwnd,GWL_STYLE)), ex=LONG(GetWindowLongPtrW(hwnd,GWL_EXSTYLE));
    if(GetAncestor(hwnd,GA_ROOT)!=hwnd){ if(!(style&WS_CHILD)||!(style&WS_CAPTION)){ OpenLogWhy(L"idoneita': non root e non child+caption"); return false; } }
    if(!(style&WS_CAPTION)){ OpenLogWhy(L"idoneita': manca WS_CAPTION"); return false; }
    if(ex&WS_EX_TOOLWINDOW){ OpenLogWhy(L"idoneita': WS_EX_TOOLWINDOW"); return false; }
    if(ex&WS_EX_NOACTIVATE){ OpenLogWhy(L"idoneita': WS_EX_NOACTIVATE"); return false; }
    if(hwnd==g_hwndAnim.load()){ OpenLogWhy(L"idoneita': e' la finestra overlay stessa"); return false; }
    if(style&WS_CHILD){ OpenLogWhy(L"idoneita': WS_CHILD"); return false; }
    if(!(style&WS_SYSMENU)){ OpenLogWhy(L"idoneita': manca WS_SYSMENU"); return false; }
    if(!(style&WS_MINIMIZEBOX)){ OpenLogWhy(L"idoneita': manca WS_MINIMIZEBOX"); return false; }
    if(GetWindow(hwnd,GW_OWNER)){ OpenLogWhy(L"idoneita': ha un owner"); return false; }
    if(GetParent(hwnd)){ OpenLogWhy(L"idoneita': ha un parent"); return false; }
    if(GetAncestor(hwnd,GA_ROOT)!=hwnd){ OpenLogWhy(L"idoneita': non e' la finestra radice"); return false; }
    DWORD tid=GetWindowThreadProcessId(hwnd,nullptr);
    if(tid!=GetCurrentThreadId()){ OpenLogWhy(L"idoneita': thread diverso da quello chiamante"); return false; }
    return true;
}
static bool ShouldAnimateWindow(HWND hwnd){
    if(g_fDisabled.load()||g_fAnimating.load()) return false; if(!IsAnimateCandidate(hwnd)) return false;
    if(!IsWindowVisible(hwnd)||IsIconic(hwnd)) return false; return true;
}
static bool ShouldAnimateClose(HWND hwnd){
    if(g_fDisabled.load()||g_fAnimating.load()) return false; if(!IsTopLevelCloseCandidate(hwnd)) return false;
    if(!IsWindowVisible(hwnd)||IsIconic(hwnd)) return false; return true;
}
// Riservazione slot + messa in coda. needsCapture=true (solo Open) significa che la
// bitmap viene prodotta dall'overlay thread prima di RunAnimation, quindi qui non si
// puo' controllare che req.capture sia gia' piena e non si puo' presentare il frame 0
// dal thread chiamante (che e' il thread UI dell'app: fermarlo li impedirebbe il paint).
static bool StartQueuedAnimation(HWND hwnd,AnimationType type,const RECT& rcWin,const RECT& rcDest,CaptureBits&& cap,bool defer,int cmd,bool needsCapture){
    auto cleanup=[hwnd](){ if(g_hwndCurrent.load()==hwnd){ g_hwndCurrent.store(nullptr); g_typeCurrent.store(0);} g_fAnimating.store(false); };
    if(!hwnd||!IsWindow(hwnd)||(!needsCapture&&cap.empty())||!WaitForAnimWndThread()){cleanup(); return false;} if(g_stopping.load()){cleanup(); return false;}
    AnimRequest req; req.hwnd=hwnd; req.type=type; req.rcWindow=rcWin; req.rcDest=rcDest; req.capture=std::move(cap); req.durationMs=DurationMsFor(type); req.deferOrig=defer; req.origShowCmd=cmd; req.needsCapture=needsCapture;
    try{ req.gdi=std::make_unique<PresentGdi>(); }catch(...){cleanup(); return false;}
    g_hwndCurrent.store(hwnd); g_typeCurrent.store(int(type));
    if(!defer&&!needsCapture) SendMessageW(g_hwndAnim.load(),g_msgAnim.load(),WPARAM(AnimMsg::FirstFrame),LPARAM(&req));
    DisableTransitions(hwnd,TRUE);
    if(!QueueRun(std::move(req))){ DisableTransitions(hwnd,FALSE); HWND ha=g_hwndAnim.load(); if(ha&&IsWindow(ha)) HideOverlayWindow(ha); cleanup(); return false; } return true;
}
static bool BeginAnimation(HWND hwnd,AnimationType type,const RECT& rcWin,const RECT& rcDest,CaptureBits&& cap,bool defer,int cmd){
    bool exp=false; if(!g_fAnimating.compare_exchange_strong(exp,true)) return false;
    return StartQueuedAnimation(hwnd,type,rcWin,rcDest,std::move(cap),defer,cmd,false);
}
static void StopAnimThread(){
    std::lock_guard<std::mutex> lock(g_animThreadMutex); if(!g_hAnimWndThread) return;
    HWND ha=g_hwndAnim.load(); if(ha&&IsWindow(ha)) SendMessageW(ha,WM_CLOSE,0,0);
    if(g_dwAnimThreadId) PostThreadMessageW(g_dwAnimThreadId,WM_QUIT,0,0); WaitForSingleObject(g_hAnimWndThread,INFINITE); CloseHandle(g_hAnimWndThread); g_hAnimWndThread=nullptr; g_dwAnimThreadId=0;
}
static void AfterOrigMinimize(HWND hwnd,bool async){
    if(!hwnd||!IsWindow(hwnd)) return; if(!g_fAnimating.load()||g_hwndCurrent.load()!=hwnd) return; if(g_typeCurrent.load()!=int(AnimationType::Minimize)) return;
    DisableTransitions(hwnd,TRUE);
    if(!async&&!IsIconic(hwnd)){ g_hwndCurrent.store(nullptr); g_typeCurrent.store(0); g_fAnimating.store(false); HWND ha=g_hwndAnim.load(); if(ha&&IsWindow(ha)) HideOverlayWindow(ha); }
}
static bool PlayMinimize(HWND hwnd){
    if(!g_animateMinimize||!ShouldAnimateWindow(hwnd)) return false; LONG s=LONG(GetWindowLongPtrW(hwnd,GWL_STYLE)); if(s&WS_MINIMIZE) return false;
    RECT rcWin{},rcMin{}; CaptureBits cap; { ScopedDpiAware dpi; if(!GetVisibleWindowRectForMinimize(hwnd,&rcWin)) return false; if(!GetMinimizeRectPhysical(hwnd,&rcMin)) return false; rcMin=AspectCorrectedMinimizeTarget(rcMin); if(!CaptureWindowForClose(hwnd,cap)) return false; }
    CacheCapture(hwnd,cap); return BeginAnimation(hwnd,AnimationType::Minimize,rcWin,rcMin,std::move(cap),false,0);
}
static bool PlayRestore(HWND hwnd){
    if(!g_animateMinimize) return false; LONG s=LONG(GetWindowLongPtrW(hwnd,GWL_STYLE)); if(s&WS_MINIMIZE){
        if(g_fDisabled.load()||g_fAnimating.load()) return false; if(!HasCachedCapture(hwnd)) return false;
        RECT rcMin{},rcRest{}; bool have=false; { ScopedDpiAware dpi; if(!GetMinimizeRectPhysical(hwnd,&rcMin)) return false; rcMin=AspectCorrectedMinimizeTarget(rcMin); WINDOWPLACEMENT wp{sizeof(wp)}; if(GetWindowPlacement(hwnd,&wp)&&(wp.flags&WPF_RESTORETOMAXIMIZED)) have=GetMaximizeRectPhysical(hwnd,&rcRest); else have=GetRestoreRectPhysical(hwnd,&rcRest); }
        if(!have||!IsRectUsable(rcRest)) return false; CaptureBits cap; if(!TakeCachedCapture(hwnd,RECTW(rcRest),RECTH(rcRest),cap)) return false;
        if(cap.width>0&&cap.height>0&&(RECTW(rcRest)!=cap.width||RECTH(rcRest)!=cap.height)){ rcRest.right=rcRest.left+cap.width; rcRest.bottom=rcRest.top+cap.height; }
        return BeginAnimation(hwnd,AnimationType::RestoreFromMinimized,rcRest,rcMin,std::move(cap),true,SW_RESTORE);
    } return false;
}

// CLOSE autentico – screenshot preciso (ha senso perché finestra è aperta)
static bool PlayClose_Authentic(HWND hwnd){
    if(!WaitForAnimWndThread()||g_stopping.load()) return false;
    RECT rcWin{}; CaptureBits cap;
    { ScopedDpiAware dpi; if(!GetWindowRectPhysical(hwnd,&rcWin)||!CaptureWindowForClose(hwnd,cap)) return false; }
    AnimRequest req; req.hwnd=hwnd; req.type=AnimationType::Close; req.rcWindow=rcWin; req.rcDest=rcWin; req.capture=std::move(cap); req.durationMs=DurationMsFor(AnimationType::Close);
    try{ req.gdi=std::make_unique<PresentGdi>(); }catch(...){ return false; }
    HWND ha=g_hwndAnim.load(); if(!ha||!IsWindow(ha)) return false;
    ScopedDwmTransitions transOverlay(ha); transOverlay.Disable();
    ScopedDwmTransitions transWnd(hwnd); transWnd.Disable();
    {
        ScopedDpiAware dpi;
        ShowOverlayWindow(ha);
        PresentOverlay(ha,*req.gdi,req,rcWin,ParamsFor(req.type,0.f,float(RECTH(rcWin))));
        if(ShowWindow_orig) ShowWindow_orig(hwnd,SW_HIDE); else ::ShowWindow(hwnd,SW_HIDE);
        ULONGLONG start=GetTickCount64(), elapsed=0; float lastT=-1;
        while((elapsed=GetTickCount64()-start)<=req.durationMs){
            float t=req.durationMs==0?1.f:std::clamp(float(elapsed)/float(req.durationMs),0.f,1.f);
            if(t-lastT>=0.001f||elapsed+8>=req.durationMs){
                lastT=t; auto params=ParamsFor(req.type,t,float(RECTH(rcWin)));
                PresentOverlay(ha,*req.gdi,req,rcWin,params);
            }
            ULONGLONG fe=GetTickCount64(); ULONGLONG tt=start+ULONGLONG(t*req.durationMs)+16;
            if(fe<tt) Sleep(DWORD(tt-fe)); else Sleep(1);
        }
        Win7TransformParams pe=ParamsFor(req.type,1.f,float(RECTH(rcWin))); pe.opacity=0.f;
        PresentOverlay(ha,*req.gdi,req,rcWin,pe); Sleep(8); HideOverlayWindow(ha);
    }
    transOverlay.Restore();
    transWnd.Dismiss(); // non ripristinare su hwnd che sta per morire
    if(IsWindow(hwnd)&&DestroyWindow_orig) DestroyWindow_orig(hwnd);
    return true;
}
// ---------------------------------------------------------------------------
// APERTURA. Flusso (nessuna architettura parallela: overlay, coda e rasterizzatore
// sono gli stessi gia' usati per minimize/restore).
// Nota sul perche' del flusso asincrono: la vecchia versione provava a catturare e
// animare DENTRO ShowWindow, sul thread UI dell'applicazione. In quel punto la
// finestra e' stata mostrata ma non ancora pitturata (l'applicazione deve ancora
// pompare i messaggi), quindi il client era vuoto e il bordo Aero - che esiste solo
// nella passata di composizione del DWM, mai nel buffer della finestra - non c'era:
// da qui il "bordo che compare alla fine".
//
//   ShowWindow_hook            ArmOpen(): tendina alpha 0 + transizioni DWM off
//            |                  (PRIMA che il DWM componga la finestra: zero pop)
//   ShowWindow_orig()          la finestra viene mostrata e dipinta dall'app
//            |
//   PlayOpen()                 riserva lo slot animazione e accoda la richiesta
//            v
//   overlay thread             ProbeComposedWindow(): attende la composizione,
//                              DwmFlush, legge DWMWA_EXTENDED_FRAME_BOUNDS dallo
//                              schermo, ricopre la finestra
//            v                  PresentFirstFrame() (t=0) -> RunAnimation()
//   FinishQueued()             HandoffOpen(): alpha 255 sotto l'overlay, DwmFlush,
//                              poi HideOverlayWindow + transizioni ripristinate
//
// Se qualcosa non funziona (paint mai arrivato, finestra coperta da un'altra,
// capture non valida, coda piena, thread overlay morto) si toglie la tendina e
// Windows mostra la finestra normalmente: nessuna animazione forzata.
// ---------------------------------------------------------------------------
static bool OpenAnimationAllowed(HWND hwnd){
    if(!g_animateOpen) return false; // disattivato dall'utente: nessun log, e' voluto
    if(g_fDisabled.load()){ OpenLogWhy(L"annullata: mod disabilitata"); return false; }
    if(g_fAnimating.load()){ OpenLogWhy(L"annullata: un'altra animazione e' gia' in corso"); return false; }
    if(g_stopping.load()) return false;
    if(IsSnippingTool()) return false;               // si fotografa da solo: niente overlay sopra
    if(!IsTopLevelCloseCandidateVerbose(hwnd)) return false; // logga gia' il motivo esatto
    // Nessuna esclusione per WS_EX_NOREDIRECTIONBITMAP: la cattura legge il desktop
    // COMPOSTO, non la superficie di reindirizzamento della finestra, quindi anche il
    // contenuto di una XAML island / finestra DirectComposition e' leggibile; se poi
    // non lo fosse, la validazione sotto annulla l'animazione in modo sicuro.
    if(IsWindowVisible(hwnd)){ OpenLogWhy(L"annullata: la finestra e' gia' visibile al momento dell'arm"); return false; }
    if(IsIconic(hwnd)){ OpenLogWhy(L"annullata: la finestra e' minimizzata al momento dell'arm"); return false; }
    if(CurtainIsApplied(hwnd)){ OpenLogWhy(L"annullata: tendina gia' applicata su questa finestra"); return false; }
    if(HasCachedCapture(hwnd)){ OpenLogWhy(L"annullata: minimize gia' in corso su questa finestra"); return false; }
    return true;
}
// Da chiamare PRIMA di ShowWindow_orig: finche' la finestra non e' mai stata
// composta la si rende invisibile con la tendina. Nessun cambio di visibilita',
// di stile di finestra, di Z order o di focus.
static bool ArmOpen(HWND hwnd){
    if(!OpenAnimationAllowed(hwnd)) return false;
    HWND expected=nullptr;
    if(!g_hwndOpenArmed.compare_exchange_strong(expected,hwnd)) return false;  // una sola apertura alla volta
    auto disarm=[hwnd](){ HWND cur=hwnd; g_hwndOpenArmed.compare_exchange_strong(cur,HWND(nullptr)); };
    auto giveUp=[&](LPCWSTR why){ disarm(); g_fAnimating.store(false); OpenLogWhy(L"%s",why); return false; };
    // NB: qui NON si aspetta l'apertura del thread dell'overlay: ArmOpen viene
    // chiamata subito prima di ShowWindow_orig e deve costare il meno possibile.
    // Il thread viene recuperato da StartQueuedAnimation, dopo il show.
    if(g_stopping.load()) return giveUp(L"annullata: mod in chiusura");
    bool exp=false; if(!g_fAnimating.compare_exchange_strong(exp,true)) return giveUp(L"annullata: animazione gia' in corso");
    bool ok=false;
    { ScopedDpiAware dpi; RECT rc{};
      // Lo sfondo va letto prima di ShowWindow_orig: e l'unico momento in cui la
      // regione contiene soltanto quello che c'era dietro la finestra.
      if(GetFrameBoundsPhysical(hwnd,&rc)&&IsRectUsable(rc)){ OpenBackgroundCapture(hwnd,rc); ok=CurtainApply(hwnd); }
    }
    if(!ok){ OpenBackgroundClear(hwnd); return giveUp(L"annullata: tendina non applicabile"); }
    DisableTransitions(hwnd,TRUE);   // il DWM non deve aggiungere il proprio fade sotto al nostro
    // Dead-man switch: un'arm rimasta orfana (ShowWindow mai tornata, thread overlay
    // morto) lascerebbe la finestra invisibile per sempre. Il timer vive sulla NOSTRA
    // finestra: nell'applicazione non resta nessun callback pendente che punti su
    // codice della mod eventualmente gia' scaricato.
    g_ulOpenArmedTick.store(GetTickCount64());
    if(HWND ha=g_hwndAnim.load()){ if(IsWindow(ha)) SetTimer(ha,kOpenWatchdogTimer,kOpenWatchdogMs,nullptr); }
    return true;
}
// Da chiamare DOPO ShowWindow_orig. Se la finestra non e' effettivamente diventata
// visibile (l'app l'ha vetoata, o ShowWindow l'ha minimizzata) si toglie la tendina.
static bool PlayOpen(HWND hwnd){
    HWND expected=hwnd;
    // consuma l'arm messo da ArmOpen(): se non eravamo noi l'ultimo armato, non tocchiamo niente
    if(!g_hwndOpenArmed.compare_exchange_strong(expected,HWND(nullptr))) return false;
    auto undo=[hwnd](){ OpenCancelCleanup(hwnd);
        if(g_hwndCurrent.load()==hwnd){ g_hwndCurrent.store(nullptr); g_typeCurrent.store(0); }
        g_fAnimating.store(false); };
    if(g_stopping.load()||!IsWindow(hwnd)||!IsWindowVisible(hwnd)||IsIconic(hwnd)){ undo(); return false; }
    RECT rc{}; { ScopedDpiAware dpi; if(!GetFrameBoundsPhysical(hwnd,&rc)||!IsRectUsable(rc)){ undo(); OpenLogWhy(L"annullata: bound non usable"); return false; } }
    if(!StartQueuedAnimation(hwnd,AnimationType::Open,rc,rc,CaptureBits{},false,0,true)){ undo(); OpenLogWhy(L"annullata: coda animazione rifiutata"); return false; }
    return true;
}
static bool PlayClose(HWND hwnd){
    if(!g_animateClose) return false;
    if(IsSnippingTool()) return false; // FIX: evita repeat animazione SnippingTool durante screenshot
    if(!ShouldAnimateClose(hwnd)) return false;
    bool exp=false; if(!g_fAnimating.compare_exchange_strong(exp,true)) return false;
    auto release=[&](){ g_fAnimating.store(false); g_hwndCurrent.store(nullptr); g_typeCurrent.store(0); };
    g_hwndCurrent.store(hwnd); g_typeCurrent.store(int(AnimationType::Close));
    bool ok=PlayClose_Authentic(hwnd); release(); return ok;
}

// ---------------------------------------------------------------------------
// Finestre create gia' con WS_VISIBLE.
// Su build recenti di Windows 11 molte app (anche Win32 pure: Explorer, MSPaint,
// Notepad...) impostano WS_VISIBLE direttamente in CreateWindowEx invece di
// chiamare ShowWindow dopo. In quel caso ShowWindow_hook non vede mai nulla: la
// finestra e' gia' visibile alla prima occasione utile per armare la tendina.
// Soluzione: togliere WS_VISIBLE PRIMA della creazione, cosi' la finestra nasce
// invisibile come nel percorso classico, poi la si arma e la si mostra noi stessi
// subito dopo con la stessa identica logica di ShowWindow_hook (ArmOpen+PlayOpen).
// Se per qualunque motivo l'animazione non parte, la finestra viene comunque
// mostrata: non deve MAI restare invisibile per un nostro errore.
// ---------------------------------------------------------------------------
typedef HWND(WINAPI* CreateWindowExW_t)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID);
typedef HWND(WINAPI* CreateWindowExA_t)(DWORD,LPCSTR,LPCSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID);
static CreateWindowExW_t CreateWindowExW_orig=nullptr;
static CreateWindowExA_t CreateWindowExA_orig=nullptr;
// Verifica solo sullo stile richiesto (l'HWND non esiste ancora): le verifiche
// complete (owner, thread, ex-style, ecc.) le rifa' OpenAnimationAllowed/ArmOpen
// sull'HWND vero, appena creato.
static bool CreateStyleIsOpenCandidate(DWORD dwStyle, HWND hWndParent){
    if(!(dwStyle&WS_VISIBLE)) return false;
    if(dwStyle&WS_CHILD) return false;
    if(hWndParent) return false;
    if(!(dwStyle&WS_CAPTION)) return false;
    if(!(dwStyle&WS_SYSMENU)) return false;
    if(!(dwStyle&WS_MINIMIZEBOX)) return false;
    return true;
}
// Diagnostica temporanea: per ogni finestra top-level (nessun parent) creata gia'
// visibile, stampa classe e stile esatti COSI' COME arrivano a CreateWindowEx.
// Serve a capire empiricamente quali bit ha davvero la finestra in questo momento,
// invece di presumere che WS_SYSMENU/WS_MINIMIZEBOX siano gia' presenti qui.
static void LogCreateCandidate(LPCWSTR cls, DWORD dwStyle, DWORD dwExStyle, HWND hWndParent){
    if(!(dwStyle&WS_VISIBLE)||(dwStyle&WS_CHILD)||hWndParent) return;
    OpenLogWhy(L"create: classe='%s' style=0x%08X exstyle=0x%08X (CAPTION=%d SYSMENU=%d MINBOX=%d)",
        cls?cls:L"?", dwStyle, dwExStyle,
        (dwStyle&WS_CAPTION)!=0, (dwStyle&WS_SYSMENU)!=0, (dwStyle&WS_MINIMIZEBOX)!=0);
}
static void FinishDeferredCreate(HWND hwnd){
    if(!hwnd||!IsWindow(hwnd)) return;
    bool armed=ArmOpen(hwnd);
    if(ShowWindow_orig) ShowWindow_orig(hwnd,SW_SHOW); else ::ShowWindow(hwnd,SW_SHOW);
    if(armed) PlayOpen(hwnd);
}
HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam){
    LogCreateCandidate(lpClassName,dwStyle,dwExStyle,hWndParent);
    bool tryDefer=!g_fDisabled.load()&&!g_fAnimating.load()&&g_animateOpen&&!g_stopping.load()&&CreateStyleIsOpenCandidate(dwStyle,hWndParent);
    HWND h=CreateWindowExW_orig(dwExStyle,lpClassName,lpWindowName,tryDefer?(dwStyle&~DWORD(WS_VISIBLE)):dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
    if(tryDefer&&h) FinishDeferredCreate(h);
    return h;
}
HWND WINAPI CreateWindowExA_hook(DWORD dwExStyle,LPCSTR lpClassName,LPCSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam){
    if((dwStyle&WS_VISIBLE)&&!(dwStyle&WS_CHILD)&&!hWndParent){
        wchar_t wcls[128]=L"?";
        if(lpClassName&&!IS_INTRESOURCE(lpClassName)) MultiByteToWideChar(CP_ACP,0,lpClassName,-1,wcls,_countof(wcls));
        LogCreateCandidate(wcls,dwStyle,dwExStyle,hWndParent);
    }
    bool tryDefer=!g_fDisabled.load()&&!g_fAnimating.load()&&g_animateOpen&&!g_stopping.load()&&CreateStyleIsOpenCandidate(dwStyle,hWndParent);
    HWND h=CreateWindowExA_orig(dwExStyle,lpClassName,lpWindowName,tryDefer?(dwStyle&~DWORD(WS_VISIBLE)):dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
    if(tryDefer&&h) FinishDeferredCreate(h);
    return h;
}

// Shell hook HSHELL_GETMINRECT
static std::mutex g_minRectMutex; static std::unordered_map<HWND, RECT> g_pendingMinRect;
struct REALRECT{ SHORT left,top,right,bottom; }; struct SHELLHOOK_MINRECT{ HWND hWnd; REALRECT rc; };
static HWND g_hShellHookWnd=nullptr; static HINSTANCE g_hShellHookInst=nullptr; static DWORD g_dwShellHookThreadId=0; static HANDLE g_hShellHookThread=nullptr; static std::atomic<bool> g_shellHookStop{false};
static void SetPendingMinRect(HWND hwnd, const RECT& rc){ std::lock_guard<std::mutex> lock(g_minRectMutex); g_pendingMinRect[hwnd]=rc; }
static void ClearPendingMinRect(HWND hwnd){ std::lock_guard<std::mutex> lock(g_minRectMutex); g_pendingMinRect.erase(hwnd); }
static LRESULT CALLBACK ShellHookWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    if(msg==RegisterWindowMessageW(L"SHELLHOOK")){
        if(wParam==20){ SHELLHOOK_MINRECT* p=(SHELLHOOK_MINRECT*)lParam; if(p&&p->hWnd){ std::lock_guard<std::mutex> lock(g_minRectMutex); auto it=g_pendingMinRect.find(p->hWnd); if(it!=g_pendingMinRect.end()){ p->rc.left=(SHORT)it->second.left; p->rc.top=(SHORT)it->second.top; p->rc.right=(SHORT)it->second.right; p->rc.bottom=(SHORT)it->second.bottom; return TRUE; } } }
    } return DefWindowProcW(hwnd,msg,wParam,lParam);
}
static DWORD CALLBACK ShellHookThreadProc(LPVOID){
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const wchar_t cls[]=L"Windhawk_Win7_ShellHook"; WNDCLASSW wc{}; wc.lpfnWndProc=ShellHookWndProc; wc.hInstance=g_hShellHookInst; wc.lpszClassName=cls; RegisterClassW(&wc);
    HWND w=CreateWindowExW(0,cls,nullptr,0,0,0,0,0,nullptr,nullptr,g_hShellHookInst,nullptr);
    if(w){ g_hShellHookWnd=w; RegisterShellHookWindow(w); MSG msg; while(!g_shellHookStop.load()&&GetMessageW(&msg,nullptr,0,0)){ TranslateMessage(&msg); DispatchMessageW(&msg);} DeregisterShellHookWindow(w); DestroyWindow(w); }
    UnregisterClassW(cls,g_hShellHookInst); return 0;
}

#define DWP_HOOK_(name,defArgs,callArgs) LRESULT(CALLBACK* name##_orig) defArgs; LRESULT CALLBACK name##_hook defArgs { if(uMsg==WM_SYSCOMMAND){ UINT cmd=UINT(wParam)&0xFFF0; if(cmd==SC_MINIMIZE&&PlayMinimize(hWnd)){ LRESULT lr=name##_orig callArgs; AfterOrigMinimize(hWnd,false); return lr; } if(cmd==SC_RESTORE&&PlayRestore(hWnd)) return 0; if(cmd==SC_CLOSE){ if(ShouldAnimateClose(hWnd)&&PlayClose(hWnd)) return 0; } } return name##_orig callArgs; }
#define DWP_HOOK(name,defArgs,callArgs) DWP_HOOK_(name##A,defArgs,callArgs) DWP_HOOK_(name##W,defArgs,callArgs)
DWP_HOOK(DefWindowProc,(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam),(hWnd,uMsg,wParam,lParam))
DWP_HOOK(DefFrameProc,(HWND hWnd,HWND hWndMDIClient,UINT uMsg,WPARAM wParam,LPARAM lParam),(hWnd,hWndMDIClient,uMsg,wParam,lParam))
DWP_HOOK(DefMDIChildProc,(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam),(hWnd,uMsg,wParam,lParam))
DWP_HOOK(DefDlgProc,(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam),(hWnd,uMsg,wParam,lParam))

static UINT CmdFromShow(int c){
    switch(c){ case SW_MINIMIZE: case SW_SHOWMINIMIZED: case SW_FORCEMINIMIZE: return SC_MINIMIZE; case SW_RESTORE: return SC_RESTORE; case SW_SHOWNORMAL: case SW_SHOW: case SW_SHOWDEFAULT: case SW_SHOWNA: return 100; default: return 0; }
}
BOOL WINAPI ShowWindow_hook(HWND hWnd,int nCmdShow){
    if(g_fDisabled.load()||g_fAnimating.load()) return ShowWindow_orig(hWnd,nCmdShow);
    UINT cmd=CmdFromShow(nCmdShow);
    if(cmd==SC_RESTORE&&PlayRestore(hWnd)) return TRUE;
    bool isOpenCmd = (cmd==100); bool wasVisible = IsWindowVisible(hWnd); bool playedMin=false; bool armedOpen=false;
    if(cmd==SC_MINIMIZE){ RECT rcMin{}; if(GetMinimizeRectPhysical(hWnd,&rcMin)){ SetPendingMinRect(hWnd, AspectCorrectedMinimizeTarget(rcMin)); } playedMin=PlayMinimize(hWnd); }
    // OPEN: la tendina va messa prima che ShowWindow componga nulla, cosi' il frame
    // reale viene visto dallo schermo solo il tempo di leggerlo. ShowWindow_orig
    // NON viene ritardato e non viene bloccato: l'hook resta in piedi solo per le
    // due chiamate cheap che servono a coprire la finestra.
    if(isOpenCmd && !wasVisible && !playedMin) armedOpen = ArmOpen(hWnd);
    else if(isOpenCmd && wasVisible) OpenLogWhy(L"annullata: la finestra era gia' visibile prima di ShowWindow (WS_VISIBLE diretta?)");
    BOOL r=ShowWindow_orig(hWnd,nCmdShow);
    if(playedMin){ AfterOrigMinimize(hWnd,false); ClearPendingMinRect(hWnd); }
    if(armedOpen) PlayOpen(hWnd);
    UNREFERENCED_PARAMETER(wasVisible);
    return r;
}
BOOL WINAPI ShowWindowAsync_hook(HWND hWnd,int nCmdShow){
    if(g_fDisabled.load()||g_fAnimating.load()) return ShowWindowAsync_orig(hWnd,nCmdShow);
    UINT cmd=CmdFromShow(nCmdShow);
    if(cmd==SC_RESTORE&&PlayRestore(hWnd)) return TRUE;
    // Da MSDN: se la finestra appartiene al thread chiamante, ShowWindowAsync si
    // comporta esattamente come ShowWindow (sincrono); l'asincronia vera scatta
    // solo cross-thread (es. Explorer che mostra finestre create su altri thread
    // del proprio processo). Solo nel caso same-thread possiamo animare l'apertura
    // in sicurezza: il resto della logica ricalca ShowWindow_hook.
    bool sameThread = GetWindowThreadProcessId(hWnd,nullptr)==GetCurrentThreadId();
    bool isOpenCmd = (cmd==100); bool wasVisible = IsWindowVisible(hWnd); bool playedMin=false; bool armedOpen=false;
    if(cmd==SC_MINIMIZE){ RECT rcMin{}; if(GetMinimizeRectPhysical(hWnd,&rcMin)){ SetPendingMinRect(hWnd, AspectCorrectedMinimizeTarget(rcMin)); } playedMin=PlayMinimize(hWnd); }
    if(sameThread && isOpenCmd && !wasVisible && !playedMin) armedOpen = ArmOpen(hWnd);
    else if(isOpenCmd && !sameThread) OpenLogWhy(L"annullata: ShowWindowAsync cross-thread, apertura non animabile in sicurezza");
    else if(isOpenCmd && wasVisible) OpenLogWhy(L"annullata: la finestra era gia' visibile prima di ShowWindowAsync");
    BOOL r=ShowWindowAsync_orig(hWnd,nCmdShow);
    if(playedMin){ AfterOrigMinimize(hWnd,true); ClearPendingMinRect(hWnd); }
    if(armedOpen) PlayOpen(hWnd);
    // Cross-thread: nessuna tendina viene armata (isOpenCmd non e' stato consumato
    // sopra), quindi qui non resta nulla da disfare: si lascia il comportamento
    // nativo, come prima.
    return r;
}
BOOL WINAPI DestroyWindow_hook(HWND hWnd){
    // Una finestra distrutta durante l'apertura non deve restare "coperta" ne'
    // bloccare lo slot animazione: si scarica tutto prima della chiamata originale.
    if(g_hwndCurrent.load()==hWnd&&g_typeCurrent.load()==int(AnimationType::Open)) g_hwndCurrent.store(nullptr);
    else { HWND armed=hWnd; if(g_hwndOpenArmed.compare_exchange_strong(armed,HWND(nullptr))) g_fAnimating.store(false); }
    OpenBackgroundClear(hWnd);
    CurtainRelease(hWnd);
    ForgetCapture(hWnd); ClearPendingMinRect(hWnd); return DestroyWindow_orig(hWnd);
}

static HMODULE GetCurrentModule(){ HMODULE m=nullptr; GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,LPCWSTR(&GetCurrentModule),&m); return m; }
typedef BOOL(WINAPI* ShowWindowRaw_t)(HWND,int); static ShowWindowRaw_t pShowWindowRaw=nullptr;

// Pulizia straordinaria ispirata a win7-network-flyout-recreation
static BOOL CALLBACK ExtraordinaryCleanupEnumProc(HWND hwnd, LPARAM lParam) {
    if(IsWindowVisible(hwnd) || IsIconic(hwnd)){
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, 
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | 
                    SWP_FRAMECHANGED | SWP_NOACTIVATE);
    }
    return TRUE;
}

static void ExtraordinaryCleanup(){
    // Come DWMBlurGlass: forza ricalcolo bordi per tutte le finestre
    EnumWindows(ExtraordinaryCleanupEnumProc, 0);
    
    HWND hDwm = FindWindowW(L"Dwm", nullptr);
    if(hDwm) PostMessageW(hDwm, WM_THEMECHANGED, 0, 0);
}

static void SafeCleanup(){
    if(InterlockedExchange(&g_isUninitializing,1)) return;
    g_fDisabled.store(true); g_stopping.store(true); g_shellHookStop.store(true);
    // Chiudi overlay con loop messaggi come fa network flyout
    HWND ha=g_hwndAnim.load();
    if(ha&&IsWindow(ha)){
        SendMessageW(ha,WM_CLOSE,0,0);
        for(int i=0;i<50&&IsWindow(ha);++i){
            MSG msg; while(PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)){ TranslateMessage(&msg); DispatchMessageW(&msg); }
            Sleep(10);
        }
        if(IsWindow(ha)) DestroyWindow(ha);
    }
    if(g_dwShellHookThreadId) PostThreadMessageW(g_dwShellHookThreadId,WM_QUIT,0,0);
    if(g_hShellHookWnd&&IsWindow(g_hShellHookWnd)) PostMessageW(g_hShellHookWnd,WM_CLOSE,0,0);
    { std::lock_guard<std::mutex> lock(g_queueMutex); while(!g_queue.empty()){ auto* req=g_queue.front(); g_queue.pop_front(); if(!req) continue; if(req->deferOrig&&req->hwnd&&IsWindow(req->hwnd)&&ShowWindowAsync_orig) ShowWindowAsync_orig(req->hwnd,req->origShowCmd?req->origShowCmd:SW_RESTORE); if(req->hwnd&&IsWindow(req->hwnd)) DisableTransitions(req->hwnd,FALSE); if(req->type==AnimationType::Open&&req->hwnd) CurtainRelease(req->hwnd); delete req; } }
    OpenBackgroundClear(nullptr);
    ReleaseAllCurtains();
    HWND armedNow=g_hwndOpenArmed.exchange(nullptr); (void)armedNow;
    HWND cur=g_hwndCurrent.load(); if(cur&&IsWindow(cur)) DisableTransitions(cur,FALSE);
    StopAnimThread();
    if(g_hShellHookThread){ WaitForSingleObject(g_hShellHookThread,1000); CloseHandle(g_hShellHookThread); g_hShellHookThread=nullptr; }
    { std::lock_guard<std::mutex> lock(g_cacheMutex); g_captureCache.clear(); g_captureLru.clear(); }
    ExtraordinaryCleanup();
}

BOOL Wh_ModInit(){
    InitExeName(); g_hinst=GetCurrentModule(); g_hShellHookInst=g_hinst; LoadSettings();
    HMODULE user32=GetModuleHandleW(L"user32.dll"); if(!user32) return FALSE;
    pShowWindowRaw=ShowWindowRaw_t(GetProcAddress(user32,"ShowWindow")); pGetWindowMinimizeRect=GetWindowMinimizeRect_t(GetProcAddress(user32,"GetWindowMinimizeRect"));
    InterlockedExchange(&g_isUninitializing, 0);
    g_shellHookStop.store(false);
    g_hShellHookThread=CreateThread(nullptr,0,ShellHookThreadProc,nullptr,0,&g_dwShellHookThreadId);
#define HOOK(f) if(!Wh_SetFunctionHook((void*)f,(void*)f##_hook,(void**)&f##_orig)){ return FALSE; }
    HOOK(DefWindowProcA) HOOK(DefWindowProcW) HOOK(DefFrameProcA) HOOK(DefFrameProcW) HOOK(DefMDIChildProcA) HOOK(DefMDIChildProcW) HOOK(DefDlgProcA) HOOK(DefDlgProcW) HOOK(ShowWindow) HOOK(ShowWindowAsync) HOOK(DestroyWindow) HOOK(CreateWindowExW) HOOK(CreateWindowExA)
#undef HOOK
    return TRUE;
}
void Wh_ModSettingsChanged(){ LoadSettings(); }
void Wh_ModBeforeUninit(){ SafeCleanup(); }
void Wh_ModUninit(){
    SafeCleanup();
    if(g_hinst){
        UnregisterClassW(kAnimClassName,g_hinst);
        UnregisterClassW(L"Windhawk_Win7_ShellHook",g_hinst);
    }
}
