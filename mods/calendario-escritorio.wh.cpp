// ==WindhawkMod==
// @id              calendario-escritorio
// @name            Calendario de escritorio
// @description     Calendarios y eventos en tu escritorio
// @version         1.0
// @author          SharkIT-sys
// @github          https://github.com/SharkIT-sys
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32 -lwinhttp
// ==/WindhawkMod==

/*
MIT License

Copyright (c) 2026 SharkIT-sys

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ==WindhawkModReadme==
/*
# Calendario de escritorio

Añade tus suscripciones iCalendar en Configuración. Admite enlaces HTTPS y
webcal. Google ofrece una dirección secreta en formato iCal. En iCloud el
enlace se obtiene activando Calendario público. Cualquiera con ese enlace
de iCloud puede leer sus eventos. Conserva los enlaces de forma privada.

Pulsa un día para ver la agenda. Usa la rueda sobre la agenda para recorrer
los eventos y el botón de actualización para volver a consultar los enlaces.
Arrastra desde el título para mover el panel durante la sesión.

Cambia de vista con las pestañas Compacta, Mes, Semana, Día y Año. Elige
la vista inicial y ajusta el ancho y la altura en Configuración.

Las suscripciones son de solo lectura. Consulta el [README del proyecto](https://github.com/SharkIT-sys/calendar_desktop#readme)
para conocer los límites de esta versión.

En Festivos puedes introducir el código INE de tu municipio o un enlace
iCalendar de festivos. Los días festivos y los domingos aparecen en rojo.
Datos municipales de [festivos.io](https://festivos.io/api) (CC BY 4.0).

El código del mod se distribuye bajo la [licencia MIT](https://github.com/SharkIT-sys/calendar_desktop/blob/main/LICENSE).

## Capturas de pantalla

### Vista compacta

Mes con indicadores de eventos y la agenda del día seleccionado.

![Vista compacta](https://raw.githubusercontent.com/SharkIT-sys/calendar_desktop/main/docs/images/vista-compacta.png)

### Vista mensual

Eventos en etiquetas de color y contadores para las fechas con más citas.

![Vista mensual](https://raw.githubusercontent.com/SharkIT-sys/calendar_desktop/main/docs/images/vista-mes.png)

### Vista semanal

Siete columnas con los eventos de la semana y la agenda de la fecha seleccionada.

![Vista semanal](https://raw.githubusercontent.com/SharkIT-sys/calendar_desktop/main/docs/images/vista-semana.png)

### Vista diaria

Agenda del día con el nombre del festivo cuando corresponde.

![Vista diaria](https://raw.githubusercontent.com/SharkIT-sys/calendar_desktop/main/docs/images/vista-dia.png)

### Vista anual

Los doce meses en un solo panel, con festivos y fechas señaladas.

![Vista anual](https://raw.githubusercontent.com/SharkIT-sys/calendar_desktop/main/docs/images/vista-ano.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- opacity: 94
  $name: Opacidad del panel (%)
- opacityMode: background
  $name: Aplicar transparencia
  $options:
    - background: Solo al fondo
    - total: A todo el widget
- width: 390
  $name: Ancho del panel (px a 96 DPI)
- height: 454
  $name: Altura del panel (px a 96 DPI)
- showClock: true
  $name: Mostrar reloj en la cabecera
  $description: Sustituye el título de la vista por la hora local y la fecha abreviada.
- initialView: compact
  $name: Vista inicial
  $options:
    - compact: Mensual compacta
    - month: Mensual con eventos
    - week: Semanal
    - day: Diaria
    - year: Anual
- background: "#171D29"
  $name: Color de fondo
- text: "#EEF4FF"
  $name: Color del texto
- accent: "#82D9CD"
  $name: Color de selección
- refresh: 15
  $name: Intervalo de actualización (minutos)
- holidays:
    - municipality: ""
      $name: Código INE del municipio (España)
      $description: Cinco dígitos. Por ejemplo, 28079 para Madrid. Vacío para desactivar.
    - url: ""
      $name: Enlace alternativo de festivos
      $description: Una URL HTTPS o webcal sustituye la fuente municipal.
    - color: "#FF7575"
      $name: Color de festivos y domingos
    - markSundays: true
      $name: Resaltar los domingos
  $name: Festivos
- sources:
    - - name: "Mi calendario"
        $name: Nombre
      - url: ""
        $name: Enlace HTTPS o webcal
      - color: "#82D9CD"
        $name: Color de eventos
  $name: Calendarios
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <winhttp.h>
#include <algorithm>
#include <atomic>
#include <array>
#include <chrono>
#include <climits>
#include <cstdio>
#include <cwchar>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace {
using namespace std::chrono;
using namespace std::chrono_literals;

struct Source {
    std::wstring name, url;
    COLORREF color;
    bool holiday = false;
    int holidayYear = 0;
};
enum class Mode { Compact, Month, Week, Day, Year };
struct Settings {
    int opacity, width, height, refresh;
    Mode initialView;
    COLORREF background, text, accent;
    std::vector<Source> sources;
    std::wstring municipality, holidayUrl;
    COLORREF holidayColor;
    bool transparentBackground;
    bool markSundays;
    bool showClock;
};
struct Event {
    sys_days start, end;
    int minute;
    std::wstring title;
};
enum ImportIssue : unsigned {
    UnsupportedRule = 1,
    UnsupportedDate = 2,
    UnsupportedException = 4,
    UnsupportedDuration = 8,
};

struct Feed {
    std::wstring url;
    std::vector<Event> events;
    bool failed = false;
    unsigned issues = 0;
};

constexpr UINT SettingsMessage = WM_APP + 1;
constexpr UINT RefreshMessage = WM_APP + 2;
constexpr size_t MaxEvents = 25000;
constexpr wchar_t WindowClass[] = L"Windhawk.CalendarioEscritorio";
HANDLE stopEvent, wakeEvent, readyEvent, uiThread, fetchThread;
std::atomic<HWND> widget;
std::atomic<bool> refreshing;
std::mutex dataMutex;
Settings pending;
std::vector<Feed> feeds;

std::wstring Setting(PCWSTR key) {
    PCWSTR raw = Wh_GetStringSetting(key);
    std::wstring result = raw ? raw : L"";
    if (raw) Wh_FreeStringSetting(raw);
    return result;
}

std::wstring Trim(std::wstring value) {
    auto first = value.find_first_not_of(L" \r\n\t");
    if (first == value.npos) return {};
    return value.substr(first, value.find_last_not_of(L" \r\n\t") - first + 1);
}

bool ValidMunicipality(std::wstring_view code) {
    return code.size() == 5 && code.find_first_not_of(L"0123456789") == code.npos;
}

std::vector<Source> SourcesForYear(const Settings& config, int currentYear) {
    auto sources = config.sources;
    if (!config.holidayUrl.empty()) {
        sources.push_back({L"Festivo", config.holidayUrl, config.holidayColor, true});
    } else if (ValidMunicipality(config.municipality)) {
        for (int offset : {0, 1, -1}) {
            const int year = currentYear + offset;
            auto url = L"https://festivos.io/v1/" + std::to_wstring(year) +
                L"/municipio/" + config.municipality + L".ics";
            sources.push_back({L"Festivo", std::move(url), config.holidayColor, true, year});
        }
    }
    return sources;
}

COLORREF Color(std::wstring value, COLORREF fallback) {
    if (value.starts_with(L"#")) value.erase(0, 1);
    if (value.size() != 6 || value.find_first_not_of(L"0123456789abcdefABCDEF") != value.npos)
        return fallback;
    auto rgb = std::wcstoul(value.c_str(), nullptr, 16);
    return RGB(rgb >> 16, rgb >> 8, rgb);
}

void LoadSettings() {
    const auto initial = Setting(L"initialView");
    Mode mode = Mode::Compact;
    if (initial == L"month") mode = Mode::Month;
    if (initial == L"week") mode = Mode::Week;
    if (initial == L"day") mode = Mode::Day;
    if (initial == L"year") mode = Mode::Year;
    int height = Wh_GetIntSetting(L"height");
    Settings next{
        std::clamp(Wh_GetIntSetting(L"opacity"), 0, 100),
        std::clamp(Wh_GetIntSetting(L"width"), 300, 1200),
        std::clamp(height > 0 ? height : 454, 390, 1100),
        std::clamp(Wh_GetIntSetting(L"refresh"), 5, 1440),
        mode,
        Color(Setting(L"background"), RGB(23, 29, 41)),
        Color(Setting(L"text"), RGB(238, 244, 255)),
        Color(Setting(L"accent"), RGB(130, 217, 205)),
        {},
        Trim(Setting(L"holidays.municipality")),
        Trim(Setting(L"holidays.url")),
        Color(Setting(L"holidays.color"), RGB(255, 117, 117)),
        Setting(L"opacityMode") != L"total",
        Wh_GetIntSetting(L"holidays.markSundays") != 0,
        Wh_GetIntSetting(L"showClock") != 0,
    };
    for (int index = 0; index < 16; ++index) {
        auto prefix = L"sources[" + std::to_wstring(index) + L"].";
        auto url = Trim(Setting((prefix + L"url").c_str()));
        if (url.empty()) continue;
        auto name = Setting((prefix + L"name").c_str());
        if (name.empty()) name = L"Calendario " + std::to_wstring(index + 1);
        next.sources.push_back({std::move(name), std::move(url),
            Color(Setting((prefix + L"color").c_str()), next.accent)});
    }
    std::lock_guard lock(dataMutex);
    pending = std::move(next);
}

struct InternetHandle {
    HINTERNET value;
    ~InternetHandle() { if (value) WinHttpCloseHandle(value); }
};

bool Download(std::wstring url, std::string& body) {
    if (url.starts_with(L"webcal://")) url.replace(0, 9, L"https://");
    if (!url.starts_with(L"https://") || url.size() > 8192) return false;
    url.resize(url.find(L'#') == url.npos ? url.size() : url.find(L'#'));
    URL_COMPONENTS parts{};
    parts.dwStructSize = sizeof(parts);
    parts.dwHostNameLength = parts.dwUrlPathLength = parts.dwExtraInfoLength =
        parts.dwUserNameLength = parts.dwPasswordLength = DWORD(-1);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts) ||
        parts.dwUserNameLength || parts.dwPasswordLength || !parts.dwHostNameLength)
        return false;

    InternetHandle session{WinHttpOpen(L"CalendarioEscritorio/0.1",
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, nullptr, nullptr, 0)};
    if (!session.value) return false;
    WinHttpSetTimeouts(session.value, 3000, 3000, 4000, 4000);
    std::wstring host(parts.lpszHostName, parts.dwHostNameLength);
    InternetHandle connection{WinHttpConnect(session.value, host.c_str(), parts.nPort, 0)};
    if (!connection.value) return false;
    std::wstring path = parts.dwUrlPathLength
        ? std::wstring(parts.lpszUrlPath, parts.dwUrlPathLength) : L"/";
    if (parts.dwExtraInfoLength) path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);
    InternetHandle request{WinHttpOpenRequest(connection.value, L"GET", path.c_str(),
        nullptr, nullptr, nullptr, WINHTTP_FLAG_SECURE)};
    if (!request.value) return false;
    DWORD disabled = WINHTTP_DISABLE_COOKIES | WINHTTP_DISABLE_AUTHENTICATION;
    if (!WinHttpSetOption(request.value, WINHTTP_OPTION_DISABLE_FEATURE, &disabled, sizeof(disabled)))
        return false;
    if (!WinHttpSendRequest(request.value, nullptr, 0, nullptr, 0, 0, 0) ||
        !WinHttpReceiveResponse(request.value, nullptr)) return false;
    DWORD status = 0, size = sizeof(status);
    if (!WinHttpQueryHeaders(request.value, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                             nullptr, &status, &size, nullptr) || status != 200) return false;
    const auto deadline = steady_clock::now() + 15s;
    std::array<char, 16384> buffer;
    while (WaitForSingleObject(stopEvent, 0) == WAIT_TIMEOUT && steady_clock::now() < deadline) {
        DWORD read = 0;
        if (!WinHttpReadData(request.value, buffer.data(), buffer.size(), &read)) return false;
        if (!read) return true;
        if (body.size() + read > 8 * 1024 * 1024) return false;
        body.append(buffer.data(), read);
    }
    return false;
}

sys_days Today() {
    SYSTEMTIME local;
    GetLocalTime(&local);
    return sys_days{year(local.wYear) / local.wMonth / local.wDay};
}

struct Date {
    sys_days day;
    int minute = -1;
    int second = 0;
    bool valid = false;
};

int Digits(std::string_view value) {
    if (value.empty() || value.size() > 9) return -1;
    int number = 0;
    for (char digit : value) {
        if (digit < '0' || digit > '9') return -1;
        number = number * 10 + digit - '0';
    }
    return number;
}

Date ParseDate(std::string_view value, std::string_view zone = {}) {
    if (value.size() != 8 && value.size() != 15 && value.size() != 16) return {};
    auto date = year(Digits(value.substr(0, 4))) /
        unsigned(Digits(value.substr(4, 2))) / unsigned(Digits(value.substr(6, 2)));
    if (!date.ok() || int(date.year()) < 1601 || int(date.year()) > 9999) return {};
    Date result{sys_days(date), -1, 0, true};
    if (value.size() == 8) return result;
    int hour = Digits(value.substr(9, 2)), minute = Digits(value.substr(11, 2));
    int second = Digits(value.substr(13, 2));
    if (value[8] != 'T' || hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
        second < 0 || second > 59 || (value.size() == 16 && value.back() != 'Z')) return {};
    result.minute = hour * 60 + minute;
    result.second = second;
    if (value.size() == 15 && zone.empty()) return result;
    SYSTEMTIME input{WORD(int(date.year())), WORD(unsigned(date.month())), 0,
        WORD(unsigned(date.day())), WORD(hour), WORD(minute), WORD(second), 0};
    SYSTEMTIME utc = input, local;
    if (value.size() == 15) {
        const std::pair<std::string_view, PCWSTR> zones[] = {
            {"Europe/Madrid", L"Romance Standard Time"}, {"Europe/Paris", L"Romance Standard Time"},
            {"Europe/London", L"GMT Standard Time"}, {"Atlantic/Canary", L"GMT Standard Time"},
            {"Europe/Berlin", L"W. Europe Standard Time"}, {"UTC", L"UTC"},
            {"America/New_York", L"Eastern Standard Time"},
            {"America/Los_Angeles", L"Pacific Standard Time"},
        };
        auto match = std::find_if(std::begin(zones), std::end(zones),
                                  [&](const auto& item) { return item.first == zone; });
        if (match == std::end(zones)) return {};
        DYNAMIC_TIME_ZONE_INFORMATION timezone{};
        DWORD index = 0;
        while (EnumDynamicTimeZoneInformation(index++, &timezone) == ERROR_SUCCESS) {
            if (std::wcscmp(timezone.TimeZoneKeyName, match->second) == 0) break;
            timezone.TimeZoneKeyName[0] = 0;
        }
        if (!timezone.TimeZoneKeyName[0] ||
            !TzSpecificLocalTimeToSystemTimeEx(&timezone, &input, &utc)) return {};
    }
    if (!SystemTimeToTzSpecificLocalTimeEx(nullptr, &utc, &local)) return {};
    return {sys_days(year(local.wYear) / local.wMonth / local.wDay),
            local.wHour * 60 + local.wMinute, local.wSecond, true};
}

std::wstring EventTitle(std::string_view raw) {
    std::string text;
    for (size_t index = 0; index < raw.size() && text.size() < 2048; ++index) {
        char ch = raw[index];
        if (ch == '\\' && index + 1 < raw.size()) {
            ch = raw[++index];
            if (ch == 'n' || ch == 'N') ch = ' ';
        }
        if (static_cast<unsigned char>(ch) >= 32) text += ch;
    }
    if (text.empty()) return L"Sin título";
    int size = MultiByteToWideChar(CP_UTF8, 0, text.data(), int(text.size()), nullptr, 0);
    std::wstring result(size, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), int(text.size()), result.data(), size);
    return result;
}

// Expand common rules in the source's time zone before converting each date.
template<typename Emit>
bool ExpandRule(std::string_view rule, std::string_view rawStart,
                std::string_view zone, Date start, Emit emit) {
    std::string_view frequency;
    int interval = 1, count = INT_MAX;
    Date until;
    while (!rule.empty()) {
        auto separator = rule.find(';');
        auto field = rule.substr(0, separator);
        auto equals = field.find('=');
        if (equals == field.npos) return false;
        auto name = field.substr(0, equals), value = field.substr(equals + 1);
        if (name == "FREQ") frequency = value;
        else if (name == "INTERVAL") interval = Digits(value);
        else if (name == "COUNT") count = Digits(value);
        else if (name == "UNTIL") {
            until = ParseDate(value, value.ends_with("Z") ? "" : zone);
            if (!until.valid) return false;
        } else return false;
        if (separator == rule.npos) break;
        rule.remove_prefix(separator + 1);
    }
    if (interval < 1 || interval > 10000 || count < 1) return false;
    if (frequency != "DAILY" && frequency != "WEEKLY" &&
        frequency != "MONTHLY" && frequency != "YEARLY") return false;
    auto origin = ParseDate(rawStart.substr(0, 8)).day;
    year_month_day base(origin);
    const auto today = Today();
    auto first = today - days{366}, last = today + days{731};
    int previous = 1;
    if (frequency == "DAILY" || frequency == "WEEKLY") {
        int stride = interval * (frequency == "DAILY" ? 1 : 7);
        previous = int(std::max(days::rep{1}, (first - origin).count() / stride));
    }
    const int maxIndex = previous + 100000;
    for (int index = previous; index <= maxIndex && previous < count; ++index) {
        int step = index * interval;
        sys_days day;
        if (frequency == "DAILY" || frequency == "WEEKLY") {
            int stride = frequency == "DAILY" ? 1 : 7;
            if (step > (last - origin).count() / stride || origin > last) return true;
            day = origin + days(step * stride);
        } else {
            if (step > 120000 || (frequency == "YEARLY" && step > 10000)) return true;
            auto candidate = frequency == "MONTHLY" ? base + months(step) : base + years(step);
            if (int(candidate.year()) > 9999 || sys_days(candidate.year() / candidate.month() / 1) > last)
                return true;
            if (!candidate.ok()) continue;
            day = sys_days(candidate);
        }
        ++previous;
        if (until.valid && day > until.day + days{1}) return true;
        if (day < first) continue;
        year_month_day date(day);
        char formatted[16];
        std::snprintf(formatted, sizeof(formatted), "%04d%02u%02u", int(date.year()),
                      unsigned(date.month()), unsigned(date.day()));
        auto occurrence = ParseDate(std::string(formatted) + std::string(rawStart.substr(8)), zone);
        if (!occurrence.valid) return false;
        if (until.valid && (occurrence.day > until.day ||
            (occurrence.day == until.day && occurrence.minute > until.minute && until.minute >= 0)))
            return true;
        if ((occurrence.day != start.day || occurrence.minute != start.minute) &&
            !emit(occurrence)) return false;
    }
    return previous >= count;
}

bool ParseCalendar(std::string_view body, Feed& feed) {
    if (body.starts_with("\xef\xbb\xbf")) body.remove_prefix(3);
    if (!body.starts_with("BEGIN:VCALENDAR")) return false;
    std::string line;
    Date start, end;
    std::wstring title;
    std::string rule, rawStart, startZone;
    std::vector<Date> extra, excluded;
    bool inside = false, cancelled = false, complete = false;
    unsigned issues = 0;
    int depth = 0;
    auto process = [&] {
        if (line == "END:VCALENDAR" && !inside) complete = true;
        if (line == "BEGIN:VEVENT" && !inside) {
            start = end = {};
            title = L"Sin título";
            extra.clear();
            excluded.clear();
            rule.clear();
            rawStart.clear();
            startZone.clear();
            inside = true;
            cancelled = false;
            issues = 0;
            depth = 0;
        } else if (line == "END:VEVENT" && inside && !depth) {
            inside = false;
            if (cancelled) return;
            feed.issues |= issues;
            if (!start.valid) feed.issues |= UnsupportedDate;
            if (!start.valid) return;
            auto emit = [&](Date entry) {
                if (feed.events.size() > MaxEvents) return false;
                if (std::ranges::any_of(excluded, [&](Date skip) {
                    return skip.day == entry.day && (skip.minute == -1 || skip.minute == entry.minute);
                })) return true;
                auto span = end.valid ? end.day - start.day : days{0};
                if (!end.valid || start.minute == -1 || end.minute > 0 || end.second > 0)
                    span += days{1};
                feed.events.push_back({entry.day, entry.day + std::max(span, days{1}),
                                       entry.minute, title});
                return feed.events.size() <= MaxEvents;
            };
            // DTSTART is the first occurrence, even if the rule isn't supported.
            // DTEND for all-day events is already exclusive.
            if (start.minute == -1 && end.valid) end.day -= days{1};
            emit(start);
            for (auto date : extra)
                if (!emit(date)) break;
            if (!rule.empty() && !ExpandRule(rule, rawStart, startZone, start, emit))
                feed.issues |= UnsupportedRule;
        } else if (inside) {
            if (line.starts_with("BEGIN:")) { ++depth; return; }
            if (line.starts_with("END:") && depth) { --depth; return; }
            if (depth) return;
            const auto colon = line.find(':');
            if (colon == line.npos) return;
            std::string_view key(line.data(), colon);
            std::string_view value(line.data() + colon + 1, line.size() - colon - 1);
            auto name = key.substr(0, key.find(';'));
            std::string_view zone;
            if (auto position = key.find(";TZID="); position != key.npos) {
                zone = key.substr(position + 6);
                zone = zone.substr(0, zone.find(';'));
                if (zone.size() >= 2 && zone.front() == '"' && zone.back() == '"')
                    zone = zone.substr(1, zone.size() - 2);
            }
            if (name == "DTSTART") {
                start = ParseDate(value, zone);
                rawStart = value;
                startZone = zone;
            }
            if (name == "DTEND") {
                end = ParseDate(value, zone);
                if (!end.valid) issues |= UnsupportedDate;
            }
            if (name == "SUMMARY") title = EventTitle(value);
            if (name == "STATUS") cancelled = value == "CANCELLED";
            if (name == "RRULE") rule = value;
            if (name == "RECURRENCE-ID") issues |= UnsupportedException;
            if (name == "DURATION") issues |= UnsupportedDuration;
            if (name == "RDATE" || name == "EXDATE") {
                auto& list = name == "RDATE" ? extra : excluded;
                while (!value.empty()) {
                    auto comma = value.find(',');
                    auto date = ParseDate(value.substr(0, comma), zone);
                    if (date.valid) list.push_back(date);
                    else issues |= UnsupportedDate;
                    if (comma == value.npos) break;
                    value.remove_prefix(comma + 1);
                }
            }
        }
    };
    while (!body.empty() && !complete) {
        auto newline = body.find('\n');
        auto physical = body.substr(0, newline);
        if (physical.ends_with("\r")) physical.remove_suffix(1);
        if (physical.starts_with(" ") || physical.starts_with("\t")) {
            line.append(physical.substr(1));
        } else {
            process();
            line.assign(physical);
        }
        if (feed.events.size() > MaxEvents || line.size() > 65536) return false;
        if (newline == body.npos) break;
        body.remove_prefix(newline + 1);
    }
    if (!complete) process();
    return complete && feed.events.size() <= MaxEvents;
}

DWORD WINAPI FetchLoop(void*) {
    HANDLE handles[] = {stopEvent, wakeEvent};
    DWORD delay = 0;
    for (;;) {
        DWORD signal = WaitForMultipleObjects(2, handles, FALSE, delay);
        if (signal != WAIT_TIMEOUT && signal != WAIT_OBJECT_0 + 1) break;
        Settings config;
        {
            std::lock_guard lock(dataMutex);
            config = pending;
        }
        refreshing = true;
        PostMessageW(widget.load(), RefreshMessage, 0, 0);
        const int year = int(year_month_day(Today()).year());
        const auto sources = SourcesForYear(config, year);
        for (const auto& source : sources) {
            if (WaitForSingleObject(stopEvent, 0) != WAIT_TIMEOUT) break;
            Feed result{source.url, {}};
            std::string body;
            bool ok = Download(source.url, body) && ParseCalendar(body, result);
            std::lock_guard lock(dataMutex);
            const auto latest = SourcesForYear(pending, year);
            if (std::ranges::find(latest, source.url, &Source::url) == latest.end()) continue;
            auto previous = std::ranges::find(feeds, source.url, &Feed::url);
            if (ok) {
                if (previous == feeds.end()) feeds.push_back(std::move(result));
                else *previous = std::move(result);
            } else if (previous == feeds.end()) {
                feeds.push_back({source.url, {}, true});
            } else {
                previous->failed = true;
            }
            PostMessageW(widget.load(), RefreshMessage, 0, 0);
        }
        {
            std::lock_guard lock(dataMutex);
            const auto latest = SourcesForYear(pending, year);
            std::erase_if(feeds, [&](const Feed& feed) {
                return std::ranges::find(latest, feed.url, &Source::url) == latest.end();
            });
        }
        refreshing = false;
        PostMessageW(widget.load(), RefreshMessage, 0, 0);
        delay = config.refresh * 60000;
    }
    return 0;
}

constexpr int GridTop = 88;
constexpr int Margin = 16;
constexpr PCWSTR MonthNames[] = {L"Enero", L"Febrero", L"Marzo", L"Abril", L"Mayo",
    L"Junio", L"Julio", L"Agosto", L"Septiembre", L"Octubre", L"Noviembre", L"Diciembre"};
constexpr PCWSTR MonthCodes[] = {L"ENE", L"FEB", L"MAR", L"ABR", L"MAY", L"JUN",
    L"JUL", L"AGO", L"SEP", L"OCT", L"NOV", L"DIC"};
constexpr PCWSTR WeekdayNames[] = {L"LU", L"MA", L"MI", L"JU", L"VI", L"SÁ", L"DO"};
constexpr PCWSTR ModeNames[] = {L"Compacta", L"Mes", L"Semana", L"Día", L"Año"};

sys_days MondayOf(sys_days date) {
    return date - days((weekday(date).c_encoding() + 6) % 7);
}

struct View {
    Settings config;
    sys_days selected = Today();
    year_month month = year_month_day(selected).year() / year_month_day(selected).month();
    int agendaOffset = 0;
    int agendaCount = 0;
    int agendaPage = 0;
    int agendaTop = 0;
    Mode mode = Mode::Compact;
    int width = 390, height = 454;
    int dpi = 96;
    DWORD clockStamp = 0;
    bool positioned = false;
    struct DayTarget { RECT area; sys_days day; };
    std::vector<DayTarget> dayTargets;
    int Scale(int value) const { return MulDiv(value, dpi, 96); }
    void Select(sys_days value) {
        selected = value;
        year_month_day date(value);
        month = date.year() / date.month();
        agendaOffset = 0;
        dayTargets.clear();
    }
    void ChangeMode(Mode value) {
        mode = value;
        agendaOffset = 0;
        dayTargets.clear();
    }
    void Navigate(int amount) {
        if (mode == Mode::Week) Select(selected + days(amount * 7));
        else if (mode == Mode::Day) Select(selected + days(amount));
        else Select(sys_days((month + months(mode == Mode::Year ? amount * 12 : amount)) / 1));
    }
    bool ScrollAgenda(int amount) {
        if (!agendaPage || agendaCount <= agendaPage) return false;
        agendaOffset = std::clamp(agendaOffset + amount, 0, agendaCount - agendaPage);
        return true;
    }
};

DWORD ClockStamp(const SYSTEMTIME& local) {
    return (DWORD(local.wDay) * 24 + local.wHour) * 60 + local.wMinute;
}

std::wstring HeaderClock(const SYSTEMTIME& local) {
    wchar_t clock[16]{};
    swprintf(clock, std::size(clock), L"%02u:%02u %ls/%02u", unsigned(local.wHour),
        unsigned(local.wMinute), MonthCodes[local.wMonth - 1], unsigned(local.wYear % 100));
    return clock;
}

void ApplyAppearance(HWND window, View& view) {
    HMONITOR monitor = view.positioned ? MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST)
                                       : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) return;
    auto area = info.rcWork;
    RECT position{};
    GetWindowRect(window, &position);
    view.width = std::min(view.config.width, MulDiv(area.right - area.left, 96, view.dpi));
    view.height = std::min(view.config.height, MulDiv(area.bottom - area.top, 96, view.dpi));
    int width = view.Scale(view.width), height = view.Scale(view.height);
    int x = view.positioned ? position.left : area.right - width - view.Scale(24);
    int y = view.positioned ? position.top : area.top + view.Scale(24);
    x = std::clamp(x, int(area.left), std::max(int(area.left), int(area.right) - width));
    y = std::clamp(y, int(area.top), std::max(int(area.top), int(area.bottom) - height));
    SetWindowPos(window, HWND_BOTTOM, x, y, width, height, SWP_NOACTIVATE);
    HRGN region = CreateRoundRectRgn(0, 0, width + 1, height + 1,
                                    view.Scale(24), view.Scale(24));
    if (region && !SetWindowRgn(window, region, TRUE)) DeleteObject(region);
    view.positioned = true;
    InvalidateRect(window, nullptr, FALSE);
}

struct Surface {
    HDC dc;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    DWORD* pixels = nullptr;

    Surface(HDC target, int width, int height)
        : dc(width > 0 && height > 0 ? CreateCompatibleDC(target) : nullptr) {
        if (!dc) return;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* data = nullptr;
        bitmap = CreateDIBSection(target, &info, DIB_RGB_COLORS, &data, nullptr, 0);
        if (bitmap) {
            previous = SelectObject(dc, bitmap);
            if (previous) pixels = static_cast<DWORD*>(data);
        }
    }
    ~Surface() {
        if (previous) SelectObject(dc, previous);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
    }
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
};

void CompositeLayer(DWORD* pixels, const DWORD* mask, size_t count,
                    COLORREF background, BYTE opacity) {
    for (size_t index = 0; index < count; ++index) {
        DWORD pixel = pixels[index];
        // The mask is white wherever foreground is opaque and gray at text edges.
        int uncovered = 255 - int(mask ? mask[index] & 255 : 0);
        int removed = (uncovered * (255 - opacity) + 127) / 255;
        int alpha = 255 - removed;
        auto channel = [removed, alpha](int composite, int base) {
            return DWORD(std::clamp(composite - (base * removed + 127) / 255, 0, alpha));
        };
        pixels[index] = (DWORD(alpha) << 24) |
            (channel((pixel >> 16) & 255, GetRValue(background)) << 16) |
            (channel((pixel >> 8) & 255, GetGValue(background)) << 8) |
            channel(pixel & 255, GetBValue(background));
    }
}

void Paint(HWND window, View& view) {
    PAINTSTRUCT paint{};
    HDC destination = BeginPaint(window, &paint);
    RECT area{};
    GetClientRect(window, &area);
    if (area.right <= 0 || area.bottom <= 0) { EndPaint(window, &paint); return; }
    const BYTE opacity = BYTE((view.config.opacity * 255 + 50) / 100);
    const bool backgroundOnly = view.config.transparentBackground && opacity < 255;
    Surface canvas(destination, area.right, area.bottom);
    Surface mask(destination, backgroundOnly ? area.right : 0, backgroundOnly ? area.bottom : 0);
    if (!canvas.pixels || (backgroundOnly && !mask.pixels)) {
        EndPaint(window, &paint);
        return;
    }
    HDC dc = canvas.dc;
    HDC maskDc = backgroundOnly ? mask.dc : nullptr;
    auto scale = [&](int value) { return view.Scale(value); };
    int width = view.width, height = view.height;
    auto fill = [&](int x, int y, int w, int h, COLORREF color, int rounding = 0) {
        RECT rect{scale(x), scale(y), scale(x + w), scale(y + h)};
        HBRUSH brush = CreateSolidBrush(color);
        HRGN rounded = rounding > 0 ? CreateRoundRectRgn(rect.left, rect.top, rect.right,
            rect.bottom, scale(rounding), scale(rounding)) : nullptr;
        if (rounded) {
            FillRgn(dc, rounded, brush);
            if (maskDc) FillRgn(maskDc, rounded, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
            DeleteObject(rounded);
        } else {
            FillRect(dc, &rect, brush);
            if (maskDc) FillRect(maskDc, &rect, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
        }
        DeleteObject(brush);
    };
    SetBkMode(dc, TRANSPARENT);
    if (maskDc) {
        SetBkMode(maskDc, TRANSPARENT);
        SetTextColor(maskDc, RGB(255, 255, 255));
        FillRect(maskDc, &area, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
    }
    HFONT normal = CreateFontW(-scale(14), 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Segoe UI");
    HFONT small = CreateFontW(-scale(12), 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Segoe UI");
    HFONT heading = CreateFontW(-scale(22), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
        DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Segoe UI");
    HFONT tiny = CreateFontW(-scale(10), 0, 0, 0, FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(dc, normal);
    HGDIOBJ oldMaskFont = maskDc ? SelectObject(maskDc, normal) : nullptr;
    auto label = [&](std::wstring_view text, int x, int y, int w, int h,
                     COLORREF color, HFONT font, bool center = false) {
        RECT rect{scale(x), scale(y), scale(x + w), scale(y + h)};
        SelectObject(dc, font);
        SetTextColor(dc, color);
        UINT format = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX |
                      (center ? DT_CENTER : DT_LEFT);
        DrawTextW(dc, text.data(), int(text.size()), &rect, format);
        if (maskDc) {
            SelectObject(maskDc, font);
            DrawTextW(maskDc, text.data(), int(text.size()), &rect, format);
        }
    };
    const auto background = view.config.background, foreground = view.config.text;
    const auto accent = view.config.accent, holidayColor = view.config.holidayColor;
    auto mutedChannel = [](int a, int b) { return (a * 6 + b * 4) / 10; };
    auto muted = RGB(mutedChannel(GetRValue(foreground), GetRValue(background)),
                     mutedChannel(GetGValue(foreground), GetGValue(background)),
                     mutedChannel(GetBValue(foreground), GetBValue(background)));
    auto mix = [](COLORREF base, COLORREF color, int percent) {
        auto channel = [percent](int a, int b) { return (a * (100 - percent) + b * percent) / 100; };
        return RGB(channel(GetRValue(base), GetRValue(color)),
                   channel(GetGValue(base), GetGValue(color)),
                   channel(GetBValue(base), GetBValue(color)));
    };
    HBRUSH panelBrush = CreateSolidBrush(background);
    FillRect(dc, &area, panelBrush);
    DeleteObject(panelBrush);


    const auto today = Today();
    const year_month_day selected(view.selected);
    const auto weekStart = MondayOf(view.selected);
    std::wstring caption = MonthNames[unsigned(view.month.month()) - 1];
    caption += L" " + std::to_wstring(int(view.month.year()));
    if (view.mode == Mode::Year) caption = std::to_wstring(int(view.month.year()));
    if (view.mode == Mode::Day) {
        caption = std::to_wstring(unsigned(selected.day())) + L" " +
            std::wstring(MonthNames[unsigned(selected.month()) - 1], 3) + L" " +
            std::to_wstring(int(selected.year()));
    }
    if (view.mode == Mode::Week) {
        year_month_day start(weekStart), end(weekStart + days{6});
        caption = std::to_wstring(unsigned(start.day()));
        if (start.month() != end.month())
            caption += L" " + std::wstring(MonthNames[unsigned(start.month()) - 1], 3);
        caption += L"–" + std::to_wstring(unsigned(end.day())) + L" " +
            std::wstring(MonthNames[unsigned(end.month()) - 1], 3);
    }
    SYSTEMTIME local{};
    GetLocalTime(&local);
    view.clockStamp = ClockStamp(local);
    if (view.config.showClock) caption = HeaderClock(local);
    label(caption, 19, 10, width - 159, 36, foreground, heading);
    label(L"‹", width - 138, 10, 27, 36, foreground, heading, true);
    label(L"Hoy", width - 111, 10, 40, 36, accent, small, true);
    label(L"›", width - 71, 10, 27, 36, foreground, heading, true);
    label(L"↻", width - 44, 10, 27, 36, accent, heading, true);

    const int tabWidth = (width - 2 * Margin) / 5;
    for (int index = 0; index < 5; ++index) {
        int x = Margin + index * tabWidth;
        bool active = view.mode == Mode(index);
        if (active) fill(x + 3, 72, tabWidth - 6, 2, accent);
        label(ModeNames[index], x, 48, tabWidth, 24, active ? accent : muted, small, true);
    }

    struct AgendaRow { int minute; std::wstring title, source; COLORREF color; bool holiday; };
    struct CalendarDay {
        std::vector<AgendaRow> entries;
        std::vector<COLORREF> colors;
        int count = 0;
        bool holiday = false;
    };
    const auto earlier = [](const AgendaRow& left, const AgendaRow& right) {
        if (left.holiday != right.holiday) return left.holiday;
        return left.minute < right.minute;
    };
    sys_days first = MondayOf(sys_days(view.month / 1));
    int dayCount = 42;
    const bool eventMonth = view.mode == Mode::Month;
    if (eventMonth) {
        auto nextMonth = sys_days((view.month + months{1}) / 1);
        dayCount = int(((nextMonth - first).count() + 6) / 7) * 7;
    } else if (view.mode == Mode::Year) {
        first = sys_days(view.month.year() / January / 1);
        dayCount = int((sys_days((view.month.year() + years{1}) / January / 1) - first).count());
    } else if (view.mode == Mode::Week) {
        first = weekStart;
        dayCount = 7;
    } else if (view.mode == Mode::Day) {
        first = view.selected;
        dayCount = 1;
    }
    std::vector<CalendarDay> calendar(dayCount);
    std::vector<AgendaRow> agenda;
    const int bodyBottom = height - 43;
    const bool hasAgenda = view.mode == Mode::Compact || view.mode == Mode::Week || view.mode == Mode::Day;
    const int gridHeight = view.mode == Mode::Compact ? 6 * 27 :
        view.mode == Mode::Week ? std::max(62, (bodyBottom - GridTop - 23) / 2) :
        std::max(0, bodyBottom - GridTop - 23);
    constexpr int EventRowHeight = 16;
    const int eventSlots = std::max(0,
        ((eventMonth ? gridHeight / (dayCount / 7) : gridHeight) - 22) / EventRowHeight);
    const int currentYear = int(year_month_day(today).year());
    const int displayedYear = int(view.month.year());
    const auto sources = SourcesForYear(view.config, currentYear);
    int errors = 0, affected = 0;
    unsigned issues = 0;
    std::wstring failedSource, affectedSource;
    {
        std::lock_guard lock(dataMutex);
        for (const auto& source : sources) {
            auto feed = std::ranges::find(feeds, source.url, &Feed::url);
            if (feed == feeds.end()) continue;
            if (!source.holidayYear || source.holidayYear == displayedYear) {
                if (feed->failed) { ++errors; failedSource = source.holiday ? L"Festivos" : source.name; }
                if (feed->issues) { ++affected; affectedSource = source.name; issues |= feed->issues; }
            }
            for (const auto& event : feed->events) {
                for (auto day = std::max(first, event.start);
                     day < std::min(first + days(dayCount), event.end); day += days{1}) {
                    auto& cell = calendar[(day - first).count()];
                    cell.holiday |= source.holiday;
                    ++cell.count;
                    if (view.mode == Mode::Compact && !source.holiday && cell.colors.size() < 4 &&
                        std::ranges::find(cell.colors, source.color) == cell.colors.end())
                        cell.colors.push_back(source.color);
                    if ((view.mode == Mode::Month || view.mode == Mode::Week) && eventSlots > 0) {
                        AgendaRow row{day == event.start ? event.minute : -1, event.title,
                                      source.name, source.color, source.holiday};
                        auto position = std::upper_bound(cell.entries.begin(), cell.entries.end(), row, earlier);
                        if (position != cell.entries.end() || int(cell.entries.size()) < eventSlots) {
                            cell.entries.insert(position, std::move(row));
                            if (int(cell.entries.size()) > eventSlots) cell.entries.pop_back();
                        }
                    }
                }
                if (hasAgenda && event.start <= view.selected && view.selected < event.end) {
                    agenda.push_back({event.start == view.selected ? event.minute : -1,
                                      event.title, source.name, source.color, source.holiday});
                }
            }
        }
    }

    auto dayColor = [&](sys_days day, bool official, COLORREF fallback) {
        if (official || (view.config.markSundays && weekday(day) == Sunday)) return holidayColor;
        return day == today ? accent : fallback;
    };
    auto timeText = [](int minute) -> std::wstring {
        if (minute < 0) return {};
        wchar_t clock[8]{};
        swprintf(clock, std::size(clock), L"%02d:%02d", minute / 60, minute % 60);
        return clock;
    };
    auto barText = [](COLORREF color) {
        int luminance = 299 * GetRValue(color) + 587 * GetGValue(color) + 114 * GetBValue(color);
        return luminance > 140000 ? RGB(14, 18, 25) : RGB(255, 255, 255);
    };
    view.dayTargets.clear();
    const int gridWidth = width - 2 * Margin;
    if (view.mode == Mode::Year) {
        constexpr int columns = 3, rows = 4, gap = 8;
        const int cardWidth = (gridWidth - gap * (columns - 1)) / columns;
        const int cardHeight = (bodyBottom - GridTop - gap * (rows - 1)) / rows;
        for (int index = 0; index < 12; ++index) {
            auto month = view.month.year() / std::chrono::month(index + 1);
            int left = Margin + (index % columns) * (cardWidth + gap);
            int top = GridTop + (index / columns) * (cardHeight + gap);
            int titleHeight = std::clamp(cardHeight / 5, 10, 21);
            auto title = cardWidth < 105 ? std::wstring_view(MonthNames[index], 3) : MonthNames[index];
            label(title, left, top, cardWidth, titleHeight, foreground, small);
            fill(left, top + titleHeight, cardWidth, 1, muted);
            auto firstCell = MondayOf(sys_days(month / 1));
            const int gridY = top + titleHeight + 2;
            const int rowsHeight = cardHeight - titleHeight - 2;
            if (rowsHeight <= 0) continue;
            for (int dayIndex = 0; dayIndex < 42; ++dayIndex) {
                auto day = firstCell + days(dayIndex);
                year_month_day date(day);
                if (date.month() != month.month()) continue;
                const auto& cell = calendar[(day - first).count()];
                int x = left + (dayIndex % 7) * cardWidth / 7;
                int right = left + (dayIndex % 7 + 1) * cardWidth / 7;
                int y = gridY + (dayIndex / 7) * rowsHeight / 6;
                int bottom = gridY + (dayIndex / 7 + 1) * rowsHeight / 6;
                if (day == view.selected) fill(x, y, right - x, bottom - y, dayColor(day, cell.holiday, accent));
                label(std::to_wstring(unsigned(date.day())), x, y, right - x, bottom - y,
                      day == view.selected ? background : dayColor(day, cell.holiday, foreground), tiny, true);
                if (cell.count && day != view.selected && bottom - y >= 10)
                    fill(x + (right - x) / 2 - 1, bottom - 1, 3, 1, cell.holiday ? holidayColor : accent);
                view.dayTargets.push_back({{x, y, right, bottom}, day});
            }
        }
    } else if (view.mode != Mode::Day) {
        for (int col = 0; col < 7; ++col) {
            int x = Margin + col * gridWidth / 7;
            int right = Margin + (col + 1) * gridWidth / 7;
            label(WeekdayNames[col], x, GridTop, right - x, 21,
                  col == 6 && view.config.markSundays ? holidayColor : muted, small, true);
        }
        const int rows = dayCount / 7;
        if (eventMonth) {
            auto divider = mix(background, foreground, 22);
            for (int row = 0; row < rows; ++row)
                fill(Margin, GridTop + 23 + row * gridHeight / rows, gridWidth, 1, divider);
        }
        for (int index = 0; index < dayCount; ++index) {
            auto day = first + days(index);
            year_month_day date(day);
            if (eventMonth && date.month() != view.month.month()) continue;
            const auto& cell = calendar[index];
            int x = Margin + (index % 7) * gridWidth / 7;
            int right = Margin + (index % 7 + 1) * gridWidth / 7;
            int y = GridTop + 23 + (index / 7) * gridHeight / rows;
            int bottom = GridTop + 23 + (index / 7 + 1) * gridHeight / rows;
            const int cellWidth = right - x;
            auto number = dayColor(day, cell.holiday,
                date.month() == view.month.month() || view.mode == Mode::Week ? foreground : muted);
            if (view.mode == Mode::Compact) {
                auto selectionColor = dayColor(day, cell.holiday, accent);
                if (day == view.selected) fill(x + 2, y, cellWidth - 4, 26, selectionColor);
                label(std::to_wstring(unsigned(date.day())), x, y, cellWidth, 22,
                      day == view.selected ? background : number, normal, true);
                int dotX = x + (cellWidth - int(cell.colors.size()) * 7 + 3) / 2;
                for (auto color : cell.colors) {
                    fill(dotX, y + 22, 4, 3,
                         day == view.selected && color == selectionColor ? background : color);
                    dotX += 7;
                }
            } else {
                if (!eventMonth) {
                    fill(x, y, cellWidth, 1, muted);
                    fill(x, y, 1, bottom - y, muted);
                    if (index % 7 == 6) fill(right - 1, y, 1, bottom - y, muted);
                }
                const int visible = int(cell.entries.size());
                const int dateX = eventMonth && (cell.count == visible || cellWidth >= 52)
                    ? x + (cellWidth - 20) / 2 : x + 3;
                if (day == view.selected) {
                    auto selection = dayColor(day, cell.holiday, accent);
                    if (eventMonth) fill(dateX, y + 2, 20, 20, selection, 20);
                    else fill(dateX, y + 3, 22, 17, selection);
                }
                label(std::to_wstring(unsigned(date.day())), dateX, y + 2, eventMonth ? 20 : 22, 19,
                      day == view.selected ? background : number, eventMonth ? normal : small, true);
                if (cell.count > visible) {
                    int badgeX = eventMonth ? dateX + 20 : x + 25;
                    if (right - badgeX >= 11)
                        label(L"+" + std::to_wstring(cell.count - visible), badgeX, y + 2,
                              right - badgeX, 19, muted, tiny, true);
                }
                for (int row = 0; row < visible; ++row) {
                    const auto& entry = cell.entries[row];
                    int barY = y + 22 + row * EventRowHeight;
                    auto time = timeText(entry.minute);
                    // Leave room for part of the title in compact columns.
                    auto text = time.empty() || cellWidth < 70 ? entry.title : time + L" " + entry.title;
                    auto eventBackground = eventMonth ? mix(background, entry.color, 30) : entry.color;
                    auto contrast = barText(eventBackground);
                    auto eventText = eventMonth ? mix(entry.color, contrast, 70) : contrast;
                    fill(x + 3, barY, cellWidth - 6, EventRowHeight - 2, eventBackground, eventMonth ? 10 : 0);
                    label(text, x + 6, barY, cellWidth - 12, EventRowHeight - 2, eventText, small);
                }
            }
            view.dayTargets.push_back({{x, y, right, bottom}, day});
        }
    }

    view.agendaTop = 0;
    view.agendaPage = 0;
    view.agendaCount = int(agenda.size());
    if (hasAgenda) {
        std::ranges::stable_sort(agenda, earlier);
        const int top = view.mode == Mode::Day ? GridTop : GridTop + 23 + gridHeight + 2;
        view.agendaTop = top;
        view.agendaPage = std::max(1, (bodyBottom - top - 28) / 36);
        view.agendaOffset = std::clamp(view.agendaOffset, 0, std::max(0, view.agendaCount - view.agendaPage));
        fill(Margin, top, gridWidth, 1, muted);
        auto title = std::to_wstring(unsigned(selected.day())) + L" de " +
                     MonthNames[unsigned(selected.month()) - 1];
        const auto& selectedCell = calendar[(view.selected - first).count()];
        label(title, 19, top + 2, width - 142, 25,
              dayColor(view.selected, selectedCell.holiday, foreground), normal);
        if (view.agendaCount > view.agendaPage) {
            auto amount = std::to_wstring(view.agendaOffset + 1) + L"–" +
                std::to_wstring(view.agendaOffset + view.agendaPage) + L" / " + std::to_wstring(view.agendaCount);
            label(L"↑", width - 114, top + 2, 21, 25, accent, normal, true);
            label(amount, width - 93, top + 2, 55, 25, muted, small, true);
            label(L"↓", width - 38, top + 2, 21, 25, accent, normal, true);
        }
        if (agenda.empty()) label(L"Sin eventos", 19, top + 28, width - 38, 25, muted, normal);
        const int end = std::min(view.agendaCount, view.agendaOffset + view.agendaPage);
        for (int index = view.agendaOffset; index < end; ++index) {
            const auto& row = agenda[index];
            int y = top + 28 + (index - view.agendaOffset) * 36;
            fill(19, y + 4, 3, 26, row.color);
            label(row.title, 29, y, width - 48, 20, row.holiday ? row.color : foreground, normal);
            auto detail = timeText(row.minute);
            if (detail.empty()) detail = L"Todo el día / en curso";
            label(detail + L"  ·  " + row.source, 29, y + 18, width - 48, 16, muted, small);
        }
    }
    std::wstring status;
    const bool municipal = view.config.holidayUrl.empty() && ValidMunicipality(view.config.municipality);
    if (view.config.holidayUrl.empty() && !view.config.municipality.empty() && !municipal)
        status = L"El código INE debe tener cinco dígitos";
    else if (sources.empty()) status = L"Añade un enlace en los ajustes de Windhawk";
    else if (refreshing) status = L"Actualizando calendarios…";
    else if (errors == 1) status = L"Error al actualizar · " + failedSource;
    else if (errors > 1) status = std::to_wstring(errors) + L" calendarios con error";
    else if (municipal && (displayedYear < currentYear - 1 || displayedYear > currentYear + 1))
        status = L"Festivos municipales fuera del intervalo consultado";
    else {
        auto count = view.config.sources.size();
        if (count) status = std::to_wstring(count) + (count == 1 ? L" calendario" : L" calendarios");
        if (municipal || !view.config.holidayUrl.empty())
            status += count ? L" · Festivos" : L"Festivos";
    }
    label(status, 19, affected ? height - 37 : height - 30, width - 38, 18, muted, small);
    if (affected) {
        PCWSTR reason = L"Eventos incompletos";
        if (issues == UnsupportedRule) reason = L"Repeticiones incompatibles";
        if (issues == UnsupportedDate) reason = L"Fechas o zonas incompatibles";
        if (issues == UnsupportedException) reason = L"Excepciones de repetición incompatibles";
        if (issues == UnsupportedDuration) reason = L"Duraciones incompatibles";
        std::wstring detail = reason;
        if (affected == 1) detail += L" · " + affectedSource;
        label(detail, 19, height - 19, width - 38, 16, muted, small);
    }

    SelectObject(dc, oldFont);
    if (maskDc) SelectObject(maskDc, oldMaskFont);
    DeleteObject(heading);
    DeleteObject(tiny);
    DeleteObject(small);
    DeleteObject(normal);
    GdiFlush();
    const size_t pixelCount = size_t(area.right) * area.bottom;
    CompositeLayer(canvas.pixels, maskDc ? mask.pixels : nullptr, pixelCount,
                   background, backgroundOnly ? opacity : 255);
    POINT origin{};
    SIZE size{area.right, area.bottom};
    // Keep a hit-testable surface at 0% so the panel can still be dragged.
    if (backgroundOnly && !opacity) {
        for (size_t index = 0; index < pixelCount; ++index)
            if (!canvas.pixels[index]) canvas.pixels[index] = 0x01000000;
    }
    BLENDFUNCTION blend{AC_SRC_OVER, 0, view.config.transparentBackground ? BYTE(255) : opacity,
                        AC_SRC_ALPHA};
    UpdateLayeredWindow(window, nullptr, nullptr, &size, dc, &origin, 0, &blend, ULW_ALPHA);
    EndPaint(window, &paint);
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
    }
    auto* view = reinterpret_cast<View*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (!view) return DefWindowProcW(window, message, wParam, lParam);
    switch (message) {
        case SettingsMessage: {
            auto previousMode = view->config.initialView;
            {
                std::lock_guard lock(dataMutex);
                view->config = pending;
            }
            if (previousMode != view->config.initialView) view->ChangeMode(view->config.initialView);
            ApplyAppearance(window, *view);
            [[fallthrough]];
        }
        case RefreshMessage:
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        case WM_TIMER: {
            SYSTEMTIME local{};
            GetLocalTime(&local);
            if (ClockStamp(local) != view->clockStamp &&
                (view->config.showClock || ClockStamp(local) / 1440 != view->clockStamp / 1440)) {
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }
        case WM_TIMECHANGE:
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        case WM_PAINT:
            Paint(window, *view);
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_DPICHANGED:
            view->dpi = HIWORD(wParam);
            ApplyAppearance(window, *view);
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            ApplyAppearance(window, *view);
            return 0;
        case WM_NCHITTEST: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(window, &point);
            if (point.y < view->Scale(47) && point.x < view->Scale(view->width - 138))
                return HTCAPTION;
            return HTCLIENT;
        }
        case WM_LBUTTONUP: {
            int x = MulDiv(GET_X_LPARAM(lParam), 96, view->dpi);
            int y = MulDiv(GET_Y_LPARAM(lParam), 96, view->dpi);
            int width = view->width;
            int tabWidth = (width - 2 * Margin) / 5;
            if (y >= 10 && y < 46 && x >= width - 138 && x < width - 17) {
                if (x < width - 111) view->Navigate(-1);
                else if (x < width - 71) view->Select(Today());
                else if (x < width - 44) view->Navigate(1);
                else SetEvent(wakeEvent);
            } else if (y >= 48 && y < 76 && x >= Margin && x < Margin + 5 * tabWidth) {
                view->ChangeMode(Mode((x - Margin) / tabWidth));
            } else if (view->agendaTop && y >= view->agendaTop && y < view->agendaTop + 27) {
                if (x >= width - 114 && x < width - 93)
                    view->ScrollAgenda(-1);
                if (x >= width - 38 && x < width - 17)
                    view->ScrollAgenda(1);
            } else {
                auto hit = std::ranges::find_if(view->dayTargets, [&](const View::DayTarget& target) {
                    return PtInRect(&target.area, {x, y});
                });
                if (hit != view->dayTargets.end()) {
                    view->Select(hit->day);
                    if (view->mode == Mode::Month) view->ChangeMode(Mode::Day);
                    else if (view->mode == Mode::Year) view->ChangeMode(Mode::Month);
                }
            }
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(window, &point);
            int direction = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1 : 1;
            if (view->agendaTop && point.y >= view->Scale(view->agendaTop))
                view->ScrollAgenda(direction);
            else view->Navigate(direction);
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }
        case WM_CLOSE:
            return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

DWORD WINAPI UiLoop(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HINSTANCE module{};
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<PCWSTR>(WindowClass), &module);
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = module;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = WindowClass;
    ATOM registered = RegisterClassW(&windowClass);
    HWND window = nullptr;
    View view{};
    {
        std::lock_guard lock(dataMutex);
        view.config = pending;
    }
    view.mode = view.config.initialView;
    if (registered) {
        window = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            WindowClass, L"Calendario de escritorio", WS_POPUP,
            0, 0, 1, 1, nullptr, nullptr, module, &view);
    }
    if (window) {
        view.dpi = GetDpiForWindow(window);
        ApplyAppearance(window, view);
        ShowWindow(window, SW_SHOWNOACTIVATE);
        InvalidateRect(window, nullptr, FALSE);
        SetTimer(window, 1, 1000, nullptr);
        widget = window;
    }
    SetEvent(readyEvent);
    if (window) {
        while (MsgWaitForMultipleObjects(1, &stopEvent, FALSE, INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0 + 1) {
            MSG msg;
            while (WaitForSingleObject(stopEvent, 0) == WAIT_TIMEOUT &&
                   PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        widget = nullptr;
        DestroyWindow(window);
    }
    if (registered) UnregisterClassW(WindowClass, module);
    return 0;
}

void Stop() {
    if (stopEvent) SetEvent(stopEvent);
    for (HANDLE thread : {fetchThread, uiThread})
        if (thread) WaitForSingleObject(thread, INFINITE);
    for (HANDLE handle : {fetchThread, uiThread, stopEvent, wakeEvent, readyEvent})
        if (handle) CloseHandle(handle);
    fetchThread = uiThread = stopEvent = wakeEvent = readyEvent = nullptr;
    feeds.clear();
}
}  // namespace

BOOL Wh_ModInit() {
    DWORD shellPid = 0;
    GetWindowThreadProcessId(GetShellWindow(), &shellPid);
    // Explorer can also run folder windows in a separate process.
    if (shellPid && shellPid != GetCurrentProcessId()) return FALSE;
    LoadSettings();
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent || !wakeEvent || !readyEvent) { Stop(); return FALSE; }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!stopEvent) return;
    uiThread = CreateThread(nullptr, 0, UiLoop, nullptr, 0, nullptr);
    if (uiThread) {
        WaitForSingleObject(readyEvent, INFINITE);
        if (widget) fetchThread = CreateThread(nullptr, 0, FetchLoop, nullptr, 0, nullptr);
    }
    if (!widget || !fetchThread) {
        Wh_Log(L"No se ha podido iniciar el calendario de escritorio");
        Stop();
    }
}

void Wh_ModSettingsChanged() {
    if (!stopEvent) return;
    LoadSettings();
    PostMessageW(widget.load(), SettingsMessage, 0, 0);
    SetEvent(wakeEvent);
}

void Wh_ModUninit() {
    Stop();
}
