// ==WindhawkMod==
// @id              desktop-snowfall-enhanced
// @name            Desktop Snowfall & Winter Wonderland
// @name:ru-RU      Снегопад на рабочем столе и Зимняя сказка
// @description     Enhanced Desktop Snowfall with plowing Steam Train / Bulldozer, snowman building, presets, and customization
// @description:ru-RU Улучшенный снегопад с паровозиком и бульдозером для расчистки снега, лепкой снеговиков, пресетами и кастомизацией
// @version         2.0
// @author          hiol09 & mikybuilds
// @github          https://github.com/hiol09
// @homepage        https://github.com/mikybuilds/desktop-snowfall
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ld3d11 -ldcomp -ldwmapi -luser32 -lole32 -loleaut32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Snowfall & Winter Wonderland

An enhanced edition of [Desktop Snowfall](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/desktop-snowfall.wh.cpp) by [mikybuilds](https://github.com/mikybuilds).

Snow falls over your desktop and settles on top of windows and the taskbar, with realistic physics, interactive snowplow vehicles, and snowman building!

![Desktop Snowfall demo](https://raw.githubusercontent.com/mikybuilds/desktop-snowfall/main/demo.gif)

### New Features:
* **Steam Train & Bulldozer Snowplow:** Vehicles drive along windows and the taskbar, plowing snow with flying clumps and chimney smoke. Drag them with the mouse, or let them auto-deploy when snow accumulates.
* **Interactive Snowman Building:** Hold **[Ctrl + E]** over snowdrifts to roll a snowball with your cursor; bring two snowballs together to build a snowman with a carrot nose, branch arms, customizable hats and scarves! Click a snowman with LMB to melt it.
* **Presets:** Quick selection of snow styles (Classic, Blizzard, Fluffy, Magic, Light) or full custom control with individual sliders.
* **Russian Translation:** Full localization for all settings and descriptions.

### Original Features by mikybuilds:
* Snow follows window motion with realistic tosses, hang time, bounce, and squash & stretch.
* Snow slides off windows moved sideways (optional).
* Sweep snow with the mouse cursor.
* Works with default and stylized taskbars (floating dock, auto-hide).
* GPU-accelerated Direct2D / DirectComposition transparent click-through overlay.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- snowMode: custom
  $name: Snow Preset / Custom Mode
  $name:ru-RU: Пресет снега / Пользовательский режим
  $description: Choose 'Custom' to use the sliders below, or select a preset style
  $description:ru-RU: Выберите 'Пользовательский', чтобы использовать ползунки ниже, или выберите готовый пресет
  $options:
    - custom: Custom (Use sliders below)
    - classic: Classic
    - blizzard: Blizzard
    - fluffy: Fluffy Snow
    - magic: Magic
    - light: Light Snow
  $options:ru-RU:
    - custom: Пользовательский (настройки ниже)
    - classic: Классический
    - blizzard: Метель / Снегопад
    - fluffy: Пушистый снег
    - magic: Волшебный
    - light: Легкий снежок
- intensity: 100
  $name: Snowfall Intensity
  $name:ru-RU: Интенсивность снегопада
  $description: Number of snowflakes on the screen (0-600)
  $description:ru-RU: Количество снежинок на экране (0-600)
  #! $min: 0
  #! $max: 600
  #! $format: slider
- speed: 100
  $name: Falling Speed
  $name:ru-RU: Скорость падения
  $description: Snow falling speed (10-500)
  $description:ru-RU: Скорость падения снега (10-500)
  #! $min: 10
  #! $max: 500
  #! $format: slider
- wind: 15
  $name: Wind Strength
  $name:ru-RU: Сила ветра
  $description: Horizontal wind strength (-100 to 100)
  $description:ru-RU: Сила горизонтального ветра (-100 до 100)
  #! $min: -100
  #! $max: 100
  #! $format: slider
- size: 100
  $name: Snowflake Size
  $name:ru-RU: Размер снежинок
  $description: Relative size of snowflakes (20-400)
  $description:ru-RU: Относительный размер снежинок (20-400)
  #! $min: 20
  #! $max: 400
  #! $format: slider
- capHeight: 8
  $name: Max Snow Drift Height
  $name:ru-RU: Максимальная высота сугробов
  $description: Maximum thickness of snow caps on windows and taskbar (0-40)
  $description:ru-RU: Максимальная толщина шапок снега на окнах и панели задач (0-40)
  #! $min: 0
  #! $max: 40
  #! $format: slider
- slideOff: false
  $name: Snow slides off moved windows
  $name:ru-RU: Снег соскальзывает при смещении окна
  $description: When a window moves sideways, snow stays in place and falls off
  $description:ru-RU: При перемещении окна вбок снег соскальзывает с открывшегося края
- sweepWithCursor: true
  $name: Sweep Snow with Cursor
  $name:ru-RU: Сгребание снега курсором
  $description: Moving cursor over snowdrifts sweeps the snow away
  $description:ru-RU: Движение курсора над сугробами сгребает снег
- vehicleType: train
  $name: Vehicle Type
  $name:ru-RU: Тип уборочной техники
  $description: Plow vehicle that drives along windows and taskbar to clean snow
  $description:ru-RU: Снегоуборочная машина, очищающая окна и панель задач
  $options:
    - none: None (Disabled)
    - bulldozer: Bulldozer
    - train: Steam Train
  $options:ru-RU:
    - none: Выключено
    - bulldozer: Бульдозер
    - train: Паровозик
- vehicleSpawnMode: auto
  $name: Vehicle Spawn Mode
  $name:ru-RU: Режим выезда техники
  $description: When does the vehicle appear to plow the snow
  $description:ru-RU: Когда техника выезжает на расчистку
  $options:
    - auto: Auto (Cleans when snow piles up)
    - always: Always Driving
  $options:ru-RU:
    - auto: Автоматически (выезжает при накоплении сугробов)
    - always: Всегда на экране
- vehicleSpeed: 100
  $name: Vehicle Speed
  $name:ru-RU: Скорость техники
  $description: Driving speed in px/s (30-300)
  $description:ru-RU: Скорость движения техники (30-300)
  #! $min: 30
  #! $max: 300
  #! $format: slider
- enableSnowman: false
  $name: Enable Snowman Building
  $name:ru-RU: Включить лепку снеговиков
  $description: Hold [Ctrl + E] over snow to roll a snowball; stack 2 to build a snowman! Click to melt.
  $description:ru-RU: Зажмите [Ctrl + E] над сугробом для лепки снежка; соедините 2 снежка в снеговика! Кликните ЛКМ, чтобы растопить.
- hatStyle: tophat
  $name: Snowman Hat Style
  $name:ru-RU: Головной убор снеговика
  $description: Style of hat worn by snowmen
  $description:ru-RU: Выберите стиль шапки для снеговиков
  $options:
    - tophat: Top Hat
    - santa: Santa Hat
    - beanie: Winter Beanie
    - bucket: Bucket
  $options:ru-RU:
    - tophat: Цилиндр
    - santa: Новогодний колпак Санты
    - beanie: Зимняя шапочка
    - bucket: Ведро
- scarfColor: red
  $name: Snowman Scarf Color
  $name:ru-RU: Цвет шарфика
  $description: Color of the snowman's scarf
  $description:ru-RU: Цвет шарфика снеговика
  $options:
    - red: Red
    - blue: Blue
    - green: Green
    - gold: Gold
    - purple: Purple
  $options:ru-RU:
    - red: Красный
    - blue: Синий
    - green: Зеленый
    - gold: Золотой
    - purple: Фиолетовый
- taskbarSnow: auto
  $name: Taskbar Snow
  $name:ru-RU: Снег на панели задач
  $description: How snow is placed on the Windows taskbar
  $description:ru-RU: Как снег ложится на панель задач Windows
  $options:
    - auto: Auto (detect visible parts)
    - whole: Whole window
    - parts: Visible parts only
    - off: Disabled
  $options:ru-RU:
    - auto: Авто (определять видимые области)
    - whole: Вся панель целиком
    - parts: Только видимые части
    - off: Отключено
- maxFps: 60
  $name: Maximum Framerate
  $name:ru-RU: Ограничение FPS
  $description: Frame rate limit to optimize GPU and CPU performance (15-240)
  $description:ru-RU: Частота кадров анимации снега (15-240)
  #! $min: 15
  #! $max: 240
  #! $format: slider
- allMonitors: false
  $name: Show on all monitors
  $name:ru-RU: Показывать на всех мониторах
- pauseInFullscreen: true
  $name: Pause in fullscreen applications
  $name:ru-RU: Приостанавливать в полноэкранных приложениях
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_3.h>
#include <uiautomation.h>
#include <wrl/client.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

using Microsoft::WRL::ComPtr;

constexpr float kGravity = 2600.0f;      // px/s^2, for snow caps
constexpr float kHangSeconds = 0.12f;    // snow hangs in the air this long
constexpr float kTossFactor = 0.5f;      // share of the window speed snow keeps
constexpr float kMinTossSpeed = 250.0f;  // px/s
constexpr float kMaxTossSpeed = 550.0f;  // px/s
constexpr float kBounceSpeed = 350.0f;   // snow landing faster bounces
constexpr float kChunkMaxSpeed = 700.0f;
constexpr size_t kMaxChunks = 4000;
constexpr size_t kMaxPuffs = 800;
constexpr int kSweepRadius = 7;
constexpr int kChunkColumns = 3;
constexpr float kCapEdgeTaper = 8.0f;
constexpr float kSnowKernel[] = {0.1f, 0.25f, 0.5f, 0.8f, 1.0f,
                                 0.8f, 0.5f,  0.25f, 0.1f};
constexpr float kSnowKernelSum = 4.3f;

enum class TaskbarSnow { Auto, Whole, Parts, Off };
enum class VehicleType { None, Bulldozer, Train };
enum class VehicleSpawnMode { Always, Auto };
enum class HatStyle { TopHat, SantaHat, Beanie, Bucket };
enum class ScarfColor { Red, Blue, Green, Gold, Purple };
enum class SnowMode { Custom, Classic, Blizzard, Fluffy, Magic, Light };

struct {
    std::atomic<SnowMode> snowMode{SnowMode::Custom};
    std::atomic<int> intensity{100};
    std::atomic<bool> slideOff{false};
    std::atomic<int> speed{100};
    std::atomic<int> wind{15};
    std::atomic<int> size{100};
    std::atomic<int> capHeight{8};
    std::atomic<bool> sweepWithCursor{true};
    std::atomic<TaskbarSnow> taskbarSnow{TaskbarSnow::Auto};
    std::atomic<int> maxFps{60};
    std::atomic<bool> allMonitors{false};
    std::atomic<bool> pauseInFullscreen{true};

    // Extended settings
    std::atomic<VehicleType> vehicleType{VehicleType::Train};
    std::atomic<VehicleSpawnMode> vehicleSpawnMode{VehicleSpawnMode::Auto};
    std::atomic<int> vehicleSpeed{100};
    std::atomic<bool> enableSnowman{false};
    std::atomic<HatStyle> hatStyle{HatStyle::TopHat};
    std::atomic<ScarfColor> scarfColor{ScarfColor::Red};
} g_settings;

HANDLE g_stopEvent;
HANDLE g_snowThread;
HANDLE g_taskbarThread;
std::atomic<bool> g_overlayHidden{false};  // hidden while a fullscreen app runs

void LoadSettings() {
    WindhawkUtils::StringSetting snowModeStr =
        WindhawkUtils::StringSetting::make(L"snowMode");
    if (!wcscmp(snowModeStr, L"blizzard")) {
        g_settings.snowMode = SnowMode::Blizzard;
        g_settings.intensity = 280;
        g_settings.speed = 220;
        g_settings.wind = 65;
        g_settings.size = 90;
        g_settings.capHeight = 18;
    } else if (!wcscmp(snowModeStr, L"fluffy")) {
        g_settings.snowMode = SnowMode::Fluffy;
        g_settings.intensity = 80;
        g_settings.speed = 50;
        g_settings.wind = 5;
        g_settings.size = 180;
        g_settings.capHeight = 14;
    } else if (!wcscmp(snowModeStr, L"magic")) {
        g_settings.snowMode = SnowMode::Magic;
        g_settings.intensity = 130;
        g_settings.speed = 80;
        g_settings.wind = -10;
        g_settings.size = 115;
        g_settings.capHeight = 12;
    } else if (!wcscmp(snowModeStr, L"light")) {
        g_settings.snowMode = SnowMode::Light;
        g_settings.intensity = 40;
        g_settings.speed = 70;
        g_settings.wind = 8;
        g_settings.size = 85;
        g_settings.capHeight = 5;
    } else if (!wcscmp(snowModeStr, L"classic")) {
        g_settings.snowMode = SnowMode::Classic;
        g_settings.intensity = 100;
        g_settings.speed = 100;
        g_settings.wind = 15;
        g_settings.size = 100;
        g_settings.capHeight = 8;
    } else {
        g_settings.snowMode = SnowMode::Custom;
        g_settings.intensity = std::clamp(Wh_GetIntSetting(L"intensity"), 0, 600);
        g_settings.speed = std::clamp(Wh_GetIntSetting(L"speed"), 10, 500);
        g_settings.wind = std::clamp(Wh_GetIntSetting(L"wind"), -100, 100);
        g_settings.size = std::clamp(Wh_GetIntSetting(L"size"), 20, 400);
        g_settings.capHeight = std::clamp(Wh_GetIntSetting(L"capHeight"), 0, 40);
    }

    g_settings.slideOff = Wh_GetIntSetting(L"slideOff") != 0;
    g_settings.sweepWithCursor = Wh_GetIntSetting(L"sweepWithCursor") != 0;
    g_settings.maxFps = std::clamp(Wh_GetIntSetting(L"maxFps"), 15, 240);
    g_settings.allMonitors = Wh_GetIntSetting(L"allMonitors") != 0;
    g_settings.pauseInFullscreen = Wh_GetIntSetting(L"pauseInFullscreen") != 0;

    WindhawkUtils::StringSetting vehicleTypeStr =
        WindhawkUtils::StringSetting::make(L"vehicleType");
    if (!wcscmp(vehicleTypeStr, L"none")) {
        g_settings.vehicleType = VehicleType::None;
    } else if (!wcscmp(vehicleTypeStr, L"bulldozer")) {
        g_settings.vehicleType = VehicleType::Bulldozer;
    } else {
        g_settings.vehicleType = VehicleType::Train;
    }

    WindhawkUtils::StringSetting vehicleSpawnModeStr =
        WindhawkUtils::StringSetting::make(L"vehicleSpawnMode");
    if (!wcscmp(vehicleSpawnModeStr, L"always")) {
        g_settings.vehicleSpawnMode = VehicleSpawnMode::Always;
    } else {
        g_settings.vehicleSpawnMode = VehicleSpawnMode::Auto;
    }

    int spd = Wh_GetIntSetting(L"vehicleSpeed");
    g_settings.vehicleSpeed = std::clamp(spd <= 0 ? 100 : spd, 30, 300);

    g_settings.enableSnowman = Wh_GetIntSetting(L"enableSnowman") != 0;

    WindhawkUtils::StringSetting hatStyleStr =
        WindhawkUtils::StringSetting::make(L"hatStyle");
    if (!wcscmp(hatStyleStr, L"santa")) {
        g_settings.hatStyle = HatStyle::SantaHat;
    } else if (!wcscmp(hatStyleStr, L"beanie")) {
        g_settings.hatStyle = HatStyle::Beanie;
    } else if (!wcscmp(hatStyleStr, L"bucket")) {
        g_settings.hatStyle = HatStyle::Bucket;
    } else {
        g_settings.hatStyle = HatStyle::TopHat;
    }

    WindhawkUtils::StringSetting scarfColorStr =
        WindhawkUtils::StringSetting::make(L"scarfColor");
    if (!wcscmp(scarfColorStr, L"blue")) {
        g_settings.scarfColor = ScarfColor::Blue;
    } else if (!wcscmp(scarfColorStr, L"green")) {
        g_settings.scarfColor = ScarfColor::Green;
    } else if (!wcscmp(scarfColorStr, L"gold")) {
        g_settings.scarfColor = ScarfColor::Gold;
    } else if (!wcscmp(scarfColorStr, L"purple")) {
        g_settings.scarfColor = ScarfColor::Purple;
    } else {
        g_settings.scarfColor = ScarfColor::Red;
    }

    WindhawkUtils::StringSetting taskbarSnow =
        WindhawkUtils::StringSetting::make(L"taskbarSnow");
    TaskbarSnow mode = TaskbarSnow::Auto;
    if (!wcscmp(taskbarSnow, L"whole")) {
        mode = TaskbarSnow::Whole;
    } else if (!wcscmp(taskbarSnow, L"parts")) {
        mode = TaskbarSnow::Parts;
    } else if (!wcscmp(taskbarSnow, L"off")) {
        mode = TaskbarSnow::Off;
    }
    g_settings.taskbarSnow = mode;
}

static bool IsTaskbarClass(const wchar_t* cls) {
    return !wcscmp(cls, L"Shell_TrayWnd") ||
           !wcscmp(cls, L"Shell_SecondaryTrayWnd");
}

static bool IsShellWindowClass(const wchar_t* cls) {
    return !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW");
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar parts (Original implementation).

struct TaskbarParts {
    RECT frame;  // relative to the taskbar window
    RECT tray;
    bool hasFrame;
    bool hasTray;
    int windowWidth;
};

// CLSID_CUIAutomation, defined here to avoid linking with uuid.lib.
constexpr GUID kClsidCUIAutomation = {
    0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};

std::mutex g_taskbarPartsMutex;
std::unordered_map<HWND, TaskbarParts> g_taskbarParts;

static bool IsUsableRect(const RECT& rc) {
    return rc.right > rc.left && rc.bottom > rc.top &&
           rc.right - rc.left < 100000 && rc.bottom - rc.top < 100000;
}

static void UnionInto(RECT& dst, bool& has, const RECT& rc) {
    if (!has) {
        dst = rc;
        has = true;
    } else {
        UnionRect(&dst, &dst, &rc);
    }
}

static bool QueryTaskbarParts(IUIAutomation* uia,
                              IUIAutomationCondition* trueCondition,
                              HWND taskbar,
                              TaskbarParts* parts) {
    RECT windowRect;
    if (!GetWindowRect(taskbar, &windowRect)) {
        return false;
    }

    ComPtr<IUIAutomationElement> root;
    if (FAILED(uia->ElementFromHandle(taskbar, &root)) || !root) {
        return false;
    }

    ComPtr<IUIAutomationElementArray> children;
    if (FAILED(root->FindAll(TreeScope_Children, trueCondition, &children)) ||
        !children) {
        return false;
    }

    RECT frame = {}, tray = {};
    bool hasFrame = false, hasTray = false;

    int childCount = 0;
    children->get_Length(&childCount);
    for (int i = 0; i < childCount; i++) {
        ComPtr<IUIAutomationElement> child;
        if (FAILED(children->GetElement(i, &child)) || !child) {
            continue;
        }

        ComPtr<IUIAutomationElementArray> items;
        if (FAILED(child->FindAll(TreeScope_Children, trueCondition, &items)) ||
            !items) {
            continue;
        }

        int itemCount = 0;
        items->get_Length(&itemCount);
        for (int j = 0; j < itemCount; j++) {
            ComPtr<IUIAutomationElement> item;
            if (FAILED(items->GetElement(j, &item)) || !item) {
                continue;
            }

            BSTR className = nullptr;
            RECT rc;
            if (FAILED(item->get_CurrentClassName(&className)) || !className) {
                continue;
            }
            if (SUCCEEDED(item->get_CurrentBoundingRectangle(&rc)) &&
                IsUsableRect(rc) && rc.bottom - rc.top >= 10) {
                if (!wcscmp(className, L"Taskbar.TaskbarFrameAutomationPeer")) {
                    UnionInto(frame, hasFrame, rc);
                } else if (!wcsncmp(className, L"SystemTray.", 11)) {
                    UnionInto(tray, hasTray, rc);
                }
            }
            SysFreeString(className);
        }
    }

    RECT windowRectAfter;
    if (!hasFrame || !GetWindowRect(taskbar, &windowRectAfter) ||
        !EqualRect(&windowRect, &windowRectAfter)) {
        return false;
    }

    OffsetRect(&frame, -windowRect.left, -windowRect.top);
    OffsetRect(&tray, -windowRect.left, -windowRect.top);
    parts->frame = frame;
    parts->tray = tray;
    parts->hasFrame = hasFrame;
    parts->hasTray = hasTray;
    parts->windowWidth = windowRect.right - windowRect.left;
    return true;
}

static BOOL CALLBACK CollectTaskbarsEnumProc(HWND hwnd, LPARAM lParam) {
    wchar_t cls[64];
    if (GetClassName(hwnd, cls, ARRAYSIZE(cls)) && IsTaskbarClass(cls)) {
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hwnd);
    }
    return TRUE;
}

DWORD WINAPI TaskbarThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        return 0;
    }

    {
        ComPtr<IUIAutomation> uia;
        ComPtr<IUIAutomationCondition> trueCondition;
        if (SUCCEEDED(CoCreateInstance(kClsidCUIAutomation, nullptr,
                                       CLSCTX_INPROC_SERVER,
                                       IID_PPV_ARGS(&uia))) &&
            SUCCEEDED(uia->CreateTrueCondition(&trueCondition))) {
            ComPtr<IUIAutomation2> uia2;
            if (SUCCEEDED(uia.As(&uia2))) {
                uia2->put_ConnectionTimeout(1000);
                uia2->put_TransactionTimeout(1000);
            }

            do {
                TaskbarSnow mode = g_settings.taskbarSnow;
                if ((mode != TaskbarSnow::Auto && mode != TaskbarSnow::Parts) ||
                    g_overlayHidden) {
                    continue;
                }

                std::vector<HWND> taskbars;
                EnumWindows(CollectTaskbarsEnumProc, reinterpret_cast<LPARAM>(&taskbars));

                std::unordered_map<HWND, TaskbarParts> previous;
                {
                    std::lock_guard<std::mutex> lock(g_taskbarPartsMutex);
                    previous = g_taskbarParts;
                }

                std::unordered_map<HWND, TaskbarParts> found;
                for (HWND taskbar : taskbars) {
                    TaskbarParts parts;
                    if (QueryTaskbarParts(uia.Get(), trueCondition.Get(),
                                          taskbar, &parts)) {
                        found[taskbar] = parts;
                    } else if (auto it = previous.find(taskbar);
                               it != previous.end()) {
                        found[taskbar] = it->second;
                    }
                }

                std::lock_guard<std::mutex> lock(g_taskbarPartsMutex);
                g_taskbarParts = std::move(found);
            } while (WaitForSingleObject(g_stopEvent, 500) == WAIT_TIMEOUT);
        }
    }

    CoUninitialize();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// The snow and objects.

struct Flake {
    float x, y;  // screen coordinates
    float size;
    float fallSpeed;
    float phase;
    float sway;
    float alpha;
};

struct Chunk {
    float x, y;
    float vx, vy;
    float size;
    float area;
    uint64_t ignoreKey;
};

struct Body {
    uint64_t key;
    HWND hwnd;
    RECT rc;
    bool snowCap;
};

struct Puff {
    float x, y;
    float vx, vy;
    float size;
    float life;
    float maxLife;
};

struct Column {
    float h = 0;
    float lift = 0;
    float vel = 0;
    float hang = 0;
};

struct Cap {
    HWND hwnd = nullptr;
    RECT rc = {};
    std::vector<Column> cols;
    uint32_t seed = 0;
    int offset = 0;
    float vx = 0, vy = 0;
    float carry = 0;
    float squash = 0, squashVel = 0;
    bool airborne = false;
    unsigned seenFrame = 0;
};

struct SmokePuff {
    float x, y;
    float vx, vy;
    float size;
    float life;
    float maxLife;
};

struct VehicleItem {
    float x = 0;
    float y = 0;
    int direction = 1;
    float speed = 100.0f;
    uint64_t bodyKey = 0;
    float width = 46.0f;
    float height = 24.0f;
    float trackAnim = 0;
    float wheelAngle = 0;
    bool isBeingDragged = false;
    float smokeTimer = 0;
    std::vector<SmokePuff> smokePuffs;
};

struct SnowballItem {
    float x;
    float y;
    float radius;
};

struct SnowmanItem {
    float x;
    float y;
    float scale = 1.0f;
    ScarfColor scarf = ScarfColor::Red;
    HatStyle hat = HatStyle::TopHat;
};

static uint64_t BodyKey(HWND hwnd, int part) {
    return (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(hwnd)) << 2) |
           part;
}

static uint32_t Hash32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

static float SmoothNoise(uint32_t seed, float x) {
    float cell = std::floor(x);
    uint32_t i = static_cast<uint32_t>(static_cast<int>(cell));
    float t = x - cell;
    t = t * t * (3.0f - 2.0f * t);
    float a = (Hash32(seed + i * 0x9E3779B1u) & 0xFFFFFF) / 16777216.0f;
    float b = (Hash32(seed + (i + 1) * 0x9E3779B1u) & 0xFFFFFF) / 16777216.0f;
    return a + (b - a) * t;
}

static float CapNoise(const Cap& cap, int column, uint32_t salt, float scale) {
    return SmoothNoise(cap.seed + salt * 0x51ED27u,
                       static_cast<float>(column + cap.offset) / scale);
}

static float SnowLimit(const Cap& cap, int column, float maxHeight) {
    float n = 0.65f * CapNoise(cap, column, 1, 70.0f) +
              0.35f * CapNoise(cap, column, 2, 23.0f);
    n = std::clamp((n - 0.5f) * 1.8f + 0.5f, 0.0f, 1.0f);
    return maxHeight * (0.3f + 0.7f * n);
}

class Overlay {
   public:
    ~Overlay();

    bool Create();
    void Run();

   private:
    bool CreateDeviceResources();
    void ReleaseDeviceResources();
    bool CreateTargetBitmap();
    bool CreateFlakeSprite();
    RECT ComputeArea();
    bool ApplyArea(const RECT& area);
    UINT ComputeSyncInterval();

    float Rand();
    void SpawnFlake(Flake& f, bool scatter);
    void SpawnChunk(float x, float y, float area, float vx, float vy,
                    uint64_t ignoreKey);
    void SpawnPuff(float x, float y, float vx, float vy, float size,
                   float life);
    void DropColumns(Cap& cap, int first, int count);

    static BOOL CALLBACK CollectBodiesEnumProc(HWND hwnd, LPARAM lParam);
    void CollectBodies();
    void AddTaskbarBodies(HWND hwnd, const RECT& rc);
    bool IsFullscreenAppActive();
    bool IsCoveredAbove(size_t bodyIndex, int x, int y);
    const Body* FindLanding(float x, float oldY, float newY,
                            uint64_t ignoreKey);
    void AddSnow(uint64_t key, int column, float amount);

    void UpdateCaps(float dt);
    bool InheritSnow(Cap& cap, unsigned lastSeen);
    void ResizeCap(Cap& cap, const RECT& rc);
    void MoveCap(Cap& cap, const RECT& rc, float dt);
    void Ambient(float dt);
    void Sweep(float dt);
    void UpdateFlakes(float dt);
    void UpdateChunks(float dt);
    void UpdatePuffs(float dt);

    // Vehicle and Snowman updates
    void UpdateVehicle(const POINT& cursor, bool isLButtonDown, float dt);
    void SummonVehicle();
    bool IsCursorOverSnow(const POINT& cursor);
    void UpdateSnowballKeyE(const POINT& cursor, bool isEKeyDown, float dt);
    void CheckClickSnowman(const POINT& cursor, bool isLButtonDown);

    bool Render();
    void DrawCap(const Cap& cap, const RECT& rc);
    void DrawVehicle();
    void DrawBulldozer(const VehicleItem& v);
    void DrawTrain(const VehicleItem& v);
    void DrawSnowmen();
    void DrawSnowballs();

    ID2D1SolidColorBrush* GetScarfBrush(ScarfColor color);

    HWND m_hwnd = nullptr;
    RECT m_area = {};
    int m_width = 0;
    int m_height = 0;
    bool m_visible = true;
    UINT m_syncInterval = 1;

    ComPtr<ID3D11Device> m_d3dDevice;
    ComPtr<ID2D1Factory1> m_d2dFactory;
    ComPtr<ID2D1Device> m_d2dDevice;
    ComPtr<ID2D1DeviceContext> m_ctx;
    ComPtr<IDXGISwapChain1> m_swapChain;
    HANDLE m_frameWait = nullptr;
    ComPtr<IDCompositionDevice> m_dcompDevice;
    ComPtr<IDCompositionTarget> m_dcompTarget;
    ComPtr<IDCompositionVisual> m_dcompVisual;
    ComPtr<ID2D1Bitmap1> m_targetBitmap;
    ComPtr<ID2D1Bitmap1> m_flakeSprite;
    ComPtr<ID2D1LinearGradientBrush> m_capBrush;

    // Brushes
    ComPtr<ID2D1SolidColorBrush> m_whiteBrush;
    ComPtr<ID2D1SolidColorBrush> m_coalBrush;
    ComPtr<ID2D1SolidColorBrush> m_carrotBrush;
    ComPtr<ID2D1SolidColorBrush> m_twigBrush;

    ComPtr<ID2D1SolidColorBrush> m_dozerBodyBrush;
    ComPtr<ID2D1SolidColorBrush> m_dozerGlassBrush;
    ComPtr<ID2D1SolidColorBrush> m_dozerTrackBrush;
    ComPtr<ID2D1SolidColorBrush> m_dozerBladeBrush;

    ComPtr<ID2D1SolidColorBrush> m_trainTenderBrush;
    ComPtr<ID2D1SolidColorBrush> m_trainChimneyBrush;
    ComPtr<ID2D1SolidColorBrush> m_trainTrimBrush;
    ComPtr<ID2D1SolidColorBrush> m_trainLightBrush;

    ComPtr<ID2D1SolidColorBrush> m_scarfRedBrush;
    ComPtr<ID2D1SolidColorBrush> m_scarfBlueBrush;
    ComPtr<ID2D1SolidColorBrush> m_scarfGreenBrush;
    ComPtr<ID2D1SolidColorBrush> m_scarfGoldBrush;
    ComPtr<ID2D1SolidColorBrush> m_scarfPurpleBrush;

    ComPtr<ID2D1SolidColorBrush> m_topHatBrush;
    ComPtr<ID2D1SolidColorBrush> m_santaHatBrush;
    ComPtr<ID2D1SolidColorBrush> m_bucketBrush;

    std::vector<Flake> m_flakes;
    std::vector<Chunk> m_chunks;
    std::vector<Puff> m_puffs;
    std::vector<Body> m_bodies;
    std::unordered_map<uint64_t, Cap> m_caps;

    VehicleItem m_vehicle;
    bool m_vehicleActive = false;
    float m_vehicleCooldown = 0.0f;
    std::vector<SnowballItem> m_snowballs;
    std::vector<SnowmanItem> m_snowmen;
    std::unique_ptr<SnowballItem> m_heldSnowball;
    bool m_prevLButtonDown = false;

    POINT m_lastCursor = {};
    bool m_cursorValid = false;
    uint32_t m_rng = 0x9E3779B9;
    float m_time = 0;
    float m_frameTime = 1.0f / 60;
    unsigned m_frame = 0;
};

constexpr wchar_t kOverlayClass[] = L"WindhawkDesktopSnowfallOverlay";

Overlay::~Overlay() {
    ReleaseDeviceResources();
    if (m_hwnd) {
        RemoveProp(m_hwnd, L"NonRudeHWND");
        DestroyWindow(m_hwnd);
        UnregisterClass(kOverlayClass, GetModuleHandle(nullptr));
    }
}

float Overlay::Rand() {
    m_rng ^= m_rng << 13;
    m_rng ^= m_rng >> 17;
    m_rng ^= m_rng << 5;
    return (m_rng & 0xFFFFFF) / 16777216.0f;
}

RECT Overlay::ComputeArea() {
    RECT rc;
    if (g_settings.allMonitors) {
        rc = {GetSystemMetrics(SM_XVIRTUALSCREEN),
              GetSystemMetrics(SM_YVIRTUALSCREEN),
              GetSystemMetrics(SM_XVIRTUALSCREEN) +
                  GetSystemMetrics(SM_CXVIRTUALSCREEN),
              GetSystemMetrics(SM_YVIRTUALSCREEN) +
                  GetSystemMetrics(SM_CYVIRTUALSCREEN)};
    } else {
        rc = {0, 0, GetSystemMetrics(SM_CXSCREEN),
              GetSystemMetrics(SM_CYSCREEN)};
    }
    return rc;
}

bool Overlay::ApplyArea(const RECT& area) {
    m_area = area;
    m_width = m_area.right - m_area.left;
    m_height = m_area.bottom - m_area.top;
    if (m_width <= 0 || m_height <= 0) {
        return false;
    }

    SetWindowPos(m_hwnd, nullptr, m_area.left, m_area.top, m_width, m_height,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    if (m_swapChain) {
        m_ctx->SetTarget(nullptr);
        m_targetBitmap.Reset();
        if (FAILED(m_swapChain->ResizeBuffers(2, m_width, m_height,
                                              DXGI_FORMAT_B8G8R8A8_UNORM,
                                              DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT))) {
            return false;
        }
        return CreateTargetBitmap();
    }
    return true;
}

UINT Overlay::ComputeSyncInterval() {
    int cap = g_settings.maxFps;
    HMONITOR hMon = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFOEX mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfo(hMon, &mi)) {
        DEVMODE dm{};
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) &&
            dm.dmDisplayFrequency > 0) {
            int rate = static_cast<int>(dm.dmDisplayFrequency);
            int interval = (rate + cap / 2) / cap;
            return static_cast<UINT>(std::clamp(interval, 1, 4));
        }
    }
    return 1;
}

bool Overlay::CreateDeviceResources() {
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   flags, nullptr, 0, D3D11_SDK_VERSION,
                                   &m_d3dDevice, nullptr, nullptr);
    if (FAILED(hr)) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags,
                               nullptr, 0, D3D11_SDK_VERSION, &m_d3dDevice,
                               nullptr, nullptr);
    }
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: %08X", hr);
        return false;
    }

    ComPtr<IDXGIDevice1> dxgiDevice;
    if (FAILED(m_d3dDevice.As(&dxgiDevice))) {
        Wh_Log(L"Querying IDXGIDevice1 failed");
        return false;
    }
    dxgiDevice->SetMaximumFrameLatency(1);

    D2D1_FACTORY_OPTIONS d2dOptions = {};
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 __uuidof(ID2D1Factory1), &d2dOptions,
                                 &m_d2dFactory)) ||
        FAILED(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice)) ||
        FAILED(m_d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_ctx))) {
        Wh_Log(L"Creating Direct2D factory/device failed");
        return false;
    }

    ComPtr<IDXGIAdapter> adapter;
    ComPtr<IDXGIFactory2> dxgiFactory;
    if (FAILED(dxgiDevice->GetAdapter(&adapter)) ||
        FAILED(adapter->GetParent(IID_PPV_ARGS(&dxgiFactory)))) {
        Wh_Log(L"Querying IDXGIFactory2 failed");
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = m_width;
    desc.Height = m_height;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    if (FAILED(dxgiFactory->CreateSwapChainForComposition(
            m_d3dDevice.Get(), &desc, nullptr, &m_swapChain))) {
        Wh_Log(L"CreateSwapChainForComposition failed");
        return false;
    }

    ComPtr<IDXGISwapChain2> swapChain2;
    if (SUCCEEDED(m_swapChain.As(&swapChain2))) {
        m_frameWait = swapChain2->GetFrameLatencyWaitableObject();
    }

    if (FAILED(DCompositionCreateDevice(dxgiDevice.Get(),
                                        IID_PPV_ARGS(&m_dcompDevice))) ||
        FAILED(m_dcompDevice->CreateTargetForHwnd(m_hwnd, TRUE,
                                                 &m_dcompTarget)) ||
        FAILED(m_dcompDevice->CreateVisual(&m_dcompVisual)) ||
        FAILED(m_dcompVisual->SetContent(m_swapChain.Get())) ||
        FAILED(m_dcompTarget->SetRoot(m_dcompVisual.Get())) ||
        FAILED(m_dcompDevice->Commit())) {
        Wh_Log(L"DirectComposition setup failed");
        return false;
    }

    if (!CreateTargetBitmap() || !CreateFlakeSprite()) {
        return false;
    }

    D2D1_GRADIENT_STOP stops[] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.98f)},
        {0.8f, D2D1::ColorF(0.92f, 0.95f, 0.99f, 0.95f)},
        {1.0f, D2D1::ColorF(0.72f, 0.82f, 0.92f, 0.92f)},
    };
    ComPtr<ID2D1GradientStopCollection> stopCollection;
    if (FAILED(m_ctx->CreateGradientStopCollection(
            stops, ARRAYSIZE(stops), D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP,
            &stopCollection)) ||
        FAILED(m_ctx->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0),
                                                D2D1::Point2F(0, 1)),
            stopCollection.Get(), &m_capBrush))) {
        return false;
    }

    m_ctx->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &m_whiteBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.14f, 0.14f, 0.16f, 1.0f), &m_coalBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.47f, 0.12f, 1.0f), &m_carrotBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.43f, 0.29f, 0.18f, 1.0f), &m_twigBrush);

    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.96f, 0.73f, 0.08f, 1.0f), &m_dozerBodyBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.24f, 0.24f, 0.27f, 1.0f), &m_dozerTrackBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.69f, 0.75f, 0.77f, 1.0f), &m_dozerBladeBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.70f, 0.88f, 0.98f, 0.75f), &m_dozerGlassBrush);

    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.16f, 0.16f, 0.19f, 1.0f), &m_trainTenderBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.11f, 0.11f, 0.13f, 1.0f), &m_trainChimneyBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.84f, 0.0f, 1.0f), &m_trainTrimBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.94f, 0.59f, 0.9f), &m_trainLightBrush);

    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.88f, 0.18f, 0.22f, 1.0f), &m_scarfRedBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.12f, 0.53f, 0.90f, 1.0f), &m_scarfBlueBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.26f, 0.63f, 0.28f, 1.0f), &m_scarfGreenBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.99f, 0.85f, 0.21f, 1.0f), &m_scarfGoldBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.56f, 0.14f, 0.67f, 1.0f), &m_scarfPurpleBrush);

    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.10f, 0.10f, 0.12f, 1.0f), &m_topHatBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.88f, 0.14f, 0.18f, 1.0f), &m_santaHatBrush);
    m_ctx->CreateSolidColorBrush(D2D1::ColorF(0.69f, 0.75f, 0.77f, 1.0f), &m_bucketBrush);

    return true;
}

ID2D1SolidColorBrush* Overlay::GetScarfBrush(ScarfColor color) {
    switch (color) {
        case ScarfColor::Blue: return m_scarfBlueBrush.Get();
        case ScarfColor::Green: return m_scarfGreenBrush.Get();
        case ScarfColor::Gold: return m_scarfGoldBrush.Get();
        case ScarfColor::Purple: return m_scarfPurpleBrush.Get();
        default: return m_scarfRedBrush.Get();
    }
}

void Overlay::ReleaseDeviceResources() {
    if (m_ctx) {
        m_ctx->SetTarget(nullptr);
    }
    m_targetBitmap.Reset();
    m_flakeSprite.Reset();
    m_capBrush.Reset();

    m_whiteBrush.Reset();
    m_coalBrush.Reset();
    m_carrotBrush.Reset();
    m_twigBrush.Reset();
    m_dozerBodyBrush.Reset();
    m_dozerGlassBrush.Reset();
    m_dozerTrackBrush.Reset();
    m_dozerBladeBrush.Reset();
    m_trainTenderBrush.Reset();
    m_trainChimneyBrush.Reset();
    m_trainTrimBrush.Reset();
    m_trainLightBrush.Reset();
    m_scarfRedBrush.Reset();
    m_scarfBlueBrush.Reset();
    m_scarfGreenBrush.Reset();
    m_scarfGoldBrush.Reset();
    m_scarfPurpleBrush.Reset();
    m_topHatBrush.Reset();
    m_santaHatBrush.Reset();
    m_bucketBrush.Reset();

    m_dcompVisual.Reset();
    m_dcompTarget.Reset();
    m_dcompDevice.Reset();
    if (m_frameWait) {
        CloseHandle(m_frameWait);
        m_frameWait = nullptr;
    }
    m_swapChain.Reset();
    m_ctx.Reset();
    m_d2dDevice.Reset();
    m_d2dFactory.Reset();
    m_d3dDevice.Reset();
}

bool Overlay::CreateTargetBitmap() {
    ComPtr<IDXGISurface> surface;
    if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface)))) {
        return false;
    }
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));
    if (FAILED(m_ctx->CreateBitmapFromDxgiSurface(surface.Get(), &props,
                                                  &m_targetBitmap))) {
        return false;
    }
    m_ctx->SetTarget(m_targetBitmap.Get());
    return true;
}

bool Overlay::CreateFlakeSprite() {
    constexpr int kRadius = 16;
    constexpr int kDim = 2 * kRadius;
    uint32_t pixels[kDim * kDim];

    for (int y = 0; y < kDim; y++) {
        for (int x = 0; x < kDim; x++) {
            float dx = (x + 0.5f - kRadius) / kRadius;
            float dy = (y + 0.5f - kRadius) / kRadius;
            float d = std::sqrt(dx * dx + dy * dy);
            float a = std::clamp(1.0f - d, 0.0f, 1.0f);
            a = a * a * (3.0f - 2.0f * a);

            float r = (d < 0.6f) ? 1.0f : (0.85f + 0.15f * (1.0f - d) / 0.4f);
            float g = (d < 0.6f) ? 1.0f : (0.90f + 0.10f * (1.0f - d) / 0.4f);
            float b = 1.0f;

            uint8_t a8 = static_cast<uint8_t>(std::round(a * 255.0f));
            uint8_t r8 = static_cast<uint8_t>(std::round(a * r * 255.0f));
            uint8_t g8 = static_cast<uint8_t>(std::round(a * g * 255.0f));
            uint8_t b8 = static_cast<uint8_t>(std::round(a * b * 255.0f));
            pixels[y * kDim + x] =
                (a8 << 24) | (r8 << 16) | (g8 << 8) | b8;
        }
    }

    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_NONE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));
    return SUCCEEDED(m_ctx->CreateBitmap(D2D1::SizeU(kDim, kDim), pixels,
                                         kDim * sizeof(uint32_t), &props,
                                         &m_flakeSprite));
}

bool Overlay::Create() {
    HINSTANCE hInst = GetModuleHandle(nullptr);
    WNDCLASSEX wc = {sizeof(wc)};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = kOverlayClass;
    RegisterClassEx(&wc);

    m_area = ComputeArea();
    m_width = m_area.right - m_area.left;
    m_height = m_area.bottom - m_area.top;

    m_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP,
        kOverlayClass, L"Desktop Snowfall", WS_POPUP, m_area.left, m_area.top,
        m_width, m_height, nullptr, nullptr, wc.hInstance, nullptr);
    if (!m_hwnd) {
        return false;
    }

    SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);
    SetProp(m_hwnd, L"NonRudeHWND", reinterpret_cast<HANDLE>(TRUE));

    if (!CreateDeviceResources()) {
        return false;
    }

    ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
    m_syncInterval = ComputeSyncInterval();
    return true;
}

void Overlay::SpawnFlake(Flake& f, bool scatter) {
    float depth = Rand();
    f.size = (0.8f + depth * depth * 2.7f) * g_settings.size / 100.0f;
    f.fallSpeed = 35.0f + f.size * 22.0f;
    f.phase = Rand() * 6.2832f;
    f.sway = 6.0f + Rand() * 14.0f;
    f.alpha = 0.45f + 0.5f * Rand();
    f.x = m_area.left + Rand() * m_width;
    f.y = scatter ? m_area.top + Rand() * m_height
                  : m_area.top - f.size * 2 - Rand() * 40.0f;
}

void Overlay::SpawnChunk(float x, float y, float area, float vx, float vy,
                        uint64_t ignoreKey) {
    if (m_chunks.size() >= kMaxChunks || area < 0.5f) {
        return;
    }
    m_chunks.push_back({
        .x = x,
        .y = y,
        .vx = vx,
        .vy = vy,
        .size = std::clamp(std::sqrt(area) * 0.55f, 1.0f, 3.5f),
        .area = area,
        .ignoreKey = ignoreKey,
    });
}

void Overlay::SpawnPuff(float x, float y, float vx, float vy, float size,
                       float life) {
    if (m_puffs.size() >= kMaxPuffs) {
        return;
    }
    m_puffs.push_back({
        .x = x,
        .y = y,
        .vx = vx,
        .vy = vy,
        .size = size,
        .life = life,
        .maxLife = life,
    });
}

void Overlay::DropColumns(Cap& cap, int first, int count) {
    for (int i = 0; i < count; i += kChunkColumns) {
        float area = 0, top = 0, lift = 0;
        int end = std::min(i + kChunkColumns, count);
        for (int k = i; k < end; k++) {
            Column& col = cap.cols[first + k];
            area += col.h;
            top = std::max(top, col.h);
            lift = std::max(lift, col.lift);
            col.h = 0;
        }
        SpawnChunk(cap.rc.left + first + i + 1.0f, cap.rc.top - lift - top / 2,
                   area, (Rand() - 0.5f) * 30.0f, 0, 0);
    }
}

void Overlay::AddTaskbarBodies(HWND hwnd, const RECT& rc) {
    TaskbarSnow mode = g_settings.taskbarSnow;
    if (mode == TaskbarSnow::Off) {
        return;
    }

    if (mode != TaskbarSnow::Whole) {
        TaskbarParts parts;
        bool found = false;
        {
            std::lock_guard<std::mutex> lock(g_taskbarPartsMutex);
            auto it = g_taskbarParts.find(hwnd);
            if (it != g_taskbarParts.end()) {
                parts = it->second;
                found = true;
            }
        }

        if (found && parts.windowWidth == rc.right - rc.left) {
            bool useParts =
                mode == TaskbarSnow::Parts ||
                parts.frame.right - parts.frame.left < parts.windowWidth * 0.85;
            if (useParts) {
                RECT frame = parts.frame;
                OffsetRect(&frame, rc.left, rc.top);
                m_bodies.push_back({BodyKey(hwnd, 1), hwnd, frame, true});
                if (parts.hasTray) {
                    RECT tray = parts.tray;
                    OffsetRect(&tray, rc.left, rc.top);
                    m_bodies.push_back({BodyKey(hwnd, 2), hwnd, tray, true});
                }
                return;
            }
        }
    }

    m_bodies.push_back({BodyKey(hwnd, 0), hwnd, rc, true});
}

BOOL CALLBACK Overlay::CollectBodiesEnumProc(HWND hwnd, LPARAM lParam) {
    Overlay* self = reinterpret_cast<Overlay*>(lParam);
    if (hwnd == self->m_hwnd || !IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return TRUE;
    }

    wchar_t cls[64];
    if (!GetClassName(hwnd, cls, ARRAYSIZE(cls)) || IsShellWindowClass(cls)) {
        return TRUE;
    }

    bool isTaskbar = IsTaskbarClass(cls);
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (!isTaskbar) {
        if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
            return TRUE;
        }
        if ((exStyle & WS_EX_LAYERED) && (exStyle & WS_EX_TRANSPARENT)) {
            return TRUE;
        }
    }

    DWORD cloaked = 0;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) {
        return TRUE;
    }

    RECT rc;
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc, sizeof(rc)))) {
        GetWindowRect(hwnd, &rc);
    }
    if (rc.right - rc.left < 80 || rc.bottom - rc.top < 30) {
        return TRUE;
    }

    if (isTaskbar) {
        self->AddTaskbarBodies(hwnd, rc);
    } else {
        bool snowCap = !IsZoomed(hwnd);
        self->m_bodies.push_back({BodyKey(hwnd, 0), hwnd, rc, snowCap});
    }
    return TRUE;
}

void Overlay::CollectBodies() {
    m_bodies.clear();
    EnumWindows(CollectBodiesEnumProc, reinterpret_cast<LPARAM>(this));
}

bool Overlay::IsFullscreenAppActive() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == m_hwnd) {
        return false;
    }

    wchar_t cls[64];
    if (!GetClassName(fg, cls, ARRAYSIZE(cls)) || IsShellWindowClass(cls) ||
        IsTaskbarClass(cls)) {
        return false;
    }

    RECT rc;
    MONITORINFO mi = {sizeof(mi)};
    if (!GetWindowRect(fg, &rc) ||
        !GetMonitorInfo(MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST), &mi)) {
        return false;
    }

    if (rc.left > mi.rcMonitor.left || rc.top > mi.rcMonitor.top ||
        rc.right < mi.rcMonitor.right || rc.bottom < mi.rcMonitor.bottom) {
        return false;
    }

    LONG style = GetWindowLong(fg, GWL_STYLE);
    return !(IsZoomed(fg) && (style & WS_CAPTION) == WS_CAPTION);
}

bool Overlay::IsCoveredAbove(size_t bodyIndex, int x, int y) {
    for (size_t j = 0; j < bodyIndex; j++) {
        const RECT& o = m_bodies[j].rc;
        if (x >= o.left && x < o.right && y >= o.top && y < o.bottom) {
            return true;
        }
    }
    return false;
}

const Body* Overlay::FindLanding(float x, float oldY, float newY,
                                 uint64_t ignoreKey) {
    int sx = static_cast<int>(std::floor(x));
    for (const Body& b : m_bodies) {
        if (sx < b.rc.left || sx >= b.rc.right || newY < b.rc.top ||
            newY >= b.rc.bottom) {
            continue;
        }
        if (b.key != ignoreKey && b.snowCap && oldY < b.rc.top) {
            return &b;
        }
        return nullptr;
    }
    return nullptr;
}

void Overlay::AddSnow(uint64_t key, int column, float amount) {
    auto it = m_caps.find(key);
    if (it == m_caps.end()) {
        return;
    }

    Cap& cap = it->second;
    std::vector<Column>& cols = cap.cols;
    const int width = static_cast<int>(cols.size());
    const float maxHeight = static_cast<float>(g_settings.capHeight);

    constexpr int kReach = 14;
    int from = std::max(column - kReach, 1);
    int to = std::min(column + kReach, width - 2);
    if (from > to) {
        return;
    }

    float limit[2 * kReach + 1];
    for (int c = from; c <= to; c++) {
        limit[c - from] = SnowLimit(cap, c, maxHeight);
    }

    for (int i = 0; i < 9; i++) {
        int c = column + i - 4;
        if (c >= from && c <= to) {
            cols[c].h = std::min(limit[c - from],
                                 cols[c].h + amount * kSnowKernel[i]);
        }
    }

    for (int pass = 0; pass < 2; pass++) {
        for (int c = from; c <= to; c++) {
            float h = cols[c].h * 0.5f + (cols[c - 1].h + cols[c + 1].h) * 0.25f;
            cols[c].h = std::min(h, limit[c - from]);
        }
    }
}

void Overlay::ResizeCap(Cap& cap, const RECT& rc) {
    int oldWidth = cap.rc.right - cap.rc.left;
    int newWidth = rc.right - rc.left;
    std::vector<Column>& cols = cap.cols;

    if (oldWidth == newWidth) {
        int dx = rc.left - cap.rc.left;
        int width = static_cast<int>(cols.size());
        if (dx == 0 || !g_settings.slideOff || width == 0) {
            return;
        }

        cap.offset -= dx;
        if (std::abs(dx) >= width) {
            DropColumns(cap, 0, width);
            return;
        }

        if (dx > 0) {
            DropColumns(cap, width - dx, dx);
            for (int i = width - 1; i >= dx; i--) {
                cols[i] = cols[i - dx];
            }
            for (int i = 0; i < dx; i++) {
                cols[i] = {};
            }
        } else {
            int shift = -dx;
            DropColumns(cap, 0, shift);
            for (int i = 0; i < width - shift; i++) {
                cols[i] = cols[i + shift];
            }
            for (int i = width - shift; i < width; i++) {
                cols[i] = {};
            }
        }
        return;
    }

    int leftDelta = rc.left - cap.rc.left;
    int rightDelta = rc.right - cap.rc.right;

    if (newWidth < oldWidth) {
        if (leftDelta > 0) {
            DropColumns(cap, 0, std::min(leftDelta, oldWidth));
        }
        if (rightDelta < 0) {
            int dropped = -rightDelta;
            int start = std::max(0, oldWidth - dropped);
            DropColumns(cap, start, oldWidth - start);
        }
    }

    cap.offset -= leftDelta;
    std::vector<Column> newCols(newWidth);
    for (int i = 0; i < newWidth; i++) {
        int src = i + leftDelta;
        if (src >= 0 && src < oldWidth) {
            newCols[i] = cols[src];
        }
    }
    cap.cols = std::move(newCols);
}

void Overlay::MoveCap(Cap& cap, const RECT& rc, float dt) {
    const float dTop = static_cast<float>(rc.top - cap.rc.top);
    const float prevVy = cap.vy;
    float vxNow = 0;
    if (rc.right - rc.left == cap.rc.right - cap.rc.left) {
        vxNow = (rc.left - cap.rc.left) / dt;
    }
    cap.vy = cap.vy * 0.5f + dTop / dt * 0.5f;
    cap.vx = cap.vx * 0.5f + vxNow * 0.5f;

    bool launch = false;
    float tossSpeed = 0;
    if (cap.vy < cap.carry) {
        cap.carry = cap.vy;
    } else if (cap.carry < -kMinTossSpeed && cap.vy > cap.carry * 0.35f) {
        launch = true;
        tossSpeed = std::max(cap.carry * kTossFactor, -kMaxTossSpeed);
        cap.carry = 0;
    } else if (cap.vy > -kMinTossSpeed * 0.5f) {
        cap.carry = 0;
    }

    const float excess = dTop - std::max(prevVy, 0.0f) * dt;
    const bool drop = excess > 3.0f;
    const float hang = excess > 8.0f ? kHangSeconds * (0.8f + 0.4f * Rand()) : 0.0f;

    float impact = 0;
    int width = static_cast<int>(cap.cols.size());
    bool anyInAir = false;

    for (int c = 0; c < width; c++) {
        Column& col = cap.cols[c];
        if (col.h <= 0) {
            continue;
        }

        if (launch && col.lift == 0) {
            col.vel = tossSpeed * (0.85f + 0.3f * Rand());
            col.hang = kHangSeconds * (0.8f + 0.4f * Rand());
            col.lift = 0.001f;
        } else if (drop && col.lift == 0) {
            col.vel = prevVy * 0.4f;
            col.hang = hang;
            col.lift = excess;
        }

        if (col.lift > 0) {
            if (col.hang > 0) {
                col.hang -= dt;
            } else {
                col.vel += kGravity * dt;
            }
            col.lift -= col.vel * dt;

            if (col.lift <= 0) {
                col.lift = 0;
                impact = std::max(impact, col.vel);
                if (col.vel > kBounceSpeed) {
                    col.vel = -col.vel * 0.25f;
                    col.lift = 0.001f;
                } else {
                    col.vel = 0;
                }
            }
        }
        if (col.lift > 0 || col.vel < 0) {
            anyInAir = true;
        }
    }
    cap.airborne = anyInAir;

    if (launch) {
        cap.squashVel += 1.5f;

        float breakShare =
            0.06f + std::clamp((-tossSpeed / kTossFactor - 900.0f) / 2500.0f,
                               0.0f, 0.25f);
        for (int c = 0; c + kChunkColumns <= width; c += kChunkColumns) {
            float top = 0;
            for (int k = c; k < c + kChunkColumns; k++) {
                top = std::max(top, cap.cols[k].h);
            }
            if (top < 1.0f) {
                continue;
            }
            if (Rand() < breakShare) {
                float area = 0;
                for (int k = c; k < c + kChunkColumns; k++) {
                    area += cap.cols[k].h * 0.35f;
                    cap.cols[k].h *= 0.65f;
                }
                SpawnChunk(rc.left + c + 1.0f, rc.top - top, area,
                           cap.vx * 0.3f + (Rand() - 0.5f) * 100.0f,
                           tossSpeed * (1.1f + 0.4f * Rand()), 0);
            }
            if (Rand() < 0.04f) {
                SpawnPuff(rc.left + c + 1.0f, rc.top - top,
                          (Rand() - 0.5f) * 60.0f,
                          tossSpeed * (0.4f + 0.5f * Rand()),
                          0.6f + Rand() * 0.8f, 0.6f + Rand() * 0.5f);
            }
        }
    }

    if (impact > 0) {
        cap.squashVel -= std::min(impact * 0.003f, 4.0f);
    }
}

void Overlay::Ambient(float dt) {
    const float maxHeight = static_cast<float>(g_settings.capHeight);
    const float wind = g_settings.wind * 0.6f;
    const float powderRate = 0.4f + std::fabs(wind) / 30.0f;

    for (size_t i = 0; i < m_bodies.size(); i++) {
        const Body& b = m_bodies[i];
        if (!b.snowCap) {
            continue;
        }
        auto it = m_caps.find(b.key);
        if (it == m_caps.end()) {
            continue;
        }

        Cap& cap = it->second;

        if (cap.squash != 0 || cap.squashVel != 0) {
            cap.squashVel += (-220.0f * cap.squash - 16.0f * cap.squashVel) * dt;
            cap.squash = std::clamp(cap.squash + cap.squashVel * dt, -0.4f,
                                    0.35f);
            if (std::fabs(cap.squash) < 0.002f &&
                std::fabs(cap.squashVel) < 0.02f) {
                cap.squash = cap.squashVel = 0;
            }
        }

        std::vector<Column>& cols = cap.cols;
        const int width = static_cast<int>(cols.size());
        if (cap.airborne || width < 40) {
            continue;
        }

        if (Rand() < powderRate * width / 1000.0f * dt) {
            int c = static_cast<int>(Rand() * width);
            float x = static_cast<float>(b.rc.left + c);
            float y = b.rc.top - cols[c].h;
            if (cols[c].h > 0.6f * SnowLimit(cap, c, maxHeight) &&
                !IsCoveredAbove(i, static_cast<int>(x),
                                static_cast<int>(y) - 1)) {
                SpawnPuff(x, y, wind * (0.5f + Rand()) + (Rand() - 0.5f) * 20.0f,
                          -15.0f - Rand() * 30.0f, 0.5f + Rand() * 0.7f,
                          1.0f + Rand() * 1.2f);
            }
        }

        constexpr int kLump = 6;
        if (Rand() < 0.05f * dt) {
            bool right = Rand() < 0.5f;
            int first = right ? width - kLump - 2 : 2;
            float area = 0;
            for (int c = first; c < first + kLump; c++) {
                area += cols[c].h * 0.5f;
            }
            float x = static_cast<float>(right ? b.rc.right - 2 : b.rc.left + 2);
            if (area > kLump * 0.3f * maxHeight &&
                !IsCoveredAbove(i, static_cast<int>(x), b.rc.top - 2)) {
                for (int c = first; c < first + kLump; c++) {
                    cols[c].h *= 0.5f;
                }
                SpawnChunk(x, b.rc.top - 2.0f, area,
                           (right ? 1.0f : -1.0f) * (25.0f + Rand() * 30.0f),
                           -20.0f, b.key);
            }
        }
    }
}

void Overlay::Sweep(float dt) {
    if (!g_settings.sweepWithCursor) {
        return;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }

    if (!m_cursorValid) {
        m_lastCursor = pt;
        m_cursorValid = true;
        return;
    }

    float speedX = (pt.x - m_lastCursor.x) / dt;
    float speedY = (pt.y - m_lastCursor.y) / dt;
    float speedSq = speedX * speedX + speedY * speedY;
    if (speedSq < 30.0f * 30.0f) {
        return;
    }

    float cursorVx = (pt.x - m_lastCursor.x) / dt;
    int x0 = std::min(m_lastCursor.x, pt.x) - kSweepRadius;
    int x1 = std::max(m_lastCursor.x, pt.x) + kSweepRadius;
    m_lastCursor = pt;
    const float maxHeight = static_cast<float>(g_settings.capHeight);

    if (x1 - x0 > 300) {
        x0 = pt.x - kSweepRadius;
        x1 = pt.x + kSweepRadius;
    }

    for (size_t i = 0; i < m_bodies.size(); i++) {
        const Body& b = m_bodies[i];
        if (!b.snowCap) {
            continue;
        }
        auto it = m_caps.find(b.key);
        if (it == m_caps.end()) {
            continue;
        }

        Cap& cap = it->second;
        const float top = static_cast<float>(b.rc.top);
        if (pt.y > top + 4 || pt.y < top - 400 || x1 < b.rc.left ||
            x0 >= b.rc.right || IsCoveredAbove(i, pt.x, b.rc.top - 2)) {
            continue;
        }

        std::vector<Column>& cols = cap.cols;
        int from = std::max(x0 - static_cast<int>(b.rc.left), 0);
        int to = std::min(x1 - static_cast<int>(b.rc.left),
                          static_cast<int>(cols.size()));
        for (int c = from; c < to; c += kChunkColumns) {
            float area = 0, height = 0;
            const float base = top - cols[c].lift;
            if (pt.y < base - maxHeight - 6 || pt.y > base + 4) {
                continue;
            }
            for (int k = c; k < std::min(c + kChunkColumns, to); k++) {
                area += cols[k].h;
                height = std::max(height, cols[k].h);
                cols[k].h = 0;
            }
            if (area < 0.5f) {
                continue;
            }
            SpawnChunk(b.rc.left + c + 1.0f, base - height / 2, area,
                       cursorVx * 0.35f + (Rand() - 0.5f) * 80.0f,
                       -Rand() * 120.0f, b.key);
            if (Rand() < 0.35f) {
                SpawnPuff(b.rc.left + c + 1.0f, base - height,
                          cursorVx * 0.2f + (Rand() - 0.5f) * 60.0f,
                          -30.0f - Rand() * 60.0f, 0.6f + Rand() * 0.7f,
                          0.5f + Rand() * 0.5f);
            }
        }
    }
}

void Overlay::UpdateFlakes(float dt) {
    const float wind = g_settings.wind * 3.5f;
    const float speedMult = g_settings.speed / 100.0f;
    const float capSnowMult = g_settings.capHeight / 8.0f;
    m_time += dt;

    for (Flake& f : m_flakes) {
        float sway = std::sin(m_time * 2.0f + f.phase) * f.sway;
        float oldY = f.y;
        f.x += (wind + sway) * dt;
        f.y += f.fallSpeed * speedMult * dt;

        if (f.y >= m_area.bottom) {
            SpawnFlake(f, false);
            continue;
        }

        if (f.x < m_area.left) {
            f.x += m_width;
        } else if (f.x >= m_area.right) {
            f.x -= m_width;
        }

        if (g_settings.capHeight > 0) {
            if (const Body* b = FindLanding(f.x, oldY, f.y, 0)) {
                int col = static_cast<int>(std::floor(f.x)) - b->rc.left;
                AddSnow(b->key, col, (0.35f + f.size * 0.25f) * capSnowMult);
                SpawnFlake(f, false);
            }
        }
    }
}

void Overlay::UpdateChunks(float dt) {
    const float wind = g_settings.wind * 1.5f;

    for (size_t i = m_chunks.size(); i-- > 0;) {
        Chunk& c = m_chunks[i];
        c.vx += (wind - c.vx) * 0.8f * dt;
        c.vy += kGravity * dt;
        c.vx = std::clamp(c.vx, -kChunkMaxSpeed, kChunkMaxSpeed);
        c.vy = std::clamp(c.vy, -kChunkMaxSpeed, kChunkMaxSpeed);

        float oldY = c.y;
        c.x += c.vx * dt;
        c.y += c.vy * dt;

        if (c.y >= m_area.bottom || c.x < m_area.left ||
            c.x >= m_area.right) {
            m_chunks.erase(m_chunks.begin() + i);
            continue;
        }

        if (c.vy > 0) {
            if (const Body* b = FindLanding(c.x, oldY, c.y, c.ignoreKey)) {
                int col = static_cast<int>(std::floor(c.x)) - b->rc.left;
                AddSnow(b->key, col, c.area * 0.7f / kSnowKernelSum);

                if (c.vy > 350.0f) {
                    SpawnPuff(c.x, b->rc.top - 2.0f,
                              c.vx * 0.3f + (Rand() - 0.5f) * 40.0f,
                              -c.vy * 0.2f - Rand() * 30.0f,
                              0.5f + Rand() * 0.5f, 0.4f + Rand() * 0.4f);
                }

                m_chunks.erase(m_chunks.begin() + i);
            }
        }
    }
}

void Overlay::UpdatePuffs(float dt) {
    const float wind = g_settings.wind * 2.5f;

    for (size_t i = m_puffs.size(); i-- > 0;) {
        Puff& p = m_puffs[i];
        p.life -= dt;
        if (p.life <= 0) {
            m_puffs.erase(m_puffs.begin() + i);
            continue;
        }

        p.vx += (wind - p.vx) * 1.5f * dt;
        p.vy += 60.0f * dt;
        p.x += p.vx * dt;
        p.y += p.vy * dt;
    }
}

bool Overlay::InheritSnow(Cap& cap, unsigned lastSeen) {
    bool inherited = false;
    for (auto& [key, other] : m_caps) {
        if (&other == &cap || other.hwnd != cap.hwnd ||
            other.seenFrame != m_frame - 1 || other.seenFrame <= lastSeen) {
            continue;
        }

        for (size_t c = 0; c < other.cols.size(); c++) {
            int dst = other.rc.left + static_cast<int>(c) - cap.rc.left;
            if (dst >= 0 && dst < static_cast<int>(cap.cols.size())) {
                cap.cols[dst].h = std::max(cap.cols[dst].h, other.cols[c].h);
                other.cols[c].h = 0;
            }
        }
        inherited = true;
    }
    return inherited;
}

void Overlay::UpdateCaps(float dt) {
    unsigned lastSeen = m_frame - 1;
    std::vector<HWND> inheritedFrom;

    for (const Body& b : m_bodies) {
        if (!b.snowCap) {
            continue;
        }

        auto [it, inserted] = m_caps.try_emplace(b.key);
        Cap& cap = it->second;
        if (inserted) {
            cap.hwnd = b.hwnd;
            cap.rc = b.rc;
            cap.seed = Hash32(static_cast<uint32_t>(b.key) ^
                              static_cast<uint32_t>(b.key >> 32));
            cap.cols.resize(b.rc.right - b.rc.left);
            if (InheritSnow(cap, lastSeen)) {
                inheritedFrom.push_back(cap.hwnd);
            }
        } else {
            ResizeCap(cap, b.rc);
            MoveCap(cap, b.rc, dt);
        }

        cap.rc = b.rc;
        cap.seenFrame = m_frame;
    }

    for (HWND hwnd : inheritedFrom) {
        for (auto it = m_caps.begin(); it != m_caps.end();) {
            Cap& cap = it->second;
            if (cap.hwnd == hwnd && cap.seenFrame != m_frame) {
                if (cap.seenFrame == m_frame - 1) {
                    DropColumns(cap, 0, static_cast<int>(cap.cols.size()));
                }
                it = m_caps.erase(it);
            } else {
                ++it;
            }
        }
    }

    if (m_frame % 60 == 0) {
        for (auto it = m_caps.begin(); it != m_caps.end();) {
            it = IsWindow(it->second.hwnd) ? std::next(it) : m_caps.erase(it);
        }
    }
}

// ─── VEHICLE (STEAM TRAIN & BULLDOZER) ───
void Overlay::SummonVehicle() {
    if (m_bodies.empty()) return;
    int bestIdx = 0;
    int bestW = 0;
    for (size_t i = 0; i < m_bodies.size(); i++) {
        if (m_bodies[i].snowCap) {
            int w = m_bodies[i].rc.right - m_bodies[i].rc.left;
            if (w > bestW) {
                bestW = w;
                bestIdx = static_cast<int>(i);
            }
        }
    }
    const auto& b = m_bodies[bestIdx];
    float vWidth = (g_settings.vehicleType == VehicleType::Train) ? 110.0f : 46.0f;
    float vHeight = (g_settings.vehicleType == VehicleType::Train) ? 40.0f : 24.0f;
    int dir = (Rand() > 0.5f) ? 1 : -1;
    float startX = (g_settings.vehicleSpawnMode == VehicleSpawnMode::Auto)
        ? ((dir > 0) ? (b.rc.left - 120.0f) : (b.rc.right + 120.0f))
        : (b.rc.left + (b.rc.right - b.rc.left) * 0.5f);

    m_vehicle.x = startX;
    m_vehicle.y = static_cast<float>(b.rc.top);
    m_vehicle.bodyKey = b.key;
    m_vehicle.speed = static_cast<float>(g_settings.vehicleSpeed);
    m_vehicle.width = vWidth;
    m_vehicle.height = vHeight;
    m_vehicle.direction = dir;
    m_vehicle.smokePuffs.clear();
    m_vehicle.isBeingDragged = false;
    m_vehicleActive = true;
    m_vehicleCooldown = 0.0f;
}

void Overlay::UpdateVehicle(const POINT& cursor, bool isLButtonDown, float dt) {
    if (g_settings.vehicleType == VehicleType::None || m_bodies.empty()) {
        m_vehicleActive = false;
        return;
    }

    if (!m_vehicleActive) {
        m_vehicleCooldown = std::max(0.0f, m_vehicleCooldown - dt);

        if (g_settings.vehicleSpawnMode == VehicleSpawnMode::Always) {
            SummonVehicle();
        } else { // Auto
            if (m_vehicleCooldown <= 0.0f) {
                float maxAvgSnow = 0.0f;
                for (size_t i = 0; i < m_bodies.size(); i++) {
                    const auto& b = m_bodies[i];
                    if (!b.snowCap) continue;
                    auto it = m_caps.find(b.key);
                    if (it == m_caps.end() || it->second.cols.empty()) continue;
                    float sumH = 0.0f;
                    for (const auto& col : it->second.cols) sumH += col.h;
                    float avgH = sumH / it->second.cols.size();
                    if (avgH > maxAvgSnow && (b.rc.right - b.rc.left) > 150) {
                        maxAvgSnow = avgH;
                    }
                }

                if (maxAvgSnow >= 2.0f) {
                    SummonVehicle();
                }
            }
        }

        if (!m_vehicleActive) return;
    }

    // Drag & Drop
    if (isLButtonDown) {
        if (!m_vehicle.isBeingDragged) {
            float bzTop = m_vehicle.y - m_vehicle.height;
            if (std::abs(cursor.x - m_vehicle.x) < m_vehicle.width * 0.7f &&
                cursor.y >= bzTop - 8 && cursor.y <= m_vehicle.y + 10) {
                m_vehicle.isBeingDragged = true;
            }
        }
        if (m_vehicle.isBeingDragged) {
            m_vehicle.x = static_cast<float>(cursor.x);
            m_vehicle.y = static_cast<float>(cursor.y);
            return;
        }
    } else if (m_vehicle.isBeingDragged) {
        m_vehicle.isBeingDragged = false;
        uint64_t bestKey = m_vehicle.bodyKey;
        float bestDist = 1e9f;
        float snapY = m_vehicle.y;
        for (const auto& b : m_bodies) {
            if (b.snowCap && m_vehicle.x >= b.rc.left && m_vehicle.x <= b.rc.right) {
                float dist = std::abs(m_vehicle.y - b.rc.top);
                if (dist < bestDist) {
                    bestDist = dist;
                    bestKey = b.key;
                    snapY = static_cast<float>(b.rc.top);
                }
            }
        }
        m_vehicle.bodyKey = bestKey;
        m_vehicle.y = snapY;
    }

    // Offscreen safety guard: despawn if vehicle drove far offscreen
    if (m_vehicle.x < m_area.left - 250.0f || m_vehicle.x > m_area.right + 250.0f) {
        m_vehicleActive = false;
        m_vehicleCooldown = 8.0f;
        return;
    }

    // Plowing and moving
    float targetW = (g_settings.vehicleType == VehicleType::Train) ? 110.0f : 46.0f;
    float targetH = (g_settings.vehicleType == VehicleType::Train) ? 40.0f : 24.0f;
    m_vehicle.width = targetW;
    m_vehicle.height = targetH;
    m_vehicle.speed = static_cast<float>(g_settings.vehicleSpeed);

    m_vehicle.x += m_vehicle.speed * m_vehicle.direction * dt;
    m_vehicle.trackAnim += m_vehicle.speed * 0.05f * dt;
    m_vehicle.wheelAngle += m_vehicle.speed * 0.08f * dt;

    // Chimney smoke puffs
    m_vehicle.smokeTimer -= dt;
    if (m_vehicle.smokeTimer <= 0.0f) {
        m_vehicle.smokeTimer = (g_settings.vehicleType == VehicleType::Train) ? 0.10f : 0.35f;
        float chimX = (g_settings.vehicleType == VehicleType::Train)
            ? ((m_vehicle.direction > 0) ? (m_vehicle.x + 34.0f) : (m_vehicle.x - 34.0f))
            : ((m_vehicle.direction > 0) ? (m_vehicle.x - m_vehicle.width * 0.3f) : (m_vehicle.x + m_vehicle.width * 0.3f));
        float chimY = m_vehicle.y - m_vehicle.height - 4.0f;
        m_vehicle.smokePuffs.push_back({
            chimX,
            chimY,
            -m_vehicle.direction * (15.0f + Rand() * 20.0f),
            -35.0f - Rand() * 30.0f,
            (g_settings.vehicleType == VehicleType::Train) ? (4.0f + Rand() * 4.0f) : (2.5f + Rand() * 2.0f),
            1.2f,
            1.2f
        });
    }

    for (size_t i = m_vehicle.smokePuffs.size(); i-- > 0;) {
        auto& sp = m_vehicle.smokePuffs[i];
        sp.life -= dt;
        if (sp.life <= 0.0f) {
            m_vehicle.smokePuffs.erase(m_vehicle.smokePuffs.begin() + i);
        } else {
            sp.x += sp.vx * dt;
            sp.y += sp.vy * dt;
            sp.vy += 10.0f * dt;
        }
    }

    // Check if body still exists
    auto it = m_caps.find(m_vehicle.bodyKey);
    if (it == m_caps.end()) {
        // Body was closed or minimized: snap to nearest or despawn
        bool foundOther = false;
        for (const auto& b : m_bodies) {
            if (b.snowCap && m_caps.count(b.key)) {
                m_vehicle.bodyKey = b.key;
                m_vehicle.y = static_cast<float>(b.rc.top);
                foundOther = true;
                break;
            }
        }
        if (!foundOther) {
            m_vehicleActive = false;
            m_vehicleCooldown = 5.0f;
            return;
        }
        it = m_caps.find(m_vehicle.bodyKey);
    }

    Cap& cap = it->second;
    int dozerCol = static_cast<int>(m_vehicle.x - cap.rc.left);
    int halfW = static_cast<int>(m_vehicle.width * 0.5f);
    int start = std::max(0, dozerCol - halfW);
    int end = std::min(static_cast<int>(cap.cols.size()) - 1, dozerCol + halfW);

    float plowedArea = 0.0f;
    for (int c = start; c <= end; c++) {
        plowedArea += cap.cols[c].h;
        cap.cols[c].h = 0.0f;
    }

    if (plowedArea > 2.0f && Rand() < 0.35f) {
        float frontX = (m_vehicle.direction > 0) ? (m_vehicle.x + halfW) : (m_vehicle.x - halfW);
        SpawnChunk(frontX, m_vehicle.y - 4.0f, plowedArea * 0.4f,
                   m_vehicle.direction * (40.0f + Rand() * 50.0f),
                   -20.0f - Rand() * 40.0f, m_vehicle.bodyKey);
        SpawnPuff(frontX, m_vehicle.y - 6.0f,
                  m_vehicle.direction * 30.0f, -30.0f, 0.7f, 0.6f);
    }

    if (g_settings.vehicleSpawnMode == VehicleSpawnMode::Always) {
        if (m_vehicle.direction > 0 && m_vehicle.x >= cap.rc.right - 20) {
            m_vehicle.direction = -1;
        } else if (m_vehicle.direction < 0 && m_vehicle.x <= cap.rc.left + 20) {
            m_vehicle.direction = 1;
        }
    } else { // Auto
        bool finished = (m_vehicle.direction > 0 && m_vehicle.x > cap.rc.right + 120.0f) ||
                        (m_vehicle.direction < 0 && m_vehicle.x < cap.rc.left - 120.0f);
        if (finished) {
            for (auto& c : cap.cols) c.h = 0.0f;
            m_vehicleActive = false;
            m_vehicleCooldown = 15.0f;
        }
    }
}

// ─── SNOWBALL ROLLING WITH [Ctrl + E] & SNOWMEN ───
bool Overlay::IsCursorOverSnow(const POINT& cursor) {
    for (size_t i = 0; i < m_bodies.size(); i++) {
        const auto& b = m_bodies[i];
        if (!b.snowCap || IsCoveredAbove(i, cursor.x, cursor.y)) continue;
        if (cursor.x >= b.rc.left && cursor.x <= b.rc.right &&
            cursor.y >= b.rc.top - 35 && cursor.y <= b.rc.top + 15) {
            auto it = m_caps.find(b.key);
            if (it != m_caps.end()) {
                int col = cursor.x - b.rc.left;
                if (col >= 0 && col < static_cast<int>(it->second.cols.size())) {
                    if (it->second.cols[col].h > 0.4f) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void Overlay::UpdateSnowballKeyE(const POINT& cursor, bool isEKeyDown, float dt) {
    if (!g_settings.enableSnowman) {
        m_heldSnowball.reset();
        return;
    }

    if (isEKeyDown) {
        if (!m_heldSnowball) {
            if (IsCursorOverSnow(cursor)) {
                m_heldSnowball = std::make_unique<SnowballItem>(SnowballItem{
                    static_cast<float>(cursor.x),
                    static_cast<float>(cursor.y),
                    8.0f
                });
            }
        } else {
            m_heldSnowball->x = static_cast<float>(cursor.x);
            m_heldSnowball->y = static_cast<float>(cursor.y);

            float gathered = 0.0f;
            for (const auto& b : m_bodies) {
                if (!b.snowCap) continue;
                auto it = m_caps.find(b.key);
                if (it == m_caps.end()) continue;
                Cap& cap = it->second;
                if (cursor.x >= b.rc.left && cursor.x <= b.rc.right &&
                    std::abs(cursor.y - b.rc.top) < 30) {
                    int c = cursor.x - b.rc.left;
                    for (int off = -5; off <= 5; off++) {
                        int col = c + off;
                        if (col >= 0 && col < static_cast<int>(cap.cols.size())) {
                            if (cap.cols[col].h > 0.5f) {
                                gathered += 0.4f;
                                cap.cols[col].h = std::max(0.0f, cap.cols[col].h - 1.2f * dt);
                            }
                        }
                    }
                }
            }

            m_heldSnowball->radius = std::min(24.0f, m_heldSnowball->radius + (3.0f + gathered * 2.0f) * dt);
        }
    } else if (m_heldSnowball) {
        SnowballItem dropped = *m_heldSnowball;
        m_heldSnowball.reset();

        bool fused = false;
        for (size_t i = 0; i < m_snowballs.size(); i++) {
            float dx = std::abs(dropped.x - m_snowballs[i].x);
            float dy = std::abs(dropped.y - m_snowballs[i].y);
            if (dx < 40.0f && dy < 55.0f) {
                float avgX = (dropped.x + m_snowballs[i].x) * 0.5f;
                float baseY = std::max(dropped.y + dropped.radius, m_snowballs[i].y + m_snowballs[i].radius);

                if (m_snowmen.size() >= 8) {
                    m_snowmen.erase(m_snowmen.begin());
                }

                m_snowmen.push_back({
                    avgX,
                    baseY,
                    1.0f,
                    g_settings.scarfColor,
                    g_settings.hatStyle
                });

                SpawnPuff(avgX, baseY - 20.0f, -20.0f, -40.0f, 1.2f, 0.8f);
                m_snowballs.erase(m_snowballs.begin() + i);
                fused = true;
                break;
            }
        }

        if (!fused) {
            m_snowballs.push_back(dropped);
            if (m_snowballs.size() > 10) {
                m_snowballs.erase(m_snowballs.begin());
            }
        }
    }
}

void Overlay::CheckClickSnowman(const POINT& cursor, bool isLButtonDown) {
    if (isLButtonDown && !m_prevLButtonDown && g_settings.enableSnowman) {
        for (size_t i = m_snowmen.size(); i-- > 0;) {
            float dx = cursor.x - m_snowmen[i].x;
            float dy = cursor.y - (m_snowmen[i].y - 18.0f);
            if (dx * dx + dy * dy < 32.0f * 32.0f) {
                for (int k = 0; k < 6; k++) {
                    SpawnPuff(m_snowmen[i].x, m_snowmen[i].y - 18.0f,
                              (Rand() - 0.5f) * 80.0f, -20.0f - Rand() * 50.0f,
                              1.2f, 0.7f);
                    SpawnChunk(m_snowmen[i].x, m_snowmen[i].y - 18.0f, 12.0f,
                               (Rand() - 0.5f) * 100.0f, -30.0f - Rand() * 40.0f, 0);
                }
                m_snowmen.erase(m_snowmen.begin() + i);
                return;
            }
        }

        for (size_t i = m_snowballs.size(); i-- > 0;) {
            float dx = cursor.x - m_snowballs[i].x;
            float dy = cursor.y - m_snowballs[i].y;
            if (dx * dx + dy * dy < 25.0f * 25.0f) {
                SpawnPuff(m_snowballs[i].x, m_snowballs[i].y, 0, -30.0f, 1.0f, 0.5f);
                m_snowballs.erase(m_snowballs.begin() + i);
                return;
            }
        }
    }
}

// ─── RENDERING ───
void Overlay::DrawCap(const Cap& cap, const RECT& rc) {
    const std::vector<Column>& cols = cap.cols;
    int width = static_cast<int>(cols.size());
    if (width < 2) {
        return;
    }

    float highest = 0, liftSum = 0;
    int snowy = 0;
    for (const Column& col : cols) {
        highest = std::max(highest, col.h);
        if (col.h > 0.2f) {
            liftSum += col.lift;
            snowy++;
        }
    }
    if (highest < 0.2f) {
        return;
    }

    ComPtr<ID2D1PathGeometry> path;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(m_d2dFactory->CreatePathGeometry(&path)) ||
        FAILED(path->Open(&sink))) {
        return;
    }

    const float dx = g_settings.slideOff
                         ? 0.0f
                         : std::clamp(cap.vx * m_frameTime, -40.0f, 40.0f);
    const float dy = -std::min(std::fabs(cap.vy) * m_frameTime, 40.0f);
    const float left = rc.left + dx;
    const float top = rc.top + dy;
    const float scale = 1.0f + cap.squash;

    auto height = [&](int c) {
        float edge = static_cast<float>(std::min(c, width - 1 - c));
        return cols[c].h * scale * std::clamp(edge / kCapEdgeTaper, 0.0f, 1.0f);
    };

    constexpr float kMinHeight = 0.3f;
    for (int c = 0; c < width;) {
        if (height(c) < kMinHeight) {
            c++;
            continue;
        }
        int first = c;
        while (c < width && height(c) >= kMinHeight) {
            c++;
        }
        int last = c - 1;

        float x0 = left + first;
        float x1 = left + last + 1.0f;
        sink->BeginFigure({x0, top - cols[first].lift},
                          D2D1_FIGURE_BEGIN_FILLED);
        for (int k = first; k <= last; k += 2) {
            sink->AddLine({left + k + 0.5f, top - cols[k].lift - height(k)});
        }
        sink->AddLine({x1 - 0.5f, top - cols[last].lift - height(last)});
        sink->AddLine({x1, top - cols[last].lift});
        if (cap.airborne) {
            for (int k = last; k >= first; k -= 2) {
                sink->AddLine({left + k + 0.5f, top - cols[k].lift});
            }
        }
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    }
    if (FAILED(sink->Close())) {
        return;
    }

    const float base = top - (snowy ? liftSum / snowy : 0.0f);
    m_capBrush->SetStartPoint({0, base - highest * scale});
    m_capBrush->SetEndPoint({0, base});
    m_ctx->FillGeometry(path.Get(), m_capBrush.Get());
}

void Overlay::DrawBulldozer(const VehicleItem& v) {
    float x = v.x;
    float y = v.y;
    float w = v.width;
    float h = v.height;
    int dir = v.direction;

    // Body
    D2D1_ROUNDED_RECT bodyRect = D2D1::RoundedRect(
        D2D1::RectF(x - w * 0.5f, y - h, x + w * 0.5f, y - 4.0f), 3.0f, 3.0f);
    m_ctx->FillRoundedRectangle(bodyRect, m_dozerBodyBrush.Get());
    m_ctx->DrawRoundedRectangle(bodyRect, m_coalBrush.Get(), 1.0f);

    // Cab window
    float winW = 12.0f;
    float winH = 8.0f;
    float winX = x + (dir > 0 ? 0.0f : -winW);
    D2D1_ROUNDED_RECT winRect = D2D1::RoundedRect(
        D2D1::RectF(winX, y - h + 2.0f, winX + winW, y - h + 2.0f + winH), 2.0f, 2.0f);
    m_ctx->FillRoundedRectangle(winRect, m_dozerGlassBrush.Get());
    m_ctx->DrawRoundedRectangle(winRect, m_coalBrush.Get(), 1.0f);

    // Track
    D2D1_ROUNDED_RECT trackRect = D2D1::RoundedRect(
        D2D1::RectF(x - w * 0.5f - 2.0f, y - 6.0f, x + w * 0.5f + 2.0f, y + 1.0f), 3.0f, 3.0f);
    m_ctx->FillRoundedRectangle(trackRect, m_dozerTrackBrush.Get());

    // Wheels inside track with animated rotation
    for (int i = 0; i < 4; i++) {
        float wheelX = x - w * 0.5f + 4.0f + i * (w / 3.8f);
        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(wheelX, y - 2.5f), 2.5f, 2.5f), m_dozerBladeBrush.Get());
        float a = v.wheelAngle + i * 0.5f;
        m_ctx->DrawLine(D2D1::Point2F(wheelX - std::cos(a) * 2.0f, y - 2.5f - std::sin(a) * 2.0f),
                        D2D1::Point2F(wheelX + std::cos(a) * 2.0f, y - 2.5f + std::sin(a) * 2.0f),
                        m_coalBrush.Get(), 0.8f);
    }

    // Blade
    float bladeX = dir > 0 ? (x + w * 0.5f) : (x - w * 0.5f - 4.0f);
    D2D1_ROUNDED_RECT bladeRect = D2D1::RoundedRect(
        D2D1::RectF(bladeX, y - h - 3.0f, bladeX + 4.0f, y), 1.0f, 1.0f);
    m_ctx->FillRoundedRectangle(bladeRect, m_dozerBladeBrush.Get());
    m_ctx->DrawRoundedRectangle(bladeRect, m_coalBrush.Get(), 1.0f);

    // Exhaust
    float exhaustX = dir > 0 ? (x - w * 0.3f) : (x + w * 0.3f);
    m_ctx->FillRectangle(D2D1::RectF(exhaustX - 1.5f, y - h - 6.0f, exhaustX + 1.5f, y - h), m_dozerTrackBrush.Get());
}

void Overlay::DrawTrain(const VehicleItem& v) {
    float x = v.x;
    float y = v.y;
    int dir = v.direction;

    // 1. Cargo Wagon (Tender)
    float wagonX = dir > 0 ? (x - 46.0f) : (x + 18.0f);
    D2D1_ROUNDED_RECT wagonRect = D2D1::RoundedRect(
        D2D1::RectF(wagonX, y - 22.0f, wagonX + 28.0f, y - 6.0f), 2.0f, 2.0f);
    m_ctx->FillRoundedRectangle(wagonRect, m_trainTenderBrush.Get());
    m_ctx->DrawRoundedRectangle(wagonRect, m_coalBrush.Get(), 1.0f);

    m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(wagonX + 14.0f, y - 22.0f), 12.0f, 5.0f), m_whiteBrush.Get());
    m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(wagonX + 7.0f, y - 4.0f), 4.0f, 4.0f), m_dozerTrackBrush.Get());
    m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(wagonX + 21.0f, y - 4.0f), 4.0f, 4.0f), m_dozerTrackBrush.Get());

    // Coupling
    float linkX = dir > 0 ? (x - 18.0f) : (x + 14.0f);
    m_ctx->FillRectangle(D2D1::RectF(linkX, y - 12.0f, linkX + 6.0f, y - 9.0f), m_dozerTrackBrush.Get());

    // 2. Cabin
    float cabX = dir > 0 ? (x - 12.0f) : (x - 16.0f);
    D2D1_ROUNDED_RECT cabRect = D2D1::RoundedRect(
        D2D1::RectF(cabX, y - 36.0f, cabX + 26.0f, y - 6.0f), 2.0f, 2.0f);
    m_ctx->FillRoundedRectangle(cabRect, m_dozerBodyBrush.Get());
    m_ctx->DrawRoundedRectangle(cabRect, m_coalBrush.Get(), 1.0f);

    float winX = cabX + (dir > 0 ? 6.0f : 4.0f);
    D2D1_ROUNDED_RECT cabWin = D2D1::RoundedRect(
        D2D1::RectF(winX, y - 32.0f, winX + 14.0f, y - 20.0f), 2.0f, 2.0f);
    m_ctx->FillRoundedRectangle(cabWin, m_dozerGlassBrush.Get());
    m_ctx->DrawRoundedRectangle(cabWin, m_coalBrush.Get(), 1.0f);

    // 3. Boiler
    float boilerX = dir > 0 ? (x + 12.0f) : (x - 44.0f);
    D2D1_ROUNDED_RECT boilerRect = D2D1::RoundedRect(
        D2D1::RectF(boilerX, y - 26.0f, boilerX + 32.0f, y - 6.0f), 4.0f, 4.0f);
    m_ctx->FillRoundedRectangle(boilerRect, m_dozerBodyBrush.Get());
    m_ctx->DrawRoundedRectangle(boilerRect, m_coalBrush.Get(), 1.0f);

    m_ctx->FillRectangle(D2D1::RectF(boilerX + 10.0f, y - 26.0f, boilerX + 12.0f, y - 6.0f), m_trainTrimBrush.Get());
    m_ctx->FillRectangle(D2D1::RectF(boilerX + 22.0f, y - 26.0f, boilerX + 24.0f, y - 6.0f), m_trainTrimBrush.Get());

    // 4. Chimney
    float chimX = dir > 0 ? (boilerX + 22.0f) : (boilerX + 10.0f);
    m_ctx->FillRectangle(D2D1::RectF(chimX - 3.5f, y - 38.0f, chimX + 3.5f, y - 24.0f), m_trainChimneyBrush.Get());
    D2D1_ROUNDED_RECT chimTrim = D2D1::RoundedRect(
        D2D1::RectF(chimX - 5.5f, y - 40.0f, chimX + 5.5f, y - 37.0f), 1.0f, 1.0f);
    m_ctx->FillRoundedRectangle(chimTrim, m_trainTrimBrush.Get());

    // 5. Headlight
    float lightX = dir > 0 ? (boilerX + 31.0f) : (boilerX - 4.0f);
    m_ctx->FillRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(lightX, y - 22.0f, lightX + 5.0f, y - 14.0f), 1.0f, 1.0f),
        m_trainTrimBrush.Get());
    m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(lightX + (dir > 0 ? 5.0f : 0.0f), y - 18.0f), 7.0f, 7.0f), m_trainLightBrush.Get());

    // 6. Driving Wheels with rotating animated spokes
    for (int i = 0; i < 3; i++) {
        float wx = (dir > 0 ? (x - 6.0f) : (x - 38.0f)) + i * 16.0f;
        float wy = y - 7.0f;
        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(wx, wy), 6.5f, 6.5f), m_dozerTrackBrush.Get());
        m_ctx->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(wx, wy), 6.5f, 6.5f), m_coalBrush.Get(), 1.0f);
        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(wx, wy), 2.5f, 2.5f), m_trainTrimBrush.Get());

        float a = v.wheelAngle + i * 0.7f;
        m_ctx->DrawLine(D2D1::Point2F(wx - std::cos(a) * 5.0f, wy - std::sin(a) * 5.0f),
                        D2D1::Point2F(wx + std::cos(a) * 5.0f, wy + std::sin(a) * 5.0f),
                        m_trainTrimBrush.Get(), 1.0f);
    }

    // 7. Cowcatcher / Plow
    float plowX = dir > 0 ? (boilerX + 32.0f) : (boilerX - 8.0f);
    m_ctx->FillRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(plowX, y - 14.0f, plowX + 8.0f, y), 2.0f, 2.0f),
        m_dozerBladeBrush.Get());
    m_ctx->DrawRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(plowX, y - 14.0f, plowX + 8.0f, y), 2.0f, 2.0f),
        m_coalBrush.Get(), 1.0f);
}

void Overlay::DrawVehicle() {
    if (!m_vehicleActive || g_settings.vehicleType == VehicleType::None) {
        return;
    }

    for (const auto& sp : m_vehicle.smokePuffs) {
        float fade = std::clamp(sp.life / sp.maxLife, 0.0f, 1.0f);
        float r = sp.size * (1.0f + (1.0f - fade) * 0.8f);
        m_ctx->DrawBitmap(m_flakeSprite.Get(),
                          D2D1::RectF(sp.x - r, sp.y - r, sp.x + r, sp.y + r),
                          0.65f * fade, D2D1_INTERPOLATION_MODE_LINEAR);
    }

    if (g_settings.vehicleType == VehicleType::Bulldozer) {
        DrawBulldozer(m_vehicle);
    } else {
        DrawTrain(m_vehicle);
    }
}

void Overlay::DrawSnowmen() {
    for (const auto& sm : m_snowmen) {
        float x = sm.x;
        float y = sm.y;

        float rBottom = 18.0f;
        float rHead = 12.0f;
        float cyBottom = y - rBottom;
        float cyHead = cyBottom - rBottom - rHead + 4.0f;

        // Twig arms
        m_ctx->DrawLine(D2D1::Point2F(x - rBottom + 2.0f, cyBottom - 2.0f),
                        D2D1::Point2F(x - rBottom - 18.0f, cyBottom - 14.0f),
                        m_twigBrush.Get(), 2.0f);
        m_ctx->DrawLine(D2D1::Point2F(x + rBottom - 2.0f, cyBottom - 2.0f),
                        D2D1::Point2F(x + rBottom + 18.0f, cyBottom - 14.0f),
                        m_twigBrush.Get(), 2.0f);

        // Body spheres
        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, cyBottom), rBottom, rBottom), m_whiteBrush.Get());
        m_ctx->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, cyBottom), rBottom, rBottom), m_dozerGlassBrush.Get(), 1.0f);

        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, cyHead), rHead, rHead), m_whiteBrush.Get());
        m_ctx->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, cyHead), rHead, rHead), m_dozerGlassBrush.Get(), 1.0f);

        // Coal buttons
        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, cyBottom - 6.0f), 2.0f, 2.0f), m_coalBrush.Get());
        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, cyBottom + 2.0f), 2.0f, 2.0f), m_coalBrush.Get());

        // Eyes
        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x - 4.0f, cyHead - 2.0f), 1.8f, 1.8f), m_coalBrush.Get());
        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + 4.0f, cyHead - 2.0f), 1.8f, 1.8f), m_coalBrush.Get());

        // Carrot nose
        m_ctx->DrawLine(D2D1::Point2F(x, cyHead + 2.0f), D2D1::Point2F(x + 9.0f, cyHead + 3.0f), m_carrotBrush.Get(), 2.5f);

        // Smile
        for (int s = -2; s <= 2; s++) {
            float sx = x + s * 2.5f;
            float sy = cyHead + 7.0f + (s * s) * 0.4f;
            m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(sx, sy), 0.9f, 0.9f), m_coalBrush.Get());
        }

        // Scarf
        ID2D1SolidColorBrush* scarfBrush = GetScarfBrush(sm.scarf);
        D2D1_ROUNDED_RECT scarfNeck = D2D1::RoundedRect(
            D2D1::RectF(x - 9.0f, cyHead + rHead - 3.0f, x + 9.0f, cyHead + rHead + 4.0f), 2.5f, 2.5f);
        m_ctx->FillRoundedRectangle(scarfNeck, scarfBrush);
        D2D1_ROUNDED_RECT scarfTail = D2D1::RoundedRect(
            D2D1::RectF(x + 2.0f, cyHead + rHead + 2.0f, x + 8.0f, cyHead + rHead + 16.0f), 2.0f, 2.0f);
        m_ctx->FillRoundedRectangle(scarfTail, scarfBrush);

        // Hat
        if (sm.hat == HatStyle::TopHat) {
            m_ctx->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(x - 14.0f, cyHead - rHead - 1.0f, x + 14.0f, cyHead - rHead + 3.0f), 1.0f, 1.0f),
                m_topHatBrush.Get());
            m_ctx->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(x - 8.0f, cyHead - rHead - 18.0f, x + 8.0f, cyHead - rHead), 1.0f, 1.0f),
                m_topHatBrush.Get());
            m_ctx->FillRectangle(
                D2D1::RectF(x - 8.0f, cyHead - rHead - 4.0f, x + 8.0f, cyHead - rHead - 1.0f),
                m_scarfRedBrush.Get());
        } else if (sm.hat == HatStyle::SantaHat) {
            m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + 5.0f, cyHead - rHead - 6.0f), 9.0f, 7.0f), m_santaHatBrush.Get());
            m_ctx->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(x - 12.0f, cyHead - rHead - 2.0f, x + 12.0f, cyHead - rHead + 3.0f), 2.0f, 2.0f),
                m_whiteBrush.Get());
            m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + 14.0f, cyHead - rHead - 4.0f), 3.5f, 3.5f), m_whiteBrush.Get());
        } else if (sm.hat == HatStyle::Bucket) {
            m_ctx->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(x - 8.0f, cyHead - rHead - 14.0f, x + 8.0f, cyHead - rHead + 1.0f), 2.0f, 2.0f),
                m_bucketBrush.Get());
            m_ctx->DrawRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(x - 8.0f, cyHead - rHead - 14.0f, x + 8.0f, cyHead - rHead + 1.0f), 2.0f, 2.0f),
                m_coalBrush.Get(), 1.0f);
        } else { // Beanie
            m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, cyHead - rHead - 2.0f), 10.0f, 8.0f), m_scarfBlueBrush.Get());
            m_ctx->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(x - 11.0f, cyHead - rHead - 2.0f, x + 11.0f, cyHead - rHead + 3.0f), 2.0f, 2.0f),
                m_whiteBrush.Get());
            m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, cyHead - rHead - 10.0f), 3.0f, 3.0f), m_whiteBrush.Get());
        }
    }
}

void Overlay::DrawSnowballs() {
    for (const auto& sb : m_snowballs) {
        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(sb.x, sb.y), sb.radius, sb.radius), m_whiteBrush.Get());
        m_ctx->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(sb.x, sb.y), sb.radius, sb.radius), m_dozerGlassBrush.Get(), 1.0f);
    }
    if (m_heldSnowball) {
        m_ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(m_heldSnowball->x, m_heldSnowball->y), m_heldSnowball->radius, m_heldSnowball->radius), m_whiteBrush.Get());
        m_ctx->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(m_heldSnowball->x, m_heldSnowball->y), m_heldSnowball->radius, m_heldSnowball->radius), m_scarfGoldBrush.Get(), 1.5f);
    }
}

bool Overlay::Render() {
    m_ctx->BeginDraw();
    m_ctx->SetTransform(D2D1::Matrix3x2F::Translation(
        static_cast<float>(-m_area.left), static_cast<float>(-m_area.top)));
    m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));

    bool anyDrawn = false;
    for (size_t i = m_bodies.size(); i-- > 0;) {
        const Body& b = m_bodies[i];
        if (anyDrawn) {
            m_ctx->PushAxisAlignedClip(
                D2D1::RectF(static_cast<float>(b.rc.left),
                            static_cast<float>(b.rc.top),
                            static_cast<float>(b.rc.right),
                            static_cast<float>(b.rc.bottom)),
                D2D1_ANTIALIAS_MODE_ALIASED);
            m_ctx->Clear(D2D1::ColorF(0, 0, 0, 0));
            m_ctx->PopAxisAlignedClip();
        }

        if (!b.snowCap) {
            continue;
        }
        auto it = m_caps.find(b.key);
        if (it != m_caps.end()) {
            DrawCap(it->second, b.rc);
            anyDrawn = true;
        }
    }

    for (const Chunk& c : m_chunks) {
        float r = c.size * 2.0f;
        m_ctx->DrawBitmap(m_flakeSprite.Get(),
                          D2D1::RectF(c.x - r, c.y - r, c.x + r, c.y + r), 0.95f,
                          D2D1_INTERPOLATION_MODE_LINEAR);
    }

    for (const Puff& p : m_puffs) {
        float r = p.size * 2.0f;
        float alpha = 0.7f * std::min(1.0f, 2.5f * p.life / p.maxLife);
        m_ctx->DrawBitmap(m_flakeSprite.Get(),
                          D2D1::RectF(p.x - r, p.y - r, p.x + r, p.y + r),
                          alpha, D2D1_INTERPOLATION_MODE_LINEAR);
    }

    DrawSnowballs();
    DrawSnowmen();
    DrawVehicle();

    for (const Flake& f : m_flakes) {
        float r = f.size * 2.0f;
        m_ctx->DrawBitmap(m_flakeSprite.Get(),
                          D2D1::RectF(f.x - r, f.y - r, f.x + r, f.y + r),
                          f.alpha, D2D1_INTERPOLATION_MODE_LINEAR);
    }

    if (FAILED(m_ctx->EndDraw())) {
        return false;
    }

    HRESULT hr = m_swapChain->Present(m_syncInterval, 0);
    return SUCCEEDED(hr) || hr == DXGI_STATUS_OCCLUDED;
}

void Overlay::Run() {
    LARGE_INTEGER freq, last;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);

    for (;;) {
        DWORD result;
        if (m_visible && m_frameWait) {
            HANDLE handles[] = {g_stopEvent, m_frameWait};
            result = WaitForMultipleObjects(2, handles, FALSE, 200);
        } else {
            result = MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE,
                                               m_visible ? 16 : 100,
                                               QS_ALLINPUT);
        }
        if (result == WAIT_OBJECT_0) {
            break;
        }

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float dt = static_cast<float>(now.QuadPart - last.QuadPart) /
                   freq.QuadPart;
        last = now;
        dt = std::clamp(dt, 0.001f, 0.05f);
        m_frameTime = m_frameTime * 0.9f + dt * 0.1f;
        m_frame++;

        bool fullscreen =
            g_settings.pauseInFullscreen && IsFullscreenAppActive();
        if (fullscreen == m_visible) {
            m_visible = !fullscreen;
            g_overlayHidden = !m_visible;
            ShowWindow(m_hwnd, m_visible ? SW_SHOWNOACTIVATE : SW_HIDE);
            m_cursorValid = false;
        }
        if (!m_visible) {
            continue;
        }

        if (m_frame % 60 == 0) {
            RECT area = ComputeArea();
            if (!EqualRect(&area, &m_area)) {
                ApplyArea(area);
            }
            m_syncInterval = ComputeSyncInterval();
            SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }

        size_t targetFlakes = static_cast<size_t>(
            g_settings.intensity * m_width * m_height / 1000000.0f);
        if (m_flakes.size() < targetFlakes) {
            size_t addCount = std::min(targetFlakes - m_flakes.size(), size_t(20));
            for (size_t k = 0; k < addCount; k++) {
                Flake f;
                SpawnFlake(f, m_frame < 5);
                m_flakes.push_back(f);
            }
        } else if (m_flakes.size() > targetFlakes) {
            m_flakes.resize(targetFlakes);
        }

        POINT cursor = {};
        GetCursorPos(&cursor);
        bool isLButtonDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        bool isCtrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool isEKeyDown = isCtrlDown && ((GetAsyncKeyState('E') & 0x8000) != 0);

        CollectBodies();
        UpdateCaps(dt);
        Ambient(dt);
        Sweep(dt);
        CheckClickSnowman(cursor, isLButtonDown);
        UpdateSnowballKeyE(cursor, isEKeyDown, dt);
        UpdateVehicle(cursor, isLButtonDown, dt);
        UpdateFlakes(dt);
        UpdateChunks(dt);
        UpdatePuffs(dt);

        m_prevLButtonDown = isLButtonDown;

        if (!Render()) {
            Wh_Log(L"Recreating device resources");
            ReleaseDeviceResources();
            if (!CreateDeviceResources()) {
                Wh_Log(L"Failed to recreate device resources");
                break;
            }
        }
    }
}

DWORD WINAPI SnowThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    g_taskbarThread = CreateThread(nullptr, 0, TaskbarThread, nullptr, 0,
                                   nullptr);

    {
        Overlay overlay;
        if (overlay.Create()) {
            overlay.Run();
        } else {
            Wh_Log(L"Failed to create the overlay");
        }
    }

    if (g_taskbarThread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_taskbarThread, INFINITE);
        CloseHandle(g_taskbarThread);
        g_taskbarThread = nullptr;
    }

    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_snowThread = CreateThread(nullptr, 0, SnowThread, nullptr, 0, nullptr);
    if (!g_snowThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    SetEvent(g_stopEvent);
    WaitForSingleObject(g_snowThread, INFINITE);
    CloseHandle(g_snowThread);
    CloseHandle(g_stopEvent);
    g_snowThread = nullptr;
    g_stopEvent = nullptr;
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
