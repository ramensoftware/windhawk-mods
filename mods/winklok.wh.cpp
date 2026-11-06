// ==WindhawkMod==
// @id              winklok
// @name            WinKlok
// @description     A calmer, smarter clock mod for the Windows 11 taskbar. Packed with essential time utilities and a powerful notes panel that seamlessly supports rich text and LaTeX.
// @version         1.0.0
// @author          Shifting Whistler
// @github          https://github.com/shifting-whistler
// @homepage        https://github.com/shifting-whistler
// @license         GPL-3.0-or-later
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -luser32 -lgdi32 -ld2d1 -ldwrite -ldwmapi -lwinmm -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# WinKlok

**A calmer, smarter clock mod for Windows 11 taskbar.**
WinKlok replaces the stock date/time flyout with a fast, native, Fluent-styled panel that
combines a **Calendar, Stopwatch, Timer, Alarm, Notes** and **Settings** in one place, and puts
live stopwatch and timer previews right next to the taskbar clock.

---

![Desktop](https://raw.githubusercontent.com/shifting-whistler/WinKlok/main/Images/desktop%20.jpg)

---

## Highlights

| | |
|---|---|
| **Native look and feel** | Drawn with Direct2D and DirectWrite in the Windows palette, with real acrylic blur, your accent colour, light and dark themes, and Windows-style slide-up animations. |
| **Everything in one panel** | Calendar, Stopwatch, Timer, Alarm, Notes and Settings, one click away from the clock. |
| **Live taskbar previews** | A running stopwatch or timer is shown beside the clock with pause/play and reset buttons. |
| **Smooth scrolling** | Mouse wheel, precision touchpad (tracked from raw two-finger movement with kinetic momentum), touch and pen share smooth scrolling. |
| **Private by design** | No network access, no telemetry, no accounts. Everything is stored locally by Windhawk. |

---

![Panel Layout modes](https://raw.githubusercontent.com/shifting-whistler/WinKlok/main/Images/panelmodetable.jpg)

---

![Panel Tabs](https://raw.githubusercontent.com/shifting-whistler/WinKlok/main/Images/alltabs.png)

---

## Features

### Calendar
* Month, year and decade views, with smooth vertical month scrolling like the Windows calendar.
* Today and the selected date are marked with your accent colour. First day of the week follows Windows.

### Stopwatch
* Centisecond precision, up to 200 laps (the newest lap is always on top) and a smooth lap list.
* Keeps running while the panel is closed and shows a live preview next to the clock.

### Timer
* Hours / minutes / seconds pickers, optional timer names and one-click presets you can save and remove.
* Run several timers at once. The most recently touched timer is shown beside the clock.

### Alarm
* Repeat on any days, custom label, snooze length and one of nine built-in sounds with live audition.
* The sound and snooze length you pick become the defaults for your next alarm.
* If an alarm or timer came due while the PC was off or asleep, it never rings late. One popup lists
  everything you missed, with dates and times.

### Notes
* Any number of notes, saved automatically, with a rich preview of Markdown and LaTeX maths.
* **Paste from ChatGPT, Gemini, Google and the web and keep the structure**: headings, lists,
  tables, block quotes, code blocks, links and equations are converted to Markdown as you paste.
* Links in the preview are clickable (only the link itself), plain URLs are detected automatically.
* One-tap **Copy** button for the whole note body (the title is not included), adjustable text size.

### Settings
* Taskbar previews, preview layout, notification icon, 12/24-hour time (applied everywhere),
  panel material (acrylic or simple) and default timer / alarm sounds.
* **Sync time now** (Windows asks for administrator permission whenever it is needed).
* A compact **About** card with the version, developer and project links.


---

## First-run defaults
Notification bell off, taskbar previews on, time format following Windows, acrylic blur, side-by-side layout, the *Windows Alarm*
tone for both timers and alarms.

## Getting started
1. Install and enable the mod in Windhawk.
2. WinKlok takes over the taskbar clock within a second or so. If the clock does not change, restart
   Windows Explorer from Task Manager.
3. Have fun...

## Tips
* Scroll over the tab bar to reveal more tabs, or click a tab to slide it to the centre.
* Right-click a timer preset to reveal its remove button.
* The notification bell is optional. Turn it off in Settings if you prefer the stock one.

## Compatibility
* Windows 11 with the XAML taskbar, 64-bit only.
* Do not combine with other mods that rebuild the taskbar clock area (for example *Taskbar Clock
  Customization*) until you have verified they coexist.
* WinKlok does not change the taskbar height. Use the *Taskbar height and icon size* mod for that.
* To turn WinKlok off, disable the mod in Windhawk: the taskbar clock goes back to Windows immediately.
* If something misbehaves, enable logging for this mod in Windhawk and check the log.

## For developers
* Single-file C++ mod, organised in blocks: engine and persistence, taskbar integration (XAML
  injection and hooks), native UI primitives, sound synthesis, rich-text and LaTeX engine, panel
  pages and alert windows.
* Rendering is plain Win32 + Direct2D/DirectWrite. There is no browser engine and no external asset.
* Alarm sounds are synthesised at runtime into in-memory WAV buffers, so nothing needs to be shipped.
* The pasted-HTML to Markdown converter lives next to the Notes editor and understands the clipboard
  *HTML Format* produced by modern browsers and chat apps.
* Contributions, bug reports and ideas are welcome on GitHub.

## Credits and licence
Made by **Shifting Whistler**.

* GitHub: https://github.com/shifting-whistler
* WinKlok Repo: https://github.com/shifting-whistler/WinKlok

WinKlok is free and open source software released under the **GNU General Public License v3.0**
(or any later version). You are free to use, study, share and improve it under the terms of that licence.
*/
// ==/WindhawkModReadme==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <winsvc.h>
#include <dwmapi.h>
#include <d2d1.h>
#include <dwrite.h>
#include <mmsystem.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <cstring>
#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>

// ============================================================================
//  WinKlok  -  Copyright (C) 2026 Shifting Whistler
//  Released under the GNU General Public License v3.0 or later (GPL-3.0-or-later).
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//  PARTICULAR PURPOSE. See <https://www.gnu.org/licenses/gpl-3.0.html>.
//
//  SOURCE MAP
//  1. Product info, global state, settings and engine: stopwatch, timers, alarms, persistence
//  2. Taskbar integration: XAML preview injection, click hooks, tooltip removal, taskbar sizing
//  3. Native UI primitives: Direct2D/DirectWrite, palette, acrylic, smooth scrolling, input
//  4. Alarm sound synthesis
//  5. Rich text + LaTeX engine (notes preview)
//  6. Panel pages: calendar, stopwatch, timer, alarms, notes (+ HTML paste), settings, about
//  7. Alert windows, UI-thread lifetime, time sync, and Windhawk entry points
//
//  UI rendering and taskbar/XAML integration stay separate from the engine.
// ============================================================================

// ---- product information ---------------------------------------------------
#define WK_NAME L"WinKlok"
#define WK_VERSION L"1.0.0"
#define WK_DEVELOPER L"Shifting Whistler"
#define WK_COPYRIGHT L"\u00A9 2026 Shifting Whistler"
#define WK_LICENSE L"GPL-3.0-or-later"
#define WK_TAGLINE L"A calmer, smarter clock mod for Windows 11 taskbar."
#define WK_WINDHAWK_URL L"https://windhawk.net/mods/winklok"

static std::atomic<bool> g_shuttingDown{false};  // mod is being unloaded

// The mod's own module handle. Window classes are registered with it (not with Explorer's handle), so they
// belong to this image and can never outlive it.
static HINSTANCE ModInst()
{
    static HINSTANCE h = nullptr;
    if (!h)
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&ModInst, &h);
    return h ? h : GetModuleHandleW(nullptr);
}

static inline bool WkActive()
{
    return !g_shuttingDown.load();
}

// ============================================================================
//  SETTINGS + ENGINE  (no UI; thread-safe; persisted through Windhawk storage)
// ============================================================================
namespace td
{

    static const int kSoundCount = 9, kDefaultAlarmSound = 0, kTimerSound = 0; // 0 = Windows Alarm
    static const wchar_t *const kSoundNames[kSoundCount] = {
        L"Windows Alarm", L"Crystal Bells", L"Soft Chimes", L"Marimba Morning", L"Golden Harp",
        L"Sunrise", L"Digital Pulse", L"Music Box", L"Zen Gong"};

    static bool SystemUses24h()
    {
        DWORD v = 0;
        if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_ITIME | LOCALE_RETURN_NUMBER, (LPWSTR)&v,
                            sizeof(v) / sizeof(WCHAR)) == 0)
            return false;
        return v == 1;
    }

    // ---- UI-affecting settings (persisted as ints) ---------------------------------
    struct Settings
    {
        std::atomic<int> showPreviews{1};
        std::atomic<int> stacked{0}; // 0 = side by side, 1 = stopwatch above timer
        std::atomic<int> showBell{0};
        std::atomic<int> hour24{-1};                     // -1 = follow Windows (default), 0 = 12 h, 1 = 24 h
        std::atomic<int> acrylic{1};                     // 1 = acrylic blur when Windows transparency is on, 0 = simple transparency
        std::atomic<int> timerSound{kTimerSound};        // default sound when a timer ends
        std::atomic<int> alarmSound{kDefaultAlarmSound}; // default sound for new alarms (follows the last saved alarm)
        std::atomic<int> alarmSnooze{10};                // default snooze (minutes) for new alarms (follows the last saved alarm)
        bool sys24 = false;

        bool Use24() const
        {
            int v = hour24.load();
            return v >= 0 ? v != 0 : sys24;
        }
        void Load()
        {
            showPreviews = Wh_GetIntValue(L"showPreviews", 1);
            stacked = Wh_GetIntValue(L"stacked", 0);
            showBell = Wh_GetIntValue(L"showBell", 0);
            hour24 = Wh_GetIntValue(L"hour24", -1);
            acrylic = Wh_GetIntValue(L"acrylic", 1);
            timerSound = std::min(kSoundCount - 1, std::max(0, Wh_GetIntValue(L"timerSound", kTimerSound)));
            alarmSound = std::min(kSoundCount - 1, std::max(0, Wh_GetIntValue(L"alarmSound", kDefaultAlarmSound)));
            alarmSnooze = std::min(60, std::max(1, Wh_GetIntValue(L"alarmSnooze", 10)));
            sys24 = SystemUses24h();
        }
        void Save()
        {
            Wh_SetIntValue(L"showPreviews", showPreviews);
            Wh_SetIntValue(L"stacked", stacked);
            Wh_SetIntValue(L"showBell", showBell);
            Wh_SetIntValue(L"hour24", hour24);
            Wh_SetIntValue(L"acrylic", acrylic);
            Wh_SetIntValue(L"timerSound", timerSound);
            Wh_SetIntValue(L"alarmSound", alarmSound);
            Wh_SetIntValue(L"alarmSnooze", alarmSnooze);
        }
    };
    static Settings g_set;

    static ULONGLONG MonoMs() { return GetTickCount64(); }

    static long long WallMs()
    {
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        ULARGE_INTEGER u;
        u.LowPart = ft.dwLowDateTime;
        u.HighPart = ft.dwHighDateTime;
        return (long long)(u.QuadPart / 10000ULL) - 11644473600000LL;
    }

    static SYSTEMTIME WallToLocal(long long wallMs)
    {
        ULARGE_INTEGER u;
        u.QuadPart = (ULONGLONG)((wallMs + 11644473600000LL) * 10000LL);
        FILETIME ft;
        ft.dwLowDateTime = u.LowPart;
        ft.dwHighDateTime = u.HighPart;
        SYSTEMTIME utc{}, loc{};
        FileTimeToSystemTime(&ft, &utc);
        SystemTimeToTzSpecificLocalTime(nullptr, &utc, &loc);
        return loc;
    }

    static long long LocalToWall(const SYSTEMTIME &loc)
    {
        SYSTEMTIME utc{};
        if (!TzSpecificLocalTimeToSystemTime(nullptr, &loc, &utc))
            return 0;
        FILETIME ft;
        if (!SystemTimeToFileTime(&utc, &ft))
            return 0;
        ULARGE_INTEGER u;
        u.LowPart = ft.dwLowDateTime;
        u.HighPart = ft.dwHighDateTime;
        return (long long)(u.QuadPart / 10000ULL) - 11644473600000LL;
    }

    // Local calendar date + dayOffset, returned as SYSTEMTIME.
    static SYSTEMTIME LocalDatePlusDays(const SYSTEMTIME &base, int dayOffset)
    {
        SYSTEMTIME s = base;
        s.wHour = 12;
        s.wMinute = 0;
        s.wSecond = 0;
        s.wMilliseconds = 0; // noon avoids DST edge cases
        FILETIME ft;
        SystemTimeToFileTime(&s, &ft);
        ULARGE_INTEGER u;
        u.LowPart = ft.dwLowDateTime;
        u.HighPart = ft.dwHighDateTime;
        u.QuadPart += (ULONGLONG)dayOffset * 864000000000ULL;
        ft.dwLowDateTime = u.LowPart;
        ft.dwHighDateTime = u.HighPart;
        SYSTEMTIME out{};
        FileTimeToSystemTime(&ft, &out);
        return out;
    }

    // Next local hh:mm strictly after afterWall. days: bit i = weekday i (Sun=0); 0 = any day.
    static long long NextFire(int hh, int mm, unsigned days, long long afterWall)
    {
        SYSTEMTIME nowLoc = WallToLocal(afterWall);
        for (int d = 0; d <= 8; ++d)
        {
            SYSTEMTIME day = LocalDatePlusDays(nowLoc, d);
            if (days != 0 && !(days & (1u << day.wDayOfWeek)))
                continue;
            SYSTEMTIME c = day;
            c.wHour = (WORD)hh;
            c.wMinute = (WORD)mm;
            c.wSecond = 0;
            c.wMilliseconds = 0;
            long long w = LocalToWall(c);
            if (w > afterWall)
                return w;
        }
        return 0;
    }

    static std::wstring Esc(const std::wstring &s)
    {
        std::wstring o;
        for (wchar_t c : s)
        {
            if (c == L'\\')
                o += L"\\\\";
            else if (c == L'|')
                o += L"\\p";
            else if (c == L'\n')
                o += L"\\n";
            else if (c != L'\r')
                o += c;
        }
        return o;
    }

    static std::wstring Unesc(const std::wstring &s)
    {
        std::wstring o;
        for (size_t i = 0; i < s.size(); ++i)
        {
            wchar_t c = s[i];
            if (c == L'\\' && i + 1 < s.size())
            {
                wchar_t n = s[++i];
                o += n == L'p' ? L'|' : n == L'n' ? L'\n'
                                                  : n;
            }
            else
                o += c;
        }
        return o;
    }

    static std::vector<std::wstring> Split(const std::wstring &s, wchar_t sep)
    {
        std::vector<std::wstring> r;
        size_t st = 0;
        for (;;)
        {
            size_t p = s.find(sep, st);
            if (p == std::wstring::npos)
            {
                r.push_back(s.substr(st));
                break;
            }
            r.push_back(s.substr(st, p - st));
            st = p + 1;
        }
        return r;
    }

    static std::wstring ClampName(std::wstring s, size_t n = 40)
    {
        if (s.size() > n)
            s.resize(n);
        return s;
    }

    static long long ToLL(const std::wstring &s, long long def = 0)
    {
        if (s.empty())
            return def;
        wchar_t *e = nullptr;
        long long v = wcstoll(s.c_str(), &e, 10);
        return (e && *e == 0) ? v : def;
    }

    // ---- formatting -------------------------------------------------------------
    static std::wstring Fmt(const wchar_t *f, int a = 0, int b = 0, int c = 0, int d = 0)
    {
        wchar_t buf[48];
        _snwprintf(buf, 48, f, a, b, c, d);
        buf[47] = 0;
        return buf;
    }

    static std::wstring FmtClock(long long ms)
    { // timer style: MM:SS / H:MM:SS
        if (ms < 0)
            ms = 0;
        long long tot = ms / 1000;
        int h = (int)(tot / 3600), m = (int)((tot / 60) % 60), s = (int)(tot % 60);
        return h > 0 ? Fmt(L"%d:%02d:%02d", h, m, s) : Fmt(L"%02d:%02d", m, s);
    }

    static std::wstring FmtStopwatch(long long ms, bool hundredths)
    {
        if (ms < 0)
            ms = 0;
        long long tot = ms / 1000;
        int h = (int)(tot / 3600), m = (int)((tot / 60) % 60), s = (int)(tot % 60);
        if (hundredths)
        {
            int cs = (int)((ms % 1000) / 10);
            return h > 0 ? Fmt(L"%d:%02d:%02d.%02d", h, m, s, cs) : Fmt(L"%02d:%02d.%02d", m, s, cs);
        }
        return h > 0 ? Fmt(L"%d:%02d:%02d", h, m, s) : Fmt(L"%02d:%02d.%d", m, s, (int)((ms % 1000) / 100));
    }

    static std::wstring FmtDuration(long long ms)
    { // "1 h 5 min 30 s"
        long long tot = ms / 1000;
        int h = (int)(tot / 3600), m = (int)((tot / 60) % 60), s = (int)(tot % 60);
        std::wstring o;
        if (h)
            o += Fmt(L"%d h ", h);
        if (m)
            o += Fmt(L"%d min ", m);
        if (s || o.empty())
            o += Fmt(L"%d s", s);
        while (!o.empty() && o.back() == L' ')
            o.pop_back();
        return o;
    }

    // Time of day honouring the mod-wide 12 h / 24 h setting.
    static std::wstring FmtTimeOfDay(int hh, int mm)
    {
        SYSTEMTIME st{};
        st.wYear = 2020;
        st.wMonth = 1;
        st.wDay = 1;
        st.wHour = (WORD)hh;
        st.wMinute = (WORD)mm;
        wchar_t b[48] = {};
        if (!GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, g_set.Use24() ? L"HH:mm" : L"h:mm tt", b, 48))
            return Fmt(L"%02d:%02d", hh, mm);
        return b;
    }

    // ---- data -------------------------------------------------------------------
    struct TimerItem
    {
        int id = 0;
        std::wstring name;
        long long totalMs = 0;
        bool running = false;
        ULONGLONG endMono = 0;
        long long remainMs = 0; // valid while paused
        long long seq = 0;      // last start/resume order (preview shows highest)
    };

    struct AlarmItem
    {
        int id = 0;
        int hh = 7, mm = 0;
        unsigned days = 0; // bit i = weekday i (Sun=0); 0 = once
        bool enabled = true;
        std::wstring label;
        int snoozeMin = 10;
        int sound = kDefaultAlarmSound;
        long long nextFire = 0; // wall ms; 0 = none
        bool ringing = false;
    };

    struct PresetItem
    {
        std::wstring name;
        long long secs = 0;
    };

    struct RingEvent
    {
        bool isAlarm = false;
        int id = 0;
        std::wstring title, sub;
        int snoozeMin = 10;
        int sound = kTimerSound;
    };

    // An alarm or timer that came due while the device (or WinKlok) was off or asleep.
    struct MissedItem
    {
        bool isAlarm = false;
        std::wstring title;
        long long whenWall = 0; // when it was due
        long long totalMs = 0;  // timers: the set duration
        int count = 1;          // alarms that repeated while away
    };

    struct TimerView
    {
        int id;
        std::wstring name;
        long long totalMs, remainMs, endWall;
        bool running;
    };
    struct PanelSnap
    {
        long long swMs = 0;
        bool swRunning = false;
        std::vector<long long> laps;
        std::vector<TimerView> timers;
        std::vector<AlarmItem> alarms;
        std::vector<PresetItem> presets;
    };
    struct PreviewSnap
    {
        bool swOn = false, swRun = false;
        std::wstring swText;
        bool tmOn = false, tmRun = false;
        std::wstring tmName, tmText;
    };

    struct Engine
    {
        std::mutex m;
        bool swRunning = false;
        ULONGLONG swStart = 0;
        long long swAccum = 0;
        std::vector<long long> laps;
        std::vector<TimerItem> timers;
        std::vector<AlarmItem> alarms;
        std::vector<PresetItem> presets;
        int nextId = 1;
        long long seqCounter = 0;
        std::vector<MissedItem> missed; // collected at load / tick, shown once by the UI as one popup
        ULONGLONG lastBeat = 0;

        long long SwElapsedL(ULONGLONG now) const
        {
            return swAccum + (swRunning ? (long long)(now - swStart) : 0LL);
        }
        static long long RemainL(const TimerItem &t, ULONGLONG now)
        {
            if (!t.running)
                return t.remainMs;
            return now >= t.endMono ? 0LL : (long long)(t.endMono - now);
        }
        const TimerItem *PreviewTimerL() const
        {
            const TimerItem *best = nullptr;
            for (auto &t : timers)
                if (!best || t.seq > best->seq)
                    best = &t;
            return best;
        }

        // ---------------- persistence ----------------
        void SaveL()
        {
            ULONGLONG now = MonoMs();
            std::wstring o = L"v1\n";
            o += L"T|" + std::to_wstring(WallMs()) + L"\n";
            o += L"SW|" + std::to_wstring(SwElapsedL(now)) + L"|" + (swRunning ? L"1" : L"0") + L"\n";
            for (long long l : laps)
                o += L"LAP|" + std::to_wstring(l) + L"\n";
            for (auto &t : timers)
                o += L"TM|" + std::to_wstring(t.id) + L"|" + Esc(t.name) + L"|" + std::to_wstring(t.totalMs) + L"|" +
                     (t.running ? L"1" : L"0") + L"|" + std::to_wstring(RemainL(t, now)) + L"|" + std::to_wstring(t.seq) + L"\n";
            for (auto &p : presets)
                o += L"TP|" + Esc(p.name) + L"|" + std::to_wstring(p.secs) + L"\n";
            for (auto &a : alarms)
                o += L"AL|" + std::to_wstring(a.id) + L"|" + std::to_wstring(a.hh) + L"|" + std::to_wstring(a.mm) + L"|" +
                     std::to_wstring(a.days) + L"|" + (a.enabled ? L"1" : L"0") + L"|" + Esc(a.label) + L"|" +
                     std::to_wstring(a.snoozeMin) + L"|" + std::to_wstring(a.sound) + L"\n";
            Wh_SetStringValue(L"state", o.c_str());
        }

        void Load()
        {
            std::lock_guard<std::mutex> g(m);
            std::vector<wchar_t> buf(32768);
            size_t n = Wh_GetStringValue(L"state", buf.data(), buf.size());
            if (n == 0 || n >= buf.size())
            {
                SeedPresetsL();
                return;
            }
            auto lines = Split(std::wstring(buf.data(), n), L'\n');
            if (lines.empty() || lines[0] != L"v1")
            {
                SeedPresetsL();
                return;
            }

            ULONGLONG now = MonoMs();
            long long wall = WallMs(), delta = 0, savedWall = wall, elapsedRaw = 0, refWall = 0;
            int maxId = 0;
            { // "alive" is a heartbeat written once a minute while something is armed
                wchar_t b[32] = {};
                const size_t bn = Wh_GetStringValue(L"alive", b, 32);
                refWall = bn > 0 && bn < 32 ? ToLL(std::wstring(b, bn), 0) : 0;
            }
            for (size_t i = 1; i < lines.size(); ++i)
            {
                auto f = Split(lines[i], L'|');
                if (f.empty())
                    continue;
                if (f[0] == L"T" && f.size() >= 2)
                {
                    savedWall = ToLL(f[1], wall);
                    elapsedRaw = std::max(0LL, wall - savedWall);
                    refWall = std::min(wall, std::max(refWall, savedWall)); // last moment WinKlok is known to have been running
                    delta = wall - savedWall;
                    if (delta < 0 || delta > 7LL * 24 * 3600 * 1000)
                        delta = 0;
                }
                else if (f[0] == L"SW" && f.size() >= 3)
                {
                    swAccum = std::max(0LL, ToLL(f[1]));
                    swRunning = f[2] == L"1";
                    if (swRunning)
                    {
                        swAccum += delta;
                        swStart = now;
                    }
                }
                else if (f[0] == L"LAP" && f.size() >= 2)
                {
                    if (laps.size() < 200)
                        laps.push_back(ToLL(f[1]));
                }
                else if (f[0] == L"TM" && f.size() >= 7 && timers.size() < 50)
                {
                    TimerItem t;
                    t.id = (int)ToLL(f[1]);
                    t.name = ClampName(Unesc(f[2]));
                    t.totalMs = std::max(1000LL, ToLL(f[3]));
                    t.running = f[4] == L"1";
                    t.remainMs = std::max(0LL, ToLL(f[5]));
                    t.seq = ToLL(f[6]);
                    maxId = std::max(maxId, t.id);
                    if (t.running && t.remainMs <= elapsedRaw)
                    { // it ended while the device was off: report it, never ring it
                        MissedItem mi;
                        mi.title = t.name.empty() ? L"Timer" : t.name;
                        mi.whenWall = savedWall + t.remainMs;
                        mi.totalMs = t.totalMs;
                        missed.push_back(mi);
                        continue;
                    }
                    t.endMono = t.running ? now + (ULONGLONG)std::max(0LL, t.remainMs - elapsedRaw) : 0;
                    seqCounter = std::max(seqCounter, t.seq);
                    timers.push_back(t);
                }
                else if (f[0] == L"TP" && f.size() >= 3 && presets.size() < 40)
                {
                    presets.push_back({ClampName(Unesc(f[1])), std::max(1LL, ToLL(f[2]))});
                }
                else if (f[0] == L"AL" && f.size() >= 8 && alarms.size() < 40)
                {
                    AlarmItem a;
                    a.id = (int)ToLL(f[1]);
                    a.hh = (int)std::min(23LL, std::max(0LL, ToLL(f[2])));
                    a.mm = (int)std::min(59LL, std::max(0LL, ToLL(f[3])));
                    a.days = (unsigned)(ToLL(f[4]) & 0x7F);
                    a.enabled = f[5] == L"1";
                    a.label = ClampName(Unesc(f[6]));
                    a.snoozeMin = (int)std::min(60LL, std::max(1LL, ToLL(f[7], 10)));
                    a.sound = f.size() >= 9 ? (int)std::min((long long)kSoundCount - 1, std::max(0LL, ToLL(f[8], kDefaultAlarmSound)))
                                            : kDefaultAlarmSound;
                    if (a.enabled && refWall > 0)
                    { // any occurrence between the last known running moment and now was missed
                        const long long first = NextFire(a.hh, a.mm, a.days, refWall);
                        if (first > 0 && first <= wall)
                        {
                            long long last = first;
                            int cnt = 1;
                            if (a.days != 0)
                                for (int k = 0; k < 400; ++k)
                                {
                                    const long long nx = NextFire(a.hh, a.mm, a.days, last);
                                    if (nx <= 0 || nx > wall)
                                        break;
                                    last = nx;
                                    ++cnt;
                                }
                            MissedItem mi;
                            mi.isAlarm = true;
                            mi.title = a.label.empty() ? L"Alarm" : a.label;
                            mi.whenWall = last;
                            mi.count = cnt;
                            missed.push_back(mi);
                            if (a.days == 0)
                                a.enabled = false; // a one-time alarm has had its day
                        }
                    }
                    a.nextFire = a.enabled ? NextFire(a.hh, a.mm, a.days, wall) : 0;
                    maxId = std::max(maxId, a.id);
                    alarms.push_back(a);
                }
            }
            nextId = maxId + 1;
            if (presets.empty())
                SeedPresetsL();
            if (!missed.empty())
                SaveL(); // the missed items are consumed: do not report them again after another reload
        }

        void SeedPresetsL()
        {
            presets.push_back({L"1 min", 60});
            presets.push_back({L"5 min", 300});
            presets.push_back({L"10 min", 600});
            presets.push_back({L"25 min", 1500});
        }

        // ---------------- stopwatch ----------------
        void SwToggle()
        {
            std::lock_guard<std::mutex> g(m);
            ULONGLONG now = MonoMs();
            if (swRunning)
            {
                swAccum = SwElapsedL(now);
                swRunning = false;
            }
            else
            {
                swStart = now;
                swRunning = true;
            }
            SaveL();
        }
        void SwReset()
        {
            std::lock_guard<std::mutex> g(m);
            swRunning = false;
            swAccum = 0;
            swStart = 0;
            laps.clear();
            SaveL();
        }
        void SwLap()
        {
            std::lock_guard<std::mutex> g(m);
            if (!swRunning)
                return;
            if (laps.size() < 200)
                laps.push_back(SwElapsedL(MonoMs()));
            SaveL();
        }

        // ---------------- timers ----------------
        int TimerStart(const std::wstring &name, long long totalMs)
        {
            std::lock_guard<std::mutex> g(m);
            if (totalMs < 1000 || timers.size() >= 50)
                return 0;
            totalMs = std::min(totalMs, 99LL * 3600 * 1000 + 59 * 60000 + 59000);
            TimerItem t;
            t.id = nextId++;
            t.name = ClampName(name);
            t.totalMs = totalMs;
            t.running = true;
            t.endMono = MonoMs() + (ULONGLONG)totalMs;
            t.remainMs = totalMs;
            t.seq = ++seqCounter;
            timers.push_back(t);
            SaveL();
            return t.id;
        }
        void TimerToggle(int id)
        {
            std::lock_guard<std::mutex> g(m);
            ULONGLONG now = MonoMs();
            for (auto &t : timers)
                if (t.id == id)
                {
                    if (t.running)
                    {
                        t.remainMs = RemainL(t, now);
                        t.running = false;
                    }
                    else
                    {
                        t.endMono = now + (ULONGLONG)t.remainMs;
                        t.running = true;
                    }
                    t.seq = ++seqCounter;
                    break;
                }
            SaveL();
        }
        void TimerCancel(int id)
        {
            std::lock_guard<std::mutex> g(m);
            for (size_t i = 0; i < timers.size(); ++i)
                if (timers[i].id == id)
                {
                    timers.erase(timers.begin() + i);
                    break;
                }
            SaveL();
        }

        // ---------------- alarms ----------------
        int AlarmSave(AlarmItem a)
        {
            std::lock_guard<std::mutex> g(m);
            a.label = ClampName(a.label);
            a.hh = std::min(23, std::max(0, a.hh));
            a.mm = std::min(59, std::max(0, a.mm));
            a.days &= 0x7F;
            a.snoozeMin = std::min(60, std::max(1, a.snoozeMin));
            a.sound = std::min(kSoundCount - 1, std::max(0, a.sound));
            a.ringing = false;
            a.nextFire = a.enabled ? NextFire(a.hh, a.mm, a.days, WallMs()) : 0;
            if (a.id == 0)
            {
                if (alarms.size() >= 40)
                    return 0;
                a.id = nextId++;
                alarms.push_back(a);
            }
            else
            {
                bool found = false;
                for (auto &x : alarms)
                    if (x.id == a.id)
                    {
                        bool r = x.ringing;
                        x = a;
                        x.ringing = r;
                        found = true;
                        break;
                    }
                if (!found)
                {
                    a.id = nextId++;
                    alarms.push_back(a);
                }
            }
            std::sort(alarms.begin(), alarms.end(), [](const AlarmItem &l, const AlarmItem &r)
                      { return l.hh != r.hh ? l.hh < r.hh : l.mm < r.mm; });
            SaveL();
            return a.id;
        }
        void AlarmDelete(int id)
        {
            std::lock_guard<std::mutex> g(m);
            for (size_t i = 0; i < alarms.size(); ++i)
                if (alarms[i].id == id)
                {
                    alarms.erase(alarms.begin() + i);
                    break;
                }
            SaveL();
        }
        void AlarmSetEnabled(int id, bool on)
        {
            std::lock_guard<std::mutex> g(m);
            for (auto &a : alarms)
                if (a.id == id)
                {
                    a.enabled = on;
                    a.ringing = false;
                    a.nextFire = on ? NextFire(a.hh, a.mm, a.days, WallMs()) : 0;
                }
            SaveL();
        }
        void AlarmDismiss(int id)
        {
            std::lock_guard<std::mutex> g(m);
            for (auto &a : alarms)
                if (a.id == id)
                {
                    a.ringing = false;
                    if (a.days == 0)
                    {
                        a.enabled = false;
                        a.nextFire = 0;
                    }
                    else
                        a.nextFire = NextFire(a.hh, a.mm, a.days, WallMs());
                }
            SaveL();
        }
        void AlarmSnooze(int id)
        {
            std::lock_guard<std::mutex> g(m);
            for (auto &a : alarms)
                if (a.id == id)
                {
                    a.ringing = false;
                    a.enabled = true;
                    a.nextFire = WallMs() + (long long)a.snoozeMin * 60000LL;
                }
            SaveL();
        }
        void RescheduleAfterTimeChange()
        {
            std::lock_guard<std::mutex> g(m);
            long long wall = WallMs();
            for (auto &a : alarms)
                if (a.enabled && !a.ringing)
                    a.nextFire = NextFire(a.hh, a.mm, a.days, wall);
            SaveL();
        }

        // ---------------- presets ----------------
        void PresetAdd(const std::wstring &name, long long secs)
        {
            std::lock_guard<std::mutex> g(m);
            if (secs < 1 || presets.size() >= 40)
                return;
            presets.push_back({name.empty() ? FmtDuration(secs * 1000) : ClampName(name), secs});
            SaveL();
        }
        void PresetDelete(size_t idx)
        {
            std::lock_guard<std::mutex> g(m);
            if (idx < presets.size())
                presets.erase(presets.begin() + idx);
            SaveL();
        }

        // ---------------- tick / snapshots ----------------
        std::vector<RingEvent> Tick()
        {
            std::vector<RingEvent> ev;
            std::lock_guard<std::mutex> g(m);
            ULONGLONG now = MonoMs();
            long long wall = WallMs();
            bool dirty = false;
            for (size_t i = 0; i < timers.size();)
            {
                TimerItem &t = timers[i];
                if (t.running && now >= t.endMono && now - t.endMono > 90000)
                { // woke from sleep long after it ended: report, do not ring
                    MissedItem mi;
                    mi.title = t.name.empty() ? L"Timer" : t.name;
                    mi.whenWall = wall - (long long)(now - t.endMono);
                    mi.totalMs = t.totalMs;
                    missed.push_back(mi);
                    timers.erase(timers.begin() + i);
                    dirty = true;
                }
                else if (t.running && now >= t.endMono)
                {
                    RingEvent e;
                    e.id = t.id;
                    e.title = t.name.empty() ? L"Timer" : t.name;
                    e.sub = L"Time's up  (" + FmtDuration(t.totalMs) + L")";
                    e.sound = g_set.timerSound.load();
                    ev.push_back(e);
                    timers.erase(timers.begin() + i);
                    dirty = true;
                }
                else
                    ++i;
            }
            for (auto &a : alarms)
            {
                if (!a.enabled || a.ringing || a.nextFire <= 0 || wall < a.nextFire)
                    continue;
                if (wall - a.nextFire > 90LL * 1000)
                { // due while the device slept (or the clock jumped): report it, do not ring it late
                    MissedItem mi;
                    mi.isAlarm = true;
                    mi.title = a.label.empty() ? L"Alarm" : a.label;
                    mi.whenWall = a.nextFire;
                    missed.push_back(mi);
                    if (a.days == 0)
                    {
                        a.enabled = false;
                        a.nextFire = 0;
                    }
                    else
                        a.nextFire = NextFire(a.hh, a.mm, a.days, wall);
                    dirty = true;
                    continue;
                }
                a.ringing = true;
                RingEvent e;
                e.isAlarm = true;
                e.id = a.id;
                e.title = a.label.empty() ? L"Alarm" : a.label;
                e.sub = FmtTimeOfDay(a.hh, a.mm);
                e.snoozeMin = a.snoozeMin;
                e.sound = a.sound;
                ev.push_back(e);
                dirty = true;
            }
            if (dirty)
                SaveL();
            if (lastBeat == 0 || now - lastBeat >= 60000)
            { // heartbeat: lets the next start tell how long WinKlok was not running
                lastBeat = now;
                bool armed = false;
                for (auto &t : timers)
                    armed |= t.running;
                for (auto &a : alarms)
                    armed |= a.enabled;
                if (armed)
                {
                    const std::wstring w = std::to_wstring(wall);
                    Wh_SetStringValue(L"alive", w.c_str());
                }
            }
            return ev;
        }

        // Milliseconds until the next running timer ends or the next enabled alarm fires; -1 when nothing is armed.
        long long MsUntilNextDue()
        {
            std::lock_guard<std::mutex> g(m);
            const ULONGLONG now = MonoMs();
            const long long wall = WallMs();
            long long best = -1;
            auto consider = [&](long long d)
            {
                if (d < 0)
                    d = 0;
                if (best < 0 || d < best)
                    best = d;
            };
            for (auto &t : timers)
                if (t.running)
                    consider(t.endMono > now ? (long long)(t.endMono - now) : 0);
            for (auto &a : alarms)
                if (a.enabled && !a.ringing && a.nextFire > 0)
                    consider(a.nextFire - wall);
            return best;
        }

        std::vector<MissedItem> TakeMissed()
        {
            std::lock_guard<std::mutex> g(m);
            std::vector<MissedItem> v;
            v.swap(missed);
            std::sort(v.begin(), v.end(), [](const MissedItem &a, const MissedItem &b)
                      { return a.whenWall < b.whenWall; });
            return v;
        }

        PanelSnap Snapshot()
        {
            PanelSnap s;
            std::lock_guard<std::mutex> g(m);
            ULONGLONG now = MonoMs();
            long long wall = WallMs();
            s.swMs = SwElapsedL(now);
            s.swRunning = swRunning;
            s.laps = laps;
            for (auto &t : timers)
            {
                long long r = RemainL(t, now);
                s.timers.push_back({t.id, t.name, t.totalMs, r, t.running ? wall + r : 0, t.running});
            }
            s.alarms = alarms;
            s.presets = presets;
            return s;
        }

        PreviewSnap Preview()
        {
            PreviewSnap p;
            std::lock_guard<std::mutex> g(m);
            ULONGLONG now = MonoMs();
            long long sw = SwElapsedL(now);
            p.swOn = swRunning || sw > 0;
            p.swRun = swRunning;
            if (p.swOn)
                p.swText = FmtStopwatch(sw, false);
            if (const TimerItem *best = PreviewTimerL())
            {
                p.tmOn = true;
                p.tmRun = best->running;
                p.tmName = best->name.empty() ? L"Timer" : best->name;
                p.tmText = FmtClock((RemainL(*best, now) + 999) / 1000 * 1000); // reads 00:00 only at the very end
            }
            return p;
        }

        // id of the timer currently shown in the taskbar preview
        int PreviewTimerId()
        {
            std::lock_guard<std::mutex> g(m);
            const TimerItem *best = PreviewTimerL();
            return best ? best->id : 0;
        }
    };

    static Engine g_eng;

} // namespace td

// ============================================================================
//  FORWARD DECLARATIONS of the panel / alert UI (defined further down)
// ============================================================================
namespace ui
{
    bool Start(); // start the UI thread (panel + alerts)
    void Stop();  // stop it and wait
    void RequestTogglePanel(bool noActivate);
    void NotifyTaskbarClockPress();
} // namespace ui

// ============================================================================
//  TASKBAR SIDE: finds each taskbar clock, injects preview + click interception
// ============================================================================
namespace tb
{

    namespace wf = winrt::Windows::Foundation;
    namespace wux = winrt::Windows::UI::Xaml;
    namespace wuxc = winrt::Windows::UI::Xaml::Controls;
    namespace wuxm = winrt::Windows::UI::Xaml::Media;
    namespace wuxi = winrt::Windows::UI::Xaml::Input;

    enum : UINT
    {
        WM_TD_CLEANUP = WM_APP + 1,
        WM_TD_REFRESH = WM_APP + 2
    };
    static const wchar_t kMsgClass[] = L"WinklokTaskbarMsgWnd";

    struct Entry
    {
        DWORD tid = 0;
        wux::FrameworkElement clock{nullptr}; // SystemTray.DateTimeIconContent
        wuxc::Panel grid{nullptr};            // its ContainerGrid
        wux::FrameworkElement overlay{nullptr};
        wuxc::StackPanel host{nullptr}, previewStack{nullptr};
        wuxc::Viewbox scale{nullptr}; // scales the whole preview to the native clock height
        wuxc::Grid swBlock{nullptr}, tmBlock{nullptr}, swCompact{nullptr}, tmCompact{nullptr};
        wuxc::Border bell{nullptr};
        wuxc::TextBlock swLabel{nullptr}, swTime{nullptr}, swGlyph{nullptr}, tmLabel{nullptr}, tmTime{nullptr}, tmGlyph{nullptr};
        wuxc::TextBlock swCTime{nullptr}, swCGlyph{nullptr}, tmCTime{nullptr}, tmCGlyph{nullptr};
        std::vector<wuxc::TextBlock> allText;
        std::vector<std::function<void()>> revokers;
        std::vector<std::pair<wux::FrameworkElement, wux::Thickness>> shifted;
        std::vector<std::pair<wux::FrameworkElement, wux::Visibility>> bells;
        // Every tooltip attached to the native clock (and its ancestors) is removed and remembered here.
        std::vector<std::pair<wux::DependencyObject, wf::IInspectable>> tooltips;
        bool injected = false, bellHidden = false, seenLoaded = false;
        int injectFailures = 0, layoutKey = -1, bellSearchTries = 0;
        // Time stamps (ms, GetTickCount64) so the periodic jobs keep their cadence whatever the tick interval is.
        ULONGLONG unloadedSinceMs = 0, lastTipMs = 0, lastMeasureMs = 0, lastBrushMs = 0;
        double shift = 0, nativeH = 0, nativeDy = 0;

        void ClearUi()
        {
            overlay = nullptr;
            host = nullptr;
            previewStack = nullptr;
            scale = nullptr;
            swBlock = nullptr;
            tmBlock = nullptr;
            swCompact = nullptr;
            tmCompact = nullptr;
            bell = nullptr;
            swLabel = nullptr;
            swTime = nullptr;
            swGlyph = nullptr;
            tmLabel = nullptr;
            tmTime = nullptr;
            tmGlyph = nullptr;
            swCTime = nullptr;
            swCGlyph = nullptr;
            tmCTime = nullptr;
            tmCGlyph = nullptr;
            allText.clear();
            grid = nullptr;
            injected = false;
            layoutKey = -1;
            nativeH = nativeDy = 0;
        }
    };

    // XAML objects must never be destroyed off the UI thread / at process exit.
    [[clang::no_destroy]] static std::optional<
        std::vector<std::shared_ptr<Entry>>> g_entries{std::in_place};
    static std::mutex g_entMtx;
    static std::map<DWORD, HWND> g_msgWnds;
    static std::map<DWORD, UINT> g_msgTickMs; // current timer interval of each message window
    static std::mutex g_msgMtx;
    static bool g_msgClassRegistered = false;

    // ---- helpers ---------------------------------------------------------------
    static bool StartsWith(const winrt::hstring &s, const wchar_t *p)
    {
        size_t n = wcslen(p);
        return s.size() >= n && wcsncmp(s.c_str(), p, n) == 0;
    }

    static bool ContainsNoCase(const wchar_t *hay, const wchar_t *needle)
    {
        if (!hay || !needle)
            return false;
        std::wstring h(hay), n(needle);
        std::transform(h.begin(), h.end(), h.begin(), ::towlower);
        std::transform(n.begin(), n.end(), n.begin(), ::towlower);
        return h.find(n) != std::wstring::npos;
    }

    template <class F>
    static bool WalkVisual(wux::DependencyObject const &root, F &&fn, int depth = 0, int maxDepth = 40)
    {
        if (!root || depth > maxDepth)
            return false;
        if (fn(root, depth))
            return true;
        int n = wuxm::VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < n; ++i)
            if (WalkVisual(wuxm::VisualTreeHelper::GetChild(root, i), fn, depth + 1, maxDepth))
                return true;
        return false;
    }

    static wuxm::SolidColorBrush MakeBrush(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
    {
        wuxm::SolidColorBrush br;
        br.Color(winrt::Windows::UI::Color{a, r, g, b});
        return br;
    }

    // ---- tooltips (the hover card) ---------------------------------------------------------
    // Windows re-attaches a tooltip every time the clock text refreshes, so the removal is
    // repeated on every pointer-enter and on a short timer. Originals are kept for restore.
    static void ClearClockTooltips(Entry &en)
    {
        if (!en.clock)
            return;
        auto clear = [&](wux::DependencyObject const &o)
        {
            try
            {
                auto cur = wuxc::ToolTipService::GetToolTip(o);
                if (!cur)
                    return;
                bool known = false;
                for (auto &t : en.tooltips)
                    if (t.first == o)
                    {
                        known = true;
                        break;
                    }
                if (!known)
                    en.tooltips.emplace_back(o, cur);
                if (auto tt = cur.try_as<wuxc::ToolTip>())
                    tt.IsOpen(false);
                wuxc::ToolTipService::SetToolTip(o, nullptr);
            }
            catch (...)
            {
            }
        };
        wux::DependencyObject a = en.clock;
        for (int i = 0; i < 5 && a; ++i)
        {
            clear(a);
            a = wuxm::VisualTreeHelper::GetParent(a);
        }
        WalkVisual(en.clock, [&](wux::DependencyObject const &o, int)
                   { clear(o); return false; }, 0, 32);
    }

    static void RestoreClockTooltips(Entry &en)
    {
        for (auto &tip : en.tooltips)
        {
            try
            {
                wuxc::ToolTipService::SetToolTip(tip.first, tip.second);
            }
            catch (...)
            {
            }
        }
        en.tooltips.clear();
    }

    // ---- brush / layout ----------------------------------------------------------
    static wuxm::Brush NativeForeground(Entry &en)
    {
        wuxm::Brush result{nullptr};
        if (!en.grid)
            return result;
        WalkVisual(en.grid, [&](wux::DependencyObject const &o, int)
                   {
        if (auto t = o.try_as<wuxc::TextBlock>()) {
            if (!StartsWith(t.Name(), L"Winklok") && t.Text().size() > 0) { result = t.Foreground(); return true; }
        }
        return false; }, 0, 12);
        return result;
    }

    static void ApplyBrushes(Entry &en)
    {
        auto fg = NativeForeground(en);
        if (!fg)
            fg = MakeBrush(255, 255, 255, 255);
        for (auto const &t : en.allText)
            if (t)
                t.Foreground(fg);
    }

    static void ShiftNative(Entry &en, double w)
    {
        if (!en.grid)
            return;
        if (w <= 0.5)
        {
            for (auto &p : en.shifted)
            {
                try
                {
                    p.first.Margin(p.second);
                }
                catch (...)
                {
                }
            }
            en.shifted.clear();
            en.shift = 0;
            return;
        }
        auto kids = en.grid.Children();
        for (uint32_t i = 0; i < kids.Size(); ++i)
        {
            auto fe = kids.GetAt(i).try_as<wux::FrameworkElement>();
            if (!fe || StartsWith(fe.Name(), L"Winklok"))
                continue;
            wux::Thickness orig = fe.Margin();
            bool known = false;
            for (auto &p : en.shifted)
                if (p.first == fe)
                {
                    orig = p.second;
                    known = true;
                    break;
                }
            if (!known)
                en.shifted.emplace_back(fe, orig);
            fe.Margin(wux::Thickness{orig.Left + w, orig.Top, orig.Right, orig.Bottom});
        }
        en.shift = w;
    }

    // Vertical extent (height + centre offset inside the grid) of the native date/time text.
    static void Remeasure(Entry &en)
    {
        double top = 1e9, bot = -1e9, h = 0, dy = 0, gridH = 0;
        try
        {
            gridH = en.grid.ActualHeight();
            WalkVisual(en.grid, [&](wux::DependencyObject const &o, int)
                       {
            if (auto t = o.try_as<wuxc::TextBlock>()) {
                if (!StartsWith(t.Name(), L"Winklok") && t.Text().size() > 0 && t.ActualHeight() > 0 &&
                    t.Visibility() == wux::Visibility::Visible) {
                    auto pt = t.TransformToVisual(en.grid).TransformPoint(wf::Point{0.f, 0.f});
                    top = std::min(top, (double)pt.Y);
                    bot = std::max(bot, (double)pt.Y + t.ActualHeight());
                }
            }
            return false; }, 0, 12);
        }
        catch (...)
        {
            top = 1e9;
            bot = -1e9;
        }
        if (bot > top)
        {
            h = bot - top;
            if (gridH > 8)
                dy = (top + bot) / 2 - gridH / 2;
        }
        else
        {
            h = 32.0;
        }
        if (gridH > 8)
            h = std::min(h, gridH - 4);
        h = std::max(18.0, h);
        if (std::fabs(dy) > 8)
            dy = 0;
        if (std::fabs(h - en.nativeH) > 0.5 || std::fabs(dy - en.nativeDy) > 0.5)
        {
            en.nativeH = h;
            en.nativeDy = dy;
            en.layoutKey = -1;
        }
    }

    // ---- notification bell (best effort, strict name match) ------------------------------
    static void ApplyBell(Entry &en)
    {
        // WinKlok provides its own notification button. Hide the Windows-owned
        // bell while WinKlok is active to prevent duplicates, then restore it.
        bool wantHidden = WkActive() && (td::g_set.showBell.load() != 0);
        if (wantHidden && !en.bellHidden && en.bellSearchTries < 50)
        {
            en.bellSearchTries++;
            std::vector<void *> chain;
            wux::DependencyObject cur = en.clock;
            for (int i = 0; i < 10 && cur; ++i)
            {
                chain.push_back(winrt::get_abi(cur));
                cur = wuxm::VisualTreeHelper::GetParent(cur);
            }
            for (size_t a = 1; a < chain.size() && a <= 8; ++a)
            {
                wux::DependencyObject anc = en.clock;
                for (size_t k = 0; k < a && anc; ++k)
                    anc = wuxm::VisualTreeHelper::GetParent(anc);
                if (!anc)
                    break;
                WalkVisual(anc, [&](wux::DependencyObject const &o, int depth)
                           {
                if (depth > 6) return false;
                auto fe = o.try_as<wux::FrameworkElement>();
                if (!fe || StartsWith(fe.Name(), L"Winklok")) return false;
                if (std::find(chain.begin(), chain.end(), winrt::get_abi(fe)) != chain.end()) return false;
                std::wstring cls;
                try { cls = winrt::get_class_name(fe).c_str(); } catch (...) {}
                winrt::hstring aid;
                try { aid = wux::Automation::AutomationProperties::GetAutomationId(fe); } catch (...) {}
                bool bell = ContainsNoCase(fe.Name().c_str(), L"bell") || ContainsNoCase(aid.c_str(), L"bell");
                if (bell && !ContainsNoCase(cls.c_str(), L"NotificationArea")) {
                    for (auto& b : en.bells) if (b.first == fe) return false;
                    en.bells.emplace_back(fe, fe.Visibility());
                    Wh_Log(L"hiding native notification candidate name='%s' id='%s' class='%s'",
                           fe.Name().c_str(), aid.c_str(), cls.c_str());
                }
                return false; }, 0, 10);
                if (!en.bells.empty())
                    break;
            }
            for (auto &b : en.bells)
            {
                try
                {
                    b.first.Visibility(wux::Visibility::Collapsed);
                }
                catch (...)
                {
                }
            }
            en.bellHidden = !en.bells.empty();
            if (en.bells.empty() && en.bellSearchTries == 50)
                Wh_Log(L"could not identify the native notification button");
        }
        else if (!wantHidden && !en.bells.empty())
        {
            for (auto &b : en.bells)
            {
                try
                {
                    b.first.Visibility(b.second);
                }
                catch (...)
                {
                }
            }
            en.bells.clear();
            en.bellHidden = false;
            en.bellSearchTries = 0;
        }
        else if (wantHidden && en.bellHidden)
        {
            for (auto &b : en.bells)
            {
                try
                {
                    if (b.first.Visibility() != wux::Visibility::Collapsed)
                        b.first.Visibility(wux::Visibility::Collapsed);
                }
                catch (...)
                {
                }
            }
        }
    }

    // ---- click handling ------------------------------------------------------------------
    static bool IsLeft(wf::IInspectable const &sender, wuxi::PointerRoutedEventArgs const &e, bool release)
    {
        try
        {
            auto src = sender.try_as<wux::UIElement>();
            if (!src)
                return false;
            auto props = e.GetCurrentPoint(src).Properties();
            if (release)
                return props.PointerUpdateKind() == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased;
            return props.IsLeftButtonPressed();
        }
        catch (...)
        {
            return false;
        }
    }

    static void PostRefreshAll();

    static void HookRegion(Entry &en, wux::UIElement const &el)
    {
        Entry *pe = &en; // entries are heap-allocated and outlive their (revoked) handlers
        auto t1 = el.PointerPressed([](wf::IInspectable const &s, wuxi::PointerRoutedEventArgs const &e)
                                    {
        if (!WkActive() || !IsLeft(s, e, false)) return;
        // Notify the panel before Windows deactivates it, so the focus-loss handler
        // cannot hide it before the clock-toggle message arrives.
        ui::NotifyTaskbarClockPress();
        e.Handled(true); });
        auto t2 = el.PointerReleased([](wf::IInspectable const &s, wuxi::PointerRoutedEventArgs const &e)
                                     {
                                         if (!WkActive() || !IsLeft(s, e, true))
                                             return;
                                         e.Handled(true);
                                         ui::RequestTogglePanel(false); // the single activation path
                                     });
        // The native flyout opens from the routed Tapped event: swallow it, but never toggle here.
        auto t3 = el.Tapped([](wf::IInspectable const &, wuxi::TappedRoutedEventArgs const &e)
                            {
        if (WkActive()) e.Handled(true); });
        auto t4 = el.PointerEntered([pe](wf::IInspectable const &, wuxi::PointerRoutedEventArgs const &)
                                    {
                                        if (WkActive())
                                            ClearClockTooltips(*pe); // kill the hover card before it can show
                                    });
        en.revokers.push_back([el, t1, t2, t3, t4]()
                              {
        try { el.PointerPressed(t1); el.PointerReleased(t2); el.Tapped(t3); el.PointerEntered(t4); } catch (...) {} });
    }

    static void HookButton(Entry &en, wux::UIElement const &el, std::function<void()> action)
    {
        auto t1 = el.PointerPressed([](wf::IInspectable const &, wuxi::PointerRoutedEventArgs const &e)
                                    { e.Handled(true); });
        auto t2 = el.PointerReleased([action](wf::IInspectable const &, wuxi::PointerRoutedEventArgs const &e)
                                     {
        e.Handled(true);
        if (!WkActive()) return;
        action();
        PostRefreshAll(); });
        auto t3 = el.Tapped([](wf::IInspectable const &, wuxi::TappedRoutedEventArgs const &e)
                            { e.Handled(true); });
        auto t4 = el.PointerEntered([](wf::IInspectable const &s, wuxi::PointerRoutedEventArgs const &)
                                    {
        if (auto b = s.try_as<wuxc::Border>()) b.Background(MakeBrush(40, 160, 160, 160)); });
        auto t5 = el.PointerExited([](wf::IInspectable const &s, wuxi::PointerRoutedEventArgs const &)
                                   {
        if (auto b = s.try_as<wuxc::Border>()) b.Background(MakeBrush(0, 0, 0, 0)); });
        en.revokers.push_back([el, t1, t2, t3, t4, t5]()
                              {
        try { el.PointerPressed(t1); el.PointerReleased(t2); el.Tapped(t3); el.PointerEntered(t4); el.PointerExited(t5); } catch (...) {} });
    }

    // ---- injection -----------------------------------------------------------------------
    // The preview is authored at a fixed 36-unit height and a Viewbox scales it to the exact
    // height of the native date/time text, so every element stays proportional at any taskbar height.
    static const wchar_t kHostXaml[] = LR"XAML(
<StackPanel xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
            xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
            x:Name="WinklokHost" Grid.RowSpan="99" Grid.ColumnSpan="99"
            Orientation="Horizontal" HorizontalAlignment="Left" VerticalAlignment="Center"
            Background="Transparent" Margin="4,0,2,0" Visibility="Collapsed">
  <Viewbox x:Name="WinklokScale" Stretch="Uniform" VerticalAlignment="Center" Height="32">
    <StackPanel Orientation="Horizontal" Height="36" VerticalAlignment="Center">
      <StackPanel x:Name="WinklokPreviewStack" Orientation="Horizontal" VerticalAlignment="Center" Margin="0,0,6,0">
        <Grid x:Name="WinklokSwBlock" Width="88" Height="36" Visibility="Collapsed">
          <Grid.ColumnDefinitions><ColumnDefinition Width="*"/><ColumnDefinition Width="20"/></Grid.ColumnDefinitions>
          <Grid.RowDefinitions><RowDefinition Height="*"/><RowDefinition Height="*"/></Grid.RowDefinitions>
          <TextBlock x:Name="WinklokSwLabel" Grid.Row="0" Text="Stopwatch" FontSize="10" Opacity="0.75"
                     HorizontalAlignment="Center" VerticalAlignment="Center" TextTrimming="CharacterEllipsis"/>
          <TextBlock x:Name="WinklokSwTime" Grid.Row="1" Text="00:00.0" FontSize="15" FontWeight="SemiBold"
                     TextAlignment="Center" HorizontalAlignment="Stretch" VerticalAlignment="Center"/>
          <StackPanel Grid.Column="1" Grid.RowSpan="2" Orientation="Vertical" HorizontalAlignment="Right" VerticalAlignment="Center">
            <Border x:Name="WinklokSwToggle" Width="20" Height="18" CornerRadius="4" Background="Transparent">
              <TextBlock x:Name="WinklokSwGlyph" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="9"
                         Text="&#xE768;" HorizontalAlignment="Center" VerticalAlignment="Center"/>
            </Border>
            <Border x:Name="WinklokSwReset" Width="20" Height="18" CornerRadius="4" Background="Transparent">
              <TextBlock x:Name="WinklokSwResetGlyph" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="9"
                         Text="&#xE777;" HorizontalAlignment="Center" VerticalAlignment="Center"/>
            </Border>
          </StackPanel>
        </Grid>
        <Grid x:Name="WinklokTmBlock" Width="88" Height="36" Visibility="Collapsed">
          <Grid.ColumnDefinitions><ColumnDefinition Width="*"/><ColumnDefinition Width="20"/></Grid.ColumnDefinitions>
          <Grid.RowDefinitions><RowDefinition Height="*"/><RowDefinition Height="*"/></Grid.RowDefinitions>
          <TextBlock x:Name="WinklokTmLabel" Grid.Row="0" Text="Timer" FontSize="10" Opacity="0.75"
                     HorizontalAlignment="Center" VerticalAlignment="Center" TextTrimming="CharacterEllipsis" MaxWidth="66"/>
          <TextBlock x:Name="WinklokTmTime" Grid.Row="1" Text="00:00" FontSize="15" FontWeight="SemiBold"
                     TextAlignment="Center" HorizontalAlignment="Stretch" VerticalAlignment="Center"/>
          <StackPanel Grid.Column="1" Grid.RowSpan="2" Orientation="Vertical" HorizontalAlignment="Right" VerticalAlignment="Center">
            <Border x:Name="WinklokTmToggle" Width="20" Height="18" CornerRadius="4" Background="Transparent">
              <TextBlock x:Name="WinklokTmGlyph" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="9"
                         Text="&#xE769;" HorizontalAlignment="Center" VerticalAlignment="Center"/>
            </Border>
            <Border x:Name="WinklokTmCancel" Width="20" Height="18" CornerRadius="4" Background="Transparent">
              <TextBlock x:Name="WinklokTmCancelGlyph" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="9"
                         Text="&#xE711;" HorizontalAlignment="Center" VerticalAlignment="Center"/>
            </Border>
          </StackPanel>
        </Grid>
        <Grid x:Name="WinklokSwCompactBlock" Width="124" Height="18" Visibility="Collapsed">
          <Grid.ColumnDefinitions>
            <ColumnDefinition Width="28"/><ColumnDefinition Width="*"/><ColumnDefinition Width="18"/><ColumnDefinition Width="18"/>
          </Grid.ColumnDefinitions>
          <TextBlock x:Name="WinklokSwTag" Grid.Column="0" Text="SW :" FontSize="10" Opacity="0.8" VerticalAlignment="Center"/>
          <TextBlock x:Name="WinklokSwCompactTime" Grid.Column="1" Text="00:00.0" FontSize="13" FontWeight="SemiBold"
                     TextAlignment="Center" HorizontalAlignment="Stretch" VerticalAlignment="Center"/>
          <Border x:Name="WinklokSwCompactToggle" Grid.Column="2" Width="18" Height="18" CornerRadius="4" Background="Transparent">
            <TextBlock x:Name="WinklokSwCompactGlyph" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="9"
                       Text="&#xE768;" HorizontalAlignment="Center" VerticalAlignment="Center"/>
          </Border>
          <Border x:Name="WinklokSwCompactReset" Grid.Column="3" Width="18" Height="18" CornerRadius="4" Background="Transparent">
            <TextBlock x:Name="WinklokSwCompactResetGlyph" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="9"
                       Text="&#xE777;" HorizontalAlignment="Center" VerticalAlignment="Center"/>
          </Border>
        </Grid>
        <Grid x:Name="WinklokTmCompactBlock" Width="124" Height="18" Visibility="Collapsed">
          <Grid.ColumnDefinitions>
            <ColumnDefinition Width="28"/><ColumnDefinition Width="*"/><ColumnDefinition Width="18"/><ColumnDefinition Width="18"/>
          </Grid.ColumnDefinitions>
          <TextBlock x:Name="WinklokTmTag" Grid.Column="0" Text="TI :" FontSize="10" Opacity="0.8" VerticalAlignment="Center"/>
          <TextBlock x:Name="WinklokTmCompactTime" Grid.Column="1" Text="00:00" FontSize="13" FontWeight="SemiBold"
                     TextAlignment="Center" HorizontalAlignment="Stretch" VerticalAlignment="Center"/>
          <Border x:Name="WinklokTmCompactToggle" Grid.Column="2" Width="18" Height="18" CornerRadius="4" Background="Transparent">
            <TextBlock x:Name="WinklokTmCompactGlyph" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="9"
                       Text="&#xE769;" HorizontalAlignment="Center" VerticalAlignment="Center"/>
          </Border>
          <Border x:Name="WinklokTmCompactCancel" Grid.Column="3" Width="18" Height="18" CornerRadius="4" Background="Transparent">
            <TextBlock x:Name="WinklokTmCompactCancelGlyph" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="9"
                       Text="&#xE711;" HorizontalAlignment="Center" VerticalAlignment="Center"/>
          </Border>
        </Grid>
      </StackPanel>
      <Border x:Name="WinklokBell" Width="26" Height="36" CornerRadius="5" Background="Transparent"
              VerticalAlignment="Center" Visibility="Collapsed">
        <TextBlock x:Name="WinklokBellGlyph" FontFamily="Segoe Fluent Icons, Segoe MDL2 Assets" FontSize="15"
                   Text="&#xEA8F;" HorizontalAlignment="Center" VerticalAlignment="Center"/>
      </Border>
    </StackPanel>
  </Viewbox>
</StackPanel>
)XAML";

    static const wchar_t kOverlayXaml[] = LR"XAML(
<Border xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        x:Name="WinklokOverlay" Grid.RowSpan="99" Grid.ColumnSpan="99"
        Background="Transparent" HorizontalAlignment="Stretch" VerticalAlignment="Stretch"/>
)XAML";

    template <class T>
    static T Named(wux::FrameworkElement const &root, const wchar_t *name)
    {
        return root.FindName(name).as<T>();
    }

    static void RemoveOurChildren(Entry &en)
    {
        if (!en.grid)
            return;
        auto kids = en.grid.Children();
        for (int i = (int)kids.Size() - 1; i >= 0; --i)
        {
            auto fe = kids.GetAt((uint32_t)i).try_as<wux::FrameworkElement>();
            if (fe && StartsWith(fe.Name(), L"Winklok"))
                kids.RemoveAt((uint32_t)i);
        }
    }

    static void OpenWindowsNotifications()
    {
        // Windows 11's Win+N shortcut opens Notification Center.
        INPUT keys[4]{};
        keys[0].type = INPUT_KEYBOARD;
        keys[0].ki.wVk = VK_LWIN;
        keys[1].type = INPUT_KEYBOARD;
        keys[1].ki.wVk = L'N';
        keys[2].type = INPUT_KEYBOARD;
        keys[2].ki.wVk = L'N';
        keys[2].ki.dwFlags = KEYEVENTF_KEYUP;
        keys[3].type = INPUT_KEYBOARD;
        keys[3].ki.wVk = VK_LWIN;
        keys[3].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(ARRAYSIZE(keys), keys, sizeof(INPUT));
    }

    static bool Inject(Entry &en)
    {
        if (en.injected || !en.clock)
            return en.injected;
        try
        {
            wux::FrameworkElement gridEl{nullptr};
            WalkVisual(en.clock, [&](wux::DependencyObject const &o, int)
                       {
            if (auto fe = o.try_as<wux::FrameworkElement>())
                if (fe.Name() == L"ContainerGrid" && fe.try_as<wuxc::Panel>()) { gridEl = fe; return true; }
            return false; });
            if (!gridEl)
            { // fallback: first Grid below the clock
                WalkVisual(en.clock, [&](wux::DependencyObject const &o, int)
                           {
                if (auto g = o.try_as<wuxc::Grid>()) { gridEl = g; return true; }
                return false; });
                if (gridEl)
                    Wh_Log(L"ContainerGrid not found, using first Grid '%s'", gridEl.Name().c_str());
            }
            if (!gridEl)
            {
                en.injectFailures++;
                return false;
            }
            en.grid = gridEl.as<wuxc::Panel>();
            RemoveOurChildren(en); // leftovers from a previous injection into this same grid

            en.host = wux::Markup::XamlReader::Load(kHostXaml).as<wuxc::StackPanel>();
            en.overlay = wux::Markup::XamlReader::Load(kOverlayXaml).as<wux::FrameworkElement>();

            auto tb = [&](const wchar_t *n)
            { return Named<wuxc::TextBlock>(en.host, n); };
            en.scale = Named<wuxc::Viewbox>(en.host, L"WinklokScale");
            en.previewStack = Named<wuxc::StackPanel>(en.host, L"WinklokPreviewStack");
            en.swBlock = Named<wuxc::Grid>(en.host, L"WinklokSwBlock");
            en.tmBlock = Named<wuxc::Grid>(en.host, L"WinklokTmBlock");
            en.swCompact = Named<wuxc::Grid>(en.host, L"WinklokSwCompactBlock");
            en.tmCompact = Named<wuxc::Grid>(en.host, L"WinklokTmCompactBlock");
            en.bell = Named<wuxc::Border>(en.host, L"WinklokBell");
            en.swLabel = tb(L"WinklokSwLabel");
            en.swTime = tb(L"WinklokSwTime");
            en.swGlyph = tb(L"WinklokSwGlyph");
            en.tmLabel = tb(L"WinklokTmLabel");
            en.tmTime = tb(L"WinklokTmTime");
            en.tmGlyph = tb(L"WinklokTmGlyph");
            en.swCTime = tb(L"WinklokSwCompactTime");
            en.swCGlyph = tb(L"WinklokSwCompactGlyph");
            en.tmCTime = tb(L"WinklokTmCompactTime");
            en.tmCGlyph = tb(L"WinklokTmCompactGlyph");
            en.allText.clear();
            for (const wchar_t *n : {L"WinklokSwLabel", L"WinklokSwTime", L"WinklokSwGlyph", L"WinklokSwResetGlyph",
                                     L"WinklokTmLabel", L"WinklokTmTime", L"WinklokTmGlyph", L"WinklokTmCancelGlyph",
                                     L"WinklokSwTag", L"WinklokSwCompactTime", L"WinklokSwCompactGlyph",
                                     L"WinklokSwCompactResetGlyph", L"WinklokTmTag", L"WinklokTmCompactTime",
                                     L"WinklokTmCompactGlyph", L"WinklokTmCompactCancelGlyph", L"WinklokBellGlyph"})
                en.allText.push_back(tb(n));

            en.grid.Children().Append(en.overlay);
            en.grid.Children().Append(en.host);

            HookRegion(en, en.overlay);
            HookRegion(en, en.host);
            HookRegion(en, en.clock); // fallback in case a clock child lies outside the overlay's hit area

            auto swToggle = []()
            { td::g_eng.SwToggle(); };
            auto swReset = []()
            { td::g_eng.SwReset(); };
            auto tmToggle = []()
            { if (int id = td::g_eng.PreviewTimerId()) td::g_eng.TimerToggle(id); };
            auto tmCancel = []()
            { if (int id = td::g_eng.PreviewTimerId()) td::g_eng.TimerCancel(id); };
            struct BtnDef
            {
                const wchar_t *name;
                std::function<void()> fn;
            };
            BtnDef defs[] = {
                {L"WinklokSwToggle", swToggle},
                {L"WinklokSwReset", swReset},
                {L"WinklokSwCompactToggle", swToggle},
                {L"WinklokSwCompactReset", swReset},
                {L"WinklokTmToggle", tmToggle},
                {L"WinklokTmCancel", tmCancel},
                {L"WinklokTmCompactToggle", tmToggle},
                {L"WinklokTmCompactCancel", tmCancel},
                {L"WinklokBell", []()
                 { OpenWindowsNotifications(); }},
            };
            for (auto &d : defs)
                HookButton(en, Named<wux::UIElement>(en.host, d.name), d.fn);

            en.layoutKey = -1;
            en.lastBrushMs = GetTickCount64();
            ClearClockTooltips(en);
            en.injected = true;
            en.injectFailures = 0;
            Wh_Log(L"injected into a taskbar clock");
            return true;
        }
        catch (winrt::hresult_error const &ex)
        {
            Wh_Log(L"inject failed: 0x%08X %s", (unsigned)ex.code().value, ex.message().c_str());
        }
        catch (...)
        {
            Wh_Log(L"inject failed (unknown exception)");
        }
        en.injectFailures++;
        return false;
    }

    static void Restore(Entry &en)
    {
        try
        {
            for (auto &r : en.revokers)
                r();
            en.revokers.clear();
            ShiftNative(en, 0);
            for (auto &b : en.bells)
            {
                try
                {
                    b.first.Visibility(b.second);
                }
                catch (...)
                {
                }
            }
            en.bells.clear();
            en.bellHidden = false;
            RestoreClockTooltips(en);
            RemoveOurChildren(en);
        }
        catch (...)
        {
            Wh_Log(L"exception while restoring clock");
        }
        en.ClearUi();
    }

    static void ApplyPreview(Entry &en, const td::PreviewSnap &p)
    {
        if (!en.injected)
            return;
        try
        {
            auto &S = td::g_set;
            const ULONGLONG nowMs = GetTickCount64();
            if (en.layoutKey < 0 || nowMs - en.lastMeasureMs >= 500)
            {
                en.lastMeasureMs = nowMs;
                Remeasure(en);
            }

            const bool showAny = S.showPreviews.load() && (p.swOn || p.tmOn);
            const bool compact = showAny && p.swOn && p.tmOn && S.stacked.load() != 0; // two one-line rows
            const bool bell = S.showBell.load() != 0;
            const int key = (showAny ? 1 : 0) | (p.swOn ? 2 : 0) | (p.tmOn ? 4 : 0) | (compact ? 8 : 0) | (bell ? 16 : 0);

            if (showAny)
            {
                auto setT = [](wuxc::TextBlock const &t, std::wstring const &s)
                { t.Text(winrt::hstring(s)); };
                setT(en.swTime, p.swText);
                setT(en.swCTime, p.swText);
                setT(en.tmTime, p.tmText);
                setT(en.tmCTime, p.tmText);
                setT(en.tmLabel, p.tmName.empty() ? std::wstring(L"Timer") : p.tmName);
                const wchar_t *swG = p.swRun ? L"\uE769" : L"\uE768";
                const wchar_t *tmG = p.tmRun ? L"\uE769" : L"\uE768";
                en.swGlyph.Text(swG);
                en.swCGlyph.Text(swG);
                en.tmGlyph.Text(tmG);
                en.tmCGlyph.Text(tmG);
            }

            if (key != en.layoutKey)
            {
                en.layoutKey = key;
                const auto vis = [](bool v)
                { return v ? wux::Visibility::Visible : wux::Visibility::Collapsed; };
                en.host.Visibility(vis(showAny || bell));
                en.previewStack.Visibility(vis(showAny));
                en.bell.Visibility(vis(bell));
                en.swBlock.Visibility(vis(showAny && p.swOn && !compact));
                en.tmBlock.Visibility(vis(showAny && p.tmOn && !compact));
                en.swCompact.Visibility(vis(compact));
                en.tmCompact.Visibility(vis(compact));
                en.previewStack.Orientation(compact ? wuxc::Orientation::Vertical : wuxc::Orientation::Horizontal);
                en.previewStack.Margin(wux::Thickness{0, 0, bell ? 6.0 : 0.0, 0});
                en.swBlock.Margin(wux::Thickness{0, 0, (p.tmOn && !compact) ? 6.0 : 0.0, 0});
                en.tmBlock.Margin(wux::Thickness{0, 0, 0, 0});

                en.scale.Height(en.nativeH); // preview + buttons are exactly as tall as the native date/time
                double dy = en.nativeDy;     // keep the preview centred on the native text
                en.host.Margin(wux::Thickness{4, dy > 0 ? 2 * dy : 0.0, 2, dy < 0 ? -2 * dy : 0.0});
                ApplyBrushes(en);
                en.host.Measure(wf::Size{1e6f, 1e6f});
                ShiftNative(en, (showAny || bell) ? en.host.DesiredSize().Width : 0);
            }
            else if (showAny && nowMs - en.lastBrushMs >= 5000)
            {
                en.lastBrushMs = nowMs;
                ApplyBrushes(en); // follows light/dark changes
            }
        }
        catch (winrt::hresult_error const &)
        {
            // transient XAML state: the next tick tries again
        }
        catch (...)
        {
        }
    }

    // ---- per-thread message window: ticking + marshalling ----------------------------------------
    static std::vector<std::shared_ptr<Entry>> EntriesForThisThread()
    {
        std::vector<std::shared_ptr<Entry>> r;
        std::lock_guard<std::mutex> g(g_entMtx);
        DWORD tid = GetCurrentThreadId();
        if (g_entries)
            for (auto &e : *g_entries)
                if (e->tid == tid)
                    r.push_back(e);
        return r;
    }

    static void DropEntries(bool all)
    {
        std::lock_guard<std::mutex> g(g_entMtx);
        if (!g_entries)
            return;
        DWORD tid = GetCurrentThreadId();
        auto &entries = *g_entries;
        entries.erase(std::remove_if(entries.begin(), entries.end(),
                                     [&](const std::shared_ptr<Entry> &e)
                                     { return e->tid == tid && (all || !e->clock); }),
                      entries.end());
    }

    // Sets the interval of this thread's message-window timer (only when it changes).
    static void SetTickMs(UINT ms)
    {
        const DWORD tid = GetCurrentThreadId();
        std::lock_guard<std::mutex> g(g_msgMtx);
        auto it = g_msgWnds.find(tid);
        if (it == g_msgWnds.end())
            return;
        UINT &cur = g_msgTickMs[tid];
        if (cur == ms)
            return;
        cur = ms;
        SetTimer(it->second, 1, ms, nullptr);
    }

    // The tick only runs as fast as what is on screen needs: 100 ms for a running stopwatch preview (tenths),
    // 250 ms for a running timer preview or while the bell is still being set up, 100 ms until the first
    // injection has succeeded, and 1 s otherwise. Engine and settings changes do not wait for the next tick:
    // they post a refresh (PostRefreshAll).
    static void OnTick()
    {
        auto list = EntriesForThisThread();
        if (list.empty())
        {
            SetTickMs(1000);
            return;
        }
        const ULONGLONG nowMs = GetTickCount64();
        const bool active = WkActive();
        td::PreviewSnap snap;
        if (active)
            snap = td::g_eng.Preview();
        for (auto &en : list)
        {
            try
            {
                bool loaded = true;
                try
                {
                    loaded = (bool)wuxm::VisualTreeHelper::GetParent(en->clock);
                }
                catch (...)
                {
                }
                if (loaded)
                {
                    en->seenLoaded = true;
                    en->unloadedSinceMs = 0;
                }
                else if (en->seenLoaded)
                {
                    if (!en->unloadedSinceMs)
                        en->unloadedSinceMs = nowMs;
                    else if (nowMs - en->unloadedSinceMs > 3000)
                    { // element left the tree (taskbar rebuilt)
                        Restore(*en);
                        en->clock = nullptr;
                        continue;
                    }
                }
                if (!active)
                {
                    if (en->injected)
                        Restore(*en);
                    continue;
                }
                if (!en->injected)
                {
                    if (en->injectFailures < 25)
                        Inject(*en);
                    if (!en->injected)
                        continue;
                }
                ApplyPreview(*en, snap);
                ApplyBell(*en);
                if (nowMs - en->lastTipMs >= 300)
                {
                    en->lastTipMs = nowMs;
                    ClearClockTooltips(*en);
                }
            }
            catch (...)
            {
                // transient XAML state: the next tick tries again
            }
        }
        DropEntries(false); // drop dead entries

        UINT want = 1000;
        if (active)
        {
            const bool previews = td::g_set.showPreviews.load() != 0;
            if (previews && snap.swOn && snap.swRun)
                want = 100;
            else if (previews && snap.tmOn && snap.tmRun)
                want = 250;
            const bool bell = td::g_set.showBell.load() != 0;
            for (auto &en : list)
            {
                if (!en->clock)
                    continue;
                if (!en->injected && en->injectFailures < 25)
                    want = std::min(want, 100u); // retry the first injection quickly
                else if (bell && !en->bellHidden && en->bellSearchTries < 50)
                    want = std::min(want, 250u); // still looking for the native bell
            }
        }
        SetTickMs(want);
    }

    static LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        switch (msg)
        {
        case WM_TIMER:
            if (wp == 1)
            {
                OnTick();
                return 0;
            }
            break;
        case WM_TD_REFRESH:
            OnTick();
            return 0;
        case WM_TD_CLEANUP:
            for (auto &en : EntriesForThisThread())
            {
                Restore(*en);
                en->clock = nullptr;
            }
            DropEntries(true);
            KillTimer(hwnd, 1);
            DestroyWindow(hwnd);
            return 0;
        case WM_NCDESTROY:
        {
            std::lock_guard<std::mutex> g(g_msgMtx);
            g_msgTickMs.erase(GetCurrentThreadId());
            for (auto it = g_msgWnds.begin(); it != g_msgWnds.end(); ++it)
                if (it->second == hwnd)
                {
                    g_msgWnds.erase(it);
                    break;
                }
            break;
        }
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }

    static bool EnsureMsgWindow()
    {
        DWORD tid = GetCurrentThreadId();
        std::lock_guard<std::mutex> g(g_msgMtx);
        // Do not create a new taskbar message window once unload cleanup has begun.
        if (g_shuttingDown.load())
            return false;
        if (g_msgWnds.count(tid))
            return true;
        if (!g_msgClassRegistered)
        {
            WNDCLASSW wc{};
            wc.lpfnWndProc = MsgProc;
            wc.hInstance = ModInst();
            wc.lpszClassName = kMsgClass;
            if (!RegisterClassW(&wc))
            {
                Wh_Log(L"could not register the message window class (%u)", GetLastError());
                return false;
            }
            g_msgClassRegistered = true;
        }
        HWND h = CreateWindowExW(0, kMsgClass, L"WinklokMsg", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                 ModInst(), nullptr);
        if (!h)
        {
            Wh_Log(L"could not create message window (%u)", GetLastError());
            return false;
        }
        SetTimer(h, 1, 100, nullptr);
        g_msgTickMs[tid] = 100;
        g_msgWnds[tid] = h;
        return true;
    }

    static void PostRefreshAll()
    {
        std::lock_guard<std::mutex> g(g_msgMtx);
        for (auto &kv : g_msgWnds)
            PostMessageW(kv.second, WM_TD_REFRESH, 0, 0);
    }

    // ---- adoption of clock elements -----------------------------------------------------------------
    static std::shared_ptr<Entry> FindEntry(wux::FrameworkElement const &fe)
    {
        std::lock_guard<std::mutex> g(g_entMtx);
        if (!g_entries)
            return nullptr;
        for (auto &e : *g_entries)
            if (e->clock && e->clock == fe)
                return e;
        return nullptr;
    }

    static void AdoptClock(wux::FrameworkElement const &fe, bool retemplated)
    {
        if (g_shuttingDown.load())
            return;
        auto en = FindEntry(fe);
        if (en)
        {
            if (retemplated)
            { // new template instance: the old ContainerGrid is gone
                Restore(*en);
                en->injectFailures = 0;
                en->bellSearchTries = 0;
            }
            return;
        }
        if (!EnsureMsgWindow())
            return; // this clock could never be cleaned up, so don't touch it
        en = std::make_shared<Entry>();
        en->tid = GetCurrentThreadId();
        en->clock = fe;
        {
            std::lock_guard<std::mutex> g(g_entMtx);
            // Cleanup may have started while this hook was finding the clock.
            if (g_shuttingDown.load() || !g_entries)
                return;
            g_entries->push_back(en);
        }
        Wh_Log(L"adopted clock element (tid %u)", en->tid);
        SetTickMs(100); // a new clock: start with the fast tick until it is set up
        if (WkActive())
            Inject(*en);
    }

    // ---- clock hooks --------------------------------------------------------------------------------
    using OnApplyTemplate_t = int(WINAPI *)(void *); // produce<> ABI thunk: HRESULT OnApplyTemplate()
    static OnApplyTemplate_t g_origOAT = nullptr;
    using GetViewModel_t = int(WINAPI *)(void *, void **); // produce<> ABI thunk: HRESULT get_ViewModel(void **)
    static GetViewModel_t g_origGVM = nullptr;
    static std::atomic<bool> g_hooksReady{false};

    // Hooked on the produce<> ABI thunk, so pThis is always an IUnknown* of the DateTimeIconContent object.
    static int WINAPI OAT_Hook(void *pThis)
    {
        const int hr = g_origOAT ? g_origOAT(pThis) : (int)E_FAIL;
        if (g_shuttingDown.load() || !pThis)
            return hr;
        try
        {
            wux::FrameworkElement fe{nullptr};
            ((IUnknown *)pThis)->QueryInterface(winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(fe));
            if (fe)
                AdoptClock(fe, true);
            else
                Wh_Log(L"OnApplyTemplate: no FrameworkElement from this=%p", pThis);
        }
        catch (...)
        {
            Wh_Log(L"OnApplyTemplate exception");
        }
        return hr;
    }

    static std::unordered_map<void *, bool> g_seenThis;
    static std::mutex g_seenMtx;

    // Hooked on the produce<> ABI thunk, so pThis is always an IUnknown* of the runtime object.
    static int WINAPI GVM_Hook(void *pThis, void **ppViewModel)
    {
        const int hr = g_origGVM ? g_origGVM(pThis, ppViewModel) : (int)E_FAIL;
        if (g_shuttingDown.load() || !pThis)
            return hr;
        try
        {
            static std::atomic<bool> logged{false};
            if (!logged.exchange(true))
                Wh_Log(L"BadgeIconContent::get_ViewModel hook is active");
            {
                std::lock_guard<std::mutex> g(g_seenMtx);
                auto it = g_seenThis.find(pThis);
                if (it != g_seenThis.end() && !it->second)
                    return hr; // known non-clock
                if (g_seenThis.size() > 512)
                    g_seenThis.clear();
            }
            wux::FrameworkElement fe{nullptr};
            ((IUnknown *)pThis)->QueryInterface(winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(fe));
            bool isClock = false;
            if (fe)
            {
                try
                {
                    isClock = winrt::get_class_name(fe) == L"SystemTray.DateTimeIconContent";
                }
                catch (...)
                {
                }
            }
            {
                std::lock_guard<std::mutex> g(g_seenMtx);
                g_seenThis[pThis] = isClock;
            }
            if (isClock)
                AdoptClock(fe, false);
        }
        catch (...)
        {
        }
        return hr;
    }

    // Modules where the symbol lookup already failed: each candidate DLL is looked up at most once.
    static std::vector<HMODULE> g_failedModules;
    static std::mutex g_failedMtx;
    // Two threads can return from LoadLibraryExW for the same candidate DLL at once; one lookup at a time
    // (recursive: HookSymbols may load modules itself, which comes back through the LoadLibraryExW hook).
    static std::recursive_mutex g_hookMtx;

    static bool ModuleAlreadyFailed(HMODULE h)
    {
        std::lock_guard<std::mutex> g(g_failedMtx);
        return std::find(g_failedModules.begin(), g_failedModules.end(), h) != g_failedModules.end();
    }

    static void RememberFailedModule(HMODULE h)
    {
        std::lock_guard<std::mutex> g(g_failedMtx);
        g_failedModules.push_back(h);
    }

    // Major part of a module's file version (the first number of a version like 2604.8002.200.6000), read from its
    // version resource. Returns 0 when it cannot be read.
    static WORD ModuleVersionMajor(HMODULE h)
    {
        HRSRC res = FindResourceW(h, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
        if (!res)
            return 0;
        HGLOBAL mem = LoadResource(h, res);
        const BYTE *data = mem ? (const BYTE *)LockResource(mem) : nullptr;
        const DWORD size = SizeofResource(h, res);
        // VS_VERSIONINFO: WORD length, WORD valueLength, WORD type, the key "VS_VERSION_INFO", padding to 4 bytes,
        // then the VS_FIXEDFILEINFO value.
        const size_t fixedOffset = (6 + sizeof(L"VS_VERSION_INFO") + 3) & ~(size_t)3;
        if (!data || size < fixedOffset + sizeof(VS_FIXEDFILEINFO))
            return 0;
        const VS_FIXEDFILEINFO *fi = (const VS_FIXEDFILEINFO *)(data + fixedOffset);
        if (fi->dwSignature != 0xFEEF04BD)
            return 0;
        return HIWORD(fi->dwFileVersionMS);
    }

    static bool IsTaskbarViewModule(HMODULE h)
    {
        wchar_t path[MAX_PATH] = {};
        if (!GetModuleFileNameW(h, path, MAX_PATH))
            return false;
        const wchar_t *name = wcsrchr(path, L'\\');
        return name && _wcsicmp(name + 1, L"Taskbar.View.dll") == 0;
    }

    static bool TryHookModule(HMODULE h)
    {
        std::lock_guard<std::recursive_mutex> hookLock(g_hookMtx);
        if (!h || g_hooksReady.load())
            return g_hooksReady.load();
        if (ModuleAlreadyFailed(h))
            return false;
        // Starting with Taskbar.View.dll 2604.8002.200.6000 the SystemTray types live in SystemTray.dll, so there is
        // nothing to look up in Taskbar.View.dll from that version on. (An unreadable version is still tried.)
        if (IsTaskbarViewModule(h))
        {
            const WORD major = ModuleVersionMajor(h);
            if (major >= 2604)
            {
                RememberFailedModule(h);
                Wh_Log(L"Skipping Taskbar.View.dll version %d", (int)major);
                return false;
            }
        }
        // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
        WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::DateTimeIconContent,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
                &g_origOAT,
                OAT_Hook,
            },
            {
                // Optional: lets WinKlok adopt a clock that already exists when the mod is enabled.
                // If this build has no such symbol, existing clocks are picked up after an Explorer restart.
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::BadgeIconContent,struct winrt::SystemTray::IBadgeIconContent>::get_ViewModel(void * *))"},
                &g_origGVM,
                GVM_Hook,
                true,
            },
        };
        wchar_t path[MAX_PATH] = {};
        GetModuleFileNameW(h, path, MAX_PATH);
        if (!WindhawkUtils::HookSymbols(h, systemTrayHooks, ARRAYSIZE(systemTrayHooks)))
        {
            RememberFailedModule(h);
            Wh_Log(L"DateTimeIconContent::OnApplyTemplate not found in %s", path);
            return false;
        }
        g_hooksReady = true;
        Wh_Log(L"DateTimeIconContent::OnApplyTemplate hooked in %s", path);
        return true;
    }

    // Where the SystemTray types live depends on the Windows build: SystemTray.dll in recent builds,
    // Taskbar.View.dll or ExplorerExtensions.dll in older ones. The first one that has them is used.
    static const wchar_t *const kCandidateModules[] = {L"SystemTray.dll", L"Taskbar.View.dll", L"ExplorerExtensions.dll"};

    static bool TryHookAnyLoadedModule()
    {
        bool clockHookReady = false;
        for (auto name : kCandidateModules)
        {
            HMODULE h = GetModuleHandleW(name);
            if (!h)
                continue;
            if (!g_hooksReady.load())
                TryHookModule(h);
            if (g_hooksReady.load())
                clockHookReady = true;
        }
        return clockHookReady;
    }

    using LoadLibraryExW_t = decltype(&LoadLibraryExW);
    static LoadLibraryExW_t g_origLoadLibraryExW = nullptr;

    static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags)
    {
        HMODULE m = g_origLoadLibraryExW(name, file, flags);
        if (m && name && !g_shuttingDown.load())
        {
            const wchar_t *base = wcsrchr(name, L'\\');
            base = base ? base + 1 : name;
            for (auto cand : kCandidateModules)
            {
                if (_wcsicmp(base, cand) != 0)
                    continue;
                bool changed = false;
                if (!g_hooksReady.load() && TryHookModule(m))
                    changed = true;
                if (changed)
                    Wh_ApplyHookOperations();
                break;
            }
        }
        return m;
    }

    static bool InstallHooks()
    {
        if (TryHookAnyLoadedModule())
            return true;
        Wh_Log(L"taskbar modules not loaded yet; waiting for them");
        HMODULE kb = GetModuleHandleW(L"kernelbase.dll");
        void *target = kb ? (void *)GetProcAddress(kb, "LoadLibraryExW") : nullptr;
        if (!target)
            return false;
        return WindhawkUtils::SetFunctionHook((LoadLibraryExW_t)target, LoadLibraryExW_Hook, &g_origLoadLibraryExW) != 0;
    }

    static void CleanupAll()
    {
        std::vector<HWND> wnds;
        {
            std::lock_guard<std::mutex> g(g_msgMtx);
            for (auto &kv : g_msgWnds)
                wnds.push_back(kv.second);
        }
        for (HWND h : wnds)
        {
            // A plain SendMessage: the taskbar thread never waits on the Windhawk thread, so this cannot deadlock,
            // and the unload must not continue while that thread could still run code from this module.
            SendMessageW(h, WM_TD_CLEANUP, 0, 0);
        }
        {
            // Each taskbar thread has now released its own XAML references. Reset only when
            // no entries remain, so their thread-affine objects can never be destroyed here.
            std::lock_guard<std::mutex> g(g_entMtx);
            if (g_entries && g_entries->empty())
                g_entries.reset(); // also releases the vector's retained allocation
        }
        if (g_msgClassRegistered)
        {
            UnregisterClassW(kMsgClass, ModInst());
            g_msgClassRegistered = false;
        }
    }

} // namespace tb

// ============================================================================
//  NATIVE UI PRIMITIVES
//  Plain Win32 + Direct2D/DirectWrite rendering, theme, acrylic, hit-testing, input.
// ============================================================================
namespace ui
{

    static ID2D1Factory *g_d2d = nullptr;
    static IDWriteFactory *g_dw = nullptr;
    static std::wstring g_uiFont = L"Segoe UI";
    static std::wstring g_iconFont = L"Segoe MDL2 Assets";
    static std::map<std::wstring, IDWriteTextFormat *> g_fmt;
    static const wchar_t kUiClass[] = L"WinklokUiWindow";
    static bool g_uiClassRegistered = false;
    static std::atomic<HWND> g_panelWnd{nullptr};
    static std::atomic<ULONGLONG> g_clockTogglePendingUntil{0};
    static HANDLE g_thread = nullptr;
    static DWORD g_threadId = 0;

    enum : UINT
    {
        WM_UI_TOGGLE = WM_APP + 20,
        WM_UI_QUIT = WM_APP + 21,
        WM_UI_CLOSEALERT = WM_APP + 22,
        WM_UI_WHEEL = WM_APP + 23,
        WM_UI_HWHEEL = WM_APP + 24,
        WM_UI_SYNCDONE = WM_APP + 25
    };
    enum
    {
        TIMER_TICK = 1,
        TIMER_DEACTIVATE_CHECK,
        TIMER_PANEL_ANIM,
        TIMER_CONTENT_ANIM,
        TIMER_INTERACTION_ANIM,
        TIMER_SCROLL_ANIM,
        TIMER_ALERT_ANIM,
        TIMER_STATUS_ANIM,
        TIMER_TAB_ANIM,
        TIMER_NOTE_SAVE,
        TIMER_ABOUT_ANIM
    };

    // ---- colors ---------------------------------------------------------------------
    using Col = D2D1_COLOR_F;
    static Col C(float r, float g, float b, float a = 1.f) { return Col{r, g, b, a}; }
    static Col Hex(unsigned rgb, float a = 1.f)
    {
        return C(((rgb >> 16) & 255) / 255.f, ((rgb >> 8) & 255) / 255.f, (rgb & 255) / 255.f, a);
    }
    static Col Mix(Col a, Col b, float t)
    {
        t = std::min(1.f, std::max(0.f, t));
        return C(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t, a.a + (b.a - a.a) * t);
    }
    static Col Fade(Col c, float t)
    {
        c.a *= std::min(1.f, std::max(0.f, t));
        return c;
    } // same colour, scaled alpha

    // Easing helpers shared by panel, content, hover, tab and scroll transitions.
    static float Clamp01(float t) { return std::min(1.f, std::max(0.f, t)); }
    static float EaseOutCubic(float t)
    {
        t = Clamp01(t);
        return 1.f - std::pow(1.f - t, 3.f);
    }
    static float EaseInOutCubic(float t)
    {
        t = Clamp01(t);
        return t < 0.5f ? 4.f * t * t * t : 1.f - std::pow(-2.f * t + 2.f, 3.f) / 2.f;
    }

    static float EaseOutQuart(float t)
    {
        t = Clamp01(t);
        return 1.f - std::pow(1.f - t, 4.f);
    }
    static float EaseInCubic(float t)
    {
        t = Clamp01(t);
        return t * t * t;
    }

    // Frame-rate independent glide towards a target. Every scrollable surface (wheel, precision touchpad,
    // touch momentum, calendar months, laps, notes) uses this, so they all share one gliding feel.
    struct Smooth
    {
        float pos = 0.f, target = 0.f;
        bool Active() const { return std::fabs(target - pos) > 0.02f; }
        void Jump(float v) { pos = target = v; }
        // Advances by dtMs; tauMs is the time constant (smaller = snappier). Returns true while moving.
        bool Step(float dtMs, float tauMs)
        {
            if (!Active())
            {
                pos = target;
                return false;
            }
            pos += (target - pos) * (1.f - std::exp(-dtMs / tauMs));
            if (std::fabs(target - pos) < 0.04f)
                pos = target;
            return Active();
        }
    };

    // A tiny from -> to tween (t0 == 0 means settled).
    struct Anim
    {
        float from = 0.f, to = 0.f;
        ULONGLONG t0 = 0;
        float Value(float durMs, bool out) const
        {
            if (!t0)
                return to;
            float t = (float)(GetTickCount64() - t0) / durMs;
            return from + (to - from) * (out ? EaseOutCubic(t) : EaseInOutCubic(t));
        }
        bool Running(float durMs) const { return t0 && (float)(GetTickCount64() - t0) < durMs; }
        void Go(float target, float current)
        {
            from = current;
            to = target;
            t0 = GetTickCount64();
        }
        void Set(float v)
        {
            from = to = v;
            t0 = 0;
        }
    };

    static bool RegDword(HKEY root, const wchar_t *sub, const wchar_t *name, DWORD &out)
    {
        DWORD sz = sizeof(DWORD), type = 0;
        return RegGetValueW(root, sub, name, RRF_RT_REG_DWORD, &type, &out, &sz) == ERROR_SUCCESS;
    }

    // Windows personalization: Settings > Personalization > Colors > Transparency effects.
    static bool TransparencyEffectsEnabled()
    {
        DWORD enabled = 0;
        return RegDword(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                        L"EnableTransparency", enabled) &&
               enabled != 0;
    }

    // Windows derives a seven-step accent palette from the colour chosen in Settings > Personalization
    // (Light3, Light2, Light1, Base, Dark1, Dark2, Dark3) and stores it in the registry. Shell surfaces and
    // Microsoft apps do not paint with the base colour: dark theme uses Light2, light theme uses Dark1.
    static bool ReadAccentPalette(Col out[7])
    {
        BYTE b[32] = {};
        DWORD sz = sizeof(b), type = 0;
        if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", L"AccentPalette",
                         RRF_RT_REG_BINARY, &type, b, &sz) != ERROR_SUCCESS ||
            sz < 28)
            return false;
        for (int i = 0; i < 7; ++i) // stored as R, G, B, A bytes
            out[i] = Hex(((unsigned)b[i * 4] << 16) | ((unsigned)b[i * 4 + 1] << 8) | (unsigned)b[i * 4 + 2]);
        return true;
    }

    struct Palette
    {
        Col bg, card, cardHover, text, sub, accent, accentHover, onAccent, border, danger, dangerHover, track, field;
        bool dark = true, trans = false;
    };

    // trans: surfaces become translucent so the acrylic backdrop shows through.
    static Palette LoadPalette(bool trans = false)
    {
        Palette p;
        DWORD light = 0;
        bool haveLight = RegDword(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                                  L"SystemUsesLightTheme", light);
        p.dark = !(haveLight && light);
        p.trans = trans;
        unsigned rgb = 0x0078D4;
        DWORD acc = 0;
        if (RegDword(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"AccentColor", acc))
        {
            unsigned r = acc & 255, g = (acc >> 8) & 255, b = (acc >> 16) & 255; // stored as ABGR
            rgb = (r << 16) | (g << 8) | b;
        }
        const Col base = Hex(rgb);
        Col steps[7];
        if (!ReadAccentPalette(steps))
        {                                               // palette missing: approximate the Windows derivation from the base colour
            steps[1] = Mix(base, Hex(0xFFFFFF), 0.38f); // Light2
            steps[4] = Mix(base, Hex(0x000000), 0.22f); // Dark1
        }
        p.accent = p.dark ? steps[1] : steps[4];
        p.onAccent = p.dark ? Hex(0x000000) : Hex(0xFFFFFF); // text on accent, as in WinUI
        if (p.dark)
        {
            p.bg = Hex(0x202020);
            p.card = Hex(0x2B2B2B);
            p.cardHover = Hex(0x373737);
            p.text = Hex(0xFFFFFF);
            p.sub = Hex(0xC8C8C8);
            p.border = Hex(0x3C3C3C);
            p.track = Hex(0x505050);
            p.field = Hex(0x2B2B2B);
            if (trans)
            {
                p.bg = Hex(0x1F1F1F, 0.70f);
                p.card = Hex(0xFFFFFF, 0.055f);
                p.cardHover = Hex(0xFFFFFF, 0.10f);
                p.border = Hex(0xFFFFFF, 0.09f);
                p.track = Hex(0xFFFFFF, 0.22f);
            }
        }
        else
        {
            p.bg = Hex(0xF3F3F3);
            p.card = Hex(0xFFFFFF);
            p.cardHover = Hex(0xE9E9E9);
            p.text = Hex(0x1A1A1A);
            p.sub = Hex(0x5E5E5E);
            p.border = Hex(0xE0E0E0);
            p.track = Hex(0xCFCFCF);
            p.field = Hex(0xFFFFFF);
            if (trans)
            {
                p.bg = Hex(0xF3F3F3, 0.54f);
                p.card = Hex(0xFFFFFF, 0.50f);
                p.cardHover = Hex(0xFFFFFF, 0.80f);
                p.border = Hex(0x000000, 0.08f);
                p.track = Hex(0x000000, 0.18f);
            }
        }
        p.danger = Hex(0xC42B1C);
        p.accentHover = p.dark ? Mix(p.accent, Hex(0x202020), 0.10f) : Mix(p.accent, Hex(0xFFFFFF), 0.10f); // 90% accent, like WinUI
        p.dangerHover = Mix(p.danger, Hex(0xFFFFFF), 0.14f);
        return p;
    }

    static D2D1_RECT_F RF(float x, float y, float w, float h) { return D2D1_RECT_F{x, y, x + w, y + h}; }
    static D2D1_RECT_F Inset(D2D1_RECT_F r, float dx, float dy) { return D2D1_RECT_F{r.left + dx, r.top + dy, r.right - dx, r.bottom - dy}; }
    static float RW(const D2D1_RECT_F &r) { return r.right - r.left; }
    static float RH(const D2D1_RECT_F &r) { return r.bottom - r.top; }

    // ---- locale/date helpers ----------------------------------------------------------------
    static std::wstring DateStr(const SYSTEMTIME &st, const wchar_t *fmt)
    {
        wchar_t b[96] = {};
        if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, fmt, b, 96, nullptr))
            b[0] = 0;
        return b;
    }
    static int FirstDayOfWeekSun()
    { // 0 = Sunday ... 6 = Saturday
        int v = 0;
        if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK | LOCALE_RETURN_NUMBER, (LPWSTR)&v,
                            sizeof(v) / sizeof(WCHAR)) == 0)
            return 0;
        return (v + 1) % 7; // Windows: 0 = Monday
    }
    static int DaysInMonth(int y, int m)
    {
        static const int d[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0))
            return 29;
        return d[(m - 1) % 12];
    }
    static int WeekdayOf(int y, int m, int d)
    { // 0 = Sunday
        SYSTEMTIME s{};
        s.wYear = (WORD)y;
        s.wMonth = (WORD)m;
        s.wDay = (WORD)d;
        s.wHour = 12;
        FILETIME ft;
        SystemTimeToFileTime(&s, &ft);
        SYSTEMTIME o{};
        FileTimeToSystemTime(&ft, &o);
        return o.wDayOfWeek;
    }
    static std::wstring WeekdayName(int dow, const wchar_t *fmt)
    { // dow 0 = Sunday
        SYSTEMTIME s{};
        s.wYear = 2024;
        s.wMonth = 1;
        s.wDay = (WORD)(7 + dow); // 2024-01-07 is a Sunday
        return DateStr(s, fmt);
    }

    // ---- fonts / text -------------------------------------------------------------------------
    static bool FontExists(const wchar_t *name)
    {
        IDWriteFontCollection *coll = nullptr;
        if (FAILED(g_dw->GetSystemFontCollection(&coll, FALSE)) || !coll)
            return false;
        UINT32 idx = 0;
        BOOL exists = FALSE;
        coll->FindFamilyName(name, &idx, &exists);
        coll->Release();
        return exists != FALSE;
    }

    static IDWriteTextFormat *GetFormat(float size, int weight, int halign, int valign, bool icon, bool wrap = false)
    {
        wchar_t key[64];
        _snwprintf(key, 64, L"%d|%d|%d|%d|%d|%d", (int)(size * 10), weight, halign, valign, icon ? 1 : 0, wrap ? 1 : 0);
        key[63] = 0;
        auto it = g_fmt.find(key);
        if (it != g_fmt.end())
            return it->second;
        IDWriteTextFormat *f = nullptr;
        HRESULT hr = g_dw->CreateTextFormat((icon ? g_iconFont : g_uiFont).c_str(), nullptr, (DWRITE_FONT_WEIGHT)weight,
                                            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"en-us", &f);
        if (FAILED(hr) || !f)
            return nullptr;
        f->SetTextAlignment(halign == 0   ? DWRITE_TEXT_ALIGNMENT_LEADING
                            : halign == 1 ? DWRITE_TEXT_ALIGNMENT_CENTER
                                          : DWRITE_TEXT_ALIGNMENT_TRAILING);
        f->SetParagraphAlignment(valign == 0 ? DWRITE_PARAGRAPH_ALIGNMENT_NEAR : DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        f->SetWordWrapping(wrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);
        g_fmt[key] = f;
        return f;
    }

    static float MeasureText(const std::wstring &s, float size, int weight)
    {
        IDWriteTextFormat *f = GetFormat(size, weight, 0, 0, false);
        if (!f)
            return (float)s.size() * size * 0.55f;
        IDWriteTextLayout *tl = nullptr;
        if (FAILED(g_dw->CreateTextLayout(s.c_str(), (UINT32)s.size(), f, 1000.f, 100.f, &tl)) || !tl)
            return (float)s.size() * size * 0.55f;
        DWRITE_TEXT_METRICS m{};
        tl->GetMetrics(&m);
        tl->Release();
        return m.widthIncludingTrailingWhitespace;
    }

    static void ReleaseFormats()
    {
        for (auto &kv : g_fmt)
            if (kv.second)
                kv.second->Release();
        g_fmt.clear();
    }

    // ---- base window ---------------------------------------------------------------------------
    struct Hit
    {
        D2D1_RECT_F r;
        int id;
        int param;
    };

    static void RouteTouchpadTo(HWND target); // defined after SetTouchpadWindowCapability

    class UiWindow
    {
    public:
        HWND hwnd = nullptr;
        ID2D1HwndRenderTarget *rt = nullptr;
        ID2D1SolidColorBrush *br = nullptr;
        UINT dpi = 96;
        Palette pal = LoadPalette();
        std::vector<Hit> hits;
        int hoverId = 0, hoverParam = 0, downId = 0, downParam = 0;
        bool tracking = false;
        bool ownedByWindow = false;
        // touchpad gesture state
        struct TpContact
        {
            POINT himetric{};
            ULONGLONG tick = 0;
        };
        std::map<UINT32, TpContact> tpContacts;
        bool tpHaveCentroid = false, tpScrolling = false;
        float tpCx = 0.f, tpCy = 0.f, tpVelX = 0.f, tpVelY = 0.f, tpLastX = 0.f, tpLastY = 0.f;
        ULONGLONG tpLastMove = 0;
        ULONGLONG lastTouchpadTick = 0; // wheel messages that duplicate a tracked gesture are ignored for a moment
        bool alphaTarget = false;       // per-pixel alpha render target (acrylic panel)
        // Grayscale text anti-aliasing for every translucent surface. ClearType writes colour fringes and
        // invalid premultiplied pixels into the alpha channel; DWM then shows them as black / white
        // "static" that shimmers while the content moves.
        bool grayText = false;
        float masterOpacity = 1.f; // fades everything drawn (used when alphaTarget is on)
        // Touch/pen contacts are handled here so a tap stays a normal click while a drag becomes
        // a native-feeling pan (axis 1 = vertical, 2 = horizontal; chosen by the derived window).
        UINT32 activePointerId = 0;
        bool pointerContact = false, pointerPanning = false, pointerRightDown = false;
        int pointerPanAxis = 0;
        float pointerDownX = 0.f, pointerDownY = 0.f, pointerLastX = 0.f, pointerLastY = 0.f;
        int pointerDownHitId = 0, pointerDownHitParam = 0;
        float drawOpacity = 1.f;
        struct HoverTrack
        {
            float from = 0.f, to = 0.f;
            ULONGLONG started = 0;
        };
        std::map<std::pair<int, int>, HoverTrack> hoverTracks;
        struct ToggleTrack
        {
            float from = 0.f, to = 0.f;
            ULONGLONG started = 0;
            bool initialized = false;
        };
        std::map<std::pair<int, int>, ToggleTrack> toggleTracks;

        virtual ~UiWindow() { ReleaseTarget(); }
        virtual void Paint() = 0;
        virtual void OnClick(int id, int param, bool right) = 0;
        // delta follows WM_MOUSEWHEEL (positive = wheel up / right); works for wheel, touchpad and pen.
        virtual void OnWheel(int /*delta*/, float /*x*/, float /*y*/, bool /*horizontal*/) {}
        virtual int PointerPanAxisAt(float /*x*/, float /*y*/) const { return 0; }
        // dx/dy are finger movement since the last event, in DIPs.
        virtual void OnPointerPan(float /*dx*/, float /*dy*/, float /*x*/, float /*y*/) {}
        virtual void OnPointerPanEnd() {} // finger / pen lifted after a pan (used for momentum)
        // Two-finger precision-touchpad scrolling, tracked from the raw touchpad pointers (see HandleTouchpadPointer).
        virtual void OnTouchpadScroll(float /*dxDip*/, float /*dyDip*/, float /*x*/, float /*y*/) {}
        virtual void OnTouchpadScrollEnd(float /*vxDipPerMs*/, float /*vyDipPerMs*/, float /*x*/, float /*y*/) {}
        virtual void OnDpiChanged() {}
        // Maps pointer coordinates into a translated/scrolling content surface before hit-testing.
        virtual float HitTestY(float y) const { return y; }

        float Scale() const { return dpi / 96.f; }

        void EndTouchpadGesture()
        {
            if (tpScrolling)
            {
                const bool fresh = GetTickCount64() - tpLastMove < 90; // a rested finger carries no momentum
                OnTouchpadScrollEnd(fresh ? tpVelX : 0.f, fresh ? tpVelY : 0.f, tpLastX, tpLastY);
            }
            tpScrolling = false;
            tpHaveCentroid = false;
            tpVelX = tpVelY = 0.f;
            tpLastMove = 0; // do not let a previous gesture's timestamp skew the next velocity sample
        }

        // Windows delivers a precision touchpad to applications as PT_TOUCHPAD pointers with no wheel messages
        // and no usable gesture API on some systems. Each contact is tracked by pointer id from its HIMETRIC
        // position; the movement of the centroid of two or more fingers becomes scroll deltas.
        // Returns true if the pointer was a touchpad pointer (the caller still lets Windows process it so the
        // cursor and clicks keep working).
        bool HandleTouchpadPointer(UINT msg, WPARAM wp)
        {
            const UINT32 id = GET_POINTERID_WPARAM(wp);
            POINTER_INPUT_TYPE type{};
            if (!GetPointerType(id, &type) || (int)type != 5 /*PT_TOUCHPAD*/)
                return false;
            POINTER_INFO info{};
            if (!GetPointerInfo(id, &info))
                return true;
            const ULONGLONG now = GetTickCount64();
            for (auto it = tpContacts.begin(); it != tpContacts.end();) // forget contacts whose UP was lost
                if (now - it->second.tick > 600)
                    it = tpContacts.erase(it);
                else
                    ++it;

            const bool down = msg != WM_POINTERUP && (info.pointerFlags & POINTER_FLAG_INCONTACT);
            if (!down)
            {
                tpContacts.erase(id);
                if (tpContacts.size() < 2)
                    EndTouchpadGesture();
                else
                    tpHaveCentroid = false;
                return true;
            }
            const bool isNew = tpContacts.find(id) == tpContacts.end();
            tpContacts[id] = TpContact{info.ptHimetricLocation, now};
            if (isNew)
                tpHaveCentroid = false; // the finger count changed: restart from the new centroid
            if (tpContacts.size() < 2)
            {
                EndTouchpadGesture();
                return true;
            }
            float cx = 0.f, cy = 0.f;
            for (auto &c : tpContacts)
            {
                cx += (float)c.second.himetric.x;
                cy += (float)c.second.himetric.y;
            }
            cx /= (float)tpContacts.size();
            cy /= (float)tpContacts.size();
            if (tpHaveCentroid)
            {
                // 2540 HIMETRIC = 1 inch = 96 DIP; the gain makes a short swipe cover a comfortable distance.
                const float k = 96.f / 2540.f * 2.4f;
                const float dx = (cx - tpCx) * k, dy = (cy - tpCy) * k;
                if (dx != 0.f || dy != 0.f)
                {
                    const float dt = (float)std::min<ULONGLONG>(50, std::max<ULONGLONG>(1, now - (tpLastMove ? tpLastMove : now - 8)));
                    tpVelX = 0.7f * tpVelX + 0.3f * dx / dt;
                    tpVelY = 0.7f * tpVelY + 0.3f * dy / dt;
                    tpLastMove = now;
                    lastTouchpadTick = now;
                    ClientDip(info.ptPixelLocation, tpLastX, tpLastY);
                    tpScrolling = true;
                    OnTouchpadScroll(dx, dy, tpLastX, tpLastY);
                }
            }
            tpCx = cx;
            tpCy = cy;
            tpHaveCentroid = true;
            return true;
        }

        void ReleaseTarget()
        {
            if (br)
            {
                br->Release();
                br = nullptr;
            }
            if (rt)
            {
                rt->Release();
                rt = nullptr;
            }
        }

        bool EnsureTarget()
        {
            if (rt)
                return true;
            if (!g_d2d || !hwnd)
                return false;
            RECT rc;
            GetClientRect(hwnd, &rc);
            D2D1_HWND_RENDER_TARGET_PROPERTIES hp{};
            hp.hwnd = hwnd;
            hp.pixelSize.width = (UINT32)std::max<LONG>(1, rc.right - rc.left);
            hp.pixelSize.height = (UINT32)std::max<LONG>(1, rc.bottom - rc.top);
            hp.presentOptions = D2D1_PRESENT_OPTIONS_NONE;
            for (int attempt = 0; attempt < 2 && !rt; ++attempt)
            {
                D2D1_RENDER_TARGET_PROPERTIES rtp{};
                rtp.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
                const bool premul = alphaTarget && attempt == 0;
                rtp.pixelFormat.format = premul ? DXGI_FORMAT_B8G8R8A8_UNORM : DXGI_FORMAT_UNKNOWN;
                rtp.pixelFormat.alphaMode = premul ? D2D1_ALPHA_MODE_PREMULTIPLIED : D2D1_ALPHA_MODE_UNKNOWN;
                rtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;
                rtp.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
                if (FAILED(g_d2d->CreateHwndRenderTarget(&rtp, &hp, &rt)))
                    rt = nullptr;
                if (!premul)
                    break;
            }
            if (!rt)
                return false;
            rt->SetDpi((float)dpi, (float)dpi);
            rt->SetTextAntialiasMode(grayText || alphaTarget ? D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE : D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
            Col c = pal.text;
            if (FAILED(rt->CreateSolidColorBrush(&c, nullptr, &br)) || !br)
            {
                ReleaseTarget();
                return false;
            }
            return true;
        }

        void Render()
        {
            if (!EnsureTarget())
                return;
            hits.clear();
            rt->BeginDraw();
            Col clear = alphaTarget ? C(0, 0, 0, 0) : pal.bg;
            rt->Clear(&clear);
            if (alphaTarget)
            { // translucent surface painted with straight alpha over the acrylic backdrop
                drawOpacity = 1.f;
                const D2D1_SIZE_F sz = rt->GetSize(); // exactly the surface, no oversized geometry
                Fill(RF(0, 0, sz.width, sz.height), pal.bg);
            }
            Paint();
            if (rt->EndDraw() == D2DERR_RECREATE_TARGET)
                ReleaseTarget();
        }

        void Invalidate()
        {
            if (hwnd)
                InvalidateRect(hwnd, nullptr, FALSE);
        }

        float TrackValue(const HoverTrack &t) const
        {
            float v = EaseInOutCubic((float)(GetTickCount64() - t.started) / 115.f);
            return t.from + (t.to - t.from) * v;
        }

        void SetHoverTarget(int id, int param)
        {
            if (id == hoverId && param == hoverParam)
                return;
            const ULONGLONG now = GetTickCount64();
            auto retarget = [&](int i, int p, float to, float defFrom)
            {
                const auto key = std::make_pair(i, p);
                auto it = hoverTracks.find(key);
                float from = it != hoverTracks.end() ? TrackValue(it->second) : defFrom;
                hoverTracks[key] = HoverTrack{from, to, now};
            };
            if (hoverId != 0)
                retarget(hoverId, hoverParam, 0.f, 1.f);
            if (id != 0)
                retarget(id, param, 1.f, 0.f);
            hoverId = id;
            hoverParam = param;
            if (hwnd)
                SetTimer(hwnd, TIMER_INTERACTION_ANIM, 15, nullptr);
            Invalidate();
        }

        float HoverAmount(int id, int param = 0) const
        {
            if (id == 0)
                return 0.f;
            auto it = hoverTracks.find(std::make_pair(id, param));
            if (it == hoverTracks.end())
                return (id == hoverId && param == hoverParam) ? 1.f : 0.f;
            return TrackValue(it->second);
        }

        void AdvanceHoverAnimation()
        {
            const ULONGLONG now = GetTickCount64();
            bool running = false;
            for (auto it = hoverTracks.begin(); it != hoverTracks.end();)
            {
                if (now - it->second.started < 115)
                {
                    running = true;
                    ++it;
                }
                else
                    it = hoverTracks.erase(it);
            }
            for (auto &item : toggleTracks)
            {
                ToggleTrack &track = item.second;
                if (!track.started)
                    continue;
                if (now - track.started < 180)
                    running = true;
                else
                {
                    track.from = track.to;
                    track.started = 0;
                }
            }
            if (!running && hwnd)
                KillTimer(hwnd, TIMER_INTERACTION_ANIM);
            Invalidate();
        }

        // ---- drawing helpers (all in DIPs) ----
        void Color(Col c)
        {
            c.a *= Clamp01(drawOpacity) * Clamp01(masterOpacity);
            br->SetColor(&c);
        }
        void Fill(D2D1_RECT_F r, Col c)
        {
            Color(c);
            rt->FillRectangle(&r, br);
        }
        void RoundFill(D2D1_RECT_F r, float rad, Col c)
        {
            Color(c);
            D2D1_ROUNDED_RECT rr{r, rad, rad};
            rt->FillRoundedRectangle(&rr, br);
        }
        void RoundStroke(D2D1_RECT_F r, float rad, Col c, float w = 1.f)
        {
            Color(c);
            D2D1_ROUNDED_RECT rr{D2D1_RECT_F{r.left + w / 2, r.top + w / 2, r.right - w / 2, r.bottom - w / 2}, rad, rad};
            rt->DrawRoundedRectangle(&rr, br, w, nullptr);
        }
        void Circle(float cx, float cy, float rad, Col c)
        {
            Color(c);
            D2D1_ELLIPSE e{D2D1_POINT_2F{cx, cy}, rad, rad};
            rt->FillEllipse(&e, br);
        }
        void CircleStroke(float cx, float cy, float rad, Col c, float w)
        {
            Color(c);
            D2D1_ELLIPSE e{D2D1_POINT_2F{cx, cy}, rad, rad};
            rt->DrawEllipse(&e, br, w, nullptr);
        }
        void Line(float x1, float y1, float x2, float y2, Col c, float w = 1.f)
        {
            Color(c);
            rt->DrawLine(D2D1_POINT_2F{x1, y1}, D2D1_POINT_2F{x2, y2}, br, w, nullptr);
        }
        void Text(const std::wstring &s, D2D1_RECT_F r, float size, int weight, Col c, int halign = 0, int valign = 1,
                  bool icon = false, bool wrap = false)
        {
            if (s.empty())
                return;
            IDWriteTextFormat *f = GetFormat(size, weight, halign, valign, icon, wrap);
            if (!f)
                return;
            Color(c);
            rt->DrawText(s.c_str(), (UINT32)s.size(), f, &r, br, D2D1_DRAW_TEXT_OPTIONS_CLIP, DWRITE_MEASURING_MODE_NATURAL);
        }
        void AddHit(D2D1_RECT_F r, int id, int param = 0) { hits.push_back({r, id, param}); }
        bool IsHover(int id, int param = 0) const { return hoverId == id && hoverParam == param; }

        // style: 0 card, 1 accent, 2 flat, 3 danger
        void Btn(D2D1_RECT_F r, int id, int param, const std::wstring &label, int style, float size = 14.f)
        {
            const float hov = HoverAmount(id, param);
            Col fill, fg = pal.text;
            if (style == 1)
            {
                fill = Mix(pal.accent, pal.accentHover, hov);
                fg = pal.onAccent;
            }
            else if (style == 3)
            {
                fill = Mix(pal.danger, pal.dangerHover, hov);
                fg = Hex(0xFFFFFF);
            }
            else if (style == 0)
                fill = Mix(pal.card, pal.cardHover, hov);
            else
                fill = Fade(pal.cardHover, hov);
            RoundFill(r, 6.f, fill);
            if (style == 0)
                RoundStroke(r, 6.f, pal.border, 1.f);
            Text(label, r, size, 600, fg, 1, 1);
            AddHit(r, id, param);
        }
        void Chip(D2D1_RECT_F r, int id, int param, const std::wstring &label, bool selected)
        {
            const float hov = HoverAmount(id, param);
            RoundFill(r, RH(r) / 2, selected ? Mix(pal.accent, pal.accentHover, hov) : Mix(pal.card, pal.cardHover, hov));
            if (!selected)
                RoundStroke(r, RH(r) / 2, pal.border, 1.f);
            Text(label, r, 12.f, selected ? 600 : 400, selected ? pal.onAccent : pal.text, 1, 1);
            AddHit(r, id, param);
        }
        void IconBtn(float cx, float cy, float diam, int id, int param, const wchar_t *glyph, int style, float glyphSize = 16.f)
        {
            D2D1_RECT_F r = RF(cx - diam / 2, cy - diam / 2, diam, diam);
            const float hov = HoverAmount(id, param);
            Col fill, fg = pal.text;
            if (style == 1)
            {
                fill = Mix(pal.accent, pal.accentHover, hov);
                fg = pal.onAccent;
            }
            else if (style == 3)
            {
                fill = Mix(pal.danger, pal.dangerHover, hov);
                fg = Hex(0xFFFFFF);
            }
            else
                fill = Mix(pal.card, pal.cardHover, hov);
            Circle(cx, cy, diam / 2, fill);
            if (style == 0)
                CircleStroke(cx, cy, diam / 2 - 0.5f, pal.border, 1.f);
            Text(glyph, r, glyphSize, 400, fg, 1, 1, true);
            AddHit(r, id, param);
        }
        void FlatIcon(D2D1_RECT_F r, int id, int param, const wchar_t *glyph, float glyphSize = 14.f)
        {
            const float hov = HoverAmount(id, param);
            if (hov > 0.001f)
                RoundFill(r, 6.f, Fade(pal.cardHover, hov));
            Text(glyph, r, glyphSize, 400, pal.text, 1, 1, true);
            AddHit(r, id, param);
        }
        void Toggle(float x, float y, int id, bool on, int param = 0)
        {
            D2D1_RECT_F r = RF(x, y, 40.f, 20.f);
            const auto key = std::make_pair(id, param);
            const float target = on ? 1.f : 0.f;
            ToggleTrack &track = toggleTracks[key];
            auto sample = [&]()
            {
                if (!track.started)
                    return track.to;
                return track.from + (track.to - track.from) * EaseOutCubic((float)(GetTickCount64() - track.started) / 180.f);
            };
            if (!track.initialized)
            {
                track.from = track.to = target;
                track.started = 0;
                track.initialized = true;
            }
            else if (std::fabs(track.to - target) > 0.001f)
            {
                track.from = sample();
                track.to = target;
                track.started = GetTickCount64();
                if (hwnd)
                    SetTimer(hwnd, TIMER_INTERACTION_ANIM, 15, nullptr);
            }
            float position = sample();
            if (track.started && GetTickCount64() - track.started >= 180)
            {
                track.from = track.to;
                track.started = 0;
                position = track.to;
            }
            const float hov = HoverAmount(id, param);
            Col trackFill = Mix(Fade(pal.accent, 0.f), pal.accent, position);
            trackFill = Mix(trackFill, pal.accentHover, hov * position);
            RoundFill(r, 10.f, trackFill);
            RoundStroke(r, 10.f, Mix(pal.sub, pal.accent, position), 1.2f);
            Circle(x + 10.f + 20.f * position, y + 10.f, 5.6f + 0.8f * hov, Mix(pal.sub, pal.onAccent, position));
            AddHit(D2D1_RECT_F{x - 6, y - 6, x + 46, y + 26}, id, param);
        }
        void Progress(D2D1_RECT_F r, float frac)
        {
            RoundFill(r, RH(r) / 2, pal.track);
            frac = Clamp01(frac);
            if (frac > 0.f)
            {
                D2D1_RECT_F f = r;
                f.right = r.left + std::max(RH(r), RW(r) * frac);
                RoundFill(f, RH(r) / 2, pal.accent);
            }
        }

        // ---- input ----
        bool HitAt(float x, float y, int &id, int &param) const
        {
            y = HitTestY(y);
            for (size_t i = hits.size(); i-- > 0;)
            {
                const Hit &h = hits[i];
                if (x >= h.r.left && x < h.r.right && y >= h.r.top && y < h.r.bottom)
                {
                    id = h.id;
                    param = h.param;
                    return true;
                }
            }
            return false;
        }

        // Screen-space pointer position (physical px) -> client DIPs.
        void ClientDip(POINT pt, float &x, float &y) const
        {
            ScreenToClient(hwnd, &pt);
            x = pt.x * 96.f / dpi;
            y = pt.y * 96.f / dpi;
        }

        virtual LRESULT HandleMessage(UINT msg, WPARAM wp, LPARAM lp)
        {
            switch (msg)
            {
            case WM_ERASEBKGND:
                return 1;
            case WM_TIMER:
                if (wp == TIMER_INTERACTION_ANIM)
                {
                    AdvanceHoverAnimation();
                    return 0;
                }
                break;
            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                BeginPaint(hwnd, &ps);
                Render();
                EndPaint(hwnd, &ps);
                return 0;
            }
            case WM_SIZE:
                if (rt)
                {
                    RECT rc;
                    GetClientRect(hwnd, &rc);
                    D2D1_SIZE_U sz{(UINT32)std::max<LONG>(1, rc.right), (UINT32)std::max<LONG>(1, rc.bottom)};
                    rt->Resize(&sz);
                }
                return 0;
            case WM_DPICHANGED:
                dpi = HIWORD(wp);
                ReleaseTarget();
                OnDpiChanged();
                Invalidate();
                return 0;
            case WM_MOUSEMOVE:
            {
                if (hwnd == g_panelWnd.load())
                    RouteTouchpadTo(hwnd); // the cursor is over the panel surface itself, not a child edit
                if (!tracking)
                {
                    TRACKMOUSEEVENT t{sizeof(t), TME_LEAVE, hwnd, 0};
                    TrackMouseEvent(&t);
                    tracking = true;
                }
                float x = GET_X_LPARAM(lp) * 96.f / dpi, y = GET_Y_LPARAM(lp) * 96.f / dpi;
                int id = 0, p = 0;
                HitAt(x, y, id, p);
                if (id != hoverId || p != hoverParam)
                    SetHoverTarget(id, p);
                return 0;
            }
            case WM_MOUSELEAVE:
                tracking = false;
                if (hoverId)
                    SetHoverTarget(0, 0);
                return 0;
            // Wheel input from a mouse, a precision touchpad (many small deltas) or a pen. Both the
            // legacy and the pointer flavours are handled so no input device is ever dropped.
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
            case WM_POINTERWHEEL:
            case WM_POINTERHWHEEL:
            case WM_UI_WHEEL:
            case WM_UI_HWHEEL:
            {
                const bool horizontal = msg == WM_MOUSEHWHEEL || msg == WM_POINTERHWHEEL || msg == WM_UI_HWHEEL;
                const bool viaHook = msg == WM_UI_WHEEL || msg == WM_UI_HWHEEL;
                const int delta = viaHook ? (int)(INT_PTR)wp : (int)GET_WHEEL_DELTA_WPARAM(wp);
                if (!viaHook && GetTickCount64() - lastTouchpadTick < 350)
                    return 0; // suppress only a possible OS-generated duplicate, never the physical-mouse hook route
                static int logged = 0;
                if (logged < 12)
                {
                    ++logged;
                    Wh_Log(L"wheel delta=%d via %s", delta, viaHook ? L"mouse hook" : L"window message");
                }
                float x, y;
                ClientDip(POINT{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)}, x, y);
                OnWheel(delta, x, y, horizontal);
                return 0;
            }
            case WM_POINTERDOWN:
            {
                if (HandleTouchpadPointer(msg, wp))
                    break;
                const UINT32 pointerId = GET_POINTERID_WPARAM(wp);
                POINTER_INPUT_TYPE pointerType{};
                POINTER_INFO info{};
                if (!GetPointerType(pointerId, &pointerType) || (pointerType != PT_TOUCH && pointerType != PT_PEN) ||
                    !GetPointerInfo(pointerId, &info))
                    break;
                float x, y;
                ClientDip(info.ptPixelLocation, x, y);
                activePointerId = pointerId;
                pointerContact = true;
                pointerPanning = false;
                pointerRightDown = (info.pointerFlags & POINTER_FLAG_SECONDBUTTON) != 0;
                pointerDownX = pointerLastX = x;
                pointerDownY = pointerLastY = y;
                pointerDownHitId = pointerDownHitParam = 0;
                HitAt(x, y, pointerDownHitId, pointerDownHitParam);
                pointerPanAxis = PointerPanAxisAt(x, y);
                if (pointerPanAxis)
                {
                    SetCapture(hwnd);
                    return 0;
                }
                // Non-scrollable areas keep Windows' normal pen/touch mouse-compatibility behaviour.
                pointerContact = false;
                activePointerId = 0;
                break;
            }
            case WM_POINTERUPDATE:
            {
                if (HandleTouchpadPointer(msg, wp))
                    break;
                const UINT32 pointerId = GET_POINTERID_WPARAM(wp);
                if (!pointerContact || pointerId != activePointerId)
                    break;
                POINTER_INFO info{};
                if (!GetPointerInfo(pointerId, &info) || !(info.pointerFlags & POINTER_FLAG_INCONTACT))
                    return 0;
                float x, y;
                ClientDip(info.ptPixelLocation, x, y);
                const float dx = x - pointerDownX, dy = y - pointerDownY;
                if (!pointerPanning)
                {
                    const bool vert = pointerPanAxis == 1 && std::fabs(dy) >= 7.f && std::fabs(dy) >= std::fabs(dx) * 0.8f;
                    const bool horz = pointerPanAxis == 2 && std::fabs(dx) >= 7.f && std::fabs(dx) >= std::fabs(dy) * 0.8f;
                    if (vert || horz)
                    {
                        pointerPanning = true;
                        pointerDownHitId = pointerDownHitParam = 0;
                    }
                }
                if (pointerPanning)
                    OnPointerPan(x - pointerLastX, y - pointerLastY, x, y);
                pointerLastX = x;
                pointerLastY = y;
                return 0;
            }
            case WM_POINTERUP:
            {
                if (HandleTouchpadPointer(msg, wp))
                    break;
                const UINT32 pointerId = GET_POINTERID_WPARAM(wp);
                if (!pointerContact || pointerId != activePointerId)
                    break;
                // lParam carries the screen position too: a reliable tap location if the pointer info expired.
                POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                POINTER_INFO info{};
                if (GetPointerInfo(pointerId, &info))
                    pt = info.ptPixelLocation;
                if (GetCapture() == hwnd)
                    ReleaseCapture();
                if (pointerPanning)
                    OnPointerPanEnd();
                if (!pointerPanning)
                {
                    float x, y;
                    ClientDip(pt, x, y);
                    int id = 0, param = 0;
                    HitAt(x, y, id, param);
                    if (id != 0 && id == pointerDownHitId && param == pointerDownHitParam)
                        OnClick(id, param, pointerRightDown);
                }
                pointerContact = pointerPanning = pointerRightDown = false;
                activePointerId = 0;
                pointerDownHitId = pointerDownHitParam = 0;
                return 0;
            }
            case WM_POINTERCAPTURECHANGED:
                pointerContact = pointerPanning = pointerRightDown = false;
                activePointerId = 0;
                pointerDownHitId = pointerDownHitParam = 0;
                return 0;
            case WM_LBUTTONDOWN:
            {
                SetCapture(hwnd);
                float x = GET_X_LPARAM(lp) * 96.f / dpi, y = GET_Y_LPARAM(lp) * 96.f / dpi;
                downId = 0;
                downParam = 0;
                HitAt(x, y, downId, downParam);
                return 0;
            }
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            {
                if (msg == WM_LBUTTONUP && GetCapture() == hwnd)
                    ReleaseCapture();
                float x = GET_X_LPARAM(lp) * 96.f / dpi, y = GET_Y_LPARAM(lp) * 96.f / dpi;
                int id = 0, p = 0;
                HitAt(x, y, id, p);
                bool right = msg == WM_RBUTTONUP;
                bool ok = id != 0 && (right || (id == downId && p == downParam));
                downId = 0;
                downParam = 0;
                if (ok)
                    OnClick(id, p, right);
                return 0;
            }
            }
            return DefWindowProcW(hwnd, msg, wp, lp);
        }
    };

    // Windows 11 precision-touchpad opt-in. Without registering the panel HWND, Windows does not
    // guarantee that PT_TOUCHPAD pointer messages reach HandleTouchpadPointer, even though the raw
    // pointer-contact tracker itself is implemented correctly. Resolve the API dynamically so this
    // single-file mod does not depend on a newer Windows SDK declaration/import library.
    using WkRegisterTouchpadCapableWindow_t = BOOL(WINAPI *)(HWND, BOOL);
    static HWND g_touchpadCapableWindow = nullptr;

    static WkRegisterTouchpadCapableWindow_t ResolveRegisterTouchpadCapableWindow()
    {
        static WkRegisterTouchpadCapableWindow_t fn = []() -> WkRegisterTouchpadCapableWindow_t
        {
            HMODULE user32 = GetModuleHandleW(L"user32.dll");
            if (!user32)
                return nullptr;
            // Resolved by name only: an export that is not found simply disables the direct touchpad tracking
            // (two-finger scrolling then uses the wheel hook).
            return reinterpret_cast<WkRegisterTouchpadCapableWindow_t>(GetProcAddress(user32, "RegisterTouchpadCapableWindow"));
        }();
        return fn;
    }

    static bool SetTouchpadWindowCapability(HWND hwnd, BOOL enable)
    {
        auto fn = ResolveRegisterTouchpadCapableWindow();
        return hwnd && fn && fn(hwnd, enable) != FALSE;
    }

    // Precision-touchpad pointers are only delivered to the window that is registered as touchpad-capable, and the
    // registration is last-caller-wins. Child EDIT controls (the notes editor) therefore never saw the two-finger
    // gesture. The registration is handed to whichever of the two (panel surface / body editor) is under the cursor.
    static HWND g_touchpadPanelWindow = nullptr; // the panel HWND that owns the registration lifetime
    // The registration exists only while the panel is on screen: it is taken when the panel opens and given back
    // when it has finished closing, so a hidden panel never holds on to the system's touchpad routing.
    static void RouteTouchpadTo(HWND target)
    {
        if (!target || !g_touchpadPanelWindow || target == g_touchpadCapableWindow)
            return;
        if (!IsWindowVisible(g_touchpadPanelWindow))
            return;
        if (g_touchpadCapableWindow && IsWindow(g_touchpadCapableWindow))
            SetTouchpadWindowCapability(g_touchpadCapableWindow, FALSE);
        if (SetTouchpadWindowCapability(target, TRUE))
            g_touchpadCapableWindow = target;
        else if (SetTouchpadWindowCapability(g_touchpadPanelWindow, TRUE))
            g_touchpadCapableWindow = g_touchpadPanelWindow;
        else
        {
            g_touchpadCapableWindow = nullptr;
            static bool logged = false;
            if (!logged)
            {
                logged = true;
                Wh_Log(L"touchpad-capable registration failed (%u); two-finger scrolling uses the wheel hook", GetLastError());
            }
        }
    }

    static void ReleaseTouchpadCapability()
    {
        if (g_touchpadCapableWindow && IsWindow(g_touchpadCapableWindow))
            SetTouchpadWindowCapability(g_touchpadCapableWindow, FALSE);
        g_touchpadCapableWindow = nullptr;
    }

    static LRESULT CALLBACK UiWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        UiWindow *self = nullptr;
        if (msg == WM_NCCREATE)
        {
            self = (UiWindow *)((CREATESTRUCTW *)lp)->lpCreateParams;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)self);
            if (self)
                self->hwnd = hwnd;
        }
        else
        {
            self = (UiWindow *)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        }
        if (!self)
            return DefWindowProcW(hwnd, msg, wp, lp);
        if (msg == WM_NCDESTROY)
        {
            // Unregister before the HWND is destroyed. RegisterTouchpadCapableWindow is last-caller-wins,
            // so only undo the registration for the exact panel window we registered.
            if (hwnd == g_touchpadPanelWindow)
            {
                if (g_touchpadCapableWindow)
                {
                    if (g_touchpadCapableWindow != hwnd && IsWindow(g_touchpadCapableWindow))
                        SetTouchpadWindowCapability(g_touchpadCapableWindow, FALSE);
                    if (!SetTouchpadWindowCapability(hwnd, FALSE))
                        Wh_Log(L"touchpad-capable unregistration failed (%u)", GetLastError());
                }
                g_touchpadCapableWindow = nullptr;
                g_touchpadPanelWindow = nullptr;
            }
            LRESULT r = DefWindowProcW(hwnd, msg, wp, lp);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            self->hwnd = nullptr;
            if (self->ownedByWindow)
                delete self;
            return r;
        }
        return self->HandleMessage(msg, wp, lp);
    }

    static void DwmStyle(HWND hwnd, const Palette &pal)
    {
        DWORD round = 2; // DWMWCP_ROUND
        DwmSetWindowAttribute(hwnd, 33, &round, sizeof(round));
        BOOL dark = pal.dark ? TRUE : FALSE; // DWMWA_USE_IMMERSIVE_DARK_MODE
        DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
        COLORREF b = pal.dark ? RGB(60, 60, 60) : RGB(224, 224, 224);
        DwmSetWindowAttribute(hwnd, 34, &b, sizeof(b)); // DWMWA_BORDER_COLOR
    }

    // SetWindowCompositionAttribute is undocumented, so it is resolved at runtime and only used
    // when the supported DWM system-backdrop attribute is unavailable (pre-22H2 Windows 11).
    struct TdAccentPolicy
    {
        int state;
        int flags;
        DWORD gradientColor;
        int animationId;
    };
    struct TdWindowCompositionAttribData
    {
        int attribute;
        void *data;
        SIZE_T size;
    };
    using TdSetWindowCompositionAttribute = BOOL(WINAPI *)(HWND, TdWindowCompositionAttribData *);

    static bool SetLegacyAcrylic(HWND hwnd, bool enabled, const Palette &pal)
    {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        auto setComposition = user32 ? reinterpret_cast<TdSetWindowCompositionAttribute>(
                                           GetProcAddress(user32, "SetWindowCompositionAttribute"))
                                     : nullptr;
        if (!setComposition)
            return false;
        TdAccentPolicy policy{};
        if (enabled)
        {
            policy.state = 4; // ACCENT_ENABLE_ACRYLICBLURBEHIND
            policy.flags = 2;
            auto ch = [](float v)
            { return (DWORD)std::min(255, std::max(0, (int)(v * 255.f))); };
            // AABBGGRR; the panel paints its own translucent tint, so the accent tint stays light.
            policy.gradientColor = (0x40u << 24) | (ch(pal.bg.b) << 16) | (ch(pal.bg.g) << 8) | ch(pal.bg.r);
        }
        TdWindowCompositionAttribData data{19, &policy, sizeof(policy)}; // WCA_ACCENT_POLICY
        return setComposition(hwnd, &data) != FALSE;
    }

    // Live blur of whatever is behind the window, set through the window's own composition attributes. Unlike the DWM
    // system backdrop (DWMWA_SYSTEMBACKDROP_TYPE) it is stored with the window, so it survives hiding and showing,
    // does not depend on the window being active, and is clipped by the window region. The panel paints the acrylic
    // tint itself on top of it.
    static bool SetLiveBlur(HWND hwnd, bool enabled)
    {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        auto setComposition = user32 ? reinterpret_cast<TdSetWindowCompositionAttribute>(
                                           GetProcAddress(user32, "SetWindowCompositionAttribute"))
                                     : nullptr;
        if (!setComposition)
            return false;
        TdAccentPolicy policy{};
        policy.state = enabled ? 3 : 0;                                  // ACCENT_ENABLE_BLURBEHIND / ACCENT_DISABLED
        TdWindowCompositionAttribData data{19, &policy, sizeof(policy)}; // WCA_ACCENT_POLICY
        return setComposition(hwnd, &data) != FALSE;
    }

    // Panel materials (mode): 0 = opaque layered window, 1 = simple transparency (layered, alpha 236, no blur),
    // 2 = acrylic: a non-layered per-pixel-alpha surface (frame extended over the whole client area)
    // on top of a live blur-behind of the desktop (SetLiveBlur) with the panel's own acrylic tint. Returns the resting layered alpha for modes 0 and 1.
    static BYTE ConfigureComposition(HWND hwnd, const Palette &pal, int mode)
    {
        if (!hwnd)
            return 255;
        LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (mode == 2)
        {
            // Acrylic needs a normal per-pixel-alpha HWND, not WS_EX_LAYERED. Apply the style change
            // after choosing the backdrop so the frame is redrawn with the new composition in one pass.
            const bool styleChanged = (ex & WS_EX_LAYERED) != 0;
            if (styleChanged)
                SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex & ~(LONG_PTR)WS_EX_LAYERED);
            MARGINS full{-1, -1, -1, -1};
            DwmExtendFrameIntoClientArea(hwnd, &full);
            const DWORD none = 1; // DWMSBT_NONE: the DWM system backdrop is not used (it went flat and ignored clipping)
            DwmSetWindowAttribute(hwnd, 38, &none, sizeof(none));
            SetLegacyAcrylic(hwnd, false, pal);
            if (!SetLiveBlur(hwnd, true))
            {                             // composition API unavailable: fall back to the system acrylic backdrop
                const DWORD backdrop = 3; // DWMSBT_TRANSIENTWINDOW (Desktop Acrylic)
                if (FAILED(DwmSetWindowAttribute(hwnd, 38, &backdrop, sizeof(backdrop))))
                    SetLegacyAcrylic(hwnd, true, pal);
            }
            if (styleChanged)
                SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            return 255;
        }

        // Modes 0 and 1 intentionally have no DWM blur. Mode 1 is simple alpha transparency only.
        MARGINS none{0, 0, 0, 0};
        DwmExtendFrameIntoClientArea(hwnd, &none);
        const DWORD backdrop = 1; // DWMSBT_NONE
        DwmSetWindowAttribute(hwnd, 38, &backdrop, sizeof(backdrop));
        SetLegacyAcrylic(hwnd, false, pal);
        SetLiveBlur(hwnd, false);
        const BYTE alpha = mode == 1 ? 236 : 255;
        const bool styleChanged = (ex & WS_EX_LAYERED) == 0;
        if (styleChanged)
            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
        if (styleChanged)
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        return alpha;
    }

    // ---- wheel capture ---------------------------------------------------------------------------
    // Many laptop touchpads (precision and vendor drivers alike) never deliver their two-finger scroll to
    // this popup as window messages, although mouse wheels do. A low-level mouse hook sees every wheel
    // event wherever it is routed, so while the panel is open, wheel events over it are taken here and
    // handed to the panel directly. The hook exists only while the panel is visible.
    static HHOOK g_wheelHook = nullptr;

    static LRESULT CALLBACK LowLevelWheelProc(int code, WPARAM wp, LPARAM lp)
    {
        if (code == HC_ACTION && (wp == WM_MOUSEWHEEL || wp == WM_MOUSEHWHEEL))
        {
            HWND h = g_panelWnd.load();
            const MSLLHOOKSTRUCT *m = (const MSLLHOOKSTRUCT *)lp;
            RECT r;
            if (h && IsWindowVisible(h) && GetWindowRect(h, &r) && PtInRect(&r, m->pt))
            {
                HWND at = WindowFromPoint(m->pt);
                if (at && (at == h || IsChild(h, at)))
                {
                    PostMessageW(h, wp == WM_MOUSEWHEEL ? WM_UI_WHEEL : WM_UI_HWHEEL, (WPARAM)(INT_PTR)(short)HIWORD(m->mouseData),
                                 MAKELPARAM((WORD)(short)m->pt.x, (WORD)(short)m->pt.y));
                    return 1; // consumed: the panel scrolls itself
                }
            }
        }
        return CallNextHookEx(nullptr, code, wp, lp);
    }
    static void InstallWheelHook()
    {
        if (g_wheelHook)
            return;
        g_wheelHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelWheelProc, ModInst(), 0);
        if (!g_wheelHook)
            Wh_Log(L"could not install the wheel hook (%u); wheel messages are used instead", GetLastError());
    }
    static void RemoveWheelHook()
    {
        if (g_wheelHook)
        {
            UnhookWindowsHookEx(g_wheelHook);
            g_wheelHook = nullptr;
        }
    }

    static bool RegisterUiClass()
    {
        if (g_uiClassRegistered)
            return true;
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.style = CS_DROPSHADOW;
        wc.lpfnWndProc = UiWndProc;
        wc.hInstance = ModInst();
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kUiClass;
        if (!RegisterClassExW(&wc))
            return false;
        g_uiClassRegistered = true;
        return true;
    }

    // ============================================================================
    //  ALARM SOUNDS
    //  Built-in sounds are synthesised once (additive synthesis with per-partial decay)
    //  into an in-memory 16-bit WAV, so there are no files to ship and every sound loops
    //  seamlessly. Sound 0 keeps the classic Windows alarm file.
    // ============================================================================
    namespace snd
    {

        struct Timbre
        {
            int n;
            float ratio[6], amp[6], dec[6];
            float k, attack;
        };
        static const Timbre kTimbre[] = {
            /* 0 bell     */ {4, {1, 2.76f, 5.4f, 8.93f}, {1, .5f, .25f, .12f}, {1, 1.6f, 2.4f, 3.4f}, 1.7f, .004f},
            /* 1 marimba  */ {3, {1, 3.97f, 9.9f}, {1, .4f, .12f}, {1, 3.f, 6.f}, 5.5f, .003f},
            /* 2 harp     */ {5, {1, 2, 3, 4, 5}, {1, .55f, .3f, .18f, .1f}, {1, 1.3f, 1.6f, 2.f, 2.4f}, 2.8f, .003f},
            /* 3 pad      */ {3, {1, 2, 3}, {1, .3f, .12f}, {1, 1.4f, 2.f}, .75f, .5f},
            /* 4 digital  */ {2, {1, 2}, {1, .18f}, {1, 1.5f}, 9.f, .004f},
            /* 5 musicbox */ {3, {1, 2.01f, 4.98f}, {1, .45f, .2f}, {1, 2.f, 3.f}, 3.6f, .002f},
            /* 6 gong     */ {5, {1, 1.47f, 2.09f, 2.56f, 3.2f}, {1, .7f, .55f, .4f, .25f}, {1, 1.1f, 1.25f, 1.5f, 1.9f}, .65f, .02f},
        };

        struct Tone
        {
            float f, t, v;
            int tb;
        }; // frequency (Hz), start (s), volume, timbre
        struct Def
        {
            float total;
            std::vector<Tone> notes;
        };

        // Index = sound id - 1.
        static const std::vector<Def> kDefs = {
            /* 1 Crystal Bells */ {4.6f, {{1046.5f, 0, .9f, 0}, {1318.5f, .24f, .85f, 0}, {1568.f, .48f, .85f, 0}, {2093.f, .72f, .8f, 0}, {1568.f, 1.f, .65f, 0}, {1318.5f, 1.24f, .65f, 0}, {1046.5f, 1.5f, .6f, 0}}},
            /* 2 Soft Chimes   */ {5.0f, {{1318.5f, 0, .8f, 0}, {1046.5f, .6f, .75f, 0}, {784.f, 1.2f, .75f, 0}, {880.f, 1.8f, .65f, 0}, {1046.5f, 2.4f, .6f, 0}}},
            /* 3 Marimba       */ {3.4f, {{523.25f, 0, .9f, 1}, {659.25f, .2f, .9f, 1}, {784.f, .4f, .9f, 1}, {659.25f, .6f, .9f, 1}, {1046.5f, .8f, .9f, 1}, {784.f, 1.f, .9f, 1}, {659.25f, 1.2f, .9f, 1}, {784.f, 1.4f, .9f, 1}, {587.33f, 1.7f, .9f, 1}, {784.f, 1.9f, .9f, 1}, {987.77f, 2.1f, .9f, 1}, {784.f, 2.3f, .9f, 1}, {1174.7f, 2.5f, .9f, 1}}},
            /* 4 Golden Harp   */ {4.0f, {{523.25f, 0, .8f, 2}, {659.25f, .2f, .8f, 2}, {784.f, .4f, .8f, 2}, {987.77f, .6f, .8f, 2}, {1174.7f, .8f, .8f, 2}, {987.77f, 1.f, .7f, 2}, {784.f, 1.2f, .7f, 2}, {659.25f, 1.4f, .7f, 2}, {1046.5f, 1.8f, .9f, 2}, {1318.5f, 1.8f, .6f, 2}}},
            /* 5 Sunrise       */ {7.0f, {{261.63f, 0, .7f, 3}, {329.63f, .7f, .6f, 3}, {392.f, 1.4f, .6f, 3}, {523.25f, 2.1f, .55f, 3}, {659.25f, 2.8f, .5f, 3}}},
            /* 6 Digital Pulse */ {2.4f, {{880.f, 0, .8f, 4}, {880.f, .2f, .8f, 4}, {880.f, .4f, .8f, 4}, {1174.7f, .9f, .8f, 4}, {1174.7f, 1.1f, .8f, 4}, {1174.7f, 1.3f, .8f, 4}}},
            /* 7 Music Box     */ {4.8f, {{1046.5f, 0, .8f, 5}, {784.f, .28f, .8f, 5}, {1318.5f, .56f, .8f, 5}, {1046.5f, .84f, .8f, 5}, {1568.f, 1.12f, .8f, 5}, {1318.5f, 1.4f, .8f, 5}, {2093.f, 1.68f, .8f, 5}, {1568.f, 1.96f, .8f, 5}, {1046.5f, 2.6f, .7f, 5}, {1318.5f, 2.9f, .7f, 5}, {1568.f, 3.2f, .7f, 5}}},
            /* 8 Zen Gong      */ {7.0f, {{196.f, 0, 1.f, 6}, {1318.5f, 2.2f, .35f, 0}, {1046.5f, 2.9f, .3f, 0}}},
        };

        static std::vector<BYTE> Render(const Def &d)
        {
            const int sr = 22050;
            const int N = (int)(d.total * sr);
            std::vector<float> buf(N, 0.f);
            for (const Tone &n : d.notes)
            {
                const Timbre &tm = kTimbre[n.tb];
                const int s0 = (int)(n.t * sr);
                const int len = (int)(std::min(d.total - n.t, 6.5f / tm.k) * sr);
                for (int i = 0; i < len && s0 + i < N; ++i)
                {
                    const float t = (float)i / sr;
                    float s = 0.f;
                    for (int p = 0; p < tm.n; ++p)
                    {
                        if (n.f * tm.ratio[p] > 10000.f)
                            continue; // stay below Nyquist
                        s += tm.amp[p] * std::sin(6.2831853f * n.f * tm.ratio[p] * t) * std::exp(-tm.k * tm.dec[p] * t);
                    }
                    buf[s0 + i] += s * std::min(1.f, t / tm.attack) * n.v;
                }
            }
            float peak = 1e-6f;
            for (float v : buf)
                peak = std::max(peak, std::fabs(v));
            const int fade = (int)(0.08f * sr); // silent seam so the loop never clicks
            std::vector<BYTE> w(44 + (size_t)N * 2);
            auto put32 = [&](int o, uint32_t v)
            { memcpy(&w[o], &v, 4); };
            auto put16 = [&](int o, uint16_t v)
            { memcpy(&w[o], &v, 2); };
            memcpy(&w[0], "RIFF", 4);
            put32(4, 36 + N * 2);
            memcpy(&w[8], "WAVEfmt ", 8);
            put32(16, 16);
            put16(20, 1);
            put16(22, 1);
            put32(24, sr);
            put32(28, sr * 2);
            put16(32, 2);
            put16(34, 16);
            memcpy(&w[36], "data", 4);
            put32(40, N * 2);
            for (int i = 0; i < N; ++i)
            {
                float v = buf[i] / peak * 0.8f;
                if (i > N - fade)
                    v *= (float)(N - i) / fade;
                put16(44 + i * 2, (uint16_t)(int16_t)std::lround(v * 32767.f));
            }
            return w;
        }

        static std::map<int, std::vector<BYTE>> g_waves; // UI thread only
        static const std::vector<BYTE> *Wave(int id)
        {
            if (id < 1 || id > (int)kDefs.size())
                return nullptr;
            auto it = g_waves.find(id);
            if (it == g_waves.end())
                it = g_waves.emplace(id, Render(kDefs[id - 1])).first;
            return &it->second;
        }

        static bool g_ringing = false; // a looping alert sound is playing

        static void Play(int sound, bool loop)
        {
            if (const auto *w = Wave(sound))
            {
                PlaySoundW((LPCWSTR)w->data(), nullptr, SND_MEMORY | SND_ASYNC | SND_NODEFAULT | (loop ? SND_LOOP : 0));
                return;
            }
            wchar_t path[MAX_PATH] = {};
            GetWindowsDirectoryW(path, MAX_PATH);
            wcscat(path, L"\\Media\\Alarm01.wav");
            if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES)
            {
                GetWindowsDirectoryW(path, MAX_PATH);
                wcscat(path, L"\\Media\\Ring01.wav");
            }
            if (GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES)
                PlaySoundW(path, nullptr, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | (loop ? SND_LOOP : 0));
            else
                PlaySoundW(L"SystemExclamation", nullptr, SND_ALIAS | SND_ASYNC | (loop ? SND_LOOP : 0));
        }
        static void Stop() { PlaySoundW(nullptr, nullptr, 0); }

    } // namespace snd

} // namespace ui

// ============================================================================
//  RICH TEXT + LaTeX ENGINE (notes preview)
//  A markdown subset (headings, bold/italic/strike, lists, quotes, code, tables, links) plus
//  TeX math ($..$, $$..$$, \(..\), \[..\], align/matrix/cases environments) is laid out into
//  a flat list of draw operations and painted with Direct2D/DirectWrite in the panel theme.
//  Everything is self-contained: no browser engine, no network, no external files.
// ============================================================================
namespace ui
{
    namespace rich
    {

        enum Fam
        {
            F_UI = 0,
            F_MONO = 1,
            F_MATH = 2
        };
        enum
        {
            C_TEXT = 0,
            C_SUB,
            C_ACCENT,
            C_CODEBG,
            C_BAR,
            C_BORDER,
            C_HEAD
        };
        static std::wstring g_monoFont = L"Consolas", g_mathFont = L"Cambria Math";

        struct Style
        {
            float size = 14.f;
            int weight = 400;
            bool italic = false;
            int fam = F_UI;
            int color = 0;
        };
        enum OpKind
        {
            OP_TEXT,
            OP_RECT,
            OP_LINE
        };
        // OP_TEXT: x, y = baseline, x2 = width; OP_RECT: x,y,x2,y2 + th = corner radius; OP_LINE: x,y -> x2,y2 + th.
        struct Op
        {
            OpKind kind = OP_TEXT;
            float x = 0, y = 0, x2 = 0, y2 = 0, th = 1.f, sx = 1.f, sy = 1.f;
            int color = 0;
            Style st;
            std::wstring s;
            std::wstring url; // non-empty on the text of a hyperlink
        };
        struct FontInfo
        {
            IDWriteTextFormat *fmt = nullptr;
            float baseline = 0, height = 0;
        };

        static std::map<std::wstring, FontInfo> g_fonts;
        static std::unordered_map<std::wstring, float> g_widths;

        static const FontInfo &Font(const Style &st)
        {
            wchar_t key[64];
            _snwprintf(key, 64, L"%d|%d|%d|%d", (int)(st.size * 10), st.weight, st.italic ? 1 : 0, st.fam);
            key[63] = 0;
            auto it = g_fonts.find(key);
            if (it != g_fonts.end())
                return it->second;
            FontInfo fi;
            const std::wstring &fam = st.fam == F_MONO ? g_monoFont : st.fam == F_MATH ? g_mathFont
                                                                                       : g_uiFont;
            if (g_dw && SUCCEEDED(g_dw->CreateTextFormat(fam.c_str(), nullptr, (DWRITE_FONT_WEIGHT)st.weight, st.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, st.size, L"en-us", &fi.fmt)) && fi.fmt)
            {
                fi.fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                IDWriteTextLayout *tl = nullptr;
                if (SUCCEEDED(g_dw->CreateTextLayout(L"Hxgy", 4, fi.fmt, 1000.f, 1000.f, &tl)) && tl)
                {
                    DWRITE_LINE_METRICS lm[2];
                    UINT32 n = 0;
                    tl->GetLineMetrics(lm, 2, &n);
                    if (n >= 1)
                    {
                        fi.baseline = lm[0].baseline;
                        fi.height = lm[0].height;
                    }
                    tl->Release();
                }
            }
            if (fi.height <= 0.f)
            {
                fi.baseline = st.size * 0.8f;
                fi.height = st.size * 1.25f;
            }
            return g_fonts.emplace(key, fi).first->second;
        }

        static float Width(const std::wstring &s, const Style &st)
        {
            if (s.empty())
                return 0.f;
            const FontInfo &fi = Font(st);
            float w = (float)s.size() * st.size * 0.55f;
            if (!fi.fmt)
                return w;
            std::wstring key = std::to_wstring((int)(st.size * 10)) + L"|" + std::to_wstring(st.weight) + (st.italic ? L"i" : L"n") +
                               std::to_wstring(st.fam) + L"|" + s;
            auto it = g_widths.find(key);
            if (it != g_widths.end())
                return it->second;
            if (g_widths.size() > 40000)
                g_widths.clear();
            IDWriteTextLayout *tl = nullptr;
            if (SUCCEEDED(g_dw->CreateTextLayout(s.c_str(), (UINT32)s.size(), fi.fmt, 100000.f, fi.height * 2.f, &tl)) && tl)
            {
                DWRITE_TEXT_METRICS m{};
                tl->GetMetrics(&m);
                w = m.widthIncludingTrailingWhitespace;
                tl->Release();
            }
            g_widths[key] = w;
            return w;
        }

        static void Release()
        {
            for (auto &kv : g_fonts)
                if (kv.second.fmt)
                    kv.second.fmt->Release();
            g_fonts.clear();
            g_widths.clear();
        }

        static Col OpColor(const Palette &p, int c)
        {
            switch (c)
            {
            case C_SUB:
                return p.sub;
            case C_ACCENT:
                return p.accent;
            case C_CODEBG:
                return Fade(p.text, 0.09f);
            case C_BAR:
                return Fade(p.accent, 0.85f);
            case C_BORDER:
                return Fade(p.text, 0.20f);
            case C_HEAD:
                return Fade(p.text, 0.07f);
            default:
                return p.text;
            }
        }

        // Paints the visible part of an op list. ox/oy translate content space into the current target space;
        // vTop/vBot (target space) cull everything outside the viewport.
        static void DrawOps(UiWindow &w, const std::vector<Op> &ops, float ox, float oy, float vTop, float vBot)
        {
            D2D1_MATRIX_3X2_F m0;
            w.rt->GetTransform(&m0);
            for (const Op &o : ops)
            {
                if (o.kind == OP_RECT)
                {
                    if (o.y2 + oy < vTop || o.y + oy > vBot)
                        continue;
                    const D2D1_RECT_F r{o.x + ox, o.y + oy, o.x2 + ox, o.y2 + oy};
                    if (o.th > 0.f)
                        w.RoundFill(r, o.th, OpColor(w.pal, o.color));
                    else
                        w.Fill(r, OpColor(w.pal, o.color));
                }
                else if (o.kind == OP_LINE)
                {
                    if (std::max(o.y, o.y2) + oy < vTop || std::min(o.y, o.y2) + oy > vBot)
                        continue;
                    w.Line(o.x + ox, o.y + oy, o.x2 + ox, o.y2 + oy, OpColor(w.pal, o.color), o.th);
                }
                else
                {
                    const FontInfo &fi = Font(o.st);
                    if (!fi.fmt)
                        continue;
                    const float top = o.y - fi.baseline + oy;
                    if (top + fi.height * std::max(1.f, o.sy) < vTop - fi.height || top > vBot)
                        continue;
                    const D2D1_RECT_F r{o.x + ox, top, o.x + ox + o.x2 + 8.f, top + fi.height};
                    w.Color(OpColor(w.pal, o.color));
                    const bool stretched = o.sx != 1.f || o.sy != 1.f;
                    if (stretched)
                    { // stretch about the glyph's visual centre (the math axis)
                        const float cx = o.x + ox + o.x2 / 2, cy = o.y + oy - 0.25f * o.st.size;
                        D2D1_MATRIX_3X2_F m = m0;
                        m._11 = o.sx;
                        m._22 = o.sy;
                        m._31 = m0._31 + cx - o.sx * cx;
                        m._32 = m0._32 + cy - o.sy * cy;
                        w.rt->SetTransform(&m);
                    }
                    w.rt->DrawText(o.s.c_str(), (UINT32)o.s.size(), fi.fmt, &r, w.br, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
                    if (stretched)
                        w.rt->SetTransform(&m0);
                    if (!o.url.empty())
                        w.Line(o.x + ox, o.y + oy + 0.14f * o.st.size + 1.f, o.x + ox + o.x2, o.y + oy + 0.14f * o.st.size + 1.f, OpColor(w.pal, o.color), 1.f);
                }
            }
        }

        // ---- p5b: markdown + TeX layout logic. Needs: Style, Op, FontInfo, Font(), Width(), Fam, C_* ----
        // (platform independent: only Font() and Width() touch DirectWrite)

        static void AppendCp(std::wstring &s, uint32_t cp)
        {
            if (sizeof(wchar_t) == 2 && cp > 0xFFFF)
            {
                cp -= 0x10000;
                s += (wchar_t)(0xD800 + (cp >> 10));
                s += (wchar_t)(0xDC00 + (cp & 0x3FF));
            }
            else
                s += (wchar_t)cp;
        }

        static void AppendOps(std::vector<Op> &dst, const std::vector<Op> &src, float dx, float dy)
        {
            for (Op o : src)
            {
                o.x += dx;
                o.y += dy;
                if (o.kind != OP_TEXT)
                {
                    o.x2 += dx;
                    o.y2 += dy;
                } // for text x2 is the width
                dst.push_back(std::move(o));
            }
        }
        static Op MkLine(float x1, float y1, float x2, float y2, float th, int color = C_TEXT)
        {
            Op o;
            o.kind = OP_LINE;
            o.x = x1;
            o.y = y1;
            o.x2 = x2;
            o.y2 = y2;
            o.th = th;
            o.color = color;
            return o;
        }
        static Op MkRect(float l, float t, float r, float b, float rad, int color)
        {
            Op o;
            o.kind = OP_RECT;
            o.x = l;
            o.y = t;
            o.x2 = r;
            o.y2 = b;
            o.th = rad;
            o.color = color;
            return o;
        }

        // ============================================================================
        //  TeX math
        // ============================================================================
        enum
        {
            M_ORD = 0,
            M_OP,
            M_BIN,
            M_REL,
            M_OPEN,
            M_CLOSE,
            M_PUNCT,
            M_INNER
        };
        struct MBox
        {
            float w = 0, asc = 0, desc = 0;
            std::vector<Op> ops;
        }; // y: 0 = baseline, positive = down
        struct MCtx
        {
            float size = 16.f;
            int sty = 0;
            int var = 0;
        }; // sty: 0 display, 1 text, 2 script, 3 scriptscript
        struct MAtom
        {
            MBox b;
            int cls = M_ORD;
            bool space = false, big = false, limits = false;
            int forceLim = 0;
        };

        static void GlyphExtent(wchar_t ch, float size, float &asc, float &desc)
        {
            float a = .72f, d = .2f;
            if (ch >= L'0' && ch <= L'9')
            {
                a = .70f;
                d = 0;
            }
            else if (ch >= L'a' && ch <= L'z')
            {
                a = wcschr(L"acemnorsuvwxz", ch) ? .46f : .72f;
                d = wcschr(L"gjpqy", ch) ? .22f : 0.f;
            }
            else if (ch >= L'A' && ch <= L'Z')
            {
                a = .70f;
                d = ch == L'Q' ? .1f : 0.f;
            }
            else if (ch >= 0x3B1 && ch <= 0x3C9)
            {
                a = .55f;
                d = wcschr(L"\u03B2\u03B3\u03B6\u03B7\u03BC\u03BE\u03C1\u03C2\u03C6\u03C7\u03C8", ch) ? .22f : 0.f;
            }
            else if (ch >= 0x391 && ch <= 0x3A9)
            {
                a = .70f;
                d = 0;
            }
            else if (ch && wcschr(L"([{|)]}\u27E8\u27E9\u230A\u230B\u2308\u2309\u2016", ch))
            {
                a = .75f;
                d = .25f;
            }
            else if (ch == L',' || ch == L';')
            {
                a = .1f;
                d = .15f;
            }
            else if (ch == L'.' || ch == 0x22C5 || ch == 0xB7)
            {
                a = .1f;
                d = 0;
            }
            else if (ch && wcschr(L"+-=<>:\u2212\u2264\u2265\u2260\u2248\u00B1\u00D7\u00F7\u2213\u2217\u2208\u2209\u2282\u2283\u2286\u2287\u2192\u2190\u2194\u21D2\u21D0\u21D4\u21A6\u221D\u223C\u2261\u2223", ch))
            {
                a = .55f;
                d = .05f;
            }
            asc = a * size;
            desc = d * size;
        }

        static std::wstring MathAlpha(wchar_t ch, int var)
        {
            std::wstring r;
            const bool up = ch >= L'A' && ch <= L'Z', lo = ch >= L'a' && ch <= L'z';
            if (!up && !lo)
            {
                r += ch;
                return r;
            }
            const int i = up ? ch - L'A' : ch - L'a';
            uint32_t cp = ch;
            switch (var)
            {
            case 0: // math italic
                if (ch == L'h')
                    cp = 0x210E;
                else
                    cp = (up ? 0x1D434 : 0x1D44E) + i;
                break;
            case 2:
                cp = (up ? 0x1D400 : 0x1D41A) + i;
                break; // bold
            case 7:
                cp = (up ? 0x1D468 : 0x1D482) + i;
                break; // bold italic
            case 3:
            { // blackboard
                static const wchar_t *ex = L"C\u2102H\u210DN\u2115P\u2119Q\u211AR\u211DZ\u2124";
                cp = (up ? 0x1D538 : 0x1D552) + i;
                for (const wchar_t *e = ex; up && *e; e += 2)
                    if (*e == ch)
                        cp = e[1];
                break;
            }
            case 4:
            { // script
                static const wchar_t *exu = L"B\u212CE\u2130F\u2131H\u210BI\u2110L\u2112M\u2133R\u211B";
                static const wchar_t *exl = L"e\u212Fg\u210Ao\u2134";
                cp = (up ? 0x1D49C : 0x1D4B6) + i;
                for (const wchar_t *e = up ? exu : exl; *e; e += 2)
                    if (*e == ch)
                        cp = e[1];
                break;
            }
            case 5:
            { // fraktur
                static const wchar_t *ex = L"C\u212DH\u210CI\u2111R\u211CZ\u2128";
                cp = (up ? 0x1D504 : 0x1D51E) + i;
                for (const wchar_t *e = ex; up && *e; e += 2)
                    if (*e == ch)
                        cp = e[1];
                break;
            }
            default:
                break; // upright
            }
            AppendCp(r, cp);
            return r;
        }

        static MBox GlyphBox(const std::wstring &s, wchar_t metricCh, const MCtx &c, int fam = F_MATH, int weight = 400,
                             bool ital = false, float mul = 1.f, int color = C_TEXT)
        {
            MBox b;
            Style st;
            st.size = c.size * mul;
            st.fam = fam;
            st.weight = weight;
            st.italic = ital;
            st.color = color;
            const float w = Width(s, st);
            Op o;
            o.kind = OP_TEXT;
            o.x = 0;
            o.y = 0;
            o.x2 = w;
            o.st = st;
            o.s = s;
            o.color = color;
            b.ops.push_back(o);
            b.w = w;
            GlyphExtent(metricCh, st.size, b.asc, b.desc);
            return b;
        }

        struct SymDef
        {
            const wchar_t *name;
            const wchar_t *text;
            int cls;
        };
        static const SymDef kSyms[] = {
            // Greek
            {L"alpha", L"\u03B1", M_ORD},
            {L"beta", L"\u03B2", M_ORD},
            {L"gamma", L"\u03B3", M_ORD},
            {L"delta", L"\u03B4", M_ORD},
            {L"epsilon", L"\u03B5", M_ORD},
            {L"varepsilon", L"\u03F5", M_ORD},
            {L"zeta", L"\u03B6", M_ORD},
            {L"eta", L"\u03B7", M_ORD},
            {L"theta", L"\u03B8", M_ORD},
            {L"vartheta", L"\u03D1", M_ORD},
            {L"iota", L"\u03B9", M_ORD},
            {L"kappa", L"\u03BA", M_ORD},
            {L"varkappa", L"\u03F0", M_ORD},
            {L"lambda", L"\u03BB", M_ORD},
            {L"mu", L"\u03BC", M_ORD},
            {L"nu", L"\u03BD", M_ORD},
            {L"xi", L"\u03BE", M_ORD},
            {L"omicron", L"\u03BF", M_ORD},
            {L"pi", L"\u03C0", M_ORD},
            {L"varpi", L"\u03D6", M_ORD},
            {L"rho", L"\u03C1", M_ORD},
            {L"varrho", L"\u03F1", M_ORD},
            {L"sigma", L"\u03C3", M_ORD},
            {L"varsigma", L"\u03C2", M_ORD},
            {L"tau", L"\u03C4", M_ORD},
            {L"upsilon", L"\u03C5", M_ORD},
            {L"phi", L"\u03C6", M_ORD},
            {L"varphi", L"\u03D5", M_ORD},
            {L"chi", L"\u03C7", M_ORD},
            {L"psi", L"\u03C8", M_ORD},
            {L"omega", L"\u03C9", M_ORD},
            {L"Gamma", L"\u0393", M_ORD},
            {L"Delta", L"\u0394", M_ORD},
            {L"Theta", L"\u0398", M_ORD},
            {L"Lambda", L"\u039B", M_ORD},
            {L"Xi", L"\u039E", M_ORD},
            {L"Pi", L"\u03A0", M_ORD},
            {L"Sigma", L"\u03A3", M_ORD},
            {L"Upsilon", L"\u03A5", M_ORD},
            {L"Phi", L"\u03A6", M_ORD},
            {L"Psi", L"\u03A8", M_ORD},
            {L"Omega", L"\u03A9", M_ORD},
            // binary operators
            {L"times", L"\u00D7", M_BIN},
            {L"cdot", L"\u22C5", M_BIN},
            {L"pm", L"\u00B1", M_BIN},
            {L"mp", L"\u2213", M_BIN},
            {L"div", L"\u00F7", M_BIN},
            {L"ast", L"\u2217", M_BIN},
            {L"star", L"\u22C6", M_BIN},
            {L"circ", L"\u2218", M_BIN},
            {L"bullet", L"\u2219", M_BIN},
            {L"oplus", L"\u2295", M_BIN},
            {L"ominus", L"\u2296", M_BIN},
            {L"otimes", L"\u2297", M_BIN},
            {L"odot", L"\u2299", M_BIN},
            {L"cup", L"\u222A", M_BIN},
            {L"cap", L"\u2229", M_BIN},
            {L"setminus", L"\u2216", M_BIN},
            {L"wedge", L"\u2227", M_BIN},
            {L"land", L"\u2227", M_BIN},
            {L"vee", L"\u2228", M_BIN},
            {L"lor", L"\u2228", M_BIN},
            {L"sqcup", L"\u2294", M_BIN},
            {L"sqcap", L"\u2293", M_BIN},
            {L"uplus", L"\u228E", M_BIN},
            {L"dagger", L"\u2020", M_BIN},
            {L"ddagger", L"\u2021", M_BIN},
            {L"cdotp", L"\u22C5", M_BIN},
            // relations
            {L"le", L"\u2264", M_REL},
            {L"leq", L"\u2264", M_REL},
            {L"ge", L"\u2265", M_REL},
            {L"geq", L"\u2265", M_REL},
            {L"ne", L"\u2260", M_REL},
            {L"neq", L"\u2260", M_REL},
            {L"approx", L"\u2248", M_REL},
            {L"equiv", L"\u2261", M_REL},
            {L"sim", L"\u223C", M_REL},
            {L"simeq", L"\u2243", M_REL},
            {L"cong", L"\u2245", M_REL},
            {L"propto", L"\u221D", M_REL},
            {L"in", L"\u2208", M_REL},
            {L"notin", L"\u2209", M_REL},
            {L"ni", L"\u220B", M_REL},
            {L"subset", L"\u2282", M_REL},
            {L"supset", L"\u2283", M_REL},
            {L"subseteq", L"\u2286", M_REL},
            {L"supseteq", L"\u2287", M_REL},
            {L"nsubseteq", L"\u2288", M_REL},
            {L"subsetneq", L"\u228A", M_REL},
            {L"supsetneq", L"\u228B", M_REL},
            {L"parallel", L"\u2225", M_REL},
            {L"perp", L"\u22A5", M_REL},
            {L"mid", L"\u2223", M_REL},
            {L"nmid", L"\u2224", M_REL},
            {L"ll", L"\u226A", M_REL},
            {L"gg", L"\u226B", M_REL},
            {L"asymp", L"\u224D", M_REL},
            {L"doteq", L"\u2250", M_REL},
            {L"prec", L"\u227A", M_REL},
            {L"succ", L"\u227B", M_REL},
            {L"preceq", L"\u2AAF", M_REL},
            {L"succeq", L"\u2AB0", M_REL},
            {L"leqslant", L"\u2A7D", M_REL},
            {L"geqslant", L"\u2A7E", M_REL},
            {L"vdash", L"\u22A2", M_REL},
            {L"dashv", L"\u22A3", M_REL},
            {L"models", L"\u22A8", M_REL},
            {L"triangleq", L"\u225C", M_REL},
            {L"ncong", L"\u2247", M_REL},
            {L"to", L"\u2192", M_REL},
            {L"rightarrow", L"\u2192", M_REL},
            {L"leftarrow", L"\u2190", M_REL},
            {L"gets", L"\u2190", M_REL},
            {L"leftrightarrow", L"\u2194", M_REL},
            {L"Rightarrow", L"\u21D2", M_REL},
            {L"Leftarrow", L"\u21D0", M_REL},
            {L"Leftrightarrow", L"\u21D4", M_REL},
            {L"iff", L"\u27FA", M_REL},
            {L"implies", L"\u27F9", M_REL},
            {L"impliedby", L"\u27F8", M_REL},
            {L"mapsto", L"\u21A6", M_REL},
            {L"longmapsto", L"\u27FC", M_REL},
            {L"uparrow", L"\u2191", M_REL},
            {L"downarrow", L"\u2193", M_REL},
            {L"updownarrow", L"\u2195", M_REL},
            {L"Uparrow", L"\u21D1", M_REL},
            {L"Downarrow", L"\u21D3", M_REL},
            {L"longrightarrow", L"\u27F6", M_REL},
            {L"longleftarrow", L"\u27F5", M_REL},
            {L"longleftrightarrow", L"\u27F7", M_REL},
            {L"Longrightarrow", L"\u27F9", M_REL},
            {L"Longleftarrow", L"\u27F8", M_REL},
            {L"Longleftrightarrow", L"\u27FA", M_REL},
            {L"hookrightarrow", L"\u21AA", M_REL},
            {L"hookleftarrow", L"\u21A9", M_REL},
            {L"rightharpoonup", L"\u21C0", M_REL},
            {L"leftharpoonup", L"\u21BD", M_REL},
            {L"nearrow", L"\u2197", M_REL},
            {L"searrow", L"\u2198", M_REL},
            {L"nwarrow", L"\u2196", M_REL},
            {L"swarrow", L"\u2199", M_REL},
            {L"rightleftharpoons", L"\u21CC", M_REL},
            // ordinary symbols
            {L"infty", L"\u221E", M_ORD},
            {L"partial", L"\u2202", M_ORD},
            {L"nabla", L"\u2207", M_ORD},
            {L"forall", L"\u2200", M_ORD},
            {L"exists", L"\u2203", M_ORD},
            {L"nexists", L"\u2204", M_ORD},
            {L"emptyset", L"\u2205", M_ORD},
            {L"varnothing", L"\u2205", M_ORD},
            {L"hbar", L"\u210F", M_ORD},
            {L"ell", L"\u2113", M_ORD},
            {L"Re", L"\u211C", M_ORD},
            {L"Im", L"\u2111", M_ORD},
            {L"aleph", L"\u2135", M_ORD},
            {L"angle", L"\u2220", M_ORD},
            {L"triangle", L"\u25B3", M_ORD},
            {L"prime", L"\u2032", M_ORD},
            {L"degree", L"\u00B0", M_ORD},
            {L"square", L"\u25A1", M_ORD},
            {L"Box", L"\u25A1", M_ORD},
            {L"blacksquare", L"\u25A0", M_ORD},
            {L"checkmark", L"\u2713", M_ORD},
            {L"therefore", L"\u2234", M_ORD},
            {L"because", L"\u2235", M_ORD},
            {L"wp", L"\u2118", M_ORD},
            {L"imath", L"\u0131", M_ORD},
            {L"jmath", L"\u0237", M_ORD},
            {L"top", L"\u22A4", M_ORD},
            {L"bot", L"\u22A5", M_ORD},
            {L"neg", L"\u00AC", M_ORD},
            {L"lnot", L"\u00AC", M_ORD},
            {L"cdots", L"\u22EF", M_INNER},
            {L"ldots", L"\u2026", M_INNER},
            {L"dots", L"\u2026", M_INNER},
            {L"dotsc", L"\u2026", M_INNER},
            {L"dotsb", L"\u22EF", M_INNER},
            {L"vdots", L"\u22EE", M_INNER},
            {L"ddots", L"\u22F1", M_INNER},
            {L"S", L"\u00A7", M_ORD},
            {L"P", L"\u00B6", M_ORD},
            {L"copyright", L"\u00A9", M_ORD},
            {L"dag", L"\u2020", M_ORD},
            {L"ddag", L"\u2021", M_ORD},
            {L"colon", L":", M_PUNCT},
            {L"textdegree", L"\u00B0", M_ORD},
            // delimiters as plain atoms
            {L"{", L"{", M_OPEN},
            {L"}", L"}", M_CLOSE},
            {L"lbrace", L"{", M_OPEN},
            {L"rbrace", L"}", M_CLOSE},
            {L"langle", L"\u27E8", M_OPEN},
            {L"rangle", L"\u27E9", M_CLOSE},
            {L"lfloor", L"\u230A", M_OPEN},
            {L"rfloor", L"\u230B", M_CLOSE},
            {L"lceil", L"\u2308", M_OPEN},
            {L"rceil", L"\u2309", M_CLOSE},
            {L"lbrack", L"[", M_OPEN},
            {L"rbrack", L"]", M_CLOSE},
            {L"|", L"\u2016", M_ORD},
            {L"vert", L"|", M_ORD},
            {L"lvert", L"|", M_OPEN},
            {L"rvert", L"|", M_CLOSE},
            {L"Vert", L"\u2016", M_ORD},
            {L"lVert", L"\u2016", M_OPEN},
            {L"rVert", L"\u2016", M_CLOSE},
            {L"backslash", L"\\", M_ORD},
            // escaped characters
            {L"$", L"$", M_ORD},
            {L"%", L"%", M_ORD},
            {L"&", L"&", M_ORD},
            {L"#", L"#", M_ORD},
            {L"_", L"_", M_ORD},
            {L"mathbin", L"", M_ORD},
        };

        struct BigDef
        {
            const wchar_t *name;
            const wchar_t *text;
            bool limits;
            float tall;
        };
        static const BigDef kBigs[] = {
            {L"sum", L"\u2211", true, 1.f},
            {L"prod", L"\u220F", true, 1.f},
            {L"coprod", L"\u2210", true, 1.f},
            {L"bigcup", L"\u22C3", true, 1.f},
            {L"bigcap", L"\u22C2", true, 1.f},
            {L"bigvee", L"\u22C1", true, 1.f},
            {L"bigwedge", L"\u22C0", true, 1.f},
            {L"bigoplus", L"\u2A01", true, 1.f},
            {L"bigotimes", L"\u2A02", true, 1.f},
            {L"bigodot", L"\u2A00", true, 1.f},
            {L"biguplus", L"\u2A04", true, 1.f},
            {L"int", L"\u222B", false, 1.25f},
            {L"iint", L"\u222C", false, 1.25f},
            {L"iiint", L"\u222D", false, 1.25f},
            {L"oint", L"\u222E", false, 1.25f},
            {L"oiint", L"\u222F", false, 1.25f},
        };

        static const wchar_t *const kFuncs[] = {L"sin", L"cos", L"tan", L"cot", L"sec", L"csc", L"arcsin", L"arccos", L"arctan",
                                                L"sinh", L"cosh", L"tanh", L"coth", L"log", L"ln", L"lg", L"exp", L"lim", L"limsup", L"liminf", L"max", L"min",
                                                L"sup", L"inf", L"det", L"gcd", L"arg", L"deg", L"dim", L"ker", L"hom", L"Pr", L"mod", L"bmod", L"tr", L"rank"};
        static const wchar_t *const kLimitFuncs[] = {L"lim", L"limsup", L"liminf", L"max", L"min", L"sup", L"inf", L"det", L"gcd", L"Pr"};

        static int Mu(int a, int b)
        { // inter-atom spacing in mu (1/18 em)
            static const signed char t[8][8] = {
                /*ORD  */ {0, 3, 4, 5, 0, 0, 0, 3},
                /*OP   */ {3, 3, 0, 5, 0, 0, 0, 3},
                /*BIN  */ {4, 4, 0, 0, 4, 0, 0, 4},
                /*REL  */ {5, 5, 0, 0, 5, 0, 0, 5},
                /*OPEN */ {0, 0, 0, 0, 0, 0, 0, 0},
                /*CLOSE*/ {0, 3, 4, 5, 0, 0, 0, 3},
                /*PUNCT*/ {3, 3, 0, 3, 3, 3, 3, 3},
                /*INNER*/ {3, 3, 4, 5, 3, 0, 3, 3}};
            return t[a][b];
        }

        static MCtx ScriptCtx(const MCtx &c)
        {
            MCtx s = c;
            if (c.sty < 2)
            {
                s.size = c.size * 0.7f;
                s.sty = 2;
            }
            else
            {
                s.size = c.size * 0.714f;
                s.sty = 3;
            }
            return s;
        }

        static MBox Assemble(std::vector<MAtom> &at, const MCtx &c)
        {
            int prev = -1;
            for (size_t i = 0; i < at.size(); ++i)
            { // TeX: a binary operator without a left/right operand is ordinary
                if (at[i].space)
                    continue;
                if (at[i].cls == M_BIN)
                {
                    bool conv = prev < 0 || prev == M_BIN || prev == M_REL || prev == M_OPEN || prev == M_PUNCT || prev == M_OP;
                    if (!conv)
                    {
                        int nx = -1;
                        for (size_t j = i + 1; j < at.size(); ++j)
                            if (!at[j].space)
                            {
                                nx = at[j].cls;
                                break;
                            }
                        conv = nx < 0 || nx == M_REL || nx == M_CLOSE || nx == M_PUNCT;
                    }
                    if (conv)
                        at[i].cls = M_ORD;
                }
                prev = at[i].cls;
            }
            MBox out;
            float x = 0;
            int pc = -1;
            const float unit = c.size / 18.f;
            for (auto &a : at)
            {
                if (!a.space && pc >= 0 && c.sty < 2)
                    x += (float)Mu(pc, a.cls) * unit;
                AppendOps(out.ops, a.b.ops, x, 0);
                x += a.b.w;
                out.asc = std::max(out.asc, a.b.asc);
                out.desc = std::max(out.desc, a.b.desc);
                if (!a.space)
                    pc = a.cls;
            }
            out.w = x;
            return out;
        }

        static MBox Delim(const std::wstring &d, float H, float yc, const MCtx &c)
        {
            MBox b;
            if (d.empty() || d == L".")
                return b;
            Style st;
            st.size = c.size;
            st.fam = F_MATH;
            const float w = Width(d, st), nat = c.size;
            const float sy = std::max(1.f, H / nat);
            Op o;
            o.kind = OP_TEXT;
            o.x = 0;
            o.y = yc + 0.25f * c.size;
            o.x2 = w;
            o.st = st;
            o.s = d;
            o.sy = sy;
            b.ops.push_back(o);
            b.w = w + (sy > 1.4f ? 0.05f * c.size : 0.f);
            const float hh = std::max(H, nat);
            b.asc = hh / 2 - yc;
            b.desc = yc + hh / 2;
            return b;
        }

        static MBox Wrap(const std::wstring &l, const std::wstring &r, const MBox &inner, const MCtx &c, float minH = 0.f)
        {
            float H = inner.asc + inner.desc, yc = (inner.desc - inner.asc) / 2;
            if (H < c.size)
            {
                H = c.size;
                yc = -0.25f * c.size;
            }
            if (minH > H)
            {
                H = minH;
                yc = -0.25f * c.size;
            }
            MBox L = Delim(l, H, yc, c), R = Delim(r, H, yc, c), out;
            const float g = 0.04f * c.size;
            float x = 0;
            AppendOps(out.ops, L.ops, x, 0);
            x += L.w + (L.w > 0 ? g : 0);
            AppendOps(out.ops, inner.ops, x, 0);
            x += inner.w + (R.w > 0 ? g : 0);
            AppendOps(out.ops, R.ops, x, 0);
            x += R.w;
            out.w = x;
            out.asc = std::max({L.asc, inner.asc, R.asc});
            out.desc = std::max({L.desc, inner.desc, R.desc});
            return out;
        }

        static MBox LayoutGrid(std::vector<std::vector<MBox>> &rows, const std::vector<int> &align, const std::vector<float> &gaps,
                               float rowGap, const MCtx &c)
        {
            const float s = c.size;
            size_t nc = 0;
            for (auto &r : rows)
                nc = std::max(nc, r.size());
            MBox out;
            if (nc == 0 || rows.empty())
                return out;
            std::vector<float> cw(nc, 0.f), ra(rows.size(), 0.72f * s), rd(rows.size(), 0.25f * s);
            for (size_t i = 0; i < rows.size(); ++i)
                for (size_t j = 0; j < rows[i].size(); ++j)
                {
                    cw[j] = std::max(cw[j], rows[i][j].w);
                    ra[i] = std::max(ra[i], rows[i][j].asc);
                    rd[i] = std::max(rd[i], rows[i][j].desc);
                }
            float Ht = rowGap * (float)(rows.size() - 1);
            for (size_t i = 0; i < rows.size(); ++i)
                Ht += ra[i] + rd[i];
            std::vector<float> cx(nc, 0.f);
            for (size_t j = 1; j < nc; ++j)
                cx[j] = cx[j - 1] + cw[j - 1] + (j - 1 < gaps.size() ? gaps[j - 1] : s);
            float yTop = -0.25f * s - Ht / 2;
            for (size_t i = 0; i < rows.size(); ++i)
            {
                const float yb = yTop + ra[i];
                for (size_t j = 0; j < rows[i].size(); ++j)
                {
                    const int al = j < align.size() ? align[j] : 1;
                    const float dx = al == 2 ? cw[j] - rows[i][j].w : al == 1 ? (cw[j] - rows[i][j].w) / 2
                                                                              : 0.f;
                    AppendOps(out.ops, rows[i][j].ops, cx[j] + dx, yb);
                }
                yTop += ra[i] + rd[i] + rowGap;
            }
            out.w = cx[nc - 1] + cw[nc - 1];
            out.asc = Ht / 2 + 0.25f * s;
            out.desc = std::max(0.f, Ht / 2 - 0.25f * s);
            return out;
        }

        static std::wstring DelimFromCmd(const std::wstring &n)
        {
            if (n == L"{" || n == L"lbrace")
                return L"{";
            if (n == L"}" || n == L"rbrace")
                return L"}";
            if (n == L"langle")
                return L"\u27E8";
            if (n == L"rangle")
                return L"\u27E9";
            if (n == L"lfloor")
                return L"\u230A";
            if (n == L"rfloor")
                return L"\u230B";
            if (n == L"lceil")
                return L"\u2308";
            if (n == L"rceil")
                return L"\u2309";
            if (n == L"|" || n == L"Vert" || n == L"lVert" || n == L"rVert")
                return L"\u2016";
            if (n == L"vert" || n == L"lvert" || n == L"rvert")
                return L"|";
            if (n == L"lbrack")
                return L"[";
            if (n == L"rbrack")
                return L"]";
            if (n == L"backslash")
                return L"\\";
            if (n == L"uparrow")
                return L"\u2191";
            if (n == L"downarrow")
                return L"\u2193";
            return L"";
        }

        struct MathParser
        {
            const std::wstring &s;
            size_t p = 0;
            int depth = 0;
            explicit MathParser(const std::wstring &str) : s(str) {}

            bool Eof() const { return p >= s.size(); }
            void SkipWs()
            {
                while (p < s.size() && iswspace(s[p]))
                    ++p;
            }
            std::wstring PeekCmdName() const
            { // s[p] == '\\'
                size_t q = p + 1;
                if (q >= s.size())
                    return L"";
                if (iswalpha(s[q]))
                {
                    size_t e = q;
                    while (e < s.size() && iswalpha(s[e]))
                        ++e;
                    return s.substr(q, e - q);
                }
                return s.substr(q, 1);
            }
            std::wstring ReadCmd()
            {
                std::wstring n = PeekCmdName();
                p += 1 + n.size();
                return n;
            }

            std::wstring ReadRawGroup()
            {
                SkipWs();
                if (p >= s.size())
                    return L"";
                if (s[p] != L'{')
                    return std::wstring(1, s[p++]);
                int d = 0;
                size_t q = p;
                for (; q < s.size(); ++q)
                {
                    if (s[q] == L'\\')
                    {
                        ++q;
                        continue;
                    }
                    if (s[q] == L'{')
                        ++d;
                    else if (s[q] == L'}')
                    {
                        if (--d == 0)
                            break;
                    }
                }
                const size_t endq = std::min(q, s.size());
                std::wstring r = s.substr(p + 1, endq > p + 1 ? endq - p - 1 : 0);
                p = q < s.size() ? q + 1 : s.size();
                return r;
            }
            static std::wstring UnescText(const std::wstring &t)
            {
                std::wstring r;
                for (size_t i = 0; i < t.size(); ++i)
                {
                    if (t[i] == L'\\' && i + 1 < t.size() && wcschr(L"$%&#_{} ", t[i + 1]))
                    {
                        r += t[++i];
                        continue;
                    }
                    if (t[i] == L'~')
                    {
                        r += L' ';
                        continue;
                    }
                    r += t[i];
                }
                return r;
            }

            MBox ParseGroupArg(const MCtx &c)
            {
                SkipWs();
                if (Eof())
                    return MBox();
                if (s[p] == L'{')
                {
                    ++p;
                    MBox b = ParseSeq(c);
                    SkipWs();
                    if (p < s.size() && s[p] == L'}')
                        ++p;
                    return b;
                }
                if (s[p] == L'}' || s[p] == L'&')
                    return MBox();
                MAtom a;
                ParseAtom(c, a);
                return a.b;
            }

            MBox ParseScriptArg(const MCtx &c) { return ParseGroupArg(ScriptCtx(c)); }

            MBox ParseSeq(MCtx c)
            {
                if (++depth > 40)
                {
                    --depth;
                    return MBox();
                }
                std::vector<MAtom> atoms;
                while (true)
                {
                    SkipWs();
                    if (Eof())
                        break;
                    const wchar_t ch = s[p];
                    if (ch == L'}' || ch == L'&')
                        break;
                    if (ch == L'\\')
                    {
                        const std::wstring n = PeekCmdName();
                        if (n == L"\\" || n == L"right" || n == L"end")
                            break;
                        if (n == L"displaystyle")
                        {
                            p += 1 + n.size();
                            c.sty = 0;
                            continue;
                        }
                        if (n == L"textstyle")
                        {
                            p += 1 + n.size();
                            c.sty = 1;
                            continue;
                        }
                    }
                    const size_t before = p;
                    MAtom a;
                    if (ParseAtomWithScripts(c, a))
                        atoms.push_back(std::move(a));
                    if (p == before)
                        ++p;
                }
                --depth;
                return Assemble(atoms, c);
            }

            bool ParseAtomWithScripts(const MCtx &c, MAtom &a)
            {
                SkipWs();
                if (Eof() || !ParseAtom(c, a))
                    return false;
                if (a.space)
                    return true;
                for (;;)
                { // \limits / \nolimits
                    const size_t save = p;
                    SkipWs();
                    if (p < s.size() && s[p] == L'\\')
                    {
                        const std::wstring n = PeekCmdName();
                        if (n == L"limits")
                        {
                            p += 7;
                            a.forceLim = 1;
                            continue;
                        }
                        if (n == L"nolimits")
                        {
                            p += 9;
                            a.forceLim = -1;
                            continue;
                        }
                    }
                    p = save;
                    break;
                }
                MBox sup, sub;
                bool hasSup = false, hasSub = false;
                int primes = 0;
                for (;;)
                {
                    const size_t save = p;
                    SkipWs();
                    if (p >= s.size())
                    {
                        p = save;
                        break;
                    }
                    const wchar_t ch = s[p];
                    if (ch == L'^' && !hasSup)
                    {
                        ++p;
                        sup = ParseScriptArg(c);
                        hasSup = true;
                    }
                    else if (ch == L'_' && !hasSub)
                    {
                        ++p;
                        sub = ParseScriptArg(c);
                        hasSub = true;
                    }
                    else if (ch == L'\'' && !hasSup)
                    {
                        ++p;
                        ++primes;
                    }
                    else
                    {
                        p = save;
                        break;
                    }
                    if (primes && hasSup)
                    {
                        primes = 0;
                    }
                }
                if (primes && !hasSup)
                {
                    const MCtx sc = ScriptCtx(c);
                    sup = GlyphBox(std::wstring(primes > 2 ? 2 : (size_t)primes, L'\u2032'), L'\'', sc);
                    hasSup = true;
                }
                if (hasSup || hasSub)
                    a.b = ApplyScripts(a, sup, sub, hasSup, hasSub, c);
                return true;
            }

            static MBox ApplyScripts(const MAtom &a, const MBox &sup, const MBox &sub, bool hasSup, bool hasSub, const MCtx &c)
            {
                const float s = c.size;
                const bool limitsMode = a.big && ((a.limits && c.sty == 0 && a.forceLim >= 0) || a.forceLim == 1);
                MBox out;
                if (limitsMode)
                {
                    const float W = std::max({a.b.w, hasSup ? sup.w : 0.f, hasSub ? sub.w : 0.f});
                    const float gap = 0.12f * s;
                    AppendOps(out.ops, a.b.ops, (W - a.b.w) / 2, 0);
                    out.asc = a.b.asc;
                    out.desc = a.b.desc;
                    if (hasSup)
                    {
                        const float y = -(a.b.asc + gap + sup.desc);
                        AppendOps(out.ops, sup.ops, (W - sup.w) / 2, y);
                        out.asc = std::max(out.asc, -y + sup.asc);
                    }
                    if (hasSub)
                    {
                        const float y = a.b.desc + gap + sub.asc;
                        AppendOps(out.ops, sub.ops, (W - sub.w) / 2, y);
                        out.desc = std::max(out.desc, y + sub.desc);
                    }
                    out.w = W;
                    return out;
                }
                const float x0 = a.b.w + 0.02f * s;
                float supShift = std::max(0.36f * s, a.b.asc - 0.27f * s);
                float subShift = std::max(0.15f * s, a.b.desc + 0.05f * s);
                if (hasSup && hasSub)
                {
                    const float gap = subShift + supShift - sub.asc - sup.desc, need = 0.18f * s;
                    if (gap < need)
                        subShift += need - gap;
                }
                AppendOps(out.ops, a.b.ops, 0, 0);
                out.asc = a.b.asc;
                out.desc = a.b.desc;
                float w = a.b.w;
                if (hasSup)
                {
                    AppendOps(out.ops, sup.ops, x0, -supShift);
                    out.asc = std::max(out.asc, supShift + sup.asc);
                    w = std::max(w, x0 + sup.w);
                }
                if (hasSub)
                {
                    AppendOps(out.ops, sub.ops, x0, subShift);
                    out.desc = std::max(out.desc, subShift + sub.desc);
                    w = std::max(w, x0 + sub.w);
                }
                out.w = w + ((hasSup || hasSub) ? 0.03f * s : 0.f);
                return out;
            }

            static MAtom CharAtom(wchar_t ch, const MCtx &c)
            {
                MAtom a;
                if (c.var == 6)
                {
                    a.b = GlyphBox(std::wstring(1, ch), ch, c, F_UI, 400, false);
                    return a;
                }
                if ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z'))
                {
                    a.b = GlyphBox(MathAlpha(ch, c.var), ch, c);
                    return a;
                }
                if (ch >= L'0' && ch <= L'9')
                {
                    std::wstring t;
                    if (c.var == 2 || c.var == 7)
                        AppendCp(t, 0x1D7CE + (ch - L'0'));
                    else if (c.var == 3)
                        AppendCp(t, 0x1D7D8 + (ch - L'0'));
                    else
                        t = std::wstring(1, ch);
                    a.b = GlyphBox(t, ch, c);
                    return a;
                }
                std::wstring t(1, ch);
                switch (ch)
                {
                case L'+':
                    a.cls = M_BIN;
                    break;
                case L'-':
                    a.cls = M_BIN;
                    t = L"\u2212";
                    break;
                case L'*':
                    a.cls = M_BIN;
                    t = L"\u2217";
                    break;
                case L'=':
                case L'<':
                case L'>':
                case L':':
                    a.cls = M_REL;
                    break;
                case L',':
                case L';':
                    a.cls = M_PUNCT;
                    break;
                case L'(':
                case L'[':
                    a.cls = M_OPEN;
                    break;
                case L')':
                case L']':
                    a.cls = M_CLOSE;
                    break;
                default:
                    break;
                }
                a.b = GlyphBox(t, ch, c);
                return a;
            }

            bool ParseAtom(const MCtx &c, MAtom &a)
            {
                SkipWs();
                if (Eof())
                    return false;
                const wchar_t ch = s[p];
                if (ch == L'{')
                {
                    ++p;
                    a.b = ParseSeq(c);
                    SkipWs();
                    if (p < s.size() && s[p] == L'}')
                        ++p;
                    return true;
                }
                if (ch == L'^' || ch == L'_')
                    return true; // empty base; the caller consumes the script
                if (ch == L'\\')
                    return ParseCommand(c, a);
                if (ch == L'~')
                {
                    ++p;
                    a.space = true;
                    a.b.w = 0.33f * c.size;
                    return true;
                }
                ++p;
                a = CharAtom(ch, c);
                return true;
            }

            static void SpaceAtom(MAtom &a, float w)
            {
                a.space = true;
                a.b.w = w;
            }

            bool ParseCommand(const MCtx &c, MAtom &a)
            {
                const std::wstring n = ReadCmd();
                const float s = c.size, u = s / 18.f;
                if (n.empty())
                    return true;
                // ---- spacing
                if (n == L",")
                {
                    SpaceAtom(a, 3 * u);
                    return true;
                }
                if (n == L":" || n == L">")
                {
                    SpaceAtom(a, 4 * u);
                    return true;
                }
                if (n == L";")
                {
                    SpaceAtom(a, 5 * u);
                    return true;
                }
                if (n == L"!")
                {
                    SpaceAtom(a, -3 * u);
                    return true;
                }
                if (n == L" ")
                {
                    SpaceAtom(a, 6 * u);
                    return true;
                }
                if (n == L"quad")
                {
                    SpaceAtom(a, 18 * u);
                    return true;
                }
                if (n == L"qquad")
                {
                    SpaceAtom(a, 36 * u);
                    return true;
                }
                if (n == L"enspace")
                {
                    SpaceAtom(a, 9 * u);
                    return true;
                }
                if (n == L"hspace" || n == L"hspace*")
                {
                    ReadRawGroup();
                    SpaceAtom(a, 18 * u);
                    return true;
                }
                // ---- ignored commands
                if (n == L"nonumber" || n == L"notag" || n == L"hline" || n == L"relax" || n == L"cr" || n == L"hfill" ||
                    n == L"toprule" || n == L"midrule" || n == L"bottomrule" || n == L"limits" || n == L"nolimits" ||
                    n == L"displaystyle" || n == L"textstyle" || n == L"scriptstyle" || n == L"scriptscriptstyle" ||
                    n == L"bigl" || n == L"bigr")
                {
                    SpaceAtom(a, 0);
                    return true;
                }
                if (n == L"tag" || n == L"label" || n == L"color" || n == L"phantom" || n == L"vspace" || n == L"vphantom" ||
                    n == L"hphantom" || n == L"pagecolor" || n == L"textcolor")
                {
                    ReadRawGroup();
                    if (n == L"textcolor")
                        a.b = ParseGroupArg(c); // \textcolor{red}{x}: keep the content
                    else
                        SpaceAtom(a, 0);
                    return true;
                }
                // ---- font / style switches
                static const struct
                {
                    const wchar_t *n;
                    int v;
                } fonts[] = {
                    {L"mathrm", 1}, {L"mathsf", 1}, {L"mathtt", 1}, {L"mathbf", 2}, {L"boldsymbol", 7}, {L"bm", 7}, {L"mathbb", 3}, {L"Bbb", 3}, {L"mathcal", 4}, {L"mathscr", 4}, {L"mathfrak", 5}, {L"mathit", 0}, {L"mathnormal", 0}, {L"pmb", 7}};
                for (auto &f : fonts)
                    if (n == f.n)
                    {
                        MCtx cc = c;
                        cc.var = f.v;
                        a.b = ParseGroupArg(cc);
                        return true;
                    }
                if (n == L"text" || n == L"textrm" || n == L"mbox" || n == L"textnormal" || n == L"textit" || n == L"textbf" ||
                    n == L"textsf" || n == L"texttt" || n == L"hbox")
                {
                    const std::wstring raw = UnescText(ReadRawGroup());
                    a.b = GlyphBox(raw, L'x', c, n == L"texttt" ? F_MONO : F_UI, n == L"textbf" ? 700 : 400, n == L"textit");
                    if (raw.empty())
                        a.b = MBox();
                    return true;
                }
                if (n == L"operatorname" || n == L"mathop" || n == L"operatorname*")
                {
                    const std::wstring raw = UnescText(ReadRawGroup());
                    a.b = GlyphBox(raw, L'l', c);
                    a.cls = M_OP;
                    return true;
                }
                if (n == L"mathbin" || n == L"mathrel" || n == L"mathord")
                {
                    a.b = ParseGroupArg(c);
                    a.cls = n == L"mathbin" ? M_BIN : n == L"mathrel" ? M_REL
                                                                      : M_ORD;
                    return true;
                }
                // ---- fractions
                if (n == L"frac" || n == L"dfrac" || n == L"tfrac" || n == L"cfrac" || n == L"binom" || n == L"dbinom" || n == L"tbinom")
                {
                    MCtx base = c;
                    if (n == L"dfrac" || n == L"cfrac" || n == L"dbinom")
                    {
                        base.sty = 0;
                    }
                    else if (n == L"tfrac" || n == L"tbinom")
                    {
                        base.sty = 1;
                    }
                    const bool binom = n[0] == L'b' || n == L"dbinom" || n == L"tbinom";
                    MCtx cs = base;
                    if (base.sty == 0)
                        cs.sty = 1;
                    else if (base.sty == 1)
                    {
                        cs.sty = 2;
                        cs.size = base.size * 0.7f;
                    }
                    else
                    {
                        cs.sty = 3;
                        cs.size = base.sty == 2 ? base.size * 0.714f : base.size;
                    }
                    MBox num = ParseGroupArg(cs), den = ParseGroupArg(cs);
                    const float th = 0.06f * s, axis = 0.25f * s, gap = base.sty == 0 ? 0.2f * s : 0.12f * s;
                    const float W = std::max(num.w, den.w) + 0.2f * s;
                    const float yn = -axis - th / 2 - gap - num.desc, yd = -axis + th / 2 + gap + den.asc;
                    MBox f;
                    AppendOps(f.ops, num.ops, (W - num.w) / 2, yn);
                    AppendOps(f.ops, den.ops, (W - den.w) / 2, yd);
                    if (!binom)
                        f.ops.push_back(MkLine(0.04f * s, -axis, W - 0.04f * s, -axis, th));
                    f.w = W;
                    f.asc = -yn + num.asc;
                    f.desc = yd + den.desc;
                    a.b = binom ? Wrap(L"(", L")", f, c) : f;
                    a.cls = M_INNER;
                    return true;
                }
                if (n == L"sqrt")
                {
                    MBox idx;
                    bool hasIdx = false;
                    SkipWs();
                    if (p < this->s.size() && this->s[p] == L'[')
                    {
                        const size_t e = this->s.find(L']', p);
                        if (e != std::wstring::npos)
                        {
                            const std::wstring raw = this->s.substr(p + 1, e - p - 1);
                            MathParser sub(raw);
                            MCtx ic = c;
                            ic.size = c.size * 0.5f;
                            ic.sty = 3;
                            idx = sub.ParseSeq(ic);
                            hasIdx = true;
                            p = e + 1;
                        }
                    }
                    MBox body = ParseGroupArg(c);
                    const float th = std::max(0.9f, 0.06f * s);
                    const float ytop = -(body.asc + 0.16f * s), ybot = body.desc + 0.04f * s, ymid = (ytop + ybot) / 2;
                    const float off = hasIdx ? std::max(0.f, idx.w - 0.3f * s) : 0.f;
                    const float x1 = off, x2 = off + 0.14f * s, x3 = off + 0.30f * s, x4 = off + 0.54f * s;
                    MBox r;
                    if (hasIdx)
                        AppendOps(r.ops, idx.ops, std::max(0.f, off - idx.w) + 0.06f * s, ymid - 0.1f * s);
                    r.ops.push_back(MkLine(x1, ymid + 0.05f * s, x2, ymid, th));
                    r.ops.push_back(MkLine(x2, ymid, x3, ybot, th));
                    r.ops.push_back(MkLine(x3, ybot, x4, ytop, th));
                    r.ops.push_back(MkLine(x4, ytop, x4 + body.w + 0.1f * s, ytop, th));
                    AppendOps(r.ops, body.ops, x4 + 0.05f * s, 0);
                    r.w = x4 + body.w + 0.1f * s;
                    r.asc = -ytop + th;
                    r.desc = ybot;
                    a.b = r;
                    return true;
                }
                // ---- \left ... \right and \big family
                if (n == L"left")
                {
                    const std::wstring l = ReadDelim();
                    MBox inner = ParseSeq(c);
                    SkipWs();
                    std::wstring r;
                    if (p < this->s.size() && this->s[p] == L'\\' && PeekCmdName() == L"right")
                    {
                        ReadCmd();
                        r = ReadDelim();
                    }
                    a.b = Wrap(l, r, inner, c);
                    a.cls = M_INNER;
                    return true;
                }
                if (n == L"right")
                {
                    ReadDelim();
                    SpaceAtom(a, 0);
                    return true;
                }
                if (n == L"middle")
                {
                    const std::wstring d = ReadDelim();
                    a.b = GlyphBox(d, L'|', c);
                    return true;
                }
                if (n == L"big" || n == L"Big" || n == L"bigg" || n == L"Bigg")
                {
                    const float k = n == L"big" ? 1.2f : n == L"Big" ? 1.8f
                                                     : n == L"bigg"  ? 2.4f
                                                                     : 3.0f;
                    const std::wstring d = ReadDelim();
                    a.b = Delim(d, k * s, -0.25f * s, c);
                    a.cls = M_ORD;
                    return true;
                }
                // ---- environments
                if (n == L"begin")
                    return ParseEnv(c, a);
                // ---- accents / decorations
                if (n == L"hat" || n == L"widehat" || n == L"tilde" || n == L"widetilde" || n == L"dot" || n == L"ddot" ||
                    n == L"check" || n == L"acute" || n == L"grave" || n == L"breve" || n == L"mathring")
                {
                    const wchar_t *g = n == L"hat" || n == L"widehat" ? L"\u02C6" : n == L"tilde" || n == L"widetilde" ? L"\u02DC"
                                                                                : n == L"dot"                          ? L"\u02D9"
                                                                                : n == L"ddot"                         ? L"\u00A8"
                                                                                : n == L"check"                        ? L"\u02C7"
                                                                                : n == L"acute"                        ? L"\u00B4"
                                                                                : n == L"grave"                        ? L"`"
                                                                                : n == L"breve"                        ? L"\u02D8"
                                                                                                                       : L"\u02DA";
                    MBox base = ParseGroupArg(c);
                    MBox gl = GlyphBox(g, L'^', c);
                    const bool wide = n == L"widehat" || n == L"widetilde";
                    float sx = wide ? std::min(3.f, std::max(1.f, base.w / std::max(0.01f, gl.w))) : 1.f;
                    const float yb = -(base.asc - 0.47f * s);
                    Op o = gl.ops[0];
                    o.sx = sx;
                    const float gw = gl.w * sx;
                    o.x = (base.w - gw) / 2 + (sx > 1.f ? (gw - gl.w) / 2 : 0.f) + 0.04f * s;
                    o.y = yb;
                    MBox r = base;
                    r.ops.push_back(o);
                    r.asc = base.asc + 0.28f * s;
                    a.b = r;
                    return true;
                }
                if (n == L"bar" || n == L"overline" || n == L"underline" || n == L"vec" || n == L"overrightarrow" ||
                    n == L"overleftarrow" || n == L"overbrace" || n == L"underbrace")
                {
                    MBox base = ParseGroupArg(c);
                    MBox r = base;
                    const float th = std::max(0.9f, 0.05f * s);
                    if (n == L"bar" || n == L"overline")
                    {
                        r.ops.push_back(MkLine(0.02f * s, -(base.asc + 0.1f * s), base.w - 0.02f * s, -(base.asc + 0.1f * s), th));
                        r.asc = base.asc + 0.16f * s;
                    }
                    else if (n == L"underline")
                    {
                        r.ops.push_back(MkLine(0, base.desc + 0.12f * s, base.w, base.desc + 0.12f * s, th));
                        r.desc = base.desc + 0.18f * s;
                    }
                    else if (n == L"vec" || n == L"overrightarrow" || n == L"overleftarrow")
                    {
                        const float y = -(base.asc + 0.13f * s), hd = 0.1f * s;
                        const bool left = n == L"overleftarrow";
                        r.ops.push_back(MkLine(0.04f * s, y, base.w, y, th));
                        const float tipx = left ? 0.04f * s : base.w, dir = left ? 1.f : -1.f;
                        r.ops.push_back(MkLine(tipx, y, tipx + dir * hd * 1.2f, y - hd, th));
                        r.ops.push_back(MkLine(tipx, y, tipx + dir * hd * 1.2f, y + hd, th));
                        r.asc = base.asc + 0.24f * s;
                    }
                    else
                    {
                        const bool over = n == L"overbrace";
                        const float yb = over ? -(base.asc + 0.14f * s) : base.desc + 0.14f * s;
                        const float ye = over ? yb + 0.12f * s : yb - 0.12f * s, yp = over ? yb - 0.12f * s : yb + 0.12f * s;
                        const float W = base.w, k = 0.1f * s;
                        r.ops.push_back(MkLine(0, ye, k, yb, th));
                        r.ops.push_back(MkLine(k, yb, W / 2 - k, yb, th));
                        r.ops.push_back(MkLine(W / 2 - k, yb, W / 2, yp, th));
                        r.ops.push_back(MkLine(W / 2, yp, W / 2 + k, yb, th));
                        r.ops.push_back(MkLine(W / 2 + k, yb, W - k, yb, th));
                        r.ops.push_back(MkLine(W - k, yb, W, ye, th));
                        if (over)
                            r.asc = base.asc + 0.3f * s;
                        else
                            r.desc = base.desc + 0.3f * s;
                        a.big = true;
                        a.limits = true;
                        a.forceLim = 1;
                    }
                    a.b = r;
                    return true;
                }
                if (n == L"overset" || n == L"stackrel" || n == L"underset")
                {
                    MBox top = ParseGroupArg(ScriptCtx(c)), base = ParseGroupArg(c);
                    const bool under = n == L"underset";
                    const float W = std::max(top.w, base.w), gap = 0.1f * s;
                    MBox r;
                    AppendOps(r.ops, base.ops, (W - base.w) / 2, 0);
                    r.asc = base.asc;
                    r.desc = base.desc;
                    if (under)
                    {
                        const float y = base.desc + gap + top.asc;
                        AppendOps(r.ops, top.ops, (W - top.w) / 2, y);
                        r.desc = y + top.desc;
                    }
                    else
                    {
                        const float y = -(base.asc + gap + top.desc);
                        AppendOps(r.ops, top.ops, (W - top.w) / 2, y);
                        r.asc = -y + top.asc;
                    }
                    r.w = W;
                    a.b = r;
                    a.cls = M_REL;
                    return true;
                }
                if (n == L"xrightarrow" || n == L"xleftarrow")
                {
                    MBox below;
                    bool hasBelow = false;
                    SkipWs();
                    if (p < this->s.size() && this->s[p] == L'[')
                    {
                        const size_t e = this->s.find(L']', p);
                        if (e != std::wstring::npos)
                        {
                            const std::wstring raw = this->s.substr(p + 1, e - p - 1);
                            MathParser sub(raw);
                            below = sub.ParseSeq(ScriptCtx(c));
                            hasBelow = true;
                            p = e + 1;
                        }
                    }
                    MBox above = ParseGroupArg(ScriptCtx(c));
                    const float W = std::max(above.w, hasBelow ? below.w : 0.f) + 0.8f * s, axis = 0.25f * s, th = std::max(0.9f, 0.05f * s);
                    MBox r;
                    const bool left = n == L"xleftarrow";
                    r.ops.push_back(MkLine(0.05f * s, -axis, W - 0.05f * s, -axis, th));
                    const float tx = left ? 0.05f * s : W - 0.05f * s, dir = left ? 1.f : -1.f, hd = 0.09f * s;
                    r.ops.push_back(MkLine(tx, -axis, tx + dir * hd * 1.4f, -axis - hd, th));
                    r.ops.push_back(MkLine(tx, -axis, tx + dir * hd * 1.4f, -axis + hd, th));
                    const float ya = -axis - 0.1f * s - above.desc;
                    AppendOps(r.ops, above.ops, (W - above.w) / 2, ya);
                    r.asc = -ya + above.asc;
                    r.desc = 0.25f * s;
                    if (hasBelow)
                    {
                        const float yb = -axis + 0.1f * s + below.asc;
                        AppendOps(r.ops, below.ops, (W - below.w) / 2, yb);
                        r.desc = std::max(r.desc, yb + below.desc);
                    }
                    r.w = W;
                    a.b = r;
                    a.cls = M_REL;
                    return true;
                }
                if (n == L"boxed")
                {
                    MBox body = ParseGroupArg(c);
                    const float pad = 0.2f * s, th = std::max(0.9f, 0.05f * s);
                    MBox r;
                    AppendOps(r.ops, body.ops, pad, 0);
                    const float t = -(body.asc + pad), b = body.desc + pad, W = body.w + 2 * pad;
                    r.ops.push_back(MkLine(0, t, W, t, th));
                    r.ops.push_back(MkLine(0, b, W, b, th));
                    r.ops.push_back(MkLine(0, t, 0, b, th));
                    r.ops.push_back(MkLine(W, t, W, b, th));
                    r.w = W;
                    r.asc = -t;
                    r.desc = b;
                    a.b = r;
                    return true;
                }
                if (n == L"not")
                { // \not= etc.: slash through the next symbol
                    MAtom nx;
                    if (ParseAtom(c, nx))
                    {
                        a = nx;
                        a.b.ops.push_back(MkLine(0.1f * a.b.w, a.b.desc, 0.9f * a.b.w, -a.b.asc, std::max(0.9f, 0.05f * s)));
                    }
                    return true;
                }
                // ---- big operators
                for (auto &bo : kBigs)
                    if (n == bo.name)
                    {
                        const float mul = c.sty == 0 ? 1.45f * bo.tall : bo.tall;
                        MBox g = GlyphBox(bo.text, L'+', c, F_MATH, 400, false, mul);
                        Op &o = g.ops[0];
                        o.y = 0.25f * s * (mul - 1.f);
                        const float half = 0.52f * s * mul;
                        g.asc = 0.25f * s + half;
                        g.desc = std::max(0.f, half - 0.25f * s);
                        a.b = g;
                        a.cls = M_OP;
                        a.big = true;
                        a.limits = bo.limits;
                        return true;
                    }
                // ---- named functions
                for (const wchar_t *f : kFuncs)
                    if (n == f)
                    {
                        std::wstring t = n;
                        if (n == L"bmod")
                            t = L"mod";
                        a.b = GlyphBox(t, L'l', c);
                        a.cls = M_OP;
                        for (const wchar_t *lf : kLimitFuncs)
                            if (n == lf)
                            {
                                a.big = true;
                                a.limits = true;
                            }
                        return true;
                    }
                // ---- symbols
                for (auto &sd : kSyms)
                    if (n == sd.name)
                    {
                        std::wstring t = sd.text;
                        if (t.size() == 1 && t[0] >= 0x3B1 && t[0] <= 0x3C9 && c.var == 0)
                        { // italic lowercase Greek
                            std::wstring it;
                            AppendCp(it, 0x1D6FC + (t[0] - 0x3B1));
                            t = it;
                        }
                        else if (t.size() == 1 && c.var == 0 && (t[0] == 0x3F5 || t[0] == 0x3D1 || t[0] == 0x3F0 || t[0] == 0x3D5 || t[0] == 0x3F1 || t[0] == 0x3D6))
                        {
                            const uint32_t cp = t[0] == 0x3F5 ? 0x1D716 : t[0] == 0x3D1 ? 0x1D717
                                                                      : t[0] == 0x3F0   ? 0x1D718
                                                                      : t[0] == 0x3D5   ? 0x1D719
                                                                      : t[0] == 0x3F1   ? 0x1D71A
                                                                                        : 0x1D71B;
                            std::wstring it;
                            AppendCp(it, cp);
                            t = it;
                        }
                        if (t.empty())
                        {
                            SpaceAtom(a, 0);
                            return true;
                        }
                        a.b = GlyphBox(t, sd.text[0], c);
                        a.cls = sd.cls;
                        return true;
                    }
                // ---- unknown: show the command so nothing silently disappears
                a.b = GlyphBox(L"\\" + n, L'x', c, F_UI, 400, false, 0.9f, C_SUB);
                return true;
            }

            std::wstring ReadDelim()
            {
                SkipWs();
                if (Eof())
                    return L"";
                const wchar_t ch = s[p];
                if (ch == L'\\')
                    return DelimFromCmd(ReadCmd());
                ++p;
                return std::wstring(1, ch);
            }

            bool ParseEnv(const MCtx &c, MAtom &a)
            {
                const std::wstring env = ReadRawGroup();
                std::wstring spec;
                if (env == L"array" || env == L"tabular" || env == L"alignat" || env == L"alignat*")
                    spec = ReadRawGroup();
                const bool aligned = env == L"aligned" || env == L"align" || env == L"align*" || env == L"split" || env == L"alignat" ||
                                     env == L"alignat*" || env == L"flalign" || env == L"flalign*" || env == L"eqnarray" || env == L"eqnarray*";
                const bool single = env == L"gather" || env == L"gather*" || env == L"gathered" || env == L"equation" || env == L"equation*" ||
                                    env == L"multline" || env == L"multline*" || env == L"displaymath" || env == L"math";
                MCtx cc = c;
                cc.sty = (aligned || single) ? 0 : 1;
                std::vector<std::vector<MBox>> rows(1);
                for (;;)
                {
                    rows.back().push_back(ParseSeq(cc));
                    SkipWs();
                    if (Eof())
                        break;
                    if (s[p] == L'&')
                    {
                        ++p;
                        continue;
                    }
                    if (s[p] == L'\\')
                    {
                        const std::wstring nn = PeekCmdName();
                        if (nn == L"\\")
                        {
                            p += 2;
                            SkipWs();
                            if (p < this->s.size() && this->s[p] == L'[')
                            {
                                const size_t e = this->s.find(L']', p);
                                p = e == std::wstring::npos ? s.size() : e + 1;
                            }
                            rows.emplace_back();
                            continue;
                        }
                        if (nn == L"end")
                        {
                            p += 4;
                            ReadRawGroup();
                            break;
                        }
                        ReadCmd(); // stray \right etc.
                        continue;
                    }
                    ++p; // stray '}'
                }
                if (rows.size() > 1 && rows.back().size() == 1 && rows.back()[0].ops.empty() && rows.back()[0].w <= 0.f)
                    rows.pop_back();
                const float s2 = c.size;
                std::vector<int> align;
                std::vector<float> gaps;
                std::wstring dl, dr;
                float rowGap = 0.35f * s2;
                size_t nc = 0;
                for (auto &r : rows)
                    nc = std::max(nc, r.size());
                if (aligned)
                {
                    for (size_t j = 0; j < nc; ++j)
                    {
                        align.push_back(j % 2 == 0 ? 2 : 0);
                        gaps.push_back(j % 2 == 0 ? 0.f : 2.f * s2);
                    }
                    rowGap = 0.45f * s2;
                }
                else if (single)
                {
                    align.assign(nc, 1);
                    rowGap = 0.45f * s2;
                }
                else if (env == L"cases" || env == L"dcases" || env == L"rcases")
                {
                    align.assign(nc, 0);
                    gaps.assign(nc, 1.f * s2);
                    if (env == L"rcases")
                        dr = L"}";
                    else
                        dl = L"{";
                }
                else if (env == L"array" || env == L"tabular")
                {
                    for (wchar_t ch : spec)
                        if (ch == L'l')
                            align.push_back(0);
                        else if (ch == L'c')
                            align.push_back(1);
                        else if (ch == L'r')
                            align.push_back(2);
                    gaps.assign(nc, 0.9f * s2);
                }
                else
                { // matrix family
                    align.assign(nc, 1);
                    gaps.assign(nc, 0.9f * s2);
                    if (env == L"pmatrix")
                    {
                        dl = L"(";
                        dr = L")";
                    }
                    else if (env == L"bmatrix")
                    {
                        dl = L"[";
                        dr = L"]";
                    }
                    else if (env == L"Bmatrix")
                    {
                        dl = L"{";
                        dr = L"}";
                    }
                    else if (env == L"vmatrix")
                    {
                        dl = L"|";
                        dr = L"|";
                    }
                    else if (env == L"Vmatrix")
                    {
                        dl = L"\u2016";
                        dr = L"\u2016";
                    }
                }
                MBox g = LayoutGrid(rows, align, gaps, rowGap, c);
                a.b = (dl.empty() && dr.empty()) ? g : Wrap(dl, dr, g, c);
                a.cls = (dl.empty() && dr.empty()) ? M_ORD : M_INNER;
                return true;
            }
        };

        static MBox MathBox(const std::wstring &latex, bool display, float size)
        {
            MathParser mp(latex);
            MCtx c;
            c.size = size;
            c.sty = display ? 0 : 1;
            c.var = 0;
            std::vector<MBox> rows;
            for (int guard = 0; guard < 400; ++guard)
            {
                rows.push_back(mp.ParseSeq(c));
                mp.SkipWs();
                if (mp.Eof())
                    break;
                const wchar_t ch = mp.s[mp.p];
                if (ch == L'\\')
                {
                    if (mp.PeekCmdName() == L"\\")
                    {
                        mp.p += 2;
                        mp.SkipWs();
                        if (mp.p < mp.s.size() && mp.s[mp.p] == L'[')
                        {
                            size_t e = mp.s.find(L']', mp.p);
                            mp.p = e == std::wstring::npos ? mp.s.size() : e + 1;
                        }
                        continue;
                    }
                    mp.ReadCmd(); // stray \right / \end
                }
                else
                    ++mp.p; // stray '}' or '&'
            }
            if (rows.size() == 1)
                return rows[0];
            MBox out;
            float W = 0;
            for (auto &r : rows)
                W = std::max(W, r.w);
            float y = 0;
            for (size_t i = 0; i < rows.size(); ++i)
            {
                const float asc = std::max(rows[i].asc, 0.72f * size), desc = std::max(rows[i].desc, 0.25f * size);
                if (i == 0)
                {
                    y = 0;
                    out.asc = asc;
                }
                else
                    y += rows[i - 1].desc + 0.4f * size + asc;
                AppendOps(out.ops, rows[i].ops, (W - rows[i].w) / 2, y);
                out.desc = y + desc;
            }
            out.w = W;
            return out;
        }

        // ============================================================================
        //  Markdown
        // ============================================================================
        struct Span
        {
            int kind = 0;
            std::wstring s;
            bool bold = false, italic = false, strike = false, link = false, disp = false;
            std::wstring url; // target of a link span
        }; // 0 text 1 code 2 math 3 break

        static std::wstring LTrim(const std::wstring &s)
        {
            size_t i = 0;
            while (i < s.size() && (s[i] == L' ' || s[i] == L'\t'))
                ++i;
            return s.substr(i);
        }
        static std::wstring Trim(const std::wstring &s)
        {
            std::wstring r = LTrim(s);
            while (!r.empty() && (r.back() == L' ' || r.back() == L'\t' || r.back() == L'\r'))
                r.pop_back();
            return r;
        }
        static bool StartsAt(const std::wstring &t, size_t p, const wchar_t *lit) { return t.compare(p, wcslen(lit), lit) == 0; }

        static std::vector<Span> ParseInline(const std::wstring &t)
        {
            std::vector<Span> out;
            std::wstring buf;
            bool b = false, it = false, st = false;
            const size_t n = t.size();
            auto flush = [&]()
            {
                if (buf.empty())
                    return;
                Span sp;
                sp.s = buf;
                sp.bold = b;
                sp.italic = it;
                sp.strike = st;
                out.push_back(sp);
                buf.clear();
            };
            auto push = [&](int kind, const std::wstring &s, bool disp = false)
            {
                flush();
                Span sp;
                sp.kind = kind;
                sp.s = s;
                sp.disp = disp;
                out.push_back(sp);
            };
            for (size_t p = 0; p < n;)
            {
                const wchar_t c = t[p];
                if (c == L'\\' && p + 1 < n)
                {
                    const wchar_t d = t[p + 1];
                    if (d == L'(' || d == L'[')
                    {
                        const size_t e = t.find(d == L'(' ? L"\\)" : L"\\]", p + 2);
                        if (e != std::wstring::npos)
                        {
                            push(2, t.substr(p + 2, e - p - 2), d == L'[');
                            p = e + 2;
                            continue;
                        }
                    }
                    if (wcschr(L"\\`*_{}[]()#+-.!|$~<>&%", d))
                    {
                        buf += d;
                        p += 2;
                        continue;
                    }
                    buf += c;
                    ++p;
                    continue;
                }
                if (c == L'`')
                {
                    size_t k = 0;
                    while (p + k < n && t[p + k] == L'`')
                        ++k;
                    const std::wstring fence(k, L'`');
                    const size_t e = t.find(fence, p + k);
                    if (e != std::wstring::npos)
                    {
                        push(1, Trim(t.substr(p + k, e - p - k)));
                        p = e + k;
                        continue;
                    }
                    buf.append(k, L'`');
                    p += k;
                    continue;
                }
                if (c == L'$')
                {
                    if (p + 1 < n && t[p + 1] == L'$')
                    {
                        const size_t e = t.find(L"$$", p + 2);
                        if (e != std::wstring::npos)
                        {
                            push(2, t.substr(p + 2, e - p - 2), true);
                            p = e + 2;
                            continue;
                        }
                    }
                    else if (p + 1 < n && !iswspace(t[p + 1]))
                    {
                        size_t e = p + 1;
                        bool found = false;
                        while (e < n)
                        {
                            e = t.find(L'$', e);
                            if (e == std::wstring::npos)
                                break;
                            if (t[e - 1] == L'\\')
                            {
                                ++e;
                                continue;
                            }
                            found = true;
                            break;
                        }
                        if (found && e > p + 1 && !iswspace(t[e - 1]) && (e + 1 >= n || !iswdigit(t[e + 1])) &&
                            t.find(L'\n', p) > e)
                        {
                            push(2, t.substr(p + 1, e - p - 1));
                            p = e + 1;
                            continue;
                        }
                    }
                    buf += c;
                    ++p;
                    continue;
                }
                if (c == L'*' || c == L'_')
                {
                    size_t k = 0;
                    while (p + k < n && t[p + k] == c)
                        ++k;
                    const wchar_t prev = p ? t[p - 1] : L' ', next = p + k < n ? t[p + k] : L' ';
                    const bool intra = c == L'_' && iswalnum(prev) && iswalnum(next);
                    const bool lonely = c == L'*' && iswspace(next) && !it && !b;
                    if (intra || lonely || k > 3)
                    {
                        buf.append(k, c);
                        p += k;
                        continue;
                    }
                    flush();
                    if (k == 3)
                    {
                        b = !b;
                        it = !it;
                    }
                    else if (k == 2)
                        b = !b;
                    else
                        it = !it;
                    p += k;
                    continue;
                }
                if (c == L'~' && p + 1 < n && t[p + 1] == L'~')
                {
                    flush();
                    st = !st;
                    p += 2;
                    continue;
                }
                if (c == L'[')
                {
                    const size_t e = t.find(L']', p + 1);
                    if (e != std::wstring::npos && e + 1 < n && t[e + 1] == L'(')
                    {
                        const size_t f = t.find(L')', e + 2);
                        if (f != std::wstring::npos)
                        {
                            flush();
                            Span sp;
                            sp.s = t.substr(p + 1, e - p - 1);
                            sp.bold = b;
                            sp.italic = it;
                            sp.link = true;
                            sp.url = Trim(t.substr(e + 2, f - e - 2));
                            out.push_back(sp);
                            p = f + 1;
                            continue;
                        }
                    }
                }
                if (c == L'h' && (StartsAt(t, p, L"http://") || StartsAt(t, p, L"https://")) && (p == 0 || !iswalnum(t[p - 1])))
                {
                    size_t e = p;
                    while (e < n && !iswspace(t[e]) && t[e] != L'<' && t[e] != L'>' && t[e] != L'"')
                        ++e;
                    while (e > p + 8 && wcschr(L".,;:!?)]}'", t[e - 1]))
                        --e; // trailing punctuation belongs to the sentence
                    flush();
                    Span sp;
                    sp.s = t.substr(p, e - p);
                    sp.url = sp.s;
                    sp.bold = b;
                    sp.italic = it;
                    sp.link = true;
                    out.push_back(sp);
                    p = e;
                    continue;
                }
                if (c == L'<' && (StartsAt(t, p, L"<br>") || StartsAt(t, p, L"<br/>") || StartsAt(t, p, L"<br />")))
                {
                    push(3, L"");
                    p = t.find(L'>', p) + 1;
                    continue;
                }
                if (c == L'\n')
                {
                    push(3, L"");
                    ++p;
                    continue;
                }
                buf += c;
                ++p;
            }
            flush();
            return out;
        }

        enum
        {
            B_PARA,
            B_HEAD,
            B_CODE,
            B_MATH,
            B_LIST,
            B_HR,
            B_TABLE
        };
        struct Block
        {
            int type = B_PARA, level = 0, quote = 0;
            std::wstring marker, text, lang;
            std::vector<Span> spans;
            std::vector<std::vector<std::vector<Span>>> rows;
            std::vector<int> aligns;
        };

        static std::vector<std::wstring> SplitLines(const std::wstring &s)
        {
            std::vector<std::wstring> r;
            std::wstring cur;
            for (wchar_t ch : s)
            {
                if (ch == L'\r')
                    continue;
                if (ch == L'\n')
                {
                    r.push_back(cur);
                    cur.clear();
                }
                else
                    cur += ch;
            }
            r.push_back(cur);
            return r;
        }

        static void StripQuote(const std::wstring &line, int &q, std::wstring &rest)
        {
            size_t p = 0;
            q = 0;
            for (;;)
            {
                size_t s = p;
                while (s < line.size() && line[s] == L' ' && s - p < 3)
                    ++s;
                if (s < line.size() && line[s] == L'>')
                {
                    ++q;
                    p = s + 1;
                    if (p < line.size() && line[p] == L' ')
                        ++p;
                }
                else
                    break;
            }
            rest = line.substr(p);
        }

        static bool IsHr(const std::wstring &t)
        {
            int cnt = 0;
            wchar_t m = 0;
            for (wchar_t ch : t)
            {
                if (ch == L' ')
                    continue;
                if (ch != L'-' && ch != L'*' && ch != L'_')
                    return false;
                if (m && ch != m)
                    return false;
                m = ch;
                ++cnt;
            }
            return cnt >= 3;
        }

        static bool ListMarker(const std::wstring &rest, int &level, std::wstring &marker, std::wstring &text)
        {
            int indent = 0;
            size_t i = 0;
            while (i < rest.size() && (rest[i] == L' ' || rest[i] == L'\t'))
            {
                indent += rest[i] == L'\t' ? 4 : 1;
                ++i;
            }
            const std::wstring t = rest.substr(i);
            if (t.size() >= 2 && (t[0] == L'-' || t[0] == L'*' || t[0] == L'+') && t[1] == L' ')
            {
                level = std::min(4, indent / 2);
                marker = L"*";
                text = t.substr(2);
                return true;
            }
            size_t d = 0;
            while (d < t.size() && iswdigit(t[d]))
                ++d;
            if (d > 0 && d <= 9 && d + 1 < t.size() && (t[d] == L'.' || t[d] == L')') && t[d + 1] == L' ')
            {
                level = std::min(4, indent / 2);
                marker = t.substr(0, d) + L".";
                text = t.substr(d + 2);
                return true;
            }
            return false;
        }

        static std::vector<std::wstring> SplitRow(const std::wstring &line)
        {
            std::wstring t = Trim(line);
            if (!t.empty() && t[0] == L'|')
                t.erase(0, 1);
            if (!t.empty() && t.back() == L'|' && (t.size() < 2 || t[t.size() - 2] != L'\\'))
                t.pop_back();
            std::vector<std::wstring> cells;
            std::wstring cur;
            bool inMath = false;
            for (size_t i = 0; i < t.size(); ++i)
            {
                const wchar_t ch = t[i];
                if (ch == L'\\' && i + 1 < t.size() && t[i + 1] == L'|')
                {
                    cur += L'|';
                    ++i;
                    continue;
                }
                if (ch == L'$')
                    inMath = !inMath;
                if (ch == L'|' && !inMath)
                {
                    cells.push_back(Trim(cur));
                    cur.clear();
                    continue;
                }
                cur += ch;
            }
            cells.push_back(Trim(cur));
            return cells;
        }

        static bool IsTableSep(const std::wstring &line)
        {
            const std::wstring t = Trim(line);
            if (t.find(L'-') == std::wstring::npos)
                return false;
            for (wchar_t ch : t)
                if (ch != L'|' && ch != L'-' && ch != L':' && ch != L' ')
                    return false;
            return t.find(L'|') != std::wstring::npos || t.find(L"---") != std::wstring::npos;
        }

        static bool IsMathEnvStart(const std::wstring &t, std::wstring &name)
        {
            if (!StartsAt(t, 0, L"\\begin{"))
                return false;
            const size_t e = t.find(L'}', 7);
            if (e == std::wstring::npos)
                return false;
            name = t.substr(7, e - 7);
            static const wchar_t *envs[] = {L"aligned", L"align", L"align*", L"equation", L"equation*", L"gather", L"gather*", L"split",
                                            L"multline", L"multline*", L"eqnarray", L"eqnarray*", L"cases", L"pmatrix", L"bmatrix", L"vmatrix", L"matrix",
                                            L"Bmatrix", L"Vmatrix", L"array", L"flalign", L"flalign*", L"alignat", L"alignat*"};
            for (const wchar_t *e2 : envs)
                if (name == e2)
                    return true;
            return false;
        }

        static std::vector<Block> ParseBlocks(const std::wstring &src)
        {
            std::vector<Block> out;
            const std::vector<std::wstring> lines = SplitLines(src);
            const size_t N = lines.size();
            std::vector<Span> dummy;
            std::wstring para;
            int paraQ = 0;
            bool inPara = false;
            auto flushPara = [&]()
            {
                if (!inPara)
                    return;
                Block b;
                b.type = B_PARA;
                b.quote = paraQ;
                b.spans = ParseInline(para);
                out.push_back(std::move(b));
                para.clear();
                inPara = false;
            };
            auto blockStart = [&](size_t i, const std::wstring &rest)
            {
                const std::wstring t = Trim(rest);
                int lv;
                std::wstring mk, tx, nm;
                if (t.empty())
                    return true;
                if (StartsAt(t, 0, L"```") || StartsAt(t, 0, L"~~~") || StartsAt(t, 0, L"$$") || StartsAt(t, 0, L"\\["))
                    return true;
                if (t[0] == L'#')
                {
                    size_t h = 0;
                    while (h < t.size() && t[h] == L'#')
                        ++h;
                    if (h <= 6 && h < t.size() && t[h] == L' ')
                        return true;
                }
                if (IsHr(t) || ListMarker(rest, lv, mk, tx) || IsMathEnvStart(t, nm))
                    return true;
                if (t.find(L'|') != std::wstring::npos && i + 1 < N && IsTableSep(lines[i + 1]))
                    return true;
                return false;
            };
            for (size_t i = 0; i < N; ++i)
            {
                int q;
                std::wstring rest;
                StripQuote(lines[i], q, rest);
                const std::wstring t = Trim(rest);
                if (inPara && q != paraQ)
                    flushPara();
                if (t.empty())
                {
                    flushPara();
                    continue;
                }
                // fenced code
                if (StartsAt(t, 0, L"```") || StartsAt(t, 0, L"~~~"))
                {
                    flushPara();
                    const wchar_t fc = t[0];
                    size_t k = 0;
                    while (k < t.size() && t[k] == fc)
                        ++k;
                    Block b;
                    b.type = B_CODE;
                    b.quote = q;
                    b.lang = Trim(t.substr(k));
                    size_t j = i + 1;
                    for (; j < N; ++j)
                    {
                        int q2;
                        std::wstring r2;
                        StripQuote(lines[j], q2, r2);
                        const std::wstring t2 = Trim(r2);
                        if (t2.size() >= k && t2.find_first_not_of(fc) == std::wstring::npos)
                            break;
                        if (!b.text.empty() || j > i + 1)
                            b.text += L'\n';
                        b.text += (q > 0 ? r2 : lines[j]);
                    }
                    out.push_back(std::move(b));
                    i = j;
                    continue;
                }
                // display math: $$ ... $$, \[ ... \], \begin{env} ... \end{env}
                std::wstring envName;
                const bool dollar = StartsAt(t, 0, L"$$"), brack = StartsAt(t, 0, L"\\["), env = IsMathEnvStart(t, envName);
                if (dollar || brack || env)
                {
                    flushPara();
                    const wchar_t *closer = dollar ? L"$$" : brack ? L"\\]"
                                                                   : nullptr;
                    const std::wstring endTag = env ? L"\\end{" + envName + L"}" : L"";
                    std::wstring first = (dollar || brack) ? t.substr(2) : t;
                    Block b;
                    b.type = B_MATH;
                    b.quote = q;
                    size_t j = i;
                    std::wstring cur = first;
                    for (;;)
                    {
                        const size_t e = env ? cur.find(endTag) : cur.find(closer);
                        if (e != std::wstring::npos)
                        {
                            b.text += cur.substr(0, e + (env ? endTag.size() : 0));
                            break;
                        }
                        b.text += cur + L'\n';
                        if (++j >= N)
                            break;
                        int q2;
                        std::wstring r2;
                        StripQuote(lines[j], q2, r2);
                        cur = r2;
                    }
                    out.push_back(std::move(b));
                    i = std::min(j, N - 1);
                    continue;
                }
                // heading
                if (t[0] == L'#')
                {
                    size_t h = 0;
                    while (h < t.size() && t[h] == L'#')
                        ++h;
                    if (h <= 6 && h < t.size() && t[h] == L' ')
                    {
                        flushPara();
                        Block b;
                        b.type = B_HEAD;
                        b.level = (int)h;
                        b.quote = q;
                        std::wstring txt = Trim(t.substr(h));
                        while (!txt.empty() && txt.back() == L'#')
                            txt.pop_back();
                        b.spans = ParseInline(Trim(txt));
                        out.push_back(std::move(b));
                        continue;
                    }
                }
                if (IsHr(t))
                {
                    flushPara();
                    Block b;
                    b.type = B_HR;
                    b.quote = q;
                    out.push_back(std::move(b));
                    continue;
                }
                // table
                if (t.find(L'|') != std::wstring::npos && i + 1 < N && IsTableSep(lines[i + 1]))
                {
                    flushPara();
                    Block b;
                    b.type = B_TABLE;
                    b.quote = q;
                    auto header = SplitRow(t);
                    for (const std::wstring &sc : SplitRow(lines[i + 1]))
                    {
                        const bool l = !sc.empty() && sc[0] == L':', r = !sc.empty() && sc.back() == L':';
                        b.aligns.push_back(l && r ? 1 : r ? 2
                                                          : 0);
                    }
                    std::vector<std::vector<Span>> hr;
                    for (auto &c : header)
                        hr.push_back(ParseInline(c));
                    b.rows.push_back(std::move(hr));
                    size_t j = i + 2;
                    for (; j < N; ++j)
                    {
                        int q2;
                        std::wstring r2;
                        StripQuote(lines[j], q2, r2);
                        const std::wstring t2 = Trim(r2);
                        if (t2.empty() || t2.find(L'|') == std::wstring::npos)
                            break;
                        std::vector<std::vector<Span>> row;
                        for (auto &c : SplitRow(t2))
                            row.push_back(ParseInline(c));
                        b.rows.push_back(std::move(row));
                    }
                    out.push_back(std::move(b));
                    i = j - 1;
                    continue;
                }
                // list item
                int lv;
                std::wstring mk, tx;
                if (ListMarker(rest, lv, mk, tx))
                {
                    flushPara();
                    Block b;
                    b.type = B_LIST;
                    b.level = lv;
                    b.quote = q;
                    b.marker = mk;
                    if (StartsAt(tx, 0, L"[ ] "))
                    {
                        b.marker = L"\u2610";
                        tx = tx.substr(4);
                    }
                    else if (StartsAt(tx, 0, L"[x] ") || StartsAt(tx, 0, L"[X] "))
                    {
                        b.marker = L"\u2611";
                        tx = tx.substr(4);
                    }
                    size_t j = i + 1;
                    for (; j < N; ++j)
                    { // lazy continuation lines
                        int q2;
                        std::wstring r2;
                        StripQuote(lines[j], q2, r2);
                        if (q2 != q || blockStart(j, r2))
                            break;
                        tx += L"\n" + Trim(r2);
                    }
                    b.spans = ParseInline(tx);
                    out.push_back(std::move(b));
                    i = j - 1;
                    continue;
                }
                // paragraph text (single newlines stay line breaks: pasted chat text relies on them)
                if (inPara)
                    para += L"\n" + t;
                else
                {
                    para = t;
                    inPara = true;
                    paraQ = q;
                }
            }
            flushPara();
            return out;
        }

        // ---- inline layout --------------------------------------------------------------------------
        struct Atom
        {
            float w = 0, asc = 0, desc = 0;
            bool space = false, brk = false;
            std::vector<Op> ops;
        };

        static void BuildAtoms(const std::vector<Span> &spans, const Style &base, std::vector<Atom> &out)
        {
            for (const Span &sp : spans)
            {
                if (sp.kind == 3)
                {
                    Atom a;
                    a.brk = true;
                    out.push_back(a);
                    continue;
                }
                if (sp.kind == 2)
                {
                    MBox mb = MathBox(sp.s, sp.disp, base.size);
                    Atom a;
                    a.w = mb.w + 4.f;
                    a.asc = mb.asc;
                    a.desc = mb.desc;
                    AppendOps(a.ops, mb.ops, 2.f, 0);
                    out.push_back(std::move(a));
                    continue;
                }
                Style st = base;
                if (sp.kind == 1)
                {
                    st.fam = F_MONO;
                    st.size = base.size * 0.9f;
                    st.weight = 400;
                    st.italic = false;
                }
                else
                {
                    if (sp.bold)
                        st.weight = std::max(base.weight, 700);
                    if (sp.italic)
                        st.italic = true;
                    if (sp.link)
                        st.color = C_ACCENT;
                }
                const FontInfo &fi = Font(st);
                size_t i = 0;
                const std::wstring &t = sp.s;
                while (i < t.size())
                {
                    const bool sp0 = t[i] == L' ' || t[i] == L'\t';
                    size_t j = i;
                    while (j < t.size() && ((t[j] == L' ' || t[j] == L'\t') == sp0))
                        ++j;
                    Atom a;
                    a.asc = fi.baseline;
                    a.desc = fi.height - fi.baseline;
                    if (sp0)
                    {
                        a.space = true;
                        a.w = Width(L" ", st) * (float)(j - i > 3 ? 3 : (j - i));
                    }
                    else
                    {
                        const std::wstring word = t.substr(i, j - i);
                        const float w = Width(word, st);
                        const float pad = sp.kind == 1 ? 3.f : 0.f;
                        if (sp.kind == 1)
                            a.ops.push_back(MkRect(0, -fi.baseline * 0.95f, w + 2 * pad, (fi.height - fi.baseline) * 0.9f, 3.f, C_CODEBG));
                        Op o;
                        o.kind = OP_TEXT;
                        o.x = pad;
                        o.y = 0;
                        o.x2 = w;
                        o.st = st;
                        o.s = word;
                        o.color = st.color;
                        if (sp.link)
                            o.url = sp.url; // underlined while drawing, so long links can still be split to fit
                        a.ops.push_back(o);
                        if (sp.strike)
                            a.ops.push_back(MkLine(0, -0.3f * st.size, w, -0.3f * st.size, 1.f, st.color));
                        a.w = w + 2 * pad;
                    }
                    out.push_back(std::move(a));
                    i = j;
                }
            }
        }

        // Splits plain over-wide words so nothing is clipped on the right.
        static void SplitWide(std::vector<Atom> &atoms, float width)
        {
            std::vector<Atom> res;
            for (Atom &a : atoms)
            {
                if (a.space || a.brk || a.w <= width || a.ops.size() != 1 || a.ops[0].kind != OP_TEXT || width < 20.f)
                {
                    res.push_back(std::move(a));
                    continue;
                }
                const Op src = a.ops[0];
                std::wstring rem = src.s;
                while (!rem.empty())
                {
                    size_t lo = 1, hi = rem.size();
                    while (lo < hi)
                    { // longest prefix that fits
                        const size_t mid = (lo + hi + 1) / 2;
                        if (Width(rem.substr(0, mid), src.st) <= width)
                            lo = mid;
                        else
                            hi = mid - 1;
                    }
                    Atom piece;
                    Op o = src;
                    o.s = rem.substr(0, lo);
                    o.x = 0;
                    o.x2 = Width(o.s, src.st);
                    piece.w = o.x2;
                    piece.asc = a.asc;
                    piece.desc = a.desc;
                    piece.ops.push_back(o);
                    res.push_back(std::move(piece));
                    rem.erase(0, lo);
                    if (!rem.empty())
                    {
                        Atom br;
                        br.brk = true;
                        res.push_back(br);
                    }
                }
            }
            atoms.swap(res);
        }

        // Greedy line breaking; returns the height used. firstBaseline = baseline of line 1 (relative to y).
        static float LayoutAtoms(std::vector<Atom> &atoms, float x0, float width, float y, float minAsc, float minDesc, int align,
                                 std::vector<Op> &out, float *firstBaseline = nullptr)
        {
            SplitWide(atoms, width);
            const float extra = 0.12f * (minAsc + minDesc);
            float used = 0;
            bool first = true;
            size_t i = 0;
            const size_t N = atoms.size();
            while (i < N)
            {
                while (i < N && atoms[i].space)
                    ++i;
                if (i >= N)
                    break;
                size_t j = i;
                float x = 0;
                if (!atoms[i].brk)
                {
                    while (j < N && !atoms[j].brk)
                    {
                        if (!atoms[j].space && x + atoms[j].w > width + 0.5f && j > i)
                            break;
                        x += atoms[j].w;
                        ++j;
                    }
                }
                size_t e = j;
                while (e > i && atoms[e - 1].space)
                {
                    x -= atoms[e - 1].w;
                    --e;
                }
                float asc = minAsc, desc = minDesc;
                for (size_t k = i; k < e; ++k)
                {
                    asc = std::max(asc, atoms[k].asc);
                    desc = std::max(desc, atoms[k].desc);
                }
                const float baseline = y + used + extra * 0.5f + asc;
                if (first)
                {
                    if (firstBaseline)
                        *firstBaseline = baseline - y;
                    first = false;
                }
                float cx = x0 + (align == 1 ? (width - x) / 2 : align == 2 ? width - x
                                                                           : 0.f);
                for (size_t k = i; k < e; ++k)
                {
                    AppendOps(out, atoms[k].ops, cx, baseline);
                    cx += atoms[k].w;
                }
                used += asc + desc + extra;
                i = (j < N && atoms[j].brk) ? j + 1 : j;
                if (i >= N && j < N && atoms[j].brk)
                    used += minAsc + minDesc + extra; // trailing explicit break
            }
            if (first)
            {
                if (firstBaseline)
                    *firstBaseline = minAsc + extra * 0.5f;
                used = minAsc + minDesc + extra;
            }
            return used;
        }

        static float LayoutSpans(const std::vector<Span> &spans, const Style &st, float x0, float width, float y, int align,
                                 std::vector<Op> &ops, float *firstBaseline = nullptr)
        {
            std::vector<Atom> atoms;
            BuildAtoms(spans, st, atoms);
            const FontInfo &fi = Font(st);
            return LayoutAtoms(atoms, x0, width, y, fi.baseline, fi.height - fi.baseline, align, ops, firstBaseline);
        }

        static std::wstring PlainOf(const std::vector<Span> &spans)
        {
            std::wstring r;
            for (auto &s : spans)
            {
                if (s.kind == 3)
                    r += L' ';
                else
                    r += s.s;
            }
            return r;
        }

        struct Doc
        {
            std::vector<Op> ops;
            float height = 0;
        };

        static Doc BuildDoc(const std::wstring &src, float W, float base)
        {
            Doc d;
            const std::vector<Block> blocks = ParseBlocks(src);
            float y = 0;
            for (size_t bi = 0; bi < blocks.size(); ++bi)
            {
                const Block &b = blocks[bi];
                const float y0 = y;
                const float x0 = b.quote * 14.f, w = std::max(40.f, W - x0);
                Style bs;
                bs.size = base;
                switch (b.type)
                {
                case B_HEAD:
                {
                    static const float hm[7] = {1.f, 1.65f, 1.4f, 1.2f, 1.08f, 1.f, .95f};
                    if (y > 0)
                        y += base * 0.3f;
                    bs.size = base * hm[std::min(6, std::max(1, b.level))];
                    bs.weight = 700;
                    y += LayoutSpans(b.spans, bs, x0, w, y, 0, d.ops);
                    if (b.level <= 2)
                    {
                        y += 3;
                        d.ops.push_back(MkLine(x0, y, x0 + w, y, 1.f, C_BORDER));
                        y += 3;
                    }
                    break;
                }
                case B_PARA:
                    y += LayoutSpans(b.spans, bs, x0, w, y, 0, d.ops);
                    break;
                case B_LIST:
                {
                    const float mx = x0 + b.level * 18.f, ordered = (b.marker != L"*" && b.marker != L"\u2610" && b.marker != L"\u2611") ? 1.f : 0.f;
                    const float tx = mx + (ordered > 0 ? 24.f : 17.f);
                    float fb = 0;
                    const float h = LayoutSpans(b.spans, bs, tx, std::max(30.f, W - tx), y, 0, d.ops, &fb);
                    std::wstring mk = b.marker;
                    if (mk == L"*")
                        mk = b.level == 0 ? L"\u2022" : b.level == 1 ? L"\u25E6"
                                                                     : L"\u25AA";
                    Op o;
                    o.kind = OP_TEXT;
                    o.st = bs;
                    o.st.color = C_SUB;
                    o.s = mk;
                    o.color = C_SUB;
                    o.x2 = Width(mk, o.st);
                    o.x = ordered > 0 ? tx - 5.f - o.x2 : mx + 2.f;
                    o.y = y + fb;
                    d.ops.push_back(o);
                    y += h;
                    break;
                }
                case B_CODE:
                {
                    Style cs = bs;
                    cs.fam = F_MONO;
                    cs.size = base * 0.88f;
                    const FontInfo &fi = Font(cs);
                    const float cw = std::max(1.f, Width(L"M", cs)), inner = w - 20.f;
                    const int cpl = std::max(8, (int)(inner / cw));
                    std::vector<std::wstring> rows;
                    size_t pos = 0;
                    const std::wstring &tx = b.text;
                    for (;;)
                    {
                        size_t e = tx.find(L'\n', pos);
                        std::wstring line = tx.substr(pos, e == std::wstring::npos ? std::wstring::npos : e - pos);
                        for (auto &ch : line)
                            if (ch == L'\t')
                                ch = L' ';
                        if (line.empty())
                            rows.push_back(L"");
                        for (size_t k = 0; k < line.size(); k += (size_t)cpl)
                            rows.push_back(line.substr(k, (size_t)cpl));
                        if (e == std::wstring::npos)
                            break;
                        pos = e + 1;
                    }
                    const float lh = fi.height * 1.08f, pad = 8.f, labelH = b.lang.empty() ? 0.f : base * 0.75f;
                    const float H = rows.size() * lh + 2 * pad + labelH * 0.6f;
                    d.ops.push_back(MkRect(x0, y, x0 + w, y + H, 6.f, C_CODEBG));
                    if (!b.lang.empty())
                    {
                        Style ls = bs;
                        ls.size = base * 0.72f;
                        ls.color = C_SUB;
                        Op o;
                        o.kind = OP_TEXT;
                        o.st = ls;
                        o.s = b.lang;
                        o.color = C_SUB;
                        o.x2 = Width(b.lang, ls);
                        o.x = x0 + w - 10.f - o.x2;
                        o.y = y + pad + Font(ls).baseline * 0.9f;
                        d.ops.push_back(o);
                    }
                    float ry = y + pad + labelH * 0.3f;
                    for (auto &r : rows)
                    {
                        if (!r.empty())
                        {
                            Op o;
                            o.kind = OP_TEXT;
                            o.st = cs;
                            o.s = r;
                            o.x = x0 + 10.f;
                            o.y = ry + fi.baseline;
                            o.x2 = Width(r, cs);
                            d.ops.push_back(o);
                        }
                        ry += lh;
                    }
                    y += H;
                    break;
                }
                case B_MATH:
                {
                    float sz = base * 1.12f;
                    MBox mb = MathBox(b.text, true, sz);
                    if (mb.w > w && mb.w > 0)
                    {
                        sz = std::max(base * 0.55f, sz * w / mb.w);
                        mb = MathBox(b.text, true, sz);
                    }
                    y += 4.f;
                    AppendOps(d.ops, mb.ops, x0 + std::max(0.f, (w - mb.w) / 2), y + mb.asc);
                    y += mb.asc + mb.desc + 4.f;
                    break;
                }
                case B_HR:
                    y += 6.f;
                    d.ops.push_back(MkLine(x0, y, x0 + w, y, 1.f, C_BORDER));
                    y += 6.f;
                    break;
                case B_TABLE:
                {
                    size_t nc = 0;
                    for (auto &r : b.rows)
                        nc = std::max(nc, r.size());
                    if (!nc)
                        break;
                    std::vector<float> nat(nc, 30.f);
                    for (auto &r : b.rows)
                        for (size_t c = 0; c < r.size(); ++c)
                        {
                            const std::wstring pt = PlainOf(r[c]);
                            Style ms = bs;
                            ms.weight = 700;
                            nat[c] = std::max(nat[c], Width(pt.substr(0, 60), ms) + 18.f);
                        }
                    float total = 0;
                    for (float v : nat)
                        total += v;
                    std::vector<float> cwid(nc);
                    if (total <= w)
                    {
                        for (size_t c = 0; c < nc; ++c)
                            cwid[c] = nat[c] + (w - total) / (float)nc;
                    }
                    else
                    {
                        float sum = 0;
                        for (size_t c = 0; c < nc; ++c)
                        {
                            cwid[c] = std::max(46.f, nat[c] * w / total);
                            sum += cwid[c];
                        }
                        if (sum > w)
                            for (size_t c = 0; c < nc; ++c)
                                cwid[c] *= w / sum;
                    }
                    std::vector<float> cx(nc + 1, 0.f);
                    for (size_t c = 0; c < nc; ++c)
                        cx[c + 1] = cx[c] + cwid[c];
                    const float ty0 = y;
                    std::vector<float> rowY;
                    for (size_t r = 0; r < b.rows.size(); ++r)
                    {
                        rowY.push_back(y);
                        std::vector<Op> tmp;
                        float rh = 0;
                        Style cs = bs;
                        if (r == 0)
                            cs.weight = 700;
                        for (size_t c = 0; c < b.rows[r].size(); ++c)
                        {
                            const int al = c < b.aligns.size() ? b.aligns[c] : 0;
                            rh = std::max(rh, LayoutSpans(b.rows[r][c], cs, x0 + cx[c] + 7.f, std::max(20.f, cwid[c] - 14.f), y + 5.f, al, tmp));
                        }
                        rh += 10.f;
                        if (r == 0)
                            d.ops.push_back(MkRect(x0, y, x0 + cx[nc], y + rh, 0.f, C_HEAD));
                        d.ops.insert(d.ops.end(), tmp.begin(), tmp.end());
                        y += rh;
                        d.ops.push_back(MkLine(x0, y, x0 + cx[nc], y, 1.f, C_BORDER));
                    }
                    d.ops.push_back(MkLine(x0, ty0, x0 + cx[nc], ty0, 1.f, C_BORDER));
                    for (size_t c = 0; c <= nc; ++c)
                        d.ops.push_back(MkLine(x0 + cx[c], ty0, x0 + cx[c], y, 1.f, C_BORDER));
                    break;
                }
                }
                const bool tightNext = b.type == B_LIST && bi + 1 < blocks.size() && blocks[bi + 1].type == B_LIST;
                const float gap = tightNext ? base * 0.25f : base * 0.6f;
                if (b.quote > 0)
                {
                    const bool contNext = bi + 1 < blocks.size() && blocks[bi + 1].quote >= b.quote;
                    for (int l = 1; l <= b.quote; ++l)
                        d.ops.push_back(MkRect((l - 1) * 14.f + 1.f, y0, (l - 1) * 14.f + 4.f, y + (contNext ? gap : 0.f), 1.5f, C_BAR));
                }
                y += gap;
            }
            d.height = y;
            return d;
        }

    } // namespace rich
} // namespace ui

// ============================================================================
//  PANEL PAGES + ALERT WINDOWS
//  PanelWindow owns content sizing/scrolling/transitions; engine state is in td::.
// ============================================================================
namespace ui
{

    enum Ids
    {
        ID_TAB = 1,
        ID_CAL_PREV,
        ID_CAL_NEXT,
        ID_CAL_HDR,
        ID_CAL_CELL,
        ID_CAL_TODAY,
        ID_SW_TOGGLE,
        ID_SW_LAP,
        ID_SW_RESET,
        ID_TM_STEP,
        ID_TM_START,
        ID_TM_PRESET,
        ID_TM_SAVEPRESET,
        ID_TM_TOGGLE,
        ID_TM_CANCEL,
        ID_TM_PRESET_DELETE,
        ID_AL_ADD,
        ID_AL_ROW,
        ID_AL_TOGGLE,
        ID_AL_DEL,
        ID_AL_STEP,
        ID_AL_DAY,
        ID_AL_SNOOZE,
        ID_AL_SAVE,
        ID_AL_CANCEL,
        ID_AL_PM,
        ID_AL_SOUND,
        ID_AL_SOUND_PLAY,
        ID_NOTE_ADD,
        ID_NOTE_OPEN,
        ID_NOTE_DEL,
        ID_NOTE_BACK,
        ID_NOTE_EDIT,
        ID_NOTE_ZOOM,
        ID_ST_BELL,
        ID_ST_PREVIEW,
        ID_ST_LAYOUT,
        ID_ST_FORMAT,
        ID_ST_ACRYLIC,
        ID_ST_TSOUND,
        ID_ST_TSOUND_PLAY,
        ID_ST_ASOUND,
        ID_ST_ASOUND_PLAY,
        ID_ST_SYNC,
        ID_ABOUT_TOGGLE,
        ID_ABOUT_WINDHAWK,
        ID_ABOUT_GITHUB,
        ID_NOTE_COPY,
        ID_NOTE_LINK,
        ID_ALERT_DISMISS,
        ID_ALERT_SNOOZE
    };
    enum EditSlot
    {
        E_TNAME,
        E_TH,
        E_TM,
        E_TS,
        E_AH,
        E_AM,
        E_ALABEL,
        E_NTITLE,
        E_NBODY,
        E_COUNT
    };

    // Layout grid (DIPs): 16 outer margin, 8 gap between cards, 14 inner card padding.
    static const float kPanelW = 372.f;
    static const float kPanelH = 488.f; // calendar: last row ends 418, button 436-470, 18 above and below
    static const float kTabH = 46.f;
    static const float kLapRegionTop = 258.f, kLapTop = 292.f, kLapRow = 36.f;
    static const int kTabs = 6;
    static const wchar_t *const kTabNames[kTabs] = {L"Calendar", L"Stopwatch", L"Timer", L"Alarm", L"Notes", L"Settings"};
    static const float kTabAnimMs = 260.f;
    static const float kGlideMs = 80.f; // scroll glide time constant
    static const float kWheelPx = 56.f; // pixels per 120-unit wheel notch
    static const float kFrameMs = 8.f;  // animation timer interval (the display paces the real rate)
    static const float kSlideOpenMs = 250.f, kSlideCloseMs = 210.f;
    static const float kAboutMs = 220.f; // About card expand / collapse

    static bool IsShellFlyoutWindow(HWND fg)
    {
        if (!fg)
            return true;
        DWORD pid = 0;
        GetWindowThreadProcessId(fg, &pid);
        if (!pid)
            return true;
        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!h)
            return false;
        wchar_t p[MAX_PATH] = {};
        DWORD n = MAX_PATH;
        bool r = false;
        if (QueryFullProcessImageNameW(h, 0, p, &n))
            r = tb::ContainsNoCase(p, L"ShellExperienceHost.exe") || tb::ContainsNoCase(p, L"ShellHost.exe");
        CloseHandle(h);
        return r;
    }

    static std::wstring Two(int v) { return td::Fmt(L"%02d", v); }

    // ---------------------------------------------------------------------------------------------
    //  CLIPBOARD HTML -> MARKDOWN
    //  Browsers and chat apps put rich HTML on the clipboard next to the plain text. The plain text of a copied
    //  table, quote or code block has lost its structure; the HTML still has it. Pasting into a note converts
    //  that HTML into the Markdown the preview understands (headings, lists, tables, quotes, code, links, maths).
    // ---------------------------------------------------------------------------------------------
    namespace htmlmd
    {
        struct HNode
        {
            std::wstring tag, text;
            std::vector<std::pair<std::wstring, std::wstring>> attrs;
            std::vector<int> kids;
            int parent = -1;
            bool isText = false;
        };

        static std::wstring Lower(std::wstring s)
        {
            for (auto &c : s)
                c = (wchar_t)towlower(c);
            return s;
        }
        static bool IsWs(wchar_t c) { return c == L' ' || c == L'\t' || c == L'\n' || c == L'\r' || c == 0xA0 || c == 0x200B; }
        static std::wstring Trim(const std::wstring &s)
        {
            size_t a = 0, b = s.size();
            while (a < b && IsWs(s[a]))
                ++a;
            while (b > a && IsWs(s[b - 1]))
                --b;
            return s.substr(a, b - a);
        }

        static std::wstring DecodeEntities(const std::wstring &s)
        {
            if (s.find(L'&') == std::wstring::npos)
                return s;
            static const struct
            {
                const wchar_t *n;
                wchar_t c;
            } kNamed[] = {{L"amp", L'&'}, {L"lt", L'<'}, {L"gt", L'>'}, {L"quot", L'"'}, {L"apos", L'\''}, {L"nbsp", L' '}, {L"ndash", 0x2013}, {L"mdash", 0x2014}, {L"hellip", 0x2026}, {L"lsquo", 0x2018}, {L"rsquo", 0x2019}, {L"ldquo", 0x201C}, {L"rdquo", 0x201D}, {L"times", 0xD7}, {L"middot", 0xB7}, {L"bull", 0x2022}, {L"copy", 0xA9}, {L"reg", 0xAE}, {L"deg", 0xB0}, {L"plusmn", 0xB1}, {L"rarr", 0x2192}, {L"larr", 0x2190}};
            std::wstring o;
            o.reserve(s.size());
            for (size_t i = 0; i < s.size(); ++i)
            {
                if (s[i] != L'&')
                {
                    o += s[i];
                    continue;
                }
                const size_t semi = s.find(L';', i + 1);
                if (semi == std::wstring::npos || semi - i > 10)
                {
                    o += s[i];
                    continue;
                }
                const std::wstring ent = s.substr(i + 1, semi - i - 1);
                unsigned long cp = 0;
                bool ok = false;
                if (!ent.empty() && ent[0] == L'#')
                {
                    wchar_t *e = nullptr;
                    cp = (ent.size() > 1 && (ent[1] == L'x' || ent[1] == L'X')) ? wcstoul(ent.c_str() + 2, &e, 16) : wcstoul(ent.c_str() + 1, &e, 10);
                    ok = e && *e == 0 && cp > 0 && cp < 0x110000;
                }
                else
                    for (auto &k : kNamed)
                        if (ent == k.n)
                        {
                            cp = k.c;
                            ok = true;
                            break;
                        }
                if (!ok)
                {
                    o += s[i];
                    continue;
                }
                if (cp >= 0x10000)
                { // surrogate pair
                    cp -= 0x10000;
                    o += (wchar_t)(0xD800 + (cp >> 10));
                    o += (wchar_t)(0xDC00 + (cp & 0x3FF));
                }
                else
                    o += (wchar_t)cp;
                i = semi;
            }
            return o;
        }

        static bool IsVoid(const std::wstring &t)
        {
            static const wchar_t *const v[] = {L"br", L"hr", L"img", L"meta", L"link", L"input", L"wbr", L"col", L"source", L"area", L"base", L"embed", L"param", L"track"};
            for (auto *x : v)
                if (t == x)
                    return true;
            return false;
        }

        struct Converter
        {
            std::vector<HNode> N;
            static const size_t kMaxNodes = 80000;

            const std::wstring *Attr(int id, const wchar_t *name) const
            {
                for (auto &a : N[id].attrs)
                    if (a.first == name)
                        return &a.second;
                return nullptr;
            }
            bool HasClass(int id, const wchar_t *cls) const
            {
                const std::wstring *c = Attr(id, L"class");
                if (!c)
                    return false;
                const std::wstring padded = L" " + *c + L" ";
                return padded.find(std::wstring(L" ") + cls + L" ") != std::wstring::npos;
            }

            int NewNode(int parent)
            {
                N.emplace_back();
                N.back().parent = parent;
                const int id = (int)N.size() - 1;
                if (parent >= 0)
                    N[parent].kids.push_back(id);
                return id;
            }

            void Parse(const std::wstring &h)
            {
                N.clear();
                N.reserve(2048);
                NewNode(-1); // root
                std::vector<int> st{0};
                const size_t n = h.size();
                size_t i = 0;
                while (i < n && N.size() < kMaxNodes)
                {
                    if (h[i] == L'<')
                    {
                        if (h.compare(i, 4, L"<!--") == 0)
                        {
                            const size_t e = h.find(L"-->", i + 4);
                            i = e == std::wstring::npos ? n : e + 3;
                            continue;
                        }
                        if (i + 1 < n && (h[i + 1] == L'!' || h[i + 1] == L'?'))
                        {
                            const size_t e = h.find(L'>', i);
                            i = e == std::wstring::npos ? n : e + 1;
                            continue;
                        }
                        if (i + 1 < n && h[i + 1] == L'/')
                        {
                            size_t j = i + 2, k = j;
                            while (k < n && h[k] != L'>' && !IsWs(h[k]))
                                ++k;
                            const std::wstring name = Lower(h.substr(j, k - j));
                            const size_t e = h.find(L'>', k);
                            i = e == std::wstring::npos ? n : e + 1;
                            for (size_t d = st.size(); d-- > 1;)
                                if (N[st[d]].tag == name)
                                {
                                    st.resize(d);
                                    break;
                                }
                            continue;
                        }
                        if (i + 1 < n && iswalpha(h[i + 1]))
                        {
                            size_t k = i + 1;
                            while (k < n && !IsWs(h[k]) && h[k] != L'>' && h[k] != L'/')
                                ++k;
                            const int id = NewNode(st.back());
                            N[id].tag = Lower(h.substr(i + 1, k - i - 1));
                            bool selfClose = false;
                            while (k < n && h[k] != L'>')
                            {
                                if (IsWs(h[k]))
                                {
                                    ++k;
                                    continue;
                                }
                                if (h[k] == L'/')
                                {
                                    selfClose = true;
                                    ++k;
                                    continue;
                                }
                                selfClose = false;
                                size_t ns = k;
                                while (k < n && !IsWs(h[k]) && h[k] != L'=' && h[k] != L'>' && h[k] != L'/')
                                    ++k;
                                std::wstring an = Lower(h.substr(ns, k - ns)), av;
                                while (k < n && IsWs(h[k]))
                                    ++k;
                                if (k < n && h[k] == L'=')
                                {
                                    ++k;
                                    while (k < n && IsWs(h[k]))
                                        ++k;
                                    if (k < n && (h[k] == L'"' || h[k] == L'\''))
                                    {
                                        const wchar_t q = h[k++];
                                        const size_t vs = k;
                                        while (k < n && h[k] != q)
                                            ++k;
                                        av = h.substr(vs, k - vs);
                                        if (k < n)
                                            ++k;
                                    }
                                    else
                                    {
                                        const size_t vs = k;
                                        while (k < n && !IsWs(h[k]) && h[k] != L'>')
                                            ++k;
                                        av = h.substr(vs, k - vs);
                                    }
                                }
                                if (!an.empty())
                                    N[id].attrs.emplace_back(an, DecodeEntities(av));
                            }
                            i = k < n ? k + 1 : n;
                            const std::wstring tag = N[id].tag;
                            if (tag == L"script" || tag == L"style")
                            { // raw text element: keep the body (maths scripts need it) and skip past it
                                const size_t e = Lower(h.substr(i, std::min<size_t>(n - i, 200000))).find(L"</" + tag);
                                const size_t end = e == std::wstring::npos ? n : i + e;
                                const int t = NewNode(id);
                                N[t].isText = true;
                                N[t].text = h.substr(i, end - i);
                                const size_t gt = h.find(L'>', end);
                                i = gt == std::wstring::npos ? n : gt + 1;
                                continue;
                            }
                            if (!selfClose && !IsVoid(tag))
                            {
                                if (tag == L"li" && N[st.back()].tag == L"li")
                                    st.pop_back();
                                if ((tag == L"td" || tag == L"th") && (N[st.back()].tag == L"td" || N[st.back()].tag == L"th"))
                                    st.pop_back();
                                if (st.size() < 256)
                                    st.push_back(NewNodeReparent(id, st));
                            }
                            continue;
                        }
                    }
                    size_t e = h.find(L'<', i + 1);
                    if (e == std::wstring::npos)
                        e = n;
                    const int t = NewNode(st.back());
                    N[t].isText = true;
                    N[t].text = DecodeEntities(h.substr(i, e - i));
                    i = e;
                }
            }
            // The element node was created with the stack top as parent before an implicit close adjusted the
            // stack; re-attach it under the (possibly new) top so nesting stays correct.
            int NewNodeReparent(int id, const std::vector<int> &st)
            {
                const int want = st.back();
                if (N[id].parent != want)
                {
                    auto &old = N[N[id].parent].kids;
                    old.erase(std::remove(old.begin(), old.end(), id), old.end());
                    N[id].parent = want;
                    N[want].kids.push_back(id);
                }
                return id;
            }

            static bool Skipped(const std::wstring &t)
            {
                return t == L"script" || t == L"style" || t == L"head" || t == L"svg" || t == L"button" || t == L"noscript" || t == L"template" ||
                       t == L"iframe" || t == L"canvas" || t == L"title" || t == L"select" || t == L"mrow-skip";
            }

            std::wstring RawText(int id) const
            {
                const HNode &n = N[id];
                if (n.isText)
                    return n.text;
                if (n.tag == L"br")
                    return L"\n";
                if (Skipped(n.tag))
                    return L"";
                std::wstring o;
                for (int k : n.kids)
                    o += RawText(k);
                return o;
            }

            int FindTag(int id, const wchar_t *tag, int depth = 0) const
            {
                if (depth > 12)
                    return -1;
                for (int k : N[id].kids)
                {
                    if (!N[k].isText && N[k].tag == tag)
                        return k;
                    const int r = FindTag(k, tag, depth + 1);
                    if (r >= 0)
                        return r;
                }
                return -1;
            }
            int FindAnnotation(int id, int depth = 0) const
            {
                if (depth > 14)
                    return -1;
                for (int k : N[id].kids)
                {
                    if (N[k].isText)
                        continue;
                    if (N[k].tag == L"annotation")
                    {
                        const std::wstring *enc = Attr(k, L"encoding");
                        if (!enc || enc->find(L"tex") != std::wstring::npos)
                            return k;
                    }
                    const int r = FindAnnotation(k, depth + 1);
                    if (r >= 0)
                        return r;
                }
                return -1;
            }

            static std::wstring Math(const std::wstring &tex, bool display)
            {
                const std::wstring t = Trim(tex);
                if (t.empty())
                    return L"";
                return display ? L"\n\n$$\n" + t + L"\n$$\n\n" : L"$" + t + L"$";
            }

            static std::wstring Fence(const std::wstring &code, const std::wstring &lang)
            {
                std::wstring c = code;
                while (!c.empty() && (c.back() == L'\n' || c.back() == L'\r'))
                    c.pop_back();
                size_t s0 = 0;
                while (s0 < c.size() && (c[s0] == L'\n' || c[s0] == L'\r'))
                    ++s0;
                c = c.substr(s0);
                std::wstring f = L"```";
                if (c.find(L"```") != std::wstring::npos)
                    f = L"````";
                return L"\n\n" + f + lang + L"\n" + c + L"\n" + f + L"\n\n";
            }

            static std::wstring Collapse(std::wstring s) // 3+ newlines -> 2, trailing blanks off lines
            {
                std::wstring o;
                o.reserve(s.size());
                int nl = 0;
                for (size_t i = 0; i < s.size(); ++i)
                {
                    wchar_t c = s[i];
                    if (c == L'\r')
                        continue;
                    if (c == L'\n')
                    {
                        while (!o.empty() && o.back() == L' ')
                            o.pop_back();
                        if (++nl > 2)
                            continue;
                    }
                    else
                        nl = 0;
                    o += c;
                }
                return o;
            }

            static std::wstring Indent(const std::wstring &s, const std::wstring &first, const std::wstring &rest)
            {
                std::wstring o = first;
                bool firstLine = true;
                for (size_t i = 0; i <= s.size();)
                {
                    size_t e = s.find(L'\n', i);
                    if (e == std::wstring::npos)
                        e = s.size();
                    const std::wstring line = s.substr(i, e - i);
                    if (!firstLine)
                    {
                        o += L'\n';
                        if (!line.empty())
                            o += rest;
                    }
                    o += line;
                    firstLine = false;
                    i = e + 1;
                }
                return o;
            }

            std::wstring Children(int id, bool pre, bool inLi, int depth)
            {
                std::wstring out;
                if (depth > 120)
                    return out;
                for (int k : N[id].kids)
                {
                    const HNode &c = N[k];
                    if (c.isText)
                    {
                        if (pre)
                        {
                            out += c.text;
                            continue;
                        }
                        std::wstring t;
                        bool lastSpace = out.empty() || out.back() == L'\n' || out.back() == L' ';
                        for (wchar_t ch : c.text)
                        {
                            if (IsWs(ch))
                            {
                                if (!lastSpace)
                                    t += L' ';
                                lastSpace = true;
                            }
                            else
                            {
                                t += ch;
                                lastSpace = false;
                            }
                        }
                        out += t;
                    }
                    else
                        out += Node(k, pre, inLi, depth + 1);
                }
                return out;
            }

            std::wstring Wrap(int id, const wchar_t *mark, bool pre, bool inLi, int depth)
            {
                const std::wstring inner = Children(id, pre, inLi, depth);
                const std::wstring core = Trim(inner);
                if (core.empty())
                    return inner;
                return (IsWs(inner.front()) ? L" " : L"") + std::wstring(mark) + core + mark + (IsWs(inner.back()) ? L" " : L"");
            }

            std::wstring Node(int id, bool pre, bool inLi, int depth)
            {
                const HNode &n = N[id];
                const std::wstring &t = n.tag;
                if (Skipped(t))
                {
                    if (t == L"script")
                    { // MathJax: <script type="math/tex[; mode=display]">
                        const std::wstring *ty = Attr(id, L"type");
                        if (ty && ty->find(L"math/tex") != std::wstring::npos)
                            return Math(RawText(id), ty->find(L"display") != std::wstring::npos);
                    }
                    return L"";
                }
                if (const std::wstring *dm = Attr(id, L"data-math")) // Gemini
                    return Math(*dm, t == L"math-block" || HasClass(id, L"math-block") || HasClass(id, L"display"));
                if (HasClass(id, L"katex-display") || HasClass(id, L"katex") || HasClass(id, L"MathJax_Display"))
                {
                    const int a = FindAnnotation(id);
                    if (a >= 0)
                        return Math(RawText(a), HasClass(id, L"katex-display"));
                    if (HasClass(id, L"katex-display") || HasClass(id, L"katex"))
                        return L""; // a KaTeX span without TeX source: drop the duplicated visual text
                }
                if (t == L"math" || t == L"mjx-container")
                {
                    const int a = FindAnnotation(id);
                    return a >= 0 ? Math(RawText(a), false) : Trim(RawText(id));
                }
                if (t == L"br")
                    return pre ? L"\n" : L"  \n";
                if (t == L"hr")
                    return L"\n\n---\n\n";
                if (t == L"img")
                {
                    const std::wstring *alt = Attr(id, L"alt");
                    return alt ? *alt : L"";
                }
                if (t.size() == 2 && t[0] == L'h' && t[1] >= L'1' && t[1] <= L'6')
                {
                    const std::wstring inner = Trim(Collapse(Children(id, false, false, depth)));
                    if (inner.empty())
                        return L"";
                    return L"\n\n" + std::wstring((size_t)(t[1] - L'0'), L'#') + L" " + inner + L"\n\n";
                }
                if (t == L"strong" || t == L"b")
                    return Wrap(id, L"**", pre, inLi, depth);
                if (t == L"em" || t == L"i")
                    return Wrap(id, L"*", pre, inLi, depth);
                if (t == L"del" || t == L"s" || t == L"strike")
                    return Wrap(id, L"~~", pre, inLi, depth);
                if (t == L"pre")
                {
                    const int code = FindTag(id, L"code");
                    const int src = code >= 0 ? code : id;
                    std::wstring lang;
                    for (int cand : {code, id})
                    {
                        if (cand < 0 || !Attr(cand, L"class"))
                            continue;
                        const std::wstring &cls = *Attr(cand, L"class");
                        size_t p = cls.find(L"language-");
                        const size_t skip = 9;
                        if (p == std::wstring::npos)
                        {
                            p = cls.find(L"lang-");
                            if (p == std::wstring::npos)
                                continue;
                            size_t e = p + 5;
                            while (e < cls.size() && !IsWs(cls[e]))
                                ++e;
                            lang = cls.substr(p + 5, e - p - 5);
                        }
                        else
                        {
                            size_t e = p + skip;
                            while (e < cls.size() && !IsWs(cls[e]))
                                ++e;
                            lang = cls.substr(p + skip, e - p - skip);
                        }
                        break;
                    }
                    return Fence(RawText(src), lang);
                }
                if (t == L"code")
                {
                    const std::wstring raw = RawText(id);
                    if (raw.find(L'\n') != std::wstring::npos)
                        return Fence(raw, L"");
                    const std::wstring core = Trim(raw);
                    if (core.empty())
                        return L"";
                    const std::wstring tick = core.find(L'`') != std::wstring::npos ? L"``" : L"`";
                    return tick + (core.front() == L'`' || core.back() == L'`' ? L" " + core + L" " : core) + tick;
                }
                if (t == L"a")
                {
                    const std::wstring text = Trim(Collapse(Children(id, false, inLi, depth)));
                    const std::wstring *href = Attr(id, L"href");
                    if (!href || text.empty() || (href->compare(0, 4, L"http") != 0 && href->compare(0, 7, L"mailto:") != 0) || text == *href)
                        return text;
                    return L"[" + text + L"](" + *href + L")";
                }
                if (t == L"blockquote")
                {
                    const std::wstring inner = Trim(Collapse(Children(id, false, false, depth)));
                    if (inner.empty())
                        return L"";
                    std::wstring o;
                    for (size_t i = 0; i <= inner.size();)
                    {
                        size_t e = inner.find(L'\n', i);
                        if (e == std::wstring::npos)
                            e = inner.size();
                        o += e > i ? L"> " + inner.substr(i, e - i) : std::wstring(L">");
                        o += L'\n';
                        i = e + 1;
                    }
                    return L"\n\n" + o + L"\n";
                }
                if (t == L"ul" || t == L"ol")
                {
                    const bool ordered = t == L"ol";
                    int num = 1;
                    if (const std::wstring *st = Attr(id, L"start"))
                        num = std::max(1, (int)wcstol(st->c_str(), nullptr, 10));
                    std::wstring items;
                    for (int k : n.kids)
                    {
                        if (N[k].isText || N[k].tag != L"li")
                            continue;
                        const std::wstring inner = Trim(Collapse(Children(k, false, true, depth)));
                        const std::wstring bullet = ordered ? std::to_wstring(num++) + L". " : std::wstring(L"- ");
                        items += Indent(inner, bullet, std::wstring(bullet.size(), L' ')) + L"\n";
                    }
                    if (items.empty())
                        return L"";
                    return inLi ? L"\n" + items : L"\n\n" + items + L"\n";
                }
                if (t == L"table")
                    return Table(id, depth);
                if (t == L"p" || t == L"section" || t == L"article" || t == L"figure" || t == L"details" || t == L"summary" || t == L"header" || t == L"footer")
                    return L"\n\n" + Children(id, pre, inLi, depth) + L"\n\n";
                if (t == L"div" || t == L"li" || t == L"tr" || t == L"dt" || t == L"dd" || t == L"main" || t == L"aside" || t == L"nav" || t == L"center")
                {
                    if (const std::wstring *style = Attr(id, L"style"))
                        if (style->find(L"white-space: pre") != std::wstring::npos || style->find(L"white-space:pre") != std::wstring::npos)
                            return L"\n" + Children(id, true, inLi, depth) + L"\n";
                    return L"\n" + Children(id, pre, inLi, depth) + L"\n";
                }
                return Children(id, pre, inLi, depth); // span, font, sup, sub, label, unknown elements: just the content
            }

            void CollectRows(int id, std::vector<std::vector<std::wstring>> &rows, int depth)
            {
                if (depth > 8)
                    return;
                for (int k : N[id].kids)
                {
                    if (N[k].isText)
                        continue;
                    if (N[k].tag == L"tr")
                    {
                        std::vector<std::wstring> cells;
                        for (int c : N[k].kids)
                        {
                            if (N[c].isText || (N[c].tag != L"td" && N[c].tag != L"th"))
                                continue;
                            std::wstring txt = Trim(Collapse(Children(c, false, true, depth)));
                            std::wstring flat;
                            for (wchar_t ch : txt)
                            {
                                if (ch == L'\n')
                                    flat += L' ';
                                else if (ch == L'|')
                                    flat += L"\\|";
                                else
                                    flat += ch;
                            }
                            cells.push_back(flat);
                        }
                        if (!cells.empty())
                            rows.push_back(std::move(cells));
                    }
                    else if (N[k].tag == L"thead" || N[k].tag == L"tbody" || N[k].tag == L"tfoot")
                        CollectRows(k, rows, depth + 1);
                }
            }

            std::wstring Table(int id, int depth)
            {
                std::vector<std::vector<std::wstring>> rows;
                CollectRows(id, rows, depth);
                if (rows.empty())
                    return L"";
                size_t cols = 0;
                for (auto &r : rows)
                    cols = std::max(cols, r.size());
                std::wstring o = L"\n\n";
                auto line = [&](const std::vector<std::wstring> &r)
                {
                    o += L"|";
                    for (size_t c = 0; c < cols; ++c)
                        o += L" " + (c < r.size() ? r[c] : std::wstring()) + L" |";
                    o += L"\n";
                };
                line(rows[0]);
                o += L"|";
                for (size_t c = 0; c < cols; ++c)
                    o += L" --- |";
                o += L"\n";
                for (size_t r = 1; r < rows.size(); ++r)
                    line(rows[r]);
                return o + L"\n";
            }
        };

        // Windows "HTML Format" clipboard payload (UTF-8 with a header) -> Markdown. Empty string = nothing usable.
        static std::wstring FromClipboardHtml(const std::string &raw)
        {
            size_t s0 = 0, e0 = raw.size();
            auto readOff = [&](const char *key) -> long
            {
                const size_t p = raw.find(key);
                return p == std::string::npos ? -1 : atol(raw.c_str() + p + strlen(key));
            };
            const long fs = readOff("StartFragment:"), fe = readOff("EndFragment:");
            if (fs > 0 && fe > fs && (size_t)fe <= raw.size())
            {
                s0 = (size_t)fs;
                e0 = (size_t)fe;
            }
            else
            {
                const size_t a = raw.find("<!--StartFragment-->"), b = raw.find("<!--EndFragment-->");
                if (a != std::string::npos && b != std::string::npos && b > a)
                {
                    s0 = a + 20;
                    e0 = b;
                }
                else
                {
                    const size_t h = raw.find('<');
                    s0 = h == std::string::npos ? 0 : h;
                }
            }
            const std::string frag = raw.substr(s0, e0 - s0);
            if (frag.empty())
                return L"";
            const int wn = MultiByteToWideChar(CP_UTF8, 0, frag.data(), (int)frag.size(), nullptr, 0);
            if (wn <= 0)
                return L"";
            std::wstring w((size_t)wn, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, frag.data(), (int)frag.size(), &w[0], wn);
            Converter cv;
            cv.Parse(w);
            std::wstring md = Trim(Converter::Collapse(cv.Children(0, false, false, 0)));
            return md;
        }

        // Plain text copied from a table (rows on lines, cells separated by tabs) -> a Markdown table.
        static std::wstring TabsToTable(const std::wstring &text)
        {
            std::vector<std::wstring> lines;
            size_t i = 0;
            while (i <= text.size())
            {
                size_t e = text.find(L'\n', i);
                if (e == std::wstring::npos)
                    e = text.size();
                std::wstring l = text.substr(i, e - i);
                while (!l.empty() && l.back() == L'\r')
                    l.pop_back();
                if (!l.empty() || e < text.size())
                    lines.push_back(l);
                i = e + 1;
            }
            while (!lines.empty() && lines.back().empty())
                lines.pop_back();
            if (lines.size() < 2)
                return L"";
            size_t cols = 0;
            std::vector<std::vector<std::wstring>> rows;
            for (auto &l : lines)
            {
                if (l.find(L'\t') == std::wstring::npos)
                    return L""; // every line must be a row
                std::vector<std::wstring> cells;
                size_t p = 0;
                for (;;)
                {
                    size_t q = l.find(L'\t', p);
                    cells.push_back(Trim(l.substr(p, q == std::wstring::npos ? std::wstring::npos : q - p)));
                    if (q == std::wstring::npos)
                        break;
                    p = q + 1;
                }
                cols = std::max(cols, cells.size());
                rows.push_back(std::move(cells));
            }
            std::wstring o;
            for (size_t r = 0; r < rows.size(); ++r)
            {
                o += L"|";
                for (size_t c = 0; c < cols; ++c)
                {
                    std::wstring cell = c < rows[r].size() ? rows[r][c] : std::wstring();
                    std::wstring esc;
                    for (wchar_t ch : cell)
                        esc += ch == L'|' ? std::wstring(L"\\|") : std::wstring(1, ch);
                    o += L" " + esc + L" |";
                }
                o += L"\n";
                if (r == 0)
                {
                    o += L"|";
                    for (size_t c = 0; c < cols; ++c)
                        o += L" --- |";
                    o += L"\n";
                }
            }
            return o;
        }

        // Reads the clipboard and returns Markdown if it carries structure worth converting.
        static bool ClipboardMarkdown(HWND owner, std::wstring &md)
        {
            static const UINT fmtHtml = RegisterClipboardFormatW(L"HTML Format");
            bool opened = false;
            for (int i = 0; i < 5 && !opened; ++i)
            {
                opened = OpenClipboard(owner) != FALSE;
                if (!opened)
                    Sleep(8);
            }
            if (!opened)
                return false;
            std::string html;
            std::wstring plain;
            if (IsClipboardFormatAvailable(fmtHtml))
                if (HANDLE h = GetClipboardData(fmtHtml))
                    if (const char *p = (const char *)GlobalLock(h))
                    {
                        const size_t sz = GlobalSize(h);
                        html.assign(p, strnlen(p, sz));
                        GlobalUnlock(h);
                    }
            if (IsClipboardFormatAvailable(CF_UNICODETEXT))
                if (HANDLE h = GetClipboardData(CF_UNICODETEXT))
                    if (const wchar_t *p = (const wchar_t *)GlobalLock(h))
                    {
                        plain.assign(p, wcsnlen(p, GlobalSize(h) / sizeof(wchar_t)));
                        GlobalUnlock(h);
                    }
            CloseClipboard();
            if (!html.empty())
            {
                md = FromClipboardHtml(html);
                if (!md.empty())
                    return true;
            }
            md = TabsToTable(plain);
            return !md.empty();
        }

        static bool SetClipboardText(HWND owner, const std::wstring &text)
        {
            bool opened = false;
            for (int i = 0; i < 5 && !opened; ++i)
            {
                opened = OpenClipboard(owner) != FALSE;
                if (!opened)
                    Sleep(8);
            }
            if (!opened)
                return false;
            bool ok = false;
            EmptyClipboard();
            HGLOBAL g = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(wchar_t));
            if (g)
            {
                if (wchar_t *p = (wchar_t *)GlobalLock(g))
                {
                    memcpy(p, text.c_str(), (text.size() + 1) * sizeof(wchar_t));
                    GlobalUnlock(g);
                    ok = SetClipboardData(CF_UNICODETEXT, g) != nullptr;
                }
                if (!ok)
                    GlobalFree(g); // on success the clipboard owns the block
            }
            CloseClipboard();
            return ok;
        }
    } // namespace htmlmd

    // ---------------------------------------------------------------------------------------------
    //  TIME SYNC
    //  Only administrators may change the system clock, so a plain user process cannot do it directly.
    //  Order of attempts, each silent until the last:
    //    1. run `w32tm /resync` directly (works when Windows already allows it);
    //    2. otherwise run it elevated. This shows one UAC prompt per sync; nothing is installed or left behind.
    //  Runs on a worker thread and posts the result back. The worker is interruptible: shutting down sets
    //  g_syncStop, which ends every wait and terminates a non-elevated child, and the unload then joins the thread.
    // ---------------------------------------------------------------------------------------------
    enum SyncResult
    {
        SYNC_OK = 0,
        SYNC_CANCELLED,
        SYNC_FAILED
    };
    static std::atomic<bool> g_syncBusy{false};
    static HANDLE g_syncThread = nullptr;
    static HANDLE g_syncStop = nullptr; // manual-reset event: created by Start(), set by Stop()

    static bool SyncStopRequested() { return g_syncStop && WaitForSingleObject(g_syncStop, 0) == WAIT_OBJECT_0; }

    // WAIT_OBJECT_0 = the process ended; anything else = timeout or shutdown.
    static DWORD WaitProcessOrStop(HANDLE proc, DWORD timeoutMs)
    {
        HANDLE hs[2] = {proc, g_syncStop};
        return WaitForMultipleObjects(g_syncStop ? 2 : 1, hs, FALSE, timeoutMs);
    }

    static bool RunHidden(const std::wstring &exe, const std::wstring &args, DWORD timeoutMs, DWORD &exitCode)
    {
        wchar_t sys[MAX_PATH] = {};
        GetSystemDirectoryW(sys, MAX_PATH);
        std::wstring cmd = L"\"" + std::wstring(sys) + L"\\" + exe + L"\" " + args;
        STARTUPINFOW si{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        PROCESS_INFORMATION pi{};
        if (!CreateProcessW(nullptr, &cmd[0], nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
            return false;
        const DWORD w = WaitProcessOrStop(pi.hProcess, timeoutMs);
        if (w != WAIT_OBJECT_0)
            TerminateProcess(pi.hProcess, 1);
        exitCode = 1;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return w == WAIT_OBJECT_0;
    }

    // Runs a Microsoft-signed system tool elevated (one UAC prompt). ShellExecuteExW returns only after the prompt
    // has been answered, so a shutdown during the prompt is handled right after it returns.
    static bool RunElevated(const wchar_t *exe, const std::wstring &args, DWORD timeoutMs, DWORD &exitCode, bool &cancelled)
    {
        wchar_t sys[MAX_PATH] = {};
        GetSystemDirectoryW(sys, MAX_PATH);
        const std::wstring file = std::wstring(sys) + L"\\" + exe;
        SHELLEXECUTEINFOW sei{};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
        sei.lpVerb = L"runas";
        sei.lpFile = file.c_str();
        sei.lpParameters = args.c_str();
        sei.nShow = SW_HIDE;
        cancelled = false;
        if (!ShellExecuteExW(&sei))
        {
            cancelled = GetLastError() == ERROR_CANCELLED;
            return false;
        }
        exitCode = 1;
        if (sei.hProcess)
        {
            if (!SyncStopRequested())
                WaitProcessOrStop(sei.hProcess, timeoutMs); // an elevated child cannot be terminated from here
            GetExitCodeProcess(sei.hProcess, &exitCode);
            CloseHandle(sei.hProcess);
        }
        return true;
    }

    // The Windows Time service is trigger-started, so it may not be running yet.
    static bool W32TimeRunning()
    {
        bool running = false;
        SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
        if (scm)
        {
            SC_HANDLE svc = OpenServiceW(scm, L"w32time", SERVICE_QUERY_STATUS);
            if (svc)
            {
                SERVICE_STATUS st{};
                if (QueryServiceStatus(svc, &st))
                    running = st.dwCurrentState == SERVICE_RUNNING;
                CloseServiceHandle(svc);
            }
            CloseServiceHandle(scm);
        }
        return running;
    }

    static DWORD WINAPI SyncThreadProc(LPVOID)
    {
        SyncResult result = SYNC_FAILED;
        DWORD ec = 1;
        if (RunHidden(L"w32tm.exe", L"/resync /force", 10000, ec) && ec == 0)
            result = SYNC_OK;
        else if (!g_shuttingDown.load() && !SyncStopRequested())
        {
            // One UAC prompt for this sync only. When the service is stopped, start it in the same elevated step.
            bool cancelled = false;
            const bool launched = W32TimeRunning()
                                      ? RunElevated(L"w32tm.exe", L"/resync /force", 20000, ec, cancelled)
                                      : RunElevated(L"cmd.exe", L"/c net start w32time & w32tm /resync /force", 30000, ec, cancelled);
            if (!launched)
                result = cancelled ? SYNC_CANCELLED : SYNC_FAILED;
            else
                result = ec == 0 ? SYNC_OK : SYNC_FAILED;
        }
        HWND h = g_panelWnd.load();
        if (h && !g_shuttingDown.load())
            PostMessageW(h, WM_UI_SYNCDONE, (WPARAM)result, 0);
        g_syncBusy = false;
        return 0;
    }

    static bool StartTimeSync()
    {
        if (g_syncBusy.exchange(true))
            return false;
        if (g_syncThread)
        {
            CloseHandle(g_syncThread);
            g_syncThread = nullptr;
        }
        g_syncThread = CreateThread(nullptr, 0, SyncThreadProc, nullptr, 0, nullptr);
        if (!g_syncThread)
        {
            g_syncBusy = false;
            return false;
        }
        return true;
    }

    static LRESULT CALLBACK BodyEditProc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR dwRefData); // defined after g_panel

    // ---------------------------------------------------------------------------------------------
    class PanelWindow : public UiWindow
    {
    public:
        int tab = 0;
        int calView = 0, calYear = 2025, calMonth = 1, calSelY = 0, calSelM = 0, calSelD = 0, yearBase = 2020;
        float panelH = kPanelH; // fixed for a given monitor; never follows the selected tab
        // panelScroll is the visible position; panelScrollTarget is where it is gliding to.
        // panelScrollStarted != 0 marks "gliding".
        float panelScroll = 0.f, panelScrollFrom = 0.f, panelScrollTarget = 0.f;
        ULONGLONG panelScrollStarted = 0;
        Smooth lapSm;  // stopwatch laps, in pixels
        Smooth prevSm; // notes preview, in pixels
        Smooth edSm;   // notes editor, in text lines
        Smooth calSm;  // calendar: vertical offset of the visible grid from the committed month/year/decade
        ULONGLONG calLastInput = 0, lastStep = 0;
        bool calDiscrete = false;
        float panVelocity = 0.f; // px/ms, for touch and pen momentum
        ULONGLONG panLastTick = 0;
        bool timerPeriodActive = false;
        std::vector<std::wstring> noteLinks; // link targets of the preview hit areas (rebuilt every paint)
        int panelWorkBottom = 0;             // physical px: bottom edge of the work area (top of the taskbar), clip line of the slide
        bool slideClipped = false;
        // The panel slides out from *behind* the taskbar (z-order below it) instead of being cut by a window region:
        // regions on a DWM-backdrop window flashed a grey/white strip over the taskbar and dropped the acrylic.
        HWND slideTaskbar = nullptr;
        int slideClipY = 0; // physical px: the panel is cut at this line (top edge of the taskbar)
        bool slideUnder = false;
        bool activateAfterOpen = false;
        // settings: About card
        bool aboutOpen = false;
        Anim aboutA;
        float settingsAboutTop = 852.f; // y of the About card (kept in step with PaintSettings)
        bool syncBusy = false;

        enum class PanelAnimationState
        {
            Idle,
            Opening,
            Closing
        };
        PanelAnimationState panelAnimation = PanelAnimationState::Idle;
        ULONGLONG panelAnimationStarted = 0;
        int panelAnimationX = 0, panelAnimationWidth = 0, panelAnimationHeight = 0, panelAnimationStartY = 0, panelAnimationEndY = 0;
        float startFade = 0.f, endFade = 1.f, currentFade = 1.f;

        ULONGLONG contentTransitionStarted = 0, statusStarted = 0, statusUntil = 0, shownAt = 0;
        bool statusTimerActive = false;
        std::wstring statusMsg;

        // scrollable tab bar
        float tabX[kTabs] = {}, tabW[kTabs] = {}, tabTotal = 0.f;
        Anim tabScrollA, indXA, indWA;

        // notes (multi-note store; list -> preview -> editor)
        struct Note
        {
            int id = 0;
            std::wstring title, body;
            long long modified = 0;
        }; // modified = unix seconds
        std::vector<Note> notes;
        int nextNoteId = 1, curNote = 0, noteView = 0; // noteView: 0 list, 1 rich preview, 2 editor
        int noteZoom = 100, pendingFocus = -1, noteDelArmedId = 0;
        bool noteDirty = false, suppressChange = false, docValid = false;
        unsigned noteVersion = 0, docVersion = 0;
        int docNote = 0;
        float docBase = 0.f, bodyLineH = 18.f;
        ULONGLONG noteDelArmedUntil = 0;
        rich::Doc doc;

        int presetDeleteIndex = -1;
        bool alarmEditing = false, editPM = false;
        td::AlarmItem edit;
        td::PanelSnap snap;
        HWND edits[E_COUNT] = {};
        D2D1_RECT_F editRect[E_COUNT] = {};
        bool editWant[E_COUNT] = {};
        RECT editLast[E_COUNT] = {};
        HFONT fontBig = nullptr, fontNorm = nullptr, fontTitle = nullptr, fontBody = nullptr;
        HBRUSH editBrush = nullptr;
        bool noActMode = false;
        int tickCount = 0, compMode = 0;
        BYTE restingAlpha = 255;

        ~PanelWindow() override
        {
            EndTimerPeriod();
            for (HFONT f : {fontBig, fontNorm, fontTitle, fontBody})
                if (f)
                    DeleteObject(f);
            if (editBrush)
                DeleteObject(editBrush);
        }

        static bool IsNumericSlot(int i) { return i == E_TH || i == E_TM || i == E_TS || i == E_AH || i == E_AM; }

        bool Create()
        {
            HINSTANCE inst = ModInst();
            CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED, kUiClass, L"WinKlok", WS_POPUP | WS_CLIPCHILDREN,
                            0, 0, 100, 100, nullptr, nullptr, inst, this);
            if (!hwnd)
                return false;
            dpi = GetDpiForWindow(hwnd);
            if (!dpi)
                dpi = 96;
            for (int i = 0; i < E_COUNT; ++i)
            {
                DWORD style = WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL;
                int limit = 40;
                if (IsNumericSlot(i))
                {
                    style |= ES_CENTER | ES_NUMBER;
                    limit = 2;
                }
                if (i == E_NTITLE)
                    limit = 120;
                if (i == E_NBODY)
                {
                    style = WS_CHILD | WS_TABSTOP | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN;
                    limit = 40000;
                }
                edits[i] = CreateWindowExW(0, L"EDIT", L"", style, 0, 0, 10, 10, hwnd, (HMENU)(INT_PTR)(100 + i), inst, nullptr);
                if (!edits[i])
                    return false;
                SendMessageW(edits[i], EM_LIMITTEXT, limit, 0);
            }
            for (int i = 0; i < E_COUNT; ++i)
                if (edits[i])
                {
                    WindhawkUtils::SetWindowSubclassFromAnyThread(edits[i], BodyEditProc, 0); // rich paste and wheel input
                }
            SendMessageW(edits[E_TNAME], 0x1501 /*EM_SETCUEBANNER*/, TRUE, (LPARAM)L"Timer name (optional)");
            SendMessageW(edits[E_ALABEL], 0x1501, TRUE, (LPARAM)L"Alarm name (optional)");
            SendMessageW(edits[E_NTITLE], 0x1501, TRUE, (LPARAM)L"Title");
            SetWindowTextW(edits[E_TH], L"00");
            SetWindowTextW(edits[E_TM], L"05");
            SetWindowTextW(edits[E_TS], L"00");
            LoadNotes();

            const float pad = 30.f; // text width + horizontal padding per tab; wider than the panel => scrollable
            float x = 6.f;
            for (int i = 0; i < kTabs; ++i)
            {
                tabX[i] = x;
                tabW[i] = std::max(64.f, MeasureText(kTabNames[i], 12.5f, 600) + pad);
                x += tabW[i];
            }
            tabTotal = x + 6.f;
            ApplyComposition();
            InitTabAnim();
            // Required for Windows to send PT_TOUCHPAD events to this panel. The active handler tracks
            // contacts by pointer ID and ptHimetricLocation; do not replace it with wheel-only fallback.
            // The registration itself is taken while the panel is open (ShowPanel) and released when it closes.
            g_touchpadPanelWindow = hwnd;
            if (ResolveRegisterTouchpadCapableWindow())
                Wh_Log(L"touchpad API found; direct per-pointer HIMETRIC tracking is registered while the panel is open");
            else
                Wh_Log(L"touchpad-capable API not available; two-finger scrolling uses the wheel hook");
            return true;
        }

        // Picks palette + panel material from the Windows transparency setting and the Settings > Panel material choice.
        void ApplyComposition()
        {
            const bool trans = TransparencyEffectsEnabled();
            const bool acrylic = trans && td::g_set.acrylic.load() != 0;
            compMode = acrylic ? 2 : trans ? 1
                                           : 0;
            grayText = compMode != 0;
            if (rt)
                rt->SetTextAntialiasMode(grayText ? D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE : D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
            pal = LoadPalette(acrylic);
            if (alphaTarget != acrylic)
            {
                alphaTarget = acrylic;
                ReleaseTarget();
            }
            DwmStyle(hwnd, pal);
            restingAlpha = ConfigureComposition(hwnd, pal, compMode);
            // GDI never writes alpha, so on the per-pixel-alpha acrylic surface native edit controls would turn
            // into see-through boxes. Layered children are composited as fully opaque instead.
            for (int i = 0; i < E_COUNT; ++i)
            {
                if (!edits[i])
                    continue;
                LONG_PTR ex = GetWindowLongPtrW(edits[i], GWL_EXSTYLE);
                if (acrylic)
                {
                    if (!(ex & WS_EX_LAYERED))
                        SetWindowLongPtrW(edits[i], GWL_EXSTYLE, ex | WS_EX_LAYERED);
                    SetLayeredWindowAttributes(edits[i], 0, 255, LWA_ALPHA);
                }
                else if (ex & WS_EX_LAYERED)
                    SetWindowLongPtrW(edits[i], GWL_EXSTYLE, ex & ~(LONG_PTR)WS_EX_LAYERED);
            }
            SetPanelFade(currentFade);
            RebuildGdi();
        }

        void RebuildGdi()
        {
            for (HFONT f : {fontBig, fontNorm, fontTitle, fontBody})
                if (f)
                    DeleteObject(f);
            if (editBrush)
                DeleteObject(editBrush);
            const float s = Scale();
            auto mk = [&](float px, int weight)
            {
                return CreateFontW(-(int)(px * s), 0, 0, 0, weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                   CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, g_uiFont.c_str());
            };
            fontBig = mk(24.f, FW_NORMAL);
            fontNorm = mk(15.f, FW_NORMAL);
            fontTitle = mk(18.f, FW_SEMIBOLD);
            fontBody = mk(15.f, FW_NORMAL);
            editBrush = CreateSolidBrush(RGB((int)(pal.field.r * 255), (int)(pal.field.g * 255), (int)(pal.field.b * 255)));
            for (int i = 0; i < E_COUNT; ++i)
            {
                HFONT f = (i == E_TH || i == E_TM || i == E_TS || i == E_AH || i == E_AM) ? fontBig
                          : i == E_NTITLE                                                 ? fontTitle
                          : i == E_NBODY                                                  ? fontBody
                                                                                          : fontNorm;
                if (edits[i])
                    SendMessageW(edits[i], WM_SETFONT, (WPARAM)f, TRUE);
            }
            memset(editLast, 0, sizeof(editLast));
            HDC dc = GetDC(hwnd); // line height of the note editor, for its thin scroll indicator
            HGDIOBJ old = SelectObject(dc, fontBody);
            TEXTMETRICW tm{};
            GetTextMetricsW(dc, &tm);
            SelectObject(dc, old);
            ReleaseDC(hwnd, dc);
            bodyLineH = std::max(8.f, (float)tm.tmHeight / Scale());
        }

        void OnDpiChanged() override { RebuildGdi(); }

        // ---------------------------------------------------------------- notes (multi-note store)
        // Each note lives in its own storage value ("note<id>" = modified|title|body, escaped); "noteIds" lists them.
        // Edits are written 600 ms after the last keystroke and again whenever the editor is left or the panel closes.
        static std::wstring NoteKey(int id) { return L"note" + std::to_wstring(id); }
        static std::wstring ToCrlf(const std::wstring &s)
        {
            std::wstring r;
            r.reserve(s.size() + 16);
            for (size_t i = 0; i < s.size(); ++i)
            {
                if (s[i] == L'\n' && (i == 0 || s[i - 1] != L'\r'))
                    r += L'\r';
                r += s[i];
            }
            return r;
        }
        std::wstring EditText(int slot)
        {
            int n = GetWindowTextLengthW(edits[slot]);
            std::wstring w((size_t)n + 1, L'\0');
            GetWindowTextW(edits[slot], &w[0], n + 1);
            w.resize((size_t)n);
            return w;
        }
        Note *FindNote(int id)
        {
            for (auto &n : notes)
                if (n.id == id)
                    return &n;
            return nullptr;
        }
        // Paste into the note body: rich clipboard content (tables, quotes, code, lists, maths) becomes Markdown.
        // Returns false to let the edit control paste the plain text itself.
        bool PasteIntoBody(HWND e)
        {
            std::wstring md;
            if (!htmlmd::ClipboardMarkdown(hwnd, md))
                return false;
            DWORD selA = 0, selB = 0;
            SendMessageW(e, EM_GETSEL, (WPARAM)&selA, (LPARAM)&selB);
            const int len = GetWindowTextLengthW(e);
            const int room = 40000 - (len - (int)(selB - selA));
            md = ToCrlf(md);
            if (room <= 0)
                return true;
            if ((int)md.size() > room)
                md.resize((size_t)room);
            SendMessageW(e, EM_REPLACESEL, TRUE, (LPARAM)md.c_str());
            return true;
        }
        void EditorWheelFromEdit(int delta) { EditorWheel(delta); }

        void SaveIndex()
        {
            std::wstring ids;
            for (auto &n : notes)
            {
                if (!ids.empty())
                    ids += L",";
                ids += std::to_wstring(n.id);
            }
            Wh_SetStringValue(L"noteIds", ids.c_str());
            Wh_SetIntValue(L"noteNext", nextNoteId);
        }
        void SaveNoteData(const Note &n)
        {
            const std::wstring v = std::to_wstring(n.modified) + L"|" + td::Esc(n.title) + L"|" + td::Esc(n.body);
            Wh_SetStringValue(NoteKey(n.id).c_str(), v.c_str());
        }
        void SortNotes()
        {
            std::stable_sort(notes.begin(), notes.end(), [](const Note &a, const Note &b)
                             { return a.modified > b.modified; });
        }
        void LoadNotes()
        {
            notes.clear();
            noteZoom = std::min(250, std::max(60, Wh_GetIntValue(L"noteZoom", 100)));
            std::vector<wchar_t> buf(100000);
            const size_t n = Wh_GetStringValue(L"noteIds", buf.data(), buf.size());
            int maxId = 0;
            if (n > 0 && n < buf.size())
            {
                for (const std::wstring &tok : td::Split(std::wstring(buf.data(), n), L','))
                {
                    const int id = (int)td::ToLL(tok, 0);
                    if (id <= 0)
                        continue;
                    const size_t m = Wh_GetStringValue(NoteKey(id).c_str(), buf.data(), buf.size());
                    if (m == 0 || m >= buf.size())
                        continue;
                    const auto f = td::Split(std::wstring(buf.data(), m), L'|');
                    if (f.size() < 3)
                        continue;
                    Note note;
                    note.id = id;
                    note.modified = td::ToLL(f[0]);
                    note.title = td::Unesc(f[1]);
                    note.body = td::Unesc(f[2]);
                    notes.push_back(std::move(note));
                    maxId = std::max(maxId, id);
                }
            }
            nextNoteId = std::max(maxId + 1, Wh_GetIntValue(L"noteNext", 1));
            SortNotes();
        }

        void FlushNote()
        {
            if (hwnd)
                KillTimer(hwnd, TIMER_NOTE_SAVE);
            if (!noteDirty)
                return;
            noteDirty = false;
            if (Note *n = FindNote(curNote))
                SaveNoteData(*n);
        }
        void OnNoteTextChanged()
        {
            if (suppressChange || noteView != 2)
                return;
            Note *n = FindNote(curNote);
            if (!n)
                return;
            n->title = EditText(E_NTITLE);
            n->body = EditText(E_NBODY);
            n->modified = td::WallMs() / 1000;
            noteDirty = true;
            ++noteVersion;
            SetTimer(hwnd, TIMER_NOTE_SAVE, 600, nullptr);
            Invalidate();
        }
        void NoteTransition()
        {
            panelScroll = panelScrollFrom = panelScrollTarget = 0.f;
            panelScrollStarted = 0;
            KillTimer(hwnd, TIMER_SCROLL_ANIM);
            edSm.Jump(0.f);
            prevSm.Jump(0.f);
            StartContentTransition();
            ResizePanelToContent();
        }
        void EnterEdit(int id, int focusSlot)
        {
            FlushNote();
            Note *n = FindNote(id);
            if (!n)
                return;
            curNote = id;
            suppressChange = true;
            SetWindowTextW(edits[E_NTITLE], n->title.c_str());
            SetWindowTextW(edits[E_NBODY], ToCrlf(n->body).c_str());
            suppressChange = false;
            noteView = 2;
            pendingFocus = focusSlot;
            NoteTransition();
        }
        void NewNote()
        {
            Note n;
            n.id = nextNoteId++;
            n.modified = td::WallMs() / 1000;
            notes.insert(notes.begin(), n);
            SaveNoteData(n);
            SaveIndex();
            EnterEdit(n.id, E_NTITLE);
        }
        void OpenNote(int id)
        {
            FlushNote();
            curNote = id;
            noteView = 1;
            docValid = false;
            prevSm.Jump(0.f);
            NoteTransition();
        }
        void LeaveEdit()
        {
            FlushNote();
            noteView = 1;
            docValid = false;
            prevSm.Jump(0.f);
            NoteTransition();
        }
        void BackToList()
        {
            FlushNote();
            noteView = 0;
            SortNotes();
            NoteTransition();
        }
        void DeleteNote(int id)
        {
            notes.erase(std::remove_if(notes.begin(), notes.end(), [&](const Note &n)
                                       { return n.id == id; }),
                        notes.end());
            Wh_DeleteValue(NoteKey(id).c_str());
            SaveIndex();
            if (curNote == id)
            {
                curNote = 0;
                noteDirty = false;
            }
            if (noteView != 0)
            {
                noteView = 0;
                NoteTransition();
            }
        }
        void SetNoteZoom(int z)
        {
            noteZoom = std::min(250, std::max(60, z));
            Wh_SetIntValue(L"noteZoom", noteZoom);
            docValid = false;
            Invalidate();
        }

        // ---- rich preview geometry / scrolling
        float PreviewViewH() const { return (panelH - 96.f) - 20.f; }
        float PreviewMax() const { return std::max(0.f, doc.height + 6.f - PreviewViewH()); }
        float PreviewNow() const { return prevSm.pos; }
        void PreviewScrollTo(float t)
        {
            prevSm.target = std::min(PreviewMax(), std::max(0.f, t));
            KickScroll();
        }
        void EnsureDoc()
        {
            Note *n = FindNote(curNote);
            if (!n)
                return;
            const float base = 14.5f * (float)noteZoom / 100.f;
            if (docValid && docNote == curNote && docVersion == noteVersion && docBase == base)
                return;
            doc = rich::BuildDoc(n->body, 324.f, base);
            docValid = true;
            docNote = curNote;
            docVersion = noteVersion;
            docBase = base;
            prevSm.Jump(std::min(PreviewMax(), std::max(0.f, prevSm.target)));
        }

        void SetStatus(const std::wstring &s)
        {
            statusMsg = s;
            statusStarted = GetTickCount64();
            statusUntil = statusStarted + 4000;
            if (hwnd)
                SetTimer(hwnd, TIMER_STATUS_ANIM, 15, nullptr);
            statusTimerActive = true;
            ResizePanelToContent();
            Invalidate();
        }

        // -------------------------- tab bar geometry ---------------------
        float TabMax() const { return std::max(0.f, tabTotal - kPanelW); }
        float TabScrollNow() const { return std::min(TabMax(), std::max(0.f, tabScrollA.Value(kTabAnimMs, true))); }
        float CenterScroll(int i) const
        {
            return std::min(TabMax(), std::max(0.f, tabX[i] + tabW[i] / 2 - kPanelW / 2));
        }
        void InitTabAnim()
        {
            indXA.Set(tabX[tab] + tabW[tab] * 0.25f);
            indWA.Set(tabW[tab] * 0.5f);
            tabScrollA.Set(CenterScroll(tab));
            if (hwnd)
                KillTimer(hwnd, TIMER_TAB_ANIM);
        }
        void StartTabTimer()
        {
            if (hwnd)
                SetTimer(hwnd, TIMER_TAB_ANIM, 15, nullptr);
        }

        // Selecting a tab slides the bar so the tab sits in the middle and its neighbours are revealed.
        void SelectTab(int i)
        {
            const float cs = TabScrollNow(), cx = indXA.Value(kTabAnimMs, false), cw = indWA.Value(kTabAnimMs, false);
            const bool changed = i != tab;
            tab = i;
            indXA.Go(tabX[i] + tabW[i] * 0.25f, cx);
            indWA.Go(tabW[i] * 0.5f, cw);
            tabScrollA.Go(CenterScroll(i), cs);
            StartTabTimer();
            if (!changed)
                return;
            StopTouchpadFling(); // a previous page's inertia must not scroll the newly selected page
            alarmEditing = false;
            StopPreviewSound();
            FlushNote();
            panelScroll = panelScrollFrom = panelScrollTarget = 0.f;
            panelScrollStarted = 0;
            lapSm.Jump(0.f);
            CalResetSlide();
            KillTimer(hwnd, TIMER_SCROLL_ANIM);
            StartContentTransition();
            ResizePanelToContent();
        }

        // -------------------------- responsive panel geometry ---------------------
        float LayoutPresets(std::vector<D2D1_RECT_F> *rects, D2D1_RECT_F *saveR, float y0) const
        {
            float x = 16, y = y0;
            const float h = 30;
            int rows = 1;
            for (size_t i = 0; i < snap.presets.size(); ++i)
            {
                float w = std::max(52.f, MeasureText(snap.presets[i].name, 12.f, 400) + 26.f);
                float total = w + (presetDeleteIndex == (int)i ? 22.f : 0.f);
                if (x + total > 356 && x > 16)
                {
                    x = 16;
                    y += h + 8;
                    ++rows;
                }
                if (rows > 3)
                    break;
                if (rects)
                    rects->push_back(RF(x, y, w, h));
                x += total + 8;
            }
            const float w = 74;
            if (x + w > 356 && x > 16)
            {
                x = 16;
                y += h + 8;
                ++rows;
            }
            if (rows <= 3 && saveR)
                *saveR = RF(x, y, w, h);
            return y + h;
        }

        float ContentBottomEstimate() const
        {
            switch (tab)
            {
            case 0:
                return 470.f + 16.f; // calendar: Today button bottom + bottom padding
            case 2:
            {
                float y = LayoutPresets(nullptr, nullptr, 228.f) + 32.f + 40.f + 18.f;
                if (!snap.timers.empty())
                    y += 26.f + (float)snap.timers.size() * 64.f - 4.f;
                return std::max(350.f, y + 16.f);
            }
            case 3:
                if (alarmEditing)
                    return 468.f + 16.f;
                return snap.alarms.empty() ? 190.f : 102.f + (float)snap.alarms.size() * 70.f + 8.f;
            case 4:
                if (noteView == 0)
                    return notes.empty() ? panelH : 92.f + (float)notes.size() * 60.f + 8.f;
                return panelH;
            case 5:
                return settingsAboutTop + AboutHeight() + 18.f; // settings stack, About card last
            default:
                return panelH;
            }
        }

        float ContentViewportBottom() const
        {
            // Non-calendar pages give up a bottom strip while a transient status message is visible.
            return std::max(58.f, panelH - ((!statusMsg.empty() && tab != 0) ? 30.f : 0.f));
        }

        float PanelScrollMax() const
        {
            if (tab == 1 || (tab == 4 && noteView != 0))
                return 0.f; // laps / note preview / editor scroll on their own
            return std::max(0.f, ContentBottomEstimate() - ContentViewportBottom());
        }

        float ContentTransitionProgress() const
        {
            return contentTransitionStarted ? EaseOutCubic((float)(GetTickCount64() - contentTransitionStarted) / 225.f) : 1.f;
        }
        float ContentTransitionOffset() const { return contentTransitionStarted ? 9.f * (1.f - ContentTransitionProgress()) : 0.f; }
        float ContentTransitionOpacity() const { return 0.22f + 0.78f * ContentTransitionProgress(); } // never a blank flash

        float StatusOpacity() const
        {
            if (statusMsg.empty())
                return 0.f;
            const ULONGLONG now = GetTickCount64();
            return std::min(EaseOutCubic((float)(now - statusStarted) / 170.f),
                            Clamp01((float)(statusUntil > now ? statusUntil - now : 0) / 190.f));
        }

        void StartContentTransition()
        {
            contentTransitionStarted = GetTickCount64();
            if (hwnd)
                SetTimer(hwnd, TIMER_CONTENT_ANIM, 15, nullptr);
            Invalidate();
        }

        // ---------------------------------------------------------------- smooth scrolling
        // Input only moves a *target*; StepScrollers() (run once per painted frame, using the real frame time)
        // glides the visible position towards it. Pixel positions are snapped to whole device pixels when drawn.
        void KickScroll()
        {
            if (hwnd)
                SetTimer(hwnd, TIMER_SCROLL_ANIM, (UINT)kFrameMs, nullptr);
            Invalidate();
        }

        void ScrollTo(float target)
        {
            panelScrollTarget = std::min(PanelScrollMax(), std::max(0.f, target));
            if (std::fabs(panelScrollTarget - panelScroll) < 0.05f)
            {
                panelScroll = panelScrollTarget;
                panelScrollStarted = 0;
                return;
            }
            panelScrollStarted = GetTickCount64();
            KickScroll();
        }

        float LapViewH() const { return std::max(40.f, ContentViewportBottom() - 12.f - kLapTop); }
        float LapMax() const { return std::max(0.f, (float)snap.laps.size() * kLapRow - 4.f - LapViewH()); }

        // calendar sliding: month grid (6 rows of 44), or the 4 x 3 year / decade grid (3 rows of 84)
        float CalGridH() const { return calView == 0 ? 264.f : 252.f; }
        bool CalStepAllowed(int dir) const
        {
            if (calView == 0)
            {
                const int idx = calYear * 12 + (calMonth - 1) + dir;
                return idx >= 1601 * 12 && idx <= 9998 * 12 + 11;
            }
            if (calView == 1)
                return calYear + dir >= 1601 && calYear + dir <= 9998;
            return yearBase + dir * 10 >= 1600 && yearBase + dir * 10 <= 9990;
        }
        void CalCommit(int dir)
        {
            if (calView == 0)
            {
                calMonth += dir;
                if (calMonth < 1)
                {
                    calMonth = 12;
                    --calYear;
                }
                if (calMonth > 12)
                {
                    calMonth = 1;
                    ++calYear;
                }
            }
            else if (calView == 1)
                calYear += dir;
            else
                yearBase += dir * 10;
        }
        // Wheel / touchpad: whole notches move one page; fine touchpad deltas scroll continuously and settle on a page.
        void CalWheel(int delta)
        {
            const float H = CalGridH();
            const bool notch = (delta % WHEEL_DELTA) == 0;
            calDiscrete = notch;
            const float t = calSm.target - (float)delta / (float)WHEEL_DELTA * (notch ? H : H * 0.55f);
            calSm.target = std::min(H * 3.f, std::max(-H * 3.f, t));
            calLastInput = GetTickCount64();
            KickScroll();
        }
        void CalSlide(int dir)
        { // arrow buttons: slide one page
            calDiscrete = true;
            calSm.target = std::min(CalGridH() * 3.f, std::max(-CalGridH() * 3.f, calSm.target + (float)dir * CalGridH()));
            calLastInput = GetTickCount64();
            KickScroll();
        }
        void CalResetSlide() { calSm.Jump(0.f); }

        // Scrolls the notes editor by lines (a native EDIT can only move whole lines; the glide animates them).
        void EditorWheel(int delta)
        {
            HWND e = edits[E_NBODY];
            if (!e)
                return;
            if (!edSm.Active())
                edSm.Jump((float)SendMessageW(e, EM_GETFIRSTVISIBLELINE, 0, 0));
            const int total = (int)SendMessageW(e, EM_GETLINECOUNT, 0, 0);
            const int vis = std::max(1, (int)(RH(editRect[E_NBODY]) / bodyLineH));
            const float maxFirst = (float)std::max(0, total - vis);
            edSm.target = std::min(maxFirst, std::max(0.f, edSm.target - (float)delta / (float)WHEEL_DELTA * 3.f));
            KickScroll();
        }

        void StepScrollers()
        {
            const ULONGLONG now = GetTickCount64();
            const float dt = lastStep ? std::min(50.f, (float)(now - lastStep)) : kFrameMs;
            lastStep = now;
            bool any = AdvanceTouchpadFling(dt);
            // Keep direct touchpad movement close to the fingers. During release momentum, follow its
            // continuously moving target with a modest smoothing lag instead of a single long target jump.
            const float tau = tpFlingActive ? 58.f : (now - lastTouchpadTick < 250 ? 42.f : kGlideMs);

            if (panelScrollStarted)
            {
                const float mx = std::max(0.f, PanelScrollMax());
                if (!aboutA.Running(kAboutMs))
                    panelScrollTarget = std::min(panelScrollTarget, mx);
                Smooth m;
                m.pos = panelScroll;
                m.target = panelScrollTarget;
                if (m.Step(dt, tau))
                    any = true;
                else
                    panelScrollStarted = 0;
                panelScroll = std::min(m.pos, mx);
            }
            panelScroll = std::min(panelScroll, std::max(0.f, PanelScrollMax()));
            if (tab == 1)
            {
                lapSm.target = std::min(LapMax(), std::max(0.f, lapSm.target));
                any |= lapSm.Step(dt, tau);
                lapSm.pos = std::min(LapMax(), std::max(0.f, lapSm.pos));
            }
            if (tab == 4 && noteView == 1)
            {
                prevSm.target = std::min(PreviewMax(), std::max(0.f, prevSm.target));
                any |= prevSm.Step(dt, tau);
                prevSm.pos = std::min(PreviewMax(), std::max(0.f, prevSm.pos));
            }
            if (tab == 4 && noteView == 2 && edits[E_NBODY] && edSm.Active())
            {
                any |= edSm.Step(dt, tau * 0.8f);
                const int want = (int)std::lround(edSm.pos);
                const int cur = (int)SendMessageW(edits[E_NBODY], EM_GETFIRSTVISIBLELINE, 0, 0);
                if (want != cur)
                    SendMessageW(edits[E_NBODY], EM_LINESCROLL, 0, want - cur);
            }
            if (tab == 0 && (calSm.Active() || calSm.pos != 0.f))
            {
                const float H = CalGridH();
                if (!calDiscrete && now - calLastInput > 110)
                    calSm.target = std::round(calSm.target / H) * H; // settle on a whole page
                any |= calSm.Step(dt, tau * 1.15f);
                while (calSm.pos > H * 0.5f && CalStepAllowed(1))
                {
                    CalCommit(1);
                    calSm.pos -= H;
                    calSm.target -= H;
                }
                while (calSm.pos < -H * 0.5f && CalStepAllowed(-1))
                {
                    CalCommit(-1);
                    calSm.pos += H;
                    calSm.target += H;
                }
                if (!CalStepAllowed(1))
                {
                    calSm.target = std::min(calSm.target, 0.f);
                    calSm.pos = std::min(calSm.pos, 0.f);
                }
                if (!CalStepAllowed(-1))
                {
                    calSm.target = std::max(calSm.target, 0.f);
                    calSm.pos = std::max(calSm.pos, 0.f);
                }
            }
            if (!any)
            {
                lastStep = 0;
                if (hwnd)
                    KillTimer(hwnd, TIMER_SCROLL_ANIM);
            }
        }

        void AdvanceScrollAnimation()
        {
            if (!hwnd || !IsWindowVisible(hwnd))
            {
                if (hwnd)
                    KillTimer(hwnd, TIMER_SCROLL_ANIM);
                lastStep = 0;
                return;
            }
            Invalidate(); // the glide itself is advanced while painting, in step with the display
        }

        // ---- precision touchpad: raw two-finger movement -> the same OnWheel path as a mouse wheel
        float tpRemX = 0.f, tpRemY = 0.f;
        bool tpFlingActive = false, tpFlingHorizontal = false;
        float tpFlingVelocity = 0.f, tpFlingWheelRemainder = 0.f, tpFlingX = 0.f, tpFlingY = 0.f;
        ULONGLONG tpFlingStarted = 0, tpFlingLastTick = 0;

        void StopTouchpadFling()
        {
            tpFlingActive = false;
            tpFlingVelocity = tpFlingWheelRemainder = 0.f;
            tpFlingStarted = tpFlingLastTick = 0;
        }

        // Integrate release velocity over real frame time and decay it exponentially. Each small displacement
        // is still routed through OnWheel, so all existing surfaces/bounds retain their normal behavior.
        bool AdvanceTouchpadFling(float frameDt)
        {
            if (!tpFlingActive)
                return false;
            const ULONGLONG now = GetTickCount64();
            float dt = frameDt;
            if (tpFlingLastTick)
                dt = std::min(32.f, std::max(1.f, (float)(now - tpFlingLastTick)));
            else
                dt = std::min(32.f, std::max(1.f, dt));
            tpFlingLastTick = now;

            const float displacementDip = tpFlingVelocity * dt;
            tpFlingWheelRemainder += displacementDip * ((float)WHEEL_DELTA / kWheelPx);
            const int wheelDelta = (int)tpFlingWheelRemainder;
            tpFlingWheelRemainder -= (float)wheelDelta;
            if (wheelDelta)
                OnWheel(wheelDelta, tpFlingX, tpFlingY, tpFlingHorizontal);

            // A 245 ms velocity time constant gives a natural glide without an abrupt stop or an endless tail.
            tpFlingVelocity *= std::exp(-dt / 245.f);
            if (now - tpFlingStarted >= 1250 || std::fabs(tpFlingVelocity) < 0.018f)
            {
                StopTouchpadFling();
                return wheelDelta != 0;
            }
            return true;
        }

        void OnTouchpadScroll(float dx, float dy, float x, float y) override
        {
            StopTouchpadFling();                           // a new finger movement always takes control from the old fling
            const float k = (float)WHEEL_DELTA / kWheelPx; // DIP -> wheel units
            if (y < 47.f)
            { // over the tab bar: horizontal movement scrolls the tabs
                tpRemX += -dx * k;
                const int d = (int)tpRemX;
                tpRemX -= (float)d;
                if (d)
                    OnWheel(d, x, y, true);
            }
            else
            {
                tpRemY += dy * k;
                const int d = (int)tpRemY;
                tpRemY -= (float)d;
                if (d)
                    OnWheel(d, x, y, false);
            }
            KickScroll();
        }
        void OnTouchpadScrollEnd(float vx, float vy, float x, float y) override
        {
            tpRemX = tpRemY = 0.f;
            StopTouchpadFling();
            const bool bar = y < 47.f;
            // Convert centroid velocity to the wheel-delta direction used by OnWheel.
            const float v = bar ? -vx : vy;
            if (std::fabs(v) < 0.025f) // under ~7.5 DIPs of predicted travel: settle without a fling
                return;
            tpFlingActive = true;
            tpFlingHorizontal = bar;
            tpFlingVelocity = std::min(1.35f, std::max(-1.35f, v));
            tpFlingWheelRemainder = 0.f;
            tpFlingX = x;
            tpFlingY = y;
            tpFlingStarted = tpFlingLastTick = GetTickCount64();
            KickScroll();
        }

        // ---- touch / pen
        int PointerPanAxisAt(float, float y) const override
        {
            if (y < 48.f)
                return TabMax() > 0.5f ? 2 : 0;
            if (y >= ContentViewportBottom())
                return 0;
            if (tab == 1)
                return (y >= kLapRegionTop && LapMax() > 0.5f) ? 1 : 0;
            if (tab == 4 && noteView == 2)
                return 0;
            if (tab == 4 && noteView == 1)
                return (y >= 88.f && PreviewMax() > 0.5f) ? 1 : 0;
            return PanelScrollMax() > 0.5f ? 1 : 0;
        }

        void OnPointerPan(float dx, float dy, float, float) override
        {
            if (pointerPanAxis == 2)
            { // tab bar follows the finger / pen
                tabScrollA.Set(std::min(TabMax(), std::max(0.f, TabScrollNow() - dx)));
                Invalidate();
                return;
            }
            const ULONGLONG now = GetTickCount64();
            const float dtMs = panLastTick ? std::max(1.f, (float)(now - panLastTick)) : 16.f;
            panLastTick = now;
            panVelocity = 0.6f * panVelocity + 0.4f * (dy / dtMs);
            // Direct manipulation tracks the finger/pen 1:1; momentum is added when it is lifted.
            if (tab == 1 && pointerDownY >= kLapRegionTop)
            {
                lapSm.Jump(std::min(LapMax(), std::max(0.f, lapSm.pos - dy)));
                Invalidate();
                return;
            }
            if (tab == 4 && noteView == 1)
            {
                prevSm.Jump(std::min(PreviewMax(), std::max(0.f, prevSm.pos - dy)));
                Invalidate();
                return;
            }
            const float target = std::min(PanelScrollMax(), std::max(0.f, panelScroll - dy));
            panelScroll = panelScrollFrom = panelScrollTarget = target;
            panelScrollStarted = 0;
            Invalidate();
        }

        void OnPointerPanEnd() override
        {
            const ULONGLONG now = GetTickCount64();
            const bool fresh = panLastTick && now - panLastTick < 90; // a rested finger carries no momentum
            const float fling = fresh ? -panVelocity * 260.f : 0.f;
            panLastTick = 0;
            panVelocity = 0.f;
            if (pointerPanAxis != 1 || std::fabs(fling) < 8.f)
                return;
            if (tab == 1)
                lapSm.target = std::min(LapMax(), std::max(0.f, lapSm.pos + fling));
            else if (tab == 4 && noteView == 1)
                prevSm.target = std::min(PreviewMax(), std::max(0.f, prevSm.pos + fling));
            else
            {
                panelScrollTarget = panelScroll;
                ScrollTo(panelScroll + fling);
                return;
            }
            KickScroll();
        }

        void ResizePanelToContent()
        {
            // Historical name: content may change, but the flyout height never follows the selected page.
            if (!hwnd)
                return;
            snap = td::g_eng.Snapshot();
            const float maxScroll = PanelScrollMax();
            panelScroll = std::min(std::max(0.f, panelScroll), maxScroll);
            panelScrollTarget = std::min(std::max(0.f, panelScrollTarget), maxScroll);
            if (std::fabs(panelScrollTarget - panelScroll) < 0.1f)
            {
                panelScroll = panelScrollTarget;
                panelScrollStarted = 0;
                KillTimer(hwnd, TIMER_SCROLL_ANIM);
            }
            Invalidate();
        }

        // ---------------------------------------------------------------- open / close animation
        // Opacity of the panel. Normally always 1: the open/close animation is a slide, not a fade.
        void SetPanelFade(float a)
        {
            currentFade = a;
            if (alphaTarget)
            {
                masterOpacity = Clamp01(a);
                Invalidate();
            }
            else
            {
                masterOpacity = 1.f;
                if (hwnd)
                    SetLayeredWindowAttributes(hwnd, 0, (BYTE)std::lround((float)restingAlpha * Clamp01(a)), LWA_ALPHA);
            }
        }

        // The panel slides up from behind the taskbar and back down again, exactly like the native flyouts.
        // There is deliberately no opacity fade: fading the painted surface over a system-owned acrylic
        // backdrop is what made the panel look whitish for the first and last instants.
        // While sliding, the window is clipped at the top edge of the taskbar with a window region
        // (rounded at the top, flat at the taskbar line); the region is removed once fully visible.
        void ApplySlideClip(int top)
        {
            if (!hwnd)
                return;
            const int w = panelAnimationWidth, h = panelAnimationHeight;
            const int clipY = slideClipY > 0 ? slideClipY : panelWorkBottom;
            const int vis = clipY - top;
            if (clipY <= 0 || vis >= h)
            {
                if (slideClipped && SetWindowRgn(hwnd, nullptr, TRUE))
                    slideClipped = false;
                return;
            }
            const int r = (int)std::lround(8.f * Scale()) * 2;
            HRGN full = CreateRoundRectRgn(0, 0, w + 1, h + 1, r, r);
            if (!full)
                return;
            // Never an empty region: DWM stops tracking a window with nothing visible, and a system backdrop (acrylic)
            // set up in that state comes back flat. A thin sliver always stays visible instead.
            HRGN cut = CreateRectRgn(0, 0, w + 1, std::max(1, vis));
            if (!cut)
            {
                DeleteObject(full);
                return;
            }
            if (CombineRgn(full, full, cut, RGN_AND) == ERROR)
            {
                DeleteObject(cut);
                DeleteObject(full);
                return;
            }
            DeleteObject(cut);
            if (!SetWindowRgn(hwnd, full, TRUE))
            { // on failure, ownership stays with us; retain the previous region and state
                DeleteObject(full);
                return;
            }
            slideClipped = true; // the system owns `full`; redraw each newly exposed strip
        }

        void EndTimerPeriod()
        {
            if (timerPeriodActive)
            {
                timeEndPeriod(1);
                timerPeriodActive = false;
            }
        }

        // Finds the bottom-docked taskbar of the monitor. The panel is kept just below it in z-order while sliding, so
        // a frame of region lag can never paint over the bar. Returns false for a non-bottom or auto-hidden bar.
        bool FindSlideTaskbar(HMONITOR mon, const MONITORINFO &mi)
        {
            slideTaskbar = nullptr;
            slideUnder = false;
            slideClipY = 0;
            const int monW = mi.rcMonitor.right - mi.rcMonitor.left, monH = mi.rcMonitor.bottom - mi.rcMonitor.top;
            auto consider = [&](HWND t)
            {
                RECT r{};
                if (!t || slideTaskbar || !IsWindowVisible(t) || !GetWindowRect(t, &r))
                    return;
                if (MonitorFromWindow(t, MONITOR_DEFAULTTONULL) != mon)
                    return;
                const int th = r.bottom - r.top;
                if (th < 24 || r.top < mi.rcMonitor.top + monH / 2 || r.top > mi.rcMonitor.bottom - 16 ||
                    r.bottom < mi.rcMonitor.bottom - 2 || (r.right - r.left) < monW / 2)
                    return;
                slideTaskbar = t;
                slideClipY = r.top; // the panel is cut exactly at the top edge of the bar
            };
            consider(FindWindowW(L"Shell_TrayWnd", nullptr));
            for (HWND t = nullptr; !slideTaskbar && (t = FindWindowExW(nullptr, t, L"Shell_SecondaryTrayWnd", nullptr));)
                consider(t);
            slideUnder = slideTaskbar != nullptr;
            if (!slideUnder)
                slideClipY = panelWorkBottom;
            return slideUnder;
        }

        HWND SlideZ()
        {
            if (slideUnder && !IsWindow(slideTaskbar))
                slideUnder = false;
            return slideUnder ? slideTaskbar : HWND_TOPMOST;
        }

        // A visible-but-nearly-fully-clipped window is the resting line of the slide: a few pixels stay visible so DWM
        // keeps the window (and its acrylic) alive.
        int SlideHiddenY() const
        {
            const int clipY = slideClipY > 0 ? slideClipY : panelWorkBottom;
            return clipY - std::max(2, (int)std::lround(3.f * Scale()));
        }

        // The same sequence as switching the panel material to Simple and back to Acrylic in Settings, which is the
        // one thing that reliably produced a live acrylic panel.
        void WarmUpMaterial()
        {
            if (!hwnd)
                return;
            alphaTarget = false;
            ReleaseTarget();
            restingAlpha = ConfigureComposition(hwnd, pal, 1);
            SetPanelFade(1.f);
            Render();
            DwmFlush();
            alphaTarget = true;
            ReleaseTarget();
            restingAlpha = ConfigureComposition(hwnd, pal, 2);
            SetPanelFade(1.f);
            Render();
            RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE | RDW_ALLCHILDREN);
            DwmFlush();
            DwmFlush();
        }

        void BeginOpenAnimation(int x, int y, int w, int h, bool noAct)
        {
            if (!hwnd)
                return;
            KillTimer(hwnd, TIMER_PANEL_ANIM);
            const bool wasVisible = IsWindowVisible(hwnd) != FALSE;
            int startY = SlideHiddenY();
            if (wasVisible && panelAnimation == PanelAnimationState::Closing)
            { // re-opened while sliding down: continue from where it is
                RECT cur{};
                GetWindowRect(hwnd, &cur);
                startY = cur.top;
            }

            // Resize and render while hidden, before applying the slide clip. This primes the
            // Direct2D surface at its final size so the first visible frame never exposes a blank backdrop.
            if (!wasVisible)
            {
                SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
                restingAlpha = ConfigureComposition(hwnd, pal, compMode);
                Render();
                RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE | RDW_ALLCHILDREN);
            }

            panelAnimation = PanelAnimationState::Opening;
            panelAnimationX = x;
            panelAnimationWidth = w;
            panelAnimationHeight = h;
            panelAnimationStartY = startY;
            panelAnimationEndY = y;
            startFade = endFade = 1.f;
            SetPanelFade(1.f);
            ApplySlideClip(startY); // only a thin sliver is visible
            // Beneath the taskbar in z-order and not activated yet: activating a topmost window would lift it over
            // the taskbar. It is activated once the slide has finished.
            activateAfterOpen = slideUnder && !noAct;
            // The acrylic backdrop only ever came up correctly when the material was switched on a window DWM was
            // already composing (Settings > Simple > Acrylic). Hiding the panel drops that state, so every open
            // replays exactly that switch. The window is cloaked meanwhile: composed by DWM, never seen.
            const bool warm = !wasVisible && compMode == 2;
            bool cloaked = false;
            if (warm)
            {
                const BOOL on = TRUE; // DWMWA_CLOAK
                cloaked = SUCCEEDED(DwmSetWindowAttribute(hwnd, 13, &on, sizeof(on)));
            }
            SetWindowPos(hwnd, SlideZ(), x, startY, w, h, SWP_SHOWWINDOW | SWP_NOACTIVATE);
            if (warm)
                WarmUpMaterial();
            if (cloaked)
            {
                const BOOL off = FALSE;
                DwmSetWindowAttribute(hwnd, 13, &off, sizeof(off));
            }
            restingAlpha = ConfigureComposition(hwnd, pal, compMode);
            SetPanelFade(1.f);
            Render();
            RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE | RDW_ALLCHILDREN);
            DwmFlush();
            panelAnimationStarted = GetTickCount64();
            SetTimer(hwnd, TIMER_PANEL_ANIM, (UINT)kFrameMs, nullptr);
            if (!noAct && !slideUnder)
                SetForegroundWindow(hwnd);
        }

        void BeginCloseAnimation()
        {
            StopTouchpadFling();
            if (!hwnd || !IsWindowVisible(hwnd))
            {
                noActMode = false;
                return;
            }
            if (panelAnimation == PanelAnimationState::Closing)
                return;
            KillTimer(hwnd, TIMER_PANEL_ANIM);
            RECT r{};
            GetWindowRect(hwnd, &r);
            panelAnimation = PanelAnimationState::Closing;
            panelAnimationStarted = GetTickCount64();
            panelAnimationX = r.left;
            panelAnimationWidth = r.right - r.left;
            panelAnimationHeight = r.bottom - r.top;
            panelAnimationStartY = r.top;
            panelAnimationEndY = std::max(SlideHiddenY(), (int)r.top);
            if (slideUnder) // sink beneath the taskbar first (no visible change: the panel rests above the bar)
                SetWindowPos(hwnd, slideTaskbar, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            activateAfterOpen = false;
            startFade = endFade = 1.f;
            noActMode = false;
            SetTimer(hwnd, TIMER_PANEL_ANIM, (UINT)kFrameMs, nullptr);
        }

        void AdvancePanelAnimation()
        {
            if (!hwnd || panelAnimation == PanelAnimationState::Idle)
            {
                if (hwnd)
                    KillTimer(hwnd, TIMER_PANEL_ANIM);
                return;
            }
            const bool opening = panelAnimation == PanelAnimationState::Opening;
            DwmFlush(); // step in time with the compositor so the motion is even
            const float t = Clamp01((float)(GetTickCount64() - panelAnimationStarted) / (opening ? kSlideOpenMs : kSlideCloseMs));
            const float eased = opening ? EaseOutQuart(t) : EaseInCubic(t);
            const int y = panelAnimationStartY + (int)std::lround((panelAnimationEndY - panelAnimationStartY) * eased);
            HWND z = SlideZ();
            if (opening)
            { // moving up: widen the visible part after the move, never before it
                SetWindowPos(hwnd, z, panelAnimationX, y, panelAnimationWidth, panelAnimationHeight,
                             SWP_NOACTIVATE | SWP_NOSENDCHANGING);
                ApplySlideClip(y);
            }
            else
            { // moving down: shrink the visible part before the move
                ApplySlideClip(y);
                SetWindowPos(hwnd, z, panelAnimationX, y, panelAnimationWidth, panelAnimationHeight,
                             SWP_NOACTIVATE | SWP_NOSENDCHANGING);
            }
            if (t >= 1.f)
            {
                KillTimer(hwnd, TIMER_PANEL_ANIM);
                panelAnimation = PanelAnimationState::Idle;
                if (!opening)
                {
                    ShowWindow(hwnd, SW_HIDE);
                    if (slideClipped)
                    {
                        SetWindowRgn(hwnd, nullptr, TRUE);
                        slideClipped = false;
                    }
                    SetPanelFade(1.f);
                    RemoveWheelHook();
                    ReleaseTouchpadCapability(); // hidden: give the touchpad routing back
                    EndTimerPeriod();
                }
                else
                {
                    SetPanelFade(1.f);
                    ApplySlideClip(panelAnimationEndY); // fully visible: removes the region
                    SetWindowPos(hwnd, HWND_TOPMOST, panelAnimationX, panelAnimationEndY, panelAnimationWidth,
                                 panelAnimationHeight, SWP_NOACTIVATE | SWP_NOSENDCHANGING);
                    if (activateAfterOpen)
                    { // now clear of the taskbar: safe to take the foreground (and the top of the topmost band)
                        activateAfterOpen = false;
                        SetForegroundWindow(hwnd);
                    }
                    restingAlpha = ConfigureComposition(hwnd, pal, compMode);
                    Render();
                    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE | RDW_ALLCHILDREN);
                    DwmFlush();
                }
            }
        }

        // ---------------------------------------------------------------- show / hide
        void ShowPanel(bool noAct)
        {
            POINT cur{};
            GetCursorPos(&cur);
            HMONITOR mon = MonitorFromPoint(cur, MONITOR_DEFAULTTOPRIMARY);
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            if (!mon || !GetMonitorInfoW(mon, &mi))
                return;
            if (!IsWindowVisible(hwnd)) // a hidden window is parked on the target monitor to learn its DPI
                SetWindowPos(hwnd, nullptr, mi.rcWork.left + 20, mi.rcWork.top + 20, 50, 50, SWP_NOZORDER | SWP_NOACTIVATE);
            UINT nd = GetDpiForWindow(hwnd);
            if (nd && nd != dpi)
            {
                dpi = nd;
                ReleaseTarget();
            }
            currentFade = 1.f;
            StopTouchpadFling();
            ApplyComposition();
            snap = td::g_eng.Snapshot();

            SYSTEMTIME now;
            GetLocalTime(&now);
            calView = 0;
            calYear = now.wYear;
            calMonth = now.wMonth;
            calSelY = now.wYear;
            calSelM = now.wMonth;
            calSelD = now.wDay;
            alarmEditing = false;
            lapSm.Jump(0.f);
            prevSm.Jump(0.f);
            edSm.Jump(0.f);
            CalResetSlide();
            panVelocity = 0.f;
            panLastTick = 0;
            if (noteView == 2)
                noteView = 1; // reopen on the preview, never mid-edit
            docValid = false;
            noteDelArmedUntil = 0;
            InstallWheelHook();
            panelScroll = panelScrollFrom = panelScrollTarget = 0.f;
            panelScrollStarted = 0;
            KillTimer(hwnd, TIMER_SCROLL_ANIM);
            hoverId = hoverParam = 0;
            hoverTracks.clear();
            InitTabAnim();
            noActMode = noAct;
            shownAt = GetTickCount64();
            if (tickMs != 100)
            { // open: back to the fast tick right away
                tickMs = 100;
                SetTimer(hwnd, TIMER_TICK, 100, nullptr);
            }
            panelWorkBottom = (int)mi.rcWork.bottom;
            if (!timerPeriodActive)
            { // finer timer resolution while the panel is open keeps scrolling and sliding even
                timeBeginPeriod(1);
                timerPeriodActive = true;
            }

            const float scale = Scale();
            const float workH = (float)(mi.rcWork.bottom - mi.rcWork.top) / scale;
            panelH = std::min(kPanelH, std::max(140.f, workH - 24.f));
            const int w = (int)std::lround(kPanelW * scale), h = (int)std::lround(panelH * scale);
            const int margin = (int)std::lround(12.f * scale);
            const int x = std::max(static_cast<int>(mi.rcWork.left), static_cast<int>(mi.rcWork.right) - w - margin);
            const int y = std::max(static_cast<int>(mi.rcWork.top), static_cast<int>(mi.rcWork.bottom) - h - margin);
            if (!(IsWindowVisible(hwnd) && panelAnimation == PanelAnimationState::Closing))
                FindSlideTaskbar(mon, mi); // a panel re-opened mid-close keeps the taskbar it is already sliding behind
            BeginOpenAnimation(x, y, w, h, noAct);
            RouteTouchpadTo(hwnd); // the panel is on screen now: take the touchpad registration
        }

        void HidePanel()
        {
            FlushNote();
            StopPreviewSound();
            BeginCloseAnimation();
        }

        void Toggle(bool noAct)
        {
            if (IsWindowVisible(hwnd) && panelAnimation != PanelAnimationState::Closing)
                HidePanel();
            else
                ShowPanel(noAct);
        }

        void StopPreviewSound()
        {
            if (!snd::g_ringing)
                snd::Stop();
        }
        void PreviewSound(int sound)
        {
            if (!snd::g_ringing)
                snd::Play(sound, false);
        }

        // ---------------------------------------------------------------- tick
        void AddAlertFor(const td::RingEvent &e);                  // defined below
        void AddMissedAlert(const std::vector<td::MissedItem> &m); // defined below

        UINT tickMs = 100; // current interval of TIMER_TICK

        // While the panel is open it needs the fast tick (animations, touchpad routing). While it is hidden the
        // tick only has to be there when the next timer or alarm is due (and at least once a second).
        void UpdateTickInterval()
        {
            UINT want = 100;
            if (!IsWindowVisible(hwnd))
            {
                const long long d = td::g_eng.MsUntilNextDue();
                want = d < 0 ? 1000u : (UINT)std::min<long long>(1000, std::max<long long>(15, d));
            }
            if (want != tickMs)
            {
                tickMs = want;
                SetTimer(hwnd, TIMER_TICK, want, nullptr);
            }
        }

        void OnTick()
        {
            auto evs = td::g_eng.Tick();
            for (auto &e : evs)
                AddAlertFor(e);
            {
                auto missedNow = td::g_eng.TakeMissed(); // one popup for everything that was missed
                if (!missedNow.empty())
                    AddMissedAlert(missedNow);
            }
            UpdateTickInterval();
            if (!IsWindowVisible(hwnd))
                return;
            {
                POINT cp{};
                if (GetCursorPos(&cp))
                {
                    HWND at = WindowFromPoint(cp);
                    if (at && edits[E_NBODY] && at == edits[E_NBODY] && IsWindowVisible(at))
                        RouteTouchpadTo(at);
                    else if (at == hwnd)
                        RouteTouchpadTo(hwnd);
                }
            }
            if (!evs.empty())
                ResizePanelToContent();
            ++tickCount;
            if (noActMode)
            {
                HWND fg = GetForegroundWindow();
                if (fg == hwnd || (fg && IsChild(hwnd, fg)))
                    noActMode = false;
                else if (GetTickCount64() - shownAt > 800 && !IsShellFlyoutWindow(fg))
                {
                    HidePanel();
                    return;
                }
            }
            const ULONGLONG now = GetTickCount64();
            if (!statusMsg.empty() && now >= statusUntil)
            {
                statusMsg.clear();
                statusStarted = 0;
                statusTimerActive = false;
                KillTimer(hwnd, TIMER_STATUS_ANIM);
                ResizePanelToContent();
            }
            else if (!statusMsg.empty() && statusUntil - now <= 280 && !statusTimerActive)
            {
                SetTimer(hwnd, TIMER_STATUS_ANIM, 15, nullptr);
                statusTimerActive = true;
            }
            if (noteDelArmedUntil && now >= noteDelArmedUntil)
            {
                noteDelArmedUntil = 0;
                noteDelArmedId = 0;
                Invalidate();
            }
            if (tab == 1 || tab == 2 || tab == 3 || !statusMsg.empty() || tickCount % 10 == 0)
                Invalidate();
        }

        // ---------------------------------------------------------------- edits
        int GetEditInt(int slot)
        {
            wchar_t b[16] = {};
            GetWindowTextW(edits[slot], b, 16);
            return (int)wcstol(b, nullptr, 10);
        }
        void SetEditInt(int slot, int v) { SetWindowTextW(edits[slot], Two(v).c_str()); }
        void StepEdit(int slot, int delta, int lo, int hi)
        {
            int v = GetEditInt(slot) + delta;
            if (v > hi)
                v = lo;
            if (v < lo)
                v = hi;
            SetEditInt(slot, v);
            Invalidate();
        }
        void UseEdit(int slot, D2D1_RECT_F field, float padX, float padY)
        {
            const bool focus = GetFocus() == edits[slot];
            RoundFill(field, 6.f, pal.field);
            RoundStroke(field, 6.f, focus ? pal.accent : pal.border, focus ? 2.f : 1.f);
            editRect[slot] = Inset(field, padX, padY);
            editWant[slot] = true;
        }
        void ApplyEdits()
        {
            const float scale = Scale();
            const float offset = SnapPx(ContentTransitionOffset() - panelScroll);
            const float viewportTop = 48.f, viewportBottom = ContentViewportBottom();
            const bool hold = false; // the panel slides instead of fading, so native children can always be shown
            for (int i = 0; i < E_COUNT; ++i)
            {
                if (!edits[i])
                    continue;
                if (editWant[i] && !hold)
                {
                    D2D1_RECT_F rr = editRect[i];
                    rr.top += offset;
                    rr.bottom += offset;
                    if (rr.top < viewportTop || rr.bottom > viewportBottom)
                    {
                        if (IsWindowVisible(edits[i]))
                            ShowWindow(edits[i], SW_HIDE);
                        continue;
                    }
                    RECT px{(LONG)std::lround(rr.left * scale), (LONG)std::lround(rr.top * scale),
                            (LONG)std::lround(RW(rr) * scale), (LONG)std::lround(RH(rr) * scale)}; // x, y, w, h
                    if (!IsWindowVisible(edits[i]) || memcmp(&px, &editLast[i], sizeof(RECT)) != 0)
                    {
                        SetWindowPos(edits[i], HWND_TOP, px.left, px.top, px.right, px.bottom, SWP_NOACTIVATE | SWP_SHOWWINDOW);
                        editLast[i] = px;
                    }
                    if (pendingFocus == i && IsWindowVisible(edits[i]))
                    {
                        SetFocus(edits[i]);
                        pendingFocus = -1;
                    }
                }
                else if (IsWindowVisible(edits[i]))
                {
                    ShowWindow(edits[i], SW_HIDE);
                }
            }
        }

        float HitTestY(float y) const override
        {
            if (y < 48.f)
                return y; // the tab strip stays in screen coordinates; only the page scrolls
            if (y >= ContentViewportBottom())
                return 1000000.f;
            return y + panelScroll - ContentTransitionOffset();
        }

        // ---------------------------------------------------------------- painting
        // Whole device pixels: sub-pixel translation makes text shimmer while content moves.
        float SnapPx(float dip) const
        {
            const float sc = Scale();
            return std::round(dip * sc) / sc;
        }

        void Paint() override
        {
            snap = td::g_eng.Snapshot();
            StepScrollers();
            for (int i = 0; i < E_COUNT; ++i)
                editWant[i] = false;
            drawOpacity = 1.f;
            PaintTabs();

            // The page is clipped independently from the fixed tab strip and status line.
            const float viewportBottom = ContentViewportBottom();
            D2D1_MATRIX_3X2_F originalTransform{};
            rt->GetTransform(&originalTransform);
            rt->PushAxisAlignedClip(RF(0, 47.f, kPanelW, std::max(1.f, viewportBottom - 47.f)), D2D1_ANTIALIAS_MODE_ALIASED);
            D2D1_MATRIX_3X2_F contentTransform = originalTransform;
            contentTransform._32 += SnapPx(ContentTransitionOffset() - panelScroll);
            rt->SetTransform(&contentTransform);
            drawOpacity = ContentTransitionOpacity();
            switch (tab)
            {
            case 0:
                PaintCalendar();
                break;
            case 1:
                PaintStopwatch();
                break;
            case 2:
                PaintTimer();
                break;
            case 3:
                PaintAlarm();
                break;
            case 4:
                PaintNotes();
                break;
            default:
                PaintSettings();
                break;
            }
            rt->SetTransform(&originalTransform);
            rt->PopAxisAlignedClip();
            drawOpacity = 1.f;

            const float maxScroll = PanelScrollMax();
            if (maxScroll > 0.5f)
            { // quiet scroll thumb
                const float trackTop = 54.f, trackH = std::max(12.f, viewportBottom - 6.f - trackTop);
                const float thumbH = std::min(trackH, std::max(24.f, trackH * ((viewportBottom - 48.f) / (ContentBottomEstimate() - 48.f))));
                const float thumbY = trackTop + (trackH - thumbH) * (panelScroll / maxScroll);
                RoundFill(RF(kPanelW - 6.f, trackTop, 2.f, trackH), 1.f, pal.border);
                RoundFill(RF(kPanelW - 6.f, thumbY, 2.f, thumbH), 1.f, pal.sub);
            }
            if (!statusMsg.empty() && tab != 0)
                Text(statusMsg, RF(16, panelH - 26, kPanelW - 32, 18), 11.5f, 400, Fade(pal.sub, StatusOpacity()), 1, 1);
            ApplyEdits();
        }

        void PaintTabs()
        {
            const float sc = TabScrollNow();
            rt->PushAxisAlignedClip(RF(0, 0, kPanelW, kTabH), D2D1_ANTIALIAS_MODE_ALIASED);
            for (int i = 0; i < kTabs; ++i)
            {
                D2D1_RECT_F r = RF(tabX[i] - sc + 3, 5, tabW[i] - 6, 34);
                const bool sel = tab == i;
                const float hov = HoverAmount(ID_TAB, i);
                if (!sel && hov > 0.001f)
                    RoundFill(r, 6.f, Fade(pal.cardHover, hov));
                Text(kTabNames[i], r, 12.5f, sel ? 600 : 400, sel ? pal.accent : pal.text, 1, 1);
                D2D1_RECT_F h = r; // only the visible part of a partly scrolled tab is clickable
                h.left = std::max(h.left, 0.f);
                h.right = std::min(h.right, kPanelW);
                if (h.right > h.left)
                    AddHit(h, ID_TAB, i);
            }
            RoundFill(RF(indXA.Value(kTabAnimMs, false) - sc, 40, indWA.Value(kTabAnimMs, false), 3), 1.5f, pal.accent);
            rt->PopAxisAlignedClip();
            Line(0, 46.5f, kPanelW, 46.5f, pal.border, 1.f);
        }

        // ---- calendar --------------------------------------------------------------------------
        // One page of the calendar, drawn `shift` px below its resting place so pages can slide.
        // view 0: month grid of (year, month); view 1: the months of `year`; view 2: the 12 years starting at ybase - 1.
        void PaintCalPage(int view, int year, int month, int ybase, float shift, const SYSTEMTIME &now, float clipTop, float clipBottom)
        {
            if (view == 0)
            {
                const int fdow = FirstDayOfWeekSun();
                const int offset = (WeekdayOf(year, month, 1) - fdow + 7) % 7;
                const int dim = DaysInMonth(year, month);
                const int pm = month == 1 ? 12 : month - 1, py = month == 1 ? year - 1 : year;
                const int nm = month == 12 ? 1 : month + 1, ny = month == 12 ? year + 1 : year;
                const int pdim = DaysInMonth(py, pm);
                for (int k = 0; k < 42; ++k)
                {
                    int d = 1 - offset + k, y = year, m = month;
                    bool inMonth = true;
                    if (d < 1)
                    {
                        d += pdim;
                        y = py;
                        m = pm;
                        inMonth = false;
                    }
                    else if (d > dim)
                    {
                        d -= dim;
                        y = ny;
                        m = nm;
                        inMonth = false;
                    }
                    const float cx = 18.f + (k % 7) * 48 + 24, cy = 158.f + (k / 7) * 44 + 22 + shift;
                    if (cy < clipTop - 22.f || cy > clipBottom + 22.f)
                        continue; // entirely outside the visible grid window
                    const int code = y * 10000 + m * 100 + d;
                    const bool isToday = (y == now.wYear && m == now.wMonth && d == now.wDay);
                    const bool isSel = (y == calSelY && m == calSelM && d == calSelD);
                    const float hov = HoverAmount(ID_CAL_CELL, code);
                    Col fg = inMonth ? pal.text : Fade(pal.sub, 0.55f);
                    if (isToday)
                    {
                        Circle(cx, cy, 18.f, pal.accent);
                        fg = pal.onAccent;
                    }
                    else if (hov > 0.001f)
                        Circle(cx, cy, 18.f, Fade(pal.cardHover, hov));
                    if (isSel && !isToday)
                        CircleStroke(cx, cy, 17.f, pal.accent, 2.f);
                    Text(std::to_wstring(d), RF(cx - 24, cy - 18, 48, 36), 13.5f, isToday ? 600 : 400, fg, 1, 1);
                    if (cy > clipTop && cy < clipBottom)
                        AddHit(RF(cx - 24, cy - 22, 48, 44), ID_CAL_CELL, code);
                }
                return;
            }
            // 4 x 3 grid: 79-wide cells with 8 gaps fill the 340 content width exactly.
            for (int i = 0; i < 12; ++i)
            {
                const int c = i % 4, r = i / 4;
                D2D1_RECT_F cell = RF(16.f + c * 87, 150.f + r * 84 + shift, 79, 76);
                if (cell.bottom < clipTop || cell.top > clipBottom)
                    continue;
                std::wstring label;
                bool cur, dim = false;
                int param;
                if (view == 1)
                {
                    SYSTEMTIME st{};
                    st.wYear = (WORD)year;
                    st.wMonth = (WORD)(i + 1);
                    st.wDay = 1;
                    label = DateStr(st, L"MMM");
                    cur = (year == now.wYear && i + 1 == now.wMonth);
                    param = i + 1;
                }
                else
                {
                    param = ybase - 1 + i;
                    label = std::to_wstring(param);
                    cur = (param == now.wYear);
                    dim = (param < ybase || param > ybase + 9);
                }
                const float hov = HoverAmount(ID_CAL_CELL, param);
                RoundFill(cell, 8.f, cur ? pal.accent : Mix(pal.card, pal.cardHover, hov));
                if (!cur)
                    RoundStroke(cell, 8.f, pal.border, 1.f);
                Text(label, cell, 14.f, 600, cur ? pal.onAccent : (dim ? pal.sub : pal.text), 1, 1);
                const float mid = (cell.top + cell.bottom) * 0.5f;
                if (mid > clipTop && mid < clipBottom)
                    AddHit(cell, ID_CAL_CELL, param);
            }
        }

        void PaintCalendar()
        {
            SYSTEMTIME now;
            GetLocalTime(&now);
            Text(DateStr(now, L"dddd, MMMM d"), RF(16, 56, 340, 32), 20.f, 600, pal.text, 0, 1);

            std::wstring title;
            if (calView == 0)
            {
                SYSTEMTIME st{};
                st.wYear = (WORD)calYear;
                st.wMonth = (WORD)calMonth;
                st.wDay = 1;
                title = DateStr(st, L"MMMM yyyy");
            }
            else if (calView == 1)
                title = std::to_wstring(calYear);
            else
                title = std::to_wstring(yearBase) + L" \u2013 " + std::to_wstring(yearBase + 9);
            D2D1_RECT_F hdr = RF(10, 94, 230, 34);
            const float hh = HoverAmount(ID_CAL_HDR);
            if (hh > 0.001f)
                RoundFill(hdr, 6.f, Fade(pal.cardHover, hh));
            Text(title, Inset(hdr, 8, 0), 15.f, 600, pal.text, 0, 1);
            AddHit(hdr, ID_CAL_HDR);
            FlatIcon(RF(286, 94, 34, 34), ID_CAL_PREV, 0, L"\uE70E");
            FlatIcon(RF(322, 94, 34, 34), ID_CAL_NEXT, 0, L"\uE70D");

            if (calView == 0)
            {
                const int fdow = FirstDayOfWeekSun();
                for (int c = 0; c < 7; ++c)
                {
                    std::wstring n = WeekdayName((fdow + c) % 7, L"ddd");
                    if (n.size() > 2)
                        n.resize(2);
                    Text(n, RF(18.f + c * 48, 132, 48, 22), 12.f, 400, pal.sub, 1, 1);
                }
            }

            // Pages above / below the committed one slide through a clipped window, like the Windows calendar.
            const float H = CalGridH();
            const float clipTop = calView == 0 ? 156.f : 146.f, clipBottom = clipTop + H + (calView == 0 ? 2.f : 8.f);
            const float off = SnapPx(calSm.pos);
            rt->PushAxisAlignedClip(RF(0, clipTop, kPanelW, clipBottom - clipTop), D2D1_ANTIALIAS_MODE_ALIASED);
            for (int k = -1; k <= 1; ++k)
            {
                int y = calYear, m = calMonth, yb = yearBase;
                if (calView == 0)
                {
                    m += k;
                    if (m < 1)
                    {
                        m = 12;
                        --y;
                    }
                    if (m > 12)
                    {
                        m = 1;
                        ++y;
                    }
                    if (y < 1601 || y > 9998)
                        continue;
                }
                else if (calView == 1)
                {
                    y += k;
                    if (y < 1601 || y > 9998)
                        continue;
                }
                else
                {
                    yb += k * 10;
                    if (yb < 1600 || yb > 9990)
                        continue;
                }
                PaintCalPage(calView, y, m, yb, (float)k * H - off, now, clipTop, clipBottom);
            }
            rt->PopAxisAlignedClip();
            Btn(RF(116, 436, 140, 34), ID_CAL_TODAY, 0, L"Go to today", 0, 13.f);
        }

        void CalClick(int id, int param)
        {
            SYSTEMTIME now;
            GetLocalTime(&now);
            if (id == ID_CAL_HDR)
            {
                CalResetSlide();
                if (calView == 0)
                    calView = 1;
                else if (calView == 1)
                {
                    calView = 2;
                    yearBase = calYear / 10 * 10;
                }
                else
                    calView = 0;
            }
            else if (id == ID_CAL_PREV || id == ID_CAL_NEXT)
            {
                const int dir = id == ID_CAL_NEXT ? 1 : -1;
                if (CalStepAllowed(dir))
                    CalSlide(dir); // slides one page; the month / year / decade commits mid-way
                return;
            }
            else if (id == ID_CAL_CELL)
            {
                if (calView != 0)
                    CalResetSlide();
                if (calView == 0)
                {
                    calSelY = param / 10000;
                    calSm.Jump(0.f);
                    calSelM = (param / 100) % 100;
                    calSelD = param % 100;
                    calYear = calSelY;
                    calMonth = calSelM;
                }
                else if (calView == 1)
                {
                    calMonth = param;
                    calView = 0;
                }
                else
                {
                    calYear = param;
                    calView = 1;
                }
            }
            else if (id == ID_CAL_TODAY)
            {
                CalResetSlide();
                calView = 0;
                calYear = now.wYear;
                calMonth = now.wMonth;
                calSelY = now.wYear;
                calSelM = now.wMonth;
                calSelD = now.wDay;
            }
            Invalidate(); // in-place update: no page slide/fade for every date click
        }

        // ---- stopwatch ---------------------------------------------------------------------------------
        void PaintStopwatch()
        {
            Text(td::FmtStopwatch(snap.swMs, true), RF(0, 66, kPanelW, 90), 58.f, 600, pal.text, 1, 1);
            const float cy = 206;
            IconBtn(kPanelW / 2 - 90, cy, 52, ID_SW_RESET, 0, L"\uE777", 0, 18.f);
            IconBtn(kPanelW / 2, cy, 70, ID_SW_TOGGLE, 0, snap.swRunning ? L"\uE769" : L"\uE768", 1, 24.f);
            IconBtn(kPanelW / 2 + 90, cy, 52, ID_SW_LAP, 0, L"\uE7C1", 0, 18.f);
            if (snap.laps.empty())
            {
                Text(L"Start the stopwatch, then press the flag to record laps.", RF(24, 276, kPanelW - 48, 40), 12.f, 400,
                     pal.sub, 1, 0, false, true);
                return;
            }
            Text(L"Laps", RF(20, kLapRegionTop + 2, 200, 24), 13.f, 600, pal.text, 0, 1);
            const int total = (int)snap.laps.size();
            const float off = SnapPx(lapSm.pos);
            const float viewTop = kLapTop - 4.f, viewBottom = kLapTop + LapViewH();
            rt->PushAxisAlignedClip(RF(0, viewTop, kPanelW, viewBottom - viewTop), D2D1_ANTIALIAS_MODE_ALIASED);
            for (int v = 0; v < total; ++v) // row v shows the v-th newest lap
            {
                const float top = kLapTop + v * kLapRow - off;
                if (top + kLapRow < viewTop)
                    continue;
                if (top > viewBottom)
                    break;
                const int idx = total - 1 - v;
                const long long tot = snap.laps[idx], split = tot - (idx > 0 ? snap.laps[idx - 1] : 0);
                D2D1_RECT_F r = RF(16, top, 340, kLapRow - 4);
                RoundFill(r, 6.f, pal.card);
                RoundStroke(r, 6.f, pal.border, 1.f);
                Text(L"Lap " + std::to_wstring(idx + 1), RF(30, r.top, 70, RH(r)), 13.f, 400, pal.sub, 0, 1);
                Text(td::FmtStopwatch(split, true), RF(100, r.top, 120, RH(r)), 14.f, 600, pal.text, 1, 1);
                Text(td::FmtStopwatch(tot, true), RF(220, r.top, 112, RH(r)), 13.f, 400, pal.sub, 2, 1);
            }
            rt->PopAxisAlignedClip();
        }

        // ---- timer -----------------------------------------------------------------------------------------
        void PaintTimer()
        {
            UseEdit(E_TNAME, RF(16, 58, 340, 36), 10, 7);
            static const wchar_t *labels[3] = {L"hours", L"min", L"sec"};
            for (int i = 0; i < 3; ++i)
            {
                const float bx = 28.f + i * 110; // 28 | 96 | 14 | 96 | 14 | 96 | 28
                FlatIcon(RF(bx, 102, 96, 24), ID_TM_STEP, i * 2 + 1, L"\uE70E");
                UseEdit(E_TH + i, RF(bx, 126, 96, 50), 4, 6);
                FlatIcon(RF(bx, 176, 96, 24), ID_TM_STEP, i * 2, L"\uE70D");
                Text(labels[i], RF(bx, 200, 96, 16), 11.f, 400, pal.sub, 1, 1);
            }
            std::vector<D2D1_RECT_F> rects;
            D2D1_RECT_F saveR{0, 0, 0, 0};
            const float pb = LayoutPresets(&rects, &saveR, 228.f);
            for (size_t i = 0; i < rects.size(); ++i)
            {
                Chip(rects[i], ID_TM_PRESET, (int)i, snap.presets[i].name, false);
                if (presetDeleteIndex == (int)i)
                    FlatIcon(RF(rects[i].right + 2.f, rects[i].top, 20.f, RH(rects[i])), ID_TM_PRESET_DELETE, (int)i, L"\uE711", 10.f);
            }
            if (saveR.right > saveR.left)
                Chip(saveR, ID_TM_SAVEPRESET, 0, L"+ Save", false);
            Text(L"Right-click a preset to show its remove button", RF(16, pb + 6, 340, 16), 10.5f, 400, pal.sub, 0, 0, false, true);
            float y = pb + 32;
            Btn(RF(16, y, 340, 40), ID_TM_START, 0, L"Start", 1, 15.f);
            y += 40 + 18;
            if (snap.timers.empty())
                return;
            Text(L"Running timers", RF(20, y, 200, 22), 13.f, 600, pal.text, 0, 1);
            y += 26;
            for (size_t idx = 0; idx < snap.timers.size(); ++idx)
            {
                const td::TimerView &t = snap.timers[idx];
                D2D1_RECT_F r = RF(16, y + (float)idx * 64, 340, 60);
                RoundFill(r, 8.f, pal.card);
                RoundStroke(r, 8.f, pal.border, 1.f);
                std::wstring sub = t.name.empty() ? std::wstring(L"Timer") : t.name;
                if (!t.running)
                    sub += L"  \u00B7  paused";
                else if (t.endWall)
                {
                    SYSTEMTIME e = td::WallToLocal(t.endWall);
                    sub += L"  \u00B7  ends " + td::FmtTimeOfDay(e.wHour, e.wMinute); // honours 12 h / 24 h
                }
                Text(sub, RF(30, r.top + 6, 200, 16), 11.5f, 400, pal.sub, 0, 1);
                Text(td::FmtClock((t.remainMs + 999) / 1000 * 1000), RF(30, r.top + 20, 190, 26), 21.f, 600, pal.text, 0, 1);
                Progress(RF(30, r.bottom - 11, 200, 3), t.totalMs > 0 ? 1.f - (float)t.remainMs / (float)t.totalMs : 0.f);
                const float mid = r.top + RH(r) / 2;
                IconBtn(286, mid, 32, ID_TM_TOGGLE, t.id, t.running ? L"\uE769" : L"\uE768", 0, 13.f);
                IconBtn(326, mid, 32, ID_TM_CANCEL, t.id, L"\uE711", 0, 12.f);
            }
        }

        void StartTimerFromEdits()
        {
            const int h = GetEditInt(E_TH), m = GetEditInt(E_TM), s = GetEditInt(E_TS);
            const long long ms = ((h * 60LL + m) * 60LL + s) * 1000LL;
            if (ms < 1000)
            {
                SetStatus(L"Set a time first");
                return;
            }
            wchar_t nm[64] = {};
            GetWindowTextW(edits[E_TNAME], nm, 64);
            td::g_eng.TimerStart(nm, ms);
            SetWindowTextW(edits[E_TNAME], L"");
            Invalidate();
        }

        void ApplyPreset(int idx)
        {
            if (idx < 0 || idx >= (int)snap.presets.size())
                return;
            const long long secs = snap.presets[idx].secs;
            SetEditInt(E_TH, (int)std::min(99LL, secs / 3600));
            SetEditInt(E_TM, (int)((secs / 60) % 60));
            SetEditInt(E_TS, (int)(secs % 60));
            const std::wstring &nm = snap.presets[idx].name;
            SetWindowTextW(edits[E_TNAME], nm == td::FmtDuration(secs * 1000) ? L"" : nm.c_str());
        }

        void SavePresetFromEdits()
        {
            const long long secs = GetEditInt(E_TH) * 3600LL + GetEditInt(E_TM) * 60LL + GetEditInt(E_TS);
            if (secs < 1)
            {
                SetStatus(L"Set a time first");
                return;
            }
            wchar_t nm[64] = {};
            GetWindowTextW(edits[E_TNAME], nm, 64);
            td::g_eng.PresetAdd(nm, secs);
            SetStatus(L"Preset saved");
        }

        // ---- alarm -----------------------------------------------------------------------------------------------
        static std::wstring DaysText(unsigned days)
        {
            if (days == 0)
                return L"Once";
            if (days == 0x7F)
                return L"Every day";
            std::wstring o;
            const int fdow = FirstDayOfWeekSun();
            for (int c = 0; c < 7; ++c)
            {
                const int d = (fdow + c) % 7;
                if (!(days & (1u << d)))
                    continue;
                if (!o.empty())
                    o += L", ";
                o += WeekdayName(d, L"ddd");
            }
            return o;
        }

        void OpenAlarmEditor(int id)
        {
            edit = td::AlarmItem();
            bool found = false;
            for (auto &a : snap.alarms)
                if (a.id == id)
                {
                    edit = a;
                    found = true;
                    break;
                }
            if (!found)
            {
                SYSTEMTIME now;
                GetLocalTime(&now);
                edit.id = 0;
                edit.hh = now.wHour;
                edit.mm = now.wMinute;
                edit.days = 0;
                edit.enabled = true;
                edit.snoozeMin = td::g_set.alarmSnooze.load(); // last used snooze
                edit.label.clear();
                edit.sound = td::g_set.alarmSound.load(); // last used / default sound
            }
            if (td::g_set.Use24())
                SetEditInt(E_AH, edit.hh);
            else
            {
                int h12 = edit.hh % 12;
                SetEditInt(E_AH, h12 == 0 ? 12 : h12);
                editPM = edit.hh >= 12;
            }
            SetEditInt(E_AM, edit.mm);
            SetWindowTextW(edits[E_ALABEL], edit.label.c_str());
            alarmEditing = true;
            panelScroll = panelScrollFrom = panelScrollTarget = 0.f;
            panelScrollStarted = 0;
            KillTimer(hwnd, TIMER_SCROLL_ANIM);
            ResizePanelToContent();
            StartContentTransition();
        }

        void CloseAlarmEditor()
        {
            alarmEditing = false;
            StopPreviewSound();
            panelScroll = panelScrollFrom = panelScrollTarget = 0.f;
            panelScrollStarted = 0;
            KillTimer(hwnd, TIMER_SCROLL_ANIM);
            ResizePanelToContent();
            StartContentTransition();
        }

        void SaveAlarmFromEditor()
        {
            const int v = GetEditInt(E_AH);
            if (td::g_set.Use24())
                edit.hh = std::min(23, std::max(0, v));
            else
                edit.hh = std::min(12, std::max(1, v)) % 12 + (editPM ? 12 : 0);
            edit.mm = std::min(59, std::max(0, GetEditInt(E_AM)));
            wchar_t nm[64] = {};
            GetWindowTextW(edits[E_ALABEL], nm, 64);
            edit.label = nm;
            edit.enabled = true;
            td::g_eng.AlarmSave(edit);
            // The tone and snooze just chosen become the defaults for the next alarm.
            td::g_set.alarmSound = std::min(td::kSoundCount - 1, std::max(0, edit.sound));
            td::g_set.alarmSnooze = std::min(60, std::max(1, edit.snoozeMin));
            td::g_set.Save();
            CloseAlarmEditor();
        }

        void PaintAlarm()
        {
            if (alarmEditing)
            {
                PaintAlarmEditor();
                return;
            }
            Text(L"Alarms", RF(16, 54, 200, 36), 20.f, 600, pal.text, 0, 1);
            IconBtn(kPanelW - 33, 72, 34, ID_AL_ADD, 0, L"\uE710", 1, 14.f);
            if (snap.alarms.empty())
            {
                Text(L"No alarms yet. Press + to add one.", RF(24, 140, kPanelW - 48, 30), 13.f, 400, pal.sub, 1, 1);
                return;
            }
            for (size_t idx = 0; idx < snap.alarms.size(); ++idx)
            {
                const td::AlarmItem &a = snap.alarms[idx];
                D2D1_RECT_F r = RF(16, 102 + (float)idx * 70, 340, 62);
                RoundFill(r, 8.f, Mix(pal.card, pal.cardHover, HoverAmount(ID_AL_ROW, a.id)));
                RoundStroke(r, 8.f, pal.border, 1.f);
                AddHit(r, ID_AL_ROW, a.id);
                Text(td::FmtTimeOfDay(a.hh, a.mm), RF(30, r.top + 5, 190, 32), 24.f, 600, a.enabled ? pal.text : pal.sub, 0, 1);
                const std::wstring sub = (a.label.empty() ? std::wstring() : a.label + L"  \u00B7  ") + DaysText(a.days);
                Text(sub, RF(30, r.top + 37, 220, 18), 11.5f, 400, pal.sub, 0, 1);
                FlatIcon(RF(258, r.top + 15, 32, 32), ID_AL_DEL, a.id, L"\uE74D", 14.f);
                UiWindow::Toggle(302, r.top + 21, ID_AL_TOGGLE, a.enabled, a.id);
            }
        }

        void PaintAlarmEditor()
        {
            const bool use24 = td::g_set.Use24();
            Text(edit.id ? L"Edit alarm" : L"New alarm", RF(16, 54, 300, 30), 20.f, 600, pal.text, 0, 1);

            // Hour / minute pickers, centred as a group (an AM/PM column joins them in 12 h mode).
            const float hx = use24 ? 88.f : 39.f, mx = hx + 108.f;
            for (int i = 0; i < 2; ++i)
            {
                const float bx = i == 0 ? hx : mx;
                FlatIcon(RF(bx, 90, 88, 24), ID_AL_STEP, i * 2 + 1, L"\uE70E");
                UseEdit(E_AH + i, RF(bx, 114, 88, 52), 4, 8);
                FlatIcon(RF(bx, 166, 88, 24), ID_AL_STEP, i * 2, L"\uE70D");
            }
            Text(L":", RF(hx + 88, 114, 20, 52), 28.f, 600, pal.text, 1, 1);
            Text(L"hours", RF(hx, 190, 88, 16), 11.f, 400, pal.sub, 1, 1);
            Text(L"min", RF(mx, 190, 88, 16), 11.f, 400, pal.sub, 1, 1);
            if (!use24)
            {
                Chip(RF(mx + 102, 114, 84, 24), ID_AL_PM, 0, L"AM", !editPM);
                Chip(RF(mx + 102, 142, 84, 24), ID_AL_PM, 1, L"PM", editPM);
            }
            UseEdit(E_ALABEL, RF(16, 216, 340, 36), 10, 7);

            const int fdow = FirstDayOfWeekSun();
            for (int c = 0; c < 7; ++c)
            {
                const int d = (fdow + c) % 7;
                std::wstring n = WeekdayName(d, L"ddd");
                if (n.size() > 3)
                    n.resize(3);
                Chip(RF(20.f + c * 48, 262, 44, 30), ID_AL_DAY, d, n, (edit.days & (1u << d)) != 0);
            }
            Text(L"No days selected: rings once", RF(16, 296, 340, 14), 10.5f, 400, pal.sub, 0, 1);

            // Snooze + sound rows share one card style; their controls are right-aligned to the card padding.
            auto card = [&](float y, const wchar_t *label)
            {
                D2D1_RECT_F c = RF(16, y, 340, 44);
                RoundFill(c, 8.f, pal.card);
                RoundStroke(c, 8.f, pal.border, 1.f);
                Text(label, RF(30, y, 90, 44), 14.f, 400, pal.text, 0, 1);
            };
            card(320, L"Snooze");
            FlatIcon(RF(212, 326, 32, 32), ID_AL_SNOOZE, 0, L"\uE70D", 12.f);
            Text(std::to_wstring(edit.snoozeMin) + L" min", RF(244, 320, 66, 44), 13.f, 600, pal.text, 1, 1);
            FlatIcon(RF(310, 326, 32, 32), ID_AL_SNOOZE, 1, L"\uE70E", 12.f);

            card(372, L"Sound");
            FlatIcon(RF(118, 378, 32, 32), ID_AL_SOUND, 0, L"\uE76B", 12.f);
            Text(td::kSoundNames[std::min(td::kSoundCount - 1, std::max(0, edit.sound))], RF(150, 372, 124, 44), 12.5f, 600, pal.text, 1, 1);
            FlatIcon(RF(274, 378, 32, 32), ID_AL_SOUND, 1, L"\uE76C", 12.f);
            IconBtn(326, 394, 32, ID_AL_SOUND_PLAY, 0, L"\uE768", 1, 12.f);

            Btn(RF(16, 428, 166, 40), ID_AL_SAVE, 0, L"Save", 1, 14.f);
            Btn(RF(190, 428, 166, 40), ID_AL_CANCEL, 0, L"Cancel", 0, 14.f);
        }

        // ---- notes: list, rich preview, editor ------------------------------------------------------------------------
        static std::wstring Snippet(const std::wstring &body)
        {
            size_t i = 0;
            while (i < body.size())
            {
                const size_t e = body.find_first_of(L"\r\n", i);
                std::wstring line = body.substr(i, e == std::wstring::npos ? std::wstring::npos : e - i);
                size_t k = 0;
                while (k < line.size() && wcschr(L" \t#>*-_`$|", line[k]))
                    ++k;
                line = line.substr(k);
                if (!line.empty())
                {
                    if (line.size() > 110)
                        line.resize(110);
                    return line;
                }
                if (e == std::wstring::npos)
                    break;
                i = e + 1;
            }
            return L"";
        }

        void DeleteIcon(D2D1_RECT_F r, int noteId)
        {
            const bool armed = noteDelArmedId == noteId && GetTickCount64() < noteDelArmedUntil;
            const float hov = HoverAmount(ID_NOTE_DEL, noteId);
            if (armed)
                RoundFill(r, 6.f, Mix(pal.danger, pal.dangerHover, hov));
            else if (hov > 0.001f)
                RoundFill(r, 6.f, Fade(pal.cardHover, hov));
            Text(L"\uE74D", r, 12.f, 400, armed ? Hex(0xFFFFFF) : pal.text, 1, 1, true);
            AddHit(r, ID_NOTE_DEL, noteId);
        }

        void PaintNotes()
        {
            if (noteView != 0 && !FindNote(curNote))
                noteView = 0;
            if (noteView == 1)
                PaintNotePreview();
            else if (noteView == 2)
                PaintNoteEditor();
            else
                PaintNoteList();
        }

        void PaintNoteList()
        {
            Text(L"Notes", RF(14, 52, 150, 30), 17.f, 600, pal.text, 0, 1);
            IconBtn(349, 67, 28, ID_NOTE_ADD, 0, L"\uE710", 1, 13.f);
            if (notes.empty())
            {
                Text(L"No notes yet. Press + to add one.", RF(24, 130, 324, 30), 13.f, 400, pal.sub, 1, 1);
                return;
            }
            for (size_t i = 0; i < notes.size(); ++i)
            {
                const Note &n = notes[i];
                D2D1_RECT_F r = RF(8, 92.f + (float)i * 60.f, 356, 54);
                RoundFill(r, 8.f, Mix(pal.card, pal.cardHover, HoverAmount(ID_NOTE_OPEN, n.id)));
                RoundStroke(r, 8.f, pal.border, 1.f);
                AddHit(r, ID_NOTE_OPEN, n.id);
                const SYSTEMTIME when = td::WallToLocal(n.modified * 1000);
                Text(DateStr(when, L"MMM d"), RF(r.right - 108, r.top + 8, 70, 18), 10.5f, 400, pal.sub, 2, 1);
                Text(n.title.empty() ? std::wstring(L"Untitled") : n.title, RF(20, r.top + 8, 236, 20), 13.5f, 600,
                     n.title.empty() ? pal.sub : pal.text, 0, 1);
                const std::wstring snip = Snippet(n.body);
                Text(snip.empty() ? std::wstring(L"Empty note") : snip, RF(20, r.top + 29, 300, 18), 11.5f, 400, pal.sub, 0, 1);
                DeleteIcon(RF(r.right - 34, r.top + 14, 26, 26), n.id);
            }
        }

        void PaintNotePreview()
        {
            Note *n = FindNote(curNote);
            FlatIcon(RF(4, 51, 30, 30), ID_NOTE_BACK, 0, L"\uE72B", 13.f);
            Text(n->title.empty() ? std::wstring(L"Untitled") : n->title, RF(38, 50, 138, 32), 14.f, 600, pal.text, 0, 1);
            FlatIcon(RF(178, 51, 26, 30), ID_NOTE_ZOOM, 0, L"\uE76B", 11.f);
            Text(std::to_wstring(noteZoom) + L"%", RF(204, 50, 42, 32), 11.5f, 600, pal.sub, 1, 1);
            FlatIcon(RF(246, 51, 26, 30), ID_NOTE_ZOOM, 1, L"\uE76C", 11.f);
            FlatIcon(RF(294, 51, 30, 30), ID_NOTE_COPY, 0, L"\uE8C8", 13.f); // copy the body text (not the title)
            FlatIcon(RF(334, 51, 30, 30), ID_NOTE_EDIT, 0, L"\uE70F", 13.f);

            const float cardH = panelH - 96.f;
            const D2D1_RECT_F card = RF(8, 88, 356, cardH);
            RoundFill(card, 8.f, pal.card);
            RoundStroke(card, 8.f, pal.border, 1.f);
            EnsureDoc();
            if (n->body.empty() || rich::Trim(n->body).empty())
            {
                Text(L"Empty note. Tap the pencil to write or paste something.", RF(24, 120, 324, 40), 12.f, 400, pal.sub, 1, 0, false, true);
                return;
            }
            const float scroll = SnapPx(PreviewNow());
            rt->PushAxisAlignedClip(RF(9, 89, 354, cardH - 2), D2D1_ANTIALIAS_MODE_ALIASED);
            rich::DrawOps(*this, doc.ops, 20.f, 98.f - scroll, 89.f, 88.f + cardH);
            rt->PopAxisAlignedClip();
            // Only the link text itself is clickable (each word of a link is its own hit area).
            noteLinks.clear();
            for (const rich::Op &o : doc.ops)
            {
                if (o.kind != rich::OP_TEXT || o.url.empty() || noteLinks.size() >= 400)
                    continue;
                const rich::FontInfo &fi = rich::Font(o.st);
                const float top = 98.f - scroll + o.y - fi.baseline, bottom = top + fi.height;
                if (bottom < 89.f || top > 88.f + cardH - 2.f)
                    continue;
                noteLinks.push_back(o.url);
                AddHit(RF(20.f + o.x, std::max(89.f, top), o.x2, std::min(88.f + cardH - 2.f, bottom) - std::max(89.f, top)), ID_NOTE_LINK,
                       (int)noteLinks.size() - 1);
            }
            const float mx = PreviewMax();
            if (mx > 0.5f)
            { // very thin scroll indicator
                const float trackTop = 96.f, trackH = cardH - 16.f;
                const float thumbH = std::min(trackH, std::max(22.f, trackH * PreviewViewH() / (doc.height + 6.f)));
                RoundFill(RF(card.right - 5.f, trackTop + (trackH - thumbH) * (scroll / mx), 2.f, thumbH), 1.f, Fade(pal.sub, 0.8f));
            }
        }

        void PaintNoteEditor()
        {
            Note *n = FindNote(curNote);
            FlatIcon(RF(4, 51, 30, 30), ID_NOTE_BACK, 0, L"\uE72B", 13.f);
            const int chars = (int)(n->title.size() + n->body.size());
            Text(std::to_wstring(chars) + (chars == 1 ? L" character" : L" characters") + L"  \u00B7  saved automatically", RF(38, 50, 280, 32), 11.5f, 400, pal.sub, 0, 1);
            DeleteIcon(RF(334, 52, 28, 28), n->id);
            UseEdit(E_NTITLE, RF(8, 88, 356, 34), 10, 6);
            const float by = 128.f, bh = panelH - 8.f - by;
            const D2D1_RECT_F bf = RF(8, by, 356, bh);
            UseEdit(E_NBODY, bf, 10, 8);
            editRect[E_NBODY].right -= 6.f; // room for the thin scroll indicator
            const int total = (int)SendMessageW(edits[E_NBODY], EM_GETLINECOUNT, 0, 0);
            const int first = (int)SendMessageW(edits[E_NBODY], EM_GETFIRSTVISIBLELINE, 0, 0);
            const int vis = std::max(1, (int)(RH(editRect[E_NBODY]) / bodyLineH));
            if (total > vis)
            {
                const float trackTop = by + 8.f, trackH = bh - 16.f;
                const float thumbH = std::max(18.f, trackH * (float)vis / (float)total);
                const float f = Clamp01((float)first / (float)std::max(1, total - vis));
                RoundFill(RF(bf.right - 6.f, trackTop + (trackH - thumbH) * f, 2.f, thumbH), 1.f, Fade(pal.sub, 0.8f));
            }
        }

        // ---- settings ------------------------------------------------------------------------------------------------
        void SettingRow(float y, float h, const wchar_t *title, const wchar_t *desc)
        {
            D2D1_RECT_F r = RF(16, y, 340, h);
            RoundFill(r, 8.f, pal.card);
            RoundStroke(r, 8.f, pal.border, 1.f);
            Text(title, RF(30, y + 9, 260, 20), 14.f, 600, pal.text, 0, 1);
            if (desc && *desc)
                Text(desc, RF(30, y + 30, 310, 18), 11.f, 400, pal.sub, 0, 1);
        }

        // Sound picker row: previous / name / next, and an audition button.
        void SoundRow(float y, const wchar_t *title, int sound, int idPick, int idPlay)
        {
            SettingRow(y, 78, title, L"");
            FlatIcon(RF(30, y + 36, 32, 32), idPick, 0, L"\uE76B", 12.f);
            Text(td::kSoundNames[std::min(td::kSoundCount - 1, std::max(0, sound))], RF(62, y + 36, 190, 32), 13.f, 600, pal.text, 1, 1);
            FlatIcon(RF(252, y + 36, 32, 32), idPick, 1, L"\uE76C", 12.f);
            IconBtn(318, y + 52, 32, idPlay, 0, L"\uE768", 1, 12.f);
        }

        float AboutT() const { return aboutA.Value(kAboutMs, false); } // already eased
        float AboutHeight(float t = -1.f) const
        {
            if (t < 0.f)
                t = AboutT();
            return 62.f + (230.f - 62.f) * t;
        }

        void PaintAbout(float y)
        {
            const float t = AboutT();
            const D2D1_RECT_F card = RF(16, y, 340, AboutHeight(t));
            const D2D1_RECT_F head = RF(16, y, 340, 62);
            const float hov = HoverAmount(ID_ABOUT_TOGGLE);
            RoundFill(card, 8.f, Mix(pal.card, pal.cardHover, hov * (1.f - t)));
            RoundStroke(card, 8.f, pal.border, 1.f);
            AddHit(head, ID_ABOUT_TOGGLE);

            rt->PushAxisAlignedClip(Inset(card, 1, 1), D2D1_ANTIALIAS_MODE_ALIASED);
            // Collapsed header: name + copyright on the left, version + chevron on the right.
            const float a0 = 1.f - Clamp01(t * 2.2f);
            Text(WK_NAME, RF(30, y + 10, 200, 22), 14.5f, 600, Fade(pal.text, a0), 0, 1);
            Text(WK_COPYRIGHT, RF(30, y + 32, 220, 20), 11.f, 400, Fade(pal.sub, a0), 0, 1);
            Text(L"v" WK_VERSION, RF(236, y + 10, 70, 42), 12.f, 400, Fade(pal.sub, a0), 2, 1);

            // Expanded content uses a single left edge and a consistent row rhythm.
            const float a1 = Clamp01((t - 0.35f) / 0.65f);
            Text(L"About " WK_NAME, RF(30, y + 10, 240, 22), 14.5f, 600, Fade(pal.text, a1), 0, 1);
            Text(WK_TAGLINE, RF(30, y + 33, 310, 20), 11.5f, 400, Fade(pal.sub, a1), 0, 1);

            const wchar_t *rowName[3] = {L"Version", L"Developer", L"License"};
            const wchar_t *rowValue[3] = {WK_VERSION, WK_DEVELOPER, WK_LICENSE};
            for (int i = 0; i < 3; ++i)
            {
                const float ry = y + 68 + 25.f * (float)i;
                Text(rowName[i], RF(30, ry, 82, 22), 12.5f, 400, Fade(pal.sub, a1), 0, 1);
                Text(rowValue[i], RF(122, ry, 220, 22), 12.5f, 400, Fade(pal.text, a1), 0, 1);
            }

            if (a1 > 0.01f)
            {
                // Two equal-width links, aligned to the same inner margins as the metadata rows.
                const D2D1_RECT_F windhawk = RF(30, y + 152, 152, 34);
                const D2D1_RECT_F github = RF(190, y + 152, 152, 34);
                const Col logoWhite = Fade(C(0.96f, 0.96f, 0.96f), a1);

                const float windhawkHover = HoverAmount(ID_ABOUT_WINDHAWK);
                const Col windhawkFill = Fade(Mix(pal.cardHover, pal.accent, 0.16f + 0.14f * windhawkHover), a1);
                RoundFill(windhawk, 6.f, windhawkFill);
                RoundStroke(windhawk, 6.f, Fade(Mix(pal.border, pal.accent, 0.35f + 0.25f * windhawkHover), a1), 1.f);

                // Clean, scalable website/globe icon using the built-in Windows icon font.
                Text(L"\uE774",
                     RF(windhawk.left + 7.f, windhawk.top, 23.f, 34.f), 15.f, 400, logoWhite, 1, 1, true);
                Text(L"Windhawk Page", RF(windhawk.left + 34.f, windhawk.top, 112.f, 34.f), 12.f, 600, Fade(pal.text, a1), 0, 1);

                const float githubHover = HoverAmount(ID_ABOUT_GITHUB);
                RoundFill(github, 6.f, Fade(Mix(pal.cardHover, pal.accent, 0.16f + 0.14f * githubHover), a1));
                RoundStroke(github, 6.f, Fade(Mix(pal.border, pal.accent, 0.35f + 0.25f * githubHover), a1), 1.f);

                // Small Octocat-inspired GitHub mark, drawn with the existing Direct2D primitives.
                const float gx = github.left + 18.f;
                const float gy = github.top + 17.f;
                const Col githubBg = Fade(Mix(pal.cardHover, pal.accent, 0.16f + 0.14f * githubHover), a1);
                Circle(gx, gy, 5.7f, Fade(pal.text, a1));
                Line(gx - 4.0f, gy - 2.3f, gx - 5.0f, gy - 6.3f, Fade(pal.text, a1), 2.2f);
                Line(gx - 5.0f, gy - 6.3f, gx - 1.5f, gy - 4.8f, Fade(pal.text, a1), 2.2f);
                Line(gx + 4.0f, gy - 2.3f, gx + 5.0f, gy - 6.3f, Fade(pal.text, a1), 2.2f);
                Line(gx + 5.0f, gy - 6.3f, gx + 1.5f, gy - 4.8f, Fade(pal.text, a1), 2.2f);
                Circle(gx - 1.9f, gy - 0.4f, 0.65f, githubBg);
                Circle(gx + 1.9f, gy - 0.4f, 0.65f, githubBg);
                Line(gx - 1.8f, gy + 4.4f, gx - 2.3f, gy + 6.4f, Fade(pal.text, a1), 1.8f);
                Line(gx + 1.8f, gy + 4.4f, gx + 2.3f, gy + 6.4f, Fade(pal.text, a1), 1.8f);
                Text(L"GitHub Repo", RF(github.left + 36.f, github.top, 108.f, 34.f), 12.f, 600, Fade(pal.text, a1), 0, 1);

                if (t > 0.6f)
                {
                    AddHit(windhawk, ID_ABOUT_WINDHAWK);
                    AddHit(github, ID_ABOUT_GITHUB);
                }
            }

            Fill(RF(30, y + 198, 312, 1), Fade(pal.border, a1));
            Text(WK_COPYRIGHT, RF(30, y + 204, 312, 22), 11.f, 400, Fade(pal.sub, a1), 0, 1);
            rt->PopAxisAlignedClip();
            // Chevron (rotates by swapping glyphs half-way).
            Text(t < 0.5f ? L"\uE70D" : L"\uE70E", RF(316, y + 10, 28, 42), 11.f, 400, pal.sub, 1, 1, true);
        }

        void PaintSettings()
        {
            auto &S = td::g_set;
            float y = 58;
            auto toggleRow = [&](const wchar_t *t, const wchar_t *d, int id, bool on)
            {
                SettingRow(y, 56, t, d);
                UiWindow::Toggle(302, y + 18, id, on);
                y += 64;
            };
            toggleRow(L"Notification icon", L"Show the bell next to the clock", ID_ST_BELL, S.showBell.load() != 0);
            toggleRow(L"Taskbar previews", L"Stopwatch / timer next to the clock", ID_ST_PREVIEW, S.showPreviews.load() != 0);

            SettingRow(y, 70, L"Time format", L"");
            Chip(RF(30, y + 36, 150, 26), ID_ST_FORMAT, 0, L"12-hour", !S.Use24());
            Chip(RF(192, y + 36, 150, 26), ID_ST_FORMAT, 1, L"24-hour", S.Use24());
            y += 78;
            SettingRow(y, 70, L"Panel material", L""); // only visible effect when Windows transparency effects are on
            Chip(RF(30, y + 36, 150, 26), ID_ST_ACRYLIC, 1, L"Acrylic blur", S.acrylic.load() != 0);
            Chip(RF(192, y + 36, 150, 26), ID_ST_ACRYLIC, 0, L"Simple", S.acrylic.load() == 0);
            y += 78;
            SettingRow(y, 70, L"Preview layout", L"");
            Chip(RF(30, y + 36, 150, 26), ID_ST_LAYOUT, 0, L"Side by side", S.stacked.load() == 0);
            Chip(RF(192, y + 36, 150, 26), ID_ST_LAYOUT, 1, L"Stacked (compact)", S.stacked.load() == 1);
            y += 78;
            SoundRow(y, L"Default timer sound", S.timerSound.load(), ID_ST_TSOUND, ID_ST_TSOUND_PLAY);
            y += 86;
            SoundRow(y, L"Default alarm sound", S.alarmSound.load(), ID_ST_ASOUND, ID_ST_ASOUND_PLAY);
            y += 86;
            Btn(RF(16, y, 340, 38), ID_ST_SYNC, 0, syncBusy ? L"Syncing time\u2026" : L"Sync time now", 0, 13.5f);
            y += 38 + 30; // a little air between the function and the About card
            settingsAboutTop = y;
            PaintAbout(y);
        }

        // ---------------------------------------------------------------- input
        void TabWheel(int delta, bool horizontal)
        {
            if (TabMax() < 0.5f)
                return;
            const float d = horizontal ? (float)delta : -(float)delta; // wheel up scrolls the bar left
            tabScrollA.Go(std::min(TabMax(), std::max(0.f, tabScrollA.to + d * 0.45f)), TabScrollNow());
            StartTabTimer();
        }

        void OnWheel(int delta, float, float y, bool horizontal) override
        {
            if (delta == 0)
                return;
            if (y < 47.f)
            {
                TabWheel(delta, horizontal);
                return;
            } // mouse wheel / touchpad over the tab bar
            if (horizontal || y >= ContentViewportBottom())
                return;
            if (tab == 1)
            { // only the lap list scrolls; the clock and controls stay put
                if (y < kLapRegionTop || snap.laps.empty())
                    return;
                lapSm.target = std::min(LapMax(), std::max(0.f, lapSm.target - (float)delta * (kWheelPx / (float)WHEEL_DELTA)));
                KickScroll();
                return;
            }
            if (tab == 4 && noteView == 1)
            {
                if (y >= 88.f)
                    PreviewScrollTo(prevSm.target - (float)delta * (kWheelPx / (float)WHEEL_DELTA));
                return;
            }
            if (tab == 4 && noteView == 2)
            { // editor: three lines per notch, smooth glide
                if (y >= 88.f)
                    EditorWheel(delta);
                return;
            }
            if (PanelScrollMax() > 0.5f)
            { // continuous: precision touchpads deliver many small deltas
                ScrollTo(panelScrollTarget - (float)delta * (kWheelPx / (float)WHEEL_DELTA));
                return;
            }
            if (tab == 0)
                CalWheel(delta); // calendar: pages slide smoothly
        }

        void OnClick(int id, int param, bool right) override
        {
            if (right)
            {
                if (id == ID_TM_PRESET && param >= 0 && param < (int)snap.presets.size())
                {
                    presetDeleteIndex = param;
                    Invalidate(); // reveal only that preset's remove affordance in place
                }
                return;
            }
            switch (id)
            {
            case ID_TAB:
                SelectTab(param);
                return;
            case ID_CAL_PREV:
            case ID_CAL_NEXT:
            case ID_CAL_HDR:
            case ID_CAL_CELL:
            case ID_CAL_TODAY:
                CalClick(id, param);
                return;
            case ID_SW_TOGGLE:
                td::g_eng.SwToggle();
                tb::PostRefreshAll();
                break;
            case ID_SW_LAP:
                td::g_eng.SwLap();
                break;
            case ID_SW_RESET:
                td::g_eng.SwReset();
                tb::PostRefreshAll();
                break;
            case ID_TM_STEP:
            {
                const int field = param / 2, dir = (param % 2) ? 1 : -1;
                if (field == 0)
                    StepEdit(E_TH, dir, 0, 99);
                else
                    StepEdit(E_TM + field - 1, dir, 0, 59);
                return;
            }
            case ID_TM_START:
                StartTimerFromEdits();
                tb::PostRefreshAll();
                break;
            case ID_TM_PRESET:
                presetDeleteIndex = -1;
                ApplyPreset(param);
                break;
            case ID_TM_PRESET_DELETE:
                if (param >= 0 && param < (int)snap.presets.size())
                {
                    td::g_eng.PresetDelete((size_t)param);
                    presetDeleteIndex = -1;
                    snap = td::g_eng.Snapshot();
                }
                break;
            case ID_TM_SAVEPRESET:
                SavePresetFromEdits();
                break;
            case ID_TM_TOGGLE:
                td::g_eng.TimerToggle(param);
                tb::PostRefreshAll();
                break;
            case ID_TM_CANCEL:
                td::g_eng.TimerCancel(param);
                tb::PostRefreshAll();
                break;
            case ID_AL_ADD:
                OpenAlarmEditor(0);
                return;
            case ID_AL_ROW:
                OpenAlarmEditor(param);
                return;
            case ID_AL_TOGGLE:
            {
                bool cur = false;
                for (auto &a : snap.alarms)
                    if (a.id == param)
                        cur = a.enabled;
                td::g_eng.AlarmSetEnabled(param, !cur);
                break;
            }
            case ID_AL_DEL:
                td::g_eng.AlarmDelete(param);
                break;
            case ID_AL_STEP:
            {
                const int field = param / 2, dir = (param % 2) ? 1 : -1;
                if (field == 0)
                {
                    if (td::g_set.Use24())
                        StepEdit(E_AH, dir, 0, 23);
                    else
                        StepEdit(E_AH, dir, 1, 12);
                }
                else
                    StepEdit(E_AM, dir, 0, 59);
                return;
            }
            case ID_AL_PM:
                editPM = param == 1;
                break;
            case ID_AL_DAY:
                edit.days ^= (1u << param);
                break;
            case ID_AL_SNOOZE:
            {
                static const int opts[] = {1, 2, 3, 5, 10, 15, 20, 30, 45, 60};
                int idx = 4;
                for (int i = 0; i < 10; ++i)
                    if (opts[i] == edit.snoozeMin)
                        idx = i;
                edit.snoozeMin = opts[std::min(9, std::max(0, idx + (param ? 1 : -1)))];
                break;
            }
            case ID_AL_SOUND:
                edit.sound = (edit.sound + (param ? 1 : -1) + td::kSoundCount) % td::kSoundCount;
                StopPreviewSound();
                PreviewSound(edit.sound); // audition on every change
                break;
            case ID_AL_SOUND_PLAY:
                StopPreviewSound();
                PreviewSound(edit.sound);
                break;
            case ID_ST_TSOUND:
                td::g_set.timerSound = (td::g_set.timerSound.load() + (param ? 1 : -1) + td::kSoundCount) % td::kSoundCount;
                td::g_set.Save();
                StopPreviewSound();
                PreviewSound(td::g_set.timerSound.load());
                break;
            case ID_ST_TSOUND_PLAY:
                StopPreviewSound();
                PreviewSound(td::g_set.timerSound.load());
                break;
            case ID_ST_ASOUND:
                td::g_set.alarmSound = (td::g_set.alarmSound.load() + (param ? 1 : -1) + td::kSoundCount) % td::kSoundCount;
                td::g_set.Save();
                StopPreviewSound();
                PreviewSound(td::g_set.alarmSound.load());
                break;
            case ID_ST_ASOUND_PLAY:
                StopPreviewSound();
                PreviewSound(td::g_set.alarmSound.load());
                break;
            case ID_ABOUT_TOGGLE:
                aboutOpen = !aboutOpen;
                aboutA.Go(aboutOpen ? 1.f : 0.f, AboutT());
                {
                    SetTimer(hwnd, TIMER_ABOUT_ANIM, 15, nullptr);
                    if (aboutOpen)
                    { // glide down so the whole expanded card is visible
                        panelScrollTarget = std::max(0.f, settingsAboutTop + AboutHeight(1.f) + 18.f - ContentViewportBottom());
                        panelScrollStarted = GetTickCount64();
                        KickScroll();
                    }
                }
                return;
            case ID_ABOUT_WINDHAWK:
                ShellExecuteW(nullptr, L"open", WK_WINDHAWK_URL, nullptr, nullptr, SW_SHOWNORMAL);
                return;
            case ID_ABOUT_GITHUB:
                ShellExecuteW(nullptr, L"open", L"https://github.com/shifting-whistler/WinKlok", nullptr, nullptr, SW_SHOWNORMAL);
                return;
            case ID_AL_SAVE:
                SaveAlarmFromEditor();
                return;
            case ID_AL_CANCEL:
                CloseAlarmEditor();
                return;
            case ID_NOTE_ADD:
                NewNote();
                return;
            case ID_NOTE_OPEN:
                OpenNote(param);
                return;
            case ID_NOTE_DEL:
            { // two clicks: the first arms the button (red), the second deletes
                const ULONGLONG now = GetTickCount64();
                if (noteDelArmedId == param && now < noteDelArmedUntil)
                {
                    noteDelArmedId = 0;
                    noteDelArmedUntil = 0;
                    DeleteNote(param);
                    return;
                }
                noteDelArmedId = param;
                noteDelArmedUntil = now + 3000;
                break;
            }
            case ID_NOTE_BACK:
                if (noteView == 2)
                    LeaveEdit();
                else
                    BackToList();
                return;
            case ID_NOTE_EDIT:
                EnterEdit(curNote, E_NBODY);
                return;
            case ID_NOTE_ZOOM:
                SetNoteZoom(noteZoom + (param ? 10 : -10));
                break;
            case ID_NOTE_LINK:
                if (param >= 0 && param < (int)noteLinks.size())
                {
                    const std::wstring &u = noteLinks[param];
                    if (u.compare(0, 7, L"http://") == 0 || u.compare(0, 8, L"https://") == 0 || u.compare(0, 7, L"mailto:") == 0)
                        ShellExecuteW(nullptr, L"open", u.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                }
                return;
            case ID_NOTE_COPY:
                if (Note *n = FindNote(curNote))
                {
                    if (htmlmd::Trim(n->body).empty())
                        SetStatus(L"Nothing to copy");
                    else if (htmlmd::SetClipboardText(hwnd, ToCrlf(n->body)))
                        SetStatus(L"Note text copied");
                    else
                        SetStatus(L"Could not access the clipboard");
                }
                break;
            case ID_ST_BELL:
                td::g_set.showBell = !td::g_set.showBell.load();
                td::g_set.Save();
                tb::PostRefreshAll();
                break;
            case ID_ST_PREVIEW:
                td::g_set.showPreviews = !td::g_set.showPreviews.load();
                td::g_set.Save();
                tb::PostRefreshAll();
                break;
            case ID_ST_LAYOUT:
                td::g_set.stacked = param;
                td::g_set.Save();
                tb::PostRefreshAll();
                break;
            case ID_ST_FORMAT: // applies everywhere at once: alarms, alerts, timer end times
                td::g_set.hour24 = param;
                td::g_set.Save();
                break;
            case ID_ST_ACRYLIC:
                td::g_set.acrylic = param;
                td::g_set.Save();
                ApplyComposition();
                break;
            case ID_ST_SYNC:
                if (StartTimeSync())
                {
                    syncBusy = true;
                    SetStatus(L"Syncing time\u2026");
                }
                break;
            }
            ResizePanelToContent();
            Invalidate();
        }

        void PrimaryAction()
        {
            if (tab == 2)
            {
                StartTimerFromEdits();
                tb::PostRefreshAll();
            }
            else if (tab == 3 && alarmEditing)
                SaveAlarmFromEditor();
            else if (tab == 4 && noteView == 2)
                SetFocus(edits[E_NBODY]); // Enter in the title moves on to the body
            ResizePanelToContent();
            Invalidate();
        }

        LRESULT HandleMessage(UINT msg, WPARAM wp, LPARAM lp) override
        {
            switch (msg)
            {
            case WM_TIMER:
                switch (wp)
                {
                case TIMER_PANEL_ANIM:
                    AdvancePanelAnimation();
                    return 0;
                case TIMER_SCROLL_ANIM:
                    AdvanceScrollAnimation();
                    return 0;
                case TIMER_NOTE_SAVE:
                    FlushNote();
                    return 0;
                case TIMER_ABOUT_ANIM:
                    if (!aboutA.Running(kAboutMs))
                    {
                        aboutA.Set(aboutA.to);
                        KillTimer(hwnd, TIMER_ABOUT_ANIM);
                        ResizePanelToContent();
                    }
                    Invalidate();
                    return 0;
                case TIMER_TAB_ANIM:
                    if (!tabScrollA.Running(kTabAnimMs) && !indXA.Running(kTabAnimMs) && !indWA.Running(kTabAnimMs))
                    {
                        tabScrollA.Set(tabScrollA.to);
                        indXA.Set(indXA.to);
                        indWA.Set(indWA.to);
                        KillTimer(hwnd, TIMER_TAB_ANIM);
                    }
                    Invalidate();
                    return 0;
                case TIMER_STATUS_ANIM:
                {
                    const ULONGLONG now = GetTickCount64();
                    if (statusMsg.empty() || now >= statusUntil)
                    {
                        statusMsg.clear();
                        statusStarted = 0;
                        statusTimerActive = false;
                        KillTimer(hwnd, TIMER_STATUS_ANIM);
                        ResizePanelToContent();
                    }
                    else if (now - statusStarted < 235 || statusUntil - now <= 280)
                    {
                        Invalidate();
                    }
                    else
                    {
                        statusTimerActive = false;
                        KillTimer(hwnd, TIMER_STATUS_ANIM);
                    }
                    return 0;
                }
                case TIMER_CONTENT_ANIM:
                    if (!contentTransitionStarted || GetTickCount64() - contentTransitionStarted >= 225)
                    {
                        contentTransitionStarted = 0;
                        KillTimer(hwnd, TIMER_CONTENT_ANIM);
                    }
                    Invalidate();
                    return 0;
                case TIMER_TICK:
                    OnTick();
                    return 0;
                case TIMER_DEACTIVATE_CHECK:
                    KillTimer(hwnd, TIMER_DEACTIVATE_CHECK);
                    // Focus-loss dismissal is deferred briefly so a clock click can deliver its
                    // pointer event and its explicit toggle message first.
                    if (IsWindowVisible(hwnd) && !noActMode)
                    {
                        if (g_clockTogglePendingUntil.load() > GetTickCount64())
                        {
                            SetTimer(hwnd, TIMER_DEACTIVATE_CHECK, 180, nullptr);
                        }
                        else
                        {
                            g_clockTogglePendingUntil = 0;
                            HWND fg = GetForegroundWindow();
                            if (fg != hwnd && (!fg || !IsChild(hwnd, fg)))
                                HidePanel();
                        }
                    }
                    return 0;
                }
                break;
            case WM_UI_TOGGLE:
                g_clockTogglePendingUntil = 0;
                Toggle(wp != 0);
                return 0;
            case WM_UI_QUIT:
                PostQuitMessage(0);
                return 0;
            case WM_UI_SYNCDONE:
                syncBusy = false;
                SetStatus(wp == SYNC_OK          ? L"Time synced"
                          : wp == SYNC_CANCELLED ? L"Time sync was cancelled"
                                                 : L"Time sync failed");
                td::g_eng.RescheduleAfterTimeChange();
                Invalidate();
                return 0;
            case WM_MOUSEACTIVATE:
                noActMode = false;
                return MA_ACTIVATE;
            case WM_ACTIVATE:
                if (LOWORD(wp) != WA_INACTIVE)
                {
                    noActMode = false;
                    KillTimer(hwnd, TIMER_DEACTIVATE_CHECK);
                }
                else if (IsWindowVisible(hwnd) && !noActMode)
                {
                    // Hiding synchronously would race the clock-toggle message and reopen the panel.
                    SetTimer(hwnd, TIMER_DEACTIVATE_CHECK, 180, nullptr);
                }
                return 0;
            case WM_COMMAND:
                if (LOWORD(wp) == IDOK)
                {
                    PrimaryAction();
                    return 0;
                }
                if (LOWORD(wp) == IDCANCEL)
                {
                    HidePanel();
                    return 0;
                }
                if (HIWORD(wp) == EN_CHANGE && (LOWORD(wp) == 100 + E_NTITLE || LOWORD(wp) == 100 + E_NBODY))
                    OnNoteTextChanged();
                if (HIWORD(wp) == EN_VSCROLL)
                    Invalidate(); // keeps the editor's thin scroll indicator in sync
                if (HIWORD(wp) == EN_SETFOCUS || HIWORD(wp) == EN_KILLFOCUS)
                    Invalidate();
                return 0;
            case WM_CTLCOLOREDIT:
            {
                HDC hdc = (HDC)wp;
                SetTextColor(hdc, RGB((int)(pal.text.r * 255), (int)(pal.text.g * 255), (int)(pal.text.b * 255)));
                SetBkColor(hdc, RGB((int)(pal.field.r * 255), (int)(pal.field.g * 255), (int)(pal.field.b * 255)));
                return (LRESULT)editBrush;
            }
            case WM_TIMECHANGE:
                td::g_eng.RescheduleAfterTimeChange();
                return 0;
            case WM_SETTINGCHANGE: // theme, accent colour or the Transparency effects switch changed
                ApplyComposition();
                ResizePanelToContent();
                Invalidate();
                return 0;
            }
            return UiWindow::HandleMessage(msg, wp, lp);
        }
    };

    static PanelWindow *g_panel = nullptr;

    // Note body editor: rich paste, and wheel / touchpad scrolling that reaches the edit directly
    // (pointer-wheel and legacy wheel messages are both taken, so every touchpad driver works).
    static LRESULT CALLBACK BodyEditProc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR)
    {
        PanelWindow *p = g_panel;
        if (p)
        {
            if (m == WM_POINTERDOWN || m == WM_POINTERUPDATE || m == WM_POINTERUP)
                p->HandleTouchpadPointer(m, w); // touchpad contacts over a text field still scroll the panel
            const bool isBody = h == p->edits[E_NBODY];
            if (m == WM_MOUSEMOVE || m == WM_SETCURSOR || m == WM_NCHITTEST)
                RouteTouchpadTo(isBody ? h : p->hwnd); // touchpad pointers go to the body editor only while it is under the cursor
            if (isBody && m == WM_PASTE && p->PasteIntoBody(h))
                return 0;
            if (isBody && (m == WM_MOUSEWHEEL || m == WM_POINTERWHEEL))
            {
                if (GetTickCount64() - p->lastTouchpadTick < 350)
                    return 0;
                const int delta = (int)GET_WHEEL_DELTA_WPARAM(w);
                if (delta)
                    p->EditorWheelFromEdit(delta);
                return 0;
            }
        }
        return DefSubclassProc(h, m, w, l);
    }

    // ---------------------------------------------------------------------------------------------
    class AlertWindow : public UiWindow
    {
    public:
        td::RingEvent ev;
        bool missedList = false; // "you missed ..." popup: no sound, no snooze
        std::vector<td::MissedItem> items;
        float height = 124.f;
        static constexpr float W = 360.f, H = 124.f;
        int compMode = 0; // same material as the panel: 0 opaque, 1 simple transparency, 2 acrylic
        BYTE restingAlpha = 255;
        bool animateOnNextLayout = true;
        ULONGLONG appearStarted = 0;
        int appearX = 0, appearStartY = 0, appearEndY = 0, appearWidth = 0, appearHeight = 0;

        // Picks palette and material exactly like the panel (Windows transparency setting + Settings > Panel material).
        void ApplyMaterial()
        {
            const bool trans = TransparencyEffectsEnabled();
            const bool acrylic = trans && td::g_set.acrylic.load() != 0;
            compMode = acrylic ? 2 : trans ? 1
                                           : 0;
            grayText = compMode != 0;
            pal = LoadPalette(acrylic);
            if (alphaTarget != acrylic)
            {
                alphaTarget = acrylic;
                ReleaseTarget();
            }
            else if (rt)
                rt->SetTextAntialiasMode(grayText ? D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE : D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
            DwmStyle(hwnd, pal);
            restingAlpha = ConfigureComposition(hwnd, pal, compMode);
        }

        // The same sequence as PanelWindow::WarmUpMaterial (Simple, then Acrylic, on a window DWM is composing).
        void WarmUpMaterial()
        {
            alphaTarget = false;
            ReleaseTarget();
            restingAlpha = ConfigureComposition(hwnd, pal, 1);
            Render();
            DwmFlush();
            alphaTarget = true;
            ReleaseTarget();
            restingAlpha = ConfigureComposition(hwnd, pal, 2);
            Render();
            RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE | RDW_ALLCHILDREN);
            DwmFlush();
            DwmFlush();
        }

        // Shows the popup. In acrylic mode the live backdrop only comes up correctly when the material is switched on
        // a window DWM is already composing, so the window is cloaked while that happens (as for the panel).
        void ShowWithMaterial(int x, int y, int w, int h)
        {
            bool cloaked = false;
            if (compMode == 2)
            {
                const BOOL on = TRUE; // DWMWA_CLOAK
                cloaked = SUCCEEDED(DwmSetWindowAttribute(hwnd, 13, &on, sizeof(on)));
            }
            SetWindowPos(hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
            if (compMode == 2)
            {
                WarmUpMaterial();
                restingAlpha = ConfigureComposition(hwnd, pal, compMode);
                Render();
                RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE | RDW_ALLCHILDREN);
            }
            if (cloaked)
            {
                const BOOL off = FALSE;
                DwmSetWindowAttribute(hwnd, 13, &off, sizeof(off));
            }
        }

        static std::wstring When(long long whenWall)
        {
            SYSTEMTIME st = td::WallToLocal(whenWall), now{};
            GetLocalTime(&now);
            auto dayIndex = [](SYSTEMTIME t)
            {
                t.wHour = t.wMinute = t.wSecond = t.wMilliseconds = 0;
                FILETIME ft{};
                SystemTimeToFileTime(&t, &ft);
                ULARGE_INTEGER u;
                u.LowPart = ft.dwLowDateTime;
                u.HighPart = ft.dwHighDateTime;
                return (long long)(u.QuadPart / 864000000000ULL);
            };
            const long long diff = dayIndex(now) - dayIndex(st);
            std::wstring day = diff == 0   ? L"Today"
                               : diff == 1 ? L"Yesterday"
                                           : DateStr(st, st.wYear == now.wYear ? L"ddd, MMM d" : L"ddd, MMM d, yyyy");
            return day + L", " + td::FmtTimeOfDay(st.wHour, st.wMinute);
        }

        void PaintMissed()
        {
            const int n = (int)items.size();
            Text(L"While you were away", RF(20, 14, W - 40, 26), 16.5f, 600, pal.text, 0, 1);
            Text(n == 1 ? std::wstring(L"You missed 1 alarm or timer") : L"You missed " + std::to_wstring(n) + L" alarms and timers",
                 RF(20, 38, W - 40, 20), 12.5f, 400, pal.sub, 0, 1);
            float y = 66.f;
            const int shown = std::min(n, 5);
            for (int i = 0; i < shown; ++i)
            {
                const td::MissedItem &m = items[i];
                const D2D1_RECT_F row = RF(14, y, W - 28, 46);
                RoundFill(row, 8.f, pal.card);
                RoundStroke(row, 8.f, pal.border, 1.f);
                Circle(40, y + 23, 15, pal.accent);
                Text(m.isAlarm ? L"\uEA8F" : L"\uE916", RF(25, y + 8, 30, 30), 14.f, 400, pal.onAccent, 1, 1, true);
                Text(m.title, RF(64, y + 5, W - 96, 20), 13.5f, 600, pal.text, 0, 1);
                std::wstring sub = m.isAlarm ? L"Alarm  \u2022  " : L"Timer ended  \u2022  ";
                sub += When(m.whenWall);
                if (!m.isAlarm && m.totalMs > 0)
                    sub += L"  \u2022  " + td::FmtDuration(m.totalMs);
                if (m.count > 1)
                    sub += L"  \u2022  " + std::to_wstring(m.count) + L" times";
                Text(sub, RF(64, y + 24, W - 96, 18), 11.5f, 400, pal.sub, 0, 1);
                y += 52.f;
            }
            if (n > shown)
            {
                Text(L"+ " + std::to_wstring(n - shown) + L" more", RF(20, y - 2, W - 40, 20), 12.f, 400, pal.sub, 0, 1);
                y += 22.f;
            }
            Btn(RF(16, y + 6, W - 32, 34), ID_ALERT_DISMISS, 0, L"Dismiss", 1, 13.f);
        }

        void Paint() override
        {
            if (missedList)
            {
                PaintMissed();
                return;
            }
            Circle(42, 44, 22, pal.accent);
            Text(ev.isAlarm ? L"\uEA8F" : L"\uE916", RF(20, 22, 44, 44), 20.f, 400, pal.onAccent, 1, 1, true);
            Text(ev.title, RF(78, 16, W - 94, 28), 17.f, 600, pal.text, 0, 1);
            Text(ev.sub, RF(78, 44, W - 94, 22), 12.5f, 400, pal.sub, 0, 1);
            const float by = H - 46;
            if (ev.isAlarm)
            {
                const float bw = (W - 40) / 2;
                Btn(RF(16, by, bw, 34), ID_ALERT_SNOOZE, 0, L"Snooze " + std::to_wstring(ev.snoozeMin) + L" min", 0, 13.f);
                Btn(RF(24 + bw, by, bw, 34), ID_ALERT_DISMISS, 0, L"Dismiss", 1, 13.f);
            }
            else
            {
                Btn(RF(16, by, W - 32, 34), ID_ALERT_DISMISS, 0, L"Dismiss", 1, 13.f);
            }
        }

        void OnClick(int id, int, bool right) override;

        LRESULT HandleMessage(UINT msg, WPARAM wp, LPARAM lp) override
        {
            if (msg == WM_UI_CLOSEALERT)
            {
                DestroyWindow(hwnd);
                return 0;
            }
            if (msg == WM_TIMER && wp == TIMER_ALERT_ANIM)
            {
                const float t = Clamp01((float)(GetTickCount64() - appearStarted) / 220.f);
                const float eased = EaseOutCubic(t);
                const int y = appearStartY + (int)std::lround((appearEndY - appearStartY) * eased);
                // Over the live acrylic backdrop the popup only slides (no fade), like the panel.
                if (compMode != 2)
                    SetLayeredWindowAttributes(hwnd, 0, (BYTE)std::lround((float)restingAlpha * eased), LWA_ALPHA);
                SetWindowPos(hwnd, HWND_TOPMOST, appearX, t >= 1.f ? appearEndY : y, appearWidth, appearHeight,
                             SWP_NOACTIVATE | SWP_NOSENDCHANGING);
                if (t >= 1.f)
                {
                    KillTimer(hwnd, TIMER_ALERT_ANIM);
                    if (compMode != 2)
                        SetLayeredWindowAttributes(hwnd, 0, restingAlpha, LWA_ALPHA);
                }
                return 0;
            }
            if (msg == WM_MOUSEACTIVATE)
                return MA_NOACTIVATE;
            return UiWindow::HandleMessage(msg, wp, lp);
        }
    };

    static std::vector<AlertWindow *> g_alerts;

    static void SoundUpdate()
    {
        bool anyRinging = false;
        for (auto *a : g_alerts)
            anyRinging |= !a->missedList;
        if (!anyRinging)
        {
            if (snd::g_ringing)
            {
                snd::Stop();
                snd::g_ringing = false;
            }
        }
        else if (!snd::g_ringing)
        {
            AlertWindow *ringing = nullptr;
            for (auto *a : g_alerts)
                if (!a->missedList)
                    ringing = a;
            if (ringing)
            {
                snd::Play(ringing->ev.sound, true);
                snd::g_ringing = true;
            }
        }
    }

    static void LayoutAlerts()
    {
        POINT cur;
        GetCursorPos(&cur);
        HMONITOR mon = MonitorFromPoint(cur, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        GetMonitorInfoW(mon, &mi);
        for (size_t i = 0; i < g_alerts.size(); ++i)
        {
            AlertWindow *a = g_alerts[i];
            if (!a->hwnd)
                continue;
            SetWindowPos(a->hwnd, nullptr, mi.rcWork.left + 20, mi.rcWork.top + 20, 50, 50,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOSENDCHANGING);
            UINT nd = GetDpiForWindow(a->hwnd);
            if (nd && nd != a->dpi)
            {
                a->dpi = nd;
                a->ReleaseTarget();
            }
            const float s = a->Scale();
            const int w = (int)(AlertWindow::W * s), h = (int)(a->height * s), gap = (int)(8 * s), margin = (int)(12 * s);
            const int x = mi.rcWork.right - w - margin;
            int below = 0; // total height of the alerts stacked beneath this one
            for (size_t j = 0; j < i; ++j)
                below += (int)(g_alerts[j]->height * s) + gap;
            int y = mi.rcWork.bottom - margin - h - below;
            if (y < mi.rcWork.top)
                y = mi.rcWork.top;
            if (a->animateOnNextLayout)
            {
                a->animateOnNextLayout = false;
                a->appearStarted = GetTickCount64();
                a->appearX = x;
                a->appearEndY = y;
                a->appearStartY = y + (int)std::lround(10.f * s);
                a->appearWidth = w;
                a->appearHeight = h;
                if (a->compMode != 2)
                    SetLayeredWindowAttributes(a->hwnd, 0, 0, LWA_ALPHA);
                a->ShowWithMaterial(x, a->appearStartY, w, h);
                SetTimer(a->hwnd, TIMER_ALERT_ANIM, 15, nullptr);
            }
            else
            {
                KillTimer(a->hwnd, TIMER_ALERT_ANIM);
                if (a->compMode != 2)
                    SetLayeredWindowAttributes(a->hwnd, 0, a->restingAlpha, LWA_ALPHA);
                SetWindowPos(a->hwnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
            }
            a->Invalidate();
        }
    }

    void AlertWindow::OnClick(int id, int, bool right)
    {
        if (right)
            return;
        if (id == ID_ALERT_DISMISS)
        {
            if (ev.isAlarm && !missedList)
                td::g_eng.AlarmDismiss(ev.id);
        }
        else if (id == ID_ALERT_SNOOZE)
        {
            td::g_eng.AlarmSnooze(ev.id);
        }
        else
            return;
        g_alerts.erase(std::remove(g_alerts.begin(), g_alerts.end(), this), g_alerts.end());
        SoundUpdate();
        LayoutAlerts();
        PostMessageW(hwnd, WM_UI_CLOSEALERT, 0, 0);
    }

    void PanelWindow::AddAlertFor(const td::RingEvent &e)
    {
        if (g_alerts.size() >= 8)
            return;
        auto *a = new AlertWindow();
        a->ev = e;
        HWND h = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED, kUiClass, L"WinKlok alert",
                                 WS_POPUP, 0, 0, 100, 100, nullptr, nullptr, ModInst(), a);
        if (!h)
        {
            delete a;
            return;
        }
        a->ownedByWindow = true;
        a->dpi = GetDpiForWindow(h);
        if (!a->dpi)
            a->dpi = 96;
        a->ApplyMaterial();
        g_alerts.push_back(a);
        StopPreviewSound(); // an alert always wins over an alarm-editor preview
        snd::g_ringing = false;
        LayoutAlerts();
        SoundUpdate();
    }

    void PanelWindow::AddMissedAlert(const std::vector<td::MissedItem> &missed)
    {
        if (missed.empty() || g_alerts.size() >= 8)
            return;
        auto *a = new AlertWindow();
        a->missedList = true;
        a->items = missed;
        const int shown = (int)std::min<size_t>(missed.size(), 5);
        a->height = 66.f + 52.f * shown + (missed.size() > 5 ? 22.f : 0.f) + 52.f;
        HWND h = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED, kUiClass, L"WinKlok missed", WS_POPUP, 0, 0,
                                 100, 100, nullptr, nullptr, ModInst(), a);
        if (!h)
        {
            delete a;
            return;
        }
        a->ownedByWindow = true;
        a->dpi = GetDpiForWindow(h);
        if (!a->dpi)
            a->dpi = 96;
        a->ApplyMaterial();
        g_alerts.push_back(a);
        LayoutAlerts(); // silent: no SoundUpdate for a missed-items popup
    }

    // ---------------------------------------------------------------------------------------------
    //  thread
    // ---------------------------------------------------------------------------------------------
    static bool g_uiOk = false;

    static bool InitFactories()
    {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr, (void **)&g_d2d)))
            return false;
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown **)&g_dw)))
            return false;
        g_uiFont = FontExists(L"Segoe UI Variable Text") ? L"Segoe UI Variable Text" : L"Segoe UI";
        rich::g_monoFont = FontExists(L"Cascadia Mono") ? L"Cascadia Mono" : L"Consolas";
        rich::g_mathFont = FontExists(L"Cambria Math") ? L"Cambria Math" : L"Segoe UI Symbol";
        g_iconFont = FontExists(L"Segoe Fluent Icons") ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
        return true;
    }

    static DWORD WINAPI UiThreadProc(LPVOID evt)
    {
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        bool ok = InitFactories() && RegisterUiClass();
        if (ok)
        {
            g_panel = new PanelWindow();
            ok = g_panel->Create();
            if (!ok)
            {
                delete g_panel;
                g_panel = nullptr;
            }
        }
        if (ok)
        {
            g_panelWnd = g_panel->hwnd;
            SetTimer(g_panel->hwnd, TIMER_TICK, 100, nullptr);
        }
        g_uiOk = ok;
        SetEvent((HANDLE)evt);
        if (ok)
        {
            MSG msg;
            while (GetMessageW(&msg, nullptr, 0, 0) > 0)
            {
                HWND ph = g_panel->hwnd;
                if (ph && (msg.hwnd == ph || IsChild(ph, msg.hwnd)))
                {
                    // Ctrl+A selects all in any panel text field (plain EDIT controls do not do this themselves).
                    if (msg.message == WM_KEYDOWN && msg.wParam == 'A' && (GetKeyState(VK_CONTROL) & 0x8000))
                    {
                        wchar_t cls[16] = {};
                        GetClassNameW(msg.hwnd, cls, 16);
                        if (!_wcsicmp(cls, L"Edit"))
                        {
                            SendMessageW(msg.hwnd, EM_SETSEL, 0, -1);
                            continue;
                        }
                    }
                    if (IsDialogMessageW(ph, &msg))
                        continue;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        // teardown
        g_panelWnd = nullptr;
        if (g_panel && g_panel->hwnd)
            g_panel->FlushNote();
        RemoveWheelHook();
        std::vector<AlertWindow *> alerts = g_alerts;
        g_alerts.clear();
        for (auto *a : alerts)
            if (a->hwnd)
                DestroyWindow(a->hwnd);
        snd::Stop();
        snd::g_ringing = false;
        snd::g_waves.clear();
        if (g_panel)
        {
            if (g_panel->hwnd)
                DestroyWindow(g_panel->hwnd);
            delete g_panel;
            g_panel = nullptr;
        }
        rich::Release();
        ReleaseFormats();
        if (g_dw)
        {
            g_dw->Release();
            g_dw = nullptr;
        }
        if (g_d2d)
        {
            g_d2d->Release();
            g_d2d = nullptr;
        }
        if (g_uiClassRegistered)
        {
            UnregisterClassW(kUiClass, ModInst());
            g_uiClassRegistered = false;
        }
        CoUninitialize();
        return 0;
    }

    bool Start()
    {
        HANDLE evt = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!evt)
            return false;
        g_syncStop = CreateEventW(nullptr, TRUE, FALSE, nullptr); // created before any thread that uses it
        g_thread = CreateThread(nullptr, 0, UiThreadProc, evt, 0, &g_threadId);
        if (!g_thread)
        {
            CloseHandle(evt);
            if (g_syncStop)
                CloseHandle(g_syncStop);
            g_syncStop = nullptr;
            return false;
        }
        // The UI thread always signals once its start-up has finished (successfully or not). Never give up
        // early: returning while it still runs would let the module be unloaded under it.
        WaitForSingleObject(evt, INFINITE);
        CloseHandle(evt);
        if (!g_uiOk)
        {
            WaitForSingleObject(g_thread, INFINITE); // join before reporting failure
            CloseHandle(g_thread);
            g_thread = nullptr;
            if (g_syncStop)
                CloseHandle(g_syncStop);
            g_syncStop = nullptr;
            return false;
        }
        return true;
    }

    void Stop()
    {
        if (!g_thread)
            return;
        if (g_syncStop)
            SetEvent(g_syncStop); // interrupt the sync worker first; it winds down while the UI thread stops
        HWND h = g_panelWnd.load();
        if (h)
            PostMessageW(h, WM_UI_QUIT, 0, 0);
        else
            PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        // No timeouts: when Wh_ModUninit returns, no thread of this module may still be running.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        if (g_syncThread)
        {
            WaitForSingleObject(g_syncThread, INFINITE);
            CloseHandle(g_syncThread);
            g_syncThread = nullptr;
        }
        if (g_syncStop)
        {
            CloseHandle(g_syncStop);
            g_syncStop = nullptr;
        }
    }

    void NotifyTaskbarClockPress() { g_clockTogglePendingUntil = GetTickCount64() + 700; }

    void RequestTogglePanel(bool noActivate)
    {
        HWND h = g_panelWnd.load();
        if (h)
            PostMessageW(h, WM_UI_TOGGLE, noActivate ? 1 : 0, 0);
        else
            g_clockTogglePendingUntil = 0;
    }

} // namespace ui

// ============================================================================
//  WINDHAWK ENTRY POINTS
// ============================================================================
BOOL Wh_ModInit()
{
    td::g_eng.Load();
    td::g_set.Load();
    if (!ui::Start())
    {
        Wh_Log(L"could not start the UI thread");
        return FALSE;
    }
    if (!tb::InstallHooks())
    {
        Wh_Log(L"could not install hooks");
        ui::Stop();
        return FALSE;
    }
    Wh_Log(L"Initialised");
    return TRUE;
}

void Wh_ModBeforeUninit()
{
    g_shuttingDown = true;
    tb::CleanupAll();
}

void Wh_ModUninit()
{
    ui::Stop();
}
