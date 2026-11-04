// ==WindhawkMod==
// @id              gradual-night-light
// @name            Gradual Night Light
// @description     Gradually adjust Windows Night light strength using a daily schedule.
// @version         0.3.0
// @author          Fenryr
// @github          https://github.com/soldier93HUN
// @include         windhawk.exe
// @compilerOptions -ladvapi32 -lshell32 -lwtsapi32
// @license         MIT
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Gradual Night Light

Automatically adjust Windows' native Night light throughout the evening, with
an editable daily schedule and optional gradual transitions.

## Default schedule

| Setting | Time | Strength |
| --- | --- | --- |
| Stage 1 | 19:00 | 50 |
| Stage 2 | 20:00 | 60 |
| Stage 3 | 21:00 | 70 |
| Off time | 07:00 | Off |

## Settings

- **Off time**: the local time when Night light turns off each day.
- **Stages**: add or remove stages with Windhawk's list controls. Each stage has
  a time and a strength from 1 (weakest) to 100 (warmest). Three stages are
  provided by default; between 1 and 96 stages are supported.
- **Gradual transitions**: interpolate strength between stages. For example,
  the default schedule produces strength 55 at 19:30. Disable this option to
  change strength only at each stage's start time.

Times use the local 24-hour `HH:MM` format. Stages are sorted automatically and
the schedule repeats daily, including across midnight. All stage times must be
unique and different from Off time. Invalid settings are reported in the mod
log; an invalid edit keeps the last valid configuration.

Night light switches off at Off time and stays off until the next stage. The
last stage holds its strength until Off time. To fade in from almost no warmth,
add a stage such as 18:30 with strength 1 before 19:00 with strength 50. Add
further stages to create a five-stage schedule or a gradual morning fade.

## Behavior and restoration

The scheduler runs in a dedicated Windhawk tool process, outside Explorer.
The schedule is checked every ten seconds and catches up after sleep, sign-in,
or an Explorer restart. While enabled, the mod replaces the native on/off
schedule. Manual changes are reapplied from the mod's schedule at the next
check; active drags of the Windows strength slider are allowed to finish.
Disable other Night light schedulers while using this mod.

Before making changes, the mod saves the original settings in storage isolated
by the current user's Windows SID. Disabling the mod restores the original
strength and native schedule enablement. When the original native schedule was
enabled, Windows determines the current on/off state; otherwise the original
manual on/off state is restored. Schedule times and unrelated fields are kept.
If restoration fails, the backup is kept and the failure is logged.

## Persistent Windows settings

This mod writes the `Data` values in these two current-user registry keys:

```
HKCU\Software\Microsoft\Windows\CurrentVersion\CloudStore\Store\DefaultAccount\Current\default$windows.data.bluelightreduction.settings\windows.data.bluelightreduction.settings
HKCU\Software\Microsoft\Windows\CurrentVersion\CloudStore\Store\DefaultAccount\Current\default$windows.data.bluelightreduction.bluelightreductionstate\windows.data.bluelightreduction.bluelightreductionstate
```

It temporarily disables the native schedule, changes color temperature and
on/off state, and briefly enables then clears the preview flag to apply changes.
Record timestamps and transition metadata advance and are not rolled back.
The separate `Cloud` records are read for their timestamps and are never written.

**A clean disable restores the settings described above. Sign-out, shutdown,
forced termination, uninstall without clean unload, or failed restoration can
leave the native schedule disabled and the last strength and on/off state on
disk.** A termination during a preview pulse can also leave preview enabled.
The next start recovers the SID-specific backup and any owned preview. To
restore, enable and then disable the mod while Windows is running, or set Night
light manually in Windows Settings. Do not delete the saved backup first.

When upgrading from 0.2.1, disable that version in the account that originally
enabled it before updating. Its old machine-wide backup has no reliable owner
information, so this version never imports or deletes those legacy values.

## Requirements and limitations

The Night light controller was tested on Windows 11 build 26300, x64. Other
Windows versions and ARM64 have not been verified. The display driver must
support Night light. The tool launcher supports Windhawk 1.7.3 and newer.

This mod uses undocumented CloudStore records with the modern `Current` layout.
Unsupported or malformed records are refused. If records are missing, open
**Settings > System > Display > Night light** once. Windows updates may require
changes to this mod. Visible warmth, HDR, and multiple displays should be
verified on the intended hardware.

## Technical references

The parser and scheduler are independently implemented using the documented
research below. No external helper, script, or runtime download is required.

- [Night light registry format research](https://github.com/kvnxiao/win-nightlight-cli/blob/main/docs/nightlight-registry-format.md)
- [Microsoft Bond CompactBinary protocol](https://github.com/microsoft/bond/blob/master/cpp/inc/bond/protocol/compact_binary.h)
- [Official Windhawk tool mod launcher](https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- OffTime: "07:00"
  $name: Off time (HH:MM)
  $description: Turn Night light off at this local time each day, until the next stage.
- Stages:
  - - Time: "19:00"
      $name: Time (HH:MM)
    - Strength: 50
      $name: Strength (1-100)
  - - Time: "20:00"
    - Strength: 60
  - - Time: "21:00"
    - Strength: 70
  $name: Stages
  $description: Add or remove stages using the list controls. Use 1-96 unique times, different from Off time. Stages are sorted automatically.
- GradualTransitions: true
  $name: Gradually change strength between times
  $description: Interpolate between stages. Night light switches off at Off time and stays off until the next stage.
*/
// ==/WindhawkModSettings==
// clang-format on

#include <sddl.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace nightlight {
using Bytes = std::vector<uint8_t>;
constexpr size_t kMaxRecord = 65536;
constexpr uint8_t kBool = 2, kU64 = 6, kStruct = 10, kList = 11, kI8 = 14,
                  kI16 = 15, kI32 = 16;

void Require(bool condition, const char* message) {
    if (!condition)
        throw std::runtime_error(message);
}

std::wstring CurrentUserSid() {
    HANDLE token = nullptr;
    Require(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token),
            "Cannot open process token");
    alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
    DWORD size = 0;
    PWSTR sid = nullptr;
    bool ok =
        GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size) &&
        ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(buffer)->User.Sid,
                               &sid);
    CloseHandle(token);
    Require(ok, "Cannot get current user's SID");
    std::wstring result;
    try {
        result = sid;
    } catch (...) {
        LocalFree(sid);
        throw;
    }
    LocalFree(sid);
    return result;
}

// Windhawk mod storage is machine-wide. Every persistent value must carry the
// owner SID; even preview and timestamp journals must never cross users.
struct StorageNames {
    std::wstring baselineSaved, originalSettings, originalState;
    std::wstring highestSettings, highestState, previewOwned;
    StorageNames() = default;
    explicit StorageNames(const std::wstring& sid) {
        Require(sid.starts_with(L"S-1-"), "Invalid storage owner SID");
        auto name = [&](PCWSTR base) {
            return std::wstring(base) + L"_" + sid;
        };
        baselineSaved = name(L"BaselineSaved");
        originalSettings = name(L"OriginalSettings");
        originalState = name(L"OriginalState");
        highestSettings = name(L"HighestSettings");
        highestState = name(L"HighestState");
        previewOwned = name(L"PreviewOwned");
    }
};

// Preserve the bytes of every field we do not own, including nested fields.
// Fail closed on unsupported types, inheritance, duplicate IDs, and bad
// lengths.
struct Field {
    uint16_t id;
    uint8_t type;
    Bytes value;
    bool operator==(const Field&) const = default;
};
using Fields = std::vector<Field>;

struct Reader {
    const Bytes& bytes;
    size_t pos = 0;
    uint8_t Byte() {
        Require(pos < bytes.size(), "Truncated Bond data");
        return bytes[pos++];
    }
    void Skip(size_t count) {
        Require(count <= bytes.size() - pos, "Invalid Bond length");
        pos += count;
    }
    uint64_t Varint() {
        uint64_t value = 0;
        for (unsigned shift = 0; shift < 70; shift += 7) {
            const auto b = Byte();
            Require(shift != 63 || b <= 1, "Bond varint overflow");
            value |= uint64_t(b & 127) << shift;
            if (!(b & 128))
                return value;
        }
        throw std::runtime_error("Invalid Bond varint");
    }
    void Value(uint8_t type, unsigned depth) {
        Require(depth < 32, "Bond nesting too deep");
        switch (type) {
            case 2:
                Require(Byte() <= 1, "Invalid Bond boolean");
                break;
            case 3:
            case 14:
                Skip(1);
                break;
            case 4:
            case 5:
            case 6:
            case 15:
            case 16:
            case 17:
                Varint();
                break;
            case 7:
                Skip(4);
                break;
            case 8:
                Skip(8);
                break;
            case 9:
            case 18: {
                auto n = Varint();
                Require(n <= kMaxRecord, "Bond string too large");
                Skip(static_cast<size_t>(n) * (type == 18 ? 2 : 1));
                break;
            }
            case 10:
                Struct(depth + 1);
                break;
            case 11:
            case 12:
            case 13: {
                const auto first = Byte();
                const auto second = type == 13 ? Byte() : 0;
                Require(first >= 2 && first <= 18 &&
                            (type != 13 || (second >= 2 && second <= 18)),
                        "Invalid Bond container type");
                const auto count = Varint();
                Require(count <= kMaxRecord, "Bond container too large");
                for (uint64_t i = 0; i < count; ++i) {
                    Value(first, depth + 1);
                    if (type == 13)
                        Value(second, depth + 1);
                }
                break;
            }
            default:
                throw std::runtime_error("Unsupported Bond type");
        }
    }
    Fields Struct(unsigned depth = 0) {
        Fields result;
        while (true) {
            const auto header = Byte();
            if (header == 0)
                return result;
            const auto type = static_cast<uint8_t>(header & 31);
            Require(type >= 2 && type <= 18, "Unsupported Bond field");
            uint16_t id = header >> 5;
            if (id == 6)
                id = Byte();
            else if (id == 7) {
                id = Byte();
                id |= uint16_t(Byte()) << 8;
            }
            Require(result.empty() || result.back().id < id,
                    "Unordered or duplicate Bond field");
            const auto start = pos;
            Value(type, depth);
            result.push_back(
                {id, type, Bytes(bytes.begin() + start, bytes.begin() + pos)});
        }
    }
};

void Varint(Bytes& bytes, uint64_t value) {
    do {
        auto b = static_cast<uint8_t>(value & 127);
        value >>= 7;
        bytes.push_back(b | (value ? 128 : 0));
    } while (value);
}
Bytes Number(uint64_t value) {
    Bytes b;
    Varint(b, value);
    return b;
}
uint64_t Number(const Bytes& value) {
    Reader r{value};
    auto n = r.Varint();
    Require(r.pos == value.size(), "Invalid numeric field");
    return n;
}
const Field* Find(const Fields& fields, uint16_t id) {
    auto it = std::find_if(fields.begin(), fields.end(),
                           [id](const Field& f) { return f.id == id; });
    return it == fields.end() ? nullptr : &*it;
}
const Field& Get(const Fields& fields, uint16_t id, uint8_t type) {
    const auto* f = Find(fields, id);
    Require(f && f->type == type, "Unexpected Night light schema");
    return *f;
}
void Set(Fields& fields, uint16_t id, uint8_t type, Bytes value) {
    auto it =
        std::lower_bound(fields.begin(), fields.end(), id,
                         [](const Field& f, uint16_t n) { return f.id < n; });
    if (it != fields.end() && it->id == id) {
        Require(it->type == type, "Unexpected field type");
        it->value = std::move(value);
    } else
        fields.insert(it, {id, type, std::move(value)});
}
void Erase(Fields& fields, uint16_t id) {
    std::erase_if(fields, [id](const Field& f) { return f.id == id; });
}
Fields Parse(const Bytes& bytes, bool marshaled = false) {
    Require(bytes.size() <= kMaxRecord, "Night light record too large");
    Reader r{bytes};
    if (marshaled) {
        Require(r.Byte() == 0x43 && r.Byte() == 0x42 && r.Byte() == 1 &&
                    r.Byte() == 0,
                "Unsupported CloudStore format (expected Bond CB v1)");
    }
    auto fields = r.Struct();
    Require(r.pos == bytes.size(), "Trailing Bond data");
    return fields;
}
Bytes Encode(const Fields& fields, bool marshaled = false) {
    Bytes result;
    if (marshaled)
        result = {0x43, 0x42, 1, 0};
    for (const auto& f : fields) {
        if (f.id < 6)
            result.push_back((f.id << 5) | f.type);
        else if (f.id <= 255) {
            result.push_back(0xC0 | f.type);
            result.push_back(f.id);
        } else {
            result.push_back(0xE0 | f.type);
            result.push_back(f.id & 255);
            result.push_back(f.id >> 8);
        }
        result.insert(result.end(), f.value.begin(), f.value.end());
    }
    result.push_back(0);
    return result;
}
bool Bool(const Fields& f, uint16_t id, bool fallback = false) {
    if (!Find(f, id))
        return fallback;
    return Get(f, id, kBool).value.at(0) != 0;
}

struct Record {
    Fields outer, container, wrapper, payload;
    uint64_t timestamp;
    explicit Record(const Bytes& bytes) {
        outer = Parse(bytes, true);
        auto metadata = Parse(Get(outer, 0, kStruct).value);
        Require(Bool(metadata, 0), "Invalid CloudStore metadata");
        container = Parse(Get(outer, 1, kStruct).value);
        timestamp = Number(Get(container, 0, kU64).value);
        wrapper = Parse(Get(container, 1, kStruct).value);
        const auto& list = Get(wrapper, 1, kList).value;
        Reader r{list};
        Require(r.Byte() == kI8, "Unexpected Night light payload type");
        auto n = r.Varint();
        Require(n == list.size() - r.pos, "Invalid Night light payload length");
        payload = Parse(Bytes(list.begin() + r.pos, list.end()), true);
    }
    Bytes EncodeWithTimestamp(uint64_t stamp) const {
        auto out = outer, c = container, w = wrapper;
        auto inner = Encode(payload, true);
        Bytes list{kI8};
        Varint(list, inner.size());
        list.insert(list.end(), inner.begin(), inner.end());
        Set(w, 1, kList, std::move(list));
        Set(c, 0, kU64, Number(stamp));
        Set(c, 1, kStruct, Encode(w));
        Set(out, 1, kStruct, Encode(c));
        return Encode(out, true);
    }
};

void ValidateSettings(const Fields& fields) {
    Bool(fields, 0);
    Bool(fields, 10, true);
    Bool(fields, 70);
    if (Find(fields, 40)) {
        auto raw = Number(Get(fields, 40, kI16).value);
        Require(!(raw & 1) && raw / 2 >= 1200 && raw / 2 <= 6500,
                "Invalid Night light temperature");
    }
}
bool IsOn(const Fields& fields) {
    Require(Bool(fields, 30, true),
            "Night light is unavailable for this display driver");
    if (!Find(fields, 0))
        return false;
    Require(Number(Get(fields, 0, kI32).value) == 0,
            "Unknown Night light state");
    return true;
}

struct Entry {
    int minute;
    int strength;
};
int ParseTime(const std::wstring& value) {
    Require(value.size() == 5 && value[2] == L':' && value[0] >= L'0' &&
                value[0] <= L'9' && value[1] >= L'0' && value[1] <= L'9' &&
                value[3] >= L'0' && value[3] <= L'9' && value[4] >= L'0' &&
                value[4] <= L'9',
            "Times must use HH:MM");
    int h = (value[0] - L'0') * 10 + value[1] - L'0';
    int m = (value[3] - L'0') * 10 + value[4] - L'0';
    Require(h < 24 && m < 60, "Time is out of range");
    return h * 60 + m;
}
void ValidateSchedule(std::vector<Entry>& entries) {
    Require(!entries.empty() && entries.size() <= 97,
            "Use at most 96 stages and one off time");
    std::sort(entries.begin(), entries.end(),
              [](Entry a, Entry b) { return a.minute < b.minute; });
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
        Require(e.minute >= 0 && e.minute < 1440 && e.strength >= 0 &&
                    e.strength <= 100,
                "Schedule time or strength is out of range");
        Require(i == 0 || entries[i - 1].minute != e.minute,
                "Schedule times must be unique");
    }
}
std::vector<Entry> BuildSchedule(std::vector<Entry> stages,
                                 const std::wstring& offTime) {
    Require(!stages.empty() && stages.size() <= 96,
            "Use between 1 and 96 stages");
    for (const auto& stage : stages)
        Require(stage.strength >= 1 && stage.strength <= 100,
                "Stage strength must be between 1 and 100");
    stages.push_back({ParseTime(offTime), 0});
    ValidateSchedule(stages);
    return stages;
}
double StrengthAt(const std::vector<Entry>& entries,
                  double minute,
                  bool gradual) {
    Require(!entries.empty() && minute >= 0 && minute < 1440,
            "Invalid schedule evaluation");
    auto next =
        std::upper_bound(entries.begin(), entries.end(), minute,
                         [](double m, const Entry& e) { return m < e.minute; });
    auto prev = next == entries.begin() ? entries.end() - 1 : next - 1;
    if (next == entries.end())
        next = entries.begin();
    if (!gradual || entries.size() == 1 || prev->strength == 0 ||
        next->strength == 0)
        return prev->strength;
    int duration = (next->minute - prev->minute + 1440) % 1440;
    double elapsed = minute - prev->minute;
    if (elapsed < 0)
        elapsed += 1440;
    return prev->strength +
           (next->strength - prev->strength) * elapsed / duration;
}
int Kelvin(double strength) {
    Require(std::isfinite(strength) && strength >= 0 && strength <= 100,
            "Invalid strength");
    return static_cast<int>(std::lround(6500 - 53 * strength));
}

std::wstring Key(bool settings, bool cloud = false) {
    std::wstring name = settings ? L"settings" : L"bluelightreductionstate";
    return std::wstring(
               L"Software\\Microsoft\\Windows\\CurrentVersion\\CloudStore\\Stor"
               L"e\\DefaultAccount\\") +
           (cloud ? L"Cloud\\" : L"Current\\") +
           L"default$windows.data.bluelightreduction." + name +
           L"\\windows.data.bluelightreduction." + name;
}
Bytes ReadRegistry(bool settings, bool cloud = false) {
    DWORD size = 0;
    auto key = Key(settings, cloud);
    auto error = RegGetValueW(HKEY_CURRENT_USER, key.c_str(), L"Data",
                              RRF_RT_REG_BINARY, nullptr, nullptr, &size);
    Require(error == ERROR_SUCCESS && size && size <= kMaxRecord,
            "Cannot read Night light registry record; open Night light in "
            "Windows Settings first");
    Bytes bytes(size);
    error = RegGetValueW(HKEY_CURRENT_USER, key.c_str(), L"Data",
                         RRF_RT_REG_BINARY, nullptr, bytes.data(), &size);
    Require(error == ERROR_SUCCESS, "Night light registry read failed");
    bytes.resize(size);
    return bytes;
}
void WriteRegistry(bool settings, const Bytes& bytes) {
    HKEY key = nullptr;
    auto error = RegOpenKeyExW(HKEY_CURRENT_USER, Key(settings).c_str(), 0,
                               KEY_SET_VALUE, &key);
    Require(error == ERROR_SUCCESS,
            "Cannot open Night light registry record for writing");
    error = RegSetValueExW(key, L"Data", 0, REG_BINARY, bytes.data(),
                           static_cast<DWORD>(bytes.size()));
    RegCloseKey(key);
    Require(error == ERROR_SUCCESS, "Night light registry write failed");
}
uint64_t FileTimeNow() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return (uint64_t(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

// This interface also lets tests exercise write failures without touching
// Windows.
struct Store {
    virtual Bytes Read(bool settings) = 0;
    virtual void Write(bool settings, const Bytes& bytes) = 0;
    virtual uint64_t CloudStamp(bool settings) = 0;
    virtual ~Store() = default;
};
struct RegistryStore : Store {
    Bytes Read(bool settings) override { return ReadRegistry(settings); }
    void Write(bool settings, const Bytes& bytes) override {
        WriteRegistry(settings, bytes);
    }
    uint64_t CloudStamp(bool settings) override {
        try {
            return Number(
                Get(Parse(ReadRegistry(settings, true), true), 1, kU64).value);
        } catch (...) {
            return 0;
        }  // Cloud record can be absent. Never write it.
    }
};

struct Controller {
    Store& store;
    uint64_t highest[2]{};
    bool ownsPreview = false;
    std::function<void(bool, uint64_t)> saveTimestamp;
    std::function<void(bool)> savePreview;
    explicit Controller(Store& s) : store(s) {}
    void Write(bool settings, Record& record) {
        auto base = std::max(
            {record.timestamp, highest[settings], store.CloudStamp(settings)});
        Require(base <= std::numeric_limits<uint64_t>::max() - 2,
                "Night light timestamp overflow");
        auto stamp =
            std::max(FileTimeNow() / 10000000 - 11644473600ULL, base + 2);
        if (saveTimestamp)
            saveTimestamp(settings, stamp);
        store.Write(settings, record.EncodeWithTimestamp(stamp));
        highest[settings] = stamp;
    }
    void Update(bool settings, const std::function<void(Fields&)>& edit) {
        Record r(store.Read(settings));
        if (settings)
            ValidateSettings(r.payload);
        else
            IsOn(r.payload);
        auto before = r.payload;
        edit(r.payload);
        if (r.payload != before)
            Write(settings, r);
    }
    void SetOn(bool on) {
        Update(false, [on](Fields& f) {
            if (IsOn(f) == on)
                return;
            if (on)
                Set(f, 0, kI32, Number(0));
            else
                Erase(f, 0);
            Set(f, 10, kI32, Number(2));  // ZigZag(1): manual transition.
            Set(f, 20, kU64, Number(FileTimeNow()));
        });
    }
    void EndPreview() {
        if (!ownsPreview)
            return;
        for (int attempt = 0;; ++attempt) {
            try {
                Update(true, [](Fields& f) { Set(f, 70, kBool, {0}); });
                if (savePreview)
                    savePreview(false);
                ownsPreview = false;
                return;
            } catch (...) {
                if (attempt == 2)
                    throw;
            }
        }
    }
    // Returns false while the user is dragging Windows' own strength slider.
    bool Apply(double strength, bool refresh = false) {
        EndPreview();
        Record settings(store.Read(true)), state(store.Read(false));
        ValidateSettings(settings.payload);
        bool on = IsOn(state.payload);
        if (Bool(settings.payload, 70))
            return false;
        const bool wantedOn = strength > 0;
        const int temperature = Kelvin(strength);
        const auto* current = Find(settings.payload, 40);
        const bool changed =
            wantedOn &&
            (!current || Number(current->value) != uint64_t(temperature * 2));
        Update(true, [=](Fields& f) {
            Set(f, 0, kBool, {0});  // The mod owns scheduling while active.
            if (wantedOn)
                Set(f, 40, kI16, Number(temperature * 2));
        });
        SetOn(wantedOn);
        if (wantedOn && (changed || !on || refresh)) {
            // Briefly emulate the native slider so Explorer applies the new
            // temperature.
            if (savePreview)
                savePreview(true);
            ownsPreview = true;
            Update(true, [](Fields& f) { Set(f, 70, kBool, {1}); });
            Sleep(50);  // Runs on our worker, never Explorer's UI thread.
            EndPreview();
        }
        return true;
    }
    void Restore(const Bytes& settingsBytes, const Bytes& stateBytes) {
        EndPreview();
        Record baseline(settingsBytes), state(stateBytes);
        ValidateSettings(baseline.payload);
        bool on = IsOn(state.payload);
        // Restore owned fields only. Keep any unrelated edits made while
        // running.
        Update(true, [&](Fields& f) {
            for (uint16_t id : {0, 40}) {
                const auto* original = Find(baseline.payload, id);
                if (original)
                    Set(f, id, original->type, original->value);
                else
                    Erase(f, id);
            }
            Set(f, 70, kBool, {0});
        });
        if (Bool(baseline.payload, 0)) {
            // Do not reinstate a stale manual override (for example, an on
            // state captured at 22:00 when disabling the mod at noon).
            // Restoring schedule enablement above notifies Windows to
            // reevaluate it.
            Update(false, [](Fields& f) {
                Erase(f, 10);  // Default transition cause: scheduled.
            });
            on = IsOn(Record(store.Read(false)).payload);
        } else {
            SetOn(on);
        }
        if (on) {
            if (savePreview)
                savePreview(true);
            ownsPreview = true;
            Update(true, [](Fields& f) { Set(f, 70, kBool, {1}); });
            Sleep(50);
            EndPreview();
        }
    }
};
}  // namespace nightlight

#ifndef NIGHT_LIGHT_TEST
#include <shellapi.h>
#include <windhawk_utils.h>
#include <wtsapi32.h>
#include <mutex>

namespace {
using namespace nightlight;
struct Config {
    std::vector<Entry> schedule;
    bool gradual;
};
std::mutex configMutex;
Config config;
HANDLE stopEvent = nullptr, changedEvent = nullptr, worker = nullptr;
StorageNames storageNames;  // Initialized with the token SID before any I/O.
UINT taskbarCreatedMessage = 0;
bool refreshRequested =
    true;  // Only accessed by the worker and its window proc.

LRESULT CALLBACK EventWindowProc(HWND hwnd,
                                 UINT message,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    if (message == WM_QUERYENDSESSION)
        return TRUE;
    if (message == WM_ENDSESSION && wParam) {
        SetEvent(stopEvent);  // Best-effort cleanup; forced termination can
                              // still interrupt it.
        return 0;
    }
    if (message == WM_DISPLAYCHANGE || message == WM_TIMECHANGE ||
        (taskbarCreatedMessage && message == taskbarCreatedMessage) ||
        (message == WM_WTSSESSION_CHANGE && wParam == WTS_SESSION_UNLOCK) ||
        (message == WM_POWERBROADCAST && (wParam == PBT_APMRESUMEAUTOMATIC ||
                                          wParam == PBT_APMRESUMESUSPEND))) {
        refreshRequested = true;
        return message == WM_POWERBROADCAST ? TRUE : 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void WaitForWork(DWORD timeout) {
    HANDLE events[]{stopEvent, changedEvent};
    MsgWaitForMultipleObjectsEx(2, events, timeout, QS_ALLINPUT,
                                MWMO_INPUTAVAILABLE);
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

Config LoadConfig() {
    Config result;
    result.gradual = Wh_GetIntSetting(L"GradualTransitions") != 0;
    std::wstring offTime = WindhawkUtils::StringSetting::make(L"OffTime").get();
    std::vector<Entry> stages;
    for (int i = 0; i <= 96; ++i) {
        std::wstring time =
            WindhawkUtils::StringSetting::make(L"Stages[%d].Time", i).get();
        if (time.empty())
            break;
        stages.push_back(
            {ParseTime(time), Wh_GetIntSetting(L"Stages[%d].Strength", i)});
    }
    result.schedule = BuildSchedule(std::move(stages), offTime);
    return result;
}
Bytes Saved(PCWSTR name) {
    Bytes bytes(kMaxRecord);
    auto size = Wh_GetBinaryValue(name, bytes.data(), bytes.size());
    Require(size > 0 && size <= bytes.size(),
            "Cannot read saved Night light baseline");
    bytes.resize(size);
    return bytes;
}
void ClearBaseline() {
    // Clear the commit marker first; stale blobs are harmless without it.
    Require(Wh_SetIntValue(storageNames.baselineSaved.c_str(), 0),
            "Cannot clear Night light backup marker");
    Wh_DeleteValue(storageNames.originalSettings.c_str());
    Wh_DeleteValue(storageNames.originalState.c_str());
}
DWORD WINAPI Run(void*) {
    RegistryStore store;
    Controller controller(store);
    controller.saveTimestamp = [](bool settings, uint64_t stamp) {
        Require(Wh_SetBinaryValue((settings ? storageNames.highestSettings
                                            : storageNames.highestState)
                                      .c_str(),
                                  &stamp, sizeof(stamp)),
                "Cannot save Night light timestamp");
    };
    controller.savePreview = [](bool owned) {
        Require(Wh_SetIntValue(storageNames.previewOwned.c_str(), owned),
                "Cannot save Night light preview marker");
    };
    Bytes originalSettings, originalState;
    bool saved = false;
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<PCWSTR>(&EventWindowProc), &module);
    const wchar_t* className = L"WindhawkGradualNightLightEvents";
    WNDCLASSW wc{};
    wc.lpfnWndProc = EventWindowProc;
    wc.hInstance = module;
    wc.lpszClassName = className;
    bool registered = RegisterClassW(&wc) != 0;
    HWND eventWindow =
        registered ? CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                     className, L"", WS_POPUP, 0, 0, 0, 0,
                                     nullptr, nullptr, module, nullptr)
                   : nullptr;
    bool sessionRegistered =
        eventWindow &&
        WTSRegisterSessionNotification(eventWindow, NOTIFY_FOR_THIS_SESSION);
    if (!eventWindow)
        Wh_Log(L"Event window unavailable; scheduled changes still run");
    taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
    refreshRequested = true;
    std::string lastError;
    while (WaitForSingleObject(stopEvent, 0) == WAIT_TIMEOUT) {
        try {
            if (!saved) {
                if (Wh_GetIntValue(storageNames.baselineSaved.c_str(), 0)) {
                    originalSettings =
                        Saved(storageNames.originalSettings.c_str());
                    originalState = Saved(storageNames.originalState.c_str());
                } else {
                    originalSettings = store.Read(true);
                    originalState = store.Read(false);
                }
                Record settings(originalSettings), state(originalState);
                ValidateSettings(settings.payload);
                IsOn(state.payload);
                if (Bool(settings.payload, 70)) {
                    // Do not capture a transient slider preview as the
                    // baseline.
                    WaitForWork(1000);
                    continue;
                }
                Require(
                    Wh_SetBinaryValue(storageNames.originalSettings.c_str(),
                                      originalSettings.data(),
                                      originalSettings.size()) &&
                        Wh_SetBinaryValue(storageNames.originalState.c_str(),
                                          originalState.data(),
                                          originalState.size()) &&
                        Wh_SetIntValue(storageNames.baselineSaved.c_str(), 1),
                    "Cannot save Night light baseline; no changes made");
                saved = true;
                // Keep our high-water marks across Explorer crashes and mod
                // reloads.
                for (int i = 0; i < 2; ++i) {
                    uint64_t highest = 0;
                    if (Wh_GetBinaryValue((i ? storageNames.highestSettings
                                             : storageNames.highestState)
                                              .c_str(),
                                          &highest,
                                          sizeof(highest)) == sizeof(highest))
                        controller.highest[i] = highest;
                }
                controller.ownsPreview =
                    Wh_GetIntValue(storageNames.previewOwned.c_str(), 0) != 0;
            }
            Config current;
            {
                std::lock_guard lock(configMutex);
                current = config;
            }
            SYSTEMTIME now;
            GetLocalTime(&now);
            double minute = now.wHour * 60 + now.wMinute + now.wSecond / 60.0;
            if (controller.Apply(
                    StrengthAt(current.schedule, minute, current.gradual),
                    refreshRequested)) {
                refreshRequested = false;
            }
            lastError.clear();
        } catch (const std::exception& e) {
            if (lastError != e.what())
                Wh_Log(L"Night light: %S", e.what());
            lastError = e.what();
        }
        WaitForWork(10000);
    }
    if (saved) {
        try {
            controller.Restore(originalSettings, originalState);
            ClearBaseline();
            Wh_Log(L"Original Night light settings restored");
        } catch (const std::exception& e) {
            Wh_Log(L"Night light restoration failed; backup retained: %S",
                   e.what());
        }
    }
    if (sessionRegistered)
        WTSUnRegisterSessionNotification(eventWindow);
    if (eventWindow)
        DestroyWindow(eventWindow);
    if (registered)
        UnregisterClassW(className, module);
    return 0;
}
}  // namespace

BOOL WhTool_ModInit() {
    try {
        config = LoadConfig();
        storageNames = StorageNames(CurrentUserSid());
    } catch (const std::exception& e) {
        Wh_Log(L"Cannot initialize Night light: %S", e.what());
        return FALSE;
    }
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    changedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (stopEvent && changedEvent)
        worker = CreateThread(nullptr, 0, Run, nullptr, 0, nullptr);
    if (!worker) {
        if (stopEvent)
            CloseHandle(stopEvent);
        if (changedEvent)
            CloseHandle(changedEvent);
        stopEvent = changedEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}
void WhTool_ModSettingsChanged() {
    try {
        auto next = LoadConfig();
        {
            std::lock_guard lock(configMutex);
            config = std::move(next);
        }
        SetEvent(changedEvent);
    } catch (const std::exception& e) {
        Wh_Log(L"Invalid schedule; keeping the last valid schedule: %S",
               e.what());
    }
}
void WhTool_ModUninit() {
    if (worker) {
        SetEvent(stopEvent);
        WaitForSingleObject(worker, INFINITE);
    }
    if (worker)
        CloseHandle(worker);
    if (stopEvent)
        CloseHandle(stopEvent);
    if (changedEvent)
        CloseHandle(changedEvent);
    worker = stopEvent = changedEvent = nullptr;
}

// clang-format off
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
// clang-format on
#endif
