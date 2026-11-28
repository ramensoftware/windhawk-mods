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
El mes completo y el año junto al reloj siguen el periodo que estás consultando.
La cabecera usa el color de selección. Las modificaciones y cancelaciones
individuales de eventos repetidos sustituyen su ocurrencia original.
En la vista mensual, +N indica cuántos eventos del día quedan sin mostrar.
La cabecera está centrada y los controles comparten la fila de las vistas.
Activa Bloqueo para impedir cambios con clics, rueda o arrastre; el selector
permanece disponible para desbloquear el calendario. Se recuerda al reiniciar.
En Compacta, las casillas más altas muestran un punto por evento en varias
filas, con +N cuando no caben todos.
El botón de actualización gira mientras se consultan las suscripciones.
El calendario no muestra mensajes ni contadores de suscripciones en el pie.

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
  $description: Muestra la hora local junto al mes completo y año de la vista consultada, en el color de selección.
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
      $name: Municipio de los festivos (código INE, no postal)
      $description: Cinco dígitos del INE. Madrid capital es 28079, aunque tu código postal sea distinto. Incluye festivos nacionales, autonómicos y locales publicados. Vacío para desactivar.
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
#include <cmath>
#include <cstdio>
#include <cwchar>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
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
    std::wstring name;
};

constexpr UINT SettingsMessage = WM_APP + 1;
constexpr UINT RefreshMessage = WM_APP + 2;
constexpr UINT_PTR ClockTimerId = 1;
constexpr UINT_PTR RefreshAnimationTimerId = 2;
constexpr UINT RefreshAnimationInterval = 80;
constexpr unsigned RefreshAnimationFrames = 24;
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
        auto rawOccurrence = std::string(formatted) + std::string(rawStart.substr(8));
        auto occurrence = ParseDate(rawOccurrence, zone);
        if (!occurrence.valid) return false;
        if (until.valid && (occurrence.day > until.day ||
            (occurrence.day == until.day && occurrence.minute > until.minute && until.minute >= 0)))
            return true;
        if ((occurrence.day != start.day || occurrence.minute != start.minute) &&
            !emit(occurrence, rawOccurrence)) return false;
    }
    return previous >= count;
}

bool ParseCalendar(std::string_view body, Feed& feed) {
    if (body.starts_with("\xef\xbb\xbf")) body.remove_prefix(3);
    if (!body.starts_with("BEGIN:VCALENDAR")) return false;
    std::string line;
    struct Occurrence {
        Date date;
        std::string raw, zone;
    };
    struct Component {
        Date start, end, recurrence;
        std::wstring title = L"Sin título";
        std::string uid, rule, rawStart, startZone, rawRecurrence, recurrenceZone, stamp;
        std::vector<Occurrence> extra;
        std::vector<Date> excluded;
        int sequence = 0;
        unsigned issues = 0;
        bool cancelled = false, hasRecurrence = false, hasTitle = false;
        bool hasStart = false, future = false;
    } current;
    std::vector<Component> components;
    bool inside = false, complete = false;
    int depth = 0;
    auto process = [&] {
        constexpr std::string_view namePrefix = "X-WR-CALNAME:";
        if (!inside && line.starts_with(namePrefix)) feed.name = EventTitle(std::string_view(line).substr(namePrefix.size()));
        if (line == "END:VCALENDAR" && !inside) complete = true;
        if (line == "BEGIN:VEVENT" && !inside) {
            current = {};
            inside = true;
            depth = 0;
        } else if (line == "END:VEVENT" && inside && !depth) {
            inside = false;
            components.push_back(std::move(current));
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
                current.hasStart = true;
                current.start = ParseDate(value, zone);
                current.rawStart = value;
                current.startZone = zone;
                if (!current.start.valid) current.issues |= UnsupportedDate;
            }
            if (name == "DTEND") {
                current.end = ParseDate(value, zone);
                if (!current.end.valid) current.issues |= UnsupportedDate;
            }
            if (name == "UID") current.uid = value;
            if (name == "SEQUENCE") current.sequence = std::max(0, Digits(value));
            if (name == "DTSTAMP") current.stamp = value;
            if (name == "SUMMARY") {
                current.title = EventTitle(value);
                current.hasTitle = true;
            }
            if (name == "STATUS") current.cancelled = value == "CANCELLED";
            if (name == "RRULE") current.rule = value;
            if (name == "RECURRENCE-ID") {
                current.hasRecurrence = true;
                current.recurrence = ParseDate(value, zone);
                current.rawRecurrence = value;
                current.recurrenceZone = zone;
                if (!current.recurrence.valid) current.issues |= UnsupportedException;
                if (auto position = key.find(";RANGE="); position != key.npos) {
                    auto range = key.substr(position + 7);
                    range = range.substr(0, range.find(';'));
                    current.future = range == "THISANDFUTURE";
                    if (!current.future) current.issues |= UnsupportedException;
                }
            }
            if (name == "DURATION") current.issues |= UnsupportedDuration;
            if (name == "RDATE" || name == "EXDATE") {
                while (!value.empty()) {
                    auto comma = value.find(',');
                    auto raw = value.substr(0, comma);
                    auto date = ParseDate(raw, zone);
                    if (date.valid) {
                        if (name == "RDATE") current.extra.push_back({date, std::string(raw), std::string(zone)});
                        else current.excluded.push_back(date);
                    } else current.issues |= UnsupportedDate;
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
        if (components.size() > MaxEvents || line.size() > 65536) return false;
        if (newline == body.npos) break;
        body.remove_prefix(newline + 1);
    }
    if (!complete) process();
    if (!complete || components.size() > MaxEvents) return false;

    using DateKey = std::tuple<sys_days, int, int>;
    auto keyOf = [](Date date) { return DateKey{date.day, date.minute, date.second}; };
    auto moment = [](Date date) {
        return sys_seconds(date.day) + minutes(std::max(0, date.minute)) + seconds(date.second);
    };
    auto durationOf = [&](const Component& component, Date fallback = {}) {
        const auto start = component.start.valid ? component.start : fallback;
        if (start.valid && component.end.valid) {
            auto duration = moment(component.end) - moment(start);
            if (duration < seconds::zero()) feed.issues |= UnsupportedDate;
            return std::max(seconds::zero(), duration);
        }
        return start.minute < 0 ? seconds(days{1}) : seconds::zero();
    };
    bool eventLimitExceeded = false;
    auto append = [&](Date date, seconds duration, const std::wstring& title) {
        if (feed.events.size() >= MaxEvents) { eventLimitExceeded = true; return false; }
        const auto end = ceil<days>(moment(date) + duration);
        feed.events.push_back({date.day, std::max(date.day + days{1}, end), date.minute, title});
        return true;
    };
    auto newer = [](const Component& left, const Component& right) {
        return std::tie(left.sequence, left.stamp) >= std::tie(right.sequence, right.stamp);
    };
    struct Series {
        const Component* master = nullptr;
        std::map<DateKey, const Component*> exceptions;
    };
    std::map<std::pair<bool, std::string>, Series> series;
    for (size_t index = 0; index < components.size(); ++index) {
        const auto& component = components[index];
        feed.issues |= component.issues;
        if (component.hasRecurrence && (component.uid.empty() || !component.recurrence.valid)) {
            feed.issues |= UnsupportedException;
            continue;
        }
        auto& group = series[{!component.uid.empty(),
            component.uid.empty() ? std::to_string(index) : component.uid}];
        if (component.hasRecurrence) {
            auto& previous = group.exceptions[keyOf(component.recurrence)];
            if (!previous || newer(component, *previous)) previous = &component;
        } else if (!group.master || newer(component, *group.master)) group.master = &component;
    }
    for (const auto& [uid, group] : series) {
        const auto* master = group.master;
        if (master && master->cancelled) continue;
        const seconds masterDuration = master ? durationOf(*master) : seconds::zero();
        std::map<DateKey, const Component*> overrides, ranges;
        for (const auto& [key, exception] : group.exceptions) {
            if ((exception->hasStart && !exception->start.valid) ||
                (master && master->start.valid &&
                 (master->start.minute < 0) != (exception->recurrence.minute < 0))) {
                feed.issues |= UnsupportedException;
                continue;
            }
            overrides.emplace(key, exception);
            if (exception->future) ranges.emplace(key, exception);
        }
        if (master && master->start.valid) {
            std::set<DateKey> emitted;
            auto emit = [&](Date original, std::string_view raw, std::string_view zone) {
                const auto key = keyOf(original);
                if (!emitted.insert(key).second || overrides.contains(key)) return true;
                if (std::ranges::any_of(master->excluded, [&](Date skip) {
                    return skip.day == original.day && (skip.minute < 0 || keyOf(skip) == key);
                })) return true;
                Date date = original;
                auto duration = masterDuration;
                const auto* title = &master->title;
                auto range = ranges.upper_bound(key);
                if (range != ranges.begin()) {
                    const auto& change = *std::prev(range)->second;
                    if (change.cancelled) return true;
                    // Shift the source's wall time before converting, preserving DST.
                    const bool sameZone = zone == change.startZone && zone == change.recurrenceZone &&
                        raw.ends_with("Z") == change.rawStart.ends_with("Z") &&
                        raw.ends_with("Z") == change.rawRecurrence.ends_with("Z");
                    if (sameZone && change.start.valid) {
                        auto wall = [](std::string_view value) {
                            if (value.ends_with("Z")) value.remove_suffix(1);
                            return ParseDate(value);
                        };
                        const auto shifted = moment(wall(raw)) +
                            (moment(wall(change.rawStart)) - moment(wall(change.rawRecurrence)));
                        const year_month_day day(floor<days>(shifted));
                        const auto time = shifted - floor<days>(shifted);
                        char formatted[32]{};
                        if (original.minute < 0) {
                            std::snprintf(formatted, sizeof(formatted), "%04d%02u%02u", int(day.year()),
                                unsigned(day.month()), unsigned(day.day()));
                        } else {
                            const auto hh = duration_cast<hours>(time);
                            const auto mm = duration_cast<minutes>(time - hh);
                            const auto ss = duration_cast<seconds>(time - hh - mm);
                            std::snprintf(formatted, sizeof(formatted), "%04d%02u%02uT%02d%02d%02d%s",
                                int(day.year()), unsigned(day.month()), unsigned(day.day()),
                                int(hh.count()), int(mm.count()), int(ss.count()), raw.ends_with("Z") ? "Z" : "");
                        }
                        date = ParseDate(formatted, zone);
                        if (!date.valid) { feed.issues |= UnsupportedException; return true; }
                        if (change.end.valid) duration = durationOf(change);
                        if (change.hasTitle) title = &change.title;
                    } else feed.issues |= UnsupportedException;
                }
                return append(date, duration, *title);
            };
            if (!emit(master->start, master->rawStart, master->startZone)) return false;
            for (const auto& extra : master->extra)
                if (!emit(extra.date, extra.raw, extra.zone)) return false;
            if (!master->rule.empty() && !ExpandRule(master->rule, master->rawStart, master->startZone,
                    master->start, [&](Date date, std::string_view raw) { return emit(date, raw, master->startZone); })) {
                if (eventLimitExceeded) return false;
                feed.issues |= UnsupportedRule;
            }
        } else if (master) feed.issues |= UnsupportedDate;
        // Overrides are emitted once, even when their master is missing or excluded.
        for (const auto& [key, exception] : overrides) {
            if (exception->cancelled) continue;
            const auto start = exception->start.valid ? exception->start : exception->recurrence;
            const auto duration = exception->end.valid || !master ? durationOf(*exception, start) : masterDuration;
            const auto& title = exception->hasTitle || !master ? exception->title : master->title;
            if (!append(start, duration, title)) return false;
        }
    }
    return feed.events.size() <= MaxEvents;
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
            if (!ok) Wh_Log(L"No se ha podido actualizar el calendario: %ls", source.name.c_str());
            else if (result.issues)
                Wh_Log(L"Incidencias de importación en %ls (máscara %u)", source.name.c_str(), result.issues);
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
constexpr int ToolbarTop = 48;
constexpr int ToolbarHeight = 24;
constexpr PCWSTR MonthNames[] = {L"Enero", L"Febrero", L"Marzo", L"Abril", L"Mayo",
    L"Junio", L"Julio", L"Agosto", L"Septiembre", L"Octubre", L"Noviembre", L"Diciembre"};
constexpr PCWSTR WeekdayNames[] = {L"LU", L"MA", L"MI", L"JU", L"VI", L"SÁ", L"DO"};
constexpr PCWSTR ModeNames[] = {L"Compacta", L"Mes", L"Semana", L"Día", L"Año"};
constexpr PCWSTR ShortModeNames[] = {L"Cmp", L"Mes", L"Sem", L"Día", L"Año"};

struct ToolbarLayout {
    int tabsRight;
    RECT previous, today, next, refresh, lock;
    RECT Tab(int index) const {
        const int available = tabsRight - Margin;
        return {Margin + index * available / 5, ToolbarTop,
                Margin + (index + 1) * available / 5, ToolbarTop + ToolbarHeight};
    }
};

ToolbarLayout LayoutToolbar(int width) {
    const bool narrow = width < 420;
    const int arrowWidth = narrow ? 18 : 22;
    const int todayWidth = narrow ? 26 : 32;
    const int refreshWidth = narrow ? 22 : 26;
    const int lockWidth = narrow ? 50 : 70;
    const int right = width - Margin;
    int left = right - lockWidth - 6 - refreshWidth - 2 * arrowWidth - todayWidth;
    ToolbarLayout layout{};
    layout.tabsRight = left - 6;
    auto take = [&](int size) {
        const RECT area{left, ToolbarTop, left + size, ToolbarTop + ToolbarHeight};
        left += size;
        return area;
    };
    layout.previous = take(arrowWidth);
    layout.today = take(todayWidth);
    layout.next = take(arrowWidth);
    layout.refresh = take(refreshWidth);
    left += 6;
    layout.lock = take(lockWidth);
    return layout;
}

int CompactRowHeight(int height) {
    // Preserve space for the selected day's title and at least one agenda item.
    return std::clamp((height - Margin - GridTop - 23 - 66) / 6, 27, 36);
}

struct CompactIndicatorLayout {
    int columns, rows, visible, hidden, badgeSlots;
};

CompactIndicatorLayout LayoutCompactIndicators(int width, int height, int count, int badgeWidth = 0) {
    const int columns = std::max(1, (width - 1) / 7);
    const int rows = std::max(1, (height - 22) / 6);
    const int capacity = columns * rows;
    const int badgeSlots = count > capacity ? std::min(columns, std::max(1, (badgeWidth + 9) / 7)) : 0;
    const int visible = std::min(count, capacity - badgeSlots);
    return {columns, rows, visible, count - visible, badgeSlots};
}

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
    bool interactionLocked = false;
    bool refreshAnimating = false;
    unsigned refreshFrame = 0;
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

void SyncRefreshAnimation(HWND window, View& view) {
    const bool active = refreshing.load();
    if (active == view.refreshAnimating) return;
    view.refreshFrame = 0;
    if (active) {
        view.refreshAnimating = SetTimer(window, RefreshAnimationTimerId, RefreshAnimationInterval, nullptr) != 0;
    } else {
        KillTimer(window, RefreshAnimationTimerId);
        view.refreshAnimating = false;
    }
}

void DrawRefreshIcon(HDC dc, HDC mask, const RECT& area, COLORREF color, HFONT font, unsigned frame) {
    const double angle = double(frame % RefreshAnimationFrames) * 6.283185307179586 / RefreshAnimationFrames;
    const FLOAT cosine = FLOAT(std::cos(angle)), sine = FLOAT(std::sin(angle));
    const FLOAT centerX = FLOAT(area.left + area.right) / 2;
    const FLOAT centerY = FLOAT(area.top + area.bottom) / 2;
    const XFORM rotation{cosine, sine, -sine, cosine,
        centerX - cosine * centerX + sine * centerY,
        centerY - sine * centerX - cosine * centerY};
    auto draw = [&](HDC target, COLORREF foreground) {
        if (!target) return;
        const int saved = SaveDC(target);
        if (!saved) return;
        IntersectClipRect(target, area.left, area.top, area.right, area.bottom);
        SetGraphicsMode(target, GM_ADVANCED);
        SetWorldTransform(target, &rotation);
        SetBkMode(target, TRANSPARENT);
        SetTextColor(target, foreground);
        SelectObject(target, font);
        RECT rect = area;
        DrawTextW(target, L"↻", 1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
        RestoreDC(target, saved);
    };
    draw(dc, color);
    draw(mask, RGB(255, 255, 255));
}

DWORD ClockStamp(const SYSTEMTIME& local) {
    return (DWORD(local.wDay) * 24 + local.wHour) * 60 + local.wMinute;
}

std::wstring HeaderClock(const SYSTEMTIME& local, year_month displayedMonth) {
    wchar_t clock[48]{};
    swprintf(clock, std::size(clock), L"%02u:%02u %ls %04d", unsigned(local.wHour),
        unsigned(local.wMinute), MonthNames[unsigned(displayedMonth.month()) - 1],
        int(displayedMonth.year()));
    return clock;
}

HFONT CreateHeaderFont(HDC dc, std::wstring_view caption, int pixelWidth, int dpi) {
    for (int size = 24; size >= 8; --size) {
        HFONT font = CreateFontW(-MulDiv(size, dpi, 96), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
            DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, L"Segoe UI");
        if (!font) continue;
        const auto previous = SelectObject(dc, font);
        SIZE extent{};
        const bool measured = GetTextExtentPoint32W(dc, caption.data(), int(caption.size()), &extent);
        SelectObject(dc, previous);
        if ((measured && extent.cx <= pixelWidth) || size == 8) return font;
        DeleteObject(font);
    }
    return nullptr;
}

struct EventCellLayout {
    int visible, hidden;
    bool counterOnOwnRow;
};

EventCellLayout LayoutEventCell(int count, int stored, bool counterFitsHeader) {
    int visible = std::min(count, stored);
    const bool counterOnOwnRow = count > visible && !counterFitsHeader && visible > 0;
    if (counterOnOwnRow) --visible;
    return {visible, count - visible, counterOnOwnRow};
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
    HFONT counterSmall = CreateFontW(-scale(7), 0, 0, 0, FW_NORMAL, 0, 0, 0,
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
            MonthNames[unsigned(selected.month()) - 1] + L" " +
            std::to_wstring(int(selected.year()));
    }
    if (view.mode == Mode::Week) {
        year_month_day start(weekStart), end(weekStart + days{6});
        caption = std::to_wstring(unsigned(start.day()));
        if (start.month() != end.month())
            caption += L" " + std::wstring(MonthNames[unsigned(start.month()) - 1]);
        caption += L"–" + std::to_wstring(unsigned(end.day())) + L" " +
            MonthNames[unsigned(end.month()) - 1];
    }
    SYSTEMTIME local{};
    GetLocalTime(&local);
    view.clockStamp = ClockStamp(local);
    if (view.config.showClock) caption = HeaderClock(local, view.month);
    HFONT captionFont = CreateHeaderFont(dc, caption, scale(width - 2 * Margin), view.dpi);
    label(caption, Margin, 6, width - 2 * Margin, 36, accent, captionFont ? captionFont : normal, true);

    const auto toolbar = LayoutToolbar(width);
    auto toolbarLabel = [&](std::wstring_view text, const RECT& area, COLORREF color, HFONT font) {
        label(text, area.left, area.top, area.right - area.left, area.bottom - area.top, color, font, true);
    };
    const auto controlsColor = view.interactionLocked ? muted : foreground;
    toolbarLabel(L"‹", toolbar.previous, controlsColor, heading);
    toolbarLabel(L"Hoy", toolbar.today, view.interactionLocked ? muted : accent, small);
    toolbarLabel(L"›", toolbar.next, controlsColor, heading);
    const bool busy = refreshing.load();
    const RECT refreshArea{scale(toolbar.refresh.left), scale(toolbar.refresh.top),
        scale(toolbar.refresh.right), scale(toolbar.refresh.bottom)};
    DrawRefreshIcon(dc, maskDc, refreshArea, busy || !view.interactionLocked ? accent : muted,
                    heading, busy ? view.refreshFrame : 0);
    const int checkX = toolbar.lock.right - 14;
    const auto lockColor = view.interactionLocked ? accent : muted;
    label(width < 420 ? L"Bloq." : L"Bloqueo", toolbar.lock.left, ToolbarTop,
          checkX - toolbar.lock.left - 2, ToolbarHeight, lockColor, width < 420 ? tiny : small);
    fill(checkX, ToolbarTop + 5, 14, 14, lockColor, 2);
    if (view.interactionLocked)
        label(L"✓", checkX, ToolbarTop + 3, 14, 18, background, tiny, true);
    else fill(checkX + 1, ToolbarTop + 6, 12, 12, background, 1);
    for (int index = 0; index < 5; ++index) {
        const auto area = toolbar.Tab(index);
        bool active = view.mode == Mode(index);
        if (active) fill(area.left + 2, ToolbarTop + ToolbarHeight, area.right - area.left - 4, 2, accent);
        SIZE extent{};
        SelectObject(dc, small);
        GetTextExtentPoint32W(dc, ModeNames[index], int(std::wcslen(ModeNames[index])), &extent);
        const bool full = extent.cx <= scale(area.right - area.left - 2);
        toolbarLabel(full ? ModeNames[index] : ShortModeNames[index], area,
                     active ? accent : muted, full ? small : tiny);
    }

    struct AgendaRow { int minute; std::wstring title, source; COLORREF color; bool holiday; };
    struct CalendarDay {
        std::vector<AgendaRow> entries;
        std::vector<COLORREF> colors;
        int count = 0;
        int eventCount = 0;
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
    const int bodyBottom = height - Margin;
    const bool hasAgenda = view.mode == Mode::Compact || view.mode == Mode::Week || view.mode == Mode::Day;
    const int compactRowHeight = CompactRowHeight(height);
    const auto compactCapacity = LayoutCompactIndicators((width - 2 * Margin) / 7, compactRowHeight, 0);
    const int gridHeight = view.mode == Mode::Compact ? 6 * compactRowHeight :
        view.mode == Mode::Week ? std::max(62, (bodyBottom - GridTop - 23) / 2) :
        std::max(0, bodyBottom - GridTop - 23);
    constexpr int EventRowHeight = 16;
    const int eventSlots = std::max(0,
        ((eventMonth ? gridHeight / (dayCount / 7) : gridHeight) - 22) / EventRowHeight);
    const int currentYear = int(year_month_day(today).year());
    const auto sources = SourcesForYear(view.config, currentYear);
    {
        std::lock_guard lock(dataMutex);
        for (const auto& source : sources) {
            auto feed = std::ranges::find(feeds, source.url, &Feed::url);
            if (feed == feeds.end()) continue;
            for (const auto& event : feed->events) {
                for (auto day = std::max(first, event.start);
                     day < std::min(first + days(dayCount), event.end); day += days{1}) {
                    auto& cell = calendar[(day - first).count()];
                    cell.holiday |= source.holiday;
                    ++cell.count;
                    if (!source.holiday) {
                        ++cell.eventCount;
                        if (view.mode == Mode::Compact &&
                            int(cell.colors.size()) < compactCapacity.columns * compactCapacity.rows)
                            cell.colors.push_back(source.color);
                    }
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
                if (day == view.selected) fill(x + 2, y, cellWidth - 4, bottom - y - 1, selectionColor);
                label(std::to_wstring(unsigned(date.day())), x, y, cellWidth, 22,
                      day == view.selected ? background : number, normal, true);
                auto indicators = LayoutCompactIndicators((width - 2 * Margin) / 7,
                    bottom - y, cell.eventCount);
                std::wstring more;
                if (indicators.hidden) {
                    for (;;) {
                        more = L"+" + std::to_wstring(indicators.hidden);
                        SIZE extent{};
                        SelectObject(dc, counterSmall);
                        GetTextExtentPoint32W(dc, more.data(), int(more.size()), &extent);
                        const int badgeWidth = int((extent.cx * 96 + view.dpi - 1) / view.dpi);
                        const auto next = LayoutCompactIndicators((width - 2 * Margin) / 7,
                            bottom - y, cell.eventCount, badgeWidth);
                        if (next.visible == indicators.visible) break;
                        indicators = next;
                    }
                }
                for (int row = 0; row < indicators.rows; ++row) {
                    const int count = std::clamp(indicators.visible - row * indicators.columns, 0, indicators.columns);
                    const bool badge = indicators.hidden && row == indicators.rows - 1;
                    const int slots = count + (badge ? indicators.badgeSlots : 0);
                    if (!slots) continue;
                    int dotX = x + (cellWidth - slots * 7 + 3) / 2;
                    for (int column = 0; column < count; ++column) {
                        const auto color = cell.colors[row * indicators.columns + column];
                        fill(dotX, y + 22 + row * 6, 4, 3,
                             day == view.selected && color == selectionColor ? background : color);
                        dotX += 7;
                    }
                    if (badge) {
                        const int badgeY = y + 20 + row * 6;
                        label(more, dotX, badgeY, indicators.badgeSlots * 7 - 3,
                              std::min(10, bottom - badgeY), day == view.selected ? background : muted,
                              counterSmall, true);
                    }
                }
            } else {
                if (!eventMonth) {
                    fill(x, y, cellWidth, 1, muted);
                    fill(x, y, 1, bottom - y, muted);
                    if (index % 7 == 6) fill(right - 1, y, 1, bottom - y, muted);
                }
                const int stored = int(cell.entries.size());
                SIZE counterSize{};
                if (cell.count > stored) {
                    const auto text = L"+" + std::to_wstring(cell.count - stored);
                    SelectObject(dc, tiny);
                    GetTextExtentPoint32W(dc, text.data(), int(text.size()), &counterSize);
                }
                const auto layout = LayoutEventCell(cell.count, stored,
                    counterSize.cx <= scale(cellWidth - 28));
                const int visible = layout.visible;
                const int centeredDateX = x + (cellWidth - 20) / 2;
                const bool counterFitsCentered = counterSize.cx <= scale(right - centeredDateX - 25);
                const int dateX = eventMonth &&
                    (!layout.hidden || layout.counterOnOwnRow || counterFitsCentered)
                    ? centeredDateX : x + 3;
                if (day == view.selected) {
                    auto selection = dayColor(day, cell.holiday, accent);
                    if (eventMonth) fill(dateX, y + 2, 20, 20, selection, 20);
                    else fill(dateX, y + 3, 22, 17, selection);
                }
                label(std::to_wstring(unsigned(date.day())), dateX, y + 2, eventMonth ? 20 : 22, 19,
                      day == view.selected ? background : number, eventMonth ? normal : small, true);
                if (layout.hidden) {
                    const auto text = L"+" + std::to_wstring(layout.hidden);
                    const int badgeX = layout.counterOnOwnRow ? x + 3 : dateX + 22;
                    const int badgeY = layout.counterOnOwnRow
                        ? y + 22 + visible * EventRowHeight : y + 2;
                    const int badgeWidth = right - badgeX - 3;
                    const int badgeHeight = layout.counterOnOwnRow ? EventRowHeight - 2 : 19;
                    SelectObject(dc, tiny);
                    GetTextExtentPoint32W(dc, text.data(), int(text.size()), &counterSize);
                    const auto font = counterSize.cx <= scale(badgeWidth) ? tiny : counterSmall;
                    label(text, badgeX, badgeY, badgeWidth, badgeHeight, muted, font, true);
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
    SelectObject(dc, oldFont);
    if (maskDc) SelectObject(maskDc, oldMaskFont);
    DeleteObject(heading);
    if (captionFont) DeleteObject(captionFont);
    DeleteObject(tiny);
    DeleteObject(counterSmall);
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
            SyncRefreshAnimation(window, *view);
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        case WM_TIMER: {
            if (wParam == RefreshAnimationTimerId) {
                SyncRefreshAnimation(window, *view);
                if (view->refreshAnimating)
                    view->refreshFrame = (view->refreshFrame + 1) % RefreshAnimationFrames;
                const auto area = LayoutToolbar(view->width).refresh;
                const RECT pixels{view->Scale(area.left), view->Scale(area.top),
                    view->Scale(area.right), view->Scale(area.bottom)};
                InvalidateRect(window, &pixels, FALSE);
                return 0;
            }
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
            if (!view->interactionLocked && point.y >= 0 && point.y < view->Scale(ToolbarTop))
                return HTCAPTION;
            return HTCLIENT;
        }
        case WM_LBUTTONUP: {
            int x = MulDiv(GET_X_LPARAM(lParam), 96, view->dpi);
            int y = MulDiv(GET_Y_LPARAM(lParam), 96, view->dpi);
            int width = view->width;
            const auto toolbar = LayoutToolbar(width);
            const POINT point{x, y};
            if (PtInRect(&toolbar.lock, point)) {
                view->interactionLocked = !view->interactionLocked;
                Wh_SetIntValue(L"interactionLocked", view->interactionLocked);
                InvalidateRect(window, nullptr, FALSE);
                return 0;
            }
            if (view->interactionLocked) return 0;
            for (int index = 0; index < 5; ++index) {
                const auto area = toolbar.Tab(index);
                if (PtInRect(&area, point)) {
                    view->ChangeMode(Mode(index));
                    InvalidateRect(window, nullptr, FALSE);
                    return 0;
                }
            }
            if (PtInRect(&toolbar.previous, point)) view->Navigate(-1);
            else if (PtInRect(&toolbar.today, point)) view->Select(Today());
            else if (PtInRect(&toolbar.next, point)) view->Navigate(1);
            else if (PtInRect(&toolbar.refresh, point)) SetEvent(wakeEvent);
            else if (view->agendaTop && y >= view->agendaTop && y < view->agendaTop + 27) {
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
            if (view->interactionLocked) return 0;
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
    view.interactionLocked = Wh_GetIntValue(L"interactionLocked", 0) != 0;
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
        SetTimer(window, ClockTimerId, 1000, nullptr);
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
