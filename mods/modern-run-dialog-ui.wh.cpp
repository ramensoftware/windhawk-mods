// ==WindhawkMod==
// @id              modern-run-dialog-ui
// @name            Modern Run Dialog UI For Windows 11
// @description     Transforms the classic Run dialog (Win+R) into a modern Windows 11 Fluent UI with Acrylic blur, rich history, timestamps & Taskbar toggle. | Biến đổi hộp thoại Run (Win+R) sang giao diện Fluent hiện đại với Acrylic Blur, lịch sử kèm tên ứng dụng & thời gian chạy, ẩn/mở từ Taskbar.
// @version         2.0.14
// @author          caotu2k5
// @include         explorer.exe
// @compilerOptions -ldwmapi -lgdi32 -luser32 -lshell32 -lshlwapi -luxtheme -lcomctl32 -lversion -lgdiplus -limm32 -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Modern Run Dialog UI For Windows 11

*[English below | Tiếng Việt ở phần sau]*

---

## 🌐 English

**Modern Run Dialog UI For Windows 11** replaces the classic Win32 **Run (`Win + R`)** dialog with a sleek, borderless **Fluent Design / Command Palette** interface tailored for Windows 11.

### ✨ Key Features

* **Fluent UI & Acrylic Blur:**
  * Completely removes the classic title bar and legacy window frames while preserving native Windows 11 rounded corners.
  * Full-window **Acrylic Blur** background with customizable opacity.
  * **GDI+ 32-bit ARGB (Premultiplied Alpha)** rendering engine with crisp system **Segoe UI** typography (no jagged or dark text edges on transparent backgrounds).

* **Smart History List:**
  * Displays up to **5 recently executed commands** right above the input bar.
  * Automatically extracts **native application icons** and **friendly application names** in the `command - App Name` format (e.g., `cmd - Command Prompt`, `winver - Windows Version`, `winword - Microsoft Word`).
  * Shows the **last executed timestamp** (e.g., `Today, 15:30` or `14:20 18/10`) next to the history clock icon on each row.
  * Supports single-click to fill the input box, double-click to execute immediately, or **Up / Down (`↑` / `↓`)** arrow keys to cycle through history items.

* **Modern Input Bar & Action Buttons:**
  * Borderless single-line input box with automatic horizontal scrolling for long file paths.
  * **Browse Button (`Browse 📁`):** Opens the native Windows file picker (`*.exe`, `*.bat`, `*.cmd`, `*.msc`, `*.cpl`) and inserts the full path (automatically quoted if spaces are present) into the input bar so you can review or add arguments before running.
  * **Run (`Run ↵`) & Exit (`Exit ✕`) Buttons:** High-contrast Fluent accent buttons with smooth hover and press states.

* **Quality-of-Life Fixes for Windows Run:**
  * **Taskbar Icon Click to Minimize / Restore:** Fixes the long-standing Windows limitation where clicking the Run icon on the Taskbar does nothing. You can now click the Taskbar icon to minimize (hide) or restore the Run window just like any normal app.
  * **`Win + R` Toggle:** Pressing `Win + R` while the Run dialog is open will immediately close it (or restore it if minimized to the Taskbar).
  * **Flawless IME & Multilingual Input:** Works seamlessly with Windows IME (Vietnamese Telex/VNI, Japanese, Chinese, Korean) and third-party input methods (Unikey, EVKey) without white composition box glitches or history selection overrides when pressing `Enter`.

* **Multilingual Interface:**
  * Automatically detects the Windows UI language or lets you manually choose from 10 languages: English, Vietnamese, Japanese, Simplified Chinese, Traditional Chinese, Korean, French, German, Spanish, and Russian.

### ⌨️ Shortcuts & Controls

| Shortcut / Action | Function |
| :--- | :--- |
| **`Win + R`** | Open Run dialog / Close if already open (Restore if minimized) |
| **`Enter`** | Execute the command currently in the input bar |
| **`Esc`** | Close the Run dialog immediately |
| **`↑` / `↓`** | Navigate through the recent command history list |
| **Single-click history row** | Fill the selected command into the input bar |
| **Double-click history row** | Execute the selected command immediately |
| **Click Run icon on Taskbar** | Minimize (Hide) / Restore the Run window (or Close, configurable) |

### ⚙️ Mod Settings

1. **Language:** Auto-detect from Windows or select a fixed language.
2. **Taskbar Icon Click Action:** Choose between *Minimize / Restore window* or *Close window*.
3. **Acrylic Blur Effect:** Enable or disable the translucent blurred background.
4. **Blur Opacity (0 - 255):** Adjust background transparency (lower values = more transparent).
5. **Max History Items (0 - 5):** Number of recent commands shown in the top panel (set to `0` for input-bar-only mode).
6. **Show Last Executed Timestamp:** Show or hide the execution time next to the history icon.
7. **Clear Initial Input on Open:** Start with an empty input box and placeholder text instead of pre-filling the last command.
8. **Close on Click Outside:** Automatically close the Run dialog when clicking outside the window.

---

## 🇻🇳 Tiếng Việt

**Modern Run Dialog UI For Windows 11** thay thế hộp thoại **Run (`Win + R`)** cổ điển của Windows bằng giao diện **Fluent Design / Command Palette** hiện đại, không viền và trong suốt chuẩn Windows 11.

### ✨ Tính năng nổi bật

* **Giao diện Fluent & Hiệu ứng kính mờ (Acrylic Blur):**
  * Loại bỏ hoàn toàn thanh tiêu đề (Titlebar) và khung viền cổ điển, giữ nguyên góc bo tròn tự nhiên của Windows 11.
  * Làm mờ nền trong suốt (**Acrylic Blur**) toàn cửa sổ, cho phép tùy chỉnh độ trong suốt theo ý thích.
  * Kết xuất đồ họa bằng **GDI+ 32-bit ARGB (Premultiplied Alpha)** với phông chữ hệ thống **Segoe UI** sắc nét, không bị rỗ hay viền đen quanh chữ.

* **Danh sách lịch sử thông minh (Smart History List):**
  * Hiển thị tối đa **5 lệnh chạy gần nhất** ở phần phía trên thanh nhập liệu.
  * Tự động trích xuất **biểu tượng (Icon) gốc** và **tên ứng dụng** theo định dạng `lệnh - Tên ứng dụng` (ví dụ: `cmd - Command Prompt`, `winver - Windows Version`, `winword - Microsoft Word`).
  * Hiển thị **thời gian chạy lệnh gần nhất** (ví dụ: `Hôm nay, 15:30` hoặc `14:20 18/10`) ngay bên cạnh biểu tượng đồng hồ ở mỗi dòng lịch sử.
  * Hỗ trợ bấm chuột 1 lần để điền nhanh lệnh xuống ô nhập, nhấp đúp chuột để chạy ngay, hoặc dùng phím mũi tên **Lên / Xuống (`↑` / `↓`)** để duyệt lịch sử.

* **Thanh nhập liệu & Cụm nút chức năng hiện đại:**
  * Ô nhập lệnh 1 dòng không viền, tự động cuộn ngang mượt mà khi đường dẫn dài.
  * **Nút Duyệt (`Duyệt 📁`):** Mở hộp thoại chọn tệp mặc định của hệ thống (`*.exe`, `*.bat`, `*.cmd`, `*.msc`, `*.cpl`) và đưa toàn bộ đường dẫn (tự động bọc dấu ngoặc kép nếu có khoảng trắng) lên thanh nhập để người dùng tùy ý kiểm tra hoặc thêm tham số trước khi quyết định chạy.
  * **Nút Chạy (`Chạy ↵`) & Thoát (`Thoát ✕`):** Thiết kế nổi bật, trực quan với hiệu ứng chuyển màu khi rê và nhấn chuột.

* **Khắc phục triệt để các bất cập cố hữu của Windows Run:**
  * **Bấm biểu tượng Run trên Taskbar để Ẩn / Mở cửa sổ:** Sửa lỗi cố hữu của Windows khi bấm vào icon Run dưới thanh Taskbar không có phản hồi. Giờ đây bạn có thể bấm vào icon trên Taskbar để thu nhỏ (ẩn) hoặc mở lại cửa sổ Run như mọi ứng dụng bình thường.
  * **Nhấn `Win + R` để đóng nhanh:** Nhấn lại `Win + R` khi hộp thoại đang mở sẽ lập tức đóng cửa sổ (hoặc khôi phục lên nếu đang thu nhỏ dưới Taskbar).
  * **Tương thích hoàn hảo bộ gõ Tiếng Việt (IME):** Hoạt động mượt mà với bộ gõ Windows (Telex/VNI), Unikey, EVKey và các bộ gõ đa ngôn ngữ (Nhật, Trung, Hàn), không bị lỗi khung trắng đè chữ và không bị chạy nhầm lệnh trong lịch sử khi nhấn `Enter`.

* **Hỗ trợ Đa ngôn ngữ (Multilingual):**
  * Tự động nhận diện ngôn ngữ hệ thống hoặc tùy chọn cố định 10 ngôn ngữ: Tiếng Việt, English, 日本語, 简体中文, 繁體中文, 한국어, Français, Deutsch, Español, Русский.

### ⌨️ Phím tắt & Thao tác

| Phím / Thao tác | Chức năng |
| :--- | :--- |
| **`Win + R`** | Mở hộp thoại Run / Đóng nếu đang mở (Khôi phục nếu đang ẩn dưới Taskbar) |
| **`Enter`** | Thực thi lệnh đang có trên thanh nhập liệu |
| **`Esc`** | Đóng hộp thoại Run ngay lập tức |
| **`↑` / `↓`** | Di chuyển và chọn nhanh các lệnh trong danh sách lịch sử |
| **Click chuột vào dòng lịch sử** | Điền lệnh đó xuống thanh nhập liệu |
| **Nhấp đúp vào dòng lịch sử** | Chạy ngay lập tức lệnh đó |
| **Click icon Run trên Taskbar** | Thu nhỏ (Ẩn) / Mở lại cửa sổ Run (hoặc Đóng hẳn tùy cài đặt) |

### ⚙️ Tùy chỉnh trong Cài đặt (Mod Settings)

1. **Ngôn ngữ hiển thị (Language):** Tự động theo Windows hoặc chọn ngôn ngữ cố định.
2. **Hành vi khi bấm icon Run trên Taskbar:** Chọn giữa *Ẩn / Mở lại cửa sổ* hoặc *Đóng hẳn cửa sổ*.
3. **Hiệu ứng làm mờ trong suốt (Blur / Acrylic):** Bật hoặc tắt nền kính mờ.
4. **Độ đậm nền Blur (0 - 255):** Điều chỉnh mức độ trong suốt của nền cửa sổ.
5. **Số mục lịch sử tối đa (0 - 5):** Số lượng lệnh gần đây hiển thị ở phần trên (đặt về `0` nếu chỉ muốn hiện 1 dòng nhập lệnh).
6. **Hiển thị thời gian chạy lệnh gần nhất:** Bật/tắt mốc thời gian bên cạnh biểu tượng đồng hồ.
7. **Để trống ô nhập khi mở:** Hiển thị dòng gợi ý thay vì tự điền sẵn lệnh cũ khi vừa mở.
8. **Tự động đóng khi bấm ra ngoài:** Tự động đóng hộp thoại Run khi click chuột ra ngoài cửa sổ.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: "Display Language / Ngôn ngữ hiển thị"
  $description: "Auto-detect system language or choose a fixed language. | Tự động theo hệ điều hành hoặc chọn ngôn ngữ cố định."
  $options:
  - auto: "Auto (System Default) / Tự động theo Windows"
  - en: "English"
  - vi: "Tiếng Việt (Vietnamese)"
  - ja: "日本語 (Japanese)"
  - zh_cn: "简体中文 (Simplified Chinese)"
  - zh_tw: "繁體中文 (Traditional Chinese)"
  - ko: "한국어 (Korean)"
  - fr: "Français (French)"
  - de: "Deutsch (German)"
  - es: "Español (Spanish)"
  - ru: "Русский (Russian)"
- taskbarClickAction: minimize
  $name: "Taskbar Icon Click Action / Hành vi khi bấm icon Run trên Taskbar"
  $description: "Action when clicking the Run icon on the Taskbar while open. | Hành động khi bấm vào biểu tượng Run dưới Taskbar lúc cửa sổ đang mở."
  $options:
  - minimize: "Minimize & Restore / Ẩn & Mở lại cửa sổ Run"
  - close: "Close Window / Đóng hẳn cửa sổ Run"
- enableBlur: true
  $name: "Acrylic Blur Effect / Hiệu ứng làm mờ trong suốt (Blur)"
  $description: "Enable or disable the translucent Acrylic blur background. | Bật hoặc tắt hiệu ứng làm mờ nền trong suốt toàn cửa sổ."
- blurOpacity: 165
  $name: "Blur Opacity (0 - 255) / Độ đậm nền Blur (0 - 255)"
  $description: "Lower values make the window more transparent (default is 165). | Giá trị càng nhỏ càng trong suốt (mặc định là 165)."
- maxHistoryItems: 5
  $name: "Max History Items (0 - 5) / Số mục lịch sử tối đa (0 - 5)"
  $description: "Maximum number of recent commands shown in the top list. | Số lượng lệnh gần đây hiển thị ở phần phía trên."
- showTimestamp: true
  $name: "Show Last Executed Time / Hiển thị thời gian chạy lệnh gần nhất"
  $description: "Display the execution timestamp next to the clock icon on each history row. | Hiển thị thời gian thực thi ngay bên cạnh biểu tượng đồng hồ ở mỗi dòng lịch sử."
- clearInitialInput: true
  $name: "Clear Input on Open / Để trống ô nhập khi mở"
  $description: "Show multilingual placeholder text on open instead of pre-filling the last command. | Hiển thị dòng gợi ý đa ngôn ngữ khi mới mở thay vì điền sẵn lệnh cũ."
- closeOnClickOutside: false
  $name: "Close on Click Outside / Tự động đóng khi bấm ra ngoài"
  $description: "Automatically close the Run dialog when clicking outside the window. | Tự động đóng hộp thoại Run khi click chuột ra ngoài cửa sổ."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commdlg.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <imm.h>
#include <cwchar>
#include <string>
#include <vector>
#include <algorithm>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif

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
using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
using SetForegroundWindow_t = BOOL(WINAPI*)(HWND);
using SwitchToThisWindow_t = void(WINAPI*)(HWND, BOOL);

ShowWindow_t ShowWindow_Orig = nullptr;
SetForegroundWindow_t SetForegroundWindow_Orig = nullptr;
SwitchToThisWindow_t SwitchToThisWindow_Orig = nullptr;

struct LocalizedStrings {
    const wchar_t* placeholder;
    const wchar_t* browseBtn;
    const wchar_t* runBtn;
    const wchar_t* exitBtn;
    const wchar_t* todayPrefix;
    const wchar_t* browseTitle;
    const wchar_t* browseFilter;
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
} g_settings;

LocalizedStrings g_i18n = {
    L"Type the name of a command to run",
    L"Browse",
    L"Run",
    L"Exit",
    L"Today",
    L"Select an executable program",
    L"Programs (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0All Files (*.*)\0*.*\0\0"
};

ULONG_PTR g_gdiplusToken = 0;

void UpdateLocalization() {
    std::wstring lang = g_settings.language;
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
        g_i18n = {
            L"Nhập tên lệnh hoặc chương trình để chạy",
            L"Duyệt",
            L"Chạy",
            L"Thoát",
            L"Hôm nay",
            L"Chọn chương trình (.exe) để đưa vào ô nhập",
            L"Chương trình (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tất cả tệp (*.*)\0*.*\0\0"
        };
    } else if (lang == L"ja") {
        g_i18n = {
            L"実行するコマンドの名前を入力してください",
            L"参照",
            L"実行",
            L"閉じる",
            L"今日",
            L"実行するプログラムを選択",
            L"プログラム (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0すべてのファイル (*.*)\0*.*\0\0"
        };
    } else if (lang == L"zh_cn") {
        g_i18n = {
            L"输入要运行的命令名称",
            L"浏览",
            L"运行",
            L"退出",
            L"今天",
            L"选择要运行的程序",
            L"程序 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0所有文件 (*.*)\0*.*\0\0"
        };
    } else if (lang == L"zh_tw") {
        g_i18n = {
            L"輸入要執行的命令名稱",
            L"瀏覽",
            L"執行",
            L"退出",
            L"今天",
            L"選擇要執行的程式",
            L"程式 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0所有檔案 (*.*)\0*.*\0\0"
        };
    } else if (lang == L"ko") {
        g_i18n = {
            L"실행할 명령의 이름을 입력하세요",
            L"찾아보기",
            L"실행",
            L"닫기",
            L"오늘",
            L"실행할 프로그램 선택",
            L"프로그램 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0모든 파일 (*.*)\0*.*\0\0"
        };
    } else if (lang == L"fr") {
        g_i18n = {
            L"Entrez le nom d'une commande à exécuter",
            L"Parcourir",
            L"Exécuter",
            L"Quitter",
            L"Aujourd'hui",
            L"Sélectionner un programme",
            L"Programmes (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tous les fichiers (*.*)\0*.*\0\0"
        };
    } else if (lang == L"de") {
        g_i18n = {
            L"Geben Sie den Namen eines Befehls ein",
            L"Durchsuchen",
            L"Ausführen",
            L"Schließen",
            L"Heute",
            L"Programm auswählen",
            L"Programme (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Alle Dateien (*.*)\0*.*\0\0"
        };
    } else if (lang == L"es") {
        g_i18n = {
            L"Escriba el nombre de un comando para ejecutar",
            L"Examinar",
            L"Ejecutar",
            L"Salir",
            L"Hoy",
            L"Seleccionar un programa",
            L"Programas (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Todos los archivos (*.*)\0*.*\0\0"
        };
    } else if (lang == L"ru") {
        g_i18n = {
            L"Введите имя команды для выполнения",
            L"Обзор",
            L"Выполнить",
            L"Выход",
            L"Сегодня",
            L"Выберите программу",
            L"Программы (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Все файлы (*.*)\0*.*\0\0"
        };
    } else {
        g_i18n = {
            L"Type the name of a command to run",
            L"Browse",
            L"Run",
            L"Exit",
            L"Today",
            L"Select an executable program",
            L"Programs (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0All Files (*.*)\0*.*\0\0"
        };
    }
}

void LoadSettings() {
    PCWSTR langSetting = Wh_GetStringSetting(L"language");
    g_settings.language = langSetting ? langSetting : L"auto";
    if (langSetting) Wh_FreeStringSetting(langSetting);

    PCWSTR tbAction = Wh_GetStringSetting(L"taskbarClickAction");
    g_settings.taskbarCloseInsteadOfMinimize = (tbAction && wcscmp(tbAction, L"close") == 0);
    if (tbAction) Wh_FreeStringSetting(tbAction);

    g_settings.enableBlur = Wh_GetIntSetting(L"enableBlur") != 0;
    g_settings.blurOpacity = std::clamp(Wh_GetIntSetting(L"blurOpacity"), 20, 250);
    g_settings.maxHistoryItems = std::clamp(Wh_GetIntSetting(L"maxHistoryItems"), 0, 5);
    g_settings.showTimestamp = Wh_GetIntSetting(L"showTimestamp") != 0;
    g_settings.clearInitialInput = Wh_GetIntSetting(L"clearInitialInput") != 0;
    g_settings.closeOnClickOutside = Wh_GetIntSetting(L"closeOnClickOutside") != 0;
    UpdateLocalization();
}

// Kích thước chuẩn 720px
constexpr int kDlgWidth       = 720;
constexpr int kRowHeight      = 46;
constexpr int kListPadTop     = 10;
constexpr int kListPadBottom  = 10;
constexpr int kInputBarHeight = 66;
constexpr UINT_PTR kTimerCaret = 9010;

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
    HWND hCancelBtn = nullptr;

    HFONT hFontEditCtl = nullptr;
    HICON hRunIcon = nullptr;
    std::vector<HistoryItem> history;

    int topAreaHeight = 0;
    int totalHeight = kInputBarHeight;
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
};

HWND g_hActiveRunDlg = nullptr;
RunDialogState* g_activeState = nullptr;
HHOOK g_hKeyHook = nullptr;
ULONGLONG g_openTick = 0;
ULONGLONG g_lastToggleTick = 0;
bool g_winRReleasedSinceOpen = false;
bool g_internalActivation = false;

void ApplyWindowBlur(HWND hDlg);
void CloseRunDialog(HWND hDlg);
void SyncInputToNativeCombo(RunDialogState* state, const wchar_t* textOverride = nullptr);

// Thu nhỏ (Ẩn) cửa sổ Run xuống Taskbar nhưng vẫn giữ nguyên icon trên Taskbar
void MinimizeRunDialog(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    if (GetTickCount64() - g_lastToggleTick < 250) return;

    if (g_settings.taskbarCloseInsteadOfMinimize) {
        CloseRunDialog(state->hDlg);
        return;
    }

    g_lastToggleTick = GetTickCount64();

    if (!IsIconic(state->hDlg)) {
        RECT rc = {};
        GetWindowRect(state->hDlg, &rc);
        if (rc.left > -10000 && rc.top > -10000) {
            state->normalX = rc.left;
            state->normalY = rc.top;
        }
    }

    g_internalActivation = true;
    if (ShowWindow_Orig) {
        ShowWindow_Orig(state->hDlg, SW_MINIMIZE);
    } else {
        ShowWindow(state->hDlg, SW_MINIMIZE);
    }
    g_internalActivation = false;
}

// Khôi phục (Mở lại) cửa sổ Run từ Taskbar về đúng vị trí và kích thước chuẩn
void RestoreRunDialog(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    if (GetTickCount64() - g_lastToggleTick < 250) return;
    g_lastToggleTick = GetTickCount64();

    g_internalActivation = true;
    if (ShowWindow_Orig) {
        ShowWindow_Orig(state->hDlg, SW_RESTORE);
    } else {
        ShowWindow(state->hDlg, SW_RESTORE);
    }

    // Đảm bảo không có cờ WS_CAPTION sau khi Restore
    LONG_PTR style = GetWindowLongPtrW(state->hDlg, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
    style |= (WS_SYSMENU | WS_MINIMIZEBOX);
    SetWindowLongPtrW(state->hDlg, GWL_STYLE, style);

    SetWindowPos(state->hDlg, HWND_TOP,
                 state->normalX, state->normalY,
                 kDlgWidth, state->totalHeight,
                 SWP_SHOWWINDOW | SWP_FRAMECHANGED);

    ApplyWindowBlur(state->hDlg);

    if (SetForegroundWindow_Orig) {
        SetForegroundWindow_Orig(state->hDlg);
    } else {
        SetForegroundWindow(state->hDlg);
    }

    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
        SetFocus(state->hModernEdit);
    }
    state->caretVisible = true;
    InvalidateRect(state->hDlg, nullptr, TRUE);
    g_internalActivation = false;
}

// Mở hộp thoại chọn file (.exe) và đưa toàn bộ đường dẫn lên thanh nhập (chưa tự động chạy)
void OpenBrowseExecutableDialog(RunDialogState* state) {
    if (!state || !state->hDlg || state->isBrowsingFile) return;
    state->isBrowsingFile = true;

    wchar_t filePath[MAX_PATH] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = state->hDlg;
    ofn.lpstrFilter = g_i18n.browseFilter;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = g_i18n.browseTitle;
    ofn.lpstrDefExt = L"exe";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;

    if (GetOpenFileNameW(&ofn) && filePath[0] != L'\0') {
        std::wstring formattedPath = filePath;
        // Nếu đường dẫn có khoảng trắng, bọc trong dấu ngoặc kép chuẩn của Windows Run
        if (formattedPath.find(L' ') != std::wstring::npos && formattedPath.front() != L'"') {
            formattedPath = L"\"" + formattedPath + L"\"";
        }

        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            SetWindowTextW(state->hModernEdit, formattedPath.c_str());
            SendMessageW(state->hModernEdit, EM_SETSEL, formattedPath.size(), formattedPath.size());
        }
        SyncInputToNativeCombo(state, formattedPath.c_str());
        state->selectedRow = -1;
    }

    state->isBrowsingFile = false;

    if (IsWindow(state->hDlg)) {
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            SetFocus(state->hModernEdit);
        }
        state->caretVisible = true;
        InvalidateRect(state->hDlg, nullptr, TRUE);
    }
}

// --- Quản lý lưu & đọc Thời gian chạy lệnh gần nhất ---

std::wstring NormalizeCmdKey(const std::wstring& cmd) {
    wchar_t buf[MAX_PATH] = {};
    wcsncpy_s(buf, cmd.c_str(), _TRUNCATE);
    PathRemoveBlanksW(buf);
    std::wstring res = buf;
    std::transform(res.begin(), res.end(), res.begin(), ::towlower);
    return res;
}

void SaveCommandTimestamp(const std::wstring& cmd) {
    std::wstring keyName = NormalizeCmdKey(cmd);
    if (keyName.empty()) return;

    HKEY hKey = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER,
                        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU_Timestamps",
                        0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        FILETIME ft = {};
        GetSystemTimeAsFileTime(&ft);
        RegSetValueExW(hKey, keyName.c_str(), 0, REG_QWORD, (const BYTE*)&ft, sizeof(ft));
        RegCloseKey(hKey);
    }
}

std::wstring FormatHistoryTimestamp(const FILETIME& ftUtc) {
    FILETIME ftLocal = {};
    SYSTEMTIME st = {}, now = {};
    FileTimeToLocalFileTime(&ftUtc, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &st);
    GetLocalTime(&now);

    wchar_t buf[64] = {};
    if (st.wYear == now.wYear && st.wMonth == now.wMonth && st.wDay == now.wDay) {
        wsprintfW(buf, L"%s, %02d:%02d", g_i18n.todayPrefix, st.wHour, st.wMinute);
    } else {
        wsprintfW(buf, L"%02d:%02d  %02d/%02d", st.wHour, st.wMinute, st.wDay, st.wMonth);
    }
    return buf;
}

std::wstring GetOrInitCommandTimestamp(const std::wstring& cmd, int rankIndex, const FILETIME& fallbackMruTime) {
    std::wstring keyName = NormalizeCmdKey(cmd);
    if (keyName.empty()) return L"";

    FILETIME ft = {};
    bool found = false;

    HKEY hKey = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER,
                        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU_Timestamps",
                        0, nullptr, 0, KEY_READ | KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        DWORD cb = sizeof(ft);
        DWORD type = 0;
        if (RegQueryValueExW(hKey, keyName.c_str(), nullptr, &type, (LPBYTE)&ft, &cb) == ERROR_SUCCESS &&
            cb == sizeof(ft) && (ft.dwLowDateTime != 0 || ft.dwHighDateTime != 0)) {
            found = true;
        } else {
            ULARGE_INTEGER uli = {};
            uli.LowPart  = fallbackMruTime.dwLowDateTime;
            uli.HighPart = fallbackMruTime.dwHighDateTime;
            if (uli.QuadPart == 0) {
                GetSystemTimeAsFileTime(&ft);
                uli.LowPart  = ft.dwLowDateTime;
                uli.HighPart = ft.dwHighDateTime;
            }
            if (rankIndex > 0) {
                ULONGLONG offset = (ULONGLONG)rankIndex * 180ULL * 10000000ULL;
                if (uli.QuadPart > offset) uli.QuadPart -= offset;
            }
            ft.dwLowDateTime  = uli.LowPart;
            ft.dwHighDateTime = uli.HighPart;
            RegSetValueExW(hKey, keyName.c_str(), 0, REG_QWORD, (const BYTE*)&ft, sizeof(ft));
            found = true;
        }
        RegCloseKey(hKey);
    }

    if (!found) {
        GetSystemTimeAsFileTime(&ft);
    }
    return FormatHistoryTimestamp(ft);
}

void CloseRunDialog(HWND hDlg) {
    if (!hDlg || !IsWindow(hDlg)) return;
    HWND hCancel = GetDlgItem(hDlg, IDCANCEL);
    if (hCancel) EnableWindow(hCancel, TRUE);
    PostMessageW(hDlg, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)hCancel);
    EndDialog(hDlg, IDCANCEL);
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

void SaveCommandToRunMRU(const std::wstring& cmd) {
    if (cmd.empty()) return;
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER,
                        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU",
                        0, nullptr, 0, KEY_READ | KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
        return;
    }

    wchar_t mruList[64] = {};
    DWORD cbList = sizeof(mruList);
    RegQueryValueExW(hKey, L"MRUList", nullptr, nullptr, (LPBYTE)mruList, &cbList);

    std::wstring mrul = mruList;
    std::wstring cmdWithSuffix = cmd + L"\\1";
    wchar_t targetChar = 0;

    for (wchar_t c : mrul) {
        wchar_t valName[2] = { c, L'\0' };
        wchar_t buf[MAX_PATH] = {};
        DWORD cb = sizeof(buf);
        if (RegQueryValueExW(hKey, valName, nullptr, nullptr, (LPBYTE)buf, &cb) == ERROR_SUCCESS) {
            std::wstring existing = buf;
            if (existing.size() >= 2 && existing.substr(existing.size() - 2) == L"\\1") {
                existing.resize(existing.size() - 2);
            }
            if (_wcsicmp(existing.c_str(), cmd.c_str()) == 0) {
                targetChar = c;
                break;
            }
        }
    }

    if (targetChar != 0) {
        mrul.erase(std::remove(mrul.begin(), mrul.end(), targetChar), mrul.end());
        mrul.insert(mrul.begin(), targetChar);
    } else {
        for (wchar_t c = L'a'; c <= L'z'; ++c) {
            if (mrul.find(c) == std::wstring::npos) {
                targetChar = c;
                break;
            }
        }
        if (targetChar == 0) {
            targetChar = mrul.empty() ? L'a' : mrul.back();
            if (!mrul.empty()) mrul.pop_back();
        }
        mrul.insert(mrul.begin(), targetChar);
    }

    if (mrul.size() > 26) mrul.resize(26);

    wchar_t valName[2] = { targetChar, L'\0' };
    RegSetValueExW(hKey, valName, 0, REG_SZ, (const BYTE*)cmdWithSuffix.c_str(),
                   (DWORD)((cmdWithSuffix.size() + 1) * sizeof(wchar_t)));
    RegSetValueExW(hKey, L"MRUList", 0, REG_SZ, (const BYTE*)mrul.c_str(),
                   (DWORD)((mrul.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);
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

    HWND hOk = state->hOkBtn ? state->hOkBtn : GetDlgItem(state->hDlg, IDOK);
    if (hOk) EnableWindow(hOk, TRUE);

    SendMessageW(state->hDlg, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)hOk);

    if (IsWindow(state->hDlg) && IsWindowVisible(state->hDlg)) {
        wchar_t expanded[MAX_PATH * 2] = {};
        ExpandEnvironmentStringsW(rawBuf, expanded, ARRAYSIZE(expanded));

        int argc = 0;
        LPWSTR* argv = CommandLineToArgvW(expanded, &argc);
        std::wstring file;
        std::wstring params;

        if (argv && argc > 0) {
            file = argv[0];
            LocalFree(argv);
            const wchar_t* p = expanded;
            if (*p == L'"') {
                ++p;
                while (*p && *p != L'"') ++p;
                if (*p == L'"') ++p;
            } else {
                while (*p && *p != L' ' && *p != L'\t') ++p;
            }
            while (*p == L' ' || *p == L'\t') ++p;
            params = p;
        } else {
            file = expanded;
        }

        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.hwnd = state->hDlg;
        sei.lpFile = file.c_str();
        sei.lpParameters = params.empty() ? nullptr : params.c_str();
        sei.nShow = SW_SHOWNORMAL;
        sei.fMask = SEE_MASK_DOENVSUBST;

        if (ShellExecuteExW(&sei)) {
            SaveCommandToRunMRU(rawBuf);
            CloseRunDialog(state->hDlg);
            return;
        }
    }

    if (IsWindow(state->hDlg)) {
        state->isTriggeringRun = false;
    }
}

LRESULT CALLBACK LowLevelKeyProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_hActiveRunDlg && IsWindow(g_hActiveRunDlg)) {
        auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isKeyUp   = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (kbd->vkCode == 'R' && isKeyUp) {
            g_winRReleasedSinceOpen = true;
        }

        if (isKeyDown && kbd->vkCode == 'R') {
            bool winDown = (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);
            if (winDown && (g_winRReleasedSinceOpen || (GetTickCount64() - g_openTick > 350))) {
                if (IsIconic(g_hActiveRunDlg) && g_activeState) {
                    RestoreRunDialog(g_activeState);
                } else {
                    CloseRunDialog(g_hActiveRunDlg);
                }
                return 1;
            }
        }

        if (isKeyDown && kbd->vkCode == VK_ESCAPE) {
            if (g_activeState && g_activeState->isBrowsingFile) {
                return CallNextHookEx(g_hKeyHook, nCode, wParam, lParam);
            }
            HWND hFg = GetForegroundWindow();
            if (hFg == g_hActiveRunDlg || IsChild(g_hActiveRunDlg, hFg)) {
                CloseRunDialog(g_hActiveRunDlg);
                return 1;
            }
        }
    }
    return CallNextHookEx(g_hKeyHook, nCode, wParam, lParam);
}

void ApplyWindowBlur(HWND hDlg) {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return;

    auto pSetWCA = reinterpret_cast<SetWindowCompositionAttribute_t>(
        GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
    if (!pSetWCA) return;

    ACCENT_POLICY policy = {};
    if (g_settings.enableBlur) {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        DWORD alpha = static_cast<DWORD>(g_settings.blurOpacity) & 0xFF;
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
        { L"claude",       L"Claude" },
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
            wsprintfW(subBlock, L"\\StringFileInfo\\%04x%04x\\%s",
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

    int maxItems = g_settings.maxHistoryItems;
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
                    item.timeStr = GetOrInitCommandTimestamp(text, (int)state->history.size(), ftRunMruWrite);
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
            if (RegQueryValueExW(hKey, L"MRUList", nullptr, nullptr, (LPBYTE)mruList, &cbList) == ERROR_SUCCESS) {
                for (int i = 0; mruList[i] != L'\0' && (int)state->history.size() < maxItems; ++i) {
                    wchar_t valName[2] = { mruList[i], L'\0' };
                    wchar_t cmdBuf[MAX_PATH] = {};
                    DWORD cbCmd = sizeof(cmdBuf);
                    if (RegQueryValueExW(hKey, valName, nullptr, nullptr, (LPBYTE)cmdBuf, &cbCmd) == ERROR_SUCCESS) {
                        size_t len = wcslen(cmdBuf);
                        if (len >= 2 && cmdBuf[len - 2] == L'\\' && cmdBuf[len - 1] == L'1') {
                            cmdBuf[len - 2] = L'\0';
                        }
                        if (cmdBuf[0] != L'\0') {
                            HistoryItem item;
                            item.cmd = cmdBuf;
                            ResolveCommandMetadata(cmdBuf, &item.hIcon, &item.appName);
                            item.timeStr = GetOrInitCommandTimestamp(cmdBuf, (int)state->history.size(), ftRunMruWrite);
                            state->history.push_back(std::move(item));
                        }
                    }
                }
            }
            RegCloseKey(hKey);
        }
    }
}

// Bố cục 3 nút ở góc dưới bên phải: [Duyệt 📁]  [Chạy ↵]  [Thoát ✕]
RECT GetBrowseButtonRect(const RunDialogState* state) {
    int top = state->topAreaHeight + (kInputBarHeight - 36) / 2;
    RECT rc = { kDlgWidth - 332, top, kDlgWidth - 226, top + 36 };
    return rc;
}

RECT GetRunButtonRect(const RunDialogState* state) {
    int top = state->topAreaHeight + (kInputBarHeight - 36) / 2;
    RECT rc = { kDlgWidth - 218, top, kDlgWidth - 118, top + 36 };
    return rc;
}

RECT GetCloseButtonRect(const RunDialogState* state) {
    int top = state->topAreaHeight + (kInputBarHeight - 36) / 2;
    RECT rc = { kDlgWidth - 110, top, kDlgWidth - 14, top + 36 };
    return rc;
}

RECT GetModernEditRect(const RunDialogState* state) {
    int top = state->topAreaHeight + (kInputBarHeight - 28) / 2;
    RECT rc = { 60, top, kDlgWidth - 344, top + 28 };
    return rc;
}

int HitTestHistoryRow(const RunDialogState* state, POINT pt) {
    if (state->history.empty() || pt.y < kListPadTop || pt.y >= state->topAreaHeight - kListPadBottom) {
        return -1;
    }
    if (pt.x < 10 || pt.x > kDlgWidth - 10) return -1;
    int idx = (pt.y - kListPadTop) / kRowHeight;
    if (idx >= 0 && idx < (int)state->history.size()) {
        return idx;
    }
    return -1;
}

// --- Các hàm hỗ trợ vẽ đồ họa GDI+ 32-bit sắc nét ---

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

void DrawHistoryClockVector(Gdiplus::Graphics& g, float cx, float cy, Gdiplus::Color col) {
    Gdiplus::Pen pen(col, 1.5f);
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);
    pen.SetLineJoin(Gdiplus::LineJoinRound);

    float r = 7.5f;
    g.DrawEllipse(&pen, cx - r, cy - r, r * 2.0f, r * 2.0f);
    g.DrawLine(&pen, cx, cy - 4.2f, cx, cy + 0.5f);
    g.DrawLine(&pen, cx, cy + 0.5f, cx + 3.5f, cy + 2.5f);
}

void DrawFolderBrowseVector(Gdiplus::Graphics& g, float cx, float cy, Gdiplus::Color col) {
    Gdiplus::Pen pen(col, 1.5f);
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);
    pen.SetLineJoin(Gdiplus::LineJoinRound);

    float left   = cx - 6.5f;
    float right  = cx + 6.5f;
    float top    = cy - 4.5f;
    float bottom = cy + 5.0f;

    // Thân thư mục
    g.DrawLine(&pen, left, top + 2.0f, right, top + 2.0f);
    g.DrawLine(&pen, right, top + 2.0f, right, bottom);
    g.DrawLine(&pen, right, bottom, left, bottom);
    g.DrawLine(&pen, left, bottom, left, top);
    // Gờ tab thư mục phía trên
    g.DrawLine(&pen, left, top, left + 4.5f, top);
    g.DrawLine(&pen, left + 4.5f, top, left + 6.2f, top + 2.0f);
}

void DrawEnterArrowVector(Gdiplus::Graphics& g, float cx, float cy, Gdiplus::Color col) {
    Gdiplus::Pen pen(col, 1.7f);
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);
    pen.SetLineJoin(Gdiplus::LineJoinRound);

    g.DrawLine(&pen, cx + 4.5f, cy - 4.5f, cx + 4.5f, cy + 2.0f);
    g.DrawLine(&pen, cx + 4.5f, cy + 2.0f, cx - 4.5f, cy + 2.0f);
    g.DrawLine(&pen, cx - 4.5f, cy + 2.0f, cx - 1.5f, cy - 1.0f);
    g.DrawLine(&pen, cx - 4.5f, cy + 2.0f, cx - 1.5f, cy + 5.0f);
}

void DrawCloseCrossVector(Gdiplus::Graphics& g, float cx, float cy, Gdiplus::Color col) {
    Gdiplus::Pen pen(col, 1.7f);
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);

    float s = 4.2f;
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
        RECT rcBar = { 0, state->topAreaHeight, kDlgWidth, state->totalHeight };
        InvalidateRect(state->hDlg, &rcBar, FALSE);
    }

    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, ModernEditSubclassProc, uIdSubclass);
    }
    return res;
}

LRESULT CALLBACK RunDlgSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto* state = reinterpret_cast<RunDialogState*>(dwRefData);

    switch (uMsg) {
        case WM_NCCALCSIZE:
            return 0;

        // Trả về TRUE trực tiếp và KHÔNG gọi DefWindowProcW để hệ thống tuyệt đối không bao giờ vẽ thanh tiêu đề cổ điển
        case WM_NCACTIVATE:
            return TRUE;

        case WM_NCPAINT:
            return 0;

        // Giữ loại bỏ hoàn toàn WS_CAPTION để win32kfull.sys không thể vẽ thanh tiêu đề khi click ra hình nền Desktop
        case WM_STYLECHANGING: {
            if (wParam == GWL_STYLE && lParam) {
                auto* ss = reinterpret_cast<STYLESTRUCT*>(lParam);
                ss->styleNew &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
                ss->styleNew |= (WS_SYSMENU | WS_MINIMIZEBOX);
            }
            break;
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
                    wp->cx = kDlgWidth;
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
                    RECT rcBar = { 0, state->topAreaHeight, kDlgWidth, state->totalHeight };
                    InvalidateRect(hWnd, &rcBar, FALSE);
                }
                return 0;
            }
            break;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == 9901 && (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == EN_UPDATE)) {
                if (state) {
                    state->caretVisible = true;
                    SyncInputToNativeCombo(state);
                    RECT rcBar = { 0, state->topAreaHeight, kDlgWidth, state->totalHeight };
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
            } else if (LOWORD(wParam) == WA_INACTIVE && g_settings.closeOnClickOutside && state && !state->isBrowsingFile) {
                if (GetTickCount64() - g_openTick > 300) {
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

            if (IsIconic(hWnd)) {
                EndPaint(hWnd, &ps);
                return 0;
            }

            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            int w = rcClient.right;
            int h = rcClient.bottom;

            if (w > 0 && h > 0) {
                Gdiplus::Bitmap canvas(w, h, PixelFormat32bppARGB);
                Gdiplus::Graphics g(&canvas);
                g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
                g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

                if (g_settings.enableBlur) {
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
                Gdiplus::Font fontCmd(&fontFamily, 15.5f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
                Gdiplus::Font fontApp(&fontFamily, 14.5f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                Gdiplus::Font fontTime(&fontFamily, 13.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                Gdiplus::Font fontInput(&fontFamily, 15.5f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                Gdiplus::Font fontBtn(&fontFamily, 14.5f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

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

                // 1. Vẽ 5 mục lịch sử + Tên App + Thời gian chạy gần nhất
                for (size_t i = 0; i < state->history.size(); ++i) {
                    float rowTop = (float)(kListPadTop + (int)i * kRowHeight);
                    float rowH   = (float)kRowHeight;

                    if ((int)i == state->hoveredRow || (int)i == state->selectedRow) {
                        Gdiplus::RectF rcHl(12.0f, rowTop + 2.0f, (float)w - 24.0f, rowH - 4.0f);
                        FillAndStrokeRoundRect(g, rcHl, 8.0f,
                                               Gdiplus::Color(65, 255, 255, 255),
                                               Gdiplus::Color(55, 255, 255, 255), 1.0f);
                    }

                    float rightLimit = (float)(w - 52);

                    if (g_settings.showTimestamp && !state->history[i].timeStr.empty()) {
                        Gdiplus::RectF rcTime((float)(w - 220), rowTop, 168.0f, rowH);
                        g.DrawString(state->history[i].timeStr.c_str(), -1, &fontTime, rcTime, &fmtRight, &brTime);
                        float timeW = MeasureGdiPlusStringWidth(g, state->history[i].timeStr.c_str(),
                                                                (int)state->history[i].timeStr.size(), fontTime);
                        rightLimit = (float)(w - 64) - timeW;
                    }

                    const std::wstring& cmdStr = state->history[i].cmd;
                    float cmdW = MeasureGdiPlusStringWidth(g, cmdStr.c_str(), (int)cmdStr.size(), fontCmd);
                    Gdiplus::RectF rcCmd(60.0f, rowTop, (std::max)(40.0f, rightLimit - 60.0f), rowH);
                    g.DrawString(cmdStr.c_str(), -1, &fontCmd, rcCmd, &fmtLeft, &brWhite);

                    if (!state->history[i].appName.empty()) {
                        float appX = 60.0f + cmdW;
                        if (appX + 30.0f < rightLimit) {
                            std::wstring suffix = L"  -  " + state->history[i].appName;
                            Gdiplus::RectF rcApp(appX, rowTop, rightLimit - appX, rowH);
                            g.DrawString(suffix.c_str(), -1, &fontApp, rcApp, &fmtLeft, &brSubtle);
                        }
                    }

                    DrawHistoryClockVector(g, (float)(w - 32), rowTop + rowH * 0.5f, Gdiplus::Color(210, 190, 198, 210));
                }

                if (state->topAreaHeight > 0) {
                    Gdiplus::Pen divPen(Gdiplus::Color(48, 255, 255, 255), 1.0f);
                    g.DrawLine(&divPen, 0, state->topAreaHeight, w, state->topAreaHeight);
                }

                // 2. Vẽ ô nhập lệnh (tự cuộn ngang mượt mà khi đường dẫn .exe từ nút Duyệt dài hơn khung nhập)
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
                float scrollOffsetX = 0.0f;
                if (caretDist > rcEdit.Width - 8.0f) {
                    scrollOffsetX = caretDist - (rcEdit.Width - 8.0f);
                }

                Gdiplus::GraphicsState clipState = g.Save();
                g.SetClip(rcEdit);

                if (textLen == 0) {
                    g.DrawString(g_i18n.placeholder, -1, &fontInput, rcEdit, &fmtLeft, &brPlaceholder);
                } else {
                    if (selStart != selEnd) {
                        DWORD s0 = (std::min)(selStart, selEnd);
                        DWORD s1 = (std::max)(selStart, selEnd);
                        float x0 = MeasureGdiPlusStringWidth(g, displayStr.c_str(), (int)s0, fontInput) - scrollOffsetX;
                        float x1 = MeasureGdiPlusStringWidth(g, displayStr.c_str(), (int)s1, fontInput) - scrollOffsetX;
                        float hlX = rcEdit.X + x0;
                        float hlW = x1 - x0;
                        if (hlW > 0.0f) {
                            Gdiplus::SolidBrush selBr(Gdiplus::Color(150, 0, 120, 215));
                            g.FillRectangle(&selBr, hlX, rcEdit.Y + 2.0f, hlW, rcEdit.Height - 4.0f);
                        }
                    }
                    Gdiplus::RectF rcScrolledText(rcEdit.X - scrollOffsetX, rcEdit.Y,
                                                  rcEdit.Width + scrollOffsetX + 2000.0f, rcEdit.Height);
                    g.DrawString(displayStr.c_str(), textLen, &fontInput, rcScrolledText, &fmtLeftNoTrim, &brWhite);
                }

                bool editFocused = (GetFocus() == state->hModernEdit) || (GetForegroundWindow() == hWnd && !state->isBrowsingFile);
                if (editFocused && state->caretVisible) {
                    float caretX = rcEdit.X + caretDist - scrollOffsetX + 1.0f;
                    caretX = std::clamp(caretX, rcEdit.X + 1.0f, rcEdit.X + rcEdit.Width - 2.0f);
                    Gdiplus::Pen caretPen(Gdiplus::Color(245, 255, 255, 255), 1.8f);
                    g.DrawLine(&caretPen, caretX, rcEdit.Y + 4.0f, caretX, rcEdit.Y + rcEdit.Height - 4.0f);
                }

                g.Restore(clipState);

                // 3. Nút "Duyệt" (Browse .exe)
                RECT rBrowse = GetBrowseButtonRect(state);
                Gdiplus::RectF rcBrowseBtn((float)rBrowse.left, (float)rBrowse.top,
                                           (float)(rBrowse.right - rBrowse.left), (float)(rBrowse.bottom - rBrowse.top));
                Gdiplus::Color browseFill = state->browsePressed ? Gdiplus::Color(215, 42, 50, 62)
                                          : (state->browseHovered ? Gdiplus::Color(230, 68, 80, 98)
                                                                  : Gdiplus::Color(185, 48, 56, 70));
                Gdiplus::Color browseBorder = state->browseHovered ? Gdiplus::Color(190, 155, 175, 205)
                                                                   : Gdiplus::Color(115, 110, 125, 145);
                FillAndStrokeRoundRect(g, rcBrowseBtn, 8.0f, browseFill, browseBorder, 1.0f);

                Gdiplus::RectF rcBrowseText(rcBrowseBtn.X + 12.0f, rcBrowseBtn.Y, rcBrowseBtn.Width - 34.0f, rcBrowseBtn.Height);
                g.DrawString(g_i18n.browseBtn, -1, &fontBtn, rcBrowseText, &fmtLeft, &brWhite);
                DrawFolderBrowseVector(g, rcBrowseBtn.X + rcBrowseBtn.Width - 18.0f, rcBrowseBtn.Y + rcBrowseBtn.Height * 0.5f,
                                       Gdiplus::Color(255, 255, 255, 255));

                // 4. Nút "Chạy ↵" nổi bật
                RECT rBtn = GetRunButtonRect(state);
                Gdiplus::RectF rcRunBtn((float)rBtn.left, (float)rBtn.top,
                                        (float)(rBtn.right - rBtn.left), (float)(rBtn.bottom - rBtn.top));
                Gdiplus::Color runFill = state->btnPressed ? Gdiplus::Color(210, 0, 96, 175)
                                       : (state->btnHovered ? Gdiplus::Color(235, 24, 142, 238)
                                                            : Gdiplus::Color(205, 0, 120, 212));
                Gdiplus::Color runBorder = state->btnHovered ? Gdiplus::Color(180, 130, 205, 255)
                                                             : Gdiplus::Color(110, 90, 175, 245);
                FillAndStrokeRoundRect(g, rcRunBtn, 8.0f, runFill, runBorder, 1.0f);

                Gdiplus::RectF rcRunText(rcRunBtn.X + 14.0f, rcRunBtn.Y, rcRunBtn.Width - 36.0f, rcRunBtn.Height);
                g.DrawString(g_i18n.runBtn, -1, &fontBtn, rcRunText, &fmtLeft, &brWhite);
                DrawEnterArrowVector(g, rcRunBtn.X + rcRunBtn.Width - 20.0f, rcRunBtn.Y + rcRunBtn.Height * 0.5f,
                                     Gdiplus::Color(255, 255, 255, 255));

                // 5. Nút "Thoát ✕" nổi bật
                RECT rClose = GetCloseButtonRect(state);
                Gdiplus::RectF rcCloseBtn((float)rClose.left, (float)rClose.top,
                                          (float)(rClose.right - rClose.left), (float)(rClose.bottom - rClose.top));
                Gdiplus::Color closeFill = state->closePressed ? Gdiplus::Color(215, 155, 30, 24)
                                         : (state->closeHovered ? Gdiplus::Color(235, 205, 46, 36)
                                                                : Gdiplus::Color(165, 92, 42, 50));
                Gdiplus::Color closeBorder = state->closeHovered ? Gdiplus::Color(190, 255, 130, 125)
                                                                 : Gdiplus::Color(110, 175, 95, 105);
                FillAndStrokeRoundRect(g, rcCloseBtn, 8.0f, closeFill, closeBorder, 1.0f);

                Gdiplus::RectF rcCloseText(rcCloseBtn.X + 12.0f, rcCloseBtn.Y, rcCloseBtn.Width - 32.0f, rcCloseBtn.Height);
                g.DrawString(g_i18n.exitBtn, -1, &fontBtn, rcCloseText, &fmtLeft, &brWhite);
                DrawCloseCrossVector(g, rcCloseBtn.X + rcCloseBtn.Width - 18.0f, rcCloseBtn.Y + rcCloseBtn.Height * 0.5f,
                                     Gdiplus::Color(255, 255, 255, 255));

                // 6. Chuyển đổi sang Premultiplied Alpha 32-bit chuẩn của DWM
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

                    for (size_t i = 0; i < state->history.size(); ++i) {
                        if (state->history[i].hIcon) {
                            int iconY = kListPadTop + (int)i * kRowHeight + (kRowHeight - 24) / 2;
                            DrawIconEx(hMemDC, 24, iconY, state->history[i].hIcon, 24, 24, 0, nullptr, DI_NORMAL);
                        }
                    }

                    if (state->hRunIcon) {
                        int runIconY = state->topAreaHeight + (kInputBarHeight - 24) / 2;
                        DrawIconEx(hMemDC, 24, runIconY, state->hRunIcon, 24, 24, 0, nullptr, DI_NORMAL);
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
            KillTimer(hWnd, kTimerCaret);
            if (g_hActiveRunDlg == hWnd) {
                g_hActiveRunDlg = nullptr;
                g_activeState = nullptr;
            }
            if (g_hKeyHook) {
                UnhookWindowsHookEx(g_hKeyHook);
                g_hKeyHook = nullptr;
            }
            RemovePropW(hWnd, L"WindhawkModernRunApplied");
            if (state) {
                for (auto& item : state->history) {
                    if (item.hIcon) DestroyIcon(item.hIcon);
                }
                if (state->hModernEdit && IsWindow(state->hModernEdit)) DestroyWindow(state->hModernEdit);
                if (state->hFontEditCtl) DeleteObject(state->hFontEditCtl);
                if (state->hRunIcon)     DestroyIcon(state->hRunIcon);
                delete state;
            }
            RemoveWindowSubclass(hWnd, RunDlgSubclassProc, uIdSubclass);
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool IsRunDialog(HWND hDlg) {
    if (!hDlg || !IsWindow(hDlg)) return false;

    // 1. Nếu đang mở hộp thoại Duyệt file (Browse) hoặc cửa sổ Run đã mở sẵn thì không can thiệp
    if (g_activeState && g_activeState->isBrowsingFile && hDlg != g_hActiveRunDlg) {
        return false;
    }
    if (g_hActiveRunDlg && IsWindow(g_hActiveRunDlg) && hDlg != g_hActiveRunDlg) {
        return false;
    }

    wchar_t cls[32] = {};
    if (!GetClassNameW(hDlg, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"#32770") != 0) return false;

    // 2. Hộp thoại Run thật của Windows luôn có ComboBox ID = 12298 (0x300A) và nút Browse ID = 12288 (0x3000)
    // Hộp thoại Chọn File (Hình 2) không có 2 ID này nên sẽ giữ nguyên giao diện mặc định
    HWND hRunCombo  = GetDlgItem(hDlg, 12298);
    HWND hRunBrowse = GetDlgItem(hDlg, 12288);
    HWND hOk        = GetDlgItem(hDlg, IDOK);
    HWND hCancel    = GetDlgItem(hDlg, IDCANCEL);
    if (!hRunCombo || !hRunBrowse || !hOk || !hCancel) return false;

    // 3. Loại trừ các lớp giao diện của hộp thoại Chọn File Explorer (DUIViewWndClassName, SHELLDLL_DefView)
    if (FindWindowExW(hDlg, nullptr, L"DUIViewWndClassName", nullptr) != nullptr ||
        FindWindowExW(hDlg, nullptr, L"SHELLDLL_DefView", nullptr) != nullptr ||
        FindWindowExW(hDlg, nullptr, L"ComboBoxEx32", nullptr) != nullptr ||
        FindWindowExW(hDlg, nullptr, L"SysTabControl32", nullptr) != nullptr ||
        FindWindowExW(hDlg, nullptr, L"SysListView32", nullptr) != nullptr ||
        FindWindowExW(hDlg, nullptr, L"SysTreeView32", nullptr) != nullptr ||
        FindWindowExW(hDlg, nullptr, L"DirectUIHWND", nullptr) != nullptr) {
        return false;
    }
    return true;
}

void TransformRunDialog(HWND hDlg) {
    if (!IsRunDialog(hDlg)) return;
    if (GetPropW(hDlg, L"WindhawkModernRunApplied")) return;
    SetPropW(hDlg, L"WindhawkModernRunApplied", (HANDLE)1);

    LoadSettings();

    auto* state = new RunDialogState();
    state->hDlg = hDlg;
    state->hFontEditCtl = CreateFontW(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    // XÓA BỎ HOÀN TOÀN WS_CAPTION | WS_DLGFRAME | WS_BORDER để hệ thống không bao giờ vẽ thanh tiêu đề cổ điển
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

    ApplyWindowBlur(hDlg);

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
                state->hCancelBtn = hChild;
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
    int histCount = (int)state->history.size();
    state->topAreaHeight = (histCount > 0) ? (kListPadTop + histCount * kRowHeight + kListPadBottom) : 0;
    state->totalHeight   = state->topAreaHeight + kInputBarHeight;

    state->hModernEdit = CreateWindowExW(
        0, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        -32000, -32000, 500, 28,
        hDlg, (HMENU)9901, GetModuleHandleW(nullptr), nullptr
    );

    if (state->hModernEdit) {
        SendMessageW(state->hModernEdit, WM_SETFONT, (WPARAM)state->hFontEditCtl, FALSE);

        if (g_settings.clearInitialInput) {
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
    RECT rcWork = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);

    int newX = rcWin.left;
    int newY = rcWin.top;
    if (newY + state->totalHeight > rcWork.bottom - 12) {
        newY = rcWork.bottom - state->totalHeight - 16;
    }
    if (newX + kDlgWidth > rcWork.right - 12) {
        newX = rcWork.right - kDlgWidth - 16;
    }
    if (newX < rcWork.left + 12) {
        newX = rcWork.left + 16;
    }

    state->normalX = newX;
    state->normalY = newY;

    SetWindowPos(hDlg, nullptr, newX, newY, kDlgWidth, state->totalHeight,
                 SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    g_hActiveRunDlg = hDlg;
    g_activeState = state;
    g_openTick = GetTickCount64();
    g_lastToggleTick = g_openTick;
    g_winRReleasedSinceOpen = ((GetAsyncKeyState('R') & 0x8000) == 0);
    if (!g_hKeyHook) {
        g_hKeyHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyProc, GetModuleHandleW(nullptr), 0);
    }

    if (state->hModernEdit) {
        SetFocus(state->hModernEdit);
    }
    InvalidateRect(hDlg, nullptr, TRUE);
}

thread_local DLGPROC g_tlsOrigDlgProc = nullptr;

INT_PTR CALLBACK WrapperDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    DLGPROC origProc = (DLGPROC)GetPropW(hDlg, L"WindhawkOrigDlgProc");
    if (!origProc && g_tlsOrigDlgProc) {
        origProc = g_tlsOrigDlgProc;
        g_tlsOrigDlgProc = nullptr;
        SetPropW(hDlg, L"WindhawkOrigDlgProc", (HANDLE)origProc);
    }

    INT_PTR ret = origProc ? origProc(hDlg, uMsg, wParam, lParam) : FALSE;

    if (uMsg == WM_INITDIALOG && IsRunDialog(hDlg)) {
        TransformRunDialog(hDlg);
        if (HWND hModernEdit = GetDlgItem(hDlg, 9901)) {
            SetFocus(hModernEdit);
            return FALSE;
        }
    } else if (uMsg == WM_NCDESTROY) {
        RemovePropW(hDlg, L"WindhawkOrigDlgProc");
    }
    return ret;
}

using DialogBoxParamW_t = INT_PTR(WINAPI*)(HINSTANCE, LPCWSTR, HWND, DLGPROC, LPARAM);
DialogBoxParamW_t DialogBoxParamW_Orig = nullptr;
INT_PTR WINAPI DialogBoxParamW_Hook(HINSTANCE hInstance, LPCWSTR lpTemplateName,
                                    HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) {
    if (lpDialogFunc && lpDialogFunc != WrapperDlgProc) {
        g_tlsOrigDlgProc = lpDialogFunc;
        lpDialogFunc = WrapperDlgProc;
    }
    return DialogBoxParamW_Orig(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

using DialogBoxIndirectParamW_t = INT_PTR(WINAPI*)(HINSTANCE, LPCDLGTEMPLATEW, HWND, DLGPROC, LPARAM);
DialogBoxIndirectParamW_t DialogBoxIndirectParamW_Orig = nullptr;
INT_PTR WINAPI DialogBoxIndirectParamW_Hook(HINSTANCE hInstance, LPCDLGTEMPLATEW hDialogTemplate,
                                            HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) {
    if (lpDialogFunc && lpDialogFunc != WrapperDlgProc) {
        g_tlsOrigDlgProc = lpDialogFunc;
        lpDialogFunc = WrapperDlgProc;
    }
    return DialogBoxIndirectParamW_Orig(hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam);
}

using CreateDialogParamW_t = HWND(WINAPI*)(HINSTANCE, LPCWSTR, HWND, DLGPROC, LPARAM);
CreateDialogParamW_t CreateDialogParamW_Orig = nullptr;
HWND WINAPI CreateDialogParamW_Hook(HINSTANCE hInstance, LPCWSTR lpTemplateName,
                                    HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) {
    if (lpDialogFunc && lpDialogFunc != WrapperDlgProc) {
        g_tlsOrigDlgProc = lpDialogFunc;
        lpDialogFunc = WrapperDlgProc;
    }
    return CreateDialogParamW_Orig(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
}

using CreateDialogIndirectParamW_t = HWND(WINAPI*)(HINSTANCE, LPCDLGTEMPLATEW, HWND, DLGPROC, LPARAM);
CreateDialogIndirectParamW_t CreateDialogIndirectParamW_Orig = nullptr;
HWND WINAPI CreateDialogIndirectParamW_Hook(HINSTANCE hInstance, LPCDLGTEMPLATEW lpTemplate,
                                            HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) {
    if (lpDialogFunc && lpDialogFunc != WrapperDlgProc) {
        g_tlsOrigDlgProc = lpDialogFunc;
        lpDialogFunc = WrapperDlgProc;
    }
    return CreateDialogIndirectParamW_Orig(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (!g_internalActivation && g_hActiveRunDlg && hWnd == g_hActiveRunDlg && g_activeState) {
        if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_FORCEMINIMIZE) {
            MinimizeRunDialog(g_activeState);
            return TRUE;
        }
        if ((nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL || nCmdShow == SW_SHOW) && IsIconic(hWnd)) {
            RestoreRunDialog(g_activeState);
            return TRUE;
        }
    }
    if (nCmdShow != SW_HIDE && IsRunDialog(hWnd)) {
        TransformRunDialog(hWnd);
    }
    return ShowWindow_Orig(hWnd, nCmdShow);
}

void WINAPI SwitchToThisWindow_Hook(HWND hWnd, BOOL fUnknown) {
    if (!g_internalActivation && g_hActiveRunDlg && hWnd == g_hActiveRunDlg && g_activeState && !g_activeState->isBrowsingFile) {
        if (GetTickCount64() - g_openTick > 350 && GetTickCount64() - g_lastToggleTick > 250) {
            if (IsIconic(hWnd)) {
                RestoreRunDialog(g_activeState);
                return;
            } else if (IsWindowVisible(hWnd)) {
                POINT pt = {};
                GetCursorPos(&pt);
                RECT rcDlg = {};
                GetWindowRect(hWnd, &rcDlg);
                if (!PtInRect(&rcDlg, pt)) {
                    MinimizeRunDialog(g_activeState);
                    return;
                }
            }
        }
    }
    if (SwitchToThisWindow_Orig) {
        SwitchToThisWindow_Orig(hWnd, fUnknown);
    }
}

BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    if (!g_internalActivation && g_hActiveRunDlg && hWnd == g_hActiveRunDlg && g_activeState && !g_activeState->isBrowsingFile) {
        if (GetTickCount64() - g_openTick > 350 && GetTickCount64() - g_lastToggleTick > 250) {
            if (IsIconic(hWnd)) {
                RestoreRunDialog(g_activeState);
                return TRUE;
            } else if (IsWindowVisible(hWnd)) {
                POINT pt = {};
                GetCursorPos(&pt);
                RECT rcDlg = {};
                GetWindowRect(hWnd, &rcDlg);
                if (!PtInRect(&rcDlg, pt)) {
                    MinimizeRunDialog(g_activeState);
                    return TRUE;
                }
            }
        }
    }
    return SetForegroundWindow_Orig(hWnd);
}

UINT g_msgApplyModernRun = 0;

LRESULT CALLBACK CallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && lParam) {
        auto* cwp = reinterpret_cast<CWPSTRUCT*>(lParam);
        if (cwp->message == g_msgApplyModernRun && IsRunDialog(cwp->hwnd)) {
            TransformRunDialog(cwp->hwnd);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL CALLBACK EnumExistingRunDlgProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && IsRunDialog(hwnd)) {
        HHOOK hHook = SetWindowsHookExW(WH_CALLWNDPROC, CallWndProcHook, nullptr, tid);
        if (hHook) {
            SendMessageW(hwnd, g_msgApplyModernRun, 0, 0);
            UnhookWindowsHookEx(hHook);
        }
    }
    return TRUE;
}

BOOL Wh_ModInit() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    LoadSettings();
    g_msgApplyModernRun = RegisterWindowMessageW(L"Windhawk_ApplyModernRun_v10");

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return FALSE;

    Wh_SetFunctionHook((void*)GetProcAddress(hUser32, "DialogBoxParamW"),
                       (void*)DialogBoxParamW_Hook, (void**)&DialogBoxParamW_Orig);
    Wh_SetFunctionHook((void*)GetProcAddress(hUser32, "DialogBoxIndirectParamW"),
                       (void*)DialogBoxIndirectParamW_Hook, (void**)&DialogBoxIndirectParamW_Orig);
    Wh_SetFunctionHook((void*)GetProcAddress(hUser32, "CreateDialogParamW"),
                       (void*)CreateDialogParamW_Hook, (void**)&CreateDialogParamW_Orig);
    Wh_SetFunctionHook((void*)GetProcAddress(hUser32, "CreateDialogIndirectParamW"),
                       (void*)CreateDialogIndirectParamW_Hook, (void**)&CreateDialogIndirectParamW_Orig);
    Wh_SetFunctionHook((void*)GetProcAddress(hUser32, "ShowWindow"),
                       (void*)ShowWindow_Hook, (void**)&ShowWindow_Orig);
    Wh_SetFunctionHook((void*)GetProcAddress(hUser32, "SetForegroundWindow"),
                       (void*)SetForegroundWindow_Hook, (void**)&SetForegroundWindow_Orig);
    Wh_SetFunctionHook((void*)GetProcAddress(hUser32, "SwitchToThisWindow"),
                       (void*)SwitchToThisWindow_Hook, (void**)&SwitchToThisWindow_Orig);

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumExistingRunDlgProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_hActiveRunDlg && IsWindow(g_hActiveRunDlg)) {
        ApplyWindowBlur(g_hActiveRunDlg);
        InvalidateRect(g_hActiveRunDlg, nullptr, TRUE);
    }
}

void Wh_ModUninit() {
    if (g_hKeyHook) {
        UnhookWindowsHookEx(g_hKeyHook);
        g_hKeyHook = nullptr;
    }
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}