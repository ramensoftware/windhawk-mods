// ==WindhawkMod==
// @id              modern-run-dialog-ui
// @name            Modern Run Dialog UI For Windows 11
// @name:vi-VN      Giao diện Hộp thoại Run Hiện đại cho Windows 11
// @description     Transforms the classic Run dialog into a modern Fluent UI with Acrylic blur, history, timestamps, and Taskbar toggle.
// @description:vi-VN Biến đổi hộp thoại Run cổ điển thành giao diện Fluent UI hiện đại với hiệu ứng kính mờ Acrylic, lịch sử, thời gian chạy và đóng/mở từ Taskbar.
// @version         2.0.15
// @author          caotu2k5
// @github          https://github.com/caotu2k5
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -luser32 -lshell32 -lshlwapi -lcomctl32 -lversion -lgdiplus -limm32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Modern Run Dialog UI For Windows 11

![Modern Run Dialog UI Preview](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/screenshot1.png)

Modern Run Dialog UI Preview

---

## 🌐 English

**Modern Run Dialog UI For Windows 11** replaces the classic Win32 **Run (`Win + R`)** dialog with a sleek, borderless **Fluent Design / Command Palette** interface tailored for Windows 11.

### ✨ Key Features

* **Fluent UI & Acrylic Blur:**
  * Completely removes the classic title bar and legacy window frames while preserving native Windows 11 rounded corners.
  * Full-window **Acrylic Blur** background with customizable opacity.
  * **GDI+ 32-bit ARGB (Premultiplied Alpha)** rendering engine with crisp system **Segoe UI** typography and full Per-Monitor DPI scaling.

* **Smart History List:**
  * Displays up to **5 recently executed commands** right above the input bar.
  * Automatically extracts **native application icons** and **friendly application names** in the `command - App Name` format.
  * Shows the **last executed timestamp** next to the history clock icon on each row (stored cleanly in Windhawk mod storage).
  * Supports single-click to fill the input box, double-click to execute immediately, or **Up / Down (`↑` / `↓`)** arrow keys to cycle through history.

* **Modern Input Bar & Action Buttons:**
  * Borderless single-line input box with automatic horizontal scrolling for long file paths.
  * **Browse Button (`Browse 📁`):** Opens the native Windows file picker (`*.exe`, `*.bat`, `*.cmd`, `*.msc`, `*.cpl`) and inserts the full path into the input bar.
  * **Run (`Run ↵`) & Exit (`Exit ✕`) Buttons:** High-contrast Fluent accent buttons with smooth hover and press states.

* **Quality-of-Life Fixes:**
  * **Taskbar Icon Click to Minimize / Restore:** Click the Run icon on the Taskbar to minimize (hide) or restore the Run window like a normal app.
  * **`Win + R` Toggle:** Pressing `Win + R` while the Run dialog is open will immediately close it (or restore it if minimized).
  * **Flawless IME Input:** Works seamlessly with Windows IME (Vietnamese Telex/VNI, CJK) and Unikey without composition glitches.

* **Multilingual Interface:**
  * Automatically detects the Windows UI language or lets you manually choose from 10 languages.

---

## 🇻🇳 Tiếng Việt

**Modern Run Dialog UI For Windows 11** thay thế hộp thoại **Run (`Win + R`)** cổ điển bằng giao diện **Fluent Design / Command Palette** hiện đại, không viền và trong suốt chuẩn Windows 11.

### ✨ Tính năng nổi bật

* **Giao diện Fluent & Kính mờ (Acrylic Blur):**
  * Loại bỏ thanh tiêu đề và khung viền cổ điển, giữ góc bo tròn tự nhiên của Windows 11.
  * Làm mờ nền trong suốt (**Acrylic Blur**) toàn cửa sổ, tùy chỉnh độ trong suốt linh hoạt.
  * Kết xuất đồ họa bằng **GDI+ 32-bit ARGB** với phông chữ hệ thống **Segoe UI** sắc nét, hỗ trợ tự động co giãn theo DPI trên từng màn hình.

* **Lịch sử thông minh:**
  * Hiển thị tối đa **5 lệnh chạy gần nhất** kèm biểu tượng gốc và tên ứng dụng (`lệnh - Tên ứng dụng`).
  * Hiển thị **thời gian chạy lệnh gần nhất** ngay bên cạnh biểu tượng đồng hồ ở mỗi dòng.
  * Hỗ trợ bấm chọn nhanh, nhấp đúp để chạy ngay, hoặc dùng phím **Lên / Xuống (`↑` / `↓`)**.

* **Thanh nhập liệu & Nút chức năng:**
  * Ô nhập lệnh không viền, tự cuộn ngang khi đường dẫn dài.
  * **Nút Duyệt (`Duyệt 📁`):** Mở hộp thoại chọn tệp hệ thống (`*.exe`, `*.bat`, `*.cmd`, `*.msc`, `*.cpl`) và đưa đường dẫn lên thanh nhập.
  * **Nút Chạy (`Chạy ↵`) & Thoát (`Thoát ✕`):** Nổi bật, trực quan với hiệu ứng chuyển màu khi rê chuột.

* **Cải tiến trải nghiệm (Quality-of-Life):**
  * **Bấm icon Taskbar để Ẩn / Mở cửa sổ:** Cho phép click vào icon Run dưới thanh Taskbar để thu nhỏ hoặc khôi phục cửa sổ.
  * **Nhấn `Win + R` để đóng nhanh:** Nhấn lại `Win + R` khi hộp thoại đang mở sẽ lập tức đóng hoặc khôi phục cửa sổ.
  * **Tương thích hoàn hảo bộ gõ Tiếng Việt (IME):** Hoạt động mượt mà với Unikey, EVKey và bộ gõ Windows, không bị lỗi khung trắng đè chữ.

* **Hỗ trợ Đa ngôn ngữ:**
  * Tự động nhận diện ngôn ngữ Windows hoặc tùy chọn cố định 10 ngôn ngữ.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Display Language
  $name:vi-VN: Ngôn ngữ hiển thị
  $description: Auto-detect system language or choose a fixed language.
  $description:vi-VN: Tự động theo hệ điều hành hoặc chọn ngôn ngữ cố định.
  $options:
  - auto: Auto (System Default)
  - en: English
  - vi: Tiếng Việt (Vietnamese)
  - ja: 日本語 (Japanese)
  - zh_cn: 简体中文 (Simplified Chinese)
  - zh_tw: 繁體中文 (Traditional Chinese)
  - ko: 한국어 (Korean)
  - fr: Français (French)
  - de: Deutsch (German)
  - es: Español (Spanish)
  - ru: Русский (Russian)
  $options:vi-VN:
  - auto: Tự động theo Windows
  - en: English (Tiếng Anh)
  - vi: Tiếng Việt
  - ja: 日本語 (Tiếng Nhật)
  - zh_cn: 简体中文 (Tiếng Trung Giản thể)
  - zh_tw: 繁體中文 (Tiếng Trung Phồn thể)
  - ko: 한국어 (Tiếng Hàn)
  - fr: Français (Tiếng Pháp)
  - de: Deutsch (Tiếng Đức)
  - es: Español (Tiếng Tây Ban Nha)
  - ru: Русский (Tiếng Nga)
- taskbarClickAction: minimize
  $name: Taskbar Icon Click Action
  $name:vi-VN: Hành vi khi bấm icon Run trên Taskbar
  $description: Action when clicking the Run icon on the Taskbar while open.
  $description:vi-VN: Hành động khi bấm vào biểu tượng Run dưới Taskbar lúc cửa sổ đang mở.
  $options:
  - minimize: Minimize & Restore
  - close: Close Window
  $options:vi-VN:
  - minimize: Ẩn & Mở lại cửa sổ Run
  - close: Đóng hẳn cửa sổ Run
- enableBlur: true
  $name: Acrylic Blur Effect
  $name:vi-VN: Hiệu ứng làm mờ trong suốt (Blur)
  $description: Enable or disable the translucent Acrylic blur background.
  $description:vi-VN: Bật hoặc tắt hiệu ứng làm mờ nền trong suốt toàn cửa sổ.
- blurOpacity: 165
  $name: Blur Opacity (0 - 255)
  $name:vi-VN: Độ đậm nền Blur (0 - 255)
  $description: Lower values make the window more transparent (0 - 255, default is 165).
  $description:vi-VN: Giá trị càng nhỏ càng trong suốt (0 - 255, mặc định là 165).
- maxHistoryItems: 5
  $name: Max History Items (0 - 5)
  $name:vi-VN: Số mục lịch sử tối đa (0 - 5)
  $description: Maximum number of recent commands shown in the top list.
  $description:vi-VN: Số lượng lệnh gần đây hiển thị ở phần phía trên.
- showTimestamp: true
  $name: Show Last Executed Time
  $name:vi-VN: Hiển thị thời gian chạy lệnh gần nhất
  $description: Display the execution timestamp next to the clock icon on each history row.
  $description:vi-VN: Hiển thị thời gian thực thi ngay bên cạnh biểu tượng đồng hồ ở mỗi dòng lịch sử.
- clearInitialInput: true
  $name: Clear Input on Open
  $name:vi-VN: Để trống ô nhập khi mở
  $description: Show placeholder text on open instead of pre-filling the last command.
  $description:vi-VN: Hiển thị dòng gợi ý khi mới mở thay vì điền sẵn lệnh cũ.
- closeOnClickOutside: false
  $name: Close on Click Outside
  $name:vi-VN: Tự động đóng khi bấm ra ngoài
  $description: Automatically close the Run dialog when clicking outside the window.
  $description:vi-VN: Tự động đóng hộp thoại Run khi click chuột ra ngoài cửa sổ.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <imm.h>
#include <cwchar>
#include <string>
#include <vector>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <thread>
#include <optional>
#include <condition_variable>

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
using ShowWindow_t = decltype(&ShowWindow);
using SetForegroundWindow_t = decltype(&SetForegroundWindow);
using SwitchToThisWindow_t = decltype(&SwitchToThisWindow);
using RunDlgProc_t = INT_PTR(CALLBACK*)(HWND, UINT, WPARAM, LPARAM);

ShowWindow_t ShowWindow_Orig = nullptr;
SetForegroundWindow_t SetForegroundWindow_Orig = nullptr;
SwitchToThisWindow_t SwitchToThisWindow_Orig = nullptr;
RunDlgProc_t RunDlgProc_Orig = nullptr;

struct LocalizedStrings {
    const wchar_t* placeholder;
    const wchar_t* browseBtn;
    const wchar_t* runBtn;
    const wchar_t* exitBtn;
    const wchar_t* todayPrefix;
};

struct ModSettings {
    std::wstring language = L"auto";
    bool taskbarCloseInsteadOfMinimize = false;
    bool enableBlur = true;
    int blurOpacity = 165;
    int maxHistoryItems = 5;
    bool showTimestamp = true;
    bool clearInitialInput = true;
    bool closeOnClickOutside = false;
};

std::mutex g_settingsMutex;
ModSettings g_settings;
LocalizedStrings g_i18n = {
    L"Type the name of a command to run",
    L"Browse",
    L"Run",
    L"Exit",
    L"Today"
};

std::mutex g_gdiplusMutex;
ULONG_PTR g_gdiplusToken = 0;

bool EnsureGdiplusInitialized() {
    std::lock_guard<std::mutex> lock(g_gdiplusMutex);
    if (g_gdiplusToken != 0) {
        return true;
    }
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    return Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr) == Gdiplus::Ok;
}

// Registered messages for safe cross-thread communication with the Run dialog thread
UINT g_msgUninit = 0;
UINT g_msgToggle = 0;
HANDLE g_hUninitEvent = nullptr;
std::atomic<bool> g_unloading{false};

enum ToggleAction : WPARAM {
    kActionMinimize = 1,
    kActionRestore = 2,
    kActionToggleWinR = 3,
    kActionReloadSettings = 4
};

// Base dimensions at 96 DPI
constexpr int kBaseDlgWidth       = 720;
constexpr int kBaseRowHeight      = 46;
constexpr int kBaseListPadTop     = 10;
constexpr int kBaseListPadBottom  = 10;
constexpr int kBaseInputBarHeight = 66;
constexpr UINT_PTR kTimerCaret    = 9010;
constexpr int kModernEditId       = 9901;
constexpr int kNativeBrowseBtnId  = 12288;
constexpr size_t kMaxMruEntries   = 26;

int DpiScale(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi) * 88, 96 * 100);
}

float DpiScaleF(float value, UINT dpi) {
    return (value * 0.88f) * static_cast<float>(dpi) / 96.0f;
}

LocalizedStrings GetLocalizedStringsForLanguage(std::wstring lang) {
    if (lang == L"auto") {
        LANGID lid = GetUserDefaultUILanguage();
        WORD prim = PRIMARYLANGID(lid);
        WORD sub  = SUBLANGID(lid);
        switch (prim) {
            case LANG_VIETNAMESE: lang = L"vi"; break;
            case LANG_JAPANESE:   lang = L"ja"; break;
            case LANG_CHINESE:
                lang = (sub == SUBLANG_CHINESE_SIMPLIFIED || sub == SUBLANG_CHINESE_SINGAPORE)
                       ? L"zh_cn" : L"zh_tw";
                break;
            case LANG_KOREAN:     lang = L"ko"; break;
            case LANG_FRENCH:     lang = L"fr"; break;
            case LANG_GERMAN:     lang = L"de"; break;
            case LANG_SPANISH:    lang = L"es"; break;
            case LANG_RUSSIAN:    lang = L"ru"; break;
            default:              lang = L"en"; break;
        }
    }

    if (lang == L"vi") {
        return {
            L"Nhập tên lệnh hoặc chương trình để chạy",
            L"Duyệt",
            L"Chạy",
            L"Thoát",
            L"Hôm nay"
        };
    }
    if (lang == L"ja") {
        return {
            L"実行するコマンドの名前を入力してください",
            L"参照",
            L"実行",
            L"閉じる",
            L"今日"
        };
    }
    if (lang == L"zh_cn") {
        return {
            L"输入要运行的命令名称",
            L"浏览",
            L"运行",
            L"退出",
            L"今天"
        };
    }
    if (lang == L"zh_tw") {
        return {
            L"輸入要執行的命令名稱",
            L"瀏覽",
            L"執行",
            L"退出",
            L"今天"
        };
    }
    if (lang == L"ko") {
        return {
            L"실행할 명령의 이름을 입력하세요",
            L"찾아보기",
            L"실행",
            L"닫기",
            L"오늘"
        };
    }
    if (lang == L"fr") {
        return {
            L"Entrez le nom d'une commande à exécuter",
            L"Parcourir",
            L"Exécuter",
            L"Quitter",
            L"Aujourd'hui"
        };
    }
    if (lang == L"de") {
        return {
            L"Geben Sie den Namen eines Befehls ein",
            L"Durchsuchen",
            L"Ausführen",
            L"Schließen",
            L"Heute"
        };
    }
    if (lang == L"es") {
        return {
            L"Escriba el nombre de un comando para ejecutar",
            L"Examinar",
            L"Ejecutar",
            L"Salir",
            L"Hoy"
        };
    }
    if (lang == L"ru") {
        return {
            L"Введите имя команды для выполнения",
            L"Обзор",
            L"Выполнить",
            L"Выход",
            L"Сегодня"
        };
    }

    return {
        L"Type the name of a command to run",
        L"Browse",
        L"Run",
        L"Exit",
        L"Today"
    };
}

void LoadSettings() {
    auto langSetting = WindhawkUtils::StringSetting::make(L"language");
    auto tbAction = WindhawkUtils::StringSetting::make(L"taskbarClickAction");

    ModSettings newSettings;
    newSettings.language = langSetting.get()[0] ? langSetting.get() : L"auto";
    newSettings.taskbarCloseInsteadOfMinimize = (wcscmp(tbAction.get(), L"close") == 0);
    newSettings.enableBlur = Wh_GetIntSetting(L"enableBlur") != 0;
    newSettings.blurOpacity = std::clamp(Wh_GetIntSetting(L"blurOpacity"), 0, 255);
    newSettings.maxHistoryItems = std::clamp(Wh_GetIntSetting(L"maxHistoryItems"), 0, 5);
    newSettings.showTimestamp = Wh_GetIntSetting(L"showTimestamp") != 0;
    newSettings.clearInitialInput = Wh_GetIntSetting(L"clearInitialInput") != 0;
    newSettings.closeOnClickOutside = Wh_GetIntSetting(L"closeOnClickOutside") != 0;

    LocalizedStrings newI18n = GetLocalizedStringsForLanguage(newSettings.language);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(newSettings);
    g_i18n = newI18n;
}

struct HistoryItem {
    std::wstring cmd;
    std::wstring appName;
    std::wstring timeStr;
    HICON hIcon = nullptr;
};

struct RunDialogState {
    HWND hDlg = nullptr;
    HWND hCombo = nullptr;
    HWND hOrigEdit = nullptr;
    HWND hModernEdit = nullptr;
    HWND hOkBtn = nullptr;
    HWND hBrowseBtn = nullptr;

    HFONT hFontEditCtl = nullptr;
    HICON hRunIcon = nullptr;
    std::vector<HistoryItem> history;

    ModSettings settings;
    LocalizedStrings i18n = {};

    UINT dpi = 96;
    int topAreaHeight = 0;
    int totalHeight = kBaseInputBarHeight;
    int normalX = 100;
    int normalY = 100;
    int hoveredRow = -1;
    int selectedRow = -1;
    bool browseHovered = false;
    bool browsePressed = false;
    bool btnHovered = false;
    bool btnPressed = false;
    bool closeHovered = false;
    bool closePressed = false;
    bool caretVisible = true;
    bool isTriggeringRun = false;
    bool isBrowsingFile = false;
    bool pendingUninit = false;
};

// Atomic state shared safely across threads
std::atomic<HWND> g_hActiveRunDlg{nullptr};
std::atomic<ULONGLONG> g_openTick{0};
std::atomic<ULONGLONG> g_lastToggleTick{0};
std::atomic<bool> g_winRReleasedSinceOpen{false};
std::atomic<bool> g_isBrowsingFile{false};

// Dedicated thread for WH_KEYBOARD_LL so the Run dialog thread never stalls global keyboard input.
// Wrapped in [[clang::no_destroy]] std::optional so process exit with an open dialog won't call std::terminate.
std::mutex g_keyHookMutex;
std::condition_variable g_keyHookCv;
[[clang::no_destroy]] std::optional<std::thread> g_keyHookThread;
DWORD g_keyHookThreadId = 0;
bool g_keyHookThreadReady = false;

LRESULT CALLBACK LowLevelKeyProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        HWND hDlg = g_hActiveRunDlg.load();
        if (hDlg && IsWindow(hDlg)) {
            auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
            bool isKeyUp   = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

            if (kbd->vkCode == 'R' && isKeyUp) {
                g_winRReleasedSinceOpen.store(true);
            }

            if (isKeyDown && kbd->vkCode == 'R') {
                bool winDown = (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);
                if (winDown && (g_winRReleasedSinceOpen.load() || (GetTickCount64() - g_openTick.load() > 350))) {
                    PostMessageW(hDlg, g_msgToggle, kActionToggleWinR, 0);
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void StartKeyHookThread() {
    std::unique_lock<std::mutex> lock(g_keyHookMutex);
    if (g_keyHookThread && g_keyHookThread->joinable()) {
        return;
    }

    g_keyHookThreadReady = false;
    g_keyHookThreadId = 0;

    g_keyHookThread.emplace([]() {
        MSG msg = {};
        PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

        HHOOK hHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyProc, GetModuleHandleW(nullptr), 0);

        {
            std::lock_guard<std::mutex> lk(g_keyHookMutex);
            g_keyHookThreadId = GetCurrentThreadId();
            g_keyHookThreadReady = true;
        }
        g_keyHookCv.notify_all();

        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (hHook) {
            UnhookWindowsHookEx(hHook);
        }
    });

    g_keyHookCv.wait(lock, []() { return g_keyHookThreadReady; });
}

void StopKeyHookThread() {
    std::thread threadToJoin;
    {
        std::lock_guard<std::mutex> lock(g_keyHookMutex);
        if (!g_keyHookThread || !g_keyHookThread->joinable()) {
            return;
        }
        if (g_keyHookThreadId != 0) {
            PostThreadMessageW(g_keyHookThreadId, WM_QUIT, 0, 0);
        }
        threadToJoin = std::move(*g_keyHookThread);
        g_keyHookThread.reset();
        g_keyHookThreadId = 0;
        g_keyHookThreadReady = false;
    }
    if (threadToJoin.joinable()) {
        threadToJoin.join();
    }
}

void ApplyWindowBlur(HWND hDlg, const RunDialogState* state);
void CloseRunDialog(HWND hDlg);
void SyncInputToNativeCombo(RunDialogState* state, const wchar_t* textOverride = nullptr);
void PerformUninitCleanup(HWND hWnd, RunDialogState* state);

int GetDlgWidth(const RunDialogState* state) {
    return DpiScale(kBaseDlgWidth, state->dpi);
}

int GetRowHeight(const RunDialogState* state) {
    return DpiScale(kBaseRowHeight, state->dpi);
}

int GetListPadTop(const RunDialogState* state) {
    return DpiScale(kBaseListPadTop, state->dpi);
}

int GetListPadBottom(const RunDialogState* state) {
    return DpiScale(kBaseListPadBottom, state->dpi);
}

int GetInputBarHeight(const RunDialogState* state) {
    return DpiScale(kBaseInputBarHeight, state->dpi);
}

void ClampDialogToMonitorWorkArea(HWND hDlg, int* inOutX, int* inOutY, int width, int height, UINT dpi) {
    HMONITOR hMon = MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    RECT rcWork = {};
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        rcWork = mi.rcWork;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    }

    int marginTrigger = DpiScale(12, dpi);
    int marginTarget  = DpiScale(16, dpi);

    if (*inOutY + height > rcWork.bottom - marginTrigger) {
        *inOutY = rcWork.bottom - height - marginTarget;
    }
    if (*inOutY < rcWork.top + marginTrigger) {
        *inOutY = rcWork.top + marginTarget;
    }
    if (*inOutX + width > rcWork.right - marginTrigger) {
        *inOutX = rcWork.right - width - marginTarget;
    }
    if (*inOutX < rcWork.left + marginTrigger) {
        *inOutX = rcWork.left + marginTarget;
    }
}

// Minimize the Run dialog to the Taskbar while keeping its Taskbar button active
void MinimizeRunDialog(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    ULONGLONG now = GetTickCount64();
    if (now - g_lastToggleTick.load() < 250) return;

    if (state->settings.taskbarCloseInsteadOfMinimize) {
        CloseRunDialog(state->hDlg);
        return;
    }

    g_lastToggleTick.store(now);

    if (!IsIconic(state->hDlg)) {
        RECT rc = {};
        GetWindowRect(state->hDlg, &rc);
        if (rc.left > -10000 && rc.top > -10000) {
            state->normalX = rc.left;
            state->normalY = rc.top;
        }
    }

    ShowWindow_Orig(state->hDlg, SW_MINIMIZE);
}

// Restore the Run dialog from the Taskbar to its proper position and DPI-scaled size
void RestoreRunDialog(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    ULONGLONG now = GetTickCount64();
    if (now - g_lastToggleTick.load() < 250) return;
    g_lastToggleTick.store(now);

    ShowWindow_Orig(state->hDlg, SW_RESTORE);

    // Ensure WS_CAPTION remains stripped after restore
    LONG_PTR style = GetWindowLongPtrW(state->hDlg, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
    style |= (WS_SYSMENU | WS_MINIMIZEBOX);
    SetWindowLongPtrW(state->hDlg, GWL_STYLE, style);

    int dlgW = GetDlgWidth(state);
    SetWindowPos(state->hDlg, HWND_TOP,
                 state->normalX, state->normalY,
                 dlgW, state->totalHeight,
                 SWP_SHOWWINDOW | SWP_FRAMECHANGED);

    ApplyWindowBlur(state->hDlg, state);
    SetForegroundWindow_Orig(state->hDlg);

    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
        SetFocus(state->hModernEdit);
    }
    state->caretVisible = true;
    InvalidateRect(state->hDlg, nullptr, TRUE);
}

// Trigger the native Browse button (ID 12288) and sync the resulting path to the modern input box
void OpenBrowseExecutableDialog(RunDialogState* state) {
    if (!state || !state->hDlg || state->isBrowsingFile) return;
    HWND hDlg = state->hDlg;
    HWND hBrowse = state->hBrowseBtn ? state->hBrowseBtn : GetDlgItem(hDlg, kNativeBrowseBtnId);
    if (!hBrowse) return;

    state->isBrowsingFile = true;
    g_isBrowsingFile.store(true);

    EnableWindow(hBrowse, TRUE);
    SendMessageW(hDlg, WM_COMMAND, MAKEWPARAM(kNativeBrowseBtnId, BN_CLICKED), (LPARAM)hBrowse);

    if (!IsWindow(hDlg) || g_hActiveRunDlg.load() != hDlg) {
        return;
    }

    state->isBrowsingFile = false;
    g_isBrowsingFile.store(false);

    if (state->pendingUninit) {
        PerformUninitCleanup(hDlg, state);
        return;
    }

    if (state->hCombo && state->hModernEdit && IsWindow(state->hModernEdit)) {
        wchar_t filePath[MAX_PATH] = {};
        GetWindowTextW(state->hCombo, filePath, MAX_PATH);
        if (filePath[0] != L'\0') {
            SetWindowTextW(state->hModernEdit, filePath);
            size_t len = wcslen(filePath);
            SendMessageW(state->hModernEdit, EM_SETSEL, len, len);
            SyncInputToNativeCombo(state, filePath);
            state->selectedRow = -1;
        }
    }

    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
        SetFocus(state->hModernEdit);
    }
    state->caretVisible = true;
    InvalidateRect(hDlg, nullptr, TRUE);
}

// --- Command Timestamp Management in Windhawk Mod Storage ---

std::wstring NormalizeCmdKey(const std::wstring& cmd) {
    wchar_t buf[MAX_PATH] = {};
    wcsncpy_s(buf, cmd.c_str(), _TRUNCATE);
    PathRemoveBlanksW(buf);
    std::wstring res = buf;
    for (wchar_t& ch : res) {
        if (ch == L'\t' || ch == L'\r' || ch == L'\n') {
            ch = L' ';
        } else {
            ch = towlower(ch);
        }
    }
    return res;
}

struct StoredTimestampEntry {
    ULONGLONG fileTime = 0;
    std::wstring cmdKey;
};

std::vector<StoredTimestampEntry> LoadStoredTimestamps() {
    std::vector<StoredTimestampEntry> entries;
    std::vector<wchar_t> buffer(8192, L'\0');
    size_t charsRead = Wh_GetStringValue(L"HistoryTimestamps", buffer.data(), buffer.size());
    if (charsRead == 0 || buffer[0] == L'\0') {
        return entries;
    }

    std::wstring data(buffer.data());
    size_t pos = 0;
    while (pos < data.size()) {
        size_t endLine = data.find(L'\n', pos);
        if (endLine == std::wstring::npos) {
            endLine = data.size();
        }
        std::wstring line = data.substr(pos, endLine - pos);
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }
        size_t tabPos = line.find(L'\t');
        if (tabPos != std::wstring::npos && tabPos > 0 && tabPos + 1 < line.size()) {
            wchar_t* endPtr = nullptr;
            ULONGLONG ftVal = wcstoull(line.substr(0, tabPos).c_str(), &endPtr, 10);
            std::wstring key = line.substr(tabPos + 1);
            if (ftVal != 0 && !key.empty()) {
                entries.push_back({ ftVal, std::move(key) });
            }
        }
        pos = endLine + 1;
    }
    return entries;
}

void SaveStoredTimestamps(const std::vector<StoredTimestampEntry>& entries) {
    std::wstring serialized;
    size_t count = (std::min)(entries.size(), kMaxMruEntries);
    for (size_t i = 0; i < count; ++i) {
        wchar_t numBuf[32] = {};
        swprintf_s(numBuf, L"%llu", entries[i].fileTime);
        serialized += numBuf;
        serialized += L'\t';
        serialized += entries[i].cmdKey;
        serialized += L'\n';
    }
    Wh_SetStringValue(L"HistoryTimestamps", serialized.c_str());
}

void SaveCommandTimestamp(const std::wstring& cmd) {
    std::wstring keyName = NormalizeCmdKey(cmd);
    if (keyName.empty()) return;

    FILETIME ft = {};
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER uli = {};
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    auto entries = LoadStoredTimestamps();
    entries.erase(
        std::remove_if(entries.begin(), entries.end(),
                       [&](const StoredTimestampEntry& e) { return e.cmdKey == keyName; }),
        entries.end());
    entries.insert(entries.begin(), { uli.QuadPart, keyName });
    if (entries.size() > kMaxMruEntries) {
        entries.resize(kMaxMruEntries);
    }
    SaveStoredTimestamps(entries);
}

std::wstring FormatHistoryTimestamp(const FILETIME& ftUtc, const wchar_t* todayPrefix) {
    FILETIME ftLocal = {};
    SYSTEMTIME st = {}, now = {};
    FileTimeToLocalFileTime(&ftUtc, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &st);
    GetLocalTime(&now);

    wchar_t buf[64] = {};
    if (st.wYear == now.wYear && st.wMonth == now.wMonth && st.wDay == now.wDay) {
        swprintf_s(buf, L"%s, %02d:%02d", todayPrefix, st.wHour, st.wMinute);
    } else {
        swprintf_s(buf, L"%02d:%02d  %02d/%02d", st.wHour, st.wMinute, st.wDay, st.wMonth);
    }
    return buf;
}

std::wstring GetOrInitCommandTimestamp(const std::wstring& cmd,
                                       int rankIndex,
                                       const FILETIME& fallbackMruTime,
                                       const wchar_t* todayPrefix,
                                       std::vector<StoredTimestampEntry>& entries,
                                       bool* outModified) {
    std::wstring keyName = NormalizeCmdKey(cmd);
    if (keyName.empty()) return L"";

    for (const auto& entry : entries) {
        if (entry.cmdKey == keyName && entry.fileTime != 0) {
            ULARGE_INTEGER uli = {};
            uli.QuadPart = entry.fileTime;
            FILETIME ft = { uli.LowPart, uli.HighPart };
            return FormatHistoryTimestamp(ft, todayPrefix);
        }
    }

    FILETIME ft = {};
    ULARGE_INTEGER uli = {};
    uli.LowPart  = fallbackMruTime.dwLowDateTime;
    uli.HighPart = fallbackMruTime.dwHighDateTime;
    if (uli.QuadPart == 0) {
        GetSystemTimeAsFileTime(&ft);
        uli.LowPart  = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;
    }
    if (rankIndex > 0) {
        ULONGLONG offset = static_cast<ULONGLONG>(rankIndex) * 180ULL * 10000000ULL;
        if (uli.QuadPart > offset) {
            uli.QuadPart -= offset;
        }
    }
    ft.dwLowDateTime  = uli.LowPart;
    ft.dwHighDateTime = uli.HighPart;

    if (entries.size() < kMaxMruEntries) {
        entries.push_back({ uli.QuadPart, keyName });
        if (outModified) {
            *outModified = true;
        }
    }

    return FormatHistoryTimestamp(ft, todayPrefix);
}

void CloseRunDialog(HWND hDlg) {
    if (!hDlg || !IsWindow(hDlg)) return;
    HWND hCancel = GetDlgItem(hDlg, IDCANCEL);
    if (hCancel) EnableWindow(hCancel, TRUE);
    PostMessageW(hDlg, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)hCancel);
}

std::wstring GetEffectiveInputText(HWND hEdit) {
    if (!hEdit || !IsWindow(hEdit)) return L"";

    wchar_t buf[MAX_PATH] = {};
    int len = GetWindowTextW(hEdit, buf, MAX_PATH);
    std::wstring text(buf, len > 0 ? len : 0);

    HIMC hImc = ImmGetContext(hEdit);
    if (hImc) {
        LONG compBytes = ImmGetCompositionStringW(hImc, GCS_COMPSTR, nullptr, 0);
        if (compBytes > 0 && compBytes < MAX_PATH * (LONG)sizeof(wchar_t)) {
            std::wstring compStr(compBytes / sizeof(wchar_t), L'\0');
            ImmGetCompositionStringW(hImc, GCS_COMPSTR, compStr.data(), compBytes);
            if (!compStr.empty() && text.empty()) {
                text = compStr;
            }
        }
        ImmReleaseContext(hEdit, hImc);
    }
    return text;
}

void SyncInputToNativeCombo(RunDialogState* state, const wchar_t* textOverride) {
    if (!state || !state->hCombo) return;

    std::wstring currentText;
    if (textOverride) {
        currentText = textOverride;
    } else if (state->hModernEdit) {
        currentText = GetEffectiveInputText(state->hModernEdit);
    }

    SendMessageW(state->hCombo, CB_SETCURSEL, (WPARAM)-1, 0);
    SetWindowTextW(state->hCombo, currentText.c_str());
    if (state->hOrigEdit) {
        SetWindowTextW(state->hOrigEdit, currentText.c_str());
        SendMessageW(state->hOrigEdit, EM_SETMODIFY, TRUE, 0);
    }
}

void TriggerRunCommand(RunDialogState* state) {
    if (!state || !state->hDlg || state->isTriggeringRun) return;
    state->isTriggeringRun = true;

    if (state->hModernEdit) {
        HIMC hImc = ImmGetContext(state->hModernEdit);
        if (hImc) {
            ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
            ImmReleaseContext(state->hModernEdit, hImc);
        }
    }

    std::wstring effectiveText = GetEffectiveInputText(state->hModernEdit);
    wchar_t rawBuf[MAX_PATH] = {};
    wcsncpy_s(rawBuf, effectiveText.c_str(), _TRUNCATE);
    PathRemoveBlanksW(rawBuf);

    if (rawBuf[0] == L'\0') {
        state->isTriggeringRun = false;
        return;
    }

    SaveCommandTimestamp(rawBuf);

    SyncInputToNativeCombo(state, rawBuf);
    if (state->hCombo) {
        int comboId = GetDlgCtrlID(state->hCombo);
        SendMessageW(state->hDlg, WM_COMMAND, MAKEWPARAM(comboId, CBN_EDITCHANGE), (LPARAM)state->hCombo);
        SendMessageW(state->hDlg, WM_COMMAND, MAKEWPARAM(comboId, CBN_EDITUPDATE), (LPARAM)state->hCombo);
    }

    HWND hDlg = state->hDlg;
    HWND hOk = state->hOkBtn ? state->hOkBtn : GetDlgItem(hDlg, IDOK);
    if (hOk) EnableWindow(hOk, TRUE);

    // Let the native OK handler execute the command, record history, or display errors
    SendMessageW(hDlg, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)hOk);

    if (g_hActiveRunDlg.load() == hDlg && IsWindow(hDlg)) {
        state->isTriggeringRun = false;
        if (state->pendingUninit) {
            PerformUninitCleanup(hDlg, state);
            return;
        }
    }
}

void ApplyWindowBlur(HWND hDlg, const RunDialogState* state) {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32 || !state) return;

    auto pSetWCA = reinterpret_cast<SetWindowCompositionAttribute_t>(
        GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
    if (!pSetWCA) return;

    ACCENT_POLICY policy = {};
    if (state->settings.enableBlur) {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        DWORD alpha = static_cast<DWORD>(state->settings.blurOpacity) & 0xFF;
        policy.GradientColor = (alpha << 24) | 0x00221C1E;
    } else {
        policy.AccentState = ACCENT_DISABLED;
    }

    WINDOWCOMPOSITIONATTRIBDATA data = {};
    data.Attrib = 19;
    data.pvData = &policy;
    data.cbData = sizeof(policy);
    pSetWCA(hDlg, &data);
}

std::wstring GetFriendlyBuiltinName(const std::wstring& baseLower) {
    struct BuiltinMap { const wchar_t* key; const wchar_t* name; };
    static const BuiltinMap kMap[] = {
        { L"winver",       L"Windows Version" },
        { L"cmd",          L"Command Prompt" },
        { L"powershell",   L"Windows PowerShell" },
        { L"pwsh",         L"PowerShell" },
        { L"wt",           L"Windows Terminal" },
        { L"notepad",      L"Notepad" },
        { L"regedit",      L"Registry Editor" },
        { L"calc",         L"Calculator" },
        { L"mspaint",      L"Paint" },
        { L"taskmgr",      L"Task Manager" },
        { L"msconfig",     L"System Configuration" },
        { L"control",      L"Control Panel" },
        { L"explorer",     L"File Explorer" },
        { L"mstsc",        L"Remote Desktop Connection" },
        { L"cleanmgr",     L"Disk Cleanup" },
        { L"dxdiag",       L"DirectX Diagnostic Tool" },
        { L"charmap",      L"Character Map" },
        { L"magnify",      L"Magnifier" },
        { L"osk",          L"On-Screen Keyboard" },
        { L"snippingtool", L"Snipping Tool" },
        { L"winword",      L"Microsoft Word" },
        { L"excel",        L"Microsoft Excel" },
        { L"powerpnt",     L"Microsoft PowerPoint" },
        { L"msedge",       L"Microsoft Edge" },
        { L"chrome",       L"Google Chrome" },
        { L"code",         L"Visual Studio Code" },
        { L"sysdm.cpl",    L"System Properties" },
        { L"main.cpl",     L"Mouse Properties" },
        { L"ncpa.cpl",     L"Network Connections" },
        { L"appwiz.cpl",   L"Programs and Features" },
        { L"mmsys.cpl",    L"Sound" },
        { L"timedate.cpl", L"Date and Time" },
        { L"desk.cpl",     L"Display Settings" },
        { L"inetcpl.cpl",  L"Internet Properties" },
        { L"powercfg.cpl", L"Power Options" },
        { L"firewall.cpl", L"Windows Defender Firewall" },
        { L"colorcpl",     L"Color Management" },
        { L"services.msc", L"Services" },
        { L"devmgmt.msc",  L"Device Manager" },
        { L"diskmgmt.msc", L"Disk Management" },
        { L"eventvwr.msc", L"Event Viewer" },
        { L"gpedit.msc",   L"Local Group Policy Editor" },
        { L"lusrmgr.msc",  L"Local Users and Groups" },
        { L"perfmon.msc",  L"Performance Monitor" },
        { L"resmon",       L"Resource Monitor" },
        { L"secpol.msc",   L"Local Security Policy" },
        { L"taskschd.msc", L"Task Scheduler" }
    };

    for (const auto& entry : kMap) {
        if (baseLower == entry.key) return entry.name;
    }
    if (baseLower.size() > 4 && baseLower.substr(baseLower.size() - 4) == L".exe") {
        std::wstring noExe = baseLower.substr(0, baseLower.size() - 4);
        for (const auto& entry : kMap) {
            if (noExe == entry.key) return entry.name;
        }
    }
    return L"";
}

std::wstring GetExecutableDescription(const wchar_t* filePath) {
    DWORD handle = 0;
    DWORD verSize = GetFileVersionInfoSizeW(filePath, &handle);
    if (verSize == 0) return L"";

    std::vector<BYTE> verData(verSize);
    if (!GetFileVersionInfoW(filePath, handle, verSize, verData.data())) return L"";

    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate = nullptr;
    UINT cbTranslate = 0;

    if (VerQueryValueW(verData.data(), L"\\VarFileInfo\\Translation",
                       (LPVOID*)&lpTranslate, &cbTranslate) &&
        cbTranslate >= sizeof(LANGANDCODEPAGE)) {
        const wchar_t* keys[] = { L"FileDescription", L"ProductName" };
        for (const wchar_t* k : keys) {
            wchar_t subBlock[128] = {};
            swprintf_s(subBlock, L"\\StringFileInfo\\%04x%04x\\%s",
                       lpTranslate[0].wLanguage, lpTranslate[0].wCodePage, k);
            wchar_t* val = nullptr;
            UINT len = 0;
            if (VerQueryValueW(verData.data(), subBlock, (LPVOID*)&val, &len) && val && len > 1) {
                std::wstring desc(val);
                if (!desc.empty()) return desc;
            }
        }
    }
    return L"";
}

void ResolveCommandMetadata(const std::wstring& rawCmd, HICON* outIcon, std::wstring* outAppName) {
    *outIcon = nullptr;
    outAppName->clear();

    wchar_t cmdOnly[MAX_PATH] = {};
    wcsncpy_s(cmdOnly, rawCmd.c_str(), _TRUNCATE);
    PathRemoveBlanksW(cmdOnly);
    PathRemoveArgsW(cmdOnly);
    PathUnquoteSpacesW(cmdOnly);

    if (cmdOnly[0] == L'\0') return;

    std::wstring baseName = PathFindFileNameW(cmdOnly);
    std::wstring baseLower = baseName;
    std::transform(baseLower.begin(), baseLower.end(), baseLower.begin(), ::towlower);

    std::wstring builtinName = GetFriendlyBuiltinName(baseLower);

    wchar_t foundPath[MAX_PATH] = {};
    bool found = false;

    if (SearchPathW(nullptr, cmdOnly, L".exe", MAX_PATH, foundPath, nullptr) ||
        SearchPathW(nullptr, cmdOnly, L".cpl", MAX_PATH, foundPath, nullptr) ||
        SearchPathW(nullptr, cmdOnly, L".msc", MAX_PATH, foundPath, nullptr) ||
        SearchPathW(nullptr, cmdOnly, nullptr, MAX_PATH, foundPath, nullptr)) {
        found = true;
    } else {
        std::wstring regSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\";
        regSubKey += baseName;
        if (wcschr(baseName.c_str(), L'.') == nullptr) regSubKey += L".exe";

        DWORD cbData = sizeof(foundPath);
        if (RegGetValueW(HKEY_LOCAL_MACHINE, regSubKey.c_str(), nullptr, RRF_RT_REG_SZ,
                         nullptr, foundPath, &cbData) == ERROR_SUCCESS ||
            RegGetValueW(HKEY_CURRENT_USER, regSubKey.c_str(), nullptr, RRF_RT_REG_SZ,
                         nullptr, foundPath, &cbData) == ERROR_SUCCESS) {
            PathUnquoteSpacesW(foundPath);
            found = true;
        }
    }

    if (found) {
        HICON hLarge = nullptr;
        HICON hSmall = nullptr;
        if (ExtractIconExW(foundPath, 0, &hLarge, &hSmall, 1) > 0) {
            if (hLarge) {
                *outIcon = hLarge;
                if (hSmall) DestroyIcon(hSmall);
            } else if (hSmall) {
                *outIcon = hSmall;
            }
        }
        if (!*outIcon) {
            SHFILEINFOW sfi = {};
            if (SHGetFileInfoW(foundPath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) {
                *outIcon = sfi.hIcon;
            }
        }

        if (!builtinName.empty()) {
            *outAppName = builtinName;
        } else {
            std::wstring verDesc = GetExecutableDescription(foundPath);
            if (!verDesc.empty()) {
                *outAppName = verDesc;
            } else {
                SHFILEINFOW sfi = {};
                if (SHGetFileInfoW(foundPath, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME) && sfi.szDisplayName[0]) {
                    *outAppName = sfi.szDisplayName;
                }
            }
        }
    } else {
        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW(L".exe", FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),
                           SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_LARGEICON)) {
            *outIcon = sfi.hIcon;
        }
        if (!builtinName.empty()) {
            *outAppName = builtinName;
        } else if (PathIsDirectoryW(cmdOnly)) {
            *outAppName = L"File Explorer";
        } else {
            std::wstring cap = baseName;
            if (!cap.empty()) cap[0] = towupper(cap[0]);
            *outAppName = cap;
        }
    }
}

void LoadHistoryItems(RunDialogState* state) {
    for (auto& item : state->history) {
        if (item.hIcon) DestroyIcon(item.hIcon);
    }
    state->history.clear();

    int maxItems = state->settings.maxHistoryItems;
    if (maxItems <= 0) return;

    FILETIME ftRunMruWrite = {};
    HKEY hMruKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU",
                      0, KEY_READ, &hMruKey) == ERROR_SUCCESS) {
        RegQueryInfoKeyW(hMruKey, nullptr, nullptr, nullptr, nullptr, nullptr,
                         nullptr, nullptr, nullptr, nullptr, nullptr, &ftRunMruWrite);
        RegCloseKey(hMruKey);
    }
    if (ftRunMruWrite.dwLowDateTime == 0 && ftRunMruWrite.dwHighDateTime == 0) {
        GetSystemTimeAsFileTime(&ftRunMruWrite);
    }

    auto storedTimestamps = LoadStoredTimestamps();
    bool timestampsModified = false;

    if (state->hCombo) {
        int count = (int)SendMessageW(state->hCombo, CB_GETCOUNT, 0, 0);
        for (int i = 0; i < count && (int)state->history.size() < maxItems; ++i) {
            int len = (int)SendMessageW(state->hCombo, CB_GETLBTEXTLEN, i, 0);
            if (len > 0 && len < MAX_PATH) {
                std::wstring text(len + 1, L'\0');
                SendMessageW(state->hCombo, CB_GETLBTEXT, i, (LPARAM)text.data());
                text.resize(wcslen(text.c_str()));
                if (!text.empty()) {
                    HistoryItem item;
                    item.cmd = text;
                    ResolveCommandMetadata(text, &item.hIcon, &item.appName);
                    item.timeStr = GetOrInitCommandTimestamp(
                        text, (int)state->history.size(), ftRunMruWrite,
                        state->i18n.todayPrefix, storedTimestamps, &timestampsModified);
                    state->history.push_back(std::move(item));
                }
            }
        }
    }

    if (state->history.empty()) {
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU",
                          0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t mruList[64] = {};
            DWORD cbList = sizeof(mruList);
            if (RegGetValueW(hKey, nullptr, L"MRUList", RRF_RT_REG_SZ, nullptr, mruList, &cbList) == ERROR_SUCCESS) {
                for (int i = 0; mruList[i] != L'\0' && (int)state->history.size() < maxItems; ++i) {
                    wchar_t valName[2] = { mruList[i], L'\0' };
                    wchar_t cmdBuf[MAX_PATH] = {};
                    DWORD cbCmd = sizeof(cmdBuf);
                    if (RegGetValueW(hKey, nullptr, valName, RRF_RT_REG_SZ, nullptr, cmdBuf, &cbCmd) == ERROR_SUCCESS) {
                        size_t len = wcslen(cmdBuf);
                        if (len >= 2 && cmdBuf[len - 2] == L'\\' && cmdBuf[len - 1] == L'1') {
                            cmdBuf[len - 2] = L'\0';
                        }
                        if (cmdBuf[0] != L'\0') {
                            HistoryItem item;
                            item.cmd = cmdBuf;
                            ResolveCommandMetadata(cmdBuf, &item.hIcon, &item.appName);
                            item.timeStr = GetOrInitCommandTimestamp(
                                cmdBuf, (int)state->history.size(), ftRunMruWrite,
                                state->i18n.todayPrefix, storedTimestamps, &timestampsModified);
                            state->history.push_back(std::move(item));
                        }
                    }
                }
            }
            RegCloseKey(hKey);
        }
    }

    if (timestampsModified) {
        SaveStoredTimestamps(storedTimestamps);
    }
}

// DPI-scaled layout rectangles for the three action buttons and input box
RECT GetBrowseButtonRect(const RunDialogState* state) {
    int dlgW = GetDlgWidth(state);
    int barH = GetInputBarHeight(state);
    int btnH = DpiScale(36, state->dpi);
    int top  = state->topAreaHeight + (barH - btnH) / 2;
    RECT rc  = { dlgW - DpiScale(332, state->dpi), top, dlgW - DpiScale(226, state->dpi), top + btnH };
    return rc;
}

RECT GetRunButtonRect(const RunDialogState* state) {
    int dlgW = GetDlgWidth(state);
    int barH = GetInputBarHeight(state);
    int btnH = DpiScale(36, state->dpi);
    int top  = state->topAreaHeight + (barH - btnH) / 2;
    RECT rc  = { dlgW - DpiScale(218, state->dpi), top, dlgW - DpiScale(118, state->dpi), top + btnH };
    return rc;
}

RECT GetCloseButtonRect(const RunDialogState* state) {
    int dlgW = GetDlgWidth(state);
    int barH = GetInputBarHeight(state);
    int btnH = DpiScale(36, state->dpi);
    int top  = state->topAreaHeight + (barH - btnH) / 2;
    RECT rc  = { dlgW - DpiScale(110, state->dpi), top, dlgW - DpiScale(14, state->dpi), top + btnH };
    return rc;
}

RECT GetModernEditRect(const RunDialogState* state) {
    int dlgW  = GetDlgWidth(state);
    int barH  = GetInputBarHeight(state);
    int editH = DpiScale(28, state->dpi);
    int top   = state->topAreaHeight + (barH - editH) / 2;
    RECT rc   = { DpiScale(60, state->dpi), top, dlgW - DpiScale(344, state->dpi), top + editH };
    return rc;
}

int HitTestHistoryRow(const RunDialogState* state, POINT pt) {
    int padTop = GetListPadTop(state);
    int padBot = GetListPadBottom(state);
    int rowH   = GetRowHeight(state);
    int dlgW   = GetDlgWidth(state);
    int padX   = DpiScale(10, state->dpi);

    if (state->history.empty() || pt.y < padTop || pt.y >= state->topAreaHeight - padBot) {
        return -1;
    }
    if (pt.x < padX || pt.x > dlgW - padX) return -1;
    int idx = (pt.y - padTop) / rowH;
    if (idx >= 0 && idx < (int)state->history.size()) {
        return idx;
    }
    return -1;
}

// --- GDI+ 32-bit Vector & Drawing Helpers ---

void AddRoundedRectToPath(Gdiplus::GraphicsPath& path, Gdiplus::RectF rc, float radius) {
    float d = radius * 2.0f;
    path.AddArc(rc.X, rc.Y, d, d, 180.0f, 90.0f);
    path.AddArc(rc.X + rc.Width - d, rc.Y, d, d, 270.0f, 90.0f);
    path.AddArc(rc.X + rc.Width - d, rc.Y + rc.Height - d, d, d, 0.0f, 90.0f);
    path.AddArc(rc.X, rc.Y + rc.Height - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

void FillAndStrokeRoundRect(Gdiplus::Graphics& g, Gdiplus::RectF rc, float radius,
                            Gdiplus::Color fillCol, Gdiplus::Color borderCol, float borderWidth = 1.0f) {
    Gdiplus::GraphicsPath path;
    AddRoundedRectToPath(path, rc, radius);
    Gdiplus::SolidBrush br(fillCol);
    g.FillPath(&br, &path);
    if (borderCol.GetAlpha() > 0 && borderWidth > 0.0f) {
        Gdiplus::Pen pen(borderCol, borderWidth);
        g.DrawPath(&pen, &path);
    }
}

void DrawHistoryClockVector(Gdiplus::Graphics& g, float cx, float cy, UINT dpi, Gdiplus::Color col) {
    Gdiplus::Pen pen(col, DpiScaleF(1.5f, dpi));
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);
    pen.SetLineJoin(Gdiplus::LineJoinRound);

    float r = DpiScaleF(7.5f, dpi);
    g.DrawEllipse(&pen, cx - r, cy - r, r * 2.0f, r * 2.0f);
    g.DrawLine(&pen, cx, cy - DpiScaleF(4.2f, dpi), cx, cy + DpiScaleF(0.5f, dpi));
    g.DrawLine(&pen, cx, cy + DpiScaleF(0.5f, dpi), cx + DpiScaleF(3.5f, dpi), cy + DpiScaleF(2.5f, dpi));
}

void DrawFolderBrowseVector(Gdiplus::Graphics& g, float cx, float cy, UINT dpi, Gdiplus::Color col) {
    Gdiplus::Pen pen(col, DpiScaleF(1.5f, dpi));
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);
    pen.SetLineJoin(Gdiplus::LineJoinRound);

    float left   = cx - DpiScaleF(6.5f, dpi);
    float right  = cx + DpiScaleF(6.5f, dpi);
    float top    = cy - DpiScaleF(4.5f, dpi);
    float bottom = cy + DpiScaleF(5.0f, dpi);
    float tabH   = DpiScaleF(2.0f, dpi);

    // Folder body
    g.DrawLine(&pen, left, top + tabH, right, top + tabH);
    g.DrawLine(&pen, right, top + tabH, right, bottom);
    g.DrawLine(&pen, right, bottom, left, bottom);
    g.DrawLine(&pen, left, bottom, left, top);
    // Top folder tab
    g.DrawLine(&pen, left, top, left + DpiScaleF(4.5f, dpi), top);
    g.DrawLine(&pen, left + DpiScaleF(4.5f, dpi), top, left + DpiScaleF(6.2f, dpi), top + tabH);
}

void DrawEnterArrowVector(Gdiplus::Graphics& g, float cx, float cy, UINT dpi, Gdiplus::Color col) {
    Gdiplus::Pen pen(col, DpiScaleF(1.7f, dpi));
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);
    pen.SetLineJoin(Gdiplus::LineJoinRound);

    float dx = DpiScaleF(4.5f, dpi);
    float y0 = cy - DpiScaleF(4.5f, dpi);
    float y1 = cy + DpiScaleF(2.0f, dpi);
    float ax = cx - DpiScaleF(1.5f, dpi);

    g.DrawLine(&pen, cx + dx, y0, cx + dx, y1);
    g.DrawLine(&pen, cx + dx, y1, cx - dx, y1);
    g.DrawLine(&pen, cx - dx, y1, ax, cy - DpiScaleF(1.0f, dpi));
    g.DrawLine(&pen, cx - dx, y1, ax, cy + DpiScaleF(5.0f, dpi));
}

void DrawCloseCrossVector(Gdiplus::Graphics& g, float cx, float cy, UINT dpi, Gdiplus::Color col) {
    Gdiplus::Pen pen(col, DpiScaleF(1.7f, dpi));
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);

    float s = DpiScaleF(4.2f, dpi);
    g.DrawLine(&pen, cx - s, cy - s, cx + s, cy + s);
    g.DrawLine(&pen, cx + s, cy - s, cx - s, cy + s);
}

float MeasureGdiPlusStringWidth(Gdiplus::Graphics& g, const wchar_t* text, int len, const Gdiplus::Font& font) {
    if (len <= 0 || !text) return 0.0f;
    const Gdiplus::StringFormat* pGeneric = Gdiplus::StringFormat::GenericTypographic();
    Gdiplus::StringFormat fmt(pGeneric);
    fmt.SetFormatFlags(fmt.GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces | Gdiplus::StringFormatFlagsNoWrap);
    Gdiplus::RectF box;
    g.MeasureString(text, len, &font, Gdiplus::PointF(0.0f, 0.0f), &fmt, &box);
    return box.Width;
}

void RecomputeDialogMetrics(RunDialogState* state) {
    if (!state) return;
    int histCount = (int)state->history.size();
    int padTop = GetListPadTop(state);
    int padBot = GetListPadBottom(state);
    int rowH   = GetRowHeight(state);
    int barH   = GetInputBarHeight(state);

    state->topAreaHeight = (histCount > 0) ? (padTop + histCount * rowH + padBot) : 0;
    state->totalHeight   = state->topAreaHeight + barH;

    if (state->hFontEditCtl) {
        DeleteObject(state->hFontEditCtl);
    }
    state->hFontEditCtl = CreateFontW(
        -DpiScale(16, state->dpi), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
        SendMessageW(state->hModernEdit, WM_SETFONT, (WPARAM)state->hFontEditCtl, FALSE);
    }
}

LRESULT CALLBACK ModernEditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                        UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto* state = reinterpret_cast<RunDialogState*>(dwRefData);

    if (uMsg == WM_GETDLGCODE) {
        return DLGC_WANTALLKEYS | DLGC_HASSETSEL;
    }

    if (uMsg == WM_KEYDOWN) {
        state->caretVisible = true;
        if (wParam == VK_ESCAPE) {
            CloseRunDialog(state->hDlg);
            return 0;
        }
        if (wParam == VK_RETURN) {
            TriggerRunCommand(state);
            return 0;
        }
        if ((wParam == VK_UP || wParam == VK_DOWN) && !state->history.empty()) {
            int n = (int)state->history.size();
            if (wParam == VK_UP) {
                state->selectedRow = (state->selectedRow <= 0) ? (n - 1) : (state->selectedRow - 1);
            } else {
                state->selectedRow = (state->selectedRow >= n - 1) ? 0 : (state->selectedRow + 1);
            }
            const auto& cmd = state->history[state->selectedRow].cmd;
            SetWindowTextW(hWnd, cmd.c_str());
            SendMessageW(hWnd, EM_SETSEL, cmd.size(), cmd.size());
            SyncInputToNativeCombo(state, cmd.c_str());
            InvalidateRect(state->hDlg, nullptr, FALSE);
            return 0;
        }
    }

    if (uMsg == WM_CHAR && (wParam == VK_RETURN || wParam == VK_ESCAPE)) {
        return 0;
    }

    LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_CHAR || uMsg == WM_KEYUP || uMsg == WM_KEYDOWN ||
        uMsg == WM_PASTE || uMsg == WM_CUT || uMsg == WM_SETTEXT ||
        uMsg == WM_IME_STARTCOMPOSITION || uMsg == WM_IME_COMPOSITION ||
        uMsg == WM_IME_ENDCOMPOSITION || uMsg == WM_IME_CHAR ||
        uMsg == EM_SETSEL) {
        state->caretVisible = true;
        SyncInputToNativeCombo(state);
        RECT rcBar = { 0, state->topAreaHeight, GetDlgWidth(state), state->totalHeight };
        InvalidateRect(state->hDlg, &rcBar, FALSE);
    }

    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, ModernEditSubclassProc, uIdSubclass);
    }
    return res;
}

void CleanupDialogState(HWND hWnd, RunDialogState* state) {
    KillTimer(hWnd, kTimerCaret);
    HWND expected = hWnd;
    g_hActiveRunDlg.compare_exchange_strong(expected, nullptr);
    g_isBrowsingFile.store(false);
    StopKeyHookThread();
    RemovePropW(hWnd, L"WindhawkModernRunApplied");

    if (state) {
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            RemoveWindowSubclass(state->hModernEdit, ModernEditSubclassProc, 1);
            DestroyWindow(state->hModernEdit);
            state->hModernEdit = nullptr;
        }
        for (auto& item : state->history) {
            if (item.hIcon) DestroyIcon(item.hIcon);
        }
        state->history.clear();
        if (state->hFontEditCtl) {
            DeleteObject(state->hFontEditCtl);
            state->hFontEditCtl = nullptr;
        }
        if (state->hRunIcon) {
            DestroyIcon(state->hRunIcon);
            state->hRunIcon = nullptr;
        }
        delete state;
    }

    if (g_hUninitEvent) {
        SetEvent(g_hUninitEvent);
    }
}

LRESULT CALLBACK RunDlgSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

void PerformUninitCleanup(HWND hWnd, RunDialogState* state) {
    RemoveWindowSubclass(hWnd, RunDlgSubclassProc, 1);
    CleanupDialogState(hWnd, state);
    HWND hCancel = GetDlgItem(hWnd, IDCANCEL);
    if (hCancel) EnableWindow(hCancel, TRUE);
    PostMessageW(hWnd, WM_COMMAND, IDCANCEL, 0);
}

LRESULT CALLBACK RunDlgSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto* state = reinterpret_cast<RunDialogState*>(dwRefData);

    if (uMsg == g_msgUninit && g_msgUninit != 0) {
        if (state && (state->isBrowsingFile || state->isTriggeringRun)) {
            state->pendingUninit = true;
            HWND hPopup = GetWindow(hWnd, GW_ENABLEDPOPUP);
            if (hPopup && hPopup != hWnd) {
                PostMessageW(hPopup, WM_CLOSE, 0, 0);
            }
            return 1;
        }
        PerformUninitCleanup(hWnd, state);
        return 1;
    }

    if (uMsg == g_msgToggle && g_msgToggle != 0) {
        if (!state) return 0;
        switch (wParam) {
            case kActionMinimize:
                MinimizeRunDialog(state);
                break;
            case kActionRestore:
                RestoreRunDialog(state);
                break;
            case kActionToggleWinR:
                if (IsIconic(hWnd)) {
                    RestoreRunDialog(state);
                } else {
                    CloseRunDialog(hWnd);
                }
                break;
            case kActionReloadSettings: {
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    state->settings = g_settings;
                    state->i18n = g_i18n;
                }
                LoadHistoryItems(state);
                RecomputeDialogMetrics(state);
                int dlgW = GetDlgWidth(state);
                ClampDialogToMonitorWorkArea(hWnd, &state->normalX, &state->normalY, dlgW, state->totalHeight, state->dpi);
                if (!IsIconic(hWnd)) {
                    SetWindowPos(hWnd, nullptr, state->normalX, state->normalY, dlgW, state->totalHeight,
                                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
                }
                ApplyWindowBlur(hWnd, state);
                InvalidateRect(hWnd, nullptr, TRUE);
                break;
            }
        }
        return 0;
    }

    switch (uMsg) {
        case WM_NCCALCSIZE:
            return 0;

        // Return TRUE directly without calling DefWindowProcW so the classic title bar is never painted
        case WM_NCACTIVATE:
            return TRUE;

        case WM_NCPAINT:
            return 0;

        // Strip WS_CAPTION on style changes so win32kfull never draws a legacy caption bar
        case WM_STYLECHANGING: {
            if (wParam == GWL_STYLE && lParam) {
                auto* ss = reinterpret_cast<STYLESTRUCT*>(lParam);
                ss->styleNew &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
                ss->styleNew |= (WS_SYSMENU | WS_MINIMIZEBOX);
            }
            break;
        }

        case WM_DPICHANGED: {
            if (state) {
                state->dpi = HIWORD(wParam);
                RecomputeDialogMetrics(state);
                const RECT* prcSuggested = reinterpret_cast<const RECT*>(lParam);
                int dlgW = GetDlgWidth(state);
                int newX = prcSuggested ? prcSuggested->left : state->normalX;
                int newY = prcSuggested ? prcSuggested->top  : state->normalY;
                ClampDialogToMonitorWorkArea(hWnd, &newX, &newY, dlgW, state->totalHeight, state->dpi);
                state->normalX = newX;
                state->normalY = newY;
                SetWindowPos(hWnd, nullptr, newX, newY, dlgW, state->totalHeight,
                             SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_SYSCOMMAND: {
            UINT cmd = (wParam & 0xFFF0);
            if (cmd == SC_MINIMIZE) {
                MinimizeRunDialog(state);
                return 0;
            }
            if (cmd == SC_RESTORE) {
                RestoreRunDialog(state);
                return 0;
            }
            break;
        }

        case WM_WINDOWPOSCHANGING: {
            auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
            if (wp && state && !(wp->flags & SWP_NOSIZE)) {
                if (!IsIconic(hWnd) && wp->x > -10000 && wp->y > -10000) {
                    wp->cx = GetDlgWidth(state);
                    wp->cy = state->totalHeight;
                }
            }
            break;
        }

        case WM_WINDOWPOSCHANGED: {
            auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
            if (wp && state && !IsIconic(hWnd) && !(wp->flags & SWP_NOMOVE)) {
                if (wp->x > -10000 && wp->y > -10000) {
                    state->normalX = wp->x;
                    state->normalY = wp->y;
                }
            }
            break;
        }

        case WM_CLOSE:
            CloseRunDialog(hWnd);
            return 0;

        case WM_TIMER: {
            if (wParam == kTimerCaret && state) {
                if (!IsIconic(hWnd)) {
                    state->caretVisible = !state->caretVisible;
                    RECT rcBar = { 0, state->topAreaHeight, GetDlgWidth(state), state->totalHeight };
                    InvalidateRect(hWnd, &rcBar, FALSE);
                }
                return 0;
            }
            break;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == kModernEditId && (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == EN_UPDATE)) {
                if (state) {
                    state->caretVisible = true;
                    SyncInputToNativeCombo(state);
                    RECT rcBar = { 0, state->topAreaHeight, GetDlgWidth(state), state->totalHeight };
                    InvalidateRect(hWnd, &rcBar, FALSE);
                }
                return 0;
            }
            if (LOWORD(wParam) == IDOK && state && !state->isTriggeringRun) {
                TriggerRunCommand(state);
                return 0;
            }
            break;
        }

        case WM_ACTIVATE: {
            if (LOWORD(wParam) != WA_INACTIVE && !IsIconic(hWnd) && state && state->hModernEdit && !state->isBrowsingFile) {
                SetFocus(state->hModernEdit);
            } else if (LOWORD(wParam) == WA_INACTIVE && state && state->settings.closeOnClickOutside && !state->isBrowsingFile) {
                if (GetTickCount64() - g_openTick.load() > 300) {
                    CloseRunDialog(hWnd);
                    return 0;
                }
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            int newHoverRow = HitTestHistoryRow(state, pt);
            RECT rcBrowse = GetBrowseButtonRect(state);
            RECT rcBtn    = GetRunButtonRect(state);
            RECT rcClose  = GetCloseButtonRect(state);
            bool newBrowseHover = PtInRect(&rcBrowse, pt);
            bool newBtnHover    = PtInRect(&rcBtn, pt);
            bool newCloseHover  = PtInRect(&rcClose, pt);

            if (newHoverRow != state->hoveredRow ||
                newBrowseHover != state->browseHovered ||
                newBtnHover != state->btnHovered ||
                newCloseHover != state->closeHovered) {
                state->hoveredRow    = newHoverRow;
                state->browseHovered = newBrowseHover;
                state->btnHovered    = newBtnHover;
                state->closeHovered  = newCloseHover;

                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            if (state->hoveredRow != -1 ||
                state->browseHovered || state->browsePressed ||
                state->btnHovered || state->btnPressed ||
                state->closeHovered || state->closePressed) {
                state->hoveredRow    = -1;
                state->browseHovered = false;
                state->browsePressed = false;
                state->btnHovered    = false;
                state->btnPressed    = false;
                state->closeHovered  = false;
                state->closePressed  = false;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            RECT rcBrowse = GetBrowseButtonRect(state);
            RECT rcBtn    = GetRunButtonRect(state);
            RECT rcClose  = GetCloseButtonRect(state);
            RECT rcEdit   = GetModernEditRect(state);

            if (PtInRect(&rcBrowse, pt)) {
                state->browsePressed = true;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRect(&rcBtn, pt)) {
                state->btnPressed = true;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRect(&rcClose, pt)) {
                state->closePressed = true;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRect(&rcEdit, pt) && state->hModernEdit) {
                SetFocus(state->hModernEdit);
                int len = GetWindowTextLengthW(state->hModernEdit);
                SendMessageW(state->hModernEdit, EM_SETSEL, len, len);
                state->caretVisible = true;
                InvalidateRect(hWnd, &rcEdit, FALSE);
                return 0;
            }

            int row = HitTestHistoryRow(state, pt);
            if (row != -1) {
                state->selectedRow = row;
                const auto& cmd = state->history[row].cmd;
                if (state->hModernEdit) {
                    SetWindowTextW(state->hModernEdit, cmd.c_str());
                    SendMessageW(state->hModernEdit, EM_SETSEL, cmd.size(), cmd.size());
                    SetFocus(state->hModernEdit);
                }
                SyncInputToNativeCombo(state, cmd.c_str());
                state->caretVisible = true;
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }

            ReleaseCapture();
            SendMessageW(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            RECT rcBrowse = GetBrowseButtonRect(state);
            RECT rcBtn    = GetRunButtonRect(state);
            RECT rcClose  = GetCloseButtonRect(state);

            if (state->browsePressed) {
                state->browsePressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&rcBrowse, pt)) {
                    OpenBrowseExecutableDialog(state);
                }
                return 0;
            }
            if (state->btnPressed) {
                state->btnPressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&rcBtn, pt)) TriggerRunCommand(state);
                return 0;
            }
            if (state->closePressed) {
                state->closePressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&rcClose, pt)) CloseRunDialog(hWnd);
                return 0;
            }
            break;
        }

        case WM_LBUTTONDBLCLK: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            int row = HitTestHistoryRow(state, pt);
            if (row != -1) {
                const auto& cmd = state->history[row].cmd;
                if (state->hModernEdit) SetWindowTextW(state->hModernEdit, cmd.c_str());
                SyncInputToNativeCombo(state, cmd.c_str());
                TriggerRunCommand(state);
                return 0;
            }
            break;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            if (IsIconic(hWnd) || !state) {
                EndPaint(hWnd, &ps);
                return 0;
            }

            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            int w = rcClient.right;
            int h = rcClient.bottom;
            UINT dpi = state->dpi;

            if (w > 0 && h > 0) {
                Gdiplus::Bitmap canvas(w, h, PixelFormat32bppARGB);
                Gdiplus::Graphics g(&canvas);
                g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
                g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

                if (state->settings.enableBlur) {
                    g.Clear(Gdiplus::Color(18, 16, 18, 24));
                    if (state->topAreaHeight > 0) {
                        Gdiplus::SolidBrush bottomTint(Gdiplus::Color(28, 8, 10, 14));
                        g.FillRectangle(&bottomTint, 0, state->topAreaHeight, w, h - state->topAreaHeight);
                    }
                } else {
                    g.Clear(Gdiplus::Color(255, 28, 30, 36));
                    if (state->topAreaHeight > 0) {
                        Gdiplus::SolidBrush bottomSolid(Gdiplus::Color(255, 22, 24, 30));
                        g.FillRectangle(&bottomSolid, 0, state->topAreaHeight, w, h - state->topAreaHeight);
                    }
                }

                Gdiplus::FontFamily fontFamily(L"Segoe UI");
                Gdiplus::Font fontCmd(&fontFamily, DpiScaleF(15.5f, dpi), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
                Gdiplus::Font fontApp(&fontFamily, DpiScaleF(14.5f, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                Gdiplus::Font fontTime(&fontFamily, DpiScaleF(13.0f, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                Gdiplus::Font fontInput(&fontFamily, DpiScaleF(15.5f, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                Gdiplus::Font fontBtn(&fontFamily, DpiScaleF(14.5f, dpi), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

                const Gdiplus::StringFormat* pGeneric = Gdiplus::StringFormat::GenericTypographic();
                Gdiplus::StringFormat fmtLeft(pGeneric);
                fmtLeft.SetLineAlignment(Gdiplus::StringAlignmentCenter);
                fmtLeft.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
                fmtLeft.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);

                Gdiplus::StringFormat fmtLeftNoTrim(pGeneric);
                fmtLeftNoTrim.SetLineAlignment(Gdiplus::StringAlignmentCenter);
                fmtLeftNoTrim.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

                Gdiplus::StringFormat fmtRight(pGeneric);
                fmtRight.SetAlignment(Gdiplus::StringAlignmentFar);
                fmtRight.SetLineAlignment(Gdiplus::StringAlignmentCenter);
                fmtRight.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

                Gdiplus::SolidBrush brWhite(Gdiplus::Color(255, 255, 255, 255));
                Gdiplus::SolidBrush brSubtle(Gdiplus::Color(230, 195, 202, 214));
                Gdiplus::SolidBrush brTime(Gdiplus::Color(205, 175, 184, 198));
                Gdiplus::SolidBrush brPlaceholder(Gdiplus::Color(180, 190, 198, 210));

                int padTop = GetListPadTop(state);
                int rowHInt = GetRowHeight(state);
                float cornerRadius = DpiScaleF(8.0f, dpi);
                float borderW = DpiScaleF(1.0f, dpi);
                float textLeftX = DpiScaleF(60.0f, dpi);

                // 1. Draw history rows (command, friendly app name, and execution timestamp)
                for (size_t i = 0; i < state->history.size(); ++i) {
                    float rowTop = (float)(padTop + (int)i * rowHInt);
                    float rowH   = (float)rowHInt;

                    if ((int)i == state->hoveredRow || (int)i == state->selectedRow) {
                        float hlPadX = DpiScaleF(12.0f, dpi);
                        float hlPadY = DpiScaleF(2.0f, dpi);
                        Gdiplus::RectF rcHl(hlPadX, rowTop + hlPadY, (float)w - hlPadX * 2.0f, rowH - hlPadY * 2.0f);
                        FillAndStrokeRoundRect(g, rcHl, cornerRadius,
                                               Gdiplus::Color(65, 255, 255, 255),
                                               Gdiplus::Color(55, 255, 255, 255), borderW);
                    }

                    float rightLimit = (float)w - DpiScaleF(52.0f, dpi);

                    if (state->settings.showTimestamp && !state->history[i].timeStr.empty()) {
                        Gdiplus::RectF rcTime((float)w - DpiScaleF(220.0f, dpi), rowTop, DpiScaleF(168.0f, dpi), rowH);
                        g.DrawString(state->history[i].timeStr.c_str(), -1, &fontTime, rcTime, &fmtRight, &brTime);
                        float timeW = MeasureGdiPlusStringWidth(g, state->history[i].timeStr.c_str(),
                                                                (int)state->history[i].timeStr.size(), fontTime);
                        rightLimit = (float)w - DpiScaleF(64.0f, dpi) - timeW;
                    }

                    const std::wstring& cmdStr = state->history[i].cmd;
                    float cmdW = MeasureGdiPlusStringWidth(g, cmdStr.c_str(), (int)cmdStr.size(), fontCmd);
                    Gdiplus::RectF rcCmd(textLeftX, rowTop, (std::max)(DpiScaleF(40.0f, dpi), rightLimit - textLeftX), rowH);
                    g.DrawString(cmdStr.c_str(), -1, &fontCmd, rcCmd, &fmtLeft, &brWhite);

                    if (!state->history[i].appName.empty()) {
                        float appX = textLeftX + cmdW;
                        if (appX + DpiScaleF(30.0f, dpi) < rightLimit) {
                            std::wstring suffix = L"  -  " + state->history[i].appName;
                            Gdiplus::RectF rcApp(appX, rowTop, rightLimit - appX, rowH);
                            g.DrawString(suffix.c_str(), -1, &fontApp, rcApp, &fmtLeft, &brSubtle);
                        }
                    }

                    DrawHistoryClockVector(g, (float)w - DpiScaleF(32.0f, dpi), rowTop + rowH * 0.5f, dpi,
                                           Gdiplus::Color(210, 190, 198, 210));
                }

                if (state->topAreaHeight > 0) {
                    Gdiplus::Pen divPen(Gdiplus::Color(48, 255, 255, 255), borderW);
                    g.DrawLine(&divPen, 0, state->topAreaHeight, w, state->topAreaHeight);
                }

                // 2. Draw the input box with smooth horizontal scrolling for long paths
                RECT rcEditWin = GetModernEditRect(state);
                Gdiplus::RectF rcEdit((float)rcEditWin.left, (float)rcEditWin.top,
                                      (float)(rcEditWin.right - rcEditWin.left),
                                      (float)(rcEditWin.bottom - rcEditWin.top));

                std::wstring displayStr = GetEffectiveInputText(state->hModernEdit);
                int textLen = (int)displayStr.size();

                DWORD selStart = 0, selEnd = 0;
                if (state->hModernEdit) {
                    SendMessageW(state->hModernEdit, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
                }
                if (selStart > (DWORD)textLen) selStart = textLen;
                if (selEnd > (DWORD)textLen)   selEnd = textLen;
                if (textLen > 0 && selEnd == 0 && selStart == 0) {
                    selEnd = textLen;
                }

                float caretDist = MeasureGdiPlusStringWidth(g, displayStr.c_str(), (int)selEnd, fontInput);
                float scrollPad = DpiScaleF(8.0f, dpi);
                float scrollOffsetX = 0.0f;
                if (caretDist > rcEdit.Width - scrollPad) {
                    scrollOffsetX = caretDist - (rcEdit.Width - scrollPad);
                }

                Gdiplus::GraphicsState clipState = g.Save();
                g.SetClip(rcEdit);

                if (textLen == 0) {
                    g.DrawString(state->i18n.placeholder, -1, &fontInput, rcEdit, &fmtLeft, &brPlaceholder);
                } else {
                    if (selStart != selEnd) {
                        DWORD s0 = (std::min)(selStart, selEnd);
                        DWORD s1 = (std::max)(selStart, selEnd);
                        float x0 = MeasureGdiPlusStringWidth(g, displayStr.c_str(), (int)s0, fontInput) - scrollOffsetX;
                        float x1 = MeasureGdiPlusStringWidth(g, displayStr.c_str(), (int)s1, fontInput) - scrollOffsetX;
                        float hlX = rcEdit.X + x0;
                        float hlW = x1 - x0;
                        if (hlW > 0.0f) {
                            float selPadY = DpiScaleF(2.0f, dpi);
                            Gdiplus::SolidBrush selBr(Gdiplus::Color(150, 0, 120, 215));
                            g.FillRectangle(&selBr, hlX, rcEdit.Y + selPadY, hlW, rcEdit.Height - selPadY * 2.0f);
                        }
                    }
                    Gdiplus::RectF rcScrolledText(rcEdit.X - scrollOffsetX, rcEdit.Y,
                                                  rcEdit.Width + scrollOffsetX + DpiScaleF(2000.0f, dpi), rcEdit.Height);
                    g.DrawString(displayStr.c_str(), textLen, &fontInput, rcScrolledText, &fmtLeftNoTrim, &brWhite);
                }

                bool editFocused = (GetFocus() == state->hModernEdit) || (GetForegroundWindow() == hWnd && !state->isBrowsingFile);
                if (editFocused && state->caretVisible) {
                    float caretX = rcEdit.X + caretDist - scrollOffsetX + DpiScaleF(1.0f, dpi);
                    caretX = std::clamp(caretX, rcEdit.X + DpiScaleF(1.0f, dpi), rcEdit.X + rcEdit.Width - DpiScaleF(2.0f, dpi));
                    float caretPadY = DpiScaleF(4.0f, dpi);
                    Gdiplus::Pen caretPen(Gdiplus::Color(245, 255, 255, 255), DpiScaleF(1.8f, dpi));
                    g.DrawLine(&caretPen, caretX, rcEdit.Y + caretPadY, caretX, rcEdit.Y + rcEdit.Height - caretPadY);
                }

                g.Restore(clipState);

                // 3. Draw Browse button
                RECT rBrowse = GetBrowseButtonRect(state);
                Gdiplus::RectF rcBrowseBtn((float)rBrowse.left, (float)rBrowse.top,
                                           (float)(rBrowse.right - rBrowse.left), (float)(rBrowse.bottom - rBrowse.top));
                Gdiplus::Color browseFill = state->browsePressed ? Gdiplus::Color(215, 42, 50, 62)
                                          : (state->browseHovered ? Gdiplus::Color(230, 68, 80, 98)
                                                                  : Gdiplus::Color(185, 48, 56, 70));
                Gdiplus::Color browseBorder = state->browseHovered ? Gdiplus::Color(190, 155, 175, 205)
                                                                   : Gdiplus::Color(115, 110, 125, 145);
                FillAndStrokeRoundRect(g, rcBrowseBtn, cornerRadius, browseFill, browseBorder, borderW);

                Gdiplus::RectF rcBrowseText(rcBrowseBtn.X + DpiScaleF(12.0f, dpi), rcBrowseBtn.Y,
                                            rcBrowseBtn.Width - DpiScaleF(34.0f, dpi), rcBrowseBtn.Height);
                g.DrawString(state->i18n.browseBtn, -1, &fontBtn, rcBrowseText, &fmtLeft, &brWhite);
                DrawFolderBrowseVector(g, rcBrowseBtn.X + rcBrowseBtn.Width - DpiScaleF(18.0f, dpi),
                                       rcBrowseBtn.Y + rcBrowseBtn.Height * 0.5f, dpi,
                                       Gdiplus::Color(255, 255, 255, 255));

                // 4. Draw Run button
                RECT rBtn = GetRunButtonRect(state);
                Gdiplus::RectF rcRunBtn((float)rBtn.left, (float)rBtn.top,
                                        (float)(rBtn.right - rBtn.left), (float)(rBtn.bottom - rBtn.top));
                Gdiplus::Color runFill = state->btnPressed ? Gdiplus::Color(210, 0, 96, 175)
                                       : (state->btnHovered ? Gdiplus::Color(235, 24, 142, 238)
                                                            : Gdiplus::Color(205, 0, 120, 212));
                Gdiplus::Color runBorder = state->btnHovered ? Gdiplus::Color(180, 130, 205, 255)
                                                             : Gdiplus::Color(110, 90, 175, 245);
                FillAndStrokeRoundRect(g, rcRunBtn, cornerRadius, runFill, runBorder, borderW);

                Gdiplus::RectF rcRunText(rcRunBtn.X + DpiScaleF(14.0f, dpi), rcRunBtn.Y,
                                         rcRunBtn.Width - DpiScaleF(36.0f, dpi), rcRunBtn.Height);
                g.DrawString(state->i18n.runBtn, -1, &fontBtn, rcRunText, &fmtLeft, &brWhite);
                DrawEnterArrowVector(g, rcRunBtn.X + rcRunBtn.Width - DpiScaleF(20.0f, dpi),
                                     rcRunBtn.Y + rcRunBtn.Height * 0.5f, dpi,
                                     Gdiplus::Color(255, 255, 255, 255));

                // 5. Draw Exit button
                RECT rClose = GetCloseButtonRect(state);
                Gdiplus::RectF rcCloseBtn((float)rClose.left, (float)rClose.top,
                                          (float)(rClose.right - rClose.left), (float)(rClose.bottom - rClose.top));
                Gdiplus::Color closeFill = state->closePressed ? Gdiplus::Color(215, 155, 30, 24)
                                         : (state->closeHovered ? Gdiplus::Color(235, 205, 46, 36)
                                                                : Gdiplus::Color(165, 92, 42, 50));
                Gdiplus::Color closeBorder = state->closeHovered ? Gdiplus::Color(190, 255, 130, 125)
                                                                 : Gdiplus::Color(110, 175, 95, 105);
                FillAndStrokeRoundRect(g, rcCloseBtn, cornerRadius, closeFill, closeBorder, borderW);

                Gdiplus::RectF rcCloseText(rcCloseBtn.X + DpiScaleF(12.0f, dpi), rcCloseBtn.Y,
                                           rcCloseBtn.Width - DpiScaleF(32.0f, dpi), rcCloseBtn.Height);
                g.DrawString(state->i18n.exitBtn, -1, &fontBtn, rcCloseText, &fmtLeft, &brWhite);
                DrawCloseCrossVector(g, rcCloseBtn.X + rcCloseBtn.Width - DpiScaleF(18.0f, dpi),
                                     rcCloseBtn.Y + rcCloseBtn.Height * 0.5f, dpi,
                                     Gdiplus::Color(255, 255, 255, 255));

                // 6. Convert to 32-bit Premultiplied Alpha DIB for DWM composition
                HDC hMemDC = CreateCompatibleDC(hdc);
                BITMAPINFO bmi = {};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = w;
                bmi.bmiHeader.biHeight = -h;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;

                DWORD* pixels = nullptr;
                HBITMAP hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, (void**)&pixels, nullptr, 0);
                if (hBmp && pixels) {
                    HGDIOBJ oldBmp = SelectObject(hMemDC, hBmp);

                    Gdiplus::BitmapData bd = {};
                    Gdiplus::Rect lockRect(0, 0, w, h);
                    if (canvas.LockBits(&lockRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bd) == Gdiplus::Ok) {
                        for (int y = 0; y < h; ++y) {
                            const DWORD* src = (const DWORD*)((const BYTE*)bd.Scan0 + y * bd.Stride);
                            DWORD* dst = pixels + y * w;
                            for (int x = 0; x < w; ++x) {
                                DWORD c = src[x];
                                DWORD a = (c >> 24) & 0xFF;
                                DWORD r = (((c >> 16) & 0xFF) * a) / 255;
                                DWORD gr = (((c >> 8) & 0xFF) * a) / 255;
                                DWORD b = ((c & 0xFF) * a) / 255;
                                dst[x] = (a << 24) | (r << 16) | (gr << 8) | b;
                            }
                        }
                        canvas.UnlockBits(&bd);
                    }

                    int iconSize = DpiScale(24, dpi);
                    int iconX    = DpiScale(24, dpi);
                    int barH     = GetInputBarHeight(state);

                    for (size_t i = 0; i < state->history.size(); ++i) {
                        if (state->history[i].hIcon) {
                            int iconY = padTop + (int)i * rowHInt + (rowHInt - iconSize) / 2;
                            DrawIconEx(hMemDC, iconX, iconY, state->history[i].hIcon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
                        }
                    }

                    if (state->hRunIcon) {
                        int runIconY = state->topAreaHeight + (barH - iconSize) / 2;
                        DrawIconEx(hMemDC, iconX, runIconY, state->hRunIcon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
                    }

                    BitBlt(hdc, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
                    SelectObject(hMemDC, oldBmp);
                    DeleteObject(hBmp);
                }
                DeleteDC(hMemDC);
            }

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_NCDESTROY: {
            RemoveWindowSubclass(hWnd, RunDlgSubclassProc, uIdSubclass);
            CleanupDialogState(hWnd, state);
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void TransformRunDialog(HWND hDlg) {
    if (g_unloading.load()) return;
    if (!hDlg || !IsWindow(hDlg)) return;
    if (GetPropW(hDlg, L"WindhawkModernRunApplied")) return;
    if (!EnsureGdiplusInitialized()) return;

    SetPropW(hDlg, L"WindhawkModernRunApplied", (HANDLE)1);

    auto* state = new RunDialogState();
    state->hDlg = hDlg;
    state->dpi  = GetDpiForWindow(hDlg);
    if (state->dpi == 0) state->dpi = 96;

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        state->settings = g_settings;
        state->i18n = g_i18n;
    }

    // Remove WS_CAPTION, WS_DLGFRAME, and WS_BORDER so the classic window frame is completely eliminated
    LONG_PTR style = GetWindowLongPtrW(hDlg, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
    style |= (WS_SYSMENU | WS_MINIMIZEBOX);
    SetWindowLongPtrW(hDlg, GWL_STYLE, style);

    LONG_PTR exStyle = GetWindowLongPtrW(hDlg, GWL_EXSTYLE);
    exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_COMPOSITED);
    exStyle |= WS_EX_APPWINDOW;
    SetWindowLongPtrW(hDlg, GWL_EXSTYLE, exStyle);

    BOOL dark = TRUE;
    DwmSetWindowAttribute(hDlg, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    DWORD corner = 2;
    DwmSetWindowAttribute(hDlg, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
    COLORREF borderCol = RGB(72, 78, 88);
    DwmSetWindowAttribute(hDlg, DWMWA_BORDER_COLOR, &borderCol, sizeof(borderCol));

    ApplyWindowBlur(hDlg, state);

    for (HWND hChild = GetWindow(hDlg, GW_CHILD); hChild != nullptr; hChild = GetWindow(hChild, GW_HWNDNEXT)) {
        wchar_t cls[64] = {};
        GetClassNameW(hChild, cls, ARRAYSIZE(cls));
        int id = GetDlgCtrlID(hChild);

        if (_wcsicmp(cls, L"Static") == 0) {
            HICON hIco = (HICON)SendMessageW(hChild, STM_GETICON, 0, 0);
            if (hIco && !state->hRunIcon) state->hRunIcon = CopyIcon(hIco);
            ShowWindow(hChild, SW_HIDE);
        } else if (_wcsicmp(cls, L"Button") == 0) {
            if (id == IDOK) {
                state->hOkBtn = hChild;
                SetWindowPos(hChild, nullptr, -32000, -32000, 1, 1, SWP_NOZORDER);
            } else if (id == IDCANCEL) {
                SetWindowPos(hChild, nullptr, -32000, -32000, 1, 1, SWP_NOZORDER);
            } else if (id == kNativeBrowseBtnId) {
                state->hBrowseBtn = hChild;
                SetWindowPos(hChild, nullptr, -32000, -32000, 1, 1, SWP_NOZORDER);
            } else {
                ShowWindow(hChild, SW_HIDE);
            }
        } else if (_wcsicmp(cls, L"ComboBox") == 0) {
            state->hCombo = hChild;
            state->hOrigEdit = FindWindowExW(hChild, nullptr, L"Edit", nullptr);
            SetWindowPos(hChild, nullptr, -32000, -32000, 1, 1, SWP_NOZORDER);
            ShowWindow(hChild, SW_HIDE);
        } else {
            ShowWindow(hChild, SW_HIDE);
        }
    }

    if (!state->hRunIcon) {
        ExtractIconExW(L"shell32.dll", 24, nullptr, &state->hRunIcon, 1);
    }

    LoadHistoryItems(state);
    RecomputeDialogMetrics(state);

    state->hModernEdit = CreateWindowExW(
        0, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        -32000, -32000, DpiScale(500, state->dpi), DpiScale(28, state->dpi),
        hDlg, (HMENU)(INT_PTR)kModernEditId, GetModuleHandleW(nullptr), nullptr
    );

    if (state->hModernEdit) {
        SendMessageW(state->hModernEdit, WM_SETFONT, (WPARAM)state->hFontEditCtl, FALSE);

        if (state->settings.clearInitialInput) {
            SetWindowTextW(state->hModernEdit, L"");
            SyncInputToNativeCombo(state, L"");
        } else if (state->hCombo) {
            wchar_t initBuf[MAX_PATH] = {};
            GetWindowTextW(state->hCombo, initBuf, MAX_PATH);
            SetWindowTextW(state->hModernEdit, initBuf);
            SendMessageW(state->hModernEdit, EM_SETSEL, 0, -1);
            SyncInputToNativeCombo(state, initBuf);
        }
        SetWindowSubclass(state->hModernEdit, ModernEditSubclassProc, 1, (DWORD_PTR)state);
    }

    SetWindowSubclass(hDlg, RunDlgSubclassProc, 1, (DWORD_PTR)state);
    SetTimer(hDlg, kTimerCaret, 530, nullptr);

    RECT rcWin = {};
    GetWindowRect(hDlg, &rcWin);
    int dlgW = GetDlgWidth(state);
    int newX = rcWin.left;
    int newY = rcWin.top;
    ClampDialogToMonitorWorkArea(hDlg, &newX, &newY, dlgW, state->totalHeight, state->dpi);

    state->normalX = newX;
    state->normalY = newY;

    SetWindowPos(hDlg, nullptr, newX, newY, dlgW, state->totalHeight,
                 SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    ULONGLONG now = GetTickCount64();
    g_openTick.store(now);
    g_lastToggleTick.store(now);
    g_winRReleasedSinceOpen.store((GetAsyncKeyState('R') & 0x8000) == 0);
    g_isBrowsingFile.store(false);
    g_hActiveRunDlg.store(hDlg);

    StartKeyHookThread();

    if (state->hModernEdit) {
        SetFocus(state->hModernEdit);
    }
    InvalidateRect(hDlg, nullptr, TRUE);
}

INT_PTR CALLBACK RunDlgProc_Hook(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    INT_PTR ret = RunDlgProc_Orig(hDlg, uMsg, wParam, lParam);

    if (uMsg == WM_INITDIALOG) {
        TransformRunDialog(hDlg);
        if (HWND hModernEdit = GetDlgItem(hDlg, kModernEditId)) {
            SetFocus(hModernEdit);
            return FALSE;
        }
    }

    return ret;
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    HWND hActive = g_hActiveRunDlg.load();
    if (hActive && hWnd == hActive) {
        if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_FORCEMINIMIZE) {
            PostMessageW(hActive, g_msgToggle, kActionMinimize, 0);
            return TRUE;
        }
        if ((nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL || nCmdShow == SW_SHOW) && IsIconic(hWnd)) {
            PostMessageW(hActive, g_msgToggle, kActionRestore, 0);
            return TRUE;
        }
    }
    return ShowWindow_Orig(hWnd, nCmdShow);
}

void WINAPI SwitchToThisWindow_Hook(HWND hWnd, BOOL fUnknown) {
    HWND hActive = g_hActiveRunDlg.load();
    if (hActive && hWnd == hActive && !g_isBrowsingFile.load()) {
        ULONGLONG now = GetTickCount64();
        if (now - g_openTick.load() > 350 && now - g_lastToggleTick.load() > 250) {
            if (IsIconic(hWnd)) {
                PostMessageW(hActive, g_msgToggle, kActionRestore, 0);
                return;
            } else if (IsWindowVisible(hWnd)) {
                POINT pt = {};
                GetCursorPos(&pt);
                RECT rcDlg = {};
                GetWindowRect(hWnd, &rcDlg);
                if (!PtInRect(&rcDlg, pt)) {
                    PostMessageW(hActive, g_msgToggle, kActionMinimize, 0);
                    return;
                }
            }
        }
    }
    SwitchToThisWindow_Orig(hWnd, fUnknown);
}

BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    HWND hActive = g_hActiveRunDlg.load();
    if (hActive && hWnd == hActive && !g_isBrowsingFile.load()) {
        ULONGLONG now = GetTickCount64();
        if (now - g_openTick.load() > 350 && now - g_lastToggleTick.load() > 250) {
            if (IsIconic(hWnd)) {
                PostMessageW(hActive, g_msgToggle, kActionRestore, 0);
                return TRUE;
            } else if (IsWindowVisible(hWnd)) {
                POINT pt = {};
                GetCursorPos(&pt);
                RECT rcDlg = {};
                GetWindowRect(hWnd, &rcDlg);
                if (!PtInRect(&rcDlg, pt)) {
                    PostMessageW(hActive, g_msgToggle, kActionMinimize, 0);
                    return TRUE;
                }
            }
        }
    }
    return SetForegroundWindow_Orig(hWnd);
}

BOOL Wh_ModInit() {
    g_unloading.store(false);
    LoadSettings();
    g_msgUninit = RegisterWindowMessageW(L"Windhawk_Uninit_" WH_MOD_ID);
    g_msgToggle = RegisterWindowMessageW(L"Windhawk_Toggle_" WH_MOD_ID);
    g_hUninitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32) {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
        {
            {
                L"__int64 __cdecl RunDlgProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
            },
            &RunDlgProc_Orig,
            RunDlgProc_Hook,
            false
        }
    };

    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, ARRAYSIZE(shell32DllHooks))) {
        Wh_Log(L"Failed to hook RunDlgProc in shell32.dll");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Orig);
    WindhawkUtils::SetFunctionHook(SetForegroundWindow, SetForegroundWindow_Hook, &SetForegroundWindow_Orig);
    WindhawkUtils::SetFunctionHook(SwitchToThisWindow, SwitchToThisWindow_Hook, &SwitchToThisWindow_Orig);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (HWND hDlg = g_hActiveRunDlg.load()) {
        PostMessageW(hDlg, g_msgToggle, kActionReloadSettings, 0);
    }
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true);
    if (HWND hDlg = g_hActiveRunDlg.load()) {
        if (g_hUninitEvent) {
            ResetEvent(g_hUninitEvent);
        }
        if (SendMessageW(hDlg, g_msgUninit, 0, 0) == 1 && g_hUninitEvent) {
            WaitForSingleObject(g_hUninitEvent, 5000);
        }
    }
    StopKeyHookThread();
}

void Wh_ModUninit() {
    StopKeyHookThread();
    if (g_hUninitEvent) {
        CloseHandle(g_hUninitEvent);
        g_hUninitEvent = nullptr;
    }
    std::lock_guard<std::mutex> lock(g_gdiplusMutex);
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}
