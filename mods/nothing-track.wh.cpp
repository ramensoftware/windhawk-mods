// ==WindhawkMod==
// @id              nothing-track
// @name            Nothing Track
// @name:ru-RU      Nothing Track
// @description     Battery monitor & audio controls (ANC, Ultra Bass, EQ, Low Latency) for Nothing & CMF earbuds on the Windows 11 taskbar
// @description:ru-RU Мониторинг батареи и управление (ANC, Ultra Bass, EQ, Low Latency) для наушников Nothing и CMF прямо в панели задач Windows 11
// @version         1.0.0
// @author          lenorio
// @github          https://github.com/lenorio
// @homepage        https://github.com/lenorio/Nothing-Track
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -luser32 -lwindowsapp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Nothing Track

Displays battery levels and quick audio controls for Nothing and CMF earbuds on the Windows 11 taskbar.

![Nothing Track taskbar widget](https://raw.githubusercontent.com/lenorio/Nothing-Track/main/assets/taskbar-widget.png)

![Nothing Track control flyout](https://raw.githubusercontent.com/lenorio/Nothing-Track/main/assets/control-flyout.png)

The mod communicates with earbuds directly over Bluetooth RFCOMM (SPP), so you don't need the phone app or an emulator to check battery or toggle features on PC.

Unlike the generic **BT Battery Monitor** mod, Nothing Track uses the Nothing/CMF vendor protocol to provide ANC, EQ, bass, low-latency, and Find My Earbuds controls in addition to battery levels.

Taskbar placement and XAML injection are based on [Taskbar Fluent Media Player](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-fluent-media-player.wh.cpp) by Salyts.

### Features
* **Taskbar widget:** shows battery percentage for left/right buds and the charging case with native battery glyphs.
* **Placement options:** dock next to the clock, tray icons, or near the Start button.
* **Control flyout (click widget to open):**
  * Noise control: ANC (High, Mid, Low, Adaptive), Transparency, Off.
  * Ultra Bass: toggle and level adjustment (1–5).
  * EQ presets: Balanced, More Bass, More Treble, Voice, Dirac Opteo.
  * Game mode (Low Latency toggle).
  * Find My Earbuds: ring the left or right earbud.
* **Single-earbud mode:** automatically dims an inactive or unavailable earbud.
* **Background reconnect:** recovers connection automatically when earbuds wake up or reconnect to Windows.

### Supported Devices
Tested with CMF Buds 2 / Buds Pro 2 and Nothing Ear series. Other Nothing/CMF models using the same protocol should work as well.

---

## Описание на русском

Мод добавляет на панель задач Windows 11 виджет для мониторинга батареи и управления наушниками Nothing и CMF.

Работает напрямую через Bluetooth RFCOMM (профиль SPP) — телефон или эмулятор для переключения режимов на компьютере больше не нужны.

### Возможности
* **Виджет в таскбаре:** выводит уровень заряда левого/правого наушника и кейса с системными иконками батареи.
* **Настройка позиции:** можно закрепить рядом с часами, в трее или возле кнопки «Пуск».
* **Окно управления (по клику на виджет):**
  * Шумоподавление: ANC (высокое, среднее, низкое, адаптивное), Прозрачность, Выкл.
  * Ultra Bass: переключатель и выбор уровня (1–5).
  * Эквалайзер: пресеты Balanced, More Bass, More Treble, Voice, Dirac Opteo.
  * Игровой режим (Low Latency).
  * Поиск наушников: звуковой сигнал на левый или правый наушник.
* **Режим одного наушника:** автоматически приглушает отображение неактивного или недоступного наушника.
* **Фоновое переподключение:** автоматически восстанавливает связь по Bluetooth при включении или подключении наушников к системе.

### Поддерживаемые устройства
Протестировано на CMF Buds 2 / Buds Pro 2 и линейке Nothing Ear. Наушники Nothing и CMF с аналогичным протоколом связи также поддерживаются.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: "tray_before_clock"
  $name: Widget position
  $name:ru-RU: Расположение виджета
  $options:
  - "taskbar_left_edge": "Taskbar - Left edge (Overlay)"
  - "taskbar_center_edge": "Taskbar - Center (Overlay)"
  - "taskbar_right_edge": "Taskbar - Right edge (Overlay)"
  - "taskbar_left_start": "Taskbar - Left of Start button"
  - "taskbar_right_start": "Taskbar - Right of Start button"
  - "taskbar_after_search_left": "Taskbar - Left of Search button"
  - "taskbar_after_search_right": "Taskbar - Right of Search button"
  - "taskbar_after_taskview_left": "Taskbar - Left of Task View button"
  - "taskbar_after_taskview_right": "Taskbar - Right of Task View button"
  - "taskbar_after_widgets_left": "Taskbar - Left of Widgets button"
  - "taskbar_after_widgets_right": "Taskbar - Right of Widgets button"
  - "tray_left": "Tray - Far left"
  - "tray_right": "Tray - Far right"
  - "tray_before_clock": "Tray - Left of Clock (Default)"
  - "tray_after_clock": "Tray - Right of Clock"
  - "tray_before_omni_left": "Tray - Left of Network/Volume button"
  - "tray_before_omni_right": "Tray - Right of Network/Volume button"
  - "tray_language_left": "Tray - Left of Language button"
  - "tray_language_right": "Tray - Right of Language button"
  - "tray_icons_left": "Tray - Left of Tray Icons"
  - "tray_icons_right": "Tray - Right of Tray Icons"
  - "tray_hidden_icons_left": "Tray - Left of Hidden icons button"
  - "tray_hidden_icons_right": "Tray - Right of Hidden icons button"
  - "tray_after_showdesktop_left": "Tray - Left of Show Desktop"
  - "tray_after_showdesktop_right": "Tray - Right of Show Desktop"
  $options:ru-RU:
  - "taskbar_left_edge": "Панель задач - Левый край (оверлей)"
  - "taskbar_center_edge": "Панель задач - Центр (оверлей)"
  - "taskbar_right_edge": "Панель задач - Правый край (оверлей)"
  - "taskbar_left_start": "Панель задач - Слева от кнопки Пуск"
  - "taskbar_right_start": "Панель задач - Справа от кнопки Пуск"
  - "taskbar_after_search_left": "Панель задач - Слева от кнопки Поиск"
  - "taskbar_after_search_right": "Панель задач - Справа от кнопки Поиск"
  - "taskbar_after_taskview_left": "Панель задач - Слева от кнопки Представление задач"
  - "taskbar_after_taskview_right": "Панель задач - Справа от кнопки Представление задач"
  - "taskbar_after_widgets_left": "Панель задач - Слева от кнопки Мини-приложений"
  - "taskbar_after_widgets_right": "Панель задач - Справа от кнопки Мини-приложений"
  - "tray_left": "Трей - Край слева"
  - "tray_right": "Трей - Край справа"
  - "tray_before_clock": "Трей - Слева от часов (По умолчанию)"
  - "tray_after_clock": "Трей - Справа от часов"
  - "tray_before_omni_left": "Трей - Слева от кнопки Сеть/Громкость"
  - "tray_before_omni_right": "Трей - Справа от кнопки Сеть/Громкость"
  - "tray_language_left": "Трей - Слева от кнопки языка"
  - "tray_language_right": "Трей - Справа от кнопки языка"
  - "tray_icons_left": "Трей - Слева от значков области уведомлений"
  - "tray_icons_right": "Трей - Справа от значков области уведомлений"
  - "tray_hidden_icons_left": "Трей - Слева от скрытых значков"
  - "tray_hidden_icons_right": "Трей - Справа от скрытых значков"
  - "tray_after_showdesktop_left": "Трей - Слева от кнопки Показать рабочий стол"
  - "tray_after_showdesktop_right": "Трей - Справа от кнопки Показать рабочий стол"
- displayFormat: "compact"
  $name: Display format
  $name:ru-RU: Формат отображения
  $options:
  - "compact": "Compact (Dual battery: L & R)"
  - "detailed": "Detailed (Name + Batteries)"
  - "single_active": "Single active earbud"
  - "icon_only": "Icon only (🎧)"
  $options:ru-RU:
  - "compact": "Компактный (Две батареи: L и R)"
  - "detailed": "Подробный (Имя + Батареи)"
  - "single_active": "Один активный наушник"
  - "icon_only": "Только иконки (🎧)"
- pollInterval: 30
  $name: Battery polling interval (sec)
  $name:ru-RU: Интервал опроса батареи (сек)
  $description: Battery update interval over Bluetooth (10-120 sec)
  $description:ru-RU: Периодичность обновления заряда аккумулятора по Bluetooth (10-120 сек)
- language: "auto"
  $name: Interface language
  $name:ru-RU: Язык интерфейса
  $options:
  - "auto": "Auto (System language)"
  - "en": "English"
  - "ru": "Russian"
  $options:ru-RU:
  - "auto": "Авто (Язык системы)"
  - "en": "English"
  - "ru": "Русский"
- marginSide: "4 4"
  $name: "Widget margins (left right)"
  $name:ru-RU: "Отступы виджета (слева справа)"
  $description: "Margin in pixels: left and right separated by a space (default: 4 4)"
  $description:ru-RU: "Отступ в пикселях: левый и правый через пробел (по умолчанию 4 4)"
- hideDisconnectedBuds: true
  $name: "Hide disconnected earbud"
  $name:ru-RU: "Скрывать отключенный наушник"
  $description: "Hide inactive earbud in the widget avoiding disconnected battery icon"
  $description:ru-RU: "Скрывать неактивный наушник в виджете и не показывать батарею с крестиком"
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.Rfcomm.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Storage.Streams.h>

#include <windows.h>
#include <cwchar>
#include <cwctype>
#include <cstdint>
#include <memory>
#include <vector>
#include <deque>
#include <string>
#include <string_view>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>
#include <type_traits>
#include <windhawk_api.h>
#include <windhawk_utils.h>

using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::Devices::Bluetooth;
using namespace winrt::Windows::Devices::Bluetooth::Rfcomm;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Storage::Streams;

static constexpr wchar_t kWidgetGridName[] = L"WindhawkNothingTrackWidget";

inline winrt::guid SppUuid() {
    return winrt::guid{0xaeac4a03, 0xdff5, 0x498f, {0x84, 0x3a, 0x34, 0x48, 0x7c, 0xf1, 0x33, 0xeb}};
}

enum class AncMode : uint8_t {
    Off = 0,
    Transparency = 1,
    High = 2,
    Mid = 3,
    Low = 4,
    Adaptive = 5,
};

struct BatteryReading {
    bool present = false;
    bool charging = false;
    int percent = -1;
    std::chrono::steady_clock::time_point lastSeen{};
};

struct EarbudsState {
    std::wstring deviceName = L"CMF Buds 2";
    bool connected = false;
    bool connecting = false;

    BatteryReading left;
    BatteryReading right;
    BatteryReading caseBattery;

    AncMode ancMode = AncMode::Off;

    uint8_t eqPreset = 0;
    bool bassEnabled = true;
    uint8_t bassLevel = 3;
    bool lowLatencyEnabled = false;

};

struct ModSettings {
    std::wstring position = L"tray_before_clock";
    std::wstring displayFormat = L"compact";
    int pollInterval = 30;
    std::wstring language = L"auto";
    double marginLeft = 4.0;
    double marginRight = 4.0;
    bool hideDisconnectedBuds = true;
};

static ModSettings g_settings;
static std::atomic<int> g_pollIntervalSeconds{30};
static EarbudsState g_earbudsState;
static std::mutex g_stateMutex;
static std::atomic<HWND> g_taskbarWnd{nullptr};
static std::atomic<bool> g_unloading{false};

// XAML Elements references
[[clang::no_destroy]] static Grid g_injectedGrid{nullptr};
[[clang::no_destroy]] static FrameworkElement g_injectionParent{nullptr};
static int g_injectedColumn = -1;
[[clang::no_destroy]] static FrameworkElement g_trackedElement{nullptr};
static Thickness g_trackedElementOriginalMargin{};
static bool g_hasTrackedElementOriginalMargin = false;
static std::wstring g_trackPosition = L"";
static winrt::event_token g_layoutUpdateToken{};
[[clang::no_destroy]] static DispatcherTimer g_dispatcherTimer{nullptr};
[[clang::no_destroy]] static DispatcherTimer g_retryTimer{nullptr};
static winrt::event_token g_retryTimerToken{};
[[clang::no_destroy]] static Flyout s_currentFlyout{nullptr};
[[clang::no_destroy]] static winrt::Windows::UI::Core::CoreDispatcher g_uiDispatcher{nullptr};
static winrt::event_token g_flyoutOpenedToken{};
static winrt::event_token g_flyoutClosedToken{};
static winrt::event_token g_timerToken{};
[[clang::no_destroy]] static std::optional<std::wstring> g_lastTooltip;

static inline uint16_t ComputeCrc16(const uint8_t* data, size_t size) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc & 1) ? static_cast<uint16_t>((crc >> 1) ^ 0xA001) : static_cast<uint16_t>(crc >> 1);
        }
    }
    return crc;
}

static inline std::vector<uint8_t> BuildPacket(uint16_t commandId, const std::vector<uint8_t>& payload, uint8_t opId) {
    uint16_t payloadSize = static_cast<uint16_t>(payload.size());
    std::vector<uint8_t> packet;
    packet.reserve(3 + 2 + 1 + 2 + payload.size() + 2);
    packet.push_back(0x55);
    packet.push_back(0x60);
    packet.push_back(0x01);
    packet.push_back(static_cast<uint8_t>(commandId & 0xFF));
    packet.push_back(static_cast<uint8_t>((commandId >> 8) & 0xFF));
    packet.push_back(static_cast<uint8_t>(payloadSize & 0xFF));
    packet.push_back(static_cast<uint8_t>((payloadSize >> 8) & 0xFF));
    packet.push_back(opId);
    packet.insert(packet.end(), payload.begin(), payload.end());
    uint16_t crc = ComputeCrc16(packet.data(), packet.size());
    packet.push_back(static_cast<uint8_t>(crc & 0xFF));
    packet.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));
    return packet;
}

class BluetoothManager {
public:
    static BluetoothManager& Instance() {
        std::lock_guard<std::mutex> lock(InstanceMutex());
        auto& instance = InstanceStorage();
        if (!instance) {
            instance.emplace();
        }
        return *instance;
    }

    static void DestroyInstance() {
        std::lock_guard<std::mutex> lock(InstanceMutex());
        InstanceStorage().reset();
    }

    void Start() {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        if (m_running.exchange(true)) return;
        m_stopRequested.store(false);
        try {
            m_worker = std::thread([this]() { WorkerLoop(); });
            m_actionWorker = std::thread([this]() { ActionLoop(); });
        } catch (...) {
            m_stopRequested.store(true);
            m_actionCv.notify_all();
            m_workerWakeState->cv.notify_all();
            CancelPendingOperations();
            if (m_actionWorker.joinable()) m_actionWorker.join();
            if (m_worker.joinable()) m_worker.join();
            m_running.store(false);
            Wh_Log(L"BluetoothManager: Failed to start worker threads");
        }
    }

    void Stop() {
        std::lock_guard<std::mutex> lock(m_lifecycleMutex);
        if (!m_running.exchange(false)) return;
        m_stopRequested.store(true);
        m_actionCv.notify_all();
        m_workerWakeState->cv.notify_all();
        CancelPendingOperations();
        Disconnect();
        if (m_actionWorker.joinable()) {
            m_actionWorker.join();
        }
        if (m_worker.joinable()) {
            m_worker.join();
        }
    }

    void PostAction(std::function<void()> action) {
        if (m_stopRequested.load()) return;
        {
            std::lock_guard<std::mutex> lock(m_actionMutex);
            m_actionQueue.push_back(std::move(action));
        }
        m_actionCv.notify_one();
    }

    void NotifyPollIntervalChanged() {
        m_pollIntervalChanged.store(true);
        m_actionCv.notify_all();
    }

    bool SendCommand(uint16_t commandId, const std::vector<uint8_t>& payload) {
        std::lock_guard<std::mutex> lock(m_socketMutex);
        if (m_stopRequested.load() || !m_connected.load() || !m_socket ||
            payload.size() > UINT16_MAX) {
            return false;
        }
        try {
            uint8_t opId = static_cast<uint8_t>((m_opCounter++ % 250) + 1);
            auto packet = BuildPacket(commandId, payload, opId);
            DataWriter writer(m_socket.OutputStream());
            writer.WriteBytes(winrt::array_view<const uint8_t>(
                packet.data(), static_cast<uint32_t>(packet.size())));
            auto deadline = std::chrono::steady_clock::now() +
                            std::chrono::seconds(3);
            GetAsync(writer.StoreAsync(), std::chrono::seconds(3));
            auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
                deadline - std::chrono::steady_clock::now());
            if (remaining <= std::chrono::milliseconds::zero()) {
                throw winrt::hresult_canceled();
            }
            GetAsync(writer.FlushAsync(), remaining);
            writer.DetachStream();
            return true;
        } catch (...) {
            m_connected.store(false);
            try {
                if (m_socket) {
                    m_socket.Close();
                    m_socket = nullptr;
                }
                m_service = nullptr;
            } catch (...) {}
            std::lock_guard<std::mutex> stateLock(g_stateMutex);
            g_earbudsState.connected = false;
            g_earbudsState.connecting = false;
            return false;
        }
    }

    void QueryAll() {
        SendCommand(49159, {}); // Battery (readBattery)
        if (m_stopRequested.load()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        SendCommand(49182, {}); // ANC (readANC)
        if (m_stopRequested.load()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        SendCommand(49232, {}); // ListeningMode / EQ for B179 / CMF Buds 2
        if (m_stopRequested.load()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        SendCommand(49183, {}); // EQ fallback (readEQ)
        if (m_stopRequested.load()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        SendCommand(49230, {}); // Bass (readEnhancedBass)
        if (m_stopRequested.load()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        SendCommand(49217, {}); // Low latency (readLatency)
    }

    void QueryBattery() {
        SendCommand(49159, {});
    }

    bool SetAnc(AncMode mode) {
        // CMF Buds 2 (B179) and Nothing Ear ANC protocol from EarWeb:
        // CMD 61455, payload: [0x01, ancByte, 0x00]
        // 0x01 = High, 0x02 = Low, 0x03 = Mid, 0x04 = Adaptive, 0x05 = Off, 0x07 = Transparency
        uint8_t ancByte = 0x05;
        switch (mode) {
        case AncMode::High:         ancByte = 0x01; break;
        case AncMode::Low:          ancByte = 0x02; break;
        case AncMode::Mid:          ancByte = 0x03; break;
        case AncMode::Adaptive:     ancByte = 0x04; break;
        case AncMode::Off:          ancByte = 0x05; break;
        case AncMode::Transparency: ancByte = 0x07; break;
        default:                    ancByte = 0x05; break;
        }
        std::vector<uint8_t> payload = {0x01, ancByte, 0x00};
        bool ok = SendCommand(61455, payload);
        if (ok) {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_earbudsState.ancMode = mode;
        }
        return ok;
    }

    bool SetBass(bool enabled, uint8_t level) {
        level = std::clamp<uint8_t>(level, 1, 5);
        std::vector<uint8_t> payload = {
            static_cast<uint8_t>(enabled ? 0x01 : 0x00),
            static_cast<uint8_t>(enabled ? level * 2 : 0x00)
        };
        bool ok = SendCommand(61521, payload);
        if (ok) {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_earbudsState.bassEnabled = enabled;
            g_earbudsState.bassLevel = level;
        }
        return ok;
    }

    bool SetEq(uint8_t preset) {
        std::vector<uint8_t> payload = {preset, 0x00};
        // CMF Buds 2 (B179) uses CMD 61469 (setListeningMode), older models use 61456
        bool ok = SendCommand(61469, payload);
        ok |= SendCommand(61456, payload);
        if (ok) {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_earbudsState.eqPreset = preset;
        }
        return ok;
    }

    bool SetLowLatency(bool enabled) {
        std::vector<uint8_t> payload = {static_cast<uint8_t>(enabled ? 0x01 : 0x02), 0x00};
        bool ok = SendCommand(61504, payload);
        if (ok) {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_earbudsState.lowLatencyEnabled = enabled;
        }
        return ok;
    }

    bool FindBuds(bool left, bool active) {
        // EarWeb protocol for B179: CMD 61442, [0x02 (L) / 0x03 (R), active ? 0x01 : 0x00]
        uint8_t side = left ? 0x02 : 0x03;
        uint8_t stateVal = active ? 0x01 : 0x00;
        bool ok = SendCommand(61442, {side, stateVal});
        if (ok) {
            (left ? m_ringingLeft : m_ringingRight).store(active);
        }
        return ok;
    }

    void StopRinging() {
        if (m_ringingLeft.exchange(false)) {
            SendCommand(61442, {0x02, 0x00});
        }
        if (m_ringingRight.exchange(false)) {
            SendCommand(61442, {0x03, 0x00});
        }
    }

private:
    static std::optional<BluetoothManager>& InstanceStorage() {
        [[clang::no_destroy]] static std::optional<BluetoothManager> s_instance;
        return s_instance;
    }

    static std::mutex& InstanceMutex() {
        static std::mutex mutex;
        return mutex;
    }

    template <typename TAsync>
    auto GetAsync(
        TAsync const& operation,
        std::chrono::milliseconds timeout = std::chrono::milliseconds::max())
        -> decltype(operation.get()) {
        auto asyncInfo = operation.template as<winrt::Windows::Foundation::IAsyncInfo>();
        {
            std::lock_guard<std::mutex> lock(m_asyncMutex);
            if (m_stopRequested.load()) {
                try { asyncInfo.Cancel(); } catch (...) {}
                throw winrt::hresult_canceled();
            }
            m_asyncOperations.push_back(asyncInfo);
        }

        auto untrack = [&]() {
            std::lock_guard<std::mutex> lock(m_asyncMutex);
            auto abi = winrt::get_abi(asyncInfo);
            m_asyncOperations.erase(
                std::remove_if(m_asyncOperations.begin(), m_asyncOperations.end(),
                    [abi](auto const& item) { return winrt::get_abi(item) == abi; }),
                m_asyncOperations.end());
        };

        try {
            if (timeout != std::chrono::milliseconds::max() &&
                operation.wait_for(timeout) !=
                    winrt::Windows::Foundation::AsyncStatus::Completed) {
                try { asyncInfo.Cancel(); } catch (...) {}
                untrack();
                throw winrt::hresult_canceled();
            }
            if constexpr (std::is_void_v<decltype(operation.get())>) {
                operation.get();
                untrack();
            } else {
                auto result = operation.get();
                untrack();
                return result;
            }
        } catch (...) {
            untrack();
            throw;
        }
    }

    void CancelPendingOperations() {
        std::vector<winrt::Windows::Foundation::IAsyncInfo> operations;
        {
            std::lock_guard<std::mutex> lock(m_asyncMutex);
            operations = m_asyncOperations;
        }
        for (auto const& operation : operations) {
            try { operation.Cancel(); } catch (...) {}
        }
    }

    std::atomic<bool> m_running{false};
    std::atomic<bool> m_stopRequested{false};
    std::atomic<bool> m_connected{false};
    std::atomic<bool> m_ringingLeft{false};
    std::atomic<bool> m_ringingRight{false};
    std::atomic<bool> m_pollIntervalChanged{false};
    std::mutex m_lifecycleMutex;
    std::thread m_worker;
    std::thread m_actionWorker;
    std::mutex m_actionMutex;
    std::condition_variable m_actionCv;
    std::deque<std::function<void()>> m_actionQueue;
    std::mutex m_asyncMutex;
    std::vector<winrt::Windows::Foundation::IAsyncInfo> m_asyncOperations;
    struct WorkerWakeState {
        std::condition_variable cv;
        std::atomic<uint64_t> connectionStatusVersion{0};
    };
    std::shared_ptr<WorkerWakeState> m_workerWakeState =
        std::make_shared<WorkerWakeState>();
    std::mutex m_workerWaitMutex;
    BluetoothDevice m_cachedDevice{nullptr};
    std::wstring m_cachedDeviceName;
    winrt::event_token m_connectionStatusToken{};

    bool WaitForWake(std::chrono::seconds duration) {
        auto wakeState = m_workerWakeState;
        uint64_t version = wakeState->connectionStatusVersion.load();
        std::unique_lock<std::mutex> lock(m_workerWaitMutex);
        return wakeState->cv.wait_for(lock, duration,
                                     [this, wakeState, version]() {
                                         return m_stopRequested.load() ||
                                                wakeState->connectionStatusVersion.load() != version;
                                     });
    }

    void ClearCachedDevice() {
        if (m_cachedDevice && m_connectionStatusToken.value) {
            try {
                m_cachedDevice.ConnectionStatusChanged(m_connectionStatusToken);
            } catch (...) {}
        }
        m_connectionStatusToken = {};
        m_cachedDevice = nullptr;
        m_cachedDeviceName.clear();
    }

    void CacheDevice(BluetoothDevice const& device, std::wstring const& name) {
        ClearCachedDevice();
        m_cachedDevice = device;
        m_cachedDeviceName = name;
        if (m_cachedDevice) {
            try {
                auto wakeState = m_workerWakeState;
                m_connectionStatusToken = m_cachedDevice.ConnectionStatusChanged(
                    [wakeState](auto const&, auto const&) {
                        wakeState->connectionStatusVersion.fetch_add(1);
                        wakeState->cv.notify_all();
                    });
            } catch (...) {}
        }
    }

    void ExpireStaleBatteryReadings() {
        auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (g_earbudsState.left.present &&
            now - g_earbudsState.left.lastSeen > std::chrono::seconds(35)) {
            g_earbudsState.left.present = false;
        }
        if (g_earbudsState.right.present &&
            now - g_earbudsState.right.lastSeen > std::chrono::seconds(35)) {
            g_earbudsState.right.present = false;
        }
        if (g_earbudsState.caseBattery.present &&
            now - g_earbudsState.caseBattery.lastSeen > std::chrono::seconds(15)) {
            g_earbudsState.caseBattery.present = false;
        }
    }

    void ActionLoop() {
        bool apartmentInitialized = false;
        try {
            init_apartment(winrt::apartment_type::multi_threaded);
            apartmentInitialized = true;
        } catch (...) {}
        auto nextPoll = std::chrono::steady_clock::now() +
                        std::chrono::seconds(g_pollIntervalSeconds.load());
        while (!m_stopRequested.load()) {
            std::function<void()> action;
            bool pollBattery = false;
            {
                std::unique_lock<std::mutex> lock(m_actionMutex);
                m_actionCv.wait_until(lock, nextPoll, [this]() {
                    return m_stopRequested.load() ||
                           m_pollIntervalChanged.load() ||
                           !m_actionQueue.empty();
                });
                if (m_stopRequested.load()) break;
                if (m_pollIntervalChanged.exchange(false)) {
                    nextPoll = std::chrono::steady_clock::now() +
                               std::chrono::seconds(g_pollIntervalSeconds.load());
                }
                if (!m_actionQueue.empty()) {
                    action = std::move(m_actionQueue.front());
                    m_actionQueue.pop_front();
                } else if (std::chrono::steady_clock::now() >= nextPoll) {
                    pollBattery = true;
                    nextPoll = std::chrono::steady_clock::now() +
                               std::chrono::seconds(g_pollIntervalSeconds.load());
                }
            }
            if (action) {
                try {
                    action();
                } catch (...) {}
            } else if (pollBattery && m_connected.load()) {
                ExpireStaleBatteryReadings();
                QueryBattery();
            }
        }
        if (apartmentInitialized) {
            winrt::uninit_apartment();
        }
    }

    std::mutex m_socketMutex;
    StreamSocket m_socket{nullptr};
    RfcommDeviceService m_service{nullptr};
    uint8_t m_opCounter{0};

    void Disconnect() {
        std::lock_guard<std::mutex> lock(m_socketMutex);
        m_connected.store(false);
        try {
            if (m_socket) {
                m_socket.Close();
                m_socket = nullptr;
            }
            m_service = nullptr;
        } catch (...) {}

        std::lock_guard<std::mutex> stateLock(g_stateMutex);
        g_earbudsState.connected = false;
        g_earbudsState.connecting = false;
        g_earbudsState.left.present = false;
        g_earbudsState.right.present = false;
        g_earbudsState.caseBattery.present = false;
    }

    void ProcessPacket(uint16_t cmd, const std::vector<uint8_t>& payload) {
        auto now = std::chrono::steady_clock::now();

        if (cmd == 16391 || cmd == 57345) { // Battery status
            if (!payload.empty()) {
                uint8_t count = payload[0];
                std::lock_guard<std::mutex> lock(g_stateMutex);
                for (uint8_t i = 0; i < count; ++i) {
                    if (1 + (i * 2) + 1 < payload.size()) {
                        uint8_t devId = payload[1 + (i * 2)];
                        uint8_t rawVal = payload[2 + (i * 2)];
                        bool charging = (rawVal & 0x80) != 0;
                        int pct = (rawVal & 0x7F);
                        if (pct <= 100) {
                            if (devId == 0x02) { // Left
                                g_earbudsState.left.present = true;
                                g_earbudsState.left.charging = charging;
                                g_earbudsState.left.percent = pct;
                                g_earbudsState.left.lastSeen = now;
                            } else if (devId == 0x03) { // Right
                                g_earbudsState.right.present = true;
                                g_earbudsState.right.charging = charging;
                                g_earbudsState.right.percent = pct;
                                g_earbudsState.right.lastSeen = now;
                            } else if (devId == 0x04) { // Case
                                g_earbudsState.caseBattery.present = true;
                                g_earbudsState.caseBattery.charging = charging;
                                g_earbudsState.caseBattery.percent = pct;
                                g_earbudsState.caseBattery.lastSeen = now;
                            }
                        }
                    }
                }

                // Check timeouts for buds not seen in 35 seconds
                if (g_earbudsState.left.present && (now - g_earbudsState.left.lastSeen > std::chrono::seconds(35))) {
                    g_earbudsState.left.present = false;
                }
                if (g_earbudsState.right.present && (now - g_earbudsState.right.lastSeen > std::chrono::seconds(35))) {
                    g_earbudsState.right.present = false;
                }
                if (g_earbudsState.caseBattery.present && (now - g_earbudsState.caseBattery.lastSeen > std::chrono::seconds(15))) {
                    g_earbudsState.caseBattery.present = false;
                }
            }
        } else if (cmd == 16414 || cmd == 57347 || cmd == 28688) { // ANC
            if (!payload.empty()) {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                AncMode mode = AncMode::Off;

                // EarWeb protocol: ancStatus is at payload[1] (full packet byte 9)
                uint8_t ancByte = (payload.size() >= 2) ? payload[1] : payload[0];
                if (ancByte == 0x05) {
                    mode = AncMode::Off;
                } else if (ancByte == 0x07) {
                    mode = AncMode::Transparency;
                } else if (ancByte == 0x01) {
                    mode = AncMode::High;
                } else if (ancByte == 0x03) {
                    mode = AncMode::Mid;
                } else if (ancByte == 0x02) {
                    mode = AncMode::Low;
                } else if (ancByte == 0x04) {
                    mode = AncMode::Adaptive;
                } else {
                    if (payload[0] == 0x02 || payload[0] == 0x07) mode = AncMode::Transparency;
                    else if (payload[0] == 0x03 || payload[0] == 0x05) mode = AncMode::Off;
                    else if (payload[0] == 0x01) mode = AncMode::High;
                }

                g_earbudsState.ancMode = mode;
            }
        } else if (cmd == 16415 || cmd == 16464 || cmd == 16469) { // EQ / ListeningMode
            if (!payload.empty()) {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                g_earbudsState.eqPreset = payload[0];
            }
        } else if (cmd == 16462) { // Bass
            if (payload.size() >= 2) {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                g_earbudsState.bassEnabled = (payload[0] != 0);
                if (payload[1] > 0) {
                    g_earbudsState.bassLevel = std::clamp<uint8_t>(
                        static_cast<uint8_t>(payload[1] / 2), 1, 5);
                }
            }
        } else if (cmd == 16449) { // Low latency
            if (!payload.empty()) {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                g_earbudsState.lowLatencyEnabled = (payload[0] == 0x01);
            }
        }
    }

    void WorkerLoop() {
        bool apartmentInitialized = false;
        try {
            init_apartment(winrt::apartment_type::multi_threaded);
            apartmentInitialized = true;
        } catch (...) {}
        unsigned int consecutiveConnectionFailures = 0;

        while (!m_stopRequested.load()) {
            if (!m_connected.load()) {
                BluetoothDevice targetBtDev = m_cachedDevice;
                RfcommDeviceService service{nullptr};
                std::wstring realName = m_cachedDeviceName;

                // 1. Find paired Nothing / CMF earbuds
                try {
                    if (!targetBtDev) {
                        auto btSelector = BluetoothDevice::GetDeviceSelectorFromPairingState(true);
                        auto paired = GetAsync(DeviceInformation::FindAllAsync(btSelector),
                                               std::chrono::seconds(10));
                        for (uint32_t i = 0; i < paired.Size(); ++i) {
                            auto dev = paired.GetAt(i);
                            std::wstring devName = dev.Name().c_str();
                            std::wstring lower = devName;
                            for (auto& c : lower) c = towlower(c);
                            if (lower.find(L"nothing") != std::wstring::npos ||
                                lower.find(L"cmf") != std::wstring::npos ||
                                lower.find(L"ear (") != std::wstring::npos ||
                                lower.find(L"ear(") != std::wstring::npos) {
                                auto btDev = GetAsync(BluetoothDevice::FromIdAsync(dev.Id()),
                                                      std::chrono::seconds(5));
                                if (btDev) {
                                    targetBtDev = btDev;
                                    realName = devName;
                                    CacheDevice(btDev, devName);
                                    break;
                                }
                            }
                        }
                    }
                } catch (...) {}

                // 2. Fallback: check RFCOMM selector, verifying device name
                if (!targetBtDev) {
                    try {
                        auto selector = RfcommDeviceService::GetDeviceSelector(RfcommServiceId::FromUuid(SppUuid()));
                        auto devices = GetAsync(DeviceInformation::FindAllAsync(selector),
                                                std::chrono::seconds(10));
                        for (uint32_t i = 0; i < devices.Size(); ++i) {
                            auto s = GetAsync(
                                RfcommDeviceService::FromIdAsync(devices.GetAt(i).Id()),
                                std::chrono::seconds(5));
                            if (s && s.Device()) {
                                std::wstring devName = s.Device().Name().c_str();
                                std::wstring lower = devName;
                                for (auto& c : lower) c = towlower(c);
                                if (lower.find(L"nothing") != std::wstring::npos ||
                                    lower.find(L"cmf") != std::wstring::npos ||
                                    lower.find(L"ear (") != std::wstring::npos ||
                                    lower.find(L"ear(") != std::wstring::npos) {
                                    targetBtDev = s.Device();
                                    service = s;
                                    realName = devName;
                                    CacheDevice(targetBtDev, devName);
                                    break;
                                }
                            }
                        }
                    } catch (...) {}
                }

                // Check if device is physically connected to Windows Bluetooth
                bool isDevConnected = false;
                if (targetBtDev) {
                    try {
                        isDevConnected = (targetBtDev.ConnectionStatus() == BluetoothConnectionStatus::Connected);
                    } catch (...) {
                        ClearCachedDevice();
                    }
                }

                if (!isDevConnected) {
                    consecutiveConnectionFailures = 0;
                    // Headphones are disconnected! DO NOT attempt to connect socket or poll RFCOMM.
                    {
                        std::lock_guard<std::mutex> lock(g_stateMutex);
                        g_earbudsState.connected = false;
                        g_earbudsState.connecting = false;
                        g_earbudsState.left.present = false;
                        g_earbudsState.right.present = false;
                        g_earbudsState.caseBattery.present = false;
                    }

                    // Reuse the matched BluetoothDevice and check it cheaply. If no
                    // matching device exists, back off before enumerating again.
                    WaitForWake(targetBtDev ? std::chrono::seconds(15)
                                            : std::chrono::seconds(30));
                    continue;
                }

                // Device is actively connected in Windows!
                {
                    std::lock_guard<std::mutex> lock(g_stateMutex);
                    g_earbudsState.connecting = true;
                    g_earbudsState.connected = false;
                }

                if (!service && targetBtDev) {
                    try {
                        auto rf = GetAsync(targetBtDev.GetRfcommServicesForIdAsync(
                                              RfcommServiceId::FromUuid(SppUuid()),
                                              BluetoothCacheMode::Uncached),
                                           std::chrono::seconds(10));
                        if (rf && rf.Services().Size() > 0) {
                            service = rf.Services().GetAt(0);
                        }
                    } catch (...) {}
                }

                bool connectionEstablished = false;
                if (service) {
                    try {
                        // Extract true friendly device name
                        if (realName.empty()) realName = L"CMF Buds 2";
                        try {
                            auto bt = service.Device();
                            if (bt && !bt.Name().empty()) {
                                realName = bt.Name().c_str();
                            }
                        } catch (...) {}

                        StreamSocket socket;
                        socket.Control().KeepAlive(true);
                        {
                            std::lock_guard<std::mutex> sLock(m_socketMutex);
                            m_socket = socket;
                            m_service = service;
                        }
                        GetAsync(socket.ConnectAsync(
                                     service.ConnectionHostName(),
                                     service.ConnectionServiceName(),
                                     SocketProtectionLevel::BluetoothEncryptionAllowNullAuthentication),
                                 std::chrono::seconds(10));

                        {
                            std::lock_guard<std::mutex> sLock(m_socketMutex);
                            if (m_stopRequested.load()) {
                                throw winrt::hresult_canceled();
                            }
                            m_connected.store(true);
                        }
                        connectionEstablished = true;
                        consecutiveConnectionFailures = 0;

                        {
                            std::lock_guard<std::mutex> lock(g_stateMutex);
                            g_earbudsState.connected = true;
                            g_earbudsState.connecting = false;
                            g_earbudsState.deviceName = realName;
                        }

                        // Query all settings upon initial connection
                        QueryAll();

                        // Enter reader loop
                        DataReader reader(socket.InputStream());
                        reader.InputStreamOptions(InputStreamOptions::None);

                        while (!m_stopRequested.load() && m_connected.load()) {
                            // Read header magic
                            auto readTimeout = std::chrono::seconds(
                                std::max(15, g_pollIntervalSeconds.load() + 10));
                            if (GetAsync(reader.LoadAsync(1), readTimeout) < 1) break;
                            if (reader.ReadByte() != 0x55) continue;

                            if (GetAsync(reader.LoadAsync(7), std::chrono::seconds(10)) < 7) break;
                            uint8_t b1 = reader.ReadByte(); // 0x60
                            uint8_t b2 = reader.ReadByte(); // 0x01
                            if (b1 != 0x60 || b2 != 0x01) continue;
                            uint8_t cmdLo = reader.ReadByte();
                            uint8_t cmdHi = reader.ReadByte();
                            uint16_t cmd = static_cast<uint16_t>(cmdLo | (cmdHi << 8));
                            uint8_t pLenLo = reader.ReadByte();
                            uint8_t pLenHi = reader.ReadByte();
                            uint16_t pLen = static_cast<uint16_t>(pLenLo | (pLenHi << 8));
                            uint8_t opId = reader.ReadByte(); // opId
                            constexpr uint16_t kMaxPacketPayload = 4096;
                            if (pLen > kMaxPacketPayload) {
                                Wh_Log(L"BluetoothManager: Rejecting oversized packet (%u bytes)", pLen);
                                continue;
                            }

                            uint32_t toRead = pLen + 2;
                            if (GetAsync(reader.LoadAsync(toRead), std::chrono::seconds(10)) < toRead) break;
                            std::vector<uint8_t> payload(pLen);
                            if (pLen > 0) reader.ReadBytes(payload);
                            uint8_t crcLo = reader.ReadByte();
                            uint8_t crcHi = reader.ReadByte();
                            uint16_t expectedCrc = static_cast<uint16_t>(crcLo | (crcHi << 8));

                            std::vector<uint8_t> packetData;
                            packetData.reserve(8 + pLen);
                            packetData.push_back(0x55);
                            packetData.push_back(b1);
                            packetData.push_back(b2);
                            packetData.push_back(cmdLo);
                            packetData.push_back(cmdHi);
                            packetData.push_back(pLenLo);
                            packetData.push_back(pLenHi);
                            packetData.push_back(opId);
                            packetData.insert(packetData.end(), payload.begin(), payload.end());

                            if (ComputeCrc16(packetData.data(), packetData.size()) != expectedCrc) {
                                continue;
                            }

                            ProcessPacket(cmd, payload);
                        }
                    } catch (...) {}
                }

                if (!connectionEstablished) {
                    consecutiveConnectionFailures++;
                }
                Disconnect();

                // Avoid repeated over-the-air service discovery when RFCOMM is
                // unavailable, while still reconnecting quickly after a normal
                // established connection drops.
                unsigned int backoffStep = std::min(consecutiveConnectionFailures, 4u);
                int retrySeconds = connectionEstablished
                    ? 5
                    : std::min(60, 5 * (1 << backoffStep));
                WaitForWake(std::chrono::seconds(retrySeconds));
            }
        }
        ClearCachedDevice();
        if (apartmentInitialized) {
            winrt::uninit_apartment();
        }
    }
};

using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);
static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;
static void* CTaskBand_ITaskListWndSite_vftable = nullptr;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void**);
static CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void*);
static TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
using Std_Ref_Decref_t = void(WINAPI*)(void*);
static Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;

using WindowThreadProc = void(*)(void*);
static bool RunFromWindowThread(HWND hWnd, WindowThreadProc proc, void* param) {
    static const UINT kMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    if (!kMsg) return false;
    struct Payload {
        WindowThreadProc proc;
        void* param;
        std::atomic<bool> succeeded{false};
    };
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) {
        try {
            proc(param);
            return true;
        } catch (...) {
            Wh_Log(L"RunFromWindowThread: callback failed");
            return false;
        }
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC,
        [](int code, WPARAM w, LPARAM l) CALLBACK -> LRESULT {
            if (code == HC_ACTION) {
                auto* cwp = reinterpret_cast<const CWPSTRUCT*>(l);
                static const UINT kM = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == kM) {
                    auto* p = reinterpret_cast<Payload*>(cwp->lParam);
                    try {
                        p->proc(p->param);
                        p->succeeded.store(true);
                    } catch (...) {
                        Wh_Log(L"RunFromWindowThread: callback failed");
                    }
                }
            }
            return CallNextHookEx(nullptr, code, w, l);
        }, nullptr, tid);
    if (!hook) return false;
    Payload pay{proc, param};
    SendMessageW(hWnd, kMsg, 0, reinterpret_cast<LPARAM>(&pay));
    UnhookWindowsHookEx(hook);
    return pay.succeeded.load();
}

static bool IsReadableMemoryRange(const void* address, size_t size) {
    if (!address || size == 0) return false;
    MEMORY_BASIC_INFORMATION memory{};
    if (!VirtualQuery(address, &memory, sizeof(memory)) ||
        memory.State != MEM_COMMIT ||
        (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }
    const auto start = reinterpret_cast<uintptr_t>(address);
    const auto regionStart = reinterpret_cast<uintptr_t>(memory.BaseAddress);
    const auto regionEnd = regionStart + memory.RegionSize;
    return start >= regionStart && start <= regionEnd && size <= regionEnd - start;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM lp) CALLBACK -> BOOL {
        DWORD pid = 0; wchar_t cls[32] = {};
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
            _wcsicmp(cls, L"Shell_TrayWnd") == 0)
        {
            *reinterpret_cast<HWND*>(lp) = hWnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetPropW(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;

    void* taskBand = (void*)GetWindowLongPtrW(hTaskSwWnd, 0);
    if (!taskBand) return nullptr;

    void* expectedVftable = CTaskBand_ITaskListWndSite_vftable;
    auto getTaskbarHost = CTaskBand_GetTaskbarHost_Original;

    if (!expectedVftable || !getTaskbarHost) return nullptr;

    void* taskBandForTaskListWndSite = taskBand;
    int i = 0;
    constexpr int kMaxSlotsToScan = 20;
    for (;; i++) {
        if (!IsReadableMemoryRange(taskBandForTaskListWndSite, sizeof(void*))) return nullptr;
        if (*(void**)taskBandForTaskListWndSite == expectedVftable) break;
        if (i == kMaxSlotsToScan) return nullptr;
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskBandForTaskListWndSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0]) {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0;
    bool frameHeightPatternRecognized = false;
#if defined(_M_X64) || defined(__x86_64__)
    {
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (IsReadableMemoryRange(b, 8) &&
            b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
            frameHeightPatternRecognized = true;
        }
    }
#elif defined(_M_ARM64) || defined(__aarch64__)
    {
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (IsReadableMemoryRange(p, sizeof(DWORD) * 4) &&
            p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
            frameHeightPatternRecognized = true;
        }
    }
#else
    taskbarElementIUnknownOffset = 0x10;
    frameHeightPatternRecognized = true;
#endif

    if (!frameHeightPatternRecognized ||
        !IsReadableMemoryRange(
            static_cast<BYTE*>(taskbarHostSharedPtr[0]) + taskbarElementIUnknownOffset,
            sizeof(IUnknown*))) {
        Wh_Log(L"GetTaskbarXamlRoot: FrameHeight pattern not recognized or offset unreadable, aborting");
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] + taskbarElementIUnknownOffset);
    if (!taskbarElementIUnknown) {
        Wh_Log(L"GetTaskbarXamlRoot: taskbarElementIUnknown is null, aborting");
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    FrameworkElement taskbarElement{nullptr};
    HRESULT hr = taskbarElementIUnknown->QueryInterface(
        winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original)
        Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
    return SUCCEEDED(hr) ? result : nullptr;
}

static FrameworkElement FindChildByName(DependencyObject parent, std::wstring_view name) {
    if (!parent) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(parent, i);
        if (auto fe = child.try_as<FrameworkElement>()) {
            if (fe.Name() == name) return fe;
        }
        auto res = FindChildByName(child, name);
        if (res) return res;
    }
    return nullptr;
}

enum class StringId {
    Connected,
    Connecting,
    Disconnected,
    LeftBud,
    RightBud,
    Case,
    InCase,
    NotPresent,
    Active,
    Charging,
    NoiseControl,
    NoiseCancellation,
    Transparency,
    Off,
    AncHigh,
    AncMid,
    AncLow,
    AncAdaptive,
    SingleBudWarning,
    UltraBass,
    Equalizer,
    Dirac,
    Balanced,
    MoreTreble,
    MoreBass,
    Pop,
    Voice,
    LowLatencyOn,
    LowLatencyOff,
    RingLeft,
    RingRight,
    StopLeft,
    StopRight,
    Refresh,
};

static inline bool IsRussian() {
    if (g_settings.language == L"ru") return true;
    if (g_settings.language == L"en") return false;
    LANGID lang = GetUserDefaultUILanguage();
    return (PRIMARYLANGID(lang) == LANG_RUSSIAN);
}

static std::wstring Loc(StringId id) {
    bool ru = IsRussian();
    switch (id) {
    case StringId::Connected:           return ru ? L"Подключено" : L"Connected";
    case StringId::Connecting:          return ru ? L"Подключение..." : L"Connecting...";
    case StringId::Disconnected:        return ru ? L"Отключено" : L"Disconnected";
    case StringId::LeftBud:             return ru ? L"Левый" : L"Left";
    case StringId::RightBud:            return ru ? L"Правый" : L"Right";
    case StringId::Case:                return ru ? L"Кейс" : L"Case";
    case StringId::InCase:              return ru ? L"В кейсе" : L"In case";
    case StringId::NotPresent:          return ru ? L"Отключен" : L"Disconnected";
    case StringId::Active:              return ru ? L"Активен" : L"Active";
    case StringId::Charging:            return ru ? L"Зарядка" : L"Charging";
    case StringId::NoiseControl:        return ru ? L"Шумоподавление" : L"Noise Control";
    case StringId::NoiseCancellation:   return ru ? L"Шумоподавление" : L"Noise Cancellation";
    case StringId::Transparency:        return ru ? L"Прозрачность" : L"Transparency";
    case StringId::Off:                 return ru ? L"Выкл" : L"Off";
    case StringId::AncHigh:             return ru ? L"Высокое" : L"High";
    case StringId::AncMid:              return ru ? L"Среднее" : L"Mid";
    case StringId::AncLow:              return ru ? L"Низкое" : L"Low";
    case StringId::AncAdaptive:         return ru ? L"Адаптивное" : L"Adaptive";
    case StringId::SingleBudWarning:    return ru ? L"Один наушник: ANC ограничен" : L"Single earbud: ANC limited";
    case StringId::UltraBass:           return L"Ultra Bass";
    case StringId::Equalizer:           return ru ? L"Эквалайзер" : L"Equalizer";
    case StringId::Dirac:               return L"Dirac Opteo";
    case StringId::Balanced:            return ru ? L"Баланс" : L"Balanced";
    case StringId::MoreTreble:          return ru ? L"Больше ВЧ" : L"More Treble";
    case StringId::MoreBass:            return ru ? L"Больше НЧ" : L"More Bass";
    case StringId::Pop:                 return ru ? L"Поп" : L"Pop";
    case StringId::Voice:               return ru ? L"Голос" : L"Voice";
    case StringId::LowLatencyOn:        return ru ? L"Игровой режим: Вкл" : L"Game Mode: On";
    case StringId::LowLatencyOff:       return ru ? L"Игровой режим: Выкл" : L"Game Mode: Off";
    case StringId::RingLeft:            return ru ? L"Звонок L" : L"Ring L";
    case StringId::RingRight:           return ru ? L"Звонок R" : L"Ring R";
    case StringId::StopLeft:            return ru ? L"Стоп L" : L"Stop L";
    case StringId::StopRight:           return ru ? L"Стоп R" : L"Stop R";
    case StringId::Refresh:             return ru ? L"Обновить" : L"Refresh";
    default:                            return L"";
    }
}

inline wchar_t GetBatteryGlyph(int percent, bool charging) {
    if (percent < 0) return L'\uE83F';
    percent = std::clamp(percent, 0, 100);
    int step = (percent + 5) / 10;
    step = std::clamp(step, 0, 10);
    if (charging) {
        return static_cast<wchar_t>(0xEBAB + step);
    } else {
        return static_cast<wchar_t>(0xE850 + step);
    }
}

static std::function<void()> s_refreshFlyoutUi = nullptr;
static bool s_flyoutOpen = false;

struct BatteryCardControls {
    Border container{nullptr};
    TextBlock icon{nullptr};
    TextBlock label{nullptr};
    TextBlock battIcon{nullptr};
    TextBlock pctText{nullptr};
    TextBlock statusText{nullptr};
    ProgressBar pb{nullptr};
};

static BatteryCardControls CreateBatteryCard(const std::wstring& labelStr, bool isBud) {
    BatteryCardControls card;
    card.container = Border();
    card.container.CornerRadius({6, 6, 6, 6});
    card.container.Background(SolidColorBrush(Color{0xFF, 0x2A, 0x2A, 0x2E}));
    card.container.BorderBrush(SolidColorBrush(Color{0x1A, 0xFF, 0xFF, 0xFF}));
    card.container.BorderThickness({1, 1, 1, 1});
    card.container.Padding({6, 8, 6, 8});

    StackPanel layout;
    layout.Spacing(3);
    layout.HorizontalAlignment(HorizontalAlignment::Center);

    StackPanel header;
    header.Orientation(Orientation::Horizontal);
    header.Spacing(4);
    header.HorizontalAlignment(HorizontalAlignment::Center);

    card.icon = TextBlock();
    card.icon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    card.icon.Text(isBud ? L"\uE7F6" : L"\uE8D6");
    card.icon.FontSize(11);
    card.icon.VerticalAlignment(VerticalAlignment::Center);
    header.Children().Append(card.icon);

    card.label = TextBlock();
    card.label.Text(labelStr);
    card.label.FontSize(11);
    card.label.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    card.label.VerticalAlignment(VerticalAlignment::Center);
    header.Children().Append(card.label);
    layout.Children().Append(header);

    card.battIcon = TextBlock();
    card.battIcon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    card.battIcon.FontSize(24);
    card.battIcon.HorizontalAlignment(HorizontalAlignment::Center);
    layout.Children().Append(card.battIcon);

    card.pctText = TextBlock();
    card.pctText.FontSize(14);
    card.pctText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    card.pctText.HorizontalAlignment(HorizontalAlignment::Center);
    layout.Children().Append(card.pctText);

    card.pb = ProgressBar();
    card.pb.Minimum(0);
    card.pb.Maximum(100);
    card.pb.Height(3);
    card.pb.CornerRadius({2, 2, 2, 2});
    card.pb.Margin({2, 2, 2, 0});
    layout.Children().Append(card.pb);

    card.statusText = TextBlock();
    card.statusText.FontSize(9.5);
    card.statusText.HorizontalAlignment(HorizontalAlignment::Center);
    card.statusText.Opacity(0.5);
    layout.Children().Append(card.statusText);

    card.container.Child(layout);
    return card;
}

static void UpdateBatteryCard(const BatteryCardControls& card, const BatteryReading& reading, bool isBud) {
    if (!card.container || !card.battIcon || !card.pctText || !card.pb || !card.statusText) return;
    card.battIcon.Text(std::wstring(1, GetBatteryGlyph(reading.present ? reading.percent : -1, reading.charging)));
    if (reading.present && reading.percent >= 0) {
        card.container.Opacity(1.0);
        card.pctText.Text(std::to_wstring(reading.percent) + L"%");
        card.pb.Value(reading.percent);
        card.pb.Visibility(Visibility::Visible);
        if (reading.percent <= 20) {
            card.battIcon.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
            card.pb.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
        } else {
            card.battIcon.Foreground(SolidColorBrush(Color{0xFF, 0x4C, 0xAF, 0x50}));
            card.pb.Foreground(SolidColorBrush(Color{0xFF, 0x4C, 0xAF, 0x50}));
        }
        card.statusText.Text(reading.charging ? Loc(StringId::Charging) : (isBud ? Loc(StringId::Active) : Loc(StringId::InCase)));
    } else {
        card.container.Opacity(0.45);
        card.pctText.Text(L"—");
        card.pb.Visibility(Visibility::Collapsed);
        card.battIcon.Foreground(SolidColorBrush(Color{0x80, 0xFF, 0xFF, 0xFF}));
        card.statusText.Text(isBud ? Loc(StringId::NotPresent) : L"—");
    }
}

struct AncBtnInfo {
    Button btn{nullptr};
    TextBlock txt{nullptr};
    AncMode mode;
};

struct BassLvlInfo {
    Button btn{nullptr};
    TextBlock txt{nullptr};
    uint8_t lvl;
};

struct EqBtnInfo {
    Button btn{nullptr};
    TextBlock txt{nullptr};
    uint8_t preset;
};

struct FlyoutContext {
    TextBlock title{nullptr};
    TextBlock statusText{nullptr};
    BatteryCardControls leftCard;
    BatteryCardControls caseCard;
    BatteryCardControls rightCard;
    Border warnBadge{nullptr};
    Border ancCard{nullptr};
    std::vector<AncBtnInfo> mainAncBtns;
    StackPanel subSp{nullptr};
    std::vector<AncBtnInfo> subAncBtns;
    Border bassCard{nullptr};
    ToggleSwitch bassToggle{nullptr};
    std::vector<BassLvlInfo> bassBtns;
    Border eqCard{nullptr};
    std::vector<EqBtnInfo> eqBtns;
    Grid toolsGrid{nullptr};
    Button lowLatencyBtn{nullptr};
    TextBlock lowLatencyIcon{nullptr};
    TextBlock lowLatencyTxt{nullptr};
    Button ringLBtn{nullptr};
    TextBlock ringLIcon{nullptr};
    TextBlock ringLTxt{nullptr};
    Button ringRBtn{nullptr};
    TextBlock ringRIcon{nullptr};
    TextBlock ringRTxt{nullptr};
    bool isRingingL = false;
    bool isRingingR = false;
    DispatcherTimer ringTimerL{nullptr};
    DispatcherTimer ringTimerR{nullptr};
    winrt::event_token ringTimerLToken{};
    winrt::event_token ringTimerRToken{};
    bool isConnectedState = true;
    bool suppressBassToggle = false;

    void StopRingTimers() {
        if (ringTimerL) {
            try { ringTimerL.Stop(); } catch (...) {}
            if (ringTimerLToken.value) {
                try { ringTimerL.Tick(ringTimerLToken); } catch (...) {}
            }
            ringTimerL = nullptr;
        }
        ringTimerLToken = {};
        if (ringTimerR) {
            try { ringTimerR.Stop(); } catch (...) {}
            if (ringTimerRToken.value) {
                try { ringTimerR.Tick(ringTimerRToken); } catch (...) {}
            }
            ringTimerR = nullptr;
        }
        ringTimerRToken = {};
    }

    void UpdateConnectionState(bool connected, bool leftPresent, bool rightPresent) {
        isConnectedState = connected;
        double targetOpacity = connected ? 1.0 : 0.38;
        bool singleBud = leftPresent ^ rightPresent;

        if (ancCard) ancCard.Opacity(targetOpacity);
        for (auto& item : mainAncBtns) {
            if (item.btn) {
                item.btn.IsEnabled(connected &&
                                   !(singleBud && item.mode == AncMode::High));
            }
        }
        for (auto& subItem : subAncBtns) {
            if (subItem.btn) subItem.btn.IsEnabled(connected && !singleBud);
        }

        if (bassCard) bassCard.Opacity(targetOpacity);
        if (bassToggle) bassToggle.IsEnabled(connected);
        for (auto& bassItem : bassBtns) {
            if (bassItem.btn) bassItem.btn.IsEnabled(connected);
        }

        if (eqCard) eqCard.Opacity(targetOpacity);
        for (auto& eqItem : eqBtns) {
            if (eqItem.btn) eqItem.btn.IsEnabled(connected);
        }

        if (toolsGrid) toolsGrid.Opacity(targetOpacity);
        if (lowLatencyBtn) lowLatencyBtn.IsEnabled(connected);
        if (ringLBtn) ringLBtn.IsEnabled(connected && leftPresent);
        if (ringRBtn) ringRBtn.IsEnabled(connected && rightPresent);
    }

    void ApplyAncVisuals(AncMode currentMode) {
        bool isNoise = (currentMode == AncMode::High || currentMode == AncMode::Mid || currentMode == AncMode::Low || currentMode == AncMode::Adaptive);
        for (auto& item : mainAncBtns) {
            bool active = false;
            if (item.mode == AncMode::High) active = isNoise;
            else if (item.mode == AncMode::Transparency) active = (currentMode == AncMode::Transparency);
            else if (item.mode == AncMode::Off) active = (currentMode == AncMode::Off);

            if (active) {
                item.btn.Background(SolidColorBrush(Color{0xFF, 0xFF, 0xFF, 0xFF}));
                item.btn.BorderThickness({0, 0, 0, 0});
                item.txt.Foreground(SolidColorBrush(Color{0xFF, 0x12, 0x12, 0x14}));
                item.txt.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
            } else {
                item.btn.Background(SolidColorBrush(Colors::Transparent()));
                item.btn.BorderThickness({0, 0, 0, 0});
                item.txt.Foreground(SolidColorBrush(Color{0x99, 0xFF, 0xFF, 0xFF}));
                item.txt.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
            }
        }

        if (subSp) subSp.Visibility(isNoise ? Visibility::Visible : Visibility::Collapsed);
        if (isNoise) {
            for (auto& subItem : subAncBtns) {
                bool sActive = (subItem.mode == currentMode);
                if (sActive) {
                    subItem.btn.Background(SolidColorBrush(Color{0xFF, 0xFF, 0xFF, 0xFF}));
                    subItem.btn.BorderThickness({0, 0, 0, 0});
                    subItem.txt.Foreground(SolidColorBrush(Color{0xFF, 0x12, 0x12, 0x14}));
                    subItem.txt.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
                } else {
                    subItem.btn.Background(SolidColorBrush(Color{0x0D, 0xFF, 0xFF, 0xFF}));
                    subItem.btn.BorderThickness({0, 0, 0, 0});
                    subItem.txt.Foreground(SolidColorBrush(Color{0x80, 0xFF, 0xFF, 0xFF}));
                    subItem.txt.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
                }
            }
        }
    }

    void ApplyBassVisuals(bool enabled, uint8_t level) {
        for (auto& item : bassBtns) {
            bool active = (enabled && item.lvl == level);
            if (active) {
                item.btn.Background(SolidColorBrush(Color{0xFF, 0xFF, 0xFF, 0xFF}));
                item.btn.BorderThickness({0, 0, 0, 0});
                item.txt.Foreground(SolidColorBrush(Color{0xFF, 0x12, 0x12, 0x14}));
                item.txt.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
            } else {
                item.btn.Background(SolidColorBrush(Colors::Transparent()));
                item.btn.BorderThickness({0, 0, 0, 0});
                item.txt.Foreground(SolidColorBrush(Color{0x80, 0xFF, 0xFF, 0xFF}));
                item.txt.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
            }
            item.btn.Opacity((enabled && isConnectedState) ? 1.0 : 0.4);
        }
    }

    void ApplyEqVisuals(uint8_t currentPreset) {
        for (auto& item : eqBtns) {
            bool active = (item.preset == currentPreset);
            if (active) {
                item.btn.Background(SolidColorBrush(Color{0xFF, 0xFF, 0xFF, 0xFF}));
                item.btn.BorderThickness({0, 0, 0, 0});
                item.txt.Foreground(SolidColorBrush(Color{0xFF, 0x12, 0x12, 0x14}));
                item.txt.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
            } else {
                item.btn.Background(SolidColorBrush(Color{0x0D, 0xFF, 0xFF, 0xFF}));
                item.btn.BorderThickness({0, 0, 0, 0});
                item.txt.Foreground(SolidColorBrush(Color{0x99, 0xFF, 0xFF, 0xFF}));
                item.txt.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
            }
        }
    }

    void ApplyLowLatencyVisuals(bool enabled) {
        if (!lowLatencyBtn || !lowLatencyTxt || !lowLatencyIcon) return;
        if (enabled) {
            lowLatencyBtn.Background(SolidColorBrush(Color{0xFF, 0xFF, 0xFF, 0xFF}));
            lowLatencyBtn.BorderThickness({0, 0, 0, 0});
            lowLatencyIcon.Foreground(SolidColorBrush(Color{0xFF, 0x12, 0x12, 0x14}));
            lowLatencyTxt.Text(Loc(StringId::LowLatencyOn));
            lowLatencyTxt.Foreground(SolidColorBrush(Color{0xFF, 0x12, 0x12, 0x14}));
            lowLatencyTxt.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        } else {
            lowLatencyBtn.Background(SolidColorBrush(Color{0x0D, 0xFF, 0xFF, 0xFF}));
            lowLatencyBtn.BorderThickness({0, 0, 0, 0});
            lowLatencyIcon.Foreground(SolidColorBrush(Color{0x99, 0xFF, 0xFF, 0xFF}));
            lowLatencyTxt.Text(Loc(StringId::LowLatencyOff));
            lowLatencyTxt.Foreground(SolidColorBrush(Color{0x99, 0xFF, 0xFF, 0xFF}));
            lowLatencyTxt.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
        }
    }
};

[[clang::no_destroy]] static std::shared_ptr<FlyoutContext> s_currentFlyoutCtx;

static UIElement BuildFlyoutContent() {
    ScrollViewer sv;
    sv.VerticalScrollBarVisibility(ScrollBarVisibility::Auto);
    sv.HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
    sv.MaxHeight(580);

    Border rootBorder;
    rootBorder.CornerRadius({8, 8, 8, 8});
    rootBorder.Background(SolidColorBrush(Color{0xF8, 0x1E, 0x1E, 0x20}));
    rootBorder.BorderBrush(SolidColorBrush(Color{0x1F, 0xFF, 0xFF, 0xFF}));
    rootBorder.BorderThickness({1, 1, 1, 1});
    rootBorder.Padding({12, 12, 12, 12});
    rootBorder.Width(365);

    StackPanel mainSp;
    mainSp.Spacing(10);

    auto ctx = std::make_shared<FlyoutContext>();
    s_currentFlyoutCtx = ctx;

    EarbudsState state;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        state = g_earbudsState;
    }

    Grid headerGrid;
    ColumnDefinition cName, cRefresh;
    cName.Width({1.0, GridUnitType::Star});
    cRefresh.Width({1.0, GridUnitType::Auto});
    headerGrid.ColumnDefinitions().Append(cName);
    headerGrid.ColumnDefinitions().Append(cRefresh);

    StackPanel titleSp;
    titleSp.Spacing(2);

    ctx->title = TextBlock();
    ctx->title.Text(state.deviceName.empty() ? L"CMF Buds 2" : state.deviceName);
    ctx->title.FontSize(15);
    ctx->title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    titleSp.Children().Append(ctx->title);

    ctx->statusText = TextBlock();
    if (state.connected) {
        ctx->statusText.Text(L"● " + Loc(StringId::Connected));
        ctx->statusText.Foreground(SolidColorBrush(Color{0xFF, 0x4C, 0xAF, 0x50}));
    } else if (state.connecting) {
        ctx->statusText.Text(L"● " + Loc(StringId::Connecting));
        ctx->statusText.Foreground(SolidColorBrush(Color{0xFF, 0xFF, 0x98, 0x00}));
    } else {
        ctx->statusText.Text(L"● " + Loc(StringId::Disconnected));
        ctx->statusText.Opacity(0.5);
    }
    ctx->statusText.FontSize(11);
    titleSp.Children().Append(ctx->statusText);
    Grid::SetColumn(titleSp, 0);
    headerGrid.Children().Append(titleSp);

    Button refreshBtn;
    refreshBtn.Padding({6, 4, 6, 4});
    refreshBtn.CornerRadius({4, 4, 4, 4});
    refreshBtn.Background(SolidColorBrush(Color{0x08, 0xFF, 0xFF, 0xFF}));
    refreshBtn.BorderThickness({0, 0, 0, 0});
    TextBlock refreshIcon;
    refreshIcon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    refreshIcon.Text(L"\uE72C");
    refreshIcon.FontSize(12);
    refreshBtn.Content(refreshIcon);
    ToolTipService::SetToolTip(refreshBtn, winrt::box_value(Loc(StringId::Refresh)));
    refreshBtn.Click([](auto const&, auto const&) {
        BluetoothManager::Instance().PostAction([]() {
            BluetoothManager::Instance().QueryAll();
        });
    });
    Grid::SetColumn(refreshBtn, 1);
    headerGrid.Children().Append(refreshBtn);

    mainSp.Children().Append(headerGrid);

    Grid battGrid;
    battGrid.Margin({0, 2, 0, 2});
    for (int i = 0; i < 3; ++i) {
        ColumnDefinition col;
        col.Width({1.0, GridUnitType::Star});
        battGrid.ColumnDefinitions().Append(col);
    }

    ctx->leftCard = CreateBatteryCard(Loc(StringId::LeftBud), true);
    Grid::SetColumn(ctx->leftCard.container, 0);
    ctx->leftCard.container.Margin({0, 0, 3, 0});
    battGrid.Children().Append(ctx->leftCard.container);

    ctx->caseCard = CreateBatteryCard(Loc(StringId::Case), false);
    Grid::SetColumn(ctx->caseCard.container, 1);
    ctx->caseCard.container.Margin({3, 0, 3, 0});
    battGrid.Children().Append(ctx->caseCard.container);

    ctx->rightCard = CreateBatteryCard(Loc(StringId::RightBud), true);
    Grid::SetColumn(ctx->rightCard.container, 2);
    ctx->rightCard.container.Margin({3, 0, 0, 0});
    battGrid.Children().Append(ctx->rightCard.container);

    UpdateBatteryCard(ctx->leftCard, state.left, true);
    UpdateBatteryCard(ctx->caseCard, state.caseBattery, false);
    UpdateBatteryCard(ctx->rightCard, state.right, true);

    mainSp.Children().Append(battGrid);

    ctx->ancCard = Border();
    ctx->ancCard.Background(SolidColorBrush(Color{0xFF, 0x2A, 0x2A, 0x2E}));
    ctx->ancCard.BorderBrush(SolidColorBrush(Color{0x1A, 0xFF, 0xFF, 0xFF}));
    ctx->ancCard.BorderThickness({1, 1, 1, 1});
    ctx->ancCard.CornerRadius({6, 6, 6, 6});
    ctx->ancCard.Padding({10, 8, 10, 8});

    StackPanel ancSp;
    ancSp.Spacing(6);

    TextBlock ancTitle;
    ancTitle.Text(Loc(StringId::NoiseControl));
    ancTitle.FontSize(12);
    ancTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    ancSp.Children().Append(ancTitle);

    ctx->warnBadge = Border();
    ctx->warnBadge.CornerRadius({4, 4, 4, 4});
    ctx->warnBadge.Background(SolidColorBrush(Color{0x18, 0xFF, 0x98, 0x00}));
    ctx->warnBadge.Padding({6, 3, 6, 3});

    StackPanel warnSp;
    warnSp.Orientation(Orientation::Horizontal);
    warnSp.Spacing(5);
    warnSp.VerticalAlignment(VerticalAlignment::Center);

    TextBlock warnIcon;
    warnIcon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    warnIcon.Text(L"\uE7BA");
    warnIcon.FontSize(11);
    warnIcon.Foreground(SolidColorBrush(Color{0xFF, 0xFF, 0xB7, 0x4D}));
    warnIcon.VerticalAlignment(VerticalAlignment::Center);
    warnSp.Children().Append(warnIcon);

    TextBlock warnText;
    warnText.Text(Loc(StringId::SingleBudWarning));
    warnText.FontSize(10.5);
    warnText.Foreground(SolidColorBrush(Color{0xFF, 0xFF, 0xB7, 0x4D}));
    warnText.VerticalAlignment(VerticalAlignment::Center);
    warnSp.Children().Append(warnText);

    ctx->warnBadge.Child(warnSp);
    bool isSingleBud = (state.left.present ^ state.right.present);
    ctx->warnBadge.Visibility(isSingleBud ? Visibility::Visible : Visibility::Collapsed);
    ancSp.Children().Append(ctx->warnBadge);

    Border segTrack;
    segTrack.CornerRadius({5, 5, 5, 5});
    segTrack.Background(SolidColorBrush(Color{0x08, 0xFF, 0xFF, 0xFF}));
    segTrack.Padding({3, 3, 3, 3});

    Grid segGrid;
    for (int i = 0; i < 3; ++i) {
        ColumnDefinition col;
        col.Width({1.0, GridUnitType::Star});
        segGrid.ColumnDefinitions().Append(col);
    }

    const std::vector<std::pair<StringId, AncMode>> mainModes = {
        {StringId::NoiseCancellation, AncMode::High},
        {StringId::Transparency, AncMode::Transparency},
        {StringId::Off, AncMode::Off}
    };

    ctx->subSp = StackPanel();
    ctx->subSp.Spacing(4);
    ctx->subSp.Margin({0, 2, 0, 0});

    Grid subGrid;
    for (int i = 0; i < 4; ++i) {
        ColumnDefinition col;
        col.Width({1.0, GridUnitType::Star});
        subGrid.ColumnDefinitions().Append(col);
    }

    const std::vector<std::pair<StringId, AncMode>> subModes = {
        {StringId::AncHigh, AncMode::High},
        {StringId::AncMid, AncMode::Mid},
        {StringId::AncLow, AncMode::Low},
        {StringId::AncAdaptive, AncMode::Adaptive}
    };

    for (size_t i = 0; i < mainModes.size(); ++i) {
        AncBtnInfo item;
        item.mode = mainModes[i].second;
        item.btn = Button();
        item.btn.HorizontalAlignment(HorizontalAlignment::Stretch);
        item.btn.Padding({4, 6, 4, 6});
        item.btn.CornerRadius({4, 4, 4, 4});

        item.txt = TextBlock();
        item.txt.Text(Loc(mainModes[i].first));
        item.txt.FontSize(11.5);
        item.txt.HorizontalAlignment(HorizontalAlignment::Center);
        item.btn.Content(item.txt);

        AncMode m = item.mode;
        std::weak_ptr<FlyoutContext> weakCtx = ctx;
        item.btn.Click([weakCtx, m](auto const&, auto const&) {
            auto ctx = weakCtx.lock();
            if (!ctx) return;
            ctx->ApplyAncVisuals(m);
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                g_earbudsState.ancMode = m;
            }
            BluetoothManager::Instance().PostAction([m]() {
                BluetoothManager::Instance().SetAnc(m);
            });
        });

        Grid::SetColumn(item.btn, (int)i);
        if (i > 0) item.btn.Margin({2, 0, 0, 0});
        segGrid.Children().Append(item.btn);
        ctx->mainAncBtns.push_back(item);
    }
    segTrack.Child(segGrid);
    ancSp.Children().Append(segTrack);

    for (size_t i = 0; i < subModes.size(); ++i) {
        AncBtnInfo subItem;
        subItem.mode = subModes[i].second;
        subItem.btn = Button();
        subItem.btn.HorizontalAlignment(HorizontalAlignment::Stretch);
        subItem.btn.Padding({2, 3, 2, 3});
        subItem.btn.CornerRadius({4, 4, 4, 4});

        subItem.txt = TextBlock();
        subItem.txt.Text(Loc(subModes[i].first));
        subItem.txt.FontSize(10);
        subItem.txt.HorizontalAlignment(HorizontalAlignment::Center);
        subItem.btn.Content(subItem.txt);

        AncMode sm = subItem.mode;
        std::weak_ptr<FlyoutContext> weakCtx = ctx;
        subItem.btn.Click([weakCtx, sm](auto const&, auto const&) {
            auto ctx = weakCtx.lock();
            if (!ctx) return;
            ctx->ApplyAncVisuals(sm);
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                g_earbudsState.ancMode = sm;
            }
            BluetoothManager::Instance().PostAction([sm]() {
                BluetoothManager::Instance().SetAnc(sm);
            });
        });

        Grid::SetColumn(subItem.btn, (int)i);
        if (i > 0) subItem.btn.Margin({2, 0, 0, 0});
        subGrid.Children().Append(subItem.btn);
        ctx->subAncBtns.push_back(subItem);
    }
    ctx->subSp.Children().Append(subGrid);
    ancSp.Children().Append(ctx->subSp);

    ctx->ApplyAncVisuals(state.ancMode);
    ctx->ancCard.Child(ancSp);
    mainSp.Children().Append(ctx->ancCard);

    ctx->bassCard = Border();
    ctx->bassCard.Background(SolidColorBrush(Color{0xFF, 0x2A, 0x2A, 0x2E}));
    ctx->bassCard.BorderBrush(SolidColorBrush(Color{0x1A, 0xFF, 0xFF, 0xFF}));
    ctx->bassCard.BorderThickness({1, 1, 1, 1});
    ctx->bassCard.CornerRadius({6, 6, 6, 6});
    ctx->bassCard.Padding({10, 8, 10, 8});

    StackPanel bassSp;
    bassSp.Spacing(8);

    Grid bassHeader;
    ColumnDefinition colTitle, colToggle;
    colTitle.Width({1.0, GridUnitType::Star});
    colToggle.Width({1.0, GridUnitType::Auto});
    bassHeader.ColumnDefinitions().Append(colTitle);
    bassHeader.ColumnDefinitions().Append(colToggle);

    TextBlock bassTitle;
    bassTitle.Text(Loc(StringId::UltraBass));
    bassTitle.FontSize(12);
    bassTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    bassTitle.VerticalAlignment(VerticalAlignment::Center);
    Grid::SetColumn(bassTitle, 0);
    bassHeader.Children().Append(bassTitle);

    ctx->bassToggle = ToggleSwitch();
    ctx->bassToggle.IsOn(state.bassEnabled);
    ctx->bassToggle.OnContent(winrt::box_value(L""));
    ctx->bassToggle.OffContent(winrt::box_value(L""));
    Grid::SetColumn(ctx->bassToggle, 1);
    bassHeader.Children().Append(ctx->bassToggle);
    bassSp.Children().Append(bassHeader);

    Border bassTrack;
    bassTrack.CornerRadius({5, 5, 5, 5});
    bassTrack.Background(SolidColorBrush(Color{0x08, 0xFF, 0xFF, 0xFF}));
    bassTrack.Padding({3, 3, 3, 3});

    Grid bassLevelsGrid;
    for (int i = 0; i < 5; ++i) {
        ColumnDefinition col;
        col.Width({1.0, GridUnitType::Star});
        bassLevelsGrid.ColumnDefinitions().Append(col);
    }

    for (int i = 0; i < 5; ++i) {
        BassLvlInfo item;
        item.lvl = static_cast<uint8_t>(i + 1);
        item.btn = Button();
        item.btn.HorizontalAlignment(HorizontalAlignment::Stretch);
        item.btn.Padding({2, 4, 2, 4});
        item.btn.CornerRadius({4, 4, 4, 4});

        item.txt = TextBlock();
        item.txt.Text(std::to_wstring(item.lvl));
        item.txt.FontSize(11);
        item.txt.HorizontalAlignment(HorizontalAlignment::Center);
        item.btn.Content(item.txt);

        uint8_t lvl = item.lvl;
        std::weak_ptr<FlyoutContext> weakCtx = ctx;
        item.btn.Click([weakCtx, lvl](auto const&, auto const&) {
            auto ctx = weakCtx.lock();
            if (!ctx) return;
            if (ctx->bassToggle && !ctx->bassToggle.IsOn()) {
                ctx->suppressBassToggle = true;
                ctx->bassToggle.IsOn(true);
                ctx->suppressBassToggle = false;
            }
            ctx->ApplyBassVisuals(true, lvl);
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                g_earbudsState.bassEnabled = true;
                g_earbudsState.bassLevel = lvl;
            }
            BluetoothManager::Instance().PostAction([lvl]() {
                BluetoothManager::Instance().SetBass(true, lvl);
            });
        });

        Grid::SetColumn(item.btn, i);
        if (i > 0) item.btn.Margin({2, 0, 0, 0});
        bassLevelsGrid.Children().Append(item.btn);
        ctx->bassBtns.push_back(item);
    }
    bassTrack.Child(bassLevelsGrid);
    bassSp.Children().Append(bassTrack);

    std::weak_ptr<FlyoutContext> weakBassCtx = ctx;
    ctx->bassToggle.Toggled([weakBassCtx](auto const&, auto const&) {
        auto ctx = weakBassCtx.lock();
        if (!ctx || ctx->suppressBassToggle) return;
        bool isOn = ctx->bassToggle.IsOn();
        uint8_t currentLvl = 1;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_earbudsState.bassEnabled = isOn;
            currentLvl = g_earbudsState.bassLevel;
        }
        ctx->ApplyBassVisuals(isOn, currentLvl);
        BluetoothManager::Instance().PostAction([isOn, currentLvl]() {
            BluetoothManager::Instance().SetBass(isOn, currentLvl);
        });
    });

    ctx->ApplyBassVisuals(state.bassEnabled, state.bassLevel);
    ctx->bassCard.Child(bassSp);
    mainSp.Children().Append(ctx->bassCard);

    ctx->eqCard = Border();
    ctx->eqCard.Background(SolidColorBrush(Color{0xFF, 0x2A, 0x2A, 0x2E}));
    ctx->eqCard.BorderBrush(SolidColorBrush(Color{0x1A, 0xFF, 0xFF, 0xFF}));
    ctx->eqCard.BorderThickness({1, 1, 1, 1});
    ctx->eqCard.CornerRadius({6, 6, 6, 6});
    ctx->eqCard.Padding({10, 8, 10, 8});

    StackPanel eqSp;
    eqSp.Spacing(6);

    TextBlock eqTitle;
    eqTitle.Text(Loc(StringId::Equalizer));
    eqTitle.FontSize(12);
    eqTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    eqSp.Children().Append(eqTitle);

    const std::vector<std::pair<StringId, uint8_t>> presets = {
        {StringId::Dirac, 0},
        {StringId::Balanced, 1},
        {StringId::MoreTreble, 2},
        {StringId::MoreBass, 3},
        {StringId::Pop, 4},
        {StringId::Voice, 5}
    };

    Grid eqGrid;
    for (int i = 0; i < 3; ++i) {
        ColumnDefinition col;
        col.Width({1.0, GridUnitType::Star});
        eqGrid.ColumnDefinitions().Append(col);
    }
    for (int r = 0; r < 2; ++r) {
        RowDefinition row;
        row.Height({1.0, GridUnitType::Auto});
        eqGrid.RowDefinitions().Append(row);
    }

    for (size_t i = 0; i < presets.size(); ++i) {
        EqBtnInfo item;
        item.preset = presets[i].second;
        item.btn = Button();
        item.btn.HorizontalAlignment(HorizontalAlignment::Stretch);
        item.btn.Padding({2, 5, 2, 5});
        item.btn.CornerRadius({4, 4, 4, 4});

        item.txt = TextBlock();
        item.txt.Text(Loc(presets[i].first));
        item.txt.FontSize(10.5);
        item.txt.HorizontalAlignment(HorizontalAlignment::Center);
        item.btn.Content(item.txt);

        uint8_t presetId = item.preset;
        std::weak_ptr<FlyoutContext> weakCtx = ctx;
        item.btn.Click([weakCtx, presetId](auto const&, auto const&) {
            auto ctx = weakCtx.lock();
            if (!ctx) return;
            ctx->ApplyEqVisuals(presetId);
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                g_earbudsState.eqPreset = presetId;
            }
            BluetoothManager::Instance().PostAction([presetId]() {
                BluetoothManager::Instance().SetEq(presetId);
            });
        });

        int row = (int)(i / 3);
        int col = (int)(i % 3);
        Grid::SetRow(item.btn, row);
        Grid::SetColumn(item.btn, col);
        item.btn.Margin({col > 0 ? 2.0 : 0.0, row > 0 ? 3.0 : 0.0, 0.0, 0.0});
        eqGrid.Children().Append(item.btn);
        ctx->eqBtns.push_back(item);
    }
    eqSp.Children().Append(eqGrid);

    ctx->ApplyEqVisuals(state.eqPreset);
    ctx->eqCard.Child(eqSp);
    mainSp.Children().Append(ctx->eqCard);

    ctx->toolsGrid = Grid();
    ColumnDefinition colLatency, colRingL, colRingR;
    colLatency.Width({1.0, GridUnitType::Star});
    colRingL.Width({1.0, GridUnitType::Auto});
    colRingR.Width({1.0, GridUnitType::Auto});
    ctx->toolsGrid.ColumnDefinitions().Append(colLatency);
    ctx->toolsGrid.ColumnDefinitions().Append(colRingL);
    ctx->toolsGrid.ColumnDefinitions().Append(colRingR);

    ctx->lowLatencyBtn = Button();
    ctx->lowLatencyBtn.HorizontalAlignment(HorizontalAlignment::Stretch);
    ctx->lowLatencyBtn.Padding({4, 5, 4, 5});
    ctx->lowLatencyBtn.CornerRadius({4, 4, 4, 4});

    StackPanel llSp;
    llSp.Orientation(Orientation::Horizontal);
    llSp.Spacing(5);
    llSp.HorizontalAlignment(HorizontalAlignment::Center);
    llSp.VerticalAlignment(VerticalAlignment::Center);

    ctx->lowLatencyIcon = TextBlock();
    ctx->lowLatencyIcon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    ctx->lowLatencyIcon.Text(L"\uE7FC");
    ctx->lowLatencyIcon.FontSize(12);
    ctx->lowLatencyIcon.VerticalAlignment(VerticalAlignment::Center);
    llSp.Children().Append(ctx->lowLatencyIcon);

    ctx->lowLatencyTxt = TextBlock();
    ctx->lowLatencyTxt.FontSize(11);
    ctx->lowLatencyTxt.VerticalAlignment(VerticalAlignment::Center);
    llSp.Children().Append(ctx->lowLatencyTxt);

    ctx->lowLatencyBtn.Content(llSp);
    ctx->ApplyLowLatencyVisuals(state.lowLatencyEnabled);

    std::weak_ptr<FlyoutContext> weakLatencyCtx = ctx;
    ctx->lowLatencyBtn.Click([weakLatencyCtx](auto const&, auto const&) {
        auto ctx = weakLatencyCtx.lock();
        if (!ctx) return;
        bool next = false;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_earbudsState.lowLatencyEnabled = !g_earbudsState.lowLatencyEnabled;
            next = g_earbudsState.lowLatencyEnabled;
        }
        ctx->ApplyLowLatencyVisuals(next);
        BluetoothManager::Instance().PostAction([next]() {
            BluetoothManager::Instance().SetLowLatency(next);
        });
    });
    Grid::SetColumn(ctx->lowLatencyBtn, 0);
    ctx->toolsGrid.Children().Append(ctx->lowLatencyBtn);

    auto setupRingButton = [weakCtx = std::weak_ptr<FlyoutContext>(ctx)](
                               Button ringBtn, TextBlock ringIcon,
                               TextBlock ringLabel, bool isLeft) {
        ringBtn.Margin({4, 0, 0, 0});
        ringBtn.Padding({8, 5, 8, 5});
        ringBtn.CornerRadius({4, 4, 4, 4});
        ringBtn.Background(SolidColorBrush(Color{0x0D, 0xFF, 0xFF, 0xFF}));
        ringBtn.BorderThickness({0, 0, 0, 0});

        StackPanel contentSp;
        contentSp.Orientation(Orientation::Horizontal);
        contentSp.Spacing(5);
        contentSp.HorizontalAlignment(HorizontalAlignment::Center);
        contentSp.VerticalAlignment(VerticalAlignment::Center);

        ringIcon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
        ringIcon.Text(L"\uEA8F");
        ringIcon.FontSize(11.5);
        ringIcon.Foreground(SolidColorBrush(Color{0xCC, 0xFF, 0xFF, 0xFF}));
        ringIcon.VerticalAlignment(VerticalAlignment::Center);
        contentSp.Children().Append(ringIcon);

        ringLabel.Text(Loc(isLeft ? StringId::RingLeft : StringId::RingRight));
        ringLabel.FontSize(11);
        ringLabel.Foreground(SolidColorBrush(Color{0xCC, 0xFF, 0xFF, 0xFF}));
        ringLabel.VerticalAlignment(VerticalAlignment::Center);
        contentSp.Children().Append(ringLabel);

        ringBtn.Content(contentSp);

        ringBtn.Click([weakCtx, isLeft](auto const& sender, auto const&) {
            auto ctx = weakCtx.lock();
            auto ringBtn = sender.template try_as<Button>();
            if (!ctx || !ringBtn) return;
            auto ringIcon = isLeft ? ctx->ringLIcon : ctx->ringRIcon;
            auto ringLabel = isLeft ? ctx->ringLTxt : ctx->ringRTxt;
            if (!ringIcon || !ringLabel) return;
            bool& isRinging = isLeft ? ctx->isRingingL : ctx->isRingingR;
            auto& timer = isLeft ? ctx->ringTimerL : ctx->ringTimerR;
            auto& timerToken = isLeft ? ctx->ringTimerLToken : ctx->ringTimerRToken;

            if (isRinging) {
                isRinging = false;
                if (timer) {
                    try { timer.Stop(); } catch (...) {}
                    if (timerToken.value) {
                        try { timer.Tick(timerToken); } catch (...) {}
                    }
                    timer = nullptr;
                }
                timerToken = {};
                BluetoothManager::Instance().PostAction([isLeft]() {
                    BluetoothManager::Instance().FindBuds(isLeft, false);
                });

                ringBtn.Background(SolidColorBrush(Color{0x0D, 0xFF, 0xFF, 0xFF}));
                ringIcon.Text(L"\uEA8F");
                ringIcon.Foreground(SolidColorBrush(Color{0xCC, 0xFF, 0xFF, 0xFF}));
                ringLabel.Text(Loc(isLeft ? StringId::RingLeft : StringId::RingRight));
                ringLabel.Foreground(SolidColorBrush(Color{0xCC, 0xFF, 0xFF, 0xFF}));
            } else {
                isRinging = true;
                ringBtn.Background(SolidColorBrush(Color{0xFF, 0xD7, 0x19, 0x20}));
                ringIcon.Text(L"\uE71A");
                ringIcon.Foreground(SolidColorBrush(Colors::White()));
                ringLabel.Text(Loc(isLeft ? StringId::StopLeft : StringId::StopRight));
                ringLabel.Foreground(SolidColorBrush(Colors::White()));

                BluetoothManager::Instance().PostAction([isLeft]() {
                    BluetoothManager::Instance().FindBuds(isLeft, true);
                });

                timer = DispatcherTimer();
                timer.Interval(std::chrono::milliseconds(5000));
                std::weak_ptr<FlyoutContext> timerWeakCtx = ctx;
                timerToken = timer.Tick([timerWeakCtx, isLeft](auto const&, auto const&) {
                    auto c = timerWeakCtx.lock();
                    if (!c) return;
                    auto ringBtn = isLeft ? c->ringLBtn : c->ringRBtn;
                    auto ringIcon = isLeft ? c->ringLIcon : c->ringRIcon;
                    auto ringLabel = isLeft ? c->ringLTxt : c->ringRTxt;
                    if (!ringBtn || !ringIcon || !ringLabel) return;
                    bool& ringing = isLeft ? c->isRingingL : c->isRingingR;
                    auto& tmr = isLeft ? c->ringTimerL : c->ringTimerR;
                    auto& token = isLeft ? c->ringTimerLToken : c->ringTimerRToken;
                    ringing = false;
                    ringBtn.Background(SolidColorBrush(Color{0x0D, 0xFF, 0xFF, 0xFF}));
                    ringIcon.Text(L"\uEA8F");
                    ringIcon.Foreground(SolidColorBrush(Color{0xCC, 0xFF, 0xFF, 0xFF}));
                    ringLabel.Text(Loc(isLeft ? StringId::RingLeft : StringId::RingRight));
                    ringLabel.Foreground(SolidColorBrush(Color{0xCC, 0xFF, 0xFF, 0xFF}));
                    if (tmr) {
                        try { tmr.Stop(); } catch (...) {}
                        if (token.value) {
                            try { tmr.Tick(token); } catch (...) {}
                        }
                        tmr = nullptr;
                    }
                    token = {};
                    BluetoothManager::Instance().PostAction([isLeft]() {
                        BluetoothManager::Instance().FindBuds(isLeft, false);
                    });
                });
                timer.Start();
            }
        });
    };

    ctx->ringLBtn = Button();
    ctx->ringLIcon = TextBlock();
    ctx->ringLTxt = TextBlock();
    setupRingButton(ctx->ringLBtn, ctx->ringLIcon, ctx->ringLTxt, true);
    Grid::SetColumn(ctx->ringLBtn, 1);
    ctx->toolsGrid.Children().Append(ctx->ringLBtn);

    ctx->ringRBtn = Button();
    ctx->ringRIcon = TextBlock();
    ctx->ringRTxt = TextBlock();
    setupRingButton(ctx->ringRBtn, ctx->ringRIcon, ctx->ringRTxt, false);
    Grid::SetColumn(ctx->ringRBtn, 2);
    ctx->toolsGrid.Children().Append(ctx->ringRBtn);

    mainSp.Children().Append(ctx->toolsGrid);
    rootBorder.Child(mainSp);
    sv.Content(rootBorder);

    ctx->UpdateConnectionState(
        state.connected, state.left.present, state.right.present);

    std::weak_ptr<FlyoutContext> weakCtx = ctx;
    s_refreshFlyoutUi = [weakCtx]() {
        auto c = weakCtx.lock();
        if (!c) return;

        EarbudsState curState;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            curState = g_earbudsState;
        }
        if (c->title) c->title.Text(curState.deviceName.empty() ? L"CMF Buds 2" : curState.deviceName);
        if (c->statusText) {
            if (curState.connected) {
                c->statusText.Text(L"● " + Loc(StringId::Connected));
                c->statusText.Foreground(SolidColorBrush(Color{0xFF, 0x4C, 0xAF, 0x50}));
            } else if (curState.connecting) {
                c->statusText.Text(L"● " + Loc(StringId::Connecting));
                c->statusText.Foreground(SolidColorBrush(Color{0xFF, 0xFF, 0x98, 0x00}));
            } else {
                c->statusText.Text(L"● " + Loc(StringId::Disconnected));
                c->statusText.Foreground(SolidColorBrush(Color{0x80, 0xFF, 0xFF, 0xFF}));
            }
        }

        UpdateBatteryCard(c->leftCard, curState.left, true);
        UpdateBatteryCard(c->caseCard, curState.caseBattery, false);
        UpdateBatteryCard(c->rightCard, curState.right, true);

        bool sBud = (curState.left.present ^ curState.right.present);
        if (c->warnBadge) {
            c->warnBadge.Visibility(sBud ? Visibility::Visible : Visibility::Collapsed);
        }

        c->UpdateConnectionState(
            curState.connected, curState.left.present, curState.right.present);

        if (curState.connected) {
            c->ApplyAncVisuals(curState.ancMode);
            c->ApplyBassVisuals(curState.bassEnabled, curState.bassLevel);
            if (c->bassToggle) c->bassToggle.IsOn(curState.bassEnabled);
            c->ApplyEqVisuals(curState.eqPreset);
            c->ApplyLowLatencyVisuals(curState.lowLatencyEnabled);
        }
    };

    return sv;
}


// Widget Grid for Taskbar
static Grid BuildWidgetGrid() {
    Grid widgetGrid;
    widgetGrid.Name(kWidgetGridName);
    widgetGrid.VerticalAlignment(VerticalAlignment::Center);
    widgetGrid.HorizontalAlignment(HorizontalAlignment::Left);

    Button btn;
    btn.Name(L"NothingTrackButton");
    btn.Padding({5, 2, 5, 2});
    btn.CornerRadius({6, 6, 6, 6});
    btn.VerticalAlignment(VerticalAlignment::Center);
    btn.BorderThickness({0, 0, 0, 0});
    btn.Background(SolidColorBrush(Colors::Transparent()));

    StackPanel sp;
    sp.Name(L"NothingTrackStackPanel");
    sp.Orientation(Orientation::Horizontal);
    sp.Spacing(5);
    sp.VerticalAlignment(VerticalAlignment::Center);

    // Device Icon (Headphones \uE7F6)
    TextBlock iconText;
    iconText.Name(L"NothingTrackIcon");
    iconText.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    iconText.Text(L"\uE7F6");
    iconText.FontSize(12.5);
    iconText.VerticalAlignment(VerticalAlignment::Center);
    sp.Children().Append(iconText);

    // Left Earbud Battery: [Fluent Battery Glyph] [L 75%]
    StackPanel leftSp;
    leftSp.Name(L"NothingTrackLeftGroup");
    leftSp.Orientation(Orientation::Horizontal);
    leftSp.Spacing(2.5);
    leftSp.VerticalAlignment(VerticalAlignment::Center);

    TextBlock leftIcon;
    leftIcon.Name(L"NothingTrackLeftIcon");
    leftIcon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    leftIcon.Text(L"\uE83F");
    leftIcon.FontSize(13.5);
    leftIcon.VerticalAlignment(VerticalAlignment::Center);
    leftSp.Children().Append(leftIcon);

    TextBlock leftText;
    leftText.Name(L"NothingTrackLeftText");
    leftText.FontSize(11.5);
    leftText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    leftText.VerticalAlignment(VerticalAlignment::Center);
    leftSp.Children().Append(leftText);
    sp.Children().Append(leftSp);

    // Right Earbud Battery: [Fluent Battery Glyph] [R 85%]
    StackPanel rightSp;
    rightSp.Name(L"NothingTrackRightGroup");
    rightSp.Orientation(Orientation::Horizontal);
    rightSp.Spacing(2.5);
    rightSp.VerticalAlignment(VerticalAlignment::Center);

    TextBlock rightIcon;
    rightIcon.Name(L"NothingTrackRightIcon");
    rightIcon.FontFamily(FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    rightIcon.Text(L"\uE83F");
    rightIcon.FontSize(13.5);
    rightIcon.VerticalAlignment(VerticalAlignment::Center);
    rightSp.Children().Append(rightIcon);

    TextBlock rightText;
    rightText.Name(L"NothingTrackRightText");
    rightText.FontSize(11.5);
    rightText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    rightText.VerticalAlignment(VerticalAlignment::Center);
    rightSp.Children().Append(rightText);
    sp.Children().Append(rightSp);

    // Single Status text for "Connecting..." or "Disconnected"
    TextBlock statusText;
    statusText.Name(L"NothingTrackStatusText");
    statusText.FontSize(11.5);
    statusText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    statusText.VerticalAlignment(VerticalAlignment::Center);
    statusText.Visibility(Visibility::Collapsed);
    sp.Children().Append(statusText);

    btn.Content(sp);

    // Setup Flyout on click (opens unconstrained above the taskbar)
    Flyout flyout;
    s_currentFlyout = flyout;
    try {
        flyout.Placement(winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutPlacementMode::Top);
        flyout.ShouldConstrainToRootBounds(false);
    } catch (...) {}

    g_flyoutOpenedToken = flyout.Opened([](auto const&, auto const&) {
        s_flyoutOpen = true;
    });

    g_flyoutClosedToken = flyout.Closed([](auto const&, auto const&) {
        s_flyoutOpen = false;
        s_refreshFlyoutUi = nullptr;
        auto c = s_currentFlyoutCtx;
        if (c) {
            c->StopRingTimers();
            if (c->isRingingL || c->isRingingR) {
                c->isRingingL = false;
                c->isRingingR = false;
                BluetoothManager::Instance().PostAction([]() {
                    BluetoothManager::Instance().FindBuds(true, false);
                    BluetoothManager::Instance().FindBuds(false, false);
                });
            }
        }
        if (s_currentFlyout) {
            try { s_currentFlyout.Content(nullptr); } catch (...) {}
        }
        s_currentFlyoutCtx.reset();
    });

    btn.Click([](auto const& sender, auto const&) {
        auto btn = sender.template try_as<Button>();
        auto flyout = s_currentFlyout;
        if (!btn || !flyout) return;
        if (s_flyoutOpen) {
            flyout.Hide();
            return;
        }

        try {
            flyout.Content(BuildFlyoutContent());
        } catch (...) {
            s_currentFlyoutCtx.reset();
            s_refreshFlyoutUi = nullptr;
            return;
        }

        try {
            flyout.ShouldConstrainToRootBounds(false);
            flyout.Placement(winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutPlacementMode::Top);
        } catch (...) {}

        try {
            auto xamlRoot = btn.XamlRoot();
            if (xamlRoot) {
                flyout.XamlRoot(xamlRoot);
                auto rootContent = xamlRoot.Content().template try_as<FrameworkElement>();
                if (rootContent) {
                    auto xform = btn.TransformToVisual(rootContent);
                    auto pt = xform.TransformPoint({0.f, 0.f});
                    float cx = pt.X + (float)btn.ActualWidth() * 0.5f;
                    float ty = pt.Y;

                    winrt::Windows::Foundation::Point anchorPoint{cx, ty};
                    winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutShowOptions options;
                    options.Placement(winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutPlacementMode::Top);
                    options.Position(anchorPoint);

                    flyout.ShowAt(rootContent, options);
                    return;
                }
            }
        } catch (...) {}

        try {
            winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutShowOptions options;
            options.Placement(winrt::Windows::UI::Xaml::Controls::Primitives::FlyoutPlacementMode::Top);
            flyout.ShowAt(btn, options);
            return;
        } catch (...) {}

        try {
            flyout.ShowAt(btn);
        } catch (...) {
            try { flyout.Content(nullptr); } catch (...) {}
            s_currentFlyoutCtx.reset();
            s_refreshFlyoutUi = nullptr;
        }
    });

    widgetGrid.Children().Append(btn);
    return widgetGrid;
}

static const wchar_t* const kStartButtonNames[] = {
    L"StartButton",
    L"StartMenuButton",
    L"StartMenuLaunchButton", 
    L"LaunchListButton",
};

static Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    FrameworkElement taskbarFrame = nullptr;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto c = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (c) {
            auto className = winrt::get_class_name(c);
            if (className == L"Taskbar.TaskbarFrame") {
                taskbarFrame = c;
                break;
            }
        }
    }
    if (!taskbarFrame) {
        return nullptr;
    }
    auto rootGrid = FindChildByName(taskbarFrame, L"RootGrid");
    return rootGrid ? rootGrid.try_as<Grid>() : nullptr;
}

static FrameworkElement FindElementInRepeater(FrameworkElement const& repeater, const wchar_t* const* names, int nameCount) {
    if (!repeater) return nullptr;
    int childCount = VisualTreeHelper::GetChildrenCount(repeater);
    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
        if (!child) continue;
        for (int j = 0; j < nameCount; j++) {
            if (child.Name() == names[j]) return child;
        }
    }
    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
        if (!child) continue;
        int subChildCount = VisualTreeHelper::GetChildrenCount(child);
        for (int k = 0; k < subChildCount; k++) {
            auto subChild = VisualTreeHelper::GetChild(child, k).try_as<FrameworkElement>();
            if (!subChild) continue;
            for (int j = 0; j < nameCount; j++) {
                if (subChild.Name() == names[j]) return subChild;
            }
        }
    }
    return nullptr;
}

static FrameworkElement FindElementByClassName(FrameworkElement const& parent, const wchar_t* className) {
    if (!parent) return nullptr;
    int childCount = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) continue;
        auto childClassName = winrt::get_class_name(child);
        if (childClassName == className) return child;
    }
    return nullptr;
}

static FrameworkElement FindNthElementByClassName(FrameworkElement const& parent, const wchar_t* className, int index) {
    if (!parent) return nullptr;
    int foundCount = 0;
    int childCount = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) continue;
        auto childClassName = winrt::get_class_name(child);
        if (childClassName == className) {
            if (foundCount == index) return child;
            foundCount++;
        }
    }
    return nullptr;
}

static FrameworkElement FindChildByClassName(FrameworkElement const& parent, const wchar_t* className, int depth = 32) {
    if (!parent || depth <= 0) return nullptr;
    int childCount = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < childCount; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (winrt::get_class_name(child) == className) return child;
        if (auto found = FindChildByClassName(child, className, depth - 1)) return found;
    }
    return nullptr;
}

static FrameworkElement FindTrayElement(FrameworkElement const& trayGrid, FrameworkElement const& root, const wchar_t* name) {
    auto elem = FindChildByName(trayGrid, name);
    if (!elem) elem = FindChildByName(root, name);
    return elem;
}

struct InjectionTarget {
    Grid grid;
    int  insertCol = 0;
};

static InjectionTarget ResolveInjectionTarget(
    FrameworkElement const& root,
    std::wstring_view position)
{
    auto trayFrame = FindChildByName(root, L"SystemTrayFrameGrid");
    if (auto trayGrid = trayFrame ? trayFrame.try_as<Grid>() : nullptr) {
        int col = -1;
        if      (position == L"tray_right")
            col = (int)trayGrid.ColumnDefinitions().Size();
        else if (position == L"tray_left")
            col = 0;
        else if (position == L"tray_before_clock") {
            auto clockBtn = FindChildByName(trayGrid, L"NotificationCenterButton");
            if (!clockBtn) clockBtn = FindChildByName(root, L"NotificationCenterButton");
            col = clockBtn ? Grid::GetColumn(clockBtn) : -1;
        }
        else if (position == L"tray_after_clock") {
            auto showDesktop = FindChildByName(trayGrid, L"ShowDesktopStack");
            if (!showDesktop) showDesktop = FindChildByName(root, L"ShowDesktopStack");
            col = showDesktop ? Grid::GetColumn(showDesktop) : -1;
        }
        else if (position == L"tray_before_omni_left") {
            auto omniBtn = FindChildByName(trayGrid, L"ControlCenterButton");
            if (!omniBtn) omniBtn = FindChildByName(root, L"ControlCenterButton");
            col = omniBtn ? Grid::GetColumn(omniBtn) : -1;
        }
        else if (position == L"tray_before_omni_right") {
            auto omniBtn = FindChildByName(trayGrid, L"ControlCenterButton");
            if (!omniBtn) omniBtn = FindChildByName(root, L"ControlCenterButton");
            if (omniBtn) col = Grid::GetColumn(omniBtn) + 1;
            else col = -1;
        }
        else if (position == L"tray_language_left") {
            auto languageBtn = FindTrayElement(trayGrid, root, L"NonActivatableStack");
            col = languageBtn ? Grid::GetColumn(languageBtn) : -1;
        }
        else if (position == L"tray_language_right") {
            auto languageBtn = FindTrayElement(trayGrid, root, L"NonActivatableStack");
            col = languageBtn ? Grid::GetColumn(languageBtn) + 1 : -1;
        }
        else if (position == L"tray_hidden_icons_left") {
            auto hiddenIconsBtn = FindTrayElement(trayGrid, root, L"NotifyIconStack");
            col = hiddenIconsBtn ? Grid::GetColumn(hiddenIconsBtn) : -1;
        }
        else if (position == L"tray_hidden_icons_right") {
            auto hiddenIconsBtn = FindTrayElement(trayGrid, root, L"NotifyIconStack");
            col = hiddenIconsBtn ? Grid::GetColumn(hiddenIconsBtn) + 1 : -1;
        }
        else if (position == L"tray_icons_left") {
            auto trayIcons = FindTrayElement(trayGrid, root, L"NotificationAreaIcons");
            col = trayIcons ? Grid::GetColumn(trayIcons) : -1;
        }
        else if (position == L"tray_icons_right") {
            auto trayIcons = FindTrayElement(trayGrid, root, L"NotificationAreaIcons");
            col = trayIcons ? Grid::GetColumn(trayIcons) + 1 : -1;
        }
        else if (position == L"tray_after_showdesktop_left") {
            auto showDesktop = FindChildByName(trayGrid, L"ShowDesktopStack");
            if (!showDesktop) showDesktop = FindChildByName(root, L"ShowDesktopStack");
            col = showDesktop ? Grid::GetColumn(showDesktop) : -1;
        }
        else if (position == L"tray_after_showdesktop_right") {
            auto showDesktop = FindChildByName(trayGrid, L"ShowDesktopStack");
            if (!showDesktop) showDesktop = FindChildByName(root, L"ShowDesktopStack");
            if (showDesktop) col = Grid::GetColumn(showDesktop) + 1;
            else col = (int)trayGrid.ColumnDefinitions().Size();
        }
        if (col >= 0) {
            return {trayGrid, col};
        }
    }
    if (position == L"taskbar_left_start"  ||
        position == L"taskbar_right_start" ||
        position == L"taskbar_after_search_left"||
        position == L"taskbar_after_search_right"||
        position == L"taskbar_after_taskview_left"||
        position == L"taskbar_after_taskview_right"||
        position == L"taskbar_after_widgets_left"||
        position == L"taskbar_after_widgets_right"||
        position == L"taskbar_left_edge"   ||
        position == L"taskbar_center_edge" ||
        position == L"taskbar_right_edge")
    {
        auto rootGrid = FindTaskbarRootGrid(root);
        if (!rootGrid) {
            auto tf2 = FindChildByName(root, L"SystemTrayFrameGrid");
            if (auto tg2 = tf2 ? tf2.try_as<Grid>() : nullptr)
                return {tg2, (int)tg2.ColumnDefinitions().Size()};
            return {};
        }
        return {rootGrid, -1};
    }
    return {};
}

static void UpdateWidgetUi() {
    if (!g_injectedGrid) return;

    auto btn = FindChildByName(g_injectedGrid, L"NothingTrackButton").try_as<Button>();
    auto iconText = FindChildByName(g_injectedGrid, L"NothingTrackIcon").try_as<TextBlock>();
    auto leftSp = FindChildByName(g_injectedGrid, L"NothingTrackLeftGroup").try_as<StackPanel>();
    auto leftIcon = FindChildByName(g_injectedGrid, L"NothingTrackLeftIcon").try_as<TextBlock>();
    auto leftText = FindChildByName(g_injectedGrid, L"NothingTrackLeftText").try_as<TextBlock>();
    auto rightSp = FindChildByName(g_injectedGrid, L"NothingTrackRightGroup").try_as<StackPanel>();
    auto rightIcon = FindChildByName(g_injectedGrid, L"NothingTrackRightIcon").try_as<TextBlock>();
    auto rightText = FindChildByName(g_injectedGrid, L"NothingTrackRightText").try_as<TextBlock>();
    auto statusText = FindChildByName(g_injectedGrid, L"NothingTrackStatusText").try_as<TextBlock>();
    if (!btn || !iconText || !leftSp || !leftIcon || !leftText ||
        !rightSp || !rightIcon || !rightText || !statusText) {
        return;
    }

    EarbudsState state;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        state = g_earbudsState;
    }

    std::wstring tooltip;

    if (state.connected) {
        if (iconText) iconText.Opacity(1.0);

        if (g_settings.displayFormat == L"icon_only") {
            statusText.Visibility(Visibility::Collapsed);
            leftText.Visibility(Visibility::Collapsed);
            rightText.Visibility(Visibility::Collapsed);

            if (g_settings.hideDisconnectedBuds) {
                leftSp.Visibility(state.left.present ? Visibility::Visible : Visibility::Collapsed);
                rightSp.Visibility(state.right.present ? Visibility::Visible : Visibility::Collapsed);
            } else {
                leftSp.Visibility(Visibility::Visible);
                rightSp.Visibility(Visibility::Visible);
            }

            if (state.left.present) {
                leftIcon.Text(std::wstring(1, GetBatteryGlyph(state.left.percent, state.left.charging)));
                leftIcon.Opacity(1.0);
                if (state.left.percent <= 20) {
                    leftIcon.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
                } else {
                    leftIcon.ClearValue(TextBlock::ForegroundProperty());
                }
            } else {
                leftIcon.Text(std::wstring(1, GetBatteryGlyph(-1, false)));
                leftIcon.Opacity(0.35);
                leftIcon.ClearValue(TextBlock::ForegroundProperty());
            }
            if (state.right.present) {
                rightIcon.Text(std::wstring(1, GetBatteryGlyph(state.right.percent, state.right.charging)));
                rightIcon.Opacity(1.0);
                if (state.right.percent <= 20) {
                    rightIcon.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
                } else {
                    rightIcon.ClearValue(TextBlock::ForegroundProperty());
                }
            } else {
                rightIcon.Text(std::wstring(1, GetBatteryGlyph(-1, false)));
                rightIcon.Opacity(0.35);
                rightIcon.ClearValue(TextBlock::ForegroundProperty());
            }
        } else if (g_settings.displayFormat == L"single_active") {
            statusText.Visibility(Visibility::Collapsed);
            if (!state.left.present && !state.right.present) {
                leftSp.Visibility(Visibility::Collapsed);
                rightSp.Visibility(Visibility::Collapsed);
                statusText.Visibility(Visibility::Visible);
                statusText.Text(state.caseBattery.present
                    ? Loc(StringId::Case) + L" " + std::to_wstring(state.caseBattery.percent) + L"%"
                    : Loc(StringId::InCase));
                statusText.Opacity(0.7);
            } else {
                bool showRight = state.right.present || !state.left.present;
                leftSp.Visibility(showRight ? Visibility::Collapsed : Visibility::Visible);
                rightSp.Visibility(showRight ? Visibility::Visible : Visibility::Collapsed);
                leftText.Visibility(Visibility::Visible);
                rightText.Visibility(Visibility::Visible);

                if (showRight) {
                    rightIcon.Text(std::wstring(1, GetBatteryGlyph(state.right.present ? state.right.percent : -1, state.right.charging)));
                    rightText.Text(state.right.present ? (std::to_wstring(state.right.percent) + L"%") : L"—");
                    if (state.right.present && state.right.percent <= 20) {
                        rightIcon.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
                    } else {
                        rightIcon.ClearValue(TextBlock::ForegroundProperty());
                    }
                    rightSp.Opacity(state.right.present ? 1.0 : 0.4);
                } else {
                    leftIcon.Text(std::wstring(1, GetBatteryGlyph(state.left.present ? state.left.percent : -1, state.left.charging)));
                    leftText.Text(state.left.present ? (std::to_wstring(state.left.percent) + L"%") : L"—");
                    if (state.left.present && state.left.percent <= 20) {
                        leftIcon.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
                    } else {
                        leftIcon.ClearValue(TextBlock::ForegroundProperty());
                    }
                    leftSp.Opacity(state.left.present ? 1.0 : 0.4);
                }
            }
        } else {
            // Default "compact" (Dual battery L & R) or "detailed"
            bool showLeft = state.left.present;
            bool showRight = state.right.present;

            if (g_settings.hideDisconnectedBuds) {
                if (!showLeft && !showRight) {
                    if (state.caseBattery.present && state.caseBattery.percent >= 0) {
                        leftSp.Visibility(Visibility::Visible);
                        rightSp.Visibility(Visibility::Collapsed);
                        leftText.Visibility(Visibility::Visible);
                        leftIcon.Text(std::wstring(1, GetBatteryGlyph(state.caseBattery.percent, state.caseBattery.charging)));
                        leftText.Text(Loc(StringId::Case) + L" " + std::to_wstring(state.caseBattery.percent) + L"%");
                        if (state.caseBattery.percent <= 20) {
                            leftIcon.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
                        } else {
                            leftIcon.ClearValue(TextBlock::ForegroundProperty());
                        }
                        leftSp.Opacity(1.0);
                    } else {
                        leftSp.Visibility(Visibility::Collapsed);
                        rightSp.Visibility(Visibility::Collapsed);
                        statusText.Visibility(Visibility::Visible);
                        statusText.Text(Loc(StringId::InCase));
                        statusText.Opacity(0.7);
                    }
                } else {
                    leftSp.Visibility(showLeft ? Visibility::Visible : Visibility::Collapsed);
                    rightSp.Visibility(showRight ? Visibility::Visible : Visibility::Collapsed);
                    leftText.Visibility(Visibility::Visible);
                    rightText.Visibility(Visibility::Visible);

                    if (showLeft) {
                        leftIcon.Text(std::wstring(1, GetBatteryGlyph(state.left.percent, state.left.charging)));
                        leftText.Text(L"L " + std::to_wstring(state.left.percent) + L"%");
                        if (state.left.percent <= 20) {
                            leftIcon.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
                        } else {
                            leftIcon.ClearValue(TextBlock::ForegroundProperty());
                        }
                        leftSp.Opacity(1.0);
                    }

                    if (showRight) {
                        rightIcon.Text(std::wstring(1, GetBatteryGlyph(state.right.percent, state.right.charging)));
                        rightText.Text(L"R " + std::to_wstring(state.right.percent) + L"%");
                        if (state.right.percent <= 20) {
                            rightIcon.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
                        } else {
                            rightIcon.ClearValue(TextBlock::ForegroundProperty());
                        }
                        rightSp.Opacity(1.0);
                    }
                }
            } else {
                leftSp.Visibility(Visibility::Visible);
                rightSp.Visibility(Visibility::Visible);
                leftText.Visibility(Visibility::Visible);
                rightText.Visibility(Visibility::Visible);

                // Left earbud
                leftIcon.Text(std::wstring(1, GetBatteryGlyph(state.left.present ? state.left.percent : -1, state.left.charging)));
                leftText.Text(state.left.present ? (L"L " + std::to_wstring(state.left.percent) + L"%") : L"L —");
                if (state.left.present && state.left.percent <= 20) {
                    leftIcon.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
                } else {
                    leftIcon.ClearValue(TextBlock::ForegroundProperty());
                }
                leftSp.Opacity(state.left.present ? 1.0 : 0.35);

                // Right earbud
                rightIcon.Text(std::wstring(1, GetBatteryGlyph(state.right.present ? state.right.percent : -1, state.right.charging)));
                rightText.Text(state.right.present ? (L"R " + std::to_wstring(state.right.percent) + L"%") : L"R —");
                if (state.right.present && state.right.percent <= 20) {
                    rightIcon.Foreground(SolidColorBrush(Color{0xFF, 0xE5, 0x39, 0x35}));
                } else {
                    rightIcon.ClearValue(TextBlock::ForegroundProperty());
                }
                rightSp.Opacity(state.right.present ? 1.0 : 0.35);
            }

            if (g_settings.displayFormat == L"detailed") {
                statusText.Visibility(Visibility::Visible);
                statusText.Text(state.deviceName.empty() ? L"CMF Buds 2" : state.deviceName);
                statusText.Opacity(0.7);
            } else if (!g_settings.hideDisconnectedBuds || showLeft || showRight) {
                statusText.Visibility(Visibility::Collapsed);
            }
        }

        tooltip = (state.deviceName.empty() ? L"CMF Buds 2" : state.deviceName) + L"\n" +
                  Loc(StringId::LeftBud) + L": " + (state.left.present ? (std::to_wstring(state.left.percent) + L"%" + (state.left.charging ? (L" (" + Loc(StringId::Charging) + L")") : L"")) : Loc(StringId::NotPresent)) + L"\n" +
                  Loc(StringId::RightBud) + L": " + (state.right.present ? (std::to_wstring(state.right.percent) + L"%" + (state.right.charging ? (L" (" + Loc(StringId::Charging) + L")") : L"")) : Loc(StringId::NotPresent)) + L"\n" +
                  Loc(StringId::Case) + L": " + (state.caseBattery.present ? (std::to_wstring(state.caseBattery.percent) + L"%" + (state.caseBattery.charging ? (L" (" + Loc(StringId::Charging) + L")") : L"")) : L"—");
    } else if (state.connecting) {
        if (iconText) {
            iconText.Opacity(0.5);
            iconText.ClearValue(TextBlock::ForegroundProperty());
        }
        leftSp.Visibility(Visibility::Collapsed);
        rightSp.Visibility(Visibility::Collapsed);
        statusText.Visibility(Visibility::Collapsed);
        tooltip = (state.deviceName.empty() ? L"CMF Buds 2" : state.deviceName) + L" (" + Loc(StringId::Connecting) + L")";
    } else {
        if (iconText) {
            iconText.Opacity(0.35);
            iconText.ClearValue(TextBlock::ForegroundProperty());
        }
        leftSp.Visibility(Visibility::Collapsed);
        rightSp.Visibility(Visibility::Collapsed);
        statusText.Visibility(Visibility::Collapsed);
        tooltip = (state.deviceName.empty() ? L"CMF Buds 2" : state.deviceName) + L" (" + Loc(StringId::Disconnected) + L")";
    }

    if (!g_lastTooltip || *g_lastTooltip != tooltip) {
        ToolTipService::SetToolTip(btn, winrt::box_value(tooltip));
        g_lastTooltip = tooltip;
    }

    // Live update open flyout if active
    if (s_refreshFlyoutUi && s_flyoutOpen) {
        try {
            s_refreshFlyoutUi();
        } catch (...) {}
    }
}

static void RemoveWidgetGridChildren(Grid const& targetGrid) {
    if (!targetGrid) return;
    for (uint32_t i = targetGrid.Children().Size(); i-- > 0;) {
        auto fe = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
        if (fe && fe.Name() == kWidgetGridName) {
            try { targetGrid.Children().RemoveAt(i); } catch (...) {}
        }
    }
}

static void RemoveWidgetGrid() {
    g_lastTooltip.reset();
    if (g_dispatcherTimer) {
        try { g_dispatcherTimer.Stop(); } catch (...) {}
        if (g_timerToken.value) {
            try { g_dispatcherTimer.Tick(g_timerToken); } catch (...) {}
        }
        g_dispatcherTimer = nullptr;
    }
    g_timerToken = {};

    if (g_retryTimer) {
        try { g_retryTimer.Stop(); } catch (...) {}
        if (g_retryTimerToken.value) {
            try { g_retryTimer.Tick(g_retryTimerToken); } catch (...) {}
        }
        g_retryTimer = nullptr;
        g_retryTimerToken = {};
    }

    auto c = s_currentFlyoutCtx;
    if (c) {
        c->StopRingTimers();
        if (c->isRingingL || c->isRingingR) {
            c->isRingingL = false;
            c->isRingingR = false;
            if (!g_unloading.load()) {
                BluetoothManager::Instance().PostAction([]() {
                    BluetoothManager::Instance().FindBuds(true, false);
                    BluetoothManager::Instance().FindBuds(false, false);
                });
            }
        }
    }
    s_currentFlyoutCtx.reset();
    s_refreshFlyoutUi = nullptr;
    s_flyoutOpen = false;

    if (s_currentFlyout) {
        if (g_flyoutOpenedToken.value) {
            try { s_currentFlyout.Opened(g_flyoutOpenedToken); } catch (...) {}
        }
        if (g_flyoutClosedToken.value) {
            try { s_currentFlyout.Closed(g_flyoutClosedToken); } catch (...) {}
        }
        try { s_currentFlyout.Hide(); } catch (...) {}
        try { s_currentFlyout.Content(nullptr); } catch (...) {}
        s_currentFlyout = nullptr;
    }
    g_flyoutOpenedToken = {};
    g_flyoutClosedToken = {};

    if (!g_injectionParent) {
        g_injectedGrid = nullptr;
        g_uiDispatcher = nullptr;
        return;
    }
    try {
        if (g_layoutUpdateToken.value) {
            auto targetGrid = g_injectionParent.try_as<Grid>();
            if (targetGrid) {
                try { targetGrid.LayoutUpdated(g_layoutUpdateToken); } catch (...) {}
            }
            g_layoutUpdateToken = {};
        }
        
        if (g_trackedElement) {
            try {
                if (g_hasTrackedElementOriginalMargin) {
                    g_trackedElement.Margin(g_trackedElementOriginalMargin);
                } else {
                    auto m = g_trackedElement.Margin();
                    if (g_trackPosition == L"left") m.Left = 0;
                    if (g_trackPosition == L"right") m.Right = 0;
                    g_trackedElement.Margin(m);
                }
            } catch (...) {}
            g_trackedElement = nullptr;
        }
        g_hasTrackedElementOriginalMargin = false;
        g_trackPosition = L"";

        auto targetGrid = g_injectionParent.try_as<Grid>();
        int widgetCol = g_injectedColumn;
        RemoveWidgetGridChildren(targetGrid);
        if (widgetCol >= 0 && targetGrid && widgetCol < (int)targetGrid.ColumnDefinitions().Size()) {
            for (uint32_t i = 0; i < targetGrid.Children().Size(); ++i) {
                auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                if (child) {
                    int childCol = Grid::GetColumn(child);
                    if (childCol > widgetCol)
                        Grid::SetColumn(child, childCol - 1);
                }
            }
            targetGrid.ColumnDefinitions().RemoveAt(static_cast<uint32_t>(widgetCol));
        }
        g_injectedGrid    = nullptr;
        g_injectionParent = nullptr;
        g_injectedColumn  = -1;
        g_uiDispatcher    = nullptr;
    } catch (...) {
        g_injectedGrid    = nullptr;
        g_injectionParent = nullptr;
        g_injectedColumn  = -1;
        g_uiDispatcher    = nullptr;
    }
}

static bool InjectWidget() {
    HWND hWnd = g_taskbarWnd.load();
    if (!hWnd) hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) {
        Wh_Log(L"InjectWidget: No taskbar window found");
        return false;
    }
    g_taskbarWnd.store(hWnd);
    try {
        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) {
            Wh_Log(L"InjectWidget: Failed to get XAML root");
            return false;
        }
        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) {
            Wh_Log(L"InjectWidget: Failed to get root FrameworkElement");
            return false;
        }
        g_uiDispatcher = root.Dispatcher();

        auto [targetGrid, insertCol] = ResolveInjectionTarget(root, g_settings.position);
        if (!targetGrid) return false;
        // Store the owner before registering any callbacks so a partial
        // injection can still be fully unwound during retry or unload.
        g_injectionParent = targetGrid;

        Grid widgetGrid = BuildWidgetGrid();
        if (!widgetGrid) return false;

        bool isTrayGrid = (targetGrid.Name() == L"SystemTrayFrameGrid");
        RemoveWidgetGridChildren(targetGrid);

        if (isTrayGrid) {
            ColumnDefinition newCol;
            newCol.Width({1.0, GridUnitType::Auto});
            if (insertCol >= (int)targetGrid.ColumnDefinitions().Size()) {
                targetGrid.ColumnDefinitions().Append(newCol);
                g_injectedColumn = insertCol;
            } else {
                targetGrid.ColumnDefinitions().InsertAt(
                    static_cast<uint32_t>(insertCol), newCol);
                g_injectedColumn = insertCol;
                for (uint32_t i = 0; i < targetGrid.Children().Size(); ++i) {
                    auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                    if (child) {
                        int childCol = Grid::GetColumn(child);
                        if (childCol >= insertCol)
                            Grid::SetColumn(child, childCol + 1);
                    }
                }
            }
            widgetGrid.Margin({(double)g_settings.marginLeft, 0,
                            (double)g_settings.marginRight, 0});
            Grid::SetColumn(widgetGrid, insertCol);
            targetGrid.Children().Append(widgetGrid);
        }
        else {
            auto repeater  = FindChildByName(targetGrid, L"TaskbarFrameRepeater");
            auto trayFrame = FindChildByName(targetGrid, L"SystemTrayFrameGrid");
            bool isEdgePosition = (g_settings.position == L"taskbar_left_edge" ||
                                g_settings.position == L"taskbar_center_edge" ||
                                g_settings.position == L"taskbar_right_edge");
            bool isTrackingPosition = (g_settings.position == L"taskbar_left_start" ||
                                    g_settings.position == L"taskbar_right_start" ||
                                    g_settings.position == L"taskbar_after_search_left" ||
                                    g_settings.position == L"taskbar_after_search_right" ||
                                    g_settings.position == L"taskbar_after_taskview_left" ||
                                    g_settings.position == L"taskbar_after_taskview_right" ||
                                    g_settings.position == L"taskbar_after_widgets_left" ||
                                    g_settings.position == L"taskbar_after_widgets_right");
            if (isEdgePosition || isTrackingPosition) {
                double leftMargin  = (double)g_settings.marginLeft;
                double rightMargin = (double)g_settings.marginRight;
                widgetGrid.HorizontalAlignment(HorizontalAlignment::Left);
                if (isEdgePosition) {
                    if (g_settings.position == L"taskbar_left_edge") {
                        widgetGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                    else if (g_settings.position == L"taskbar_center_edge") {
                        widgetGrid.HorizontalAlignment(HorizontalAlignment::Center);
                        widgetGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                    else if (g_settings.position == L"taskbar_right_edge") {
                        widgetGrid.HorizontalAlignment(HorizontalAlignment::Right);
                        if (trayFrame) rightMargin += trayFrame.ActualWidth() + 4;
                        widgetGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                } else if (isTrackingPosition) {
                    FrameworkElement targetElem = nullptr;
                    std::wstring trackSide = L"right";
                    if (repeater) {
                        if (g_settings.position == L"taskbar_left_start") {
                            targetElem = FindElementInRepeater(repeater, kStartButtonNames, ARRAYSIZE(kStartButtonNames));
                            trackSide = L"left";
                        } else if (g_settings.position == L"taskbar_right_start") {
                            targetElem = FindElementInRepeater(repeater, kStartButtonNames, ARRAYSIZE(kStartButtonNames));
                            trackSide = L"right";
                        } else if (g_settings.position == L"taskbar_after_search_left") {
                            targetElem = FindElementByClassName(repeater, L"Taskbar.TaskbarExtensionElement");
                            trackSide = L"left";
                        } else if (g_settings.position == L"taskbar_after_search_right") {
                            targetElem = FindElementByClassName(repeater, L"Taskbar.TaskbarExtensionElement");
                            trackSide = L"right";
                        } else if (g_settings.position == L"taskbar_after_taskview_left") {
                            targetElem = FindNthElementByClassName(repeater, L"Taskbar.ExperienceToggleButton", 1);
                            trackSide = L"left";
                        } else if (g_settings.position == L"taskbar_after_taskview_right") {
                            targetElem = FindNthElementByClassName(repeater, L"Taskbar.ExperienceToggleButton", 1);
                            trackSide = L"right";
                        } else if (g_settings.position == L"taskbar_after_widgets_left") {
                            targetElem = FindChildByName(repeater, L"AugmentedEntryPointButton");
                            if (!targetElem) targetElem = FindChildByClassName(repeater, L"Taskbar.AugmentedEntryPointButton");
                            trackSide = L"left";
                        } else if (g_settings.position == L"taskbar_after_widgets_right") {
                            targetElem = FindChildByName(repeater, L"AugmentedEntryPointButton");
                            if (!targetElem) targetElem = FindChildByClassName(repeater, L"Taskbar.AugmentedEntryPointButton");
                            trackSide = L"right";
                        }
                    }
                    if (targetElem) {
                        g_trackedElement = targetElem;
                        g_trackedElementOriginalMargin = targetElem.Margin();
                        g_hasTrackedElementOriginalMargin = true;
                        g_trackPosition = trackSide;
                        g_layoutUpdateToken = targetGrid.LayoutUpdated(
                            [](winrt::Windows::Foundation::IInspectable const& sender,
                               winrt::Windows::Foundation::IInspectable const&) {
                                try {
                                    auto layoutGrid = sender.try_as<Grid>();
                                    if (!layoutGrid) return;
                                    if (!g_injectedGrid || !g_trackedElement || g_unloading) return;
                                    bool isVisible = (g_injectedGrid.Visibility() == Visibility::Visible);
                                    double w = isVisible ? g_injectedGrid.ActualWidth() : 0.0;
                                    double desiredGap = isVisible ? (w + g_settings.marginLeft + g_settings.marginRight) : 0.0;
                                    auto m = g_hasTrackedElementOriginalMargin ? g_trackedElementOriginalMargin : g_trackedElement.Margin();
                                    auto currentMargin = g_trackedElement.Margin();
                                    bool changedMargin = false;
                                    if (g_trackPosition == L"left") {
                                        if (std::abs(currentMargin.Left - desiredGap) > 1.0) { m.Left = desiredGap; changedMargin = true; }
                                    } else {
                                        if (std::abs(currentMargin.Right - desiredGap) > 1.0) { m.Right = desiredGap; changedMargin = true; }
                                    }
                                    if (changedMargin) g_trackedElement.Margin(m);
                                    if (isVisible) {
                                        try {
                                            auto transform = g_trackedElement.TransformToVisual(layoutGrid);
                                            auto point = transform.TransformPoint({0, 0});
                                            double leftPos = point.X;
                                            if (g_trackPosition == L"left") {
                                                leftPos = point.X - desiredGap + g_settings.marginLeft;
                                            } else {
                                                leftPos = point.X + g_trackedElement.ActualWidth() + g_settings.marginLeft;
                                            }
                                            auto pm = g_injectedGrid.Margin();
                                            if (std::abs(pm.Left - leftPos) > 1.0) {
                                                g_injectedGrid.Margin({leftPos, 0, 0, 0});
                                            }
                                        } catch (...) {}
                                    }
                                } catch (...) {
                                    g_trackedElement = nullptr;
                                    g_hasTrackedElementOriginalMargin = false;
                                }
                            }
                        );
                    } else {
                        widgetGrid.Margin({leftMargin, 0, rightMargin, 0});
                    }
                }
                Grid::SetColumn(widgetGrid, 0);
                Canvas::SetZIndex(widgetGrid, 1000);
                targetGrid.Children().Append(widgetGrid);
                g_injectedColumn = -1;
            }
            else {
                ColumnDefinition newCol;
                newCol.Width({1.0, GridUnitType::Auto});
                if (insertCol >= (int)targetGrid.ColumnDefinitions().Size()) {
                    targetGrid.ColumnDefinitions().Append(newCol);
                    g_injectedColumn = insertCol;
                } else {
                    targetGrid.ColumnDefinitions().InsertAt(
                        static_cast<uint32_t>(insertCol), newCol);
                    g_injectedColumn = insertCol;
                    for (uint32_t i = 0; i < targetGrid.Children().Size(); ++i) {
                        auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
                        if (child) {
                            int childCol = Grid::GetColumn(child);
                            if (childCol >= insertCol)
                                Grid::SetColumn(child, childCol + 1);
                        }
                    }
                }
                widgetGrid.Margin({(double)g_settings.marginLeft, 0,
                                (double)g_settings.marginRight, 0});
                Grid::SetColumn(widgetGrid, insertCol);
                targetGrid.Children().Append(widgetGrid);
            }
        }

        g_injectedGrid = widgetGrid;
        Canvas::SetZIndex(g_injectedGrid, 1000);

        // Setup 1-second UI updater timer
        g_dispatcherTimer = DispatcherTimer();
        g_dispatcherTimer.Interval(std::chrono::seconds(1));
        g_timerToken = g_dispatcherTimer.Tick([](auto const&, auto const&) {
            UpdateWidgetUi();
        });
        g_dispatcherTimer.Start();
        UpdateWidgetUi();
        return true;
    } catch (...) {
        Wh_Log(L"InjectWidget: Exception during widget injection");
        return false;
    }
}

static bool ApplySettings() {
    try { RemoveWidgetGrid(); } catch (...) { Wh_Log(L"ApplySettings: Exception in RemoveWidgetGrid"); }
    if (!g_unloading) {
        try { return InjectWidget(); } catch (...) { Wh_Log(L"ApplySettings: Exception in InjectWidget"); }
    }
    return false;
}

static void ApplySettingsWithRetry(FrameworkElement xamlRootContent, int retryCount = 0) {
    if (g_unloading.load()) return;
    static constexpr int kMaxRetries = 50;
    auto retry = [&]() {
        if (g_unloading.load() || retryCount >= kMaxRetries) {
            if (retryCount >= kMaxRetries) {
                Wh_Log(L"ApplySettingsWithRetry: giving up after %d retries", kMaxRetries);
            }
            return;
        }
        if (g_retryTimer) {
            try { g_retryTimer.Stop(); } catch (...) {}
            if (g_retryTimerToken.value) {
                try { g_retryTimer.Tick(g_retryTimerToken); } catch (...) {}
            }
            g_retryTimer = nullptr;
            g_retryTimerToken = {};
        }
        auto timer = DispatcherTimer();
        timer.Interval(winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(100)});
        g_retryTimerToken = timer.Tick(
            [xamlRootContent, retryCount](
                winrt::Windows::Foundation::IInspectable const& sender,
                winrt::Windows::Foundation::IInspectable const&) {
                auto timer = sender.try_as<DispatcherTimer>();
                if (!timer) return;
                try { timer.Stop(); } catch (...) {}
                if (g_retryTimerToken.value) {
                    try { timer.Tick(g_retryTimerToken); } catch (...) {}
                    g_retryTimerToken = {};
                }
                if (g_retryTimer == timer) {
                    g_retryTimer = nullptr;
                }
                if (!g_unloading.load()) {
                    try {
                        ApplySettingsWithRetry(xamlRootContent, retryCount + 1);
                    } catch (...) {
                        Wh_Log(L"ApplySettingsWithRetry: retry callback failed");
                    }
                }
            });
        g_retryTimer = timer;
        timer.Start();
    };
    if (g_unloading.load()) return;
    auto systemTrayFrame = FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame) {
        retry();
        return;
    }
    auto systemTrayFrameGrid = FindChildByName(systemTrayFrame, L"SystemTrayFrameGrid");
    if (!systemTrayFrameGrid) {
        retry();
        return;
    }
    if (!ApplySettings()) {
        retry();
    }
}

static void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    try {
        if (g_unloading) return;
        HWND hWnd = FindCurrentProcessTaskbarWnd();
        if (!hWnd) {
            Wh_Log(L"TrayUI_StartTaskbar_Hook: Taskbar window not found");
            return;
        }
        RemoveWidgetGrid();
        g_taskbarWnd.store(hWnd);
        BluetoothManager::Instance().Start();

        auto xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) {
            Wh_Log(L"TrayUI_StartTaskbar_Hook: Failed to get XAML root");
            return;
        }
        auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
        if (!xamlRootContent) {
            Wh_Log(L"TrayUI_StartTaskbar_Hook: Failed to get XAML root content");
            return;
        }
        ApplySettingsWithRetry(xamlRootContent);
    } catch (...) {
        Wh_Log(L"TrayUI_StartTaskbar_Hook: setup failed");
    }
}

static bool HookTaskbarDllSymbols() {
    HMODULE h = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!h) return false;

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &Std_Ref_Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original,
         TrayUI_StartTaskbar_Hook},
    };

    return WindhawkUtils::HookSymbols(h, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

static void LoadSettings() {
    g_settings.position =
        WindhawkUtils::StringSetting::make(L"position").get();
    g_settings.displayFormat =
        WindhawkUtils::StringSetting::make(L"displayFormat").get();
    g_settings.language =
        WindhawkUtils::StringSetting::make(L"language").get();
    g_settings.pollInterval = Wh_GetIntSetting(L"pollInterval");
    if (g_settings.pollInterval <= 0) g_settings.pollInterval = 30;
    g_settings.pollInterval = std::clamp(g_settings.pollInterval, 10, 120);
    g_pollIntervalSeconds.store(g_settings.pollInterval);

    auto margins = WindhawkUtils::StringSetting::make(L"marginSide");
    double marginLeft = 4.0;
    double marginRight = 4.0;
    if (swscanf_s(margins.get(), L"%lf %lf", &marginLeft, &marginRight) == 2 &&
        std::isfinite(marginLeft) && std::isfinite(marginRight)) {
        g_settings.marginLeft = std::clamp(marginLeft, 0.0, 100.0);
        g_settings.marginRight = std::clamp(marginRight, 0.0, 100.0);
    } else {
        g_settings.marginLeft = 4.0;
        g_settings.marginRight = 4.0;
    }

    g_settings.hideDisconnectedBuds = Wh_GetIntSetting(L"hideDisconnectedBuds") != 0;
}

// Windhawk mod entry points
BOOL Wh_ModInit() {
    Wh_Log(L"Wh_ModInit NothingTrack");
    g_unloading = false;
    g_taskbarWnd.store(nullptr);

    try {
        LoadSettings();
        if (!HookTaskbarDllSymbols()) {
            Wh_Log(L"HookTaskbarDllSymbols failed");
            return FALSE;
        }
    } catch (...) {
        Wh_Log(L"Wh_ModInit: initialization failed");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"Wh_ModAfterInit NothingTrack");

    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    g_taskbarWnd.store(taskbarWnd);
    if (!taskbarWnd) {
        // This is a secondary explorer.exe process, or the shell taskbar hasn't
        // been created yet. TrayUI_StartTaskbar_Hook will start the manager in
        // the latter case.
        return;
    }

    BluetoothManager::Instance().Start();
    if (!RunFromWindowThread(taskbarWnd, [](void*) {
        try {
            auto xamlRoot = GetTaskbarXamlRoot(g_taskbarWnd.load());
            auto content = xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
            if (content) {
                ApplySettingsWithRetry(content);
            }
        } catch (...) {
            Wh_Log(L"Wh_ModAfterInit: Exception in ApplySettingsWithRetry");
        }
    }, nullptr)) {
        Wh_Log(L"Wh_ModAfterInit: Failed to dispatch taskbar setup");
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Wh_ModUninit NothingTrack");
    g_unloading = true;

    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) hWnd = g_taskbarWnd.load();
    bool removed = false;
    if (hWnd) {
        removed = RunFromWindowThread(hWnd, [](void*) {
            RemoveWidgetGrid();
        }, nullptr);
    }

    // A taskbar window can disappear before uninitialization during Explorer
    // shutdown. Keep a dispatcher captured from the injected XAML tree so that
    // teardown still runs synchronously on its owning thread.
    if (!removed && g_uiDispatcher) {
        try {
            auto dispatcher = g_uiDispatcher;
            if (dispatcher.HasThreadAccess()) {
                RemoveWidgetGrid();
            } else {
                dispatcher.RunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                    []() { RemoveWidgetGrid(); }).get();
            }
            removed = true;
        } catch (...) {
            Wh_Log(L"Wh_ModUninit: Dispatcher teardown failed");
        }
    }

    if (!removed && (g_injectedGrid || g_injectionParent || s_currentFlyout ||
                     g_dispatcherTimer || g_retryTimer)) {
        Wh_Log(L"Wh_ModUninit: UI objects remain after teardown");
    }

    BluetoothManager::Instance().StopRinging();
    BluetoothManager::Instance().Stop();
    BluetoothManager::DestroyInstance();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Wh_ModSettingsChanged NothingTrack");
    if (g_unloading.load()) return;
    HWND hWnd = FindCurrentProcessTaskbarWnd();
    if (!hWnd) hWnd = g_taskbarWnd.load();
    if (hWnd) {
        g_taskbarWnd.store(hWnd);
        if (!RunFromWindowThread(hWnd, [](void*) {
            LoadSettings();
            BluetoothManager::Instance().NotifyPollIntervalChanged();
            Wh_Log(L"NothingTrack: New pos=%s, format=%s, lang=%s",
                   g_settings.position.c_str(), g_settings.displayFormat.c_str(),
                   g_settings.language.c_str());
            ApplySettings();
        }, nullptr)) {
            Wh_Log(L"Wh_ModSettingsChanged: Failed to dispatch settings update");
        }
    } else {
        try {
            LoadSettings();
        } catch (...) {
            Wh_Log(L"Wh_ModSettingsChanged: Failed to load settings");
        }
    }
}

