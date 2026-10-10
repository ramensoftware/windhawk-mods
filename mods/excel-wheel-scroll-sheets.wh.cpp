// ==WindhawkMod==
// @id              excel-wheel-scroll-sheets
// @name            Excel Wheel Sheet Navigation
// @name:zh-CN      Excel 滚轮切换工作表
// @description     Scroll the sheet tab strip with the vertical wheel and switch sheets with the horizontal wheel
// @description:zh-CN 使用原生箭头滚动标签栏，左右拨动一次切换一张工作表
// @version         0.5.4
// @author          youareanimal
// @github          https://github.com/youareanimal
// @homepage        https://github.com/youareanimal/excel-wheel-scroll-sheets
// @license         GPL-3.0-or-later
// @include         EXCEL.EXE
// @architecture    x86-64
// @compilerOptions -loleacc -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// Source code is licensed under GPL-3.0-or-later.

// ==WindhawkModReadme==
/*
# Excel Wheel Sheet Navigation

For 64-bit desktop Excel on Windows:

- Vertical wheel over the bottom sheet tabs scrolls the tab strip without changing the active sheet.
- Horizontal wheel over the worksheet or sheet tabs activates the adjacent visible sheet.
- The tab strip uses Excel's native navigation arrows and animation. If the arrows cannot be used, it falls back to `ScrollWorkbookTabs`.
- Repeated same-direction horizontal wheel messages are grouped with a 200 ms quiet period by default; the first switch is immediate.
- Hidden sheets are skipped. Modified wheel input and ordinary vertical worksheet scrolling keep Excel's behavior.

Settings allow direct sheet activation instead of tab scrolling, reversing direction, disabling horizontal switching, and optional wraparound.
No macros are required. Activating a sheet can trigger existing workbook activation events, just like clicking a tab.
Tested on one installation of 64-bit Microsoft 365 Excel; other Office versions and 32-bit Excel have not been tested.

---

# Excel 滚轮切换工作表

默认操作：标签栏上下滚轮平滑滚动标签；点击标签切换工作表。
单元格区域左右滚轮切换到相邻的可见工作表。

标签栏滚动使用左下角左右箭头的原生操作路径和动画，
不再使用图像覆盖或淡入淡出。滚动不改变当前工作表。
左右拨动产生的连续同方向信号会合并，一次拨动只切换一张。

将鼠标放到 Excel 底部的工作表标签上，根据“滚轮行为”选择：

- 直接切换工作表：向下切换到下一张可见工作表，向上切换到上一张。
  隐藏的工作表会跳过。
- 只滚动标签栏：向下显示右侧标签，向上显示左侧标签；当前工作表保持不变，
  找到目标标签后点击进入。

单元格区域的上下滚轮仍然滚动表格内容。左右滚轮切换可以单独关闭，
关闭后恢复 Excel 原来的横向滚动。水平滚动条、状态栏或功能区保留原有行为。
单元格编辑、查找对话框或其他窗口占用焦点时不进行切换。

只适用于 Windows 桌面版 64 位 Excel。通过 Excel 自身的工作表对象切换，
不需要启用宏，也不会更改工作簿中的数据。切换工作表会触发工作簿自身已有
的工作表激活事件，与手动点击标签一样。

左右拨动防重复默认 200 毫秒：同方向连续信号合并为一次拨动，
停顿后再次切换，反方向立即切换。标签栏直接切换模式的间隔仍为 150 毫秒。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wheelAction: scroll
  $name: Vertical wheel over sheet tabs
  $name:zh-CN: 标签栏上下滚轮行为
  $options:
  - activate: Switch to the adjacent sheet
  - scroll: Scroll the tab strip only
  $options:zh-CN:
  - activate: 直接切换工作表
  - scroll: 只滚动标签栏，点击切换
- smoothAnimation: true
  $name: Use Excel's native tab scrolling
  $name:zh-CN: 使用 Excel 原生标签滚动
  $description: Use Excel's native navigation arrows and animation. When disabled, scroll the tab strip directly.
  $description:zh-CN: 使用左下角左右箭头相同的滚动方式，动画速度由 Excel 决定。关闭后直接按标签滚动。
- horizontalGestureMs: 200
  $name: Horizontal gesture quiet period (milliseconds)
  $name:zh-CN: 左右拨动防重复（毫秒）
  $description: Group repeated same-direction input until this quiet period elapses. Reversal switches immediately. Valid range: 150–2000 ms (values outside are clamped); default 200.
  $description:zh-CN: 同方向连续信号视为一次拨动，停顿达到此时长后可再次切换；反向拨动立即切换。有效范围 150–2000 毫秒（超出会被限制）；默认 200。
- horizontalSwitching: true
  $name: Switch sheets with the horizontal wheel
  $name:zh-CN: 左右滚轮切换工作表
  $description: Over the worksheet or sheet tabs, scroll right for the next visible sheet and left for the previous one. Disable to retain horizontal worksheet scrolling.
  $description:zh-CN: 鼠标在单元格或底部工作表标签上时，向右切换下一张，向左切换上一张。关闭后恢复原有横向滚动。
- reverseDirection: false
  $name: Reverse wheel direction
  $name:zh-CN: 反转滚轮方向
- throttleMs: 150
  $name: Vertical sheet switching interval (milliseconds)
  $name:zh-CN: 切换间隔（毫秒）
  $description: Minimum interval for direct sheet activation using the vertical wheel over the tabs. Set to 0 to disable. Does not delay tab-strip scrolling.
  $description:zh-CN: 仅用于标签栏上下滚轮直接切换工作表，防止快速滚动连续跳过多张；设为 0 取消限制。不影响标签栏滚动。
- wrapAround: false
  $name: Wrap around at the first or last sheet
  $name:zh-CN: 到达首尾时循环切换
  $description: Applies to horizontal wheel switching and vertical direct-switching mode. Continue from the last visible sheet to the first, or vice versa.
  $description:zh-CN: 用于左右滚轮切换及上下滚轮直接切换模式。开启后，最后一张的下一张为第一张，反之亦然。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <oleacc.h>
#include <oleauto.h>
#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <windhawk_utils.h>

std::atomic<bool> g_reverse{false}, g_wrap{false};
std::atomic<bool> g_scrollTabs{false};
std::atomic<bool> g_horizontal{true};
std::atomic<unsigned> g_throttle{150};
std::atomic<bool> g_smooth{true};
std::atomic<unsigned> g_horizontalGestureMs{200};
// Debug output is handled by Windhawk logging; no synchronous mod-storage writes.
void LogDiagnostic(PCWSTR name, int value) {
    Wh_Log(L"%ls=%d",name,value);
}
thread_local bool g_insideHook = false;
thread_local HWND g_lastRoot = nullptr;
thread_local UINT g_lastAxis = 0;
thread_local int g_deltaRemainder = 0;
thread_local ULONGLONG g_lastWheel = 0, g_lastSwitch = 0;

struct Variant {
    VARIANT value;
    Variant() { VariantInit(&value); }
    ~Variant() { VariantClear(&value); }
    Variant(const Variant&) = delete;
};
template<class T> struct ComPtr {
    T* p = nullptr;
    ComPtr() = default;
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ~ComPtr() { if (p) p->Release(); }
};

HRESULT InvokeMember(IDispatch* object, const wchar_t* name, WORD flags,
                     VARIANT* result, VARIANT* argument = nullptr) {
    if (!object) return E_POINTER;
    DISPID id;
    LPOLESTR mutableName = const_cast<LPOLESTR>(name);
    HRESULT hr = object->GetIDsOfNames(IID_NULL, &mutableName, 1,
                                     LOCALE_USER_DEFAULT, &id);
    if (FAILED(hr)) return hr;
    DISPPARAMS params{};
    params.rgvarg = argument;
    params.cArgs = argument ? 1 : 0;
    EXCEPINFO exception{};
    hr = object->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, flags, &params,
                        result, &exception, nullptr);
    if (exception.pfnDeferredFillIn) exception.pfnDeferredFillIn(&exception);
    SysFreeString(exception.bstrSource);
    SysFreeString(exception.bstrDescription);
    SysFreeString(exception.bstrHelpFile);
    return hr;
}

bool GetDispatchProperty(IDispatch* parent, const wchar_t* name, ComPtr<IDispatch>& result,
               VARIANT* argument = nullptr) {
    Variant value;
    if (FAILED(InvokeMember(parent, name, DISPATCH_PROPERTYGET,
                            &value.value, argument))) return false;
    if (value.value.vt != VT_DISPATCH || !value.value.pdispVal) return false;
    result.p = value.value.pdispVal;
    result.p->AddRef();
    return true;
}

bool GetNumber(IDispatch* parent, const wchar_t* name, LONG& result) {
    Variant value, converted;
    if (FAILED(InvokeMember(parent, name, DISPATCH_PROPERTYGET,
                            &value.value))) return false;
    if (FAILED(VariantChangeType(&converted.value, &value.value, 0, VT_I4)))
        return false;
    result = converted.value.lVal;
    return true;
}

// Only the workbook's native window in this Excel process is eligible.
HWND GetWorksheetWindow(HWND window, HWND root) {
    DWORD rootPid=0, windowPid=0;
    GetWindowThreadProcessId(root,&rootPid);
    if(rootPid!=GetCurrentProcessId() || GetAncestor(window,GA_ROOT)!=root)
        return nullptr;
    for(HWND current=window;current && current!=root;current=GetParent(current)) {
        GetWindowThreadProcessId(current,&windowPid);
        if(windowPid!=rootPid) return nullptr;
        wchar_t className[64]{};
        GetClassNameW(current,className,ARRAYSIZE(className));
        if(wcscmp(className,L"EXCEL7")==0) return current;
    }
    return nullptr;
}

bool SameRect(const RECT& a,const RECT& b) {
    return a.left==b.left && a.top==b.top &&
           a.right==b.right && a.bottom==b.bottom;
}
bool ContainsRect(const RECT& outer,const RECT& inner) {
    return inner.right>inner.left && inner.bottom>inner.top &&
           inner.left>=outer.left && inner.top>=outer.top &&
           inner.right<=outer.right && inner.bottom<=outer.bottom;
}
bool ContainsPoint(const RECT& rect,POINT point) {
    return point.x>=rect.left && point.x<rect.right &&
           point.y>=rect.top && point.y<rect.bottom;
}

bool GetAccessibleRect(IAccessible* object, VARIANT child, RECT& rect) {
    LONG x=0, y=0, width=0, height=0;
    if (FAILED(object->accLocation(&x, &y, &width, &height, child)) ||
        width <= 0 || height <= 0) return false;
    rect = {x, y, x+width, y+height};
    return true;
}

bool NativeScrollTabs(IDispatch* excelWindow, int steps) {
    Variant argument, result;
    argument.value.vt=VT_I4; argument.value.lVal=steps;
    return SUCCEEDED(InvokeMember(excelWindow,L"ScrollWorkbookTabs",
        DISPATCH_METHOD,&result.value,&argument.value));
}

// The native sheet row is at the bottom of EXCEL7. Reject task-pane tabs
// before doing any accessibility lookup, even if they belong to XLMAIN.
bool IsSheetTab(POINT point,HWND worksheet,RECT& row) {
    RECT hostRect{};
    if(WindowFromPoint(point)!=worksheet || !GetWindowRect(worksheet,&hostRect))
        return false;
    LONG band=MulDiv(64,GetDpiForWindow(worksheet),96);
    if(point.y<hostRect.bottom-band || !ContainsPoint(hostRect,point)) return false;
    ComPtr<IAccessible> object;
    Variant child,role;
    RECT tab{};
    if(FAILED(AccessibleObjectFromPoint(point,&object.p,&child.value)) ||
       !object.p || FAILED(object.p->get_accRole(child.value,&role.value)) ||
       role.value.vt!=VT_I4 ||
       (role.value.lVal!=ROLE_SYSTEM_PAGETAB &&
        role.value.lVal!=ROLE_SYSTEM_PAGETABLIST) ||
       !GetAccessibleRect(object.p,child.value,tab) ||
       !ContainsRect(hostRect,tab) || !ContainsPoint(tab,point) ||
       tab.bottom-tab.top>band || tab.top<hostRect.bottom-band) return false;
    row={hostRect.left,tab.top,hostRect.right,tab.bottom};
    return true;
}

// The bottom band contains both tabs and the horizontal scroll bar. The
// scroll bar must retain Excel's WM_MOUSEHWHEEL behavior even on EXCEL7.
bool IsHorizontalSwitchTarget(HWND worksheet,POINT point,RECT& row) {
    RECT hostRect{};
    if(!GetWindowRect(worksheet,&hostRect) || !ContainsPoint(hostRect,point))
        return false;
    LONG band=MulDiv(64,GetDpiForWindow(worksheet),96);
    return point.y<hostRect.bottom-band || IsSheetTab(point,worksheet,row);
}

struct NativeArrowCandidates {
    RECT boxes[2]{};
    unsigned count=0;
};

// Scan from the EXCEL7 left edge, within the validated sheet row. The
// first two native buttons are the arrows; a later overflow button is ignored.
// A missing/foreign window is a hard boundary, never another search candidate.
template<class Probe>
bool FindNativeArrowPair(const RECT& searchRect,POINT point,Probe probe,
                         NativeArrowCandidates& arrows) {
    arrows.count=0;
    if(!ContainsPoint(searchRect,point)) return false;
    for(LONG x=searchRect.left+1;x<=point.x && x<searchRect.right;x+=4) {
        LONG role=0;
        RECT box{};
        bool bounds=false;
        if(!probe(POINT{x,point.y},role,box,bounds)) return false;
        if(role==ROLE_SYSTEM_PAGETAB || role==ROLE_SYSTEM_PAGETABLIST)
            return false; // Both arrows must precede the first tab.
        if(role!=ROLE_SYSTEM_PUSHBUTTON) continue;
        if(!bounds || !ContainsRect(searchRect,box) ||
           !ContainsPoint(box,POINT{x,point.y})) return false;
        if(arrows.count && box.left<arrows.boxes[0].right) return false;
        arrows.boxes[arrows.count++]=box;
        if(arrows.count==2) return true;
        x=box.right-1;
    }
    return false;
}

struct NativeArrowCache {
    HWND worksheet=nullptr;
    RECT hostRect{},row{},boxes[2]{};
    unsigned long long names[2]{};
    bool valid=false,failed=false;
    DWORD failedAt=0;
    bool Matches(HWND current,const RECT& host,const RECT& currentRow) const {
        return worksheet==current && SameRect(hostRect,host) && SameRect(row,currentRow);
    }
    bool IdentityMatches(unsigned index,const RECT& rect,unsigned long long name) const {
        return SameRect(boxes[index],rect) && names[index]==name;
    }
    bool FailedRecently(DWORD now) const {
        return failed && DWORD(now-failedAt)<500;
    }
    void Reset() { worksheet=nullptr; valid=false; failed=false; }
};
thread_local NativeArrowCache g_nativeArrowCache;

struct NativeArrowTarget {
    ComPtr<IAccessible> object;
    Variant child;
    RECT box{};
    HWND target=nullptr;
    unsigned long long name=0;
};
bool ReadNativeArrow(HWND worksheet,const RECT& row,const RECT& expected,NativeArrowTarget& arrow) {
    if(!ContainsRect(row,expected)) return false;
    POINT center{(expected.left+expected.right)/2,
                 (expected.top+expected.bottom)/2};
    arrow.target=WindowFromPoint(center);
    if(arrow.target!=worksheet) return false;
    if(FAILED(AccessibleObjectFromPoint(center,&arrow.object.p,
                                       &arrow.child.value)) ||
       !arrow.object.p) return false;
    Variant role;
    if(FAILED(arrow.object.p->get_accRole(arrow.child.value,&role.value)) ||
       role.value.vt!=VT_I4 || role.value.lVal!=ROLE_SYSTEM_PUSHBUTTON ||
       !GetAccessibleRect(arrow.object.p,arrow.child.value,arrow.box) ||
       !SameRect(expected,arrow.box)) return false;
    BSTR name=nullptr;
    if(SUCCEEDED(arrow.object.p->get_accName(arrow.child.value,&name)) && name) {
        arrow.name=14695981039346656037ULL;
        for(unsigned i=0;i<SysStringLen(name);++i) {
            arrow.name^=static_cast<unsigned short>(name[i]);
            arrow.name*=1099511628211ULL;
        }
    }
    SysFreeString(name);
    return true;
}

bool InvokeNativeArrow(NativeArrowTarget& arrow,int steps) {
    for(int i=0;i<steps;++i) {
        Variant state;
        if(FAILED(arrow.object.p->get_accState(arrow.child.value,&state.value)) ||
           state.value.vt!=VT_I4) return false;
        if(state.value.lVal&(STATE_SYSTEM_INVISIBLE|STATE_SYSTEM_OFFSCREEN)) return false;
        if(state.value.lVal&STATE_SYSTEM_UNAVAILABLE)
            return true;
        HRESULT hr=arrow.object.p->accDoDefaultAction(arrow.child.value);
        LogDiagnostic(L"nativeArrowResult",hr);
        if(FAILED(hr)) return false;
    }
    LogDiagnostic(L"nativeArrowOutcome",1);
    return true;
}

// Only geometry and fingerprints are cached, never COM interface pointers.
// Every event checks both controls, so moved panes cannot reuse stale buttons.
bool ScrollUsingNativeArrow(HWND worksheet,POINT point,const RECT& row,
                            int direction,int steps) {
    RECT hostRect{};
    if(!GetWindowRect(worksheet,&hostRect) ||
       !ContainsRect(hostRect,row) || !ContainsPoint(row,point)) return false;
    if(!g_nativeArrowCache.Matches(worksheet,hostRect,row)) g_nativeArrowCache.Reset();
    if(g_nativeArrowCache.FailedRecently(GetTickCount())) return false;
    if(g_nativeArrowCache.valid) {
        NativeArrowTarget cached[2];
        bool valid=true;
        for(unsigned i=0;i<2;++i) {
            if(!ReadNativeArrow(worksheet,row,g_nativeArrowCache.boxes[i],cached[i]) ||
               !g_nativeArrowCache.IdentityMatches(i,cached[i].box,cached[i].name)) {
                valid=false;break;
            }
        }
        if(valid) return InvokeNativeArrow(cached[direction>0?1:0],steps);
    }
    g_nativeArrowCache.Reset();
    g_nativeArrowCache.worksheet=worksheet;
    g_nativeArrowCache.hostRect=hostRect;
    g_nativeArrowCache.row=row;
    auto fail=[&]() {
        g_nativeArrowCache.failed=true;
        g_nativeArrowCache.failedAt=GetTickCount();
        return false;
    };
    NativeArrowCandidates arrows;
    // Even an unfamiliar Office build performs only a bounded initial scan.
    RECT searchRect=row;
    searchRect.right=std::min(row.right,row.left+MulDiv(256,GetDpiForWindow(worksheet),96));
    POINT searchPoint{std::min(point.x,searchRect.right-1),point.y};
    auto probe=[&](POINT sample,LONG& role,RECT& box,bool& bounds) {
        // This check happens BEFORE MSAA: never message a task-pane/foreign HWND.
        if(WindowFromPoint(sample)!=worksheet) return false;
        ComPtr<IAccessible> object;
        Variant child,value;
        if(FAILED(AccessibleObjectFromPoint(sample,&object.p,&child.value)) ||
           !object.p || FAILED(object.p->get_accRole(child.value,&value.value)) ||
           value.value.vt!=VT_I4) return false;
        role=value.value.lVal;
        bounds=role==ROLE_SYSTEM_PUSHBUTTON &&
               GetAccessibleRect(object.p,child.value,box);
        return true;
    };
    if(!FindNativeArrowPair(searchRect,searchPoint,probe,arrows)) return fail();
    NativeArrowTarget targets[2];
    for(unsigned i=0;i<2;++i) {
        if(!ReadNativeArrow(worksheet,row,arrows.boxes[i],targets[i])) return fail();
    }
    for(unsigned i=0;i<2;++i) {
        g_nativeArrowCache.boxes[i]=targets[i].box;
        g_nativeArrowCache.names[i]=targets[i].name;
    }
    g_nativeArrowCache.valid=true;
    return InvokeNativeArrow(targets[direction>0?1:0],steps);
}

// One horizontal gesture is a burst of input timestamps. Use the input's
// timestamp, not COM processing time, so a slow sheet cannot create a repeat.
struct HorizontalGesture {
    HWND root=nullptr;
    DWORD lastInput=0;
    int direction=0, accumulated=0;
    bool active=false, fired=false;
    bool Accept(HWND currentRoot,DWORD inputTime,int delta,unsigned idleMs) {
        if (!delta) return false;
        int currentDirection=delta>0?1:-1;
        if (!active || root!=currentRoot || currentDirection!=direction ||
            DWORD(inputTime-lastInput)>=idleMs) {
            root=currentRoot; direction=currentDirection;
            accumulated=0; fired=false; active=true;
        }
        lastInput=inputTime;
        if(fired) return false;
        accumulated+=delta;
        if (std::abs(accumulated)<WHEEL_DELTA) return false;
        fired=true;
        return true;
    }
    void Reset() { active=false; fired=false; accumulated=0; }
};
thread_local HorizontalGesture g_horizontalGesture;

// RTL can place the sheet arrows on the right and scrollbar buttons on the
// left. Only use left-edge arrow discovery in a known left-to-right layout.
// A missing COM property also falls back to Excel's safe tab-scroll API.
bool IsLeftToRightTabLayout(IDispatch* excelWindow,HWND worksheet) {
    if (GetWindowLongPtrW(worksheet,GWL_EXSTYLE)&WS_EX_LAYOUTRTL) return false;
    Variant rtl;
    return SUCCEEDED(InvokeMember(excelWindow,L"DisplayRightToLeft",
                                  DISPATCH_PROPERTYGET,&rtl.value)) &&
           rtl.value.vt==VT_BOOL && rtl.value.boolVal==VARIANT_FALSE;
}

bool ApplyWheelAction(HWND worksheet, int direction, bool horizontal, POINT point,
                      int steps,const RECT& tabRow) {
    ComPtr<IDispatch> excelWindow, application, activeSheet, workbook, sheets;
    AccessibleObjectFromWindow(worksheet,OBJID_NATIVEOM,IID_IDispatch,
                               reinterpret_cast<void**>(&excelWindow.p));
    if (!excelWindow.p || !GetDispatchProperty(excelWindow.p, L"Application", application))
        return false;
    Variant ready;
    if (FAILED(InvokeMember(application.p, L"Ready", DISPATCH_PROPERTYGET,
                            &ready.value)) || ready.value.vt != VT_BOOL ||
        ready.value.boolVal == VARIANT_FALSE) return false;
    if (!horizontal && g_scrollTabs.load()) {
        if (g_smooth.load() && IsLeftToRightTabLayout(excelWindow.p,worksheet) &&
            ScrollUsingNativeArrow(worksheet,point,tabRow,direction,steps))
            return true;
        return NativeScrollTabs(excelWindow.p,direction*steps);
    }
    if (!GetDispatchProperty(excelWindow.p, L"ActiveSheet", activeSheet) ||
        !GetDispatchProperty(activeSheet.p, L"Parent", workbook) ||
        !GetDispatchProperty(workbook.p, L"Sheets", sheets)) return false;
    LONG index = 0, count = 0;
    if (!GetNumber(activeSheet.p, L"Index", index) ||
        !GetNumber(sheets.p, L"Count", count) || count < 2) return true;
    for (LONG tried = 0; tried < count - 1; ++tried) {
        index += direction;
        if (index < 1 || index > count) {
            if (!g_wrap.load()) return true;
            index = index < 1 ? count : 1;
        }
        Variant argument;
        argument.value.vt = VT_I4;
        argument.value.lVal = index;
        ComPtr<IDispatch> candidate;
        LONG visible = 0;
        if (!GetDispatchProperty(sheets.p, L"Item", candidate, &argument.value) ||
            !GetNumber(candidate.p, L"Visible", visible)) return false;
        if (visible != -1) continue; // xlSheetVisible
        HRESULT hr = InvokeMember(candidate.p, L"Activate", DISPATCH_METHOD,
                                   nullptr);
        if (FAILED(hr)) Wh_Log(L"Sheet activation failed: %08X", hr);
        else {
            Wh_Log(L"Sheet activated: index=%d axis=%s", index,
                   horizontal ? L"horizontal" : L"vertical");
        }
        return SUCCEEDED(hr);
    }
    return true;
}

using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t g_originalDispatchMessageW;
LRESULT WINAPI DispatchMessageWHook(const MSG* message) {
    if (!message || (message->message != WM_MOUSEWHEEL &&
        message->message != WM_MOUSEHWHEEL) || g_insideHook)
        return g_originalDispatchMessageW(message);
    bool horizontal = message->message == WM_MOUSEHWHEEL;
    if (horizontal && !g_horizontal.load())
        return g_originalDispatchMessageW(message);
    HWND root = GetAncestor(message->hwnd, GA_ROOT);
    wchar_t className[64]{};
    GetClassNameW(root, className, ARRAYSIZE(className));
    if (wcscmp(className, L"XLMAIN") != 0 || GetForegroundWindow() != root ||
        !IsWindowEnabled(root) || GET_KEYSTATE_WPARAM(message->wParam) != 0 ||
        GetAsyncKeyState(VK_CONTROL) < 0 || GetAsyncKeyState(VK_SHIFT) < 0 ||
        GetAsyncKeyState(VK_MENU) < 0)
        return g_originalDispatchMessageW(message);
    POINT point{GET_X_LPARAM(message->lParam), GET_Y_LPARAM(message->lParam)};
    HWND underMouse = WindowFromPoint(point);
    HWND worksheet=GetWorksheetWindow(underMouse,root);
    // Embedded child controls and docked/cross-process task panes retain input.
    if(!worksheet || underMouse!=worksheet)
        return g_originalDispatchMessageW(message);
    g_insideHook = true;
    bool consumed = false;
    try {
        RECT tabRow{};
        bool eligible = horizontal ? IsHorizontalSwitchTarget(worksheet,point,tabRow)
                                   : IsSheetTab(point,worksheet,tabRow);
        if (eligible) {
            ULONGLONG now = GetTickCount64();
            if (horizontal) {
                int delta=GET_WHEEL_DELTA_WPARAM(message->wParam);
                consumed=true;
                if(g_horizontalGesture.Accept(root,message->time,delta,
                    g_horizontalGestureMs.load())) {
                    int direction=delta>0?1:-1;
                    if(g_reverse.load()) direction=-direction;
                    Wh_Log(L"Horizontal gesture accepted");
                    consumed=ApplyWheelAction(worksheet,direction,true,point,1,tabRow);
                    if (!consumed) g_horizontalGesture.Reset();
                } else {
                    Wh_Log(L"Horizontal gesture repeat suppressed");
                }
            } else {
            g_horizontalGesture.Reset();
            if (root != g_lastRoot || message->message != g_lastAxis ||
                now - g_lastWheel > 1000)
                g_deltaRemainder = 0;
            g_lastRoot = root;
            g_lastAxis = message->message;
            g_lastWheel = now;
            g_deltaRemainder += GET_WHEEL_DELTA_WPARAM(message->wParam);
            int steps = g_deltaRemainder / WHEEL_DELTA;
            g_deltaRemainder %= WHEEL_DELTA;
            consumed = true;
            bool tabScroll=g_scrollTabs.load();
            if (steps != 0 && (tabScroll || g_lastSwitch == 0 ||
                now - g_lastSwitch >= g_throttle.load())) {
                int direction = steps > 0 ? -1 : 1;
                if (g_reverse.load()) direction = -direction;
                consumed = ApplyWheelAction(worksheet,direction,false,point,
                    tabScroll ? std::min(8,std::abs(steps)) : 1,tabRow);
                if (consumed) g_lastSwitch = now;
            }
            }
        } else if (horizontal) {
            g_horizontalGesture.Reset();
        }
    } catch (...) {
        Wh_Log(L"Wheel handler failed; preserving Excel's normal behavior");
    }
    g_insideHook = false;
    return consumed ? 0 : g_originalDispatchMessageW(message);
}

void LoadSettings() {
    g_scrollTabs = wcscmp(WindhawkUtils::StringSetting::make(L"wheelAction"),L"scroll")==0;
    g_horizontal = Wh_GetIntSetting(L"horizontalSwitching") != 0;
    g_smooth = Wh_GetIntSetting(L"smoothAnimation") != 0;
    g_horizontalGestureMs = std::clamp(Wh_GetIntSetting(L"horizontalGestureMs"),150,2000);
    g_reverse = Wh_GetIntSetting(L"reverseDirection") != 0;
    g_wrap = Wh_GetIntSetting(L"wrapAround") != 0;
    g_throttle = std::clamp(Wh_GetIntSetting(L"throttleMs"), 0, 5000);
    LogDiagnostic(L"settingsSmooth",g_smooth.load());
    LogDiagnostic(L"horizontalGestureMs",g_horizontalGestureMs.load());
}
BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"Excel sheet wheel mod initializing: horizontal=%d", g_horizontal.load());
    BOOL hooked=WindhawkUtils::SetFunctionHook(DispatchMessageW,DispatchMessageWHook,
                                             &g_originalDispatchMessageW);
    if (hooked) LogDiagnostic(L"initializedPid", GetCurrentProcessId());
    return hooked;
}
void Wh_ModAfterInit() {}
void Wh_ModUninit() {}
void Wh_ModSettingsChanged() { LoadSettings(); }
