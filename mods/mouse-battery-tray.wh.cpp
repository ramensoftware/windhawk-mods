// ==WindhawkMod==
// @id              mouse-battery-tray
// @name            Mouse Battery Tray
// @description     Shows the battery percentage of wireless mice (Attack Shark, MCHOSE, ATK/VXE, Lamzu, WLmouse, Razer, Logitech, Bluetooth and more) in the Windows 11 tray
// @version         1.0.0
// @author          xa0xs
// @github          https://github.com/xa0xs
// @homepage        https://github.com/xa0xs/mouse-battery-tray
// @include         windhawk.exe
// @compilerOptions -lsetupapi -lhid -lgdi32 -lshell32 -ladvapi32 -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mouse Battery Tray

Shows the battery level of your wireless mouse as a number (or a battery gauge)
in the Windows 11 notification area. No vendor app has to be running: the mod
asks the mouse the same way the vendor's **web driver / online software** does
(WebHID), directly over USB HID.

## Supported mice

| Family | How it is read | Examples |
|---|---|---|
| Attack Shark Ultra series and other "DriverCore" web-driver mice | feature report `0x83`, the web driver's own `getBatPer` | **R5 Ultra**, R11 Ultra, X11 Ultra |
| WLmouse, Lamzu, G-Wolves, Glorious (Sinowealth), MCHOSE A5 Pro Max | the same `0x83` feature exchange | Beast X, Maya X, HSK Pro, Model O |
| Attack Shark (Beken dongle, VID 1D57) | pushed status frames | X3, X3 Max, X6, X11, R1 |
| MCHOSE RealTek / YJX chips | inverted feature report `0x06`, pushed `0x13` frames, `AA 30` reply | M7 Ultra, L7 Pro, A7 V2 Ultra, G7 |
| Compx / Nordic 52840 | 17-byte report `0x08`, command `0x04` | ATK, VXE R1, VGN, Pulsar, Scyrox, Dareu, Hitscan |
| Compx / Nordic 54L15 | 64-byte report `0x08`, command `0x72` | ATK Zero |
| Razer | 90-byte feature report, class `0x07` | Viper, DeathAdder, Basilisk... |
| Logitech | HID++ 2.0 (features `0x1004`, `0x1000`, `0x1001`) | Lightspeed, Bolt, Unifying |
| Any Bluetooth mouse | the level Windows itself reads | R5 Ultra in Bluetooth mode, MX Master... |

Unknown 2.4 GHz receivers are probed with the two most common read-only
battery queries (`0x83` and Compx `0x04`). If your mouse is still not found,
right-click the icon, choose **Save diagnostics**, and open an issue with
that file, or describe the protocol yourself under **Custom devices**.

## Tips

- Close the vendor's web driver tab while you read the level. Both apps share
  the same HID channel, so they can briefly step on each other's replies.
- A grey number means the mouse is asleep or out of range. It shows the last
  level that was read.
- Blue means it is charging.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pollInterval: 60
  $name: Poll interval (seconds)
  $description: How often the battery is read (10 - 3600)
- iconMode: perDevice
  $name: Tray icons
  $options:
  - perDevice: One icon per mouse
  - lowest: One icon, for the mouse with the lowest battery
- iconStyle: number
  $name: Icon style
  $options:
  - number: Percentage number
  - battery: Battery gauge
- colorByLevel: true
  $name: Color by level
  $description: Green / amber / red by charge, blue while charging. Off uses the taskbar text color (red when low)
- lowBattery: 20
  $name: Low battery notification (%)
  $description: 0 turns the notification off
- showWhenNone: true
  $name: Show an icon when no mouse is found
- bluetooth: true
  $name: Read Bluetooth mice
- logitech: true
  $name: Read Logitech mice (HID++)
- razer: true
  $name: Read Razer mice
- probeUnknown: true
  $name: Probe unknown 2.4 GHz receivers
  $description: Sends the common read-only battery queries to unknown receivers that also carry a mouse
- allDevices: false
  $name: Also show keyboards, headsets and other devices
- customDevices:
  - - name: ""
      $name: Display name
    - vid: ""
      $name: Vendor ID (hex, e.g. 1D57)
    - pid: ""
      $name: Product ID (hex, empty = any)
    - usagePage: ""
      $name: HID usage page (hex, empty = any vendor page FFxx)
    - method: feature
      $name: Method
      $options:
      - feature: Feature report (SetFeature, then GetFeature)
      - output: Output report, then read the input report
      - listen: Only listen for pushed input reports
    - request: ""
      $name: Request bytes in hex, starting with the report ID
    - match: ""
      $name: The reply must contain (offset=hex, e.g. 1=A1,6=83)
    - levelByte: 8
      $name: Battery byte offset (report ID = byte 0)
    - chargingByte: -1
      $name: Charging byte offset (-1 = none)
    - levelScale: 100
      $name: Value of a full battery (100, 255 or 10)
  $name: Custom devices
  $description: Teach the mod a mouse it does not know yet. See docs/ADDING_A_MOUSE.md
*/
// ==/WindhawkModSettings==

////////////////////////////////////////////////////////////////////////////////
// Part 1: protocol parsers.
//
// Pure functions with no Windows dependency, so tests/parsers_test.cpp can
// check them on any OS against frames captured from real devices. Every
// buffer is laid out as Windows returns it: byte 0 is the report ID (0 when
// the device does not number its reports).

#include <cstdint>
#include <cstring>

namespace mbt {

struct Parsed {
    bool ok = false;        // the frame was a battery answer
    int level = -1;         // 0..100, -1 = not in this frame
    bool charging = false;
    bool asleep = false;    // answered, but the mouse itself is asleep / off
};

static inline Parsed Asleep() {
    Parsed p;
    p.ok = true;
    p.asleep = true;
    return p;
}

// --- "0x83" feature exchange ------------------------------------------------
// The Attack Shark R5 Ultra web driver (xvalleyinno DriverCore), WLmouse,
// Lamzu, G-Wolves, Glorious/Sinowealth and the MCHOSE A5 Pro Max all send the
// 65-byte feature report 00 | 00 00 02 02 00 83 00.. and read back
// 00 | A1 00 02 02 00 83 <charging> <level>. A1 = awake, A2 = waking,
// A4 = asleep. The web driver's getBatPer also accepts the frame without the
// leading report ID, so both layouts are parsed here.
static const int kF83Len = 65;

static inline void BuildF83(uint8_t* req /* kF83Len */) {
    memset(req, 0, kF83Len);
    req[3] = 0x02;  // route: the mouse (behind the receiver)
    req[4] = 0x02;  // command type
    req[6] = 0x83;  // battery
}

static inline Parsed ParseF83(const uint8_t* r, int n) {
    Parsed p;
    for (int at = 1; at >= 0; at--) {
        if (n < at + 8) {
            continue;
        }
        uint8_t status = r[at];
        if (status == 0xA2 || status == 0xA4) {
            return Asleep();
        }
        if (status == 0xA1 && r[at + 3] == 0x02 && r[at + 5] == 0x83) {
            int level = r[at + 7];
            if (level > 100) {
                return p;
            }
            p.ok = true;
            p.level = level;
            p.charging = r[at + 6] != 0;
            return p;
        }
    }
    return p;
}

// --- Compx / Nordic 52840 ---------------------------------------------------
// 17-byte report 0x08, command 0x04. All 17 bytes sum to 0x55.
static const int kCompxLen = 17;

static inline void BuildCompx(uint8_t* req /* kCompxLen */) {
    memset(req, 0, kCompxLen);
    req[0] = 0x08;
    req[1] = 0x04;
    uint8_t sum = 0;
    for (int i = 0; i < 16; i++) {
        sum += req[i];
    }
    req[16] = (uint8_t)(0x55 - sum);
}

static inline Parsed ParseCompx(const uint8_t* r, int n) {
    Parsed p;
    if (n < kCompxLen || (r[0] != 0x08 && r[0] != 0x09) || r[1] != 0x04) {
        return p;
    }
    uint8_t sum = 0;
    for (int i = 0; i < kCompxLen; i++) {
        sum += r[i];
    }
    if (sum != 0x55 || r[6] > 100) {
        return p;
    }
    bool wired = r[7] != 0;
    if (r[6] == 0 && !wired) {
        return Asleep();
    }
    p.ok = true;
    p.level = r[6];
    p.charging = wired && r[6] < 100;
    return p;
}

// --- Compx / Nordic 54L15 (ATK Zero) ----------------------------------------
static const int kN54Len = 64;

static inline void BuildN54(uint8_t* req /* kN54Len */, bool cable) {
    memset(req, 0, kN54Len);
    req[0] = 0x08;
    req[1] = 0x7D;
    req[2] = 0x72;
    req[3] = 0x02;
    req[5] = cable ? 0x00 : 0x01;  // link index
    req[6] = 0x07;                 // battery
    req[7] = 0x01;
}

static inline Parsed ParseN54(const uint8_t* r, int n, bool cable) {
    Parsed p;
    if (n < 10 || r[0] != 0x08 || r[1] != 0x72 || r[5] != 0x07) {
        return p;
    }
    if (r[2] != 0x00) {
        return Asleep();
    }
    if (r[7] > 100) {
        return p;
    }
    p.ok = true;
    p.level = r[7];
    p.charging = cable && r[7] < 100;
    return p;
}

// --- Attack Shark / Inphic Beken dongle (VID 1D57) --------------------------
// Pushed every ~2 s: 03 <model> 40 <sub> <level>. Model 0x10 (X6/R1) sends
// the level in steps of ten and has no charging state. Sub 0x03 = charging,
// 0x02 = full (the X3 Max, model 0x4E, uses 0x02 for "charging" instead).
static inline Parsed ParseBeken(const uint8_t* r, int n) {
    Parsed p;
    if (n < 5 || r[0] != 0x03 || r[2] != 0x40) {
        return p;
    }
    uint8_t model = r[1], sub = r[3], raw = r[4];
    if (model == 0x10) {
        int level = raw * 10;
        if (level < 1 || level > 100) {
            return p;
        }
        p.ok = true;
        p.level = level;
        return p;
    }
    if (sub == 0x03 || (sub == 0x02 && model == 0x4E)) {
        p.ok = true;
        p.charging = true;
        return p;
    }
    if (sub == 0x02) {
        p.ok = true;
        p.level = 100;
        p.charging = true;
        return p;
    }
    if (raw < 1 || raw > 100) {
        return p;
    }
    p.ok = true;
    p.level = raw;
    return p;
}

// --- MCHOSE RealTek (VID 5253 / 3837) ---------------------------------------
// Feature report 0x11 (21 bytes) or 0x12 (65 bytes), command 0x06, every
// payload byte inverted. The decoded reply is <vid LE> <model LE> .. <flags>
// <level> <charging> at payload offsets 0, 2, 8, 9, 10.
static inline void BuildMchoseStatus(uint8_t* req, int len, uint8_t reportId) {
    memset(req, 0xFF, len);
    req[0] = reportId;
    req[1] = 0x06 ^ 0xFF;
}

static inline Parsed ParseMchoseStatus(const uint8_t* r, int n) {
    Parsed p;
    if (n < 13 || (r[0] != 0x11 && r[0] != 0x12) || r[1] != (0x06 ^ 0xFF)) {
        return p;
    }
    uint8_t pay[11];
    for (int i = 0; i < 11; i++) {
        pay[i] = r[2 + i] ^ 0xFF;
    }
    int vid = pay[0] | (pay[1] << 8);
    if (vid != 0x5253 && vid != 0x3837) {
        return p;
    }
    if (pay[9] > 100) {
        return p;
    }
    // A sleeping mouse makes the receiver answer with an all-zero payload,
    // which fails the vid check above.
    p.ok = true;
    p.level = pay[9];
    p.charging = pay[10] != 0;
    return p;
}

// Pushed report 0x13, sub-type 0x1D, XOR 0xFF: level at 5, wired flag at 4.
static inline Parsed ParseMchosePush(const uint8_t* r, int n) {
    Parsed p;
    if (n < 6 || r[0] != 0x13 || r[1] != 0x1D) {
        return p;
    }
    int level = r[5] ^ 0xFF;
    if (level > 100) {
        return p;
    }
    bool wired = (r[4] ^ 0xFF) != 0;
    p.ok = true;
    p.level = level;
    p.charging = wired && level < 100;
    return p;
}

// MCHOSE G7 (YJX chip, A8A5:2255): reply AA 30 .. level at +8, charging +9.
static const uint8_t kG7Request[9] = {0x00, 0x55, 0x30, 0xA5, 0x0B,
                                      0x2E, 0x01, 0x01, 0x01};

static inline Parsed ParseG7(const uint8_t* r, int n) {
    Parsed p;
    for (int at = 0; at <= 1; at++) {
        if (n < at + 10 || r[at] != 0xAA || r[at + 1] != 0x30) {
            continue;
        }
        if (r[at + 8] > 100) {
            return p;
        }
        p.ok = true;
        p.level = r[at + 8];
        p.charging = r[at + 9] != 0;
        return p;
    }
    return p;
}

// --- Razer --------------------------------------------------------------
// 90-byte message behind report ID 0: [0] status, [1] transaction id,
// [5] size, [6] class, [7] id, [8..] args, [88] XOR of 2..87.
static const int kRazerLen = 91;  // report ID + 90

static inline void BuildRazer(uint8_t* req /* kRazerLen */, uint8_t tid,
                              uint8_t cls, uint8_t id) {
    memset(req, 0, kRazerLen);
    uint8_t* m = req + 1;
    m[1] = tid;
    m[5] = 0x02;
    m[6] = cls;
    m[7] = id;
    uint8_t crc = 0;
    for (int i = 2; i < 88; i++) {
        crc ^= m[i];
    }
    m[88] = crc;
}

// Returns the status byte (2 = OK, 1 = busy, 4 = asleep, 5 = unsupported),
// or -1 when the frame answers another command. *value gets args[1].
static inline int ParseRazer(const uint8_t* r, int n, uint8_t cls, uint8_t id,
                             int* value) {
    const uint8_t* m = r;
    if (n >= kRazerLen) {
        m = r + 1;
    } else if (n < kRazerLen - 1) {
        return -1;
    }
    if (m[6] != cls || m[7] != id) {
        return m[0] == 0x01 ? 1 : -1;
    }
    *value = m[9];
    return m[0];
}

// --- Logitech HID++ 2.0 -------------------------------------------------
// Battery voltage (feature 0x1001) to percent, Solaar's discharge curve.
static inline int LogitechVoltageToPercent(int mv) {
    static const int kCurve[][2] = {
        {4186, 100}, {4067, 90}, {3989, 80}, {3922, 70}, {3859, 60},
        {3811, 50},  {3778, 40}, {3751, 30}, {3717, 20}, {3671, 10},
        {3646, 5},   {3579, 2},  {3500, 0},
    };
    const int count = sizeof(kCurve) / sizeof(kCurve[0]);
    if (mv >= kCurve[0][0]) {
        return 100;
    }
    for (int i = 1; i < count; i++) {
        if (mv >= kCurve[i][0]) {
            int v0 = kCurve[i][0], p0 = kCurve[i][1];
            int v1 = kCurve[i - 1][0], p1 = kCurve[i - 1][1];
            return p0 + (mv - v0) * (p1 - p0) / (v1 - v0);
        }
    }
    return 0;
}

// --- Custom devices -----------------------------------------------------
struct ByteMatch {
    int offset;
    uint8_t value;
};

static inline Parsed ParseCustom(const uint8_t* r, int n, const ByteMatch* m,
                                 int matchCount, int levelByte,
                                 int chargingByte, int levelScale) {
    Parsed p;
    for (int i = 0; i < matchCount; i++) {
        if (m[i].offset < 0 || m[i].offset >= n || r[m[i].offset] != m[i].value) {
            return p;
        }
    }
    if (levelByte < 0 || levelByte >= n) {
        return p;
    }
    if (levelScale <= 0) {
        levelScale = 100;
    }
    int level = r[levelByte] * 100 / levelScale;
    if (level > 100) {
        return p;
    }
    p.ok = true;
    p.level = level;
    if (chargingByte >= 0 && chargingByte < n) {
        p.charging = r[chargingByte] != 0;
    }
    return p;
}

}  // namespace mbt

#ifndef MBT_PARSER_TEST

////////////////////////////////////////////////////////////////////////////////
// Part 2: Windows HID access.

#include <windows.h>

#include <cfgmgr32.h>
#include <dbt.h>
#include <devpropdef.h>
#include <setupapi.h>
#include <shellapi.h>

extern "C" {
#include <hidsdi.h>
#include <hidpi.h>
}

#include <algorithm>
#include <cstdarg>
#include <cwchar>
#include <cwctype>
#include <map>
#include <string>
#include <vector>

#ifndef MBT_VERSION
#define MBT_VERSION L"1.0.0"
#endif

namespace mbt {

static std::wstring Fmt(const wchar_t* format, ...) {
    wchar_t buf[2048];
    va_list args;
    va_start(args, format);
    int n = _vsnwprintf(buf, ARRAYSIZE(buf) - 1, format, args);
    va_end(args);
    buf[ARRAYSIZE(buf) - 1] = L'\0';
    if (n < 0) {
        n = (int)wcslen(buf);
    }
    return std::wstring(buf, std::min<size_t>(n, ARRAYSIZE(buf) - 1));
}

static std::wstring Hex(const uint8_t* d, int n, int max = 24) {
    std::wstring s;
    for (int i = 0; i < n && i < max; i++) {
        s += Fmt(L"%02X ", d[i]);
    }
    if (n > max) {
        s += L"..";
    }
    return s;
}

// Diagnostics of the latest poll; written to a file on request.
static SRWLOCK g_diagLock = SRWLOCK_INIT;
static std::wstring g_diagNext;
static std::wstring g_diagLast;

static void Diag(const std::wstring& line) {
    g_diagNext += line;
    g_diagNext += L"\r\n";
}

////////////////////////////////////////////////////////////////////////////////
// Settings

struct CustomDevice {
    std::wstring name;
    int vid = -1, pid = -1, usagePage = -1;
    std::wstring method;
    std::vector<uint8_t> request;
    std::vector<ByteMatch> match;
    int levelByte = 8, chargingByte = -1, levelScale = 100;
};

struct Settings {
    int pollSeconds = 60;
    bool lowestOnly = false;
    bool batteryStyle = false;
    bool colorByLevel = true;
    int lowBattery = 20;
    bool showWhenNone = true;
    bool bluetooth = true;
    bool logitech = true;
    bool razer = true;
    bool probeUnknown = true;
    bool allDevices = false;
    std::vector<CustomDevice> custom;
};

static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static Settings g_settings;

static Settings GetSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings s = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return s;
}

static std::wstring ReadString(PCWSTR name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

static std::wstring ReadStringIdx(PCWSTR format, int index) {
    PCWSTR v = Wh_GetStringSetting(format, index);
    std::wstring s = v ? v : L"";
    Wh_FreeStringSetting(v);
    return s;
}

static int ParseHexNumber(const std::wstring& s) {
    std::wstring t;
    for (wchar_t c : s) {
        if (iswxdigit(c)) {
            t += c;
        } else if (c == L'x' || c == L'X') {
            t.clear();  // "0x" prefix
        }
    }
    if (t.empty()) {
        return -1;
    }
    return (int)wcstoul(t.c_str(), nullptr, 16);
}

static std::vector<uint8_t> ParseHexBytes(const std::wstring& s) {
    std::vector<uint8_t> out;
    int nibble = -1;
    for (wchar_t c : s) {
        int v = -1;
        if (c >= L'0' && c <= L'9') v = c - L'0';
        else if (c >= L'a' && c <= L'f') v = c - L'a' + 10;
        else if (c >= L'A' && c <= L'F') v = c - L'A' + 10;
        if (v < 0) {
            if (nibble >= 0) {  // a lone digit before a separator
                out.push_back((uint8_t)nibble);
                nibble = -1;
            }
            continue;
        }
        if (nibble < 0) {
            nibble = v;
        } else {
            out.push_back((uint8_t)(nibble * 16 + v));
            nibble = -1;
        }
    }
    if (nibble >= 0) {
        out.push_back((uint8_t)nibble);
    }
    return out;
}

static std::vector<ByteMatch> ParseMatch(const std::wstring& s) {
    std::vector<ByteMatch> out;
    size_t pos = 0;
    while (pos < s.size()) {
        size_t end = s.find_first_of(L",; ", pos);
        if (end == std::wstring::npos) {
            end = s.size();
        }
        std::wstring item = s.substr(pos, end - pos);
        size_t eq = item.find(L'=');
        if (eq != std::wstring::npos) {
            int off = _wtoi(item.substr(0, eq).c_str());
            int val = ParseHexNumber(item.substr(eq + 1));
            if (val >= 0 && val <= 0xFF) {
                out.push_back({off, (uint8_t)val});
            }
        }
        pos = end + 1;
    }
    return out;
}

static void LoadSettings() {
    Settings s;
    s.pollSeconds = std::clamp(Wh_GetIntSetting(L"pollInterval"), 10, 3600);
    s.lowestOnly = ReadString(L"iconMode") == L"lowest";
    s.batteryStyle = ReadString(L"iconStyle") == L"battery";
    s.colorByLevel = Wh_GetIntSetting(L"colorByLevel") != 0;
    s.lowBattery = std::clamp(Wh_GetIntSetting(L"lowBattery"), 0, 100);
    s.showWhenNone = Wh_GetIntSetting(L"showWhenNone") != 0;
    s.bluetooth = Wh_GetIntSetting(L"bluetooth") != 0;
    s.logitech = Wh_GetIntSetting(L"logitech") != 0;
    s.razer = Wh_GetIntSetting(L"razer") != 0;
    s.probeUnknown = Wh_GetIntSetting(L"probeUnknown") != 0;
    s.allDevices = Wh_GetIntSetting(L"allDevices") != 0;

    for (int i = 0; i < 32; i++) {
        CustomDevice d;
        d.vid = ParseHexNumber(ReadStringIdx(L"customDevices[%d].vid", i));
        if (d.vid < 0) {
            break;
        }
        d.name = ReadStringIdx(L"customDevices[%d].name", i);
        d.pid = ParseHexNumber(ReadStringIdx(L"customDevices[%d].pid", i));
        d.usagePage =
            ParseHexNumber(ReadStringIdx(L"customDevices[%d].usagePage", i));
        d.method = ReadStringIdx(L"customDevices[%d].method", i);
        d.request =
            ParseHexBytes(ReadStringIdx(L"customDevices[%d].request", i));
        d.match = ParseMatch(ReadStringIdx(L"customDevices[%d].match", i));
        d.levelByte = Wh_GetIntSetting(L"customDevices[%d].levelByte", i);
        d.chargingByte = Wh_GetIntSetting(L"customDevices[%d].chargingByte", i);
        d.levelScale = Wh_GetIntSetting(L"customDevices[%d].levelScale", i);
        if (d.name.empty()) {
            d.name = Fmt(L"Custom mouse %04X", d.vid);
        }
        s.custom.push_back(d);
    }

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = s;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

////////////////////////////////////////////////////////////////////////////////
// HID enumeration and I/O

static const DEVPROPKEY kKeyContainerId = {
    {0x8c7ed206, 0x3f8a, 0x4827, {0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c}},
    2};
static const DEVPROPKEY kKeyFriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};
static const DEVPROPKEY kKeyDeviceDesc = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    2};
// The battery level Windows reads from Bluetooth devices (BLE Battery
// Service, HFP), shown in Settings > Bluetooth & devices.
static const DEVPROPKEY kKeyBtBattery = {
    {0x104ea319, 0x6ee2, 0x4701, {0xbd, 0x47, 0x8d, 0xdb, 0xf4, 0x25, 0xbb, 0xe5}},
    2};
static const DEVPROPKEY kKeyIsConnected = {
    {0x83da6326, 0x97a6, 0x4088, {0x94, 0x53, 0xa1, 0x92, 0x3f, 0x57, 0x3b, 0x29}},
    15};

struct HidCol {
    std::wstring path;
    uint16_t vid = 0, pid = 0;
    uint16_t usagePage = 0, usage = 0;
    int inLen = 0, outLen = 0, featLen = 0;
    std::wstring product;
    GUID container{};
};

static bool IsVendorPage(uint16_t page) {
    return page >= 0xFF00;
}

static bool IsMouse(const HidCol& c) {
    return c.usagePage == 0x0001 && c.usage == 0x0002;
}

static std::vector<HidCol> EnumerateHid() {
    std::vector<HidCol> out;
    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);
    HDEVINFO di = SetupDiGetClassDevsW(&hidGuid, nullptr, nullptr,
                                       DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (di == INVALID_HANDLE_VALUE) {
        return out;
    }
    SP_DEVICE_INTERFACE_DATA ifd{sizeof(ifd)};
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(di, nullptr, &hidGuid, i, &ifd);
         i++) {
        DWORD size = 0;
        SetupDiGetDeviceInterfaceDetailW(di, &ifd, nullptr, 0, &size, nullptr);
        if (!size) {
            continue;
        }
        std::vector<BYTE> buf(size);
        auto* detail = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)buf.data();
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        SP_DEVINFO_DATA dd{sizeof(dd)};
        if (!SetupDiGetDeviceInterfaceDetailW(di, &ifd, detail, size, nullptr,
                                              &dd)) {
            continue;
        }
        HidCol c;
        c.path = detail->DevicePath;
        DEVPROPTYPE type;
        SetupDiGetDevicePropertyW(di, &dd, &kKeyContainerId, &type,
                                  (BYTE*)&c.container, sizeof(GUID), nullptr, 0);

        HANDLE h = CreateFileW(c.path.c_str(), 0,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                               OPEN_EXISTING, 0, nullptr);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
        }
        HIDD_ATTRIBUTES attr{sizeof(attr)};
        if (HidD_GetAttributes(h, &attr)) {
            c.vid = attr.VendorID;
            c.pid = attr.ProductID;
        }
        PHIDP_PREPARSED_DATA pp = nullptr;
        if (HidD_GetPreparsedData(h, &pp)) {
            HIDP_CAPS caps{};
            if (HidP_GetCaps(pp, &caps) == HIDP_STATUS_SUCCESS) {
                c.usagePage = caps.UsagePage;
                c.usage = caps.Usage;
                c.inLen = caps.InputReportByteLength;
                c.outLen = caps.OutputReportByteLength;
                c.featLen = caps.FeatureReportByteLength;
            }
            HidD_FreePreparsedData(pp);
        }
        wchar_t name[128] = {};
        if (HidD_GetProductString(h, name, sizeof(name) - sizeof(wchar_t))) {
            c.product = name;
            while (!c.product.empty() && iswspace(c.product.back())) {
                c.product.pop_back();
            }
        }
        CloseHandle(h);
        out.push_back(c);
    }
    SetupDiDestroyDeviceInfoList(di);
    return out;
}

class Hid {
   public:
    explicit Hid(const HidCol& c) : col_(c) {}
    Hid(const Hid&) = delete;
    Hid& operator=(const Hid&) = delete;
    ~Hid() {
        if (h_ != INVALID_HANDLE_VALUE) {
            CloseHandle(h_);
        }
        if (ev_) {
            CloseHandle(ev_);
        }
    }

    // Read/write access is needed for input and output reports; feature
    // reports also work on a handle without access rights, which is the only
    // kind Windows gives out for the mouse collection itself.
    bool Open(bool needReadWrite) {
        h_ = CreateFileW(col_.path.c_str(), GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                         OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
        if (h_ == INVALID_HANDLE_VALUE && !needReadWrite) {
            h_ = CreateFileW(col_.path.c_str(), 0,
                             FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                             OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
        }
        if (h_ == INVALID_HANDLE_VALUE) {
            Diag(Fmt(L"    open failed (%lu)", GetLastError()));
            return false;
        }
        ev_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        return ev_ != nullptr;
    }

    const HidCol& col() const { return col_; }

    void Flush() { HidD_FlushQueue(h_); }

    // Windows wants at least FeatureReportByteLength bytes, so shorter
    // reports are padded with zeros (as hidapi does).
    bool SetFeature(const uint8_t* data, int len) {
        int n = std::max(len, col_.featLen);
        std::vector<uint8_t> b(n, 0);
        memcpy(b.data(), data, len);
        if (!HidD_SetFeature(h_, b.data(), n)) {
            Diag(Fmt(L"    SetFeature failed (%lu): %ls", GetLastError(),
                     Hex(data, len, 12).c_str()));
            return false;
        }
        return true;
    }

    int GetFeature(uint8_t reportId, uint8_t* out, int cap) {
        int n = std::max(col_.featLen, 2);
        std::vector<uint8_t> b(n, 0);
        b[0] = reportId;
        if (!HidD_GetFeature(h_, b.data(), n)) {
            return 0;
        }
        int copy = std::min(n, cap);
        memcpy(out, b.data(), copy);
        return copy;
    }

    bool Write(const uint8_t* data, int len) {
        int n = std::max(len, col_.outLen);
        std::vector<uint8_t> b(n, 0);
        memcpy(b.data(), data, len);
        OVERLAPPED ov{};
        ov.hEvent = ev_;
        ResetEvent(ev_);
        DWORD written = 0;
        if (!WriteFile(h_, b.data(), n, &written, &ov)) {
            if (GetLastError() != ERROR_IO_PENDING) {
                Diag(Fmt(L"    write failed (%lu)", GetLastError()));
                return false;
            }
            if (WaitForSingleObject(ev_, 1000) != WAIT_OBJECT_0) {
                CancelIoEx(h_, &ov);
                GetOverlappedResult(h_, &ov, &written, TRUE);
                Diag(L"    write timed out");
                return false;
            }
            if (!GetOverlappedResult(h_, &ov, &written, FALSE)) {
                return false;
            }
        }
        return true;
    }

    // One input report, or 0 bytes after timeoutMs.
    int Read(uint8_t* out, int cap, int timeoutMs) {
        int n = std::max(col_.inLen, 2);
        std::vector<uint8_t> b(n, 0);
        OVERLAPPED ov{};
        ov.hEvent = ev_;
        ResetEvent(ev_);
        DWORD got = 0;
        if (!ReadFile(h_, b.data(), n, &got, &ov)) {
            if (GetLastError() != ERROR_IO_PENDING) {
                return 0;
            }
            if (WaitForSingleObject(ev_, std::max(timeoutMs, 1)) !=
                WAIT_OBJECT_0) {
                CancelIoEx(h_, &ov);
                GetOverlappedResult(h_, &ov, &got, TRUE);
                return 0;
            }
            if (!GetOverlappedResult(h_, &ov, &got, FALSE)) {
                return 0;
            }
        }
        int copy = std::min<int>(got, cap);
        memcpy(out, b.data(), copy);
        return copy;
    }

   private:
    HidCol col_;
    HANDLE h_ = INVALID_HANDLE_VALUE;
    HANDLE ev_ = nullptr;
};

// Reads input reports until `accept` returns true or the time is up.
template <typename F>
static bool ReadUntil(Hid& h, int timeoutMs, F accept) {
    ULONGLONG deadline = GetTickCount64() + timeoutMs;
    uint8_t buf[512];
    while (true) {
        ULONGLONG now = GetTickCount64();
        if (now >= deadline) {
            return false;
        }
        int n = h.Read(buf, sizeof(buf), (int)(deadline - now));
        if (n > 0 && accept(buf, n)) {
            return true;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Devices and drivers

struct Reading {
    std::wstring key;
    std::wstring name;
    std::wstring source;
    int level = -1;
    bool charging = false;
    bool asleep = false;
};

struct Group {
    uint16_t vid = 0, pid = 0;
    std::vector<HidCol> cols;
    std::wstring product;
    bool hasMouse = false;
};

static std::wstring GroupName(const Group& g, const wchar_t* fallback) {
    if (!g.product.empty()) {
        return g.product;
    }
    return Fmt(L"%ls (%04X:%04X)", fallback, g.vid, g.pid);
}

static Reading MakeReading(const Group& g, const wchar_t* driver,
                           const wchar_t* fallbackName, const Parsed& p) {
    Reading r;
    r.key = Fmt(L"%ls/%04X:%04X", driver, g.vid, g.pid);
    r.name = GroupName(g, fallbackName);
    r.source = driver;
    r.level = p.level;
    r.charging = p.charging;
    r.asleep = p.asleep;
    return r;
}

static bool VidIn(uint16_t vid, std::initializer_list<uint16_t> list) {
    return std::find(list.begin(), list.end(), vid) != list.end();
}

// Brands with their own protocols: the generic probes never write to them.
static bool IsProbeExcluded(uint16_t vid) {
    return VidIn(vid, {0x046D, 0x1532, 0x1038, 0x1B1C, 0x0B05, 0x045E, 0x05AC,
                       0x054C, 0x057E, 0x28DE, 0x0951, 0x03F0, 0x1E71, 0x3057,
                       0x8087, 0x8086, 0x0BDA, 0x413C});
}

static std::vector<const HidCol*> Cols(const Group& g, bool (*pred)(const HidCol&)) {
    std::vector<const HidCol*> out;
    for (auto& c : g.cols) {
        if (pred(c)) {
            out.push_back(&c);
        }
    }
    return out;
}

// --- 0x83 feature exchange (Attack Shark Ultra web driver and friends) -----

static bool IsF83Col(const HidCol& c) {
    return IsVendorPage(c.usagePage) && c.featLen == kF83Len;
}

static bool HasF83(const Group& g) {
    return !Cols(g, IsF83Col).empty();
}

static bool ClaimF83Known(const Group& g, const Settings&) {
    return VidIn(g.vid, {0x36A7, 0x373E, 0x33E4, 0x258A, 0x2023, 0x37B0}) &&
           HasF83(g);
}

static bool ClaimF83Probe(const Group& g, const Settings& s) {
    return s.probeUnknown && g.hasMouse && !IsProbeExcluded(g.vid) && HasF83(g) &&
           !ClaimF83Known(g, s);
}

static bool ReadF83(const Group& g, const Settings&, std::vector<Reading>& out) {
    for (const HidCol* c : Cols(g, IsF83Col)) {
        Diag(Fmt(L"  0x83 feature on %04X:%04X", c->usagePage, c->usage));
        Hid h(*c);
        if (!h.Open(false)) {
            continue;
        }
        uint8_t req[kF83Len];
        BuildF83(req);
        bool asleep = false;
        for (int send = 0; send < 2; send++) {
            if (!h.SetFeature(req, kF83Len)) {
                break;
            }
            Sleep(100);
            // The receiver relays the question over the air; until the
            // answer arrives the feature buffer holds something else.
            for (int i = 0; i < 8; i++) {
                uint8_t r[kF83Len];
                int n = h.GetFeature(0, r, sizeof(r));
                Parsed p = ParseF83(r, n);
                if (i == 0 || p.ok) {
                    Diag(L"    reply: " + Hex(r, n, 12));
                }
                if (p.ok && !p.asleep) {
                    out.push_back(MakeReading(g, L"0x83", L"Mouse", p));
                    return true;
                }
                if (p.asleep) {
                    asleep = true;
                    break;  // A2/A4: ask again, the mouse may wake up
                }
                Sleep(50);
            }
            Sleep(150);
        }
        if (asleep) {
            out.push_back(MakeReading(g, L"0x83", L"Mouse", Asleep()));
            return true;
        }
    }
    return false;
}

// --- Compx / Nordic 52840 --------------------------------------------------

static bool IsCompxCol(const HidCol& c) {
    return c.usagePage == 0xFF02 && c.usage == 0x0002 &&
           c.outLen >= kCompxLen && c.inLen >= kCompxLen;
}

static bool ClaimCompxKnown(const Group& g, const Settings&) {
    bool vid = VidIn(g.vid, {0x373B, 0x3554, 0x3710, 0x260D, 0x3770, 0x33E4}) ||
               (g.vid == 0x1B1C && g.pid == 0x2B2A);
    return vid && !Cols(g, IsCompxCol).empty();
}

static bool ClaimCompxProbe(const Group& g, const Settings& s) {
    return s.probeUnknown && g.hasMouse && !IsProbeExcluded(g.vid) &&
           !Cols(g, IsCompxCol).empty() && !ClaimCompxKnown(g, s);
}

static bool ReadCompx(const Group& g, const Settings&, std::vector<Reading>& out) {
    for (const HidCol* c : Cols(g, IsCompxCol)) {
        Diag(L"  Compx 0x04 on FF02:0002");
        Hid h(*c);
        if (!h.Open(true)) {
            continue;
        }
        uint8_t req[kCompxLen];
        BuildCompx(req);
        for (int attempt = 0; attempt < 2; attempt++) {
            h.Flush();
            if (!h.Write(req, kCompxLen)) {
                break;
            }
            Parsed got;
            ReadUntil(h, 1000, [&](const uint8_t* r, int n) {
                Parsed p = ParseCompx(r, n);
                if (p.ok) {
                    Diag(L"    reply: " + Hex(r, n, 17));
                    got = p;
                }
                return p.ok;
            });
            if (got.ok) {
                out.push_back(MakeReading(g, L"Compx", L"Compx mouse", got));
                return true;
            }
        }
    }
    return false;
}

// --- Compx / Nordic 54L15 (ATK Zero) ---------------------------------------

static bool IsN54Col(const HidCol& c) {
    return c.usagePage == 0xFF05 && c.usage == 0x0001 && c.outLen >= kN54Len;
}

static bool ClaimN54(const Group& g, const Settings&) {
    return g.vid == 0x373B && !Cols(g, IsN54Col).empty();
}

static bool ReadN54(const Group& g, const Settings&, std::vector<Reading>& out) {
    for (const HidCol* c : Cols(g, IsN54Col)) {
        Hid h(*c);
        if (!h.Open(true)) {
            continue;
        }
        for (int cable = 0; cable <= 1; cable++) {
            Diag(Fmt(L"  Nordic 54L15 0x72, link %d", cable ? 0 : 1));
            uint8_t req[kN54Len];
            BuildN54(req, cable != 0);
            h.Flush();
            if (!h.Write(req, kN54Len)) {
                break;
            }
            Parsed got;
            ReadUntil(h, 800, [&](const uint8_t* r, int n) {
                Parsed p = ParseN54(r, n, cable != 0);
                if (p.ok) {
                    Diag(L"    reply: " + Hex(r, n, 12));
                    got = p;
                }
                return p.ok;
            });
            if (got.ok && !got.asleep) {
                out.push_back(MakeReading(g, L"Nordic54", L"ATK mouse", got));
                return true;
            }
        }
    }
    return false;
}

// --- Attack Shark Beken (pushed) -------------------------------------------

static bool IsBekenCol(const HidCol& c) {
    return (c.usagePage == 0x000A && c.usage == 0x0000) ||
           (c.usagePage == 0xFF00 && c.usage == 0x0001);
}

static bool ClaimBeken(const Group& g, const Settings&) {
    return g.vid == 0x1D57 && !Cols(g, IsBekenCol).empty();
}

static bool ReadBeken(const Group& g, const Settings&, std::vector<Reading>& out) {
    auto cols = Cols(g, IsBekenCol);
    // The status collection 000A:0000 first: that's where the vendor app reads.
    std::stable_sort(cols.begin(), cols.end(), [](const HidCol* a, const HidCol* b) {
        return a->usagePage == 0x000A && b->usagePage != 0x000A;
    });
    for (const HidCol* c : cols) {
        Diag(Fmt(L"  Attack Shark push on %04X:%04X", c->usagePage, c->usage));
        Hid h(*c);
        if (!h.Open(true)) {
            continue;
        }
        Parsed result;
        ReadUntil(h, 2600, [&](const uint8_t* r, int n) {
            Parsed p = ParseBeken(r, n);
            if (!p.ok) {
                return false;
            }
            Diag(L"    frame: " + Hex(r, n, 8));
            if (p.charging) {
                result.charging = true;
            }
            result.ok = true;
            if (p.level >= 0) {
                result.level = p.level;
                return true;  // a level frame ends the wait
            }
            return false;     // charging-only frame: a level may follow
        });
        if (result.ok) {
            out.push_back(MakeReading(g, L"AttackShark", L"Attack Shark mouse", result));
            return true;
        }
    }
    return false;
}

// --- MCHOSE RealTek --------------------------------------------------------

static bool IsMchoseCol(const HidCol& c) {
    return c.usagePage == 0xFF01;
}

static bool ClaimMchose(const Group& g, const Settings&) {
    return VidIn(g.vid, {0x5253, 0x3837}) && !Cols(g, IsMchoseCol).empty();
}

static bool ReadMchose(const Group& g, const Settings&, std::vector<Reading>& out) {
    for (const HidCol* c : Cols(g, IsMchoseCol)) {
        Hid h(*c);
        if (!h.Open(false)) {
            continue;
        }
        // 1. The status command, on the short report first.
        const struct {
            uint8_t id;
            int len;
        } channels[] = {{0x11, 21}, {0x12, 65}};
        for (auto ch : channels) {
            if (c->featLen < ch.len) {
                continue;
            }
            Diag(Fmt(L"  MCHOSE status 0x06 on report %02X", ch.id));
            uint8_t req[65];
            BuildMchoseStatus(req, ch.len, ch.id);
            for (int attempt = 0; attempt < 4; attempt++) {
                if (!h.SetFeature(req, ch.len)) {
                    break;
                }
                Sleep(120);
                uint8_t r[65];
                int n = h.GetFeature(ch.id, r, sizeof(r));
                Parsed p = ParseMchoseStatus(r, n);
                if (p.ok) {
                    Diag(L"    reply: " + Hex(r, n, 14));
                    out.push_back(MakeReading(g, L"MCHOSE", L"MCHOSE mouse", p));
                    return true;
                }
            }
        }
        // 2. The pushed status report (L7 Pro).
        Hid listen(*c);
        if (listen.Open(true)) {
            Diag(L"  MCHOSE push 0x13/0x1D");
            Parsed got;
            ReadUntil(listen, 3000, [&](const uint8_t* r, int n) {
                got = ParseMchosePush(r, n);
                if (got.ok) {
                    Diag(L"    frame: " + Hex(r, n, 16));
                }
                return got.ok;
            });
            if (got.ok) {
                out.push_back(MakeReading(g, L"MCHOSE", L"MCHOSE mouse", got));
                return true;
            }
        }
    }
    return false;
}

static bool IsG7Col(const HidCol& c) {
    return c.usagePage == 0xFF01 && c.outLen >= 9;
}

static bool ClaimG7(const Group& g, const Settings&) {
    return g.vid == 0xA8A5 && !Cols(g, IsG7Col).empty();
}

static bool ReadG7(const Group& g, const Settings&, std::vector<Reading>& out) {
    for (const HidCol* c : Cols(g, IsG7Col)) {
        Diag(L"  MCHOSE G7 AA30");
        Hid h(*c);
        if (!h.Open(true)) {
            continue;
        }
        h.Flush();
        if (!h.Write(kG7Request, sizeof(kG7Request))) {
            continue;
        }
        Parsed got;
        ReadUntil(h, 700, [&](const uint8_t* r, int n) {
            got = ParseG7(r, n);
            return got.ok;
        });
        if (got.ok) {
            out.push_back(MakeReading(g, L"MCHOSE-G7", L"MCHOSE G7", got));
            return true;
        }
    }
    return false;
}

// --- Razer -----------------------------------------------------------------

static std::map<uint32_t, uint8_t> g_razerTid;  // vid:pid -> working tid

static bool IsRazerCol(const HidCol& c) {
    return c.featLen >= kRazerLen;
}

static bool ClaimRazer(const Group& g, const Settings& s) {
    return s.razer && g.vid == 0x1532 && !Cols(g, IsRazerCol).empty();
}

// Returns the status byte, or -1 if no answer for our command arrived.
static int RazerQuery(Hid& h, uint8_t tid, uint8_t cls, uint8_t id, int* value) {
    uint8_t req[kRazerLen];
    BuildRazer(req, tid, cls, id);
    if (!h.SetFeature(req, kRazerLen)) {
        return -1;
    }
    Sleep(60);
    ULONGLONG deadline = GetTickCount64() + 1000;
    int status = -1;
    while (GetTickCount64() < deadline) {
        uint8_t r[kRazerLen + 8];
        int n = h.GetFeature(0, r, sizeof(r));
        status = ParseRazer(r, n, cls, id, value);
        if (status >= 0 && status != 0x01) {
            return status;
        }
        Sleep(80);  // busy, or Synapse's answer to something else
    }
    return status == 0x01 ? 0x04 : -1;
}

static bool ReadRazer(const Group& g, const Settings&, std::vector<Reading>& out) {
    auto cols = Cols(g, IsRazerCol);
    std::stable_sort(cols.begin(), cols.end(), [](const HidCol* a, const HidCol* b) {
        auto rank = [](const HidCol* c) {
            return (c->usagePage == 0x0001 || c->usagePage == 0xFF00) ? 0 : 1;
        };
        return rank(a) < rank(b);
    });
    uint32_t key = (g.vid << 16) | g.pid;
    std::vector<uint8_t> tids;
    if (g_razerTid.count(key)) {
        tids.push_back(g_razerTid[key]);
    }
    for (uint8_t t : {0x1F, 0x3F, 0xFF, 0x9F}) {
        if (std::find(tids.begin(), tids.end(), t) == tids.end()) {
            tids.push_back(t);
        }
    }
    for (const HidCol* c : cols) {
        Hid h(*c);
        if (!h.Open(false)) {
            continue;
        }
        for (uint8_t tid : tids) {
            int raw = 0;
            int st = RazerQuery(h, tid, 0x07, 0x80, &raw);
            Diag(Fmt(L"  Razer tid %02X on %04X:%04X -> status %d, raw %d", tid,
                     c->usagePage, c->usage, st, raw));
            if (st == 0x02) {
                g_razerTid[key] = tid;
                Parsed p;
                p.ok = true;
                p.level = (raw * 100 + 127) / 255;
                int chg = 0;
                if (RazerQuery(h, tid, 0x07, 0x84, &chg) == 0x02) {
                    p.charging = chg != 0;
                }
                out.push_back(MakeReading(g, L"Razer", L"Razer mouse", p));
                return true;
            }
            if (st == 0x04) {
                out.push_back(MakeReading(g, L"Razer", L"Razer mouse", Asleep()));
                return true;
            }
            if (st == 0x05) {
                return false;  // no battery on this device
            }
            if (st < 0) {
                break;         // collection does not take feature reports
            }
        }
    }
    return false;
}

// --- Logitech HID++ 2.0 ----------------------------------------------------

static bool IsHidppLong(const HidCol& c) {
    return c.usagePage == 0xFF00 && c.usage == 0x0002 && c.outLen >= 20;
}

static bool ClaimLogitech(const Group& g, const Settings& s) {
    return s.logitech && g.vid == 0x046D && !Cols(g, IsHidppLong).empty();
}

struct Hidpp {
    Hid* h;
    uint8_t dev;
    bool answered = false;  // any reply (even an error) from this index

    // Sends a long report and waits for the matching answer. False on an
    // HID++ error or a timeout (device index not paired / mouse off).
    bool Call(uint8_t feature, uint8_t fn, const uint8_t* params, int n,
              uint8_t* reply /* 16 */, int timeoutMs = 400) {
        uint8_t req[20] = {0x11, dev, feature, (uint8_t)((fn << 4) | 0x0A)};
        if (params && n) {
            memcpy(req + 4, params, std::min(n, 16));
        }
        h->Flush();
        if (!h->Write(req, 20)) {
            return false;
        }
        bool ok = false;
        ReadUntil(*h, timeoutMs, [&](const uint8_t* r, int len) {
            if (len < 7 || (r[0] != 0x10 && r[0] != 0x11) || r[1] != dev) {
                return false;
            }
            if (r[2] == 0x8F || (r[2] == 0xFF && r[3] == feature)) {
                answered = r[2] == 0xFF;  // 2.0 error: a device is there
                return true;              // 0x8F: nothing paired at this index
            }
            if (r[2] == feature && r[3] == req[3]) {
                answered = true;
                memcpy(reply, r + 4, std::min(16, len - 4));
                ok = true;
                return true;
            }
            return false;
        });
        return ok;
    }

    uint8_t FeatureIndex(uint16_t id) {
        uint8_t p[2] = {(uint8_t)(id >> 8), (uint8_t)id};
        uint8_t r[16] = {};
        if (!Call(0x00, 0, p, 2, r)) {
            return 0;
        }
        return r[0];
    }
};

static std::map<uint32_t, std::vector<uint8_t>> g_logiIndexes;  // known devices
static int g_logiScanCountdown = 0;

static bool ReadLogitech(const Group& g, const Settings& s, std::vector<Reading>& out) {
    auto cols = Cols(g, IsHidppLong);
    Hid h(*cols[0]);
    if (!h.Open(true)) {
        return false;
    }
    uint32_t key = (g.vid << 16) | g.pid;
    std::vector<uint8_t> indexes;
    bool rescan = g_logiScanCountdown <= 0 || !g_logiIndexes.count(key);
    if (rescan) {
        indexes = {1, 2, 3, 4, 5, 6, 0xFF};
    } else {
        indexes = g_logiIndexes[key];
    }
    std::vector<uint8_t> alive;
    bool any = false;
    for (uint8_t dev : indexes) {
        Hidpp pp{&h, dev};
        uint8_t r[16] = {};
        // Root feature ping tells whether a HID++ 2.0 device sits here.
        uint8_t fUnified = pp.FeatureIndex(0x1004);
        if (!pp.answered) {
            continue;  // empty slot or mouse off: don't wait on it twice more
        }
        uint8_t fStatus = fUnified ? 0 : pp.FeatureIndex(0x1000);
        uint8_t fVoltage = (fUnified || fStatus) ? 0 : pp.FeatureIndex(0x1001);
        if (!fUnified && !fStatus && !fVoltage) {
            continue;
        }
        alive.push_back(dev);

        std::wstring name;
        int type = -1;
        uint8_t fName = pp.FeatureIndex(0x0005);
        if (fName && pp.Call(fName, 0, nullptr, 0, r)) {
            int len = r[0];
            while ((int)name.size() < len) {
                uint8_t off = (uint8_t)name.size();
                uint8_t part[16] = {};
                if (!pp.Call(fName, 1, &off, 1, part)) {
                    break;
                }
                for (int i = 0; i < 16 && (int)name.size() < len; i++) {
                    name += (wchar_t)part[i];
                }
            }
            if (pp.Call(fName, 2, nullptr, 0, r)) {
                type = r[0];
            }
        }
        // 3 mouse, 4 trackpad, 5 trackball; everything else only on request
        if (type >= 0 && !(type == 3 || type == 4 || type == 5) && !s.allDevices) {
            continue;
        }

        Parsed p;
        if (fUnified && pp.Call(fUnified, 1, nullptr, 0, r)) {
            p.ok = r[0] <= 100;
            p.level = r[0];
            p.charging = r[2] == 1 || r[2] == 2 || r[2] == 3;
        } else if (fStatus && pp.Call(fStatus, 0, nullptr, 0, r)) {
            p.ok = r[0] <= 100;
            p.level = r[0];
            p.charging = r[2] >= 1 && r[2] <= 4;
        } else if (fVoltage && pp.Call(fVoltage, 0, nullptr, 0, r)) {
            int mv = (r[0] << 8) | r[1];
            p.ok = mv > 0;
            p.level = LogitechVoltageToPercent(mv);
            p.charging = (r[2] & 0x80) != 0;
        }
        Diag(Fmt(L"  Logitech index %u '%ls' type %d -> %d%%", dev, name.c_str(),
                 type, p.level));
        if (!p.ok) {
            continue;
        }
        Reading rd;
        rd.key = Fmt(L"Logitech/%04X:%04X/%u", g.vid, g.pid, dev);
        rd.name = name.empty() ? Fmt(L"Logitech device %u", dev)
                               : L"Logitech " + name;
        rd.source = L"HID++";
        rd.level = p.level;
        rd.charging = p.charging;
        out.push_back(rd);
        any = true;
    }
    if (rescan) {
        g_logiIndexes[key] = alive;
    }
    return any;
}

// --- Custom devices from the settings --------------------------------------

static bool ReadCustom(const Group& g, const CustomDevice& d,
                       std::vector<Reading>& out) {
    for (auto& c : g.cols) {
        if (d.usagePage >= 0 ? c.usagePage != d.usagePage
                             : !IsVendorPage(c.usagePage)) {
            continue;
        }
        bool listen = d.method == L"listen";
        bool feature = !listen && d.method != L"output";
        if (!listen && d.request.empty()) {
            continue;
        }
        Diag(Fmt(L"  custom '%ls' (%ls) on %04X:%04X", d.name.c_str(),
                 d.method.c_str(), c.usagePage, c.usage));
        Hid h(c);
        if (!h.Open(!feature)) {
            continue;
        }
        Parsed got;
        auto parse = [&](const uint8_t* r, int n) {
            got = ParseCustom(r, n, d.match.data(), (int)d.match.size(),
                              d.levelByte, d.chargingByte, d.levelScale);
            if (got.ok) {
                Diag(L"    reply: " + Hex(r, n, 16));
            }
            return got.ok;
        };
        if (feature) {
            if (!h.SetFeature(d.request.data(), (int)d.request.size())) {
                continue;
            }
            Sleep(100);
            for (int i = 0; i < 8 && !got.ok; i++) {
                uint8_t r[512];
                int n = h.GetFeature(d.request[0], r, sizeof(r));
                if (i == 0) {
                    Diag(L"    reply: " + Hex(r, n, 16));
                }
                if (!parse(r, n)) {
                    Sleep(50);
                }
            }
        } else {
            h.Flush();
            if (!listen && !h.Write(d.request.data(), (int)d.request.size())) {
                continue;
            }
            ReadUntil(h, listen ? 3000 : 1000, parse);
        }
        if (got.ok) {
            Reading r = MakeReading(g, L"Custom", d.name.c_str(), got);
            r.name = d.name;
            out.push_back(r);
            return true;
        }
    }
    return false;
}

struct Driver {
    const wchar_t* name;
    bool (*claims)(const Group&, const Settings&);
    bool (*read)(const Group&, const Settings&, std::vector<Reading>&);
};

static const Driver kDrivers[] = {
    {L"Logitech HID++", ClaimLogitech, ReadLogitech},
    {L"Razer", ClaimRazer, ReadRazer},
    {L"Attack Shark (Beken)", ClaimBeken, ReadBeken},
    {L"MCHOSE (RealTek)", ClaimMchose, ReadMchose},
    {L"MCHOSE G7", ClaimG7, ReadG7},
    {L"Compx Nordic 52840", ClaimCompxKnown, ReadCompx},
    {L"Compx Nordic 54L15", ClaimN54, ReadN54},
    {L"0x83 feature (known)", ClaimF83Known, ReadF83},
    {L"0x83 feature (probe)", ClaimF83Probe, ReadF83},
    {L"Compx (probe)", ClaimCompxProbe, ReadCompx},
};

////////////////////////////////////////////////////////////////////////////////
// Bluetooth

static void ReadBluetooth(const Settings& s, const std::vector<HidCol>& all,
                          std::vector<Reading>& out) {
    std::vector<GUID> mice;
    for (auto& c : all) {
        if (IsMouse(c)) {
            mice.push_back(c.container);
        }
    }
    std::vector<GUID> seen;
    for (const wchar_t* enumerator : {L"BTHLE", L"BTHENUM"}) {
        HDEVINFO di = SetupDiGetClassDevsW(nullptr, enumerator, nullptr,
                                           DIGCF_ALLCLASSES | DIGCF_PRESENT);
        if (di == INVALID_HANDLE_VALUE) {
            continue;
        }
        SP_DEVINFO_DATA dd{sizeof(dd)};
        for (DWORD i = 0; SetupDiEnumDeviceInfo(di, i, &dd); i++) {
            DEVPROPTYPE type = 0;
            BYTE level = 0;
            if (!SetupDiGetDevicePropertyW(di, &dd, &kKeyBtBattery, &type,
                                           &level, sizeof(level), nullptr, 0) ||
                type != DEVPROP_TYPE_BYTE || level > 100) {
                continue;
            }
            GUID container{};
            SetupDiGetDevicePropertyW(di, &dd, &kKeyContainerId, &type,
                                      (BYTE*)&container, sizeof(container),
                                      nullptr, 0);
            bool isMouse = std::any_of(mice.begin(), mice.end(), [&](const GUID& m) {
                return IsEqualGUID(m, container);
            });
            if (!isMouse && !s.allDevices) {
                continue;
            }
            if (std::any_of(seen.begin(), seen.end(), [&](const GUID& m) {
                    return IsEqualGUID(m, container);
                })) {
                continue;
            }
            seen.push_back(container);

            wchar_t name[256] = {};
            if (!SetupDiGetDevicePropertyW(di, &dd, &kKeyFriendlyName, &type,
                                           (BYTE*)name, sizeof(name) - 2,
                                           nullptr, 0)) {
                SetupDiGetDevicePropertyW(di, &dd, &kKeyDeviceDesc, &type,
                                          (BYTE*)name, sizeof(name) - 2,
                                          nullptr, 0);
            }
            DEVPROP_BOOLEAN connected = DEVPROP_TRUE;
            bool hasConnected = SetupDiGetDevicePropertyW(
                di, &dd, &kKeyIsConnected, &type, (BYTE*)&connected,
                sizeof(connected), nullptr, 0) && type == DEVPROP_TYPE_BOOLEAN;

            Reading r;
            r.key = Fmt(L"BT/%08lX-%04X-%04X", container.Data1, container.Data2,
                        container.Data3);
            r.name = name[0] ? name : L"Bluetooth device";
            r.source = L"Bluetooth";
            r.level = level;
            r.asleep = hasConnected && connected == DEVPROP_FALSE;
            Diag(Fmt(L"Bluetooth '%ls': %u%%%ls", r.name.c_str(), level,
                     r.asleep ? L" (not connected)" : L""));
            out.push_back(r);
        }
        SetupDiDestroyDeviceInfoList(di);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Poll engine (worker thread only)

struct Known {
    Reading last;
    ULONGLONG seen = 0;
};

static std::map<std::wstring, Known> g_known;
static std::map<uint32_t, int> g_cachedDriver;          // vid:pid -> driver
static std::map<uint32_t, ULONGLONG> g_unreadableUntil; // negative cache
static volatile LONG g_resetCaches = 0;

static const ULONGLONG kKeepAsleepMs = 15 * 60 * 1000;
static const ULONGLONG kRetryUnreadableMs = 10 * 60 * 1000;

static std::vector<Reading> PollOnce() {
    Settings s = GetSettings();
    ULONGLONG now = GetTickCount64();
    if (InterlockedExchange(&g_resetCaches, 0)) {
        g_unreadableUntil.clear();
        g_logiScanCountdown = 0;
    }
    g_diagNext.clear();
    Diag(Fmt(L"Mouse Battery Tray %ls diagnostics", MBT_VERSION));
    SYSTEMTIME st;
    GetLocalTime(&st);
    Diag(Fmt(L"Poll at %04u-%02u-%02u %02u:%02u:%02u", st.wYear, st.wMonth,
             st.wDay, st.wHour, st.wMinute, st.wSecond));

    std::vector<HidCol> all = EnumerateHid();
    std::map<uint32_t, Group> groups;
    for (auto& c : all) {
        uint32_t key = (c.vid << 16) | c.pid;
        Group& g = groups[key];
        g.vid = c.vid;
        g.pid = c.pid;
        g.cols.push_back(c);
        if (g.product.empty() && !c.product.empty()) {
            g.product = c.product;
        }
        if (IsMouse(c)) {
            g.hasMouse = true;
        }
    }

    Diag(L"");
    Diag(L"HID collections (VID:PID  usagePage:usage  in/out/feature bytes  product)");
    for (auto& [key, g] : groups) {
        for (auto& c : g.cols) {
            Diag(Fmt(L"  %04X:%04X  %04X:%04X  %3d/%3d/%3d  %ls", c.vid, c.pid,
                     c.usagePage, c.usage, c.inLen, c.outLen, c.featLen,
                     c.product.c_str()));
        }
    }
    Diag(L"");

    std::vector<Reading> readings;
    std::vector<std::wstring> presentKeysPrefix;

    for (auto& [key, g] : groups) {
        // Custom devices come first: they can override a built-in driver.
        bool done = false;
        for (auto& d : s.custom) {
            if (d.vid == g.vid && (d.pid < 0 || d.pid == g.pid)) {
                Diag(Fmt(L"[%04X:%04X] %ls", g.vid, g.pid, g.product.c_str()));
                done = ReadCustom(g, d, readings);
                if (done) {
                    break;
                }
            }
        }
        if (done) {
            continue;
        }

        std::vector<int> order;
        auto cached = g_cachedDriver.find(key);
        if (cached != g_cachedDriver.end() &&
            kDrivers[cached->second].claims(g, s)) {
            order.push_back(cached->second);
        }
        for (int i = 0; i < (int)ARRAYSIZE(kDrivers); i++) {
            if (std::find(order.begin(), order.end(), i) == order.end() &&
                kDrivers[i].claims(g, s)) {
                order.push_back(i);
            }
        }
        if (order.empty()) {
            continue;
        }
        bool cachedKnown = cached != g_cachedDriver.end();
        auto bad = g_unreadableUntil.find(key);
        if (!cachedKnown && bad != g_unreadableUntil.end() && now < bad->second) {
            continue;  // nothing answered recently; retried later or on plug-in
        }
        Diag(Fmt(L"[%04X:%04X] %ls", g.vid, g.pid, g.product.c_str()));
        bool ok = false;
        for (int i : order) {
            Diag(Fmt(L" driver: %ls", kDrivers[i].name));
            if (kDrivers[i].read(g, s, readings)) {
                g_cachedDriver[key] = i;
                ok = true;
                break;
            }
            // A device that answered before but is quiet now keeps its
            // driver; other drivers are not tried on it every poll.
            if (cachedKnown) {
                break;
            }
        }
        if (!ok) {
            if (!cachedKnown) {
                g_unreadableUntil[key] = now + kRetryUnreadableMs;
            }
            // Keep the last values of this device, greyed out.
            for (auto& [k, known] : g_known) {
                if (k.find(Fmt(L"%04X:%04X", g.vid, g.pid)) != std::wstring::npos &&
                    now - known.seen < kKeepAsleepMs) {
                    Reading r = known.last;
                    r.asleep = true;
                    readings.push_back(r);
                }
            }
        }
    }
    if (--g_logiScanCountdown < 0) {
        g_logiScanCountdown = 10;
    }

    if (s.bluetooth) {
        ReadBluetooth(s, all, readings);
    }

    // Merge with what is known: an asleep answer or a charging-only frame
    // keeps the last level; fresh levels are remembered.
    std::vector<Reading> result;
    for (auto& r : readings) {
        if (std::any_of(result.begin(), result.end(),
                        [&](const Reading& x) { return x.key == r.key; })) {
            continue;
        }
        auto it = g_known.find(r.key);
        if (r.level < 0 && it != g_known.end()) {
            r.level = it->second.last.level;
        }
        if (!r.asleep && r.level >= 0) {
            g_known[r.key] = {r, now};
        }
        if (r.asleep && r.level < 0) {
            continue;  // asleep and never read: nothing to show
        }
        result.push_back(r);
    }

    Diag(L"");
    Diag(L"Result:");
    for (auto& r : result) {
        Diag(Fmt(L"  %ls: %d%%%ls%ls [%ls]", r.name.c_str(), r.level,
                 r.charging ? L" charging" : L"", r.asleep ? L" asleep" : L"",
                 r.source.c_str()));
    }
    if (result.empty()) {
        Diag(L"  no battery found");
    }
    AcquireSRWLockExclusive(&g_diagLock);
    g_diagLast = g_diagNext;
    ReleaseSRWLockExclusive(&g_diagLock);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Part 3: tray icons (UI thread)

static const UINT WM_APP_TRAY = WM_APP + 1;
static const UINT WM_APP_READINGS = WM_APP + 2;
static const UINT WM_APP_SETTINGS = WM_APP + 3;
static const UINT_PTR TIMER_DEVICE_CHANGE = 1;
static const UINT kNoneIconId = 999;

enum MenuId : UINT {
    ID_REFRESH = 100,
    ID_DIAGNOSTICS,
    ID_AUTOSTART,
    ID_OPEN_SETTINGS,
    ID_EXIT,
};

static HANDLE g_uiThread = nullptr;
static HANDLE g_workerThread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_refreshEvent = nullptr;
static HWND g_hwnd = nullptr;
static UINT g_taskbarCreatedMsg = 0;
static HDEVNOTIFY g_devNotify = nullptr;

static SRWLOCK g_readingsLock = SRWLOCK_INIT;
static std::vector<Reading> g_pendingReadings;

static std::vector<Reading> g_shown;                // UI thread
static std::map<std::wstring, UINT> g_iconIds;      // key -> tray id
static std::map<UINT, HICON> g_icons;
static std::map<std::wstring, bool> g_lowNotified;
static UINT g_nextIconId = 1;

static bool TaskbarIsLight() {
    DWORD v = 0, size = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &v,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return v != 0;
}

static int IconSize() {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    using GetSystemMetricsForDpi_t = int(WINAPI*)(int, UINT);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto getDpi = (GetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow");
    auto getMetric =
        (GetSystemMetricsForDpi_t)GetProcAddress(user32, "GetSystemMetricsForDpi");
    if (getDpi && getMetric && g_hwnd) {
        return getMetric(SM_CXSMICON, getDpi(g_hwnd));
    }
    return GetSystemMetrics(SM_CXSMICON);
}

// Paints white shapes on a black mask, then blends the mask in one color
// onto a premultiplied ARGB canvas.
struct Painter {
    int n;
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    uint32_t* mask = nullptr;
    std::vector<uint32_t> canvas;

    explicit Painter(int size) : n(size), canvas(size * size, 0) {
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = n;
        bi.bmiHeader.biHeight = -n;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        dc = CreateCompatibleDC(nullptr);
        bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, (void**)&mask, nullptr, 0);
        old = SelectObject(dc, bmp);
    }
    ~Painter() {
        SelectObject(dc, old);
        DeleteObject(bmp);
        DeleteDC(dc);
    }
    void Clear() {
        GdiFlush();
        memset(mask, 0, n * n * 4);
    }
    void Blend(COLORREF color, int opacity = 255) {
        GdiFlush();
        for (int i = 0; i < n * n; i++) {
            int a = (mask[i] & 0xFF) * opacity / 255;
            if (!a) {
                continue;
            }
            uint32_t d = canvas[i];
            int da = d >> 24, dr = (d >> 16) & 0xFF, dg = (d >> 8) & 0xFF, db = d & 0xFF;
            int inv = 255 - a;
            int oa = a + da * inv / 255;
            int orr = GetRValue(color) * a / 255 + dr * inv / 255;
            int og = GetGValue(color) * a / 255 + dg * inv / 255;
            int ob = GetBValue(color) * a / 255 + db * inv / 255;
            canvas[i] = (oa << 24) | (orr << 16) | (og << 8) | ob;
        }
    }
    void Rect(int l, int t, int r, int b) {
        RECT rc{l, t, r, b};
        HBRUSH br = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(dc, &rc, br);
        DeleteObject(br);
    }
    void Text(const wchar_t* text, int boxTop, int boxBottom) {
        int len = (int)wcslen(text);
        int height = boxBottom - boxTop + n / 6;
        HFONT font = nullptr;
        SIZE sz{};
        for (; height > 6; height--) {
            if (font) {
                SelectObject(dc, GetStockObject(SYSTEM_FONT));
                DeleteObject(font);
            }
            font = CreateFontW(-height, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
                               OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe UI");
            SelectObject(dc, font);
            GetTextExtentPoint32W(dc, text, len, &sz);
            if (sz.cx <= n + 1) {
                break;
            }
        }
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));
        TEXTMETRICW tm{};
        GetTextMetricsW(dc, &tm);
        // Center the digits themselves, not the font's internal leading.
        int digitTop = tm.tmInternalLeading;
        int digitHeight = tm.tmAscent - tm.tmInternalLeading;
        int y = boxTop + (boxBottom - boxTop - digitHeight) / 2 - digitTop;
        int x = (n - sz.cx) / 2;
        TextOutW(dc, x, y, text, len);
        SelectObject(dc, GetStockObject(SYSTEM_FONT));
        DeleteObject(font);
    }
    void Bolt(int cx, int cy, int h) {
        POINT pts[6] = {
            {cx + h / 6, cy - h / 2}, {cx - h / 4, cy + h / 12}, {cx, cy + h / 12},
            {cx - h / 6, cy + h / 2}, {cx + h / 4, cy - h / 12}, {cx, cy - h / 12},
        };
        HBRUSH br = CreateSolidBrush(RGB(255, 255, 255));
        HGDIOBJ oldBrush = SelectObject(dc, br);
        HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
        Polygon(dc, pts, 6);
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBrush);
        DeleteObject(br);
    }
    HICON Finish() {
        BITMAPV5HEADER bh{};
        bh.bV5Size = sizeof(bh);
        bh.bV5Width = n;
        bh.bV5Height = -n;
        bh.bV5Planes = 1;
        bh.bV5BitCount = 32;
        bh.bV5Compression = BI_BITFIELDS;
        bh.bV5RedMask = 0x00FF0000;
        bh.bV5GreenMask = 0x0000FF00;
        bh.bV5BlueMask = 0x000000FF;
        bh.bV5AlphaMask = 0xFF000000;
        uint32_t* px = nullptr;
        HDC screen = GetDC(nullptr);
        HBITMAP color = CreateDIBSection(screen, (BITMAPINFO*)&bh, DIB_RGB_COLORS,
                                         (void**)&px, nullptr, 0);
        ReleaseDC(nullptr, screen);
        // Icons take straight (not premultiplied) alpha.
        for (int i = 0; i < n * n; i++) {
            uint32_t c = canvas[i];
            int a = c >> 24;
            if (!a) {
                px[i] = 0;
                continue;
            }
            int r = std::min(255, (int)((c >> 16) & 0xFF) * 255 / a);
            int g = std::min(255, (int)((c >> 8) & 0xFF) * 255 / a);
            int b = std::min(255, (int)(c & 0xFF) * 255 / a);
            px[i] = (a << 24) | (r << 16) | (g << 8) | b;
        }
        HBITMAP monoMask = CreateBitmap(n, n, 1, 1, nullptr);
        ICONINFO ii{TRUE, 0, 0, monoMask, color};
        HICON icon = CreateIconIndirect(&ii);
        DeleteObject(monoMask);
        DeleteObject(color);
        return icon;
    }
};

static COLORREF LevelColor(const Reading* r, const Settings& s, COLORREF text) {
    if (!r || r->level < 0) {
        return text;
    }
    bool low = s.lowBattery > 0 && r->level <= s.lowBattery;
    if (!s.colorByLevel) {
        return (low && !r->charging) ? RGB(0xFF, 0x45, 0x3A) : text;
    }
    if (r->charging) return RGB(0x3A, 0xA8, 0xFF);
    if (r->level < 20 || low) return RGB(0xFF, 0x45, 0x3A);
    if (r->level < 50) return RGB(0xFF, 0xB0, 0x20);
    return RGB(0x4C, 0xD9, 0x64);
}

static HICON CreateLevelIcon(const Reading* r, const Settings& s) {
    int n = IconSize();
    bool light = TaskbarIsLight();
    COLORREF text = light ? RGB(0x1C, 0x1C, 0x1C) : RGB(0xFF, 0xFF, 0xFF);
    COLORREF color = LevelColor(r, s, text);
    int opacity = (r && r->asleep) ? 120 : 255;
    if (r && r->asleep) {
        color = RGB(0x90, 0x90, 0x90);
    }
    Painter p(n);
    if (!s.batteryStyle || !r || r->level < 0) {
        std::wstring t = !r ? L"–" : r->level < 0 ? L"?" : std::to_wstring(r->level);
        p.Clear();
        p.Text(t.c_str(), 0, n);
        p.Blend(r ? color : text, r ? opacity : 140);
        return p.Finish();
    }
    // Battery gauge: outline, nub, fill, bolt while charging.
    int stroke = std::max(1, n / 16);
    int left = 0, right = n - 2 - stroke, top = n * 5 / 16, bottom = n - n * 5 / 16;
    p.Clear();
    p.Rect(left, top, right, top + stroke);
    p.Rect(left, bottom - stroke, right, bottom);
    p.Rect(left, top, left + stroke, bottom);
    p.Rect(right - stroke, top, right, bottom);
    p.Rect(right, top + (bottom - top) / 3, right + stroke + 1,
           bottom - (bottom - top) / 3);
    p.Blend(text, opacity);
    int inner = right - left - 4 * stroke;
    int fill = std::max(r->level > 0 ? 1 : 0, inner * r->level / 100);
    p.Clear();
    p.Rect(left + 2 * stroke, top + 2 * stroke, left + 2 * stroke + fill,
           bottom - 2 * stroke);
    p.Blend(color, opacity);
    if (r->charging) {
        p.Clear();
        p.Bolt((left + right) / 2, (top + bottom) / 2, (bottom - top) + 2 * stroke);
        p.Blend(text, opacity);
    }
    return p.Finish();
}

static std::wstring Describe(const Reading& r) {
    std::wstring s = r.name + L": ";
    if (r.level >= 0) {
        s += std::to_wstring(r.level) + L"%";
    } else {
        s += L"?";
    }
    if (r.asleep) {
        s += L" (asleep, last known)";
    } else if (r.charging) {
        s += L" (charging)";
    }
    return s;
}

static void SetTrayIcon(UINT id, HICON icon, const std::wstring& tip, bool add) {
    NOTIFYICONDATAW nid{sizeof(nid)};
    nid.hWnd = g_hwnd;
    nid.uID = id;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
    nid.uCallbackMessage = WM_APP_TRAY;
    nid.hIcon = icon;
    wcsncpy_s(nid.szTip, tip.c_str(), _TRUNCATE);
    if (add) {
        if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
            Shell_NotifyIconW(NIM_MODIFY, &nid);
        }
        nid.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nid);
    } else if (!Shell_NotifyIconW(NIM_MODIFY, &nid)) {
        SetTrayIcon(id, icon, tip, true);
        return;
    }
    if (g_icons.count(id)) {
        DestroyIcon(g_icons[id]);
    }
    g_icons[id] = icon;
}

static void RemoveTrayIcon(UINT id) {
    NOTIFYICONDATAW nid{sizeof(nid)};
    nid.hWnd = g_hwnd;
    nid.uID = id;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    if (g_icons.count(id)) {
        DestroyIcon(g_icons[id]);
        g_icons.erase(id);
    }
}

static void Balloon(UINT id, const std::wstring& title, const std::wstring& text) {
    NOTIFYICONDATAW nid{sizeof(nid)};
    nid.hWnd = g_hwnd;
    nid.uID = id;
    nid.uFlags = NIF_INFO;
    nid.dwInfoFlags = NIIF_WARNING;
    wcsncpy_s(nid.szInfoTitle, title.c_str(), _TRUNCATE);
    wcsncpy_s(nid.szInfo, text.c_str(), _TRUNCATE);
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

static void UpdateTray(bool recreate) {
    Settings s = GetSettings();
    if (recreate) {
        for (auto& [id, icon] : g_icons) {
            DestroyIcon(icon);
        }
        g_icons.clear();
    }

    // Which icons are wanted: one per device, or the lowest one.
    std::vector<std::pair<UINT, const Reading*>> wanted;
    std::wstring allTips;
    for (auto& r : g_shown) {
        if (!allTips.empty()) {
            allTips += L"\n";
        }
        allTips += Describe(r);
    }
    if (s.lowestOnly && !g_shown.empty()) {
        const Reading* low = &g_shown[0];
        for (auto& r : g_shown) {
            bool better = !r.asleep && (low->asleep || r.level < low->level);
            if (better) {
                low = &r;
            }
        }
        wanted.push_back({1, low});
    } else {
        for (auto& r : g_shown) {
            if (!g_iconIds.count(r.key)) {
                g_iconIds[r.key] = ++g_nextIconId;
            }
            wanted.push_back({g_iconIds[r.key], &r});
        }
    }

    std::vector<UINT> keep;
    for (auto& [id, r] : wanted) {
        std::wstring tip = s.lowestOnly ? allTips : Describe(*r);
        bool add = recreate || !g_icons.count(id);
        SetTrayIcon(id, CreateLevelIcon(r, s), tip, add);
        keep.push_back(id);

        // Low battery notification, once per discharge.
        bool low = s.lowBattery > 0 && r->level >= 0 && r->level <= s.lowBattery;
        bool& notified = g_lowNotified[r->key];
        if (low && !r->charging && !r->asleep && !notified) {
            Balloon(id, L"Mouse battery low",
                    Fmt(L"%ls is at %d%%. Time to charge it.", r->name.c_str(),
                        r->level));
            notified = true;
        } else if (r->charging || (r->level > s.lowBattery + 3)) {
            notified = false;
        }
    }
    if (wanted.empty() && s.showWhenNone) {
        bool add = recreate || !g_icons.count(kNoneIconId);
        SetTrayIcon(kNoneIconId, CreateLevelIcon(nullptr, s),
                    L"Mouse Battery Tray: no supported mouse found.\n"
                    L"Right-click > Save diagnostics",
                    add);
        keep.push_back(kNoneIconId);
    }
    std::vector<UINT> stale;
    for (auto& [id, icon] : g_icons) {
        if (std::find(keep.begin(), keep.end(), id) == keep.end()) {
            stale.push_back(id);
        }
    }
    for (UINT id : stale) {
        RemoveTrayIcon(id);
    }
}

static std::wstring DiagnosticsPath() {
    wchar_t tmp[MAX_PATH];
    GetTempPathW(MAX_PATH, tmp);
    return std::wstring(tmp) + L"mouse-battery-tray-diagnostics.txt";
}

static void SaveDiagnostics() {
    AcquireSRWLockShared(&g_diagLock);
    std::wstring text = g_diagLast;
    ReleaseSRWLockShared(&g_diagLock);
    if (text.empty()) {
        text = L"No poll has finished yet. Try again in a few seconds.\r\n";
    }
    std::wstring path = DiagnosticsPath();
    HANDLE f = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        return;
    }
    int bytes = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(),
                                    nullptr, 0, nullptr, nullptr);
    std::string utf8(bytes, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), utf8.data(),
                        bytes, nullptr, nullptr);
    DWORD written;
    WriteFile(f, "\xEF\xBB\xBF", 3, &written, nullptr);
    WriteFile(f, utf8.data(), (DWORD)utf8.size(), &written, nullptr);
    CloseHandle(f);
    ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

}  // namespace mbt
#ifdef MBT_STANDALONE
// Implemented in standalone/main.cpp.
bool StandaloneAutostartEnabled();
void StandaloneSetAutostart(bool enable);
void StandaloneOpenSettings();
#endif
namespace mbt {

static void ShowMenu(POINT pt) {
    HMENU menu = CreatePopupMenu();
    if (g_shown.empty()) {
        AppendMenuW(menu, MF_STRING | MF_GRAYED, 0, L"No supported mouse found");
    }
    for (auto& r : g_shown) {
        AppendMenuW(menu, MF_STRING | MF_GRAYED, 0, Describe(r).c_str());
    }
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, ID_REFRESH, L"Refresh now");
    AppendMenuW(menu, MF_STRING, ID_DIAGNOSTICS, L"Save diagnostics...");
#ifdef MBT_STANDALONE
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (StandaloneAutostartEnabled() ? MF_CHECKED : 0),
                ID_AUTOSTART, L"Start with Windows");
    AppendMenuW(menu, MF_STRING, ID_OPEN_SETTINGS, L"Settings...");
    AppendMenuW(menu, MF_STRING, ID_EXIT, L"Exit");
#endif
    SetForegroundWindow(g_hwnd);
    UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
                              pt.x, pt.y, 0, g_hwnd, nullptr);
    PostMessageW(g_hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);
    switch (cmd) {
        case ID_REFRESH:
            InterlockedExchange(&g_resetCaches, 1);
            SetEvent(g_refreshEvent);
            break;
        case ID_DIAGNOSTICS:
            SaveDiagnostics();
            break;
#ifdef MBT_STANDALONE
        case ID_AUTOSTART:
            StandaloneSetAutostart(!StandaloneAutostartEnabled());
            break;
        case ID_OPEN_SETTINGS:
            StandaloneOpenSettings();
            break;
        case ID_EXIT:
            DestroyWindow(g_hwnd);
            break;
#endif
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg) {
        UpdateTray(true);  // explorer restarted
        return 0;
    }
    switch (msg) {
        case WM_APP_READINGS: {
            AcquireSRWLockExclusive(&g_readingsLock);
            g_shown = g_pendingReadings;
            ReleaseSRWLockExclusive(&g_readingsLock);
            UpdateTray(false);
            return 0;
        }
        case WM_APP_SETTINGS:
            UpdateTray(true);
            return 0;
        case WM_APP_TRAY: {
            UINT event = LOWORD(lp);
            if (event == WM_CONTEXTMENU || event == NIN_SELECT ||
                event == NIN_KEYSELECT) {
                POINT pt{(short)LOWORD(wp), (short)HIWORD(wp)};
                ShowMenu(pt);
            }
            return 0;
        }
        case WM_DEVICECHANGE:
            if (wp == 0x8000 /* DBT_DEVICEARRIVAL */ ||
                wp == 0x8004 /* DBT_DEVICEREMOVECOMPLETE */) {
                SetTimer(hwnd, TIMER_DEVICE_CHANGE, 2500, nullptr);
            }
            return TRUE;
        case WM_TIMER:
            if (wp == TIMER_DEVICE_CHANGE) {
                KillTimer(hwnd, TIMER_DEVICE_CHANGE);
                InterlockedExchange(&g_resetCaches, 1);
                SetEvent(g_refreshEvent);
            }
            return 0;
        case WM_SETTINGCHANGE:
        case WM_DPICHANGED:
        case WM_THEMECHANGED:
            UpdateTray(false);  // light/dark taskbar, scale
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY: {
            std::vector<UINT> ids;
            for (auto& [id, icon] : g_icons) {
                ids.push_back(id);
            }
            for (UINT id : ids) {
                RemoveTrayIcon(id);
            }
            if (g_devNotify) {
                UnregisterDeviceNotification(g_devNotify);
                g_devNotify = nullptr;
            }
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI WorkerThread(void*) {
    while (true) {
        std::vector<Reading> r = PollOnce();
        AcquireSRWLockExclusive(&g_readingsLock);
        g_pendingReadings = r;
        ReleaseSRWLockExclusive(&g_readingsLock);
        if (g_hwnd) {
            PostMessageW(g_hwnd, WM_APP_READINGS, 0, 0);
        }
        HANDLE events[2] = {g_stopEvent, g_refreshEvent};
        DWORD wait = WaitForMultipleObjects(2, events, FALSE,
                                            GetSettings().pollSeconds * 1000);
        if (wait == WAIT_OBJECT_0) {
            break;
        }
    }
    return 0;
}

static DWORD WINAPI UiThread(void* readyEvent) {
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"MouseBatteryTrayWindow";
    RegisterClassW(&wc);
    g_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, wc.lpszClassName,
                             L"Mouse Battery Tray", WS_POPUP, 0, 0, 0, 0, nullptr,
                             nullptr, wc.hInstance, nullptr);
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    if (g_hwnd) {
        // Let the TaskbarCreated broadcast through even when elevated.
        ChangeWindowMessageFilterEx(g_hwnd, g_taskbarCreatedMsg, MSGFLT_ALLOW,
                                    nullptr);
        DEV_BROADCAST_DEVICEINTERFACE_W filter{};
        filter.dbcc_size = sizeof(filter);
        filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        HidD_GetHidGuid(&filter.dbcc_classguid);
        g_devNotify = RegisterDeviceNotificationW(g_hwnd, &filter,
                                                  DEVICE_NOTIFY_WINDOW_HANDLE);
        if (GetSettings().showWhenNone) {
            g_shown.clear();
            UpdateTray(true);
        }
    }
    SetEvent((HANDLE)readyEvent);
    if (!g_hwnd) {
        return 1;
    }
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    g_hwnd = nullptr;
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    if (g_stopEvent) {
        SetEvent(g_stopEvent);  // the window closed (Exit): stop polling too
    }
    return 0;
}

}  // namespace mbt

////////////////////////////////////////////////////////////////////////////////
// Part 4: Windhawk callbacks

BOOL WhTool_ModInit() {
    using namespace mbt;
    Wh_Log(L"Mouse Battery Tray starting");
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_refreshEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThread, ready, 0, nullptr);
    if (!g_uiThread) {
        return FALSE;
    }
    WaitForSingleObject(ready, 5000);
    CloseHandle(ready);
    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    return g_workerThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    using namespace mbt;
    Wh_Log(L"Settings changed");
    LoadSettings();
    InterlockedExchange(&g_resetCaches, 1);
    SetEvent(g_refreshEvent);
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    using namespace mbt;
    Wh_Log(L"Mouse Battery Tray stopping");
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 5000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_workerThread) {
        // A poll in flight finishes its current HID read (at most ~3 s).
        WaitForSingleObject(g_workerThread, 10000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
}

#ifndef MBT_STANDALONE

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

#endif  // !MBT_STANDALONE
#endif  // !MBT_PARSER_TEST
