// ==WindhawkMod==
// @id              better-eject-dialog
// @name            Better Eject Dialog
// @description     Shows which programs are blocking a USB drive from ejecting, and lets you close them, eject automatically, or force-eject
// @version         1.0
// @author          Ender
// @github          https://github.com/EnderDragonEP
// @twitter         https://twitter.com/NoobieNoodle89
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcfgmgr32 -lwevtapi -lversion -lole32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// Source code is published under the GNU General Public License v3.0 only.
//
// How it works:
//   * When a drive is ejected from the "Safely Remove Hardware and Eject Media"
//     tray icon or with "Eject" in File Explorer, hotplug.dll calls
//     CM_Request_Device_Eject_ExW inside explorer.exe. If the device is in use,
//     the call fails with CR_REMOVE_VETOED and hotplug.dll shows a
//     "Problem Ejecting <device>" message box.
//   * This mod hooks CM_Request_Device_Eject_ExW. When a call from hotplug.dll
//     is vetoed because the device is in use, the mod shows its own dialog on
//     the same thread. If the device gets ejected from that dialog, the hook
//     returns CR_SUCCESS, so Windows continues with its usual "Safe To Remove
//     Hardware" notification.
//   * The blocking programs come from the System event log: for every failed
//     eject, the kernel writes a Kernel-PnP event 225 for each process that has
//     the device open.
//   * If the dialog is dismissed, hotplug.dll's own message box is skipped by a
//     MessageBoxW hook that only applies to hotplug.dll on that thread.

// ==WindhawkModReadme==
/*
# Better Eject Dialog

When a USB drive is in use, Windows refuses to eject it with a message like
*"Problem Ejecting USB Attached SCSI (UAS) Mass Storage Device. This device is
currently in use. Close any programs or windows that might be using the device,
and then try again."* It doesn't say which program is using the drive.

| Before | After |
| --- | --- |
| ![Before](https://raw.githubusercontent.com/EnderDragonEP/asset/19e0943aaf3ddeb2f8e8457875f4ed6cd588fe90/windhawk-mods/better-eject-dialog/preview-before.png) | ![After](https://raw.githubusercontent.com/EnderDragonEP/asset/e826e35bed4c5f0b2a36bec47168b76a7d263676/windhawk-mods/better-eject-dialog/preview-after.png) |

This mod replaces that message with a dialog that:

* **Lists the programs that are using the drive**, with their name, process ID,
  path and command line. Background helpers get a clearer name, for example
  *COM Surrogate (Plan9FileSystem)* for the server that WSL uses to access
  Windows drives.
* **Explains common causes**, such as WSL, Windows Search, Microsoft Defender
  and File Explorer.
* **Closes or ends a program** with one click. Programs with windows are asked
  to close, so they can still ask you to save your work. Background processes
  are ended. Programs that run as administrator get a UAC prompt.
* **Ejects automatically** once the blocking programs exit, so you can leave the
  dialog open and close them yourself. If the drive keeps being used, it stops
  after 5 attempts in a row until you click a button.
* **Force ejects** as a last resort. After a confirmation and a UAC prompt, the
  drive is flushed and dismounted so that programs lose access to it, and then
  it's ejected. Programs may lose unsaved changes to files on the drive.
* Keeps Windows' usual **Safe To Remove Hardware** notification when the drive
  is ejected.

It works when you eject from the *Safely Remove Hardware and Eject Media* tray
icon and from *Eject* in File Explorer.

## How it finds the programs

Every time an eject fails, Windows writes an event to the System event log
(Kernel-PnP, event ID 225) for each process that has the drive open. The mod
reads those events, so it can also name programs that run as administrator,
services and Windows itself.

## What it doesn't close

* **System** means that Windows or a driver has the drive open, for example for
  indexing, virus scanning, WSL or a mounted disk image. It usually lets go once
  the other programs are closed. If it doesn't, use **Force eject**.
* **File Explorer** can't be closed from the dialog, because the dialog runs
  inside it. Close File Explorer windows or previews that show the drive.
* **Windows services and core Windows processes** are listed so that you know
  what's using the drive, but they aren't closed from the dialog.

## Notes

* Ejects started by other programs, such as third-party eject tools, aren't
  changed.
* Force eject can lose unsaved changes that programs still have for files on
  the drive. Use it only if you can't close the programs.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- autoEject: true
  $name: Eject automatically when the programs close
  $description: >-
    The initial state of the dialog's checkbox. When it's checked, the dialog
    stays open and ejects the drive as soon as all blocking programs have
    exited.
- programButtons: true
  $name: Show Close and End buttons
  $description: >-
    Show a button for each blocking program. Programs with windows are asked to
    close, and background processes are ended.
- elevatedEndTask: true
  $name: Allow closing programs that run as administrator
  $description: >-
    Show buttons with a UAC shield for blocking programs that run as
    administrator in your session.
- forceEject: true
  $name: Show Force eject
  $description: >-
    Offer a last-resort button that flushes and dismounts the drive after a UAC
    prompt, and then ejects it. Programs may lose unsaved changes to files on
    the drive.
- reportWaitMs: 1500
  $name: Wait for the list of programs (milliseconds)
  $description: >-
    How long to wait for Windows to report the blocking programs before the
    dialog opens. The list keeps updating while the dialog is open.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

#include <cfgmgr32.h>
#include <commctrl.h>
#include <shellapi.h>
#include <winevt.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#ifndef EVT_VARIANT_TYPE_MASK
#define EVT_VARIANT_TYPE_MASK 0x7f
#endif

////////////////////////////////////////////////////////////////////////////////
// Constants and globals

constexpr ULONGLONG kFileTimeTicksPerMs = 10'000;
constexpr ULONGLONG kFileTimeTicksPerSecond = 10'000'000;

// GUID_DEVINTERFACE_VOLUME.
constexpr GUID kVolumeInterfaceGuid = {
    0x53f5630d,
    0xb6bf,
    0x11d0,
    {0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b}};

// DEVPKEY_Device_FriendlyName and DEVPKEY_Device_DeviceDesc.
constexpr DEVPROPKEY kFriendlyNameKey = {
    {0xa45c254e,
     0xdf1c,
     0x4efd,
     {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14};
constexpr DEVPROPKEY kDeviceDescKey = {
    {0xa45c254e,
     0xdf1c,
     0x4efd,
     {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    2};

constexpr int kBlockerButtonBase = 1000;
constexpr int kMaxBlockerButtons = 6;
constexpr int kOtherBlockersButton = 1900;
constexpr int kForceEjectButton = 2000;
constexpr int kConfirmForceButton = 2001;
constexpr int kBackButton = 2002;

// How long a program gets to close politely before "Close" becomes "End".
constexpr ULONGLONG kCloseGraceMs = 6000;
// How long after an eject attempt late Kernel-PnP events are picked up.
constexpr ULONGLONG kEventPollWindowMs = 10000;
// Minimum time between automatic eject attempts.
constexpr ULONGLONG kAutoEjectIntervalMs = 2000;
// Automatic attempts in a row without user input, for example when a program
// keeps opening files on the drive.
constexpr int kMaxAutomaticAttempts = 5;
// How long a request to skip hotplug.dll's message box stays valid.
constexpr ULONGLONG kSuppressionLifetimeMs = 15000;

struct Settings {
    bool autoEject = true;
    bool programButtons = true;
    bool elevatedEndTask = true;
    bool forceEject = true;
    int reportWaitMs = 1500;
};

std::mutex g_settingsMutex;
Settings g_settings;

std::atomic<bool> g_unloading{false};

// Number of hooked eject calls from hotplug.dll that are still running. The
// mod waits for them before its hooks are removed.
std::atomic<int> g_inFlight{0};

struct InFlightScope {
    InFlightScope() { g_inFlight.fetch_add(1); }
    ~InFlightScope() { g_inFlight.fetch_sub(1); }
    InFlightScope(const InFlightScope&) = delete;
    InFlightScope& operator=(const InFlightScope&) = delete;
};

// Waits for a handle, but stops early when the mod is being unloaded.
void WaitUnlessUnloading(HANDLE handle, DWORD timeoutMs) {
    ULONGLONG start = GetTickCount64();
    while (!g_unloading) {
        ULONGLONG elapsed = GetTickCount64() - start;
        if (elapsed >= timeoutMs) {
            return;
        }

        DWORD slice = static_cast<DWORD>(std::min<ULONGLONG>(100, timeoutMs - elapsed));
        if (WaitForSingleObject(handle, slice) != WAIT_TIMEOUT) {
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Small helpers

class UniqueHandle {
   public:
    UniqueHandle() = default;
    explicit UniqueHandle(HANDLE handle) : m_handle(handle) {}
    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;
    UniqueHandle(UniqueHandle&& other) noexcept
        : m_handle(std::exchange(other.m_handle, nullptr)) {}
    UniqueHandle& operator=(UniqueHandle&& other) noexcept {
        if (this != &other) {
            Reset();
            m_handle = std::exchange(other.m_handle, nullptr);
        }
        return *this;
    }
    ~UniqueHandle() { Reset(); }

    HANDLE Get() const { return m_handle; }
    explicit operator bool() const { return m_handle != nullptr; }

    void Reset() {
        if (m_handle) {
            CloseHandle(m_handle);
            m_handle = nullptr;
        }
    }

   private:
    HANDLE m_handle = nullptr;
};

class UniqueEvtHandle {
   public:
    UniqueEvtHandle() = default;
    explicit UniqueEvtHandle(EVT_HANDLE handle) : m_handle(handle) {}
    UniqueEvtHandle(const UniqueEvtHandle&) = delete;
    UniqueEvtHandle& operator=(const UniqueEvtHandle&) = delete;
    UniqueEvtHandle(UniqueEvtHandle&& other) noexcept
        : m_handle(std::exchange(other.m_handle, nullptr)) {}
    UniqueEvtHandle& operator=(UniqueEvtHandle&& other) noexcept {
        if (this != &other) {
            Reset();
            m_handle = std::exchange(other.m_handle, nullptr);
        }
        return *this;
    }
    ~UniqueEvtHandle() { Reset(); }

    EVT_HANDLE Get() const { return m_handle; }
    explicit operator bool() const { return m_handle != nullptr; }

    void Reset() {
        if (m_handle) {
            EvtClose(m_handle);
            m_handle = nullptr;
        }
    }

   private:
    EVT_HANDLE m_handle = nullptr;
};

ULONGLONG FileTimeToUInt64(const FILETIME& fileTime) {
    return (static_cast<ULONGLONG>(fileTime.dwHighDateTime) << 32) |
           fileTime.dwLowDateTime;
}

ULONGLONG GetCurrentFileTime() {
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    return FileTimeToUInt64(fileTime);
}

bool EqualsIgnoreCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) {
        return false;
    }

    if (a.empty()) {
        return true;
    }

    return CompareStringOrdinal(a.data(), static_cast<int>(a.size()), b.data(),
                                static_cast<int>(b.size()),
                                TRUE) == CSTR_EQUAL;
}

bool StartsWithIgnoreCase(std::wstring_view text, std::wstring_view prefix) {
    return text.size() >= prefix.size() &&
           EqualsIgnoreCase(text.substr(0, prefix.size()), prefix);
}

size_t FindIgnoreCase(std::wstring_view text, std::wstring_view part) {
    if (part.empty()) {
        return 0;
    }

    for (size_t i = 0; i + part.size() <= text.size(); i++) {
        if (EqualsIgnoreCase(text.substr(i, part.size()), part)) {
            return i;
        }
    }

    return std::wstring_view::npos;
}

bool ContainsIgnoreCase(std::wstring_view text, std::wstring_view part) {
    return FindIgnoreCase(text, part) != std::wstring_view::npos;
}

bool IsOneOfIgnoreCase(std::wstring_view text,
                       std::initializer_list<PCWSTR> candidates) {
    for (PCWSTR candidate : candidates) {
        if (EqualsIgnoreCase(text, candidate)) {
            return true;
        }
    }

    return false;
}

void AddUniqueIgnoreCase(std::vector<std::wstring>& list,
                         const std::wstring& value) {
    if (value.empty()) {
        return;
    }

    for (const auto& item : list) {
        if (EqualsIgnoreCase(item, value)) {
            return;
        }
    }

    list.push_back(value);
}

std::wstring Trim(std::wstring_view text) {
    constexpr std::wstring_view kWhitespace = L" \t\r\n";
    size_t start = text.find_first_not_of(kWhitespace);
    if (start == std::wstring_view::npos) {
        return {};
    }

    size_t end = text.find_last_not_of(kWhitespace);
    return std::wstring(text.substr(start, end - start + 1));
}

std::wstring FileNameFromPath(std::wstring_view path) {
    size_t pos = path.find_last_of(L"\\/");
    return std::wstring(pos == std::wstring_view::npos ? path
                                                        : path.substr(pos + 1));
}

std::vector<std::wstring> SplitMultiSz(const WCHAR* text) {
    std::vector<std::wstring> items;
    if (!text) {
        return items;
    }

    while (*text) {
        size_t length = wcslen(text);
        items.emplace_back(text, length);
        text += length + 1;
    }

    return items;
}

std::vector<std::wstring> SplitLines(std::wstring_view text) {
    std::vector<std::wstring> lines;
    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find_first_of(L"\r\n", start);
        if (end == std::wstring_view::npos) {
            end = text.size();
        }

        std::wstring line = Trim(text.substr(start, end - start));
        if (!line.empty()) {
            lines.push_back(std::move(line));
        }

        start = end + 1;
    }

    return lines;
}

std::wstring JoinWithAnd(const std::vector<std::wstring>& items) {
    std::wstring result;
    for (size_t i = 0; i < items.size(); i++) {
        if (i > 0) {
            result += i + 1 == items.size() ? L" and " : L", ";
        }

        result += items[i];
    }

    return result;
}

std::wstring Join(const std::vector<std::wstring>& items,
                  std::wstring_view separator) {
    std::wstring result;
    for (size_t i = 0; i < items.size(); i++) {
        if (i > 0) {
            result += separator;
        }

        result += items[i];
    }

    return result;
}

std::wstring ToHex(DWORD value, int digits) {
    constexpr WCHAR kDigits[] = L"0123456789ABCDEF";
    std::wstring result(digits, L'0');
    for (int i = digits - 1; i >= 0; i--) {
        result[i] = kDigits[value & 0xF];
        value >>= 4;
    }

    return result;
}

HMODULE GetModuleFromAddress(const void* address) {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            static_cast<LPCWSTR>(address), &module)) {
        return nullptr;
    }

    return module;
}

bool IsCallerHotplug(const void* returnAddress) {
    HMODULE hotplug = GetModuleHandleW(L"hotplug.dll");
    return hotplug && GetModuleFromAddress(returnAddress) == hotplug;
}

void LoadSettings() {
    Settings settings;
    settings.autoEject = Wh_GetIntSetting(L"autoEject") != 0;
    settings.programButtons = Wh_GetIntSetting(L"programButtons") != 0;
    settings.elevatedEndTask = Wh_GetIntSetting(L"elevatedEndTask") != 0;
    settings.forceEject = Wh_GetIntSetting(L"forceEject") != 0;
    settings.reportWaitMs =
        std::clamp(Wh_GetIntSetting(L"reportWaitMs"), 0, 10000);

    std::lock_guard lock(g_settingsMutex);
    g_settings = settings;
}

Settings GetSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

////////////////////////////////////////////////////////////////////////////////
// Kernel-PnP event 225: "The application ... with process id ... stopped the
// removal or ejection for the device ..."

struct VetoEvent {
    DWORD processId = 0;
    ULONGLONG time = 0;
    std::wstring processName;  // An NT path, or "System".
    std::wstring commandLine;
    std::vector<std::wstring> vetoingDevices;
};

// Returns a bookmark of the newest System log event. Events of an eject
// attempt that starts after this call come after the bookmark.
EVT_HANDLE CreateSystemLogBookmark() {
    UniqueEvtHandle query(EvtQuery(nullptr, L"System", L"*",
                                   EvtQueryChannelPath |
                                       EvtQueryReverseDirection));
    if (!query) {
        Wh_Log(L"EvtQuery failed: %u", GetLastError());
        return nullptr;
    }

    EVT_HANDLE event = nullptr;
    DWORD returned = 0;
    if (!EvtNext(query.Get(), 1, &event, 500, 0, &returned) ||
        returned != 1) {
        return nullptr;
    }

    UniqueEvtHandle eventHandle(event);
    EVT_HANDLE bookmark = EvtCreateBookmark(nullptr);
    if (bookmark && !EvtUpdateBookmark(bookmark, event)) {
        EvtClose(bookmark);
        bookmark = nullptr;
    }

    return bookmark;
}

DWORD GetVariantType(const EVT_VARIANT& value) {
    return value.Type & EVT_VARIANT_TYPE_MASK;
}

std::wstring VariantToString(const EVT_VARIANT& value) {
    if (GetVariantType(value) == EvtVarTypeString && value.StringVal) {
        return value.StringVal;
    }

    return {};
}

ULONGLONG VariantToUInt64(const EVT_VARIANT& value) {
    switch (GetVariantType(value)) {
        case EvtVarTypeByte:
            return value.ByteVal;
        case EvtVarTypeUInt16:
            return value.UInt16Val;
        case EvtVarTypeUInt32:
        case EvtVarTypeHexInt32:
            return value.UInt32Val;
        case EvtVarTypeUInt64:
        case EvtVarTypeHexInt64:
            return value.UInt64Val;
        case EvtVarTypeInt32:
            return value.Int32Val > 0 ? value.Int32Val : 0;
        case EvtVarTypeInt64:
            return value.Int64Val > 0 ? value.Int64Val : 0;
        case EvtVarTypeString:
            return value.StringVal ? wcstoull(value.StringVal, nullptr, 0) : 0;
        default:
            return 0;
    }
}

ULONGLONG VariantToFileTime(const EVT_VARIANT& value) {
    switch (GetVariantType(value)) {
        case EvtVarTypeFileTime:
            return value.FileTimeVal;
        case EvtVarTypeSysTime: {
            FILETIME fileTime;
            if (value.SysTimeVal &&
                SystemTimeToFileTime(value.SysTimeVal, &fileTime)) {
                return FileTimeToUInt64(fileTime);
            }
            return 0;
        }
        default:
            return 0;
    }
}

bool ReadVetoEvent(EVT_HANDLE context,
                   EVT_HANDLE event,
                   std::vector<BYTE>& buffer,
                   const std::wstring& deviceInstanceId,
                   ULONGLONG sinceFileTime,
                   VetoEvent& result) {
    DWORD used = 0;
    DWORD count = 0;
    if (!EvtRender(context, event, EvtRenderEventValues,
                   static_cast<DWORD>(buffer.size()), buffer.data(), &used,
                   &count)) {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            return false;
        }

        buffer.resize(used);
        if (!EvtRender(context, event, EvtRenderEventValues,
                       static_cast<DWORD>(buffer.size()), buffer.data(), &used,
                       &count)) {
            return false;
        }
    }

    if (count < 8) {
        return false;
    }

    const auto* values = reinterpret_cast<const EVT_VARIANT*>(buffer.data());

    if (!EqualsIgnoreCase(VariantToString(values[0]),
                          L"Microsoft-Windows-Kernel-PnP") ||
        VariantToUInt64(values[1]) != 225) {
        return false;
    }

    result.time = VariantToFileTime(values[2]);
    if (result.time && sinceFileTime &&
        result.time + 2 * kFileTimeTicksPerSecond < sinceFileTime) {
        return false;
    }

    if (!EqualsIgnoreCase(VariantToString(values[5]), deviceInstanceId)) {
        return false;
    }

    result.processId = static_cast<DWORD>(VariantToUInt64(values[3]));
    result.processName = Trim(VariantToString(values[4]));
    result.commandLine = Trim(VariantToString(values[6]));
    result.vetoingDevices = SplitLines(VariantToString(values[7]));
    return true;
}

// Returns the Kernel-PnP veto events of the given device that were logged
// after the bookmark. Without a bookmark, events since the given time are
// returned.
std::vector<VetoEvent> QueryVetoEvents(const std::wstring& deviceInstanceId,
                                       EVT_HANDLE bookmark,
                                       ULONGLONG sinceFileTime) {
    std::vector<VetoEvent> events;

    UniqueEvtHandle query;
    if (bookmark) {
        query = UniqueEvtHandle(
            EvtQuery(nullptr, L"System", L"*",
                     EvtQueryChannelPath | EvtQueryForwardDirection));
        if (query &&
            !EvtSeek(query.Get(), 1, bookmark, 0, EvtSeekRelativeToBookmark)) {
            Wh_Log(L"EvtSeek failed: %u", GetLastError());
            query.Reset();
        }
    }

    if (!query) {
        ULONGLONG now = GetCurrentFileTime();
        ULONGLONG windowMs =
            (now > sinceFileTime ? (now - sinceFileTime) / kFileTimeTicksPerMs
                                 : 0) +
            60000;
        std::wstring xpath =
            L"*[System[Provider[@Name='Microsoft-Windows-Kernel-PnP'] and "
            L"(EventID=225) and TimeCreated[timediff(@SystemTime) <= " +
            std::to_wstring(windowMs) + L"]]]";
        query = UniqueEvtHandle(EvtQuery(
            nullptr, L"System", xpath.c_str(),
            EvtQueryChannelPath | EvtQueryReverseDirection |
                EvtQueryTolerateQueryErrors));
    }

    if (!query) {
        Wh_Log(L"EvtQuery failed: %u", GetLastError());
        return events;
    }

    LPCWSTR valuePaths[] = {
        L"Event/System/Provider/@Name",
        L"Event/System/EventID",
        L"Event/System/TimeCreated/@SystemTime",
        L"Event/EventData/Data[@Name='ProcessId']",
        L"Event/EventData/Data[@Name='ProcessName']",
        L"Event/EventData/Data[@Name='DeviceInstance']",
        L"Event/EventData/Data[@Name='CommandLine']",
        L"Event/EventData/Data[@Name='VetoingDevices']",
    };
    UniqueEvtHandle context(EvtCreateRenderContext(
        ARRAYSIZE(valuePaths), valuePaths, EvtRenderContextValues));
    if (!context) {
        Wh_Log(L"EvtCreateRenderContext failed: %u", GetLastError());
        return events;
    }

    std::vector<BYTE> buffer(4096);
    EVT_HANDLE handles[32];
    for (int batch = 0; batch < 64; batch++) {
        DWORD returned = 0;
        if (!EvtNext(query.Get(), ARRAYSIZE(handles), handles, 1000, 0,
                     &returned)) {
            break;
        }

        for (DWORD i = 0; i < returned; i++) {
            UniqueEvtHandle event(handles[i]);
            VetoEvent vetoEvent;
            if (ReadVetoEvent(context.Get(), event.Get(), buffer,
                              deviceInstanceId, sinceFileTime, vetoEvent)) {
                events.push_back(std::move(vetoEvent));
            }
        }
    }

    return events;
}

std::vector<VetoEvent> WaitForVetoEvents(const std::wstring& deviceInstanceId,
                                         EVT_HANDLE bookmark,
                                         ULONGLONG sinceFileTime,
                                         int waitMs) {
    ULONGLONG deadline = GetTickCount64() + std::max(waitMs, 0);
    std::vector<VetoEvent> events;
    while (true) {
        events = QueryVetoEvents(deviceInstanceId, bookmark, sinceFileTime);
        if (!events.empty() || g_unloading || GetTickCount64() >= deadline) {
            break;
        }

        Sleep(bookmark ? 100 : 400);
    }

    if (!events.empty() && !g_unloading) {
        // All events of a failed eject are logged together, but they can
        // become visible one after another.
        Sleep(200);
        std::vector<VetoEvent> more =
            QueryVetoEvents(deviceInstanceId, bookmark, sinceFileTime);
        if (more.size() > events.size()) {
            events = std::move(more);
        }
    }

    return events;
}

////////////////////////////////////////////////////////////////////////////////
// Devices and volumes

std::wstring GetDeviceInstanceId(DEVINST devInst) {
    WCHAR buffer[MAX_DEVICE_ID_LEN + 1] = {};
    if (CM_Get_Device_IDW(devInst, buffer, ARRAYSIZE(buffer), 0) !=
        CR_SUCCESS) {
        return {};
    }

    return buffer;
}

std::wstring GetDevNodeString(DEVINST devInst, const DEVPROPKEY& key) {
    WCHAR buffer[512] = {};
    DEVPROPTYPE type = 0;
    ULONG size = sizeof(buffer) - sizeof(WCHAR);
    if (CM_Get_DevNode_PropertyW(devInst, &key, &type,
                                 reinterpret_cast<PBYTE>(buffer), &size,
                                 0) != CR_SUCCESS ||
        type != DEVPROP_TYPE_STRING) {
        return {};
    }

    return Trim(buffer);
}

std::wstring GetDeviceName(DEVINST devInst) {
    std::wstring name = GetDevNodeString(devInst, kFriendlyNameKey);
    if (name.empty()) {
        name = GetDevNodeString(devInst, kDeviceDescKey);
    }

    if (name.empty()) {
        name = L"USB Mass Storage Device";
    }

    return name;
}

bool IsVolumeInstanceId(std::wstring_view instanceId) {
    return StartsWithIgnoreCase(instanceId, L"STORAGE\\VOLUME\\");
}

std::vector<std::wstring> GetRemovalRelations(const std::wstring& instanceId) {
    ULONG length = 0;
    if (CM_Get_Device_ID_List_SizeW(&length, instanceId.c_str(),
                                    CM_GETIDLIST_FILTER_REMOVALRELATIONS) !=
            CR_SUCCESS ||
        length <= 1) {
        return {};
    }

    std::vector<WCHAR> buffer(length + 1);
    if (CM_Get_Device_ID_ListW(instanceId.c_str(), buffer.data(), length,
                               CM_GETIDLIST_FILTER_REMOVALRELATIONS) !=
        CR_SUCCESS) {
        return {};
    }

    return SplitMultiSz(buffer.data());
}

void CollectDeviceTree(DEVINST devInst,
                       std::vector<DEVINST>& nodes,
                       int depth) {
    if (depth > 8 || nodes.size() >= 64) {
        return;
    }

    nodes.push_back(devInst);

    DEVINST child = 0;
    if (CM_Get_Child(&child, devInst, 0) != CR_SUCCESS) {
        return;
    }

    do {
        CollectDeviceTree(child, nodes, depth + 1);
    } while (nodes.size() < 64 && CM_Get_Sibling(&child, child, 0) == CR_SUCCESS);
}

// Volumes of a device are not its children. They are reported as removal
// relations of the disk below the device.
std::vector<std::wstring> FindDeviceVolumeIds(DEVINST devInst) {
    std::vector<DEVINST> nodes;
    CollectDeviceTree(devInst, nodes, 0);

    std::vector<std::wstring> volumeIds;
    for (DEVINST node : nodes) {
        std::wstring instanceId = GetDeviceInstanceId(node);
        if (instanceId.empty()) {
            continue;
        }

        if (IsVolumeInstanceId(instanceId)) {
            AddUniqueIgnoreCase(volumeIds, instanceId);
        }

        for (const auto& relation : GetRemovalRelations(instanceId)) {
            if (IsVolumeInstanceId(relation)) {
                AddUniqueIgnoreCase(volumeIds, relation);
            }
        }
    }

    return volumeIds;
}

struct VolumeInfo {
    std::wstring instanceId;
    std::wstring guidPath;                  // \\?\Volume{...}\ .
    std::vector<std::wstring> mountPoints;  // E:\ and mounted folders.
    std::wstring label;
    bool inUse = false;
};

VolumeInfo DescribeVolume(const std::wstring& instanceId) {
    VolumeInfo volume;
    volume.instanceId = instanceId;

    GUID interfaceGuid = kVolumeInterfaceGuid;
    std::wstring deviceId = instanceId;
    ULONG length = 0;
    if (CM_Get_Device_Interface_List_SizeW(
            &length, &interfaceGuid, deviceId.data(),
            CM_GET_DEVICE_INTERFACE_LIST_PRESENT) != CR_SUCCESS ||
        length <= 1) {
        return volume;
    }

    std::vector<WCHAR> interfaces(length + 1);
    if (CM_Get_Device_Interface_ListW(&interfaceGuid, deviceId.data(),
                                      interfaces.data(), length,
                                      CM_GET_DEVICE_INTERFACE_LIST_PRESENT) !=
        CR_SUCCESS) {
        return volume;
    }

    std::wstring interfacePath = interfaces.data();
    if (interfacePath.empty()) {
        return volume;
    }

    if (interfacePath.back() != L'\\') {
        interfacePath += L'\\';
    }

    WCHAR guidPath[MAX_PATH] = {};
    if (!GetVolumeNameForVolumeMountPointW(interfacePath.c_str(), guidPath,
                                           ARRAYSIZE(guidPath))) {
        return volume;
    }

    volume.guidPath = guidPath;

    std::vector<WCHAR> pathNames(MAX_PATH + 1);
    DWORD needed = 0;
    if (!GetVolumePathNamesForVolumeNameW(
            guidPath, pathNames.data(), static_cast<DWORD>(pathNames.size()),
            &needed)) {
        pathNames.assign(2, L'\0');
        if (GetLastError() == ERROR_MORE_DATA && needed) {
            pathNames.assign(needed + 1, L'\0');
            if (!GetVolumePathNamesForVolumeNameW(
                    guidPath, pathNames.data(),
                    static_cast<DWORD>(pathNames.size()), &needed)) {
                pathNames.assign(2, L'\0');
            }
        }
    }

    volume.mountPoints = SplitMultiSz(pathNames.data());

    DWORD oldErrorMode = 0;
    SetThreadErrorMode(SEM_FAILCRITICALERRORS, &oldErrorMode);
    WCHAR label[MAX_PATH + 1] = {};
    if (GetVolumeInformationW(guidPath, label, ARRAYSIZE(label), nullptr,
                              nullptr, nullptr, nullptr, 0)) {
        volume.label = Trim(label);
    }
    SetThreadErrorMode(oldErrorMode, nullptr);

    return volume;
}

// Returns "E:" for a volume with a drive letter.
std::wstring GetVolumeLetter(const VolumeInfo& volume) {
    for (const auto& mountPoint : volume.mountPoints) {
        if (mountPoint.size() == 3 && mountPoint[1] == L':' &&
            mountPoint[2] == L'\\') {
            return mountPoint.substr(0, 2);
        }
    }

    return {};
}

std::wstring GetVolumeDisplayName(const VolumeInfo& volume) {
    std::wstring location = GetVolumeLetter(volume);
    if (location.empty() && !volume.mountPoints.empty()) {
        location = volume.mountPoints.front();
    }

    if (!volume.label.empty() && !location.empty()) {
        return volume.label + L" (" + location + L")";
    }

    if (!location.empty()) {
        return L"Drive " + location;
    }

    return volume.label;
}

// The argument for "fsutil volume dismount".
std::wstring GetVolumeDismountTarget(const VolumeInfo& volume) {
    std::wstring letter = GetVolumeLetter(volume);
    if (!letter.empty()) {
        return letter;
    }

    std::wstring path = volume.guidPath;
    if (!path.empty() && path.back() == L'\\') {
        path.pop_back();
    }

    return path;
}

////////////////////////////////////////////////////////////////////////////////
// Blocking processes

enum class BlockerKind {
    App,        // Can be closed or ended from the dialog.
    Service,    // A service or a process of another session.
    Explorer,   // File Explorer, which hosts this dialog.
    Protected,  // A critical or core Windows process.
    System,     // The System process (PID 4).
};

struct Blocker {
    DWORD pid = 0;
    ULONGLONG eventTime = 0;
    std::wstring ntImagePath;
    std::wstring imagePath;
    std::wstring exeName;
    std::wstring commandLine;
    std::vector<std::wstring> vetoingDevices;

    std::wstring displayName;
    std::wstring comName;
    std::wstring services;
    std::wstring hint;
    BlockerKind kind = BlockerKind::App;

    // SYNCHRONIZE access, used to notice when the process exits.
    UniqueHandle process;
    bool alive = true;
    // Whether the exit of the process can be noticed.
    bool trackable = false;
    bool hasWindows = false;
    bool canTerminate = false;
    bool needsElevation = false;

    bool closeRequested = false;
    ULONGLONG closeRequestedTick = 0;
    // The program didn't close after being asked, so "End" is offered.
    bool forceOffered = false;
};

struct DescribeContext {
    DWORD sessionId = 0;
    bool sessionKnown = false;
    std::multimap<DWORD, std::wstring> services;
    std::wstring wslPath;
};

std::wstring NtPathToDosPath(const std::wstring& ntPath) {
    if (ntPath.empty() || ntPath[0] != L'\\') {
        return ntPath;
    }

    WCHAR drives[512] = {};
    DWORD length = GetLogicalDriveStringsW(ARRAYSIZE(drives) - 1, drives);
    if (!length || length >= ARRAYSIZE(drives)) {
        return ntPath;
    }

    for (const auto& drive : SplitMultiSz(drives)) {
        if (drive.size() < 2) {
            continue;
        }

        std::wstring letter = drive.substr(0, 2);
        WCHAR target[MAX_PATH] = {};
        if (!QueryDosDeviceW(letter.c_str(), target, ARRAYSIZE(target))) {
            continue;
        }

        std::wstring_view prefix = target;
        if (ntPath.size() > prefix.size() &&
            StartsWithIgnoreCase(ntPath, prefix) &&
            ntPath[prefix.size()] == L'\\') {
            return letter + ntPath.substr(prefix.size());
        }
    }

    return ntPath;
}

std::wstring GetFileDescription(const std::wstring& path) {
    if (path.empty()) {
        return {};
    }

    DWORD handle = 0;
    DWORD size =
        GetFileVersionInfoSizeExW(FILE_VER_GET_LOCALISED, path.c_str(), &handle);
    if (!size) {
        return {};
    }

    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoExW(FILE_VER_GET_LOCALISED, path.c_str(), 0, size,
                               data.data())) {
        return {};
    }

    struct LanguageAndCodePage {
        WORD language;
        WORD codePage;
    };

    std::vector<std::wstring> keys;
    LanguageAndCodePage* translations = nullptr;
    UINT translationsSize = 0;
    if (VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation",
                       reinterpret_cast<void**>(&translations),
                       &translationsSize) &&
        translations) {
        for (UINT i = 0; i < translationsSize / sizeof(LanguageAndCodePage);
             i++) {
            keys.push_back(L"\\StringFileInfo\\" +
                           ToHex(translations[i].language, 4) +
                           ToHex(translations[i].codePage, 4) +
                           L"\\FileDescription");
        }
    }

    keys.push_back(L"\\StringFileInfo\\040904B0\\FileDescription");
    keys.push_back(L"\\StringFileInfo\\040904E4\\FileDescription");

    for (const auto& key : keys) {
        WCHAR* value = nullptr;
        UINT valueLength = 0;
        if (VerQueryValueW(data.data(), key.c_str(),
                           reinterpret_cast<void**>(&value), &valueLength) &&
            value && valueLength > 1) {
            std::wstring description = Trim(value);
            if (!description.empty()) {
                return description;
            }
        }
    }

    return {};
}

std::wstring ReadRegistryString(HKEY root,
                                const std::wstring& subKey,
                                PCWSTR valueName) {
    WCHAR buffer[512] = {};
    DWORD size = sizeof(buffer) - sizeof(WCHAR);
    if (RegGetValueW(root, subKey.c_str(), valueName, RRF_RT_REG_SZ, nullptr,
                     buffer, &size) != ERROR_SUCCESS) {
        return {};
    }

    return Trim(buffer);
}

std::wstring FindComClassNameByAppId(const std::wstring& appId) {
    HKEY clsidKey = nullptr;
    if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"CLSID", 0, KEY_READ, &clsidKey) !=
        ERROR_SUCCESS) {
        return {};
    }

    std::wstring result;
    for (DWORD index = 0;; index++) {
        WCHAR clsid[128];
        DWORD clsidLength = ARRAYSIZE(clsid);
        LONG status = RegEnumKeyExW(clsidKey, index, clsid, &clsidLength,
                                    nullptr, nullptr, nullptr, nullptr);
        if (status == ERROR_NO_MORE_ITEMS) {
            break;
        }

        if (status != ERROR_SUCCESS) {
            continue;
        }

        WCHAR value[64] = {};
        DWORD valueSize = sizeof(value) - sizeof(WCHAR);
        if (RegGetValueW(clsidKey, clsid, L"AppID", RRF_RT_REG_SZ, nullptr,
                         value, &valueSize) != ERROR_SUCCESS ||
            !EqualsIgnoreCase(value, appId)) {
            continue;
        }

        result = ReadRegistryString(clsidKey, clsid, nullptr);
        if (!result.empty()) {
            break;
        }
    }

    RegCloseKey(clsidKey);
    return result;
}

std::mutex g_comNameCacheMutex;
std::map<std::wstring, std::wstring> g_comNameCache;

// Returns the name of the COM server that a COM Surrogate (dllhost.exe
// /Processid:{...}) process hosts.
std::wstring GetComSurrogateName(const std::wstring& commandLine) {
    size_t pos = FindIgnoreCase(commandLine, L"/processid:");
    if (pos == std::wstring::npos) {
        return {};
    }

    size_t open = commandLine.find(L'{', pos);
    size_t close =
        open == std::wstring::npos ? open : commandLine.find(L'}', open);
    if (close == std::wstring::npos) {
        return {};
    }

    std::wstring guid = commandLine.substr(open, close - open + 1);
    CharUpperBuffW(guid.data(), static_cast<DWORD>(guid.size()));

    {
        std::lock_guard lock(g_comNameCacheMutex);
        auto it = g_comNameCache.find(guid);
        if (it != g_comNameCache.end()) {
            return it->second;
        }
    }

    std::wstring name =
        ReadRegistryString(HKEY_CLASSES_ROOT, L"AppID\\" + guid, nullptr);
    if (name.empty()) {
        name = ReadRegistryString(HKEY_CLASSES_ROOT, L"CLSID\\" + guid, nullptr);
    }

    if (name.empty()) {
        name = FindComClassNameByAppId(guid);
    }

    std::lock_guard lock(g_comNameCacheMutex);
    g_comNameCache[guid] = name;
    return name;
}

std::multimap<DWORD, std::wstring> GetRunningServicesByProcess() {
    std::multimap<DWORD, std::wstring> services;

    SC_HANDLE manager =
        OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
    if (!manager) {
        return services;
    }

    std::vector<BYTE> buffer(64 * 1024);
    DWORD resumeHandle = 0;
    for (int i = 0; i < 16; i++) {
        DWORD bytesNeeded = 0;
        DWORD count = 0;
        BOOL succeeded = EnumServicesStatusExW(
            manager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_ACTIVE,
            buffer.data(), static_cast<DWORD>(buffer.size()), &bytesNeeded,
            &count, &resumeHandle, nullptr);
        if (!succeeded && GetLastError() != ERROR_MORE_DATA) {
            break;
        }

        const auto* entries =
            reinterpret_cast<const ENUM_SERVICE_STATUS_PROCESSW*>(
                buffer.data());
        for (DWORD j = 0; j < count; j++) {
            DWORD pid = entries[j].ServiceStatusProcess.dwProcessId;
            PCWSTR name = entries[j].lpDisplayName && *entries[j].lpDisplayName
                              ? entries[j].lpDisplayName
                              : entries[j].lpServiceName;
            if (pid && name) {
                services.emplace(pid, name);
            }
        }

        if (succeeded) {
            break;
        }

        if (bytesNeeded > buffer.size()) {
            buffer.resize(bytesNeeded);
        }
    }

    CloseServiceHandle(manager);
    return services;
}

struct FindWindowsParam {
    DWORD pid;
    std::vector<HWND> windows;
};

BOOL CALLBACK FindProcessWindowsProc(HWND hwnd, LPARAM lParam) {
    auto& param = *reinterpret_cast<FindWindowsParam*>(lParam);

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != param.pid || !IsWindowVisible(hwnd) ||
        GetWindow(hwnd, GW_OWNER) ||
        (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)) {
        return TRUE;
    }

    param.windows.push_back(hwnd);
    return TRUE;
}

// Visible top-level windows of a process, front-most first.
std::vector<HWND> GetProcessTopLevelWindows(DWORD pid) {
    FindWindowsParam param{pid, {}};
    EnumWindows(FindProcessWindowsProc, reinterpret_cast<LPARAM>(&param));
    return param.windows;
}

// Checks that a process handle still refers to the process from the event,
// and not to a new process that reused its ID.
bool ProcessMatches(HANDLE process,
                    const std::wstring& exeName,
                    ULONGLONG eventTime,
                    std::wstring* imagePath) {
    WCHAR path[MAX_PATH * 2] = {};
    DWORD size = ARRAYSIZE(path);
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        if (imagePath) {
            *imagePath = path;
        }

        if (!exeName.empty() &&
            !EqualsIgnoreCase(FileNameFromPath(path), exeName)) {
            return false;
        }
    }

    FILETIME creation{};
    FILETIME exit{};
    FILETIME kernel{};
    FILETIME user{};
    if (eventTime && GetProcessTimes(process, &creation, &exit, &kernel, &user) &&
        FileTimeToUInt64(creation) > eventTime + kFileTimeTicksPerSecond) {
        return false;
    }

    return true;
}

bool IsCriticalProcess(HANDLE process) {
    using IsProcessCritical_t = BOOL(WINAPI*)(HANDLE, PBOOL);
    static auto isProcessCritical = reinterpret_cast<IsProcessCritical_t>(
        GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "IsProcessCritical"));

    BOOL critical = FALSE;
    return isProcessCritical && isProcessCritical(process, &critical) &&
           critical;
}

bool IsCoreWindowsProcess(const std::wstring& exeName) {
    return IsOneOfIgnoreCase(
        exeName, {L"csrss.exe", L"smss.exe", L"wininit.exe", L"winlogon.exe",
                  L"services.exe", L"lsass.exe", L"lsaiso.exe", L"svchost.exe",
                  L"dwm.exe", L"fontdrvhost.exe", L"sihost.exe", L"ctfmon.exe",
                  L"audiodg.exe", L"spoolsv.exe", L"WUDFHost.exe",
                  L"Registry", L"MemCompression", L"Memory Compression",
                  L"Secure System"});
}

bool IsWslProcess(const std::wstring& exeName) {
    return IsOneOfIgnoreCase(
        exeName, {L"wsl.exe", L"wslhost.exe", L"wslservice.exe",
                  L"wslrelay.exe", L"vmmem", L"vmmemWSL"});
}

// A one-line hint below the program in the list. Task dialogs can't indent
// wrapped lines, so the hints are kept short enough to fit on one line.
std::wstring GetBlockerHint(const Blocker& blocker,
                            const DescribeContext& context) {
    const std::wstring& exe = blocker.exeName;

    if (blocker.kind == BlockerKind::System) {
        return L"Windows or a driver has the drive open. It usually lets go "
               L"once programs close.";
    }

    if (blocker.kind == BlockerKind::Explorer) {
        return L"File Explorer has something open on the drive. Close windows "
               L"that show it.";
    }

    if (ContainsIgnoreCase(blocker.comName, L"Plan9") || IsWslProcess(exe)) {
        return L"WSL shares this drive as " + context.wslPath +
               L". Close WSL shells or run \"wsl --shutdown\".";
    }

    if (IsOneOfIgnoreCase(exe, {L"SearchIndexer.exe", L"SearchProtocolHost.exe",
                                L"SearchFilterHost.exe"})) {
        return L"Windows Search is indexing the drive. It usually stops by "
               L"itself.";
    }

    if (IsOneOfIgnoreCase(exe, {L"MsMpEng.exe", L"MpDefenderCoreService.exe",
                                L"NisSrv.exe", L"MpCopyAccelerator.exe"})) {
        return L"Microsoft Defender is scanning the drive. It usually finishes "
               L"by itself.";
    }

    if (!blocker.services.empty()) {
        return L"Runs the service: " + blocker.services + L".";
    }

    if (EqualsIgnoreCase(exe, L"dllhost.exe")) {
        return L"Runs helpers for other programs, such as thumbnails or file "
               L"properties.";
    }

    if (blocker.kind == BlockerKind::Protected) {
        return L"A core Windows process, so it isn't closed from here.";
    }

    if (blocker.kind == BlockerKind::Service) {
        return L"Runs as a service or for another user, so it isn't closed "
               L"from here.";
    }

    return {};
}

// A longer explanation for the details section, where text can wrap freely.
std::wstring GetBlockerDetailNote(const Blocker& blocker) {
    if (blocker.kind == BlockerKind::System) {
        return L"Windows or a driver has the drive open, for example for "
               L"indexing, virus scanning, WSL or a mounted disk image. If it "
               L"doesn't let go after the other programs are closed, use Force "
               L"eject.";
    }

    if (blocker.kind == BlockerKind::Explorer) {
        return L"File Explorer has something on the drive open, such as a "
               L"folder window, the preview pane or a thumbnail. It can't be "
               L"closed from here, because this dialog runs inside it.";
    }

    return {};
}

void DescribeBlocker(Blocker& blocker, const DescribeContext& context) {
    if (blocker.pid == 4 || blocker.pid == 0 ||
        EqualsIgnoreCase(blocker.ntImagePath, L"System")) {
        blocker.kind = BlockerKind::System;
        blocker.displayName = L"System";
        blocker.hint = GetBlockerHint(blocker, context);
        return;
    }

    blocker.exeName = FileNameFromPath(blocker.ntImagePath);

    UniqueHandle process(OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, blocker.pid));
    if (process) {
        std::wstring imagePath;
        bool matches = ProcessMatches(process.Get(), blocker.exeName,
                                      blocker.eventTime, &imagePath);
        blocker.trackable = true;
        if (!matches || WaitForSingleObject(process.Get(), 0) == WAIT_OBJECT_0) {
            blocker.alive = false;
        } else {
            blocker.imagePath = std::move(imagePath);
            blocker.process = std::move(process);
        }
    } else if (GetLastError() == ERROR_INVALID_PARAMETER) {
        // The process doesn't exist anymore.
        blocker.alive = false;
        blocker.trackable = true;
    }

    if (blocker.imagePath.empty()) {
        blocker.imagePath = NtPathToDosPath(blocker.ntImagePath);
    }

    if (blocker.exeName.empty()) {
        blocker.exeName = FileNameFromPath(blocker.imagePath);
    }

    std::wstring description = GetFileDescription(blocker.imagePath);
    blocker.displayName = !description.empty() ? description : blocker.exeName;

    std::vector<std::wstring> services;
    auto range = context.services.equal_range(blocker.pid);
    for (auto it = range.first; it != range.second; ++it) {
        services.push_back(it->second);
    }
    std::sort(services.begin(), services.end());
    blocker.services = Join(services, L", ");

    if (EqualsIgnoreCase(blocker.exeName, L"dllhost.exe")) {
        blocker.comName = GetComSurrogateName(blocker.commandLine);
        if (!blocker.comName.empty()) {
            blocker.displayName += L" (" + blocker.comName + L")";
        }
    }

    DWORD sessionId = 0;
    bool sessionKnown = ProcessIdToSessionId(blocker.pid, &sessionId) != FALSE;

    if (EqualsIgnoreCase(blocker.exeName, L"explorer.exe")) {
        blocker.kind = BlockerKind::Explorer;
    } else if (IsCoreWindowsProcess(blocker.exeName) ||
               (blocker.process && IsCriticalProcess(blocker.process.Get()))) {
        blocker.kind = BlockerKind::Protected;
    } else if (!services.empty() ||
               (sessionKnown && context.sessionKnown &&
                sessionId != context.sessionId)) {
        blocker.kind = BlockerKind::Service;
    } else {
        blocker.kind = BlockerKind::App;
    }

    if (blocker.kind == BlockerKind::App && blocker.alive) {
        blocker.hasWindows = !GetProcessTopLevelWindows(blocker.pid).empty();

        HANDLE terminate = OpenProcess(PROCESS_TERMINATE, FALSE, blocker.pid);
        if (terminate) {
            blocker.canTerminate = true;
            CloseHandle(terminate);
        } else {
            blocker.needsElevation = GetLastError() == ERROR_ACCESS_DENIED;
        }
    }

    blocker.hint = GetBlockerHint(blocker, context);
}

int GetKindOrder(BlockerKind kind) {
    switch (kind) {
        case BlockerKind::App:
            return 0;
        case BlockerKind::Service:
            return 1;
        case BlockerKind::Explorer:
            return 2;
        case BlockerKind::Protected:
            return 3;
        case BlockerKind::System:
            return 4;
    }

    return 5;
}

////////////////////////////////////////////////////////////////////////////////
// Actions on blocking processes

struct ProcessTarget {
    DWORD pid = 0;
    std::wstring exeName;
    std::wstring displayName;
    ULONGLONG eventTime = 0;
};

ProcessTarget MakeProcessTarget(const Blocker& blocker) {
    return {blocker.pid, blocker.exeName, blocker.displayName,
            blocker.eventTime};
}

// Brings the program forward, so that a "save changes?" prompt is visible, and
// asks its windows to close.
void AskProcessToClose(DWORD pid) {
    std::vector<HWND> windows = GetProcessTopLevelWindows(pid);
    if (windows.empty()) {
        return;
    }

    AllowSetForegroundWindow(pid);

    HWND mainWindow = windows.front();
    if (IsIconic(mainWindow)) {
        ShowWindowAsync(mainWindow, SW_RESTORE);
    }

    SetForegroundWindow(mainWindow);

    for (HWND hwnd : windows) {
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
    }
}

bool TerminateTarget(const ProcessTarget& target, std::wstring& error) {
    UniqueHandle process(OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE |
                                         PROCESS_QUERY_LIMITED_INFORMATION,
                                     FALSE, target.pid));
    if (!process) {
        DWORD lastError = GetLastError();
        if (lastError == ERROR_INVALID_PARAMETER) {
            // Already gone.
            return true;
        }

        error = L"Couldn't end " + target.displayName + L" (error " +
                std::to_wstring(lastError) + L").";
        return false;
    }

    if (!ProcessMatches(process.Get(), target.exeName, target.eventTime,
                        nullptr) ||
        WaitForSingleObject(process.Get(), 0) == WAIT_OBJECT_0) {
        return true;
    }

    if (!TerminateProcess(process.Get(), 1)) {
        error = L"Couldn't end " + target.displayName + L" (error " +
                std::to_wstring(GetLastError()) + L").";
        return false;
    }

    Wh_Log(L"Ended %s (PID %u)", target.exeName.c_str(), target.pid);
    WaitUnlessUnloading(process.Get(), 5000);
    return true;
}

void WaitForTargetExit(const ProcessTarget& target, DWORD timeoutMs) {
    UniqueHandle process(OpenProcess(
        SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, target.pid));
    if (process &&
        ProcessMatches(process.Get(), target.exeName, target.eventTime,
                       nullptr)) {
        WaitUnlessUnloading(process.Get(), timeoutMs);
    }
}

std::wstring BuildTaskKillParameters(const ProcessTarget& target, bool force) {
    // The image name filter makes sure that the process ID wasn't reused while
    // the UAC prompt was open.
    std::wstring parameters =
        L"/FI \"PID eq " + std::to_wstring(target.pid) + L"\"";
    if (!target.exeName.empty()) {
        parameters += L" /FI \"IMAGENAME eq " + target.exeName + L"\"";
    }

    if (force) {
        parameters += L" /F";
    }

    return parameters;
}

// Runs a System32 program as administrator with a UAC prompt, hidden, and
// waits for it to finish.
bool RunElevated(PCWSTR program,
                 const std::wstring& parameters,
                 HWND owner,
                 DWORD timeoutMs,
                 std::wstring& error) {
    WCHAR systemDirectory[MAX_PATH] = {};
    UINT length = GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
    if (!length || length >= ARRAYSIZE(systemDirectory)) {
        error = L"Couldn't find the Windows system folder.";
        return false;
    }

    std::wstring file = std::wstring(systemDirectory) + L"\\" + program;

    HRESULT comInit = CoInitializeEx(
        nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask =
        SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    info.hwnd = owner;
    info.lpVerb = L"runas";
    info.lpFile = file.c_str();
    info.lpParameters = parameters.c_str();
    info.nShow = SW_HIDE;

    BOOL succeeded = ShellExecuteExW(&info);
    DWORD lastError = succeeded ? ERROR_SUCCESS : GetLastError();

    if (SUCCEEDED(comInit)) {
        CoUninitialize();
    }

    if (!succeeded) {
        if (lastError == ERROR_CANCELLED) {
            error = L"Administrator permission wasn't given, so nothing was "
                    L"changed.";
        } else {
            error = std::wstring(L"Couldn't run ") + program +
                    L" as administrator (error " +
                    std::to_wstring(lastError) + L").";
        }

        return false;
    }

    Wh_Log(L"Ran %s %s", program, parameters.c_str());

    if (info.hProcess) {
        WaitUnlessUnloading(info.hProcess, timeoutMs);
        CloseHandle(info.hProcess);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// The dialog

enum class Page {
    Blockers,
    Working,
    ConfirmForce,
};

enum class WorkOp {
    Eject,
    Terminate,
    ElevatedTaskKill,
    ForceDismount,
};

struct WorkRequest {
    WorkOp op = WorkOp::Eject;
    std::vector<ProcessTarget> targets;
    // For ElevatedTaskKill: end the process instead of asking it to close.
    bool force = true;
    std::vector<std::wstring> dismountTargets;
};

struct WorkResult {
    bool ejectAttempted = false;
    CONFIGRET result = CR_FAILURE;
    PNP_VETO_TYPE vetoType = PNP_VetoTypeUnknown;
    std::wstring vetoName;
    UniqueEvtHandle bookmark;
    ULONGLONG attemptFileTime = 0;
    ULONGLONG attemptTick = 0;
    std::vector<VetoEvent> events;
    std::wstring message;
    std::vector<DWORD> closeRequestedPids;
};

struct PageData {
    std::wstring instruction;
    std::wstring content;
    std::wstring expanded;
    std::wstring verification;
    std::vector<std::pair<int, std::wstring>> buttonDefinitions;
    std::vector<TASKDIALOG_BUTTON> buttons;
    std::vector<int> elevatedButtons;
    TASKDIALOGCONFIG config{};
};

struct EjectSession {
    // Set before the dialog is shown, then not changed.
    DEVINST devInst = 0;
    ULONG ejectFlags = 0;
    std::wstring deviceInstanceId;
    std::wstring deviceName;
    std::wstring windowTitle;
    Settings settings;
    HWND owner = nullptr;

    std::vector<VolumeInfo> volumes;

    // The latest eject attempt.
    CONFIGRET lastResult = CR_REMOVE_VETOED;
    PNP_VETO_TYPE lastVetoType = PNP_VetoTypeUnknown;
    std::wstring lastVetoName;
    UniqueEvtHandle bookmark;
    ULONGLONG attemptFileTime = 0;
    ULONGLONG attemptTick = 0;

    std::vector<Blocker> blockers;
    std::wstring status;

    std::atomic<HWND> hwnd{nullptr};
    Page page = Page::Blockers;
    // The width of the first page, in dialog units. Later pages use it too, so
    // that the dialog doesn't change its width when it navigates.
    UINT widthDlu = 0;
    bool autoEject = true;
    bool expanded = false;
    bool succeeded = false;
    bool closing = false;
    ULONGLONG lastEventPollTick = 0;
    ULONGLONG lastAutoEjectTick = 0;
    // A program that was asked to close has exited, so the eject is retried.
    bool ejectWhenReady = false;
    int automaticAttempts = 0;
    bool automaticAttemptsExhausted = false;

    // An action that runs on a worker thread.
    HANDLE worker = nullptr;
    WorkRequest request;
    WorkResult workResult;
    std::wstring workingInstruction;
    std::wstring workingContent;

    // Two buffers, so that the strings of the displayed page stay valid while
    // the dialog navigates to the next one.
    PageData pages[2];
    int currentPage = 0;
};

using CM_Request_Device_Eject_ExW_t = CONFIGRET(WINAPI*)(DEVINST,
                                                         PPNP_VETO_TYPE,
                                                         LPWSTR,
                                                         ULONG,
                                                         ULONG,
                                                         HMACHINE);
CM_Request_Device_Eject_ExW_t CM_Request_Device_Eject_ExW_Original;

HRESULT CALLBACK TaskDialogCallback(HWND hwnd,
                                    UINT notification,
                                    WPARAM wParam,
                                    LPARAM lParam,
                                    LONG_PTR data);
DWORD WINAPI WorkerThreadProc(LPVOID parameter);

bool IsTrackableBlocker(const Blocker& blocker) {
    return blocker.trackable && (blocker.kind == BlockerKind::App ||
                                 blocker.kind == BlockerKind::Service);
}

bool CanActOnBlocker(const EjectSession& session, const Blocker& blocker) {
    return session.settings.programButtons && blocker.alive &&
           blocker.kind == BlockerKind::App &&
           (blocker.canTerminate ||
            (blocker.needsElevation && session.settings.elevatedEndTask));
}

void MarkVolumeInUse(EjectSession& session, const std::wstring& instanceId) {
    if (!IsVolumeInstanceId(instanceId)) {
        return;
    }

    for (auto& volume : session.volumes) {
        if (EqualsIgnoreCase(volume.instanceId, instanceId)) {
            volume.inUse = true;
            return;
        }
    }

    VolumeInfo volume = DescribeVolume(instanceId);
    volume.inUse = true;
    session.volumes.push_back(std::move(volume));
}

// The volumes that are in use, or all volumes of the device if that isn't
// known.
std::vector<const VolumeInfo*> GetRelevantVolumes(const EjectSession& session) {
    std::vector<const VolumeInfo*> inUse;
    std::vector<const VolumeInfo*> all;
    for (const auto& volume : session.volumes) {
        if (GetVolumeDisplayName(volume).empty()) {
            continue;
        }

        all.push_back(&volume);
        if (volume.inUse) {
            inUse.push_back(&volume);
        }
    }

    return inUse.empty() ? all : inUse;
}

std::wstring GetTargetName(const EjectSession& session) {
    std::vector<std::wstring> names;
    for (const VolumeInfo* volume : GetRelevantVolumes(session)) {
        names.push_back(GetVolumeDisplayName(*volume));
    }

    return names.empty() ? session.deviceName : JoinWithAnd(names);
}

std::vector<std::wstring> GetDismountTargets(const EjectSession& session) {
    std::vector<std::wstring> targets;
    for (const auto& volume : session.volumes) {
        AddUniqueIgnoreCase(targets, GetVolumeDismountTarget(volume));
    }

    return targets;
}

DescribeContext MakeDescribeContext(const EjectSession& session) {
    DescribeContext context;
    context.sessionKnown =
        ProcessIdToSessionId(GetCurrentProcessId(), &context.sessionId) !=
        FALSE;
    context.services = GetRunningServicesByProcess();

    context.wslPath = L"/mnt/<drive>";
    for (const VolumeInfo* volume : GetRelevantVolumes(session)) {
        std::wstring letter = GetVolumeLetter(*volume);
        if (!letter.empty()) {
            context.wslPath = L"/mnt/";
            context.wslPath += static_cast<WCHAR>(towlower(letter[0]));
            break;
        }
    }

    return context;
}

// Rebuilds the list of blocking processes from the events of the latest eject
// attempt. Processes that were already listed keep their state.
void ReplaceBlockers(EjectSession& session,
                     const std::vector<VetoEvent>& events) {
    if (events.empty()) {
        // Keep what's known until the events of this attempt show up.
        return;
    }

    std::vector<Blocker> previous = std::move(session.blockers);
    session.blockers.clear();

    std::optional<DescribeContext> context;
    for (const auto& event : events) {
        auto existing = std::find_if(
            session.blockers.begin(), session.blockers.end(),
            [&](const Blocker& blocker) {
                return blocker.pid == event.processId;
            });
        if (existing != session.blockers.end()) {
            existing->eventTime = std::max(existing->eventTime, event.time);
            for (const auto& device : event.vetoingDevices) {
                AddUniqueIgnoreCase(existing->vetoingDevices, device);
            }
            continue;
        }

        auto old = std::find_if(
            previous.begin(), previous.end(), [&](const Blocker& blocker) {
                return blocker.pid == event.processId && blocker.alive &&
                       EqualsIgnoreCase(blocker.ntImagePath,
                                        event.processName);
            });
        if (old != previous.end()) {
            Blocker blocker = std::move(*old);
            blocker.eventTime = std::max(blocker.eventTime, event.time);
            for (const auto& device : event.vetoingDevices) {
                AddUniqueIgnoreCase(blocker.vetoingDevices, device);
            }
            session.blockers.push_back(std::move(blocker));
            continue;
        }

        if (!context) {
            context = MakeDescribeContext(session);
        }

        Blocker blocker;
        blocker.pid = event.processId;
        blocker.eventTime = event.time;
        blocker.ntImagePath = event.processName;
        blocker.commandLine = event.commandLine;
        blocker.vetoingDevices = event.vetoingDevices;
        DescribeBlocker(blocker, *context);

        Wh_Log(L"Blocked by %s (PID %u, %s)", blocker.displayName.c_str(),
               blocker.pid, blocker.ntImagePath.c_str());

        session.blockers.push_back(std::move(blocker));
    }

    std::stable_sort(session.blockers.begin(), session.blockers.end(),
                     [](const Blocker& a, const Blocker& b) {
                         return GetKindOrder(a.kind) < GetKindOrder(b.kind);
                     });

    for (const auto& blocker : session.blockers) {
        for (const auto& device : blocker.vetoingDevices) {
            MarkVolumeInUse(session, device);
        }
    }
}

bool HasUnlistedProcesses(const EjectSession& session,
                          const std::vector<VetoEvent>& events) {
    for (const auto& event : events) {
        bool listed = std::any_of(
            session.blockers.begin(), session.blockers.end(),
            [&](const Blocker& blocker) {
                return blocker.pid == event.processId;
            });
        if (!listed) {
            return true;
        }
    }

    return false;
}

std::wstring GetVetoReasonText(PNP_VETO_TYPE vetoType,
                               const std::wstring& vetoName) {
    switch (vetoType) {
        case PNP_VetoWindowsApp:
            return vetoName.empty()
                       ? L"A program is preventing the device from being "
                         L"stopped."
                       : L"The program \"" + vetoName +
                             L"\" is preventing the device from being stopped.";
        case PNP_VetoWindowsService:
            return L"The service \"" + vetoName +
                   L"\" is preventing the device from being stopped.";
        case PNP_VetoDriver:
            return L"The driver \"" + vetoName +
                   L"\" is preventing the device from being stopped right now.";
        case PNP_VetoPendingClose:
            return L"Part of the device was removed while a program was still "
                   L"using it.";
        case PNP_VetoIllegalDeviceRequest:
            return L"The device can't be ejected.";
        case PNP_VetoInsufficientRights:
            return L"You don't have permission to eject the device.";
        default:
            return L"The device is currently in use.";
    }
}

PCWSTR GetVetoTypeName(PNP_VETO_TYPE vetoType) {
    switch (vetoType) {
        case PNP_VetoTypeUnknown:
            return L"PNP_VetoTypeUnknown";
        case PNP_VetoLegacyDevice:
            return L"PNP_VetoLegacyDevice";
        case PNP_VetoPendingClose:
            return L"PNP_VetoPendingClose";
        case PNP_VetoWindowsApp:
            return L"PNP_VetoWindowsApp";
        case PNP_VetoWindowsService:
            return L"PNP_VetoWindowsService";
        case PNP_VetoOutstandingOpen:
            return L"PNP_VetoOutstandingOpen";
        case PNP_VetoDevice:
            return L"PNP_VetoDevice";
        case PNP_VetoDriver:
            return L"PNP_VetoDriver";
        case PNP_VetoIllegalDeviceRequest:
            return L"PNP_VetoIllegalDeviceRequest";
        case PNP_VetoInsufficientPower:
            return L"PNP_VetoInsufficientPower";
        case PNP_VetoNonDisableable:
            return L"PNP_VetoNonDisableable";
        case PNP_VetoLegacyDriver:
            return L"PNP_VetoLegacyDriver";
        case PNP_VetoInsufficientRights:
            return L"PNP_VetoInsufficientRights";
        case PNP_VetoAlreadyRemoved:
            return L"PNP_VetoAlreadyRemoved";
    }

    return L"unknown";
}

std::wstring BuildMainInstruction(const EjectSession& session) {
    std::vector<std::wstring> names;
    for (const VolumeInfo* volume : GetRelevantVolumes(session)) {
        names.push_back(GetVolumeDisplayName(*volume));
    }

    if (names.empty()) {
        return L"The device is in use";
    }

    return JoinWithAnd(names) + (names.size() == 1 ? L" is in use"
                                                    : L" are in use");
}

std::wstring BuildBlockersContent(const EjectSession& session) {
    std::vector<const Blocker*> running;
    for (const auto& blocker : session.blockers) {
        if (blocker.alive) {
            running.push_back(&blocker);
        }
    }

    std::wstring text;
    if (!running.empty()) {
        text = L"It's being used by:";

        int number = 0;
        for (const Blocker* blocker : running) {
            text += L"\n\n" + std::to_wstring(++number) + L". " +
                    blocker->displayName;
            if (blocker->kind == BlockerKind::System ||
                EqualsIgnoreCase(blocker->displayName, blocker->exeName)) {
                text += L" (PID " + std::to_wstring(blocker->pid) + L")";
            } else {
                text += L" \u2014 " + blocker->exeName + L", PID " +
                        std::to_wstring(blocker->pid);
            }

            if (blocker->closeRequested) {
                text += L" (asked to close)";
            }

            if (!blocker->hint.empty()) {
                // Indented to line up with the name after the number.
                text += L"\n    " + blocker->hint;
            }
        }
    } else if (!session.blockers.empty()) {
        text = L"The programs that were using the device have closed. Select "
               L"Retry to eject it.";
    } else {
        text = GetVetoReasonText(session.lastVetoType, session.lastVetoName) +
               L"\n\nWindows didn't report which program is using it. Close "
               L"programs that might have files open on the drive, and then "
               L"try again.";
    }

    if (!session.status.empty()) {
        text += L"\n\n" + session.status;
    }

    return text;
}

std::wstring DescribeVetoingDevice(const EjectSession& session,
                                   const std::wstring& instanceId) {
    for (const auto& volume : session.volumes) {
        if (EqualsIgnoreCase(volume.instanceId, instanceId)) {
            std::wstring name = GetVolumeDisplayName(volume);
            if (!name.empty()) {
                return name;
            }
        }
    }

    return instanceId;
}

std::wstring BuildDetails(const EjectSession& session) {
    std::wstring text = L"Device: " + session.deviceName;
    text += L"\nDevice instance ID: " + session.deviceInstanceId;
    text += L"\nReason: ";
    text += GetVetoTypeName(session.lastVetoType);
    if (!session.lastVetoName.empty()) {
        text += L" (" + session.lastVetoName + L")";
    }

    for (const auto& volume : session.volumes) {
        std::wstring name = GetVolumeDisplayName(volume);
        text += L"\nVolume: " + (name.empty() ? volume.instanceId : name);
        if (!volume.guidPath.empty()) {
            text += L" \u2014 " + volume.guidPath;
        }
        if (!name.empty()) {
            text += L"\n" + volume.instanceId;
        }
    }

    for (const auto& blocker : session.blockers) {
        text += L"\n\n" + blocker.displayName;
        if (!blocker.alive) {
            text += L" (closed)";
        }

        text += L"\nPID " + std::to_wstring(blocker.pid);
        if (blocker.kind != BlockerKind::System) {
            const std::wstring& path = !blocker.imagePath.empty()
                                           ? blocker.imagePath
                                           : blocker.ntImagePath;
            if (!path.empty()) {
                text += L" \u00B7 " + path;
            }
        }

        if (!blocker.commandLine.empty()) {
            text += L"\nCommand line: " + blocker.commandLine;
        }

        if (!blocker.services.empty()) {
            text += L"\nServices: " + blocker.services;
        }

        std::wstring note = GetBlockerDetailNote(blocker);
        if (!note.empty()) {
            text += L"\n" + note;
        }

        for (const auto& device : blocker.vetoingDevices) {
            text += L"\nUsing: " + DescribeVetoingDevice(session, device);
        }
    }

    text +=
        L"\n\nWindows reports these programs in the System event log "
        L"(Kernel-PnP, event ID 225).";
    return text;
}

std::wstring GetBlockerButtonText(const Blocker& blocker) {
    bool askToClose = blocker.hasWindows && !blocker.forceOffered;
    std::wstring text = (askToClose ? L"Close " : L"End ") + blocker.displayName;
    text += L"\n";
    if (!EqualsIgnoreCase(blocker.displayName, blocker.exeName)) {
        text += blocker.exeName + L" \u00B7 ";
    }
    text += L"PID " + std::to_wstring(blocker.pid) + L" \u00B7 ";

    if (askToClose) {
        text += L"It can still ask you to save your work.";
    } else if (blocker.closeRequested) {
        text += L"It didn't close. Unsaved work in it will be lost.";
    } else {
        text += L"Unsaved work in it will be lost.";
    }

    if (blocker.needsElevation) {
        text += L" Runs as administrator.";
    }

    return text;
}

void BuildBlockersPage(EjectSession& session, PageData& page) {
    TASKDIALOGCONFIG& config = page.config;
    page.instruction = BuildMainInstruction(session);
    page.content = BuildBlockersContent(session);
    page.expanded = BuildDetails(session);

    config.pszMainIcon = TD_WARNING_ICON;
    config.dwCommonButtons = TDCBF_RETRY_BUTTON | TDCBF_CANCEL_BUTTON;
    config.nDefaultButton = IDRETRY;

    int shownButtons = 0;
    int otherActionable = 0;
    for (size_t i = 0; i < session.blockers.size(); i++) {
        const Blocker& blocker = session.blockers[i];
        if (!CanActOnBlocker(session, blocker)) {
            continue;
        }

        if (shownButtons == kMaxBlockerButtons) {
            otherActionable++;
            continue;
        }

        int id = kBlockerButtonBase + static_cast<int>(i);
        page.buttonDefinitions.emplace_back(id, GetBlockerButtonText(blocker));
        if (blocker.needsElevation) {
            page.elevatedButtons.push_back(id);
        }
        shownButtons++;
    }

    if (otherActionable > 0) {
        page.buttonDefinitions.emplace_back(
            kOtherBlockersButton,
            L"Close or end the other " + std::to_wstring(otherActionable) +
                L" programs\nPrograms with windows are asked to close, and the "
                L"others are ended. Programs that run as administrator are "
                L"skipped.");
    }

    std::vector<std::wstring> dismountTargets = GetDismountTargets(session);
    if (session.settings.forceEject && !dismountTargets.empty()) {
        page.buttonDefinitions.emplace_back(
            kForceEjectButton,
            L"Force eject\nFlush and dismount " +
                JoinWithAnd(dismountTargets) +
                L" so that programs lose access to it, and then eject the "
                L"device. Needs administrator permission.");
        page.elevatedButtons.push_back(kForceEjectButton);
    }

    if (!page.buttonDefinitions.empty()) {
        config.dwFlags |= TDF_USE_COMMAND_LINKS;
    }

    if (std::any_of(session.blockers.begin(), session.blockers.end(),
                    IsTrackableBlocker)) {
        page.verification = L"Eject automatically when these programs close";
        if (session.autoEject) {
            config.dwFlags |= TDF_VERIFICATION_FLAG_CHECKED;
        }
    }

    // The details are shown below the buttons, so that the buttons don't move.
    config.dwFlags |= TDF_EXPAND_FOOTER_AREA;
    config.pszCollapsedControlText = L"Show details";
    config.pszExpandedControlText = L"Hide details";
    if (session.expanded) {
        config.dwFlags |= TDF_EXPANDED_BY_DEFAULT;
    }
}

void BuildWorkingPage(EjectSession& session, PageData& page) {
    page.instruction = session.workingInstruction;
    page.content = session.workingContent;
    page.config.dwFlags |= TDF_SHOW_MARQUEE_PROGRESS_BAR;
    page.config.dwCommonButtons = TDCBF_CANCEL_BUTTON;
}

void BuildConfirmForcePage(EjectSession& session, PageData& page) {
    std::vector<std::wstring> names;
    for (const auto& volume : session.volumes) {
        std::wstring name = GetVolumeDisplayName(volume);
        if (!name.empty()) {
            names.push_back(name);
        }
    }

    page.instruction =
        L"Force eject " +
        (names.empty() ? std::wstring(L"the device") : JoinWithAnd(names)) +
        L"?";
    page.content =
        L"Windows will flush and dismount the drive so that every program "
        L"loses access to it immediately, and then eject the device.\n\n"
        L"Programs that still have files open on the drive can lose unsaved "
        L"changes, and a file that's being written can be damaged. Use this "
        L"only if you can't close the programs.";
    page.config.pszMainIcon = TD_WARNING_ICON;
    page.buttonDefinitions.emplace_back(kConfirmForceButton, L"Force eject");
    page.buttonDefinitions.emplace_back(kBackButton, L"Back");
    page.elevatedButtons.push_back(kConfirmForceButton);
    page.config.nDefaultButton = kBackButton;
}

void BuildPage(EjectSession& session, PageData& page, Page kind) {
    page = PageData{};

    switch (kind) {
        case Page::Blockers:
            BuildBlockersPage(session, page);
            break;
        case Page::Working:
            BuildWorkingPage(session, page);
            break;
        case Page::ConfirmForce:
            BuildConfirmForcePage(session, page);
            break;
    }

    // The strings are complete, so pointers to them stay valid.
    for (const auto& [id, text] : page.buttonDefinitions) {
        page.buttons.push_back({id, text.c_str()});
    }

    TASKDIALOGCONFIG& config = page.config;
    config.cbSize = sizeof(config);
    config.hwndParent = session.owner;
    config.pszWindowTitle = session.windowTitle.c_str();
    config.dwFlags |= TDF_ALLOW_DIALOG_CANCELLATION | TDF_CALLBACK_TIMER;
    config.dwFlags |= session.owner ? TDF_POSITION_RELATIVE_TO_WINDOW
                                    : TDF_CAN_BE_MINIMIZED;
    config.pszMainInstruction =
        page.instruction.empty() ? nullptr : page.instruction.c_str();
    config.pszContent = page.content.empty() ? nullptr : page.content.c_str();
    config.pszExpandedInformation =
        page.expanded.empty() ? nullptr : page.expanded.c_str();
    config.pszVerificationText =
        page.verification.empty() ? nullptr : page.verification.c_str();
    config.cButtons = static_cast<UINT>(page.buttons.size());
    config.pButtons = page.buttons.empty() ? nullptr : page.buttons.data();
    config.pfCallback = TaskDialogCallback;
    config.lpCallbackData = reinterpret_cast<LONG_PTR>(&session);
    config.cxWidth = session.widthDlu;
}

void RememberDialogWidth(EjectSession& session, HWND hwnd) {
    if (session.widthDlu) {
        return;
    }

    RECT client{};
    RECT units = {0, 0, 4, 8};
    if (!GetClientRect(hwnd, &client) || !MapDialogRect(hwnd, &units) ||
        units.right <= 0) {
        return;
    }

    session.widthDlu = static_cast<UINT>(
        MulDiv(client.right - client.left, 4, units.right));
}

void NavigateTo(EjectSession& session, Page page) {
    int next = session.currentPage ^ 1;
    session.page = page;
    BuildPage(session, session.pages[next], page);
    session.currentPage = next;

    if (HWND hwnd = session.hwnd.load()) {
        SendMessageW(hwnd, TDM_NAVIGATE_PAGE, 0,
                     reinterpret_cast<LPARAM>(&session.pages[next].config));
    }
}

void ApplyPageState(EjectSession& session, HWND hwnd) {
    const PageData& page = session.pages[session.currentPage];
    for (int id : page.elevatedButtons) {
        SendMessageW(hwnd, TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE, id, TRUE);
    }

    if (session.page == Page::Working) {
        SendMessageW(hwnd, TDM_SET_PROGRESS_BAR_MARQUEE, TRUE, 0);
        SendMessageW(hwnd, TDM_ENABLE_BUTTON, IDCANCEL, FALSE);
    }
}

void RequestClose(EjectSession& session) {
    session.closing = true;
    if (HWND hwnd = session.hwnd.load()) {
        SendMessageW(hwnd, TDM_ENABLE_BUTTON, IDCANCEL, TRUE);
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
    }
}

void StartWork(EjectSession& session,
               WorkRequest request,
               std::wstring instruction,
               std::wstring content) {
    session.request = std::move(request);
    session.workResult = WorkResult{};
    session.status.clear();
    session.workingInstruction = std::move(instruction);
    session.workingContent = std::move(content);

    // The dialog doesn't close while the worker runs, and the mod waits for
    // open dialogs before it unloads, so the worker never outlives the mod.
    HANDLE thread =
        CreateThread(nullptr, 0, WorkerThreadProc, &session, 0, nullptr);
    if (!thread) {
        session.status = L"Couldn't start the action (error " +
                         std::to_wstring(GetLastError()) + L").";
        NavigateTo(session, Page::Blockers);
        return;
    }

    session.worker = thread;
    NavigateTo(session, Page::Working);
}

void StartEject(EjectSession& session, bool automatic) {
    session.ejectWhenReady = false;

    // Programs that have exited don't block the device anymore.
    std::erase_if(session.blockers,
                  [](const Blocker& blocker) { return !blocker.alive; });

    StartWork(session, WorkRequest{},
              L"Ejecting " + GetTargetName(session) + L"\u2026",
              automatic ? L"A program that was using the drive has closed, so "
                          L"Windows is trying again."
                        : L"Asking Windows to eject the device.");
}

void StartForceEject(EjectSession& session) {
    WorkRequest request;
    request.op = WorkOp::ForceDismount;
    request.dismountTargets = GetDismountTargets(session);

    StartWork(session, std::move(request),
              L"Force ejecting " + GetTargetName(session) + L"\u2026",
              L"Approve the administrator prompt. The drive is flushed and "
              L"dismounted, and then the device is ejected.");
}

void ActOnBlocker(EjectSession& session, size_t index) {
    Blocker& blocker = session.blockers[index];
    if (!blocker.alive) {
        NavigateTo(session, Page::Blockers);
        return;
    }

    bool askToClose = blocker.hasWindows && !blocker.forceOffered;

    if (blocker.needsElevation) {
        WorkRequest request;
        request.op = WorkOp::ElevatedTaskKill;
        request.targets.push_back(MakeProcessTarget(blocker));
        request.force = !askToClose;
        StartWork(session, std::move(request),
                  (askToClose ? L"Closing " : L"Ending ") +
                      blocker.displayName + L"\u2026",
                  L"Approve the administrator prompt to continue.");
        return;
    }

    if (askToClose) {
        AskProcessToClose(blocker.pid);
        blocker.closeRequested = true;
        blocker.closeRequestedTick = GetTickCount64();
        session.status = L"Asked " + blocker.displayName +
                         L" to close. If it asks to save your work, answer "
                         L"it. The drive is ejected as soon as it exits.";
        NavigateTo(session, Page::Blockers);
        return;
    }

    WorkRequest request;
    request.op = WorkOp::Terminate;
    request.targets.push_back(MakeProcessTarget(blocker));
    StartWork(session, std::move(request),
              L"Ending " + blocker.displayName + L"\u2026",
              L"The device is ejected as soon as the program has closed.");
}

// Closes or ends the actionable programs that don't have their own button.
void ActOnOtherBlockers(EjectSession& session) {
    WorkRequest request;
    request.op = WorkOp::Terminate;

    int shown = 0;
    bool askedToClose = false;
    ULONGLONG now = GetTickCount64();
    for (auto& blocker : session.blockers) {
        if (!CanActOnBlocker(session, blocker)) {
            continue;
        }

        if (shown < kMaxBlockerButtons) {
            shown++;
            continue;
        }

        if (blocker.needsElevation) {
            continue;
        }

        if (blocker.hasWindows && !blocker.forceOffered) {
            AskProcessToClose(blocker.pid);
            blocker.closeRequested = true;
            blocker.closeRequestedTick = now;
            askedToClose = true;
        } else {
            request.targets.push_back(MakeProcessTarget(blocker));
        }
    }

    if (!request.targets.empty()) {
        StartWork(session, std::move(request), L"Ending programs\u2026",
                  L"The device is ejected as soon as the programs have "
                  L"closed.");
        return;
    }

    if (askedToClose) {
        session.status =
            L"Asked the programs to close. The drive is ejected as soon as "
            L"they exit.";
    }

    NavigateTo(session, Page::Blockers);
}

void RunWork(EjectSession& session) {
    const WorkRequest& request = session.request;
    WorkResult& result = session.workResult;

    switch (request.op) {
        case WorkOp::Eject:
            break;

        case WorkOp::Terminate: {
            std::vector<std::wstring> errors;
            for (const auto& target : request.targets) {
                std::wstring error;
                if (!TerminateTarget(target, error)) {
                    errors.push_back(std::move(error));
                }
            }

            if (!errors.empty()) {
                result.message = Join(errors, L"\n");
                if (errors.size() == request.targets.size()) {
                    return;
                }
            }
            break;
        }

        case WorkOp::ElevatedTaskKill: {
            const ProcessTarget& target = request.targets.front();
            if (!RunElevated(L"taskkill.exe",
                             BuildTaskKillParameters(target, request.force),
                             session.hwnd.load(), 15000, result.message)) {
                return;
            }

            if (!request.force) {
                result.closeRequestedPids.push_back(target.pid);
                result.message =
                    L"Asked " + target.displayName +
                    L" to close. If it asks to save your work, answer it. The "
                    L"drive is ejected as soon as it exits.";
                return;
            }

            WaitForTargetExit(target, 5000);
            break;
        }

        case WorkOp::ForceDismount: {
            std::wstring parameters = L"/d /c";
            for (size_t i = 0; i < request.dismountTargets.size(); i++) {
                std::wstring target = request.dismountTargets[i];
                if (target.find(L' ') != std::wstring::npos) {
                    target = L"\"" + target + L"\"";
                }

                if (i > 0) {
                    parameters += L" &";
                }

                parameters += L" fsutil volume flush " + target +
                              L" & fsutil volume dismount " + target;
            }

            if (!RunElevated(L"cmd.exe", parameters, session.hwnd.load(), 60000,
                             result.message)) {
                return;
            }
            break;
        }
    }

    if (g_unloading) {
        return;
    }

    result.bookmark = UniqueEvtHandle(CreateSystemLogBookmark());
    result.attemptFileTime = GetCurrentFileTime();
    result.attemptTick = GetTickCount64();

    PNP_VETO_TYPE vetoType = PNP_VetoTypeUnknown;
    WCHAR vetoName[MAX_PATH] = {};
    result.result = CM_Request_Device_Eject_ExW_Original(
        session.devInst, &vetoType, vetoName, ARRAYSIZE(vetoName),
        session.ejectFlags, nullptr);
    result.ejectAttempted = true;
    result.vetoType = vetoType;
    result.vetoName = vetoName;

    Wh_Log(L"Eject attempt: result 0x%X, veto %s (%s)", result.result,
           GetVetoTypeName(vetoType), vetoName);

    if (result.result == CR_REMOVE_VETOED) {
        result.events = WaitForVetoEvents(
            session.deviceInstanceId, result.bookmark.Get(),
            result.attemptFileTime, session.settings.reportWaitMs);
    }
}

DWORD WINAPI WorkerThreadProc(LPVOID parameter) {
    RunWork(*static_cast<EjectSession*>(parameter));
    return 0;
}

bool IsDeviceGone(CONFIGRET result, PNP_VETO_TYPE vetoType) {
    return result == CR_NO_SUCH_DEVNODE || result == CR_INVALID_DEVNODE ||
           (result == CR_REMOVE_VETOED && vetoType == PNP_VetoAlreadyRemoved);
}

void OnWorkFinished(EjectSession& session) {
    WorkResult result = std::move(session.workResult);
    session.workResult = WorkResult{};

    ULONGLONG now = GetTickCount64();
    for (DWORD pid : result.closeRequestedPids) {
        for (auto& blocker : session.blockers) {
            if (blocker.pid == pid) {
                blocker.closeRequested = true;
                blocker.closeRequestedTick = now;
            }
        }
    }

    if (!result.ejectAttempted) {
        session.status = std::move(result.message);
        NavigateTo(session, Page::Blockers);
        return;
    }

    session.lastResult = result.result;
    session.lastVetoType = result.vetoType;
    session.lastVetoName = result.vetoName;
    session.bookmark = std::move(result.bookmark);
    session.attemptFileTime = result.attemptFileTime;
    session.attemptTick = result.attemptTick;
    session.lastEventPollTick = now;

    if (result.result == CR_SUCCESS) {
        session.succeeded = true;
        RequestClose(session);
        return;
    }

    if (IsDeviceGone(result.result, result.vetoType)) {
        RequestClose(session);
        return;
    }

    if (result.result != CR_REMOVE_VETOED) {
        session.status = L"Windows couldn't eject the device (error 0x" +
                         ToHex(result.result, 8) + L").";
        NavigateTo(session, Page::Blockers);
        return;
    }

    MarkVolumeInUse(session, result.vetoName);
    ReplaceBlockers(session, result.events);

    std::wstring stillInUse = L"The device is still in use.";
    session.status = result.message.empty()
                         ? stillInUse
                         : result.message + L"\n" + stillInUse;
    NavigateTo(session, Page::Blockers);
}

// Returns false if the worker is still running.
bool CollectWorker(EjectSession& session) {
    if (!session.worker) {
        return true;
    }

    if (WaitForSingleObject(session.worker, 0) != WAIT_OBJECT_0) {
        return false;
    }

    CloseHandle(session.worker);
    session.worker = nullptr;

    if (g_unloading) {
        RequestClose(session);
    } else {
        OnWorkFinished(session);
    }

    return true;
}

HRESULT OnButtonClicked(EjectSession& session, int id) {
    if (session.worker) {
        if (!CollectWorker(session)) {
            return S_FALSE;
        }

        // The finished action decided what happens next.
        return session.closing ? S_OK : S_FALSE;
    }

    if (session.closing || g_unloading) {
        return S_OK;
    }

    // The user is back in control, so automatic attempts may continue.
    session.automaticAttempts = 0;
    session.automaticAttemptsExhausted = false;

    switch (id) {
        case IDCANCEL:
            if (session.page == Page::ConfirmForce) {
                NavigateTo(session, Page::Blockers);
                return S_FALSE;
            }
            return S_OK;

        case IDRETRY:
            StartEject(session, false);
            return S_FALSE;

        case kForceEjectButton:
            NavigateTo(session, Page::ConfirmForce);
            return S_FALSE;

        case kConfirmForceButton:
            StartForceEject(session);
            return S_FALSE;

        case kBackButton:
            NavigateTo(session, Page::Blockers);
            return S_FALSE;

        case kOtherBlockersButton:
            ActOnOtherBlockers(session);
            return S_FALSE;
    }

    if (id >= kBlockerButtonBase &&
        id < kBlockerButtonBase + static_cast<int>(session.blockers.size())) {
        ActOnBlocker(session, id - kBlockerButtonBase);
    }

    return S_FALSE;
}

void OnTimer(EjectSession& session) {
    if (session.worker) {
        CollectWorker(session);
        return;
    }

    if (session.closing || g_unloading) {
        RequestClose(session);
        return;
    }

    if (session.page != Page::Blockers) {
        return;
    }

    ULONGLONG now = GetTickCount64();
    bool changed = false;

    // Pick up events of the latest attempt that were logged late.
    ULONGLONG pollInterval = session.bookmark ? 700 : 2500;
    if (now - session.attemptTick < kEventPollWindowMs &&
        now - session.lastEventPollTick >= pollInterval) {
        session.lastEventPollTick = now;
        std::vector<VetoEvent> events =
            QueryVetoEvents(session.deviceInstanceId, session.bookmark.Get(),
                            session.attemptFileTime);
        if (HasUnlistedProcesses(session, events)) {
            ReplaceBlockers(session, events);
            changed = true;
        }
    }

    for (auto& blocker : session.blockers) {
        if (blocker.alive && blocker.process &&
            WaitForSingleObject(blocker.process.Get(), 0) == WAIT_OBJECT_0) {
            Wh_Log(L"%s (PID %u) exited", blocker.exeName.c_str(),
                   blocker.pid);
            blocker.alive = false;
            changed = true;
            if (blocker.closeRequested) {
                // The user asked for this program to close, so retry even if
                // automatic ejecting is off.
                session.ejectWhenReady = true;
            }
        }
    }

    bool allTrackableExited =
        std::any_of(session.blockers.begin(), session.blockers.end(),
                    IsTrackableBlocker) &&
        std::none_of(session.blockers.begin(), session.blockers.end(),
                     [](const Blocker& blocker) {
                         return IsTrackableBlocker(blocker) && blocker.alive;
                     });

    if (session.ejectWhenReady || (session.autoEject && allTrackableExited)) {
        if (session.automaticAttempts >= kMaxAutomaticAttempts) {
            if (!session.automaticAttemptsExhausted) {
                session.automaticAttemptsExhausted = true;
                session.ejectWhenReady = false;
                session.status =
                    L"Stopped retrying automatically because the drive keeps "
                    L"being used. Select Retry to try again.";
                changed = true;
            }
        } else if (now - session.lastAutoEjectTick >= kAutoEjectIntervalMs) {
            session.lastAutoEjectTick = now;
            session.automaticAttempts++;
            StartEject(session, true);
            return;
        }
    }

    // Offer "End" for programs that didn't close when asked.
    for (auto& blocker : session.blockers) {
        if (blocker.alive && blocker.closeRequested && !blocker.forceOffered &&
            now - blocker.closeRequestedTick >= kCloseGraceMs) {
            blocker.forceOffered = true;
            changed = true;
        }
    }

    if (changed) {
        NavigateTo(session, Page::Blockers);
    }
}

HRESULT CALLBACK TaskDialogCallback(HWND hwnd,
                                    UINT notification,
                                    WPARAM wParam,
                                    LPARAM lParam,
                                    LONG_PTR data) {
    auto& session = *reinterpret_cast<EjectSession*>(data);

    switch (notification) {
        case TDN_DIALOG_CONSTRUCTED:
        case TDN_NAVIGATED:
            ApplyPageState(session, hwnd);
            break;

        case TDN_CREATED:
            session.hwnd = hwnd;
            RememberDialogWidth(session, hwnd);
            ApplyPageState(session, hwnd);
            SetForegroundWindow(hwnd);
            break;

        case TDN_BUTTON_CLICKED:
            return OnButtonClicked(session, static_cast<int>(wParam));

        case TDN_VERIFICATION_CLICKED:
            session.autoEject = wParam != 0;
            break;

        case TDN_EXPANDO_BUTTON_CLICKED:
            session.expanded = wParam != 0;
            break;

        case TDN_TIMER:
            OnTimer(session);
            break;

        case TDN_DESTROYED:
            session.hwnd = nullptr;
            break;
    }

    return S_OK;
}

using TaskDialogIndirect_t = HRESULT(WINAPI*)(const TASKDIALOGCONFIG*,
                                              int*,
                                              int*,
                                              BOOL*);

TaskDialogIndirect_t GetTaskDialogIndirect() {
    // Explorer always has comctl32 loaded. Without it, the original message
    // box is shown.
    HMODULE comctl32 = GetModuleHandleW(L"comctl32.dll");
    return comctl32 ? reinterpret_cast<TaskDialogIndirect_t>(
                          GetProcAddress(comctl32, "TaskDialogIndirect"))
                    : nullptr;
}

// When the eject runs on a thread with an active window (for example "Eject"
// in a File Explorer window), the dialog is owned by that window, like the
// original message box. The taskbar and the desktop aren't used as owners.
HWND GetDialogOwner() {
    HWND active = GetActiveWindow();
    if (!active || !IsWindowVisible(active)) {
        return nullptr;
    }

    HWND root = GetAncestor(active, GA_ROOT);
    if (!root) {
        root = active;
    }

    WCHAR className[64] = {};
    GetClassNameW(root, className, ARRAYSIZE(className));
    if (IsOneOfIgnoreCase(className, {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
                                      L"Progman", L"WorkerW"})) {
        return nullptr;
    }

    return root;
}

// Shows the dialog and returns when it's closed. Returns false if it couldn't
// be shown.
bool RunEjectDialog(EjectSession& session) {
    TaskDialogIndirect_t taskDialogIndirect = GetTaskDialogIndirect();
    if (!taskDialogIndirect) {
        Wh_Log(L"TaskDialogIndirect isn't available");
        return false;
    }

    session.page = Page::Blockers;
    session.currentPage = 0;
    BuildPage(session, session.pages[0], Page::Blockers);

    int button = 0;
    HRESULT hr =
        taskDialogIndirect(&session.pages[0].config, &button, nullptr, nullptr);

    if (session.worker) {
        // Not expected, the dialog only closes when no action is running.
        WaitForSingleObject(session.worker, INFINITE);
        CloseHandle(session.worker);
        session.worker = nullptr;
    }

    if (FAILED(hr)) {
        Wh_Log(L"TaskDialogIndirect failed: 0x%08X", hr);
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Open dialogs and skipped message boxes

std::mutex g_sessionsMutex;
std::vector<EjectSession*> g_sessions;

// Returns false if a dialog for the same device is already open, after
// bringing that dialog to the front.
bool RegisterSession(EjectSession* session) {
    HWND existingDialog = nullptr;

    {
        std::lock_guard lock(g_sessionsMutex);
        auto it = std::find_if(
            g_sessions.begin(), g_sessions.end(), [&](EjectSession* other) {
                return EqualsIgnoreCase(other->deviceInstanceId,
                                        session->deviceInstanceId);
            });
        if (it == g_sessions.end()) {
            g_sessions.push_back(session);
            return true;
        }

        existingDialog = (*it)->hwnd.load();
    }

    if (existingDialog) {
        if (IsIconic(existingDialog)) {
            ShowWindowAsync(existingDialog, SW_RESTORE);
        }
        SetForegroundWindow(existingDialog);
    }

    return false;
}

void UnregisterSession(EjectSession* session) {
    std::lock_guard lock(g_sessionsMutex);
    std::erase(g_sessions, session);
}

struct Suppression {
    DWORD threadId;
    ULONGLONG expiresAt;
};

std::mutex g_suppressionsMutex;
std::vector<Suppression> g_suppressions;
std::atomic<int> g_suppressionCount{0};

// The next message box that hotplug.dll shows on this thread is skipped.
void AddSuppression(DWORD threadId) {
    std::lock_guard lock(g_suppressionsMutex);
    g_suppressions.push_back({threadId, GetTickCount64() + kSuppressionLifetimeMs});
    g_suppressionCount = static_cast<int>(g_suppressions.size());
}

bool ConsumeSuppression(DWORD threadId) {
    std::lock_guard lock(g_suppressionsMutex);
    ULONGLONG now = GetTickCount64();
    std::erase_if(g_suppressions, [now](const Suppression& suppression) {
        return suppression.expiresAt <= now;
    });

    auto it = std::find_if(g_suppressions.begin(), g_suppressions.end(),
                           [threadId](const Suppression& suppression) {
                               return suppression.threadId == threadId;
                           });
    bool found = it != g_suppressions.end();
    if (found) {
        g_suppressions.erase(it);
    }

    g_suppressionCount = static_cast<int>(g_suppressions.size());
    return found;
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

bool IsInUseVeto(PNP_VETO_TYPE vetoType) {
    switch (vetoType) {
        case PNP_VetoTypeUnknown:
        case PNP_VetoPendingClose:
        case PNP_VetoWindowsApp:
        case PNP_VetoWindowsService:
        case PNP_VetoOutstandingOpen:
        case PNP_VetoDevice:
        case PNP_VetoDriver:
            return true;
        default:
            return false;
    }
}

CONFIGRET HandleVetoedEject(DEVINST devInst,
                            ULONG ejectFlags,
                            CONFIGRET result,
                            PNP_VETO_TYPE vetoType,
                            const std::wstring& vetoName,
                            UniqueEvtHandle bookmark,
                            ULONGLONG attemptFileTime,
                            ULONGLONG attemptTick,
                            PPNP_VETO_TYPE pVetoType,
                            LPWSTR pszVetoName,
                            ULONG ulNameLength) {
    auto session = std::make_unique<EjectSession>();
    session->devInst = devInst;
    session->ejectFlags = ejectFlags;
    session->deviceInstanceId = GetDeviceInstanceId(devInst);
    if (session->deviceInstanceId.empty()) {
        return result;
    }

    if (!RegisterSession(session.get())) {
        // A dialog for this device is already open.
        AddSuppression(GetCurrentThreadId());
        return result;
    }

    session->settings = GetSettings();
    session->autoEject = session->settings.autoEject;
    session->owner = GetDialogOwner();
    session->deviceName = GetDeviceName(devInst);
    session->windowTitle = L"Problem Ejecting " + session->deviceName;
    session->lastResult = result;
    session->lastVetoType = vetoType;
    session->lastVetoName = vetoName;
    session->bookmark = std::move(bookmark);
    session->attemptFileTime = attemptFileTime;
    session->attemptTick = attemptTick;

    for (const auto& volumeId : FindDeviceVolumeIds(devInst)) {
        session->volumes.push_back(DescribeVolume(volumeId));
    }

    MarkVolumeInUse(*session, vetoName);

    ReplaceBlockers(*session,
                    WaitForVetoEvents(session->deviceInstanceId,
                                      session->bookmark.Get(), attemptFileTime,
                                      session->settings.reportWaitMs));
    session->lastEventPollTick = GetTickCount64();

    bool shown = !g_unloading && RunEjectDialog(*session);
    UnregisterSession(session.get());

    if (!shown) {
        return result;
    }

    if (session->succeeded) {
        Wh_Log(L"Ejected %s", session->deviceInstanceId.c_str());
        if (pVetoType) {
            *pVetoType = PNP_VetoTypeUnknown;
        }
        if (pszVetoName && ulNameLength) {
            pszVetoName[0] = L'\0';
        }
        return CR_SUCCESS;
    }

    if (pVetoType) {
        *pVetoType = session->lastVetoType;
    }
    if (pszVetoName && ulNameLength) {
        lstrcpynW(pszVetoName, session->lastVetoName.c_str(),
                  static_cast<int>(ulNameLength));
    }

    AddSuppression(GetCurrentThreadId());
    return session->lastResult;
}

CONFIGRET WINAPI CM_Request_Device_Eject_ExW_Hook(DEVINST dnDevInst,
                                                  PPNP_VETO_TYPE pVetoType,
                                                  LPWSTR pszVetoName,
                                                  ULONG ulNameLength,
                                                  ULONG ulFlags,
                                                  HMACHINE hMachine) {
    void* returnAddress = __builtin_return_address(0);

    // Counted before g_unloading is checked, so that Wh_ModBeforeUninit can't
    // miss a call that has just passed the check.
    InFlightScope inFlight;

    if (hMachine || g_unloading || !IsCallerHotplug(returnAddress)) {
        return CM_Request_Device_Eject_ExW_Original(dnDevInst, pVetoType,
                                                    pszVetoName, ulNameLength,
                                                    ulFlags, hMachine);
    }

    // Events of this attempt come after the bookmark.
    UniqueEvtHandle bookmark(CreateSystemLogBookmark());
    ULONGLONG attemptFileTime = GetCurrentFileTime();
    ULONGLONG attemptTick = GetTickCount64();

    CONFIGRET result = CM_Request_Device_Eject_ExW_Original(
        dnDevInst, pVetoType, pszVetoName, ulNameLength, ulFlags, hMachine);
    if (result != CR_REMOVE_VETOED) {
        return result;
    }

    PNP_VETO_TYPE vetoType = pVetoType ? *pVetoType : PNP_VetoTypeUnknown;
    std::wstring vetoName;
    if (pszVetoName && ulNameLength) {
        vetoName.assign(pszVetoName, wcsnlen(pszVetoName, ulNameLength));
    }

    Wh_Log(L"Eject was vetoed: %s (%s)", GetVetoTypeName(vetoType),
           vetoName.c_str());

    if (!IsInUseVeto(vetoType) || g_unloading) {
        return result;
    }

    return HandleVetoedEject(dnDevInst, ulFlags, result, vetoType, vetoName,
                             std::move(bookmark), attemptFileTime, attemptTick,
                             pVetoType, pszVetoName, ulNameLength);
}

using MessageBoxW_t = decltype(&MessageBoxW);
MessageBoxW_t MessageBoxW_Original;

int WINAPI MessageBoxW_Hook(HWND hWnd,
                            LPCWSTR lpText,
                            LPCWSTR lpCaption,
                            UINT uType) {
    if (g_suppressionCount.load() > 0 &&
        IsCallerHotplug(__builtin_return_address(0)) &&
        ConsumeSuppression(GetCurrentThreadId())) {
        Wh_Log(L"Skipped hotplug.dll's message box: %s",
               lpCaption ? lpCaption : L"");
        return IDOK;
    }

    return MessageBoxW_Original(hWnd, lpText, lpCaption, uType);
}

////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE cfgmgr32 = GetModuleHandleW(L"cfgmgr32.dll");
    if (!cfgmgr32) {
        cfgmgr32 = LoadLibraryExW(L"cfgmgr32.dll", nullptr,
                                  LOAD_LIBRARY_SEARCH_SYSTEM32);
    }

    auto ejectFunction =
        cfgmgr32 ? reinterpret_cast<CM_Request_Device_Eject_ExW_t>(
                       GetProcAddress(cfgmgr32, "CM_Request_Device_Eject_ExW"))
                 : nullptr;
    if (!ejectFunction) {
        Wh_Log(L"CM_Request_Device_Eject_ExW wasn't found");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(ejectFunction,
                                        CM_Request_Device_Eject_ExW_Hook,
                                        &CM_Request_Device_Eject_ExW_Original)) {
        Wh_Log(L"Hooking CM_Request_Device_Eject_ExW failed");
        return FALSE;
    }

    auto messageBoxFunction = reinterpret_cast<MessageBoxW_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "MessageBoxW"));
    if (!messageBoxFunction ||
        !WindhawkUtils::SetFunctionHook(messageBoxFunction, MessageBoxW_Hook,
                                        &MessageBoxW_Original)) {
        Wh_Log(L"Hooking MessageBoxW failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    {
        std::lock_guard lock(g_sessionsMutex);
        for (EjectSession* session : g_sessions) {
            if (HWND hwnd = session->hwnd.load()) {
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
            }
        }
    }

    // The hooks are removed after this function returns, and the mod is
    // unloaded right after that. Wait for every hooked call to return. Open
    // dialogs close as soon as a running action finishes, and every action
    // stops its waits early while unloading, except for an open UAC prompt,
    // which ends when the user answers it.
    while (g_inFlight.load() > 0) {
        Sleep(50);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
