// ==WindhawkMod==
// @id              en-ar-tur-keyboard-layout-switcher
// @name            EN / AR / TUR Keyboard Layout Switcher
// @description     Alt+Shift toggles EN/AR on release; Alt+Shift+Grave switches directly to Turkish.
// @version         1.0
// @author          Ahmad Mozaik
// @github          https://github.com/ahmadmozaik
// @include         explorer.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# EN / AR / TUR Keyboard Layout Switcher

IMPORTANT:
Windows' built-in Alt+Shift input-language shortcut must be set to
"Not Assigned".

Shortcuts:

    Alt + Shift

        Release Alt+Shift without pressing another key:

            English (US)          -> Arabic (Saudi Arabia)
            Arabic (Saudi Arabia) -> English (US)
            Turkish               -> English (US)
            Anything else         -> English (US)

    Alt + Shift + `

        Switch directly to Turkish.

The grave key is the key immediately below Escape.

The Alt+Shift action is triggered on RELEASE, not key-down.

The Alt+Shift action is cancelled if any other key is pressed while
Alt+Shift is being held.

Either left or right Alt can be used.
Either left or right Shift can be used.

The mod does NOT swallow Alt or Shift key events. This is deliberate:
it prevents stuck Alt/Shift states if the mod is unloaded.

The grave key is swallowed only when it is actually recognized as
Alt+Shift+Grave.

Verified layout IDs:

    English (United States) = 00000409
    Arabic (Saudi Arabia)   = 00000401
    Turkish                = 0000041F
*/
// ==/WindhawkModReadme==

#include <windows.h>

// ============================================================
// Layout IDs
// ============================================================

static constexpr wchar_t kEnglishKleid[] = L"00000409";
static constexpr wchar_t kArabicKleid[]  = L"00000401";
static constexpr wchar_t kTurkishKleid[] = L"0000041F";

// Language IDs contained in the low word of HKL.
static constexpr LANGID kEnglishLangId = 0x0409;
static constexpr LANGID kArabicLangId  = 0x0401;
static constexpr LANGID kTurkishLangId = 0x041F;

// Grave / backtick key.
static constexpr UINT kGraveVk = VK_OEM_3;

// Custom window message.
static constexpr UINT kSwitchMessage = WM_APP + 0x431;

// Single-instance mutex.
static constexpr wchar_t kMutexName[] =
    L"Local\\Windhawk_EN_AR_TUR_KeyboardSwitcher";

// Window class name.
static constexpr wchar_t kWindowClassName[] =
    L"Windhawk_EN_AR_TUR_KeyboardSwitcher_Window";

// ============================================================
// Global state
// ============================================================

static HANDLE g_mutex = nullptr;

static HANDLE g_workerThread = nullptr;

static HANDLE g_readyEvent = nullptr;

static DWORD g_workerThreadId = 0;

static bool g_workerInitialized = false;

static HHOOK g_keyboardHook = nullptr;

static HWND g_messageWindow = nullptr;

static HMODULE g_module = nullptr;

static ATOM g_windowClass = 0;

static HKL g_englishHkl = nullptr;
static HKL g_arabicHkl = nullptr;
static HKL g_turkishHkl = nullptr;

// ============================================================
// Shortcut state
// ============================================================

struct ShortcutState
{
    bool leftAltDown = false;
    bool rightAltDown = false;

    bool leftShiftDown = false;
    bool rightShiftDown = false;

    bool leftCtrlDown = false;
    bool rightCtrlDown = false;

    // True after Alt+Shift are both down.
    bool candidate = false;

    // Some other key was pressed while the candidate was active.
    bool otherKeyPressed = false;

    // Turkish shortcut has been recognized.
    bool turkishShortcut = false;

    // Grave is physically held.
    bool graveDown = false;

    // Ctrl generated as part of AltGr / Right Alt.
    bool altGrCtrl = false;
};

static ShortcutState g_state;

// ============================================================
// Helpers
// ============================================================

static bool IsAltVk(UINT vk)
{
    return vk == VK_LMENU || vk == VK_RMENU;
}

static bool IsShiftVk(UINT vk)
{
    return vk == VK_LSHIFT || vk == VK_RSHIFT;
}

static bool IsCtrlVk(UINT vk)
{
    return vk == VK_LCONTROL || vk == VK_RCONTROL;
}

static bool IsAltDown()
{
    return g_state.leftAltDown ||
           g_state.rightAltDown;
}

static bool IsShiftDown()
{
    return g_state.leftShiftDown ||
           g_state.rightShiftDown;
}

static bool IsCtrlDown()
{
    return g_state.leftCtrlDown ||
           g_state.rightCtrlDown;
}

static bool IsAltShiftDown()
{
    return IsAltDown() && IsShiftDown();
}

static void ResetShortcutState()
{
    g_state = {};
}

static void StartShortcutCandidate()
{
    // Windows can synthesize Left Ctrl before Right Alt for AltGr. Preserve
    // support for Right Alt while still rejecting an unrelated Ctrl key.
    g_state.altGrCtrl =
        g_state.rightAltDown &&
        g_state.leftCtrlDown &&
        !g_state.rightCtrlDown;

    g_state.candidate = true;
    g_state.otherKeyPressed =
        IsCtrlDown() && !g_state.altGrCtrl;
    g_state.turkishShortcut = false;
    g_state.graveDown = false;

    Wh_Log(
        L"Alt+Shift candidate started.");
}

// ============================================================
// Modifier tracking
// ============================================================

static void SetModifierDown(UINT vk)
{
    switch (vk)
    {
        case VK_LMENU:
            g_state.leftAltDown = true;
            break;

        case VK_RMENU:
            g_state.rightAltDown = true;
            break;

        case VK_LSHIFT:
            g_state.leftShiftDown = true;
            break;

        case VK_RSHIFT:
            g_state.rightShiftDown = true;
            break;

        case VK_LCONTROL:
            g_state.leftCtrlDown = true;
            break;

        case VK_RCONTROL:
            g_state.rightCtrlDown = true;
            break;
    }
}

static void SetModifierUp(UINT vk)
{
    switch (vk)
    {
        case VK_LMENU:
            g_state.leftAltDown = false;
            break;

        case VK_RMENU:
            g_state.rightAltDown = false;
            break;

        case VK_LSHIFT:
            g_state.leftShiftDown = false;
            break;

        case VK_RSHIFT:
            g_state.rightShiftDown = false;
            break;

        case VK_LCONTROL:
            g_state.leftCtrlDown = false;
            break;

        case VK_RCONTROL:
            g_state.rightCtrlDown = false;
            break;
    }
}

// ============================================================
// Current layout
// ============================================================

static HKL GetForegroundKeyboardLayout()
{
    HWND foreground =
        GetForegroundWindow();

    if (!foreground)
        return nullptr;

    DWORD threadId =
        GetWindowThreadProcessId(
            foreground,
            nullptr);

    if (!threadId)
        return nullptr;

    /*
     * GetKeyboardLayout() works on the GUI/input thread that
     * owns the foreground window.
     */
    HKL hkl =
        GetKeyboardLayout(threadId);

    if (hkl)
    {
        return hkl;
    }

    /*
     * If the foreground thread has no keyboard layout available,
     * fall back to the current keyboard layout.
     */
    return GetKeyboardLayout(0);
}

// ============================================================
// Layout switching
// ============================================================

static bool RequestKeyboardLayout(HWND hwnd, HKL hkl, const wchar_t* name)
{
    if (!hwnd || !hkl)
        return false;

    DWORD_PTR result = 0;

    LRESULT messageResult =
        SendMessageTimeoutW(
            hwnd,
            WM_INPUTLANGCHANGEREQUEST,
            0,
            reinterpret_cast<LPARAM>(hkl),
            SMTO_ABORTIFHUNG | SMTO_BLOCK,
            500,
            &result);

    if (!messageResult)
    {
        Wh_Log(
            L"WM_INPUTLANGCHANGEREQUEST to 0x%p for %ls failed/timed out. Error=%lu",
            hwnd,
            name,
            GetLastError());

        return false;
    }

    Wh_Log(
        L"WM_INPUTLANGCHANGEREQUEST sent to 0x%p for %ls.",
        hwnd,
        name);

    return true;
}


static void SwitchToLayout(HKL hkl, const wchar_t* name)
{
    if (!hkl)
    {
        Wh_Log(
            L"Cannot switch to %ls: HKL is null.",
            name);

        return;
    }

    HWND foreground =
        GetForegroundWindow();

    if (!foreground)
    {
        Wh_Log(
            L"Cannot switch to %ls: no foreground window.",
            name);

        return;
    }

    /*
     * --------------------------------------------------------
     * Find the actual keyboard-focus window belonging to the
     * foreground GUI thread.
     *
     * This is important because GetFocus() cannot be used from
     * our Explorer thread to retrieve another application's
     * focus window.
     * --------------------------------------------------------
     */

    DWORD foregroundThreadId =
        GetWindowThreadProcessId(
            foreground,
            nullptr);

    HWND focusWindow = nullptr;

    if (foregroundThreadId)
    {
        GUITHREADINFO guiInfo{};

        guiInfo.cbSize =
            sizeof(guiInfo);

        if (GetGUIThreadInfo(
                foregroundThreadId,
                &guiInfo))
        {
            focusWindow =
                guiInfo.hwndFocus;
        }
    }

    /*
     * --------------------------------------------------------
     * First try the actual focused control.
     *
     * This is the normal Windows input-language path.
     * --------------------------------------------------------
     */

    if (focusWindow)
    {
        Wh_Log(
            L"Requesting %ls from focused window 0x%p "
            L"(foreground 0x%p).",
            name,
            focusWindow,
            foreground);

        if (RequestKeyboardLayout(
                focusWindow,
                hkl,
                name))
        {
            return;
        }
    }

    /*
     * --------------------------------------------------------
     * Some applications don't expose a normal focus child.
     *
     * In that case send the request directly to the foreground
     * top-level window.
     * --------------------------------------------------------
     */

    if (foreground != focusWindow)
    {
        Wh_Log(
            L"Trying foreground window 0x%p for %ls.",
            foreground,
            name);

        if (RequestKeyboardLayout(
                foreground,
                hkl,
                name))
        {
            return;
        }
    }

    /*
     * --------------------------------------------------------
     * Last fallback:
     *
     * If there is no focus/control window, there is still a
     * foreground application. Ask its top-level window to
     * process the request.
     * --------------------------------------------------------
     */

    Wh_Log(
        L"Could not confirm layout request for %ls.",
        name);
}

static void SwitchToEnglish()
{
    SwitchToLayout(
        g_englishHkl,
        L"English (US)");
}

static void SwitchToArabic()
{
    SwitchToLayout(
        g_arabicHkl,
        L"Arabic (Saudi Arabia)");
}

static void SwitchToTurkish()
{
    SwitchToLayout(
        g_turkishHkl,
        L"Turkish");
}

// ============================================================
// EN / AR toggle
// ============================================================

static void ToggleEnglishArabic()
{
    HKL current =
        GetForegroundKeyboardLayout();

    if (!current)
    {
        Wh_Log(
            L"Could not determine foreground keyboard layout.");

        SwitchToEnglish();
        return;
    }

    LANGID languageId =
        LOWORD(reinterpret_cast<ULONG_PTR>(current));

    Wh_Log(
        L"Current layout LANGID = 0x%04X",
        languageId);

    switch (languageId)
    {
        case kEnglishLangId:
            /*
             * EN -> AR
             */
            SwitchToArabic();
            break;

        case kArabicLangId:
            /*
             * AR -> EN
             */
            SwitchToEnglish();
            break;

        case kTurkishLangId:
            /*
             * TUR -> EN
             */
            SwitchToEnglish();
            break;

        default:
            /*
             * Anything else -> EN.
             */
            SwitchToEnglish();
            break;
    }
}

// ============================================================
// Message-only window
// ============================================================

static LRESULT CALLBACK MessageWindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
        case kSwitchMessage:
        {
            switch (wParam)
            {
                case 1:
                    ToggleEnglishArabic();
                    break;

                case 2:
                    SwitchToTurkish();
                    break;
            }

            return 0;
        }

        default:
            break;
    }

    return DefWindowProcW(
        hwnd,
        message,
        wParam,
        lParam);
}

static bool CreateMessageWindow()
{
    WNDCLASSEXW wc{};

    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = MessageWindowProc;
    wc.hInstance = g_module;
    wc.lpszClassName = kWindowClassName;

    g_windowClass =
        RegisterClassExW(&wc);

    if (!g_windowClass)
    {
        DWORD error = GetLastError();

        if (error != ERROR_CLASS_ALREADY_EXISTS)
        {
            Wh_Log(
                L"RegisterClassExW failed: %lu",
                error);

            return false;
        }
    }

    g_messageWindow =
        CreateWindowExW(
            0,
            kWindowClassName,
            L"",
            0,
            0,
            0,
            0,
            0,
            HWND_MESSAGE,
            nullptr,
            g_module,
            nullptr);

    if (!g_messageWindow)
    {
        Wh_Log(
            L"CreateWindowExW failed: %lu",
            GetLastError());

        if (g_windowClass)
        {
            UnregisterClassW(
                kWindowClassName,
                g_module);

            g_windowClass = 0;
        }

        return false;
    }

    return true;
}

static void DestroyMessageWindow()
{
    if (g_messageWindow)
    {
        DestroyWindow(g_messageWindow);
        g_messageWindow = nullptr;
    }

    if (g_windowClass)
    {
        UnregisterClassW(
            kWindowClassName,
            g_module);

        g_windowClass = 0;
    }
}

// ============================================================
// Keyboard hook
// ============================================================

static LRESULT CALLBACK LowLevelKeyboardProc(
    int nCode,
    WPARAM wParam,
    LPARAM lParam)
{
    if (nCode != HC_ACTION)
    {
        return CallNextHookEx(
            g_keyboardHook,
            nCode,
            wParam,
            lParam);
    }

    const KBDLLHOOKSTRUCT* key =
        reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);

    if (!key)
    {
        return CallNextHookEx(
            g_keyboardHook,
            nCode,
            wParam,
            lParam);
    }

    /*
     * Ignore injected keyboard events.
     */
    if (key->flags & LLKHF_INJECTED)
    {
        return CallNextHookEx(
            g_keyboardHook,
            nCode,
            wParam,
            lParam);
    }

    UINT vk = key->vkCode;

    bool keyDown =
        wParam == WM_KEYDOWN ||
        wParam == WM_SYSKEYDOWN;

    bool keyUp =
        wParam == WM_KEYUP ||
        wParam == WM_SYSKEYUP;

    // ========================================================
    // KEY DOWN
    // ========================================================

    if (keyDown)
    {
        // ----------------------------------------------------
        // ALT
        // ----------------------------------------------------

        if (IsAltVk(vk))
        {
            bool wasDown =
                (vk == VK_LMENU)
                    ? g_state.leftAltDown
                    : g_state.rightAltDown;

            SetModifierDown(vk);

            if (!wasDown &&
                IsAltShiftDown())
            {
                /*
                 * Start a possible Alt+Shift gesture.
                 */
                StartShortcutCandidate();
            }

            /*
             * NEVER swallow Alt.
             *
             * This is critical for preventing stuck Alt.
             */
            return CallNextHookEx(
                g_keyboardHook,
                nCode,
                wParam,
                lParam);
        }

        // ----------------------------------------------------
        // SHIFT
        // ----------------------------------------------------

        if (IsShiftVk(vk))
        {
            bool wasDown =
                (vk == VK_LSHIFT)
                    ? g_state.leftShiftDown
                    : g_state.rightShiftDown;

            SetModifierDown(vk);

            if (!wasDown &&
                IsAltShiftDown())
            {
                /*
                 * Start a possible Alt+Shift gesture.
                 */
                StartShortcutCandidate();
            }

            /*
             * NEVER swallow Shift.
             */
            return CallNextHookEx(
                g_keyboardHook,
                nCode,
                wParam,
                lParam);
        }

        // ----------------------------------------------------
        // CTRL
        // ----------------------------------------------------

        if (IsCtrlVk(vk))
        {
            bool wasDown =
                (vk == VK_LCONTROL)
                    ? g_state.leftCtrlDown
                    : g_state.rightCtrlDown;

            SetModifierDown(vk);

            if (g_state.candidate && !wasDown)
            {
                /*
                 * Right Alt can produce an associated Ctrl event
                 * as part of AltGr behavior.
                 */
                if (vk == VK_LCONTROL &&
                    g_state.rightAltDown)
                {
                    g_state.altGrCtrl = true;
                }
                else
                {
                    g_state.otherKeyPressed = true;
                }
            }

            return CallNextHookEx(
                g_keyboardHook,
                nCode,
                wParam,
                lParam);
        }

        // ----------------------------------------------------
        // GRAVE
        // ----------------------------------------------------

        if (vk == kGraveVk)
        {
            if (g_state.graveDown)
            {
                return CallNextHookEx(
                    g_keyboardHook,
                    nCode,
                    wParam,
                    lParam);
            }

            g_state.graveDown = true;

            /*
             * Turkish requires:
             *
             *     Alt down
             *     Shift down
             *     Grave down
             *
             * with no other key having been pressed.
             */
            if (g_state.candidate &&
                IsAltShiftDown() &&
                !g_state.otherKeyPressed)
            {
                g_state.turkishShortcut = true;

                /*
                 * Queue the action to our message window.
                 *
                 * The hook itself remains extremely short.
                 */
                if (g_messageWindow)
                {
                    PostMessageW(
                        g_messageWindow,
                        kSwitchMessage,
                        2,
                        0);
                }

                /*
                 * Swallow only the grave key.
                 */
                return 1;
            }

            if (g_state.candidate)
            {
                g_state.otherKeyPressed = true;
            }

            return CallNextHookEx(
                g_keyboardHook,
                nCode,
                wParam,
                lParam);
        }

        // ----------------------------------------------------
        // ANY OTHER KEY
        // ----------------------------------------------------

        if (g_state.candidate)
        {
            /*
             * Any other key invalidates the plain Alt+Shift
             * toggle.
             */
            g_state.otherKeyPressed = true;

            Wh_Log(
                L"Alt+Shift candidate cancelled by VK=0x%02X",
                vk);
        }

        return CallNextHookEx(
            g_keyboardHook,
            nCode,
            wParam,
            lParam);
    }

    // ========================================================
    // KEY UP
    // ========================================================

    if (keyUp)
    {
        // ----------------------------------------------------
        // GRAVE RELEASE
        // ----------------------------------------------------

        if (vk == kGraveVk)
        {
            g_state.graveDown = false;

            if (g_state.turkishShortcut)
            {
                /*
                 * Swallow the release corresponding to the
                 * Turkish shortcut.
                 */
                return 1;
            }

            return CallNextHookEx(
                g_keyboardHook,
                nCode,
                wParam,
                lParam);
        }

        // ----------------------------------------------------
        // CTRL RELEASE
        // ----------------------------------------------------

        if (IsCtrlVk(vk))
        {
            bool altGrCtrl =
                g_state.altGrCtrl &&
                vk == VK_LCONTROL &&
                g_state.rightAltDown;

            SetModifierUp(vk);

            if (g_state.candidate &&
                !altGrCtrl)
            {
                g_state.otherKeyPressed = true;
            }

            return CallNextHookEx(
                g_keyboardHook,
                nCode,
                wParam,
                lParam);
        }

        // ----------------------------------------------------
        // ALT / SHIFT RELEASE
        // ----------------------------------------------------

        if (IsAltVk(vk) || IsShiftVk(vk))
        {
            /*
             * Determine whether this was the FINAL Alt/Shift
             * release BEFORE updating the state.
             */
            bool wasAltShiftDown =
                IsAltShiftDown();

            bool validToggle =
                g_state.candidate &&
                !g_state.otherKeyPressed &&
                !g_state.turkishShortcut;

            SetModifierUp(vk);

            bool stillAltShiftDown =
                IsAltShiftDown();

            /*
             * This is the exact trigger requested:
             *
             * the last Alt/Shift key was released,
             * and no other key was pressed.
             */
            if (wasAltShiftDown &&
                !stillAltShiftDown &&
                validToggle)
            {
                Wh_Log(
                    L"Alt+Shift released: toggling EN/AR.");

                if (g_messageWindow)
                {
                    PostMessageW(
                        g_messageWindow,
                        kSwitchMessage,
                        1,
                        0);
                }
            }

            /*
             * Clear the gesture after both modifiers are released.
             */
            if (!IsAltDown() &&
                !IsShiftDown())
            {
                ResetShortcutState();
            }

            /*
             * NEVER swallow Alt or Shift release.
             *
             * This is the important safety property of this
             * version.
             */
            return CallNextHookEx(
                g_keyboardHook,
                nCode,
                wParam,
                lParam);
        }

        return CallNextHookEx(
            g_keyboardHook,
            nCode,
            wParam,
            lParam);
    }

    return CallNextHookEx(
        g_keyboardHook,
        nCode,
        wParam,
        lParam);
}

// ============================================================
// Windhawk initialization
// ============================================================

static void CleanupWorkerResources()
{
    if (g_keyboardHook)
    {
        if (!UnhookWindowsHookEx(g_keyboardHook))
        {
            Wh_Log(
                L"UnhookWindowsHookEx failed: %lu",
                GetLastError());
        }

        g_keyboardHook = nullptr;
    }

    DestroyMessageWindow();

    if (g_mutex)
    {
        CloseHandle(g_mutex);
        g_mutex = nullptr;
    }

    // The loaded layouts can be shared by the user's input system, so don't
    // unload them when the worker stops.
    g_englishHkl = nullptr;
    g_arabicHkl = nullptr;
    g_turkishHkl = nullptr;

    ResetShortcutState();
}

static DWORD WINAPI KeyboardWorkerThread(void*)
{
    g_mutex =
        CreateMutexW(
            nullptr,
            FALSE,
            kMutexName);

    if (!g_mutex)
    {
        Wh_Log(
            L"CreateMutexW failed: %lu",
            GetLastError());

        SetEvent(g_readyEvent);
        return 0;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        Wh_Log(
            L"Another Explorer instance already owns the keyboard hook.");

        SetEvent(g_readyEvent);
        CleanupWorkerResources();
        return 0;
    }

    g_englishHkl =
        LoadKeyboardLayoutW(
            kEnglishKleid,
            KLF_NOTELLSHELL);

    g_arabicHkl =
        LoadKeyboardLayoutW(
            kArabicKleid,
            KLF_NOTELLSHELL);

    g_turkishHkl =
        LoadKeyboardLayoutW(
            kTurkishKleid,
            KLF_NOTELLSHELL);

    if (!g_englishHkl)
    {
        Wh_Log(
            L"English layout %ls could not be loaded. Error=%lu",
            kEnglishKleid,
            GetLastError());
    }

    if (!g_arabicHkl)
    {
        Wh_Log(
            L"Arabic layout %ls could not be loaded. Error=%lu",
            kArabicKleid,
            GetLastError());
    }

    if (!g_turkishHkl)
    {
        Wh_Log(
            L"Turkish layout %ls could not be loaded. Error=%lu",
            kTurkishKleid,
            GetLastError());
    }

    if (!g_englishHkl ||
        !g_arabicHkl ||
        !g_turkishHkl)
    {
        Wh_Log(
            L"One or more requested layouts are unavailable.");

        SetEvent(g_readyEvent);
        CleanupWorkerResources();
        return 0;
    }

    Wh_Log(
        L"English HKL = 0x%p",
        g_englishHkl);

    Wh_Log(
        L"Arabic HKL = 0x%p",
        g_arabicHkl);

    Wh_Log(
        L"Turkish HKL = 0x%p",
        g_turkishHkl);

    if (!CreateMessageWindow())
    {
        Wh_Log(
            L"Could not create message window.");

        SetEvent(g_readyEvent);
        CleanupWorkerResources();
        return 0;
    }

    g_keyboardHook =
        SetWindowsHookExW(
            WH_KEYBOARD_LL,
            LowLevelKeyboardProc,
            g_module,
            0);

    if (!g_keyboardHook)
    {
        Wh_Log(
            L"SetWindowsHookExW failed: %lu",
            GetLastError());

        SetEvent(g_readyEvent);
        CleanupWorkerResources();
        return 0;
    }

    g_workerInitialized = true;
    SetEvent(g_readyEvent);

    Wh_Log(
        L"EN/AR/TUR keyboard layout switcher is active.");

    MSG message;
    int result;

    while ((result = GetMessageW(
                &message,
                nullptr,
                0,
                0)) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (result == -1)
    {
        Wh_Log(
            L"GetMessageW failed: %lu",
            GetLastError());
    }

    CleanupWorkerResources();
    return 0;
}

BOOL Wh_ModInit()
{
    Wh_Log(
        L"EN/AR/TUR keyboard layout switcher starting.");

    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&Wh_ModInit),
            &g_module))
    {
        Wh_Log(
            L"GetModuleHandleExW failed: %lu",
            GetLastError());

        return FALSE;
    }

    g_readyEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr);

    if (!g_readyEvent)
    {
        Wh_Log(
            L"CreateEventW failed: %lu",
            GetLastError());

        return FALSE;
    }

    g_workerThread =
        CreateThread(
            nullptr,
            0,
            KeyboardWorkerThread,
            nullptr,
            0,
            &g_workerThreadId);

    if (!g_workerThread)
    {
        Wh_Log(
            L"CreateThread failed: %lu",
            GetLastError());

        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(
        g_readyEvent,
        INFINITE);

    if (!g_workerInitialized)
    {
        WaitForSingleObject(
            g_workerThread,
            INFINITE);

        CloseHandle(g_workerThread);
        CloseHandle(g_readyEvent);
        g_workerThread = nullptr;
        g_readyEvent = nullptr;
        g_workerThreadId = 0;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(
        L"EN/AR/TUR keyboard layout switcher stopping.");

    if (g_workerThread)
    {
        if (!PostThreadMessageW(
                g_workerThreadId,
                WM_QUIT,
                0,
                0))
        {
            Wh_Log(
                L"PostThreadMessageW failed: %lu",
                GetLastError());
        }

        WaitForSingleObject(
            g_workerThread,
            INFINITE);

        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_readyEvent)
    {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }

    g_workerThreadId = 0;
    g_workerInitialized = false;

    Wh_Log(
        L"EN/AR/TUR keyboard layout switcher stopped.");
}

