// ==WindhawkMod==
// @id              excel-wheel-scroll-sheets
// @name            Excel Wheel Sheet Navigation
// @name:zh-CN      Excel 滚轮切换工作表
// @description     Scroll the sheet tab strip with the vertical wheel and switch sheets with the horizontal wheel
// @description:zh-CN 使用原生箭头滚动标签栏，左右拨动一次切换一张工作表
// @version         0.5.3
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
The settings labels below are currently in Chinese; the project README explains the controls.
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
  $name: 标签栏上下滚轮行为
  $options:
  - activate: 直接切换工作表
  - scroll: 只滚动标签栏，点击切换
- smoothAnimation: true
  $name: 使用 Excel 原生标签滚动
  $description: 使用左下角左右箭头相同的滚动方式，动画速度由 Excel 决定。关闭后直接按标签滚动。
- horizontalGestureMs: 200
  $name: 左右拨动防重复（毫秒）
  $description: 同方向连续信号视为一次拨动，停顿达到此时长后可再次切换；反向拨动立即切换。默认 200；若一次拨动仍切换多张，可适当增加。
- horizontalSwitching: true
  $name: 左右滚轮切换工作表
  $description: 鼠标在单元格或底部工作表标签上时，向右切换下一张，向左切换上一张。关闭后恢复原有横向滚动。
- reverseDirection: false
  $name: 反转滚轮方向
- throttleMs: 150
  $name: 切换间隔（毫秒）
  $description: 防止快速滚动时连续跳过多张工作表，设为 0 取消限制。
- wrapAround: false
  $name: 到达首尾时循环切换
  $description: 仅用于“直接切换工作表”模式。开启后，最后一张的下一张为第一张，反之亦然。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <oleacc.h>
#include <oleauto.h>
#include <algorithm>
#include <atomic>
#include <string>
#include <vector>

std::atomic<bool> g_reverse{false}, g_wrap{false};
std::atomic<bool> g_scrollTabs{false};
std::atomic<bool> g_horizontal{true};
std::atomic<unsigned> g_throttle{150};
std::atomic<bool> g_smooth{true};
std::atomic<unsigned> g_horizontalGestureMs{200};
// Numeric diagnostics use Windhawk's own optional logging and local storage.
void RecordDiagnostic(PCWSTR name, int value) {
    Wh_SetIntValue(name, value);
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

bool GetObject(IDispatch* parent, const wchar_t* name, ComPtr<IDispatch>& result,
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

BOOL CALLBACK FindExcelDocument(HWND window, LPARAM data) {
    wchar_t className[64]{};
    GetClassNameW(window, className, ARRAYSIZE(className));
    if (wcscmp(className, L"EXCEL7") != 0 || !IsWindowVisible(window)) return TRUE;
    auto result = reinterpret_cast<ComPtr<IDispatch>*>(data);
    HRESULT hr = AccessibleObjectFromWindow(window, OBJID_NATIVEOM,
                                            IID_IDispatch,
                                            reinterpret_cast<void**>(&result->p));
    return FAILED(hr) || !result->p;
}

bool IsSheetTab(POINT point, HWND root) {
    RECT rect{};
    if (!GetWindowRect(root, &rect)) return false;
    // Functional tabs are above the document; sheet tabs are below it.
    if (point.y < rect.top + (rect.bottom - rect.top) / 2) return false;
    ComPtr<IAccessible> object;
    Variant child, role;
    if (FAILED(AccessibleObjectFromPoint(point, &object.p, &child.value)) ||
        !object.p) return false;
    if (FAILED(object.p->get_accRole(child.value, &role.value))) return false;
    if (role.value.vt == VT_I4 &&
        (role.value.lVal == ROLE_SYSTEM_PAGETAB ||
         role.value.lVal == ROLE_SYSTEM_PAGETABLIST)) return true;
    return false;
}

bool IsWorksheetPoint(HWND window, HWND root) {
    for (HWND current = window; current && current != root;
         current = GetParent(current)) {
        wchar_t className[64]{};
        GetClassNameW(current, className, ARRAYSIZE(className));
        if (wcscmp(className, L"EXCEL7") == 0) return true;
    }
    return false;
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

// Keep the two buttons immediately before the first visible sheet tab.
// A left task pane can add earlier buttons and move the arrows past x=200.
struct NativeArrowCandidates {
    RECT boxes[2]{};
    unsigned count=0;
    void Add(const RECT& box) {
        if(count) {
            const RECT& last=boxes[count-1];
            if(last.left==box.left && last.top==box.top &&
               last.right==box.right && last.bottom==box.bottom) return;
        }
        if(count<2) boxes[count++]=box;
        else { boxes[0]=boxes[1]; boxes[1]=box; }
    }
    const RECT* Select(int direction) const {
        return count==2 ? &boxes[direction>0?1:0] : nullptr;
    }
};

// The discovery algorithm starts at the wheel's sheet tab, not the left edge
// of the window. Whole tab rectangles can be skipped in one accessibility call.
template<class Probe>
bool FindNativeArrowPair(const RECT& rootRect, POINT point, Probe probe,
                         NativeArrowCandidates& arrows) {
    RECT rightArrow{};
    bool haveRight=false;
    for(LONG x=point.x; x>rootRect.left; x-=4) {
        LONG role=0;
        RECT box{};
        bool bounds=false;
        if(!probe(POINT{x,point.y},role,box,bounds)) continue;
        if(role==ROLE_SYSTEM_PAGETAB) {
            if(bounds && box.left<=x) x=std::min(x,box.left);
            continue;
        }
        if(role!=ROLE_SYSTEM_PUSHBUTTON || !bounds ||
           box.bottom-box.top>80 || box.right-box.left>80) continue;
        if(!haveRight) { rightArrow=box; haveRight=true; }
        else {
            arrows.Add(box); arrows.Add(rightArrow);
            return true;
        }
        x=std::min(x,box.left);
    }
    return false;
}

struct NativeArrowCache {
    HWND root=nullptr;
    RECT rootRect{}, boxes[2]{};
    HWND targets[2]{};
    unsigned long long names[2]{};
    static bool SameRect(const RECT& a,const RECT& b) {
        return a.left==b.left && a.top==b.top &&
               a.right==b.right && a.bottom==b.bottom;
    }
    bool Matches(HWND current,const RECT& rect,POINT point) const {
        return root==current && SameRect(rootRect,rect) &&
            point.y>=boxes[0].top && point.y<boxes[0].bottom &&
            point.y>=boxes[1].top && point.y<boxes[1].bottom;
    }
    bool IdentityMatches(unsigned index,const RECT& rect,HWND target,
                         unsigned long long name) const {
        return SameRect(boxes[index],rect) && targets[index]==target &&
               names[index]==name;
    }
    void Reset() { root=nullptr; }
};
thread_local NativeArrowCache g_nativeArrowCache;

struct NativeArrowTarget {
    ComPtr<IAccessible> object;
    Variant child;
    RECT box{};
    HWND target=nullptr;
    unsigned long long name=0;
};
bool ReadNativeArrow(HWND root,const RECT& expected,NativeArrowTarget& arrow) {
    POINT center{(expected.left+expected.right)/2,
                 (expected.top+expected.bottom)/2};
    arrow.target=WindowFromPoint(center);
    if(!arrow.target || GetAncestor(arrow.target,GA_ROOT)!=root) return false;
    if(FAILED(AccessibleObjectFromPoint(center,&arrow.object.p,
                                       &arrow.child.value)) ||
       !arrow.object.p) return false;
    Variant role;
    if(FAILED(arrow.object.p->get_accRole(arrow.child.value,&role.value)) ||
       role.value.vt!=VT_I4 || role.value.lVal!=ROLE_SYSTEM_PUSHBUTTON ||
       !GetAccessibleRect(arrow.object.p,arrow.child.value,arrow.box) ||
       !NativeArrowCache::SameRect(expected,arrow.box)) return false;
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
        arrow.object.p->get_accState(arrow.child.value,&state.value);
        if(state.value.vt==VT_I4 && (state.value.lVal&STATE_SYSTEM_UNAVAILABLE))
            return true;
        HRESULT hr=arrow.object.p->accDoDefaultAction(arrow.child.value);
        RecordDiagnostic(L"nativeArrowResult",hr);
        if(FAILED(hr)) return false;
    }
    RecordDiagnostic(L"nativeArrowOutcome",1);
    return true;
}

// Only geometry and fingerprints are cached, never COM interface pointers.
// Every event checks both controls, so moved panes cannot reuse stale buttons.
bool ScrollUsingNativeArrow(HWND root,POINT point,int direction,int steps) {
    RECT rootRect{};
    if(!GetWindowRect(root,&rootRect)) return false;
    if(g_nativeArrowCache.Matches(root,rootRect,point)) {
        NativeArrowTarget cached[2];
        bool valid=true;
        for(unsigned i=0;i<2;++i) {
            if(!ReadNativeArrow(root,g_nativeArrowCache.boxes[i],cached[i]) ||
               !g_nativeArrowCache.IdentityMatches(i,cached[i].box,
                    cached[i].target,cached[i].name)) { valid=false; break; }
        }
        if(valid) {
            Wh_Log(L"Native arrow cache hit");
            return InvokeNativeArrow(cached[direction>0?1:0],steps);
        }
    }
    g_nativeArrowCache.Reset();
    NativeArrowCandidates arrows;
    unsigned probes=0;
    auto probe=[&](POINT sample,LONG& role,RECT& box,bool& bounds) {
        ++probes;
        ComPtr<IAccessible> object;
        Variant child,value;
        if(FAILED(AccessibleObjectFromPoint(sample,&object.p,&child.value)) ||
           !object.p || FAILED(object.p->get_accRole(child.value,&value.value)) ||
           value.value.vt!=VT_I4) return false;
        role=value.value.lVal;
        bounds=(role==ROLE_SYSTEM_PAGETAB || role==ROLE_SYSTEM_PUSHBUTTON) &&
                GetAccessibleRect(object.p,child.value,box);
        return true;
    };
    if(!FindNativeArrowPair(rootRect,point,probe,arrows)) {
        RecordDiagnostic(L"nativeArrowOutcome",-1);
        return false;
    }
    NativeArrowTarget targets[2];
    for(unsigned i=0;i<2;++i) {
        if(!ReadNativeArrow(root,arrows.boxes[i],targets[i])) return false;
    }
    g_nativeArrowCache.root=root;
    g_nativeArrowCache.rootRect=rootRect;
    for(unsigned i=0;i<2;++i) {
        g_nativeArrowCache.boxes[i]=targets[i].box;
        g_nativeArrowCache.targets[i]=targets[i].target;
        g_nativeArrowCache.names[i]=targets[i].name;
    }
    RecordDiagnostic(L"nativeArrowProbeCount",probes);
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
thread_local int g_horizontalAccepted=0, g_horizontalSuppressed=0;

bool ApplyWheelAction(HWND root, int direction, bool horizontal, POINT point, int steps) {
    ComPtr<IDispatch> excelWindow, application, activeSheet, workbook, sheets;
    EnumChildWindows(root, FindExcelDocument,
                     reinterpret_cast<LPARAM>(&excelWindow));
    if (!excelWindow.p || !GetObject(excelWindow.p, L"Application", application))
        return false;
    Variant ready;
    if (FAILED(InvokeMember(application.p, L"Ready", DISPATCH_PROPERTYGET,
                            &ready.value)) || ready.value.vt != VT_BOOL ||
        ready.value.boolVal == VARIANT_FALSE) return false;
    if (!horizontal && g_scrollTabs.load()) {
        if (g_smooth.load() && ScrollUsingNativeArrow(root,point,direction,steps))
            return true;
        return NativeScrollTabs(excelWindow.p,direction*steps);
    }
    if (!GetObject(excelWindow.p, L"ActiveSheet", activeSheet) ||
        !GetObject(activeSheet.p, L"Parent", workbook) ||
        !GetObject(workbook.p, L"Sheets", sheets)) return false;
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
        if (!GetObject(sheets.p, L"Item", candidate, &argument.value) ||
            !GetNumber(candidate.p, L"Visible", visible)) return false;
        if (visible != -1) continue; // xlSheetVisible
        HRESULT hr = InvokeMember(candidate.p, L"Activate", DISPATCH_METHOD,
                                   nullptr);
        if (FAILED(hr)) Wh_Log(L"Sheet activation failed: %08X", hr);
        else {
            Wh_Log(L"Sheet activated: index=%d axis=%s", index,
                   horizontal ? L"horizontal" : L"vertical");
            Wh_SetIntValue(L"lastActivatedSheetIndex", index);
            Wh_SetIntValue(L"lastActivationAxis", horizontal ? 2 : 1);
            Wh_SetIntValue(L"lastActivationPid", GetCurrentProcessId());
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
    if (GetAncestor(underMouse, GA_ROOT) != root)
        return g_originalDispatchMessageW(message);
    g_insideHook = true;
    bool consumed = false;
    try {
        if ((horizontal && IsWorksheetPoint(underMouse, root)) ||
            IsSheetTab(point, root)) {
            ULONGLONG now = GetTickCount64();
            if (horizontal) {
                int delta=GET_WHEEL_DELTA_WPARAM(message->wParam);
                consumed=true;
                if(g_horizontalGesture.Accept(root,message->time,delta,
                    g_horizontalGestureMs.load())) {
                    int direction=delta>0?1:-1;
                    if(g_reverse.load()) direction=-direction;
                    RecordDiagnostic(L"horizontalAccepted",++g_horizontalAccepted);
                    consumed=ApplyWheelAction(root,direction,true,point,1);
                    if (!consumed) g_horizontalGesture.Reset();
                } else {
                    RecordDiagnostic(L"horizontalSuppressed",++g_horizontalSuppressed);
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
            bool tabScroll=!horizontal && g_scrollTabs.load();
            if (steps != 0 && (tabScroll || g_lastSwitch == 0 ||
                now - g_lastSwitch >= g_throttle.load())) {
                int direction = steps > 0 ? -1 : 1;
                if (horizontal) direction = -direction;
                if (g_reverse.load()) direction = -direction;
                consumed = ApplyWheelAction(root,direction,horizontal,point,
                    tabScroll ? std::min(8,std::abs(steps)) : 1);
                if (consumed) g_lastSwitch = now;
            }
            }
        }
    } catch (...) {
        Wh_Log(L"Wheel handler failed; preserving Excel's normal behavior");
    }
    g_insideHook = false;
    return consumed ? 0 : g_originalDispatchMessageW(message);
}

void LoadSettings() {
    PCWSTR action = Wh_GetStringSetting(L"wheelAction");
    g_scrollTabs = action && wcscmp(action, L"scroll") == 0;
    Wh_FreeStringSetting(action);
    g_horizontal = Wh_GetIntSetting(L"horizontalSwitching") != 0;
    g_smooth = Wh_GetIntSetting(L"smoothAnimation") != 0;
    g_horizontalGestureMs = std::clamp(Wh_GetIntSetting(L"horizontalGestureMs"),150,2000);
    g_reverse = Wh_GetIntSetting(L"reverseDirection") != 0;
    g_wrap = Wh_GetIntSetting(L"wrapAround") != 0;
    g_throttle = std::clamp(Wh_GetIntSetting(L"throttleMs"), 0, 5000);
    RecordDiagnostic(L"settingsSmooth",g_smooth.load());
    RecordDiagnostic(L"horizontalGestureMs",g_horizontalGestureMs.load());
}
BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"Excel sheet wheel mod initializing: horizontal=%d", g_horizontal.load());
    BOOL hooked = Wh_SetFunctionHook(reinterpret_cast<void*>(DispatchMessageW),
        reinterpret_cast<void*>(DispatchMessageWHook),
        reinterpret_cast<void**>(&g_originalDispatchMessageW));
    if (hooked) RecordDiagnostic(L"initializedPid", GetCurrentProcessId());
    return hooked;
}
void Wh_ModAfterInit() {}
void Wh_ModUninit() {}
void Wh_ModSettingsChanged() { LoadSettings(); }
