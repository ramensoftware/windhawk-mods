// ==WindhawkMod==
// @id              deepseek-balance-tray
// @name            DeepSeek Balance in Taskbar
// @description     Shows the current DeepSeek API account balance next to the taskbar clock
// @version         1.0.0
// @author          MrAlexFreeman
// @github          https://github.com/MrAlexFreeman
// @license         MIT
// @architecture    x86-64
// @include         explorer.exe
// @compilerOptions -lwinhttp -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DeepSeek Balance in Taskbar

Shows the current DeepSeek API account balance right in the Windows taskbar
clock:

```
0:17
22.10.2026 | ≈1 058 ₽
```

Works in `explorer.exe` on Windows 10 and Windows 11 with the stock taskbar.
Networking uses the built-in WinHTTP stack and JSON/XML parsing is hand-written,
so there are no external dependencies.

![screenshot](https://raw.githubusercontent.com/MrAlexFreeman/deepseek-balance-tray/main/screenshots/clock.png)

## How it works

* The mod hooks `GetTimeFormatEx` / `GetDateFormatEx` from `kernelbase.dll` —
  the functions the taskbar clock uses to format its text (Windows 10:
  `ClockButton::UpdateTextStringsIfNecessary`, Windows 11:
  `ClockSystemTrayIconDataModel::RefreshIcon`).
* Formatting is only decorated while the clock is refreshing (a thread-local
  flag), so dates in other Explorer windows are left alone.
* A background thread polls `https://api.deepseek.com/user/balance` through
  WinHTTP, parses the JSON and updates the clock suffix.
* When a display currency is selected, the mod fetches the official Bank of
  Russia rates (`https://www.cbr.ru/scripts/XML_daily.asp`, windows-1251),
  caches them for six hours and converts the balance. With the default
  `native` mode no such request is made.

## Settings

* **DeepSeek API key** — `sk-...` key stored in Windhawk's mod settings on this
  PC. Leave it empty to use the `DEEPSEEK_API_KEY` environment variable.
* **Refresh interval** — how often to poll the API, in minutes.
* **Clock line** — append to the bottom (date) line or the top (time) line.
* **Display currency** — `native` keeps the currency returned by the API;
  `CNY`, `RUB` and `USD` convert the balance with the official Bank of Russia
  rates.

## Notes

* Requires symbols (PDB) for `explorer.exe` / `SystemTray.dll`; Windhawk
  downloads them automatically on first start.
* The old Windows 10 taskbar on Windows 11 (ExplorerPatcher) is not supported.
* The API key is stored in plain text in the mod settings: Windhawk has no
  secret setting type. The key is only sent to `api.deepseek.com` and is never
  logged; no other endpoints are contacted except `www.cbr.ru`, and only when a
  display currency other than `native` is selected.
  In a regular (non-portable) Windhawk install the settings are machine-wide,
  so every Windows account on this PC shares the same key.
* Do not enable this mod together with Taskbar Clock Customization: both hook
  the same formatting functions of the clock, and that mod replaces the line
  text, so only one of them can win. Pick one of the two.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- apiKey: ""
  $name: DeepSeek API key
  $description: >-
    DeepSeek API key (starts with "sk-"). It is stored in Windhawk's mod
    settings on this PC in plain text. Leave this field empty to read the key
    from the DEEPSEEK_API_KEY environment variable instead.
- refreshInterval: 5
  $name: Refresh interval (minutes)
  $description: >-
    How often to poll the API. Values below 1 are treated as 1, values above
    1440 as 1440 (one day).
- clockLine: date
  $name: Clock line
  $description: >-
    Which clock line to append the balance to: the small date line (looks
    tidier) or the large time line.
  $options:
  - date: Date line (bottom)
  - time: Time line (top)
- displayCurrency: native
  $name: Display currency
  $description: >-
    Currency to show the balance in. native keeps the currency returned by the
    API and makes no additional network requests; CNY, RUB and USD convert the
    balance using the official Bank of Russia rates (updated daily, cached for
    6 hours). If a rate is not available, the balance is shown in its original
    currency.
  $options:
  - native: Native (as returned by the API)
  - CNY: Chinese yuan (¥)
  - RUB: Russian rubles (₽)
  - USD: US dollars ($)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winhttp.h>

#include <atomic>
#include <cstring>
#include <cwctype>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <windhawk_utils.h>

namespace {

// ---------------------------------------------------------------------------
// 1. Настройки мода и состояние отображения
// ---------------------------------------------------------------------------

// Все поля ниже защищены одним мьютексом g_stateMutex. Один мьютекс выбран
// намеренно: так невозможно ошибиться с порядком захвата (settings vs state),
// а критическая секция всегда короткая — сетевой ввод-вывод выполняется
// строго вне блокировки.
std::mutex g_stateMutex;

struct Settings {
    std::wstring apiKey;            // ключ DeepSeek без префикса "Bearer "
    std::wstring clockLine = L"date"; // "date" (нижняя строка) или "time"
    std::wstring displayCurrency = L"native"; // "native", "CNY", "RUB" или "USD"
    int refreshMinutes = 5;         // период опроса API
};

// Курс ЦБ РФ для одной валюты: сколько рублей стоит одна её единица.
struct RubRate {
    std::wstring charCode;   // "CNY", "USD", ...
    double rubPerUnit = 0.0; // рублей за 1 единицу валюты
};

Settings g_settings;

std::wstring g_amount;              // "12.34" — сумма без префикса
double g_amountValue = 0.0;         // она же числом
std::wstring g_currency;            // валюта суммы: "CNY", "USD", ...
bool g_hasAmount = false;           // есть ли актуальное значение
std::wstring g_clockSuffix;         // " | $12.34" — то, что дописывается к часам

// Кэш курсов ЦБ РФ. Обновляется не чаще, чем раз в kRatesTtlMs.
std::vector<RubRate> g_rubRates;
ULONGLONG g_rubRatesFetchedAt = 0;
double g_rubPerUnit = 0.0;          // курс текущей валюты (0 — неизвестен)

// Фоновый поток, событие его пробуждения и флаг остановки.
//
// [[clang::no_destroy]] обязателен: Windhawk не вызывает Wh_ModUninit, когда
// завершается сам Explorer (перезапуск, выход из системы, выключение). Если
// std::thread к этому моменту всё ещё joinable, его деструктор вызывает
// std::terminate() и роняет Explorer. Поэтому объект вообще не должен
// разрушаться при завершении процесса.
[[clang::no_destroy]] std::optional<std::thread> g_workerThread;
HANDLE g_wakeEvent = nullptr;       // guarded by g_stateMutex
std::atomic<bool> g_stopRequested{false};

// Состояние подключения часов Windows 11 (SystemTray.dll и т. п.).
std::atomic<bool> g_systemTrayHooked{false};

// ---------------------------------------------------------------------------
// 2. Вспомогательные функции работы со строками
// ---------------------------------------------------------------------------

void TrimInPlace(std::wstring& text) {
    size_t begin = 0;
    size_t end = text.size();

    while (begin < end && iswspace(text[begin])) {
        ++begin;
    }

    while (end > begin && iswspace(text[end - 1])) {
        --end;
    }

    text.erase(end);
    text.erase(0, begin);
}

// Разбирает десятичное число, не завися от локали процесса: принимает и
// точку, и запятую (ЦБ РФ отдаёт "12,2806"). Экспоненту не поддерживаем —
// в ответах API и ЦБ её нет.
bool ParseDecimal(const std::wstring& rawValue, double& out) {
    std::wstring raw = rawValue;
    TrimInPlace(raw);

    size_t pos = 0;
    bool negative = false;

    if (pos < raw.size() && (raw[pos] == L'-' || raw[pos] == L'+')) {
        negative = raw[pos] == L'-';
        ++pos;
    }

    double value = 0.0;
    bool hasDigits = false;

    while (pos < raw.size() && iswdigit(raw[pos])) {
        value = value * 10.0 + (raw[pos] - L'0');
        hasDigits = true;
        ++pos;
    }

    if (pos < raw.size() && (raw[pos] == L'.' || raw[pos] == L',')) {
        ++pos;

        double scale = 1.0;

        while (pos < raw.size() && iswdigit(raw[pos])) {
            value = value * 10.0 + (raw[pos] - L'0');
            scale *= 10.0;
            hasDigits = true;
            ++pos;
        }

        value /= scale;
    }

    if (!hasDigits) {
        return false;
    }

    out = negative ? -value : value;
    return true;
}

// Разбивает целое число на разряды пробелами: 1234567 -> "1 234 567".
std::wstring GroupDigits(long long value) {
    bool negative = value < 0;

    if (negative) {
        value = -value;
    }

    std::wstring digits = std::to_wstring(value);
    std::wstring grouped;
    grouped.reserve(digits.size() + digits.size() / 3 + 1);

    int count = 0;

    for (size_t i = digits.size(); i-- > 0;) {
        grouped.insert(grouped.begin(), digits[i]);

        if (++count % 3 == 0 && i > 0) {
            grouped.insert(grouped.begin(), L' ');
        }
    }

    if (negative) {
        grouped.insert(grouped.begin(), L'-');
    }

    return grouped;
}

// Форматирует сумму в выбранной валюте: рубли — целыми, юани и доллары — с
// двумя знаками после точки.
// Знак "≈" показывает, что сумма пересчитана по курсу и может отличаться от
// точного списания в исходной валюте.
std::wstring FormatAmount(double value, const std::wstring& code,
                          bool approximate) {
    if (value < 0.0) {
        value = 0.0;
    }

    const std::wstring prefix = approximate ? L"≈" : L"";

    if (_wcsicmp(code.c_str(), L"RUB") == 0) {
        return prefix + GroupDigits(static_cast<long long>(value + 0.5)) + L" ₽";
    }

    const long long cents = static_cast<long long>(value * 100.0 + 0.5);
    std::wstring number = GroupDigits(cents / 100);
    number += L'.';
    number += static_cast<wchar_t>(L'0' + (cents % 100) / 10);
    number += static_cast<wchar_t>(L'0' + cents % 10);

    if (_wcsicmp(code.c_str(), L"CNY") == 0) {
        return prefix + L"¥" + number;
    }
    if (_wcsicmp(code.c_str(), L"USD") == 0) {
        return prefix + L"$" + number;
    }

    // Неизвестная валюта — показываем её код после суммы.
    return prefix + number + L" " + code;
}

// Обрезает длинный текст для Wh_Log: в лог не должны попадать переводы строк
// из JSON и километровые простыни.
std::wstring SnippetForLog(const std::wstring& text, size_t maxChars = 300) {
    std::wstring snippet = text.substr(0, maxChars);

    for (wchar_t& c : snippet) {
        if (c == L'\r' || c == L'\n' || c == L'\t') {
            c = L' ';
        }
    }

    if (text.size() > maxChars) {
        snippet += L"...";
    }

    return snippet;
}

std::wstring GetStringSetting(PCWSTR name) {
    std::wstring result;

    // Wh_GetStringSetting возвращает буфер, который надо освободить
    // Wh_FreeStringSetting. Возвращённый указатель действителен только до
    // следующего вызова, поэтому сразу копируем строку.
    if (PCWSTR value = Wh_GetStringSetting(name)) {
        result = value;
        Wh_FreeStringSetting(value);
    }

    return result;
}

std::wstring ReadApiKeyFromEnvironment() {
    DWORD size = GetEnvironmentVariableW(L"DEEPSEEK_API_KEY", nullptr, 0);
    if (size == 0) {
        return {};
    }

    std::wstring value(size, L'\0');
    DWORD written = GetEnvironmentVariableW(L"DEEPSEEK_API_KEY", value.data(), size);
    if (written == 0 || written >= size) {
        return {};
    }

    value.resize(written);
    TrimInPlace(value);
    return value;
}

// ---------------------------------------------------------------------------
// 3. Настройки: чтение и пересборка строки, которую видит пользователь
// ---------------------------------------------------------------------------

// Вызывается с захваченным g_stateMutex.
void LoadSettingsLocked() {
    Settings settings;

    settings.apiKey = GetStringSetting(L"apiKey");
    TrimInPlace(settings.apiKey);

    // Пользователь может скопировать заголовок целиком — это не ошибка,
    // просто отрезаем схему авторизации, если она есть.
    if (settings.apiKey.rfind(L"Bearer ", 0) == 0) {
        settings.apiKey.erase(0, 7);
        TrimInPlace(settings.apiKey);
    }

    settings.clockLine = GetStringSetting(L"clockLine");
    TrimInPlace(settings.clockLine);
    if (settings.clockLine.empty()) {
        settings.clockLine = L"date";
    }

    settings.displayCurrency = GetStringSetting(L"displayCurrency");
    TrimInPlace(settings.displayCurrency);
    if (settings.displayCurrency.empty()) {
        settings.displayCurrency = L"native";
    }

    int minutes = Wh_GetIntSetting(L"refreshInterval");
    if (minutes < 1) {
        minutes = 1;
    }
    if (minutes > 24 * 60) {
        minutes = 24 * 60;
    }
    settings.refreshMinutes = minutes;

    g_settings = std::move(settings);
}

// Определён в разделе курсов ЦБ ниже; объявлен заранее, потому что строка
// часов пересобирается раньше.
double LookupRateLocked(PCWSTR code);

// Вызывается с захваченным g_stateMutex.
void RebuildSuffixLocked() {
    if (!g_hasAmount || g_amount.empty()) {
        g_clockSuffix.clear();
        return;
    }

    std::wstring target = g_settings.displayCurrency;
    if (_wcsicmp(target.c_str(), L"native") == 0) {
        // native — показываем в валюте ответа API, без пересчёта и без запроса
        // курсов ЦБ.
        target = g_currency;
    }

    double value = g_amountValue;
    bool converted = false;

    // Пересчёт идёт через рубли: amount * rate(source) / rate(target).
    if (!g_currency.empty() &&
        _wcsicmp(g_currency.c_str(), target.c_str()) != 0) {
        const double sourceRate = g_rubPerUnit;
        // Рубль — базовая валюта курсов ЦБ, его курс всегда 1.
        const double targetRate =
            _wcsicmp(target.c_str(), L"RUB") == 0
                ? 1.0
                : LookupRateLocked(target.c_str());

        if (sourceRate > 0.0 && targetRate > 0.0) {
            value = value * sourceRate / targetRate;
            converted = true;
        } else {
            // Курса нет — показываем исходную валюту, а не пустую строку.
            target = g_currency;
        }
    }

    // Формат: " | ¥86.37". Разделитель с пробелами отделяет баланс от времени.
    g_clockSuffix.assign(L" | ");
    g_clockSuffix += FormatAmount(value, target, converted);
}

// ---------------------------------------------------------------------------
// 4. Разбор JSON без сторонних библиотек
// ---------------------------------------------------------------------------

// Ответ API (https://api-docs.deepseek.com/api/get-user-balance):
// {
//   "is_available": true,
//   "balance_infos": [
//     { "currency": "CNY", "total_balance": "110.00",
//       "granted_balance": "10.00", "topped_up_balance": "100.00" }
//   ]
// }
//
// Парсер намеренно минимальный: ищем ключ, проверяем двоеточие и читаем
// строковый литерал. Полноценный JSON-парсер здесь не нужен, а ручной разбор
// компилируется любым GCC/Clang без зависимостей.

// Находит позицию значения для "key" в диапазоне [from, to).
bool FindJsonValuePos(const std::wstring& json,
                      size_t from,
                      size_t to,
                      PCWSTR key,
                      size_t& valuePos) {
    std::wstring quotedKey = L"\"";
    quotedKey += key;
    quotedKey += L'"';

    size_t keyPos = json.find(quotedKey, from);
    if (keyPos == std::wstring::npos || keyPos >= to) {
        return false;
    }

    size_t pos = keyPos + quotedKey.size();

    while (pos < to && iswspace(json[pos])) {
        ++pos;
    }

    if (pos >= to || json[pos] != L':') {
        return false;
    }

    ++pos;

    while (pos < to && iswspace(json[pos])) {
        ++pos;
    }

    valuePos = pos;
    return true;
}

// Читает строковый литерал JSON, начиная с кавычки.
bool ReadJsonString(const std::wstring& json, size_t pos, std::wstring& out) {
    if (pos >= json.size() || json[pos] != L'"') {
        return false;
    }

    ++pos;
    out.clear();

    while (pos < json.size()) {
        wchar_t c = json[pos];

        if (c == L'"') {
            return true;
        }

        if (c == L'\\' && pos + 1 < json.size()) {
            // Экранирование: суммы и валюты его не содержат, но обработать
            // "\\" и "\"" нужно, чтобы не оборвать строку раньше времени.
            ++pos;
            out += json[pos];
            ++pos;
            continue;
        }

        out += c;
        ++pos;
    }

    return false;
}

// Приводит "110.00", "9.9", "0" к виду с ровно двумя знаками после точки.
// Возвращает false, если строка не является числом.
bool NormalizeAmount(const std::wstring& rawValue, std::wstring& out) {
    std::wstring raw = rawValue;
    TrimInPlace(raw);

    size_t pos = 0;
    bool negative = false;

    if (pos < raw.size() && (raw[pos] == L'-' || raw[pos] == L'+')) {
        negative = raw[pos] == L'-';
        ++pos;
    }

    std::wstring integerPart;
    std::wstring fractionPart;

    while (pos < raw.size() && iswdigit(raw[pos])) {
        integerPart += raw[pos];
        ++pos;
    }

    if (pos < raw.size() && raw[pos] == L'.') {
        ++pos;

        while (pos < raw.size() && iswdigit(raw[pos])) {
            fractionPart += raw[pos];
            ++pos;
        }
    }

    if (integerPart.empty() && fractionPart.empty()) {
        return false;
    }

    // Убираем ведущие нули: "007" -> "7", "000" -> "0".
    size_t firstNonZero = integerPart.find_first_not_of(L'0');
    if (firstNonZero == std::wstring::npos) {
        integerPart = L"0";
    } else {
        integerPart.erase(0, firstNonZero);
    }

    // Ровно два знака после точки. Более длинную дробь отбрасываем
    // (баланс в копейках/центах, округление вверх тут не нужно).
    fractionPart.resize(2, L'0');

    out.clear();

    if (negative && (integerPart != L"0" || fractionPart != L"00")) {
        out += L'-';
    }

    out += integerPart;
    out += L'.';
    out += fractionPart;

    return true;
}

// Извлекает total_balance из массива balance_infos.
//
// В ответе может быть несколько валют (например, USD с нулём и CNY с реальным
// балансом), поэтому сначала собираем все записи, а потом выбираем нужную:
//   - "auto" — первая с ненулевым балансом (иначе первая в списке);
//   - "CNY"/"USD" — запись с этой валютой, если её нет — как "auto".

struct BalanceEntry {
    std::wstring currency;
    std::wstring amount;
};

bool ParseBalanceEntries(const std::wstring& json,
                         std::vector<BalanceEntry>& entries) {
    entries.clear();

    size_t infosPos = json.find(L"\"balance_infos\"");
    if (infosPos == std::wstring::npos) {
        return false;
    }

    size_t arrayStart = json.find(L'[', infosPos);
    if (arrayStart == std::wstring::npos) {
        return false;
    }

    size_t arrayEnd = json.find(L']', arrayStart);
    if (arrayEnd == std::wstring::npos) {
        arrayEnd = json.size();
    }

    size_t pos = arrayStart;

    for (;;) {
        size_t objectStart = json.find(L'{', pos);
        if (objectStart == std::wstring::npos || objectStart >= arrayEnd) {
            break;
        }

        size_t objectEnd = json.find(L'}', objectStart);
        if (objectEnd == std::wstring::npos || objectEnd > arrayEnd) {
            objectEnd = arrayEnd;
        }

        BalanceEntry entry;
        size_t valuePos = 0;

        if (FindJsonValuePos(json, objectStart, objectEnd, L"currency", valuePos)) {
            ReadJsonString(json, valuePos, entry.currency);
        }

        if (FindJsonValuePos(json, objectStart, objectEnd, L"total_balance",
                             valuePos)) {
            std::wstring rawAmount;
            if (ReadJsonString(json, valuePos, rawAmount) &&
                NormalizeAmount(rawAmount, entry.amount)) {
                entries.push_back(std::move(entry));
            }
        }

        pos = objectEnd + 1;
    }

    return !entries.empty();
}

const BalanceEntry* SelectBalanceEntry(const std::vector<BalanceEntry>& entries,
                                       const std::wstring& preferredCurrency) {
    if (entries.empty()) {
        return nullptr;
    }

    if (!preferredCurrency.empty() &&
        _wcsicmp(preferredCurrency.c_str(), L"auto") != 0) {
        for (const auto& entry : entries) {
            if (_wcsicmp(entry.currency.c_str(), preferredCurrency.c_str()) == 0) {
                return &entry;
            }
        }
        // Выбранной валюты нет в ответе — не показываем ничего наугад,
        // а откатываемся к авто-выбору.
    }

    for (const auto& entry : entries) {
        if (entry.amount != L"0.00") {
            return &entry;
        }
    }

    return &entries.front();
}

bool ParseBalanceResponse(const std::wstring& json,
                          const std::wstring& preferredCurrency,
                          std::wstring& amount,
                          std::wstring& currency,
                          bool& isAvailable) {
    std::vector<BalanceEntry> entries;
    if (!ParseBalanceEntries(json, entries)) {
        return false;
    }

    const BalanceEntry* selected = SelectBalanceEntry(entries, preferredCurrency);
    if (!selected) {
        return false;
    }

    amount = selected->amount;
    currency = selected->currency;

    // is_available нужен только для лога и диагностики.
    isAvailable = true;
    size_t valuePos = 0;
    if (FindJsonValuePos(json, 0, json.size(), L"is_available", valuePos)) {
        isAvailable = json.compare(valuePos, 4, L"true") == 0;
    }

    return true;
}

// ---------------------------------------------------------------------------
// 5. Сетевой слой: WinHTTP
// ---------------------------------------------------------------------------

constexpr wchar_t kUserAgent[] = L"Windhawk-DeepSeekBalance/1.0";
constexpr DWORD kMaxResponseBytes = 256 * 1024;
constexpr DWORD kReceiveTimeoutMs = 8000;

// RAII-обёртка над HINTERNET: закрывает дескриптор на любом пути выхода,
// включая ранние return и исключения (bad_alloc).
struct HttpHandle {
    HINTERNET handle = nullptr;

    HttpHandle() = default;
    explicit HttpHandle(HINTERNET h) : handle(h) {}

    ~HttpHandle() {
        Close();
    }

    HttpHandle(const HttpHandle&) = delete;
    HttpHandle& operator=(const HttpHandle&) = delete;

    HttpHandle(HttpHandle&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }

    HttpHandle& operator=(HttpHandle&& other) noexcept {
        if (this != &other) {
            Close();
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }

    void Close() {
        if (handle) {
            WinHttpCloseHandle(handle);
            handle = nullptr;
        }
    }

    bool valid() const {
        return handle != nullptr;
    }

    operator HINTERNET() const {
        return handle;
    }
};

std::wstring Utf8ToWide(const std::string& text) {
    if (text.empty()) {
        return {};
    }

    int length = MultiByteToWideChar(CP_UTF8, 0, text.data(),
                                     static_cast<int>(text.size()), nullptr, 0);
    if (length <= 0) {
        return {};
    }

    std::wstring wide(length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()),
                        wide.data(), length);
    return wide;
}

// Читает тело ответа кусками. WinHttpQueryDataAvailable сообщает, сколько
// байт уже пришло, WinHttpReadData отдаёт их порциями.
bool ReadResponseBody(HINTERNET request, std::string& data) {
    data.clear();

    for (;;) {
        DWORD available = 0;
        if (!WinHttpQueryDataAvailable(request, &available)) {
            Wh_Log(L"WinHttpQueryDataAvailable failed: %u", GetLastError());
            return false;
        }

        if (available == 0) {
            return true;
        }

        if (data.size() + available > kMaxResponseBytes) {
            Wh_Log(L"Response is too large, aborting");
            return false;
        }

        size_t oldSize = data.size();
        data.resize(oldSize + available);

        DWORD read = 0;
        if (!WinHttpReadData(request, data.data() + oldSize, available, &read)) {
            Wh_Log(L"WinHttpReadData failed: %u", GetLastError());
            data.resize(oldSize);
            return false;
        }

        data.resize(oldSize + read);

        if (read == 0) {
            return true;
        }
    }
}

// GET https://api.deepseek.com/user/balance
// Заголовки: Authorization: Bearer <key>, Accept: application/json.
// Возвращает true только для HTTP 200 с непустым телом.
bool QueryBalance(const std::wstring& apiKey,
                  std::wstring& responseBody,
                  DWORD& statusCode) {
    responseBody.clear();
    statusCode = 0;

    // Автоматический прокси (PAC/WPAD) — как у остальных приложений,
    // использующих системные настройки. Если он не поднялся, откатываемся
    // на прокси по умолчанию.
    HttpHandle session(WinHttpOpen(kUserAgent, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                   WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
    if (!session.valid()) {
        session = HttpHandle(WinHttpOpen(kUserAgent,
                                         WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                         WINHTTP_NO_PROXY_NAME,
                                         WINHTTP_NO_PROXY_BYPASS, 0));
    }

    if (!session.valid()) {
        Wh_Log(L"WinHttpOpen failed: %u", GetLastError());
        return false;
    }

    // Таймауты: разрешение имени, соединение, отправка, приём. Без них
    // WinHTTP может висеть на «мёртвой» сети десятки секунд, а мод должен
    // корректно выгружаться.
    WinHttpSetTimeouts(session, 4000, 4000, 8000, kReceiveTimeoutMs);

    HttpHandle connection(WinHttpConnect(session, L"api.deepseek.com",
                                         INTERNET_DEFAULT_HTTPS_PORT, 0));
    if (!connection.valid()) {
        Wh_Log(L"WinHttpConnect failed: %u", GetLastError());
        return false;
    }

    // WINHTTP_FLAG_SECURE — HTTPS; сертификат проверяется системным хранилищем,
    // ничего отключать не нужно (иначе мод стал бы уязвим к MITM).
    HttpHandle request(WinHttpOpenRequest(connection, L"GET", L"/user/balance",
                                          nullptr, WINHTTP_NO_REFERER,
                                          WINHTTP_DEFAULT_ACCEPT_TYPES,
                                          WINHTTP_FLAG_SECURE));
    if (!request.valid()) {
        Wh_Log(L"WinHttpOpenRequest failed: %u", GetLastError());
        return false;
    }

    std::wstring headers = L"Authorization: Bearer ";
    headers += apiKey;
    headers += L"\r\nAccept: application/json\r\n";

    // dwHeadersLength = -1: строка завершается нулём, длина вычисляется
    // WinHTTP самостоятельно.
    BOOL sent = WinHttpSendRequest(request, headers.c_str(),
                                   static_cast<DWORD>(-1), WINHTTP_NO_REQUEST_DATA,
                                   0, 0, 0);
    if (!sent) {
        Wh_Log(L"WinHttpSendRequest failed: %u", GetLastError());
        return false;
    }

    if (!WinHttpReceiveResponse(request, nullptr)) {
        Wh_Log(L"WinHttpReceiveResponse failed: %u", GetLastError());
        return false;
    }

    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    if (WinHttpQueryHeaders(request,
                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                            WINHTTP_NO_HEADER_INDEX)) {
        statusCode = status;
    }

    std::string rawBody;
    if (!ReadResponseBody(request, rawBody)) {
        return false;
    }

    responseBody = Utf8ToWide(rawBody);

    if (statusCode != 200) {
        // 401 — неверный ключ, 402 — недостаточно средств, 429 — лимит запросов.
        // Тело ответа содержит пояснение, но не содержит ключ.
        Wh_Log(L"HTTP %u: %s", statusCode, SnippetForLog(responseBody).c_str());
        return false;
    }

    if (responseBody.empty()) {
        Wh_Log(L"Empty response body");
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// 6. Курсы валют ЦБ РФ
// ---------------------------------------------------------------------------
//
// Официальный XML: https://www.cbr.ru/scripts/XML_daily.asp
// Кодировка windows-1251, десятичный разделитель — запятая, для некоторых
// валют Nominal > 1 (например, 10 JPY), поэтому курс за единицу считается
// как Value / Nominal. Курс ЦБ меняется раз в сутки, поэтому кэшируем его на
// несколько часов и не дёргаем сайт на каждом обновлении баланса.

constexpr ULONGLONG kRatesTtlMs = 6ull * 60ull * 60ull * 1000ull;

std::wstring Cp1251ToWide(const std::string& text) {
    if (text.empty()) {
        return {};
    }

    int length = MultiByteToWideChar(1251, 0, text.data(),
                                     static_cast<int>(text.size()), nullptr, 0);
    if (length <= 0) {
        return {};
    }

    std::wstring wide(length, L'\0');
    MultiByteToWideChar(1251, 0, text.data(), static_cast<int>(text.size()),
                        wide.data(), length);
    return wide;
}

// Достаёт текст простого элемента <tag>...</tag> в диапазоне [from, to).
bool GetXmlText(const std::wstring& xml,
                size_t from,
                size_t to,
                PCWSTR tag,
                std::wstring& out) {
    std::wstring openTag = L"<";
    openTag += tag;
    openTag += L">";

    std::wstring closeTag = L"</";
    closeTag += tag;
    closeTag += L">";

    size_t start = xml.find(openTag, from);
    if (start == std::wstring::npos || start >= to) {
        return false;
    }

    start += openTag.size();

    size_t end = xml.find(closeTag, start);
    if (end == std::wstring::npos || end > to) {
        return false;
    }

    out = xml.substr(start, end - start);
    return true;
}

bool ParseCbrRates(const std::wstring& xml, std::vector<RubRate>& rates) {
    rates.clear();

    size_t pos = 0;

    for (;;) {
        size_t start = xml.find(L"<Valute", pos);
        if (start == std::wstring::npos) {
            break;
        }

        size_t end = xml.find(L"</Valute>", start);
        if (end == std::wstring::npos) {
            break;
        }

        std::wstring charCode;
        std::wstring nominalText;
        std::wstring valueText;

        if (GetXmlText(xml, start, end, L"CharCode", charCode) &&
            GetXmlText(xml, start, end, L"Nominal", nominalText) &&
            GetXmlText(xml, start, end, L"Value", valueText)) {
            double nominal = 0.0;
            double value = 0.0;

            if (ParseDecimal(nominalText, nominal) && nominal > 0.0 &&
                ParseDecimal(valueText, value)) {
                RubRate rate;
                rate.charCode = std::move(charCode);
                rate.rubPerUnit = value / nominal;
                rates.push_back(std::move(rate));
            }
        }

        pos = end + 9;  // длина "</Valute>"
    }

    return !rates.empty();
}

bool FetchCbrRates(std::vector<RubRate>& rates) {
    rates.clear();

    HttpHandle session(WinHttpOpen(kUserAgent, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                   WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
    if (!session.valid()) {
        session = HttpHandle(WinHttpOpen(kUserAgent,
                                         WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                         WINHTTP_NO_PROXY_NAME,
                                         WINHTTP_NO_PROXY_BYPASS, 0));
    }

    if (!session.valid()) {
        Wh_Log(L"WinHttpOpen (CBR) failed: %u", GetLastError());
        return false;
    }

    WinHttpSetTimeouts(session, 4000, 4000, 8000, 8000);

    HttpHandle connection(WinHttpConnect(session, L"www.cbr.ru",
                                         INTERNET_DEFAULT_HTTPS_PORT, 0));
    if (!connection.valid()) {
        Wh_Log(L"WinHttpConnect (CBR) failed: %u", GetLastError());
        return false;
    }

    HttpHandle request(WinHttpOpenRequest(connection, L"GET", L"/scripts/XML_daily.asp",
                                          nullptr, WINHTTP_NO_REFERER,
                                          WINHTTP_DEFAULT_ACCEPT_TYPES,
                                          WINHTTP_FLAG_SECURE));
    if (!request.valid()) {
        Wh_Log(L"WinHttpOpenRequest (CBR) failed: %u", GetLastError());
        return false;
    }

    const wchar_t* headers = L"Accept: application/xml\r\n";

    if (!WinHttpSendRequest(request, headers, static_cast<DWORD>(-1),
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(request, nullptr)) {
        Wh_Log(L"CBR request failed: %u", GetLastError());
        return false;
    }

    DWORD status = 0;
    DWORD statusSize = sizeof(status);
    WinHttpQueryHeaders(request,
                        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                        WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                        WINHTTP_NO_HEADER_INDEX);

    if (status != 200) {
        Wh_Log(L"CBR HTTP %u", status);
        return false;
    }

    std::string rawBody;
    if (!ReadResponseBody(request, rawBody)) {
        return false;
    }

    // Ответ приходит в windows-1251.
    const std::wstring xml = Cp1251ToWide(rawBody);
    if (xml.empty()) {
        Wh_Log(L"CBR: cannot decode response");
        return false;
    }

    return ParseCbrRates(xml, rates);
}

// Курс валюты из кэша; 0 — если курса нет. Вызывается с g_stateMutex.
double LookupRateLocked(PCWSTR code) {
    for (const auto& rate : g_rubRates) {
        if (_wcsicmp(rate.charCode.c_str(), code) == 0) {
            return rate.rubPerUnit;
        }
    }

    return 0.0;
}

// Обновляет кэш курсов, если он пуст или устарел. Сетевой запрос выполняется
// без удержания блокировки: хук часов не должен ждать ответ ЦБ.
void EnsureRubRates() {
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);

        if (!g_rubRates.empty() &&
            GetTickCount64() - g_rubRatesFetchedAt < kRatesTtlMs) {
            return;
        }
    }

    std::vector<RubRate> rates;

    if (!FetchCbrRates(rates)) {
        Wh_Log(L"Cannot fetch CBR rates, staying on the native currency");
        return;
    }

    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_rubRates = std::move(rates);
    g_rubRatesFetchedAt = GetTickCount64();

    Wh_Log(L"Got %zu CBR rates", g_rubRates.size());
}

// ---------------------------------------------------------------------------
// 7. Фоновый поток обновления баланса
// ---------------------------------------------------------------------------

void FetchAndUpdate() {
    Settings settings;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        settings = g_settings;
    }

    // Если ключ не задан в настройках — пробуем переменную окружения.
    // Это удобно, когда не хочется хранить ключ в реестре.
    std::wstring apiKey = settings.apiKey;
    if (apiKey.empty()) {
        apiKey = ReadApiKeyFromEnvironment();
    }

    if (apiKey.empty()) {
        Wh_Log(L"No API key configured");

        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_hasAmount = false;
        g_amount.clear();
        RebuildSuffixLocked();
        return;
    }

    std::wstring body;
    DWORD statusCode = 0;
    if (!QueryBalance(apiKey, body, statusCode)) {
        // Сетевые ошибки и 5xx не должны стирать последний известный баланс:
        // иначе строка в трее будет мигать при каждом сбое.
        Wh_Log(L"Balance request failed (HTTP %u), keeping the previous value",
               statusCode);
        return;
    }

    std::wstring amount;
    std::wstring currency;
    bool isAvailable = true;
    if (!ParseBalanceResponse(body, L"auto", amount, currency, isAvailable)) {
        Wh_Log(L"Unexpected API response: %s", SnippetForLog(body).c_str());
        return;
    }

    double amountValue = 0.0;
    if (!ParseDecimal(amount, amountValue)) {
        Wh_Log(L"Cannot parse the amount: %s", amount.c_str());
        return;
    }

    Wh_Log(L"Balance: %s %s (is_available=%d)", amount.c_str(), currency.c_str(),
           isAvailable ? 1 : 0);

    bool needRates = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_currency = currency;

        // Курсы ЦБ нужны, только если пользователь явно выбрал валюту,
        // отличную от валюты ответа. В режиме native второй запрос не нужен.
        const wchar_t* display = g_settings.displayCurrency.c_str();
        needRates = _wcsicmp(display, L"native") != 0 &&
                    _wcsicmp(display, currency.c_str()) != 0;
    }

    // Курс нужен до пересборки строки, но сеть — строго без блокировки.
    // Проверяем флаг остановки между двумя запросами: иначе выгрузка мода
    // ждала бы ещё и запрос к ЦБ.
    if (needRates && !g_stopRequested.load(std::memory_order_acquire)) {
        EnsureRubRates();
    }

    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_amount = amount;
    g_amountValue = amountValue;
    g_hasAmount = true;
    g_rubPerUnit = LookupRateLocked(g_currency.c_str());
    RebuildSuffixLocked();

    Wh_Log(L"Display: %s", g_clockSuffix.c_str());
}

void WorkerProc() {
    for (;;) {
        if (g_stopRequested.load(std::memory_order_acquire)) {
            break;
        }

        FetchAndUpdate();

        int minutes = 5;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            minutes = g_settings.refreshMinutes;
        }

        // Ожидание с возможностью проснуться раньше: смену настроек
        // Wh_ModSettingsChanged сигналит событием, и поток сразу идёт
        // за новым балансом (например, после смены ключа).
        DWORD timeoutMs = static_cast<DWORD>(minutes) * 60u * 1000u;
        DWORD waitResult = WaitForSingleObject(g_wakeEvent, timeoutMs);

        if (g_stopRequested.load(std::memory_order_acquire)) {
            break;
        }

        if (waitResult == WAIT_FAILED) {
            // Не крутимся в busy-loop, если событие внезапно стало
            // невалидным.
            Sleep(1000);
        }
    }
}

bool StartWorker() {
    std::lock_guard<std::mutex> lock(g_stateMutex);

    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_wakeEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return false;
    }

    g_stopRequested.store(false, std::memory_order_release);

    // std::thread здесь безопасен: Wh_ModInit вызывается не из DllMain,
    // загрузчик уже отпустил loader lock.
    try {
        g_workerThread.emplace(WorkerProc);
    } catch (...) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
        return false;
    }

    return true;
}

void StopWorker() {
    g_stopRequested.store(true, std::memory_order_release);

    HANDLE wakeEvent = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        wakeEvent = g_wakeEvent;
    }

    if (wakeEvent) {
        SetEvent(wakeEvent);
    }

    // join() дождётся завершения текущего запроса, но не дольше таймаутов
    // WinHTTP (максимум ~8 секунд на приём ответа).
    if (g_workerThread) {
        g_workerThread->join();
        g_workerThread.reset();
    }

    std::lock_guard<std::mutex> lock(g_stateMutex);
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
}

// ---------------------------------------------------------------------------
// 8. Перехват текста часов
// ---------------------------------------------------------------------------
//
// Идея: часы форматируют своё время вызовом GetTimeFormatEx. Если вызов
// происходит внутри обновления часов (см. хуки RefreshIcon / UpdateTextStrings
// ниже), мы дописываем к результату " | $12.34".
//
// Глубина вложенности, а не bool: на случай, если реализация часов вызовет
// RefreshIcon из RefreshIcon (или форматирование из форматирования), флаг не
// сбросится слишком рано.

thread_local int g_clockUpdateDepth = 0;

// Windows 10 за одно обновление форматирует дату дважды: сначала строку
// даты, потом строку дня недели. Баланс дописываем только к первой, иначе он
// появился бы на обеих строках.
thread_local int g_dateFormatCalls = 0;

// RAII-счётчик: если внутри форматирования часов случится исключение или
// ранний выход, глубина всё равно вернётся к прежнему значению, и суффикс не
// «прилипнет» к посторонним вызовам GetTimeFormatEx на этом потоке.
struct ClockUpdateScope {
    ClockUpdateScope() {
        ++g_clockUpdateDepth;
    }

    ~ClockUpdateScope() {
        --g_clockUpdateDepth;
    }
};

using GetTimeFormatEx_t = int(WINAPI*)(LPCWSTR lpLocaleName,
                                       DWORD dwFlags,
                                       const SYSTEMTIME* lpTime,
                                       LPCWSTR lpFormat,
                                       LPWSTR lpTimeStr,
                                       int cchTime);

GetTimeFormatEx_t GetTimeFormatEx_Original = nullptr;

// Дописывает " | CNY 86.61" к уже отформатированной строке. Возвращает новую
// длину вместе с завершающим нулём; если суффикс не нужен — исходное значение.
int AppendClockSuffix(int result, LPWSTR buffer, int cchBuffer) {
    std::wstring suffix;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        suffix = g_clockSuffix;
    }

    if (suffix.empty()) {
        return result;
    }

    // Контракт Get*FormatEx: возвращаемое значение включает завершающий ноль.
    const int newLength = (result - 1) + static_cast<int>(suffix.size()) + 1;

    // Вызов «узнать размер»: сообщаем требуемый размер, чтобы вызывающий код
    // сам выделил буфер.
    if (cchBuffer == 0) {
        return newLength;
    }

    // Буфера не хватает: оставляем исходный текст нетронутым и возвращаем
    // исходную длину. Возвращать здесь newLength нельзя — некоторые
    // вызывающие считают возвращённое значение числом записанных символов и
    // прочитали бы за пределами своего буфера.
    if (!buffer || cchBuffer < newLength) {
        return result;
    }

    memcpy(buffer + (result - 1), suffix.c_str(),
           (suffix.size() + 1) * sizeof(wchar_t));

    return newLength;
}

// Проверяет, выбрана ли в настройках эта строка часов ("time" или "date").
bool IsClockLineSelected(PCWSTR line) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    return _wcsicmp(g_settings.clockLine.c_str(), line) == 0;
}

int WINAPI GetTimeFormatEx_Hook(LPCWSTR lpLocaleName,
                                DWORD dwFlags,
                                const SYSTEMTIME* lpTime,
                                LPCWSTR lpFormat,
                                LPWSTR lpTimeStr,
                                int cchTime) {
    // Сначала получаем исходный текст (или требуемый размер буфера).
    const int result = GetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime,
                                                lpFormat, lpTimeStr, cchTime);

    // Вне обновления часов ничего не меняем: иначе суффикс попал бы в даты
    // свойств файлов, тултипы и другие места Проводника.
    if (g_clockUpdateDepth <= 0 || result <= 0 ||
        !IsClockLineSelected(L"time")) {
        return result;
    }

    return AppendClockSuffix(result, lpTimeStr, cchTime);
}

// Нижняя строка часов — дата. Мелкий шрифт, поэтому дополнительный текст там
// выглядит аккуратнее, чем на крупной строке со временем.
using GetDateFormatEx_t = int(WINAPI*)(LPCWSTR lpLocaleName,
                                       DWORD dwFlags,
                                       const SYSTEMTIME* lpDate,
                                       LPCWSTR lpFormat,
                                       LPWSTR lpDateStr,
                                       int cchDate,
                                       LPCWSTR lpCalendar);

GetDateFormatEx_t GetDateFormatEx_Original = nullptr;

int WINAPI GetDateFormatEx_Hook(LPCWSTR lpLocaleName,
                                DWORD dwFlags,
                                const SYSTEMTIME* lpDate,
                                LPCWSTR lpFormat,
                                LPWSTR lpDateStr,
                                int cchDate,
                                LPCWSTR lpCalendar) {
    const int result = GetDateFormatEx_Original(
        lpLocaleName, dwFlags, lpDate, lpFormat, lpDateStr, cchDate, lpCalendar);

    if (g_clockUpdateDepth <= 0 || result <= 0 ||
        !IsClockLineSelected(L"date")) {
        return result;
    }

    // DATE_LONGDATE — длинный формат, которым часы пользуются для тултипа:
    // его не трогаем, чтобы баланс не задваивался во всплывающей подсказке.
    if (dwFlags & DATE_LONGDATE) {
        return result;
    }

    // Дописываем только к первой дате за обновление (см. g_dateFormatCalls).
    if (cchDate != 0 && g_dateFormatCalls++ != 0) {
        return result;
    }

    return AppendClockSuffix(result, lpDateStr, cchDate);
}

// --- Windows 11: SystemTray.dll / Taskbar.View.dll / ExplorerExtensions.dll ---

using RefreshIcon_t = void(WINAPI*)(void* pThis, void* clockUpdate);

RefreshIcon_t RefreshIcon_Original = nullptr;
RefreshIcon_t RefreshIcon2_Original = nullptr;

// Внутренний метод модели часов, который перерисовывает иконку и текст.
// Обернув его, мы точно знаем: всё, что форматируется внутри, — это часы.
void WINAPI RefreshIcon_Hook(void* pThis, void* clockUpdate) {
    ClockUpdateScope scope;
    g_dateFormatCalls = 0;
    RefreshIcon_Original(pThis, clockUpdate);
}

// Часть сборок Windows 11 использует ClockSystemTrayIconDataModel2
// (появился с фича-флагом 38762814).
void WINAPI RefreshIcon2_Hook(void* pThis, void* clockUpdate) {
    ClockUpdateScope scope;
    g_dateFormatCalls = 0;
    RefreshIcon2_Original(pThis, clockUpdate);
}

// --- Windows 10: ClockButton в explorer.exe ---

using UpdateTextStringsIfNecessary_t = unsigned int(WINAPI*)(void* pThis,
                                                             bool* param);

UpdateTextStringsIfNecessary_t UpdateTextStringsIfNecessary_Original = nullptr;

// Пересчитывает строки времени/даты кнопки часов. Возвращает задержку до
// следующего обновления в миллисекундах — возвращаем её без изменений.
unsigned int WINAPI UpdateTextStringsIfNecessary_Hook(void* pThis, bool* param) {
    ClockUpdateScope scope;
    g_dateFormatCalls = 0;
    return UpdateTextStringsIfNecessary_Original(pThis, param);
}

// ---------------------------------------------------------------------------
// 9. Поиск внутренних символов и установка хуков
// ---------------------------------------------------------------------------

// Символы ищутся через WindhawkUtils::HookSymbols: утилита кэширует
// найденные адреса и не перечитывает PDB модуля при каждом поиске, в отличие
// от ручного перебора символов модуля.

bool IsSystemTrayModuleName(PCWSTR path) {
    if (!path) {
        return false;
    }

    PCWSTR baseName = wcsrchr(path, L'\\');
    baseName = baseName ? baseName + 1 : path;

    return _wcsicmp(baseName, L"SystemTray.dll") == 0 ||
           _wcsicmp(baseName, L"Taskbar.View.dll") == 0 ||
           _wcsicmp(baseName, L"ExplorerExtensions.dll") == 0;
}

// Подключает RefreshIcon в конкретном модуле. На разных сборках Windows 11
// классы лежат в разных модулях, поэтому вызывающий код перебирает их.
bool HookSystemTraySymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
            &RefreshIcon_Original,
            RefreshIcon_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
            &RefreshIcon2_Original,
            RefreshIcon2_Hook,
            /* optional */ true,  // Added with feature flag 38762814.
        },
    };

    if (!WindhawkUtils::HookSymbols(module, systemTrayDllHooks, ARRAYSIZE(systemTrayDllHooks))) {
        Wh_Log(L"HookSymbols (system tray) failed");
        return false;
    }

    return true;
}

// Версия модуля из его ресурса версии. Вызывается из Wh_ModInit,
// Wh_ModAfterInit и из хука LoadLibraryExW — там оригинальная функция уже
// вернулась, поэтому loader lock отпущен.
VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module) {
    HRSRC resource =
        FindResourceW(module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (!resource) {
        return nullptr;
    }

    HGLOBAL global = LoadResource(module, resource);
    if (!global) {
        return nullptr;
    }

    void* data = LockResource(global);
    if (!data) {
        return nullptr;
    }

    VS_FIXEDFILEINFO* info = nullptr;
    UINT size = 0;
    if (!VerQueryValueW(data, L"\\", reinterpret_cast<void**>(&info), &size) ||
        !info || size == 0) {
        return nullptr;
    }

    return info;
}

WORD GetModuleMajorVersion(HMODULE module) {
    VS_FIXEDFILEINFO* info = GetModuleVersionInfo(module);
    return info ? HIWORD(info->dwFileVersionMS) : 0;
}

// Возвращает единственный модуль, в котором могут жить типы SystemTray.
// Решает версия Taskbar.View.dll, а не сборка ОС: типы переехали в
// SystemTray.dll с версии 2604, и на 24H2/25H2 без этого обновления (включая
// LTSC 2024) SystemTray.dll ещё нет, а типы лежат в Taskbar.View.dll.
HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandleW(L"SystemTray.dll");
    if (module) {
        return module;
    }

    module = GetModuleHandleW(L"Taskbar.View.dll");
    if (module) {
        const WORD major = GetModuleMajorVersion(module);
        if (!major || major >= 2604) {
            // Типы уже переехали в SystemTray.dll — этот модуль не подходит.
            module = nullptr;
        }
    }

    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

// Подключает часы Windows 11. Флаг захватывается только вместе с конкретным
// модулем, поэтому HookSymbols не вызывается для заведомо неподходящих
// модулей и не повторяется (повторный вызов сбрасывает его кэш).
// Возвращает true, только если хуки установлены именно в этом вызове.
bool TryHookSystemTray() {
    HMODULE module = GetSystemTrayModuleHandle();
    if (!module) {
        return false;
    }

    if (g_systemTrayHooked.exchange(true, std::memory_order_acq_rel)) {
        return false;  // Уже подключено другим потоком или в Wh_ModInit.
    }

    return HookSystemTraySymbols(module);
}

// Windows 10: часы — обычная кнопка в MSTaskSwWClass, её строки
// пересчитывает ClockButton::UpdateTextStringsIfNecessary.
bool HookWin10Clock() {
    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {LR"(private: unsigned int __cdecl ClockButton::UpdateTextStringsIfNecessary(bool *))"},
            &UpdateTextStringsIfNecessary_Original,
            UpdateTextStringsIfNecessary_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(GetModuleHandleW(nullptr), explorerExeHooks,
                                    ARRAYSIZE(explorerExeHooks))) {
        Wh_Log(L"HookSymbols (ClockButton) failed");
        return false;
    }

    return true;
}

bool HookFormattingFunctions() {
    // Обязательно GetProcAddress из kernelbase.dll: адрес в kernel32.dll —
    // это лишь stub-переходник, и перехват по нему не сработает.
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        return false;
    }

    auto getTimeFormatEx = reinterpret_cast<decltype(&GetTimeFormatEx)>(
        GetProcAddress(kernelBase, "GetTimeFormatEx"));
    if (!getTimeFormatEx) {
        Wh_Log(L"GetTimeFormatEx not found");
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(getTimeFormatEx, GetTimeFormatEx_Hook,
                                        &GetTimeFormatEx_Original)) {
        Wh_Log(L"Cannot hook GetTimeFormatEx");
        return false;
    }

    auto getDateFormatEx = reinterpret_cast<decltype(&GetDateFormatEx)>(
        GetProcAddress(kernelBase, "GetDateFormatEx"));
    if (!getDateFormatEx) {
        Wh_Log(L"GetDateFormatEx not found");
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(getDateFormatEx, GetDateFormatEx_Hook,
                                        &GetDateFormatEx_Original)) {
        Wh_Log(L"Cannot hook GetDateFormatEx");
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// 10. Хук LoadLibraryExW: часы Windows 11 могут загрузиться позже мода
// ---------------------------------------------------------------------------

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR lpLibFileName,
                                         HANDLE hFile,
                                         DWORD dwFlags);

LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    // SystemTray.dll (новые сборки) или Taskbar.View.dll (22H2) загружаются
    // при старте оболочки. LoadLibraryExW уже вернулся, loader lock отпущен,
    // поэтому здесь можно и прочитать версию модуля, и поставить хуки —
    // так же делает taskbar-clock-customization.
    if (module && IsSystemTrayModuleName(lpLibFileName) &&
        module == GetSystemTrayModuleHandle() && TryHookSystemTray()) {
        // Хуки, зарегистрированные после Wh_ModInit, применяются только
        // явным вызовом Wh_ApplyHookOperations.
        Wh_ApplyHookOperations();
    }

    return module;
}

bool HookLateModuleLoading() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        return false;
    }

    auto loadLibraryExW =
        reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW"));
    if (!loadLibraryExW) {
        return false;
    }

    return WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                          &LoadLibraryExW_Original) != FALSE;
}

// ---------------------------------------------------------------------------
// 11. Определение версии Windows
// ---------------------------------------------------------------------------

DWORD GetWindowsBuildNumber() {
    static DWORD cachedBuild = 0;
    if (cachedBuild) {
        return cachedBuild;
    }

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll) {
        using RtlGetVersion_t = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);

        auto rtlGetVersion =
            reinterpret_cast<RtlGetVersion_t>(GetProcAddress(ntdll, "RtlGetVersion"));
        if (rtlGetVersion) {
            RTL_OSVERSIONINFOW versionInfo{};
            versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

            if (rtlGetVersion(&versionInfo) == 0) {
                cachedBuild = versionInfo.dwBuildNumber;
            }
        }
    }

    return cachedBuild;
}

}  // namespace

// ---------------------------------------------------------------------------
// 12. Точки входа Windhawk
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L">");

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        LoadSettingsLocked();
        g_rubPerUnit = LookupRateLocked(g_currency.c_str());
        RebuildSuffixLocked();
    }

    if (!HookFormattingFunctions()) {
        return FALSE;
    }

    const DWORD build = GetWindowsBuildNumber();
    Wh_Log(L"Windows build: %u", build);

    if (build >= 22000) {
        // Windows 11: XAML-часы. Если модуль с типами SystemTray ещё не
        // загружен, ждём его через LoadLibraryExW.
        if (GetSystemTrayModuleHandle()) {
            if (!TryHookSystemTray()) {
                Wh_Log(L"Cannot hook the system tray clock");
            }
        } else {
            Wh_Log(L"System tray module is not loaded yet, waiting for it");

            if (!HookLateModuleLoading()) {
                Wh_Log(L"Cannot hook LoadLibraryExW");
                return FALSE;
            }
        }
    } else {
        // Windows 10: классическая кнопка часов.
        if (!HookWin10Clock()) {
            Wh_Log(L"Cannot hook the clock button");
            return FALSE;
        }
    }

    if (!StartWorker()) {
        Wh_Log(L"Cannot start the worker thread");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Гонка на старте: модуль часов мог загрузиться между Wh_ModInit и
    // Wh_ModAfterInit. Пробуем ещё раз, не дожидаясь LoadLibraryExW.
    if (GetWindowsBuildNumber() < 22000) {
        return;
    }

    if (!GetSystemTrayModuleHandle()) {
        return;
    }

    // TryHookSystemTray возвращает true, только если хуки установлены именно
    // сейчас: если их уже поставил Wh_ModInit или LoadLibraryExW_Hook,
    // применять операции повторно не нужно.
    if (!TryHookSystemTray()) {
        return;
    }

    Wh_ApplyHookOperations();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Останавливаем поток ДО выгрузки: он может держать HINTERNET, а
    // использовать код мода после unload нельзя.
    StopWorker();

    // Сами перехваты снимает Windhawk при выгрузке мода: вызывать
    // Wh_RemoveFunctionHook для каждого адреса не требуется, движок хранит
    // список установленных хуков и откатывает их сам.
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        LoadSettingsLocked();
        g_rubPerUnit = LookupRateLocked(g_currency.c_str());
        RebuildSuffixLocked();

        // Разбудить фоновый поток, чтобы новый ключ/префикс применились
        // сразу, не дожидаясь конца интервала. SetEvent вызывается под тем же
        // мьютексом, под которым StopWorker закрывает дескриптор, — так
        // исключается обращение к уже закрытому событию при выгрузке мода.
        if (g_wakeEvent) {
            SetEvent(g_wakeEvent);
        }
    }

    // Структура мода не меняется — полная перезагрузка не нужна.
    *bReload = FALSE;
    return TRUE;
}
