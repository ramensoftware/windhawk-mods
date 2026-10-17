// ==WindhawkMod==
// @id              taskmgr-network-bytes
// @name            Task Manager Network Speed in Bytes
// @description     Show network throughput in Task Manager as KB/s, MB/s, GB/s (bytes) instead of Kbps, Mbps, Gbps (bits)
// @version         1.1
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

- The mod needs the debug symbols of `taskmgr.exe`. Windhawk downloads them
  (about 20 MB) the first time Task Manager starts after the mod is installed —
  wait for the "Loading symbols" popup to reach 100%.
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

// Not real resources (string IDs are 16-bit): the WdcStringMap::LoadStringW
// hook answers these with the English unit names.
constexpr DWORD kIdEnglishKBytesPerSec = 0x10001;
constexpr DWORD kIdEnglishMBytesPerSec = 0x10002;
constexpr DWORD kIdEnglishGBytesPerSec = 0x10003;

constexpr PCWSTR kEnglishByteUnits[] = {L"B/s", L"KB/s", L"MB/s", L"GB/s"};

// WDC column type of the Processes/Users "Network" column.
constexpr BYTE kNetworkColumnType = 0x0A;

// GetColumnText raises any non-zero network value below this to this.
constexpr double kNetworkColumnMinMbps = static_cast<double>(0.1f);

// Fixed output size used by the link speed overload of GetBestFitUnit.
constexpr size_t kLinkSpeedBufferChars = 0x80;

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
constexpr ATM_INSTANT_VALUE_SCALE kLocalizedByteScale[] = {
    {999499.999, 1e-3, kIdKBytesPerSec, 0},
    {999499999.999, 1e-6, kIdMBytesPerSec, 0},
    {999999999999.0, 1e-9, kIdGBytesPerSec, 0},
};
constexpr ATM_INSTANT_VALUE_SCALE kEnglishByteScale[] = {
    {999499.999, 1e-3, kIdEnglishKBytesPerSec, 0},
    {999499999.999, 1e-6, kIdEnglishMBytesPerSec, 0},
    {999999999999.0, 1e-9, kIdEnglishGBytesPerSec, 0},
};

// Settings.
bool g_englishUnits = true;
// English units on the Performance page need the WdcStringMap::LoadStringW
// hook to answer kIdEnglish*; without it, that page keeps localized units.
bool g_useEnglishUnitIds = false;

const ATM_INSTANT_VALUE_SCALE* ByteScale() {
    return g_useEnglishUnitIds ? kEnglishByteScale : kLocalizedByteScale;
}

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
std::wstring g_valueUnitPattern = L"%1!s! %2!s!";
std::wstring g_byteUnits[4];  // B/s, KB/s, MB/s, GB/s
PCWSTR g_byteUnitText[4];
std::wstring g_networkColumnPrefix;

thread_local bool t_hasRawNetworkMbps;
thread_local double t_rawNetworkMbps;

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

void LoadStrings() {
    std::wstring pattern = LoadTmString(kIdValueUnitPattern);
    if (!pattern.empty()) {
        g_valueUnitPattern = pattern;
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
    g_networkColumnPrefix = LeadingBidiMarks(LoadTmString(kIdNetworkColumnMbps));
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

// Text for a string ID the mod answers itself (graph labels and the English
// unit IDs), or nullptr to let Task Manager load it.
PCWSTR GetReplacementText(DWORD id) {
    if (id >= kIdEnglishKBytesPerSec && id <= kIdEnglishGBytesPerSec) {
        EnsureInitialized();
        return g_byteUnitText[1 + id - kIdEnglishKBytesPerSec];
    }
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
    if (IsNetworkScale(scale)) {
        scale = ByteScale();
        value /= 8;
    }
    return GetBestFitUnit_Original(pThis, numberFormat, output, outputChars,
                                   scale, scaleCount, value, flags);
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
    if (IsNetworkScale(scale)) {
        scale = ByteScale();
        value1 /= 8;
        value2 /= 8;
    }
    return GetBestFitUnitPair_Original(pThis, numberFormat, output1, output2,
                                       outputChars, scale, scaleCount, value1,
                                       value2, formatId1, formatId2,
                                       sharedUnit);
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
// cache.
using WdcStringMap_LoadStringW_t = HRESULT(WINAPI*)(void* pThis,
                                                    DWORD id,
                                                    PWSTR* string);
WdcStringMap_LoadStringW_t WdcStringMap_LoadStringW_Original;
HRESULT WINAPI WdcStringMap_LoadStringW_Hook(void* pThis,
                                             DWORD id,
                                             PWSTR* string) {
    if (PCWSTR label = GetReplacementText(id)) {
        *string = const_cast<PWSTR>(label);
        return S_OK;
    }
    return WdcStringMap_LoadStringW_Original(pThis, id, string);
}

// Graph scale labels, uncached loads.
using WdcLoadString_t = HRESULT(WINAPI*)(DWORD id,
                                         PWSTR buffer,
                                         ULONGLONG bufferChars);
WdcLoadString_t WdcLoadString_Original;
HRESULT WINAPI WdcLoadString_Hook(DWORD id,
                                  PWSTR buffer,
                                  ULONGLONG bufferChars) {
    if (PCWSTR label = GetReplacementText(id)) {
        return StringCchCopyW(buffer, bufferChars, label);
    }
    return WdcLoadString_Original(id, buffer, bufferChars);
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
        Wh_Log(L"Failed to look up Task Manager symbols");
        return FALSE;
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

    g_useEnglishUnitIds =
        g_englishUnits && WdcStringMap_LoadStringW_Original != nullptr;
    if (g_englishUnits && !g_useEnglishUnitIds) {
        Wh_Log(L"Performance page keeps localized unit names: "
               L"WdcStringMap::LoadStringW not hooked");
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
