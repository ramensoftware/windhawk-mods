// ==WindhawkMod==
// @id              crazy-time
// @name            Crazy Time
// @name:pt-BR      Crazy Time
// @description     Schedule shutdown, sleep or hibernate from a tray icon, with warnings and weekly repeats.
// @description:pt-BR Agende desligar, suspender ou hibernar pelo ícone da barra, com avisos e repetição semanal.
// @version         0.6.1
// @author          caikx11
// @github          https://github.com/caikx11
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lshell32 -lcomctl32 -lgdi32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Crazy Time

A minimalist scheduler for shutdown, sleep and hibernate, living in the
notification area. Português abaixo.

![Crazy Time panel](https://raw.githubusercontent.com/caikx11/Crazy-timer/be7f37d596ba221e65c336a6472a9bd11677c073/docs/images/crazy-time-panel.jpg)

Screenshot of the standalone preview using simulated actions. The mod itself
performs real power actions as described below.

> **This mod really shuts down, sleeps or hibernates your computer** when a
> schedule fires. Save your work first. A warning notification appears before
> the action, and you can cancel or postpone an occurrence from the panel.

> **Windows 10 is untested.** It was developed and tested on Windows 11 only.
> It is expected to work on Windows 10 (version 1809 or newer for the dark
> theme of the controls), but this has not been verified. Please report issues.

## What it does

- Schedule an action **by duration** (1 minute to 7 days), at a **date and time**,
  or **repeating on chosen weekdays**.
- Keep several schedules at once: edit, pause, cancel one occurrence,
  postpone it by 10 minutes, or delete.
- Countdown warnings at 30, 15, 10, 5 and 1 minute, with optional sound.
- Schedules at the same instant are detected and refused.
- Optional per-schedule "force apps to close" (shutdown only), off by default,
  with a data-loss warning.
- Actions the computer cannot perform are explained instead of scheduled.
- Portuguese and English; automatic, light or dark theme; high contrast.

## How to use

1. Enable the mod. A tray icon appears; click it to open the panel.
2. Pick the action and how it is scheduled, then press **Add schedule**.
3. Click the tray icon to open the panel again. Closing the panel keeps the
   countdown running; right-click the icon or use **Options** to exit.
   **Exit** stops scheduling until Windhawk restarts or the mod is toggled off
   and on again; Windhawk still lists the mod as enabled.
4. Disabling the mod stops everything and removes the icon. Schedules are saved
   per user; occurrences that expired while the mod was off are not run.

Schedules are stored in Windhawk's mod storage, in a file named with your user
SID. On first use, an existing legacy LocalAppData schedule file is copied there
without overwriting an existing destination. The legacy file is kept as a backup.
Removing the mod deletes Windhawk's managed storage; the legacy backup remains.

## Português

Agendador minimalista de desligar, suspender e hibernar, na área de notificação.

**Este mod realmente desliga, suspende ou hiberna o computador** quando um
agendamento dispara. Salve seu trabalho antes. Uma notificação de aviso aparece
antes da ação e você pode cancelar ou adiar a ocorrência pelo painel.

**Windows 10 em teste.** Desenvolvido e testado apenas no Windows 11. Deve
funcionar no Windows 10 (versão 1809 ou mais nova para o tema escuro dos
controles), mas isso não foi verificado. Relate problemas.

**Como usar:** ative o mod, escolha a ação e o tipo de agendamento (por
duração, data e hora, ou repetição semanal) e clique em **Adicionar
agendamento**. Clique no ícone da barra para reabrir o painel; fechar o
painel não interrompe a contagem. Desativar o mod encerra tudo.
**Sair** interrompe os agendamentos até reiniciar o Windhawk ou desativar e
reativar o mod; ele ainda aparece como ativado no Windhawk.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- soundEnabled: false
  $name: Play notification sounds
  $name:pt-BR: Reproduzir sons de notificação
  $description: Play a sound when warnings and countdown notifications appear.
  $description:pt-BR: Reproduzir um som quando os avisos e notificações de contagem regressiva aparecerem.
*/
// ==/WindhawkModSettings==
/*
MIT License

Copyright (c) 2026 caikx11

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
// Source: src/scheduler.hpp

#include <cstdint>
#include <limits>
#include <optional>
#include <string_view>

namespace power_timer {

enum class Action { Shutdown, Sleep, Hibernate };
enum class State { Idle, Scheduled, Warning, Requested, Cancelled, Expired };

// Pure state machine. Times are monotonic milliseconds from a caller-owned clock.
// It emits an intention only; it never performs a system power operation.
class Scheduler {
public:
    static constexpr std::uint64_t min_minutes = 1;
    static constexpr std::uint64_t max_minutes = 10080;
    static constexpr std::uint64_t warning_ms = 60000;
    static constexpr std::uint64_t max_lateness_ms = 5000;

    [[nodiscard]] static std::optional<std::uint64_t>
    parse_duration(std::wstring_view text) noexcept {
        constexpr std::wstring_view whitespace = L" \t\r\n\v\f";
        const auto first = text.find_first_not_of(whitespace);
        if (first == std::wstring_view::npos) return std::nullopt;
        const auto last = text.find_last_not_of(whitespace);
        text = text.substr(first, last - first + 1);
        if (text.empty()) return std::nullopt;
        std::uint64_t minutes = 0;
        for (const auto character : text) {
            if (character < L'0' || character > L'9') return std::nullopt;
            const auto digit = static_cast<std::uint64_t>(character - L'0');
            if (minutes > (max_minutes - digit) / 10) return std::nullopt;
            minutes = minutes * 10 + digit;
        }
        if (minutes < min_minutes) return std::nullopt;
        return minutes;
    }

    [[nodiscard]] bool schedule_text(Action action, std::wstring_view text,
                                     std::uint64_t now_ms) noexcept {
        const auto minutes = parse_duration(text);
        return minutes && schedule(action, *minutes, now_ms);
    }

    // Invalid input leaves any existing schedule intact.
    [[nodiscard]] bool schedule(Action action, std::uint64_t minutes,
                                std::uint64_t now_ms) noexcept {
        if (!valid_action(action) || minutes < min_minutes || minutes > max_minutes)
            return false;
        const auto duration_ms = minutes * 60000;
        if (now_ms > std::numeric_limits<std::uint64_t>::max() - duration_ms)
            return false;
        action_ = action;
        deadline_ = now_ms + duration_ms;
        state_ = duration_ms <= warning_ms ? State::Warning : State::Scheduled;
        return true;
    }

    [[nodiscard]] std::optional<Action> tick(std::uint64_t now_ms) noexcept {
        if (!active()) return std::nullopt;
        if (now_ms >= deadline_) {
            if (now_ms - deadline_ > max_lateness_ms) {
                state_ = State::Expired;
                return std::nullopt;
            }
            state_ = State::Requested;
            return action_;
        }
        state_ = deadline_ - now_ms <= warning_ms ? State::Warning : State::Scheduled;
        return std::nullopt;
    }

    // A resume notification must be delivered before a timer tick after waking.
    // Never issue a missed power action merely because the computer woke up.
    void on_resume(std::uint64_t now_ms) noexcept {
        if (!active()) return;
        if (now_ms >= deadline_) {
            state_ = State::Expired;
        } else {
            state_ = deadline_ - now_ms <= warning_ms ? State::Warning : State::Scheduled;
        }
    }

    void cancel() noexcept {
        if (active()) state_ = State::Cancelled;
    }

    [[nodiscard]] State state() const noexcept { return state_; }
    [[nodiscard]] bool active() const noexcept {
        return state_ == State::Scheduled || state_ == State::Warning;
    }
    [[nodiscard]] std::uint64_t deadline_ms() const noexcept { return deadline_; }
    [[nodiscard]] Action action() const noexcept { return action_; }
    [[nodiscard]] std::uint64_t remaining_ms(std::uint64_t now_ms) const noexcept {
        return active() && now_ms < deadline_ ? deadline_ - now_ms : 0;
    }

private:
    static constexpr bool valid_action(Action action) noexcept {
        return action == Action::Shutdown || action == Action::Sleep ||
               action == Action::Hibernate;
    }
    State state_ = State::Idle;
    Action action_ = Action::Shutdown;
    std::uint64_t deadline_ = 0;
};

} // namespace power_timer

// Source: src/calendar.hpp

#include <chrono>
#include <cstdint>
#include <optional>
#include <vector>

namespace power_timer {

struct CivilTime {
    int year;
    unsigned month, day, hour, minute;
    bool operator==(const CivilTime&) const = default;
};

class Calendar {
public:
    virtual ~Calendar() = default;
    virtual std::optional<CivilTime> local(std::uint64_t utc_ms) const = 0;
    // nullopt = adapter failure; empty vector = skipped local time (DST gap).
    virtual std::optional<std::vector<std::uint64_t>> resolve(CivilTime civil) const = 0;
};

inline bool valid_civil(CivilTime civil) {
    using namespace std::chrono;
    return civil.year >= 1970 && civil.year <= 9999 &&
           civil.month >= 1 && civil.month <= 12 && civil.day >= 1 && civil.day <= 31 &&
           civil.hour < 24 && civil.minute < 60 &&
           year_month_day{year{civil.year}, month{civil.month}, day{civil.day}}.ok();
}

inline std::uint64_t civil_key(CivilTime civil) {
    if (!valid_civil(civil)) return 0;
    return static_cast<std::uint64_t>(civil.year) * 100000000ULL +
           civil.month * 1000000ULL + civil.day * 10000ULL +
           civil.hour * 100ULL + civil.minute;
}

inline std::optional<CivilTime> civil_from_key(std::uint64_t key) {
    if (key / 100000000ULL < 1970 || key / 100000000ULL > 9999) return std::nullopt;
    const CivilTime civil{
        static_cast<int>(key / 100000000ULL),
        static_cast<unsigned>((key / 1000000ULL) % 100),
        static_cast<unsigned>((key / 10000ULL) % 100),
        static_cast<unsigned>((key / 100ULL) % 100),
        static_cast<unsigned>(key % 100)};
    if (!valid_civil(civil)) return std::nullopt;
    return civil;
}

inline std::optional<CivilTime> next_day(CivilTime civil) {
    using namespace std::chrono;
    if (!valid_civil(civil)) return std::nullopt;
    const auto next = sys_days{year{civil.year}/month{civil.month}/day{civil.day}} + days{1};
    const year_month_day date{next};
    const CivilTime result{int(date.year()), unsigned(date.month()), unsigned(date.day()),
                           civil.hour, civil.minute};
    if (!valid_civil(result)) return std::nullopt;
    return result;
}

inline unsigned weekday(CivilTime civil) {
    using namespace std::chrono;
    if (!valid_civil(civil)) return 0;
    return std::chrono::weekday{sys_days{year{civil.year}/month{civil.month}/day{civil.day}}}.c_encoding();
}

} // namespace power_timer

// Source: src/schedule_types.hpp

#include <vector>

namespace power_timer {

struct ClockSample { std::uint64_t utc_ms, monotonic_ms; };
enum class OccurrenceState { Scheduled, Cancelled, Expired, Uncertain, Completed };
enum class ScheduleKind { Duration, Once, Weekly };
struct ScheduleRecord {
    std::uint64_t id, occurrence_id;
    Action action;
    std::uint64_t minutes, due_utc_ms;
    bool paused;
    OccurrenceState state;
    ScheduleKind kind = ScheduleKind::Duration;
    CivilTime civil{};
    unsigned weekdays = 0, minute_of_day = 0;
    std::uint64_t civil_high_water = 0;
    std::uint64_t last_occurrence_id = 0;
    OccurrenceState last_state = OccurrenceState::Scheduled;
    std::uint64_t conflict_with = 0;
    bool force_shutdown = false;
    // Weekly only: one-off postponement of the current occurrence; the rule's
    // usual time (civil/minute_of_day) is never changed. 0 means none.
    std::uint64_t postponed_utc_ms = 0;
};
// The instant at which the current occurrence is actually due.
inline std::uint64_t effective_due_utc_ms(const ScheduleRecord& record) {
    return record.postponed_utc_ms > record.due_utc_ms ? record.postponed_utc_ms : record.due_utc_ms;
}
struct ScheduleSnapshot {
    std::uint64_t next_id = 1, next_occurrence_id = 1;
    std::vector<ScheduleRecord> records;
    bool sound_enabled = false;
    unsigned theme = 0; // 0=automatic, 1=light, 2=dark
    unsigned locale = 0; // 0=pt-BR, 1=English
};
enum class LoadStatus { Missing, Loaded, Invalid, Unavailable };
struct LoadResult { LoadStatus status; ScheduleSnapshot snapshot; };
class ScheduleStorage {
public:
    virtual ~ScheduleStorage() = default;
    virtual LoadResult load() = 0;
    virtual bool save(const ScheduleSnapshot&) = 0;
};
struct Delivery { std::uint64_t id, occurrence_id; Action action; bool force = false; };
enum class BookError { None, InvalidInput, NotFound, Storage, InvalidData, NotReady, Calendar, Conflict };

} // namespace power_timer

// Source: src/schedule_validation.hpp

#include <unordered_set>

namespace power_timer {

inline constexpr std::size_t max_schedule_records = 1000;

inline bool valid_action(Action action) {
    return action == Action::Shutdown || action == Action::Sleep || action == Action::Hibernate;
}
inline bool valid_occurrence_state(OccurrenceState state) {
    return state == OccurrenceState::Scheduled || state == OccurrenceState::Cancelled ||
        state == OccurrenceState::Expired || state == OccurrenceState::Uncertain || state == OccurrenceState::Completed;
}

inline bool valid_snapshot(const ScheduleSnapshot& snapshot) {
    if (snapshot.next_id == 0 || snapshot.next_occurrence_id == 0 ||
        snapshot.records.size() > max_schedule_records) return false;
    // sound_enabled is a bool, no validation needed beyond type safety
    std::unordered_set<std::uint64_t> ids, occurrences;
    for (const auto& record : snapshot.records) {
        if (record.id == 0 || record.id >= snapshot.next_id || record.occurrence_id == 0 ||
            record.occurrence_id >= snapshot.next_occurrence_id || !ids.insert(record.id).second ||
            !occurrences.insert(record.occurrence_id).second || !valid_action(record.action) ||
            !valid_occurrence_state(record.state) || !valid_occurrence_state(record.last_state) ||
            (record.force_shutdown && record.action != Action::Shutdown) ||
            record.conflict_with >= snapshot.next_id || record.conflict_with == record.id) return false;
        if (record.postponed_utc_ms &&
            (record.kind != ScheduleKind::Weekly || record.postponed_utc_ms >= 253402300800000ULL)) return false;
        if (record.kind == ScheduleKind::Duration) {
            if (record.minutes < Scheduler::min_minutes || record.minutes > Scheduler::max_minutes ||
                !(record.civil == CivilTime{})) return false;
        } else if (record.kind == ScheduleKind::Once || record.kind == ScheduleKind::Weekly) {
            if (record.minutes != 0 || !valid_civil(record.civil) || record.due_utc_ms >= 253402300800000ULL)
                return false;
        } else return false;
        if (record.kind == ScheduleKind::Weekly) {
            if (!record.weekdays || record.weekdays > 127 || record.minute_of_day >= 1440 ||
                record.civil.hour * 60 + record.civil.minute != record.minute_of_day ||
                !(record.weekdays & (1U << weekday(record.civil))) ||
                record.state != OccurrenceState::Scheduled ||
                !civil_from_key(record.civil_high_water) ||
                record.civil_high_water >= civil_key(record.civil)) return false;
            if (record.last_occurrence_id) {
                if (record.last_occurrence_id >= record.occurrence_id ||
                    !occurrences.insert(record.last_occurrence_id).second ||
                    record.last_state == OccurrenceState::Scheduled) return false;
            } else if (record.last_state != OccurrenceState::Scheduled) return false;
        } else if (record.weekdays || record.minute_of_day || record.civil_high_water ||
                   record.last_occurrence_id || record.last_state != OccurrenceState::Scheduled) return false;
    }
    return true;
}

} // namespace power_timer

// Source: src/schedule_codec.hpp
#include <charconv>
#include <limits>
#include <string>

namespace power_timer {
inline constexpr std::size_t max_schedule_file_bytes = 1024 * 1024;
// Canonical ASCII: exact spaces/newlines, decimal unsigned integers, no leading
// zeroes, and no ignored fields/trailing bytes. Versions 1, 2, and 3 remain readable;
// every successful write uses version 5.
inline std::string encode_snapshot(const ScheduleSnapshot& snapshot) {
    if (!valid_snapshot(snapshot)) return {};
    std::string data = "POWER_TIMER 5\n" + std::to_string(snapshot.next_id) + " " +
        std::to_string(snapshot.next_occurrence_id) + " " + std::to_string(snapshot.records.size()) + " " +
        (snapshot.sound_enabled ? "1" : "0") + "\n" +
        std::to_string(snapshot.theme) + "\n" +
        std::to_string(snapshot.locale) + "\n";
    for (const auto& record : snapshot.records) {
        data += std::to_string(record.id) + " " + std::to_string(record.occurrence_id) + " " +
            std::to_string(static_cast<unsigned>(record.action)) + " " + std::to_string(record.minutes) + " " +
            std::to_string(record.due_utc_ms) + " " + (record.paused ? "1" : "0") + " " +
            std::to_string(static_cast<unsigned>(record.state)) + " " +
            std::to_string(static_cast<unsigned>(record.kind)) + " " +
            std::to_string(record.civil.year) + " " + std::to_string(record.civil.month) + " " +
            std::to_string(record.civil.day) + " " + std::to_string(record.civil.hour) + " " +
            std::to_string(record.civil.minute) + " " + std::to_string(record.weekdays) + " " +
            std::to_string(record.minute_of_day) + " " + std::to_string(record.civil_high_water) + " " +
            std::to_string(record.last_occurrence_id) + " " +
            std::to_string(static_cast<unsigned>(record.last_state)) + " " +
            std::to_string(record.conflict_with) + " " + (record.force_shutdown ? "1" : "0") + " " +
            std::to_string(record.postponed_utc_ms) + "\n";
    }
    return data.size() <= max_schedule_file_bytes ? data : std::string{};
}

inline std::optional<ScheduleSnapshot> decode_snapshot(std::string_view data) {
    constexpr std::string_view version_1 = "POWER_TIMER 1\n";
    constexpr std::string_view version_2 = "POWER_TIMER 2\n";
    constexpr std::string_view version_3 = "POWER_TIMER 3\n";
    constexpr std::string_view version_4 = "POWER_TIMER 4\n";
    constexpr std::string_view version_5 = "POWER_TIMER 5\n";
    if (data.size() > max_schedule_file_bytes) return std::nullopt;
    const bool legacy = data.starts_with(version_1);
    const bool v2 = data.starts_with(version_2);
    const bool v3 = data.starts_with(version_3);
    const bool v5 = data.starts_with(version_5);
    const bool v4 = data.starts_with(version_4) || v5; // v5 is v4 plus a per-record postponement
    if (!legacy && !v2 && !v3 && !v4) return std::nullopt;
    data.remove_prefix(legacy ? version_1.size() : v2 ? version_2.size() : v3 ? version_3.size() : version_4.size());
    auto number = [&data](char delimiter) -> std::optional<std::uint64_t> {
        const auto end = data.find(delimiter);
        if (end == std::string_view::npos || end == 0) return std::nullopt;
        const auto token = data.substr(0, end);
        if (token.size() > 1 && token.front() == '0') return std::nullopt;
        if (token.find_first_not_of("0123456789") != std::string_view::npos) return std::nullopt;
        std::uint64_t value = 0;
        const auto result = std::from_chars(token.data(), token.data() + token.size(), value);
        if (result.ec != std::errc{} || result.ptr != token.data() + token.size()) return std::nullopt;
        data.remove_prefix(end + 1);
        return value;
    };
    const auto next_id = number(' '), next_occurrence = number(' '), count = number(v4 ? ' ' : '\n');
    std::optional<std::uint64_t> sound_enabled{};
    if (v4) {
        sound_enabled = number('\n');
        if (!sound_enabled || *sound_enabled > 1) return std::nullopt;
    }
    if (!next_id || !next_occurrence || !count || *count > max_schedule_records) return std::nullopt;
    ScheduleSnapshot snapshot{*next_id, *next_occurrence, {}};
    if (sound_enabled) snapshot.sound_enabled = *sound_enabled == 1;
    // Skip any leading newlines before the theme
    while (!data.empty() && data[0] == '\n') {
        data.remove_prefix(1);
    }
    // Early version-4 files (written before theme/locale existed) carry no such
    // lines; the first record line has spaces, a theme line is a single number.
    // Version 5 always has them.
    const auto first_line = data.substr(0, data.find('\n'));
    const bool has_theme_lines = v5 || (v4 && !data.empty() && first_line.find(' ') == std::string_view::npos);
    if (has_theme_lines) {
        auto theme_val = number('\n');
        if (!theme_val || *theme_val > 2) return std::nullopt;
        snapshot.theme = *theme_val;
        auto locale_val = number('\n');
        if (!locale_val || *locale_val > 1) return std::nullopt;
        snapshot.locale = *locale_val;
    }
    for (std::uint64_t i = 0; i < *count; ++i) {
        const auto id = number(' '), occurrence = number(' '), action = number(' '), minutes = number(' '),
            due = number(' '), paused = number(' '), state = number(legacy ? '\n' : ' ');
        if (!id || !occurrence || !action || !minutes || !due || !paused || !state || *paused > 1 || *action > 2 || *state > 4) return std::nullopt;
        ScheduleRecord record{*id, *occurrence, static_cast<Action>(*action), *minutes,
            *due, *paused == 1, static_cast<OccurrenceState>(*state)};
        if (!legacy) {
            const auto kind = number(' '), year = number(' '), month = number(' '), day = number(' '),
                hour = number(' '), minute = number(' '), weekdays = number(' '), minute_of_day = number(' '),
                high_water = number(' '), last_id = number(' '), last_state = number((v3 || v4) ? ' ' : '\n');
            if (!kind || !year || !month || !day || !hour || !minute || !weekdays || !minute_of_day ||
                !high_water || !last_id || !last_state || *kind > 2 || *last_state > 4 ||
                *year > static_cast<std::uint64_t>(std::numeric_limits<int>::max()) ||
                *month > std::numeric_limits<unsigned>::max() || *day > std::numeric_limits<unsigned>::max() ||
                *hour > std::numeric_limits<unsigned>::max() || *minute > std::numeric_limits<unsigned>::max() ||
                *weekdays > std::numeric_limits<unsigned>::max() ||
                *minute_of_day > std::numeric_limits<unsigned>::max()) return std::nullopt;
            record.kind = static_cast<ScheduleKind>(*kind);
            record.civil = {static_cast<int>(*year), static_cast<unsigned>(*month),
                static_cast<unsigned>(*day), static_cast<unsigned>(*hour), static_cast<unsigned>(*minute)};
            record.weekdays = static_cast<unsigned>(*weekdays);
            record.minute_of_day = static_cast<unsigned>(*minute_of_day);
            record.civil_high_water = *high_water;
            record.last_occurrence_id = *last_id;
            record.last_state = static_cast<OccurrenceState>(*last_state);
            if (v3 || v4) {
                const auto conflict_with = number(' '), force_shutdown = number(v5 ? ' ' : '\n');
                if (!conflict_with || !force_shutdown || *force_shutdown > 1) return std::nullopt;
                if (v5) {
                    const auto postponed = number('\n');
                    if (!postponed) return std::nullopt;
                    record.postponed_utc_ms = *postponed;
                }
                record.conflict_with = *conflict_with;
                record.force_shutdown = *force_shutdown == 1;
            }
        }
        snapshot.records.push_back(record);
    }
    if (!data.empty() || !valid_snapshot(snapshot)) return std::nullopt;
    return snapshot;
}
} // namespace power_timer

// Source: src/schedule_book.hpp

#include <map>
#include <algorithm>

namespace power_timer {

// The host serializes commands, resume notifications and timer delivery.
// This module emits intentions only and never calls Windows power APIs.
class ScheduleBook {
public:
    explicit ScheduleBook(ScheduleStorage& storage) : storage_(storage) {}
    ScheduleBook(ScheduleStorage& storage, Calendar& calendar) : storage_(storage), calendar_(&calendar) {}

    bool initialize(ClockSample now) {
        ready_ = false;
        snapshot_ = {};
        deadlines_.clear();
        const auto loaded = storage_.load();
        if (loaded.status == LoadStatus::Unavailable) return fail(BookError::Storage);
        if (loaded.status != LoadStatus::Missing &&
            (loaded.status != LoadStatus::Loaded || !valid_snapshot(loaded.snapshot)))
            return fail(BookError::InvalidData);
        for (const auto& record : loaded.snapshot.records) {
            if (record.kind == ScheduleKind::Duration && record.state == OccurrenceState::Scheduled && record.due_utc_ms > now.utc_ms &&
                record.due_utc_ms - now.utc_ms > maximum - now.monotonic_ms)
                return fail(BookError::InvalidData);
        }
        snapshot_ = loaded.status == LoadStatus::Missing ? ScheduleSnapshot{} : loaded.snapshot;
        for (const auto& record : snapshot_.records) {
            if (record.kind == ScheduleKind::Duration && record.state == OccurrenceState::Scheduled)
                deadlines_[record.id] = now.monotonic_ms +
                    (record.due_utc_ms > now.utc_ms ? record.due_utc_ms - now.utc_ms : 0);
        }
        ready_ = true;
        return reconcile(now);
    }

    bool reconcile(ClockSample now) {
        if (!ready_) return false;
        auto candidate = snapshot_;
        bool changed = false;
        for (auto& record : candidate.records) {
            if (!calendar_available(record, now)) { ready_ = false; return false; }
            if (record.kind == ScheduleKind::Weekly) {
                const auto before = record;
                if (!refresh_weekly(candidate, record, now)) { ready_ = false; return false; }
                changed = changed || before.due_utc_ms != record.due_utc_ms ||
                    before.occurrence_id != record.occurrence_id || before.civil_high_water != record.civil_high_water;
                continue;
            }
            if (record.state != OccurrenceState::Scheduled) continue;
            const auto remaining = remaining_ms(record.id, now);
            if (remaining == 0) {
                record.state = OccurrenceState::Expired;
                changed = true;
            } else if (record.kind == ScheduleKind::Duration) {
                // Checkpoint civil-clock changes while retaining the session's
                // elapsed-time deadline. A restart restores this durable anchor.
                if (now.utc_ms > maximum - remaining) {
                    ready_ = false;
                    return fail(BookError::InvalidData);
                }
                const auto due = now.utc_ms + remaining;
                if (record.due_utc_ms != due) {
                    record.due_utc_ms = due;
                    changed = true;
                }
            }
        }
        if (!update_conflicts(candidate, now, changed)) { ready_ = false; return false; }
        if (changed && !commit(candidate)) { ready_ = false; return false; }
        error_ = BookError::None;
        return true;
    }

    std::vector<Delivery> tick(ClockSample now) {
        if (!ready_) return {};
        auto candidate = snapshot_;
        std::vector<Delivery> deliveries;
        bool changed = false;
        std::unordered_set<std::uint64_t> replanned;
        // Refresh every rule first: a later row may move onto an earlier row's
        // deadline. No intent may be produced until the complete set is checked.
        for (auto& record : candidate.records) {
            if (!calendar_available(record, now)) { ready_ = false; return {}; }
            if (record.kind != ScheduleKind::Weekly) continue;
            const auto values = calendar_->resolve(record.civil);
            if (!values) { fail(BookError::Calendar); ready_ = false; return {}; }
            if (values->empty() || values->front() != record.due_utc_ms) {
                if (values->empty()) {
                    if (!refresh_weekly(candidate, record, now)) { ready_ = false; return {}; }
                } else record.due_utc_ms = values->front();
                changed = true;
                replanned.insert(record.id);
            }
        }
        if (!update_conflicts(candidate, now, changed)) { ready_ = false; return {}; }
        for (auto& record : candidate.records) {
            if (record.conflict_with) continue;
            if (replanned.contains(record.id)) {
                if (!refresh_weekly(candidate, record, now)) { ready_ = false; return {}; }
                continue;
            }
            if (record.state != OccurrenceState::Scheduled || remaining_ms(record.id, now) != 0) continue;
            changed = true;
            const auto late = record.kind == ScheduleKind::Duration
                ? now.monotonic_ms - deadlines_.at(record.id) : now.utc_ms - effective_due_utc_ms(record);
            const bool deliver = !record.paused && late <= 5000;
            record.state = deliver ? OccurrenceState::Uncertain : OccurrenceState::Expired;
            if (deliver) deliveries.push_back({record.id, record.occurrence_id, record.action, record.force_shutdown});
            if (record.kind == ScheduleKind::Weekly) {
                consume_weekly(record, record.state);
                if (!advance_weekly(candidate, record, now)) { ready_ = false; return {}; }
            }
        }
        if (changed && !commit(candidate)) { ready_ = false; return {}; }
        return deliveries;
    }

    // Explicit per-schedule opt-in; only meaningful for Shutdown. No global default.
    bool set_force_shutdown(std::uint64_t id, bool force) {
        if (!ready_) return false;
        auto candidate = snapshot_;
        auto record = find(candidate, id);
        if (record == candidate.records.end()) return fail(BookError::NotFound);
        if (force && record->action != Action::Shutdown) return fail(BookError::InvalidInput);
        if (record->force_shutdown == force) { error_ = BookError::None; return true; }
        record->force_shutdown = force;
        if (!commit(candidate)) { ready_ = false; return false; }
        return true;
    }

    bool complete(Delivery delivery) {
        if (!ready_) return false;
        auto candidate = snapshot_;
        auto record = find(candidate, delivery.id);
        if (record == candidate.records.end()) return fail(BookError::NotFound);
        const bool weekly = record->kind == ScheduleKind::Weekly;
        const auto occurrence_id = weekly ? record->last_occurrence_id : record->occurrence_id;
        auto& result = weekly ? record->last_state : record->state;
        if (occurrence_id != delivery.occurrence_id || record->action != delivery.action ||
            (result != OccurrenceState::Uncertain && result != OccurrenceState::Completed))
            return fail(BookError::InvalidInput);
        if (result == OccurrenceState::Completed) { error_ = BookError::None; return true; }
        result = OccurrenceState::Completed;
        if (!commit(candidate)) { ready_ = false; return false; }
        return true;
    }

    bool create(Action action, std::wstring_view text, ClockSample now) {
        if (!ready_) return false;
        const auto minutes = Scheduler::parse_duration(text);
        if (!minutes || !valid_action(action) || !valid_deadline(now, *minutes) ||
            snapshot_.records.size() >= max_schedule_records || snapshot_.next_id == maximum ||
            snapshot_.next_occurrence_id == maximum) return fail(BookError::InvalidInput);
        auto candidate = snapshot_;
        const auto id = candidate.next_id++;
        candidate.records.push_back({id, candidate.next_occurrence_id++, action,
            *minutes, now.utc_ms + *minutes * 60000, false, OccurrenceState::Scheduled});
        if (!accept_candidate(candidate, id, now) || !commit(candidate)) return false;
        deadlines_[id] = now.monotonic_ms + *minutes * 60000;
        return true;
    }

    bool edit(std::uint64_t id, Action action, std::wstring_view text, ClockSample now) {
        if (!ready_) return false;
        auto candidate = snapshot_;
        auto record = find(candidate, id);
        if (record == candidate.records.end()) return fail(BookError::NotFound);
        const auto minutes = Scheduler::parse_duration(text);
        if (!minutes || !valid_action(action) || !valid_deadline(now, *minutes) ||
            snapshot_.next_occurrence_id == maximum) return fail(BookError::InvalidInput);
        const bool paused = record->paused;
        const bool force = record->force_shutdown && action == Action::Shutdown;
        *record = {id, 0, action, 0, 0, paused, OccurrenceState::Scheduled};
        record->force_shutdown = force;
        record->action = action;
        record->minutes = *minutes;
        record->due_utc_ms = now.utc_ms + *minutes * 60000;
        record->occurrence_id = candidate.next_occurrence_id++;
        record->state = OccurrenceState::Scheduled;
        if (!accept_candidate(candidate, id, now) || !commit(candidate)) return false;
        deadlines_[id] = now.monotonic_ms + *minutes * 60000;
        return true;
    }

    std::optional<std::uint64_t> preview_once(CivilTime civil, ClockSample now) const {
        BookError error;
        return resolve_once(civil, now, error);
    }
    bool create_once(Action action, CivilTime civil, ClockSample now) {
        return save_once(0, action, civil, now);
    }
    bool edit_once(std::uint64_t id, Action action, CivilTime civil, ClockSample now) {
        if (id == 0) return fail(BookError::NotFound);
        return save_once(id, action, civil, now);
    }
    std::optional<std::uint64_t> preview_weekly(unsigned weekdays, unsigned minute_of_day, ClockSample now) const {
        ScheduleRecord record{};
        record.weekdays = weekdays;
        record.minute_of_day = minute_of_day;
        BookError error;
        if (!plan_weekly(record, now, error)) return {};
        return record.due_utc_ms;
    }
    bool create_weekly(Action action, unsigned weekdays, unsigned minute_of_day, ClockSample now) {
        return save_weekly(0, action, weekdays, minute_of_day, now);
    }
    bool edit_weekly(std::uint64_t id, Action action, unsigned weekdays, unsigned minute_of_day, ClockSample now) {
        if (id == 0) return fail(BookError::NotFound);
        return save_weekly(id, action, weekdays, minute_of_day, now);
    }

    bool set_paused(std::uint64_t id, bool paused, ClockSample now) {
        if (!ready_) return false;
        auto candidate = snapshot_;
        auto record = find(candidate, id);
        if (record == candidate.records.end()) return fail(BookError::NotFound);
        if (record->kind == ScheduleKind::Weekly) {
            if (!refresh_weekly(candidate, *record, now)) return false;
        } else if (record->state == OccurrenceState::Scheduled && remaining_ms(id, now) == 0)
            record->state = OccurrenceState::Expired;
        else if (record->paused == paused) { error_ = BookError::None; return true; }
        record->paused = paused;
        return accept_candidate(candidate, id, now, false) && commit(candidate);
    }

    bool erase(std::uint64_t id) {
        if (!ready_) return false;
        auto candidate = snapshot_;
        auto record = find(candidate, id);
        if (record == candidate.records.end()) return fail(BookError::NotFound);
        candidate.records.erase(record);
        bool changed = false;
        if (!update_conflicts(candidate, last_now_, changed)) return false;
        if (!commit(candidate)) return false;
        deadlines_.erase(id);
        return true;
    }

    bool cancel(std::uint64_t id) {
        if (!ready_) return false;
        auto candidate = snapshot_;
        auto record = find(candidate, id);
        if (record == candidate.records.end()) return fail(BookError::NotFound);
        if (record->state != OccurrenceState::Scheduled) { error_ = BookError::None; return true; }
        record->state = OccurrenceState::Cancelled;
        if (record->kind == ScheduleKind::Weekly) {
            consume_weekly(*record, OccurrenceState::Cancelled);
            if (!advance_weekly(candidate, *record, {record->due_utc_ms, 0})) return false;
        }
        bool changed = false;
        if (!update_conflicts(candidate, last_now_, changed)) return false;
        return commit(candidate);
    }

    // Moves only the current occurrence later; the repetition rule is untouched.
    // On a collision nothing changes and conflicting_id() names the other row.
    bool postpone(std::uint64_t id, std::uint64_t minutes, ClockSample now) {
        if (!ready_) return false;
        if (minutes == 0 || minutes > Scheduler::max_minutes || !valid_deadline(now, minutes))
            return fail(BookError::InvalidInput);
        auto candidate = snapshot_;
        auto record = find(candidate, id);
        if (record == candidate.records.end()) return fail(BookError::NotFound);
        const auto remaining = remaining_ms(id, now);
        const auto delta = minutes * 60000;
        // At the deadline the intent belongs to tick(); never produce two intents.
        if (record->state != OccurrenceState::Scheduled || remaining == 0 ||
            remaining > maximum - now.utc_ms - delta) return fail(BookError::InvalidInput);
        const auto due = now.utc_ms + remaining + delta;
        if (record->kind != ScheduleKind::Duration && due >= calendar_end) return fail(BookError::InvalidInput);
        for (const auto& other : candidate.records) {
            if (other.id == id || other.paused || other.state != OccurrenceState::Scheduled) continue;
            const auto other_due = other.kind == ScheduleKind::Duration
                ? now.utc_ms + remaining_ms(other.id, now) : effective_due_utc_ms(other);
            if (other_due == due) { conflicting_id_ = other.id; return fail(BookError::Conflict); }
        }
        if (record->kind == ScheduleKind::Weekly) record->postponed_utc_ms = due;
        else record->due_utc_ms = due;
        if (!accept_candidate(candidate, id, now) || !commit(candidate)) return false;
        if (record->kind == ScheduleKind::Duration) deadlines_[id] = now.monotonic_ms + remaining + delta;
        return true;
    }

    const std::vector<ScheduleRecord>& list() const { return snapshot_.records; }
    std::uint64_t remaining_ms(std::uint64_t id, ClockSample now) const {
        const auto record = std::find_if(snapshot_.records.begin(), snapshot_.records.end(),
            [id](const auto& value) { return value.id == id; });
        if (record == snapshot_.records.end() || record->state != OccurrenceState::Scheduled) return 0;
        if (record->kind != ScheduleKind::Duration)
            return effective_due_utc_ms(*record) > now.utc_ms ? effective_due_utc_ms(*record) - now.utc_ms : 0;
        const auto deadline = deadlines_.find(id);
        return deadline != deadlines_.end() && deadline->second > now.monotonic_ms
            ? deadline->second - now.monotonic_ms : 0;
    }
    bool writable() const { return ready_; }
    BookError error() const { return error_; }
    std::uint64_t conflicting_id() const { return conflicting_id_; }

private:
    static constexpr auto maximum = std::numeric_limits<std::uint64_t>::max();
    static constexpr std::uint64_t calendar_end = 253402300800000ULL;
    static constexpr std::uint64_t invalid_due = std::numeric_limits<std::uint64_t>::max();
    bool calendar_available(const ScheduleRecord& record, ClockSample now) {
        if (record.kind == ScheduleKind::Duration) return true;
        if (now.utc_ms >= calendar_end) return fail(BookError::InvalidData);
        if (!calendar_) return fail(BookError::Calendar);
        const auto local = calendar_->local(now.utc_ms);
        return local && valid_civil(*local) ? true : fail(BookError::Calendar);
    }
    // A civil high-water mark is durable even for dates skipped by a gap or clock change.
    bool plan_weekly(ScheduleRecord& record, ClockSample now, BookError& error) const {
        error = BookError::InvalidInput;
        if (!record.weekdays || record.weekdays > 127 || record.minute_of_day >= 1440 || now.utc_ms >= calendar_end)
            return false;
        if (!calendar_) { error = BookError::Calendar; return false; }
        const auto local = calendar_->local(now.utc_ms);
        if (!local || !valid_civil(*local)) { error = BookError::Calendar; return false; }
        record.civil_high_water = std::max(record.civil_high_water, civil_key(*local));
        auto day = *local;
        if (const auto high = civil_from_key(record.civil_high_water); high && civil_key(*high) > civil_key(day)) day = *high;
        day.hour = record.minute_of_day / 60;
        day.minute = record.minute_of_day % 60;
        // Ordinary weekly rules resolve within eight days. A whole year without
        // a reachable date indicates an unusable adapter; fail explicitly rather
        // than scanning thousands of years on the UI thread or dropping the rule.
        for (unsigned searched_days = 0; searched_days < 366; ++searched_days) {
            const auto key = civil_key(day);
            if (key > record.civil_high_water && (record.weekdays & (1U << weekday(day)))) {
                const auto values = calendar_->resolve(day);
                if (!values) { error = BookError::Calendar; return false; }
                if (!values->empty() && values->front() >= calendar_end) return false;
                if (!values->empty() && values->front() > now.utc_ms) {
                    record.civil = day;
                    record.due_utc_ms = values->front();
                    error = BookError::None;
                    return true;
                }
                record.civil_high_water = key;
            }
            const auto next = next_day(day);
            if (!next) return false;
            day = *next;
        }
        error = BookError::Calendar;
        return false;
    }
    static void consume_weekly(ScheduleRecord& record, OccurrenceState result) {
        record.last_occurrence_id = record.occurrence_id;
        record.last_state = result;
        record.civil_high_water = std::max(record.civil_high_water, civil_key(record.civil));
        record.state = result;
        record.postponed_utc_ms = 0;
    }
    bool advance_weekly(ScheduleSnapshot& candidate, ScheduleRecord& record, ClockSample now) {
        BookError error;
        if (!plan_weekly(record, now, error)) return fail(error);
        if (candidate.next_occurrence_id == maximum) return fail(BookError::InvalidInput);
        record.occurrence_id = candidate.next_occurrence_id++;
        record.state = OccurrenceState::Scheduled;
        return true;
    }
    bool refresh_weekly(ScheduleSnapshot& candidate, ScheduleRecord& record, ClockSample now) {
        if (!calendar_) return fail(BookError::Calendar);
        const auto values = calendar_->resolve(record.civil);
        if (!values) return fail(BookError::Calendar);
        // A postponed occurrence stays due until its new instant even after the rule's time passed.
        if (record.state == OccurrenceState::Scheduled &&
            (effective_due_utc_ms(record) <= now.utc_ms || values->empty() ||
             (!record.postponed_utc_ms && values->front() <= now.utc_ms)))
            consume_weekly(record, OccurrenceState::Expired);
        if (record.state != OccurrenceState::Scheduled) return advance_weekly(candidate, record, now);
        const auto previous_civil = record.civil;
        BookError error;
        if (!plan_weekly(record, now, error)) return fail(error);
        if (!(previous_civil == record.civil)) {
            if (candidate.next_occurrence_id == maximum) return fail(BookError::InvalidInput);
            record.occurrence_id = candidate.next_occurrence_id++;
            record.postponed_utc_ms = 0;
        }
        return true;
    }
    bool save_weekly(std::uint64_t id, Action action, unsigned weekdays, unsigned minute_of_day, ClockSample now) {
        if (!ready_) return false;
        auto candidate = snapshot_;
        const auto old = find(candidate, id);
        if (id && old == candidate.records.end()) return fail(BookError::NotFound);
        if (!valid_action(action) || candidate.next_occurrence_id == maximum ||
            (!id && (candidate.records.size() >= max_schedule_records || candidate.next_id == maximum)))
            return fail(BookError::InvalidInput);
        ScheduleRecord record{id ? id : candidate.next_id++, candidate.next_occurrence_id++, action,
            0, 0, id ? old->paused : false, OccurrenceState::Scheduled};
        record.force_shutdown = id && old->force_shutdown && action == Action::Shutdown;
        record.kind = ScheduleKind::Weekly;
        record.weekdays = weekdays;
        record.minute_of_day = minute_of_day;
        BookError error;
        if (!plan_weekly(record, now, error)) return fail(error);
        if (id) *old = record; else candidate.records.push_back(record);
        if (!accept_candidate(candidate, record.id, now) || !commit(candidate)) return false;
        deadlines_.erase(record.id);
        return true;
    }
    std::optional<std::uint64_t> resolve_once(CivilTime civil, ClockSample now, BookError& error) const {
        error = BookError::InvalidInput;
        if (!valid_civil(civil) || now.utc_ms >= calendar_end) return {};
        if (!calendar_) { error = BookError::Calendar; return {}; }
        const auto values = calendar_->resolve(civil);
        if (!values) { error = BookError::Calendar; return {}; }
        if (values->empty() || values->front() <= now.utc_ms || values->front() >= calendar_end) return {};
        error = BookError::None;
        return values->front();
    }
    bool save_once(std::uint64_t id, Action action, CivilTime civil, ClockSample now) {
        if (!ready_) return false;
        auto candidate = snapshot_;
        auto old = find(candidate, id);
        if (id && old == candidate.records.end()) return fail(BookError::NotFound);
        BookError error;
        const auto due = resolve_once(civil, now, error);
        if (!due) return fail(error);
        if (!valid_action(action) || candidate.next_occurrence_id == maximum ||
            (!id && (candidate.records.size() >= max_schedule_records || candidate.next_id == maximum)))
            return fail(BookError::InvalidInput);
        ScheduleRecord record{id ? id : candidate.next_id++, candidate.next_occurrence_id++, action,
            0, *due, id ? old->paused : false, OccurrenceState::Scheduled};
        record.force_shutdown = id && old->force_shutdown && action == Action::Shutdown;
        record.kind = ScheduleKind::Once;
        record.civil = civil;
        if (id) *old = record; else candidate.records.push_back(record);
        if (!accept_candidate(candidate, record.id, now) || !commit(candidate)) return false;
        deadlines_.erase(record.id);
        return true;
    }
    struct ConflictTime {
        std::uint64_t due = 0, civil = 0;
        unsigned weekday_bit = 0, minute = 0;
    };
    bool update_conflicts(ScheduleSnapshot& candidate, ClockSample now, bool& changed,
                          std::uint64_t replaced_id = 0) {
        last_now_ = now;
        const bool has_weekly = std::any_of(candidate.records.begin(), candidate.records.end(),
            [](const auto& row) { return !row.paused && row.kind == ScheduleKind::Weekly; });
        std::vector<ConflictTime> times(candidate.records.size());
        std::vector<std::uint64_t> conflicts(candidate.records.size());
        for (std::size_t i = 0; i < candidate.records.size(); ++i) {
            const auto& row = candidate.records[i];
            if (row.paused || row.state != OccurrenceState::Scheduled) continue;
            auto& time = times[i];
            time.due = row.due_utc_ms;
            const auto deadline = deadlines_.find(row.id);
            if (row.kind == ScheduleKind::Duration && row.id != replaced_id && deadline != deadlines_.end()) {
                if (deadline->second >= now.monotonic_ms) {
                    const auto remaining = deadline->second - now.monotonic_ms;
                    if (now.utc_ms > maximum - remaining) return fail(BookError::InvalidData);
                    time.due = now.utc_ms + remaining;
                } else {
                    const auto late = now.monotonic_ms - deadline->second;
                    // An elapsed deadline before the UTC epoch cannot match a civil rule.
                    if (late > now.utc_ms) {
                        time.due = row.due_utc_ms;
                        // civil fields remain zero/unset
                    } else {
                        time.due = now.utc_ms - late;
                    }
                }
                // Update the record's due_utc_ms to the projected civil time for persistence across restarts
                if (row.due_utc_ms != time.due) {
                    candidate.records[i].due_utc_ms = time.due;
                    changed = true;
                }
            }
            if (!has_weekly || row.kind == ScheduleKind::Weekly || time.due >= calendar_end) continue;
            if (!calendar_) return fail(BookError::Calendar);
            const auto local = calendar_->local(time.due);
            if (!local || !valid_civil(*local)) return fail(BookError::Calendar);
            const auto values = calendar_->resolve(*local);
            if (!values) return fail(BookError::Calendar);
            // Only an exact match to the first instant of a civil minute can be
            // a weekly occurrence; second-fold instants and milliseconds differ.
            if (!values->empty() && values->front() == time.due) {
                time.civil = civil_key(*local);
                time.weekday_bit = 1U << weekday(*local);
                time.minute = local->hour * 60 + local->minute;
            }
        }
        for (std::size_t i = 0; i < candidate.records.size(); ++i) {
            const auto& a = candidate.records[i];
            if (a.paused || a.state != OccurrenceState::Scheduled) continue;
            for (std::size_t j = i + 1; j < candidate.records.size(); ++j) {
                const auto& b = candidate.records[j];
                if (b.paused || b.state != OccurrenceState::Scheduled) continue;
                bool collision;
                if (a.kind == ScheduleKind::Weekly && b.kind == ScheduleKind::Weekly) {
                    collision = (a.weekdays & b.weekdays) && a.minute_of_day == b.minute_of_day;
                } else if (a.kind == ScheduleKind::Weekly || b.kind == ScheduleKind::Weekly) {
                    const auto& weekly = a.kind == ScheduleKind::Weekly ? a : b;
                    const auto& finite = a.kind == ScheduleKind::Weekly ? times[j] : times[i];
                    collision = finite.civil > weekly.civil_high_water &&
                        (finite.weekday_bit & weekly.weekdays) && finite.minute == weekly.minute_of_day;
                } else collision = times[i].due == times[j].due;
                if (!collision) continue;
                // A deterministic neighbour identifies the component without
                // depending on row order or silently selecting an action.
                if (!conflicts[i] || b.id < conflicts[i]) conflicts[i] = b.id;
                if (!conflicts[j] || a.id < conflicts[j]) conflicts[j] = a.id;
            }
        }
        for (std::size_t i = 0; i < candidate.records.size(); ++i) {
            changed = changed || candidate.records[i].conflict_with != conflicts[i];
            candidate.records[i].conflict_with = conflicts[i];
        }
        return true;
    }
    bool accept_candidate(ScheduleSnapshot& candidate, std::uint64_t id, ClockSample now,
                          bool replaced = true) {
        bool changed = false;
        if (!update_conflicts(candidate, now, changed, replaced ? id : 0)) return false;
        const auto selected = find(candidate, id);
        if (selected != candidate.records.end() && selected->conflict_with) {
            conflicting_id_ = selected->conflict_with;
            return fail(BookError::Conflict);
        }
        return true;
    }
    static bool valid_deadline(ClockSample now, std::uint64_t minutes) {
        return now.utc_ms <= maximum - minutes * 60000 &&
               now.monotonic_ms <= maximum - minutes * 60000;
    }
    static std::vector<ScheduleRecord>::iterator find(ScheduleSnapshot& snapshot, std::uint64_t id) {
        return std::find_if(snapshot.records.begin(), snapshot.records.end(),
            [id](const auto& record) { return record.id == id; });
    }
    bool fail(BookError error) { error_ = error; if (error != BookError::Conflict) conflicting_id_ = 0; return false; }
    bool commit(const ScheduleSnapshot& candidate) {
        if (!storage_.save(candidate)) return fail(BookError::Storage);
        snapshot_ = candidate;
        conflicting_id_ = 0;
        error_ = BookError::None;
        return true;
    }

public:
    // Getter for theme preference
    unsigned get_theme() const {
        return snapshot_.theme;
    }

    // Setter for theme preference
    bool set_theme(unsigned theme) {
        if (!ready_ || theme > 2) return false;
        auto candidate = snapshot_;
        candidate.theme = theme;
        return commit(candidate);
    }

    bool set_sound_enabled(bool enabled) {
        if (!ready_) return false;
        if (snapshot_.sound_enabled == enabled) return true;
        auto candidate = snapshot_;
        candidate.sound_enabled = enabled;
        return commit(candidate);
    }

    // Getter for sound enabled
    bool get_sound_enabled() const {
        return snapshot_.sound_enabled;
    }

    // Getter for locale
    unsigned get_locale() const {
        return snapshot_.locale;
    }

    // Setter for locale
    bool set_locale(unsigned locale) {
        if (!ready_ || locale > 1) return false;
        auto candidate = snapshot_;
        candidate.locale = locale;
        return commit(candidate);
    }
    ScheduleStorage& storage_;
    Calendar* calendar_ = nullptr;
    ScheduleSnapshot snapshot_;
    std::map<std::uint64_t, std::uint64_t> deadlines_;
    bool ready_ = false;
    BookError error_ = BookError::NotReady;
    std::uint64_t conflicting_id_ = 0;
    ClockSample last_now_{};
};

} // namespace power_timer



// Source: src/strings.hpp

namespace power_timer::text {
// Portuguese UI catalog; strings_en.hpp provides the English counterpart.
inline constexpr auto title = L"Crazy Time";
inline constexpr auto theme_label = L"Tema";
inline constexpr auto language_label = L"Idioma";
inline constexpr auto portuguese = L"Português";
inline constexpr auto english = L"English";
inline constexpr auto brand = L"Crazy Time";
inline constexpr auto subtitle = L"Programe uma pausa.";
inline constexpr auto simulation = L"SIMULAÇÃO";
inline constexpr auto simulation_note = L"O computador será desligado, suspenso ou hibernado na hora programada.";
inline constexpr auto action_label = L"O que você quer fazer?";
inline constexpr const wchar_t* actions[] = {L"Desligar", L"Suspender", L"Hibernar"};
inline constexpr auto duration_label = L"Daqui a quantos minutos?";
inline constexpr auto kind_label = L"Quando?";
inline constexpr const wchar_t* kinds[] = {L"Por duração", L"Data e hora", L"Repetir na semana"};
inline constexpr auto date_label = L"Data";
inline constexpr auto time_label = L"Horário local";
inline constexpr auto weekly_label = L"Repetir nos dias selecionados";
inline constexpr const wchar_t* weekdays[] = {L"Dom", L"Seg", L"Ter", L"Qua", L"Qui", L"Sex", L"Sáb"};
inline constexpr auto daily = L"Todos os dias";
inline constexpr auto calendar_preview_error = L"Escolha uma data futura ou dias da semana e um horário válido.";
inline constexpr auto calendar_error = L"Não foi possível consultar o calendário. Tente reabrir o painel.";
inline constexpr auto calendar_input_error = L"Confira data, horário futuro e dias selecionados. Horas inexistentes não são aceitas.";
inline constexpr auto rule_column = L"Regra";
inline constexpr auto once_rule = L"Avulso";
inline constexpr auto weekly_rule = L"Semanal";
inline constexpr auto last_result = L"Anterior: ";
inline constexpr auto duration_hint = L"De 1 minuto a 7 dias (10080 minutos)";
struct DurationPreset { const wchar_t* label; unsigned minutes; };
inline constexpr DurationPreset presets[] = {
    {L"15 min", 15}, {L"30 min", 30}, {L"1 hora", 60}, {L"2 horas", 120}
};
inline constexpr auto schedule = L"Agendar ação";
inline constexpr auto replace = L"Substituir agendamento";
inline constexpr auto cancel = L"Cancelar ocorrência";
inline constexpr auto idle = L"Tudo no seu tempo";
inline constexpr auto idle_detail = L"Escolha uma ação e quando ela deve acontecer.";
inline constexpr auto active = L"Ação agendada";
inline constexpr auto warning = L"Últimos 60 segundos";
inline constexpr auto warning_30min = L"Faltam 30 minutos";
inline constexpr auto warning_30min_detail = L"Agendamento disparará em 30 minutos. Abra o painel para ajustar ou cancelar.";
inline constexpr auto warning_15min = L"Faltam 15 minutos";
inline constexpr auto warning_15min_detail = L"Agendamento disparará em 15 minutos. Abra o painel para ajustar ou cancelar.";
inline constexpr auto warning_10min = L"Faltam 10 minutos";
inline constexpr auto warning_10min_detail = L"Agendamento disparará em 10 minutos. Abra o painel para ajustar ou cancelar.";
inline constexpr auto warning_5min = L"Faltam 5 minutos";
inline constexpr auto warning_5min_detail = L"Agendamento disparará em 5 minutos. Abra o painel para ajustar ou cancelar.";
inline constexpr auto completed = L"Ação solicitada";
inline constexpr auto completed_detail = L"Solicitação enviada ao Windows.";
inline constexpr auto cancelled = L"Ocorrência cancelada";
inline constexpr auto cancelled_detail = L"Você pode programar uma nova ação.";
inline constexpr auto expired = L"Ocorrência expirada";
inline constexpr auto expired_detail = L"O prazo passou. Nenhuma ação foi executada.";
inline constexpr auto invalid_duration = L"Informe um número inteiro de 1 a 10080 minutos.";
inline constexpr auto preview_prefix = L"Previsão: ";
inline constexpr auto scheduled_prefix = L"Agendado: ";
inline constexpr auto separator = L" · ";
inline constexpr auto invalid_preview = L"Preencha uma duração válida para ver o horário.";
inline constexpr auto open = L"Abrir painel";
inline constexpr auto settings = L"Configurações";
inline constexpr auto options = L"Opções";
inline constexpr auto theme_auto = L"Automático";
inline constexpr auto theme_light = L"Claro";
inline constexpr auto theme_dark = L"Escuro";
inline constexpr const wchar_t* theme_strings[] = {theme_auto, theme_light, theme_dark};
inline constexpr auto notification_settings = L"Visibilidade do ícone no Windows";
inline constexpr auto exit = L"Sair até reativar o mod";
inline constexpr auto settings_note = L"Duração e ação são configuradas neste painel.";
inline constexpr auto settings_error = L"Não foi possível abrir as configurações do Windows.";
inline constexpr auto tray_error = L"Ícone indisponível. Tentando reconectar à barra…";
inline constexpr auto clock_empty = L"— — : — —";
inline constexpr auto countdown_notification = L"Falta 1 minuto. Abra o painel para acompanhar ou cancelar.";
inline constexpr auto unavailable_time = L"Horário indisponível";
inline constexpr auto add_schedule = L"Adicionar agendamento";
inline constexpr auto save_schedule = L"Salvar alterações";
inline constexpr auto new_schedule = L"Novo";
inline constexpr auto schedules = L"Seus agendamentos";
inline constexpr auto edit = L"Editar";
inline constexpr auto pause = L"Pausar";
inline constexpr auto resume = L"Reativar";
inline constexpr auto erase = L"Excluir";
inline constexpr auto postpone = L"Adiar 10 min";
inline constexpr auto action_column = L"Ação";
inline constexpr auto id_column = L"Nº";
inline constexpr auto state_conflict = L"Conflito";
inline constexpr auto conflict_error = L"Não salvo: o horário conflita com ";
inline constexpr auto conflict_detail = L"Bloqueado por conflito com #";
inline constexpr auto conflict_hint = L". Edite, pause ou exclua uma das programações.";
inline constexpr auto conflict_notification = L"Há horários conflitantes. Os envolvidos estão bloqueados; abra o painel para ajustar.";
inline constexpr auto schedule_number = L"Agendamento #";
inline constexpr auto due_column = L"Próxima ocorrência";
inline constexpr auto state_column = L"Estado";
inline constexpr auto duration_column = L"Minutos";
inline constexpr auto state_scheduled = L"Agendada";
inline constexpr auto state_paused = L"Pausada";
inline constexpr auto state_cancelled = L"Cancelada";
inline constexpr auto state_expired = L"Expirada";
inline constexpr auto state_uncertain = L"Resultado incerto";
inline constexpr auto state_completed = L"Concluída";
inline constexpr auto no_next = L"—";
inline constexpr auto force_label = L"Forçar fechamento de apps";
inline constexpr auto force_title = L"Desligamento forçado";
inline constexpr auto force_warning = L"Apps abertos serão fechados sem perguntar e dados não salvos serão PERDIDOS. Ativar somente para esta programação?";
inline constexpr auto forced_tag = L" (forçado)";
inline constexpr auto power_unavailable = L"Ação indisponível neste computador. Nada foi executado.";
inline constexpr auto power_failed = L"O sistema recusou a solicitação. Não será repetida automaticamente.";
inline constexpr auto uncertain_detail = L"Não será repetida automaticamente.";
inline constexpr auto paused_detail = L"O prazo continua correndo enquanto está pausada.";
inline constexpr auto storage_error = L"Não foi possível salvar. A alteração não foi confirmada.";
inline constexpr auto invalid_data = L"Arquivo de agendamentos inválido. Dados preservados; edição bloqueada.";
inline constexpr auto not_ready = L"Armazenamento indisponível. Reabra o Crazy Time para tentar novamente.";
inline constexpr auto missing_record = L"Selecione um agendamento na lista.";
inline constexpr auto record_input_error = L"Duração inválida ou limite de 1000 agendamentos atingido.";
}

// Source: src/strings_en.hpp

namespace power_timer::text_en {
// English UI catalog
inline constexpr auto title = L"Crazy Time";
inline constexpr auto brand = L"Crazy Time";
inline constexpr auto subtitle = L"Schedule a break.";
inline constexpr auto simulation = L"SIMULATION";
inline constexpr auto simulation_note = L"The computer will shut down, sleep or hibernate on schedule.";
inline constexpr auto action_label = L"What do you want to do?";
inline constexpr const wchar_t* actions[] = {L"Shut down", L"Sleep", L"Hibernate"};
inline constexpr auto duration_label = L"How many minutes from now?";
inline constexpr auto kind_label = L"When?";
inline constexpr const wchar_t* kinds[] = {L"By duration", L"Date and time", L"Repeat weekly"};
inline constexpr auto date_label = L"Date";
inline constexpr auto time_label = L"Local time";
inline constexpr auto weekly_label = L"Repeat on selected days";
inline constexpr const wchar_t* weekdays[] = {L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"};
inline constexpr auto daily = L"Every day";
inline constexpr auto calendar_preview_error = L"Choose a future date or weekdays and a valid time.";
inline constexpr auto calendar_error = L"Could not consult the calendar. Try reopening the panel.";
inline constexpr auto calendar_input_error = L"Check date, future time and selected days. Non-existent hours are not accepted.";
inline constexpr auto rule_column = L"Rule";
inline constexpr auto once_rule = L"One-time";
inline constexpr auto weekly_rule = L"Weekly";
inline constexpr auto last_result = L"Previous: ";
inline constexpr auto duration_hint = L"From 1 minute to 7 days (10080 minutes)";
struct DurationPreset { const wchar_t* label; unsigned minutes; };
inline constexpr DurationPreset presets[] = {
    {L"15 min", 15}, {L"30 min", 30}, {L"1 hour", 60}, {L"2 hours", 120}
};
inline constexpr auto schedule = L"Schedule action";
inline constexpr auto replace = L"Replace schedule";
inline constexpr auto cancel = L"Cancel occurrence";
inline constexpr auto idle = L"All in good time";
inline constexpr auto idle_detail = L"Choose an action and when it should happen.";
inline constexpr auto active = L"Action scheduled";
inline constexpr auto warning = L"Last 60 seconds";
inline constexpr auto warning_30min = L"30 minutes remaining";
inline constexpr auto warning_30min_detail = L"Schedule will trigger in 30 minutes. Open the panel to adjust or cancel.";
inline constexpr auto warning_15min = L"15 minutes remaining";
inline constexpr auto warning_15min_detail = L"Schedule will trigger in 15 minutes. Open the panel to adjust or cancel.";
inline constexpr auto warning_10min = L"10 minutes remaining";
inline constexpr auto warning_10min_detail = L"Schedule will trigger in 10 minutes. Open the panel to adjust or cancel.";
inline constexpr auto warning_5min = L"5 minutes remaining";
inline constexpr auto warning_5min_detail = L"Schedule will trigger in 5 minutes. Open the panel to adjust or cancel.";
inline constexpr auto completed = L"Action requested";
inline constexpr auto completed_detail = L"Request sent to Windows.";
inline constexpr auto cancelled = L"Occurrence cancelled";
inline constexpr auto cancelled_detail = L"You can schedule a new action.";
inline constexpr auto expired = L"Occurrence expired";
inline constexpr auto expired_detail = L"The deadline passed. No action was executed.";
inline constexpr auto invalid_duration = L"Please enter an integer from 1 to 10080 minutes.";
inline constexpr auto preview_prefix = L"Preview: ";
inline constexpr auto scheduled_prefix = L"Scheduled: ";
inline constexpr auto separator = L" · ";
inline constexpr auto invalid_preview = L"Enter a valid duration to see the time.";
inline constexpr auto open = L"Open panel";
inline constexpr auto settings = L"Settings";
inline constexpr auto options = L"Options";
inline constexpr auto theme_auto = L"Automatic";
inline constexpr auto theme_light = L"Light";
inline constexpr auto theme_dark = L"Dark";
inline constexpr const wchar_t* theme_strings[] = {theme_auto, theme_light, theme_dark};
inline constexpr auto notification_settings = L"Icon visibility in Windows";
inline constexpr auto exit = L"Exit until mod restart";
inline constexpr auto settings_note = L"Duration and action are configured in this panel.";
inline constexpr auto settings_error = L"Could not open Windows settings.";
inline constexpr auto tray_error = L"Icon unavailable. Trying to reconnect to taskbar…";
inline constexpr auto clock_empty = L"— — : — —";
inline constexpr auto countdown_notification = L"1 minute remaining. Open the panel to monitor or cancel.";
inline constexpr auto unavailable_time = L"Time unavailable";
inline constexpr auto add_schedule = L"Add schedule";
inline constexpr auto save_schedule = L"Save changes";
inline constexpr auto new_schedule = L"New";
inline constexpr auto schedules = L"Your schedules";
inline constexpr auto edit = L"Edit";
inline constexpr auto pause = L"Pause";
inline constexpr auto resume = L"Resume";
inline constexpr auto erase = L"Delete";
inline constexpr auto postpone = L"Postpone 10 min";
inline constexpr auto action_column = L"Action";
inline constexpr auto id_column = L"No.";
inline constexpr auto state_conflict = L"Conflict";
inline constexpr auto conflict_error = L"Not saved: time conflicts with ";
inline constexpr auto conflict_detail = L"Blocked by conflict with #";
inline constexpr auto conflict_hint = L". Edit, pause or delete one of the schedules.";
inline constexpr auto conflict_notification = L"Conflicting times exist. The involved ones are blocked; open the panel to adjust.";
inline constexpr auto schedule_number = L"Schedule #";
inline constexpr auto due_column = L"Next occurrence";
inline constexpr auto state_column = L"State";
inline constexpr auto duration_column = L"Minutes";
inline constexpr auto state_scheduled = L"Scheduled";
inline constexpr auto state_paused = L"Paused";
inline constexpr auto state_cancelled = L"Cancelled";
inline constexpr auto state_expired = L"Expired";
inline constexpr auto state_uncertain = L"Uncertain result";
inline constexpr auto state_completed = L"Completed";
inline constexpr auto no_next = L"—";
inline constexpr auto force_label = L"Force apps to close";
inline constexpr auto force_title = L"Forced shutdown";
inline constexpr auto force_warning = L"Open apps will be closed without asking and unsaved data will be LOST. Enable for this schedule only?";
inline constexpr auto forced_tag = L" (forced)";
inline constexpr auto power_unavailable = L"Action unavailable on this computer. Nothing was executed.";
inline constexpr auto power_failed = L"The system rejected the request. It will not be repeated automatically.";
inline constexpr auto uncertain_detail = L"It will not be repeated automatically.";
inline constexpr auto paused_detail = L"The deadline continues to run while paused.";
inline constexpr auto storage_error = L"Could not save. The change was not confirmed.";
inline constexpr auto invalid_data = L"Schedule file invalid. Data preserved; editing blocked.";
inline constexpr auto not_ready = L"Storage unavailable. Try reopening Crazy Time.";
inline constexpr auto missing_record = L"Select a schedule from the list.";
inline constexpr auto record_input_error = L"Invalid duration or limit of 1000 schedules reached.";
inline constexpr auto theme_label = L"Theme";
inline constexpr auto language_label = L"Language";
inline constexpr auto portuguese = L"Portuguese";
inline constexpr auto english = L"English";
}

// Source: src/warning_tracker.hpp

#include <array>
#include <cstdint>
#include <optional>
#include <unordered_set>

namespace power_timer {

// Decides which countdown warning (if any) is due for an occurrence. Thresholds
// already crossed the first time an occurrence is seen were never "reached" by
// the countdown and are marked without notifying.
class WarningTracker {
public:
    static constexpr std::array<std::uint64_t, 5> thresholds = {
        30ULL * 60000, 15ULL * 60000, 10ULL * 60000, 5ULL * 60000, Scheduler::warning_ms};

    // Returns the threshold index to announce now, or nothing.
    std::optional<std::size_t> due(std::uint64_t occurrence_id, std::uint64_t remaining_ms) {
        if (remaining_ms == 0) return std::nullopt;
        const bool first_sight = keys_.insert(occurrence_id * 10 + 9).second;
        int applicable = -1;
        for (int i = 0; i < static_cast<int>(thresholds.size()); ++i)
            if (remaining_ms <= thresholds[i]) applicable = i;
        if (applicable < 0) return std::nullopt;
        for (int i = 0; i < applicable; ++i) keys_.insert(occurrence_id * 10 + i);
        const bool fresh = keys_.insert(occurrence_id * 10 + applicable).second;
        if (first_sight || !fresh) return std::nullopt;
        return static_cast<std::size_t>(applicable);
    }
    // After a postponement only the warnings still ahead of the new deadline may fire again.
    void reopen_ahead_of(std::uint64_t occurrence_id, std::uint64_t remaining_ms) {
        for (std::size_t i = 0; i < thresholds.size(); ++i)
            if (remaining_ms > thresholds[i]) keys_.erase(occurrence_id * 10 + i);
    }

private:
    std::unordered_set<std::uint64_t> keys_; // occurrence_id * 10 + index; +9 marks "seen"
};

} // namespace power_timer

// Source: src/power_executor.hpp

#include <cstdint>

namespace power_timer {

// Why an action cannot run now. Shown to the user next to the schedule.
enum class Availability { Available, Unsupported, ModernStandbyOnly, DisabledByPolicy, QueryFailed };
struct PowerCapabilities {
    Availability shutdown = Availability::Available;
    Availability sleep = Availability::Available;
    Availability hibernate = Availability::Available;
    Availability of(Action action) const {
        switch (action) {
            case Action::Shutdown: return shutdown;
            case Action::Sleep: return sleep;
            case Action::Hibernate: return hibernate;
        }
        return Availability::Unsupported;
    }
};
enum class ExecStatus {
    Accepted,          // The system accepted the request; completion is not proven.
    NotAvailable,      // Capability check failed; nothing was requested.
    PrivilegeFailed,   // Enabling the temporary privilege failed (partly or fully).
    Rejected           // The system call itself reported an error.
};
struct ExecResult { ExecStatus status; Availability availability; std::uint32_t error_code; };

// The only seam to the operating system. The simulated build never provides a
// real implementation; tests use fakes. A real one needs deliberate manual testing.
class PowerBackend {
public:
    virtual ~PowerBackend() = default;
    virtual PowerCapabilities query() = 0;
    // Must restore any privilege it enabled, on every path, and return the real error.
    virtual ExecResult request(Action action, bool force) = 0;
};

// Checks capability right before the request; never retries. Applications are
// forced to close only when the schedule explicitly opted in, and only for Shutdown.
inline ExecResult execute_power_action(PowerBackend& backend, Action action, bool force = false) {
    const auto availability = backend.query().of(action);
    if (availability != Availability::Available)
        return {ExecStatus::NotAvailable, availability, 0};
    return backend.request(action, force && action == Action::Shutdown);
}

// Simulation target: reports everything available and accepts without touching
// the system. Physically separate from any real executor.
class SimulatedBackend : public PowerBackend {
public:
    PowerCapabilities query() override { return {}; }
    ExecResult request(Action, bool) override { return {ExecStatus::Accepted, Availability::Available, 0}; }
};

} // namespace power_timer

// Source: src/windows/windows_power_backend.hpp

#include <windows.h>

namespace power_timer::windows {

// Real Windows power actions, used by the mod; the standalone preview simulates.
// powrprof.dll is resolved at run time so the mod needs no
// extra import library.
class WindowsPowerBackend : public PowerBackend {
public:
    PowerCapabilities query() override {
        PowerCapabilities caps; // shutdown is always attempted; privilege failure is reported by request()
        SYSTEM_POWER_CAPABILITIES power{};
        const auto get = functions().get_capabilities;
        if (!get) { caps.sleep = caps.hibernate = Availability::QueryFailed; return caps; }
        if (!get(&power)) { caps.sleep = caps.hibernate = Availability::QueryFailed; return caps; }
        // Classic S1-S3 sleep, or Modern Standby (SetSuspendState also enters it).
        const bool sleep_supported = power.SystemS1 || power.SystemS2 || power.SystemS3 || modern_standby(power);
        caps.sleep = sleep_supported ? Availability::Available : Availability::Unsupported;
        // Hibernate needs S4 support and an enabled hibernation file.
        caps.hibernate = !power.SystemS4 ? Availability::Unsupported
            : !power.HiberFilePresent ? Availability::DisabledByPolicy : Availability::Available;
        return caps;
    }

    ExecResult request(Action action, bool force) override {
        Privilege privilege;
        if (!privilege.acquired())
            return {ExecStatus::PrivilegeFailed, Availability::Available, privilege.error()};
        SetLastError(ERROR_SUCCESS);
        BOOL ok = FALSE;
        switch (action) {
            case Action::Shutdown: {
                // Normal shutdown lets applications object; EWX_FORCE only with the schedule's explicit opt-in.
                const UINT flags = EWX_POWEROFF | (force ? EWX_FORCE : 0);
                ok = ExitWindowsEx(flags, SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_MAINTENANCE | SHTDN_REASON_FLAG_PLANNED);
                break;
            }
            case Action::Sleep:
            case Action::Hibernate: {
                const auto suspend = functions().set_suspend_state;
                if (!suspend) return {ExecStatus::Rejected, Availability::Available, static_cast<std::uint32_t>(ERROR_PROC_NOT_FOUND)};
                // Never force and never disable wake events. Blocks until the PC resumes.
                ok = suspend(action == Action::Hibernate ? TRUE : FALSE, FALSE, FALSE);
                break;
            }
        }
        if (!ok) return {ExecStatus::Rejected, Availability::Available, static_cast<std::uint32_t>(GetLastError())};
        return {ExecStatus::Accepted, Availability::Available, 0};
    }

private:
    using GetCapsFn = BOOLEAN(WINAPI*)(PSYSTEM_POWER_CAPABILITIES);
    using SuspendFn = BOOLEAN(WINAPI*)(BOOLEAN, BOOLEAN, BOOLEAN);
    struct Functions { GetCapsFn get_capabilities = nullptr; SuspendFn set_suspend_state = nullptr; };
    static const Functions& functions() {
        static const Functions loaded = [] {
            Functions result;
            if (const auto module = LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)) {
                result.get_capabilities = reinterpret_cast<GetCapsFn>(reinterpret_cast<void*>(GetProcAddress(module, "GetPwrCapabilities")));
                result.set_suspend_state = reinterpret_cast<SuspendFn>(reinterpret_cast<void*>(GetProcAddress(module, "SetSuspendState")));
            }
            return result;
        }();
        return loaded;
    }
    // Older SDK headers lack AoAc; then Modern Standby cannot be told apart from "no sleep",
    // so sleep is attempted and request() reports a real failure if it is unsupported.
    template <class Capabilities>
    static bool modern_standby(const Capabilities& power) {
        if constexpr (requires { power.AoAc; }) return power.AoAc != FALSE;
        else return true;
    }

    // Enables SeShutdownPrivilege for the call and restores the previous state on every path.
    class Privilege {
    public:
        Privilege() {
            if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token_)) {
                error_ = GetLastError();
                return;
            }
            TOKEN_PRIVILEGES wanted{};
            wanted.PrivilegeCount = 1;
            wanted.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            if (!LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME, &wanted.Privileges[0].Luid)) {
                error_ = GetLastError();
                return;
            }
            DWORD returned = 0;
            SetLastError(ERROR_SUCCESS);
            const BOOL adjusted = AdjustTokenPrivileges(token_, FALSE, &wanted, sizeof(previous_), &previous_, &returned);
            const DWORD last = GetLastError();
            // AdjustTokenPrivileges can "succeed" while assigning nothing (ERROR_NOT_ALL_ASSIGNED).
            if (!adjusted) { error_ = last; return; }
            if (last == ERROR_NOT_ALL_ASSIGNED) { error_ = last; return; }
            changed_ = returned != 0 && previous_.PrivilegeCount != 0;
            ok_ = true;
        }
        ~Privilege() {
            if (token_) {
                if (changed_) AdjustTokenPrivileges(token_, FALSE, &previous_, 0, nullptr, nullptr);
                CloseHandle(token_);
            }
        }
        Privilege(const Privilege&) = delete;
        Privilege& operator=(const Privilege&) = delete;
        bool acquired() const { return ok_; }
        std::uint32_t error() const { return static_cast<std::uint32_t>(error_); }
    private:
        HANDLE token_{};
        TOKEN_PRIVILEGES previous_{};
        bool ok_ = false, changed_ = false;
        DWORD error_ = 0;
    };
};

} // namespace power_timer::windows

// Source: src/windows/schedule_file.hpp
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <atomic>
#include <utility>

namespace power_timer {
// The host creates the parent directory and serializes access. Invalid/Unavailable load
// must gate writes in the engine, preserving corrupt files for explicit recovery.
// Temp content is flushed before a same-volume replacement. Flush/write-through cannot
// guarantee survival of power loss across every filesystem/device write cache. A crash
// before replacement may leave an orphan temp file; load never promotes it to live data.
class FileScheduleStorage final : public ScheduleStorage {
public:
    explicit FileScheduleStorage(std::wstring path) : path_(std::move(path)) {}
    LoadResult load() override {
        try {
            Handle file(CreateFileW(path_.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
            if (!file.valid()) {
                return {GetLastError() == ERROR_FILE_NOT_FOUND ? LoadStatus::Missing : LoadStatus::Unavailable, {}};
            }
            LARGE_INTEGER size{};
            if (!GetFileSizeEx(file.get(), &size)) return {LoadStatus::Unavailable, {}};
            if (size.QuadPart <= 0 || size.QuadPart > static_cast<LONGLONG>(max_schedule_file_bytes))
                return {LoadStatus::Invalid, {}};
            std::string data(static_cast<std::size_t>(size.QuadPart), '\0');
            DWORD read = 0;
            if (!ReadFile(file.get(), data.data(), static_cast<DWORD>(data.size()), &read, nullptr))
                return {LoadStatus::Unavailable, {}};
            if (read != data.size()) return {LoadStatus::Invalid, {}};
            auto snapshot = decode_snapshot(data);
            return snapshot ? LoadResult{LoadStatus::Loaded, std::move(*snapshot)} : LoadResult{LoadStatus::Invalid, {}};
        } catch (...) { return {LoadStatus::Unavailable, {}}; }
    }
    bool save(const ScheduleSnapshot& snapshot) override {
        try {
            const auto data = encode_snapshot(snapshot);
            if (data.empty() || data.size() > max_schedule_file_bytes) return false;
            static std::atomic<std::uint64_t> sequence{0};
            for (unsigned attempt = 0; attempt < 16; ++attempt) {
                Temporary temporary(path_ + L".tmp-" + std::to_wstring(GetCurrentProcessId()) + L"-" +
                    std::to_wstring(GetTickCount64()) + L"-" + std::to_wstring(sequence.fetch_add(1)));
                Handle file(CreateFileW(temporary.path.c_str(), GENERIC_WRITE, 0, nullptr,
                    CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, nullptr));
                if (!file.valid()) {
                    if (GetLastError() == ERROR_FILE_EXISTS || GetLastError() == ERROR_ALREADY_EXISTS) continue;
                    return false;
                }
                temporary.owned = true;
                DWORD written = 0;
                if (!WriteFile(file.get(), data.data(), static_cast<DWORD>(data.size()), &written, nullptr) ||
                    written != data.size() || !FlushFileBuffers(file.get())) return false;
                if (!file.close()) return false;
                // Same directory/volume; COPY_ALLOWED is deliberately absent.
                // A failed/ambiguous commit returns false, so callers can block effects.
                if (!MoveFileExW(temporary.path.c_str(), path_.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
                    return false;
                temporary.owned = false;
                return true;
            }
            return false;
        } catch (...) { return false; }
    }
private:
    class Handle {
    public:
        explicit Handle(HANDLE value) : value_(value) {}
        ~Handle() { if (valid()) CloseHandle(value_); }
        bool valid() const { return value_ != INVALID_HANDLE_VALUE; }
        HANDLE get() const { return value_; }
        bool close() { const auto value = value_; value_ = INVALID_HANDLE_VALUE; return CloseHandle(value) != 0; }
    private:
        HANDLE value_;
    };
    struct Temporary {
        explicit Temporary(std::wstring name) : path(std::move(name)) {}
        ~Temporary() { if (owned) DeleteFileW(path.c_str()); }
        std::wstring path;
        bool owned = false;
    };
    std::wstring path_;
};
} // namespace power_timer


// Source: src/windows/local_calendar.hpp

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <algorithm>

namespace power_timer {

class WindowsCalendar final : public Calendar {
public:
    WindowsCalendar() = default;
    explicit WindowsCalendar(const DYNAMIC_TIME_ZONE_INFORMATION& zone) : fixed_zone_(zone) {}

    std::optional<CivilTime> local(std::uint64_t utc_ms) const override {
        const auto zone = snapshot();
        if (!zone) return std::nullopt;
        return local_with_zone(utc_ms, *zone);
    }

    std::optional<std::vector<std::uint64_t>> resolve(CivilTime civil) const override {
        if (!valid_civil(civil)) return std::nullopt;
        const auto zone = snapshot();
        if (!zone) return std::nullopt;

        TIME_ZONE_INFORMATION annual{};
        if (zone->TimeZoneKeyName[0] && !zone->DynamicDaylightTimeDisabled) {
            // GetTimeZoneInformationForYear requires the *local* year. Microsoft Learn:
            // https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-gettimezoneinformationforyear
            auto mutable_zone = *zone;
            if (!GetTimeZoneInformationForYear(static_cast<USHORT>(civil.year), &mutable_zone, &annual))
                return std::nullopt;
        } else {
            annual.Bias = zone->Bias;
            annual.StandardBias = zone->StandardBias;
            annual.DaylightBias = zone->DaylightBias;
            annual.StandardDate = zone->StandardDate;
            annual.DaylightDate = zone->DaylightDate;
        }

        // DYNAMIC_TIME_ZONE_INFORMATION defines UTC = local + Bias; the two
        // annual biases cover both sides of a repeated hour. Microsoft Learn:
        // https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/ns-timezoneapi-dynamic_time_zone_information
        // TzSpecificLocalTimeToSystemTimeEx chooses only one side of a fold and
        // can map an invalid gap. Round-trip each candidate instead:
        // https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-tzspecificlocaltimetosystemtimeex
        // https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-systemtimetotzspecificlocaltimeex
        const auto naive = naive_utc_ms(civil);
        std::vector<std::uint64_t> result;
        const auto try_bias = [&](std::int64_t bias) -> bool {
            const std::int64_t shifted = naive + bias * 60000;
            if (shifted < 0 || shifted >= last_utc_ms) return true;
            const auto candidate = static_cast<std::uint64_t>(shifted);
            const auto roundtrip = local_with_zone(candidate, *zone);
            if (!roundtrip) return false;
            if (*roundtrip == civil) result.push_back(candidate);
            return true;
        };
        const bool has_transitions = annual.StandardDate.wMonth && annual.DaylightDate.wMonth &&
                                     !zone->DynamicDaylightTimeDisabled;
        const std::int64_t standard_bias = static_cast<std::int64_t>(annual.Bias) +
                                           (has_transitions ? annual.StandardBias : 0);
        if (!try_bias(standard_bias))
            return std::nullopt;
        if (has_transitions &&
            !try_bias(static_cast<std::int64_t>(annual.Bias) + annual.DaylightBias))
            return std::nullopt;
        std::sort(result.begin(), result.end());
        result.erase(std::unique(result.begin(), result.end()), result.end());
        return result;
    }

private:
    // Unix milliseconds for 10000-01-01T00:00Z; upper bound is exclusive.
    static constexpr std::int64_t last_utc_ms = 253402300800000LL;
    static constexpr std::uint64_t filetime_epoch_ticks = 116444736000000000ULL;
    std::optional<DYNAMIC_TIME_ZONE_INFORMATION> fixed_zone_;

    std::optional<DYNAMIC_TIME_ZONE_INFORMATION> snapshot() const {
        if (fixed_zone_) return *fixed_zone_;
        DYNAMIC_TIME_ZONE_INFORMATION zone{};
        // Captured once per public call, so a time-zone change cannot split a
        // resolve operation. TIME_ZONE_ID_INVALID alone signals API failure.
        // https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-getdynamictimezoneinformation
        if (GetDynamicTimeZoneInformation(&zone) == TIME_ZONE_ID_INVALID) return std::nullopt;
        return zone;
    }

    static std::int64_t naive_utc_ms(CivilTime civil) {
        using namespace std::chrono;
        return duration_cast<milliseconds>(sys_days{year{civil.year}/month{civil.month}/day{civil.day}}.time_since_epoch()).count() +
               static_cast<std::int64_t>(civil.hour) * 3600000 +
               static_cast<std::int64_t>(civil.minute) * 60000;
    }

    static std::optional<CivilTime> local_with_zone(
        std::uint64_t utc_ms, const DYNAMIC_TIME_ZONE_INFORMATION& zone) {
        if (utc_ms >= static_cast<std::uint64_t>(last_utc_ms)) return std::nullopt;
        const std::uint64_t ticks = filetime_epoch_ticks + utc_ms * 10000;
        const FILETIME filetime{static_cast<DWORD>(ticks), static_cast<DWORD>(ticks >> 32)};
        SYSTEMTIME utc{};
        if (!FileTimeToSystemTime(&filetime, &utc)) return std::nullopt;
        SYSTEMTIME local_time{};
        if (!zone.TimeZoneKeyName[0] || zone.DynamicDaylightTimeDisabled) {
            // An explicit test zone has no registry key. The legacy converter
            // accepts its transition rules directly and does not read system
            // time-zone state. Live zones use the dynamic converter below to
            // avoid the legacy converter's documented cross-year limitation:
            // https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-systemtimetotzspecificlocaltime
            TIME_ZONE_INFORMATION rules{};
            rules.Bias = zone.Bias;
            rules.StandardBias = zone.StandardBias;
            rules.DaylightBias = zone.DaylightBias;
            rules.StandardDate = zone.StandardDate;
            rules.DaylightDate = zone.DaylightDate;
            if (zone.DynamicDaylightTimeDisabled) {
                rules.StandardDate = {};
                rules.DaylightDate = {};
            }
            if (!SystemTimeToTzSpecificLocalTime(&rules, &utc, &local_time)) return std::nullopt;
        } else {
            // Windhawk's MinGW headers omit this Windows 7+ declaration,
            // though kernel32 exports it. Resolve it at runtime.
            using Convert = BOOL (WINAPI *)(const DYNAMIC_TIME_ZONE_INFORMATION*,
                                             const SYSTEMTIME*, LPSYSTEMTIME);
            const auto module = GetModuleHandleW(L"kernel32.dll");
            if (!module) return std::nullopt;
            const auto convert = reinterpret_cast<Convert>(
                GetProcAddress(module, "SystemTimeToTzSpecificLocalTimeEx"));
            if (!convert || !convert(&zone, &utc, &local_time)) return std::nullopt;
        }
        const CivilTime result{local_time.wYear, local_time.wMonth, local_time.wDay,
                               local_time.wHour, local_time.wMinute};
        if (!valid_civil(result)) return std::nullopt;
        return result;
    }
};

} // namespace power_timer

// Source: src/windows/tray_host.hpp

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <sddl.h>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cwchar>
#include <cstdio>
#include <string>
#include <vector>
#include <unordered_set>

namespace power_timer::windows {
// The mod performs real power actions. Only the local preview executable runs in
// simulation mode, which swaps the wording below and uses SimulatedBackend.
inline bool simulation_mode = false;
struct SimulationText { const wchar_t* base_pt; const wchar_t* pt; const wchar_t* en; };
inline const SimulationText simulation_texts[] = {
    {text::title, L"Crazy Time · Simulação", L"Crazy Time · Simulation"},
    {text::simulation_note, L"Seu computador não será desligado nesta versão.", L"Your computer will not be shut down in this version."},
    {text::settings_note, L"SIMULAÇÃO · Duração e ação são configuradas neste painel.", L"SIMULATION · Duration and action are configured in this panel."},
    {text::schedule, L"Iniciar simulação", L"Start simulation"},
    {text::active, L"Simulação agendada", L"Simulation scheduled"},
    {text::completed, L"Simulação concluída", L"Simulation completed"},
    {text::completed_detail, L"Intenção registrada. Nenhuma ação real executada.", L"Intent recorded. No actual action executed."},
    {text::cancelled_detail, L"Você pode programar uma nova simulação.", L"You can schedule a new simulation."},
};
inline const wchar_t* get_text(unsigned locale, const wchar_t* pt_br, const wchar_t* en) {
    if (simulation_mode)
        for (const auto& item : simulation_texts)
            if (wcscmp(pt_br, item.base_pt) == 0) return locale == 1 ? item.en : item.pt;
    return locale == 1 ? en : pt_br;
}
inline constexpr wchar_t window_class[] = L"PowerTimer.Simulation.Panel";
inline constexpr UINT tray_message = WM_APP + 1;
inline constexpr UINT show_message = WM_APP + 2;
inline constexpr UINT stop_message = WM_APP + 3;
inline constexpr UINT_PTR timer_id = 1;
inline HANDLE stop_event{}, ready_event{}, ui_thread{};
inline std::atomic<HWND> host_window{};
inline std::atomic_bool initialized{};
inline std::atomic_bool user_requested_exit{};
inline bool show_on_start = true;
inline bool dedicated_process = false;
inline void (*log_message)(const wchar_t*) = nullptr;
inline size_t (*mod_storage_path)(wchar_t*, size_t) = nullptr;
inline bool (*notification_sound_setting)() = nullptr;

enum ControlId { action_id = 100, duration_id, schedule_id, cancel_id,
                 preset_id = 110, open_id = 120, settings_id, visibility_id, exit_id, menu_id,
                 list_id = 130, edit_id, pause_id, erase_id, new_id, postpone_id,
                 force_id = 180, kind_id = 140, date_id, time_id, daily_id, weekday_id = 150, theme_id = 160, locale_id = 170 };

inline ClockSample clock_sample() {
    FILETIME file{};
    GetSystemTimeAsFileTime(&file);
    ULARGE_INTEGER ticks{};
    ticks.LowPart = file.dwLowDateTime; ticks.HighPart = file.dwHighDateTime;
    return {(ticks.QuadPart - 116444736000000000ULL) / 10000, GetTickCount64()};
}

inline std::wstring current_user_sid() {
    HANDLE token{};
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return L"";
    DWORD length = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &length);
    std::vector<BYTE> data(length);
    const bool ok = length && GetTokenInformation(token, TokenUser, data.data(), length, &length);
    CloseHandle(token);
    if (!ok) return L"";
    LPWSTR sid{};
    if (!ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(data.data())->User.Sid, &sid)) return L"";
    const std::wstring result(sid);
    LocalFree(sid);
    return result;
}

inline std::wstring legacy_data_file() {
    wchar_t local[MAX_PATH]{};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, local))) return L"";
    const auto directory = std::wstring(local) + L"\\PowerTimer";
    return directory + L"\\schedules-v1.dat";
}

inline std::wstring data_file() {
    if (dedicated_process) {
        wchar_t directory[32768]{};
        const auto sid = current_user_sid();
        if (!mod_storage_path || !mod_storage_path(directory, std::size(directory)) || sid.empty()) return L"";
        const auto path = std::wstring(directory) + L"\\schedules-" + sid + L".dat";
        const DWORD attributes = GetFileAttributesW(path.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES) return path;
        if (GetLastError() != ERROR_FILE_NOT_FOUND) return L"";
        const auto legacy = legacy_data_file();
        if (legacy.empty()) return L"";
        if (!legacy.empty() && !CopyFileW(legacy.c_str(), path.c_str(), TRUE)) {
            const DWORD error = GetLastError();
            // Never replace an existing destination or hide a failed migration.
            if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND &&
                error != ERROR_FILE_EXISTS && error != ERROR_ALREADY_EXISTS) return L"";
        }
        return path;
    }
    wchar_t local[MAX_PATH]{};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, local))) return L"";
    const auto directory = std::wstring(local) + L"\\PowerTimer";
    if (!CreateDirectoryW(directory.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) return L"";
    return directory + L"\\preview-v1.dat";
}

inline void set_text(HWND window, const std::wstring& value) {
    if (!window) return;
    const int count = GetWindowTextLengthW(window);
    std::wstring old(static_cast<size_t>(count) + 1, L'\0');
    GetWindowTextW(window, old.data(), count + 1);
    old.resize(count);
    if (old != value) SetWindowTextW(window, value.c_str());
}

inline std::wstring local_time_at(std::uint64_t utc_ms, unsigned locale) {
    const auto civil = WindowsCalendar{}.local(utc_ms);
    if (!civil) return get_text(locale, text::unavailable_time, text_en::unavailable_time);
    SYSTEMTIME local{};
    local.wYear = static_cast<WORD>(civil->year); local.wMonth = static_cast<WORD>(civil->month);
    local.wDay = static_cast<WORD>(civil->day); local.wHour = static_cast<WORD>(civil->hour);
    local.wMinute = static_cast<WORD>(civil->minute);
    wchar_t date[96]{}, time[96]{};
    if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &local, nullptr, date, 96, nullptr) ||
        !GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &local, nullptr, time, 96))
        return get_text(locale, text::unavailable_time, text_en::unavailable_time);
    return std::wstring(date) + get_text(locale, text::separator, text_en::separator) + time;
}

inline HICON make_icon() {
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = 32;
    info.bmiHeader.biHeight = -32;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits{};
    HBITMAP color = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!color) return nullptr;
    auto* pixels = static_cast<DWORD*>(bits);
    for (int y = 0; y < 32; ++y) for (int x = 0; x < 32; ++x) {
        const double dx = x - 15.5, dy = y - 15.5;
        const double radius = std::sqrt(dx * dx + dy * dy);
        const bool clock = (radius > 9.0 && radius < 11.0) ||
            (x >= 15 && x <= 16 && y >= 8 && y <= 16) ||
            (x >= 16 && x <= 22 && y >= 15 && y <= 16);
        pixels[y * 32 + x] = radius > 15.3 ? 0 : clock ? 0xFFFFFFFF : 0xFF0078D4;
    }
    HBITMAP mask = CreateBitmap(32, 32, 1, 1, nullptr);
    ICONINFO icon_info{TRUE, 0, 0, mask, color};
    HICON icon = mask ? CreateIconIndirect(&icon_info) : nullptr;
    if (mask) DeleteObject(mask);
    DeleteObject(color);
    return icon;
}

// All scheduler and UI state belongs to this one message-loop thread.
class Panel {
    struct Item { HWND handle; int x, y, width, height, font; };
    std::vector<Item> items_;
    HFONT fonts_[4]{};
    HBRUSH background_{}, field_brush_{};
    HICON icon_{};
    SimulatedBackend simulated_backend_;
    WindowsPowerBackend real_backend_;
    PowerBackend& backend() { return simulation_mode ? static_cast<PowerBackend&>(simulated_backend_) : real_backend_; }
    HWND force_{};
    std::vector<std::pair<HWND, std::pair<const wchar_t*, const wchar_t*>>> localized_;
    bool force_on_ = false;
    HWND window_{}, action_{}, duration_{}, preview_{}, status_{}, countdown_{}, detail_{}, error_{}, start_{}, cancel_{}, postpone_{}, theme_combo_{}, locale_combo_{};
    FileScheduleStorage storage_;
    WindowsCalendar calendar_;
    ScheduleBook book_;
    HWND kind_{}, date_{}, time_{}, days_[7]{};
    std::vector<HWND> duration_controls_, once_controls_, weekly_controls_, calendar_controls_;
    HWND list_{}, edit_{}, pause_{}, erase_{};
    std::uint64_t selected_id_{}, editing_id_{};
    std::vector<std::uint64_t> displayed_ids_;
    std::wstring list_signature_;
    std::wstring conflict_signature_;
    bool rebuilding_list_ = false;
    std::unordered_set<std::uint64_t> warned_;
    WarningTracker warnings_;
    NOTIFYICONDATAW tray_{};
    UINT taskbar_created_{};
    UINT dpi_ = 96;
    int scroll_offset_ = 0;
    bool tray_added_ = false;
    std::uint64_t last_tray_try_{};
    COLORREF bg_ = RGB(247, 249, 252), ink_ = RGB(24, 35, 53), accent_ = RGB(0, 120, 212);
    bool high_contrast_ = false, dark_ = false;
    unsigned theme_value_ = 0; // 0=automatic, 1=light, 2=dark
    unsigned locale_ = 1; // 0=pt-BR, 1=English
    bool initialize_locale_ = false;

    int px(int value) const { return MulDiv(value, static_cast<int>(dpi_), 96); }
    void scroll_to(int position) {
        if (!window_) return;
        RECT client{};
        GetClientRect(window_, &client);
        const int limit = (std::max)(0, px(920) - static_cast<int>(client.bottom));
        scroll_offset_ = (std::clamp)(position, 0, limit);
        SCROLLINFO info{};
        info.cbSize = sizeof(info);
        info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        info.nMin = 0;
        info.nMax = px(920) - 1;
        info.nPage = static_cast<UINT>(client.bottom);
        info.nPos = scroll_offset_;
        SetScrollInfo(window_, SB_VERT, &info, TRUE);
        for (const auto& item : items_)
            MoveWindow(item.handle, px(item.x), px(item.y) - scroll_offset_, px(item.width), px(item.height), TRUE);
        InvalidateRect(window_, nullptr, TRUE);
    }
    void reveal(HWND control) {
        if (!control || !window_) return;
        RECT bounds{}, client{};
        GetWindowRect(control, &bounds);
        MapWindowPoints(nullptr, window_, reinterpret_cast<POINT*>(&bounds), 2);
        GetClientRect(window_, &client);
        if (bounds.top < 0) scroll_to(scroll_offset_ + bounds.top);
        else if (bounds.bottom > client.bottom) scroll_to(scroll_offset_ + bounds.bottom - client.bottom);
    }
    HWND add(const wchar_t* type, const wchar_t* label, DWORD style, int id,
             int x, int y, int width, int height, int font = 0, DWORD ex = 0) {
        if (wcscmp(type, L"BUTTON") == 0) style |= BS_NOTIFY;
        HWND control = CreateWindowExW(ex, type, label, WS_CHILD | WS_VISIBLE | style,
            px(x), px(y), px(width), px(height), window_,
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), GetModuleHandleW(nullptr), nullptr);
        if (!control) throw GetLastError();
        items_.push_back({control, x, y, width, height, font});
        return control;
    }
    // Creates a control whose caption follows the selected language.
    HWND add_l(const wchar_t* type, const wchar_t* pt, const wchar_t* en, DWORD style, int id,
               int x, int y, int width, int height, int font = 0, DWORD ex = 0) {
        HWND control = add(type, get_text(locale_, pt, en), style, id, x, y, width, height, font, ex);
        localized_.push_back({control, {pt, en}});
        return control;
    }
    void fill_choice_combos() {
        if (theme_combo_) {
            SendMessageW(theme_combo_, CB_RESETCONTENT, 0, 0);
            for (std::size_t i = 0; i < std::size(text::theme_strings); ++i)
                SendMessageW(theme_combo_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(get_text(locale_, text::theme_strings[i], text_en::theme_strings[i])));
            SendMessageW(theme_combo_, CB_SETCURSEL, static_cast<WPARAM>(theme_value_), 0);
        }
        if (action_) {
            const auto keep = SendMessageW(action_, CB_GETCURSEL, 0, 0);
            SendMessageW(action_, CB_RESETCONTENT, 0, 0);
            for (std::size_t i = 0; i < std::size(text::actions); ++i)
                SendMessageW(action_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(get_text(locale_, text::actions[i], text_en::actions[i])));
            SendMessageW(action_, CB_SETCURSEL, keep < 0 ? 0 : keep, 0);
        }
        if (kind_) {
            const auto keep = SendMessageW(kind_, CB_GETCURSEL, 0, 0);
            SendMessageW(kind_, CB_RESETCONTENT, 0, 0);
            for (std::size_t i = 0; i < std::size(text::kinds); ++i)
                SendMessageW(kind_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(get_text(locale_, text::kinds[i], text_en::kinds[i])));
            SendMessageW(kind_, CB_SETCURSEL, keep < 0 ? 0 : keep, 0);
        }
    }
    void refresh_locale_text() {
        if (!window_) return;
        SetWindowTextW(window_, get_text(locale_, text::title, text_en::title));
        for (const auto& item : localized_) SetWindowTextW(item.first, get_text(locale_, item.second.first, item.second.second));
        // Texts that depend on state.
        set_text(start_, editing_id_ ? get_text(locale_, text::save_schedule, text_en::save_schedule) : get_text(locale_, text::add_schedule, text_en::add_schedule));
        if (pause_) set_text(pause_, selected_record() && selected_record()->paused ? get_text(locale_, text::resume, text_en::resume) : get_text(locale_, text::pause, text_en::pause));
        if (locale_combo_) {
            SendMessageW(locale_combo_, CB_RESETCONTENT, 0, 0);
            SendMessageW(locale_combo_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(get_text(locale_, text::portuguese, text_en::portuguese)));
            SendMessageW(locale_combo_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(get_text(locale_, text::english, text_en::english)));
            SendMessageW(locale_combo_, CB_SETCURSEL, static_cast<WPARAM>(locale_), 0);
        }
        fill_choice_combos();
        for (int i = 0; i < static_cast<int>(std::size(text::presets)); ++i)
            if (HWND preset = GetDlgItem(window_, preset_id + i)) SetWindowTextW(preset, get_text(locale_, text::presets[i].label, text_en::presets[i].label));
        for (int i = 0; i < 7; ++i)
            if (days_[i]) SetWindowTextW(days_[i], get_text(locale_, text::weekdays[i], text_en::weekdays[i]));
        if (list_) {
            const wchar_t* labels[] = {get_text(locale_, text::id_column, text_en::id_column), get_text(locale_, text::action_column, text_en::action_column), get_text(locale_, text::due_column, text_en::due_column), get_text(locale_, text::state_column, text_en::state_column), get_text(locale_, text::rule_column, text_en::rule_column)};
            const int widths[] = {40, 80, 172, 130, 80};
            for (int i = 0; i < 5; ++i) {
                LVCOLUMNW column{}; column.mask = LVCF_TEXT | LVCF_WIDTH;
                column.pszText = const_cast<wchar_t*>(labels[i]); column.cx = px(widths[i]);
                ListView_SetColumn(list_, i, &column);
            }
        }
        list_signature_.clear();

        // Refresh dynamic content
        preview();
        refresh();
    }

    void fonts_and_layout() {
        for (auto& font : fonts_) { if (font) DeleteObject(font); font = nullptr; }
        const int sizes[] = {14, 25, 11, 40};
        for (int i = 0; i < 4; ++i)
            fonts_[i] = CreateFontW(-px(sizes[i]), 0, 0, 0, i == 1 || i == 3 ? FW_SEMIBOLD : FW_NORMAL,
                FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        for (const auto& item : items_) {
            MoveWindow(item.handle, px(item.x), px(item.y) - scroll_offset_, px(item.width), px(item.height), TRUE);
            SendMessageW(item.handle, WM_SETFONT, reinterpret_cast<WPARAM>(fonts_[item.font]), TRUE);
        }
        if (list_) {
            const int widths[] = {40, 80, 172, 130, 80};
            for (int i = 0; i < 5; ++i) ListView_SetColumnWidth(list_, i, px(widths[i]));
        }
    }
    // Get system theme preference: 0=dark, 1=light
    bool get_system_dark_theme() {
        HKEY hKey;
        DWORD value = 0;
        DWORD size = sizeof(DWORD);
        LONG result = RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0,
            KEY_READ,
            &hKey
        );
        if (result == ERROR_SUCCESS) {
            result = RegQueryValueExW(
                hKey,
                L"AppsUseLightTheme",
                nullptr,
                nullptr,
                reinterpret_cast<LPBYTE>(&value),
                &size
            );
            RegCloseKey(hKey);
            if (result == ERROR_SUCCESS) {
                return value == 0; // 0 = dark theme, 1 = light theme
            }
        }
        // Default to light if we can't read the registry
        return false;
    }

    void colors() {
        HIGHCONTRASTW contrast{sizeof(contrast), 0, nullptr};
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0);
        high_contrast_ = (contrast.dwFlags & HCF_HIGHCONTRASTON) != 0;

        dark_ = false;
        if (high_contrast_) {
            bg_ = GetSysColor(COLOR_WINDOW);
            ink_ = GetSysColor(COLOR_WINDOWTEXT);
            accent_ = GetSysColor(COLOR_HIGHLIGHT);
        } else {
            bool use_dark = false;
            switch (theme_value_) {
                case 0: // automatic
                    use_dark = get_system_dark_theme();
                    break;
                case 1: // light
                    use_dark = false;
                    break;
                case 2: // dark
                    use_dark = true;
                    break;
            }

            dark_ = use_dark;
            if (use_dark) {
                // Dark theme colors
                bg_ = RGB(30, 30, 30);
                ink_ = RGB(240, 240, 240);
                accent_ = RGB(0, 120, 212); // Keep our brand blue
            } else {
                // Light theme colors
                bg_ = RGB(247, 249, 252);
                ink_ = RGB(24, 35, 53);
                accent_ = RGB(0, 120, 212); // Our brand blue
            }
        }

        if (background_) DeleteObject(background_);
        background_ = CreateSolidBrush(bg_);
        if (window_) RedrawWindow(window_, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
        if (list_) {
            ListView_SetTextColor(list_, ink_);
            ListView_SetTextBkColor(list_, bg_);
        }
        apply_control_themes();
    }
    // Dark variants of the stock edit/combo/list/scrollbar visuals (Windows 10 1809+ / 11).
    // Looked up at run time so the mod needs no extra import library; harmless if missing.
    void apply_control_themes() {
        using SetWindowThemeFn = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
        static const auto set_theme = reinterpret_cast<SetWindowThemeFn>(
            [] { const auto module = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32); return module ? GetProcAddress(module, "SetWindowTheme") : nullptr; }());
        if (!set_theme) return;
        const bool dark = dark_ && !high_contrast_;
        const auto apply = [&](HWND control, const wchar_t* dark_name) {
            if (control) set_theme(control, dark ? dark_name : nullptr, nullptr);
        };
        apply(duration_, L"DarkMode_CFD");
        for (HWND combo : {action_, kind_, theme_combo_, locale_combo_}) apply(combo, L"DarkMode_CFD");
        apply(date_, L"DarkMode_CFD");
        apply(time_, L"DarkMode_CFD");
        if (list_) {
            ListView_SetTextColor(list_, ink_);
            ListView_SetTextBkColor(list_, bg_);
            ListView_SetBkColor(list_, bg_);
            apply(list_, L"DarkMode_Explorer");
            apply(ListView_GetHeader(list_), L"DarkMode_ItemsView");
        }
    }
    void controls() {
        add_l(L"STATIC", text::brand, text_en::brand, 0, 0, 24, 20, 360, 36, 1);
        if (simulation_mode) add_l(L"STATIC", text::simulation, text_en::simulation, SS_RIGHT, 0, 454, 28, 102, 20, 2);
        add_l(L"STATIC", text::subtitle, text_en::subtitle, 0, 0, 24, 60, 360, 22);
        add_l(L"BUTTON", text::options, text_en::options, WS_TABSTOP | BS_OWNERDRAW, menu_id, 484, 58, 72, 28, 2);

        // Theme selection near other combos for logical tab order
        add_l(L"STATIC", text::theme_label, text_en::theme_label, 0, 0, 24, 100, 254, 22);
        theme_combo_ = add(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL,
                     theme_id, 24, 125, 254, 180);
        SendMessageW(theme_combo_, CB_SETCURSEL, static_cast<WPARAM>(theme_value_), 0);

        // Language selection
        add_l(L"STATIC", text::language_label, text_en::language_label, 0, 0, 24, 170, 254, 22);
        locale_combo_ = add(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL,
                     locale_id, 24, 195, 254, 180);
        SendMessageW(locale_combo_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(get_text(locale_, text::portuguese, text_en::portuguese)));

        // Kind selection: right column, same row as language
        add_l(L"STATIC", text::kind_label, text_en::kind_label, 0, 0, 304, 170, 252, 22);
        kind_ = add(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL,
                    kind_id, 304, 195, 252, 160);
        SendMessageW(locale_combo_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(get_text(locale_, text::english, text_en::english)));
        SendMessageW(locale_combo_, CB_SETCURSEL, static_cast<WPARAM>(book_.get_locale()), 0);

        add_l(L"STATIC", text::action_label, text_en::action_label, 0, 0, 304, 100, 252, 22);
        action_ = add(L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL,
                      action_id, 304, 125, 252, 180);
        SendMessageW(action_, CB_SETCURSEL, 0, 0);
        SendMessageW(kind_, CB_SETCURSEL, 0, 0);
        duration_controls_.push_back(add_l(L"STATIC", text::duration_label, text_en::duration_label, 0, 0, 24, 240, 254, 22));
        duration_ = add(L"EDIT", L"30", WS_TABSTOP | ES_AUTOHSCROLL, duration_id,
                        24, 265, 254, 34, 0, WS_EX_CLIENTEDGE);
        duration_controls_.push_back(duration_);
        SendMessageW(duration_, EM_SETLIMITTEXT, 32, 0);
        duration_controls_.push_back(add_l(L"STATIC", text::duration_hint, text_en::duration_hint, 0, 0, 304, 272, 252, 22, 2));
        for (int i = 0; i < static_cast<int>(std::size(text::presets)); ++i)
            duration_controls_.push_back(add(L"BUTTON", get_text(locale_, text::presets[i].label, text_en::presets[i].label), WS_TABSTOP | BS_OWNERDRAW, preset_id + i,
                24 + 136 * i, 315, 124, 32, 2));
        once_controls_.push_back(add_l(L"STATIC", text::date_label, text_en::date_label, 0, 0, 24, 240, 254, 22));
        date_ = add(DATETIMEPICK_CLASSW, L"", WS_TABSTOP | DTS_SHORTDATEFORMAT, date_id, 24, 265, 254, 34);
        SYSTEMTIME range[2]{}; range[0].wYear = 1970; range[0].wMonth = 1; range[0].wDay = 1;
        range[1].wYear = 9999; range[1].wMonth = 12; range[1].wDay = 31;
        DateTime_SetRange(date_, GDTR_MIN | GDTR_MAX, range);
        once_controls_.push_back(date_);
        calendar_controls_.push_back(add_l(L"STATIC", text::time_label, text_en::time_label, 0, 0, 304, 240, 252, 22));
        time_ = add(DATETIMEPICK_CLASSW, L"", WS_TABSTOP | DTS_TIMEFORMAT | DTS_UPDOWN, time_id, 304, 265, 252, 34);
        SendMessageW(time_, DTM_SETFORMATW, 0, reinterpret_cast<LPARAM>(L"HH:mm"));
        calendar_controls_.push_back(time_);
        weekly_controls_.push_back(add_l(L"STATIC", text::weekly_label, text_en::weekly_label, 0, 0, 24, 240, 254, 22, 2));
        weekly_controls_.push_back(add_l(L"BUTTON", text::daily, text_en::daily, WS_TABSTOP | BS_OWNERDRAW, daily_id, 24, 265, 254, 34, 2));
        for (int i = 0; i < 7; ++i) {
            days_[i] = add(L"BUTTON", get_text(locale_, text::weekdays[i], text_en::weekdays[i]), WS_TABSTOP | BS_AUTOCHECKBOX, weekday_id + i,
                           24 + 76 * i, 315, 74, 30, 2);
            SendMessageW(days_[i], BM_SETCHECK, BST_CHECKED, 0);
            weekly_controls_.push_back(days_[i]);
        }
        preview_ = add(L"STATIC", L"", 0, 0, 24, 360, 300, 30, 2);
        force_ = add_l(L"BUTTON", text::force_label, text_en::force_label, WS_TABSTOP | BS_OWNERDRAW, force_id, 330, 360, 226, 30, 2);
        start_ = add_l(L"BUTTON", text::add_schedule, text_en::add_schedule, WS_TABSTOP | BS_OWNERDRAW, schedule_id,
                     24, 400, 344, 36);
        add_l(L"BUTTON", text::new_schedule, text_en::new_schedule, WS_TABSTOP | BS_OWNERDRAW, new_id, 380, 400, 176, 36);
        add_l(L"STATIC", text::schedules, text_en::schedules, 0, 0, 24, 450, 532, 22);
        list_ = add(WC_LISTVIEWW, L"", WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
                    list_id, 24, 476, 532, 120, 2, WS_EX_CLIENTEDGE);
        ListView_SetExtendedListViewStyle(list_, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
        const wchar_t* labels[] = {get_text(locale_, text::id_column, text_en::id_column), get_text(locale_, text::action_column, text_en::action_column), get_text(locale_, text::due_column, text_en::due_column), get_text(locale_, text::state_column, text_en::state_column), get_text(locale_, text::rule_column, text_en::rule_column)};
        const int widths[] = {40, 80, 172, 130, 80};
        for (int i = 0; i < 5; ++i) {
            LVCOLUMNW column{}; column.mask = LVCF_TEXT | LVCF_WIDTH;
            column.pszText = const_cast<wchar_t*>(labels[i]); column.cx = px(widths[i]);
            ListView_InsertColumn(list_, i, &column);
        }
        edit_ = add_l(L"BUTTON", text::edit, text_en::edit, WS_TABSTOP | BS_OWNERDRAW, edit_id, 24, 608, 76, 36, 2);
        pause_ = add_l(L"BUTTON", text::pause, text_en::pause, WS_TABSTOP | BS_OWNERDRAW, pause_id, 108, 608, 84, 36, 2);
        erase_ = add_l(L"BUTTON", text::erase, text_en::erase, WS_TABSTOP | BS_OWNERDRAW, erase_id, 200, 608, 76, 36, 2);
        cancel_ = add_l(L"BUTTON", text::cancel, text_en::cancel, WS_TABSTOP | BS_OWNERDRAW, cancel_id, 284, 608, 140, 36, 2);
        postpone_ = add_l(L"BUTTON", text::postpone, text_en::postpone, WS_TABSTOP | BS_OWNERDRAW, postpone_id, 432, 608, 124, 36, 2);
        status_ = add_l(L"STATIC", text::idle, text_en::idle, SS_CENTER, 0, 24, 660, 532, 24);
        countdown_ = add_l(L"STATIC", text::clock_empty, text_en::clock_empty, SS_CENTER, 0, 24, 688, 532, 56, 3);
        detail_ = add_l(L"STATIC", text::idle_detail, text_en::idle_detail, SS_CENTER, 0, 24, 748, 532, 40, 2);
        error_ = add(L"STATIC", L"", SS_CENTER, 0, 24, 792, 532, 42, 2);
        add_l(L"STATIC", text::simulation_note, text_en::simulation_note, SS_CENTER, 0, 24, 842, 532, 22, 2);
        add_l(L"STATIC", text::settings_note, text_en::settings_note, SS_CENTER, 0, 24, 872, 532, 22, 2);
        fill_choice_combos();
        apply_control_themes();
        fonts_and_layout();
        show_kind();
    }
    std::wstring duration_text() const {
        wchar_t value[64]{};
        GetWindowTextW(duration_, value, 64);
        return value;
    }
    Action selected_action() const {
        return static_cast<Action>(SendMessageW(action_, CB_GETCURSEL, 0, 0));
    }
    ScheduleKind selected_kind() const {
        return static_cast<ScheduleKind>(SendMessageW(kind_, CB_GETCURSEL, 0, 0));
    }
    CivilTime selected_civil() const {
        SYSTEMTIME date{}, time{};
        if (DateTime_GetSystemtime(date_, &date) != GDT_VALID ||
            DateTime_GetSystemtime(time_, &time) != GDT_VALID) return {};
        return {date.wYear, date.wMonth, date.wDay, time.wHour, time.wMinute};
    }
    unsigned selected_days() const {
        unsigned mask = 0;
        for (unsigned i = 0; i < 7; ++i)
            if (SendMessageW(days_[i], BM_GETCHECK, 0, 0) == BST_CHECKED) mask |= 1u << i;
        return mask;
    }
    void show_kind() {
        const auto kind = selected_kind();
        for (const auto control : duration_controls_) ShowWindow(control, kind == ScheduleKind::Duration ? SW_SHOW : SW_HIDE);
        for (const auto control : once_controls_) ShowWindow(control, kind == ScheduleKind::Once ? SW_SHOW : SW_HIDE);
        for (const auto control : weekly_controls_) ShowWindow(control, kind == ScheduleKind::Weekly ? SW_SHOW : SW_HIDE);
        for (const auto control : calendar_controls_) ShowWindow(control, kind != ScheduleKind::Duration ? SW_SHOW : SW_HIDE);
        preview();
    }
    void preview() {
        if (!preview_) return;
        const auto now = clock_sample();
        const auto kind = selected_kind();
        const auto civil = selected_civil();
        std::optional<std::uint64_t> due;
        if (kind == ScheduleKind::Duration) {
            const auto minutes = Scheduler::parse_duration(duration_text());
            if (minutes && now.utc_ms <= UINT64_MAX - *minutes * 60000) due = now.utc_ms + *minutes * 60000;
        } else if (kind == ScheduleKind::Once) due = book_.preview_once(civil, now);
        else if (kind == ScheduleKind::Weekly) due = book_.preview_weekly(selected_days(), civil.hour * 60 + civil.minute, now);
        const auto action = static_cast<int>(selected_action());
        if (!due || action < 0 || action > 2) {
            set_text(preview_, kind == ScheduleKind::Duration ? get_text(locale_, text::invalid_preview, text_en::invalid_preview) : get_text(locale_, text::calendar_preview_error, text_en::calendar_preview_error));
            return;
        }
        set_text(preview_, std::wstring(get_text(locale_, text::preview_prefix, text_en::preview_prefix)) + get_text(locale_, text::actions[action], text_en::actions[action]) +
            get_text(locale_, text::separator, text_en::separator) + local_time_at(*due, locale_));
    }
    void notify(const wchar_t* title, const wchar_t* body) {
        if (!tray_added_) return;
        NOTIFYICONDATAW notice = tray_;
        notice.uFlags = NIF_INFO;
        wcsncpy_s(notice.szInfoTitle, title, _TRUNCATE);
        wcsncpy_s(notice.szInfo, body, _TRUNCATE);
        // Respect sound setting: NIIF_NOSOUND only if sound disabled
        const bool sound = notification_sound_setting ? notification_sound_setting() : book_.get_sound_enabled();
        const UINT sound_flag = sound ? 0 : NIIF_NOSOUND;
        notice.dwInfoFlags = NIIF_INFO | sound_flag | NIIF_RESPECT_QUIET_TIME;
        Shell_NotifyIconW(NIM_MODIFY, &notice);
    }
    const ScheduleRecord* selected_record() const {
        for (const auto& record : book_.list()) if (record.id == selected_id_) return &record;
        return nullptr;
    }
    const wchar_t* state_label(const ScheduleRecord& record) {
        if (record.conflict_with) return get_text(locale_, text::state_conflict, text_en::state_conflict);
        if (record.state == OccurrenceState::Scheduled && record.paused) return get_text(locale_, text::state_paused, text_en::state_paused);
        switch (record.state) {
            case OccurrenceState::Scheduled: return get_text(locale_, text::state_scheduled, text_en::state_scheduled);
            case OccurrenceState::Cancelled: return get_text(locale_, text::state_cancelled, text_en::state_cancelled);
            case OccurrenceState::Expired: return get_text(locale_, text::state_expired, text_en::state_expired);
            case OccurrenceState::Uncertain: return get_text(locale_, text::state_uncertain, text_en::state_uncertain);
            case OccurrenceState::Completed: return get_text(locale_, text::state_completed, text_en::state_completed);
        }
        return get_text(locale_, text::state_uncertain, text_en::state_uncertain);
    }
    void show_error() {
        if (book_.error() == BookError::Conflict) {
            std::wstring message = std::wstring(get_text(locale_, text::conflict_error, text_en::conflict_error)) + get_text(locale_, text::schedule_number, text_en::schedule_number) + std::to_wstring(book_.conflicting_id());
            for (const auto& record : book_.list()) if (record.id == book_.conflicting_id()) {
                message += std::wstring(get_text(locale_, text::separator, text_en::separator)) + get_text(locale_, text::actions[static_cast<int>(record.action)], text_en::actions[static_cast<int>(record.action)]) +
                    std::wstring(get_text(locale_, text::separator, text_en::separator)) + local_time_at(effective_due_utc_ms(record), locale_);
                break;
            }
            set_text(error_, message);
            return;
        }
        const wchar_t* message = L"";
        switch (book_.error()) {
            case BookError::InvalidInput: message = selected_kind() == ScheduleKind::Duration ? get_text(locale_, text::record_input_error, text_en::record_input_error) : get_text(locale_, text::calendar_input_error, text_en::calendar_input_error); break;
            case BookError::Calendar: message = get_text(locale_, text::calendar_error, text_en::calendar_error); break;
            case BookError::NotFound: message = get_text(locale_, text::missing_record, text_en::missing_record); break;
            case BookError::Storage: message = get_text(locale_, text::storage_error, text_en::storage_error); break;
            case BookError::InvalidData: message = get_text(locale_, text::invalid_data, text_en::invalid_data); break;
            case BookError::NotReady: message = get_text(locale_, text::not_ready, text_en::not_ready); break;
            case BookError::None: break;
            case BookError::Conflict: break;
        }
        set_text(error_, message);
    }
    void sync_list() {
        std::wstring signature;
        for (const auto& record : book_.list()) {
            signature += std::to_wstring(record.id) + L":" + std::to_wstring(record.occurrence_id) + L":" +
                std::to_wstring(record.due_utc_ms) + L":" + std::to_wstring(static_cast<int>(record.state)) +
                (record.paused ? L"p:" : L"a:") + std::to_wstring(record.conflict_with) + L";";
        }
        if (signature == list_signature_) return;
        list_signature_ = signature;
        if (!selected_record()) selected_id_ = book_.list().empty() ? 0 : book_.list().front().id;
        rebuilding_list_ = true;
        SendMessageW(list_, WM_SETREDRAW, FALSE, 0);
        ListView_DeleteAllItems(list_);
        displayed_ids_.clear();
        for (const auto& record : book_.list()) {
            const int row = static_cast<int>(displayed_ids_.size());
            displayed_ids_.push_back(record.id);
            LVITEMW item{}; item.mask = LVIF_TEXT; item.iItem = row;
            auto number = std::to_wstring(record.id);
            item.pszText = number.data();
            ListView_InsertItem(list_, &item);
            auto due = record.state == OccurrenceState::Scheduled ? local_time_at(effective_due_utc_ms(record), locale_) : std::wstring(get_text(locale_, text::no_next, text_en::no_next));
            auto rule = record.kind == ScheduleKind::Duration ? std::to_wstring(record.minutes) + L" min" :
                std::wstring(record.kind == ScheduleKind::Once ? get_text(locale_, text::once_rule, text_en::once_rule) : get_text(locale_, text::weekly_rule, text_en::weekly_rule));
            auto action_text = std::wstring(get_text(locale_, text::actions[static_cast<int>(record.action)], text_en::actions[static_cast<int>(record.action)]));
            if (record.force_shutdown) action_text += get_text(locale_, text::forced_tag, text_en::forced_tag);
            ListView_SetItemText(list_, row, 1, action_text.data());
            ListView_SetItemText(list_, row, 2, due.data());
            ListView_SetItemText(list_, row, 3, const_cast<wchar_t*>(state_label(record)));
            ListView_SetItemText(list_, row, 4, rule.data());
            if (record.id == selected_id_) ListView_SetItemState(list_, row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        }
        SendMessageW(list_, WM_SETREDRAW, TRUE, 0);
        InvalidateRect(list_, nullptr, TRUE);
        rebuilding_list_ = false;
    }
    void refresh() {
        if (!countdown_) return;
        const auto now = clock_sample();
        for (const auto& delivery : book_.tick(now)) {
            if (log_message) log_message(get_text(locale_, text::completed_detail, text_en::completed_detail));
            const auto exec = execute_power_action(backend(), delivery.action, delivery.force);
            if (exec.status != ExecStatus::Accepted) {
                // Intent stays Uncertain: never retried, next weekly occurrence untouched.
                notify(get_text(locale_, text::state_uncertain, text_en::state_uncertain),
                    exec.status == ExecStatus::NotAvailable
                        ? get_text(locale_, text::power_unavailable, text_en::power_unavailable)
                        : get_text(locale_, text::power_failed, text_en::power_failed));
                continue;
            }
            const bool recorded = book_.complete(delivery);
            if (!recorded) show_error();
            notify(recorded ? get_text(locale_, text::completed, text_en::completed) : get_text(locale_, text::state_uncertain, text_en::state_uncertain),
                recorded ? get_text(locale_, text::completed_detail, text_en::completed_detail) : get_text(locale_, text::uncertain_detail, text_en::uncertain_detail));
            if (!recorded) break;
        }
        // Countdown warnings: 30, 15, 10, 5 and 1 minute (see WarningTracker).
        for (const auto& record : book_.list()) {
            if (!book_.writable()) break;
            if (record.state != OccurrenceState::Scheduled || record.paused || record.conflict_with) continue;
            const auto index = warnings_.due(record.occurrence_id, book_.remaining_ms(record.id, now));
            if (!index) continue;
            const wchar_t* title = get_text(locale_, text::warning, text_en::warning);
            const wchar_t* body = get_text(locale_, text::countdown_notification, text_en::countdown_notification);
            switch (*index) {
                case 0: title = get_text(locale_, text::warning_30min, text_en::warning_30min);
                        body = get_text(locale_, text::warning_30min_detail, text_en::warning_30min_detail); break;
                case 1: title = get_text(locale_, text::warning_15min, text_en::warning_15min);
                        body = get_text(locale_, text::warning_15min_detail, text_en::warning_15min_detail); break;
                case 2: title = get_text(locale_, text::warning_10min, text_en::warning_10min);
                        body = get_text(locale_, text::warning_10min_detail, text_en::warning_10min_detail); break;
                case 3: title = get_text(locale_, text::warning_5min, text_en::warning_5min);
                        body = get_text(locale_, text::warning_5min_detail, text_en::warning_5min_detail); break;
                default: break; // 1 minute: the default texts above
            }
            notify(title, body);
            // Only the 1-minute warning brings the panel up, to avoid too many popups.
            if (*index == 4) show(false);
        }
        std::wstring conflicts;
        for (const auto& record : book_.list()) if (record.conflict_with)
            conflicts += std::to_wstring(record.id) + L":" + std::to_wstring(record.conflict_with) + L";";
        if (conflicts != conflict_signature_) {
            conflict_signature_ = conflicts;
            if (!conflicts.empty()) notify(get_text(locale_, text::state_conflict, text_en::state_conflict), get_text(locale_, text::conflict_notification, text_en::conflict_notification));
        }
        sync_list();
        const auto* record = selected_record();
        const bool writable = book_.writable();
        const bool scheduled = record && record->state == OccurrenceState::Scheduled;
        EnableWindow(start_, writable);
        EnableWindow(edit_, writable && record);
        EnableWindow(erase_, writable && record);
        EnableWindow(pause_, writable && scheduled);
        EnableWindow(cancel_, writable && scheduled);
        EnableWindow(postpone_, writable && scheduled);
        set_text(pause_, record && record->paused ? get_text(locale_, text::resume, text_en::resume) : get_text(locale_, text::pause, text_en::pause));
        set_text(start_, editing_id_ ? get_text(locale_, text::save_schedule, text_en::save_schedule) : get_text(locale_, text::add_schedule, text_en::add_schedule));
        if (record) {
            set_text(status_, state_label(*record));
            if (scheduled && !record->paused && !record->conflict_with) {
                const auto seconds = (book_.remaining_ms(record->id, now) + 999) / 1000;
                wchar_t value[48]{};
                swprintf_s(value, L"%02llu:%02llu:%02llu", seconds / 3600, (seconds / 60) % 60, seconds % 60);
                set_text(countdown_, value);
            } else set_text(countdown_, get_text(locale_, text::clock_empty, text_en::clock_empty));
            const wchar_t* note = record->state == OccurrenceState::Uncertain ? get_text(locale_, text::uncertain_detail, text_en::uncertain_detail) :
                record->paused && scheduled ? get_text(locale_, text::paused_detail, text_en::paused_detail) : nullptr;
            std::wstring detail = note ? std::wstring(note) : std::wstring(get_text(locale_, text::actions[static_cast<int>(record->action)], text_en::actions[static_cast<int>(record->action)])) +
                get_text(locale_, text::separator, text_en::separator) + local_time_at(effective_due_utc_ms(*record), locale_);
            if (record->conflict_with) detail = std::wstring(get_text(locale_, text::conflict_detail, text_en::conflict_detail)) +
                std::to_wstring(record->conflict_with) + get_text(locale_, text::conflict_hint, text_en::conflict_hint);
            else if (record->kind == ScheduleKind::Weekly) {
                detail += L"\n";
                for (unsigned i = 0; i < 7; ++i) if (record->weekdays & (1u << i)) detail += std::wstring(get_text(locale_, text::weekdays[i], text_en::weekdays[i])) + L" ";
                if (record->last_occurrence_id) {
                    auto last = *record; last.state = record->last_state; last.paused = false; last.conflict_with = 0;
                    detail += std::wstring(get_text(locale_, text::separator, text_en::separator)) + get_text(locale_, text::last_result, text_en::last_result) + state_label(last);
                }
            }
            set_text(detail_, detail);
        } else {
            set_text(status_, get_text(locale_, text::idle, text_en::idle));
            set_text(countdown_, get_text(locale_, text::clock_empty, text_en::clock_empty));
            set_text(detail_, get_text(locale_, text::idle_detail, text_en::idle_detail));
        }
        if (!writable) show_error();
        preview();
        if (!tray_added_ && now.monotonic_ms - last_tray_try_ >= 5000) add_tray();
    }
    // Forced close only applies to Shutdown; any other action clears it.
    void set_force_on(bool on) { force_on_ = on; if (force_) InvalidateRect(force_, nullptr, TRUE); }
    void sync_force() {
        if (!force_) return;
        const bool shutdown = selected_action() == Action::Shutdown;
        if (!shutdown) set_force_on(false);
        EnableWindow(force_, shutdown);
    }
    void confirm_force() {
        set_force_on(!force_on_);  // owner-drawn: toggle by hand
        if (!force_on_) return;
        if (MessageBoxW(window_, get_text(locale_, text::force_warning, text_en::force_warning),
                get_text(locale_, text::force_title, text_en::force_title), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES)
            set_force_on(false);
    }
    bool force_checked() const {
        return force_ && force_on_ && selected_action() == Action::Shutdown;
    }
    void command(int id) {
        if (id >= preset_id && id < preset_id + static_cast<int>(std::size(text::presets))) {
            const auto minutes = std::to_wstring(text::presets[id - preset_id].minutes);
            SetWindowTextW(duration_, minutes.c_str()); SetFocus(duration_); SendMessageW(duration_, EM_SETSEL, 0, -1);
        } else if (id == schedule_id) {
            const bool editing = editing_id_ != 0;
            const auto now = clock_sample();
            const auto action = selected_action();
            const auto civil = selected_civil();
            bool ok = false;
            if (backend().query().of(action) != Availability::Available) {
                set_text(error_, get_text(locale_, text::power_unavailable, text_en::power_unavailable));
                return;
            }
            switch (selected_kind()) {
                case ScheduleKind::Duration: ok = editing ? book_.edit(editing_id_, action, duration_text(), now) :
                    book_.create(action, duration_text(), now); break;
                case ScheduleKind::Once: ok = editing ? book_.edit_once(editing_id_, action, civil, now) :
                    book_.create_once(action, civil, now); break;
                case ScheduleKind::Weekly: ok = editing ? book_.edit_weekly(editing_id_, action, selected_days(), civil.hour * 60 + civil.minute, now) :
                    book_.create_weekly(action, selected_days(), civil.hour * 60 + civil.minute, now); break;
            }
            if (ok) {
                selected_id_ = editing ? editing_id_ : book_.list().back().id;
                ok = book_.set_force_shutdown(selected_id_, force_checked());
                editing_id_ = 0;
            }
            show_error(); refresh();
        } else if (id == new_id) {
            editing_id_ = 0; set_text(error_, L"");
            set_force_on(false); sync_force(); refresh();
            SetFocus(selected_kind() == ScheduleKind::Duration ? duration_ : time_);
        } else if (id == edit_id) {
            if (const auto* record = selected_record()) {
                editing_id_ = record->id;
                SendMessageW(action_, CB_SETCURSEL, static_cast<WPARAM>(record->action), 0);
                set_force_on(record->force_shutdown); sync_force();
                SendMessageW(kind_, CB_SETCURSEL, static_cast<WPARAM>(record->kind), 0);
                set_text(duration_, std::to_wstring(record->minutes));
                if (record->kind != ScheduleKind::Duration) {
                    const auto civil = record->kind == ScheduleKind::Once ? calendar_.local(record->due_utc_ms).value_or(record->civil) : record->civil;
                    SYSTEMTIME selected{};
                    selected.wYear = static_cast<WORD>(civil.year); selected.wMonth = static_cast<WORD>(civil.month);
                    selected.wDay = static_cast<WORD>(civil.day); selected.wHour = static_cast<WORD>(civil.hour); selected.wMinute = static_cast<WORD>(civil.minute);
                    DateTime_SetSystemtime(date_, GDT_VALID, &selected); DateTime_SetSystemtime(time_, GDT_VALID, &selected);
                    for (unsigned i = 0; i < 7; ++i) SendMessageW(days_[i], BM_SETCHECK, record->weekdays & (1u << i) ? BST_CHECKED : BST_UNCHECKED, 0);
                }
                const auto focus = record->kind == ScheduleKind::Duration ? duration_ : time_;
                show_kind(); refresh(); SetFocus(focus);
            }
        } else if (id == pause_id || id == erase_id || id == cancel_id || id == postpone_id) {
            if (const auto* record = selected_record()) {
                const auto selected = record->id;
                const auto occurrence = record->occurrence_id;
                if (id == postpone_id) {
                    const auto now = clock_sample();
                    if (book_.postpone(selected, 10, now)) {
                        // Only warnings still ahead of the new deadline may fire again.
                        warnings_.reopen_ahead_of(occurrence, book_.remaining_ms(selected, now));
                    }
                } else if (id == pause_id) book_.set_paused(selected, !record->paused, clock_sample());
                else if (id == erase_id) {
                    if (book_.erase(selected) && editing_id_ == selected) editing_id_ = 0;
                } else book_.cancel(selected);
                show_error(); refresh();
            }
        } else if (id == daily_id) {
            for (auto day : days_) SendMessageW(day, BM_SETCHECK, BST_CHECKED, 0);
            preview();
        } else if (id >= weekday_id && id < weekday_id + 7) {
            preview();
        } else if (id == open_id || id == settings_id) {
            show(true);
            if (id == settings_id && theme_combo_) {
                // Settings are theme and language: jump to them and open the theme list.
                set_text(error_, L"");
                SetFocus(theme_combo_);
                SendMessageW(theme_combo_, CB_SHOWDROPDOWN, TRUE, 0);
            } else if (id == open_id) {
                SetWindowPos(window_, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                FlashWindow(window_, FALSE);
            }
        } else if (id == visibility_id) {
            const auto result = reinterpret_cast<INT_PTR>(ShellExecuteW(window_, L"open",
                L"ms-settings:taskbar", nullptr, nullptr, SW_SHOWNORMAL));
            if (result <= 32) { show(true); set_text(error_, get_text(locale_, text::settings_error, text_en::settings_error)); }
        } else if (id == menu_id) {
            menu();
        } else if (id == exit_id) {
            user_requested_exit.store(true); SetEvent(stop_event); DestroyWindow(window_);
        }
    }
    void menu() {
        HMENU menu = CreatePopupMenu();
        if (!menu) return;
        // Open/Settings only make sense while the panel is hidden or minimised.
        if (!IsWindowVisible(window_) || IsIconic(window_)) {
            AppendMenuW(menu, MF_STRING, open_id, get_text(locale_, text::open, text_en::open));
            AppendMenuW(menu, MF_STRING, settings_id, get_text(locale_, text::settings, text_en::settings));
        }
        AppendMenuW(menu, MF_STRING, visibility_id, get_text(locale_, text::notification_settings, text_en::notification_settings));
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING | (book_.writable() && selected_record() && selected_record()->state == OccurrenceState::Scheduled ? MF_ENABLED : MF_GRAYED), postpone_id, get_text(locale_, text::postpone, text_en::postpone));
        AppendMenuW(menu, MF_STRING | (book_.writable() && selected_record() && selected_record()->state == OccurrenceState::Scheduled ? MF_ENABLED : MF_GRAYED), cancel_id, get_text(locale_, text::cancel, text_en::cancel));
        AppendMenuW(menu, MF_STRING, exit_id, get_text(locale_, text::exit, text_en::exit));
        POINT point{};
        GetCursorPos(&point);
        SetForegroundWindow(window_);
        const int chosen = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, point.x, point.y, 0, window_, nullptr);
        DestroyMenu(menu);
        if (chosen) command(chosen);
        if (window_) PostMessageW(window_, WM_NULL, 0, 0);
        if (tray_added_) Shell_NotifyIconW(NIM_SETFOCUS, &tray_);
    }
    // Warning-coloured checkbox, drawn by hand so it follows light/dark/high contrast.
    void draw_force(const DRAWITEMSTRUCT& draw, bool use_dark, bool disabled) {
        const COLORREF fill = high_contrast_ ? GetSysColor(COLOR_BTNFACE) : use_dark ? RGB(96, 62, 0) : RGB(255, 233, 170);
        const COLORREF edge = high_contrast_ ? GetSysColor(COLOR_BTNTEXT) : use_dark ? RGB(255, 190, 70) : RGB(200, 120, 0);
        const COLORREF text = disabled ? GetSysColor(COLOR_GRAYTEXT) : high_contrast_ ? GetSysColor(COLOR_BTNTEXT) :
            use_dark ? RGB(255, 236, 190) : RGB(60, 38, 0);
        FillRect(draw.hDC, &draw.rcItem, background_);
        HBRUSH brush = CreateSolidBrush(fill);
        HPEN pen = CreatePen(PS_SOLID, px(2), edge);
        const auto old_brush = SelectObject(draw.hDC, brush);
        const auto old_pen = SelectObject(draw.hDC, pen);
        RoundRect(draw.hDC, draw.rcItem.left, draw.rcItem.top, draw.rcItem.right, draw.rcItem.bottom, px(10), px(10));
        const int box = px(18), top = (draw.rcItem.top + draw.rcItem.bottom - box) / 2, left = draw.rcItem.left + px(10);
        SelectObject(draw.hDC, GetStockObject(NULL_BRUSH));
        Rectangle(draw.hDC, left, top, left + box, top + box);
        if (force_on_) {
            MoveToEx(draw.hDC, left + px(4), top + box / 2, nullptr);
            LineTo(draw.hDC, left + box / 2 - px(1), top + box - px(5));
            LineTo(draw.hDC, left + box - px(4), top + px(4));
        }
        SelectObject(draw.hDC, old_brush); SelectObject(draw.hDC, old_pen);
        DeleteObject(brush); DeleteObject(pen);
        HFONT old_font = fonts_[2] ? static_cast<HFONT>(SelectObject(draw.hDC, fonts_[2])) : nullptr;
        wchar_t label[128]{};
        GetWindowTextW(draw.hwndItem, label, 128);
        SetBkMode(draw.hDC, TRANSPARENT);
        SetTextColor(draw.hDC, text);
        RECT rect = draw.rcItem; rect.left = left + box + px(8); rect.right -= px(6);
        DrawTextW(draw.hDC, label, -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        if (draw.itemState & ODS_FOCUS) { RECT focus = draw.rcItem; InflateRect(&focus, -px(3), -px(3)); DrawFocusRect(draw.hDC, &focus); }
        if (old_font) SelectObject(draw.hDC, old_font);
    }
    void draw_button(const DRAWITEMSTRUCT& draw) {
        const bool primary = draw.CtlID == schedule_id;
        const bool disabled = (draw.itemState & ODS_DISABLED) != 0;

        const bool use_dark = dark_ && !high_contrast_;

        const COLORREF fill = primary && !disabled ? accent_ : high_contrast_ ? GetSysColor(COLOR_BTNFACE) :
            disabled ? (use_dark ? RGB(44, 44, 44) : RGB(238, 241, 246)) :
            use_dark ? RGB(60, 60, 60) : RGB(233, 239, 248);
        const COLORREF foreground = disabled ? (high_contrast_ ? GetSysColor(COLOR_GRAYTEXT) : use_dark ? RGB(130, 130, 130) : RGB(140, 150, 165)) : primary ?
            (high_contrast_ ? GetSysColor(COLOR_BTNTEXT) : RGB(255, 255, 255)) :
            use_dark ? RGB(240, 240, 240) : RGB(24, 35, 53);

        if (draw.CtlID == force_id) { draw_force(draw, use_dark, disabled); return; }
        // Pressed buttons darken; a thin border separates buttons from the page; disabled ones stay flat.
        const bool pressed = (draw.itemState & ODS_SELECTED) != 0 && !disabled;
        const auto shade = [](COLORREF c, int delta) {
            const auto clamp = [](int v) { return v < 0 ? 0 : v > 255 ? 255 : v; };
            return RGB(clamp(GetRValue(c) + delta), clamp(GetGValue(c) + delta), clamp(GetBValue(c) + delta));
        };
        const COLORREF body = pressed ? shade(fill, use_dark ? 22 : -18) : fill;
        const COLORREF edge = high_contrast_ ? GetSysColor(COLOR_BTNTEXT) : disabled ? fill :
            primary ? shade(accent_, -40) : use_dark ? RGB(96, 96, 96) : RGB(190, 203, 224);
        // Owner-draw leaves the rounded corners untouched: paint the page colour first.
        FillRect(draw.hDC, &draw.rcItem, background_);
        HBRUSH brush = CreateSolidBrush(body);
        HPEN pen = CreatePen(PS_SOLID, 1, edge);
        const auto old_brush = SelectObject(draw.hDC, brush);
        const auto old_pen = SelectObject(draw.hDC, pen);
        RoundRect(draw.hDC, draw.rcItem.left, draw.rcItem.top, draw.rcItem.right, draw.rcItem.bottom, px(14), px(14));
        SelectObject(draw.hDC, old_brush); SelectObject(draw.hDC, old_pen);
        DeleteObject(brush); DeleteObject(pen);

        // Find font for this button
        int font_index = 2; // default font
        for (const auto& item : items_) {
            if (item.handle == draw.hwndItem) {
                font_index = item.font;
                break;
            }
        }
        HFONT old_font = nullptr;
        if (fonts_[font_index]) {
            old_font = static_cast<HFONT>(SelectObject(draw.hDC, fonts_[font_index]));
        }

        wchar_t label[128]{};
        GetWindowTextW(draw.hwndItem, label, 128);
        SetBkMode(draw.hDC, TRANSPARENT);
        SetTextColor(draw.hDC, foreground);
        RECT rect = draw.rcItem;
        if (draw.itemState & ODS_SELECTED) OffsetRect(&rect, 0, 1);
        DrawTextW(draw.hDC, label, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        if (draw.itemState & ODS_FOCUS) { InflateRect(&rect, -px(4), -px(4)); DrawFocusRect(draw.hDC, &rect); }

        if (old_font) {
            SelectObject(draw.hDC, old_font);
        }
    }
public:
    Panel() : storage_(data_file()), book_(storage_, calendar_) {
        // Load theme and locale preference from storage
        auto loaded = storage_.load();
        if (loaded.status == power_timer::LoadStatus::Loaded) {
            theme_value_ = loaded.snapshot.theme;
            locale_ = loaded.snapshot.locale;
        } else if (loaded.status == LoadStatus::Missing) {
            locale_ = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_PORTUGUESE ? 0 : 1;
            initialize_locale_ = true;
        }
        // Apply theme immediately
        colors();
        refresh_locale_text();
    }
    ~Panel() {
        if (window_ && IsWindow(window_)) DestroyWindow(window_);
        for (auto font : fonts_) if (font) DeleteObject(font);
        if (background_) DeleteObject(background_);
        if (field_brush_) DeleteObject(field_brush_);
        if (icon_) DestroyIcon(icon_);
    }
    void show(bool activate) {
        ShowWindow(window_, activate ? SW_RESTORE : SW_SHOWNOACTIVATE);
        if (activate) { SetForegroundWindow(window_); SetFocus(selected_kind() == ScheduleKind::Duration ? duration_ : time_); }
    }
    void add_tray() {
        last_tray_try_ = GetTickCount64();
        tray_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
        tray_added_ = Shell_NotifyIconW(NIM_ADD, &tray_) != FALSE;
        if (tray_added_) {
            tray_.uVersion = NOTIFYICON_VERSION_4;
            Shell_NotifyIconW(NIM_SETVERSION, &tray_);
            if (error_) set_text(error_, L"");
        } else {
            if (error_) set_text(error_, get_text(locale_, text::tray_error, text_en::tray_error));
            if (log_message) log_message(L"Tray icon registration failed; will retry without opening the panel.");
        }
    }
    bool create() {
        colors();
        icon_ = make_icon();
        taskbar_created_ = RegisterWindowMessageW(L"TaskbarCreated");
        WNDCLASSW cls{};
        cls.lpfnWndProc = procedure;
        cls.hInstance = GetModuleHandleW(nullptr);
        cls.hIcon = icon_;
        cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        cls.lpszClassName = window_class;
        if (!RegisterClassW(&cls)) return false;
        dpi_ = GetDpiForSystem();
        RECT work{};
        // Position window on the monitor where the tray icon / cursor is
        POINT cursor{};
        GetCursorPos(&cursor);
        HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        GetMonitorInfoW(monitor, &mi);
        work = mi.rcWork;
        RECT size{0, 0, px(580), px(920)};
        constexpr DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VSCROLL;
        AdjustWindowRectExForDpi(&size, style, FALSE, WS_EX_CONTROLPARENT, dpi_);
        const int width = size.right - size.left;
        const int height = (std::min)(size.bottom - size.top, work.bottom - work.top);
        window_ = CreateWindowExW(WS_EX_CONTROLPARENT, window_class, get_text(locale_, text::title, text_en::title), style,
            work.right - width - px(20), (std::max)(work.top, work.bottom - height - px(20)),
            width, height, nullptr, nullptr, cls.hInstance, this);
        if (!window_) return false;
        if (const UINT dpi = GetDpiForWindow(window_); dpi && dpi != dpi_) {
            dpi_ = dpi;
            RECT actual{0, 0, px(580), px(920)};
            AdjustWindowRectExForDpi(&actual, style, FALSE, WS_EX_CONTROLPARENT, dpi_);
            const int actual_width = actual.right - actual.left;
            const int actual_height = (std::min)(actual.bottom - actual.top, work.bottom - work.top);
            SetWindowPos(window_, nullptr, work.right - actual_width - px(20),
                (std::max)(work.top, work.bottom - actual_height - px(20)),
                actual_width, actual_height, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        // Explorer may have lower integrity than the tool host. Allow only its
        // inert tray-recreation broadcast, never commands or our WM_APP channel.
        if (!taskbar_created_ || !ChangeWindowMessageFilterEx(window_, taskbar_created_, MSGFLT_ALLOW, nullptr)) {
            if (log_message) log_message(L"TaskbarCreated message filter initialization failed.");
            DestroyWindow(window_);
            return false;
        }
        if (book_.initialize(clock_sample()) && initialize_locale_)
            book_.set_locale(locale_);
        try { controls(); } catch (...) { DestroyWindow(window_); return false; }
        scroll_to(0);
        tray_.cbSize = sizeof(tray_);
        tray_.hWnd = window_;
        tray_.uID = 1;
        tray_.uCallbackMessage = tray_message;
        tray_.hIcon = icon_;
        wcsncpy_s(tray_.szTip, get_text(locale_, text::title, text_en::title), _TRUNCATE);
        if (!SetTimer(window_, timer_id, 250, nullptr)) { DestroyWindow(window_); return false; }
        add_tray();
        // Initialize theme combo selection after controls created
        if (theme_combo_) SendMessageW(theme_combo_, CB_SETCURSEL, static_cast<WPARAM>(theme_value_), 0);
        refresh();
        host_window.store(window_);
        if (show_on_start) show(true);
        return true;
    }
    static LRESULT CALLBACK procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
        auto* self = reinterpret_cast<Panel*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (message == WM_NCCREATE) {
            self = static_cast<Panel*>(reinterpret_cast<CREATESTRUCTW*>(lparam)->lpCreateParams);
            self->window_ = window;
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        if (!self) return DefWindowProcW(window, message, wparam, lparam);
        if (message == self->taskbar_created_ && self->taskbar_created_) {
            self->tray_added_ = false; self->add_tray(); return 0;
        }
        switch (message) {
            case WM_SIZE:
                self->scroll_to(self->scroll_offset_); return 0;
            case WM_VSCROLL: {
                RECT client{};
                GetClientRect(window, &client);
                int position = self->scroll_offset_;
                switch (LOWORD(wparam)) {
                    case SB_LINEUP: position -= self->px(20); break;
                    case SB_LINEDOWN: position += self->px(20); break;
                    case SB_PAGEUP: position -= client.bottom; break;
                    case SB_PAGEDOWN: position += client.bottom; break;
                    case SB_TOP: position = 0; break;
                    case SB_BOTTOM: position = self->px(920); break;
                    case SB_THUMBTRACK: {
                        SCROLLINFO info{};
                        info.cbSize = sizeof(info);
                        info.fMask = SIF_TRACKPOS;
                        GetScrollInfo(window, SB_VERT, &info);
                        position = info.nTrackPos;
                        break;
                    }
                    default: return 0;
                }
                self->scroll_to(position); return 0;
            }
            case WM_MOUSEWHEEL:
                self->scroll_to(self->scroll_offset_ - GET_WHEEL_DELTA_WPARAM(wparam) * self->px(60) / WHEEL_DELTA);
                return 0;
            case WM_COMMAND:
                if (HIWORD(wparam) == EN_SETFOCUS || HIWORD(wparam) == CBN_SETFOCUS || HIWORD(wparam) == BN_SETFOCUS)
                    self->reveal(reinterpret_cast<HWND>(lparam));
                if (LOWORD(wparam) == duration_id && HIWORD(wparam) == EN_CHANGE) {
                    self->preview();
                }
                else if (LOWORD(wparam) == action_id && HIWORD(wparam) == CBN_SELCHANGE) { self->sync_force(); self->preview(); }
                else if (LOWORD(wparam) == force_id && HIWORD(wparam) == BN_CLICKED) self->confirm_force();
                else if (LOWORD(wparam) == kind_id && HIWORD(wparam) == CBN_SELCHANGE) self->show_kind();
                else if (LOWORD(wparam) == theme_id && HIWORD(wparam) == CBN_SELCHANGE) {
                    self->theme_value_ = static_cast<unsigned>(SendMessageW(self->theme_combo_, CB_GETCURSEL, 0, 0));
                    if (!self->book_.set_theme(self->theme_value_)) self->show_error();
                    self->colors();
                    self->refresh_locale_text();
                    return 0;
                }
                else if (LOWORD(wparam) == locale_id && HIWORD(wparam) == CBN_SELCHANGE) {
                    unsigned locale = static_cast<unsigned>(SendMessageW(self->locale_combo_, CB_GETCURSEL, 0, 0));
                    if (!self->book_.set_locale(locale)) self->show_error();
                    self->locale_ = locale;
                    self->refresh_locale_text();
                    return 0;
                }
                else if (HIWORD(wparam) == BN_CLICKED) self->command(LOWORD(wparam));
                return 0;
            case WM_NOTIFY: {
                const auto* header = reinterpret_cast<NMHDR*>(lparam);
                if (header->code == NM_SETFOCUS) self->reveal(header->hwndFrom);
                // Dark theme: the column header ignores the list text colour, so force light text.
                if (self->dark_ && !self->high_contrast_ && self->list_ && header->code == NM_CUSTOMDRAW &&
                    header->hwndFrom == ListView_GetHeader(self->list_)) {
                    const auto* draw = reinterpret_cast<NMCUSTOMDRAW*>(lparam);
                    if (draw->dwDrawStage == CDDS_PREPAINT) return CDRF_NOTIFYITEMDRAW;
                    if (draw->dwDrawStage == CDDS_ITEMPREPAINT) { SetTextColor(draw->hdc, self->ink_); return CDRF_NEWFONT; }
                    return CDRF_DODEFAULT;
                }
                if ((header->idFrom == date_id || header->idFrom == time_id) && header->code == DTN_DATETIMECHANGE) self->preview();
                if (header->idFrom == list_id && header->code == LVN_ITEMCHANGED && !self->rebuilding_list_) {
                    const int row = ListView_GetNextItem(self->list_, -1, LVNI_SELECTED);
                    if (row >= 0 && static_cast<size_t>(row) < self->displayed_ids_.size()) {
                        const auto selected = self->displayed_ids_[row];
                        if (selected != self->selected_id_) self->editing_id_ = 0;
                        self->selected_id_ = selected;
                        self->refresh();
                    }
                }
                return 0;
            }
            case WM_TIMER: if (wparam == timer_id) self->refresh(); return 0;
            case WM_POWERBROADCAST:
                if (wparam == PBT_APMRESUMEAUTOMATIC || wparam == PBT_APMRESUMESUSPEND) {
                    self->list_signature_.clear();
                    self->book_.reconcile(clock_sample()); self->refresh();
                }
                return TRUE;
            case WM_TIMECHANGE:
                self->list_signature_.clear();
                self->book_.reconcile(clock_sample()); self->refresh(); return 0;
            case tray_message:
                switch (LOWORD(lparam)) {
                    case NIN_SELECT: case NIN_KEYSELECT: case NIN_BALLOONUSERCLICK: self->show(true); break;
                    case WM_CONTEXTMENU: self->menu(); break;
                }
                return 0;
            case show_message: self->show(true); return 0;
            case WM_CONTEXTMENU: self->menu(); return 0;
            case stop_message: EndMenu(); DestroyWindow(window); return 0;
            case WM_CLOSE: ShowWindow(window, SW_HIDE); return 0;
            case WM_DESTROY:
                KillTimer(window, timer_id);
                self->book_.reconcile(clock_sample());
                if (self->tray_added_) Shell_NotifyIconW(NIM_DELETE, &self->tray_);
                self->tray_added_ = false;
                host_window.store(nullptr);
                self->window_ = nullptr;
                PostQuitMessage(0); return 0;
            case WM_DPICHANGED: {
                self->dpi_ = HIWORD(wparam);
                self->fonts_and_layout();
                const auto* rect = reinterpret_cast<RECT*>(lparam);
                SetWindowPos(window, nullptr, rect->left, rect->top, rect->right - rect->left,
                    rect->bottom - rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
                // Ensure window stays on the correct monitor after DPI change
                POINT center{rect->left + (rect->right - rect->left) / 2, rect->top + (rect->bottom - rect->top) / 2};
                HMONITOR monitor = MonitorFromPoint(center, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi{};
                mi.cbSize = sizeof(mi);
                if (GetMonitorInfoW(monitor, &mi)) {
                    // Check if window is outside monitor work area
                    if (rect->left < mi.rcWork.left || rect->top < mi.rcWork.top ||
                        rect->right > mi.rcWork.right || rect->bottom > mi.rcWork.bottom) {
                        int width = rect->right - rect->left;
                        int height = rect->bottom - rect->top;
                        int x = mi.rcWork.right - width - self->px(20);
                        int y = (std::max)(mi.rcWork.top, mi.rcWork.bottom - height - self->px(20));
                        SetWindowPos(window, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
                    }
                }
                self->scroll_to(self->scroll_offset_);
                return 0;
            }
            case WM_SETTINGCHANGE:
                self->colors();
                if (lparam && (lstrcmpiW(reinterpret_cast<LPCWSTR>(lparam), L"TimeZoneInformation") == 0)) {
                    self->list_signature_.clear();
                    self->book_.reconcile(clock_sample()); self->refresh();
                }
                return 0;
            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORLISTBOX:
                if (self->dark_ && !self->high_contrast_) {
                    SetTextColor(reinterpret_cast<HDC>(wparam), self->ink_);
                    SetBkColor(reinterpret_cast<HDC>(wparam), RGB(45, 45, 45));
                    if (!self->field_brush_) self->field_brush_ = CreateSolidBrush(RGB(45, 45, 45));
                    return reinterpret_cast<LRESULT>(self->field_brush_);
                }
                break;
            case WM_CTLCOLORSTATIC:
                SetTextColor(reinterpret_cast<HDC>(wparam), self->ink_);
                SetBkColor(reinterpret_cast<HDC>(wparam), self->bg_);
                return reinterpret_cast<LRESULT>(self->background_);
            case WM_ERASEBKGND: {
                RECT rect{}; GetClientRect(window, &rect);
                FillRect(reinterpret_cast<HDC>(wparam), &rect, self->background_); return 1;
            }
            case WM_DRAWITEM: self->draw_button(*reinterpret_cast<DRAWITEMSTRUCT*>(lparam)); return TRUE;
        }
        return DefWindowProcW(window, message, wparam, lparam);
    }
};

inline HANDLE user_mutex() {
    const auto sid = current_user_sid();
    if (sid.empty()) return nullptr;
    const std::wstring name = std::wstring(L"Global\\CrazyTime.") + (dedicated_process ? L"Mod." : L"Preview.") + sid;
    return CreateMutexW(nullptr, FALSE, name.c_str());
}

inline DWORD WINAPI ui_main(void*) {
    HANDLE instance = user_mutex();
    if (!instance) { SetEvent(ready_event); return 1; }
    const DWORD locked = WaitForSingleObject(instance, 0);
    if (locked != WAIT_OBJECT_0 && locked != WAIT_ABANDONED) {
        if (show_on_start)
            if (HWND prior = FindWindowW(window_class, nullptr)) PostMessageW(prior, show_message, 0, 0);
        CloseHandle(instance); SetEvent(ready_event); return 0;
    }
    const auto prior_dpi = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    INITCOMMONCONTROLSEX common{sizeof(common), ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES | ICC_DATE_CLASSES};
    InitCommonControlsEx(&common);
    {
        Panel panel;
        const bool created = panel.create();
        initialized.store(created);
        SetEvent(ready_event);
        bool running = created;
        while (running) {
            const DWORD wait = MsgWaitForMultipleObjects(1, &stop_event, FALSE, INFINITE, QS_ALLINPUT);
            if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;
            MSG message{};
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (message.message == WM_QUIT) { running = false; break; }
                HWND window = host_window.load();
                if (message.message == WM_KEYDOWN && message.wParam == VK_ESCAPE && window) {
                    ShowWindow(window, SW_HIDE); continue;
                }
                if (message.message == WM_KEYDOWN && message.wParam == VK_RETURN && window) {
                    HWND focus = GetFocus();
                    // ENTER on duration edit triggers schedule
                    if (focus == GetDlgItem(window, duration_id)) {
                        PostMessageW(window, WM_COMMAND, schedule_id, 0); continue;
                    }
                    // ENTER on list view triggers edit
                    if (focus == GetDlgItem(window, list_id)) {
                        PostMessageW(window, WM_COMMAND, edit_id, 0); continue;
                    }
                    // ENTER on combos confirms selection (standard behavior)
                }
                if (!window || !IsDialogMessageW(window, &message)) {
                    TranslateMessage(&message); DispatchMessageW(&message);
                }
            }
        }
    }
    UnregisterClassW(window_class, GetModuleHandleW(nullptr));
    if (prior_dpi) SetThreadDpiAwarenessContext(prior_dpi);
    ReleaseMutex(instance); CloseHandle(instance);
    // The engine can retain other threads in a dedicated tool process after
    // our UI thread finishes. Explicit user exit ends that process after cleanup.
    if (dedicated_process && user_requested_exit.load()) ExitProcess(0);
    return 0;
}

inline void stop() {
    if (stop_event) SetEvent(stop_event);
    if (HWND window = host_window.load()) PostMessageW(window, stop_message, 0, 0);
    if (ui_thread) { WaitForSingleObject(ui_thread, INFINITE); CloseHandle(ui_thread); ui_thread = nullptr; }
    if (ready_event) { CloseHandle(ready_event); ready_event = nullptr; }
    if (stop_event) { CloseHandle(stop_event); stop_event = nullptr; }
    initialized.store(false);
}
inline bool start(bool visible, void (*logger)(const wchar_t*) = nullptr, bool dedicated = false, bool simulate = false) {
    simulation_mode = simulate;
    show_on_start = visible;
    dedicated_process = dedicated;
    user_requested_exit.store(false);
    log_message = logger;
    initialized.store(false);
    stop_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    ready_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (stop_event && ready_event) ui_thread = CreateThread(nullptr, 0, ui_main, nullptr, 0, nullptr);
    if (!ui_thread || WaitForSingleObject(ready_event, 10000) != WAIT_OBJECT_0 || !initialized.load()) {
        stop(); return false;
    }
    return true;
}
}

// Source: src/windows/mod_entry.cpp



BOOL WhTool_ModInit() {
    power_timer::windows::mod_storage_path = [](wchar_t* buffer, size_t count) {
        return Wh_GetModStoragePath(buffer, count);
    };
    power_timer::windows::notification_sound_setting = [] {
        return Wh_GetIntSetting(L"soundEnabled") != 0;
    };
    return power_timer::windows::start(false,
        [](const wchar_t* message) { Wh_Log(L"%s", message); }, true);
}

void WhTool_ModSettingsChanged() {
    // Notifications read the current Windhawk setting when they are emitted.
}

void WhTool_ModUninit() {
    power_timer::windows::stop();
}


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
