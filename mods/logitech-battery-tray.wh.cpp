// ==WindhawkMod==
// @id              logitech-battery-tray
// @name            Logitech Battery Tray
// @description     Native taskbar battery indicators for Logitech G HUB and HID++ devices.
// @description:ru  Индикаторы заряда устройств Logitech через G HUB и HID++ в панели задач.
// @version         0.1.1
// @author          uMk0
// @github          https://github.com/umk0
// @license         GPL-3.0-or-later
// @include         explorer.exe
// @architecture    x86-64
// @architecture    x86
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lcomctl32 -lwinhttp -lshell32 -lsetupapi -lhid -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Logitech Battery Tray

Battery indicators for supported Logitech devices, with automatic discovery,
charging status and optional percentage labels.

## Screenshots

Horizontal taskbar with a battery tooltip:

![Logitech battery indicators on a horizontal taskbar](https://raw.githubusercontent.com/umk0/windhawk-mods/a05686db7316fb9dc5520a769165e1341c0fa551/logitech-battery-tray/horizontal.png)

Vertical taskbar with stacked device indicators:

![Logitech battery indicators on a vertical taskbar](https://raw.githubusercontent.com/umk0/windhawk-mods/a05686db7316fb9dc5520a769165e1341c0fa551/logitech-battery-tray/vertical.png)

## Compatibility and scope

Windows 10/11: standard notification icons provide the compatibility backend.
On supported Windows 11 x64/ARM64 taskbars, Auto uses native XAML battery buttons
on the primary taskbar. Unrecognized taskbar layouts use notification icons
instead of an overlay. 32-bit Windows uses notification icons only.
Native icons show the percentage and a tooltip; charging is green. Windows
controls their placement and overflow visibility. XAML placement, button styling
and click-to-open settings do not apply to notification icons.

Select **Native notification icons** to test or force the compatibility backend.
Successful compilation is not a claim of runtime testing on every Windows build.

Unlike BT Battery Monitor (generic Bluetooth battery reporting), this mod reads
Logitech G HUB and Logitech HID++ battery data, including supported USB receivers.
It runs in Explorer because its optional XAML backend modifies Explorer's own
taskbar visual tree. No pairing, firmware or device configuration is changed.

## Battery sources

The XAML backend uses built-in Segoe Fluent Icons and standard Windows buttons
and tooltips. Its battery glyph changes with the reported charge level, with an
optional smaller icon indicating the device type.

G HUB must be running to use its data; direct HID++ reading works independently.
The G HUB client connects only to 127.0.0.1:9010. It requests /devices/list and
/battery/<deviceId>/state, subscribes to /battery/state/changed, and uses a
persistent query connection with a 30-second fallback poll by default. Failed
connections retry with exponential backoff, up to 60 seconds. Events update
readings independently of fallback polling.
Updates cannot arrive earlier than G HUB reports them.

The HID++ provider reads compatible Logitech devices through Unifying, Bolt,
LIGHTSPEED, USB and accessible Bluetooth HID++ interfaces. It supports battery
features 0x1004, 0x1000, 0x1001, 0x1F20 and legacy HID++ 1.0 registers.
Voltage-based and coarse readings are marked with an approximation sign.
G HUB readings take priority when available. Models using other protocols may
not report a battery level; support for every Logitech model is not guaranteed.

## Discovery and language

All detected, supported battery devices are shown by default. No model-specific
rules are shipped. Optional visibility rules match a device name or model;
empty rules are ignored, * and ? are supported, and the last match wins.
The user-facing tooltip contains no internal IDs or provider diagnostics.
Tooltip translations follow the preferred Windows UI languages, with English
as fallback. Reload the mod after changing Windows display languages.
The mod description and settings default to English, with Russian localization
for Windhawk's Russian interface. Tooltips follow Windows UI languages independently.

When G HUB stops, HID++ provides readings for compatible devices. Fully
disconnected devices are hidden; confirmed charging remains visible even when
the device is inactive. Repeated explicit HID++ offline reports and failed G HUB
queries use a five-second grace. Missing HID++ replies are treated as an unknown
connection state, retaining the last reading for up to thirty seconds of failed
communication. A hidden HID++ device needs two valid readings at
least one second apart before reappearing, with no intervening failed reading.
These windows absorb temporary failures, not indefinitely cache charging state.
Provider requests run outside the taskbar UI thread. Reconnection is automatic.
Windows HID arrival/removal and receiver notifications trigger rediscovery;
three-minute scanning also handles devices that do not send connection events.
Event-capable HID++ features use the configured fallback interval. ADC (0x1F20)
and legacy register devices use a two-second poll because they may lack events.
Presence confirmation temporarily uses a short interval to avoid delayed recovery.
The XAML UI updates on data, size and theme changes, without a periodic repaint.
In Auto mode, a transient XAML failure must last 30 seconds before notification
icons appear; missing taskbar symbols select the compatibility backend immediately.
On supported left/right taskbars, indicators are stacked vertically and the
percentage is placed below the icons. Button width follows the taskbar width;
the configured button width applies to horizontal taskbars only.

## Credits and limitations

Disable direct HID++ reading before updating device firmware. Persistent HID
handles are read-only; write access is opened only for each battery/probe request.
This reduces sharing conflicts, but cannot guarantee compatibility with tools
that require exclusive access. Firmware updates with this mod have not been tested.

The G HUB interface is unofficial and may change with G HUB updates.
XAML root access is adapted from Taskbar Folder Menus by sb4ssman:
https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-folder-menus.wh.cpp
G HUB protocol: https://github.com/someweirdhuman/LogiBAT
G HUB events: https://github.com/bmrussell/LGBattery
Icons: https://learn.microsoft.com/windows/apps/design/iconography/segoe-fluent-icons-font
HID++: https://github.com/Logitech/cpg-docs/tree/master/hidpp20
Battery features: https://github.com/pwr-Solaar/Solaar
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- displayMode: auto
  $name: Display backend
  $name:ru: Способ отображения
  $description: Auto uses the Windows 11 XAML tray when available. Native uses standard notification icons; Windows controls their location and overflow visibility.
  $description:ru: Автоматический режим использует XAML-панель Windows 11, если она доступна. Системные значки размещаются средствами Windows, в том числе в меню переполнения трея.
  $options:
  - auto: Automatic
  - native: Native notification icons (Windows 10/11)
  $options:ru:
  - auto: Автоматически
  - native: Системные значки трея (Windows 10/11)
- placement: batteryArea
  $name: Position (XAML)
  $name:ru: Положение (XAML)
  $options:
  - batteryArea: After network and volume
  - beforeIcons: Before tray overflow
  - beforeOmni: Before network and volume
  - beforeClock: Before the clock
  $options:ru:
  - batteryArea: После сети и громкости
  - beforeIcons: Перед стрелкой трея
  - beforeOmni: Перед сетью и громкостью
  - beforeClock: Перед часами
- refreshSeconds: 30
  $name: Poll interval (seconds)
  $name:ru: Интервал резервного опроса (секунды)
  $description: 30-300 seconds. G HUB and HID++ events update readings immediately. ADC and legacy HID++ devices use a separate 2-second interval.
  $description:ru: От 30 до 300 секунд. События G HUB и HID++ обновляют данные сразу. ADC и устаревшие HID++ устройства опрашиваются отдельно каждые 2 секунды.
- iconStyle: batteryAndDevice
  $name: Icon style (XAML)
  $name:ru: Стиль значка (XAML)
  $options:
  - batteryAndDevice: Battery and device icon
  - battery: Battery only
  - device: Device and charging icon
  $options:ru:
  - batteryAndDevice: Батарея и значок устройства
  - battery: Только батарея
  - device: Устройство и значок зарядки
- directHidpp: true
  $name: Read Logitech HID++ directly
  $name:ru: Прямое чтение Logitech HID++
  $description: Detect supported Logitech battery devices without requiring G HUB. Disable before updating device firmware.
  $description:ru: Определять заряд поддерживаемых устройств Logitech без обязательного запуска G HUB. Отключайте перед обновлением прошивки устройств.
- showNewDevices: true
  $name: Show devices without a matching rule
  $name:ru: Показывать устройства без отдельного правила
  $description: Enabled by default. No device configuration is required for automatic detection.
  $description:ru: Включено по умолчанию. Для автоматического определения настройка устройств не нужна.
- devices:
  - - match: ""
      $name: Device name or model (optional)
      $name:ru: Имя или модель устройства (необязательно)
    - show: true
      $name: Show device
      $name:ru: Показывать устройство
  $name: Optional visibility rules
  $name:ru: Необязательные правила отображения
  $description: Leave empty for automatic detection. Rules match names or models; * and ? are supported. The last matching rule wins.
  $description:ru: Для автоматического определения оставьте пустым. Правила сравниваются с именем или моделью; допустимы * и ?. Последнее совпадение имеет приоритет.
- showPercentage: true
  $name: Show percentage (XAML)
  $name:ru: Показывать проценты (XAML)
- showBatteryBar: false
  $name: Show battery bar (XAML)
  $name:ru: Полоска заряда под значком (XAML)
- buttonWidth: 80
  $name: Indicator width (XAML)
  $name:ru: Ширина индикатора (XAML)
  $description: 58-120 logical pixels. Expands to fit the contents; compact when percentage is hidden.
  $description:ru: 58–120 логических пикселей. Увеличивается для содержимого; без процентов используется компактная ширина.
- iconSize: 16
  $name: Icon size (XAML)
  $name:ru: Размер значка (XAML)
  $description: 12-24 logical pixels.
  $description:ru: 12–24 логических пикселя.
- lowBatteryPercent: 20
  $name: Low battery threshold (XAML, %)
  $name:ru: Порог низкого заряда (XAML, %)
  $description: 10-50%. Amber below this threshold, red at 10% or less.
  $description:ru: 10–50%. Ниже порога цвет янтарный; при 10% и меньше — красный.
- openGHubOnClick: true
  $name: Open G HUB on click (XAML)
  $name:ru: Открывать G HUB нажатием (XAML)
*/
// ==/WindhawkModSettings==

// SPDX-License-Identifier: GPL-3.0-or-later

#undef GetCurrentTime
#include <winhttp.h>
#include <commctrl.h>
#include <shellapi.h>
#include <setupapi.h>
#include <dbt.h>
#include <hidsdi.h>
#include <hidpi.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <mutex>
#include <string>
#include <vector>
#include <cwctype>
#include <map>
#include <memory>
#include <optional>
#include <limits>

using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

struct DeviceBattery {
    std::wstring model, name, type;
    int percent = -1;
    bool online = false;
    bool charging = false;
    bool critical = false;
    double hours = -1;
    bool fullyCharged = false;
    bool chargingError = false;
    std::wstring id, key;
    std::wstring unit;
    bool fromHidpp = false;
    bool approximate = false;
    ULONGLONG observedAt = 0;
    ULONGLONG missingSince = 0;
    ULONGLONG revision = 0;
};

struct BatterySnapshot {
    std::vector<DeviceBattery> devices;
    bool hubAvailable = false;
};
static std::atomic<ULONGLONG> g_hubRevision{0};

static bool SameDeviceState(const DeviceBattery& a, const DeviceBattery& b) {
    return a.model == b.model && a.name == b.name && a.type == b.type &&
        a.percent == b.percent && a.online == b.online && a.charging == b.charging &&
        a.critical == b.critical && (a.hours == b.hours ||
            (std::isnan(a.hours) && std::isnan(b.hours))) &&
        a.fullyCharged == b.fullyCharged && a.chargingError == b.chargingError &&
        a.id == b.id && a.key == b.key && a.unit == b.unit &&
        a.fromHidpp == b.fromHidpp && a.approximate == b.approximate;
}

static bool SameDeviceList(const std::vector<DeviceBattery>& a,
                           const std::vector<DeviceBattery>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(),
        [](const auto& left, const auto& right) { return SameDeviceState(left, right); });
}

static bool DeviceShouldBeShown(const DeviceBattery& device) {
    return device.percent >= 0 && device.percent <= 100 && (device.online || device.charging);
}

static bool DecodeHostOffset(const BYTE* code, size_t length, bool arm64, size_t& offset) {
    if (!arm64) {
        if (length < 8 || code[0] != 0x48 || code[1] != 0x83 || code[2] != 0xEC ||
            code[4] != 0x48 || code[5] != 0x83 || code[6] != 0xC1 || code[7] > 0x7F) return false;
        offset = code[7];
        return true;
    }
    if (length < 16) return false;
    DWORD instructions[4];
    memcpy(instructions, code, sizeof(instructions));
    if (instructions[0] != 0xD503237F || (instructions[1] & 0xFFC07FFF) != 0xA9807BFD ||
        instructions[2] != 0x910003FD || (instructions[3] & 0xFFF00FE0) != 0xF8400C00) return false;
    offset = (instructions[3] >> 12) & 0xFF;
    return true;
}

// One in-flight operation per handle. No WinHTTP callback outlives its context.
class AsyncHttpHandle {
public:
    HINTERNET value = nullptr;
    HANDLE complete = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HANDLE closed = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    DWORD status = 0;
    DWORD error = 0;
    WINHTTP_WEB_SOCKET_STATUS read{};
    bool callbackSet = false;

    AsyncHttpHandle() = default;
    AsyncHttpHandle(const AsyncHttpHandle&) = delete;
    AsyncHttpHandle& operator=(const AsyncHttpHandle&) = delete;

    ~AsyncHttpHandle() {
        Close();
        if (complete) CloseHandle(complete);
        if (closed) CloseHandle(closed);
    }

    static void CALLBACK Callback(HINTERNET, DWORD_PTR context,
                                 DWORD notification, void* info, DWORD length) {
        auto* state = reinterpret_cast<AsyncHttpHandle*>(context);
        if (!state) return;
        if (notification == WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING) {
            SetEvent(state->closed);
            return;
        }
        if (notification == WINHTTP_CALLBACK_STATUS_REQUEST_ERROR) {
            state->error = length >= sizeof(WINHTTP_ASYNC_RESULT)
                ? static_cast<WINHTTP_ASYNC_RESULT*>(info)->dwError : ERROR_GEN_FAILURE;
        } else if (notification == WINHTTP_CALLBACK_STATUS_READ_COMPLETE) {
            if (length < sizeof(WINHTTP_WEB_SOCKET_STATUS)) return;
            state->read = *static_cast<WINHTTP_WEB_SOCKET_STATUS*>(info);
        } else if (notification != WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE &&
                   notification != WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE &&
                   notification != WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE) {
            return;
        }
        state->status = notification;
        SetEvent(state->complete);
    }

    bool Attach(HINTERNET handle) {
        value = handle;
        if (!value || !complete || !closed) return false;
        DWORD_PTR context = reinterpret_cast<DWORD_PTR>(this);
        if (!WinHttpSetOption(value, WINHTTP_OPTION_CONTEXT_VALUE,
                              &context, sizeof(context))) return false;
        auto previous = WinHttpSetStatusCallback(value, Callback,
            WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_HANDLES, 0);
        callbackSet = previous != WINHTTP_INVALID_STATUS_CALLBACK;
        return callbackSet;
    }

    void Prepare() {
        ResetEvent(complete);
        status = 0;
        error = 0;
        read = {};
    }

    bool Wait(DWORD expected, HANDLE cancel, DWORD timeout = 2500) {
        HANDLE handles[] = {cancel, complete};
        DWORD result = cancel ? WaitForMultipleObjects(2, handles, FALSE, timeout)
                              : WaitForSingleObject(complete, timeout);
        return result == (cancel ? WAIT_OBJECT_0 + 1 : WAIT_OBJECT_0) &&
               !error && status == expected;
    }

    void Close() {
        if (!value) return;
        WinHttpCloseHandle(value);
        value = nullptr;
        // HANDLE_CLOSING is the last callback and follows all other callbacks.
        if (callbackSet) WaitForSingleObject(closed, INFINITE);
        callbackSet = false;
    }
};

class GHubClient {
    HINTERNET session = nullptr;
    HINTERNET connection = nullptr;
    AsyncHttpHandle socket;
    HANDLE cancel = nullptr;
    char buffer[16384]{};

public:
    explicit GHubClient(HANDLE stopEvent) : cancel(stopEvent) {}
    ~GHubClient() {
        socket.Close();
        if (connection) WinHttpCloseHandle(connection);
        if (session) WinHttpCloseHandle(session);
    }

    bool Connect(unsigned short port = 9010) {
        session = WinHttpOpen(L"Windhawk Logitech Battery Tray/1.0",
            WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC);
        if (!session) return false;
        WinHttpSetTimeouts(session, 1500, 1500, 1500, 2500);
        connection = WinHttpConnect(session, L"127.0.0.1", port, 0);
        if (!connection) return false;
        AsyncHttpHandle request;
        if (!request.Attach(WinHttpOpenRequest(connection, L"GET", L"/", nullptr,
                WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0))) return false;
        if (!WinHttpSetOption(request.value, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET,
                              nullptr, 0)) return false;
        // Never follow a redirect away from the local G HUB endpoint.
        DWORD redirectPolicy = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
        WinHttpSetOption(request.value, WINHTTP_OPTION_REDIRECT_POLICY,
                         &redirectPolicy, sizeof(redirectPolicy));
        request.Prepare();
        if (!WinHttpSendRequest(request.value, L"Sec-WebSocket-Protocol: json\r\n",
                static_cast<DWORD>(-1), WINHTTP_NO_REQUEST_DATA, 0, 0,
                reinterpret_cast<DWORD_PTR>(&request)) ||
            !request.Wait(WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE, cancel)) return false;
        request.Prepare();
        if (!WinHttpReceiveResponse(request.value, nullptr) ||
            !request.Wait(WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE, cancel)) return false;
        DWORD statusCode = 0, length = sizeof(statusCode);
        if (!WinHttpQueryHeaders(request.value, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &length,
                WINHTTP_NO_HEADER_INDEX) || statusCode != 101) return false;
        return socket.Attach(WinHttpWebSocketCompleteUpgrade(request.value,
                              reinterpret_cast<DWORD_PTR>(&socket)));
    }

    void Send(const std::wstring& path, PCWSTR verb) {
        JsonObject request;
        request.SetNamedValue(L"path", JsonValue::CreateStringValue(path));
        request.SetNamedValue(L"verb", JsonValue::CreateStringValue(verb));
        auto bytes = winrt::to_string(request.Stringify());
        socket.Prepare();
        if (WinHttpWebSocketSend(socket.value, WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE,
                bytes.data(), static_cast<DWORD>(bytes.size())) != NO_ERROR ||
            !socket.Wait(WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE, cancel)) {
            // WinHTTP can still be using bytes until cancellation completes.
            socket.Close();
            throw winrt::hresult_error(E_FAIL, L"G HUB send failed or timed out");
        }
    }

    JsonObject Receive(ULONGLONG deadline = 0) {
        std::string message;
        for (;;) {
            auto now = GetTickCount64();
            if (deadline && now >= deadline)
                throw winrt::hresult_error(E_FAIL, L"G HUB response timeout");
            socket.Prepare();
            if (WinHttpWebSocketReceive(socket.value, buffer, sizeof(buffer), nullptr, nullptr) != NO_ERROR ||
                !socket.Wait(WINHTTP_CALLBACK_STATUS_READ_COMPLETE, cancel,
                    deadline ? static_cast<DWORD>(deadline - now) : INFINITE)) {
                // The buffer and callback context stay alive until close finishes.
                socket.Close();
                throw winrt::hresult_error(E_FAIL, L"G HUB receive failed or timed out");
            }
            if (socket.read.eBufferType == WINHTTP_WEB_SOCKET_CLOSE_BUFFER_TYPE)
                throw winrt::hresult_error(E_FAIL, L"G HUB disconnected");
            if (socket.read.eBufferType != WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE &&
                socket.read.eBufferType != WINHTTP_WEB_SOCKET_UTF8_FRAGMENT_BUFFER_TYPE)
                throw winrt::hresult_error(E_FAIL, L"Unexpected G HUB response");
            message.append(buffer, socket.read.dwBytesTransferred);
            if (message.size() > 1024 * 1024)
                throw winrt::hresult_error(E_FAIL, L"G HUB response too large");
            if (socket.read.eBufferType == WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE) break;
        }
        JsonObject response;
        if (!JsonObject::TryParse(winrt::to_hstring(message), response))
            throw winrt::hresult_error(E_FAIL, L"Invalid G HUB JSON");
        return response;
    }

    JsonObject Get(const std::wstring& path) {
        Send(path, L"GET");
        auto deadline = GetTickCount64() + 3500;
        for (int messageCount = 0; messageCount < 32; ++messageCount) {
            auto response = Receive(deadline);
            if (response.GetNamedString(L"path", L"") == path) return response;
        }
        throw winrt::hresult_error(E_FAIL, L"No matching G HUB response");
    }
};

static bool RequestSucceeded(const JsonObject& response) {
    auto result = response.GetNamedObject(L"result", nullptr);
    return result && result.GetNamedString(L"code", L"") == L"SUCCESS";
}

static bool ApplyBattery(DeviceBattery& device, const JsonObject& battery, bool active,
                         bool preserveMissingState = false) {
    if (!battery || !battery.HasKey(L"percentage")) return false;
    auto percent = battery.GetNamedNumber(L"percentage", -1);
    if (!std::isfinite(percent) || percent < 0 || percent > 100 ||
        battery.GetNamedString(L"batteryStateOfChargeSupport", L"") == L"STATE_OF_CHARGE_UNSUPPORTED" ||
        battery.GetNamedBoolean(L"isBatteryRemoved", false)) {
        device.online = false;
        device.percent = -1;
        device.charging = false;
        device.chargingError = false;
        device.fullyCharged = false;
        device.hours = -1;
        return true;
    }
    device.percent = static_cast<int>(std::lround(percent));
    device.online = active;
    // Charging can continue to be reported while a headset is powered off.
    device.charging = battery.GetNamedBoolean(L"charging",
        preserveMissingState ? device.charging : false);
    device.critical = battery.GetNamedBoolean(L"criticalLevel", device.critical && device.percent <= 10);
    device.hours = battery.GetNamedNumber(L"mileage", device.hours);
    device.fullyCharged = battery.GetNamedBoolean(L"fullyCharged", device.fullyCharged && device.percent == 100);
    device.chargingError = battery.GetNamedBoolean(L"chargingError",
        preserveMissingState ? device.chargingError : false);
    return true;
}

static BatterySnapshot QueryBatteries(GHubClient& client, HANDLE cancel) {
    BatterySnapshot result;
    auto list = client.Get(L"/devices/list");
    if (!RequestSucceeded(list)) return result;
    auto payload = list.GetNamedObject(L"payload", nullptr);
    if (!payload) return result;
    auto devices = payload.GetNamedArray(L"deviceInfos", nullptr);
    if (!devices) return result;
    result.hubAvailable = true;
    for (auto value : devices) {
        if (result.devices.size() >= 64 || WaitForSingleObject(cancel, 0) == WAIT_OBJECT_0) break;
        auto info = value.GetObject();
        auto type = info.GetNamedString(L"deviceType", L"");
        auto capabilities = info.GetNamedObject(L"capabilities", nullptr);
        if (capabilities && capabilities.HasKey(L"hasBatteryStatus") &&
            !capabilities.GetNamedBoolean(L"hasBatteryStatus", false)) continue;
        DeviceBattery device;
        device.type = type;
        device.model = info.GetNamedString(L"deviceModel", L"");
        device.name = info.GetNamedString(L"displayName", type);
        auto id = info.GetNamedString(L"id", L"");
        if (id.empty()) continue;
        device.id = id;
        auto unit = info.GetNamedString(L"deviceUnitId", L"");
        if (!unit.empty()) {
            try {
                wchar_t unitText[24]{};
                auto value = std::stoull(std::wstring(unit));
                swprintf_s(unitText, ARRAYSIZE(unitText), L"%08llX", value);
                device.unit = unitText;
            } catch (...) { device.unit = unit; }
        }
        auto signature = info.GetNamedString(L"deviceSignature", id);
        device.key = L"ghub:" + device.model + L":" + (device.unit.empty() ? std::wstring(signature) : device.unit);
        bool active = info.GetNamedString(L"state", L"") == L"ACTIVE";
        device.revision = ++g_hubRevision;
        device.online = active;
        bool hasValue = false, transportFailed = false;
        try {
            auto response = client.Get(L"/battery/" + std::wstring(id) + L"/state");
            auto battery = response.GetNamedObject(L"payload", nullptr);
            hasValue = RequestSucceeded(response) && ApplyBattery(device, battery, active);
            if (hasValue) device.observedAt = GetTickCount64();
        } catch (...) {
            // Keep completed readings; do not reuse a broken connection for every device.
            transportFailed = true;
            result.hubAvailable = false;
        }
        // Unknown capabilities plus an unsupported endpoint is not a battery device.
        if (hasValue || (capabilities && capabilities.GetNamedBoolean(L"hasBatteryStatus", false)))
            result.devices.push_back(std::move(device));
        if (transportFailed) break;
    }
    std::stable_sort(result.devices.begin(), result.devices.end(), [](const auto& a, const auto& b) {
        return a.type == L"MOUSE" && b.type != L"MOUSE";
    });
    return result;
}

static bool ApplyHubEvent(BatterySnapshot& snapshot, const JsonObject& event) {
    if (event.GetNamedString(L"path", L"") != L"/battery/state/changed") return false;
    auto payload = event.GetNamedObject(L"payload", nullptr);
    if (!payload) return false;
    auto id = payload.GetNamedString(L"deviceId", L"");
    auto found = std::find_if(snapshot.devices.begin(), snapshot.devices.end(), [&](auto& d) { return d.id == id; });
    if (found == snapshot.devices.end()) return false;
    auto updated = *found;
    if (payload.GetNamedBoolean(L"isBatteryRemoved", false)) {
        updated.online = false;
        updated.percent = -1;
        updated.charging = updated.chargingError = false;
    } else if (payload.HasKey(L"percentage")) {
        if (!ApplyBattery(updated, payload, updated.online, true)) return false;
    } else if (payload.HasKey(L"charging")) {
        updated.charging = payload.GetNamedBoolean(L"charging", false);
    } else return false;
    updated.observedAt = GetTickCount64();
    updated.missingSince = 0;
    updated.revision = ++g_hubRevision;
    bool stateChanged = !SameDeviceState(*found, updated);
    bool hubAvailabilityChanged = !snapshot.hubAvailable;
    *found = std::move(updated);
    snapshot.hubAvailable = true;
    return stateChanged || hubAvailabilityChanged;
}

static bool HubEventNeedsDiscovery(const BatterySnapshot& snapshot, const JsonObject& event) {
    if (event.GetNamedString(L"path", L"") != L"/battery/state/changed") return false;
    auto payload = event.GetNamedObject(L"payload", nullptr);
    if (!payload) return false;
    auto id = payload.GetNamedString(L"deviceId", L"");
    if (id.empty()) return false;
    auto found = std::find_if(snapshot.devices.begin(), snapshot.devices.end(), [&](auto& d) { return d.id == id; });
    return found == snapshot.devices.end() || !found->online;
}

static void KeepFreshHubReadings(BatterySnapshot& next, const BatterySnapshot& current) {
    for (auto& device : next.devices) {
        auto previous = std::find_if(current.devices.begin(), current.devices.end(), [&](auto& d) { return d.key == device.key; });
        if (previous != current.devices.end() && previous->revision > device.revision) {
            bool online = device.online;
            device = *previous;
            // Battery events carry no connection state; the list remains authoritative.
            device.online = online;
        }
    }
}

static BatterySnapshot MergeHubSnapshot(BatterySnapshot next,
        const BatterySnapshot& current, ULONGLONG queryRevision, ULONGLONG now) {
    constexpr ULONGLONG presenceGraceMs = 5000;
    KeepFreshHubReadings(next, current);
    for (const auto& previous : current.devices) {
        auto present = std::find_if(next.devices.begin(), next.devices.end(), [&](auto& device) {
            return device.key == previous.key;
        });
        if (present != next.devices.end() && present->observedAt) continue;
        if (present == next.devices.end() && next.devices.size() >= 64) continue;

        auto missing = previous;
        if (!missing.missingSince) missing.missingSince = now;
        if (previous.revision <= queryRevision &&
            (!previous.observedAt || now - missing.missingSince >= presenceGraceMs)) {
            missing.online = false;
            missing.charging = false;
            missing.percent = -1;
            missing.hours = -1;
            missing.chargingError = false;
        }
        if (present != next.devices.end()) *present = std::move(missing);
        else next.devices.push_back(std::move(missing));
    }
    std::stable_sort(next.devices.begin(), next.devices.end(), [](const auto& a, const auto& b) {
        return a.type == L"MOUSE" && b.type != L"MOUSE";
    });
    return next;
}

static std::wstring Lower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(), [](wchar_t c) { return std::towlower(c); });
    return value;
}

static bool MatchDeviceText(std::wstring pattern, std::wstring text) {
    pattern = Lower(std::move(pattern));
    text = Lower(std::move(text));
    // A plain name is a substring; explicit wildcards match the whole value.
    if (pattern.find_first_of(L"*?") == std::wstring::npos)
        return text.find(pattern) != std::wstring::npos;
    size_t p = 0, t = 0, star = std::wstring::npos, retry = 0;
    while (t < text.size()) {
        if (p < pattern.size() && (pattern[p] == L'?' || pattern[p] == text[t])) { ++p; ++t; }
        else if (p < pattern.size() && pattern[p] == L'*') { star = p++; retry = t; }
        else if (star != std::wstring::npos) { p = star + 1; t = ++retry; }
        else return false;
    }
    while (p < pattern.size() && pattern[p] == L'*') ++p;
    return p == pattern.size();
}

struct DeviceRule { std::wstring match; bool show = true; };

static bool DeviceVisible(const DeviceBattery& device, const std::vector<DeviceRule>& rules, bool showNew) {
    bool show = showNew;
    for (const auto& rule : rules) {
        if (!rule.match.empty() && (MatchDeviceText(rule.match, device.name) ||
            MatchDeviceText(rule.match, device.model) || MatchDeviceText(rule.match, device.key))) show = rule.show;
    }
    return show;
}

static std::wstring BatteryGlyph(int percent, bool charging) {
    // Horizontal battery glyphs built into Windows. Charging9/10 are noncontiguous.
    static constexpr wchar_t ordinary[] = {0xE850,0xE851,0xE852,0xE853,0xE854,0xE855,0xE856,0xE857,0xE858,0xE859,0xE83F};
    static constexpr wchar_t powered[] = {0xE85A,0xE85B,0xE85C,0xE85D,0xE85E,0xE85F,0xE860,0xE861,0xE862,0xE83E,0xEA93};
    int level = percent < 0 ? 0 : std::clamp(percent / 10, 0, 10);
    return std::wstring(1, charging ? powered[level] : ordinary[level]);
}

static PCWSTR DeviceGlyph(const std::wstring& rawType) {
    auto type = Lower(rawType);
    if (type.find(L"mouse") != std::wstring::npos) return L"\uE962";
    if (type.find(L"head") != std::wstring::npos || type.find(L"ear") != std::wstring::npos) return L"\uE7F6";
    if (type.find(L"keyboard") != std::wstring::npos) return L"\uE765";
    if (type.find(L"game") != std::wstring::npos || type.find(L"controller") != std::wstring::npos || type.find(L"joystick") != std::wstring::npos) return L"\uE7FC";
    if (type.find(L"speaker") != std::wstring::npos) return L"\uE7F5";
    if (type.find(L"phone") != std::wstring::npos) return L"\uE8EA";
    if (type.find(L"tablet") != std::wstring::npos || type.find(L"touch") != std::wstring::npos) return L"\uE70A";
    if (type.find(L"presenter") != std::wstring::npos) return L"\uE714";
    return L"\uE772";
}

// HID++ queries only: no configuration registers, pairing, or firmware writes.
static int VoltagePercent(int mv) {
    static constexpr int curve[][2] = {{4186,100},{4067,90},{3989,80},{3922,70},{3859,60},{3811,50},{3778,40},{3751,30},{3717,20},{3671,10},{3646,5},{3579,2},{3500,0}};
    if (mv >= curve[0][0]) return 100;
    for (size_t i = 1; i < ARRAYSIZE(curve); ++i)
        if (mv >= curve[i][0]) return static_cast<int>(std::lround(curve[i][1] +
            double(mv - curve[i][0]) * (curve[i-1][1] - curve[i][1]) / (curve[i-1][0] - curve[i][0])));
    return 0;
}

static bool DecodeHidBattery(DeviceBattery& device, unsigned feature, const std::vector<BYTE>& p) {
    if (p.size() < 3) return false;
    int percent = -1;
    bool charging = false, full = false, error = false, approximate = false;
    if (feature == 0x1004 || feature == 0x1000) {
        if (p[0] > 100 || p[2] > 6) return false;
        percent = p[0];
        if (feature == 0x1004 && !percent && p[1]) {
            percent = p[1] == 8 ? 90 : p[1] == 4 ? 50 : p[1] == 2 ? 20 : p[1] == 1 ? 5 : -1;
            approximate = true;
        }
        charging = p[2] >= 1 && p[2] <= 4;
        full = p[2] == 3;
        error = p[2] == 5 || p[2] == 6;
    } else if (feature == 0x1001 || feature == 0x1F20) {
        int mv = (p[0] << 8) | p[1];
        if (mv < 2500 || mv > 5000) return false;
        // ADC flags are status values, not independent connected/charging bits.
        if (feature == 0x1F20 && p[2] != 1 && p[2] != 3 && p[2] != 7) return false;
        percent = VoltagePercent(mv);
        approximate = true;
        charging = feature == 0x1001 ? (p[2] & 0x80) != 0 : (p[2] & 2) != 0;
        full = feature == 0x1001 ? charging && (p[2] & 3) == 3 : p[2] == 7;
        if (feature == 0x1F20 && full) percent = 100;
    } else if (feature == 0x0D) {
        if (p[0] > 100) return false;
        percent = p[0];
        charging = (p[2] & 0xF0) == 0x50 || (p[2] & 0xF0) == 0x90;
        full = (p[2] & 0xF0) == 0x90;
    } else if (feature == 0x07) {
        percent = p[0] == 7 ? 90 : p[0] == 5 ? 50 : p[0] == 3 ? 20 : p[0] == 1 ? 5 : -1;
        approximate = true;
        charging = (p[1] & 0x21) == 0x21 || (p[1] & 0x22) == 0x22;
        full = (p[1] & 0x22) == 0x22;
    } else return false;
    if (percent < 0) return false;
    device.percent = percent;
    device.online = true;
    device.charging = charging;
    device.fullyCharged = full;
    device.chargingError = error;
    device.approximate = approximate;
    device.critical = percent <= 5;
    device.observedAt = GetTickCount64();
    return true;
}

class HidEndpoint {
    HANDLE handle = INVALID_HANDLE_VALUE;
    std::wstring devicePath;
    OVERLAPPED readOverlapped{};
    std::vector<BYTE> readBuffer;
    bool readPending = false, ready = false;
    DWORD readBytes = 0;
public:
    bool failed = false;
    USHORT inputLength = 0, outputLength = 0;
    HidEndpoint() { readOverlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr); }
    ~HidEndpoint() {
        if (readPending) {
            CancelIoEx(handle, &readOverlapped);
            GetOverlappedResult(handle, &readOverlapped, &readBytes, TRUE);
        }
        if (handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
        if (readOverlapped.hEvent) CloseHandle(readOverlapped.hEvent);
    }
    bool Open(const std::wstring& path) {
        devicePath = path;
        handle = CreateFileW(path.c_str(), GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
        if (handle == INVALID_HANDLE_VALUE || !readOverlapped.hEvent) return false;
        PHIDP_PREPARSED_DATA data = nullptr;
        HIDP_CAPS caps{};
        if (!HidD_GetPreparsedData(handle, &data)) return false;
        NTSTATUS status = HidP_GetCaps(data, &caps);
        HidD_FreePreparsedData(data);
        if (status != HIDP_STATUS_SUCCESS || caps.InputReportByteLength < 7 ||
            caps.InputReportByteLength > 512 || caps.OutputReportByteLength < 7 || caps.OutputReportByteLength > 512) return false;
        inputLength = caps.InputReportByteLength;
        outputLength = caps.OutputReportByteLength;
        // Lighting traffic shares this input collection with battery replies.
        HidD_SetNumInputBuffers(handle, 256);
        readBuffer.resize(inputLength);
        return StartRead();
    }
    HANDLE Event() const { return readOverlapped.hEvent; }
    bool StartRead() {
        if (readPending || ready) return true;
        ResetEvent(readOverlapped.hEvent);
        BOOL result = ReadFile(handle, readBuffer.data(), inputLength, &readBytes, &readOverlapped);
        if (result) { ready = true; SetEvent(readOverlapped.hEvent); return true; }
        readPending = GetLastError() == ERROR_IO_PENDING;
        if (!readPending) failed = true;
        return readPending;
    }
    std::vector<BYTE> TakeRead() {
        if (!ready && (!readPending || WaitForSingleObject(readOverlapped.hEvent, 0) != WAIT_OBJECT_0)) return {};
        bool succeeded = ready || GetOverlappedResult(handle, &readOverlapped, &readBytes, FALSE);
        readPending = ready = false;
        ResetEvent(readOverlapped.hEvent);
        if (!succeeded || readBytes > readBuffer.size()) { failed = true; return {}; }
        std::vector<BYTE> packet(readBuffer.begin(), readBuffer.begin() + readBytes);
        StartRead();
        return packet;
    }
    bool Write(const std::vector<BYTE>& packet, HANDLE cancel) {
        if (packet.size() > outputLength) return false;
        // Keep write access only for the request, not for the mod's lifetime.
        HANDLE output = CreateFileW(devicePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
        if (output == INVALID_HANDLE_VALUE) return false;
        struct CloseOutput { HANDLE handle; ~CloseOutput() { CloseHandle(handle); } } closeOutput{output};
        std::vector<BYTE> buffer(outputLength);
        std::copy(packet.begin(), packet.end(), buffer.begin());
        OVERLAPPED operation{};
        operation.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!operation.hEvent) return false;
        DWORD bytes = 0;
        bool success = WriteFile(output, buffer.data(), static_cast<DWORD>(buffer.size()), &bytes, &operation);
        if (!success && GetLastError() == ERROR_IO_PENDING) {
            HANDLE events[] = {cancel, operation.hEvent};
            DWORD wait = WaitForMultipleObjects(2, events, FALSE, 800);
            if (wait != WAIT_OBJECT_0 + 1) CancelIoEx(output, &operation);
            success = GetOverlappedResult(output, &operation, &bytes, TRUE) && wait == WAIT_OBJECT_0 + 1;
        }
        DWORD error = success ? ERROR_SUCCESS : GetLastError();
        CloseHandle(operation.hEvent);
        // A write timeout is not proof that the USB interface was removed.
        if (!success && (error == ERROR_DEVICE_NOT_CONNECTED || error == ERROR_INVALID_HANDLE)) failed = true;
        return success && bytes == buffer.size();
    }
};

struct HidCollection {
    std::wstring key, shortPath, longPath, product;
    USHORT pid = 0;
    bool receiver = false, headset = false;
};

static std::vector<HidCollection> EnumerateLogitechHid() {
    GUID hidGuid{};
    HidD_GetHidGuid(&hidGuid);
    HDEVINFO set = SetupDiGetClassDevsW(&hidGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (set == INVALID_HANDLE_VALUE) return {};
    struct Guard { HDEVINFO set; ~Guard() { SetupDiDestroyDeviceInfoList(set); } } guard{set};
    std::map<std::wstring, HidCollection> groups;
    for (DWORD i = 0; i < 4096; ++i) {
        SP_DEVICE_INTERFACE_DATA item{};
        item.cbSize = sizeof(item);
        if (!SetupDiEnumDeviceInterfaces(set, nullptr, &hidGuid, i, &item)) break;
        DWORD needed = 0;
        SetupDiGetDeviceInterfaceDetailW(set, &item, nullptr, 0, &needed, nullptr);
        if (!needed || needed > 65536) continue;
        std::vector<BYTE> storage(needed);
        auto* detail = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(storage.data());
        detail->cbSize = sizeof(*detail);
        if (!SetupDiGetDeviceInterfaceDetailW(set, &item, detail, needed, nullptr, nullptr)) continue;
        std::wstring path = detail->DevicePath;
        // Most HID interfaces belong to other vendors; don't even open them.
        if (Lower(path).find(L"046d") == std::wstring::npos) continue;
        HANDLE probe = CreateFileW(path.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr, OPEN_EXISTING, 0, nullptr);
        if (probe == INVALID_HANDLE_VALUE) continue;
        HIDD_ATTRIBUTES attributes{sizeof(HIDD_ATTRIBUTES)};
        PHIDP_PREPARSED_DATA data = nullptr;
        HIDP_CAPS caps{};
        bool valid = HidD_GetAttributes(probe, &attributes) && attributes.VendorID == 0x046D &&
            HidD_GetPreparsedData(probe, &data);
        if (valid) valid = HidP_GetCaps(data, &caps) == HIDP_STATUS_SUCCESS;
        if (data) HidD_FreePreparsedData(data);
        wchar_t product[256]{};
        if (valid) HidD_GetProductString(probe, product, sizeof(product));
        CloseHandle(probe);
        if (!valid) continue;
        bool standard = caps.UsagePage == 0xFF00 && (caps.Usage == 1 || caps.Usage == 2);
        bool headset = (caps.UsagePage == 0xFF43 && caps.Usage == 0x0202) ||
            (attributes.ProductID == 0x0AC4 && caps.UsagePage == 0x000C && caps.Usage == 1);
        if (!standard && !headset) continue;
        // Adjacent short/long collections differ in COLxx and their last instance suffix.
        auto normalized = Lower(path);
        size_t first = normalized.find(L'#');
        size_t begin = first == std::wstring::npos ? first : normalized.find(L'#', first + 1);
        size_t end = begin == std::wstring::npos ? begin : normalized.find(L'#', begin + 1);
        auto instance = end == std::wstring::npos ? normalized : normalized.substr(begin + 1, end - begin - 1);
        auto suffix = instance.find_last_of(L'&');
        if (suffix != std::wstring::npos) instance.resize(suffix);
        std::wstring key = std::to_wstring(attributes.ProductID) + L":" + instance;
        auto& group = groups[key];
        group.key = key;
        group.pid = attributes.ProductID;
        group.product = product;
        group.headset |= headset;
        group.receiver = !headset && (Lower(group.product).find(L"receiver") != std::wstring::npos ||
            (group.pid >= 0xC500 && group.pid <= 0xC5FF));
        if (standard && caps.Usage == 1) group.shortPath = path;
        else if (standard || group.longPath.empty()) group.longPath = path;
    }
    std::vector<HidCollection> result;
    for (auto& [key, group] : groups) if (!group.longPath.empty()) result.push_back(std::move(group));
    return result;
}

struct HidPresence {
    ULONGLONG availableSince = 0, unavailableSince = 0, disconnectedSince = 0;
};

static DWORD HidPollDelay(unsigned feature, bool confirming, DWORD fallback) {
    return confirming || feature == 0x1F20 || feature < 0x1000 ? 2000 : fallback;
}

struct RetryBackoff {
    DWORD delay = 1000;
    DWORD Failed() { DWORD result = delay; delay = std::min<DWORD>(delay * 2, 60000); return result; }
    void Reset() { delay = 1000; }
};

static bool HidAddressMatches(BYTE expected, BYTE actual, bool directHeadset) {
    return expected == actual || (directHeadset && expected == 0xFF && actual == 0);
}

static bool IsBatteryInput(const std::vector<BYTE>& packet, BYTE slot, BYTE index,
                           unsigned feature, bool directHeadset) {
    if (packet.size() < 7 || (packet[0] != 0x10 && packet[0] != 0x11) ||
        !HidAddressMatches(slot, packet[1], directHeadset) || packet[2] != index) return false;
    // ADC read replies are also useful when a different client requested them.
    // Only the known battery function is accepted, never lighting/control replies.
    return packet[3] == 0 || (feature == 0x1F20 && (packet[3] >> 4) == 0);
}

static bool AcceptBatteryReply(unsigned feature, const std::vector<BYTE>& payload) {
    if (feature != 0x1F20) return true;
    DeviceBattery reading;
    return DecodeHidBattery(reading, feature, payload);
}

static bool IsConfirmedHidAbsence(unsigned feature, const std::vector<BYTE>& payload) {
    return feature == 0x1F20 && payload.size() >= 3 && payload[0] == 0 && payload[1] == 0 &&
        (payload[2] == 0 || payload[2] == 0x0F);
}

static void UpdateHidPresence(DeviceBattery& device, const DeviceBattery* reading,
                              HidPresence& presence, ULONGLONG now, bool confirmedUnavailable = true) {
    if (reading && DeviceShouldBeShown(*reading)) {
        presence.unavailableSince = presence.disconnectedSince = 0;
        if (!DeviceShouldBeShown(device)) {
            if (!presence.availableSince) presence.availableSince = now;
            // A powered-off charging headset can briefly answer between disconnects.
            if (now - presence.availableSince < 1000) return;
        }
        device = *reading;
        return;
    }
    presence.availableSince = 0;
    if (!presence.unavailableSince) presence.unavailableSince = now;
    if (confirmedUnavailable && !presence.disconnectedSince) presence.disconnectedSince = now;
    // A missing reply says nothing about the power switch. Explicit repeated
    // offline reports get a short grace; communication loss has a bounded cache.
    bool disconnected = confirmedUnavailable && now - presence.disconnectedSince >= 5000;
    if (disconnected || now - presence.unavailableSince >= 30000) {
        device.online = false;
        device.percent = -1;
        device.charging = device.chargingError = device.fullyCharged = false;
        device.hours = -1;
    }
}

class HidChannel {
    std::unique_ptr<HidEndpoint> shortEndpoint, longEndpoint;
    HANDLE cancel;
    BYTE software = 9;
    struct Slot {
        BYTE slot = 0, index = 0;
        unsigned feature = 0;
        bool discoverAgain = false;
        HidPresence presence;
        ULONGLONG nextPoll = 0;
        DeviceBattery device;
    };
    std::vector<Slot> slots;
    bool discoveryRequested = false;
    void Notification(const std::vector<BYTE>& packet) {
        if (packet.size() < 7 || (packet[0] != 0x10 && packet[0] != 0x11)) return;
        if (packet[0] == 0x10 && packet[2] == 0x41) {
            // This can be the first notification from a previously unknown receiver slot.
            discoveryRequested = true;
            for (auto& slot : slots) if (packet[1] == slot.slot) slot.discoverAgain = true;
            return;
        }
        for (auto& slot : slots) {
            if (slot.feature < 0x1000 || !IsBatteryInput(packet, slot.slot, slot.index,
                slot.feature, collection.headset && !collection.receiver)) continue;
            auto previous = slot.device;
            auto reading = slot.device;
            std::vector<BYTE> payload(packet.begin() + 4, packet.end());
            bool valid = DecodeHidBattery(reading, slot.feature, payload);
            if (valid || IsConfirmedHidAbsence(slot.feature, payload)) {
                UpdateHidPresence(slot.device, valid ? &reading : nullptr, slot.presence, GetTickCount64());
                if ((DeviceShouldBeShown(slot.device) && slot.presence.unavailableSince) ||
                    (!DeviceShouldBeShown(slot.device) && slot.presence.availableSince))
                    slot.nextPoll = std::min(slot.nextPoll, GetTickCount64() + 2000);
                if (!SameDeviceState(previous, slot.device) && changed) changed();
            }
        }
    }
public:
    HidCollection collection;
    std::function<void()> changed;
    BYTE lastError = 0;
    bool NeedsDiscovery() const { return discoveryRequested; }
    bool Healthy() const { return longEndpoint && !longEndpoint->failed && (!shortEndpoint || !shortEndpoint->failed); }
    void AppendWaitHandles(std::vector<HANDLE>& events) const {
        for (auto* endpoint : {longEndpoint.get(), shortEndpoint.get()})
            if (endpoint && !endpoint->failed) events.push_back(endpoint->Event());
    }
    explicit HidChannel(HidCollection value, HANDLE stop) : cancel(stop), collection(std::move(value)) {}
    bool Open() {
        longEndpoint = std::make_unique<HidEndpoint>();
        if (!longEndpoint->Open(collection.longPath) || longEndpoint->outputLength < 20) return false;
        if (!collection.shortPath.empty()) {
            auto endpoint = std::make_unique<HidEndpoint>();
            if (endpoint->Open(collection.shortPath)) shortEndpoint = std::move(endpoint);
        }
        return true;
    }
    std::vector<BYTE> Request(BYTE slot, BYTE feature, BYTE function, const std::vector<BYTE>& params = {},
                              bool legacy = false, unsigned batteryFeature = 0) {
        lastError = 0;
        if (WaitForSingleObject(cancel, 0) == WAIT_OBJECT_0) return {};
        software = software >= 15 ? 10 : software + 1;
        BYTE address = legacy ? function : static_cast<BYTE>((function << 4) | software);
        std::vector<BYTE> packet(legacy ? 7 : 20);
        packet[0] = legacy ? 0x10 : 0x11;
        packet[1] = slot; packet[2] = feature; packet[3] = address;
        if (params.size() > packet.size() - 4) return {};
        std::copy(params.begin(), params.end(), packet.begin() + 4);
        auto* output = legacy && shortEndpoint ? shortEndpoint.get() : longEndpoint.get();
        if (!output->Write(packet, cancel)) return {};
        ULONGLONG deadline = GetTickCount64() + 600;
        std::vector<BYTE> unavailableReply;
        HANDLE events[] = {cancel, longEndpoint->Event(), shortEndpoint ? shortEndpoint->Event() : longEndpoint->Event()};
        for (;;) {
            auto now = GetTickCount64();
            if (now >= deadline) break;
            DWORD wait = WaitForMultipleObjects(shortEndpoint ? 3 : 2, events, FALSE,
                static_cast<DWORD>(deadline - now));
            if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) return {};
            if (wait == WAIT_TIMEOUT) break;
            auto reply = (wait == WAIT_OBJECT_0 + 2 ? shortEndpoint : longEndpoint)->TakeRead();
            if (reply.size() < 4 || (reply[0] != 0x10 && reply[0] != 0x11)) continue;
            if (HidAddressMatches(slot, reply[1], collection.headset && !collection.receiver)) {
                if (reply.size() >= 6 && ((reply[0] == 0x10 && reply[2] == 0x8F) || (reply[0] == 0x11 && reply[2] == 0xFF)) &&
                    reply[3] == feature && reply[4] == address) { lastError = reply[5]; return {}; }
                if (reply[2] == feature && reply[3] == address) {
                    std::vector<BYTE> payload(reply.begin() + 4, reply.end());
                    if (AcceptBatteryReply(batteryFeature, payload)) return payload;
                    // Some headsets send a zero ADC response followed by real data
                    // for the same request. Keep receiving within the original deadline.
                    unavailableReply = std::move(payload);
                    continue;
                }
            }
            Notification(reply);
        }
        return unavailableReply;
    }
    BYTE Feature(BYTE slot, unsigned feature) {
        auto answer = Request(slot, 0, 0, {static_cast<BYTE>(feature >> 8), static_cast<BYTE>(feature)});
        return answer.empty() ? 0 : answer[0];
    }
    void Discover() {
        discoveryRequested = false;
        for (int number = collection.receiver ? 1 : 255; number <= (collection.receiver ? 6 : 255); ++number) {
            if (WaitForSingleObject(cancel, 0) == WAIT_OBJECT_0) break;
            BYTE slotNumber = static_cast<BYTE>(number);
            auto existing = std::find_if(slots.begin(), slots.end(), [&](auto& s) { return s.slot == slotNumber; });
            if (existing != slots.end()) {
                if (!existing->discoverAgain) continue;
            }
            auto ping = Request(slotNumber, 0, 1, {0, 0, 0xA5});
            bool legacy = ping.empty() && lastError == 1;
            if (ping.empty() && !legacy) continue;
            Slot slot;
            slot.slot = slotNumber;
            slot.device.fromHidpp = true;
            slot.device.name = collection.product.empty() ? L"Logitech" : collection.product;
            slot.device.type = collection.headset ? L"HEADSET" : L"DEVICE";
            slot.device.id = collection.key + L":" + std::to_wstring(number);
            slot.device.model = slot.device.name;
            slot.device.key = L"hidpp:" + slot.device.id;
            if (legacy) {
                for (unsigned reg : {0x0DU, 0x07U}) {
                    auto answer = Request(slotNumber, 0x81, static_cast<BYTE>(reg), {}, true);
                    if (DecodeHidBattery(slot.device, reg, answer)) { slot.feature = reg; break; }
                }
            } else {
                for (unsigned feature : {0x1004U, 0x1000U, 0x1001U, 0x1F20U}) {
                    BYTE index = Feature(slotNumber, feature);
                    if (!index) continue;
                    auto answer = Request(slotNumber, index, feature == 0x1004 ? 1 : 0, {}, false, feature);
                    auto reading = slot.device;
                    if (DecodeHidBattery(reading, feature, answer)) {
                        slot.feature = feature;
                        slot.index = index;
                        slot.device = std::move(reading);
                        break;
                    }
                    // A known ADC endpoint remains pollable while its headset is off.
                    // Do not require a valid battery sample to discover the connection.
                    if (feature == 0x1F20) { slot.feature = feature; slot.index = index; }
                }
                if (!slot.feature) continue;
                if (BYTE nameIndex = Feature(slotNumber, 5)) {
                    auto size = Request(slotNumber, nameIndex, 0);
                    std::string name;
                    unsigned length = size.empty() ? 0 : std::min<unsigned>(size[0], 128);
                    while (name.size() < length) {
                        auto part = Request(slotNumber, nameIndex, 1, {static_cast<BYTE>(name.size())});
                        if (part.empty()) break;
                        size_t count = std::min<size_t>(part.size(), length - name.size());
                        name.append(reinterpret_cast<const char*>(part.data()), count);
                    }
                    if (name.size() == length && length) {
                        try { slot.device.name = winrt::to_hstring(name); slot.device.model = slot.device.name; } catch (...) {}
                    }
                    auto type = Request(slotNumber, nameIndex, 2);
                    if (!type.empty() && !collection.headset)
                        slot.device.type = type[0] == 0 || type[0] == 2 ? L"KEYBOARD" : type[0] == 3 || type[0] == 5 ? L"MOUSE" : type[0] == 4 ? L"TOUCHPAD" : type[0] == 6 ? L"PRESENTER" : L"DEVICE";
                }
                if (BYTE infoIndex = Feature(slotNumber, 3)) {
                    auto info = Request(slotNumber, infoIndex, 0);
                    if (info.size() >= 5) {
                        unsigned unit = (unsigned(info[1]) << 24) | (unsigned(info[2]) << 16) | (unsigned(info[3]) << 8) | info[4];
                        if (unit) { wchar_t text[16]{}; swprintf_s(text, ARRAYSIZE(text), L"%08X", unit); slot.device.unit = text; slot.device.key = L"hidpp:" + slot.device.unit; }
                    }
                }
            }
            if (slot.feature) {
                auto reading = slot.device;
                if (existing != slots.end()) {
                    // A failed probe must never erase the last working device.
                    slot.device = existing->device;
                    slot.presence = existing->presence;
                    UpdateHidPresence(slot.device, DeviceShouldBeShown(reading) ? &reading : nullptr, slot.presence, GetTickCount64());
                    *existing = std::move(slot);
                } else {
                    slot.device.online = slot.device.charging = false;
                    slot.device.percent = -1;
                    UpdateHidPresence(slot.device, DeviceShouldBeShown(reading) ? &reading : nullptr, slot.presence, GetTickCount64());
                    slots.push_back(std::move(slot));
                }
                if (changed) changed();
            }
        }
    }
    void Poll(DWORD fallback = 30000, bool force = false) {
        for (auto& slot : slots) {
            if (!force && GetTickCount64() < slot.nextPoll) continue;
            auto previous = slot.device;
            auto answer = slot.feature < 0x1000 ? Request(slot.slot, 0x81, static_cast<BYTE>(slot.feature), {}, true)
                : Request(slot.slot, slot.index, slot.feature == 0x1004 ? 1 : 0, {}, false, slot.feature);
            if (slot.feature == 0x1F20 && answer.empty() && !lastError && Healthy())
                answer = Request(slot.slot, slot.index, 0, {}, false, slot.feature);
            auto reading = slot.device;
            bool valid = DecodeHidBattery(reading, slot.feature, answer);
            UpdateHidPresence(slot.device, valid ? &reading : nullptr, slot.presence, GetTickCount64(),
                              IsConfirmedHidAbsence(slot.feature, answer));
            if (!valid) {
                if (lastError && lastError != 4 && lastError != 9) slot.discoverAgain = true;
            }
            slot.nextPoll = GetTickCount64() + HidPollDelay(slot.feature,
                (DeviceShouldBeShown(slot.device) && slot.presence.unavailableSince) ||
                (!DeviceShouldBeShown(slot.device) && slot.presence.availableSince), fallback);
            if (!SameDeviceState(previous, slot.device) && changed) changed();
        }
    }
    ULONGLONG NextPoll() const {
        ULONGLONG due = std::numeric_limits<ULONGLONG>::max();
        for (auto& slot : slots) due = std::min(due, slot.nextPoll);
        return due;
    }
    void Pump() {
        for (auto* endpoint : {longEndpoint.get(), shortEndpoint.get()}) {
            if (!endpoint) continue;
            for (int i = 0; i < 32; ++i) {
                auto packet = endpoint->TakeRead();
                if (packet.empty()) break;
                Notification(packet);
            }
        }
    }
    std::vector<DeviceBattery> Snapshot() const {
        std::vector<DeviceBattery> result;
        for (const auto& slot : slots) result.push_back(slot.device);
        return result;
    }
};

class HidProvider {
    HANDLE cancel;
    std::vector<std::unique_ptr<HidChannel>> channels;
public:
    std::function<void()> changed;
    explicit HidProvider(HANDLE stop) : cancel(stop) {}
    void Discover() {
        auto collections = EnumerateLogitechHid();
        std::erase_if(channels, [&](auto& channel) {
            return !channel->Healthy() || std::none_of(collections.begin(), collections.end(), [&](auto& c) { return c.key == channel->collection.key; });
        });
        for (auto& collection : collections) {
            if (WaitForSingleObject(cancel, 0) == WAIT_OBJECT_0) break;
            auto existing = std::find_if(channels.begin(), channels.end(), [&](auto& c) { return c->collection.key == collection.key; });
            if (existing == channels.end()) {
                auto channel = std::make_unique<HidChannel>(collection, cancel);
                if (!channel->Open()) continue;
                channel->changed = [this] { if (changed) changed(); };
                channels.push_back(std::move(channel));
                existing = channels.end() - 1;
            }
            (*existing)->Discover();
        }
        if (changed) changed();
    }
    void Poll(DWORD fallback = 30000, bool force = false) { for (auto& channel : channels) channel->Poll(fallback, force); }
    ULONGLONG NextPoll() const {
        ULONGLONG due = std::numeric_limits<ULONGLONG>::max();
        for (auto& channel : channels) due = std::min(due, channel->NextPoll());
        return due;
    }
    void Pump() { for (auto& channel : channels) channel->Pump(); }
    bool NeedsDiscovery() const {
        return std::any_of(channels.begin(), channels.end(), [](auto& channel) { return channel->NeedsDiscovery(); });
    }
    bool Wait(DWORD timeout, HANDLE refresh = nullptr) const {
        std::vector<HANDLE> events{cancel};
        if (refresh) events.push_back(refresh);
        for (const auto& channel : channels) channel->AppendWaitHandles(events);
        // Very large receiver collections fall back to bounded pumping.
        if (events.size() > MAXIMUM_WAIT_OBJECTS) {
            events.resize(refresh ? 2 : 1);
            timeout = std::min<DWORD>(timeout, 100);
        }
        DWORD result = WaitForMultipleObjects(static_cast<DWORD>(events.size()), events.data(), FALSE, timeout);
        return result != WAIT_OBJECT_0 && result != WAIT_FAILED;
    }
    std::vector<DeviceBattery> Snapshot() const {
        std::vector<DeviceBattery> result;
        for (const auto& channel : channels) for (auto device : channel->Snapshot()) {
            auto same = std::find_if(result.begin(), result.end(), [&](auto& d) { return d.key == device.key; });
            if (same == result.end()) result.push_back(std::move(device));
            else if (device.online && (!same->online || device.observedAt > same->observedAt)) *same = std::move(device);
        }
        return result;
    }
};

static bool SameLogitechDevice(const DeviceBattery& a, const DeviceBattery& b) {
    if (!a.unit.empty() && !b.unit.empty()) return a.unit == b.unit;
    auto nameA = Lower(a.name), nameB = Lower(b.name);
    if (nameA.size() < 4 || nameB.size() < 4 || nameA == L"logitech" || nameB == L"logitech") return false;
    return nameA == nameB || nameA.find(nameB) != std::wstring::npos || nameB.find(nameA) != std::wstring::npos;
}

static BatterySnapshot CombineLogitechProviders(BatterySnapshot hub, const std::vector<DeviceBattery>& hid) {
    for (const auto& device : hid) {
        auto same = std::find_if(hub.devices.begin(), hub.devices.end(), [&](auto& d) { return SameLogitechDevice(d, device); });
        if (same == hub.devices.end()) hub.devices.push_back(device);
        else if ((!hub.hubAvailable || (!same->online && !same->charging)) && device.online) {
            auto key = same->key, model = same->model;
            *same = device;
            same->key = key;
            same->model = model;
        }
    }
    std::stable_sort(hub.devices.begin(), hub.devices.end(), [](auto& a, auto& b) { return a.type == L"MOUSE" && b.type != L"MOUSE"; });
    return hub;
}

struct TooltipStrings {
    PCWSTR language, charging, charged, error, low, unavailable, remaining;
};

static constexpr TooltipStrings kTooltipLanguages[] = {
    {L"en", L"Charging", L"Fully charged", L"Charging error", L"Low battery", L"Battery unavailable", L"Remaining"},
    {L"ru", L"Заряжается", L"Полностью заряжено", L"Ошибка зарядки", L"Низкий заряд", L"Заряд недоступен", L"Осталось"},
    {L"uk", L"Заряджається", L"Повністю заряджено", L"Помилка заряджання", L"Низький заряд", L"Заряд недоступний", L"Залишилося"},
    {L"de", L"Wird geladen", L"Vollständig geladen", L"Ladefehler", L"Niedriger Akkustand", L"Akkustand nicht verfügbar", L"Verbleibend"},
    {L"fr", L"En charge", L"Charge complète", L"Erreur de charge", L"Batterie faible", L"Batterie indisponible", L"Temps restant"},
    {L"es", L"Cargando", L"Carga completa", L"Error de carga", L"Batería baja", L"Batería no disponible", L"Tiempo restante"},
    {L"it", L"In carica", L"Carica completa", L"Errore di ricarica", L"Batteria scarica", L"Batteria non disponibile", L"Tempo rimanente"},
    {L"pt", L"A carregar", L"Carga completa", L"Erro de carregamento", L"Bateria fraca", L"Bateria indisponível", L"Tempo restante"},
    {L"nl", L"Opladen", L"Volledig opgeladen", L"Oplaadfout", L"Batterij bijna leeg", L"Batterij niet beschikbaar", L"Resterend"},
    {L"pl", L"Ładowanie", L"W pełni naładowane", L"Błąd ładowania", L"Niski poziom baterii", L"Bateria niedostępna", L"Pozostało"},
    {L"cs", L"Nabíjení", L"Plně nabito", L"Chyba nabíjení", L"Nízký stav baterie", L"Baterie není dostupná", L"Zbývá"},
    {L"sk", L"Nabíjanie", L"Úplne nabité", L"Chyba nabíjania", L"Nízky stav batérie", L"Batéria nie je dostupná", L"Zostáva"},
    {L"tr", L"Şarj oluyor", L"Tam şarj oldu", L"Şarj hatası", L"Düşük pil", L"Pil bilgisi yok", L"Kalan süre"},
    {L"sv", L"Laddar", L"Fulladdad", L"Laddningsfel", L"Låg batterinivå", L"Batteri ej tillgängligt", L"Återstående tid"},
    {L"da", L"Oplader", L"Fuldt opladet", L"Opladningsfejl", L"Lavt batteriniveau", L"Batteri ikke tilgængeligt", L"Resterende tid"},
    {L"nb", L"Lader", L"Fulladet", L"Ladefeil", L"Lavt batterinivå", L"Batteri utilgjengelig", L"Gjenstående tid"},
    {L"fi", L"Ladataan", L"Täyteen ladattu", L"Latausvirhe", L"Akun varaus vähissä", L"Akun tietoja ei saatavilla", L"Aikaa jäljellä"},
    {L"ro", L"Se încarcă", L"Încărcare completă", L"Eroare de încărcare", L"Baterie descărcată", L"Baterie indisponibilă", L"Timp rămas"},
    {L"hu", L"Töltés", L"Teljesen feltöltve", L"Töltési hiba", L"Alacsony töltöttség", L"Az akkumulátor nem elérhető", L"Hátralévő idő"},
    {L"el", L"Φόρτιση", L"Πλήρης φόρτιση", L"Σφάλμα φόρτισης", L"Χαμηλή μπαταρία", L"Μη διαθέσιμη μπαταρία", L"Υπολειπόμενος χρόνος"},
    {L"bg", L"Зареждане", L"Напълно заредено", L"Грешка при зареждане", L"Нисък заряд", L"Няма данни за батерията", L"Оставащо време"},
    {L"ja", L"充電中", L"充電完了", L"充電エラー", L"バッテリー残量低下", L"バッテリー情報なし", L"残り時間"},
    {L"ko", L"충전 중", L"충전 완료", L"충전 오류", L"배터리 부족", L"배터리 정보 없음", L"남은 시간"},
    {L"zh-hans", L"正在充电", L"已充满", L"充电错误", L"电量低", L"电池信息不可用", L"剩余时间"},
    {L"zh-hant", L"正在充電", L"已充滿", L"充電錯誤", L"電量低", L"無電池資訊", L"剩餘時間"},
    {L"ar", L"جارٍ الشحن", L"مكتملة الشحن", L"خطأ في الشحن", L"البطارية منخفضة", L"معلومات البطارية غير متاحة", L"الوقت المتبقي"},
    {L"he", L"בטעינה", L"טעינה מלאה", L"שגיאת טעינה", L"סוללה חלשה", L"מידע הסוללה אינו זמין", L"זמן שנותר"},
    {L"hi", L"चार्ज हो रहा है", L"पूरी तरह चार्ज", L"चार्जिंग त्रुटि", L"बैटरी कम है", L"बैटरी की जानकारी उपलब्ध नहीं", L"शेष समय"},
    {L"id", L"Mengisi daya", L"Terisi penuh", L"Kesalahan pengisian daya", L"Baterai lemah", L"Baterai tidak tersedia", L"Waktu tersisa"},
    {L"vi", L"Đang sạc", L"Đã sạc đầy", L"Lỗi sạc", L"Pin yếu", L"Không có thông tin pin", L"Thời gian còn lại"},
    {L"th", L"กำลังชาร์จ", L"ชาร์จเต็มแล้ว", L"ข้อผิดพลาดในการชาร์จ", L"แบตเตอรี่ต่ำ", L"ไม่มีข้อมูลแบตเตอรี่", L"เวลาที่เหลือ"},
};

static const TooltipStrings* FindTooltipLanguage(std::wstring locale) {
    locale = Lower(std::move(locale));
    std::replace(locale.begin(), locale.end(), L'_', L'-');
    if (locale == L"zh" || locale.starts_with(L"zh-")) {
        locale = locale.starts_with(L"zh-hant") || locale == L"zh-tw" ||
            locale == L"zh-hk" || locale == L"zh-mo" ? L"zh-hant" : L"zh-hans";
    } else {
        locale = locale.substr(0, locale.find(L'-'));
        if (locale == L"no") locale = L"nb";
    }
    for (const auto& strings : kTooltipLanguages) if (locale == strings.language) return &strings;
    return nullptr;
}

static const TooltipStrings* g_tooltipStrings = &kTooltipLanguages[0];
static std::wstring g_tooltipLocale = L"en-US";

static void LoadTooltipLanguage() {
    ULONG count = 0, length = 0;
    GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &count, nullptr, &length);
    if (length && length <= 65536) {
        std::vector<wchar_t> buffer(length);
        if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &count, buffer.data(), &length)) {
            for (size_t pos = 0; pos < buffer.size() && buffer[pos];) {
                std::wstring locale(buffer.data() + pos);
                if (const auto* strings = FindTooltipLanguage(locale)) {
                    g_tooltipStrings = strings;
                    g_tooltipLocale = locale;
                    return;
                }
                pos += locale.size() + 1;
            }
        }
    }
    wchar_t locale[LOCALE_NAME_MAX_LENGTH]{};
    if (LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT), locale, ARRAYSIZE(locale), 0)) {
        if (const auto* strings = FindTooltipLanguage(locale)) {
            g_tooltipStrings = strings;
            g_tooltipLocale = locale;
        }
    }
}

static std::wstring FormatBatteryTooltip(const DeviceBattery& device,
        const TooltipStrings& strings, PCWSTR locale) {
    std::wstring text = device.name + L"\n";
    if (!DeviceShouldBeShown(device)) return text + strings.unavailable;
    bool rtl = std::wstring_view(strings.language) == L"ar" || std::wstring_view(strings.language) == L"he";
    if (rtl) text += L"\u2068";
    text += std::wstring(device.approximate ? L"≈" : L"") + std::to_wstring(device.percent) + L"%";
    if (rtl) text += L"\u2069";
    PCWSTR status = nullptr;
    if (device.chargingError) status = strings.error;
    else if (device.charging) status = device.fullyCharged ? strings.charged : strings.charging;
    else if (device.fullyCharged || device.percent == 100) status = strings.charged;
    else if (device.critical || device.percent <= 10) status = strings.low;
    if (status) text += std::wstring(L" · ") + status;
    if (!device.charging && std::isfinite(device.hours) && device.hours > 0 && device.hours < 10000) {
        wchar_t duration[128]{};
        auto minutes = static_cast<ULONGLONG>(std::llround(device.hours * 60));
        if (!GetDurationFormatEx(locale, 0, nullptr, minutes * 600000000ULL,
                                nullptr, duration, ARRAYSIZE(duration))) {
            swprintf_s(duration, L"%llu:%02llu", minutes / 60, minutes % 60);
        }
        text += L"\n" + std::wstring(strings.remaining) + L": ≈ ";
        if (rtl) text += L"\u2068";
        text += duration;
        if (rtl) text += L"\u2069";
    }
    return text;
}

struct BatteryButtonSize {
    double width, height, contentWidth;
};

static BatteryButtonSize SizeBatteryButton(bool vertical, double trayWidth, double trayHeight,
                                           double naturalWidth, double naturalHeight, double preferredWidth) {
    if (vertical) {
        double width = std::max(1.0, trayWidth - 4);
        double contentWidth = std::max(1.0, width - 8);
        double scale = std::min(1.0, contentWidth / std::max(1.0, naturalWidth));
        return {width, std::max(32.0, std::ceil(naturalHeight * scale + 8)), contentWidth};
    }
    double width = std::max(preferredWidth, naturalWidth + 12);
    return {width, std::clamp(trayHeight - 8, 24.0, 36.0), std::max(1.0, width - 12)};
}

struct Settings {
    bool nativeOnly = false;
    std::wstring position = L"batteryArea";
    std::wstring iconStyle = L"batteryAndDevice";
    int pollSeconds = 30;
    bool directHidpp = true;
    bool showNewDevices = true;
    std::vector<DeviceRule> rules;
    bool showPercentage = true;
    bool showBatteryBar = false;
    int buttonWidth = 66;
    int iconSize = 16;
    int lowBatteryPercent = 20;
    bool openGHubOnClick = true;
};
static Settings g_settings;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_hubRefresh = nullptr;
static HANDLE g_hidRefresh = nullptr;
static HANDLE g_eventRefresh = nullptr;
static HANDLE g_nativeRefresh = nullptr;
static HANDLE g_worker = nullptr;
static HANDLE g_eventWorker = nullptr;
static std::atomic<bool> g_unloading{false};
static std::atomic<HWND> g_taskbar{nullptr};
static UINT_PTR g_timerId = 0;
static const UINT g_uiMessage = RegisterWindowMessageW(L"Windhawk_LogitechBatteryTray_UI_" WH_MOD_ID);
static std::mutex g_snapshotMutex;
static BatterySnapshot g_snapshot;
static std::vector<DeviceBattery> g_hidSnapshot;
static HANDLE g_hidWorker = nullptr;
static HANDLE g_nativeWorker = nullptr;
static std::atomic<bool> g_modernAttached{false};
static std::atomic<ULONGLONG> g_detachedSince{0};

static void NotifyConsumers() {
    if (g_unloading) return;
    if (auto window = g_taskbar.load()) PostMessageW(window, g_uiMessage, 0, 0);
    if (g_nativeRefresh) SetEvent(g_nativeRefresh);
}

static void SetModernAttached(bool attached) {
    bool previous = g_modernAttached.exchange(attached);
    if (attached) g_detachedSince = 0;
    else if (!g_detachedSince.load()) g_detachedSince = GetTickCount64();
    if (previous != attached && g_nativeRefresh) SetEvent(g_nativeRefresh);
}

struct DeviceView {
    Button button{nullptr};
    FontIcon icon{nullptr};
    FontIcon charging{nullptr};
    FontIcon deviceIcon{nullptr};
    TextBlock percent{nullptr};
    TextBlock tooltipText{nullptr};
    ToolTip tooltip{nullptr};
    Border meter{nullptr};
    Border meterTrack{nullptr};
    Grid iconContainer{nullptr};
    Viewbox contentBox{nullptr};
    winrt::event_token click{};
};
[[clang::no_destroy]] static std::optional<std::vector<DeviceView>> g_views;
[[clang::no_destroy]] static StackPanel g_group{nullptr};
[[clang::no_destroy]] static Panel g_parent{nullptr};
[[clang::no_destroy]] static RowDefinition g_insertedRow{nullptr};
[[clang::no_destroy]] static ColumnDefinition g_insertedColumn{nullptr};
static bool g_verticalLayout = false;
static bool g_visualDirty = true;
static winrt::event_token g_unloadedToken{}, g_sizeToken{}, g_themeToken{};
static std::vector<DeviceBattery> g_renderedDevices;
static double g_renderedWidth = -1, g_renderedHeight = -1;
static ElementTheme g_renderedTheme = ElementTheme::Default;

struct TrayPalette {
    Brush foreground{nullptr}, transparent{nullptr}, hover{nullptr}, pressed{nullptr},
        green{nullptr}, critical{nullptr}, low{nullptr};
};
[[clang::no_destroy]] static std::optional<TrayPalette> g_palette;

static void ReportUIStage(int stage, PCWSTR message) {
    if (stage != 40) SetModernAttached(false);
    static int previous = -1;
    if (previous == stage) return;
    previous = stage;
    Wh_Log(L"Logitech Battery Tray: %s", message);
}

static void LoadSettings() {
    LoadTooltipLanguage();
    auto mode = WindhawkUtils::StringSetting::make(L"displayMode");
    g_settings.nativeOnly = std::wstring_view(mode.get()) == L"native";
    auto position = WindhawkUtils::StringSetting::make(L"placement");
    g_settings.position = *position.get() ? position.get() : L"batteryArea";
    if (g_settings.position != L"beforeOmni" && g_settings.position != L"beforeClock" && g_settings.position != L"beforeIcons")
        g_settings.position = L"batteryArea";
    auto style = WindhawkUtils::StringSetting::make(L"iconStyle");
    g_settings.iconStyle = *style.get() ? style.get() : L"batteryAndDevice";
    g_settings.pollSeconds = std::clamp(Wh_GetIntSetting(L"refreshSeconds"), 30, 300);
    g_settings.directHidpp = Wh_GetIntSetting(L"directHidpp") != 0;
    g_settings.showNewDevices = Wh_GetIntSetting(L"showNewDevices") != 0;
    g_settings.rules.clear();
    for (int i = 0; i < 256; ++i) {
        auto match = WindhawkUtils::StringSetting::make(L"devices[%d].match", i);
        bool empty = !*match.get();
        if (!empty) g_settings.rules.push_back({match.get(), Wh_GetIntSetting(L"devices[%d].show", i) != 0});
    }
    g_settings.showPercentage = Wh_GetIntSetting(L"showPercentage") != 0;
    g_settings.showBatteryBar = Wh_GetIntSetting(L"showBatteryBar") != 0;
    g_settings.buttonWidth = std::clamp(Wh_GetIntSetting(L"buttonWidth"), 58, 120);
    g_settings.iconSize = std::clamp(Wh_GetIntSetting(L"iconSize"), 12, 24);
    g_settings.lowBatteryPercent = std::clamp(Wh_GetIntSetting(L"lowBatteryPercent"), 10, 50);
    g_settings.openGHubOnClick = Wh_GetIntSetting(L"openGHubOnClick") != 0;
}

// XAML root access follows the existing Windhawk Taskbar Folder Menus mod.
using GetTaskbarHost_t = void* (WINAPI*)(void*, void*);
using FrameHeight_t = int (WINAPI*)(void*);
using Decref_t = void (WINAPI*)(void*);
static GetTaskbarHost_t g_getTaskbarHost = nullptr;
static FrameHeight_t g_frameHeight = nullptr;
static Decref_t g_decref = nullptr;
static void* g_taskBandSiteVtable = nullptr;
static HMODULE g_taskbarModule = nullptr;

static bool ReadableRange(const void* address, size_t length) {
    MEMORY_BASIC_INFORMATION info{};
    if (!address || !VirtualQuery(address, &info, sizeof(info)) || info.State != MEM_COMMIT ||
        (info.Protect & (PAGE_GUARD | PAGE_NOACCESS))) return false;
    auto start = reinterpret_cast<uintptr_t>(address);
    auto base = reinterpret_cast<uintptr_t>(info.BaseAddress);
    return start >= base && start - base <= info.RegionSize && length <= info.RegionSize - (start - base);
}

static XamlRoot GetTaskbarXamlRoot(HWND window) {
    auto taskBandWindow = reinterpret_cast<HWND>(GetPropW(window, L"TaskbandHWND"));
    if (!taskBandWindow) return nullptr;
    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskBandWindow, 0));
    if (!taskBand) return nullptr;
    void* site = taskBand;
    for (int i = 0;; ++i) {
        if (!ReadableRange(site, sizeof(void*))) return nullptr;
        if (*static_cast<void**>(site) == g_taskBandSiteVtable) break;
        if (i == 20) return nullptr;
        site = static_cast<void**>(site) + 1;
    }
    void* host[2]{};
    g_getTaskbarHost(site, host);
    struct HostRelease {
        void* control;
        ~HostRelease() { if (control) g_decref(control); }
    } release{host[1]};
    if (!host[0] || !host[1]) {
        return nullptr;
    }
    // Fail closed on an unrecognized instruction layout after a Windows update.
    auto* code = reinterpret_cast<const BYTE*>(g_frameHeight);
#if defined(_M_ARM64) || defined(__aarch64__)
    constexpr bool arm64 = true;
#else
    constexpr bool arm64 = false;
#endif
    constexpr size_t codeLength = arm64 ? 16 : 8;
    size_t offset = 0;
    if (!ReadableRange(code, codeLength) || !DecodeHostOffset(code, codeLength, arm64, offset)) return nullptr;
    auto field = static_cast<BYTE*>(host[0]) + offset;
    if (!ReadableRange(field, sizeof(IUnknown*))) return nullptr;
    auto* unknown = *reinterpret_cast<IUnknown**>(field);
    FrameworkElement element{nullptr};
    if (unknown) unknown->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
    auto root = element ? element.XamlRoot() : nullptr;
    return root;
}

static FrameworkElement FindChild(const FrameworkElement& parent,
        const std::function<bool(const FrameworkElement&)>& match, int depth = 24) {
    if (!parent || depth <= 0) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (match(child)) return child;
        if (auto found = FindChild(child, match, depth - 1)) return found;
    }
    return nullptr;
}

static HWND FindTaskbar() {
    auto window = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD owner = 0;
    if (window) GetWindowThreadProcessId(window, &owner);
    return owner == GetCurrentProcessId() ? window : nullptr;
}

static Brush BrushFromColor(BYTE alpha, BYTE red, BYTE green, BYTE blue) {
    return SolidColorBrush(Color{alpha, red, green, blue});
}

static Brush NativeForeground(const Panel& parent) {
    // Respect Taskbar Styler and the actual tray foreground when it is available.
    for (auto child : parent.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (!element || element == g_group) continue;
        auto label = FindChild(element, [](auto& candidate) { return !!candidate.template try_as<TextBlock>(); });
        if (label) {
            auto brush = label.as<TextBlock>().Foreground();
            if (brush) return brush;
        }
    }
    return parent.ActualTheme() == ElementTheme::Light
        ? BrushFromColor(255, 26, 26, 26) : BrushFromColor(255, 245, 245, 245);
}

static void PutBrush(Button button, PCWSTR key, Brush brush) {
    button.Resources().Insert(winrt::box_value(winrt::hstring(key)), brush);
}

static void ClearViews() {
    if (g_views) for (auto& view : *g_views) {
        try {
            view.button.Click(view.click);
            ToolTipService::SetToolTip(view.button, nullptr);
            view.tooltip.IsOpen(false);
            view.tooltip.Content(nullptr);
            view.button.Content(nullptr);
        } catch (...) {}
    }
    g_views.reset();
    if (g_group) g_group.Children().Clear();
}

static int TrayCell(const FrameworkElement& element, bool vertical) {
    return vertical ? Grid::GetRow(element) : Grid::GetColumn(element);
}

static int TraySpan(const FrameworkElement& element, bool vertical) {
    return vertical ? Grid::GetRowSpan(element) : Grid::GetColumnSpan(element);
}

static void SetTrayCell(const FrameworkElement& element, bool vertical, int cell) {
    if (vertical) Grid::SetRow(element, cell);
    else Grid::SetColumn(element, cell);
}

static void SetTraySpan(const FrameworkElement& element, bool vertical, int span) {
    if (vertical) Grid::SetRowSpan(element, span);
    else Grid::SetColumnSpan(element, span);
}

static void RemoveGroup() {
    SetModernAttached(false);
    if (g_unloadedToken.value) g_group.Unloaded(g_unloadedToken);
    if (g_sizeToken.value) g_parent.SizeChanged(g_sizeToken);
    if (g_themeToken.value) g_parent.ActualThemeChanged(g_themeToken);
    g_unloadedToken = g_sizeToken = g_themeToken = {};
    ClearViews();
    if (g_parent) {
        uint32_t index = 0;
        if (g_group && g_parent.Children().IndexOf(g_group, index)) {
            g_parent.Children().RemoveAt(index);
        }
        if (auto grid = g_parent.try_as<Grid>()) {
            uint32_t track = 0;
            bool vertical = !!g_insertedRow;
            bool owned = vertical ? grid.RowDefinitions().IndexOf(g_insertedRow, track)
                : g_insertedColumn && grid.ColumnDefinitions().IndexOf(g_insertedColumn, track);
            if (owned) {
                if (vertical) grid.RowDefinitions().RemoveAt(track);
                else grid.ColumnDefinitions().RemoveAt(track);
                for (auto child : grid.Children()) {
                    auto element = child.try_as<FrameworkElement>();
                    if (!element) continue;
                    int cell = TrayCell(element, vertical), span = TraySpan(element, vertical);
                    if (cell > static_cast<int>(track)) SetTrayCell(element, vertical, cell - 1);
                    else if (cell < static_cast<int>(track) && cell + span > static_cast<int>(track))
                        SetTraySpan(element, vertical, span - 1);
                }
            }
        }
    }
    g_group = nullptr;
    g_parent = nullptr;
    g_insertedRow = nullptr;
    g_insertedColumn = nullptr;
    g_palette.reset();
    g_renderedDevices.clear();
    g_visualDirty = true;
}

static DeviceView MakeDeviceView() {
    DeviceView view;
    view.button = Button();
    view.button.MinWidth(0);
    view.button.MinHeight(0);
    view.button.Padding({6, 0, 6, 0});
    view.button.BorderThickness({0, 0, 0, 0});
    view.button.CornerRadius({4, 4, 4, 4});
    view.button.VerticalAlignment(VerticalAlignment::Center);
    view.button.HorizontalContentAlignment(HorizontalAlignment::Center);
    view.button.VerticalContentAlignment(VerticalAlignment::Center);
    view.button.RequestedTheme(g_parent.ActualTheme());

    StackPanel content;
    content.Orientation(g_verticalLayout ? Orientation::Vertical : Orientation::Horizontal);
    content.Spacing(g_verticalLayout ? 2 : 5);
    content.VerticalAlignment(VerticalAlignment::Center);
    content.HorizontalAlignment(HorizontalAlignment::Center);
    StackPanel symbols;
    symbols.Orientation(Orientation::Horizontal);
    symbols.Spacing(5);
    symbols.HorizontalAlignment(HorizontalAlignment::Center);
    if (g_verticalLayout) content.Children().Append(symbols);
    auto iconParent = g_verticalLayout ? symbols : content;
    view.deviceIcon = FontIcon();
    view.deviceIcon.FontFamily(FontFamily(L"Segoe Fluent Icons"));
    view.deviceIcon.FontSize(12);
    view.deviceIcon.VerticalAlignment(VerticalAlignment::Center);
    view.deviceIcon.Opacity(0.7);
    view.deviceIcon.Visibility(g_settings.iconStyle == L"batteryAndDevice" ? Visibility::Visible : Visibility::Collapsed);
    iconParent.Children().Append(view.deviceIcon);

    view.iconContainer = Grid();
    view.iconContainer.Width(g_settings.iconSize);
    view.iconContainer.Height(g_settings.iconSize + (g_settings.showBatteryBar ? 5 : 0));
    view.icon = FontIcon();
    view.icon.FontFamily(FontFamily(L"Segoe Fluent Icons"));
    view.icon.FontSize(g_settings.iconSize);
    view.icon.VerticalAlignment(VerticalAlignment::Top);
    view.iconContainer.Children().Append(view.icon);
    view.meterTrack = Border();
    view.meterTrack.Height(2);
    view.meterTrack.CornerRadius({1, 1, 1, 1});
    view.meterTrack.VerticalAlignment(VerticalAlignment::Bottom);
    view.meter = Border();
    view.meter.Height(2);
    view.meter.CornerRadius({1, 1, 1, 1});
    view.meter.HorizontalAlignment(HorizontalAlignment::Left);
    view.meter.VerticalAlignment(VerticalAlignment::Bottom);
    view.iconContainer.Children().Append(view.meterTrack);
    view.iconContainer.Children().Append(view.meter);
    iconParent.Children().Append(view.iconContainer);
    view.percent = TextBlock();
    view.percent.FontFamily(FontFamily(L"Segoe UI"));
    view.percent.FontSize(12);
    view.percent.VerticalAlignment(VerticalAlignment::Center);
    view.percent.HorizontalAlignment(HorizontalAlignment::Center);
    view.percent.Visibility(g_settings.showPercentage ? Visibility::Visible : Visibility::Collapsed);
    content.Children().Append(view.percent);
    view.charging = FontIcon();
    view.charging.FontFamily(FontFamily(L"Segoe Fluent Icons"));
    view.charging.Glyph(L"\uE945");
    view.charging.FontSize(9);
    view.charging.Margin({-2, 0, 0, 0});
    view.charging.VerticalAlignment(VerticalAlignment::Center);
    iconParent.Children().Append(view.charging);
    view.contentBox = Viewbox();
    view.contentBox.Stretch(Stretch::Uniform);
    view.contentBox.StretchDirection(StretchDirection::DownOnly);
    view.contentBox.Child(content);
    view.button.Content(view.contentBox);

    view.tooltipText = TextBlock();
    view.tooltipText.FontFamily(FontFamily(L"Segoe UI"));
    view.tooltipText.FontSize(12);
    view.tooltipText.TextWrapping(TextWrapping::Wrap);
    view.tooltipText.MaxWidth(360);
    view.tooltip = ToolTip();
    view.tooltip.Content(view.tooltipText);
    view.tooltip.Placement(Primitives::PlacementMode::Top);
    ToolTipService::SetToolTip(view.button, view.tooltip);
    view.click = view.button.Click([](auto& sender, auto&) {
        if (g_unloading || !g_settings.openGHubOnClick) return;
        WCHAR programFiles[MAX_PATH]{};
        if (!GetEnvironmentVariableW(L"ProgramW6432", programFiles, ARRAYSIZE(programFiles)))
            GetEnvironmentVariableW(L"ProgramFiles", programFiles, ARRAYSIZE(programFiles));
        std::wstring path = std::wstring(programFiles) + L"\\LGHUB\\lghub.exe";
        if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES)
            ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    });
    return view;
}

static void UpdateUI() {
    if (g_unloading) return;
    try {
        HWND window = g_taskbar.load();
        if (!window) return;
        auto root = g_parent ? g_parent.XamlRoot() : GetTaskbarXamlRoot(window);
        if (!root) {
            ReportUIStage(21, L"Waiting for the taskbar XAML root");
            return;
        }
        auto parent = g_parent ? g_parent : FindChild(root.Content().try_as<FrameworkElement>(), [](auto& element) {
            return element.Name() == L"SystemTrayFrameGrid";
        }).try_as<Panel>();
        if (!parent) {
            ReportUIStage(22, L"SystemTrayFrameGrid was not found as a Panel");
            return;
        }
        auto parentGrid = parent.try_as<Grid>();
        auto parentStack = parent.try_as<StackPanel>();
        if (!parentGrid && !parentStack) {
            ReportUIStage(24, L"Unsupported tray container type");
            return;
        }
        if (parent.ActualHeight() < 1) {
            ReportUIStage(23, L"Waiting for tray layout");
            return;
        }
        RECT taskbarRect{};
        if (!GetWindowRect(window, &taskbarRect)) return;
        bool vertical = taskbarRect.bottom - taskbarRect.top > taskbarRect.right - taskbarRect.left;
        if (parent.ActualWidth() < 1) return;
        if (g_group && g_verticalLayout != vertical) RemoveGroup();
        if (g_parent && g_parent != parent) RemoveGroup();
        uint32_t index = 0;
        if (g_group && !parent.Children().IndexOf(g_group, index)) RemoveGroup();
        if (!g_group) {
            g_verticalLayout = vertical;
            int insertCell = 0;
            uint32_t insertIndex = 0;
            PCWSTR anchor = (g_settings.position == L"beforeOmni" || g_settings.position == L"batteryArea") ? L"ControlCenterButton"
                           : g_settings.position == L"beforeClock" ? L"NotificationCenterButton" : nullptr;
            PCWSTR findAnchor = anchor ? anchor : (parentStack ? L"NotifyIconStack" : nullptr);
            if (findAnchor) {
                bool found = false;
                for (uint32_t childIndex = 0; childIndex < parent.Children().Size(); ++childIndex) {
                    auto child = parent.Children().GetAt(childIndex);
                    auto element = child.try_as<FrameworkElement>();
                    if (element && element.Name() == findAnchor) {
                        if (parentGrid) insertCell = TrayCell(element, vertical);
                        insertIndex = childIndex;
                        if (g_settings.position == L"batteryArea") {
                            ++insertIndex;
                            if (parentGrid) insertCell += TraySpan(element, vertical);
                        }
                        found = true;
                        break;
                    }
                }
                if (!found && anchor) {
                    ReportUIStage(25, L"Waiting for the selected tray anchor");
                    return;
                }
            }
            if (parentGrid) {
                if (vertical) {
                    if (static_cast<uint32_t>(insertCell) > parentGrid.RowDefinitions().Size()) return;
                    g_insertedRow = RowDefinition();
                    g_insertedRow.Height({1, GridUnitType::Auto});
                    parentGrid.RowDefinitions().InsertAt(insertCell, g_insertedRow);
                } else {
                    if (static_cast<uint32_t>(insertCell) > parentGrid.ColumnDefinitions().Size()) return;
                    g_insertedColumn = ColumnDefinition();
                    g_insertedColumn.Width({1, GridUnitType::Auto});
                    parentGrid.ColumnDefinitions().InsertAt(insertCell, g_insertedColumn);
                }
                g_parent = parent;
                for (auto child : parent.Children()) {
                    auto element = child.try_as<FrameworkElement>();
                    if (!element) continue;
                    int cell = TrayCell(element, vertical), span = TraySpan(element, vertical);
                    if (cell >= insertCell) SetTrayCell(element, vertical, cell + 1);
                    else if (cell + span > insertCell) SetTraySpan(element, vertical, span + 1);
                }
            }
            g_parent = parent;
            g_group = StackPanel();
            g_group.Name(L"LogitechBatteryTray");
            g_group.Orientation(vertical ? Orientation::Vertical : Orientation::Horizontal);
            g_group.Spacing(0);
            g_group.Margin(vertical ? Thickness{0, 2, 0, 2} : Thickness{2, 0, 2, 0});
            g_group.VerticalAlignment(VerticalAlignment::Center);
            g_group.HorizontalAlignment(HorizontalAlignment::Center);
            if (parentGrid) {
                SetTrayCell(g_group, vertical, insertCell);
                parent.Children().Append(g_group);
                Wh_Log(L"Inserted into Grid, vertical=%d, cell=%d", vertical, insertCell);
            } else {
                parent.Children().InsertAt(insertIndex, g_group);
                Wh_Log(L"Inserted into StackPanel, index=%u", insertIndex);
            }
            g_unloadedToken = g_group.Unloaded([](auto&, auto&) {
                if (g_unloading) return;
                SetModernAttached(false);
                if (auto hwnd = g_taskbar.load()) PostMessageW(hwnd, g_uiMessage, 2, 0);
            });
            g_sizeToken = g_parent.SizeChanged([](auto&, auto&) {
                if (g_unloading) return;
                g_visualDirty = true;
                if (auto hwnd = g_taskbar.load()) PostMessageW(hwnd, g_uiMessage, 0, 0);
            });
            g_themeToken = g_parent.ActualThemeChanged([](auto&, auto&) {
                if (g_unloading) return;
                g_palette.reset();
                g_visualDirty = true;
                if (auto hwnd = g_taskbar.load()) PostMessageW(hwnd, g_uiMessage, 0, 0);
            });
        }
        BatterySnapshot snapshot;
        {
            std::lock_guard lock(g_snapshotMutex);
            snapshot = CombineLogitechProviders(g_snapshot, g_hidSnapshot);
        }
        auto& devices = snapshot.devices;
        std::erase_if(devices, [](auto& device) { return !DeviceVisible(device, g_settings.rules, g_settings.showNewDevices); });
        std::erase_if(devices, [](const auto& device) { return !DeviceShouldBeShown(device); });
        double trayWidth = std::min(parent.ActualWidth(),
            (taskbarRect.right - taskbarRect.left) / std::max(0.1, root.RasterizationScale()));
        double layoutHeight = vertical ? 0 : parent.ActualHeight();
        auto theme = parent.ActualTheme();
        if (!g_visualDirty && SameDeviceList(devices, g_renderedDevices) &&
            trayWidth == g_renderedWidth && layoutHeight == g_renderedHeight && theme == g_renderedTheme) {
            SetModernAttached(true);
            return;
        }
        g_group.Visibility(devices.empty() ? Visibility::Collapsed : Visibility::Visible);
        if (!g_views || g_views->size() != devices.size()) {
            ClearViews();
            g_views.emplace();
            for (size_t i = 0; i < devices.size(); ++i) {
                g_views->push_back(MakeDeviceView());
                g_group.Children().Append(g_views->back().button);
            }
        }
        if (!g_palette || theme != g_renderedTheme) {
            TrayPalette palette;
            palette.foreground = NativeForeground(parent);
            auto solid = palette.foreground.try_as<SolidColorBrush>();
            Color color = solid ? solid.Color() : Color{255, 245, 245, 245};
            bool dark = int(color.R) + color.G + color.B > 384;
            palette.transparent = BrushFromColor(0, 0, 0, 0);
            palette.hover = dark ? BrushFromColor(18, 255, 255, 255) : BrushFromColor(12, 0, 0, 0);
            palette.pressed = dark ? BrushFromColor(12, 255, 255, 255) : BrushFromColor(8, 0, 0, 0);
            palette.green = dark ? BrushFromColor(255, 108, 203, 95) : BrushFromColor(255, 26, 127, 55);
            palette.critical = dark ? BrushFromColor(255, 255, 153, 164) : BrushFromColor(255, 196, 43, 28);
            palette.low = dark ? BrushFromColor(255, 252, 225, 0) : BrushFromColor(255, 157, 93, 0);
            g_palette = std::move(palette);
        }
        const auto& [foreground, transparent, hover, pressed, green, critical, low] = *g_palette;
        auto placement = Primitives::PlacementMode::Top;
        if (vertical) {
            MONITORINFO monitor{sizeof(monitor)};
            if (GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor))
                placement = taskbarRect.left < (monitor.rcMonitor.left + monitor.rcMonitor.right) / 2
                    ? Primitives::PlacementMode::Right : Primitives::PlacementMode::Left;
        }
        for (size_t i = 0; i < devices.size(); ++i) {
            auto& view = (*g_views)[i];
            auto& device = devices[i];
            Brush tint = foreground;
            bool available = DeviceShouldBeShown(device);
            if (available && !device.charging) {
                if (device.critical || device.percent <= 10)
                    tint = critical;
                else if (device.percent <= g_settings.lowBatteryPercent)
                    tint = low;
            }
            if (available && device.chargingError)
                tint = critical;
            bool batteryStyle = g_settings.iconStyle != L"device";
            bool chargingVisible = available && (device.chargingError || (!batteryStyle && device.charging));
            double iconWidth = g_settings.iconSize + (g_settings.iconStyle == L"batteryAndDevice" ? 17 : 0) + (chargingVisible ? 12 : 0);
            double percentWidth = g_settings.showPercentage ? (device.approximate ? 47 : 38) : 0;
            double naturalWidth = vertical ? std::max(iconWidth, percentWidth) : iconWidth + percentWidth;
            double naturalHeight = g_settings.iconSize + (g_settings.showBatteryBar ? 5 : 0) +
                (vertical && g_settings.showPercentage ? 20 : 0);
            auto size = SizeBatteryButton(vertical, trayWidth, parent.ActualHeight(), naturalWidth,
                naturalHeight, g_settings.showPercentage ? g_settings.buttonWidth : 0);
            view.button.Width(size.width);
            view.button.Height(size.height);
            view.button.Padding(vertical ? Thickness{4, 4, 4, 4} : Thickness{6, 0, 6, 0});
            view.contentBox.MaxWidth(size.contentWidth);
            view.tooltip.Placement(placement);
            view.button.Foreground(foreground);
            view.button.Background(transparent);
            view.button.Opacity(1.0);
            PutBrush(view.button, L"ButtonBackground", transparent);
            PutBrush(view.button, L"ButtonBackgroundPointerOver", hover);
            PutBrush(view.button, L"ButtonBackgroundPressed", pressed);
            PutBrush(view.button, L"ButtonBorderBrush", transparent);
            PutBrush(view.button, L"ButtonBorderBrushPointerOver", transparent);
            PutBrush(view.button, L"ButtonBorderBrushPressed", transparent);
            PutBrush(view.button, L"ButtonForeground", foreground);
            PutBrush(view.button, L"ButtonForegroundPointerOver", foreground);
            PutBrush(view.button, L"ButtonForegroundPressed", foreground);
            view.button.RequestedTheme(parent.ActualTheme());
            view.icon.Glyph(batteryStyle ? BatteryGlyph(device.percent, available && device.charging && !device.chargingError) : DeviceGlyph(device.type));
            view.icon.Foreground(available && device.charging && !device.chargingError ? green : tint);
            view.deviceIcon.Glyph(DeviceGlyph(device.type));
            view.deviceIcon.Foreground(foreground);
            view.percent.Foreground(tint);
            view.percent.Text(available ? std::wstring(device.approximate ? L"≈" : L"") + std::to_wstring(device.percent) + L"%" : L"\u2014");
            view.charging.Glyph(device.chargingError ? L"\uE7BA" : L"\uE945");
            view.charging.Foreground(device.chargingError ? tint : green);
            view.charging.Visibility(chargingVisible ? Visibility::Visible : Visibility::Collapsed);
            view.meterTrack.Visibility(g_settings.showBatteryBar && available ? Visibility::Visible : Visibility::Collapsed);
            view.meter.Visibility(g_settings.showBatteryBar && available ? Visibility::Visible : Visibility::Collapsed);
            view.meterTrack.Background(foreground);
            view.meterTrack.Opacity(0.18);
            view.meter.Background(device.charging ? green : tint);
            view.meter.Opacity(0.75);
            view.meter.Width(available ? g_settings.iconSize * device.percent / 100.0 : 0);
            auto tooltip = FormatBatteryTooltip(device, *g_tooltipStrings, g_tooltipLocale.c_str());
            bool rtl = std::wstring_view(g_tooltipStrings->language) == L"ar" ||
                std::wstring_view(g_tooltipStrings->language) == L"he";
            view.tooltipText.FlowDirection(rtl ? FlowDirection::RightToLeft : FlowDirection::LeftToRight);
            view.tooltipText.Text(tooltip);
            Automation::AutomationProperties::SetName(view.button, tooltip);
        }
        ReportUIStage(40, L"Battery indicators created and updated");
        g_renderedDevices = devices;
        g_renderedWidth = trayWidth;
        g_renderedHeight = layoutHeight;
        g_renderedTheme = theme;
        g_visualDirty = false;
        SetModernAttached(true);
    } catch (const winrt::hresult_error& error) {
        try { RemoveGroup(); } catch (...) {}
        SetModernAttached(false);
        ReportUIStage(26, L"XAML operation failed; see the detailed error in the log");
        Wh_Log(L"Tray is not ready: 0x%08X %s", static_cast<unsigned>(error.code()), error.message().c_str());
    } catch (...) {
        try { RemoveGroup(); } catch (...) {}
        SetModernAttached(false);
        Wh_Log(L"Tray is not ready");
    }
}

// Native notification icons are the compatibility backend, never an overlay.
// This window and all its icons belong to the worker and are removed before unload.
struct NativeTrayState {
    std::vector<DeviceBattery> devices;
    std::vector<std::pair<std::wstring, UINT>> identities;
    int iconSize = 0;
    UINT taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    UINT IdFor(const std::wstring& key) {
        for (const auto& [saved, id] : identities) if (saved == key) return id;
        UINT id = 2166136261U;
        for (wchar_t ch : key) id = (id ^ static_cast<unsigned>(ch)) * 16777619U;
        // Preserve collision assignments until unload, including disconnected devices.
        while (!id || std::any_of(identities.begin(), identities.end(), [&](auto& item) { return item.second == id; })) ++id;
        identities.emplace_back(key, id);
        return id;
    }
};

static DWORD NativeFallbackDelay(bool modernAvailable, bool attached, ULONGLONG detachedAt, ULONGLONG now) {
    if (attached) return INFINITE;
    if (!modernAvailable) return 0;
    if (!detachedAt) return 30000;
    if (now - detachedAt >= 30000) return 0;
    return static_cast<DWORD>(30000 - (now - detachedAt));
}

static int NativeIconSize() {
    HWND taskbar = FindTaskbar();
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto windowDpi = reinterpret_cast<UINT(WINAPI*)(HWND)>(GetProcAddress(user32, "GetDpiForWindow"));
    auto metrics = reinterpret_cast<int(WINAPI*)(int, UINT)>(GetProcAddress(user32, "GetSystemMetricsForDpi"));
    UINT dpi = taskbar && windowDpi ? windowDpi(taskbar) : 0;
    if (!dpi) {
        // The first Windows 10 releases predate GetDpiForWindow.
        HMODULE shcore = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (shcore) {
            auto monitorDpi = reinterpret_cast<HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*)>(GetProcAddress(shcore, "GetDpiForMonitor"));
            UINT y = 96;
            if (monitorDpi) monitorDpi(MonitorFromWindow(taskbar, MONITOR_DEFAULTTOPRIMARY), 0, &dpi, &y);
            FreeLibrary(shcore);
        }
    }
    if (!dpi) dpi = 96;
    return std::max(1, metrics ? metrics(SM_CXSMICON, dpi) : MulDiv(16, dpi, 96));
}

static HICON MakeNativeBatteryIcon(const DeviceBattery& device, int size) {
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = size;
    info.bmiHeader.biHeight = -size;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    HBITMAP color = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    HBITMAP mask = CreateBitmap(size, size, 1, 1, nullptr);
    HDC dc = CreateCompatibleDC(nullptr);
    HFONT font = CreateFontW(-(device.percent == 100 ? size / 2 : size * 3 / 4), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        DEFAULT_PITCH, L"Segoe UI");
    HICON icon = nullptr;
    if (color && mask && dc && font) {
        auto oldBitmap = SelectObject(dc, mask);
        PatBlt(dc, 0, 0, size, size, BLACKNESS);
        SelectObject(dc, color);
        auto data = static_cast<DWORD*>(pixels);
        std::fill_n(data, size * size, 0x00303030);
        auto oldFont = SelectObject(dc, font);
        SetBkMode(dc, TRANSPARENT);
        // An opaque neutral tile keeps the number legible on either taskbar theme.
        SetTextColor(dc, device.charging ? RGB(130, 235, 150) : RGB(255, 255, 255));
        RECT rect{0, 0, size, size};
        auto text = std::to_wstring(device.percent);
        DrawTextW(dc, text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        for (int i = 0; i < size * size; ++i) {
            data[i] |= 0xFF000000;
        }
        SelectObject(dc, oldFont);
        SelectObject(dc, oldBitmap);
        ICONINFO iconInfo{TRUE, 0, 0, mask, color};
        icon = CreateIconIndirect(&iconInfo);
    }
    if (font) DeleteObject(font);
    if (dc) DeleteDC(dc);
    if (mask) DeleteObject(mask);
    if (color) DeleteObject(color);
    return icon;
}

static void ClearNativeIcons(HWND window, NativeTrayState& state) {
    for (size_t i = 0; i < state.devices.size(); ++i) {
        NOTIFYICONDATAW data{sizeof(data)};
        data.hWnd = window;
        data.uID = state.IdFor(state.devices[i].key);
        Shell_NotifyIconW(NIM_DELETE, &data);
    }
    state.devices.clear();
}

static bool RefreshNativeIcons(HWND window, NativeTrayState& state) {
    BatterySnapshot snapshot;
    if (!NativeFallbackDelay(g_taskbarModule != nullptr, g_modernAttached.load(),
                             g_detachedSince.load(), GetTickCount64())) {
        std::lock_guard lock(g_snapshotMutex);
        snapshot = CombineLogitechProviders(g_snapshot, g_hidSnapshot);
    }
    std::erase_if(snapshot.devices, [](const auto& device) {
        return !DeviceShouldBeShown(device) || !DeviceVisible(device, g_settings.rules, g_settings.showNewDevices);
    });
    int size = NativeIconSize();
    if (SameDeviceList(snapshot.devices, state.devices) && size == state.iconSize) return true;
    std::erase_if(state.devices, [&](const auto& device) {
        if (std::any_of(snapshot.devices.begin(), snapshot.devices.end(), [&](auto& current) { return current.key == device.key; })) return false;
        NOTIFYICONDATAW data{sizeof(data)};
        data.hWnd = window;
        data.uID = state.IdFor(device.key);
        Shell_NotifyIconW(NIM_DELETE, &data);
        return true;
    });
    for (const auto& device : snapshot.devices) {
        auto previous = std::find_if(state.devices.begin(), state.devices.end(), [&](auto& item) { return item.key == device.key; });
        bool exists = previous != state.devices.end();
        if (exists && size == state.iconSize && SameDeviceState(*previous, device)) continue;
        NOTIFYICONDATAW data{sizeof(data)};
        data.hWnd = window;
        data.uID = state.IdFor(device.key);
        data.uFlags = NIF_ICON | NIF_TIP;
        data.hIcon = MakeNativeBatteryIcon(device, size);
        if (!data.hIcon) return false;
        auto tip = FormatBatteryTooltip(device, *g_tooltipStrings, g_tooltipLocale.c_str());
        wcsncpy_s(data.szTip, tip.c_str(), _TRUNCATE);
        BOOL updated = Shell_NotifyIconW(exists ? NIM_MODIFY : NIM_ADD, &data);
        DestroyIcon(data.hIcon);
        if (!updated) return false;
        if (exists) *previous = device;
        else state.devices.push_back(device);
    }
    state.devices = std::move(snapshot.devices);
    state.iconSize = size;
    return true;
}

static void RequestDeviceRefresh() {
    if (g_hubRefresh) SetEvent(g_hubRefresh);
    if (g_hidRefresh) SetEvent(g_hidRefresh);
    if (g_eventRefresh) SetEvent(g_eventRefresh);
}

static bool IsLogitechInterfaceChange(WPARAM event, LPARAM data) {
    if ((event != DBT_DEVICEARRIVAL && event != DBT_DEVICEREMOVECOMPLETE) || !data) return false;
    auto header = reinterpret_cast<const DEV_BROADCAST_HDR*>(data);
    if (header->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE ||
        header->dbch_size < sizeof(DEV_BROADCAST_DEVICEINTERFACE_W)) return false;
    auto info = reinterpret_cast<const DEV_BROADCAST_DEVICEINTERFACE_W*>(header);
    size_t capacity = (header->dbch_size - offsetof(DEV_BROADCAST_DEVICEINTERFACE_W, dbcc_name)) / sizeof(wchar_t);
    size_t length = wcsnlen(info->dbcc_name, capacity);
    for (size_t i = 0; i + 8 <= length; ++i)
        if (!_wcsnicmp(info->dbcc_name + i, L"vid_046d", 8)) return true;
    return false;
}

static LRESULT CALLBACK NativeTrayProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    auto state = reinterpret_cast<NativeTrayState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        auto create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
    } else if (state && message == state->taskbarCreated) {
        state->devices.clear();
        RequestDeviceRefresh();
        if (g_nativeRefresh) SetEvent(g_nativeRefresh);
    } else if (message == WM_SETTINGCHANGE || message == WM_DISPLAYCHANGE || message == WM_DPICHANGED) {
        if (g_nativeRefresh) SetEvent(g_nativeRefresh);
    } else if (message == WM_DEVICECHANGE && IsLogitechInterfaceChange(wParam, lParam)) {
        RequestDeviceRefresh();
    } else if (message == WM_POWERBROADCAST && wParam == PBT_APMRESUMEAUTOMATIC) {
        RequestDeviceRefresh();
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static DWORD WINAPI NativeTrayWorker(void*) {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<PCWSTR>(&NativeTrayWorker), &module);
    constexpr PCWSTR name = L"Windhawk_LogitechBattery_" WH_MOD_ID;
    WNDCLASSW cls{};
    cls.hInstance = module;
    cls.lpfnWndProc = NativeTrayProc;
    cls.lpszClassName = name;
    if (!RegisterClassW(&cls)) { Wh_Log(L"Native tray class registration failed"); return 1; }
    NativeTrayState state;
    HWND window = CreateWindowExW(0, name, L"", 0, 0, 0, 0, 0, nullptr, nullptr, module, &state);
    if (window) {
        DEV_BROADCAST_DEVICEINTERFACE_W filter{};
        filter.dbcc_size = sizeof(filter);
        filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        HidD_GetHidGuid(&filter.dbcc_classguid);
        HDEVNOTIFY notifications = RegisterDeviceNotificationW(window, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
        if (!notifications) Wh_Log(L"HID arrival notifications unavailable; periodic discovery remains active");
        RetryBackoff retry;
        bool refresh = true;
        ULONGLONG retryAt = 0;
        for (;;) {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
            DWORD delay = NativeFallbackDelay(g_taskbarModule != nullptr, g_modernAttached.load(),
                                              g_detachedSince.load(), GetTickCount64());
            bool success = true;
            if (refresh) {
                try { success = RefreshNativeIcons(window, state); }
                catch (...) { success = false; }
                if (success) { retry.Reset(); retryAt = 0; }
                else retryAt = GetTickCount64() + retry.Failed();
            }
            if (!delay) delay = INFINITE;
            if (retryAt) {
                auto now = GetTickCount64();
                delay = std::min(delay, retryAt <= now ? DWORD{0} : static_cast<DWORD>(retryAt - now));
            }
            HANDLE events[] = {g_stopEvent, g_nativeRefresh};
            DWORD wait = MsgWaitForMultipleObjectsEx(ARRAYSIZE(events), events, delay, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;
            refresh = wait == WAIT_TIMEOUT || wait == WAIT_OBJECT_0 + 1;
            MSG message;
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        ClearNativeIcons(window, state);
        if (notifications) UnregisterDeviceNotification(notifications);
        DestroyWindow(window);
    }
    UnregisterClassW(name, module);
    return window ? 0 : 2;
}

static void UpdateUIAndRetry(HWND window) {
    UpdateUI();
    if (g_timerId) KillTimer(window, g_timerId);
    g_timerId = 0;
    // Once attached, XAML events and provider changes are the only update sources.
    if (!g_modernAttached && !g_unloading)
        g_timerId = SetTimer(window, reinterpret_cast<UINT_PTR>(&g_timerId), 5000, nullptr);
}

static LRESULT CALLBACK TaskbarSubclass(HWND window, UINT message,
        WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (message == g_uiMessage) {
        if (wParam == 1) {
            if (g_timerId) KillTimer(window, g_timerId);
            g_timerId = 0;
            RemoveGroup();
        } else if (!g_unloading) {
            if (wParam == 2) RemoveGroup();
            UpdateUIAndRetry(window);
        }
        return 0;
    }
    if (message == WM_TIMER && g_timerId && wParam == g_timerId) {
        UpdateUIAndRetry(window);
        return 0;
    }
    if (!g_unloading && (message == WM_SIZE || message == WM_DPICHANGED || message == WM_THEMECHANGED)) {
        g_visualDirty = true;
        if (message == WM_THEMECHANGED) g_palette.reset();
        PostMessageW(window, g_uiMessage, 0, 0);
        if (g_nativeRefresh) SetEvent(g_nativeRefresh);
    }
    if (message == WM_NCDESTROY) {
        if (g_timerId) KillTimer(window, g_timerId);
        g_timerId = 0;
        RemoveGroup();
        g_taskbar = nullptr;
        if (g_hubRefresh) SetEvent(g_hubRefresh);
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

static DWORD WINAPI BatteryWorker(void*) {
    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
        std::unique_ptr<GHubClient> client;
        RetryBackoff reconnect;
        ULONGLONG nextQuery = 0;
        for (;;) {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
            if (g_taskbarModule && !g_taskbar && !g_unloading) {
                HWND window = FindTaskbar();
                if (window && WindhawkUtils::SetWindowSubclassFromAnyThread(window, TaskbarSubclass, 0)) {
                    g_taskbar = window;
                    PostMessageW(window, g_uiMessage, 0, 0);
                }
            }
            if (GetTickCount64() >= nextQuery) {
                auto queryRevision = g_hubRevision.load(std::memory_order_relaxed);
                BatterySnapshot next;
                try {
                    if (!client) {
                        auto connection = std::make_unique<GHubClient>(g_stopEvent);
                        if (connection->Connect()) client = std::move(connection);
                    }
                    if (client) next = QueryBatteries(*client, g_stopEvent);
                } catch (const winrt::hresult_error& error) {
                    Wh_Log(L"G HUB query: 0x%08X %s", static_cast<unsigned>(error.code()), error.message().c_str());
                } catch (...) { Wh_Log(L"G HUB query failed"); }
                bool connected = next.hubAvailable;
                if (!connected) client.reset();
                else reconnect.Reset();
                bool snapshotChanged = false, confirming = false;
                {
                    std::lock_guard lock(g_snapshotMutex);
                    next = MergeHubSnapshot(std::move(next), g_snapshot, queryRevision, GetTickCount64());
                    snapshotChanged = next.hubAvailable != g_snapshot.hubAvailable ||
                        !SameDeviceList(next.devices, g_snapshot.devices);
                    g_snapshot = std::move(next);
                    confirming = std::any_of(g_snapshot.devices.begin(), g_snapshot.devices.end(),
                        [](auto& device) { return device.missingSince && DeviceShouldBeShown(device); });
                }
                if (snapshotChanged) NotifyConsumers();
                nextQuery = GetTickCount64() + (connected ? g_settings.pollSeconds * 1000 : reconnect.Failed());
                if (confirming) nextQuery = std::min(nextQuery, GetTickCount64() + 5000);
            }
            auto now = GetTickCount64();
            DWORD delay = nextQuery <= now ? 0 : static_cast<DWORD>(nextQuery - now);
            if (g_taskbarModule && !g_taskbar) delay = std::min<DWORD>(delay, 5000);
            HANDLE events[] = {g_stopEvent, g_hubRefresh};
            DWORD wait = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, delay);
            if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;
            if (wait == WAIT_OBJECT_0 + 1) {
                if (WaitForSingleObject(g_stopEvent, 500) != WAIT_TIMEOUT) break;
                nextQuery = 0;
            }
        }
    } catch (...) { Wh_Log(L"Battery worker could not start"); }
    if (apartmentInitialized) winrt::uninit_apartment();
    return 0;
}

static DWORD WINAPI GHubEventWorker(void*) {
    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
        RetryBackoff reconnect;
        while (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
            ULONGLONG connectedAt = 0;
            try {
                GHubClient client(g_stopEvent);
                if (client.Connect()) {
                    client.Send(L"/battery/state/changed", L"SUBSCRIBE");
                    connectedAt = GetTickCount64();
                    SetEvent(g_hubRefresh);
                    Wh_Log(L"G HUB battery event stream connected");
                    while (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
                        auto event = client.Receive();
                        bool changed, discover;
                        {
                            std::lock_guard lock(g_snapshotMutex);
                            discover = HubEventNeedsDiscovery(g_snapshot, event);
                            changed = ApplyHubEvent(g_snapshot, event);
                        }
                        if (discover) SetEvent(g_hubRefresh);
                        if (changed) NotifyConsumers();
                    }
                }
            } catch (...) {}
            if (connectedAt && GetTickCount64() - connectedAt >= 60000) reconnect.Reset();
            HANDLE events[] = {g_stopEvent, g_eventRefresh};
            DWORD wait = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, reconnect.Failed());
            if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;
        }
    } catch (...) { Wh_Log(L"G HUB event worker could not start"); }
    if (apartmentInitialized) winrt::uninit_apartment();
    return 0;
}

static DWORD WINAPI HidBatteryWorker(void*) {
    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
        HidProvider provider(g_stopEvent);
        provider.changed = [&] {
            auto devices = provider.Snapshot();
            bool changed = false;
            {
                std::lock_guard lock(g_snapshotMutex);
                changed = !SameDeviceList(devices, g_hidSnapshot);
                g_hidSnapshot = std::move(devices);
            }
            if (changed) NotifyConsumers();
        };
        ULONGLONG nextDiscovery = 0, nextEventDiscovery = 0;
        while (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
            auto now = GetTickCount64();
            bool refresh = WaitForSingleObject(g_hidRefresh, 0) == WAIT_OBJECT_0;
            if (refresh) { ResetEvent(g_hidRefresh); nextDiscovery = 0; }
            if (now >= nextDiscovery || (provider.NeedsDiscovery() && now >= nextEventDiscovery)) {
                provider.Discover();
                nextEventDiscovery = GetTickCount64() + 1000;
                nextDiscovery = GetTickCount64() + 180000;
            }
            provider.Poll(g_settings.pollSeconds * 1000, refresh);
            provider.Pump();
            now = GetTickCount64();
            auto due = std::min(nextDiscovery, provider.NextPoll());
            if (provider.NeedsDiscovery()) due = std::min(due, nextEventDiscovery);
            if (!provider.Wait(due <= now ? 0 : static_cast<DWORD>(due - now), g_hidRefresh)) break;
        }
    } catch (...) { Wh_Log(L"Logitech HID++ worker could not start"); }
    if (apartmentInitialized) winrt::uninit_apartment();
    return 0;
}

static void StopWorker() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    for (HANDLE* handle : {&g_eventWorker, &g_hidWorker, &g_nativeWorker}) {
        if (*handle) {
            WaitForSingleObject(*handle, INFINITE);
            CloseHandle(*handle);
            *handle = nullptr;
        }
    }
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
}

static void CloseWorkerEvents() {
    // UI callbacks are also producers: close events only after detaching XAML.
    for (HANDLE* event : {&g_stopEvent, &g_hubRefresh, &g_hidRefresh, &g_eventRefresh, &g_nativeRefresh}) {
        if (*event) CloseHandle(*event);
        *event = nullptr;
    }
}

static void StartWorker() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hubRefresh = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_hidRefresh = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_eventRefresh = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_nativeRefresh = CreateEventW(nullptr, FALSE, TRUE, nullptr);
    if (!g_stopEvent || !g_hubRefresh || !g_hidRefresh || !g_eventRefresh || !g_nativeRefresh) { StopWorker(); return; }
    g_worker = CreateThread(nullptr, 0, BatteryWorker, nullptr, 0, nullptr);
    g_eventWorker = CreateThread(nullptr, 0, GHubEventWorker, nullptr, 0, nullptr);
    if (g_settings.directHidpp) g_hidWorker = CreateThread(nullptr, 0, HidBatteryWorker, nullptr, 0, nullptr);
    g_nativeWorker = CreateThread(nullptr, 0, NativeTrayWorker, nullptr, 0, nullptr);
    if (!g_worker || !g_eventWorker || !g_nativeWorker || (g_settings.directHidpp && !g_hidWorker)) {
        Wh_Log(L"Failed to start workers; rolling back partial startup");
        StopWorker();
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Logitech Battery Tray " WH_MOD_VERSION);
    LoadSettings();
    if (HWND existing = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        DWORD owner = 0;
        GetWindowThreadProcessId(existing, &owner);
        if (owner != GetCurrentProcessId()) return FALSE;
    }
    if (g_settings.nativeOnly) return TRUE;
#if defined(_M_IX86) || defined(__i386__)
    return TRUE;
#endif
    g_taskbarModule = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_taskbarModule) return TRUE;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_taskBandSiteVtable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &g_getTaskbarHost},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &g_frameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &g_decref},
    };
    bool hooked = WindhawkUtils::HookSymbols(g_taskbarModule, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
    if (!hooked) {
        Wh_Log(L"Unsupported taskbar: required symbols were not found");
        FreeLibrary(g_taskbarModule);
        g_taskbarModule = nullptr;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    g_detachedSince = GetTickCount64();
    HWND window = FindTaskbar();
    if (g_taskbarModule && window && WindhawkUtils::SetWindowSubclassFromAnyThread(window, TaskbarSubclass, 0)) {
        g_taskbar = window;
        SendMessageW(window, g_uiMessage, 0, 0);
    } else {
        Wh_Log(L"Waiting for the primary taskbar; the worker will retry");
    }
    StartWorker();
}

void Wh_ModUninit() {
    g_unloading = true;
    StopWorker();
    if (auto window = g_taskbar.exchange(nullptr)) {
        SendMessageW(window, g_uiMessage, 1, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, TaskbarSubclass);
    }
    CloseWorkerEvents();
    if (g_taskbarModule) {
        FreeLibrary(g_taskbarModule);
        g_taskbarModule = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    // Windhawk unloads the old instance before loading the updated settings.
    // This avoids mutating settings while the worker or UI is using them.
    *bReload = TRUE;
    return TRUE;
}
