// ==WindhawkMod==
// @id              hide-virtual-desktop-confirmator
// @name            Hide Virtual Desktop Switcher OSD
// @description     Hides the "Desktop N" popup on virtual desktop switch; keeps the slide animation; volume and brightness flyouts keep working
// @version         1.0
// @author          Zygaross
// @github          https://github.com/Zygaross
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Virtual Desktop Switcher OSD

Hides the "Desktop N" pill that Windows 11 pops up when you switch virtual
desktops (trackpad swipe or Win+Ctrl+Arrow), while keeping the slide
animation. Volume, brightness, and the other hardware flyouts are unaffected.

## How it works

Windows 11 renders the desktop-switch OSD in the taskbar's shared confirmator
host (`HWConfirmatorUI`), the same surface used by the volume and brightness
flyouts. Hiding it after it appears means racing the compositor and juggling
the shared container.

This mod prevents the OSD from being requested instead:

- Hooks the desktop-switch entry points in `twinui.pcshell.dll`
  (`CVirtualDesktopManager::SwitchDesktop` / `SwitchDesktopWithAnimation`,
  `CVirtualDesktopHotkeyHandler::_CycleInDirection`) to arm a short
  suppression window around each switch.
- Hooks the desktop-only text-confirmator entry points in
  `Windows.Internal.HardwareConfirmator.dll`
  (`HardwareConfirmatorHost::ShowTextAsync`,
  `ConfirmatorHostControl::ShowText`) and swallows the call inside that
  window.

The OSD element is never created: no flash, no layout work, and the shared
flyout host is never touched, so volume and brightness OSDs keep working by
construction.

## Notes

- Windows 11 only. Developed and tested on 24H2/25H2-class builds that use
  `HWConfirmatorUI`.
- Only the pill is suppressed — desktop switching and its animation are
  untouched.
- No XAML Diagnostics TAP; compatible with Windows 11 Taskbar Styler.
- If a Windows update renames the hooked symbols, the mod logs the failure
  and does nothing (the OSD reappears; nothing breaks).
*/
// ==/WindhawkModReadme==

// Copyright (C) 2026 Zygaross. This mod is published under the
// GNU General Public License v3.0.

#include <windhawk_utils.h>

#include <winstring.h>

#include <atomic>
#include <mutex>

// The switch hooks arm this window; the ShowText hooks suppress inside it.
// The OSD request lands a few hundred ms after the switch call on test
// hardware; 2000 ms adds headroom without becoming an ambient gate.
constexpr ULONGLONG kSuppressWindowMs = 2000;

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_switchHooksInstalled{false};
std::atomic<bool> g_osdHooksInstalled{false};
std::atomic<ULONGLONG> g_suppressDeadline{0};

// Serializes hook installation: Wh_ModInit (loader thread), Wh_ModAfterInit,
// and the retry thread can all race to install the same hooks, and on a cold
// symbol cache HookSymbols can outlast the retry thread's first sleep. Holding
// this across each installer's check-resolve-apply-set makes it atomic and
// prevents concurrent Wh_ApplyHookOperations.
std::mutex g_installMutex;
HANDLE g_retryThread = nullptr;

static void ArmSuppressionWindow()
{
    if (g_unloading.load(std::memory_order_relaxed))
    {
        return;
    }

    const ULONGLONG deadline = GetTickCount64() + kSuppressWindowMs;
    ULONGLONG expected = g_suppressDeadline.load(std::memory_order_relaxed);
    while (expected < deadline &&
           !g_suppressDeadline.compare_exchange_weak(expected, deadline,
                                                     std::memory_order_relaxed))
    {
    }

    Wh_Log(L"Suppression window armed");
}

static bool IsSuppressionWindowActive()
{
    return !g_unloading.load(std::memory_order_relaxed) &&
           GetTickCount64() < g_suppressDeadline.load(std::memory_order_relaxed);
}

static PCWSTR SafeHstringBuffer(HSTRING text)
{
    PCWSTR buffer = WindowsGetStringRawBuffer(text, nullptr);
    return buffer ? buffer : L"";
}

// ==== Desktop switch triggers (twinui.pcshell.dll) ====

struct IVirtualDesktop;

using CVirtualDesktopManager_SwitchDesktop_t =
    HRESULT(WINAPI*)(void* pThis, IVirtualDesktop* desktop);
CVirtualDesktopManager_SwitchDesktop_t CVirtualDesktopManager_SwitchDesktop_Original =
    nullptr;

static HRESULT WINAPI CVirtualDesktopManager_SwitchDesktop_Hook(void* pThis,
                                                                IVirtualDesktop* desktop)
{
    ArmSuppressionWindow();
    return CVirtualDesktopManager_SwitchDesktop_Original(pThis, desktop);
}

using CVirtualDesktopManager_SwitchDesktopWithAnimation_t =
    HRESULT(WINAPI*)(void* pThis, IVirtualDesktop* desktop);
CVirtualDesktopManager_SwitchDesktopWithAnimation_t
    CVirtualDesktopManager_SwitchDesktopWithAnimation_Original = nullptr;

static HRESULT WINAPI CVirtualDesktopManager_SwitchDesktopWithAnimation_Hook(
    void* pThis,
    IVirtualDesktop* desktop)
{
    ArmSuppressionWindow();
    return CVirtualDesktopManager_SwitchDesktopWithAnimation_Original(pThis, desktop);
}

using CVirtualDesktopHotkeyHandler_CycleInDirection_t =
    HRESULT(WINAPI*)(void* pThis, int direction);
CVirtualDesktopHotkeyHandler_CycleInDirection_t
    CVirtualDesktopHotkeyHandler_CycleInDirection_Original = nullptr;

static HRESULT WINAPI CVirtualDesktopHotkeyHandler_CycleInDirection_Hook(void* pThis,
                                                                         int direction)
{
    ArmSuppressionWindow();
    return CVirtualDesktopHotkeyHandler_CycleInDirection_Original(pThis, direction);
}

// ==== OSD suppression (Windows.Internal.HardwareConfirmator.dll) ====
//
// The desktop-number pill is the text-confirmator path:
//   HardwareConfirmatorHost::ShowTextAsync
//     -> HWConfirmator::ShowText -> ConfirmatorHostControl::ShowText
// Volume, brightness, microphone, and airplane-mode OSDs enter through
// separate Show* functions and never reach these hooks.

// ShowTextAsync returns winrt::fire_and_forget by value. Under the MS x64 ABI
// this UDT is returned via a hidden caller-allocated buffer whose pointer is
// passed as an implicit first argument (RCX), before `this` (RDX); the callee
// must return that same pointer in RAX. So the register order is:
//   RCX=retBuf, RDX=this, R8=hstring (by value), R9=bool.
// Omitting retBuf shifts every argument one register (verified on 26200: the
// bool then reads the hstring pointer's low byte as garbage).
using HardwareConfirmatorHost_ShowTextAsync_t = void*(WINAPI*)(void* retBuf,
                                                               void* pThis,
                                                               HSTRING text,
                                                               bool flag);
HardwareConfirmatorHost_ShowTextAsync_t HardwareConfirmatorHost_ShowTextAsync_Original =
    nullptr;

static void* WINAPI HardwareConfirmatorHost_ShowTextAsync_Hook(void* retBuf,
                                                               void* pThis,
                                                               HSTRING text,
                                                               bool flag)
{
    if (IsSuppressionWindowActive())
    {
        Wh_Log(L"Suppressed ShowTextAsync '%s'", SafeHstringBuffer(text));
        return retBuf;  // ABI: return the caller's buffer pointer in RAX
    }

    return HardwareConfirmatorHost_ShowTextAsync_Original(retBuf, pThis, text, flag);
}

// ConfirmatorHostControl::ShowText returns void, so there is no hidden slot.
// The winrt::hstring const& parameter is a pointer to a single-HSTRING struct.
using ConfirmatorHostControl_ShowText_t = void(WINAPI*)(void* pThis,
                                                        HSTRING* textRef,
                                                        bool flag);
ConfirmatorHostControl_ShowText_t ConfirmatorHostControl_ShowText_Original = nullptr;

static void WINAPI ConfirmatorHostControl_ShowText_Hook(void* pThis,
                                                        HSTRING* textRef,
                                                        bool flag)
{
    if (IsSuppressionWindowActive())
    {
        Wh_Log(L"Suppressed ShowText '%s'",
               SafeHstringBuffer(textRef ? *textRef : nullptr));
        return;
    }

    ConfirmatorHostControl_ShowText_Original(pThis, textRef, flag);
}

// ==== Hook installation ====

template <typename Orig, typename Hook>
static bool TryHookSymbol(HMODULE module, const wchar_t* symbol, Orig* orig, Hook hook)
{
    WindhawkUtils::SYMBOL_HOOK entry{
        {symbol},
        reinterpret_cast<void**>(orig),
        reinterpret_cast<void*>(hook),
        true,  // optional: report per-symbol success via *orig instead
    };
    return WindhawkUtils::HookSymbols(module, &entry, 1) && *orig;
}

static bool InstallSwitchHooks()
{
    std::lock_guard<std::mutex> lock(g_installMutex);
    if (g_switchHooksInstalled.load(std::memory_order_relaxed))
    {
        return true;
    }

    HMODULE twinui = LoadLibraryW(L"twinui.pcshell.dll");
    if (!twinui)
    {
        Wh_Log(L"LoadLibrary twinui.pcshell.dll failed: %u", GetLastError());
        return false;
    }

    int hooked = 0;

    if (TryHookSymbol(
            twinui,
            LR"(public: virtual long __cdecl CVirtualDesktopManager::SwitchDesktopWithAnimation(struct IVirtualDesktop *))",
            &CVirtualDesktopManager_SwitchDesktopWithAnimation_Original,
            CVirtualDesktopManager_SwitchDesktopWithAnimation_Hook))
    {
        Wh_Log(L"Hooked SwitchDesktopWithAnimation");
        ++hooked;
    }

    if (TryHookSymbol(
            twinui,
            LR"(public: virtual long __cdecl CVirtualDesktopManager::SwitchDesktop(struct IVirtualDesktop *))",
            &CVirtualDesktopManager_SwitchDesktop_Original,
            CVirtualDesktopManager_SwitchDesktop_Hook))
    {
        Wh_Log(L"Hooked SwitchDesktop");
        ++hooked;
    }

    if (TryHookSymbol(
            twinui,
            LR"(private: long __cdecl CVirtualDesktopHotkeyHandler::_CycleInDirection(enum VirtualDesktopSwitchDirection))",
            &CVirtualDesktopHotkeyHandler_CycleInDirection_Original,
            CVirtualDesktopHotkeyHandler_CycleInDirection_Hook))
    {
        Wh_Log(L"Hooked CycleInDirection");
        ++hooked;
    }

    if (hooked == 0)
    {
        Wh_Log(L"No desktop switch hooks installed");
        return false;
    }

    Wh_ApplyHookOperations();
    g_switchHooksInstalled.store(true, std::memory_order_relaxed);
    return true;
}

static bool InstallOsdHooks()
{
    std::lock_guard<std::mutex> lock(g_installMutex);
    if (g_osdHooksInstalled.load(std::memory_order_relaxed))
    {
        return true;
    }

    HMODULE module = LoadLibraryW(L"Windows.Internal.HardwareConfirmator.dll");
    if (!module)
    {
        Wh_Log(L"LoadLibrary Windows.Internal.HardwareConfirmator.dll failed: %u",
               GetLastError());
        return false;
    }

    int hooked = 0;

    if (TryHookSymbol(
            module,
            LR"(private: struct winrt::fire_and_forget __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::ShowTextAsync(struct winrt::hstring,bool))",
            &HardwareConfirmatorHost_ShowTextAsync_Original,
            HardwareConfirmatorHost_ShowTextAsync_Hook))
    {
        Wh_Log(L"Hooked HardwareConfirmatorHost::ShowTextAsync");
        ++hooked;
    }

    if (TryHookSymbol(
            module,
            LR"(public: void __cdecl winrt::HWConfirmatorUI::implementation::ConfirmatorHostControl::ShowText(struct winrt::hstring const &,bool))",
            &ConfirmatorHostControl_ShowText_Original,
            ConfirmatorHostControl_ShowText_Hook))
    {
        Wh_Log(L"Hooked ConfirmatorHostControl::ShowText");
        ++hooked;
    }

    if (hooked == 0)
    {
        Wh_Log(L"Confirmator ShowText hooks unavailable — OSD will stay visible");
        return false;
    }

    Wh_ApplyHookOperations();
    g_osdHooksInstalled.store(true, std::memory_order_relaxed);
    Wh_Log(L"Desktop OSD suppression active (%d ShowText hooks)", hooked);
    return true;
}

// ==== Lifecycle ====

static DWORD WINAPI InitRetryThread(LPVOID)
{
    for (int attempt = 0;
         attempt < 20 &&
         (!g_switchHooksInstalled.load() || !g_osdHooksInstalled.load());
         ++attempt)
    {
        Sleep(500);
        if (g_unloading.load())
        {
            return 0;  // teardown started — do not touch mod code/data
        }
        if (!g_switchHooksInstalled.load())
        {
            InstallSwitchHooks();
        }
        if (g_unloading.load())
        {
            return 0;
        }
        if (!g_osdHooksInstalled.load())
        {
            InstallOsdHooks();
        }
    }

    if (!g_switchHooksInstalled.load())
    {
        Wh_Log(L"Init timed out: switch hooks not installed — mod inactive");
    }
    if (!g_osdHooksInstalled.load())
    {
        Wh_Log(L"Init timed out: ShowText hooks not installed — mod inactive");
    }

    return 0;
}

static void StartInitRetryThread()
{
    // Keep the handle so Wh_ModUninit can join before the mod DLL is unmapped.
    g_retryThread = CreateThread(nullptr, 0, InitRetryThread, nullptr, 0, nullptr);
}

BOOL Wh_ModInit()
{
    Wh_Log(L">");

    const bool switchOk = InstallSwitchHooks();
    const bool osdOk = InstallOsdHooks();

    if (!switchOk || !osdOk)
    {
        Wh_Log(L"Initial hook install incomplete; retrying in background");
        StartInitRetryThread();
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    if (!g_switchHooksInstalled.load())
    {
        InstallSwitchHooks();
    }

    if (!g_osdHooksInstalled.load())
    {
        InstallOsdHooks();
    }
}

void Wh_ModUninit()
{
    Wh_Log(L">");
    g_unloading = true;

    // Join the retry thread before returning: Windhawk unmaps this DLL right
    // after Wh_ModUninit, so any still-running thread executing mod code would
    // fault. It checks g_unloading after each step and exits within one
    // in-flight install call. (It is normally never started — both hook sets
    // install synchronously in Wh_ModInit on tested builds.)
    if (g_retryThread)
    {
        WaitForSingleObject(g_retryThread, INFINITE);
        CloseHandle(g_retryThread);
        g_retryThread = nullptr;
    }
}
