// ==WindhawkMod==
// @id              ai-balance-manager
// @name            AI Balance Manager
// @name:ru-RU      AI Balance — балансы API
// @description     Shows remaining AI API credit on the Windows 11 taskbar
// @description:ru-RU Показывает остаток средств на API-счетах ИИ прямо на панели задач Windows 11
// @version         1.0.1
// @author          VitalyVolkov87
// @github          https://github.com/VitalyVolkov87
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -lwinhttp -lcrypt32 -lshell32 -lcomctl32 -lgdi32
// ==/WindhawkMod==

// XAML taskbar discovery and window-thread marshaling are based on Taskbar
// System Info and Multirow taskbar for Windows 11 (GPL-3.0).

// ==WindhawkModReadme==
/*
# AI Balance Manager

Shows the remaining balance of your AI API accounts directly in the Windows 11
taskbar. It is useful when several APIs are prepaid and you want to notice a
low balance before requests start failing, without opening every provider's
billing page.

AI Balance Manager runs entirely as a Windhawk mod. It does not install or run
a companion application, background service, or separate tray process. It adds
a small XAML widget next to the taskbar's existing buttons and does not change
the clock, date, weather, or other taskbar elements.

## Features

- Automatic balances for DeepSeek, Kimi / Moonshot, and OpenRouter.
- Presets for OpenAI, Anthropic Claude, Google Gemini, xAI Grok, Alibaba Qwen,
  Mistral, Groq, and Together AI. Those providers do not publish a balance
  endpoint, so their amount is entered manually.
- Add, remove, rename, enable/disable, and reorder providers.
- Custom HTTPS GET/JSON endpoint with configurable value and subtraction paths.
- Configurable refresh interval.
- API keys are encrypted with Windows DPAPI for the current Windows user.
- English interface by default; Russian can be selected in the Windhawk mod
  settings.
- Clicking a provider opens its website. Right-clicking the widget opens the
  settings window. Double-clicking refreshes all balances.

## Setup

1. Install and enable the mod in Windhawk.
2. Right-click the balance widget and choose settings.
3. Select DeepSeek, Kimi, or OpenRouter and paste the API key from the provider's
   dashboard. Endpoint, auth header, currency, and JSON path are already set.
4. For a provider without a public balance API, choose its preset and enter the
   current amount manually.
5. Set the refresh interval and save.
6. Select the interface language in the Windhawk mod settings. English is the
   default; the setting applies when the settings window is reopened.

`Left offset` moves the widget horizontally when another taskbar app occupies
the area next to the weather button.

## Network access

The mod contacts these hosts only for enabled, non-manual providers:

- `https://api.deepseek.com/user/balance` — DeepSeek.
- `https://api.moonshot.ai/v1/users/me/balance` — Kimi / Moonshot.
- `https://openrouter.ai/api/v1/credits` — OpenRouter.
- Any HTTPS URL configured by the user for a custom provider.

Requests are HTTPS `GET` requests made with WinHTTP. The API key is sent in the
configured header, by default `Authorization: Bearer <key>`. The user agent is
`AI-Balance-Windhawk/1.0`. The response is a JSON body of up to 4 MB, from which
only a number is read; no response content is executed.

Requests run when the mod loads, every configured number of minutes, after
saving settings, and when the widget is double-clicked. The built-in DeepSeek
and Kimi presets have no key by default and make no request until a key is
entered. Disable or remove a provider to stop its requests; removing the mod
removes all requests.

## Privacy

API keys are stored only on this PC and protected with Windows DPAPI for the
current Windows account. Each key is sent only to the HTTPS endpoint of its own
provider. The mod sends no telemetry and does not log keys.

Balances come from provider APIs and may be delayed or unavailable. Manual
amounts are not refreshed automatically.

## Related mods

[DeepSeek Balance in Taskbar](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/deepseek-balance-tray.wh.cpp)
shows DeepSeek's balance inside the clock text. This mod exists separately
because it also supports Kimi, OpenRouter, arbitrary HTTPS/JSON balance
endpoints, and manually entered amounts, and presents them in a separate XAML
widget instead of changing the clock.

Taskbar AI Quota and Taskbar AI Usage display subscription quota or usage
limits. They do not read prepaid API-account balances, so the overlap is small.

## Requirements

- Windows 11, 64-bit.
- Windhawk with the mod enabled for `explorer.exe`.

## License

GPL-3.0. Taskbar XAML discovery and window-thread marshaling are based on the
GPL-3.0 projects Taskbar System Info and Multirow taskbar for Windows 11.

---

## Русский

Показывает остаток средств на API-счетах ИИ прямо на панели задач Windows 11.
Мод помогает заметить низкий баланс до того, как запросы начнут завершаться
ошибкой, и не требует открывать страницы оплаты каждого сервиса.

Мод работает полностью внутри Windhawk: отдельная программа, служба или
значок в трее не устанавливаются. Он добавляет небольшой XAML-виджет рядом с
существующими кнопками панели задач и не изменяет часы, дату, погоду и другие
элементы.

### Возможности

- Автоматический баланс DeepSeek, Kimi / Moonshot и OpenRouter.
- Пресеты OpenAI, Anthropic Claude, Google Gemini, xAI Grok, Alibaba Qwen,
  Mistral, Groq и Together AI. Эти сервисы не публикуют API баланса, поэтому
  сумма для них вводится вручную.
- Добавление, удаление, переименование, включение и изменение порядка сервисов.
- Произвольный HTTPS GET/JSON API с настраиваемыми путями значения и вычитания.
- Настраиваемый интервал обновления.
- API-ключи шифруются Windows DPAPI для текущего пользователя Windows.
- Английский интерфейс по умолчанию; русский выбирается в настройках мода
  Windhawk.
- Щелчок по сервису открывает его сайт, правая кнопка открывает настройки,
  двойной щелчок обновляет балансы.

### Настройка

1. Установите и включите мод в Windhawk.
2. Нажмите правой кнопкой по виджету и выберите настройки.
3. Выберите DeepSeek, Kimi или OpenRouter и вставьте API-ключ из кабинета
   сервиса. Адрес API, заголовок, валюта и путь JSON уже заполнены.
4. Для сервиса без публичного API баланса выберите его пресет и укажите сумму
   вручную.
5. Задайте интервал обновления и нажмите «Сохранить».
6. Выберите язык интерфейса в настройках мода Windhawk. По умолчанию English;
   настройка применяется после повторного открытия окна настроек.

Параметр `Left offset` сдвигает виджет по горизонтали, если место рядом с
погодой занято другим приложением.

### Сетевые запросы

Мод обращается к указанным ниже адресам только для включённых сервисов, баланс
которых не задан вручную:

- `https://api.deepseek.com/user/balance` — DeepSeek.
- `https://api.moonshot.ai/v1/users/me/balance` — Kimi / Moonshot.
- `https://openrouter.ai/api/v1/credits` — OpenRouter.
- Любой HTTPS-адрес, указанный пользователем для произвольного сервиса.

Запросы выполняются по HTTPS методом `GET` через WinHTTP. API-ключ передаётся
в настроенном заголовке, по умолчанию `Authorization: Bearer <key>`;
user-agent — `AI-Balance-Windhawk/1.0`. Ответ — JSON до 4 МБ, из которого
читается только число; содержимое не исполняется.

Запросы выполняются при загрузке мода, каждые заданные минуты, после сохранения
настроек и по двойному щелчку по виджету. У встроенных пресетов DeepSeek и Kimi
по умолчанию нет ключа, поэтому без введённого ключа запросы не отправляются.
Чтобы остановить запросы, отключите или удалите сервис; удаление мода прекращает
все запросы.

### Приватность

Ключи хранятся только на этом компьютере и защищены Windows DPAPI для текущей
учётной записи. Каждый ключ отправляется только на HTTPS-адрес своего сервиса.
Мод не передаёт телеметрию и не записывает ключи в журнал.

Баланс зависит от API сервиса и может обновляться с задержкой. Ручные суммы
автоматически не обновляются.

### Связанные моды

Мод DeepSeek Balance in Taskbar показывает баланс DeepSeek в тексте часов.
Этот мод сделан отдельно, потому что дополнительно поддерживает Kimi,
OpenRouter, произвольные HTTPS/JSON-адреса и ручные суммы, а также выводит
данные в отдельном XAML-виджете, не изменяя часы.

Taskbar AI Quota и Taskbar AI Usage показывают квоты и лимиты подписок, а не
остаток средств на API-счетах, поэтому пересечение с ними небольшое.

### Требования

- Windows 11, 64 бита.
- Windhawk с включённым модом для `explorer.exe`.

### Лицензия

GPL-3.0. Поиск XAML-элементов панели задач основан на GPL-3.0 проектах
Taskbar System Info и Multirow taskbar for Windows 11.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- leftOffset: 170
  $name: Left offset
- language: en
  $name: Interface language
  $description: English is used by default. Changes apply to the settings window after it is reopened.
  $options:
  - en: English
  - ru: Русский
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <objbase.h>
#include <commctrl.h>
#include <cryptuiapi.h>
#include <shellapi.h>
#include <winhttp.h>
#include <wincrypt.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cwchar>
#include <functional>
#include <list>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Input;
using winrt::Windows::Data::Json::IJsonValue;
using winrt::Windows::Data::Json::JsonArray;
using winrt::Windows::Data::Json::JsonObject;
using winrt::Windows::Data::Json::JsonValue;
using winrt::Windows::Data::Json::JsonValueType;

namespace {

constexpr wchar_t kWidgetName[] = L"AIBalanceManagerWidget";
constexpr wchar_t kSettingsClass[] = L"AIBalanceManagerSettingsWindow";

enum class ProviderKind : int { DeepSeek = 0, Kimi = 1, Custom = 2, Manual = 3 };

struct PresetDefinition {
    const wchar_t* id;
    const wchar_t* name;
    const wchar_t* nameRu;
    ProviderKind kind;
    const wchar_t* website;
    const wchar_t* apiUrl;
    const wchar_t* jsonPath;
    const wchar_t* subtractPath;
};

const PresetDefinition kPresetDefinitions[] = {
    {L"deepseek", L"DeepSeek", L"DeepSeek", ProviderKind::DeepSeek,
     L"https://platform.deepseek.com", L"https://api.deepseek.com/user/balance", L"", L""},
    {L"kimi", L"Kimi / Moonshot", L"Kimi / Moonshot", ProviderKind::Kimi,
     L"https://platform.kimi.ai", L"https://api.moonshot.ai/v1/users/me/balance", L"", L""},
    {L"openrouter", L"OpenRouter", L"OpenRouter", ProviderKind::Custom,
     L"https://openrouter.ai/settings/credits", L"https://openrouter.ai/api/v1/credits",
     L"data.total_credits", L"data.total_usage"},
    {L"openai", L"OpenAI / ChatGPT (manual balance)", L"OpenAI / ChatGPT (баланс вручную)", ProviderKind::Manual,
     L"https://platform.openai.com/settings/organization/billing/overview", L"", L"", L""},
    {L"anthropic", L"Anthropic Claude (manual balance)", L"Anthropic Claude (баланс вручную)", ProviderKind::Manual,
     L"https://console.anthropic.com/settings/billing", L"", L"", L""},
    {L"gemini", L"Google Gemini (manual balance)", L"Google Gemini (баланс вручную)", ProviderKind::Manual,
     L"https://aistudio.google.com/app/apikey", L"", L"", L""},
    {L"xai", L"xAI Grok (manual balance)", L"xAI Grok (баланс вручную)", ProviderKind::Manual,
     L"https://console.x.ai", L"", L"", L""},
    {L"qwen", L"Alibaba Qwen / DashScope (manual balance)", L"Alibaba Qwen / DashScope (вручную)", ProviderKind::Manual,
     L"https://bailian.console.aliyun.com", L"", L"", L""},
    {L"mistral", L"Mistral AI (manual balance)", L"Mistral AI (вручную)", ProviderKind::Manual,
     L"https://console.mistral.ai", L"", L"", L""},
    {L"groq", L"Groq (manual balance)", L"Groq (вручную)", ProviderKind::Manual,
     L"https://console.groq.com/settings/billing", L"", L"", L""},
    {L"together", L"Together AI (manual balance)", L"Together AI (вручную)", ProviderKind::Manual,
     L"https://api.together.ai/settings/billing", L"", L"", L""},
    {L"custom", L"Other API (GET / JSON)", L"Другой API (GET / JSON)", ProviderKind::Custom,
     L"https://example.com", L"", L"balance", L""},
    {L"manual", L"Other manual amount", L"Другая сумма вручную", ProviderKind::Manual,
     L"https://example.com", L"", L"", L""},
};

struct Provider {
    std::wstring id, preset, name = L"New service", website = L"https://example.com";
    ProviderKind kind = ProviderKind::Custom;
    bool enabled = true;
    std::wstring apiUrl, jsonPath = L"balance", subtractPath, currency = L"USD";
    std::wstring authHeader = L"Authorization", authPrefix = L"Bearer";
    std::wstring secret, manual = L"";
    // Runtime only; never serialized.
    bool hasValue = false, stale = false;
    double value = 0;
    std::wstring valueCurrency = L"USD", error;
};

std::mutex dataMutex;
std::vector<Provider> providers;
int refreshMinutes = 5;
std::atomic<uint64_t> configRevision{1}, dataRevision{1};
std::atomic<bool> unloading = false;
std::atomic<int> leftOffset = 170;
HANDLE stopEvent = nullptr, wakeEvent = nullptr, networkThread = nullptr;
std::atomic<HWND> settingsWindow{nullptr};
std::atomic<int> languageCode{0};

const wchar_t* Tr(const wchar_t* english, const wchar_t* russian) {
    return languageCode.load() == 1 ? russian : english;
}

const wchar_t* PresetName(const PresetDefinition& preset) {
    return languageCode.load() == 1 && preset.nameRu ? preset.nameRu : preset.name;
}

void LoadLanguage() {
    PCWSTR value = Wh_GetStringSetting(L"language");
    languageCode = value && _wcsicmp(value, L"ru") == 0 ? 1 : 0;
    if (value) Wh_FreeStringSetting(value);
}

std::wstring StorageFile(const wchar_t* name);
std::string ToUtf8(std::wstring_view value);

std::wstring Trim(std::wstring value) {
    size_t first = value.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return {};
    size_t last = value.find_last_not_of(L" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string ToUtf8(std::wstring_view value) {
    if (value.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()),
                                   nullptr, 0, nullptr, nullptr);
    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()),
                        result.data(), size, nullptr, nullptr);
    return result;
}

std::wstring FromUtf8(std::string_view value) {
    if (value.empty()) return {};
    int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(),
                                   static_cast<int>(value.size()), nullptr, 0);
    if (size <= 0) throw winrt::hresult_error(E_INVALIDARG);
    std::wstring result(size, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(),
                        static_cast<int>(value.size()), result.data(), size);
    return result;
}

std::wstring StorageFile(const wchar_t* name) {
    wchar_t folder[MAX_PATH];
    if (!Wh_GetModStoragePath(folder, ARRAYSIZE(folder))) return {};
    CreateDirectoryW(folder, nullptr);
    return std::wstring(folder) + L"\\" + name;
}

bool WriteFileAtomic(const std::wstring& path, const std::string& bytes) {
    if (path.empty()) return false;
    std::wstring temp = path + L".tmp";
    HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    BOOL ok = WriteFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr);
    CloseHandle(file);
    return ok && written == bytes.size() &&
           MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
}

bool ReadFileBytes(const std::wstring& path, std::string& bytes, bool missingAllowed = true) {
    bytes.clear();
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return missingAllowed && GetLastError() == ERROR_FILE_NOT_FOUND;
    DWORD size = GetFileSize(file, nullptr);
    if (size == INVALID_FILE_SIZE || size > 4 * 1024 * 1024) {
        CloseHandle(file);
        return false;
    }
    bytes.resize(size);
    DWORD read = 0;
    BOOL ok = size == 0 || ReadFile(file, bytes.data(), size, &read, nullptr);
    CloseHandle(file);
    return ok && read == size;
}

std::wstring NewId() {
    GUID guid{};
    if (FAILED(CoCreateGuid(&guid))) return std::to_wstring(GetTickCount64());
    wchar_t text[40]{};
    StringFromGUID2(guid, text, ARRAYSIZE(text));
    return text;
}

std::wstring Base64Encode(const BYTE* data, DWORD size) {
    DWORD length = 0;
    if (!CryptBinaryToStringW(data, size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                              nullptr, &length)) return {};
    std::wstring result(length ? length - 1 : 0, L'\0');
    CryptBinaryToStringW(data, size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
                         result.data(), &length);
    return result;
}

std::vector<BYTE> Base64Decode(const std::wstring& text) {
    DWORD size = 0;
    if (!CryptStringToBinaryW(text.c_str(), static_cast<DWORD>(text.size()),
                              CRYPT_STRING_BASE64, nullptr, &size, nullptr, nullptr)) return {};
    std::vector<BYTE> result(size);
    if (!CryptStringToBinaryW(text.c_str(), static_cast<DWORD>(text.size()),
                              CRYPT_STRING_BASE64, result.data(), &size, nullptr, nullptr)) return {};
    return result;
}

std::wstring ProtectSecret(const std::wstring& plain) {
    if (plain.empty()) return {};
    DATA_BLOB input{static_cast<DWORD>(plain.size() * sizeof(wchar_t)),
                    reinterpret_cast<BYTE*>(const_cast<wchar_t*>(plain.data()))};
    DATA_BLOB output{};
    if (!CryptProtectData(&input, L"AI Balance API key", nullptr, nullptr, nullptr,
                          CRYPTPROTECT_UI_FORBIDDEN, &output)) return {};
    std::wstring result = Base64Encode(output.pbData, output.cbData);
    LocalFree(output.pbData);
    return result;
}

std::wstring RevealSecret(const std::wstring& encrypted) {
    if (encrypted.empty()) return {};
    auto bytes = Base64Decode(encrypted);
    if (bytes.empty()) return {};
    DATA_BLOB input{static_cast<DWORD>(bytes.size()), bytes.data()}, output{};
    if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr,
                            CRYPTPROTECT_UI_FORBIDDEN, &output)) return {};
    std::wstring result(reinterpret_cast<wchar_t*>(output.pbData), output.cbData / sizeof(wchar_t));
    SecureZeroMemory(output.pbData, output.cbData);
    LocalFree(output.pbData);
    return result;
}

bool IsHttps(const std::wstring& text) {
    return _wcsnicmp(text.c_str(), L"https://", 8) == 0 && text.find_first_of(L"\r\n\t") == std::wstring::npos;
}

Provider Preset(ProviderKind kind) {
    size_t index = 0;
    if (kind == ProviderKind::Kimi) index = 1;
    else if (kind == ProviderKind::Custom) index = 11;
    else if (kind == ProviderKind::Manual) index = 12;
    const auto& d = kPresetDefinitions[index];
    Provider p;
    p.id = NewId();
    p.preset = d.id;
    p.name = PresetName(d);
    p.kind = d.kind;
    p.website = d.website;
    p.apiUrl = d.apiUrl;
    p.jsonPath = d.jsonPath;
    p.subtractPath = d.subtractPath;
    p.currency = L"USD";
    return p;
}

Provider PresetByIndex(size_t index) {
    if (index >= std::size(kPresetDefinitions)) index = std::size(kPresetDefinitions) - 1;
    const auto& d = kPresetDefinitions[index];
    Provider p;
    p.id = NewId();
    p.preset = d.id;
    p.name = PresetName(d);
    p.kind = d.kind;
    p.website = d.website;
    p.apiUrl = d.apiUrl;
    p.jsonPath = d.jsonPath;
    p.subtractPath = d.subtractPath;
    p.currency = L"USD";
    return p;
}

size_t PresetIndexOf(const Provider& p) {
    if (!p.preset.empty()) {
        for (size_t i = 0; i < std::size(kPresetDefinitions); i++)
            if (p.preset == kPresetDefinitions[i].id) return i;
    }
    if (p.kind == ProviderKind::DeepSeek) return 0;
    if (p.kind == ProviderKind::Kimi) return 1;
    for (size_t i = 0; i < std::size(kPresetDefinitions); i++) {
        if (p.apiUrl == kPresetDefinitions[i].apiUrl &&
            (p.name == kPresetDefinitions[i].name || p.name == kPresetDefinitions[i].nameRu))
            return i;
    }
    return p.kind == ProviderKind::Manual ? 12 : 11;
}

std::wstring JsonString(const JsonObject& object, const wchar_t* name, const std::wstring& fallback = {}) {
    if (!object.HasKey(name)) return fallback;
    auto value = object.GetNamedValue(name);
    if (value.ValueType() == JsonValueType::String) return value.GetString().c_str();
    return fallback;
}

double JsonNumber(const IJsonValue& value) {
    if (value.ValueType() == JsonValueType::Number) return value.GetNumber();
    if (value.ValueType() == JsonValueType::String)
        return std::stod(value.GetString().c_str());
    throw winrt::hresult_error(E_INVALIDARG);
}

bool JsonBool(const JsonObject& object, const wchar_t* name, bool fallback) {
    if (!object.HasKey(name)) return fallback;
    auto value = object.GetNamedValue(name);
    return value.ValueType() == JsonValueType::Boolean ? value.GetBoolean() : fallback;
}

int JsonInt(const JsonObject& object, const wchar_t* name, int fallback) {
    if (!object.HasKey(name)) return fallback;
    auto value = object.GetNamedValue(name);
    return value.ValueType() == JsonValueType::Number ? static_cast<int>(value.GetNumber()) : fallback;
}

void SetString(JsonObject& object, const wchar_t* name, const std::wstring& value) {
    object.SetNamedValue(name, JsonValue::CreateStringValue(value));
}

Provider ReadProvider(const JsonObject& object) {
    Provider p;
    p.id = JsonString(object, L"id");
    if (p.id.empty()) p.id = NewId();
    p.preset = JsonString(object, L"preset");
    p.name = JsonString(object, L"name", Tr(L"Service", L"Сервис"));
    p.website = JsonString(object, L"website", L"https://example.com");
    ProviderKind storedKind = static_cast<ProviderKind>(JsonInt(object, L"kind", 2));
    if (storedKind < ProviderKind::DeepSeek || storedKind > ProviderKind::Manual)
        storedKind = ProviderKind::Custom;
    p.kind = storedKind;
    for (const auto& preset : kPresetDefinitions) {
        if (!p.preset.empty() && p.preset == preset.id) {
            p.kind = preset.kind;
            break;
        }
    }
    p.enabled = JsonBool(object, L"enabled", true);
    p.apiUrl = JsonString(object, L"apiUrl");
    p.jsonPath = JsonString(object, L"jsonPath", L"balance");
    p.subtractPath = JsonString(object, L"subtractPath");
    p.currency = JsonString(object, L"currency", L"USD");
    p.authHeader = JsonString(object, L"authHeader", L"Authorization");
    p.authPrefix = JsonString(object, L"authPrefix", L"Bearer");
    p.secret = JsonString(object, L"secret");
    p.manual = JsonString(object, L"manual");
    return p;
}

JsonObject WriteProvider(const Provider& p) {
    JsonObject object;
    SetString(object, L"id", p.id);
    SetString(object, L"preset", p.preset);
    SetString(object, L"name", p.name);
    SetString(object, L"website", p.website);
    object.SetNamedValue(L"kind", JsonValue::CreateNumberValue(static_cast<int>(p.kind)));
    object.SetNamedValue(L"enabled", JsonValue::CreateBooleanValue(p.enabled));
    SetString(object, L"apiUrl", p.apiUrl);
    SetString(object, L"jsonPath", p.jsonPath);
    SetString(object, L"subtractPath", p.subtractPath);
    SetString(object, L"currency", p.currency);
    SetString(object, L"authHeader", p.authHeader);
    SetString(object, L"authPrefix", p.authPrefix);
    SetString(object, L"secret", p.secret);
    SetString(object, L"manual", p.manual);
    return object;
}

void LoadProviders() {
    std::vector<Provider> loaded;
    int minutes = 5;
    std::string bytes;
    std::wstring path = StorageFile(L"providers.json");
    try {
        if (ReadFileBytes(path, bytes) && !bytes.empty()) {
            auto root = JsonObject::Parse(FromUtf8(bytes));
            minutes = std::clamp(JsonInt(root, L"refreshMinutes", 5), 1, 1440);
            auto array = root.GetNamedArray(L"providers");
            for (const auto& value : array) {
                if (value.ValueType() == JsonValueType::Object)
                    loaded.push_back(ReadProvider(value.GetObject()));
            }
        }
    } catch (...) {
        loaded.clear();
    }
    if (loaded.empty() && providers.empty()) {
        loaded.push_back(Preset(ProviderKind::DeepSeek));
        loaded.push_back(Preset(ProviderKind::Kimi));
    }
    {
        std::lock_guard lock(dataMutex);
        providers = std::move(loaded);
        refreshMinutes = minutes;
        configRevision++;
        dataRevision++;
    }
}

bool SaveProviders(const std::vector<Provider>& value, int minutes) {
    minutes = std::clamp(minutes, 1, 1440);
    {
        std::lock_guard lock(dataMutex);
        providers = value;
        refreshMinutes = minutes;
        configRevision++;
        dataRevision++;
    }
    try {
        JsonObject root;
        root.SetNamedValue(L"refreshMinutes", JsonValue::CreateNumberValue(minutes));
        JsonArray array;
        for (const auto& p : value) array.Append(WriteProvider(p));
        root.SetNamedValue(L"providers", array);
        if (!WriteFileAtomic(StorageFile(L"providers.json"), ToUtf8(root.Stringify().c_str())))
            return false;
    } catch (...) {
        return false;
    }
    if (wakeEvent) SetEvent(wakeEvent);
    return true;
}

std::vector<Provider> ProvidersSnapshot() {
    std::lock_guard lock(dataMutex);
    return providers;
}

std::wstring CurrencySuffix(const std::wstring& currency) {
    if (currency == L"USD") return L"$";
    if (currency == L"CNY") return L"¥";
    if (currency == L"EUR") return L"€";
    if (currency == L"RUB") return L"₽";
    return L" " + currency;
}

std::wstring FormatMoney(double amount, const std::wstring& currency) {
    wchar_t buffer[80];
    swprintf(buffer, ARRAYSIZE(buffer), L"%.2f", amount);
    wchar_t decimal[8]{};
    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, decimal,
                         ARRAYSIZE(decimal)) || !decimal[0]) {
        wcscpy(decimal, L".");
    }
    std::wstring result = buffer;
    size_t dot = result.find(L'.');
    if (dot != std::wstring::npos) result.replace(dot, 1, decimal);
    return result + CurrencySuffix(currency);
}

std::wstring DisplayText(const Provider& p) {
    if (p.hasValue) return p.name + L" " + (p.stale ? L"~" : L"") + FormatMoney(p.value, p.valueCurrency);
    return p.name + L" —";
}

struct FetchError {
    std::wstring message;
};

struct InternetCloser {
    void operator()(HINTERNET handle) const {
        if (handle) WinHttpCloseHandle(handle);
    }
};
using InternetPtr = std::unique_ptr<void, InternetCloser>;

std::string HttpGet(const std::wstring& url, const Provider& provider,
                    const std::wstring& key) {
    URL_COMPONENTS parts{sizeof(parts)};
    wchar_t host[256]{}, path[2048]{}, extra[2048]{};
    parts.lpszHostName = host;
    parts.dwHostNameLength = ARRAYSIZE(host);
    parts.lpszUrlPath = path;
    parts.dwUrlPathLength = ARRAYSIZE(path);
    parts.lpszExtraInfo = extra;
    parts.dwExtraInfoLength = ARRAYSIZE(extra);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts) || parts.nScheme != INTERNET_SCHEME_HTTPS)
        throw FetchError{Tr(L"A valid HTTPS API address is required",
                            L"Нужен корректный HTTPS-адрес API")};

    InternetPtr session(WinHttpOpen(L"AI-Balance-Windhawk/1.0",
                                    WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
    if (!session) throw FetchError{Tr(L"Could not create an HTTP session",
                                      L"Не удалось создать HTTP-сеанс")};
    BOOL fastIpv4Fallback = TRUE;
    WinHttpSetOption(session.get(), WINHTTP_OPTION_IPV6_FAST_FALLBACK,
                     &fastIpv4Fallback, sizeof(fastIpv4Fallback));
    InternetPtr connection(WinHttpConnect(session.get(), host, parts.nPort, 0));
    if (!connection) throw FetchError{Tr(L"Could not connect to the server",
                                        L"Не удалось подключиться к серверу")};
    std::wstring target = std::wstring(path) + extra;
    InternetPtr request(WinHttpOpenRequest(connection.get(), L"GET", target.c_str(),
                                           nullptr, WINHTTP_NO_REFERER,
                                           WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE));
    if (!request) throw FetchError{Tr(L"Could not create the HTTP request",
                                      L"Не удалось создать HTTP-запрос")};
    WinHttpSetOption(request.get(), WINHTTP_OPTION_IPV6_FAST_FALLBACK,
                     &fastIpv4Fallback, sizeof(fastIpv4Fallback));
    WinHttpSetTimeouts(request.get(), 5000, 5000, 10000, 10000);

    if (!key.empty()) {
        if (provider.authHeader.empty() ||
            provider.authHeader.find_first_not_of(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-") != std::wstring::npos)
            throw FetchError{Tr(L"Invalid API key header name",
                                L"Неверное имя заголовка ключа")};
        if (key.find_first_of(L"\r\n") != std::wstring::npos ||
            provider.authPrefix.find_first_of(L"\r\n") != std::wstring::npos)
            throw FetchError{Tr(L"Invalid API key", L"Неверный API-ключ")};
        std::wstring header = provider.authHeader + L": " +
                              (provider.authPrefix.empty() ? L"" : provider.authPrefix + L" ") + key;
        if (!WinHttpAddRequestHeaders(request.get(), header.c_str(),
                                      static_cast<DWORD>(header.size()), WINHTTP_ADDREQ_FLAG_ADD))
            throw FetchError{Tr(L"Could not add the API key header",
                                L"Не удалось добавить API-ключ")};
    }
    if (!WinHttpSendRequest(request.get(), WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(request.get(), nullptr))
        throw FetchError{Tr(L"No connection to the API", L"Нет соединения с API")};

    DWORD status = 0, statusSize = sizeof(status);
    WinHttpQueryHeaders(request.get(), WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                        WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                        WINHTTP_NO_HEADER_INDEX);
    if (status != 200) {
        if (status == 401 || status == 403)
            throw FetchError{std::wstring(Tr(L"Key rejected (HTTP ", L"Ключ не принят (HTTP ")) +
                              std::to_wstring(status) + L")"};
        if (status == 429) throw FetchError{Tr(
            L"API rate limit reached (429)", L"API ограничил частоту запросов (429)")};
        throw FetchError{std::wstring(Tr(L"API error HTTP ", L"Ошибка API HTTP ")) +
                         std::to_wstring(status)};
    }

    std::string body;
    for (;;) {
        DWORD available = 0;
        if (!WinHttpQueryDataAvailable(request.get(), &available))
            throw FetchError{Tr(L"Could not read the API response",
                                L"Ошибка чтения ответа API")};
        if (!available) break;
        if (body.size() + available > 4 * 1024 * 1024)
            throw FetchError{Tr(L"API response is too large",
                                L"Слишком большой ответ API")};
        size_t oldSize = body.size();
        body.resize(oldSize + available);
        DWORD read = 0;
        if (!WinHttpReadData(request.get(), body.data() + oldSize, available, &read))
            throw FetchError{Tr(L"Could not read the API response",
                                L"Ошибка чтения ответа API")};
        body.resize(oldSize + read);
        if (!read) break;
    }
    return body;
}

IJsonValue JsonAt(const IJsonValue& root, std::wstring path) {
    if (path.rfind(L"$.", 0) == 0) path.erase(0, 2);
    for (auto& c : path) {
        if (c == L'[') c = L'.';
        if (c == L']') c = L' ';
    }
    path = Trim(path);
    IJsonValue current = root;
    size_t start = 0;
    while (start < path.size()) {
        size_t end = path.find(L'.', start);
        std::wstring token = path.substr(start, end == std::wstring::npos ? end : end - start);
        if (token.empty()) throw FetchError{Tr(L"Invalid JSON path", L"Неверный путь JSON")};
        if (current.ValueType() == JsonValueType::Object) {
            auto object = current.GetObject();
            if (!object.HasKey(token)) throw FetchError{
                std::wstring(Tr(L"JSON field not found: ", L"Поле JSON не найдено: ")) + token};
            current = object.GetNamedValue(token);
        } else if (current.ValueType() == JsonValueType::Array) {
            int index = std::stoi(token);
            auto array = current.GetArray();
            if (index < 0 || static_cast<uint32_t>(index) >= array.Size())
                throw FetchError{Tr(L"JSON array index is out of range",
                                    L"Индекс JSON вне диапазона")};
            current = array.GetAt(index);
        } else {
            throw FetchError{Tr(L"JSON path does not point to an object",
                                L"Путь JSON ведёт не в объект")};
        }
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    return current;
}

std::pair<double, std::wstring> ParseBalance(const Provider& provider,
                                             const std::string& body) {
    auto root = JsonObject::Parse(FromUtf8(body));
    if (provider.kind == ProviderKind::DeepSeek) {
        auto rows = root.GetNamedArray(L"balance_infos");
        std::optional<std::pair<double, std::wstring>> fallback;
        for (const auto& value : rows) {
            auto row = value.GetObject();
            std::wstring currency = JsonString(row, L"currency");
            auto result = std::make_pair(JsonNumber(row.GetNamedValue(L"total_balance")), currency);
            if (currency == L"USD") return result;
            if (!fallback) fallback = result;
        }
        if (fallback) return *fallback;
        throw FetchError{Tr(L"DeepSeek response contains no balance",
                            L"В ответе DeepSeek нет баланса")};
    }
    if (provider.kind == ProviderKind::Kimi) {
        if (JsonInt(root, L"code", -1) != 0 || !JsonBool(root, L"status", false))
            throw FetchError{Tr(L"Kimi did not confirm the balance request",
                                L"Kimi не подтвердил запрос баланса")};
        auto data = root.GetNamedObject(L"data");
        return {JsonNumber(data.GetNamedValue(L"available_balance")), L"USD"};
    }
    double amount = JsonNumber(JsonAt(root, provider.jsonPath));
    if (!Trim(provider.subtractPath).empty())
        amount -= JsonNumber(JsonAt(root, provider.subtractPath));
    return {amount, provider.currency};
}

void UpdateProviderResult(const Provider& result, uint64_t generation) {
    std::lock_guard lock(dataMutex);
    if (configRevision.load() != generation) return;
    auto it = std::find_if(providers.begin(), providers.end(), [&](const Provider& p) {
        return p.id == result.id;
    });
    if (it != providers.end()) {
        bool enabled = it->enabled;
        std::wstring secret = it->secret;
        *it = result;
        it->enabled = enabled;
        it->secret = secret;
        dataRevision++;
    }
}

void RefreshOne(Provider provider, uint64_t generation) {
    try {
        if (!provider.enabled) return;
        if (provider.kind == ProviderKind::Manual) {
            std::wstring number = Trim(provider.manual);
            std::replace(number.begin(), number.end(), L',', L'.');
            if (number.empty()) throw FetchError{Tr(L"Enter a manual amount",
                                                    L"Введите сумму вручную")};
            provider.value = std::stod(number);
            provider.valueCurrency = provider.currency;
        } else {
            std::wstring key = RevealSecret(provider.secret);
            if (key.empty() && provider.kind != ProviderKind::Custom)
                throw FetchError{Tr(L"Enter an API key in the settings",
                                    L"Введите API-ключ в настройках")};
            if (!IsHttps(provider.apiUrl)) throw FetchError{Tr(
                L"An HTTPS API address is required", L"Нужен HTTPS-адрес API")};
            std::string body = HttpGet(provider.apiUrl, provider, key);
            SecureZeroMemory(key.data(), key.size() * sizeof(wchar_t));
            auto parsed = ParseBalance(provider, body);
            provider.value = parsed.first;
            provider.valueCurrency = parsed.second;
        }
        provider.hasValue = true;
        provider.stale = false;
        provider.error.clear();
    } catch (const FetchError& error) {
        provider.stale = provider.hasValue;
        provider.error = error.message;
    } catch (const winrt::hresult_error&) {
        provider.stale = provider.hasValue;
        provider.error = Tr(L"Unexpected API response", L"Неожиданный ответ API");
    } catch (...) {
        provider.stale = provider.hasValue;
        provider.error = Tr(L"Could not retrieve the balance", L"Не удалось получить баланс");
    }
    UpdateProviderResult(provider, generation);
}

void RefreshAll() {
    auto snapshot = ProvidersSnapshot();
    uint64_t generation = configRevision.load();
    for (auto& provider : snapshot) {
        if (unloading) return;
        RefreshOne(provider, generation);
    }
}

DWORD WINAPI NetworkWorker(void*) {
    HANDLE events[] = {stopEvent, wakeEvent};
    while (!unloading) {
        RefreshAll();
        int minutes;
        {
            std::lock_guard lock(dataMutex);
            minutes = refreshMinutes;
        }
        DWORD timeout = static_cast<DWORD>(std::clamp(minutes, 1, 1440) * 60 * 1000);
        DWORD result = WaitForMultipleObjects(2, events, FALSE, timeout);
        if (result == WAIT_OBJECT_0) break;
    }
    return 0;
}

void StartNetwork() {
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    networkThread = CreateThread(nullptr, 0, NetworkWorker, nullptr, 0, nullptr);
}

void RefreshNow() {
    if (wakeEvent) SetEvent(wakeEvent);
}

void StopNetwork() {
    unloading = true;
    if (stopEvent) SetEvent(stopEvent);
    if (wakeEvent) SetEvent(wakeEvent);
    if (networkThread) {
        WaitForSingleObject(networkThread, INFINITE);
        CloseHandle(networkThread);
        networkThread = nullptr;
    }
    if (stopEvent) CloseHandle(stopEvent);
    if (wakeEvent) CloseHandle(wakeEvent);
    stopEvent = wakeEvent = nullptr;
}

// ---- XAML widget and taskbar integration ----

void ShowSettings();

std::mutex uiMutex;
[[clang::no_destroy]] Grid rootGrid{nullptr};
[[clang::no_destroy]] Grid widgetHost{nullptr};
[[clang::no_destroy]] StackPanel widgetRow{nullptr};
[[clang::no_destroy]] DispatcherTimer refreshTimer{nullptr};
uint64_t renderedRevision = 0;

void RemoveWidgetOnUiThread() {
    std::lock_guard lock(uiMutex);
    if (refreshTimer) {
        refreshTimer.Stop();
        refreshTimer = nullptr;
    }
    if (rootGrid && widgetHost) {
        uint32_t index = 0;
        if (rootGrid.Children().IndexOf(widgetHost, index))
            rootGrid.Children().RemoveAt(index);
    }
    widgetRow = nullptr;
    widgetHost = nullptr;
    rootGrid = nullptr;
    renderedRevision = 0;
}

void OpenWebsite(std::wstring url) {
    if (IsHttps(url)) ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void AttachSettingsGesture(const UIElement& element) {
    element.RightTapped(RightTappedEventHandler{
        [](const winrt::Windows::Foundation::IInspectable&, const RightTappedRoutedEventArgs&) {
            ShowSettings();
        }});
}

void RebuildWidgetItems() {
    auto snapshot = ProvidersSnapshot();
    widgetRow.Children().Clear();
    bool any = false;
    for (const auto& p : snapshot) {
        if (!p.enabled) continue;
        any = true;
        Button button;
        button.Content(winrt::box_value(DisplayText(p)));
        button.FontSize(11);
        button.Height(32);
        button.MinWidth(0);
        button.MinHeight(0);
        button.Padding(Thickness{7, 0, 7, 0});
        button.Margin(Thickness{0});
        button.BorderThickness(Thickness{0});
        button.Background(Media::SolidColorBrush{winrt::Windows::UI::Colors::Transparent()});
        button.VerticalContentAlignment(VerticalAlignment::Center);
        button.HorizontalContentAlignment(HorizontalAlignment::Center);
        button.IsTabStop(false);
        std::wstring tooltip = p.website;
        if (!p.error.empty()) tooltip = p.error + L"\n" + tooltip;
        ToolTipService::SetToolTip(button, winrt::box_value(tooltip));
        std::wstring url = p.website;
        button.Click(RoutedEventHandler{
            [url](const winrt::Windows::Foundation::IInspectable&, const RoutedEventArgs&) {
                OpenWebsite(url);
            }});
        AttachSettingsGesture(button);
        widgetRow.Children().Append(button);
    }
    if (!any) {
        Button button;
        button.Content(winrt::box_value(Tr(L"AI Balance — settings", L"AI Balance — настройки")));
        button.FontSize(11);
        button.Click(RoutedEventHandler{
            [](const winrt::Windows::Foundation::IInspectable&, const RoutedEventArgs&) {
                ShowSettings();
            }});
        widgetRow.Children().Append(button);
    }
}

void RefreshWidget() {
    if (unloading) return;
    std::lock_guard lock(uiMutex);
    if (!widgetHost) return;
    widgetHost.Margin(Thickness{static_cast<double>(leftOffset.load()), 0, 0, 0});
    uint64_t revision = dataRevision.load();
    if (revision == renderedRevision) return;
    renderedRevision = revision;
    RebuildWidgetItems();
    if (widgetRow) {
        widgetRow.Measure(winrt::Windows::Foundation::Size{
            std::numeric_limits<float>::infinity(), 40});
        double desired = widgetRow.DesiredSize().Width;
        if (desired > 0) widgetHost.Width(desired);
    }
}

FrameworkElement FindDirectChildByName(FrameworkElement parent, const wchar_t* name) {
    if (!parent) return nullptr;
    int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto element = Media::VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (element && element.Name() == name) return element;
    }
    return nullptr;
}

FrameworkElement FindChildRecursive(FrameworkElement element,
                                    const std::function<bool(FrameworkElement)>& predicate) {
    if (!element) return nullptr;
    if (predicate(element)) return element;
    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (auto result = FindChildRecursive(child, predicate)) return result;
    }
    return nullptr;
}

void InjectWidget(FrameworkElement taskbarFrame) {
    if (unloading || !taskbarFrame) return;
    {
    std::lock_guard lock(uiMutex);
    auto root = FindDirectChildByName(taskbarFrame, L"RootGrid").try_as<Grid>();
    if (!root) {
        Wh_Log(L"RootGrid not found");
        return;
    }
    if (widgetHost) {
        uint32_t existingIndex = 0;
        if (rootGrid == root && root.Children().IndexOf(widgetHost, existingIndex))
            return;
        if (refreshTimer) {
            refreshTimer.Stop();
            refreshTimer = nullptr;
        }
        if (rootGrid && rootGrid.Children().IndexOf(widgetHost, existingIndex))
            rootGrid.Children().RemoveAt(existingIndex);
        widgetHost = nullptr;
        widgetRow = nullptr;
        rootGrid = nullptr;
        renderedRevision = 0;
    }

    Grid host;
    host.Name(kWidgetName);
    host.HorizontalAlignment(HorizontalAlignment::Left);
    host.VerticalAlignment(VerticalAlignment::Center);
    host.Height(40);
    host.IsHitTestVisible(true);
    Grid::SetColumn(host, 0);
    Grid::SetColumnSpan(host, std::max(1, static_cast<int>(root.ColumnDefinitions().Size())));
    Canvas::SetZIndex(host, 10000);

    StackPanel row;
    row.Orientation(Orientation::Horizontal);
    row.VerticalAlignment(VerticalAlignment::Center);
    host.Children().Append(row);
    ToolTipService::SetToolTip(host, winrt::box_value(Tr(
        L"AI Balance\nRight-click — settings\nDouble-click — refresh",
        L"AI Balance\nПравая кнопка — настройки\nДвойной щелчок — обновить")));
    AttachSettingsGesture(host);
    host.DoubleTapped(DoubleTappedEventHandler{
        [](const winrt::Windows::Foundation::IInspectable&, const DoubleTappedRoutedEventArgs&) {
            RefreshNow();
        }});

    root.Children().Append(host);
    rootGrid = root;
    widgetHost = host;
    widgetRow = row;
    refreshTimer = DispatcherTimer{};
    refreshTimer.Interval(winrt::Windows::Foundation::TimeSpan{20'000'000});
    refreshTimer.Tick([](const auto&, const auto&) {
        try { RefreshWidget(); } catch (...) {
            Wh_Log(L"Widget refresh failed: %08X", static_cast<unsigned>(winrt::to_hresult()));
        }
    });
    refreshTimer.Start();
    }
    RefreshWidget();
    Wh_Log(L"AI Balance widget injected on the left");
}

// Run a callback on the native window's UI thread without a helper process.
using RunFromWindowThreadProc = void (*)(void*);
struct WindowThreadCallbackContext {
    RunFromWindowThreadProc callback;
    void* context;
    std::atomic<bool> invoked{false};
};
std::mutex callbackRegistryMutex;
std::optional<std::unordered_map<ULONG_PTR, WindowThreadCallbackContext*>> callbackRegistry{
    std::in_place};
std::atomic<ULONG_PTR> nextCallbackToken{1};

bool RunFromWindowThread(HWND window, RunFromWindowThreadProc callback, void* context) {
    static const UINT message = RegisterWindowMessageW(L"Windhawk_AIBalance_RunFromWindowThread");
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) return false;
    if (threadId == GetCurrentThreadId()) {
        callback(context);
        return true;
    }
    WindowThreadCallbackContext callbackContext{callback, context};
    ULONG_PTR token = 0;
    {
        std::lock_guard lock(callbackRegistryMutex);
        if (!callbackRegistry) return false;
        do {
            token = nextCallbackToken.fetch_add(1);
        } while (!token || callbackRegistry->contains(token));
        callbackRegistry->emplace(token, &callbackContext);
    }
    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const auto* data = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (data->message == message) {
                    WindowThreadCallbackContext* current = nullptr;
                    {
                        std::lock_guard lock(callbackRegistryMutex);
                        if (callbackRegistry) {
                            auto it = callbackRegistry->find(static_cast<ULONG_PTR>(data->lParam));
                            if (it != callbackRegistry->end()) {
                                current = it->second;
                                callbackRegistry->erase(it);
                            }
                        }
                    }
                    if (current) {
                        current->callback(current->context);
                        current->invoked.store(true, std::memory_order_release);
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        std::lock_guard lock(callbackRegistryMutex);
        callbackRegistry->erase(token);
        return false;
    }
    SendMessageW(window, message, 0, static_cast<LPARAM>(token));
    bool unhooked = false;
    for (int i = 0; i < 3 && !unhooked; i++) {
        unhooked = UnhookWindowsHookEx(hook) != FALSE;
        if (!unhooked) Sleep(10);
    }
    {
        std::lock_guard lock(callbackRegistryMutex);
        if (callbackRegistry) callbackRegistry->erase(token);
    }
    return unhooked && callbackContext.invoked.load(std::memory_order_acquire);
}

bool IsCurrentProcessTaskbarWindow(HWND window) {
    if (!window) return false;
    wchar_t className[64]{};
    DWORD processId = 0;
    return GetWindowThreadProcessId(window, &processId) &&
           processId == GetCurrentProcessId() && GetClassNameW(window, className, 64) &&
           _wcsicmp(className, L"Shell_TrayWnd") == 0;
}

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
using RefCountBase_Decref_t = void(WINAPI*)(void*);
RefCountBase_Decref_t RefCountBase_Decref_Original = nullptr;
void* CTaskBand_ITaskListWndSite_vftable = nullptr;

XamlRoot GetTaskbarXamlRoot(HWND taskbarWindow) {
    if (!IsCurrentProcessTaskbarWindow(taskbarWindow) ||
        !TaskbarHost_FrameHeight_Original || !RefCountBase_Decref_Original ||
        !CTaskBand_GetTaskbarHost_Original || !CTaskBand_ITaskListWndSite_vftable)
        return nullptr;
    HWND taskBandWindow = reinterpret_cast<HWND>(GetPropW(taskbarWindow, L"TaskbandHWND"));
    if (!taskBandWindow) return nullptr;
    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskBandWindow, 0));
    if (!taskBand) return nullptr;
    void* taskBandForSite = taskBand;
    for (int i = 0;; i++) {
        if (*reinterpret_cast<void**>(taskBandForSite) == CTaskBand_ITaskListWndSite_vftable) break;
        if (i == 20) return nullptr;
        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
    }
    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);
    struct Guard {
        void* reference;
        ~Guard() { if (reference) RefCountBase_Decref_Original(reference); }
    } guard{taskbarHostSharedPtr[1]};
    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) return nullptr;

#if defined(_M_X64)
    const BYTE* code = reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    size_t elementOffset = 0;
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 && code[7] <= 0x7F) {
        elementOffset = code[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern");
        return nullptr;
    }
#elif defined(_M_ARM64)
    const DWORD* code = reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    size_t elementOffset = 0;
    if (code[0] == 0xD503237F &&
        (code[1] & 0xFFC07FFF) == 0xA9807BFD &&
        code[2] == 0x910003FD &&
        (code[3] & 0xFFF00FE0) == 0xF8400C00) {
        elementOffset = (code[3] >> 12) & 0xFF;
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern");
        return nullptr;
    }
#else
#error "Unsupported architecture"
#endif
    auto* elementAddress = static_cast<BYTE*>(taskbarHostSharedPtr[0]) + elementOffset;
    auto* unknown = *reinterpret_cast<IUnknown**>(elementAddress);
    if (!unknown) return nullptr;
    FrameworkElement element = nullptr;
    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    return element ? element.XamlRoot() : nullptr;
}

FrameworkElement FindTaskbarFrame(HWND taskbarWindow) {
    auto xamlRoot = GetTaskbarXamlRoot(taskbarWindow);
    if (!xamlRoot) return nullptr;
    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    return FindChildRecursive(content, [](FrameworkElement child) {
        return winrt::get_class_name(child) == L"Taskbar.TaskbarFrame";
    });
}

void InjectIntoCurrentTaskbar(void* context) {
    HWND window = static_cast<HWND>(context);
    if (auto frame = FindTaskbarFrame(window)) InjectWidget(frame);
}

bool ApplyOnTaskbarThread() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    return taskbar && IsCurrentProcessTaskbarWindow(taskbar) &&
           RunFromWindowThread(taskbar, InjectIntoCurrentTaskbar, taskbar);
}

[[clang::no_destroy]] std::mutex loadedRevokersMutex;
[[clang::no_destroy]] std::optional<std::vector<FrameworkElement::Loaded_revoker>>
    loadedRevokers{std::in_place};

void RemoveFromCurrentTaskbar(void*) {
    {
        std::lock_guard lock(loadedRevokersMutex);
        loadedRevokers.reset();
    }
    RemoveWidgetOnUiThread();
}

bool RemoveOnTaskbarThread() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    return taskbar && RunFromWindowThread(taskbar, RemoveFromCurrentTaskbar, nullptr);
}

using TaskbarFrameConstructor = void*(WINAPI*)(void*);
TaskbarFrameConstructor TaskbarFrameConstructorOriginal = nullptr;

void* WINAPI TaskbarFrameConstructorHook(void* self) {
    void* result = TaskbarFrameConstructorOriginal(self);
    if (unloading) return result;
    try {
        FrameworkElement frame = nullptr;
        reinterpret_cast<IUnknown**>(self)[1]->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(frame));
        if (!frame) return result;
        auto revoker = frame.Loaded(winrt::auto_revoke,
            [](const winrt::Windows::Foundation::IInspectable& sender,
               const RoutedEventArgs&) {
                if (unloading) return;
                try {
                    auto frame = sender.try_as<FrameworkElement>();
                    HWND primaryTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
                    if (!frame || !primaryTaskbar) return;
                    XamlRoot primaryRoot = GetTaskbarXamlRoot(primaryTaskbar);
                    if (primaryRoot && frame.XamlRoot() == primaryRoot)
                        InjectWidget(frame);
                } catch (...) {
                    Wh_Log(L"Injection failed: %08X",
                           static_cast<unsigned>(winrt::to_hresult()));
                }
            });
        std::lock_guard lock(loadedRevokersMutex);
        if (loadedRevokers) loadedRevokers->push_back(std::move(revoker));
    } catch (...) {
        Wh_Log(L"TaskbarFrame Loaded hook failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
    return result;
}

bool HookViewModule(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {{
        {LR"(public: __cdecl winrt::Taskbar::implementation::TaskbarFrame::TaskbarFrame(void))"},
        &TaskbarFrameConstructorOriginal,
        TaskbarFrameConstructorHook,
    }};
    return WindhawkUtils::HookSymbols(module, taskbarViewHooks,
                                      std::size(taskbarViewHooks));
}

HMODULE ViewModule() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    return module ? module : GetModuleHandleW(L"ExplorerExtensions.dll");
}

std::atomic<bool> viewHookAttempted = false;
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExWOriginal = nullptr;

HMODULE WINAPI LoadLibraryExWHook(LPCWSTR file, HANDLE handle, DWORD flags) {
    HMODULE module = LoadLibraryExWOriginal(file, handle, flags);
    if (module && !viewHookAttempted && ViewModule() == module) {
        viewHookAttempted = true;
        if (HookViewModule(module)) Wh_ApplyHookOperations();
    }
    return module;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &RefCountBase_Decref_Original},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks, std::size(taskbarDllHooks));
}

void RevokeLoadedHandlers() {
    std::lock_guard lock(loadedRevokersMutex);
    loadedRevokers.reset();
}

// ---- Built-in settings window; no companion process ----

enum ControlId : int {
    ID_LIST = 100, ID_ADD, ID_REMOVE, ID_UP, ID_DOWN, ID_ENABLED, ID_NAME,
    ID_KIND, ID_WEBSITE, ID_APIURL, ID_KEY, ID_JSONPATH, ID_SUBTRACT,
    ID_CURRENCY, ID_HEADER, ID_PREFIX, ID_MANUAL, ID_REFRESH, ID_SAVE, ID_CANCEL
};

HWND edits[256]{};
std::vector<Provider> draft;
int selectedProvider = -1;
std::mutex settingsThreadMutex;
HANDLE settingsThreadHandle = nullptr;
std::atomic<HMODULE> settingsModuleHandle{nullptr};
std::atomic<bool> settingsClassRegistered{false};
UINT settingsDpi = 96;
HFONT settingsFont = nullptr;

int ScaleDpi(int value) { return MulDiv(value, static_cast<int>(settingsDpi), 96); }

std::wstring WindowText(HWND window) {
    int length = GetWindowTextLengthW(window);
    std::wstring text(length, L'\0');
    GetWindowTextW(window, text.data(), length + 1);
    return text;
}

HWND AddControl(HWND parent, const wchar_t* className, const wchar_t* text,
                DWORD style, int x, int y, int width, int height, int id = 0) {
    HWND control = CreateWindowExW(0, className, text, WS_CHILD | WS_VISIBLE | style,
                                   ScaleDpi(x), ScaleDpi(y), ScaleDpi(width), ScaleDpi(height),
                                   parent,
                                   reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                                   nullptr, nullptr);
    if (wcscmp(className, WC_EDITW) == 0 || wcscmp(className, WC_COMBOBOXW) == 0) {
        SetWindowLongPtrW(control, GWL_EXSTYLE,
                          GetWindowLongPtrW(control, GWL_EXSTYLE) | WS_EX_CLIENTEDGE);
    }
    SendMessageW(control, WM_SETFONT,
                 reinterpret_cast<WPARAM>(settingsFont ? settingsFont : GetStockObject(DEFAULT_GUI_FONT)), TRUE);
    if (id) edits[id] = control;
    return control;
}

void AddLabel(HWND parent, const wchar_t* text, int y) {
    AddControl(parent, WC_STATICW, text, SS_LEFT, 230, y + 4, 145, 24);
}

void SetEdit(int id, const std::wstring& text) {
    SetWindowTextW(edits[id], text.c_str());
}

void LoadSelected() {
    if (selectedProvider < 0 || selectedProvider >= static_cast<int>(draft.size())) return;
    const auto& p = draft[selectedProvider];
    SendMessageW(edits[ID_ENABLED], BM_SETCHECK, p.enabled ? BST_CHECKED : BST_UNCHECKED, 0);
    SetEdit(ID_NAME, p.name);
    SendMessageW(edits[ID_KIND], CB_SETCURSEL, static_cast<int>(PresetIndexOf(p)), 0);
    SetEdit(ID_WEBSITE, p.website);
    SetEdit(ID_APIURL, p.apiUrl);
    SetEdit(ID_KEY, RevealSecret(p.secret));
    SetEdit(ID_JSONPATH, p.jsonPath);
    SetEdit(ID_SUBTRACT, p.subtractPath);
    SetEdit(ID_CURRENCY, p.currency);
    SetEdit(ID_HEADER, p.authHeader);
    SetEdit(ID_PREFIX, p.authPrefix);
    SetEdit(ID_MANUAL, p.manual);
}

void FlushSelected() {
    if (selectedProvider < 0 || selectedProvider >= static_cast<int>(draft.size())) return;
    auto& p = draft[selectedProvider];
    p.enabled = SendMessageW(edits[ID_ENABLED], BM_GETCHECK, 0, 0) == BST_CHECKED;
    p.name = Trim(WindowText(edits[ID_NAME]));
    int presetIndex = static_cast<int>(SendMessageW(edits[ID_KIND], CB_GETCURSEL, 0, 0));
    if (presetIndex >= 0 && presetIndex < static_cast<int>(std::size(kPresetDefinitions))) {
        p.preset = kPresetDefinitions[presetIndex].id;
        p.kind = kPresetDefinitions[presetIndex].kind;
    }
    p.website = Trim(WindowText(edits[ID_WEBSITE]));
    p.apiUrl = Trim(WindowText(edits[ID_APIURL]));
    p.secret = ProtectSecret(Trim(WindowText(edits[ID_KEY])));
    p.jsonPath = Trim(WindowText(edits[ID_JSONPATH]));
    p.subtractPath = Trim(WindowText(edits[ID_SUBTRACT]));
    p.currency = Trim(WindowText(edits[ID_CURRENCY]));
    p.authHeader = Trim(WindowText(edits[ID_HEADER]));
    p.authPrefix = Trim(WindowText(edits[ID_PREFIX]));
    p.manual = Trim(WindowText(edits[ID_MANUAL]));
}

void RefreshProviderList(int select) {
    SendMessageW(edits[ID_LIST], LB_RESETCONTENT, 0, 0);
    for (const auto& p : draft) {
        std::wstring name = (p.enabled ? L"● " : L"○ ") + p.name;
        SendMessageW(edits[ID_LIST], LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(name.c_str()));
    }
    selectedProvider = draft.empty() ? -1 : std::clamp(select, 0, static_cast<int>(draft.size()) - 1);
    if (selectedProvider >= 0) {
        SendMessageW(edits[ID_LIST], LB_SETCURSEL, selectedProvider, 0);
        LoadSelected();
    }
}

void ChooseKind(int index) {
    if (index < 0 || index >= static_cast<int>(draft.size())) return;
    int presetIndex = static_cast<int>(SendMessageW(edits[ID_KIND], CB_GETCURSEL, 0, 0));
    if (presetIndex < 0) return;
    Provider old = draft[index];
    Provider p = PresetByIndex(static_cast<size_t>(presetIndex));
    p.id = old.id;
    p.enabled = old.enabled;
    p.manual = old.manual;
    draft[index] = std::move(p);
    LoadSelected();
}

void SaveSettings(HWND window) {
    FlushSelected();
    for (const auto& p : draft) {
        if (p.name.empty() || !IsHttps(p.website)) {
            MessageBoxW(window, Tr(L"Check the service name and HTTPS website address.",
                                      L"Проверьте название и HTTPS-адрес сайта каждого сервиса."),
                        L"AI Balance", MB_ICONWARNING);
            return;
        }
        if (p.kind != ProviderKind::Manual && !IsHttps(p.apiUrl)) {
            MessageBoxW(window, Tr(L"Check the HTTPS API address.",
                                      L"Проверьте HTTPS-адрес API."), L"AI Balance", MB_ICONWARNING);
            return;
        }
        if (p.kind == ProviderKind::Custom && p.jsonPath.empty()) {
            MessageBoxW(window, Tr(L"Specify the JSON value path.",
                                      L"Укажите путь к сумме в JSON."), L"AI Balance", MB_ICONWARNING);
            return;
        }
        if (!p.secret.empty() && RevealSecret(p.secret).empty() && !WindowText(edits[ID_KEY]).empty()) {
            MessageBoxW(window, Tr(L"Could not encrypt the API key.",
                                      L"Не удалось зашифровать API-ключ."), L"AI Balance", MB_ICONERROR);
            return;
        }
    }
    int minutes = 5;
    try { minutes = std::clamp(std::stoi(WindowText(edits[ID_REFRESH])), 1, 1440); }
    catch (...) {
        MessageBoxW(window, Tr(L"The refresh interval must be a number of minutes.",
                                  L"Интервал должен быть числом минут."), L"AI Balance", MB_ICONWARNING);
        return;
    }
    if (!SaveProviders(draft, minutes)) {
        MessageBoxW(window, Tr(L"Could not save the settings.",
                                  L"Не удалось сохранить настройки."), L"AI Balance", MB_ICONERROR);
        return;
    }
    DestroyWindow(window);
}

LRESULT CALLBACK SettingsProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            settingsWindow = window;
            settingsDpi = GetDpiForWindow(window);
            if (!settingsDpi) settingsDpi = 96;
            settingsFont = CreateFontW(-ScaleDpi(9), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                       CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
            draft = ProvidersSnapshot();
            AddControl(window, WC_LISTBOXW, nullptr, WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                       15, 15, 195, 420, ID_LIST);
            AddControl(window, WC_BUTTONW, Tr(L"Add", L"Добавить"), BS_PUSHBUTTON, 15, 450, 95, 30, ID_ADD);
            AddControl(window, WC_BUTTONW, Tr(L"Delete", L"Удалить"), BS_PUSHBUTTON, 115, 450, 95, 30, ID_REMOVE);
            AddControl(window, WC_BUTTONW, Tr(L"Up", L"Вверх"), BS_PUSHBUTTON, 15, 490, 95, 30, ID_UP);
            AddControl(window, WC_BUTTONW, Tr(L"Down", L"Вниз"), BS_PUSHBUTTON, 115, 490, 95, 30, ID_DOWN);
            AddControl(window, WC_BUTTONW, Tr(L"Show on taskbar", L"Показывать на панели"), BS_AUTOCHECKBOX,
                       230, 15, 400, 26, ID_ENABLED);
            const wchar_t* labelsEn[] = {L"Name", L"Service type (preset)", L"Website on click",
                L"HTTPS balance API", L"API key (paste from dashboard)", L"Balance JSON field",
                L"Subtract JSON field (optional)", L"Currency", L"Auth header",
                L"Auth prefix", L"Manual amount"};
            const wchar_t* labelsRu[] = {L"Название", L"Тип сервиса (пресет)", L"Сайт по нажатию",
                L"HTTPS API баланса", L"API-ключ (вставить из кабинета)", L"Поле суммы JSON",
                L"Вычесть поле JSON (необязательно)", L"Валюта", L"Заголовок ключа",
                L"Префикс ключа", L"Сумма для режима «Вручную»"};
            const wchar_t** labels = languageCode.load() == 1 ? labelsRu : labelsEn;
            const int ids[] = {ID_NAME, ID_KIND, ID_WEBSITE, ID_APIURL, ID_KEY, ID_JSONPATH,
                ID_SUBTRACT, ID_CURRENCY, ID_HEADER, ID_PREFIX, ID_MANUAL};
            for (int i = 0; i < 11; i++) {
                int y = 55 + i * 36;
                AddLabel(window, labels[i], y);
                if (ids[i] == ID_KIND) {
                    HWND combo = AddControl(window, WC_COMBOBOXW, nullptr,
                        CBS_DROPDOWNLIST | WS_VSCROLL, 385, y, 425, 220, ID_KIND);
                    for (const auto& preset : kPresetDefinitions)
                        SendMessageW(combo, CB_ADDSTRING, 0,
                                     reinterpret_cast<LPARAM>(PresetName(preset)));
                } else {
                    AddControl(window, WC_EDITW, nullptr,
                        ES_AUTOHSCROLL | WS_TABSTOP | (ids[i] == ID_KEY ? ES_PASSWORD : 0),
                        385, y, 425, 27, ids[i]);
                }
            }
            AddControl(window, WC_STATICW, Tr(
                L"API keys are encrypted with Windows DPAPI. Click a balance to open its website.",
                L"Ключи шифруются Windows DPAPI. Щелчок по балансу открывает сайт."),
                       SS_LEFT, 230, 458, 580, 36);
            AddLabel(window, Tr(L"Refresh, minutes", L"Обновление, минут"), 505);
            AddControl(window, WC_EDITW, L"5", ES_NUMBER | WS_TABSTOP, 385, 505, 90, 27, ID_REFRESH);
            AddControl(window, WC_BUTTONW, Tr(L"Save", L"Сохранить"), BS_DEFPUSHBUTTON, 555, 550, 120, 34, ID_SAVE);
            AddControl(window, WC_BUTTONW, Tr(L"Cancel", L"Отмена"), BS_PUSHBUTTON, 690, 550, 120, 34, ID_CANCEL);
            int minutes;
            {
                std::lock_guard lock(dataMutex);
                minutes = refreshMinutes;
            }
            SetEdit(ID_REFRESH, std::to_wstring(minutes));
            RefreshProviderList(0);
            return 0;
        }
        case WM_COMMAND: {
            int id = LOWORD(wParam), code = HIWORD(wParam);
            if (id == ID_LIST && code == LBN_SELCHANGE) {
                FlushSelected();
                selectedProvider = static_cast<int>(SendMessageW(edits[ID_LIST], LB_GETCURSEL, 0, 0));
                LoadSelected();
            } else if (id == ID_KIND && code == CBN_SELCHANGE) {
                ChooseKind(selectedProvider);
            } else if (id == ID_ADD && code == BN_CLICKED) {
                FlushSelected();
                draft.push_back(PresetByIndex(11));
                RefreshProviderList(static_cast<int>(draft.size()) - 1);
            } else if (id == ID_REMOVE && code == BN_CLICKED && selectedProvider >= 0) {
                draft.erase(draft.begin() + selectedProvider);
                RefreshProviderList(std::min(selectedProvider, static_cast<int>(draft.size()) - 1));
            } else if ((id == ID_UP || id == ID_DOWN) && code == BN_CLICKED) {
                int next = selectedProvider + (id == ID_UP ? -1 : 1);
                if (selectedProvider >= 0 && next >= 0 && next < static_cast<int>(draft.size())) {
                    FlushSelected();
                    std::swap(draft[selectedProvider], draft[next]);
                    RefreshProviderList(next);
                }
            } else if (id == ID_SAVE && code == BN_CLICKED) {
                SaveSettings(window);
            } else if (id == ID_CANCEL && code == BN_CLICKED) {
                DestroyWindow(window);
            }
            return 0;
        }
        case WM_CLOSE:
            DestroyWindow(window);
            return 0;
        case WM_DPICHANGED: {
            settingsDpi = HIWORD(wParam);
            RECT* suggested = reinterpret_cast<RECT*>(lParam);
            SetWindowPos(window, nullptr, suggested->left, suggested->top,
                         suggested->right - suggested->left,
                         suggested->bottom - suggested->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            return 0;
        }
        case WM_DESTROY:
            settingsWindow = nullptr;
            std::fill(std::begin(edits), std::end(edits), nullptr);
            selectedProvider = -1;
            if (settingsFont) {
                DeleteObject(settingsFont);
                settingsFont = nullptr;
            }
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

DWORD WINAPI SettingsThread(void*) {
    if (unloading) return 0;
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&SettingsProc), &module))
        return 0;
    WNDCLASSW wc{};
    wc.lpfnWndProc = SettingsProc;
    wc.hInstance = module;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = kSettingsClass;
    WNDCLASSEXW existing{};
    existing.cbSize = sizeof(existing);
    if (!GetClassInfoExW(module, kSettingsClass, &existing)) {
        if (!RegisterClassW(&wc)) return 0;
        settingsClassRegistered = true;
    }
    settingsModuleHandle = module;
    settingsDpi = GetDpiForSystem();
    if (!settingsDpi) settingsDpi = 96;
    if (unloading) return 0;
    HWND window = CreateWindowExW(WS_EX_DLGMODALFRAME, kSettingsClass,
        Tr(L"AI Balance — settings", L"AI Balance — настройки"),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, ScaleDpi(840), ScaleDpi(640),
        nullptr, nullptr, module, nullptr);
    if (!window) return 0;
    if (unloading) {
        DestroyWindow(window);
        return 0;
    }
    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageW(window, &message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    return 0;
}

void CloseSettingsWindowAndWait() {
    HWND window = settingsWindow.load();
    if (window) PostMessageW(window, WM_CLOSE, 0, 0);
    HANDLE thread = nullptr;
    {
        std::lock_guard lock(settingsThreadMutex);
        thread = settingsThreadHandle;
        settingsThreadHandle = nullptr;
    }
    if (thread) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

void ShowSettings() {
    if (unloading) return;
    HWND existing = settingsWindow.load();
    if (existing && IsWindow(existing)) {
        ShowWindow(existing, SW_RESTORE);
        SetForegroundWindow(existing);
        return;
    }
    std::lock_guard lock(settingsThreadMutex);
    if (unloading) return;
    if (settingsThreadHandle) {
        if (WaitForSingleObject(settingsThreadHandle, 0) == WAIT_TIMEOUT) return;
        CloseHandle(settingsThreadHandle);
        settingsThreadHandle = nullptr;
    }
    settingsThreadHandle = CreateThread(nullptr, 0, SettingsThread, nullptr, 0, nullptr);
}

}  // namespace

BOOL Wh_ModInit() {
    unloading = false;
    LoadLanguage();
    leftOffset = std::max(0, Wh_GetIntSetting(L"leftOffset"));
    LoadProviders();
    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"taskbar.dll symbols unavailable");
        return FALSE;
    }
    if (HMODULE module = ViewModule()) {
        viewHookAttempted = true;
        if (!HookViewModule(module)) {
            Wh_Log(L"Taskbar.View symbols unavailable");
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        auto loadLibrary = kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
            GetProcAddress(kernelBase, "LoadLibraryExW")) : nullptr;
        if (!loadLibrary || !WindhawkUtils::SetFunctionHook(
                loadLibrary, LoadLibraryExWHook, &LoadLibraryExWOriginal))
            return FALSE;
    }
    StartNetwork();
    return TRUE;
}

void Wh_ModAfterInit() {
    ApplyOnTaskbarThread();
}

void Wh_ModSettingsChanged() {
    LoadLanguage();
    leftOffset = std::max(0, Wh_GetIntSetting(L"leftOffset"));
}

void Wh_ModBeforeUninit() {
    unloading = true;
    RemoveOnTaskbarThread();
    RevokeLoadedHandlers();
    CloseSettingsWindowAndWait();
    StopNetwork();
}

void Wh_ModUninit() {
    StopNetwork();
    RevokeLoadedHandlers();
    if (settingsClassRegistered.exchange(false)) {
        HMODULE module = settingsModuleHandle.exchange(nullptr);
        if (module) UnregisterClassW(kSettingsClass, module);
    }
    std::lock_guard lock(callbackRegistryMutex);
    callbackRegistry.reset();
}
