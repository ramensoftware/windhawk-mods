// ==WindhawkMod==
// @id              taskbar-battery-time
// @name            Battery time next to the battery icon
// @description     Shows time-to-full (charging) or time-to-empty (on battery) and the percentage right next to the taskbar battery icon
// @version         1.0
// @author          pantr1x
// @github          https://github.com/pantr1x
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lsetupapi -lpowrprof -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Finding the taskbar's XAML tree (GetTaskbarXamlRoot, RunFromWindowThread and
// the path to the battery icon) is adapted from m417z's "Taskbar tray system
// icon tweaks" mod (windhawk-mods, GPL-3.0).

// ==WindhawkModReadme==
/*
# Battery time next to the battery icon

Adds the remaining time and the percentage right next to the battery icon in
the taskbar (the one in the network / volume / battery button). Windows 11
only.

- while **charging**: time until full, e.g. `99% 🔋 1H 21min`
- while **on battery**: time until empty, e.g. `62% 🔋 2H 13min`
- when **full / plugged in**: nothing is added, the icon looks as usual

In **Settings → Battery time position** you choose whether the time goes
**after** the battery icon (`99% 🔋 1H 21min`) or **before** it
(`1H 21min 🔋 99%`). The percentage always goes on the other side. Windows' own
percentage is hidden meanwhile so it is not shown twice. The text slides in
and out smoothly (can be turned off in the settings), and hovering it shows
the long form, e.g. `1,25h remaining` or `13min until full`; **Time format →
Long** shows the long form right in the taskbar.

Plugging in or unplugging shows a time at once: Windows tells the mod about it
right away. Batteries report the new power draw only after a little while, so
until then the mod counts with the draw it remembered from last time (kept
across restarts, also when it had to measure the draw itself). Before it has
ever seen one, it shows a rough guess from the percentage, which quietly turns
into the exact time as soon as the battery reports more. If nothing measurable
comes for 10 minutes (e.g. charging held at a limit), the icon is left as
Windows draws it.

Windows does not report the time-to-full, so this mod computes it from the
power draw and capacity reported by Windows' power manager or the battery
driver, and falls back to measuring how fast the charge changes over time.

The first time the mod is enabled, Windhawk downloads symbols for
`taskbar.dll`, which can take a little while.

If nothing shows up, turn on **Enable logging**, click **Show Log Output**, and
include the lines that start with `[bateria]` when reporting the problem. They
show what the battery reports and what the battery icon is made of.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Layout: timeAfter
  $name: Battery time position
  $description: Where the time goes relative to the battery icon. The percentage goes on the other side.
  $options:
  - timeAfter: "After the battery (99% 🔋 1H 21min)"
  - timeBefore: "Before the battery (1H 21min 🔋 99%)"
- Format: short
  $name: Time format
  $options:
  - short: "Short (1H 15min)"
  - long: "Long (1,25h remaining)"
- Spacing: 4
  $name: Space next to the battery icon (pixels)
- Animate: true
  $name: Slide the text in and out
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <setupapi.h>
#include <powrprof.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <chrono>
#include <atomic>
#include <functional>
#include <limits>
#include <string>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#include <initguid.h> // must precede DEFINE_GUID so the GUID gets storage

using namespace winrt::Windows::UI::Xaml;

// --- estimate core begin ---------------------------------------------------
// Everything from here to "estimate core end" is plain C++ without Windows
// calls: taskbarclock/test/run.sh cuts it out and tests it on its own.

// One reading from all sources. Unknown numbers are negative (rates: 0).
struct PowerInputs {
    // GetSystemPowerStatus – the percentage Windows shows in the taskbar.
    bool haveSps = false;
    int acLine = 255;            // 0 off, 1 on, 255 unknown
    int batteryFlag = 255;       // 8 charging, 128 no battery, 255 unknown
    int percent = -1;            // 0..100
    long long lifeTimeSec = -1;  // Windows' time-to-empty

    // CallNtPowerInformation(SystemBatteryState) – Windows' power manager.
    bool haveSbs = false;
    bool sbsAc = false, sbsPresent = false;
    bool sbsCharging = false, sbsDischarging = false;
    double sbsRemaining = -1, sbsMax = -1; // mWh
    double sbsRate = 0;                    // mW, + charging, - discharging
    long long sbsEstimatedSec = -1;        // time-to-empty

    // Battery driver, summed over all batteries.
    bool haveDrv = false;
    bool drvOnline = false, drvCharging = false, drvDischarging = false;
    double drvCap = -1, drvFull = -1; // mWh
    double drvRate = 0;               // mW, + charging, - discharging
    long long drvEstimatedSec = -1;   // time-to-empty
};

// What to show next to the battery icon.
struct BatteryView {
    int state = 0; // 1 charging, -1 on battery, 0 nothing to add
    int percent = 0;
    std::wstring time;   // "1H 21min"
    std::wstring detail; // "1,25h remaining" / "13min until full" (tooltip)
    bool guess = false;  // only a rough guess from the percentage so far
};

static const double kMaxSec = 72.0 * 3600;
// Without anything measurable for this long, stop guessing and remembering.
static const unsigned long long kGraceMs = 10 * 60 * 1000;
// Right after plugging in or unplugging, batteries report an averaged draw
// that still lags behind; it is trusted over the remembered one after this.
static const unsigned long long kTrustLiveMs = 30 * 1000;
// A draw seen for this long in one state is remembered for the next time.
static const unsigned long long kRememberAfterMs = 60 * 1000;
static bool saneSec(double sec) { return sec > 0 && sec <= kMaxSec; }

// Rough time from the percentage alone, for the first moments after plugging
// in or unplugging when the battery has not reported anything better yet and
// nothing is remembered. A typical laptop charges to 80 % quickly (about
// 1.3 %/min), then slowly (about 0.4 %/min), and lasts about 6 hours.
static double guessSec(int state, double percent) {
    if (state == 1) {
        double min = percent < 80 ? (80 - percent) / 1.33 + 50 : (100 - percent) / 0.4;
        return min * 60;
    }
    return percent / 100.0 * 6 * 3600;
}

// formatTime gives "1H 21min", "2H" or "21min".
static std::wstring formatTime(long long sec) {
    long long total = (sec + 30) / 60;
    if (total < 1) total = 1;
    long long h = total / 60, m = total % 60;
    if (h == 0) return std::to_wstring(m) + L"min";
    if (m == 0) return std::to_wstring(h) + L"H";
    return std::to_wstring(h) + L"H " + std::to_wstring(m) + L"min";
}

// formatLong gives "1,25h remaining", "2h until full" or "13min remaining".
static std::wstring formatLong(long long sec, int state) {
    std::wstring t;
    if (sec >= 3600) {
        long long hundredths = (sec * 100 + 1800) / 3600; // hours, 2 decimals
        std::wstring frac = std::to_wstring(hundredths % 100);
        if (frac.size() < 2) frac = L"0" + frac;
        while (!frac.empty() && frac.back() == L'0') frac.pop_back();
        t = std::to_wstring(hundredths / 100) + (frac.empty() ? L"" : L"," + frac) + L"h";
    } else {
        t = formatTime(sec);
    }
    return t + (state == 1 ? L" until full" : L" remaining");
}

// Remembers what is needed between readings: the current state, a smoothed
// power draw, recent charge samples for when no draw is reported, and the
// last settled draw of each direction so the time can be shown the moment
// the charger is plugged in or pulled out.
struct Estimator {
    struct Sample {
        unsigned long long t;
        double v;
    };
    static const int kMaxSamples = 128;
    Sample hist[kMaxSamples];
    int n = 0;
    int state = 0;
    bool usesCap = false;
    unsigned long long stateSince = 0;
    double rateEma = 0;           // mW, 0 = none yet
    double chargeMemory = 0;      // mW, last settled draw while charging
    double dischargeMemory = 0;   // mW (positive), last settled draw on battery
    bool live = false;            // this reading had a usable draw
    const wchar_t* source = L"-"; // where the last time came from (for the log)

    // Keeps one sample per 10 seconds from the last 15 minutes.
    void push(unsigned long long t, double v) {
        if (n > 0 && t - hist[n - 1].t < 10000) return;
        if (n == kMaxSamples) {
            for (int i = 1; i < n; i++) hist[i - 1] = hist[i];
            n--;
        }
        hist[n++] = {t, v};
        const unsigned long long window = 15ULL * 60 * 1000;
        int drop = 0;
        while (drop < n - 2 && t - hist[drop].t > window) drop++;
        if (drop > 0) {
            for (int i = drop; i < n; i++) hist[i - drop] = hist[i];
            n -= drop;
        }
    }

    // Change per hour, fitted through the samples (least squares), once they
    // span 30 seconds and the value has actually moved.
    bool slope(double* perHour) const {
        if (n < 3 || hist[n - 1].t - hist[0].t < 30000) return false;
        double lo = hist[0].v, hi = hist[0].v;
        for (int i = 1; i < n; i++) {
            if (hist[i].v < lo) lo = hist[i].v;
            if (hist[i].v > hi) hi = hist[i].v;
        }
        if (hi == lo) return false;

        double meanT = 0, meanV = 0;
        for (int i = 0; i < n; i++) {
            meanT += (double)(hist[i].t - hist[0].t) / 3600000.0;
            meanV += hist[i].v;
        }
        meanT /= n;
        meanV /= n;
        double num = 0, den = 0;
        for (int i = 0; i < n; i++) {
            double dt = (double)(hist[i].t - hist[0].t) / 3600000.0 - meanT;
            num += dt * (hist[i].v - meanV);
            den += dt * dt;
        }
        if (den <= 0) return false;
        *perHour = num / den;
        return true;
    }
};

static BatteryView estimate(const PowerInputs& in,
                            unsigned long long now,
                            Estimator& e) {
    BatteryView v;

    bool spsBattery =
        in.haveSps && in.batteryFlag != 255 && !(in.batteryFlag & 128);
    bool present = (in.haveSbs && in.sbsPresent) || in.haveDrv || spsBattery;

    double cap = -1, fullCap = -1;
    if (in.haveSbs && in.sbsRemaining >= 0 && in.sbsMax > 0) {
        cap = in.sbsRemaining;
        fullCap = in.sbsMax;
    } else if (in.haveDrv && in.drvCap >= 0 && in.drvFull > 0) {
        cap = in.drvCap;
        fullCap = in.drvFull;
    }
    bool usesCap = cap >= 0 && fullCap > 0;

    int percent = in.percent;
    if (percent < 0 && usesCap) percent = (int)(cap / fullCap * 100.0 + 0.5);
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    v.percent = percent;
    bool isFull = percent >= 100 || (usesCap && cap >= fullCap);

    // Plugged in? The power manager knows best; ACLineStatus can be 255.
    int ac = -1;
    if (in.haveSbs) ac = in.sbsAc ? 1 : 0;
    else if (in.haveSps && (in.acLine == 0 || in.acLine == 1)) ac = in.acLine;
    else if (in.haveDrv) ac = in.drvOnline ? 1 : 0;

    double rate = 0;
    if (in.haveSbs && in.sbsRate != 0) rate = in.sbsRate;
    else if (in.haveDrv && in.drvRate != 0) rate = in.drvRate;

    bool charging = (in.haveSbs && in.sbsCharging) ||
                    (in.haveDrv && in.drvCharging) ||
                    (spsBattery && (in.batteryFlag & 8));
    bool discharging =
        (in.haveSbs && in.sbsDischarging) || (in.haveDrv && in.drvDischarging);

    int state = 0;
    if (present) {
        if (ac == 0) {
            state = -1;
        } else if (ac == 1) {
            // Plugged in but discharging (battery care) or idle: nothing to add.
            state = ((charging || rate > 0) && !isFull) ? 1 : 0;
        } else if ((charging || rate > 0) && !isFull) {
            state = 1;
        } else if (discharging || rate < 0) {
            state = -1;
        }
    }

    if (state != e.state) { // plugged in / unplugged: start over
        e.state = state;
        e.stateSince = now;
        e.rateEma = 0;
        e.n = 0;
    }
    if (usesCap != e.usesCap) { // samples would mix mWh and percent
        e.usesCap = usesCap;
        e.n = 0;
    }
    e.source = L"-";
    e.live = false;
    if (state == 0) return v;

    double cur = usesCap ? cap : (double)percent;
    double target = usesCap ? fullCap : 100.0;
    e.push(now, cur);

    // A smoothed draw keeps the time from jumping at every reading.
    e.live = (state == 1 && rate > 0) || (state == -1 && rate < 0);
    if (e.live) {
        e.rateEma = e.rateEma == 0 ? rate : e.rateEma + 0.25 * (rate - e.rateEma);
    }
    unsigned long long inState = now - e.stateSince;
    if (e.rateEma != 0 && inState >= kRememberAfterMs) {
        if (state == 1) e.chargeMemory = e.rateEma;
        else e.dischargeMemory = -e.rateEma;
    }

    double memory = state == 1 ? e.chargeMemory : e.dischargeMemory;
    if (inState >= kGraceMs) memory = 0; // e.g. charging held at a limit
    auto fromDraw = [&](double draw) {   // draw in mW, + charging, - on battery
        return state == 1 ? (fullCap - cap) / draw * 3600.0 : cap / -draw * 3600.0;
    };

    // Best source first: Windows' own time-to-empty, the live draw, the
    // measured change of the charge, the draw remembered from last time, and
    // finally a rough guess from the percentage - so there is always a time
    // right away and it gets exact as soon as the battery reports more.
    double sec = -1;
    if (state == -1) { // Windows' own time-to-empty is already smoothed
        const long long windows[] = {
            in.haveSbs ? in.sbsEstimatedSec : -1,
            in.haveSps ? in.lifeTimeSec : -1,
            in.haveDrv ? in.drvEstimatedSec : -1,
        };
        for (long long s : windows) {
            if (saneSec((double)s)) {
                sec = (double)s;
                e.source = L"windows";
                break;
            }
        }
    }
    if (sec < 0 && e.rateEma != 0 && usesCap) {
        // Right after the switch the reported draw is still an average that
        // lags behind; an implausible one waits for a moment.
        double live = e.rateEma < 0 ? -e.rateEma : e.rateEma;
        bool lagging = memory > 0 && inState < kTrustLiveMs &&
                       !(live > memory / 3 && live < memory * 3);
        double s = fromDraw(e.rateEma);
        if (!lagging && saneSec(s)) {
            sec = s;
            e.source = L"rate";
        }
    }
    double perHour = 0;
    if (sec < 0 && e.slope(&perHour)) {
        double s = -1;
        if (state == 1 && perHour > 0) s = (target - cur) / perHour * 3600.0;
        if (state == -1 && perHour < 0) s = cur / -perHour * 3600.0;
        if (saneSec(s)) {
            sec = s;
            e.source = L"history";
            // Batteries that never report a draw: remember the measured one.
            if (usesCap && inState >= kRememberAfterMs) {
                if (state == 1) e.chargeMemory = perHour;
                else e.dischargeMemory = -perHour;
            }
        }
    }
    if (sec < 0 && memory > 0 && usesCap) {
        double s = fromDraw(state * memory);
        if (saneSec(s)) {
            sec = s;
            e.source = L"memory";
        }
    }

    if (sec > 0) {
        v.state = state;
        v.time = formatTime((long long)sec);
        v.detail = formatLong((long long)sec, state);
    } else if (inState < kGraceMs) {
        // Shown like any other time; it quietly turns exact once the battery
        // reports more.
        long long guessed = (long long)guessSec(state, percent);
        v.state = state;
        v.time = formatTime(guessed);
        v.detail = formatLong(guessed, state);
        v.guess = true;
        e.source = L"guess";
    } else {
        // Still nothing measurable (e.g. charging held at a limit): show the
        // icon as Windows draws it rather than a made-up time forever.
        e.source = L"none";
    }
    return v;
}

// --- estimate core end -----------------------------------------------------

// --- battery driver (self-contained, no batclass.h dependency) -------------

DEFINE_GUID(GUID_BATTERY_DEVICE, 0x72631e54, 0x78A4, 0x11d0,
            0xbc, 0xf7, 0x00, 0xaa, 0x00, 0xb7, 0xb3, 0x2a);

#define IOCTL_BATTERY_QUERY_TAG 0x00294040
#define IOCTL_BATTERY_QUERY_INFORMATION 0x00294044
#define IOCTL_BATTERY_QUERY_STATUS 0x0029404C

#define BATTERY_UNKNOWN_RATE 0x80000000
#define BATTERY_UNKNOWN_CAPACITY 0xFFFFFFFF
#define BATTERY_UNKNOWN_TIME 0xFFFFFFFF
#define BATTERY_POWER_ON_LINE 0x00000001
#define BATTERY_DISCHARGING 0x00000002
#define BATTERY_CHARGING 0x00000004

typedef enum {
    BatteryInformation = 0,
    BatteryEstimatedTime = 3,
} BATTERY_QUERY_INFORMATION_LEVEL;

typedef struct {
    ULONG BatteryTag;
    BATTERY_QUERY_INFORMATION_LEVEL InformationLevel;
    LONG AtRate;
} BATTERY_QUERY_INFORMATION_S;

typedef struct {
    ULONG Capabilities;
    UCHAR Technology;
    UCHAR Reserved[3];
    UCHAR Chemistry[4];
    ULONG DesignedCapacity;
    ULONG FullChargedCapacity;
    ULONG DefaultAlert1;
    ULONG DefaultAlert2;
    ULONG CriticalBias;
    ULONG CycleCount;
} BATTERY_INFORMATION_S;

typedef struct {
    ULONG BatteryTag;
    ULONG Timeout;
    ULONG PowerState;
    ULONG LowCapacity;
    ULONG HighCapacity;
} BATTERY_WAIT_STATUS_S;

typedef struct {
    ULONG PowerState;
    ULONG Capacity;
    ULONG Voltage;
    LONG Rate;
} BATTERY_STATUS_S;

// --- reading ---------------------------------------------------------------

static void readDriver(PowerInputs* in) {
    HDEVINFO devs = SetupDiGetClassDevsW(&GUID_BATTERY_DEVICE, NULL, NULL,
                                         DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (devs == INVALID_HANDLE_VALUE) return;

    int batteries = 0;
    double full = 0, cap = 0, rate = 0;
    bool fullKnown = true, capKnown = true;
    long long estimated = -1;

    for (DWORD i = 0; i < 16; i++) {
        SP_DEVICE_INTERFACE_DATA did = {};
        did.cbSize = sizeof(did);
        if (!SetupDiEnumDeviceInterfaces(devs, NULL, &GUID_BATTERY_DEVICE, i, &did))
            break;

        DWORD need = 0;
        SetupDiGetDeviceInterfaceDetailW(devs, &did, NULL, 0, &need, NULL);
        if (need == 0) continue;

        auto detail = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)malloc(need);
        if (!detail) continue;
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        if (SetupDiGetDeviceInterfaceDetailW(devs, &did, detail, need, &need, NULL)) {
            HANDLE h = CreateFileW(detail->DevicePath,
                                   GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (h != INVALID_HANDLE_VALUE) {
                ULONG wait = 0, tag = 0, ret = 0;
                if (DeviceIoControl(h, IOCTL_BATTERY_QUERY_TAG, &wait, sizeof(wait),
                                    &tag, sizeof(tag), &ret, NULL) &&
                    tag != 0) {
                    BATTERY_QUERY_INFORMATION_S q = {tag, BatteryInformation, 0};
                    BATTERY_INFORMATION_S info = {};
                    BATTERY_WAIT_STATUS_S ws = {tag, 0, 0, 0, 0};
                    BATTERY_STATUS_S st = {};
                    if (DeviceIoControl(h, IOCTL_BATTERY_QUERY_INFORMATION, &q, sizeof(q),
                                        &info, sizeof(info), &ret, NULL) &&
                        DeviceIoControl(h, IOCTL_BATTERY_QUERY_STATUS, &ws, sizeof(ws),
                                        &st, sizeof(st), &ret, NULL)) {
                        batteries++;
                        if (st.PowerState & BATTERY_POWER_ON_LINE) in->drvOnline = true;
                        if (st.PowerState & BATTERY_CHARGING) in->drvCharging = true;
                        if (st.PowerState & BATTERY_DISCHARGING) in->drvDischarging = true;
                        if (info.FullChargedCapacity != BATTERY_UNKNOWN_CAPACITY)
                            full += info.FullChargedCapacity;
                        else
                            fullKnown = false;
                        if (st.Capacity != BATTERY_UNKNOWN_CAPACITY)
                            cap += st.Capacity;
                        else
                            capKnown = false;
                        if ((ULONG)st.Rate != BATTERY_UNKNOWN_RATE && st.Rate != 0) {
                            // Some drivers report a positive draw while
                            // discharging; trust the state bits for the sign.
                            LONG r = st.Rate;
                            if ((st.PowerState & BATTERY_DISCHARGING) && r > 0) r = -r;
                            if ((st.PowerState & BATTERY_CHARGING) && r < 0) r = -r;
                            rate += r;
                        }

                        q.InformationLevel = BatteryEstimatedTime;
                        ULONG secs = BATTERY_UNKNOWN_TIME;
                        if (DeviceIoControl(h, IOCTL_BATTERY_QUERY_INFORMATION, &q,
                                            sizeof(q), &secs, sizeof(secs), &ret, NULL) &&
                            secs != BATTERY_UNKNOWN_TIME && secs != 0) {
                            estimated = secs;
                        }
                    }
                }
                CloseHandle(h);
            }
        }
        free(detail);
    }
    SetupDiDestroyDeviceInfoList(devs);

    if (batteries == 0) return;
    in->haveDrv = true;
    if (fullKnown && full > 0) in->drvFull = full;
    if (capKnown) in->drvCap = cap;
    in->drvRate = rate;
    if (batteries == 1) in->drvEstimatedSec = estimated; // can't add up estimates
}

static PowerInputs readInputs() {
    PowerInputs in;

    SYSTEM_POWER_STATUS sps = {};
    if (GetSystemPowerStatus(&sps)) {
        in.haveSps = true;
        in.acLine = sps.ACLineStatus;
        in.batteryFlag = sps.BatteryFlag;
        if (sps.BatteryLifePercent <= 100) in.percent = sps.BatteryLifePercent;
        if (sps.BatteryLifeTime != (DWORD)-1) in.lifeTimeSec = sps.BatteryLifeTime;
    }

    SYSTEM_BATTERY_STATE sbs = {};
    if (CallNtPowerInformation(SystemBatteryState, NULL, 0, &sbs, sizeof(sbs)) == 0) {
        in.haveSbs = true;
        in.sbsAc = sbs.AcOnLine;
        in.sbsPresent = sbs.BatteryPresent;
        in.sbsCharging = sbs.Charging;
        in.sbsDischarging = sbs.Discharging;
        if (sbs.MaxCapacity != 0 && sbs.MaxCapacity != BATTERY_UNKNOWN_CAPACITY)
            in.sbsMax = sbs.MaxCapacity;
        if (sbs.RemainingCapacity != BATTERY_UNKNOWN_CAPACITY)
            in.sbsRemaining = sbs.RemainingCapacity;
        LONG r = (LONG)sbs.Rate; // documented as signed despite the DWORD
        if ((DWORD)r != BATTERY_UNKNOWN_RATE && r != 0) {
            if (sbs.Discharging && r > 0) r = -r;
            if (sbs.Charging && r < 0) r = -r;
            in.sbsRate = r;
        }
        if (sbs.EstimatedTime != BATTERY_UNKNOWN_TIME && sbs.EstimatedTime != 0)
            in.sbsEstimatedSec = sbs.EstimatedTime;
    }

    readDriver(&in);
    return in;
}

// --- shared state ----------------------------------------------------------

// The worker thread writes g_view, the taskbar thread reads it; g_lock guards
// it together with g_settings.
static SRWLOCK g_lock = SRWLOCK_INIT;
static BatteryView g_view;

struct {
    bool timeAfter = true; // "99% 🔋 1H 21min"; false = "1H 21min 🔋 99%"
    int spacing = 4;
    bool animate = true;
    bool longFormat = false; // "1,25h remaining" instead of "1H 15min"
} g_settings;

static void loadSettings() {
    PCWSTR layout = Wh_GetStringSetting(L"Layout");
    bool timeAfter = !(layout && wcscmp(layout, L"timeBefore") == 0);
    Wh_FreeStringSetting(layout);
    int spacing = Wh_GetIntSetting(L"Spacing");
    if (spacing < 0) spacing = 0;
    if (spacing > 40) spacing = 40;
    bool animate = Wh_GetIntSetting(L"Animate") != 0;
    PCWSTR format = Wh_GetStringSetting(L"Format");
    bool longFormat = format && wcscmp(format, L"long") == 0;
    Wh_FreeStringSetting(format);

    AcquireSRWLockExclusive(&g_lock);
    g_settings.timeAfter = timeAfter;
    g_settings.spacing = spacing;
    g_settings.animate = animate;
    g_settings.longFormat = longFormat;
    ReleaseSRWLockExclusive(&g_lock);
}

// --- getting to the taskbar's XAML -----------------------------------------

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

// Only looks up addresses in taskbar.dll, nothing gets redirected.
static bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"[bateria] failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

// Must run on the taskbar thread.
static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetPropW(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x48;
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"[bateria] unsupported TaskbarHost::FrameHeight");
        }
    }

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

// Runs proc on the thread that owns hWnd and waits until it is done.
static bool RunFromWindowThread(HWND hWnd,
                                RunFromWindowThreadProc_t proc,
                                void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_taskbar-battery-time");

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

static FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

static FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

static FrameworkElement FindChildByClassName(FrameworkElement element,
                                             PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// --- the battery icon ------------------------------------------------------

// Path to the battery icon in the network / volume / battery button:
// SystemTray.SystemTrayFrame > #SystemTrayFrameGrid > #ControlCenterButton >
// Grid > #ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter >
// #SystemTrayIcon > #ContainerGrid > #ContentGrid >
// SystemTray.BatteryIconContent > #ContainerGrid > StackPanel.
// That StackPanel holds a Grid with the battery glyph and, on newer builds,
// Windows' own percentage TextBlock.
static Controls::StackPanel FindBatteryStack(XamlRoot xamlRoot) {
    FrameworkElement icons = xamlRoot.Content().try_as<FrameworkElement>();
    if (!(icons &&
          (icons = FindChildByClassName(icons, L"SystemTray.SystemTrayFrame")) &&
          (icons = FindChildByName(icons, L"SystemTrayFrameGrid")) &&
          (icons = FindChildByName(icons, L"ControlCenterButton")) &&
          (icons = FindChildByClassName(icons,
                                        L"Windows.UI.Xaml.Controls.Grid")) &&
          (icons = FindChildByName(icons, L"ContentPresenter")) &&
          (icons = FindChildByClassName(
               icons, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
          (icons = FindChildByClassName(
               icons, L"Windows.UI.Xaml.Controls.StackPanel")))) {
        return nullptr;
    }

    FrameworkElement battery = nullptr;
    EnumChildElements(icons, [&battery](FrameworkElement presenter) {
        FrameworkElement c = presenter;
        if ((c = FindChildByName(c, L"SystemTrayIcon")) &&
            (c = FindChildByName(c, L"ContainerGrid")) &&
            (c = FindChildByName(c, L"ContentGrid")) &&
            (c = FindChildByClassName(c, L"SystemTray.BatteryIconContent"))) {
            battery = c;
            return true;
        }
        return false;
    });
    if (!battery) {
        return nullptr;
    }

    FrameworkElement stack = battery;
    if ((stack = FindChildByName(stack, L"ContainerGrid")) &&
        (stack = FindChildByClassName(stack,
                                      L"Windows.UI.Xaml.Controls.StackPanel"))) {
        return stack.as<Controls::StackPanel>();
    }
    return nullptr;
}

static const wchar_t kTimeName[] = L"BateriaTimeText";
static const wchar_t kPctName[] = L"BateriaPercentText";
// Names used by versions up to 3.5 (removed on unload just in case).
static const wchar_t kOldLeftName[] = L"BateriaLeftText";
static const wchar_t kOldRightName[] = L"BateriaRightText";

static bool IsOurs(FrameworkElement const& fe) {
    auto name = fe.Name();
    return name == kTimeName || name == kPctName || name == kOldLeftName ||
           name == kOldRightName;
}

namespace Anim = winrt::Windows::UI::Xaml::Media::Animation;

// Only touched on the taskbar thread.
static winrt::weak_ref<Controls::StackPanel> g_loggedStack;

// Our text slides in and out: its MaxWidth grows from 0 to the width of the
// text (pushing the neighbors aside smoothly) while it fades in, and the other
// way round when it goes; Windows' own percentage does the opposite.
//
// No flashing: before a storyboard starts, the element's own (local) values
// are set to where the animation starts, so a frame drawn before its first
// tick already looks right. Once it has finished, the end values are made
// local too and the storyboard is dropped ("settled"). The storyboards have
// no event handlers, so none of our code runs from them.
struct Fade {
    Anim::Storyboard storyboard{nullptr};
    winrt::weak_ref<FrameworkElement> element;
    bool toShown = false;
    ULONGLONG started = 0;
    int ms = 0;
};
static Fade g_fadeTime, g_fadePct, g_fadeWinPct;
static winrt::weak_ref<Controls::StackPanel> g_shownStack;
static winrt::weak_ref<Controls::TextBlock> g_hiddenPercent; // Windows' own
static bool g_timeShown = false; // our time is shown (or on its way in)
static bool g_pctShown = false;  // our percentage is shown (or on its way in)
static const int kAnimMs = 350;
// When the last animation ends; the worker redraws right then, so the end
// look is settled at once instead of up to 10 seconds later.
static std::atomic<ULONGLONG> g_animUntil{0};

static const double kInf = std::numeric_limits<double>::infinity();

// The look of a fully shown element is its default (no local values), the look
// of a hidden one is zero width and opacity.
static void SetLook(FrameworkElement el, bool shown) {
    auto obj = el.as<DependencyObject>();
    if (shown) {
        obj.ClearValue(UIElement::OpacityProperty());
        obj.ClearValue(FrameworkElement::MaxWidthProperty());
        if (el.try_as<Controls::TextBlock>()) {
            obj.ClearValue(Controls::TextBlock::TextWrappingProperty());
        }
    } else {
        el.Opacity(0);
        el.MaxWidth(0);
    }
}

// Ends a fade for good: its end look becomes the element's own.
static void Settle(Fade& f, bool force) {
    if (!f.storyboard) return;
    if (!force && GetTickCount64() - f.started < (ULONGLONG)f.ms + 50) return;
    if (auto el = f.element.get()) SetLook(el, f.toShown);
    f.storyboard.Stop();
    f.storyboard = nullptr;
}

// Drops a fade.
static void Reset(Fade& f) {
    if (f.storyboard) f.storyboard.Stop();
    f.storyboard = nullptr;
    f.element = nullptr;
}

// Drops all fades; Windows' percentage gets its own look back whether it is
// hidden or still on its way back (unload, or Windows rebuilt the icon).
static void ForgetFades() {
    if (auto percent = g_fadeWinPct.element.get()) SetLook(percent, true);
    if (auto percent = g_hiddenPercent.get()) SetLook(percent, true);
    Reset(g_fadeTime);
    Reset(g_fadePct);
    Reset(g_fadeWinPct);
    g_timeShown = g_pctShown = false;
    g_hiddenPercent = nullptr;
}

// Width el needs, measured in place with the width limit lifted for a moment
// (nothing is drawn in between).
static double NaturalWidth(FrameworkElement el) {
    double limit = el.MaxWidth();
    el.MaxWidth(kInf);
    el.Measure(winrt::Windows::Foundation::Size{std::numeric_limits<float>::infinity(),
                                                std::numeric_limits<float>::infinity()});
    double width = el.DesiredSize().Width;
    el.MaxWidth(limit);
    return width + 1;
}

// Width of "100%" in the font of Windows' percentage: its text can change
// right while it slides back (99 % -> 100 %), and a too small limit would cut
// it to "10…". A larger limit is harmless, the text takes only what it needs.
static double WidestPercentWidth(Controls::TextBlock tb) {
    Controls::TextBlock m;
    m.Text(L"100%");
    m.FontSize(tb.FontSize());
    m.FontFamily(tb.FontFamily());
    m.FontWeight(tb.FontWeight());
    m.Padding(tb.Padding());
    const float inf = std::numeric_limits<float>::infinity();
    m.Measure(winrt::Windows::Foundation::Size{inf, inf});
    Thickness margin = tb.Margin();
    return m.DesiredSize().Width + margin.Left + margin.Right + 2;
}

// Slides el in (show) or out, starting from wherever it is right now.
// minWidth raises the width it slides in to.
static void StartFade(Fade& f, FrameworkElement el, bool show, int ms,
                      double minWidth = 0) {
    // A narrowing text must be cut, never wrapped: a wrapped "100%" turns
    // into a crooked column of characters and changes the height.
    if (auto tb = el.try_as<Controls::TextBlock>()) tb.TextWrapping(TextWrapping::NoWrap);
    double fromWidth = el.ActualWidth();
    double fromOpacity = el.Opacity();
    // Freeze the current look as local values, then drop the old storyboard:
    // nothing changes on screen.
    el.Opacity(fromOpacity);
    el.MaxWidth(fromWidth);
    if (f.storyboard) f.storyboard.Stop();
    double toWidth = 0;
    if (show) {
        toWidth = NaturalWidth(el);
        if (toWidth < minWidth) toWidth = minWidth;
    }

    Anim::Storyboard sb;
    auto add = [&](PCWSTR property, double from, double to, bool dependent) {
        Anim::DoubleAnimation a;
        a.From(from);
        a.To(to);
        a.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(ms)));
        Anim::CubicEase ease;
        ease.EasingMode(Anim::EasingMode::EaseInOut);
        a.EasingFunction(ease);
        a.EnableDependentAnimation(dependent); // MaxWidth moves the layout
        Anim::Storyboard::SetTarget(a, el);
        Anim::Storyboard::SetTargetProperty(a, property);
        sb.Children().Append(a);
    };
    add(L"MaxWidth", fromWidth, toWidth, true);
    add(L"Opacity", fromOpacity, show ? 1.0 : 0.0, false);
    sb.FillBehavior(Anim::FillBehavior::HoldEnd);
    sb.Begin();

    f.storyboard = sb;
    f.element = el;
    f.toShown = show;
    f.started = GetTickCount64();
    f.ms = ms;
    ULONGLONG until = f.started + ms + 80;
    if (until > g_animUntil.load()) g_animUntil = until;
}

// Logs what the battery icon is made of, once per icon, so a different layout
// on another Windows build can be diagnosed from the log.
static void LogStack(Controls::StackPanel stack) {
    if (g_loggedStack.get() == stack) return;
    g_loggedStack = stack;

    Wh_Log(L"[bateria] battery icon found, orientation=%d",
           (int)stack.Orientation());
    for (auto child : stack.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (!fe) continue;
        std::wstring text;
        if (auto tb = fe.try_as<Controls::TextBlock>()) text = tb.Text();
        Wh_Log(L"[bateria]   %s #%s '%s'", winrt::get_class_name(fe).c_str(),
               fe.Name().c_str(), text.c_str());
    }
}

// Puts everything back as Windows draws it (on unload).
static void RestoreAll(Controls::StackPanel stack) {
    ForgetFades();
    g_shownStack = nullptr;
    if (!stack) return;
    auto children = stack.Children();
    for (uint32_t i = children.Size(); i-- > 0;) {
        auto fe = children.GetAt(i).try_as<FrameworkElement>();
        if (fe && IsOurs(fe)) children.RemoveAt(i);
    }
}

// Creates one of our TextBlocks. The font and color follow Windows' own
// percentage text when there is one (including theme changes).
static Controls::TextBlock MakeText(PCWSTR name, Controls::TextBlock pattern) {
    Controls::TextBlock tb;
    tb.Name(name);
    SetLook(tb, false); // starts hidden, slides in from there
    tb.VerticalAlignment(VerticalAlignment::Center);
    tb.TextWrapping(TextWrapping::NoWrap);
    if (pattern) {
        auto follow = [&](DependencyProperty property, PCWSTR path) {
            Data::Binding binding;
            binding.Source(pattern);
            binding.Path(PropertyPath(path));
            tb.SetBinding(property, binding);
        };
        follow(Controls::TextBlock::FontSizeProperty(), L"FontSize");
        follow(Controls::TextBlock::FontFamilyProperty(), L"FontFamily");
        follow(Controls::TextBlock::FontWeightProperty(), L"FontWeight");
        follow(Controls::TextBlock::ForegroundProperty(), L"Foreground");
    } else {
        tb.FontSize(12);
    }
    return tb;
}

static bool SetText(Controls::TextBlock tb, const std::wstring& text) {
    if (std::wstring_view(tb.Text()) == text) return false;
    tb.Text(text);
    return true;
}

static void SetTooltip(Controls::TextBlock tb, const std::wstring& text) {
    auto current = Controls::ToolTipService::GetToolTip(tb).try_as<winrt::hstring>();
    if (!current || std::wstring_view(*current) != text) {
        Controls::ToolTipService::SetToolTip(tb, winrt::box_value(winrt::hstring(text)));
    }
}

static int IndexIn(Controls::UIElementCollection const& children,
                   UIElement const& el) {
    uint32_t i = 0;
    return children.IndexOf(el, i) ? (int)i : -1;
}

// Makes sure el sits right next to the glyph on the given side (moved at
// once; only happens when the layout setting changes).
static void KeepBeside(Controls::UIElementCollection const& children,
                       FrameworkElement const& el, FrameworkElement const& glyph,
                       bool after) {
    int i = IndexIn(children, el), g = IndexIn(children, glyph);
    if (i >= 0 && (after ? i > g : i < g)) return;
    if (i >= 0) children.RemoveAt(i);
    g = IndexIn(children, glyph);
    children.InsertAt(after ? g + 1 : g, el);
}

// Puts the time on one side of the battery glyph and the percentage on the
// other, sliding them in and out. When Windows' own percentage already sits
// on the right side, it is simply kept - nothing needs to move. Safe to call
// repeatedly: our TextBlocks are found by name and only a change between
// shown and hidden animates.
static void ApplyBattery(Controls::StackPanel stack,
                         const BatteryView& v,
                         bool timeAfter,
                         int spacing,
                         bool animate) {
    LogStack(stack);
    int ms = animate ? kAnimMs : 0;

    if (g_shownStack.get() != stack) { // new icon (e.g. Explorer rebuilt it)
        ForgetFades();
        g_shownStack = stack;
    }
    Settle(g_fadeTime, false);
    Settle(g_fadePct, false);
    Settle(g_fadeWinPct, false);

    auto children = stack.Children();
    FrameworkElement glyph = nullptr;
    Controls::TextBlock timeTb = nullptr, pctTb = nullptr, winPct = nullptr;
    for (auto child : children) {
        auto fe = child.try_as<FrameworkElement>();
        if (!fe) continue;
        auto name = fe.Name();
        if (name == kTimeName) {
            timeTb = fe.as<Controls::TextBlock>();
        } else if (name == kPctName) {
            pctTb = fe.as<Controls::TextBlock>();
        } else if (IsOurs(fe)) {
            // leftover of an older version: ignore
        } else if (auto tb = fe.try_as<Controls::TextBlock>()) {
            if (!winPct) winPct = tb;
        } else if (!glyph &&
                   winrt::get_class_name(fe) == L"Windows.UI.Xaml.Controls.Grid") {
            glyph = fe;
        }
    }

    auto giveBackWindowsPercent = [&] {
        if (auto hidden = g_hiddenPercent.get()) {
            StartFade(g_fadeWinPct, hidden, true, ms, WidestPercentWidth(hidden));
        }
        g_hiddenPercent = nullptr;
    };

    if (v.state == 0) { // full or plugged in: the icon as Windows draws it
        if (g_timeShown && timeTb) StartFade(g_fadeTime, timeTb, false, ms);
        if (g_pctShown && pctTb) StartFade(g_fadePct, pctTb, false, ms);
        g_timeShown = g_pctShown = false;
        giveBackWindowsPercent();
        return;
    }

    if (!glyph) {
        Wh_Log(L"[bateria] battery glyph not found, not adding text");
        return;
    }

    // The percentage goes opposite to the time. Windows' own one can stay if
    // it is shown and already on that side.
    bool pctAfter = !timeAfter;
    bool keepWindows = winPct && winPct.Visibility() == Visibility::Visible &&
                       (IndexIn(children, winPct) > IndexIn(children, glyph)) == pctAfter;

    if (!timeTb) {
        timeTb = MakeText(kTimeName, winPct);
        g_timeShown = false;
    }
    KeepBeside(children, timeTb, glyph, timeAfter);
    bool timeChanged = SetText(timeTb, v.time);
    SetTooltip(timeTb, v.detail);
    // Padding rather than Margin, so the gap goes away with the width.
    double gap = spacing;
    timeTb.Padding(timeAfter ? Thickness{gap, 0, 0, 0} : Thickness{0, 0, gap, 0});

    if (!g_timeShown) {
        StartFade(g_fadeTime, timeTb, true, ms);
        g_timeShown = true;
    } else if (timeChanged && g_fadeTime.storyboard) {
        StartFade(g_fadeTime, timeTb, true, ms); // slide on to the new width
    }

    if (keepWindows) {
        if (g_pctShown && pctTb) StartFade(g_fadePct, pctTb, false, ms);
        g_pctShown = false;
        if (g_hiddenPercent.get() == winPct) giveBackWindowsPercent();
        return;
    }

    if (!pctTb) {
        pctTb = MakeText(kPctName, winPct);
        g_pctShown = false;
    }
    KeepBeside(children, pctTb, glyph, pctAfter);
    bool pctChanged = SetText(pctTb, std::to_wstring(v.percent) + L"%");
    SetTooltip(pctTb, v.detail);
    pctTb.Padding(pctAfter ? Thickness{gap, 0, 0, 0} : Thickness{0, 0, gap, 0});

    if (!g_pctShown) {
        StartFade(g_fadePct, pctTb, true, ms);
        g_pctShown = true;
    } else if (pctChanged && g_fadePct.storyboard) {
        StartFade(g_fadePct, pctTb, true, ms);
    }
    // Our percentage replaces Windows' own one while we show the time (also
    // when Windows recreates its percentage text meanwhile).
    if (winPct && g_hiddenPercent.get() != winPct) {
        StartFade(g_fadeWinPct, winPct, false, ms);
        g_hiddenPercent = winPct;
    }
}

struct ApplyParam {
    HWND hTaskbarWnd;
    bool remove;
};

static void WINAPI ApplyOnTaskbarThread(void* param) {
    auto& p = *(ApplyParam*)param;
    try {
        XamlRoot root = GetTaskbarXamlRoot(p.hTaskbarWnd);
        Controls::StackPanel stack = root ? FindBatteryStack(root) : nullptr;
        if (p.remove) {
            RestoreAll(stack); // stops the animations even without the icon
            return;
        }
        if (!root) {
            Wh_Log(L"[bateria] taskbar XAML not found (yet)");
            return;
        }
        if (!stack) {
            Wh_Log(L"[bateria] battery icon not found");
            return;
        }

        AcquireSRWLockShared(&g_lock);
        BatteryView v = g_view;
        bool timeAfter = g_settings.timeAfter;
        int spacing = g_settings.spacing;
        bool animate = g_settings.animate;
        if (g_settings.longFormat) v.time = v.detail;
        ReleaseSRWLockShared(&g_lock);

        ApplyBattery(stack, v, timeAfter, spacing, animate);
    } catch (...) {
        Wh_Log(L"[bateria] XAML error %08X", (unsigned)winrt::to_hresult());
    }
}

static void UpdateTaskbar(bool remove) {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) return;
    ApplyParam p{hTaskbarWnd, remove};
    RunFromWindowThread(hTaskbarWnd, ApplyOnTaskbarThread, &p);
}

// --- worker thread ---------------------------------------------------------

// Reading the battery goes through the driver and must not run on Explorer's UI
// thread, so a small worker does it: every half second right after plugging
// in or unplugging, every 2 seconds otherwise, and at once when Windows
// announces a power change. The taskbar is only touched when the text changes,
// and every 10 seconds in case Windows rebuilt the battery icon.
static HANDLE g_stopEvent = NULL;
static HANDLE g_wakeEvent = NULL;
static HANDLE g_thread = NULL;

static void logReading(const PowerInputs& in, const BatteryView& v,
                       const Estimator& e) {
    Wh_Log(L"[bateria] sps: ac=%d flag=%d pct=%d life=%d | "
           L"sbs: ok=%d ac=%d chg=%d dis=%d rem=%d max=%d rate=%d est=%d | "
           L"drv: ok=%d on=%d chg=%d dis=%d cap=%d full=%d rate=%d est=%d",
           in.acLine, in.batteryFlag, in.percent, (int)in.lifeTimeSec,
           in.haveSbs, in.sbsAc, in.sbsCharging, in.sbsDischarging,
           (int)in.sbsRemaining, (int)in.sbsMax, (int)in.sbsRate,
           (int)in.sbsEstimatedSec, in.haveDrv, in.drvOnline, in.drvCharging,
           in.drvDischarging, (int)in.drvCap, (int)in.drvFull, (int)in.drvRate,
           (int)in.drvEstimatedSec);
    Wh_Log(L"[bateria] -> state=%d pct=%d time='%s' from=%s live=%d "
           L"draw=%d memory=%d/%d samples=%d",
           v.state, v.percent, v.time.c_str(), e.source, e.live, (int)e.rateEma,
           (int)e.chargeMemory, (int)e.dischargeMemory, e.n);
}

// The remembered draws survive restarts in the mod's storage.
static void loadMemory(Estimator& e) {
    int charge = Wh_GetIntValue(L"ChargeDrawMw", 0);
    int discharge = Wh_GetIntValue(L"DischargeDrawMw", 0);
    e.chargeMemory = charge > 0 ? charge : 0;
    e.dischargeMemory = discharge > 0 ? discharge : 0;
}

static void saveMemory(const Estimator& e) {
    Wh_SetIntValue(L"ChargeDrawMw", (int)e.chargeMemory);
    Wh_SetIntValue(L"DischargeDrawMw", (int)e.dischargeMemory);
}

static bool differs(double a, double b) {
    double d = a > b ? a - b : b - a;
    return d > 0.05 * (a > b ? a : b);
}

static DWORD WINAPI worker(LPVOID) {
    Estimator est;
    loadMemory(est);
    double savedCharge = est.chargeMemory, savedDischarge = est.dischargeMemory;

    HANDLE events[2] = {g_stopEvent, g_wakeEvent};
    std::wstring shown; // what the taskbar shows now
    ULONGLONG lastApply = 0, lastLog = 0, lastSave = 0;
    unsigned long long timedSince = 0;
    bool timeLogged = true, liveLogged = true;
    bool force = true;
    for (;;) {
        PowerInputs in = readInputs();
        ULONGLONG now = GetTickCount64();
        BatteryView v = estimate(in, now, est);

        AcquireSRWLockExclusive(&g_lock);
        g_view = v;
        ReleaseSRWLockExclusive(&g_lock);

        std::wstring key = std::to_wstring(v.state) + L"|" +
                           std::to_wstring(v.percent) + L"|" + v.time;
        bool changed = key != shown;
        // An animation just ended: redraw so its end look is settled now.
        ULONGLONG animUntil = g_animUntil.load();
        bool settleDue = animUntil != 0 && now >= animUntil;
        if (changed || force || settleDue || now - lastApply >= 10000) {
            if (settleDue) g_animUntil.compare_exchange_strong(animUntil, 0);
            UpdateTaskbar(false);
            shown = key;
            lastApply = now;
        }
        if (changed || force || now - lastLog >= 30000) {
            logReading(in, v, est);
            lastLog = now;
        }

        // After plugging in / unplugging, log how long the time took and
        // where it came from - that is what tells why it was slow.
        if (est.stateSince != timedSince) {
            timedSince = est.stateSince;
            timeLogged = liveLogged = est.state == 0;
        }
        if (!timeLogged && v.state != 0 && !v.guess) {
            Wh_Log(L"[bateria] state %d: first time '%s' after %d ms (from %s)",
                   est.state, v.time.c_str(), (int)(now - est.stateSince),
                   est.source);
            timeLogged = true;
        }
        if (!liveLogged && est.live) {
            Wh_Log(L"[bateria] state %d: first live draw after %d ms", est.state,
                   (int)(now - est.stateSince));
            liveLogged = true;
        }

        if (now - lastSave >= 60000 && (differs(est.chargeMemory, savedCharge) ||
                                        differs(est.dischargeMemory, savedDischarge))) {
            saveMemory(est);
            savedCharge = est.chargeMemory;
            savedDischarge = est.dischargeMemory;
            lastSave = now;
        }

        unsigned long long inState = now - est.stateSince;
        bool settling = est.state != 0 && inState < kRememberAfterMs;
        DWORD wait = settling ? 500 : 2000;
        ULONGLONG pending = g_animUntil.load(), after = GetTickCount64();
        if (pending != 0) {
            ULONGLONG left = pending > after ? pending - after : 0;
            if (left < wait) wait = (DWORD)left + 1;
        }
        DWORD r = WaitForMultipleObjects(2, events, FALSE, wait);
        if (r == WAIT_OBJECT_0) break;       // stop requested
        force = r == WAIT_OBJECT_0 + 1;      // settings or power changed: apply now
    }
    if (differs(est.chargeMemory, savedCharge) ||
        differs(est.dischargeMemory, savedDischarge)) {
        saveMemory(est);
    }
    return 0;
}

// --- power notifications ---------------------------------------------------

// Windows calls back the moment the charger is plugged in or pulled out (and
// when the percentage changes); the worker then reads the battery right away.
// The functions are looked up at run time since not every MinGW declares them.
static const GUID kGuidAcDcPowerSource = {
    0x5d3e9a59, 0xe9d5, 0x4b00, {0xa6, 0xbd, 0xff, 0x34, 0xff, 0x51, 0x65, 0x48}};
static const GUID kGuidBatteryPercentage = {
    0xa7ad8041, 0xb45a, 0x4cae, {0x87, 0xa3, 0xee, 0xcb, 0xb4, 0x68, 0xa9, 0xe1}};

// Same layout as DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS.
struct PowerNotifySubscribe {
    ULONG(CALLBACK* callback)(PVOID context, ULONG type, PVOID setting);
    PVOID context;
};
static const DWORD kDeviceNotifyCallback = 2; // DEVICE_NOTIFY_CALLBACK

using PowerSettingRegisterNotification_t = DWORD(WINAPI*)(LPCGUID, DWORD, HANDLE, PVOID*);
using PowerSettingUnregisterNotification_t = DWORD(WINAPI*)(PVOID);

static PowerNotifySubscribe g_powerSubscribe;
static PVOID g_powerNotify[2];

static ULONG CALLBACK OnPowerChange(PVOID, ULONG, PVOID) {
    HANDLE wake = g_wakeEvent;
    if (wake) SetEvent(wake);
    return 0;
}

template <typename T>
static T powrprofProc(const char* name) {
    HMODULE powrprof = GetModuleHandleW(L"powrprof.dll");
    return powrprof ? reinterpret_cast<T>(
                          reinterpret_cast<void*>(GetProcAddress(powrprof, name)))
                    : nullptr;
}

static void RegisterPowerNotifications() {
    auto reg = powrprofProc<PowerSettingRegisterNotification_t>(
        "PowerSettingRegisterNotification");
    if (!reg) {
        Wh_Log(L"[bateria] no power notifications, polling only");
        return;
    }
    g_powerSubscribe = {OnPowerChange, nullptr};
    const GUID* guids[2] = {&kGuidAcDcPowerSource, &kGuidBatteryPercentage};
    for (int i = 0; i < 2; i++) {
        DWORD err = reg(guids[i], kDeviceNotifyCallback, (HANDLE)&g_powerSubscribe,
                        &g_powerNotify[i]);
        if (err != ERROR_SUCCESS) {
            g_powerNotify[i] = nullptr;
            Wh_Log(L"[bateria] power notification %d failed: %u", i, err);
        }
    }
}

static void UnregisterPowerNotifications() {
    auto unreg = powrprofProc<PowerSettingUnregisterNotification_t>(
        "PowerSettingUnregisterNotification");
    for (PVOID& h : g_powerNotify) {
        if (h && unreg) unreg(h);
        h = nullptr;
    }
}

static void StopWorker() {
    if (!g_thread) return;
    SetEvent(g_stopEvent);
    // The worker may be waiting for the taskbar thread. In case this runs on
    // that very thread, keep handling sent messages while waiting.
    ULONGLONG deadline = GetTickCount64() + 10000;
    for (;;) {
        ULONGLONG now = GetTickCount64();
        if (now >= deadline) break;
        DWORD r = MsgWaitForMultipleObjects(1, &g_thread, FALSE,
                                            (DWORD)(deadline - now), QS_SENDMESSAGE);
        if (r != WAIT_OBJECT_0 + 1) break; // worker ended, timeout or error
        MSG msg;
        PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE); // delivers sent messages
    }
    CloseHandle(g_thread);
    g_thread = NULL;
}

// --- Windhawk entry points -------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"[bateria] init");
    loadSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"[bateria] taskbar.dll symbols not found");
        return FALSE;
    }

    g_stopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_wakeEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!g_stopEvent || !g_wakeEvent) return FALSE;
    g_thread = CreateThread(NULL, 0, worker, NULL, 0, NULL);
    if (!g_thread) return FALSE;
    RegisterPowerNotifications();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[bateria] settings changed");
    loadSettings();
    if (g_wakeEvent) SetEvent(g_wakeEvent); // apply right away
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"[bateria] before uninit");
    UnregisterPowerNotifications();
    StopWorker();
    UpdateTaskbar(true); // remove our text, give Windows its percentage back
}

void Wh_ModUninit() {
    Wh_Log(L"[bateria] uninit");
    if (g_thread) { // Windhawk without Wh_ModBeforeUninit
        UnregisterPowerNotifications();
        StopWorker();
        UpdateTaskbar(true);
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = NULL;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = NULL;
    }
}
