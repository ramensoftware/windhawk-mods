// ==WindhawkMod==
// @id              taskmgr-network-bytes
// @name            Task Manager Network Speed in Bytes
// @description     Show network throughput in Task Manager as KB/s, MB/s, GB/s (bytes) instead of Kbps, Mbps, Gbps (bits)
// @version         1.3
// @author          Nehoray-Bitan
// @github          https://github.com/Nehoray-Bitan
// @include         taskmgr.exe
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task Manager Network Speed in Bytes

Task Manager reports network throughput in **bits** per second (Kbps, Mbps,
Gbps), while browsers, download managers and Task Manager's own disk graphs use
**bytes** per second. This mod converts every network rate to bytes:

| Where | Before | After |
|---|---|---|
| Performance → Ethernet / Wi-Fi: Send / Receive | `8.0 Kbps` | `1.0 KB/s` |
| Performance sidebar | `S: 0 R: 8.0 Kbps` | `S: 0 R: 1.0 KB/s` |
| Performance graph scale labels | `500 Kbps` / `450 Kbps` | `62.5 KB/s` / `56.3 KB/s` |
| Processes / Users tabs, Network column | `0.1 Mbps`, `2.4 Mbps` | `12.5 KB/s`, `300 KB/s` |
| View network details → Link speed | `1 Gbps` | `125 MB/s` |

The Processes column switches unit per value: KB/s below 1 MB/s, MB/s above.
Units are decimal (1 KB = 1000 bytes), like Task Manager's disk rates.

By default the units are always the English `KB/s`, `MB/s` and `GB/s`,
whatever Task Manager's display language is. The **Unit names** setting can
switch them to Task Manager's own translated names instead (the ones its disk
rates use).

## Notes

- The mod prefers the debug symbols of `taskmgr.exe`. Windhawk downloads them
  (about 20 MB) the first time Task Manager starts after the mod is installed —
  wait for the "Loading symbols" popup to reach 100%. If a Windows update ships
  a Task Manager build whose symbols aren't on Microsoft's symbol server yet,
  the mod falls back to matching the current build's code by byte patterns.
  The patterns are tied to the executable's function table, so moving code
  around doesn't break the mod. If an update changes the start of a function
  the mod hooks, that hook is skipped until the patterns are updated.
- Restart Task Manager after enabling or disabling the mod. Graph scale labels
  are cached until the graph rescales.
- The graph keeps Task Manager's original scale steps, which are round numbers
  in bits, so some labels are uneven in bytes (`54 Mbps` → `6.75 MB/s`).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- unitNames: english
  $name: Unit names
  $description: >-
    English always shows KB/s, MB/s and GB/s. Task Manager's language uses the
    translated unit names Task Manager shows for disk rates. Graph labels
    update after Task Manager is restarted.
  $options:
  - english: English (KB/s, MB/s, GB/s)
  - localized: Task Manager's display language
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <string>

#include <strsafe.h>

namespace {

// Task Manager string resource IDs (Taskmgr.exe.mui).
constexpr DWORD kIdBps = 32875;   // "bps"
constexpr DWORD kIdKbps = 32876;  // "Kbps"
constexpr DWORD kIdMbps = 32877;  // "Mbps"
constexpr DWORD kIdGbps = 32878;  // "Gbps"
constexpr DWORD kIdBytesPerSec = 32879;      // "B/sec"
constexpr DWORD kIdKBytesPerSec = 32880;     // "KB/s"
constexpr DWORD kIdMBytesPerSec = 32881;     // "MB/s"
constexpr DWORD kIdGBytesPerSec = 32882;     // "GB/s"
constexpr DWORD kIdValueUnitPattern = 34563;  // "%1!s! %2!s!"
constexpr DWORD kIdNetworkColumnMbps = 37034;  // "%s Mbps"

constexpr PCWSTR kEnglishByteUnits[] = {L"B/s", L"KB/s", L"MB/s", L"GB/s"};
constexpr PCWSTR kDefaultValueUnitPattern = L"%1!s! %2!s!";

// WDC column type of the Processes/Users "Network" column.
constexpr BYTE kNetworkColumnType = 0x0A;

// GetColumnText raises any non-zero network value below this to this.
constexpr double kNetworkColumnMinMbps = static_cast<double>(0.1f);

// Fixed output size used by the link speed overload of GetBestFitUnit.
constexpr size_t kLinkSpeedBufferChars = 0x80;

// Byte patterns for the hook targets, used when Microsoft's symbol server
// doesn't have a PDB for the installed Task Manager build yet. '??' marks
// bytes that can change without the function itself changing: relocated
// addresses, stack frame sizes and local variable offsets. Matching is exact
// otherwise, and only function starts from the executable's function table
// are considered, so moved code still matches but a lookalike function does
// not.
constexpr PCSTR kPatternGetBestFitUnit =
    "48 8B C4 48 89 58 08 55 56 57 41 56 41 57 "
    "48 81 EC ?? ?? ?? ?? 0F 29 70 C8 48 8B 05 "
    "?? ?? ?? ?? 48 33 C4 48 89 84 24 ?? ?? ?? "
    "?? 48 8B BC 24 ?? ?? ?? ?? 4C 8B F2 F2 0F "
    "10 B4 24 ?? ?? ?? ?? 45 33 FF 0F 28 DE 4C "
    "89 7C 24 ?? 48 8B D7 4C 89 7C 24 ?? 49 8B "
    "F0 E8 ?? ?? ?? ?? 48 98 4C 8D 44 24 ?? 66 "
    "44 89 3E 48 8D 2C 40 8B 54 EF 10 E8 ?? ?? "
    "?? ?? 8B D8 85 C0";
constexpr PCSTR kPatternGetBestFitUnitPair =
    "48 8B C4 48 89 58 08 55 56 57 41 54 41 55 "
    "41 56 41 57 48 8D A8 ?? ?? ?? ?? 48 81 EC "
    "?? ?? ?? ?? 0F 29 70 B8 0F 29 78 A8 44 0F "
    "29 40 98 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 "
    "89 45 60 48 8B BD ?? ?? ?? ?? 33 C9 4C 8B "
    "E2 F2 0F 10 B5 ?? ?? ?? ?? 4D 8B E8 66 41 "
    "89 08 44 8B F9 66 41 89 09 48 8B D7 4C 89 "
    "4C 24 ?? 0F 28 DE 48 89 4C 24 ?? 48 89 4C "
    "24 ?? 48 89 4C 24 ?? 48 89 4C 24 ?? 38 8D "
    "?? ?? ?? ?? 0F 85 ?? ?? ?? ?? E8 ?? ?? ?? "
    "?? F2 0F 10 BD ?? ?? ?? ??";
constexpr PCSTR kPatternGetBestFitUnitLinkSpeed =
    "48 89 5C 24 08 48 89 6C 24 18 56 57 41 56 "
    "48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? "
    "48 33 C4 48 89 84 24 ?? ?? ?? ?? 48 8B 9C "
    "24 ?? ?? ?? ?? 4C 8D 1D ?? ?? ?? ?? 33 ED "
    "4C 8B F2 48 89 6C 24 ?? 44 8B CD 48 89 6C "
    "24 ?? 66 89 2A 48 85 DB 74 ?? 44 8B C5 4C "
    "8D 15 ?? ?? ?? ?? 49 8B D3 49 3B D2 7D ?? "
    "0F 57 C9 48 85 DB 78 ?? F2 48 0F 2A CB EB "
    "?? 48 8B C3 48 8B CB 48 D1 E9 83 E0 01 48 "
    "0B C8 F2 48 0F 2A C9 F2 0F 58 C9 F2 0F 10 "
    "02 66 0F 2F C1 73 ?? 41 FF C0 48 83 C2 18 "
    "EB ??";
constexpr PCSTR kPatternTmPrintOrFormatBuffer =
    "40 53 55 56 57 41 54 41 55 41 56 41 57 48 "
    "81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 "
    "33 C4 48 89 84 24 ?? ?? ?? ?? 41 69 D9 6D "
    "4E C6 41 4C 8B FA 41 69 C1 CD 0D 01 00 4D "
    "8B E0 41 B8 01 00 00 00 41 8B E9 41 B9 AB "
    "AA AA AA 4C 8B F1 81 C3 39 30 00 00 41 03 "
    "C0 C1 EB 10 25 00 00 FF FF 0B D8 41 8B C1 "
    "F7 E3 C1 EA 09 8D 04 52 C1 E0 08 2B D8 45 "
    "33 ED 41 0F B7 FD B8 00 03 00 00 48 8D 0D "
    "?? ?? ?? ?? 66 3B F8 0F 83 ?? ?? ?? ?? 0F "
    "B7 C3 48 03 C0 83 7C C1 ?? FF 0F 84 ?? ?? "
    "?? ?? 3B 6C C1 ?? 0F 85 ?? ?? ?? ?? 48 8B "
    "7C C1 ??";
constexpr PCSTR kPatternGetColumnText =
    "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 "
    "18 57 41 56 41 57 48 83 EC ?? 41 0F B6 18 "
    "49 8B F9 48 83 22 00 49 8B E8 0F 29 70 D8 "
    "48 8B F2 4C 8B F1 48 8D 0D ?? ?? ?? ?? E8 "
    "?? ?? ?? ?? 44 8D 53 F9 41 83 FA 04 76 6B "
    "83 FB 0E 74 66 84 C0 74 05 83 FB 20 74 5D "
    "83 FB 0F 0F 84 ?? ?? ?? ?? 8B 84 24 ?? ?? "
    "?? ?? 4C 8B CF 89 44 24 ?? 4C 8B C5 48 8B "
    "84 24 ?? ?? ?? ?? 48 8B D6 49 8B CE 48 89 "
    "44 24 ?? E8 ?? ?? ?? ?? 8B D8 85 C0 0F 88 "
    "?? ?? ?? ?? 48 8B 6C 24 ?? 8B C3 48 8B 5C "
    "24 ?? 48 8B 74 24 ?? 0F 28 74 24 30 48 83 "
    "C4 40 41 5F 41 5E 5F C3";
constexpr PCSTR kPatternWdcStringMapLoadStringW =
    "48 8B C4 48 89 58 10 48 89 68 18 48 89 70 "
    "20 48 89 48 08 57 41 54 41 55 41 56 41 57 "
    "48 83 EC ?? 69 F2 6D 4E C6 41 4C 8D 25 ?? "
    "?? ?? ?? 69 C2 CD 0D 01 00 4D 8B F8 44 8B "
    "F2 41 B8 01 00 00 00 41 B9 AB AA AA AA 81 "
    "C6 39 30 00 00 41 03 C0 C1 EE 10 25 00 00 "
    "FF FF 0B F0 41 8B C1 F7 E6 C1 EA 09 8D 04 "
    "52 C1 E0 08 2B F0 33 FF 0F B7 EF 49 89 3F "
    "B8 00 03 00 00 66 3B E8 73 ?? 0F B7 CE 8B "
    "C1 48 03 C0 41 83 7C C4 ?? FF 74 ?? 45 3B "
    "74 C4 ?? 75 ?? 48 03 C9 49 8B 44 CC ?? 49 "
    "89 07 33 C0 48 8B 5C 24 ?? 48 8B 6C 24 ?? "
    "48 8B 74 24 ?? 48 83 C4 30 41 5F 41 5E 41 "
    "5D 41 5C 5F C3";
constexpr PCSTR kPatternWdcLoadString =
    "48 89 5C 24 08 57 48 83 EC ?? 49 81 F8 FF "
    "FF FF 7F 77 ?? 49 63 C0 49 3B C0 0F 87 ?? "
    "?? ?? ?? 45 8B C8 4C 8B C2 8B D1 48 8B 0D "
    "?? ?? ?? ?? 48 FF 15 ?? ?? ?? ?? 0F 1F 44 "
    "00 00 85 C0 7E ?? 33 DB 8B C3 48 8B 5C 24 "
    "?? 48 83 C4 30 5F C3 CC BB 16 02 07 80 48 "
    "FF 15 ?? ?? ?? ?? 0F 1F 44 00 00 41 B8 27 "
    "00 00 00 89 5C 24 ?? 4C 8D 0D ?? ?? ?? ?? "
    "89 44 24 ?? 48 8D 15 ?? ?? ?? ?? 48 8D 0D "
    "?? ?? ?? ?? E8 ?? ?? ?? ?? EB ?? 48 FF 15 "
    "?? ?? ?? ?? 0F 1F 44 00 00 8B D8 85 C0 74 "
    "?? 7E ?? 0F B7 D8 81 CB 00 00 07 80 8B FB "
    "85 DB 79 ??";

struct ATM_INSTANT_VALUE_SCALE {
    double threshold;
    double multiplier;
    DWORD stringId;
    DWORD padding;
};
static_assert(sizeof(ATM_INSTANT_VALUE_SCALE) == 0x18);

// Same shape as Task Manager's network table (Kbps, Mbps, Gbps), with byte
// units. Values are divided by 8 before they reach it. Each unit switches to
// the next one where it would round up to "1,000" (999.5 and above).
//
// Task Manager reads exactly 3 rows: its scaleCount argument is unused, and
// the callers never set it. The 4th row is spare, so a later build that reads
// one more row still stays within this table.
constexpr ATM_INSTANT_VALUE_SCALE kByteScale[] = {
    {999499.999, 1e-3, kIdKBytesPerSec, 0},
    {999499999.999, 1e-6, kIdMBytesPerSec, 0},
    {999999999999.0, 1e-9, kIdGBytesPerSec, 0},
    {1e300, 1e-9, kIdGBytesPerSec, 0},
};

// Settings.
bool g_englishUnits = true;

// Identified by the unit string IDs. scaleCount can't be used: see kByteScale.
bool IsNetworkScale(const ATM_INSTANT_VALUE_SCALE* scale) {
    return scale && scale[0].stringId == kIdKbps &&
           scale[1].stringId == kIdMbps && scale[2].stringId == kIdGbps;
}

// Network graph scale labels: string ID and the bits/s value it names.
struct NetworkLabel {
    DWORD id;
    double bitsPerSec;
};

constexpr NetworkLabel kNetworkLabels[] = {
    {32808, 1},     {32809, 1e4},   {32810, 1e6},   {32811, 1e7},
    {32812, 1e8},   {32813, 1e9},   {32814, 1e10},  {32815, 1e11},
    {32816, 1e5},   {32820, 1e5},   {32821, 5e5},   {32822, 1e6},
    {32823, 11e6},  {32824, 54e6},  {32825, 1e8},   {32826, 5e8},
    {32827, 1e9},   {32828, 1e10},  {32829, 1e11},  {32830, 45e4},
    {32831, 8e5},   {32832, 77e5},  {32833, 32e6},  {32834, 5e7},
    {32835, 2e8},   {32836, 3e8},   {32837, 2e9},   {32838, 1e10},
    {32884, 5e6},   {32885, 26e6},  {32886, 25e7},  {32887, 36e5},
    {32888, 17e6},  {32889, 113e6},
};

// Built once, never freed: Task Manager may hold these pointers after the mod
// unloads. nullptr means "leave this ID alone".
PCWSTR g_networkLabelText[ARRAYSIZE(kNetworkLabels)];
INIT_ONCE g_initOnce = INIT_ONCE_STATIC_INIT;

WCHAR g_decimalSep[8] = L".";
WCHAR g_thousandSep[8] = L",";
NUMBERFMTW g_localeNumberFormat = {0, 1, 3, g_decimalSep, g_thousandSep, 1};

// Strings loaded once. g_byteUnits are English or localized per the
// "unitNames" setting; g_byteUnitText holds never-freed copies for the hooks.
std::wstring g_valueUnitPattern = kDefaultValueUnitPattern;
std::wstring g_byteUnits[4];  // B/s, KB/s, MB/s, GB/s
PCWSTR g_byteUnitText[4];
std::wstring g_networkColumnPrefix;
// Whether string 37034 is still the "%s Mbps" network column format.
bool g_networkColumnFormatValid;

thread_local bool t_hasRawNetworkMbps;
thread_local double t_rawNetworkMbps;

// Set while a network GetBestFitUnit call runs, so the string hook can answer
// its byte unit IDs with the English names without affecting other callers.
thread_local bool t_inNetworkBestFitUnit;

bool IsBidiMark(WCHAR c) {
    return c == 0x200E || c == 0x200F || c == 0x061C ||
           (c >= 0x202A && c <= 0x202E) || (c >= 0x2066 && c <= 0x2069);
}

std::wstring LoadTmString(DWORD id) {
    WCHAR buffer[256];
    int len = LoadStringW(GetModuleHandleW(nullptr), id, buffer,
                          ARRAYSIZE(buffer));
    return len > 0 ? std::wstring(buffer, len) : std::wstring();
}

std::wstring LeadingBidiMarks(const std::wstring& s) {
    size_t i = 0;
    while (i < s.size() && IsBidiMark(s[i])) {
        i++;
    }
    return s.substr(0, i);
}

std::wstring StripBidiMarks(const std::wstring& s) {
    std::wstring result;
    for (WCHAR c : s) {
        if (!IsBidiMark(c)) {
            result += c;
        }
    }
    return result;
}

PCWSTR CopyToProcessHeap(const std::wstring& s) {
    size_t bytes = (s.size() + 1) * sizeof(WCHAR);
    auto* copy = static_cast<PWSTR>(HeapAlloc(GetProcessHeap(), 0, bytes));
    if (copy) {
        memcpy(copy, s.c_str(), bytes);
    }
    return copy;
}

// Whether `pattern` is safe for FormatMessageW with the two string arguments of
// ComposeValueUnit: it uses inserts %1 and %2, both as strings, and no other.
bool IsValidValueUnitPattern(const std::wstring& pattern) {
    bool has1 = false;
    bool has2 = false;
    for (size_t i = 0; i < pattern.size(); i++) {
        if (pattern[i] != L'%' || i + 1 >= pattern.size()) {
            continue;
        }
        WCHAR next = pattern[i + 1];
        if (next < L'1' || next > L'9') {
            i++;  // An escape like %% or %n, not an insert.
            continue;
        }

        size_t end = i + 1;
        int insert = 0;
        // FormatMessageW only reads ASCII digits as an insert number.
        while (end < pattern.size() && pattern[end] >= L'0' &&
               pattern[end] <= L'9') {
            insert = insert * 10 + (pattern[end] - L'0');
            end++;
        }
        if (insert != 1 && insert != 2) {
            return false;
        }
        if (end < pattern.size() && pattern[end] == L'!' &&
            pattern.compare(end, 3, L"!s!") != 0) {
            return false;
        }
        (insert == 1 ? has1 : has2) = true;
        i = end - 1;
    }
    return has1 && has2;
}

void LoadStrings() {
    std::wstring pattern = LoadTmString(kIdValueUnitPattern);
    if (IsValidValueUnitPattern(pattern)) {
        g_valueUnitPattern = pattern;
    } else {
        Wh_Log(L"Using the default value/unit pattern instead of \"%s\"",
               pattern.c_str());
    }

    for (int i = 0; i < 4; i++) {
        if (!g_englishUnits) {
            g_byteUnits[i] = LoadTmString(kIdBytesPerSec + i);
        }
        if (g_byteUnits[i].empty()) {
            g_byteUnits[i] = kEnglishByteUnits[i];
        }
        g_byteUnitText[i] = CopyToProcessHeap(g_byteUnits[i]);
    }

    std::wstring columnFormat = LoadTmString(kIdNetworkColumnMbps);
    std::wstring mbps = StripBidiMarks(LoadTmString(kIdMbps));
    g_networkColumnFormatValid =
        !mbps.empty() && columnFormat.find(L"%s") != std::wstring::npos &&
        StripBidiMarks(columnFormat).find(mbps) != std::wstring::npos;
    if (!g_networkColumnFormatValid) {
        Wh_Log(L"Not converting the Network column: string %u is \"%s\"",
               kIdNetworkColumnMbps, columnFormat.c_str());
    }
    g_networkColumnPrefix = LeadingBidiMarks(columnFormat);
}

// "%1!s! %2!s!" with a value and a byte unit (kIdBytesPerSec..kIdGBytesPerSec).
std::wstring ComposeValueUnit(const std::wstring& value, DWORD unitId) {
    const std::wstring& unit = g_byteUnits[unitId - kIdBytesPerSec];

    DWORD_PTR args[] = {reinterpret_cast<DWORD_PTR>(value.c_str()),
                        reinterpret_cast<DWORD_PTR>(unit.c_str())};
    WCHAR buffer[256];
    DWORD len = FormatMessageW(
        FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
        g_valueUnitPattern.c_str(), 0, 0, buffer, ARRAYSIZE(buffer),
        reinterpret_cast<va_list*>(args));
    if (len == 0) {
        return value + L" " + unit;
    }
    return std::wstring(buffer, len);
}

// Formats `value` with `digits` decimals. With `format`, uses that number
// format (locale separators); otherwise plain "%.*f".
std::wstring FormatNumber(double value, int digits, const NUMBERFMTW* format) {
    WCHAR raw[64];
    swprintf_s(raw, ARRAYSIZE(raw), L"%.*f", digits, value);
    if (!format) {
        return raw;
    }

    NUMBERFMTW fmt = *format;
    fmt.NumDigits = digits;
    WCHAR formatted[64];
    if (!GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, raw, &fmt, formatted,
                           ARRAYSIZE(formatted))) {
        return raw;
    }
    return formatted;
}

// Up to 2 decimals below 10, 1 below 100, none above, trailing zeros dropped:
// 1.25, 6.75, 12.5, 62.5, 125, 963.
std::wstring FormatSmart(double value) {
    int maxDigits = value >= 100 ? 0 : value >= 10 ? 1 : 2;
    double maxScale = std::pow(10.0, maxDigits);
    double rounded = std::round(value * maxScale) / maxScale;

    int digits = 0;
    while (digits < maxDigits) {
        double scale = std::pow(10.0, digits);
        if (std::fabs(std::round(rounded * scale) / scale - rounded) < 1e-9) {
            break;
        }
        digits++;
    }
    return FormatNumber(rounded, digits, &g_localeNumberFormat);
}

// e.g. 125000000 -> "125 MB/s".
std::wstring FormatBytesPerSec(double bytesPerSec) {
    DWORD unitId;
    double scaled;
    if (bytesPerSec > 0 && bytesPerSec < 1e3) {
        unitId = kIdBytesPerSec;
        scaled = bytesPerSec;
    } else if (bytesPerSec < 1e6) {
        unitId = kIdKBytesPerSec;
        scaled = bytesPerSec / 1e3;
    } else if (bytesPerSec < 1e9) {
        unitId = kIdMBytesPerSec;
        scaled = bytesPerSec / 1e6;
    } else {
        unitId = kIdGBytesPerSec;
        scaled = bytesPerSec / 1e9;
    }
    return ComposeValueUnit(FormatSmart(scaled), unitId);
}

UINT ParseGrouping(PCWSTR grouping) {
    UINT result = 0;
    for (PCWSTR p = grouping; *p; p++) {
        if (*p >= L'0' && *p <= L'9') {
            result = result * 10 + (*p - L'0');
        }
    }
    size_t len = wcslen(grouping);
    if (len >= 2 && wcscmp(grouping + len - 2, L";0") == 0) {
        return result / 10;  // "3;0" -> 3, "3;2;0" -> 32
    }
    return result * 10;  // "3" -> 30
}

void InitLocaleNumberFormat() {
    GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, g_decimalSep,
                    ARRAYSIZE(g_decimalSep));
    GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_STHOUSAND, g_thousandSep,
                    ARRAYSIZE(g_thousandSep));

    DWORD value;
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                        LOCALE_ILZERO | LOCALE_RETURN_NUMBER,
                        reinterpret_cast<LPWSTR>(&value),
                        sizeof(value) / sizeof(WCHAR))) {
        g_localeNumberFormat.LeadingZero = value;
    }
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                        LOCALE_INEGNUMBER | LOCALE_RETURN_NUMBER,
                        reinterpret_cast<LPWSTR>(&value),
                        sizeof(value) / sizeof(WCHAR))) {
        g_localeNumberFormat.NegativeOrder = value;
    }
    WCHAR grouping[16];
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SGROUPING, grouping,
                        ARRAYSIZE(grouping))) {
        g_localeNumberFormat.Grouping = ParseGrouping(grouping);
    }
}

// The bit unit string ID a label for `bitsPerSec` is expected to contain.
DWORD ExpectedBitUnitId(double bitsPerSec) {
    return bitsPerSec < 1e3   ? kIdBps
           : bitsPerSec < 1e6 ? kIdKbps
           : bitsPerSec < 1e9 ? kIdMbps
                              : kIdGbps;
}

// The bit unit a localized label uses, or 0 if none matches.
DWORD DetectBitUnitId(const std::wstring& label,
                      const std::wstring (&units)[4]) {
    // Prefixed units first: "Kbps" contains "bps".
    constexpr DWORD ids[] = {kIdKbps, kIdMbps, kIdGbps, kIdBps};
    constexpr int indexes[] = {1, 2, 3, 0};
    for (int i = 0; i < 4; i++) {
        const std::wstring& unit = units[indexes[i]];
        if (!unit.empty() && label.find(unit) != std::wstring::npos) {
            return ids[i];
        }
    }
    return 0;
}

void BuildNetworkLabels() {
    std::wstring bitUnits[4] = {
        StripBidiMarks(LoadTmString(kIdBps)),
        StripBidiMarks(LoadTmString(kIdKbps)),
        StripBidiMarks(LoadTmString(kIdMbps)),
        StripBidiMarks(LoadTmString(kIdGbps)),
    };

    for (size_t i = 0; i < ARRAYSIZE(kNetworkLabels); i++) {
        const NetworkLabel& label = kNetworkLabels[i];
        std::wstring original = LoadTmString(label.id);
        DWORD expected = ExpectedBitUnitId(label.bitsPerSec);
        DWORD detected = DetectBitUnitId(StripBidiMarks(original), bitUnits);
        if (detected != expected) {
            Wh_Log(L"Not remapping label %u (\"%s\"): unexpected unit",
                   label.id, original.c_str());
            continue;
        }

        g_networkLabelText[i] =
            CopyToProcessHeap(LeadingBidiMarks(original) +
                              FormatBytesPerSec(label.bitsPerSec / 8));
    }
}

BOOL CALLBACK InitOnceCallback(PINIT_ONCE, PVOID, PVOID*) {
    InitLocaleNumberFormat();
    LoadStrings();
    BuildNetworkLabels();
    return TRUE;
}

void EnsureInitialized() {
    InitOnceExecuteOnce(&g_initOnce, InitOnceCallback, nullptr, nullptr);
}

// In English mode, the English name for a byte unit ID that a network
// GetBestFitUnit call loads, or nullptr. Other callers of the same IDs, such as
// the disk view, keep Task Manager's names.
PCWSTR GetNetworkUnitText(DWORD id) {
    if (!t_inNetworkBestFitUnit || !g_englishUnits || id < kIdKBytesPerSec ||
        id > kIdGBytesPerSec) {
        return nullptr;
    }
    EnsureInitialized();
    return g_byteUnitText[id - kIdBytesPerSec];
}

// The replacement for a network graph label ID, or nullptr.
PCWSTR GetNetworkLabelText(DWORD id) {
    if (id < kNetworkLabels[0].id ||
        id > kNetworkLabels[ARRAYSIZE(kNetworkLabels) - 1].id) {
        return nullptr;
    }
    for (size_t i = 0; i < ARRAYSIZE(kNetworkLabels); i++) {
        if (kNetworkLabels[i].id == id) {
            EnsureInitialized();
            return g_networkLabelText[i];
        }
    }
    return nullptr;
}

// Performance page Send / Receive values.
using GetBestFitUnit_t = HRESULT(WINAPI*)(void* pThis,
                                          NUMBERFMTW* numberFormat,
                                          PWSTR output,
                                          DWORD outputChars,
                                          const ATM_INSTANT_VALUE_SCALE* scale,
                                          int scaleCount,
                                          double value,
                                          int flags);
GetBestFitUnit_t GetBestFitUnit_Original;
HRESULT WINAPI GetBestFitUnit_Hook(void* pThis,
                                   NUMBERFMTW* numberFormat,
                                   PWSTR output,
                                   DWORD outputChars,
                                   const ATM_INSTANT_VALUE_SCALE* scale,
                                   int scaleCount,
                                   double value,
                                   int flags) {
    if (!IsNetworkScale(scale)) {
        return GetBestFitUnit_Original(pThis, numberFormat, output,
                                       outputChars, scale, scaleCount, value,
                                       flags);
    }

    bool wasInNetworkBestFitUnit = t_inNetworkBestFitUnit;
    t_inNetworkBestFitUnit = true;
    HRESULT hr = GetBestFitUnit_Original(pThis, numberFormat, output,
                                         outputChars, kByteScale, scaleCount,
                                         value / 8, flags);
    t_inNetworkBestFitUnit = wasInNetworkBestFitUnit;
    return hr;
}

// Performance sidebar "S: ... R: ..." pair.
using GetBestFitUnitPair_t =
    HRESULT(WINAPI*)(void* pThis,
                     NUMBERFMTW* numberFormat,
                     PWSTR output1,
                     PWSTR output2,
                     DWORD outputChars,
                     const ATM_INSTANT_VALUE_SCALE* scale,
                     int scaleCount,
                     double value1,
                     double value2,
                     int formatId1,
                     int formatId2,
                     bool sharedUnit);
GetBestFitUnitPair_t GetBestFitUnitPair_Original;
HRESULT WINAPI GetBestFitUnitPair_Hook(void* pThis,
                                       NUMBERFMTW* numberFormat,
                                       PWSTR output1,
                                       PWSTR output2,
                                       DWORD outputChars,
                                       const ATM_INSTANT_VALUE_SCALE* scale,
                                       int scaleCount,
                                       double value1,
                                       double value2,
                                       int formatId1,
                                       int formatId2,
                                       bool sharedUnit) {
    if (!IsNetworkScale(scale)) {
        return GetBestFitUnitPair_Original(
            pThis, numberFormat, output1, output2, outputChars, scale,
            scaleCount, value1, value2, formatId1, formatId2, sharedUnit);
    }

    bool wasInNetworkBestFitUnit = t_inNetworkBestFitUnit;
    t_inNetworkBestFitUnit = true;
    HRESULT hr = GetBestFitUnitPair_Original(
        pThis, numberFormat, output1, output2, outputChars, kByteScale,
        scaleCount, value1 / 8, value2 / 8, formatId1, formatId2, sharedUnit);
    t_inNetworkBestFitUnit = wasInNetworkBestFitUnit;
    return hr;
}

// "View network details" link speed. This overload is only used for network
// values: it ignores `scale` and always uses the network table. `outputChars`
// is garbage at the call site; the original writes at most 0x80 characters.
using GetBestFitUnitLinkSpeed_t =
    HRESULT(WINAPI*)(void* pThis,
                     PWSTR output,
                     DWORD outputChars,
                     const ATM_INSTANT_VALUE_SCALE* scale,
                     int scaleCount,
                     ULONGLONG bitsPerSec,
                     int flags);
GetBestFitUnitLinkSpeed_t GetBestFitUnitLinkSpeed_Original;
HRESULT WINAPI
GetBestFitUnitLinkSpeed_Hook(void* /*pThis*/,
                             PWSTR output,
                             DWORD /*outputChars*/,
                             const ATM_INSTANT_VALUE_SCALE* /*scale*/,
                             int /*scaleCount*/,
                             ULONGLONG bitsPerSec,
                             int /*flags*/) {
    EnsureInitialized();
    std::wstring text =
        FormatBytesPerSec(static_cast<double>(bitsPerSec) / 8);
    return StringCchCopyW(output, kLinkSpeedBufferChars, text.c_str());
}

// Processes / Users tabs: remembers the network value before Task Manager
// raises small values to 0.1 Mbps, for TmPrintOrFormatBuffer_Hook.
using GetColumnText_t = HRESULT(WINAPI*)(void* pThis,
                                         PWSTR* outputText,
                                         const BYTE* column,
                                         PWSTR buffer,
                                         ULONGLONG bufferChars,
                                         int flags);
GetColumnText_t GetColumnText_Original;
HRESULT WINAPI GetColumnText_Hook(void* pThis,
                                  PWSTR* outputText,
                                  const BYTE* column,
                                  PWSTR buffer,
                                  ULONGLONG bufferChars,
                                  int flags) {
    bool isNetwork = column && column[0] == kNetworkColumnType;
    if (isNetwork) {
        t_rawNetworkMbps = *reinterpret_cast<const double*>(column + 0x10);
        t_hasRawNetworkMbps = true;
    }
    HRESULT hr = GetColumnText_Original(pThis, outputText, column, buffer,
                                        bufferChars, flags);
    if (isNetwork) {
        t_hasRawNetworkMbps = false;
    }
    return hr;
}

// The Network column value as Task Manager computed it, before rounding up.
double GetNetworkColumnMbps(double passedMbps) {
    if (!t_hasRawNetworkMbps) {
        return passedMbps;
    }
    double raw = t_rawNetworkMbps;
    if (raw == passedMbps ||
        (raw > 0 && raw < kNetworkColumnMinMbps &&
         passedMbps == kNetworkColumnMinMbps)) {
        return raw;
    }
    return passedMbps;
}

// Processes / Users tabs "Network" column: "%s Mbps" -> "x KB/s" / "x MB/s".
using TmPrintOrFormatBuffer_t = HRESULT(WINAPI*)(PWSTR buffer,
                                                 SIZE_T bufferChars,
                                                 NUMBERFMTW* numberFormat,
                                                 DWORD formatId,
                                                 double value,
                                                 int useNumberFormat);
TmPrintOrFormatBuffer_t TmPrintOrFormatBuffer_Original;
HRESULT WINAPI TmPrintOrFormatBuffer_Hook(PWSTR buffer,
                                          SIZE_T bufferChars,
                                          NUMBERFMTW* numberFormat,
                                          DWORD formatId,
                                          double value,
                                          int useNumberFormat) {
    if (formatId != kIdNetworkColumnMbps) {
        return TmPrintOrFormatBuffer_Original(buffer, bufferChars,
                                              numberFormat, formatId, value,
                                              useNumberFormat);
    }

    EnsureInitialized();
    if (!g_networkColumnFormatValid) {
        return TmPrintOrFormatBuffer_Original(buffer, bufferChars,
                                              numberFormat, formatId, value,
                                              useNumberFormat);
    }

    double kbps = GetNetworkColumnMbps(value) * 125;  // 1 Mbps = 125 KB/s
    if (kbps < 0) {
        kbps = 0;
    } else if (kbps > 0 && kbps < 0.1) {
        kbps = 0.1;
    }

    DWORD unitId;
    double scaled;
    int digits;
    if (kbps >= 999.95) {
        unitId = kIdMBytesPerSec;
        scaled = kbps / 1000;
        digits = 1;
    } else {
        unitId = kIdKBytesPerSec;
        scaled = kbps;
        digits = kbps == 0 ? 0 : kbps >= 99.95 ? 0 : 1;
    }

    const NUMBERFMTW* format =
        useNumberFormat && numberFormat ? numberFormat : nullptr;
    std::wstring text =
        g_networkColumnPrefix +
        ComposeValueUnit(FormatNumber(scaled, digits, format), unitId);
    return StringCchCopyW(buffer, bufferChars, text.c_str());
}

// Graph scale labels and English unit names, through Task Manager's string
// cache. Replacements are returned without entering the cache.
using WdcStringMap_LoadStringW_t = HRESULT(WINAPI*)(void* pThis,
                                                    DWORD id,
                                                    PWSTR* string);
WdcStringMap_LoadStringW_t WdcStringMap_LoadStringW_Original;
HRESULT WINAPI WdcStringMap_LoadStringW_Hook(void* pThis,
                                             DWORD id,
                                             PWSTR* string) {
    PCWSTR text = GetNetworkUnitText(id);
    if (!text) {
        text = GetNetworkLabelText(id);
    }
    if (text) {
        *string = const_cast<PWSTR>(text);
        return S_OK;
    }
    return WdcStringMap_LoadStringW_Original(pThis, id, string);
}

// Graph scale labels, uncached loads. Unit names are not replaced here: when
// the cache calls this, the result is cached and shared with the disk view.
using WdcLoadString_t = HRESULT(WINAPI*)(DWORD id,
                                         PWSTR buffer,
                                         ULONGLONG bufferChars);
WdcLoadString_t WdcLoadString_Original;
HRESULT WINAPI WdcLoadString_Hook(DWORD id,
                                  PWSTR buffer,
                                  ULONGLONG bufferChars) {
    if (PCWSTR label = GetNetworkLabelText(id)) {
        return StringCchCopyW(buffer, bufferChars, label);
    }
    return WdcLoadString_Original(id, buffer, bufferChars);
}

struct PatternByte {
    BYTE value;
    bool exact;
};

struct Pattern {
    PatternByte bytes[256];
    size_t count;
};

int HexDigit(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

// Parses a space-separated byte pattern. '??' matches any byte.
bool ParsePattern(PCSTR text, Pattern* pattern) {
    pattern->count = 0;
    for (PCSTR p = text; *p;) {
        if (*p == ' ' || *p == '\t') {
            p++;
            continue;
        }
        if (pattern->count == ARRAYSIZE(pattern->bytes)) {
            return false;
        }
        if (p[0] == '?' && p[1] == '?') {
            pattern->bytes[pattern->count++] = {0, false};
            p += 2;
            continue;
        }
        int high = HexDigit(p[0]);
        int low = HexDigit(p[1]);
        if (high < 0 || low < 0) {
            return false;
        }
        pattern->bytes[pattern->count++] = {
            static_cast<BYTE>((high << 4) | low), true};
        p += 2;
    }
    return pattern->count != 0;
}

bool MatchesPattern(const BYTE* address, const Pattern& pattern) {
    for (size_t i = 0; i < pattern.count; i++) {
        if (pattern.bytes[i].exact && address[i] != pattern.bytes[i].value) {
            return false;
        }
    }
    return true;
}

const IMAGE_SECTION_HEADER* FindTextSection(const IMAGE_NT_HEADERS* nt) {
    const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++, section++) {
        if (memcmp(section->Name, ".text", 5) == 0) {
            return section;
        }
    }
    return nullptr;
}

bool GetTextRange(HMODULE module,
                  const IMAGE_NT_HEADERS* nt,
                  const BYTE** start,
                  const BYTE** end) {
    const IMAGE_SECTION_HEADER* section = FindTextSection(nt);
    if (!section) {
        return false;
    }
    size_t imageSize = nt->OptionalHeader.SizeOfImage;
    size_t sectionSize = section->Misc.VirtualSize
                             ? section->Misc.VirtualSize
                             : section->SizeOfRawData;
    if (section->VirtualAddress > imageSize ||
        sectionSize > imageSize - section->VirtualAddress) {
        return false;
    }
    *start = reinterpret_cast<const BYTE*>(module) + section->VirtualAddress;
    *end = *start + sectionSize;
    return true;
}

// A RUNTIME_FUNCTION entry from the exception directory (.pdata).
struct PdataEntry {
    DWORD beginAddress;
    DWORD endAddress;
    DWORD unwindInfo;
};
static_assert(sizeof(PdataEntry) == 0xC);

// Matches `pattern` at the start of a function from the exception directory
// (.pdata). Moving a function to a different address doesn't matter because
// the function table supplies the candidates. The match must be unique; a
// pattern that fits more than one function is refused.
const BYTE* FindPatternInFunctions(HMODULE module,
                                   const IMAGE_NT_HEADERS* nt,
                                   const Pattern& pattern) {
    const BYTE* textStart;
    const BYTE* textEnd;
    if (!GetTextRange(module, nt, &textStart, &textEnd)) {
        return nullptr;
    }

    const IMAGE_DATA_DIRECTORY& directory =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
    size_t imageSize = nt->OptionalHeader.SizeOfImage;
    if (directory.VirtualAddress == 0 ||
        directory.Size < sizeof(PdataEntry) ||
        directory.Size % sizeof(PdataEntry) != 0 ||
        directory.VirtualAddress > imageSize ||
        directory.Size > imageSize - directory.VirtualAddress) {
        return nullptr;
    }

    const BYTE* base = reinterpret_cast<const BYTE*>(module);
    size_t textStartOffset = static_cast<size_t>(textStart - base);
    size_t textEndOffset = static_cast<size_t>(textEnd - base);
    const auto* functions = reinterpret_cast<const PdataEntry*>(
        base + directory.VirtualAddress);
    size_t functionCount = directory.Size / sizeof(PdataEntry);

    const BYTE* match = nullptr;

    for (size_t i = 0; i < functionCount; i++) {
        size_t offset = functions[i].beginAddress;
        if (offset < textStartOffset || offset > textEndOffset ||
            pattern.count > textEndOffset - offset) {
            continue;
        }

        const BYTE* start = base + offset;
        if (start == match || !MatchesPattern(start, pattern)) {
            continue;
        }
        if (match) {
            Wh_Log(L"Byte pattern matches more than one function");
            return nullptr;
        }
        match = start;
    }

    return match;
}

// Finds `pattern` at a function start. Returns nullptr unless it matches
// exactly one function.
const BYTE* FindPattern(HMODULE module, PCSTR patternText) {
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return nullptr;
    }
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
        reinterpret_cast<const BYTE*>(module) + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        return nullptr;
    }

    Pattern pattern;
    if (!ParsePattern(patternText, &pattern)) {
        return nullptr;
    }

    return FindPatternInFunctions(module, nt, pattern);
}

// Hooks `pattern` if it can be found unambiguously. Used only for functions
// that HookSymbols couldn't resolve.
template <typename Prototype>
bool HookByPattern(PCSTR pattern,
                   Prototype* hookFunction,
                   Prototype** originalFunction,
                   PCWSTR name) {
    const BYTE* target = FindPattern(GetModuleHandleW(nullptr), pattern);
    if (!target) {
        Wh_Log(L"%s: byte pattern not found or ambiguous", name);
        return false;
    }
    if (!Wh_SetFunctionHook(
            const_cast<BYTE*>(target), reinterpret_cast<void*>(hookFunction),
            reinterpret_cast<void**>(originalFunction))) {
        Wh_Log(L"%s: Wh_SetFunctionHook failed", name);
        return false;
    }
    Wh_Log(L"%s: hooked at %p (byte pattern)", name, target);
    return true;
}

void LoadSettings() {
    auto unitNames = WindhawkUtils::StringSetting::make(L"unitNames");
    g_englishUnits = wcscmp(unitNames, L"localized") != 0;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    WindhawkUtils::SYMBOL_HOOK taskmgrExeHooks[] = {
        {
            {LR"(protected: long __cdecl AtmBaseView::GetBestFitUnit(struct _numberfmtW *,unsigned short *,unsigned long,struct ATM_INSTANT_VALUE_SCALE const *,int,double,int))"},
            &GetBestFitUnit_Original,
            GetBestFitUnit_Hook,
            true,
        },
        {
            {LR"(protected: long __cdecl AtmBaseView::GetBestFitUnit(struct _numberfmtW *,unsigned short *,unsigned short *,unsigned long,struct ATM_INSTANT_VALUE_SCALE const *,int,double,double,int,int,bool))"},
            &GetBestFitUnitPair_Original,
            GetBestFitUnitPair_Hook,
            true,
        },
        {
            {LR"(protected: long __cdecl AtmBaseView::GetBestFitUnit(unsigned short *,unsigned long,struct ATM_INSTANT_VALUE_SCALE const *,int,unsigned __int64,int))"},
            &GetBestFitUnitLinkSpeed_Original,
            GetBestFitUnitLinkSpeed_Hook,
            true,
        },
        {
            {LR"(long __cdecl TmPrintOrFormatBuffer(unsigned short *,unsigned __int64,struct _numberfmtW *,unsigned long,double,int))"},
            &TmPrintOrFormatBuffer_Original,
            TmPrintOrFormatBuffer_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl WdcApplicationsMonitor::GetColumnText(unsigned short * *,struct _WDC_DATA_COLUMN const *,unsigned short *,unsigned __int64,int))"},
            &GetColumnText_Original,
            GetColumnText_Hook,
            true,
        },
        {
            {LR"(public: long __cdecl WdcStringMap::LoadStringW(unsigned long,unsigned short * *))"},
            &WdcStringMap_LoadStringW_Original,
            WdcStringMap_LoadStringW_Hook,
            true,
        },
        {
            {LR"(long __cdecl WdcLoadString(unsigned long,unsigned short *,unsigned __int64))"},
            &WdcLoadString_Original,
            WdcLoadString_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(GetModuleHandleW(nullptr), taskmgrExeHooks,
                                    ARRAYSIZE(taskmgrExeHooks))) {
        Wh_Log(L"Task Manager symbols are unavailable; trying byte patterns");
    }

    // Microsoft's symbol server can lag behind a Windows update. The patterns
    // are only used for functions the symbol lookup didn't resolve, so normal
    // symbol-based hooking stays in charge whenever a PDB is available.
    if (!GetBestFitUnit_Original) {
        HookByPattern(kPatternGetBestFitUnit, GetBestFitUnit_Hook,
                      &GetBestFitUnit_Original,
                      L"GetBestFitUnit (Send/Receive)");
    }
    if (!GetBestFitUnitPair_Original) {
        HookByPattern(kPatternGetBestFitUnitPair, GetBestFitUnitPair_Hook,
                      &GetBestFitUnitPair_Original,
                      L"GetBestFitUnit (sidebar)");
    }
    if (!GetBestFitUnitLinkSpeed_Original) {
        HookByPattern(kPatternGetBestFitUnitLinkSpeed,
                      GetBestFitUnitLinkSpeed_Hook,
                      &GetBestFitUnitLinkSpeed_Original,
                      L"GetBestFitUnit (link speed)");
    }
    if (!TmPrintOrFormatBuffer_Original) {
        HookByPattern(kPatternTmPrintOrFormatBuffer,
                      TmPrintOrFormatBuffer_Hook,
                      &TmPrintOrFormatBuffer_Original,
                      L"TmPrintOrFormatBuffer");
    }
    if (!GetColumnText_Original) {
        HookByPattern(kPatternGetColumnText, GetColumnText_Hook,
                      &GetColumnText_Original,
                      L"WdcApplicationsMonitor::GetColumnText");
    }
    if (!WdcStringMap_LoadStringW_Original) {
        HookByPattern(kPatternWdcStringMapLoadStringW,
                      WdcStringMap_LoadStringW_Hook,
                      &WdcStringMap_LoadStringW_Original,
                      L"WdcStringMap::LoadStringW");
    }
    if (!WdcLoadString_Original) {
        HookByPattern(kPatternWdcLoadString, WdcLoadString_Hook,
                      &WdcLoadString_Original, L"WdcLoadString");
    }

    struct {
        PCWSTR name;
        bool found;
    } const results[] = {
        {L"GetBestFitUnit (Send/Receive)", GetBestFitUnit_Original != nullptr},
        {L"GetBestFitUnit (sidebar)", GetBestFitUnitPair_Original != nullptr},
        {L"GetBestFitUnit (link speed)",
         GetBestFitUnitLinkSpeed_Original != nullptr},
        {L"TmPrintOrFormatBuffer", TmPrintOrFormatBuffer_Original != nullptr},
        {L"WdcApplicationsMonitor::GetColumnText",
         GetColumnText_Original != nullptr},
        {L"WdcStringMap::LoadStringW",
         WdcStringMap_LoadStringW_Original != nullptr},
        {L"WdcLoadString", WdcLoadString_Original != nullptr},
    };

    bool anyFound = false;
    for (const auto& result : results) {
        Wh_Log(L"%s: %s", result.name, result.found ? L"hooked" : L"NOT FOUND");
        anyFound |= result.found;
    }

    if (g_englishUnits && !WdcStringMap_LoadStringW_Original) {
        Wh_Log(L"The Performance page keeps Task Manager's unit names: "
               L"WdcStringMap::LoadStringW is not hooked");
    }

    return anyFound;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    // Strings and labels are built once per load; reload to rebuild them.
    *bReload = TRUE;
    return TRUE;
}
