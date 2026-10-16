// ==WindhawkMod==
// @id              coin-stock-monitor
// @name            Coin & Stock Monitor
// @description     Shows stock and crypto prices on the Windows 11 taskbar, with a configurable price API, symbol list, colors and entry-price percentage
// @version         1.2.1
// @author          jayll1303
// @github          https://github.com/jayll1303
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32 -lwinhttp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Coin & Stock Monitor

![Taskbar screenshot](https://raw.githubusercontent.com/jayll1303/coin-stock-monitor/main/docs/screenshot.png)

Shows live stock and crypto prices on the left side of the taskbar.

Prices come from a configurable JSON API. The default is the Yahoo Finance chart
API, which works without an API key for both stocks (`AAPL`, `VNM.VN`) and crypto
(`BTC-USD`, `ETH-USD`).

## Network access

This mod sends HTTPS requests from `explorer.exe` to the URL set in **Price API URL**.
By default that is `query1.finance.yahoo.com`. It sends one request per symbol (up to
4) at startup, then once every **Refresh interval** (default 60 seconds). Each request
identifies the mod by name in its User-Agent header. No other data is sent. To
stop all requests, disable the mod.

## Settings

* **Price API URL** - requested once per symbol. `{symbol}` is replaced with the
  URL-encoded symbol.
* **Price JSON key** - the key whose numeric value is the price in the response.
  Yahoo Finance uses `regularMarketPrice`.
* **Symbols** - separated by `|`, shown in order. At most 4 symbols, shown 2 per line.
* **Entry prices** - `SYMBOL=PRICE` pairs separated by `|`. Use `.` as the decimal
  separator. A comma is accepted only as a thousands separator, followed by exactly
  three digits, so `12,005` means `12005` and `0,45` is rejected. Lists saved with
  commas by older versions still work. When a symbol has an entry price, its price
  and signed percentage change from the entry are shown.
  The color is the gain color when the price is at or above the entry, and the
  loss color otherwise.
* **Text alignment** - left or right inside the monitor area.
* **Left offset** and **Monitor width** - position and size of the monitor area, in
  pixels from the left edge of the taskbar.
* **Text color** - `#RRGGBB` for symbols and prices without an entry price. The
  default is green. Leave empty to follow the taskbar theme.
* **Gain color** and **Loss color** - `#RRGGBB` colors for prices and percentages
  compared with entry prices.
* **Refresh interval** - seconds between fetches (minimum 15).

Changing any setting reloads the mod.

### Example: CoinGecko

* Price API URL: `https://api.coingecko.com/api/v3/simple/price?ids={symbol}&vs_currencies=usd`
* Price JSON key: `usd`
* Symbols: `bitcoin|ethereum` (CoinGecko coin ids, not ticker symbols)
* Entry prices: `bitcoin=60000` (use the same ids as Symbols)

## Limitations

* Horizontal taskbar on the primary monitor only.
* The monitor is placed by pixel offset, so it assumes the Start button is centered.
  With a left-aligned Start button, the monitor overlaps Start.
* If the taskbar has many apps or a wide Start area, reduce **Monitor width** so
  the monitor does not cover the task list.
* Pixel values are for 100% display scaling.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ApiUrl: "https://query1.finance.yahoo.com/v8/finance/chart/{symbol}?interval=1d&range=1d"
  $name: Price API URL
  $description: >-
    URL requested once per symbol. {symbol} is replaced with the URL-encoded
    symbol. The default is the Yahoo Finance chart API (stocks and crypto, no key).
- PriceKey: regularMarketPrice
  $name: Price JSON key
  $description: >-
    JSON key whose numeric value is the price in the API response. Yahoo Finance
    uses regularMarketPrice. CoinGecko uses the currency code, e.g. usd.
- Symbols: BTC-USD|ETH-USD|AAPL
  $name: Symbols
  $description: >-
    Symbols to show, separated by |, in order. At most 4 symbols, shown 2 per line.
    Yahoo examples: BTC-USD, AAPL, VNM.VN, ^GSPC. For CoinGecko, use coin ids such
    as bitcoin|ethereum.
- EntryPrices: ""
  $name: Entry prices
  $description: >-
    SYMBOL=PRICE pairs, separated by |, e.g. BTC-USD=60000|AAPL=300. Use . as the
    decimal separator. A comma is accepted only as a thousands separator, followed
    by exactly three digits, so 12,005 means 12005 and 0,45 is rejected. Lists saved
    with commas by older versions still work. Symbols with an entry price show the
    signed percentage change from it. Symbols without one show only the price.
- TextAlign: left
  $name: Text alignment
  $description: >-
    Aligns the text inside the monitor area. Left puts the text at the left edge of
    the area, and right puts it at the right edge.
  $options:
  - left: Left
  - right: Right
- TextColor: "#19FF00"
  $name: Text color
  $description: >-
    Hex color (#RRGGBB) for symbols and prices without an entry price. Leave
    empty to follow the taskbar theme.
- UpColor: "#00A850"
  $name: Gain color
  $description: >-
    Hex color (#RRGGBB) for prices and percentages at or above their entry price.
- DownColor: "#E0301E"
  $name: Loss color
  $description: >-
    Hex color (#RRGGBB) for prices and percentages below their entry price.
- RefreshSeconds: 60
  $name: Refresh interval (seconds)
  $description: >-
    How often prices are fetched. Values below 15 are raised to 15.
- LeftOffset: 8
  $name: Left offset (pixels)
  $description: >-
    Distance from the left edge of the taskbar to the monitor. Pixel values are for
    100% display scaling.
- Width: 480
  $name: Monitor width (pixels)
  $description: >-
    Width of the monitor area. Text is aligned inside it. The area grows if the text
    is wider. Keep LeftOffset + Width less than the distance to the Start button.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <winhttp.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <iterator>
#include <locale>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

using WindhawkUtils::StringSetting;

namespace {

constexpr wchar_t kOverlayClassName[] = L"WhTaskbarPriceMonitorOverlay";
constexpr wchar_t kUserAgent[] = L"CoinStockMonitor/" WH_MOD_VERSION L" (Windhawk mod)";
constexpr wchar_t kSymbolPlaceholder[] = L"{symbol}";
constexpr wchar_t kSeparator[] = L"  |  ";
constexpr wchar_t kHexDigits[] = L"0123456789ABCDEF";
constexpr wchar_t kLoadingText[] = L"Loading prices...";
constexpr UINT kMsgPricesUpdated = WM_APP + 1;
constexpr UINT_PTR kPlacementTimerId = 1;
constexpr UINT kPlacementIntervalMs = 1000;
constexpr DWORD kTaskbarPollIntervalMs = 1000;
constexpr int kMinRefreshSeconds = 15;
constexpr COLORREF kColorKey = RGB(255, 0, 255);
constexpr int kTextPaddingX = 8;
constexpr size_t kSymbolsPerLine = 2;
constexpr size_t kMaxSymbols = 4;
constexpr int kFontPointSize = 9;

struct EntryPrice {
    std::wstring symbol;
    double price;
};

struct Settings {
    std::wstring apiUrl;
    std::string priceKey;
    std::vector<std::wstring> symbols;
    std::vector<EntryPrice> entryPrices;
    int refreshSeconds = 60;
    int leftOffset = 8;
    int width = 480;
    bool alignRight = true;
    bool hasTextColor = false;
    COLORREF textColor = 0;
    COLORREF upColor = 0;
    COLORREF downColor = 0;
};

// Gain and loss tones use the configured up and down colors. Neutral uses the text color.
enum class Tone { neutral, gain, loss };

struct TextPart {
    std::wstring text;
    Tone tone;
};

using TextLine = std::vector<TextPart>;
using TextLines = std::vector<TextLine>;

Settings g_settings;
std::mutex g_textMutex;
std::vector<TextLine> g_displayLines = {{{kLoadingText, Tone::neutral}}};
std::atomic<HWND> g_overlay{nullptr};
HANDLE g_stopEvent;
HANDLE g_workerThread;
HFONT g_font;

bool StopRequested() {
    return WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
}

std::string ToUtf8(const std::wstring& text) {
    int size = WideCharToMultiByte(CP_UTF8, 0, text.data(),
                                   static_cast<int>(text.size()), nullptr, 0,
                                   nullptr, nullptr);
    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()),
                        result.data(), size, nullptr, nullptr);
    return result;
}

std::wstring Trim(const std::wstring& text) {
    size_t first = text.find_first_not_of(L" \t");
    if (first == std::wstring::npos) {
        return {};
    }
    size_t last = text.find_last_not_of(L" \t");
    return text.substr(first, last - first + 1);
}

// Splits on any of the characters in `separators`, trims the items and drops empty ones.
std::vector<std::wstring> SplitList(const std::wstring& list,
                                    const wchar_t* separators) {
    std::vector<std::wstring> items;
    size_t start = 0;
    while (start <= list.size()) {
        size_t end = list.find_first_of(separators, start);
        if (end == std::wstring::npos) {
            end = list.size();
        }
        std::wstring item = Trim(list.substr(start, end - start));
        if (!item.empty()) {
            items.push_back(item);
        }
        start = end + 1;
    }
    return items;
}

// Splits an EntryPrices list into SYMBOL=PRICE entries. Entries are separated by '|'.
// Lists saved by older versions use commas, so a comma also starts a new entry when
// the text after it is a SYMBOL=PRICE pair. Otherwise it is a thousands separator:
// "NVL.VN=12,005,AAPL=300" reads as NVL.VN=12,005 and AAPL=300.
std::vector<std::wstring> SplitEntryPrices(const std::wstring& list) {
    std::vector<std::wstring> entries;
    for (const std::wstring& item : SplitList(list, L"|")) {
        bool first = true;
        for (const std::wstring& part : SplitList(item, L",")) {
            if (!first && part.find(L'=') == std::wstring::npos) {
                entries.back() += L"," + part;
            } else {
                entries.push_back(part);
            }
            first = false;
        }
    }
    return entries;
}

// Parses a number with '.' as the decimal separator. A comma is accepted only as a
// thousands separator, followed by exactly three digits, so "12,005" reads as 12005.
// A decimal comma such as "0,45" is rejected instead of being read as 45. The classic
// locale keeps the result the same whatever the system locale is.
bool ParseDecimal(std::wstring text, double& value) {
    for (size_t comma = text.find(L','); comma != std::wstring::npos;
         comma = text.find(L',', comma + 1)) {
        size_t groupEnd = text.find_first_not_of(L"0123456789", comma + 1);
        size_t digits =
            (groupEnd == std::wstring::npos ? text.size() : groupEnd) - comma - 1;
        if (digits != 3) {
            return false;
        }
    }
    text.erase(std::remove(text.begin(), text.end(), L','), text.end());
    std::wistringstream stream(Trim(text));
    stream.imbue(std::locale::classic());
    stream >> value;
    return !stream.fail() && stream.eof();
}

std::vector<EntryPrice> ParseEntryPrices(const std::wstring& list) {
    std::vector<EntryPrice> entries;
    for (const std::wstring& item : SplitEntryPrices(list)) {
        size_t equals = item.find(L'=');
        if (equals == std::wstring::npos) {
            Wh_Log(L"Entry price without '=': %s", item.c_str());
            continue;
        }
        std::wstring symbol = Trim(item.substr(0, equals));
        double price = 0;
        if (!ParseDecimal(item.substr(equals + 1), price) || symbol.empty() ||
            price <= 0) {
            Wh_Log(L"Invalid entry price: %s", item.c_str());
            continue;
        }
        entries.push_back({symbol, price});
    }
    return entries;
}

// Parses "#RRGGBB" or "RRGGBB". Leaves `color` unchanged on failure.
bool ParseColor(const std::wstring& text, COLORREF& color) {
    std::wstring hex = Trim(text);
    if (!hex.empty() && hex[0] == L'#') {
        hex.erase(0, 1);
    }
    if (hex.size() != 6) {
        return false;
    }
    wchar_t* end = nullptr;
    unsigned long value = std::wcstoul(hex.c_str(), &end, 16);
    if (*end != L'\0') {
        return false;
    }
    color = RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
    return true;
}

std::wstring PercentEncode(const std::wstring& text) {
    std::wstring encoded;
    for (unsigned char c : ToUtf8(text)) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += static_cast<wchar_t>(c);
        } else {
            encoded += L'%';
            encoded += kHexDigits[c >> 4];
            encoded += kHexDigits[c & 0xF];
        }
    }
    return encoded;
}

void ReplaceAll(std::wstring& text,
                const std::wstring& from,
                const std::wstring& to) {
    for (size_t pos = 0; (pos = text.find(from, pos)) != std::wstring::npos;
         pos += to.size()) {
        text.replace(pos, from.size(), to);
    }
}

// Finds `"key":` in the JSON body and parses the number that follows it.
bool ParsePrice(const std::string& body,
                const std::string& key,
                double& price) {
    const std::string needle = "\"" + key + "\":";
    size_t pos = body.find(needle);
    if (pos == std::string::npos) {
        return false;
    }
    std::istringstream stream(body.substr(pos + needle.size(), 32));
    stream.imbue(std::locale::classic());
    return static_cast<bool>(stream >> price);
}

// Whole prices drop the decimals, and the integer part gets thousands separators,
// e.g. 82122650.56 -> "82,122,650.56" and 11200.00 -> "11,200".
std::wstring FormatPrice(double price) {
    char buffer[64];
    if (price >= 1.0 && std::fabs(price - std::round(price)) < 0.005) {
        snprintf(buffer, std::size(buffer), "%.0f", price);
    } else {
        snprintf(buffer, std::size(buffer), price >= 1.0 ? "%.2f" : "%.6f", price);
    }

    std::string text = buffer;
    size_t integerEnd = text.find('.');
    if (integerEnd == std::string::npos) {
        integerEnd = text.size();
    }
    for (size_t pos = integerEnd; pos > 3; pos -= 3) {
        text.insert(pos - 3, ",");
    }
    return std::wstring(text.begin(), text.end());
}

std::wstring FormatChange(double percent) {
    wchar_t buffer[32];
    _snwprintf(buffer, std::size(buffer), L"  %+.2f%%", percent);
    return buffer;
}

// The request in flight, so that Wh_ModUninit can abort it.
std::mutex g_requestMutex;
HINTERNET g_activeRequest = nullptr;

// Returns false if the mod is unloading, in which case the caller closes the request.
// Checking the stop event under the same lock as CancelActiveRequest closes the gap
// where a request could start after the cancel.
bool SetActiveRequest(HINTERNET request) {
    std::lock_guard<std::mutex> lock(g_requestMutex);
    if (StopRequested()) {
        return false;
    }
    g_activeRequest = request;
    return true;
}

// Closes the request unless CancelActiveRequest has already closed it.
void FinishActiveRequest(HINTERNET request) {
    bool owned = false;
    {
        std::lock_guard<std::mutex> lock(g_requestMutex);
        owned = request && g_activeRequest == request;
        if (owned) {
            g_activeRequest = nullptr;
        }
    }
    if (owned) {
        WinHttpCloseHandle(request);
    }
}

void CancelActiveRequest() {
    std::lock_guard<std::mutex> lock(g_requestMutex);
    if (g_activeRequest) {
        WinHttpCloseHandle(g_activeRequest);
        g_activeRequest = nullptr;
    }
}

bool HttpGet(HINTERNET session, const std::wstring& url, std::string& body) {
    URL_COMPONENTS parts{};
    parts.dwStructSize = sizeof(parts);
    parts.dwSchemeLength = parts.dwHostNameLength = parts.dwUrlPathLength =
        parts.dwExtraInfoLength = static_cast<DWORD>(-1);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts)) {
        return false;
    }

    // The path and query are contiguous in the URL, so join them back together.
    std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
    std::wstring path(parts.lpszUrlPath,
                      parts.dwUrlPathLength + parts.dwExtraInfoLength);

    HINTERNET connection =
        WinHttpConnect(session, host.c_str(), parts.nPort, 0);
    if (!connection) {
        return false;
    }

    bool secure = parts.nScheme == INTERNET_SCHEME_HTTPS;
    HINTERNET request = WinHttpOpenRequest(
        connection, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, secure ? WINHTTP_FLAG_SECURE : 0);
    if (request && !SetActiveRequest(request)) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        return false;
    }

    bool ok = false;
    if (request &&
        WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, nullptr,
                           0, 0, 0) &&
        WinHttpReceiveResponse(request, nullptr)) {
        DWORD status = 0;
        DWORD statusSize = sizeof(status);
        ok = WinHttpQueryHeaders(request,
                                 WINHTTP_QUERY_STATUS_CODE |
                                     WINHTTP_QUERY_FLAG_NUMBER,
                                 WINHTTP_HEADER_NAME_BY_INDEX, &status,
                                 &statusSize, WINHTTP_NO_HEADER_INDEX) &&
             status == 200;

        while (ok) {
            DWORD available = 0;
            if (!WinHttpQueryDataAvailable(request, &available)) {
                ok = false;
                break;
            }
            if (available == 0) {
                break;
            }
            std::string chunk(available, '\0');
            DWORD read = 0;
            if (!WinHttpReadData(request, chunk.data(), available, &read)) {
                ok = false;
                break;
            }
            body.append(chunk.data(), read);
        }
    }

    FinishActiveRequest(request);
    WinHttpCloseHandle(connection);
    return ok;
}

bool FetchPrice(HINTERNET session,
                const Settings& settings,
                const std::wstring& symbol,
                double& price) {
    std::wstring url = settings.apiUrl;
    ReplaceAll(url, kSymbolPlaceholder, PercentEncode(symbol));

    std::string body;
    if (!HttpGet(session, url, body)) {
        Wh_Log(L"Price request failed for %s", symbol.c_str());
        return false;
    }
    if (!ParsePrice(body, settings.priceKey, price)) {
        Wh_Log(L"Price key not found for %s", symbol.c_str());
        return false;
    }
    return true;
}

const EntryPrice* FindEntryPrice(const Settings& settings,
                                 const std::wstring& symbol) {
    for (const EntryPrice& entry : settings.entryPrices) {
        if (_wcsicmp(entry.symbol.c_str(), symbol.c_str()) == 0) {
            return &entry;
        }
    }
    return nullptr;
}

// Builds one line per kSymbolsPerLine symbols.
TextLines BuildMonitorLines(HINTERNET session, const Settings& settings) {
    if (settings.symbols.empty()) {
        return {{{L"No symbols configured", Tone::neutral}}};
    }

    TextLines lines;
    for (size_t i = 0; i < settings.symbols.size(); i++) {
        if (StopRequested()) {
            break;
        }
        if (i % kSymbolsPerLine == 0) {
            lines.emplace_back();
        }
        TextLine& line = lines.back();
        if (!line.empty()) {
            line.push_back({kSeparator, Tone::neutral});
        }

        const std::wstring& symbol = settings.symbols[i];
        line.push_back({symbol + L" ", Tone::neutral});

        double price = 0;
        if (!FetchPrice(session, settings, symbol, price)) {
            line.push_back({L"--", Tone::neutral});
            continue;
        }

        const EntryPrice* entry = FindEntryPrice(settings, symbol);
        Tone tone = Tone::neutral;
        if (entry) {
            tone = price >= entry->price ? Tone::gain : Tone::loss;
        }
        line.push_back({FormatPrice(price), tone});
        if (entry) {
            double change = (price - entry->price) / entry->price * 100.0;
            line.push_back({FormatChange(change), tone});
        }
    }
    return lines;
}

// Returns this process's taskbar. Secondary Explorer processes have none.
HWND FindCurrentProcessTaskbarWnd() {
    HWND taskbar = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbar));
    return taskbar;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

// Runs proc on the thread that owns hWnd, and waits until it returns. The overlay
// must be created and destroyed on the taskbar's thread, because a child window
// on another thread shares input state with its parent.
bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runMsg =
        RegisterWindowMessageW(L"CoinStockMonitor_RunFromWindowThread");

    struct RunParam {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runMsg) {
                    RunParam* param = reinterpret_cast<RunParam*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RunParam param{proc, procParam};
    SendMessageW(hWnd, runMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

TextLines GetDisplayLines() {
    std::lock_guard<std::mutex> lock(g_textMutex);
    return g_displayLines;
}

bool IsLightTaskbarTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return value != 0;
}

COLORREF ResolveColor(Tone tone) {
    switch (tone) {
        case Tone::gain:
            return g_settings.upColor;
        case Tone::loss:
            return g_settings.downColor;
        default:
            if (g_settings.hasTextColor) {
                return g_settings.textColor;
            }
            return IsLightTaskbarTheme() ? RGB(0, 0, 0) : RGB(255, 255, 255);
    }
}

SIZE MeasurePart(HDC dc, const TextPart& part) {
    SIZE size{};
    GetTextExtentPoint32W(dc, part.text.c_str(),
                          static_cast<int>(part.text.size()), &size);
    return size;
}

int MeasureLineWidth(HDC dc, const TextLine& line) {
    int width = 0;
    for (const TextPart& part : line) {
        width += MeasurePart(dc, part).cx;
    }
    return width;
}

int MeasureTextWidth(HWND overlay, const TextLines& lines) {
    HDC dc = GetDC(overlay);
    HGDIOBJ oldFont = SelectObject(dc, g_font);
    int width = 0;
    for (const TextLine& line : lines) {
        width = std::max(width, MeasureLineWidth(dc, line));
    }
    SelectObject(dc, oldFont);
    ReleaseDC(overlay, dc);
    return width;
}

// Places the monitor area at LeftOffset from the taskbar's left edge. The text is
// aligned inside the area. The area is at least Width wide, and wider if the text
// needs more room.
void RefreshOverlay(HWND overlay) {
    HWND taskbar = GetParent(overlay);
    int dpi = static_cast<int>(GetDpiForWindow(taskbar));

    RECT taskbarClient;
    GetClientRect(taskbar, &taskbarClient);

    int textWidth = MeasureTextWidth(overlay, GetDisplayLines());
    int width = std::max(MulDiv(g_settings.width, dpi, 96),
                         textWidth + 2 * kTextPaddingX);
    int left = MulDiv(g_settings.leftOffset, dpi, 96);
    SetWindowPos(overlay, HWND_TOP, left, 0, width, taskbarClient.bottom,
                 SWP_NOACTIVATE);
    InvalidateRect(overlay, nullptr, TRUE);
}

void PaintOverlay(HWND overlay) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(overlay, &ps);

    RECT rect;
    GetClientRect(overlay, &rect);
    HBRUSH keyBrush = CreateSolidBrush(kColorKey);
    FillRect(dc, &rect, keyBrush);
    DeleteObject(keyBrush);

    TextLines lines = GetDisplayLines();
    HGDIOBJ oldFont = SelectObject(dc, g_font);
    SetBkMode(dc, TRANSPARENT);

    TEXTMETRICW metrics;
    GetTextMetricsW(dc, &metrics);
    int lineHeight = metrics.tmHeight;
    int y = (rect.bottom - lineHeight * static_cast<int>(lines.size())) / 2;

    for (const TextLine& line : lines) {
        int width = MeasureLineWidth(dc, line);
        int x = g_settings.alignRight ? rect.right - kTextPaddingX - width
                                      : rect.left + kTextPaddingX;
        for (const TextPart& part : line) {
            SetTextColor(dc, ResolveColor(part.tone));
            TextOutW(dc, x, y, part.text.c_str(),
                     static_cast<int>(part.text.size()));
            x += MeasurePart(dc, part).cx;
        }
        y += lineHeight;
    }

    SelectObject(dc, oldFont);
    EndPaint(overlay, &ps);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd,
                                UINT message,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (message) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            PaintOverlay(hwnd);
            return 0;

        case WM_TIMER:
            if (wParam == kPlacementTimerId) {
                RefreshOverlay(hwnd);
            }
            return 0;

        case kMsgPricesUpdated:
            RefreshOverlay(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

HMODULE GetThisModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OverlayWndProc), &module);
    return module;
}

// Runs on the taskbar's thread.
void WINAPI CreateOverlayOnTaskbarThread(PVOID parameter) {
    HWND taskbar = static_cast<HWND>(parameter);
    HMODULE module = GetThisModule();

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = OverlayWndProc;
    windowClass.hInstance = module;
    windowClass.lpszClassName = kOverlayClassName;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
        return;
    }

    if (!g_font) {
        int dpi = static_cast<int>(GetDpiForWindow(taskbar));
        g_font = CreateFontW(-MulDiv(kFontPointSize, dpi, 72), 0, 0, 0,
                             FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             NONANTIALIASED_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    }

    // Layered + color key makes the background transparent, and WS_EX_TRANSPARENT
    // lets clicks fall through to the taskbar.
    HWND overlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, kOverlayClassName,
        nullptr, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, taskbar, nullptr, module,
        nullptr);
    if (!overlay) {
        return;
    }
    SetLayeredWindowAttributes(overlay, kColorKey, 0, LWA_COLORKEY);
    SetTimer(overlay, kPlacementTimerId, kPlacementIntervalMs, nullptr);
    g_overlay = overlay;
    RefreshOverlay(overlay);
}

// Runs on the taskbar's thread, after the worker thread has stopped.
void WINAPI DestroyOverlayOnTaskbarThread(PVOID parameter) {
    HWND overlay = static_cast<HWND>(parameter);
    KillTimer(overlay, kPlacementTimerId);
    DestroyWindow(overlay);
}

// Waits for this process's taskbar, then fetches prices until the mod unloads.
DWORD WINAPI WorkerThreadProc(LPVOID) {
    HWND taskbar = nullptr;
    while (!(taskbar = FindCurrentProcessTaskbarWnd())) {
        if (WaitForSingleObject(g_stopEvent, kTaskbarPollIntervalMs) !=
            WAIT_TIMEOUT) {
            return 0;
        }
    }
    if (!RunFromWindowThread(taskbar, CreateOverlayOnTaskbarThread, taskbar)) {
        return 0;
    }

    Settings settings = g_settings;
    HINTERNET session = WinHttpOpen(kUserAgent, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                    WINHTTP_NO_PROXY_NAME,
                                    WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) {
        return 0;
    }
    WinHttpSetTimeouts(session, 5000, 5000, 5000, 10000);

    while (!StopRequested()) {
        TextLines lines = BuildMonitorLines(session, settings);
        if (StopRequested()) {
            break;
        }
        {
            std::lock_guard<std::mutex> lock(g_textMutex);
            g_displayLines = lines;
        }
        HWND overlay = g_overlay;
        if (overlay) {
            PostMessageW(overlay, kMsgPricesUpdated, 0, 0);
        }
        if (WaitForSingleObject(g_stopEvent,
                                static_cast<DWORD>(settings.refreshSeconds) *
                                    1000) != WAIT_TIMEOUT) {
            break;
        }
    }

    WinHttpCloseHandle(session);
    return 0;
}

COLORREF ColorSetting(PCWSTR name, COLORREF fallback) {
    COLORREF color = fallback;
    std::wstring text = StringSetting::make(name).get();
    if (!ParseColor(text, color)) {
        Wh_Log(L"Invalid color for %s, using default", name);
    }
    return color;
}

void LoadSettings() {
    g_settings.apiUrl = StringSetting::make(L"ApiUrl").get();
    g_settings.priceKey = ToUtf8(StringSetting::make(L"PriceKey").get());
    // Symbols never contain commas, so lists saved by older versions still work.
    g_settings.symbols = SplitList(StringSetting::make(L"Symbols").get(), L"|,");
    if (g_settings.symbols.size() > kMaxSymbols) {
        Wh_Log(L"Only the first %u symbols are shown", static_cast<unsigned>(kMaxSymbols));
        g_settings.symbols.resize(kMaxSymbols);
    }
    g_settings.entryPrices =
        ParseEntryPrices(StringSetting::make(L"EntryPrices").get());
    g_settings.refreshSeconds =
        std::max(Wh_GetIntSetting(L"RefreshSeconds"), kMinRefreshSeconds);
    g_settings.leftOffset = std::max(Wh_GetIntSetting(L"LeftOffset"), 0);
    g_settings.width = std::max(Wh_GetIntSetting(L"Width"), 0);
    g_settings.alignRight =
        wcscmp(StringSetting::make(L"TextAlign").get(), L"left") != 0;

    std::wstring textColor = StringSetting::make(L"TextColor").get();
    g_settings.hasTextColor = ParseColor(textColor, g_settings.textColor);

    g_settings.upColor = ColorSetting(L"UpColor", RGB(0x00, 0xA8, 0x50));
    g_settings.downColor = ColorSetting(L"DownColor", RGB(0xE0, 0x30, 0x1E));
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        CloseHandle(g_stopEvent);
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    SetEvent(g_stopEvent);
    CancelActiveRequest();

    // Every WinHTTP call has a timeout, so the worker always returns. Waiting
    // without a limit keeps the mod's code mapped until the thread has left it.
    WaitForSingleObject(g_workerThread, INFINITE);
    CloseHandle(g_workerThread);

    HWND overlay = g_overlay.exchange(nullptr);
    if (overlay && IsWindow(overlay)) {
        RunFromWindowThread(GetParent(overlay), DestroyOverlayOnTaskbarThread,
                            overlay);
    }

    // Always release the class and font, even if the overlay was already gone.
    // A class left registered would point into this DLL after it unloads.
    UnregisterClassW(kOverlayClassName, GetThisModule());
    if (g_font) {
        DeleteObject(g_font);
        g_font = nullptr;
    }
    CloseHandle(g_stopEvent);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
