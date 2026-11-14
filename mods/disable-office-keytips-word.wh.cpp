// ==WindhawkMod==
// @id              disable-office-keytips-word
// @name            Disable accidental Office KeyTips in Word
// @description     Prevent accidental yellow Office KeyTips activation in Microsoft Word during layout/language switching and stray Alt-like transitions
// @version         1.0
// @author          communism420
// @github          https://github.com/communism420
// @include         WINWORD.EXE
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable accidental Office KeyTips in Word

Prevent Microsoft Word from entering the yellow Office KeyTips overlay
accidentally during keyboard layout or language switching, or due to stray
Alt-related state transitions.

## What it does

This mod is intended for cases where Word unexpectedly enters ribbon keyboard
navigation mode and shows the yellow KeyTips boxes while you're editing text.

The implementation uses a conservative prevention-based approach:

* Tracks modifier-key transitions that commonly lead to accidental KeyTips
  activation.
* Suppresses isolated Alt activation when configured.
* Suppresses layout-switch artifacts after `Ctrl+Shift`, `Alt+Shift`, and
  `Win+Space`.
* Blocks suspicious menu activation paths such as `SC_KEYMENU` when they match
  an accidental pattern.

The mod is scoped to `WINWORD.EXE` and does **not** disable Alt globally or
blindly swallow all `WM_SYS*` messages.

## Settings

* **Suppress single Alt activation**: Blocks obvious isolated Alt-triggered
  KeyTips activation.
* **Suppress layout switch artifacts**: Suppresses KeyTips activation patterns
  caused by layout/language switching side effects.
* **Preserve intentional Alt navigation**: Allows plain Alt ribbon navigation
  when possible. Disabled by default to prioritize suppression of accidental
  KeyTips.
* **Suppress menu activation paths**: Blocks suspicious menu/system activation
  paths such as `SC_KEYMENU`.

## Notes

With the default settings, deliberate plain-Alt ribbon navigation may be
reduced. This is intentional: the primary goal is to stop unwanted KeyTips
activation while keeping normal typing and common Ctrl shortcuts stable.

## Screenshot of the problem

![Word KeyTips](https://raw.githubusercontent.com/communism420/Media-Host-For-My-Windhawk-Mods/refs/heads/main/disable%20office%20keytips.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- suppressSingleAlt: true
  $name: Suppress single Alt activation
  $description: Block obvious isolated Alt-triggered KeyTips activation in Word.
- suppressLayoutSwitchArtifacts: true
  $name: Suppress layout switch artifacts
  $description: Suppress KeyTips activation patterns caused by layout/language switching side effects.
- preserveIntentionalAltNavigation: false
  $name: Preserve intentional Alt navigation
  $description: Allow plain Alt navigation when possible. Disabling this keeps the mod more aggressive against accidental KeyTips.
- suppressMenuActivationPaths: true
  $name: Suppress menu activation paths
  $description: Block suspicious system/menu activation paths such as SC_KEYMENU when they follow an accidental trigger pattern.
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <windows.h>
#include <commctrl.h>
#include <windhawk_utils.h>

#include <mutex>

struct Settings
{
    bool suppressSingleAlt = true;
    bool suppressLayoutSwitchArtifacts = true;
    bool preserveIntentionalAltNavigation = false;
    bool suppressMenuActivationPaths = true;
};

struct InputState
{
    bool leftAltDown = false;
    bool rightAltDown = false;
    bool leftCtrlDown = false;
    bool rightCtrlDown = false;
    bool leftShiftDown = false;
    bool rightShiftDown = false;
    bool leftWinDown = false;
    bool rightWinDown = false;

    bool altChordUsed = false;
    bool altWasUsedForLayoutSwitch = false;

    DWORD altPressTick = 0;
    DWORD lastLayoutComboTick = 0;
    DWORD lastInputLangChangeTick = 0;
    DWORD lastWinSpaceTick = 0;
    DWORD suppressUntilTick = 0;
};

namespace
{
constexpr DWORD kLayoutArtifactWindowMs = 450;
constexpr DWORD kKeyTipFollowupWindowMs = 250;
constexpr DWORD kFocusRecoveryWindowMs = 250;
constexpr DWORD kRecentAltWindowMs = 1200;

Settings g_settings;
InputState g_state;
std::mutex g_stateMutex;
bool g_initializedForWord = false;

using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_orig = nullptr;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_orig = nullptr;

bool AnyAltDownLocked()
{
    return g_state.leftAltDown || g_state.rightAltDown;
}

bool AnyCtrlDownLocked()
{
    return g_state.leftCtrlDown || g_state.rightCtrlDown;
}

bool AnyShiftDownLocked()
{
    return g_state.leftShiftDown || g_state.rightShiftDown;
}

bool AnyWinDownLocked()
{
    return g_state.leftWinDown || g_state.rightWinDown;
}

bool IsLikelyAltGrStateLocked()
{
    return g_state.rightAltDown && AnyCtrlDownLocked();
}

bool IsTickActive(DWORD now, DWORD untilTick)
{
    return untilTick != 0 && static_cast<LONG>(untilTick - now) >= 0;
}

bool HasRecentEvent(DWORD now, DWORD eventTick, DWORD windowMs)
{
    return eventTick != 0 && static_cast<DWORD>(now - eventTick) <= windowMs;
}

bool IsSuppressionActiveLocked(DWORD now)
{
    return IsTickActive(now, g_state.suppressUntilTick);
}

void ExtendSuppressionLocked(DWORD now, DWORD durationMs, PCWSTR reason)
{
    DWORD candidate = now + durationMs;
    if (!IsTickActive(now, g_state.suppressUntilTick) ||
        static_cast<LONG>(candidate - g_state.suppressUntilTick) > 0)
    {
        g_state.suppressUntilTick = candidate;
    }

    Wh_Log(L"Suppression window extended for %lu ms (%s), until=%lu",
           durationMs,
           reason ? reason : L"no-reason",
           g_state.suppressUntilTick);
}

void LoadSettings()
{
    g_settings.suppressSingleAlt =
        Wh_GetIntSetting(L"suppressSingleAlt") != 0;
    g_settings.suppressLayoutSwitchArtifacts =
        Wh_GetIntSetting(L"suppressLayoutSwitchArtifacts") != 0;
    g_settings.preserveIntentionalAltNavigation =
        Wh_GetIntSetting(L"preserveIntentionalAltNavigation") != 0;
    g_settings.suppressMenuActivationPaths =
        Wh_GetIntSetting(L"suppressMenuActivationPaths") != 0;
}

bool IsWordFrameWindow(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))
    {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId())
    {
        return false;
    }

    WCHAR className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)))
    {
        return false;
    }

    // Word main document windows use the OpusApp frame class.
    return wcscmp(className, L"OpusApp") == 0;
}

HWND GetWordRootWindow(HWND hWnd)
{
    if (!hWnd)
    {
        return nullptr;
    }

    HWND root = GetAncestor(hWnd, GA_ROOT);
    if (!root)
    {
        return nullptr;
    }

    return IsWordFrameWindow(root) ? root : nullptr;
}

UINT ResolveVirtualKey(WPARAM wParam, LPARAM lParam)
{
    UINT vk = static_cast<UINT>(wParam);

    if (vk == VK_SHIFT)
    {
        UINT scanCode = (static_cast<UINT>(lParam) >> 16) & 0xFF;
        UINT mappedVk = MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX);
        if (mappedVk == VK_LSHIFT || mappedVk == VK_RSHIFT)
        {
            return mappedVk;
        }
    }
    else if (vk == VK_CONTROL)
    {
        return (lParam & 0x01000000) ? VK_RCONTROL : VK_LCONTROL;
    }
    else if (vk == VK_MENU)
    {
        return (lParam & 0x01000000) ? VK_RMENU : VK_LMENU;
    }

    return vk;
}

void SetModifierStateLocked(UINT vk, bool isDown)
{
    switch (vk)
    {
        case VK_LSHIFT:
            g_state.leftShiftDown = isDown;
            break;

        case VK_RSHIFT:
            g_state.rightShiftDown = isDown;
            break;

        case VK_LCONTROL:
            g_state.leftCtrlDown = isDown;
            break;

        case VK_RCONTROL:
            g_state.rightCtrlDown = isDown;
            break;

        case VK_LMENU:
            g_state.leftAltDown = isDown;
            break;

        case VK_RMENU:
            g_state.rightAltDown = isDown;
            break;

        case VK_LWIN:
            g_state.leftWinDown = isDown;
            break;

        case VK_RWIN:
            g_state.rightWinDown = isDown;
            break;
    }
}

void PostCancelMode(HWND root, PCWSTR reason)
{
    if (!root || !IsWindow(root))
    {
        return;
    }

    Wh_Log(L"Posting WM_CANCELMODE to %p (%s)",
           root,
           reason ? reason : L"no-reason");
    PostMessageW(root, WM_CANCELMODE, 0, 0);
}

bool ProcessQueuedKeyboardMessage(HWND root, const MSG* msg, DWORD now)
{
    // Queued keyboard messages are the safest place to watch the real modifier
    // sequence that Word receives while the focus is inside the document view.
    // We only suppress narrowly targeted Alt/menu-related messages instead of
    // blocking all WM_SYS* traffic.
    bool suppress = false;
    bool cancelMode = false;

    std::lock_guard<std::mutex> lock(g_stateMutex);

    UINT vk = ResolveVirtualKey(msg->wParam, msg->lParam);
    bool keyDown =
        msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN;
    bool keyUp = msg->message == WM_KEYUP || msg->message == WM_SYSKEYUP;

    bool preAltDown = AnyAltDownLocked();
    bool preCtrlDown = AnyCtrlDownLocked();
    bool preShiftDown = AnyShiftDownLocked();
    bool preWinDown = AnyWinDownLocked();
    bool preAltGr = IsLikelyAltGrStateLocked();
    bool suppressionActive = IsSuppressionActiveLocked(now);

    if (keyDown)
    {
        if (vk == VK_LSHIFT || vk == VK_RSHIFT)
        {
            if (g_settings.suppressLayoutSwitchArtifacts && preAltDown &&
                !preAltGr)
            {
                g_state.altWasUsedForLayoutSwitch = true;
                g_state.lastLayoutComboTick = now;
                Wh_Log(L"Observed Alt+Shift pattern");
            }

            if (g_settings.suppressLayoutSwitchArtifacts && preCtrlDown)
            {
                g_state.lastLayoutComboTick = now;
                Wh_Log(L"Observed Ctrl+Shift pattern");
            }

            SetModifierStateLocked(vk, true);
        }
        else if (vk == VK_LCONTROL || vk == VK_RCONTROL)
        {
            if (g_settings.suppressLayoutSwitchArtifacts && preShiftDown)
            {
                g_state.lastLayoutComboTick = now;
                Wh_Log(L"Observed Ctrl+Shift pattern");
            }

            SetModifierStateLocked(vk, true);
        }
        else if (vk == VK_LMENU || vk == VK_RMENU)
        {
            bool likelyAltGrPress = (vk == VK_RMENU && preCtrlDown);

            if (!preAltDown)
            {
                g_state.altPressTick = now;
                g_state.altChordUsed = false;
                g_state.altWasUsedForLayoutSwitch = false;
            }

            if (g_settings.suppressLayoutSwitchArtifacts && preShiftDown &&
                !likelyAltGrPress)
            {
                g_state.altWasUsedForLayoutSwitch = true;
                g_state.lastLayoutComboTick = now;
                Wh_Log(L"Observed Alt+Shift pattern");
            }

            SetModifierStateLocked(vk, true);

            // A stray Alt message that arrives during the post-layout-switch
            // suppression window is exactly the artifact we're trying to block.
            if (msg->message == WM_SYSKEYDOWN &&
                g_settings.suppressLayoutSwitchArtifacts && suppressionActive &&
                !likelyAltGrPress)
            {
                suppress = true;
                Wh_Log(L"Suppressing WM_SYSKEYDOWN for Alt inside artifact window");
            }
        }
        else if (vk == VK_LWIN || vk == VK_RWIN)
        {
            SetModifierStateLocked(vk, true);
        }
        else
        {
            if (vk == VK_SPACE && preWinDown &&
                g_settings.suppressLayoutSwitchArtifacts)
            {
                g_state.lastWinSpaceTick = now;
                ExtendSuppressionLocked(now,
                                        kLayoutArtifactWindowMs,
                                        L"Win+Space sequence");
                Wh_Log(L"Observed Win+Space pattern");
            }

            // Once a real non-modifier key is used while Alt is down, treat it
            // as a deliberate Alt chord and stop treating the sequence as a
            // plain Alt / layout-switch artifact.
            if (preAltDown && !preAltGr)
            {
                g_state.altChordUsed = true;
                g_state.altWasUsedForLayoutSwitch = false;
            }
        }
    }
    else if (keyUp)
    {
        if (vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_LCONTROL ||
            vk == VK_RCONTROL || vk == VK_LWIN || vk == VK_RWIN)
        {
            SetModifierStateLocked(vk, false);
        }
        else if (vk == VK_LMENU || vk == VK_RMENU)
        {
            bool likelyAltGrRelease = (vk == VK_RMENU && preCtrlDown) || preAltGr;
            bool recentLayoutCombo = HasRecentEvent(now,
                                                    g_state.lastLayoutComboTick,
                                                    kLayoutArtifactWindowMs);
            bool recentLangChange = HasRecentEvent(
                now, g_state.lastInputLangChangeTick, kLayoutArtifactWindowMs);

            if (!likelyAltGrRelease)
            {
                if (g_settings.suppressLayoutSwitchArtifacts &&
                    (g_state.altWasUsedForLayoutSwitch || recentLayoutCombo ||
                     recentLangChange || suppressionActive))
                {
                    suppress = true;
                    cancelMode = true;
                    ExtendSuppressionLocked(now,
                                            kKeyTipFollowupWindowMs,
                                            L"Alt release after layout switch");
                    Wh_Log(L"Suppressing Alt release after layout/lang switch artifact");
                }
                else if (g_settings.suppressSingleAlt &&
                         !g_settings.preserveIntentionalAltNavigation &&
                         !g_state.altChordUsed)
                {
                    suppress = true;
                    cancelMode = true;
                    ExtendSuppressionLocked(now,
                                            kKeyTipFollowupWindowMs,
                                            L"isolated Alt release");
                    Wh_Log(L"Suppressing isolated Alt release");
                }
            }

            SetModifierStateLocked(vk, false);
            if (!AnyAltDownLocked())
            {
                g_state.altChordUsed = false;
                g_state.altWasUsedForLayoutSwitch = false;
            }
        }
    }
    else if ((msg->message == WM_SYSCHAR || msg->message == WM_SYSDEADCHAR) &&
             g_settings.suppressMenuActivationPaths && suppressionActive)
    {
        suppress = true;
        cancelMode = true;
        Wh_Log(L"Suppressing WM_SYSCHAR/WM_SYSDEADCHAR inside artifact window");
    }

    if (cancelMode)
    {
        PostCancelMode(root, L"keyboard suppression");
    }

    return suppress;
}

bool ProcessRootWindowMessage(HWND root,
                              UINT message,
                              WPARAM wParam,
                              LPARAM lParam,
                              DWORD now)
{
    // Some activation paths are delivered directly to the Word frame rather
    // than as queued keyboard messages. The OpusApp subclass closes those gaps
    // by blocking SC_KEYMENU / menu-loop entry when the tracked state says the
    // transition is accidental.
    bool suppress = false;
    bool cancelMode = false;

    std::lock_guard<std::mutex> lock(g_stateMutex);

    switch (message)
    {
        case WM_INPUTLANGCHANGEREQUEST:
        case WM_INPUTLANGCHANGE:
            if (g_settings.suppressLayoutSwitchArtifacts)
            {
                g_state.lastInputLangChangeTick = now;
                ExtendSuppressionLocked(
                    now, kLayoutArtifactWindowMs, L"input language change");
                cancelMode = true;
                Wh_Log(L"Observed input language change message 0x%04X",
                       message);
            }
            break;

        case WM_ACTIVATEAPP:
            if (wParam)
            {
                if (g_settings.suppressLayoutSwitchArtifacts &&
                    (HasRecentEvent(now,
                                    g_state.lastWinSpaceTick,
                                    kLayoutArtifactWindowMs) ||
                     HasRecentEvent(now,
                                    g_state.lastInputLangChangeTick,
                                    kLayoutArtifactWindowMs)))
                {
                    ExtendSuppressionLocked(now,
                                            kFocusRecoveryWindowMs,
                                            L"focus recovery after layout switch");
                    cancelMode = true;
                    Wh_Log(L"Observed focus recovery after layout switch");
                }
            }
            break;

        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == SC_KEYMENU &&
                g_settings.suppressMenuActivationPaths)
            {
                bool suppressionActive = IsSuppressionActiveLocked(now);
                bool recentPlainAlt =
                    HasRecentEvent(now, g_state.altPressTick, kRecentAltWindowMs) &&
                    !g_state.altChordUsed &&
                    !IsLikelyAltGrStateLocked();

                if (suppressionActive ||
                    (g_settings.suppressSingleAlt &&
                     !g_settings.preserveIntentionalAltNavigation &&
                     recentPlainAlt))
                {
                    suppress = true;
                    cancelMode = true;
                    ExtendSuppressionLocked(now,
                                            kKeyTipFollowupWindowMs,
                                            L"SC_KEYMENU");
                    Wh_Log(L"Suppressing SC_KEYMENU");
                }
            }
            break;

        case WM_ENTERMENULOOP:
        {
            bool recentPlainAlt =
                HasRecentEvent(now, g_state.altPressTick, kRecentAltWindowMs) &&
                !g_state.altChordUsed && !IsLikelyAltGrStateLocked();

            if (g_settings.suppressMenuActivationPaths &&
                (IsSuppressionActiveLocked(now) ||
                 (g_settings.suppressSingleAlt &&
                  !g_settings.preserveIntentionalAltNavigation &&
                  recentPlainAlt)))
            {
                suppress = true;
                cancelMode = true;
                Wh_Log(L"Suppressing WM_ENTERMENULOOP");
            }
            break;
        }
    }

    if (cancelMode)
    {
        PostCancelMode(root, L"root message");
    }

    return suppress;
}

LRESULT CALLBACK WordFrameSubclassProc(HWND hWnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       DWORD_PTR dwRefData)
{
    UNREFERENCED_PARAMETER(dwRefData);

    switch (uMsg)
    {
        case WM_INPUTLANGCHANGEREQUEST:
        case WM_INPUTLANGCHANGE:
        case WM_ACTIVATEAPP:
        case WM_SYSCOMMAND:
        case WM_ENTERMENULOOP:
        {
            if (ProcessRootWindowMessage(hWnd,
                                         uMsg,
                                         wParam,
                                         lParam,
                                         GetTickCount()))
            {
                return 0;
            }
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void MaybeSubclassWordWindow(HWND hWnd)
{
    if (!g_initializedForWord || !IsWordFrameWindow(hWnd))
    {
        return;
    }

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                                                      WordFrameSubclassProc,
                                                      0))
    {
        Wh_Log(L"Subclassed Word frame window %p", hWnd);
    }
    else
    {
        Wh_Log(L"Failed to subclass Word frame window %p", hWnd);
    }
}

BOOL CALLBACK EnumWordWindowsProc(HWND hWnd, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    MaybeSubclassWordWindow(hWnd);
    return TRUE;
}

void SubclassExistingWordWindows()
{
    EnumWindows(EnumWordWindowsProc, 0);
}

BOOL CALLBACK RemoveWordSubclassProc(HWND hWnd, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    if (IsWordFrameWindow(hWnd))
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                         WordFrameSubclassProc);
    }

    return TRUE;
}

void RemoveExistingWordSubclasses()
{
    EnumWindows(RemoveWordSubclassProc, 0);
}

LRESULT WINAPI DispatchMessageW_hook(const MSG* lpMsg)
{
    if (!lpMsg || !g_initializedForWord)
    {
        return DispatchMessageW_orig(lpMsg);
    }

    switch (lpMsg->message)
    {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_SYSCHAR:
        case WM_SYSDEADCHAR:
            break;

        default:
            return DispatchMessageW_orig(lpMsg);
    }

    HWND root = GetWordRootWindow(lpMsg->hwnd);
    if (!root)
    {
        return DispatchMessageW_orig(lpMsg);
    }

    if (ProcessQueuedKeyboardMessage(root, lpMsg, lpMsg->time))
    {
        return 0;
    }

    return DispatchMessageW_orig(lpMsg);
}

HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam)
{
    HWND hWnd = CreateWindowExW_orig(dwExStyle,
                                     lpClassName,
                                     lpWindowName,
                                     dwStyle,
                                     X,
                                     Y,
                                     nWidth,
                                     nHeight,
                                     hWndParent,
                                     hMenu,
                                     hInstance,
                                     lpParam);

    if (hWnd)
    {
        MaybeSubclassWordWindow(hWnd);
    }

    return hWnd;
}

void ResetState()
{
    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_state = {};
}
}  // namespace

BOOL Wh_ModInit()
{
    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(DispatchMessageW,
                                        DispatchMessageW_hook,
                                        &DispatchMessageW_orig))
    {
        Wh_Log(L"Failed to hook DispatchMessageW");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW,
                                        CreateWindowExW_hook,
                                        &CreateWindowExW_orig))
    {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    g_initializedForWord = true;
    ResetState();
    SubclassExistingWordWindows();

    Wh_Log(L"Mod initialized");
    return TRUE;
}

void Wh_ModAfterInit()
{
    if (!g_initializedForWord)
    {
        return;
    }

    SubclassExistingWordWindows();
}

void Wh_ModSettingsChanged()
{
    LoadSettings();

    if (!g_initializedForWord)
    {
        return;
    }

    ResetState();
    SubclassExistingWordWindows();
    Wh_Log(L"Settings reloaded");
}

void Wh_ModBeforeUninit()
{
    if (!g_initializedForWord)
    {
        return;
    }

    RemoveExistingWordSubclasses();
    g_initializedForWord = false;
}

void Wh_ModUninit()
{
    ResetState();
}
