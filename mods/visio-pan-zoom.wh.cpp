// ==WindhawkMod==
// @id              visio-pan-zoom
// @name            Visio: Middle Mouse Pan & Smart Zoom
// @name:zh-CN      Visio 中键平移与智能缩放
// @description     Brings modern CAD-like navigation to Visio: Middle-drag to pan, scroll wheel to zoom.
// @description:zh-CN 为 Visio 带来现代化的导航体验：中键拖拽平移，滚轮直接缩放，完美兼容触摸板。
// @version         1.0
// @author          Joe Ye
// @github          https://github.com/JoeYe-233
// @include         VISIO.EXE
// @compilerOptions -lcomctl32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Visio: Middle Pan & Smart Zoom

This mod brings modern CAD/design software navigation experience to Microsoft Visio, allowing you to navigate the canvas with just *one hand*.

### Features
* **Middle-Click Panning:** Press and hold the middle mouse button to freely pan the canvas (replaces the cumbersome `Ctrl + Shift + Right Click` default).
* **Smart Zoom:** Use the mouse scroll wheel to zoom in and out directly (replaces `Ctrl + Scroll`).
* **Horizontal Scroll Kept:** Holding `Shift` while scrolling will still pan the canvas horizontally.
* **Touchpad Perfection:** Intelligently distinguishes between a physical mouse and a Windows Precision Touchpad. Your native two-finger scrolling and pinch-to-zoom gestures are preserved perfectly without interference.

### Settings Note
If you are using a mouse with an "infinite / free-spinning / continuous" scroll wheel (like the Logitech MX Master series with MagSpeed), the Smart Zoom might not trigger correctly because the wheel sends micro-signals similar to a touchpad. You can fix this by disabling the **"Preserve Native Touchpad Scroll"** option in the mod's Settings page.

---

# Visio: 中键平移与智能缩放

为 Microsoft Visio 带来类似于 CAD 或设计软件的现代化导航体验，只需使用单手即可进行画布导航，彻底告别反人类的默认快捷键操作。

### 核心功能
* **中键平移画布：** 按住鼠标中键即可自由拖拽平移画布（完美替代原生繁琐的 `Ctrl + Shift + 右键` 操作）。
* **滚轮智能缩放：** 直接滚动鼠标滚轮即可进行画布缩放（替代 `Ctrl + 滚轮`）。
* **保留水平滚动：** 按住 `Shift` 键滚动滚轮，依然可以水平移动画布。
* **完美兼容触摸板：** 模块底层内置了智能启发式识别算法，能够精确区分物理鼠标刻度与 Windows 精密触摸板。触摸板原生的双指上下滑动与捏合缩放手势将被完美保留，互不干扰。

### 设置项说明
如果您使用的是支持“无极滚轮/自由滚轮”的鼠标（例如罗技 MX Master 系列的 MagSpeed），由于其自由滚动模式下产生的微小信号特征与触摸板类似，可能会被算法识别为触摸板导致智能缩放失效。遇到此类情况，请在模块的设置页中关闭【保留原生触摸板滚动】选项。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- isolateTouchpad: true
  $name: Preserve Native Touchpad Scroll
  $name:zh-CN: 保留原生触摸板滚动
  $description: |
    Why this is needed: Touchpads essentially send rapid, small mouse wheel signals to Windows. If this mod intercepts all wheel signals, your standard two-finger vertical scroll on a Windows Precision Touchpad will be forcefully converted into zooming, which is highly unintuitive.
     - Keep this ENABLED on laptops to intelligently differentiate touchpad micro-scrolls from physical mouse clicks, preserving your native touchpad scrolling and pinching.
     - EXCEPTION: Mice with "free-spinning" or "infinite" scroll wheels (e.g., Logitech MX Master series with MagSpeed) also generate micro-scroll signals and might be misidentified as touchpads by this algorithm. If your free-spinning mouse wheel fails to trigger Smart Zoom, UNCHECK this option.
  $description:zh-CN: |
    触摸板双指滑动本质上也是在向系统发送高频的微小鼠标滚轮信号。如果本模组盲目拦截所有滚轮信号，你在 Windows 精密触摸板上的双指上下滑动操作将被强制转换为缩放，极为反直觉且难用。
     - 建议在笔记本电脑上始终保持本选项【开启】，本模组将智能区分触摸板的微小滚动与物理鼠标滚轮的明确刻度，从而保留原生的触摸板双指滚动和捏合缩放体验。
     - 注意：带有“无极滚轮”功能的鼠标（例如罗技 MX Master 系列的 MagSpeed）在自由滚动模式下产生的信号和触摸板类似，可能会被误判为触摸板。如果你的无极滚轮无法正常触发智能缩放，请【关闭】本选项。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windhawk_api.h>
#include <vector>
#include <tlhelp32.h>

bool g_isPanning = false;
bool g_isWheeling = false;
bool g_isolateTouchpad = true;

std::vector<HHOOK> g_msgHooks;

// -----------------------------------------------------------------------------
// Settings Loading Logic (Supports Dynamic Updates)
// -----------------------------------------------------------------------------
void LoadSettings() {
    g_isolateTouchpad = Wh_GetIntSetting(L"isolateTouchpad") != 0;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

// -----------------------------------------------------------------------------
// 1. Physical Input Simulation (SendInput)
// -----------------------------------------------------------------------------
void SendPhysicalInput(bool down) {
    INPUT inputs[3] = {};
    // Simulate pressing Ctrl + Alt (Visio's hard requirement for panning)
    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_CONTROL; inputs[0].ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_MENU;    inputs[1].ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
    // Simulate middle mouse button down/up
    inputs[2].type = INPUT_MOUSE;    inputs[2].mi.dwFlags = down ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
    
    SendInput(3, inputs, sizeof(INPUT));
}

// -----------------------------------------------------------------------------
// 2. API Hook
// -----------------------------------------------------------------------------
typedef SHORT (WINAPI *GetKeyState_t)(int nVirtKey);
GetKeyState_t pOriginalGetKeyState;

SHORT WINAPI DetourGetKeyState(int nVirtKey) {
    // Zoom Fallback: When Smart Zoom is active, force the system to think the Ctrl key is pressed (even if the user isn't actually pressing it), thereby triggering Visio's zoom logic.
    if (g_isWheeling && nVirtKey == VK_CONTROL) {
        return pOriginalGetKeyState(nVirtKey) ^ 0x8000;
    }
    // Panning Fallback: When middle-button panning is active, force the system to think Ctrl, Alt, and Right Mouse Button are pressed, ensuring that even if the user releases the middle button or the SendInput fails for some reason, the panning state remains consistent.
    if (g_isPanning) {
        if (nVirtKey == VK_CONTROL || nVirtKey == VK_MENU || nVirtKey == VK_RBUTTON) return 0x8000;
    }
    
    return pOriginalGetKeyState(nVirtKey);
}

// -----------------------------------------------------------------------------
// 3. Message Hook (WH_GETMESSAGE) for Wheel and Middle Button Logic
// -----------------------------------------------------------------------------
LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= 0) {
        MSG* pMsg = (MSG*)lParam;

        // [A] Wheel Logic: Heuristic Detection + Smart Zoom Trigger
        if (pMsg->message == WM_MOUSEWHEEL) {
            WORD fwKeys = LOWORD(pMsg->wParam);
            short zDelta = HIWORD(pMsg->wParam);
            
            // 1. System Ctrl (e.g., from pinch-to-zoom) -> Let it pass through
            if (fwKeys & MK_CONTROL) {
                g_isWheeling = false; 
            }
            // 2. User is holding Shift -> This is likely a horizontal scroll, so let it pass through
            else if (fwKeys & MK_SHIFT) {
                g_isWheeling = false;
            }
            // 3. If the "Preserve Native Touchpad Scroll" option is enabled and the current scroll is not a standard 120 increment, treat it as a touchpad scroll and let it pass through
            else if (g_isolateTouchpad && (zDelta % 120 != 0)) {
                g_isWheeling = false;
            }
            // 4. Otherwise, this is likely a physical mouse wheel event intended for zooming, so we set the flag and modify the wParam to include Ctrl for zooming
            else {
                g_isWheeling = true;
                fwKeys |= MK_CONTROL;
                pMsg->wParam = MAKEWPARAM(fwKeys, zDelta);
            }
        }
        else if (pMsg->message != WM_MOUSEWHEEL) {
            g_isWheeling = false;
        }

        // [B] Middle Button Panning Logic
        if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_NCMOUSEMOVE) {
            bool isPhysicalMiddleDown = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

            if (isPhysicalMiddleDown && !g_isPanning) {
                g_isPanning = true;
                SendPhysicalInput(true);
            }
            else if (!isPhysicalMiddleDown && g_isPanning) {
                g_isPanning = false;
                SendPhysicalInput(false);
            }

            if (g_isPanning) {
                if (pMsg->message == WM_MBUTTONDOWN || pMsg->message == WM_MBUTTONUP || pMsg->message == WM_MBUTTONDBLCLK) {
                    pMsg->message = WM_NULL;
                }
                pMsg->wParam &= ~MK_MBUTTON; 
            }
        }
        // If the message is a middle button down/up event but we're not in a panning state and the physical middle button isn't actually down, we should also nullify it to prevent accidental clicks from triggering Visio's default behavior. This can happen if the user quickly taps the middle button without holding it, which might be intended for something else (like auto-scroll) but could trigger the default panning if not handled.
        else if (pMsg->message == WM_MBUTTONDOWN || pMsg->message == WM_MBUTTONUP) {
             if (g_isPanning || (GetAsyncKeyState(VK_MBUTTON) & 0x8000)) {
                 pMsg->message = WM_NULL;
             }
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}

// -----------------------------------------------------------------------------
// Module Initialization and Cleanup
// -----------------------------------------------------------------------------
BOOL Wh_ModInit() {
    LoadSettings(); // Initial settings load

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        void* pGetKeyState = (void*)GetProcAddress(hUser32, "GetKeyState");
        if (pGetKeyState) Wh_SetFunctionHook(pGetKeyState, (void*)DetourGetKeyState, (void**)&pOriginalGetKeyState);
    }

    DWORD currentProcessId = GetCurrentProcessId();
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);
        if (Thread32First(hThreadSnap, &te32)) {
            do {
                if (te32.th32OwnerProcessID == currentProcessId) {
                    HHOOK hMsg = SetWindowsHookExW(WH_GETMESSAGE, GetMsgProc, NULL, te32.th32ThreadID);
                    if (hMsg) g_msgHooks.push_back(hMsg);
                }
            } while (Thread32Next(hThreadSnap, &te32));
        }
        CloseHandle(hThreadSnap);
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (g_isPanning) {
        SendPhysicalInput(false);
    }
    for (HHOOK h : g_msgHooks) UnhookWindowsHookEx(h);
    g_msgHooks.clear();
}