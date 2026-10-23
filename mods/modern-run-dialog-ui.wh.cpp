// ==WindhawkMod==
// @id              modern-run-dialog-ui
// @name            Modern Run Dialog UI For Windows 10 & 11
// @name:vi-VN      Giao diện Hộp thoại Run Hiện đại cho Windows 10 & 11
// @description     Transforms the classic Run dialog on Windows 10 & 11 into a modern Fluent UI with Liquid Glass, DirectWrite typography, Windows Blur, and instant history execution.
// @description:vi-VN Biến đổi hộp thoại Run cổ điển trên Windows 10 & 11 thành giao diện Fluent UI hiện đại với hiệu ứng Liquid Glass, phông chữ DirectWrite sắc nét và chạy lệnh nhanh từ lịch sử.
// @version         3.0.4
// @author          caotu2k5
// @github          https://github.com/caotu2k5
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -luser32 -lshell32 -lshlwapi -lcomctl32 -lversion -limm32 -lcomdlg32 -ld2d1 -ldwrite -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Modern Run Dialog UI For Windows 10 & 11

![Modern Run Dialog UI Preview (Liquid Glass)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/liquidglass.png)

Modern Run Dialog UI Preview (Liquid Glass)

![Modern Run Dialog UI Preview (Windows Blur)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/windowsblur.png)

Modern Run Dialog UI Preview (Windows Blur)

---

## 🌐 English

**Modern Run Dialog UI For Windows 10 & 11** replaces the classic Win32 **Run (`Win + R`)** dialog with a borderless **Liquid Glass / Fluent Design** command palette powered by **Direct2D & DirectWrite**, fully compatible with both **Windows 10** and **Windows 11**.

### ✨ Key Features

- **Full Windows 10 & Windows 11 Compatibility:**
  * Works seamlessly on both **Windows 10** and **Windows 11** (`x86-64`), automatically anchoring right above the Taskbar and supporting Per-Monitor DPI scaling across both operating systems.

- **Liquid Glass, Windows Blur & Opaque Themes:**
  * **Liquid Glass:** Translucent low-tint glass backdrop paired with dual refractive rim strokes, pointer-following specular highlights, and glass-styled pill buttons.
  * **16×16 Luminance Contrast Adaptation:** Automatically switches glass text contrast between light and dark based on a temporary 16×16 luminance sample of the backdrop.
  * **Windows Blur & Opaque Fallbacks:** Respects High Contrast mode and automatically falls back to Windows Blur on older unsupported CPUs.

- **Native DirectWrite Typography:**
  * Rendered with **DirectWrite (`IDWriteTextLayout`)** and **Direct2D** for crisp Semibold command titles, Regular metadata, Per-Monitor DPI scaling, and accurate multilingual diacritics.

- **One-Click Smart History (Per-User Storage):**
  * Displays up to **10 recently executed commands** with native icons, friendly app names, and execution timestamps.
  * **Click any history item to execute it immediately** without cluttering the input box.
  * Maintains a per-user command and timestamp history in Windhawk mod storage (`HistoryTimestamps_<SID>`), allowing the mod's history list to work even when Windows' own `RunMRU` tracking is disabled.
  * **Clearing History (Non-Destructive):** Enable **History · Clear Stored History** in the mod settings and click Save (or press `Delete` on a hovered/selected history row) to clear the mod's displayed history for the current user without deleting Windows' native `RunMRU` registry entries, or set **History · Max Items** to `0` to disable history completely.

- **Quality-of-Life Enhancements:**
  * **Taskbar Icon & `Win + R` Toggle:** Click the Run icon on the Taskbar to minimize/restore, or press `Win + R` while open to close immediately.
  * **Seamless IME & Multilingual Support:** Works smoothly with Windows IME (Vietnamese Telex/VNI, CJK), Unikey, and EVKey, and supports 11 display languages (including Italian).

---

## 🇻🇳 Tiếng Việt

**Modern Run Dialog UI For Windows 10 & 11** thay thế hộp thoại **Run (`Win + R`)** cổ điển bằng giao diện **Liquid Glass / Fluent Design** không viền hiện đại, được kết xuất trực tiếp bằng **Direct2D & DirectWrite** và tương thích hoàn hảo với cả **Windows 10** lẫn **Windows 11**.

### ✨ Tính năng nổi bật

- **Tương thích tốt với cả Windows 10 & Windows 11:**
  * Hoạt động mượt mà, ổn định trên cả **Windows 10** và **Windows 11** (`x86-64`), tự động căn chỉnh vị trí sát thanh Taskbar và co giãn chuẩn theo DPI từng màn hình.

- **3 Chế độ bề mặt (Liquid Glass, Làm mờ Windows & Nền đặc):**
  * **Liquid Glass:** Lớp nền kính trong suốt kết hợp viền khúc xạ ánh sáng 2 lớp, hiệu ứng phản sáng chạy theo trỏ chuột và các nút bấm dạng viên thuốc bằng kính.
  * **Tự động thích ứng độ tương phản (Mẫu 16×16):** Tự động chuyển đổi màu chữ và sắc độ kính giữa Sáng và Tối dựa trên mẫu đo độ sáng 16×16 tạm thời của vùng nền phía sau.
  * **Làm mờ Windows & Nền đặc tương thích:** Tôn trọng chế độ Tương phản cao (High Contrast) của hệ thống và tự động chuyển về Làm mờ Windows trên các dòng CPU đời cũ.

- **Phông chữ DirectWrite sắc nét:**
  * Kết xuất bằng **DirectWrite (`IDWriteTextLayout`)** và **Direct2D** mang lại chất lượng chữ sắc nét: tên lệnh Semibold nổi bật, tên ứng dụng Regular cân đối, hiển thị tiếng Việt có dấu đều đặn và tự co giãn chuẩn theo DPI màn hình.

- **Lịch sử thông minh — Bấm là chạy ngay (Lưu trữ riêng theo từng User):**
  * Hiển thị tối đa **10 lệnh chạy gần nhất** kèm biểu tượng gốc, tên ứng dụng thân thiện (`lệnh · Tên ứng dụng`) và thời gian thực thi.
  * **Bấm trực tiếp vào bất kỳ mục lịch sử nào để chạy lệnh ngay lập tức** mà không cần đẩy xuống ô nhập liệu.
  * Lưu lịch sử và thời gian chạy riêng biệt cho từng tài khoản người dùng Windows (`HistoryTimestamps_<SID>`), giúp lịch sử hoạt động ổn định kể cả khi bạn tắt tính năng lưu lịch sử `RunMRU` của Windows.
  * **Xóa lịch sử an toàn:** Bật tùy chọn **Xóa lịch sử đã lưu** trong phần Cài đặt mod rồi bấm Lưu (hoặc nhấn phím `Delete` khi đang trỏ chuột/chọn một dòng lịch sử) để làm sạch danh sách lịch sử hiển thị trong mod mà không xóa dữ liệu `RunMRU` gốc của Windows, hoặc đặt **Số mục lịch sử tối đa** về `0` để tắt hẳn.

- **Tiện ích nâng cao trải nghiệm (Quality-of-Life):**
  * **Đóng/mở nhanh từ Taskbar & `Win + R`:** Bấm vào biểu tượng Run dưới Taskbar để thu nhỏ/khôi phục cửa sổ, hoặc nhấn lại `Win + R` khi hộp thoại đang mở để đóng nhanh.
  * **Tương thích bộ gõ Tiếng Việt & Đa ngôn ngữ:** Hoạt động mượt mà với Unikey, EVKey, bộ gõ Windows IME và hỗ trợ 11 ngôn ngữ hiển thị.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Display · Language
  $name:vi-VN: Ngôn ngữ hiển thị
  $description: Auto-detect the system’s UI language or choose a fixed language.
  $description:vi-VN: Tự động theo hệ điều hành hoặc chọn ngôn ngữ cố định.
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
  - it: Italiano (Tiếng Ý)
  - ru: Русский (Tiếng Nga)
  $options:
  - auto: Auto – System Default
  - en: English
  - vi: Tiếng Việt (Vietnamese)
  - ja: 日本語 (Japanese)
  - zh_cn: 简体中文 (Simplified Chinese)
  - zh_tw: 繁體中文 (Traditional Chinese)
  - ko: 한국어 (Korean)
  - fr: Français (French)
  - de: Deutsch (German)
  - es: Español (Spanish)
  - it: Italiano (Italian)
  - ru: Русский (Russian)
- surfaceTheme: glass
  $name: Appearance · Surface Theme
  $name:vi-VN: Giao diện bề mặt
  $description: Choose between Liquid Glass, Windows Blur, or Off – Opaque surface.
  $description:vi-VN: Chọn giữa hiệu ứng Liquid Glass, Làm mờ Windows hoặc Tắt làm mờ (nền đặc).
  $options:vi-VN:
  - glass: Liquid Glass
  - blur: Làm mờ Windows
  - solid: Tắt làm mờ (Nền đặc)
  $options:
  - glass: Liquid Glass
  - blur: Windows Blur
  - solid: Off – Opaque
- colorMode: auto
  $name: Appearance · Color Mode
  $name:vi-VN: Chế độ Sáng / Tối
  $description: Follow Windows’ Light/Dark mode and backdrop contrast automatically, or choose a fixed mode.
  $description:vi-VN: Tự động theo giao diện Sáng/Tối của Windows và độ sáng nền, hoặc chọn cố định.
  $options:vi-VN:
  - auto: Tự động theo Windows
  - dark: Giao diện Tối (Dark)
  - light: Giao diện Sáng (Light)
  $options:
  - auto: Auto – Follow Windows
  - dark: Dark Mode
  - light: Light Mode
- blurOpacity: 170
  $name: Appearance · Blur Opacity (0–255)
  $name:vi-VN: Độ đậm nền Blur (0–255)
  $description: Background tint opacity when using Windows Blur (0–255, default is 170).
  $description:vi-VN: Độ đậm lớp phủ nền khi dùng Làm mờ Windows (0–255, mặc định là 170).
- maxHistoryItems: 5
  $name: History · Max Items (0–10)
  $name:vi-VN: Số mục lịch sử tối đa (0–10)
  $description: Maximum number of recent commands shown in the top list (0–10, set 0 to disable).
  $description:vi-VN: Số lượng lệnh gần đây hiển thị ở phần phía trên (0–10, nhập 0 để tắt).
- showTimestamp: true
  $name: History · Show Last Executed Time
  $name:vi-VN: Hiển thị thời gian chạy lệnh gần nhất
  $description: Display the execution timestamp and clock icon on each history row — or hide both when disabled.
  $description:vi-VN: Hiển thị thời gian thực thi và biểu tượng đồng hồ ở mỗi dòng lịch sử.
- clearStoredHistory: false
  $name: History · Clear Stored History
  $name:vi-VN: Xóa lịch sử đã lưu trong Mod
  $description: Turn on and save to clear the mod’s displayed history for the current user (without deleting Windows’ native RunMRU) — or press Delete on any row in the dialog.
  $description:vi-VN: Bật và lưu để xóa lịch sử hiển thị trong mod của người dùng hiện tại (không xóa RunMRU gốc của Windows) — hoặc nhấn phím Delete trên dòng bất kỳ.
- clearInitialInput: true
  $name: Input · Clear Input on Open
  $name:vi-VN: Để trống ô nhập khi mở
  $description: Show placeholder text on open — instead of pre-filling the last command.
  $description:vi-VN: Hiển thị dòng gợi ý khi mới mở thay vì điền sẵn lệnh cũ.
- taskbarClickAction: minimize
  $name: Behavior · Taskbar Icon Click Action
  $name:vi-VN: Hành vi khi bấm icon Run trên Taskbar
  $description: Action when clicking the Run icon on the Taskbar while open — minimize/restore or close.
  $description:vi-VN: Hành động khi bấm vào biểu tượng Run dưới Taskbar lúc cửa sổ đang mở.
  $options:vi-VN:
  - minimize: Ẩn & Mở lại cửa sổ Run
  - close: Đóng hẳn cửa sổ Run
  $options:
  - minimize: Minimize – Restore
  - close: Close Window
- closeOnClickOutside: false
  $name: Behavior · Close on Click Outside
  $name:vi-VN: Tự động đóng khi bấm ra ngoài
  $description: Automatically close the Run dialog when clicking outside the window — similar to Command Palette.
  $description:vi-VN: Tự động đóng hộp thoại Run khi click chuột ra ngoài cửa sổ.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <sddl.h>
#include <commdlg.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <imm.h>
#include <intrin.h>
#include <cstring>
#include <cwchar>
#include <cmath>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

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
using ShowWindow_t = decltype(&ShowWindow);
using SetForegroundWindow_t = decltype(&SetForegroundWindow);
using SwitchToThisWindow_t = decltype(&SwitchToThisWindow);
using RunDlgProc_t = INT_PTR(CALLBACK*)(HWND, UINT, WPARAM, LPARAM);

ShowWindow_t ShowWindow_Orig = nullptr;
SetForegroundWindow_t SetForegroundWindow_Orig = nullptr;
SwitchToThisWindow_t SwitchToThisWindow_Orig = nullptr;
RunDlgProc_t RunDlgProc_Orig = nullptr;

ID2D1Factory* g_pD2DFactory = nullptr;
IDWriteFactory* g_pDWriteFactory = nullptr;

// --- CPU Policy for Liquid Glass Surface ---

bool GlassCpuAllowed(std::string_view vendor, unsigned family, unsigned model, std::string brand) {
    for (auto& ch : brand) {
        if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
    }
    if (vendor == "AuthenticAMD") {
        return family == 0x17 || family == 0x19 || family == 0x1A;
    }
    if (vendor != "GenuineIntel") return false;
    if (brand.find("core") != std::string::npos) {
        if (brand.find("ultra") != std::string::npos) return true;
        for (auto marker : { "i3-", "i5-", "i7-", "i9-" }) {
            auto at = brand.find(marker);
            if (at == std::string::npos) continue;
            at += 3;
            size_t end = at;
            while (end < brand.size() && brand[end] >= '0' && brand[end] <= '9') ++end;
            auto digits = brand.substr(at, end - at);
            if (digits.empty()) return false;
            if (digits.size() == 3) return false;
            if (digits.size() == 1) return digits[0] >= '7';
            unsigned generation = digits[0] - '0';
            if (digits.size() >= 5 || (digits.size() == 4 && end < brand.size() && brand[end] == 'g')) {
                generation = generation * 10 + digits[1] - '0';
            }
            return generation >= 7;
        }
    }
    if (family != 6) return false;
    switch (model) {
        case 0x8E: case 0x9E: case 0x66: case 0x7D: case 0x7E:
        case 0xA5: case 0xA6: case 0x8C: case 0x8D: case 0xA7:
        case 0x97: case 0x9A: case 0xB7: case 0xBA: case 0xBF:
        case 0xAA: case 0xAC: case 0xC5: case 0xC6: case 0xBD:
            return true;
        default:
            return false;
    }
}

bool DetectGlassCpuSupport() {
#if defined(_M_X64) || defined(__x86_64__)
    int info[4]{};
    __cpuid(info, 0);
    if (info[0] < 1) return false;
    char vendor[13]{};
    std::memcpy(vendor, &info[1], 4);
    std::memcpy(vendor + 4, &info[3], 4);
    std::memcpy(vendor + 8, &info[2], 4);
    __cpuid(info, 1);
    unsigned signature = static_cast<unsigned>(info[0]);
    unsigned baseFamily = (signature >> 8) & 15, family = baseFamily, model = (signature >> 4) & 15;
    if (baseFamily == 15) family += (signature >> 20) & 255;
    if (baseFamily == 6 || baseFamily == 15) model |= ((signature >> 16) & 15) << 4;
    char brand[49]{};
    __cpuid(info, static_cast<int>(0x80000000u));
    if (static_cast<unsigned>(info[0]) >= 0x80000004u) {
        for (unsigned i = 0; i < 3; ++i) {
            __cpuid(info, static_cast<int>(0x80000002u + i));
            std::memcpy(brand + i * 16, info, 16);
        }
    }
    return GlassCpuAllowed(vendor, family, model, brand);
#else
    return false;
#endif
}

bool g_glassCpuSupported = false;

enum class SurfaceTheme { Glass, Blur, Solid };
enum class ColorMode { Auto, Dark, Light };

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
    SurfaceTheme surfaceTheme = SurfaceTheme::Glass;
    ColorMode colorMode = ColorMode::Auto;
    bool taskbarCloseInsteadOfMinimize = false;
    int blurOpacity = 170;
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
    L"Today",
    L"Select an executable program",
    L"Programs (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0All Files (*.*)\0*.*\0\0"
};

UINT g_msgUninit = 0;
UINT g_msgToggle = 0;

enum ToggleAction : WPARAM {
    kActionMinimize = 1,
    kActionRestore = 2,
    kActionToggleWinR = 3,
    kActionReloadSettings = 4
};

// Base dimensions at 96 DPI
constexpr int kBaseDlgWidth         = 600;
constexpr int kBaseRowHeight        = 38;
constexpr int kBaseListPadTop       = 8;
constexpr int kBaseListPadBottom    = 8;
constexpr int kBaseInputBarHeight   = 52;
constexpr UINT_PTR kTimerCaret      = 9010;
constexpr UINT_PTR kTimerLuminance  = 9011;
constexpr int kModernEditId         = 9901;
constexpr size_t kMaxMruEntries     = 26;

int DpiScale(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi), 96);
}

float DpiScaleF(float value, UINT dpi) {
    return value * static_cast<float>(dpi) / 96.0f;
}

bool IsHighContrastActive() {
    HIGHCONTRASTW hc = { sizeof(hc) };
    return SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) &&
           (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

bool IsSystemAppLightTheme() {
    DWORD val = 0;
    DWORD cb = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme",
                     RRF_RT_REG_DWORD, nullptr, &val, &cb) == ERROR_SUCCESS) {
        return val != 0;
    }
    return false;
}

D2D1_COLOR_F GetSystemAccentColorF() {
    DWORD colorization = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) {
        BYTE r = (colorization >> 16) & 0xFF;
        BYTE g = (colorization >> 8) & 0xFF;
        BYTE b = colorization & 0xFF;
        if (r != 0 || g != 0 || b != 0) {
            return D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
        }
    }
    return D2D1::ColorF(0.44f, 0.38f, 0.88f, 1.0f);
}

// Temporary 16x16 background luminance sampler to adapt Liquid Glass text contrast
bool SampleBackdropLuminanceIsLight(int x, int y, int w, int h, bool dialogAlreadyVisible, bool prevLight) {
    if (w <= 0 || h <= 0) return prevLight;

    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) return prevLight;

    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = 16;
    bmi.bmiHeader.biHeight = -16;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* pixels = nullptr;
    HBITMAP hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, (void**)&pixels, nullptr, 0);
    bool valid = false;
    double lum = 0.0;

    if (hBmp && pixels) {
        HGDIOBJ oldBmp = SelectObject(hMemDC, hBmp);
        SetStretchBltMode(hMemDC, HALFTONE);

        BOOL ok = FALSE;
        if (!dialogAlreadyVisible) {
            int sx = x + w / 6;
            int sy = y + h / 6;
            int sw = (std::max)(16, (w * 2) / 3);
            int sh = (std::max)(16, (h * 2) / 3);
            ok = StretchBlt(hMemDC, 0, 0, 16, 16, hScreenDC, sx, sy, sw, sh, SRCCOPY);
        } else {
            int sampleY = (y >= 28) ? (y - 24) : (y + h + 4);
            int sx = x + w / 6;
            int sw = (std::max)(16, (w * 2) / 3);
            ok = StretchBlt(hMemDC, 0, 0, 16, 16, hScreenDC, sx, sampleY, sw, 16, SRCCOPY);
        }

        if (ok) {
            auto srgbToLinear = [](BYTE v) -> double {
                double c = v / 255.0;
                return (c <= 0.04045) ? (c / 12.92) : std::pow((c + 0.055) / 1.055, 2.4);
            };
            double total = 0.0;
            for (int i = 0; i < 256; ++i) {
                DWORD px = pixels[i];
                double r = srgbToLinear((px >> 16) & 0xFF);
                double g = srgbToLinear((px >> 8) & 0xFF);
                double b = srgbToLinear(px & 0xFF);
                total += 0.2126 * r + 0.7152 * g + 0.0722 * b;
            }
            lum = total / 256.0;
            valid = true;
        }

        SelectObject(hMemDC, oldBmp);
        DeleteObject(hBmp);
    }

    DeleteDC(hMemDC);
    ReleaseDC(nullptr, hScreenDC);

    if (!valid) return prevLight;
    return prevLight ? (lum > 0.52) : (lum > 0.62);
}

SurfaceTheme ResolveEffectiveSurface(const ModSettings& settings) {
    if (IsHighContrastActive()) {
        return SurfaceTheme::Solid;
    }
    if (settings.surfaceTheme == SurfaceTheme::Glass && !g_glassCpuSupported) {
        return SurfaceTheme::Blur;
    }
    return settings.surfaceTheme;
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
            case LANG_ITALIAN:    lang = L"it"; break;
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
            L"Hôm nay",
            L"Chọn chương trình (.exe) để đưa vào ô nhập",
            L"Chương trình (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tất cả tệp (*.*)\0*.*\0\0"
        };
    }
    if (lang == L"ja") {
        return {
            L"実行するコマンドの名前を入力してください",
            L"参照",
            L"実行",
            L"閉じる",
            L"今日",
            L"実行するプログラムを選択",
            L"プログラム (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0すべてのファイル (*.*)\0*.*\0\0"
        };
    }
    if (lang == L"zh_cn") {
        return {
            L"输入要运行的命令名称",
            L"浏览",
            L"运行",
            L"退出",
            L"今天",
            L"选择要运行的程序",
            L"程序 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0所有文件 (*.*)\0*.*\0\0"
        };
    }
    if (lang == L"zh_tw") {
        return {
            L"輸入要執行的命令名稱",
            L"瀏覽",
            L"執行",
            L"退出",
            L"今天",
            L"選擇要執行的程式",
            L"程式 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0所有檔案 (*.*)\0*.*\0\0"
        };
    }
    if (lang == L"ko") {
        return {
            L"실행할 명령의 이름을 입력하세요",
            L"찾아보기",
            L"실행",
            L"닫기",
            L"오늘",
            L"실행할 프로그램 선택",
            L"프로그램 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0모든 파일 (*.*)\0*.*\0\0"
        };
    }
    if (lang == L"fr") {
        return {
            L"Entrez le nom d'une commande à exécuter",
            L"Parcourir",
            L"Exécuter",
            L"Quitter",
            L"Aujourd'hui",
            L"Sélectionner un programme",
            L"Programmes (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tous les fichiers (*.*)\0*.*\0\0"
        };
    }
    if (lang == L"de") {
        return {
            L"Geben Sie den Namen eines Befehls ein",
            L"Durchsuchen",
            L"Ausführen",
            L"Schließen",
            L"Heute",
            L"Programm auswählen",
            L"Programme (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Alle Dateien (*.*)\0*.*\0\0"
        };
    }
    if (lang == L"es") {
        return {
            L"Escriba el nombre de un comando para ejecutar",
            L"Examinar",
            L"Ejecutar",
            L"Salir",
            L"Hoy",
            L"Seleccionar un programa",
            L"Programas (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Todos los archivos (*.*)\0*.*\0\0"
        };
    }
    if (lang == L"it") {
        return {
            L"Digitare il nome del programma da eseguire",
            L"Sfoglia...",
            L"OK",
            L"Annulla",
            L"Oggi",
            L"Seleziona un programma da eseguire",
            L"Programmi (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tutti i file (*.*)\0*.*\0\0"
        };
    }
    if (lang == L"ru") {
        return {
            L"Введите имя команды для выполнения",
            L"Обзор",
            L"Выполнить",
            L"Выход",
            L"Сегодня",
            L"Выберите программу",
            L"Программы (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Все файлы (*.*)\0*.*\0\0"
        };
    }

    return {
        L"Type the name of a command to run",
        L"Browse",
        L"Run",
        L"Exit",
        L"Today",
        L"Select an executable program",
        L"Programs (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0All Files (*.*)\0*.*\0\0"
    };
}

// --- Per-User Command, Timestamp & Hidden-MRU Storage in Windhawk Mod Storage ---

std::wstring TrimCommandString(const std::wstring& cmd) {
    wchar_t buf[MAX_PATH] = {};
    wcsncpy_s(buf, cmd.c_str(), _TRUNCATE);
    PathRemoveBlanksW(buf);
    std::wstring res = buf;
    for (wchar_t& ch : res) {
        if (ch == L'\t' || ch == L'\r' || ch == L'\n') {
            ch = L' ';
        }
    }
    return res;
}

std::wstring NormalizeCmdKey(const std::wstring& cmd) {
    std::wstring res = TrimCommandString(cmd);
    for (wchar_t& ch : res) {
        ch = towlower(ch);
    }
    return res;
}

std::wstring GetCurrentUserSidString() {
    HANDLE hToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        return L"Default";
    }
    DWORD dwSize = 0;
    GetTokenInformation(hToken, TokenUser, nullptr, 0, &dwSize);
    if (dwSize == 0) {
        CloseHandle(hToken);
        return L"Default";
    }
    std::vector<BYTE> buffer(dwSize);
    std::wstring sidResult = L"Default";
    if (GetTokenInformation(hToken, TokenUser, buffer.data(), dwSize, &dwSize)) {
        auto* pTokenUser = reinterpret_cast<TOKEN_USER*>(buffer.data());
        LPWSTR strSid = nullptr;
        if (ConvertSidToStringSidW(pTokenUser->User.Sid, &strSid) && strSid) {
            sidResult = strSid;
            LocalFree(strSid);
        }
    }
    CloseHandle(hToken);
    return sidResult;
}

const std::wstring& GetHistoryStorageKey() {
    static const std::wstring kKey = L"HistoryTimestamps_" + GetCurrentUserSidString();
    return kKey;
}

const std::wstring& GetHiddenMruStorageKey() {
    static const std::wstring kHiddenKey = L"HiddenRunMru_" + GetCurrentUserSidString();
    return kHiddenKey;
}

const std::wstring& GetClearAckStorageKey() {
    static const std::wstring kAckKey = L"ClearHistoryAck_" + GetCurrentUserSidString();
    return kAckKey;
}

std::vector<std::wstring> LoadHiddenMruCommands() {
    std::vector<std::wstring> hiddenKeys;
    std::vector<wchar_t> buffer(8192, L'\0');
    size_t charsRead = Wh_GetStringValue(GetHiddenMruStorageKey().c_str(), buffer.data(), buffer.size());
    if (charsRead == 0 || buffer[0] == L'\0') {
        return hiddenKeys;
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
        std::wstring norm = NormalizeCmdKey(line);
        if (!norm.empty() && std::find(hiddenKeys.begin(), hiddenKeys.end(), norm) == hiddenKeys.end()) {
            hiddenKeys.push_back(std::move(norm));
        }
        pos = endLine + 1;
    }
    return hiddenKeys;
}

void SaveHiddenMruCommands(const std::vector<std::wstring>& hiddenKeys) {
    if (hiddenKeys.empty()) {
        Wh_DeleteValue(GetHiddenMruStorageKey().c_str());
        return;
    }
    std::wstring serialized;
    size_t count = (std::min)(hiddenKeys.size(), (size_t)64);
    for (size_t i = 0; i < count; ++i) {
        serialized += hiddenKeys[i];
        serialized += L'\n';
    }
    Wh_SetStringValue(GetHiddenMruStorageKey().c_str(), serialized.c_str());
}

void AddCommandToHiddenMru(const std::wstring& cmd) {
    std::wstring norm = NormalizeCmdKey(cmd);
    if (norm.empty()) return;
    auto hiddenKeys = LoadHiddenMruCommands();
    if (std::find(hiddenKeys.begin(), hiddenKeys.end(), norm) == hiddenKeys.end()) {
        hiddenKeys.insert(hiddenKeys.begin(), norm);
        SaveHiddenMruCommands(hiddenKeys);
    }
}

void RemoveCommandFromHiddenMru(const std::wstring& cmd) {
    std::wstring norm = NormalizeCmdKey(cmd);
    if (norm.empty()) return;
    auto hiddenKeys = LoadHiddenMruCommands();
    auto newEnd = std::remove(hiddenKeys.begin(), hiddenKeys.end(), norm);
    if (newEnd != hiddenKeys.end()) {
        hiddenKeys.erase(newEnd, hiddenKeys.end());
        SaveHiddenMruCommands(hiddenKeys);
    }
}

// Clears the mod's per-user history and hides any commands currently in Windows' RunMRU
// without modifying or deleting Windows' native RunMRU registry values.
void ClearCurrentUserStoredHistory() {
    Wh_DeleteValue(GetHistoryStorageKey().c_str());

    auto hiddenKeys = LoadHiddenMruCommands();
    auto addHidden = [&](const std::wstring& rawCmd) {
        std::wstring norm = NormalizeCmdKey(rawCmd);
        if (!norm.empty() && std::find(hiddenKeys.begin(), hiddenKeys.end(), norm) == hiddenKeys.end()) {
            hiddenKeys.insert(hiddenKeys.begin(), norm);
        }
    };

    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t mruList[64] = {};
        DWORD cbList = sizeof(mruList);
        if (RegGetValueW(hKey, nullptr, L"MRUList", RRF_RT_REG_SZ, nullptr, mruList, &cbList) == ERROR_SUCCESS) {
            for (int i = 0; mruList[i] != L'\0'; ++i) {
                wchar_t valName[2] = { mruList[i], L'\0' };
                wchar_t cmdBuf[MAX_PATH] = {};
                DWORD cbCmd = sizeof(cmdBuf);
                if (RegGetValueW(hKey, nullptr, valName, RRF_RT_REG_SZ, nullptr, cmdBuf, &cbCmd) == ERROR_SUCCESS) {
                    size_t len = wcslen(cmdBuf);
                    if (len >= 2 && cmdBuf[len - 2] == L'\\' && cmdBuf[len - 1] == L'1') {
                        cmdBuf[len - 2] = L'\0';
                    }
                    addHidden(cmdBuf);
                }
            }
        }
        RegCloseKey(hKey);
    }

    SaveHiddenMruCommands(hiddenKeys);
}

void LoadSettings(bool fromSettingsChanged = false) {
    auto langSetting = WindhawkUtils::StringSetting::make(L"language");
    auto surfaceSetting = WindhawkUtils::StringSetting::make(L"surfaceTheme");
    auto colorSetting = WindhawkUtils::StringSetting::make(L"colorMode");
    auto tbAction = WindhawkUtils::StringSetting::make(L"taskbarClickAction");

    ModSettings newSettings;
    newSettings.language = langSetting.get()[0] ? langSetting.get() : L"auto";

    if (wcscmp(surfaceSetting.get(), L"blur") == 0) {
        newSettings.surfaceTheme = SurfaceTheme::Blur;
    } else if (wcscmp(surfaceSetting.get(), L"solid") == 0) {
        newSettings.surfaceTheme = SurfaceTheme::Solid;
    } else {
        newSettings.surfaceTheme = SurfaceTheme::Glass;
    }

    if (wcscmp(colorSetting.get(), L"light") == 0) {
        newSettings.colorMode = ColorMode::Light;
    } else if (wcscmp(colorSetting.get(), L"dark") == 0) {
        newSettings.colorMode = ColorMode::Dark;
    } else {
        newSettings.colorMode = ColorMode::Auto;
    }

    newSettings.taskbarCloseInsteadOfMinimize = (wcscmp(tbAction.get(), L"close") == 0);
    newSettings.blurOpacity = std::clamp(Wh_GetIntSetting(L"blurOpacity"), 0, 255);
    newSettings.maxHistoryItems = std::clamp(Wh_GetIntSetting(L"maxHistoryItems"), 0, 10);
    newSettings.showTimestamp = Wh_GetIntSetting(L"showTimestamp") != 0;
    newSettings.clearInitialInput = Wh_GetIntSetting(L"clearInitialInput") != 0;
    newSettings.closeOnClickOutside = Wh_GetIntSetting(L"closeOnClickOutside") != 0;

    bool clearRequested = Wh_GetIntSetting(L"clearStoredHistory") != 0;
    const std::wstring& ackKey = GetClearAckStorageKey();
    int ack = Wh_GetIntValue(ackKey.c_str(), 0);
    if (clearRequested) {
        if (ack == 0) {
            if (fromSettingsChanged) {
                ClearCurrentUserStoredHistory();
            }
            Wh_SetIntValue(ackKey.c_str(), 1);
        }
    } else {
        if (ack != 0) {
            Wh_SetIntValue(ackKey.c_str(), 0);
        }
    }

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

    HFONT hFontEditCtl = nullptr;
    HICON hRunIcon = nullptr;
    std::vector<HistoryItem> history;

    ModSettings settings;
    LocalizedStrings i18n = {};
    SurfaceTheme effectiveSurface = SurfaceTheme::Glass;
    bool isLightMode = false;
    bool backdropLightSample = false;

    UINT dpi = 96;
    int topAreaHeight = 0;
    int totalHeight = kBaseInputBarHeight;
    int normalX = 100;
    int normalY = 100;
    int hoveredRow = -1;
    int pressedRow = -1;
    int selectedRow = -1;
    POINT mousePt = { -1, -1 };
    bool hasMousePt = false;
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
std::atomic<bool> g_unloading{false};
thread_local bool g_internalActivation = false;

// Wrapped in std::optional with [[clang::no_destroy]] so the CRT does not invoke
// std::thread::~thread() (which calls std::terminate() if joinable) when Explorer
// exits while the Run dialog is still open and Wh_ModUninit is skipped.
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
    if (g_keyHookThread.has_value()) {
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
        if (!g_keyHookThread.has_value()) {
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

void ApplyWindowAppearance(HWND hDlg, RunDialogState* state);
void CloseRunDialog(HWND hDlg);
void SyncInputToNativeCombo(RunDialogState* state, const wchar_t* textOverride = nullptr);
void PerformDialogUninit(HWND hWnd, RunDialogState* state);

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

// Computes the initial dialog position anchored right next to the Taskbar
void GetDefaultTaskbarAnchoredPosition(HWND hDlg, int width, int height, UINT dpi, int* outX, int* outY) {
    HMONITOR hMon = MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    RECT rcWork = {};
    RECT rcMon = {};
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        rcWork = mi.rcWork;
        rcMon  = mi.rcMonitor;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
        rcMon = rcWork;
    }

    int margin = DpiScale(12, dpi);
    bool taskbarAtTop   = (rcWork.top > rcMon.top);
    bool taskbarAtRight = (rcWork.right < rcMon.right) &&
                          (rcWork.top == rcMon.top) &&
                          (rcWork.bottom == rcMon.bottom);

    if (taskbarAtRight) {
        *outX = rcWork.right - width - margin;
    } else {
        *outX = rcWork.left + margin;
    }

    if (taskbarAtTop) {
        *outY = rcWork.top + margin;
    } else {
        *outY = rcWork.bottom - height - margin;
    }
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

    int margin = DpiScale(12, dpi);

    if (*inOutY + height > rcWork.bottom - margin) {
        *inOutY = rcWork.bottom - height - margin;
    }
    if (*inOutY < rcWork.top + margin) {
        *inOutY = rcWork.top + margin;
    }
    if (*inOutX + width > rcWork.right - margin) {
        *inOutX = rcWork.right - width - margin;
    }
    if (*inOutX < rcWork.left + margin) {
        *inOutX = rcWork.left + margin;
    }
}

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

    g_internalActivation = true;
    if (ShowWindow_Orig) {
        ShowWindow_Orig(state->hDlg, SW_MINIMIZE);
    } else {
        ShowWindow(state->hDlg, SW_MINIMIZE);
    }
    g_internalActivation = false;
}

void RestoreRunDialog(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    ULONGLONG now = GetTickCount64();
    if (now - g_lastToggleTick.load() < 250) return;
    g_lastToggleTick.store(now);

    int dlgW = GetDlgWidth(state);
    state->backdropLightSample = SampleBackdropLuminanceIsLight(
        state->normalX, state->normalY, dlgW, state->totalHeight, false, state->backdropLightSample);

    g_internalActivation = true;
    if (ShowWindow_Orig) {
        ShowWindow_Orig(state->hDlg, SW_RESTORE);
    } else {
        ShowWindow(state->hDlg, SW_RESTORE);
    }

    LONG_PTR style = GetWindowLongPtrW(state->hDlg, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
    style |= (WS_SYSMENU | WS_MINIMIZEBOX);
    SetWindowLongPtrW(state->hDlg, GWL_STYLE, style);

    SetWindowPos(state->hDlg, HWND_TOP,
                 state->normalX, state->normalY,
                 dlgW, state->totalHeight,
                 SWP_SHOWWINDOW | SWP_FRAMECHANGED);

    ApplyWindowAppearance(state->hDlg, state);

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

void OpenBrowseExecutableDialog(RunDialogState* state) {
    if (!state || !state->hDlg || state->isBrowsingFile) return;
    HWND hDlg = state->hDlg;
    state->isBrowsingFile = true;
    g_isBrowsingFile.store(true);

    wchar_t filePath[MAX_PATH] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hDlg;
    ofn.lpstrFilter = state->i18n.browseFilter;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = state->i18n.browseTitle;
    ofn.lpstrDefExt = L"exe";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;

    BOOL picked = GetOpenFileNameW(&ofn);

    if (g_hActiveRunDlg.load() != hDlg || !IsWindow(hDlg)) {
        g_isBrowsingFile.store(false);
        return;
    }

    if (state->pendingUninit) {
        g_isBrowsingFile.store(false);
        state->isBrowsingFile = false;
        return;
    }

    g_isBrowsingFile.store(false);
    state->isBrowsingFile = false;

    if (picked && filePath[0] != L'\0') {
        std::wstring formattedPath = filePath;
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

    if (IsWindow(hDlg)) {
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            SetFocus(state->hModernEdit);
        }
        state->caretVisible = true;
        InvalidateRect(hDlg, nullptr, TRUE);
    }
}

struct StoredTimestampEntry {
    ULONGLONG fileTime = 0;
    std::wstring cmdKey;
    std::wstring displayCmd;
};

std::vector<StoredTimestampEntry> LoadStoredTimestamps() {
    std::vector<StoredTimestampEntry> entries;
    std::vector<wchar_t> buffer(16384, L'\0');
    size_t charsRead = Wh_GetStringValue(GetHistoryStorageKey().c_str(), buffer.data(), buffer.size());
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
        size_t tab1 = line.find(L'\t');
        if (tab1 != std::wstring::npos && tab1 > 0 && tab1 + 1 < line.size()) {
            wchar_t* endPtr = nullptr;
            ULONGLONG ftVal = wcstoull(line.substr(0, tab1).c_str(), &endPtr, 10);
            size_t tab2 = line.find(L'\t', tab1 + 1);
            std::wstring key;
            std::wstring origCmd;
            if (tab2 != std::wstring::npos) {
                key = line.substr(tab1 + 1, tab2 - (tab1 + 1));
                origCmd = line.substr(tab2 + 1);
            } else {
                key = line.substr(tab1 + 1);
                origCmd = key;
            }
            if (ftVal != 0 && !key.empty()) {
                if (origCmd.empty()) origCmd = key;
                entries.push_back({ ftVal, std::move(key), std::move(origCmd) });
            }
        }
        pos = endLine + 1;
    }
    return entries;
}

void SaveStoredTimestamps(const std::vector<StoredTimestampEntry>& entries) {
    if (entries.empty()) {
        Wh_DeleteValue(GetHistoryStorageKey().c_str());
        return;
    }
    std::wstring serialized;
    size_t count = (std::min)(entries.size(), kMaxMruEntries);
    for (size_t i = 0; i < count; ++i) {
        wchar_t numBuf[32] = {};
        swprintf_s(numBuf, L"%llu", entries[i].fileTime);
        serialized += numBuf;
        serialized += L'\t';
        serialized += entries[i].cmdKey;
        serialized += L'\t';
        serialized += entries[i].displayCmd.empty() ? entries[i].cmdKey : entries[i].displayCmd;
        serialized += L'\n';
    }
    Wh_SetStringValue(GetHistoryStorageKey().c_str(), serialized.c_str());
}

void SaveCommandTimestamp(const std::wstring& cmd) {
    std::wstring cleanCmd = TrimCommandString(cmd);
    std::wstring keyName = NormalizeCmdKey(cleanCmd);
    if (keyName.empty()) return;

    // Unhide the command if it was previously hidden
    RemoveCommandFromHiddenMru(cleanCmd);

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
    entries.insert(entries.begin(), { uli.QuadPart, keyName, cleanCmd });
    if (entries.size() > kMaxMruEntries) {
        entries.resize(kMaxMruEntries);
    }
    SaveStoredTimestamps(entries);
}

void RemoveCommandFromHistoryStorage(const std::wstring& cmd) {
    std::wstring keyName = NormalizeCmdKey(cmd);
    if (keyName.empty()) return;

    auto entries = LoadStoredTimestamps();
    auto newEnd = std::remove_if(entries.begin(), entries.end(),
                                 [&](const StoredTimestampEntry& e) { return e.cmdKey == keyName; });
    if (newEnd != entries.end()) {
        entries.erase(newEnd, entries.end());
        SaveStoredTimestamps(entries);
    }

    // Also mark as hidden in HiddenRunMru_<SID> so it isn't re-imported from Windows' RunMRU
    AddCommandToHiddenMru(cmd);
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

std::wstring LookupOrInitTimestamp(const std::wstring& cmd,
                                   const FILETIME& fallbackMruTime,
                                   const wchar_t* todayPrefix,
                                   std::vector<StoredTimestampEntry>& entries,
                                   bool* outModified) {
    std::wstring cleanCmd = TrimCommandString(cmd);
    std::wstring keyName = NormalizeCmdKey(cleanCmd);
    if (keyName.empty()) return L"";

    for (auto& entry : entries) {
        if (entry.cmdKey == keyName && entry.fileTime != 0) {
            if (entry.displayCmd == entry.cmdKey && cleanCmd != keyName) {
                entry.displayCmd = cleanCmd;
                if (outModified) *outModified = true;
            }
            ULARGE_INTEGER uli = {};
            uli.QuadPart = entry.fileTime;
            FILETIME ft = { uli.LowPart, uli.HighPart };
            return FormatHistoryTimestamp(ft, todayPrefix);
        }
    }

    ULARGE_INTEGER uli = {};
    uli.LowPart  = fallbackMruTime.dwLowDateTime;
    uli.HighPart = fallbackMruTime.dwHighDateTime;
    if (uli.QuadPart == 0) {
        FILETIME ftNow = {};
        GetSystemTimeAsFileTime(&ftNow);
        uli.LowPart  = ftNow.dwLowDateTime;
        uli.HighPart = ftNow.dwHighDateTime;
    }

    FILETIME ft = { uli.LowPart, uli.HighPart };
    if (entries.size() < kMaxMruEntries) {
        entries.push_back({ uli.QuadPart, keyName, cleanCmd });
        if (outModified) *outModified = true;
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

void ExecuteCommandImmediate(RunDialogState* state, const std::wstring& rawCommand) {
    if (!state || !state->hDlg || state->isTriggeringRun) return;
    state->isTriggeringRun = true;

    wchar_t rawBuf[MAX_PATH] = {};
    wcsncpy_s(rawBuf, rawCommand.c_str(), _TRUNCATE);
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

    SendMessageW(hDlg, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)hOk);

    if (g_hActiveRunDlg.load() == hDlg && IsWindow(hDlg)) {
        state->isTriggeringRun = false;
    }
}

void TriggerRunCommand(RunDialogState* state) {
    if (!state || !state->hDlg || state->isTriggeringRun) return;

    if (state->hModernEdit) {
        HIMC hImc = ImmGetContext(state->hModernEdit);
        if (hImc) {
            ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
            ImmReleaseContext(state->hModernEdit, hImc);
        }
    }

    std::wstring effectiveText = GetEffectiveInputText(state->hModernEdit);
    ExecuteCommandImmediate(state, effectiveText);
}

bool ResolveDialogLightContrast(const RunDialogState* state) {
    if (IsHighContrastActive()) {
        COLORREF winBg = GetSysColor(COLOR_WINDOW);
        double lum = (0.299 * GetRValue(winBg) + 0.587 * GetGValue(winBg) + 0.114 * GetBValue(winBg)) / 255.0;
        return lum > 0.5;
    }
    if (state->settings.colorMode == ColorMode::Light) return true;
    if (state->settings.colorMode == ColorMode::Dark) return false;
    if (state->effectiveSurface == SurfaceTheme::Glass) {
        return state->backdropLightSample;
    }
    return IsSystemAppLightTheme();
}

void ApplyWindowAppearance(HWND hDlg, RunDialogState* state) {
    if (!hDlg || !state) return;

    state->effectiveSurface = ResolveEffectiveSurface(state->settings);
    state->isLightMode = ResolveDialogLightContrast(state);

    BOOL dark = state->isLightMode ? FALSE : TRUE;
    DwmSetWindowAttribute(hDlg, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    DWORD corner = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(hDlg, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    COLORREF borderCol = (state->effectiveSurface == SurfaceTheme::Glass)
        ? 0xFFFFFFFE // DWMWA_COLOR_NONE (Liquid Glass draws its own dual refractive rim)
        : (state->isLightMode ? RGB(204, 208, 216) : RGB(64, 68, 76));
    DwmSetWindowAttribute(hDlg, DWMWA_BORDER_COLOR, &borderCol, sizeof(borderCol));

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return;

    auto pSetWCA = reinterpret_cast<SetWindowCompositionAttribute_t>(
        GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
    if (!pSetWCA) return;

    ACCENT_POLICY policy = {};
    if (state->effectiveSurface == SurfaceTheme::Glass) {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        DWORD alpha = state->isLightMode ? 32 : 24;
        DWORD bgr   = state->isLightMode ? 0x00F2F0F4 : 0x00181214;
        policy.GradientColor = (alpha << 24) | bgr;
    } else if (state->effectiveSurface == SurfaceTheme::Blur) {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        DWORD alpha = static_cast<DWORD>(state->settings.blurOpacity) & 0xFF;
        DWORD bgr   = state->isLightMode ? 0x00F6F3F2 : 0x00241E1C;
        policy.GradientColor = (alpha << 24) | bgr;
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
    auto hiddenMruKeys = LoadHiddenMruCommands();
    bool timestampsModified = false;

    auto isHiddenInMru = [&](const std::wstring& normKey) {
        return std::find(hiddenMruKeys.begin(), hiddenMruKeys.end(), normKey) != hiddenMruKeys.end();
    };

    auto addUniqueCommand = [&](const std::wstring& rawText, bool checkHiddenList) {
        if ((int)state->history.size() >= maxItems) return;
        std::wstring cleanText = TrimCommandString(rawText);
        std::wstring normKey = NormalizeCmdKey(cleanText);
        if (normKey.empty()) return;

        if (checkHiddenList && isHiddenInMru(normKey)) {
            return;
        }

        for (const auto& existing : state->history) {
            if (NormalizeCmdKey(existing.cmd) == normKey) {
                return;
            }
        }

        HistoryItem item;
        item.cmd = cleanText;
        ResolveCommandMetadata(cleanText, &item.hIcon, &item.appName);
        item.timeStr = LookupOrInitTimestamp(
            cleanText, ftRunMruWrite, state->i18n.todayPrefix, storedTimestamps, &timestampsModified);
        state->history.push_back(std::move(item));
    };

    for (const auto& entry : storedTimestamps) {
        if ((int)state->history.size() >= maxItems) break;
        addUniqueCommand(entry.displayCmd.empty() ? entry.cmdKey : entry.displayCmd, false);
    }

    if (state->hCombo) {
        int count = (int)SendMessageW(state->hCombo, CB_GETCOUNT, 0, 0);
        for (int i = 0; i < count && (int)state->history.size() < maxItems; ++i) {
            int len = (int)SendMessageW(state->hCombo, CB_GETLBTEXTLEN, i, 0);
            if (len > 0 && len < MAX_PATH) {
                std::wstring text(len + 1, L'\0');
                SendMessageW(state->hCombo, CB_GETLBTEXT, i, (LPARAM)text.data());
                text.resize(wcslen(text.c_str()));
                addUniqueCommand(text, true);
            }
        }
    }

    if ((int)state->history.size() < maxItems) {
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
                        addUniqueCommand(cmdBuf, true);
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

// Action Button & Input Bar Layout Rectangles
RECT GetBrowseButtonRect(const RunDialogState* state) {
    int dlgW = GetDlgWidth(state);
    int barH = GetInputBarHeight(state);
    int btnH = DpiScale(30, state->dpi);
    int top  = state->topAreaHeight + (barH - btnH) / 2;
    RECT rc  = { dlgW - DpiScale(266, state->dpi), top, dlgW - DpiScale(182, state->dpi), top + btnH };
    return rc;
}

RECT GetRunButtonRect(const RunDialogState* state) {
    int dlgW = GetDlgWidth(state);
    int barH = GetInputBarHeight(state);
    int btnH = DpiScale(30, state->dpi);
    int top  = state->topAreaHeight + (barH - btnH) / 2;
    RECT rc  = { dlgW - DpiScale(174, state->dpi), top, dlgW - DpiScale(94, state->dpi), top + btnH };
    return rc;
}

RECT GetCloseButtonRect(const RunDialogState* state) {
    int dlgW = GetDlgWidth(state);
    int barH = GetInputBarHeight(state);
    int btnH = DpiScale(30, state->dpi);
    int top  = state->topAreaHeight + (barH - btnH) / 2;
    RECT rc  = { dlgW - DpiScale(86, state->dpi), top, dlgW - DpiScale(12, state->dpi), top + btnH };
    return rc;
}

RECT GetModernEditRect(const RunDialogState* state) {
    int dlgW  = GetDlgWidth(state);
    int barH  = GetInputBarHeight(state);
    int editH = DpiScale(24, state->dpi);
    int top   = state->topAreaHeight + (barH - editH) / 2;
    RECT rc   = { DpiScale(46, state->dpi), top, dlgW - DpiScale(278, state->dpi), top + editH };
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

// --- Direct2D Liquid Glass & DirectWrite Rendering Helpers ---

void DrawD2DGlassRim(ID2D1RenderTarget* pRT, D2D1_RECT_F rc, float radius, UINT dpi, bool isLight, float outerThicknessDip = 1.2f) {
    D2D1_GRADIENT_STOP stops[5] = {
        { 0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.90f : 0.87f) },
        { 0.30f, D2D1::ColorF(0.75f, 0.87f, 1.00f, isLight ? 0.42f : 0.27f) },
        { 0.50f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.16f : 0.07f) },
        { 0.80f, D2D1::ColorF(1.00f, 0.85f, 0.93f, isLight ? 0.48f : 0.38f) },
        { 1.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.78f : 0.69f) }
    };

    ID2D1GradientStopCollection* pStops = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(stops, 5, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
        ID2D1LinearGradientBrush* pEdgeBrush = nullptr;
        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gradProps = D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(rc.left, rc.top), D2D1::Point2F(rc.right, rc.bottom));
        if (SUCCEEDED(pRT->CreateLinearGradientBrush(gradProps, pStops, &pEdgeBrush))) {
            D2D1_ROUNDED_RECT outerRRect = D2D1::RoundedRect(rc, radius, radius);
            pRT->DrawRoundedRectangle(outerRRect, pEdgeBrush, DpiScaleF(outerThicknessDip, dpi));
            pEdgeBrush->Release();
        }
        pStops->Release();
    }

    float inset = DpiScaleF(2.0f, dpi);
    if ((rc.right - rc.left) > inset * 2.0f && (rc.bottom - rc.top) > inset * 2.0f) {
        D2D1_RECT_F innerRc = D2D1::RectF(rc.left + inset, rc.top + inset, rc.right - inset, rc.bottom - inset);
        float innerRadius = (std::max)(1.0f, radius - inset);
        ID2D1SolidColorBrush* pInnerBr = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(
                D2D1::ColorF(1.0f, 1.0f, 1.0f, isLight ? 0.25f : 0.125f), &pInnerBr))) {
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(innerRc, innerRadius, innerRadius),
                                      pInnerBr, DpiScaleF(0.55f, dpi));
            pInnerBr->Release();
        }
    }
}

void DrawD2DGlassButton(ID2D1RenderTarget* pRT, D2D1_RECT_F rc, float radius, UINT dpi,
                        D2D1_COLOR_F bgTint, bool hovered, bool pressed, bool isLight) {
    D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(rc, radius, radius);

    ID2D1SolidColorBrush* pBaseBr = nullptr;
    if (SUCCEEDED(pRT->CreateSolidColorBrush(
            isLight ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.22f) : D2D1::ColorF(0.69f, 0.69f, 0.69f, 0.08f), &pBaseBr))) {
        pRT->FillRoundedRectangle(rrect, pBaseBr);
        pBaseBr->Release();
    }

    if (bgTint.a > 0.001f) {
        ID2D1SolidColorBrush* pTintBr = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(bgTint, &pTintBr))) {
            pRT->FillRoundedRectangle(rrect, pTintBr);
            pTintBr->Release();
        }
    }

    float hlOpacity = pressed ? 0.50f : (hovered ? 0.30f : 0.16f);
    D2D1_GRADIENT_STOP hlStops[3] = {
        { 0.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.00f * hlOpacity) },
        { 0.45f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.00f) },
        { 1.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.53f * hlOpacity) }
    };

    ID2D1GradientStopCollection* pHlStops = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(hlStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pHlStops))) {
        ID2D1LinearGradientBrush* pHlBrush = nullptr;
        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gradProps = D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(rc.left, rc.top), D2D1::Point2F(rc.right, rc.bottom));
        if (SUCCEEDED(pRT->CreateLinearGradientBrush(gradProps, pHlStops, &pHlBrush))) {
            pRT->FillRoundedRectangle(rrect, pHlBrush);
            pHlBrush->Release();
        }
        pHlStops->Release();
    }

    DrawD2DGlassRim(pRT, rc, radius, dpi, isLight, 1.2f);
}

void DrawD2DPointerSpecularGlow(ID2D1RenderTarget* pRT, POINT mousePt, int w, int h, UINT dpi, bool isLight) {
    float radius = DpiScaleF(190.0f, dpi);
    D2D1_GRADIENT_STOP stops[2] = {
        { 0.0f, isLight ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.18f) : D2D1::ColorF(0.88f, 0.92f, 1.0f, 0.10f) },
        { 1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f) }
    };

    ID2D1GradientStopCollection* pStops = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
        ID2D1RadialGradientBrush* pRadial = nullptr;
        D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radProps = D2D1::RadialGradientBrushProperties(
            D2D1::Point2F((float)mousePt.x, (float)mousePt.y),
            D2D1::Point2F(0.0f, 0.0f), radius, radius);
        if (SUCCEEDED(pRT->CreateRadialGradientBrush(radProps, pStops, &pRadial))) {
            pRT->FillRectangle(D2D1::RectF(0.0f, 0.0f, (float)w, (float)h), pRadial);
            pRadial->Release();
        }
        pStops->Release();
    }
}

void DrawD2DClockVector(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                        float cx, float cy, UINT dpi) {
    float r = DpiScaleF(5.8f, dpi);
    float sw = DpiScaleF(1.25f, dpi);
    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(cx, cy - DpiScaleF(3.2f, dpi)),
                  D2D1::Point2F(cx, cy + DpiScaleF(0.4f, dpi)), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(cx, cy + DpiScaleF(0.4f, dpi)),
                  D2D1::Point2F(cx + DpiScaleF(2.5f, dpi), cy + DpiScaleF(1.8f, dpi)), pBrush, sw, pStroke);
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
        -DpiScale(13, state->dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
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
        if (wParam == VK_DELETE && !state->history.empty()) {
            int targetIdx = (state->hoveredRow >= 0 && state->hoveredRow < (int)state->history.size())
                            ? state->hoveredRow
                            : state->selectedRow;
            if (targetIdx >= 0 && targetIdx < (int)state->history.size()) {
                int oldHeight = state->totalHeight;
                RemoveCommandFromHistoryStorage(state->history[targetIdx].cmd);
                state->hoveredRow = -1;
                state->selectedRow = -1;
                LoadHistoryItems(state);
                RecomputeDialogMetrics(state);
                state->normalY += (oldHeight - state->totalHeight);
                int dlgW = GetDlgWidth(state);
                ClampDialogToMonitorWorkArea(state->hDlg, &state->normalX, &state->normalY, dlgW, state->totalHeight, state->dpi);
                SetWindowPos(state->hDlg, nullptr, state->normalX, state->normalY, dlgW, state->totalHeight,
                             SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
                InvalidateRect(state->hDlg, nullptr, TRUE);
                return 0;
            }
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
    KillTimer(hWnd, kTimerLuminance);
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
}

LRESULT CALLBACK RunDlgSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

void PerformDialogUninit(HWND hWnd, RunDialogState* state) {
    RemoveWindowSubclass(hWnd, RunDlgSubclassProc, 1);
    CleanupDialogState(hWnd, state);
    HWND hCancel = GetDlgItem(hWnd, IDCANCEL);
    if (hCancel) EnableWindow(hCancel, TRUE);
    PostMessageW(hWnd, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)hCancel);
}

LRESULT CALLBACK RunDlgSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto* state = reinterpret_cast<RunDialogState*>(dwRefData);

    if (uMsg == g_msgUninit && g_msgUninit != 0) {
        if (!state) {
            return 1;
        }
        // If the dialog thread is inside a nested modal call (Browse file picker or IDOK error box),
        // close the modal popup and return 0 so Wh_ModBeforeUninit retries after the modal loop exits.
        if (state->isBrowsingFile || state->isTriggeringRun) {
            if (!state->pendingUninit) {
                state->pendingUninit = true;
                HWND hPopup = GetWindow(hWnd, GW_ENABLEDPOPUP);
                if (hPopup && hPopup != hWnd && IsWindow(hPopup)) {
                    PostMessageW(hPopup, WM_CLOSE, 0, 0);
                }
            }
            return 0;
        }
        PerformDialogUninit(hWnd, state);
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
                int oldHeight = state->totalHeight;
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    state->settings = g_settings;
                    state->i18n = g_i18n;
                }
                LoadHistoryItems(state);
                RecomputeDialogMetrics(state);
                state->normalY += (oldHeight - state->totalHeight);
                int dlgW = GetDlgWidth(state);
                ClampDialogToMonitorWorkArea(hWnd, &state->normalX, &state->normalY, dlgW, state->totalHeight, state->dpi);
                if (!IsIconic(hWnd)) {
                    SetWindowPos(hWnd, nullptr, state->normalX, state->normalY, dlgW, state->totalHeight,
                                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
                }
                ApplyWindowAppearance(hWnd, state);
                InvalidateRect(hWnd, nullptr, TRUE);
                break;
            }
        }
        return 0;
    }

    switch (uMsg) {
        case WM_NCCALCSIZE:
            return 0;

        case WM_NCACTIVATE:
            return TRUE;

        case WM_NCPAINT:
            return 0;

        case WM_STYLECHANGING: {
            if (wParam == GWL_STYLE && lParam) {
                auto* ss = reinterpret_cast<STYLESTRUCT*>(lParam);
                ss->styleNew &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
                ss->styleNew |= (WS_SYSMENU | WS_MINIMIZEBOX);
            }
            break;
        }

        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED: {
            if (state) {
                ApplyWindowAppearance(hWnd, state);
                InvalidateRect(hWnd, nullptr, TRUE);
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
                    if (state->effectiveSurface == SurfaceTheme::Glass && state->settings.colorMode == ColorMode::Auto) {
                        bool newSample = SampleBackdropLuminanceIsLight(
                            state->normalX, state->normalY, GetDlgWidth(state), state->totalHeight, true, state->backdropLightSample);
                        if (newSample != state->backdropLightSample) {
                            state->backdropLightSample = newSample;
                            ApplyWindowAppearance(hWnd, state);
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                    }
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
            if (wParam == kTimerLuminance && state) {
                if (!IsIconic(hWnd) && state->effectiveSurface == SurfaceTheme::Glass && state->settings.colorMode == ColorMode::Auto) {
                    bool newSample = SampleBackdropLuminanceIsLight(
                        state->normalX, state->normalY, GetDlgWidth(state), state->totalHeight, true, state->backdropLightSample);
                    if (newSample != state->backdropLightSample) {
                        state->backdropLightSample = newSample;
                        ApplyWindowAppearance(hWnd, state);
                        InvalidateRect(hWnd, nullptr, FALSE);
                    }
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

            bool pointerMoved = (state->effectiveSurface == SurfaceTheme::Glass) &&
                                (pt.x != state->mousePt.x || pt.y != state->mousePt.y);
            state->mousePt = pt;
            state->hasMousePt = true;

            if (newHoverRow != state->hoveredRow ||
                newBrowseHover != state->browseHovered ||
                newBtnHover != state->btnHovered ||
                newCloseHover != state->closeHovered ||
                pointerMoved) {
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
            if (state->hoveredRow != -1 || state->pressedRow != -1 || state->hasMousePt ||
                state->browseHovered || state->browsePressed ||
                state->btnHovered || state->btnPressed ||
                state->closeHovered || state->closePressed) {
                state->hoveredRow    = -1;
                state->pressedRow    = -1;
                state->hasMousePt    = false;
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
                state->pressedRow = row;
                SetCapture(hWnd);
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

            if (state->pressedRow != -1) {
                int targetRow = state->pressedRow;
                state->pressedRow = -1;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                int releasedRow = HitTestHistoryRow(state, pt);
                if (releasedRow == targetRow && targetRow >= 0 && targetRow < (int)state->history.size()) {
                    ExecuteCommandImmediate(state, state->history[targetRow].cmd);
                }
                return 0;
            }
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

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            if (IsIconic(hWnd) || !state || !g_pD2DFactory || !g_pDWriteFactory) {
                EndPaint(hWnd, &ps);
                return 0;
            }

            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            int w = rcClient.right;
            int h = rcClient.bottom;
            UINT dpi = state->dpi;
            bool isLight = state->isLightMode;
            SurfaceTheme surface = state->effectiveSurface;
            bool highContrast = IsHighContrastActive();

            if (w > 0 && h > 0) {
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

                    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                        96.0f, 96.0f);

                    ID2D1DCRenderTarget* pRT = nullptr;
                    if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&rtProps, &pRT)) && pRT) {
                        pRT->BindDC(hMemDC, &rcClient);
                        pRT->BeginDraw();
                        pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                        pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

                        // 1. Paint Surface Backdrop (Liquid Glass / Windows Blur / Solid / High Contrast)
                        if (highContrast) {
                            COLORREF winBg = GetSysColor(COLOR_WINDOW);
                            pRT->Clear(D2D1::ColorF(GetRValue(winBg) / 255.0f, GetGValue(winBg) / 255.0f, GetBValue(winBg) / 255.0f, 1.0f));
                        } else if (surface == SurfaceTheme::Glass) {
                            pRT->Clear(isLight ? D2D1::ColorF(0.95f, 0.96f, 0.98f, 0.34f)
                                               : D2D1::ColorF(0.07f, 0.07f, 0.09f, 0.32f));

                            D2D1_GRADIENT_STOP sheenStops[3] = {
                                { 0.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, isLight ? 0.16f : 0.10f) },
                                { 0.45f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.01f) },
                                { 1.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, isLight ? 0.09f : 0.06f) }
                            };
                            ID2D1GradientStopCollection* pSheenStops = nullptr;
                            if (SUCCEEDED(pRT->CreateGradientStopCollection(sheenStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pSheenStops))) {
                                ID2D1LinearGradientBrush* pSheenBr = nullptr;
                                D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                                    D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F((float)w, (float)h));
                                if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pSheenStops, &pSheenBr))) {
                                    pRT->FillRectangle(D2D1::RectF(0.0f, 0.0f, (float)w, (float)h), pSheenBr);
                                    pSheenBr->Release();
                                }
                                pSheenStops->Release();
                            }

                            if (state->hasMousePt) {
                                DrawD2DPointerSpecularGlow(pRT, state->mousePt, w, h, dpi, isLight);
                            }
                        } else if (surface == SurfaceTheme::Blur) {
                            pRT->Clear(isLight ? D2D1::ColorF(0.96f, 0.97f, 0.99f, 0.20f)
                                               : D2D1::ColorF(0.08f, 0.08f, 0.10f, 0.22f));
                        } else {
                            pRT->Clear(isLight ? D2D1::ColorF(0.96f, 0.96f, 0.98f, 1.0f)
                                               : D2D1::ColorF(0.12f, 0.12f, 0.15f, 1.0f));
                        }

                        // 2. Create Brushes & Round-Cap Stroke Style
                        D2D1_COLOR_F colPrimary, colSecondary, colTime, colHint;
                        if (highContrast) {
                            COLORREF fg = GetSysColor(COLOR_WINDOWTEXT);
                            colPrimary = colSecondary = colTime = colHint =
                                D2D1::ColorF(GetRValue(fg) / 255.0f, GetGValue(fg) / 255.0f, GetBValue(fg) / 255.0f, 1.0f);
                        } else {
                            colPrimary   = isLight ? D2D1::ColorF(0.08f, 0.08f, 0.10f, 1.00f) : D2D1::ColorF(0.96f, 0.96f, 0.97f, 1.00f);
                            colSecondary = isLight ? D2D1::ColorF(0.15f, 0.16f, 0.20f, 0.72f) : D2D1::ColorF(0.94f, 0.94f, 0.96f, 0.68f);
                            colTime      = isLight ? D2D1::ColorF(0.18f, 0.20f, 0.24f, 0.66f) : D2D1::ColorF(0.92f, 0.93f, 0.96f, 0.64f);
                            colHint      = isLight ? D2D1::ColorF(0.20f, 0.22f, 0.26f, 0.56f) : D2D1::ColorF(0.90f, 0.91f, 0.95f, 0.54f);
                        }

                        ID2D1SolidColorBrush* pBrPrimary = nullptr;
                        ID2D1SolidColorBrush* pBrSecondary = nullptr;
                        ID2D1SolidColorBrush* pBrTime = nullptr;
                        ID2D1SolidColorBrush* pBrHint = nullptr;
                        pRT->CreateSolidColorBrush(colPrimary, &pBrPrimary);
                        pRT->CreateSolidColorBrush(colSecondary, &pBrSecondary);
                        pRT->CreateSolidColorBrush(colTime, &pBrTime);
                        pRT->CreateSolidColorBrush(colHint, &pBrHint);

                        ID2D1StrokeStyle* pRoundStroke = nullptr;
                        D2D1_STROKE_STYLE_PROPERTIES strokeProps = D2D1::StrokeStyleProperties(
                            D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND);
                        g_pD2DFactory->CreateStrokeStyle(strokeProps, nullptr, 0, &pRoundStroke);

                        // 3. Create DirectWrite Text Formats
                        IDWriteTextFormat* pFmtRow = nullptr;
                        IDWriteTextFormat* pFmtTime = nullptr;
                        IDWriteTextFormat* pFmtInput = nullptr;
                        IDWriteTextFormat* pFmtBtn = nullptr;

                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.5f, dpi), L"", &pFmtRow);
                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.0f, dpi), L"", &pFmtTime);
                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.5f, dpi), L"", &pFmtInput);
                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.0f, dpi), L"", &pFmtBtn);

                        if (pFmtRow) {
                            pFmtRow->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtRow->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                            IDWriteInlineObject* pEllipsis = nullptr;
                            if (SUCCEEDED(g_pDWriteFactory->CreateEllipsisTrimmingSign(pFmtRow, &pEllipsis))) {
                                DWRITE_TRIMMING trim = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
                                pFmtRow->SetTrimming(&trim, pEllipsis);
                                pEllipsis->Release();
                            }
                        }
                        if (pFmtTime) {
                            pFmtTime->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                            pFmtTime->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtTime->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                        }
                        if (pFmtInput) {
                            pFmtInput->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtInput->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                        }
                        if (pFmtBtn) {
                            pFmtBtn->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                            pFmtBtn->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtBtn->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                        }

                        int padTop = GetListPadTop(state);
                        int rowHInt = GetRowHeight(state);
                        float pillRadius = DpiScaleF(6.5f, dpi);
                        float textLeftX = DpiScaleF(46.0f, dpi);

                        // 4. Draw History Rows
                        for (size_t i = 0; i < state->history.size(); ++i) {
                            float rowTop = (float)(padTop + (int)i * rowHInt);
                            float rowH   = (float)rowHInt;
                            bool isHovered  = ((int)i == state->hoveredRow);
                            bool isPressed  = ((int)i == state->pressedRow);
                            bool isSelected = ((int)i == state->selectedRow);

                            if (isHovered || isPressed || isSelected) {
                                float padX = DpiScaleF(10.0f, dpi);
                                float padY = DpiScaleF(2.0f, dpi);
                                D2D1_RECT_F rcPill = D2D1::RectF(padX, rowTop + padY, (float)w - padX, rowTop + rowH - padY);
                                if (surface == SurfaceTheme::Glass) {
                                    D2D1_COLOR_F rowTint = isLight
                                        ? D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.26f : 0.14f)
                                        : D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.16f : 0.08f);
                                    DrawD2DGlassButton(pRT, rcPill, pillRadius, dpi, rowTint, isHovered, isPressed, isLight);
                                } else {
                                    ID2D1SolidColorBrush* pHlBr = nullptr;
                                    D2D1_COLOR_F hlCol = isLight
                                        ? D2D1::ColorF(0.0f, 0.0f, 0.0f, isPressed ? 0.12f : 0.07f)
                                        : D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.16f : 0.10f);
                                    if (SUCCEEDED(pRT->CreateSolidColorBrush(hlCol, &pHlBr))) {
                                        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcPill, pillRadius, pillRadius), pHlBr);
                                        pHlBr->Release();
                                    }
                                }
                            }

                            float rightLimit = (float)w - DpiScaleF(18.0f, dpi);

                            if (state->settings.showTimestamp && pFmtTime && pBrTime) {
                                rightLimit = (float)w - DpiScaleF(42.0f, dpi);
                                const std::wstring& tStr = state->history[i].timeStr;
                                if (!tStr.empty()) {
                                    IDWriteTextLayout* pTimeLayout = nullptr;
                                    if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                            tStr.c_str(), (UINT32)tStr.size(), pFmtTime, DpiScaleF(160.0f, dpi), rowH, &pTimeLayout))) {
                                        DWRITE_TEXT_METRICS tm = {};
                                        pTimeLayout->GetMetrics(&tm);
                                        float timeRight = (float)w - DpiScaleF(42.0f, dpi);
                                        pRT->DrawTextLayout(D2D1::Point2F(timeRight - DpiScaleF(160.0f, dpi), rowTop),
                                                            pTimeLayout, pBrTime);
                                        rightLimit = timeRight - tm.widthIncludingTrailingWhitespace - DpiScaleF(12.0f, dpi);
                                        pTimeLayout->Release();
                                    }
                                }
                                DrawD2DClockVector(pRT, pBrTime, pRoundStroke,
                                                   (float)w - DpiScaleF(26.0f, dpi), rowTop + rowH * 0.5f, dpi);
                            }

                            // Single IDWriteTextLayout for "cmd · App Name" so Semibold + Regular share the exact same baseline
                            if (pFmtRow && pBrPrimary) {
                                const std::wstring& cmdStr = state->history[i].cmd;
                                const std::wstring& appStr = state->history[i].appName;
                                std::wstring fullStr = cmdStr;
                                if (!appStr.empty()) {
                                    fullStr += L"  \u00B7  " + appStr;
                                }

                                float availW = (std::max)(DpiScaleF(40.0f, dpi), rightLimit - textLeftX);
                                IDWriteTextLayout* pRowLayout = nullptr;
                                if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                        fullStr.c_str(), (UINT32)fullStr.size(), pFmtRow, availW, rowH, &pRowLayout))) {
                                    DWRITE_TEXT_RANGE cmdRange = { 0, (UINT32)cmdStr.size() };
                                    pRowLayout->SetFontWeight(DWRITE_FONT_WEIGHT_SEMI_BOLD, cmdRange);

                                    if (!appStr.empty() && pBrSecondary) {
                                        DWRITE_TEXT_RANGE appRange = {
                                            (UINT32)cmdStr.size(),
                                            (UINT32)(fullStr.size() - cmdStr.size())
                                        };
                                        pRowLayout->SetDrawingEffect(pBrSecondary, appRange);
                                    }

                                    pRT->DrawTextLayout(D2D1::Point2F(textLeftX, rowTop),
                                                        pRowLayout, pBrPrimary, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                                    pRowLayout->Release();
                                }
                            }
                        }

                        // Subtle divider line between history list and input bar
                        if (state->topAreaHeight > 0) {
                            ID2D1SolidColorBrush* pDivBr = nullptr;
                            D2D1_COLOR_F divCol = isLight ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.10f)
                                                          : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f);
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(divCol, &pDivBr))) {
                                float divX = DpiScaleF(12.0f, dpi);
                                pRT->DrawLine(D2D1::Point2F(divX, (float)state->topAreaHeight),
                                              D2D1::Point2F((float)w - divX, (float)state->topAreaHeight),
                                              pDivBr, DpiScaleF(1.0f, dpi));
                                pDivBr->Release();
                            }
                        }

                        // 5. Draw Input Box with DirectWrite HitTestTextPosition for caret & selection placement
                        RECT rcEditWin = GetModernEditRect(state);
                        D2D1_RECT_F rcEdit = D2D1::RectF((float)rcEditWin.left, (float)rcEditWin.top,
                                                         (float)rcEditWin.right, (float)rcEditWin.bottom);
                        float editW = rcEdit.right - rcEdit.left;
                        float editH = rcEdit.bottom - rcEdit.top;

                        std::wstring displayStr = GetEffectiveInputText(state->hModernEdit);
                        UINT32 textLen = (UINT32)displayStr.size();

                        DWORD selStart = 0, selEnd = 0;
                        if (state->hModernEdit) {
                            SendMessageW(state->hModernEdit, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);
                        }
                        if (selStart > textLen) selStart = textLen;
                        if (selEnd > textLen)   selEnd = textLen;
                        if (textLen > 0 && selEnd == 0 && selStart == 0) {
                            selEnd = textLen;
                        }

                        pRT->PushAxisAlignedClip(rcEdit, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

                        float caretXPos = 0.0f;
                        float scrollOffsetX = 0.0f;

                        if (textLen == 0) {
                            if (pFmtInput && pBrHint) {
                                pRT->DrawText(state->i18n.placeholder, (UINT32)wcslen(state->i18n.placeholder),
                                              pFmtInput, rcEdit, pBrHint);
                            }
                        } else if (pFmtInput && pBrPrimary) {
                            IDWriteTextLayout* pInputLayout = nullptr;
                            if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                    displayStr.c_str(), textLen, pFmtInput, 10000.0f, editH, &pInputLayout))) {
                                FLOAT hitX = 0.0f, hitY = 0.0f;
                                DWRITE_HIT_TEST_METRICS htm = {};
                                pInputLayout->HitTestTextPosition(selEnd, FALSE, &hitX, &hitY, &htm);
                                caretXPos = hitX;

                                float scrollPad = DpiScaleF(6.0f, dpi);
                                if (caretXPos > editW - scrollPad) {
                                    scrollOffsetX = caretXPos - (editW - scrollPad);
                                }

                                if (selStart != selEnd) {
                                    DWORD s0 = (std::min)(selStart, selEnd);
                                    DWORD s1 = (std::max)(selStart, selEnd);
                                    FLOAT x0 = 0.0f, y0 = 0.0f, x1 = 0.0f, y1 = 0.0f;
                                    DWRITE_HIT_TEST_METRICS m0 = {}, m1 = {};
                                    pInputLayout->HitTestTextPosition(s0, FALSE, &x0, &y0, &m0);
                                    pInputLayout->HitTestTextPosition(s1, FALSE, &x1, &y1, &m1);

                                    D2D1_COLOR_F accent = GetSystemAccentColorF();
                                    accent.a = 0.45f;
                                    ID2D1SolidColorBrush* pSelBr = nullptr;
                                    if (SUCCEEDED(pRT->CreateSolidColorBrush(accent, &pSelBr))) {
                                        float padY = DpiScaleF(2.0f, dpi);
                                        pRT->FillRectangle(
                                            D2D1::RectF(rcEdit.left + x0 - scrollOffsetX, rcEdit.top + padY,
                                                        rcEdit.left + x1 - scrollOffsetX, rcEdit.bottom - padY),
                                            pSelBr);
                                        pSelBr->Release();
                                    }
                                }

                                pRT->DrawTextLayout(D2D1::Point2F(rcEdit.left - scrollOffsetX, rcEdit.top),
                                                    pInputLayout, pBrPrimary);
                                pInputLayout->Release();
                            }
                        }

                        bool editFocused = (GetFocus() == state->hModernEdit) ||
                                           (GetForegroundWindow() == hWnd && !state->isBrowsingFile);
                        if (editFocused && state->caretVisible && pBrPrimary) {
                            float cx = rcEdit.left + caretXPos - scrollOffsetX + DpiScaleF(0.8f, dpi);
                            cx = std::clamp(cx, rcEdit.left + DpiScaleF(0.8f, dpi), rcEdit.right - DpiScaleF(1.5f, dpi));
                            float padY = DpiScaleF(3.5f, dpi);
                            pRT->DrawLine(D2D1::Point2F(cx, rcEdit.top + padY),
                                          D2D1::Point2F(cx, rcEdit.bottom - padY),
                                          pBrPrimary, DpiScaleF(1.25f, dpi));
                        }

                        pRT->PopAxisAlignedClip();

                        // 6. Draw Action Pill Buttons
                        auto drawPillButton = [&](RECT rWin, const wchar_t* label, bool isPrimaryAccent,
                                                  bool hovered, bool pressed) {
                            D2D1_RECT_F rcBtn = D2D1::RectF((float)rWin.left, (float)rWin.top,
                                                            (float)rWin.right, (float)rWin.bottom);
                            if (surface == SurfaceTheme::Glass) {
                                D2D1_COLOR_F overlay = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f);
                                if (isPrimaryAccent) {
                                    D2D1_COLOR_F acc = GetSystemAccentColorF();
                                    float alpha = pressed ? 0.44f : (hovered ? 0.36f : 0.28f);
                                    overlay = D2D1::ColorF(acc.r, acc.g, acc.b, alpha);
                                } else {
                                    overlay = isLight
                                        ? D2D1::ColorF(1.0f, 1.0f, 1.0f, pressed ? 0.22f : (hovered ? 0.14f : 0.06f))
                                        : D2D1::ColorF(1.0f, 1.0f, 1.0f, pressed ? 0.14f : (hovered ? 0.08f : 0.03f));
                                }
                                DrawD2DGlassButton(pRT, rcBtn, pillRadius, dpi, overlay, hovered, pressed, isLight);
                            } else {
                                D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rcBtn, pillRadius, pillRadius);
                                D2D1_COLOR_F fillCol;
                                if (isPrimaryAccent) {
                                    D2D1_COLOR_F acc = GetSystemAccentColorF();
                                    fillCol = D2D1::ColorF(acc.r, acc.g, acc.b, pressed ? 0.80f : (hovered ? 0.95f : 0.88f));
                                } else {
                                    fillCol = isLight
                                        ? D2D1::ColorF(0.0f, 0.0f, 0.0f, pressed ? 0.14f : (hovered ? 0.09f : 0.06f))
                                        : D2D1::ColorF(1.0f, 1.0f, 1.0f, pressed ? 0.18f : (hovered ? 0.12f : 0.08f));
                                }
                                ID2D1SolidColorBrush* pFillBr = nullptr;
                                if (SUCCEEDED(pRT->CreateSolidColorBrush(fillCol, &pFillBr))) {
                                    pRT->FillRoundedRectangle(rr, pFillBr);
                                    pFillBr->Release();
                                }
                            }

                            if (pFmtBtn) {
                                D2D1_COLOR_F txtCol = (isPrimaryAccent && surface != SurfaceTheme::Glass)
                                    ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)
                                    : colPrimary;
                                if (pressed && surface == SurfaceTheme::Glass) {
                                    txtCol.a *= 0.78f;
                                }
                                ID2D1SolidColorBrush* pTxtBr = nullptr;
                                if (SUCCEEDED(pRT->CreateSolidColorBrush(txtCol, &pTxtBr))) {
                                    pRT->DrawText(label, (UINT32)wcslen(label), pFmtBtn, rcBtn, pTxtBr);
                                    pTxtBr->Release();
                                }
                            }
                        };

                        drawPillButton(GetBrowseButtonRect(state), state->i18n.browseBtn, false,
                                       state->browseHovered, state->browsePressed);
                        drawPillButton(GetRunButtonRect(state), state->i18n.runBtn, true,
                                       state->btnHovered, state->btnPressed);
                        drawPillButton(GetCloseButtonRect(state), state->i18n.exitBtn, false,
                                       state->closeHovered, state->closePressed);

                        // 7. Draw Outer Window Liquid Glass Dual Rim
                        if (surface == SurfaceTheme::Glass && !highContrast) {
                            float rimInset = DpiScaleF(0.6f, dpi);
                            D2D1_RECT_F rcWindowRim = D2D1::RectF(rimInset, rimInset,
                                                                  (float)w - rimInset, (float)h - rimInset);
                            DrawD2DGlassRim(pRT, rcWindowRim, DpiScaleF(8.0f, dpi), dpi, isLight, 1.3f);
                        }

                        pRT->EndDraw();

                        if (pFmtBtn)   pFmtBtn->Release();
                        if (pFmtInput) pFmtInput->Release();
                        if (pFmtTime)  pFmtTime->Release();
                        if (pFmtRow)   pFmtRow->Release();
                        if (pRoundStroke) pRoundStroke->Release();
                        if (pBrHint)      pBrHint->Release();
                        if (pBrTime)      pBrTime->Release();
                        if (pBrSecondary) pBrSecondary->Release();
                        if (pBrPrimary)   pBrPrimary->Release();
                        pRT->Release();
                    }

                    // 8. Draw Native App Icons onto the premultiplied DIB
                    int iconSize = DpiScale(18, dpi);
                    int iconX    = DpiScale(20, dpi);
                    int padTop   = GetListPadTop(state);
                    int rowHInt  = GetRowHeight(state);
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

    LONG_PTR style = GetWindowLongPtrW(hDlg, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
    style |= (WS_SYSMENU | WS_MINIMIZEBOX);
    SetWindowLongPtrW(hDlg, GWL_STYLE, style);

    LONG_PTR exStyle = GetWindowLongPtrW(hDlg, GWL_EXSTYLE);
    exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_COMPOSITED);
    exStyle |= WS_EX_APPWINDOW;
    SetWindowLongPtrW(hDlg, GWL_EXSTYLE, exStyle);

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

    int dlgW = GetDlgWidth(state);
    int newX = 100;
    int newY = 100;
    GetDefaultTaskbarAnchoredPosition(hDlg, dlgW, state->totalHeight, state->dpi, &newX, &newY);
    ClampDialogToMonitorWorkArea(hDlg, &newX, &newY, dlgW, state->totalHeight, state->dpi);

    state->normalX = newX;
    state->normalY = newY;

    state->backdropLightSample = SampleBackdropLuminanceIsLight(
        newX, newY, dlgW, state->totalHeight, false, false);

    ApplyWindowAppearance(hDlg, state);

    state->hModernEdit = CreateWindowExW(
        0, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        -32000, -32000, DpiScale(400, state->dpi), DpiScale(24, state->dpi),
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
        } else if (!state->history.empty()) {
            const std::wstring& latestCmd = state->history.front().cmd;
            SetWindowTextW(state->hModernEdit, latestCmd.c_str());
            SendMessageW(state->hModernEdit, EM_SETSEL, 0, -1);
            SyncInputToNativeCombo(state, latestCmd.c_str());
        }
        SetWindowSubclass(state->hModernEdit, ModernEditSubclassProc, 1, (DWORD_PTR)state);
    }

    SetWindowSubclass(hDlg, RunDlgSubclassProc, 1, (DWORD_PTR)state);
    SetTimer(hDlg, kTimerCaret, 530, nullptr);
    SetTimer(hDlg, kTimerLuminance, 2000, nullptr);

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
    if (!g_internalActivation && hActive && hWnd == hActive) {
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
    if (!g_internalActivation && hActive && hWnd == hActive && !g_isBrowsingFile.load()) {
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
    if (SwitchToThisWindow_Orig) {
        SwitchToThisWindow_Orig(hWnd, fUnknown);
    }
}

BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    HWND hActive = g_hActiveRunDlg.load();
    if (!g_internalActivation && hActive && hWnd == hActive && !g_isBrowsingFile.load()) {
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

void ReleaseGlobalResources() {
    if (g_pDWriteFactory) {
        g_pDWriteFactory->Release();
        g_pDWriteFactory = nullptr;
    }
    if (g_pD2DFactory) {
        g_pD2DFactory->Release();
        g_pD2DFactory = nullptr;
    }
}

BOOL Wh_ModInit() {
    g_unloading.store(false);
    g_glassCpuSupported = DetectGlassCpuSupport();

    // Clean up legacy v2.0.15 shared (non-SID) timestamp key if present
    Wh_DeleteValue(L"HistoryTimestamps");

    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory),
                                 nullptr, reinterpret_cast<void**>(&g_pD2DFactory)))) {
        Wh_Log(L"Failed to create Direct2D factory");
        ReleaseGlobalResources();
        return FALSE;
    }

    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                   reinterpret_cast<IUnknown**>(&g_pDWriteFactory)))) {
        Wh_Log(L"Failed to create DirectWrite factory");
        ReleaseGlobalResources();
        return FALSE;
    }

    LoadSettings(false);
    g_msgUninit = RegisterWindowMessageW(L"Windhawk_Uninit_" WH_MOD_ID);
    g_msgToggle = RegisterWindowMessageW(L"Windhawk_Toggle_" WH_MOD_ID);

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32) {
        Wh_Log(L"Failed to load shell32.dll");
        ReleaseGlobalResources();
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
        ReleaseGlobalResources();
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Orig);
    WindhawkUtils::SetFunctionHook(SetForegroundWindow, SetForegroundWindow_Hook, &SetForegroundWindow_Orig);
    WindhawkUtils::SetFunctionHook(SwitchToThisWindow, SwitchToThisWindow_Hook, &SwitchToThisWindow_Orig);

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings(true);
    if (HWND hDlg = g_hActiveRunDlg.load()) {
        PostMessageW(hDlg, g_msgToggle, kActionReloadSettings, 0);
    }
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true);
    HWND hDlg = g_hActiveRunDlg.load();
    while (hDlg && IsWindow(hDlg) && g_hActiveRunDlg.load() == hDlg &&
           SendMessageW(hDlg, g_msgUninit, 0, 0) != 1) {
        Sleep(50);
    }
    StopKeyHookThread();
}

void Wh_ModUninit() {
    StopKeyHookThread();
    ReleaseGlobalResources();
}
