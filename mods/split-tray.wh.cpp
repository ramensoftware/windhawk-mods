// ==WindhawkMod==
// @id              split-tray
// @name            Split Tray
// @description     A notification area on every display's taskbar: choose, per application, which tray its icon shows in
// @version         1.3.2
// @author          Brandon Stonebridge
// @github          https://github.com/st0nebridge
// @homepage        https://github.com/st0nebridge/SplitTray
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -luser32 -lole32 -loleaut32 -lruntimeobject -lshlwapi -luiautomationcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Split Tray

Windows 11 shows the notification area - the system tray - on the main
display only. Split Tray puts one on the taskbar of **every other display**,
and lets you choose which tray each application's icon lives in.

![Tray 2 in the second display's taskbar: the chevron, five icons, and the clock](https://raw.githubusercontent.com/st0nebridge/SplitTray/v1.3.1/docs/images/tray-2.png)

## What you get

* A tray inside each extra display's taskbar, beside the clock, where the
  native one would be. Icons work as in the real tray: left, right, double and
  middle clicks, context menus, tooltips, the popups some applications draw
  instead, the keyboard, and screen readers.
* Balloon notifications from icons in Split Tray's trays, shown by Windows as
  for any other icon.
* Per-application rules - "this program's icon goes to tray 3" - and a
  default for everything else.
* Move any icon by hand: **Shift+right-click** an icon in one of Split Tray's
  trays for its menu, or open **Arrange icons** to drag icons between every
  tray. Where you put an icon is remembered, until it has not been seen for a
  year.
* An overflow chevron for the icons you would rather not see all the time.
* Unplug a display and its icons go back to the main tray; plug it back in
  and they return. Every tray keeps its number meanwhile.
* Extra floating trays anywhere you like - for a display without a taskbar, or
  to try the mod out with one display. Screen readers reach them too.

## Trays are numbered

Tray 1 is Windows' own tray on the main display. Split Tray's trays are 2 and
up: one for each other display, left to right as the mod first sees them, then
any extra trays in the order they are listed in the settings. Rules and the
menus use these numbers.

A display keeps its number while it is unplugged. A display seen for the first
time takes the number of one that is not connected, so a laptop's external
display is tray 2 whichever display it is.

## How it works

An application puts an icon in the tray by calling `Shell_NotifyIcon`, which
sends a `WM_COPYDATA` message to Explorer's `Shell_TrayWnd`. Split Tray
subclasses that window, so it sees every icon from every process and can pass
each one on to the real tray, keep it for one of its own, or both. An icon sent
to another tray is really gone from the main one - it is not an overlay.

## Notes

* Icons that already existed when the mod loaded are collected by asking
  applications to re-register (the standard `TaskbarCreated` broadcast). A few
  ignore it; their icons appear the next time they update.
* Windows shows balloon notifications only for icons Explorer holds. For an
  icon in one of Split Tray's trays, Explorer is given a hidden copy of it the
  first time it has one to show; the copy goes when the icon does.
* More displays connected at once than ever before move the extra trays'
  numbers up by one, to make room.
* A floating tray has the keyboard when an application gives the focus back to
  its icon, or a screen reader moves to it. No key of its own reaches one.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- defaultTray: primary
  $name: Default tray
  $description: >-
    Where an icon goes when no rule below matches it. Tray 1 is Windows' own;
    Split Tray's are 2 and up, one for each other display from left to right,
    then the extra trays.
  $options:
  - primary: Tray 1 (Windows' own)
  - secondary: Tray 2
  - tray3: Tray 3
  - tray4: Tray 4
  - tray5: Tray 5
  - tray6: Tray 6
  - tray7: Tray 7
  - tray8: Tray 8
  - tray9: Tray 9
  - tray10: Tray 10
  - both: Both tray 1 and tray 2
- perProcessRouting:
  - - exe: ""
      $name: Executable name
      $description: >-
        For example discord.exe - matched case-insensitively against the file
        name. Include a backslash to match against the full path instead.
    - destination: secondary
      $name: Tray
      $options:
      - primary: Tray 1 (Windows' own)
      - secondary: Tray 2
      - tray3: Tray 3
      - tray4: Tray 4
      - tray5: Tray 5
      - tray6: Tray 6
      - tray7: Tray 7
      - tray8: Tray 8
      - tray9: Tray 9
      - tray10: Tray 10
      - both: Both tray 1 and tray 2
  $name: Per-application rules
  $description: >-
    Rules are evaluated top to bottom; the first match wins. An icon you move
    by hand stays where you put it, whatever the rules say.
- extraTrays:
  - - display: ""
      $name: Display
      $description: >-
        primary, or a display number counted from the left (1 is the leftmost).
        Leave it empty for no extra tray.
    - corner: bottomLeft
      $name: Corner
      $options:
      - bottomRight: Bottom right
      - bottomLeft: Bottom left
      - topRight: Top right
      - topLeft: Top left
    - disabled: false
      $name: Disabled
      $description: >-
        Keep this entry but show no tray. It keeps its number, so the trays
        after it do not shift, and icons meant for it wait in tray 1.
  $name: Extra trays
  $description: >-
    Floating trays in addition to the one on each display's taskbar - for a
    display without a taskbar, a second tray on the same display, or trying
    Split Tray out with one display. They are numbered after the displays'
    trays.
- trayPosition: bottomRight
  $name: Floating position
  $description: >-
    Where a display's tray floats when it cannot sit in that display's
    taskbar: with embedding off, or on a display that shows no taskbar.
  $options:
  - bottomRight: Bottom right
  - bottomLeft: Bottom left
  - topRight: Top right
  - topLeft: Top left
- offsetX: 8
  $name: Horizontal offset
  $description: Distance in pixels of a floating tray from its corner of the work area.
- offsetY: 8
  $name: Vertical offset
  $description: Distance in pixels of a floating tray from its corner of the work area.
- iconSize: 16
  $name: Icon size
  $description: Icon size in a floating tray, in pixels at 100% scaling.
- cellSize: 28
  $name: Cell size
  $description: Size of the clickable square around each icon in a floating tray, in pixels at 100% scaling.
- maxColumns: 12
  $name: Icons per row
  $description: A floating tray wraps onto more rows once this many icons are shown.
- backgroundColor: "202020"
  $name: Background colour
  $description: A floating tray's background, as hex RRGGBB.
- opacity: 235
  $name: Opacity
  $description: A floating tray's opacity, 0 (invisible) to 255 (opaque).
- alwaysOnTop: true
  $name: Keep floating trays above other windows
- showTooltips: true
  $name: Show tooltips
- mirrorHiddenIcons: true
  $name: Show icons the application marked as hidden
  $description: >-
    Applications can ask for an icon to be hidden (NIS_HIDDEN). Turn this off
    to respect that in Split Tray's trays too.
- embedInTaskbar: true
  $name: Embed in each display's taskbar
  $description: >-
    Put each display's tray inside that display's taskbar, where the native
    tray sits, instead of drawing it as a floating panel. This reaches the
    taskbar by hooking symbols in Explorer's own DLLs, so it coexists with
    other taskbar mods. Turn it off to fall back to floating panels if a
    Windows update moves those symbols.
- maxVisibleIcons: 8
  $name: Icons before the overflow chevron
  $description: >-
    How many icons a tray in a taskbar shows before the rest move behind a
    "show hidden icons" chevron, as the native tray does. 0 shows all of them.
- dumpXamlTree: false
  $name: Log the taskbar's XAML tree
  $description: >-
    Diagnostic. Prints the structure of a taskbar's tray area to the mod log
    once, which is how the elements this mod attaches to are identified after
    a Windows update changes them. Leave it off otherwise: the dump holds up
    the taskbar for seconds while Explorer starts, and applications whose
    icons arrive in that time can lose them.
- repopulateOnLoad: true
  $name: Collect existing icons on load
  $description: >-
    Asks already-running applications to re-register their icons (the standard
    TaskbarCreated broadcast) so they can be routed. Turn this off if an
    application misbehaves when it is asked to re-register.
*/
// ==/WindhawkModSettings==

// ============================================================================
// Implementation
//
// Windhawk compiles a mod from a single translation unit, so this file is
// organised into clearly separated sections rather than separate modules:
//
//   1. Wire protocol   - parsing Explorer's Shell_TrayWnd WM_COPYDATA payload
//   2. Settings        - the model, and loading it from Windhawk
//   3. Routing         - resolving a notification to a destination
//   4. Monitors/layout - where the secondary tray goes and how big it is
//   5. Icon store      - the mirrored icons and their sticky routing decisions
//   6. Secondary tray  - the window, its thread, painting and hit testing
//   7. Click forwarding- the tray callback protocol back to the owning app
//   8. Interception    - the Shell_TrayWnd subclass, and replaying decisions
//   9. Lifecycle       - Wh_ModInit / AfterInit / SettingsChanged / Uninit
//  10. XAML           - attaching to Explorer's taskbar XAML tree
//
// Sections 1-4 are pure functions of their inputs and are covered by
// tests/regression, which compiles this file against stub Windhawk headers so
// that the tested code is literally the shipped code.
// ============================================================================

#include <windhawk_utils.h>

#include <commctrl.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Section 10 lives below, but the tray thread's retry timer in section 6
// drives it, so it is declared here.
#ifndef SPLITTRAY_NO_XAML
namespace SplitTrayXaml {
void EnsureTaskbarXamlHooked();
void OnIconStoreChanged();
// Must run on the taskbar's UI thread. Finds each display's tray row by walking
// down from its taskbar's XamlRoot, rather than waiting to be handed an element.
void TryAttachEmbeddedTray();
// Safe from any thread: marshals a redraw onto the taskbar's UI thread.
void RequestEmbeddedRefresh();
// Must run on the taskbar's UI thread. Where the icon with this serial is
// drawn, in screen pixels: its cell, or its tray's chevron when it is in the
// overflow.
bool IconScreenRect(uint64_t serial, RECT* out);
// Whether a display's tray still has to be put into its taskbar, so the tray
// thread keeps asking the taskbar's thread to try. Safe from any thread.
bool AnyDisplayTrayWaitingToEmbed();
// Must run on the taskbar's UI thread: takes every panel back out and lets go
// of every XAML object the mod holds, for unloading.
void RemoveEverything();
// Must run on the taskbar's UI thread: gives the keyboard focus to the cell of
// the icon with this serial, as NIM_SETFOCUS asks (DECISIONS 80).
void FocusIconCell(uint64_t serial);
}  // namespace SplitTrayXaml
#endif

namespace SplitTray {

// ============================================================================
// Section 1 - Wire protocol
//
// Shell_NotifyIcon does not call into the shell through an API. shell32 builds a
// fixed-layout record and sends it to the tray window:
//
//     hTray = FindWindowW(L"Shell_TrayWnd", NULL);
//     COPYDATASTRUCT cds = { .dwData = 1, .cbData = 1484, .lpData = &record };
//     SendMessageTimeout(hTray, WM_COPYDATA, (WPARAM)nid.hWnd, (LPARAM)&cds);
//
// Every caller is normalised into one layout before it reaches the wire: ANSI
// callers are converted to UTF-16, and a caller passing any historical cbSize
// (V1/V2/V3/V4, 32-bit or 64-bit) arrives with cbSize == 956. Handle fields are
// 32 bits wide in both bitnesses, because USER handles are 32-bit safe.
//
// The offsets below were captured from the real shell32 on Windows 10.0.26100 by
// tests/probe/shell32_wire_probe.cpp, which puts its own Shell_TrayWnd on a
// private desktop and dumps what shell32 sends it. See
// tests/probe/probe-output-26100.txt for the raw evidence. Everything is bounds
// checked against the received cbData so that a future OS that grows or shrinks
// the record degrades instead of reading out of bounds.
// ============================================================================

// cds.dwData for a notification-area message. Other values carry appbar and
// in-proc-load requests, which this mod must pass through untouched.
constexpr ULONG_PTR kTrayCopyDataId = 1;
constexpr DWORD kTrayDataSignature = 0x34753423;

namespace wire {
constexpr size_t kSignature = 0x000;    // DWORD, kTrayDataSignature
constexpr size_t kMessage = 0x004;      // DWORD, NIM_*
constexpr size_t kNidCbSize = 0x008;    // DWORD, 956
constexpr size_t kOwnerWnd = 0x00C;     // DWORD, HWND of the icon owner
constexpr size_t kUID = 0x010;          // DWORD
constexpr size_t kFlags = 0x014;        // DWORD, NIF_*
constexpr size_t kCallbackMsg = 0x018;  // DWORD
constexpr size_t kIcon = 0x01C;         // DWORD, HICON
constexpr size_t kTip = 0x020;          // WCHAR[128]
constexpr size_t kTipChars = 128;
constexpr size_t kState = 0x120;      // DWORD, NIS_*
constexpr size_t kStateMask = 0x124;  // DWORD
constexpr size_t kInfo = 0x128;       // WCHAR[256], a balloon's text
constexpr size_t kInfoChars = 256;
constexpr size_t kVersion = 0x328;    // DWORD, uVersion / uTimeout
constexpr size_t kInfoTitle = 0x32C;  // WCHAR[64]
constexpr size_t kInfoTitleChars = 64;
constexpr size_t kInfoFlags = 0x3AC;  // DWORD, NIIF_*
constexpr size_t kGuid = 0x3B0;       // GUID
constexpr size_t kBalloonIcon = 0x3C0;  // DWORD, HICON
constexpr size_t kExePath = 0x3C4;      // WCHAR[260], owner's image path
constexpr size_t kExePathChars = 260;
constexpr size_t kTotalSize = 0x5CC;  // 1484

// The shortest prefix that still carries an identifiable notification.
constexpr size_t kMinUsableSize = kIcon + sizeof(DWORD);
constexpr DWORD kExpectedNidCbSize = 956;
}  // namespace wire

// A notification decoded off the wire. Handles are widened from the 32-bit wire
// fields; strings are copied out of the fixed-size, possibly unterminated
// buffers.
struct TrayNotification {
    DWORD message = 0;  // NIM_ADD / NIM_MODIFY / NIM_DELETE / ...
    HWND ownerWnd = nullptr;
    UINT uID = 0;
    UINT flags = 0;
    UINT callbackMessage = 0;
    HICON icon = nullptr;
    DWORD state = 0;
    DWORD stateMask = 0;
    DWORD version = 0;
    GUID guid = {};
    bool hasGuid = false;
    std::wstring tip;
    std::wstring exePath;
};

inline DWORD ReadDword(const BYTE* data, size_t offset) {
    DWORD value = 0;
    memcpy(&value, data + offset, sizeof(value));
    return value;
}

// Copies a fixed-width WCHAR field, tolerating a missing terminator.
inline std::wstring ReadFixedString(const BYTE* data,
                                    size_t offset,
                                    size_t maxChars) {
    const wchar_t* p = reinterpret_cast<const wchar_t*>(data + offset);
    size_t len = 0;
    while (len < maxChars && p[len] != L'\0') {
        len++;
    }
    return std::wstring(p, len);
}

// Returns false for anything that is not a tray notification, including appbar
// traffic and truncated or foreign payloads. `out` is only written on success.
bool ParseTrayNotification(ULONG_PTR copyDataId,
                           const void* payload,
                           size_t payloadSize,
                           TrayNotification* out) {
    if (copyDataId != kTrayCopyDataId || !payload || !out) {
        return false;
    }
    if (payloadSize < wire::kMinUsableSize) {
        return false;
    }

    const BYTE* data = static_cast<const BYTE*>(payload);
    if (ReadDword(data, wire::kSignature) != kTrayDataSignature) {
        return false;
    }

    // Only the record the offsets were captured from is read (DECISIONS 57). A
    // future Windows that changes it would have its fields decoded from the
    // wrong places - an identity that is not the icon's, a folded record that
    // replays garbage into Explorer - and staying within the buffer does not
    // make them right. So anything else is left to Explorer, untouched, and
    // the mod does nothing with it; the log says so once.
    const DWORD nidCbSize = payloadSize >= wire::kNidCbSize + sizeof(DWORD)
                                ? ReadDword(data, wire::kNidCbSize)
                                : 0;
    if (nidCbSize != wire::kExpectedNidCbSize || payloadSize != wire::kTotalSize) {
        static bool warned = false;
        if (!warned) {
            warned = true;
            Wh_Log(L"unexpected tray record shape: cbData=%zu (expected %zu), "
                   L"nid.cbSize=%u (expected %u) - leaving these to Explorer",
                   payloadSize, wire::kTotalSize, nidCbSize,
                   wire::kExpectedNidCbSize);
        }
        return false;
    }

    TrayNotification n;
    n.message = ReadDword(data, wire::kMessage);
    // USER handles are 32-bit values zero-extended into 64-bit handles.
    n.ownerWnd =
        reinterpret_cast<HWND>(static_cast<ULONG_PTR>(ReadDword(data, wire::kOwnerWnd)));
    n.uID = ReadDword(data, wire::kUID);
    n.flags = ReadDword(data, wire::kFlags);
    n.callbackMessage = ReadDword(data, wire::kCallbackMsg);
    n.icon =
        reinterpret_cast<HICON>(static_cast<ULONG_PTR>(ReadDword(data, wire::kIcon)));

    if (payloadSize >= wire::kTip + wire::kTipChars * sizeof(wchar_t)) {
        n.tip = ReadFixedString(data, wire::kTip, wire::kTipChars);
    }
    if (payloadSize >= wire::kStateMask + sizeof(DWORD)) {
        n.state = ReadDword(data, wire::kState);
        n.stateMask = ReadDword(data, wire::kStateMask);
    }
    if (payloadSize >= wire::kVersion + sizeof(DWORD)) {
        n.version = ReadDword(data, wire::kVersion);
    }
    if (payloadSize >= wire::kGuid + sizeof(GUID)) {
        memcpy(&n.guid, data + wire::kGuid, sizeof(GUID));
        n.hasGuid = (n.flags & NIF_GUID) != 0;
    }
    if (payloadSize >= wire::kExePath + wire::kExePathChars * sizeof(wchar_t)) {
        n.exePath = ReadFixedString(data, wire::kExePath, wire::kExePathChars);
    }

    *out = std::move(n);
    return true;
}

inline void WriteDword(BYTE* data, size_t offset, DWORD value) {
    memcpy(data + offset, &value, sizeof(value));
}

// Rewrites dwMessage in a copy of a stored payload, so an add can be replayed as
// a delete (and vice versa) without reconstructing the record.
std::vector<BYTE> PayloadWithMessage(const std::vector<BYTE>& source, DWORD message) {
    std::vector<BYTE> copy = source;
    if (copy.size() >= wire::kMessage + sizeof(DWORD)) {
        WriteDword(copy.data(), wire::kMessage, message);
    }
    return copy;
}

// ---------------------------------------------------------------------------
// The record that recreates an icon
//
// An icon is put back into the shell - moved back from the secondary tray, or
// restored when the mod unloads - by replaying a record as NIM_ADD. That record
// used to be simply the last message seen, and the last message is usually a
// partial NIM_MODIFY: the Claude usage monitor, for one, changes its picture and
// its tooltip in separate modifies. Replayed as an add, that recreated an icon
// with a picture and nothing else - no callback, no tooltip, no executable path
// (a modify never carries one) - and the picture handle had been destroyed by
// then. Measured with real shell32: flags 0x2, callback 0, the icon dead. So
// every message is folded into one record instead, field by field, exactly as
// the shell applies a partial modify to what it already holds.
// ---------------------------------------------------------------------------

// Flags that describe the icon rather than one message about it. NIF_INFO is a
// balloon, and replaying it would show the notification again; NIF_REALTIME
// only qualifies a balloon.
constexpr UINT kLastingFlags =
    NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_STATE | NIF_GUID | NIF_SHOWTIP;

void FoldTrayRecord(std::vector<BYTE>* state, const std::vector<BYTE>& incoming) {
    if (incoming.size() < wire::kMinUsableSize) {
        return;
    }
    const BYTE* in = incoming.data();
    const DWORD message = ReadDword(in, wire::kMessage);
    const DWORD flags = ReadDword(in, wire::kFlags);

    // An add describes the whole icon, so it starts the record afresh.
    if (message == NIM_ADD || state->size() != incoming.size()) {
        *state = incoming;
        WriteDword(state->data(), wire::kFlags, flags & kLastingFlags);
        return;
    }

    BYTE* out = state->data();
    auto copy = [&](size_t offset, size_t length) {
        if (incoming.size() >= offset + length) {
            memcpy(out + offset, in + offset, length);
        }
    };
    // Which icon it is. A GUID icon can be re-registered from a new window.
    copy(wire::kOwnerWnd, sizeof(DWORD));
    copy(wire::kUID, sizeof(DWORD));
    if (flags & NIF_MESSAGE) {
        copy(wire::kCallbackMsg, sizeof(DWORD));
    }
    if (flags & NIF_ICON) {
        copy(wire::kIcon, sizeof(DWORD));
    }
    if (flags & NIF_TIP) {
        copy(wire::kTip, wire::kTipChars * sizeof(wchar_t));
    }
    if ((flags & NIF_STATE) && incoming.size() >= wire::kStateMask + sizeof(DWORD)) {
        const DWORD mask = ReadDword(in, wire::kStateMask);
        WriteDword(out, wire::kState,
                   (ReadDword(out, wire::kState) & ~mask) |
                       (ReadDword(in, wire::kState) & mask));
        WriteDword(out, wire::kStateMask, ReadDword(out, wire::kStateMask) | mask);
    }
    if (flags & NIF_GUID) {
        copy(wire::kGuid, sizeof(GUID));
    }
    // Only an add carries the owner's path; a modify leaves the field empty,
    // and an empty one would cost the icon its identity with the shell.
    if (incoming.size() >= wire::kExePath + sizeof(wchar_t) &&
        (in[wire::kExePath] || in[wire::kExePath + 1])) {
        copy(wire::kExePath, wire::kExePathChars * sizeof(wchar_t));
    }
    WriteDword(out, wire::kFlags,
               (ReadDword(out, wire::kFlags) | flags) & kLastingFlags);
}

// The add that puts an icon back into the shell, drawn with `icon` - the mod's
// own copy - rather than whatever handle the application last sent, which it
// has usually destroyed by now. With no picture of its own to give, the mod
// gives none: the old handle may by then be another icon's (DECISIONS 72).
std::vector<BYTE> AddRecordFor(const std::vector<BYTE>& state, HICON icon) {
    std::vector<BYTE> record = PayloadWithMessage(state, NIM_ADD);
    if (record.size() >= wire::kFlags + sizeof(DWORD) &&
        record.size() >= wire::kIcon + sizeof(DWORD)) {
        const DWORD flags = ReadDword(record.data(), wire::kFlags);
        // USER handles are 32-bit values, as on the wire.
        WriteDword(record.data(), wire::kIcon,
                   static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(icon)));
        WriteDword(record.data(), wire::kFlags,
                   icon ? flags | NIF_ICON : flags & ~static_cast<DWORD>(NIF_ICON));
    }
    return record;
}

// How an add Explorer refused is asked about: the same icon as a modify, which
// Explorer takes only for an icon it has (DECISIONS 70). A balloon the add
// carried is left out, so asking does not show it, and so is the picture: it
// is asked later, and the handle the record names may be gone by then.
std::vector<BYTE> ProbeRecordFor(const std::vector<BYTE>& add) {
    std::vector<BYTE> record = PayloadWithMessage(add, NIM_MODIFY);
    if (record.size() >= wire::kFlags + sizeof(DWORD) &&
        record.size() >= wire::kIcon + sizeof(DWORD)) {
        WriteDword(record.data(), wire::kFlags,
                   ReadDword(record.data(), wire::kFlags) & kLastingFlags &
                       ~static_cast<DWORD>(NIF_ICON));
        WriteDword(record.data(), wire::kIcon, 0);
    }
    return record;
}

// A re-added icon starts at version 0, which changes the shape of every
// callback the application receives, so the version it asked for is replayed
// after the add.
std::vector<BYTE> SetVersionRecordFor(const std::vector<BYTE>& state, UINT version) {
    std::vector<BYTE> record = PayloadWithMessage(state, NIM_SETVERSION);
    if (record.size() >= wire::kVersion + sizeof(DWORD)) {
        WriteDword(record.data(), wire::kVersion, version);
    }
    return record;
}

// Sets or clears NIS_HIDDEN in a record, explicitly: the flag and the mask say
// so, and nothing else of the icon's state changes.
std::vector<BYTE> WithHiddenState(std::vector<BYTE> record, bool hidden) {
    if (record.size() >= wire::kStateMask + sizeof(DWORD)) {
        BYTE* data = record.data();
        WriteDword(data, wire::kFlags, ReadDword(data, wire::kFlags) | NIF_STATE);
        WriteDword(data, wire::kState,
                   (ReadDword(data, wire::kState) & ~static_cast<DWORD>(NIS_HIDDEN)) |
                       (hidden ? NIS_HIDDEN : 0));
        WriteDword(data, wire::kStateMask, ReadDword(data, wire::kStateMask) | NIS_HIDDEN);
    }
    return record;
}

// The copy of an icon Explorer holds hidden to show its balloons (DECISIONS 78):
// the whole icon, drawn with the mod's own picture, as `message` - the add
// that makes the copy, or the modify a balloon goes on.
std::vector<BYTE> HiddenCopyRecordFor(const std::vector<BYTE>& state,
                                      HICON icon,
                                      DWORD message) {
    return WithHiddenState(PayloadWithMessage(AddRecordFor(state, icon), message), true);
}

// A balloon on the hidden copy: `copy`, with the balloon of its application's
// `message` - its text, title, flags and picture, and whether it is to be shown
// only now (NIF_REALTIME). Everything else of the icon is the store's, so each
// balloon brings the copy up to date as well.
std::vector<BYTE> BalloonRecordFor(const std::vector<BYTE>& copy,
                                   const std::vector<BYTE>& message) {
    std::vector<BYTE> record = copy;
    if (record.size() < wire::kBalloonIcon + sizeof(DWORD) ||
        message.size() < wire::kBalloonIcon + sizeof(DWORD)) {
        return record;
    }
    auto take = [&](size_t offset, size_t length) {
        memcpy(record.data() + offset, message.data() + offset, length);
    };
    take(wire::kInfo, wire::kInfoChars * sizeof(wchar_t));
    take(wire::kVersion, sizeof(DWORD));  // uTimeout, for a balloon
    take(wire::kInfoTitle, wire::kInfoTitleChars * sizeof(wchar_t));
    take(wire::kInfoFlags, sizeof(DWORD));
    take(wire::kBalloonIcon, sizeof(DWORD));
    WriteDword(record.data(), wire::kFlags,
               ReadDword(record.data(), wire::kFlags) | NIF_INFO |
                   (ReadDword(message.data(), wire::kFlags) & NIF_REALTIME));
    return record;
}

// ---------------------------------------------------------------------------
// Shell_NotifyIconGetRect
//
// An application asks where its icon is on screen over the same window, with
// its own dwData. shell32 sends two messages and builds the RECT from the
// answers: the icon's size first, then its position, each packed like a mouse
// position in the LRESULT. A size of 0 is how the tray says it has no such
// icon, and the caller then gets E_FAIL. Captured by the wire probe, see
// tests/probe/probe-rect-output-26100.txt.
//
// It matters because Tauri's tray library - Telemachus and Desk Tray on this
// machine - asks before it handles any click on its icon, and drops the click
// when the answer is a failure. Explorer can only answer for icons it has.
// ---------------------------------------------------------------------------

constexpr ULONG_PTR kIconRectCopyDataId = 3;
constexpr DWORD kIconRectPosition = 1;
constexpr DWORD kIconRectSize = 2;

namespace rectwire {
constexpr size_t kSignature = 0x00;  // DWORD, kTrayDataSignature
constexpr size_t kPart = 0x04;       // DWORD, kIconRectPosition or kIconRectSize
constexpr size_t kOwnerWnd = 0x10;   // DWORD, HWND of the icon owner
constexpr size_t kUID = 0x14;        // DWORD
constexpr size_t kGuid = 0x18;       // GUID, all zeroes when not asked by GUID
}  // namespace rectwire

struct IconRectQuery {
    DWORD part = 0;
    HWND ownerWnd = nullptr;
    UINT uID = 0;
    GUID guid = {};
};

bool ParseIconRectQuery(ULONG_PTR copyDataId,
                        const void* payload,
                        size_t payloadSize,
                        IconRectQuery* out) {
    if (copyDataId != kIconRectCopyDataId || !payload || !out ||
        payloadSize < rectwire::kUID + sizeof(DWORD)) {
        return false;
    }
    const BYTE* data = static_cast<const BYTE*>(payload);
    if (ReadDword(data, rectwire::kSignature) != kTrayDataSignature) {
        return false;
    }
    IconRectQuery q;
    q.part = ReadDword(data, rectwire::kPart);
    if (q.part != kIconRectPosition && q.part != kIconRectSize) {
        return false;
    }
    q.ownerWnd = reinterpret_cast<HWND>(
        static_cast<ULONG_PTR>(ReadDword(data, rectwire::kOwnerWnd)));
    q.uID = ReadDword(data, rectwire::kUID);
    if (payloadSize >= rectwire::kGuid + sizeof(GUID)) {
        memcpy(&q.guid, data + rectwire::kGuid, sizeof(GUID));
    }
    *out = q;
    return true;
}

// Halves are signed, as a mouse position's are: a monitor left of the primary
// one has negative coordinates.
inline LRESULT PackScreenPair(int x, int y) {
    return static_cast<LRESULT>(static_cast<DWORD>(
        MAKELONG(static_cast<WORD>(x), static_cast<WORD>(y))));
}

LRESULT IconRectReply(const RECT& rect, DWORD part) {
    if (part == kIconRectSize) {
        const int width = rect.right - rect.left;
        const int height = rect.bottom - rect.top;
        // Never report an empty rect as found: a 0 here reads as "no icon".
        if (width <= 0 || height <= 0) {
            return 0;
        }
        return PackScreenPair(width, height);
    }
    return PackScreenPair(rect.left, rect.top);
}

// ============================================================================
// Section 2 - Settings
// ============================================================================

// Where an icon belongs.
//
// Trays are numbered. Tray 1 is Explorer's own. Split Tray's trays are 2 and
// up: one for every display other than the primary one, left to right, then the
// extra trays from the settings in the order they are listed (PlanTrays).
// `alsoPrimary` is "both": shown in `tray` and kept in Explorer's tray as well.
//
// The names Primary, Secondary and Both are the destinations the mod had when
// it had exactly two trays; "secondary" is tray 2, so settings and remembered
// placements written then still mean what they meant.
struct Destination {
    int tray = 1;
    bool alsoPrimary = false;

    static const Destination Primary;
    static const Destination Secondary;
    static const Destination Both;
    static Destination Tray(int number) { return {number < 1 ? 1 : number, false}; }

    bool operator==(const Destination&) const = default;
};

const Destination Destination::Primary{1, false};
const Destination Destination::Secondary{2, false};
const Destination Destination::Both{2, true};

enum class Corner { BottomRight, BottomLeft, TopRight, TopLeft };

struct RoutingRule {
    std::wstring pattern;  // file name, or a path fragment if it has a backslash
    Destination destination = Destination::Secondary;
};

// A tray in addition to the one on every display's taskbar, drawn as a floating
// panel: for a display without a taskbar, a second tray on the same display, or
// trying the mod out with one display.
struct ExtraTray {
    int display = 0;  // 0 = the primary display, otherwise 1-based, left to right
    Corner corner = Corner::BottomLeft;
    // Off rather than on, so that a missing value - which reads as 0 - leaves
    // the tray on: an entry saved before the switch existed has none.
    bool disabled = false;
};

struct Settings {
    Destination defaultTray = Destination::Primary;
    std::vector<RoutingRule> rules;
    std::vector<ExtraTray> extraTrays;

    Corner corner = Corner::BottomRight;
    int offsetX = 8;
    int offsetY = 8;
    int iconSize = 16;
    int cellSize = 28;
    int maxColumns = 12;
    COLORREF background = RGB(0x20, 0x20, 0x20);
    int opacity = 235;
    bool alwaysOnTop = true;
    bool showTooltips = true;
    bool mirrorHiddenIcons = true;
    bool repopulateOnLoad = true;
    bool embedInTaskbar = true;
    bool dumpXamlTree = false;
    int maxVisibleIcons = 8;
};

// For the log: "primary", "tray 3", or "tray 2 and primary".
std::wstring DestinationName(Destination destination) {
    if (destination.tray <= 1) {
        return L"primary";
    }
    std::wstring name = L"tray " + std::to_wstring(destination.tray);
    if (destination.alsoPrimary) {
        name += L" and primary";
    }
    return name;
}

// A number made only of digits, or -1.
int ParseTrayNumber(std::wstring_view digits) {
    if (digits.empty() || digits.size() > 3) {
        return -1;
    }
    int number = 0;
    for (wchar_t c : digits) {
        if (c < L'0' || c > L'9') {
            return -1;
        }
        number = number * 10 + (c - L'0');
    }
    return number;
}

// The settings' names: primary, secondary (tray 2), both (tray 2 and the
// primary tray), and trayN for any tray by number.
Destination ParseDestination(PCWSTR value, Destination fallback) {
    if (!value) {
        return fallback;
    }
    const std::wstring_view text(value);
    if (text == L"primary") {
        return Destination::Primary;
    }
    if (text == L"secondary") {
        return Destination::Secondary;
    }
    if (text == L"both") {
        return Destination::Both;
    }
    if (text.rfind(L"tray", 0) == 0) {
        const int number = ParseTrayNumber(text.substr(4));
        if (number >= 1) {
            return Destination::Tray(number);
        }
    }
    return fallback;
}

Corner ParseCorner(PCWSTR value) {
    if (value) {
        if (wcscmp(value, L"bottomLeft") == 0) {
            return Corner::BottomLeft;
        }
        if (wcscmp(value, L"topRight") == 0) {
            return Corner::TopRight;
        }
        if (wcscmp(value, L"topLeft") == 0) {
            return Corner::TopLeft;
        }
    }
    return Corner::BottomRight;
}

// Accepts RRGGBB, with or without a leading '#'. Returns `fallback` on anything
// it cannot read, so a typo cannot make the tray invisible.
COLORREF ParseHexColor(PCWSTR value, COLORREF fallback) {
    if (!value) {
        return fallback;
    }
    if (*value == L'#') {
        value++;
    }
    unsigned components[3] = {0, 0, 0};
    for (int i = 0; i < 3; i++) {
        unsigned v = 0;
        for (int d = 0; d < 2; d++) {
            wchar_t c = value[i * 2 + d];
            unsigned digit;
            if (c >= L'0' && c <= L'9') {
                digit = static_cast<unsigned>(c - L'0');
            } else if (c >= L'a' && c <= L'f') {
                digit = static_cast<unsigned>(c - L'a') + 10;
            } else if (c >= L'A' && c <= L'F') {
                digit = static_cast<unsigned>(c - L'A') + 10;
            } else {
                return fallback;
            }
            v = v * 16 + digit;
        }
        components[i] = v;
    }
    return RGB(components[0], components[1], components[2]);
}

int Clamp(int value, int low, int high) {
    return value < low ? low : (value > high ? high : value);
}

Settings LoadSettings() {
    Settings s;

    s.defaultTray = ParseDestination(
        WindhawkUtils::StringSetting::make(L"defaultTray"), Destination::Primary);

    // The display is text so an empty one can end the list, the way an empty
    // executable ends the routing rules: "primary", or a display number.
    for (int i = 0; i <= 16; i++) {
        auto display = WindhawkUtils::StringSetting::make(L"extraTrays[%d].display", i);
        if (!display.get() || !*display.get()) {
            break;
        }
        ExtraTray extra;
        const std::wstring_view text(display.get());
        extra.display = (text == L"primary") ? 0 : ParseTrayNumber(text);
        extra.corner = ParseCorner(
            WindhawkUtils::StringSetting::make(L"extraTrays[%d].corner", i));
        extra.disabled = Wh_GetIntSetting(L"extraTrays[%d].disabled", i) != 0;
        s.extraTrays.push_back(extra);
    }

    for (int i = 0;; i++) {
        auto exe = WindhawkUtils::StringSetting::make(L"perProcessRouting[%d].exe", i);
        if (!exe.get() || !*exe.get()) {
            break;
        }
        auto dest = WindhawkUtils::StringSetting::make(
            L"perProcessRouting[%d].destination", i);
        s.rules.push_back({exe.get(), ParseDestination(dest, Destination::Secondary)});
        if (i > 256) {  // defensive: never spin on a malformed settings blob
            break;
        }
    }

    // What was actually read, not what was meant to be there. A rule that is in
    // the registry but not in the mod's hands looks exactly like a rule that
    // does not match, and the two were indistinguishable in the log.
    Wh_Log(L"settings: defaultTray=%s, %zu per-process rule(s), %zu extra tray(s)",
           DestinationName(s.defaultTray).c_str(), s.rules.size(),
           s.extraTrays.size());
    for (size_t i = 0; i < s.rules.size(); i++) {
        Wh_Log(L"settings:   rule %zu: '%s' -> %s", i, s.rules[i].pattern.c_str(),
               DestinationName(s.rules[i].destination).c_str());
    }

    s.corner = ParseCorner(WindhawkUtils::StringSetting::make(L"trayPosition"));
    s.offsetX = Clamp(Wh_GetIntSetting(L"offsetX"), -4096, 4096);
    s.offsetY = Clamp(Wh_GetIntSetting(L"offsetY"), -4096, 4096);
    s.iconSize = Clamp(Wh_GetIntSetting(L"iconSize"), 8, 128);
    s.cellSize = Clamp(Wh_GetIntSetting(L"cellSize"), s.iconSize + 2, 256);
    s.maxColumns = Clamp(Wh_GetIntSetting(L"maxColumns"), 1, 64);
    s.background = ParseHexColor(WindhawkUtils::StringSetting::make(L"backgroundColor"),
                                 RGB(0x20, 0x20, 0x20));
    s.opacity = Clamp(Wh_GetIntSetting(L"opacity"), 16, 255);
    s.alwaysOnTop = Wh_GetIntSetting(L"alwaysOnTop") != 0;
    s.showTooltips = Wh_GetIntSetting(L"showTooltips") != 0;
    s.mirrorHiddenIcons = Wh_GetIntSetting(L"mirrorHiddenIcons") != 0;
    s.repopulateOnLoad = Wh_GetIntSetting(L"repopulateOnLoad") != 0;
    s.embedInTaskbar = Wh_GetIntSetting(L"embedInTaskbar") != 0;
    s.dumpXamlTree = Wh_GetIntSetting(L"dumpXamlTree") != 0;
    s.maxVisibleIcons = Clamp(Wh_GetIntSetting(L"maxVisibleIcons"), 0, 64);

    return s;
}

// ============================================================================
// Section 3 - Routing
// ============================================================================

// Returns the substring after the last path separator.
std::wstring_view FileNameOf(std::wstring_view path) {
    size_t pos = path.find_last_of(L"\\/");
    return pos == std::wstring_view::npos ? path : path.substr(pos + 1);
}

bool EqualsInsensitive(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (towlower(a[i]) != towlower(b[i])) {
            return false;
        }
    }
    return true;
}

bool ContainsInsensitive(std::wstring_view haystack, std::wstring_view needle) {
    if (needle.empty() || needle.size() > haystack.size()) {
        return false;
    }
    for (size_t i = 0; i + needle.size() <= haystack.size(); i++) {
        if (EqualsInsensitive(haystack.substr(i, needle.size()), needle)) {
            return true;
        }
    }
    return false;
}

// A rule with a separator in it matches anywhere in the full path; otherwise it
// matches the executable's file name exactly. First match wins.
Destination ResolveDestination(const Settings& settings, std::wstring_view exePath) {
    const std::wstring_view fileName = FileNameOf(exePath);
    for (const auto& rule : settings.rules) {
        if (rule.pattern.empty()) {
            continue;
        }
        const bool isPathPattern =
            rule.pattern.find(L'\\') != std::wstring::npos ||
            rule.pattern.find(L'/') != std::wstring::npos;
        if (isPathPattern ? ContainsInsensitive(exePath, rule.pattern)
                          : EqualsInsensitive(fileName, rule.pattern)) {
            return rule.destination;
        }
    }
    return settings.defaultTray;
}

// What the mod actually does with a notification, once it is known whether the
// destination tray exists right now. Requirement: an icon whose tray is missing
// - its display unplugged, asleep, or never there - behaves as `primary`, and
// goes back when the tray does.
struct RoutingPlan {
    bool forwardToShell = true;  // let the real tray see it
    bool mirror = false;         // show it in one of Split Tray's trays
    int tray = 0;                // which one, when mirrored
};

RoutingPlan PlanFor(Destination destination, bool trayAvailable) {
    if (destination.tray <= 1 || !trayAvailable) {
        return {true, false, 0};
    }
    return {destination.alsoPrimary, true, destination.tray};
}

// ============================================================================
// Section 4 - Monitors and layout
// ============================================================================

struct MonitorInfoEntry {
    HMONITOR handle = nullptr;
    RECT workArea = {};
    bool primary = false;
    UINT dpi = 96;
    // Which monitor it is, whatever its place in the arrangement: the path
    // Windows gives its monitor device, or the display's device name when there
    // is none (DECISIONS 86).
    std::wstring id;
};

// The identity of the monitor on display device `device` ("\\.\DISPLAY2"):
// its device interface path, which names the monitor and the connection it
// is on, and stays the same across restarts and rearrangements. The device's
// own name when Windows gives no monitor for it.
std::wstring MonitorIdentity(const wchar_t* device) {
    DISPLAY_DEVICEW monitor = {sizeof(monitor)};
    if (EnumDisplayDevicesW(device, 0, &monitor, EDD_GET_DEVICE_INTERFACE_NAME) &&
        monitor.DeviceID[0]) {
        return monitor.DeviceID;
    }
    return device;
}

UINT GetMonitorDpi(HMONITOR monitor) {
    using GetDpiForMonitorProc = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static GetDpiForMonitorProc proc = []() -> GetDpiForMonitorProc {
        HMODULE shcore = LoadLibraryW(L"shcore.dll");
        return shcore ? reinterpret_cast<GetDpiForMonitorProc>(
                            GetProcAddress(shcore, "GetDpiForMonitor"))
                      : nullptr;
    }();
    UINT dpiX = 96, dpiY = 96;
    if (proc && proc(monitor, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY) == S_OK) {
        return dpiX;
    }
    return 96;
}

BOOL CALLBACK CollectMonitorProc(HMONITOR monitor, HDC, LPRECT, LPARAM param) {
    auto* list = reinterpret_cast<std::vector<MonitorInfoEntry>*>(param);
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(monitor, &mi)) {
        MonitorInfoEntry entry;
        entry.handle = monitor;
        entry.workArea = mi.rcWork;
        entry.primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        entry.dpi = GetMonitorDpi(monitor);
        entry.id = MonitorIdentity(mi.szDevice);
        list->push_back(entry);
    }
    return TRUE;
}

std::vector<MonitorInfoEntry> EnumerateMonitors() {
    std::vector<MonitorInfoEntry> list;
    EnumDisplayMonitors(nullptr, nullptr, CollectMonitorProc,
                        reinterpret_cast<LPARAM>(&list));
    // EnumDisplayMonitors order is not documented as stable; sorting by position
    // gives the user an index that matches how the displays are arranged.
    std::sort(list.begin(), list.end(),
              [](const MonitorInfoEntry& a, const MonitorInfoEntry& b) {
                  if (a.workArea.left != b.workArea.left) {
                      return a.workArea.left < b.workArea.left;
                  }
                  return a.workArea.top < b.workArea.top;
              });
    return list;
}

// A display named in the settings: 0 is the primary one, otherwise a 1-based
// index over the displays sorted left to right (EnumerateMonitors). False when
// there is no such display, which is the signal to fall back to the primary
// tray.
bool ResolveDisplay(const std::vector<MonitorInfoEntry>& monitors,
                    int display,
                    MonitorInfoEntry* out) {
    if (display == 0) {
        for (const auto& m : monitors) {
            if (m.primary) {
                *out = m;
                return true;
            }
        }
        return false;
    }
    if (display < 0 || static_cast<size_t>(display) > monitors.size()) {
        return false;
    }
    *out = monitors[static_cast<size_t>(display) - 1];
    return true;
}

// One of Split Tray's trays.
struct TrayTarget {
    int number = 0;           // 2 and up; tray 1 is Explorer's own
    bool forDisplay = false;  // the tray of a non-primary display, not an extra
    bool available = false;   // its display is connected, and it is not disabled
    bool disabled = false;    // an extra tray the user switched off
    MonitorInfoEntry monitor;
    Corner corner = Corner::BottomRight;  // where it goes when it floats
};

// Which display has which tray: each slot holds the identity of the display
// whose tray is number 2 + its index (DECISIONS 86). A display keeps its slot
// while it is away. One seen for the first time takes, from the left, the slot
// of a display that is not connected now - so a laptop's external display is
// tray 2 whichever display it meets - and a new slot only when there is none,
// with more displays connected at once than ever before. The primary display
// holds none: it has Explorer's tray. Returns whether a slot changed.
bool AssignDisplaySlots(const std::vector<MonitorInfoEntry>& monitors,
                        std::vector<std::wstring>* slots) {
    std::vector<const MonitorInfoEntry*> newcomers;
    std::vector<bool> held(slots->size(), false);
    for (const auto& monitor : monitors) {
        if (monitor.primary) {
            continue;
        }
        bool found = false;
        for (size_t i = 0; i < slots->size(); i++) {
            if (!held[i] && (*slots)[i] == monitor.id) {
                held[i] = true;
                found = true;
                break;
            }
        }
        if (!found) {
            newcomers.push_back(&monitor);
        }
    }
    bool changed = false;
    for (const MonitorInfoEntry* monitor : newcomers) {
        const auto free = std::find(held.begin(), held.end(), false);
        if (free != held.end()) {
            const size_t index = static_cast<size_t>(free - held.begin());
            (*slots)[index] = monitor->id;
            held[index] = true;
        } else {
            slots->push_back(monitor->id);
            held.push_back(true);
        }
        changed = true;
    }
    return changed;
}

// Which trays there are, in number order.
//
// A tray for each display slot (AssignDisplaySlots), there while its display
// is connected and not the primary one, then each extra tray from the
// settings. A tray keeps its number while its display is missing, or an extra
// tray while it is disabled, and is marked unavailable, so the trays after it
// do not shift (DECISIONS 86). With one display and no extras there are none,
// and every icon stays in the primary tray.
std::vector<TrayTarget> PlanTrays(const std::vector<MonitorInfoEntry>& monitors,
                                  const Settings& settings,
                                  const std::vector<std::wstring>& slots) {
    std::vector<TrayTarget> trays;
    std::vector<bool> placed(monitors.size(), false);
    for (const std::wstring& slot : slots) {
        TrayTarget tray;
        tray.number = static_cast<int>(trays.size()) + 2;
        tray.forDisplay = true;
        tray.corner = settings.corner;
        for (size_t i = 0; i < monitors.size(); i++) {
            if (!placed[i] && !monitors[i].primary && monitors[i].id == slot) {
                placed[i] = true;
                tray.available = true;
                tray.monitor = monitors[i];
                break;
            }
        }
        trays.push_back(tray);
    }
    for (const auto& extra : settings.extraTrays) {
        TrayTarget tray;
        tray.number = static_cast<int>(trays.size()) + 2;
        tray.corner = extra.corner;
        tray.disabled = extra.disabled;
        tray.available =
            ResolveDisplay(monitors, extra.display, &tray.monitor) && !extra.disabled;
        trays.push_back(tray);
    }
    return trays;
}

// The trays with every display seen for the first time: numbered from the
// left, as a fresh install numbers them.
std::vector<TrayTarget> PlanTrays(const std::vector<MonitorInfoEntry>& monitors,
                                  const Settings& settings) {
    std::vector<std::wstring> slots;
    AssignDisplaySlots(monitors, &slots);
    return PlanTrays(monitors, settings, slots);
}

// Where a tray is, in words a menu can use: "display to the left", "floating,
// primary display". Displays have no names worth showing, and their numbers
// follow the arrangement rather than what Windows' own settings call them, so
// the direction from the primary display is what identifies one.
std::wstring DescribeTrayPlace(const TrayTarget& tray, const MonitorInfoEntry& primary) {
    if (tray.disabled) {
        return L"disabled";
    }
    std::wstring where;
    if (!tray.available) {
        where = L"display not connected";
    } else if (tray.monitor.primary) {
        where = L"primary display";
    } else {
        const RECT& a = tray.monitor.workArea;
        const RECT& p = primary.workArea;
        const LONG dx = (a.left + a.right) / 2 - (p.left + p.right) / 2;
        const LONG dy = (a.top + a.bottom) / 2 - (p.top + p.bottom) / 2;
        if (std::labs(dx) >= std::labs(dy)) {
            where = dx < 0 ? L"display to the left" : L"display to the right";
        } else {
            where = dy < 0 ? L"display above" : L"display below";
        }
    }
    return tray.forDisplay ? where : L"floating, " + where;
}

struct TrayLayout {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int cell = 28;
    int icon = 16;
    int columns = 0;
    int rows = 0;
};

int ScaleForDpi(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi), 96);
}

// Lays the icons out in rows of at most maxColumns, anchored to `corner` of the
// work area. Sizes are DPI-scaled for the target monitor.
TrayLayout ComputeLayout(const RECT& workArea,
                         int iconCount,
                         const Settings& settings,
                         UINT dpi,
                         Corner corner) {
    TrayLayout layout;
    layout.cell = ScaleForDpi(settings.cellSize, dpi);
    layout.icon = ScaleForDpi(settings.iconSize, dpi);

    if (iconCount <= 0) {
        return layout;
    }

    layout.columns = std::min(iconCount, settings.maxColumns);
    layout.rows = (iconCount + settings.maxColumns - 1) / settings.maxColumns;
    layout.width = layout.columns * layout.cell;
    layout.height = layout.rows * layout.cell;

    const int offsetX = ScaleForDpi(settings.offsetX, dpi);
    const int offsetY = ScaleForDpi(settings.offsetY, dpi);

    switch (corner) {
        case Corner::BottomRight:
            layout.x = workArea.right - offsetX - layout.width;
            layout.y = workArea.bottom - offsetY - layout.height;
            break;
        case Corner::BottomLeft:
            layout.x = workArea.left + offsetX;
            layout.y = workArea.bottom - offsetY - layout.height;
            break;
        case Corner::TopRight:
            layout.x = workArea.right - offsetX - layout.width;
            layout.y = workArea.top + offsetY;
            break;
        case Corner::TopLeft:
            layout.x = workArea.left + offsetX;
            layout.y = workArea.top + offsetY;
            break;
    }
    return layout;
}

// At the corner the settings choose for the display trays.
TrayLayout ComputeLayout(const RECT& workArea,
                         int iconCount,
                         const Settings& settings,
                         UINT dpi) {
    return ComputeLayout(workArea, iconCount, settings, dpi, settings.corner);
}

// Which icon index sits under a client point, or -1. Kept next to the layout so
// painting and hit testing cannot drift apart.
int HitTestCell(const TrayLayout& layout, int clientX, int clientY, int iconCount) {
    if (layout.cell <= 0 || iconCount <= 0) {
        return -1;
    }
    if (clientX < 0 || clientY < 0 || clientX >= layout.width ||
        clientY >= layout.height) {
        return -1;
    }
    const int column = clientX / layout.cell;
    const int row = clientY / layout.cell;
    const int index = row * layout.columns + column;
    return index < iconCount ? index : -1;
}

// Where the icon at `index` is painted, in screen coordinates. The inverse of
// HitTestCell, and next to it for the same reason.
bool CellScreenRect(const TrayLayout& layout, int index, int iconCount, RECT* out) {
    if (layout.cell <= 0 || layout.columns <= 0 || index < 0 || index >= iconCount) {
        return false;
    }
    const int column = index % layout.columns;
    const int row = index / layout.columns;
    if (row >= layout.rows) {
        return false;
    }
    out->left = layout.x + column * layout.cell;
    out->top = layout.y + row * layout.cell;
    out->right = out->left + layout.cell;
    out->bottom = out->top + layout.cell;
    return true;
}

// Bounds measured inside a XAML island - device-independent pixels from its
// top-left corner - in screen pixels, given where the island's window is.
// Each edge is rounded on its own, so a row of cells tiles without gaps.
RECT IslandBoundsToScreen(POINT islandOrigin,
                          double x,
                          double y,
                          double width,
                          double height,
                          double scale) {
    RECT r;
    r.left = islandOrigin.x + static_cast<LONG>(std::lround(x * scale));
    r.top = islandOrigin.y + static_cast<LONG>(std::lround(y * scale));
    r.right = islandOrigin.x + static_cast<LONG>(std::lround((x + width) * scale));
    r.bottom = islandOrigin.y + static_cast<LONG>(std::lround((y + height) * scale));
    return r;
}

// ============================================================================
// Section 5 - Icon store
// ============================================================================

// An icon handle that is destroyed with its owner.
//
// For whatever holds a picture once the store's lock is released: a snapshot
// the mod's own thread draws, a replay on its way to Explorer. The store
// destroys the picture it replaces - on the taskbar's thread, whenever an
// application changes its icon - so a bare handle copied out of it can be gone
// by the time it is used (DECISIONS 60).
class OwnedIcon {
public:
    OwnedIcon() = default;
    explicit OwnedIcon(HICON icon) : icon_(icon) {}
    OwnedIcon(OwnedIcon&& other) noexcept : icon_(std::exchange(other.icon_, nullptr)) {}
    OwnedIcon& operator=(OwnedIcon&& other) noexcept {
        if (this != &other) {
            reset();
            icon_ = std::exchange(other.icon_, nullptr);
        }
        return *this;
    }
    OwnedIcon(const OwnedIcon&) = delete;
    OwnedIcon& operator=(const OwnedIcon&) = delete;
    ~OwnedIcon() { reset(); }

    // A copy of `icon` for this owner. Taken while the store's lock is held.
    static OwnedIcon CopyOf(HICON icon) {
        return OwnedIcon(icon ? CopyIcon(icon) : nullptr);
    }

    HICON get() const { return icon_; }
    void reset() {
        if (icon_) {
            DestroyIcon(icon_);
            icon_ = nullptr;
        }
    }

private:
    HICON icon_ = nullptr;
};

struct MirroredIcon {
    // Identity. Modern applications identify an icon by GUID and may send uID 0,
    // so both keys have to be supported.
    bool hasGuid = false;
    GUID guid = {};
    HWND ownerWnd = nullptr;
    UINT uID = 0;

    // Which icon this is while it lives, for anything that has to find it again
    // later - a tray cell, a menu item. Unique, unlike the placement key, which
    // two running copies of the same application share.
    //
    // Cells used to carry their position instead, and a click looked the icon
    // up at that position in the store. The trays draw icons in the user's
    // order, not the store's, so after a reorder, or an icon moved out and back,
    // a click on one icon could reach another application.
    uint64_t serial = 0;

    // Which of Split Tray's trays shows it, or 0 when none does.
    int shownTray = 0;

    HICON icon = nullptr;  // our own copy, owned by this mod
    // Counts the pictures it has had, so a tray in the taskbar can tell which
    // one a cell shows (DECISIONS 82).
    uint64_t pictureRevision = 0;
    std::wstring tip;
    std::wstring exePath;
    UINT callbackMessage = 0;
    UINT version = 0;
    DWORD state = 0;

    // The routing decision is taken once, at NIM_ADD, and reused for every later
    // message about this icon. Re-deciding mid-life would leave the primary tray
    // holding an icon it can no longer be told about.
    Destination destination = Destination::Primary;

    // Whether Explorer has the icon, as far as its answers say: to its
    // application's own adds and modifies, and to the mod's (DECISIONS 70).
    // Only the taskbar's thread changes it.
    bool forwardedToShell = true;

    // Whether the icon is meant to be with Explorer. A move changes this and
    // nothing else; the taskbar's thread then settles the difference, handing
    // Explorer the icon as it is by then (SettleShellIcons, DECISIONS 66).
    bool shellTarget = true;

    // How many times Explorer has refused to take the icon back, or its
    // version, since it was last asked to. After kShellAttempts it is left as
    // Explorer has it - one Explorer does not have, to its application to add
    // again (OwedToShell).
    int shellRefusals = 0;

    // Counts what its application has changed, so the taskbar's thread can
    // tell whether anything arrived while Explorer was taking the icon back.
    uint64_t revision = 0;

    // Whether Explorer has less of the icon than the store: something arrived
    // while Explorer was taking it back - swallowed, since Explorer did not
    // have it yet - or Explorer did not take its version. The taskbar's thread
    // then hands Explorer the whole record again (DECISIONS 71).
    bool shellBehind = false;

    // Whether Explorer refused its application's add and has not yet been
    // asked if that was because it has the icon already. The taskbar's next
    // round asks, after the messages waiting then (DECISIONS 70).
    bool shellUnconfirmed = false;

    // Whether Explorer holds a hidden copy of an icon it does not show, to
    // show its balloons: Windows shows a balloon, as a notification, only for
    // an icon Explorer holds (DECISIONS 78). Made at the icon's first balloon;
    // taken out by its application's delete, and shown - the whole icon as a
    // modify - when the icon moves into Explorer's tray. Taskbar's thread only.
    bool shellHidden = false;

    // Whether that decision was made with anything to decide from.
    //
    // The first message the mod sees about an icon is not always its NIM_ADD.
    // An application that updates a live icon - SystemInformer's four graphs do
    // it every second - lands a NIM_MODIFY first, and a modify carries no
    // executable path, so the rules have nothing to match and the icon is filed
    // under the default tray. Sticky routing then made that guess permanent:
    // measured on this machine, uID 2 happened to arrive as an add and moved,
    // while 3, 5 and 14 arrived as modifies and never did, however the rule was
    // written. The decision is now deferred until a message actually carries the
    // path, and applied through the same replay the settings use.
    bool destinationDecided = false;

    // Whether its application has asked where it is (Shell_NotifyIconGetRect)
    // yet, so the answer is logged once rather than on every click.
    bool askedWhere = false;

    // Every message about the icon folded into one wire record (FoldTrayRecord),
    // kept so the mod can replay an add into the real tray, or retract one from
    // it, when routing or monitor availability changes. Not the last message:
    // that is usually a partial modify, and replaying one recreates an icon
    // with most of it missing.
    std::vector<BYTE> payload;
};

// A GUID of all zeroes is not an identity.
//
// NIF_GUID only says the caller filled the flag in, not that it put anything in
// the field, and applications do set the flag over an empty GUID. Treating that
// as an identity makes every such icon the same icon: on this machine all four
// of SystemInformer's icons and several of Explorer's own arrive that way, so
// the first one decided the tray for all of them and the other three inherited
// it as a "sticky" decision. Measured, not supposed - the log prints the GUID.
bool IsEmptyGuid(const GUID& guid) {
    static const GUID empty = {};
    return memcmp(&guid, &empty, sizeof(GUID)) == 0;
}

bool UsableGuid(bool hasGuid, const GUID& guid) {
    return hasGuid && !IsEmptyGuid(guid);
}

// Set when an icon's routing was settled after the fact, so the caller knows to
// run the same replay a settings change would. Not done inside the notification
// handler: decision 5 says routing changes are applied by replaying a stored
// payload, never by rewriting the decision mid-message.
std::atomic<bool> g_routingNeedsReapply{false};

bool SameIcon(const MirroredIcon& icon, const TrayNotification& n) {
    if (UsableGuid(icon.hasGuid, icon.guid) && UsableGuid(n.hasGuid, n.guid)) {
        return memcmp(&icon.guid, &n.guid, sizeof(GUID)) == 0;
    }
    return icon.ownerWnd == n.ownerWnd && icon.uID == n.uID;
}

// The same rule for Shell_NotifyIconGetRect's question, which has no NIF_GUID
// flag: a GUID that is filled in is the identity.
bool SameIcon(const MirroredIcon& icon, const IconRectQuery& query) {
    if (UsableGuid(icon.hasGuid, icon.guid) && !IsEmptyGuid(query.guid)) {
        return memcmp(&icon.guid, &query.guid, sizeof(GUID)) == 0;
    }
    return icon.ownerWnd == query.ownerWnd && icon.uID == query.uID;
}

// ---------------------------------------------------------------------------
// Per-icon placement
//
// Two trays, not a mirror. The per-process rules in section 3 are a starting
// position; anything the user moves by hand overrides them and is remembered.
//
// The key has to survive the owning application restarting, so it is the icon's
// GUID where it has one, otherwise the executable's file name and uID. A window
// handle would not do: it is a fresh handle every launch, so a placement keyed
// on it would be forgotten whenever the application came back - which is exactly
// the case this exists to handle.
// ---------------------------------------------------------------------------

constexpr PCWSTR kPlacementValue = L"iconPlacement";

std::map<std::wstring, Destination, std::less<>> g_placements;
bool g_placementsLoaded = false;

// How a placement is stored: 'p' and 's' for trays 1 and 2, as the two-tray
// versions wrote them, and the number for any tray after that. A hand-made
// placement is always a single tray; "both" only ever comes from the rules.
std::wstring PlacementCode(Destination destination) {
    if (destination.tray <= 1) {
        return L"p";
    }
    if (destination.tray == 2) {
        return L"s";
    }
    return std::to_wstring(destination.tray);
}

Destination ParsePlacementCode(std::wstring_view code) {
    if (code == L"s") {
        return Destination::Secondary;
    }
    const int number = ParseTrayNumber(code);
    return number >= 2 ? Destination::Tray(number) : Destination::Primary;
}

std::wstring FormatGuidKey(const GUID& guid) {
    WCHAR buffer[48];
    swprintf_s(buffer, L"{%08lX-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X}",
               guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1],
               guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5],
               guid.Data4[6], guid.Data4[7]);
    return buffer;
}

std::wstring MakeStableKey(bool hasGuid,
                           const GUID& guid,
                           std::wstring_view exePath,
                           UINT uID) {
    // Same reasoning as SameIcon: an empty GUID would make one key for every
    // application that sets NIF_GUID without filling the field in, so moving one
    // of those icons would move all of them.
    if (UsableGuid(hasGuid, guid)) {
        return FormatGuidKey(guid);
    }
    std::wstring key{FileNameOf(exePath)};
    if (key.empty()) {
        key = exePath;
    }
    key += L'#';
    key += std::to_wstring(uID);
    return key;
}

std::wstring StableKeyOf(const MirroredIcon& icon) {
    return MakeStableKey(icon.hasGuid, icon.guid, icon.exePath, icon.uID);
}

std::wstring StableKeyOf(const TrayNotification& n) {
    return MakeStableKey(n.hasGuid, n.guid, n.exePath, n.uID);
}

// The mod's own storage, whatever a value's length. Windhawk hands back an
// empty string, not a cut one, when the buffer is too small, and does not say
// how big the value is. So the read starts small and grows, and a value is
// never cut to fit: placements used to be saved up to about 7,900 characters
// and read through an 8K buffer, which dropped every placement past the limit
// and, once a long key took the value past 8K, all of them.
constexpr size_t kStoredStringBuffers[] = {8192, 65536, 1048576};

std::wstring ReadStoredString(PCWSTR name) {
    for (size_t chars : kStoredStringBuffers) {
        std::wstring buffer(chars, L'\0');
        const size_t length = Wh_GetStringValue(name, buffer.data(), chars);
        if (length > 0) {
            buffer.resize(length);
            return buffer;
        }
    }
    return {};
}

void WriteStoredString(PCWSTR name, const std::wstring& value) {
    if (value.size() >= std::end(kStoredStringBuffers)[-1]) {
        Wh_Log(L"%s is %zu characters, more than can be read back; it will be "
               L"lost at the next start",
               name, value.size());
    }
    Wh_SetStringValue(name, value.c_str());
}

void LoadPlacements() {
    if (g_placementsLoaded) {
        return;
    }
    g_placementsLoaded = true;

    const std::wstring stored = ReadStoredString(kPlacementValue);
    std::wstring_view remaining(stored);
    while (!remaining.empty()) {
        const size_t lineEnd = remaining.find(L'\n');
        std::wstring_view line = remaining.substr(0, lineEnd);
        const size_t split = line.rfind(L'=');
        if (split != std::wstring_view::npos && split + 1 < line.size()) {
            g_placements.emplace(std::wstring(line.substr(0, split)),
                                 ParsePlacementCode(line.substr(split + 1)));
        }
        if (lineEnd == std::wstring_view::npos) {
            break;
        }
        remaining.remove_prefix(lineEnd + 1);
    }
    Wh_Log(L"loaded %zu remembered icon placement(s)", g_placements.size());
}

void SavePlacements() {
    std::wstring joined;
    for (const auto& [key, destination] : g_placements) {
        if (!joined.empty()) {
            joined += L'\n';
        }
        joined += key;
        joined += L'=';
        joined += PlacementCode(destination);
    }
    WriteStoredString(kPlacementValue, joined);
}

// The user's choice wins; the rules decide only what has never been moved.
Destination ResolvePlacement(const Settings& settings,
                             std::wstring_view key,
                             std::wstring_view exePath) {
    LoadPlacements();
    auto it = g_placements.find(key);
    if (it != g_placements.end()) {
        return it->second;
    }
    return ResolveDestination(settings, exePath);
}

void RememberPlacement(std::wstring_view key, Destination destination) {
    LoadPlacements();
    g_placements[std::wstring(key)] = destination;
    SavePlacements();
}

// ---------------------------------------------------------------------------
// Icons the user has put in the overflow
//
// Which icons are hidden was decided purely by count - the first maxVisibleIcons
// were shown and the rest went to the chevron - so there was no way to say "this
// one belongs in the popup and that one on the bar". That is most of what a
// notification area is for.
//
// Kept separate from placement: an icon is in a tray, and within the secondary
// tray it is on the bar or in the overflow. Two questions, two answers.
// ---------------------------------------------------------------------------

constexpr PCWSTR kHiddenValue = L"iconHidden";

std::set<std::wstring, std::less<>> g_hidden;
bool g_hiddenLoaded = false;
// Read on the taskbar thread when the tray is redrawn, written from there by the
// tray menu and from the mod's own thread by the arrange window. Its own lock,
// always taken innermost, so it cannot order against g_mutex the wrong way.
std::mutex g_hiddenMutex;

void LoadHidden() {
    if (g_hiddenLoaded) {
        return;
    }
    g_hiddenLoaded = true;

    const std::wstring stored = ReadStoredString(kHiddenValue);
    std::wstring_view remaining(stored);
    while (!remaining.empty()) {
        const size_t lineEnd = remaining.find(L'\n');
        std::wstring_view line = remaining.substr(0, lineEnd);
        if (!line.empty()) {
            g_hidden.emplace(line);
        }
        if (lineEnd == std::wstring_view::npos) {
            break;
        }
        remaining.remove_prefix(lineEnd + 1);
    }
    Wh_Log(L"loaded %zu icon(s) kept in the overflow", g_hidden.size());
}

void SaveHidden() {
    std::wstring joined;
    for (const auto& key : g_hidden) {
        if (!joined.empty()) {
            joined += L'\n';
        }
        joined += key;
    }
    WriteStoredString(kHiddenValue, joined);
}

bool IsIconHidden(std::wstring_view key) {
    std::lock_guard<std::mutex> lock(g_hiddenMutex);
    LoadHidden();
    return g_hidden.find(key) != g_hidden.end();
}

void SetIconHidden(std::wstring_view key, bool hidden) {
    std::lock_guard<std::mutex> lock(g_hiddenMutex);
    LoadHidden();
    if (hidden) {
        g_hidden.emplace(key);
    } else {
        auto it = g_hidden.find(key);
        if (it != g_hidden.end()) {
            g_hidden.erase(it);
        }
    }
    SaveHidden();
}

// Which icons go on the bar and which into the overflow, by index into `keys`.
//
// The user's choice is applied first and the count second: an icon they hid is
// in the overflow however much room there is, and the row limit then only
// decides among the icons they want shown. Before this, the count was the only
// thing that decided, so the user could not choose at all. Pure, so that order
// of precedence is tested rather than read off the drawing code.
struct BarSplit {
    std::vector<size_t> shown;
    std::vector<size_t> overflow;
};

template <typename IsHidden>
BarSplit SplitBarAndOverflow(const std::vector<std::wstring>& keys,
                             IsHidden isHidden,
                             int maxVisible) {
    BarSplit split;
    for (size_t i = 0; i < keys.size(); i++) {
        (isHidden(keys[i]) ? split.overflow : split.shown).push_back(i);
    }
    if (maxVisible > 0 && split.shown.size() > static_cast<size_t>(maxVisible)) {
        split.overflow.insert(
            split.overflow.end(),
            split.shown.begin() + static_cast<ptrdiff_t>(maxVisible),
            split.shown.end());
        split.shown.resize(static_cast<size_t>(maxVisible));
    }
    return split;
}

void ShowAllHiddenIcons() {
    std::lock_guard<std::mutex> lock(g_hiddenMutex);
    LoadHidden();
    const size_t count = g_hidden.size();
    g_hidden.clear();
    SaveHidden();
    Wh_Log(L"brought %zu icon(s) back out of the overflow", count);
}

// Forget every hand-made placement, so the per-process rules decide again.
//
// There was no way back: once an icon had been moved, that choice outranked the
// rules for good, and the only way to undo it was to move every icon back one
// at a time - or to know where the mod keeps its storage.
void ForgetAllPlacements() {
    LoadPlacements();
    const size_t count = g_placements.size();
    g_placements.clear();
    SavePlacements();
    Wh_Log(L"forgot %zu remembered icon placement(s)", count);
}

// ---------------------------------------------------------------------------
// Forgetting icons not seen for a year (DECISIONS 90)
//
// What the mod remembers about an icon - where the user moved it, that they hid
// it, its place in the taskbar's order - was kept for ever, for icons whose
// applications were long gone. So the day each icon was last seen is kept too,
// and at load whatever belongs to an icon not seen for a year is forgotten. One
// with no sighting yet - everything, the first time this version loads - is
// given one that day, so the upgrade itself forgets nothing. The displays'
// tray numbers are not touched: a display keeps its number while it is away,
// however long (DECISIONS 86).
// ---------------------------------------------------------------------------

constexpr PCWSTR kLastSeenValue = L"iconLastSeen";
// The taskbar's icon order, which section 10 keeps and this forgets from.
constexpr PCWSTR kIconOrderValue = L"embeddedIconOrder";
constexpr int kForgetAfterDays = 365;

// Days since 1601, in UTC: a day number that only goes up.
int CurrentDay() {
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    const ULONGLONG ticks =
        (static_cast<ULONGLONG>(now.dwHighDateTime) << 32) | now.dwLowDateTime;
    return static_cast<int>(ticks / (10000000ULL * 60 * 60 * 24));
}

// The tests set their own day.
int (*g_currentDay)() = CurrentDay;

// Key -> the day it was last seen. Guarded by g_mutex.
std::map<std::wstring, int, std::less<>> g_lastSeen;
bool g_lastSeenLoaded = false;
// The day every icon in the store was last stamped (NoteIconSeenLocked).
int g_lastSeenSweepDay = -1;

std::vector<std::wstring> StoredLines(PCWSTR name) {
    std::vector<std::wstring> lines;
    const std::wstring stored = ReadStoredString(name);
    std::wstring_view remaining(stored);
    while (!remaining.empty()) {
        const size_t lineEnd = remaining.find(L'\n');
        if (std::wstring_view line = remaining.substr(0, lineEnd); !line.empty()) {
            lines.emplace_back(line);
        }
        if (lineEnd == std::wstring_view::npos) {
            break;
        }
        remaining.remove_prefix(lineEnd + 1);
    }
    return lines;
}

void WriteStoredLines(PCWSTR name, const std::vector<std::wstring>& lines) {
    std::wstring joined;
    for (const auto& line : lines) {
        if (!joined.empty()) {
            joined += L'\n';
        }
        joined += line;
    }
    WriteStoredString(name, joined);
}

// A day number as stored: digits only, or -1.
int ParseDay(std::wstring_view digits) {
    if (digits.empty() || digits.size() > 9) {
        return -1;
    }
    int day = 0;
    for (wchar_t c : digits) {
        if (c < L'0' || c > L'9') {
            return -1;
        }
        day = day * 10 + (c - L'0');
    }
    return day;
}

void LoadLastSeenLocked() {
    if (g_lastSeenLoaded) {
        return;
    }
    g_lastSeenLoaded = true;
    for (const std::wstring& line : StoredLines(kLastSeenValue)) {
        const size_t split = line.rfind(L'=');
        if (split == std::wstring::npos || split == 0) {
            continue;
        }
        const int day = ParseDay(std::wstring_view(line).substr(split + 1));
        if (day >= 0) {
            g_lastSeen[line.substr(0, split)] = day;
        }
    }
}

void SaveLastSeenLocked() {
    std::vector<std::wstring> lines;
    for (const auto& [key, day] : g_lastSeen) {
        lines.push_back(key + L"=" + std::to_wstring(day));
    }
    WriteStoredLines(kLastSeenValue, lines);
}

// Marks `key` as seen on `today`. Returns whether that changed anything.
bool StampSeenLocked(std::wstring_view key, int today) {
    auto it = g_lastSeen.find(key);
    if (it == g_lastSeen.end()) {
        g_lastSeen.emplace(std::wstring(key), today);
        return true;
    }
    if (it->second == today) {
        return false;
    }
    it->second = today;
    return true;
}

// Once, at load, before any icon arrives. Caller holds g_mutex.
void ForgetIconsLongUnseenLocked() {
    LoadLastSeenLocked();
    LoadPlacements();
    const int today = g_currentDay();
    std::vector<std::wstring> order = StoredLines(kIconOrderValue);

    // Everything something is remembered under has a sighting from here on.
    bool changed = false;
    std::vector<std::wstring> keys;
    for (const auto& [key, destination] : g_placements) {
        keys.push_back(key);
    }
    {
        std::lock_guard<std::mutex> lock(g_hiddenMutex);
        LoadHidden();
        keys.insert(keys.end(), g_hidden.begin(), g_hidden.end());
    }
    keys.insert(keys.end(), order.begin(), order.end());
    for (const auto& key : keys) {
        if (g_lastSeen.find(key) == g_lastSeen.end()) {
            g_lastSeen.emplace(key, today);
            changed = true;
        }
    }

    std::set<std::wstring, std::less<>> forgotten;
    for (auto it = g_lastSeen.begin(); it != g_lastSeen.end();) {
        if (today - it->second > kForgetAfterDays) {
            forgotten.insert(it->first);
            it = g_lastSeen.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }

    if (!forgotten.empty()) {
        size_t moves = 0;
        for (auto it = g_placements.begin(); it != g_placements.end();) {
            if (forgotten.count(it->first)) {
                it = g_placements.erase(it);
                moves++;
            } else {
                ++it;
            }
        }
        if (moves) {
            SavePlacements();
        }
        size_t hidden = 0;
        {
            std::lock_guard<std::mutex> lock(g_hiddenMutex);
            for (auto it = g_hidden.begin(); it != g_hidden.end();) {
                if (forgotten.count(*it)) {
                    it = g_hidden.erase(it);
                    hidden++;
                } else {
                    ++it;
                }
            }
            if (hidden) {
                SaveHidden();
            }
        }
        const size_t orderBefore = order.size();
        std::erase_if(order, [&](const std::wstring& key) { return forgotten.count(key) > 0; });
        if (order.size() != orderBefore) {
            WriteStoredLines(kIconOrderValue, order);
        }
        Wh_Log(L"forgot %zu icon(s) not seen for a year: %zu move(s), %zu kept in the "
               L"overflow, %zu place(s) in the order",
               forgotten.size(), moves, hidden, orderBefore - order.size());
    }
    if (changed) {
        SaveLastSeenLocked();
    }
}

// ============================================================================
// Shared state
//
// Two threads touch it: Explorer's taskbar thread (the Shell_TrayWnd subclass)
// and the mod's own tray-window thread. Neither calls out to the shell while
// holding the lock.
// ============================================================================

std::mutex g_mutex;
Settings g_settings;
std::vector<MirroredIcon> g_icons;         // shown in one of Split Tray's trays
std::vector<MirroredIcon> g_primaryOnly;   // tracked for routing stickiness only

// Split Tray's trays as they stand (PlanTrays), in number order: g_trays[i] is
// tray i + 2.
std::vector<TrayTarget> g_trays;
MonitorInfoEntry g_primaryMonitor;

// The layout of every tray drawn as a floating panel right now, by number. A
// display tray that is embedded in its taskbar is not in here.
std::map<int, TrayLayout> g_floatingLayouts;

// The displays whose tray is embedded in their taskbar. Written on the
// taskbar's thread as panels attach and detach, read by the tray thread to know
// which trays still need a floating panel.
std::set<HMONITOR> g_embeddedMonitors;

// Each floating tray's window, by number. Created and destroyed on the tray
// thread; kept here so other threads (and the tests) can find them.
std::map<int, HWND> g_floatingWnds;

std::atomic<uint64_t> g_nextSerial{1};

// --- Trays, from the shared state. Each of these needs g_mutex held. --------

TrayTarget* FindTrayLocked(int number) {
    if (number < 2) {
        return nullptr;
    }
    const size_t index = static_cast<size_t>(number - 2);
    return index < g_trays.size() ? &g_trays[index] : nullptr;
}

// The primary tray always exists; one of Split Tray's exists while its display
// does.
bool TrayAvailableLocked(int number) {
    if (number <= 1) {
        return true;
    }
    const TrayTarget* tray = FindTrayLocked(number);
    return tray && tray->available;
}

bool TrayEmbeddedLocked(const TrayTarget& tray) {
    return tray.forDisplay && g_embeddedMonitors.count(tray.monitor.handle) != 0;
}

// Indices into g_icons of the icons tray `number` shows, in store order.
std::vector<size_t> IconsInTrayLocked(int number) {
    std::vector<size_t> indices;
    for (size_t i = 0; i < g_icons.size(); i++) {
        if (g_icons[i].shownTray == number) {
            indices.push_back(i);
        }
    }
    return indices;
}

// ---------------------------------------------------------------------------
// The program behind an icon, when its message does not say
//
// The rules match the program's path, and only an add carries it (DECISIONS
// 4). An application that only ever updates its icon - SystemInformer redraws
// four graphs a second and adds them again only when an update fails - never
// sends one while Explorer already has its icons. That is every load of the
// mod into a running Explorer: installing it, updating it, switching it off and
// on. Its icons then stayed in the main tray until Explorer restarted, whatever
// the rules said. So when a message carries no path and the icon's tray still
// depends on one, Windows is asked which program owns its window (DECISIONS
// 63, refining 4), with the least a process can be opened for: the right Task
// Manager uses to show the paths of elevated programs.
// ---------------------------------------------------------------------------

std::wstring ProcessImagePathOfWindow(HWND window) {
    DWORD pid = 0;
    if (!window || !GetWindowThreadProcessId(window, &pid) || !pid) {
        return std::wstring();
    }
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return std::wstring();
    }
    WCHAR path[1024] = {};
    DWORD size = ARRAYSIZE(path);
    std::wstring result;
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        result.assign(path, size);
    }
    CloseHandle(process);
    return result;
}

// Replaceable, so the tests decide what a window's program is.
std::wstring (*g_lookUpProcessPath)(HWND) = ProcessImagePathOfWindow;

// Fills in the path a message leaves out, when the icon's tray depends on it:
// an icon the mod has not seen, or one it has not been able to place yet.
// Caller holds g_mutex.
void FillMissingPathLocked(TrayNotification* n) {
    if (!n->exePath.empty() || (n->message != NIM_ADD && n->message != NIM_MODIFY)) {
        return;
    }
    for (auto* list : {&g_icons, &g_primaryOnly}) {
        for (const auto& icon : *list) {
            if (SameIcon(icon, *n) && icon.destinationDecided) {
                return;
            }
        }
    }
    n->exePath = g_lookUpProcessPath(n->ownerWnd);
}

// The icon with this serial in g_icons, or -1.
int IndexOfSerialLocked(uint64_t serial) {
    for (size_t i = 0; i < g_icons.size(); i++) {
        if (g_icons[i].serial == serial) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// "the primary tray", "tray 2 (display to the left)".
std::wstring TrayLabelLocked(int number) {
    if (number <= 1) {
        return L"the primary tray";
    }
    std::wstring label = L"tray " + std::to_wstring(number);
    if (const TrayTarget* tray = FindTrayLocked(number)) {
        label += L" (" + DescribeTrayPlace(*tray, g_primaryMonitor) + L")";
    }
    return label;
}

std::atomic<bool> g_unloading{false};
std::atomic<HWND> g_trayWnd{nullptr};
std::atomic<HWND> g_shellTrayWnd{nullptr};
HANDLE g_trayThread = nullptr;
DWORD g_trayThreadId = 0;
// Whether it was asked to end and did not (StopTrayThread).
bool g_trayThreadStuck = false;
// How far it got: set once its window exists, or once it has given up.
enum class TrayThreadState { Starting, Running, GaveUp };
std::atomic<TrayThreadState> g_trayThreadState{TrayThreadState::Starting};
// How long Wh_ModInit waits for it to be running. A thread slower than this is
// left to carry on, and an unload can then come before its window exists
// (StopTrayThread); the integration test sets it to 0 to make that happen.
DWORD g_trayThreadStartWaitMs = 5000;
// The integration test holds the tray thread here, before it registers
// anything, to load the mod while the thread is still starting.
HANDLE g_trayThreadHold = nullptr;

// Set on the taskbar's thread once every icon is back with Explorer as the mod
// unloads. Until then the subclass keeps track of icons, unloading or not;
// from then it passes everything on untouched until it is removed
// (DECISIONS 68).
std::atomic<bool> g_handedBack{false};
// Whether unloading ended before that happened (HandBackToShell).
bool g_handBackStuck = false;
// How long unloading waits for the taskbar's thread at each step that needs
// it (DECISIONS 73); the tests shorten it.
DWORD g_taskbarWaitMs = 5000;
// How many calls of the mod's subclass are under way on the taskbar's thread.
// Unloading does not finish while there are any (DECISIONS 73).
std::atomic<int> g_subclassDepth{0};
// Whether the mod's panels could not be taken out of the taskbars in time.
bool g_panelsStuck = false;

// Messages posted to the mod's own tray window.
constexpr UINT WM_ST_REFRESH = WM_APP + 0x101;   // icons changed, re-layout
constexpr UINT WM_ST_SETTINGS = WM_APP + 0x102;  // settings changed
constexpr UINT WM_ST_SHUTDOWN = WM_APP + 0x103;
constexpr UINT WM_ST_ARRANGE = WM_APP + 0x104;   // open the arrange window
// wParam: the serial of an icon its application gave the focus back to
// (NIM_SETFOCUS), for the floating tray that shows it (DECISIONS 85).
constexpr UINT WM_ST_FOCUS_ICON = WM_APP + 0x105;
// To a floating tray: the cell with serial wParam chosen by a screen reader.
constexpr UINT WM_ST_CELL_ACTIVATE = WM_APP + 0x110;
// To a floating tray: the keyboard to the cell with serial wParam, as a screen
// reader asked. Posted: a window is not activated from inside the call that
// asks, which UI Automation makes from another thread.
constexpr UINT WM_ST_CELL_FOCUS = WM_APP + 0x111;

// Posted, or sent while unloading, to Shell_TrayWnd to settle the icons whose
// place in Explorer's tray is not where it should be, on the taskbar's thread,
// straight to Explorer's own window procedure without re-entering the mod's
// handler. It carries nothing: what to do is read from the icon store when it
// arrives (DECISIONS 58, 66). Being registered by name, it can be posted by any
// process on the desktop.
UINT GetReplayMessage() {
    static UINT msg = RegisterWindowMessageW(L"SplitTray_ReplayToShell_" WH_MOD_ID);
    return msg;
}

// Posted to Shell_TrayWnd to run an attach attempt on the taskbar's UI thread.
//
// Both taskbars are on the same thread - verified on this machine: Shell_TrayWnd
// and Shell_SecondaryTrayWnd both report thread 93292 - so the window the mod
// already subclasses is a usable way onto the thread that owns the secondary
// taskbar's XAML. XAML objects are thread-affine, so the timer cannot touch them
// itself.
UINT GetAttachMessage() {
    static UINT msg = RegisterWindowMessageW(L"SplitTray_AttachXaml_" WH_MOD_ID);
    return msg;
}

// Same idea, for redrawing the embedded trays from a thread that must not touch
// XAML - the arrange window's, for one.
UINT GetXamlRefreshMessage() {
    static UINT msg = RegisterWindowMessageW(L"SplitTray_RefreshXaml_" WH_MOD_ID);
    return msg;
}

// Sent while unloading, so the panels are taken out of the taskbars on the
// thread that owns them. Removing them from Windhawk's unload thread threw the
// wrong-thread error inside a catch-all and left them in place until Explorer
// restarted.
UINT GetXamlRemoveMessage() {
    static UINT msg = RegisterWindowMessageW(L"SplitTray_RemoveXaml_" WH_MOD_ID);
    return msg;
}

// ============================================================================
// Section 6 - The floating trays, and the mod's own window
//
// The tray thread owns a hidden controller window - the target of every
// WM_ST_* message, of the retry timer, and of Explorer's TaskbarCreated
// broadcast - and a floating panel for each tray that is not embedded in a
// taskbar: every extra tray, and a display's tray while embedding is off or
// has not attached.
// ============================================================================

constexpr PCWSTR kControllerClassName = L"SplitTrayController";
constexpr PCWSTR kFloatingClassName = L"SplitTrayFloatingTray";
constexpr PCWSTR kArrangeClassName = L"SplitTrayArrangeWindow";

// ---------------------------------------------------------------------------
// Which Explorer shows the taskbar
//
// Explorer runs folder windows in processes of their own - every one, or only
// some, opened through COM as `explorer.exe /factory,{...} -Embedding` - and
// Windhawk loads the mod into each. Only the process that shows the taskbar is
// the mod's. Another took the taskbar it found for its own: it tried and failed
// to attach to that process's window every two seconds, and drew its trays - a
// second tray 2, empty, floating beside the real one (DECISIONS 92).
// ---------------------------------------------------------------------------

enum class TaskbarShownBy { Nobody, ThisProcess, AnotherProcess };

// Pure: from the process of every Shell_TrayWnd on the desktop.
TaskbarShownBy WhoShowsTheTaskbar(const std::vector<DWORD>& owners, DWORD self) {
    if (owners.empty()) {
        return TaskbarShownBy::Nobody;
    }
    return std::find(owners.begin(), owners.end(), self) != owners.end()
               ? TaskbarShownBy::ThisProcess
               : TaskbarShownBy::AnotherProcess;
}

struct ShellTrayWindows {
    std::vector<DWORD> owners;
    HWND own = nullptr;  // this process's, if it has one
};

ShellTrayWindows FindShellTrayWindows() {
    ShellTrayWindows found;
    const DWORD self = GetCurrentProcessId();
    HWND wnd = nullptr;
    // Bounded: windows reordered while they are walked could in principle
    // lead the walk round again.
    for (int i = 0; i < 32; i++) {
        wnd = FindWindowExW(nullptr, wnd, L"Shell_TrayWnd", nullptr);
        if (!wnd) {
            break;
        }
        DWORD owner = 0;
        GetWindowThreadProcessId(wnd, &owner);
        found.owners.push_back(owner);
        if (owner == self && !found.own) {
            found.own = wnd;
        }
    }
    return found;
}

TaskbarShownBy TaskbarShownNow() {
    return WhoShowsTheTaskbar(FindShellTrayWindows().owners, GetCurrentProcessId());
}

// Whether the last pass found the taskbar another process's. Tray thread only.
bool g_taskbarElsewhere = false;

// ---------------------------------------------------------------------------
// The mod's window classes
//
// A window class outlives the module that registered it: Windows does not
// unregister a DLL's classes when the DLL unloads, and a class left behind
// points at a window procedure that is no longer there. They were registered
// against Explorer's own module, which the mod never unloads, and the arrange
// window's was never unregistered at all - so after the mod was reloaded, its
// next arrange window would have been created with the old, unloaded window
// procedure (DECISIONS 59). They now belong to the mod's own module and are
// unregistered, all of them, when the tray thread ends.
// ---------------------------------------------------------------------------

HINSTANCE ModuleInstance() {
    static const HINSTANCE instance = [] {
        HMODULE module = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&ModuleInstance), &module);
        return module;
    }();
    return instance;
}

bool RegisterModClass(WNDCLASSEXW* wc) {
    wc->hInstance = ModuleInstance();
    // One an earlier build registered against Explorer's module, which nothing
    // ever took down. Harmless when there is none.
    if (ModuleInstance() != GetModuleHandleW(nullptr)) {
        UnregisterClassW(wc->lpszClassName, GetModuleHandleW(nullptr));
    }
    if (RegisterClassExW(wc)) {
        return true;
    }
    // Left by a load of this module that could not clean up. Reused, it would
    // run that load's window procedure; replaced, it runs this one's.
    if (GetLastError() == ERROR_CLASS_ALREADY_EXISTS &&
        UnregisterClassW(wc->lpszClassName, wc->hInstance) && RegisterClassExW(wc)) {
        return true;
    }
    Wh_Log(L"could not register window class %s: %lu", wc->lpszClassName,
           GetLastError());
    return false;
}

void UnregisterModClass(PCWSTR name) {
    if (!UnregisterClassW(name, ModuleInstance()) &&
        GetLastError() != ERROR_CLASS_DOES_NOT_EXIST) {
        Wh_Log(L"could not unregister window class %s: %lu", name, GetLastError());
    }
}

class FloatingTrayUia;

// Tray-thread state for one floating panel.
struct FloatingTray {
    HWND wnd = nullptr;
    HWND tooltip = nullptr;
    int hotIndex = -1;  // the cell under the pointer, for the highlight
    // The icon whose own popup was opened (NIN_POPUPOPEN), to be closed when
    // the pointer leaves it (DECISIONS 79).
    uint64_t popupSerial = 0;
    // The keyboard's cell - its icon's serial, or 0 for an empty tray's
    // handle - and whether the tray has the focus (DECISIONS 85).
    uint64_t focusSerial = 0;
    bool focused = false;
    // What screen readers read, once one has asked (DECISIONS 85). Held.
    FloatingTrayUia* uia = nullptr;
};

std::map<int, FloatingTray> g_floatingTrays;  // tray thread only, by number

// The last window of anyone but the tray thread to have had the foreground:
// where Escape in a floating tray goes back to (DECISIONS 85). Noted as the
// foreground changes, by a hook the tray thread holds while it runs. Noting it
// when the tray took the focus was too late: UI Automation brings the tray's
// window to the front itself before the provider is asked to focus a cell.
HWND g_lastForeground = nullptr;  // tray thread only

void NoteForeground(HWND wnd) {
    if (wnd && GetWindowThreadProcessId(wnd, nullptr) != GetCurrentThreadId()) {
        g_lastForeground = wnd;
    }
}

void CALLBACK ForegroundChanged(HWINEVENTHOOK, DWORD, HWND wnd, LONG object, LONG, DWORD,
                                DWORD) {
    if (object == OBJID_WINDOW) {
        NoteForeground(wnd);
    }
}

// Where the displays come from: Windows, or in the regression tests a fixed set,
// so that what they check does not depend on the machine they run on.
std::vector<MonitorInfoEntry> (*g_enumerateMonitors)() = EnumerateMonitors;

// Which display has which tray number, remembered across restarts with the
// mod's other state (AssignDisplaySlots, DECISIONS 86). Guarded by g_mutex.
constexpr PCWSTR kDisplaySlotsValue = L"displaySlots";
std::vector<std::wstring> g_displaySlots;
bool g_displaySlotsLoaded = false;

void LoadDisplaySlotsLocked() {
    if (g_displaySlotsLoaded) {
        return;
    }
    g_displaySlotsLoaded = true;
    const std::wstring stored = ReadStoredString(kDisplaySlotsValue);
    std::wstring_view remaining(stored);
    while (!remaining.empty()) {
        const size_t lineEnd = remaining.find(L'\n');
        g_displaySlots.emplace_back(remaining.substr(0, lineEnd));
        if (lineEnd == std::wstring_view::npos) {
            break;
        }
        remaining.remove_prefix(lineEnd + 1);
    }
}

void SaveDisplaySlotsLocked() {
    std::wstring joined;
    for (const std::wstring& slot : g_displaySlots) {
        if (!joined.empty()) {
            joined += L'\n';
        }
        joined += slot;
    }
    WriteStoredString(kDisplaySlotsValue, joined);
}

// Recomputes which trays exist and how the floating ones are laid out. Caller
// must hold g_mutex.
void RecomputeGeometryLocked() {
    const auto monitors = g_enumerateMonitors();
    LoadDisplaySlotsLocked();
    if (AssignDisplaySlots(monitors, &g_displaySlots)) {
        SaveDisplaySlotsLocked();
        Wh_Log(L"a display seen for the first time: %zu display tray number(s) remembered",
               g_displaySlots.size());
    }
    g_trays = PlanTrays(monitors, g_settings, g_displaySlots);
    g_primaryMonitor = MonitorInfoEntry{};
    for (const auto& monitor : monitors) {
        if (monitor.primary) {
            g_primaryMonitor = monitor;
        }
    }
    g_floatingLayouts.clear();
    for (const auto& tray : g_trays) {
        if (!tray.available || TrayEmbeddedLocked(tray)) {
            continue;
        }
        // An empty tray still gets one cell: a handle to reach the mod's menu
        // by, as the embedded tray has. Otherwise an extra tray nothing has been
        // routed to yet would be invisible and unreachable.
        const int count = static_cast<int>(IconsInTrayLocked(tray.number).size());
        g_floatingLayouts[tray.number] =
            ComputeLayout(tray.monitor.workArea, std::max(count, 1), g_settings,
                          tray.monitor.dpi, tray.corner);
    }
}

// The name an icon goes by in a list or a menu: the first line of its tooltip
// - SystemInformer's run to several lines of live data - or its executable.
std::wstring IconLabel(const MirroredIcon& icon) {
    std::wstring label = icon.tip;
    const size_t newline = label.find_first_of(L"\r\n");
    if (newline != std::wstring::npos) {
        label.erase(newline);
    }
    while (!label.empty() && label.back() == L' ') {
        label.pop_back();
    }
    if (label.empty()) {
        label = FileNameOf(icon.exePath);
    }
    if (label.empty()) {
        label = L"(unnamed icon)";
    }
    return label;
}

// Every icon the mod knows about, with the tray it is in now. An icon whose own
// tray is missing is in the primary tray, which is where it actually shows.
struct KnownIcon {
    std::wstring key;
    std::wstring label;
    int tray = 1;
};

std::vector<KnownIcon> KnownIcons() {
    std::vector<KnownIcon> known;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (const auto& icon : g_icons) {
            known.push_back({StableKeyOf(icon), IconLabel(icon), icon.shownTray});
        }
        for (const auto& icon : g_primaryOnly) {
            known.push_back({StableKeyOf(icon), IconLabel(icon), 1});
        }
    }
    std::sort(known.begin(), known.end(), [](KnownIcon const& a, KnownIcon const& b) {
        return _wcsicmp(a.label.c_str(), b.label.c_str()) < 0;
    });
    return known;
}

// Every tray an icon can be sent to right now, primary first.
std::vector<int> AvailableTrayNumbers() {
    std::vector<int> numbers = {1};
    std::lock_guard<std::mutex> lock(g_mutex);
    for (const auto& tray : g_trays) {
        if (tray.available) {
            numbers.push_back(tray.number);
        }
    }
    return numbers;
}

std::wstring TrayLabel(int number) {
    std::lock_guard<std::mutex> lock(g_mutex);
    return TrayLabelLocked(number);
}

bool ShiftHeld() {
    return (GetKeyState(VK_SHIFT) & 0x8000) != 0;
}

int TrayNumberOfWindow(HWND hWnd) {
    return static_cast<int>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
}

// Whether an icon has the tray's own tooltip. From version 4 Explorer shows it
// only when the icon asks with NIF_SHOWTIP; otherwise the application draws a
// popup of its own, on NIN_POPUPOPEN (DECISIONS 79).
bool UsesStandardTooltip(UINT version, DWORD recordFlags) {
    return version < NOTIFYICON_VERSION_4 || (recordFlags & NIF_SHOWTIP) != 0;
}

// The NIF_* flags an icon's folded record holds (FoldTrayRecord).
DWORD RecordFlagsOf(const MirroredIcon& icon) {
    return icon.payload.size() >= wire::kFlags + sizeof(DWORD)
               ? ReadDword(icon.payload.data(), wire::kFlags)
               : 0;
}

// What one floating tray draws: its layout, and its icons in store order.
struct FloatingView {
    TrayLayout layout;
    std::vector<uint64_t> serials;
    std::vector<OwnedIcon> icons;  // empty unless asked for
    // Each icon's tooltip; empty for one that draws a popup of its own
    // (UsesStandardTooltip, DECISIONS 79).
    std::vector<std::wstring> tips;
    int Cells() const { return std::max(1, static_cast<int>(serials.size())); }
};

// `withIcons` copies each icon's picture for drawing; hit tests and tooltips
// do without.
FloatingView FloatingViewOf(int number, bool withIcons = true) {
    FloatingView view;
    std::lock_guard<std::mutex> lock(g_mutex);
    auto layout = g_floatingLayouts.find(number);
    if (layout != g_floatingLayouts.end()) {
        view.layout = layout->second;
    }
    for (size_t i : IconsInTrayLocked(number)) {
        view.serials.push_back(g_icons[i].serial);
        if (withIcons) {
            view.icons.push_back(OwnedIcon::CopyOf(g_icons[i].icon));
        }
        const MirroredIcon& icon = g_icons[i];
        view.tips.push_back(UsesStandardTooltip(icon.version, RecordFlagsOf(icon))
                                ? icon.tip
                                : std::wstring());
    }
    return view;
}

// Where the keyboard goes from cell `current` of `count`, laid out in rows of
// `columns`, for `key`: arrows to the next cell or row, Home and End to the
// ends (DECISIONS 85). From no cell any of them goes to the first; a key that
// does not move it leaves it where it is.
int NextFocusIndex(int current, UINT key, int count, int columns) {
    if (count <= 0) {
        return -1;
    }
    columns = std::max(columns, 1);
    const bool moves = key == VK_LEFT || key == VK_RIGHT || key == VK_UP ||
                       key == VK_DOWN || key == VK_HOME || key == VK_END;
    if (current < 0 || current >= count) {
        return moves ? 0 : current;
    }
    switch (key) {
        case VK_LEFT:
            return std::max(current - 1, 0);
        case VK_RIGHT:
            return std::min(current + 1, count - 1);
        case VK_UP:
            return current >= columns ? current - columns : current;
        case VK_DOWN:
            return current + columns < count ? current + columns : current;
        case VK_HOME:
            return 0;
        case VK_END:
            return count - 1;
    }
    return current;
}

// The cell of the icon with `serial` in `view`: its index, the handle's (0)
// for serial 0 in an empty tray, or -1 when it is not there.
int CellIndexOfSerial(const FloatingView& view, uint64_t serial) {
    if (view.serials.empty()) {
        return serial == 0 ? 0 : -1;
    }
    const auto found = std::find(view.serials.begin(), view.serials.end(), serial);
    return found == view.serials.end() ? -1
                                        : static_cast<int>(found - view.serials.begin());
}

// The serial of the cell at `index`: its icon's, or 0 for an empty tray's
// handle.
uint64_t SerialOfCellIndex(const FloatingView& view, int index) {
    return index >= 0 && index < static_cast<int>(view.serials.size())
               ? view.serials[static_cast<size_t>(index)]
               : 0;
}

// One icon of a tray embedded in a taskbar, as the taskbar's thread draws it:
// from its own copy of the picture, like everything else drawn after the
// store's lock is released. The tray thread's watchdog destroys the store's
// copy when the icon's application goes (DECISIONS 60).
struct CellSnapshot {
    OwnedIcon icon;
    // The cell's tooltip: empty when tooltips are switched off (DECISIONS 77).
    std::wstring tip;
    std::wstring key;
    uint64_t serial = 0;
    // Whether the store has a picture for the icon, which `icon` is a copy of
    // unless copying it failed.
    bool hasPicture = false;
    // What screen readers call the icon (IconLabel), whether tooltips are
    // shown or not (DECISIONS 81).
    std::wstring name;
    // Which of the store's pictures of the icon this is, and whether it is the
    // one the cell already shows, when nothing is copied for it (DECISIONS 82).
    uint64_t pictureRevision = 0;
    bool pictureUnchanged = false;
};

// What a cell updated in place does with its picture (DECISIONS 76). It was
// given a new one only when there was one, so a picture its application took
// away stayed on show in the taskbar until something else rebuilt the tray.
// One that could not be copied for this refresh is still in the store
// (DECISIONS 72): the cell keeps what it shows, and the next refresh copies it
// again.
enum class CellPicture { Keep, Replace, Clear };

CellPicture CellPictureOf(const CellSnapshot& cell) {
    if (cell.pictureUnchanged) {
        return CellPicture::Keep;
    }
    if (!cell.hasPicture) {
        return CellPicture::Clear;
    }
    return cell.icon.get() ? CellPicture::Replace : CellPicture::Keep;
}

// A copy of the picture of the icon with `serial`, for a cell that has to draw
// it although it has not changed: a cell made afresh, or one moved into the
// overflow popup.
OwnedIcon PictureOfSerial(uint64_t serial) {
    std::lock_guard<std::mutex> lock(g_mutex);
    for (const auto& icon : g_icons) {
        if (icon.serial == serial) {
            return OwnedIcon::CopyOf(icon.icon);
        }
    }
    return OwnedIcon();
}

// Tray `number`'s icons, in the store's order. `drawn`, when given, is the
// picture revision each cell shows, by serial: a picture that has not changed
// since is not copied again (DECISIONS 82). Every refresh copied every icon's
// picture and made a bitmap of it again, on the taskbar's thread, several
// times a second while SystemInformer draws its graphs.
std::vector<CellSnapshot> CellSnapshotsOf(int number,
                                          const std::map<uint64_t, uint64_t>* drawn = nullptr) {
    std::vector<CellSnapshot> cells;
    std::lock_guard<std::mutex> lock(g_mutex);
    for (size_t i : IconsInTrayLocked(number)) {
        const MirroredIcon& icon = g_icons[i];
        CellSnapshot cell;
        if (drawn) {
            const auto shown = drawn->find(icon.serial);
            cell.pictureUnchanged =
                shown != drawn->end() && shown->second == icon.pictureRevision;
        }
        if (!cell.pictureUnchanged) {
            cell.icon = OwnedIcon::CopyOf(icon.icon);
        }
        const bool tooltip =
            g_settings.showTooltips && UsesStandardTooltip(icon.version, RecordFlagsOf(icon));
        cell.tip = tooltip ? icon.tip : std::wstring();
        cell.key = StableKeyOf(icon);
        cell.serial = icon.serial;
        cell.hasPicture = icon.icon != nullptr;
        cell.name = IconLabel(icon);
        cell.pictureRevision = icon.pictureRevision;
        cells.push_back(std::move(cell));
    }
    return cells;
}

FloatingTray* FloatingTrayOfWindow(HWND hWnd) {
    auto found = g_floatingTrays.find(TrayNumberOfWindow(hWnd));
    return (found != g_floatingTrays.end() && found->second.wnd == hWnd)
               ? &found->second
               : nullptr;
}

void UpdateTooltipText(HWND hWnd) {
    FloatingTray* tray = FloatingTrayOfWindow(hWnd);
    if (!tray || !tray->tooltip) {
        return;
    }
    const int number = TrayNumberOfWindow(hWnd);
    const FloatingView view = FloatingViewOf(number, /*withIcons=*/false);
    std::wstring text;
    if (view.serials.empty()) {
        text = L"Split Tray - " + TrayLabel(number) + L" is empty";
    } else if (tray->hotIndex >= 0 &&
               tray->hotIndex < static_cast<int>(view.tips.size())) {
        text = view.tips[static_cast<size_t>(tray->hotIndex)];
    }
    TTTOOLINFOW ti = {sizeof(ti)};
    ti.hwnd = hWnd;
    ti.uId = 0;
    ti.lpszText = text.empty() ? const_cast<PWSTR>(L" ") : text.data();
    SendMessageW(tray->tooltip, TTM_UPDATETIPTEXTW, 0, reinterpret_cast<LPARAM>(&ti));
}

void PaintTray(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    if (!hdc) {
        return;
    }

    RECT client;
    GetClientRect(hWnd, &client);

    // Double buffer: the tray repaints on every icon change and flicker on top of
    // someone's wallpaper is very visible.
    HDC memDc = CreateCompatibleDC(hdc);
    HBITMAP bmp =
        CreateCompatibleBitmap(hdc, client.right - client.left, client.bottom - client.top);
    HGDIOBJ oldBmp = SelectObject(memDc, bmp);

    const FloatingView view = FloatingViewOf(TrayNumberOfWindow(hWnd));
    const FloatingTray* tray = FloatingTrayOfWindow(hWnd);
    const int hotIndex = tray ? tray->hotIndex : -1;
    COLORREF background;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        background = g_settings.background;
    }
    const TrayLayout& layout = view.layout;

    HBRUSH bgBrush = CreateSolidBrush(background);
    FillRect(memDc, &client, bgBrush);
    DeleteObject(bgBrush);

    if (layout.cell > 0 && layout.columns > 0) {
        const int inset = (layout.cell - layout.icon) / 2;
        for (int i = 0; i < view.Cells(); i++) {
            const int column = i % layout.columns;
            const int row = i / layout.columns;
            RECT cell = {column * layout.cell, row * layout.cell,
                         (column + 1) * layout.cell, (row + 1) * layout.cell};
            if (i == hotIndex) {
                // A subtle highlight so it is obvious the icons are live.
                HBRUSH hot = CreateSolidBrush(
                    RGB(std::min(255, GetRValue(background) + 28),
                        std::min(255, GetGValue(background) + 28),
                        std::min(255, GetBValue(background) + 28)));
                FillRect(memDc, &cell, hot);
                DeleteObject(hot);
            }
            if (i < static_cast<int>(view.icons.size())) {
                if (view.icons[static_cast<size_t>(i)].get()) {
                    DrawIconEx(memDc, cell.left + inset, cell.top + inset,
                               view.icons[static_cast<size_t>(i)].get(), layout.icon,
                               layout.icon, 0, nullptr, DI_NORMAL);
                }
                continue;
            }
            // The empty tray's handle: three dots, as a "more" affordance.
            HBRUSH dots = CreateSolidBrush(RGB(0x9A, 0x9A, 0x9A));
            HGDIOBJ oldBrush = SelectObject(memDc, dots);
            HGDIOBJ oldPen = SelectObject(memDc, GetStockObject(NULL_PEN));
            const int dot = std::max(2, layout.icon / 6);
            const int midY = (cell.top + cell.bottom) / 2;
            const int midX = (cell.left + cell.right) / 2;
            for (int d = -1; d <= 1; d++) {
                const int x = midX + d * dot * 2;
                Ellipse(memDc, x - dot / 2, midY - dot / 2, x + dot / 2 + 1,
                        midY + dot / 2 + 1);
            }
            SelectObject(memDc, oldPen);
            SelectObject(memDc, oldBrush);
            DeleteObject(dots);
        }
        // The keyboard's cell, while the tray has the focus (DECISIONS 85).
        const int focusIndex = tray && tray->focused
                                   ? CellIndexOfSerial(view, tray->focusSerial)
                                   : -1;
        if (focusIndex >= 0) {
            RECT cell = {(focusIndex % layout.columns) * layout.cell,
                         (focusIndex / layout.columns) * layout.cell, 0, 0};
            cell.right = cell.left + layout.cell;
            cell.bottom = cell.top + layout.cell;
            InflateRect(&cell, -1, -1);
            DrawFocusRect(memDc, &cell);
        }
    }

    BitBlt(hdc, 0, 0, client.right - client.left, client.bottom - client.top, memDc,
           0, 0, SRCCOPY);

    SelectObject(memDc, oldBmp);
    DeleteObject(bmp);
    DeleteDC(memDc);
    EndPaint(hWnd, &ps);
}

void ForwardClick(uint64_t serial, UINT mouseMessage, POINT screenPoint);
void ForwardHover(uint64_t serial, POINT screenPoint);
bool ForwardPopup(uint64_t serial, bool open, POINT screenPoint);
void ForwardKey(uint64_t serial, bool contextMenu, POINT anchor);
void ApplySettingsToTrackedIcons();
void ReplayRoutingChanges();
void WakeReplayDelivery();
void MoveIconToTray(std::wstring_view key, Destination destination);
void EnsureShellTrayWindowSubclassed();
UINT TaskbarCreatedMessage();
void NoteShellAnnouncedTaskbar();
void ShowArrangeWindow();
void CloseArrangeWindow();
void RefreshArrangeWindow(bool repopulate);
void NotifyTrayWindow(UINT message);
void RequestRedrawOfAllTrays();

// The mod's menu on a floating tray, as ShowFloatingTrayMenu puts it up: built
// and acted on apart from showing it, so what it offers and what each choice
// does are tested without a modal menu.
struct FloatingTrayMenu {
    static constexpr UINT kArrange = 1;
    static constexpr UINT kResetMoved = 2;
    static constexpr UINT kMoveThis = 100;   // + target tray number
    static constexpr UINT kMoveHere = 1000;  // + index into `others`

    HMENU menu = nullptr;           // with its submenu; the caller destroys it
    std::wstring key;               // the icon it was opened on, if any
    std::vector<KnownIcon> others;  // what "Move an icon to this tray" offers
};

// The menu for tray `number`, opened on the icon with `serial`, or on the tray
// itself for 0. The same choices as the embedded tray's menu.
FloatingTrayMenu BuildFloatingTrayMenu(int number, uint64_t serial) {
    constexpr UINT kArrange = FloatingTrayMenu::kArrange;
    constexpr UINT kResetMoved = FloatingTrayMenu::kResetMoved;
    constexpr UINT kMoveThis = FloatingTrayMenu::kMoveThis;
    constexpr UINT kMoveHere = FloatingTrayMenu::kMoveHere;

    FloatingTrayMenu built;
    std::wstring& key = built.key;
    if (serial) {
        std::lock_guard<std::mutex> lock(g_mutex);
        const int index = IndexOfSerialLocked(serial);
        if (index >= 0) {
            key = StableKeyOf(g_icons[static_cast<size_t>(index)]);
        }
    }

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return built;
    }
    built.menu = menu;
    if (!key.empty()) {
        for (int target : AvailableTrayNumbers()) {
            if (target == number) {
                continue;
            }
            const std::wstring text = L"Move to " + TrayLabel(target);
            AppendMenuW(menu, MF_STRING, kMoveThis + static_cast<UINT>(target),
                        text.c_str());
        }
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    }

    std::vector<KnownIcon>& others = built.others;
    for (auto& icon : KnownIcons()) {
        if (icon.tray != number) {
            others.push_back(std::move(icon));
        }
    }
    HMENU here = CreatePopupMenu();
    for (size_t i = 0; i < others.size() && i < 500; i++) {
        AppendMenuW(here, MF_STRING, kMoveHere + static_cast<UINT>(i),
                    others[i].label.c_str());
    }
    if (others.empty()) {
        AppendMenuW(here, MF_STRING | MF_GRAYED, 0, L"Every icon is already here");
    }
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(here),
                L"Move an icon to this tray");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kArrange, L"Arrange icons\x2026");
    AppendMenuW(menu, MF_STRING, kResetMoved, L"Reset moved icons");
    return built;
}

// What choosing `command` from `menu`, tray `number`'s, does; 0 is nothing.
void RunFloatingTrayMenuCommand(const FloatingTrayMenu& menu, int number, UINT command) {
    if (command == FloatingTrayMenu::kArrange) {
        ShowArrangeWindow();
    } else if (command == FloatingTrayMenu::kResetMoved) {
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            ForgetAllPlacements();
        }
        ApplySettingsToTrackedIcons();
        RequestRedrawOfAllTrays();
    } else if (command >= FloatingTrayMenu::kMoveHere) {
        const size_t index = command - FloatingTrayMenu::kMoveHere;
        if (index < menu.others.size()) {
            MoveIconToTray(menu.others[index].key, Destination::Tray(number));
        }
    } else if (command >= FloatingTrayMenu::kMoveThis && !menu.key.empty()) {
        MoveIconToTray(menu.key, Destination::Tray(
                                     static_cast<int>(command - FloatingTrayMenu::kMoveThis)));
    }
}

// The mod's menu on a floating tray: Shift+right-click on an icon, or any click
// on an empty tray's handle, or its handle chosen from the keyboard.
void ShowFloatingTrayMenu(HWND hWnd, int number, uint64_t serial, POINT at) {
    FloatingTrayMenu menu = BuildFloatingTrayMenu(number, serial);
    if (!menu.menu) {
        return;
    }
    // Without the foreground, a tray menu does not close when the user clicks
    // elsewhere; the WM_NULL afterwards is the documented companion.
    SetForegroundWindow(hWnd);
    const UINT command = TrackPopupMenu(menu.menu, TPM_RETURNCMD | TPM_RIGHTBUTTON |
                                                        TPM_BOTTOMALIGN | TPM_NONOTIFY,
                                        at.x, at.y, 0, hWnd, nullptr);
    PostMessageW(hWnd, WM_NULL, 0, 0);
    DestroyMenu(menu.menu);  // and the submenu with it
    RunFloatingTrayMenuCommand(menu, number, command);
}

// A mouse message's client position, on the screen.
POINT ScreenPointOf(HWND hWnd, LPARAM lParam) {
    POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    ClientToScreen(hWnd, &point);
    return point;
}

// Closes the popup an icon of this tray opened, if one did (DECISIONS 79).
void ClosePopupOf(FloatingTray* tray) {
    if (tray->popupSerial) {
        POINT cursor = {};
        GetCursorPos(&cursor);
        ForwardPopup(tray->popupSerial, false, cursor);
        tray->popupSerial = 0;
    }
}

// ---------------------------------------------------------------------------
// Screen readers and the keyboard in a floating tray (DECISIONS 85)
//
// A floating tray is a plain window that paints its icons, so on its own it is
// one blank rectangle to a screen reader. Each tray answers WM_GETOBJECT with a
// UI Automation provider: the tray, as a tool bar, and a button for each icon
// named as the taskbar's cells are (IconLabel, DECISIONS 81), or for an empty
// tray's handle. UI Automation calls a provider on the thread that owns its
// window - the tray thread - so these read the tray thread's own state.
// ---------------------------------------------------------------------------

// What a screen reader calls a cell: the icon's label, or the handle's purpose.
std::wstring FloatingCellName(uint64_t serial) {
    if (serial == 0) {
        return L"Split Tray menu";
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    const int index = IndexOfSerialLocked(serial);
    return index < 0 ? std::wstring() : IconLabel(g_icons[static_cast<size_t>(index)]);
}

class FloatingTrayUia;

// How many of these providers are alive. Each window lets go of its own as it
// is destroyed, and the tray thread's windows all are before the mod unloads:
// one still held then would be called into after the mod's code has gone.
std::atomic<int> g_uiaObjects{0};

// One cell: an icon's button, or an empty tray's handle (serial 0).
class FloatingCellUia final : public IRawElementProviderSimple,
                              public IRawElementProviderFragment,
                              public IInvokeProvider {
   public:
    FloatingCellUia(FloatingTrayUia* root, uint64_t serial);
    uint64_t Serial() const { return serial_; }

    STDMETHODIMP QueryInterface(REFIID riid, void** out) override;
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&refs_); }
    STDMETHODIMP_(ULONG) Release() override;

    STDMETHODIMP get_ProviderOptions(ProviderOptions* out) override;
    STDMETHODIMP GetPatternProvider(PATTERNID pattern, IUnknown** out) override;
    STDMETHODIMP GetPropertyValue(PROPERTYID property, VARIANT* out) override;
    STDMETHODIMP get_HostRawElementProvider(IRawElementProviderSimple** out) override;

    STDMETHODIMP Navigate(NavigateDirection direction,
                          IRawElementProviderFragment** out) override;
    STDMETHODIMP GetRuntimeId(SAFEARRAY** out) override;
    STDMETHODIMP get_BoundingRectangle(UiaRect* out) override;
    STDMETHODIMP GetEmbeddedFragmentRoots(SAFEARRAY** out) override;
    STDMETHODIMP SetFocus() override;
    STDMETHODIMP get_FragmentRoot(IRawElementProviderFragmentRoot** out) override;

    STDMETHODIMP Invoke() override;

   private:
    ~FloatingCellUia();
    FloatingTrayUia* root_;  // held
    uint64_t serial_;
    LONG refs_ = 1;
};

// The tray: the root of its cells, hosted by its window.
class FloatingTrayUia final : public IRawElementProviderSimple,
                              public IRawElementProviderFragment,
                              public IRawElementProviderFragmentRoot {
   public:
    FloatingTrayUia(HWND wnd, int number) : wnd_(wnd), number_(number) { g_uiaObjects++; }

    HWND Window() const { return wnd_; }
    int Number() const { return number_; }
    // Whether the tray is still there to be read; a provider outlives its
    // window for as long as a client holds it.
    bool Alive() const { return wnd_ != nullptr || testing_; }
    // The cell for `serial`, held for the caller, or null when it is not in
    // the tray now.
    FloatingCellUia* CellFor(uint64_t serial);
    // The cell whose icon has the keyboard focus, held, or null.
    FloatingCellUia* FocusedCell();
    // Lets go of cells for icons that have left the tray, and tells clients
    // the tray's children changed.
    void ChildrenChanged();
    // Tells clients the keyboard is on the cell for `serial`.
    void FocusMoved(uint64_t serial);
    // Called as the window goes: no client reaches this tray again.
    void Disconnect();
    // The regression tests read a tray that has no window.
    void TestWithoutWindow() { testing_ = true; }

    STDMETHODIMP QueryInterface(REFIID riid, void** out) override;
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&refs_); }
    STDMETHODIMP_(ULONG) Release() override;

    STDMETHODIMP get_ProviderOptions(ProviderOptions* out) override;
    STDMETHODIMP GetPatternProvider(PATTERNID pattern, IUnknown** out) override;
    STDMETHODIMP GetPropertyValue(PROPERTYID property, VARIANT* out) override;
    STDMETHODIMP get_HostRawElementProvider(IRawElementProviderSimple** out) override;

    STDMETHODIMP Navigate(NavigateDirection direction,
                          IRawElementProviderFragment** out) override;
    STDMETHODIMP GetRuntimeId(SAFEARRAY** out) override;
    STDMETHODIMP get_BoundingRectangle(UiaRect* out) override;
    STDMETHODIMP GetEmbeddedFragmentRoots(SAFEARRAY** out) override;
    STDMETHODIMP SetFocus() override;
    STDMETHODIMP get_FragmentRoot(IRawElementProviderFragmentRoot** out) override;

    STDMETHODIMP ElementProviderFromPoint(double x,
                                          double y,
                                          IRawElementProviderFragment** out) override;
    STDMETHODIMP GetFocus(IRawElementProviderFragment** out) override;

   private:
    ~FloatingTrayUia() { g_uiaObjects--; }
    HWND wnd_;
    int number_;
    bool testing_ = false;
    LONG refs_ = 1;
    std::map<uint64_t, FloatingCellUia*> cells_;  // each held
};

FloatingTray* FloatingTrayOfNumber(int number) {
    auto found = g_floatingTrays.find(number);
    return found != g_floatingTrays.end() ? &found->second : nullptr;
}

// Keeps the keyboard on a cell that is still there: on the first one when its
// icon has gone.
void KeepKeyboardInTray(FloatingTray& tray, int number) {
    const FloatingView view = FloatingViewOf(number, /*withIcons=*/false);
    if (CellIndexOfSerial(view, tray.focusSerial) < 0) {
        tray.focusSerial = SerialOfCellIndex(view, 0);
    }
}

// Puts the keyboard on the cell for `serial` in the floating tray `wnd`, taking
// the foreground for it: an application giving the focus back to its icon
// (NIM_SETFOCUS), or a screen reader moving to it.
void FocusFloatingCell(HWND wnd, uint64_t serial) {
    FloatingTray* tray = FloatingTrayOfWindow(wnd);
    if (!tray) {
        return;
    }
    tray->focusSerial = serial;
    HWND foreground = GetForegroundWindow();
    if (foreground != wnd) {
        NoteForeground(foreground);
        if (!SetForegroundWindow(wnd)) {
            Wh_Log(L"tray %d could not take the foreground for the keyboard",
                   TrayNumberOfWindow(wnd));
        }
    }
    ::SetFocus(wnd);
    InvalidateRect(wnd, nullptr, FALSE);
    if (tray->uia && tray->focused) {
        tray->uia->FocusMoved(serial);
    }
}

// The icon with `serial` given the focus in whichever floating tray shows it,
// as NIM_SETFOCUS asks (DECISIONS 80, 85). Tray thread only.
void FocusIconInFloatingTray(uint64_t serial) {
    if (!serial) {
        return;
    }
    for (auto& [number, tray] : g_floatingTrays) {
        const FloatingView view = FloatingViewOf(number, /*withIcons=*/false);
        if (tray.wnd && !view.serials.empty() && CellIndexOfSerial(view, serial) >= 0) {
            FocusFloatingCell(tray.wnd, serial);
            return;
        }
    }
}

// A cell chosen from the keyboard or by a screen reader. An icon's application
// is told as Explorer tells it (ForwardKey, DECISIONS 80), with the middle of
// the cell as the anchor for its menu; an empty tray's handle opens the mod's
// menu, as a click on it does.
void ActivateFloatingCell(HWND wnd, uint64_t serial, bool contextMenu) {
    const int number = TrayNumberOfWindow(wnd);
    const FloatingView view = FloatingViewOf(number, /*withIcons=*/false);
    const int index = CellIndexOfSerial(view, serial);
    if (index < 0) {
        return;
    }
    RECT cell = {};
    POINT anchor = {};
    if (CellScreenRect(view.layout, index, view.Cells(), &cell)) {
        anchor = {(cell.left + cell.right) / 2, (cell.top + cell.bottom) / 2};
    }
    if (serial == 0) {
        ShowFloatingTrayMenu(wnd, number, 0, anchor);
        return;
    }
    ForwardKey(serial, contextMenu, anchor);
}

// Whether the keyboard is on the cell for `serial` in tray `number`.
bool FloatingCellHasFocus(int number, uint64_t serial) {
    const FloatingTray* tray = FloatingTrayOfNumber(number);
    return tray && tray->focused && tray->focusSerial == serial;
}

VARIANT BoolVariant(bool value) {
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_BOOL;
    v.boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
    return v;
}

VARIANT IntVariant(LONG value) {
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_I4;
    v.lVal = value;
    return v;
}

VARIANT StringVariant(const std::wstring& value) {
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(value.c_str());
    return v;
}

// --- a cell ----------------------------------------------------------------

FloatingCellUia::FloatingCellUia(FloatingTrayUia* root, uint64_t serial)
    : root_(root), serial_(serial) {
    root_->AddRef();
    g_uiaObjects++;
}

FloatingCellUia::~FloatingCellUia() {
    root_->Release();
    g_uiaObjects--;
}

STDMETHODIMP FloatingCellUia::QueryInterface(REFIID riid, void** out) {
    if (!out) {
        return E_POINTER;
    }
    if (riid == __uuidof(IUnknown) || riid == __uuidof(IRawElementProviderSimple)) {
        *out = static_cast<IRawElementProviderSimple*>(this);
    } else if (riid == __uuidof(IRawElementProviderFragment)) {
        *out = static_cast<IRawElementProviderFragment*>(this);
    } else if (riid == __uuidof(IInvokeProvider)) {
        *out = static_cast<IInvokeProvider*>(this);
    } else {
        *out = nullptr;
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) FloatingCellUia::Release() {
    const LONG left = InterlockedDecrement(&refs_);
    if (left == 0) {
        delete this;
    }
    return static_cast<ULONG>(left);
}

// The cell's place in its tray now; false once its icon or its tray has gone.
bool CellNow(const FloatingTrayUia* root, uint64_t serial, FloatingView* view, int* index) {
    if (!root->Alive()) {
        return false;
    }
    *view = FloatingViewOf(root->Number(), /*withIcons=*/false);
    *index = CellIndexOfSerial(*view, serial);
    return *index >= 0;
}

STDMETHODIMP FloatingCellUia::get_ProviderOptions(ProviderOptions* out) {
    if (!out) {
        return E_POINTER;
    }
    *out = ProviderOptions_ServerSideProvider;
    return S_OK;
}

STDMETHODIMP FloatingCellUia::GetPatternProvider(PATTERNID pattern, IUnknown** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = nullptr;
    if (pattern == UIA_InvokePatternId) {
        *out = static_cast<IInvokeProvider*>(this);
        AddRef();
    }
    return S_OK;
}

STDMETHODIMP FloatingCellUia::GetPropertyValue(PROPERTYID property, VARIANT* out) {
    if (!out) {
        return E_POINTER;
    }
    VariantInit(out);
    FloatingView view;
    int index = -1;
    if (!CellNow(root_, serial_, &view, &index)) {
        return UIA_E_ELEMENTNOTAVAILABLE;
    }
    switch (property) {
        case UIA_ControlTypePropertyId:
            *out = IntVariant(UIA_ButtonControlTypeId);
            break;
        case UIA_NamePropertyId:
            *out = StringVariant(FloatingCellName(serial_));
            break;
        case UIA_IsKeyboardFocusablePropertyId:
        case UIA_IsEnabledPropertyId:
            *out = BoolVariant(true);
            break;
        case UIA_HasKeyboardFocusPropertyId:
            *out = BoolVariant(FloatingCellHasFocus(root_->Number(), serial_));
            break;
    }
    return S_OK;
}

STDMETHODIMP FloatingCellUia::get_HostRawElementProvider(IRawElementProviderSimple** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = nullptr;
    return S_OK;
}

STDMETHODIMP FloatingCellUia::Navigate(NavigateDirection direction,
                                       IRawElementProviderFragment** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = nullptr;
    FloatingView view;
    int index = -1;
    if (!CellNow(root_, serial_, &view, &index)) {
        return UIA_E_ELEMENTNOTAVAILABLE;
    }
    int sibling = -1;
    switch (direction) {
        case NavigateDirection_Parent:
            *out = static_cast<IRawElementProviderFragment*>(root_);
            root_->AddRef();
            return S_OK;
        case NavigateDirection_NextSibling:
            sibling = index + 1;
            break;
        case NavigateDirection_PreviousSibling:
            sibling = index - 1;
            break;
        default:
            return S_OK;  // a cell has no children
    }
    if (sibling >= 0 && sibling < static_cast<int>(view.serials.size())) {
        if (FloatingCellUia* cell = root_->CellFor(SerialOfCellIndex(view, sibling))) {
            *out = static_cast<IRawElementProviderFragment*>(cell);
        }
    }
    return S_OK;
}

STDMETHODIMP FloatingCellUia::GetRuntimeId(SAFEARRAY** out) {
    if (!out) {
        return E_POINTER;
    }
    // Appended to the tray's own, so unique across trays; a serial is never
    // reused, so a new icon is a new element.
    LONG parts[3] = {UiaAppendRuntimeId, static_cast<LONG>(serial_ & 0xFFFFFFFF),
                     static_cast<LONG>(serial_ >> 32)};
    SAFEARRAY* ids = SafeArrayCreateVector(VT_I4, 0, 3);
    if (!ids) {
        return E_OUTOFMEMORY;
    }
    for (LONG i = 0; i < 3; i++) {
        SafeArrayPutElement(ids, &i, &parts[i]);
    }
    *out = ids;
    return S_OK;
}

STDMETHODIMP FloatingCellUia::get_BoundingRectangle(UiaRect* out) {
    if (!out) {
        return E_POINTER;
    }
    *out = UiaRect{};
    FloatingView view;
    int index = -1;
    if (!CellNow(root_, serial_, &view, &index)) {
        return UIA_E_ELEMENTNOTAVAILABLE;
    }
    RECT cell = {};
    if (CellScreenRect(view.layout, index, view.Cells(), &cell)) {
        *out = UiaRect{static_cast<double>(cell.left), static_cast<double>(cell.top),
                       static_cast<double>(cell.right - cell.left),
                       static_cast<double>(cell.bottom - cell.top)};
    }
    return S_OK;
}

STDMETHODIMP FloatingCellUia::GetEmbeddedFragmentRoots(SAFEARRAY** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = nullptr;
    return S_OK;
}

STDMETHODIMP FloatingCellUia::SetFocus() {
    FloatingView view;
    int index = -1;
    if (!CellNow(root_, serial_, &view, &index)) {
        return UIA_E_ELEMENTNOTAVAILABLE;
    }
    if (HWND wnd = root_->Window()) {
        PostMessageW(wnd, WM_ST_CELL_FOCUS, static_cast<WPARAM>(serial_), 0);
    }
    return S_OK;
}

STDMETHODIMP FloatingCellUia::get_FragmentRoot(IRawElementProviderFragmentRoot** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = static_cast<IRawElementProviderFragmentRoot*>(root_);
    root_->AddRef();
    return S_OK;
}

STDMETHODIMP FloatingCellUia::Invoke() {
    FloatingView view;
    int index = -1;
    if (!CellNow(root_, serial_, &view, &index)) {
        return UIA_E_ELEMENTNOTAVAILABLE;
    }
    // Invoke is to return at once, and the handle's menu is modal: posted.
    if (HWND wnd = root_->Window()) {
        PostMessageW(wnd, WM_ST_CELL_ACTIVATE, static_cast<WPARAM>(serial_), 0);
    }
    return S_OK;
}

// --- the tray ------------------------------------------------------------------

FloatingCellUia* FloatingTrayUia::CellFor(uint64_t serial) {
    if (!Alive()) {
        return nullptr;
    }
    const FloatingView view = FloatingViewOf(number_, /*withIcons=*/false);
    if (CellIndexOfSerial(view, serial) < 0) {
        return nullptr;
    }
    FloatingCellUia*& cell = cells_[serial];
    if (!cell) {
        cell = new FloatingCellUia(this, serial);  // the map's reference
    }
    cell->AddRef();  // the caller's
    return cell;
}

FloatingCellUia* FloatingTrayUia::FocusedCell() {
    const FloatingTray* tray = FloatingTrayOfNumber(number_);
    return tray && tray->focused ? CellFor(tray->focusSerial) : nullptr;
}

void FloatingTrayUia::ChildrenChanged() {
    const FloatingView view = FloatingViewOf(number_, /*withIcons=*/false);
    for (auto it = cells_.begin(); it != cells_.end();) {
        if (CellIndexOfSerial(view, it->first) >= 0) {
            ++it;
            continue;
        }
        UiaDisconnectProvider(static_cast<IRawElementProviderSimple*>(it->second));
        it->second->Release();
        it = cells_.erase(it);
    }
    if (wnd_ && UiaClientsAreListening()) {
        UiaRaiseStructureChangedEvent(static_cast<IRawElementProviderSimple*>(this),
                                      StructureChangeType_ChildrenInvalidated, nullptr, 0);
    }
}

void FloatingTrayUia::FocusMoved(uint64_t serial) {
    if (!wnd_ || !UiaClientsAreListening()) {
        return;
    }
    if (FloatingCellUia* cell = CellFor(serial)) {
        UiaRaiseAutomationEvent(static_cast<IRawElementProviderSimple*>(cell),
                                UIA_AutomationFocusChangedEventId);
        cell->Release();
    }
}

void FloatingTrayUia::Disconnect() {
    for (auto& [serial, cell] : cells_) {
        UiaDisconnectProvider(static_cast<IRawElementProviderSimple*>(cell));
        cell->Release();
    }
    cells_.clear();
    UiaDisconnectProvider(static_cast<IRawElementProviderSimple*>(this));
    wnd_ = nullptr;
    testing_ = false;
}

STDMETHODIMP FloatingTrayUia::QueryInterface(REFIID riid, void** out) {
    if (!out) {
        return E_POINTER;
    }
    if (riid == __uuidof(IUnknown) || riid == __uuidof(IRawElementProviderSimple)) {
        *out = static_cast<IRawElementProviderSimple*>(this);
    } else if (riid == __uuidof(IRawElementProviderFragment)) {
        *out = static_cast<IRawElementProviderFragment*>(this);
    } else if (riid == __uuidof(IRawElementProviderFragmentRoot)) {
        *out = static_cast<IRawElementProviderFragmentRoot*>(this);
    } else {
        *out = nullptr;
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) FloatingTrayUia::Release() {
    const LONG left = InterlockedDecrement(&refs_);
    if (left == 0) {
        delete this;
    }
    return static_cast<ULONG>(left);
}

STDMETHODIMP FloatingTrayUia::get_ProviderOptions(ProviderOptions* out) {
    if (!out) {
        return E_POINTER;
    }
    *out = ProviderOptions_ServerSideProvider;
    return S_OK;
}

STDMETHODIMP FloatingTrayUia::GetPatternProvider(PATTERNID, IUnknown** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = nullptr;
    return S_OK;
}

STDMETHODIMP FloatingTrayUia::GetPropertyValue(PROPERTYID property, VARIANT* out) {
    if (!out) {
        return E_POINTER;
    }
    VariantInit(out);
    if (!Alive()) {
        return UIA_E_ELEMENTNOTAVAILABLE;
    }
    switch (property) {
        case UIA_ControlTypePropertyId:
            *out = IntVariant(UIA_ToolBarControlTypeId);
            break;
        case UIA_NamePropertyId:
            *out = StringVariant(L"Split Tray, " + TrayLabel(number_));
            break;
    }
    return S_OK;
}

STDMETHODIMP FloatingTrayUia::get_HostRawElementProvider(IRawElementProviderSimple** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = nullptr;
    return wnd_ ? UiaHostProviderFromHwnd(wnd_, out) : S_OK;
}

STDMETHODIMP FloatingTrayUia::Navigate(NavigateDirection direction,
                                       IRawElementProviderFragment** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = nullptr;
    if (!Alive() ||
        (direction != NavigateDirection_FirstChild && direction != NavigateDirection_LastChild)) {
        return S_OK;  // the window's host provides its parent and siblings
    }
    const FloatingView view = FloatingViewOf(number_, /*withIcons=*/false);
    const int index = direction == NavigateDirection_FirstChild ? 0 : view.Cells() - 1;
    if (FloatingCellUia* cell = CellFor(SerialOfCellIndex(view, index))) {
        *out = static_cast<IRawElementProviderFragment*>(cell);
    }
    return S_OK;
}

STDMETHODIMP FloatingTrayUia::GetRuntimeId(SAFEARRAY** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = nullptr;  // a root hosted by a window takes the window's
    return S_OK;
}

STDMETHODIMP FloatingTrayUia::get_BoundingRectangle(UiaRect* out) {
    if (!out) {
        return E_POINTER;
    }
    *out = UiaRect{};  // the window's, from its host
    return S_OK;
}

STDMETHODIMP FloatingTrayUia::GetEmbeddedFragmentRoots(SAFEARRAY** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = nullptr;
    return S_OK;
}

STDMETHODIMP FloatingTrayUia::SetFocus() {
    if (!wnd_) {
        return Alive() ? S_OK : UIA_E_ELEMENTNOTAVAILABLE;
    }
    if (FloatingTray* tray = FloatingTrayOfNumber(number_)) {
        KeepKeyboardInTray(*tray, number_);
        PostMessageW(wnd_, WM_ST_CELL_FOCUS, static_cast<WPARAM>(tray->focusSerial), 0);
    }
    return S_OK;
}

STDMETHODIMP FloatingTrayUia::get_FragmentRoot(IRawElementProviderFragmentRoot** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = static_cast<IRawElementProviderFragmentRoot*>(this);
    AddRef();
    return S_OK;
}

STDMETHODIMP FloatingTrayUia::ElementProviderFromPoint(double x,
                                                       double y,
                                                       IRawElementProviderFragment** out) {
    if (!out) {
        return E_POINTER;
    }
    *out = nullptr;
    if (!Alive()) {
        return S_OK;
    }
    const FloatingView view = FloatingViewOf(number_, /*withIcons=*/false);
    const int index = HitTestCell(view.layout, static_cast<int>(x) - view.layout.x,
                                  static_cast<int>(y) - view.layout.y, view.Cells());
    if (index >= 0) {
        if (FloatingCellUia* cell = CellFor(SerialOfCellIndex(view, index))) {
            *out = static_cast<IRawElementProviderFragment*>(cell);
        }
    }
    return S_OK;
}

STDMETHODIMP FloatingTrayUia::GetFocus(IRawElementProviderFragment** out) {
    if (!out) {
        return E_POINTER;
    }
    FloatingCellUia* cell = FocusedCell();
    *out = cell ? static_cast<IRawElementProviderFragment*>(cell) : nullptr;
    return S_OK;
}

LRESULT CALLBACK FloatingTrayProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT:
            PaintTray(hWnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;  // painted in WM_PAINT

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        // --- the keyboard and screen readers (DECISIONS 85) -----------------
        case WM_GETOBJECT:
            // UI Automation's own request, and an older client's: the
            // provider answers both, the second through Windows' bridge.
            if (static_cast<LONG>(lParam) == UiaRootObjectId ||
                static_cast<LONG>(lParam) == OBJID_CLIENT) {
                if (FloatingTray* tray = FloatingTrayOfWindow(hWnd)) {
                    if (!tray->uia) {
                        tray->uia = new FloatingTrayUia(hWnd, TrayNumberOfWindow(hWnd));
                    }
                    return UiaReturnRawElementProvider(
                        hWnd, wParam, lParam,
                        static_cast<IRawElementProviderSimple*>(tray->uia));
                }
            }
            break;

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            if (FloatingTray* tray = FloatingTrayOfWindow(hWnd)) {
                tray->focused = msg == WM_SETFOCUS;
                if (tray->focused) {
                    KeepKeyboardInTray(*tray, TrayNumberOfWindow(hWnd));
                    if (tray->uia) {
                        tray->uia->FocusMoved(tray->focusSerial);
                    }
                }
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;

        case WM_KEYDOWN: {
            FloatingTray* tray = FloatingTrayOfWindow(hWnd);
            if (!tray) {
                break;
            }
            if (wParam == VK_RETURN || wParam == VK_SPACE) {
                ActivateFloatingCell(hWnd, tray->focusSerial, /*contextMenu=*/false);
                return 0;
            }
            if (wParam == VK_ESCAPE) {
                // Back to where the user was before the tray took the focus.
                if (g_lastForeground && IsWindow(g_lastForeground)) {
                    SetForegroundWindow(g_lastForeground);
                }
                return 0;
            }
            const FloatingView view =
                FloatingViewOf(TrayNumberOfWindow(hWnd), /*withIcons=*/false);
            const int current = CellIndexOfSerial(view, tray->focusSerial);
            const int next = NextFocusIndex(current, static_cast<UINT>(wParam), view.Cells(),
                                            view.layout.columns);
            if (next >= 0 && next != current) {
                tray->focusSerial = SerialOfCellIndex(view, next);
                InvalidateRect(hWnd, nullptr, FALSE);
                if (tray->uia) {
                    tray->uia->FocusMoved(tray->focusSerial);
                }
                return 0;
            }
            break;
        }

        case WM_CONTEXTMENU:
            // Shift+F10 or the menu key, which DefWindowProc turns into this
            // with no position. A right click never gets here: its release is
            // the application's, and is not passed on (DECISIONS 30).
            if (GET_X_LPARAM(lParam) == -1 && GET_Y_LPARAM(lParam) == -1) {
                if (FloatingTray* tray = FloatingTrayOfWindow(hWnd)) {
                    ActivateFloatingCell(hWnd, tray->focusSerial, /*contextMenu=*/true);
                }
            }
            return 0;

        case WM_ST_CELL_ACTIVATE:
            ActivateFloatingCell(hWnd, static_cast<uint64_t>(wParam), /*contextMenu=*/false);
            return 0;

        case WM_ST_CELL_FOCUS:
            FocusFloatingCell(hWnd, static_cast<uint64_t>(wParam));
            return 0;

        case WM_MOUSEMOVE: {
            FloatingTray* tray = FloatingTrayOfWindow(hWnd);
            if (!tray) {
                return 0;
            }
            const FloatingView view =
                FloatingViewOf(TrayNumberOfWindow(hWnd), /*withIcons=*/false);
            const int index = HitTestCell(view.layout, GET_X_LPARAM(lParam),
                                          GET_Y_LPARAM(lParam), view.Cells());
            if (index != tray->hotIndex) {
                tray->hotIndex = index;
                UpdateTooltipText(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                ClosePopupOf(tray);
            }
            // Explorer tells an icon's application the pointer is moving over
            // it (DECISIONS 79).
            if (index >= 0 && index < static_cast<int>(view.serials.size())) {
                ForwardHover(view.serials[static_cast<size_t>(index)],
                             ScreenPointOf(hWnd, lParam));
            }
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE | TME_HOVER, hWnd, HOVER_DEFAULT};
            TrackMouseEvent(&tme);
            break;  // on to the tooltip relay below
        }

        case WM_MOUSEHOVER: {
            // Resting on an icon that draws its own popup opens it.
            FloatingTray* tray = FloatingTrayOfWindow(hWnd);
            const FloatingView view =
                FloatingViewOf(TrayNumberOfWindow(hWnd), /*withIcons=*/false);
            const int index = HitTestCell(view.layout, GET_X_LPARAM(lParam),
                                          GET_Y_LPARAM(lParam), view.Cells());
            if (tray && !tray->popupSerial && index >= 0 &&
                index < static_cast<int>(view.serials.size())) {
                const uint64_t serial = view.serials[static_cast<size_t>(index)];
                if (ForwardPopup(serial, true, ScreenPointOf(hWnd, lParam))) {
                    tray->popupSerial = serial;
                }
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            if (FloatingTray* tray = FloatingTrayOfWindow(hWnd)) {
                tray->hotIndex = -1;
                ClosePopupOf(tray);
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {
            const int number = TrayNumberOfWindow(hWnd);
            const FloatingView view = FloatingViewOf(number, /*withIcons=*/false);
            const POINT client = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const int index = HitTestCell(view.layout, client.x, client.y, view.Cells());
            if (index < 0) {
                return 0;
            }
            POINT screen = client;
            ClientToScreen(hWnd, &screen);

            const bool onIcon = index < static_cast<int>(view.serials.size());
            const bool release = (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP ||
                                  msg == WM_MBUTTONUP);
            // The handle belongs to the mod, so any click on it is the mod's.
            // On an icon, plain right-click is the application's (DECISIONS 30)
            // and Shift+right-click is the mod's.
            if (!onIcon) {
                if (release) {
                    ShowFloatingTrayMenu(hWnd, number, 0, screen);
                }
                return 0;
            }
            if (ShiftHeld() && (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)) {
                if (msg == WM_RBUTTONUP) {
                    ShowFloatingTrayMenu(hWnd, number,
                                         view.serials[static_cast<size_t>(index)],
                                         screen);
                }
                return 0;
            }
            ForwardClick(view.serials[static_cast<size_t>(index)], msg, screen);
            return 0;
        }

        case WM_NCHITTEST:
            return HTCLIENT;  // never show a resize or caption cursor

        case WM_DESTROY:
            if (FloatingTray* tray = FloatingTrayOfWindow(hWnd)) {
                if (tray->tooltip) {
                    DestroyWindow(tray->tooltip);
                    tray->tooltip = nullptr;
                }
                // No client reaches the tray's provider after this: the mod's
                // code may be unloaded next, from inside Explorer, while a
                // screen reader still holds one of its buttons.
                if (tray->uia) {
                    UiaReturnRawElementProvider(hWnd, 0, 0, nullptr);
                    tray->uia->Disconnect();
                    tray->uia->Release();
                    tray->uia = nullptr;
                }
            }
            return 0;
    }

    FloatingTray* tray = FloatingTrayOfWindow(hWnd);
    if (tray && tray->tooltip && msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) {
        MSG relay = {hWnd, msg, wParam, lParam};
        SendMessageW(tray->tooltip, TTM_RELAYEVENT, 0, reinterpret_cast<LPARAM>(&relay));
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

HWND CreateTooltip(HWND owner) {
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_BAR_CLASSES};
    InitCommonControlsEx(&icc);

    HWND tooltip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                                   WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX, 0, 0, 0, 0,
                                   owner, nullptr, nullptr, nullptr);
    if (!tooltip) {
        return nullptr;
    }
    // The whole window is one tool; its text follows the cell under the pointer.
    TTTOOLINFOW ti = {sizeof(ti)};
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = owner;
    ti.uId = 0;
    ti.rect = RECT{0, 0, 32767, 32767};
    ti.lpszText = const_cast<PWSTR>(L" ");
    SendMessageW(tooltip, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&ti));
    SendMessageW(tooltip, TTM_SETMAXTIPWIDTH, 0, 400);
    return tooltip;
}

// Brings the floating panels in line with the trays: one for every tray that
// floats, where its layout says, and none for any other. Tray thread only.
void SyncFloatingTrays() {
    std::map<int, TrayLayout> layouts;
    int opacity;
    bool onTop;
    bool tooltips;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        RecomputeGeometryLocked();
        if (!g_unloading.load()) {
            layouts = g_floatingLayouts;
        }
        opacity = g_settings.opacity;
        onTop = g_settings.alwaysOnTop;
        tooltips = g_settings.showTooltips;
    }
    // None in an Explorer that does not show the taskbar: one loaded before
    // the taskbar existed finds out only now. With no taskbar anywhere - while
    // Explorer makes its own again - they stay.
    const bool elsewhere = TaskbarShownNow() == TaskbarShownBy::AnotherProcess;
    if (elsewhere != g_taskbarElsewhere) {
        g_taskbarElsewhere = elsewhere;
        Wh_Log(L"%s", elsewhere ? L"the taskbar belongs to another process: this "
                                  L"Explorer draws no trays"
                                : L"no other process shows the taskbar now");
    }
    if (elsewhere) {
        layouts.clear();
    }

    for (auto it = g_floatingTrays.begin(); it != g_floatingTrays.end();) {
        if (layouts.count(it->first)) {
            ++it;
            continue;
        }
        const int number = it->first;
        HWND wnd = it->second.wnd;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_floatingWnds.erase(number);
        }
        if (wnd) {
            DestroyWindow(wnd);  // WM_DESTROY takes its tooltip with it
        }
        it = g_floatingTrays.erase(it);
    }

    for (const auto& [number, layout] : layouts) {
        FloatingTray& tray = g_floatingTrays[number];
        if (!tray.wnd) {
            tray.wnd = CreateWindowExW(
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED |
                    (onTop ? WS_EX_TOPMOST : 0),
                kFloatingClassName, L"Split Tray", WS_POPUP, layout.x, layout.y,
                std::max(1, layout.width), std::max(1, layout.height), nullptr, nullptr,
                ModuleInstance(), nullptr);
            if (!tray.wnd) {
                Wh_Log(L"could not create the floating panel for tray %d: %lu", number,
                       GetLastError());
                g_floatingTrays.erase(number);
                continue;
            }
            SetWindowLongPtrW(tray.wnd, GWLP_USERDATA, number);
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                g_floatingWnds[number] = tray.wnd;
            }
            Wh_Log(L"tray %d floats at (%d,%d) %dx%d", number, layout.x, layout.y,
                   layout.width, layout.height);
        }
        // Every time, not only for a new window: the setting was read when the
        // window was made, so switching it had no effect on a tray already
        // there (DECISIONS 77).
        if (tooltips && !tray.tooltip) {
            tray.tooltip = CreateTooltip(tray.wnd);
        } else if (!tooltips && tray.tooltip) {
            TTTOOLINFOW ti = {sizeof(ti)};
            ti.hwnd = tray.wnd;
            ti.uId = 0;
            SendMessageW(tray.tooltip, TTM_DELTOOLW, 0, reinterpret_cast<LPARAM>(&ti));
            DestroyWindow(tray.tooltip);
            tray.tooltip = nullptr;
        }
        SetLayeredWindowAttributes(tray.wnd, 0, static_cast<BYTE>(opacity), LWA_ALPHA);
        SetWindowPos(tray.wnd, onTop ? HWND_TOPMOST : HWND_NOTOPMOST, layout.x, layout.y,
                     layout.width, layout.height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        InvalidateRect(tray.wnd, nullptr, FALSE);
        // Its icons may have come and gone: the keyboard stays on one that is
        // there, and a screen reader is told to read the tray again.
        KeepKeyboardInTray(tray, number);
        if (tray.uia) {
            tray.uia->ChildrenChanged();
        }
    }
}

void DestroyFloatingTrays() {
    for (auto& [number, tray] : g_floatingTrays) {
        if (tray.wnd) {
            DestroyWindow(tray.wnd);
        }
    }
    g_floatingTrays.clear();
    std::lock_guard<std::mutex> lock(g_mutex);
    g_floatingWnds.clear();
}

LRESULT CALLBACK ControllerProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Explorer's own announcement that its taskbar is ready. This window is a
    // top-level one, so it hears it like any application's.
    if (msg == TaskbarCreatedMessage()) {
        NoteShellAnnouncedTaskbar();
        return 0;
    }

    switch (msg) {
        case WM_ST_SETTINGS:
            // New rules can change where an icon that is already on screen
            // belongs, so re-resolve every tracked icon and replay the
            // difference into the shell before re-laying out.
            ApplySettingsToTrackedIcons();
            SyncFloatingTrays();
            RefreshArrangeWindow(true);
#ifndef SPLITTRAY_NO_XAML
            SplitTrayXaml::RequestEmbeddedRefresh();
#endif
            return 0;

        case WM_ST_REFRESH:
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            for (auto& [number, tray] : g_floatingTrays) {
                tray.hotIndex = -1;
            }
            SyncFloatingTrays();
            RefreshArrangeWindow(false);
            return 0;

        case WM_ST_ARRANGE:
            ShowArrangeWindow();
            return 0;

        case WM_ST_FOCUS_ICON:
            FocusIconInFloatingTray(static_cast<uint64_t>(wParam));
            return 0;

        case WM_ST_SHUTDOWN:
            CloseArrangeWindow();
            DestroyFloatingTrays();
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ArrangeWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ReleaseArrangeResources();

// Everything the tray thread registers, taken down again once its windows are
// gone; a class with a window still open cannot be unregistered.
void UnregisterTrayThreadClasses() {
    UnregisterModClass(kArrangeClassName);
    UnregisterModClass(kFloatingClassName);
    UnregisterModClass(kControllerClassName);
}

DWORD WINAPI TrayThreadProc(LPVOID) {
    if (g_trayThreadHold) {
        WaitForSingleObject(g_trayThreadHold, INFINITE);
    }

    WNDCLASSEXW controllerClass = {sizeof(controllerClass)};
    controllerClass.lpfnWndProc = ControllerProc;
    controllerClass.lpszClassName = kControllerClassName;

    WNDCLASSEXW floatingClass = {sizeof(floatingClass)};
    floatingClass.lpfnWndProc = FloatingTrayProc;
    floatingClass.lpszClassName = kFloatingClassName;
    floatingClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    floatingClass.style = CS_DBLCLKS;

    WNDCLASSEXW arrangeClass = {sizeof(arrangeClass)};
    arrangeClass.lpfnWndProc = ArrangeWndProc;
    arrangeClass.lpszClassName = kArrangeClassName;
    arrangeClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    arrangeClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);

    // All or nothing: without its thread the mod never attaches, and every
    // icon stays where Explorer puts it (DECISIONS 69). RegisterModClass says
    // which failed. Wh_ModInit may have stopped waiting already, so this is
    // where the outcome is logged.
    auto giveUp = []() -> DWORD {
        UnregisterTrayThreadClasses();
        Wh_Log(L"the tray thread gave up; Split Tray leaves Explorer's tray alone");
        g_trayThreadState.store(TrayThreadState::GaveUp);
        return 1;
    };
    if (!RegisterModClass(&controllerClass) || !RegisterModClass(&floatingClass) ||
        !RegisterModClass(&arrangeClass)) {
        return giveUp();
    }

    // Never shown. A hidden top-level window still receives broadcasts, which
    // a message-only window would not - and TaskbarCreated is one.
    HWND hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, kControllerClassName, L"Split Tray",
                                WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                ModuleInstance(), nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowExW for the controller window failed: %u", GetLastError());
        return giveUp();
    }

    g_trayWnd.store(hWnd);
    g_trayThreadState.store(TrayThreadState::Running);
    g_taskbarElsewhere = false;
    SyncFloatingTrays();

    // Cheap watchdog: prunes icons whose owner died, notices display changes that
    // arrive without a WM_DISPLAYCHANGE, and re-subclasses a recreated taskbar.
    SetTimer(hWnd, 1, 2000, nullptr);

    // Which window had the foreground, for Escape in a floating tray
    // (DECISIONS 85). Out of context, so it is called on this thread, from its
    // message loop; taken off below, before the thread ends and long before
    // the mod's code can be unloaded.
    NoteForeground(GetForegroundWindow());
    HWINEVENTHOOK foregroundHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                        ForegroundChanged, 0, 0, WINEVENT_OUTOFCONTEXT);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_TIMER && msg.hwnd == hWnd) {
            EnsureShellTrayWindowSubclassed();
#ifndef SPLITTRAY_NO_XAML
            SplitTrayXaml::EnsureTaskbarXamlHooked();
            // The IconView constructor hook only catches elements built after it
            // is installed, and resolving the symbols takes seconds - long
            // enough that on a normal boot the other taskbars' trays are already
            // built and the mod never sees an element on them at all. Asking
            // again on the timer is the same remedy as DECISIONS 18, for the
            // same shape of defect.
            if (SplitTrayXaml::AnyDisplayTrayWaitingToEmbed()) {
                if (HWND tray = g_shellTrayWnd.load()) {
                    PostMessageW(tray, GetAttachMessage(), 0, 0);
                }
            }
#endif
            // Also wakes the taskbar's thread while any icon is waiting for
            // it: a wake-up can be lost - the taskbar was being recreated when
            // it was posted - and one Explorer refused is asked again.
            ReplayRoutingChanges();
            SyncFloatingTrays();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (foregroundHook) {
        UnhookWinEvent(foregroundHook);
    }
    g_trayWnd.store(nullptr);
    CloseArrangeWindow();
    DestroyFloatingTrays();
    ReleaseArrangeResources();
    UnregisterTrayThreadClasses();
    return 0;
}

// ============================================================================
// Section 6b - The arrange window
//
// One list per tray: the primary tray, then each of Split Tray's that exists.
// An icon in a tray's overflow is listed in that tray and marked. Drag an icon
// to another list, or select it and press that tray's number; H puts it in its
// tray's overflow or brings it back out. Every choice is remembered.
//
// This exists because dragging inside the taskbar could not be made to work.
// The mod's cells live in Explorer's XAML island, where the pointer does not
// behave the way it does in an ordinary window: pointer capture on our Border
// is not enough to keep a drag alive, and two attempts at it changed nothing
// the user could see. This window is the mod's own, on the mod's own thread,
// with no other claim on its input - so a drag here is just a drag.
//
// Plain Win32 rather than XAML: the mod's thread already pumps messages, and
// this way the feature does not depend on any of the taskbar internals that
// have been the fragile part of this project throughout.
// ============================================================================

constexpr int kArrangeListIdBase = 1001;

// Tray-thread state only; no locking needed beyond the snapshot itself.
HWND g_arrangeWnd = nullptr;
// Whether it is open, for the taskbar's thread: a tooltip changed in the main
// tray asks the arrange window to rename its row only then (DECISIONS 83).
std::atomic<bool> g_arrangeOpen{false};
std::vector<HWND> g_arrangeLists;
std::vector<int> g_arrangeTrays;          // the tray number each list shows
// One image list for every list in the window. The window owns it, and each
// list is created with LVS_SHAREIMAGELISTS: without that, every list destroys
// the image list it was given when it is destroyed itself, so closing the
// window freed the one list once per list, after the window had freed it
// already.
HIMAGELIST g_arrangeImages = nullptr;
HFONT g_arrangeFont = nullptr;  // the shell's message font, made once
std::vector<std::wstring> g_arrangeKeys;  // indexed by ListView item lParam
std::vector<uint64_t> g_arrangeSerials;   // likewise: which icon each row is
bool g_arrangeDragging = false;
int g_arrangeDragItem = -1;
int g_arrangeDragList = -1;

int ArrangeListOfId(UINT_PTR id) {
    const int list = static_cast<int>(id) - kArrangeListIdBase;
    return (list >= 0 && list < static_cast<int>(g_arrangeLists.size())) ? list : -1;
}

// The list that shows tray `number`, or -1.
int ArrangeListOfTray(int number) {
    for (size_t i = 0; i < g_arrangeTrays.size(); i++) {
        if (g_arrangeTrays[i] == number) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// What the arrange window's rows are made of, without their pictures and
// labels: which icons there are, the tray each is in, and whether it is in that
// tray's overflow (DECISIONS 75). The window was filled when it opened and
// again only after a move made in it, so an application started or closed
// meanwhile, or an icon moved from a tray's menu, left rows missing or stale.
// Pictures and tooltips are left out: they change several times a second, and
// filling the lists again resets what the user has selected.
std::wstring ArrangeLayoutNow() {
    std::wstring layout;
    std::lock_guard<std::mutex> lock(g_mutex);
    for (const auto& icon : g_primaryOnly) {
        layout += std::to_wstring(icon.serial) + L":1;";
    }
    for (const auto& icon : g_icons) {
        layout += std::to_wstring(icon.serial) + L':' + std::to_wstring(icon.shownTray) +
                  (IsIconHidden(StableKeyOf(icon)) ? L"h;" : L";");
    }
    return layout;
}

// What the lists were last filled with (ArrangeLayoutNow).
std::wstring g_arrangeLayout;

std::wstring ArrangeKeyOfItem(int list, int item);

// A row's text: the icon's name, and whether it is in its tray's overflow.
std::wstring ArrangeRowText(const std::wstring& label, bool inOverflow) {
    return inOverflow ? label + L"  (in the overflow)" : label;
}

// Each icon's row text now, by serial. Caller holds g_mutex.
std::map<uint64_t, std::wstring> ArrangeRowTextsLocked() {
    std::map<uint64_t, std::wstring> texts;
    for (const auto& icon : g_primaryOnly) {
        texts[icon.serial] = ArrangeRowText(IconLabel(icon), false);
    }
    for (const auto& icon : g_icons) {
        const int list = std::max(0, ArrangeListOfTray(icon.shownTray));
        texts[icon.serial] =
            ArrangeRowText(IconLabel(icon), list > 0 && IsIconHidden(StableKeyOf(icon)));
    }
    return texts;
}

// Renames the rows whose icon's name has changed, in place: a tooltip changes
// often, and filling the lists again for it would reset what is selected
// (DECISIONS 83). Their order is left until the lists are next filled.
void UpdateArrangeLabels() {
    std::map<uint64_t, std::wstring> texts;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        texts = ArrangeRowTextsLocked();
    }
    for (HWND list : g_arrangeLists) {
        const int rows = ListView_GetItemCount(list);
        for (int item = 0; item < rows; item++) {
            LVITEMW query = {};
            query.mask = LVIF_PARAM;
            query.iItem = item;
            if (!ListView_GetItem(list, &query) ||
                static_cast<size_t>(query.lParam) >= g_arrangeSerials.size()) {
                continue;
            }
            const auto text = texts.find(g_arrangeSerials[static_cast<size_t>(query.lParam)]);
            if (text == texts.end()) {
                continue;
            }
            wchar_t current[260] = {};
            ListView_GetItemText(list, item, 0, current, ARRAYSIZE(current));
            if (text->second != current) {
                ListView_SetItemText(list, item, 0, const_cast<PWSTR>(text->second.c_str()));
            }
        }
    }
}

void PopulateArrangeLists() {
    if (g_arrangeLists.empty()) {
        return;
    }
    g_arrangeLayout = ArrangeLayoutNow();
    // What is selected in each list stays selected, where it is still there.
    std::vector<std::wstring> selected;
    for (size_t i = 0; i < g_arrangeLists.size(); i++) {
        selected.push_back(ArrangeKeyOfItem(
            static_cast<int>(i),
            ListView_GetNextItem(g_arrangeLists[i], -1, LVNI_SELECTED)));
    }

    struct Row {
        std::wstring key;
        std::wstring label;
        OwnedIcon icon;  // drawn after the lock is released (DECISIONS 60)
        int list;
        bool hidden;
        uint64_t serial;
    };
    std::vector<Row> rows;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        rows.reserve(g_icons.size() + g_primaryOnly.size());
        for (const auto& icon : g_primaryOnly) {
            rows.push_back({StableKeyOf(icon), IconLabel(icon),
                            OwnedIcon::CopyOf(icon.icon), 0, false, icon.serial});
        }
        for (const auto& icon : g_icons) {
            const std::wstring key = StableKeyOf(icon);
            const int list = std::max(0, ArrangeListOfTray(icon.shownTray));
            rows.push_back({key, IconLabel(icon), OwnedIcon::CopyOf(icon.icon), list,
                            list > 0 && IsIconHidden(key), icon.serial});
        }
    }
    // Within a tray, the icons on its bar first, then those in its overflow.
    std::sort(rows.begin(), rows.end(), [](Row const& a, Row const& b) {
        if (a.hidden != b.hidden) {
            return !a.hidden;
        }
        return _wcsicmp(a.label.c_str(), b.label.c_str()) < 0;
    });

    for (HWND list : g_arrangeLists) {
        ListView_DeleteAllItems(list);
    }
    if (g_arrangeImages) {
        ImageList_RemoveAll(g_arrangeImages);
    }
    g_arrangeKeys.clear();
    g_arrangeSerials.clear();

    std::vector<int> counts(g_arrangeLists.size(), 0);
    for (const auto& row : rows) {
        const int imageIndex =
            (g_arrangeImages && row.icon.get())
                ? ImageList_ReplaceIcon(g_arrangeImages, -1, row.icon.get())
                : -1;
        g_arrangeKeys.push_back(row.key);
        g_arrangeSerials.push_back(row.serial);
        const std::wstring text = ArrangeRowText(row.label, row.hidden);

        LVITEMW item = {};
        item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        item.iItem = counts[static_cast<size_t>(row.list)];
        item.pszText = const_cast<PWSTR>(text.c_str());
        item.iImage = imageIndex;
        item.lParam = static_cast<LPARAM>(g_arrangeKeys.size() - 1);
        HWND list = g_arrangeLists[static_cast<size_t>(row.list)];
        const int inserted = ListView_InsertItem(list, &item);
        std::wstring& wasSelected = selected[static_cast<size_t>(row.list)];
        if (inserted >= 0 && !wasSelected.empty() && wasSelected == row.key) {
            ListView_SetItemState(list, inserted, LVIS_SELECTED | LVIS_FOCUSED,
                                  LVIS_SELECTED | LVIS_FOCUSED);
            wasSelected.clear();
        }
        counts[static_cast<size_t>(row.list)]++;
    }

    for (HWND list : g_arrangeLists) {
        ListView_SetColumnWidth(list, 0, LVSCW_AUTOSIZE_USEHEADER);
    }
}

// Which of the lists, if any, the cursor is over.
int ArrangeListUnderCursor() {
    POINT cursor = {};
    GetCursorPos(&cursor);
    for (size_t i = 0; i < g_arrangeLists.size(); i++) {
        RECT rect = {};
        if (g_arrangeLists[i] && GetWindowRect(g_arrangeLists[i], &rect) &&
            PtInRect(&rect, cursor)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::wstring ArrangeKeyOfItem(int list, int item) {
    if (list < 0 || list >= static_cast<int>(g_arrangeLists.size()) || item < 0) {
        return std::wstring();
    }
    LVITEMW query = {};
    query.mask = LVIF_PARAM;
    query.iItem = item;
    if (!ListView_GetItem(g_arrangeLists[static_cast<size_t>(list)], &query)) {
        return std::wstring();
    }
    const size_t index = static_cast<size_t>(query.lParam);
    return index < g_arrangeKeys.size() ? g_arrangeKeys[index] : std::wstring();
}

// Moves an icon to the tray another list shows. Only a change of tray goes
// through MoveIconToTray, which records a placement.
void ArrangeMove(std::wstring key, int fromList, int toList) {
    if (key.empty() || toList < 0 || toList == fromList ||
        toList >= static_cast<int>(g_arrangeTrays.size())) {
        return;
    }
    const int toTray = g_arrangeTrays[static_cast<size_t>(toList)];
    MoveIconToTray(key, Destination::Tray(toTray));
    // An icon sent to the primary tray is not left marked hidden, or it would
    // come back into an overflow unasked the next time it was moved over.
    if (toTray <= 1 && IsIconHidden(key)) {
        SetIconHidden(key, false);
    }
    PopulateArrangeLists();
    RequestRedrawOfAllTrays();
}

// H: into the tray's overflow, or back out onto its bar. The primary tray's
// overflow is Windows' own business.
void ArrangeToggleHidden(int list) {
    if (list <= 0 || list >= static_cast<int>(g_arrangeLists.size())) {
        return;
    }
    const int item =
        ListView_GetNextItem(g_arrangeLists[static_cast<size_t>(list)], -1, LVNI_SELECTED);
    const std::wstring key = ArrangeKeyOfItem(list, item);
    if (key.empty()) {
        return;
    }
    SetIconHidden(key, !IsIconHidden(key));
    PopulateArrangeLists();
    RequestRedrawOfAllTrays();
}

// Double-click and Enter: between the primary tray and tray 2, which is the
// move people make most, and back to the primary tray from any other.
int ArrangeDefaultTarget(int fromList) {
    if (fromList < 0 || fromList >= static_cast<int>(g_arrangeTrays.size())) {
        return -1;
    }
    if (g_arrangeTrays[static_cast<size_t>(fromList)] <= 1) {
        return g_arrangeLists.size() > 1 ? 1 : -1;
    }
    return 0;
}

// Moves the selected row of `list` and keeps a row selected at the same place,
// so a run of icons can be sent across by pressing the same key repeatedly.
void ArrangeMoveSelected(int list, int toList) {
    if (list < 0 || list >= static_cast<int>(g_arrangeLists.size())) {
        return;
    }
    HWND view = g_arrangeLists[static_cast<size_t>(list)];
    const int item = ListView_GetNextItem(view, -1, LVNI_SELECTED);
    if (item < 0) {
        return;
    }
    ArrangeMove(ArrangeKeyOfItem(list, item), list, toList);

    const int remaining = ListView_GetItemCount(view);
    if (remaining > 0) {
        const int next = std::min(item, remaining - 1);
        ListView_SetItemState(view, next, LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(view, next, FALSE);
    }
    SetFocus(view);
}

void EndArrangeDrag(bool drop) {
    if (!g_arrangeDragging) {
        return;
    }
    g_arrangeDragging = false;
    if (GetCapture() == g_arrangeWnd) {
        ReleaseCapture();
    }

    const int target = drop ? ArrangeListUnderCursor() : -1;
    if (target >= 0 && target != g_arrangeDragList) {
        ArrangeMove(ArrangeKeyOfItem(g_arrangeDragList, g_arrangeDragItem),
                    g_arrangeDragList, target);
    }
    g_arrangeDragItem = -1;
    g_arrangeDragList = -1;
    // What changed during the drag was left for its end (RefreshArrangeWindow).
    // Posted: this can run while the window is being destroyed.
    NotifyTrayWindow(WM_ST_REFRESH);
}

LRESULT CALLBACK ArrangeWndProc(HWND hWnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
    switch (msg) {
        case WM_NOTIFY: {
            auto* header = reinterpret_cast<NMHDR*>(lParam);
            const int list = ArrangeListOfId(header->idFrom);
            if (list < 0) {
                break;
            }

            if (header->code == LVN_KEYDOWN) {
                // The fast way: select an icon, press the number of the tray it
                // should go to. Repeating the key walks down the list.
                const WORD key = reinterpret_cast<NMLVKEYDOWN*>(lParam)->wVKey;
                int tray = -1;
                if (key >= L'1' && key <= L'9') {
                    tray = key - L'0';
                } else if (key >= VK_NUMPAD1 && key <= VK_NUMPAD9) {
                    tray = key - VK_NUMPAD0;
                }
                if (tray > 0) {
                    const int target = ArrangeListOfTray(tray);
                    if (target >= 0) {
                        ArrangeMoveSelected(list, target);
                    }
                } else if (key == L'H') {
                    ArrangeToggleHidden(list);
                } else if (key == VK_RETURN || key == VK_SPACE) {
                    ArrangeMoveSelected(list, ArrangeDefaultTarget(list));
                }
                return 0;
            }

            if (header->code == LVN_BEGINDRAG) {
                auto* view = reinterpret_cast<NMLISTVIEW*>(lParam);
                g_arrangeDragList = list;
                g_arrangeDragItem = view->iItem;
                g_arrangeDragging = true;
                SetCapture(hWnd);
                // Deliberately no ImageList drag image. ImageList_DragEnter with
                // a null window locks the screen DC, so a drag that cannot end -
                // and one did, under synthetic input during testing - takes the
                // whole desktop with it (DECISIONS 41). A cursor change says the
                // same thing and cannot wedge anything.
                SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
                return 0;
            }

            if (header->code == NM_DBLCLK) {
                // The same move without the drag: quicker once you know, and it
                // works if the drag is ever awkward.
                auto* activate = reinterpret_cast<NMITEMACTIVATE*>(lParam);
                ArrangeMove(ArrangeKeyOfItem(list, activate->iItem), list,
                            ArrangeDefaultTarget(list));
                return 0;
            }
            break;
        }

        case WM_MOUSEMOVE:
            if (g_arrangeDragging) {
                // The button going up without a WM_LBUTTONUP reaching here is
                // how a drag gets stuck holding capture. Checked every move
                // rather than trusted.
                if (!(GetKeyState(VK_LBUTTON) & 0x8000)) {
                    EndArrangeDrag(true);
                    return 0;
                }
                const int over = ArrangeListUnderCursor();
                SetCursor(LoadCursorW(
                    nullptr, (over >= 0 && over != g_arrangeDragList)
                                 ? IDC_SIZEALL
                                 : IDC_NO));
            }
            return 0;

        case WM_LBUTTONUP:
            EndArrangeDrag(true);
            return 0;

        case WM_CANCELMODE:
        case WM_CAPTURECHANGED:
            EndArrangeDrag(false);
            return 0;

        case WM_SETCURSOR:
            if (g_arrangeDragging) {
                return TRUE;  // the drag owns the cursor
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            EndArrangeDrag(false);
            g_arrangeWnd = nullptr;
            g_arrangeOpen.store(false);
            g_arrangeLists.clear();
            g_arrangeTrays.clear();
            g_arrangeKeys.clear();
            g_arrangeSerials.clear();
            return 0;

        case WM_NCDESTROY:
            // The last message, after the lists have gone: nothing that could
            // still draw with the image list is left.
            if (g_arrangeImages) {
                ImageList_Destroy(g_arrangeImages);
                g_arrangeImages = nullptr;
            }
            break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// What the arrange window keeps beyond its own lifetime, released when the
// tray thread ends.
void ReleaseArrangeResources() {
    if (g_arrangeFont) {
        DeleteObject(g_arrangeFont);
        g_arrangeFont = nullptr;
    }
}

HWND CreateArrangeList(HWND parent, int id, int x, int y, int width,
                       int height) {
    HWND list = CreateWindowExW(
        WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS |
            LVS_SHAREIMAGELISTS,
        x, y, width, height, parent, reinterpret_cast<HMENU>(
                                         static_cast<UINT_PTR>(id)),
        nullptr, nullptr);
    if (!list) {
        return nullptr;
    }
    ListView_SetExtendedListViewStyle(list, LVS_EX_FULLROWSELECT |
                                                LVS_EX_DOUBLEBUFFER);
    LVCOLUMNW column = {};
    column.mask = LVCF_TEXT | LVCF_WIDTH;
    column.pszText = const_cast<PWSTR>(L"Icon");
    column.cx = width - 24;
    ListView_InsertColumn(list, 0, &column);
    return list;
}

void ShowArrangeWindow() {
    if (g_arrangeWnd && IsWindow(g_arrangeWnd)) {
        ShowWindow(g_arrangeWnd, SW_RESTORE);
        SetForegroundWindow(g_arrangeWnd);
        PopulateArrangeLists();
        return;
    }

    INITCOMMONCONTROLSEX controls = {sizeof(controls), ICC_LISTVIEW_CLASSES};
    InitCommonControlsEx(&controls);

    // The class is registered with the tray thread's others (TrayThreadProc).
    const std::vector<int> trays = AvailableTrayNumbers();
    const int lists = static_cast<int>(trays.size());

    constexpr int kMargin = 12;
    constexpr int kListWidth = 250;
    constexpr int kHeight = 460;
    RECT frame = {0, 0, kMargin + lists * (kListWidth + kMargin), kHeight};
    AdjustWindowRectEx(&frame, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE,
                       WS_EX_TOOLWINDOW);
    g_arrangeWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW, kArrangeClassName, L"Split Tray - arrange icons",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
        frame.right - frame.left, frame.bottom - frame.top, nullptr, nullptr,
        ModuleInstance(), nullptr);
    if (!g_arrangeWnd) {
        Wh_Log(L"could not create the arrange window: %lu", GetLastError());
        return;
    }

    RECT client = {};
    GetClientRect(g_arrangeWnd, &client);
    const int labelHeight = 20;
    const int hintHeight = 52;
    const int listTop = kMargin + labelHeight;
    const int listHeight = client.bottom - listTop - kMargin - hintHeight - kMargin / 2;

    g_arrangeTrays = trays;
    g_arrangeLists.assign(trays.size(), nullptr);
    for (int i = 0; i < lists; i++) {
        const int number = trays[static_cast<size_t>(i)];
        std::wstring title = std::to_wstring(number) + L"  ";
        if (number <= 1) {
            title += L"Primary tray";
        } else {
            std::wstring label = TrayLabel(number);
            label[0] = static_cast<wchar_t>(towupper(label[0]));
            title += label;
        }
        const int x = kMargin + i * (kListWidth + kMargin);
        CreateWindowExW(0, L"STATIC", title.c_str(),
                        WS_CHILD | WS_VISIBLE | SS_ENDELLIPSIS, x, kMargin,
                        kListWidth, labelHeight, g_arrangeWnd, nullptr, nullptr,
                        nullptr);
        g_arrangeLists[static_cast<size_t>(i)] = CreateArrangeList(
            g_arrangeWnd, kArrangeListIdBase + i, x, listTop, kListWidth, listHeight);
    }

    CreateWindowExW(0, L"STATIC",
                    L"Drag an icon to another tray, or select it and press that "
                    L"tray's number - hold the key to work down the list. H puts "
                    L"an icon in its tray's overflow or brings it back. "
                    L"Double-click or Enter moves it between the primary tray "
                    L"and tray 2. Every choice is remembered.",
                    WS_CHILD | WS_VISIBLE, kMargin, listTop + listHeight + 6,
                    client.right - kMargin * 2, hintHeight, g_arrangeWnd,
                    nullptr, nullptr, nullptr);

    // Same font the rest of the shell uses; the default is the 1990s one.
    NONCLIENTMETRICSW metrics = {sizeof(metrics)};
    if (!g_arrangeFont && SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,
                                                sizeof(metrics), &metrics, 0)) {
        g_arrangeFont = CreateFontIndirectW(&metrics.lfMessageFont);
    }
    if (g_arrangeFont) {
        EnumChildWindows(
            g_arrangeWnd,
            [](HWND child, LPARAM param) -> BOOL {
                SendMessageW(child, WM_SETFONT, param, TRUE);
                return TRUE;
            },
            reinterpret_cast<LPARAM>(g_arrangeFont));
    }

    g_arrangeImages = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 8, 8);
    for (HWND list : g_arrangeLists) {
        ListView_SetImageList(list, g_arrangeImages, LVSIL_SMALL);
    }

    PopulateArrangeLists();
    g_arrangeOpen.store(true);
    ShowWindow(g_arrangeWnd, SW_SHOW);
    SetForegroundWindow(g_arrangeWnd);
    Wh_Log(L"arrange window opened with %d tray(s)", lists);
}

void CloseArrangeWindow() {
    if (g_arrangeWnd && IsWindow(g_arrangeWnd)) {
        DestroyWindow(g_arrangeWnd);
    }
}

// After a change to the icons or the trays. The lists follow the trays, so a
// window laid out for the old set is replaced. Otherwise they are filled again
// when asked, or when which icons there are or where has changed
// (ArrangeLayoutNow, DECISIONS 75) - not for every change of picture or
// tooltip, which would reset the user's selection several times a second, and
// not in the middle of a drag, whose end catches up (EndArrangeDrag).
void RefreshArrangeWindow(bool repopulate) {
    if (!g_arrangeWnd || !IsWindow(g_arrangeWnd)) {
        return;
    }
    if (AvailableTrayNumbers() != g_arrangeTrays) {
        CloseArrangeWindow();
        ShowArrangeWindow();
    } else if (!g_arrangeDragging) {
        if (repopulate || ArrangeLayoutNow() != g_arrangeLayout) {
            PopulateArrangeLists();
        } else {
            UpdateArrangeLabels();
        }
    }
}

void NotifyTrayWindow(UINT message) {
    HWND hWnd = g_trayWnd.load();
    if (hWnd) {
        PostMessageW(hWnd, message, 0, 0);
    }
}

// Every tray redraws: the floating panels on the tray thread, the embedded
// ones on the taskbar's.
void RequestRedrawOfAllTrays() {
    NotifyTrayWindow(WM_ST_REFRESH);
#ifndef SPLITTRAY_NO_XAML
    SplitTrayXaml::RequestEmbeddedRefresh();
#endif
}

// ============================================================================
// Section 7 - Click forwarding
//
// The notification-area callback protocol, as the real tray implements it.
// Version 0-3 icons get (uID, mouseMessage); version 4 icons get the anchor
// point in wParam and (mouseMessage, uID) in lParam.
// ============================================================================

struct TrayCallback {
    WPARAM wParam = 0;
    LPARAM lParam = 0;
};

// What the real tray posts to an icon's owner for one mouse message, in order
// (DECISIONS 62). Version 4 packs the anchor point into wParam and the message
// and uID into lParam; earlier versions send (uID, message). From version 3 a
// left button-up is followed by NIN_SELECT and a right one by WM_CONTEXTMENU -
// documented for version 4, but Explorer does it for 3 as well, and Cairo's
// ManagedShell, which reimplements the tray, does the same. Applications
// written to the newer protocol act on those, not on the raw buttons.
// One callback, packed as the icon's version has it: version 4 puts the anchor
// point in wParam and the message and uID in lParam, earlier versions send
// (uID, message).
TrayCallback PackTrayCallback(UINT version, UINT uID, UINT message, POINT point) {
    if (version >= NOTIFYICON_VERSION_4) {
        return TrayCallback{MAKEWPARAM(point.x, point.y), MAKELPARAM(message, uID)};
    }
    return TrayCallback{static_cast<WPARAM>(uID), static_cast<LPARAM>(message)};
}

std::vector<TrayCallback> TrayCallbacksFor(UINT version,
                                           UINT uID,
                                           UINT mouseMessage,
                                           POINT screenPoint) {
    std::vector<TrayCallback> callbacks = {
        PackTrayCallback(version, uID, mouseMessage, screenPoint)};
    if (version >= NOTIFYICON_VERSION) {
        if (mouseMessage == WM_LBUTTONUP) {
            callbacks.push_back(PackTrayCallback(version, uID, NIN_SELECT, screenPoint));
        } else if (mouseMessage == WM_RBUTTONUP) {
            callbacks.push_back(PackTrayCallback(version, uID, WM_CONTEXTMENU, screenPoint));
        }
    }
    return callbacks;
}

// What Explorer sends for an icon selected from the keyboard (DECISIONS 80):
// from version 3, NIN_KEYSELECT for Enter or Space and WM_CONTEXTMENU for
// Shift+F10 or the menu key; an older icon gets the clicks those stand for.
std::vector<TrayCallback> KeyboardCallbacksFor(UINT version,
                                               UINT uID,
                                               bool contextMenu,
                                               POINT anchor) {
    if (version >= NOTIFYICON_VERSION) {
        return {PackTrayCallback(version, uID, contextMenu ? WM_CONTEXTMENU : NIN_KEYSELECT,
                                 anchor)};
    }
    const UINT down = contextMenu ? WM_RBUTTONDOWN : WM_LBUTTONDOWN;
    const UINT up = contextMenu ? WM_RBUTTONUP : WM_LBUTTONUP;
    return {PackTrayCallback(version, uID, down, anchor),
            PackTrayCallback(version, uID, up, anchor)};
}

// Where one icon's callbacks go.
struct IconCallbackTarget {
    int index = -1;  // in g_icons, for the log
    HWND ownerWnd = nullptr;
    UINT uID = 0;
    UINT callbackMessage = 0;
    UINT version = 0;
    DWORD recordFlags = 0;
};

// The icon with `serial` in Split Tray's trays, if it is still there and has
// somewhere to send its callbacks.
bool CallbackTargetOf(uint64_t serial, IconCallbackTarget* out) {
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        out->index = IndexOfSerialLocked(serial);
        if (out->index < 0) {
            return false;  // gone since it was drawn
        }
        const auto& icon = g_icons[static_cast<size_t>(out->index)];
        out->ownerWnd = icon.ownerWnd;
        out->uID = icon.uID;
        out->callbackMessage = icon.callbackMessage;
        out->version = icon.version;
        out->recordFlags = RecordFlagsOf(icon);
    }
    return out->callbackMessage && IsWindow(out->ownerWnd);
}

void PostCallbacks(const IconCallbackTarget& target,
                   const std::vector<TrayCallback>& callbacks) {
    for (const TrayCallback& callback : callbacks) {
        PostMessageW(target.ownerWnd, target.callbackMessage, callback.wParam,
                     callback.lParam);
    }
}

// Applications put their context menu up with TrackPopupMenu, which needs the
// owner to be allowed to take the foreground - the real tray does the same.
void LetOwnerTakeForeground(HWND ownerWnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(ownerWnd, &pid);
    if (pid) {
        AllowSetForegroundWindow(pid);
    }
}

void ForwardClick(uint64_t serial, UINT mouseMessage, POINT screenPoint) {
    IconCallbackTarget target;
    if (!CallbackTargetOf(serial, &target)) {
        return;
    }
    LetOwnerTakeForeground(target.ownerWnd);
    PostCallbacks(target,
                  TrayCallbacksFor(target.version, target.uID, mouseMessage, screenPoint));
    Wh_Log(L"forwarded 0x%X to icon %d (hWnd=%p uID=%u v%u)", mouseMessage, target.index,
           target.ownerWnd, target.uID, target.version);
}

// The pointer moving over an icon, as Explorer tells its application - some
// bring their tooltip up to date on it. Not logged: it is every move
// (DECISIONS 44, 79).
void ForwardHover(uint64_t serial, POINT screenPoint) {
    IconCallbackTarget target;
    if (CallbackTargetOf(serial, &target)) {
        PostCallbacks(target, {PackTrayCallback(target.version, target.uID, WM_MOUSEMOVE,
                                                screenPoint)});
    }
}

// NIN_POPUPOPEN once the pointer rests on an icon that draws a popup of its own,
// or NIN_POPUPCLOSE when it leaves (DECISIONS 79). Returns whether the icon
// draws its own; one that does not has the tray's tooltip instead.
bool ForwardPopup(uint64_t serial, bool open, POINT screenPoint) {
    IconCallbackTarget target;
    if (!CallbackTargetOf(serial, &target) ||
        UsesStandardTooltip(target.version, target.recordFlags)) {
        return false;
    }
    if (open) {
        LetOwnerTakeForeground(target.ownerWnd);
    }
    PostCallbacks(target, {PackTrayCallback(target.version, target.uID,
                                            open ? NIN_POPUPOPEN : NIN_POPUPCLOSE,
                                            screenPoint)});
    return true;
}

// An icon selected from the keyboard, or its menu asked for (DECISIONS 80).
void ForwardKey(uint64_t serial, bool contextMenu, POINT anchor) {
    IconCallbackTarget target;
    if (!CallbackTargetOf(serial, &target)) {
        return;
    }
    LetOwnerTakeForeground(target.ownerWnd);
    PostCallbacks(target,
                  KeyboardCallbacksFor(target.version, target.uID, contextMenu, anchor));
    Wh_Log(L"forwarded the keyboard's %s to icon %d (hWnd=%p uID=%u v%u)",
           contextMenu ? L"menu" : L"selection", target.index, target.ownerWnd, target.uID,
           target.version);
}

// ============================================================================
// Section 8 - Interception
// ============================================================================

// Hands a stored payload to Explorer's own window procedure, bypassing our
// handler, and returns what Explorer answered. Runs on the taskbar thread, from
// inside our subclass.
LRESULT DeliverToShell(HWND shellTrayWnd,
                       HWND senderWnd,
                       const std::vector<BYTE>& payload) {
    COPYDATASTRUCT cds = {};
    cds.dwData = kTrayCopyDataId;
    cds.cbData = static_cast<DWORD>(payload.size());
    cds.lpData = const_cast<BYTE*>(payload.data());
    return DefSubclassProc(shellTrayWnd, WM_COPYDATA,
                           reinterpret_cast<WPARAM>(senderWnd),
                           reinterpret_cast<LPARAM>(&cds));
}

// ---------------------------------------------------------------------------
// Settling Explorer's tray (DECISIONS 58, 66, 68, 71)
//
// A move changes where an icon should be (shellTarget) and wakes the taskbar's
// thread, and nothing else. That thread then compares, icon by icon, where it
// should be with where Explorer has it (forwardedToShell), and hands Explorer
// what closes the gap: an add built from the icon as it is at that moment,
// followed by the version its application negotiated, or a delete. What
// Explorer answers is what is recorded.
//
// Moves used to be queued as finished records. A record made when the move was
// asked for missed what arrived while it waited - a new callback, a new
// version, swallowed because Explorer did not have the icon yet - and two moves
// of one icon could be queued in the wrong order, each prepared under the
// store's lock and queued after it was released: an old removal delivered
// after a newer add took the icon out of Explorer for good. Reading the store
// when the thread gets to it leaves no order to get wrong and nothing to go
// stale. The wake-up carries nothing, because any process on the desktop can
// post it.
//
// In between, an application's own messages are passed on by where the icon
// really is. One Explorer has not been handed yet swallows them, and they are
// in the add when it goes. So are ones that arrive while Explorer is taking it
// - Explorer may send messages of its own while it handles a record, and they
// come back through the subclass - by a second record, the whole icon again as
// a modify (shellBehind).
// ---------------------------------------------------------------------------

// How many times Explorer is asked to take an icon back, or its version,
// before it is left as Explorer has it (OwedToShell).
constexpr int kShellAttempts = 3;

void RecordShellHoldingLocked(MirroredIcon* icon, bool holds);

// The icon with this serial in either list, or null. Caller holds g_mutex.
MirroredIcon* FindIconBySerialLocked(uint64_t serial) {
    for (auto* list : {&g_icons, &g_primaryOnly}) {
        for (auto& icon : *list) {
            if (icon.serial == serial) {
                return &icon;
            }
        }
    }
    return nullptr;
}

// Whether the taskbar's thread has something to hand Explorer for this icon.
// Caller holds g_mutex.
bool NeedsSettlingLocked(const MirroredIcon& icon) {
    if (icon.payload.empty()) {
        return false;
    }
    if (!icon.shellTarget) {
        // Always taken out: a ghost left in Explorer's tray costs more than a
        // refused call.
        return icon.forwardedToShell;
    }
    if (icon.shellUnconfirmed) {
        return true;
    }
    if (icon.forwardedToShell && !icon.shellBehind) {
        return false;
    }
    return icon.shellRefusals < kShellAttempts;
}

// An icon meant to be in Explorer's tray that Explorer would not take back.
// Its application's own adds and modifies go to Explorer, as they would with
// no mod at all: a modify for an icon Explorer does not have fails, and an
// application that recovers from that adds its icon again, which Explorer
// takes (RecordShellAnswerLocked).
bool OwedToShell(const MirroredIcon& icon) {
    return icon.shellTarget && !icon.forwardedToShell &&
           icon.shellRefusals >= kShellAttempts;
}

// What the taskbar's thread hands Explorer for one icon.
enum class ShellChange {
    Add,      // Explorer does not have it
    Update,   // Explorer has less of it than the store (shellBehind)
    Delete,   // Explorer has it and should not
    Confirm,  // whether Explorer has it, after refusing its add (shellUnconfirmed)
};

struct ShellDelivery {
    uint64_t serial = 0;
    HWND senderWnd = nullptr;
    ShellChange change = ShellChange::Delete;
    std::vector<BYTE> record;         // the add, the icon as a modify, or the delete
    std::vector<BYTE> versionRecord;  // after an add or update, when there is a version
    uint64_t revision = 0;            // the icon's, when the record was built
    // A copy of the icon's picture for Explorer to copy in turn, released once
    // the record has been delivered.
    OwnedIcon picture;
    bool pictureLost = false;  // the store had one, and copying it failed
    std::wstring exe;          // whose it is, for the log
};

// The next icon not in `skip` whose place in Explorer's tray is not where it
// should be, and what to hand Explorer for it. Caller holds g_mutex.
//
// An add is built from the folded record and drawn with a fresh copy of the
// mod's own picture, or with none when that cannot be copied (DECISIONS 72).
// It is followed by the version the application negotiated, since a re-added
// icon starts again at version 0. An update is the same, as a modify.
bool NextShellDeliveryLocked(const std::vector<uint64_t>& skip, ShellDelivery* out) {
    for (auto* list : {&g_icons, &g_primaryOnly}) {
        for (const auto& icon : *list) {
            if (!NeedsSettlingLocked(icon) ||
                std::find(skip.begin(), skip.end(), icon.serial) != skip.end()) {
                continue;
            }
            out->serial = icon.serial;
            out->senderWnd = icon.ownerWnd;
            out->revision = icon.revision;
            out->exe = std::wstring(FileNameOf(icon.exePath));
            if (!icon.shellTarget) {
                out->change = ShellChange::Delete;
                out->record = PayloadWithMessage(icon.payload, NIM_DELETE);
                return true;
            }
            if (icon.shellUnconfirmed) {
                out->change = ShellChange::Confirm;
                out->record = ProbeRecordFor(icon.payload);
                return true;
            }
            // Explorer refuses to add an icon it holds, hidden or not. One it
            // holds hidden is shown by the whole icon as a modify, which takes
            // NIS_HIDDEN off - unless its application hid it (DECISIONS 78).
            out->change = icon.forwardedToShell || icon.shellHidden ? ShellChange::Update
                                                                    : ShellChange::Add;
            out->picture = OwnedIcon::CopyOf(icon.icon);
            out->pictureLost = icon.icon && !out->picture.get();
            out->record = AddRecordFor(icon.payload, out->picture.get());
            if (out->change == ShellChange::Update) {
                out->record = PayloadWithMessage(out->record, NIM_MODIFY);
            }
            if (icon.shellHidden) {
                out->record = WithHiddenState(out->record, (icon.state & NIS_HIDDEN) != 0);
            }
            if (icon.version) {
                out->versionRecord = SetVersionRecordFor(icon.payload, icon.version);
            }
            return true;
        }
    }
    return false;
}

// What became of one delivery.
enum class ShellOutcome {
    Settled,   // Explorer has the icon, or has let it go, as it should
    Refused,   // Explorer would not take it back, or its version; asked again later
    GaveUp,    // ...kShellAttempts times, so it is left as Explorer has it
    Orphaned,  // taken back, for an icon removed while it was being handed over
};

// Records what Explorer did with `delivery`: whether it took the record, and
// the version after it. Caller holds g_mutex, on the taskbar's thread.
ShellOutcome RecordShellDeliveryLocked(const ShellDelivery& delivery,
                                       bool taken,
                                       bool versionTaken) {
    MirroredIcon* icon = FindIconBySerialLocked(delivery.serial);
    if (delivery.change == ShellChange::Delete) {
        // Explorer does not have it now, whatever it answered.
        if (icon) {
            icon->forwardedToShell = false;
            icon->shellBehind = false;
            icon->shellUnconfirmed = false;
            icon->shellHidden = false;
        }
        return ShellOutcome::Settled;
    }
    if (delivery.change == ShellChange::Confirm) {
        // Taken, Explorer has it; refused, it has not, and the icon is left
        // to its application, which was told its add failed (DECISIONS 70).
        if (icon) {
            icon->shellUnconfirmed = false;
            RecordShellHoldingLocked(icon, taken);
        }
        return ShellOutcome::Settled;
    }
    if (!icon) {
        return taken && delivery.change == ShellChange::Add ? ShellOutcome::Orphaned
                                                            : ShellOutcome::Settled;
    }
    // A hidden copy taken is shown now; refused, Explorer does not hold one -
    // it restarted - and the icon is added afresh next time (DECISIONS 78).
    icon->shellHidden = false;
    if (taken) {
        icon->forwardedToShell = true;
        // What arrived meanwhile was swallowed - Explorer did not have the
        // icon yet - and a version Explorer did not take is not its version.
        icon->shellBehind = icon->revision != delivery.revision || !versionTaken;
        if (versionTaken) {
            if (!icon->shellBehind) {
                icon->shellRefusals = 0;
            }
            return ShellOutcome::Settled;
        }
    } else {
        // An add refused, or an update for an icon Explorer turns out not to
        // have: either way, it does not have it.
        icon->forwardedToShell = false;
        icon->shellBehind = false;
    }
    icon->shellRefusals++;
    return icon->shellRefusals >= kShellAttempts ? ShellOutcome::GaveUp
                                                 : ShellOutcome::Refused;
}

// Set while SettleShellIcons runs. Taskbar's thread only.
bool g_settlingShell = false;

// Settles every icon whose place in Explorer's tray is not where it should be,
// through `deliver`, which hands Explorer a record and returns its answer. On
// the taskbar's thread; the tests pass a stand-in for Explorer. Returns whether
// it handed Explorer anything.
//
// Each icon is tried once per call. One Explorer refused is tried again on a
// later call - the tray thread's timer makes one every tick while anything is
// waiting - and after kShellAttempts is left as Explorer has it.
//
// Never inside itself. Explorer may run a message loop while it handles a
// record, and a wake-up posted meanwhile is dispatched from inside it: a round
// started there handed Explorer again what the round under way was handing it.
// What that wake-up was for is left to the round under way, or the next one.
template <typename Deliver>
bool SettleShellIcons(Deliver deliver) {
    if (g_settlingShell) {
        return false;
    }
    g_settlingShell = true;
    bool handedOver = false;
    std::vector<uint64_t> tried;
    for (;;) {
        ShellDelivery delivery;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            if (!NextShellDeliveryLocked(tried, &delivery)) {
                break;
            }
        }
        tried.push_back(delivery.serial);
        handedOver = true;

        // Outside the lock: Explorer may send messages of its own while it
        // handles this one, and they come back through the subclass.
        bool taken = deliver(delivery.senderWnd, delivery.record) != FALSE;
        if (delivery.change == ShellChange::Add && !taken) {
            // Explorer refuses an add for an icon it has already. Asked to
            // update it instead, it says which it was.
            taken = deliver(delivery.senderWnd,
                            PayloadWithMessage(delivery.record, NIM_MODIFY)) != FALSE;
        }
        bool versionTaken = true;
        if ((delivery.change == ShellChange::Add || delivery.change == ShellChange::Update) &&
            taken && !delivery.versionRecord.empty()) {
            versionTaken = deliver(delivery.senderWnd, delivery.versionRecord) != FALSE;
        }

        ShellOutcome outcome;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            outcome = RecordShellDeliveryLocked(delivery, taken, versionTaken);
        }
        const DWORD uID = ReadDword(delivery.record.data(), wire::kUID);
        const wchar_t* exe = delivery.exe.empty() ? L"?" : delivery.exe.c_str();
        switch (outcome) {
            case ShellOutcome::Settled:
                break;
            case ShellOutcome::Refused:
                if (taken) {
                    Wh_Log(L"Explorer took back icon uID %u (%s) but not its version; "
                           L"asking again later",
                           uID, exe);
                } else {
                    Wh_Log(L"Explorer did not take back icon uID %u (%s); asking again "
                           L"later",
                           uID, exe);
                }
                break;
            case ShellOutcome::GaveUp:
                if (taken) {
                    Wh_Log(L"Explorer would not take the version of icon uID %u (%s); "
                           L"it is left at the one Explorer gives it",
                           uID, exe);
                } else {
                    Wh_Log(L"Explorer would not take back icon uID %u (%s); it is left "
                           L"to its application to add again",
                           uID, exe);
                }
                break;
            case ShellOutcome::Orphaned:
                // Its application removed it while Explorer was taking it.
                deliver(delivery.senderWnd,
                        PayloadWithMessage(delivery.record, NIM_DELETE));
                break;
        }
        if (delivery.pictureLost) {
            Wh_Log(L"the picture of icon uID %u (%s) could not be copied; Explorer "
                   L"was handed it without one",
                   uID, exe);
        }
    }
    g_settlingShell = false;
    return handedOver;
}

// As the mod unloads, on the taskbar's thread: every icon whose application is
// still there goes back to Explorer as it is by then, and the subclass stops
// keeping track, in one go (DECISIONS 68). Keeping track used to stop as soon
// as unloading began, while the mod's thread was still being stopped, and an
// application's messages then went to an Explorer that did not have the icon:
// one removed meanwhile was put back by the hand-back, and one changed
// meanwhile was put back as it had been. A message handled between a
// hand-back and the subclass letting go would be swallowed into a tray that is
// gone.
//
// What arrives while it runs is handed back by another round: a new icon,
// swallowed into a tray that is going, or a change to one Explorer was taking.
// A hand-back that arrives inside a round of settling waits for that round to
// end (OnShellTrayWake).
//
// Only icons the mod took away are handed back. One meant for Explorer all
// along that Explorer has already refused is left to its application, as
// before: found live, Explorer's own icons - its volume icon among them -
// register again when the mod is loaded into a running Explorer, and Explorer
// refuses them; handing them back meant three more refusals each, every time
// the mod unloaded.
template <typename Deliver>
void HandIconsBackToShell(Deliver deliver) {
    if (g_handedBack.load() || g_settlingShell) {
        return;
    }
    for (int round = 0; round < kShellAttempts; round++) {
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            for (auto* list : {&g_icons, &g_primaryOnly}) {
                for (auto& icon : *list) {
                    // An icon whose application has gone is left as it is.
                    const bool target =
                        IsWindow(icon.ownerWnd) ? true : icon.forwardedToShell;
                    if (target != icon.shellTarget) {
                        icon.shellTarget = target;
                        icon.shellRefusals = 0;
                    }
                }
            }
        }
        if (!SettleShellIcons(deliver)) {
            break;
        }
    }
    g_handedBack.store(true);
}

// What a wake-up does, on the taskbar's thread: settles what is waiting, or,
// once the mod is unloading, hands every icon back. Both after a round that
// unloading began in the middle of, since the hand-back may have been
// dispatched from inside it and waited (DECISIONS 68).
template <typename Deliver>
void OnShellTrayWake(Deliver deliver) {
    if (!g_unloading.load()) {
        SettleShellIcons(deliver);
    }
    if (g_unloading.load()) {
        HandIconsBackToShell(deliver);
    }
}

// Wakes the taskbar's thread to settle what is waiting. Posting fails only when
// the window has gone or its queue is full; the tray thread's timer wakes it
// again while anything is waiting (ReplayRoutingChanges).
void WakeReplayDelivery() {
    HWND shellTrayWnd = g_shellTrayWnd.load();
    if (shellTrayWnd && IsWindow(shellTrayWnd)) {
        PostMessageW(shellTrayWnd, GetReplayMessage(), 0, 0);
    }
}

// Whether Explorer has an icon, as its answer to its application - or to the
// mod asking on its behalf - says (DECISIONS 70). One Explorer does not have is
// left to its application, which has been told so, rather than handed to
// Explorer by the mod: its record may be made of modifies alone, which would
// recreate the icon without most of it (DECISIONS 49). Caller holds g_mutex.
void RecordShellHoldingLocked(MirroredIcon* icon, bool holds) {
    if (!holds) {
        icon->forwardedToShell = false;
        icon->shellBehind = false;
        icon->shellRefusals = kShellAttempts;
    } else if (!icon->forwardedToShell) {
        icon->forwardedToShell = true;
        icon->shellBehind = false;
        icon->shellRefusals = 0;
    }
}

// What Explorer answered an application's own add or modify, passed on to it.
// Returns whether the taskbar's thread has to ask Explorer about the icon.
//
// A refused add says nothing yet: Explorer refuses to add an icon it has
// already, which is every application's add when the mod is loaded into a
// running Explorer. It is asked with a modify on the taskbar's next round, not
// here: every application registers again at once then, shell32 gives up on a
// tray that keeps it waiting, and an application that does not retry - Tauri's
// tray library does not - loses its icon (DECISIONS 51). Asked here, every
// refused add made two calls into Explorer in that burst instead of one; live,
// with the question asked here, a log listener attached and the processor
// busy, Telemachus's icon was lost in two loads of six. Caller holds g_mutex,
// on the taskbar's thread.
bool RecordShellAnswerLocked(const TrayNotification& n, bool taken) {
    for (auto* list : {&g_icons, &g_primaryOnly}) {
        for (auto& icon : *list) {
            if (!SameIcon(icon, n)) {
                continue;
            }
            if (!taken && n.message == NIM_ADD) {
                icon.shellUnconfirmed = true;
                return true;
            }
            // An add Explorer takes is a new icon there, at version 0; one it
            // refused because it has the icon already, above, leaves the
            // version as it was (DECISIONS 74).
            if (n.message == NIM_ADD) {
                icon.version = 0;
            }
            icon.shellUnconfirmed = false;
            RecordShellHoldingLocked(&icon, taken);
            return false;
        }
    }
    return false;
}

// Shows a balloon for an icon Explorer does not show - one that lives only in
// Split Tray's trays - through a hidden copy of it that Explorer holds
// (DECISIONS 78). Windows shows a balloon, as a notification, only for an icon
// Explorer holds, so it was swallowed with the rest of its message. Measured
// against Explorer: a hidden icon's balloon reaches the notification history,
// and its application gets NIN_BALLOONSHOW, as for one Explorer shows.
//
// The first balloon makes the copy: the whole icon, hidden, then its version,
// by which Explorer packs the balloon's callbacks. Each balloon then goes on
// the copy with the icon as the store has it by then, after the version, which
// is not passed on when its application sends it: refused by an Explorer that
// has lost the copy, it would tell the application its tray cannot take that
// version. A copy lost with an Explorer that restarted refuses the balloon,
// and is made again. Returns whether Explorer took the balloon. On the
// taskbar's thread, inside its application's message; the tests pass a
// stand-in for Explorer.
template <typename Deliver>
bool ShowBalloonThroughShell(const TrayNotification& n,
                             const std::vector<BYTE>& message,
                             Deliver deliver) {
    uint64_t serial = 0;
    HWND senderWnd = nullptr;
    bool hidden = false;
    OwnedIcon picture;  // for Explorer to copy, released once it has
    std::vector<BYTE> add;
    std::vector<BYTE> version;
    std::vector<BYTE> balloon;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        const MirroredIcon* icon = nullptr;
        for (auto* list : {&g_icons, &g_primaryOnly}) {
            for (const auto& candidate : *list) {
                if (!icon && SameIcon(candidate, n)) {
                    icon = &candidate;
                }
            }
        }
        if (!icon || icon->forwardedToShell || icon->payload.empty()) {
            return false;  // Explorer shows it, or nobody has it
        }
        serial = icon->serial;
        senderWnd = icon->ownerWnd;
        hidden = icon->shellHidden;
        picture = OwnedIcon::CopyOf(icon->icon);
        add = HiddenCopyRecordFor(icon->payload, picture.get(), NIM_ADD);
        balloon = BalloonRecordFor(
            HiddenCopyRecordFor(icon->payload, picture.get(), NIM_MODIFY), message);
        if (icon->version) {
            version = SetVersionRecordFor(icon->payload, icon->version);
        }
    }

    auto show = [&](bool makeCopy) {
        if (makeCopy) {
            // Refused when Explorer holds a copy already, left by an earlier
            // load of the mod: the balloon goes on that one.
            deliver(senderWnd, add);
        }
        if (!version.empty()) {
            deliver(senderWnd, version);
        }
        return deliver(senderWnd, balloon) != FALSE;
    };
    bool shown = show(!hidden);
    if (!shown && hidden) {
        shown = show(true);
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    if (MirroredIcon* icon = FindIconBySerialLocked(serial); icon && !icon->forwardedToShell) {
        icon->shellHidden = shown;
    }
    return shown;
}

// Puts one icon where its destination says, given which trays exist now: into
// Explorer's tray or out of it, and into one of Split Tray's trays or none.
// Returns whether the taskbar's thread has anything to hand Explorer for it.
// Caller holds g_mutex, and re-splits the lists afterwards (ResplitStoreLocked).
bool ReconcileIconLocked(MirroredIcon& icon) {
    const RoutingPlan plan =
        PlanFor(icon.destination, TrayAvailableLocked(icon.destination.tray));
    if (plan.forwardToShell != icon.shellTarget && !icon.payload.empty()) {
        icon.shellTarget = plan.forwardToShell;
        icon.shellRefusals = 0;
    }
    // An icon its application hid (NIS_HIDDEN) stays out of Split Tray's trays
    // when the settings say to respect that, as it does on arrival.
    const bool appHidden = (icon.state & NIS_HIDDEN) != 0;
    const bool mirror = plan.mirror && !(appHidden && !g_settings.mirrorHiddenIcons);
    icon.shownTray = mirror ? plan.tray : 0;
    return NeedsSettlingLocked(icon);
}

// Moves icons between g_icons and g_primaryOnly to match their shownTray,
// keeping each list's order. Caller holds g_mutex.
void ResplitStoreLocked() {
    std::vector<MirroredIcon> shown;
    std::vector<MirroredIcon> primaryOnly;
    shown.reserve(g_icons.size() + g_primaryOnly.size());
    for (auto* list : {&g_icons, &g_primaryOnly}) {
        for (auto& icon : *list) {
            (icon.shownTray > 0 ? shown : primaryOnly).push_back(std::move(icon));
        }
    }
    g_icons = std::move(shown);
    g_primaryOnly = std::move(primaryOnly);
}

// Moves icons between the trays after the monitor set changed, and forgets
// icons whose application has gone. Runs on the tray thread, every tick.
void ReplayRoutingChanges() {
    if (g_unloading.load()) {
        return;
    }

    bool settle = false;
    bool changed = false;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::vector<int> wasAvailable;
        for (const auto& tray : g_trays) {
            wasAvailable.push_back(tray.available ? tray.number : -tray.number);
        }
        RecomputeGeometryLocked();
        std::vector<int> nowAvailable;
        for (const auto& tray : g_trays) {
            nowAvailable.push_back(tray.available ? tray.number : -tray.number);
        }

        // Drop icons whose owner has gone away.
        for (auto* list : {&g_icons, &g_primaryOnly}) {
            for (size_t i = list->size(); i-- > 0;) {
                if (!IsWindow((*list)[i].ownerWnd)) {
                    if ((*list)[i].icon) {
                        DestroyIcon((*list)[i].icon);
                    }
                    list->erase(list->begin() + static_cast<ptrdiff_t>(i));
                    changed = true;
                }
            }
        }

        for (auto* list : {&g_icons, &g_primaryOnly}) {
            for (auto& icon : *list) {
                const int before = icon.shownTray;
                settle = ReconcileIconLocked(icon) || settle;
                changed = changed || icon.shownTray != before;
            }
        }
        ResplitStoreLocked();

        if (wasAvailable != nowAvailable) {
            changed = true;
            Wh_Log(L"trays changed: %zu of Split Tray's tray(s) now, %zu shown",
                   g_trays.size(),
                   static_cast<size_t>(std::count_if(
                       g_trays.begin(), g_trays.end(),
                       [](const TrayTarget& tray) { return tray.available; })));
        }
        if (changed) {
            RecomputeGeometryLocked();
        }
    }

    if (settle) {
        WakeReplayDelivery();
    }
    if (changed) {
        // Among them icons whose application went without removing them,
        // which no message says (DECISIONS 75).
        NotifyTrayWindow(WM_ST_REFRESH);
#ifndef SPLITTRAY_NO_XAML
        SplitTrayXaml::RequestEmbeddedRefresh();
#endif
    }
}

// Moves a single icon to a tray and remembers it. Called from the trays' own
// menus, the arrange window and dragging.
//
// This is the same work ApplySettingsToTrackedIcons does for every icon at once:
// the routing decision is sticky, so moving an icon means replaying a stored
// payload into the shell as an add or retracting it as a delete, not waiting for
// the application to touch its icon again. Between two of Split Tray's trays
// nothing goes to the shell at all.
void MoveIconToTray(std::wstring_view key, Destination destination) {
    bool settle = false;
    bool found = false;
    std::wstring label;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        RememberPlacement(key, destination);
        label = TrayLabelLocked(destination.tray);

        for (auto* list : {&g_icons, &g_primaryOnly}) {
            for (auto& icon : *list) {
                if (StableKeyOf(icon) != key) {
                    continue;
                }
                found = true;
                icon.destination = destination;
                icon.destinationDecided = true;
                settle = ReconcileIconLocked(icon) || settle;
            }
        }
        // The icon copy is kept either way: the arrange window lists icons in
        // every tray and needs something to draw.
        ResplitStoreLocked();
        RecomputeGeometryLocked();
    }

    if (settle) {
        WakeReplayDelivery();
    }

    Wh_Log(L"moved icon '%s' to %s%s", std::wstring(key).c_str(), label.c_str(),
           found ? L"" : L" (not currently present)");

    RequestRedrawOfAllTrays();
}

// Re-evaluates every tracked icon against freshly loaded settings, moving it
// between the lists and between the trays as needed. Runs on the tray thread.
void ApplySettingsToTrackedIcons() {
    bool settle = false;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        RecomputeGeometryLocked();

        for (auto* list : {&g_icons, &g_primaryOnly}) {
            for (auto& icon : *list) {
                // Placement first, not just the rules: a settings change used to
                // re-resolve straight from ResolveDestination, which threw away
                // every icon the user had moved by hand (DECISIONS 29). An icon
                // whose path is still unknown keeps its provisional tray.
                if (!icon.exePath.empty()) {
                    icon.destination =
                        ResolvePlacement(g_settings, StableKeyOf(icon), icon.exePath);
                    icon.destinationDecided = true;
                }
                settle = ReconcileIconLocked(icon) || settle;
            }
        }
        ResplitStoreLocked();
        RecomputeGeometryLocked();
    }

    if (settle) {
        WakeReplayDelivery();
    }
}

// Takes the mod's own copy of a picture an application sent: the application
// is free to destroy its own once the shell has answered. One that cannot be
// copied leaves the picture the icon had - it used to be let go first, which
// left the icon with none at all (DECISIONS 72). Caller holds g_mutex.
void TakePictureLocked(MirroredIcon* icon, HICON sent) {
    HICON copy = nullptr;
    if (sent) {
        copy = CopyIcon(sent);
        if (!copy) {
            return;
        }
    }
    if (icon->icon) {
        DestroyIcon(icon->icon);
    }
    icon->icon = copy;
    icon->pictureRevision++;
}

// An icon seen (DECISIONS 90): stamped when it arrives, and every icon the
// store has on the first message of each day, so one that stays for months is
// not taken for gone. On the taskbar's thread, for every message, so it is one
// comparison, and a write at most once a day for each icon. Caller holds
// g_mutex.
void NoteIconSeenLocked(const TrayNotification& n, bool isNew) {
    const int today = g_currentDay();
    LoadLastSeenLocked();
    bool changed = false;
    if (today != g_lastSeenSweepDay) {
        g_lastSeenSweepDay = today;
        for (const auto& icon : g_icons) {
            changed |= StampSeenLocked(StableKeyOf(icon), today);
        }
        for (const auto& icon : g_primaryOnly) {
            changed |= StampSeenLocked(StableKeyOf(icon), today);
        }
    }
    if (isNew) {
        changed |= StampSeenLocked(StableKeyOf(n), today);
    }
    if (changed) {
        SaveLastSeenLocked();
    }
}

// Updates the store from a parsed notification. Caller must hold g_mutex.
// Returns true if the secondary tray needs repainting. `outRetractFromShell`
// is set for an icon new to the mod that goes to one of its trays: Explorer
// may have it already, and is to be told to let it go (DECISIONS 64).
// `outRecordShellAnswer` is set for an add or modify passed on to Explorer,
// whose answer says whether it has the icon (RecordShellAnswerLocked).
bool ApplyNotificationLocked(const TrayNotification& n,
                             const std::vector<BYTE>& payload,
                             bool* outForwardToShell,
                             bool* outRetractFromShell = nullptr,
                             bool* outRecordShellAnswer = nullptr) {
    if (outRetractFromShell) {
        *outRetractFromShell = false;
    }
    if (outRecordShellAnswer) {
        *outRecordShellAnswer = false;
    }
    // Find the icon in whichever list it currently lives in.
    MirroredIcon* existing = nullptr;
    bool existingIsMirrored = false;
    for (auto& icon : g_icons) {
        if (SameIcon(icon, n)) {
            existing = &icon;
            existingIsMirrored = true;
            break;
        }
    }
    if (!existing) {
        for (auto& icon : g_primaryOnly) {
            if (SameIcon(icon, n)) {
                existing = &icon;
                break;
            }
        }
    }

    if (n.message == NIM_DELETE) {
        if (!existing) {
            *outForwardToShell = true;
            return false;
        }
        *outForwardToShell = existing->forwardedToShell;
        // A hidden copy goes too, and the notifications it showed with it, as
        // they go with an icon Explorer shows. Explorer is told to let it go,
        // and the application is answered as for the icon it knows: one lost
        // with an Explorer that restarted would fail its delete (DECISIONS 78).
        if (outRetractFromShell) {
            *outRetractFromShell = existing->shellHidden;
        }
        if (existing->icon) {
            DestroyIcon(existing->icon);
            existing->icon = nullptr;
        }
        if (existingIsMirrored) {
            g_icons.erase(g_icons.begin() +
                          (existing - g_icons.data()));
            return true;
        }
        g_primaryOnly.erase(g_primaryOnly.begin() +
                            (existing - g_primaryOnly.data()));
        return false;
    }

    if (n.message == NIM_SETVERSION) {
        // Recorded either way, because it is replayed after the icon is next
        // added to the shell. Passed on only to a shell that has the icon: one
        // that was never given it fails the call, and the application is then
        // told its tray does not support the version it asked for.
        if (existing) {
            existing->version = n.version;
            existing->revision++;
            *outForwardToShell = existing->forwardedToShell;
        } else {
            *outForwardToShell = true;
        }
        return false;
    }

    if (n.message == NIM_SETFOCUS) {
        // An application gives the keyboard focus back to its icon once its
        // menu closes. One Explorer does not show is focused in the tray that
        // shows it, by the subclass; passing it on failed (DECISIONS 80).
        *outForwardToShell = !(existing && existingIsMirrored && !existing->forwardedToShell);
        return false;
    }

    if (n.message != NIM_ADD && n.message != NIM_MODIFY) {
        *outForwardToShell = true;  // anything unknown
        return false;
    }

    NoteIconSeenLocked(n, existing == nullptr);

    // An icon known by its GUID can come back from a new window, when its
    // application restarts, and with a new uID. The folded record takes both
    // from every message (FoldTrayRecord), and the icon has to agree with it:
    // its owner is where clicks go, and the window whose end the watchdog takes
    // for the icon's. For any other icon these are what identified it anyway.
    if (existing) {
        existing->ownerWnd = n.ownerWnd;
        existing->uID = n.uID;
    }

    // Decide the destination once, when the icon first appears.
    Destination destination;
    bool forwardToShell;
    bool mirror;
    int mirrorTray;
    const bool wasSticky = (existing != nullptr);
    if (existing) {
        // An earlier guess made without an executable path is not a decision.
        // The first message that brings one settles it, and the move itself goes
        // through the settings replay rather than being done mid-message.
        if (!existing->destinationDecided && !n.exePath.empty()) {
            existing->exePath = n.exePath;
            existing->destination =
                ResolvePlacement(g_settings, StableKeyOf(*existing), n.exePath);
            existing->destinationDecided = true;
            g_routingNeedsReapply.store(true);
            Wh_Log(L"decide: uID=%u settled as %s once the path arrived (%s)",
                   n.uID, DestinationName(existing->destination).c_str(),
                   FileNameOf(n.exePath).empty()
                       ? L"?"
                       : std::wstring(FileNameOf(n.exePath)).c_str());
        }
        destination = existing->destination;
        const RoutingPlan plan =
            PlanFor(destination, TrayAvailableLocked(destination.tray));
        // By where the icon really is (DECISIONS 58) - or to Explorer when
        // Explorer would not take it back, and only its application can put it
        // there now (OwedToShell, DECISIONS 66).
        const bool owed = OwedToShell(*existing);
        forwardToShell = existing->forwardedToShell || owed;
        mirror = plan.mirror;
        mirrorTray = plan.tray;
    } else {
        // Remembered placement first: the rules only decide what the user has
        // never moved by hand.
        destination = ResolvePlacement(g_settings, StableKeyOf(n), n.exePath);
        const RoutingPlan plan =
            PlanFor(destination, TrayAvailableLocked(destination.tray));
        forwardToShell = plan.forwardToShell;
        mirror = plan.mirror;
        mirrorTray = plan.tray;
    }
    *outForwardToShell = forwardToShell;
    // Whatever Explorer answers is recorded, not only for an icon it had
    // refused: an application's own add it refused was recorded as there
    // (DECISIONS 70).
    if (outRecordShellAnswer) {
        *outRecordShellAnswer = forwardToShell;
    }
    // An add the mod answers itself is taken, and is a registration afresh:
    // version 0 until its application asks for another. An icon re-registered
    // by GUID from a new window kept the version the old one had asked for.
    // One passed on to Explorer is decided by Explorer's answer
    // (RecordShellAnswerLocked) (DECISIONS 74).
    if (existing && n.message == NIM_ADD && !forwardToShell) {
        existing->version = 0;
    }

    // Why this icon went where it did. A sticky decision and a fresh one that
    // happened to agree are indistinguishable from the outcome alone, and that
    // is the difference this is being used to find.
    if (n.message == NIM_ADD) {
        Wh_Log(L"decide: uID=%u hwnd=%p guid=%s path='%s' -> %s (%s)", n.uID,
               n.ownerWnd,
               n.hasGuid ? FormatGuidKey(n.guid).c_str() : L"(none)",
               n.exePath.c_str(), DestinationName(destination).c_str(),
               wasSticky ? L"sticky, from the existing entry" : L"fresh");
    }

    // Whether the icon is hidden once this message is applied - not whether
    // this message says so. A modify that leaves the state out, a new tooltip,
    // said nothing about it and was read as "not hidden", which put an icon its
    // application had hidden back in the tray.
    DWORD state = existing ? existing->state : 0;
    if (n.flags & NIF_STATE) {
        state = (state & ~n.stateMask) | (n.state & n.stateMask);
    }
    const bool hidden = (state & NIS_HIDDEN) != 0;
    if (mirror && hidden && !g_settings.mirrorHiddenIcons) {
        mirror = false;
    }

    if (!mirror) {
        // Track it anyway, so routing stays sticky and settings changes can move
        // it later without waiting for the application to touch its icon.
        if (existingIsMirrored && existing) {
            MirroredIcon moved = std::move(*existing);
            g_icons.erase(g_icons.begin() + (existing - g_icons.data()));
            g_primaryOnly.push_back(std::move(moved));
            existing = &g_primaryOnly.back();
        } else if (!existing) {
            MirroredIcon icon;
            icon.serial = g_nextSerial.fetch_add(1);
            icon.hasGuid = n.hasGuid;
            icon.guid = n.guid;
            icon.ownerWnd = n.ownerWnd;
            icon.uID = n.uID;
            icon.exePath = n.exePath;
            icon.destination = destination;
            icon.forwardedToShell = forwardToShell;
            icon.shellTarget = forwardToShell;
            icon.destinationDecided = !n.exePath.empty();
            g_primaryOnly.push_back(std::move(icon));
            if (outRetractFromShell) {
                *outRetractFromShell = !forwardToShell;
            }
            existing = &g_primaryOnly.back();
        }
        existing->shownTray = 0;
        FoldTrayRecord(&existing->payload, payload);
        existing->revision++;
        if (n.flags & NIF_MESSAGE) {
            existing->callbackMessage = n.callbackMessage;
        }
        // Kept here too, so a later reconcile knows the application hid it.
        if (n.flags & NIF_STATE) {
            existing->state = (existing->state & ~n.stateMask) | (n.state & n.stateMask);
        }
        // The tooltip and the icon are kept for icons in the primary tray too.
        // The secondary tray does not draw these, but the arrange window lists
        // them, and an icon the user cannot see is one they cannot pick out of a
        // list of twenty-odd. One HICON copy each is cheap.
        if (n.flags & NIF_TIP) {
            existing->tip = n.tip;
        }
        if (n.flags & NIF_ICON) {
            TakePictureLocked(existing, n.icon);
        }
        if (!n.exePath.empty()) {
            existing->exePath = n.exePath;
        }
        return existingIsMirrored;  // repaint only if it just left the tray
    }

    if (!existing) {
        MirroredIcon icon;
        icon.serial = g_nextSerial.fetch_add(1);
        icon.hasGuid = n.hasGuid;
        icon.guid = n.guid;
        icon.ownerWnd = n.ownerWnd;
        icon.uID = n.uID;
        icon.exePath = n.exePath;
        icon.destination = destination;
        icon.forwardedToShell = forwardToShell;
        icon.shellTarget = forwardToShell;
        icon.destinationDecided = !n.exePath.empty();
        g_icons.push_back(std::move(icon));
        if (outRetractFromShell) {
            *outRetractFromShell = !forwardToShell;
        }
        existing = &g_icons.back();
    } else if (!existingIsMirrored) {
        MirroredIcon moved = std::move(*existing);
        g_primaryOnly.erase(g_primaryOnly.begin() + (existing - g_primaryOnly.data()));
        g_icons.push_back(std::move(moved));
        existing = &g_icons.back();
    }
    existing->shownTray = mirrorTray;

    // NIF_* flags say which fields are meaningful; anything else keeps its old
    // value, exactly as the real tray behaves on a partial NIM_MODIFY.
    if (n.flags & NIF_ICON) {
        TakePictureLocked(existing, n.icon);
    }
    if (n.flags & NIF_TIP) {
        existing->tip = n.tip;
    }
    if (n.flags & NIF_MESSAGE) {
        existing->callbackMessage = n.callbackMessage;
    }
    if (n.flags & NIF_STATE) {
        existing->state = (existing->state & ~n.stateMask) | (n.state & n.stateMask);
    }
    if (!n.exePath.empty()) {
        existing->exePath = n.exePath;
    }
    FoldTrayRecord(&existing->payload, payload);
    existing->revision++;
    return true;
}

// ---------------------------------------------------------------------------
// Saying where an icon is
//
// Explorer answers Shell_NotifyIconGetRect for the icons it has, and an icon
// that lives only in one of Split Tray's trays is not one of them. The question
// failed, and Tauri's tray library, which asks before it handles any click,
// dropped every click on such an icon: Telemachus gave no menu and no window
// from the secondary tray, and worked again as soon as it was moved back. So
// the mod answers for those icons, with where it drew them (DECISIONS 52).
// ---------------------------------------------------------------------------

// The icon a question is about, as an index into g_icons, when the answer is
// the mod's to give; -1 when Explorer has the icon or nobody does. Caller holds
// g_mutex.
int SecondaryOnlyIconIndexLocked(const IconRectQuery& query) {
    for (size_t i = 0; i < g_icons.size(); i++) {
        if (SameIcon(g_icons[i], query)) {
            return g_icons[i].forwardedToShell ? -1 : static_cast<int>(i);
        }
    }
    return -1;
}

// Runs on the taskbar's thread, which is also the XAML thread (DECISIONS 37).
bool SecondaryIconScreenRect(const IconRectQuery& query, RECT* out) {
    uint64_t serial;
    std::wstring exe;
    bool firstAsk;
    bool embedded = false;
    bool found = false;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        const int index = SecondaryOnlyIconIndexLocked(query);
        if (index < 0) {
            return false;
        }
        MirroredIcon& icon = g_icons[static_cast<size_t>(index)];
        serial = icon.serial;
        exe = FileNameOf(icon.exePath);
        firstAsk = !icon.askedWhere;
        icon.askedWhere = true;

        // Drawn either in a taskbar or in a floating panel, never both.
        const TrayTarget* tray = FindTrayLocked(icon.shownTray);
        embedded = tray && TrayEmbeddedLocked(*tray);
        auto layout = g_floatingLayouts.find(icon.shownTray);
        if (!embedded && layout != g_floatingLayouts.end()) {
            const auto inTray = IconsInTrayLocked(icon.shownTray);
            const auto at = std::find(inTray.begin(), inTray.end(),
                                      static_cast<size_t>(index));
            found = at != inTray.end() &&
                    CellScreenRect(layout->second,
                                   static_cast<int>(at - inTray.begin()),
                                   static_cast<int>(inTray.size()), out);
        }
    }
#ifndef SPLITTRAY_NO_XAML
    if (embedded) {
        found = SplitTrayXaml::IconScreenRect(serial, out);
    }
#else
    (void)serial;
#endif

    // Once per icon: applications ask on every click, and this is the
    // taskbar's thread (DECISIONS 51).
    if (firstAsk) {
        if (found) {
            Wh_Log(L"told %s where its icon is: (%ld,%ld)-(%ld,%ld)",
                   exe.empty() ? L"?" : exe.c_str(), out->left, out->top, out->right,
                   out->bottom);
        } else {
            Wh_Log(L"could not say where %s's icon is; Explorer will say it has none",
                   exe.empty() ? L"?" : exe.c_str());
        }
    }
    return found;
}

// Counts a call of the subclass for as long as it runs: unloading does not
// finish while one is under way on the taskbar's thread (DECISIONS 73).
struct SubclassCall {
    SubclassCall() { g_subclassDepth.fetch_add(1); }
    ~SubclassCall() { g_subclassDepth.fetch_sub(1); }
    SubclassCall(const SubclassCall&) = delete;
    SubclassCall& operator=(const SubclassCall&) = delete;
};

LRESULT CALLBACK ShellTrayWndSubclassProc(HWND hWnd,
                                          UINT msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          DWORD_PTR) {
    const SubclassCall call;

    if (msg == GetReplayMessage()) {
        // Only a wake-up: what to hand Explorer is read from the icon store,
        // and wParam and lParam mean nothing (DECISIONS 58, 66). While the
        // mod unloads, it is the hand-back (DECISIONS 68).
        OnShellTrayWake([hWnd](HWND senderWnd, const std::vector<BYTE>& record) {
            // The shell copies an icon's picture while it handles the message -
            // which is why an application may destroy its own straight after
            // Shell_NotifyIcon returns - so the delivery's copy can go after.
            return DeliverToShell(hWnd, senderWnd, record);
        });
        // Once every icon is back, the subclass takes itself off, here: from
        // the unloading thread that is a message this thread has to answer,
        // and one that did not answer held unloading for as long as it did
        // not (DECISIONS 73). On this thread it is a direct call.
        if (g_handedBack.load()) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ShellTrayWndSubclassProc);
        }
        return 0;
    }

#ifndef SPLITTRAY_NO_XAML
    if (msg == GetXamlRefreshMessage()) {
        if (!g_unloading.load()) {
            SplitTrayXaml::OnIconStoreChanged();
        }
        return 0;
    }

    if (msg == GetAttachMessage()) {
        // On the taskbar's UI thread, which is the only place XAML may be
        // touched. Posted by the tray thread's timer until it takes.
        if (!g_unloading.load()) {
            SplitTrayXaml::TryAttachEmbeddedTray();
        }
        return 0;
    }

    if (msg == GetXamlRemoveMessage()) {
        SplitTrayXaml::RemoveEverything();
        return 0;
    }
#endif

    // Kept track of until every icon is back with Explorer, unloading or not
    // (DECISIONS 68).
    if (msg != WM_COPYDATA || g_handedBack.load()) {
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    }

    auto* cds = reinterpret_cast<const COPYDATASTRUCT*>(lParam);

    IconRectQuery rectQuery;
    if (cds && ParseIconRectQuery(cds->dwData, cds->lpData, cds->cbData, &rectQuery)) {
        RECT rect;
        if (SecondaryIconScreenRect(rectQuery, &rect)) {
            return IconRectReply(rect, rectQuery.part);
        }
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    }

    TrayNotification n;
    if (!cds || !ParseTrayNotification(cds->dwData, cds->lpData, cds->cbData, &n)) {
        // Appbar traffic, in-proc load requests, anything unrecognised: untouched.
        return DefSubclassProc(hWnd, msg, wParam, lParam);
    }

    const BYTE* raw = static_cast<const BYTE*>(cds->lpData);
    std::vector<BYTE> payload(raw, raw + cds->cbData);

    bool forwardToShell = true;
    bool retractFromShell = false;
    bool recordShellAnswer = false;
    bool needsRepaint = false;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        FillMissingPathLocked(&n);
        needsRepaint = ApplyNotificationLocked(n, payload, &forwardToShell,
                                               &retractFromShell, &recordShellAnswer);
    }

    // Every arrival is logged, not only the ones routed away.
    //
    // Only swallowed icons used to be logged, so an icon that a rule should have
    // matched and did not was indistinguishable from an icon that never reached
    // the mod - which is precisely the question that could not be answered when
    // the SystemInformer rule appeared to do nothing. NIM_ADD and NIM_DELETE are
    // rare; NIM_MODIFY is not, so it is left out.
    if (n.message == NIM_ADD || n.message == NIM_DELETE) {
        std::wstring exe{SplitTray::FileNameOf(n.exePath)};
        if (exe.empty()) {
            exe = n.exePath.empty() ? L"(none)" : n.exePath;
        }
        Wh_Log(L"tray %s: uID=%u exe=%s tip='%s' -> %s",
               n.message == NIM_ADD ? L"add" : L"delete", n.uID, exe.c_str(),
               n.tip.c_str(),
               forwardToShell ? L"primary tray" : L"secondary tray only");
    }

    // An icon whose routing was settled late needs the same replay a settings
    // change performs, to retract it from the tray it was provisionally put in.
    if (g_routingNeedsReapply.exchange(false)) {
        NotifyTrayWindow(WM_ST_SETTINGS);
    }

    if (needsRepaint) {
        NotifyTrayWindow(WM_ST_REFRESH);
#ifndef SPLITTRAY_NO_XAML
        // This handler already runs on the taskbar's UI thread, which is the
        // XAML thread, so the embedded tray is updated here rather than
        // marshalled across.
        SplitTrayXaml::OnIconStoreChanged();
#endif
    } else if (n.message == NIM_ADD || n.message == NIM_DELETE ||
               (g_arrangeOpen.load() && (n.flags & NIF_TIP))) {
        // An icon of the main tray's coming or going, or renamed, repaints
        // none of the mod's trays, but the arrange window lists it
        // (DECISIONS 75, 83).
        NotifyTrayWindow(WM_ST_REFRESH);
    }

    if (forwardToShell) {
        const LRESULT answer = DefSubclassProc(hWnd, msg, wParam, lParam);
        if (recordShellAnswer) {
            bool ask;
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                ask = RecordShellAnswerLocked(n, answer != FALSE);
            }
            // Posted, so it is handled once the messages waiting now are.
            if (ask) {
                WakeReplayDelivery();
            }
        }
        return answer;
    }

    // An icon new to the mod, going to one of its trays, may be in Explorer's
    // already. Loaded into a running Explorer - installed, updated, switched
    // off and on - the mod cannot know what Explorer holds: the icons an
    // earlier load put back as it unloaded, or ones registered before it was
    // there. Swallowing the message left those in the main tray as well, for
    // good. So Explorer is told to let it go; one it never had costs a refused
    // call (DECISIONS 64).
    if (retractFromShell) {
        DeliverToShell(hWnd, reinterpret_cast<HWND>(wParam),
                       PayloadWithMessage(payload, NIM_DELETE));
        // A modify for an icon the mod has never seen added is answered as
        // Explorer answers one for an icon it does not have. An application
        // that recovers from that - SystemInformer does - adds its icon again,
        // with the callback and everything else a modify leaves out: what a
        // click in Split Tray's tray is sent with, and what puts the icon back
        // whole when the mod unloads. Saying yes left it with neither - its
        // icons did nothing when clicked, and Explorer refused them back.
        if (n.message == NIM_MODIFY) {
            return FALSE;
        }
    }

    // A balloon Explorer shows, from a hidden copy it holds (DECISIONS 78).
    if ((n.message == NIM_ADD || n.message == NIM_MODIFY) && (n.flags & NIF_INFO)) {
        ShowBalloonThroughShell(n, payload,
                                [hWnd](HWND senderWnd, const std::vector<BYTE>& record) {
                                    return DeliverToShell(hWnd, senderWnd, record);
                                });
    }

    // The keyboard focus, given back to an icon in one of Split Tray's trays
    // (DECISIONS 80): in a taskbar on this thread, and in a floating tray on
    // the tray thread, which owns its window (DECISIONS 85). Each leaves an
    // icon it does not show alone.
    if (n.message == NIM_SETFOCUS) {
        uint64_t serial = 0;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            for (const auto& icon : g_icons) {
                if (SameIcon(icon, n)) {
                    serial = icon.serial;
                    break;
                }
            }
        }
#ifndef SPLITTRAY_NO_XAML
        SplitTrayXaml::FocusIconCell(serial);
#endif
        if (HWND trayWnd = g_trayWnd.load(); trayWnd && serial) {
            PostMessageW(trayWnd, WM_ST_FOCUS_ICON, static_cast<WPARAM>(serial), 0);
        }
        return TRUE;
    }

    // Swallowed: the icon lives only in the secondary tray. Shell_NotifyIcon
    // reports success to the caller, which is what the real tray would do.
    //
    // Not logged for NIM_MODIFY. SystemInformer sends four a second, and this
    // runs on the taskbar's own thread: with a log viewer attached each line
    // costs tens of milliseconds there, time in which every other
    // application's tray messages wait - and shell32 gives up on a tray that
    // does not answer, so an icon arriving then is lost. Adds and deletes are
    // logged above already (DECISIONS 44).
    if (n.message != NIM_MODIFY) {
        Wh_Log(L"routed away from primary tray: msg=%u uID=%u exe=%s", n.message,
               n.uID, n.exePath.c_str());
    }
    return TRUE;
}

void RequestIconRepopulation();

// Finds the tray window that shell32 targets, if it is this process's: one in
// another Explorer is not the mod's to attach to (DECISIONS 92). Secondary
// taskbars use the class Shell_SecondaryTrayWnd and never receive
// notification-area messages.
HWND FindShellTrayWindow() {
    return FindShellTrayWindows().own;
}

// Returns true only when this call attached to a tray window the mod was not
// already watching.
//
// Only once the tray thread is running (DECISIONS 69). Without it nothing
// draws the mod's trays, and an icon swallowed into one is lost. Wh_ModInit
// took the end of its wait for the thread as leave to attach, and a thread
// that went on to fail left a subclass swallowing icons into trays nothing
// drew; one still starting attaches from its own timer once it runs.
bool SubclassShellTrayWindow() {
    if (g_trayThreadState.load() != TrayThreadState::Running) {
        return false;
    }
    HWND hWnd = FindShellTrayWindow();
    if (!hWnd || g_shellTrayWnd.load() == hWnd) {
        return false;
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                                                      ShellTrayWndSubclassProc, 0)) {
        Wh_Log(L"failed to subclass Shell_TrayWnd %p", hWnd);
        return false;
    }
    g_shellTrayWnd.store(hWnd);
    Wh_Log(L"subclassed Shell_TrayWnd %p", hWnd);
    return true;
}

// Whether applications have to be asked to re-register their icons, now that the
// mod is watching a tray window.
//
// Explorer announces every taskbar it creates (TaskbarCreated) once its tray is
// ready, and every application re-registers in answer. The mod asked as well,
// as soon as it attached - which on an Explorer start is about 3 seconds in,
// against Explorer's own announcement at about 17. Everything registered twice,
// the first time into a tray that was not ready and dropped it; and an icon
// whose application answered only once was simply gone. Measured on this
// machine: Desk Tray's "WhatsApp (default)" answered the mod and not Explorer,
// was forwarded to the primary tray, and was not in it.
//
// So the mod asks only when Explorer will not: when it was loaded into an
// Explorer whose taskbar already existed, or when Explorer's announcement went
// out while the mod was not watching. Pure, so the three cases are tested.
bool ShouldAskAppsToReRegister(bool shellExistedAtLoad,
                               bool firstAttach,
                               bool missedShellAnnouncement) {
    return (firstAttach && shellExistedAtLoad) || missedShellAnnouncement;
}

std::atomic<bool> g_shellExistedAtLoad{false};
std::atomic<bool> g_attachedBefore{false};
std::atomic<bool> g_missedShellAnnouncement{false};
// The tray window the re-register question was last settled for, so it is
// asked once per window and not on every tick of the timer.
std::atomic<HWND> g_reRegisterSettledFor{nullptr};

UINT TaskbarCreatedMessage() {
    static const UINT msg = RegisterWindowMessageW(L"TaskbarCreated");
    return msg;
}

// Explorer said its taskbar is ready. If the mod is not watching that taskbar
// yet, every application has just re-registered where the mod cannot see it,
// and has to be asked again once it can.
void NoteShellAnnouncedTaskbar() {
    HWND watched = g_shellTrayWnd.load();
    const bool watching =
        watched && IsWindow(watched) && FindShellTrayWindow() == watched;
    if (!watching) {
        g_missedShellAnnouncement.store(true);
    }
    Wh_Log(L"Explorer announced its taskbar (TaskbarCreated)%s",
           watching ? L"" : L" before the mod was watching it");
}

// Keeps the mod attached to whatever tray window currently exists.
//
// This cannot be a one-shot at startup. Windhawk injects into explorer.exe before
// the shell has created its taskbar, so at Wh_ModInit there is usually no
// Shell_TrayWnd to find at all; and the taskbar is destroyed and recreated again
// later on some display, DPI and theme changes. Called from Wh_ModAfterInit and
// then from the tray thread's timer until it succeeds.
//
// Icons are collected only once the mod is actually watching: broadcasting
// TaskbarCreated before the subclass is in place asks every application to
// re-register into a tray the mod cannot see, which wastes the one chance to pick
// up icons that pre-date the mod.
void EnsureShellTrayWindowSubclassed() {
    if (g_unloading.load()) {
        return;
    }

    HWND watched = g_shellTrayWnd.load();
    if (watched && !(IsWindow(watched) && FindShellTrayWindow() == watched)) {
        Wh_Log(L"Shell_TrayWnd %p is gone; looking for the new one", watched);
        g_shellTrayWnd.store(nullptr);
        watched = nullptr;
    }

    if (!watched) {
        if (!SubclassShellTrayWindow()) {
            return;
        }
        watched = g_shellTrayWnd.load();
    }

    // Once per tray window. Wh_ModInit may have attached already - the case of
    // a mod loaded into a running Explorer - and that attachment is settled
    // here too; it used to return early and never ask, which was the one case
    // where asking is needed.
    if (g_reRegisterSettledFor.exchange(watched) == watched) {
        return;
    }
    const bool firstAttach = !g_attachedBefore.exchange(true);
    const bool ask = ShouldAskAppsToReRegister(g_shellExistedAtLoad.load(), firstAttach,
                                               g_missedShellAnnouncement.exchange(false));
    bool repopulate;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        repopulate = g_settings.repopulateOnLoad;
    }
    if (!ask) {
        Wh_Log(L"not asking applications to re-register: Explorer announces this "
               L"taskbar itself once it is ready");
    } else if (repopulate) {
        RequestIconRepopulation();
    } else {
        Wh_Log(L"applications would be asked to re-register, but repopulateOnLoad "
               L"is off");
    }
}

// Asks every application to re-register its tray icon. This is the message the
// shell broadcasts after an Explorer restart, so applications already handle it;
// it is the only supported way to learn about icons that existed before the mod
// loaded.
void RequestIconRepopulation() {
    const UINT taskbarCreated = TaskbarCreatedMessage();
    if (!taskbarCreated) {
        return;
    }
    DWORD recipients = BSM_APPLICATIONS;
    BroadcastSystemMessageW(BSF_IGNORECURRENTTASK | BSF_POSTMESSAGE, &recipients,
                           taskbarCreated, 0, 0);
    Wh_Log(L"broadcast TaskbarCreated to collect existing icons");
}

// ---------------------------------------------------------------------------
// Moving one cell along a row
//
// Pure, and deliberately on this side of the XAML guard so it can be tested.
// Reordering the tray by removing the dragged cell and re-inserting it is what
// stopped dragging working at all: an element that leaves the visual tree loses
// pointer capture, so the drag ended on its first step and the release that
// followed was taken for a click. The invariant the plan has to hold is that
// the dragged cell is never the one removed; the cells around it move instead.
// ---------------------------------------------------------------------------

struct CellShiftStep {
    size_t removeAt;
    size_t insertAt;
};

std::vector<CellShiftStep> PlanCellShift(size_t from, size_t to) {
    std::vector<CellShiftStep> steps;
    while (from != to) {
        // Walk the neighbour on the side we are heading for across to the far
        // side of the dragged cell. That advances the dragged cell one slot
        // without touching it.
        const size_t neighbour = (to > from) ? from + 1 : from - 1;
        steps.push_back({neighbour, from});
        from = neighbour;
    }
    return steps;
}

// Where a TaskbarHost keeps its root XAML element, read out of the first
// instructions of TaskbarHost::FrameHeight, which loads that very member:
//
//   48 83 EC xx    sub rsp, xx
//   48 83 C1 nn    add rcx, nn    <- nn is the offset
//
// Any other code is a layout this mod does not know, and the answer is "no"
// rather than a guess: the offset is dereferenced and called through, so a
// guess that is wrong after a Windows update is a crash inside Explorer
// (DECISIONS 61). The pattern comes from taskbar-start-button-position.
// How good an anchor an element of the tray is for the tree walk, best first.
// The row the tray goes into is found by walking up from the anchor
// (FindTrayRow), so the anchor has to be below the row: a tray icon, not the
// frame around them. SystemTrayFrame comes first in the tree and sits above
// the row, so walking up from it found no row at all and settled for the Grid
// beside the clock - where the tray went on every reload, when the icons exist
// already and only the tree walk can find them. The icon the constructor hook
// hands over on a fresh start is an IconView named SystemTrayIcon (DECISIONS
// 65).
int AnchorPreference(std::wstring_view className, std::wstring_view name) {
    if (className == L"SystemTray.IconView") {
        return name == L"SystemTrayIcon" ? 0 : 1;
    }
    if (className == L"SystemTray.SystemTrayFrame") {
        return 3;
    }
    return 2;
}

bool ElementOffsetFromFrameHeight(const BYTE* code, size_t* offset) {
    if (!code || !offset) {
        return false;
    }
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 && code[7] <= 0x7F) {
        *offset = code[7];
        return true;
    }
    return false;
}

// Reading Explorer's private objects, at offsets nothing promises. After a
// Windows update one of these reads could land on memory that is not there, and
// a fault inside Explorer takes the shell down. A C++ catch does not stop a
// fault and this compiler has no SEH, so the reads go through ReadProcessMemory,
// which fails instead, and a failure means the tray floats (from an external
// review).
bool TryReadPointer(const void* address, void** value) {
    if (!address || !value) {
        return false;
    }
    void* read = nullptr;
    SIZE_T bytes = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), address, &read, sizeof(read), &bytes) ||
        bytes != sizeof(read)) {
        return false;
    }
    *value = read;
    return true;
}

bool IsExecutableCode(const void* address) {
    MEMORY_BASIC_INFORMATION info;
    if (!address || !VirtualQuery(address, &info, sizeof(info))) {
        return false;
    }
    constexpr DWORD kExecutable = PAGE_EXECUTE | PAGE_EXECUTE_READ |
                                  PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    return info.State == MEM_COMMIT && (info.Protect & kExecutable) != 0 &&
           (info.Protect & PAGE_GUARD) == 0;
}

// Whether a pointer read out of a private object is worth calling through: it
// points at readable memory whose first entry is a vtable of IUnknown's three
// methods, each of them code. A layout that moved leaves some other value
// there, which this turns down rather than calls.
bool LooksLikeComObject(const void* object) {
    void* vtable = nullptr;
    if (!TryReadPointer(object, &vtable) || !vtable) {
        return false;
    }
    for (int i = 0; i < 3; i++) {
        void* method = nullptr;
        if (!TryReadPointer(static_cast<void**>(vtable) + i, &method) ||
            !IsExecutableCode(method)) {
            return false;
        }
    }
    return true;
}

// Walks a task band object to the sub-object whose vftable is the one for
// ITaskListWndSite, which is what GetTaskbarHost has to be called on. Each slot
// is read guarded: an object whose layout changed can end before the twenty
// slots looked at.
void* TaskListWndSiteOf(void* taskBand, void* wantedVftable) {
    if (!taskBand || !wantedVftable) {
        return nullptr;
    }
    for (int i = 0; i < 20; i++) {
        void** candidate = static_cast<void**>(taskBand) + i;
        void* vftable = nullptr;
        if (!TryReadPointer(candidate, &vftable)) {
            return nullptr;
        }
        if (vftable == wantedVftable) {
            return candidate;
        }
    }
    return nullptr;
}

}  // namespace SplitTray

// ============================================================================
// Section 10 - The taskbar's XAML
//
// The Windows 11 taskbar is system XAML (Windows.UI.Xaml, not WinUI 3) hosted in
// a DesktopWindowXamlSource island inside explorer.exe, and there is no public
// API for reaching it.
//
// The XAML debugging route - registering a TAP with InitializeXamlDiagnosticsEx
// - was tried first and abandoned: it is a single-consumer-per-process resource,
// and windows-11-taskbar-styler holds it and challenges anyone else who asks.
// Using it would mean breaking that mod (DECISIONS.md 24).
//
// So this section does what every mod that manipulates SystemTray elements does
// instead: it hooks private symbols in Explorer's own DLLs. That needs no
// exclusive connection and coexists with the styler. It is the fragile part of
// the mod, which is why tools/check-symbols.py verifies every symbol against the
// live binaries at build time - a hook that fails to resolve is otherwise silent.
//
// Everything in this section runs on Explorer's XAML UI thread except the hook
// installation itself. XAML objects are not agile and must never be touched from
// the mod's tray thread.
// ============================================================================

// ---------------------------------------------------------------------------
// Reaching the taskbar's XAML
//
// Not through XAML diagnostics: that is a single-consumer-per-process resource
// and windows-11-taskbar-styler holds it (DECISIONS.md 24). Instead the mod
// hooks private symbols in Explorer's own DLLs, which is what every mod that
// manipulates SystemTray elements does, and which coexists with the styler.
//
// Two separate jobs:
//
//   SystemTray.dll   IconView's constructor is the anchor. Every tray icon view
//                    Explorer creates runs through it, and the XAML element is
//                    the implementation object's projected interface. This is
//                    how the mod gets a live element to work from at all.
//
//   taskbar.dll      Turns a taskbar *window* into its XamlRoot, so an element
//                    can be matched to the taskbar that owns it by identity.
//                    Element -> window is not possible without diagnostics;
//                    window -> XamlRoot is (DECISIONS.md 27).
//
// Every symbol here is checked against the live binaries at build time by
// tools/check-symbols.py, because a hook that fails to resolve is silent.
//
// This part is compiled into every build, the test binaries included, while
// the rest of the section is not. Windhawk's catalog reads the tables of
// symbols from the source, to cache the symbols for the mod's users, and it
// cannot evaluate a condition of the mod's own: a table under one goes
// unread (DECISIONS 94).
// ---------------------------------------------------------------------------

namespace SplitTrayXaml {

// --- taskbar.dll ------------------------------------------------------------

void* g_CTaskBand_ITaskListWndSite_vftable = nullptr;
void* g_CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

// GetTaskbarHost returns a std::shared_ptr by value, so on x64 it takes a
// hidden pointer to the caller's two-pointer result slot.
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t g_CTaskBand_GetTaskbarHost = nullptr;
CTaskBand_GetTaskbarHost_t g_CSecondaryTaskBand_GetTaskbarHost = nullptr;

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t g_TaskbarHost_FrameHeight = nullptr;

using Ref_count_base_Decref_t = void(WINAPI*)(void* pThis);
Ref_count_base_Decref_t g_Ref_count_base_Decref = nullptr;

// --- SystemTray.dll ---------------------------------------------------------

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t g_IconView_IconView_Original = nullptr;
// With the rest of the XAML, below. The test binaries, which leave that out,
// have a stand-in that is never installed.
void* WINAPI IconView_IconView_Hook(void* pThis);

// --- state ------------------------------------------------------------------

std::atomic<bool> g_taskbarSymbolsHooked{false};
std::atomic<bool> g_systemTraySymbolsHooked{false};
std::atomic<bool> g_symbolFailureLogged{false};

// ---------------------------------------------------------------------------
// Installing the hooks
//
// Neither module is loaded when Windhawk injects, for the same reason the
// taskbar window does not exist yet (DECISIONS.md 18), so this is retried from
// the tray thread's timer rather than attempted once at startup.
// ---------------------------------------------------------------------------

bool HookTaskbarSymbols() {
    if (g_taskbarSymbolsHooked.load()) {
        return true;
    }
    HMODULE module = GetModuleHandleW(L"taskbar.dll");
    if (!module) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &g_CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &g_CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &g_CTaskBand_GetTaskbarHost},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &g_CSecondaryTaskBand_GetTaskbarHost},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &g_TaskbarHost_FrameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &g_Ref_count_base_Decref},
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        if (!g_symbolFailureLogged.exchange(true)) {
            Wh_Log(L"[xaml] could not resolve taskbar.dll symbols; the embedded "
                   L"tray cannot find which taskbar an element belongs to");
        }
        return false;
    }

    g_taskbarSymbolsHooked.store(true);
    Wh_Log(L"[xaml] taskbar.dll symbols resolved");
    return true;
}

bool HookSystemTraySymbols() {
    if (g_systemTraySymbolsHooked.load()) {
        return true;
    }
    HMODULE module = GetModuleHandleW(L"SystemTray.dll");
    if (!module) {
        return false;
    }

    // Verified present in this exact binary by tools/check-symbols.py;
    // Taskbar.View.dll does not carry it (DECISIONS.md 26).
    WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
        {{LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
         &g_IconView_IconView_Original, IconView_IconView_Hook},
    };

    if (!WindhawkUtils::HookSymbols(module, systemTrayDllHooks,
                                    ARRAYSIZE(systemTrayDllHooks))) {
        if (!g_symbolFailureLogged.exchange(true)) {
            Wh_Log(L"[xaml] could not resolve the SystemTray.dll IconView "
                   L"constructor; no tray elements will be seen");
        }
        return false;
    }

    g_systemTraySymbolsHooked.store(true);
    Wh_Log(L"[xaml] SystemTray.dll IconView constructor hooked");
    return true;
}

}  // namespace SplitTrayXaml

// The rest of the section. The regression and integration binaries define
// SPLITTRAY_NO_XAML and leave it out: they cannot exercise it (no XAML island in
// a test process, no taskbar to attach to) and compiling nine WinRT projections
// roughly doubles every build in the test loop. The compile check and the DLL
// build - the two that decide whether the shipped mod is correct - always
// compile it, and so does the XAML suite (DECISIONS 91).
#ifndef SPLITTRAY_NO_XAML

// winbase.h defines GetCurrentTime as a macro, which collides with
// Windows.UI.Xaml.Media.Animation's Timeline::GetCurrentTime.
#undef GetCurrentTime

#include <winrt/base.h>

#include <winrt/Windows.Foundation.h>
// WriteableBitmap::PixelBuffer returns an IBuffer. Its accessors have deduced
// return types, so the consumer definitions have to be in scope before they can
// be called, not just the forward declaration the imaging projection pulls in.
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>  // xaml_typename, for the popup style
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Input.h>

#include <robuffer.h>

#include <functional>
#include <list>
#include <memory>

namespace SplitTrayXaml {

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxmi = winrt::Windows::UI::Xaml::Media::Imaging;

using SplitTray::g_mutex;
using SplitTray::g_settings;
using SplitTray::g_unloading;

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }
    return module;
}

// ---------------------------------------------------------------------------
// Tree dump (discovery aid)
//
// Explorer's taskbar XAML is undocumented and changes between Windows builds.
// Rather than hardcode a path through it, the mod can print the subtree it is
// looking at, so the element types and names it targets are chosen from what is
// actually there. Off by default; turned on with the dumpXamlTree setting.
// ---------------------------------------------------------------------------

std::wstring DescribeElement(wux::DependencyObject const& obj) {
    std::wstring description;
    try {
        description = winrt::get_class_name(obj).c_str();
    } catch (...) {
        description = L"<unknown type>";
    }
    if (auto element = obj.try_as<wux::FrameworkElement>()) {
        auto name = element.Name();
        if (!name.empty()) {
            description += L" name='";
            description += name.c_str();
            description += L"'";
        }
        WCHAR size[96];
        swprintf_s(size, L" %gx%g vis=%d", element.ActualWidth(),
                   element.ActualHeight(),
                   element.Visibility() == wux::Visibility::Visible ? 1 : 0);
        description += size;
    }
    return description;
}

void DumpSubtree(wux::DependencyObject const& obj, int depth, int maxDepth) {
    if (!obj || depth > maxDepth) {
        return;
    }
    std::wstring indent(static_cast<size_t>(depth) * 2, L' ');
    Wh_Log(L"[xaml] %s%s", indent.c_str(), DescribeElement(obj).c_str());

    const int count = wuxm::VisualTreeHelper::GetChildrenCount(obj);
    for (int i = 0; i < count; i++) {
        DumpSubtree(wuxm::VisualTreeHelper::GetChild(obj, i), depth + 1, maxDepth);
    }
}

// ---------------------------------------------------------------------------
// HICON to a XAML image source
//
// The floating renderer could hand an HICON straight to DrawIconEx. XAML cannot:
// it needs pixels. Converting properly matters more than it looks, because tray
// icons come in two historical shapes and getting the alpha wrong is not subtle
// - it shows up as a black box around every icon.
//
//   * A 32-bit icon carries its own alpha channel in the colour bitmap.
//   * An older icon has no alpha at all, and its transparency lives in a
//     separate 1bpp mask where a set bit means "transparent".
//
// GetDIBits reads the colour bitmap; if every alpha byte comes back zero the
// icon is one of the older ones and the alpha is rebuilt from the mask. The
// result is premultiplied, which is what WriteableBitmap expects.
// ---------------------------------------------------------------------------

struct IconPixels {
    int width = 0;
    int height = 0;
    std::vector<BYTE> bgra;  // top-down, premultiplied
};

bool ReadIconPixels(HICON icon, IconPixels* out) {
    if (!icon || !out) {
        return false;
    }

    ICONINFO info = {};
    if (!GetIconInfo(icon, &info)) {
        return false;
    }
    // GetIconInfo hands back two bitmaps that belong to the caller.
    struct BitmapGuard {
        HBITMAP colour;
        HBITMAP mask;
        ~BitmapGuard() {
            if (colour) DeleteObject(colour);
            if (mask) DeleteObject(mask);
        }
    } guard{info.hbmColor, info.hbmMask};

    BITMAP bitmap = {};
    // A monochrome icon has no colour bitmap; its mask holds the image stacked
    // above the mask, which is double height.
    const HBITMAP source = info.hbmColor ? info.hbmColor : info.hbmMask;
    if (!source || !GetObjectW(source, sizeof(bitmap), &bitmap)) {
        return false;
    }

    const int width = bitmap.bmWidth;
    const int height = info.hbmColor ? bitmap.bmHeight : bitmap.bmHeight / 2;
    if (width <= 0 || height <= 0 || width > 512 || height > 512) {
        return false;
    }

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return false;
    }
    struct DcGuard {
        HDC dc;
        ~DcGuard() {
            if (dc) ReleaseDC(nullptr, dc);
        }
    } dcGuard{screenDc};

    BITMAPINFO header = {};
    header.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    header.bmiHeader.biWidth = width;
    header.bmiHeader.biHeight = -height;  // negative: top-down
    header.bmiHeader.biPlanes = 1;
    header.bmiHeader.biBitCount = 32;
    header.bmiHeader.biCompression = BI_RGB;

    std::vector<BYTE> pixels(static_cast<size_t>(width) * height * 4);
    if (!GetDIBits(screenDc, source, 0, static_cast<UINT>(height), pixels.data(),
                   &header, DIB_RGB_COLORS)) {
        return false;
    }

    // Older icons come back with a zero alpha channel throughout; their
    // transparency has to be taken from the mask instead.
    bool hasAlpha = false;
    for (size_t i = 3; i < pixels.size(); i += 4) {
        if (pixels[i] != 0) {
            hasAlpha = true;
            break;
        }
    }

    if (!hasAlpha && info.hbmMask) {
        BITMAPINFO maskHeader = header;
        std::vector<BYTE> maskPixels(static_cast<size_t>(width) * height * 4);
        if (GetDIBits(screenDc, info.hbmMask, 0, static_cast<UINT>(height),
                      maskPixels.data(), &maskHeader, DIB_RGB_COLORS)) {
            for (size_t i = 0; i < pixels.size(); i += 4) {
                // In the mask, white (non-zero) means transparent.
                pixels[i + 3] = maskPixels[i] ? 0 : 255;
            }
        } else {
            for (size_t i = 3; i < pixels.size(); i += 4) {
                pixels[i] = 255;
            }
        }
    }

    // WriteableBitmap treats its buffer as premultiplied BGRA; handing it
    // straight alpha leaves a dark fringe on every anti-aliased edge.
    for (size_t i = 0; i < pixels.size(); i += 4) {
        const unsigned alpha = pixels[i + 3];
        if (alpha == 255) {
            continue;
        }
        pixels[i + 0] = static_cast<BYTE>(pixels[i + 0] * alpha / 255);
        pixels[i + 1] = static_cast<BYTE>(pixels[i + 1] * alpha / 255);
        pixels[i + 2] = static_cast<BYTE>(pixels[i + 2] * alpha / 255);
    }

    out->width = width;
    out->height = height;
    out->bgra = std::move(pixels);
    return true;
}

// Builds a XAML image source from an icon. Must be called on the XAML thread.
wuxmi::WriteableBitmap IconToBitmap(HICON icon) {
    IconPixels pixels;
    if (!ReadIconPixels(icon, &pixels)) {
        return nullptr;
    }

    wuxmi::WriteableBitmap bitmap(pixels.width, pixels.height);
    auto buffer = bitmap.PixelBuffer();
    if (buffer.Capacity() < pixels.bgra.size()) {
        return nullptr;
    }

    // IBufferByteAccess is how the pixels are reached without a copy through a
    // WinRT collection.
    winrt::com_ptr<::Windows::Storage::Streams::IBufferByteAccess> access;
    if (FAILED(reinterpret_cast<::IUnknown*>(winrt::get_abi(buffer))
                   ->QueryInterface(winrt::guid_of<
                                        ::Windows::Storage::Streams::IBufferByteAccess>(),
                                    access.put_void()))) {
        return nullptr;
    }
    BYTE* target = nullptr;
    if (FAILED(access->Buffer(&target)) || !target) {
        return nullptr;
    }
    memcpy(target, pixels.bgra.data(), pixels.bgra.size());
    buffer.Length(static_cast<uint32_t>(pixels.bgra.size()));
    bitmap.Invalidate();
    return bitmap;
}

bool g_loggedTargetStack = false;

// One display's tray, inside that display's taskbar. Taskbar-thread only, like
// the XAML it holds.
struct EmbeddedTray {
    HMONITOR monitor = nullptr;
    int number = 0;  // its tray number, refreshed from g_trays on every sync

    // Resolved lazily on the taskbar's own UI thread, because touching XAML
    // from the mod's tray thread is not allowed. Cleared when the taskbar is
    // recreated.
    HWND taskbarWnd = nullptr;
    wux::XamlRoot root = nullptr;
    bool loggedTarget = false;

    winrt::weak_ref<wuxc::StackPanel> panel;
    bool active = false;
    bool loggedEmbedded = false;

    // Measured off an element of this taskbar (EnsureEmbeddedPanel), so a
    // taskbar at another scale, or resized by another mod, gets cells its own
    // size.
    double cellWidthDip = 32;
    double cellHeightDip = 38;
    double iconSizeDip = 16;

    // What was last drawn, so a refresh can tell an update from a change of
    // layout: every icon in the tray, then those on the bar and in the overflow.
    std::vector<uint64_t> drawnAll;
    std::vector<uint64_t> drawnShown;
    std::vector<uint64_t> drawnOverflow;
    // The icons behind the chevron, as its overflow popup draws them.
    std::vector<SplitTray::CellSnapshot> hiddenEntries;
    // The picture revision each cell on the bar shows, by serial: a refresh
    // copies and draws only the pictures that changed (DECISIONS 82).
    std::map<uint64_t, uint64_t> drawnPictures;
};

// This and every other global that holds XAML is never destroyed by the C++
// runtime. When Explorer exits, Wh_ModUninit is not called and the runtime would
// release that XAML from whichever thread is exiting, after XAML has gone - a
// crash or a hang at sign-out. RemoveEverything lets go of all of it on the
// taskbar's thread when the mod unloads (DECISIONS 94).
[[clang::no_destroy]] std::vector<std::unique_ptr<EmbeddedTray>> g_embeddedTrays;

EmbeddedTray* TrayOfMonitor(HMONITOR monitor) {
    for (auto& tray : g_embeddedTrays) {
        if (tray->monitor == monitor) {
            return tray.get();
        }
    }
    return nullptr;
}

EmbeddedTray* TrayOfPanel(wuxc::StackPanel const& panel) {
    if (!panel) {
        return nullptr;
    }
    for (auto& tray : g_embeddedTrays) {
        if (tray->panel.get() == panel) {
            return tray.get();
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Window -> XamlRoot
// ---------------------------------------------------------------------------

// The TaskbarHost keeps its root XAML element at an offset which is not a
// stable part of any contract, so it is read out of TaskbarHost::FrameHeight
// (ElementOffsetFromFrameHeight). When that code is not what it was, the
// taskbar is left alone and the tray floats instead.
wux::XamlRoot XamlRootFromTaskbarHost(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t elementOffset = 0;
    const bool known =
        taskbarHostSharedPtr[0] &&
        SplitTray::ElementOffsetFromFrameHeight(
            reinterpret_cast<const BYTE*>(g_TaskbarHost_FrameHeight), &elementOffset);
    if (!known) {
        static std::atomic<bool> logged{false};
        if (!logged.exchange(true)) {
            Wh_Log(L"[xaml] TaskbarHost::FrameHeight is not the code this mod "
                   L"knows; not embedding, the trays float instead");
        }
    }

    wux::FrameworkElement element = nullptr;
    if (known) {
        // Read and checked before it is called through: a layout that moved
        // leaves something else at the offset (SplitTray::LooksLikeComObject).
        void* elementPointer = nullptr;
        if (SplitTray::TryReadPointer(
                reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + elementOffset,
                &elementPointer) &&
            SplitTray::LooksLikeComObject(elementPointer)) {
            static_cast<IUnknown*>(elementPointer)
                ->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                 winrt::put_abi(element));
        } else if (elementPointer) {
            static std::atomic<bool> logged{false};
            if (!logged.exchange(true)) {
                Wh_Log(L"[xaml] the TaskbarHost's element is not an object; not "
                       L"embedding, the trays float instead");
            }
        }
    }

    wux::XamlRoot result = nullptr;
    if (element) {
        try {
            result = element.XamlRoot();
        } catch (...) {
        }
    }

    // GetTaskbarHost handed back a shared_ptr; release the reference it took.
    if (taskbarHostSharedPtr[1] && g_Ref_count_base_Decref) {
        g_Ref_count_base_Decref(taskbarHostSharedPtr[1]);
    }
    return result;
}

wux::XamlRoot XamlRootOfTaskbar(HWND taskbarWnd) {
    if (!taskbarWnd || !g_taskbarSymbolsHooked.load()) {
        return nullptr;
    }

    WCHAR className[64] = {};
    GetClassNameW(taskbarWnd, className, ARRAYSIZE(className));
    const bool secondary =
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;

    HWND bandWnd;
    void* wantedVftable;
    CTaskBand_GetTaskbarHost_t getTaskbarHost;

    if (secondary) {
        bandWnd = FindWindowExW(taskbarWnd, nullptr, L"WorkerW", nullptr);
        wantedVftable = g_CSecondaryTaskBand_ITaskListWndSite_vftable;
        getTaskbarHost = g_CSecondaryTaskBand_GetTaskbarHost;
    } else {
        bandWnd = reinterpret_cast<HWND>(GetPropW(taskbarWnd, L"TaskbandHWND"));
        wantedVftable = g_CTaskBand_ITaskListWndSite_vftable;
        getTaskbarHost = g_CTaskBand_GetTaskbarHost;
    }

    if (!bandWnd || !wantedVftable || !getTaskbarHost) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(bandWnd, 0));
    void* site = SplitTray::TaskListWndSiteOf(taskBand, wantedVftable);
    if (!site) {
        Wh_Log(L"[xaml] could not find the ITaskListWndSite sub-object on %s",
               className);
        return nullptr;
    }

    void* taskbarHostSharedPtr[2] = {};
    getTaskbarHost(site, taskbarHostSharedPtr);
    return XamlRootFromTaskbarHost(taskbarHostSharedPtr);
}

// A display's taskbar window, if it has one: Windows can be set to show the
// taskbar on the main display only, and then there is nothing to embed into
// and the tray floats instead.
HWND FindTaskbarWindowOn(HMONITOR monitor) {
    if (!monitor) {
        return nullptr;
    }

    struct Search {
        HMONITOR wanted;
        HWND found;
    } search{monitor, nullptr};

    EnumWindows(
        [](HWND wnd, LPARAM param) -> BOOL {
            auto* search = reinterpret_cast<Search*>(param);
            WCHAR className[64] = {};
            GetClassNameW(wnd, className, ARRAYSIZE(className));
            if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
                return TRUE;
            }
            // Its objects are read from this process's memory, so only this
            // process's taskbar will do (DECISIONS 92).
            DWORD owner = 0;
            GetWindowThreadProcessId(wnd, &owner);
            if (owner != GetCurrentProcessId()) {
                return TRUE;
            }
            if (MonitorFromWindow(wnd, MONITOR_DEFAULTTONULL) == search->wanted) {
                search->found = wnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&search));

    return search.found;
}

// Must run on the taskbar's UI thread.
wux::XamlRoot EnsureTargetXamlRoot(EmbeddedTray& tray) {
    if (tray.root && tray.taskbarWnd && IsWindow(tray.taskbarWnd)) {
        return tray.root;
    }

    tray.root = nullptr;
    tray.taskbarWnd = FindTaskbarWindowOn(tray.monitor);
    if (!tray.taskbarWnd) {
        return nullptr;
    }

    tray.root = XamlRootOfTaskbar(tray.taskbarWnd);
    if (tray.root && !tray.loggedTarget) {
        tray.loggedTarget = true;
        RECT rect = {};
        GetWindowRect(tray.taskbarWnd, &rect);
        Wh_Log(L"[xaml] tray %d: taskbar hwnd=%p (%ld,%ld)-(%ld,%ld), XamlRoot "
               L"resolved", tray.number, tray.taskbarWnd, rect.left, rect.top,
               rect.right, rect.bottom);
    }
    return tray.root;
}

void RemovePanel(EmbeddedTray& tray);

// Brings the embedded trays in line with the displays: one for every connected
// display's tray while embedding is on, and none otherwise. Numbers are taken
// afresh each time, since plugging a display in renumbers the trays after it.
void SyncEmbeddedTrays() {
    std::vector<std::pair<HMONITOR, int>> wanted;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_settings.embedInTaskbar && !g_unloading.load()) {
            for (const auto& tray : SplitTray::g_trays) {
                if (tray.forDisplay && tray.available) {
                    wanted.push_back({tray.monitor.handle, tray.number});
                }
            }
        }
    }

    for (auto it = g_embeddedTrays.begin(); it != g_embeddedTrays.end();) {
        const bool keep =
            std::any_of(wanted.begin(), wanted.end(),
                        [&](const auto& want) { return want.first == (*it)->monitor; });
        if (keep) {
            ++it;
            continue;
        }
        RemovePanel(**it);
        it = g_embeddedTrays.erase(it);
    }

    for (const auto& [monitor, number] : wanted) {
        if (EmbeddedTray* existing = TrayOfMonitor(monitor)) {
            existing->number = number;
            continue;
        }
        auto tray = std::make_unique<EmbeddedTray>();
        tray->monitor = monitor;
        tray->number = number;
        g_embeddedTrays.push_back(std::move(tray));
    }
}

// ---------------------------------------------------------------------------
// The anchor hook
// ---------------------------------------------------------------------------

// Keeps the Loaded revokers alive until they fire. A raw token would outlive
// the element and fire on a dead object. Never destroyed by the runtime, like
// g_embeddedTrays.
[[clang::no_destroy]] std::list<wux::FrameworkElement::Loaded_revoker> g_loadedRevokers;

// Walks up to the named container an element sits in, the way Explorer's own
// tray elements are addressed - by name, not by position (DECISIONS.md 25).
wux::FrameworkElement AncestorNamed(wux::FrameworkElement const& element,
                                    std::wstring_view name) {
    wux::DependencyObject current = element;
    for (int depth = 0; depth < 32 && current; depth++) {
        if (auto asElement = current.try_as<wux::FrameworkElement>()) {
            if (asElement.Name() == name) {
                return asElement;
            }
        }
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    return nullptr;
}

bool EnsureEmbeddedPanel(EmbeddedTray& tray, wux::FrameworkElement const& anchor);
void RefreshEmbeddedTray();
void RefreshTray(EmbeddedTray& tray);

void OnTrayIconViewLoaded(wux::FrameworkElement const& iconView) {
    wux::XamlRoot elementRoot = nullptr;
    try {
        elementRoot = iconView.XamlRoot();
    } catch (...) {
        return;
    }
    if (!elementRoot) {
        return;
    }

    // Which display's taskbar it is on. Identity, not geometry: two taskbars on
    // identically sized monitors would be indistinguishable by size or scale.
    SyncEmbeddedTrays();
    EmbeddedTray* owner = nullptr;
    for (auto& tray : g_embeddedTrays) {
        auto root = EnsureTargetXamlRoot(*tray);
        if (root && root == elementRoot) {
            owner = tray.get();
            break;
        }
    }
    if (!owner) {
        return;  // the primary taskbar's, or one Split Tray has no tray on
    }

    std::wstring className;
    try {
        className = winrt::get_class_name(iconView).c_str();
    } catch (...) {
        return;
    }

    // Which named stack it landed in says what kind of tray slot it is.
    PCWSTR stackName = L"(none)";
    for (PCWSTR candidate : {L"MainStack", L"NonActivatableStack",
                             L"ControlCenterButton", L"NotificationCenterButton"}) {
        if (AncestorNamed(iconView, candidate)) {
            stackName = candidate;
            break;
        }
    }

    Wh_Log(L"[xaml] tray %d element: %s name='%s' in %s", owner->number,
           className.c_str(), iconView.Name().c_str(), stackName);

    // This element is on the tray's taskbar, so it is a usable anchor: its
    // parent panel is where the mod's tray goes, and its size is the native
    // cell size.
    if (EnsureEmbeddedPanel(*owner, iconView)) {
        RefreshTray(*owner);
    }

    bool dump;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        dump = g_settings.dumpXamlTree;
    }
    if (dump && !g_loggedTargetStack) {
        // Print from the tray frame down once, so the insertion point is chosen
        // from what is there rather than assumed.
        if (auto frame = AncestorNamed(iconView, L"SystemTrayFrameGrid")) {
            g_loggedTargetStack = true;
            Wh_Log(L"[xaml] ---- target taskbar tray subtree ----");
            DumpSubtree(frame, 0, 14);
            Wh_Log(L"[xaml] ---- end ----");
        } else {
            // No element of that name: walk to the top of the island instead.
            wux::DependencyObject root = iconView;
            for (int i = 0; i < 32; i++) {
                auto parent = wuxm::VisualTreeHelper::GetParent(root);
                if (!parent) {
                    break;
                }
                root = parent;
            }
            g_loggedTargetStack = true;
            Wh_Log(L"[xaml] ---- target island subtree (no SystemTrayFrameGrid) ----");
            DumpSubtree(root, 0, 14);
            Wh_Log(L"[xaml] ---- end ----");
        }
    }
}

void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* result = g_IconView_IconView_Original(pThis);

    if (g_unloading.load()) {
        return result;
    }

    // The C++/WinRT implementation object carries its projected interface in the
    // second slot; this is how the reference mods reach the element. Read and
    // checked before it is called through, as no contract promises the slot.
    wux::FrameworkElement iconView = nullptr;
    void* projected = nullptr;
    if (!pThis ||
        !SplitTray::TryReadPointer(static_cast<void**>(pThis) + 1, &projected) ||
        !SplitTray::LooksLikeComObject(projected)) {
        static std::atomic<bool> logged{false};
        if (!logged.exchange(true)) {
            Wh_Log(L"[xaml] an IconView is not laid out as this mod knows; its "
                   L"element is left alone");
        }
        return result;
    }
    try {
        static_cast<IUnknown*>(projected)->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(iconView));
    } catch (...) {
        return result;
    }
    if (!iconView) {
        return result;
    }

    // XamlRoot is not available until the element is in a tree.
    try {
        g_loadedRevokers.emplace_back();
        auto revoker = std::prev(g_loadedRevokers.end());
        *revoker = iconView.Loaded(
            winrt::auto_revoke,
            [revoker](wf::IInspectable const& sender, wux::RoutedEventArgs const&) {
                g_loadedRevokers.erase(revoker);
                if (g_unloading.load()) {
                    return;
                }
                if (auto element = sender.try_as<wux::FrameworkElement>()) {
                    try {
                        OnTrayIconViewLoaded(element);
                    } catch (...) {
                        Wh_Log(L"[xaml] tray element handling failed: %08X",
                               winrt::to_hresult());
                    }
                }
            });
    } catch (...) {
        Wh_Log(L"[xaml] could not observe a tray element: %08X",
               winrt::to_hresult());
    }

    return result;
}

// ---------------------------------------------------------------------------
// The tray itself, inside the taskbar
//
// The insertion point is derived at runtime rather than written down: from a
// tray element known to be on the target taskbar, walk up to the first XAML
// Panel that can hold children, and put the mod's own panel at the front of it.
// That lands the icons to the left of the clock, which is where the native tray
// sits, without depending on the shape of Explorer's private tree.
//
// Sizes come from the anchor element too - its ActualWidth/ActualHeight are the
// real cell size in DIPs, already correct for the monitor's scaling and for
// whatever other taskbar mods have done to it. Measuring beats assuming 32x38.
//
// All of this runs on the taskbar's UI thread. That is also the thread the
// Shell_TrayWnd subclass runs on, so icon changes can update the XAML directly
// with no marshalling. Every taskbar is on that one thread (DECISIONS 37), so
// the same holds for every display's tray.
// ---------------------------------------------------------------------------

// Where the mod's panel belongs, and which sibling it goes in front of.
//
// The first attempt took the anchor's nearest Panel ancestor. On a secondary
// taskbar the anchor is the clock, and its nearest Panel ancestor is *inside the
// clock's own button* - so the icons became children of the clock and inherited
// its context menu.
//
// What is wanted is the tray row: the container the clock button itself sits in,
// so the mod's panel is the clock's sibling. Walking up keeps hold of the child
// it came through, which is the clock's top-level wrapper and therefore exactly
// the element to insert in front of.
//
// The row is recognised by the names Explorer gives it (DECISIONS.md 25), with
// the SystemTray.Stack class as a fallback, rather than by counting levels.
struct TrayRow {
    wuxc::Panel panel = nullptr;
    wux::UIElement insertBefore = nullptr;
};

std::wstring ClassNameOf(wux::DependencyObject const& object) {
    try {
        return winrt::get_class_name(object).c_str();
    } catch (...) {
        return L"?";
    }
}

bool g_loggedAncestorChain = false;

TrayRow FindTrayRow(wux::FrameworkElement const& anchor) {
    // The whole ancestor chain, so the decision can be made with all of it in
    // view rather than stopping at the first thing that looked plausible.
    std::vector<wux::DependencyObject> chain;
    chain.push_back(anchor);
    wux::DependencyObject current = wuxm::VisualTreeHelper::GetParent(anchor);
    for (int i = 0; i < 24 && current; i++) {
        chain.push_back(current);
        current = wuxm::VisualTreeHelper::GetParent(current);
    }

    if (!g_loggedAncestorChain) {
        g_loggedAncestorChain = true;
        // Printed once: if the tray lands in the wrong place again, this says
        // exactly what the walk had to choose from.
        for (size_t i = 0; i < chain.size(); i++) {
            auto element = chain[i].try_as<wux::FrameworkElement>();
            Wh_Log(L"[xaml]   ancestor %zu: %s name='%s'%s", i,
                   ClassNameOf(chain[i]).c_str(),
                   element ? element.Name().c_str() : L"",
                   chain[i].try_as<wuxc::StackPanel>()  ? L" [StackPanel]"
                   : chain[i].try_as<wuxc::Panel>()     ? L" [Panel]"
                                                        : L"");
        }
    }

    // The clock lives inside a SystemTray.OmniButton. Everything at or below the
    // highest such ancestor is that button's own template - inserting there is
    // what put the icons under its context menu. Start looking above it.
    size_t base = 0;
    for (size_t i = 0; i < chain.size(); i++) {
        const std::wstring name = ClassNameOf(chain[i]);
        if (name == L"SystemTray.OmniButton" || name == L"SystemTray.IconView") {
            base = i;
        }
    }

    TrayRow result;

    // Above the button, the first panel that lays children out in a line. A Grid
    // puts every child in the same cell, which is why the clock ended up behind
    // the icons rather than beside them.
    for (size_t i = base + 1; i < chain.size(); i++) {
        if (auto stack = chain[i].try_as<wuxc::StackPanel>()) {
            result.panel = stack;
            result.insertBefore = chain[i - 1].try_as<wux::UIElement>();
            return result;
        }
    }

    // No sequential panel anywhere above it. Any panel above the button still
    // beats being inside the button, but say so - it will overlap.
    for (size_t i = base + 1; i < chain.size(); i++) {
        if (auto panel = chain[i].try_as<wuxc::Panel>()) {
            Wh_Log(L"[xaml] no StackPanel above the tray button; falling back to "
                   L"%s, which may overlap the clock",
                   ClassNameOf(chain[i]).c_str());
            result.panel = panel;
            result.insertBefore = chain[i - 1].try_as<wux::UIElement>();
            return result;
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// Order, overflow and dragging
//
// Three things that belong together because they all act on the same list:
//
//   * A user-chosen order, which has to outlive the applications that own the
//     icons - so it is keyed on something stable, not on a window handle.
//   * An overflow flyout behind a chevron, the way the native tray hides icons
//     past a certain count.
//   * Dragging a cell to a new position, which rewrites that order.
//
// The store (section 5) is deliberately not involved. Order is a presentation
// concern; the routing model underneath has no opinion about it.
// ---------------------------------------------------------------------------

// The saved order, most-left first. Kept in the mod's own storage rather than in
// its settings: it is state the mod maintains, not something to hand-edit.
std::vector<std::wstring> g_iconOrder;
bool g_iconOrderLoaded = false;

void LoadIconOrder() {
    if (g_iconOrderLoaded) {
        return;
    }
    g_iconOrderLoaded = true;
    g_iconOrder.clear();

    const std::wstring stored = SplitTray::ReadStoredString(SplitTray::kIconOrderValue);
    std::wstring_view remaining(stored);
    while (!remaining.empty()) {
        const size_t split = remaining.find(L'\n');
        std::wstring_view entry = remaining.substr(0, split);
        if (!entry.empty()) {
            g_iconOrder.emplace_back(entry);
        }
        if (split == std::wstring_view::npos) {
            break;
        }
        remaining.remove_prefix(split + 1);
    }
}

void SaveIconOrder() {
    std::wstring joined;
    for (const auto& key : g_iconOrder) {
        if (!joined.empty()) {
            joined += L'\n';
        }
        joined += key;
    }
    SplitTray::WriteStoredString(SplitTray::kIconOrderValue, joined);
}

size_t OrderPositionOf(std::wstring_view key) {
    for (size_t i = 0; i < g_iconOrder.size(); i++) {
        if (g_iconOrder[i] == key) {
            return i;
        }
    }
    return g_iconOrder.size();  // unknown icons go to the end, in store order
}

// ---------------------------------------------------------------------------
// Dragging
// ---------------------------------------------------------------------------

uint64_t SerialOfCell(wux::FrameworkElement const& cell);
void CommitVisualOrder(wuxc::StackPanel const& panel);

struct DragState {
    bool pointerDown = false;
    bool dragging = false;
    // Set when a drag ended some way other than a clean release - capture taken
    // away, say. Without it the release that follows looks like a click and
    // activates whatever icon the pointer was over, which is the worst possible
    // outcome of a failed drag.
    bool suppressClick = false;
    // The pointer has been dragged clear of the tray row, which means "move this
    // icon to the other tray" rather than "reorder it".
    bool outside = false;
    double startX = 0;
    // The row the press was in, and its cell size: a drag belongs to one tray.
    winrt::weak_ref<wuxc::StackPanel> panel;
    double cellWidthDip = 32;
    double cellHeightDip = 38;
    // Diagnostic only: keeps the move log to one line per press.
    bool loggedMove = false;
};

DragState g_drag;

// A refresh that arrived while the pointer was down, to run once it is released.
// See RefreshEmbeddedTray for why it waits.
bool g_refreshPending = false;

// Far enough that a click with a shaky hand is still a click.
constexpr double kDragThresholdDip = 5;

// Dragged this far above or below the row and the icon is leaving this tray.
constexpr double kDragOutThresholdDip = 28;

// Move the child at `from` to `to` without ever taking it out of the panel.
//
// The obvious implementation - remove the dragged cell and insert it at the new
// index - is why dragging did not work. An element that leaves the visual tree
// loses pointer capture, XAML raises PointerCaptureLost, the drag state is torn
// down, and the release that follows is taken for a click on whatever the icon
// was dropped on. The drag therefore died on its first step, every time.
//
// Walking the neighbour across the dragged cell has the same effect on the
// order while leaving the dragged cell itself in place, so its capture holds
// for the whole gesture.
void ShiftCellTo(wuxc::StackPanel const& panel, uint32_t from, uint32_t to) {
    auto children = panel.Children();
    for (const auto& step : SplitTray::PlanCellShift(from, to)) {
        auto element = children.GetAt(static_cast<uint32_t>(step.removeAt));
        children.RemoveAt(static_cast<uint32_t>(step.removeAt));
        children.InsertAt(static_cast<uint32_t>(step.insertAt), element);
    }
}

// The first slot a real icon may occupy: the chevron holds slot 0 when it is
// shown, and "show hidden icons" belongs at the start of the row, as it does in
// the native tray. The chevron is the only child without an icon in its Tag.
uint32_t FirstIconSlot(wuxc::StackPanel const& panel) {
    auto children = panel.Children();
    if (children.Size() == 0) {
        return 0;
    }
    auto first = children.GetAt(0).try_as<wux::FrameworkElement>();
    return (first && SerialOfCell(first) == 0) ? 1 : 0;
}


// ---------------------------------------------------------------------------
// Making the icons behave like tray icons
//
// Click forwarding itself is section 7 and is already tested; these are the
// pointer handlers that feed it. Events are marked handled so they do not bubble
// on to the taskbar, which is the other half of why the clock's menu was
// appearing.
// ---------------------------------------------------------------------------

wuxc::Border MakeCell(EmbeddedTray const& tray, SplitTray::CellSnapshot const& entry,
                      bool draggable);
void RefreshEmbeddedTray();
void ShowTrayContextMenu(wux::FrameworkElement const& target, uint64_t serial,
                         HMONITOR monitor);
void HideOverflowFlyout();
using SplitTray::ShiftHeld;

wuxm::SolidColorBrush TransparentBrush() {
    return wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0});
}

wuxm::SolidColorBrush HoverBrush() {
    // Close to the native tray's hover wash: a low-alpha white over whatever the
    // taskbar material is.
    return wuxm::SolidColorBrush(winrt::Windows::UI::Color{36, 255, 255, 255});
}

wuxm::SolidColorBrush PressedBrush() {
    return wuxm::SolidColorBrush(winrt::Windows::UI::Color{64, 255, 255, 255});
}

// Which icon a cell draws: the serial it carries in its Tag (MirroredIcon::
// serial), or 0 for the chevron and the empty tray's handle.
uint64_t SerialOfCell(wux::FrameworkElement const& cell) {
    try {
        if (auto boxed = cell.Tag()) {
            return static_cast<uint64_t>(winrt::unbox_value<int64_t>(boxed));
        }
    } catch (...) {
    }
    return 0;
}

// The placement key of the icon with this serial, or empty if it has gone.
std::wstring KeyOfSerial(uint64_t serial) {
    std::lock_guard<std::mutex> lock(g_mutex);
    const int index = SplitTray::IndexOfSerialLocked(serial);
    return index < 0 ? std::wstring()
                     : SplitTray::StableKeyOf(SplitTray::g_icons[static_cast<size_t>(index)]);
}

// The number of the tray on this display, or 0.
int TrayNumberOf(HMONITOR monitor) {
    EmbeddedTray* tray = TrayOfMonitor(monitor);
    return tray ? tray->number : 0;
}

// The real cursor position, which is what the tray callback protocol wants.
// The pointer args give island-relative device-independent coordinates, which
// would have to be converted twice to get back to the same number.
POINT CursorPoint() {
    POINT point = {};
    GetCursorPos(&point);
    return point;
}

// ---------------------------------------------------------------------------
// A cell's face: the button its picture sits in
//
// Screen readers and the keyboard could not reach Split Tray's cells: a
// Border with an Image in it has no automation peer and takes no focus. The
// native tray's icons are buttons, so each cell holds one - named for its icon
// (DECISIONS 81), focusable, and answering the keyboard as Explorer does
// (DECISIONS 80). It takes no pointer input: the cell handles that, as before,
// so clicks and dragging are unchanged. Its template is the bare minimum,
// since the taskbar's own style for buttons would draw one around the picture.
// ---------------------------------------------------------------------------

// Made once, on the taskbar's thread. A function-local static was released when
// the mod's DLL unloaded, on Windhawk's thread rather than the taskbar's; as a
// global it is let go of by RemoveEverything, like g_embeddedTrays.
[[clang::no_destroy]] wux::Controls::ControlTemplate g_faceTemplate{nullptr};
bool g_faceTemplateTried = false;

wux::Controls::ControlTemplate FaceTemplate() {
    if (!g_faceTemplateTried) {
        g_faceTemplateTried = true;
        try {
            g_faceTemplate = wux::Markup::XamlReader::Load(
                         L"<ControlTemplate "
                         L"xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' "
                         L"TargetType='Button'><ContentPresenter "
                         L"Content='{TemplateBinding Content}' HorizontalAlignment='Center' "
                         L"VerticalAlignment='Center'/></ControlTemplate>")
                         .as<wux::Controls::ControlTemplate>();
        } catch (...) {
            Wh_Log(L"[xaml] could not make the cells' button template: %08X",
                   winrt::to_hresult());
        }
    }
    return g_faceTemplate;
}

wuxc::Button FaceOfCell(wuxc::Border const& cell) {
    return cell.Child().try_as<wuxc::Button>();
}

// Where the icon with `serial` is, as the anchor point of a keyboard callback.
POINT AnchorOfSerial(uint64_t serial) {
    RECT rect;
    if (IconScreenRect(serial, &rect)) {
        return POINT{(rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2};
    }
    return CursorPoint();
}

// Shift+F10 or the menu key on a cell: its application's menu (DECISIONS 80).
// The key and ContextRequested can both report one press; the second report
// within half a second is the same press, and is not sent again. Taskbar
// thread only.
void SendKeyboardMenu(uint64_t serial) {
    static uint64_t lastSerial = 0;
    static ULONGLONG lastTick = 0;
    const ULONGLONG now = GetTickCount64();
    if (serial == lastSerial && now - lastTick < 500) {
        return;
    }
    lastSerial = serial;
    lastTick = now;
    SplitTray::ForwardKey(serial, true, AnchorOfSerial(serial));
}

// ---------------------------------------------------------------------------
// Popups of an icon's own (DECISIONS 79)
//
// From version 4 an icon without NIF_SHOWTIP has no tooltip of the tray's: its
// application draws a popup, on NIN_POPUPOPEN once the pointer has rested on
// the icon for the hover time, and takes it down on NIN_POPUPCLOSE.
// ---------------------------------------------------------------------------

[[clang::no_destroy]] wux::DispatcherTimer g_popupTimer{nullptr};  // as g_embeddedTrays
uint64_t g_popupWaiting = 0;  // the icon the pointer rests on
uint64_t g_popupOpen = 0;     // the icon whose popup is open

void ClosePopup() {
    if (g_popupOpen) {
        SplitTray::ForwardPopup(g_popupOpen, false, CursorPoint());
        g_popupOpen = 0;
    }
}

void WaitToOpenPopup(uint64_t serial) {
    g_popupWaiting = serial;
    if (!g_popupTimer) {
        g_popupTimer = wux::DispatcherTimer();
        UINT hoverMs = 400;
        SystemParametersInfoW(SPI_GETMOUSEHOVERTIME, 0, &hoverMs, 0);
        g_popupTimer.Interval(std::chrono::milliseconds(hoverMs));
        g_popupTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            g_popupTimer.Stop();
            const uint64_t serial = std::exchange(g_popupWaiting, 0);
            if (serial && serial != g_popupOpen) {
                ClosePopup();
                if (SplitTray::ForwardPopup(serial, true, CursorPoint())) {
                    g_popupOpen = serial;
                }
            }
        });
    }
    g_popupTimer.Stop();
    g_popupTimer.Start();
}

// The pointer has left the icon, or pressed it.
void EndPopup(uint64_t serial) {
    if (g_popupWaiting == serial) {
        g_popupWaiting = 0;
        if (g_popupTimer) {
            g_popupTimer.Stop();
        }
    }
    if (g_popupOpen == serial) {
        ClosePopup();
    }
}

// Records whether a display's tray is in its taskbar, so the tray thread knows
// whether it still needs a floating panel, and tells it to look.
void SetTrayEmbedded(EmbeddedTray& tray, bool embedded) {
    if (tray.active == embedded) {
        return;
    }
    tray.active = embedded;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (embedded) {
            SplitTray::g_embeddedMonitors.insert(tray.monitor);
        } else {
            SplitTray::g_embeddedMonitors.erase(tray.monitor);
        }
    }
    SplitTray::NotifyTrayWindow(SplitTray::WM_ST_REFRESH);
}

// Creates the mod's panel inside a display's taskbar, once. `anchor` must
// already be known to belong to that taskbar.
bool EnsureEmbeddedPanel(EmbeddedTray& tray, wux::FrameworkElement const& anchor) {
    if (auto existing = tray.panel.get()) {
        // Still parented? A taskbar rebuild drops it.
        if (wuxm::VisualTreeHelper::GetParent(existing)) {
            return true;
        }
        tray.panel = nullptr;
        tray.loggedEmbedded = false;
        tray.drawnAll.clear();
        tray.drawnShown.clear();
        tray.drawnOverflow.clear();
        SetTrayEmbedded(tray, false);
    }

    const TrayRow row = FindTrayRow(anchor);
    auto host = row.panel;
    if (!host) {
        Wh_Log(L"[xaml] no panel to insert into");
        return false;
    }

    // Size from the anchor's HEIGHT only.
    //
    // The anchor is whatever tray element loaded first on this taskbar, and on a
    // secondary taskbar that is the clock - a wide element (about 73x38 DIP),
    // not a square icon cell. Its width says nothing about how wide a
    // notification icon should be; taking it and halving it gave a 36 DIP icon
    // where the native one is 16.
    //
    // The heights do correspond: every element in the tray row is the same
    // height, so the anchor's height is the row height, and the native
    // proportions can be scaled off it. XAML works in device-independent pixels,
    // so these are the same numbers on a 96 DPI and a 120 DPI monitor - the
    // scaling is not this code's job.
    constexpr double kNativeCellWidthDip = 32;
    constexpr double kNativeCellHeightDip = 38;
    constexpr double kNativeIconDip = 16;

    const double anchorHeight = anchor.ActualHeight();
    double scale = 1.0;
    if (anchorHeight > 0) {
        scale = anchorHeight / kNativeCellHeightDip;
        // A taskbar resized by another mod is fine; a nonsense value is not.
        scale = std::clamp(scale, 0.5, 3.0);
    }
    tray.cellHeightDip = anchorHeight > 0 ? anchorHeight : kNativeCellHeightDip;
    tray.cellWidthDip = kNativeCellWidthDip * scale;
    tray.iconSizeDip = kNativeIconDip * scale;

    wuxc::StackPanel panel;
    panel.Orientation(wuxc::Orientation::Horizontal);
    panel.VerticalAlignment(wux::VerticalAlignment::Center);
    panel.Name(L"SplitTrayIcons");

    try {
        // In front of the element the walk came through - the clock's wrapper -
        // so the icons sit to its left, where the native tray is.
        uint32_t index = 0;
        if (!row.insertBefore ||
            !host.Children().IndexOf(row.insertBefore, index)) {
            index = 0;
        }
        host.Children().InsertAt(index, panel);
    } catch (...) {
        Wh_Log(L"[xaml] could not insert into the host panel: %08X",
               winrt::to_hresult());
        return false;
    }

    tray.panel = winrt::make_weak(panel);
    SetTrayEmbedded(tray, true);

    if (!tray.loggedEmbedded) {
        tray.loggedEmbedded = true;
        std::wstring hostClass = L"?";
        try {
            hostClass = winrt::get_class_name(host).c_str();
        } catch (...) {
        }
        auto hostElement = host.try_as<wux::FrameworkElement>();
        std::wstring anchorClass = L"?";
        try {
            anchorClass = winrt::get_class_name(anchor).c_str();
        } catch (...) {
        }
        // The anchor's own measurements are logged because the sizing is derived
        // from them: if the icons come out wrong, this line says why.
        Wh_Log(L"[xaml] inserted tray %d into %s name='%s'; anchor %s "
               L"is %.1fx%.1f DIP -> cell %.1fx%.1f, icon %.1f",
               tray.number, hostClass.c_str(),
               hostElement ? hostElement.Name().c_str() : L"",
               anchorClass.c_str(), anchor.ActualWidth(), anchor.ActualHeight(),
               tray.cellWidthDip, tray.cellHeightDip, tray.iconSizeDip);
    }
    return true;
}

// Saves the order of a tray's row, as the user has just dragged it. The order
// is one list for every tray: an icon's position only matters among the icons
// that share its tray, and keeping them in one list means an icon keeps its
// place when it is moved to another tray and back.
void CommitVisualOrder(wuxc::StackPanel const& panel) {
    LoadIconOrder();

    std::vector<std::wstring> order;
    auto children = panel.Children();
    for (uint32_t i = 0; i < children.Size(); i++) {
        auto cell = children.GetAt(i).try_as<wux::FrameworkElement>();
        if (!cell) {
            continue;
        }
        // The chevron carries no icon; only real icons take part in the order.
        if (const uint64_t serial = SerialOfCell(cell)) {
            std::wstring key = KeyOfSerial(serial);
            if (!key.empty()) {
                order.push_back(std::move(key));
            }
        }
    }

    // Anything not on screen right now - overflowed, or its application is not
    // running - keeps its saved position rather than being dropped.
    for (const auto& existing : g_iconOrder) {
        if (std::find(order.begin(), order.end(), existing) == order.end()) {
            order.push_back(existing);
        }
    }

    g_iconOrder = std::move(order);
    SaveIconOrder();
    Wh_Log(L"[xaml] icon order updated (%zu entries)", g_iconOrder.size());
}


// ---------------------------------------------------------------------------
// The tray's own context menu
//
// Plain right-click cannot be used: that belongs to the application that owns
// the icon, and taking it would break the thing tray icons are mostly for.
// Shift+right-click is the mod's own gesture instead, on an icon or on the
// chevron.
// ---------------------------------------------------------------------------

// The menu that makes a tray usable when it is empty.
//
// Without it there is no way into the mod at all from an empty tray: nothing is
// drawn, so there is nothing to right-click, and an icon can only arrive by a
// per-process rule the user has to write in the settings first.
void AppendMoveHereItems(wuxc::MenuFlyout const& menu, int trayNumber) {
    const auto known = SplitTray::KnownIcons();

    wuxc::MenuFlyoutSubItem submenu;
    submenu.Text(L"Move an icon to this tray");
    int offered = 0;
    for (const auto& entry : known) {
        if (entry.tray == trayNumber) {
            continue;
        }
        wuxc::MenuFlyoutItem item;
        item.Text(winrt::hstring{entry.label});
        const std::wstring key = entry.key;
        item.Click([key, trayNumber](wf::IInspectable const&,
                                     wux::RoutedEventArgs const&) {
            SplitTray::MoveIconToTray(key, SplitTray::Destination::Tray(trayNumber));
            RefreshEmbeddedTray();
        });
        submenu.Items().Append(item);
        offered++;
    }
    if (offered == 0) {
        wuxc::MenuFlyoutItem none;
        none.Text(L"Every icon is already here");
        none.IsEnabled(false);
        submenu.Items().Append(none);
    }
    menu.Items().Append(submenu);
}

// The handle shown when the tray holds nothing, so there is always somewhere to
// click. It is the only affordance on an empty tray.
wuxc::Border MakeTrayHandle(EmbeddedTray const& tray) {
    wuxc::Border handle;
    handle.Width(tray.cellWidthDip);
    handle.Height(tray.cellHeightDip);
    handle.Background(TransparentBrush());

    wuxc::TextBlock glyph;
    glyph.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    glyph.Text(L"");  // "See more"
    glyph.FontSize(tray.iconSizeDip * 0.75);
    glyph.Opacity(0.6);
    glyph.HorizontalAlignment(wux::HorizontalAlignment::Center);
    glyph.VerticalAlignment(wux::VerticalAlignment::Center);
    glyph.IsHitTestVisible(false);
    wux::Automation::AutomationProperties::SetName(glyph, L"Split Tray, no icons here yet");
    handle.Child(glyph);

    wuxc::ToolTipService::SetToolTip(
        handle, winrt::box_value(winrt::hstring{L"Split Tray - no icons here yet"}));

    handle.PointerEntered([](wf::IInspectable const& sender,
                             wux::Input::PointerRoutedEventArgs const&) {
        if (auto border = sender.try_as<wuxc::Border>()) {
            border.Background(HoverBrush());
        }
    });
    handle.PointerExited([](wf::IInspectable const& sender,
                            wux::Input::PointerRoutedEventArgs const&) {
        if (auto border = sender.try_as<wuxc::Border>()) {
            border.Background(TransparentBrush());
        }
    });
    handle.PointerPressed([](wf::IInspectable const&,
                             wux::Input::PointerRoutedEventArgs const& args) {
        args.Handled(true);  // do not let the taskbar underneath react
    });
    handle.PointerReleased([monitor = tray.monitor](
                               wf::IInspectable const& sender,
                               wux::Input::PointerRoutedEventArgs const& args) {
        args.Handled(true);
        if (auto border = sender.try_as<wux::FrameworkElement>()) {
            // Plain left-click: on the handle there is no application whose
            // right-click this would be taking, so it does not need Shift.
            ShowTrayContextMenu(border, 0, monitor);
        }
    });
    handle.DoubleTapped([](wf::IInspectable const&,
                           wux::Input::DoubleTappedRoutedEventArgs const& args) {
        args.Handled(true);
        SplitTray::NotifyTrayWindow(SplitTray::WM_ST_ARRANGE);
    });
    return handle;
}

// The mod's menu, built apart from showing it so what it offers and what each
// choice does are tested (tests/xaml). `serial` is the icon it was opened on,
// or 0 for the tray itself; `monitor` says which display's tray it was opened
// in.
wuxc::MenuFlyout BuildTrayContextMenu(uint64_t serial, HMONITOR monitor) {
    const std::wstring key = serial ? KeyOfSerial(serial) : std::wstring();
    const int here = TrayNumberOf(monitor);

    wuxc::MenuFlyout menu;

    if (!key.empty()) {
        // Every other tray there is to send it to, the primary one first.
        for (int target : SplitTray::AvailableTrayNumbers()) {
            if (target == here) {
                continue;
            }
            wuxc::MenuFlyoutItem move;
            move.Text(winrt::hstring{L"Move to " + SplitTray::TrayLabel(target)});
            move.Click([key, target](wf::IInspectable const&,
                                     wux::RoutedEventArgs const&) {
                SplitTray::MoveIconToTray(key, SplitTray::Destination::Tray(target));
                RefreshEmbeddedTray();
            });
            menu.Items().Append(move);
        }

        wuxc::MenuFlyoutItem hide;
        const bool hidden = SplitTray::IsIconHidden(key);
        hide.Text(hidden ? L"Show on the tray" : L"Hide in the overflow menu");
        hide.Click([key, hidden](wf::IInspectable const&,
                                 wux::RoutedEventArgs const&) {
            SplitTray::SetIconHidden(key, !hidden);
            RefreshEmbeddedTray();
        });
        menu.Items().Append(hide);
    }

    if (here > 0) {
        AppendMoveHereItems(menu, here);
    }

    {
        wuxc::MenuFlyoutSeparator separator;
        menu.Items().Append(separator);
    }

    // The reliable way to move icons, and the only one that works in both
    // directions: a window of the mod's own, where a drag is just a drag.
    wuxc::MenuFlyoutItem arrange;
    arrange.Text(L"Arrange icons…");
    arrange.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        SplitTray::NotifyTrayWindow(SplitTray::WM_ST_ARRANGE);
    });
    menu.Items().Append(arrange);

    wuxc::MenuFlyoutItem reset;
    reset.Text(L"Reset icon order");
    reset.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        g_iconOrder.clear();
        SaveIconOrder();
        RefreshEmbeddedTray();
        Wh_Log(L"[xaml] icon order reset");
    });
    menu.Items().Append(reset);

    wuxc::MenuFlyoutItem unhide;
    unhide.Text(L"Show every hidden icon");
    unhide.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        SplitTray::ShowAllHiddenIcons();
        RefreshEmbeddedTray();
    });
    menu.Items().Append(unhide);

    wuxc::MenuFlyoutItem forget;
    forget.Text(L"Reset moved icons");
    forget.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        {
            // Placements are otherwise only touched under g_mutex; the tray
            // thread may be re-resolving routing at this moment.
            std::lock_guard<std::mutex> lock(g_mutex);
            SplitTray::ForgetAllPlacements();
        }
        // Sends every icon back to where the rules put it, through the same
        // replay a settings change uses.
        SplitTray::NotifyTrayWindow(SplitTray::WM_ST_SETTINGS);
        RefreshEmbeddedTray();
    });
    menu.Items().Append(forget);
    return menu;
}

void ShowTrayContextMenu(wux::FrameworkElement const& target, uint64_t serial,
                         HMONITOR monitor) {
    try {
        BuildTrayContextMenu(serial, monitor).ShowAt(target);
    } catch (...) {
        Wh_Log(L"[xaml] could not show the tray menu: %08X", winrt::to_hresult());
    }
}

// ---------------------------------------------------------------------------
// A tray cell
// ---------------------------------------------------------------------------

// A cell for the icon with `serial`, sized for `tray`. The serial goes in the
// Tag, and everything a cell does - click, menu, drag - looks the icon up by it.
// The image of an icon's picture, sized for `tray`, or null if there is none.
wuxc::Image MakeCellImage(EmbeddedTray const& tray, HICON icon) {
    if (!icon) {
        return nullptr;
    }
    auto bitmap = IconToBitmap(icon);
    if (!bitmap) {
        return nullptr;
    }
    wuxc::Image image;
    image.Source(bitmap);
    image.Width(tray.iconSizeDip);
    image.Height(tray.iconSizeDip);
    image.Stretch(wuxm::Stretch::Uniform);
    image.HorizontalAlignment(wux::HorizontalAlignment::Center);
    image.VerticalAlignment(wux::VerticalAlignment::Center);
    image.IsHitTestVisible(false);  // the cell handles the pointer
    return image;
}

// ---------------------------------------------------------------------------
// What a cell does with the pointer
//
// Each handler's work, apart from reading XAML's event arguments: the cell,
// and where, and which button, as plain values. MakeCell's handlers read the
// arguments and call these, and the XAML suite calls them directly, since a
// test cannot raise a pointer event without moving the real pointer.
// ---------------------------------------------------------------------------

enum class PointerButton { Left, Right, Middle, Other };

// The pointer came onto a cell: its application hears it, and an icon that
// draws its own popup is waited on to open it (DECISIONS 79).
void CellPointerEntered(wuxc::Border const& border, POINT screenPoint) {
    border.Background(HoverBrush());
    if (!g_drag.pointerDown) {
        const uint64_t serial = SerialOfCell(border);
        SplitTray::ForwardHover(serial, screenPoint);
        WaitToOpenPopup(serial);
    }
}

void CellPointerMoved(wuxc::Border const& border, POINT screenPoint) {
    if (!g_drag.pointerDown) {
        SplitTray::ForwardHover(SerialOfCell(border), screenPoint);
    }
}

void CellPointerExited(wuxc::Border const& border) {
    border.Background(TransparentBrush());
    EndPopup(SerialOfCell(border));
}

void CellCaptureLost(wuxc::Border const& border) {
    border.Background(TransparentBrush());
    border.Opacity(1.0);
    // Capture can be taken away mid-drag. Keep the rearranging done so far
    // rather than discarding it, and make sure the release that follows is not
    // mistaken for a click on whatever the icon was left sitting over.
    if (g_drag.dragging) {
        if (auto panel = g_drag.panel.get()) {
            CommitVisualOrder(panel);
        }
        g_drag.suppressClick = true;
        Wh_Log(L"[xaml][drag] capture lost mid-drag; order committed");
    }
    g_drag.pointerDown = false;
    g_drag.dragging = false;
    g_drag.outside = false;
    // Posted, not run here: rebuilding the panel from inside one of its cells'
    // own handlers would destroy the element that is handling it.
    if (g_refreshPending) {
        RequestEmbeddedRefresh();
    }
}

// A press on a cell, at `rowX` along its row in the row's DIPs. Returns
// whether the pointer is to be captured for a drag: the cell is in a tray's
// row and can be dragged.
bool CellPressed(wuxc::Border const& border, bool draggable, double rowX) {
    border.Background(PressedBrush());
    EndPopup(SerialOfCell(border));

    // Presses on a tray icon are rare enough to log every one.
    Wh_Log(L"[xaml][drag] PointerPressed on a cell (draggable=%d)", draggable ? 1 : 0);
    g_drag.loggedMove = false;

    g_drag.pointerDown = true;
    g_drag.dragging = false;
    g_drag.outside = false;
    g_drag.suppressClick = false;
    g_drag.panel = nullptr;
    if (!draggable) {
        return false;
    }
    // The row this cell is in, and so the tray the drag belongs to.
    auto panel = wuxm::VisualTreeHelper::GetParent(border).try_as<wuxc::StackPanel>();
    EmbeddedTray* tray = TrayOfPanel(panel);
    if (!tray) {
        return false;
    }
    g_drag.panel = winrt::make_weak(panel);
    g_drag.cellWidthDip = tray->cellWidthDip;
    g_drag.cellHeightDip = tray->cellHeightDip;
    g_drag.startX = rowX;
    return true;
}

// The pointer, pressed, is at `x`,`y` in the row's DIPs. Returns whether that
// is a drag, which the event is then marked handled for.
bool CellDragged(wuxc::Border const& border, double x, double y) {
    // Once per press, not once per session. A one-shot probe is spent by the
    // first drag, which may well be one nobody was capturing - and then every
    // later attempt looks silent for the wrong reason.
    if (!g_drag.loggedMove) {
        g_drag.loggedMove = true;
        Wh_Log(L"[xaml][drag] PointerMoved on a cell (pointerDown=%d)",
               g_drag.pointerDown ? 1 : 0);
    }
    if (!g_drag.pointerDown) {
        return false;
    }
    auto panel = g_drag.panel.get();
    if (!panel || g_drag.cellWidthDip <= 0) {
        return false;
    }
    if (!g_drag.dragging && std::abs(x - g_drag.startX) < kDragThresholdDip) {
        return false;  // still a click, not yet a drag
    }
    if (!g_drag.dragging) {
        Wh_Log(L"[xaml][drag] started");
    }
    g_drag.dragging = true;

    // Dragged clear of the row: the gesture now means "send this icon to the
    // other tray", so stop reordering and say so by dimming it.
    const bool outside =
        (y < -kDragOutThresholdDip) || (y > g_drag.cellHeightDip + kDragOutThresholdDip);
    if (outside != g_drag.outside) {
        g_drag.outside = outside;
        border.Opacity(outside ? 0.4 : 1.0);
    }
    if (outside) {
        return true;
    }

    auto children = panel.Children();
    uint32_t current = 0;
    if (!children.IndexOf(border, current)) {
        return true;
    }
    // Which slot the pointer is over now. The chevron keeps slot 0.
    const uint32_t firstSlot = FirstIconSlot(panel);
    int target = static_cast<int>(x / g_drag.cellWidthDip);
    target = std::clamp(target, static_cast<int>(firstSlot),
                        static_cast<int>(children.Size()) - 1);
    if (static_cast<uint32_t>(target) == current) {
        return true;
    }
    try {
        ShiftCellTo(panel, current, static_cast<uint32_t>(target));
    } catch (...) {
        Wh_Log(L"[xaml][drag] reorder failed: %08X", winrt::to_hresult());
    }
    return true;
}

// The press ended with `button` let go, and Shift held or not.
// `releaseCapture` lets the pointer go. It is called once the drag's state is
// cleared, since letting the pointer go raises PointerCaptureLost, which must
// not take this for a drag cut short. `draggable` is false only for cells in
// the overflow popup.
void CellReleased(wuxc::Border const& border, bool draggable, HMONITOR monitor,
                  PointerButton button, bool shift,
                  std::function<void()> const& releaseCapture) {
    border.Background(HoverBrush());
    border.Opacity(1.0);

    const bool wasDragging = g_drag.dragging;
    const bool wasOutside = g_drag.outside;
    const bool suppressed = g_drag.suppressClick;
    g_drag.pointerDown = false;
    g_drag.dragging = false;
    g_drag.outside = false;
    g_drag.suppressClick = false;
    // Whatever arrived while the pointer was down. Posted, so it runs after
    // this handler - including the click forwarded below, which has to resolve
    // its index against the layout the user actually clicked.
    if (g_refreshPending) {
        RequestEmbeddedRefresh();
    }
    releaseCapture();

    // A drag is not also a click; forwarding one here would activate whatever
    // icon happened to be dropped on. That holds for a drag that ended badly
    // too - see CellCaptureLost.
    if (wasDragging || suppressed) {
        if (wasDragging) {
            if (auto panel = g_drag.panel.get()) {
                CommitVisualOrder(panel);
            }
        }
        // Dropped off the row: this is how an icon leaves for the primary tray,
        // and the choice is remembered like any other.
        if (wasOutside) {
            const std::wstring key = KeyOfSerial(SerialOfCell(border));
            if (!key.empty()) {
                Wh_Log(L"[xaml][drag] dropped off the row: to the primary tray");
                SplitTray::MoveIconToTray(key, SplitTray::Destination::Primary);
                RefreshEmbeddedTray();
            }
        }
        return;
    }

    const uint64_t serial = SerialOfCell(border);
    if (!serial) {
        return;
    }

    UINT down = 0;
    UINT up = 0;
    switch (button) {
        case PointerButton::Left:
            if (shift) {
                // The quick way across: no menu, no window. Shift+right is
                // already the mod's menu, so Shift+left being the mod's move
                // keeps the pair together.
                const std::wstring key = KeyOfSerial(serial);
                if (!key.empty()) {
                    SplitTray::MoveIconToTray(key, SplitTray::Destination::Primary);
                    RefreshEmbeddedTray();
                }
                return;
            }
            down = WM_LBUTTONDOWN;
            up = WM_LBUTTONUP;
            break;
        case PointerButton::Right:
            if (shift) {
                // The mod's own menu, not the application's.
                ShowTrayContextMenu(border, serial, monitor);
                return;
            }
            down = WM_RBUTTONDOWN;
            up = WM_RBUTTONUP;
            break;
        case PointerButton::Middle:
            down = WM_MBUTTONDOWN;
            up = WM_MBUTTONUP;
            break;
        default:
            return;
    }

    const POINT point = CursorPoint();
    SplitTray::ForwardClick(serial, down, point);
    SplitTray::ForwardClick(serial, up, point);

    // A click in the overflow popup closes it, as the native one does: the
    // application is about to open a window or a menu of its own, and the
    // popup would otherwise sit over it.
    if (!draggable) {
        HideOverflowFlyout();
    }
}

void CellDoubleTapped(wuxc::Border const& border) {
    if (const uint64_t serial = SerialOfCell(border)) {
        SplitTray::ForwardClick(serial, WM_LBUTTONDBLCLK, CursorPoint());
    }
}

wuxc::Border MakeCell(EmbeddedTray const& tray,
                      SplitTray::CellSnapshot const& entry,
                      bool draggable) {
    const HMONITOR monitor = tray.monitor;
    const uint64_t serial = entry.serial;
    wuxc::Border cell;
    cell.Width(tray.cellWidthDip);
    cell.Height(tray.cellHeightDip);
    cell.Background(TransparentBrush());  // a null background does not hit test
    // The hover and pressed washes are rounded, as the native tray's are.
    cell.CornerRadius(wux::CornerRadius{4, 4, 4, 4});
    cell.Tag(winrt::box_value(static_cast<int64_t>(serial)));

    wuxc::Button face;
    if (auto templ = FaceTemplate()) {
        face.Template(templ);
    }
    face.IsHitTestVisible(false);  // the cell handles the pointer
    face.IsTabStop(true);
    face.UseSystemFocusVisuals(true);
    face.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    face.VerticalAlignment(wux::VerticalAlignment::Stretch);
    wux::Automation::AutomationProperties::SetName(face, winrt::hstring{entry.name});
    if (auto image = MakeCellImage(tray, entry.icon.get())) {
        face.Content(image);
    }
    // Enter or Space, or a screen reader invoking it: the keyboard's selection.
    face.Click([serial](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        SplitTray::ForwardKey(serial, false, AnchorOfSerial(serial));
    });
    // Shift+F10 or the menu key. In the taskbar, XAML turns neither into
    // ContextRequested: checked in Explorer, neither reached the application
    // (2026-09-26). So the keys are taken as they come, and ContextRequested
    // is kept for a host that does raise it.
    face.KeyDown([serial](wf::IInspectable const&,
                          wux::Input::KeyRoutedEventArgs const& args) {
        const int key = static_cast<int>(args.Key());
        const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        if (key == VK_APPS || (key == VK_F10 && shift)) {
            args.Handled(true);
            SendKeyboardMenu(serial);
        }
    });
    face.ContextRequested([serial](wux::UIElement const&,
                                   wux::Input::ContextRequestedEventArgs const& args) {
        args.Handled(true);
        SendKeyboardMenu(serial);
    });
    cell.Child(face);

    if (!entry.tip.empty()) {
        wuxc::ToolTipService::SetToolTip(
            cell, winrt::box_value(winrt::hstring{entry.tip}));
    }

    // The pointer over an icon is its application's news too (DECISIONS 79).
    cell.PointerEntered([](wf::IInspectable const& sender,
                           wux::Input::PointerRoutedEventArgs const&) {
        if (auto border = sender.try_as<wuxc::Border>()) {
            CellPointerEntered(border, CursorPoint());
        }
    });

    cell.PointerMoved([](wf::IInspectable const& sender,
                         wux::Input::PointerRoutedEventArgs const&) {
        if (auto border = sender.try_as<wuxc::Border>()) {
            CellPointerMoved(border, CursorPoint());
        }
    });

    cell.PointerExited([](wf::IInspectable const& sender,
                          wux::Input::PointerRoutedEventArgs const&) {
        if (auto border = sender.try_as<wuxc::Border>()) {
            CellPointerExited(border);
        }
    });

    cell.PointerCaptureLost([](wf::IInspectable const& sender,
                               wux::Input::PointerRoutedEventArgs const&) {
        if (auto border = sender.try_as<wuxc::Border>()) {
            CellCaptureLost(border);
        }
    });

    cell.PointerPressed([draggable](wf::IInspectable const& sender,
                                    wux::Input::PointerRoutedEventArgs const& args) {
        auto border = sender.try_as<wuxc::Border>();
        if (!border) {
            return;
        }
        // Handled here as well as on release: an unhandled press reaches the
        // taskbar underneath and opens the clock flyout.
        args.Handled(true);
        double rowX = 0;
        bool located = false;
        try {
            if (auto row = wuxm::VisualTreeHelper::GetParent(border)
                               .try_as<wuxc::StackPanel>()) {
                rowX = args.GetCurrentPoint(row).Position().X;
                located = true;
            }
        } catch (...) {
        }
        if (!CellPressed(border, draggable, rowX)) {
            return;
        }
        // Without capture the pointer stops reporting the moment it leaves this
        // 32-DIP cell, so a failure here is the whole gesture failing.
        // Measured rather than assumed.
        try {
            if (!located || !border.CapturePointer(args.Pointer())) {
                g_drag.pointerDown = false;
                Wh_Log(L"[xaml][drag] CapturePointer refused; this icon cannot be "
                       L"dragged");
            }
        } catch (...) {
            g_drag.pointerDown = false;
            Wh_Log(L"[xaml][drag] CapturePointer threw: %08X", winrt::to_hresult());
        }
    });

    if (draggable) {
        cell.PointerMoved([](wf::IInspectable const& sender,
                             wux::Input::PointerRoutedEventArgs const& args) {
            auto border = sender.try_as<wuxc::Border>();
            if (!border) {
                return;
            }
            double x = 0;
            double y = 0;
            if (auto panel = g_drag.panel.get(); g_drag.pointerDown && panel) {
                try {
                    const auto position = args.GetCurrentPoint(panel).Position();
                    x = position.X;
                    y = position.Y;
                } catch (...) {
                    return;
                }
            }
            if (CellDragged(border, x, y)) {
                args.Handled(true);
            }
        });
    }

    // `draggable` is false only for cells in the overflow popup.
    cell.PointerReleased([draggable, monitor](
                             wf::IInspectable const& sender,
                             wux::Input::PointerRoutedEventArgs const& args) {
        auto border = sender.try_as<wuxc::Border>();
        if (!border) {
            return;
        }
        args.Handled(true);
        PointerButton button = PointerButton::Other;
        try {
            using winrt::Windows::UI::Input::PointerUpdateKind;
            switch (args.GetCurrentPoint(nullptr).Properties().PointerUpdateKind()) {
                case PointerUpdateKind::LeftButtonReleased:
                    button = PointerButton::Left;
                    break;
                case PointerUpdateKind::RightButtonReleased:
                    button = PointerButton::Right;
                    break;
                case PointerUpdateKind::MiddleButtonReleased:
                    button = PointerButton::Middle;
                    break;
                default:
                    break;
            }
        } catch (...) {
        }
        CellReleased(border, draggable, monitor, button, ShiftHeld(), [&] {
            try {
                border.ReleasePointerCapture(args.Pointer());
            } catch (...) {
            }
        });
    });

    cell.DoubleTapped([](wf::IInspectable const& sender,
                         wux::Input::DoubleTappedRoutedEventArgs const& args) {
        args.Handled(true);
        if (auto border = sender.try_as<wuxc::Border>()) {
            CellDoubleTapped(border);
        }
    });

    return cell;
}

// ---------------------------------------------------------------------------
// The chevron and its flyout
// ---------------------------------------------------------------------------

// One popup for every tray: only one can be open at a time. Never destroyed by
// the runtime, like g_embeddedTrays.
[[clang::no_destroy]] wuxc::Flyout g_overflowFlyout{nullptr};

void HideOverflowFlyout() {
    if (g_overflowFlyout) {
        try {
            g_overflowFlyout.Hide();
        } catch (...) {
        }
    }
}

// What the overflow popup holds: a grid of square cells, as the native
// overflow is, at most five to a row, so a handful of icons makes a compact
// block rather than a strip.
wuxc::VariableSizedWrapGrid OverflowContent(EmbeddedTray const& tray) {
    constexpr double kOverflowCellDip = 40;
    constexpr int kOverflowColumns = 5;
    wuxc::VariableSizedWrapGrid content;
    content.Orientation(wuxc::Orientation::Horizontal);
    content.ItemWidth(kOverflowCellDip);
    content.ItemHeight(kOverflowCellDip);
    content.MaximumRowsOrColumns(std::min<int>(
        kOverflowColumns, std::max<int>(1, static_cast<int>(tray.hiddenEntries.size()))));
    for (const auto& entry : tray.hiddenEntries) {
        // Not draggable in the flyout: ordering happens in the tray itself,
        // where there is a row to drag along.
        auto cell = MakeCell(tray, entry, false);
        cell.Width(kOverflowCellDip);
        cell.Height(kOverflowCellDip);
        content.Children().Append(cell);
    }
    return content;
}

// The chevron was clicked: the overflow popup, or with Shift the mod's menu.
void ChevronReleased(wux::FrameworkElement const& chevron, HMONITOR monitor, bool shift) {
    EmbeddedTray* tray = TrayOfMonitor(monitor);
    if (!tray || tray->hiddenEntries.empty()) {
        return;
    }
    if (shift) {
        ShowTrayContextMenu(chevron, 0, monitor);
        return;
    }

    // Held in a global rather than built fresh on the stack. A flyout that goes
    // out of scope as the handler returns is why it appeared for a moment and
    // vanished.
    if (!g_overflowFlyout) {
        g_overflowFlyout = wuxc::Flyout();

        // Its own window. By default a Flyout is drawn inside the XAML root it
        // belongs to, and this one belongs to the taskbar - an island about 48
        // DIP tall. The popup was squeezed into that strip, which is why it came
        // out as a wide box with the icon pushed against the bottom edge and
        // cut off. The mod's MenuFlyout never had the problem because menus
        // default to a window of their own.
        g_overflowFlyout.ShouldConstrainToRootBounds(false);

        // The presenter's defaults are for a flyout with prose in it: a minimum
        // width, generous padding, square corners. None of that suits a few
        // icons.
        wux::Style presenter{winrt::xaml_typename<wuxc::FlyoutPresenter>()};
        auto setters = presenter.Setters();
        setters.Append(wux::Setter(wuxc::Control::PaddingProperty(),
                                   winrt::box_value(wux::Thickness{4, 4, 4, 4})));
        setters.Append(
            wux::Setter(wux::FrameworkElement::MinWidthProperty(), winrt::box_value(0.0)));
        setters.Append(
            wux::Setter(wux::FrameworkElement::MinHeightProperty(), winrt::box_value(0.0)));
        setters.Append(wux::Setter(wuxc::Control::CornerRadiusProperty(),
                                   winrt::box_value(wux::CornerRadius{8, 8, 8, 8})));
        g_overflowFlyout.FlyoutPresenterStyle(presenter);

        // Standard, not Transient: the pointer sequence that opened it would
        // otherwise light-dismiss it immediately.
        g_overflowFlyout.ShowMode(wuxc::Primitives::FlyoutShowMode::Standard);
        g_overflowFlyout.Placement(wuxc::Primitives::FlyoutPlacementMode::Top);
    }
    g_overflowFlyout.Content(OverflowContent(*tray));
    try {
        g_overflowFlyout.ShowAt(chevron);
    } catch (...) {
        Wh_Log(L"[xaml] could not show the overflow flyout: %08X", winrt::to_hresult());
    }
}

wuxc::Border MakeChevron(EmbeddedTray const& tray) {
    const HMONITOR monitor = tray.monitor;
    wuxc::Border chevron;
    chevron.Width(tray.cellWidthDip);
    chevron.Height(tray.cellHeightDip);
    chevron.Background(TransparentBrush());
    chevron.CornerRadius(wux::CornerRadius{4, 4, 4, 4});

    wuxc::TextBlock glyph;
    glyph.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    glyph.Text(L"");  // chevron up, as the native "Show hidden icons" uses
    glyph.FontSize(tray.iconSizeDip * 0.75);
    glyph.HorizontalAlignment(wux::HorizontalAlignment::Center);
    glyph.VerticalAlignment(wux::VerticalAlignment::Center);
    glyph.IsHitTestVisible(false);
    // Read as what it does, not as the glyph's code point (DECISIONS 81).
    wux::Automation::AutomationProperties::SetName(glyph, L"Show hidden icons");
    chevron.Child(glyph);

    wuxc::ToolTipService::SetToolTip(chevron,
                                     winrt::box_value(winrt::hstring{L"Show hidden icons"}));

    chevron.PointerEntered([](wf::IInspectable const& sender,
                              wux::Input::PointerRoutedEventArgs const&) {
        if (auto border = sender.try_as<wuxc::Border>()) {
            border.Background(HoverBrush());
        }
    });
    chevron.PointerExited([](wf::IInspectable const& sender,
                             wux::Input::PointerRoutedEventArgs const&) {
        if (auto border = sender.try_as<wuxc::Border>()) {
            border.Background(TransparentBrush());
        }
    });
    chevron.PointerPressed([](wf::IInspectable const&,
                              wux::Input::PointerRoutedEventArgs const& args) {
        args.Handled(true);
    });
    chevron.PointerReleased([monitor](wf::IInspectable const& sender,
                                      wux::Input::PointerRoutedEventArgs const& args) {
        args.Handled(true);
        if (auto border = sender.try_as<wux::FrameworkElement>()) {
            ChevronReleased(border, monitor, ShiftHeld());
        }
    });

    return chevron;
}

// ---------------------------------------------------------------------------
// Rebuilding the tray
// ---------------------------------------------------------------------------

// Swaps a cell's picture, name and tooltip without replacing the cell. A
// picture taken away is taken away; one that could not be copied, or has not
// changed, is left as it is (CellPictureOf, DECISIONS 76, 82). Returns whether
// the cell now shows the entry's picture revision, having drawn or cleared it.
bool UpdateCellInPlace(EmbeddedTray const& tray, wuxc::Border const& cell,
                       SplitTray::CellSnapshot const& entry) {
    const std::wstring& tip = entry.tip;
    wuxc::Button face = FaceOfCell(cell);
    bool drawn = false;
    const SplitTray::CellPicture picture = SplitTray::CellPictureOf(entry);
    if (picture == SplitTray::CellPicture::Clear) {
        if (face) {
            face.Content(nullptr);
        }
        drawn = true;
    } else if (picture == SplitTray::CellPicture::Replace) {
        if (auto bitmap = IconToBitmap(entry.icon.get())) {
            if (auto image = face ? face.Content().try_as<wuxc::Image>() : nullptr) {
                image.Source(bitmap);
                drawn = true;
            } else if (auto fresh = MakeCellImage(tray, entry.icon.get()); fresh && face) {
                face.Content(fresh);
                drawn = true;
            }
        }
    }
    if (face && wux::Automation::AutomationProperties::GetName(face) != entry.name) {
        wux::Automation::AutomationProperties::SetName(face, winrt::hstring{entry.name});
    }
    // Only when the text changed: setting a tooltip closes one that is open,
    // and SystemInformer's change every second.
    std::wstring current;
    try {
        if (auto boxed = wuxc::ToolTipService::GetToolTip(cell)) {
            current = winrt::unbox_value_or<winrt::hstring>(boxed, L"").c_str();
        }
    } catch (...) {
    }
    if (current != tip) {
        wuxc::ToolTipService::SetToolTip(
            cell, tip.empty() ? nullptr : winrt::box_value(winrt::hstring{tip}));
    }
    return drawn;
}

// Redraws one display's tray from the icon store.
//
// This used to clear the panel and rebuild every cell on every call - and it is
// called on every change to any icon in the tray. SystemInformer redraws four
// live graphs every second, so the cell under the pointer was being destroyed
// several times a second. That ends a press before it can become a drag (a
// destroyed element loses pointer capture), closes tooltips as they open, and
// would leave an open overflow popup pointing at the wrong icons. It is the most
// likely reason dragging on the taskbar never did anything.
//
// So a change of picture or tooltip is applied to the existing cells, and the
// panel is only rebuilt when the layout itself changes - which icons, in which
// order, on the bar or in the overflow. Nothing is rebuilt while the pointer is
// down; the refresh waits for the release.
void RefreshTray(EmbeddedTray& tray) {
    auto panel = tray.panel.get();
    if (!panel) {
        return;
    }
    if (g_drag.pointerDown || g_drag.dragging) {
        g_refreshPending = true;
        return;
    }

    using Entry = SplitTray::CellSnapshot;
    // Pictures the cells already show are not copied again (DECISIONS 82).
    std::vector<Entry> entries = SplitTray::CellSnapshotsOf(tray.number, &tray.drawnPictures);
    // A cell made afresh, or one in the overflow popup, draws its picture
    // whether it changed or not.
    auto withPicture = [](Entry& entry) {
        if (entry.pictureUnchanged) {
            entry.icon = SplitTray::PictureOfSerial(entry.serial);
            entry.pictureUnchanged = false;
        }
    };
    int maxVisible = 0;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        maxVisible = g_settings.maxVisibleIcons;
    }

    LoadIconOrder();
    // Stable sort: icons the user has never moved keep the order the store gave
    // them, which is the order they registered in.
    std::stable_sort(entries.begin(), entries.end(),
                     [](Entry const& a, Entry const& b) {
                         return OrderPositionOf(a.key) < OrderPositionOf(b.key);
                     });

    std::vector<std::wstring> keys;
    std::vector<uint64_t> all;
    for (const auto& entry : entries) {
        keys.push_back(entry.key);
        all.push_back(entry.serial);
    }

    // Two reasons an icon is in the chevron: the user put it there, or the row
    // ran out of space - in that order (SplitBarAndOverflow).
    const SplitTray::BarSplit split = SplitTray::SplitBarAndOverflow(
        keys, [](std::wstring const& key) { return SplitTray::IsIconHidden(key); },
        maxVisible);
    const auto& shown = split.shown;
    const auto& overflow = split.overflow;

    tray.hiddenEntries.clear();
    std::vector<uint64_t> shownSerials;
    std::vector<uint64_t> overflowSerials;
    for (size_t i : overflow) {
        overflowSerials.push_back(entries[i].serial);
        // Kept for the popup, which is opened later; a shown icon is drawn now.
        withPicture(entries[i]);
        tray.drawnPictures.erase(entries[i].serial);
        tray.hiddenEntries.push_back(std::move(entries[i]));
    }
    for (size_t i : shown) {
        shownSerials.push_back(entries[i].serial);
    }

    try {
        auto children = panel.Children();

        const uint32_t chevronSlots = overflow.empty() ? 0 : 1;
        uint32_t expectedChildren =
            chevronSlots + static_cast<uint32_t>(shown.size());
        if (expectedChildren == 0) {
            expectedChildren = 1;  // the handle
        }
        const bool sameLayout = all == tray.drawnAll &&
                                shownSerials == tray.drawnShown &&
                                overflowSerials == tray.drawnOverflow &&
                                children.Size() == expectedChildren;
        if (sameLayout) {
            for (size_t n = 0; n < shown.size(); n++) {
                auto cell = children.GetAt(chevronSlots + static_cast<uint32_t>(n))
                                .try_as<wuxc::Border>();
                if (cell && UpdateCellInPlace(tray, cell, entries[shown[n]])) {
                    tray.drawnPictures[entries[shown[n]].serial] =
                        entries[shown[n]].pictureRevision;
                }
            }
            return;
        }

        // The layout changed, so an open overflow popup may now hold icons that
        // have moved on. Close it rather than leave it pointing at them.
        HideOverflowFlyout();
        tray.drawnAll = all;
        tray.drawnShown = shownSerials;
        tray.drawnOverflow = overflowSerials;

        children.Clear();

        // The chevron goes first, as it does in the native tray.
        if (!tray.hiddenEntries.empty()) {
            children.Append(MakeChevron(tray));
        }
        tray.drawnPictures.clear();
        for (size_t i : shown) {
            withPicture(entries[i]);
            children.Append(MakeCell(tray, entries[i], true));
            // Drawn, unless there was a picture and copying it failed: the next
            // refresh tries again.
            if (!entries[i].hasPicture || entries[i].icon.get()) {
                tray.drawnPictures[entries[i].serial] = entries[i].pictureRevision;
            }
        }
        // An empty tray still needs somewhere to click, or the mod is invisible
        // and unreachable until a rule happens to match something.
        if (children.Size() == 0) {
            children.Append(MakeTrayHandle(tray));
        }

        // Every rebuild is logged. They should be rare - a change of which
        // icons, their order, or bar versus overflow - and a line a second here
        // would mean the in-place path is not being taken.
        Wh_Log(L"[xaml] tray %d rebuilt: %zu icon(s), %zu shown, %zu hidden",
               tray.number, entries.size(), shown.size(), tray.hiddenEntries.size());
    } catch (...) {
        Wh_Log(L"[xaml] refreshing tray %d failed: %08X", tray.number,
               winrt::to_hresult());
    }
}

// Redraws every display's tray. Called whenever the store changes, and cheap
// when nothing about the layout did (RefreshTray).
void RefreshEmbeddedTray() {
    if (g_drag.pointerDown || g_drag.dragging) {
        g_refreshPending = true;
        return;
    }
    g_refreshPending = false;
    SyncEmbeddedTrays();
    for (auto& tray : g_embeddedTrays) {
        if (tray->active) {
            RefreshTray(*tray);
        }
    }
}

// ---------------------------------------------------------------------------
// Where an icon is on screen
// ---------------------------------------------------------------------------

// An element's bounds in screen pixels. The taskbar's XAML island fills the
// taskbar window - measured on this machine, its content bridge has exactly
// the window's rect on both taskbars - so the window's corner is the island's.
bool ElementScreenRect(EmbeddedTray const& tray, wux::FrameworkElement const& element,
                       RECT* out) {
    RECT window;
    if (!element || !tray.taskbarWnd || !GetWindowRect(tray.taskbarWnd, &window)) {
        return false;
    }
    try {
        auto root = element.XamlRoot();
        if (!root) {
            return false;
        }
        const wf::Rect bounds = element.TransformToVisual(nullptr).TransformBounds(
            wf::Rect{0, 0, static_cast<float>(element.ActualWidth()),
                     static_cast<float>(element.ActualHeight())});
        if (bounds.Width <= 0 || bounds.Height <= 0) {
            return false;
        }
        *out = SplitTray::IslandBoundsToScreen(POINT{window.left, window.top}, bounds.X,
                                               bounds.Y, bounds.Width, bounds.Height,
                                               root.RasterizationScale());
        return true;
    } catch (...) {
        return false;
    }
}

bool IconScreenRect(uint64_t serial, RECT* out) {
    for (auto& tray : g_embeddedTrays) {
        auto panel = tray->panel.get();
        if (!tray->active || !panel) {
            continue;
        }
        try {
            auto children = panel.Children();
            for (uint32_t i = 0; i < children.Size(); i++) {
                auto cell = children.GetAt(i).try_as<wux::FrameworkElement>();
                if (cell && SerialOfCell(cell) == serial) {
                    return ElementScreenRect(*tray, cell, out);
                }
            }
            // Not on the bar, so in the overflow, whose chevron goes first. A
            // click there closes the popup before the application gets to ask,
            // so the chevron is where the icon is by then.
            const bool hidden = std::any_of(
                tray->hiddenEntries.begin(), tray->hiddenEntries.end(),
                [serial](SplitTray::CellSnapshot const& entry) {
                    return entry.serial == serial;
                });
            if (hidden && children.Size() > 0) {
                return ElementScreenRect(
                    *tray, children.GetAt(0).try_as<wux::FrameworkElement>(), out);
            }
        } catch (...) {
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Attaching without waiting to be handed an element
//
// The mod used to reach the taskbar only through the IconView constructor hook,
// which sees elements built after the hook is installed and nothing that already
// exists. Resolving SystemTray.dll's symbols takes several seconds - measured at
// 8.2s after Wh_ModInit on this machine - and by then the secondary taskbar's
// tray is built and loaded. No element on it ever came through, EnsureEmbeddedPanel
// was never called, and the mod sat there with a healthy log and no tray. It had
// worked before only by winning that race.
//
// So the tree is walked downwards from the target taskbar's XamlRoot instead,
// which needs nothing to happen first, and it is retried on the timer.
// ---------------------------------------------------------------------------

// Breadth-first, so the shallowest matches come first, and bounded: the taskbar
// tree is not deep and an unbounded walk over a live tree is not worth the risk.
void CollectDescendants(wux::DependencyObject const& root,
                        std::vector<wux::FrameworkElement>* out,
                        size_t limit) {
    std::vector<wux::DependencyObject> level{root};
    for (int depth = 0; depth < 24 && !level.empty() && out->size() < limit;
         depth++) {
        std::vector<wux::DependencyObject> next;
        for (const auto& node : level) {
            int count = 0;
            try {
                count = wuxm::VisualTreeHelper::GetChildrenCount(node);
            } catch (...) {
                continue;
            }
            for (int i = 0; i < count && out->size() < limit; i++) {
                wux::DependencyObject child = nullptr;
                try {
                    child = wuxm::VisualTreeHelper::GetChild(node, i);
                } catch (...) {
                    continue;
                }
                if (!child) {
                    continue;
                }
                if (auto element = child.try_as<wux::FrameworkElement>()) {
                    out->push_back(element);
                }
                next.push_back(child);
            }
        }
        level = std::move(next);
    }
}

bool g_loggedAttachCandidates = false;

void TryAttachTray(EmbeddedTray& tray) {
    if (tray.active) {
        if (auto existing = tray.panel.get()) {
            if (wuxm::VisualTreeHelper::GetParent(existing)) {
                return;
            }
        }
        // The taskbar was rebuilt under us.
        tray.panel = nullptr;
        tray.drawnAll.clear();
        tray.drawnShown.clear();
        tray.drawnOverflow.clear();
        SetTrayEmbedded(tray, false);
    }

    auto targetRoot = EnsureTargetXamlRoot(tray);
    if (!targetRoot) {
        return;  // no taskbar on that display, or not built yet
    }

    wux::UIElement content = nullptr;
    try {
        content = targetRoot.Content();
    } catch (...) {
        return;
    }
    if (!content) {
        return;
    }

    std::vector<wux::FrameworkElement> all;
    try {
        CollectDescendants(content, &all, 600);
    } catch (...) {
        Wh_Log(L"[xaml] walking the target island failed: %08X",
               winrt::to_hresult());
        return;
    }

    // Anything the tray is made of can anchor it: FindTrayRow walks up from it,
    // and the sizing comes from its height. Tried icons first and the frame
    // last (AnchorPreference). The comment said so before the code did: they
    // were tried in tree order, which puts the frame first.
    std::vector<wux::FrameworkElement> candidates;
    for (const auto& element : all) {
        const std::wstring className = ClassNameOf(element);
        if (className.rfind(L"SystemTray.", 0) == 0) {
            candidates.push_back(element);
        }
    }
    std::stable_sort(candidates.begin(), candidates.end(),
                     [](wux::FrameworkElement const& a, wux::FrameworkElement const& b) {
                         const winrt::hstring nameA = a.Name();
                         const winrt::hstring nameB = b.Name();
                         return SplitTray::AnchorPreference(ClassNameOf(a), nameA) <
                                SplitTray::AnchorPreference(ClassNameOf(b), nameB);
                     });

    if (!g_loggedAttachCandidates) {
        g_loggedAttachCandidates = true;
        Wh_Log(L"[xaml] attach: %zu element(s) in the target island, "
               L"%zu SystemTray.* candidate(s)", all.size(), candidates.size());
        for (size_t i = 0; i < candidates.size() && i < 24; i++) {
            Wh_Log(L"[xaml]   candidate %zu: %s name='%s' %.1fx%.1f", i,
                   ClassNameOf(candidates[i]).c_str(),
                   candidates[i].Name().c_str(),
                   candidates[i].ActualWidth(), candidates[i].ActualHeight());
        }
        if (candidates.empty()) {
            // Nothing recognisable: say what is actually there rather than
            // failing silently, which is how this went unnoticed once already.
            Wh_Log(L"[xaml] ---- target island subtree ----");
            DumpSubtree(content, 0, 14);
            Wh_Log(L"[xaml] ---- end ----");
        }
    }

    // Try them in turn. One that yields no row costs nothing; betting the whole
    // attach on a single guess is what needed fixing.
    for (const auto& candidate : candidates) {
        if (candidate.ActualHeight() <= 0) {
            continue;
        }
        if (EnsureEmbeddedPanel(tray, candidate)) {
            Wh_Log(L"[xaml] tray %d attached from the tree walk, anchored on %s",
                   tray.number, ClassNameOf(candidate).c_str());
            RefreshTray(tray);
            return;
        }
    }
}

// Every display's tray that is not in its taskbar yet. Posted by the tray
// thread's timer while any is waiting (AnyDisplayTrayWaitingToEmbed).
void TryAttachEmbeddedTray() {
    SyncEmbeddedTrays();
    for (auto& tray : g_embeddedTrays) {
        TryAttachTray(*tray);
    }
}

bool AnyDisplayTrayWaitingToEmbed() {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_settings.embedInTaskbar) {
        return false;
    }
    for (const auto& tray : SplitTray::g_trays) {
        if (tray.forDisplay && tray.available &&
            !SplitTray::g_embeddedMonitors.count(tray.monitor.handle)) {
            return true;
        }
    }
    return false;
}

// Callable from any thread: posts to the window the mod subclasses, which lives
// on the taskbar's UI thread, because XAML objects are thread-affine.
void RequestEmbeddedRefresh() {
    HWND tray = SplitTray::g_shellTrayWnd.load();
    if (tray && IsWindow(tray)) {
        PostMessageW(tray, SplitTray::GetXamlRefreshMessage(), 0, 0);
    }
}

// Called from the Shell_TrayWnd subclass, which runs on this same UI thread,
// whenever the icon store changes.
void OnIconStoreChanged() {
    if (g_unloading.load() || g_embeddedTrays.empty()) {
        return;
    }
    RefreshEmbeddedTray();
}

// Takes a tray's panel back out of its taskbar: when its display goes, when
// embedding is turned off, and when the mod unloads, so no stray element is left
// in Explorer's taskbar until the next restart.
void RemovePanel(EmbeddedTray& tray) {
    auto panel = tray.panel.get();
    tray.panel = nullptr;
    tray.loggedEmbedded = false;
    tray.drawnAll.clear();
    tray.drawnShown.clear();
    tray.drawnOverflow.clear();
    tray.hiddenEntries.clear();
    SetTrayEmbedded(tray, false);
    if (!panel) {
        return;
    }
    try {
        if (auto host = wuxm::VisualTreeHelper::GetParent(panel)
                            .try_as<wuxc::Panel>()) {
            uint32_t index = 0;
            if (host.Children().IndexOf(panel, index)) {
                host.Children().RemoveAt(index);
            }
        }
    } catch (...) {
    }
}

// Called from the tray thread's timer until both modules are present.
void EnsureTaskbarXamlHooked() {
    if (g_unloading.load()) {
        return;
    }
    bool embed;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        embed = g_settings.embedInTaskbar;
    }
    if (!embed) {
        return;
    }
    if (g_taskbarSymbolsHooked.load() && g_systemTraySymbolsHooked.load()) {
        return;
    }

    const bool taskbar = HookTaskbarSymbols();
    const bool systemTray = HookSystemTraySymbols();
    // Resolving symbols takes seconds, and the mod may have begun to unload
    // meanwhile; its hooks are not to be applied after that (DECISIONS 67).
    if ((taskbar || systemTray) && !g_unloading.load()) {
        // Hooks registered after Wh_ModInit have to be applied explicitly.
        Wh_ApplyHookOperations();
    }
}

void FocusIconCell(uint64_t serial) {
    if (!serial) {
        return;
    }
    for (auto& tray : g_embeddedTrays) {
        auto panel = tray->panel.get();
        if (!panel) {
            continue;
        }
        try {
            for (auto child : panel.Children()) {
                auto cell = child.try_as<wuxc::Border>();
                if (cell && SerialOfCell(cell) == serial) {
                    if (auto face = FaceOfCell(cell)) {
                        face.Focus(wux::FocusState::Keyboard);
                    }
                    return;
                }
            }
        } catch (...) {
        }
    }
}

void RemoveEverything() {
    // An application's popup open from one of the mod's cells is closed.
    ClosePopup();
    g_popupWaiting = 0;
    if (g_popupTimer) {
        g_popupTimer.Stop();
        g_popupTimer = nullptr;
    }
    HideOverflowFlyout();
    g_overflowFlyout = nullptr;
    for (auto& tray : g_embeddedTrays) {
        RemovePanel(*tray);
    }
    // Its storage too, which clear() keeps: the runtime never frees it.
    std::vector<std::unique_ptr<EmbeddedTray>>().swap(g_embeddedTrays);
    g_loggedTargetStack = false;
    g_loadedRevokers.clear();  // an empty std::list holds nothing
    // The cells that used it went with their panels.
    g_faceTemplate = nullptr;
    g_faceTemplateTried = false;
}

}  // namespace SplitTrayXaml

#else  // SPLITTRAY_NO_XAML

namespace SplitTrayXaml {

// The symbol table above names the hook, so the test binaries need one. They
// never install it: a test process has no SystemTray.dll.
void* WINAPI IconView_IconView_Hook(void* pThis) {
    return g_IconView_IconView_Original(pThis);
}

}  // namespace SplitTrayXaml

#endif  // SPLITTRAY_NO_XAML

// ============================================================================
// Section 9 - Lifecycle
// ============================================================================

using namespace SplitTray;

BOOL Wh_ModInit() {
    Wh_Log(L"Split Tray initialising");

    // An Explorer that shows folder windows only, beside the one that shows
    // the taskbar: refused before anything starts or is written (DECISIONS 92).
    // One loaded before there was a taskbar draws nothing once it finds it is
    // another's (SyncFloatingTrays).
    if (TaskbarShownNow() == TaskbarShownBy::AnotherProcess) {
        Wh_Log(L"the taskbar belongs to another process: this Explorer shows folder "
               L"windows only, and Split Tray stays out of it");
        return FALSE;
    }

    // Cleared explicitly rather than relying on the initial value: Windhawk can
    // load a mod again in the same process after an unload, and a stale flag would
    // leave every hook path silently disabled.
    g_unloading.store(false);
    g_trayThreadStuck = false;
    g_handedBack.store(false);
    g_handBackStuck = false;
    g_panelsStuck = false;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_settings = LoadSettings();
        RecomputeGeometryLocked();
        // Before any icon arrives and is decided by what is remembered.
        ForgetIconsLongUnseenLocked();
    }

    const auto monitors = EnumerateMonitors();
    for (size_t i = 0; i < monitors.size(); i++) {
        Wh_Log(L"monitor %zu: work area (%d,%d)-(%d,%d) dpi=%u%s", i + 1,
               monitors[i].workArea.left, monitors[i].workArea.top,
               monitors[i].workArea.right, monitors[i].workArea.bottom,
               monitors[i].dpi, monitors[i].primary ? L" [primary]" : L"");
    }
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (const auto& tray : g_trays) {
            Wh_Log(L"tray %d: %s%s", tray.number,
                   TrayLabelLocked(tray.number).c_str(),
                   tray.available ? L"" : L" - icons meant for it wait in the primary "
                                          L"tray");
        }
        if (g_trays.empty()) {
            Wh_Log(L"one display and no extra trays: every icon stays in the "
                   L"primary tray");
        }
    }

    // A taskbar that exists already was announced before the mod arrived, so
    // the mod has to ask for its icons itself; one created from here on will be
    // announced by Explorer (ShouldAskAppsToReRegister).
    g_shellExistedAtLoad.store(FindShellTrayWindow() != nullptr);
    g_attachedBefore.store(false);
    g_missedShellAnnouncement.store(false);
    g_reRegisterSettledFor.store(nullptr);

    // The tray thread first, and the subclass only once it runs. Without it
    // nothing draws the mod's trays, and an icon sent to one would be lost; and
    // Windhawk unloads a mod whose Wh_ModInit fails without calling
    // Wh_ModUninit, so nothing may be left attached to Explorer by then.
    g_trayThreadState.store(TrayThreadState::Starting);
    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, &g_trayThreadId);
    if (!g_trayThread) {
        Wh_Log(L"failed to start the tray thread: %u", GetLastError());
        return FALSE;
    }
    // A few milliseconds. One that takes longer is left to carry on.
    const ULONGLONG deadline = GetTickCount64() + g_trayThreadStartWaitMs;
    while (g_trayThreadState.load() == TrayThreadState::Starting &&
           GetTickCount64() < deadline &&
           WaitForSingleObject(g_trayThread, 10) == WAIT_TIMEOUT) {
    }
    if (g_trayThreadState.load() == TrayThreadState::GaveUp ||
        WaitForSingleObject(g_trayThread, 0) == WAIT_OBJECT_0) {
        Wh_Log(L"the tray thread could not start; Split Tray is not loaded");
        WaitForSingleObject(g_trayThread, INFINITE);
        CloseHandle(g_trayThread);
        g_trayThread = nullptr;
        return FALSE;
    }

    if (g_trayThreadState.load() != TrayThreadState::Running) {
        // Slower than the wait. It attaches from its own timer once it runs,
        // and if it gives up instead nothing was attached (DECISIONS 69).
        Wh_Log(L"the tray thread is still starting; Split Tray attaches once it runs");
    } else if (!SubclassShellTrayWindow()) {
        // Not fatal: the taskbar may still be starting up. The tray thread's timer
        // retries until it appears.
        Wh_Log(L"Shell_TrayWnd not found yet, will retry");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // If the taskbar already exists - which it does when the mod is loaded into a
    // running Explorer - this attaches and collects the existing icons right away.
    // On a cold Explorer start there is nothing to attach to yet, and the tray
    // thread's timer keeps trying.
    EnsureShellTrayWindowSubclassed();

    bool embed;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        embed = g_settings.embedInTaskbar;
    }
    if (embed) {
#ifndef SPLITTRAY_NO_XAML
        // Neither SystemTray.dll nor taskbar.dll is necessarily loaded yet; the
        // tray thread's timer keeps trying.
        SplitTrayXaml::EnsureTaskbarXamlHooked();
#endif
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"settings changed");
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_settings = LoadSettings();
    }
    // The heavy lifting happens on the tray thread, which is the only thread
    // allowed to send replays to Explorer's taskbar thread.
    HWND hWnd = g_trayWnd.load();
    if (hWnd) {
        PostMessageW(hWnd, WM_ST_SETTINGS, 0, 0);
    }
}

// Sends `message` to the taskbar's thread and waits for it until `deadline`.
// Returns whether it was answered in time (DECISIONS 73).
//
// One that was not is posted as well, to be handled once the thread gets to it:
// a sent message that times out before it is handled is dropped, as the tests
// found - the hand-back never came, and the subclass went on swallowing icons
// into trays nothing drew. It is handled by the mod's code, so the module then
// has to stay loaded. The messages sent this way carry nothing, and one handled
// twice - begun as it was sent, and again as posted - does nothing the second
// time.
bool SendToTaskbarBy(HWND taskbar, UINT message, ULONGLONG deadline) {
    const ULONGLONG now = GetTickCount64();
    const DWORD wait = deadline > now ? static_cast<DWORD>(deadline - now) : 0;
    DWORD_PTR result = 0;
    if (SendMessageTimeoutW(taskbar, message, 0, 0, SMTO_NORMAL, wait, &result) != 0 ||
        !IsWindow(taskbar)) {
        return true;
    }
    PostMessageW(taskbar, message, 0, 0);
    return false;
}

// Takes the mod's panels out of the taskbars, on the taskbar's own thread:
// XAML objects belong to it (DECISIONS 37). Returns false if that thread did
// not answer in time (DECISIONS 73).
bool RemoveEmbeddedTrays() {
#ifndef SPLITTRAY_NO_XAML
    if (HWND taskbar = g_shellTrayWnd.load(); taskbar && IsWindow(taskbar)) {
        return SendToTaskbarBy(taskbar, GetXamlRemoveMessage(),
                               GetTickCount64() + g_taskbarWaitMs);
    }
#endif
    return true;
}

// Stops the tray thread and waits for it to end (DECISIONS 59).
//
// The shutdown goes to the thread's window, which does not exist yet when the
// mod is unloaded as soon as it has loaded. It was posted only if the window
// was already there, so an early unload lost it and the thread ran on. It is
// now posted as soon as there is a window to post it to, until the thread has
// ended or the time is up.
bool StopTrayThread() {
    if (!g_trayThread) {
        return true;
    }
    constexpr DWORD kBudgetMs = 5000;
    const ULONGLONG deadline = GetTickCount64() + kBudgetMs;
    bool posted = false;
    for (;;) {
        if (!posted) {
            if (HWND trayWnd = g_trayWnd.load()) {
                posted = PostMessageW(trayWnd, WM_ST_SHUTDOWN, 0, 0) != FALSE;
            }
        }
        if (WaitForSingleObject(g_trayThread, 20) == WAIT_OBJECT_0) {
            break;
        }
        if (GetTickCount64() >= deadline) {
            return false;
        }
    }
    CloseHandle(g_trayThread);
    g_trayThread = nullptr;
    // Its windows let go of what screen readers held of them as they went
    // (DECISIONS 85).
    if (const int held = g_uiaObjects.load()) {
        Wh_Log(L"%d screen-reader object(s) still held as the tray thread ended", held);
    }
    return true;
}

// Hands every icon back to Explorer (DECISIONS 68), and waits until the mod's
// code has left the taskbar's thread (DECISIONS 73). The hand-back runs on that
// thread (HandIconsBackToShell), which then takes the subclass off itself. It
// can arrive inside a round of settling there - Explorer may run a message
// loop while it handles a record - and is then done once that round is over;
// and a call of the subclass may still be under way below it, an application's
// message Explorer was handling when it ran the loop. Returns false if the
// thread was not done in time, not answering or still in the mod's code.
//
// Nothing here waits longer than the budget. The hand-back was asked for with
// SendMessageW, before the wait began, and taking the subclass off from here
// is a message that thread has to answer too, so a taskbar thread that did not
// answer held unloading for as long as it did not. One that was not done in
// time does all of it once it answers, with the module kept loaded for it.
bool HandBackToShell() {
    HWND shellTrayWnd = g_shellTrayWnd.load();
    bool done = true;
    if (shellTrayWnd && IsWindow(shellTrayWnd)) {
        const ULONGLONG deadline = GetTickCount64() + g_taskbarWaitMs;
        SendToTaskbarBy(shellTrayWnd, GetReplayMessage(), deadline);
        auto left = [shellTrayWnd] {
            return g_subclassDepth.load() == 0 &&
                   (g_handedBack.load() || !IsWindow(shellTrayWnd));
        };
        while (!left() && GetTickCount64() < deadline) {
            Sleep(10);
        }
        // With no call of the subclass under way and none to come, what is
        // left of the last is its return; a message answered after it finds
        // the thread out of the mod's code.
        done = left() && SendToTaskbarBy(shellTrayWnd, WM_NULL, deadline);
    }
    g_shellTrayWnd.store(nullptr);
    return done;
}

// Everything unloading does before the store can go: the mod's panels out of
// the taskbars, its thread stopped, and every icon handed back to Explorer.
// The subclass keeps track of icons until that last step (DECISIONS 68).
void PrepareToUnload() {
    g_unloading.store(true);
    g_panelsStuck = !RemoveEmbeddedTrays();
    g_trayThreadStuck = !StopTrayThread();
    g_handBackStuck = !HandBackToShell();
}

// Before Windhawk takes the mod's hooks out (DECISIONS 67). The tray thread
// installs hooks of its own as the taskbar's modules appear
// (EnsureTaskbarXamlHooked), so it is stopped here, while they are all still
// in place, rather than left to install one after they have gone.
void Wh_ModBeforeUninit() {
    PrepareToUnload();
}

void Wh_ModUninit() {
    Wh_Log(L"Split Tray unloading");

    // Done already, by a Windhawk that calls Wh_ModBeforeUninit.
    if (!g_unloading.load()) {
        PrepareToUnload();
    }

    if (g_trayThreadStuck || g_handBackStuck || g_panelsStuck) {
        // The mod's code is still running, or will: the tray thread, or on the
        // taskbar's thread a round of settling with the hand-back after it, or
        // a message sent there that it has not answered yet. Keeping the
        // module loaded until Explorer exits is a leak; unloading it under
        // running code would take Explorer down with it. The store is left as
        // it is, for that code.
        const wchar_t* what =
            g_trayThreadStuck ? L"the tray thread did not stop"
            : g_panelsStuck   ? L"the taskbar did not take the mod's trays out in time"
                              : L"the icons were not all back with Explorer in time";
        HMODULE self = nullptr;
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                   GET_MODULE_HANDLE_EX_FLAG_PIN,
                               reinterpret_cast<LPCWSTR>(&StopTrayThread), &self)) {
            Wh_Log(L"%s; the mod stays loaded until Explorer exits rather than "
                   L"unload code it is still running",
                   what);
        } else {
            const DWORD error = GetLastError();
            Wh_Log(L"%s, and the mod could not keep itself loaded: %lu", what, error);
        }
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        // Both lists own an icon copy now, so both have to be released.
        for (auto* list : {&g_icons, &g_primaryOnly}) {
            for (auto& icon : *list) {
                if (icon.icon) {
                    DestroyIcon(icon.icon);
                    icon.icon = nullptr;
                }
            }
        }
        g_icons.clear();
        g_primaryOnly.clear();
    }

    Wh_Log(L"Split Tray unloaded");
}
