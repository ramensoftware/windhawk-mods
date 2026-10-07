// ==WindhawkMod==
// @id              windows-10-legacy-flyouts-restorer
// @name            Windows 10 legacy flyouts on Win11 24H2 restorer
// @description     This mod restores the Windows 10 network icon and its flyout and the Action Center button in the private Windows 10 shell, with the verified Windows 10 tray modules
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lgdi32 -lshell32 -ladvapi32 -luser32 -lwintrust -lcrypt32 -lurlmon -lversion -lwininet -lbcrypt -lcomctl32 -lshlwapi -ldwmapi -luuid -lwlanapi -lruntimeobject
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 legacy flyouts on Win11 24H2 restorer

This mod tries to restore the legacy flyouts on the taskbar when running the Windows 10 explorer.exe on Windows 11 24H2+ using this mod https://windhawk.net/mods/win10-taskbar-on-win11-24h2
Everything on this page happens **inside the private Windows 10 shell** (an `explorer.exe`
that is not the one in `%SystemRoot%`): the Windows 11 shell is never touched.

* the Windows 10 tray modules (`pnidui.dll`, `stobject.dll`) are
  **downloaded by this mod** from the Microsoft symbol server, verified against their pinned
  SHA-256 and their Microsoft signature, then loaded into the private shell. The network
  icon and its menu, the volume icon and the battery flyout come from the genuine Windows 10
  code, not from a reimplementation;
* the window classes the Windows 10 shell asks for are redirected to those verified copies by
  hooking the loader, so the shell cannot end up mixing a Windows 11 DLL with a Windows 10 one;
* the Action Center button that the Windows 11 shell hides is written back on (the opposite of
  the `hide-action-center-icon` mod) and that conflict is handled.

## The click on the network icon and the battery flyout

The left click on the network icon is taken over **at the icon itself**: the window that
`pnidui.dll` registers its tray icon with - the one that receives the `NIN_*` / `WM_LBUTTONUP`
callbacks - is subclassed, and the click is consumed before pnidui's own handler can see it.
That handler is what opened the Settings page, and it does not necessarily go through
`shell32!ShellExecuteW`: blocking URIs there was not enough. The consumed click becomes a
request for the genuine Windows 10 network flyout (shell experience manager, experience
`Windows.Internal.ShellExperience.NetworkFlyout`, anchored on the rectangle of the icon). The
right click and every other message are passed straight through, so the native menu of the icon
is untouched, and the icon itself - registration, image, tooltip - is not modified.

As a second net, every `ms-settings:network*` and `ms-availablenetworks:` target of this shell
is answered with the same request instead of being forwarded, and the shims report success so
that the caller does not fall back to anything else. **No page is ever opened from the network
icon**, and there is no fallback that can open one.

The battery icon is taken over the same way, and opens the same kind of window: the left click is
**consumed**, and the genuine Windows 10 battery flyout is opened through the shell experience
manager - experience `Windows.Internal.ShellExperience.TrayBatteryFlyout`, interface
`IID_TrayBatteryFlyoutExperienceManager`, anchored on the rectangle of the battery icon. That is
the same authentic call the Windows 10 shell makes for this icon, exactly as for the network
flyout above; if the call fails, nothing at all is started and the log names the step that
failed.

1.3.4 fixed two things that kept that click from arriving. The first is where the icon lives:
the previous versions took the click on the service window of `stobject.dll` and recognised the
battery by the **text** of its registration. On this shell neither is true - the battery is
registered on `SystemTray_Main` with an **empty text** (id 1225, and the log of the previous
round says exactly that: `window SystemTray_Main, id 1225, guid {7820AE75-...}, text ""`), so no
click was ever recognised and the mod's request was never made. The GUID names the icon, and it
does not depend on the language of the system, so the mod now follows the GUID: it remembers
where that icon is registered and answers its left click there, on whatever window the shell has
put it. Only that icon is answered - the callback message carries the id of the icon in
`wParam`, and a click that arrives without it must fall inside the rectangle of the battery icon.

**This mod does not answer any registry value.** `UseWin32BatteryFlyout = 1` makes
`stobject.dll` show its Windows 7 era Win32 flyout: that is the compatible behaviour of an older
shell, not the Windows 10 flyout, and it is not used here. No setting is involved either:
Windhawk writes the settings of a mod **when the mod is installed**, and for a value that is not
in the stored list `Wh_GetIntSetting` returns 0 (Windhawk wiki, "Creating a new mod": "if the
value doesn't exist ... the return value is zero"), so a behaviour that depends on a **new**
setting stays switched off on every existing installation. The settings added by 1.3.0/1.3.1
(`LegacyNetworkUx`, `NetworkClickOpensFlyout`, `LegacyBatteryFlyout`, `FlyoutDiagnostics`) are
gone.

The three tiles of the flyout (`networkux.dll` and the `Windows.Networking.UX.*` classes) are
**not** part of this mod: the extra DLL is no longer downloaded, pinned, redirected or loaded,
and the flyout is the one the private Windows 10 shell draws by itself.

## Why the flyout used to close again, and what keeps it open

The flyout is not drawn by Explorer: `ShellExperienceHost.exe` draws it, and how it draws it is
decided by `Windows.UI.QuickActions.dll` - the module that brings the templates of the flyout.
From build 25951 on that module enters the flyout through the new **Control Center** template
set, and the Windows 10 flyout cannot be built with it: the window appears and is torn down
again at once (the "it opens for a millisecond and then closes" of a click on the network icon),
and the battery flyout does not even get that far and takes the process down while it is being
built. This is the same failure ExplorerPatcher fixes on these builds, and this mod applies the
same correction: in the module the shell has loaded, five bytes are turned into NOPs, eight
bytes are copied from the older template loader and the call that follows is pointed at it, so
the shell builds the flyout with the Windows 10 template set and keeps it on screen.

That module is **not loaded** when the mod starts, and it cannot be loaded from here: asking for
it at that moment fails (the log of the previous round says `Windows.UI.QuickActions.dll is not
available (error 1114)`), because the shell loads it later, when it builds the flyout. So the
mod does not load it: it watches the loads of that process and patches the module as soon as it
appears, which is before the shell has built anything with it. The patterns are the ones of the
real module; when they are not found, nothing at all is written and the log says so.

The template set alone is not the whole of it. The page of the network flyout (`NetworkUX.dll`)
asks for the quick action button with the name of the **Windows 11** template, and under the
Windows 10 template set that name is the wrong one: the Windows 10 button is never used and the
buttons the flyout draws are inert - ExplorerPatcher writes it next to the same correction
("they will only appear as non-interactive text blocks"). A flyout that is built in half is a
flyout the shell takes down again. From 1.3.5 the mod takes over the **one** import of that page
which calls `WindowsCreateStringReference` and answers with the Windows 10 button name
(`QuickToggleWinuiFluentTemplate`), and only when the template set of this process really is the
Windows 10 one; it is what ExplorerPatcher does in its `HandleLoadedNetworkUX`. That page is not
loaded by this mod, and nothing else of it is touched. Both halves are applied in **every**
process that draws the flyout, so the flyout opens on every click and not only on the first one.

From 1.3.7 the flyout also takes the part of the **Windows 10 skin** that lives in the resource dictionary of its page - the same one ExplorerPatcher writes next to this correction: the margin of the quick action panel (12,0,0,12), the size of the quick action button (4,0,0,4, 90x64), the two global corner radii and the two focus rectangle thicknesses of the Windows 10 look. The rules of the skin that name single controls of the page need a visual tree engine (the mod the style export comes from); this mod does not carry one, so those are not applied and the log says so.

From 1.3.8 the two halves of that correction survive a **reload of the mod**, which is what used to leave the flyout built in half (inert buttons, or a window taken down at once). The bytes written into `Windows.UI.QuickActions.dll` stay written in memory while the mod is reloaded - the module is not restored on purpose, because putting the original bytes back while the shell is drawing a flyout would build it with the Windows 11 template set again - while the flags of the new copy of the mod read zero, so the site could not be found any more and the button name was never asked for. The two halves are now decided by the **content**: the site is searched first in its untouched form and then in the written one (a copy of the mask with the groups this patch rewrites left free), and the import entry of the page is recognised by looking at where it points. When the work is already there, the module is pinned in memory (`GetModuleHandleExW` with `GET_MODULE_HANDLE_EX_FLAG_PIN`, which by Microsoft's documentation keeps it loaded until the process ends and does not change the reference count), so that a COM unload - a DLL whose `DllCanUnloadNow` says `S_OK` may be unloaded once the `CoFreeUnusedLibrariesEx` delay expires, ten minutes by default - cannot replace a patched copy with a fresh one. The pin is not reversible, lives only in `ShellExperienceHost.exe`, and is never done when the patterns are not found.

## The crash on enable and on disable

The Windhawk documentation says how a mod has to behave around its own lifetime, and 1.3.0 did
not follow it. From the mod API pages:

* `Wh_SetFunctionHook`: "can't be called after `Wh_ModBeforeUninit` returns";
* `Wh_ApplyHookOperations`: "called automatically by Windhawk after `Wh_ModInit`" and, in its own
  words, "ideally, all hooks should be set in `Wh_ModInit` and this function should never be used";
* the "Mod lifetime" page draws the order: `Wh_ModInit`, the implicit apply, then the hooks are
  removed between `Wh_ModBeforeUninit` and `Wh_ModUninit`.

1.3.0 registered the tray hooks from the services thread, that is after the queue had already been
applied, so they were inert; the mod then called `Wh_ApplyHookOperations` in a loop from that
thread while the engine was loading or unloading hooks of its own. The hook queue is not meant to
be operated by two threads at once, which is what took explorer.exe down at enable and at disable.

From 1.3.1: **every hook of this mod is registered inside `Wh_ModInit`** (the tray support hooks,
the key policy hooks, the network click hooks, the battery and Action Center hooks), no other
thread calls a `Wh_*` hook function, and `Wh_ApplyHookOperations` is not used at all.

The disable path followed the same documentation. The shell's clock and show-desktop windows were
subclassed by the mod and the subclass was never removed: their window procedure still pointed
into this module when it was unloaded, so the next message to the clock (it repaints every second)
jumped into freed code. The tray work now tears down what it created - the forced icon, the hidden
owner window and its thread, the fallback icon, and every subclass (including the click
interception added in 1.3.2) - on its own thread, while the module is still loaded, and
`Wh_ModBeforeUninit` stops and joins that thread with a bounded wait because it is the last
callback in which the mod may be running. `Wh_ModUninit` runs after the hooks have been removed:
it no longer waits with `INFINITE`, and it closes the worker's handles only when the worker has
actually exited.

## Where the files come from

The Windows 10 binaries this mod needs (`pnidui.dll`, `stobject.dll` and
`explorer.exe`) are downloaded from the Microsoft symbol server, checked against their pinned
SHA-256 **and** against their Microsoft Authenticode signature, and stored in the folder the
Windows 10 taskbar mod by Anixx uses for its own download:

`%ProgramData%\Windhawk\Engine\ModsWritable\LegacyStore`

Both mods therefore share one set of files instead of two copies of the same system binaries: a
file that is already there and matches the pin is reused as it is, and only what is missing is
downloaded. If that folder cannot be created or written, the mod falls back to its own Windhawk
storage and keeps working on its own.

## Settings

| Setting | What it does |
|---|---|
| `ProvideTrayModules` (default on) | download and load `pnidui.dll` and `stobject.dll` |
| `ShowActionCenterButton` (default on) | show the Action Center button; off leaves the byte alone, so the mod that hides it wins |
| `ActionCenterConflict` (default `reassert`) | `reassert` keeps the button visible and logs the conflict; `log` writes once and only reports |
| `LogTrayActivity` (default off) | writes every load, class factory and menu operation to the log |
| `RequireSignature` (default on) | also check the Authenticode signature of what is downloaded |
| `DownloadTimeoutSec` (default 20) | per-connection timeout of the downloads |

## The Action Center animation

The panel slides in from the right edge and out again, and its close delay is shortened:
the ~2 s timer the shell sets while the pointer is away becomes `ActionCenterCloseDelayMs`.
The technique is the one of the mod **"Action Center fixes (fast close + slide)" by
AdmXP8** (v0.9) - the panel is caught at the moment the shell cloaks it, parked just
outside the screen edge and moved in by a worker thread - with two differences: everything
is behind one setting that is checked at every call, and the docked position is restored on
close and on unload, so the panel can never be left parked off-screen. The panel belongs to
`ShellExperienceHost.exe`, so this mod is loaded in that process too; there, only this
section runs.

## The Action Center button, and the conflict with "hide-action-center-icon"

The Windows 10 taskbar keeps "show the Action Center button" in one byte of the button
window's own data, 120 bytes in. `hide-action-center-icon` writes `FALSE` there; this mod
writes `TRUE` and keeps it there:

* every write is validated first (`VirtualQuery`: committed, writable, not a guard page), so a
  different layout can never crash the shell - it is logged instead;
* the byte is re-read twice per second on this mod's own thread; if another mod sets it back
  to 0, the conflict is named in the log and the value is written again
  (`ActionCenterConflict=reassert`). With `log`, the mod writes once and only reports it;
* with `ShowActionCenterButton` off the byte is never read nor written, which is the setting to
  use when the other mod has to win;
* no registry value is written anywhere: everything is served in memory.

## What is not in this module

Win+X, the Alt+Tab host and the rest of the Windows 10 shell are separate mods of the same
set (two mods hooking the same method would be a race, not a fix). This module resolves no
symbol by name at all.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ForceNetworkTrayIcon: true
  $name: Force the network icon into the taskbar
  $description: >-
    If the native PNI (pnidui.dll) does not register the network icon within the delay
    below, the mod registers it itself with Shell_NotifyIconW, its own owner window and
    the system GUID of the network icon, and verifies with Shell_NotifyIconGetRect that
    the icon really is in the taskbar. When the native icon appears, the forced one is
    retired by itself. ms-availablenetworks: is never used as a substitute.
- ForceNetworkTrayDelaySec: 12
  $name: Wait before forcing the icon (seconds)
  $description: >-
    How long the native icon is given before the mod registers it itself (0-600).
- ForceNetworkTrayResetTraySettings: false
  $name: Reset the saved tray state if the icon still does not appear
  $description: >-
    Last resort: IconStreams/PastIconsStream under TrayNotify are backed up (the backup is
    written next to the store) and then cleared once, because that binary state is shared
    with the Windows 11 shell and can keep the icon marked as hidden. The real registry
    values are never touched by the other steps.
- TrayRestoreOverflowChevron: true
  $name: Give the taskbar its overflow chevron back
  $description: >-
    While the forced icon exists the mod serves EnableAutoTray=0 in memory only; the
    virtual override is removed (and the real value left alone) once the forced icon is
    stable.
- ShellOpGuardTimeoutMs: 1500
  $name: Time limit for the guarded shell operations (ms)
  $description: >-
    The shell operations started from a menu (for example "Customize notification area
    icons") run on a service thread with this time cap (200-10000).
- ProvideTrayModules: true
  $name: Load the Windows 10 tray modules
  $description: >-
    Downloads pnidui.dll and stobject.dll, verifies them (SHA-256 and signature) and loads
    them in the private Windows 10 shell: network icon with its menu, volume icon with the
    battery flyout.
- ShowActionCenterButton: true
  $name: Show the Action Center button
  $description: >-
    The Windows 11 shell does not draw the Action Center button of the Windows 10 taskbar.
    This writes the button flag back on, the opposite of what the mod
    "hide-action-center-icon" does. Off = the byte is left alone.
- ActionCenterConflict: reassert
  $name: Conflict with another mod
  $description: >-
    reassert = keep the button visible and name the conflict in the log; log = write once and
    only report it if another mod changes the byte.
  $options:
  - reassert: Keep it visible
  - log: Write once, report the conflict
- ActionCenterAnimation: true
  $name: Action Center animation
  $description: >-
    The panel slides in and out from the right edge and its ~2 s close timer becomes the
    delay below. Base: the mod "Action Center fixes (fast close + slide)" by AdmXP8.
- ActionCenterCloseDelayMs: 50
  $name: Action Center close delay (ms)
  $description: Replaces the ~2000 ms timer that closes the panel (1-1900).
- ActionCenterSlideInMs: 220
  $name: Action Center slide-in (ms)
  $description: How long the panel takes to slide in. 0 = off.
- ActionCenterSlideOutMs: 140
  $name: Action Center slide-out (ms)
  $description: How long the panel takes to slide out. 0 = off.
- ExperimentalSquareFlyoutCorners: false
  $name: "Experimental: square window corners for the flyouts"
  $description: >-
    EXPERIMENTAL, off by default, and it does not work: it asks DWM for square window corners
    (DWMWA_WINDOW_CORNER_PREFERENCE = DWMWCP_DONOTROUND) on the windows of ShellExperienceHost.exe.
    DWM accepts the request and nothing fails, but the corners of the network flyout stay
    rounded (they are probably drawn by the XAML of the page). Kept only so that someone who
    knows why can fix it. Takes effect when the mod is reloaded.
- LogTrayActivity: false
  $name: Log every tray operation
  $description: >-
    Writes every LoadLibrary, class factory and menu operation to the log. Useful to see which
    module the shell is asking for; it can be verbose.
- RequireSignature: true
  $name: Check the signature of the downloaded files
  $description: >-
    Every file is checked against its pinned SHA-256 first. With this on, the Authenticode
    signature is checked as well (Microsoft signer); with it off a matching hash is enough.
- DownloadTimeoutSec: 20
  $name: Download timeout (seconds)
  $description: >-
    Connection, receive and send timeout of the downloads of the Windows 10 shell files.
*/
// ==/WindhawkModSettings==
// 1.3.3: winsock2.h belongs before windows.h and here it is the first include of the file.
// The compiler of the mod pulls windows.h in before the file, though, and windows.h brings
// MinGW's winsock 1 with it: winsock2.h then reports "Please include winsock2.h before
// windows.h". Nothing in this mod uses the socket API (the downloads go through WinINet), so
// the guard winsock 1 has left behind is dropped before the header is read.
#ifdef _WINSOCKAPI_
#undef _WINSOCKAPI_
#endif
#include <winsock2.h>
#include <xamlom.h>
#include <atomic>
#include <array>
#include <vector>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <Unknwn.h>
#include <winrt/base.h>
#include <ocidl.h>
#include <combaseapi.h>
#include <algorithm>
#include <charconv>
#include <chrono>
#include <cmath>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <initguid.h>
#include <commctrl.h>
#include <d2d1_1.h>
#include <roapi.h>
#include <windows.graphics.effects.h>
#include <winstring.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <dwmapi.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <exception>
#undef INTERFACE
#include <functional>
#include <shellscalingapi.h>  // GetDpiForMonitor, MDT_DEFAULT
#include <windows.h>
#include <stdlib.h>
#include <cstdint>
#include <limits.h>
#include <string.h>
#include <wininet.h>
#include <aclapi.h>
#include <wintrust.h>
#include <softpub.h>
#include <wincrypt.h>
#include <bcrypt.h>
#include <winternl.h>   // UNICODE_STRING for the LdrLoadDll hook
#include <time.h>
#include <tlhelp32.h>   // fotografia dei processi (sonda del centro operativo)
#include <shellapi.h>
#include <shlobj.h>     // SHParseDisplayName / SHOpenFolderAndSelectItems
#include <wlanapi.h>    // interruttore Wi-Fi vero (riquadro in fondo al flyout)
#include <iphlpapi.h>   // mappatura adattatori del fallback rete NLM
#include <netlistmgr.h> // stato NLM per il fallback rete
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winrt/Windows.Networking.NetworkOperators.h>
#include <intrin.h>

// The shell this module lives in. In the monolith g_unloading lived among the globals of
// the shell-services section; here it is the only flag of that kind.
static std::atomic<bool> g_unloading{false};
// The monolith's detailed diagnostic switch: off here, this module logs what it does.
static std::atomic<bool> g_verboseDiagnostics{false};

// Options of this module. They are read once and copied into the fields of the monolith
// configuration the tray/menu code already uses (g_cfg), so those blocks stay as they are.
static bool g_logTrayActivity = false;
static bool g_showActionCenterButton = true;
static bool g_actionCenterReassert = true;

// ---------------------------------------------------------------------------
// Action Center: fast close and slide
//
// Base: the mod "Action Center fixes (fast close + slide)" by AdmXP8 (v0.9), whose
// technique is kept here as it is: the panel is caught at the moment the shell cloaks
// it, parked just outside the screen edge and slid in, and the ~2000 ms close timer is
// replaced with a short one while the panel is open.
//
// Three things are different from the original, all of them inside this mod:
//   * everything is behind one setting and every hook re-checks it at each call, so the
//     animation can be switched on and off while the shell runs, without touching hooks;
//   * the docked position is restored on close, when the panel is identified again and on
//     unload, so the panel can never be left parked outside the screen;
//   * the timer is replaced only when the caller is the shell's own multitasking code
//     (twinui.pcshell.dll in the stack) and the timeout is the ~2 s one, so no other
//     timer of the process is touched.
//
// The panel belongs to ShellExperienceHost.exe: this mod is loaded in that process as
// well (see the include list) and there this section is the only part that runs.
// ---------------------------------------------------------------------------
static bool g_acAnimation = true;
// EXPERIMENTAL (1.3.9), off by default: square window corners for the flyouts of
// ShellExperienceHost.exe through DWMWA_WINDOW_CORNER_PREFERENCE = DWMWCP_DONOTROUND.
// It causes no errors (the log shows DWM answering 0x00000000 for the window of the network
// flyout, class Windows.UI.Core.CoreWindow, title "Network connections"), but it does NOT
// work: the corners of the flyout stay rounded, most likely because they are drawn by the
// XAML of the page and not by the window. Left here, disabled, for anyone who knows why.
static bool g_squareFlyoutCorners = false;
static int g_acCloseDelayMs = 50;      // the ~2000 ms close timer becomes this
static int g_acSlideInMs = 220;        // 0 = off
static int g_acSlideOutMs = 140;       // 0 = off
static volatile bool g_acOpen = false;
static bool g_acHaveFinal = false;
static int g_acFinalX = 0;
static int g_acFinalY = 0;
static HWND g_acPanel = nullptr;       // identified once: the Action Center window
static volatile LONG g_acGeneration = 0;
static bool g_acAnimationHooksInstalled = false;
static std::atomic<unsigned int> g_acActiveCloakCalls{0};
static std::mutex g_acSlideThreadsMutex;
static std::vector<HANDLE> g_acSlideThreads;
static bool g_acSlideThreadsStopping = false;
static int g_acAnimLogs = 0;

static int ClampInt(int value, int low, int high) {
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

// https://stackoverflow.com/a/51274008
template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};
using string_setting_unique_ptr =
    std::unique_ptr<const WCHAR[], deleter_from_fn<Wh_FreeStringSetting>>;


// Legacy component headers stay at global scope.
#undef INTERFACE
#include <windows.h>
#include <stdlib.h>
#include <cstdint>
#include <limits.h>
#include <roapi.h>
#include <winstring.h>
#include <string.h>
#include <wininet.h>
#include <aclapi.h>
#include <wintrust.h>
#include <softpub.h>
#include <wincrypt.h>
#include <bcrypt.h>
#include <string>
#include <atomic>
#include <mutex>
#include <unordered_set>
#include <vector>
#include <winternl.h>   // UNICODE_STRING for the LdrLoadDll hook
#include <time.h>
#include <commctrl.h>   // sottoclasse di finestre (SetWindowSubclass)
#include <tlhelp32.h>   // fotografia dei processi (sonda del centro operativo)
#include <shellapi.h>
#include <shlobj.h>     // SHParseDisplayName / SHOpenFolderAndSelectItems
#include <wlanapi.h>    // interruttore Wi-Fi vero (riquadro in fondo al flyout)
#include <iphlpapi.h>   // mappatura adattatori del fallback rete NLM
// The code resolves IP Helper dynamically; keep these stable GetAdaptersAddresses
// flags available even when an SDK target macro hides their declarations.
#ifndef GAA_FLAG_SKIP_ANYCAST
#define GAA_FLAG_SKIP_ANYCAST 0x00000002
#endif
#ifndef GAA_FLAG_SKIP_MULTICAST
#define GAA_FLAG_SKIP_MULTICAST 0x00000004
#endif
#ifndef GAA_FLAG_SKIP_DNS_SERVER
#define GAA_FLAG_SKIP_DNS_SERVER 0x00000008
#endif
#include <netlistmgr.h> // stato NLM per il fallback rete
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.Networking.NetworkOperators.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <algorithm>
#include <shellscalingapi.h>

// Original legacy-component history (not release documentation):
//
// MODIFICHE CANDIDATE (2026-10-04) - 1.0.0 (bootstrap PNI path-gated):
// - Il log utente di 1.0.0 mostra SHEnableServiceObject -> S_OK prima che il redirect
//   di stobject.dll verso ProgramData risulti nella traccia. La chiamata viene ora rinviata
//   finché pnidui.dll e stobject.dll mappate corrispondono ai percorsi privati attesi;
//   il class-factory lookup usa anch'esso solo i moduli privati e gli hash kTrayFiles sono
//   riverificati prima dell'SSO. Se un basename era già caricato da un altro percorso, la
//   copia fissata viene richiesta esplicitamente e il risultato viene verificato. Niente
//   patch allo stobject di sistema o all'Explorer nativo.
// - Non viene applicata la patch Windows-To-Go-slot pensata per stobject 26100: questo
//   candidato fissa stobject 10.0.19041.7664 (SHA-256 7c0037535c4da20ae15b4df9662f9330a989d4e2f36284aac5c9c3bce429e118).
//   Nel file fissato il sentinel Windows-To-Go non è presente; è presente il CLSID del
//   Network Tray SSO, quindi la riscrittura 26100 non è trasferibile a questa DLL.
// - Il servizio SSO viene richiesto solo se la DLL fissata e' quella attesa e il
//   CLSID risolve; senza callback PNI il fallback resta fail-closed (nessun URI).
//
// MODIFICHE CANDIDATE (2026-10-04) - 1.0.0:
// - verboseDiagnostics defaults to false: repetitive XAML/property/style traces and
//   the optional notification-database probe require an explicit opt-in. Its copied
//   files, SQLite connection and statements are RAII-managed; payloads are never read
//   or logged, while operational errors remain visible.
// - ActionCenterEntranceAnimation is limited to the private Explorer and qualifying
//   ShellExperienceHost/ShellHost CoreWindows. It derives the first-open endpoint
//   from live monitor/work-area/DPI/window geometry, animates on RAII-owned workers,
//   preserves the native DWM result on exception, and drains workers before unload.
// - Hook state changes only for non-duplicate transitions; a failed initial placement
//   restores the closed state, and exception fallback never calls DWM twice.
// - New runtime trace still has no PNIHiddenWnd/NIM_ADD. The network icon remains
//   fail-closed until a verified pnidui click callback can open the Win10 flyout.
//   The name is resolved through the variant table and the probe logs the outcome.
//
// MODIFICHE AGGIUNTE (2026-10-06) - 1.3.8 (il flyout costruito a meta' dopo un ricaricamento del
// mod, e il modulo che non deve piu' sparire: contenuto invece di flag, e modulo fissato in
// memoria):
// - Difetto corretto: dopo che Windhawk ricarica il mod in un processo gia' avviato
//   (ShellExperienceHost.exe, il processo che disegna i flyout), il flyout di rete tornava a
//   essere costruito a meta' - pulsanti inerti, e nei casi peggiori la finestra disfatta subito.
//   La patch dei byte in Windows.UI.QuickActions.dll resta scritta in memoria perche'
//   Wh_ModBeforeUninit non la annulla (e non deve annullarla: rimettere i byte originali mentre
//   la shell disegna un flyout lo farebbe costruire di nuovo col set di Windows 11 e cadere),
//   mentre i flag della copia nuova del mod sono nuovi e valgono zero: la ricerca del sito con la
//   maschera intatta non lo trovava piu' (l'indice 6 vuole la 0xE8 della "call LoadComponent",
//   che li' e' stata sostituita da cinque NOP) e la funzione usciva con "nothing is patched".
//   TemplatesPatched() restava falso, quindi la pagina del flyout di rete non riceveva mai il
//   nome del pulsante di Windows 10 (QuickToggleWinuiFluentTemplate) e restava col nome di
//   Windows 11, che il set di Windows 10 non conosce.
// - La correzione legge il CONTENUTO del modulo invece dei flag (A, B, C del giro):
//   * B - nella stessa posizione si cerca anche la forma che porta GIA' la patch, con una copia
//     della maschera in cui sono liberi solo i gruppi che questa patch riscrive (indici 6-10,
//     52-59 e 74-77: nel file 7-10 e 74-77 erano gia' '?', quindi cambiano davvero l'indice 6 e
//     gli indici 52-59). Se il sito si trova cosi', e i cinque NOP ci sono e gli otto byte del
//     caricatore vecchio corrispondono, lo stato viene preso com'e': g_patched torna vero
//     ("already carries the Windows 10 template set") e non si scrive nulla. La ricerca della
//     forma intatta resta la prima, cosi' le due strade non si confondono.
//   * C - via il fermo "se g_patched esci subito" (diceva "l'ho fatto io", non "il modulo in
//     memoria e' riscritto") e via il fermo su g_redirected nel blocco di rete: la voce della
//     tabella delle importazioni di NetworkUX.dll viene riconosciuta dal suo contenuto (punta
//     gia' alla funzione di questo mod?) e la decisione non usa mai l'indirizzo di base del
//     modulo, che ASLR puo' riassegnare a una copia nuova.
//   * Se il sito c'e' ma i suoi byte non sono ne' la forma intatta ne' quella riscritta, non si
//     scrive niente e il log lo dice: una build sconosciuta non viene toccata.
// - A - il modulo viene fissato in memoria con GetModuleHandleExW e
//   GET_MODULE_HANDLE_EX_FLAG_PIN (piu' GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, con la base del
//   modulo come indirizzo): documentazione Microsoft, "the module stays loaded until the process
//   terminates, regardless of the number of calls to FreeLibrary", e il conteggio dei
//   riferimenti non viene incrementato (nessun FreeLibrary da fare, e non se ne fa nessuno). E'
//   la risposta al caso che la documentazione di CoFreeUnusedLibrariesEx descrive: una DLL COM
//   il cui DllCanUnloadNow risponde S_OK puo' essere scaricata quando il ritardo e' scaduto
//   (dieci minuti per impostazione predefinita), e una copia nuova non porterebbe nulla di
//   quello che il mod ha fatto. Il pin si fa dopo che la patch e' stata scritta O riconosciuta
//   gia' presente, mai quando i pattern non si trovano; non e' reversibile e vive solo in
//   ShellExperienceHost.exe, e il log lo scrive.
// - Diagnostica: le righe di [flyout-host] e [networkux] portano ora la base del modulo e
//   l'esito (written now / already carries the template set / site unknown / not found / pinned),
//   con i contatori limitati come nel resto del file. Nessuna impostazione nuova, nessun valore
//   di registro, nessuna pagina, nessun modulo caricato dal mod, e i menu contestuali, il
//   vassoio, la batteria e l'Action Center restano come sono.
// MODIFICHE AGGIUNTE (2026-10-06) - 1.3.7 (la pelle grafica del flyout di rete: le regole
// del file "10Flyouts v4.5" che si possono scrivere nel dizionario di risorse):
// - Richiesta di questo giro: applicare la pelle grafica del file allegato
//   (uploads/10Flyouts v4.5.txt) al flyout di rete. Quel file e' un elenco di regole
//   scritte per il motore di una mod di stile ("Windows 11 Notification Center Styler"):
//   ogni regola nomina un elemento dell'albero XAML per nome e posizione, e quel motore le
//   va a scrivere elemento per elemento. Questo mod non porta quel motore dentro di se' -
//   non prende in mano l'albero XAML di un altro programma, non fa da ponte verso un
//   motore esterno e non finge di averlo: le regole che nominano i singoli controlli della
//   pagina (bordo del LogonFrame, fondo acrilico, collegamento "Impostazioni" e la sua
//   descrizione, indicatore di selezione della lista delle reti, margini dei pulsanti,
//   caratteri) restano fuori, e il log lo dice.
// - Quello che si applica e' la parte che vive nel dizionario di risorse della pagina, la
//   stessa che ExplorerPatcher scrive nella sua NetworkUX_PatchResourceDictionary (chiamata
//   subito dopo NetworkUX::App::LoadResourceDictionaries): i valori di Windows 10.
//   * QuickActionPanelMargin = Thickness(12,0,0,12) - il margine del pannello delle azioni
//     rapide (Windows 11 usa 12,0,24,0).
//   * QuickActionControlStyle - la misura del singolo pulsante: Margin 4,0,0,4, Width 90,
//     Height 64 (Windows 11: 12,0,0,0 e 96x90). Si toccano solo i tre setter della misura e
//     solo quando lo stile non e' ancora in uso (sealed): gli altri setter restano dove
//     sono, e uno stile gia' usato non si forza.
//   * ControlCornerRadius e OverlayCornerRadius a 0 - la documentazione Microsoft li chiama
//     raggi d'angolo globali ("You can override these values in your App.xaml to change the
//     rounding across all controls in your app"). E' il punto in cui il "CornerRadius=0"
//     che il file chiede su bordi, pulsanti, caselle e barre di scorrimento si puo' chiedere
//     per tutti.
//   * FocusVisualPrimaryThickness e FocusVisualSecondaryThickness a 0, quando questa build
//     li tiene nel dizionario: il file li azzera su griglie, pulsanti e link, ed e' la stessa
//     cosa che la comunita' usa per togliere il rettangolo bianco dai flyout di Windows 10
//     su Windows 11 ("10FlyoutFix").
// - Un valore si sostituisce solo se il tipo che c'e' regge quello nuovo (il tipo si legge
//   dal valore presente, non si indovina); le due chiavi dei raggi d'angolo si aggiungono
//   quando non ci sono, perche' e' la loro stessa documentazione a dire che si
//   sovrascrivono cosi'. In tutti gli altri casi non si scrive niente.
// - Come si applica: dalla voce di 1.3.5 (la chiamata WindowsCreateStringReference di
//   NetworkUX.dll). E' la pagina stessa a chiamare, quindi il filo e' quello che disegna il
//   flyout e il momento e' il suo; il dizionario arriva mentre la pagina si costruisce,
//   quindi finche' le sue chiavi non ci sono si riprova alla chiamata dopo (al massimo
//   qualche decina di tentativi, poi si smette). Il punto in cui ExplorerPatcher si aggancia
//   per scrivere il dizionario (NetworkUX::App::LoadResourceDictionaries, in
//   chunk-skin-pattern.inc) si legge con lo stesso pattern, ma NON viene agganciato: il mod
//   registra tutti i suoi hook in Wh_ModInit (vedi la nota li') e questa parte non tocca la
//   coda degli hook, non aggiunge impostazioni, non legge e non scrive il registro e non
//   carica nessun modulo.
// MODIFICHE AGGIUNTE (2026-10-06) - 1.3.6 (compilazione: la voce presa da combase.dll; via i
// menu contestuali che duplicano altre mod):
// - Compilazione: GetProcAddress restituisce FARPROC, non un void*, e il compilatore di
//   Windhawk (clang) non lo converte da solo. La voce WindowsCreateStringReference presa da
//   combase.dll viene ora passata con un cast esplicito, come il file fa gia' per le altre
//   voci prese allo stesso modo (LoadLibraryW, LdrLoadDll, DwmSetWindowAttribute):
//   "cannot initialize a variable of type 'void *' with an rvalue of type 'FARPROC'".
// - Via le voci della disposizione delle finestre dal menu della barra: "Sovrapponi le
//   finestre", "Mostra le finestre in pila", "Mostra le finestre affiancate" e "Mostra
//   desktop" non vengono piu' aggiunte. Erano ricostruite da questo mod (CascadeWindows,
//   TileWindows, l'oggetto Shell) e le gestisce un'altra mod.
// - Via la cascata "Cerca" (Search) sotto "Toolbars", con le sue tre voci: non viene piu'
//   inserita nel menu della barra, ne' in quello dell'orologio, ne' in quello dell'indicatore
//   di lingua. Con lei se ne vanno la memoria virtuale del valore SearchboxTaskbarMode e la
//   scrittura del modo di ricerca.
// - Via le due voci del menu dell'orologio ("Regola data/ora" e "Personalizza icone di
//   notifica") e il menu di ripiego dell'orologio: se la shell non mostra un menu per
//   l'orologio, questo mod non ne mostra uno suo.
// - Via il percorso del menu Win+X della mod: la risposta alla richiesta (che nel log si
//   vedeva come "Win+X request on the Win10 taskbar") e il messaggio privato che la portava.
//   Resta il controllo che lascia il menu nativo quando il clic e' sul pulsante Start: quel
//   menu lo mostra un altro modulo, e questo mod non deve sostituirlo con quello della barra.
// - Non cambia nient'altro, e in particolare la logica dei menu contestuali resta la sua: il
//   menu della barra e' ancora quello di Windows 10 ricostruito dalla risorsa di shell32
//   (Task Manager, Blocca la barra delle applicazioni, Impostazioni della barra delle
//   applicazioni), il menu di rete e' ancora quello della pnidui consegnato alla pnidui
//   stessa, il menu della batteria porta ancora le due voci di stobject, il menu del pulsante
//   Mostra desktop tiene "Mostra desktop" e Aero Peek, e i menu di destra delle icone del
//   vassoio e il disegno immersivo restano come prima. Nessuna impostazione nuova, nessun
//   valore di registro nuovo, nessuna pagina aperta.
// MODIFICHE AGGIUNTE (2026-10-06) - 1.3.5 (i pulsanti del flyout di rete tornano quelli di
// Windows 10; il mod prende in prestito le risorse con RAII):
// - Il set di template di Windows 10 della 1.3.4 non basta da solo nel processo che disegna il
//   flyout. La pagina del flyout di rete (NetworkUX.dll) chiede il pulsante delle azioni rapide
//   con il nome del template di Windows 11 ("ToggleButtonWinuiFluentTemplate"): con quel nome
//   il pulsante del set di Windows 10 non viene mai usato e i pulsanti restano blocchi di testo
//   inerti - ExplorerPatcher lo scrive accanto alla stessa correzione ("If we're doing the
//   quick actions patch but not this, they will only appear as non-interactive text blocks").
//   La 1.3.4 faceva quindi meta' del ricambio, ed e' il pezzo che manca perche' il flyout si
//   disegni a ogni clic e non una volta sola. La 1.3.5 fa la stessa cosa della sua
//   HandleLoadedNetworkUX: la voce della tabella delle importazioni di NetworkUX.dll che chiama
//   WindowsCreateStringReference viene mandata a una funzione di questo mod, che cambia quel
//   solo nome in "QuickToggleWinuiFluentTemplate" (il pulsante di Windows 10), e soltanto se in
//   questo processo il set di template di Windows 10 e' stato davvero applicato.
// - NetworkUX.dll non viene caricato da questo mod: si prende se e' gia' presente, o quando la
//   shell lo carica - lo stesso avviso (LdrLoadDll) che prende Windows.UI.QuickActions.dll.
//   Del modulo si tocca una sola voce della tabella delle importazioni: nient'altro.
// - Le due meta' valgono in ogni processo che disegna il flyout, non nel primo che ci riesce:
//   ogni ShellExperienceHost.exe che la shell avvia prende il set di template di Windows 10 e il
//   nome del pulsante di Windows 10, perche' il mod li applica in quel processo quando i due
//   moduli arrivano (o subito, se li ha gia' caricati quando il mod parte). Nessun fermo "una
//   volta sola" tra un clic e l'altro, nessun altro processo toccato.
// - RAII (chiesto in questo giro): ScopedWriteProtect rende scrivibile una pagina e la rimette
//   com'era quando esce di scena (anche uscendo prima, con return, o con un'eccezione C++), e
//   ScopedImportRedirect fa lo stesso con la voce della tabella delle importazioni - che quindi
//   torna al valore di prima quando il mod si scarica (Wh_ModBeforeUninit). Il ricambio dei
//   template della 1.3.4 usa lo stesso ScopedWriteProtect al posto della coppia di
//   VirtualProtect scritta a mano.
// - Dal file "10Flyouts v4.5" dell'utente (le sue regole per il flyout di rete) questo giro
//   applica la parte che riguarda i pulsanti delle azioni rapide: le regole su
//   QuickActions.QuickToggleButtonDesktopWinuiFluent sono esattamente il pulsante "QuickToggle"
//   di Windows 10 che qui si torna a usare, e le regole sul set di template sono quelle che la
//   1.3.4 aveva gia' ripreso (il set di Windows 10 nel processo dei flyout).
//   Le regole che restano riguardano i controlli dentro la pagina (i bordi del LogonFrame, il
//   fondo acrilico, i margini, l'indicatore di selezione della lista delle reti, i rettangoli
//   del focus): agiscono sull'albero XAML e arrivano con la revisione successiva, con lo stesso
//   riferimento di ExplorerPatcher (NetworkUX_PatchResourceDictionary, chiamata subito dopo
//   NetworkUX::App::LoadResourceDictionaries). Qui non si inventa nulla e non si stravolge
//   niente: niente impostazioni nuove, nessun registro, nessuna pagina, nessuna DLL caricata a
//   forza.
// MODIFICHE AGGIUNTE (2026-10-06) - 1.3.4 (il flyout si apre e resta aperto; la batteria si
// riconosce dal suo GUID):
// - Il flyout non si chiude piu' dopo un istante. Il processo che disegna i flyout
//   (ShellExperienceHost.exe) costruiva il flyout di Windows 10 con il set di template nuovo
//   ("Control Center") di Windows.UI.QuickActions.dll, che esiste da Windows 11 build 25951 e
//   con cui il flyout di Windows 10 non si costruisce: la finestra si apriva e veniva disfatta
//   subito (e la batteria faceva cadere il processo mentre lo costruiva). In quel modulo
//   cinque byte diventano NOP, otto byte vengono copiati dal vecchio caricatore di template e
//   la chiamata che segue viene puntata li': e' la stessa correzione di ExplorerPatcher per
//   queste build ("Fix battery flyout crashing on 25951+"), applicata al modulo che la shell
//   ha caricato.
// - La 1.3.3 caricava quel modulo con LoadLibraryW all'avvio del processo dei flyout: la
//   chiamata fallisce (error 1114, "not available" nel log) perche' la shell lo carica dopo.
//   La 1.3.4 non lo carica: sorveglia i caricamenti del processo (LdrLoadDll) e cambia il
//   modulo appena compare, prima che la shell ci costruisca qualcosa. Se i pattern non si
//   trovano non viene scritto nulla e il log lo dice.
// - Il clic sull'icona della batteria ora arriva. La batteria non si registra con un testo
//   (registrazione con testo vuoto) e non sta in una finestra di servizio della pnidui: in
//   questa build e' su SystemTray_Main (il log della 1.3.3 lo mostra: id 1225, guid
//   {7820AE75-...}, testo ""), quindi il riconoscimento per suggerimento non la prendeva mai e
//   la presa del clic non si armava: nessuna riga [battery] nel log e nessun flyout. Ora
//   l'icona si riconosce dal suo GUID - che non dipende dalla lingua del sistema - e il clic
//   viene preso sulla finestra dove quella registrazione e' arrivata, qualunque sia.
// - La presa del clic della batteria tocca solo quell'icona: il messaggio di richiamo porta in
//   wParam l'id dell'icona, e un clic che arriva senza di esso vale solo se cade dentro il
//   rettangolo dell'icona (Shell_NotifyIconGetRect). Le altre icone della stessa finestra non
//   vengono toccate.
// - Il clic resta consumato e non apre nient'altro: nessuna pagina, nessun flyout Win32 e
//   nessun valore di registro; la chiamata e' quella autentica della shell Windows 10
//   (GetExperienceManager(L"Windows.Internal.ShellExperience.TrayBatteryFlyout") ->
//   QueryInterface(IID_TrayBatteryFlyoutExperienceManager) -> ShowFlyout(rect)).
// - La 1.3.3 resta com'era per il resto: tre tile rimosse, nessuna impostazione nuova, hook
//   registrati mentre Wh_ModInit gira secondo la documentazione Windhawk.
// MODIFICHE AGGIUNTE (2026-10-06) - 1.3.3 (batteria: flyout Windows 10 autentico):
// - Via la risposta in memoria a UseWin32BatteryFlyout: quello era il flyout Win32 di
//   stobject.dll, cioe' il comportamento compatibile con Windows 7, non il flyout di
//   Windows 10. Nessun valore di registro viene piu' letto o risposto.
// - Il clic sinistro sull'icona della batteria viene preso sulla finestra di servizio di
//   stobject.dll e consumato: il gestore di stobject non lo vede mai. Il flyout che si apre
//   e' quello della shell Windows 10, chiamato come lo chiama la shell stessa:
//   GetExperienceManager(L"Windows.Internal.ShellExperience.TrayBatteryFlyout") ->
//   QueryInterface(IID_TrayBatteryFlyoutExperienceManager) -> ShowFlyout(rect), con il
//   rettangolo dell'icona preso da Shell_NotifyIconGetRect. E' la stessa strada autentica del
//   flyout di rete, non una pagina e non un flyout Win32.
// - Se la chiamata non riesce non viene avviato nulla (nessuna pagina, nessun flyout Win32,
//   nessun valore di registro) e il log nomina il passo che ha fallito.
// - Il flyout resta aperto. ShellExperienceHost.exe e' il processo che disegna il flyout, e
//   decide come disegnarlo con una funzione di Windows.UI.QuickActions.dll (la sua modalita'
//   "remodel"): in quella modalita' il flyout di Windows 10 viene costruito e disfatto subito
//   dopo, ed e' il "si apre per qualche secondo e si chiude". Quando il mod parte dentro quel
//   processo fa rispondere quella funzione come risponde Windows 10: il flyout viene disegnato
//   e resta a schermo. E' la stessa correzione di ExplorerPatcher; i due byte vengono cambiati
//   solo quando il pattern e' trovato esattamente una volta, altrimenti non si scrive niente e
//   il log lo dice.
// - Compilazione: la procedura della finestra dell'icona viene prima del namespace della
//   batteria, quindi la richiesta del flyout della batteria e' dichiarata dove il file dichiara
//   gia' ShowNetworkIconMenuHere (prima era "no member named 'BatteryFlyout' in namespace
//   'RestorerTaskbar::NetworkTrayForce'"), e #include <winsock2.h> non produce piu' l'avviso
//   "Please include winsock2.h before windows.h" portato dal compilatore del mod.
// - La 1.3.2 resta com'era per il resto: tre tile rimosse, nessuna impostazione nuova,
//   ciclo di vita degli hook secondo la documentazione Windhawk.
// MODIFICHE AGGIUNTE (2026-10-03) - 1.0.0 (bootstrap SSO + fallback dinamico, candidato):
// - Il log runtime fornito per 1.0.0 mostra pnidui.dll caricata e l'attivazione SSO, ma
//   non PNIHiddenWnd né una network NIM_ADD con il GUID 7820AE74; `.73` e `.75` sono
//   volume/alimentazione. La candidata carica pnidui.dll prima di stobject.dll e intercetta
//   soltanto il CLSID Network Tray SSO
//   nell'Explorer privato, forza i soli read di ReplaceVan/VANFromPCSettings a 0
//   e prova SHEnableServiceObject dopo che la taskbar privata esiste. CoCreateInstance viene instradata alla factory della pnidui.dll Windows 10 fissata e il
//   log registra caller/HRESULT; la tabella SSO non viene patchata e non si duplicano slot.
//   networkux.dll (le tre tile del flyout di rete) e' stato rimosso del tutto nella 1.3.2;
//   NetworkFlyout torna alla normale attivazione PNI/shell.
// - Se e solo se pnidui ha tentato NIM_ADD con un PNIHiddenWnd del modulo pnidui e un vero
//   callback message, un NIM_ADD nativo fallito o privo di HICON abilita il fallback dinamico
//   NLM: risorse RT_GROUP_ICON autentiche della DLL fissata, stato NLM + tipo/segnale
//   adattatore, GUID di rete di sistema e stessa identità/callback PNI. Il clic passa al
//   gestore PNI originale; il mod non crea WndProc/URI/Settings come destinazione. Il fatto
//   che il gestore apra il flyout sul build bersaglio resta da verificare. Senza callback PNI
//   il fallback non registra alcuna icona. `try/catch`, RAII per HICON, DLL, WLAN memory/handle e
//   apartment COM; modifiche limitate all'Explorer privato. Nessun codice EP riutilizzato;
//   `ep_taskbar` non ispezionato/decompilato. Il nome viene verificato a runtime (log).
//
// MODIFICHE AGGIUNTE (2026-10-03) - 1.0.0 (icona nativa / URL esatto):
// - Su segnalazione dell'utente, rimosso lo shim sintetico: la voce nativa compare
//   nella pagina Notification Area Icons ma non nella taskbar ripristinata.
// - "Personalizza icone di notifica" lancia soltanto l'URL esatto
//   shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}; niente PIDL alternativo né
//   fallback a ms-settings:taskbar. La traccia PNI resta solo diagnostica.
// - Report ricevuti: Explorer 10.0.19039.1 -> RPCRT4.dll 10.0.26100.6899,
//   AV 0xC0000005 offset 0xB0A4; ShellHost -> local@..._unloaded, AV offset
//   0x589E0. Senza dump/stack non si identifica il chiamante né si attribuisce
//   la causa a RPCRT4 o a un hook. Il sorgente locale non è stato compilato da noi;
//   il log runtime fornito è marcato 1.0.0.
//
// MODIFICHE AGGIUNTE (2026-10-03) - 1.0.0 (shim candidato, dopo il feedback):
// - L'utente segnala che l'icona resta assente con la candidata 1.0.0: rimosso il
//   tentativo esplicito di avvio SSO/IOleCommandTarget; non viene presentato come fix.
// - Nell'Explorer privato, una finestra owner nascosta del mod registra un'icona
//   NIM_ADD caricata dal gruppo RT_GROUP_ICON 3048 della pnidui.dll Windows 10
//   hash-pinned. Il file è mappato come risorsa dati; LookupIconIdFromDirectoryEx
//   seleziona la variante adatta alla tray e CreateIconFromResourceEx ne crea l'HICON.
// - Il callback sinistro apre ms-availablenetworks:;
//   TaskbarCreated re-registra l'icona. Reset RAII esegue NIM_DELETE, DestroyIcon,
//   DestroyWindow e UnregisterClass. Try/catch nei punti di callback e cleanup.
// - Il tracking Shell_NotifyIconW è installato prima di caricare pnidui.dll e marca
//   la registrazione nativa solo dopo un NIM_ADD riuscito. Se l'icona PNI nativa
//   compare, lo shim rimuove la propria per evitare duplicati; ricontrolla anche
//   la classe HWND e ritenta una rimozione fallita. Il message pump dispatcha i
//   callback della finestra nascosta e TaskbarCreated. Il glifo è statico: non
//   indica tipo o intensità del collegamento.
//   Solo Explorer privato; questa funzione non scrive HKLM, non riusa codice EP e
//   non ispeziona/decompila ep_taskbar. Candidata 1.0.0 poi ritirata.
//
// MODIFICHE AGGIUNTE (2026-10-03) - 1.0.0 (tentativo SSO superato, non confermato):
// - Il log 1.0.0 mostrava pnidui.dll caricata e IClassFactory::CreateInstance
//   riuscita, ma non PNIHiddenWnd né NIM_ADD. Il ciclo OLECMDID_NEW/SAVE era un
//   tentativo clean-room per distinguere la creazione COM dall'avvio del servizio.
// - L'utente ha poi confermato che l'icona non è apparsa; questa strada è stata
//   abbandonata in favore dello shim 1.0.0. Non dichiarare la patch SSO riuscita.
//
// MODIFICHE AGGIUNTE (2026-10-03) - 1.0.0:
// - La sola LoadLibrary di pnidui.dll non avvia necessariamente l'icona di rete:
//   la shell deve attivare il Network Tray SSO ({C2796011-81BA-4148-8FCA-C6643245113F}).
//   Nella shell privata il mod attiva direttamente la classe COM dalla fabbrica
//   della pnidui.dll verificata; mantiene l'IUnknown per tutta la vita del thread
//   STA a coda messaggi, poi lo rilascia prima di CoUninitialize (winrt::com_ptr,
//   RAII). Nessuna scrittura HKLM, scansione di .rdata o patch di strutture
//   private di stobject.dll; percorso isolato alla shell Windows 10 privata.
// - Se la DLL o la classe non sono pronte all'avvio, il tentativo COM viene
//   ripetuto ogni 5 s fino a 60 s, con HRESULT nel log. Il thread STA viene
//   avviato anche quando sono disattivati gli altri servizi, finché provideTrayDlls
//   è attivo; il SSO viene attivato solo nel processo Explorer privato. Le chiamate
//   COM sono protette da try/catch e il class factory usa winrt::com_ptr; la
//   callback che cerca PNIHiddenWnd ora restituisce davvero la finestra senza
//   subclass laterali.
//
// MODIFICHE AGGIUNTE (2026-10-03) - 1.0.0:
// - Diagnostica del vassoio: all'avvio della shell privata la mod scrive per
//   esteso cosa contiene la cartella dati (pnidui.dll, stobject.dll: presenti con
//   la dimensione, oppure mancanti) e avvisa se un
//   modulo del vassoio non e' stato caricato. Se un modulo manca, il
//   caricamento viene ritentato ogni 10 s per un minuto invece di lasciare
//   l'icona assente per tutta la sessione (l'icona di rete e il suo fumetto
//   sono creati da pnidui.dll caricata da questa mod nella shell di Windows 10:
//   senza la mod attiva in quel processo non esistono).
// - La riga di log [restorer] riporta la versione effettiva: prima diceva
//   sempre 1.0.0 anche dopo l'aggiornamento.
//
// MODIFICHE AGGIUNTE (2026-10-03) - 1.0.0:
// - Tentata correzione di WIN+X: di default la chord resta alla shell di Windows 10
//   (nessun RegisterHotKey MOD_WIN, nessun hook low-level installato). L'opzione
//   winXMenu=custom riattiva il menu localizzato della mod, che non consuma la chord,
//   lascia ~350 ms di priorita' al menu nativo e usa una finestra owner creata dal
//   thread che mostra il menu. Il comportamento resta noto come non funzionante
//   sull'hardware provato: non va considerato un fix confermato (vedi Known Issues).
// - Sezione SehTiles riscritta in modo indipendente: le sequenze di byte sono
//   espresse come descrittori di istruzione annotati (un byte per voce, con
//   l'istruzione che codificano) e ogni bersaglio risolto e' validato a runtime
//   prima di scrivere. Nessun codice, commento, identificatore o tabella e'
//   tratto da ExplorerPatcher (GPL-2.0), che resta citato solo per la tecnica.
// - README corretto: lo stato reale dei test (eseguiti su macchina fisica, non in
//   VM), i limiti noti (rotazione non supportata) e la nuova sezione "Licensing"
//   con la licenza effettiva di ogni componente upstream.
//
// MODIFICHE AGGIUNTE (2026-10-02):
// - Aggiunto menu WIN+X completo con tutte le voci standard (17 voci)
// - Aggiunta voce "Cerca" al menu contestuale della lingua (ITA/ENG/ESP)
// - Implementato fallback per WIN+X con menu personalizzato se il nativo fallisce
// - Usato RAII (ScopedMenu) e try-catch per la gestione degli errori
// - Supporto multilingua per 10 lingue (IT, EN, FR, ES, DE, PT, NL, RU, JA, PL)
// - Tutte le API usate sono pubbliche (user32.dll, shell32.dll)
//
// ---------------------------------------------------------------------------
// WHAT THIS MOD DOES (and what it does NOT do)
//
//  userinit.exe (shell redirection):
//   1. prepares the data folder with the binaries downloaded from the Microsoft
//      symbol server and VERIFIES them (pinned SHA-256 + Authenticode signature
//      + build gate) before they are used;
//   2. redirects reads of the "Shell" value to the private copy;
//   3. also writes the per-user Shell value (HKCU): when Windows honours it the
//      legacy shell starts without going through the logon path (see the note
//      further down);
//  explorer.exe (legacy taskbar fixes inside the private copy only):
//   4. path spoof (GetModuleFileNameW), as in the "Fake Explorer path" mod;
//      v0.8.0 also hooks native explorer for the configurable ribbon and Alt+Tab.
//      Tray patches and path spoof stay private-shell-only.
//   5. taskbar context menu fix (LoadMenuW): Windows 10 Search cascade below
//      Toolbars plus the four classic entries (cascade, stacked, side by side,
//      show the desktop), with documented registry/menu APIs;
//   5b. language indicator context menu: adds "Search" entry to the ITA/ENG/ESP
//       menu with multilingual support;
//   6. (removed in 1.3.9) language indicator colours: now the separate mod
//      "windows-10-language-flyout-guard";
//   7. notification crash fix, enabled on the verified runtime build when a
//      known offset exists; otherwise DisableNotificationCenter is virtual only
//      in the relevant process and registry reads;
//   8. unloading clears that virtual fallback without changing a real policy;
//   9. click on the network icon: ms-settings:network -> ms-availablenetworks:;
//   9b. right click on the network icon: pnidui is given resource 3014 (the
//       Windows 10 menu, "Troubleshoot problems" / "Open Network & Internet
//       settings"), which is missing on 24H2 because it lives in the MUI;
//  10. taskbar watchdog + emergency hotkey Ctrl+Alt+Shift+R;
//  11. anti-loop guard: after three failed starts the native shell is restored;
//  12. tray module strings (battery included) served in place of their .mui,
//      which is not part of the data folder: the same gap that left the network
//      icon without a menu (resource 3014);
//  13. (removed in 1.3.9) the language indicator cell painting: see item 6;
//  14. popup menu supervision (TrackPopupMenu/Ex): it says whether the battery
//      menu is missing because the click never arrives or because the menu is
//      shown empty;
//  15. unloading: the return to the Windows 11 shell is prepared by
//      Wh_ModBeforeUninit and performed by a detached script. No ExitProcess in
//      the mod unload path (Windhawk frees the DLL with a single FreeLibrary as
//      soon as Wh_ModUninit returns: removing the process there leaves the UI on
//      "Uninitializing...");
//  16. battery context menu: the two Windows 10 entries (commands 101 and 102)
//      are appended when the shell's own strings for ids 150/151 come up empty;
//  17. (removed in 1.3.9) language guard: now in "windows-10-language-flyout-guard";
//  18. (removed in 1.3.9) indicator cell background: see item 6;
//  19. UWP apps (Settings, Calculator, Store, ...) on the taskbar: SPERIMENTALE e
//      spento per default (g_cfg.fixUwpTaskbar). Quando e' acceso, explorer is told
//      to treat ApplicationFrameWindow as a normal window (build-gated hook, see
//      the UwpTaskbar section) and a helper thread keeps the buttons in step with
//      the cloaked / un-cloaked state of the frames through ITaskbarList.
//
// DOES NOT do: Start menu or delivered Windows 11 notifications, integration
// with Windows 11 shell components. The Search submenu configures the standard
// Windows 10 per-user taskbar-search mode; the actual search host remains a
// Windows component and is not bundled or injected by this mod.
//
// In short: if anything fails verification the mod does not touch the shell and
// the native shell stays (fail-closed). No shortcut of this mod can leave the
// machine without a shell: there is always a way back to the Windows 11 shell
// (guard, watchdog, hotkey), exactly as in the 4.2.0 mod these parts come from.
// ---------------------------------------------------------------------------

// MSVC-only intrinsic declaration; headers must remain outside namespaces.
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
#include <intrin.h>
#endif

// The mod uses C++ exception boundaries only. It does not register a VEH, use
// Microsoft structured-exception syntax, rewrite CONTEXT records, or claim to recover from native faults.
// Keep this namespace at global scope: Wh_ModInit and AppletGuard use it below.
#include <tlhelp32.h>

namespace CppGuard {

static constexpr DWORD kCppExceptionCode = 0xE06D7363u;
static std::atomic<unsigned int> g_cppExceptionLogs{0};
static std::atomic<unsigned int> g_foreignModsLogged{0};
static std::atomic<bool> g_installed{false};
static ULONGLONG g_selfBase = 0;

static bool IsExactLoadedModIdentifier(const wchar_t* module,
                                       const wchar_t* identifier) noexcept {
    if (!module || !identifier) return false;
    const size_t moduleLength = wcslen(module);
    const size_t identifierLength = wcslen(identifier);
    return (moduleLength == identifierLength &&
            _wcsicmp(module, identifier) == 0) ||
           (moduleLength == identifierLength + 4 &&
            _wcsnicmp(module, identifier, identifierLength) == 0 &&
            _wcsicmp(module + identifierLength, L".whl") == 0);
}

// Preserve the original one-time diagnostics about other Windhawk modules.
static void LogForeignModsOnce() noexcept {
    if (g_foreignModsLogged.fetch_add(1, std::memory_order_acq_rel) > 0) return;
    try {
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
                                               GetCurrentProcessId());
        if (snap == INVALID_HANDLE_VALUE) return;
        MODULEENTRY32W entry = {};
        entry.dwSize = sizeof(entry);
        wchar_t list[600] = {};
        wchar_t incompatible[320] = {};
        size_t used = 0;
        size_t incompatibleUsed = 0;
        int count = 0;
        int incompatibleCount = 0;
        static const wchar_t* const kIntegratedAnixxMods[] = {
            L"win10-taskbar-on-win11-24h2",
            L"fake-explorer-path",
            L"win10-taskbar-context-menu-fix-24h2",
        };
        if (Module32FirstW(snap, &entry)) {
            do {
                if (reinterpret_cast<ULONGLONG>(entry.modBaseAddr) == g_selfBase) continue;
                if (!wcsstr(entry.szExePath, L"\\Windhawk\\Engine\\Mods\\")) continue;
                const size_t len = wcslen(entry.szModule);
                if (!len || used + len + 3 >= _countof(list)) continue;
                if (used) {
                    list[used++] = L',';
                    list[used++] = L' ';
                }
                wmemcpy(list + used, entry.szModule, len);
                used += len;
                list[used] = L'\0';
                count++;
                for (const wchar_t* identifier : kIntegratedAnixxMods) {
                    if (!IsExactLoadedModIdentifier(entry.szModule, identifier)) continue;
                    const size_t identifierLength = wcslen(identifier);
                    if (incompatibleUsed + identifierLength + 3 >= _countof(incompatible)) continue;
                    if (incompatibleUsed) {
                        incompatible[incompatibleUsed++] = L',';
                        incompatible[incompatibleUsed++] = L' ';
                    }
                    wmemcpy(incompatible + incompatibleUsed, identifier, identifierLength);
                    incompatibleUsed += identifierLength;
                    incompatible[incompatibleUsed] = L'\0';
                    incompatibleCount++;
                }
            } while (Module32NextW(snap, &entry));
        }
        CloseHandle(snap);
        if (count)
            Wh_Log(L"[cpp-guard] other Windhawk mods loaded in this process (%d): %s", count, list);
        else
            Wh_Log(L"[cpp-guard] no other Windhawk mod is loaded in this process");
        if (incompatibleCount)
            Wh_Log(L"[cpp-guard] WARNING: integrated Anixx mod(s) also loaded: %s; disable them to avoid duplicate shell hooks", incompatible);
    } catch (...) {
    }
}

static void Install() noexcept {
    if (g_installed.exchange(true, std::memory_order_acq_rel)) return;
    try {
        HMODULE self = nullptr;
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                   GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                               reinterpret_cast<LPCWSTR>(&Install), &self) && self) {
            g_selfBase = reinterpret_cast<ULONGLONG>(self);
        }
        LogForeignModsOnce();
    } catch (...) {
    }
}

// There is no process-wide exception handler to remove.
static void Uninstall() noexcept {}

// Catches only language-level C++ and C++/WinRT exceptions. Hardware/OS
// exceptions are not translated to C++ and must remain unhandled for diagnosis.
static bool RunGuarded(const wchar_t* tag, void (*fn)(void*), void* ctx,
                       DWORD* outFault) noexcept {
    if (outFault) *outFault = 0;
    if (!fn) return false;
    try {
        fn(ctx);
        return true;
    } catch (winrt::hresult_error const& ex) {
        if (outFault) *outFault = kCppExceptionCode;
        if (g_cppExceptionLogs.fetch_add(1, std::memory_order_relaxed) < 8)
            Wh_Log(L"[cpp-guard] %s: C++/WinRT exception (0x%08X)",
                   tag ? tag : L"mod operation", static_cast<unsigned>(ex.code()));
    } catch (std::exception const&) {
        if (outFault) *outFault = kCppExceptionCode;
        if (g_cppExceptionLogs.fetch_add(1, std::memory_order_relaxed) < 8)
            Wh_Log(L"[cpp-guard] %s: std::exception caught", tag ? tag : L"mod operation");
    } catch (...) {
        if (outFault) *outFault = kCppExceptionCode;
        if (g_cppExceptionLogs.fetch_add(1, std::memory_order_relaxed) < 8)
            Wh_Log(L"[cpp-guard] %s: C++ exception caught", tag ? tag : L"mod operation");
    }
    return false;
}

}  // namespace CppGuard

// ===========================================================================
// AppletGuard (1.0.0) - C++ exception boundaries for Control Panel APIs.
// It installs no native OS exception handler and does not claim native-fault recovery.
// These definitions remain at global scope; they install no native exception handler.
// ===========================================================================
namespace AppletGuard {

using PropertySheetW_t = INT_PTR(WINAPI*)(LPCPROPSHEETHEADERW);
using PropertySheetA_t = INT_PTR(WINAPI*)(LPCPROPSHEETHEADERA);
using DialogBoxParamW_t = INT_PTR(WINAPI*)(HINSTANCE, LPCWSTR, HWND, DLGPROC, LPARAM);
using DialogBoxParamA_t = INT_PTR(WINAPI*)(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM);
using DialogBoxIndirectParamW_t = INT_PTR(WINAPI*)(HINSTANCE, LPCDLGTEMPLATEW, HWND, DLGPROC, LPARAM);
using DialogBoxIndirectParamA_t = INT_PTR(WINAPI*)(HINSTANCE, LPCDLGTEMPLATEA, HWND, DLGPROC, LPARAM);
using CreateDialogParamW_t = HWND(WINAPI*)(HINSTANCE, LPCWSTR, HWND, DLGPROC, LPARAM);
using CreateDialogParamA_t = HWND(WINAPI*)(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM);
using CreateDialogIndirectParamW_t = HWND(WINAPI*)(HINSTANCE, LPCDLGTEMPLATEW, HWND, DLGPROC, LPARAM);
using CreateDialogIndirectParamA_t = HWND(WINAPI*)(HINSTANCE, LPCDLGTEMPLATEA, HWND, DLGPROC, LPARAM);
using PeekMessageW_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT, UINT);
using PeekMessageA_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT, UINT);
using GetMessageW_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT);
using GetMessageA_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT);
using DispatchMessageW_t = LRESULT(WINAPI*)(const MSG*);
using DispatchMessageA_t = LRESULT(WINAPI*)(const MSG*);
using IsDialogMessageW_t = BOOL(WINAPI*)(HWND, LPMSG);
using IsDialogMessageA_t = BOOL(WINAPI*)(HWND, LPMSG);

static PropertySheetW_t PropertySheetW_Original = nullptr;
static PropertySheetA_t PropertySheetA_Original = nullptr;
static DialogBoxParamW_t DialogBoxParamW_Original = nullptr;
static DialogBoxParamA_t DialogBoxParamA_Original = nullptr;
static DialogBoxIndirectParamW_t DialogBoxIndirectParamW_Original = nullptr;
static DialogBoxIndirectParamA_t DialogBoxIndirectParamA_Original = nullptr;
static CreateDialogParamW_t CreateDialogParamW_Original = nullptr;
static CreateDialogParamA_t CreateDialogParamA_Original = nullptr;
static CreateDialogIndirectParamW_t CreateDialogIndirectParamW_Original = nullptr;
static CreateDialogIndirectParamA_t CreateDialogIndirectParamA_Original = nullptr;
static PeekMessageW_t PeekMessageW_Original = nullptr;
static PeekMessageA_t PeekMessageA_Original = nullptr;
static GetMessageW_t GetMessageW_Original = nullptr;
static GetMessageA_t GetMessageA_Original = nullptr;
static DispatchMessageW_t DispatchMessageW_Original = nullptr;
static DispatchMessageA_t DispatchMessageA_Original = nullptr;
static IsDialogMessageW_t IsDialogMessageW_Original = nullptr;
static IsDialogMessageA_t IsDialogMessageA_Original = nullptr;
static std::atomic<bool> g_installed{false};
static std::atomic<unsigned int> g_cppExceptionLogs{0};

// C++ exception boundaries for applet APIs; no native-fault recovery is installed.
static void ReportCppException(const wchar_t* api) noexcept {
    const unsigned int count = g_cppExceptionLogs.fetch_add(1, std::memory_order_relaxed);
    if (count < 8) {
        Wh_Log(L"[applet-guard] C++ exception crossed %s; operation cancelled", api);
    } else if (count == 8) {
        Wh_Log(L"[applet-guard] further C++ exception messages suppressed");
    }
}

// Nome del processo host: e' una impostazione (guardHosts) perche' un altro
// applet puo' vivere in un processo diverso (per esempio mmc.exe).
static bool IsHostProcess(PCWSTR exe) noexcept {
    if (!exe) return false;
    try {
        string_setting_unique_ptr hosts(Wh_GetStringSetting(L"guardHosts"));
        if (!hosts.get() || !*hosts.get()) return false;
        bool match = false;
        const wchar_t* p = hosts.get();
        while (*p) {
            while (*p == L' ' || *p == L'\t' || *p == L';' || *p == L',') ++p;
            const wchar_t* start = p;
            while (*p && *p != L';' && *p != L',') ++p;
            size_t len = static_cast<size_t>(p - start);
            while (len && (start[len - 1] == L' ' || start[len - 1] == L'\t')) --len;
            if (len && len < 64) {
                wchar_t name[64] = {};
                wmemcpy(name, start, len);
                if (_wcsicmp(name, exe) == 0) {
                    match = true;
                    break;
                }
            }
        }
        return match;
    } catch (...) {
        Wh_Log(L"[applet-guard] guardHosts read raised a C++ exception; this process is not treated as an applet host");
        return false;
    }
}

template <typename Result, typename Call>
static Result InvokeGuardedAppletApi(const wchar_t* operation, const wchar_t* api,
                                    Result failure, Call&& call) {
    (void)operation;
    try {
        return call();
    } catch (...) {
        ReportCppException(api);
        return failure;
    }
}

static INT_PTR WINAPI PropertySheetW_Hook(LPCPROPSHEETHEADERW header) {
    if (!PropertySheetW_Original) return -1;
    return InvokeGuardedAppletApi<INT_PTR>(L"property sheet of a Control Panel applet", L"PropertySheetW", -1,
        [&] { return PropertySheetW_Original(header); });
}

static INT_PTR WINAPI PropertySheetA_Hook(LPCPROPSHEETHEADERA header) {
    if (!PropertySheetA_Original) return -1;
    return InvokeGuardedAppletApi<INT_PTR>(L"property sheet of a Control Panel applet (ANSI)", L"PropertySheetA", -1,
        [&] { return PropertySheetA_Original(header); });
}

static INT_PTR WINAPI DialogBoxParamW_Hook(HINSTANCE instance, LPCWSTR templ, HWND parent,
                                           DLGPROC proc, LPARAM param) {
    if (!DialogBoxParamW_Original) return -1;
    return InvokeGuardedAppletApi<INT_PTR>(L"dialog of a Control Panel applet", L"DialogBoxParamW", -1,
        [&] { return DialogBoxParamW_Original(instance, templ, parent, proc, param); });
}

static INT_PTR WINAPI DialogBoxParamA_Hook(HINSTANCE instance, LPCSTR templ, HWND parent,
                                           DLGPROC proc, LPARAM param) {
    if (!DialogBoxParamA_Original) return -1;
    return InvokeGuardedAppletApi<INT_PTR>(L"dialog of a Control Panel applet (ANSI)", L"DialogBoxParamA", -1,
        [&] { return DialogBoxParamA_Original(instance, templ, parent, proc, param); });
}

static INT_PTR WINAPI DialogBoxIndirectParamW_Hook(HINSTANCE instance, LPCDLGTEMPLATEW templ,
                                                   HWND parent, DLGPROC proc, LPARAM param) {
    if (!DialogBoxIndirectParamW_Original) return -1;
    return InvokeGuardedAppletApi<INT_PTR>(L"dialog of a Control Panel applet", L"DialogBoxIndirectParamW", -1,
        [&] { return DialogBoxIndirectParamW_Original(instance, templ, parent, proc, param); });
}

static INT_PTR WINAPI DialogBoxIndirectParamA_Hook(HINSTANCE instance, LPCDLGTEMPLATEA templ,
                                                   HWND parent, DLGPROC proc, LPARAM param) {
    if (!DialogBoxIndirectParamA_Original) return -1;
    return InvokeGuardedAppletApi<INT_PTR>(L"dialog of a Control Panel applet (ANSI)", L"DialogBoxIndirectParamA", -1,
        [&] { return DialogBoxIndirectParamA_Original(instance, templ, parent, proc, param); });
}

static HWND WINAPI CreateDialogParamW_Hook(HINSTANCE instance, LPCWSTR templ, HWND parent,
                                            DLGPROC proc, LPARAM param) {
    if (!CreateDialogParamW_Original) return nullptr;
    return InvokeGuardedAppletApi<HWND>(L"modeless dialog of a Control Panel applet", L"CreateDialogParamW", nullptr,
        [&] { return CreateDialogParamW_Original(instance, templ, parent, proc, param); });
}

static HWND WINAPI CreateDialogParamA_Hook(HINSTANCE instance, LPCSTR templ, HWND parent,
                                            DLGPROC proc, LPARAM param) {
    if (!CreateDialogParamA_Original) return nullptr;
    return InvokeGuardedAppletApi<HWND>(L"modeless dialog of a Control Panel applet (ANSI)", L"CreateDialogParamA", nullptr,
        [&] { return CreateDialogParamA_Original(instance, templ, parent, proc, param); });
}

static HWND WINAPI CreateDialogIndirectParamW_Hook(HINSTANCE instance, LPCDLGTEMPLATEW templ,
                                                    HWND parent, DLGPROC proc, LPARAM param) {
    if (!CreateDialogIndirectParamW_Original) return nullptr;
    return InvokeGuardedAppletApi<HWND>(L"modeless dialog of a Control Panel applet", L"CreateDialogIndirectParamW", nullptr,
        [&] { return CreateDialogIndirectParamW_Original(instance, templ, parent, proc, param); });
}

static HWND WINAPI CreateDialogIndirectParamA_Hook(HINSTANCE instance, LPCDLGTEMPLATEA templ,
                                                    HWND parent, DLGPROC proc, LPARAM param) {
    if (!CreateDialogIndirectParamA_Original) return nullptr;
    return InvokeGuardedAppletApi<HWND>(L"modeless dialog of a Control Panel applet (ANSI)", L"CreateDialogIndirectParamA", nullptr,
        [&] { return CreateDialogIndirectParamA_Original(instance, templ, parent, proc, param); });
}

// I messaggi che il sistema consegna direttamente a una finestra dell'applet
// arrivano mentre il thread e' dentro PeekMessage/GetMessage: la sezione
// protetta copre anche quel percorso.
static BOOL WINAPI PeekMessageW_Hook(LPMSG msg, HWND hwnd, UINT first, UINT last, UINT remove) {
    if (!PeekMessageW_Original) return FALSE;
    return InvokeGuardedAppletApi<BOOL>(L"message delivered to a Control Panel applet", L"PeekMessageW", FALSE,
        [&] { return PeekMessageW_Original(msg, hwnd, first, last, remove); });
}

static BOOL WINAPI PeekMessageA_Hook(LPMSG msg, HWND hwnd, UINT first, UINT last, UINT remove) {
    if (!PeekMessageA_Original) return FALSE;
    return InvokeGuardedAppletApi<BOOL>(L"message delivered to a Control Panel applet (ANSI)", L"PeekMessageA", FALSE,
        [&] { return PeekMessageA_Original(msg, hwnd, first, last, remove); });
}

static BOOL WINAPI GetMessageW_Hook(LPMSG msg, HWND hwnd, UINT first, UINT last) {
    if (!GetMessageW_Original) return FALSE;
    return InvokeGuardedAppletApi<BOOL>(L"message delivered to a Control Panel applet", L"GetMessageW", FALSE,
        [&] { return GetMessageW_Original(msg, hwnd, first, last); });
}

static BOOL WINAPI GetMessageA_Hook(LPMSG msg, HWND hwnd, UINT first, UINT last) {
    if (!GetMessageA_Original) return FALSE;
    return InvokeGuardedAppletApi<BOOL>(L"message delivered to a Control Panel applet (ANSI)", L"GetMessageA", FALSE,
        [&] { return GetMessageA_Original(msg, hwnd, first, last); });
}

static LRESULT WINAPI DispatchMessageW_Hook(const MSG* msg) {
    if (!DispatchMessageW_Original) return 0;
    return InvokeGuardedAppletApi<LRESULT>(L"message dispatched to a Control Panel applet", L"DispatchMessageW", 0,
        [&] { return DispatchMessageW_Original(msg); });
}

static LRESULT WINAPI DispatchMessageA_Hook(const MSG* msg) {
    if (!DispatchMessageA_Original) return 0;
    return InvokeGuardedAppletApi<LRESULT>(L"message dispatched to a Control Panel applet (ANSI)", L"DispatchMessageA", 0,
        [&] { return DispatchMessageA_Original(msg); });
}

static BOOL WINAPI IsDialogMessageW_Hook(HWND dialog, LPMSG msg) {
    if (!IsDialogMessageW_Original) return FALSE;
    return InvokeGuardedAppletApi<BOOL>(L"dialog message dispatched to a Control Panel applet", L"IsDialogMessageW", FALSE,
        [&] { return IsDialogMessageW_Original(dialog, msg); });
}

static BOOL WINAPI IsDialogMessageA_Hook(HWND dialog, LPMSG msg) {
    if (!IsDialogMessageA_Original) return FALSE;
    return InvokeGuardedAppletApi<BOOL>(L"dialog message dispatched to a Control Panel applet (ANSI)", L"IsDialogMessageA", FALSE,
        [&] { return IsDialogMessageA_Original(dialog, msg); });
}

// `original` is the address of the slot Windhawk fills during installation.
// It is normally null before the hook is installed, so test the slot pointer,
// not `*original`; testing the latter silently skipped every AppletGuard hook.
static bool InstallHook(void* target, void* hook, void** original) {
    if (!target || !hook || !original) return false;
    return Wh_SetFunctionHook(target, hook, original);
}

static bool Install() noexcept {
    if (g_installed.exchange(true)) return true;
    try {
        CppGuard::Install();
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        HMODULE comctl32 = GetModuleHandleW(L"comctl32.dll");
        if (!user32) {
            Wh_Log(L"[applet-guard] user32.dll is not loaded: the applet host stays unguarded");
            g_installed.store(false);
            CppGuard::Uninstall();          // no process-wide exception handler is registered
            return false;
        }

        // Le funzioni possono mancare o un hook puo' fallire: misuriamo ogni esito.
        // Un controllo dei soli originali prima della chiamata impedirebbe
        // l'installazione: Windhawk li valorizza soltanto quando l'hook riesce.
        bool sheetW = false, sheetA = false;
        if (comctl32) {
            sheetW = InstallHook(reinterpret_cast<void*>(GetProcAddress(comctl32, "PropertySheetW")),
                                 reinterpret_cast<void*>(PropertySheetW_Hook),
                                 reinterpret_cast<void**>(&PropertySheetW_Original)) &&
                     PropertySheetW_Original != nullptr;
            sheetA = InstallHook(reinterpret_cast<void*>(GetProcAddress(comctl32, "PropertySheetA")),
                                 reinterpret_cast<void*>(PropertySheetA_Hook),
                                 reinterpret_cast<void**>(&PropertySheetA_Original)) &&
                     PropertySheetA_Original != nullptr;
        }
        const bool dialogParamW =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "DialogBoxParamW")),
                        reinterpret_cast<void*>(DialogBoxParamW_Hook),
                        reinterpret_cast<void**>(&DialogBoxParamW_Original)) &&
            DialogBoxParamW_Original != nullptr;
        const bool dialogParamA =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "DialogBoxParamA")),
                        reinterpret_cast<void*>(DialogBoxParamA_Hook),
                        reinterpret_cast<void**>(&DialogBoxParamA_Original)) &&
            DialogBoxParamA_Original != nullptr;
        const bool dialogIndirectW =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "DialogBoxIndirectParamW")),
                        reinterpret_cast<void*>(DialogBoxIndirectParamW_Hook),
                        reinterpret_cast<void**>(&DialogBoxIndirectParamW_Original)) &&
            DialogBoxIndirectParamW_Original != nullptr;
        const bool dialogIndirectA =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "DialogBoxIndirectParamA")),
                        reinterpret_cast<void*>(DialogBoxIndirectParamA_Hook),
                        reinterpret_cast<void**>(&DialogBoxIndirectParamA_Original)) &&
            DialogBoxIndirectParamA_Original != nullptr;
        const bool createDialogW =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "CreateDialogParamW")),
                        reinterpret_cast<void*>(CreateDialogParamW_Hook),
                        reinterpret_cast<void**>(&CreateDialogParamW_Original)) &&
            CreateDialogParamW_Original != nullptr;
        const bool createDialogA =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "CreateDialogParamA")),
                        reinterpret_cast<void*>(CreateDialogParamA_Hook),
                        reinterpret_cast<void**>(&CreateDialogParamA_Original)) &&
            CreateDialogParamA_Original != nullptr;
        const bool createDialogIndirectW =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "CreateDialogIndirectParamW")),
                        reinterpret_cast<void*>(CreateDialogIndirectParamW_Hook),
                        reinterpret_cast<void**>(&CreateDialogIndirectParamW_Original)) &&
            CreateDialogIndirectParamW_Original != nullptr;
        const bool createDialogIndirectA =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "CreateDialogIndirectParamA")),
                        reinterpret_cast<void*>(CreateDialogIndirectParamA_Hook),
                        reinterpret_cast<void**>(&CreateDialogIndirectParamA_Original)) &&
            CreateDialogIndirectParamA_Original != nullptr;
        const bool peekMessageW =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "PeekMessageW")),
                        reinterpret_cast<void*>(PeekMessageW_Hook),
                        reinterpret_cast<void**>(&PeekMessageW_Original)) &&
            PeekMessageW_Original != nullptr;
        const bool peekMessageA =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "PeekMessageA")),
                        reinterpret_cast<void*>(PeekMessageA_Hook),
                        reinterpret_cast<void**>(&PeekMessageA_Original)) &&
            PeekMessageA_Original != nullptr;
        const bool getMessageW =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "GetMessageW")),
                        reinterpret_cast<void*>(GetMessageW_Hook),
                        reinterpret_cast<void**>(&GetMessageW_Original)) &&
            GetMessageW_Original != nullptr;
        const bool getMessageA =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "GetMessageA")),
                        reinterpret_cast<void*>(GetMessageA_Hook),
                        reinterpret_cast<void**>(&GetMessageA_Original)) &&
            GetMessageA_Original != nullptr;
        const bool dispatchMessageW =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "DispatchMessageW")),
                        reinterpret_cast<void*>(DispatchMessageW_Hook),
                        reinterpret_cast<void**>(&DispatchMessageW_Original)) &&
            DispatchMessageW_Original != nullptr;
        const bool dispatchMessageA =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "DispatchMessageA")),
                        reinterpret_cast<void*>(DispatchMessageA_Hook),
                        reinterpret_cast<void**>(&DispatchMessageA_Original)) &&
            DispatchMessageA_Original != nullptr;
        const bool isDialogMessageW =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "IsDialogMessageW")),
                        reinterpret_cast<void*>(IsDialogMessageW_Hook),
                        reinterpret_cast<void**>(&IsDialogMessageW_Original)) &&
            IsDialogMessageW_Original != nullptr;
        const bool isDialogMessageA =
            InstallHook(reinterpret_cast<void*>(GetProcAddress(user32, "IsDialogMessageA")),
                        reinterpret_cast<void*>(IsDialogMessageA_Hook),
                        reinterpret_cast<void**>(&IsDialogMessageA_Original)) &&
            IsDialogMessageA_Original != nullptr;

        Wh_Log(L"[applet-guard] hooks: PropertySheetW=%d PropertySheetA=%d "
               L"DialogBoxParamW=%d DialogBoxParamA=%d DialogBoxIndirectParamW=%d DialogBoxIndirectParamA=%d "
               L"CreateDialogParamW=%d CreateDialogParamA=%d "
               L"CreateDialogIndirectParamW=%d CreateDialogIndirectParamA=%d "
               L"PeekMessageW=%d PeekMessageA=%d GetMessageW=%d GetMessageA=%d "
               L"DispatchMessageW=%d DispatchMessageA=%d IsDialogMessageW=%d IsDialogMessageA=%d",
               sheetW, sheetA, dialogParamW, dialogParamA, dialogIndirectW, dialogIndirectA,
               createDialogW, createDialogA, createDialogIndirectW, createDialogIndirectA,
               peekMessageW, peekMessageA, getMessageW, getMessageA, dispatchMessageW,
               dispatchMessageA, isDialogMessageW, isDialogMessageA);
        if (!(sheetW || sheetA || dialogParamW || dialogParamA || dialogIndirectW || dialogIndirectA ||
              createDialogW || createDialogA || createDialogIndirectW || createDialogIndirectA ||
              peekMessageW || peekMessageA || getMessageW || getMessageA || dispatchMessageW ||
              dispatchMessageA || isDialogMessageW || isDialogMessageA)) {
            Wh_Log(L"[applet-guard] no API hook installed: guard-only load aborted");
            g_installed.store(false);
            CppGuard::Uninstall();
            return false;
        }
        return true;
    } catch (...) {
        Wh_Log(L"[applet-guard] C++ exception while installing the Control Panel boundaries; guard-only load aborted");
        g_installed.store(false);
        CppGuard::Uninstall();
        return false;
    }
}

static bool Installed() noexcept { return g_installed.load(); }

static void Uninstall() noexcept {
    if (!g_installed.exchange(false)) return;
    PropertySheetW_Original = nullptr;
    PropertySheetA_Original = nullptr;
    DialogBoxParamW_Original = nullptr;
    DialogBoxParamA_Original = nullptr;
    DialogBoxIndirectParamW_Original = nullptr;
    DialogBoxIndirectParamA_Original = nullptr;
    CreateDialogParamW_Original = nullptr;
    CreateDialogParamA_Original = nullptr;
    CreateDialogIndirectParamW_Original = nullptr;
    CreateDialogIndirectParamA_Original = nullptr;
    PeekMessageW_Original = nullptr;
    PeekMessageA_Original = nullptr;
    GetMessageW_Original = nullptr;
    GetMessageA_Original = nullptr;
    DispatchMessageW_Original = nullptr;
    DispatchMessageA_Original = nullptr;
    IsDialogMessageW_Original = nullptr;
    IsDialogMessageA_Original = nullptr;
    CppGuard::Uninstall();
}

}  // namespace AppletGuard

namespace RestorerTaskbar {

// Definita in coda al namespace: qui serve al blocco tray che la chiama prima.
static bool IsLegacyShellProcess();

#if defined(__clang__) || defined(__GNUC__)
#define WhReturnAddress() __builtin_return_address(0)
#elif defined(_MSC_VER)
#define WhReturnAddress() ::_ReturnAddress()
#else
#error Unsupported compiler for WhReturnAddress
#endif
// header moved to global include section
// header moved to global include section
// header moved to global include section

// ===========================================================================
// CONFIGURAZIONE
// ===========================================================================

struct BuildInfo {
    const wchar_t* label;
    const wchar_t* symbolId;      // <timestamp><SizeOfImage>, as on the symbol server
    const wchar_t* sha256;        // real hash of the file served by msdl (verified)
    DWORD timeDateStamp;          // gate di build
    DWORD sizeOfImage;
    DWORD notificationOffset;     // 0 = offset unknown for this build
    DWORD shellManagedOffset;     // ShouldTreatShellManagedWindowAsNotShellManaged (0 = unknown)
};

static const BuildInfo kBuilds[] = {
    // 10.0.19039.1 — used by the official mod and by the community fixes
    { L"10.0.19039.1", L"7AC6EEC3442000", L"58f78b5f90efc75d6c7d3d85bc8b36983fe410406f217619dbe2384130d65bfe", 0x7AC6EEC3, 0x442000, 0x14DC40, 0x4FEA8 },
    // 10.0.19041.7725 (KB5122878, September 2026) — more recent; the log says which is installed
    { L"10.0.19041.7725", L"7A77DC0C5c9000", L"c493059ca065b0780ce5a430ade23f74c79c9578ec49c1fb18d7ad3f81dfec5e", 0x7A77DC0C, 0x5C9000, 0, 0x5F0E0 },
};
static const int kBuildCount = _countof(kBuilds);

// Extra legacy files. windows.ui.search.dll is deliberately not loaded: the
// Search submenu below changes the standard SearchboxTaskbarMode setting through
// public Win32 APIs and does not inject a separate Search host. Its verified
// identity is retained for reference: id = 4D6D1C59e5000,
// sha256 = 8950639236b5000973ff14f7a57577066f4c075a52e8582fe11b8eb75526522e.
struct ExtraFile {
    const wchar_t* name;
    const wchar_t* symbolId;
    const wchar_t* sha256;
};
static const ExtraFile kTrayFiles[] = {
    { L"pnidui.dll",  L"CC2D6BBC219000", L"7c8fa315e73e22c0d66c1b424118e3441251fe3c0e2b557e4dbf16166d14411c" },
    { L"stobject.dll", L"465AE25A52000", L"7c0037535c4da20ae15b4df9662f9330a989d4e2f36284aac5c9c3bce429e118" },
};

static const wchar_t* kSymbolUrlFmt = L"https://msdl.microsoft.com/download/symbols/%s/%s/%s";

static struct {
    int  buildIndex;
    wchar_t storePath[MAX_PATH];
    wchar_t explorerPath[MAX_PATH];
    bool provideTrayDlls;
    bool requireSignature;
    bool fixContextMenu;

    bool winXMenuAnchorCursor;   // 1.0.0: legacy cursor anchor instead of the Start button
    int  winXMenuOffsetX;        // 1.0.0: shift of the resolved anchor point (px)
    int  winXMenuOffsetY;
    bool fixNotificationsCrash;
    bool fixUwpTaskbar;          // UWP apps (ApplicationFrameWindow) on the taskbar
    bool spoofExplorerPath;
    bool perUserShellRedirect;
    bool shellRedirectHook;
    bool restoreOnUnload;
    bool emergencyHotkey;
    int  liveSwitch;             // 0 none, 1 to-legacy, 2 to-win11
    bool watchdog;
    bool notificationsOff;          // virtual read fallback when the crash hook is unavailable
    bool languageGuard;          // sezione 9: sopprime il flyout di lingua all'avvio
    int  downloadTimeoutSec;
    bool tightenStoreAcl;
    bool forceWin11StartLeft;
    bool xamlRestyle;            // Win10 graphical restyle injected into the native flyout (Win10Restyle)
    // 1.0.0
    bool forceNetworkTrayIcon;             // registra l'icona di rete se la PNI nativa non lo fa
    int  forceNetworkTrayDelaySec;         // attesa prima dell'intervento (s)
    bool forceNetworkTrayResetTraySettings;// azzera una volta lo stato della barra (con backup)
    bool trayRestoreOverflowChevron;       // 1.0.0: rimuove l'override virtuale per rivedere il pulsante "^"
    bool actionCenterAnimation;            // movimento del pannello del centro operativo
    int  actionCenterAnimInMs;             // durata apertura
    int  actionCenterAnimOutMs;            // durata chiusura
    int  actionCenterCloseDelayMs;         // ritardo di chiusura al posto dei ~2000 ms
    int  shellOpGuardTimeoutMs;            // tetto di tempo per le operazioni shell:::
} g_cfg = {};

static bool g_userinitReady = false;   // tutti i file verificati → possiamo reindirizzare
static wchar_t g_realExePath[MAX_PATH] = {};   // real path, captured before the hooks
static DWORD g_explorerTs = 0, g_explorerImageSize = 0;

// ===========================================================================
// RAII
// ===========================================================================

class ScopedHandle {
public:
    explicit ScopedHandle(HANDLE h = nullptr) : m_h(h) {}
    ~ScopedHandle() { reset(); }
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
    HANDLE get() const { return m_h; }
    bool valid() const { return m_h != nullptr && m_h != INVALID_HANDLE_VALUE; }
    void reset(HANDLE h = nullptr) {
        if (valid()) CloseHandle(m_h);
        m_h = h;
    }
private:
    HANDLE m_h;
};

class ScopedRegKey {
public:
    ScopedRegKey() = default;
    ~ScopedRegKey() { reset(); }
    ScopedRegKey(const ScopedRegKey&) = delete;
    ScopedRegKey& operator=(const ScopedRegKey&) = delete;
    HKEY* put() { reset(); return &m_h; }
    HKEY get() const { return m_h; }
    HKEY release() noexcept { HKEY h = m_h; m_h = nullptr; return h; }
    bool valid() const { return m_h != nullptr; }
    void reset() { if (m_h) { RegCloseKey(m_h); m_h = nullptr; } }
private:
    HKEY m_h = nullptr;
};

class ScopedGdiObj {
public:
    explicit ScopedGdiObj(HGDIOBJ o = nullptr) : m_o(o) {}
    ~ScopedGdiObj() { if (m_o) DeleteObject(m_o); }
    ScopedGdiObj(const ScopedGdiObj&) = delete;
    ScopedGdiObj& operator=(const ScopedGdiObj&) = delete;
    HGDIOBJ get() const { return m_o; }
private:
    HGDIOBJ m_o;
};

class ScopedMenu {
public:
    explicit ScopedMenu(HMENU menu = nullptr) : m_menu(menu) {}
    ~ScopedMenu() { if (m_menu) DestroyMenu(m_menu); }
    ScopedMenu(const ScopedMenu&) = delete;
    ScopedMenu& operator=(const ScopedMenu&) = delete;
    HMENU get() const { return m_menu; }
    HMENU release() { HMENU menu = m_menu; m_menu = nullptr; return menu; }
private:
    HMENU m_menu;
};

class ScopedInternet {
public:
    explicit ScopedInternet(HINTERNET h = nullptr) : m_h(h) {}
    ~ScopedInternet() { if (m_h) InternetCloseHandle(m_h); }
    ScopedInternet(const ScopedInternet&) = delete;
    ScopedInternet& operator=(const ScopedInternet&) = delete;
    HINTERNET get() const { return m_h; }
    bool valid() const { return m_h != nullptr; }
private:
    HINTERNET m_h;
};

// Restores the previously selected GDI object (SelectObject is sticky).
class ScopedSelectedObject {
public:
    ScopedSelectedObject(HDC hdc, HGDIOBJ obj) : m_hdc(hdc), m_old(nullptr) {
        if (obj) m_old = SelectObject(hdc, obj);
    }
    ~ScopedSelectedObject() { if (m_old && m_old != HGDI_ERROR) SelectObject(m_hdc, m_old); }
    ScopedSelectedObject(const ScopedSelectedObject&) = delete;
    ScopedSelectedObject& operator=(const ScopedSelectedObject&) = delete;
private:
    HDC m_hdc;
    HGDIOBJ m_old;
};

// ===========================================================================

// Ribbon: adapted from m417z, explorer-frame-classic 1.0.0@@KEEP@@ (GPL-3.0).
// Alt+Tab: independently integrated native-host routing. The technique is the one
// documented by ExplorerPatcher (valinet / Amrsatrio, TwinUIPatches.cpp, commit
// 0a88a6e0ef6b1752fea36e581cffff1097e862b0), credited as a reference for the idea;
// no code is copied and no SimpleWindowSwitcher is bundled.
// These boundaries handle C++ exceptions; no native-fault recovery is installed.
// ===========================================================================
static std::atomic<bool> g_notificationCrashFixActive{false};
static std::atomic<bool> g_notificationPolicyReady{false};

namespace NativeUi {
static std::atomic<bool> actionCenter{true};
static std::atomic<bool> ribbon10{true};
static bool registryProcess = false;
static bool explorerProcess = false;
static bool privateExplorer = false;
// Only the private legacy Explorer and the two shell hosts can create/read the
// Action Center policy in the processes into which this mod is injected.
static bool actionCenterPolicyProcess = false;
static std::atomic<bool> stopping{false};

static void LoadSettings() noexcept {
    // Each option fails independently; StringSetting owns the Windhawk string.
    try {
        actionCenter.store(Wh_GetIntSetting(L"Win10ActionCenter") != 0);
    } catch (...) { Wh_Log(L"[action-center] settings exception; keeping previous option"); }
}

// Exact registry paths, including hive. Never override an unrelated value merely
// because it has the same name. NtQueryKey also recognizes handles opened before
// this mod, so no global bookkeeping of ordinary registry handles is necessary.
/*static constexpr wchar_t kControlSubkey[] =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Control Center";*/
static constexpr wchar_t kControlNative[] =
    L"\\REGISTRY\\MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Control Center";
static constexpr wchar_t kExplorerSubkey[] =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer";
static constexpr wchar_t kNotificationPolicySubkey[] =
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer";
using NtQueryKeyFn = LONG(NTAPI*)(HANDLE, int, void*, ULONG, ULONG*);
static NtQueryKeyFn ntQueryKey = nullptr;
static std::wstring currentUserNative;
static std::mutex proxyMutex;
static std::unordered_set<HKEY> proxyKeys;
static decltype(&RegOpenKeyExW) openKeyOriginal = nullptr;
static decltype(&RegCloseKey) closeKeyOriginal = nullptr;

static bool IsProxy(HKEY key) {
    std::lock_guard<std::mutex> lock(proxyMutex);
    return proxyKeys.find(key) != proxyKeys.end();
}

static std::wstring KeyPath(HKEY key) {
    if (key == HKEY_LOCAL_MACHINE) return L"\\REGISTRY\\MACHINE";
    if (key == HKEY_CURRENT_USER) return currentUserNative;
    if (!ntQueryKey || !key) return {};
    struct KeyNameBuffer { ULONG length; wchar_t name[2048]; } buffer{};
    ULONG required = 0;
    // KeyNameInformation == 3. No native link dependency.
    if (ntQueryKey(key, 3, &buffer, sizeof(buffer), &required) < 0 ||
        buffer.length > sizeof(buffer.name) || buffer.length % sizeof(wchar_t))
        return {};
    return std::wstring(buffer.name, buffer.length / sizeof(wchar_t));
}

static std::wstring FullPath(HKEY key, LPCWSTR subkey) {
    auto path = KeyPath(key);
    if (path.empty()) return {};
    if (subkey && *subkey) {
        path += L'\\';
        path += subkey;
    }
    while (!path.empty() && path.back() == L'\\') path.pop_back();
    return path;
}

static bool IsControlPath(HKEY key, LPCWSTR subkey) {
    return _wcsicmp(FullPath(key, subkey).c_str(), kControlNative) == 0;
}

static bool IsExplorerPath(HKEY key, LPCWSTR subkey) {
    if (currentUserNative.empty()) return false;
    auto expected = currentUserNative + L"\\" + kExplorerSubkey;
    return _wcsicmp(FullPath(key, subkey).c_str(), expected.c_str()) == 0;
}

static bool IsNotificationPolicyPath(HKEY key, LPCWSTR subkey) {
    if (currentUserNative.empty()) return false;
    auto expected = currentUserNative + L"\\" + kNotificationPolicySubkey;
    return _wcsicmp(FullPath(key, subkey).c_str(), expected.c_str()) == 0;
}

// Win32 DWORD buffer contract, including size-only requests and ZEROONFAILURE.
// Kept separate from the old Search helper to avoid changing its behavior.
static LSTATUS CopyDword(DWORD value, bool getValue, DWORD flags,
                         LPDWORD type, void* data, LPDWORD bytes) noexcept {
    const DWORD capacity = bytes ? *bytes : 0;
    auto fail = [&](LSTATUS error) noexcept {
        if (getValue && (flags & RRF_ZEROONFAILURE) && data && bytes && capacity)
            memset(data, 0, capacity);
        return error;
    };
    if (data && !bytes) return ERROR_INVALID_PARAMETER;
    if (getValue) {
        constexpr DWORD allowed = RRF_RT_ANY | RRF_NOEXPAND | RRF_ZEROONFAILURE |
                                  RRF_SUBKEY_WOW6432KEY | RRF_SUBKEY_WOW6464KEY;
        if ((flags & ~allowed) ||
            ((flags & RRF_SUBKEY_WOW6432KEY) && (flags & RRF_SUBKEY_WOW6464KEY)))
            return fail(ERROR_INVALID_PARAMETER);
    }
    if (type) *type = REG_DWORD;
    if (bytes) *bytes = sizeof(DWORD);
    const DWORD filter = flags & RRF_RT_ANY;
    if (getValue && filter && !(filter & RRF_RT_REG_DWORD))
        return fail(ERROR_UNSUPPORTED_TYPE);
    if (!data) return ERROR_SUCCESS;
    if (capacity < sizeof(DWORD)) return fail(ERROR_MORE_DATA);
    memcpy(data, &value, sizeof(value));
    return ERROR_SUCCESS;
}

static bool TryRead(HKEY key, LPCWSTR subkey, LPCWSTR name, bool getValue,
                    DWORD flags, LPDWORD type, void* data, LPDWORD bytes,
                    LSTATUS* result) noexcept {
    try {
        if (!registryProcess || stopping.load()) return false;
        // Proxies represent only a missing, read-only Control Center key.
        const bool proxy = IsProxy(key);
        if (proxy && ((subkey && *subkey) || !name ||
                      _wcsicmp(name, L"UseLiteLayout") != 0 || !actionCenter.load())) {
            *result = ERROR_FILE_NOT_FOUND;
            if (getValue && (flags & RRF_ZEROONFAILURE) && data && bytes)
                memset(data, 0, *bytes);
            return true;
        }
        if (!name) return false;
        if (actionCenter.load() && _wcsicmp(name, L"UseLiteLayout") == 0 &&
            (proxy || IsControlPath(key, subkey))) {
            *result = CopyDword(1, getValue, flags, type, data, bytes);
            static std::atomic<bool> reported{false};
            if (!reported.exchange(true))
                Wh_Log(L"[action-center] serving virtual HKLM Control Center / UseLiteLayout = 1 (not a registry write)");
            return true;
        }
        if (actionCenterPolicyProcess &&
            g_notificationPolicyReady.load(std::memory_order_acquire) &&
            g_cfg.notificationsOff &&
            !g_notificationCrashFixActive.load(std::memory_order_acquire) &&
            _wcsicmp(name, L"DisableNotificationCenter") == 0 &&
            IsNotificationPolicyPath(key, subkey)) {
            *result = CopyDword(1, getValue, flags, type, data, bytes);
            static std::atomic<bool> reported{false};
            if (!reported.exchange(true))
                Wh_Log(L"[notif] serving virtual DisableNotificationCenter=1 in this Action Center host (not a registry write)");
            return true;
        }
    } catch (...) {
        Wh_Log(L"[native-ui] registry read exception; using the real registry");
    }
    return false;
}

static LSTATUS WINAPI OpenKeyHook(HKEY key, LPCWSTR subkey, DWORD options,
                                  REGSAM access, PHKEY output) {
    try {
        const bool proxy = registryProcess && IsProxy(key);
        const bool target = registryProcess && !stopping.load() && actionCenter.load() &&
                            (proxy ? (!subkey || !*subkey) : IsControlPath(key, subkey));
        if (proxy && !target) return ERROR_FILE_NOT_FOUND;
        if (target && output && options == 0 &&
            !(access & ~(KEY_READ | KEY_WOW64_32KEY | KEY_WOW64_64KEY)) &&
            (access & (KEY_WOW64_32KEY | KEY_WOW64_64KEY)) !=
                (KEY_WOW64_32KEY | KEY_WOW64_64KEY)) {
            LSTATUS status = proxy ? ERROR_FILE_NOT_FOUND :
                openKeyOriginal(key, subkey, options, access, output);
            if (status != ERROR_FILE_NOT_FOUND && status != ERROR_PATH_NOT_FOUND)
                return status;  // Never bypass ACCESS_DENIED.
            // A genuine read-only handle is returned, never a made-up HKEY.
            // Its ownership passes to the caller. The proxy is only a projection
            // for OpenKeyExW / QueryValueExW / GetValueW / CloseKey, not a full hive.
            ScopedRegKey backing;
            status = openKeyOriginal(HKEY_LOCAL_MACHINE, L"SOFTWARE", 0,
                                     KEY_QUERY_VALUE, backing.put());
            if (status != ERROR_SUCCESS) return status;
            {
                std::lock_guard<std::mutex> lock(proxyMutex);
                proxyKeys.insert(backing.get());
            }
            *output = backing.release();
            return ERROR_SUCCESS;
        }
        if (proxy) return ERROR_ACCESS_DENIED;  // Never grant write access.
    } catch (...) {
        Wh_Log(L"[action-center] virtual key open exception");
        // No retry of an open that may already have produced an owned handle.
        if (output) *output = nullptr;
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    try { return openKeyOriginal(key, subkey, options, access, output); }
    catch (...) {
        Wh_Log(L"[action-center] original registry open raised a C++ exception");
        return ERROR_GEN_FAILURE;
    }
}

static LSTATUS WINAPI CloseKeyHook(HKEY key) {
    try {
        std::lock_guard<std::mutex> lock(proxyMutex);
        // Erase before closing to avoid a recycled handle inheriting proxy state.
        proxyKeys.erase(key);
    } catch (...) { Wh_Log(L"[action-center] proxy bookkeeping exception on close"); }
    try { return closeKeyOriginal(key); }
    catch (...) {
        Wh_Log(L"[action-center] original registry close raised a C++ exception; not retrying");
        return ERROR_GEN_FAILURE;
    }
}

// The two policy hooks are installed at most once each: a second Wh_SetFunctionHook on
// the same target fails, and reporting "not installed" for a hook that is in fact in
// place would make the caller retry it forever.
static bool closeKeyHooked = false;
static bool openKeyHooked = false;

static bool InstallKeyHooks() noexcept {
    try {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        ntQueryKey = ntdll ? reinterpret_cast<NtQueryKeyFn>(
            GetProcAddress(ntdll, "NtQueryKey")) : nullptr;
        ScopedRegKey user;
        if (RegOpenCurrentUser(KEY_QUERY_VALUE, user.put()) == ERROR_SUCCESS)
            currentUserNative = KeyPath(user.get());
        if (!ntQueryKey) {
            Wh_Log(L"[action-center] NtQueryKey unavailable: handle-based reads cannot be matched");
            return false;
        }
        // Register close first. If open registration fails, close remains harmless.
        if (!closeKeyHooked) {
            if (!Wh_SetFunctionHook(reinterpret_cast<void*>(RegCloseKey),
                                    reinterpret_cast<void*>(CloseKeyHook),
                                    reinterpret_cast<void**>(&closeKeyOriginal))) {
                Wh_Log(L"[actioncenter] the RegCloseKey hook could not be installed (%lu)",
                       GetLastError());
                return false;
            }
            closeKeyHooked = true;
        }
        if (!openKeyHooked) {
            // Wh_SetFunctionHook refuses a target that this process has already hooked,
            // so a half-installed state must never be reported as "install from scratch":
            // the second call would fail forever and the caller would retry it at every
            // tick.
            if (!Wh_SetFunctionHook(reinterpret_cast<void*>(RegOpenKeyExW),
                                    reinterpret_cast<void*>(OpenKeyHook),
                                    reinterpret_cast<void**>(&openKeyOriginal))) {
                Wh_Log(L"[actioncenter] the RegOpenKeyExW hook could not be installed (%lu)",
                       GetLastError());
                return false;
            }
            openKeyHooked = true;
        }
        return true;
    } catch (...) {
        Wh_Log(L"[action-center] key hook installation exception");
        return false;
    }
}

static bool BlockXamlAdapter(REFCLSID clsid) noexcept {
    try {
        // m417z's classicRibbonUI branch; no Moments navigation-bar patches.
        static constexpr GUID adapter = {0x6480100b, 0x5a83, 0x4d1e,
            {0x9f, 0x69, 0x8a, 0xe5, 0xa8, 0x8e, 0x9a, 0x33}};
        return explorerProcess && !stopping.load() && ribbon10.load() &&
               IsEqualCLSID(clsid, adapter);
    } catch (...) { Wh_Log(L"[ribbon] selector exception; keeping system UI"); }
    return false;
}

static void Cleanup() noexcept {
    // Called only AFTER Windhawk has removed hooks. Caller-owned registry handles
    // are deliberately not closed here. Their owners still close them normally.
    try {
        std::lock_guard<std::mutex> lock(proxyMutex);
        proxyKeys.clear();
    } catch (...) { Wh_Log(L"[native-ui] cleanup exception"); }
}
}  // namespace NativeUi

static bool Sha256File(const wchar_t* path, std::wstring& outHex) {
    ScopedHandle file(CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                                  OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
    if (!file.valid()) {
        Wh_Log(L"[hash] open failed (%lu): %s", GetLastError(), path);
        return false;
    }

    BCRYPT_ALG_HANDLE alg = nullptr;
    if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) {
        Wh_Log(L"[hash] BCryptOpenAlgorithmProvider failed");
        return false;
    }

    bool ok = false;
    BCRYPT_HASH_HANDLE hash = nullptr;
    BYTE hashObj[512] = {};
    DWORD hashObjLen = 0, cb = 0;
    BYTE digest[32] = {};
    BYTE buffer[64 * 1024];

    if (BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&hashObjLen, sizeof(hashObjLen), &cb, 0) == 0 &&
        hashObjLen <= sizeof(hashObj) &&
        BCryptCreateHash(alg, &hash, hashObj, hashObjLen, nullptr, 0, 0) == 0) {
        for (;;) {
            DWORD read = 0;
            if (!ReadFile(file.get(), buffer, sizeof(buffer), &read, nullptr)) break;
            if (read == 0) {
                if (BCryptFinishHash(hash, digest, sizeof(digest), 0) == 0) ok = true;
                break;
            }
            if (BCryptHashData(hash, buffer, read, 0) != 0) break;
        }
        BCryptDestroyHash(hash);
    }
    BCryptCloseAlgorithmProvider(alg, 0);

    if (ok) {
        static const wchar_t* kHex = L"0123456789abcdef";
        outHex.clear();
        outHex.reserve(64);
        for (BYTE b : digest) {
            outHex.push_back(kHex[b >> 4]);
            outHex.push_back(kHex[b & 0xF]);
        }
    }
    return ok;
}

// Outcome of the signature check.
// NB: the most common Windows components (pnidui.dll, stobject.dll, ...) do NOT
// carry an embedded signature in the file: they are signed through the system
// catalog, which on a Windows 11 machine does not exist for Windows 10 files.
// Verified with osslsigncode: explorer.exe has an embedded Microsoft signature,
// pnidui/stobject/windows.ui.search have none. That is why a missing embedded
// signature is not an error, but it is reported: the identity of the file stays
// guaranteed by the pinned SHA-256 hash and the HTTPS origin (msdl.microsoft.com).
enum class SigResult { Ok, None, Bad };

static SigResult VerifyMicrosoftSignature(const wchar_t* path, bool* signerIsMicrosoft) {
    if (signerIsMicrosoft) *signerIsMicrosoft = false;

    WINTRUST_FILE_INFO fileInfo = {};
    fileInfo.cbStruct = sizeof(fileInfo);
    fileInfo.pcwszFilePath = path;

    WINTRUST_DATA data = {};
    data.cbStruct = sizeof(data);
    data.dwUIChoice = WTD_UI_NONE;
    data.fdwRevocationChecks = WTD_REVOKE_NONE;   // offline: nessuna rete al logon
    data.dwUnionChoice = WTD_CHOICE_FILE;
    data.pFile = &fileInfo;
    data.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL | WTD_REVOCATION_CHECK_NONE;
    data.dwStateAction = WTD_STATEACTION_VERIFY;

    GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    LONG status = WinVerifyTrust(nullptr, &action, &data);
    data.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(nullptr, &action, &data);

    if (status != ERROR_SUCCESS) {
        // 0x800B0100 TRUST_E_NOSIGNATURE: no embedded signature (catalog)
        if ((DWORD)status == (DWORD)TRUST_E_NOSIGNATURE) {
            Wh_Log(L"[sig] no embedded signature (component signed through the catalog): %s", path);
            return SigResult::None;
        }
        Wh_Log(L"[sig] signature NOT valid (0x%08X): %s", status, path);
        return SigResult::Bad;
    }

    // Firmatario
    HCERTSTORE store = nullptr;
    HCRYPTMSG  msg = nullptr;
    if (CryptQueryObject(CERT_QUERY_OBJECT_FILE, path,
                         CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
                         CERT_QUERY_FORMAT_FLAG_BINARY, 0,
                         nullptr, nullptr, nullptr, &store, &msg, nullptr)) {
        DWORD size = 0;
        if (CryptMsgGetParam(msg, CMSG_SIGNER_INFO_PARAM, 0, nullptr, &size) && size) {
            std::string buf(size, '\0');
            if (CryptMsgGetParam(msg, CMSG_SIGNER_INFO_PARAM, 0, &buf[0], &size)) {
                CMSG_SIGNER_INFO* si = (CMSG_SIGNER_INFO*)&buf[0];
                CERT_INFO ci = {};
                ci.Issuer = si->Issuer;
                ci.SerialNumber = si->SerialNumber;
                PCCERT_CONTEXT ctx = CertFindCertificateInStore(
                    store, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                    CERT_FIND_SUBJECT_CERT, &ci, nullptr);
                if (ctx) {
                    wchar_t name[256] = {};
                    if (CertGetNameStringW(ctx, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, nullptr, name, _countof(name)) > 1) {
                        Wh_Log(L"[sig] signer: %s", name);
                        if (signerIsMicrosoft) *signerIsMicrosoft = (wcsstr(name, L"Microsoft") != nullptr);
                    }
                    CertFreeCertificateContext(ctx);
                }
            }
        }
        if (msg) CryptMsgClose(msg);
        if (store) CertCloseStore(store, 0);
    }
    return SigResult::Ok;
}

static bool HttpDownloadToFile(const wchar_t* url, const wchar_t* destPath, DWORD timeoutSec) {
    ScopedInternet session(InternetOpenW(L"Win10TaskbarClean/0.2",
                                         INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0));
    if (!session.valid()) {
        Wh_Log(L"[dl] InternetOpen failed (%lu)", GetLastError());
        return false;
    }

    DWORD ms = (timeoutSec ? timeoutSec : 20) * 1000;
    InternetSetOptionW(session.get(), INTERNET_OPTION_CONNECT_TIMEOUT, &ms, sizeof(ms));
    InternetSetOptionW(session.get(), INTERNET_OPTION_RECEIVE_TIMEOUT, &ms, sizeof(ms));
    InternetSetOptionW(session.get(), INTERNET_OPTION_SEND_TIMEOUT, &ms, sizeof(ms));

    ScopedInternet request(InternetOpenUrlW(session.get(), url, nullptr, 0,
                                            INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD, 0));
    if (!request.valid()) {
        Wh_Log(L"[dl] opening the URL failed (%lu): %s", GetLastError(), url);
        return false;
    }

    DWORD status = 0, len = sizeof(status);
    if (!HttpQueryInfoW(request.get(), HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                        &status, &len, nullptr) || status != 200) {
        Wh_Log(L"[dl] HTTP response %lu for %s", status, url);
        return false;
    }

    ScopedHandle file(CreateFileW(destPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_TEMPORARY, nullptr));
    if (!file.valid()) {
        Wh_Log(L"[dl] creating the file failed (%lu): %s", GetLastError(), destPath);
        return false;
    }

    const DWORD kMaxBytes = 64u * 1024u * 1024u;   // limite di sanita'
    BYTE buffer[64 * 1024];
    DWORD total = 0;
    for (;;) {
        DWORD got = 0;
        if (!InternetReadFile(request.get(), buffer, sizeof(buffer), &got)) {
            Wh_Log(L"[dl] read failed (%lu)", GetLastError());
            return false;
        }
        if (got == 0) break;
        total += got;
        if (total > kMaxBytes) {
            Wh_Log(L"[dl] file beyond the allowed limit: aborted");
            return false;
        }
        DWORD written = 0;
        if (!WriteFile(file.get(), buffer, got, &written, nullptr) || written != got) {
            Wh_Log(L"[dl] write failed (%lu)", GetLastError());
            return false;
        }
    }
    Wh_Log(L"[dl] %lu bytes downloaded", total);
    return total > 0;
}


static bool FileMatchesPin(const wchar_t* path, const wchar_t* pin) {
    std::wstring hex;
    if (!Sha256File(path, hex)) return false;
    bool ok = (_wcsicmp(hex.c_str(), pin) == 0);
    if (!ok) Wh_Log(L"[hash] file does not match: %s", path);
    return ok;
}

// Downloads (if needed) and verifies a file. On failure the temporary
// file is removed and the function returns false (fail-closed).
static bool EnsureVerifiedFile(const wchar_t* dir, const wchar_t* fileName,
                               const wchar_t* symbolId, const wchar_t* sha256Pin,
                               wchar_t* outPath, size_t outPathCount) {
    wchar_t target[MAX_PATH] = {};
    _snwprintf_s(target, _countof(target), _TRUNCATE, L"%s\\%s", dir, fileName);

    if (GetFileAttributesW(target) != INVALID_FILE_ATTRIBUTES && FileMatchesPin(target, sha256Pin)) {
        wcscpy_s(outPath, outPathCount, target);
        return true;
    }

    wchar_t url[512] = {};
    _snwprintf_s(url, _countof(url), _TRUNCATE, kSymbolUrlFmt, fileName, symbolId, fileName);

    wchar_t temp[MAX_PATH] = {};
    _snwprintf_s(temp, _countof(temp), _TRUNCATE, L"%s.part", target);

    Wh_Log(L"[dl] downloading %s ...", url);
    if (!HttpDownloadToFile(url, temp, (DWORD)g_cfg.downloadTimeoutSec)) {
        DeleteFileW(temp);
        return false;
    }

    if (!FileMatchesPin(temp, sha256Pin)) {
        DeleteFileW(temp);
        return false;
    }

    if (g_cfg.requireSignature) {
        bool ms = false;
        SigResult sig = VerifyMicrosoftSignature(temp, &ms);
        if (sig == SigResult::Bad || (sig == SigResult::Ok && !ms)) {
            // Signature present but not valid, or signer other than Microsoft:
            // the file is discarded.
            Wh_Log(L"[sig] file rejected: %s", fileName);
            DeleteFileW(temp);
            return false;
        }
        if (sig == SigResult::None)
            Wh_Log(L"[sig] accepted on the pinned hash (catalog-only signature): %s", fileName);
    }

    if (!MoveFileExW(temp, target, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"[dl] MoveFileEx failed (%lu)", GetLastError());
        DeleteFileW(temp);
        return false;
    }

    wcscpy_s(outPath, outPathCount, target);
    return true;
}

static bool EnsureDirectory(const wchar_t* dir) {
    DWORD attr = GetFileAttributesW(dir);
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) return true;
    if (CreateDirectoryW(dir, nullptr)) return true;
    Wh_Log(L"[fs] CreateDirectory failed (%lu): %s", GetLastError(), dir);
    return false;
}

static const int kHotkeyId = 0xA1B2;

static DWORD g_servicesThreadId = 0;

// 1.0.0: lo stato della mod sta nello storage di Windhawk, non in una chiave di
// HKCU creata per l'occasione: sparisce con la mod e non lascia residui.
static DWORD GetFailureCount() {
    return (DWORD)Wh_GetIntValue(L"FailureCount", 0);
}

static bool ContainsNoCase(const wchar_t* haystack, const wchar_t* needle) {
    if (!haystack || !needle || !*needle) return false;
    size_t n = wcslen(needle);
    for (const wchar_t* p = haystack; *p; p++) {
        if (_wcsnicmp(p, needle, n) == 0) return true;
    }
    return false;
}

// so pnidui takes the flyout path instead of opening Settings on its
// own. The registry is not touched: the forcing only applies to the
// reads of this shell and disappears when the shell closes.
static std::atomic<bool> g_networkValueForce{false};
// The forced network-icon path only needs EnableAutoTray=0 as observed by the
// private legacy Explorer. Never persist that transient answer in HKCU.
static std::atomic<bool> g_autoTrayVirtual{false};
static bool g_replaceVanLogged = false;

// Chiave esatta: ...\CurrentVersion\Control Panel\Settings\Network. Il nome del
// valore da solo non basta: "Van" o "NetworkFlyout" possono comparire in altre
// chiavi, e li' il valore dell'utente non va toccato. Il confronto e' sulla coda
// del percorso, cosi' vale sia per HKCU (che ha il SID davanti) sia per HKLM.
static constexpr wchar_t kNetworkPolicyKeyTail[] =
    L"\\CurrentVersion\\Control Panel\\Settings\\Network";

static bool EndsWithNetworkPolicyTail(const wchar_t* path, size_t chars) {
    const size_t tailChars = (sizeof(kNetworkPolicyKeyTail) / sizeof(wchar_t)) - 1;
    if (!path || chars < tailChars) return false;
    return _wcsicmp(path + (chars - tailChars), kNetworkPolicyKeyTail) == 0;
}

static bool IsNetworkPolicyKeyPath(LPCWSTR path) {
    return path && EndsWithNetworkPolicyTail(path, wcslen(path));
}

// Il percorso di un handle si legge con NtQueryKey (RegQueryInfoKey non lo da'):
// risolta a runtime, cosi' non serve nessuna importazione in piu'.
typedef LONG(NTAPI* NtQueryKey_t)(HANDLE, ULONG, PVOID, ULONG, PULONG);
static NtQueryKey_t ResolveNtQueryKey() noexcept {
    static NtQueryKey_t resolved = []() -> NtQueryKey_t {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        return ntdll ? (NtQueryKey_t)GetProcAddress(ntdll, "NtQueryKey") : nullptr;
    }();
    return resolved;
}

static bool KeyIsNetworkPolicyKey(HKEY key) noexcept {
    if (!key) return false;
    NtQueryKey_t query = ResolveNtQueryKey();
    if (!query) return false;
    struct {
        ULONG nameLength;   // in byte, senza il terminatore
        WCHAR name[512];
    } info = {};
    ULONG length = 0;
    if (query((HANDLE)key, 3 /* KeyNameInformation */, &info, sizeof(info), &length) < 0)
        return false;
    if (info.nameLength == 0) return false;
    size_t chars = info.nameLength / sizeof(wchar_t);
    if (chars >= _countof(info.name)) chars = _countof(info.name) - 1;
    info.name[chars] = 0;
    return IsNetworkPolicyKeyPath(info.name);
}

static bool IsNetworkPolicyValueName(LPCWSTR name) {
    if (!name) return false;
    if (_wcsicmp(name, L"ReplaceVan") == 0 || _wcsicmp(name, L"VANFromPCSettings") == 0) return true;
    // Le grafie storiche con "Van" restano accettate, ma solo DENTRO la chiave
    // giusta (vedi KeyIsNetworkPolicyKey/IsNetworkPolicyKeyPath): su altre build
    // il nome cambia e il log dice quale era.
    return ContainsNoCase(name, L"Van") || ContainsNoCase(name, L"NetworkFlyout");
}

static wchar_t g_forcedNames[8][64] = {};
static int g_forcedNamesCount = 0;

// One line per name (max 8): the user's log shows exactly which
// value tells pnidui which interface to show.
static void ReportNetworkPolicyForced(LPCWSTR name) {
    if (!name) return;
    for (int i = 0; i < g_forcedNamesCount; i++) {
        if (_wcsicmp(g_forcedNames[i], name) == 0) return;
    }
    if (g_forcedNamesCount >= (int)_countof(g_forcedNames)) return;
    wcsncpy_s(g_forcedNames[g_forcedNamesCount++], _countof(g_forcedNames[0]), name, _TRUNCATE);
    Wh_Log(L"[network] policy value forced to 0: %s", name);
}

static void ReportReplaceVanForced() {
    if (g_replaceVanLogged) return;
    g_replaceVanLogged = true;
    Wh_Log(L"[network] ReplaceVan/VANFromPCSettings forced to 0 in this shell: pnidui must use the flyout, not Settings");
}

// true = value forced and answer given; false = a bigger buffer is needed
// (the caller answers ERROR_MORE_DATA, as the system does).
static bool ForceNetworkValueZero(LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    ReportReplaceVanForced();
    if (lpType) *lpType = REG_DWORD;
    if (!lpData) {
        if (lpcbData) *lpcbData = sizeof(DWORD);
        return true;
    }
    if (!lpcbData || *lpcbData < sizeof(DWORD)) {
        if (lpcbData) *lpcbData = sizeof(DWORD);
        return false;
    }
    *(DWORD*)lpData = 0;
    *lpcbData = sizeof(DWORD);
    return true;
}

typedef LONG(WINAPI* RegQueryValueExW_t)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
static RegQueryValueExW_t RegQueryValueExW_Original = nullptr;
typedef LSTATUS(WINAPI* RegSetValueExW_t)(HKEY, LPCWSTR, DWORD, DWORD, const BYTE*, DWORD);
static RegSetValueExW_t RegSetValueExW_Original = nullptr;



// This applies only to the private Explorer's exact Explorer key. It deliberately
// covers only the two public read APIs already hooked by this mod.
static bool TryReadVirtualAutoTray(HKEY key, LPCWSTR subkey, LPCWSTR name,
                                   bool getValue, DWORD flags, LPDWORD type,
                                   void* data, LPDWORD bytes,
                                   LSTATUS* result) noexcept {
    try {
        if (!result || !g_autoTrayVirtual.load(std::memory_order_acquire) ||
            !NativeUi::privateExplorer || !name ||
            _wcsicmp(name, L"EnableAutoTray") != 0 ||
            !NativeUi::IsExplorerPath(key, subkey)) {
            return false;
        }
        *result = NativeUi::CopyDword(0, getValue, flags, type, data, bytes);
        static std::atomic<bool> reported{false};
        if (!reported.exchange(true))
            Wh_Log(L"[tray-force] serving virtual EnableAutoTray=0 in the private Explorer (not a registry write)");
        return true;
    } catch (...) {
        Wh_Log(L"[tray-force] EnableAutoTray virtual read exception; using the real registry");
        return false;
    }
}

#define IDM_SHOWDESKTOP  0x197
#define IDM_TASKMANAGER  0x1A4
#define IDM_LOCKTASKBAR  0x1A8
#define IDM_SETTINGS     0x19D
#define IDM_LOCKTOOLBARS 41484


#define IDM_MOD_SHOWDESKTOP 0x7C74
// The show desktop button panel: "Show desktop" and "Peek at desktop". Explorer's own
// ids for the two are 0x1A2D and 0x1A2E; the mod uses its own so that the two never mix.

#define IDM_MOD_PEEK          0x7C75


static bool IsSeparatorItem(HMENU menu, int pos) {
    MENUITEMINFOW mii = {};
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_FTYPE;
    return menu && GetMenuItemInfoW(menu, pos, TRUE, &mii) && (mii.fType & MFT_SEPARATOR);
}


// Forward declarations needed by ShowBatteryMenu (defined further below).
static bool HandleClassicMenuCommand(UINT id);

static bool MenuContainsId(HMENU menu, UINT id);
static HWND FindNativeTaskbarStartButton();
static bool IsWinXNativeContextMenuRequest();

// === MULTI-LANGUAGE SUPPORT for tray context menus (battery, network,
// clock, show-desktop) and for the strings served to the tray modules.
// Language detected from GetUserDefaultUILanguage, fallback English.
// 15 languages: IT, EN, FR, ES, DE, PT, NL, RU, JA, PL, DA, SV, NO, FI, TR.

enum UiLangId {
    LANG_IT = 0, LANG_EN, LANG_FR, LANG_ES, LANG_DE,
    LANG_PT, LANG_NL, LANG_RU, LANG_JA, LANG_PL,
    LANG_DA, LANG_SV, LANG_NO, LANG_FI, LANG_TR, LANG_COUNT
};

static UiLangId DetectUiLang() {
    LANGID lid = (LANGID)(GetUserDefaultUILanguage() & 0xFFFF);
    WORD primary = PRIMARYLANGID(lid);
    switch (primary) {
        case LANG_ITALIAN:    return LANG_IT;
        case LANG_ENGLISH:    return LANG_EN;
        case LANG_FRENCH:     return LANG_FR;
        case LANG_SPANISH:    return LANG_ES;
        case LANG_GERMAN:     return LANG_DE;
        case LANG_PORTUGUESE: return LANG_PT;
        case LANG_DUTCH:      return LANG_NL;
        case LANG_RUSSIAN:    return LANG_RU;
        case LANG_JAPANESE:   return LANG_JA;
        case LANG_POLISH:     return LANG_PL;
        case LANG_DANISH:     return LANG_DA;
        case LANG_SWEDISH:    return LANG_SV;
        case LANG_NORWEGIAN:  return LANG_NO;
        case LANG_FINNISH:    return LANG_FI;
        case LANG_TURKISH:    return LANG_TR;
        default:              return LANG_EN;
    }
}



// Battery right-click menu entries. Command ids are stobject's own
// (101 = Power Options, 102 = Windows Mobility Center); when the user
// selects an entry we forward WM_COMMAND to stobject's tray window so
// it handles them natively, just like Windows 10.
static const wchar_t* const kBatteryPowerOptions[LANG_COUNT] = {
    L"&Opzioni risparmio energia",
    L"&Power Options",
    L"&Options d'alimentation",
    L"&Opciones de energía",
    L"&Energieoptionen",
    L"Opções de &Energia",
    L"&Energiebeheer",
    L"&Электропитание",
    L"電源オプション(&O)",
    L"Opcje &zasilania",
    L"&Strømstyring", L"&Energialternativ", L"&Strømalternativer", L"&Virranhallinta", L"&Güç Seçenekleri",
};
static const wchar_t* const kBatteryMobilityCenter[LANG_COUNT] = {
    L"&Centro PC portatile Windows",
    L"Windows Mobility &Center",
    L"Centre de mobilité W&indows",
    L"Centro de movilidad de W&indows",
    L"Windows-&Mobility Center",
    L"Centro de Mobilidade do Windows (&M)",
    L"Windows Mobi&lity Center",
    L"Центр мобильности &Windows",
    L"Windows モビリティ センター(&M)",
    L"Centrum mobilności w systemie &Windows",
    L"Windows &Mobilitetscenter", L"Windows &Mobility Center", L"Windows &Mobilitetssenter", L"Windowsin &liikkuvuuskeskus", L"Windows &Mobility Center",
};

// Network right-click (pnidui resource 3014): troubleshoot (3107), settings (3109)
static const wchar_t* const kNetTroubleshoot[LANG_COUNT] = {
    L"Risoluzione &problemi",
    L"&Troubleshoot problems",
    L"&Résoudre les problèmes",
    L"&Solucionar problemas",
    L"Problem&behandlung",
    L"&Resolver Problemas",
    L"Problemen &oplossen",
    L"&Диагностика неполадок",
    L"トラブルシューティング(&T)",
    L"Rozwiąż &problemy",
    L"&Løs problemer", L"&Felsök problem", L"&Løs problemer", L"&Vianmääritys", L"&Sorun giderme",
};
static const wchar_t* const kNetOpenSettings[LANG_COUNT] = {
    L"Apri impostazioni &Rete e Internet",
    L"Open Network &Internet settings",
    L"Ouvrir les paramètres &réseau et Internet",
    L"Abrir configuración de &Red e Internet",
    L"Netzwerk- und &Interneteinstellungen öffnen",
    L"Abrir Definições de &Rede e Internet",
    L"Netwerk- en &internetinstellingen openen",
    L"Открыть параметры &сети и Интернета",
    L"ネットワークとインターネットの設定を開く(&I)",
    L"Otwórz ustawienia &sieci i Internetu",
    L"Åbn indstillinger for &netværk og internet", L"Öppna inställningar för &nätverk och internet", L"Åpne innstillinger for &nettverk og Internett", L"Avaa verkon ja Internetin &asetukset", L"&Ağ ve Internet ayarlarını aç",
};

// Show-desktop button
static const wchar_t* const kShowDesktopText[LANG_COUNT] = {
    L"Mostra &desktop",
    L"&Show the desktop",
    L"&Afficher le bureau",
    L"&Mostrar el escritorio",
    L"&Desktop anzeigen",
    L"Mostrar o &Ambiente de Trabalho",
    L"&Bureaublad weergeven",
    L"&Показать рабочий стол",
    L"デスクトップを表示(&S)",
    L"Pokaż &pulpit",
    L"Vis &skrivebordet", L"&Visa skrivbordet", L"Vis &skrivebordet", L"&Näytä työpöytä", L"&Masaüstünü göster",
};
static const wchar_t* const kPeekText[LANG_COUNT] = {
    L"Visualizza desktop con &Aero Peek",
    L"Peek at &desktop (Aero Peek)",
    L"Afficher un &aperçu du bureau (Aero Peek)",
    L"Echar un vistazo al &escritorio (Aero Peek)",
    L"Desktop mit Aero &Peek anzeigen",
    L"Observar o A&mbiente de Trabalho (Aero Peek)",
    L"Bu&reaublad bekijken (Aero Peek)",
    L"Просмотреть рабочий стол с помощ&ью Aero Peek",
    L"デスクトップをプレビュー(&D) (Aero Peek)",
    L"Rzuć okiem na pulpit (&Aero Peek)",
    L"Kig på &skrivebordet (Aero Peek)", L"Granska &skrivbordet (Aero Peek)", L"Titt på &skrivebordet (Aero Peek)", L"Työpöydän &esikatselu (Aero Peek)", L"&Masaüstüne bak (Aero Peek)",
};





// ===========================================================================
// ImmersiveMenu - menu "immersivi" (stile Windows 10) per i menu della mod.
//
// 1.0.0: proportions, spacing and colours are taken from Windows 10 reference
// screenshots of the power-user (Win+X) menu: 32 px rows, text 35 px inside the
// left edge, one chevron 11 px from the right edge, separator 2 px inside the item
// rectangle, F9F9F9/black in the light scheme and 2B2B2B/white with a 414141
// highlight in the dark one. The OS submenu arrow is clipped away so the chevron
// is not drawn twice.
//
// La mod "Non Immersive Taskbar Context Menu" rende classici i menu della barra
// togliendo MFT_OWNERDRAW dalle voci e azzerando lo sfondo (hbrBack). Qui si fa
// l'inverso: ogni voce diventa MFT_OWNERDRAW, il menu riceve uno sfondo preso dal
// tema "ImmersiveStart::Menu" (chiaro/scuro) e il disegno di voci, selezione e
// separatori e' fatto con gli stessi colori e lo stesso font del tema. La finestra
// proprietaria viene sottoclassata solo mentre il menu e' aperto, per rispondere a
// WM_MEASUREITEM e WM_DRAWITEM delle sole voci create qui. Se il tema non e'
// disponibile il menu resta un normale popup.
// ===========================================================================
namespace ImmersiveMenu {

using ThemeHandle = void*;
static constexpr int kPartBackground = 9;      // MENU_POPUPBACKGROUND
static constexpr int kPartItem = 14;           // MENU_POPUPITEM
static constexpr int kStateNormal = 1;
static constexpr int kStateHot = 2;
static constexpr int kStateDisabled = 3;
static constexpr int kPropBorderColor = 3801;  // TMT_BORDERCOLOR
static constexpr int kPropFillColor = 3802;    // TMT_FILLCOLOR
static constexpr int kPropTextColor = 3803;    // TMT_TEXTCOLOR
static constexpr int kPropFont = 210;          // TMT_FONT
static constexpr UINT_PTR kSubclassId = 0x494D4D31;   // "IMM1"

struct Api {
    ThemeHandle (WINAPI* openTheme)(HWND, LPCWSTR) = nullptr;
    ThemeHandle (WINAPI* openThemeForDpi)(HWND, LPCWSTR, UINT) = nullptr;
    HRESULT (WINAPI* closeTheme)(ThemeHandle) = nullptr;
    HRESULT (WINAPI* drawBackground)(ThemeHandle, HDC, int, int, const RECT*, const RECT*) = nullptr;
    HRESULT (WINAPI* getFont)(ThemeHandle, HDC, int, int, int, LOGFONTW*) = nullptr;
    HRESULT (WINAPI* getColor)(ThemeHandle, int, int, int, COLORREF*) = nullptr;
    bool ready = false;
};
static Api g_api;

static bool LoadApi() noexcept {
    try {
        if (g_api.ready) return true;
        HMODULE module = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) return false;
        g_api.openTheme = reinterpret_cast<ThemeHandle (WINAPI*)(HWND, LPCWSTR)>(
            GetProcAddress(module, "OpenThemeData"));
        g_api.openThemeForDpi = reinterpret_cast<ThemeHandle (WINAPI*)(HWND, LPCWSTR, UINT)>(
            GetProcAddress(module, "OpenThemeDataForDpi"));
        g_api.closeTheme = reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle)>(
            GetProcAddress(module, "CloseThemeData"));
        g_api.drawBackground =
            reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle, HDC, int, int, const RECT*, const RECT*)>(
                GetProcAddress(module, "DrawThemeBackground"));
        g_api.getFont = reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle, HDC, int, int, int, LOGFONTW*)>(
            GetProcAddress(module, "GetThemeFont"));
        g_api.getColor = reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle, int, int, int, COLORREF*)>(
            GetProcAddress(module, "GetThemeColor"));
        g_api.ready = g_api.openTheme && g_api.closeTheme && g_api.getColor;
        return g_api.ready;
    } catch (...) {
        return false;
    }
}

struct ItemData {
    wchar_t text[128];
    bool separator;
    bool submenu;
};

struct Session {
    HWND owner = nullptr;
    ThemeHandle theme = nullptr;
    int dpi = 96;
    LOGFONTW font = {};
    HFONT hfont = nullptr;
    HBRUSH brush = nullptr;
    // Colours of the Windows 10 menu: the light scheme is the one the menu has
    // always used (F9F9F9 / black text), the dark one is the Windows 10 dark menu
    // (2B2B2B / white text, 414141 highlight) as in the reference screenshot. The
    // theme of the shell refines them when it is available, so the menu follows the
    // Windows app theme or, when it is not "Auto", the skin's Colour scheme.
    bool light = true;
    COLORREF fill = RGB(249, 249, 249);
    COLORREF textNormal = RGB(0, 0, 0);
    COLORREF textDisabled = RGB(160, 160, 160);
    COLORREF border = RGB(205, 205, 205);
    COLORREF separator = RGB(205, 205, 205);
    COLORREF hotFill = RGB(229, 229, 229);
    int itemHeight = 0;
    // Proportions measured on the Windows 10 reference screenshot (uploads/image-2.png,
    // 100% DPI) and used as they are, so the recreation has the proportions of the
    // original: 32 px rows, text 32 px inside the item rectangle, the submenu chevron
    // 6 px from its right edge, the separator line inset by 8 px, and a menu about
    // 258 px wide - the width of the reference menu, from which the shell subtracts
    // its own right-hand gutter when the items are measured.
    int padLeft = 0;
    int padRight = 0;
    int chevronInset = 0;
    int separatorInset = 0;
    int row = 0;                 // position inside the menu, for the log only
    std::vector<ItemData*> items;
};
static Session* g_session = nullptr;

// Which scheme the menu uses (1.0.0). "Auto" follows the Windows app theme
// (Settings > Personalization > Colors), "Light"/"Dark" force the Windows 10 light
// or dark menu; the value comes from the skin's own Colour scheme setting, so the
// Win+X menu and the rest of the mod cannot disagree. Both colours sets are always
// available: if the shell theme cannot be opened, the menu is painted with the
// Windows 10 constants of the selected scheme instead of an unstyled popup.
static bool IsLightTheme() noexcept {
    try {
        WindhawkUtils::StringSetting scheme(Wh_GetStringSetting(L"themeScheme"));
        if (scheme.get()) {
            if (_wcsicmp(scheme.get(), L"light") == 0) return true;
            if (_wcsicmp(scheme.get(), L"dark") == 0) return false;
        }
    } catch (...) {
    }
    DWORD value = 1, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS)
        return value != 0;
    return true;
}

// Stessa sequenza di pnidui: variante chiaro/scuro, poi neutra, infine "Menu".
static ThemeHandle OpenMenuTheme(HWND owner, int dpi) noexcept {
    const wchar_t* candidates[3] = {
        IsLightTheme() ? L"LightMode_ImmersiveStart::Menu" : L"DarkMode_ImmersiveStart::Menu",
        L"ImmersiveStart::Menu",
        L"Menu" };
    for (const wchar_t* name : candidates) {
        ThemeHandle theme = nullptr;
        if (g_api.openThemeForDpi) theme = g_api.openThemeForDpi(owner, name, (UINT)dpi);
        if (!theme && g_api.openTheme) theme = g_api.openTheme(owner, name);
        if (theme) {
            static int logged = 0;
            if (logged++ < 3) Wh_Log(L"[menu] immersive menu: theme class '%s'", name);
            return theme;
        }
    }
    return nullptr;
}

static void End(Session& s) noexcept {
    for (ItemData* d : s.items) delete d;
    s.items.clear();
    if (s.brush) { DeleteObject(s.brush); s.brush = nullptr; }
    if (s.hfont) { DeleteObject(s.hfont); s.hfont = nullptr; }
    if (s.theme && g_api.closeTheme) { g_api.closeTheme(s.theme); s.theme = nullptr; }
}

static void Prepare(Session& s, HMENU menu) noexcept {
    const int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        wchar_t buffer[128] = {};
        MENUITEMINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_SUBMENU;
        info.dwTypeData = buffer;
        info.cch = _countof(buffer) - 1;
        if (!GetMenuItemInfoW(menu, i, TRUE, &info)) continue;
        ItemData* data = new (std::nothrow) ItemData{};
        if (!data) continue;
        data->separator = (info.fType & MFT_SEPARATOR) != 0;
        data->submenu = info.hSubMenu != nullptr;
        wcsncpy_s(data->text, buffer, _TRUNCATE);
        s.items.push_back(data);

        MENUITEMINFOW set = {};
        set.cbSize = sizeof(set);
        set.fMask = MIIM_FTYPE | MIIM_DATA;
        set.fType = info.fType | MFT_OWNERDRAW;        // l'inverso di ApplyClassicMenu
        set.dwItemData = reinterpret_cast<ULONG_PTR>(data);
        SetMenuItemInfoW(menu, i, TRUE, &set);
        if (info.hSubMenu) Prepare(s, info.hSubMenu);
    }
    MENUINFO mi = {};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND;
    mi.hbrBack = s.brush;                               // sfondo del tema, non nullo
    SetMenuInfo(menu, &mi);
}

static bool Begin(Session& s, HMENU menu, HWND owner) noexcept {
    try {
        if (!owner || !LoadApi()) return false;
        s.owner = owner;
        const UINT dpi = GetDpiForWindow(owner);
        s.dpi = dpi >= 96 && dpi <= 480 ? (int)dpi : 96;
        // Windows 10 constants for the selected scheme: they stay in place if the
        // theme cannot be opened or does not carry a colour, so both themes are
        // supported even on builds where the immersive menu theme is missing.
        s.light = IsLightTheme();
        s.fill = s.light ? RGB(249, 249, 249) : RGB(43, 43, 43);
        s.textNormal = s.light ? RGB(0, 0, 0) : RGB(255, 255, 255);
        s.textDisabled = s.light ? RGB(120, 120, 120) : RGB(160, 160, 160);
        s.border = s.light ? RGB(205, 205, 205) : RGB(128, 128, 128);
        s.separator = s.light ? RGB(205, 205, 205) : RGB(128, 128, 128);
        s.hotFill = s.light ? RGB(229, 229, 229) : RGB(65, 65, 65);
        s.theme = OpenMenuTheme(owner, s.dpi);
        if (!s.theme) {
            // No immersive theme on this build: paint the menu with the Windows 10
            // colours anyway instead of falling back to a plain popup.
            Wh_Log(L"[menu] immersive theme unavailable: the menu keeps the Windows 10 %s colours",
                   s.light ? L"light" : L"dark");
        }

        HDC dc = GetDC(owner);
        if (!s.theme || !g_api.getFont ||
            FAILED(g_api.getFont(s.theme, dc, kPartItem, 0, kPropFont, &s.font)) ||
            s.font.lfHeight == 0) {
            NONCLIENTMETRICSW info = {};
            info.cbSize = sizeof(info);
            if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0))
                s.font = info.lfMenuFont;
            else
                s.font.lfHeight = -MulDiv(9, s.dpi, 72);
            wcscpy_s(s.font.lfFaceName, LF_FACESIZE, L"Segoe UI");
        }
        if (s.theme) {
            g_api.getColor(s.theme, kPartBackground, 0, kPropFillColor, &s.fill);
            g_api.getColor(s.theme, kPartItem, kStateNormal, kPropTextColor, &s.textNormal);
            g_api.getColor(s.theme, kPartItem, kStateDisabled, kPropTextColor, &s.textDisabled);
            COLORREF hot = s.hotFill;
            if (g_api.getColor(s.theme, kPartItem, kStateHot, kPropFillColor, &hot) == S_OK)
                s.hotFill = hot;
        }
        // Colour of the separator line for the scheme in use. In the light scheme the
        // border colour the menu theme gives to its own popups is used when it is
        // available (it is the colour the earlier builds showed); in the dark scheme the
        // grey measured on the Windows 10 dark reference screenshot is used, which is
        // clearly visible on the 2B2B2B background.
        if (s.light && s.theme && g_api.getColor) {
            COLORREF themed = s.separator;
            if (g_api.getColor(s.theme, kPartBackground, 0, kPropBorderColor, &themed) == S_OK &&
                themed != s.fill)
                s.separator = themed;
        }
        static int themeLogs = 0;
        if (themeLogs++ < 6)
            Wh_Log(L"[menu] %s menu: background RGB(%u,%u,%u), highlight RGB(%u,%u,%u), "
                   L"text RGB(%u,%u,%u), separator RGB(%u,%u,%u)",
                   s.light ? L"light" : L"dark",
                   GetRValue(s.fill), GetGValue(s.fill), GetBValue(s.fill),
                   GetRValue(s.hotFill), GetGValue(s.hotFill), GetBValue(s.hotFill),
                   GetRValue(s.textNormal), GetGValue(s.textNormal), GetBValue(s.textNormal),
                   GetRValue(s.separator), GetGValue(s.separator), GetBValue(s.separator));
        s.hfont = CreateFontIndirectW(&s.font);
        int textHeight = abs(s.font.lfHeight);
        if (s.hfont && dc) {
            HGDIOBJ old = SelectObject(dc, s.hfont);
            TEXTMETRICW tm = {};
            if (GetTextMetricsW(dc, &tm)) textHeight = tm.tmHeight;
            if (old) SelectObject(dc, old);
        }
        if (dc) ReleaseDC(owner, dc);
        // Reference proportions (see Session), in device independent pixels so the
        // menu looks the same at every display scale: rows 32 px tall, text 32 px in
        // from the item rectangle, chevron 6 px from its right edge, separator line
        // inset by 8 px. A row never becomes shorter than the text plus 8 px, so a
        // large font or an East Asian face cannot clip the glyphs.
        s.itemHeight = MulDiv(32, s.dpi, 96);
        const int minItemHeight = textHeight + MulDiv(8, s.dpi, 96);
        if (s.itemHeight < minItemHeight) s.itemHeight = minItemHeight;
        s.padLeft = MulDiv(32, s.dpi, 96);
        s.padRight = MulDiv(10, s.dpi, 96);
        s.chevronInset = MulDiv(4, s.dpi, 96);
        s.separatorInset = MulDiv(8, s.dpi, 96);
        s.brush = CreateSolidBrush(s.fill);
        if (!s.hfont || !s.brush) return false;
        Prepare(s, menu);
        return true;
    } catch (...) {
        return false;
    }
}

static bool Owns(const Session* s, const ItemData* d) noexcept {
    if (!s || !d) return false;
    for (const ItemData* item : s->items)
        if (item == d) return true;
    return false;
}

static void Measure(Session* s, HWND hwnd, MEASUREITEMSTRUCT* m, const ItemData* d) noexcept {
    if (d->separator) {
        m->itemWidth = 1;
        m->itemHeight = MulDiv(9, s->dpi, 96);
        return;
    }
    int width = 0;
    HDC dc = GetDC(hwnd);
    if (dc) {
        HGDIOBJ old = SelectObject(dc, s->hfont);
        RECT rc = {};
        DrawTextW(dc, d->text, -1, &rc, DT_CALCRECT | DT_SINGLELINE | DT_LEFT);
        width = rc.right - rc.left;
        if (old) SelectObject(dc, old);
        ReleaseDC(hwnd, dc);
    }
    width += s->padLeft + s->padRight;
    if (d->submenu) width += s->chevronInset + MulDiv(14, s->dpi, 96);
    // 242 px plus the ~16 px gutter the shell keeps on the right of the item
    // rectangle gives the 258 px wide menu of the Windows 10 reference screenshot.
    const int minWidth = MulDiv(242, s->dpi, 96);
    m->itemWidth = static_cast<UINT>(width < minWidth ? minWidth : width);
    m->itemHeight = static_cast<UINT>(s->itemHeight);
}

static void Draw(Session* s, DRAWITEMSTRUCT* di, const ItemData* d) noexcept {
    HDC dc = di->hDC;
    RECT rc = di->rcItem;
    // Start every item from a clean clip region: the submenu rows take their own
    // rectangle out of the clip below (it is the documented way of stopping the
    // shell from painting a second arrow on top of ours), and a clip left over from
    // an earlier paint of the same row would otherwise erase parts of this one.
    SelectClipRgn(dc, nullptr);
    FillRect(dc, &rc, s->brush);
    if (d->separator) {
        // One pixel line of the separator colour of the scheme, always painted here.
        // The menu theme cannot be trusted for this part: on the build that reported
        // missing separators the themed MENU_POPUPSEPARATOR answered success without
        // painting anything, so the section is drawn directly and never depends on the
        // theme. Position and inset are the ones measured on the Windows 10 reference
        // menu (a 1 px line, 8 px inside the item rectangle, in the middle of the row).
        const int y = (rc.top + rc.bottom) / 2;
        RECT r = { rc.left + s->separatorInset, y, rc.right - s->separatorInset, y + 1 };
        HBRUSH line = CreateSolidBrush(s->separator);
        if (line) {
            FillRect(dc, &r, line);
            DeleteObject(line);
        } else {
            // Only possible when the process is out of GDI objects: the row keeps the
            // menu background (a null brush would leave a black line behind).
            Wh_Log(L"[menu] separator brush not available: the line is skipped");
        }
        return;
    }
    const bool disabled = (di->itemState & (ODS_GRAYED | ODS_DISABLED)) != 0;
    const bool selected = (di->itemState & ODS_SELECTED) != 0 && !disabled;
    COLORREF textColor = disabled ? s->textDisabled : s->textNormal;
    if (selected) {
        if (!s->theme || !g_api.drawBackground ||
            FAILED(g_api.drawBackground(s->theme, dc, kPartItem, kStateHot, &rc, nullptr))) {
            // Fallback of the selected row: the Windows 10 highlight colour of the
            // scheme in use (light 229,229,229 / dark 65,65,65), never an inverted
            // background, which used to turn the row black on a light menu.
            HBRUSH hot = CreateSolidBrush(s->hotFill);
            if (hot) { FillRect(dc, &rc, hot); DeleteObject(hot); }
        }
        COLORREF hotText = textColor;
        if (s->theme &&
            g_api.getColor(s->theme, kPartItem, kStateHot, kPropTextColor, &hotText) == S_OK)
            textColor = hotText;
    }
    HGDIOBJ old = SelectObject(dc, s->hfont);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, textColor);
    RECT tr = rc;
    tr.left += s->padLeft;
    tr.right -= s->padRight;
    DrawTextW(dc, d->text, -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_HIDEPREFIX);
    if (d->submenu) {
        // One chevron only: the shell glyph Windows 10 shows for a submenu, at the
        // same distance from the right edge (see the reference screenshot).
        RECT ar = rc;
        ar.right -= s->chevronInset;
        HFONT arrowFont = CreateFontW(s->font.lfHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0,
                                      L"Segoe MDL2 Assets");
        if (arrowFont) {
            SelectObject(dc, arrowFont);
            DrawTextW(dc, L"\xE76C", -1, &ar, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
            SelectObject(dc, s->hfont);
            DeleteObject(arrowFont);
        }
    }
    if (old) SelectObject(dc, old);
    if (d->submenu) {
        // 1.0.0 - "one arrow, not two". The shell paints its own submenu arrow after
        // WM_DRAWITEM has returned, on top of the chevron drawn just above: the
        // documented way to stop it is to take the item rectangle out of the clip
        // region of this DC before returning, so only our chevron survives.
        ExcludeClipRect(dc, rc.left, rc.top, rc.right, rc.bottom);
    }
}

static LRESULT CALLBACK OwnerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                  UINT_PTR, DWORD_PTR) {
    try {
        Session* s = g_session;
        if (s && msg == WM_MEASUREITEM && lParam) {
            auto* m = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);
            const auto* d = reinterpret_cast<const ItemData*>(m->itemData);
            if (m->CtlType == ODT_MENU && Owns(s, d)) {
                Measure(s, hwnd, m, d);
                return TRUE;
            }
        } else if (s && msg == WM_DRAWITEM && lParam) {
            auto* di = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
            const auto* d = reinterpret_cast<const ItemData*>(di->itemData);
            if (di->CtlType == ODT_MENU && Owns(s, d)) {
                Draw(s, di, d);
                return TRUE;
            }
        }
    } catch (...) {
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// Come TrackPopupMenuEx con TPM_RETURNCMD, ma con voci immersive. Va chiamata dal
// thread che possiede "owner". Senza tema usabile mostra il popup normale.
static UINT Track(HMENU menu, HWND owner, int x, int y, UINT flags) noexcept {
    Session session;
    try {
        flags = (flags & ~TPM_NONOTIFY) | TPM_RETURNCMD;
        if (g_session || !Begin(session, menu, owner)) {
            End(session);
            return static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
        }
        g_session = &session;
        SetWindowSubclass(owner, OwnerProc, kSubclassId, 0);
        const UINT result = static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
        RemoveWindowSubclass(owner, OwnerProc, kSubclassId);
        g_session = nullptr;
        End(session);
        return result;
    } catch (...) {
        // The session must not survive an exception: the owner window would keep the
        // subclass and the next menu would be drawn with the flags of this one.
        if (g_session == &session) {
            RemoveWindowSubclass(owner, OwnerProc, kSubclassId);
            g_session = nullptr;
        }
        End(session);
        Wh_Log(L"[menu] exception while showing the menu: the standard popup is used");
        return static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
    }
}

}  // namespace ImmersiveMenu

#define IDM_MOD_BATTERY_POWER       0x7C78
#define IDM_MOD_BATTERY_MOBILITY    0x7C79

static int g_batteryMenuLogs = 0;
static void ShowBatteryMenu(HWND serviceWindow) {
    const UiLangId lang = DetectUiLang();
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    AppendMenuW(menu, MF_STRING, IDM_MOD_BATTERY_POWER,    kBatteryPowerOptions[lang]);
    AppendMenuW(menu, MF_STRING, IDM_MOD_BATTERY_MOBILITY, kBatteryMobilityCenter[lang]);
    if (g_batteryMenuLogs < 5) {
        g_batteryMenuLogs++;
        Wh_Log(L"[battery] context menu shown (language %d)", (int)lang);
    }

    POINT pt = {};
    GetCursorPos(&pt);

    // Owner per il TEMA: Shell_TrayWnd fa applicare a Windows il tema
    // Explorer (stesso stile del menu audio/volume). Se non c'è, si ripiega
    // sulla finestra di servizio di stobject.
    HWND themeOwner = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!themeOwner) themeOwner = serviceWindow;

    // Documentato da Microsoft: per i menu delle icone della tray il
    // foreground deve essere la finestra proprietaria, altrimenti il menu
    // non riceve il tema corretto (e può non chiudersi cliccando fuori).
    SetForegroundWindow(themeOwner);

    const BOOL scelta = static_cast<BOOL>(
        ImmersiveMenu::Track(menu, themeOwner, pt.x, pt.y, TPM_RIGHTBUTTON));
    DestroyMenu(menu);
    if (!scelta) return;

    const UINT cmd = (UINT)(UINT_PTR)scelta;
    UINT realId = 0;
    switch (cmd) {
        case IDM_MOD_BATTERY_POWER:    realId = 101; break;
        case IDM_MOD_BATTERY_MOBILITY: realId = 102; break;
        default: HandleClassicMenuCommand(cmd); return;
    }
    // I comandi 101/102 sono di stobject: glieli rimandiamo sulla SUA
    // finestra di servizio, non su Shell_TrayWnd.
    if (realId && serviceWindow && IsWindow(serviceWindow)) {
        SendMessageW(serviceWindow, WM_COMMAND, MAKEWPARAM(realId, 0), 0);
        Wh_Log(L"[battery] sent WM_COMMAND %u to stobject tray window", realId);
    }
}

static int g_classicCmdLogs = 0;




#define IDS_TASKMANAGER  24743   // shell32.dll
#define IDS_SETTINGS     2128    // bthprops.cpl

typedef HMENU(WINAPI* LoadMenuW_t)(HINSTANCE, LPCWSTR);
static LoadMenuW_t LoadMenuW_Original = nullptr;

static HMODULE g_shell32 = nullptr;
static HMODULE g_bthprops = nullptr;
static HMODULE g_explorerframe = nullptr;

static wchar_t* LoadStr(HMODULE mod, UINT id, wchar_t* buf, int size) {
    if (mod && LoadStringW(mod, id, buf, size) > 0) return buf;
    return nullptr;
}

static bool GetLockToolbarsText(wchar_t* buf, int size) {
    if (!g_explorerframe) return false;
    HMENU menu = LoadMenuW_Original(g_explorerframe, MAKEINTRESOURCEW(264));
    if (!menu) return false;

    bool found = false;
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count && !found; i++) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_SUBMENU;
        if (!GetMenuItemInfoW(menu, i, TRUE, &mii) || !mii.hSubMenu) continue;
        for (int j = 0, subCount = GetMenuItemCount(mii.hSubMenu); j < subCount; j++) {
            wchar_t text[256] = {};
            MENUITEMINFOW sub = {};
            sub.cbSize = sizeof(sub);
            sub.fMask = MIIM_ID | MIIM_STRING;
            sub.dwTypeData = text;
            sub.cch = _countof(text) - 1;
            if (GetMenuItemInfoW(mii.hSubMenu, j, TRUE, &sub) && sub.wID == IDM_LOCKTOOLBARS) {
                wcsncpy_s(buf, size, text, _TRUNCATE);
                found = true;
                break;
            }
        }
    }
    DestroyMenu(menu);
    return found;
}


// One line with the entries of the menu as they came out, so the order can be
// checked against the Windows 10 menu without guessing.
static int g_menuOrderLogs = 0;

static void LogTaskbarMenuOrder(HMENU menu) {
    if (!menu || g_menuOrderLogs >= 1) return;
    g_menuOrderLogs++;
    wchar_t line[512] = {};
    size_t used = 0;
    const int count = GetMenuItemCount(menu);
    for (int i = 0; i < count && used < _countof(line) - 40; i++) {
        wchar_t buf[96] = {};
        wchar_t piece[104] = {};
        if (IsSeparatorItem(menu, i))
            wcscpy_s(piece, L"---");
        else if (GetMenuStringW(menu, i, buf, _countof(buf), MF_BYPOSITION) > 0) {
            MENUITEMINFOW mii = {};
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_SUBMENU;
            GetMenuItemInfoW(menu, i, TRUE, &mii);
            swprintf_s(piece, mii.hSubMenu ? L"%s >" : L"%s", buf);
        } else
            continue;
        if (used) { wcscat_s(line, L" | "); used = wcslen(line); }
        wcsncat_s(line, piece, _TRUNCATE);
        used = wcslen(line);
    }
    Wh_Log(L"[menu] native menu order: %s", line);
}

static void EnhanceTaskbarMenu(HMENU menu) {
    HMENU popup = GetSubMenu(menu, 0);
    if (!popup) popup = menu;

    for (int i = GetMenuItemCount(popup) - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_FTYPE | MIIM_ID | MIIM_SUBMENU;
        if (GetMenuItemInfoW(popup, i, TRUE, &mii) &&
            !(mii.fType & MFT_SEPARATOR) && !mii.hSubMenu) {
            DeleteMenu(popup, i, MF_BYPOSITION);
        }
    }


    LogTaskbarMenuOrder(popup);



    wchar_t buf[256] = {};
    wchar_t* text = nullptr;


    text = LoadStr(g_shell32, IDS_TASKMANAGER, buf, _countof(buf));
    AppendMenuW(popup, MF_STRING, IDM_TASKMANAGER, text ? text : L"Task Manager");
    AppendMenuW(popup, MF_SEPARATOR, 0, nullptr);

    wchar_t lockBuf[256] = {};
    AppendMenuW(popup, MF_STRING, IDM_LOCKTASKBAR,
                GetLockToolbarsText(lockBuf, _countof(lockBuf)) ? lockBuf : L"Lock the taskbar");

    text = LoadStr(g_bthprops, IDS_SETTINGS, buf, _countof(buf));
    AppendMenuW(popup, MF_STRING, IDM_SETTINGS, text ? text : L"Taskbar settings");
}


static bool MenuContainsId(HMENU menu, UINT id) {
    const int count = menu ? GetMenuItemCount(menu) : -1;
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_ID | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(menu, i, TRUE, &mii)) continue;
        if (mii.wID == id) return true;
        if (mii.hSubMenu && MenuContainsId(mii.hSubMenu, id)) return true;
    }
    return false;
}


static bool ToggleDesktopLikeWin10() {
    // CLSID_Shell and IID_IDispatch written by hand so as not to depend on the
    // uuid libraries in the build flags.
    static const GUID kClsidShell = { 0x13709620, 0xC279, 0x11CE,
                                      { 0xA4, 0x9E, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
    static const GUID kIidDispatch = { 0x00020400, 0x0000, 0x0000,
                                       { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
    static const GUID kIidNull = { 0x00000000, 0x0000, 0x0000,
                                   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
    IDispatch* shell = nullptr;
    const HRESULT hr = CoCreateInstance(kClsidShell, nullptr,
                                        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
                                        kIidDispatch, (void**)&shell);
    if (SUCCEEDED(hr) && shell) {
        OLECHAR* name = const_cast<OLECHAR*>(L"ToggleDesktop");
        DISPID dispid = 0;
        if (SUCCEEDED(shell->GetIDsOfNames(kIidNull, &name, 1, LOCALE_USER_DEFAULT, &dispid))) {
            DISPPARAMS params = {};
            VARIANT result = {};
            const HRESULT hrInvoke = shell->Invoke(dispid, kIidNull, LOCALE_USER_DEFAULT,
                                                   DISPATCH_METHOD, &params, &result, nullptr, nullptr);
            shell->Release();
            return SUCCEEDED(hrInvoke);
        }
        shell->Release();
    }
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        DWORD_PTR ignored = 0;
        SendMessageTimeoutW(tray, WM_COMMAND, MAKEWPARAM(IDM_SHOWDESKTOP, 0), 0,
                            SMTO_ABORTIFHUNG, 1000, &ignored);
        return true;
    }
    return false;
}

// --- the show desktop button and the clock: the two menus of Windows 10 ----
// Everything here is cosmetic on the surface and careful underneath: nothing is
// written except the two documented registry values, and every call is inside a
// try/catch with RAII. What the Windows 10 menus contain, and the ids explorer
// uses, come from the shipped binary (see the note at the top of the file).

// (the four texts and the insertion helper are defined above, together with
// the ids, because the menu is built before this code runs)

// HKCU keys the shell itself uses for Peek (Explorer\\Advanced\\DisablePreviewDesktop
// and DWM\\EnableAeroPeek). Explorer keeps the answer in its settings cache, so a
// change is announced with the group name its own code compares against
// ("SettingsCacheChangeMessage"), which is how the toggle applies without a
// shell restart on the builds that listen for it.
static const wchar_t* const kPeekKeyEsc = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
static const wchar_t* const kPeekKeyDwm = L"Software\\Microsoft\\Windows\\DWM";
static const wchar_t* const kPeekCacheGroup = L"SettingsCacheChangeMessage";

class ScopedHKey {
public:
    explicit ScopedHKey(HKEY k = nullptr) : m_k(k) {}
    ~ScopedHKey() { if (m_k) RegCloseKey(m_k); }
    ScopedHKey(const ScopedHKey&) = delete;
    ScopedHKey& operator=(const ScopedHKey&) = delete;
    HKEY* receive() { return &m_k; }
    HKEY get() const { return m_k; }
    bool valid() const { return m_k != nullptr; }
private:
    HKEY m_k;
};

static bool ReadRegDwordHkcu(const wchar_t* sub, const wchar_t* value, DWORD* out) {
    if (!sub || !value || !out) return false;
    ScopedHKey key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, sub, 0, KEY_QUERY_VALUE, key.receive()) != ERROR_SUCCESS)
        return false;
    if (!key.valid()) return false;
    DWORD type = 0, size = sizeof(DWORD), v = 0;
    const LONG r = RegQueryValueExW(key.get(), value, nullptr, &type, (BYTE*)&v, &size);
    if (r != ERROR_SUCCESS || type != REG_DWORD || size != sizeof(DWORD)) return false;
    *out = v;
    return true;
}

static bool WriteRegDwordHkcu(const wchar_t* sub, const wchar_t* value, DWORD v) {
    if (!sub || !value) return false;
    ScopedHKey key;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, sub, 0, nullptr, 0, KEY_SET_VALUE, nullptr,
                        key.receive(), nullptr) != ERROR_SUCCESS)
        return false;
    if (!key.valid()) return false;
    return RegSetValueExW(key.get(), value, 0, REG_DWORD, (const BYTE*)&v, sizeof(v)) == ERROR_SUCCESS;
}

// Peek is ON when DisablePreviewDesktop is 0, exactly as the shell reads it.
static bool PeekAtDesktopEnabled() {
    DWORD disable = 0;
    if (!ReadRegDwordHkcu(kPeekKeyEsc, L"DisablePreviewDesktop", &disable)) return true;
    return disable == 0;
}

// The shell greys the entry out when Aero Peek is off at system level.
static bool PeekAeroAllowed() {
    DWORD on = 1;
    if (!ReadRegDwordHkcu(kPeekKeyDwm, L"EnableAeroPeek", &on)) return true;
    return on != 0;
}

static bool TogglePeekAtDesktop(bool* nowEnabled) {
    const bool want = !PeekAtDesktopEnabled();
    bool ok = WriteRegDwordHkcu(kPeekKeyEsc, L"DisablePreviewDesktop", want ? 0 : 1);
    if (want) ok = WriteRegDwordHkcu(kPeekKeyDwm, L"EnableAeroPeek", 1) && ok;
    DWORD_PTR ignored = 0;
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)kPeekCacheGroup,
                        SMTO_ABORTIFHUNG, 800, &ignored);
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray)
        SendMessageTimeoutW(tray, WM_SETTINGCHANGE, 0, (LPARAM)kPeekCacheGroup,
                            SMTO_ABORTIFHUNG, 800, &ignored);
    if (nowEnabled) *nowEnabled = want;
    return ok;
}


// ===========================================================================
// ShellOpGuard - isolates shell namespace launches from menu callbacks.
//
// The caller has a configurable wait cap; a timed-out ShellExecute operation is
// not forcibly terminated. Its worker and thread handle stay tracked, and unload
// joins every outstanding worker before the mod DLL can be released. This avoids
// returning from Wh_ModUninit while a worker can still execute mod code.
// C++ try/catch is used; no SEH/VEH handler or native access-violation recovery is installed.
//
// Rules:
//   * ShellExecute runs on a worker, not on the menu thread;
//   * the configured timeout bounds the caller's wait, not the OS operation;
//   * on unload, all workers are joined (unload can wait for a slow Shell API);
//   * no C++ exception escapes these wrappers. Success is reported only when
//     ShellExecuteW returns a value > 32.
// ===========================================================================
namespace ShellOpGuard {

enum class Outcome : int {
    Completed = 0,
    Failed,
    TimedOut,
    CppException,
    NotStarted,
};

using Body = void (WINAPI*)(void*);

struct WorkSpec {
    Body body = nullptr;
    void* context = nullptr;
    void (*contextDelete)(void*) = nullptr;
    INT_PTR (*contextResult)(const void*) = nullptr;
};

struct WorkItem {
    WorkSpec spec{};
    unsigned long fault = 0;
    std::atomic<int> state{0};   // 0 = running, 1 = finished, 2 = detached by caller
};

static std::atomic<int> g_liveWorkers{0};
static std::atomic<int> g_detachedWorkers{0};
static std::atomic<bool> g_guardStopping{false};
static std::atomic<unsigned int> g_faultLogs{0};
static std::atomic<unsigned int> g_activeRunCalls{0};
static std::mutex g_runGateMutex;
static std::mutex g_workerHandlesMutex;
static std::vector<HANDLE> g_detachedWorkerHandles;

static const wchar_t* OutcomeName(Outcome outcome) noexcept {
    switch (outcome) {
        case Outcome::Completed:    return L"completed";
        case Outcome::Failed:       return L"not completed";
        case Outcome::TimedOut:     return L"late; worker retained until it exits";
        case Outcome::CppException: return L"C++ exception";
        default:                    return L"not started";
    }
}

static bool TryBeginRun() noexcept {
    try {
        std::lock_guard<std::mutex> lock(g_runGateMutex);
        if (g_guardStopping.load(std::memory_order_acquire)) return false;
        g_activeRunCalls.fetch_add(1, std::memory_order_acq_rel);
        return true;
    } catch (...) {
        return false;
    }
}

static void EndRun() noexcept {
    g_activeRunCalls.fetch_sub(1, std::memory_order_acq_rel);
}

class ActiveRunScope {
public:
    ActiveRunScope() noexcept = default;
    ~ActiveRunScope() { if (m_active) EndRun(); }
    ActiveRunScope(const ActiveRunScope&) = delete;
    ActiveRunScope& operator=(const ActiveRunScope&) = delete;
    void activate() noexcept { m_active = true; }
private:
    bool m_active = false;
};

static void DeleteContextNoThrow(const WorkSpec& spec) noexcept {
    if (!spec.contextDelete || !spec.context) return;
    try {
        spec.contextDelete(spec.context);
    } catch (...) {
        Wh_Log(L"[shell-guard] C++ exception while releasing an operation context");
    }
}

// The thread handle is retained even after the caller's timeout. A signaled
// handle can be closed immediately; otherwise Wh_ModUninit waits on it.
static void KeepWorkerHandleUntilExit(HANDLE thread) noexcept {
    if (!thread) return;
    if (WaitForSingleObject(thread, 0) == WAIT_OBJECT_0) {
        CloseHandle(thread);
        return;
    }

    bool stored = false;
    try {
        std::lock_guard<std::mutex> lock(g_workerHandlesMutex);
        for (size_t i = 0; i < g_detachedWorkerHandles.size();) {
            if (WaitForSingleObject(g_detachedWorkerHandles[i], 0) == WAIT_OBJECT_0) {
                CloseHandle(g_detachedWorkerHandles[i]);
                g_detachedWorkerHandles.erase(g_detachedWorkerHandles.begin() + i);
            } else {
                ++i;
            }
        }
        g_detachedWorkerHandles.push_back(thread);
        stored = true;
    } catch (...) {
        Wh_Log(L"[shell-guard] could not track a late worker; waiting for it before returning");
    }

    if (!stored) {
        // Safer to delay the caller in this rare allocation/locking failure than
        // to let the DLL unload while this worker can still execute its code.
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

static DWORD WINAPI WorkThreadProc(LPVOID raw) noexcept {
    WorkItem* item = static_cast<WorkItem*>(raw);
    g_liveWorkers.fetch_add(1, std::memory_order_acq_rel);
    try {
        DWORD cppFault = 0;
        if (!CppGuard::RunGuarded(L"shell:::", item->spec.body,
                                  item->spec.context, &cppFault)) {
            item->fault = cppFault ? cppFault : CppGuard::kCppExceptionCode;
        }
    } catch (...) {
        item->fault = CppGuard::kCppExceptionCode;
    }

    int expected = 0;
    if (!item->state.compare_exchange_strong(expected, 1, std::memory_order_acq_rel)) {
        // The caller timed out and transferred WorkItem/context ownership here.
        DeleteContextNoThrow(item->spec);
        delete item;
        g_detachedWorkers.fetch_sub(1, std::memory_order_acq_rel);
    }
    g_liveWorkers.fetch_sub(1, std::memory_order_acq_rel);
    return 0;
}

// Executes a shell operation on a worker. A caller timeout does not terminate
// the worker: its handle is retained so the module can join it before unload.
static Outcome Run(PCWSTR what, const WorkSpec& spec, DWORD timeoutMs,
                   INT_PTR* outCode, DWORD* outError) noexcept {
    if (outCode) *outCode = 0;
    if (outError) *outError = 0;
    if (!spec.body || !spec.context) {
        DeleteContextNoThrow(spec);
        return Outcome::NotStarted;
    }
    if (!TryBeginRun()) {
        DeleteContextNoThrow(spec);
        return Outcome::NotStarted;
    }
    ActiveRunScope activeRun;
    activeRun.activate();

    if (timeoutMs < 200) timeoutMs = 200;

    WorkItem* item = nullptr;
    try {
        item = new WorkItem();
    } catch (...) {
        Wh_Log(L"[shell-guard] %s: work item cannot be allocated; operation cancelled", what);
        DeleteContextNoThrow(spec);
        return Outcome::NotStarted;
    }
    item->spec = spec;

    HANDLE thread = CreateThread(nullptr, 0, WorkThreadProc, item, 0, nullptr);
    if (!thread) {
        const DWORD error = GetLastError();
        DeleteContextNoThrow(spec);
        delete item;
        Wh_Log(L"[shell-guard] %s: service thread not created (%lu)", what, error);
        return Outcome::NotStarted;
    }

    const DWORD waited = WaitForSingleObject(thread, timeoutMs);
    Outcome outcome = Outcome::Completed;
    unsigned long fault = 0;
    if (waited == WAIT_OBJECT_0) {
        if (outCode && spec.contextResult) *outCode = spec.contextResult(item->spec.context);
        fault = item->fault;
        if (fault) outcome = Outcome::CppException;
        DeleteContextNoThrow(item->spec);
        item->spec.context = nullptr;
        delete item;
        item = nullptr;
    } else if (waited == WAIT_TIMEOUT) {
        int expected = 0;
        if (item->state.compare_exchange_strong(expected, 2, std::memory_order_acq_rel)) {
            g_detachedWorkers.fetch_add(1, std::memory_order_acq_rel);
            item = nullptr;  // worker now owns item and its context
            outcome = Outcome::TimedOut;
        } else {
            // Finished concurrently with the timeout, but the thread may still be
            // executing its final instructions; keep its handle until termination.
            if (outCode && spec.contextResult) *outCode = spec.contextResult(item->spec.context);
            fault = item->fault;
            if (fault) outcome = Outcome::CppException;
            DeleteContextNoThrow(item->spec);
            item->spec.context = nullptr;
            delete item;
            item = nullptr;
        }
    } else {
        int expected = 0;
        if (item->state.compare_exchange_strong(expected, 2, std::memory_order_acq_rel)) {
            g_detachedWorkers.fetch_add(1, std::memory_order_acq_rel);
            item = nullptr;
        } else {
            fault = item->fault;
            DeleteContextNoThrow(item->spec);
            item->spec.context = nullptr;
            delete item;
            item = nullptr;
        }
        outcome = Outcome::Failed;
    }

    if (waited == WAIT_OBJECT_0) {
        CloseHandle(thread);
    } else {
        KeepWorkerHandleUntilExit(thread);
    }

    if (fault && g_faultLogs.fetch_add(1, std::memory_order_acq_rel) < 8)
        Wh_Log(L"[shell-guard] %s: C++ exception caught (0x%08X)", what, fault);
    if (waited == WAIT_TIMEOUT && outcome == Outcome::TimedOut)
        Wh_Log(L"[shell-guard] %s: caller timed out after %lu ms; worker continues, "
               L"its handle is retained for safe unload", what, timeoutMs);
    else if (waited != WAIT_OBJECT_0 && outcome == Outcome::Failed)
        Wh_Log(L"[shell-guard] %s: wait failed (%lu); worker handle retained for safe unload",
               what, GetLastError());
    return outcome;
}

// Called from Wh_ModBeforeUninit, before Windhawk removes the hooks. New shell operations are refused.
static void BeginShutdown() noexcept {
    try {
        std::lock_guard<std::mutex> lock(g_runGateMutex);
        g_guardStopping.store(true, std::memory_order_release);
    } catch (...) {
        g_guardStopping.store(true, std::memory_order_release);
    }
}

// Wait for all Run callers to finish registering their handles, then join every
// timed-out worker. A slow Shell API can delay unload; it cannot outlive the DLL.
static void Shutdown() noexcept {
    BeginShutdown();
    while (g_activeRunCalls.load(std::memory_order_acquire) != 0) Sleep(10);

    for (;;) {
        HANDLE thread = nullptr;
        try {
            std::lock_guard<std::mutex> lock(g_workerHandlesMutex);
            if (g_detachedWorkerHandles.empty()) break;
            thread = g_detachedWorkerHandles.back();
            g_detachedWorkerHandles.pop_back();
        } catch (...) {
            Wh_Log(L"[shell-guard] worker-list lock raised a C++ exception; retrying");
            Sleep(10);
            continue;
        }
        if (thread) {
            const DWORD waited = WaitForSingleObject(thread, INFINITE);
            if (waited != WAIT_OBJECT_0)
                Wh_Log(L"[shell-guard] WaitForSingleObject on a worker failed (%lu)", GetLastError());
            CloseHandle(thread);
        }
    }
    Wh_Log(L"[shell-guard] all late shell workers have exited; safe to unload");
}

// ---- corpo concreto: apertura di un URL shell / pannello -------------------
// 1.0.0: the strings are copied into the context, not referenced. A worker that
// overruns the timeout is detached and keeps running: it must not read a caller
// buffer (or a caller stack frame) that no longer exists.
struct OpenUriCall {
    wchar_t uri[512] = {};
    INT_PTR code = 0;
    DWORD error = 0;
};

static void OpenUriBody(void* raw) {
    OpenUriCall* call = static_cast<OpenUriCall*>(raw);
    SetLastError(0);
    const HINSTANCE result = ShellExecuteW(nullptr, L"open", call->uri,
                                           nullptr, nullptr, SW_SHOWNORMAL);
    call->code = reinterpret_cast<INT_PTR>(result);
    call->error = GetLastError();
}

static void DeleteOpenUriCall(void* raw) { delete static_cast<OpenUriCall*>(raw); }
static INT_PTR ResultOfOpenUriCall(const void* raw) {
    return static_cast<const OpenUriCall*>(raw)->code;
}

static DWORD GuardTimeoutMs() noexcept {
    int configured = g_cfg.shellOpGuardTimeoutMs;
    if (configured < 200) configured = 200;
    if (configured > 10000) configured = 10000;
    return static_cast<DWORD>(configured);
}

// Apre un URL "shell:::" o un pannello dei nomi di shell senza mai propagare
// eccezioni e senza bloccare il chiamante oltre il tetto di tempo.
static bool OpenShellUriGuarded(const wchar_t* uri, const wchar_t* what) noexcept {
    if (!uri || !*uri) return false;
    OpenUriCall* call = nullptr;
    try {
        call = new OpenUriCall();
    } catch (...) {
        Wh_Log(L"[shell-guard] %s: context cannot be allocated", what);
        return false;
    }
    wcsncpy_s(call->uri, uri, _TRUNCATE);   // copia: il worker puo' sopravvivere al chiamante
    if (!call->uri[0]) { delete call; return false; }

    const WorkSpec spec{ OpenUriBody, call, DeleteOpenUriCall, ResultOfOpenUriCall };
    INT_PTR code = 0;
    DWORD error = 0;
    const Outcome outcome = Run(what, spec, GuardTimeoutMs(), &code, &error);

    if (outcome == Outcome::Completed && code > 32) {
        Wh_Log(L"[shell-guard] %s: opened (%s)", what, uri);
        return true;
    }
    if (outcome == Outcome::TimedOut || outcome == Outcome::Failed) return false;
    if (code <= 32) {
        Wh_Log(L"[shell-guard] %s: the shell refused to open it (code %ld, error %lu): %s",
               what, static_cast<long>(code), error, uri);
    } else {
        Wh_Log(L"[shell-guard] %s: %s", what, OutcomeName(outcome));
    }
    return false;
}

// ---- corpo concreto: comando con argomenti (msdt.exe e simili) -------------
struct RunCommandCall {
    wchar_t file[320] = {};
    wchar_t params[512] = {};
    wchar_t verb[16] = L"open";   // "open" oppure "runas" per le voci amministrative
    INT_PTR code = 0;
    DWORD error = 0;
};

static void RunCommandBody(void* raw) {
    RunCommandCall* call = static_cast<RunCommandCall*>(raw);
    SetLastError(0);
    const HINSTANCE result = ShellExecuteW(nullptr, call->verb, call->file, call->params,
                                           nullptr, SW_SHOWNORMAL);
    call->code = reinterpret_cast<INT_PTR>(result);
    call->error = GetLastError();
}

static void DeleteRunCommandCall(void* raw) { delete static_cast<RunCommandCall*>(raw); }
static INT_PTR ResultOfRunCommandCall(const void* raw) {
    return static_cast<const RunCommandCall*>(raw)->code;
}

// Esegue un comando esterno con la stessa protezione delle operazioni shell:::
// thread di servizio, tetto di attesa e catch C++ (nessun SEH/VEH).
static bool RunCommandGuarded(const wchar_t* file, const wchar_t* params,
                              const wchar_t* verb, const wchar_t* what) noexcept {
    const wchar_t* callVerb = (verb && *verb) ? verb : L"open";
    if (!file || !*file) return false;
    RunCommandCall* call = nullptr;
    try {
        call = new RunCommandCall();
    } catch (...) {
        Wh_Log(L"[shell-guard] %s: context cannot be allocated", what);
        return false;
    }
    // Copies: the context outlives the caller when the worker is detached.
    wcsncpy_s(call->file, file, _TRUNCATE);
    if (params) wcsncpy_s(call->params, params, _TRUNCATE);
    if (verb && *verb) wcsncpy_s(call->verb, verb, _TRUNCATE);
    if (!call->file[0]) { delete call; return false; }
    const WorkSpec spec{ RunCommandBody, call, DeleteRunCommandCall, ResultOfRunCommandCall };
    INT_PTR code = 0;
    DWORD error = 0;
    const Outcome outcome = Run(what, spec, GuardTimeoutMs(), &code, &error);
    if (outcome == Outcome::Completed && code > 32) {
        Wh_Log(L"[shell-guard] %s: started (%s %s %s)", what, callVerb, file,
               params ? params : L"");
        return true;
    }
    if (outcome == Outcome::TimedOut || outcome == Outcome::Failed) return false;
    Wh_Log(L"[shell-guard] %s: start failed (code %ld, error %lu)", what,
           static_cast<long>(code), error);
    return false;
}

// Accordo iniettato: usato dalle voci che devono per forza passare da una scorciatoia
// di Windows (Esegui e Desktop nel menu Win+X).
static void InjectSystemChord(wchar_t key) noexcept {
    try {
        INPUT in[4] = {};
        for (INPUT& i : in) i.type = INPUT_KEYBOARD;
        in[0].ki.wVk = VK_LWIN;
        in[1].ki.wVk = static_cast<WORD>(key);
        in[2].ki.wVk = static_cast<WORD>(key);   in[2].ki.dwFlags = KEYEVENTF_KEYUP;
        in[3].ki.wVk = VK_LWIN;                  in[3].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(4, in, sizeof(INPUT));
    } catch (...) {
    }
}

}  // namespace ShellOpGuard





// Who asked for the menu, so that the show desktop button and the clock can be
// told apart from every other window of the tray.
static bool IsShowDesktopClass(const wchar_t* cls) {
    return cls && _wcsicmp(cls, L"TrayShowDesktopButtonWClass") == 0;
}














static int g_peekMenuLogs = 0;


// The menu of the show desktop button: the same two entries of Windows 10, with
// the same behaviour (checkmark, greyed out when Aero Peek is off).
static void ShowShowDesktopMenu(HWND owner) {
    const UiLangId lang = DetectUiLang();
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    AppendMenuW(menu, MF_STRING, IDM_MOD_SHOWDESKTOP, kShowDesktopText[lang]);
    SetMenuDefaultItem(menu, IDM_MOD_SHOWDESKTOP, FALSE);
    UINT flags = MF_STRING | (PeekAtDesktopEnabled() ? MF_CHECKED : 0);
    if (!PeekAeroAllowed()) flags |= MF_GRAYED;
    AppendMenuW(menu, flags, IDM_MOD_PEEK, kPeekText[lang]);
    if (g_peekMenuLogs < 5) {
        g_peekMenuLogs++;
        Wh_Log(L"[menu] show desktop button: menu shown (peek %s, Aero Peek %s)",
               PeekAtDesktopEnabled() ? L"on" : L"off", PeekAeroAllowed() ? L"available" : L"off");
    }
    POINT pt = {};
    if (!GetCursorPos(&pt)) GetCursorPos(&pt);
    const UINT command = (UINT)TrackPopupMenuEx(
        menu, TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, owner, nullptr);
    if (command) HandleClassicMenuCommand(command);
    DestroyMenu(menu);
}






static bool HandleClassicMenuCommand(UINT id) {
    try {
        const wchar_t* label = nullptr;
        bool ok = false;
        switch (id) {

        case IDM_MOD_SHOWDESKTOP:
            label = L"show desktop";
            ok = ToggleDesktopLikeWin10();
            break;
        case IDM_MOD_PEEK: {
            bool nowOn = false;
            ok = TogglePeekAtDesktop(&nowOn);
            Wh_Log(L"[menu] peek at desktop turned %s%s", nowOn ? L"on" : L"off",
                   ok ? L"" : L" (the registry write failed)");
            label = L"peek at desktop";
            break;
        }

        case IDM_MOD_BATTERY_POWER:
        case IDM_MOD_BATTERY_MOBILITY:
            return true;
        default:
            return false;
        }
        if (g_classicCmdLogs < 10) {
            g_classicCmdLogs++;
            Wh_Log(L"[menu] classic entry chosen: %s (%s)", label, ok ? L"done" : L"failed");
        }
        return true;
    } catch (...) {
        return false;
    }
}

// --- network icon menu: pnidui's resource 3014 ------------------------------
// Evidence (pnidui.dll 10.0.19041.7663, disassembled):
//   on right click pnidui calls LoadMenuW(<pnidui>, 3014), takes
//   submenu 0, shows it with TrackPopupMenu and handles the entries itself
//   0x0C23 (3107) e 0x0C25 (3109).
// Resource 3014 is NOT inside pnidui.dll (the DLL has icons only): it is in
// its MUI, pnidui.dll.mui, which no longer exists on 24H2. Without the MUI LoadMenuW
// returns NULL, pnidui leaves the handler and the right click opens nothing:
// that is the cause of the missing menu.
// Here the resource is handed back to it, byte for byte as in the official
// Windows 10 MUI (10.0.19041.1): 140 bytes for Italian, 142 for English.
// The format is the resource format, so LoadMenuIndirectW is fine.
// If it fails to load, the same structure is rebuilt (two entries, same ids)
// with AppendMenuW: pnidui finds the menu it expects anyway.
static int g_netMenuGiven = 0;

// Build the network right-click menu (pnidui resource 3014) in the user's
// UI language. Binary templates kept for IT/EN (byte-exact MUI copies);
// other languages built by hand with AppendMenuW using multilingual tables.
// Command ids (3107 troubleshoot, 3109 settings) are pnidui's native ids.
// 140 bytes - Italian MUI 10.0.19041.1
static const unsigned char kNetMenu3014_it[] = {
    0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0x5F, 0x00, 0x50, 0x00, 0x4F, 0x00,
    0x50, 0x00, 0x5F, 0x00, 0x55, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x23, 0x0C, 0x52, 0x00, 0x69, 0x00, 0x73, 0x00, 0x6F, 0x00, 0x6C, 0x00,
    0x75, 0x00, 0x7A, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x65, 0x00,
    0x20, 0x00, 0x70, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x62, 0x00, 0x6C, 0x00,
    0x65, 0x00, 0x6D, 0x00, 0x69, 0x00, 0x00, 0x00, 0x80, 0x00, 0x25, 0x0C,
    0x41, 0x00, 0x70, 0x00, 0x72, 0x00, 0x69, 0x00, 0x20, 0x00, 0x69, 0x00,
    0x6D, 0x00, 0x70, 0x00, 0x6F, 0x00, 0x73, 0x00, 0x74, 0x00, 0x61, 0x00,
    0x7A, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x69, 0x00, 0x20, 0x00,
    0x52, 0x00, 0x65, 0x00, 0x74, 0x00, 0x65, 0x00, 0x20, 0x00, 0x65, 0x00,
    0x20, 0x00, 0x49, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00,
    0x6E, 0x00, 0x65, 0x00, 0x74, 0x00, 0x00, 0x00,
};
// 142 bytes - English MUI 10.0.19041.1
static const unsigned char kNetMenu3014_en[] = {
    0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0x5F, 0x00, 0x50, 0x00, 0x4F, 0x00,
    0x50, 0x00, 0x5F, 0x00, 0x55, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x23, 0x0C, 0x54, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x75, 0x00, 0x62, 0x00,
    0x6C, 0x00, 0x65, 0x00, 0x73, 0x00, 0x68, 0x00, 0x6F, 0x00, 0x6F, 0x00,
    0x74, 0x00, 0x20, 0x00, 0x70, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x62, 0x00,
    0x6C, 0x00, 0x65, 0x00, 0x6D, 0x00, 0x73, 0x00, 0x00, 0x00, 0x80, 0x00,
    0x25, 0x0C, 0x4F, 0x00, 0x70, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x20, 0x00,
    0x4E, 0x00, 0x65, 0x00, 0x74, 0x00, 0x77, 0x00, 0x6F, 0x00, 0x72, 0x00,
    0x6B, 0x00, 0x20, 0x00, 0x26, 0x00, 0x26, 0x00, 0x20, 0x00, 0x49, 0x00,
    0x6E, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x6E, 0x00, 0x65, 0x00,
    0x74, 0x00, 0x20, 0x00, 0x73, 0x00, 0x65, 0x00, 0x74, 0x00, 0x74, 0x00,
    0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x73, 0x00, 0x00, 0x00,
};

static HMENU BuildNetworkIconMenu() {
    const UiLangId lang = DetectUiLang();
    const unsigned char* tpl = (lang == LANG_IT) ? kNetMenu3014_it
                              : (lang == LANG_EN) ? kNetMenu3014_en : nullptr;
    if (tpl) {
        HMENU menu = LoadMenuIndirectW((const void*)tpl);
        if (menu) return menu;
    }
    HMENU bar = CreateMenu();
    HMENU popup = CreatePopupMenu();
    if (!bar || !popup) {
        if (bar) DestroyMenu(bar);
        if (popup) DestroyMenu(popup);
        return nullptr;
    }
    AppendMenuW(popup, MF_STRING, 3107, kNetTroubleshoot[lang]);
    AppendMenuW(popup, MF_STRING, 3109, kNetOpenSettings[lang]);
    AppendMenuW(bar, MF_POPUP | MF_STRING, (UINT_PTR)popup, L"");
    return bar;
}

static HMENU WINAPI LoadMenuW_Hook(HINSTANCE hInstance, LPCWSTR lpMenuName) {
    try {
        // pnidui's resource 3014: it is the right-click menu of the network icon
        HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
        if (pnidui && IS_INTRESOURCE(lpMenuName) && (HMODULE)hInstance == pnidui &&
            (UINT)(ULONG_PTR)lpMenuName == 3014) {
            HMENU net = BuildNetworkIconMenu();
            if (net) {
                if (g_netMenuGiven < 3) {
                    g_netMenuGiven++;
                    Wh_Log(L"[network] network icon menu handed to pnidui (resource 3014: troubleshoot + settings)");
                }
                return net;
            }
            Wh_Log(L"[network] resource 3014 not loaded: pnidui stays without a menu");
        }
        if (IS_INTRESOURCE(lpMenuName) && (HMODULE)hInstance == GetModuleHandleW(nullptr) && g_shell32) {
            UINT id = (UINT)(ULONG_PTR)lpMenuName;
            if (id == 205 || id == 206) {
                // Il menu del pulsante Start (Win+X) resta quello nativo: quando il clic
                // e' sul pulsante Start la risorsa dell'eseguibile non viene sostituita.
                // Quel menu e' di un altro modulo: questo mod non lo tocca.
                if (!IsWinXNativeContextMenuRequest()) {
                    HMENU result = LoadMenuW_Original(g_shell32, MAKEINTRESOURCEW(205));
                    if (result) {
                        EnhanceTaskbarMenu(result);   // covered by the try/catch of this function
                        return result;
                    }
                }
            }
        }

    } catch (...) {
        Wh_Log(L"[menu] exception, using the original resource");
    }
    return LoadMenuW_Original(hInstance, lpMenuName);
}

// --- 3) language indicator fix: removed from this mod (1.3.9). It lives in the separate
// mod "windows-10-language-flyout-guard"; the hooks it needed were never registered here.

// --- 4) notification crash fix (ilovethisgame's mod), exact-build gated ---
// Signature identical to ilovethisgame's original (no WINAPI convention:
// on x86-64 there is only one anyway, but the match must stay exact).
typedef HRESULT (*NotificationsAdded_t)(void* pthis, uint32_t arg2, void* const* iterator, uint32_t length);
static NotificationsAdded_t NotificationsAdded_Original = nullptr;

static HRESULT NotificationsAdded_Hook(void* pthis, uint32_t arg2, void* const* iterator, uint32_t length) {
    (void)pthis;
    (void)arg2;
    (void)iterator;
    try {
        Wh_Log(L"[notif] %u notifications suppressed by the compatibility hook (legacy shell)", length);
    } catch (...) {
        // No C++ exception is allowed to escape this callback.
    }
    return S_OK;
}

static bool InstallNotificationCrashFix() {
    if (!g_cfg.fixNotificationsCrash) {
        Wh_Log(L"[notif] compatibility hook disabled; virtual policy fallback may be used");
        return false;
    }

    const BuildInfo* runtimeBuild = nullptr;
    for (int i = 0; i < kBuildCount; i++) {
        if (g_explorerTs == kBuilds[i].timeDateStamp &&
            g_explorerImageSize == kBuilds[i].sizeOfImage) {
            runtimeBuild = &kBuilds[i];
            break;
        }
    }

    if (!runtimeBuild || !runtimeBuild->notificationOffset) {
        Wh_Log(L"[notif] no verified offset for this running shell build; using the virtual suppression fallback");
        return false;
    }

    HMODULE module = GetModuleHandleW(nullptr);
    if (!module) {
        Wh_Log(L"[notif] main module handle unavailable; using the virtual suppression fallback");
        return false;
    }

    void* target = (void*)((BYTE*)module + runtimeBuild->notificationOffset);
    if (!Wh_SetFunctionHook(target, (void*)NotificationsAdded_Hook,
                            (void**)&NotificationsAdded_Original)) {
        Wh_Log(L"[notif] compatibility hook installation failed for %s; using the virtual suppression fallback",
               runtimeBuild->label);
        return false;
    }

    g_notificationCrashFixActive.store(true, std::memory_order_release);
    Wh_Log(L"[notif] compatibility fix active for verified runtime build %s (base+0x%X)",
           runtimeBuild->label, runtimeBuild->notificationOffset);
    return true;
}

// --- 5) network: guarded routing for non-PNI network launches --------------
// The old generic workaround can still rewrite a network-settings launch from
// another caller. A click delivered to pnidui's real PNIHiddenWnd is different:
// PNI-origin URI fallbacks are blocked below, and the dynamic icon fallback
// never installs its own click handler or launches a URI.
static const wchar_t* kNetworkSettingsUri = L"ms-settings:network";
static const wchar_t* kNetworkFlyoutUri = L"ms-availablenetworks:";

// The comparison used to be exact: "ms-settings:network-wifi" or
// "ms-settings:network-status" slipped through and opened Settings.
static bool IsNetworkSettingsUri(LPCWSTR target) {
    return target && _wcsnicmp(target, kNetworkSettingsUri, wcslen(kNetworkSettingsUri)) == 0;
}

static bool IsPniduiCallerAddress(const void* address) noexcept {
    try {
        HMODULE caller = nullptr;
        const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
        if (!address || !GetModuleHandleExW(flags, (LPCWSTR)address, &caller) || !caller) return false;
        HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
        return pnidui && caller == pnidui;
    } catch (...) { return false; }
}

static bool IsPniduiNetworkUri(LPCWSTR target) {
    return IsNetworkSettingsUri(target) ||
           (target && _wcsnicmp(target, kNetworkFlyoutUri, wcslen(kNetworkFlyoutUri)) == 0);
}

// 1.3.2: every network page of this shell is answered with the Windows 10 flyout, for every
// caller. The list is explicit on purpose: a target that is not recognised here is left alone.
static bool IsBlockedNetworkPage(LPCWSTR target) {
    if (IsPniduiNetworkUri(target)) return true;   // ms-settings:network* and ms-availablenetworks:
    static const wchar_t* const kPages[] = {
        L"ms-settings:wifi",          L"ms-settings:ethernet",    L"ms-settings:vpn",
        L"ms-settings:airplanemode",  L"ms-settings:datausage",   L"ms-settings:cellulardata",
        L"ms-settings:mobilehotspot", L"ms-settings:proximity",   L"ms-settings:nfctransactions",
    };
    for (const wchar_t* page : kPages)
        if (target && _wcsnicmp(target, page, wcslen(page)) == 0) return true;
    return false;
}

namespace Win10Restyle {
void Arm();
void Shutdown();
}
// 1.3.2: the click on the network icon opens the genuine Windows 10 flyout through the shell
// experience manager. It is defined further down, inside NetworkTrayForce (it needs the tray
// icon rectangle and the PNI registration), and declared here in that same namespace: a
// declaration at this level would name a different function, which nothing ever defines.
namespace NetworkTrayForce {
static HRESULT OpenAuthenticNetworkFlyout(const wchar_t* reason) noexcept;
static UINT ShowNetworkFlyoutMessage() noexcept;
static bool RequestNetworkFlyout() noexcept;
}
typedef HINSTANCE(WINAPI* ShellExecuteW_t)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
static ShellExecuteW_t ShellExecuteW_Original = nullptr;

static HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR operation, LPCWSTR file,
                                           LPCWSTR parameters, LPCWSTR directory, INT show) {
    try {
        if (IsBlockedNetworkPage(file)) {
            // 1.3.2: no page, ever, and nothing that can switch it back off. Every
            // ms-settings:network* / ms-availablenetworks: target of this shell becomes a
            // request for the Windows 10 flyout, and the caller is told that it succeeded so
            // that it does not try anything else. Settings is where this used to end: from
            // here it can no longer be reached, by pnidui or by anyone else.
            const bool fromPnidui = IsPniduiCallerAddress(WhReturnAddress());
            Wh_Log(L"[network] %s: no page is launched, the Windows 10 flyout is requested "
                   L"(caller pnidui: %d)", file ? file : L"(null)", fromPnidui ? 1 : 0);
            NetworkTrayForce::RequestNetworkFlyout();
            return (HINSTANCE)33;   // > 32 = success: the caller does not retry
        }
    } catch (...) {
        Wh_Log(L"[network] exception while rewriting, using the original target");
    }
    return ShellExecuteW_Original(hwnd, operation, file, parameters, directory, show);
}

typedef BOOL(WINAPI* ShellExecuteExW_t)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExW_Original = nullptr;          // aggancio globale
static ShellExecuteExW_t ShellExecuteExW_TargetedOriginal = nullptr;  // aggancio mirato (IAT di pnidui)

static BOOL HandleNetworkShellExecute(SHELLEXECUTEINFOW* info, ShellExecuteExW_t original,
                                      bool fromPnidui) {
    if (!info || !original) return FALSE;
    try {
        if (IsBlockedNetworkPage(info->lpFile)) {
            // 1.3.2: see ShellExecuteW_Hook. No setting, no forwarding, no page.
            Wh_Log(L"[network] %s: no page is launched, the Windows 10 flyout is requested "
                   L"(caller pnidui: %d)", info->lpFile ? info->lpFile : L"(null)",
                   fromPnidui ? 1 : 0);
            NetworkTrayForce::RequestNetworkFlyout();
            info->hInstApp = (HINSTANCE)33;
            return TRUE;
        }
    } catch (...) {
        Wh_Log(L"[network] exception while rewriting, using the original target");
    }
    return original(info);
}

static BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* info) {
    return HandleNetworkShellExecute(info, ShellExecuteExW_Original,
                                     IsPniduiCallerAddress(WhReturnAddress()));
}

static BOOL WINAPI ShellExecuteExW_TargetedHook(SHELLEXECUTEINFOW* info) {
    return HandleNetworkShellExecute(info, ShellExecuteExW_TargetedOriginal,
                                     IsPniduiCallerAddress(WhReturnAddress()));
}

// Defined further down with the rest of the IAT helpers of the reference mod.
static void** FindIatSlot(HMODULE module, const char* dllHint, const char* funcHint);

// Installs the two entry points of the network click. The global hook covers every
// caller; the slot pnidui calls through its import table is hooked as well, because a
// module that keeps a copy of the address would otherwise slip past the global one.
static bool InstallShellExecuteExHooks() {
    bool installed = false;

    if (Wh_SetFunctionHook((void*)ShellExecuteExW, (void*)ShellExecuteExW_Hook,
                           (void**)&ShellExecuteExW_Original))
        installed = true;

    HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
    void** slot = pnidui ? FindIatSlot(pnidui, "SHELL32", "ShellExecuteExW") : nullptr;
    if (slot && *slot) {
        void* target = *slot;
        if (target == (void*)ShellExecuteExW) {
            Wh_Log(L"[network] ShellExecuteExW: pnidui calls the same address we hook (0x%p)", target);
        } else if (Wh_SetFunctionHook(target, (void*)ShellExecuteExW_TargetedHook,
                                      (void**)&ShellExecuteExW_TargetedOriginal)) {
            Wh_Log(L"[network] ShellExecuteExW: the address used by pnidui is hooked too (0x%p)", target);
            installed = true;
        } else {
            Wh_Log(L"[network] ShellExecuteExW: the address used by pnidui refuses to be hooked");
        }
    } else {
        Wh_Log(L"[network] ShellExecuteExW: pnidui not loaded, the global hook stays");
    }

    return installed;
}

// 1.3.2: the two entry points of the click are registered here, once, and only from
// Wh_ModInit, together with every other hook of this mod (see the note there). Both shims
// never forward a page to the shell: every ms-settings:network* and ms-availablenetworks:
// target of this shell is answered with a request for the Windows 10 flyout and with a
// success result, so nothing else is launched. The registration is not tied to a setting:
// Windhawk writes the settings of a mod when the mod is installed, so a setting added by a
// later version reads as 0 until they are saved again - and that is exactly how the old
// path (the Settings page) stayed active.
static void InstallNetworkClickHooks() {
    static bool done = false;
    if (done) return;
    done = true;

    if (Wh_SetFunctionHook((void*)ShellExecuteW, (void*)ShellExecuteW_Hook,
                           (void**)&ShellExecuteW_Original)) {
        Wh_Log(L"[init] network click hook active on ShellExecuteW: no page can be opened "
               L"from a network target, the Windows 10 flyout is requested instead");
    } else {
        Wh_Log(L"[init] ShellExecuteW hook unavailable: the click may reach the shell page");
    }

    if (InstallShellExecuteExHooks()) {
        Wh_Log(L"[init] network click hook active on ShellExecuteEx, and on the address "
               L"pnidui itself calls");
    } else {
        Wh_Log(L"[init] ShellExecuteEx hooks unavailable: the click may reach the shell page");
    }
}

// --- 8e) right click on the network icon (supervision only) ------------------
// The real menu belongs to pnidui and now arrives: it is resource 3014 handed
// over to LoadMenuW above. Nothing is invented here and no message is absorbed.
// It only serves two purposes:
//   1. to find pnidui's service window again. It is an ordinary hidden
//      top-level window (CreateWindowExW with style 0 and parent 0), so it
//      is NOT under HWND_MESSAGE: the earlier search could not find it;
//   2. to note in the log the first times the right click arrives there,
//      so the log says whether the click reaches pnidui or gets lost earlier.
// The tray callback message carries the mouse message in lParam:
// the right button is recognised without knowing the exact number.
typedef BOOL(WINAPI* Shell_NotifyIconW_t)(DWORD, PNOTIFYICONDATAW);
static Shell_NotifyIconW_t Shell_NotifyIconW_Original = nullptr;
static int g_netIconLogs = 0;
struct SeenTrayIcon { HWND wnd; UINT id; };
static SeenTrayIcon g_trayIconSeen[24] = {};
static int g_trayIconSeenCount = 0;
static HWND g_netIconWnd = nullptr;
static UINT g_netIconMsg = 0;
static bool g_netMenuLogged = false;
static int g_netClickLogs = 0;

// Remember which (hwnd,id) tray-icon registrations are the battery meter
// (tooltip at registration time), so the right-click can be intercepted
// and served with our multilang menu.
static bool IsBatteryTooltipText(const wchar_t* tip) {
    if (!tip || !*tip) return false;
    const wchar_t* markers[] = {
        L"Power", L"Alimentazione", L"Aliment", L"Battery", L"Batterie",
        L"Batteria", L"Energie", L"Énergie", L"Energ", L"Akku",
        L"Stroom", L"Питан", L"Батар",
        L"電源", L"Zasilan",
    };
    for (const wchar_t* m : markers) {
        if (wcsstr(tip, m)) return true;
    }
    return false;
}
struct TrayOwnerInfo { HWND wnd; UINT id; bool isBattery; };
static TrayOwnerInfo g_trayOwnerInfo[24] = {};
static int g_trayOwnerInfoCount = 0;
static TrayOwnerInfo* FindTrayOwnerInfo(HWND wnd, UINT id) {
    for (int i = 0; i < g_trayOwnerInfoCount; i++)
        if (g_trayOwnerInfo[i].wnd == wnd && g_trayOwnerInfo[i].id == id)
            return &g_trayOwnerInfo[i];
    return nullptr;
}
static void RememberTrayOwner(HWND wnd, UINT id, const wchar_t* tip) {
    TrayOwnerInfo* t = FindTrayOwnerInfo(wnd, id);
    if (!t) {
        if (g_trayOwnerInfoCount >= (int)_countof(g_trayOwnerInfo)) return;
        t = &g_trayOwnerInfo[g_trayOwnerInfoCount++];
        t->wnd = wnd; t->id = id; t->isBattery = false;
    }
    if (IsBatteryTooltipText(tip)) t->isBattery = true;
}
// 1.3.4: la batteria di questa shell non si annuncia con un testo. Il log della 1.3.3 lo
// mostra: la sua registrazione arriva con il testo vuoto (finestra SystemTray_Main, id 1225,
// guid {7820AE75-...}, testo ""), quindi il riconoscimento per suggerimento non la prendeva mai
// e l'icona non e' mai risultata "batteria". A nominarla e' il GUID, che non dipende dalla
// lingua del sistema: e' lo stesso GUID che il resto del mod usa per la sua icona di sistema.
static const GUID kBatteryTrayIconGuid = {
    0x7820AE75, 0x23E3, 0x4229, { 0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C }
};
// Dove la batteria si e' registrata l'ultima volta (finestra e id) e con quale messaggio
// chiama la sua finestra: servono alla presa del clic, che sta piu' sotto.
static HWND g_batteryTrayIconWnd = nullptr;
static UINT g_batteryTrayIconId = 0;
static UINT g_batteryTrayIconMessage = 0;
static int g_batteryTrayIconLogs = 0;

static bool IsBatteryTrayIconRegistration(const NOTIFYICONDATAW* data) noexcept {
    try {
        if (!data || !data->hWnd) return false;
        if (!(data->uFlags & NIF_GUID)) return false;
        if (data->cbSize < NOTIFYICONDATA_V3_SIZE) return false;
        return IsEqualGUID(data->guidItem, kBatteryTrayIconGuid) != FALSE;
    } catch (...) {
        return false;
    }
}

// Segna quell'icona come batteria e ricorda dove sta. Gira anche quando il bilancio dei log
// dell'icona di rete e' esaurito: la batteria non deve dipendere da quel bilancio.
static void RememberBatteryTrayIcon(const NOTIFYICONDATAW* data) noexcept {
    try {
        if (!IsBatteryTrayIconRegistration(data)) return;
        const bool moved =
            (g_batteryTrayIconWnd != data->hWnd) || (g_batteryTrayIconId != data->uID);
        g_batteryTrayIconWnd = data->hWnd;
        g_batteryTrayIconId = data->uID;
        if (data->uCallbackMessage) g_batteryTrayIconMessage = data->uCallbackMessage;
        TrayOwnerInfo* t = FindTrayOwnerInfo(data->hWnd, data->uID);
        if (t) t->isBattery = true;
        if (moved && g_batteryTrayIconLogs++ < 6) {
            wchar_t cls[64] = {};
            GetClassNameW(data->hWnd, cls, _countof(cls));
            Wh_Log(L"[battery] icon found: window %s, id %u, message 0x%X: its own GUID names it, "
                   L"so the empty text of this registration does not matter",
                   cls, data->uID, (unsigned)data->uCallbackMessage);
        }
    } catch (...) {
    }
}
static bool IsBatteryServiceWindow(HWND hwnd) {
    for (int i = 0; i < g_trayOwnerInfoCount; i++)
        if (g_trayOwnerInfo[i].wnd == hwnd && g_trayOwnerInfo[i].isBattery)
            return true;
    return false;
}

// Menu dell'icona di rete: deve essere mostrato sul thread della barra (lo stesso
// delle finestre di servizio pnidui/stobject), con Shell_TrayWnd come proprietario,
// come il menu audio/batteria. Il messaggio privato porta la richiesta su quel thread.
namespace NetworkTrayForce { static void ShowNetworkIconMenuHere(HWND owner) noexcept; }
// 1.3.3: the click on the battery icon, in the window procedure below, asks for the Windows 10
// battery flyout. That function is defined further down, together with the rest of the battery
// code; a member of a namespace is usable only after it has been declared, and the window
// procedure comes first, so the declaration stands here, next to ShowNetworkIconMenuHere.
namespace NetworkTrayForce { namespace BatteryFlyout { bool RequestBatteryFlyout() noexcept; } }
static UINT NetworkMenuMessage() {
    static UINT message = RegisterWindowMessageW(L"Win10ExplorerRestorer.ShowNetworkMenu");
    return message;
}


static LRESULT CALLBACK NetworkIconSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                UINT_PTR sid, DWORD_PTR ref) {
    (void)sid;
    (void)ref;
    (void)wParam;
    try {

        if (msg == NetworkMenuMessage()) {
            NetworkTrayForce::ShowNetworkIconMenuHere(hwnd);   // gira sul thread della barra
            return 0;
        }
        // Right-click on the battery service window: intercept and show
        // our multilang menu (same style as the network menu); do NOT let
        // stobject build its own (which ends up empty because strings
        // 150/151 are empty in the Win10 MUI).
        const bool battery = IsBatteryServiceWindow(hwnd);
        // 1.3.3: left click on the battery icon. The handler of stobject.dll for this click is
        // never reached: the click is consumed here and the Windows 10 battery flyout is asked
        // for with the authentic call of this shell (shell experience manager, experience
        // Windows.Internal.ShellExperience.TrayBatteryFlyout). No page, no Win32 flyout, no
        // registry value.
        if (battery) {
            const UINT event = LOWORD(lParam);   // NIN_* or WM_*
            const bool leftClick = (msg == WM_LBUTTONUP || event == WM_LBUTTONUP ||
                                    event == NIN_SELECT || event == NIN_KEYSELECT ||
                                    msg == NIN_SELECT || msg == NIN_KEYSELECT);
            if (leftClick) {
                if (g_netClickLogs < 10) {
                    g_netClickLogs++;
                    wchar_t cls[64] = {};
                    GetClassNameW(hwnd, cls, _countof(cls));
                    Wh_Log(L"[battery] left click on the battery service window (class %s): the "
                           L"Windows 10 battery flyout is requested here, and the handler of "
                           L"stobject never sees the click", cls);
                }
                NetworkTrayForce::BatteryFlyout::RequestBatteryFlyout();
                return 0;   // consumed: no Win32 flyout, no page, no registry value
            }
        }
        if (battery && (msg == WM_CONTEXTMENU || msg == WM_RBUTTONUP)) {
            if (g_netClickLogs < 8) {
                g_netClickLogs++;
                wchar_t cls[64] = {};
                GetClassNameW(hwnd, cls, _countof(cls));
                Wh_Log(L"[battery] right click on battery service window (class %s): multilang menu served", cls);
            }
            ShowBatteryMenu(hwnd);
            return 0;
        }
        if (lParam == WM_RBUTTONUP || msg == WM_CONTEXTMENU) {
            if (g_netClickLogs < 6) {
                g_netClickLogs++;
                wchar_t cls[64] = {};
                GetClassNameW(hwnd, cls, _countof(cls));
                Wh_Log(L"[tray] right click reached a service window (class %s, message 0x%X): the Windows component shows the menu",
                       cls, (unsigned)msg);
            }
        }
    } catch (...) {
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// pnidui's service window is a hidden top-level window
// (seen in the disassembly: CreateWindowExW with style 0 and parent 0).
// Who owns the window? Every tray icon delivers its clicks to the window of
// the module that registered it: pnidui for the network ("PNIHiddenWnd"), stobject
// for battery and volume (its service windows). The module is recognised
// through GWLP_HINSTANCE, so no path comparison is needed (the path spoof
// would falsify it).
static const wchar_t* TrayOwnerModuleOfWindow(HWND w) {
    HINSTANCE inst = (HINSTANCE)GetWindowLongPtrW(w, GWLP_HINSTANCE);
    if (!inst) return nullptr;
    if (inst == GetModuleHandleW(L"stobject.dll")) return L"stobject.dll";
    if (inst == GetModuleHandleW(L"pnidui.dll")) return L"pnidui.dll";
    return nullptr;
}

static HWND g_trayWnds[2] = {};
static int g_trayWndCount = 0;

static BOOL CALLBACK FindPniWindowProc(HWND w, LPARAM param) {
    wchar_t cls[64] = {};
    if (!GetClassNameW(w, cls, _countof(cls))) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;

    const wchar_t* owner = TrayOwnerModuleOfWindow(w);
    bool isPni = _wcsicmp(cls, L"PNIHiddenWnd") == 0;
    bool isStobject = owner && _wcsicmp(owner, L"stobject.dll") == 0;
    if (!isPni && !isStobject) return TRUE;

    // With a non-null context this is a read-only lookup for an existing PNI
    // service window. Do not subclass windows on that path.
    if (param) {
        if (isPni) {
            *reinterpret_cast<HWND*>(param) = w;
            return FALSE;
        }
        return TRUE;
    }

    for (int i = 0; i < g_trayWndCount; i++)
        if (g_trayWnds[i] == w) return TRUE;
    if (g_trayWndCount >= (int)_countof(g_trayWnds)) return FALSE;

    g_trayWnds[g_trayWndCount++] = w;
    SetWindowSubclass(w, NetworkIconSubclassProc, (UINT_PTR)(2 + g_trayWndCount), 0);
    Wh_Log(L"[tray] right-click supervision: service window class %s (%s)",
           cls, owner ? owner : L"unknown module");
    if (g_trayWndCount >= (int)_countof(g_trayWnds)) return FALSE;
    return TRUE;
}

static HWND FindPniHiddenWindow() {
    HWND found = nullptr;
    EnumWindows(FindPniWindowProc, (LPARAM)&found);   // comprende le finestre invisibili
    if (found) return found;

    // fallback: should it ever be created as a message-only window
    for (HWND w = FindWindowExW(HWND_MESSAGE, nullptr, nullptr, nullptr); w;
         w = FindWindowExW(HWND_MESSAGE, w, nullptr, nullptr)) {
        wchar_t cls[128] = {};
        if (!GetClassNameW(w, cls, _countof(cls))) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (pid != GetCurrentProcessId()) continue;
        if (_wcsicmp(cls, L"PNIHiddenWnd") == 0) return w;
    }
    return nullptr;
}

static void InstallNetworkIconMenu() {
    if (g_trayWndCount >= (int)_countof(g_trayWnds)) return;

    // pnidui: the network service window (hidden top-level).
    HWND pni = FindPniHiddenWindow();
    if (pni && !g_netIconWnd) {
        g_netIconWnd = pni;
        if (!g_netMenuLogged) {
            g_netMenuLogged = true;
            Wh_Log(L"[network] pnidui service window found (class PNIHiddenWnd among normal windows)");
        }
    }
    // All service windows (pnidui + stobject): the right-click log
    // says which one receives the click. No message is absorbed.
    EnumWindows(FindPniWindowProc, 0);
}

// Dynamic fallback is deliberately attached to the real pnidui callback window.
// It is never created unless pnidui.dll has attempted NIM_ADD and supplied a
// live PNIHiddenWnd + callback message. The click is sent to that same PNI
// HWND/ID/callback; this preserves PNI's handler rather than installing a mod
// WndProc or URI target. Whether that handler can display the Windows 10 flyout
// on the target build is a runtime question; the candidate fails closed if PNI
// tries to fall back to a network URI.
static const GUID kSystemNetworkIconGuid = {
    0x7820AE74, 0x23E3, 0x4229, { 0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C }
};

enum : WORD {
    kNetworkIconDisconnected = 3020,
    kNetworkIconWifiOnline0 = 3021,
    kNetworkIconWifiLimited0 = 3027,
    kNetworkIconWiredOnline = 3048,
    kNetworkIconWiredLimited = 3051,
};

struct NetworkPniRegistration {
    HWND hwnd;
    UINT id;
    UINT callbackMessage;
    UINT version;
    UINT originalFlags;
    GUID originalGuid;
    wchar_t tip[128];
    bool hasVersion;
    bool hasTip;
    bool hasGuid;
    bool valid;
    bool nativeFallbackRequired;
    bool forced;                 // 1.0.0: registrazione creata dalla mod, non da pnidui
    LONG generation;
};

static bool g_traySupportInstalled = false;
static SRWLOCK g_networkPniLock = SRWLOCK_INIT;
static NetworkPniRegistration g_networkPniRegistration = {};
static NetworkPniRegistration g_networkFallbackOwner = {};
static std::atomic<bool> g_networkFallbackStopping{false};
static bool g_networkFallbackAdded = false;              // ExplorerServicesThread only
static HICON g_networkFallbackIcon = nullptr;             // ExplorerServicesThread only
static WORD g_networkFallbackResource = 0;
static LONG g_networkFallbackGeneration = 0;
static ULONGLONG g_networkFallbackNextTick = 0;
static unsigned g_networkFallbackAddFailures = 0;
static LONG g_networkFallbackLogBudget = 16;
static thread_local bool g_insideNetworkIconNotifyCall = false;

class ScopedNetworkPniReadLock {
public:
    ScopedNetworkPniReadLock() { AcquireSRWLockShared(&g_networkPniLock); }
    ~ScopedNetworkPniReadLock() { ReleaseSRWLockShared(&g_networkPniLock); }
    ScopedNetworkPniReadLock(const ScopedNetworkPniReadLock&) = delete;
    ScopedNetworkPniReadLock& operator=(const ScopedNetworkPniReadLock&) = delete;
};

class ScopedNetworkPniWriteLock {
public:
    ScopedNetworkPniWriteLock() { AcquireSRWLockExclusive(&g_networkPniLock); }
    ~ScopedNetworkPniWriteLock() { ReleaseSRWLockExclusive(&g_networkPniLock); }
    ScopedNetworkPniWriteLock(const ScopedNetworkPniWriteLock&) = delete;
    ScopedNetworkPniWriteLock& operator=(const ScopedNetworkPniWriteLock&) = delete;
};

class ScopedNetworkNotifyBypass {
public:
    ScopedNetworkNotifyBypass() : m_old(g_insideNetworkIconNotifyCall) {
        g_insideNetworkIconNotifyCall = true;
    }
    ~ScopedNetworkNotifyBypass() { g_insideNetworkIconNotifyCall = m_old; }
    ScopedNetworkNotifyBypass(const ScopedNetworkNotifyBypass&) = delete;
    ScopedNetworkNotifyBypass& operator=(const ScopedNetworkNotifyBypass&) = delete;
private:
    bool m_old;
};

class ScopedNetworkIcon {
public:
    explicit ScopedNetworkIcon(HICON icon = nullptr) : m_icon(icon) {}
    ~ScopedNetworkIcon() { reset(); }
    ScopedNetworkIcon(const ScopedNetworkIcon&) = delete;
    ScopedNetworkIcon& operator=(const ScopedNetworkIcon&) = delete;
    HICON get() const { return m_icon; }
    HICON release() noexcept { HICON icon = m_icon; m_icon = nullptr; return icon; }
    void reset(HICON icon = nullptr) noexcept {
        if (m_icon) DestroyIcon(m_icon);
        m_icon = icon;
    }
private:
    HICON m_icon;
};

class ScopedNetworkModule {
public:
    explicit ScopedNetworkModule(HMODULE module = nullptr) : m_module(module) {}
    ~ScopedNetworkModule() { if (m_module) FreeLibrary(m_module); }
    ScopedNetworkModule(const ScopedNetworkModule&) = delete;
    ScopedNetworkModule& operator=(const ScopedNetworkModule&) = delete;
    HMODULE get() const { return m_module; }
private:
    HMODULE m_module;
};

class ScopedNetworkComApartment {
public:
    ScopedNetworkComApartment() : m_hr(CoInitializeEx(nullptr, COINIT_MULTITHREADED)),
                                  m_uninitialize(SUCCEEDED(m_hr)) {}
    ~ScopedNetworkComApartment() { if (m_uninitialize) CoUninitialize(); }
    ScopedNetworkComApartment(const ScopedNetworkComApartment&) = delete;
    ScopedNetworkComApartment& operator=(const ScopedNetworkComApartment&) = delete;
    bool usable() const { return SUCCEEDED(m_hr) || m_hr == RPC_E_CHANGED_MODE; }
private:
    HRESULT m_hr;
    bool m_uninitialize;
};

class ScopedNetworkWlanClient {
public:
    explicit ScopedNetworkWlanClient(HANDLE handle = nullptr) : m_handle(handle) {}
    ~ScopedNetworkWlanClient() { if (m_handle) WlanCloseHandle(m_handle, nullptr); }
    ScopedNetworkWlanClient(const ScopedNetworkWlanClient&) = delete;
    ScopedNetworkWlanClient& operator=(const ScopedNetworkWlanClient&) = delete;
    HANDLE get() const { return m_handle; }
private:
    HANDLE m_handle;
};

class ScopedNetworkWlanMemory {
public:
    explicit ScopedNetworkWlanMemory(PVOID memory = nullptr) : m_memory(memory) {}
    ~ScopedNetworkWlanMemory() { if (m_memory) WlanFreeMemory(m_memory); }
    ScopedNetworkWlanMemory(const ScopedNetworkWlanMemory&) = delete;
    ScopedNetworkWlanMemory& operator=(const ScopedNetworkWlanMemory&) = delete;
    PVOID get() const { return m_memory; }
private:
    PVOID m_memory;
};

static bool IsPniduiServiceWindow(HWND hwnd) noexcept {
    if (!hwnd || !IsWindow(hwnd)) return false;
    try {
        wchar_t cls[64] = {};
        if (!GetClassNameW(hwnd, cls, _countof(cls)) || _wcsicmp(cls, L"PNIHiddenWnd") != 0)
            return false;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != GetCurrentProcessId()) return false;
        const HINSTANCE classModule = (HINSTANCE)GetClassLongPtrW(hwnd, GCLP_HMODULE);
        const HINSTANCE windowInstance = (HINSTANCE)GetWindowLongPtrW(hwnd, GWLP_HINSTANCE);
        const HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
        return pnidui && (classModule == pnidui || windowInstance == pnidui);
    } catch (...) {
        return false;
    }
}

static bool CopyNetworkPniRegistration(NetworkPniRegistration* out) noexcept {
    if (!out) return false;
    try {
        ScopedNetworkPniReadLock lock;
        *out = g_networkPniRegistration;
        return out->valid;
    } catch (...) {
        return false;
    }
}

static bool CopyNotifyIconTipBounded(const NOTIFYICONDATAW* data, wchar_t* out,
                              size_t outCount) noexcept {
    if (!data || !out || outCount == 0 || !(data->uFlags & NIF_TIP)) return false;
    out[0] = L'\0';
    const size_t tipOffset = FIELD_OFFSET(NOTIFYICONDATAW, szTip);
    if (data->cbSize <= tipOffset) return false;
    const size_t suppliedBytes = (size_t)data->cbSize - tipOffset;
    const size_t boundedBytes = (std::min)(suppliedBytes, sizeof(data->szTip));
    const size_t suppliedChars = boundedBytes / sizeof(wchar_t);
    if (!suppliedChars) return false;
    const size_t copyCount = (std::min)(suppliedChars - 1, outCount - 1);
    if (!copyCount) return true;
    return wcsncpy_s(out, outCount, data->szTip, copyCount) == 0;
}

static void RecordNetworkPniRegistration(DWORD message, PNOTIFYICONDATAW data,
                                         BOOL nativeResult) noexcept {
    try {
        if (!data) return;
        if (message == NIM_ADD && IsPniduiServiceWindow(data->hWnd)) {
            NetworkPniRegistration next = {};
            next.hwnd = data->hWnd;
            next.id = data->uID;
            next.callbackMessage = data->uCallbackMessage;
            next.originalFlags = data->uFlags;
            if (data->cbSize >= NOTIFYICONDATA_V3_SIZE && (data->uFlags & NIF_GUID)) {
                next.hasGuid = true;
                next.originalGuid = data->guidItem;
            }
            next.hasTip = CopyNotifyIconTipBounded(data, next.tip, _countof(next.tip));
            next.forced = false;   // 1.0.0: registrazione nativa
            const bool nativeIconPresent = nativeResult != FALSE &&
                (data->uFlags & NIF_ICON) != 0 && data->hIcon != nullptr;
            next.nativeFallbackRequired = !nativeIconPresent;
            next.valid = next.callbackMessage != 0 &&
                         (!next.hasGuid || IsEqualGUID(next.originalGuid, kSystemNetworkIconGuid));
            {
                ScopedNetworkPniWriteLock lock;
                next.generation = g_networkPniRegistration.generation + 1;
                g_networkPniRegistration = next;
            }
            if (next.hasGuid && !IsEqualGUID(next.originalGuid, kSystemNetworkIconGuid)) {
                wchar_t guid[64] = {};
                StringFromGUID2(next.originalGuid, guid, _countof(guid));
                Wh_Log(L"[tray] PNI NIM_ADD used unexpected GUID %s; fail-closed: no dynamic replacement", guid);
            } else if (!next.callbackMessage) {
                Wh_Log(L"[tray] PNI NIM_ADD did not provide a callback message; fail-closed: no dynamic replacement");
            } else {
                Wh_Log(L"[tray] native PNI NIM_ADD %s (hWnd 0x%p, id %u, callback 0x%X, system GUID %s)",
                       nativeResult ? L"accepted" : L"failed",
                       next.hwnd, next.id, next.callbackMessage,
                       next.nativeFallbackRequired ? L"dynamic NLM fallback pending" : L"native icon active");
            }
            return;
        }

        if (message == NIM_SETVERSION) {
            ScopedNetworkPniWriteLock lock;
            if (g_networkPniRegistration.valid && data->hWnd == g_networkPniRegistration.hwnd &&
                data->uID == g_networkPniRegistration.id) {
                g_networkPniRegistration.hasVersion = true;
                g_networkPniRegistration.version = data->uVersion;
                ++g_networkPniRegistration.generation;
            }
            return;
        }

        if (message == NIM_MODIFY) {
            ScopedNetworkPniWriteLock lock;
            if (g_networkPniRegistration.valid && data->hWnd == g_networkPniRegistration.hwnd &&
                data->uID == g_networkPniRegistration.id) {
                bool changed = false;
                if (data->uFlags & NIF_MESSAGE) {
                    g_networkPniRegistration.callbackMessage = data->uCallbackMessage;
                    if (!data->uCallbackMessage) g_networkPniRegistration.valid = false;
                    changed = true;
                }
                if (data->uFlags & NIF_ICON) {
                    g_networkPniRegistration.nativeFallbackRequired =
                        nativeResult == FALSE || data->hIcon == nullptr;
                    changed = true;
                }
                if (CopyNotifyIconTipBounded(data, g_networkPniRegistration.tip,
                                      _countof(g_networkPniRegistration.tip))) {
                    g_networkPniRegistration.hasTip = true;
                    changed = true;
                }
                if (changed) ++g_networkPniRegistration.generation;
            }
            return;
        }

        if (message == NIM_DELETE) {
            ScopedNetworkPniWriteLock lock;
            if (g_networkPniRegistration.valid && data->hWnd == g_networkPniRegistration.hwnd &&
                (data->uID == g_networkPniRegistration.id ||
                 ((data->uFlags & NIF_GUID) && data->cbSize >= NOTIFYICONDATA_V3_SIZE &&
                  IsEqualGUID(data->guidItem, kSystemNetworkIconGuid)))) {
                g_networkPniRegistration.valid = false;
                g_networkPniRegistration.nativeFallbackRequired = false;
                ++g_networkPniRegistration.generation;
                Wh_Log(L"[tray] native PNI NIM_DELETE observed; dynamic fallback will be withdrawn");
            }
        }
    } catch (...) {
        Wh_Log(L"[tray] exception recording the native PNI registration");
    }
}

static BOOL CallNetworkIconNotify(DWORD message, PNOTIFYICONDATAW data) noexcept {
    try {
        ScopedNetworkNotifyBypass bypass;
        return Shell_NotifyIconW(message, data);
    } catch (...) {
        Wh_Log(L"[tray] Shell_NotifyIconW raised a C++ exception in the dynamic fallback");
        return FALSE;
    }
}

struct NetworkAdapterInfo {
    GUID id;
    IFTYPE type;
    IF_INDEX index;
};

typedef ULONG (WINAPI* NetworkGetAdaptersAddresses_t)(ULONG, ULONG, PVOID,
                                                       PIP_ADAPTER_ADDRESSES, PULONG);
typedef DWORD (WINAPI* NetworkGetBestInterface_t)(ULONG, PDWORD);

static int ReadNetworkAdapters(NetworkAdapterInfo* out, int maxCount,
                               DWORD* bestIf) noexcept {
    if (!out || maxCount <= 0 || !bestIf) return 0;
    *bestIf = 0;
    try {
        ScopedNetworkModule iphlpapi(LoadLibraryExW(L"iphlpapi.dll", nullptr,
                                                    LOAD_LIBRARY_SEARCH_SYSTEM32));
        if (!iphlpapi.get()) return 0;
        auto getAdapters = (NetworkGetAdaptersAddresses_t)GetProcAddress(
            iphlpapi.get(), "GetAdaptersAddresses");
        auto getBestInterface = (NetworkGetBestInterface_t)GetProcAddress(
            iphlpapi.get(), "GetBestInterface");
        if (getBestInterface) {
            DWORD index = 0;
            if (getBestInterface(0x01010101, &index) == NO_ERROR) *bestIf = index;
        }
        if (!getAdapters) return 0;

        ULONG bytes = 16 * 1024;
        for (int attempt = 0; attempt < 3; ++attempt) {
            std::vector<BYTE> buffer(bytes);
            ULONG supplied = bytes;
            const ULONG rc = getAdapters(AF_UNSPEC,
                GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                nullptr, (PIP_ADAPTER_ADDRESSES)buffer.data(), &supplied);
            if (rc == ERROR_BUFFER_OVERFLOW) {
                bytes = supplied;
                continue;
            }
            if (rc != NO_ERROR) return 0;

            int count = 0;
            for (PIP_ADAPTER_ADDRESSES adapter = (PIP_ADAPTER_ADDRESSES)buffer.data();
                 adapter && count < maxCount; adapter = adapter->Next) {
                if (!adapter->AdapterName) continue;
                wchar_t text[64] = {};
                if (!MultiByteToWideChar(CP_ACP, 0, adapter->AdapterName, -1,
                                         text, _countof(text))) continue;
                GUID id = {};
                if (FAILED(CLSIDFromString(text, &id))) continue;
                out[count].id = id;
                out[count].type = adapter->IfType;
                out[count].index = adapter->IfIndex ? adapter->IfIndex : adapter->Ipv6IfIndex;
                ++count;
            }
            return count;
        }
    } catch (...) {
        Wh_Log(L"[tray] adapter enumeration exception in the NLM fallback");
    }
    return 0;
}

static int QueryNetworkWifiBars(const GUID& adapterGuid) noexcept {
    try {
        HANDLE rawClient = nullptr;
        DWORD negotiated = 0;
        const DWORD openResult = WlanOpenHandle(2, nullptr, &negotiated, &rawClient);
        ScopedNetworkWlanClient client(rawClient);
        if (openResult != ERROR_SUCCESS || !client.get()) return -1;
        PWLAN_INTERFACE_INFO_LIST rawList = nullptr;
        const DWORD enumResult = WlanEnumInterfaces(client.get(), nullptr, &rawList);
        ScopedNetworkWlanMemory listOwner(rawList);
        if (enumResult != ERROR_SUCCESS || !rawList) return -1;
        const WLAN_INTERFACE_INFO_LIST* list = rawList;
        for (DWORD i = 0; i < list->dwNumberOfItems; ++i) {
            if (!IsEqualGUID(list->InterfaceInfo[i].InterfaceGuid, adapterGuid)) continue;
            PVOID rawData = nullptr;
            DWORD bytes = 0;
            const DWORD rssiResult = WlanQueryInterface(client.get(), &adapterGuid,
                wlan_intf_opcode_rssi, nullptr, &bytes, &rawData, nullptr);
            ScopedNetworkWlanMemory rssiOwner(rawData);
            if (rssiResult == ERROR_SUCCESS && rawData && bytes >= sizeof(LONG)) {
                const LONG rssi = *(const LONG*)rawData;
                return rssi >= -55 ? 4 : rssi >= -67 ? 3 : rssi >= -75 ? 2 : rssi >= -85 ? 1 : 0;
            }

            PVOID rawConnection = nullptr;
            bytes = 0;
            const DWORD connectionResult = WlanQueryInterface(client.get(), &adapterGuid,
                wlan_intf_opcode_current_connection, nullptr, &bytes, &rawConnection, nullptr);
            ScopedNetworkWlanMemory connectionOwner(rawConnection);
            if (connectionResult == ERROR_SUCCESS && rawConnection &&
                bytes >= sizeof(WLAN_CONNECTION_ATTRIBUTES)) {
                const WLAN_CONNECTION_ATTRIBUTES* connection =
                    (const WLAN_CONNECTION_ATTRIBUTES*)rawConnection;
                const int quality = (int)connection->wlanAssociationAttributes.wlanSignalQuality;
                return quality >= 80 ? 4 : quality >= 60 ? 3 : quality >= 40 ? 2 : quality >= 20 ? 1 : 0;
            }
            return -1;
        }
    } catch (...) {
        Wh_Log(L"[tray] WLAN signal query exception in the NLM fallback");
    }
    return -1;
}

static const CLSID kClsidNetworkListManager = {
    0xDCB00C01, 0x570F, 0x4A9B, { 0x8D, 0x69, 0x19, 0x9F, 0xDB, 0xA5, 0x72, 0x3B }
};
static const IID kIidNetworkListManager = {
    0xDCB00000, 0x570F, 0x4A9B, { 0x8D, 0x69, 0x19, 0x9F, 0xDB, 0xA5, 0x72, 0x3B }
};

static bool QueryNetworkIconResource(WORD* resourceId) noexcept {
    if (!resourceId) return false;
    *resourceId = kNetworkIconDisconnected;
    try {
        ScopedNetworkComApartment apartment;
        if (!apartment.usable()) return false;

        NetworkAdapterInfo adapters[32] = {};
        DWORD bestIf = 0;
        const int adapterCount = ReadNetworkAdapters(adapters, _countof(adapters), &bestIf);

        winrt::com_ptr<INetworkListManager> manager;
        HRESULT hr = CoCreateInstance(kClsidNetworkListManager, nullptr, CLSCTX_ALL,
                                      kIidNetworkListManager, manager.put_void());
        if (FAILED(hr) || !manager) return false;
        winrt::com_ptr<IEnumNetworkConnections> connections;
        hr = manager->GetNetworkConnections(connections.put());
        if (FAILED(hr) || !connections) return false;

        bool found = false;
        bool chosenWifi = false;
        bool chosenInternet = false;
        GUID chosenAdapter = {};
        int bestScore = -1;
        const DWORD internetMask = (DWORD)NLM_CONNECTIVITY_IPV4_INTERNET |
                                   (DWORD)NLM_CONNECTIVITY_IPV6_INTERNET;
        for (;;) {
            winrt::com_ptr<INetworkConnection> connection;
            ULONG fetched = 0;
            const HRESULT next = connections->Next(1, connection.put(), &fetched);
            if (next != S_OK || fetched != 1 || !connection) break;

            VARIANT_BOOL isConnected = VARIANT_FALSE;
            if (FAILED(connection->get_IsConnected(&isConnected)) || isConnected != VARIANT_TRUE)
                continue;
            GUID adapterGuid = {};
            if (FAILED(connection->GetAdapterId(&adapterGuid))) continue;
            NLM_CONNECTIVITY connectivity = NLM_CONNECTIVITY_DISCONNECTED;
            connection->GetConnectivity(&connectivity);

            IFTYPE adapterType = 0;
            IF_INDEX adapterIndex = 0;
            for (int i = 0; i < adapterCount; ++i) {
                if (IsEqualGUID(adapters[i].id, adapterGuid)) {
                    adapterType = adapters[i].type;
                    adapterIndex = adapters[i].index;
                    break;
                }
            }
            if (adapterType == IF_TYPE_SOFTWARE_LOOPBACK || adapterType == IF_TYPE_TUNNEL)
                continue;

            const bool internet = (((DWORD)connectivity & internetMask) != 0);
            const int score = (adapterIndex && adapterIndex == bestIf ? 4 : 0) +
                              (internet ? 2 : 0) + 1;
            if (score <= bestScore) continue;
            bestScore = score;
            found = true;
            chosenWifi = adapterType == IF_TYPE_IEEE80211;
            chosenInternet = internet;
            chosenAdapter = adapterGuid;
        }

        if (!found) {
            *resourceId = kNetworkIconDisconnected;
            return true;
        }
        if (!chosenWifi) {
            *resourceId = chosenInternet ? kNetworkIconWiredOnline : kNetworkIconWiredLimited;
            return true;
        }
        int bars = QueryNetworkWifiBars(chosenAdapter);
        if (bars < 0) bars = 4;  // NLM state is authoritative; signal can be unavailable by policy.
        if (bars > 4) bars = 4;
        *resourceId = (WORD)((chosenInternet ? kNetworkIconWifiOnline0 :
                              kNetworkIconWifiLimited0) + bars);
        return true;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"[tray] NLM network-state query failed (0x%08X)", (unsigned)ex.code());
    } catch (...) {
        Wh_Log(L"[tray] NLM network-state query raised a C++ exception");
    }
    return false;
}

static void BuildNetworkFallbackNid(const NetworkPniRegistration& reg, HICON icon,
                                    NOTIFYICONDATAW* nid) noexcept {
    if (!nid) return;
    memset(nid, 0, sizeof(*nid));
    nid->cbSize = sizeof(*nid);
    nid->hWnd = reg.hwnd;
    nid->uID = reg.id;
    nid->uFlags = NIF_ICON | NIF_MESSAGE | NIF_GUID;
    nid->guidItem = kSystemNetworkIconGuid;
    nid->uCallbackMessage = reg.callbackMessage;
    nid->hIcon = icon;
    if (reg.hasTip) {
        nid->uFlags |= NIF_TIP;
        wcsncpy_s(nid->szTip, _countof(nid->szTip), reg.tip, _TRUNCATE);
    }
}

static bool DeleteNetworkFallbackIcon(const NetworkPniRegistration& reg) noexcept {
    try {
        NOTIFYICONDATAW nid = {};
        nid.cbSize = sizeof(nid);
        nid.hWnd = reg.hwnd;
        nid.uID = reg.id;
        nid.uFlags = NIF_GUID;
        nid.guidItem = kSystemNetworkIconGuid;
        return CallNetworkIconNotify(NIM_DELETE, &nid) != FALSE;
    } catch (...) {
        Wh_Log(L"[tray] exception deleting the dynamic PNI fallback icon");
        return false;
    }
}

static void ReleaseNetworkFallbackIconState() noexcept {
    g_networkFallbackAdded = false;
    g_networkFallbackResource = 0;
    g_networkFallbackGeneration = 0;
    g_networkFallbackNextTick = 0;
    g_networkFallbackOwner = {};
    if (g_networkFallbackIcon) {
        DestroyIcon(g_networkFallbackIcon);
        g_networkFallbackIcon = nullptr;
    }
}

// 1.0.0 (dichiarazione anticipata: la definizione completa sta piu' sotto, dopo
// IsOurTaskbarUp). Serve al percorso esistente per riconoscere la finestra
// proprietaria dell'icona forzata.
namespace NetworkTrayForce {
static bool IsForcedOwnerWindow(HWND hwnd) noexcept;
}

static void NetworkIconFallbackTick() noexcept {
    try {
        if (!g_traySupportInstalled || g_unloading.load() ||
            g_networkFallbackStopping.load()) return;

        NetworkPniRegistration reg = {};
        const bool haveRegistration = CopyNetworkPniRegistration(&reg);
        if (!g_cfg.provideTrayDlls) {
            if (!g_networkFallbackAdded) return;
            const bool preserveNativePniItem = haveRegistration && reg.valid &&
                !reg.nativeFallbackRequired && reg.hasGuid &&
                IsEqualGUID(reg.originalGuid, kSystemNetworkIconGuid) &&
                reg.hwnd == g_networkFallbackOwner.hwnd && reg.id == g_networkFallbackOwner.id;
            if (preserveNativePniItem) {
                ReleaseNetworkFallbackIconState();
                Wh_Log(L"[tray] dynamic fallback disabled; preserving the native PNI icon");
                return;
            }
            const ULONGLONG now = GetTickCount64();
            if (now < g_networkFallbackNextTick) return;
            g_networkFallbackNextTick = now + 5000;
            if (!DeleteNetworkFallbackIcon(g_networkFallbackOwner)) {
                Wh_Log(L"[tray] dynamic PNI fallback NIM_DELETE failed after disabling provideTrayDlls; retry in 5 s");
                return;
            }
            ReleaseNetworkFallbackIconState();
            Wh_Log(L"[tray] dynamic fallback removed after disabling provideTrayDlls");
            return;
        }
        if (!haveRegistration || !reg.valid) {
            if (g_networkFallbackAdded) {
                const ULONGLONG now = GetTickCount64();
                if (now < g_networkFallbackNextTick) return;
                g_networkFallbackNextTick = now + 5000;
                if (!DeleteNetworkFallbackIcon(g_networkFallbackOwner)) {
                    Wh_Log(L"[tray] dynamic PNI fallback NIM_DELETE failed; retry in 5 s");
                    return;
                }
                g_networkFallbackAdded = false;
                g_networkFallbackResource = 0;
                g_networkFallbackGeneration = 0;
                if (g_networkFallbackIcon) {
                    DestroyIcon(g_networkFallbackIcon);
                    g_networkFallbackIcon = nullptr;
                }
                Wh_Log(L"[tray] dynamic PNI fallback removed because its native callback registration ended");
            }
            return;
        }
        if (!reg.nativeFallbackRequired) {
            // A later native NIM_ADD may have replaced our fallback. Preserve that
            // native item only when the owner identity is the same; otherwise retire
            // our old registration so it cannot remain orphaned in the tray.
            if (g_networkFallbackAdded) {
                const bool sameIdentity = g_networkFallbackOwner.hwnd == reg.hwnd &&
                    g_networkFallbackOwner.id == reg.id && reg.hasGuid &&
                    IsEqualGUID(reg.originalGuid, kSystemNetworkIconGuid);
                if (!sameIdentity) {
                    const ULONGLONG now = GetTickCount64();
                    if (now < g_networkFallbackNextTick) return;
                    g_networkFallbackNextTick = now + 5000;
                    if (!DeleteNetworkFallbackIcon(g_networkFallbackOwner)) {
                        g_networkFallbackGeneration = reg.generation;
                        Wh_Log(L"[tray] old dynamic PNI fallback NIM_DELETE failed; retry in 5 s");
                        return;
                    }
                }
                g_networkFallbackAdded = false;
                g_networkFallbackResource = 0;
                g_networkFallbackGeneration = 0;
                if (g_networkFallbackIcon) {
                    DestroyIcon(g_networkFallbackIcon);
                    g_networkFallbackIcon = nullptr;
                }
                Wh_Log(L"[tray] native PNI icon is active; dynamic fallback retired");
            }
            return;
        }
        if ((!IsPniduiServiceWindow(reg.hwnd) && !NetworkTrayForce::IsForcedOwnerWindow(reg.hwnd)) ||
            reg.callbackMessage == 0) {
            g_networkFallbackGeneration = reg.generation;
            return;
        }

        const ULONGLONG now = GetTickCount64();
        if (now < g_networkFallbackNextTick &&
            g_networkFallbackGeneration == reg.generation) return;
        g_networkFallbackNextTick = now + 5000;

        WORD resourceId = 0;
        if (!QueryNetworkIconResource(&resourceId)) {
            if (!reg.forced) {
                g_networkFallbackGeneration = reg.generation;
                if (InterlockedDecrement(&g_networkFallbackLogBudget) >= 0)
                    Wh_Log(L"[tray] dynamic fallback waiting for Network List Manager; no URI or substitute click target is installed");
                return;
            }
            // Icona forzata: Network List Manager non risponde, ma l'icona deve
            // comparire lo stesso. Si usa l'icona autentica "a cavo" della
            // pnidui.dll verificata; quando NLM torna disponibile il tick la
            // aggiorna con lo stato reale.
            resourceId = kNetworkIconWiredOnline;
            if (InterlockedDecrement(&g_networkFallbackLogBudget) >= 0)
                Wh_Log(L"[tray-force] Network List Manager unavailable: authentic icon "
                       L"3048 used so that the icon appears anyway");
        }
        const bool sameIdentity = g_networkFallbackAdded &&
            g_networkFallbackOwner.hwnd == reg.hwnd &&
            g_networkFallbackOwner.id == reg.id;
        if (g_networkFallbackAdded && !sameIdentity) {
            if (!DeleteNetworkFallbackIcon(g_networkFallbackOwner)) {
                g_networkFallbackGeneration = reg.generation;
                Wh_Log(L"[tray] old dynamic PNI fallback NIM_DELETE failed; retry in 5 s");
                return;
            }
            g_networkFallbackAdded = false;
            if (g_networkFallbackIcon) {
                DestroyIcon(g_networkFallbackIcon);
                g_networkFallbackIcon = nullptr;
            }
        }
        if (sameIdentity && resourceId == g_networkFallbackResource &&
            g_networkFallbackGeneration == reg.generation) return;

        HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
        const int cx = GetSystemMetrics(SM_CXSMICON) > 0 ? GetSystemMetrics(SM_CXSMICON) : 16;
        const int cy = GetSystemMetrics(SM_CYSMICON) > 0 ? GetSystemMetrics(SM_CYSMICON) : 16;
        ScopedNetworkIcon icon(pnidui ? (HICON)LoadImageW(pnidui, MAKEINTRESOURCEW(resourceId),
                                                          IMAGE_ICON, cx, cy, 0)
                                      : nullptr);
        if (!icon.get()) {
            // 1.0.0: se la pnidui.dll non e' mappata in questo processo l'icona
            // autentica viene presa direttamente dal file verificato nella
            // cartella dati. Serve a far comparire l'icona anche quando la SSO
            // nativa non e' mai partita.
            wchar_t modulePath[MAX_PATH] = {};
            _snwprintf_s(modulePath, _countof(modulePath), _TRUNCATE, L"%s\\pnidui.dll",
                         g_cfg.storePath);
            icon.reset((HICON)LoadImageW(nullptr, modulePath, IMAGE_ICON, cx, cy,
                                         LR_LOADFROMFILE));
            if (icon.get()) {
                static bool loggedFromFile = false;
                if (!loggedFromFile) {
                    loggedFromFile = true;
                    Wh_Log(L"[tray-force] authentic icon %u loaded from the file %s "
                           L"(pnidui.dll is not mapped in this process)", resourceId, modulePath);
                }
            }
        }
        if (!icon.get()) {
            g_networkFallbackGeneration = reg.generation;
            if (InterlockedDecrement(&g_networkFallbackLogBudget) >= 0)
                Wh_Log(L"[tray] authentic pnidui icon resource %u could not be loaded", resourceId);
            return;
        }

        NOTIFYICONDATAW nid = {};
        BuildNetworkFallbackNid(reg, icon.get(), &nid);
        BOOL ok = FALSE;
        if (g_networkFallbackAdded) {
            ok = CallNetworkIconNotify(NIM_MODIFY, &nid);
            if (!ok) ok = CallNetworkIconNotify(NIM_ADD, &nid);
        } else {
            ok = CallNetworkIconNotify(NIM_ADD, &nid);
        }
        if (!ok) {
            ++g_networkFallbackAddFailures;
            // Keep the last registered icon and HICON owned until a delete or
            // replacement succeeds; a zero resource forces a retry if it is active.
            if (g_networkFallbackAdded) g_networkFallbackResource = 0;
            g_networkFallbackGeneration = reg.generation;
            if (g_networkFallbackAddFailures <= 6 || g_networkFallbackAddFailures % 6 == 0)
                Wh_Log(L"[tray] dynamic PNI/NLM NIM_ADD/MODIFY failed (%u), identity hWnd 0x%p id %u; retry in 5 s",
                       g_networkFallbackAddFailures, reg.hwnd, reg.id);
            return;
        }

        if (reg.hasVersion) {
            NOTIFYICONDATAW versionData = {};
            versionData.cbSize = sizeof(versionData);
            versionData.hWnd = reg.hwnd;
            versionData.uID = reg.id;
            versionData.uFlags = NIF_GUID;
            versionData.guidItem = kSystemNetworkIconGuid;
            versionData.uVersion = reg.version;
            CallNetworkIconNotify(NIM_SETVERSION, &versionData);
        }

        HICON oldIcon = g_networkFallbackIcon;
        g_networkFallbackIcon = icon.release();
        g_networkFallbackOwner = reg;
        g_networkFallbackAdded = true;
        g_networkFallbackResource = resourceId;
        g_networkFallbackGeneration = reg.generation;
        g_networkFallbackAddFailures = 0;
        if (oldIcon) DestroyIcon(oldIcon);
        Wh_Log(L"[tray] dynamic NLM fallback active: resource %u, system network GUID, native PNI callback 0x%X",
               resourceId, reg.callbackMessage);
    } catch (...) {
        Wh_Log(L"[tray] dynamic network-icon fallback exception");
    }
}

static void NetworkIconFallbackShutdown() noexcept {
    try {
        g_networkFallbackStopping.store(true);
        const bool hadResources = g_networkFallbackAdded || g_networkFallbackIcon != nullptr;
        NetworkPniRegistration current = {};
        const bool preserveNativePniItem =
            CopyNetworkPniRegistration(&current) && current.valid && !current.nativeFallbackRequired &&
            current.hasGuid && IsEqualGUID(current.originalGuid, kSystemNetworkIconGuid) &&
            current.hwnd == g_networkFallbackOwner.hwnd && current.id == g_networkFallbackOwner.id;
        if (g_networkFallbackAdded) {
            if (!preserveNativePniItem) {
                bool deleted = false;
                for (int attempt = 0; attempt < 3 && !deleted; ++attempt)
                    deleted = DeleteNetworkFallbackIcon(g_networkFallbackOwner);
                if (!deleted)
                    Wh_Log(L"[tray] dynamic PNI fallback NIM_DELETE failed during shutdown after 3 attempts");
            }
            g_networkFallbackAdded = false;
        }
        if (g_networkFallbackIcon) {
            DestroyIcon(g_networkFallbackIcon);
            g_networkFallbackIcon = nullptr;
        }
        g_networkFallbackResource = 0;
        g_networkFallbackGeneration = 0;
        if (hadResources) Wh_Log(L"[tray] dynamic PNI fallback cleaned up");
    } catch (...) {
        Wh_Log(L"[tray] dynamic network-icon fallback cleanup exception");
    }
}

class ScopedNetworkFallbackThreadCleanup {
public:
    ~ScopedNetworkFallbackThreadCleanup() { NetworkIconFallbackShutdown(); }
    ScopedNetworkFallbackThreadCleanup(const ScopedNetworkFallbackThreadCleanup&) = delete;
    ScopedNetworkFallbackThreadCleanup& operator=(const ScopedNetworkFallbackThreadCleanup&) = delete;
    ScopedNetworkFallbackThreadCleanup() = default;
};

static BOOL WINAPI Shell_NotifyIconW_Hook(DWORD message, PNOTIFYICONDATAW data) {
    if (g_insideNetworkIconNotifyCall && Shell_NotifyIconW_Original)
        return Shell_NotifyIconW_Original(message, data);
    if (g_unloading.load() && Shell_NotifyIconW_Original) {
        const BOOL result = Shell_NotifyIconW_Original(message, data);
        RecordNetworkPniRegistration(message, data, result);
        return result;
    }
    try {
        if (data && data->hWnd) {
            wchar_t cls[64] = {};
            if (GetClassNameW(data->hWnd, cls, _countof(cls)) && _wcsicmp(cls, L"PNIHiddenWnd") == 0) {
                // It is pnidui's window: the right click goes through here too.
                g_netIconWnd = data->hWnd;
                if (data->uCallbackMessage) g_netIconMsg = data->uCallbackMessage;
                InstallNetworkIconMenu();
            }
        }
        // One line per icon: the Action Center icon re-registers every
        // 15 seconds and on its own filled the log (user report).
        bool newIcon = false;
        if (data && message == NIM_ADD) {
            newIcon = true;
            for (int i = 0; i < g_trayIconSeenCount; i++)
                if (g_trayIconSeen[i].wnd == data->hWnd && g_trayIconSeen[i].id == data->uID) {
                    newIcon = false;
                    break;
                }
            if (newIcon && g_trayIconSeenCount < (int)_countof(g_trayIconSeen)) {
                g_trayIconSeen[g_trayIconSeenCount].wnd = data->hWnd;
                g_trayIconSeen[g_trayIconSeenCount].id = data->uID;
                g_trayIconSeenCount++;
            }
        }
        if (data && message == NIM_ADD && newIcon && g_netIconLogs < 24) {
            g_netIconLogs++;
            wchar_t cls[64] = {};
            if (data->hWnd) GetClassNameW(data->hWnd, cls, _countof(cls));
            wchar_t guid[64] = {};
            if ((data->uFlags & NIF_GUID) && data->cbSize >= NOTIFYICONDATA_V3_SIZE)
                StringFromGUID2(data->guidItem, guid, _countof(guid));
            wchar_t boundedTip[128] = {};
            const wchar_t* tip = L"(no text)";
            if (data->uFlags & NIF_TIP)
                tip = CopyNotifyIconTipBounded(data, boundedTip, _countof(boundedTip))
                    ? boundedTip : L"(tip unavailable)";
            RememberTrayOwner(data->hWnd, data->uID, tip);
            Wh_Log(L"[network] tray icon: window %s, id %u, message 0x%X, guid %s, text \"%s\", battery %s",
                   cls, data->uID, (unsigned)data->uCallbackMessage, guid, tip,
                   IsBatteryServiceWindow(data->hWnd) ? L"yes" : L"no");
        }
        if (data && message == NIM_MODIFY && data->hWnd && (data->uFlags & NIF_TIP)) {
            wchar_t boundedTip[128] = {};
            if (CopyNotifyIconTipBounded(data, boundedTip, _countof(boundedTip)))
                RememberTrayOwner(data->hWnd, data->uID, boundedTip);
        }
        // 1.3.4: la batteria si registra con il testo vuoto, quindi il GUID e' l'unica cosa
        // che la nomina; e questo non dipende dal bilancio dei log qui sopra.
        if (data && (message == NIM_ADD || message == NIM_MODIFY)) RememberBatteryTrayIcon(data);
    } catch (...) {
        Wh_Log(L"[network] exception in the icon supervision");
    }
    const BOOL result = Shell_NotifyIconW_Original(message, data);
    RecordNetworkPniRegistration(message, data, result);
    return result;
}

static void InstallNetworkIconTrace() {
    if (Wh_SetFunctionHook((void*)Shell_NotifyIconW, (void*)Shell_NotifyIconW_Hook,
                           (void**)&Shell_NotifyIconW_Original))
        Wh_Log(L"[network] tray icon registration tracking active");
    else
        Wh_Log(L"[network] tray icon registration tracking not installed");
}


// --- 8f) tray module strings: the MUI that is not in the data folder --------
// Evidence (stobject.dll 10.0.19041.7664, disassembled): stobject does NOT have
// the strings inside itself (file resources: MUI, VERSION, MANIFEST) and takes
// EVERY text with LoadStringW(hInstance, id, ...) from its MUI,
// "stobject.dll.mui": that is where the entries of the battery right-click
// menu come from (ids 166, 167: "Power options", "Windows Mobility Center",
// ...) and the flyout texts (ids 206-214). In the data folder the .mui
// is not there (Microsoft's symbol server does not publish .mui files: msdl answers
// 404, as seen for pnidui) and LoadStringW returns 0: the entries stay
// without text and the menu does not draw. pnidui has the same problem and that is
// why resource 3014 has to be served by hand.
// The same is done here for the strings: they are the AUTHENTIC ones
// of the Italian Windows 10 MUI (10.0.19041.1, taken from the Microsoft
// language pack; for pnidui: 10 strings, for stobject: 57), served ONLY if
// Windows does not find its own: if the .mui is there, nothing is touched.
// One entry of the tray tables below: the text in every language of UiLangId, in
// the same order (IT, EN, FR, ES, DE, PT, NL, RU, JA, PL, DA, SV, NO, FI, TR). The
// text is served only when Windows does not find its own (see LoadStringW_Hook):
// first the Windows 10 file of the store, then this table in the language of the
// interface, then English.
struct TrayString {
    UINT id;
    const wchar_t* text[LANG_COUNT];
};

static const TrayString kStobjectStrings[] = {
    { 144,
      {
        L"Avvisa quando è necessario sostituire la batteria",   // IT
        L"Notify me when the battery needs to be replaced",   // EN
        L"M'avertir quand la batterie doit être remplacée",   // FR
        L"Notificarme cuando sea necesario reemplazar la batería",   // ES
        L"Benachrichtigen, wenn der Akku ausgetauscht werden muss",   // DE
        L"Avisar quando for necessário substituir a bateria",   // PT
        L"Mij waarschuwen wanneer de batterij moet worden vervangen",   // NL
        L"Уведомлять о необходимости замены батареи",   // RU
        L"バッテリの交換が必要なときに通知する",   // JA
        L"Powiadom, gdy bateria wymaga wymiany",   // PL
        L"Giv mig besked, når batteriet skal skiftes",   // DA
        L"Meddela när batteriet behöver bytas",   // SV
        L"Varsle når batteriet må skiftes",   // NO
        L"Ilmoita, kun akku on vaihdettava",   // FI
        L"Pilin değiştirilmesi gerektiğinde bildir"   // TR
      } },
    { 150,
      {
        L"Opzioni risparmio energia",   // IT
        L"Power options",   // EN
        L"Options d'alimentation",   // FR
        L"Opciones de energía",   // ES
        L"Energieoptionen",   // DE
        L"Opções de energia",   // PT
        L"Energiebeheer",   // NL
        L"Электропитание",   // RU
        L"電源オプション",   // JA
        L"Opcje zasilania",   // PL
        L"Strømindstillinger",   // DA
        L"Energialternativ",   // SV
        L"Alternativer for strøm",   // NO
        L"Virta-asetukset",   // FI
        L"Güç seçenekleri"   // TR
      } },
    { 151,
      {
        L"Centro PC portatile Windows",   // IT
        L"Windows Mobility Center",   // EN
        L"Centre de mobilité Windows",   // FR
        L"Centro de movilidad de Windows",   // ES
        L"Windows Mobilitätscenter",   // DE
        L"Centro de Mobilidade do Windows",   // PT
        L"Windows Mobiliteitscentrum",   // NL
        L"Центр мобильности Windows",   // RU
        L"Windows モビリティ センター",   // JA
        L"Centrum mobilności systemu Windows",   // PL
        L"Windows Mobilitetscenter",   // DA
        L"Windows Mobility Center",   // SV
        L"Windows Mobility Center",   // NO
        L"Windows Mobility Center",   // FI
        L"Windows Mobility Center"   // TR
      } },
    { 166,
      {
        L"Opzioni risparmio energia",   // IT
        L"Power options",   // EN
        L"Options d'alimentation",   // FR
        L"Opciones de energía",   // ES
        L"Energieoptionen",   // DE
        L"Opções de energia",   // PT
        L"Energiebeheer",   // NL
        L"Электропитание",   // RU
        L"電源オプション",   // JA
        L"Opcje zasilania",   // PL
        L"Strømindstillinger",   // DA
        L"Energialternativ",   // SV
        L"Alternativer for strøm",   // NO
        L"Virta-asetukset",   // FI
        L"Güç seçenekleri"   // TR
      } },
    { 167,
      {
        L"Centro PC portatile Windows",   // IT
        L"Windows Mobility Center",   // EN
        L"Centre de mobilité Windows",   // FR
        L"Centro de movilidad de Windows",   // ES
        L"Windows Mobilitätscenter",   // DE
        L"Centro de Mobilidade do Windows",   // PT
        L"Windows Mobiliteitscentrum",   // NL
        L"Центр мобильности Windows",   // RU
        L"Windows モビリティ センター",   // JA
        L"Centrum mobilności systemu Windows",   // PL
        L"Windows Mobilitetscenter",   // DA
        L"Windows Mobility Center",   // SV
        L"Windows Mobility Center",   // NO
        L"Windows Mobility Center",   // FI
        L"Windows Mobility Center"   // TR
      } },
    { 181,
      {
        L"Misuratore alimentazione",   // IT
        L"Power meter",   // EN
        L"Compteur d'alimentation",   // FR
        L"Medidor de energía",   // ES
        L"Energieanzeige",   // DE
        L"Medidor de energia",   // PT
        L"Energiemeter",   // NL
        L"Индикатор питания",   // RU
        L"電源メーター",   // JA
        L"Miernik zasilania",   // PL
        L"Strømmåler",   // DA
        L"Energimätare",   // SV
        L"Strømmåler",   // NO
        L"Virran mittari",   // FI
        L"Güç ölçer"   // TR
      } },
    { 186,
      {
        L"Combinazione per il risparmio di energia corrente: ",   // IT
        L"Current power plan: ",   // EN
        L"Mode de gestion de l'alimentation actuel : ",   // FR
        L"Plan de energía actual: ",   // ES
        L"Aktueller Energiesparplan: ",   // DE
        L"Plano de energia atual: ",   // PT
        L"Huidig energieplan: ",   // NL
        L"Текущий план электропитания: ",   // RU
        L"現在の電源プラン: ",   // JA
        L"Bieżący plan zasilania: ",   // PL
        L"Aktuel strømfunktion: ",   // DA
        L"Aktuellt energischema: ",   // SV
        L"Gjeldende strømplan: ",   // NO
        L"Nykyinen virransäästösuunnitelma: ",   // FI
        L"Geçerli güç planı: "   // TR
      } },
    { 187,
      {
        L"Sconosciuto",   // IT
        L"Unknown",   // EN
        L"Inconnu",   // FR
        L"Desconocido",   // ES
        L"Unbekannt",   // DE
        L"Desconhecido",   // PT
        L"Onbekend",   // NL
        L"Неизвестно",   // RU
        L"不明",   // JA
        L"Nieznany",   // PL
        L"Ukendt",   // DA
        L"Okänd",   // SV
        L"Ukjent",   // NO
        L"Tuntematon",   // FI
        L"Bilinmiyor"   // TR
      } },
    { 188,
      {
        L"La combinazione per il risparmio di energia corrente potrebbe ridurre le prestazioni del sistema.",   // IT
        L"The current power plan may reduce system performance.",   // EN
        L"Le mode de gestion de l'alimentation actuel peut réduire les performances du système.",   // FR
        L"El plan de energía actual puede reducir el rendimiento del sistema.",   // ES
        L"Der aktuelle Energiesparplan kann die Systemleistung verringern.",   // DE
        L"O plano de energia atual pode reduzir o desempenho do sistema.",   // PT
        L"Het huidige energieplan kan de systeemprestaties verminderen.",   // NL
        L"Текущий план электропитания может снизить производительность системы.",   // RU
        L"現在の電源プランではシステムのパフォーマンスが低下する可能性があります。",   // JA
        L"Bieżący plan zasilania może obniżyć wydajność systemu.",   // PL
        L"Den aktuelle strømfunktion kan nedsætte systemets ydeevne.",   // DA
        L"Det aktuella energischemat kan minska systemprestandan.",   // SV
        L"Gjeldende strømplan kan redusere systemytelsen.",   // NO
        L"Nykyinen virransäästösuunnitelma voi heikentää järjestelmän suorituskykyä.",   // FI
        L"Geçerli güç planı sistem performansını düşürebilir."   // TR
      } },
    { 189,
      {
        L"La combinazione per il risparmio di energia corrente potrebbe ridurre la durata della batteria.",   // IT
        L"The current power plan may reduce battery life.",   // EN
        L"Le mode de gestion de l'alimentation actuel peut réduire l'autonomie de la batterie.",   // FR
        L"El plan de energía actual puede reducir la duración de la batería.",   // ES
        L"Der aktuelle Energiesparplan kann die Akkulaufzeit verkürzen.",   // DE
        L"O plano de energia atual pode reduzir a duração da bateria.",   // PT
        L"Het huidige energieplan kan de batterijduur verkorten.",   // NL
        L"Текущий план электропитания может сократить время работы батареи.",   // RU
        L"現在の電源プランではバッテリの駆動時間が短くなる可能性があります。",   // JA
        L"Bieżący plan zasilania może skrócić czas pracy baterii.",   // PL
        L"Den aktuelle strømfunktion kan nedsætte batterilevetiden.",   // DA
        L"Det aktuella energischemat kan minska batteritiden.",   // SV
        L"Gjeldende strømplan kan redusere batterilevetiden.",   // NO
        L"Nykyinen virransäästösuunnitelma voi lyhentää akun kestoa.",   // FI
        L"Geçerli güç planı pil ömrünü kısaltabilir."   // TR
      } },
    { 190,
      {
        L"L'impostazione della luminosità corrente potrebbe ridurre la durata della batteria.",   // IT
        L"The current brightness setting may reduce battery life.",   // EN
        L"Le réglage de luminosité actuel peut réduire l'autonomie de la batterie.",   // FR
        L"La configuración de brillo actual puede reducir la duración de la batería.",   // ES
        L"Die aktuelle Helligkeitseinstellung kann die Akkulaufzeit verkürzen.",   // DE
        L"A definição de luminosidade atual pode reduzir a duração da bateria.",   // PT
        L"De huidige helderheidsinstelling kan de batterijduur verkorten.",   // NL
        L"Текущая настройка яркости может сократить время работы батареи.",   // RU
        L"現在の明るさの設定ではバッテリの駆動時間が短くなる可能性があります。",   // JA
        L"Bieżące ustawienie jasności może skrócić czas pracy baterii.",   // PL
        L"Den aktuelle indstilling for lysstyrke kan nedsætte batterilevetiden.",   // DA
        L"Den aktuella ljusstyrkeinställningen kan minska batteritiden.",   // SV
        L"Gjeldende innstilling for lysstyrke kan redusere batterilevetiden.",   // NO
        L"Nykyinen kirkkausasetus voi lyhentää akun kestoa.",   // FI
        L"Geçerli parlaklık ayarı pil ömrünü kısaltabilir."   // TR
      } },
    { 191,
      {
        L"Problema con la batteria. Il computer potrebbe spegnersi all'improvviso.",   // IT
        L"There is a problem with the battery. The computer may shut down suddenly.",   // EN
        L"Problème avec la batterie. L'ordinateur peut s'arrêter brusquement.",   // FR
        L"Hay un problema con la batería. Es posible que el equipo se apague de repente.",   // ES
        L"Es liegt ein Problem mit dem Akku vor. Der Computer kann plötzlich herunterfahren.",   // DE
        L"Existe um problema com a bateria. O computador pode desligar-se de repente.",   // PT
        L"Er is een probleem met de batterij. De computer kan plotseling uitschakelen.",   // NL
        L"Проблема с батареей. Компьютер может внезапно выключиться.",   // RU
        L"バッテリに問題があります。コンピューターが突然シャットダウンする可能性があります。",   // JA
        L"Wystąpił problem z baterią. Komputer może nagle się wyłączyć.",   // PL
        L"Der er et problem med batteriet. Computer kan lukke ned uden varsel.",   // DA
        L"Det finns ett problem med batteriet. Datorn kan stängas av plötsligt.",   // SV
        L"Det er et problem med batteriet. Datamaskinen kan slås av plutselig.",   // NO
        L"Akussa on ongelma. Tietokone voi sammua yllättäen.",   // FI
        L"Pille ilgili bir sorun var. Bilgisayar aniden kapanabilir."   // TR
      } },
    { 192,
      {
        L"Gli avvisi sullo stato delle batterie sono disabilitati. È possibile che il computer si spenga improvvisamente.",   // IT
        L"Battery status notifications are turned off. The computer may shut down unexpectedly.",   // EN
        L"Les notifications d'état de la batterie sont désactivées. L'ordinateur peut s'arrêter de façon inattendue.",   // FR
        L"Las notificaciones de estado de la batería están desactivadas. Es posible que el equipo se apague inesperadamente.",   // ES
        L"Benachrichtigungen zum Akkustatus sind deaktiviert. Der Computer kann unerwartet herunterfahren.",   // DE
        L"As notificações de estado da bateria estão desativadas. O computador pode desligar-se inesperadamente.",   // PT
        L"Meldingen over de batterijstatus zijn uitgeschakeld. De computer kan onverwacht uitschakelen.",   // NL
        L"Уведомления о состоянии батареи отключены. Компьютер может неожиданно выключиться.",   // RU
        L"バッテリの状態の通知がオフになっています。コンピューターが予期せずシャットダウンする可能性があります。",   // JA
        L"Powiadomienia o stanie baterii są wyłączone. Komputer może nieoczekiwanie się wyłączyć.",   // PL
        L"Meddelelser om batteristatus er slået fra. Computer kan lukke ned uventet.",   // DA
        L"Aviseringsfunktionen för batteristatus är avstängd. Datorn kan stängas av oväntat.",   // SV
        L"Varsler om batteristatus er slått av. Datamaskinen kan slås av uventet.",   // NO
        L"Akun tilaa koskevat ilmoitukset on poistettu käytöstä. Tietokone voi sammua odottamatta.",   // FI
        L"Pil durumu bildirimleri kapalı. Bilgisayar beklenmedik şekilde kapanabilir."   // TR
      } },
    { 206,
      {
        L"Misuratore alimentazione",   // IT
        L"Power meter",   // EN
        L"Compteur d'alimentation",   // FR
        L"Medidor de energía",   // ES
        L"Energieanzeige",   // DE
        L"Medidor de energia",   // PT
        L"Energiemeter",   // NL
        L"Индикатор питания",   // RU
        L"電源メーター",   // JA
        L"Miernik zasilania",   // PL
        L"Strømmåler",   // DA
        L"Energimätare",   // SV
        L"Strømmåler",   // NO
        L"Virran mittari",   // FI
        L"Güç ölçer"   // TR
      } },
    { 207,
      {
        L"Informazioni sul risparmio di energia",   // IT
        L"About power saving",   // EN
        L"À propos des économies d'énergie",   // FR
        L"Acerca del ahorro de energía",   // ES
        L"Informationen zur Energieeinsparung",   // DE
        L"Acerca da poupança de energia",   // PT
        L"Over energiebesparing",   // NL
        L"Об экономии энергии",   // RU
        L"省電力について",   // JA
        L"Informacje o oszczędzaniu energii",   // PL
        L"Om strømbesparelse",   // DA
        L"Om energisparande",   // SV
        L"Om strømsparing",   // NO
        L"Tietoja virransäästöstä",   // FI
        L"Güç tasarrufu hakkında"   // TR
      } },
    { 208,
      {
        L"Selezionare una combinazione per il risparmio di energia:",   // IT
        L"Select a power plan:",   // EN
        L"Sélectionner un mode de gestion de l'alimentation :",   // FR
        L"Seleccione un plan de energía:",   // ES
        L"Energiesparplan auswählen:",   // DE
        L"Selecionar um plano de energia:",   // PT
        L"Een energieplan selecteren:",   // NL
        L"Выбор плана электропитания:",   // RU
        L"電源プランを選択してください:",   // JA
        L"Wybierz plan zasilania:",   // PL
        L"Vælg en strømfunktion:",   // DA
        L"Välj ett energischema:",   // SV
        L"Velg en strømplan:",   // NO
        L"Valitse virransäästösuunnitelma:",   // FI
        L"Bir güç planı seçin:"   // TR
      } },
    { 209,
      {
        L"Altre opzioni di risparmio energia",   // IT
        L"More power options",   // EN
        L"Autres options d'alimentation",   // FR
        L"Más opciones de energía",   // ES
        L"Weitere Energieoptionen",   // DE
        L"Mais opções de energia",   // PT
        L"Meer energieopties",   // NL
        L"Дополнительные параметры электропитания",   // RU
        L"その他の電源オプション",   // JA
        L"Więcej opcji zasilania",   // PL
        L"Flere strømindstillinger",   // DA
        L"Fler energialternativ",   // SV
        L"Flere strømalternativer",   // NO
        L"Lisää virta-asetuksia",   // FI
        L"Diğer güç seçenekleri"   // TR
      } },
    { 210,
      {
        L"Combinazioni risparmio energia",   // IT
        L"Power plans",   // EN
        L"Modes de gestion de l'alimentation",   // FR
        L"Planes de energía",   // ES
        L"Energiesparpläne",   // DE
        L"Planos de energia",   // PT
        L"Energieplannen",   // NL
        L"Планы электропитания",   // RU
        L"電源プラン",   // JA
        L"Plany zasilania",   // PL
        L"Strømfunktioner",   // DA
        L"Energischeman",   // SV
        L"Strømplaner",   // NO
        L"Virransäästösuunnitelmat",   // FI
        L"Güç planları"   // TR
      } },
    { 211,
      {
        L"Alcune impostazioni sono gestite dall'amministratore del sistema.",   // IT
        L"Some settings are managed by your system administrator.",   // EN
        L"Certains paramètres sont gérés par votre administrateur système.",   // FR
        L"El administrador del sistema gestiona algunas opciones de configuración.",   // ES
        L"Einige Einstellungen werden von Ihrem Systemadministrator verwaltet.",   // DE
        L"Algumas definições são geridas pelo administrador do sistema.",   // PT
        L"Sommige instellingen worden beheerd door uw systeembeheerder.",   // NL
        L"Некоторыми параметрами управляет системный администратор.",   // RU
        L"一部の設定はシステム管理者が管理しています。",   // JA
        L"Niektóre ustawienia są zarządzane przez administratora systemu.",   // PL
        L"Nogle indstillinger administreres af systemadministratoren.",   // DA
        L"Vissa inställningar hanteras av systemadministratören.",   // SV
        L"Noen innstillinger administreres av systemansvarlig.",   // NO
        L"Järjestelmänvalvoja hallinnoi joitakin asetuksia.",   // FI
        L"Bazı ayarlar sistem yöneticiniz tarafından yönetilir."   // TR
      } },
    { 212,
      {
        L"Ragioni che impediscono la modifica di alcune impostazioni",   // IT
        L"Reasons why some settings cannot be changed",   // EN
        L"Raisons pour lesquelles certains paramètres ne peuvent pas être modifiés",   // FR
        L"Razones por las que no se pueden cambiar algunas opciones",   // ES
        L"Gründe, warum einige Einstellungen nicht geändert werden können",   // DE
        L"Motivos pelos quais algumas definições não podem ser alteradas",   // PT
        L"Redenen waarom sommige instellingen niet kunnen worden gewijzigd",   // NL
        L"Причины, по которым некоторые параметры нельзя изменить",   // RU
        L"一部の設定を変更できない理由",   // JA
        L"Powody, dla których nie można zmienić niektórych ustawień",   // PL
        L"Årsager til, at nogle indstillinger ikke kan ændres",   // DA
        L"Orsaker till att vissa inställningar inte kan ändras",   // SV
        L"Årsaker til at noen innstillinger ikke kan endres",   // NO
        L"Syyt, joiden vuoksi joitakin asetuksia ei voi muuttaa",   // FI
        L"Bazı ayarların değiştirilememesinin nedenleri"   // TR
      } },
    { 213,
      {
        L"Batterie",   // IT
        L"Batteries",   // EN
        L"Batteries",   // FR
        L"Baterías",   // ES
        L"Akkus",   // DE
        L"Baterias",   // PT
        L"Batterijen",   // NL
        L"Батареи",   // RU
        L"バッテリ",   // JA
        L"Baterie",   // PL
        L"Batterier",   // DA
        L"Batterier",   // SV
        L"Batterier",   // NO
        L"Akut",   // FI
        L"Piller"   // TR
      } },
    { 214,
      {
        L"Alcune impostazioni sono gestite dall'amministratore del sistema.",   // IT
        L"Some settings are managed by your system administrator.",   // EN
        L"Certains paramètres sont gérés par votre administrateur système.",   // FR
        L"El administrador del sistema gestiona algunas opciones de configuración.",   // ES
        L"Einige Einstellungen werden von Ihrem Systemadministrator verwaltet.",   // DE
        L"Algumas definições são geridas pelo administrador do sistema.",   // PT
        L"Sommige instellingen worden beheerd door uw systeembeheerder.",   // NL
        L"Некоторыми параметрами управляет системный администратор.",   // RU
        L"一部の設定はシステム管理者が管理しています。",   // JA
        L"Niektóre ustawienia są zarządzane przez administratora systemu.",   // PL
        L"Nogle indstillinger administreres af systemadministratoren.",   // DA
        L"Vissa inställningar hanteras av systemadministratören.",   // SV
        L"Noen innstillinger administreres av systemansvarlig.",   // NO
        L"Järjestelmänvalvoja hallinnoi joitakin asetuksia.",   // FI
        L"Bazı ayarlar sistem yöneticiniz tarafından yönetilir."   // TR
      } },
    { 227,
      {
        L"Rimozione sicura dell'hardware ed espulsione supporti",   // IT
        L"Safely Remove Hardware and Eject Media",   // EN
        L"Supprimer le périphérique en toute sécurité et éjecter le média",   // FR
        L"Quitar hardware de forma segura y expulsar el medio",   // ES
        L"Hardware sicher entfernen und Medium auswerfen",   // DE
        L"Remover hardware em segurança e ejetar suporte",   // PT
        L"Hardware veilig verwijderen en media uitwerpen",   // NL
        L"Безопасное извлечение устройства и носителя",   // RU
        L"ハードウェアを安全に取り外してメディアを取り出す",   // JA
        L"Bezpieczne usuwanie sprzętu i wysuwanie nośnika",   // PL
        L"Sikker fjernelse af hardware og udskubning af medie",   // DA
        L"Säker borttagning av maskinvara och utmatning av media",   // SV
        L"Sikker fjerning av maskinvare og utløsing av medier",   // NO
        L"Poista laitteisto turvallisesti ja poista tietoväline",   // FI
        L"Donanımı Güvenle Kaldır ve Medyayı Çıkar"   // TR
      } },
    { 231,
      {
        L"A&pri Dispositivi e stampanti",   // IT
        L"&Open Devices and Printers",   // EN
        L"&Ouvrir Périphériques et imprimantes",   // FR
        L"&Abrir dispositivos e impresoras",   // ES
        L"&Geräte und Drucker öffnen",   // DE
        L"&Abrir Dispositivos e Impressoras",   // PT
        L"Apparaten en printers &openen",   // NL
        L"&Открыть устройства и принтеры",   // RU
        L"デバイスとプリンターを開く(&O)",   // JA
        L"&Otwórz urządzenia i drukarki",   // PL
        L"&Åbn Enheder og printere",   // DA
        L"&Öppna enheter och skrivare",   // SV
        L"&Åpne enheter og skrivere",   // NO
        L"&Avaa laitteet ja tulostimet",   // FI
        L"&Aygıtlar ve Yazıcılar'ı aç"   // TR
      } },
    { 238,
      {
        L"%s",   // IT
        L"%s",   // EN
        L"%s",   // FR
        L"%s",   // ES
        L"%s",   // DE
        L"%s",   // PT
        L"%s",   // NL
        L"%s",   // RU
        L"%s",   // JA
        L"%s",   // PL
        L"%s",   // DA
        L"%s",   // SV
        L"%s",   // NO
        L"%s",   // FI
        L"%s"   // TR
      } },
    { 239,
      {
        L"Espelli %s",   // IT
        L"Eject %s",   // EN
        L"Éjecter %s",   // FR
        L"Expulsar %s",   // ES
        L"%s auswerfen",   // DE
        L"Ejetar %s",   // PT
        L"%s uitwerpen",   // NL
        L"Извлечь %s",   // RU
        L"%s を取り出す",   // JA
        L"Wysuń %s",   // PL
        L"Skub %s ud",   // DA
        L"Mata ut %s",   // SV
        L"Løs ut %s",   // NO
        L"Poista %s",   // FI
        L"%s öğesini çıkar"   // TR
      } },
    { 240,
      {
        L"   -   %s",   // IT
        L"   -   %s",   // EN
        L"   -   %s",   // FR
        L"   -   %s",   // ES
        L"   -   %s",   // DE
        L"   -   %s",   // PT
        L"   -   %s",   // NL
        L"   -   %s",   // RU
        L"   -   %s",   // JA
        L"   -   %s",   // PL
        L"   -   %s",   // DA
        L"   -   %s",   // SV
        L"   -   %s",   // NO
        L"   -   %s",   // FI
        L"   -   %s"   // TR
      } },
    { 241,
      {
        L"   -   Espelli %s",   // IT
        L"   -   Eject %s",   // EN
        L"   -   Éjecter %s",   // FR
        L"   -   Expulsar %s",   // ES
        L"   -   %s auswerfen",   // DE
        L"   -   Ejetar %s",   // PT
        L"   -   %s uitwerpen",   // NL
        L"   -   Извлечь %s",   // RU
        L"   -   %s を取り出す",   // JA
        L"   -   Wysuń %s",   // PL
        L"   -   Skub %s ud",   // DA
        L"   -   Mata ut %s",   // SV
        L"   -   Løs ut %s",   // NO
        L"   -   Poista %s",   // FI
        L"   -   %s öğesini çıkar"   // TR
      } },
    { 346,
      {
        L"Tasti permanenti",   // IT
        L"Sticky Keys",   // EN
        L"Touches rémanentes",   // FR
        L"Teclas especiales",   // ES
        L"Einrastfunktion",   // DE
        L"Teclas Persistentes",   // PT
        L"Plaktoetsen",   // NL
        L"Залипание клавиш",   // RU
        L"固定キー機能",   // JA
        L"Klawisze trwałe",   // PL
        L"Fastholdelsestaster",   // DA
        L"Fästtangenter",   // SV
        L"Festetaster",   // NO
        L"Kiinnitys",   // FI
        L"Yapışkan Tuşlar"   // TR
      } },
    { 347,
      {
        L"Controllo puntatore",   // IT
        L"Pointer control",   // EN
        L"Contrôle du pointeur",   // FR
        L"Control del puntero",   // ES
        L"Zeigersteuerung",   // DE
        L"Controlo do ponteiro",   // PT
        L"Aanwijzerbeheer",   // NL
        L"Управление указателем",   // RU
        L"ポインターの制御",   // JA
        L"Sterowanie wskaźnikiem",   // PL
        L"Styring af markør",   // DA
        L"Pekarkontroll",   // SV
        L"Pekekontroll",   // NO
        L"Osoittimen hallinta",   // FI
        L"İşaretçi denetimi"   // TR
      } },
    { 348,
      {
        L"Filtro tasti",   // IT
        L"Filter Keys",   // EN
        L"Touches filtrées",   // FR
        L"Filtros de teclas",   // ES
        L"Filtertasten",   // DE
        L"Teclas de Filtragem",   // PT
        L"Filtertoetsen",   // NL
        L"Фильтрация ввода",   // RU
        L"フィルター キー",   // JA
        L"Klawisze filtrujące",   // PL
        L"Filtertaster",   // DA
        L"Filtertangenter",   // SV
        L"Filtertaster",   // NO
        L"Suodatusavaimet",   // FI
        L"Filtre Tuşları"   // TR
      } },
    { 417,
      {
        L"Batteria %s: %s",   // IT
        L"Battery %s: %s",   // EN
        L"Batterie %s : %s",   // FR
        L"Batería %s: %s",   // ES
        L"Akku %s: %s",   // DE
        L"Bateria %s: %s",   // PT
        L"Batterij %s: %s",   // NL
        L"Батарея %s: %s",   // RU
        L"バッテリ %s: %s",   // JA
        L"Bateria %s: %s",   // PL
        L"Batteri %s: %s",   // DA
        L"Batteri %s: %s",   // SV
        L"Batteri %s: %s",   // NO
        L"Akku %s: %s",   // FI
        L"Pil %s: %s"   // TR
      } },
    { 418,
      {
        L"Batteria di breve durata %s: %s",   // IT
        L"Short battery life %s: %s",   // EN
        L"Autonomie faible %s : %s",   // FR
        L"Duración de batería baja %s: %s",   // ES
        L"Kurze Akkulaufzeit %s: %s",   // DE
        L"Bateria de curta duração %s: %s",   // PT
        L"Korte batterijduur %s: %s",   // NL
        L"Короткое время работы батареи %s: %s",   // RU
        L"バッテリ駆動時間が短い %s: %s",   // JA
        L"Krótki czas pracy baterii %s: %s",   // PL
        L"Kort batterilevetid %s: %s",   // DA
        L"Kort batteritid %s: %s",   // SV
        L"Kort batterilevetid %s: %s",   // NO
        L"Akun kesto on lyhyt %s: %s",   // FI
        L"Kısa pil ömrü %s: %s"   // TR
      } },
    { 419,
      {
        L"Non presente",   // IT
        L"Not present",   // EN
        L"Non présent",   // FR
        L"No presente",   // ES
        L"Nicht vorhanden",   // DE
        L"Não presente",   // PT
        L"Niet aanwezig",   // NL
        L"Отсутствует",   // RU
        L"存在しません",   // JA
        L"Nieobecny",   // PL
        L"Ikke til stede",   // DA
        L"Saknas",   // SV
        L"Ikke til stede",   // NO
        L"Ei paikalla",   // FI
        L"Yok"   // TR
      } },
    { 420,
      {
        L"#%1!u!",   // IT
        L"#%1!u!",   // EN
        L"#%1!u!",   // FR
        L"#%1!u!",   // ES
        L"#%1!u!",   // DE
        L"#%1!u!",   // PT
        L"#%1!u!",   // NL
        L"#%1!u!",   // RU
        L"#%1!u!",   // JA
        L"#%1!u!",   // PL
        L"#%1!u!",   // DA
        L"#%1!u!",   // SV
        L"#%1!u!",   // NO
        L"#%1!u!",   // FI
        L"#%1!u!"   // TR
      } },
    { 421,
      {
        L"La batteria in uso è quasi scarica e non può essere caricata completamente. Collegare il PC alla rete elettrica oppure arrestarlo e sostituire la batteria.",   // IT
        L"The battery in use is critically low and cannot be charged completely. Connect the PC to power or shut it down and replace the battery.",   // EN
        L"La batterie utilisée est presque déchargée et ne peut pas être rechargée complètement. Branchez le PC sur secteur ou arrêtez-le et remplacez la batterie.",   // FR
        L"La batería en uso está muy baja y no se puede cargar por completo. Conecte el equipo a la red eléctrica o apáguelo y sustituya la batería.",   // ES
        L"Der verwendete Akku ist fast leer und kann nicht vollständig geladen werden. Schließen Sie den PC an das Stromnetz an, oder fahren Sie ihn herunter und tauschen Sie den Akku aus.",   // DE
        L"A bateria em uso está quase vazia e não pode ser carregada completamente. Ligue o computador à corrente elétrica ou desligue-o e substitua a bateria.",   // PT
        L"De gebruikte batterij is bijna leeg en kan niet volledig worden opgeladen. Sluit de pc aan op het stopcontact of schakel deze uit en vervang de batterij.",   // NL
        L"Используемая батарея почти разряжена и не может быть полностью заряжена. Подключите компьютер к электросети или выключите его и замените батарею.",   // RU
        L"使用中のバッテリは残量が非常に少なく、完全に充電できません。PC を電源に接続するか、シャットダウンしてバッテリを交換してください。",   // JA
        L"Używana bateria jest prawie rozładowana i nie można jej w pełni naładować. Podłącz komputer do zasilania albo wyłącz go i wymień baterię.",   // PL
        L"Det anvendte batteri er næsten afladet og kan ikke oplades helt. Slut pc'en til strøm, eller luk den ned, og udskift batteriet.",   // DA
        L"Batteriet som används är nästan slut och kan inte laddas helt. Anslut datorn till elnätet eller stäng av den och byt batteri.",   // SV
        L"Batteriet som brukes er nesten tomt og kan ikke lades helt opp. Koble PC-en til strøm, eller slå den av og bytt batteri.",   // NO
        L"Käytössä oleva akku on lähes tyhjä, eikä sitä voi ladata täyteen. Kytke tietokone sähköverkkoon tai sammuta se ja vaihda akku.",   // FI
        L"Kullanılan pilin şarjı neredeyse bitti ve tam olarak şarj edilemiyor. PC'yi güç kaynağına bağlayın veya kapatıp pili değiştirin."   // TR
      } },
    { 422,
      {
        L"Provare a sostituire la batteria",   // IT
        L"Try replacing the battery",   // EN
        L"Essayez de remplacer la batterie",   // FR
        L"Intente reemplazar la batería",   // ES
        L"Versuchen Sie, den Akku auszutauschen",   // DE
        L"Tente substituir a bateria",   // PT
        L"Probeer de batterij te vervangen",   // NL
        L"Попробуйте заменить батарею",   // RU
        L"バッテリの交換を試してください",   // JA
        L"Spróbuj wymienić baterię",   // PL
        L"Prøv at udskifte batteriet",   // DA
        L"Försök byta batteriet",   // SV
        L"Prøv å bytte batteriet",   // NO
        L"Yritä vaihtaa akku",   // FI
        L"Pili değiştirmeyi deneyin"   // TR
      } },
    { 423,
      {
        L"Collegare il computer ad una presa di corrente.",   // IT
        L"Connect the computer to a power outlet.",   // EN
        L"Connectez l'ordinateur à une prise électrique.",   // FR
        L"Conecte el equipo a una toma de corriente.",   // ES
        L"Schließen Sie den Computer an eine Steckdose an.",   // DE
        L"Ligue o computador a uma tomada elétrica.",   // PT
        L"Sluit de computer aan op een stopcontact.",   // NL
        L"Подключите компьютер к электрической розетке.",   // RU
        L"コンピューターを電源コンセントに接続してください。",   // JA
        L"Podłącz komputer do gniazda zasilania.",   // PL
        L"Slut computeren til en stikkontakt.",   // DA
        L"Anslut datorn till ett eluttag.",   // SV
        L"Koble datamaskinen til et strømuttak.",   // NO
        L"Kytke tietokone pistorasiaan.",   // FI
        L"Bilgisayarı bir prize takın."   // TR
      } },
    { 424,
      {
        L"Impostazioni sfondo del desktop",   // IT
        L"Desktop background settings",   // EN
        L"Paramètres d'arrière-plan du Bureau",   // FR
        L"Configuración del fondo de escritorio",   // ES
        L"Desktophintergrund-Einstellungen",   // DE
        L"Definições do plano de fundo da área de trabalho",   // PT
        L"Instellingen voor bureaubladachtergrond",   // NL
        L"Параметры фона рабочего стола",   // RU
        L"デスクトップの背景の設定",   // JA
        L"Ustawienia tła pulpitu",   // PL
        L"Indstillinger for skrivebordsbaggrund",   // DA
        L"Inställningar för skrivbordsbakgrund",   // SV
        L"Innstillinger for skrivebordsbakgrunn",   // NO
        L"Työpöydän taustan asetukset",   // FI
        L"Masaüstü arka planı ayarları"   // TR
      } },
    { 425,
      {
        L"Modifica le impostazioni di risparmio energia per lo sfondo del desktop.",   // IT
        L"Change power saving settings for the desktop background.",   // EN
        L"Modifiez les paramètres d'économie d'énergie pour l'arrière-plan du Bureau.",   // FR
        L"Cambie la configuración de ahorro de energía del fondo de escritorio.",   // ES
        L"Ändern Sie die Energieeinstellungen für den Desktophintergrund.",   // DE
        L"Altere as definições de poupança de energia do plano de fundo.",   // PT
        L"Wijzig energiebesparingsinstellingen voor de bureaubladachtergrond.",   // NL
        L"Измените параметры энергосбережения для фона рабочего стола.",   // RU
        L"デスクトップの背景の省電力設定を変更します。",   // JA
        L"Zmień ustawienia oszczędzania energii tła pulpitu.",   // PL
        L"Skift strømbesparelsesindstillinger for skrivebordsbaggrunden.",   // DA
        L"Ändra energisparinställningar för skrivbordsbakgrunden.",   // SV
        L"Endre strømsparingsinnstillinger for skrivebordsbakgrunnen.",   // NO
        L"Muuta työpöydän taustan virransäästöasetuksia.",   // FI
        L"Masaüstü arka planı için güç tasarrufu ayarlarını değiştirin."   // TR
      } },
    { 426,
      {
        L"Presentazione",   // IT
        L"Slide show",   // EN
        L"Diaporama",   // FR
        L"Presentación",   // ES
        L"Diashow",   // DE
        L"Apresentação de diapositivos",   // PT
        L"Diavoorstelling",   // NL
        L"Слайд-шоу",   // RU
        L"スライド ショー",   // JA
        L"Pokaz slajdów",   // PL
        L"Diasshow",   // DA
        L"Bildspel",   // SV
        L"Lysbildeframvisning",   // NO
        L"Diaesitys",   // FI
        L"Slayt gösterisi"   // TR
      } },
    { 427,
      {
        L"Specificare quando si desidera che sia disponibile la diapositiva di sfondo del desktop.",   // IT
        L"Specify when you want the desktop background slide show to be available.",   // EN
        L"Spécifiez quand vous souhaitez que le diaporama d'arrière-plan du Bureau soit disponible.",   // FR
        L"Especifique cuándo desea que la presentación del fondo de escritorio esté disponible.",   // ES
        L"Geben Sie an, wann die Desktophintergrund-Diashow verfügbar sein soll.",   // DE
        L"Especifique quando pretende que a apresentação de diapositivos de fundo esteja disponível.",   // PT
        L"Geef op wanneer de diavoorstelling voor de bureaubladachtergrond beschikbaar moet zijn.",   // NL
        L"Укажите, когда должно быть доступно слайд-шоу фона рабочего стола.",   // RU
        L"デスクトップの背景のスライド ショーを利用するタイミングを指定します。",   // JA
        L"Określ, kiedy pokaz slajdów tła pulpitu ma być dostępny.",   // PL
        L"Angiv, hvornår diasshowet for skrivebordsbaggrunden skal være tilgængeligt.",   // DA
        L"Ange när bildspelet för skrivbordsbakgrunden ska vara tillgängligt.",   // SV
        L"Angi når lysbildeframvisningen for skrivebordsbakgrunnen skal være tilgjengelig.",   // NO
        L"Määritä, milloin työpöydän taustan diaesitys on käytettävissä.",   // FI
        L"Masaüstü arka planı slayt gösterisinin ne zaman kullanılabilir olacağını belirtin."   // TR
      } },
    { 428,
      {
        L"Sospesa",   // IT
        L"Suspended",   // EN
        L"Suspendu",   // FR
        L"Suspendida",   // ES
        L"Angehalten",   // DE
        L"Suspensa",   // PT
        L"Onderbroken",   // NL
        L"Приостановлено",   // RU
        L"一時停止",   // JA
        L"Wstrzymane",   // PL
        L"Suspenderet",   // DA
        L"Pausat",   // SV
        L"Satt på pause",   // NO
        L"Keskeytetty",   // FI
        L"Askıya alındı"   // TR
      } },
    { 429,
      {
        L"Presentazione sospesa per risparmiare energia.",   // IT
        L"Slide show suspended to save power.",   // EN
        L"Diaporama suspendu pour économiser l'énergie.",   // FR
        L"Presentación suspendida para ahorrar energía.",   // ES
        L"Diashow wurde angehalten, um Energie zu sparen.",   // DE
        L"Apresentação suspensa para poupar energia.",   // PT
        L"Diavoorstelling onderbroken om energie te besparen.",   // NL
        L"Слайд-шоу приостановлено для экономии энергии.",   // RU
        L"省電力のためスライド ショーは一時停止されています。",   // JA
        L"Pokaz slajdów wstrzymany w celu oszczędzania energii.",   // PL
        L"Diasshowet er sat på pause for at spare strøm.",   // DA
        L"Bildspelet pausades för att spara energi.",   // SV
        L"Lysbildeframvisningen er satt på pause for å spare strøm.",   // NO
        L"Diaesitys keskeytettiin virran säästämiseksi.",   // FI
        L"Güç tasarrufu için slayt gösterisi askıya alındı."   // TR
      } },
    { 430,
      {
        L"Disponibile",   // IT
        L"Available",   // EN
        L"Disponible",   // FR
        L"Disponible",   // ES
        L"Verfügbar",   // DE
        L"Disponível",   // PT
        L"Beschikbaar",   // NL
        L"Доступно",   // RU
        L"利用可能",   // JA
        L"Dostępne",   // PL
        L"Tilgængelig",   // DA
        L"Tillgänglig",   // SV
        L"Tilgjengelig",   // NO
        L"Käytettävissä",   // FI
        L"Kullanılabilir"   // TR
      } },
    { 431,
      {
        L"Disponibile",   // IT
        L"Available",   // EN
        L"Disponible",   // FR
        L"Disponible",   // ES
        L"Verfügbar",   // DE
        L"Disponível",   // PT
        L"Beschikbaar",   // NL
        L"Доступно",   // RU
        L"利用可能",   // JA
        L"Dostępne",   // PL
        L"Tilgængelig",   // DA
        L"Tillgänglig",   // SV
        L"Tilgjengelig",   // NO
        L"Käytettävissä",   // FI
        L"Kullanılabilir"   // TR
      } },
    { 432,
      {
        L"&Sfondo per il desktop successivo",   // IT
        L"&Next desktop background",   // EN
        L"Arrière-plan du Bureau suiva&nt",   // FR
        L"Fondo de escritorio &siguiente",   // ES
        L"&Nächster Desktophintergrund",   // DE
        L"Plano de fundo seguinte da área de &trabalho",   // PT
        L"&Volgende bureaubladachtergrond",   // NL
        L"&Следующий фон рабочего стола",   // RU
        L"次のデスクトップの背景(&N)",   // JA
        L"&Następne tło pulpitu",   // PL
        L"&Næste skrivebordsbaggrund",   // DA
        L"&Nästa skrivbordsbakgrund",   // SV
        L"&Neste skrivebordsbakgrunn",   // NO
        L"&Seuraava työpöydän tausta",   // FI
        L"&Sonraki masaüstü arka planı"   // TR
      } },
    { 433,
      {
        L"Imposta come sfondo del des&ktop",   // IT
        L"Set as desktop back&ground",   // EN
        L"Définir comme arrière-plan du Bureau (&g)",   // FR
        L"Establecer como fondo de escritorio (&g)",   // ES
        L"Als Desktophintergrund festlegen (&g)",   // DE
        L"Definir como plano de fundo (&g)",   // PT
        L"Instellen als bureaubladachtergrond (&g)",   // NL
        L"Сделать фоном рабочего стола (&g)",   // RU
        L"デスクトップの背景に設定(&G)",   // JA
        L"Ustaw jako tło pulpitu (&g)",   // PL
        L"Angiv som skrivebordsbaggrund (&g)",   // DA
        L"Ange som skrivbordsbakgrund (&g)",   // SV
        L"Angi som skrivebordsbakgrunn (&g)",   // NO
        L"Aseta työpöydän taustaksi (&g)",   // FI
        L"Masaüstü arka planı olarak ayarla (&g)"   // TR
      } },
    { 434,
      {
        L"La batteria si scarica anche se il PC è collegato alla rete elettrica.",   // IT
        L"The battery is draining even though the PC is plugged in.",   // EN
        L"La batterie se décharge même si le PC est branché.",   // FR
        L"La batería se agota aunque el equipo esté enchufado.",   // ES
        L"Der Akku wird entladen, obwohl der PC angeschlossen ist.",   // DE
        L"A bateria está a descarregar-se apesar de o computador estar ligado à corrente.",   // PT
        L"De batterij loopt leeg terwijl de pc is aangesloten.",   // NL
        L"Батарея разряжается, несмотря на подключённый компьютер.",   // RU
        L"PC が電源に接続されているのにバッテリが放電しています。",   // JA
        L"Bateria rozładowuje się mimo podłączonego zasilania.",   // PL
        L"Batteriet aflades, selv om pc'en er tilsluttet.",   // DA
        L"Batteriet laddas ur trots att datorn är ansluten.",   // SV
        L"Batteriet tappes selv om PC-en er koblet til.",   // NO
        L"Akku tyhjenee, vaikka tietokone on kytketty verkkovirtaan.",   // FI
        L"PC prize takılı olmasına rağmen pil boşalıyor."   // TR
      } },
    { 436,
      {
        L"Collegare il PC a una presa di corrente",   // IT
        L"Connect the PC to a power outlet",   // EN
        L"Branchez le PC à une prise électrique",   // FR
        L"Conecte el equipo a una toma de corriente",   // ES
        L"Schließen Sie den PC an eine Steckdose an",   // DE
        L"Ligue o computador a uma tomada elétrica",   // PT
        L"Sluit de pc aan op een stopcontact",   // NL
        L"Подключите компьютер к электрической розетке",   // RU
        L"PC を電源コンセントに接続してください",   // JA
        L"Podłącz komputer do gniazda zasilania",   // PL
        L"Slut pc'en til en stikkontakt",   // DA
        L"Anslut datorn till ett eluttag",   // SV
        L"Koble PC-en til et strømuttak",   // NO
        L"Kytke tietokone pistorasiaan",   // FI
        L"PC'yi bir prize takın"   // TR
      } },
    { 437,
      {
        L"Collegarsi alla rete elettrica o trovare un'altra fonte di alimentazione.",   // IT
        L"Connect to power or find another power source.",   // EN
        L"Branchez-vous sur secteur ou recherchez une autre source d'alimentation.",   // FR
        L"Conéctese a la red eléctrica o busque otra fuente de alimentación.",   // ES
        L"Schließen Sie den PC an das Stromnetz an, oder suchen Sie eine andere Stromquelle.",   // DE
        L"Ligue-se à corrente elétrica ou encontre outra fonte de alimentação.",   // PT
        L"Sluit aan op het stopcontact of zoek een andere energiebron.",   // NL
        L"Подключитесь к электросети или найдите другой источник питания.",   // RU
        L"電源に接続するか、別の電源を見つけてください。",   // JA
        L"Podłącz zasilanie lub znajdź inne źródło zasilania.",   // PL
        L"Slut til strøm, eller find en anden strømkilde.",   // DA
        L"Anslut till elnätet eller hitta en annan strömkälla.",   // SV
        L"Koble til strøm eller finn en annen strømkilde.",   // NO
        L"Kytke virtaan tai etsi toinen virtalähde.",   // FI
        L"Güce bağlanın veya başka bir güç kaynağı bulun."   // TR
      } },
    { 438,
      {
        L"Il livello di carica della batteria è molto basso",   // IT
        L"The battery level is very low",   // EN
        L"Le niveau de la batterie est très faible",   // FR
        L"El nivel de batería es muy bajo",   // ES
        L"Der Akkustand ist sehr niedrig",   // DE
        L"O nível da bateria está muito baixo",   // PT
        L"Het batterijniveau is zeer laag",   // NL
        L"Уровень заряда батареи очень низкий",   // RU
        L"バッテリ残量が非常に少なくなっています",   // JA
        L"Poziom naładowania baterii jest bardzo niski",   // PL
        L"Batteriniveauet er meget lavt",   // DA
        L"Batterinivån är mycket låg",   // SV
        L"Batterinivået er svært lavt",   // NO
        L"Akun varaustaso on erittäin alhainen",   // FI
        L"Pil düzeyi çok düşük"   // TR
      } },
    { 439,
      {
        L"Collegare il PC a una presa di corrente.",   // IT
        L"Connect the PC to a power outlet.",   // EN
        L"Branchez le PC à une prise électrique.",   // FR
        L"Conecte el equipo a una toma de corriente.",   // ES
        L"Schließen Sie den PC an eine Steckdose an.",   // DE
        L"Ligue o computador a uma tomada elétrica.",   // PT
        L"Sluit de pc aan op een stopcontact.",   // NL
        L"Подключите компьютер к электрической розетке.",   // RU
        L"PC を電源コンセントに接続してください。",   // JA
        L"Podłącz komputer do gniazda zasilania.",   // PL
        L"Slut pc'en til en stikkontakt.",   // DA
        L"Anslut datorn till ett eluttag.",   // SV
        L"Koble PC-en til et strømuttak.",   // NO
        L"Kytke tietokone pistorasiaan.",   // FI
        L"PC'yi bir prize takın."   // TR
      } },
    { 441,
      {
        L"La batteria è quasi scarica.",   // IT
        L"The battery is almost drained.",   // EN
        L"La batterie est presque déchargée.",   // FR
        L"La batería está casi agotada.",   // ES
        L"Der Akku ist fast leer.",   // DE
        L"A bateria está quase vazia.",   // PT
        L"De batterij is bijna leeg.",   // NL
        L"Батарея почти разряжена.",   // RU
        L"バッテリ残量がほとんどありません。",   // JA
        L"Bateria jest prawie rozładowana.",   // PL
        L"Batteriet er næsten afladet.",   // DA
        L"Batteriet är nästan slut.",   // SV
        L"Batteriet er nesten tomt.",   // NO
        L"Akku on lähes tyhjä.",   // FI
        L"Pilin şarjı neredeyse bitti."   // TR
      } },
    { 442,
      {
        L"La batteria è quasi scarica.",   // IT
        L"The battery is almost drained.",   // EN
        L"La batterie est presque déchargée.",   // FR
        L"La batería está casi agotada.",   // ES
        L"Der Akku ist fast leer.",   // DE
        L"A bateria está quase vazia.",   // PT
        L"De batterij is bijna leeg.",   // NL
        L"Батарея почти разряжена.",   // RU
        L"バッテリ残量がほとんどありません。",   // JA
        L"Bateria jest prawie rozładowana.",   // PL
        L"Batteriet er næsten afladet.",   // DA
        L"Batteriet är nästan slut.",   // SV
        L"Batteriet er nesten tomt.",   // NO
        L"Akku on lähes tyhjä.",   // FI
        L"Pilin şarjı neredeyse bitti."   // TR
      } },
};
static const TrayString kPniduiStrings[] = {
    { 18,
      {
        L"Non connesso - Nessuna connessione disponibile",   // IT
        L"Not connected - No connections are available",   // EN
        L"Non connecté - Aucune connexion disponible",   // FR
        L"Sin conexión: no hay conexiones disponibles",   // ES
        L"Nicht verbunden - Keine Verbindungen verfügbar",   // DE
        L"Sem ligação - Não existem ligações disponíveis",   // PT
        L"Niet verbonden - Geen verbindingen beschikbaar",   // NL
        L"Не подключено — нет доступных подключений",   // RU
        L"非接続 - 利用できる接続はありません",   // JA
        L"Brak połączenia — nie są dostępne żadne połączenia",   // PL
        L"Ikke forbundet – ingen forbindelser er tilgængelige",   // DA
        L"Inte ansluten – inga anslutningar är tillgängliga",   // SV
        L"Ikke tilkoblet – ingen tilkoblinger er tilgjengelige",   // NO
        L"Ei yhteyttä – yhteyksiä ei ole saatavilla",   // FI
        L"Bağlı değil - Kullanılabilir bağlantı yok"   // TR
      } },
    { 24,
      {
        L"Accesso a Internet",   // IT
        L"Internet access",   // EN
        L"Accès Internet",   // FR
        L"Acceso a Internet",   // ES
        L"Internetzugriff",   // DE
        L"Acesso à Internet",   // PT
        L"Internettoegang",   // NL
        L"Доступ к Интернету",   // RU
        L"インターネット アクセス",   // JA
        L"Dostęp do Internetu",   // PL
        L"Internettadgang",   // DA
        L"Internetåtkomst",   // SV
        L"Internett-tilgang",   // NO
        L"Internet-yhteys",   // FI
        L"İnternet erişimi"   // TR
      } },
    { 31,
      {
        L"Stato connessione - Sconosciuto",   // IT
        L"Connection status - Unknown",   // EN
        L"État de la connexion - Inconnu",   // FR
        L"Estado de conexión: desconocido",   // ES
        L"Verbindungsstatus - Unbekannt",   // DE
        L"Estado da ligação - Desconhecido",   // PT
        L"Verbindingsstatus - Onbekend",   // NL
        L"Состояние подключения — неизвестно",   // RU
        L"接続の状態 - 不明",   // JA
        L"Stan połączenia — nieznany",   // PL
        L"Forbindelsesstatus – ukendt",   // DA
        L"Anslutningsstatus – okänd",   // SV
        L"Tilkoblingsstatus – ukjent",   // NO
        L"Yhteyden tila – tuntematon",   // FI
        L"Bağlantı durumu - Bilinmiyor"   // TR
      } },
    { 51,
      {
        L"Nessun accesso a Internet",   // IT
        L"No Internet access",   // EN
        L"Aucun accès Internet",   // FR
        L"Sin acceso a Internet",   // ES
        L"Kein Internetzugriff",   // DE
        L"Sem acesso à Internet",   // PT
        L"Geen internettoegang",   // NL
        L"Нет доступа к Интернету",   // RU
        L"インターネット アクセスなし",   // JA
        L"Brak dostępu do Internetu",   // PL
        L"Ingen internettadgang",   // DA
        L"Ingen internetåtkomst",   // SV
        L"Ingen Internett-tilgang",   // NO
        L"Ei Internet-yhteyttä",   // FI
        L"İnternet erişimi yok"   // TR
      } },
    { 54,
      {
        L"Non connesso - Sono disponibili connessioni",   // IT
        L"Not connected - Connections are available",   // EN
        L"Non connecté - Des connexions sont disponibles",   // FR
        L"Sin conexión: hay conexiones disponibles",   // ES
        L"Nicht verbunden - Verbindungen sind verfügbar",   // DE
        L"Sem ligação - Existem ligações disponíveis",   // PT
        L"Niet verbonden - Verbindingen zijn beschikbaar",   // NL
        L"Не подключено — доступны подключения",   // RU
        L"非接続 - 利用可能な接続があります",   // JA
        L"Brak połączenia — są dostępne połączenia",   // PL
        L"Ikke forbundet – der er forbindelser",   // DA
        L"Inte ansluten – anslutningar finns",   // SV
        L"Ikke tilkoblet – tilkoblinger er tilgjengelige",   // NO
        L"Ei yhteyttä – yhteyksiä on saatavilla",   // FI
        L"Bağlı değil - Bağlantılar kullanılabilir"   // TR
      } },
    { 63,
      {
        L"Rete Windows",   // IT
        L"Windows Network",   // EN
        L"Réseau Windows",   // FR
        L"Red de Windows",   // ES
        L"Windows-Netzwerk",   // DE
        L"Rede Windows",   // PT
        L"Windows-netwerk",   // NL
        L"Сеть Windows",   // RU
        L"Windows ネットワーク",   // JA
        L"Sieć Windows",   // PL
        L"Windows-netværk",   // DA
        L"Windows-nätverk",   // SV
        L"Windows-nettverk",   // NO
        L"Windows-verkko",   // FI
        L"Windows Ağı"   // TR
      } },
    { 64,
      {
        L"Icona sistema di rete",   // IT
        L"Network system icon",   // EN
        L"Icône système réseau",   // FR
        L"Icono del sistema de red",   // ES
        L"Netzwerksystemsymbol",   // DE
        L"Ícone de sistema de rede",   // PT
        L"Systeempictogram netwerk",   // NL
        L"Системный значок сети",   // RU
        L"ネットワーク システム アイコン",   // JA
        L"Ikona systemowa sieci",   // PL
        L"Netværkssystemikon",   // DA
        L"Systemikon för nätverk",   // SV
        L"Systemikon for nettverk",   // NO
        L"Verkon järjestelmäkuvake",   // FI
        L"Ağ sistem simgesi"   // TR
      } },
    { 76,
      {
        L"Non connesso - Modalità aereo",   // IT
        L"Not connected - Airplane mode",   // EN
        L"Non connecté - Mode avion",   // FR
        L"Sin conexión: modo avión",   // ES
        L"Nicht verbunden - Flugzeugmodus",   // DE
        L"Sem ligação - Modo de avião",   // PT
        L"Niet verbonden - Vliegtuigmodus",   // NL
        L"Не подключено — режим полёта",   // RU
        L"非接続 - 機内モード",   // JA
        L"Brak połączenia — tryb samolotowy",   // PL
        L"Ikke forbundet – flytilstand",   // DA
        L"Inte ansluten – flygplansläge",   // SV
        L"Ikke tilkoblet – flymodus",   // NO
        L"Ei yhteyttä – lentotila",   // FI
        L"Bağlı değil - Uçak modu"   // TR
      } },
    { 77,
      {
        L"Modalità aereo",   // IT
        L"Airplane mode",   // EN
        L"Mode avion",   // FR
        L"Modo avión",   // ES
        L"Flugzeugmodus",   // DE
        L"Modo de avião",   // PT
        L"Vliegtuigmodus",   // NL
        L"Режим полёта",   // RU
        L"機内モード",   // JA
        L"Tryb samolotowy",   // PL
        L"Flytilstand",   // DA
        L"Flygplansläge",   // SV
        L"Flymodus",   // NO
        L"Lentotila",   // FI
        L"Uçak modu"   // TR
      } },
    { 3027,
      {
        L"Icona sistema di rete",   // IT
        L"Network system icon",   // EN
        L"Icône système réseau",   // FR
        L"Icono del sistema de red",   // ES
        L"Netzwerksymbol",   // DE
        L"Ícone de sistema de rede",   // PT
        L"Systeempictogram netwerk",   // NL
        L"Системный значок сети",   // RU
        L"ネットワーク システム アイコン",   // JA
        L"Ikona systemowa sieci",   // PL
        L"Netværkssystemikon",   // DA
        L"Systemikon för nätverk",   // SV
        L"Systemikon for nettverk",   // NO
        L"Verkon järjestelmäkuvake",   // FI
        L"Ağ sistem simgesi"   // TR
      } },
};


// The language actually used by the table, for the log only.
static const wchar_t* TrayLangName() {
    static const wchar_t* const names[LANG_COUNT] = {
        L"it", L"en", L"fr", L"es", L"de", L"pt", L"nl", L"ru", L"ja", L"pl",
        L"da", L"sv", L"no", L"fi", L"tr"};
    const UiLangId lang = DetectUiLang();
    return names[(lang > LANG_IT && lang < LANG_COUNT) ? lang : LANG_EN];
}

// The order is the one fixed for the module: the Windows 10 text of the store first
// (LoadStringW_Hook calls the original function before this table), then the table in
// the language of the interface, then English.
static const wchar_t* LookupTrayString(const TrayString* table, size_t count, UINT id) {
    for (size_t i = 0; i < count; i++) {
        if (table[i].id != id) continue;
        static UiLangId chosen = LANG_COUNT;      // LANG_COUNT = not looked up yet
        if (chosen == LANG_COUNT) chosen = DetectUiLang();
        const wchar_t* text = table[i].text[chosen];
        if (text && text[0]) return text;
        return table[i].text[LANG_EN];
    }
    return nullptr;
}

// Is the module one of the two redirected to the data folder? It is compared
// with the handle loaded in this process (the redirection makes it the Windows 10
// copy): no path comparisons, which the path spoof would falsify.
static const wchar_t* RedirectedTrayModule(HINSTANCE hInst) {
    if (!hInst) return nullptr;
    if (hInst == GetModuleHandleW(L"stobject.dll")) return L"stobject.dll";
    if (hInst == GetModuleHandleW(L"pnidui.dll")) return L"pnidui.dll";
    return nullptr;
}

typedef int(WINAPI* LoadStringW_t)(HINSTANCE, UINT, LPWSTR, int);
static LoadStringW_t LoadStringW_Original = nullptr;
static int g_trayTextLogs = 0;
static int g_trayTextMisses = 0;

static int WINAPI LoadStringW_Hook(HINSTANCE hInst, UINT id, LPWSTR buffer, int cch) {
    int result = LoadStringW_Original(hInst, id, buffer, cch);
    if (result > 0 || !buffer || cch <= 0) return result;   // it is there: leave it alone
    try {
        const wchar_t* moduleName = RedirectedTrayModule(hInst);
        if (!moduleName) return result;
        const wchar_t* text = (moduleName[0] == L's')
                                  ? LookupTrayString(kStobjectStrings, _countof(kStobjectStrings), id)
                                  : LookupTrayString(kPniduiStrings, _countof(kPniduiStrings), id);
        // Diagnostics: an id asked by a tray module and missing from the
        // Windows 10 table as well. In the 0.3.3 log no "string served" lines
        // appeared exactly because the ids involved were not in the table.
        if (!text) {
            if (g_trayTextMisses < 8) {
                g_trayTextMisses++;
                Wh_Log(L"[tray] string %u requested by %s: not in the Windows 10 table", id, moduleName);
            }
            return result;
        }
        int length = (int)wcslen(text);
        if (length >= cch) length = cch - 1;
        memcpy(buffer, text, (size_t)length * sizeof(wchar_t));
        buffer[length] = 0;
        if (g_trayTextLogs < 6) {
            g_trayTextLogs++;
            Wh_Log(L"[tray] string %u served from the module table (%s, language %s: the .mui is not in the data folder)",
                   id, moduleName, TrayLangName());
        }
        return length;
    } catch (...) {
    }
    return result;
}

static void InstallTrayStrings() {
    if (Wh_SetFunctionHook((void*)LoadStringW, (void*)LoadStringW_Hook, (void**)&LoadStringW_Original))
        Wh_Log(L"[tray] tray module strings: Windows 10 MUI ready in place of the missing .mui "
               L"(%d stobject + %d pnidui strings in 15 languages, current language %s)",
               (int)_countof(kStobjectStrings), (int)_countof(kPniduiStrings), TrayLangName());
    else
        Wh_Log(L"[tray] tray module strings not served (hook not installed)");
}

// --- 8f-bis) where the battery right-click menu comes from -------------------
// Again from the disassembly: the battery menu is NOT a resource. stobject
// builds CreatePopupMenu + InsertMenuItemW/AppendMenuW and takes the entries from
// the registry:
//   HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Applets\SysTray\BattMeter\ContextMenu
// (numeric subkeys containing "ItemName"), while the flyout is defined in
//   ...\BattMeter\Flyout   (UseWin32BatteryFlyout = 0/1).
// If those keys do not exist (or are empty) the menu stays without entries: that is
// the first thing to rule out, and it was not in the log. This check writes into
// the log what really is on the machine.
static void LogBatteryMenuSource() {
    const wchar_t* paths[] = {
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\SysTray\\BattMeter\\ContextMenu",
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\SysTray\\BattMeter\\Flyout",
    };
    for (const wchar_t* path : paths) {
        ScopedRegKey key;
        LONG rc = RegOpenKeyExW(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, key.put());
        if (rc != ERROR_SUCCESS) {
            Wh_Log(L"[battery] key missing (error %ld): %s", rc, path);
            continue;
        }

        DWORD subkeys = 0, values = 0;
        RegQueryInfoKeyW(key.get(), nullptr, nullptr, nullptr, &subkeys, nullptr, nullptr,
                         &values, nullptr, nullptr, nullptr, nullptr);
        Wh_Log(L"[battery] %s: %lu subkeys, %lu values", path, subkeys, values);

        for (DWORD i = 0; i < subkeys && i < 8; i++) {
            wchar_t name[128] = {};
            DWORD length = _countof(name);
            if (RegEnumKeyExW(key.get(), i, name, &length, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
                break;
            ScopedRegKey sub;
            if (RegOpenKeyExW(key.get(), name, 0, KEY_READ, sub.put()) == ERROR_SUCCESS) {
                wchar_t item[256] = {};
                DWORD bytes = sizeof(item), type = 0;
                if (RegQueryValueExW(sub.get(), L"ItemName", nullptr, &type, (LPBYTE)item, &bytes) == ERROR_SUCCESS)
                    Wh_Log(L"[battery]   entry %s: \"%s\"", name, item);
                else
                    Wh_Log(L"[battery]   entry %s: (no ItemName)", name);
            }
        }

        // 1.3.3: this value is not read here either. It is the switch of the Windows 7 era
        // Win32 flyout of stobject.dll, not of the Windows 10 flyout this mod opens through
        // the shell experience manager.
    }
}

// --- 8g) the right-click menu: who really builds it --------------------------
// It separates the two cases the log did not tell apart for the battery
// icon:
//   a) the click does not reach stobject -> NO TrackPopupMenu line appears;
//   b) the menu is shown but is empty (missing data: registry key or
//      MUI strings) -> the line with "0 items" appears.
// Both hooks are pass-through (they always call the original) and the log is capped.
typedef BOOL(WINAPI* TrackPopupMenuEx_t)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
static TrackPopupMenuEx_t TrackPopupMenuEx_Original = nullptr;
typedef BOOL(WINAPI* TrackPopupMenu_t)(HMENU, UINT, int, int, int, HWND, const RECT*);
static TrackPopupMenu_t TrackPopupMenu_Original = nullptr;
static int g_tpmLogs = 0;

// Defined further below (WinRT diagnostics section): only the signature is needed here.
static void LogCallerModule(void* caller, wchar_t* buf, size_t count);

// The real caller is not WhReturnAddress(): that one points inside our own
// module (at the hook trampoline). The stack is walked and the first
// frame that does not belong to the mod is taken (the 0.3.3 log always wrote
// "local@win10-taskbar-clean...dll" in that column).
static const wchar_t* RealCallerModule(wchar_t* buf, size_t count) {
    buf[0] = 0;
    void* frames[8] = {};
    const USHORT total = CaptureStackBackTrace(0, (DWORD)_countof(frames), frames, nullptr);
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&RealCallerModule, &self);
    for (USHORT i = 0; i < total; i++) {
        HMODULE m = nullptr;
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                (LPCWSTR)frames[i], &m))
            continue;
        if (m == self) continue;
        LogCallerModule(frames[i], buf, count);
        break;
    }
    return buf;
}

// Fallback for the battery menu. The native path is the one of the MUI
// strings (ids 150/151 served above); this covers the case where the
// request for the strings arrives another way and the menu stays empty: the
// entries are the same stobject's code adds by itself (commands 101
// and 102, which stobject can handle), so the menu does not stay empty.
static int g_batteryFillLogs = 0;

static bool FillEmptyBatteryMenu(HMENU menu, HWND owner) {
    if (!menu || !owner) return false;
    if (GetMenuItemCount(menu) != 0) return false;
    const wchar_t* ownerModule = TrayOwnerModuleOfWindow(owner);
    if (!ownerModule || _wcsicmp(ownerModule, L"stobject.dll") != 0) return false;

    // Safety net: if the WM_CONTEXTMENU interception did not fire (subclass
    // not installed yet, different build path) we still populate the menu
    // with the two entries in the user's language.
    const UiLangId lang = DetectUiLang();
    AppendMenuW(menu, MF_STRING, 101, kBatteryPowerOptions[lang]);
    AppendMenuW(menu, MF_STRING, 102, kBatteryMobilityCenter[lang]);
    if (g_batteryFillLogs < 3) {
        g_batteryFillLogs++;
        Wh_Log(L"[battery] empty context menu: added two multilang entries (\"%s\", \"%s\")",
               kBatteryPowerOptions[lang], kBatteryMobilityCenter[lang]);
    }
    return true;
}

static void LogPopupMenuCall(const wchar_t* api, HMENU menu, HWND owner) {
    if (g_tpmLogs >= 3) return;
    const wchar_t* ownerModule = owner ? TrayOwnerModuleOfWindow(owner) : nullptr;
    const int count = menu ? GetMenuItemCount(menu) : -1;
    // The menus of the tray modules are logged and, in any case, the empty ones:
    // those explain "the right click does nothing".
    if (!ownerModule && count != 0) return;

    g_tpmLogs++;
    wchar_t cls[64] = {};
    if (owner) GetClassNameW(owner, cls, _countof(cls));
    wchar_t who[64] = {};
    RealCallerModule(who, _countof(who));
    Wh_Log(L"[tray] %s: %d items, caller %s, window %s (%s)", api, count,
           who[0] ? who : L"unknown module", cls[0] ? cls : L"no class",
           ownerModule ? ownerModule : L"not a tray window");
}

// The battery menu stays the Windows 10 one: only the power
// entries (commands 101 and 102 of stobject). The classic window-arrangement
// entries are only in the taskbar menu.
static BOOL WINAPI TrackPopupMenuEx_Hook(HMENU menu, UINT flags, int x, int y, HWND owner,
                                         LPTPMPARAMS params) {
    try {
        FillEmptyBatteryMenu(menu, owner);
        LogPopupMenuCall(L"TrackPopupMenuEx", menu, owner);

    } catch (...) {
    }
    const BOOL scelta = TrackPopupMenuEx_Original(menu, flags, x, y, owner, params);
    if (scelta && HandleClassicMenuCommand((UINT)(UINT_PTR)scelta)) return FALSE;
    return scelta;
}

static BOOL WINAPI TrackPopupMenu_Hook(HMENU menu, UINT flags, int x, int y, int reserved,
                                       HWND owner, const RECT* rect) {
    try {
        FillEmptyBatteryMenu(menu, owner);
        LogPopupMenuCall(L"TrackPopupMenu", menu, owner);

    } catch (...) {
    }
    const BOOL scelta = TrackPopupMenu_Original(menu, flags, x, y, reserved, owner, rect);
    if (scelta && HandleClassicMenuCommand((UINT)(UINT_PTR)scelta)) return FALSE;
    return scelta;
}

static bool g_trackPopupMenuExHooked = false;
static bool g_trackPopupMenuHooked = false;

static void InstallPopupMenuTrace() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) return;
    void* ex = (void*)GetProcAddress(user32, "TrackPopupMenuEx");
    void* plain = (void*)GetProcAddress(user32, "TrackPopupMenu");
    if (!g_trackPopupMenuExHooked && ex)
        g_trackPopupMenuExHooked = Wh_SetFunctionHook(ex, (void*)TrackPopupMenuEx_Hook,
                                                       (void**)&TrackPopupMenuEx_Original);
    if (!g_trackPopupMenuHooked && plain)
        g_trackPopupMenuHooked = Wh_SetFunctionHook(plain, (void*)TrackPopupMenu_Hook,
                                                     (void**)&TrackPopupMenu_Original);
    if (g_trackPopupMenuExHooked || g_trackPopupMenuHooked)
        Wh_Log(L"[tray] popup menu supervision active (TrackPopupMenuEx %s, TrackPopupMenu %s)",
               g_trackPopupMenuExHooked ? L"yes" : L"no",
               g_trackPopupMenuHooked ? L"yes" : L"no");
    else
        Wh_Log(L"[tray] popup menu supervision not installed");
}

// ===========================================================================
// LEGACY TRAY: network and volume icons in the Windows 10 shell
//
// 1.0.0's direct OLECMDID_NEW attempt did not make the icon appear, and 1.0.0's
// static URI-click shim was withdrawn. Candidate 1.0.0 instead asks the native
// Shell Service Object path to start PNI and only creates a dynamic NLM icon if
// a real PNIHiddenWnd callback has been observed. Shell_NotifyIconW tracing stays
// diagnostic; no independent click target is fabricated.
// ===========================================================================

static const IID kIidClassFactory = {0x00000001, 0x0000, 0x0000,
                                     {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

static bool TrayRedirectTarget(const wchar_t* moduleName, std::wstring& target) {
    if (!moduleName || !*moduleName) return false;

    const wchar_t* base = moduleName;
    for (const wchar_t* p = moduleName; *p; p++)
        if (*p == L'\\' || *p == L'/') base = p + 1;

    const bool wanted = _wcsicmp(base, L"pnidui.dll") == 0 || _wcsicmp(base, L"stobject.dll") == 0;
    if (!wanted) return false;

    target = std::wstring(g_cfg.storePath) + L"\\" + base;
    if (_wcsicmp(moduleName, target.c_str()) == 0) return false;   // it is already ours
    return GetFileAttributesW(target.c_str()) != INVALID_FILE_ATTRIBUTES;
}

typedef HMODULE(WINAPI* LoadLibraryW_t)(LPCWSTR);
static LoadLibraryW_t LoadLibraryW_Original = nullptr;

typedef HMODULE(WINAPI* LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

static HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR name) {
    try {
        std::wstring target;
        if (TrayRedirectTarget(name, target)) {
            HMODULE module = LoadLibraryW_Original(target.c_str());
            if (module) {
                Wh_Log(L"[tray] %s -> %s", name, target.c_str());
                return module;
            }
        }
    } catch (...) {
    }
    return LoadLibraryW_Original(name);
}

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    try {
        std::wstring target;
        if (TrayRedirectTarget(name, target)) {
            HMODULE module = LoadLibraryExW_Original(target.c_str(), nullptr, flags);
            if (module) {
                Wh_Log(L"[tray] %s -> %s", name, target.c_str());
                return module;
            }
        }
    } catch (...) {
    }
    return LoadLibraryExW_Original(name, file, flags);
}

// ntdll: it catches loads that do not go through the kernelbase functions.
typedef LONG(NTAPI* LdrLoadDll_t)(PWSTR, PULONG, UNICODE_STRING*, HMODULE*);
static LdrLoadDll_t LdrLoadDll_Original = nullptr;
typedef VOID(NTAPI* RtlInitUnicodeString_t)(UNICODE_STRING*, PCWSTR);
static RtlInitUnicodeString_t RtlInitUnicodeString_Original = nullptr;

static LONG NTAPI LdrLoadDll_Hook(PWSTR searchPath, PULONG flags, UNICODE_STRING* name, HMODULE* handle) {
    try {
        if (name && name->Buffer) {
            std::wstring asked(name->Buffer, name->Length / sizeof(wchar_t));
            std::wstring target;
            if (TrayRedirectTarget(asked.c_str(), target)) {
                UNICODE_STRING redirected = {};
                RtlInitUnicodeString_Original(&redirected, target.c_str());
                LONG status = LdrLoadDll_Original(searchPath, flags, &redirected, handle);
                if (status >= 0) {
                    Wh_Log(L"[tray] (ntdll) %s -> %s", asked.c_str(), target.c_str());
                    return status;
                }
            }
        }
    } catch (...) {
    }
    return LdrLoadDll_Original(searchPath, flags, name, handle);
}

typedef HRESULT(STDAPICALLTYPE* DllGetClassObject_t)(REFCLSID, REFIID, void**);
typedef HRESULT(WINAPI* CoCreateInstance_t)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
static CoCreateInstance_t CoCreateInstance_Original = nullptr;

static volatile LONG g_shimProbes = 0;
static HMODULE FindPrivateTrayModule(const wchar_t* moduleName) noexcept;

static bool GetLegacyClassFactory(REFCLSID clsid, winrt::com_ptr<IClassFactory>& factory) noexcept {
    try {
        const wchar_t* modules[] = { L"pnidui.dll", L"stobject.dll" };
        for (const wchar_t* name : modules) {
            HMODULE module = FindPrivateTrayModule(name);
            if (!module) continue;
            auto getClassObject = (DllGetClassObject_t)GetProcAddress(module, "DllGetClassObject");
            if (!getClassObject) continue;

            winrt::com_ptr<IClassFactory> candidate;
            const HRESULT hr = getClassObject(clsid, kIidClassFactory, candidate.put_void());
            if (SUCCEEDED(hr) && candidate.get()) {
                factory = std::move(candidate);
                return true;
            }
        }
    } catch (...) {
        Wh_Log(L"[tray] legacy class-factory lookup exception");
    }
    return false;
}

static bool IsPrivateExplorerProcess();

static const CLSID kNetworkTraySsoClsid = {
    0xC2796011, 0x81BA, 0x4148, { 0x8F, 0xCA, 0xC6, 0x64, 0x32, 0x45, 0x11, 0x3F }
};
static std::atomic<bool> g_networkSsoActivationSeen{false};
static std::atomic<ULONGLONG> g_networkSsoActivationObservedAt{0};

static bool GetPniduiClassFactory(REFCLSID clsid,
                                  winrt::com_ptr<IClassFactory>& factory) noexcept {
    try {
        HMODULE module = FindPrivateTrayModule(L"pnidui.dll");
        if (!module) return false;
        auto getClassObject = (DllGetClassObject_t)GetProcAddress(module, "DllGetClassObject");
        if (!getClassObject) return false;
        winrt::com_ptr<IClassFactory> candidate;
        const HRESULT hr = getClassObject(clsid, kIidClassFactory, candidate.put_void());
        if (FAILED(hr) || !candidate.get()) return false;
        factory = std::move(candidate);
        return true;
    } catch (...) {
        Wh_Log(L"[tray] pnidui class-factory lookup exception");
        return false;
    }
}

static void DescribeModuleAtAddress(const void* address, wchar_t* out, size_t cch) noexcept {
    if (!out || cch == 0) return;
    out[0] = L'\0';
    try {
        HMODULE module = nullptr;
        const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
        if (!address || !GetModuleHandleExW(flags, (LPCWSTR)address, &module) || !module) {
            wcscpy_s(out, cch, L"sconosciuto");
            return;
        }
        wchar_t path[MAX_PATH] = {};
        if (!GetModuleFileNameW(module, path, _countof(path))) {
            wcscpy_s(out, cch, L"module-without-path");
            return;
        }
        const wchar_t* base = wcsrchr(path, L'\\');
        base = base ? base + 1 : path;
        wcsncpy_s(out, cch, base, _TRUNCATE);
    } catch (...) {
        wcscpy_s(out, cch, L"sconosciuto");
    }
}

static HRESULT WINAPI CoCreateInstance_Hook(REFCLSID clsid, LPUNKNOWN outer, DWORD context,
                                            REFIID iid, LPVOID* ppv) {
    try {
        if (NativeUi::BlockXamlAdapter(clsid)) {
            if (!ppv) return E_POINTER;
            *ppv = nullptr;
            static std::atomic<bool> reported{false};
            if (!reported.exchange(true))
                Wh_Log(L"[ribbon] requesting native Win10 ribbon via m417z classicRibbonUI COM fallback");
            return REGDB_E_CLASSNOTREG;
        }
    } catch (...) { Wh_Log(L"[ribbon] COM gate exception; using original activation"); }

    const bool networkSso = IsEqualCLSID(clsid, kNetworkTraySsoClsid) != FALSE;
    if (networkSso && NativeUi::privateExplorer && g_cfg.provideTrayDlls) {
        if (!g_networkSsoActivationSeen.exchange(true))
            g_networkSsoActivationObservedAt.store(GetTickCount64());
        wchar_t caller[128] = {};
        DescribeModuleAtAddress(WhReturnAddress(), caller, _countof(caller));
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        try {
            winrt::com_ptr<IClassFactory> factory;
            if (GetPniduiClassFactory(clsid, factory)) {
                const HRESULT created = factory->CreateInstance(outer, iid, ppv);
                if (SUCCEEDED(created)) {
                    Wh_Log(L"[tray] Network Tray SSO routed to the pinned pnidui.dll factory: 0x%08X (caller %s)",
                           (unsigned)created, caller);
                    return created;
                }
                if (*ppv) {
                    ((IUnknown*)*ppv)->Release();
                    *ppv = nullptr;
                }
                Wh_Log(L"[tray] pinned pnidui.dll factory failed for Network Tray SSO: 0x%08X (caller %s); trying COM registration",
                       (unsigned)created, caller);
            } else {
                Wh_Log(L"[tray] Network Tray SSO requested by %s, but pnidui.dll has no usable class factory; trying COM registration",
                       caller);
            }
        } catch (...) {
            Wh_Log(L"[tray] Network Tray SSO pnidui activation raised a C++ exception (caller %s)", caller);
            if (*ppv) {
                ((IUnknown*)*ppv)->Release();
                *ppv = nullptr;
            }
        }
    }

    HRESULT hr;
    try {
        hr = CoCreateInstance_Original(clsid, outer, context, iid, ppv);
    } catch (...) {
        Wh_Log(L"[tray/ribbon] original COM activation raised a C++ exception");
        return E_FAIL;  // Never activate twice after an exception.
    }
    if (networkSso) {
        if (NativeUi::privateExplorer && g_cfg.provideTrayDlls) {
            wchar_t caller[128] = {};
            DescribeModuleAtAddress(WhReturnAddress(), caller, _countof(caller));
            Wh_Log(L"[tray] Network Tray SSO original COM activation -> 0x%08X (caller %s)",
                   (unsigned)hr, caller);
        }
        return hr;
    }

    // The legacy DLL fallback must NEVER leak into native Explorer windows.
    if (!NativeUi::privateExplorer ||
        (hr != REGDB_E_CLASSNOTREG && hr != HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND))) return hr;
    if (InterlockedIncrement(&g_shimProbes) > 64) return hr;
    try {
        winrt::com_ptr<IClassFactory> factory;
        if (!GetLegacyClassFactory(clsid, factory)) return hr;
        HRESULT created = factory->CreateInstance(outer, iid, ppv);
        if (SUCCEEDED(created)) {
            wchar_t guid[64] = {};
            StringFromGUID2(clsid, guid, _countof(guid));
            Wh_Log(L"[tray] unregistered class created with the Windows 10 DLLs: %s", guid);
            return created;
        }
    } catch (...) { Wh_Log(L"[tray] legacy class factory C++ exception"); }
    return hr;
}
static bool EnsureCoCreateHook() noexcept {
    try {
        if (CoCreateInstance_Original) return true;
        return Wh_SetFunctionHook((void*)CoCreateInstance, (void*)CoCreateInstance_Hook,
                                  (void**)&CoCreateInstance_Original);
    } catch (...) { Wh_Log(L"[ribbon] COM hook installation exception"); return false; }
}

// The modules that draw the two icons of the restored tray: pnidui.dll is the
// network icon, stobject.dll the volume/battery one. They are loaded by this mod
// into the Windows 10 shell, so if they are not loaded the icons do not exist.
// Load pnidui before stobject so the exact Windows 10 SSO factory is available
// if stobject requests the network class during its own initialization.
static const wchar_t* const kTrayIconModules[] = { L"pnidui.dll", L"stobject.dll" };
static bool g_trayWrongPathLoadAttempted[_countof(kTrayIconModules)] = {};

static std::wstring ExpectedPrivateTrayModulePath(const wchar_t* moduleName) {
    std::wstring path = g_cfg.storePath;
    while (!path.empty() && (path.back() == L'\\' || path.back() == L'/'))
        path.pop_back();
    path += L"\\";
    path += moduleName;
    return path;
}

static bool GetTrayModulePath(HMODULE module, wchar_t* path, size_t cch) noexcept {
    if (!path || cch == 0) return false;
    path[0] = L'\0';
    if (!module || cch > MAXDWORD) return false;
    try {
        const DWORD length = GetModuleFileNameW(module, path, (DWORD)cch);
        if (!length || length >= cch) {
            path[0] = L'\0';
            return false;
        }
        return true;
    } catch (...) {
        path[0] = L'\0';
        return false;
    }
}

static bool IsPrivateTrayModulePath(HMODULE module, const wchar_t* moduleName) noexcept {
    try {
        if (!moduleName || !g_cfg.storePath[0]) return false;
        const std::wstring expected = ExpectedPrivateTrayModulePath(moduleName);
        wchar_t actual[MAX_PATH] = {};
        return GetTrayModulePath(module, actual, _countof(actual)) &&
               _wcsicmp(actual, expected.c_str()) == 0;
    } catch (...) {
        return false;
    }
}

// Find the exact pinned module, even if another DLL with the same basename was
// loaded earlier. The snapshot handle is RAII-owned; returned HMODULE is borrowed.
static HMODULE FindPrivateTrayModule(const wchar_t* moduleName) noexcept {
    try {
        if (!moduleName || !*moduleName || !g_cfg.storePath[0]) return nullptr;
        if (HMODULE byName = GetModuleHandleW(moduleName)) {
            if (IsPrivateTrayModulePath(byName, moduleName)) return byName;
        }

        HANDLE rawSnapshot = INVALID_HANDLE_VALUE;
        for (int attempt = 0; attempt < 3; ++attempt) {
            rawSnapshot = CreateToolhelp32Snapshot(
                TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
            if (rawSnapshot != INVALID_HANDLE_VALUE || GetLastError() != ERROR_BAD_LENGTH)
                break;
        }
        ScopedHandle snapshot(rawSnapshot);
        if (!snapshot.valid()) return nullptr;

        MODULEENTRY32W entry = {};
        entry.dwSize = sizeof(entry);
        if (!Module32FirstW(snapshot.get(), &entry)) return nullptr;
        do {
            if (_wcsicmp(entry.szModule, moduleName) == 0 &&
                IsPrivateTrayModulePath(entry.hModule, moduleName))
                return entry.hModule;
        } while (Module32NextW(snapshot.get(), &entry));
    } catch (...) {
    }
    return nullptr;
}

static bool VerifyPinnedTrayFile(const wchar_t* moduleName) noexcept {
    try {
        if (!moduleName) return false;
        const ExtraFile* pin = nullptr;
        for (const ExtraFile& candidate : kTrayFiles) {
            if (_wcsicmp(candidate.name, moduleName) == 0) {
                pin = &candidate;
                break;
            }
        }
        if (!pin) {
            Wh_Log(L"[tray] no SHA-256 pin is configured for %s", moduleName);
            return false;
        }
        const std::wstring path = ExpectedPrivateTrayModulePath(moduleName);
        std::wstring actualHash;
        if (!Sha256File(path.c_str(), actualHash)) return false;
        if (_wcsicmp(actualHash.c_str(), pin->sha256) != 0) {
            Wh_Log(L"[tray] refusing SSO activation: %s hash mismatch (expected %s, got %s)",
                   moduleName, pin->sha256, actualHash.c_str());
            return false;
        }
        return true;
    } catch (...) {
        Wh_Log(L"[tray] exception verifying the pinned hash for %s", moduleName);
        return false;
    }
}

static bool AnyTrayIconModuleMissing() {
    for (const wchar_t* name : kTrayIconModules)
        if (!FindPrivateTrayModule(name)) return true;
    return false;
}

// What the data folder really contains. Without this line "the icon does not
// appear" has no visible cause in the log.
static void ReportTrayStoreFile(const wchar_t* name) {
    std::wstring full = std::wstring(g_cfg.storePath) + L"\\" + name;
    WIN32_FILE_ATTRIBUTE_DATA fad = {};
    if (GetFileAttributesExW(full.c_str(), GetFileExInfoStandard, &fad)) {
        Wh_Log(L"[tray] data folder: %s present (%lu bytes)", name,
               (unsigned long)fad.nFileSizeLow);
    } else {
        Wh_Log(L"[tray] data folder: %s MISSING (%lu): the related icon cannot be "
               L"created in this session", name, GetLastError());
    }
}

static void LoadTrayModules(const wchar_t* why) {
    try {
        for (size_t i = 0; i < _countof(kTrayIconModules); ++i) {
            const wchar_t* name = kTrayIconModules[i];
            if (FindPrivateTrayModule(name)) continue;

            HMODULE existing = GetModuleHandleW(name);
            if (existing && g_trayWrongPathLoadAttempted[i]) continue;

            const std::wstring full = ExpectedPrivateTrayModulePath(name);
            if (existing) {
                wchar_t existingPath[MAX_PATH] = {};
                if (GetTrayModulePath(existing, existingPath, _countof(existingPath)) &&
                    _wcsicmp(existingPath, full.c_str()) != 0) {
                    Wh_Log(L"[tray] %s already mapped from %s; requesting the pinned private copy",
                           name, existingPath);
                }
            }

            HMODULE module = LoadLibraryW_Original ? LoadLibraryW_Original(full.c_str())
                                                   : LoadLibraryW(full.c_str());
            if (module && IsPrivateTrayModulePath(module, name)) {
                Wh_Log(L"[tray] %s loaded from the private pinned path (%s)", name, why);
            } else if (module) {
                wchar_t actualPath[MAX_PATH] = {};
                GetTrayModulePath(module, actualPath, _countof(actualPath));
                g_trayWrongPathLoadAttempted[i] = true;
                Wh_Log(L"[tray] %s request did not yield the private file (mapped %s); native SSO will wait",
                       name, actualPath[0] ? actualPath : L"path unavailable");
            } else {
                Wh_Log(L"[tray] %s not loaded (%lu)", name, GetLastError());
            }
        }
    } catch (...) {
        Wh_Log(L"[tray] exception while loading the private pinned tray modules");
    }
}

static ULONGLONG g_nextNetworkSsoEnableTry = 0;
static ULONGLONG g_nextNetworkPniWindowScan = 0;
static ULONGLONG g_nextNetworkSsoModuleScan = 0;
static ULONGLONG g_networkSsoEnableAcceptedAt = 0;
static HMODULE g_privatePniduiForSso = nullptr;
static HMODULE g_privateStobjectForSso = nullptr;
static unsigned g_networkSsoEnableAttempts = 0;
static bool g_networkSsoModulesWaitLogged = false;
static bool g_networkSsoModulesReadyLogged = false;
static bool g_networkSsoPinsChecked = false;
static bool g_networkSsoPinsRejected = false;
static bool g_networkSsoEnableAccepted = false;
static bool g_networkSsoEnableUnavailableLogged = false;
static bool g_networkSsoEnableGiveUpLogged = false;
static bool g_networkSsoWaitLogged = false;

// Shell32's legacy SHEnableServiceObject export asks the current taskbar to
// enable an SSO. Do not treat a same-named System32 DLL as the pinned module:
// the log showed the private stobject redirect arriving after the old S_OK.
// The current candidate pins stobject 10.0.19041.7664 (SHA-256 in kTrayFiles); the
// Windows-To-Go-slot rewrite used by the separate 26100 project is not applied:
// this exact file has no Windows-To-Go sentinel and already contains the Network
// Tray SSO CLSID. We only enable it after both exact private DLLs are mapped.
typedef HRESULT (WINAPI* SHEnableServiceObject_t)(REFCLSID, BOOL);
static void EnsureNativeNetworkTraySso() noexcept {
    try {
        if (!NativeUi::privateExplorer || !g_cfg.provideTrayDlls || g_unloading.load()) return;

        const ULONGLONG now = GetTickCount64();
        if (!g_privatePniduiForSso || !g_privateStobjectForSso ||
            now >= g_nextNetworkSsoModuleScan) {
            g_nextNetworkSsoModuleScan = now + 1000;
            g_privatePniduiForSso = FindPrivateTrayModule(L"pnidui.dll");
            g_privateStobjectForSso = FindPrivateTrayModule(L"stobject.dll");
        }
        if (!g_privatePniduiForSso || !g_privateStobjectForSso) {
            if (!g_networkSsoModulesWaitLogged) {
                g_networkSsoModulesWaitLogged = true;
                HMODULE anyPnidui = GetModuleHandleW(L"pnidui.dll");
                HMODULE anyStobject = GetModuleHandleW(L"stobject.dll");
                wchar_t pniPath[MAX_PATH] = {};
                wchar_t stPath[MAX_PATH] = {};
                if (anyPnidui) GetTrayModulePath(anyPnidui, pniPath, _countof(pniPath));
                if (anyStobject) GetTrayModulePath(anyStobject, stPath, _countof(stPath));
                Wh_Log(L"[tray] SSO bootstrap waiting for the pinned private tray DLLs (pnidui=%s, stobject=%s)",
                       pniPath[0] ? pniPath : L"not loaded",
                       stPath[0] ? stPath : L"not loaded");
            }
            return;
        }
        if (!g_networkSsoModulesReadyLogged) {
            g_networkSsoModulesReadyLogged = true;
            wchar_t pniPath[MAX_PATH] = {};
            wchar_t stPath[MAX_PATH] = {};
            GetTrayModulePath(g_privatePniduiForSso, pniPath, _countof(pniPath));
            GetTrayModulePath(g_privateStobjectForSso, stPath, _countof(stPath));
            Wh_Log(L"[tray] SSO bootstrap verified private DLL paths: pnidui=%s; stobject=%s",
                   pniPath, stPath);
        }
        if (!g_networkSsoPinsChecked) {
            g_networkSsoPinsChecked = true;
            const bool pniPinned = VerifyPinnedTrayFile(L"pnidui.dll");
            const bool stPinned = VerifyPinnedTrayFile(L"stobject.dll");
            g_networkSsoPinsRejected = !pniPinned || !stPinned;
            if (!g_networkSsoPinsRejected)
                Wh_Log(L"[tray] private pnidui.dll and stobject.dll match their pinned SHA-256 values");
        }
        if (g_networkSsoPinsRejected) return;

        NetworkPniRegistration registration = {};
        // 1.0.0: l'icona forzata non deve fermare il tentativo nativo: la SSO
        // continua a essere richiesta, cosi' la PNI vera puo' ancora prendere il
        // sopravvento.
        if (CopyNetworkPniRegistration(&registration) && !registration.forced) return;
        if (now >= g_nextNetworkPniWindowScan) {
            g_nextNetworkPniWindowScan = now + 2000;
            if (HWND pni = FindPniHiddenWindow()) {
                static bool reported = false;
                if (!reported) {
                    reported = true;
                    Wh_Log(L"[tray] PNIHiddenWnd found at 0x%p, but no qualifying PNI callback registration has been captured", pni);
                }
            }
        }

        const bool activationSeen = g_networkSsoActivationSeen.load();
        const ULONGLONG activationObservedAt = g_networkSsoActivationObservedAt.load();
        if (activationSeen && activationObservedAt && now - activationObservedAt < 15000) return;
        if (g_networkSsoEnableAccepted) {
            if (!g_networkSsoWaitLogged && g_networkSsoEnableAcceptedAt &&
                now - g_networkSsoEnableAcceptedAt >= 15000) {
                g_networkSsoWaitLogged = true;
                Wh_Log(L"[tray] SHEnableServiceObject on the pinned private DLLs returned S_OK, but no PNIHiddenWnd/NIM_ADD followed after 15 s; no URI substitute is used. The mod escalation ladder registers the icon in place of the native PNI");
            }
            return;
        }
        if (g_networkSsoEnableAttempts >= 6) {
            if (!g_networkSsoEnableGiveUpLogged) {
                g_networkSsoEnableGiveUpLogged = true;
                Wh_Log(L"[tray] native Network Tray SSO enable did not succeed after 6 attempts; no URI click substitute will be used");
            }
            return;
        }
        if (now < g_nextNetworkSsoEnableTry) return;
        g_nextNetworkSsoEnableTry = now + 5000;
        ++g_networkSsoEnableAttempts;

        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        auto enable = shell32 ? (SHEnableServiceObject_t)GetProcAddress(shell32, "SHEnableServiceObject") : nullptr;
        if (!enable) {
            if (!g_networkSsoEnableUnavailableLogged) {
                g_networkSsoEnableUnavailableLogged = true;
                Wh_Log(L"[tray] shell32 does not export SHEnableServiceObject; native SSO bootstrap unavailable");
            }
            g_networkSsoEnableAttempts = 6;
            g_networkSsoEnableGiveUpLogged = true;
            return;
        }

        const HRESULT hr = enable(kNetworkTraySsoClsid, TRUE);
        Wh_Log(L"[tray] SHEnableServiceObject(Network Tray SSO) attempt %u -> 0x%08X",
               g_networkSsoEnableAttempts, (unsigned)hr);
        if (SUCCEEDED(hr)) {
            g_networkSsoEnableAccepted = true;
            g_networkSsoEnableAcceptedAt = now;
        }
    } catch (...) {
        Wh_Log(L"[tray] exception while enabling the native Network Tray SSO");
    }
}

// 1.3.1: the hook part of the tray support, split out so that it is registered in
// Wh_ModInit, where the Windhawk API wants every hook to be set - the engine applies the
// queue automatically right after Wh_ModInit returns. Everything that needs a running shell
// stays in InstallTraySupport below, on the mod's own thread. The registrations carry their
// own once-only guard, so calling the hook part from either place is harmless: Wh_ModInit
// gets there first.
static void InstallTraySupportHooks() {
    static bool done = false;
    if (done) return;
    done = true;
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (kernelBase) {
        void* target = (void*)GetProcAddress(kernelBase, "LoadLibraryW");
        if (target) Wh_SetFunctionHook(target, (void*)LoadLibraryW_Hook, (void**)&LoadLibraryW_Original);
        target = (void*)GetProcAddress(kernelBase, "LoadLibraryExW");
        if (target) Wh_SetFunctionHook(target, (void*)LoadLibraryExW_Hook, (void**)&LoadLibraryExW_Original);
    }

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll) {
        LdrLoadDll_Original = (LdrLoadDll_t)GetProcAddress(ntdll, "LdrLoadDll");
        RtlInitUnicodeString_Original = (RtlInitUnicodeString_t)GetProcAddress(ntdll, "RtlInitUnicodeString");
        if (LdrLoadDll_Original && RtlInitUnicodeString_Original)
            Wh_SetFunctionHook((void*)LdrLoadDll_Original, (void*)LdrLoadDll_Hook, (void**)&LdrLoadDll_Original);
    }

    if (EnsureCoCreateHook())
        Wh_Log(L"[tray] network icon support active");
    else
        Wh_Log(L"[tray] tray support not installed");

    // The strings (and therefore the menus) of the tray modules come from their .mui,
    // which is not in the data folder: the Windows 10 one is served instead.
    InstallTrayStrings();
    InstallPopupMenuTrace();

    // Observe icon registrations before loading the private DLLs, so the log
    // records whether pnidui actually submits a native NIM_ADD in this process.
    InstallNetworkIconTrace();
}

static void InstallTraySupport() {
    g_traySupportInstalled = true;
    InstallTraySupportHooks();   // already registered in Wh_ModInit: a no-op here

    LogBatteryMenuSource();

    for (const wchar_t* name : kTrayIconModules) ReportTrayStoreFile(name);

    // The DLLs are loaded at once: the tray creates the icons at shell
    // start-up, so they must be ready before, not after.
    LoadTrayModules(L"at shell start");
    if (AnyTrayIconModuleMissing()) {
        Wh_Log(L"[tray] a tray module is missing: the network / volume icon will not be "
               L"created in this session unless the file appears. If the data folder is "
               L"empty, run the mod once from the Windows 11 shell with internet access: "
               L"the file is downloaded and verified there");
    }

}

// The icons are created at shell start-up: if a module was not in the data folder
// yet (the folder is filled by the native shell, which can be working at the same
// time), giving up would leave the icon missing for the whole session. The load is
// retried for a minute and every attempt is written to the log.
static void RetryTrayModulesIfNeeded() {
    if (!g_traySupportInstalled || g_unloading) return;
    static ULONGLONG nextTry = 0;
    static ULONGLONG giveUpAt = 0;
    const ULONGLONG now = GetTickCount64();

    if (!AnyTrayIconModuleMissing()) {
        if (giveUpAt) {
            Wh_Log(L"[tray] tray modules available again: the network / volume icons "
                   L"can be created now");
        }
        nextTry = 0;
        giveUpAt = 0;
        return;
    }
    if (!giveUpAt) giveUpAt = now + 60000;
    if (now > giveUpAt || now < nextTry) return;
    nextTry = now + 10000;
    LoadTrayModules(L"recupero differito");
}

// ===========================================================================
// ACTION CENTRE: WHAT THIS WINDOWS STILL HAS (diagnostics only)
// ===========================================================================
// The Windows 10 shell opens its action centre by asking ShellExperienceHost for
// it. On Windows 11 24H2 ShellExperienceHost no longer carries that code path,
// so nothing appears - and here the Windows 11 shell is not running at all,
// because the shell is the Windows 10 one. Before choosing a way to restore it,
// the mod writes down what exists. Three probes, three kinds of evidence:
//   A) processes: if asking for the action centre starts a XAML host, it is named;
//   B) windows: if a panel window appears anywhere, its class is named;
//   C) the notification platform: the per-user database is copied and read with
//      the winsqlite3.dll that ships with Windows, so the count and the last
//      notifications are known (nothing is written, the live file is only read).
// Every line carries the sender process, so the log says who did what.

static const wchar_t* const kAcHostProcesses[] = {
    L"ShellExperienceHost.exe",
    L"ShellHost.exe",
};
static const int kAcHostProcessCount = (int)(sizeof(kAcHostProcesses) / sizeof(kAcHostProcesses[0]));

// Window classes that mean "a shell panel was created": the XAML hosts and the
// names the panels have used across builds.
static const wchar_t* const kAcWindowMarkers[] = {
    L"corewindow", L"actioncenter", L"controlcenter", L"notification",
    L"shellexperience", L"immersive",
};
static const int kAcWindowMarkerCount = (int)(sizeof(kAcWindowMarkers) / sizeof(kAcWindowMarkers[0]));

// Defined with the indicator measurement further below, used here to name the
// module a window class procedure comes from.
static void ModuleOfAddress(const void* addr, wchar_t* buf, size_t count);

static DWORD g_acProbeUntil = 0;
static int g_acProbeLogs = 0;
static bool g_acDbProbed = false;
static wchar_t g_acWindowsSeen[8][64] = {};
static int g_acWindowsSeenCount = 0;

// A library loaded just for this probe: RAII, so it is freed on every path.
class ScopedLibrary {
public:
    explicit ScopedLibrary(HMODULE m = nullptr) : m_m(m) {}
    ~ScopedLibrary() { if (m_m) FreeLibrary(m_m); }
    ScopedLibrary(const ScopedLibrary&) = delete;
    ScopedLibrary& operator=(const ScopedLibrary&) = delete;
    HMODULE get() const { return m_m; }
private:
    HMODULE m_m;
};

class ScopedDeleteFile {
public:
    explicit ScopedDeleteFile(const wchar_t* path) noexcept {
        if (path) wcsncpy_s(m_path, _countof(m_path), path, _TRUNCATE);
    }
    ~ScopedDeleteFile() noexcept {
        if (!m_path[0]) return;
        if (!DeleteFileW(m_path)) {
            const DWORD error = GetLastError();
            if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND)
                Wh_Log(L"[actioncenter] temporary notification-probe file cleanup failed (%lu): %s",
                       error, m_path);
        }
    }
    ScopedDeleteFile(const ScopedDeleteFile&) = delete;
    ScopedDeleteFile& operator=(const ScopedDeleteFile&) = delete;
private:
    wchar_t m_path[MAX_PATH]{};
};

// Copies a file that another process keeps open (the notification database is
// open in the service): widest sharing on the source, plain create on the copy.
static bool CopyFileWideShare(const wchar_t* src, const wchar_t* dst) {
    if (!src || !dst) return false;
    ScopedHandle in(CreateFileW(src, GENERIC_READ,
                                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!in.valid()) return false;
    ScopedHandle out(CreateFileW(dst, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!out.valid()) return false;
    char buf[64 * 1024];
    DWORD read = 0;
    while (ReadFile(in.get(), buf, sizeof(buf), &read, nullptr) && read) {
        DWORD written = 0;
        if (!WriteFile(out.get(), buf, read, &written, nullptr) || written != read) return false;
    }
    return true;
}

// --- A) and B): processes and windows --------------------------------------
static void AcLogProcessProbe() {
    ScopedHandle snap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
    if (!snap.valid()) return;
    PROCESSENTRY32W pe = {};
    pe.dwSize = sizeof(pe);
    if (!Process32FirstW(snap.get(), &pe)) return;
    do {
        for (int i = 0; i < kAcHostProcessCount && g_acProbeLogs < 20; i++) {
            if (_wcsicmp(pe.szExeFile, kAcHostProcesses[i]) != 0) continue;
            static bool logged[4] = {};
            if (logged[i]) continue;
            logged[i] = true;
            g_acProbeLogs++;
            Wh_Log(L"[actioncenter] XAML host running: %s (pid %lu)", pe.szExeFile, pe.th32ProcessID);
        }
    } while (Process32NextW(snap.get(), &pe));
}

static BOOL CALLBACK AcWindowProbeProc(HWND hwnd, LPARAM) {
    try {
        if (!IsWindowVisible(hwnd)) return TRUE;
        wchar_t cls[128] = {};
        if (!GetClassNameW(hwnd, cls, _countof(cls)) || !cls[0]) return TRUE;
        wchar_t low[128] = {};
        for (int i = 0; i < 127 && cls[i]; i++) low[i] = (wchar_t)towlower(cls[i]);
        bool matches = false;
        for (int i = 0; i < kAcWindowMarkerCount; i++)
            if (wcsstr(low, kAcWindowMarkers[i])) matches = true;
        if (!matches) return TRUE;
        for (int i = 0; i < g_acWindowsSeenCount; i++)
            if (_wcsicmp(g_acWindowsSeen[i], cls) == 0) return TRUE;
        if (g_acWindowsSeenCount >= (int)(sizeof(g_acWindowsSeen) / sizeof(g_acWindowsSeen[0]))) return TRUE;
        wcsncpy_s(g_acWindowsSeen[g_acWindowsSeenCount], 64, cls, _TRUNCATE);
        g_acWindowsSeenCount++;
        // The sender: which process owns this window - the answer to "who showed it".
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        wchar_t mod[MAX_PATH] = {};
        ModuleOfAddress((const void*)GetClassLongPtrW(hwnd, GCLP_WNDPROC), mod, _countof(mod));
        const wchar_t* modName = wcsrchr(mod, L'\\');
        Wh_Log(L"[actioncenter] window appears: %s 0x%p (pid %lu, %s)",
               cls, (void*)hwnd, pid, modName ? modName + 1 : L"unknown module");
    } catch (...) {
        static std::atomic<bool> reported{false};
        if (!reported.exchange(true))
            Wh_Log(L"[actioncenter] window probe callback raised an exception");
    }
    return TRUE;
}

// --- C) the notification platform -----------------------------------------
static void ProbeNotificationDatabase() {
    wchar_t srcDir[MAX_PATH] = {};
    if (!GetEnvironmentVariableW(L"LOCALAPPDATA", srcDir, _countof(srcDir))) {
        Wh_Log(L"[actioncenter] LOCALAPPDATA not readable: notification database probe skipped");
        return;
    }
    wchar_t src[MAX_PATH] = {};
    swprintf_s(src, L"%s\\Microsoft\\Windows\\Notifications\\wpndatabase.db", srcDir);
    const DWORD attrs = GetFileAttributesW(src);
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"[actioncenter] no notification database on this machine: %s", src);
        return;
    }
    WIN32_FILE_ATTRIBUTE_DATA fad = {};
    if (GetFileAttributesExW(src, GetFileExInfoStandard, &fad) &&
        (fad.nFileSizeHigh > 0 || fad.nFileSizeLow > 32u * 1024u * 1024u)) {
        Wh_Log(L"[actioncenter] notification database larger than 32 MB: probe skipped");
        return;
    }

    // A copy is read, never the live file, and the copies are removed at the end.
    wchar_t copy[MAX_PATH] = {};
    swprintf_s(copy, L"%s\\wpndb-probe.db", g_cfg.storePath);
    ScopedDeleteFile copyCleanup(copy);
    if (!CopyFileWideShare(src, copy)) {
        Wh_Log(L"[actioncenter] the notification database is locked and could not be copied");
        return;
    }
    wchar_t srcWal[MAX_PATH] = {}, copyWal[MAX_PATH] = {};
    wcscpy_s(srcWal, src); wcscat_s(srcWal, L"-wal");
    wcscpy_s(copyWal, copy); wcscat_s(copyWal, L"-wal");
    ScopedDeleteFile walCleanup(copyWal);
    const bool haveWal = GetFileAttributesW(srcWal) != INVALID_FILE_ATTRIBUTES &&
                         CopyFileWideShare(srcWal, copyWal);
    wchar_t srcShm[MAX_PATH] = {}, copyShm[MAX_PATH] = {};
    wcscpy_s(srcShm, src); wcscat_s(srcShm, L"-shm");
    wcscpy_s(copyShm, copy); wcscat_s(copyShm, L"-shm");
    ScopedDeleteFile shmCleanup(copyShm);
    const bool haveShm = GetFileAttributesW(srcShm) != INVALID_FILE_ATTRIBUTES &&
                         CopyFileWideShare(srcShm, copyShm);

    ScopedLibrary sqlLib(LoadLibraryExW(L"winsqlite3.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
    if (!sqlLib.get()) {
        Wh_Log(L"[actioncenter] winsqlite3.dll not available: probe skipped");
        return;
    }
    typedef int (*open_v2_t)(const char*, void**, int, const char*);
    typedef int (*prepare_t)(void*, const char*, int, void**, const char**);
    typedef int (*step_t)(void*);
    typedef const unsigned char* (*col_text_t)(void*, int);
    typedef int (*col_int_t)(void*, int);
    typedef int (*finalize_t)(void*);
    typedef int (*close_t)(void*);
    typedef const char* (*errmsg_t)(void*);
    struct ScopedSqliteDb {
        void* handle = nullptr;
        close_t close = nullptr;
        ~ScopedSqliteDb() { closeNow(); }
        void closeNow() noexcept {
            if (!handle || !close) return;
            const int result = close(handle);
            if (result != 0) {
                Wh_Log(L"[actioncenter] SQLite close returned %d during probe cleanup", result);
                return;  // Keep ownership so the destructor can retry once.
            }
            handle = nullptr;
        }
    };
    struct ScopedSqliteStatement {
        void* handle = nullptr;
        finalize_t finalize = nullptr;
        ~ScopedSqliteStatement() noexcept {
            if (handle && finalize) {
                const int result = finalize(handle);
                if (result != 0)
                    Wh_Log(L"[actioncenter] SQLite statement finalization returned %d", result);
            }
        }
    };
    auto pOpen = (open_v2_t)GetProcAddress(sqlLib.get(), "sqlite3_open_v2");
    auto pPrepare = (prepare_t)GetProcAddress(sqlLib.get(), "sqlite3_prepare_v2");
    auto pStep = (step_t)GetProcAddress(sqlLib.get(), "sqlite3_step");
    auto pText = (col_text_t)GetProcAddress(sqlLib.get(), "sqlite3_column_text");
    auto pInt = (col_int_t)GetProcAddress(sqlLib.get(), "sqlite3_column_int");
    auto pFinal = (finalize_t)GetProcAddress(sqlLib.get(), "sqlite3_finalize");
    auto pClose = (close_t)GetProcAddress(sqlLib.get(), "sqlite3_close");
    auto pErrmsg = (errmsg_t)GetProcAddress(sqlLib.get(), "sqlite3_errmsg");
    if (!pOpen || !pPrepare || !pStep || !pText || !pFinal || !pClose) {
        Wh_Log(L"[actioncenter] winsqlite3.dll has no usable entry points: probe skipped");
        return;
    }

    ScopedSqliteDb db{};
    db.close = pClose;
    if (pOpen((const char*)copy, &db.handle, 0x00000002 /*READWRITE*/ | 0x00000040 /*URI*/, nullptr) != 0 || !db.handle) {
        wchar_t wideErr[200] = L"no error text";
        if (db.handle && pErrmsg) {
            const char* msg = pErrmsg(db.handle);
            if (msg) MultiByteToWideChar(CP_UTF8, 0, msg, -1, wideErr, _countof(wideErr));
        }
        Wh_Log(L"[actioncenter] the notification database copy did not open (%s)", wideErr);
        db.closeNow();
        return;
    }

    Wh_Log(L"[actioncenter] notification database copy ready (%s%s)",
           haveWal ? L"with WAL" : L"without WAL", haveShm ? L", with SHM" : L"");

    // What tables are there: the schema is the answer to "can this be read at all".
    {
        wchar_t tables[240] = {};
        ScopedSqliteStatement st{};
        st.finalize = pFinal;
        if (pPrepare(db.handle, "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name", -1, &st.handle, nullptr) == 0) {
            size_t used = 0;
            int rows = 0;
            while (pStep(st.handle) == 100 && rows < 14) {
                const unsigned char* name = pText(st.handle, 0);
                if (name) {
                    wchar_t wname[64] = {};
                    MultiByteToWideChar(CP_UTF8, 0, (const char*)name, -1, wname, _countof(wname));
                    if (used + wcslen(wname) + 2 < _countof(tables)) {
                        if (used) { tables[used++] = L','; tables[used++] = L' '; }
                        wcsncpy_s(tables + used, _countof(tables) - used, wname, _TRUNCATE);
                        used = wcslen(tables);
                    }
                }
                rows++;
            }
            Wh_Log(L"[actioncenter] notification database tables (%d): %s", rows, tables);
        } else {
            Wh_Log(L"[actioncenter] the table list could not be read");
        }
    }

    // Count only. Notification payloads can contain private user content and
    // are deliberately never copied into the Windhawk log.
    {
        ScopedSqliteStatement st{};
        st.finalize = pFinal;
        if (pPrepare(db.handle, "SELECT COUNT(*) FROM Notification", -1, &st.handle, nullptr) == 0) {
            if (pStep(st.handle) == 100 && pInt)
                Wh_Log(L"[actioncenter] notifications in the database: %d", pInt(st.handle, 0));
        } else {
            Wh_Log(L"[actioncenter] the Notification table is not readable");
        }
    }

    db.closeNow();
    Wh_Log(L"[actioncenter] probe finished; RAII is removing temporary copies");
}

// Runs for the first three minutes of the shell: that is when the user tries the
// action centre. After that the probe is over and costs nothing.
static void ActionCenterProbeTick() {
    try {
        const DWORD now = GetTickCount();
        if (!g_acProbeUntil) g_acProbeUntil = now + 180000;
        if (now > g_acProbeUntil) return;
        static DWORD last = 0;
        if (last && now - last < 2000) return;
        const bool first = (last == 0);
        last = now;

        AcLogProcessProbe();
        EnumWindows(AcWindowProbeProc, 0);

        // The database probe waits for the notification service to have written
        // something and runs once: 12 seconds after the shell started.
        if (g_verboseDiagnostics.load(std::memory_order_relaxed) &&
            !g_acDbProbed && !first &&
            (now - (g_acProbeUntil - 180000)) > 12000) {
            g_acDbProbed = true;
            ProbeNotificationDatabase();
        }
    } catch (...) {
        Wh_Log(L"[actioncenter] exception in the probe");
    }
}

static void** FindIatSlot(HMODULE module, const char* dllHint, const char* funcHint) {
#ifdef _WIN64
    if (!module || !dllHint || !funcHint) return nullptr;
    BYTE* base = (BYTE*)module;
    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return nullptr;
    DWORD rva = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if (!rva) return nullptr;

    size_t hintLen = strlen(dllHint);
    for (PIMAGE_IMPORT_DESCRIPTOR imp = (PIMAGE_IMPORT_DESCRIPTOR)(base + rva); imp->Name; imp++) {
        const char* dll = (const char*)(base + imp->Name);
        bool dllOk = false;
        for (const char* p = dll; *p && !dllOk; p++)
            dllOk = _strnicmp(p, dllHint, hintLen) == 0;
        if (!dllOk) continue;

        PIMAGE_THUNK_DATA oft = (PIMAGE_THUNK_DATA)(base + (imp->OriginalFirstThunk ? imp->OriginalFirstThunk
                                                                                    : imp->FirstThunk));
        PIMAGE_THUNK_DATA ft = (PIMAGE_THUNK_DATA)(base + imp->FirstThunk);
        for (; oft->u1.AddressOfData; oft++, ft++) {
            if (oft->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;
            const char* name = (const char*)(base + oft->u1.AddressOfData + sizeof(WORD));
            if (_stricmp(name, funcHint) == 0) return (void**)&ft->u1.Function;
        }
    }
#else
    (void)module; (void)dllHint; (void)funcHint;
#endif
    return nullptr;
}


// The image name of the module that made a call: the columns of the log that name a
// caller use it.
static void LogCallerModule(void* caller, wchar_t* buf, size_t count) {
    buf[0] = 0;
    if (!caller) return;
    HMODULE mod = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            (LPCWSTR)caller, &mod) || !mod)
        return;
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(mod, path, _countof(path));
    const wchar_t* base = wcsrchr(path, L'\\');
    wcsncpy_s(buf, count, base ? base + 1 : path, _TRUNCATE);
}


// and only once every 12 seconds.
static void ModuleOfAddress(const void* addr, wchar_t* buf, size_t count) {
    if (!buf || count == 0) return;
    buf[0] = 0;
    HMODULE m = nullptr;
    if (addr && GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   (LPCWSTR)addr, &m) && m)
        GetModuleFileNameW(m, buf, (DWORD)count);
}


// 1.3.9: OpenSettingsPage and ToggleWifiRadio are gone. Both were unused, and the first one
// launched ms-settings: pages through the original ShellExecuteW, i.e. around the hooks.

namespace NetworkTrayForce {

static constexpr wchar_t kExplorerKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer";
static constexpr wchar_t kTrayNotifyKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\TrayNotify";
static constexpr wchar_t kOwnerClass[] = L"Win10Restorer_NetworkIconWnd";
static constexpr UINT kCallbackMessage = WM_APP + 0x51;
static constexpr UINT kIconId = 1;
static constexpr UINT kMsgQuitOwner = WM_APP + 0x52;
static constexpr int kVerifyIntervalMs = 5000;
static constexpr int kMaxRetryLog = 6;

static std::atomic<bool> g_stopping{false};
static std::atomic<bool> g_reinstallRequested{false};
static HWND g_ownerWindow = nullptr;
static HANDLE g_ownerThread = nullptr;
static DWORD g_ownerThreadId = 0;
static UINT g_taskbarCreatedMessage = 0;
static std::mutex g_ownerLock;
static bool g_guaranteesApplied = false;
static bool g_forcedPublished = false;
static bool g_retiredLogged = false;
static bool g_verifiedLogged = false;
static bool g_ownerRunning = false;
static bool g_taskbarWasUp = false;
static ULONGLONG g_firstTick = 0;
static ULONGLONG g_nextVerify = 0;
static ULONGLONG g_nextGuaranteeTry = 0;
static int g_verifyFailures = 0;
static int g_publishCount = 0;

// Stato del ripristino del pulsante di overflow e del flyout autentico.
// Le tre costanti seguenti servono esclusivamente a migrare un backup creato da
// una versione precedente; questa versione non ne crea uno nuovo.
static constexpr wchar_t kTrayForceStateKey[] = L"Software\\Win10Shell\\TrayForce";
static constexpr wchar_t kAutoTrayBackupValue[] = L"EnableAutoTrayOriginal";
static constexpr DWORD kAutoTrayAbsent = 0xFFFFFFFF;
static std::atomic<bool> g_autoTrayRestored{false};
// All'avvio la mod non tocca la barra. L'override virtuale e l'eventuale reset
// di TrayNotify scattano solo se, dopo l'attesa, l'icona forzata non e' comparsa:
// "menu overflow all'avvio" invariato.
static std::atomic<bool> g_escalate{false};
static ULONGLONG g_verifiedSince = 0;
static int g_restoreAttempts = 0;
static int g_restoreLogNotes = 0;
static int g_clickLogNotes = 0;
static ULONGLONG g_lastClickTick = 0;

static int ClampValue(int value, int low, int high) noexcept {
    return value < low ? low : (value > high ? high : value);
}

// ------------------------------------------------------------- registro ----
static void NudgeTray() noexcept {
    try {
        DWORD_PTR ignored = 0;
        SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                            reinterpret_cast<LPARAM>(L"TraySettings"),
                            SMTO_ABORTIFHUNG | SMTO_NORMAL, 400, &ignored);
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray)
            SendMessageTimeoutW(tray, WM_SETTINGCHANGE, 0,
                                reinterpret_cast<LPARAM>(L"TraySettings"),
                                SMTO_ABORTIFHUNG | SMTO_NORMAL, 400, &ignored);
    } catch (...) {
    }
}

// ----------------------------------------------- ripristino del chevron ----
// EnableAutoTray=0 viene ora servito solo in memoria. Il valore fisico resta
// intatto, quindi togliendo la risposta torna il comportamento normale dell'overflow.
static bool PublishForcedRegistration() noexcept;   // definita piu' sotto

// 1.0.0: solo lettura. Un backup lasciato da una versione precedente viene
// riconosciuto e registrato nel log, ma la mod non scrive piu' nel registro
// dell'utente, nemmeno per rimediare a se stessa: se il valore memorizzato non
// serve piu', l'utente cancella la chiave indicata nel log.
static void MigrateLegacyAutoTrayStateOnce() noexcept {
    static std::atomic<bool> attempted{false};
    if (attempted.exchange(true, std::memory_order_acq_rel)) return;
    try {

        ScopedHKey legacy;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, kTrayForceStateKey, 0,
                          KEY_QUERY_VALUE | KEY_SET_VALUE,
                          legacy.receive()) != ERROR_SUCCESS || !legacy.valid()) {
            return;
        }
        DWORD stored = 0;
        DWORD type = 0;
        DWORD size = sizeof(stored);
        if (RegQueryValueExW(legacy.get(), kAutoTrayBackupValue, nullptr, &type,
                             reinterpret_cast<BYTE*>(&stored), &size) != ERROR_SUCCESS ||
            type != REG_DWORD || size != sizeof(stored)) {
            return;
        }

        // Nessuna scrittura: il valore reale di Explorer resta quello che e'.
        Wh_Log(L"[tray-force] legacy EnableAutoTray backup found (%s in a former build): "
               L"the real Explorer value is left untouched. If the key is no longer "
               L"needed, delete HKCU\\Software\\Win10Shell\\TrayForce",
               stored == kAutoTrayAbsent ? L"no value" : L"a saved value");
    } catch (...) {
        Wh_Log(L"[tray-force] exception while migrating legacy EnableAutoTray state");
    }
}

static bool RestoreOverflowChevron() noexcept {
    try {
        ++g_restoreAttempts;
        g_autoTrayVirtual.store(false, std::memory_order_release);
        NudgeTray();
        Wh_Log(L"[tray-force] virtual EnableAutoTray=0 removed (attempt %d): the overflow button \"^\" is available again", g_restoreAttempts);
        return true;
    } catch (...) {
        Wh_Log(L"[tray-force] exception while removing the virtual EnableAutoTray override");
        return false;
    }
}

// L'icona e' sparita dopo il ripristino: si torna all'override virtuale.
// Il pulsante di overflow resta nascosto, ma l'icona di rete ha la precedenza
// (e' la richiesta principale); il log lo dice chiaramente.
static void ReApplyAllIconsMode() noexcept {
    try {
        g_verifiedSince = 0;
        g_autoTrayRestored.store(false);
        g_autoTrayVirtual.store(true, std::memory_order_release);
        NudgeTray();
        PublishForcedRegistration();
        if (g_restoreLogNotes++ < 3)
            Wh_Log(L"[tray-force] with the real value the icon does not stay visible: "
                   L"virtual \"all icons visible\" is active in the private shell "
                   L"(the overflow button stays hidden while the icon is in forced mode)");
    } catch (...) {
    }
}

// Copia di sicurezza dello stato binario della barra di notifica e sua
// cancellazione, una sola volta. Senza backup non si cancella nulla.
static bool BackupAndResetTrayValuesOnce() noexcept {
    try {
        const wchar_t* kValues[] = { L"IconStreams", L"PastIconsStream" };
        ScopedHKey key;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, kTrayNotifyKey, 0, KEY_READ | KEY_SET_VALUE,
                          key.receive()) != ERROR_SUCCESS ||
            !key.valid())
            return true;   // nulla da azzerare

        wchar_t marker[MAX_PATH] = {};
        _snwprintf_s(marker, _countof(marker), _TRUNCATE, L"%s\\tray-state-reset.done",
                     g_cfg.storePath);
        if (GetFileAttributesW(marker) != INVALID_FILE_ATTRIBUTES) return true;

        // Solo per il percorso di backup: dump grezzo dei valori
        std::vector<BYTE> dump;
        bool anyValue = false;
        for (const wchar_t* value : kValues) {
            DWORD type = 0, size = 0;
            if (RegQueryValueExW(key.get(), value, nullptr, &type, nullptr, &size) != ERROR_SUCCESS ||
                type != REG_BINARY || size == 0)
                continue;
            std::vector<BYTE> data(size);
            if (RegQueryValueExW(key.get(), value, nullptr, &type, data.data(), &size) != ERROR_SUCCESS)
                continue;
            const size_t nameChars = wcslen(value) + 1;
            const BYTE* name = reinterpret_cast<const BYTE*>(value);
            dump.insert(dump.end(), name, name + nameChars * sizeof(wchar_t));
            dump.insert(dump.end(), reinterpret_cast<const BYTE*>(&size),
                        reinterpret_cast<const BYTE*>(&size) + sizeof(size));
            dump.insert(dump.end(), data.begin(), data.begin() + size);
            anyValue = true;
        }
        if (!anyValue) {
            // Nessuno stato da salvare: si segna comunque il marker.
            ScopedHandle markerFile(CreateFileW(marker, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                                                FILE_ATTRIBUTE_NORMAL, nullptr));
            return markerFile.valid();
        }

        wchar_t backup[MAX_PATH] = {};
        _snwprintf_s(backup, _countof(backup), _TRUNCATE, L"%s\\tray-state-backup.bin",
                     g_cfg.storePath);
        {
            ScopedHandle file(CreateFileW(backup, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                                          FILE_ATTRIBUTE_NORMAL, nullptr));
            if (!file.valid()) {
                Wh_Log(L"[tray-force] tray state backup not writable (%lu): "
                       L"no clearing", GetLastError());
                return false;
            }
            DWORD written = 0;
            if (!WriteFile(file.get(), dump.data(), static_cast<DWORD>(dump.size()),
                           &written, nullptr) || written != dump.size()) {
                Wh_Log(L"[tray-force] incomplete backup: no clearing");
                return false;
            }
        }
        Wh_Log(L"[tray-force] tray state saved to %s (%lu bytes)", backup,
               static_cast<unsigned long>(dump.size()));

        int removed = 0;
        for (const wchar_t* value : kValues) {
            if (RegDeleteValueW(key.get(), value) == ERROR_SUCCESS) ++removed;
        }
        if (removed == 0) {
            Wh_Log(L"[tray-force] tray state cannot be cleared: the shell will regenerate it "
                   L"or the icon stays hidden by the user's choice");
            return false;
        }
        ScopedHandle markerFile(CreateFileW(marker, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                                            FILE_ATTRIBUTE_NORMAL, nullptr));
        Wh_Log(L"[tray-force] tray state cleared (%d values): the Windows 10 shell "
               L"rebuilds the default state, with the system icons visible", removed);
        return true;
    } catch (...) {
        Wh_Log(L"[tray-force] exception while resetting the tray state");
        return false;
    }
}

static bool ApplyVisibilityGuarantees() noexcept {
    try {
        if (g_guaranteesApplied) return true;

        const DWORD now = GetTickCount();
        if (g_nextGuaranteeTry && now < g_nextGuaranteeTry) return false;

        bool ok = true;

        // EnableAutoTray e' solo virtuale. Si arma esattamente dove il vecchio
        // codice avrebbe scritto 0 al posto di un valore reale diverso o assente.
        if (g_escalate.load(std::memory_order_acquire)) {
            if (!g_autoTrayRestored.load() &&
                !g_autoTrayVirtual.load(std::memory_order_acquire)) {
                DWORD enableAutoTray = 1;
                if (!ReadRegDwordHkcu(kExplorerKey, L"EnableAutoTray", &enableAutoTray) ||
                    enableAutoTray != 0) {
                    g_autoTrayVirtual.store(true, std::memory_order_release);
                    Wh_Log(L"[tray-force] escalation: virtual EnableAutoTray=0 to make the icon appear "
                           L"(the real registry value is unchanged)");
                }
            }

            // Stato binario della barra (con backup) - una sola volta, in escalation.
            if (g_cfg.forceNetworkTrayResetTraySettings)
                BackupAndResetTrayValuesOnce();
        }

        NudgeTray();

        g_guaranteesApplied = ok;
        if (!ok) {
            g_nextGuaranteeTry = now + 30000;
            Wh_Log(L"[tray-force] some visibility guarantees were not applied: "
                   L"new attempt in 30 s");
        } else {
            Wh_Log(L"[tray-force] visibility guarantees applied (virtual all-icons mode, "
                   L"optional tray state reset)");
        }
        return ok;
    } catch (...) {
        Wh_Log(L"[tray-force] exception in the visibility guarantees");
        return false;
    }
}

// --------------------------------------------------- finestra proprietaria --
// ------------------------------------------------- flyout autentico Win10 ---
// Windows 10 apre il riquadro di rete chiedendo alla factory della shell
// l'esperienza "Windows.Internal.ShellExperience.NetworkFlyout" e invocandone
// ShowFlyout: e' esattamente il percorso che usa pnidui.dll (la stringa e' nella
// sua tabella). Qui si replica quel percorso, senza URI sostitutivi:
//   CoCreateInstance(CLSID_ImmersiveShell, IID_IServiceProvider)
//   -> QueryService(CLSID_ShellExperienceManagerFactory)
//   -> GetExperienceManager("...NetworkFlyout")
//   -> QueryInterface(IID_NetworkFlyoutExperienceManager) -> ShowFlyout(rect)
static const GUID kClsidImmersiveShell =
    { 0xc2f03a33, 0x21f5, 0x47fa, { 0xb4, 0xbb, 0x15, 0x63, 0x62, 0xa2, 0xf2, 0x39 } };
static const GUID kIidServiceProvider =
    { 0x6d5140c1, 0x7436, 0x11ce, { 0x80, 0x34, 0x00, 0xaa, 0x00, 0x60, 0x09, 0xfa } };
static const GUID kClsidShellExperienceManagerFactory =
    { 0x2e8fcb18, 0xa0ee, 0x41ad, { 0x8e, 0xf8, 0x77, 0xfb, 0x3a, 0x37, 0x0c, 0xa5 } };
static const GUID kIidNetworkFlyoutExperienceManager =
    { 0xc9ddc674, 0xb44b, 0x4c67, { 0x9d, 0x79, 0x2b, 0x23, 0x7d, 0x9b, 0xe0, 0x5a } };

struct Win10FlyoutRect { float x, y, width, height; };

struct ServiceProviderVtbl {
    HRESULT (STDMETHODCALLTYPE* QueryInterface)(void*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE* AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE* Release)(void*);
    HRESULT (STDMETHODCALLTYPE* QueryService)(void*, REFGUID, REFIID, void**);
};

struct ExperienceManagerFactoryVtbl {
    HRESULT (STDMETHODCALLTYPE* QueryInterface)(void*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE* AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE* Release)(void*);
    HRESULT (STDMETHODCALLTYPE* GetIids)(void*, ULONG*, IID**);
    HRESULT (STDMETHODCALLTYPE* GetRuntimeClassName)(void*, void**);
    HRESULT (STDMETHODCALLTYPE* GetTrustLevel)(void*, int*);
    HRESULT (STDMETHODCALLTYPE* GetExperienceManager)(void*, void*, void**);
};

struct ExperienceManagerVtbl {
    HRESULT (STDMETHODCALLTYPE* QueryInterface)(void*, REFIID, void**);
    ULONG   (STDMETHODCALLTYPE* AddRef)(void*);
    ULONG   (STDMETHODCALLTYPE* Release)(void*);
    HRESULT (STDMETHODCALLTYPE* GetIids)(void*, ULONG*, IID**);
    HRESULT (STDMETHODCALLTYPE* GetRuntimeClassName)(void*, void**);
    HRESULT (STDMETHODCALLTYPE* GetTrustLevel)(void*, int*);
    HRESULT (STDMETHODCALLTYPE* ShowFlyout)(void*, void*);
    HRESULT (STDMETHODCALLTYPE* HideFlyout)(void*);
};

class ScopedUnknownRef {
public:
    ScopedUnknownRef() = default;
    explicit ScopedUnknownRef(void* pointer) noexcept : m_pointer(pointer) {}
    ~ScopedUnknownRef() { reset(); }
    ScopedUnknownRef(const ScopedUnknownRef&) = delete;
    ScopedUnknownRef& operator=(const ScopedUnknownRef&) = delete;
    void* get() const noexcept { return m_pointer; }
    void reset(void* pointer = nullptr) noexcept {
        if (m_pointer) {
            auto vtbl = *reinterpret_cast<ServiceProviderVtbl* const*>(m_pointer);
            if (vtbl && vtbl->Release) vtbl->Release(m_pointer);
        }
        m_pointer = pointer;
    }
private:
    void* m_pointer = nullptr;
};

static bool QueryIconRect(const NetworkPniRegistration& reg, RECT* out) noexcept;   // definita sotto

class ScopedFlyoutCom {
public:
    ScopedFlyoutCom() noexcept {
        m_hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        m_uninit = SUCCEEDED(m_hr);
    }
    ~ScopedFlyoutCom() { if (m_uninit) CoUninitialize(); }
    ScopedFlyoutCom(const ScopedFlyoutCom&) = delete;
    ScopedFlyoutCom& operator=(const ScopedFlyoutCom&) = delete;
private:
    HRESULT m_hr = E_FAIL;
    bool m_uninit = false;
};

static std::atomic<unsigned int> g_networkFlyoutFailureLogs{0};

static void LogNetworkFlyoutFailure(PCWSTR stage, HRESULT hr) noexcept {
    if (g_networkFlyoutFailureLogs.fetch_add(1, std::memory_order_relaxed) < 12)
        Wh_Log(L"[tray-force] network flyout activation failed at %s (0x%08X)",
               stage, static_cast<unsigned>(hr));
}

static HRESULT InvokeWin10NetworkFlyout(const RECT* anchor) noexcept {
    try {
        ScopedFlyoutCom com;
        void* shell = nullptr;
        HRESULT hr = CoCreateInstance(kClsidImmersiveShell, nullptr,
                                      CLSCTX_NO_CODE_DOWNLOAD | CLSCTX_LOCAL_SERVER,
                                      kIidServiceProvider, &shell);
        ScopedUnknownRef shellRef(shell);
        if (FAILED(hr) || !shell) {
            LogNetworkFlyoutFailure(L"CoCreateInstance(ImmersiveShell)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        auto sp = *reinterpret_cast<ServiceProviderVtbl* const*>(shell);
        if (!sp || !sp->QueryService) {
            LogNetworkFlyoutFailure(L"IServiceProvider::QueryService (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        void* factory = nullptr;
        hr = sp->QueryService(shell, kClsidShellExperienceManagerFactory,
                              kClsidShellExperienceManagerFactory, &factory);
        ScopedUnknownRef factoryRef(factory);
        if (FAILED(hr) || !factory) {
            LogNetworkFlyoutFailure(L"QueryService(ShellExperienceManagerFactory)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        HMODULE combase = GetModuleHandleW(L"combase.dll");
        auto createString = combase ? (HRESULT (WINAPI*)(const wchar_t*, UINT32, void**))
                                          GetProcAddress(combase, "WindowsCreateString")
                                    : nullptr;
        auto deleteString = combase ? (HRESULT (WINAPI*)(void*))
                                          GetProcAddress(combase, "WindowsDeleteString")
                                    : nullptr;
        if (!createString) {
            const HRESULT missing = HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
            LogNetworkFlyoutFailure(L"WindowsCreateString lookup", missing);
            return missing;
        }

        static const wchar_t kExperience[] = L"Windows.Internal.ShellExperience.NetworkFlyout";
        void* name = nullptr;
        hr = createString(kExperience, static_cast<UINT32>(wcslen(kExperience)), &name);
        if (FAILED(hr) || !name) {
            LogNetworkFlyoutFailure(L"WindowsCreateString(NetworkFlyout)",
                                    FAILED(hr) ? hr : E_FAIL);
            return FAILED(hr) ? hr : E_FAIL;
        }

        auto factoryVtbl = *reinterpret_cast<ExperienceManagerFactoryVtbl* const*>(factory);
        HRESULT hrManager = E_NOINTERFACE;
        void* manager = nullptr;
        if (factoryVtbl && factoryVtbl->GetExperienceManager)
            hrManager = factoryVtbl->GetExperienceManager(factory, name, &manager);
        if (deleteString) deleteString(name);
        ScopedUnknownRef managerRef(manager);
        if (FAILED(hrManager) || !manager) {
            LogNetworkFlyoutFailure(L"GetExperienceManager(NetworkFlyout)",
                                    FAILED(hrManager) ? hrManager : E_NOINTERFACE);
            return FAILED(hrManager) ? hrManager : E_NOINTERFACE;
        }

        auto managerVtbl = *reinterpret_cast<ExperienceManagerVtbl* const*>(manager);
        if (!managerVtbl || !managerVtbl->QueryInterface) {
            LogNetworkFlyoutFailure(L"IExperienceManager::QueryInterface (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        void* flyout = nullptr;
        hr = managerVtbl->QueryInterface(manager, kIidNetworkFlyoutExperienceManager, &flyout);
        ScopedUnknownRef flyoutRef(flyout);
        if (FAILED(hr) || !flyout) {
            LogNetworkFlyoutFailure(L"QueryInterface(NetworkFlyoutExperienceManager)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        auto flyoutVtbl = *reinterpret_cast<ExperienceManagerVtbl* const*>(flyout);
        if (!flyoutVtbl || !flyoutVtbl->ShowFlyout) {
            LogNetworkFlyoutFailure(L"ShowFlyout (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        Win10FlyoutRect rect = {};
        if (anchor) {
            rect.x = static_cast<float>(anchor->left);
            rect.y = static_cast<float>(anchor->top);
            rect.width = static_cast<float>(anchor->right - anchor->left);
            rect.height = static_cast<float>(anchor->bottom - anchor->top);
        }
        // ShowFlyout is a private shell API. Do not call it a second time after a
        // failed HRESULT: it may already have initiated rendering, and a duplicate
        // request can close or race the flyout that is being created.
        const HRESULT showResult = flyoutVtbl->ShowFlyout(flyout, &rect);
        if (FAILED(showResult)) LogNetworkFlyoutFailure(L"IExperienceManager::ShowFlyout", showResult);
        return showResult;
    } catch (...) {
        Wh_Log(L"[tray-force] C++ exception while opening the network flyout");
        return E_FAIL;
    }
}

// ------------------------------------ menu contestuale dell'icona di rete ----
// Le due voci del menu originale di Windows 10 con le destinazioni richieste:
//   * "Risoluzione dei problemi" -> problemi di RETE (non hardware): risolutore
//     Microsoft "Internet Connections", msdt.exe /id NetworkDiagnosticsWeb;
//   * "Apri impostazioni di rete e Internet" ->
//     shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}.
// Entrambe passano dalla guardia (thread di servizio + tetto di tempo). I gestori
// del menu usano solo try/catch C++: non intercettano errori di accesso nativi.

static void RunNetworkMenuAction(UINT command) noexcept {
    try {
        if (command == 1) {
            Wh_Log(L"[tray-force] network icon menu: network troubleshooting "
                   L"(Internet Connections - NetworkDiagnosticsWeb)");
            ShellOpGuard::RunCommandGuarded(L"msdt.exe", L"/id NetworkDiagnosticsWeb", L"open",
                                            L"network: troubleshooting (Internet Connections)");
        } else if (command == 2) {
            Wh_Log(L"[tray-force] network icon menu: network and Internet settings "
                   L"(shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D})");
            ShellOpGuard::OpenShellUriGuarded(L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}",
                                              L"network: network and Internet settings");
        }
    } catch (...) {
        Wh_Log(L"[tray-force] exception while running the network menu entry");
    }
}

// ------------------------- menu dell'icona di rete: stesso stile del menu audio ---
// Stesso meccanismo del menu del volume/batteria (ShowBatteryMenu): popup nativo
// con CreatePopupMenu + TrackPopupMenuEx, proprietario Shell_TrayWnd. In questo modo
// Windows applica da solo il tema Explorer (aspetto Windows 10) senza disegno
// personalizzato. Voci e destinazioni invariate: 1 = risoluzione dei problemi,
// 2 = impostazioni di rete e Internet (vedi RunNetworkMenuAction).
static int g_networkMenuLogs = 0;
static void ShowNetworkIconMenuHere(HWND owner) noexcept {
    try {
        HMENU menu = CreatePopupMenu();
        if (!menu) return;
        const LANGID lang = GetUserDefaultUILanguage();
        const bool italian = PRIMARYLANGID(lang) == LANG_ITALIAN;
        AppendMenuW(menu, MF_STRING, 1,
                    italian ? L"Risoluzione dei problemi" : L"Troubleshoot problems");
        AppendMenuW(menu, MF_STRING, 2,
                    italian ? L"Apri impostazioni di rete e Internet"
                            : L"Open Network && Internet settings");

        POINT point = {};
        GetCursorPos(&point);

        // TrackPopupMenuEx richiede che la finestra proprietaria appartenga al thread
        // chiamante. Questo codice gira sul thread della finestra proprietaria della
        // mod (non su quello della barra): con Shell_TrayWnd la chiamata falliva e il
        // menu non compariva. Si usa quindi la finestra della mod; Shell_TrayWnd solo
        // se per caso e' dello stesso thread (tema Explorer, come il menu audio).
        HWND menuOwner = owner;
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray && IsWindow(tray) &&
            GetWindowThreadProcessId(tray, nullptr) == GetCurrentThreadId())
            menuOwner = tray;
        if (!menuOwner || !IsWindow(menuOwner)) {
            DestroyMenu(menu);
            return;
        }
        SetForegroundWindow(menuOwner);

        const BOOL scelta = static_cast<BOOL>(
            ImmersiveMenu::Track(menu, menuOwner, point.x, point.y, TPM_RIGHTBUTTON));
        if (!scelta && g_networkMenuLogs++ < 3)
            Wh_Log(L"[tray-force] TrackPopupMenuEx: no choice (error %lu)", GetLastError());
        DestroyMenu(menu);
        // Messaggio nullo che permette la chiusura corretta del menu (MSDN).
        PostMessageW(menuOwner, WM_NULL, 0, 0);
        if (scelta) RunNetworkMenuAction(static_cast<UINT>(scelta));
    } catch (...) {
        Wh_Log(L"[tray-force] exception in the network icon menu");
    }
}
// Chiamata dal thread della finestra proprietaria della mod: inoltra la richiesta
// a una finestra di servizio (sottoclassata) che vive sul thread della barra, cosi'
// il menu ha Shell_TrayWnd come proprietario e lo stesso aspetto del menu audio.
static void ShowNetworkIconMenu(HWND owner) noexcept {
    try {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        const DWORD trayThread = tray ? GetWindowThreadProcessId(tray, nullptr) : 0;
        if (trayThread) {
            for (int i = 0; i < g_trayWndCount; ++i) {
                HWND w = g_trayWnds[i];
                if (w && IsWindow(w) && GetWindowThreadProcessId(w, nullptr) == trayThread &&
                    PostMessageW(w, NetworkMenuMessage(), 0, 0)) {
                    if (g_networkMenuLogs < 3)
                        Wh_Log(L"[tray-force] network menu forwarded to the bar thread");
                    return;
                }
            }
        }
        if (g_networkMenuLogs++ < 3)
            Wh_Log(L"[tray-force] no service window on the bar thread: "
                   L"menu shown by the mod thread");
    } catch (...) {
    }
    ShowNetworkIconMenuHere(owner);
}
// ===========================================================================
// The click on the network icon: the genuine Windows 10 flyout, and nothing else.
//
// The Windows 10 shell opens its network flyout with a call, not with a URI:
//   CoCreateInstance(CLSID_ImmersiveShell, IID_IServiceProvider)
//   -> QueryService(CLSID_ShellExperienceManagerFactory)
//   -> GetExperienceManager(L"Windows.Internal.ShellExperience.NetworkFlyout")
//   -> QueryInterface(IID_INetworkFlyoutExperienceManager) -> ShowFlyout(rect)
// (the same path as the reference implementation; InvokeWin10NetworkFlyout above).
//
// 1.3.2: the click is taken over at the icon itself (see the click interception above) and
// every network URI of this shell is answered with a request for this flyout. Nothing else
// is left that can end on a page: the native pnidui handler and the ms-settings /
// ms-availablenetworks targets are what opened Settings or the Windows 11 panel. If the
// flyout never opens, nothing at all is launched and the log names the step that failed.
//
// A hook cannot sleep or call COM: the request travels as a registered window message to
// the owner window of the icon, the same way the icon menu does it (NetworkMenuMessage).
// ===========================================================================

// 1.3.1: the icon owner window lives in this namespace but is defined further down.
static bool EnsureOwnerWindow() noexcept;

static UINT ShowNetworkFlyoutMessage() noexcept {
    static UINT message = RegisterWindowMessageW(L"Win10ExplorerRestorer.ShowNetworkFlyout");
    return message;
}

// Asks the flyout to the owner window of the icon: that window runs on the thread with the
// message loop, which is where the shell expects ShowFlyout to be called from.
static bool RequestNetworkFlyout() noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return false;
        HWND owner = g_ownerWindow;
        if (!owner || !IsWindow(owner)) {
            if (!EnsureOwnerWindow()) return false;
            owner = g_ownerWindow;
        }
        if (!owner || !IsWindow(owner)) return false;
        const UINT message = ShowNetworkFlyoutMessage();
        if (!message) return false;
        return PostMessageW(owner, message, 0, 0) != FALSE;
    } catch (...) {
        return false;
    }
}

// The genuine Windows 10 network flyout, anchored on the tray icon. Returns the HRESULT of
// the last call: the caller decides whether to retry.
static HRESULT OpenAuthenticNetworkFlyout(const wchar_t* reason) noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return E_ABORT;

        NetworkPniRegistration reg = {};
        const bool have = CopyNetworkPniRegistration(&reg);
        RECT anchor = {};
        bool haveAnchor = have && reg.valid && QueryIconRect(reg, &anchor);

        if (!haveAnchor) {
            // The registration was never captured (the icon is pnidui's own): the system
            // GUID still gives the icon rectangle.
            NOTIFYICONIDENTIFIER identifier = {};
            identifier.cbSize = sizeof(identifier);
            identifier.guidItem = kSystemNetworkIconGuid;
            if (SUCCEEDED(Shell_NotifyIconGetRect(&identifier, &anchor))) haveAnchor = true;
        }

        HRESULT hr = InvokeWin10NetworkFlyout(haveAnchor ? &anchor : nullptr);
        if (FAILED(hr) && haveAnchor) {
            // As in the reference implementation: when the call anchored on the icon is
            // refused, it is asked once more without a rectangle.
            Wh_Log(L"[network] %s: the call anchored on the icon failed (0x%08X): asking "
                   L"again without the anchor", reason ? reason : L"click", (unsigned)hr);
            hr = InvokeWin10NetworkFlyout(nullptr);
        }
        Wh_Log(L"[network] %s: the Windows 10 network flyout call returned 0x%08X%s",
               reason ? reason : L"click", (unsigned)hr,
               haveAnchor ? L", anchored on the tray icon" : L", without an anchor");
        return hr;
    } catch (...) {
        Wh_Log(L"[network] exception while opening the Windows 10 network flyout");
        return E_FAIL;
    }
}

// ===========================================================================
// The battery icon: the genuine Windows 10 battery flyout, and nothing else.
//
// The Windows 10 shell opens this flyout exactly the way it opens the network one - with a
// call, not with a page, not with a substitute window and not with a registry value:
//
//   CoCreateInstance(CLSID_ImmersiveShell, IID_IServiceProvider)
//   -> QueryService(CLSID_ShellExperienceManagerFactory)
//   -> GetExperienceManager(L"Windows.Internal.ShellExperience.TrayBatteryFlyout")
//   -> QueryInterface(IID_TrayBatteryFlyoutExperienceManager) -> ShowFlyout(rect)
//
// The experience name and the interface id are the ones twinui.dll registers for the
// Windows 10 battery flyout: the same table the Windows 10 network flyout comes from.
//
// 1.3.3: the Win32 path of 1.3.2 is gone. Answering UseWin32BatteryFlyout = 1 on
// RegQueryValueExW made stobject.dll show its Windows 7 era Win32 flyout - that is not the
// Windows 10 flyout, it is the compatible behaviour of an older shell. No registry value is
// read or answered here any more, and the flyout that opens is the one of this shell.
// ===========================================================================
namespace BatteryFlyout {

// The power icon of the notification area of this shell (0x7820AE75; 0x7820AE74 is the
// network icon the rest of this mod pins, 0x7820AE73 the volume one).
static const GUID kSystemBatteryIconGuid = {
    0x7820AE75, 0x23E3, 0x4229, { 0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C }
};
static const GUID kIidTrayBatteryFlyoutExperienceManager = {
    0x0A73AEDC, 0x1C68, 0x410D, { 0x8D, 0x53, 0x63, 0xAF, 0x80, 0x95, 0x1E, 0x8F }
};

static std::atomic<unsigned int> g_batteryFailureLogs{0};
static int g_batteryClickLogs = 0;
static ULONGLONG g_lastBatteryClickTick = 0;

static void LogBatteryFlyoutFailure(PCWSTR stage, HRESULT hr) noexcept {
    if (g_batteryFailureLogs.fetch_add(1, std::memory_order_relaxed) < 12)
        Wh_Log(L"[battery] Windows 10 battery flyout activation failed at %s (0x%08X)",
               stage, static_cast<unsigned>(hr));
}

// The rectangle of the battery icon of this shell. The system GUID names the icon even when
// its registration was never captured by this mod.
static bool QueryBatteryIconRect(RECT* out) noexcept {
    try {
        if (!out) return false;
        NOTIFYICONIDENTIFIER identifier = {};
        identifier.cbSize = sizeof(identifier);
        identifier.guidItem = kSystemBatteryIconGuid;
        return SUCCEEDED(Shell_NotifyIconGetRect(&identifier, out));
    } catch (...) {
        return false;
    }
}

// The authentic call: the one the Windows 10 shell makes for this icon. Same chain as
// InvokeWin10NetworkFlyout above, with the battery experience name and interface id.
static HRESULT InvokeWin10BatteryFlyout(const RECT* anchor) noexcept {
    try {
        ScopedFlyoutCom com;
        void* shell = nullptr;
        HRESULT hr = CoCreateInstance(kClsidImmersiveShell, nullptr,
                                      CLSCTX_NO_CODE_DOWNLOAD | CLSCTX_LOCAL_SERVER,
                                      kIidServiceProvider, &shell);
        ScopedUnknownRef shellRef(shell);
        if (FAILED(hr) || !shell) {
            LogBatteryFlyoutFailure(L"CoCreateInstance(ImmersiveShell)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        auto sp = *reinterpret_cast<ServiceProviderVtbl* const*>(shell);
        if (!sp || !sp->QueryService) {
            LogBatteryFlyoutFailure(L"IServiceProvider::QueryService (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        void* factory = nullptr;
        hr = sp->QueryService(shell, kClsidShellExperienceManagerFactory,
                              kClsidShellExperienceManagerFactory, &factory);
        ScopedUnknownRef factoryRef(factory);
        if (FAILED(hr) || !factory) {
            LogBatteryFlyoutFailure(L"QueryService(ShellExperienceManagerFactory)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        HMODULE combase = GetModuleHandleW(L"combase.dll");
        auto createString = combase ? (HRESULT(WINAPI*)(const wchar_t*, UINT32, void**))
                                          GetProcAddress(combase, "WindowsCreateString")
                                    : nullptr;
        auto deleteString = combase ? (HRESULT(WINAPI*)(void*))
                                          GetProcAddress(combase, "WindowsDeleteString")
                                    : nullptr;
        if (!createString) {
            const HRESULT missing = HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
            LogBatteryFlyoutFailure(L"WindowsCreateString lookup", missing);
            return missing;
        }

        static const wchar_t kExperience[] = L"Windows.Internal.ShellExperience.TrayBatteryFlyout";
        void* name = nullptr;
        hr = createString(kExperience, static_cast<UINT32>(wcslen(kExperience)), &name);
        if (FAILED(hr) || !name) {
            LogBatteryFlyoutFailure(L"WindowsCreateString(TrayBatteryFlyout)",
                                    FAILED(hr) ? hr : E_FAIL);
            return FAILED(hr) ? hr : E_FAIL;
        }

        auto factoryVtbl = *reinterpret_cast<ExperienceManagerFactoryVtbl* const*>(factory);
        HRESULT hrManager = E_NOINTERFACE;
        void* manager = nullptr;
        if (factoryVtbl && factoryVtbl->GetExperienceManager)
            hrManager = factoryVtbl->GetExperienceManager(factory, name, &manager);
        if (deleteString) deleteString(name);
        ScopedUnknownRef managerRef(manager);
        if (FAILED(hrManager) || !manager) {
            LogBatteryFlyoutFailure(L"GetExperienceManager(TrayBatteryFlyout)",
                                    FAILED(hrManager) ? hrManager : E_NOINTERFACE);
            return FAILED(hrManager) ? hrManager : E_NOINTERFACE;
        }

        auto managerVtbl = *reinterpret_cast<ExperienceManagerVtbl* const*>(manager);
        if (!managerVtbl || !managerVtbl->QueryInterface) {
            LogBatteryFlyoutFailure(L"IExperienceManager::QueryInterface (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        void* flyout = nullptr;
        hr = managerVtbl->QueryInterface(manager, kIidTrayBatteryFlyoutExperienceManager, &flyout);
        ScopedUnknownRef flyoutRef(flyout);
        if (FAILED(hr) || !flyout) {
            LogBatteryFlyoutFailure(L"QueryInterface(TrayBatteryFlyoutExperienceManager)",
                                    FAILED(hr) ? hr : E_NOINTERFACE);
            return FAILED(hr) ? hr : E_NOINTERFACE;
        }

        auto flyoutVtbl = *reinterpret_cast<ExperienceManagerVtbl* const*>(flyout);
        if (!flyoutVtbl || !flyoutVtbl->ShowFlyout) {
            LogBatteryFlyoutFailure(L"ShowFlyout (vtable)", E_NOINTERFACE);
            return E_NOINTERFACE;
        }
        Win10FlyoutRect rect = {};
        if (anchor) {
            rect.x = static_cast<float>(anchor->left);
            rect.y = static_cast<float>(anchor->top);
            rect.width = static_cast<float>(anchor->right - anchor->left);
            rect.height = static_cast<float>(anchor->bottom - anchor->top);
        }
        // ShowFlyout is a private shell API: it is not called a second time after a failed
        // HRESULT, exactly as in the network flyout path.
        const HRESULT showResult = flyoutVtbl->ShowFlyout(flyout, &rect);
        if (FAILED(showResult)) LogBatteryFlyoutFailure(L"IExperienceManager::ShowFlyout", showResult);
        return showResult;
    } catch (...) {
        Wh_Log(L"[battery] C++ exception while opening the Windows 10 battery flyout");
        return E_FAIL;
    }
}

// The call anchored on the battery icon. When it is refused, it is asked once more without a
// rectangle, as the reference implementation does for the network flyout.
static HRESULT OpenAuthenticBatteryFlyout(const wchar_t* reason) noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return E_ABORT;
        RECT anchor = {};
        const bool haveAnchor = QueryBatteryIconRect(&anchor);
        HRESULT hr = InvokeWin10BatteryFlyout(haveAnchor ? &anchor : nullptr);
        if (FAILED(hr) && haveAnchor) {
            Wh_Log(L"[battery] %s: the call anchored on the icon failed (0x%08X): asking again "
                   L"without the anchor", reason ? reason : L"click", static_cast<unsigned>(hr));
            hr = InvokeWin10BatteryFlyout(nullptr);
        }
        Wh_Log(L"[battery] %s: the Windows 10 battery flyout call returned 0x%08X%s",
               reason ? reason : L"click", static_cast<unsigned>(hr),
               haveAnchor ? L", anchored on the tray icon" : L", without an anchor");
        return hr;
    } catch (...) {
        Wh_Log(L"[battery] exception while opening the Windows 10 battery flyout");
        return E_FAIL;
    }
}

static UINT ShowBatteryFlyoutMessage() noexcept {
    static UINT message = RegisterWindowMessageW(L"Win10ExplorerRestorer.ShowBatteryFlyout");
    return message;
}

// Asks the flyout to the owner window of the icon: the window the network flyout uses, on the
// thread with the message loop. A window procedure of the shell never sleeps and never calls
// COM, so from there the request is only posted.
//
// Not `static`: the window procedure of the battery icon comes earlier in the file than this
// definition and calls this function, so its declaration stands there (see the declaration next
// to ShowNetworkIconMenuHere) and the two have to name the same function.
bool RequestBatteryFlyout() noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return false;
        HWND owner = g_ownerWindow;
        if (!owner || !IsWindow(owner)) {
            if (!EnsureOwnerWindow()) return false;
            owner = g_ownerWindow;
        }
        if (!owner || !IsWindow(owner)) return false;
        const UINT message = ShowBatteryFlyoutMessage();
        return message != 0 && PostMessageW(owner, message, 0, 0) != FALSE;
    } catch (...) {
        return false;
    }
}

// Served on the owner window of the icon, exactly like the network request.
static bool OpenNativeBatteryFlyout(HWND owner, PCWSTR how) noexcept {
    (void)owner;
    try {
        if (g_unloading.load(std::memory_order_acquire)) return false;
        const ULONGLONG now = GetTickCount64();
        if (now - g_lastBatteryClickTick < 300) return true;   // the same click, delivered twice
        g_lastBatteryClickTick = now;

        HRESULT hr = E_FAIL;
        for (int attempt = 0; attempt < 3; ++attempt) {
            hr = OpenAuthenticBatteryFlyout(attempt == 0 ? L"click on the battery icon"
                                                         : L"click on the battery icon (retry)");
            if (SUCCEEDED(hr)) break;
            Sleep(150);   // the shell may not be ready yet (first click after the logon)
        }
        if (SUCCEEDED(hr)) {
            if (g_batteryClickLogs++ < 4)
                Wh_Log(L"[battery] %s: the Windows 10 battery flyout opened through the shell "
                       L"experience manager (0x%08X)", how ? how : L"left click",
                       static_cast<unsigned>(hr));
            return true;
        }
        Wh_Log(L"[battery] %s: the Windows 10 battery flyout did not open (0x%08X). Nothing "
               L"else is started: no page, no Win32 flyout, no registry value. The log line "
               L"above names the step that failed", how ? how : L"left click",
               static_cast<unsigned>(hr));
    } catch (...) {
        Wh_Log(L"[battery] exception while opening the flyout");
    }
    return true;   // the click is consumed: it must not end anywhere else
}

// 1.3.3: there is nothing to install - the flyout is opened by the call above, every time the
// click arrives. The name stays because Wh_ModInit calls it, and the log line says what the
// battery icon opens now.
static void Install() noexcept {
    Wh_Log(L"[battery] the battery icon opens the Windows 10 battery flyout "
           L"(Windows.Internal.ShellExperience.TrayBatteryFlyout) through the shell experience "
           L"manager: the same authentic call the network flyout goes through; no registry "
           L"value and no Win32 flyout is involved");
}

}  // namespace BatteryFlyout
// ===========================================================================
// 1.3.2 - the click on the network icon never reaches pnidui's own handler.
//
// The tray icon is registered by pnidui.dll together with the window that receives its
// callbacks (`hWnd`/`uCallbackMessage` of the SNI registration, recorded above). On this
// build, pnidui's handler for the left click ends on a page: it is the one thing that
// opened Settings no matter what the ShellExecute hooks answered, because the page is not
// necessarily launched through shell32!ShellExecuteW/ShellExecuteExW.
//
// The window is subclassed instead, and the left click is consumed before that handler can
// see it:
//
//   * a left click becomes a request for the Windows 10 flyout, posted to the mod's owner
//     window (a window procedure of the shell never sleeps and never calls COM, so the
//     request is only posted from here);
//   * the right click and everything else are passed straight through, so the native menu
//     of the icon is exactly what it was;
//   * nothing of the icon changes: registration, image, tooltip and menu are untouched.
//
// The subclass lives on the shell's window, so it is installed and removed with
// WindhawkUtils' cross-thread helpers, and it is removed while the module is still loaded.
// ===========================================================================
static HWND g_pniClickWnd = nullptr;    // the icon window whose click this mod answers
static UINT g_pniClickMessage = 0;      // the message the icon sends its callbacks in
static const UINT_PTR kPniClickSubclassId = 78;
static int g_pniClickLogs = 0;

static LRESULT PniClickSubclassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam,
                                    DWORD_PTR refData) {
    (void)refData;
    try {
        if (message == g_pniClickMessage && g_pniClickMessage) {
            const UINT event = LOWORD(lParam);   // NIN_* or WM_*
            const bool leftDown = (event == WM_LBUTTONDOWN || event == WM_LBUTTONDBLCLK);
            const bool leftUp = (event == WM_LBUTTONUP || event == NIN_SELECT ||
                                 event == NIN_KEYSELECT);
            if (leftDown || leftUp) {
                if (leftDown) return 0;   // the press belongs to the click answered below
                const bool requested = RequestNetworkFlyout();
                if (g_pniClickLogs++ < 4) {
                    if (requested)
                        Wh_Log(L"[network] the click on the network icon is answered here: the "
                               L"Windows 10 flyout is opened and pnidui's own handler never "
                               L"sees the click");
                    else
                        Wh_Log(L"[network] the click on the network icon is answered here, but "
                               L"the flyout request could not be posted: the click stays "
                               L"consumed and nothing else is opened");
                }
                return 0;   // consumed: no page, and no native handler that could open one
            }
        }
        if (message == WM_NCDESTROY && hwnd == g_pniClickWnd) {
            g_pniClickWnd = nullptr;
            g_pniClickMessage = 0;
        }
    } catch (...) {
        Wh_Log(L"[network] exception while answering a click on the network icon");
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

// Called at every tray tick, on the mod's own thread: the icon is registered again whenever
// the bar is recreated, with a new window, and the subclass follows it. Re-arming is limited
// to once every 10 s, which is also what repairs a subclass the shell has dropped.
static void ArmNetworkIconClickSubclass() noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return;
        NetworkPniRegistration reg = {};
        if (!CopyNetworkPniRegistration(&reg) || !reg.valid) return;
        if (!reg.hwnd || !reg.callbackMessage || !IsWindow(reg.hwnd)) return;
        if (!IsPniduiServiceWindow(reg.hwnd)) return;   // only pnidui's own icon window
        if (g_pniClickWnd == reg.hwnd && g_pniClickMessage == reg.callbackMessage) {
            // 1.3.9: the subclass is believed to be in place, but nothing ever checked it, and
            // the comment above promised a repair that this early return made impossible.
            // Every 10 s the same (proc, id) pair is set again: SetWindowSubclass with a pair
            // that already exists only refreshes it, and with a pair the shell dropped it
            // installs it again, with no gap in between (nothing is removed first).
            static ULONGLONG s_lastReassert = 0;
            const ULONGLONG now = GetTickCount64();
            if (now - s_lastReassert < 10000) return;
            s_lastReassert = now;
            if (!WindhawkUtils::SetWindowSubclassFromAnyThread(reg.hwnd, PniClickSubclassProc,
                                                               kPniClickSubclassId)) {
                Wh_Log(L"[network] the click subclass could not be refreshed: it is armed again");
                g_pniClickWnd = nullptr;   // falls through to a full arm below
            } else {
                return;
            }
        }

        // The flyout is opened by the owner window of the mod, so that window is made ready
        // here, on this thread: the window procedure of the shell is not the place where a
        // window is created or waited for.
        if (!EnsureOwnerWindow()) {
            Wh_Log(L"[network] the owner window is not available: the click of the network "
                   L"icon stays with the shell for now");
            return;
        }

        if (g_pniClickWnd && IsWindow(g_pniClickWnd))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_pniClickWnd,
                                                             PniClickSubclassProc);
        g_pniClickWnd = nullptr;
        g_pniClickMessage = reg.callbackMessage;
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(reg.hwnd, PniClickSubclassProc,
                                                          kPniClickSubclassId)) {
            g_pniClickWnd = reg.hwnd;
            Wh_Log(L"[network] the click of the network icon is answered by this mod from now "
                   L"on (window 0x%p, callback message 0x%X): the Windows 10 flyout opens and "
                   L"no page can", (void*)reg.hwnd, reg.callbackMessage);
        } else {
            g_pniClickMessage = 0;
            Wh_Log(L"[network] the click of the network icon stays with the shell: the "
                   L"subclass could not be installed");
        }
    } catch (...) {
        Wh_Log(L"[network] exception while taking over the click of the network icon");
    }
}

// Runs on the mod's tray thread while the module is still loaded, together with the rest of
// the teardown: no window procedure of the shell may outlive this module.
static void DisarmNetworkIconClickSubclass() noexcept {
    try {
        if (g_pniClickWnd && IsWindow(g_pniClickWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_pniClickWnd,
                                                             PniClickSubclassProc);
            Wh_Log(L"[network] the click of the network icon goes back to the shell");
        }
    } catch (...) {
    }
    g_pniClickWnd = nullptr;
    g_pniClickMessage = 0;
}
// --- 1.3.4) il clic sull'icona della batteria --------------------------------
// La batteria non sta in una finestra di servizio della pnidui: in questa build si registra su
// SystemTray_Main (il log della 1.3.3 lo mostra). La presa del clic dell'icona di rete, che
// lavora solo sulle finestre della pnidui, quindi non la vedeva mai: il clic arrivava al
// gestore della shell e la richiesta della mod non partiva (nessuna riga [battery] nel log).
// La presa si arma qui, sulla finestra e sull'id che il GUID della batteria ha nominato, e
// consuma il clic: l'unica cosa che si apre e' il flyout della batteria di Windows 10.
static HWND g_batteryClickWnd = nullptr;
static UINT g_batteryClickMessage = 0;
static UINT g_batteryClickId = 0;
static const UINT_PTR kBatteryClickSubclassId = 80;
static int g_batteryClickLogs = 0;

// Il clic e' dell'icona della batteria? Due strade, tutte e due strette su quell'icona: il
// messaggio di richiamo, che porta in wParam l'id dell'icona (quindi un'altra icona della stessa
// finestra non viene toccata), oppure un clic arrivato direttamente alla finestra, che vale solo
// se cade dentro il rettangolo dell'icona.
static bool BatteryIconClickIsOurs(UINT message, WPARAM wParam, LPARAM lParam) noexcept {
    try {
        const UINT event = LOWORD(lParam);
        if (message == g_batteryClickMessage && g_batteryClickMessage) {
            if (g_batteryClickId && (UINT)wParam != g_batteryClickId) return false;
            return event == WM_LBUTTONUP || event == NIN_SELECT || event == NIN_KEYSELECT ||
                   event == WM_LBUTTONDOWN;
        }
        if (message != WM_LBUTTONUP && message != WM_LBUTTONDOWN) return false;
        POINT pt = {};
        if (!GetCursorPos(&pt)) return false;
        NOTIFYICONIDENTIFIER identifier = {};
        identifier.cbSize = sizeof(identifier);
        identifier.guidItem = kBatteryTrayIconGuid;
        RECT icon = {};
        if (FAILED(Shell_NotifyIconGetRect(&identifier, &icon))) return false;
        return PtInRect(&icon, pt) != FALSE;
    } catch (...) {
        return false;
    }
}

// La forma della procedura e' quella che questo file usa gia' per l'icona di rete (le due
// funzioni di WindhawkUtils e la presa del clic definita piu' sopra).
static LRESULT BatteryClickSubclassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam,
                                        DWORD_PTR refData) {
    (void)refData;
    try {
        if (BatteryIconClickIsOurs(message, wParam, lParam)) {
            const bool press = (LOWORD(lParam) == WM_LBUTTONDOWN || message == WM_LBUTTONDOWN);
            if (press) return 0;   // la pressione appartiene al clic risposto qui sotto
            if (g_batteryClickLogs++ < 6) {
                wchar_t cls[64] = {};
                GetClassNameW(hwnd, cls, _countof(cls));
                Wh_Log(L"[battery] left click taken here (window %s, id %u): the handler of the "
                       L"shell does not see it, so nothing else can open", cls, g_batteryClickId);
            }
            BatteryFlyout::RequestBatteryFlyout();
            return 0;   // consumato: niente pagina e niente flyout della shell
        }
        if (message == WM_NCDESTROY && hwnd == g_batteryClickWnd) {
            g_batteryClickWnd = nullptr;
            g_batteryClickMessage = 0;
        }
    } catch (...) {
        Wh_Log(L"[battery] exception while answering a click on the battery icon");
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

// Chiamata dal tick dell'icona, sul thread della mod: come per l'icona di rete, la finestra
// proprietaria della mod viene preparata qui, e mai dentro un window procedure della shell.
// Riarmare ad ogni tick non serve: la presa resta finche' finestra e id non cambiano.
static void ArmBatteryIconClickSubclass() noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return;
        if (!g_batteryTrayIconWnd || !IsWindow(g_batteryTrayIconWnd)) return;
        if (g_batteryClickWnd == g_batteryTrayIconWnd &&
            g_batteryClickId == g_batteryTrayIconId &&
            g_batteryClickMessage == g_batteryTrayIconMessage)
            return;
        if (!EnsureOwnerWindow()) return;

        if (g_batteryClickWnd && IsWindow(g_batteryClickWnd))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_batteryClickWnd,
                                                             BatteryClickSubclassProc);
        g_batteryClickWnd = nullptr;
        g_batteryClickMessage = g_batteryTrayIconMessage;
        g_batteryClickId = g_batteryTrayIconId;
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(g_batteryTrayIconWnd,
                                                          BatteryClickSubclassProc,
                                                          kBatteryClickSubclassId)) {
            g_batteryClickWnd = g_batteryTrayIconWnd;
            Wh_Log(L"[battery] the click of the battery icon is answered by this mod from now on "
                   L"(window 0x%p, id %u, message 0x%X): the Windows 10 battery flyout opens, and "
                   L"nothing else", (void*)g_batteryClickWnd, g_batteryClickId,
                   (unsigned)g_batteryClickMessage);
        } else {
            g_batteryClickMessage = 0;
            g_batteryClickId = 0;
            Wh_Log(L"[battery] the click of the battery icon stays with the shell: the subclass "
                   L"could not be installed");
        }
    } catch (...) {
        Wh_Log(L"[battery] exception while taking over the click of the battery icon");
    }
}

// Sul thread della mod, mentre il modulo e' ancora caricato: nessun window procedure della
// shell deve sopravvivere a questo modulo.
static void DisarmBatteryIconClickSubclass() noexcept {
    try {
        if (g_batteryClickWnd && IsWindow(g_batteryClickWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_batteryClickWnd,
                                                             BatteryClickSubclassProc);
            Wh_Log(L"[battery] the click of the battery icon goes back to the shell");
        }
    } catch (...) {
    }
    g_batteryClickWnd = nullptr;
    g_batteryClickMessage = 0;
    g_batteryClickId = 0;
}

static bool OpenNativeNetworkFlyout(HWND owner, PCWSTR how) noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return false;
        const ULONGLONG now = GetTickCount64();
        if (now - g_lastClickTick < 300) return true;   // the same click, delivered twice
        g_lastClickTick = now;

        NetworkPniRegistration reg = {};
        const bool have = CopyNetworkPniRegistration(&reg);
        const bool rightClick = how && _wcsicmp(how, L"destro") == 0;

        if (rightClick) {
            // The right click stays as it was: the menu of pnidui's native callback, then
            // the mod's own. No flyout and no URI.
            if (have && reg.valid && reg.callbackMessage && IsPniduiServiceWindow(reg.hwnd)) {
                PostMessageW(reg.hwnd, reg.callbackMessage, static_cast<WPARAM>(reg.id),
                             MAKELPARAM(WM_RBUTTONUP, 0));
                Wh_Log(L"[tray-force] right click forwarded to the native PNI callback (0x%X)",
                       reg.callbackMessage);
                return true;
            }
            ShowNetworkIconMenu(owner);
            return true;
        }

        // 1.3.1: the left click opens the Windows 10 flyout and nothing else. The click is
        // no longer forwarded to pnidui's native callback: on this build that path ends on a
        // URI and opens Settings. No substitute page is ever launched. Three attempts, 150 ms
        // apart, as in the reference implementation.
        HRESULT hr = E_FAIL;
        for (int attempt = 0; attempt < 3; ++attempt) {
            hr = OpenAuthenticNetworkFlyout(attempt == 0 ? L"left click on the icon"
                                                         : L"left click on the icon (retry)");
            if (SUCCEEDED(hr)) break;
            Sleep(150);   // the shell may not be ready yet (first click after the logon)
        }
        if (SUCCEEDED(hr)) {
            if (g_clickLogNotes++ < 4)
                Wh_Log(L"[tray-force] left click: Windows 10 network flyout opened "
                       L"through the shell experience manager (0x%08X)",
                       static_cast<unsigned>(hr));
            return true;
        }
        Wh_Log(L"[tray-force] left click: the network flyout did not open (0x%08X). Nothing "
               L"else is launched: no Settings, no ms-availablenetworks. The log lines of the "
               L"call above name the step that failed", static_cast<unsigned>(hr));
    } catch (...) {
        Wh_Log(L"[tray-force] exception while opening the flyout");
    }
    return true;   // the click is consumed: it must not end on a URI
}



static LRESULT CALLBACK OwnerWindowProc(HWND hwnd, UINT message, WPARAM wParam,
                                        LPARAM lParam) noexcept {
    try {
        if (g_taskbarCreatedMessage && message == g_taskbarCreatedMessage) {
            // La barra e' stata ricreata: la registrazione forzata va rifatta.
            Wh_Log(L"[tray-force] the notification bar has been recreated: "
                   L"the forced icon is published again");
            g_forcedPublished = false;
            g_verifiedLogged = false;
            g_firstTick = 0;
            return 0;
        }
        if (message == ShowNetworkFlyoutMessage()) {
            // 1.3.1: a hook (or a module of the bar) has asked for the flyout: it is
            // opened here, on the thread of the icon's owner window, as a click would do.
            if (!g_unloading.load(std::memory_order_acquire)) OpenNativeNetworkFlyout(hwnd, L"left");
            return 0;
        }
        if (message == NetworkTrayForce::BatteryFlyout::ShowBatteryFlyoutMessage()) {
            // 1.3.3: the click on the battery icon has asked for the Windows 10 battery
            // flyout: it is opened here, on the thread of the icon's owner window, with the
            // same authentic call the network flyout goes through.
            if (!g_unloading.load(std::memory_order_acquire))
                NetworkTrayForce::BatteryFlyout::OpenNativeBatteryFlyout(hwnd, L"left");
            return 0;
        }
        if (message == kCallbackMessage) {
            if (g_unloading.load(std::memory_order_acquire)) return 0;
            const UINT event = LOWORD(lParam);          // NIN_* oppure WM_*
            if (event == WM_LBUTTONUP || event == NIN_SELECT || event == NIN_KEYSELECT)
                OpenNativeNetworkFlyout(hwnd, L"left");
            else if (event == WM_RBUTTONUP || event == WM_CONTEXTMENU)
                OpenNativeNetworkFlyout(hwnd, L"destro");   // menu di pnidui (stesse voci)
            // WM_MOUSEMOVE / WM_LBUTTONDOWN / altro: nessuna azione, nessun log
            // ad alta frequenza.
            return 0;
        }
        if (message == kMsgQuitOwner) {
            DestroyWindow(hwnd);
            return 0;
        }
        if (message == WM_DESTROY) {
            PostQuitMessage(0);
            return 0;
        }
    } catch (...) {
        Wh_Log(L"[tray-force] exception in the owner window of the icon");
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static DWORD WINAPI OwnerThreadProc(LPVOID) noexcept {
    HWND window = nullptr;
    try {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = OwnerWindowProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = kOwnerClass;
        RegisterClassW(&wc);   // se e' gia' registrata la creazione usa quella
        window = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kOwnerClass, L"", WS_POPUP,
                                 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
        if (!window)
            Wh_Log(L"[tray-force] owner window not created (%lu): the forced icon "
                   L"not possible", GetLastError());
        std::lock_guard<std::mutex> lock(g_ownerLock);
        g_ownerWindow = window;
        g_ownerThreadId = GetCurrentThreadId();
    } catch (...) {
        Wh_Log(L"[tray-force] exception in the creation of the owner window");
    }

    if (window) {
        MSG msg = {};
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    try {
        std::lock_guard<std::mutex> lock(g_ownerLock);
        g_ownerWindow = nullptr;
        g_ownerThreadId = 0;
        g_ownerRunning = false;
    } catch (...) {
    }
    UnregisterClassW(kOwnerClass, GetModuleHandleW(nullptr));
    return 0;
}

static bool EnsureOwnerWindow() noexcept {
    try {
        if (g_unloading.load(std::memory_order_acquire)) return false;
        {
            std::lock_guard<std::mutex> lock(g_ownerLock);
            if (g_ownerWindow && IsWindow(g_ownerWindow)) return true;
            if (g_ownerRunning) return false;   // in creazione
            if (g_ownerThread) {
                if (WaitForSingleObject(g_ownerThread, 0) != WAIT_OBJECT_0) return false;
                CloseHandle(g_ownerThread);
                g_ownerThread = nullptr;
            }
            g_ownerRunning = true;
        }
        if (!g_taskbarCreatedMessage)
            g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

        HANDLE thread = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_ownerLock);
            thread = CreateThread(nullptr, 0, OwnerThreadProc, nullptr, 0, nullptr);
            if (!thread) {
                g_ownerRunning = false;
                Wh_Log(L"[tray-force] owner thread not created (%lu)", GetLastError());
                return false;
            }
            g_ownerThread = thread;
        }
        // attesa breve: la finestra serve subito per la registrazione
        const ULONGLONG start = GetTickCount64();
        while (GetTickCount64() - start < 2000) {
            std::lock_guard<std::mutex> lock(g_ownerLock);
            if (g_ownerWindow && IsWindow(g_ownerWindow)) return true;
            if (!g_ownerRunning) return false;
            Sleep(10);
        }
        Wh_Log(L"[tray-force] the owner window did not appear within 2 s");
        return false;
    } catch (...) {
        Wh_Log(L"[tray-force] exception while creating the owner window");
        return false;
    }
}

// ------------------------------------------------------ registrazione ------
static bool BuildTipText(wchar_t* out, size_t outCount) noexcept {
    if (!out || outCount < 2) return false;
    out[0] = L'\0';
    try {
        NetworkPniRegistration reg = {};
        if (CopyNetworkPniRegistration(&reg) && reg.hasTip && reg.tip[0]) {
            wcsncpy_s(out, outCount, reg.tip, _TRUNCATE);
            return true;
        }
        HMODULE pnidui = GetModuleHandleW(L"pnidui.dll");
        if (pnidui) {
            static const UINT kTipIds[] = { 3001, 3002, 3003, 3004, 3005, 3006, 3007, 3008,
                                            3010, 3011, 3012 };
            for (UINT id : kTipIds) {
                wchar_t buffer[128] = {};
                if (LoadStringW(pnidui, id, buffer, _countof(buffer)) > 0 && buffer[0]) {
                    wcsncpy_s(out, outCount, buffer, _TRUNCATE);
                    return true;
                }
            }
        }
        const LANGID lang = GetUserDefaultUILanguage();
        const wchar_t* fallback = (PRIMARYLANGID(lang) == LANG_ITALIAN) ? L"Rete" : L"Network";
        wcsncpy_s(out, outCount, fallback, _TRUNCATE);
        return true;
    } catch (...) {
        wcsncpy_s(out, outCount, L"Network", _TRUNCATE);
        return true;
    }
}

static bool PublishForcedRegistration() noexcept {
    try {
        HWND owner = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_ownerLock);
            owner = g_ownerWindow;
        }
        if (!owner || !IsWindow(owner)) return false;

        NetworkPniRegistration reg = {};
        reg.hwnd = owner;
        reg.id = kIconId;
        reg.callbackMessage = kCallbackMessage;
        reg.hasTip = BuildTipText(reg.tip, _countof(reg.tip));
        reg.hasGuid = false;                 // il GUID viene applicato dal percorso NIM_ADD
        reg.hasVersion = true;               // protocollo moderno: NIN_SELECT/NIN_KEYSELECT
        reg.version = NOTIFYICON_VERSION_4;
        reg.valid = true;
        reg.nativeFallbackRequired = true;   // l'icona va disegnata dalla mod
        reg.forced = true;
        reg.generation = 0;
        {
            ScopedNetworkPniWriteLock lock;
            reg.generation = g_networkPniRegistration.generation + 1;
            g_networkPniRegistration = reg;
        }
        ++g_publishCount;
        Wh_Log(L"[tray-force] forced registration published (owner window 0x%p): "
               L"the existing NLM path draws the authentic icon and keeps it updated", owner);
        return true;
    } catch (...) {
        Wh_Log(L"[tray-force] exception while publishing the forced registration");
        return false;
    }
}

static bool QueryIconRect(const NetworkPniRegistration& reg, RECT* out) noexcept {
    try {
        NOTIFYICONIDENTIFIER identifier = {};
        identifier.cbSize = sizeof(identifier);
        identifier.hWnd = reg.hwnd;
        identifier.uID = reg.id;
        identifier.guidItem = kSystemNetworkIconGuid;
        HRESULT hr = Shell_NotifyIconGetRect(&identifier, out);
        if (FAILED(hr)) {
            identifier.guidItem = GUID_NULL;
            hr = Shell_NotifyIconGetRect(&identifier, out);
        }
        return SUCCEEDED(hr);
    } catch (...) {
        return false;
    }
}

static void VerifyForcedIcon() noexcept {
    try {
        const ULONGLONG now = GetTickCount64();
        if (now < g_nextVerify) return;
        g_nextVerify = now + kVerifyIntervalMs;

        NetworkPniRegistration reg = {};
        if (!CopyNetworkPniRegistration(&reg) || !reg.valid || !reg.forced) return;
        if (!reg.hwnd || !IsWindow(reg.hwnd)) return;

        RECT rect = {};
        if (QueryIconRect(reg, &rect)) {
            if (!g_verifiedLogged) {
                g_verifiedLogged = true;
                Wh_Log(L"[tray-force] VERIFIED: the network icon is present in the bar "
                       L"(rectangle %ld,%ld,%ld,%ld)", rect.left, rect.top, rect.right,
                       rect.bottom);
            }
            g_verifyFailures = 0;
            if (!g_verifiedSince) g_verifiedSince = now;
            return;
        }
        ++g_verifyFailures;
        if (g_autoTrayRestored.load()) ReApplyAllIconsMode();
        if (g_verifyFailures == 1 || g_verifyFailures % kMaxRetryLog == 0)
            Wh_Log(L"[tray-force] the icon is still not in the bar (attempt %d): "
                   L"the registration is published again", g_verifyFailures);
        // ripubblicazione: il percorso NLM riprova NIM_ADD/MODIFY
        if (!PublishForcedRegistration()) return;
    } catch (...) {
        Wh_Log(L"[tray-force] exception while verifying the icon");
    }
}

static void RetireForcedIcon(PCWSTR reason) noexcept {
    try {
        if (!g_forcedPublished) return;
        NetworkPniRegistration current = {};
        if (CopyNetworkPniRegistration(&current) && current.valid && !current.forced) {
            // La PNI nativa ha preso il sopravvento: il percorso esistente ritira
            // da solo l'icona forzata (identita' diversa).
            if (!g_retiredLogged) {
                g_retiredLogged = true;
                Wh_Log(L"[tray-force] %s: the forced icon is withdrawn, the native one applies",
                       reason ? reason : L"native icon active");
            }
        }
        g_forcedPublished = false;
        g_verifiedLogged = false;
        g_verifyFailures = 0;
    } catch (...) {
    }
}

// ---------------------------------------------------------------- lifecycle --
static bool IsForcedOwnerWindow(HWND hwnd) noexcept {
    try {
        std::lock_guard<std::mutex> lock(g_ownerLock);
        return hwnd && g_ownerWindow && hwnd == g_ownerWindow;
    } catch (...) {
        return false;
    }
}

static void OnTaskbarTransition(bool up) noexcept {
    if (up == g_taskbarWasUp) return;
    g_taskbarWasUp = up;
    if (up) {
        g_firstTick = 0;
        g_forcedPublished = false;
        g_verifiedLogged = false;
        g_verifyFailures = 0;
        g_nextVerify = 0;
        g_guaranteesApplied = false;
        Wh_Log(L"[tray-force] the mod bar is active: escalation ladder rearmed");
    } else {
        Wh_Log(L"[tray-force] the mod bar is no longer active: waiting");
    }
}

static void RequestReinstall(PCWSTR reason) noexcept {
    g_reinstallRequested.store(true, std::memory_order_release);
    g_guaranteesApplied = false;
    g_firstTick = 0;
    Wh_Log(L"[tray-force] reinstall requested for the next cycle (%s)",
           reason ? reason : L"settings");
}

static void Tick() noexcept {
    try {
        if (!g_cfg.forceNetworkTrayIcon || g_unloading.load(std::memory_order_acquire) ||
            g_stopping.load(std::memory_order_acquire)) return;
        const bool taskbarUp = IsLegacyShellProcess();
        OnTaskbarTransition(taskbarUp);
        if (!taskbarUp) return;
        if (g_reinstallRequested.exchange(false, std::memory_order_acq_rel)) {
            PublishForcedRegistration();
            g_forcedPublished = true;
            g_verifiedLogged = false;
            g_verifyFailures = 0;
        }

        ApplyVisibilityGuarantees();

        // 1.3.2: il clic sull'icona di rete viene preso in carico sull'icona stessa.
        // Gira sul thread della mod, quindi nessuna finestra viene creata o attesa
        // dentro un window procedure della shell (vedi la presa del clic, sopra).
        ArmNetworkIconClickSubclass();
        ArmBatteryIconClickSubclass();

        NetworkPniRegistration reg = {};
        const bool have = CopyNetworkPniRegistration(&reg);
        const bool native = have && reg.valid && !reg.forced && IsPniduiServiceWindow(reg.hwnd);
        if (native) {
            RetireForcedIcon(L"native PNI icon active");
            return;
        }

        const ULONGLONG now = GetTickCount64();
        if (!g_firstTick) {
            g_firstTick = now;
            Wh_Log(L"[tray-force] supervision started: if within %d s the native PNI does not "
                   L"registers the icon, the mod registers it",
                   ClampValue(g_cfg.forceNetworkTrayDelaySec, 0, 600));
            return;
        }
        const ULONGLONG delayMs =
            static_cast<ULONGLONG>(ClampValue(g_cfg.forceNetworkTrayDelaySec, 0, 600)) * 1000;
        if (now - g_firstTick < delayMs) return;

        // 1.0.0: escalation armata solo se l'icona non e' comparsa entro
        // l'attesa + 8 s. Prima di quel momento la barra non viene toccata.
        if (!g_escalate.load(std::memory_order_acquire) && !g_verifiedLogged &&
            now - g_firstTick > delayMs + 8000) {
            g_escalate.store(true, std::memory_order_release);
            Wh_Log(L"[tray-force] the icon did not appear within %llu s: enabling the guarantees on the "
                   L"bar (state saved and restorable). Up to here the bar has not been "
                   L"touched", (now - g_firstTick) / 1000);
        }

        if (!EnsureOwnerWindow()) return;

        if (!g_forcedPublished || !have || !reg.forced)
            g_forcedPublished = PublishForcedRegistration();

        if (g_forcedPublished) VerifyForcedIcon();

        // Icona vista per alcuni secondi -> togliere solo l'override virtuale,
        // cosi' il valore reale e il pulsante di overflow restano dell'utente.
        if (g_cfg.trayRestoreOverflowChevron &&
            g_autoTrayVirtual.load(std::memory_order_acquire) &&
            !g_autoTrayRestored.load() && g_restoreAttempts < 2 &&
            g_verifiedLogged && g_verifiedSince && now - g_verifiedSince > 8000) {
            if (RestoreOverflowChevron()) {
                g_autoTrayRestored.store(true);
                PublishForcedRegistration();
                g_nextVerify = 0;
            }
        }
    } catch (...) {
        Wh_Log(L"[tray-force] exception in the escalation ladder loop");
    }
}

static void Shutdown() noexcept {
    // L'hook puo' essere interrogato durante lo shutdown: non lasciare una
    // risposta locale al processo attiva dopo l'arresto del componente tray.
    g_autoTrayVirtual.store(false, std::memory_order_release);
    try {
        if (g_stopping.exchange(true, std::memory_order_acq_rel)) return;

        // 1) ritira solo l'icona forzata (se una nativa l'ha sostituita, l'identita'
        //    hWnd/uID non e' la nostra e non si tocca nulla).
        NetworkPniRegistration reg = {};
        const bool ours = CopyNetworkPniRegistration(&reg) && reg.valid && reg.forced &&
                          IsForcedOwnerWindow(reg.hwnd);
        if (ours) {
            NOTIFYICONDATAW nid = {};
            nid.cbSize = sizeof(nid);
            nid.hWnd = reg.hwnd;
            nid.uID = reg.id;
            nid.uFlags = NIF_GUID;
            nid.guidItem = kSystemNetworkIconGuid;
            if (CallNetworkIconNotify(NIM_DELETE, &nid))
                Wh_Log(L"[tray-force] forced icon removed");
            else
                Wh_Log(L"[tray-force] removing the forced icon failed");
        }

        // 2) chiude la finestra proprietaria e unisce completamente il thread.
        // Un timeout non sarebbe sicuro: il thread esegue ancora codice della mod.
        HANDLE thread = nullptr;
        HWND window = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_ownerLock);
            window = g_ownerWindow;
            thread = g_ownerThread;
            g_ownerThread = nullptr;
        }
        if (window && IsWindow(window)) {
            PostMessageW(window, WM_CANCELMODE, 0, 0);
            PostMessageW(window, kMsgQuitOwner, 0, 0);
        }
        if (thread) {
            const DWORD waited = WaitForSingleObject(thread, INFINITE);
            if (waited != WAIT_OBJECT_0)
                Wh_Log(L"[tray-force] waiting for the owner thread failed (%lu)", GetLastError());
            CloseHandle(thread);
        }
    } catch (...) {
        Wh_Log(L"[tray-force] exception during unload");
    }
}

static void SettingsChanged() noexcept {
    try {
        g_autoTrayVirtual.store(false, std::memory_order_release);
        g_stopping.store(false, std::memory_order_release);
        g_guaranteesApplied = false;
    } catch (...) {
    }
}

}  // namespace NetworkTrayForce





struct StartReplacementWindowProbe {
    wchar_t evidence[128] = {};
};


static int StartButtonEdgeDistance(HWND hwnd, const RECT& taskbarRect) {
    RECT buttonRect = {};
    if (!GetWindowRect(hwnd, &buttonRect)) return INT_MAX;
    const int taskbarWidth = taskbarRect.right - taskbarRect.left;
    const int taskbarHeight = taskbarRect.bottom - taskbarRect.top;
    const bool horizontal = taskbarWidth >= taskbarHeight;
    return horizontal ? abs((int)buttonRect.left - (int)taskbarRect.left)
                      : abs((int)buttonRect.top - (int)taskbarRect.top);
}

static bool IsPlausibleStartButton(HWND hwnd, HWND taskbar, const RECT& taskbarRect,
                                   bool allowEdgeButton) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) || !IsWindowEnabled(hwnd) ||
        !IsChild(taskbar, hwnd)) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return false;

    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect)) return false;
    const int width = (int)(rect.right - rect.left);
    const int height = (int)(rect.bottom - rect.top);
    if (width <= 0 || height <= 0 || width > 240 || height > 240) return false;

    wchar_t cls[128] = {};
    if (!GetClassNameW(hwnd, cls, _countof(cls))) return false;
    const int controlId = GetDlgCtrlID(hwnd);
    // Unico vero criterio per il pulsante Start nativo di Windows:
    // control ID 0x130. Non accettiamo più la classe "Start"/"StartButton"
    // perché OpenShell usa esattamente quella classe per il suo pulsante.
    if (controlId == 0x130) return true;

    // Ultima spiaggia: solo per build dove l'ID è cambiato, e solo se è
    // davvero un "Button" al bordo della taskbar.
    if (!allowEdgeButton || _wcsicmp(cls, L"Button") != 0) return false;
    const int edgeDistance = StartButtonEdgeDistance(hwnd, taskbarRect);
    return edgeDistance >= 0 && edgeDistance <= 180;
}

struct StartButtonSearch {
    HWND taskbar = nullptr;
    RECT taskbarRect = {};
    HWND byId = nullptr;
    HWND byClass = nullptr;
    HWND byEdge = nullptr;
    int edgeDistance = INT_MAX;
};

static BOOL CALLBACK FindStartButtonChildProc(HWND hwnd, LPARAM param) {
    auto* search = (StartButtonSearch*)param;
    if (GetDlgCtrlID(hwnd) == 0x130 &&
        IsPlausibleStartButton(hwnd, search->taskbar, search->taskbarRect, false)) {
        search->byId = hwnd;
        return FALSE;
    }

    wchar_t cls[128] = {};
    if (GetClassNameW(hwnd, cls, _countof(cls)) &&
        ((_wcsicmp(cls, L"Start") == 0) || ContainsNoCase(cls, L"StartButton")) &&
        IsPlausibleStartButton(hwnd, search->taskbar, search->taskbarRect, false)) {
        search->byClass = hwnd;
        return FALSE;
    }

    if (IsPlausibleStartButton(hwnd, search->taskbar, search->taskbarRect, true)) {
        const int distance = StartButtonEdgeDistance(hwnd, search->taskbarRect);
        if (distance < search->edgeDistance) {
            search->byEdge = hwnd;
            search->edgeDistance = distance;
        }
    }
    return TRUE;
}

static HWND FindNativeTaskbarStartButton() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) return nullptr;
    DWORD pid = 0;
    GetWindowThreadProcessId(taskbar, &pid);
    if (pid != GetCurrentProcessId()) return nullptr;

    RECT taskbarRect = {};
    if (!GetWindowRect(taskbar, &taskbarRect)) return nullptr;

    HWND byId = GetDlgItem(taskbar, 0x130);
    if (IsPlausibleStartButton(byId, taskbar, taskbarRect, false)) return byId;

    StartButtonSearch search = {};
    search.taskbar = taskbar;
    search.taskbarRect = taskbarRect;
    EnumChildWindows(taskbar, FindStartButtonChildProc, (LPARAM)&search);
    if (search.byId) return search.byId;
    if (search.byClass) return search.byClass;
    return search.byEdge;
}

static bool IsWinXNativeContextMenuRequest() {
    // Il clic destro sul pulsante Start resta di Windows: il menu del pulsante Start
    // (Win+X) non e' di questo mod, ed e' un altro modulo a mostrarlo. Il test sul
    // cursore protegge quel percorso anche dove un hook non si e' potuto installare.
    HWND startButton = FindNativeTaskbarStartButton();
    POINT pt = {};
    RECT rect = {};
    if (!startButton || !GetCursorPos(&pt) ||
        !GetWindowRect(startButton, &rect) || !PtInRect(&rect, pt))
        return false;
    HWND hitWindow = WindowFromPoint(pt);
    return hitWindow == startButton ||
           (hitWindow && IsChild(startButton, hitWindow));
}


// To be used ONLY with g_realExePath: after the path spoof GetModuleFileNameW
// answers with the fake path, and asking it whether we are the private shell
// gave the wrong answer (bug seen in the 14:49 log).
static bool IsProcessImageName(const wchar_t* path, const wchar_t* expected) noexcept {
    if (!path || !expected) return false;
    const wchar_t* base = wcsrchr(path, L'\\');
    return _wcsicmp(base ? base + 1 : path, expected) == 0;
}

static bool IsPrivateExplorerProcess() {
    if (g_realExePath[0] == 0) return false;
    return _wcsicmp(g_realExePath, g_cfg.explorerPath) == 0;
}

// ---- explorer.exe ---------------------------------------------------------

// The Windows 10 shell is an explorer.exe that is not the system one.
static bool IsLegacyShellProcess() {
    wchar_t path[MAX_PATH] = {};
    if (!GetModuleFileNameW(nullptr, path, _countof(path))) return false;
    if (g_realExePath[0] == 0) wcscpy_s(g_realExePath, path);
    if (IsPrivateExplorerProcess()) return true;
    wchar_t systemRoot[MAX_PATH] = {};
    const DWORD rootLength =
        GetEnvironmentVariableW(L"SystemRoot", systemRoot, _countof(systemRoot));
    if (!rootLength || rootLength >= _countof(systemRoot)) return false;
    const size_t rootChars = wcslen(systemRoot);
    const size_t pathChars = wcslen(path);
    if (pathChars == rootChars + 13 && _wcsnicmp(path, systemRoot, rootChars) == 0 &&
        _wcsicmp(path + rootChars, L"\explorer.exe") == 0) {
        return false;
    }
    return IsProcessImageName(path, L"explorer.exe");
}

}

using namespace RestorerTaskbar;


// ---------------------------------------------------------------------------
// Where the downloaded Windows 10 files live
//
// The folder the Windows 10 taskbar mod by Anixx uses for its own download:
// %ProgramData%\Windhawk\Engine\ModsWritable\LegacyStore. Both mods then read and write
// a single set of verified files instead of keeping two copies of the same system
// binaries: a file already there is reused as soon as it matches this mod's pin, and
// only what is missing is downloaded into that folder. If the folder cannot be created
// or written (no rights over ProgramData), this mod's own storage is used as a
// fallback, so the mod keeps working on its own.
// ---------------------------------------------------------------------------
static const wchar_t kSharedStorePath[] =
    L"%ProgramData%\\Windhawk\\Engine\\ModsWritable\\LegacyStore";

static bool DirectoryAcceptsWrites(const wchar_t* dir) {
    wchar_t probe[MAX_PATH] = {};
    _snwprintf_s(probe, _countof(probe), _TRUNCATE, L"%s\\.w10er-write-probe", dir);
    HANDLE file = CreateFileW(probe, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    CloseHandle(file);
    return true;
}

static bool ResolveStorePath(wchar_t* out, size_t count) {
    out[0] = 0;
    wchar_t shared[MAX_PATH] = {};
    if (ExpandEnvironmentStringsW(kSharedStorePath, shared, _countof(shared)) > 0 && shared[0]) {
        if (EnsureDirectory(shared) && DirectoryAcceptsWrites(shared)) {
            Wh_Log(L"[store] shared folder: %s (the same one the Win10 taskbar mod uses)", shared);
            wcsncpy_s(out, count, shared, _TRUNCATE);
            return true;
        }
        Wh_Log(L"[store] the shared folder is not writable (%s): this mod's own storage is used",
               shared);
    }
    wchar_t storage[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storage, _countof(storage)) || !storage[0]) return false;
    _snwprintf_s(out, count, _TRUNCATE, L"%s\\Win10Shell", storage);
    Wh_Log(L"[store] private folder: %s", out);
    return true;
}

static void LoadFlyoutSettings() {
    // Data folder: the folder the Windows 10 taskbar mod uses for its own download, so
    // the two mods share one set of verified files (see ResolveStorePath above).
    if (!ResolveStorePath(g_cfg.storePath, _countof(g_cfg.storePath))) g_cfg.storePath[0] = 0;
    _snwprintf_s(g_cfg.explorerPath, _countof(g_cfg.explorerPath), _TRUNCATE,
                 L"%s\\explorer.exe", g_cfg.storePath);

    g_cfg.buildIndex = 0;                 // 10.0.19039.1: the build the tray files are pinned to
    g_cfg.provideTrayDlls = Wh_GetIntSetting(L"ProvideTrayModules") != 0;
    g_cfg.requireSignature = Wh_GetIntSetting(L"RequireSignature") != 0;
    g_cfg.downloadTimeoutSec = Wh_GetIntSetting(L"DownloadTimeoutSec");
    if (g_cfg.downloadTimeoutSec <= 0) g_cfg.downloadTimeoutSec = 20;

    // The reference mod's tray/network options: without these the code that forces the
    // network icon is switched off (the two flags are read in other blocks).
    g_cfg.forceNetworkTrayIcon = Wh_GetIntSetting(L"ForceNetworkTrayIcon") != 0;
    g_cfg.forceNetworkTrayDelaySec = Wh_GetIntSetting(L"ForceNetworkTrayDelaySec");
    if (g_cfg.forceNetworkTrayDelaySec < 0) g_cfg.forceNetworkTrayDelaySec = 0;
    if (g_cfg.forceNetworkTrayDelaySec > 600) g_cfg.forceNetworkTrayDelaySec = 600;
    g_cfg.forceNetworkTrayResetTraySettings =
        Wh_GetIntSetting(L"ForceNetworkTrayResetTraySettings") != 0;
    g_cfg.trayRestoreOverflowChevron =
        Wh_GetIntSetting(L"TrayRestoreOverflowChevron") != 0;
    g_cfg.shellOpGuardTimeoutMs = Wh_GetIntSetting(L"ShellOpGuardTimeoutMs");
    if (g_cfg.shellOpGuardTimeoutMs < 200) g_cfg.shellOpGuardTimeoutMs = 200;
    if (g_cfg.shellOpGuardTimeoutMs > 10000) g_cfg.shellOpGuardTimeoutMs = 10000;
    // Constants of the reference mod: the notification policy is only a virtual
    // fallback. No private QuickActions binary patch is applied by this source, and
    // the experimental UWP taskbar buttons stay off.
    g_cfg.notificationsOff = true;
    g_cfg.fixUwpTaskbar = false;
    g_cfg.fixNotificationsCrash = true;   // the Action Center policy is served in memory
    g_logTrayActivity = Wh_GetIntSetting(L"LogTrayActivity") != 0;

    // Action Center animation (base: the mod by AdmXP8, v0.9).
    g_acAnimation = Wh_GetIntSetting(L"ActionCenterAnimation") != 0;
    g_squareFlyoutCorners = Wh_GetIntSetting(L"ExperimentalSquareFlyoutCorners") != 0;
    g_acCloseDelayMs = ClampInt(Wh_GetIntSetting(L"ActionCenterCloseDelayMs"), 1, 1900);
    g_acSlideInMs = ClampInt(Wh_GetIntSetting(L"ActionCenterSlideInMs"), 0, 1000);
    g_acSlideOutMs = ClampInt(Wh_GetIntSetting(L"ActionCenterSlideOutMs"), 0, 1000);
    g_showActionCenterButton = Wh_GetIntSetting(L"ShowActionCenterButton") != 0;
    try {
        auto value = WindhawkUtils::StringSetting::make(L"ActionCenterConflict");
        g_actionCenterReassert = !value.get() || _wcsicmp(value.get(), L"reassert") == 0;
    } catch (...) {
        g_actionCenterReassert = true;
    }
}
// ---------------------------------------------------------------------------
// The two tray windows that own a menu of their own
//
// The show desktop button and the clock are children of the taskbar, created by the
// shell's own thread: the same cross-thread subclass machinery the indicator uses in
// the other module is used here (WindhawkUtils, five-parameter procedure). If the
// shell has shown no menu for the click, this mod shows the Windows 10 one - the two
// fallback menus defined above, which exist for exactly this case.
// ---------------------------------------------------------------------------
typedef LRESULT (*IndicatorSubclassFn)(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

class ScopedWindowSubclass {
public:
    ScopedWindowSubclass(HWND w, IndicatorSubclassFn proc, DWORD_PTR ref, bool* installed)
        : m_w(w), m_proc(proc), m_ref(ref), m_installed(installed) {
        if (m_installed) *m_installed = false;
        if (m_w) WindhawkUtils::RemoveWindowSubclassFromAnyThread(m_w, m_proc);
    }
    ~ScopedWindowSubclass() {
        if (!m_w) return;
        const bool ok = WindhawkUtils::SetWindowSubclassFromAnyThread(m_w, m_proc, m_ref);
        if (m_installed) *m_installed = ok;
    }
    ScopedWindowSubclass(const ScopedWindowSubclass&) = delete;
    ScopedWindowSubclass& operator=(const ScopedWindowSubclass&) = delete;
private:
    HWND m_w;
    IndicatorSubclassFn m_proc;
    DWORD_PTR m_ref;
    bool* m_installed;
};

// explorer creates the cell and its buttons at different times (the 18:14 log
// shows a first, empty instance of the cell before the real one, and the button
// appearing afterwards), so the scan runs on every tick and reports what it
// finds; while nothing is there it says so, so an absent cell never looks like a

// --- the two tray windows that own those menus -----------------------------
// The show desktop button and the clock are children of the taskbar, created by
// the shell's own thread: the cross-thread subclass machinery of WindhawkUtils is used
// (five-parameter procedure), and the
// same rules apply - every call inside try/catch, RAII for the handles.

static const wchar_t* const kTrayMenuClasses[] = {
    L"TrayShowDesktopButtonWClass",
    L"TrayClockWClass",
    L"ClockButton",
};
static const int kTrayMenuClassCount = (int)(sizeof(kTrayMenuClasses) / sizeof(kTrayMenuClasses[0]));

struct TrayMenuTarget {
    HWND wnd;
    bool subclassed;
};
static TrayMenuTarget g_trayMenuTargets[8] = {};
static int g_trayMenuTargetCount = 0;
static const UINT_PTR kTrayMenuSubclassId = 79;
static DWORD g_lastTrayMenuArm = 0;

static bool IsTrayMenuClass(const wchar_t* cls) {
    if (!cls || !cls[0]) return false;
    for (int i = 0; i < kTrayMenuClassCount; i++)
        if (_wcsicmp(cls, kTrayMenuClasses[i]) == 0) return true;
    return false;
}

static int TrackedMenuIndex(HWND w) {
    for (int i = 0; i < g_trayMenuTargetCount; i++)
        if (g_trayMenuTargets[i].wnd == w) return i;
    return -1;
}

static void UntrackMenuTarget(int idx) {
    if (idx < 0 || idx >= g_trayMenuTargetCount) return;
    for (int i = idx; i + 1 < g_trayMenuTargetCount; i++)
        g_trayMenuTargets[i] = g_trayMenuTargets[i + 1];
    g_trayMenuTargets[--g_trayMenuTargetCount] = TrayMenuTarget{};
}

struct TrayMenuEnumCtx {
    HWND found[8];
    int count;
};

static BOOL CALLBACK TrayMenuEnumProc(HWND w, LPARAM param) {
    TrayMenuEnumCtx* ctx = (TrayMenuEnumCtx*)param;
    if (!ctx || ctx->count >= (int)(sizeof(ctx->found) / sizeof(ctx->found[0]))) return FALSE;
    wchar_t cls[64] = {};
    GetClassNameW(w, cls, _countof(cls));
    if (IsTrayMenuClass(cls)) ctx->found[ctx->count++] = w;
    return TRUE;
}

// Answers the right click on those two windows. If explorer has already shown a
// menu for this very window in this click, nothing is done here: the entries
// have been completed in the hook instead, and a second menu would be wrong.
static LRESULT TrayMenuSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                    DWORD_PTR refData) {
    (void)wParam;
    (void)refData;
    try {
        switch (msg) {
        case WM_CONTEXTMENU: {
            wchar_t cls[64] = {};
            GetClassNameW(hwnd, cls, _countof(cls));
            if (IsShowDesktopClass(cls)) {
                // Windows 10 gives this button its own small menu: that one is the
                // mod's, because explorer has none.
                ShowShowDesktopMenu(hwnd);
                return 0;
            }

            break;
        }
        case WM_NCDESTROY: {
            const int idx = TrackedMenuIndex(hwnd);
            if (idx >= 0) {
                UntrackMenuTarget(idx);
                Wh_Log(L"[menu] tray menu window 0x%p went away: supervision removed", (void*)hwnd);
            }
            break;
        }
        default:
            break;
        }
        (void)lParam;
    } catch (...) {
        Wh_Log(L"[menu] exception while answering a tray menu request");
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static void ArmTrayMenuSubclass() {
    try {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (!tray) return;
        TrayMenuEnumCtx ctx = {};
        EnumChildWindows(tray, TrayMenuEnumProc, (LPARAM)&ctx);

        bool added = false;
        for (int i = 0; i < ctx.count; i++) {
            HWND w = ctx.found[i];
            if (!w || !IsWindow(w)) continue;
            if (TrackedMenuIndex(w) >= 0) continue;
            if (g_trayMenuTargetCount >= (int)(sizeof(g_trayMenuTargets) / sizeof(g_trayMenuTargets[0])))
                break;
            wchar_t cls[64] = {};
            GetClassNameW(w, cls, _countof(cls));
            g_trayMenuTargets[g_trayMenuTargetCount].wnd = w;
            g_trayMenuTargets[g_trayMenuTargetCount].subclassed = false;
            g_trayMenuTargetCount++;
            added = true;
            Wh_Log(L"[menu] tray menu window found (%s 0x%p): the mod answers its right click",
                   cls[0] ? cls : L"window", (void*)w);
        }

        for (int i = g_trayMenuTargetCount - 1; i >= 0; i--) {
            wchar_t cls[64] = {};
            HWND w = g_trayMenuTargets[i].wnd;
            if (!w || !IsWindow(w) || !GetClassNameW(w, cls, _countof(cls)) || !IsTrayMenuClass(cls))
                UntrackMenuTarget(i);
        }

        const DWORD now = GetTickCount();
        if (!added && now - g_lastTrayMenuArm < 10000) return;
        g_lastTrayMenuArm = now;
        for (int i = 0; i < g_trayMenuTargetCount; i++) {
            HWND w = g_trayMenuTargets[i].wnd;
            if (!w || !IsWindow(w)) continue;
            wchar_t cls[64] = {};
            GetClassNameW(w, cls, _countof(cls));
            bool installed = false;
            {
                ScopedWindowSubclass slot(w, TrayMenuSubclassProc, kTrayMenuSubclassId, &installed);
            }
            const bool first = !g_trayMenuTargets[i].subclassed;
            g_trayMenuTargets[i].subclassed = installed;
            if (installed && first)
                Wh_Log(L"[menu] subclass installed on %s 0x%p (its menu is the mod's)", cls, (void*)w);
            else if (!installed) {
                static DWORD lastReport = 0;
                if (!lastReport || now - lastReport > 30000) {
                    lastReport = now;
                    Wh_Log(L"[menu] the shell did not accept the subclass on %s 0x%p, retrying",
                           cls, (void*)w);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[menu] exception while supervising the tray menu windows");
    }
}


// ---------------------------------------------------------------------------
// Action Center button: the opposite of what "hide-action-center-icon" does
//
// The Windows 10 taskbar keeps "show the Action Center button" in the window's own data,
// 120 bytes in: the same byte that mod writes with FALSE. Here it is written with TRUE and
// kept there, because a byte cannot be shared - whoever writes last wins. Two things make
// running next to that mod safe:
//   * every write is validated first (the byte must be committed, writable and not a guard
//     page), so a different layout of the window data can never take the shell down;
//   * the byte is re-read twice per second: when another mod sets it back to 0, the
//     conflict is named in the log and the value is written again.
// Nothing is written to the registry for this: the flag lives in the taskbar process only.
// ---------------------------------------------------------------------------
static const wchar_t* kActionCenterButtonClass = L"ControlCenterButton";
static const size_t kActionCenterButtonFlagOffset = 120;   // same field as that mod

typedef HWND(WINAPI* CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
                                        HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;
static HWND g_actionCenterButton = nullptr;
static unsigned g_actionCenterConflicts = 0;
static bool g_actionCenterConflictNamed = false;
static bool g_actionCenterWriteRefused = false;

// A byte is written only after this check: committed, writable, inside this process.
static bool CanWriteByte(const void* address) {
    MEMORY_BASIC_INFORMATION info = {};
    if (!address) return false;
    if (VirtualQuery(address, &info, sizeof(info)) != sizeof(info)) return false;
    if (info.State != MEM_COMMIT) return false;
    if (info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) return false;
    const DWORD protect = info.Protect & 0xFF;
    return protect == PAGE_READWRITE || protect == PAGE_WRITECOPY ||
           protect == PAGE_EXECUTE_READWRITE || protect == PAGE_EXECUTE_WRITECOPY;
}

static BYTE* ActionCenterButtonFlag(HWND hwnd) {
    BYTE* data = reinterpret_cast<BYTE*>(GetWindowLongPtrW(hwnd, 0));
    if (!data) return nullptr;
    BYTE* flag = data + kActionCenterButtonFlagOffset;
    return CanWriteByte(flag) ? flag : nullptr;
}

static bool WriteActionCenterButtonFlag(HWND hwnd, unsigned char value, const wchar_t* why) {
    BYTE* flag = ActionCenterButtonFlag(hwnd);
    if (!flag) {
        if (!g_actionCenterWriteRefused) {
            g_actionCenterWriteRefused = true;
            Wh_Log(L"[actioncenter] the button flag is not writable in this build: the button "
                   L"is left as the shell made it");
        }
        return false;
    }
    if (*flag == value) return true;
    *flag = value;
    Wh_Log(L"[actioncenter] button flag written to %u (%s)", (unsigned)value, why);
    return true;
}

static bool IsActionCenterButtonClass(LPCWSTR className) {
    if (!className || ((ULONG_PTR)className & ~(ULONG_PTR)0xffff) == 0) return false;
    return _wcsicmp(className, kActionCenterButtonClass) == 0;
}

// Called from the CreateWindowExW hook, so the button is caught as the taskbar makes it.
static void OnActionCenterButtonCreated(HWND hwnd) {
    if (!g_showActionCenterButton) return;
    g_actionCenterButton = hwnd;
    WriteActionCenterButtonFlag(hwnd, 1, L"button created");
}

// Retry path: the button can also exist from before this mod was loaded.
struct ActionCenterSearch { HWND found; };
static BOOL CALLBACK ActionCenterSearchProc(HWND hwnd, LPARAM param) {
    wchar_t cls[64] = {};
    if (GetClassNameW(hwnd, cls, _countof(cls)) &&
        _wcsicmp(cls, kActionCenterButtonClass) == 0) {
        reinterpret_cast<ActionCenterSearch*>(param)->found = hwnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindActionCenterButton() {
    if (g_actionCenterButton && IsWindow(g_actionCenterButton)) return g_actionCenterButton;
    ActionCenterSearch search = {};
    HWND shell = GetShellWindow();
    if (shell) EnumChildWindows(shell, ActionCenterSearchProc, reinterpret_cast<LPARAM>(&search));
    if (!search.found) EnumWindows(ActionCenterSearchProc, reinterpret_cast<LPARAM>(&search));
    if (search.found) g_actionCenterButton = search.found;
    return g_actionCenterButton;
}

// Twice per second: keeps the value written and names the conflict.
static void ActionCenterButtonTick() {
    if (!g_showActionCenterButton || g_unloading.load()) return;
    HWND button = FindActionCenterButton();
    if (!button) return;
    BYTE* flag = ActionCenterButtonFlag(button);
    if (!flag) return;
    if (*flag == 1) return;

    if (!g_actionCenterReassert) {
        if (!g_actionCenterConflictNamed) {
            g_actionCenterConflictNamed = true;
            Wh_Log(L"[actioncenter] another mod hides the Action Center button "
                   L"(hide-action-center-icon writes the same byte). ActionCenterConflict=log: "
                   L"the button stays hidden; set ShowActionCenterButton=off to stop both mods "
                   L"from writing this byte");
        }
        return;
    }

    ++g_actionCenterConflicts;
    WriteActionCenterButtonFlag(button, 1, L"reasserted against another mod");
    if (!g_actionCenterConflictNamed) {
        g_actionCenterConflictNamed = true;
        Wh_Log(L"[actioncenter] conflict detected: another mod (hide-action-center-icon) writes "
               L"the same button flag. This mod writes it back, so the button stays visible; "
               L"disable the other mod, or set ShowActionCenterButton=off here, to stop both "
               L"from writing this byte");
    } else if (g_actionCenterConflicts % 20 == 0) {
        Wh_Log(L"[actioncenter] the button flag was reset again (%u times): another mod is still "
               L"writing this byte", g_actionCenterConflicts);
    }
}

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                        DWORD dwStyle, int x, int y, int nWidth, int nHeight,
                                        HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                        LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y,
                                         nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    try {
        if (hwnd && IsActionCenterButtonClass(lpClassName)) OnActionCenterButtonCreated(hwnd);
    } catch (...) {
    }
    return hwnd;
}

// ---------------------------------------------------------------------------
// The Windows 10 tray DLLs: this module downloads and verifies them itself
//
// The data folder is this mod's own storage (…\Win10Shell). A file already there is reused
// (same SHA-256, same signature); nothing assumes that another mod ran first. If a download
// fails the tray part stays off instead of holding up the shell, and it is retried later.
// ---------------------------------------------------------------------------
static bool g_trayStoreReady = false;

static bool EnsureTrayStoreFile(const wchar_t* name, const wchar_t* symbolId,
                                const wchar_t* sha256) {
    wchar_t path[MAX_PATH] = {};
    if (EnsureVerifiedFile(g_cfg.storePath, name, symbolId, sha256, path, _countof(path))) {
        Wh_Log(L"[store] %s ready (%s)", name, path);
        return true;
    }
    Wh_Log(L"[store] %s not available: the download or the verification failed", name);
    return false;
}

static bool PrepareTrayStore() {
    if (g_trayStoreReady) return true;
    if (!g_cfg.storePath[0]) {
        Wh_Log(L"[store] no data folder available: the Windows 10 tray DLLs cannot be fetched");
        return false;
    }
    if (!EnsureDirectory(g_cfg.storePath)) return false;

    bool complete = true;
    for (const ExtraFile& file : kTrayFiles) {
        if (!EnsureTrayStoreFile(file.name, file.symbolId, file.sha256)) complete = false;
    }
    // explorer.exe: the private Windows 10 shell itself. Same symbol-server entry and
    // same pin the Windows 10 taskbar mod uses, so the file and its locale folder are
    // shared with it: whoever needs it first downloads it, the other one finds it
    // verified and does not download it again.
    if (g_cfg.provideTrayDlls) {
        wchar_t explorerPath[MAX_PATH] = {};
        if (EnsureVerifiedFile(g_cfg.storePath, L"explorer.exe", kBuilds[0].symbolId,
                               kBuilds[0].sha256, explorerPath, _countof(explorerPath))) {
            Wh_Log(L"[store] explorer.exe ready (%s)", explorerPath);
        } else {
            Wh_Log(L"[store] explorer.exe not available: the private shell cannot start until "
                   L"the file is there (the Win10 taskbar mod downloads the same one)");
        }
    }

    g_trayStoreReady = complete;
    return complete;
}

// Is the running process the given image? The panel belongs to another process, so the
// name of the image is what tells the two apart.
static bool ImageNameIs(const wchar_t* path, const wchar_t* expected) {
    if (!path || !expected) return false;
    const wchar_t* base = wcsrchr(path, L'\\');
    return _wcsicmp(base ? base + 1 : path, expected) == 0;
}

static bool IsCoreWindow(HWND hwnd) {
    if (!hwnd) return false;
    wchar_t cls[64] = {};
    if (!GetClassNameW(hwnd, cls, _countof(cls))) return false;
    return wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
}

// The panel is a window of ShellExperienceHost.exe, not of this process.
static bool IsShellExperienceHostWindow(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return false;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    wchar_t path[MAX_PATH] = {};
    DWORD size = _countof(path);
    const bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != 0;
    CloseHandle(process);
    if (!ok) return false;
    const wchar_t* base = wcsrchr(path, L'\\');
    return _wcsicmp(base ? base + 1 : path, L"ShellExperienceHost.exe") == 0;
}

// Shape of the Action Center: full height of the work area, docked to the right edge of
// its monitor, 250-700 px wide. Every other flyout is left alone.
static bool LooksLikeActionCenterPanel(const RECT& rect, const MONITORINFO& monitor) {
    const int width = rect.right - rect.left;
    return rect.top <= monitor.rcWork.top + 2 &&
           rect.bottom >= monitor.rcWork.bottom - 2 &&
           rect.right >= monitor.rcMonitor.right - 2 &&
           rect.right <= monitor.rcMonitor.right + 2 &&
           width >= 250 && width <= 700;
}

// The close timer belongs to the shell's own code: only a timer set from inside
// twinui.pcshell.dll is replaced.
static bool TwinuiInStack() {
    void* frames[12] = {};
    const USHORT count = CaptureStackBackTrace(1, 12, frames, nullptr);
    for (USHORT i = 0; i < count; ++i) {
        HMODULE module = nullptr;
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                (LPCWSTR)frames[i], &module) || !module)
            continue;
        wchar_t path[MAX_PATH] = {};
        if (!GetModuleFileNameW(module, path, _countof(path))) continue;
        const wchar_t* base = wcsrchr(path, L'\\');
        if (_wcsicmp(base ? base + 1 : path, L"twinui.pcshell.dll") == 0) return true;
    }
    return false;
}

// Only a relative timeout of about two seconds, only while the panel is open, and only
// from the shell's own code: anything else is passed through untouched.
static bool IsCloseTimer(const FILETIME* due) {
    if (!g_acOpen || !due) return false;
    ULARGE_INTEGER value = {};
    value.LowPart = due->dwLowDateTime;
    value.HighPart = due->dwHighDateTime;
    const LONGLONG ticks = static_cast<LONGLONG>(value.QuadPart);
    if (ticks >= 0) return false;
    const LONGLONG ms = (-ticks) / 10000;
    if (ms < 1900 || ms > 2100) return false;
    return TwinuiInStack();
}

static FILETIME RelativeDue(int ms) {
    LARGE_INTEGER value = {};
    value.QuadPart = -static_cast<LONGLONG>(ms) * 10000;
    FILETIME due = {};
    due.dwLowDateTime = value.LowPart;
    due.dwHighDateTime = static_cast<DWORD>(value.HighPart);
    return due;
}

static double EaseOut(double t) {
    const double u = 1.0 - t;
    return 1.0 - u * u * u;
}

static ULONGLONG AcNowMs() {
    LARGE_INTEGER frequency = {}, counter = {};
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return static_cast<ULONGLONG>(counter.QuadPart * 1000 / frequency.QuadPart);
}

static void MovePanelX(HWND hwnd, int x, int y) {
    SetWindowPos(hwnd, nullptr, x, y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

// Returns false when a newer open or close started while this slide was running.
static bool RunAcSlide(HWND hwnd, int fromX, int toX, int y, int ms, LONG generation) {
    const ULONGLONG start = AcNowMs();
    for (;;) {
        if (generation != g_acGeneration) return false;
        const ULONGLONG elapsed = AcNowMs() - start;
        const double t = elapsed >= static_cast<ULONGLONG>(ms)
                             ? 1.0
                             : static_cast<double>(elapsed) / static_cast<double>(ms);
        MovePanelX(hwnd, fromX + static_cast<int>((toX - fromX) * EaseOut(t)), y);
        if (t >= 1.0) return true;
        Sleep(8);
    }
}

struct AcSlideThreadParam {
    HWND hwnd;
    int fromX;
    int toX;
    int y;
    int ms;
    LONG generation;
};

static DWORD WINAPI AcSlideInThread(LPVOID parameter) noexcept {
    std::unique_ptr<AcSlideThreadParam> owned(
        static_cast<AcSlideThreadParam*>(parameter));
    try {
        const AcSlideThreadParam params = *owned;
        if (RunAcSlide(params.hwnd, params.fromX, params.toX, params.y,
                       params.ms, params.generation) && g_acAnimLogs++ < 6)
            Wh_Log(L"[ac-anim] slide-in finished at x=%d", params.toX);
    } catch (...) {
        Wh_Log(L"[ac-anim] C++ exception in the slide-in worker");
    }
    return 0;
}

static bool StartAcSlideInThread(const AcSlideThreadParam& value) noexcept {
    AcSlideThreadParam* params = nullptr;
    try {
        params = new AcSlideThreadParam(value);
        std::lock_guard<std::mutex> lock(g_acSlideThreadsMutex);
        if (g_acSlideThreadsStopping || g_unloading.load(std::memory_order_acquire)) {
            delete params;
            return false;
        }

        // Prune finished thread handles while retaining all running workers.
        for (size_t i = 0; i < g_acSlideThreads.size();) {
            if (WaitForSingleObject(g_acSlideThreads[i], 0) == WAIT_OBJECT_0) {
                CloseHandle(g_acSlideThreads[i]);
                g_acSlideThreads.erase(g_acSlideThreads.begin() + i);
            } else {
                ++i;
            }
        }
        // Reserve before starting the thread, so a successful CreateThread can
        // always be recorded and later joined by StopActionCenterAnimation.
        g_acSlideThreads.reserve(g_acSlideThreads.size() + 1);
        HANDLE thread = CreateThread(nullptr, 0, AcSlideInThread, params, 0, nullptr);
        if (!thread) {
            delete params;
            Wh_Log(L"[ac-anim] slide-in worker not created (%lu)", GetLastError());
            return false;
        }
        g_acSlideThreads.push_back(thread);
        params = nullptr; // ownership transferred to the worker
        return true;
    } catch (...) {
        delete params;
        Wh_Log(L"[ac-anim] C++ exception while starting the slide-in worker");
        return false;
    }
}

// Called during unload: cancel and join slide workers before restoring the docked
// position, so a worker cannot move the panel off-screen after the restoration.
static void StopActionCenterAnimation() noexcept {
    InterlockedIncrement(&g_acGeneration);
    g_acOpen = false;

    std::vector<HANDLE> threads;
    bool collected = false;
    bool loggedLockError = false;
    while (!collected) {
        try {
            std::lock_guard<std::mutex> lock(g_acSlideThreadsMutex);
            g_acSlideThreadsStopping = true;
            threads.swap(g_acSlideThreads);
            collected = true;
        } catch (...) {
            if (!loggedLockError) {
                Wh_Log(L"[ac-anim] C++ exception while collecting slide-in worker handles; retrying");
                loggedLockError = true;
            }
            Sleep(10);
        }
    }

    // A cloaking callback may be running the close animation synchronously. The
    // generation change above asks it to stop; wait before the final position reset.
    while (g_acActiveCloakCalls.load(std::memory_order_seq_cst) != 0) Sleep(1);

    for (HANDLE thread : threads) {
        if (!thread) continue;
        const DWORD waited = WaitForSingleObject(thread, INFINITE);
        if (waited != WAIT_OBJECT_0)
            Wh_Log(L"[ac-anim] waiting for a slide-in worker failed (%lu)", GetLastError());
        CloseHandle(thread);
    }

    if (g_acPanel && g_acHaveFinal && IsWindow(g_acPanel))
        MovePanelX(g_acPanel, g_acFinalX, g_acFinalY);
}

typedef HRESULT(WINAPI* DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
static DwmSetWindowAttribute_t DwmSetWindowAttribute_Original = nullptr;
static const DWORD kDwmwaCloak = 13;

typedef VOID(WINAPI* SetThreadpoolTimer_t)(void*, PFILETIME, DWORD, DWORD);
static SetThreadpoolTimer_t SetThreadpoolTimer_Original = nullptr;
typedef BOOL(WINAPI* SetThreadpoolTimerEx_t)(void*, PFILETIME, DWORD, DWORD);
static SetThreadpoolTimerEx_t SetThreadpoolTimerEx_Original = nullptr;

// C++ exceptions caught by these hooks, so the log says it once per kind.
static LONG g_acCppExceptionLogs = 0;

// The panel handle is only touched while it is still a live window of this process: a
// recycled handle would be a window of somebody else.
static bool AcPanelUsable(HWND hwnd) {
    return hwnd && IsWindow(hwnd) &&
           GetWindowThreadProcessId(hwnd, nullptr) == GetCurrentProcessId();
}

// The animation itself (base: the "Action Center fixes" mod by AdmXP8, v0.9). Its
// hook boundary handles C++ exceptions; no native-fault recovery is installed.
static HRESULT AcCloakImpl(HWND hwnd, DWORD attribute, LPCVOID value, DWORD size) {
    if (!g_acAnimation || g_unloading.load() || attribute != kDwmwaCloak || !value ||
        size < sizeof(int) || !IsCoreWindow(hwnd))
        return DwmSetWindowAttribute_Original(hwnd, attribute, value, size);
    if (!IsShellExperienceHostWindow(hwnd) && hwnd != g_acPanel)
        return DwmSetWindowAttribute_Original(hwnd, attribute, value, size);

    const int cloaked = *static_cast<const int*>(value);
    MONITORINFO monitor = {};
    monitor.cbSize = sizeof(monitor);
    GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitor);
    RECT rect = {};
    GetWindowRect(hwnd, &rect);

    bool known = (hwnd == g_acPanel);
    if (!known && cloaked == 0 && LooksLikeActionCenterPanel(rect, monitor)) {
        g_acPanel = hwnd;
        known = true;
        if (g_acAnimLogs++ < 6)
            Wh_Log(L"[ac-anim] Action Center panel identified (%ld,%ld,%ld,%ld)",
                   rect.left, rect.top, rect.right, rect.bottom);
    }
    if (!known) return DwmSetWindowAttribute_Original(hwnd, attribute, value, size);

    if (cloaked == 0) {                        // the panel is about to be shown
        g_acOpen = true;
        const LONG generation = InterlockedIncrement(&g_acGeneration);
        // Learn the docked position, only when the panel really is docked.
        if (rect.left < monitor.rcMonitor.right - 10 &&
            rect.right >= monitor.rcWork.right - 2) {
            g_acFinalX = rect.left;
            g_acFinalY = rect.top;
            g_acHaveFinal = true;
        }
        if (g_acSlideInMs > 0 && g_acHaveFinal && AcPanelUsable(hwnd)) {
            const int offscreen = monitor.rcMonitor.right;
            MovePanelX(hwnd, offscreen, g_acFinalY);   // parked just outside
            Sleep(30);                                 // let the window thread apply it
            const HRESULT result =
                DwmSetWindowAttribute_Original(hwnd, attribute, value, size);
            if (AcPanelUsable(hwnd)) {
                const AcSlideThreadParam params{
                    hwnd, offscreen, g_acFinalX, g_acFinalY, g_acSlideInMs, generation };
                if (StartAcSlideInThread(params)) {
                    if (g_acAnimLogs++ < 6)
                        Wh_Log(L"[ac-anim] slide-in %d -> %d (%d ms)", offscreen, g_acFinalX,
                               g_acSlideInMs);
                } else {
                    // The panel was parked before the worker was requested. If the
                    // worker cannot be started (especially during unload), restore it.
                    MovePanelX(hwnd, g_acFinalX, g_acFinalY);
                }
            }
            return result;
        }
        return DwmSetWindowAttribute_Original(hwnd, attribute, value, size);
    }

    // The panel is about to be hidden: slide it out, then let the shell cloak it.
    g_acOpen = false;
    const LONG generation = InterlockedIncrement(&g_acGeneration);
    if (g_acSlideOutMs > 0 && g_acHaveFinal && AcPanelUsable(hwnd) &&
        rect.left < monitor.rcMonitor.right) {
        if (g_acAnimLogs++ < 6)
            Wh_Log(L"[ac-anim] slide-out %ld -> %ld (%d ms)", rect.left,
                   monitor.rcMonitor.right, g_acSlideOutMs);
        RunAcSlide(hwnd, rect.left, monitor.rcMonitor.right, g_acFinalY, g_acSlideOutMs,
                   generation);
    }
    const HRESULT result = DwmSetWindowAttribute_Original(hwnd, attribute, value, size);
    // Ready for the next time: only while the panel still is a live window.
    if (g_acHaveFinal && AcPanelUsable(hwnd)) MovePanelX(hwnd, g_acFinalX, g_acFinalY);
    return result;
}

struct AcCloakCall {
    HWND hwnd;
    DWORD attribute;
    LPCVOID value;
    DWORD size;
    HRESULT result;
};

static void AcCloakEntry(void* parameter) {
    auto* call = static_cast<AcCloakCall*>(parameter);
    call->result = AcCloakImpl(call->hwnd, call->attribute, call->value, call->size);
}

static HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd, DWORD attribute, LPCVOID value,
                                                DWORD size) {
    if (!DwmSetWindowAttribute_Original) return E_FAIL;   // hook in place, original unknown
    if (g_unloading.load(std::memory_order_seq_cst))
        return DwmSetWindowAttribute_Original(hwnd, attribute, value, size);
    g_acActiveCloakCalls.fetch_add(1, std::memory_order_seq_cst);
    struct ActiveCloakCallScope {
        ~ActiveCloakCallScope() {
            g_acActiveCloakCalls.fetch_sub(1, std::memory_order_seq_cst);
        }
    } activeCloakCall;
    if (g_unloading.load(std::memory_order_seq_cst))
        return DwmSetWindowAttribute_Original(hwnd, attribute, value, size);
    // 1.3.9, EXPERIMENTAL and off by default (ExperimentalSquareFlyoutCorners; it does not work,
    // see g_squareFlyoutCorners): square corners for the flyouts of this process. Windows 11 rounds every top-level
    // window through DWM; the documented switch is DWMWA_WINDOW_CORNER_PREFERENCE (33) with
    // DWMWCP_DONOTROUND (1). It is set when a CoreWindow of ShellExperienceHost.exe is about to
    // be shown (cloak value 0), once per window handle, on the thread that shows it. The window
    // is the one the network flyout lives in, as well as the other flyouts of this shell.
    if (g_squareFlyoutCorners && attribute == kDwmwaCloak && value && size >= sizeof(int) &&
        *static_cast<const int*>(value) == 0 && IsCoreWindow(hwnd) &&
        ImageNameIs(g_realExePath, L"ShellExperienceHost.exe")) {
        try {
            static HWND s_squared[16] = {};
            static int s_next = 0;
            bool already = false;
            for (HWND w : s_squared) if (w == hwnd) { already = true; break; }
            if (!already) {
                const int doNotRound = 1;   // DWMWCP_DONOTROUND
                const HRESULT hrCorner =
                    DwmSetWindowAttribute_Original(hwnd, 33, &doNotRound, sizeof(doNotRound));
                s_squared[s_next++ % 16] = hwnd;
                static LONG s_cornerLogs = 0;
                if (InterlockedIncrement(&s_cornerLogs) <= 4)
                    Wh_Log(L"[networkux] square window corners requested (DWMWCP_DONOTROUND) "
                           L"for 0x%p: 0x%08X", (void*)hwnd, (unsigned)hrCorner);
            }
        } catch (...) {
        }
    }
    AcCloakCall call = { hwnd, attribute, value, size, E_FAIL };
    DWORD fault = 0;
    if (!CppGuard::RunGuarded(L"ac-anim cloak", AcCloakEntry, &call, &fault)) {
        if (InterlockedIncrement(&g_acCppExceptionLogs) <= 3)
            Wh_Log(L"[ac-anim] a C++ exception was caught (0x%08X) while the panel was being shown",
                   fault);
        return DwmSetWindowAttribute_Original(hwnd, attribute, value, size);
    }
    return call.result;
}

// 1.3.9: the cloak hook above never fired for the network flyout in the user's log (no
// "square window corners" line), so the corner preference is also applied directly: every
// top-level window of this process gets DWMWA_WINDOW_CORNER_PREFERENCE = DWMWCP_DONOTROUND.
// It is called when the flyout page is being built (CreateStringReferenceShim) and once at
// start. The classes and titles of the windows found are logged, so the log says which window
// the flyout really lives in and what DWM answered.
static BOOL CALLBACK SquareWindowProc(HWND hwnd, LPARAM lParam) {
    try {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != GetCurrentProcessId()) return TRUE;
        const int doNotRound = 1;   // DWMWCP_DONOTROUND
        const HRESULT hr = DwmSetWindowAttribute_Original
            ? DwmSetWindowAttribute_Original(hwnd, 33, &doNotRound, sizeof(doNotRound))
            : DwmSetWindowAttribute(hwnd, 33, &doNotRound, sizeof(doNotRound));
        int* counter = reinterpret_cast<int*>(lParam);
        if (counter) ++*counter;
        static LONG s_logs = 0;
        if (InterlockedIncrement(&s_logs) <= 12) {
            wchar_t cls[64] = {};
            wchar_t title[64] = {};
            GetClassNameW(hwnd, cls, _countof(cls));
            GetWindowTextW(hwnd, title, _countof(title));
            Wh_Log(L"[networkux] square corners: window 0x%p class '%s' title '%s' visible %d "
                   L"-> 0x%08X", (void*)hwnd, cls, title, IsWindowVisible(hwnd) ? 1 : 0,
                   (unsigned)hr);
        }
    } catch (...) {
    }
    return TRUE;
}

static void SquareShellWindowsNow() noexcept {
    if (!g_squareFlyoutCorners) return;   // experimental, off by default (see g_squareFlyoutCorners)
    try {
        static ULONGLONG s_last = 0;
        const ULONGLONG now = GetTickCount64();
        if (now - s_last < 400) return;
        s_last = now;
        int count = 0;
        EnumWindows(SquareWindowProc, reinterpret_cast<LPARAM>(&count));
        static LONG s_summary = 0;
        if (count == 0 && InterlockedIncrement(&s_summary) <= 3)
            Wh_Log(L"[networkux] square corners: no top-level window of this process yet");
    } catch (...) {
    }
}

// The close delay: the shell sets a ~2000 ms threadpool timer while the pointer is away
// from the panel; with this hook it becomes the configured value. Both entry points are
// taken by address, so the module does not depend on the declarations of these two.
static BOOL AcCloseTimerImpl(bool ex, void* timer, PFILETIME due, DWORD period,
                             DWORD windowLength) {
    if (g_acAnimation && !g_unloading.load() && IsCloseTimer(due)) {
        FILETIME shorter = RelativeDue(g_acCloseDelayMs);
        if (g_acAnimLogs++ < 6)
            Wh_Log(L"[ac-anim] close timer replaced: ~2000 ms -> %d ms", g_acCloseDelayMs);
        if (ex) return SetThreadpoolTimerEx_Original(timer, &shorter, period, windowLength);
        SetThreadpoolTimer_Original(timer, &shorter, period, windowLength);
        return TRUE;
    }
    if (ex) return SetThreadpoolTimerEx_Original(timer, due, period, windowLength);
    SetThreadpoolTimer_Original(timer, due, period, windowLength);
    return TRUE;
}

struct AcTimerCall {
    void* timer;
    PFILETIME due;
    DWORD period;
    DWORD windowLength;
    bool ex;
    BOOL result;
};

static void AcTimerEntry(void* parameter) {
    auto* call = static_cast<AcTimerCall*>(parameter);
    call->result = AcCloseTimerImpl(call->ex, call->timer, call->due, call->period,
                                    call->windowLength);
}

static VOID WINAPI SetThreadpoolTimer_Hook(void* timer, PFILETIME due, DWORD period,
                                           DWORD windowLength) {
    if (!SetThreadpoolTimer_Original) return;
    AcTimerCall call = { timer, due, period, windowLength, false, TRUE };
    DWORD fault = 0;
    if (!CppGuard::RunGuarded(L"ac-anim timer", AcTimerEntry, &call, &fault))
        SetThreadpoolTimer_Original(timer, due, period, windowLength);
}

static BOOL WINAPI SetThreadpoolTimerEx_Hook(void* timer, PFILETIME due, DWORD period,
                                             DWORD windowLength) {
    if (!SetThreadpoolTimerEx_Original) return FALSE;
    AcTimerCall call = { timer, due, period, windowLength, true, TRUE };
    DWORD fault = 0;
    if (!CppGuard::RunGuarded(L"ac-anim timer", AcTimerEntry, &call, &fault))
        return SetThreadpoolTimerEx_Original(timer, due, period, windowLength);
    return call.result;
}

// Installed once, from both processes this mod is loaded in. Every hook checks the
// setting at each call, so the animation can be switched while the shell runs.
static void InstallActionCenterAnimation() {
    if (g_acAnimationHooksInstalled) return;
    g_acAnimationHooksInstalled = true;
    try {
        HMODULE dwmapi = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (dwmapi) {
            void* target = reinterpret_cast<void*>(GetProcAddress(dwmapi, "DwmSetWindowAttribute"));
            if (target) {
                if (!Wh_SetFunctionHook(target, (void*)DwmSetWindowAttribute_Hook,
                                        (void**)&DwmSetWindowAttribute_Original))
                    Wh_Log(L"[ac-anim] the cloak hook could not be installed: no slide");
            } else {
                Wh_Log(L"[ac-anim] DwmSetWindowAttribute not found: no slide");
            }
        } else {
            Wh_Log(L"[ac-anim] dwmapi.dll unavailable: no slide");
        }
        HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
        if (kernel32) {
            void* timer = reinterpret_cast<void*>(GetProcAddress(kernel32, "SetThreadpoolTimer"));
            void* timerEx =
                reinterpret_cast<void*>(GetProcAddress(kernel32, "SetThreadpoolTimerEx"));
            if (timer) {
                if (!Wh_SetFunctionHook(timer, (void*)SetThreadpoolTimer_Hook,
                                        (void**)&SetThreadpoolTimer_Original))
                    Wh_Log(L"[ac-anim] the close-timer hook could not be installed");
            }
            if (timerEx) {
                if (!Wh_SetFunctionHook(timerEx, (void*)SetThreadpoolTimerEx_Hook,
                                        (void**)&SetThreadpoolTimerEx_Original))
                    Wh_Log(L"[ac-anim] the close-timer hook (Ex) could not be installed");
            }
        }
        Wh_Log(L"[ac-anim] ready (close delay %d ms, slide in %d ms, out %d ms)",
               g_acCloseDelayMs, g_acSlideInMs, g_acSlideOutMs);
    } catch (...) {
        Wh_Log(L"[ac-anim] setup exception: the animation stays off");
    }
}

// ---------------------------------------------------------------------------
// This module's own thread
//
// Every hook of this module is installed from here and this thread does nothing else than
// pump messages and tick: a shell hook is never blocked by the tray work, and the shell's
// own threads are never held up (the same shape the Win+X and Alt+Tab mods of this set use).
// ---------------------------------------------------------------------------
static HANDLE g_stopEvent = nullptr;
static HANDLE g_servicesThread = nullptr;
static DWORD g_trayThreadId = 0;

static void InstallActionCenterButtonHook() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) return;
    void* target = reinterpret_cast<void*>(GetProcAddress(user32, "CreateWindowExW"));
    if (!target) return;
    if (!Wh_SetFunctionHook(target, (void*)CreateWindowExW_Hook,
                            (void**)&CreateWindowExW_Original)) {
        Wh_Log(L"[actioncenter] the button hook could not be installed: the button flag is "
               L"still kept by the monitor");
    } else {
        Wh_Log(L"[actioncenter] button hook installed (class %s, flag at +%u)",
               kActionCenterButtonClass, (unsigned)kActionCenterButtonFlagOffset);
    }
}

// The policy hooks are tried at most three times. A build (or a process) where they
// cannot be installed is reported once and then left alone: retrying at every tick wrote
// the same line to the log every 500 ms for the whole lifetime of the process.
// 1.3.1: the policy hooks moved into Wh_ModInit (see the note there). The retry ladder that
// used to live here registered them from the tray thread, that is after the engine had
// already applied the hook queue: the hooks stayed queued and inert, and the registration ran
// in parallel with the engine's own hook operations. It is gone on purpose.

static void TrayThreadWork(bool firstRun) {
    if (firstRun) {
        // Which process this is, in the terms the tray code uses: the private Windows 10
        // shell. Without this the redirect decisions answer "not our shell" and the tray
        // support never runs.
        NativeUi::registryProcess = true;
        NativeUi::explorerProcess = true;
        NativeUi::privateExplorer = IsPrivateExplorerProcess();
        Wh_Log(L"[flyout] process role: private shell=%d", NativeUi::privateExplorer ? 1 : 0);
        Wh_Log(L"[flyout] the Windows 10 tray is being restored in this shell "
               L"(tray modules=%s, AC button=%s)",
               g_cfg.provideTrayDlls ? L"on" : L"off",
               g_showActionCenterButton ? L"on" : L"off");
    }

    // The DLLs come from this mod: downloaded if missing, verified against the pinned
    // SHA-256 (and signature) if present. Nothing is assumed to be already there.
    if (g_cfg.provideTrayDlls) {
        PrepareTrayStore();
    } else {
        Wh_Log(L"[store] the tray modules are switched off by the settings");
    }

    if (!g_traySupportInstalled) InstallTraySupport();

    // 1.3.2: the click on the network icon is taken over at the icon itself, and this
    // thread registers nothing. The interception is a window subclass, installed by
    // ArmNetworkIconClickSubclass further down: it runs on this thread, so no window is ever
    // created or waited for inside a window procedure of the shell. The two entry points of
    // the click are registered in Wh_ModInit together with every other hook of this mod (see
    // the note there): a registration made here comes after Wh_ModInit returned, so the
    // engine has already applied the hook queue and the hook would stay inert, and doing it
    // from this thread is also what made explorer.exe crash whenever the mod was enabled or
    // disabled, because the hook queue is then operated on by two threads at once.

    // The clock and the show desktop button: if the shell shows no menu of its own,
    // the Windows 10 one of this mod answers the right click.
    ArmTrayMenuSubclass();

    // The Action Center policy of Windows 10, served in memory: no registry write.
    // 1.3.1: the policy hooks themselves are registered in Wh_ModInit (see the note there);
    // this thread never touches the hook queue. Only the process flag is kept alive here, so
    // that it follows a settings change.
    if (g_cfg.fixNotificationsCrash) NativeUi::actionCenterPolicyProcess = true;

    RetryTrayModulesIfNeeded();
    ActionCenterButtonTick();

    // The three steps the reference mod runs on every tray tick while the private shell
    // is up. The last one is the ladder that FORCES the network icon: the SSO of
    // pnidui.dll is nudged first, then the state fallback is given a chance, and only
    // after the waiting time does the mod register the icon itself (and verify it).
    // Without these three calls the support is "active" but nothing is ever registered,
    // which is exactly what the log showed.
    if (IsLegacyShellProcess()) {
        try {
            EnsureNativeNetworkTraySso();
            NetworkIconFallbackTick();
            NetworkTrayForce::Tick();
        } catch (...) {
            Wh_Log(L"[tray] exception in the network icon step: the shell is unaffected");
        }
    }

    // 1.3.1 (crash on enable/disable): no hook is registered from this thread any more, so
    // there is nothing to apply here either. Calling Wh_ApplyHookOperations from a worker
    // while the engine is initializing or unloading the mod is what made Explorer die on
    // every enable and every disable: the queue is applied by the engine itself at the end
    // of Wh_ModInit, where every Wh_SetFunctionHook of this mod now lives.
    if (g_logTrayActivity && firstRun)
        Wh_Log(L"[tray] per-operation logging is on: every load and menu operation is logged");
}

static DWORD WINAPI FlyoutServicesThread(LPVOID) {
    g_trayThreadId = GetCurrentThreadId();
    MSG queueInit = {};
    PeekMessageW(&queueInit, nullptr, 0, 0, PM_NOREMOVE);

    LoadFlyoutSettings();
    try {
        TrayThreadWork(true);
    } catch (...) {
        Wh_Log(L"[flyout] exception in the tray thread: the shell is unaffected");
    }

    for (;;) {
        if (g_unloading.load(std::memory_order_acquire)) break;
        // 200 ms: the period of the tray ticks is unchanged, but unloading does not
        // wait for a long timeout when the stop event is already set.
        if (WaitForSingleObject(g_stopEvent, 200) == WAIT_OBJECT_0) break;
        try {
            TrayThreadWork(false);
        } catch (...) {
            Wh_Log(L"[flyout] exception in the tray thread: the shell is unaffected");
        }
    }

    // 1.3.1 - TEARDOWN ON THIS THREAD, WHILE THE MODULE IS STILL LOADED.
    //
    // Everything the tray work created belongs to this thread: the forced network icon, the
    // hidden owner window and its thread, the fallback icon, and the subclasses installed on
    // the shell's clock / show-desktop windows. 1.3.0 retired the first three from
    // Wh_ModUninit, that is on the engine's thread and after Windhawk had already removed the
    // hooks, and it never removed the subclasses at all: the window procedure of those shell
    // windows still pointed into this module when it was unloaded, so the next message to the
    // clock (which repaints every second) jumped into freed code and took explorer.exe with it.
    // No window procedure of the shell may outlive this module.
    // La presa del clic sull'icona e' una sottoclasse di una finestra della shell: se
    // restasse, il suo window procedure punterebbe dentro questo modulo quando viene
    // scaricato. Si toglie per prima.
    NetworkTrayForce::DisarmNetworkIconClickSubclass();
    NetworkTrayForce::DisarmBatteryIconClickSubclass();
    NetworkIconFallbackShutdown();
    NetworkTrayForce::Shutdown();
    for (int i = g_trayMenuTargetCount - 1; i >= 0; i--) {
        HWND wnd = g_trayMenuTargets[i].wnd;
        if (wnd) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(wnd, TrayMenuSubclassProc);
            Wh_Log(L"[menu] subclass removed from 0x%p while unloading the mod", (void*)wnd);
        }
        UntrackMenuTarget(i);
    }

    return 0;
}

// ===========================================================================
// 1.3.4 - the process that draws the flyouts: the Windows 10 template set.
//
// The network flyout and the battery flyout are not drawn by Explorer: ShellExperienceHost.exe
// draws them. That process starts, the mod asks this shell for the Windows 10 flyout, the
// window appears and is taken down again at once (a new ShellExperienceHost.exe for every
// click) - and the battery one does not even get that far, because its flyout makes that
// process go away while it is being built.
//
// The reason is in Windows.UI.QuickActions.dll, the module this shell builds the flyout with:
// from build 25951 on it enters the flyout through the new "Control Center" template set, which
// the Windows 10 flyouts cannot be built with. ExplorerPatcher fixes exactly this on these
// builds ("Fix battery flyout crashing on 25951+") by making the shell load the older
// QuickActions template set instead: five bytes are turned into NOPs, eight bytes are copied
// from the older template loader and one call is pointed at it. This block does the same, on
// the module the shell has loaded, the moment that module is loaded.
//
// The module is NOT loaded, and cannot be loaded, when the mod starts: a LoadLibrary of it from
// the mod init fails (the log of the previous round says "not available (error 1114)"). So the
// mod does not load it: it watches the loads of this process (LdrLoadDll) and patches the
// module as soon as it appears, which is before the shell has built anything with it.
//
// Nothing else is touched: only those bytes of that one module, in memory, in this process.
// If the patterns are not found (a build this code does not know) nothing at all is written
// and the log says so.
// ===========================================================================
// 1.3.5: il blocco del flyout di rete (definito dopo questo, prima di Wh_ModInit) vuole
// sapere quando la shell carica NetworkUX.dll, la pagina di quel flyout; e il blocco dei
// template, che sta qui sotto, la usa prima che quel namespace venga definito. La
// dichiarazione e' quindi a livello di file, come quella del flyout della batteria.
namespace NetworkUxHostPatch { void OnNetworkUxLoaded(void* module) noexcept; }

// ===========================================================================
// 1.3.5 - RAII: le risorse che questo mod prende in prestito e restituisce.
//
// Il mod scrive in memoria, in due punti, e in tutti e due i casi la scrittura e'
// una parentesi: la pagina torna come era quando la parentesi si chiude.
//
//   * ScopedWriteProtect: rende scrivibile una pagina e la rimette com'era quando
//     l'oggetto esce di scena - anche se in mezzo si esce con un return, o se il
//     codice alza un'eccezione C++ (che il chiamante intercetta piu' in alto);
//   * ScopedImportRedirect: manda una voce della tabella delle importazioni di un
//     modulo a una funzione di questo mod, e la rimette com'era allo stesso modo.
//
// Stanno davanti ai due blocchi che li prendono in prestito (il set di template
// Windows 10 del processo dei flyout e il flyout di rete), perche' entrambi ne
// hanno bisogno e nessuno dei due deve sapere come si sblocca una pagina.
// ===========================================================================
class ScopedWriteProtect {
public:
    ScopedWriteProtect(void* address, size_t size) noexcept {
        if (!address || !size) return;
        m_address = address;
        m_size = size;
        m_ok = VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &m_previous) != FALSE;
    }
    ~ScopedWriteProtect() { Restore(); }
    ScopedWriteProtect(const ScopedWriteProtect&) = delete;
    ScopedWriteProtect& operator=(const ScopedWriteProtect&) = delete;
    explicit operator bool() const noexcept { return m_ok; }
    void Restore() noexcept {
        if (!m_ok) return;
        DWORD ignored = 0;
        VirtualProtect(m_address, m_size, m_previous, &ignored);
        m_ok = false;
    }
private:
    void* m_address = nullptr;
    size_t m_size = 0;
    DWORD m_previous = 0;
    bool m_ok = false;
};

class ScopedImportRedirect {
public:
    ScopedImportRedirect() = default;
    ~ScopedImportRedirect() { Restore(); }
    ScopedImportRedirect(const ScopedImportRedirect&) = delete;
    ScopedImportRedirect& operator=(const ScopedImportRedirect&) = delete;

    // Una volta sola: se la voce e' gia' nostra, o se manca, non si tocca nulla.
    bool Redirect(void** slot, void* replacement) noexcept {
        if (m_redirected || !slot || !replacement) return false;
        m_slot = slot;
        m_previous = *slot;
        ScopedWriteProtect writable(slot, sizeof(void*));
        if (!writable) return false;
        *slot = replacement;
        m_redirected = true;
        return true;
    }

    bool redirected() const noexcept { return m_redirected; }

    // 1.3.8 (C): la voce che questo oggetto tiene e' ancora quella? Serve a chi deve decidere
    // DAL CONTENUTO (la voce punta gia' alla funzione di questo mod?) e non da un flag.
    bool owns(void** slot) const noexcept { return m_redirected && m_slot == slot; }

    // 1.3.8: lascia andare la voce SENZA riscriverla. Si usa quando la voce presa in prestito
    // stava in una copia del modulo che non c'e' piu' (la pagina era stata scaricata e
    // ricaricata): riscriverla sarebbe un accesso a memoria liberata, quindi il mod non la tocca
    // e prende in prestito la voce della copia nuova. Con il pin di questa revisione (A) il caso
    // non dovrebbe presentarsi; se il pin non riesce, pero', l'oggetto non deve restare con un
    // indirizzo che non gli appartiene.
    void Abandon() noexcept {
        m_slot = nullptr;
        m_previous = nullptr;
        m_redirected = false;
    }

    // La voce torna al valore che aveva: la chiamata e' nostra solo finche' il mod
    // e' caricato, come per ogni altra cosa che questo mod prende in prestito.
    void Restore() noexcept {
        if (!m_redirected) return;
        m_redirected = false;
        if (!m_slot) return;
        ScopedWriteProtect writable(m_slot, sizeof(void*));
        if (!writable) return;
        *m_slot = m_previous;
    }

private:
    void** m_slot = nullptr;
    void* m_previous = nullptr;
    bool m_redirected = false;
};
namespace FlyoutHostPatch {

static std::atomic<bool> g_patched{false};
static std::atomic<int> g_logs{0};
// 1.3.8 (A): il modulo e' stato fissato in memoria (GetModuleHandleEx_W con
// GET_MODULE_HANDLE_EX_FLAG_PIN). Una volta per processo: il pin non si rifa' e non si annulla.
static std::atomic<bool> g_pinned{false};

// 1.3.5: il processo ha davvero preso il set di template di Windows 10? Lo chiede il blocco
// del flyout di rete (sotto, chunk-netux.inc): il nome del pulsante di Windows 10 ha senso
// solo su quel set, altrimenti quei pulsanti non si costruiscono piu' (la nota di
// ExplorerPatcher accanto alla stessa correzione).
static bool TemplatesPatched() noexcept { return g_patched.load(std::memory_order_relaxed); }

// ---- ricerca nei byte: 'x' = byte esatto, '?' = qualunque -------------------
static bool MaskMatches(const unsigned char* at, const unsigned char* pattern,
                        const char* mask) noexcept {
    for (size_t i = 0; mask[i]; ++i)
        if (mask[i] == 'x' && at[i] != pattern[i]) return false;
    return true;
}

static unsigned char* FindPattern(unsigned char* begin, size_t size,
                                  const unsigned char* pattern, const char* mask) noexcept {
    if (!begin || !pattern || !mask) return nullptr;
    const size_t length = strlen(mask);
    if (!length || size < length) return nullptr;
    for (size_t i = 0; i + length <= size; ++i)
        if (MaskMatches(begin + i, pattern, mask)) return begin + i;
    return nullptr;
}

static unsigned char* TextSectionOf(HMODULE module, size_t* size) noexcept {
    if (size) *size = 0;
    if (!module) return nullptr;
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(
        reinterpret_cast<unsigned char*>(module) + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return nullptr;
    auto* section = IMAGE_FIRST_SECTION(nt);
    for (unsigned int i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section) {
        if (!section->SizeOfRawData) continue;
        const char* name = reinterpret_cast<const char*>(section->Name);
        if ((section->Characteristics & IMAGE_SCN_CNT_CODE) || strncmp(name, ".text", 5) == 0) {
            if (size) *size = section->SizeOfRawData;
            return reinterpret_cast<unsigned char*>(module) + section->VirtualAddress;
        }
    }
    return nullptr;
}

// ---- il ricambio dei template (ExplorerPatcher, build 25951 e successive) ---
//
// The older template loader ("ref new QuickActions::QuickActionTemplates()"):
//   48 89 45 50 BA 90 00 00 00 8D 4A E8 FF 15 ?? ?? ?? ?? 48 89 45 58 48 8B C8 E8 ?? ?? ?? ?? 48 8B F0
// The five bytes of the getter call that follows are read from here; the
// place that builds the flyout with the new toolset, and that is changed:
//   ... the 5 bytes of "call LoadComponent" become NOP, the 8 bytes of the "mov edx / lea rcx"
//   pair are copied from the older loader (the size the template needs), and the last call is
//   pointed at the older getter.
static const unsigned char kSourcePattern[] = {
    0x48, 0x89, 0x45, 0x50, 0xBA, 0x90, 0x00, 0x00, 0x00, 0x8D, 0x4A, 0xE8, 0xFF, 0x15,
    0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0x45, 0x58, 0x48, 0x8B, 0xC8, 0xE8, 0x00, 0x00,
    0x00, 0x00, 0x48, 0x8B, 0xF0,
};
static const char kSourceMask[] = "xxxxxxxxxxxxxx????xxxxxxxx????xxx";

static const unsigned char kTargetPattern[] = {
    0x48, 0x8B, 0xD0, 0x48, 0x8B, 0xCB, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x90, 0x4D, 0x85,
    0x00, 0x74, 0x10, 0x49, 0x8B, 0x00, 0x49, 0x8B, 0x00, 0x48, 0x8B, 0x40, 0x10, 0xE8,
    0x00, 0x00, 0x00, 0x00, 0x90, 0x49, 0x8B, 0xCC,
    0xE8, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0xD3, 0x48, 0x8B, 0xCE, 0xE8, 0x00, 0x00,
    0x00, 0x00, 0xBA, 0xB8, 0x00, 0x00, 0x00, 0x8D, 0x4A, 0xE8, 0xFF, 0x15, 0x00, 0x00,
    0x00, 0x00, 0x48, 0x89, 0x45, 0x00,
    0x48, 0x8B, 0xC8, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x8B,
};
static const char kTargetMask[] = "xxxxxxx????xxx?xxxx?xx?xxxxx????xxxx"
                                  "x????xxxxxxx????xxxxxxxxxx????xxx?"
                                  "xxxx????xx";

// 1.3.8 (B) - la stessa maschera con i gruppi che questa patch riscrive lasciati liberi:
// indici 6-10 (i cinque NOP al posto della "call LoadComponent": nel file 6 e' 'x' e 7-10 sono
// gia' '?'), 52-59 (gli otto byte del caricatore vecchio: nel file 52-61 sono 'x', e l'unico
// byte che cambia davvero e' il 53, 0xB8 -> 0x90) e 74-77 (rel32, gia' '?'). Serve a riconoscere
// DAL CONTENUTO un modulo che porta gia' la patch, invece di fidarsi dell'indirizzo di base
// (ASLR puo' riassegnare a una copia nuova lo stesso indirizzo) o di un flag (che dice "l'ho
// fatto io", non "il modulo in memoria e' riscritto"). La ricerca va fatta PRIMA con la maschera
// intatta: un sito gia' riscritto non compare li'.
static const char kTargetMaskRelaxed[] = "xxxxxx?????xxx?xxxx?xx?xxxxx????xxxx"
                                         "x????xxxxxxx????????????xx????xxx?"
                                         "xxxx????xx";

// Le due maschere descrivono lo stesso numero di byte del pattern: se una delle due righe non
// regge, il file non compila.
static_assert(sizeof(kTargetPattern) == sizeof(kTargetMask) - 1, "kTargetMask: 80 byte");
static_assert(sizeof(kTargetPattern) == sizeof(kTargetMaskRelaxed) - 1,
              "kTargetMaskRelaxed: 80 byte");
static_assert(sizeof(kTargetMaskRelaxed) == sizeof(kTargetMask), "le due maschere: 81 byte");

// ===========================================================================
// 1.3.8 (A) - IL MODULO FISSATO IN MEMORIA (documentazione Microsoft).
//
// Una DLL COM puo' essere scaricata quando DllCanUnloadNow risponde S_OK (nessun oggetto e'
// piu' in uso) e il ritardo di CoFreeUnusedLibrariesEx e' scaduto: dieci minuti per
// impostazione predefinita, trattati come zero per i componenti apartment. Se
// Windows.UI.QuickActions.dll (o NetworkUX.dll) venisse scaricata e ricaricata mentre il mod e'
// caricato, la copia nuova - mappata di nuovo dal file su disco - non porterebbe ne' i byte
// riscritti qui ne' la voce della tabella delle importazioni presa in prestito.
//
// La risposta documentata e' GetModuleHandleExW con GET_MODULE_HANDLE_EX_FLAG_PIN: "the module
// stays loaded until the process terminates, regardless of the number of calls to FreeLibrary",
// e il conteggio dei riferimenti non viene incrementato (nessun FreeLibrary da fare: qui non se
// ne fa nessuno, nemmeno alla disattivazione). GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS dice che
// il primo parametro e' un indirizzo dentro il modulo: si passa la base del modulo, che un
// indirizzo dentro il modulo lo e'.
//
// Il pin NON e' reversibile e vive solo nel processo in cui e' stato fatto, cioe'
// ShellExperienceHost.exe (il processo che disegna i flyout): finche' quel processo vive i due
// moduli restano caricati, e se il mod viene disattivato la voce della tabella delle
// importazioni torna com'era (ScopedImportRedirect) ma i byte riscritti e il pin restano. Non
// tocca altri processi e non scrive nulla su disco. Se la chiamata non riesce il mod lo scrive e
// prosegue: la patch vale finche' il modulo resta caricato, e il riconoscimento dal contenuto
// (B) copre il caso di una copia nuova.
// ===========================================================================
static void PinModuleOrLog(HMODULE module, const wchar_t* tag, const wchar_t* name,
                           std::atomic<bool>& alreadyPinned) noexcept {
    try {
        if (!module || alreadyPinned.load(std::memory_order_relaxed)) return;
        HMODULE pinned = nullptr;
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN |
                                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                                reinterpret_cast<LPCWSTR>(module), &pinned) ||
            !pinned) {
            Wh_Log(L"%s %s (0x%p) could not be pinned (error %lu): if the shell unloads it, a new "
                   L"copy would not carry what was done here", tag, name, (void*)module,
                   GetLastError());
            return;
        }
        alreadyPinned.store(true, std::memory_order_relaxed);
        Wh_Log(L"%s %s (0x%p) is pinned: it stays loaded until ShellExperienceHost.exe ends (the "
               L"pin is not reversible and is not undone when the mod is disabled)", tag, name,
               (void*)module);
    } catch (...) {
        Wh_Log(L"%s exception while pinning a module", tag);
    }
}

// ===========================================================================
// 1.3.8 (B) - LA FIRMA DELLE TRE SCRITTURE, LETTA NEL MODULO (osservazione empirica: non e'
// una proprieta' documentata del modulo). I cinque byte 6..10 sono NOP (la "call LoadComponent"
// non c'e' piu') e gli otto byte 52..59 sono quelli copiati dal caricatore vecchio
// (source + 4): nessun altro le scrive. La rel32 74..77 non si controlla, perche' senza il
// getter non si puo' ricalcolare e le due scritture qui sopra bastano.
// ===========================================================================
static bool SiteLooksPatched(const unsigned char* target, const unsigned char* source) noexcept {
    if (!target || !source) return false;
    for (int i = 0; i < 5; ++i)
        if (target[6 + i] != 0x90) return false;
    return memcmp(target + 52, source + 4, 8) == 0;
}

static void PatchQuickActionsTemplates(HMODULE module) noexcept {
    try {
        if (!module) return;

        // 1.3.8 (C): il fermo "se g_patched esci subito" non c'e' piu': quel flag dice "l'ho
        // fatto io", non "il modulo in memoria e' riscritto", e dopo un ricaricamento del mod i
        // flag sono nuovi mentre i byte restano scritti. Si guarda il modulo (prima la forma
        // intatta, poi quella gia' scritta) e in nessun caso l'indirizzo di base.
        size_t size = 0;
        unsigned char* text = TextSectionOf(module, &size);
        if (!text) {
            Wh_Log(L"[flyout-host] the code of Windows.UI.QuickActions.dll was not found: "
                   L"nothing is patched");
            return;
        }

        // La copia vecchia del caricatore di template non viene mai riscritta da nessuno dei
        // due lati: se non c'e', la build non e' quella che questo codice conosce.
        unsigned char* source = FindPattern(text, size, kSourcePattern, kSourceMask);
        unsigned char* getter = nullptr;
        if (source) {
            getter = source + 25;                                   // the call to the getter
            getter += 5 + *reinterpret_cast<int*>(getter + 1);       // follow its rel32
        }
        if (!source || !getter) {
            Wh_Log(L"[flyout-host] Windows.UI.QuickActions.dll (0x%p): the loader of the Windows 10 "
                   L"template set was not found (source %s): nothing is patched", (void*)module,
                   source ? L"yes" : L"no");
            return;
        }

        // 1.3.8: prima la FORMA INTATTA. La maschera intera vuole la 0xE8 all'indice 6 (l'inizio
        // della "call LoadComponent"): un sito gia' riscritto non compare in questa ricerca,
        // quindi le due strade non si confondono.
        unsigned char* target = FindPattern(text, size, kTargetPattern, kTargetMask);
        if (!target) {
            // 1.3.8 (B) - POI LA FORMA CHE PORTA GIA' LA PATCH: e' il caso del ricaricamento
            // del mod. Windhawk ricarica il mod anche nei processi gia' avviati (qui
            // ShellExperienceHost.exe); Wh_ModBeforeUninit NON rimette i byte originali (e non li
            // rimette di proposito: rimetterli mentre la shell disegna un flyout lo farebbe
            // costruire di nuovo col set di Windows 11 e cadere); i flag di questa copia del mod
            // sono nuovi e valgono zero. Senza questa strada TemplatesPatched() resterebbe falso,
            // il nome del pulsante di Windows 10 non verrebbe mai chiesto e il flyout sarebbe
            // costruito a meta' (pulsanti inerti).
            unsigned char* already = FindPattern(text, size, kTargetPattern, kTargetMaskRelaxed);
            if (already && SiteLooksPatched(already, source)) {
                g_patched.store(true, std::memory_order_relaxed);
                Wh_Log(L"[flyout-host] Windows.UI.QuickActions.dll (0x%p) already carries the "
                       L"Windows 10 template set (it survived a reload of the mod, or the shell "
                       L"loaded it again): nothing is written, the state is taken as it is",
                       (void*)module);
                PinModuleOrLog(module, L"[flyout-host]", L"Windows.UI.QuickActions.dll", g_pinned);
                return;
            }
            if (already) {
                // Il sito c'e' ma i suoi byte non sono ne' quelli interi ne' quelli di questa
                // patch: non si sa cosa sia quella copia, quindi non si scrive e non si fissa.
                Wh_Log(L"[flyout-host] Windows.UI.QuickActions.dll (0x%p): the site is there but "
                       L"its bytes are none of the two forms this code knows: nothing is written",
                       (void*)module);
                return;
            }
            Wh_Log(L"[flyout-host] the Windows 10 template set of Windows.UI.QuickActions.dll "
                   L"(0x%p) was not found (source yes, target no): nothing is patched",
                   (void*)module);
            return;
        }

        // 1.3.5: RAII. La pagina torna eseguibile e leggibile com'era quando questo oggetto
        // esce di scena, e la scrittura qui sotto non ha piu' una coppia di VirtualProtect
        // scritta a mano che qualcuno possa dimenticare di chiudere (anche uscendo prima, con
        // return, o su un'eccezione C++ intercettata piu' in alto).
        ScopedWriteProtect writable(target, 80);
        if (!writable) {
            Wh_Log(L"[flyout-host] those bytes could not be changed (error %lu)", GetLastError());
            return;
        }
        for (int i = 0; i < 5; ++i) target[6 + i] = 0x90;            // call LoadComponent
        memcpy(target + 52, source + 4, 8);                          // mov edx / lea rcx
        *reinterpret_cast<int*>(target + 74) =
            static_cast<int>(reinterpret_cast<long long>(getter) -
                             reinterpret_cast<long long>(target + 78));
        writable.Restore();   // subito: la scrittura e' finita qui
        FlushInstructionCache(GetCurrentProcess(), target, 80);
        g_patched.store(true, std::memory_order_relaxed);
        Wh_Log(L"[flyout-host] this process builds the flyouts with the Windows 10 template set "
               L"(written now, module 0x%p): the flyout is drawn and stays on screen, network and "
               L"battery", (void*)module);
        // 1.3.8 (A): la copia appena riscritta non si scarica piu' finche' questo processo vive.
        PinModuleOrLog(module, L"[flyout-host]", L"Windows.UI.QuickActions.dll", g_pinned);
    } catch (...) {
        Wh_Log(L"[flyout-host] exception while changing the template set");
    }
}

// ---- il modulo arriva: lo si prende quando compare -------------------------
// Il nome del modulo, senza il percorso e senza badare alle maiuscole: la shell carica questi
// moduli con il percorso pieno del pacchetto.
static bool NameIsModuleNamed(const wchar_t* text, size_t length,
                              const wchar_t* name) noexcept {
    if (!text || !name || !length) return false;
    size_t start = 0;
    for (size_t i = 0; i < length; ++i)
        if (text[i] == L'\\' || text[i] == L'/') start = i + 1;
    const size_t kLength = wcslen(name);
    if (length - start != kLength) return false;
    for (size_t i = 0; i < kLength; ++i) {
        wchar_t a = text[start + i];
        wchar_t b = name[i];
        if (a >= L'A' && a <= L'Z') a += 32;
        if (b >= L'A' && b <= L'Z') b += 32;
        if (a != b) return false;
    }
    return true;
}

static bool NameIsQuickActions(const wchar_t* text, size_t length) noexcept {
    return NameIsModuleNamed(text, length, L"Windows.UI.QuickActions.dll");
}

static void OnModuleLoaded(HMODULE module, const wchar_t* text, size_t length) noexcept {
    try {
        if (!module) return;
        if (NameIsQuickActions(text, length)) {
            if (g_logs.fetch_add(1) < 8)
                Wh_Log(L"[flyout-host] Windows.UI.QuickActions.dll is here (0x%p): the template "
                       L"set is changed before the shell builds anything with it",
                       (void*)module);
            PatchQuickActionsTemplates(module);
            return;
        }
        // 1.3.5: nello stesso momento, la pagina del flyout di rete. Il modulo e' suo, e il
        // blocco qui sotto non lo carica: prende solo la voce che gli serve, se c'e'.
        if (NameIsModuleNamed(text, length, L"NetworkUX.dll"))
            NetworkUxHostPatch::OnNetworkUxLoaded(reinterpret_cast<void*>(module));
    } catch (...) {
    }
}

// Le stesse due righe che il mod usa gia' per il reindirizzamento dei moduli del vassoio:
// il tipo e' quello di <winternl.h>, incluso in cima al file.
typedef LONG(NTAPI* LdrLoadDll_t)(PWSTR, PULONG, UNICODE_STRING*, HMODULE*);
static LdrLoadDll_t LdrLoadDll_Original = nullptr;

static LONG NTAPI LdrLoadDll_Hook(PWSTR searchPath, PULONG flags, UNICODE_STRING* name, HMODULE* handle) {
    const LONG status = LdrLoadDll_Original(searchPath, flags, name, handle);
    try {
        if (status >= 0 && name && name->Buffer && handle && *handle)
            OnModuleLoaded(*handle, name->Buffer, name->Length / sizeof(wchar_t));
    } catch (...) {
    }
    return status;
}

// Chiamata da Wh_ModInit, dentro il ramo di ShellExperienceHost.exe: gli hook sono registrati
// mentre Wh_ModInit gira (documentazione Windhawk), e il modulo dei template arriva dopo.
static void Install() noexcept {
    try {
        // A flyout may have been built before the mod started: then the module is already here.
        HMODULE loaded = GetModuleHandleW(L"Windows.UI.QuickActions.dll");
        if (loaded) PatchQuickActionsTemplates(loaded);

        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        void* target = ntdll ? reinterpret_cast<void*>(GetProcAddress(ntdll, "LdrLoadDll"))
                             : nullptr;
        if (target && Wh_SetFunctionHook(target, reinterpret_cast<void*>(LdrLoadDll_Hook),
                                         reinterpret_cast<void**>(&LdrLoadDll_Original)))
            Wh_Log(L"[flyout-host] the flyout module is watched (LdrLoadDll)");
    } catch (...) {
        Wh_Log(L"[flyout-host] exception while arming the flyout module watch");
    }
}

}  // namespace FlyoutHostPatch
// ===========================================================================
// 1.3.5 - IL FLYOUT DI RETE: i pulsanti tornano quelli di Windows 10.
//
// Il set di template di Windows 10 del blocco qui sopra (Windows.UI.QuickActions.dll)
// non basta da solo nel processo che disegna il flyout. La pagina del flyout di rete
// (NetworkUX.dll) chiede il pulsante delle azioni rapide con il nome del template di
// Windows 11, e con quel nome il pulsante del set di Windows 10 non viene mai usato:
// ExplorerPatcher lo scrive accanto alla stessa correzione - "If we're doing the quick
// actions patch but not this, they will only appear as non-interactive text blocks".
// Un flyout i cui pulsanti non rispondono e' un flyout che la shell richiude: da li'
// viene "si apre una volta e poi piu'".
//
// ExplorerPatcher risolve nella sua HandleLoadedNetworkUX(), e questo blocco fa la
// stessa cosa - la parte che si puo' fare senza toccare l'albero XAML:
//
//   * il nome "ToggleButtonWinuiFluentTemplate" che NetworkUX.dll passa a
//     WindowsCreateStringReference diventa "QuickToggleWinuiFluentTemplate", il nome
//     del pulsante di Windows 10. Si manda a una nostra funzione la voce della tabella
//     delle importazioni di NetworkUX.dll (una sola, il resto del modulo non si tocca);
//   * il nome si cambia SOLO se il set di template di Windows 10 e' stato applicato in
//     questo processo (TemplatesPatched): senza quella patch il pulsante deve restare
//     quello di questa build.
//
// La seconda meta' di quella correzione (il dizionario di risorse della pagina:
// QuickActionPanelMargin e QuickActionControlStyle, la geometria dei pulsanti di
// Windows 10) usa le API XAML e arriva con la prossima revisione: qui il nome e' la
// parte che decide se il pulsante e' vivo o e' un blocco di testo.
//
// Il modulo non viene caricato da questo mod: si prende se e' gia' qui, o quando la
// shell lo carica (lo stesso avviso del blocco dei template, OnModuleLoaded).
//
// 1.3.8 - due correzioni in questo blocco (A e C del giro): niente piu' fermo su g_redirected
// (quel flag dice "l'ho presa io", non "la voce che c'e' adesso punta alla mia funzione": dopo
// uno scarico e un ricarico del modulo sarebbe vero lo stesso e la voce della copia nuova
// resterebbe quella di Windows) e, dopo la presa, il modulo fissato in memoria con
// FlyoutHostPatch::PinModuleOrLog (una copia nuova non porterebbe la voce presa in prestito).
// ===========================================================================
namespace NetworkUxSkin { void TryApply() noexcept; }  // 1.3.7: la pelle grafica (chunk-skin.inc)

namespace NetworkUxHostPatch {

static std::atomic<bool> g_redirected{false};
static std::atomic<int> g_logs{0};
// 1.3.8 (A): NetworkUX.dll e' stata fissata in memoria (una volta per processo).
static std::atomic<bool> g_pinned{false};

typedef HRESULT(WINAPI* WindowsCreateStringReference_t)(const wchar_t* source, UINT32 length,
                                                        void* header, void** string);
static WindowsCreateStringReference_t WindowsCreateStringReference_Original = nullptr;

// I due nomi, gli stessi di ExplorerPatcher (NetworkUX_WindowsCreateStringReference).
static const wchar_t kWin11ButtonTemplate[] = L"ToggleButtonWinuiFluentTemplate";
static const wchar_t kWin10ButtonTemplate[] = L"QuickToggleWinuiFluentTemplate";

// La voce della tabella delle importazioni e' nostra solo finche' il mod e' caricato.
static ScopedImportRedirect g_import;

static HRESULT WINAPI CreateStringReferenceShim(const wchar_t* source, UINT32 length,
                                                void* header, void** string) {
    try {
        if (source && length == _countof(kWin11ButtonTemplate) - 1 &&
            wcsncmp(source, kWin11ButtonTemplate, length) == 0 &&
            FlyoutHostPatch::TemplatesPatched()) {
            if (g_logs.fetch_add(1, std::memory_order_relaxed) < 4)
                Wh_Log(L"[networkux] the network flyout asks for the Windows 11 button: with "
                       L"the Windows 10 template set of this process it becomes the button of "
                       L"Windows 10");
            source = kWin10ButtonTemplate;
            length = static_cast<UINT32>(_countof(kWin10ButtonTemplate) - 1);
        }
    } catch (...) {
    }
    // 1.3.7: la pelle grafica (il dizionario di risorse della pagina). E' la pagina
    // stessa a passare di qui, quindi il filo e' quello che disegna il flyout; se le
    // chiavi del dizionario non ci sono ancora, si riprova alla chiamata dopo.
    SquareShellWindowsNow();   // 1.3.9
    NetworkUxSkin::TryApply();
    if (!WindowsCreateStringReference_Original) return E_FAIL;
    return WindowsCreateStringReference_Original(source, length, header, string);
}

// Chiamata quando NetworkUX.dll compare in questo processo (o se era gia' qui).
// Prende la voce della tabella delle importazioni del modulo e la manda al nostro shim.
//
// Non e' `static`: la dichiarazione anticipata sta nel blocco dei template, che la usa
// (OnModuleLoaded) prima che questo namespace sia definito, esattamente come per
// RequestBatteryFlyout. Le due hanno lo stesso nome, la stessa firma e lo stesso namespace.
void OnNetworkUxLoaded(void* module) noexcept {
    try {
        if (!module) return;

        // 1.3.8 (D): la base del modulo e l'esito, come per i template. Questa riga esce sia
        // quando la pagina arriva (OnModuleLoaded) sia quando c'era gia' all'avvio del processo
        // (Install): sotto ci sono gli altri esiti (voce presa, gia' nostra, non trovata, fissata).
        if (g_logs.fetch_add(1, std::memory_order_relaxed) < 8)
            Wh_Log(L"[networkux] NetworkUX.dll is here (0x%p): the entry of the page is looked at "
                   L"now", module);

        HMODULE combase = GetModuleHandleW(L"combase.dll");
        // GetProcAddress restituisce FARPROC, non un void*: il compilatore del mod (clang)
        // non lo converte da solo, e il cast e' quello che il file usa gia' per le altre
        // voci prese allo stesso modo (LoadLibraryW, LdrLoadDll, DwmSetWindowAttribute).
        void* original = combase ? reinterpret_cast<void*>(
                                       GetProcAddress(combase, "WindowsCreateStringReference"))
                                 : nullptr;
        if (!original) {
            Wh_Log(L"[networkux] NetworkUX.dll (0x%p): the entry point of the string of this "
                   L"shell was not found: the button of the network flyout stays the one of this "
                   L"build", module);
            return;
        }
        WindowsCreateStringReference_Original =
            reinterpret_cast<WindowsCreateStringReference_t>(original);

        void** slot = FindIatSlot(static_cast<HMODULE>(module),
                                  "api-ms-win-core-winrt-string-l1-1-0.dll",
                                  "WindowsCreateStringReference");
        if (!slot) {
            Wh_Log(L"[networkux] NetworkUX.dll (0x%p) does not call that entry point through "
                   L"its import table: the button of the network flyout stays the one of this "
                   L"build", module);
            return;
        }
        // 1.3.8 (C): la decisione non poggia ne' su un flag ne' sull'indirizzo di base del modulo
        // (ASLR puo' riassegnare a una copia nuova lo stesso indirizzo): poggia sul CONTENUTO
        // della voce. Se punta gia' alla funzione di questo mod, il lavoro e' fatto - ed e' il caso
        // di una seconda segnalazione dello stesso modulo, o di un mod ricaricato in un processo
        // che aveva gia' la voce presa (in quel caso la voce non e' mai stata rimessa: il nuovo
        // Uninstall() di Wh_ModBeforeUninit la rimettera' com'era quando il mod verra' scaricato).
        if (*slot == reinterpret_cast<void*>(&CreateStringReferenceShim)) {
            g_redirected.store(true, std::memory_order_relaxed);
            Wh_Log(L"[networkux] the entry of NetworkUX.dll (0x%p) already points to this mod: "
                   L"nothing is taken over again, the button of the network flyout is already the "
                   L"one of Windows 10", module);
            FlyoutHostPatch::PinModuleOrLog(static_cast<HMODULE>(module), L"[networkux]",
                                            L"NetworkUX.dll", g_pinned);
            return;
        }

        // Se la voce presa in prestito prima stava in una copia di questo modulo che non c'e'
        // piu', quell'indirizzo non e' piu' memoria di nessuno: si lascia andare senza
        // riscriverlo (Abandon) e si prende in prestito la voce che c'e' adesso.
        if (g_import.redirected() && !g_import.owns(slot)) {
            Wh_Log(L"[networkux] the entry taken over before belongs to a copy of NetworkUX.dll "
                   L"that is not here any more: it is left as it is and the entry of this copy is "
                   L"taken instead");
            g_import.Abandon();
        }

        if (!g_import.owns(slot) &&
            !g_import.Redirect(slot, reinterpret_cast<void*>(&CreateStringReferenceShim))) {
            Wh_Log(L"[networkux] NetworkUX.dll (0x%p): that entry could not be taken over: the "
                   L"button of the network flyout stays the one of this build", module);
            return;
        }
        g_redirected.store(true, std::memory_order_relaxed);
        Wh_Log(L"[networkux] the page of the network flyout now asks the Windows 10 button "
               L"(module 0x%p): with the Windows 10 template set, its buttons are the ones of "
               L"Windows 10 (and they answer)", module);
        // 1.3.8 (A): la voce presa in prestito non puo' piu' sparire con il modulo.
        FlyoutHostPatch::PinModuleOrLog(static_cast<HMODULE>(module), L"[networkux]",
                                        L"NetworkUX.dll", g_pinned);
    } catch (...) {
        Wh_Log(L"[networkux] exception while taking over the button of the network flyout");
    }
}

// Chiamata da Wh_ModInit, dentro il ramo di ShellExperienceHost.exe, accanto al blocco dei
// template: gli hook sono registrati mentre Wh_ModInit gira (documentazione Windhawk) e il
// modulo della pagina puo' arrivare dopo.
static void Install() noexcept {
    try {
        OnNetworkUxLoaded(reinterpret_cast<void*>(GetModuleHandleW(L"NetworkUX.dll")));
    } catch (...) {
    }
}

// Chiamata da Wh_ModBeforeUninit, sullo stesso filo della fine del mod: la voce della
// tabella delle importazioni torna com'era prima che il modulo venga scaricato.
static void Uninstall() noexcept {
    try {
        if (!g_redirected.load(std::memory_order_relaxed)) return;
        g_import.Restore();
        g_redirected.store(false, std::memory_order_relaxed);
        Wh_Log(L"[networkux] the page of the network flyout is back to its own entry point");
    } catch (...) {
    }
}

}  // namespace NetworkUxHostPatch

// ===========================================================================
// 1.3.7 - IL PUNTO DEL DIZIONARIO DI RISORSE DEL FLYOUT DI RETE (solo byte).
//
// La pelle grafica (chunk-skin.inc) ha bisogno di sapere se questa build ha il
// punto in cui la pagina del flyout di rete carica il proprio dizionario di
// risorse. ExplorerPatcher lo trova cosi' (HandleLoadedNetworkUX) e questo blocco
// fa la stessa lettura. Qui NON c'e' niente di XAML e niente di Windhawk: solo
// byte, maschera e aritmetica dell'indirizzo - cosi' il banco di prova
// (patch/harness2.cpp) la esegue su un'immagine finta e controlla i conti.
//
// Il sito, dentro NetworkUX.dll (NetworkUX::App::StaticOnLaunched):
//
//   48 8B 40 10              mov  rax, [rax+10h]
//   E8 ?? ?? ?? ??           call rel32
//   E8 ?? ?? ?? ??           call rel32   <- questa e' LoadResourceDictionaries
//   80 3D ?? ?? ?? ?? 00     cmp  byte ptr [rip+disp32], 0
//   75 05                    jne  +5
//   E8                       call rel32
//
// Se il sito non c'e' (una build che questo codice non conosce) non si tocca
// niente: la pelle non si applica e il log lo dice.
// ===========================================================================
namespace NetworkUxSkinPattern {

static const unsigned char kCallSite[] = {
    0x48, 0x8B, 0x40, 0x10, 0xE8, 0x00, 0x00, 0x00, 0x00, 0xE8,
    0x00, 0x00, 0x00, 0x00, 0x80, 0x3D, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x75, 0x05, 0xE8,
};
static const char kCallSiteMask[] = "xxxxx????x????xx????xxxx";

// Quanti byte separano l'inizio del sito dalla call che interessa: i quattro del
// mov e i cinque della prima call.
static const size_t kDictionaryCallAt = 9;

// Segue la rel32 di una call e restituisce dove porta.
static unsigned char* Rel32Target(unsigned char* call) noexcept {
    if (!call || call[0] != 0xE8) return nullptr;
    int displacement = 0;
    memcpy(&displacement, call + 1, sizeof(displacement));
    return call + 5 + displacement;
}

// La routine cercata, se il sito e' nell'intervallo dato: un indirizzo fuori dal
// codice (o un sito trovato piu' di una volta) vuol dire una build non conosciuta,
// e in quel caso non si scrive nulla.
static unsigned char* ResolveDictionaryRoutine(unsigned char* text, size_t size) noexcept {
    if (!text || size < sizeof(kCallSite)) return nullptr;
    unsigned char* site = FlyoutHostPatch::FindPattern(text, size, kCallSite, kCallSiteMask);
    if (!site) return nullptr;
    if (FlyoutHostPatch::FindPattern(site + 1, size - (site + 1 - text), kCallSite,
                                     kCallSiteMask))
        return nullptr;   // due siti: non si sa quale sia quello giusto
    unsigned char* target = Rel32Target(site + kDictionaryCallAt);
    if (!target || target < text || target >= text + size) return nullptr;
    return target;
}

}  // namespace NetworkUxSkinPattern

// ===========================================================================
// 1.3.7 - LA PELLE GRAFICA DEL FLYOUT DI RETE (le regole di "10Flyouts v4.5").
//
// Il file dell'utente (uploads/10Flyouts v4.5.txt) e' un elenco di regole per i
// flyout della shell, scritte per il motore di una mod di stile (la "Windows 11
// Notification Center Styler"): ogni regola nomina un elemento dell'albero XAML
// per nome e posizione, e quel motore le va a scrivere elemento per elemento.
// Questo mod non porta quel motore dentro di se': non prende in mano l'albero XAML
// di un altro programma e non fa da ponte verso un motore esterno.
//
// La parte di quelle regole che si puo' scrivere con le API XAML documentate e'
// quella che riguarda il dizionario di risorse della pagina - ed e' la stessa che
// ExplorerPatcher scrive accanto alla correzione dei template
// (NetworkUX_PatchResourceDictionary, chiamata subito dopo
// NetworkUX::App::LoadResourceDictionaries, il cui punto sta in
// chunk-skin-pattern.inc). Sono le regole della geometria dei pulsanti delle
// azioni rapide e delle superfici:
//
//   * QuickActionPanelMargin - il margine del pannello delle azioni rapide.
//     Windows 10 usa 12,0,0,12 (Windows 11: 12,0,24,0).
//   * QuickActionControlStyle - la misura del singolo pulsante. Windows 10 usa
//     Margin 4,0,0,4 e Width 90 Height 64 (Windows 11: 12,0,0,0 e 96x90). Si
//     toccano SOLO i tre setter della misura: gli altri restano dove sono
//     (ExplorerPatcher toglie tutti i setter e li rimette; qui si fa di meno,
//     perche' quello che la pagina si aspetta non si sa e non si tocca).
//   * ControlCornerRadius e OverlayCornerRadius a 0 - la documentazione Microsoft
//     li chiama raggi d'angolo globali: "You can override these values in your
//     App.xaml to change the rounding across all controls in your app". Il file
//     chiede CornerRadius=0 su bordi, pulsanti, caselle e barre di scorrimento:
//     questo e' il punto in cui si puo' chiedere per tutti.
//   * FocusVisualPrimaryThickness e FocusVisualSecondaryThickness a 0, quando
//     questa build li tiene nel dizionario: il file li azzera su griglie, pulsanti
//     e link, ed e' la stessa cosa che la comunita' usa per togliere il rettangolo
//     bianco dai flyout di Windows 10 su Windows 11 ("10FlyoutFix").
//
// Cosa NON si applica, detto chiaro: le regole che nominano i singoli controlli
// della pagina (il bordo del LogonFrame, il fondo acrilico, il collegamento
// "Impostazioni" e la sua descrizione, l'indicatore di selezione della lista delle
// reti, i margini dei pulsanti, i caratteri). Quelle vanno scritte dentro l'albero
// XAML mentre quegli elementi esistono, cioe' vogliono un motore di stile: qui non
// c'e' e non si finge che ci sia. Il log dice cosa e' stato scritto e cosa no.
//
// Come si applica, e perche' cosi': dalla voce di 1.3.5 (la chiamata
// WindowsCreateStringReference di NetworkUX.dll). E' la pagina stessa a chiamare,
// quindi il filo e' quello che disegna il flyout e il momento e' il suo. Il
// dizionario pero' arriva mentre la pagina si costruisce: finche' le sue chiavi
// non ci sono si riprova alla chiamata dopo (poche decine di tentativi, poi si
// smette). Nessun hook nuovo - il mod registra tutti i suoi hook in Wh_ModInit
// (vedi la nota li'), e questa parte non tocca la coda degli hook -, nessuna
// impostazione, nessuna chiave di registro, nessun modulo caricato.
// ===========================================================================
namespace NetworkUxSkin {

namespace wux = winrt::Windows::UI::Xaml;

static std::atomic<bool> g_valuesWritten{false};
static std::atomic<bool> g_styleWritten{false};
static std::atomic<int> g_attempts{0};
static std::atomic<int> g_logs{0};

// Il tipo di un valore del dizionario. I valori di XAML possono essere racchiusi
// ("Windows.Foundation.IReference`1<...>"): si guarda il nome per intero, cosi'
// va bene sia il valore diretto sia quello racchiuso.
static bool ValueIsOfType(winrt::Windows::Foundation::IInspectable const& value,
                          const wchar_t* needle) noexcept {
    try {
        winrt::hstring name = winrt::get_class_name(value);
        return name.c_str() && wcsstr(name.c_str(), needle) != nullptr;
    } catch (...) {
        return false;
    }
}

static bool DictionaryHas(wux::ResourceDictionary const& resources, const wchar_t* key) noexcept {
    try {
        return resources.HasKey(winrt::box_value(winrt::hstring(key)));
    } catch (...) {
        return false;
    }
}

// Scrive un valore del dizionario.
// - la chiave c'e' gia': si sostituisce solo se il tipo che c'e' regge quello
//   nuovo (si legge dal valore che c'e', non si indovina);
// - la chiave non c'e': si aggiunge solo quando la documentazione Microsoft dice
//   che quella chiave si sovrascrive proprio cosi' (i due raggi d'angolo);
// - qualunque altra cosa: non si scrive, e il log lo dice.
static bool PutValue(wux::ResourceDictionary const& resources, const wchar_t* key,
                     winrt::Windows::Foundation::IInspectable const& value,
                     const wchar_t* typeNeedle, bool insertIfAbsent) noexcept {
    try {
        auto boxedKey = winrt::box_value(winrt::hstring(key));
        if (resources.HasKey(boxedKey)) {
            if (!ValueIsOfType(resources.Lookup(boxedKey), typeNeedle)) {
                if (g_logs.fetch_add(1, std::memory_order_relaxed) < 8)
                    Wh_Log(L"[networkux] %s: this build keeps it with another type, left as "
                           L"it is", key);
                return false;
            }
        } else if (!insertIfAbsent) {
            return false;
        }
        resources.Insert(boxedKey, value);
        return true;
    } catch (...) {
        return false;
    }
}

// La misura del pulsante delle azioni rapide come la scrive ExplorerPatcher:
// Margin 4,0,0,4, Width 90, Height 64. Solo quei tre setter.
static bool PutControlStyleMetrics(wux::ResourceDictionary const& resources) noexcept {
    try {
        auto style = resources.Lookup(winrt::box_value(winrt::hstring(L"QuickActionControlStyle")))
                         .try_as<wux::Style>();
        if (!style) {
            if (g_logs.fetch_add(1, std::memory_order_relaxed) < 8)
                Wh_Log(L"[networkux] QuickActionControlStyle is not a style in this build: the "
                       L"size of the quick action buttons stays as it is");
            return false;
        }
        if (style.IsSealed()) {
            // Uno stile gia' usato non si puo' piu' cambiare: lo dice la regola
            // degli stili di XAML, e il log lo scrive invece di forzare qualcosa.
            if (g_logs.fetch_add(1, std::memory_order_relaxed) < 8)
                Wh_Log(L"[networkux] QuickActionControlStyle is already in use (sealed): the "
                       L"size of the quick action buttons stays as it is");
            return false;
        }

        wux::DependencyProperty properties[3] = {
            wux::FrameworkElement::MarginProperty(),
            wux::FrameworkElement::WidthProperty(),
            wux::FrameworkElement::HeightProperty(),
        };
        winrt::Windows::Foundation::IInspectable values[3] = {
            winrt::box_value(wux::Thickness{4.0, 0.0, 0.0, 4.0}),
            winrt::box_value(90.0),
            winrt::box_value(64.0),
        };

        auto setters = style.Setters();
        for (int i = 0; i < 3; ++i) {
            bool done = false;
            const uint32_t count = setters.Size();
            for (uint32_t j = 0; j < count; ++j) {
                auto setter = setters.GetAt(j).try_as<wux::Setter>();
                if (!setter || !setter.Property()) continue;
                if (setter.Property() != properties[i]) continue;
                setter.Value(values[i]);
                done = true;
                break;
            }
            if (!done) setters.Append(wux::Setter(properties[i], values[i]));
        }
        Wh_Log(L"[networkux] the quick action buttons of this flyout have the size of Windows 10 "
               L"(margin 4,0,0,4, 90x64)");
        return true;
    } catch (...) {
        return false;
    }
}

// Un tentativo, chiamato dalla voce di 1.3.5. Quando le chiavi del dizionario ci
// sono, i valori si scrivono una volta sola; la misura dello stile si riprova
// finche' non riesce (se lo stile e' gia' in uso non riesce mai, e si smette di
// provare dopo qualche decina di tentativi).
void TryApply() noexcept {
    try {
        const bool valuesDone = g_valuesWritten.load(std::memory_order_relaxed);
        const bool styleDone = g_styleWritten.load(std::memory_order_relaxed);
        if (valuesDone && styleDone) return;
        if (g_attempts.fetch_add(1, std::memory_order_relaxed) >= 256) return;

        auto app = wux::Application::Current();
        if (!app) return;   // il programma XAML non c'e' ancora
        auto resources = app.Resources();
        if (!resources) return;

        // La chiave che ExplorerPatcher usa come prova che questo e' il dizionario
        // della pagina: finche' non c'e', non e' il momento.
        if (!valuesDone && DictionaryHas(resources, L"QuickActionPanelMargin")) {
            int written = 0;
            if (PutValue(resources, L"QuickActionPanelMargin",
                         winrt::box_value(wux::Thickness{12.0, 0.0, 0.0, 12.0}),
                         L"Thickness", false))
                ++written;
            if (PutValue(resources, L"ControlCornerRadius",
                         winrt::box_value(wux::CornerRadius{0.0, 0.0, 0.0, 0.0}),
                         L"CornerRadius", true))
                ++written;
            if (PutValue(resources, L"OverlayCornerRadius",
                         winrt::box_value(wux::CornerRadius{0.0, 0.0, 0.0, 0.0}),
                         L"CornerRadius", true))
                ++written;
            if (PutValue(resources, L"FocusVisualPrimaryThickness",
                         winrt::box_value(wux::Thickness{0.0, 0.0, 0.0, 0.0}),
                         L"Thickness", false))
                ++written;
            if (PutValue(resources, L"FocusVisualSecondaryThickness",
                         winrt::box_value(wux::Thickness{0.0, 0.0, 0.0, 0.0}),
                         L"Thickness", false))
                ++written;
            g_valuesWritten.store(true, std::memory_order_relaxed);
            Wh_Log(L"[networkux] the dictionary of this flyout takes the Windows 10 skin: %d "
                   L"value(s) written (panel margin of Windows 10, square corners, no focus "
                   L"rectangle)", written);
        }

        if (!styleDone && DictionaryHas(resources, L"QuickActionControlStyle") &&
            PutControlStyleMetrics(resources))
            g_styleWritten.store(true, std::memory_order_relaxed);
    } catch (...) {
    }
}

}  // namespace NetworkUxSkin
BOOL Wh_ModInit() {
    try {
        LoadFlyoutSettings();

        wchar_t exePath[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, exePath, _countof(exePath));
        if (g_realExePath[0] == 0) wcscpy_s(g_realExePath, exePath);
        Wh_Log(L"[flyout] init: Windows 10 legacy flyouts restorer");
        Wh_Log(L"[flyout] process: %s", exePath);
        Wh_Log(L"[flyout] data folder: %s",
               g_cfg.storePath[0] ? g_cfg.storePath : L"(not available)");

        // ShellExperienceHost.exe hosts the Action Center panel. This mod is loaded there
        // as well, and in that process only the animation section runs: it is not the
        // shell process, and nothing else of the mod applies to it.
        if (ImageNameIs(g_realExePath, L"ShellExperienceHost.exe")) {
            Wh_Log(L"[flyout] ShellExperienceHost: the Action Center animation is the only "
                   L"part of this mod that runs here");
            // This process is the one that draws the flyout: it has to build it the Windows 10
            // way, otherwise the flyout it shows is torn down again after a moment.
            FlyoutHostPatch::Install();
            // 1.3.5: la pagina del flyout di rete deve chiedere il pulsante di
            // Windows 10, altrimenti i pulsanti del set di template qui sopra
            // restano blocchi di testo inerti (ExplorerPatcher, stessa nota).
            NetworkUxHostPatch::Install();
            // The cloak hook also carries the experimental square corners, so it is installed
            // when either option is on. The animation parts check g_acAnimation at every call.
            if (g_acAnimation || g_squareFlyoutCorners) InstallActionCenterAnimation();
            SquareShellWindowsNow();
            return TRUE;
        }

        if (!IsLegacyShellProcess()) {
            Wh_Log(L"[flyout] this is not the private Windows 10 shell: nothing to do here");
            return TRUE;
        }

        // 1.3.4: the battery icon opens the Windows 10 battery flyout. The hook
        // is installed here, inside Wh_ModInit, so that the engine applies it at once (see
        // the note below), and it is not tied to a setting: with a settings list saved by an
        // older version a new setting reads as 0 and the battery click did nothing.
        NetworkTrayForce::BatteryFlyout::Install();

        // 1.3.1 - THE REGISTRATION POINT OF EVERY HOOK OF THIS MOD.
        //
        // Windhawk wiki, "Creating a new mod": Wh_SetFunctionHook "can't be called after
        // Wh_ModBeforeUninit returns"; Wh_ApplyHookOperations "is called automatically by
        // Windhawk after Wh_ModInit" and, in its own words, "ideally, all hooks should be
        // set in Wh_ModInit and this function should never be used". The "Mod lifetime" page
        // shows the same order: Wh_ModInit, the implicit apply, and only afterwards are the
        // hooks removed, between Wh_ModBeforeUninit and Wh_ModUninit.
        //
        // 1.3.0 registered the tray hooks from the services thread instead, that is after
        // Wh_ModInit had returned: the engine had already applied the queue, so they stayed
        // queued and inert, and the mod then called Wh_ApplyHookOperations in a loop from
        // that thread while the engine was loading or unloading hooks of its own. The hook
        // queue of the engine (MinHook) is not meant to be operated by two threads at once,
        // which is what took explorer.exe down on every enable and on every disable.
        // Nothing outside Wh_ModInit touches the hook queue any more.
        InstallTraySupportHooks();

        // The Action Center policy of Windows 10, served in memory: no registry write.
        // The hooks themselves belong here, for the same reason.
        if (g_cfg.fixNotificationsCrash) {
            NativeUi::actionCenterPolicyProcess = true;
            if (NativeUi::InstallKeyHooks())
                Wh_Log(L"[actioncenter] the policy hooks are installed (in Wh_ModInit)");
            else
                Wh_Log(L"[actioncenter] the policy hooks are not available in this process: "
                       L"the notification policy stays as the shell serves it");
        }

        // The click on the network icon: ShellExecuteW and ShellExecuteExW.
        InstallNetworkClickHooks();

        // The button hook is installed once and checks the setting at every call, so the
        // setting can be switched at run time without installing or removing hooks.
        InstallActionCenterButtonHook();

        // On some builds the panel is hosted by this very process, so the animation hooks
        // are installed here too (in ShellExperienceHost the same call was made above).
        if (g_acAnimation) InstallActionCenterAnimation();

        g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_stopEvent) {
            Wh_Log(L"[flyout] the stop event could not be created");
            return TRUE;
        }
        g_servicesThread = CreateThread(nullptr, 0, FlyoutServicesThread, nullptr, 0, nullptr);
        if (!g_servicesThread)
            Wh_Log(L"[flyout] the services thread could not be created (%lu)", GetLastError());
        return TRUE;
    } catch (...) {
        Wh_Log(L"[flyout] init exception: the shell is left as it is");
        return TRUE;
    }
}

// 1.3.1 - the unload, in the order the Windhawk documentation describes it.
//
// "Mod lifetime": Wh_ModBeforeUninit, then the engine removes the hooks, then Wh_ModUninit;
// the API pages add that Wh_SetFunctionHook, Wh_RemoveFunctionHook and Wh_ApplyHookOperations
// can no longer be used once Wh_ModBeforeUninit has returned. So the threads of this mod are
// stopped and joined in Wh_ModBeforeUninit - there is no other place left where it can be
// done - and Wh_ModUninit only finishes what is left, without blocking: 1.3.0 waited with
// INFINITE there, that is after the hooks were already gone and the module was about to be
// unloaded, and it closed the handles of a thread it had not seen exit.
static bool JoinServicesThread(PCWSTR where) {
    if (!g_servicesThread) return true;
    const ULONGLONG deadline = GetTickCount64() + 10000;
    DWORD waited = WAIT_TIMEOUT;
    while (GetTickCount64() < deadline) {
        waited = WaitForSingleObject(g_servicesThread, 100);
        if (waited == WAIT_OBJECT_0) break;
    }
    if (waited != WAIT_OBJECT_0) {
        Wh_Log(L"[flyout] the services thread did not stop within 10 s (%s): its handles are "
               L"kept and nothing it owns is freed from another thread", where);
        return false;
    }
    return true;
}

void Wh_ModBeforeUninit() {
    // Called by Windhawk "when the mod is about to be unloaded, before the Windhawk engine
    // removes hooks": from the return of this callback on, no hook operation is allowed any
    // more, so the mod has to be quiet here already.
    g_unloading.store(true, std::memory_order_seq_cst);
    ShellOpGuard::BeginShutdown();
    StopActionCenterAnimation();
    // 1.3.5: la voce della tabella delle importazioni del flyout di rete torna al
    // valore di prima mentre il modulo e' ancora caricato (ScopedImportRedirect).
    NetworkUxHostPatch::Uninstall();
    if (g_stopEvent) SetEvent(g_stopEvent);
    JoinServicesThread(L"Wh_ModBeforeUninit");
}

void Wh_ModUninit() {
    // Called "when the mod is about to be unloaded, after the Windhawk engine removes hooks",
    // and the module is unloaded when this returns: no hook function may be used here, and a
    // callback that blocks forever would hold the unload. The thread was stopped above, so
    // this is a bounded check that normally finds it already gone.
    const bool threadStopped = JoinServicesThread(L"Wh_ModUninit");
    if (threadStopped) {
        if (g_servicesThread) {
            CloseHandle(g_servicesThread);
            g_servicesThread = nullptr;
        }
        // The stop event belongs to the thread: it is closed only once the thread is gone.
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
    }
    // The timed-out ShellExecute workers of this mod are joined here (bounded).
    ShellOpGuard::Shutdown();
    // The panel must never stay parked outside the screen.
    StopActionCenterAnimation();
    NativeUi::stopping.store(true, std::memory_order_release);
    g_trayThreadId = 0;
    Wh_Log(L"[flyout] unloaded");
}

void Wh_ModSettingsChanged() {
    LoadFlyoutSettings();
    NetworkTrayForce::SettingsChanged();
    Wh_Log(L"[flyout] settings reloaded: tray modules=%s AC button=%s (conflict=%s) "
           L"AC animation=%s",
           g_cfg.provideTrayDlls ? L"on" : L"off",
           g_showActionCenterButton ? L"on" : L"off",
           g_actionCenterReassert ? L"reassert" : L"log",
           g_acAnimation ? L"on" : L"off");
}
