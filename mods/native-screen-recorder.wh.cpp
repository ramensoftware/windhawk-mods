// ==WindhawkMod==
// @id              native-screen-recorder
// @name            Native Windows Screen Recorder
// @name:vi-VN      Trình Quay Màn Hình Native cho Windows
// @description     Lightweight native Windows screen recorder with 13 themes, a WinUI 3 Settings window, hardware-capped resolution & FPS, live screen drawing, custom multi-monitor layout, and 21 languages.
// @description:vi-VN Trình quay màn hình native cho Windows với 13 chủ đề, cửa sổ Cài đặt WinUI 3, độ phân giải & FPS theo phần cứng, vẽ trực tiếp lên màn hình, tự bố trí đa màn hình và 21 ngôn ngữ.
// @version         2.0.4
// @author          caotu2005
// @github          https://github.com/caotu2005
// @include         explorer.exe
// @include         windhawk.exe
// @compilerOptions -ld3d11 -ldxgi -lmfplat -lmfreadwrite -lmfuuid -lole32 -luuid -lgdi32 -luser32 -lshell32 -lshlwapi -ldwmapi -lwinmm -ld2d1 -ldwrite
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Native Windows Screen Recorder

> ℹ️ **Compatibility Note (Smart App Control) / Lưu ý tương thích:**
> - **🌐 English:** Like all Windhawk mods, this mod is compiled locally into an unsigned `.dll` file and loaded into `explorer.exe` and `windhawk.exe`. If Windows **Smart App Control** is active on your PC, Windows will block the compiled `.dll` and the mod will not load. See [Windhawk's documentation and guidance on Smart App Control](https://github.com/ramensoftware/windhawk/issues/696) for details.
> - **🇻🇳 Tiếng Việt:** Giống như mọi bản mod trên Windhawk, mod này được biên dịch cục bộ thành tệp `.dll` không có chữ ký số để nạp vào `explorer.exe` và `windhawk.exe`. Nếu máy đang bật **Smart App Control (Kiểm soát Ứng dụng Thông minh)**, Windows sẽ chặn tệp `.dll` này khiến mod không thể hoạt động. Xem thêm [hướng dẫn và thảo luận chính thức của Windhawk về Smart App Control](https://github.com/ramensoftware/windhawk/issues/696).

![Native Windows Screen Recorder Preview (Floating Pill Bar)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/nativerecord/main.jpg)

*Floating Pill Bar (13 Themes & Windows 11 Blur / Liquid Glass)*

![Native Windows Screen Recorder Preview (Active Recording HUD)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/nativerecord/record.jpg)

*Active Recording HUD*

![Native Windows Screen Recorder Preview (Live Screen Drawing)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/nativerecord/drawscreen.jpg)

*Live Screen Drawing / Annotation Toolbar & Esc Hint Pill*

![Native Windows Screen Recorder Preview (WinUI 3 Settings Window)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/nativerecord/settingsui.jpg)

*Dedicated WinUI 3 Settings Window (Hosted in `windhawk.exe`)*

![Native Windows Screen Recorder Preview (Multi-Monitor Layout Arranger)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/nativerecord/monitor.jpg)

*Interactive Multi-Monitor Custom Layout Arranger*

---

## 🌐 English

A lightweight, native screen recorder integrated directly into Windows (`explorer.exe`) with **13 built-in visual themes**, a **dedicated WinUI 3 Settings window hosted in `windhawk.exe`**, **hardware-capped resolution & active refresh-rate FPS selector**, **live screen drawing/annotation toolbar**, **multi-monitor custom layout arranger**, and **21-language support** — no third-party apps required.

### Key Features

- **Dedicated WinUI 3 Settings Window (Hosted in `windhawk.exe`):**
  * Clicking the `⚙️` button on the floating pill bar opens a standalone **Windows 11 Settings–style** window running in a dedicated `windhawk.exe -tool-mod` process rather than `explorer.exe`.
  * Built end-to-end with Direct2D WinUI 3 controls: grouped SettingsExpander cards, WinUI 3 ComboBox flyouts with vertical accent indicators, and a native-style WinUI 3 HSV ColorPicker with 16 Fluent swatches. Changes apply immediately to the live pill bar.

- **13 Built-In Themes & Full Visual Customization:**
  * Defaults to **Windows 11 Blur** (authentic frosted Fluent Acrylic), with **Liquid Glass** (refractive 3D glass pill) and 11 more curated themes (**Windows 11 Mica**, **Fluent Solid**, **Classic Win11**, **Midnight OLED**, **Nord Frost**, **Catppuccin Mocha**, **Dracula Velvet**, **Rosé Pine**, **Emerald Slate**, **Warm Paper**, and **Custom**).
  * Customize **Background Color**, **Border Color**, and **Corner Radius (`0–28 px`)** across the pill bar, drawing toolbar, and layout arranger, excluded from recorded video via `WDA_EXCLUDEFROMCAPTURE`.

- **Hardware-Capped Resolution & Active Refresh-Rate FPS:**
  * Automatically detects each monitor's native hardware resolution and active Windows display refresh rate (`ENUM_CURRENT_SETTINGS`), only exposing presets up to your current hardware limits (e.g., 60 Hz caps at 60 FPS; 144 Hz unlocks up to 144 FPS; 240 Hz unlocks up to 240 FPS) with automatic GPU H.264 encoder step-down fallback.

- **Multi-Monitor Capture & Interactive Custom Layout Arranger:**
  * Record the monitor under your cursor, pick any individual monitor (`Monitor 1`, `Monitor 2`, ...), record **All Monitors Combined**, or open the **Custom Monitor Layout** arranger directly from Settings to drag, snap, resize, and toggle monitors into any corner or Picture-in-Picture layout.

- **Live Screen Drawing / Annotation (`✏️`), WASAPI Audio & 21 Languages:**
  * Toggleable from the Idle Pill Bar or Active Recording HUD with **Pen**, **Highlighter**, **Line**, **Arrow**, **Rectangle**, **Ellipse**, **Eraser**, **Mouse Pass-through**, **7 colors**, **3 sizes**, **Undo / Redo**, **Clear All**, and a `"Press Esc to exit"` hint pill.
  * DXGI Desktop Duplication with automatic GDI fallback, Media Foundation H.264 + AAC (192 kbps) direct MP4 encoding, WASAPI System Audio Loopback + Microphone mixing, automatic Explorer reveal on finish, and **21 built-in languages**.

### Default Hotkeys

- **Ctrl + Alt + R:** Open / Close the floating Pill Launcher bar (or Stop if currently recording)
- **Ctrl + Alt + S:** Drag-select a custom screen region and start recording
- **Ctrl + Alt + P:** Pause / Resume active recording
- **Esc:** Exit Live Screen Drawing mode (or cancel region selection)

---

## 🇻🇳 Tiếng Việt

Trình quay màn hình native gọn nhẹ tích hợp trực tiếp trong Windows (`explorer.exe`) với **13 chủ đề giao diện**, **cửa sổ Cài đặt WinUI 3 độc lập chạy trên tiến trình `windhawk.exe`**, **tùy chỉnh độ phân giải & FPS giới hạn theo tần số quét thực tế của phần cứng**, **bộ công cụ vẽ trực tiếp lên màn hình khi quay**, **trình tự bố trí đa màn hình trực quan** và **hỗ trợ 21 ngôn ngữ** — không cần cài thêm phần mềm bên thứ 3.

### Tính năng chính

- **Cửa sổ Cài đặt WinUI 3 Độc Lập (Chạy trên `windhawk.exe`):**
  * Khi bấm nút `⚙️` trên thanh viên thuốc, cửa sổ **Cài đặt Quay Màn Hình** phong cách Windows 11 sẽ mở trong tiến trình riêng `windhawk.exe -tool-mod` thay vì chạy trong `explorer.exe`.
  * Thiết kế đồng bộ chuẩn WinUI 3 với thẻ nhóm SettingsExpander, menu thả xuống ComboBox Flyout có thanh nhấn dọc và bảng chọn màu WinUI 3 ColorPicker 16 ô màu nhanh Fluent — mọi thay đổi áp dụng tức thì lên thanh viên thuốc.

- **13 Chủ Đề Giao Diện & Tùy Chỉnh Màu Sắc, Bo Góc:**
  * Mặc định **Windows 11 Blur** (Acrylic mờ dịu mắt chuẩn Windows 11), kèm **Liquid Glass** (kính trong suốt khúc xạ 3D) và 11 chủ đề tinh chọn khác (**Windows 11 Mica**, **Fluent Solid**, **Classic Win11**, **Midnight OLED**, **Nord Frost**, **Catppuccin Mocha**, **Dracula Velvet**, **Rosé Pine**, **Emerald Slate**, **Warm Paper** và **Tự tùy chỉnh**).
  * Tự do đổi **Màu nền**, **Màu viền** và **Bán kính bo góc (`0–28 px`)** đồng bộ trên toàn bộ thanh viên thuốc, thanh vẽ và trình bố trí màn hình, tự ẩn khỏi video quay nhờ `WDA_EXCLUDEFROMCAPTURE`.

- **Độ Phân Giải & FPS Giới Hạn Thông Minh Theo Phần Cứng:**
  * Tự động nhận diện độ phân giải gốc và tần số quét Windows đang thiết lập (`ENUM_CURRENT_SETTINGS`), chỉ mở khóa mức FPS tương ứng (60Hz tối đa 60 FPS; 144Hz mở lên 144 FPS; 240Hz mở lên 240 FPS) kèm cơ chế tự hạ bậc an toàn theo bộ mã hóa H.264 của GPU.

- **Quay Đa Màn Hình & Tự Bố Trí Màn Hình Trực Quan:**
  * Chọn màn hình chứa con trỏ, từng màn hình riêng lẻ (`Màn hình 1`, `Màn hình 2`, ...), **quay tất cả màn hình cùng lúc**, hoặc mở trình **Tự bố trí màn hình** ngay từ cửa sổ Cài đặt để kéo thả, phóng to/thu nhỏ và sắp xếp các màn hình (Xếp ngang, Xếp dọc, Góc PiP).

- **Vẽ Trực Tiếp Lên Màn Hình (`✏️`), Thu Âm WASAPI & 21 Ngôn Ngữ:**
  * Trang bị đầy đủ **Bút vẽ**, **Dạ quang**, **Đường thẳng**, **Mũi tên**, **Hình chữ nhật**, **Elip**, **Tẩy**, **Xuyên chuột**, **7 màu**, **3 cỡ nét**, **Hoàn tác / Làm lại**, **Xóa hết** kèm viên thuốc nhắc `"Đang vẽ • Ấn Esc để thoát"`.
  * Ghi hình DXGI Desktop Duplication (tự chuyển GDI dự phòng), xuất MP4 H.264 + AAC (192 kbps), thu âm thanh hệ thống & Micro, tự mở thư mục chọn tệp video khi xong và hỗ trợ **21 ngôn ngữ phổ biến**.

### Phím tắt mặc định

- **Ctrl + Alt + R:** Mở / Ẩn thanh viên thuốc quay màn hình (hoặc Dừng nếu đang quay)
- **Ctrl + Alt + S:** Kéo chuột chọn vùng màn hình và quay ngay
- **Ctrl + Alt + P:** Tạm dừng / Tiếp tục quay
- **Esc:** Thoát chế độ vẽ lên màn hình (hoặc hủy chọn vùng)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- appearanceGroup:
  - themePreset: windows_blur
    $name: Theme Preset
    $name:vi-VN: Chủ đề giao diện (Theme Preset)
    $description: Choose from 13 built-in visual themes (Windows 11 Blur, Liquid Glass, Mica, OLED, Nord, Catppuccin, Dracula, and Custom).
    $description:vi-VN: Chọn trong 13 chủ đề có sẵn (mặc định Windows 11 Blur; hỗ trợ Liquid Glass, Mica, OLED, Nord, Catppuccin, Dracula và Tự tùy chỉnh).
    $options:
    - windows_blur: Windows 11 Blur (Recommended Default)
    - liquid_glass: Liquid Glass (Refractive Glass)
    - mica_alt: Windows 11 Mica (Soft Native Mica)
    - fluent_solid: Fluent Solid (Opaque Win11 Pill)
    - classic_win11: Classic Win11 (Subtle Corners & Border)
    - midnight_oled: Midnight OLED (Deep Black)
    - nord_frost: Nord Frost (Arctic Slate & Ice)
    - catppuccin_mocha: Catppuccin Mocha (Warm Dark & Lavender)
    - dracula_velvet: Dracula Velvet (Deep Plum & Violet)
    - rose_pine: Rosé Pine (Twilight & Rose Gold)
    - emerald_slate: Emerald Slate (Deep Forest & Mint)
    - warm_paper: Warm Paper (Soft Light Ivory)
    - custom: Custom Colors & Corner Radius
    $options:vi-VN:
    - windows_blur: Windows 11 Blur (Mặc định khuyên dùng)
    - liquid_glass: Liquid Glass (Kính trong suốt khúc xạ)
    - mica_alt: Windows 11 Mica (Bề mặt Mica mềm mại)
    - fluent_solid: Fluent Solid (Nền đặc Win11 hiện đại)
    - classic_win11: Classic Win11 (Bo góc nhẹ, viền tinh giản)
    - midnight_oled: Midnight OLED (Đen sâu tiết kiệm pin)
    - nord_frost: Nord Frost (Xám Bắc Âu viền băng)
    - catppuccin_mocha: Catppuccin Mocha (Tối dịu viền oải hương)
    - dracula_velvet: Dracula Velvet (Tím than viền tím sáng)
    - rose_pine: Rosé Pine (Tối trầm viền hồng vàng)
    - emerald_slate: Emerald Slate (Xanh rừng sâu viền ngọc)
    - warm_paper: Warm Paper (Nền giấy sáng dịu mắt)
    - custom: Tự tùy chỉnh màu & bo góc (Custom)
  - colorMode: auto
    $name: Color Mode (Light / Dark)
    $name:vi-VN: Chế độ màu Sáng / Tối
    $description: Follow Windows Light/Dark mode automatically, or choose a fixed mode.
    $description:vi-VN: Tự động theo giao diện Sáng/Tối của Windows hoặc chọn cố định.
    $options:
    - auto: Auto – Follow Windows
    - dark: Dark Mode
    - light: Light Mode
    $options:vi-VN:
    - auto: Tự động theo Windows
    - dark: Giao diện Tối (Dark)
    - light: Giao diện Sáng (Light)
  - customBgColor: ""
    $name: Custom Background Color (Hex, e.g. #1E1E24)
    $name:vi-VN: Màu nền tùy chỉnh (Hex, ví dụ #1E1E24)
    $description: Optional hex color (#RRGGBB) to override the recorder pill background color. Leave empty to follow Theme Preset.
    $description:vi-VN: Mã màu hex (#RRGGBB) để tự đổi màu nền thanh quay. Để trống để dùng theo Theme Preset đã chọn.
  - customBorderColor: ""
    $name: Custom Border Color (Hex, e.g. #4C566A)
    $name:vi-VN: Màu viền tùy chỉnh (Hex, ví dụ #4C566A)
    $description: Optional hex color (#RRGGBB) to override the recorder pill border color. Leave empty to follow Theme Preset.
    $description:vi-VN: Mã màu hex (#RRGGBB) để tự đổi màu viền thanh quay. Để trống để dùng theo Theme Preset đã chọn.
  - customCornerRadius: -1
    $name: Corner Radius in px (-1 = Theme Default, or 0–28)
    $name:vi-VN: Bán kính bo góc viền (-1 = Theo chủ đề, hoặc 0–28 px)
    $description: Set exact corner roundness in pixels (0 = square, 14 = Win11, 27 = full pill, -1 = Theme default).
    $description:vi-VN: Chỉnh độ bo góc theo pixel (0 = góc vuông, 14 = Win11, 27 = viên thuốc, -1 = theo chủ đề).
  - language: auto
    $name: Display Language
    $name:vi-VN: Ngôn ngữ hiển thị
    $description: Auto-detect your Windows UI language or choose from 21 built-in languages.
    $description:vi-VN: Tự động theo hệ điều hành hoặc chọn trong 21 ngôn ngữ tích hợp sẵn.
    $options:
    - auto: Auto – System Default
    - en: English
    - vi: Tiếng Việt (Vietnamese)
    - th: ไทย (Thai)
    - id: Bahasa Indonesia (Indonesian)
    - ms: Bahasa Melayu (Malay)
    - ja: 日本語 (Japanese)
    - zh_cn: 简体中文 (Simplified Chinese)
    - zh_tw: 繁體中文 (Traditional Chinese)
    - ko: 한국어 (Korean)
    - fr: Français (French)
    - de: Deutsch (German)
    - es: Español (Spanish)
    - pt: Português (Portuguese)
    - it: Italiano (Italian)
    - ru: Русский (Russian)
    - uk: Українська (Ukrainian)
    - pl: Polski (Polish)
    - nl: Nederlands (Dutch)
    - tr: Türkçe (Turkish)
    - hi: हिन्दी (Hindi)
    - ar: العربية (Arabic)
    $options:vi-VN:
    - auto: Tự động theo Windows
    - en: English (Tiếng Anh)
    - vi: Tiếng Việt
    - th: ไทย (Tiếng Thái)
    - id: Bahasa Indonesia (Tiếng Indonesia)
    - ms: Bahasa Melayu (Tiếng Mã Lai)
    - ja: 日本語 (Tiếng Nhật)
    - zh_cn: 简体中文 (Tiếng Trung Giản thể)
    - zh_tw: 繁體中文 (Tiếng Trung Phồn thể)
    - ko: 한국어 (Tiếng Hàn)
    - fr: Français (Tiếng Pháp)
    - de: Deutsch (Tiếng Đức)
    - es: Español (Tiếng Tây Ban Nha)
    - pt: Português (Tiếng Bồ Đào Nha)
    - it: Italiano (Tiếng Ý)
    - ru: Русский (Tiếng Nga)
    - uk: Українська (Tiếng Ukraina)
    - pl: Polski (Tiếng Ba Lan)
    - nl: Nederlands (Tiếng Hà Lan)
    - tr: Türkçe (Tiếng Thổ Nhĩ Kỳ)
    - hi: हिन्दी (Tiếng Hindi)
    - ar: العربية (Tiếng Ả Rập)
  $name: 1. Appearance & Themes
  $name:vi-VN: 1. Giao diện & 13 Chủ đề

- recordingGroup:
  - outputFolder: ""
    $name: Output folder
    $name:vi-VN: Thư mục lưu video
    $description: Folder where MP4 files are saved. Leave empty to use Videos\ScreenRecordings.
    $description:vi-VN: Thư mục lưu tệp MP4. Để trống để lưu tại Videos\ScreenRecordings.
  - videoResolution: "0"
    $name: Recording Resolution (never exceeds monitor hardware)
    $name:vi-VN: Độ phân giải quay (không vượt quá phần cứng màn hình)
    $options:
    - "0": Native (Monitor Hardware)
    - "2160": 2160p (4K)
    - "1440": 1440p (2K)
    - "1080": 1080p (Full HD)
    - "900": 900p (HD+)
    - "720": 720p (HD)
    - "480": 480p (SD)
    $options:vi-VN:
    - "0": Gốc (Theo phần cứng màn hình)
    - "2160": 2160p (4K)
    - "1440": 1440p (2K)
    - "1080": 1080p (Full HD)
    - "900": 900p (HD+)
    - "720": 720p (HD)
    - "480": 480p (SD)
  - fps: 60
    $name: Frame rate (FPS)
    $name:vi-VN: Tốc độ khung hình (FPS)
    $description: Target FPS (automatically capped to your active Windows monitor refresh rate and GPU H.264 encoder).
    $description:vi-VN: Tốc độ khung hình mục tiêu (tự động giới hạn theo tần số quét Windows đang đặt và GPU H.264).
    $options:
    - 30: 30 FPS
    - 60: 60 FPS
    - 75: 75 FPS
    - 90: 90 FPS (High Refresh)
    - 120: 120 FPS (High Refresh)
    - 144: 144 FPS (High Refresh)
    - 165: 165 FPS (High Refresh)
    - 240: 240 FPS (High Refresh)
  - videoBitrateMbps: 12
    $name: Video bitrate (Mbps)
    $name:vi-VN: Bitrate video (Mbps)
  - captureMonitor: cursor
    $name: Monitor selection (full-screen)
    $name:vi-VN: Màn hình quay (chế độ toàn màn hình)
    $description: Choose cursor monitor, a specific monitor (1, 2, ...), all monitors, or arrange a custom layout from Settings.
    $description:vi-VN: Chọn màn hình theo con trỏ, màn hình 1/2/..., quay tất cả màn hình, hoặc mở trình Tự bố trí màn hình trong Cài đặt.
    $options:
    - cursor: Monitor under mouse cursor
    - primary: Primary monitor
    - all: All monitors combined
    - custom: Custom monitor layout
    $options:vi-VN:
    - cursor: Màn hình chứa con trỏ chuột
    - primary: Màn hình chính
    - all: Tất cả màn hình cùng lúc
    - custom: Tự bố trí màn hình
  - recordCursor: true
    $name: Capture mouse cursor
    $name:vi-VN: Ghi lại con trỏ chuột
  - recordSystemAudio: true
    $name: Record system audio
    $name:vi-VN: Thu âm thanh hệ thống (WASAPI)
  - recordMicrophone: false
    $name: Record microphone
    $name:vi-VN: Thu âm từ Micro
  - showFloatingHud: true
    $name: Show floating pill HUD while recording
    $name:vi-VN: Hiển thị thanh viên thuốc khi đang quay
  - openFolderOnFinish: true
    $name: Reveal video in Explorer on finish
    $name:vi-VN: Mở thư mục chọn tệp MP4 khi quay xong
  $name: 2. Recording & Audio
  $name:vi-VN: 2. Cài đặt Quay & Âm thanh

- hotkeyGroup:
  - hotkeyModifiers: ctrl+alt
    $name: Hotkey modifiers
    $name:vi-VN: Phím bổ trợ (Modifiers)
    $options:
    - ctrl+alt: Ctrl + Alt
    - ctrl+shift: Ctrl + Shift
    - alt+shift: Alt + Shift
    - win+alt: Win + Alt
  - hotkeyRecordKey: "R"
    $name: Toggle Pill Bar / Stop Recording key
    $name:vi-VN: Phím mở thanh Viên thuốc / Dừng quay
  - hotkeyRegionKey: "S"
    $name: Select region & record key
    $name:vi-VN: Phím chọn vùng quay nhanh
  - hotkeyPauseKey: "P"
    $name: Pause / Resume key
    $name:vi-VN: Phím Tạm dừng / Tiếp tục
  $name: 3. Global Hotkeys
  $name:vi-VN: 3. Phím tắt toàn cục
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1.h>
#include <dwrite.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <ksmedia.h>
#include <dwmapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <mmsystem.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif

#ifndef MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS
static const GUID kMF_READWRITE_ENABLE_HARDWARE_TRANSFORMS = {
    0xa634a91c, 0x822b, 0x41b9, {0xa4, 0x94, 0x4d, 0xe4, 0x64, 0x36, 0x12, 0xb0}
};
#define MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS kMF_READWRITE_ENABLE_HARDWARE_TRANSFORMS
#endif

extern "C" IMAGE_DOS_HEADER __ImageBase;

static inline HINSTANCE GetModInstanceHandle() {
    return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

// Explicit WASAPI GUIDs for MinGW compatibility
static const CLSID kCLSID_MMDeviceEnumerator = {
    0xbcde0395, 0xe52f, 0x467c, {0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e}
};
static const IID kIID_IMMDeviceEnumerator = {
    0xa95664d2, 0x9614, 0x4f35, {0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6}
};
static const IID kIID_IAudioClient = {
    0x1cb9ad4c, 0xdbfa, 0x4c32, {0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2}
};
static const IID kIID_IAudioCaptureClient = {
    0xc8adbd64, 0xe71e, 0x48a0, {0xa4, 0xde, 0x18, 0x5c, 0x39, 0x5c, 0xd3, 0x17}
};
static const GUID kKSDATAFORMAT_SUBTYPE_IEEE_FLOAT = {
    0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
};

template <typename T>
class ComPtr {
public:
    ComPtr() = default;
    ComPtr(std::nullptr_t) {}
    ComPtr(const ComPtr& other) : ptr_(other.ptr_) { if (ptr_) ptr_->AddRef(); }
    ComPtr(ComPtr&& other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }
    ~ComPtr() { Reset(); }

    ComPtr& operator=(const ComPtr& other) {
        if (this != &other) { Reset(); ptr_ = other.ptr_; if (ptr_) ptr_->AddRef(); }
        return *this;
    }
    ComPtr& operator=(ComPtr&& other) noexcept {
        if (this != &other) { Reset(); ptr_ = other.ptr_; other.ptr_ = nullptr; }
        return *this;
    }
    ComPtr& operator=(std::nullptr_t) { Reset(); return *this; }

    void Reset() { if (ptr_) { ptr_->Release(); ptr_ = nullptr; } }
    T* Get() const { return ptr_; }
    T** GetAddressOf() { return &ptr_; }
    T* operator->() const { return ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }

private:
    T* ptr_ = nullptr;
};

// ============================================================================
// Settings, Localization & Global State
// ============================================================================

enum class SurfaceTheme { Glass, Blur, Solid };
enum class ColorMode { Auto, Dark, Light };

struct ModSettings {
    std::wstring themePreset = L"windows_blur";
    SurfaceTheme surfaceTheme = SurfaceTheme::Blur;
    ColorMode colorMode = ColorMode::Auto;
    std::wstring customBgColor;
    std::wstring customBorderColor;
    int customCornerRadius = -1;
    int glassOpacity = -100;
    int blurOpacity = 255;
    std::wstring language = L"auto";

    std::wstring outputFolder;
    int targetResHeight = 0; // 0 = Native hardware resolution; never exceeds monitor hardware
    int fps = 60;
    int videoBitrateMbps = 12;
    std::wstring captureMonitorMode = L"cursor";
    bool recordCursor = true;
    bool recordSystemAudio = true;
    bool recordMicrophone = false;
    bool showFloatingHud = true;
    bool openFolderOnFinish = true;

    UINT hotkeyModifiers = MOD_CONTROL | MOD_ALT;
    UINT hotkeyRecordVk = 'R';
    UINT hotkeyRegionVk = 'S';
    UINT hotkeyPauseVk = 'P';
};


static bool TryParseHexColor(const std::wstring& raw, COLORREF* outRgb) {
    std::wstring s = raw;
    while (!s.empty() && iswspace(s.front())) s.erase(s.begin());
    while (!s.empty() && iswspace(s.back())) s.pop_back();
    if (!s.empty() && s.front() == L'#') s.erase(s.begin());
    if (s.size() != 6) return false;
    unsigned int val = 0;
    for (wchar_t ch : s) {
        val <<= 4;
        if (ch >= L'0' && ch <= L'9') val |= static_cast<unsigned int>(ch - L'0');
        else if (ch >= L'a' && ch <= L'f') val |= static_cast<unsigned int>(10 + (ch - L'a'));
        else if (ch >= L'A' && ch <= L'F') val |= static_cast<unsigned int>(10 + (ch - L'A'));
        else return false;
    }
    if (outRgb) {
        *outRgb = RGB((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
    }
    return true;
}

static std::wstring FormatHexColor(COLORREF rgb) {
    wchar_t buf[16] = {};
    swprintf_s(buf, L"#%02X%02X%02X", static_cast<int>(GetRValue(rgb)), static_cast<int>(GetGValue(rgb)), static_cast<int>(GetBValue(rgb)));
    return buf;
}

struct ResolvedThemeVisuals {
    SurfaceTheme surface = SurfaceTheme::Blur;
    bool hasCustomBg = false;
    D2D1_COLOR_F bgColor = D2D1::ColorF(0.12f, 0.12f, 0.15f, 1.0f);
    bool hasCustomBorder = false;
    D2D1_COLOR_F borderColor = D2D1::ColorF(0.32f, 0.34f, 0.40f, 1.0f);
    float cornerRadiusDip = 14.0f;
};

static ResolvedThemeVisuals ResolveEffectiveThemeVisuals(const ModSettings& s, bool isLight) {
    ResolvedThemeVisuals v{};
    const std::wstring& p = s.themePreset;
    if (p == L"liquid_glass") {
        v.surface = SurfaceTheme::Glass;
        v.cornerRadiusDip = 27.0f;
    } else if (p == L"mica_alt") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = isLight ? D2D1::ColorF(0.953f, 0.957f, 0.973f, 0.97f)
                            : D2D1::ColorF(0.133f, 0.137f, 0.153f, 0.97f);
        v.hasCustomBorder = true;
        v.borderColor = isLight ? D2D1::ColorF(0.840f, 0.850f, 0.880f, 0.95f)
                                : D2D1::ColorF(0.265f, 0.275f, 0.305f, 0.95f);
        v.cornerRadiusDip = 12.0f;
    } else if (p == L"fluent_solid") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = isLight ? D2D1::ColorF(0.965f, 0.968f, 0.980f, 0.98f)
                            : D2D1::ColorF(0.125f, 0.128f, 0.145f, 0.98f);
        v.hasCustomBorder = true;
        v.borderColor = isLight ? D2D1::ColorF(0.78f, 0.80f, 0.84f, 0.95f)
                                : D2D1::ColorF(0.28f, 0.30f, 0.35f, 0.95f);
        v.cornerRadiusDip = 27.0f;
    } else if (p == L"classic_win11") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = isLight ? D2D1::ColorF(0.975f, 0.975f, 0.980f, 0.99f)
                            : D2D1::ColorF(0.145f, 0.148f, 0.160f, 0.99f);
        v.hasCustomBorder = true;
        v.borderColor = isLight ? D2D1::ColorF(0.76f, 0.77f, 0.80f, 1.00f)
                                : D2D1::ColorF(0.31f, 0.32f, 0.36f, 1.00f);
        v.cornerRadiusDip = 10.0f;
    } else if (p == L"midnight_oled") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = D2D1::ColorF(0.043f, 0.043f, 0.058f, 0.99f);
        v.hasCustomBorder = true;
        v.borderColor = D2D1::ColorF(0.20f, 0.22f, 0.28f, 0.95f);
        v.cornerRadiusDip = 12.0f;
    } else if (p == L"nord_frost") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = D2D1::ColorF(0.180f, 0.204f, 0.251f, 0.98f);
        v.hasCustomBorder = true;
        v.borderColor = D2D1::ColorF(0.533f, 0.753f, 0.816f, 0.88f);
        v.cornerRadiusDip = 14.0f;
    } else if (p == L"catppuccin_mocha") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = D2D1::ColorF(0.118f, 0.118f, 0.180f, 0.98f);
        v.hasCustomBorder = true;
        v.borderColor = D2D1::ColorF(0.706f, 0.745f, 0.996f, 0.82f);
        v.cornerRadiusDip = 14.0f;
    } else if (p == L"dracula_velvet") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = D2D1::ColorF(0.157f, 0.165f, 0.212f, 0.98f);
        v.hasCustomBorder = true;
        v.borderColor = D2D1::ColorF(0.741f, 0.576f, 0.976f, 0.85f);
        v.cornerRadiusDip = 14.0f;
    } else if (p == L"rose_pine") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = D2D1::ColorF(0.098f, 0.090f, 0.141f, 0.98f);
        v.hasCustomBorder = true;
        v.borderColor = D2D1::ColorF(0.922f, 0.737f, 0.729f, 0.82f);
        v.cornerRadiusDip = 14.0f;
    } else if (p == L"emerald_slate") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = D2D1::ColorF(0.067f, 0.118f, 0.110f, 0.98f);
        v.hasCustomBorder = true;
        v.borderColor = D2D1::ColorF(0.204f, 0.827f, 0.600f, 0.82f);
        v.cornerRadiusDip = 14.0f;
    } else if (p == L"warm_paper") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = D2D1::ColorF(0.980f, 0.969f, 0.949f, 0.99f);
        v.hasCustomBorder = true;
        v.borderColor = D2D1::ColorF(0.839f, 0.812f, 0.769f, 1.00f);
        v.cornerRadiusDip = 12.0f;
    } else if (p == L"custom") {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = isLight ? D2D1::ColorF(0.96f, 0.96f, 0.98f, 0.98f)
                            : D2D1::ColorF(0.12f, 0.12f, 0.15f, 0.98f);
        v.hasCustomBorder = true;
        v.borderColor = isLight ? D2D1::ColorF(0.76f, 0.78f, 0.82f, 0.95f)
                                : D2D1::ColorF(0.32f, 0.34f, 0.40f, 0.95f);
        v.cornerRadiusDip = 14.0f;
    } else {
        // Default: windows_blur
        v.surface = SurfaceTheme::Blur;
        v.hasCustomBorder = true;
        v.borderColor = isLight ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.12f)
                                : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.15f);
        v.cornerRadiusDip = 14.0f;
    }

    COLORREF rgb = 0;
    if (!s.customBgColor.empty() && TryParseHexColor(s.customBgColor, &rgb)) {
        v.surface = SurfaceTheme::Solid;
        v.hasCustomBg = true;
        v.bgColor = D2D1::ColorF(GetRValue(rgb) / 255.0f, GetGValue(rgb) / 255.0f, GetBValue(rgb) / 255.0f, 0.98f);
    }
    if (!s.customBorderColor.empty() && TryParseHexColor(s.customBorderColor, &rgb)) {
        v.hasCustomBorder = true;
        v.borderColor = D2D1::ColorF(GetRValue(rgb) / 255.0f, GetGValue(rgb) / 255.0f, GetBValue(rgb) / 255.0f, 1.0f);
    }
    if (s.customCornerRadius >= 0) {
        v.cornerRadiusDip = static_cast<float>(std::clamp(s.customCornerRadius, 0, 28));
    }
    return v;
}

struct UiStrings {
    const wchar_t* title;
    const wchar_t* recordBtn;
    const wchar_t* regionBtn;
    const wchar_t* pausedTag;
    const wchar_t* selectHint;
    const wchar_t* drawEscHint;

    const wchar_t* mDrawMode;
    const wchar_t* mSysAudio;
    const wchar_t* mMicAudio;
    const wchar_t* mCursor;
    const wchar_t* mOpenOnFinish;
    const wchar_t* mResolution;
    const wchar_t* mResNativePrefix;
    const wchar_t* mFps;
    const wchar_t* mFpsCurrentFmt;
    const wchar_t* mBitrate;
    const wchar_t* mMonitor;
    const wchar_t* mMonCursor;
    const wchar_t* mMonPrimary;
    const wchar_t* mMonItemFmt;
    const wchar_t* mMonAll;
    const wchar_t* mMonCustom;
    const wchar_t* mMonAllTag;
    const wchar_t* mMonCustomTag;
    const wchar_t* mMonShortFmt;
    const wchar_t* layoutTitle;
    const wchar_t* layoutHint;
    const wchar_t* layoutSideBySide;
    const wchar_t* layoutStacked;
    const wchar_t* layoutPip;
    const wchar_t* layoutReset;
    const wchar_t* layoutApply;
    const wchar_t* layoutCancel;
    const wchar_t* mSurface;
    const wchar_t* mSurfGlass;
    const wchar_t* mSurfBlur;
    const wchar_t* mSurfSolid;
    const wchar_t* mGlassMax;
    const wchar_t* mGlassClear;
    const wchar_t* mGlassBalanced;
    const wchar_t* mGlassTinted;
    const wchar_t* mColor;
    const wchar_t* mColorAuto;
    const wchar_t* mColorDark;
    const wchar_t* mColorLight;
    const wchar_t* mLanguage;
    const wchar_t* mLangAuto;
    const wchar_t* mOpenFolder;
    const wchar_t* mCloseBar;
};

static std::wstring ResolveEffectiveLanguageCode(const std::wstring& lang) {
    if (!lang.empty() && lang != L"auto") return lang;
    LANGID lid = GetUserDefaultUILanguage();
    WORD primary = PRIMARYLANGID(lid);
    WORD sub = SUBLANGID(lid);
    switch (primary) {
        case LANG_VIETNAMESE: return L"vi";
        case LANG_THAI:       return L"th";
        case LANG_INDONESIAN: return L"id";
        case LANG_MALAY:      return L"ms";
        case LANG_JAPANESE:   return L"ja";
        case LANG_CHINESE:
            if (sub == SUBLANG_CHINESE_TRADITIONAL || sub == SUBLANG_CHINESE_HONGKONG || sub == SUBLANG_CHINESE_MACAU)
                return L"zh_tw";
            return L"zh_cn";
        case LANG_KOREAN:     return L"ko";
        case LANG_FRENCH:     return L"fr";
        case LANG_GERMAN:     return L"de";
        case LANG_SPANISH:    return L"es";
        case LANG_PORTUGUESE: return L"pt";
        case LANG_ITALIAN:    return L"it";
        case LANG_RUSSIAN:    return L"ru";
        case LANG_UKRAINIAN:  return L"uk";
        case LANG_POLISH:     return L"pl";
        case LANG_DUTCH:      return L"nl";
        case LANG_TURKISH:    return L"tr";
        case LANG_HINDI:      return L"hi";
        case LANG_ARABIC:     return L"ar";
        default:              return L"en";
    }
}

static UiStrings GetUiStrings(const std::wstring& lang) {
    std::wstring code = ResolveEffectiveLanguageCode(lang);

    if (code == L"vi") return {
        L"Quay màn hình", L"Quay", L"Vùng", L"TẠM DỪNG",
        L"Kéo chuột chọn vùng cần quay  |  Esc hoặc Chuột phải để hủy",
        L"Đang vẽ  \u2022  Ấn Esc để thoát",
        L"Vẽ lên màn hình (Annotation)", L"Âm thanh hệ thống", L"Thu âm Micro",
        L"Hiển thị con trỏ chuột", L"Mở thư mục khi xong", L"Độ phân giải quay",
        L"Gốc phần cứng", L"Tốc độ khung hình (FPS)", L"%d FPS (Hiện tại %d Hz)",
        L"Chất lượng video (Bitrate)", L"Màn hình quay", L"Màn hình chứa con trỏ",
        L"Màn hình chính", L"Màn hình %d", L"Tất cả màn hình cùng lúc", L"Tự bố trí màn hình...",
        L"Tất cả", L"Tự bố trí", L"Màn %d",
        L"Tự bố trí màn hình quay",
        L"Kéo thẻ màn hình vào vị trí/góc mong muốn  \u2022  Kéo góc dưới phải để đổi kích thước  \u2022  Nhấp mắt để bật/tắt",
        L"Xếp ngang", L"Xếp dọc", L"Góc PiP", L"Đặt lại", L"Áp dụng", L"Hủy",
        L"Hiệu ứng bề mặt", L"Liquid Glass", L"Làm mờ Windows", L"Nền đặc (Opaque)",
        L"Kính: Trong suốt tối đa (-100)", L"Kính: Trong suốt cao (-50)",
        L"Kính: Cân bằng (0)", L"Kính: Đậm vừa (45)",
        L"Chế độ Sáng / Tối", L"Tự động (Auto)", L"Giao diện Tối (Dark)", L"Giao diện Sáng (Light)",
        L"Ngôn ngữ hiển thị", L"Tự động theo Windows", L"Mở thư mục video đã quay", L"Ẩn thanh quay màn hình"
    };

    if (code == L"th") return {
        L"บันทึกหน้าจอ", L"บันทึก", L"พื้นที่", L"หยุดชั่วคราว",
        L"ลากเพื่อเลือกพื้นที่บันทึก  |  กด Esc หรือคลิกขวาเพื่อยกเลิก",
        L"กำลังวาด  \u2022  กด Esc เพื่อออก",
        L"วาดบนหน้าจอ (คำอธิบาย)", L"เสียงระบบ", L"ไมโครโฟน",
        L"แสดงเคอร์เซอร์เมาส์", L"เปิดโฟลเดอร์เมื่อเสร็จ", L"ความละเอียดการบันทึก",
        L"ความละเอียดดั้งเดิม", L"อัตราเฟรม (FPS)", L"%d FPS (ปัจจุบัน %d Hz)",
        L"คุณภาพวิดีโอ (Bitrate)", L"หน้าจอที่บันทึก", L"หน้าจอใต้เคอร์เซอร์",
        L"หน้าจอหลัก", L"หน้าจอ %d", L"รวมทุกหน้าจอ", L"จัดวางหน้าจอเอง...",
        L"ทั้งหมด", L"กำหนดเอง", L"จอ %d",
        L"จัดวางหน้าจอแบบกำหนดเอง",
        L"ลากการ์ดหน้าจอไปยังมุมที่ต้องการ  \u2022  ลากมุมขวาล่างเพื่อปรับขนาด  \u2022  คลิกไอคอนเพื่อเปิด/ปิด",
        L"เคียงข้าง", L"เรียงแนวตั้ง", L"มุม PiP", L"รีเซ็ต", L"นำไปใช้", L"ยกเลิก",
        L"ธีมพื้นผิว", L"Liquid Glass", L"เบลอ Windows", L"ทึบแสง (Opaque)",
        L"กระจก: ใสสูงสุด (-100)", L"กระจก: ใสมาก (-50)", L"กระจก: สมดุล (0)", L"กระจก: เข้มปานกลาง (45)",
        L"โหมดสี สว่าง / มืด", L"อัตโนมัติ", L"โหมดมืด", L"โหมดสว่าง",
        L"ภาษาที่แสดง", L"อัตโนมัติตาม Windows", L"เปิดโฟลเดอร์วิดีโอ", L"ซ่อนแถบบันทึก"
    };

    if (code == L"id") return {
        L"Perekam Layar", L"Rekam", L"Area", L"JEDA",
        L"Seret untuk memilih area rekam  |  Esc atau Klik Kanan untuk batal",
        L"Menggambar  \u2022  Tekan Esc untuk keluar",
        L"Gambar di Layar (Anotasi)", L"Audio Sistem", L"Mikrofon",
        L"Rekam Kursor Mouse", L"Buka Folder Saat Selesai", L"Resolusi Rekaman",
        L"Asli Perangkat Keras", L"Laju Bingkai (FPS)", L"%d FPS (Saat ini %d Hz)",
        L"Kualitas Video (Bitrate)", L"Layar Rekaman", L"Layar di Bawah Kursor",
        L"Layar Utama", L"Layar %d", L"Semua Layar Sekaligus", L"Tata Letak Layar Kustom...",
        L"Semua", L"Kustom", L"Layar %d",
        L"Tata Letak Layar Kustom",
        L"Seret kartu layar ke posisi/sudut  \u2022  Seret sudut kanan bawah untuk ubah ukuran  \u2022  Klik ikon untuk aktif/nonaktif",
        L"Berdampingan", L"Bertumpuk", L"Sudut PiP", L"Reset", L"Terapkan", L"Batal",
        L"Tema Permukaan", L"Liquid Glass", L"Windows Blur", L"Solid (Opaque)",
        L"Kaca: Transparan Maks (-100)", L"Kaca: Bening (-50)", L"Kaca: Seimbang (0)", L"Kaca: Pekat (45)",
        L"Mode Warna", L"Otomatis", L"Mode Gelap", L"Mode Terang",
        L"Bahasa Tampilan", L"Otomatis (Windows)", L"Buka Folder Rekaman", L"Sembunyikan Bilah"
    };

    if (code == L"ms") return {
        L"Perakam Skrin", L"Rakam", L"Kawasan", L"JEDA",
        L"Seret untuk pilih kawasan rakaman  |  Esc atau Klik Kanan untuk batal",
        L"Melukis  \u2022  Tekan Esc untuk keluar",
        L"Lukis pada Skrin (Anotasi)", L"Audio Sistem", L"Mikrofon",
        L"Rakam Kursor Tetikus", L"Buka Folder Selepas Selesai", L"Resolusi Rakaman",
        L"Asal Perkakasan", L"Kadar Bingkai (FPS)", L"%d FPS (Semasa %d Hz)",
        L"Kualiti Video (Bitrate)", L"Monitor Rakaman", L"Monitor di Bawah Kursor",
        L"Monitor Utama", L"Monitor %d", L"Semua Monitor Digabungkan", L"Susun Atur Monitor Tersuai...",
        L"Semua", L"Tersuai", L"Skrin %d",
        L"Susun Atur Monitor Tersuai",
        L"Seret kad monitor ke mana-mana sudut  \u2022  Seret sudut kanan bawah untuk ubah saiz  \u2022  Klik lencana untuk togol",
        L"Bersebelahan", L"Bertingkat", L"Sudut PiP", L"Tetap Semula", L"Guna", L"Batal",
        L"Tema Permukaan", L"Liquid Glass", L"Windows Blur", L"Legap (Opaque)",
        L"Kaca: Lutsinar Maks (-100)", L"Kaca: Jelas (-50)", L"Kaca: Seimbang (0)", L"Kaca: Berwarna (45)",
        L"Mod Warna", L"Auto", L"Mod Gelap", L"Mod Cerah",
        L"Bahasa Paparan", L"Auto – Ikut Windows", L"Buka Folder Rakaman", L"Sembunyikan Bar"
    };

    if (code == L"ja") return {
        L"画面録画", L"録画", L"範囲", L"一時停止",
        L"ドラッグして録画範囲を選択  |  Esc または右クリックでキャンセル",
        L"描画中  \u2022  Esc で終了",
        L"画面描画（注釈）", L"システム音声", L"マイク",
        L"マウスカーソルを録画", L"完了後にフォルダーを開く", L"録画解像度",
        L"ネイティブ解像度", L"フレームレート (FPS)", L"%d FPS (現在 %d Hz)",
        L"画質 (ビットレート)", L"録画モニター", L"カーソル位置のモニター",
        L"メインモニター", L"モニター %d", L"すべてのモニター", L"カスタムレイアウト...",
        L"すべて", L"カスタム", L"画面%d",
        L"カスタムモニターレイアウト",
        L"モニターカードをドラッグして配置  \u2022  右下角をドラッグしてリサイズ  \u2022  アイコンで表示切替",
        L"横並び", L"縦並び", L"PiPコーナー", L"リセット", L"適用", L"キャンセル",
        L"サーフェステーマ", L"Liquid Glass", L"Windows ブラー", L"不透明 (Opaque)",
        L"ガラス: 最大透明 (-100)", L"ガラス: クリア (-50)", L"ガラス: バランス (0)", L"ガラス: 濃いめ (45)",
        L"カラーモード", L"自動 (Auto)", L"ダークモード", L"ライトモード",
        L"表示言語", L"Windows に合わせる", L"録画フォルダーを開く", L"バーを非表示"
    };

    if (code == L"zh_cn") return {
        L"屏幕录制", L"录制", L"区域", L"已暂停",
        L"拖动以选择录制区域  |  按 Esc 或右键取消",
        L"正在标注  \u2022  按 Esc 退出",
        L"屏幕标注绘制", L"系统音频", L"麦克风",
        L"录制鼠标指针", L"完成后打开文件夹", L"录制分辨率",
        L"原生硬件分辨率", L"帧率 (FPS)", L"%d FPS (当前 %d Hz)",
        L"视频码率 (Bitrate)", L"录制显示器", L"光标所在显示器",
        L"主显示器", L"显示器 %d", L"合并所有显示器", L"自定义显示器布局...",
        L"全部", L"自定义", L"屏幕%d",
        L"自定义显示器布局",
        L"拖动显示器卡片到任意位置  \u2022  拖动右下角调整大小  \u2022  点击图标切换显示",
        L"左右并排", L"上下堆叠", L"画中画角落", L"重置", L"应用", L"取消",
        L"表面主题", L"Liquid Glass", L"Windows 模糊", L"不透明 (Opaque)",
        L"玻璃: 最高透明 (-100)", L"玻璃: 通透 (-50)", L"玻璃: 均衡 (0)", L"玻璃: 深色 (45)",
        L"颜色模式", L"自动 (Auto)", L"深色模式", L"浅色模式",
        L"显示语言", L"跟随 Windows", L"打开录像文件夹", L"隐藏录制栏"
    };

    if (code == L"zh_tw") return {
        L"螢幕錄影", L"錄影", L"區域", L"已暫停",
        L"拖曳以選取錄影區域  |  按 Esc 或右鍵取消",
        L"正在繪圖  \u2022  按 Esc 結束",
        L"螢幕繪圖標註", L"系統音訊", L"麥克風",
        L"錄製滑鼠游標", L"完成後開啟資料夾", L"錄影解析度",
        L"原生硬體解析度", L"幀率 (FPS)", L"%d FPS (目前 %d Hz)",
        L"視訊位元率 (Bitrate)", L"錄製顯示器", L"游標所在顯示器",
        L"主顯示器", L"顯示器 %d", L"合併所有顯示器", L"自訂顯示器配置...",
        L"全部", L"自訂", L"螢幕%d",
        L"自訂顯示器配置",
        L"拖曳顯示器卡片至任意位置  \u2022  拖曳右下角調整大小  \u2022  點擊圖示切換顯示",
        L"並排", L"堆疊", L"子母畫面", L"重設", L"套用", L"取消",
        L"表面主題", L"Liquid Glass", L"Windows 模糊", L"不透明 (Opaque)",
        L"玻璃: 最高透明 (-100)", L"玻璃: 清透 (-50)", L"玻璃: 均衡 (0)", L"玻璃: 深色 (45)",
        L"色彩模式", L"自動 (Auto)", L"深色模式", L"淺色模式",
        L"顯示語言", L"自動偵測 Windows", L"開啟錄影資料夾", L"隱藏錄影列"
    };

    if (code == L"ko") return {
        L"화면 녹화", L"녹화", L"영역", L"일시정지",
        L"드래그하여 녹화 영역 선택  |  Esc 또는 우클릭으로 취소",
        L"그리기 모드  \u2022  Esc를 눌러 종료",
        L"화면 그리기 (주석)", L"시스템 오디오", L"마이크",
        L"마우스 커서 녹화", L"완료 시 폴더 열기", L"녹화 해상도",
        L"하드웨어 원본", L"프레임 속도 (FPS)", L"%d FPS (현재 %d Hz)",
        L"비디오 화질 (Bitrate)", L"녹화 모니터", L"커서 위치 모니터",
        L"주 모니터", L"모니터 %d", L"모든 모니터 합치기", L"사용자 지정 레이아웃...",
        L"전체", L"사용자", L"모니터%d",
        L"사용자 지정 모니터 레이아웃",
        L"모니터 카드를 원하는 위치로 드래그  \u2022  오른쪽 아래 모서리로 크기 조절  \u2022  아이콘 클릭으로 켜기/끄기",
        L"가로 배치", L"세로 배치", L"PiP 코너", L"초기화", L"적용", L"취소",
        L"표면 테마", L"Liquid Glass", L"Windows 블러", L"불투명 (Opaque)",
        L"글래스: 최대 투명 (-100)", L"글래스: 투명 (-50)", L"글래스: 균형 (0)", L"글래스: 진하게 (45)",
        L"색상 모드", L"자동 (Auto)", L"다크 모드", L"라이트 모드",
        L"표시 언어", L"Windows 언어 자동", L"녹화 폴더 열기", L"녹화 바 숨기기"
    };

    if (code == L"fr") return {
        L"Enregistreur d’écran", L"Enreg.", L"Zone", L"PAUSE",
        L"Glissez pour sélectionner une zone  |  Échap ou clic droit pour annuler",
        L"Dessin actif  \u2022  Appuyez sur Échap pour quitter",
        L"Dessin à l’écran (Annoter)", L"Audio système", L"Microphone",
        L"Capturer le curseur", L"Ouvrir le dossier à la fin", L"Résolution d’enregistrement",
        L"Native (Matériel)", L"Fréquence d’images (FPS)", L"%d FPS (Actuel %d Hz)",
        L"Qualité vidéo (Débit)", L"Écran à capturer", L"Écran sous le curseur",
        L"Écran principal", L"Écran %d", L"Tous les écrans combinés", L"Disposition personnalisée...",
        L"Tous", L"Perso", L"Écran %d",
        L"Disposition personnalisée des écrans",
        L"Glissez les écrans où vous voulez  \u2022  Tirez le coin inférieur droit pour redimensionner  \u2022  Cliquez pour activer",
        L"Côte à côte", L"Empilé", L"Coin PiP", L"Réinitialiser", L"Appliquer", L"Annuler",
        L"Thème de surface", L"Liquid Glass", L"Flou Windows", L"Opaque",
        L"Verre : Transparence max (-100)", L"Verre : Clair (-50)", L"Verre : Équilibré (0)", L"Verre : Teinté (45)",
        L"Mode de couleur", L"Auto", L"Mode sombre", L"Mode clair",
        L"Langue d’affichage", L"Auto – Windows", L"Ouvrir le dossier vidéo", L"Masquer la barre"
    };

    if (code == L"de") return {
        L"Bildschirmrekorder", L"Aufn.", L"Bereich", L"PAUSIERT",
        L"Ziehen für Aufnahmebereich  |  Esc oder Rechtsklick zum Abbrechen",
        L"Zeichnen  \u2022  Esc zum Beenden drücken",
        L"Auf Bildschirm zeichnen", L"Systemaudio", L"Mikrofon",
        L"Mauszeiger aufnehmen", L"Ordner nach Ende öffnen", L"Aufnahmeauflösung",
        L"Native Hardware", L"Bildrate (FPS)", L"%d FPS (Aktuell %d Hz)",
        L"Videoqualität (Bitrate)", L"Aufnahmebildschirm", L"Bildschirm unter Cursor",
        L"Hauptbildschirm", L"Bildschirm %d", L"Alle Bildschirme kombiniert", L"Eigenes Monitor-Layout...",
        L"Alle", L"Eigenes", L"Mon %d",
        L"Benutzerdefiniertes Monitor-Layout",
        L"Karten an beliebige Position ziehen  \u2022  Unten rechts skalieren  \u2022  Symbol zum Umschalten klicken",
        L"Nebeneinander", L"Gestapelt", L"PiP-Ecke", L"Zurücksetzen", L"Übernehmen", L"Abbrechen",
        L"Oberflächenthema", L"Liquid Glass", L"Windows-Unschärfe", L"Deckend (Opaque)",
        L"Glas: Max. Transparenz (-100)", L"Glas: Klar (-50)", L"Glas: Ausgewogen (0)", L"Glas: Getönt (45)",
        L"Farbmodus", L"Automatisch", L"Dunkelmodus", L"Hellmodus",
        L"Anzeigesprache", L"Auto – Windows", L"Aufnahmeordner öffnen", L"Leiste ausblenden"
    };

    if (code == L"es") return {
        L"Grabador de pantalla", L"Grabar", L"Región", L"PAUSA",
        L"Arrastra para elegir el área  |  Esc o clic derecho para cancelar",
        L"Dibujando  \u2022  Pulsa Esc para salir",
        L"Dibujar en pantalla", L"Audio del sistema", L"Micrófono",
        L"Capturar cursor", L"Abrir carpeta al terminar", L"Resolución de grabación",
        L"Nativa del hardware", L"Fotogramas (FPS)", L"%d FPS (Actual %d Hz)",
        L"Calidad de vídeo (Bitrate)", L"Monitor de captura", L"Monitor bajo el cursor",
        L"Monitor principal", L"Monitor %d", L"Todos los monitores", L"Diseño personalizado...",
        L"Todos", L"Pers.", L"Mon %d",
        L"Diseño personalizado de monitores",
        L"Arrastra las tarjetas a cualquier esquina  \u2022  Esquina inferior derecha para escalar  \u2022  Clic para alternar",
        L"Lado a lado", L"Apilado", L"Esquina PiP", L"Restablecer", L"Aplicar", L"Cancelar",
        L"Tema de superficie", L"Liquid Glass", L"Desenfoque Windows", L"Opaco",
        L"Cristal: Máx. transp. (-100)", L"Cristal: Claro (-50)", L"Cristal: Equilibrado (0)", L"Cristal: Tintado (45)",
        L"Modo de color", L"Automático", L"Modo oscuro", L"Modo claro",
        L"Idioma de pantalla", L"Auto – Windows", L"Abrir carpeta de vídeos", L"Ocultar barra"
    };

    if (code == L"pt") return {
        L"Gravador de Tela", L"Gravar", L"Região", L"PAUSADO",
        L"Arraste para selecionar a área  |  Esc ou botão direito para cancelar",
        L"Desenhando  \u2022  Pressione Esc para sair",
        L"Desenhar na tela (Anotar)", L"Áudio do sistema", L"Microfone",
        L"Capturar cursor", L"Abrir pasta ao concluir", L"Resolução de gravação",
        L"Nativa do hardware", L"Taxa de quadros (FPS)", L"%d FPS (Atual %d Hz)",
        L"Qualidade de vídeo (Bitrate)", L"Monitor de captura", L"Monitor sob o cursor",
        L"Monitor principal", L"Monitor %d", L"Todos os monitores", L"Layout personalizado...",
        L"Todos", L"Pers.", L"Mon %d",
        L"Layout personalizado de monitores",
        L"Arraste os cartões para qualquer canto  \u2022  Canto inferior direito para redimensionar  \u2022  Clique para alternar",
        L"Lado a lado", L"Empilhado", L"Canto PiP", L"Redefinir", L"Aplicar", L"Cancelar",
        L"Tema de superfície", L"Liquid Glass", L"Desfoque Windows", L"Opaco",
        L"Vidro: Transp. máx. (-100)", L"Vidro: Claro (-50)", L"Vidro: Equilibrado (0)", L"Vidro: Escurecido (45)",
        L"Modo de cor", L"Automático", L"Modo escuro", L"Modo claro",
        L"Idioma de exibição", L"Auto – Windows", L"Abrir pasta de gravações", L"Ocultar barra"
    };

    if (code == L"it") return {
        L"Registratore schermo", L"Registra", L"Area", L"PAUSA",
        L"Trascina per selezionare l’area  |  Esc o tasto destro per annullare",
        L"Disegno attivo  \u2022  Premi Esc per uscire",
        L"Disegna sullo schermo", L"Audio di sistema", L"Microfono",
        L"Cattura cursore", L"Apri cartella al termine", L"Risoluzione registrazione",
        L"Nativa hardware", L"Frequenza fotogrammi (FPS)", L"%d FPS (Attuale %d Hz)",
        L"Qualità video (Bitrate)", L"Monitor di cattura", L"Monitor sotto il cursore",
        L"Monitor principale", L"Monitor %d", L"Tutti i monitor", L"Layout personalizzato...",
        L"Tutti", L"Pers.", L"Mon %d",
        L"Layout personalizzato monitor",
        L"Trascina le schede in qualsiasi angolo  \u2022  Angolo in basso a destra per ridimensionare  \u2022  Clicca per attivare",
        L"Affiancati", L"Impilati", L"Angolo PiP", L"Ripristina", L"Applica", L"Annulla",
        L"Tema superficie", L"Liquid Glass", L"Sfocatura Windows", L"Opaco",
        L"Vetro: Trasp. max (-100)", L"Vetro: Chiaro (-50)", L"Vetro: Bilanciato (0)", L"Vetro: Tinto (45)",
        L"Modalità colore", L"Automatica", L"Modalità scura", L"Modalità chiara",
        L"Lingua visualizzazione", L"Auto – Windows", L"Apri cartella registrazioni", L"Nascondi barra"
    };

    if (code == L"ru") return {
        L"Запись экрана", L"Запись", L"Область", L"ПАУЗА",
        L"Выделите область мышью  |  Esc или ПКМ для отмены",
        L"Рисование  \u2022  Нажмите Esc для выхода",
        L"Рисование на экране", L"Системный звук", L"Микрофон",
        L"Запись курсора", L"Открыть папку по завершении", L"Разрешение записи",
        L"Родное разрешение", L"Частота кадров (FPS)", L"%d FPS (Текущая %d Гц)",
        L"Качество видео (Битрейт)", L"Монитор захвата", L"Монитор под курсором",
        L"Основной монитор", L"Монитор %d", L"Все мониторы вместе", L"Своя раскладка мониторов...",
        L"Все", L"Своя", L"Мон %d",
        L"Настройка раскладки мониторов",
        L"Перетащите карточки в нужный угол  \u2022  Тяните правый нижний угол для размера  \u2022  Клик — вкл/выкл",
        L"Рядом", L"Стопкой", L"Угол PiP", L"Сброс", L"Применить", L"Отмена",
        L"Тема поверхности", L"Liquid Glass", L"Размытие Windows", L"Непрозрачный",
        L"Стекло: Макс. прозр. (-100)", L"Стекло: Прозрачное (-50)", L"Стекло: Баланс (0)", L"Стекло: Плотное (45)",
        L"Цветовой режим", L"Авто", L"Тёмная тема", L"Светлая тема",
        L"Язык интерфейса", L"Авто – Windows", L"Открыть папку записей", L"Скрыть панель"
    };

    if (code == L"uk") return {
        L"Запис екрана", L"Запис", L"Область", L"ПАУЗА",
        L"Виділіть область мишею  |  Esc або ПКМ для скасування",
        L"Малювання  \u2022  Натисніть Esc для виходу",
        L"Малювання на екрані", L"Системний звук", L"Мікрофон",
        L"Запис курсора", L"Відкрити папку після завершення", L"Роздільна здатність",
        L"Рідна апаратна", L"Частота кадрів (FPS)", L"%d FPS (Поточна %d Гц)",
        L"Якість відео (Бітрейт)", L"Монітор захоплення", L"Монітор під курсором",
        L"Основний монітор", L"Монітор %d", L"Усі монітори разом", L"Власне розташування...",
        L"Усі", L"Власне", L"Мон %d",
        L"Власне розташування моніторів",
        L"Перетягніть картки у потрібний кут  \u2022  Тягніть правий нижній кут для розміру  \u2022  Клік — увімк/вимк",
        L"Поруч", L"Стовпцем", L"Кут PiP", L"Скинути", L"Застосувати", L"Скасувати",
        L"Тема поверхні", L"Liquid Glass", L"Розмиття Windows", L"Непрозорий",
        L"Скло: Макс. прозор. (-100)", L"Скло: Прозоре (-50)", L"Скло: Баланс (0)", L"Скло: Щільне (45)",
        L"Режим кольору", L"Авто", L"Темний режим", L"Світлий режим",
        L"Мова інтерфейсу", L"Авто – Windows", L"Відкрити папку записів", L"Сховати панель"
    };

    if (code == L"pl") return {
        L"Nagrywanie ekranu", L"Nagraj", L"Obszar", L"PAUZA",
        L"Przeciągnij, aby wybrać obszar  |  Esc lub PPM, aby anulować",
        L"Rysowanie  \u2022  Naciśnij Esc, aby wyjść",
        L"Rysowanie na ekranie", L"Dźwięk systemowy", L"Mikrofon",
        L"Nagrywaj kursor", L"Otwórz folder po zakończeniu", L"Rozdzielczość nagrywania",
        L"Natywna sprzętowa", L"Klatki na sekundę (FPS)", L"%d FPS (Obecnie %d Hz)",
        L"Jakość wideo (Bitrate)", L"Monitor nagrywania", L"Monitor pod kursorem",
        L"Główny monitor", L"Monitor %d", L"Wszystkie monitory", L"Własny układ monitorów...",
        L"Wszystkie", L"Własny", L"Mon %d",
        L"Własny układ monitorów",
        L"Przeciągnij karty w dowolne miejsce  \u2022  Prawy dolny róg zmienia rozmiar  \u2022  Kliknij ikonę, aby przełączyć",
        L"Obok siebie", L"Pionowo", L"Róg PiP", L"Resetuj", L"Zastosuj", L"Anuluj",
        L"Styl powierzchni", L"Liquid Glass", L"Rozmycie Windows", L"Nieprzezroczysty",
        L"Szkło: Maks. przezr. (-100)", L"Szkło: Czyste (-50)", L"Szkło: Zrównoważone (0)", L"Szkło: Przyciemn. (45)",
        L"Tryb kolorów", L"Auto", L"Tryb ciemny", L"Tryb jasny",
        L"Język wyświetlania", L"Auto – Windows", L"Otwórz folder nagrań", L"Ukryj pasek"
    };

    if (code == L"nl") return {
        L"Schermrecorder", L"Opnemen", L"Gebied", L"GEPAUZEERD",
        L"Sleep om gebied te selecteren  |  Esc of rechtermuisknop om te annuleren",
        L"Tekenen  \u2022  Druk op Esc om te sluiten",
        L"Op scherm tekenen", L"Systeemaudio", L"Microfoon",
        L"Muiscursor opnemen", L"Map openen na afloop", L"Opnameresolutie",
        L"Native hardware", L"Beeldsnelheid (FPS)", L"%d FPS (Huidig %d Hz)",
        L"Videokwaliteit (Bitrate)", L"Opnamescherm", L"Scherm onder cursor",
        L"Hoofdscherm", L"Scherm %d", L"Alle schermen samen", L"Aangepaste schermindeling...",
        L"Alle", L"Aangep.", L"Mon %d",
        L"Aangepaste schermindeling",
        L"Sleep schermkaarten naar elke hoek  \u2022  Rechtsonder schalen  \u2022  Klik op badge om te schakelen",
        L"Naast elkaar", L"Gestapeld", L"PiP-hoek", L"Resetten", L"Toepassen", L"Annuleren",
        L"Oppervlaktethema", L"Liquid Glass", L"Windows Blur", L"Ondoorzichtig",
        L"Glas: Max. transparant (-100)", L"Glas: Helder (-50)", L"Glas: Gebalanceerd (0)", L"Glas: Getint (45)",
        L"Kleurmodus", L"Automatisch", L"Donkere modus", L"Lichte modus",
        L"Weergavetaal", L"Auto – Windows", L"Opnamemap openen", L"Balk verbergen"
    };

    if (code == L"tr") return {
        L"Ekran Kaydedici", L"Kaydet", L"Alan", L"DURAKLATILDI",
        L"Kayıt alanı seçmek için sürükleyin  |  İptal için Esc veya Sağ Tık",
        L"Çizim modu  \u2022  Çıkmak için Esc’ye basın",
        L"Ekrana Çizim (Not Ekle)", L"Sistem Sesi", L"Mikrofon",
        L"Fare İmleçini Kaydet", L"Bitince Klasörü Aç", L"Kayıt Çözünürlüğü",
        L"Yerel Donanım", L"Kare Hızı (FPS)", L"%d FPS (Geçerli %d Hz)",
        L"Video Kalitesi (Bitrate)", L"Kayıt Monitörü", L"İmleç Altındaki Monitör",
        L"Birincil Monitör", L"Monitör %d", L"Tüm Monitörler Birlikte", L"Özel Monitör Düzeni...",
        L"Tümü", L"Özel", L"Mon %d",
        L"Özel Monitör Düzeni",
        L"Kartları istediğiniz köşeye sürükleyin  \u2022  Boyut için sağ alt köşeyi çekin  \u2022  Aç/kapa için simgeye tıklayın",
        L"Yan Yana", L"Üst Üste", L"PiP Köşesi", L"Sıfırla", L"Uygula", L"İptal",
        L"Yüzey Teması", L"Liquid Glass", L"Windows Bulanıklığı", L"Opak",
        L"Cam: Maks. Şeffaf (-100)", L"Cam: Berrak (-50)", L"Cam: Dengeli (0)", L"Cam: Tonlu (45)",
        L"Renk Modu", L"Otomatik", L"Koyu Mod", L"Açık Mod",
        L"Görüntüleme Dili", L"Otomatik – Windows", L"Kayıt Klasörünü Aç", L"Çubuğu Gizle"
    };

    if (code == L"hi") return {
        L"स्क्रीन रिकॉर्डर", L"रिकॉर्ड", L"क्षेत्र", L"रुका हुआ",
        L"रिकॉर्डिंग क्षेत्र चुनने के लिए खींचें  |  रद्द करने के लिए Esc या राइट-क्लिक",
        L"ड्राइंग चालू  \u2022  बाहर निकलने के लिए Esc दबाएँ",
        L"स्क्रीन पर चित्र बनाएँ", L"सिस्टम ऑडियो", L"माइक्रोफ़ोन",
        L"माउस कर्सर कैप्चर करें", L"समाप्त होने पर फ़ोल्डर खोलें", L"रिकॉर्डिंग रिज़ॉल्यूशन",
        L"मूल हार्डवेयर", L"फ़्रेम दर (FPS)", L"%d FPS (वर्तमान %d Hz)",
        L"वीडियो गुणवत्ता (बिटरेट)", L"कैप्चर मॉनिटर", L"कर्सर के नीचे मॉनिटर",
        L"प्राथमिक मॉनिटर", L"मॉनिटर %d", L"सभी मॉनिटर एक साथ", L"कस्टम मॉनिटर लेआउट...",
        L"सभी", L"कस्टम", L"स्क्रीन %d",
        L"कस्टम मॉनिटर लेआउट",
        L"मॉनिटर कार्ड को किसी भी कोने में खींचें  \u2022  आकार बदलने के लिए नीचे-दाएँ कोना खींचें  \u2022  टॉगल के लिए क्लिक करें",
        L"अगल-बगल", L"ऊपर-नीचे", L"PiP कोना", L"रीसेट", L"लागू करें", L"रद्द करें",
        L"सरफ़ेस थीम", L"Liquid Glass", L"Windows ब्लर", L"अपारदर्शी (Opaque)",
        L"ग्लास: अधिकतम पारदर्शी (-100)", L"ग्लास: स्पष्ट (-50)", L"ग्लास: संतुलित (0)", L"ग्लास: गहरा (45)",
        L"रंग मोड", L"स्वतः (Auto)", L"डार्क मोड", L"लाइट मोड",
        L"प्रदर्शन भाषा", L"स्वतः – Windows", L"रिकॉर्डिंग फ़ोल्डर खोलें", L"बार छुपाएँ"
    };

    if (code == L"ar") return {
        L"مسجل الشاشة", L"تسجيل", L"منطقة", L"متوقف مؤقتاً",
        L"اسحب لتحديد منطقة التسجيل  |  Esc أو زر الفأرة الأيمن للإلغاء",
        L"وضع الرسم  \u2022  اضغط Esc للخروج",
        L"الرسم على الشاشة", L"صوت النظام", L"الميكروفون",
        L"تصوير مؤشر الفأرة", L"فتح المجلد عند الانتهاء", L"دقة التسجيل",
        L"الدقة الأصلية للشاشة", L"معدل الإطارات (FPS)", L"%d FPS (الحالي %d Hz)",
        L"جودة الفيديو (Bitrate)", L"شاشة التسجيل", L"الشاشة أسفل المؤشر",
        L"الشاشة الرئيسية", L"شاشة %d", L"جميع الشاشات معاً", L"تخطيط الشاشات المخصص...",
        L"الكل", L"مخصص", L"شاشة %d",
        L"تخطيط الشاشات المخصص",
        L"اسحب بطاقات الشاشات لأي زاوية  \u2022  اسحب الزاوية السفلية اليمنى لتغيير الحجم  \u2022  انقر للتبديل",
        L"جنباً إلى جنب", L"عمودي", L"زاوية PiP", L"إعادة تعيين", L"تطبيق", L"إلغاء",
        L"سمة السطح", L"Liquid Glass", L"تمويه Windows", L"معتم (Opaque)",
        L"زجاج: شفافية قصوى (-100)", L"زجاج: صافي (-50)", L"زجاج: متوازن (0)", L"زجاج: ملون (45)",
        L"وضع الألوان", L"تلقائي", L"الوضع الداكن", L"الوضع الفاتح",
        L"لغة العرض", L"تلقائي – Windows", L"فتح مجلد التسجيلات", L"إخفاء الشريط"
    };

    return {
        L"Screen Recorder", L"Record", L"Region", L"PAUSED",
        L"Drag to select recording area  |  Esc or Right-click to cancel",
        L"Drawing  \u2022  Press Esc to exit",
        L"Screen Drawing (Annotate)", L"System Audio", L"Microphone",
        L"Capture Mouse Cursor", L"Reveal Video on Finish", L"Recording Resolution",
        L"Native Hardware", L"Frame Rate (FPS)", L"%d FPS (Current %d Hz)",
        L"Video Quality (Bitrate)", L"Capture Monitor", L"Monitor Under Cursor",
        L"Primary Monitor", L"Monitor %d", L"All Monitors Combined", L"Custom Monitor Layout...",
        L"All", L"Custom", L"Mon %d",
        L"Custom Monitor Layout",
        L"Drag monitor cards into any corner or position  \u2022  Drag bottom-right corner to resize  \u2022  Click badge to toggle",
        L"Side-by-Side", L"Stacked", L"PiP Corner", L"Reset", L"Apply", L"Cancel",
        L"Surface Theme", L"Liquid Glass", L"Windows Blur", L"Opaque",
        L"Glass: Max Transparent (-100)", L"Glass: Clear (-50)", L"Glass: Balanced (0)", L"Glass: Tinted (45)",
        L"Color Mode", L"Auto", L"Dark Mode", L"Light Mode",
        L"Display Language", L"Auto – Follow Windows", L"Open Recordings Folder", L"Hide Recorder Bar"
    };
}

static std::mutex g_settingsMutex;
static ModSettings g_settings;

enum HotkeyId : int {
    HOTKEY_TOGGLE_BAR = 1,
    HOTKEY_RECORD_REGION = 2,
    HOTKEY_PAUSE_RESUME = 3,
    HOTKEY_DRAW_EXIT_ESC = 4,
};

enum CustomMsg : UINT {
    WM_APP_RELOAD_SETTINGS = WM_APP + 10,
    WM_APP_RECORDING_STARTED = WM_APP + 11,
    WM_APP_RECORDING_STOPPED = WM_APP + 12,
    WM_APP_RECORDING_ERROR = WM_APP + 13,
    WM_APP_START_REGION_RECORD = WM_APP + 14,
    WM_APP_SHUTDOWN = WM_APP + 15,
};

static std::atomic<bool> g_isRecording{false};
static std::atomic<bool> g_isPaused{false};
static std::atomic<bool> g_stopRequested{false};
static std::atomic<bool> g_isSelectingRegion{false};
static std::atomic<bool> g_drawingEnabled{false};
static std::atomic<bool> g_unloading{false};
static std::atomic<int64_t> g_elapsedMs{0};

static std::thread g_uiThread;
static std::thread g_recordThread;
static std::mutex g_recordLifecycleMutex;

static std::atomic<HWND> g_msgHwnd{nullptr};
static HWND g_pillHwnd = nullptr;
static HWND g_regionBorderHwnd = nullptr;
static HWND g_drawCanvasHwnd = nullptr;
static HWND g_drawBarHwnd = nullptr;
static HWND g_drawHintHwnd = nullptr;
static HWND g_layoutHwnd = nullptr;
static std::atomic<HWND> g_activeMenuHwnd{nullptr};

static ID2D1Factory* g_pD2DFactory = nullptr;
static IDWriteFactory* g_pDWriteFactory = nullptr;
static IDWriteRenderingParams* g_pNaturalRenderParams = nullptr;

static const wchar_t* kMsgWndClass = L"WH_ScreenRec_MsgWnd_v21";
static const wchar_t* kPillWndClass = L"WH_ScreenRec_PillWnd_v21";
static const wchar_t* kMenuWndClass = L"WH_ScreenRec_MenuWnd_v21";
static const wchar_t* kRegionSelWndClass = L"WH_ScreenRec_RegionSelWnd_v21";
static const wchar_t* kRegionBorderWndClass = L"WH_ScreenRec_RegionBorderWnd_v21";
static const wchar_t* kDrawCanvasWndClass = L"WH_ScreenRec_DrawCanvasWnd_v21";
static const wchar_t* kDrawBarWndClass = L"WH_ScreenRec_DrawBarWnd_v21";
static const wchar_t* kDrawHintWndClass = L"WH_ScreenRec_DrawHintWnd_v22";
static const wchar_t* kLayoutWndClass = L"WH_ScreenRec_LayoutWnd_v23";

static inline int DpiScale(int val, UINT dpi) {
    return MulDiv(val, static_cast<int>(dpi), 96);
}

static inline float DpiScaleF(float val, UINT dpi) {
    return val * (static_cast<float>(dpi) / 96.0f);
}

static bool IsSystemAppLightTheme() {
    DWORD val = 0, cb = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &val, &cb) == ERROR_SUCCESS) {
        return val != 0;
    }
    return false;
}

static const wchar_t* GetPreferredFluentFontFamily() {
    static int s_hasVariableFont = -1;
    if (s_hasVariableFont == -1 && g_pDWriteFactory) {
        IDWriteFontCollection* pCol = nullptr;
        if (SUCCEEDED(g_pDWriteFactory->GetSystemFontCollection(&pCol, FALSE)) && pCol) {
            UINT32 idx = 0;
            BOOL exists = FALSE;
            s_hasVariableFont = (SUCCEEDED(pCol->FindFamilyName(L"Segoe UI Variable Text", &idx, &exists)) && exists) ? 1 : 0;
            pCol->Release();
        }
    }
    return (s_hasVariableFont == 1) ? L"Segoe UI Variable Text" : L"Segoe UI";
}

static void ApplyFluentTextRendering(ID2D1RenderTarget* pRT) {
    if (!pRT) return;
    pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    if (g_pNaturalRenderParams) pRT->SetTextRenderingParams(g_pNaturalRenderParams);
}

// ============================================================================
// Hardware Resolution Helper (Strictly Caps Output <= Hardware Resolution)
// ============================================================================

struct MonitorInfoItem {
    int displayNumber = 1; // 1-based
    HMONITOR hMon = nullptr;
    RECT rc{};
    int width = 0;
    int height = 0;
    int refreshHz = 60;
    bool isPrimary = false;
};

struct CustomMonitorSlot {
    int monitorIndex = 0; // 0-based index into EnumerateConnectedMonitors()
    bool enabled = true;
    float nx = 0.0f;      // Normalized [0..1] X in output canvas
    float ny = 0.0f;      // Normalized [0..1] Y in output canvas
    float nw = 1.0f;      // Normalized (0..1] Width in output canvas
    float nh = 1.0f;      // Normalized (0..1] Height in output canvas
};

static std::mutex g_customLayoutMutex;
static std::vector<CustomMonitorSlot> g_customLayout;

static BOOL CALLBACK EnumConnectedMonitorsProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* out = reinterpret_cast<std::vector<MonitorInfoItem>*>(lParam);
    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMon, &mi)) {
        MonitorInfoItem item;
        item.hMon = hMon;
        item.rc = mi.rcMonitor;
        item.width = static_cast<int>(mi.rcMonitor.right - mi.rcMonitor.left);
        item.height = static_cast<int>(mi.rcMonitor.bottom - mi.rcMonitor.top);
        item.isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        DEVMODEW dm{};
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) && dm.dmDisplayFrequency > 1) {
            int hz = static_cast<int>(dm.dmDisplayFrequency);
            // Normalize fractional Windows refresh rates (e.g. 59.94->60, 143.98->144, 239.96->240)
            if (hz == 59 || hz == 74 || hz == 89 || hz == 119 || hz == 143 || hz == 164 || hz == 179 || hz == 239 || hz == 359) {
                hz += 1;
            }
            item.refreshHz = hz;
        }
        out->push_back(item);
    }
    return TRUE;
}

static std::vector<MonitorInfoItem> EnumerateConnectedMonitors() {
    std::vector<MonitorInfoItem> list;
    EnumDisplayMonitors(nullptr, nullptr, EnumConnectedMonitorsProc, reinterpret_cast<LPARAM>(&list));

    std::sort(list.begin(), list.end(), [](const MonitorInfoItem& a, const MonitorInfoItem& b) {
        if (a.isPrimary != b.isPrimary) return a.isPrimary > b.isPrimary;
        if (a.rc.left != b.rc.left) return a.rc.left < b.rc.left;
        return a.rc.top < b.rc.top;
    });
    for (size_t i = 0; i < list.size(); ++i) {
        list[i].displayNumber = static_cast<int>(i) + 1;
    }
    return list;
}

static void EnsureDefaultCustomLayout(std::vector<CustomMonitorSlot>& slots, const std::vector<MonitorInfoItem>& mons) {
    if (mons.empty()) { slots.clear(); return; }
    bool valid = (slots.size() == mons.size());
    if (valid) {
        for (size_t i = 0; i < slots.size(); ++i) {
            if (slots[i].monitorIndex != static_cast<int>(i)) { valid = false; break; }
        }
    }
    if (valid) return;

    slots.clear();
    int n = static_cast<int>(mons.size());
    float cellW = 1.0f / static_cast<float>(n);
    for (int i = 0; i < n; ++i) {
        CustomMonitorSlot s;
        s.monitorIndex = i;
        s.enabled = true;
        s.nx = i * cellW;
        s.ny = 0.0f;
        s.nw = cellW;
        s.nh = 1.0f;
        slots.push_back(s);
    }
}

static int ParseMonitorIndexMode(const std::wstring& mode) {
    if (mode.rfind(L"mon:", 0) == 0) {
        return _wtoi(mode.c_str() + 4);
    }
    return -1;
}

static RECT GetCaptureTargetRect(const std::wstring& mode) {
    if (mode == L"all") {
        int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
        int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        return RECT{ vx, vy, vx + vw, vy + vh };
    }
    auto mons = EnumerateConnectedMonitors();
    int idx = ParseMonitorIndexMode(mode);
    if (idx >= 0 && idx < static_cast<int>(mons.size())) {
        return mons[idx].rc;
    }
    if (mode == L"primary" || mode == L"custom") {
        if (!mons.empty()) return mons[0].rc;
    }
    // Default: monitor under mouse cursor
    POINT pt{};
    GetCursorPos(&pt);
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{sizeof(mi)};
    if (hMon && GetMonitorInfoW(hMon, &mi)) return mi.rcMonitor;
    return RECT{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
}

static int GetMonitorRefreshRateHz(const std::wstring& mode) {
    auto mons = EnumerateConnectedMonitors();
    if (mons.empty()) return 60;

    if (mode == L"all" || mode == L"custom") {
        int maxHz = 60;
        for (const auto& m : mons) maxHz = std::max(maxHz, m.refreshHz);
        return maxHz;
    }
    int idx = ParseMonitorIndexMode(mode);
    if (idx >= 0 && idx < static_cast<int>(mons.size())) {
        return mons[idx].refreshHz;
    }
    if (mode == L"primary") {
        return mons[0].refreshHz;
    }
    POINT pt{};
    GetCursorPos(&pt);
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    for (const auto& m : mons) {
        if (m.hMon == hMon) return m.refreshHz;
    }
    return mons[0].refreshHz;
}

static int ResolveEffectiveFps(int requestedFps, const std::wstring& mode) {
    int hwHz = std::max(30, GetMonitorRefreshRateHz(mode));
    return std::clamp(requestedFps, 30, hwHz);
}

// Computes output (outW, outH) from source (srcW, srcH) and targetHeight (0 = native).
// Strictly enforces outW <= srcW and outH <= srcH (never upscales beyond hardware!) and even dimensions.
static void ComputeCappedOutputSize(int srcW, int srcH, int targetHeight, int& outW, int& outH) {
    srcW = srcW & ~1;
    srcH = srcH & ~1;
    if (srcW < 64) srcW = 64;
    if (srcH < 64) srcH = 64;

    if (targetHeight <= 0 || targetHeight >= srcH) {
        outW = srcW;
        outH = srcH;
        return;
    }

    int h = std::max(64, targetHeight & ~1);
    int w = static_cast<int>(std::llround(static_cast<double>(srcW) * h / static_cast<double>(srcH)));
    w = std::clamp(w & ~1, 64, srcW);
    outW = w;
    outH = std::min(h, srcH);
}

// ============================================================================
// Settings Persistence
// ============================================================================

static UINT ParseKeySetting(PCWSTR str, UINT fallbackVk) {
    if (!str || !str[0]) return fallbackVk;
    wchar_t c = str[0];
    if (c >= L'a' && c <= L'z') c = static_cast<wchar_t>(c - L'a' + L'A');
    if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) return static_cast<UINT>(c);
    return fallbackVk;
}

static UINT ParseModifiersSetting(PCWSTR str) {
    if (!str) return MOD_CONTROL | MOD_ALT;
    if (wcscmp(str, L"ctrl+shift") == 0) return MOD_CONTROL | MOD_SHIFT;
    if (wcscmp(str, L"alt+shift") == 0) return MOD_ALT | MOD_SHIFT;
    if (wcscmp(str, L"win+alt") == 0) return MOD_WIN | MOD_ALT;
    return MOD_CONTROL | MOD_ALT;
}

static std::wstring ReadStringSetting(const wchar_t* groupedKey, const wchar_t* flatKey, const wchar_t* defVal) {
    PCWSTR v = Wh_GetStringSetting(groupedKey);
    if (v) {
        std::wstring s(v);
        Wh_FreeStringSetting(v);
        if (!s.empty()) return s;
    }
    if (flatKey) {
        v = Wh_GetStringSetting(flatKey);
        if (v) {
            std::wstring s(v);
            Wh_FreeStringSetting(v);
            if (!s.empty()) return s;
        }
    }
    return defVal ? defVal : L"";
}

static void ClearInAppOverrides() {
    const wchar_t* keys[] = {
        L"Ov_themePreset", L"Ov_surfaceTheme", L"Ov_colorMode", L"Ov_customBgColor",
        L"Ov_customBorderColor", L"Ov_customCornerRadius", L"Ov_glassOpacity", L"Ov_language",
        L"Ov_targetResHeight", L"Ov_fps", L"Ov_videoBitrateMbps", L"Ov_captureMonitor",
        L"Ov_recordCursor", L"Ov_recordSystemAudio", L"Ov_recordMicrophone",
        L"Ov_showFloatingHud", L"Ov_openFolderOnFinish"
    };
    for (const wchar_t* k : keys) Wh_DeleteValue(k);
}

static void LoadSettings(bool fromSettingsChanged = false) {
    if (fromSettingsChanged) ClearInAppOverrides();

    ModSettings s;
    wchar_t ovBuf[128] = {};

    std::wstring preset = ReadStringSetting(L"appearanceGroup.themePreset", L"themePreset", L"windows_blur");
    if (Wh_GetStringValue(L"Ov_themePreset", ovBuf, ARRAYSIZE(ovBuf)) > 0 && ovBuf[0]) preset = ovBuf;
    s.themePreset = preset.empty() ? L"windows_blur" : preset;

    std::wstring bgHex = ReadStringSetting(L"appearanceGroup.customBgColor", L"customBgColor", L"");
    if (Wh_GetStringValue(L"Ov_customBgColor", ovBuf, ARRAYSIZE(ovBuf)) > 0) {
        bgHex = (wcscmp(ovBuf, L"__NONE__") == 0) ? L"" : ovBuf;
    }
    s.customBgColor = bgHex;

    std::wstring bdHex = ReadStringSetting(L"appearanceGroup.customBorderColor", L"customBorderColor", L"");
    if (Wh_GetStringValue(L"Ov_customBorderColor", ovBuf, ARRAYSIZE(ovBuf)) > 0) {
        bdHex = (wcscmp(ovBuf, L"__NONE__") == 0) ? L"" : ovBuf;
    }
    s.customBorderColor = bdHex;

    int crSet = Wh_GetIntSetting(L"appearanceGroup.customCornerRadius");
    int crOv  = Wh_GetIntValue(L"Ov_customCornerRadius", -99);
    s.customCornerRadius = (crOv >= -1) ? std::clamp(crOv, -1, 28) : std::clamp(crSet, -1, 28);

    ResolvedThemeVisuals rvPreview = ResolveEffectiveThemeVisuals(s, false);
    s.surfaceTheme = rvPreview.surface;
    s.glassOpacity = -100;
    s.blurOpacity  = 255;

    std::wstring col = ReadStringSetting(L"appearanceGroup.colorMode", L"colorMode", L"auto");
    if (Wh_GetStringValue(L"Ov_colorMode", ovBuf, ARRAYSIZE(ovBuf)) > 0 && ovBuf[0]) col = ovBuf;
    if (col == L"dark") s.colorMode = ColorMode::Dark;
    else if (col == L"light") s.colorMode = ColorMode::Light;
    else s.colorMode = ColorMode::Auto;

    s.language = ReadStringSetting(L"appearanceGroup.language", L"language", L"auto");
    if (Wh_GetStringValue(L"Ov_language", ovBuf, ARRAYSIZE(ovBuf)) > 0 && ovBuf[0]) s.language = ovBuf;

    s.outputFolder = ReadStringSetting(L"recordingGroup.outputFolder", L"outputFolder", L"");

    std::wstring resStr = ReadStringSetting(L"recordingGroup.videoResolution", L"videoResolution", L"0");
    int resSet = _wtoi(resStr.c_str());
    int resOv = Wh_GetIntValue(L"Ov_targetResHeight", -1);
    s.targetResHeight = (resOv >= 0) ? resOv : std::max(0, resSet);

    auto isValidFps = [](int f) {
        return f >= 30 && f <= 360;
    };
    int fpsSet = Wh_GetIntSetting(L"recordingGroup.fps");
    int fpsOv = Wh_GetIntValue(L"Ov_fps", 0);
    s.fps = isValidFps(fpsOv) ? fpsOv : (isValidFps(fpsSet) ? fpsSet : 60);

    int brSet = Wh_GetIntSetting(L"recordingGroup.videoBitrateMbps");
    if (brSet <= 0) brSet = 12;
    int brOv = Wh_GetIntValue(L"Ov_videoBitrateMbps", 0);
    s.videoBitrateMbps = std::clamp(brOv > 0 ? brOv : brSet, 2, 50);

    std::wstring mon = ReadStringSetting(L"recordingGroup.captureMonitor", L"captureMonitor", L"cursor");
    if (Wh_GetStringValue(L"Ov_captureMonitor", ovBuf, ARRAYSIZE(ovBuf)) > 0 && ovBuf[0]) mon = ovBuf;
    s.captureMonitorMode = mon.empty() ? L"cursor" : mon;

    int ovCur = Wh_GetIntValue(L"Ov_recordCursor", -1);
    s.recordCursor = (ovCur >= 0) ? (ovCur != 0) : (Wh_GetIntSetting(L"recordingGroup.recordCursor") != 0);

    int ovSys = Wh_GetIntValue(L"Ov_recordSystemAudio", -1);
    s.recordSystemAudio = (ovSys >= 0) ? (ovSys != 0) : (Wh_GetIntSetting(L"recordingGroup.recordSystemAudio") != 0);

    int ovMic = Wh_GetIntValue(L"Ov_recordMicrophone", -1);
    s.recordMicrophone = (ovMic >= 0) ? (ovMic != 0) : (Wh_GetIntSetting(L"recordingGroup.recordMicrophone") != 0);

    int ovHud = Wh_GetIntValue(L"Ov_showFloatingHud", -1);
    s.showFloatingHud = (ovHud >= 0) ? (ovHud != 0) : (Wh_GetIntSetting(L"recordingGroup.showFloatingHud") != 0);

    int ovOpen = Wh_GetIntValue(L"Ov_openFolderOnFinish", -1);
    s.openFolderOnFinish = (ovOpen >= 0) ? (ovOpen != 0) : (Wh_GetIntSetting(L"recordingGroup.openFolderOnFinish") != 0);

    std::wstring mods = ReadStringSetting(L"hotkeyGroup.hotkeyModifiers", L"hotkeyModifiers", L"ctrl+alt");
    s.hotkeyModifiers = ParseModifiersSetting(mods.c_str());

    std::wstring recKey = ReadStringSetting(L"hotkeyGroup.hotkeyRecordKey", L"hotkeyRecordKey", L"R");
    s.hotkeyRecordVk = ParseKeySetting(recKey.c_str(), 'R');

    std::wstring regKey = ReadStringSetting(L"hotkeyGroup.hotkeyRegionKey", L"hotkeyRegionKey", L"S");
    s.hotkeyRegionVk = ParseKeySetting(regKey.c_str(), 'S');

    std::wstring pauseKey = ReadStringSetting(L"hotkeyGroup.hotkeyPauseKey", L"hotkeyPauseKey", L"P");
    s.hotkeyPauseVk = ParseKeySetting(pauseKey.c_str(), 'P');

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
}

static ModSettings GetSettingsCopy() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

static std::wstring GetRecordingsFolder(const std::wstring& configuredFolder) {
    std::wstring dir = configuredFolder;
    if (dir.empty()) {
        PWSTR videosPath = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Videos, 0, nullptr, &videosPath)) && videosPath) {
            dir = videosPath;
            CoTaskMemFree(videosPath);
            dir += L"\\ScreenRecordings";
        } else {
            wchar_t tempPath[MAX_PATH] = {};
            GetTempPathW(MAX_PATH, tempPath);
            dir = tempPath;
        }
    }
    while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) dir.pop_back();
    SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
    return dir;
}

static std::wstring BuildOutputFilePath(const std::wstring& configuredFolder) {
    std::wstring dir = GetRecordingsFolder(configuredFolder);
    SYSTEMTIME st{};
    GetLocalTime(&st);
    wchar_t filename[128];
    swprintf_s(filename, L"\\Recording_%04d%02d%02d_%02d%02d%02d.mp4",
               st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    return dir + filename;
}

static void RevealFileInExplorer(const std::wstring& path) {
    if (path.empty()) return;
    std::wstring args = L"/select,\"" + path + L"\"";
    ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
}

static void OpenRecordingsFolderInExplorer() {
    ModSettings cfg = GetSettingsCopy();
    std::wstring dir = GetRecordingsFolder(cfg.outputFolder);
    ShellExecuteW(nullptr, L"open", dir.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

// ============================================================================
// Liquid Glass / Windows Blur Optical Backdrop Engine
// ============================================================================

struct DesktopBackdropSnap {
    std::vector<DWORD> pixels;
    int monX = 0, monY = 0, monW = 0, monH = 0;
    ULONGLONG tick = 0;
    bool valid = false;
};

static std::mutex g_snapMutex;
static DesktopBackdropSnap g_backdropSnap;

static void CaptureMonitorBackdrop(HWND hRefWnd) {
    HMONITOR hMon = (hRefWnd && IsWindow(hRefWnd))
        ? MonitorFromWindow(hRefWnd, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{sizeof(mi)};
    int monX = 0, monY = 0, monW = GetSystemMetrics(SM_CXSCREEN), monH = GetSystemMetrics(SM_CYSCREEN);
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        monX = static_cast<int>(mi.rcMonitor.left);
        monY = static_cast<int>(mi.rcMonitor.top);
        monW = static_cast<int>(mi.rcMonitor.right - mi.rcMonitor.left);
        monH = static_cast<int>(mi.rcMonitor.bottom - mi.rcMonitor.top);
    }
    if (monW <= 0 || monH <= 0 || monW > 7680 || monH > 4320) return;

    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) return;
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = monW;
    bmi.bmiHeader.biHeight = -monH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* dibPixels = nullptr;
    HBITMAP hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&dibPixels), nullptr, 0);
    if (hBmp && dibPixels) {
        HGDIOBJ oldBmp = SelectObject(hMemDC, hBmp);
        if (BitBlt(hMemDC, 0, 0, monW, monH, hScreenDC, monX, monY, SRCCOPY)) {
            GdiFlush();
            std::lock_guard<std::mutex> lock(g_snapMutex);
            g_backdropSnap.monX = monX;
            g_backdropSnap.monY = monY;
            g_backdropSnap.monW = monW;
            g_backdropSnap.monH = monH;
            g_backdropSnap.pixels.assign(dibPixels, dibPixels + static_cast<size_t>(monW) * monH);
            g_backdropSnap.tick = GetTickCount64();
            g_backdropSnap.valid = true;
        }
        SelectObject(hMemDC, oldBmp);
        DeleteObject(hBmp);
    }
    DeleteDC(hMemDC);
    ReleaseDC(nullptr, hScreenDC);
}

static void BlurBufferSeparableRGB(std::vector<DWORD>& buf, std::vector<DWORD>& tmp, int w, int h, int radius) {
    if (radius <= 0 || w <= 2 || h <= 2) return;
    radius = std::clamp(radius, 1, std::min(w - 1, h - 1) / 2);
    if (radius <= 0) return;
    const uint32_t diam = static_cast<uint32_t>(radius * 2 + 1);
    const uint32_t invDiam = (1u << 24) / diam + 1u;

    for (int pass = 0; pass < 3; ++pass) {
        for (int y = 0; y < h; ++y) {
            const DWORD* rowIn = &buf[static_cast<size_t>(y) * w];
            DWORD* rowOut = &tmp[static_cast<size_t>(y) * w];
            uint32_t sumR = 0, sumG = 0, sumB = 0;
            for (int k = -radius; k <= radius; ++k) {
                DWORD p = rowIn[std::clamp(k, 0, w - 1)];
                sumR += (p >> 16) & 0xFF; sumG += (p >> 8) & 0xFF; sumB += p & 0xFF;
            }
            for (int x = 0; x < w; ++x) {
                rowOut[x] = (((sumR * invDiam) >> 24) << 16) | (((sumG * invDiam) >> 24) << 8) | ((sumB * invDiam) >> 24);
                DWORD pOut = rowIn[std::clamp(x - radius, 0, w - 1)];
                DWORD pIn  = rowIn[std::clamp(x + radius + 1, 0, w - 1)];
                sumR += ((pIn >> 16) & 0xFF) - ((pOut >> 16) & 0xFF);
                sumG += ((pIn >> 8) & 0xFF)  - ((pOut >> 8) & 0xFF);
                sumB += (pIn & 0xFF)         - (pOut & 0xFF);
            }
        }
        for (int x = 0; x < w; ++x) {
            uint32_t sumR = 0, sumG = 0, sumB = 0;
            for (int k = -radius; k <= radius; ++k) {
                DWORD p = tmp[static_cast<size_t>(std::clamp(k, 0, h - 1)) * w + x];
                sumR += (p >> 16) & 0xFF; sumG += (p >> 8) & 0xFF; sumB += p & 0xFF;
            }
            for (int y = 0; y < h; ++y) {
                buf[static_cast<size_t>(y) * w + x] = (((sumR * invDiam) >> 24) << 16) | (((sumG * invDiam) >> 24) << 8) | ((sumB * invDiam) >> 24);
                DWORD pOut = tmp[static_cast<size_t>(std::clamp(y - radius, 0, h - 1)) * w + x];
                DWORD pIn  = tmp[static_cast<size_t>(std::clamp(y + radius + 1, 0, h - 1)) * w + x];
                sumR += ((pIn >> 16) & 0xFF) - ((pOut >> 16) & 0xFF);
                sumG += ((pIn >> 8) & 0xFF)  - ((pOut >> 8) & 0xFF);
                sumB += (pIn & 0xFF)         - (pOut & 0xFF);
            }
        }
    }
}

static bool SampleBackdropIsLight(int wndX, int wndY, int w, int h, bool prevLight) {
    std::lock_guard<std::mutex> lock(g_snapMutex);
    if (!g_backdropSnap.valid || g_backdropSnap.pixels.empty() || w <= 0 || h <= 0) return prevLight;
    int relX = wndX - g_backdropSnap.monX;
    int relY = wndY - g_backdropSnap.monY;
    int stepX = std::max(1, w / 12);
    int stepY = std::max(1, h / 12);
    double sumLum = 0.0;
    int count = 0;
    for (int y = relY + stepY / 2; y < relY + h && y < g_backdropSnap.monH; y += stepY) {
        if (y < 0) continue;
        const DWORD* row = &g_backdropSnap.pixels[static_cast<size_t>(y) * g_backdropSnap.monW];
        for (int x = relX + stepX / 2; x < relX + w && x < g_backdropSnap.monW; x += stepX) {
            if (x < 0) continue;
            DWORD px = row[x];
            sumLum += 0.2126 * (((px >> 16) & 0xFF) / 255.0)
                    + 0.7152 * (((px >> 8) & 0xFF) / 255.0)
                    + 0.0722 * ((px & 0xFF) / 255.0);
            ++count;
        }
    }
    if (count == 0) return prevLight;
    double avg = sumLum / count;
    return prevLight ? (avg > 0.56) : (avg > 0.64);
}

static bool ResolveIsLightMode(const ModSettings& cfg, int wndX, int wndY, int w, int h, bool prevLight) {
    if (cfg.colorMode == ColorMode::Light) return true;
    if (cfg.colorMode == ColorMode::Dark) return false;
    COLORREF customRgb = 0;
    if (!cfg.customBgColor.empty() && TryParseHexColor(cfg.customBgColor, &customRgb)) {
        double lum = (0.299 * GetRValue(customRgb) + 0.587 * GetGValue(customRgb) + 0.114 * GetBValue(customRgb)) / 255.0;
        return lum > 0.54;
    }
    if (cfg.themePreset == L"warm_paper") return true;
    if (cfg.themePreset == L"midnight_oled" || cfg.themePreset == L"nord_frost" ||
        cfg.themePreset == L"catppuccin_mocha" || cfg.themePreset == L"dracula_velvet" ||
        cfg.themePreset == L"rose_pine" || cfg.themePreset == L"emerald_slate") {
        return false;
    }
    if (cfg.surfaceTheme == SurfaceTheme::Glass) return SampleBackdropIsLight(wndX, wndY, w, h, prevLight);
    return IsSystemAppLightTheme();
}

static void DrawPillGlassBackdrop(ID2D1RenderTarget* pRT, HWND hWnd, int w, int h,
                                  float cornerR, UINT dpi, SurfaceTheme surface,
                                  int glassPct, int blurOp, bool isLight) {
    if (!pRT || w <= 0 || h <= 0) return;

    if (surface == SurfaceTheme::Solid) {
        ID2D1SolidColorBrush* pBr = nullptr;
        D2D1_COLOR_F col = isLight ? D2D1::ColorF(0.96f, 0.96f, 0.98f, 1.0f) : D2D1::ColorF(0.11f, 0.12f, 0.15f, 1.0f);
        if (SUCCEEDED(pRT->CreateSolidColorBrush(col, &pBr)) && pBr) {
            pRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0.5f, 0.5f, w - 0.5f, h - 0.5f), cornerR, cornerR), pBr);
            pBr->Release();
        }
        return;
    }

    RECT rcWnd{};
    GetWindowRect(hWnd, &rcWnd);

    std::lock_guard<std::mutex> lock(g_snapMutex);
    if (!g_backdropSnap.valid || g_backdropSnap.pixels.empty()) {
        ID2D1SolidColorBrush* pBr = nullptr;
        D2D1_COLOR_F col = isLight ? D2D1::ColorF(0.95f, 0.96f, 0.98f, 0.85f) : D2D1::ColorF(0.11f, 0.12f, 0.16f, 0.85f);
        if (SUCCEEDED(pRT->CreateSolidColorBrush(col, &pBr)) && pBr) {
            pRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0.5f, 0.5f, w - 0.5f, h - 0.5f), cornerR, cornerR), pBr);
            pBr->Release();
        }
        return;
    }

    int monX = g_backdropSnap.monX, monY = g_backdropSnap.monY;
    int monW = g_backdropSnap.monW, monH = g_backdropSnap.monH;
    int pad = 48;
    int x0 = std::clamp(static_cast<int>(rcWnd.left) - monX - pad, 0, monW - 1);
    int y0 = std::clamp(static_cast<int>(rcWnd.top) - monY - pad, 0, monH - 1);
    int x1 = std::clamp(static_cast<int>(rcWnd.left) - monX + w + pad, x0 + 4, monW);
    int y1 = std::clamp(static_cast<int>(rcWnd.top) - monY + h + pad, y0 + 4, monH);
    int cw = x1 - x0, ch = y1 - y0;
    int qw = std::max(4, cw / 4), qh = std::max(4, ch / 4);

    std::vector<DWORD> cropQuarter(static_cast<size_t>(qw) * qh);
    const DWORD* snap = g_backdropSnap.pixels.data();
    for (int qy = 0; qy < qh; ++qy) {
        int sy = std::clamp(y0 + qy * 4 + 1, 0, monH - 1);
        const DWORD* sRow = &snap[static_cast<size_t>(sy) * monW];
        DWORD* dRow = &cropQuarter[static_cast<size_t>(qy) * qw];
        for (int qx = 0; qx < qw; ++qx) {
            int sx = std::clamp(x0 + qx * 4 + 1, 0, monW - 1);
            dRow[qx] = sRow[sx] & 0x00FFFFFF;
        }
    }

    std::vector<DWORD> tmp(cropQuarter.size());
    BlurBufferSeparableRGB(cropQuarter, tmp, qw, qh, (surface == SurfaceTheme::Blur) ? 10 : 9);

    bool isBlur = (surface == SurfaceTheme::Blur);
    float negF = (!isBlur && glassPct < 0) ? (std::clamp(-glassPct, 0, 100) / 100.0f) : 0.0f;
    float posF = (!isBlur && glassPct > 0) ? (std::clamp(glassPct, 0, 100) / 100.0f) : 0.0f;
    float satBoost = isBlur ? 1.24f : (1.16f + 0.10f * negF - 0.06f * posF);
    float tintWeight = isBlur
        ? (0.10f + (std::clamp(blurOp, 0, 255) / 255.0f) * 0.24f)
        : ((glassPct < 0) ? (0.06f + 0.09f * (1.0f - negF)) : (0.15f + 0.42f * posF));

    int satScale = static_cast<int>(std::round(satBoost * 256.0f));
    int tintW = static_cast<int>(std::round(tintWeight * 256.0f));
    int invTintW = 256 - tintW;
    int tintR = (isLight ? 246 : 16) * tintW;
    int tintG = (isLight ? 248 : 18) * tintW;
    int tintB = (isLight ? 254 : 26) * tintW;

    for (DWORD& p : cropQuarter) {
        int r = (p >> 16) & 0xFF, g = (p >> 8) & 0xFF, b = p & 0xFF;
        int luma = (77 * r + 150 * g + 29 * b) >> 8;
        r = std::clamp(luma + (((r - luma) * satScale) >> 8), 0, 255);
        g = std::clamp(luma + (((g - luma) * satScale) >> 8), 0, 255);
        b = std::clamp(luma + (((b - luma) * satScale) >> 8), 0, 255);
        r = std::clamp((r * invTintW + tintR) >> 8, 0, 255);
        g = std::clamp((g * invTintW + tintG) >> 8, 0, 255);
        b = std::clamp((b * invTintW + tintB) >> 8, 0, 255);
        p = (static_cast<DWORD>(r) << 16) | (static_cast<DWORD>(g) << 8) | static_cast<DWORD>(b);
    }

    std::vector<DWORD> outPx(static_cast<size_t>(w) * h, 0);
    float cx = w * 0.5f, cy = h * 0.5f;
    float hxBox = std::max(0.0f, cx - cornerR);
    float hyBox = std::max(0.0f, cy - cornerR);
    float bevelW = isBlur ? 1.5f : DpiScaleF(13.0f + 4.0f * negF, dpi);
    float maxDisp = isBlur ? 0.0f : DpiScaleF(10.0f + 10.0f * negF, dpi);

    auto sampleQuarter = [&](float sxMon, float syMon) -> DWORD {
        int ix = std::clamp(static_cast<int>((sxMon - x0) * 0.25f), 0, qw - 1);
        int iy = std::clamp(static_cast<int>((syMon - y0) * 0.25f), 0, qh - 1);
        return cropQuarter[static_cast<size_t>(iy) * qw + ix];
    };

    for (int y = 0; y < h; ++y) {
        float dy = (y + 0.5f) - cy;
        float qy = std::fabs(dy) - hyBox;
        float oy = std::max(qy, 0.0f);
        DWORD* dstRow = &outPx[static_cast<size_t>(y) * w];

        for (int x = 0; x < w; ++x) {
            float dx = (x + 0.5f) - cx;
            float qx = std::fabs(dx) - hxBox;
            float ox = std::max(qx, 0.0f);
            float outDist = std::sqrt(ox * ox + oy * oy);
            float inDist = std::min(std::max(qx, qy), 0.0f);
            float sdf = outDist + inDist - cornerR;
            if (sdf >= 0.65f) continue;

            float edgeAA = std::clamp(0.65f - sdf, 0.0f, 1.0f);
            float distFromEdge = std::max(0.0f, -sdf);
            float tBevel = (distFromEdge < bevelW) ? (1.0f - distFromEdge / bevelW) : 0.0f;

            float baseX = static_cast<float>(static_cast<int>(rcWnd.left) - monX + x);
            float baseY = static_cast<float>(static_cast<int>(rcWnd.top) - monY + y);

            uint32_t r = 0, g = 0, b = 0;
            if (tBevel > 0.0f && !isBlur) {
                float nx = 0.0f, ny = 0.0f;
                if (outDist > 1e-4f) {
                    nx = (dx >= 0.0f ? 1.0f : -1.0f) * (ox / outDist);
                    ny = (dy >= 0.0f ? 1.0f : -1.0f) * (oy / outDist);
                } else if (qx > qy) {
                    nx = (dx >= 0.0f ? 1.0f : -1.0f);
                } else {
                    ny = (dy >= 0.0f ? 1.0f : -1.0f);
                }
                float lens = tBevel * tBevel * (0.4f + 0.6f * tBevel);
                float dispX = -nx * lens * maxDisp;
                float dispY = -ny * lens * maxDisp;

                r = (sampleQuarter(baseX + dispX * 0.86f, baseY + dispY * 0.86f) >> 16) & 0xFF;
                g = (sampleQuarter(baseX + dispX,         baseY + dispY)         >> 8)  & 0xFF;
                b =  sampleQuarter(baseX + dispX * 1.14f, baseY + dispY * 1.14f)        & 0xFF;

                float lightTL = std::max(0.0f, -0.68f * nx - 0.73f * ny);
                float lightBR = std::max(0.0f,  0.68f * nx + 0.73f * ny);
                float fresnel = std::clamp(std::pow(tBevel, 2.1f) * (0.16f + 0.36f * lightTL + 0.26f * lightBR), 0.0f, 0.72f);
                uint32_t fr = static_cast<uint32_t>(std::round(fresnel * 256.0f));
                uint32_t invFr = 256u - fr;
                r = (r * invFr + 245u * fr) >> 8;
                g = (g * invFr + 248u * fr) >> 8;
                b = (b * invFr + 255u * fr) >> 8;
            } else {
                DWORD p = sampleQuarter(baseX, baseY);
                r = (p >> 16) & 0xFF; g = (p >> 8) & 0xFF; b = p & 0xFF;
            }

            uint32_t aByte = static_cast<uint32_t>(std::clamp(static_cast<int>(std::round(edgeAA * 255.0f)), 1, 255));
            dstRow[x] = (aByte << 24) | (((r * aByte) >> 8) << 16) | (((g * aByte) >> 8) << 8) | ((b * aByte) >> 8);
        }
    }

    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
    ID2D1Bitmap* pBmp = nullptr;
    if (SUCCEEDED(pRT->CreateBitmap(D2D1::SizeU(static_cast<UINT32>(w), static_cast<UINT32>(h)),
                                    outPx.data(), static_cast<UINT32>(w * 4), &props, &pBmp)) && pBmp) {
        pRT->DrawBitmap(pBmp, D2D1::RectF(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h)));
        pBmp->Release();
    }
}

static void DrawD2DGlassRim(ID2D1RenderTarget* pRT, D2D1_RECT_F rc, float radius, UINT dpi,
                            bool isLight, float thicknessDip = 1.3f) {
    D2D1_GRADIENT_STOP stops[5] = {
        { 0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.88f : 0.92f) },
        { 0.30f, D2D1::ColorF(185.0f / 255.0f, 228.0f / 255.0f, 1.00f, isLight ? 0.42f : 0.46f) },
        { 0.52f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.22f : 0.18f) },
        { 0.76f, D2D1::ColorF(1.00f, 210.0f / 255.0f, 240.0f / 255.0f, isLight ? 0.44f : 0.48f) },
        { 1.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.72f : 0.76f) }
    };
    ID2D1GradientStopCollection* pStops = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(stops, 5, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
        ID2D1LinearGradientBrush* pEdgeBr = nullptr;
        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(rc.left, rc.top), D2D1::Point2F(rc.right, rc.bottom));
        if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pStops, &pEdgeBr))) {
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rc, radius, radius), pEdgeBr, DpiScaleF(thicknessDip, dpi));
            pEdgeBr->Release();
        }
        pStops->Release();
    }
    if (isLight) {
        ID2D1SolidColorBrush* pDarkRim = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.12f), &pDarkRim))) {
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rc, radius, radius), pDarkRim, DpiScaleF(0.85f, dpi));
            pDarkRim->Release();
        }
    }
}


static void DrawThemedPillSurface(ID2D1RenderTarget* pRT, HWND hWnd, int w, int h,
                                  D2D1_RECT_F rc, float cornerR, UINT dpi,
                                  const ModSettings& cfg, bool isLight) {
    ResolvedThemeVisuals tv = ResolveEffectiveThemeVisuals(cfg, isLight);
    if (tv.surface == SurfaceTheme::Solid) {
        ID2D1SolidColorBrush* pBr = nullptr;
        D2D1_COLOR_F col = tv.hasCustomBg
            ? tv.bgColor
            : (isLight ? D2D1::ColorF(0.96f, 0.96f, 0.98f, 0.98f) : D2D1::ColorF(0.11f, 0.12f, 0.15f, 0.98f));
        if (SUCCEEDED(pRT->CreateSolidColorBrush(col, &pBr)) && pBr) {
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rc, cornerR, cornerR), pBr);
            pBr->Release();
        }
        return;
    }
    DrawPillGlassBackdrop(pRT, hWnd, w, h, cornerR, dpi, tv.surface, cfg.glassOpacity, cfg.blurOpacity, isLight);
}

static void DrawThemedPillBorder(ID2D1RenderTarget* pRT, D2D1_RECT_F rc, float cornerR, UINT dpi,
                                 const ModSettings& cfg, bool isLight, float thickDip = 1.3f) {
    ResolvedThemeVisuals tv = ResolveEffectiveThemeVisuals(cfg, isLight);
    if (tv.hasCustomBorder) {
        ID2D1SolidColorBrush* pBr = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(tv.borderColor, &pBr)) && pBr) {
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rc, cornerR, cornerR), pBr, DpiScaleF(thickDip, dpi));
            pBr->Release();
        }
    } else {
        DrawD2DGlassRim(pRT, rc, cornerR, dpi, isLight, thickDip);
    }
}

static void DrawD2DGlassButton(ID2D1RenderTarget* pRT, D2D1_RECT_F rc, float radius, UINT dpi,
                               D2D1_COLOR_F bgTint, bool hovered, bool pressed, bool isLight) {
    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rc, radius, radius);

    ID2D1SolidColorBrush* pBaseBr = nullptr;
    if (SUCCEEDED(pRT->CreateSolidColorBrush(
            isLight ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.035f) : D2D1::ColorF(0.69f, 0.69f, 0.69f, 0.06f), &pBaseBr))) {
        pRT->FillRoundedRectangle(rr, pBaseBr);
        pBaseBr->Release();
    }
    if (bgTint.a > 0.001f) {
        ID2D1SolidColorBrush* pTintBr = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(bgTint, &pTintBr))) {
            pRT->FillRoundedRectangle(rr, pTintBr);
            pTintBr->Release();
        }
    }

    float hlOp = pressed ? 0.50f : (hovered ? 0.30f : 0.16f);
    D2D1_GRADIENT_STOP hlStops[3] = {
        { 0.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.00f * hlOp) },
        { 0.45f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.00f) },
        { 1.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.53f * hlOp) }
    };
    ID2D1GradientStopCollection* pHlStops = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(hlStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pHlStops))) {
        ID2D1LinearGradientBrush* pHlBr = nullptr;
        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(rc.left, rc.top), D2D1::Point2F(rc.right, rc.bottom));
        if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pHlStops, &pHlBr))) {
            pRT->FillRoundedRectangle(rr, pHlBr);
            pHlBr->Release();
        }
        pHlStops->Release();
    }

    DrawD2DGlassRim(pRT, rc, radius, dpi, isLight, 1.15f);
}

static void DrawD2DLiquidToggleSwitch(ID2D1RenderTarget* pRT, float rightEdgeX, float centerY,
                                      UINT dpi, bool isOn, bool isLight, bool isHovered) {
    float trackW = DpiScaleF(36.0f, dpi);
    float trackH = DpiScaleF(18.0f, dpi);
    float trackR = trackH * 0.5f;
    D2D1_RECT_F rcTrack = D2D1::RectF(rightEdgeX - trackW, centerY - trackR, rightEdgeX, centerY + trackR);
    D2D1_ROUNDED_RECT rrTrack = D2D1::RoundedRect(rcTrack, trackR, trackR);

    if (isOn) {
        D2D1_GRADIENT_STOP onStops[3] = {
            { 0.00f, D2D1::ColorF(0.28f, 0.84f, 1.00f, isHovered ? 0.74f : 0.62f) },
            { 0.52f, D2D1::ColorF(0.14f, 0.58f, 0.98f, isHovered ? 0.64f : 0.52f) },
            { 1.00f, D2D1::ColorF(0.42f, 0.40f, 0.98f, isHovered ? 0.70f : 0.58f) }
        };
        ID2D1GradientStopCollection* pStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(onStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
            ID2D1LinearGradientBrush* pBr = nullptr;
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(rcTrack.left, rcTrack.top), D2D1::Point2F(rcTrack.right, rcTrack.bottom));
            if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pStops, &pBr))) {
                pRT->FillRoundedRectangle(rrTrack, pBr);
                pBr->Release();
            }
            pStops->Release();
        }
    } else {
        ID2D1SolidColorBrush* pOffBr = nullptr;
        D2D1_COLOR_F offCol = isLight
            ? D2D1::ColorF(0.04f, 0.08f, 0.16f, isHovered ? 0.15f : 0.09f)
            : D2D1::ColorF(0.82f, 0.90f, 1.00f, isHovered ? 0.18f : 0.11f);
        if (SUCCEEDED(pRT->CreateSolidColorBrush(offCol, &pOffBr))) {
            pRT->FillRoundedRectangle(rrTrack, pOffBr);
            pOffBr->Release();
        }
    }

    DrawD2DGlassRim(pRT, rcTrack, trackR, dpi, isLight, 1.1f);
    float thumbR = DpiScaleF(isHovered ? 6.4f : 5.9f, dpi);
    float thumbCx = isOn ? (rcTrack.right - trackR) : (rcTrack.left + trackR);
    ID2D1SolidColorBrush* pThumbBr = nullptr;
    if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, isOn ? 0.96f : 0.84f), &pThumbBr))) {
        pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(thumbCx, centerY), thumbR, thumbR), pThumbBr);
        pThumbBr->Release();
    }
}

static void ApplyBorderlessPerPixelDwm(HWND hWnd, bool isLight, bool excludeFromCapture = true) {
    if (!hWnd || !IsWindow(hWnd)) return;
    BOOL dark = isLight ? FALSE : TRUE;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    DWORD corner = 1; // DWMWCP_DONOTROUND
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    COLORREF borderCol = 0xFFFFFFFE;
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &borderCol, sizeof(borderCol));

    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hWnd, &margins);

    HRGN hEmpty = CreateRectRgn(0, 0, -1, -1);
    DWM_BLURBEHIND bb{};
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.fEnable = TRUE;
    bb.hRgnBlur = hEmpty;
    DwmEnableBlurBehindWindow(hWnd, &bb);
    if (hEmpty) DeleteObject(hEmpty);

    SetWindowDisplayAffinity(hWnd, excludeFromCapture ? WDA_EXCLUDEFROMCAPTURE : 0);
}

// ============================================================================
// Crisp Direct2D Vector Icons
// ============================================================================

static void DrawIconGear(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float innerR = DpiScaleF(2.1f, dpi), bodyR = DpiScaleF(4.4f, dpi), toothR = DpiScaleF(6.1f, dpi);
    for (int i = 0; i < 8; ++i) {
        float a = i * (3.14159265f / 4.0f), c = std::cos(a), s = std::sin(a);
        pRT->DrawLine(D2D1::Point2F(cx + c * (bodyR - DpiScaleF(0.4f, dpi)), cy + s * (bodyR - DpiScaleF(0.4f, dpi))),
                      D2D1::Point2F(cx + c * toothR, cy + s * toothR), pBr, DpiScaleF(1.5f, dpi), pSt);
    }
    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), bodyR, bodyR), pBr, DpiScaleF(1.35f, dpi), pSt);
    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), innerR, innerR), pBr, DpiScaleF(1.15f, dpi), pSt);
}

static void DrawIconClose(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float r = DpiScaleF(3.8f, dpi), sw = DpiScaleF(1.4f, dpi);
    pRT->DrawLine(D2D1::Point2F(cx - r, cy - r), D2D1::Point2F(cx + r, cy + r), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + r, cy - r), D2D1::Point2F(cx - r, cy + r), pBr, sw, pSt);
}

static void DrawIconRegion(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float r = DpiScaleF(5.2f, dpi), arm = DpiScaleF(2.8f, dpi), sw = DpiScaleF(1.4f, dpi);
    pRT->DrawLine(D2D1::Point2F(cx - r, cy - r + arm), D2D1::Point2F(cx - r, cy - r), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx - r, cy - r), D2D1::Point2F(cx - r + arm, cy - r), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + r - arm, cy - r), D2D1::Point2F(cx + r, cy - r), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + r, cy - r), D2D1::Point2F(cx + r, cy - r + arm), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx - r, cy + r - arm), D2D1::Point2F(cx - r, cy + r), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx - r, cy + r), D2D1::Point2F(cx - r + arm, cy + r), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + r - arm, cy + r), D2D1::Point2F(cx + r, cy + r), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + r, cy + r), D2D1::Point2F(cx + r, cy + r - arm), pBr, sw, pSt);
}

static void DrawIconPen(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.35f, dpi);
    auto pt = [&](float dx, float dy) { return D2D1::Point2F(cx + DpiScaleF(dx, dpi), cy + DpiScaleF(dy, dpi)); };
    // Diagonal pencil body + tip
    pRT->DrawLine(pt(-5.0f, 5.0f), pt(-3.8f, 1.4f), pBr, sw, pSt);
    pRT->DrawLine(pt(-3.8f, 1.4f), pt(2.6f, -5.0f), pBr, sw, pSt);
    pRT->DrawLine(pt(2.6f, -5.0f), pt(5.0f, -2.6f), pBr, sw, pSt);
    pRT->DrawLine(pt(5.0f, -2.6f), pt(-1.4f, 3.8f), pBr, sw, pSt);
    pRT->DrawLine(pt(-1.4f, 3.8f), pt(-5.0f, 5.0f), pBr, sw, pSt);
}

static void DrawIconHighlighter(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.4f, dpi);
    auto pt = [&](float dx, float dy) { return D2D1::Point2F(cx + DpiScaleF(dx, dpi), cy + DpiScaleF(dy, dpi)); };
    pRT->DrawLine(pt(-4.8f, 4.8f), pt(4.8f, 4.8f), pBr, DpiScaleF(2.2f, dpi), pSt);
    pRT->DrawLine(pt(-3.0f, 2.2f), pt(2.0f, -4.6f), pBr, sw, pSt);
    pRT->DrawLine(pt(2.0f, -4.6f), pt(4.6f, -2.0f), pBr, sw, pSt);
    pRT->DrawLine(pt(4.6f, -2.0f), pt(-0.4f, 2.6f), pBr, sw, pSt);
}

static void DrawIconLine(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float r = DpiScaleF(4.8f, dpi);
    pRT->DrawLine(D2D1::Point2F(cx - r, cy + r), D2D1::Point2F(cx + r, cy - r), pBr, DpiScaleF(1.6f, dpi), pSt);
}

static void DrawIconArrow(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float r = DpiScaleF(4.8f, dpi), sw = DpiScaleF(1.5f, dpi);
    pRT->DrawLine(D2D1::Point2F(cx - r, cy + r), D2D1::Point2F(cx + r, cy - r), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + r, cy - r), D2D1::Point2F(cx + r - DpiScaleF(4.2f, dpi), cy - r), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + r, cy - r), D2D1::Point2F(cx + r, cy - r + DpiScaleF(4.2f, dpi)), pBr, sw, pSt);
}

static void DrawIconRect(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float hw = DpiScaleF(5.2f, dpi), hh = DpiScaleF(3.8f, dpi);
    pRT->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - hw, cy - hh, cx + hw, cy + hh), DpiScaleF(1.2f, dpi), DpiScaleF(1.2f, dpi)),
                              pBr, DpiScaleF(1.4f, dpi), pSt);
}

static void DrawIconEllipse(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), DpiScaleF(5.2f, dpi), DpiScaleF(4.2f, dpi)),
                     pBr, DpiScaleF(1.4f, dpi), pSt);
}

static void DrawIconEraser(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.35f, dpi);
    auto pt = [&](float dx, float dy) { return D2D1::Point2F(cx + DpiScaleF(dx, dpi), cy + DpiScaleF(dy, dpi)); };
    pRT->DrawLine(pt(-4.8f, 1.4f), pt(0.8f, -4.4f), pBr, sw, pSt);
    pRT->DrawLine(pt(0.8f, -4.4f), pt(5.0f, -0.2f), pBr, sw, pSt);
    pRT->DrawLine(pt(5.0f, -0.2f), pt(-0.2f, 4.8f), pBr, sw, pSt);
    pRT->DrawLine(pt(-0.2f, 4.8f), pt(-3.2f, 4.8f), pBr, sw, pSt);
    pRT->DrawLine(pt(-3.2f, 4.8f), pt(-4.8f, 1.4f), pBr, sw, pSt);
    pRT->DrawLine(pt(-2.0f, 4.8f), pt(5.0f, 4.8f), pBr, sw, pSt);
}

static void DrawIconCursor(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.35f, dpi);
    auto pt = [&](float dx, float dy) { return D2D1::Point2F(cx + DpiScaleF(dx, dpi), cy + DpiScaleF(dy, dpi)); };
    pRT->DrawLine(pt(-3.2f, -5.2f), pt(-3.2f, 4.4f), pBr, sw, pSt);
    pRT->DrawLine(pt(-3.2f, 4.4f), pt(-0.4f, 1.8f), pBr, sw, pSt);
    pRT->DrawLine(pt(-0.4f, 1.8f), pt(1.6f, 5.4f), pBr, sw, pSt);
    pRT->DrawLine(pt(-3.2f, -5.2f), pt(3.8f, 1.0f), pBr, sw, pSt);
    pRT->DrawLine(pt(3.8f, 1.0f), pt(-0.4f, 1.8f), pBr, sw, pSt);
}

static void DrawIconUndo(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.4f, dpi);
    auto pt = [&](float dx, float dy) { return D2D1::Point2F(cx + DpiScaleF(dx, dpi), cy + DpiScaleF(dy, dpi)); };
    pRT->DrawLine(pt(-4.6f, -1.2f), pt(2.2f, -1.2f), pBr, sw, pSt);
    pRT->DrawLine(pt(2.2f, -1.2f), pt(4.6f, 1.4f), pBr, sw, pSt);
    pRT->DrawLine(pt(4.6f, 1.4f), pt(2.2f, 4.0f), pBr, sw, pSt);
    pRT->DrawLine(pt(-4.6f, -1.2f), pt(-2.0f, -3.8f), pBr, sw, pSt);
    pRT->DrawLine(pt(-4.6f, -1.2f), pt(-2.0f, 1.4f), pBr, sw, pSt);
}

static void DrawIconRedo(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.4f, dpi);
    auto pt = [&](float dx, float dy) { return D2D1::Point2F(cx + DpiScaleF(dx, dpi), cy + DpiScaleF(dy, dpi)); };
    pRT->DrawLine(pt(4.6f, -1.2f), pt(-2.2f, -1.2f), pBr, sw, pSt);
    pRT->DrawLine(pt(-2.2f, -1.2f), pt(-4.6f, 1.4f), pBr, sw, pSt);
    pRT->DrawLine(pt(-4.6f, 1.4f), pt(-2.2f, 4.0f), pBr, sw, pSt);
    pRT->DrawLine(pt(4.6f, -1.2f), pt(2.0f, -3.8f), pBr, sw, pSt);
    pRT->DrawLine(pt(4.6f, -1.2f), pt(2.0f, 1.4f), pBr, sw, pSt);
}

static void DrawIconTrash(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.3f, dpi);
    float topY = cy - DpiScaleF(3.6f, dpi), botY = cy + DpiScaleF(5.0f, dpi), hw = DpiScaleF(4.0f, dpi);
    pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(5.2f, dpi), topY), D2D1::Point2F(cx + DpiScaleF(5.2f, dpi), topY), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(1.8f, dpi), topY - DpiScaleF(1.6f, dpi)),
                  D2D1::Point2F(cx + DpiScaleF(1.8f, dpi), topY - DpiScaleF(1.6f, dpi)), pBr, sw, pSt);
    pRT->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - hw, topY, cx + hw, botY), 1.0f, 1.0f), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(1.4f, dpi), topY + DpiScaleF(2.2f, dpi)),
                  D2D1::Point2F(cx - DpiScaleF(1.4f, dpi), botY - DpiScaleF(1.8f, dpi)), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + DpiScaleF(1.4f, dpi), topY + DpiScaleF(2.2f, dpi)),
                  D2D1::Point2F(cx + DpiScaleF(1.4f, dpi), botY - DpiScaleF(1.8f, dpi)), pBr, sw, pSt);
}

static void DrawIconSpeaker(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi, bool enabled) {
    float sw = DpiScaleF(1.3f, dpi);
    D2D1_RECT_F box = D2D1::RectF(cx - DpiScaleF(5.5f, dpi), cy - DpiScaleF(2.2f, dpi),
                                  cx - DpiScaleF(2.5f, dpi), cy + DpiScaleF(2.2f, dpi));
    pRT->DrawRectangle(box, pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(2.5f, dpi), cy - DpiScaleF(2.2f, dpi)),
                  D2D1::Point2F(cx + DpiScaleF(0.8f, dpi), cy - DpiScaleF(4.6f, dpi)), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + DpiScaleF(0.8f, dpi), cy - DpiScaleF(4.6f, dpi)),
                  D2D1::Point2F(cx + DpiScaleF(0.8f, dpi), cy + DpiScaleF(4.6f, dpi)), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + DpiScaleF(0.8f, dpi), cy + DpiScaleF(4.6f, dpi)),
                  D2D1::Point2F(cx - DpiScaleF(2.5f, dpi), cy + DpiScaleF(2.2f, dpi)), pBr, sw, pSt);
    if (enabled) {
        pRT->DrawLine(D2D1::Point2F(cx + DpiScaleF(3.2f, dpi), cy - DpiScaleF(2.2f, dpi)),
                      D2D1::Point2F(cx + DpiScaleF(3.2f, dpi), cy + DpiScaleF(2.2f, dpi)), pBr, sw, pSt);
        pRT->DrawLine(D2D1::Point2F(cx + DpiScaleF(5.4f, dpi), cy - DpiScaleF(3.8f, dpi)),
                      D2D1::Point2F(cx + DpiScaleF(5.4f, dpi), cy + DpiScaleF(3.8f, dpi)), pBr, sw, pSt);
    } else {
        pRT->DrawLine(D2D1::Point2F(cx + DpiScaleF(2.8f, dpi), cy - DpiScaleF(2.4f, dpi)),
                      D2D1::Point2F(cx + DpiScaleF(6.2f, dpi), cy + DpiScaleF(2.4f, dpi)), pBr, sw, pSt);
        pRT->DrawLine(D2D1::Point2F(cx + DpiScaleF(6.2f, dpi), cy - DpiScaleF(2.4f, dpi)),
                      D2D1::Point2F(cx + DpiScaleF(2.8f, dpi), cy + DpiScaleF(2.4f, dpi)), pBr, sw, pSt);
    }
}

static void DrawIconMic(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi, bool enabled) {
    float sw = DpiScaleF(1.3f, dpi);
    D2D1_RECT_F cap = D2D1::RectF(cx - DpiScaleF(2.4f, dpi), cy - DpiScaleF(5.6f, dpi),
                                  cx + DpiScaleF(2.4f, dpi), cy + DpiScaleF(0.6f, dpi));
    pRT->DrawRoundedRectangle(D2D1::RoundedRect(cap, DpiScaleF(2.4f, dpi), DpiScaleF(2.4f, dpi)), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(4.4f, dpi), cy - DpiScaleF(0.6f, dpi)),
                  D2D1::Point2F(cx - DpiScaleF(4.4f, dpi), cy + DpiScaleF(2.2f, dpi)), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + DpiScaleF(4.4f, dpi), cy - DpiScaleF(0.6f, dpi)),
                  D2D1::Point2F(cx + DpiScaleF(4.4f, dpi), cy + DpiScaleF(2.2f, dpi)), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(4.4f, dpi), cy + DpiScaleF(2.2f, dpi)),
                  D2D1::Point2F(cx + DpiScaleF(4.4f, dpi), cy + DpiScaleF(2.2f, dpi)), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx, cy + DpiScaleF(2.2f, dpi)), D2D1::Point2F(cx, cy + DpiScaleF(5.4f, dpi)), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(2.6f, dpi), cy + DpiScaleF(5.4f, dpi)),
                  D2D1::Point2F(cx + DpiScaleF(2.6f, dpi), cy + DpiScaleF(5.4f, dpi)), pBr, sw, pSt);
    if (!enabled) {
        pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(5.2f, dpi), cy - DpiScaleF(5.0f, dpi)),
                      D2D1::Point2F(cx + DpiScaleF(5.2f, dpi), cy + DpiScaleF(5.0f, dpi)), pBr, sw, pSt);
    }
}

static void DrawIconPauseOrPlay(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi, bool isPaused) {
    if (!isPaused) {
        float bw = DpiScaleF(2.2f, dpi), bh = DpiScaleF(5.0f, dpi), gap = DpiScaleF(2.0f, dpi);
        pRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - gap - bw, cy - bh, cx - gap, cy + bh), 1.0f, 1.0f), pBr);
        pRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx + gap, cy - bh, cx + gap + bw, cy + bh), 1.0f, 1.0f), pBr);
    } else if (g_pD2DFactory) {
        ID2D1PathGeometry* pPath = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pPath)) && pPath) {
            ID2D1GeometrySink* pSink = nullptr;
            if (SUCCEEDED(pPath->Open(&pSink)) && pSink) {
                pSink->BeginFigure(D2D1::Point2F(cx - DpiScaleF(3.2f, dpi), cy - DpiScaleF(4.8f, dpi)), D2D1_FIGURE_BEGIN_FILLED);
                pSink->AddLine(D2D1::Point2F(cx + DpiScaleF(4.6f, dpi), cy));
                pSink->AddLine(D2D1::Point2F(cx - DpiScaleF(3.2f, dpi), cy + DpiScaleF(4.8f, dpi)));
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                pSink->Close();
                pSink->Release();
                pRT->FillGeometry(pPath, pBr);
            }
            pPath->Release();
        }
    } else {
        (void)pSt;
    }
}

static void DrawIconStop(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, float cx, float cy, UINT dpi) {
    float r = DpiScaleF(4.4f, dpi);
    pRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - r, cy - r, cx + r, cy + r), DpiScaleF(1.6f, dpi), DpiScaleF(1.6f, dpi)), pBr);
}

static void DrawIconChevronRight(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float h = DpiScaleF(3.6f, dpi), w = DpiScaleF(2.1f, dpi), sw = DpiScaleF(1.35f, dpi);
    pRT->DrawLine(D2D1::Point2F(cx - w, cy - h), D2D1::Point2F(cx + w, cy), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx + w, cy), D2D1::Point2F(cx - w, cy + h), pBr, sw, pSt);
}

static void DrawIconCheckmark(ID2D1RenderTarget* pRT, ID2D1Brush* pBr, ID2D1StrokeStyle* pSt, float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.55f, dpi);
    pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(4.0f, dpi), cy + DpiScaleF(0.2f, dpi)),
                  D2D1::Point2F(cx - DpiScaleF(1.2f, dpi), cy + DpiScaleF(3.2f, dpi)), pBr, sw, pSt);
    pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(1.2f, dpi), cy + DpiScaleF(3.2f, dpi)),
                  D2D1::Point2F(cx + DpiScaleF(4.4f, dpi), cy - DpiScaleF(3.0f, dpi)), pBr, sw, pSt);
}

// ============================================================================
// Screen Annotation / Drawing Engine (Full Tools + Liquid Glass Toolbar)
// ============================================================================

enum class DrawTool : int {
    Pen = 0,
    Highlighter = 1,
    Line = 2,
    Arrow = 3,
    Rect = 4,
    Ellipse = 5,
    Eraser = 6,
    MousePassThrough = 7
};

struct DrawStroke {
    DrawTool tool = DrawTool::Pen;
    D2D1_COLOR_F color = D2D1::ColorF(0.94f, 0.27f, 0.27f, 1.0f);
    float width = 5.0f;
    std::vector<D2D1_POINT_2F> points;
};

static const D2D1_COLOR_F kDrawPalette[7] = {
    D2D1::ColorF(0.94f, 0.27f, 0.27f, 1.0f), // Red
    D2D1::ColorF(0.98f, 0.80f, 0.08f, 1.0f), // Yellow
    D2D1::ColorF(0.13f, 0.77f, 0.37f, 1.0f), // Green
    D2D1::ColorF(0.22f, 0.74f, 0.97f, 1.0f), // Cyan/Blue
    D2D1::ColorF(0.66f, 0.33f, 0.97f, 1.0f), // Purple
    D2D1::ColorF(1.00f, 1.00f, 1.00f, 1.0f), // White
    D2D1::ColorF(0.08f, 0.09f, 0.12f, 1.0f)  // Black
};
static const float kDrawSizes[3] = { 3.5f, 6.5f, 12.0f };

struct DrawingState {
    DrawTool tool = DrawTool::Pen;
    int colorIdx = 0;
    int sizeIdx = 1;
    std::vector<DrawStroke> strokes;
    std::vector<DrawStroke> redoStack;
    bool isDrawing = false;
    DrawStroke activeStroke;

    // Toolbar state
    UINT barDpi = 96;
    bool barLight = false;
    int barX = 100, barY = 80;
    bool barDragged = false;
    bool barDragging = false;
    POINT barDragStartCursor{};
    int barDragStartX = 0, barDragStartY = 0;
    int hoveredItem = -1;
    int pressedItem = -1;
};

static DrawingState g_draw;

static void RenderDrawCanvas();
static void UpdateDrawCanvasPassThrough();
static void SetDrawingMode(bool enabled);

static float PointToSegmentDistSq(D2D1_POINT_2F p, D2D1_POINT_2F a, D2D1_POINT_2F b) {
    float vx = b.x - a.x, vy = b.y - a.y;
    float wx = p.x - a.x, wy = p.y - a.y;
    float c1 = vx * wx + vy * wy;
    if (c1 <= 0.0f) return (p.x - a.x) * (p.x - a.x) + (p.y - a.y) * (p.y - a.y);
    float c2 = vx * vx + vy * vy;
    if (c2 <= c1) return (p.x - b.x) * (p.x - b.x) + (p.y - b.y) * (p.y - b.y);
    float t = c1 / c2;
    float px = a.x + t * vx, py = a.y + t * vy;
    return (p.x - px) * (p.x - px) + (p.y - py) * (p.y - py);
}

static bool StrokeHitTest(const DrawStroke& s, D2D1_POINT_2F pt, float radius) {
    if (s.points.empty()) return false;
    float rSq = (radius + s.width * 0.6f) * (radius + s.width * 0.6f);
    if (s.tool == DrawTool::Rect || s.tool == DrawTool::Ellipse) {
        if (s.points.size() < 2) return false;
        float x0 = std::min(s.points.front().x, s.points.back().x) - radius;
        float x1 = std::max(s.points.front().x, s.points.back().x) + radius;
        float y0 = std::min(s.points.front().y, s.points.back().y) - radius;
        float y1 = std::max(s.points.front().y, s.points.back().y) + radius;
        return pt.x >= x0 && pt.x <= x1 && pt.y >= y0 && pt.y <= y1;
    }
    if (s.points.size() == 1) {
        float dx = pt.x - s.points[0].x, dy = pt.y - s.points[0].y;
        return (dx * dx + dy * dy) <= rSq;
    }
    for (size_t i = 1; i < s.points.size(); ++i) {
        if (PointToSegmentDistSq(pt, s.points[i - 1], s.points[i]) <= rSq) return true;
    }
    return false;
}

static void DrawSingleStrokeD2D(ID2D1RenderTarget* pRT, ID2D1StrokeStyle* pStroke, const DrawStroke& s) {
    if (s.points.empty()) return;
    D2D1_COLOR_F col = s.color;
    float w = s.width;
    if (s.tool == DrawTool::Highlighter) {
        col.a = 0.36f;
        w = s.width * 3.2f;
    }
    ID2D1SolidColorBrush* pBr = nullptr;
    if (FAILED(pRT->CreateSolidColorBrush(col, &pBr)) || !pBr) return;

    if (s.tool == DrawTool::Pen || s.tool == DrawTool::Highlighter) {
        if (s.points.size() == 1) {
            pRT->FillEllipse(D2D1::Ellipse(s.points[0], w * 0.5f, w * 0.5f), pBr);
        } else if (g_pD2DFactory) {
            ID2D1PathGeometry* pPath = nullptr;
            if (SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pPath)) && pPath) {
                ID2D1GeometrySink* pSink = nullptr;
                if (SUCCEEDED(pPath->Open(&pSink)) && pSink) {
                    pSink->BeginFigure(s.points[0], D2D1_FIGURE_BEGIN_HOLLOW);
                    for (size_t i = 1; i < s.points.size(); ++i) {
                        pSink->AddLine(s.points[i]);
                    }
                    pSink->EndFigure(D2D1_FIGURE_END_OPEN);
                    pSink->Close();
                    pSink->Release();
                    pRT->DrawGeometry(pPath, pBr, w, pStroke);
                }
                pPath->Release();
            }
        }
    } else if (s.points.size() >= 2) {
        D2D1_POINT_2F p0 = s.points.front();
        D2D1_POINT_2F p1 = s.points.back();
        if (s.tool == DrawTool::Line || s.tool == DrawTool::Arrow) {
            pRT->DrawLine(p0, p1, pBr, w, pStroke);
            if (s.tool == DrawTool::Arrow) {
                float dx = p1.x - p0.x, dy = p1.y - p0.y;
                float len = std::sqrt(dx * dx + dy * dy);
                if (len > 4.0f) {
                    float ux = dx / len, uy = dy / len;
                    float headLen = std::max(14.0f, w * 3.2f);
                    float headW = headLen * 0.55f;
                    D2D1_POINT_2F leftP{ p1.x - ux * headLen - uy * headW, p1.y - uy * headLen + ux * headW };
                    D2D1_POINT_2F rightP{ p1.x - ux * headLen + uy * headW, p1.y - uy * headLen - ux * headW };
                    pRT->DrawLine(p1, leftP, pBr, w, pStroke);
                    pRT->DrawLine(p1, rightP, pBr, w, pStroke);
                }
            }
        } else if (s.tool == DrawTool::Rect) {
            D2D1_RECT_F r = D2D1::RectF(std::min(p0.x, p1.x), std::min(p0.y, p1.y),
                                        std::max(p0.x, p1.x), std::max(p0.y, p1.y));
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(r, 4.0f, 4.0f), pBr, w, pStroke);
        } else if (s.tool == DrawTool::Ellipse) {
            float rx = std::fabs(p1.x - p0.x) * 0.5f;
            float ry = std::fabs(p1.y - p0.y) * 0.5f;
            D2D1_POINT_2F c{ (p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f };
            pRT->DrawEllipse(D2D1::Ellipse(c, rx, ry), pBr, w, pStroke);
        }
    }

    pBr->Release();
}

static void RenderDrawCanvas() {
    if (!g_drawCanvasHwnd || !IsWindow(g_drawCanvasHwnd) || !g_pD2DFactory) return;

    int virtX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int virtY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (w <= 0 || h <= 0) return;

    HDC screenDc = GetDC(nullptr);
    HDC memDc = CreateCompatibleDC(screenDc);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* pixels = nullptr;
    HBITMAP hBmp = CreateDIBSection(memDc, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels), nullptr, 0);
    if (hBmp && pixels) {
        HGDIOBJ oldBmp = SelectObject(memDc, hBmp);

        RECT rc{0, 0, w, h};
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f);

        ID2D1DCRenderTarget* pRT = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&rtProps, &pRT)) && pRT) {
            pRT->BindDC(memDc, &rc);
            pRT->BeginDraw();
            pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

            // Alpha = 1/255 when actively drawing so Windows routes mouse clicks anywhere on screen to the canvas;
            // Alpha = 0 when in Mouse Pass-through mode.
            bool passThrough = (g_draw.tool == DrawTool::MousePassThrough);
            pRT->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, passThrough ? 0.0f : (1.0f / 255.0f)));

            ID2D1StrokeStyle* pStroke = nullptr;
            D2D1_STROKE_STYLE_PROPERTIES sp = D2D1::StrokeStyleProperties(
                D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND);
            g_pD2DFactory->CreateStrokeStyle(sp, nullptr, 0, &pStroke);

            for (const auto& s : g_draw.strokes) {
                DrawSingleStrokeD2D(pRT, pStroke, s);
            }
            if (g_draw.isDrawing) {
                DrawSingleStrokeD2D(pRT, pStroke, g_draw.activeStroke);
            }

            pRT->EndDraw();
            if (pStroke) pStroke->Release();
            pRT->Release();
        }

        POINT ptSrc{0, 0};
        POINT ptDst{virtX, virtY};
        SIZE sz{w, h};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(g_drawCanvasHwnd, screenDc, &ptDst, &sz, memDc, &ptSrc, 0, &bf, ULW_ALPHA);

        SelectObject(memDc, oldBmp);
        DeleteObject(hBmp);
    }

    DeleteDC(memDc);
    ReleaseDC(nullptr, screenDc);
}

static void UpdateDrawCanvasPassThrough() {
    if (!g_drawCanvasHwnd || !IsWindow(g_drawCanvasHwnd)) return;
    LONG_PTR ex = GetWindowLongPtrW(g_drawCanvasHwnd, GWL_EXSTYLE);
    if (g_draw.tool == DrawTool::MousePassThrough) {
        ex |= WS_EX_TRANSPARENT;
    } else {
        ex &= ~WS_EX_TRANSPARENT;
    }
    SetWindowLongPtrW(g_drawCanvasHwnd, GWL_EXSTYLE, ex);
    RenderDrawCanvas();
}

static LRESULT CALLBACK DrawCanvasWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_SETCURSOR:
            if (g_draw.tool != DrawTool::MousePassThrough) {
                SetCursor(LoadCursorW(nullptr, IDC_CROSS));
                return TRUE;
            }
            break;

        case WM_LBUTTONDOWN: {
            if (g_draw.tool == DrawTool::MousePassThrough) return 0;
            D2D1_POINT_2F pt{ static_cast<float>(static_cast<short>(LOWORD(lParam))),
                              static_cast<float>(static_cast<short>(HIWORD(lParam))) };
            SetCapture(hWnd);
            if (g_draw.tool == DrawTool::Eraser) {
                g_draw.isDrawing = true;
                auto oldSize = g_draw.strokes.size();
                g_draw.strokes.erase(
                    std::remove_if(g_draw.strokes.begin(), g_draw.strokes.end(),
                                   [&](const DrawStroke& s) { return StrokeHitTest(s, pt, 14.0f); }),
                    g_draw.strokes.end());
                if (g_draw.strokes.size() != oldSize) RenderDrawCanvas();
                return 0;
            }
            g_draw.isDrawing = true;
            g_draw.activeStroke = {};
            g_draw.activeStroke.tool = g_draw.tool;
            g_draw.activeStroke.color = kDrawPalette[std::clamp(g_draw.colorIdx, 0, 6)];
            g_draw.activeStroke.width = kDrawSizes[std::clamp(g_draw.sizeIdx, 0, 2)];
            g_draw.activeStroke.points.push_back(pt);
            g_draw.activeStroke.points.push_back(pt);
            RenderDrawCanvas();
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!g_draw.isDrawing) return 0;
            D2D1_POINT_2F pt{ static_cast<float>(static_cast<short>(LOWORD(lParam))),
                              static_cast<float>(static_cast<short>(HIWORD(lParam))) };
            if (g_draw.tool == DrawTool::Eraser) {
                auto oldSize = g_draw.strokes.size();
                g_draw.strokes.erase(
                    std::remove_if(g_draw.strokes.begin(), g_draw.strokes.end(),
                                   [&](const DrawStroke& s) { return StrokeHitTest(s, pt, 14.0f); }),
                    g_draw.strokes.end());
                if (g_draw.strokes.size() != oldSize) RenderDrawCanvas();
                return 0;
            }
            if (g_draw.activeStroke.tool == DrawTool::Pen || g_draw.activeStroke.tool == DrawTool::Highlighter) {
                auto& pts = g_draw.activeStroke.points;
                float dx = pt.x - pts.back().x, dy = pt.y - pts.back().y;
                if (dx * dx + dy * dy >= 4.0f) {
                    pts.push_back(pt);
                    RenderDrawCanvas();
                }
            } else {
                g_draw.activeStroke.points.back() = pt;
                RenderDrawCanvas();
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (!g_draw.isDrawing) return 0;
            g_draw.isDrawing = false;
            ReleaseCapture();
            if (g_draw.tool != DrawTool::Eraser && !g_draw.activeStroke.points.empty()) {
                g_draw.strokes.push_back(std::move(g_draw.activeStroke));
                g_draw.redoStack.clear();
                RenderDrawCanvas();
            }
            return 0;
        }

        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                SetDrawingMode(false);
                return 0;
            }
            if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
                if (wParam == 'Z' && !g_draw.strokes.empty()) {
                    g_draw.redoStack.push_back(std::move(g_draw.strokes.back()));
                    g_draw.strokes.pop_back();
                    RenderDrawCanvas();
                    return 0;
                }
                if (wParam == 'Y' && !g_draw.redoStack.empty()) {
                    g_draw.strokes.push_back(std::move(g_draw.redoStack.back()));
                    g_draw.redoStack.pop_back();
                    RenderDrawCanvas();
                    return 0;
                }
            }
            break;
        }
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// Drawing Toolbar Pill items:
// 0..7   = Tools (Pen, Highlighter, Line, Arrow, Rect, Ellipse, Eraser, MousePassThrough)
// 8..14  = 7 Color Swatches
// 15..17 = 3 Brush Sizes
// 18     = Undo
// 19     = Redo
// 20     = Clear All
// 21     = Close Drawing Mode
constexpr int kDrawBarItemCount = 22;

struct DrawBarLayout {
    int width = 0;
    int height = 0;
    RECT items[kDrawBarItemCount]{};
};

static DrawBarLayout ComputeDrawBarLayout(UINT dpi) {
    DrawBarLayout l{};
    int h = DpiScale(46, dpi);
    int btn = DpiScale(30, dpi);
    int sw = DpiScale(20, dpi);
    int gap = DpiScale(4, dpi);
    int sepGap = DpiScale(12, dpi);
    int x = DpiScale(10, dpi);
    int topBtn = (h - btn) / 2;
    int topSw = (h - sw) / 2;

    // 8 tool buttons (0..7)
    for (int i = 0; i < 8; ++i) {
        l.items[i] = { x, topBtn, x + btn, topBtn + btn };
        x += btn + gap;
    }
    x += sepGap - gap;

    // 7 color swatches (8..14)
    for (int i = 0; i < 7; ++i) {
        l.items[8 + i] = { x, topSw, x + sw, topSw + sw };
        x += sw + DpiScale(5, dpi);
    }
    x += sepGap - DpiScale(5, dpi);

    // 3 brush sizes (15..17)
    for (int i = 0; i < 3; ++i) {
        l.items[15 + i] = { x, topBtn, x + DpiScale(26, dpi), topBtn + btn };
        x += DpiScale(26, dpi) + gap;
    }
    x += sepGap - gap;

    // 4 action buttons: Undo(18), Redo(19), Clear(20), Close(21)
    for (int i = 0; i < 4; ++i) {
        l.items[18 + i] = { x, topBtn, x + btn, topBtn + btn };
        x += btn + gap;
    }

    l.width = x - gap + DpiScale(10, dpi);
    l.height = h;
    return l;
}

static void PaintDrawToolbar(HWND hWnd) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);
    if (!g_pD2DFactory) { EndPaint(hWnd, &ps); return; }

    RECT rc{};
    GetClientRect(hWnd, &rc);
    int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0) { EndPaint(hWnd, &ps); return; }

    ModSettings cfg = GetSettingsCopy();
    UINT dpi = g_draw.barDpi;
    bool isLight = g_draw.barLight;
    DrawBarLayout lay = ComputeDrawBarLayout(dpi);

    HDC memDc = CreateCompatibleDC(hdc);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* pixels = nullptr;
    HBITMAP hBmp = CreateDIBSection(memDc, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels), nullptr, 0);
    if (hBmp && pixels) {
        HGDIOBJ oldBmp = SelectObject(memDc, hBmp);
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f);

        ID2D1DCRenderTarget* pRT = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&rtProps, &pRT)) && pRT) {
            pRT->BindDC(memDc, &rc);
            pRT->BeginDraw();
            pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            pRT->Clear(D2D1::ColorF(0, 0, 0, 0));

            float rimInset = DpiScaleF(0.75f, dpi);
            D2D1_RECT_F rcPill = D2D1::RectF(rimInset, rimInset, w - rimInset, h - rimInset);
            float cornerR = (rcPill.bottom - rcPill.top) * 0.5f;

            DrawThemedPillSurface(pRT, hWnd, w, h, D2D1::RectF(rimInset, rimInset, w - rimInset, h - rimInset), cornerR, dpi, cfg, isLight);

            ID2D1SolidColorBrush* pBrIcon = nullptr;
            ID2D1SolidColorBrush* pBrDiv = nullptr;
            pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0.08f, 0.09f, 0.12f, 1.0f) : D2D1::ColorF(0.98f, 0.98f, 1.0f, 1.0f), &pBrIcon);
            pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0, 0, 0, 0.14f) : D2D1::ColorF(1, 1, 1, 0.18f), &pBrDiv);

            ID2D1StrokeStyle* pStroke = nullptr;
            D2D1_STROKE_STYLE_PROPERTIES sp = D2D1::StrokeStyleProperties(
                D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND);
            g_pD2DFactory->CreateStrokeStyle(sp, nullptr, 0, &pStroke);

            // Subtle dividers between groups
            if (pBrDiv) {
                auto drawDiv = [&](int leftIdx, int rightIdx) {
                    float dx = (lay.items[leftIdx].right + lay.items[rightIdx].left) * 0.5f;
                    pRT->DrawLine(D2D1::Point2F(dx, DpiScaleF(11.0f, dpi)),
                                  D2D1::Point2F(dx, h - DpiScaleF(11.0f, dpi)), pBrDiv, DpiScaleF(1.0f, dpi));
                };
                drawDiv(7, 8);
                drawDiv(14, 15);
                drawDiv(17, 18);
            }

            // Tools 0..7
            for (int i = 0; i < 8; ++i) {
                RECT r = lay.items[i];
                D2D1_RECT_F rf = D2D1::RectF(static_cast<float>(r.left), static_cast<float>(r.top),
                                             static_cast<float>(r.right), static_cast<float>(r.bottom));
                bool active = (static_cast<int>(g_draw.tool) == i);
                bool hov = (g_draw.hoveredItem == i), prs = (g_draw.pressedItem == i);
                if (active || hov || prs) {
                    D2D1_COLOR_F tint = active
                        ? D2D1::ColorF(0.14f, 0.58f, 0.98f, prs ? 0.50f : 0.36f)
                        : (isLight ? D2D1::ColorF(0.06f, 0.08f, 0.14f, prs ? 0.18f : 0.10f)
                                   : D2D1::ColorF(0.85f, 0.92f, 1.00f, prs ? 0.22f : 0.13f));
                    DrawD2DGlassButton(pRT, rf, (rf.bottom - rf.top) * 0.5f, dpi, tint, hov, prs, isLight);
                }
                float cx = (rf.left + rf.right) * 0.5f, cy = (rf.top + rf.bottom) * 0.5f;
                if (pBrIcon && pStroke) {
                    if (i == 0) DrawIconPen(pRT, pBrIcon, pStroke, cx, cy, dpi);
                    else if (i == 1) DrawIconHighlighter(pRT, pBrIcon, pStroke, cx, cy, dpi);
                    else if (i == 2) DrawIconLine(pRT, pBrIcon, pStroke, cx, cy, dpi);
                    else if (i == 3) DrawIconArrow(pRT, pBrIcon, pStroke, cx, cy, dpi);
                    else if (i == 4) DrawIconRect(pRT, pBrIcon, pStroke, cx, cy, dpi);
                    else if (i == 5) DrawIconEllipse(pRT, pBrIcon, pStroke, cx, cy, dpi);
                    else if (i == 6) DrawIconEraser(pRT, pBrIcon, pStroke, cx, cy, dpi);
                    else if (i == 7) DrawIconCursor(pRT, pBrIcon, pStroke, cx, cy, dpi);
                }
            }

            // Color swatches 8..14
            for (int i = 0; i < 7; ++i) {
                RECT r = lay.items[8 + i];
                float cx = (r.left + r.right) * 0.5f, cy = (r.top + r.bottom) * 0.5f;
                float rad = DpiScaleF((g_draw.colorIdx == i || g_draw.hoveredItem == 8 + i) ? 8.2f : 7.0f, dpi);
                ID2D1SolidColorBrush* pSw = nullptr;
                if (SUCCEEDED(pRT->CreateSolidColorBrush(kDrawPalette[i], &pSw))) {
                    pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rad, rad), pSw);
                    pSw->Release();
                }
                if (g_draw.colorIdx == i && pBrIcon) {
                    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rad + DpiScaleF(1.6f, dpi), rad + DpiScaleF(1.6f, dpi)),
                                     pBrIcon, DpiScaleF(1.6f, dpi));
                }
            }

            // Brush sizes 15..17
            for (int i = 0; i < 3; ++i) {
                RECT r = lay.items[15 + i];
                D2D1_RECT_F rf = D2D1::RectF(static_cast<float>(r.left), static_cast<float>(r.top),
                                             static_cast<float>(r.right), static_cast<float>(r.bottom));
                bool active = (g_draw.sizeIdx == i);
                bool hov = (g_draw.hoveredItem == 15 + i), prs = (g_draw.pressedItem == 15 + i);
                if (active || hov || prs) {
                    D2D1_COLOR_F tint = active
                        ? D2D1::ColorF(0.14f, 0.58f, 0.98f, 0.34f)
                        : (isLight ? D2D1::ColorF(0, 0, 0, 0.10f) : D2D1::ColorF(1, 1, 1, 0.13f));
                    DrawD2DGlassButton(pRT, rf, (rf.bottom - rf.top) * 0.5f, dpi, tint, hov, prs, isLight);
                }
                float cx = (rf.left + rf.right) * 0.5f, cy = (rf.top + rf.bottom) * 0.5f;
                float dotR = DpiScaleF(i == 0 ? 2.4f : (i == 1 ? 4.0f : 5.8f), dpi);
                if (pBrIcon) pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), dotR, dotR), pBrIcon);
            }

            // Actions 18..21 (Undo, Redo, Clear, Close)
            for (int i = 0; i < 4; ++i) {
                int idx = 18 + i;
                RECT r = lay.items[idx];
                D2D1_RECT_F rf = D2D1::RectF(static_cast<float>(r.left), static_cast<float>(r.top),
                                             static_cast<float>(r.right), static_cast<float>(r.bottom));
                bool hov = (g_draw.hoveredItem == idx), prs = (g_draw.pressedItem == idx);
                if (hov || prs || i == 3) {
                    D2D1_COLOR_F tint = (i == 3)
                        ? D2D1::ColorF(0.90f, 0.20f, 0.28f, prs ? 0.42f : (hov ? 0.30f : 0.14f))
                        : (isLight ? D2D1::ColorF(0.06f, 0.08f, 0.14f, prs ? 0.18f : 0.10f)
                                   : D2D1::ColorF(0.85f, 0.92f, 1.00f, prs ? 0.22f : 0.13f));
                    DrawD2DGlassButton(pRT, rf, (rf.bottom - rf.top) * 0.5f, dpi, tint, hov, prs, isLight);
                }
                float cx = (rf.left + rf.right) * 0.5f, cy = (rf.top + rf.bottom) * 0.5f;
                if (pBrIcon && pStroke) {
                    if (i == 0) DrawIconUndo(pRT, pBrIcon, pStroke, cx, cy, dpi);
                    else if (i == 1) DrawIconRedo(pRT, pBrIcon, pStroke, cx, cy, dpi);
                    else if (i == 2) DrawIconTrash(pRT, pBrIcon, pStroke, cx, cy, dpi);
                    else if (i == 3) DrawIconClose(pRT, pBrIcon, pStroke, cx, cy, dpi);
                }
            }

            DrawThemedPillBorder(pRT, rcPill, cornerR, dpi, cfg, isLight, 1.3f);
            pRT->EndDraw();

            if (pStroke) pStroke->Release();
            if (pBrDiv) pBrDiv->Release();
            if (pBrIcon) pBrIcon->Release();
            pRT->Release();
        }

        BitBlt(hdc, 0, 0, w, h, memDc, 0, 0, SRCCOPY);
        SelectObject(memDc, oldBmp);
        DeleteObject(hBmp);
    }
    DeleteDC(memDc);
    EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK DrawBarWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCCALCSIZE: return 0;
        case WM_ERASEBKGND: return 1;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;

        case WM_MOUSEMOVE: {
            if (g_draw.barDragging) {
                POINT cur{};
                GetCursorPos(&cur);
                int nx = g_draw.barDragStartX + (cur.x - g_draw.barDragStartCursor.x);
                int ny = g_draw.barDragStartY + (cur.y - g_draw.barDragStartCursor.y);
                if (nx != g_draw.barX || ny != g_draw.barY) {
                    g_draw.barX = nx; g_draw.barY = ny; g_draw.barDragged = true;
                    SetWindowPos(hWnd, HWND_TOPMOST, nx, ny, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                return 0;
            }
            POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            DrawBarLayout lay = ComputeDrawBarLayout(g_draw.barDpi);
            int hit = -1;
            for (int i = 0; i < kDrawBarItemCount; ++i) {
                if (PtInRect(&lay.items[i], pt)) { hit = i; break; }
            }
            if (hit != g_draw.hoveredItem) {
                g_draw.hoveredItem = hit;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hWnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_draw.hoveredItem != -1 || g_draw.pressedItem != -1) {
                g_draw.hoveredItem = -1; g_draw.pressedItem = -1;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONDOWN: {
            POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            DrawBarLayout lay = ComputeDrawBarLayout(g_draw.barDpi);
            int hit = -1;
            for (int i = 0; i < kDrawBarItemCount; ++i) {
                if (PtInRect(&lay.items[i], pt)) { hit = i; break; }
            }
            if (hit >= 0) {
                g_draw.pressedItem = hit;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
            } else {
                g_draw.barDragging = true;
                GetCursorPos(&g_draw.barDragStartCursor);
                g_draw.barDragStartX = g_draw.barX;
                g_draw.barDragStartY = g_draw.barY;
                SetCapture(hWnd);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_draw.barDragging) {
                g_draw.barDragging = false;
                ReleaseCapture();
                return 0;
            }
            POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            DrawBarLayout lay = ComputeDrawBarLayout(g_draw.barDpi);
            int hit = -1;
            for (int i = 0; i < kDrawBarItemCount; ++i) {
                if (PtInRect(&lay.items[i], pt)) { hit = i; break; }
            }
            int pressed = g_draw.pressedItem;
            g_draw.pressedItem = -1;
            ReleaseCapture();

            if (hit >= 0 && hit == pressed) {
                if (hit >= 0 && hit <= 7) {
                    g_draw.tool = static_cast<DrawTool>(hit);
                    UpdateDrawCanvasPassThrough();
                } else if (hit >= 8 && hit <= 14) {
                    g_draw.colorIdx = hit - 8;
                    if (g_draw.tool == DrawTool::Eraser || g_draw.tool == DrawTool::MousePassThrough) {
                        g_draw.tool = DrawTool::Pen;
                        UpdateDrawCanvasPassThrough();
                    }
                } else if (hit >= 15 && hit <= 17) {
                    g_draw.sizeIdx = hit - 15;
                } else if (hit == 18 && !g_draw.strokes.empty()) {
                    g_draw.redoStack.push_back(std::move(g_draw.strokes.back()));
                    g_draw.strokes.pop_back();
                    RenderDrawCanvas();
                } else if (hit == 19 && !g_draw.redoStack.empty()) {
                    g_draw.strokes.push_back(std::move(g_draw.redoStack.back()));
                    g_draw.redoStack.pop_back();
                    RenderDrawCanvas();
                } else if (hit == 20) {
                    g_draw.strokes.clear();
                    g_draw.redoStack.clear();
                    RenderDrawCanvas();
                } else if (hit == 21) {
                    SetDrawingMode(false);
                    return 0;
                }
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }

        case WM_PAINT:
            PaintDrawToolbar(hWnd);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static void PaintDrawHintPill(HWND hWnd) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);
    if (!g_pD2DFactory || !g_pDWriteFactory) { EndPaint(hWnd, &ps); return; }

    RECT rc{};
    GetClientRect(hWnd, &rc);
    int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0) { EndPaint(hWnd, &ps); return; }

    ModSettings cfg = GetSettingsCopy();
    UiStrings ui = GetUiStrings(cfg.language);
    UINT dpi = g_draw.barDpi;
    bool isLight = g_draw.barLight;

    HDC memDc = CreateCompatibleDC(hdc);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* pixels = nullptr;
    HBITMAP hBmp = CreateDIBSection(memDc, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels), nullptr, 0);
    if (hBmp && pixels) {
        HGDIOBJ oldBmp = SelectObject(memDc, hBmp);
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
        ID2D1DCRenderTarget* pRT = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&rtProps, &pRT)) && pRT) {
            pRT->BindDC(memDc, &rc);
            pRT->BeginDraw();
            pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            ApplyFluentTextRendering(pRT);
            pRT->Clear(D2D1::ColorF(0, 0, 0, 0));

            float rimInset = DpiScaleF(0.75f, dpi);
            D2D1_RECT_F rcPill = D2D1::RectF(rimInset, rimInset, w - rimInset, h - rimInset);
            ResolvedThemeVisuals tv = ResolveEffectiveThemeVisuals(cfg, isLight);
            float cornerR = std::min(DpiScaleF(tv.cornerRadiusDip, dpi), (rcPill.bottom - rcPill.top) * 0.5f);
            DrawThemedPillSurface(pRT, hWnd, w, h, rcPill, cornerR, dpi, cfg, isLight);

            // Small cyan indicator dot
            ID2D1SolidColorBrush* pDotBr = nullptr;
            if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(0.22f, 0.74f, 0.97f, 1.0f), &pDotBr))) {
                pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(DpiScaleF(16.0f, dpi), h * 0.5f),
                                               DpiScaleF(3.8f, dpi), DpiScaleF(3.8f, dpi)), pDotBr);
                pDotBr->Release();
            }

            ID2D1SolidColorBrush* pBrText = nullptr;
            pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0.06f, 0.07f, 0.09f, 1.0f) : D2D1::ColorF(0.98f, 0.98f, 1.0f, 1.0f), &pBrText);
            IDWriteTextFormat* pFmt = nullptr;
            g_pDWriteFactory->CreateTextFormat(GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                               DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.0f, dpi), L"", &pFmt);
            if (pFmt && pBrText) {
                pFmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                pFmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                pRT->DrawText(ui.drawEscHint, static_cast<UINT32>(wcslen(ui.drawEscHint)), pFmt,
                              D2D1::RectF(DpiScaleF(27.0f, dpi), 0, static_cast<float>(w - DpiScale(12, dpi)), static_cast<float>(h)),
                              pBrText, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }

            DrawThemedPillBorder(pRT, rcPill, cornerR, dpi, cfg, isLight, 1.25f);
            pRT->EndDraw();

            if (pFmt) pFmt->Release();
            if (pBrText) pBrText->Release();
            pRT->Release();
        }
        BitBlt(hdc, 0, 0, w, h, memDc, 0, 0, SRCCOPY);
        SelectObject(memDc, oldBmp);
        DeleteObject(hBmp);
    }
    DeleteDC(memDc);
    EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK DrawHintWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCCALCSIZE: return 0;
        case WM_ERASEBKGND: return 1;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_PAINT: PaintDrawHintPill(hWnd); return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static void SetDrawingMode(bool enabled) {
    g_drawingEnabled = enabled;
    if (!enabled) {
        g_draw.isDrawing = false;
        g_draw.strokes.clear();
        g_draw.redoStack.clear();
        if (HWND msgWnd = g_msgHwnd.load()) {
            UnregisterHotKey(msgWnd, HOTKEY_DRAW_EXIT_ESC);
        }
        if (g_drawHintHwnd && IsWindow(g_drawHintHwnd)) {
            DestroyWindow(g_drawHintHwnd);
            g_drawHintHwnd = nullptr;
        }
        if (g_drawBarHwnd && IsWindow(g_drawBarHwnd)) {
            DestroyWindow(g_drawBarHwnd);
            g_drawBarHwnd = nullptr;
        }
        if (g_drawCanvasHwnd && IsWindow(g_drawCanvasHwnd)) {
            DestroyWindow(g_drawCanvasHwnd);
            g_drawCanvasHwnd = nullptr;
        }
        if (g_pillHwnd && IsWindow(g_pillHwnd)) InvalidateRect(g_pillHwnd, nullptr, FALSE);
        return;
    }

    int virtX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int virtY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int virtW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int virtH = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    if (!g_drawCanvasHwnd || !IsWindow(g_drawCanvasHwnd)) {
        g_drawCanvasHwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
            kDrawCanvasWndClass, L"Screen Recorder Canvas", WS_POPUP,
            virtX, virtY, virtW, virtH,
            nullptr, nullptr, GetModInstanceHandle(), nullptr);
    }
    if (g_drawCanvasHwnd) {
        // Canvas is NOT excluded from capture so drawings appear in the recorded MP4!
        SetWindowDisplayAffinity(g_drawCanvasHwnd, 0);
        ShowWindow(g_drawCanvasHwnd, SW_SHOWNOACTIVATE);
        UpdateDrawCanvasPassThrough();
    }

    if (!g_drawBarHwnd || !IsWindow(g_drawBarHwnd)) {
        g_drawBarHwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            kDrawBarWndClass, L"Drawing Toolbar", WS_POPUP,
            0, 0, 400, 46,
            nullptr, nullptr, GetModInstanceHandle(), nullptr);
    }
    if (g_drawBarHwnd) {
        UINT dpi = GetDpiForWindow(g_drawBarHwnd);
        g_draw.barDpi = dpi ? dpi : 96;
        DrawBarLayout lay = ComputeDrawBarLayout(g_draw.barDpi);

        // Position neatly above or below the main pill bar (centered horizontally)
        HMONITOR hMon = (g_pillHwnd && IsWindow(g_pillHwnd))
            ? MonitorFromWindow(g_pillHwnd, MONITOR_DEFAULTTONEAREST)
            : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi{sizeof(mi)};
        RECT rcWork{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
        if (hMon && GetMonitorInfoW(hMon, &mi)) rcWork = mi.rcWork;

        int barX = static_cast<int>(rcWork.left) + (static_cast<int>(rcWork.right - rcWork.left) - lay.width) / 2;
        int barY = static_cast<int>(rcWork.top) + DpiScale(72, g_draw.barDpi);
        if (g_pillHwnd && IsWindow(g_pillHwnd) && IsWindowVisible(g_pillHwnd)) {
            RECT rcPill{};
            GetWindowRect(g_pillHwnd, &rcPill);
            int pillMidY = (rcPill.top + rcPill.bottom) / 2;
            int workMidY = (rcWork.top + rcWork.bottom) / 2;
            if (pillMidY > workMidY) {
                barY = static_cast<int>(rcPill.top) - lay.height - DpiScale(10, g_draw.barDpi);
            } else {
                barY = static_cast<int>(rcPill.bottom) + DpiScale(10, g_draw.barDpi);
            }
        }
        g_draw.barX = barX;
        g_draw.barY = barY;
        g_draw.barDragged = false;

        ModSettings cfg = GetSettingsCopy();
        g_draw.barLight = ResolveIsLightMode(cfg, g_draw.barX, g_draw.barY, lay.width, lay.height, g_draw.barLight);
        // Drawing toolbar IS excluded from capture so only your drawings appear in the MP4!
        ApplyBorderlessPerPixelDwm(g_drawBarHwnd, g_draw.barLight, true);
        SetWindowPos(g_drawBarHwnd, HWND_TOPMOST, g_draw.barX, g_draw.barY, lay.width, lay.height,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
        // Ensure main pill stays above canvas too
        if (g_pillHwnd && IsWindow(g_pillHwnd) && IsWindowVisible(g_pillHwnd)) {
            SetWindowPos(g_pillHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        InvalidateRect(g_drawBarHwnd, nullptr, FALSE);

        // Create floating click-through "Press Esc to exit" Liquid Glass hint pill at bottom-center (excluded from capture)
        if (!g_drawHintHwnd || !IsWindow(g_drawHintHwnd)) {
            g_drawHintHwnd = CreateWindowExW(
                WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                kDrawHintWndClass, L"Drawing Esc Hint", WS_POPUP,
                0, 0, 220, 32,
                nullptr, nullptr, GetModInstanceHandle(), nullptr);
        }
        if (g_drawHintHwnd) {
            UiStrings ui = GetUiStrings(cfg.language);
            int hintH = DpiScale(32, g_draw.barDpi);
            float txtW = 160.0f;
            if (g_pDWriteFactory) {
                IDWriteTextFormat* pFmt = nullptr;
                if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
                        GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                        DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.0f, g_draw.barDpi), L"", &pFmt)) && pFmt) {
                    pFmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                    IDWriteTextLayout* pL = nullptr;
                    if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                            ui.drawEscHint, static_cast<UINT32>(wcslen(ui.drawEscHint)), pFmt, 600.0f, 40.0f, &pL)) && pL) {
                        DWRITE_TEXT_METRICS tm{};
                        if (SUCCEEDED(pL->GetMetrics(&tm))) txtW = tm.widthIncludingTrailingWhitespace;
                        pL->Release();
                    }
                    pFmt->Release();
                }
            }
            int hintW = DpiScale(42, g_draw.barDpi) + static_cast<int>(std::ceil(txtW));
            int hintX = static_cast<int>(rcWork.left) + (static_cast<int>(rcWork.right - rcWork.left) - hintW) / 2;
            int hintY = static_cast<int>(rcWork.bottom) - hintH - DpiScale(18, g_draw.barDpi);
            // If main pill is at bottom, place hint pill just above the drawing toolbar instead
            if (g_pillHwnd && IsWindow(g_pillHwnd) && IsWindowVisible(g_pillHwnd)) {
                RECT rcP{}; GetWindowRect(g_pillHwnd, &rcP);
                if (rcP.bottom > rcWork.bottom - DpiScale(100, g_draw.barDpi)) {
                    hintY = g_draw.barY - hintH - DpiScale(8, g_draw.barDpi);
                }
            }
            ApplyBorderlessPerPixelDwm(g_drawHintHwnd, g_draw.barLight, true);
            SetWindowPos(g_drawHintHwnd, HWND_TOPMOST, hintX, hintY, hintW, hintH,
                         SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
            InvalidateRect(g_drawHintHwnd, nullptr, FALSE);
        }
    }

    if (HWND msgWnd = g_msgHwnd.load()) {
        RegisterHotKey(msgWnd, HOTKEY_DRAW_EXIT_ESC, MOD_NOREPEAT, VK_ESCAPE);
    }

    if (g_pillHwnd && IsWindow(g_pillHwnd)) InvalidateRect(g_pillHwnd, nullptr, FALSE);
}

// ============================================================================
// Mouse Cursor Overlay & Fast BGRA Bilinear Downscaler
// ============================================================================

static void DrawCursorOntoBGRA(std::vector<uint8_t>& bgra, int width, int height, const RECT& captureRect) {
    CURSORINFO ci{};
    ci.cbSize = sizeof(ci);
    if (!GetCursorInfo(&ci) || !(ci.flags & CURSOR_SHOWING) || !ci.hCursor) return;

    ICONINFO ii{};
    if (!GetIconInfo(ci.hCursor, &ii)) return;

    int cursorX = static_cast<int>(ci.ptScreenPos.x) - static_cast<int>(ii.xHotspot) - static_cast<int>(captureRect.left);
    int cursorY = static_cast<int>(ci.ptScreenPos.y) - static_cast<int>(ii.yHotspot) - static_cast<int>(captureRect.top);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (cursorX < -64 || cursorY < -64 || cursorX >= width || cursorY >= height) return;

    constexpr int kBox = 64;
    int drawLeft = std::max(0, cursorX), drawTop = std::max(0, cursorY);
    int drawRight = std::min(width, cursorX + kBox), drawBottom = std::min(height, cursorY + kBox);
    int copyW = drawRight - drawLeft, copyH = drawBottom - drawTop;
    if (copyW <= 0 || copyH <= 0) return;

    HDC screenDc = GetDC(nullptr);
    HDC memDc = CreateCompatibleDC(screenDc);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = kBox;
    bmi.bmiHeader.biHeight = -kBox;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* dibBits = nullptr;
    HBITMAP hBmp = CreateDIBSection(memDc, &bmi, DIB_RGB_COLORS, &dibBits, nullptr, 0);
    if (hBmp && dibBits) {
        HGDIOBJ oldBmp = SelectObject(memDc, hBmp);
        auto* patch = static_cast<uint8_t*>(dibBits);
        memset(patch, 0, kBox * kBox * 4);
        int offX = drawLeft - cursorX, offY = drawTop - cursorY;
        for (int y = 0; y < copyH; ++y) {
            memcpy(patch + ((offY + y) * kBox + offX) * 4,
                   bgra.data() + ((drawTop + y) * width + drawLeft) * 4,
                   static_cast<size_t>(copyW) * 4);
        }
        DrawIconEx(memDc, 0, 0, ci.hCursor, 0, 0, 0, nullptr, DI_NORMAL | DI_COMPAT);
        GdiFlush();
        for (int y = 0; y < copyH; ++y) {
            memcpy(bgra.data() + ((drawTop + y) * width + drawLeft) * 4,
                   patch + ((offY + y) * kBox + offX) * 4,
                   static_cast<size_t>(copyW) * 4);
        }
        SelectObject(memDc, oldBmp);
        DeleteObject(hBmp);
    }
    DeleteDC(memDc);
    ReleaseDC(nullptr, screenDc);
}

static void DownscaleBGRA(const uint8_t* src, int srcW, int srcH, uint8_t* dst, int dstW, int dstH) {
    const uint32_t xStep = static_cast<uint32_t>((static_cast<uint64_t>(srcW) << 16) / static_cast<uint32_t>(dstW));
    const uint32_t yStep = static_cast<uint32_t>((static_cast<uint64_t>(srcH) << 16) / static_cast<uint32_t>(dstH));
    for (int y = 0; y < dstH; ++y) {
        uint32_t sy16 = static_cast<uint32_t>(y) * yStep;
        int sy0 = std::min(static_cast<int>(sy16 >> 16), srcH - 1);
        int sy1 = std::min(sy0 + 1, srcH - 1);
        uint32_t fy = (sy16 >> 8) & 0xFF;
        uint32_t ify = 256u - fy;

        const uint8_t* row0 = src + static_cast<size_t>(sy0) * srcW * 4;
        const uint8_t* row1 = src + static_cast<size_t>(sy1) * srcW * 4;
        uint8_t* outRow = dst + static_cast<size_t>(y) * dstW * 4;

        uint32_t sx16 = 0;
        for (int x = 0; x < dstW; ++x) {
            int sx0 = std::min(static_cast<int>(sx16 >> 16), srcW - 1);
            int sx1 = std::min(sx0 + 1, srcW - 1);
            uint32_t fx = (sx16 >> 8) & 0xFF;
            uint32_t ifx = 256u - fx;
            sx16 += xStep;

            const uint8_t* p00 = row0 + sx0 * 4;
            const uint8_t* p10 = row0 + sx1 * 4;
            const uint8_t* p01 = row1 + sx0 * 4;
            const uint8_t* p11 = row1 + sx1 * 4;

            for (int c = 0; c < 3; ++c) {
                uint32_t top = (p00[c] * ifx + p10[c] * fx) >> 8;
                uint32_t bot = (p01[c] * ifx + p11[c] * fx) >> 8;
                outRow[x * 4 + c] = static_cast<uint8_t>((top * ify + bot * fy) >> 8);
            }
            outRow[x * 4 + 3] = 255;
        }
    }
}

// Bilinear-scales a source BGRA image directly into a sub-rectangle of a destination BGRA canvas
static void BlitScaledBGRA(const uint8_t* src, int srcW, int srcH,
                           uint8_t* canvas, int canvasW, int canvasH,
                           int dstX, int dstY, int dstW, int dstH) {
    if (!src || !canvas || srcW <= 0 || srcH <= 0 || dstW <= 0 || dstH <= 0) return;
    const uint32_t xStep = static_cast<uint32_t>((static_cast<uint64_t>(srcW) << 16) / static_cast<uint32_t>(dstW));
    const uint32_t yStep = static_cast<uint32_t>((static_cast<uint64_t>(srcH) << 16) / static_cast<uint32_t>(dstH));

    for (int y = 0; y < dstH; ++y) {
        int cy = dstY + y;
        if (cy < 0 || cy >= canvasH) continue;
        uint32_t sy16 = static_cast<uint32_t>(y) * yStep;
        int sy0 = std::min(static_cast<int>(sy16 >> 16), srcH - 1);
        int sy1 = std::min(sy0 + 1, srcH - 1);
        uint32_t fy = (sy16 >> 8) & 0xFF;
        uint32_t ify = 256u - fy;

        const uint8_t* row0 = src + static_cast<size_t>(sy0) * srcW * 4;
        const uint8_t* row1 = src + static_cast<size_t>(sy1) * srcW * 4;
        uint8_t* outRow = canvas + static_cast<size_t>(cy) * canvasW * 4;

        uint32_t sx16 = 0;
        for (int x = 0; x < dstW; ++x) {
            int cx = dstX + x;
            uint32_t curSx16 = sx16;
            sx16 += xStep;
            if (cx < 0 || cx >= canvasW) continue;

            int sx0 = std::min(static_cast<int>(curSx16 >> 16), srcW - 1);
            int sx1 = std::min(sx0 + 1, srcW - 1);
            uint32_t fx = (curSx16 >> 8) & 0xFF;
            uint32_t ifx = 256u - fx;

            const uint8_t* p00 = row0 + sx0 * 4;
            const uint8_t* p10 = row0 + sx1 * 4;
            const uint8_t* p01 = row1 + sx0 * 4;
            const uint8_t* p11 = row1 + sx1 * 4;

            for (int c = 0; c < 3; ++c) {
                uint32_t top = (p00[c] * ifx + p10[c] * fx) >> 8;
                uint32_t bot = (p01[c] * ifx + p11[c] * fx) >> 8;
                outRow[cx * 4 + c] = static_cast<uint8_t>((top * ify + bot * fy) >> 8);
            }
            outRow[cx * 4 + 3] = 255;
        }
    }
}

// ============================================================================
// Screen Capture Engine (DXGI Desktop Duplication + GDI Fallback + Downscale)
// ============================================================================

class ScreenCapturer {
public:
    ~ScreenCapturer() { Cleanup(); }

    bool InitCustomLayout(const std::vector<MonitorInfoItem>& mons,
                          const std::vector<CustomMonitorSlot>& slots,
                          int targetResHeight) {
        Cleanup();
        if (mons.empty() || slots.empty()) return false;

        int baseW = mons[0].width & ~1;
        int baseH = mons[0].height & ~1;
        for (const auto& m : mons) {
            baseW = std::max(baseW, m.width & ~1);
            baseH = std::max(baseH, m.height & ~1);
        }
        ComputeCappedOutputSize(baseW, baseH, targetResHeight, outWidth_, outHeight_);
        srcWidth_ = outWidth_;
        srcHeight_ = outHeight_;

        customMode_ = true;
        customMons_ = mons;
        customSlots_ = slots;
        workBuffer_.assign(static_cast<size_t>(outWidth_) * outHeight_ * 4, 0);

        screenDc_ = GetDC(nullptr);
        if (!screenDc_) return false;
        memDc_ = CreateCompatibleDC(screenDc_);
        if (!memDc_) { CleanupGdi(); return false; }

        int maxMonW = 64, maxMonH = 64;
        for (const auto& m : mons) {
            maxMonW = std::max(maxMonW, m.width);
            maxMonH = std::max(maxMonH, m.height);
        }
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = maxMonW;
        bmi.bmiHeader.biHeight = -maxMonH;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        dibBmp_ = CreateDIBSection(memDc_, &bmi, DIB_RGB_COLORS, &dibBits_, nullptr, 0);
        if (!dibBmp_ || !dibBits_) { CleanupGdi(); return false; }
        oldBmp_ = SelectObject(memDc_, dibBmp_);
        customScratchStride_ = maxMonW;
        customMonBuf_.resize(static_cast<size_t>(maxMonW) * maxMonH * 4);
        Wh_Log(L"[ScreenRecorder] Custom multi-monitor layout output %dx%d (%zu monitors)", outWidth_, outHeight_, mons.size());
        return true;
    }

    bool Init(const RECT& targetRect, int targetResHeight) {
        Cleanup();
        captureRect_ = targetRect;
        srcWidth_ = static_cast<int>(captureRect_.right - captureRect_.left) & ~1;
        srcHeight_ = static_cast<int>(captureRect_.bottom - captureRect_.top) & ~1;
        captureRect_.right = captureRect_.left + srcWidth_;
        captureRect_.bottom = captureRect_.top + srcHeight_;
        if (srcWidth_ < 64 || srcHeight_ < 64) return false;

        ComputeCappedOutputSize(srcWidth_, srcHeight_, targetResHeight, outWidth_, outHeight_);

        frameBuffer_.assign(static_cast<size_t>(srcWidth_) * srcHeight_ * 4, 0);
        if (outWidth_ != srcWidth_ || outHeight_ != srcHeight_) {
            scaledBuffer_.assign(static_cast<size_t>(outWidth_) * outHeight_ * 4, 0);
        }

        if (TryInitDxgi()) {
            useDxgi_ = true;
            Wh_Log(L"[ScreenRecorder] DXGI %dx%d -> Output %dx%d", srcWidth_, srcHeight_, outWidth_, outHeight_);
            return true;
        }

        Wh_Log(L"[ScreenRecorder] GDI fallback %dx%d -> Output %dx%d", srcWidth_, srcHeight_, outWidth_, outHeight_);
        useDxgi_ = false;
        return InitGdi();
    }

    bool CaptureFrame(bool drawCursor, UINT dxgiTimeoutMs = 16) {
        if (customMode_) {
            return CaptureCustomLayout(drawCursor);
        }
        bool ok = false;
        if (useDxgi_) {
            ok = CaptureDxgi(dxgiTimeoutMs);
            if (!ok) {
                ++dxgiFailCount_;
                if (dxgiFailCount_ >= 15) {
                    CleanupDxgi();
                    useDxgi_ = false;
                    InitGdi();
                    ok = CaptureGdi();
                } else {
                    ok = hasFrame_;
                }
            } else {
                dxgiFailCount_ = 0;
            }
        } else {
            ok = CaptureGdi();
        }

        if (ok) {
            hasFrame_ = true;
            workBuffer_ = frameBuffer_;
            if (drawCursor) {
                DrawCursorOntoBGRA(workBuffer_, srcWidth_, srcHeight_, captureRect_);
            }
            if (outWidth_ != srcWidth_ || outHeight_ != srcHeight_) {
                DownscaleBGRA(workBuffer_.data(), srcWidth_, srcHeight_, scaledBuffer_.data(), outWidth_, outHeight_);
            }
        }
        return ok;
    }

    const uint8_t* GetFrameData() const {
        if (customMode_) return workBuffer_.data();
        return (outWidth_ != srcWidth_ || outHeight_ != srcHeight_) ? scaledBuffer_.data() : workBuffer_.data();
    }
    int GetWidth() const { return outWidth_; }
    int GetHeight() const { return outHeight_; }

private:
    bool TryInitDxgi() {
        CleanupDxgi();
        ComPtr<IDXGIFactory1> factory;
        if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(factory.GetAddressOf())))) return false;

        ComPtr<IDXGIAdapter1> matchedAdapter;
        ComPtr<IDXGIOutput1> matchedOutput1;
        RECT outputRect{};
        for (UINT aIdx = 0;; ++aIdx) {
            ComPtr<IDXGIAdapter1> adapter;
            if (factory->EnumAdapters1(aIdx, adapter.GetAddressOf()) == DXGI_ERROR_NOT_FOUND) break;
            for (UINT oIdx = 0;; ++oIdx) {
                ComPtr<IDXGIOutput> output;
                if (adapter->EnumOutputs(oIdx, output.GetAddressOf()) == DXGI_ERROR_NOT_FOUND) break;
                DXGI_OUTPUT_DESC desc{};
                if (FAILED(output->GetDesc(&desc))) continue;
                RECT r = desc.DesktopCoordinates;
                if (captureRect_.left >= r.left && captureRect_.top >= r.top &&
                    captureRect_.right <= r.right && captureRect_.bottom <= r.bottom) {
                    if (SUCCEEDED(output->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(matchedOutput1.GetAddressOf())))) {
                        matchedAdapter = adapter; outputRect = r; break;
                    }
                }
            }
            if (matchedOutput1) break;
        }
        if (!matchedAdapter || !matchedOutput1) return false;

        D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
        D3D_FEATURE_LEVEL createdLevel{};
        if (FAILED(D3D11CreateDevice(matchedAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                                     levels, static_cast<UINT>(std::size(levels)), D3D11_SDK_VERSION,
                                     device_.GetAddressOf(), &createdLevel, context_.GetAddressOf()))) return false;

        if (FAILED(matchedOutput1->DuplicateOutput(device_.Get(), duplication_.GetAddressOf()))) { CleanupDxgi(); return false; }

        outputOriginX_ = static_cast<int>(outputRect.left);
        outputOriginY_ = static_cast<int>(outputRect.top);

        D3D11_TEXTURE2D_DESC stDesc{};
        stDesc.Width = static_cast<UINT>(srcWidth_);
        stDesc.Height = static_cast<UINT>(srcHeight_);
        stDesc.MipLevels = 1;
        stDesc.ArraySize = 1;
        stDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        stDesc.SampleDesc.Count = 1;
        stDesc.Usage = D3D11_USAGE_STAGING;
        stDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        if (FAILED(device_->CreateTexture2D(&stDesc, nullptr, stagingTex_.GetAddressOf()))) { CleanupDxgi(); return false; }
        return true;
    }

    bool CaptureDxgi(UINT timeoutMs) {
        if (!duplication_ && !TryInitDxgi()) return false;
        DXGI_OUTDUPL_FRAME_INFO frameInfo{};
        ComPtr<IDXGIResource> desktopRes;
        HRESULT hr = duplication_->AcquireNextFrame(timeoutMs, &frameInfo, desktopRes.GetAddressOf());
        if (hr == DXGI_ERROR_WAIT_TIMEOUT) return hasFrame_;
        if (hr == DXGI_ERROR_ACCESS_LOST) { TryInitDxgi(); return false; }
        if (FAILED(hr)) return false;

        bool copied = false;
        ComPtr<ID3D11Texture2D> desktopTex;
        if (SUCCEEDED(desktopRes->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(desktopTex.GetAddressOf())))) {
            D3D11_BOX srcBox{};
            srcBox.left = static_cast<UINT>(captureRect_.left - outputOriginX_);
            srcBox.top = static_cast<UINT>(captureRect_.top - outputOriginY_);
            srcBox.front = 0;
            srcBox.right = srcBox.left + static_cast<UINT>(srcWidth_);
            srcBox.bottom = srcBox.top + static_cast<UINT>(srcHeight_);
            srcBox.back = 1;
            context_->CopySubresourceRegion(stagingTex_.Get(), 0, 0, 0, 0, desktopTex.Get(), 0, &srcBox);

            D3D11_MAPPED_SUBRESOURCE mapped{};
            if (SUCCEEDED(context_->Map(stagingTex_.Get(), 0, D3D11_MAP_READ, 0, &mapped))) {
                const uint8_t* src = static_cast<const uint8_t*>(mapped.pData);
                uint8_t* dst = frameBuffer_.data();
                const size_t rowBytes = static_cast<size_t>(srcWidth_) * 4;
                for (int y = 0; y < srcHeight_; ++y) memcpy(dst + y * rowBytes, src + y * mapped.RowPitch, rowBytes);
                context_->Unmap(stagingTex_.Get(), 0);
                copied = true;
            }
        }
        duplication_->ReleaseFrame();
        return copied;
    }

    bool InitGdi() {
        CleanupGdi();
        screenDc_ = GetDC(nullptr);
        if (!screenDc_) return false;
        memDc_ = CreateCompatibleDC(screenDc_);
        if (!memDc_) { CleanupGdi(); return false; }

        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = srcWidth_;
        bmi.bmiHeader.biHeight = -srcHeight_;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        dibBmp_ = CreateDIBSection(memDc_, &bmi, DIB_RGB_COLORS, &dibBits_, nullptr, 0);
        if (!dibBmp_ || !dibBits_) { CleanupGdi(); return false; }
        oldBmp_ = SelectObject(memDc_, dibBmp_);
        return true;
    }

    bool CaptureGdi() {
        if (!memDc_ || !screenDc_ || !dibBits_) return false;
        if (!BitBlt(memDc_, 0, 0, srcWidth_, srcHeight_, screenDc_, captureRect_.left, captureRect_.top, SRCCOPY | CAPTUREBLT)) return false;
        GdiFlush();
        memcpy(frameBuffer_.data(), dibBits_, static_cast<size_t>(srcWidth_) * srcHeight_ * 4);
        return true;
    }

    void CleanupDxgi() { stagingTex_.Reset(); duplication_.Reset(); context_.Reset(); device_.Reset(); }
    void CleanupGdi() {
        if (memDc_ && oldBmp_) { SelectObject(memDc_, oldBmp_); oldBmp_ = nullptr; }
        if (dibBmp_) { DeleteObject(dibBmp_); dibBmp_ = nullptr; dibBits_ = nullptr; }
        if (memDc_) { DeleteDC(memDc_); memDc_ = nullptr; }
        if (screenDc_) { ReleaseDC(nullptr, screenDc_); screenDc_ = nullptr; }
    }
    bool CaptureCustomLayout(bool drawCursor) {
        if (!memDc_ || !screenDc_ || !dibBits_) return false;
        // Fill matte dark background
        uint32_t bg = 0xFF100C0Au;
        auto* out32 = reinterpret_cast<uint32_t*>(workBuffer_.data());
        std::fill(out32, out32 + static_cast<size_t>(outWidth_) * outHeight_, bg);

        for (const auto& sl : customSlots_) {
            if (!sl.enabled || sl.monitorIndex < 0 || sl.monitorIndex >= static_cast<int>(customMons_.size())) continue;
            const auto& m = customMons_[sl.monitorIndex];
            if (m.width <= 0 || m.height <= 0) continue;

            if (!BitBlt(memDc_, 0, 0, m.width, m.height, screenDc_, m.rc.left, m.rc.top, SRCCOPY | CAPTUREBLT)) continue;
            GdiFlush();

            const auto* srcBits = static_cast<const uint8_t*>(dibBits_);
            const size_t rowBytes = static_cast<size_t>(m.width) * 4;
            const size_t pitchBytes = static_cast<size_t>(customScratchStride_) * 4;
            for (int y = 0; y < m.height; ++y) {
                memcpy(customMonBuf_.data() + y * rowBytes, srcBits + y * pitchBytes, rowBytes);
            }
            if (drawCursor) {
                DrawCursorOntoBGRA(customMonBuf_, m.width, m.height, m.rc);
            }

            int dstX = static_cast<int>(std::round(sl.nx * outWidth_));
            int dstY = static_cast<int>(std::round(sl.ny * outHeight_));
            int dstW = std::max(16, static_cast<int>(std::round(sl.nw * outWidth_)));
            int dstH = std::max(16, static_cast<int>(std::round(sl.nh * outHeight_)));
            BlitScaledBGRA(customMonBuf_.data(), m.width, m.height,
                           workBuffer_.data(), outWidth_, outHeight_,
                           dstX, dstY, dstW, dstH);
        }
        hasFrame_ = true;
        return true;
    }

    void Cleanup() { CleanupDxgi(); CleanupGdi(); hasFrame_ = false; dxgiFailCount_ = 0; customMode_ = false; }

    RECT captureRect_{};
    int srcWidth_ = 0, srcHeight_ = 0;
    int outWidth_ = 0, outHeight_ = 0;
    int outputOriginX_ = 0, outputOriginY_ = 0;
    bool useDxgi_ = false, hasFrame_ = false;
    int dxgiFailCount_ = 0;

    ComPtr<ID3D11Device> device_;
    ComPtr<ID3D11DeviceContext> context_;
    ComPtr<IDXGIOutputDuplication> duplication_;
    ComPtr<ID3D11Texture2D> stagingTex_;

    HDC screenDc_ = nullptr, memDc_ = nullptr;
    HBITMAP dibBmp_ = nullptr;
    HGDIOBJ oldBmp_ = nullptr;
    void* dibBits_ = nullptr;

    std::vector<uint8_t> frameBuffer_;
    std::vector<uint8_t> workBuffer_;
    std::vector<uint8_t> scaledBuffer_;
    bool customMode_ = false;
    std::vector<MonitorInfoItem> customMons_;
    std::vector<CustomMonitorSlot> customSlots_;
    int customScratchStride_ = 0;
    std::vector<uint8_t> customMonBuf_;
};

// ============================================================================
// WASAPI Audio Capture Helper (48kHz Stereo S16)
// ============================================================================

constexpr UINT32 kAudioSampleRate = 48000;
constexpr UINT32 kAudioChannels = 2;

class WasapiSource {
public:
    ~WasapiSource() { Stop(); }

    bool Init(bool isLoopback) {
        Stop();
        ComPtr<IMMDeviceEnumerator> enumerator;
        if (FAILED(CoCreateInstance(kCLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL,
                                    kIID_IMMDeviceEnumerator, reinterpret_cast<void**>(enumerator.GetAddressOf())))) return false;

        ComPtr<IMMDevice> device;
        if (FAILED(enumerator->GetDefaultAudioEndpoint(isLoopback ? eRender : eCapture, eConsole, device.GetAddressOf()))) return false;
        if (FAILED(device->Activate(kIID_IAudioClient, CLSCTX_ALL, nullptr, reinterpret_cast<void**>(audioClient_.GetAddressOf())))) return false;

        WAVEFORMATEX* mixFmt = nullptr;
        if (FAILED(audioClient_->GetMixFormat(&mixFmt)) || !mixFmt) return false;
        srcRate_ = mixFmt->nSamplesPerSec;
        srcChannels_ = mixFmt->nChannels;
        srcBits_ = mixFmt->wBitsPerSample;
        isFloat_ = (mixFmt->wFormatTag == WAVE_FORMAT_IEEE_FLOAT);
        if (!isFloat_ && mixFmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE && mixFmt->cbSize >= 22) {
            auto* ext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(mixFmt);
            isFloat_ = IsEqualGUID(ext->SubFormat, kKSDATAFORMAT_SUBTYPE_IEEE_FLOAT) != FALSE;
        }

        HRESULT hr = audioClient_->Initialize(AUDCLNT_SHAREMODE_SHARED, isLoopback ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0, 1000000, 0, mixFmt, nullptr);
        CoTaskMemFree(mixFmt);
        if (FAILED(hr)) { Stop(); return false; }

        if (FAILED(audioClient_->GetService(kIID_IAudioCaptureClient, reinterpret_cast<void**>(captureClient_.GetAddressOf())))) { Stop(); return false; }
        if (FAILED(audioClient_->Start())) { Stop(); return false; }
        active_ = true;
        return true;
    }

    void Drain(std::vector<int16_t>& dst) {
        if (!active_ || !captureClient_) return;
        UINT32 packetFrames = 0;
        while (SUCCEEDED(captureClient_->GetNextPacketSize(&packetFrames)) && packetFrames > 0) {
            BYTE* data = nullptr; UINT32 numFrames = 0; DWORD flags = 0;
            if (FAILED(captureClient_->GetBuffer(&data, &numFrames, &flags, nullptr, nullptr))) break;
            if (numFrames > 0) ConvertAndAppend(data, numFrames, (flags & AUDCLNT_BUFFERFLAGS_SILENT) != 0, dst);
            captureClient_->ReleaseBuffer(numFrames);
        }
    }

    void Stop() {
        if (audioClient_ && active_) audioClient_->Stop();
        active_ = false;
        captureClient_.Reset();
        audioClient_.Reset();
    }

private:
    static float ReadSampleAsFloat(const BYTE* data, UINT32 frame, UINT32 ch, UINT32 totalCh, bool isFloat, UINT32 bits) {
        size_t idx = static_cast<size_t>(frame) * totalCh + ch;
        if (isFloat && bits == 32) return reinterpret_cast<const float*>(data)[idx];
        if (!isFloat && bits == 16) return static_cast<float>(reinterpret_cast<const int16_t*>(data)[idx]) / 32768.0f;
        if (!isFloat && bits == 32) return static_cast<float>(reinterpret_cast<const int32_t*>(data)[idx]) / 2147483648.0f;
        return 0.0f;
    }

    void ConvertAndAppend(const BYTE* data, UINT32 numFrames, bool silent, std::vector<int16_t>& dst) {
        double ratio = static_cast<double>(kAudioSampleRate) / static_cast<double>(srcRate_);
        UINT32 outFrames = static_cast<UINT32>(std::llround(numFrames * ratio));
        if (outFrames == 0) return;

        size_t baseIdx = dst.size();
        dst.resize(baseIdx + static_cast<size_t>(outFrames) * kAudioChannels, 0);
        if (silent || !data) return;

        for (UINT32 i = 0; i < outFrames; ++i) {
            double srcPos = static_cast<double>(i) / ratio;
            UINT32 f0 = std::min(static_cast<UINT32>(srcPos), numFrames - 1);
            UINT32 f1 = std::min(f0 + 1, numFrames - 1);
            float frac = static_cast<float>(srcPos - f0);
            auto sampleCh = [&](UINT32 ch) -> float {
                UINT32 c = std::min(ch, srcChannels_ - 1);
                return ReadSampleAsFloat(data, f0, c, srcChannels_, isFloat_, srcBits_) * (1.0f - frac)
                     + ReadSampleAsFloat(data, f1, c, srcChannels_, isFloat_, srcBits_) * frac;
            };
            float left = sampleCh(0), right = (srcChannels_ >= 2) ? sampleCh(1) : left;
            dst[baseIdx + i * 2]     = static_cast<int16_t>(std::lrintf(std::clamp(left, -1.0f, 1.0f) * 32767.0f));
            dst[baseIdx + i * 2 + 1] = static_cast<int16_t>(std::lrintf(std::clamp(right, -1.0f, 1.0f) * 32767.0f));
        }
    }

    ComPtr<IAudioClient> audioClient_;
    ComPtr<IAudioCaptureClient> captureClient_;
    bool active_ = false, isFloat_ = true;
    UINT32 srcRate_ = 48000, srcChannels_ = 2, srcBits_ = 32;
};

// ============================================================================
// Media Foundation MP4 Recorder Session
// ============================================================================

static bool ConfigureSinkWriter(const std::wstring& outputPath, int width, int height,
                                int fps, int bitrateMbps, bool enableAudio,
                                ComPtr<IMFSinkWriter>& outWriter,
                                DWORD& outVideoStream, DWORD& outAudioStream) {
    ComPtr<IMFAttributes> attrs;
    if (SUCCEEDED(MFCreateAttributes(attrs.GetAddressOf(), 2))) {
        attrs->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
        attrs->SetUINT32(MF_LOW_LATENCY, TRUE);
    }

    if (FAILED(MFCreateSinkWriterFromURL(outputPath.c_str(), nullptr, attrs.Get(), outWriter.GetAddressOf()))) return false;

    ComPtr<IMFMediaType> videoOut;
    if (FAILED(MFCreateMediaType(videoOut.GetAddressOf()))) return false;
    videoOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    videoOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
    videoOut->SetUINT32(MF_MT_AVG_BITRATE, static_cast<UINT32>(bitrateMbps) * 1000000u);
    videoOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    MFSetAttributeSize(videoOut.Get(), MF_MT_FRAME_SIZE, static_cast<UINT32>(width), static_cast<UINT32>(height));
    MFSetAttributeRatio(videoOut.Get(), MF_MT_FRAME_RATE, static_cast<UINT32>(fps), 1);
    MFSetAttributeRatio(videoOut.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    if (FAILED(outWriter->AddStream(videoOut.Get(), &outVideoStream))) return false;

    ComPtr<IMFMediaType> videoIn;
    if (FAILED(MFCreateMediaType(videoIn.GetAddressOf()))) return false;
    videoIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    videoIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    videoIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    videoIn->SetUINT32(MF_MT_DEFAULT_STRIDE, static_cast<UINT32>(width * 4));
    MFSetAttributeSize(videoIn.Get(), MF_MT_FRAME_SIZE, static_cast<UINT32>(width), static_cast<UINT32>(height));
    MFSetAttributeRatio(videoIn.Get(), MF_MT_FRAME_RATE, static_cast<UINT32>(fps), 1);
    MFSetAttributeRatio(videoIn.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    if (FAILED(outWriter->SetInputMediaType(outVideoStream, videoIn.Get(), nullptr))) return false;

    outAudioStream = static_cast<DWORD>(-1);
    if (enableAudio) {
        ComPtr<IMFMediaType> audioOut;
        if (SUCCEEDED(MFCreateMediaType(audioOut.GetAddressOf()))) {
            audioOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
            audioOut->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
            audioOut->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
            audioOut->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, kAudioSampleRate);
            audioOut->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, kAudioChannels);
            audioOut->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 24000);
            audioOut->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, 0);
            if (SUCCEEDED(outWriter->AddStream(audioOut.Get(), &outAudioStream))) {
                ComPtr<IMFMediaType> audioIn;
                if (SUCCEEDED(MFCreateMediaType(audioIn.GetAddressOf()))) {
                    audioIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
                    audioIn->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
                    audioIn->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
                    audioIn->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, kAudioSampleRate);
                    audioIn->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, kAudioChannels);
                    audioIn->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, kAudioChannels * 2);
                    audioIn->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, kAudioSampleRate * kAudioChannels * 2);
                    if (FAILED(outWriter->SetInputMediaType(outAudioStream, audioIn.Get(), nullptr))) {
                        outAudioStream = static_cast<DWORD>(-1);
                    }
                }
            }
        }
    }

    return SUCCEEDED(outWriter->BeginWriting());
}

static void RecordingWorkerProc(RECT targetRect, bool useCustomLayout) {
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool mfStarted = SUCCEEDED(MFStartup(MF_VERSION));
    ModSettings cfg = GetSettingsCopy();
    std::wstring outputPath = BuildOutputFilePath(cfg.outputFolder);

    auto notifyError = [&]() {
        g_isRecording = false;
        g_isPaused = false;
        HWND msgWnd = g_msgHwnd.load();
        if (msgWnd) PostMessageW(msgWnd, WM_APP_RECORDING_ERROR, 0, 0);
    };

    if (!mfStarted) { if (SUCCEEDED(comHr)) CoUninitialize(); notifyError(); return; }

    ScreenCapturer capturer;
    bool capOk = false;
    if (useCustomLayout) {
        auto mons = EnumerateConnectedMonitors();
        std::vector<CustomMonitorSlot> slotsCopy;
        {
            std::lock_guard<std::mutex> lock(g_customLayoutMutex);
            EnsureDefaultCustomLayout(g_customLayout, mons);
            slotsCopy = g_customLayout;
        }
        capOk = capturer.InitCustomLayout(mons, slotsCopy, cfg.targetResHeight);
    } else {
        capOk = capturer.Init(targetRect, cfg.targetResHeight);
    }
    if (!capOk) {
        MFShutdown();
        if (SUCCEEDED(comHr)) CoUninitialize();
        notifyError();
        return;
    }

    WasapiSource sysAudio, micAudio;
    bool hasSysAudio = cfg.recordSystemAudio && sysAudio.Init(true);
    bool hasMicAudio = cfg.recordMicrophone && micAudio.Init(false);

    // Cap FPS to monitor hardware refresh rate, and gracefully step down if GPU H.264 encoder rejects high FPS
    int effectiveFps = ResolveEffectiveFps(cfg.fps, cfg.captureMonitorMode);
    ComPtr<IMFSinkWriter> sinkWriter;
    DWORD videoStream = 0, audioStream = static_cast<DWORD>(-1);
    bool writerReady = ConfigureSinkWriter(outputPath, capturer.GetWidth(), capturer.GetHeight(),
                                           effectiveFps, cfg.videoBitrateMbps, hasSysAudio || hasMicAudio,
                                           sinkWriter, videoStream, audioStream);
    if (!writerReady && effectiveFps > 60) {
        static constexpr int kFallbackFps[] = { 240, 165, 144, 120, 90, 75, 60 };
        for (int fb : kFallbackFps) {
            if (fb >= effectiveFps) continue;
            sinkWriter.Reset();
            Wh_Log(L"[ScreenRecorder] Hardware encoder rejected %d FPS; trying %d FPS fallback", effectiveFps, fb);
            if (ConfigureSinkWriter(outputPath, capturer.GetWidth(), capturer.GetHeight(),
                                    fb, cfg.videoBitrateMbps, hasSysAudio || hasMicAudio,
                                    sinkWriter, videoStream, audioStream)) {
                effectiveFps = fb;
                writerReady = true;
                break;
            }
        }
    }
    if (!writerReady) {
        sysAudio.Stop(); micAudio.Stop();
        MFShutdown();
        if (SUCCEEDED(comHr)) CoUninitialize();
        notifyError();
        return;
    }
    Wh_Log(L"[ScreenRecorder] Recording %dx%d @ %d FPS (%d Mbps)", capturer.GetWidth(), capturer.GetHeight(), effectiveFps, cfg.videoBitrateMbps);

    g_elapsedMs = 0;
    g_isPaused = false;
    g_isRecording = true;

    HWND msgWnd = g_msgHwnd.load();
    if (msgWnd) PostMessageW(msgWnd, WM_APP_RECORDING_STARTED, 0, 0);

    timeBeginPeriod(1);
    LARGE_INTEGER qpcFreq{}, qpcStart{}, qpcPauseStart{};
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);
    int64_t pausedTicksTotal = 0;
    bool wasPaused = false;

    const LONGLONG frameDuration100ns = 10000000LL / effectiveFps;
    const DWORD frameBytes = static_cast<DWORD>(capturer.GetWidth()) * capturer.GetHeight() * 4;
    const auto frameIntervalNs = std::chrono::nanoseconds(1000000000LL / effectiveFps);
    const UINT dxgiTimeoutMs = static_cast<UINT>(std::max(1, (1000 / effectiveFps) / 2));

    int64_t audioFramesWritten = 0;
    std::vector<int16_t> sysBuf, micBuf, mixedBuf;
    LONGLONG lastVideoPts = -1;
    auto nextFrameTime = std::chrono::steady_clock::now();

    while (!g_stopRequested.load()) {
        if (g_isPaused.load()) {
            if (!wasPaused) { QueryPerformanceCounter(&qpcPauseStart); wasPaused = true; }
            sysBuf.clear(); micBuf.clear();
            if (hasSysAudio) sysAudio.Drain(sysBuf);
            if (hasMicAudio) micAudio.Drain(micBuf);
            Sleep(25);
            nextFrameTime = std::chrono::steady_clock::now();
            continue;
        } else if (wasPaused) {
            LARGE_INTEGER qpcNow{};
            QueryPerformanceCounter(&qpcNow);
            pausedTicksTotal += (qpcNow.QuadPart - qpcPauseStart.QuadPart);
            wasPaused = false;
        }

        LARGE_INTEGER qpcNow{};
        QueryPerformanceCounter(&qpcNow);
        int64_t activeTicks = (qpcNow.QuadPart - qpcStart.QuadPart) - pausedTicksTotal;
        LONGLONG elapsed100ns = static_cast<LONGLONG>(
            (static_cast<double>(activeTicks) * 10000000.0) / static_cast<double>(qpcFreq.QuadPart));
        if (elapsed100ns <= lastVideoPts) elapsed100ns = lastVideoPts + 1;
        g_elapsedMs = elapsed100ns / 10000;

        if (capturer.CaptureFrame(cfg.recordCursor, dxgiTimeoutMs)) {
            ComPtr<IMFMediaBuffer> buf;
            if (SUCCEEDED(MFCreateMemoryBuffer(frameBytes, buf.GetAddressOf()))) {
                BYTE* dst = nullptr;
                if (SUCCEEDED(buf->Lock(&dst, nullptr, nullptr))) {
                    memcpy(dst, capturer.GetFrameData(), frameBytes);
                    buf->Unlock();
                    buf->SetCurrentLength(frameBytes);
                    ComPtr<IMFSample> sample;
                    if (SUCCEEDED(MFCreateSample(sample.GetAddressOf()))) {
                        sample->AddBuffer(buf.Get());
                        sample->SetSampleTime(elapsed100ns);
                        sample->SetSampleDuration(frameDuration100ns);
                        sinkWriter->WriteSample(videoStream, sample.Get());
                        lastVideoPts = elapsed100ns;
                    }
                }
            }
        }

        if (audioStream != static_cast<DWORD>(-1)) {
            sysBuf.clear(); micBuf.clear();
            if (hasSysAudio) sysAudio.Drain(sysBuf);
            if (hasMicAudio) micAudio.Drain(micBuf);

            int64_t targetAudioFrames = (elapsed100ns * kAudioSampleRate) / 10000000LL;
            int64_t neededFrames = targetAudioFrames - audioFramesWritten;
            size_t availFrames = std::max(sysBuf.size(), micBuf.size()) / kAudioChannels;
            size_t emitFrames = std::max(availFrames, neededFrames > 0 ? static_cast<size_t>(neededFrames) : static_cast<size_t>(0));

            if (emitFrames > 0) {
                mixedBuf.assign(emitFrames * kAudioChannels, 0);
                for (size_t i = 0; i < emitFrames * kAudioChannels; ++i) {
                    int32_t sum = 0;
                    if (i < sysBuf.size()) sum += sysBuf[i];
                    if (i < micBuf.size()) sum += micBuf[i];
                    mixedBuf[i] = static_cast<int16_t>(std::clamp(sum, -32768, 32767));
                }
                DWORD audioBytes = static_cast<DWORD>(mixedBuf.size() * sizeof(int16_t));
                ComPtr<IMFMediaBuffer> abuf;
                if (SUCCEEDED(MFCreateMemoryBuffer(audioBytes, abuf.GetAddressOf()))) {
                    BYTE* dst = nullptr;
                    if (SUCCEEDED(abuf->Lock(&dst, nullptr, nullptr))) {
                        memcpy(dst, mixedBuf.data(), audioBytes);
                        abuf->Unlock();
                        abuf->SetCurrentLength(audioBytes);
                        ComPtr<IMFSample> asample;
                        if (SUCCEEDED(MFCreateSample(asample.GetAddressOf()))) {
                            asample->AddBuffer(abuf.Get());
                            asample->SetSampleTime((audioFramesWritten * 10000000LL) / kAudioSampleRate);
                            asample->SetSampleDuration((static_cast<LONGLONG>(emitFrames) * 10000000LL) / kAudioSampleRate);
                            sinkWriter->WriteSample(audioStream, asample.Get());
                            audioFramesWritten += static_cast<int64_t>(emitFrames);
                        }
                    }
                }
            }
        }

        nextFrameTime += frameIntervalNs;
        auto now = std::chrono::steady_clock::now();
        if (nextFrameTime > now) std::this_thread::sleep_until(nextFrameTime);
        else if (now - nextFrameTime > std::chrono::milliseconds(100)) nextFrameTime = now;
    }

    timeEndPeriod(1);
    sysAudio.Stop();
    micAudio.Stop();

    HRESULT finHr = sinkWriter->Finalize();
    Wh_Log(L"[ScreenRecorder] Finalized '%s' (hr=0x%08X)", outputPath.c_str(), finHr);
    sinkWriter.Reset();

    MFShutdown();
    if (SUCCEEDED(comHr)) CoUninitialize();

    g_isRecording = false;
    g_isPaused = false;
    msgWnd = g_msgHwnd.load();
    if (msgWnd) PostMessageW(msgWnd, WM_APP_RECORDING_STOPPED, 0, 0);

    if (SUCCEEDED(finHr) && cfg.openFolderOnFinish) RevealFileInExplorer(outputPath);
}

static void StopRecordingSync() {
    std::lock_guard<std::mutex> lock(g_recordLifecycleMutex);
    if (g_isRecording.load() || g_recordThread.joinable()) {
        g_stopRequested = true;
        if (g_recordThread.joinable()) g_recordThread.join();
        g_stopRequested = false;
    }
}

static void StartRecordingRect(const RECT& rect, bool useCustomLayout = false) {
    std::lock_guard<std::mutex> lock(g_recordLifecycleMutex);
    if (g_isRecording.load()) return;
    if (g_recordThread.joinable()) g_recordThread.join();
    g_stopRequested = false;
    g_recordThread = std::thread(RecordingWorkerProc, rect, useCustomLayout);
}

// ============================================================================
// Synchronized Liquid Glass / Fluent Popup Menu
// ============================================================================

struct MenuItem {
    UINT id = 0;
    std::wstring label;
    bool isToggle = false;
    bool checked = false;
    bool isSeparator = false;
    std::vector<MenuItem> subItems;
};

struct MenuPanelState {
    HWND hWnd = nullptr;
    std::vector<MenuItem> items;
    bool isSubMenu = false;
    int hoveredIdx = -1, pressedIdx = -1, openSubIdx = -1;
    int width = 240, height = 100;
    int contentHeight = 100, scrollOffset = 0, maxScroll = 0;
    struct MenuSession* session = nullptr;
};

struct MenuSession {
    UINT dpi = 96;
    SurfaceTheme surface = SurfaceTheme::Glass;
    int glassOpacity = -80, blurOpacity = 255;
    bool isLight = false, anchoredTop = false;
    MenuPanelState mainPanel, subPanel;
    UINT selectedCmd = 0;
    bool running = true;
    int pendingSubIdx = -1;
};

constexpr UINT_PTR kTimerMenuSub = 9501;

static void ComputeMenuPanelSize(MenuSession* s, MenuPanelState* p) {
    int rowH = DpiScale(34, s->dpi), sepH = DpiScale(9, s->dpi), padY = DpiScale(8, s->dpi);
    float maxTextW = DpiScaleF(140.0f, s->dpi);
    if (g_pDWriteFactory) {
        IDWriteTextFormat* pFmt = nullptr;
        if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
                GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.0f, s->dpi), L"", &pFmt)) && pFmt) {
            for (const auto& it : p->items) {
                if (it.isSeparator || it.label.empty()) continue;
                IDWriteTextLayout* pLayout = nullptr;
                if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                        it.label.c_str(), static_cast<UINT32>(it.label.size()), pFmt, 600.0f, static_cast<float>(rowH), &pLayout)) && pLayout) {
                    DWRITE_TEXT_METRICS tm{};
                    if (SUCCEEDED(pLayout->GetMetrics(&tm))) maxTextW = std::max(maxTextW, tm.widthIncludingTrailingWhitespace);
                    pLayout->Release();
                }
            }
            pFmt->Release();
        }
    }
    bool hasToggle = false;
    for (const auto& it : p->items) if (it.isToggle) { hasToggle = true; break; }
    p->width = std::clamp(DpiScale(24, s->dpi) + static_cast<int>(std::ceil(maxTextW)) + DpiScale(hasToggle ? 62 : 40, s->dpi),
                          DpiScale(216, s->dpi), DpiScale(380, s->dpi));
    int h = padY * 2;
    for (const auto& it : p->items) h += it.isSeparator ? sepH : rowH;
    p->contentHeight = h;
    int maxVisibleH = padY * 2 + rowH * 12;
    p->height = std::min(h, maxVisibleH);
    p->maxScroll = std::max(0, p->contentHeight - p->height);
    p->scrollOffset = std::clamp(p->scrollOffset, 0, p->maxScroll);
}

static int HitTestMenuRow(const MenuSession* s, const MenuPanelState* p, POINT pt) {
    int padY = DpiScale(8, s->dpi);
    if (pt.x < 0 || pt.x >= p->width || pt.y < padY || pt.y >= p->height - padY) return -1;
    int targetY = pt.y + p->scrollOffset;
    int curY = padY, rowH = DpiScale(34, s->dpi), sepH = DpiScale(9, s->dpi);
    for (int i = 0; i < static_cast<int>(p->items.size()); ++i) {
        int h = p->items[i].isSeparator ? sepH : rowH;
        if (targetY >= curY && targetY < curY + h) return p->items[i].isSeparator ? -1 : i;
        curY += h;
    }
    return -1;
}

static int GetMenuRowTopY(const MenuSession* s, const MenuPanelState* p, int idx) {
    int curY = DpiScale(8, s->dpi), rowH = DpiScale(34, s->dpi), sepH = DpiScale(9, s->dpi);
    for (int i = 0; i < idx && i < static_cast<int>(p->items.size()); ++i) curY += p->items[i].isSeparator ? sepH : rowH;
    return curY - p->scrollOffset;
}

static void ShowSubMenu(MenuSession* s, int parentIdx) {
    if (!s) return;
    if (parentIdx < 0 || parentIdx >= static_cast<int>(s->mainPanel.items.size()) || s->mainPanel.items[parentIdx].subItems.empty()) {
        s->mainPanel.openSubIdx = -1;
        if (s->subPanel.hWnd && IsWindowVisible(s->subPanel.hWnd)) {
            ShowWindow(s->subPanel.hWnd, SW_HIDE);
            InvalidateRect(s->mainPanel.hWnd, nullptr, FALSE);
        }
        return;
    }
    if (s->mainPanel.openSubIdx == parentIdx && s->subPanel.hWnd && IsWindowVisible(s->subPanel.hWnd)) return;

    s->mainPanel.openSubIdx = parentIdx;
    s->subPanel.items = s->mainPanel.items[parentIdx].subItems;
    s->subPanel.hoveredIdx = -1; s->subPanel.pressedIdx = -1; s->subPanel.scrollOffset = 0;
    ComputeMenuPanelSize(s, &s->subPanel);

    RECT rcMain{};
    GetWindowRect(s->mainPanel.hWnd, &rcMain);
    HMONITOR hMon = MonitorFromWindow(s->mainPanel.hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    RECT rcWork{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    if (hMon && GetMonitorInfoW(hMon, &mi)) rcWork = mi.rcWork;

    int margin = DpiScale(8, s->dpi);
    int subX = static_cast<int>(rcMain.right) - DpiScale(2, s->dpi);
    if (subX + s->subPanel.width > rcWork.right - margin) subX = static_cast<int>(rcMain.left) - s->subPanel.width + DpiScale(2, s->dpi);
    int subY = static_cast<int>(rcMain.top) + GetMenuRowTopY(s, &s->mainPanel, parentIdx) - DpiScale(8, s->dpi);
    subY = std::clamp(subY, static_cast<int>(rcWork.top) + margin,
                      std::max(static_cast<int>(rcWork.top) + margin, static_cast<int>(rcWork.bottom) - s->subPanel.height - margin));

    SetWindowPos(s->subPanel.hWnd, HWND_TOPMOST, subX, subY, s->subPanel.width, s->subPanel.height, SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
    ApplyBorderlessPerPixelDwm(s->subPanel.hWnd, s->isLight, true);
    InvalidateRect(s->subPanel.hWnd, nullptr, FALSE);
    InvalidateRect(s->mainPanel.hWnd, nullptr, FALSE);
}

static void PaintMenuPanel(HWND hWnd, MenuPanelState* p) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);
    if (!p || !p->session || !g_pD2DFactory || !g_pDWriteFactory) { EndPaint(hWnd, &ps); return; }

    MenuSession* s = p->session;
    RECT rc{};
    GetClientRect(hWnd, &rc);
    int w = rc.right, h = rc.bottom;
    UINT dpi = s->dpi;
    bool isLight = s->isLight;

    if (w > 0 && h > 0) {
        HDC memDc = CreateCompatibleDC(hdc);
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;

        DWORD* pixels = nullptr;
        HBITMAP hBmp = CreateDIBSection(memDc, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels), nullptr, 0);
        if (hBmp && pixels) {
            HGDIOBJ oldBmp = SelectObject(memDc, hBmp);
            D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
            ID2D1DCRenderTarget* pRT = nullptr;
            if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&rtProps, &pRT)) && pRT) {
                pRT->BindDC(memDc, &rc);
                pRT->BeginDraw();
                pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                ApplyFluentTextRendering(pRT);
                pRT->Clear(D2D1::ColorF(0, 0, 0, 0));

                float rimInset = DpiScaleF(0.75f, dpi);
                D2D1_RECT_F rcCard = D2D1::RectF(rimInset, rimInset, w - rimInset, h - rimInset);
                float cornerR = std::min(DpiScaleF(20.0f, dpi), (rcCard.bottom - rcCard.top) * 0.5f);
                DrawPillGlassBackdrop(pRT, hWnd, w, h, cornerR, dpi, s->surface, s->glassOpacity, s->blurOpacity, isLight);

                ID2D1SolidColorBrush *pBrText = nullptr, *pBrSub = nullptr, *pBrAccent = nullptr;
                pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0.08f, 0.09f, 0.12f, 1.0f) : D2D1::ColorF(0.96f, 0.97f, 0.99f, 1.0f), &pBrText);
                pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0.22f, 0.25f, 0.32f, 0.72f) : D2D1::ColorF(0.82f, 0.86f, 0.94f, 0.72f), &pBrSub);
                pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0.07f, 0.43f, 0.80f, 1.0f) : D2D1::ColorF(0.38f, 0.74f, 1.00f, 1.0f), &pBrAccent);

                ID2D1StrokeStyle* pStroke = nullptr;
                D2D1_STROKE_STYLE_PROPERTIES sp = D2D1::StrokeStyleProperties(
                    D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND);
                g_pD2DFactory->CreateStrokeStyle(sp, nullptr, 0, &pStroke);

                IDWriteTextFormat* pFmt = nullptr;
                g_pDWriteFactory->CreateTextFormat(GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                                   DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.0f, dpi), L"", &pFmt);
                if (pFmt) { pFmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); pFmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP); }

                float padY = DpiScaleF(8.0f, dpi);
                float curY = padY - static_cast<float>(p->scrollOffset);
                float rowH = DpiScaleF(34.0f, dpi), sepH = DpiScaleF(9.0f, dpi), padX = DpiScaleF(8.0f, dpi);
                float scrollPadR = (p->maxScroll > 0) ? DpiScaleF(7.0f, dpi) : 0.0f;
                pRT->PushAxisAlignedClip(D2D1::RectF(0.0f, padY - 1.0f, static_cast<float>(w), static_cast<float>(h) - padY + 1.0f),
                                         D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                for (int i = 0; i < static_cast<int>(p->items.size()); ++i) {
                    const auto& it = p->items[i];
                    float itemH = it.isSeparator ? sepH : rowH;
                    if (curY + itemH < padY || curY > static_cast<float>(h) - padY) {
                        curY += itemH;
                        continue;
                    }
                    if (it.isSeparator) {
                        ID2D1SolidColorBrush* pDiv = nullptr;
                        if (SUCCEEDED(pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0, 0, 0, 0.09f) : D2D1::ColorF(1, 1, 1, 0.11f), &pDiv))) {
                            float ly = curY + sepH * 0.5f;
                            pRT->DrawLine(D2D1::Point2F(DpiScaleF(14.0f, dpi), ly), D2D1::Point2F(w - DpiScaleF(14.0f, dpi) - scrollPadR, ly), pDiv, DpiScaleF(1.0f, dpi));
                            pDiv->Release();
                        }
                        curY += sepH;
                        continue;
                    }
                    bool hov = (i == p->hoveredIdx || i == p->openSubIdx), prs = (i == p->pressedIdx);
                    if (hov || prs) {
                        D2D1_RECT_F rcPill = D2D1::RectF(padX, curY + DpiScaleF(2.0f, dpi), w - padX - scrollPadR, curY + rowH - DpiScaleF(2.0f, dpi));
                        D2D1_COLOR_F tint = isLight ? D2D1::ColorF(0, 0, 0, prs ? 0.14f : 0.07f) : D2D1::ColorF(1, 1, 1, prs ? 0.18f : 0.10f);
                        DrawD2DGlassButton(pRT, rcPill, (rcPill.bottom - rcPill.top) * 0.5f, dpi, tint, hov, prs, isLight);
                    }
                    float cy = curY + rowH * 0.5f;
                    float rightRes = (it.isToggle ? DpiScaleF(54.0f, dpi) : DpiScaleF(28.0f, dpi)) + scrollPadR;
                    if (pFmt && pBrText) {
                        pRT->DrawText(it.label.c_str(), static_cast<UINT32>(it.label.size()), pFmt,
                                      D2D1::RectF(DpiScaleF(16.0f, dpi), curY, w - rightRes, curY + rowH), pBrText, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    }
                    if (!it.subItems.empty() && pBrSub && pStroke) DrawIconChevronRight(pRT, pBrSub, pStroke, w - DpiScaleF(16.0f, dpi) - scrollPadR, cy, dpi);
                    else if (it.isToggle) DrawD2DLiquidToggleSwitch(pRT, w - DpiScaleF(13.0f, dpi) - scrollPadR, cy, dpi, it.checked, isLight, hov);
                    else if (it.checked && pStroke && pBrAccent) DrawIconCheckmark(pRT, pBrAccent, pStroke, w - DpiScaleF(18.0f, dpi) - scrollPadR, cy, dpi);
                    curY += rowH;
                }
                pRT->PopAxisAlignedClip();

                if (p->maxScroll > 0 && p->contentHeight > 0) {
                    float trackTop = padY + DpiScaleF(4.0f, dpi);
                    float trackH = static_cast<float>(h) - padY * 2.0f - DpiScaleF(8.0f, dpi);
                    float thumbH = std::max(DpiScaleF(24.0f, dpi), trackH * (static_cast<float>(p->height) / static_cast<float>(p->contentHeight)));
                    float ratio = static_cast<float>(p->scrollOffset) / static_cast<float>(p->maxScroll);
                    float thumbY = trackTop + ratio * (trackH - thumbH);
                    float sbW = DpiScaleF(3.5f, dpi);
                    float sbX = static_cast<float>(w) - DpiScaleF(7.5f, dpi);
                    ID2D1SolidColorBrush* pBrSb = nullptr;
                    if (SUCCEEDED(pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0, 0, 0, 0.28f) : D2D1::ColorF(1, 1, 1, 0.34f), &pBrSb))) {
                        pRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(sbX, thumbY, sbX + sbW, thumbY + thumbH), sbW * 0.5f, sbW * 0.5f), pBrSb);
                        pBrSb->Release();
                    }
                }

                DrawD2DGlassRim(pRT, rcCard, cornerR, dpi, isLight, 1.3f);
                pRT->EndDraw();

                if (pFmt) pFmt->Release();
                if (pStroke) pStroke->Release();
                if (pBrAccent) pBrAccent->Release();
                if (pBrSub) pBrSub->Release();
                if (pBrText) pBrText->Release();
                pRT->Release();
            }
            BitBlt(hdc, 0, 0, w, h, memDc, 0, 0, SRCCOPY);
            SelectObject(memDc, oldBmp);
            DeleteObject(hBmp);
        }
        DeleteDC(memDc);
    }
    EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK MenuWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto* p = reinterpret_cast<MenuPanelState*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    switch (uMsg) {
        case WM_NCCREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: PaintMenuPanel(hWnd, p); return 0;
        case WM_MOUSEMOVE: {
            if (!p || !p->session) return 0;
            POINT pt{static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
            int row = HitTestMenuRow(p->session, p, pt);
            if (row != p->hoveredIdx) {
                p->hoveredIdx = row;
                if (!p->isSubMenu) { p->session->pendingSubIdx = row; SetTimer(hWnd, kTimerMenuSub, 60, nullptr); }
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hWnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_TIMER:
            if (wParam == kTimerMenuSub && p && p->session && !p->isSubMenu) {
                KillTimer(hWnd, kTimerMenuSub);
                if (p->session->subPanel.hWnd && IsWindowVisible(p->session->subPanel.hWnd)) {
                    POINT cur{}; GetCursorPos(&cur);
                    RECT rcSub{}; GetWindowRect(p->session->subPanel.hWnd, &rcSub);
                    if (PtInRect(&rcSub, cur)) { p->hoveredIdx = p->openSubIdx; InvalidateRect(hWnd, nullptr, FALSE); return 0; }
                }
                ShowSubMenu(p->session, p->session->pendingSubIdx);
                return 0;
            }
            break;
        case WM_MOUSELEAVE: {
            if (!p || !p->session) return 0;
            p->pressedIdx = -1;
            if (p->isSubMenu) p->hoveredIdx = -1;
            else {
                POINT cur{}; GetCursorPos(&cur);
                RECT rcSub{};
                if (p->session->subPanel.hWnd && IsWindowVisible(p->session->subPanel.hWnd) &&
                    GetWindowRect(p->session->subPanel.hWnd, &rcSub) && PtInRect(&rcSub, cur)) p->hoveredIdx = p->openSubIdx;
                else p->hoveredIdx = -1;
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (!p || !p->session) return 0;
            POINT pt{static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
            int row = HitTestMenuRow(p->session, p, pt);
            p->pressedIdx = row;
            if (row >= 0 && !p->isSubMenu && !p->items[row].subItems.empty()) ShowSubMenu(p->session, row);
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        case WM_LBUTTONUP: {
            if (!p || !p->session) return 0;
            POINT pt{static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
            int row = HitTestMenuRow(p->session, p, pt);
            p->pressedIdx = -1;
            if (row >= 0 && row < static_cast<int>(p->items.size())) {
                const auto& it = p->items[row];
                if (!it.subItems.empty()) ShowSubMenu(p->session, row);
                else if (it.id != 0) { p->session->selectedCmd = it.id; p->session->running = false; }
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            if (!p || !p->session || p->maxScroll <= 0) return 0;
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            int step = DpiScale(68, p->session->dpi);
            int nextOff = std::clamp(p->scrollOffset - (delta > 0 ? step : -step), 0, p->maxScroll);
            if (nextOff != p->scrollOffset) {
                p->scrollOffset = nextOff;
                POINT cur{}; GetCursorPos(&cur); ScreenToClient(hWnd, &cur);
                p->hoveredIdx = HitTestMenuRow(p->session, p, cur);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_CLOSE:
            if (p && p->session) p->session->running = false;
            return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static UINT RunPillPopupMenu(HWND hOwner, UINT dpi, const ModSettings& cfg, bool isLight, bool anchoredTop,
                             std::vector<MenuItem>&& items, POINT ptAnchorRight) {
    MenuSession s;
    s.dpi = dpi; s.surface = cfg.surfaceTheme; s.glassOpacity = cfg.glassOpacity;
    s.blurOpacity = cfg.blurOpacity; s.isLight = isLight; s.anchoredTop = anchoredTop;
    s.mainPanel.items = std::move(items); s.mainPanel.isSubMenu = false; s.mainPanel.session = &s;
    s.subPanel.isSubMenu = true; s.subPanel.session = &s;
    ComputeMenuPanelSize(&s, &s.mainPanel);

    HMONITOR hMon = MonitorFromWindow(hOwner, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    RECT rcWork{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    if (hMon && GetMonitorInfoW(hMon, &mi)) rcWork = mi.rcWork;

    int margin = DpiScale(8, dpi);
    int mainX = std::clamp(static_cast<int>(ptAnchorRight.x) - s.mainPanel.width,
                           static_cast<int>(rcWork.left) + margin,
                           std::max(static_cast<int>(rcWork.left) + margin, static_cast<int>(rcWork.right) - s.mainPanel.width - margin));
    int mainY = anchoredTop
        ? (static_cast<int>(ptAnchorRight.y) + DpiScale(8, dpi))
        : (static_cast<int>(ptAnchorRight.y) - s.mainPanel.height - DpiScale(8, dpi));
    mainY = std::clamp(mainY, static_cast<int>(rcWork.top) + margin,
                       std::max(static_cast<int>(rcWork.top) + margin, static_cast<int>(rcWork.bottom) - s.mainPanel.height - margin));

    HWND hMain = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kMenuWndClass, L"", WS_POPUP,
                                 mainX, mainY, s.mainPanel.width, s.mainPanel.height, hOwner, nullptr, GetModInstanceHandle(), &s.mainPanel);
    HWND hSub = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kMenuWndClass, L"", WS_POPUP,
                                0, 0, 10, 10, hOwner, nullptr, GetModInstanceHandle(), &s.subPanel);

    s.mainPanel.hWnd = hMain; s.subPanel.hWnd = hSub;
    g_activeMenuHwnd.store(hMain);
    ApplyBorderlessPerPixelDwm(hMain, isLight, true);
    ApplyBorderlessPerPixelDwm(hSub, isLight, true);
    ShowWindow(hMain, SW_SHOWNOACTIVATE);
    InvalidateRect(hMain, nullptr, FALSE);

    bool mouseReleased = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0;
    MSG msg{};
    while (s.running && !g_unloading.load() && IsWindow(hMain)) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { s.running = false; PostQuitMessage(static_cast<int>(msg.wParam)); break; }
            if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) { s.running = false; break; }
            if (msg.message == WM_MOUSEWHEEL) {
                POINT cur{}; GetCursorPos(&cur);
                RECT rM{}, rS{}; GetWindowRect(hMain, &rM);
                if (IsWindowVisible(hSub) && GetWindowRect(hSub, &rS) && PtInRect(&rS, cur)) {
                    SendMessageW(hSub, WM_MOUSEWHEEL, msg.wParam, msg.lParam);
                    continue;
                } else if (PtInRect(&rM, cur)) {
                    SendMessageW(hMain, WM_MOUSEWHEEL, msg.wParam, msg.lParam);
                    continue;
                }
            }
            if (msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN) {
                RECT rM{}, rS{}; GetWindowRect(hMain, &rM);
                bool inM = PtInRect(&rM, msg.pt);
                bool inS = IsWindowVisible(hSub) && GetWindowRect(hSub, &rS) && PtInRect(&rS, msg.pt);
                if (!inM && !inS) { s.running = false; break; }
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (!s.running) break;
        bool btnDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 || (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
        if (!btnDown) mouseReleased = true;
        else if (mouseReleased) {
            POINT pt{}; GetCursorPos(&pt);
            RECT rM{}, rS{}; GetWindowRect(hMain, &rM);
            bool inM = PtInRect(&rM, pt);
            bool inS = IsWindowVisible(hSub) && GetWindowRect(hSub, &rS) && PtInRect(&rS, pt);
            if (!inM && !inS) { s.running = false; break; }
        }
        MsgWaitForMultipleObjects(0, nullptr, FALSE, 25, QS_ALLINPUT);
    }

    g_activeMenuHwnd.store(nullptr);
    if (IsWindow(hSub)) DestroyWindow(hSub);
    if (IsWindow(hMain)) DestroyWindow(hMain);
    return s.selectedCmd;
}

// ============================================================================
// Interactive Liquid Glass Multi-Monitor Layout Arranger
// ============================================================================

struct LayoutArrangerState {
    UINT dpi = 96;
    bool isLight = false;
    std::vector<MonitorInfoItem> mons;
    std::vector<CustomMonitorSlot> slots;
    int dragIdx = -1;
    bool resizing = false;
    POINT dragStartPt{};
    CustomMonitorSlot dragOrigSlot{};
    int hoveredBtn = -1; // 0=Side, 1=Stack, 2=PiP, 3=Reset, 4=Apply, 5=Cancel
    int pressedBtn = -1;
};

static LayoutArrangerState g_layoutState;

static void UpdatePillGeometry(bool recenterIfNotDragged);

static RECT GetLayoutCanvasRect(int w, int h, UINT dpi) {
    int padX = DpiScale(22, dpi);
    int topY = DpiScale(58, dpi);
    int botY = h - DpiScale(62, dpi);
    int availW = w - padX * 2;
    int availH = botY - topY;
    // Maintain crisp 16:9 aspect ratio inside the dialog
    int cw = availW;
    int ch = (cw * 9) / 16;
    if (ch > availH) {
        ch = availH;
        cw = (ch * 16) / 9;
    }
    int left = (w - cw) / 2;
    int top = topY + (availH - ch) / 2;
    return RECT{ left, top, left + cw, top + ch };
}

static RECT GetLayoutBtnRect(int btnIdx, int w, int h, UINT dpi) {
    int padX = DpiScale(22, dpi);
    int btnH = DpiScale(32, dpi);
    int top = h - DpiScale(46, dpi);
    int gap = DpiScale(8, dpi);
    int wPreset = DpiScale(82, dpi);
    int wAct = DpiScale(84, dpi);
    if (btnIdx >= 0 && btnIdx <= 3) {
        int x = padX + btnIdx * (wPreset + gap);
        return RECT{ x, top, x + wPreset, top + btnH };
    }
    if (btnIdx == 4) { // Apply
        return RECT{ w - padX - wAct, top, w - padX, top + btnH };
    }
    if (btnIdx == 5) { // Cancel
        return RECT{ w - padX - wAct * 2 - gap, top, w - padX - wAct - gap, top + btnH };
    }
    return RECT{0, 0, 0, 0};
}

static void ApplyLayoutPreset(int preset) {
    int n = static_cast<int>(g_layoutState.slots.size());
    if (n <= 0) return;
    if (preset == 0 || preset == 3) { // Side-by-Side / Reset
        float cw = 1.0f / static_cast<float>(n);
        for (int i = 0; i < n; ++i) {
            g_layoutState.slots[i] = { i, true, i * cw, 0.0f, cw, 1.0f };
        }
    } else if (preset == 1) { // Stacked
        float ch = 1.0f / static_cast<float>(n);
        for (int i = 0; i < n; ++i) {
            g_layoutState.slots[i] = { i, true, 0.0f, i * ch, 1.0f, ch };
        }
    } else if (preset == 2) { // PiP Corner (Monitor 1 full, Monitor 2+ in bottom-right corner)
        g_layoutState.slots[0] = { 0, true, 0.0f, 0.0f, 1.0f, 1.0f };
        for (int i = 1; i < n; ++i) {
            float pw = 0.32f, ph = 0.32f;
            float px = std::max(0.02f, 0.98f - pw - (i - 1) * 0.34f);
            float py = 0.96f - ph;
            g_layoutState.slots[i] = { i, true, px, py, pw, ph };
        }
    }
}

static void PaintLayoutArranger(HWND hWnd) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);
    if (!g_pD2DFactory || !g_pDWriteFactory) { EndPaint(hWnd, &ps); return; }

    RECT rc{}; GetClientRect(hWnd, &rc);
    int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0) { EndPaint(hWnd, &ps); return; }

    ModSettings cfg = GetSettingsCopy();
    UiStrings ui = GetUiStrings(cfg.language);
    UINT dpi = g_layoutState.dpi;
    bool isLight = g_layoutState.isLight;

    HDC memDc = CreateCompatibleDC(hdc);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* pixels = nullptr;
    HBITMAP hBmp = CreateDIBSection(memDc, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels), nullptr, 0);
    if (hBmp && pixels) {
        HGDIOBJ oldBmp = SelectObject(memDc, hBmp);
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
        ID2D1DCRenderTarget* pRT = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&rtProps, &pRT)) && pRT) {
            pRT->BindDC(memDc, &rc);
            pRT->BeginDraw();
            pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            ApplyFluentTextRendering(pRT);
            pRT->Clear(D2D1::ColorF(0, 0, 0, 0));

            float rimInset = DpiScaleF(0.75f, dpi);
            D2D1_RECT_F rcWin = D2D1::RectF(rimInset, rimInset, w - rimInset, h - rimInset);
            float cornerR = DpiScaleF(22.0f, dpi);
            DrawThemedPillSurface(pRT, hWnd, w, h, D2D1::RectF(rimInset, rimInset, w - rimInset, h - rimInset), cornerR, dpi, cfg, isLight);

            ID2D1SolidColorBrush *pBrText = nullptr, *pBrSub = nullptr, *pBrWhite = nullptr;
            pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0.06f, 0.07f, 0.10f, 1.0f) : D2D1::ColorF(0.98f, 0.98f, 1.0f, 1.0f), &pBrText);
            pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0.24f, 0.27f, 0.34f, 0.85f) : D2D1::ColorF(0.82f, 0.86f, 0.92f, 0.82f), &pBrSub);
            pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &pBrWhite);

            const wchar_t* fontName = GetPreferredFluentFontFamily();
            IDWriteTextFormat *pFmtTitle = nullptr, *pFmtHint = nullptr, *pFmtBtn = nullptr, *pFmtCard = nullptr;
            g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                               DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(15.0f, dpi), L"", &pFmtTitle);
            g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                               DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(11.5f, dpi), L"", &pFmtHint);
            g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                               DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.0f, dpi), L"", &pFmtBtn);
            g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                               DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.0f, dpi), L"", &pFmtCard);
            if (pFmtBtn)  { pFmtBtn->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); pFmtBtn->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); }
            if (pFmtCard) { pFmtCard->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); pFmtCard->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); }

            // Header Title & Hint
            float padX = DpiScaleF(22.0f, dpi);
            if (pFmtTitle && pBrText) {
                pRT->DrawText(ui.layoutTitle, static_cast<UINT32>(wcslen(ui.layoutTitle)), pFmtTitle,
                              D2D1::RectF(padX, DpiScaleF(14.0f, dpi), w - padX, DpiScaleF(36.0f, dpi)), pBrText);
            }
            if (pFmtHint && pBrSub) {
                pRT->DrawText(ui.layoutHint, static_cast<UINT32>(wcslen(ui.layoutHint)), pFmtHint,
                              D2D1::RectF(padX, DpiScaleF(36.0f, dpi), w - padX, DpiScaleF(54.0f, dpi)), pBrSub);
            }

            // 16:9 Output Video Preview Canvas
            RECT rcC = GetLayoutCanvasRect(w, h, dpi);
            D2D1_RECT_F rcCanvas = D2D1::RectF(static_cast<float>(rcC.left), static_cast<float>(rcC.top),
                                               static_cast<float>(rcC.right), static_cast<float>(rcC.bottom));
            float cw = rcCanvas.right - rcCanvas.left, ch = rcCanvas.bottom - rcCanvas.top;
            ID2D1SolidColorBrush* pCanvasBg = nullptr;
            if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(0.05f, 0.06f, 0.09f, 0.82f), &pCanvasBg))) {
                pRT->FillRoundedRectangle(D2D1::RoundedRect(rcCanvas, DpiScaleF(12.0f, dpi), DpiScaleF(12.0f, dpi)), pCanvasBg);
                pCanvasBg->Release();
            }
            DrawD2DGlassRim(pRT, rcCanvas, DpiScaleF(12.0f, dpi), dpi, isLight, 1.1f);

            // Subtle 2x2 corner guide lines on canvas
            ID2D1SolidColorBrush* pGridBr = nullptr;
            if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &pGridBr))) {
                pRT->DrawLine(D2D1::Point2F(rcCanvas.left + cw * 0.5f, rcCanvas.top),
                              D2D1::Point2F(rcCanvas.left + cw * 0.5f, rcCanvas.bottom), pGridBr, 1.0f);
                pRT->DrawLine(D2D1::Point2F(rcCanvas.left, rcCanvas.top + ch * 0.5f),
                              D2D1::Point2F(rcCanvas.right, rcCanvas.top + ch * 0.5f), pGridBr, 1.0f);
                pGridBr->Release();
            }

            // Monitor Cards inside the canvas
            static const D2D1_COLOR_F kMonColors[4] = {
                D2D1::ColorF(0.14f, 0.54f, 0.98f, 0.44f),
                D2D1::ColorF(0.56f, 0.32f, 0.96f, 0.44f),
                D2D1::ColorF(0.12f, 0.72f, 0.48f, 0.44f),
                D2D1::ColorF(0.94f, 0.48f, 0.16f, 0.44f)
            };

            pRT->PushAxisAlignedClip(rcCanvas, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            for (size_t i = 0; i < g_layoutState.slots.size(); ++i) {
                const auto& sl = g_layoutState.slots[i];
                if (sl.monitorIndex < 0 || sl.monitorIndex >= static_cast<int>(g_layoutState.mons.size())) continue;
                const auto& m = g_layoutState.mons[sl.monitorIndex];

                D2D1_RECT_F rcCard = D2D1::RectF(
                    rcCanvas.left + sl.nx * cw + 2.0f,
                    rcCanvas.top  + sl.ny * ch + 2.0f,
                    rcCanvas.left + (sl.nx + sl.nw) * cw - 2.0f,
                    rcCanvas.top  + (sl.ny + sl.nh) * ch - 2.0f);
                D2D1_COLOR_F tint = sl.enabled ? kMonColors[i % 4] : D2D1::ColorF(0.3f, 0.3f, 0.35f, 0.22f);
                DrawD2DGlassButton(pRT, rcCard, DpiScaleF(10.0f, dpi), dpi, tint,
                                   g_layoutState.dragIdx == static_cast<int>(i), false, false);

                // Toggle ON/OFF badge in top-left corner of card
                float badgeSz = DpiScaleF(20.0f, dpi);
                D2D1_RECT_F rcBadge = D2D1::RectF(rcCard.left + DpiScaleF(8.0f, dpi), rcCard.top + DpiScaleF(8.0f, dpi),
                                                  rcCard.left + DpiScaleF(8.0f, dpi) + badgeSz, rcCard.top + DpiScaleF(8.0f, dpi) + badgeSz);
                ID2D1SolidColorBrush* pBadgeBr = nullptr;
                if (SUCCEEDED(pRT->CreateSolidColorBrush(sl.enabled ? D2D1::ColorF(0.16f, 0.80f, 0.45f, 0.95f) : D2D1::ColorF(0.5f, 0.5f, 0.55f, 0.6f), &pBadgeBr))) {
                    pRT->FillRoundedRectangle(D2D1::RoundedRect(rcBadge, badgeSz * 0.5f, badgeSz * 0.5f), pBadgeBr);
                    pBadgeBr->Release();
                }

                // Monitor label
                wchar_t cardLbl[96];
                swprintf_s(cardLbl, L"%d\n%d\u00D7%d \u2022 %dHz", m.displayNumber, m.width, m.height, m.refreshHz);
                if (pFmtCard && pBrWhite) {
                    pRT->DrawText(cardLbl, static_cast<UINT32>(wcslen(cardLbl)), pFmtCard, rcCard, pBrWhite, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                }

                // Bottom-right resize grip indicator
                if (pBrWhite) {
                    float gx = rcCard.right - DpiScaleF(10.0f, dpi), gy = rcCard.bottom - DpiScaleF(10.0f, dpi);
                    pRT->DrawLine(D2D1::Point2F(gx, rcCard.bottom - DpiScaleF(4.0f, dpi)),
                                  D2D1::Point2F(rcCard.right - DpiScaleF(4.0f, dpi), gy), pBrWhite, DpiScaleF(1.6f, dpi));
                }
            }
            pRT->PopAxisAlignedClip();

            // Bottom Action & Preset Pill Buttons
            const wchar_t* btnLabels[6] = {
                ui.layoutSideBySide, ui.layoutStacked, ui.layoutPip,
                ui.layoutReset, ui.layoutApply, ui.layoutCancel
            };
            for (int i = 0; i < 6; ++i) {
                RECT rb = GetLayoutBtnRect(i, w, h, dpi);
                D2D1_RECT_F rf = D2D1::RectF(static_cast<float>(rb.left), static_cast<float>(rb.top),
                                             static_cast<float>(rb.right), static_cast<float>(rb.bottom));
                bool hov = (g_layoutState.hoveredBtn == i), prs = (g_layoutState.pressedBtn == i);
                D2D1_COLOR_F tint = (i == 4)
                    ? D2D1::ColorF(0.12f, 0.54f, 0.98f, prs ? 0.56f : (hov ? 0.44f : 0.34f))
                    : (isLight ? D2D1::ColorF(0.06f, 0.08f, 0.14f, prs ? 0.18f : (hov ? 0.11f : 0.06f))
                               : D2D1::ColorF(0.85f, 0.92f, 1.00f, prs ? 0.22f : (hov ? 0.14f : 0.08f)));
                DrawD2DGlassButton(pRT, rf, (rf.bottom - rf.top) * 0.5f, dpi, tint, hov, prs, isLight);
                if (pFmtBtn && pBrText) {
                    pRT->DrawText(btnLabels[i], static_cast<UINT32>(wcslen(btnLabels[i])), pFmtBtn, rf,
                                  (i == 4) ? pBrWhite : pBrText, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                }
            }

            DrawThemedPillBorder(pRT, rcWin, cornerR, dpi, cfg, isLight, 1.35f);
            pRT->EndDraw();

            if (pFmtCard) pFmtCard->Release();
            if (pFmtBtn) pFmtBtn->Release();
            if (pFmtHint) pFmtHint->Release();
            if (pFmtTitle) pFmtTitle->Release();
            if (pBrWhite) pBrWhite->Release();
            if (pBrSub) pBrSub->Release();
            if (pBrText) pBrText->Release();
            pRT->Release();
        }
        BitBlt(hdc, 0, 0, w, h, memDc, 0, 0, SRCCOPY);
        SelectObject(memDc, oldBmp);
        DeleteObject(hBmp);
    }
    DeleteDC(memDc);
    EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK LayoutArrangerWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCCALCSIZE: return 0;
        case WM_ERASEBKGND: return 1;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) { DestroyWindow(hWnd); g_layoutHwnd = nullptr; return 0; }
            break;
        case WM_LBUTTONDOWN: {
            POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            RECT rc{}; GetClientRect(hWnd, &rc);
            int w = rc.right, h = rc.bottom;
            for (int i = 0; i < 6; ++i) {
                RECT rb = GetLayoutBtnRect(i, w, h, g_layoutState.dpi);
                if (PtInRect(&rb, pt)) {
                    g_layoutState.pressedBtn = i;
                    SetCapture(hWnd);
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
            }

            RECT rcC = GetLayoutCanvasRect(w, h, g_layoutState.dpi);
            float cw = static_cast<float>(rcC.right - rcC.left);
            float ch = static_cast<float>(rcC.bottom - rcC.top);
            // Hit-test monitor cards top-to-bottom (reverse z-order)
            for (int i = static_cast<int>(g_layoutState.slots.size()) - 1; i >= 0; --i) {
                auto& sl = g_layoutState.slots[i];
                RECT rcCard{
                    rcC.left + static_cast<LONG>(sl.nx * cw),
                    rcC.top  + static_cast<LONG>(sl.ny * ch),
                    rcC.left + static_cast<LONG>((sl.nx + sl.nw) * cw),
                    rcC.top  + static_cast<LONG>((sl.ny + sl.nh) * ch)
                };
                if (PtInRect(&rcCard, pt)) {
                    // Check top-left toggle badge
                    int badgeArea = DpiScale(30, g_layoutState.dpi);
                    if (pt.x <= rcCard.left + badgeArea && pt.y <= rcCard.top + badgeArea) {
                        sl.enabled = !sl.enabled;
                        InvalidateRect(hWnd, nullptr, FALSE);
                        return 0;
                    }
                    // Check bottom-right resize grip
                    int grip = DpiScale(18, g_layoutState.dpi);
                    g_layoutState.resizing = (pt.x >= rcCard.right - grip && pt.y >= rcCard.bottom - grip);
                    g_layoutState.dragIdx = i;
                    g_layoutState.dragStartPt = pt;
                    g_layoutState.dragOrigSlot = sl;
                    SetCapture(hWnd);
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            RECT rc{}; GetClientRect(hWnd, &rc);
            int w = rc.right, h = rc.bottom;

            if (g_layoutState.dragIdx >= 0 && g_layoutState.dragIdx < static_cast<int>(g_layoutState.slots.size())) {
                RECT rcC = GetLayoutCanvasRect(w, h, g_layoutState.dpi);
                float cw = static_cast<float>(std::max<LONG>(1, rcC.right - rcC.left));
                float ch = static_cast<float>(std::max<LONG>(1, rcC.bottom - rcC.top));
                float dx = static_cast<float>(pt.x - g_layoutState.dragStartPt.x) / cw;
                float dy = static_cast<float>(pt.y - g_layoutState.dragStartPt.y) / ch;
                auto& sl = g_layoutState.slots[g_layoutState.dragIdx];

                auto snapVal = [](float v, float target, float thresh) -> float {
                    return (std::fabs(v - target) < thresh) ? target : v;
                };

                if (g_layoutState.resizing) {
                    sl.nw = std::clamp(g_layoutState.dragOrigSlot.nw + dx, 0.15f, 1.0f - sl.nx);
                    sl.nh = std::clamp(g_layoutState.dragOrigSlot.nh + dy, 0.15f, 1.0f - sl.ny);
                    sl.nw = snapVal(sl.nw, 0.5f, 0.03f);
                    sl.nw = snapVal(sl.nw, 1.0f - sl.nx, 0.03f);
                    sl.nh = snapVal(sl.nh, 0.5f, 0.03f);
                    sl.nh = snapVal(sl.nh, 1.0f - sl.ny, 0.03f);
                } else {
                    float nx = std::clamp(g_layoutState.dragOrigSlot.nx + dx, 0.0f, 1.0f - sl.nw);
                    float ny = std::clamp(g_layoutState.dragOrigSlot.ny + dy, 0.0f, 1.0f - sl.nh);
                    // Magnetic snap to corners, edges, and center
                    nx = snapVal(nx, 0.0f, 0.035f);
                    nx = snapVal(nx, 1.0f - sl.nw, 0.035f);
                    nx = snapVal(nx, 0.5f - sl.nw * 0.5f, 0.03f);
                    ny = snapVal(ny, 0.0f, 0.035f);
                    ny = snapVal(ny, 1.0f - sl.nh, 0.035f);
                    ny = snapVal(ny, 0.5f - sl.nh * 0.5f, 0.03f);
                    sl.nx = nx;
                    sl.ny = ny;
                }
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }

            int hit = -1;
            for (int i = 0; i < 6; ++i) {
                RECT rb = GetLayoutBtnRect(i, w, h, g_layoutState.dpi);
                if (PtInRect(&rb, pt)) { hit = i; break; }
            }
            if (hit != g_layoutState.hoveredBtn) {
                g_layoutState.hoveredBtn = hit;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (g_layoutState.dragIdx >= 0) {
                g_layoutState.dragIdx = -1;
                g_layoutState.resizing = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            RECT rc{}; GetClientRect(hWnd, &rc);
            int hit = -1;
            for (int i = 0; i < 6; ++i) {
                RECT rb = GetLayoutBtnRect(i, rc.right, rc.bottom, g_layoutState.dpi);
                if (PtInRect(&rb, pt)) { hit = i; break; }
            }
            int pressed = g_layoutState.pressedBtn;
            g_layoutState.pressedBtn = -1;
            ReleaseCapture();

            if (hit >= 0 && hit == pressed) {
                if (hit >= 0 && hit <= 3) {
                    ApplyLayoutPreset(hit);
                    InvalidateRect(hWnd, nullptr, FALSE);
                } else if (hit == 4) { // Apply
                    {
                        std::lock_guard<std::mutex> lock(g_customLayoutMutex);
                        g_customLayout = g_layoutState.slots;
                    }
                    Wh_SetStringValue(L"Ov_captureMonitor", L"custom");
                    LoadSettings(false);
                    DestroyWindow(hWnd);
                    g_layoutHwnd = nullptr;
                    if (g_pillHwnd && IsWindow(g_pillHwnd)) {
                        UpdatePillGeometry(false);
                        InvalidateRect(g_pillHwnd, nullptr, FALSE);
                    }
                    return 0;
                } else if (hit == 5) { // Cancel
                    DestroyWindow(hWnd);
                    g_layoutHwnd = nullptr;
                    return 0;
                }
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        case WM_PAINT:
            PaintLayoutArranger(hWnd);
            return 0;
        case WM_DESTROY:
            g_layoutHwnd = nullptr;
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static void OpenCustomMonitorLayoutArranger(HWND hOwner) {
    if (g_layoutHwnd && IsWindow(g_layoutHwnd)) {
        SetForegroundWindow(g_layoutHwnd);
        return;
    }

    g_layoutState = {};
    g_layoutState.mons = EnumerateConnectedMonitors();
    {
        std::lock_guard<std::mutex> lock(g_customLayoutMutex);
        EnsureDefaultCustomLayout(g_customLayout, g_layoutState.mons);
        g_layoutState.slots = g_customLayout;
    }

    UINT dpi = (hOwner && IsWindow(hOwner)) ? GetDpiForWindow(hOwner) : 96;
    g_layoutState.dpi = dpi ? dpi : 96;
    int w = DpiScale(640, g_layoutState.dpi);
    int h = DpiScale(440, g_layoutState.dpi);

    HMONITOR hMon = (hOwner && IsWindow(hOwner))
        ? MonitorFromWindow(hOwner, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{sizeof(mi)};
    RECT rcWork{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    if (hMon && GetMonitorInfoW(hMon, &mi)) rcWork = mi.rcWork;

    int x = static_cast<int>(rcWork.left) + (static_cast<int>(rcWork.right - rcWork.left) - w) / 2;
    int y = static_cast<int>(rcWork.top)  + (static_cast<int>(rcWork.bottom - rcWork.top) - h) / 2;

    g_layoutHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        kLayoutWndClass, L"Custom Monitor Layout", WS_POPUP,
        x, y, w, h, hOwner, nullptr, GetModInstanceHandle(), nullptr);
    if (!g_layoutHwnd) return;

    ModSettings cfg = GetSettingsCopy();
    if (cfg.surfaceTheme == SurfaceTheme::Glass || cfg.surfaceTheme == SurfaceTheme::Blur) {
        CaptureMonitorBackdrop(g_layoutHwnd);
    }
    g_layoutState.isLight = ResolveIsLightMode(cfg, x, y, w, h, false);
    ApplyBorderlessPerPixelDwm(g_layoutHwnd, g_layoutState.isLight, true);
    ShowWindow(g_layoutHwnd, SW_SHOW);
    SetForegroundWindow(g_layoutHwnd);
    InvalidateRect(g_layoutHwnd, nullptr, FALSE);
}

// ============================================================================
// Floating Liquid Glass Pill Bar (Auto-Sized Idle Launcher + Recording HUD)
// ============================================================================

constexpr UINT_PTR kTimerPillTick = 201;
constexpr UINT_PTR kTimerPillAnim = 202;

struct PillLayout {
    RECT rcRecord{};
    RECT rcRegion{};
    RECT rcDraw{};
    RECT rcSysAudio{};
    RECT rcMicAudio{};
    RECT rcGear{};
    RECT rcClose{};
    // Recording mode buttons
    RECT rcRecDraw{};
    RECT rcPause{};
    RECT rcStop{};
};

struct PillState {
    UINT dpi = 96;
    bool visible = false;
    bool isLight = false;
    bool anchoredTop = false;
    int posX = 100, posY = 100;
    bool userDragged = false, dragging = false;
    POINT dragStartCursor{};
    int dragStartX = 0, dragStartY = 0;

    // 1=Rec, 2=Reg, 3=Draw(idle), 4=Sys, 5=Mic, 6=Gear, 7=Close, 8=Draw(rec), 9=Pause, 10=Stop
    int hoveredBtn = 0;
    int pressedBtn = 0;

    bool animActive = false;
    ULONGLONG animStartTick = 0;
    float animAlpha = 1.0f;
};

static PillState g_pill;

static void LaunchRegionSelector();
static void ShowPillBar(bool forRecording);
static void HidePillBar();
static void UpdatePillGeometry(bool recenterIfNotDragged);

static float MeasureTextWidthD2D(const wchar_t* text, float fontSizePx, DWRITE_FONT_WEIGHT weight) {
    if (!text || !text[0] || !g_pDWriteFactory) return 140.0f;
    float w = 140.0f;
    IDWriteTextFormat* pFmt = nullptr;
    if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
            GetPreferredFluentFontFamily(), nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, fontSizePx, L"", &pFmt)) && pFmt) {
        pFmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        IDWriteTextLayout* pLayout = nullptr;
        if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                text, static_cast<UINT32>(wcslen(text)), pFmt, 1000.0f, 60.0f, &pLayout)) && pLayout) {
            DWRITE_TEXT_METRICS tm{};
            if (SUCCEEDED(pLayout->GetMetrics(&tm))) w = tm.widthIncludingTrailingWhitespace;
            pLayout->Release();
        }
        pFmt->Release();
    }
    return w;
}

static std::wstring BuildPillSubtitle(const ModSettings& cfg) {
    RECT mon = GetCaptureTargetRect(cfg.captureMonitorMode);
    int outW = 0, outH = 0;
    ComputeCappedOutputSize(static_cast<int>(mon.right - mon.left), static_cast<int>(mon.bottom - mon.top),
                            cfg.targetResHeight, outW, outH);
    int effFps = ResolveEffectiveFps(cfg.fps, cfg.captureMonitorMode);

    UiStrings ui = GetUiStrings(cfg.language);
    std::wstring monTag;
    int idx = ParseMonitorIndexMode(cfg.captureMonitorMode);
    if (cfg.captureMonitorMode == L"all") {
        monTag = std::wstring(ui.mMonAllTag) + L"  \u2022  ";
    } else if (cfg.captureMonitorMode == L"custom") {
        monTag = std::wstring(ui.mMonCustomTag) + L"  \u2022  ";
    } else if (idx >= 0) {
        wchar_t t[32];
        swprintf_s(t, ui.mMonShortFmt, idx + 1);
        monTag = std::wstring(t) + L"  \u2022  ";
    }

    wchar_t buf[128];
    swprintf_s(buf, L"%s%d\u00D7%d  \u2022  %d FPS  \u2022  %d Mbps", monTag.c_str(), outW, outH, effFps, cfg.videoBitrateMbps);
    return buf;
}

static void GetPillTargetSize(bool recording, UINT dpi, int& outW, int& outH) {
    if (recording) {
        outW = DpiScale(306, dpi);
        outH = DpiScale(48, dpi);
        return;
    }

    ModSettings cfg = GetSettingsCopy();
    UiStrings ui = GetUiStrings(cfg.language);
    std::wstring sub = BuildPillSubtitle(cfg);

    float titleW = MeasureTextWidthD2D(ui.title, DpiScaleF(13.5f, dpi), DWRITE_FONT_WEIGHT_SEMI_BOLD);
    float subW   = MeasureTextWidthD2D(sub.c_str(), DpiScaleF(11.0f, dpi), DWRITE_FONT_WEIGHT_NORMAL);
    int leftInfoW = DpiScale(42, dpi) + static_cast<int>(std::ceil(std::max(titleW, subW))) + DpiScale(18, dpi);

    int btnH = DpiScale(34, dpi);
    int gap = DpiScale(6, dpi);
    int recW = DpiScale(84, dpi);
    int regW = DpiScale(80, dpi);
    int buttonsTotalW = recW + regW + btnH * 5 + gap * 6 + DpiScale(12, dpi);

    outW = std::max(DpiScale(580, dpi), leftInfoW + buttonsTotalW);
    outH = DpiScale(56, dpi);
}

static PillLayout ComputePillLayout(bool recording, int w, int h, UINT dpi) {
    PillLayout l{};
    int btnH = DpiScale(recording ? 32 : 34, dpi);
    int top = (h - btnH) / 2;
    int rightMargin = (h - btnH) / 2;
    int gap = DpiScale(6, dpi);

    if (recording) {
        int btnW = btnH;
        l.rcStop    = { w - rightMargin - btnW, top, w - rightMargin, top + btnH };
        l.rcPause   = { l.rcStop.left - gap - btnW, top, l.rcStop.left - gap, top + btnH };
        l.rcRecDraw = { l.rcPause.left - gap - btnW, top, l.rcPause.left - gap, top + btnH };
        return l;
    }

    int iconW = btnH;
    int recW  = DpiScale(84, dpi);
    int regW  = DpiScale(80, dpi);

    l.rcClose    = { w - rightMargin - iconW, top, w - rightMargin, top + btnH };
    l.rcGear     = { l.rcClose.left - gap - iconW, top, l.rcClose.left - gap, top + btnH };
    l.rcMicAudio = { l.rcGear.left - gap - iconW, top, l.rcGear.left - gap, top + btnH };
    l.rcSysAudio = { l.rcMicAudio.left - gap - iconW, top, l.rcMicAudio.left - gap, top + btnH };
    l.rcDraw     = { l.rcSysAudio.left - gap - iconW, top, l.rcSysAudio.left - gap, top + btnH };
    l.rcRegion   = { l.rcDraw.left - gap - regW, top, l.rcDraw.left - gap, top + btnH };
    l.rcRecord   = { l.rcRegion.left - gap - recW, top, l.rcRegion.left - gap, top + btnH };
    return l;
}

static int HitTestPillButton(const PillLayout& l, bool recording, POINT pt) {
    if (recording) {
        if (PtInRect(&l.rcRecDraw, pt)) return 8;
        if (PtInRect(&l.rcPause, pt)) return 9;
        if (PtInRect(&l.rcStop, pt)) return 10;
        return 0;
    }
    if (PtInRect(&l.rcRecord, pt)) return 1;
    if (PtInRect(&l.rcRegion, pt)) return 2;
    if (PtInRect(&l.rcDraw, pt)) return 3;
    if (PtInRect(&l.rcSysAudio, pt)) return 4;
    if (PtInRect(&l.rcMicAudio, pt)) return 5;
    if (PtInRect(&l.rcGear, pt)) return 6;
    if (PtInRect(&l.rcClose, pt)) return 7;
    return 0;
}

static void ComputeTaskbarAwarePosition(int w, int h, UINT dpi, int& outX, int& outY, bool& outAnchoredTop) {
    POINT pt{};
    GetCursorPos(&pt);
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{sizeof(mi)};
    RECT rcWork{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    if (hMon && GetMonitorInfoW(hMon, &mi)) rcWork = mi.rcWork;
    else mi.rcMonitor = rcWork;

    int leftIn = static_cast<int>(rcWork.left - mi.rcMonitor.left);
    int rightIn = static_cast<int>(mi.rcMonitor.right - rcWork.right);
    int topIn = static_cast<int>(rcWork.top - mi.rcMonitor.top);
    int botIn = static_cast<int>(mi.rcMonitor.bottom - rcWork.bottom);

    int gap = DpiScale(14, dpi);
    outX = static_cast<int>(rcWork.left) + (static_cast<int>(rcWork.right - rcWork.left) - w) / 2;

    if (g_isRecording.load() || leftIn > 0 || rightIn > 0 || topIn > 0 || botIn == 0) {
        outY = static_cast<int>(rcWork.top) + gap;
        outAnchoredTop = true;
    } else {
        outY = static_cast<int>(rcWork.bottom) - h - gap;
        outAnchoredTop = false;
    }
}

static void OpenScreenRecorderSettingsApp(HWND hPillWnd);

static void OpenSettingsMenuFromPill(HWND hWnd) {
    ModSettings cfg = GetSettingsCopy();
    UiStrings ui = GetUiStrings(cfg.language);

    RECT monRect = GetCaptureTargetRect(cfg.captureMonitorMode);
    int hwW = static_cast<int>(monRect.right - monRect.left) & ~1;
    int hwH = static_cast<int>(monRect.bottom - monRect.top) & ~1;

    MenuItem sep;
    sep.isSeparator = true;

    std::vector<MenuItem> items;
    items.push_back({ 100, ui.mDrawMode,     true,  g_drawingEnabled.load(), false, {} });
    items.push_back({ 101, ui.mSysAudio,     true,  cfg.recordSystemAudio,   false, {} });
    items.push_back({ 102, ui.mMicAudio,     true,  cfg.recordMicrophone,    false, {} });
    items.push_back({ 103, ui.mCursor,       true,  cfg.recordCursor,        false, {} });
    items.push_back({ 104, ui.mOpenOnFinish, true,  cfg.openFolderOnFinish,  false, {} });
    items.push_back(sep);

    // Hardware-capped Resolution submenu (ONLY shows presets <= hardware resolution!)
    {
        MenuItem resMenu{ 0, ui.mResolution, false, false, false, {} };
        wchar_t nativeLbl[96];
        swprintf_s(nativeLbl, L"%s (%d\u00D7%d)", ui.mResNativePrefix, hwW, hwH);
        bool isNativeSel = (cfg.targetResHeight == 0 || cfg.targetResHeight >= hwH);
        resMenu.subItems.push_back({ 250, nativeLbl, false, isNativeSel, false, {} });

        struct ResPreset { UINT id; int h; const wchar_t* tag; };
        static constexpr ResPreset kPresets[] = {
            { 251, 2160, L"4K" },
            { 252, 1440, L"2K" },
            { 253, 1080, L"Full HD" },
            { 254, 900,  L"HD+" },
            { 255, 720,  L"HD" },
            { 256, 480,  L"SD" }
        };
        for (const auto& pr : kPresets) {
            if (pr.h < hwH) {
                int pw = 0, ph = 0;
                ComputeCappedOutputSize(hwW, hwH, pr.h, pw, ph);
                wchar_t lbl[96];
                swprintf_s(lbl, L"%dp %s (%d\u00D7%d)", ph, pr.tag, pw, ph);
                resMenu.subItems.push_back({ pr.id, lbl, false, (!isNativeSel && cfg.targetResHeight == pr.h), false, {} });
            }
        }
        items.push_back(std::move(resMenu));
    }

    // Hardware-capped FPS submenu (ONLY shows FPS presets <= current monitor hardware refresh rate!)
    {
        int hwHz = GetMonitorRefreshRateHz(cfg.captureMonitorMode);
        int effFps = ResolveEffectiveFps(cfg.fps, cfg.captureMonitorMode);
        MenuItem fpsMenu{ 0, ui.mFps, false, false, false, {} };
        struct FpsOpt { UINT id; int fps; };
        static constexpr FpsOpt kFpsOpts[] = {
            { 201, 30 }, { 202, 60 }, { 203, 75 }, { 204, 90 },
            { 205, 120 }, { 206, 144 }, { 207, 165 }, { 208, 240 }
        };
        bool hasExactHw = false;
        for (const auto& opt : kFpsOpts) {
            if (opt.fps <= hwHz || opt.fps == 30) {
                if (opt.fps == hwHz) hasExactHw = true;
                wchar_t lbl[64];
                if (opt.fps == hwHz) {
                    swprintf_s(lbl, ui.mFpsCurrentFmt, opt.fps, hwHz);
                } else {
                    swprintf_s(lbl, L"%d FPS", opt.fps);
                }
                fpsMenu.subItems.push_back({ opt.id, lbl, false, effFps == opt.fps, false, {} });
            }
        }
        if (!hasExactHw && hwHz > 30 && hwHz <= 360) {
            wchar_t lbl[64];
            swprintf_s(lbl, ui.mFpsCurrentFmt, hwHz, hwHz);
            fpsMenu.subItems.push_back({ 209, lbl, false, effFps == hwHz, false, {} });
        }
        items.push_back(std::move(fpsMenu));
    }

    // Bitrate submenu
    items.push_back({ 0, ui.mBitrate, false, false, false, {
        { 211, L"6 Mbps",  false, cfg.videoBitrateMbps <= 8, false, {} },
        { 212, L"12 Mbps", false, cfg.videoBitrateMbps > 8 && cfg.videoBitrateMbps <= 16, false, {} },
        { 213, L"20 Mbps", false, cfg.videoBitrateMbps > 16 && cfg.videoBitrateMbps <= 28, false, {} },
        { 214, L"35 Mbps", false, cfg.videoBitrateMbps > 28, false, {} }
    }});

    // Dynamic Multi-Monitor submenu (Monitor 1, 2, ..., All Monitors, Custom Layout)
    {
        auto mons = EnumerateConnectedMonitors();
        MenuItem monMenu{ 0, ui.mMonitor, false, false, false, {} };
        monMenu.subItems.push_back({ 220, ui.mMonCursor, false, cfg.captureMonitorMode == L"cursor", false, {} });
        int selIdx = ParseMonitorIndexMode(cfg.captureMonitorMode);
        for (size_t i = 0; i < mons.size() && i < 8; ++i) {
            wchar_t lbl[96];
            wchar_t base[32];
            swprintf_s(base, ui.mMonItemFmt, mons[i].displayNumber);
            swprintf_s(lbl, L"%s (%d\u00D7%d \u2022 %dHz)%s",
                       base, mons[i].width, mons[i].height, mons[i].refreshHz,
                       mons[i].isPrimary ? L" ★" : L"");
            bool isSel = (selIdx == static_cast<int>(i)) || (cfg.captureMonitorMode == L"primary" && i == 0);
            monMenu.subItems.push_back({ static_cast<UINT>(230 + i), lbl, false, isSel, false, {} });
        }
        monMenu.subItems.push_back(sep);
        if (mons.size() > 1) {
            monMenu.subItems.push_back({ 225, ui.mMonAll, false, cfg.captureMonitorMode == L"all", false, {} });
        }
        monMenu.subItems.push_back({ 226, ui.mMonCustom, false, cfg.captureMonitorMode == L"custom", false, {} });
        items.push_back(std::move(monMenu));
    }

    items.push_back(sep);

    // Surface Theme submenu
    items.push_back({ 0, ui.mSurface, false, false, false, {
        { 301, ui.mSurfGlass,    false, cfg.surfaceTheme == SurfaceTheme::Glass, false, {} },
        { 302, ui.mSurfBlur,     false, cfg.surfaceTheme == SurfaceTheme::Blur,  false, {} },
        { 303, ui.mSurfSolid,    false, cfg.surfaceTheme == SurfaceTheme::Solid, false, {} },
        sep,
        { 311, ui.mGlassMax,      false, cfg.glassOpacity <= -75, false, {} },
        { 312, ui.mGlassClear,    false, cfg.glassOpacity > -75 && cfg.glassOpacity <= -25, false, {} },
        { 313, ui.mGlassBalanced, false, cfg.glassOpacity > -25 && cfg.glassOpacity <= 20, false, {} },
        { 314, ui.mGlassTinted,   false, cfg.glassOpacity > 20, false, {} }
    }});

    // Color Mode submenu
    items.push_back({ 0, ui.mColor, false, false, false, {
        { 321, ui.mColorAuto,  false, cfg.colorMode == ColorMode::Auto,  false, {} },
        { 322, ui.mColorDark,  false, cfg.colorMode == ColorMode::Dark,  false, {} },
        { 323, ui.mColorLight, false, cfg.colorMode == ColorMode::Light, false, {} }
    }});

    // Language submenu (Auto + 21 common languages, scrollable)
    struct LangMenuOpt { UINT id; const wchar_t* code; const wchar_t* label; };
    static constexpr LangMenuOpt kLangOpts[] = {
        { 331, L"auto",  nullptr },
        { 332, L"en",    L"English" },
        { 333, L"vi",    L"Tiếng Việt" },
        { 334, L"th",    L"ไทย (Thai)" },
        { 335, L"id",    L"Bahasa Indonesia" },
        { 336, L"ms",    L"Bahasa Melayu" },
        { 337, L"ja",    L"日本語" },
        { 338, L"zh_cn", L"简体中文" },
        { 339, L"zh_tw", L"繁體中文" },
        { 340, L"ko",    L"한국어" },
        { 341, L"fr",    L"Français" },
        { 342, L"de",    L"Deutsch" },
        { 343, L"es",    L"Español" },
        { 344, L"pt",    L"Português" },
        { 345, L"it",    L"Italiano" },
        { 346, L"ru",    L"Русский" },
        { 347, L"uk",    L"Українська" },
        { 348, L"pl",    L"Polski" },
        { 349, L"nl",    L"Nederlands" },
        { 350, L"tr",    L"Türkçe" },
        { 351, L"hi",    L"हिन्दी (Hindi)" },
        { 352, L"ar",    L"العربية (Arabic)" }
    };
    {
        MenuItem langMenu{ 0, ui.mLanguage, false, false, false, {} };
        for (const auto& lo : kLangOpts) {
            const wchar_t* lbl = lo.label ? lo.label : ui.mLangAuto;
            langMenu.subItems.push_back({ lo.id, lbl, false, cfg.language == lo.code, false, {} });
        }
        items.push_back(std::move(langMenu));
    }

    items.push_back(sep);
    items.push_back({ 401, ui.mOpenFolder, false, false, false, {} });
    items.push_back({ 402, ui.mCloseBar,   false, false, false, {} });

    RECT rcWnd{};
    GetWindowRect(hWnd, &rcWnd);
    POINT anchor{ rcWnd.right, g_pill.anchoredTop ? rcWnd.bottom : rcWnd.top };

    UINT cmd = RunPillPopupMenu(hWnd, g_pill.dpi, cfg, g_pill.isLight, g_pill.anchoredTop, std::move(items), anchor);
    if (cmd == 0) return;

    if (cmd == 100) { SetDrawingMode(!g_drawingEnabled.load()); return; }
    else if (cmd == 101) Wh_SetIntValue(L"Ov_recordSystemAudio", cfg.recordSystemAudio ? 0 : 1);
    else if (cmd == 102) Wh_SetIntValue(L"Ov_recordMicrophone", cfg.recordMicrophone ? 0 : 1);
    else if (cmd == 103) Wh_SetIntValue(L"Ov_recordCursor", cfg.recordCursor ? 0 : 1);
    else if (cmd == 104) Wh_SetIntValue(L"Ov_openFolderOnFinish", cfg.openFolderOnFinish ? 0 : 1);
    else if (cmd == 250) Wh_SetIntValue(L"Ov_targetResHeight", 0);
    else if (cmd == 251) Wh_SetIntValue(L"Ov_targetResHeight", 2160);
    else if (cmd == 252) Wh_SetIntValue(L"Ov_targetResHeight", 1440);
    else if (cmd == 253) Wh_SetIntValue(L"Ov_targetResHeight", 1080);
    else if (cmd == 254) Wh_SetIntValue(L"Ov_targetResHeight", 900);
    else if (cmd == 255) Wh_SetIntValue(L"Ov_targetResHeight", 720);
    else if (cmd == 256) Wh_SetIntValue(L"Ov_targetResHeight", 480);
    else if (cmd == 201) Wh_SetIntValue(L"Ov_fps", 30);
    else if (cmd == 202) Wh_SetIntValue(L"Ov_fps", 60);
    else if (cmd == 203) Wh_SetIntValue(L"Ov_fps", 75);
    else if (cmd == 204) Wh_SetIntValue(L"Ov_fps", 90);
    else if (cmd == 205) Wh_SetIntValue(L"Ov_fps", 120);
    else if (cmd == 206) Wh_SetIntValue(L"Ov_fps", 144);
    else if (cmd == 207) Wh_SetIntValue(L"Ov_fps", 165);
    else if (cmd == 208) Wh_SetIntValue(L"Ov_fps", 240);
    else if (cmd == 209) Wh_SetIntValue(L"Ov_fps", GetMonitorRefreshRateHz(cfg.captureMonitorMode));
    else if (cmd == 211) Wh_SetIntValue(L"Ov_videoBitrateMbps", 6);
    else if (cmd == 212) Wh_SetIntValue(L"Ov_videoBitrateMbps", 12);
    else if (cmd == 213) Wh_SetIntValue(L"Ov_videoBitrateMbps", 20);
    else if (cmd == 214) Wh_SetIntValue(L"Ov_videoBitrateMbps", 35);
    else if (cmd == 220) Wh_SetStringValue(L"Ov_captureMonitor", L"cursor");
    else if (cmd >= 230 && cmd <= 237) {
        wchar_t mVal[16];
        swprintf_s(mVal, L"mon:%u", cmd - 230);
        Wh_SetStringValue(L"Ov_captureMonitor", mVal);
    }
    else if (cmd == 225) Wh_SetStringValue(L"Ov_captureMonitor", L"all");
    else if (cmd == 226) { OpenCustomMonitorLayoutArranger(hWnd); return; }
    else if (cmd == 301) Wh_SetStringValue(L"Ov_surfaceTheme", L"glass");
    else if (cmd == 302) Wh_SetStringValue(L"Ov_surfaceTheme", L"blur");
    else if (cmd == 303) Wh_SetStringValue(L"Ov_surfaceTheme", L"solid");
    else if (cmd == 311) { Wh_SetStringValue(L"Ov_surfaceTheme", L"glass"); Wh_SetIntValue(L"Ov_glassOpacity", -100); }
    else if (cmd == 312) { Wh_SetStringValue(L"Ov_surfaceTheme", L"glass"); Wh_SetIntValue(L"Ov_glassOpacity", -50); }
    else if (cmd == 313) { Wh_SetStringValue(L"Ov_surfaceTheme", L"glass"); Wh_SetIntValue(L"Ov_glassOpacity", 0); }
    else if (cmd == 314) { Wh_SetStringValue(L"Ov_surfaceTheme", L"glass"); Wh_SetIntValue(L"Ov_glassOpacity", 45); }
    else if (cmd == 321) Wh_SetStringValue(L"Ov_colorMode", L"auto");
    else if (cmd == 322) Wh_SetStringValue(L"Ov_colorMode", L"dark");
    else if (cmd == 323) Wh_SetStringValue(L"Ov_colorMode", L"light");
    else if (cmd >= 331 && cmd <= 352) {
        for (const auto& lo : kLangOpts) {
            if (lo.id == cmd) { Wh_SetStringValue(L"Ov_language", lo.code); break; }
        }
    }
    else if (cmd == 401) { OpenRecordingsFolderInExplorer(); return; }
    else if (cmd == 402) { HidePillBar(); return; }

    LoadSettings(false);
    UpdatePillGeometry(false);
    InvalidateRect(hWnd, nullptr, FALSE);
    if (g_drawBarHwnd && IsWindow(g_drawBarHwnd)) {
        ModSettings nc = GetSettingsCopy();
        RECT dr{}; GetWindowRect(g_drawBarHwnd, &dr);
        g_draw.barLight = ResolveIsLightMode(nc, static_cast<int>(dr.left), static_cast<int>(dr.top),
                                             static_cast<int>(dr.right - dr.left), static_cast<int>(dr.bottom - dr.top), g_draw.barLight);
        ApplyBorderlessPerPixelDwm(g_drawBarHwnd, g_draw.barLight, true);
        InvalidateRect(g_drawBarHwnd, nullptr, FALSE);
    }
}

static void PaintPillBar(HWND hWnd) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);
    if (!g_pD2DFactory || !g_pDWriteFactory) { EndPaint(hWnd, &ps); return; }

    RECT rc{};
    GetClientRect(hWnd, &rc);
    int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0) { EndPaint(hWnd, &ps); return; }

    ModSettings cfg = GetSettingsCopy();
    UiStrings ui = GetUiStrings(cfg.language);
    bool recording = g_isRecording.load();
    bool paused = g_isPaused.load();
    bool drawOn = g_drawingEnabled.load();
    UINT dpi = g_pill.dpi;
    bool isLight = g_pill.isLight;
    PillLayout layout = ComputePillLayout(recording, w, h, dpi);

    HDC memDc = CreateCompatibleDC(hdc);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w; bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1; bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* pixels = nullptr;
    HBITMAP hBmp = CreateDIBSection(memDc, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels), nullptr, 0);
    if (hBmp && pixels) {
        HGDIOBJ oldBmp = SelectObject(memDc, hBmp);
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);

        ID2D1DCRenderTarget* pRT = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&rtProps, &pRT)) && pRT) {
            pRT->BindDC(memDc, &rc);
            pRT->BeginDraw();
            pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            ApplyFluentTextRendering(pRT);
            pRT->Clear(D2D1::ColorF(0, 0, 0, 0));

            float rimInset = DpiScaleF(0.75f, dpi);
            D2D1_RECT_F rcPill = D2D1::RectF(rimInset, rimInset, w - rimInset, h - rimInset);
            float cornerR = (rcPill.bottom - rcPill.top) * 0.5f;
            DrawThemedPillSurface(pRT, hWnd, w, h, D2D1::RectF(rimInset, rimInset, w - rimInset, h - rimInset), cornerR, dpi, cfg, isLight);

            ID2D1SolidColorBrush *pBrPrimary = nullptr, *pBrSub = nullptr, *pBrWhite = nullptr;
            pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0.06f, 0.07f, 0.09f, 1.0f) : D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &pBrPrimary);
            pRT->CreateSolidColorBrush(isLight ? D2D1::ColorF(0.24f, 0.26f, 0.32f, 0.88f) : D2D1::ColorF(0.84f, 0.87f, 0.92f, 0.88f), &pBrSub);
            pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &pBrWhite);

            ID2D1StrokeStyle* pStroke = nullptr;
            D2D1_STROKE_STYLE_PROPERTIES sp = D2D1::StrokeStyleProperties(
                D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND);
            g_pD2DFactory->CreateStrokeStyle(sp, nullptr, 0, &pStroke);

            const wchar_t* fontName = GetPreferredFluentFontFamily();
            IDWriteTextFormat *pFmtBold = nullptr, *pFmtSub = nullptr, *pFmtBtn = nullptr;
            g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                               DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.5f, dpi), L"", &pFmtBold);
            g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                               DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(11.0f, dpi), L"", &pFmtSub);
            g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                               DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.5f, dpi), L"", &pFmtBtn);
            if (pFmtBold) { pFmtBold->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); pFmtBold->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP); }
            if (pFmtSub)  { pFmtSub->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);  pFmtSub->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP); }
            if (pFmtBtn)  { pFmtBtn->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);  pFmtBtn->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP); }

            if (recording) {
                int64_t ms = g_elapsedMs.load();
                bool blinkOn = paused || ((ms / 500) % 2 == 0);
                D2D1_COLOR_F dotCol = paused ? D2D1::ColorF(0.96f, 0.70f, 0.18f, 1.0f)
                                             : (blinkOn ? D2D1::ColorF(0.94f, 0.24f, 0.28f, 1.0f) : D2D1::ColorF(0.60f, 0.14f, 0.18f, 0.85f));
                ID2D1SolidColorBrush* pDotBr = nullptr;
                if (SUCCEEDED(pRT->CreateSolidColorBrush(dotCol, &pDotBr))) {
                    pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(DpiScaleF(22.0f, dpi), h * 0.5f), DpiScaleF(5.5f, dpi), DpiScaleF(5.5f, dpi)), pDotBr);
                    pDotBr->Release();
                }

                int totalSec = static_cast<int>(ms / 1000);
                wchar_t timeBuf[48];
                if (paused) swprintf_s(timeBuf, L"%02d:%02d  %s", totalSec / 60, totalSec % 60, ui.pausedTag);
                else swprintf_s(timeBuf, L"%02d:%02d", totalSec / 60, totalSec % 60);
                if (pFmtBold && pBrPrimary) {
                    D2D1_RECT_F rcTime = D2D1::RectF(DpiScaleF(36.0f, dpi), 0, static_cast<float>(layout.rcRecDraw.left - DpiScale(8, dpi)), static_cast<float>(h));
                    pRT->DrawText(timeBuf, static_cast<UINT32>(wcslen(timeBuf)), pFmtBold, rcTime, pBrPrimary, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                }

                // Draw button during recording (8)
                {
                    D2D1_RECT_F rBtn = D2D1::RectF(static_cast<float>(layout.rcRecDraw.left), static_cast<float>(layout.rcRecDraw.top),
                                                   static_cast<float>(layout.rcRecDraw.right), static_cast<float>(layout.rcRecDraw.bottom));
                    bool hov = (g_pill.hoveredBtn == 8), prs = (g_pill.pressedBtn == 8);
                    D2D1_COLOR_F tint = drawOn
                        ? D2D1::ColorF(0.16f, 0.64f, 0.98f, prs ? 0.48f : (hov ? 0.38f : 0.28f))
                        : (isLight ? D2D1::ColorF(0.06f, 0.08f, 0.14f, prs ? 0.18f : (hov ? 0.12f : 0.06f))
                                   : D2D1::ColorF(0.85f, 0.92f, 1.00f, prs ? 0.22f : (hov ? 0.15f : 0.08f)));
                    DrawD2DGlassButton(pRT, rBtn, (rBtn.bottom - rBtn.top) * 0.5f, dpi, tint, hov, prs, isLight);
                    if (pBrPrimary && pStroke) DrawIconPen(pRT, pBrPrimary, pStroke, (rBtn.left + rBtn.right) * 0.5f, (rBtn.top + rBtn.bottom) * 0.5f, dpi);
                }

                // Pause / Resume button (9)
                {
                    D2D1_RECT_F rBtn = D2D1::RectF(static_cast<float>(layout.rcPause.left), static_cast<float>(layout.rcPause.top),
                                                   static_cast<float>(layout.rcPause.right), static_cast<float>(layout.rcPause.bottom));
                    bool hov = (g_pill.hoveredBtn == 9), prs = (g_pill.pressedBtn == 9);
                    D2D1_COLOR_F tint = isLight ? D2D1::ColorF(0.06f, 0.08f, 0.14f, prs ? 0.18f : (hov ? 0.12f : 0.06f))
                                                : D2D1::ColorF(0.85f, 0.92f, 1.00f, prs ? 0.22f : (hov ? 0.15f : 0.08f));
                    DrawD2DGlassButton(pRT, rBtn, (rBtn.bottom - rBtn.top) * 0.5f, dpi, tint, hov, prs, isLight);
                    if (pBrPrimary && pStroke) DrawIconPauseOrPlay(pRT, pBrPrimary, pStroke, (rBtn.left + rBtn.right) * 0.5f, (rBtn.top + rBtn.bottom) * 0.5f, dpi, paused);
                }

                // Stop button (10)
                {
                    D2D1_RECT_F rBtn = D2D1::RectF(static_cast<float>(layout.rcStop.left), static_cast<float>(layout.rcStop.top),
                                                   static_cast<float>(layout.rcStop.right), static_cast<float>(layout.rcStop.bottom));
                    bool hov = (g_pill.hoveredBtn == 10), prs = (g_pill.pressedBtn == 10);
                    D2D1_COLOR_F tint = D2D1::ColorF(0.90f, 0.18f, 0.24f, prs ? 0.65f : (hov ? 0.50f : 0.38f));
                    DrawD2DGlassButton(pRT, rBtn, (rBtn.bottom - rBtn.top) * 0.5f, dpi, tint, hov, prs, isLight);
                    if (pBrWhite) DrawIconStop(pRT, pBrWhite, (rBtn.left + rBtn.right) * 0.5f, (rBtn.top + rBtn.bottom) * 0.5f, dpi);
                }
            } else {
                // Idle Launcher Pill: Icon + Title + Resolution/FPS/Bitrate Subtitle
                {
                    float icx = DpiScaleF(24.0f, dpi), icy = h * 0.5f;
                    ID2D1SolidColorBrush* pRedBr = nullptr;
                    if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.25f, 0.30f, 1.0f), &pRedBr))) {
                        pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(icx, icy), DpiScaleF(8.0f, dpi), DpiScaleF(8.0f, dpi)), pRedBr, DpiScaleF(1.6f, dpi));
                        pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(icx, icy), DpiScaleF(4.2f, dpi), DpiScaleF(4.2f, dpi)), pRedBr);
                        pRedBr->Release();
                    }
                }

                float textRight = static_cast<float>(layout.rcRecord.left - DpiScale(10, dpi));
                if (pFmtBold && pBrPrimary) {
                    pRT->DrawText(ui.title, static_cast<UINT32>(wcslen(ui.title)), pFmtBold,
                                  D2D1::RectF(DpiScaleF(40.0f, dpi), DpiScaleF(8.0f, dpi), textRight, DpiScaleF(29.0f, dpi)),
                                  pBrPrimary, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                }
                if (pFmtSub && pBrSub) {
                    std::wstring meta = BuildPillSubtitle(cfg);
                    pRT->DrawText(meta.c_str(), static_cast<UINT32>(meta.size()), pFmtSub,
                                  D2D1::RectF(DpiScaleF(40.0f, dpi), DpiScaleF(28.0f, dpi), textRight, DpiScaleF(47.0f, dpi)),
                                  pBrSub, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                }

                // 1. Record Fullscreen pill
                {
                    D2D1_RECT_F rBtn = D2D1::RectF(static_cast<float>(layout.rcRecord.left), static_cast<float>(layout.rcRecord.top),
                                                   static_cast<float>(layout.rcRecord.right), static_cast<float>(layout.rcRecord.bottom));
                    bool hov = (g_pill.hoveredBtn == 1), prs = (g_pill.pressedBtn == 1);
                    DrawD2DGlassButton(pRT, rBtn, (rBtn.bottom - rBtn.top) * 0.5f, dpi,
                                       D2D1::ColorF(0.92f, 0.20f, 0.28f, prs ? 0.56f : (hov ? 0.44f : 0.30f)), hov, prs, isLight);
                    float cy = (rBtn.top + rBtn.bottom) * 0.5f;
                    ID2D1SolidColorBrush* pRecDot = nullptr;
                    if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.36f, 0.40f, 1.0f), &pRecDot))) {
                        pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rBtn.left + DpiScaleF(16.0f, dpi), cy), DpiScaleF(4.2f, dpi), DpiScaleF(4.2f, dpi)), pRecDot);
                        pRecDot->Release();
                    }
                    if (pFmtBtn && pBrPrimary) {
                        pRT->DrawText(ui.recordBtn, static_cast<UINT32>(wcslen(ui.recordBtn)), pFmtBtn,
                                      D2D1::RectF(rBtn.left + DpiScaleF(26.0f, dpi), rBtn.top, rBtn.right - DpiScaleF(8.0f, dpi), rBtn.bottom),
                                      isLight ? pBrPrimary : pBrWhite, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    }
                }

                // 2. Region Select pill
                {
                    D2D1_RECT_F rBtn = D2D1::RectF(static_cast<float>(layout.rcRegion.left), static_cast<float>(layout.rcRegion.top),
                                                   static_cast<float>(layout.rcRegion.right), static_cast<float>(layout.rcRegion.bottom));
                    bool hov = (g_pill.hoveredBtn == 2), prs = (g_pill.pressedBtn == 2);
                    DrawD2DGlassButton(pRT, rBtn, (rBtn.bottom - rBtn.top) * 0.5f, dpi,
                                       D2D1::ColorF(0.12f, 0.54f, 0.98f, prs ? 0.48f : (hov ? 0.36f : 0.22f)), hov, prs, isLight);
                    float cy = (rBtn.top + rBtn.bottom) * 0.5f;
                    ID2D1Brush* pIcoBr = isLight ? pBrPrimary : pBrWhite;
                    if (pIcoBr && pStroke) DrawIconRegion(pRT, pIcoBr, pStroke, rBtn.left + DpiScaleF(16.0f, dpi), cy, dpi);
                    if (pFmtBtn && pIcoBr) {
                        pRT->DrawText(ui.regionBtn, static_cast<UINT32>(wcslen(ui.regionBtn)), pFmtBtn,
                                      D2D1::RectF(rBtn.left + DpiScaleF(27.0f, dpi), rBtn.top, rBtn.right - DpiScaleF(8.0f, dpi), rBtn.bottom),
                                      pIcoBr, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    }
                }

                // 3. Draw toggle circle
                {
                    D2D1_RECT_F rBtn = D2D1::RectF(static_cast<float>(layout.rcDraw.left), static_cast<float>(layout.rcDraw.top),
                                                   static_cast<float>(layout.rcDraw.right), static_cast<float>(layout.rcDraw.bottom));
                    bool hov = (g_pill.hoveredBtn == 3), prs = (g_pill.pressedBtn == 3);
                    D2D1_COLOR_F tint = drawOn
                        ? D2D1::ColorF(0.16f, 0.64f, 0.98f, prs ? 0.46f : (hov ? 0.38f : 0.26f))
                        : (isLight ? D2D1::ColorF(0.06f, 0.08f, 0.14f, prs ? 0.16f : (hov ? 0.10f : 0.05f))
                                   : D2D1::ColorF(0.85f, 0.92f, 1.00f, prs ? 0.20f : (hov ? 0.13f : 0.06f)));
                    DrawD2DGlassButton(pRT, rBtn, (rBtn.bottom - rBtn.top) * 0.5f, dpi, tint, hov, prs, isLight);
                    if (pBrPrimary && pStroke) DrawIconPen(pRT, pBrPrimary, pStroke, (rBtn.left + rBtn.right) * 0.5f, (rBtn.top + rBtn.bottom) * 0.5f, dpi);
                }

                // 4. System Audio toggle circle
                {
                    D2D1_RECT_F rBtn = D2D1::RectF(static_cast<float>(layout.rcSysAudio.left), static_cast<float>(layout.rcSysAudio.top),
                                                   static_cast<float>(layout.rcSysAudio.right), static_cast<float>(layout.rcSysAudio.bottom));
                    bool hov = (g_pill.hoveredBtn == 4), prs = (g_pill.pressedBtn == 4);
                    D2D1_COLOR_F tint = cfg.recordSystemAudio
                        ? D2D1::ColorF(0.14f, 0.62f, 0.96f, prs ? 0.42f : (hov ? 0.34f : 0.22f))
                        : (isLight ? D2D1::ColorF(0.06f, 0.08f, 0.14f, prs ? 0.16f : (hov ? 0.10f : 0.05f))
                                   : D2D1::ColorF(0.85f, 0.92f, 1.00f, prs ? 0.20f : (hov ? 0.13f : 0.06f)));
                    DrawD2DGlassButton(pRT, rBtn, (rBtn.bottom - rBtn.top) * 0.5f, dpi, tint, hov, prs, isLight);
                    if (pBrPrimary && pStroke) DrawIconSpeaker(pRT, cfg.recordSystemAudio ? pBrPrimary : pBrSub, pStroke,
                                                               (rBtn.left + rBtn.right) * 0.5f, (rBtn.top + rBtn.bottom) * 0.5f, dpi, cfg.recordSystemAudio);
                }

                // 5. Microphone toggle circle
                {
                    D2D1_RECT_F rBtn = D2D1::RectF(static_cast<float>(layout.rcMicAudio.left), static_cast<float>(layout.rcMicAudio.top),
                                                   static_cast<float>(layout.rcMicAudio.right), static_cast<float>(layout.rcMicAudio.bottom));
                    bool hov = (g_pill.hoveredBtn == 5), prs = (g_pill.pressedBtn == 5);
                    D2D1_COLOR_F tint = cfg.recordMicrophone
                        ? D2D1::ColorF(0.14f, 0.62f, 0.96f, prs ? 0.42f : (hov ? 0.34f : 0.22f))
                        : (isLight ? D2D1::ColorF(0.06f, 0.08f, 0.14f, prs ? 0.16f : (hov ? 0.10f : 0.05f))
                                   : D2D1::ColorF(0.85f, 0.92f, 1.00f, prs ? 0.20f : (hov ? 0.13f : 0.06f)));
                    DrawD2DGlassButton(pRT, rBtn, (rBtn.bottom - rBtn.top) * 0.5f, dpi, tint, hov, prs, isLight);
                    if (pBrPrimary && pStroke) DrawIconMic(pRT, cfg.recordMicrophone ? pBrPrimary : pBrSub, pStroke,
                                                           (rBtn.left + rBtn.right) * 0.5f, (rBtn.top + rBtn.bottom) * 0.5f, dpi, cfg.recordMicrophone);
                }

                // 6. Settings Gear circle
                {
                    D2D1_RECT_F rBtn = D2D1::RectF(static_cast<float>(layout.rcGear.left), static_cast<float>(layout.rcGear.top),
                                                   static_cast<float>(layout.rcGear.right), static_cast<float>(layout.rcGear.bottom));
                    bool hov = (g_pill.hoveredBtn == 6), prs = (g_pill.pressedBtn == 6);
                    D2D1_COLOR_F tint = isLight ? D2D1::ColorF(0.06f, 0.08f, 0.14f, prs ? 0.16f : (hov ? 0.10f : 0.05f))
                                                : D2D1::ColorF(0.85f, 0.92f, 1.00f, prs ? 0.20f : (hov ? 0.13f : 0.07f));
                    DrawD2DGlassButton(pRT, rBtn, (rBtn.bottom - rBtn.top) * 0.5f, dpi, tint, hov, prs, isLight);
                    if (pBrPrimary && pStroke) DrawIconGear(pRT, pBrPrimary, pStroke, (rBtn.left + rBtn.right) * 0.5f, (rBtn.top + rBtn.bottom) * 0.5f, dpi);
                }

                // 7. Close circle
                {
                    D2D1_RECT_F rBtn = D2D1::RectF(static_cast<float>(layout.rcClose.left), static_cast<float>(layout.rcClose.top),
                                                   static_cast<float>(layout.rcClose.right), static_cast<float>(layout.rcClose.bottom));
                    bool hov = (g_pill.hoveredBtn == 7), prs = (g_pill.pressedBtn == 7);
                    D2D1_COLOR_F tint = isLight ? D2D1::ColorF(0.82f, 0.16f, 0.26f, prs ? 0.30f : (hov ? 0.20f : 0.08f))
                                                : D2D1::ColorF(0.92f, 0.22f, 0.32f, prs ? 0.36f : (hov ? 0.26f : 0.12f));
                    DrawD2DGlassButton(pRT, rBtn, (rBtn.bottom - rBtn.top) * 0.5f, dpi, tint, hov, prs, isLight);
                    if (pBrPrimary && pStroke) DrawIconClose(pRT, pBrPrimary, pStroke, (rBtn.left + rBtn.right) * 0.5f, (rBtn.top + rBtn.bottom) * 0.5f, dpi);
                }
            }

            DrawThemedPillBorder(pRT, rcPill, cornerR, dpi, cfg, isLight, 1.35f);
            pRT->EndDraw();

            if (pFmtBtn) pFmtBtn->Release();
            if (pFmtSub) pFmtSub->Release();
            if (pFmtBold) pFmtBold->Release();
            if (pStroke) pStroke->Release();
            if (pBrWhite) pBrWhite->Release();
            if (pBrSub) pBrSub->Release();
            if (pBrPrimary) pBrPrimary->Release();
            pRT->Release();
        }

        if (g_pill.animAlpha < 0.999f) {
            uint32_t aScale = static_cast<uint32_t>(std::clamp(static_cast<int>(std::round(g_pill.animAlpha * 256.0f)), 0, 256));
            size_t total = static_cast<size_t>(w) * h;
            for (size_t i = 0; i < total; ++i) {
                DWORD p = pixels[i];
                if (p == 0) continue;
                pixels[i] = (((((p >> 24) & 0xFF) * aScale) >> 8) << 24)
                          | (((((p >> 16) & 0xFF) * aScale) >> 8) << 16)
                          | (((((p >> 8)  & 0xFF) * aScale) >> 8) << 8)
                          |  (((p         & 0xFF) * aScale) >> 8);
            }
        }

        BitBlt(hdc, 0, 0, w, h, memDc, 0, 0, SRCCOPY);
        SelectObject(memDc, oldBmp);
        DeleteObject(hBmp);
    }
    DeleteDC(memDc);
    EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK PillWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCCALCSIZE: return 0;
        case WM_ERASEBKGND: return 1;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;

        case WM_TIMER:
            if (wParam == kTimerPillTick) {
                if (g_isRecording.load()) InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (wParam == kTimerPillAnim) {
                float t = std::clamp(static_cast<float>(GetTickCount64() - g_pill.animStartTick) / 150.0f, 0.0f, 1.0f);
                float ease = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
                g_pill.animAlpha = 0.08f + 0.92f * ease;
                int slide = static_cast<int>(std::round((1.0f - ease) * DpiScaleF(12.0f, g_pill.dpi) * (g_pill.anchoredTop ? -1.0f : 1.0f)));
                int targetY = g_pill.posY + slide;
                if (t >= 1.0f) {
                    KillTimer(hWnd, kTimerPillAnim);
                    g_pill.animActive = false; g_pill.animAlpha = 1.0f; targetY = g_pill.posY;
                }
                SetWindowPos(hWnd, HWND_TOPMOST, g_pill.posX, targetY, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            break;

        case WM_MOUSEMOVE: {
            if (g_pill.dragging) {
                POINT cur{}; GetCursorPos(&cur);
                int nx = g_pill.dragStartX + (cur.x - g_pill.dragStartCursor.x);
                int ny = g_pill.dragStartY + (cur.y - g_pill.dragStartCursor.y);
                if (nx != g_pill.posX || ny != g_pill.posY) {
                    g_pill.posX = nx; g_pill.posY = ny; g_pill.userDragged = true;
                    SetWindowPos(hWnd, HWND_TOPMOST, nx, ny, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                return 0;
            }
            POINT pt{static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
            RECT rc{}; GetClientRect(hWnd, &rc);
            bool rec = g_isRecording.load();
            int hit = HitTestPillButton(ComputePillLayout(rec, rc.right, rc.bottom, g_pill.dpi), rec, pt);
            if (hit != g_pill.hoveredBtn) { g_pill.hoveredBtn = hit; InvalidateRect(hWnd, nullptr, FALSE); }
            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hWnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_pill.hoveredBtn != 0 || g_pill.pressedBtn != 0) {
                g_pill.hoveredBtn = 0; g_pill.pressedBtn = 0;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONDOWN: {
            POINT pt{static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
            RECT rc{}; GetClientRect(hWnd, &rc);
            bool rec = g_isRecording.load();
            int hit = HitTestPillButton(ComputePillLayout(rec, rc.right, rc.bottom, g_pill.dpi), rec, pt);
            if (hit != 0) {
                g_pill.pressedBtn = hit; SetCapture(hWnd); InvalidateRect(hWnd, nullptr, FALSE);
            } else {
                g_pill.dragging = true; GetCursorPos(&g_pill.dragStartCursor);
                g_pill.dragStartX = g_pill.posX; g_pill.dragStartY = g_pill.posY;
                SetCapture(hWnd);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_pill.dragging) { g_pill.dragging = false; ReleaseCapture(); return 0; }
            POINT pt{static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
            RECT rc{}; GetClientRect(hWnd, &rc);
            bool rec = g_isRecording.load();
            int hit = HitTestPillButton(ComputePillLayout(rec, rc.right, rc.bottom, g_pill.dpi), rec, pt);
            int pressed = g_pill.pressedBtn;
            g_pill.pressedBtn = 0; ReleaseCapture(); InvalidateRect(hWnd, nullptr, FALSE);

            if (hit != 0 && hit == pressed) {
                ModSettings cfg = GetSettingsCopy();
                if (hit == 1) StartRecordingRect(GetCaptureTargetRect(cfg.captureMonitorMode), cfg.captureMonitorMode == L"custom");
                else if (hit == 2) { HidePillBar(); LaunchRegionSelector(); }
                else if (hit == 3 || hit == 8) SetDrawingMode(!g_drawingEnabled.load());
                else if (hit == 4) { Wh_SetIntValue(L"Ov_recordSystemAudio", cfg.recordSystemAudio ? 0 : 1); LoadSettings(false); InvalidateRect(hWnd, nullptr, FALSE); }
                else if (hit == 5) { Wh_SetIntValue(L"Ov_recordMicrophone", cfg.recordMicrophone ? 0 : 1); LoadSettings(false); InvalidateRect(hWnd, nullptr, FALSE); }
                else if (hit == 6) OpenScreenRecorderSettingsApp(hWnd);
                else if (hit == 7) HidePillBar();
                else if (hit == 9) { g_isPaused = !g_isPaused.load(); InvalidateRect(hWnd, nullptr, FALSE); }
                else if (hit == 10) g_stopRequested = true;
            }
            return 0;
        }

        case WM_PAINT:
            PaintPillBar(hWnd);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static void UpdatePillGeometry(bool recenterIfNotDragged) {
    if (!g_pillHwnd || !IsWindow(g_pillHwnd)) return;
    UINT dpi = GetDpiForWindow(g_pillHwnd);
    g_pill.dpi = dpi ? dpi : 96;

    int w = 0, h = 0;
    GetPillTargetSize(g_isRecording.load(), g_pill.dpi, w, h);

    if (recenterIfNotDragged || !g_pill.userDragged) {
        ComputeTaskbarAwarePosition(w, h, g_pill.dpi, g_pill.posX, g_pill.posY, g_pill.anchoredTop);
    }

    ModSettings cfg = GetSettingsCopy();
    if (cfg.surfaceTheme == SurfaceTheme::Glass || cfg.surfaceTheme == SurfaceTheme::Blur) {
        CaptureMonitorBackdrop(g_pillHwnd);
    }
    g_pill.isLight = ResolveIsLightMode(cfg, g_pill.posX, g_pill.posY, w, h, g_pill.isLight);
    ApplyBorderlessPerPixelDwm(g_pillHwnd, g_pill.isLight, true);
    SetWindowPos(g_pillHwnd, HWND_TOPMOST, g_pill.posX, g_pill.posY, w, h, SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

static void ShowPillBar(bool forRecording) {
    if (!g_pillHwnd || !IsWindow(g_pillHwnd)) {
        g_pillHwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            kPillWndClass, L"Screen Recorder Pill", WS_POPUP,
            0, 0, 300, 56, nullptr, nullptr, GetModInstanceHandle(), nullptr);
    }
    if (!g_pillHwnd) return;

    g_pill.hoveredBtn = 0; g_pill.pressedBtn = 0; g_pill.userDragged = false;
    UpdatePillGeometry(true);

    g_pill.visible = true;
    g_pill.animActive = true;
    g_pill.animStartTick = GetTickCount64();
    g_pill.animAlpha = 0.08f;

    int slide = static_cast<int>(std::round(DpiScaleF(12.0f, g_pill.dpi) * (g_pill.anchoredTop ? -1.0f : 1.0f)));
    SetWindowPos(g_pillHwnd, HWND_TOPMOST, g_pill.posX, g_pill.posY + slide, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SetTimer(g_pillHwnd, kTimerPillAnim, 12, nullptr);
    if (forRecording) SetTimer(g_pillHwnd, kTimerPillTick, 250, nullptr);
    else KillTimer(g_pillHwnd, kTimerPillTick);
    InvalidateRect(g_pillHwnd, nullptr, FALSE);
}

static void HidePillBar() {
    if (HWND hMenu = g_activeMenuHwnd.load()) {
        if (IsWindow(hMenu)) PostMessageW(hMenu, WM_CLOSE, 0, 0);
    }
    if (g_pillHwnd && IsWindow(g_pillHwnd)) {
        KillTimer(g_pillHwnd, kTimerPillTick);
        KillTimer(g_pillHwnd, kTimerPillAnim);
        ShowWindow(g_pillHwnd, SW_HIDE);
    }
    g_pill.visible = false;
}

static void DestroyPillBar() {
    if (g_layoutHwnd && IsWindow(g_layoutHwnd)) { DestroyWindow(g_layoutHwnd); g_layoutHwnd = nullptr; }
    SetDrawingMode(false);
    HidePillBar();
    if (g_pillHwnd && IsWindow(g_pillHwnd)) {
        DestroyWindow(g_pillHwnd);
        g_pillHwnd = nullptr;
    }
}

// ============================================================================
// Region Border Indicator & Interactive Region Selector
// ============================================================================

static void DestroyRegionBorder() {
    if (g_regionBorderHwnd) { DestroyWindow(g_regionBorderHwnd); g_regionBorderHwnd = nullptr; }
}

static LRESULT CALLBACK RegionBorderWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc{}; GetClientRect(hwnd, &rc);
        HBRUSH keyBrush = CreateSolidBrush(RGB(1, 2, 3));
        FillRect(hdc, &rc, keyBrush); DeleteObject(keyBrush);
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(235, 59, 68));
        HGDIOBJ oldPen = SelectObject(hdc, pen), oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, 1, 1, rc.right - 1, rc.bottom - 1);
        SelectObject(hdc, oldBrush); SelectObject(hdc, oldPen); DeleteObject(pen);
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void ShowRegionBorder(const RECT& r) {
    DestroyRegionBorder();
    int pad = 2;
    g_regionBorderHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        kRegionBorderWndClass, L"", WS_POPUP,
        r.left - pad, r.top - pad, (r.right - r.left) + pad * 2, (r.bottom - r.top) + pad * 2,
        nullptr, nullptr, GetModInstanceHandle(), nullptr);
    if (!g_regionBorderHwnd) return;
    SetLayeredWindowAttributes(g_regionBorderHwnd, RGB(1, 2, 3), 255, LWA_COLORKEY);
    SetWindowDisplayAffinity(g_regionBorderHwnd, WDA_EXCLUDEFROMCAPTURE);
    ShowWindow(g_regionBorderHwnd, SW_SHOWNOACTIVATE);
}

struct RegionSelectorState {
    bool dragging = false;
    POINT startPt{}, curPt{};
    int virtLeft = 0, virtTop = 0;
};

static RegionSelectorState g_selState;
static RECT g_pendingRegionRect{};

static LRESULT CALLBACK RegionSelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) { g_isSelectingRegion = false; DestroyWindow(hwnd); }
            return 0;
        case WM_RBUTTONDOWN:
            g_isSelectingRegion = false; DestroyWindow(hwnd); return 0;
        case WM_LBUTTONDOWN:
            g_selState.dragging = true;
            g_selState.startPt = {static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
            g_selState.curPt = g_selState.startPt;
            SetCapture(hwnd); InvalidateRect(hwnd, nullptr, FALSE); return 0;
        case WM_MOUSEMOVE:
            if (g_selState.dragging) {
                g_selState.curPt = {static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_LBUTTONUP:
            if (g_selState.dragging) {
                g_selState.dragging = false; ReleaseCapture();
                RECT sel{
                    std::min(g_selState.startPt.x, g_selState.curPt.x) + g_selState.virtLeft,
                    std::min(g_selState.startPt.y, g_selState.curPt.y) + g_selState.virtTop,
                    std::max(g_selState.startPt.x, g_selState.curPt.x) + g_selState.virtLeft,
                    std::max(g_selState.startPt.y, g_selState.curPt.y) + g_selState.virtTop
                };
                g_isSelectingRegion = false; DestroyWindow(hwnd);
                LONG w = (sel.right - sel.left) & ~1L, h = (sel.bottom - sel.top) & ~1L;
                if (w >= 64 && h >= 64) {
                    sel.right = sel.left + w; sel.bottom = sel.top + h;
                    g_pendingRegionRect = sel;
                    HWND msgWnd = g_msgHwnd.load();
                    if (msgWnd) PostMessageW(msgWnd, WM_APP_START_REGION_RECORD, 0, 0);
                }
            }
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps{}; HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc{}; GetClientRect(hwnd, &rc);
            HDC memDc = CreateCompatibleDC(hdc);
            HBITMAP bmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HGDIOBJ oldBmp = SelectObject(memDc, bmp);
            HBRUSH dimBrush = CreateSolidBrush(RGB(14, 17, 23));
            FillRect(memDc, &rc, dimBrush); DeleteObject(dimBrush);
            if (g_selState.dragging) {
                RECT sel{
                    std::min(g_selState.startPt.x, g_selState.curPt.x), std::min(g_selState.startPt.y, g_selState.curPt.y),
                    std::max(g_selState.startPt.x, g_selState.curPt.x), std::max(g_selState.startPt.y, g_selState.curPt.y)
                };
                HBRUSH clearBrush = CreateSolidBrush(RGB(42, 58, 82));
                FillRect(memDc, &sel, clearBrush); DeleteObject(clearBrush);
                HPEN pen = CreatePen(PS_SOLID, 2, RGB(56, 189, 248));
                HGDIOBJ oldPen = SelectObject(memDc, pen), oldBr = SelectObject(memDc, GetStockObject(NULL_BRUSH));
                Rectangle(memDc, sel.left, sel.top, sel.right, sel.bottom);
                SelectObject(memDc, oldBr); SelectObject(memDc, oldPen); DeleteObject(pen);
                wchar_t label[64];
                swprintf_s(label, L"%ld \u00D7 %ld", sel.right - sel.left, sel.bottom - sel.top);
                SetBkMode(memDc, TRANSPARENT); SetTextColor(memDc, RGB(255, 255, 255));
                TextOutW(memDc, sel.left + 8, std::max<LONG>(8L, sel.top - 22L), label, static_cast<int>(wcslen(label)));
            } else {
                ModSettings cfg = GetSettingsCopy();
                UiStrings ui = GetUiStrings(cfg.language);
                SetBkMode(memDc, TRANSPARENT); SetTextColor(memDc, RGB(240, 245, 255));
                DrawTextW(memDc, ui.selectHint, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDc, 0, 0, SRCCOPY);
            SelectObject(memDc, oldBmp); DeleteObject(bmp); DeleteDC(memDc);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void LaunchRegionSelector() {
    if (g_isRecording.load() || g_isSelectingRegion.load()) return;
    g_isSelectingRegion = true;
    g_selState = {};
    g_selState.virtLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    g_selState.virtTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int virtW = GetSystemMetrics(SM_CXVIRTUALSCREEN), virtH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    HWND selHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        kRegionSelWndClass, L"Select Recording Region", WS_POPUP,
        g_selState.virtLeft, g_selState.virtTop, virtW, virtH,
        nullptr, nullptr, GetModInstanceHandle(), nullptr);
    if (!selHwnd) { g_isSelectingRegion = false; return; }
    SetLayeredWindowAttributes(selHwnd, 0, 120, LWA_ALPHA);
    ShowWindow(selHwnd, SW_SHOW);
    SetForegroundWindow(selHwnd);
}

// ============================================================================
// Hidden Hotkey & UI Controller Thread
// ============================================================================

static void UnregisterAllHotkeys(HWND hwnd) {
    UnregisterHotKey(hwnd, HOTKEY_TOGGLE_BAR);
    UnregisterHotKey(hwnd, HOTKEY_RECORD_REGION);
    UnregisterHotKey(hwnd, HOTKEY_PAUSE_RESUME);
    UnregisterHotKey(hwnd, HOTKEY_DRAW_EXIT_ESC);
}

static void RegisterAllHotkeys(HWND hwnd) {
    UnregisterAllHotkeys(hwnd);
    ModSettings cfg = GetSettingsCopy();
    RegisterHotKey(hwnd, HOTKEY_TOGGLE_BAR, cfg.hotkeyModifiers | MOD_NOREPEAT, cfg.hotkeyRecordVk);
    RegisterHotKey(hwnd, HOTKEY_RECORD_REGION, cfg.hotkeyModifiers | MOD_NOREPEAT, cfg.hotkeyRegionVk);
    RegisterHotKey(hwnd, HOTKEY_PAUSE_RESUME, cfg.hotkeyModifiers | MOD_NOREPEAT, cfg.hotkeyPauseVk);
}

static UINT g_msgOpenSettings = 0;
static UINT g_msgReloadSettings = 0;
static UINT g_msgOpenLayoutArranger = 0;

static LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == g_msgReloadSettings && g_msgReloadSettings != 0) {
        LoadSettings(false);
        RegisterAllHotkeys(hwnd);
        if (g_pill.visible && g_pillHwnd) {
            ModSettings cfg = GetSettingsCopy();
            RECT rcP{};
            GetWindowRect(g_pillHwnd, &rcP);
            int pw = std::max(1, static_cast<int>(rcP.right - rcP.left));
            int ph = std::max(1, static_cast<int>(rcP.bottom - rcP.top));
            g_pill.isLight = ResolveIsLightMode(cfg, g_pill.posX, g_pill.posY, pw, ph, g_pill.isLight);
            UpdatePillGeometry(false);
            InvalidateRect(g_pillHwnd, nullptr, FALSE);
        }
        if (g_drawingEnabled.load() && g_drawBarHwnd && IsWindow(g_drawBarHwnd)) {
            InvalidateRect(g_drawBarHwnd, nullptr, FALSE);
        }
        if (g_drawingEnabled.load() && g_drawHintHwnd && IsWindow(g_drawHintHwnd)) {
            InvalidateRect(g_drawHintHwnd, nullptr, FALSE);
        }
        return 0;
    }
    if (msg == g_msgOpenLayoutArranger && g_msgOpenLayoutArranger != 0) {
        OpenCustomMonitorLayoutArranger(g_pillHwnd);
        return 0;
    }
    switch (msg) {
        case WM_HOTKEY: {
            int id = static_cast<int>(wParam);
            if (id == HOTKEY_TOGGLE_BAR) {
                if (g_isRecording.load()) g_stopRequested = true;
                else if (!g_isSelectingRegion.load()) {
                    if (g_pill.visible) HidePillBar();
                    else ShowPillBar(false);
                }
            } else if (id == HOTKEY_RECORD_REGION) {
                if (g_isRecording.load()) g_stopRequested = true;
                else { HidePillBar(); LaunchRegionSelector(); }
            } else if (id == HOTKEY_PAUSE_RESUME) {
                if (g_isRecording.load()) {
                    g_isPaused = !g_isPaused.load();
                    if (g_pillHwnd) InvalidateRect(g_pillHwnd, nullptr, FALSE);
                }
            } else if (id == HOTKEY_DRAW_EXIT_ESC) {
                if (g_drawingEnabled.load()) {
                    SetDrawingMode(false);
                }
            }
            return 0;
        }
        case WM_APP_RELOAD_SETTINGS:
            RegisterAllHotkeys(hwnd);
            if (g_pill.visible && g_pillHwnd) {
                UpdatePillGeometry(false);
                InvalidateRect(g_pillHwnd, nullptr, FALSE);
            }
            return 0;
        case WM_APP_START_REGION_RECORD:
            Sleep(80);
            ShowRegionBorder(g_pendingRegionRect);
            StartRecordingRect(g_pendingRegionRect);
            return 0;
        case WM_APP_RECORDING_STARTED: {
            ModSettings cfg = GetSettingsCopy();
            if (cfg.showFloatingHud) ShowPillBar(true);
            else HidePillBar();
            return 0;
        }
        case WM_APP_RECORDING_STOPPED:
        case WM_APP_RECORDING_ERROR:
            DestroyRegionBorder();
            SetDrawingMode(false);
            HidePillBar();
            return 0;
        case WM_APP_SHUTDOWN:
            DestroyPillBar();
            DestroyRegionBorder();
            UnregisterAllHotkeys(hwnd);
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void UiThreadProc() {
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    HINSTANCE hInst = GetModInstanceHandle();

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr, reinterpret_cast<void**>(&g_pD2DFactory));
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_pDWriteFactory));
    if (g_pDWriteFactory) {
        IDWriteRenderingParams* pDef = nullptr;
        if (SUCCEEDED(g_pDWriteFactory->CreateRenderingParams(&pDef)) && pDef) {
            g_pDWriteFactory->CreateCustomRenderingParams(
                pDef->GetGamma(), pDef->GetEnhancedContrast(), pDef->GetClearTypeLevel(),
                pDef->GetPixelGeometry(), DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC, &g_pNaturalRenderParams);
            pDef->Release();
        }
    }

    auto regCls = [&](const wchar_t* name, WNDPROC proc, HCURSOR cur) {
        WNDCLASSEXW wc{sizeof(wc)};
        wc.lpfnWndProc = proc; wc.hInstance = hInst; wc.hCursor = cur; wc.lpszClassName = name;
        RegisterClassExW(&wc);
    };
    regCls(kMsgWndClass, MsgWndProc, nullptr);
    regCls(kPillWndClass, PillWndProc, LoadCursorW(nullptr, IDC_ARROW));
    regCls(kMenuWndClass, MenuWndProc, LoadCursorW(nullptr, IDC_ARROW));
    regCls(kRegionSelWndClass, RegionSelWndProc, LoadCursorW(nullptr, IDC_CROSS));
    regCls(kRegionBorderWndClass, RegionBorderWndProc, nullptr);
    regCls(kDrawCanvasWndClass, DrawCanvasWndProc, LoadCursorW(nullptr, IDC_CROSS));
    regCls(kDrawBarWndClass, DrawBarWndProc, LoadCursorW(nullptr, IDC_ARROW));
    regCls(kDrawHintWndClass, DrawHintWndProc, nullptr);
    regCls(kLayoutWndClass, LayoutArrangerWndProc, LoadCursorW(nullptr, IDC_ARROW));

    HWND hwnd = CreateWindowExW(0, kMsgWndClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInst, nullptr);
    g_msgHwnd.store(hwnd);
    if (hwnd) {
        if (g_msgReloadSettings) ChangeWindowMessageFilterEx(hwnd, g_msgReloadSettings, MSGFLT_ALLOW, nullptr);
        if (g_msgOpenLayoutArranger) ChangeWindowMessageFilterEx(hwnd, g_msgOpenLayoutArranger, MSGFLT_ALLOW, nullptr);
    }

    if (hwnd) {
        RegisterAllHotkeys(hwnd);
        MSG msg{};
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    g_msgHwnd.store(nullptr);
    UnregisterClassW(kLayoutWndClass, hInst);
    UnregisterClassW(kDrawHintWndClass, hInst);
    UnregisterClassW(kDrawBarWndClass, hInst);
    UnregisterClassW(kDrawCanvasWndClass, hInst);
    UnregisterClassW(kRegionBorderWndClass, hInst);
    UnregisterClassW(kRegionSelWndClass, hInst);
    UnregisterClassW(kMenuWndClass, hInst);
    UnregisterClassW(kPillWndClass, hInst);
    UnregisterClassW(kMsgWndClass, hInst);

    if (g_pNaturalRenderParams) { g_pNaturalRenderParams->Release(); g_pNaturalRenderParams = nullptr; }
    if (g_pDWriteFactory) { g_pDWriteFactory->Release(); g_pDWriteFactory = nullptr; }
    if (g_pD2DFactory) { g_pD2DFactory->Release(); g_pD2DFactory = nullptr; }
    if (SUCCEEDED(comHr)) CoUninitialize();
}


// ============================================================================
// Dedicated WinUI 3 Settings Window (Hosted in windhawk.exe -tool-mod)
// ============================================================================

static constexpr const wchar_t* kRecSettingsHostClass = L"WindhawkScreenRecSettingsHost_v2";
static constexpr const wchar_t* kRecWin11SettingsClass = L"WindhawkScreenRecWin11Settings_v2";

static bool g_isExplorerProcess = false;
static bool g_isToolModProcessLauncher = false;
static HANDLE g_toolModProcessMutex = nullptr;
static bool g_recSettingsHostRegistered = false;
static bool g_recWin11SettingsRegistered = false;
static std::atomic<HWND> g_hRecSettingsHostWnd{nullptr};
static std::atomic<HWND> g_hRecWin11SettingsWnd{nullptr};
static HANDLE g_hRecSettingsHostThread = nullptr;
static DWORD g_recSettingsHostThreadId = 0;

static void OpenScreenRecorderSettingsApp(HWND) {
    if (g_unloading.load()) return;
    HWND hHost = FindWindowW(kRecSettingsHostClass, nullptr);
    if (!hHost || !IsWindow(hHost) || g_msgOpenSettings == 0) {
        Wh_Log(L"[ScreenRecorder] Settings host not running (hHost=%p, msg=%u)", hHost, g_msgOpenSettings);
        return;
    }
    DWORD hostPid = 0;
    GetWindowThreadProcessId(hHost, &hostPid);
    if (hostPid != 0) AllowSetForegroundWindow(hostPid);
    PostMessageW(hHost, g_msgOpenSettings, 0, 0);
}

static void BroadcastReloadToExplorerRecorder() {
    HWND hMsg = FindWindowExW(HWND_MESSAGE, nullptr, kMsgWndClass, nullptr);
    if (hMsg && IsWindow(hMsg) && g_msgReloadSettings != 0) {
        PostMessageW(hMsg, g_msgReloadSettings, 0, 0);
    }
}

static void TriggerLayoutArrangerInExplorer() {
    HWND hMsg = FindWindowExW(HWND_MESSAGE, nullptr, kMsgWndClass, nullptr);
    if (hMsg && IsWindow(hMsg) && g_msgOpenLayoutArranger != 0) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hMsg, &pid);
        if (pid != 0) AllowSetForegroundWindow(pid);
        PostMessageW(hMsg, g_msgOpenLayoutArranger, 0, 0);
    }
}

static D2D1_COLOR_F GetWindowsSystemAccentColor(bool isLight) {
    DWORD abgr = 0;
    DWORD cb = sizeof(abgr);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"AccentColor",
                     RRF_RT_REG_DWORD, nullptr, &abgr, &cb) == ERROR_SUCCESS && abgr != 0) {
        float r = static_cast<float>(abgr & 0xFF) / 255.0f;
        float g = static_cast<float>((abgr >> 8) & 0xFF) / 255.0f;
        float b = static_cast<float>((abgr >> 16) & 0xFF) / 255.0f;
        if (!isLight) {
            r = std::min(1.0f, r * 1.25f + 0.12f);
            g = std::min(1.0f, g * 1.25f + 0.12f);
            b = std::min(1.0f, b * 1.25f + 0.12f);
        }
        return D2D1::ColorF(r, g, b, 1.0f);
    }
    return isLight ? D2D1::ColorF(0.000f, 0.404f, 0.753f, 1.0f)
                   : D2D1::ColorF(0.376f, 0.804f, 1.000f, 1.0f);
}

static void RgbToHsv(COLORREF rgb, float& hDeg, float& s, float& v) {
    float r = GetRValue(rgb) / 255.0f;
    float g = GetGValue(rgb) / 255.0f;
    float b = GetBValue(rgb) / 255.0f;
    float cmax = std::max(r, std::max(g, b));
    float cmin = std::min(r, std::min(g, b));
    float d = cmax - cmin;
    v = cmax;
    s = (cmax > 0.0001f) ? (d / cmax) : 0.0f;
    if (d < 0.0001f) hDeg = 0.0f;
    else if (cmax == r) hDeg = 60.0f * fmodf((g - b) / d, 6.0f);
    else if (cmax == g) hDeg = 60.0f * ((b - r) / d + 2.0f);
    else hDeg = 60.0f * ((r - g) / d + 4.0f);
    if (hDeg < 0.0f) hDeg += 360.0f;
}

static COLORREF HsvToRgb(float hDeg, float s, float v) {
    hDeg = fmodf(hDeg, 360.0f);
    if (hDeg < 0.0f) hDeg += 360.0f;
    s = std::clamp(s, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);
    float c = v * s;
    float x = c * (1.0f - fabsf(fmodf(hDeg / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    float r = 0, g = 0, b = 0;
    if (hDeg < 60.0f)       { r = c; g = x; b = 0; }
    else if (hDeg < 120.0f) { r = x; g = c; b = 0; }
    else if (hDeg < 180.0f) { r = 0; g = c; b = x; }
    else if (hDeg < 240.0f) { r = 0; g = x; b = c; }
    else if (hDeg < 300.0f) { r = x; g = 0; b = c; }
    else                    { r = c; g = 0; b = x; }
    return RGB(
        std::clamp(static_cast<int>(std::lround((r + m) * 255.0f)), 0, 255),
        std::clamp(static_cast<int>(std::lround((g + m) * 255.0f)), 0, 255),
        std::clamp(static_cast<int>(std::lround((b + m) * 255.0f)), 0, 255));
}

struct RecColorPickerSession {
    HWND hWnd = nullptr;
    UINT dpi = 96;
    bool isLight = false;
    bool isVi = false;
    float hue = 210.0f, sat = 0.7f, val = 0.85f;
    bool confirmed = false, done = false;
    int dragMode = 0, hoveredId = 0;
    RECT rcSpectrum{}, rcValBar{}, rcApply{}, rcCancel{}, rcSwatches[16]{};
    ID2D1HwndRenderTarget* pRT = nullptr;
    ID2D1Bitmap* pSpectrumBmp = nullptr;
};

static constexpr COLORREF kRecCuratedSwatches[16] = {
    RGB(12, 12, 16),   RGB(30, 30, 36),   RGB(46, 52, 64),   RGB(30, 30, 46),
    RGB(40, 42, 54),   RGB(25, 23, 36),   RGB(17, 30, 28),   RGB(250, 247, 242),
    RGB(0, 120, 212),  RGB(136, 192, 208),RGB(180, 190, 254),RGB(189, 147, 249),
    RGB(235, 188, 186),RGB(52, 211, 153), RGB(245, 158, 11), RGB(255, 255, 255)
};

static void PaintRecColorPicker(HWND hWnd, RecColorPickerSession* s) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rc{};
    GetClientRect(hWnd, &rc);
    int w = static_cast<int>(rc.right - rc.left);
    int h = static_cast<int>(rc.bottom - rc.top);
    if (!s || w <= 0 || h <= 0 || !g_pD2DFactory || !g_pDWriteFactory) { EndPaint(hWnd, &ps); return; }

    D2D1_SIZE_U sz = D2D1::SizeU(static_cast<UINT32>(w), static_cast<UINT32>(h));
    if (!s->pRT) {
        D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 96.0f, 96.0f);
        D2D1_HWND_RENDER_TARGET_PROPERTIES hrtp = D2D1::HwndRenderTargetProperties(hWnd, sz, D2D1_PRESENT_OPTIONS_IMMEDIATELY);
        if (FAILED(g_pD2DFactory->CreateHwndRenderTarget(rtp, hrtp, &s->pRT))) s->pRT = nullptr;
    } else {
        D2D1_SIZE_U cur = s->pRT->GetPixelSize();
        if (cur.width != sz.width || cur.height != sz.height) s->pRT->Resize(sz);
    }
    if (!s->pRT) { EndPaint(hWnd, &ps); return; }

    if (!s->pSpectrumBmp) {
        constexpr UINT32 bw = 256, bh = 160;
        std::vector<uint32_t> px(bw * bh);
        for (UINT32 y = 0; y < bh; ++y) {
            float sat = 1.0f - (static_cast<float>(y) / static_cast<float>(bh - 1));
            for (UINT32 x = 0; x < bw; ++x) {
                float hue = (static_cast<float>(x) / static_cast<float>(bw - 1)) * 360.0f;
                COLORREF c = HsvToRgb(hue, sat, 1.0f);
                px[y * bw + x] = 0xFF000000u | (static_cast<uint32_t>(GetRValue(c)) << 16) |
                                 (static_cast<uint32_t>(GetGValue(c)) << 8) | static_cast<uint32_t>(GetBValue(c));
            }
        }
        D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
        s->pRT->CreateBitmap(D2D1::SizeU(bw, bh), px.data(), bw * 4, bp, &s->pSpectrumBmp);
    }

    auto S = [&](float v) { return DpiScaleF(v, s->dpi); };
    bool isLight = s->isLight;
    COLORREF curRgb = HsvToRgb(s->hue, s->sat, s->val);
    D2D1_COLOR_F colBg     = isLight ? D2D1::ColorF(0.965f, 0.968f, 0.978f, 1.0f) : D2D1::ColorF(0.145f, 0.148f, 0.160f, 1.0f);
    D2D1_COLOR_F colCard   = isLight ? D2D1::ColorF(1.000f, 1.000f, 1.000f, 1.0f) : D2D1::ColorF(0.195f, 0.198f, 0.214f, 1.0f);
    D2D1_COLOR_F colBorder = isLight ? D2D1::ColorF(0.840f, 0.850f, 0.875f, 1.0f) : D2D1::ColorF(0.280f, 0.285f, 0.310f, 1.0f);
    D2D1_COLOR_F colText   = isLight ? D2D1::ColorF(0.100f, 0.105f, 0.120f, 1.0f) : D2D1::ColorF(0.960f, 0.965f, 0.978f, 1.0f);
    D2D1_COLOR_F colSub    = isLight ? D2D1::ColorF(0.380f, 0.395f, 0.430f, 1.0f) : D2D1::ColorF(0.680f, 0.695f, 0.730f, 1.0f);
    D2D1_COLOR_F colAccent = GetWindowsSystemAccentColor(isLight);
    D2D1_COLOR_F colCur    = D2D1::ColorF(GetRValue(curRgb) / 255.0f, GetGValue(curRgb) / 255.0f, GetBValue(curRgb) / 255.0f, 1.0f);

    ID2D1HwndRenderTarget* pRT = s->pRT;
    pRT->BeginDraw();
    pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
    pRT->Clear(colBg);

    ID2D1SolidColorBrush *pBrCard = nullptr, *pBrBorder = nullptr, *pBrText = nullptr, *pBrSub = nullptr;
    ID2D1SolidColorBrush *pBrAccent = nullptr, *pBrCur = nullptr, *pBrWhite = nullptr, *pBrBlack = nullptr;
    pRT->CreateSolidColorBrush(colCard, &pBrCard);
    pRT->CreateSolidColorBrush(colBorder, &pBrBorder);
    pRT->CreateSolidColorBrush(colText, &pBrText);
    pRT->CreateSolidColorBrush(colSub, &pBrSub);
    pRT->CreateSolidColorBrush(colAccent, &pBrAccent);
    pRT->CreateSolidColorBrush(colCur, &pBrCur);
    pRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &pBrWhite);
    pRT->CreateSolidColorBrush(D2D1::ColorF(0.05f, 0.05f, 0.07f, 0.85f), &pBrBlack);

    const wchar_t* fontName = GetPreferredFluentFontFamily();
    IDWriteTextFormat *pFmtLabel = nullptr, *pFmtHex = nullptr, *pFmtBtn = nullptr;
    g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(12.5f), L"", &pFmtLabel);
    g_pDWriteFactory->CreateTextFormat(L"Cascadia Mono", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(13.5f), L"", &pFmtHex);
    g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(13.0f), L"", &pFmtBtn);
    if (pFmtBtn) { pFmtBtn->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); pFmtBtn->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); }

    float pad = S(18.0f);
    float specW = static_cast<float>(w) - pad * 2.0f - S(34.0f);
    float specH = S(170.0f);
    D2D1_RECT_F rSpec = D2D1::RectF(pad, pad, pad + specW, pad + specH);
    s->rcSpectrum = { static_cast<LONG>(rSpec.left), static_cast<LONG>(rSpec.top), static_cast<LONG>(rSpec.right), static_cast<LONG>(rSpec.bottom) };

    if (s->pSpectrumBmp) pRT->DrawBitmap(s->pSpectrumBmp, rSpec);
    if (s->val < 0.999f) {
        ID2D1SolidColorBrush* pShade = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f - s->val), &pShade))) {
            pRT->FillRectangle(rSpec, pShade);
            pShade->Release();
        }
    }
    pRT->DrawRectangle(rSpec, pBrBorder, 1.0f);

    float cx = rSpec.left + (s->hue / 360.0f) * (rSpec.right - rSpec.left);
    float cy = rSpec.top + (1.0f - s->sat) * (rSpec.bottom - rSpec.top);
    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), S(7.0f), S(7.0f)), pBrBlack, 2.2f);
    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), S(5.5f), S(5.5f)), pBrWhite, 2.0f);

    float barX = rSpec.right + S(12.0f);
    D2D1_RECT_F rBar = D2D1::RectF(barX, rSpec.top, static_cast<float>(w) - pad, rSpec.bottom);
    s->rcValBar = { static_cast<LONG>(rBar.left), static_cast<LONG>(rBar.top), static_cast<LONG>(rBar.right), static_cast<LONG>(rBar.bottom) };
    COLORREF topRgb = HsvToRgb(s->hue, s->sat, 1.0f);
    D2D1_GRADIENT_STOP stops[2] = {
        { 0.0f, D2D1::ColorF(GetRValue(topRgb) / 255.0f, GetGValue(topRgb) / 255.0f, GetBValue(topRgb) / 255.0f, 1.0f) },
        { 1.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f) }
    };
    ID2D1GradientStopCollection* pStops = nullptr;
    if (SUCCEEDED(pRT->CreateGradientStopCollection(stops, 2, &pStops))) {
        ID2D1LinearGradientBrush* pGrad = nullptr;
        if (SUCCEEDED(pRT->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(rBar.left, rBar.top), D2D1::Point2F(rBar.left, rBar.bottom)), pStops, &pGrad))) {
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rBar, S(6), S(6)), pGrad);
            pGrad->Release();
        }
        pStops->Release();
    }
    pRT->DrawRoundedRectangle(D2D1::RoundedRect(rBar, S(6), S(6)), pBrBorder, 1.0f);
    float valY = rBar.top + (1.0f - s->val) * (rBar.bottom - rBar.top);
    float barCx = (rBar.left + rBar.right) * 0.5f;
    pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(barCx, valY), S(8.0f), S(8.0f)), pBrWhite);
    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(barCx, valY), S(8.0f), S(8.0f)), pBrBlack, 1.5f);

    float prevY = rSpec.bottom + S(14.0f);
    D2D1_RECT_F rPrevCard = D2D1::RectF(pad, prevY, static_cast<float>(w) - pad, prevY + S(52.0f));
    pRT->FillRoundedRectangle(D2D1::RoundedRect(rPrevCard, S(6), S(6)), pBrCard);
    pRT->DrawRoundedRectangle(D2D1::RoundedRect(rPrevCard, S(6), S(6)), pBrBorder, 1.0f);
    D2D1_RECT_F rSwatch = D2D1::RectF(rPrevCard.left + S(10), rPrevCard.top + S(10), rPrevCard.left + S(56), rPrevCard.bottom - S(10));
    pRT->FillRoundedRectangle(D2D1::RoundedRect(rSwatch, S(5), S(5)), pBrCur);
    pRT->DrawRoundedRectangle(D2D1::RoundedRect(rSwatch, S(5), S(5)), pBrBorder, 1.0f);

    std::wstring hexStr = FormatHexColor(curRgb);
    wchar_t rgbInfo[64] = {};
    swprintf_s(rgbInfo, L"R %u  \u2022  G %u  \u2022  B %u", static_cast<unsigned>(GetRValue(curRgb)), static_cast<unsigned>(GetGValue(curRgb)), static_cast<unsigned>(GetBValue(curRgb)));
    if (pFmtHex) pRT->DrawText(hexStr.c_str(), static_cast<UINT32>(hexStr.size()), pFmtHex,
        D2D1::RectF(rSwatch.right + S(12), rPrevCard.top + S(7), rPrevCard.right - S(10), rPrevCard.top + S(27)), pBrText);
    if (pFmtLabel) pRT->DrawText(rgbInfo, static_cast<UINT32>(wcslen(rgbInfo)), pFmtLabel,
        D2D1::RectF(rSwatch.right + S(12), rPrevCard.top + S(27), rPrevCard.right - S(10), rPrevCard.bottom - S(6)), pBrSub);

    float swTop = rPrevCard.bottom + S(14.0f);
    const wchar_t* swHdr = s->isVi ? L"Bảng màu nhanh Fluent" : L"Quick Fluent Swatches";
    if (pFmtLabel) pRT->DrawText(swHdr, static_cast<UINT32>(wcslen(swHdr)), pFmtLabel,
        D2D1::RectF(pad, swTop, static_cast<float>(w) - pad, swTop + S(20)), pBrSub);
    float gridTop = swTop + S(24.0f);
    float gap = S(8.0f);
    float tileW = (static_cast<float>(w) - pad * 2.0f - gap * 7.0f) / 8.0f;
    float tileH = S(28.0f);
    for (int i = 0; i < 16; ++i) {
        int r = i / 8, c = i % 8;
        D2D1_RECT_F rt = D2D1::RectF(pad + c * (tileW + gap), gridTop + r * (tileH + gap),
                                     pad + c * (tileW + gap) + tileW, gridTop + r * (tileH + gap) + tileH);
        s->rcSwatches[i] = { static_cast<LONG>(rt.left), static_cast<LONG>(rt.top), static_cast<LONG>(rt.right), static_cast<LONG>(rt.bottom) };
        COLORREF sc = kRecCuratedSwatches[i];
        ID2D1SolidColorBrush* pSwBr = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(GetRValue(sc) / 255.0f, GetGValue(sc) / 255.0f, GetBValue(sc) / 255.0f, 1.0f), &pSwBr))) {
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rt, S(5), S(5)), pSwBr);
            pSwBr->Release();
        }
        bool sel = (sc == curRgb) || (s->hoveredId == 100 + i);
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rt, S(5), S(5)), sel ? pBrAccent : pBrBorder, sel ? 2.0f : 1.0f);
    }

    float btnH = S(34.0f);
    float btnY = static_cast<float>(h) - pad - btnH;
    float btnW = (static_cast<float>(w) - pad * 2.0f - S(10.0f)) * 0.5f;
    D2D1_RECT_F rApply  = D2D1::RectF(pad, btnY, pad + btnW, btnY + btnH);
    D2D1_RECT_F rCancel = D2D1::RectF(rApply.right + S(10.0f), btnY, static_cast<float>(w) - pad, btnY + btnH);
    s->rcApply  = { static_cast<LONG>(rApply.left),  static_cast<LONG>(rApply.top),  static_cast<LONG>(rApply.right),  static_cast<LONG>(rApply.bottom) };
    s->rcCancel = { static_cast<LONG>(rCancel.left), static_cast<LONG>(rCancel.top), static_cast<LONG>(rCancel.right), static_cast<LONG>(rCancel.bottom) };

    pRT->FillRoundedRectangle(D2D1::RoundedRect(rApply, S(5), S(5)), pBrAccent);
    if (pFmtBtn) {
        const wchar_t* okTxt = s->isVi ? L"Áp dụng" : L"Apply";
        pRT->DrawText(okTxt, static_cast<UINT32>(wcslen(okTxt)), pFmtBtn, rApply, isLight ? pBrWhite : pBrBlack);
    }
    pRT->FillRoundedRectangle(D2D1::RoundedRect(rCancel, S(5), S(5)), pBrCard);
    pRT->DrawRoundedRectangle(D2D1::RoundedRect(rCancel, S(5), S(5)), (s->hoveredId == 2) ? pBrAccent : pBrBorder, 1.0f);
    if (pFmtBtn) {
        const wchar_t* cancelTxt = s->isVi ? L"Hủy" : L"Cancel";
        pRT->DrawText(cancelTxt, static_cast<UINT32>(wcslen(cancelTxt)), pFmtBtn, rCancel, pBrText);
    }

    HRESULT hrEnd = pRT->EndDraw();
    if (pFmtLabel) pFmtLabel->Release();
    if (pFmtHex)   pFmtHex->Release();
    if (pFmtBtn)   pFmtBtn->Release();
    if (pBrCard)   pBrCard->Release();
    if (pBrBorder) pBrBorder->Release();
    if (pBrText)   pBrText->Release();
    if (pBrSub)    pBrSub->Release();
    if (pBrAccent) pBrAccent->Release();
    if (pBrCur)    pBrCur->Release();
    if (pBrWhite)  pBrWhite->Release();
    if (pBrBlack)  pBrBlack->Release();
    if (hrEnd == D2DERR_RECREATE_TARGET) {
        if (s->pSpectrumBmp) { s->pSpectrumBmp->Release(); s->pSpectrumBmp = nullptr; }
        if (s->pRT) { s->pRT->Release(); s->pRT = nullptr; }
        InvalidateRect(hWnd, nullptr, FALSE);
    }
    EndPaint(hWnd, &ps);
}

static void UpdateRecColorPickerFromPoint(RecColorPickerSession* s, POINT pt) {
    if (!s) return;
    if (s->dragMode == 1) {
        float w = static_cast<float>(std::max<LONG>(1L, s->rcSpectrum.right - s->rcSpectrum.left));
        float h = static_cast<float>(std::max<LONG>(1L, s->rcSpectrum.bottom - s->rcSpectrum.top));
        s->hue = std::clamp((pt.x - s->rcSpectrum.left) / w, 0.0f, 1.0f) * 360.0f;
        s->sat = 1.0f - std::clamp((pt.y - s->rcSpectrum.top) / h, 0.0f, 1.0f);
        InvalidateRect(s->hWnd, nullptr, FALSE);
    } else if (s->dragMode == 2) {
        float h = static_cast<float>(std::max<LONG>(1L, s->rcValBar.bottom - s->rcValBar.top));
        s->val = 1.0f - std::clamp((pt.y - s->rcValBar.top) / h, 0.0f, 1.0f);
        InvalidateRect(s->hWnd, nullptr, FALSE);
    }
}

static LRESULT CALLBACK RecColorPickerWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto* s = reinterpret_cast<RecColorPickerSession*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    switch (uMsg) {
        case WM_CREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            s = reinterpret_cast<RecColorPickerSession*>(cs->lpCreateParams);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(s));
            if (s) {
                s->hWnd = hWnd;
                BOOL dark = s->isLight ? FALSE : TRUE;
                DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
                DWORD corner = 2;
                DwmSetWindowAttribute(hWnd, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/, &corner, sizeof(corner));
                COLORREF cap = s->isLight ? RGB(246, 247, 249) : RGB(37, 38, 41);
                DwmSetWindowAttribute(hWnd, 35 /*DWMWA_CAPTION_COLOR*/, &cap, sizeof(cap));
            }
            return 0;
        }
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: PaintRecColorPicker(hWnd, s); return 0;
        case WM_LBUTTONDOWN: {
            if (!s) return 0;
            POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            if (PtInRect(&s->rcSpectrum, pt)) { s->dragMode = 1; SetCapture(hWnd); UpdateRecColorPickerFromPoint(s, pt); return 0; }
            if (PtInRect(&s->rcValBar, pt))   { s->dragMode = 2; SetCapture(hWnd); UpdateRecColorPickerFromPoint(s, pt); return 0; }
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (!s) return 0;
            POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            if (s->dragMode != 0) { UpdateRecColorPickerFromPoint(s, pt); return 0; }
            int hov = 0;
            if (PtInRect(&s->rcApply, pt)) hov = 1;
            else if (PtInRect(&s->rcCancel, pt)) hov = 2;
            else { for (int i = 0; i < 16; ++i) if (PtInRect(&s->rcSwatches[i], pt)) { hov = 100 + i; break; } }
            if (hov != s->hoveredId) { s->hoveredId = hov; InvalidateRect(hWnd, nullptr, FALSE); }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (!s) return 0;
            POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            if (s->dragMode != 0) { s->dragMode = 0; ReleaseCapture(); return 0; }
            if (PtInRect(&s->rcApply, pt))  { s->confirmed = true; s->done = true; DestroyWindow(hWnd); return 0; }
            if (PtInRect(&s->rcCancel, pt)) { s->confirmed = false; s->done = true; DestroyWindow(hWnd); return 0; }
            for (int i = 0; i < 16; ++i) {
                if (PtInRect(&s->rcSwatches[i], pt)) {
                    RgbToHsv(kRecCuratedSwatches[i], s->hue, s->sat, s->val);
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
            }
            return 0;
        }
        case WM_KEYDOWN:
            if (s && wParam == VK_ESCAPE) { s->confirmed = false; s->done = true; DestroyWindow(hWnd); return 0; }
            if (s && wParam == VK_RETURN) { s->confirmed = true;  s->done = true; DestroyWindow(hWnd); return 0; }
            break;
        case WM_CLOSE:
            if (s) { s->confirmed = false; s->done = true; }
            DestroyWindow(hWnd);
            return 0;
        case WM_NCDESTROY:
            if (s) {
                if (s->pSpectrumBmp) { s->pSpectrumBmp->Release(); s->pSpectrumBmp = nullptr; }
                if (s->pRT) { s->pRT->Release(); s->pRT = nullptr; }
                s->done = true;
            }
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
            return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static bool PickRecColorViaWinUI3(HWND hOwner, COLORREF initRgb, bool isLight, bool isVi, UINT dpi, COLORREF* outRgb) {
    static constexpr const wchar_t* kColorCls = L"WindhawkScreenRecColorPicker_v2";
    WNDCLASSEXW wc{ sizeof(wc) };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = RecColorPickerWndProc;
    wc.hInstance = GetModInstanceHandle();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kColorCls;
    RegisterClassExW(&wc);

    RecColorPickerSession sess;
    sess.dpi = dpi ? dpi : 96;
    sess.isLight = isLight;
    sess.isVi = isVi;
    RgbToHsv(initRgb, sess.hue, sess.sat, sess.val);

    int cw = DpiScale(364, sess.dpi), ch = DpiScale(412, sess.dpi);
    DWORD style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
    RECT rcAdj{ 0, 0, cw, ch };
    AdjustWindowRectEx(&rcAdj, style, FALSE, WS_EX_DLGMODALFRAME);
    int ww = static_cast<int>(rcAdj.right - rcAdj.left);
    int wh = static_cast<int>(rcAdj.bottom - rcAdj.top);
    RECT rcOwn{};
    int px = 160, py = 120;
    if (hOwner && GetWindowRect(hOwner, &rcOwn)) {
        px = static_cast<int>(rcOwn.left) + (static_cast<int>(rcOwn.right - rcOwn.left) - ww) / 2;
        py = static_cast<int>(rcOwn.top)  + (static_cast<int>(rcOwn.bottom - rcOwn.top) - wh) / 2;
    }

    HWND hDlg = CreateWindowExW(WS_EX_DLGMODALFRAME, kColorCls,
        isVi ? L"Chọn màu (WinUI 3)" : L"Pick a Color",
        style, px, py, ww, wh, hOwner, nullptr, GetModInstanceHandle(), &sess);
    if (!hDlg) return false;

    if (hOwner) EnableWindow(hOwner, FALSE);
    ShowWindow(hDlg, SW_SHOWNORMAL);
    UpdateWindow(hDlg);
    SetForegroundWindow(hDlg);

    MSG msg{};
    while (!sess.done && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_QUIT) { PostQuitMessage(static_cast<int>(msg.wParam)); break; }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (hOwner && IsWindow(hOwner)) { EnableWindow(hOwner, TRUE); SetForegroundWindow(hOwner); }
    if (sess.confirmed && outRgb) { *outRgb = HsvToRgb(sess.hue, sess.sat, sess.val); return true; }
    return false;
}

struct RecComboFlyoutSession {
    HWND hWnd = nullptr;
    UINT dpi = 96;
    bool isLight = false;
    std::vector<std::pair<int, std::wstring>> items;
    int checkedId = 0, hoveredIdx = -1, scrollY = 0, maxScrollY = 0, chosenId = 0;
    bool done = false;
    ID2D1HwndRenderTarget* pRT = nullptr;
};

static void PaintRecComboFlyout(HWND hWnd, RecComboFlyoutSession* s) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rc{};
    GetClientRect(hWnd, &rc);
    int w = static_cast<int>(rc.right - rc.left), h = static_cast<int>(rc.bottom - rc.top);
    if (!s || w <= 0 || h <= 0 || !g_pD2DFactory || !g_pDWriteFactory) { EndPaint(hWnd, &ps); return; }

    D2D1_SIZE_U sz = D2D1::SizeU(static_cast<UINT32>(w), static_cast<UINT32>(h));
    if (!s->pRT) {
        D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 96.0f, 96.0f);
        D2D1_HWND_RENDER_TARGET_PROPERTIES hrtp = D2D1::HwndRenderTargetProperties(hWnd, sz, D2D1_PRESENT_OPTIONS_IMMEDIATELY);
        if (FAILED(g_pD2DFactory->CreateHwndRenderTarget(rtp, hrtp, &s->pRT))) s->pRT = nullptr;
    }
    if (!s->pRT) { EndPaint(hWnd, &ps); return; }

    auto S = [&](float v) { return DpiScaleF(v, s->dpi); };
    bool isLight = s->isLight;
    D2D1_COLOR_F colBg     = isLight ? D2D1::ColorF(0.985f, 0.986f, 0.992f, 1.0f) : D2D1::ColorF(0.172f, 0.175f, 0.190f, 1.0f);
    D2D1_COLOR_F colBorder = isLight ? D2D1::ColorF(0.865f, 0.872f, 0.895f, 1.0f) : D2D1::ColorF(0.270f, 0.276f, 0.300f, 1.0f);
    D2D1_COLOR_F colRowHov = isLight ? D2D1::ColorF(0.928f, 0.934f, 0.950f, 1.0f) : D2D1::ColorF(0.238f, 0.242f, 0.262f, 1.0f);
    D2D1_COLOR_F colRowSel = isLight ? D2D1::ColorF(0.936f, 0.942f, 0.958f, 1.0f) : D2D1::ColorF(0.222f, 0.226f, 0.246f, 1.0f);
    D2D1_COLOR_F colText   = isLight ? D2D1::ColorF(0.100f, 0.105f, 0.120f, 1.0f) : D2D1::ColorF(0.960f, 0.965f, 0.978f, 1.0f);
    D2D1_COLOR_F colAccent = GetWindowsSystemAccentColor(isLight);

    ID2D1HwndRenderTarget* pRT = s->pRT;
    pRT->BeginDraw();
    pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
    pRT->Clear(colBg);

    ID2D1SolidColorBrush *pBrBorder = nullptr, *pBrHov = nullptr, *pBrSel = nullptr, *pBrText = nullptr, *pBrAccent = nullptr;
    pRT->CreateSolidColorBrush(colBorder, &pBrBorder);
    pRT->CreateSolidColorBrush(colRowHov, &pBrHov);
    pRT->CreateSolidColorBrush(colRowSel, &pBrSel);
    pRT->CreateSolidColorBrush(colText, &pBrText);
    pRT->CreateSolidColorBrush(colAccent, &pBrAccent);

    IDWriteTextFormat* pFmt = nullptr;
    g_pDWriteFactory->CreateTextFormat(GetPreferredFluentFontFamily(), nullptr,
        DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(13.5f), L"", &pFmt);
    if (pFmt) { pFmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); pFmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP); }

    float pad = S(5.0f), rowH = S(36.0f);
    float curY = pad - static_cast<float>(s->scrollY);
    for (int i = 0; i < static_cast<int>(s->items.size()); ++i) {
        D2D1_RECT_F rRow = D2D1::RectF(pad, curY, static_cast<float>(w) - pad, curY + rowH - S(2.0f));
        bool sel = (s->items[i].first == s->checkedId);
        bool hov = (i == s->hoveredIdx);
        if (hov || sel) pRT->FillRoundedRectangle(D2D1::RoundedRect(rRow, S(4.5f), S(4.5f)), hov ? pBrHov : pBrSel);
        if (sel && pBrAccent) {
            D2D1_RECT_F rPill = D2D1::RectF(rRow.left + S(1.0f), rRow.top + S(9.0f), rRow.left + S(4.0f), rRow.bottom - S(9.0f));
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rPill, S(1.5f), S(1.5f)), pBrAccent);
        }
        if (pFmt && pBrText) {
            pRT->DrawText(s->items[i].second.c_str(), static_cast<UINT32>(s->items[i].second.size()), pFmt,
                D2D1::RectF(rRow.left + S(12.0f), rRow.top, rRow.right - S(10.0f), rRow.bottom), pBrText);
        }
        curY += rowH;
    }
    if (pBrBorder) pRT->DrawRectangle(D2D1::RectF(0.5f, 0.5f, static_cast<float>(w) - 0.5f, static_cast<float>(h) - 0.5f), pBrBorder, 1.0f);

    HRESULT hrEnd = pRT->EndDraw();
    if (pFmt) pFmt->Release();
    if (pBrBorder) pBrBorder->Release();
    if (pBrHov) pBrHov->Release();
    if (pBrSel) pBrSel->Release();
    if (pBrText) pBrText->Release();
    if (pBrAccent) pBrAccent->Release();
    if (hrEnd == D2DERR_RECREATE_TARGET) { s->pRT->Release(); s->pRT = nullptr; InvalidateRect(hWnd, nullptr, FALSE); }
    EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK RecComboFlyoutWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto* s = reinterpret_cast<RecComboFlyoutSession*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    switch (uMsg) {
        case WM_CREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            s = reinterpret_cast<RecComboFlyoutSession*>(cs->lpCreateParams);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(s));
            if (s) {
                s->hWnd = hWnd;
                BOOL dark = s->isLight ? FALSE : TRUE;
                DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
                DWORD corner = 2;
                DwmSetWindowAttribute(hWnd, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/, &corner, sizeof(corner));
            }
            return 0;
        }
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: PaintRecComboFlyout(hWnd, s); return 0;
        case WM_MOUSEWHEEL:
            if (s && s->maxScrollY > 0) {
                s->scrollY = std::clamp(s->scrollY - (GET_WHEEL_DELTA_WPARAM(wParam) / 3), 0, s->maxScrollY);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        case WM_MOUSEMOVE:
            if (s) {
                int y = static_cast<short>(HIWORD(lParam));
                int idx = static_cast<int>(std::floor((y + s->scrollY - DpiScaleF(5.0f, s->dpi)) / DpiScaleF(36.0f, s->dpi)));
                if (idx < 0 || idx >= static_cast<int>(s->items.size())) idx = -1;
                if (idx != s->hoveredIdx) { s->hoveredIdx = idx; InvalidateRect(hWnd, nullptr, FALSE); }
            }
            return 0;
        case WM_LBUTTONUP:
            if (s) {
                int y = static_cast<short>(HIWORD(lParam));
                int idx = static_cast<int>(std::floor((y + s->scrollY - DpiScaleF(5.0f, s->dpi)) / DpiScaleF(36.0f, s->dpi)));
                if (idx >= 0 && idx < static_cast<int>(s->items.size())) s->chosenId = s->items[idx].first;
                s->done = true;
                DestroyWindow(hWnd);
            }
            return 0;
        case WM_KEYDOWN:
            if (s && wParam == VK_ESCAPE) { s->done = true; DestroyWindow(hWnd); return 0; }
            break;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && s && !s->done) { s->done = true; DestroyWindow(hWnd); }
            return 0;
        case WM_NCDESTROY:
            if (s) { if (s->pRT) { s->pRT->Release(); s->pRT = nullptr; } s->done = true; }
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
            return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static int ShowRecComboFlyout(HWND hOwner, const RECT& rcCombo, const std::vector<std::pair<int, std::wstring>>& items,
                              int checkedId, bool isLight, UINT dpi) {
    if (items.empty()) return 0;
    static constexpr const wchar_t* kComboCls = L"WindhawkScreenRecComboFlyout_v2";
    WNDCLASSEXW wc{ sizeof(wc) };
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;
    wc.lpfnWndProc = RecComboFlyoutWndProc;
    wc.hInstance = GetModInstanceHandle();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kComboCls;
    RegisterClassExW(&wc);

    RecComboFlyoutSession sess;
    sess.dpi = dpi ? dpi : 96;
    sess.isLight = isLight;
    sess.items = items;
    sess.checkedId = checkedId;

    int padPx = DpiScale(5, sess.dpi), rowPx = DpiScale(36, sess.dpi);
    int totalH = padPx * 2 + rowPx * static_cast<int>(items.size());
    int popH = std::min(totalH, DpiScale(372, sess.dpi));
    sess.maxScrollY = std::max(0, totalH - popH);
    int comboW = static_cast<int>(rcCombo.right - rcCombo.left);
    int popW = std::max(comboW + DpiScale(36, sess.dpi), DpiScale(250, sess.dpi));

    POINT ptTopLeft{ rcCombo.left, rcCombo.bottom + DpiScale(4, sess.dpi) };
    ClientToScreen(hOwner, &ptTopLeft);
    HMONITOR hMon = MonitorFromPoint(ptTopLeft, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    if (GetMonitorInfoW(hMon, &mi)) {
        if (static_cast<int>(ptTopLeft.y) + popH > static_cast<int>(mi.rcWork.bottom) - 8) {
            POINT ptAbove{ rcCombo.left, rcCombo.top - DpiScale(4, sess.dpi) };
            ClientToScreen(hOwner, &ptAbove);
            ptTopLeft.y = std::max(static_cast<int>(mi.rcWork.top) + 8, static_cast<int>(ptAbove.y) - popH);
        }
        if (static_cast<int>(ptTopLeft.x) + popW > static_cast<int>(mi.rcWork.right) - 8) {
            ptTopLeft.x = static_cast<int>(mi.rcWork.right) - 8 - popW;
        }
        if (static_cast<int>(ptTopLeft.x) < static_cast<int>(mi.rcWork.left) + 8) {
            ptTopLeft.x = static_cast<int>(mi.rcWork.left) + 8;
        }
    }

    HWND hPop = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, kComboCls, L"", WS_POPUP,
        ptTopLeft.x, ptTopLeft.y, popW, popH, hOwner, nullptr, GetModInstanceHandle(), &sess);
    if (!hPop) return 0;

    ShowWindow(hPop, SW_SHOWNORMAL);
    UpdateWindow(hPop);
    SetForegroundWindow(hPop);

    MSG msg{};
    while (!sess.done && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_QUIT) { PostQuitMessage(static_cast<int>(msg.wParam)); break; }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return sess.chosenId;
}

static void DrawWin11NativeToggle(ID2D1RenderTarget* pRT, float rightX, float cy, UINT dpi,
                                  bool checked, bool isLight, bool hovered, D2D1_COLOR_F accentCol) {
    float swW = DpiScaleF(40.0f, dpi), swH = DpiScaleF(20.0f, dpi);
    float leftX = rightX - swW, r = swH * 0.5f;
    D2D1_RECT_F rc = D2D1::RectF(leftX, cy - r, rightX, cy + r);
    D2D1_COLOR_F trackCol = checked ? accentCol
        : (isLight ? D2D1::ColorF(0.95f, 0.95f, 0.96f, 1.0f) : D2D1::ColorF(0.18f, 0.18f, 0.20f, 1.0f));
    D2D1_COLOR_F borderCol = checked ? trackCol
        : (isLight ? D2D1::ColorF(0.48f, 0.49f, 0.52f, 1.0f) : D2D1::ColorF(0.62f, 0.63f, 0.66f, 1.0f));
    D2D1_COLOR_F thumbCol = checked
        ? (isLight ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f) : D2D1::ColorF(0.06f, 0.06f, 0.08f, 1.0f))
        : (isLight ? D2D1::ColorF(0.36f, 0.37f, 0.40f, 1.0f) : D2D1::ColorF(0.82f, 0.83f, 0.86f, 1.0f));

    ID2D1SolidColorBrush *pTr = nullptr, *pBd = nullptr, *pTh = nullptr;
    if (SUCCEEDED(pRT->CreateSolidColorBrush(trackCol, &pTr))) { pRT->FillRoundedRectangle(D2D1::RoundedRect(rc, r, r), pTr); pTr->Release(); }
    if (!checked && SUCCEEDED(pRT->CreateSolidColorBrush(borderCol, &pBd))) { pRT->DrawRoundedRectangle(D2D1::RoundedRect(rc, r, r), pBd, DpiScaleF(1.2f, dpi)); pBd->Release(); }
    float thumbR = DpiScaleF(hovered ? 6.8f : 6.0f, dpi);
    float thumbCx = checked ? (rightX - r) : (leftX + r);
    if (SUCCEEDED(pRT->CreateSolidColorBrush(thumbCol, &pTh))) { pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(thumbCx, cy), thumbR, thumbR), pTh); pTh->Release(); }
}

static void ApplyRecSettingsWindowFrame(HWND hWnd, bool isLight) {
    BOOL dark = isLight ? FALSE : TRUE;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    DWORD corner = 2;
    DwmSetWindowAttribute(hWnd, 33 /*DWMWA_WINDOW_CORNER_PREFERENCE*/, &corner, sizeof(corner));
    COLORREF captionCol = isLight ? RGB(243, 244, 247) : RGB(32, 32, 34);
    DwmSetWindowAttribute(hWnd, 35 /*DWMWA_CAPTION_COLOR*/, &captionCol, sizeof(captionCol));
}

struct RecSettingsState {
    HWND hWnd = nullptr;
    UINT dpi = 96;
    bool isLight = false;
    int activeTab = 0; // 0 = Personalization, 1 = Recording & Display, 2 = Audio & Behavior, 3 = Language (21)
    int scrollY = 0;
    int maxScrollY = 0;
    int hoveredId = 0;
    ID2D1HwndRenderTarget* pHwndRT = nullptr;
    ModSettings settings;

    struct HitItem {
        int id = 0;
        RECT rc{};
        std::wstring strVal;
        int intVal = 0;
    };
    std::vector<HitItem> hits;
};

static std::wstring GetRecThemePresetLabel(const std::wstring& k, bool isVi) {
    if (k == L"liquid_glass")     return L"Liquid Glass";
    if (k == L"mica_alt")         return L"Windows 11 Mica";
    if (k == L"fluent_solid")     return L"Fluent Solid";
    if (k == L"classic_win11")    return L"Classic Win11";
    if (k == L"midnight_oled")    return L"Midnight OLED";
    if (k == L"nord_frost")       return L"Nord Frost";
    if (k == L"catppuccin_mocha") return L"Catppuccin Mocha";
    if (k == L"dracula_velvet")   return L"Dracula Velvet";
    if (k == L"rose_pine")        return L"Rosé Pine";
    if (k == L"emerald_slate")    return L"Emerald Slate";
    if (k == L"warm_paper")       return L"Warm Paper";
    if (k == L"custom")           return isVi ? L"Tự tùy chỉnh" : L"Custom";
    return L"Windows 11 Blur";
}

static void PaintRecSettingsWindow(HWND hWnd, RecSettingsState* st) {
    if (!hWnd) return;
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rcClient{};
    GetClientRect(hWnd, &rcClient);
    int w = static_cast<int>(rcClient.right - rcClient.left);
    int h = static_cast<int>(rcClient.bottom - rcClient.top);
    if (!st || w <= 0 || h <= 0 || !g_pD2DFactory || !g_pDWriteFactory) {
        HBRUSH hFb = CreateSolidBrush((st && st->isLight) ? RGB(243, 244, 247) : RGB(32, 32, 34));
        FillRect(hdc, &rcClient, hFb);
        DeleteObject(hFb);
        EndPaint(hWnd, &ps);
        return;
    }

    D2D1_SIZE_U sz = D2D1::SizeU(static_cast<UINT32>(w), static_cast<UINT32>(h));
    if (!st->pHwndRT) {
        D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 96.0f, 96.0f);
        D2D1_HWND_RENDER_TARGET_PROPERTIES hrtp = D2D1::HwndRenderTargetProperties(hWnd, sz, D2D1_PRESENT_OPTIONS_IMMEDIATELY);
        if (FAILED(g_pD2DFactory->CreateHwndRenderTarget(rtp, hrtp, &st->pHwndRT))) st->pHwndRT = nullptr;
    } else {
        D2D1_SIZE_U cur = st->pHwndRT->GetPixelSize();
        if (cur.width != sz.width || cur.height != sz.height) st->pHwndRT->Resize(sz);
    }

    ID2D1HwndRenderTarget* pRT = st->pHwndRT;
    if (!pRT) { EndPaint(hWnd, &ps); return; }

    pRT->BeginDraw();
    pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

    UINT dpi = st->dpi;
    auto S = [&](float v) { return DpiScaleF(v, dpi); };
    bool isLight = st->isLight;
    std::wstring langCode = ResolveEffectiveLanguageCode(st->settings.language);
    bool isVi = (langCode == L"vi");
    UiStrings ui = GetUiStrings(st->settings.language);

    D2D1_COLOR_F colBg        = isLight ? D2D1::ColorF(0.953f, 0.957f, 0.969f, 1.0f) : D2D1::ColorF(0.125f, 0.125f, 0.133f, 1.0f);
    D2D1_COLOR_F colSide      = isLight ? D2D1::ColorF(0.945f, 0.949f, 0.963f, 1.0f) : D2D1::ColorF(0.114f, 0.114f, 0.122f, 1.0f);
    D2D1_COLOR_F colCard      = isLight ? D2D1::ColorF(0.992f, 0.993f, 0.996f, 1.0f) : D2D1::ColorF(0.172f, 0.174f, 0.184f, 1.0f);
    D2D1_COLOR_F colCardHover = isLight ? D2D1::ColorF(0.968f, 0.971f, 0.980f, 1.0f) : D2D1::ColorF(0.208f, 0.212f, 0.224f, 1.0f);
    D2D1_COLOR_F colBorder    = isLight ? D2D1::ColorF(0.890f, 0.896f, 0.912f, 1.0f) : D2D1::ColorF(0.235f, 0.239f, 0.255f, 1.0f);
    D2D1_COLOR_F colDivider   = isLight ? D2D1::ColorF(0.915f, 0.920f, 0.935f, 1.0f) : D2D1::ColorF(0.225f, 0.229f, 0.242f, 1.0f);
    D2D1_COLOR_F colText      = isLight ? D2D1::ColorF(0.100f, 0.105f, 0.120f, 1.0f) : D2D1::ColorF(0.960f, 0.965f, 0.975f, 1.0f);
    D2D1_COLOR_F colSub       = isLight ? D2D1::ColorF(0.380f, 0.395f, 0.425f, 1.0f) : D2D1::ColorF(0.680f, 0.695f, 0.725f, 1.0f);
    D2D1_COLOR_F colAccent    = GetWindowsSystemAccentColor(isLight);
    D2D1_COLOR_F colNavSel    = isLight ? D2D1::ColorF(0.902f, 0.910f, 0.930f, 1.0f) : D2D1::ColorF(0.200f, 0.204f, 0.220f, 1.0f);
    D2D1_COLOR_F colCtlBg     = isLight ? D2D1::ColorF(1.000f, 1.000f, 1.000f, 1.0f) : D2D1::ColorF(0.218f, 0.222f, 0.236f, 1.0f);
    D2D1_COLOR_F colChipSel   = D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, isLight ? 0.13f : 0.20f);

    pRT->Clear(colBg);

    ID2D1SolidColorBrush *pBrSide = nullptr, *pBrCard = nullptr, *pBrCardHov = nullptr, *pBrBorder = nullptr, *pBrDiv = nullptr;
    ID2D1SolidColorBrush *pBrText = nullptr, *pBrSub = nullptr, *pBrAccent = nullptr, *pBrNavSel = nullptr;
    ID2D1SolidColorBrush *pBrCtlBg = nullptr, *pBrChipSel = nullptr;
    pRT->CreateSolidColorBrush(colSide, &pBrSide);
    pRT->CreateSolidColorBrush(colCard, &pBrCard);
    pRT->CreateSolidColorBrush(colCardHover, &pBrCardHov);
    pRT->CreateSolidColorBrush(colBorder, &pBrBorder);
    pRT->CreateSolidColorBrush(colDivider, &pBrDiv);
    pRT->CreateSolidColorBrush(colText, &pBrText);
    pRT->CreateSolidColorBrush(colSub, &pBrSub);
    pRT->CreateSolidColorBrush(colAccent, &pBrAccent);
    pRT->CreateSolidColorBrush(colNavSel, &pBrNavSel);
    pRT->CreateSolidColorBrush(colCtlBg, &pBrCtlBg);
    pRT->CreateSolidColorBrush(colChipSel, &pBrChipSel);

    const wchar_t* fontName = GetPreferredFluentFontFamily();
    IDWriteTextFormat *pFmtHeader = nullptr, *pFmtTitle = nullptr, *pFmtSub = nullptr;
    IDWriteTextFormat *pFmtNav = nullptr, *pFmtCtl = nullptr, *pFmtRight = nullptr, *pFmtGlyph = nullptr;
    g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(24.0f), L"", &pFmtHeader);
    g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR,   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(13.5f), L"", &pFmtTitle);
    g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR,   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(11.5f), L"", &pFmtSub);
    g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR,   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(13.5f), L"", &pFmtNav);
    g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR,   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(12.5f), L"", &pFmtCtl);
    g_pDWriteFactory->CreateTextFormat(fontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR,   DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(13.0f), L"", &pFmtRight);
    g_pDWriteFactory->CreateTextFormat(L"Segoe Fluent Icons", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, S(16.0f), L"", &pFmtGlyph);
    if (pFmtNav)   pFmtNav->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    if (pFmtCtl)   { pFmtCtl->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); pFmtCtl->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); }
    if (pFmtRight) { pFmtRight->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING); pFmtRight->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); }
    if (pFmtGlyph) { pFmtGlyph->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); pFmtGlyph->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); }

    st->hits.clear();
    auto addHit = [&](int id, const D2D1_RECT_F& r, const std::wstring& sv = L"", int iv = 0) {
        st->hits.push_back({ id, { static_cast<LONG>(r.left), static_cast<LONG>(r.top), static_cast<LONG>(r.right), static_cast<LONG>(r.bottom) }, sv, iv });
    };

    // Left Sidebar
    float sideW = S(238.0f);
    pRT->FillRectangle(D2D1::RectF(0, 0, sideW, static_cast<float>(h)), pBrSide);
    {
        D2D1_RECT_F rBadge = D2D1::RectF(S(16), S(16), S(54), S(54));
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rBadge, S(19), S(19)), pBrChipSel);
        if (pFmtGlyph) pRT->DrawText(L"\uE714", 1, pFmtGlyph, rBadge, pBrAccent);
        if (pFmtTitle) pRT->DrawText(ui.title, static_cast<UINT32>(wcslen(ui.title)), pFmtTitle, D2D1::RectF(S(64), S(18), sideW - S(10), S(38)), pBrText);
        const wchar_t* verStr = isVi ? L"Phiên bản 2.0.4" : L"Version 2.0.4";
        if (pFmtSub)   pRT->DrawText(verStr, static_cast<UINT32>(wcslen(verStr)), pFmtSub, D2D1::RectF(S(64), S(37), sideW - S(10), S(55)), pBrSub);
    }

    struct NavDef { int tab; const wchar_t* glyph; const wchar_t* label; };
    NavDef kNavs[] = {
        { 0, L"\uE790", isVi ? L"Cá nhân hoá" : L"Personalization" },
        { 1, L"\uE7F4", isVi ? L"Quay & Màn hình" : L"Recording & Display" },
        { 2, L"\uE767", isVi ? L"Âm thanh & Tiện ích" : L"Audio & Behavior" },
        { 3, L"\uE774", isVi ? L"Ngôn ngữ (21)" : L"Language (21)" }
    };
    float navY = S(76.0f);
    for (const auto& nv : kNavs) {
        D2D1_RECT_F rNav = D2D1::RectF(S(12), navY, sideW - S(12), navY + S(38));
        int navId = 1000 + nv.tab;
        bool sel = (st->activeTab == nv.tab), hov = (st->hoveredId == navId);
        if (sel || hov) pRT->FillRoundedRectangle(D2D1::RoundedRect(rNav, S(5), S(5)), sel ? pBrNavSel : pBrCardHov);
        if (sel) pRT->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(rNav.left + S(1), rNav.top + S(10), rNav.left + S(4), rNav.bottom - S(10)), S(1.5f), S(1.5f)), pBrAccent);
        if (pFmtGlyph) pRT->DrawText(nv.glyph, 1, pFmtGlyph, D2D1::RectF(rNav.left + S(10), rNav.top, rNav.left + S(38), rNav.bottom), sel ? pBrAccent : pBrSub);
        if (pFmtNav)   pRT->DrawText(nv.label, static_cast<UINT32>(wcslen(nv.label)), pFmtNav, D2D1::RectF(rNav.left + S(44), rNav.top, rNav.right - S(8), rNav.bottom), pBrText);
        addHit(navId, rNav, L"", nv.tab);
        navY += S(42.0f);
    }

    // Right Content Area
    float contentX = sideW + S(26.0f);
    float contentW = std::max(S(360.0f), static_cast<float>(w) - contentX - S(30.0f));
    pRT->PushAxisAlignedClip(D2D1::RectF(sideW, 0, static_cast<float>(w), static_cast<float>(h)), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    float curY = S(20.0f) - static_cast<float>(st->scrollY);
    std::wstring hdr = std::wstring(ui.title) + L"  \u203A  " + kNavs[st->activeTab].label;
    if (pFmtHeader) pRT->DrawText(hdr.c_str(), static_cast<UINT32>(hdr.size()), pFmtHeader, D2D1::RectF(contentX, curY, contentX + contentW, curY + S(38)), pBrText);
    curY += S(52.0f);

    auto beginGroupCard = [&](const wchar_t* gt, const wchar_t* gs, int rowCount, float rowH) -> std::pair<D2D1_RECT_F, float> {
        float headerH = S(62.0f);
        float totalH = headerH + rowCount * rowH;
        D2D1_RECT_F rc = D2D1::RectF(contentX, curY, contentX + contentW, curY + totalH);
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rc, S(6), S(6)), pBrCard);
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rc, S(6), S(6)), pBrBorder, 1.0f);
        if (pFmtTitle) pRT->DrawText(gt, static_cast<UINT32>(wcslen(gt)), pFmtTitle, D2D1::RectF(rc.left + S(18), rc.top + S(13), rc.right - S(42), rc.top + S(33)), pBrText);
        if (pFmtSub)   pRT->DrawText(gs, static_cast<UINT32>(wcslen(gs)), pFmtSub,   D2D1::RectF(rc.left + S(18), rc.top + S(34), rc.right - S(42), rc.top + S(54)), pBrSub);
        if (pFmtGlyph) pRT->DrawText(L"\uE70E", 1, pFmtGlyph, D2D1::RectF(rc.right - S(40), rc.top + S(14), rc.right - S(14), rc.top + S(48)), pBrSub);
        curY += totalH + S(12.0f);
        return { rc, rc.top + headerH };
    };

    auto drawToggleRow = [&](const D2D1_RECT_F& grpRc, float rowTop, float rowH, int id,
                             const wchar_t* glyph, const wchar_t* title, const wchar_t* desc, bool checked) {
        pRT->DrawLine(D2D1::Point2F(grpRc.left + 1.0f, rowTop), D2D1::Point2F(grpRc.right - 1.0f, rowTop), pBrDiv, 1.0f);
        D2D1_RECT_F rRow = D2D1::RectF(grpRc.left + 1.0f, rowTop + 1.0f, grpRc.right - 1.0f, rowTop + rowH - 1.0f);
        bool hov = (st->hoveredId == id);
        if (hov) pRT->FillRectangle(rRow, pBrCardHov);
        if (pFmtGlyph) pRT->DrawText(glyph, 1, pFmtGlyph, D2D1::RectF(grpRc.left + S(26), rowTop, grpRc.left + S(56), rowTop + rowH), pBrSub);
        if (pFmtTitle) pRT->DrawText(title, static_cast<UINT32>(wcslen(title)), pFmtTitle, D2D1::RectF(grpRc.left + S(64), rowTop + S(11), grpRc.right - S(124), rowTop + S(31)), pBrText);
        if (pFmtSub)   pRT->DrawText(desc,  static_cast<UINT32>(wcslen(desc)),  pFmtSub,   D2D1::RectF(grpRc.left + S(64), rowTop + S(32), grpRc.right - S(124), rowTop + rowH - S(6)), pBrSub);
        const wchar_t* sTxt = checked ? (isVi ? L"Bật" : L"On") : (isVi ? L"Tắt" : L"Off");
        if (pFmtRight) pRT->DrawText(sTxt, static_cast<UINT32>(wcslen(sTxt)), pFmtRight, D2D1::RectF(grpRc.right - S(118), rowTop, grpRc.right - S(68), rowTop + rowH), pBrText);
        DrawWin11NativeToggle(pRT, grpRc.right - S(18.0f), rowTop + rowH * 0.5f, dpi, checked, isLight, hov, colAccent);
        addHit(id, rRow);
    };

    auto drawDropdownRow = [&](const D2D1_RECT_F& grpRc, float rowTop, float rowH, int id,
                               const wchar_t* glyph, const wchar_t* title, const wchar_t* desc, const std::wstring& valLbl) {
        pRT->DrawLine(D2D1::Point2F(grpRc.left + 1.0f, rowTop), D2D1::Point2F(grpRc.right - 1.0f, rowTop), pBrDiv, 1.0f);
        if (pFmtGlyph) pRT->DrawText(glyph, 1, pFmtGlyph, D2D1::RectF(grpRc.left + S(26), rowTop, grpRc.left + S(56), rowTop + rowH), pBrSub);
        if (pFmtTitle) pRT->DrawText(title, static_cast<UINT32>(wcslen(title)), pFmtTitle, D2D1::RectF(grpRc.left + S(64), rowTop + S(11), grpRc.right - S(225), rowTop + S(31)), pBrText);
        if (pFmtSub)   pRT->DrawText(desc,  static_cast<UINT32>(wcslen(desc)),  pFmtSub,   D2D1::RectF(grpRc.left + S(64), rowTop + S(32), grpRc.right - S(225), rowTop + rowH - S(6)), pBrSub);
        D2D1_RECT_F rcCombo = D2D1::RectF(grpRc.right - S(212), rowTop + S(14), grpRc.right - S(18), rowTop + rowH - S(14));
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcCombo, S(4), S(4)), (st->hoveredId == id) ? pBrCardHov : pBrCtlBg);
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcCombo, S(4), S(4)), pBrBorder, 1.0f);
        if (pFmtNav) pRT->DrawText(valLbl.c_str(), static_cast<UINT32>(valLbl.size()), pFmtNav,
            D2D1::RectF(rcCombo.left + S(12), rcCombo.top, rcCombo.right - S(28), rcCombo.bottom), pBrText, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        if (pFmtGlyph) pRT->DrawText(L"\uE70D", 1, pFmtGlyph, D2D1::RectF(rcCombo.right - S(26), rcCombo.top, rcCombo.right - S(6), rcCombo.bottom), pBrSub);
        addHit(id, rcCombo);
    };

    auto drawColorRow = [&](const D2D1_RECT_F& grpRc, float rowTop, float rowH, int pickId, int resetId,
                            const wchar_t* glyph, const wchar_t* title, const wchar_t* desc, const std::wstring& hexVal) {
        pRT->DrawLine(D2D1::Point2F(grpRc.left + 1.0f, rowTop), D2D1::Point2F(grpRc.right - 1.0f, rowTop), pBrDiv, 1.0f);
        if (pFmtGlyph) pRT->DrawText(glyph, 1, pFmtGlyph, D2D1::RectF(grpRc.left + S(26), rowTop, grpRc.left + S(56), rowTop + rowH), pBrSub);
        if (pFmtTitle) pRT->DrawText(title, static_cast<UINT32>(wcslen(title)), pFmtTitle, D2D1::RectF(grpRc.left + S(64), rowTop + S(11), grpRc.right - S(215), rowTop + S(31)), pBrText);
        if (pFmtSub)   pRT->DrawText(desc,  static_cast<UINT32>(wcslen(desc)),  pFmtSub,   D2D1::RectF(grpRc.left + S(64), rowTop + S(32), grpRc.right - S(215), rowTop + rowH - S(6)), pBrSub);
        D2D1_RECT_F rcReset = D2D1::RectF(grpRc.right - S(86), rowTop + S(15), grpRc.right - S(18), rowTop + rowH - S(15));
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcReset, S(4), S(4)), st->hoveredId == resetId ? pBrCardHov : pBrCtlBg);
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcReset, S(4), S(4)), pBrBorder, 1.0f);
        const wchar_t* rstTxt = isVi ? L"Mặc định" : L"Default";
        if (pFmtCtl) pRT->DrawText(rstTxt, static_cast<UINT32>(wcslen(rstTxt)), pFmtCtl, rcReset, pBrText);
        addHit(resetId, rcReset);

        D2D1_RECT_F rcPick = D2D1::RectF(rcReset.left - S(106), rowTop + S(15), rcReset.left - S(8), rowTop + rowH - S(15));
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcPick, S(4), S(4)), st->hoveredId == pickId ? pBrCardHov : pBrCtlBg);
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcPick, S(4), S(4)), pBrBorder, 1.0f);
        COLORREF parsedRgb = 0;
        if (!hexVal.empty() && TryParseHexColor(hexVal, &parsedRgb)) {
            ID2D1SolidColorBrush* pSw = nullptr;
            if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(GetRValue(parsedRgb)/255.0f, GetGValue(parsedRgb)/255.0f, GetBValue(parsedRgb)/255.0f, 1.0f), &pSw))) {
                D2D1_RECT_F rcSw = D2D1::RectF(rcPick.left + S(8), rcPick.top + S(6), rcPick.left + S(24), rcPick.bottom - S(6));
                pRT->FillRoundedRectangle(D2D1::RoundedRect(rcSw, S(3), S(3)), pSw);
                pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcSw, S(3), S(3)), pBrBorder, 1.0f);
                pSw->Release();
            }
            if (pFmtCtl) pRT->DrawText(hexVal.c_str(), static_cast<UINT32>(hexVal.size()), pFmtCtl, D2D1::RectF(rcPick.left + S(26), rcPick.top, rcPick.right - S(4), rcPick.bottom), pBrText);
        } else {
            const wchar_t* pkTxt = isVi ? L"Chọn màu..." : L"Pick color...";
            if (pFmtCtl) pRT->DrawText(pkTxt, static_cast<UINT32>(wcslen(pkTxt)), pFmtCtl, rcPick, pBrText);
        }
        addHit(pickId, rcPick);
    };

    auto drawStepperRow = [&](const D2D1_RECT_F& grpRc, float rowTop, float rowH, int minusId, int plusId, int resetId,
                              const wchar_t* glyph, const wchar_t* title, const wchar_t* desc, const std::wstring& valStr) {
        pRT->DrawLine(D2D1::Point2F(grpRc.left + 1.0f, rowTop), D2D1::Point2F(grpRc.right - 1.0f, rowTop), pBrDiv, 1.0f);
        if (pFmtGlyph) pRT->DrawText(glyph, 1, pFmtGlyph, D2D1::RectF(grpRc.left + S(26), rowTop, grpRc.left + S(56), rowTop + rowH), pBrSub);
        if (pFmtTitle) pRT->DrawText(title, static_cast<UINT32>(wcslen(title)), pFmtTitle, D2D1::RectF(grpRc.left + S(64), rowTop + S(11), grpRc.right - S(215), rowTop + S(31)), pBrText);
        if (pFmtSub)   pRT->DrawText(desc,  static_cast<UINT32>(wcslen(desc)),  pFmtSub,   D2D1::RectF(grpRc.left + S(64), rowTop + S(32), grpRc.right - S(215), rowTop + rowH - S(6)), pBrSub);
        D2D1_RECT_F rcReset = D2D1::RectF(grpRc.right - S(72), rowTop + S(15), grpRc.right - S(18), rowTop + rowH - S(15));
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcReset, S(4), S(4)), st->hoveredId == resetId ? pBrCardHov : pBrCtlBg);
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcReset, S(4), S(4)), pBrBorder, 1.0f);
        if (pFmtCtl) pRT->DrawText(L"Auto", 4, pFmtCtl, rcReset, pBrText);
        addHit(resetId, rcReset);
        D2D1_RECT_F rcPlus = D2D1::RectF(rcReset.left - S(34), rowTop + S(15), rcReset.left - S(6), rowTop + rowH - S(15));
        D2D1_RECT_F rcVal  = D2D1::RectF(rcPlus.left - S(58), rowTop + S(15), rcPlus.left - S(4), rowTop + rowH - S(15));
        D2D1_RECT_F rcMin  = D2D1::RectF(rcVal.left - S(30), rowTop + S(15), rcVal.left - S(4), rowTop + rowH - S(15));
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcMin, S(4), S(4)), st->hoveredId == minusId ? pBrCardHov : pBrCtlBg);
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcMin, S(4), S(4)), pBrBorder, 1.0f);
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcPlus, S(4), S(4)), st->hoveredId == plusId ? pBrCardHov : pBrCtlBg);
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcPlus, S(4), S(4)), pBrBorder, 1.0f);
        if (pFmtCtl) {
            pRT->DrawText(L"\u2212", 1, pFmtCtl, rcMin, pBrText);
            pRT->DrawText(valStr.c_str(), static_cast<UINT32>(valStr.size()), pFmtCtl, rcVal, pBrText);
            pRT->DrawText(L"+", 1, pFmtCtl, rcPlus, pBrText);
        }
        addHit(minusId, rcMin);
        addHit(plusId, rcPlus);
    };

    auto drawActionRow = [&](const D2D1_RECT_F& grpRc, float rowTop, float rowH, int id,
                             const wchar_t* glyph, const wchar_t* title, const wchar_t* desc, const wchar_t* btnTxt) {
        pRT->DrawLine(D2D1::Point2F(grpRc.left + 1.0f, rowTop), D2D1::Point2F(grpRc.right - 1.0f, rowTop), pBrDiv, 1.0f);
        if (pFmtGlyph) pRT->DrawText(glyph, 1, pFmtGlyph, D2D1::RectF(grpRc.left + S(26), rowTop, grpRc.left + S(56), rowTop + rowH), pBrSub);
        if (pFmtTitle) pRT->DrawText(title, static_cast<UINT32>(wcslen(title)), pFmtTitle, D2D1::RectF(grpRc.left + S(64), rowTop + S(11), grpRc.right - S(160), rowTop + S(31)), pBrText);
        if (pFmtSub)   pRT->DrawText(desc,  static_cast<UINT32>(wcslen(desc)),  pFmtSub,   D2D1::RectF(grpRc.left + S(64), rowTop + S(32), grpRc.right - S(160), rowTop + rowH - S(6)), pBrSub);
        D2D1_RECT_F rcBtn = D2D1::RectF(grpRc.right - S(148), rowTop + S(14), grpRc.right - S(18), rowTop + rowH - S(14));
        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcBtn, S(4), S(4)), (st->hoveredId == id) ? pBrCardHov : pBrCtlBg);
        pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcBtn, S(4), S(4)), pBrBorder, 1.0f);
        if (pFmtCtl) pRT->DrawText(btnTxt, static_cast<UINT32>(wcslen(btnTxt)), pFmtCtl, rcBtn, pBrText);
        addHit(id, rcBtn);
    };

    float rH = S(60.0f);
    if (st->activeTab == 0) {
        auto [g1, y1] = beginGroupCard(
            isVi ? L"Chủ đề & Giao diện bề mặt" : L"Theme & Surface",
            isVi ? L"Chọn trong 13 chủ đề đồng bộ giống Modern Run (Windows 11 Blur & Liquid Glass đã tối ưu)" : L"Choose from 13 built-in themes matching Modern Run (Windows 11 Blur & Liquid Glass tuned)",
            2, rH);
        drawDropdownRow(g1, y1, rH, 2000, L"\uE790",
            isVi ? L"Chủ đề giao diện (Theme Preset)" : L"Theme Preset",
            isVi ? L"Chuyển đổi giữa 13 bộ giao diện có sẵn" : L"Switch between 13 built-in themes",
            GetRecThemePresetLabel(st->settings.themePreset, isVi));
        std::wstring cmLbl = (st->settings.colorMode == ColorMode::Dark) ? (isVi ? L"Tối (Dark)" : L"Dark")
                           : ((st->settings.colorMode == ColorMode::Light) ? (isVi ? L"Sáng (Light)" : L"Light")
                                                                           : (isVi ? L"Tự động" : L"Auto"));
        drawDropdownRow(g1, y1 + rH, rH, 2100, L"\uE706",
            ui.mColor,
            isVi ? L"Tự động theo Windows hoặc chọn cố định" : L"Follow Windows theme or pick Dark / Light",
            cmLbl);

        auto [g2, y2] = beginGroupCard(
            isVi ? L"Màu sắc & Bo góc tùy chỉnh" : L"Custom Colors & Corner Radius",
            isVi ? L"Tùy biến màu nền, màu viền và bán kính bo góc cho thanh viên thuốc" : L"Override background color, border rim, and corner roundness",
            3, rH);
        drawColorRow(g2, y2, rH, 2201, 2202, L"\uE790",
            isVi ? L"Màu nền thanh quay" : L"Pill Background Color",
            isVi ? L"Để Mặc định nếu dùng theo Theme Preset" : L"Leave Default to follow Theme Preset",
            st->settings.customBgColor);
        drawColorRow(g2, y2 + rH, rH, 2203, 2204, L"\uE91B",
            isVi ? L"Màu viền thanh quay" : L"Pill Border Color",
            isVi ? L"Đổi màu viền quanh thanh viên thuốc & thanh vẽ" : L"Override border rim color on pill & drawing bar",
            st->settings.customBorderColor);
        std::wstring crStr = (st->settings.customCornerRadius < 0)
            ? (isVi ? L"Mặc định" : L"Default")
            : (std::to_wstring(st->settings.customCornerRadius) + L" px");
        drawStepperRow(g2, y2 + rH * 2, rH, 2210, 2211, 2212, L"\uE744",
            isVi ? L"Bán kính bo góc (Border Radius)" : L"Border Radius",
            isVi ? L"0 px = vuông, 14 px = Win11, 27 px = viên thuốc" : L"0 px = square, 14 px = Win11, 27 px = pill",
            crStr);
    } else if (st->activeTab == 1) {
        RECT monRect = GetCaptureTargetRect(st->settings.captureMonitorMode);
        int hwW = static_cast<int>(monRect.right - monRect.left) & ~1;
        int hwH = static_cast<int>(monRect.bottom - monRect.top) & ~1;
        int outW = 0, outH = 0;
        ComputeCappedOutputSize(hwW, hwH, st->settings.targetResHeight, outW, outH);
        wchar_t resLbl[64] = {};
        if (st->settings.targetResHeight == 0 || st->settings.targetResHeight >= hwH)
            swprintf_s(resLbl, L"%s (%d\u00D7%d)", ui.mResNativePrefix, outW, outH);
        else
            swprintf_s(resLbl, L"%dp (%d\u00D7%d)", outH, outW, outH);

        int hwHz = GetMonitorRefreshRateHz(st->settings.captureMonitorMode);
        int effFps = ResolveEffectiveFps(st->settings.fps, st->settings.captureMonitorMode);
        wchar_t fpsLbl[64] = {};
        swprintf_s(fpsLbl, L"%d FPS (Max %d Hz)", effFps, hwHz);

        wchar_t brLbl[32] = {};
        swprintf_s(brLbl, L"%d Mbps", st->settings.videoBitrateMbps);

        auto [g1, y1] = beginGroupCard(
            isVi ? L"Độ phân giải, FPS & Chất lượng" : L"Resolution, FPS & Quality",
            isVi ? L"Tự động giới hạn theo độ phân giải và tần số quét Windows hiện tại" : L"Automatically capped to your monitor hardware & active refresh rate",
            3, rH);
        drawDropdownRow(g1, y1, rH, 2301, L"\uE7F4",
            ui.mResolution,
            isVi ? L"Chỉ hiển thị các mức bằng hoặc thấp hơn phần cứng" : L"Only offers Native or lower downscaled presets",
            resLbl);
        drawDropdownRow(g1, y1 + rH, rH, 2302, L"\uE916",
            ui.mFps,
            isVi ? L"Chỉ mở mức FPS tối đa bằng tần số quét màn hình đang đặt" : L"Capped to active Windows refresh rate with GPU fallback",
            fpsLbl);
        drawDropdownRow(g1, y1 + rH * 2, rH, 2303, L"\uE714",
            ui.mBitrate,
            isVi ? L"Bitrate cao hơn cho video sắc nét hơn (6 – 35 Mbps)" : L"Higher bitrate produces crisper MP4 output (6 – 35 Mbps)",
            brLbl);

        std::wstring monLbl = ui.mMonCursor;
        int selIdx = ParseMonitorIndexMode(st->settings.captureMonitorMode);
        if (st->settings.captureMonitorMode == L"all") monLbl = ui.mMonAll;
        else if (st->settings.captureMonitorMode == L"custom") monLbl = ui.layoutTitle;
        else if (selIdx >= 0) {
            wchar_t mb[48] = {};
            swprintf_s(mb, ui.mMonItemFmt, selIdx + 1);
            monLbl = mb;
        } else if (st->settings.captureMonitorMode == L"primary") monLbl = ui.mMonPrimary;

        auto [g2, y2] = beginGroupCard(
            isVi ? L"Màn hình quay & Bố trí" : L"Capture Monitor & Layout",
            isVi ? L"Chọn màn hình riêng lẻ, gộp tất cả hoặc tự kéo thả bố trí" : L"Pick an individual display, combine all, or arrange a custom layout",
            2, rH);
        drawDropdownRow(g2, y2, rH, 2310, L"\uE7F4",
            ui.mMonitor,
            isVi ? L"Áp dụng cho chế độ quay toàn màn hình" : L"Applies to full-screen recording mode",
            monLbl);
        drawActionRow(g2, y2 + rH, rH, 2311, L"\uE740",
            ui.layoutTitle,
            isVi ? L"Mở khung kéo thả sắp xếp vị trí các màn hình (Xếp ngang, Dọc, PiP)" : L"Open interactive drag-and-drop multi-monitor layout editor",
            isVi ? L"Mở bố trí..." : L"Arrange...");
    } else if (st->activeTab == 2) {
        auto [g1, y1] = beginGroupCard(
            isVi ? L"Âm thanh, Con trỏ & Hành vi" : L"Audio, Cursor & Behavior",
            isVi ? L"Bật/tắt thu âm WASAPI, con trỏ chuột và tự mở thư mục khi xong" : L"Configure WASAPI audio loopback, microphone, cursor, and post-record actions",
            6, rH);
        drawToggleRow(g1, y1, rH, 2401, L"\uE767",
            ui.mSysAudio,
            isVi ? L"Thu âm thanh phát ra từ máy tính qua WASAPI Loopback" : L"Capture desktop audio via WASAPI Loopback",
            st->settings.recordSystemAudio);
        drawToggleRow(g1, y1 + rH, rH, 2402, L"\uE720",
            ui.mMicAudio,
            isVi ? L"Trộn thêm giọng nói từ Micro mặc định vào video" : L"Mix default Microphone input into the MP4",
            st->settings.recordMicrophone);
        drawToggleRow(g1, y1 + rH * 2, rH, 2403, L"\uE8B0",
            ui.mCursor,
            isVi ? L"Ghi lại hình ảnh con trỏ chuột trên màn hình" : L"Include the mouse cursor in recorded frames",
            st->settings.recordCursor);
        drawToggleRow(g1, y1 + rH * 3, rH, 2404, L"\uE8B7",
            ui.mOpenOnFinish,
            isVi ? L"Tự động mở File Explorer và chọn sẵn tệp MP4 vừa quay" : L"Reveal the newly saved MP4 in File Explorer when stopped",
            st->settings.openFolderOnFinish);
        drawToggleRow(g1, y1 + rH * 4, rH, 2405, L"\uE78B",
            isVi ? L"Hiện thanh viên thuốc khi đang quay" : L"Show Floating Pill HUD While Recording",
            isVi ? L"Hiển thị đồng hồ đếm giờ, nút Tạm dừng/Dừng và Vẽ (ẩn khỏi video)" : L"Display timer, Pause/Stop, and Drawing toggle (excluded from video)",
            st->settings.showFloatingHud);
        drawActionRow(g1, y1 + rH * 5, rH, 2410, L"\uE8B7",
            ui.mOpenFolder,
            isVi ? L"Mở thư mục chứa các tệp MP4 đã quay" : L"Open the folder where MP4 recordings are saved",
            isVi ? L"Mở thư mục" : L"Open Folder");
    } else {
        struct LangItem { int id; const wchar_t* code; const wchar_t* label; };
        static constexpr LangItem kLangs[] = {
            { 2600, L"auto",  L"Auto (Windows)" }, { 2601, L"vi",    L"Tiếng Việt" },     { 2602, L"en",    L"English" },
            { 2603, L"zh_cn", L"简体中文" },        { 2604, L"zh_tw", L"繁體中文" },        { 2605, L"ja",    L"日本語" },
            { 2606, L"ko",    L"한국어" },          { 2607, L"th",    L"ไทย" },            { 2608, L"id",    L"Bahasa Indonesia" },
            { 2609, L"ms",    L"Bahasa Melayu" },  { 2610, L"fr",    L"Français" },       { 2611, L"de",    L"Deutsch" },
            { 2612, L"es",    L"Español" },        { 2613, L"pt",    L"Português" },      { 2614, L"it",    L"Italiano" },
            { 2615, L"ru",    L"Русский" },        { 2616, L"uk",    L"Українська" },     { 2617, L"pl",    L"Polski" },
            { 2618, L"nl",    L"Nederlands" },     { 2619, L"tr",    L"Türkçe" },         { 2620, L"hi",    L"हिन्दी" },
            { 2621, L"ar",    L"العربية" }
        };
        int cols = 3;
        int rows = (static_cast<int>(ARRAYSIZE(kLangs)) + cols - 1) / cols;
        float gridH = S(16.0f) + rows * S(38.0f);
        auto [g1, y1] = beginGroupCard(
            isVi ? L"Ngôn ngữ hiển thị (21 ngôn ngữ)" : L"Display Language (21 Languages)",
            isVi ? L"Áp dụng ngay cho cả thanh quay màn hình và cửa sổ Cài đặt" : L"Applies immediately to both the Pill Bar and Settings window",
            1, gridH);
        pRT->DrawLine(D2D1::Point2F(g1.left + 1.0f, y1), D2D1::Point2F(g1.right - 1.0f, y1), pBrDiv, 1.0f);
        float gx = g1.left + S(18.0f), gw = (g1.right - g1.left) - S(36.0f), gap = S(8.0f);
        float cw = (gw - gap * (cols - 1)) / static_cast<float>(cols), ch = S(30.0f);
        for (int i = 0; i < static_cast<int>(ARRAYSIZE(kLangs)); ++i) {
            int r = i / cols, c = i % cols;
            D2D1_RECT_F rcChip = D2D1::RectF(gx + c * (cw + gap), y1 + S(10) + r * S(38),
                                             gx + c * (cw + gap) + cw, y1 + S(10) + r * S(38) + ch);
            bool sel = (st->settings.language == kLangs[i].code);
            bool hov = (st->hoveredId == kLangs[i].id);
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rcChip, S(4), S(4)), sel ? pBrChipSel : (hov ? pBrCardHov : pBrCtlBg));
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcChip, S(4), S(4)), sel ? pBrAccent : pBrBorder, sel ? 1.5f : 1.0f);
            if (pFmtCtl) pRT->DrawText(kLangs[i].label, static_cast<UINT32>(wcslen(kLangs[i].label)), pFmtCtl, rcChip, sel ? pBrAccent : pBrText);
            addHit(kLangs[i].id, rcChip, kLangs[i].code);
        }
    }

    float totalContentH = (curY + static_cast<float>(st->scrollY)) + S(24.0f);
    st->maxScrollY = std::max(0, static_cast<int>(std::ceil(totalContentH - static_cast<float>(h))));
    st->scrollY = std::clamp(st->scrollY, 0, st->maxScrollY);

    pRT->PopAxisAlignedClip();
    HRESULT hrEnd = pRT->EndDraw();

    if (pFmtHeader) pFmtHeader->Release();
    if (pFmtTitle)  pFmtTitle->Release();
    if (pFmtSub)    pFmtSub->Release();
    if (pFmtNav)    pFmtNav->Release();
    if (pFmtCtl)    pFmtCtl->Release();
    if (pFmtRight)  pFmtRight->Release();
    if (pFmtGlyph)  pFmtGlyph->Release();
    if (pBrSide)    pBrSide->Release();
    if (pBrCard)    pBrCard->Release();
    if (pBrCardHov) pBrCardHov->Release();
    if (pBrBorder)  pBrBorder->Release();
    if (pBrDiv)     pBrDiv->Release();
    if (pBrText)    pBrText->Release();
    if (pBrSub)     pBrSub->Release();
    if (pBrAccent)  pBrAccent->Release();
    if (pBrNavSel)  pBrNavSel->Release();
    if (pBrCtlBg)   pBrCtlBg->Release();
    if (pBrChipSel) pBrChipSel->Release();

    if (hrEnd == D2DERR_RECREATE_TARGET) { st->pHwndRT->Release(); st->pHwndRT = nullptr; InvalidateRect(hWnd, nullptr, FALSE); }
    EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK RecWin11SettingsWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto* st = reinterpret_cast<RecSettingsState*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (uMsg == g_msgReloadSettings && g_msgReloadSettings != 0 && st) {
        LoadSettings(false);
        st->settings = GetSettingsCopy();
        st->isLight = (st->settings.colorMode == ColorMode::Light) ? true
                    : ((st->settings.colorMode == ColorMode::Dark) ? false : IsSystemAppLightTheme());
        ApplyRecSettingsWindowFrame(hWnd, st->isLight);
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    }
    switch (uMsg) {
        case WM_CREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            st = reinterpret_cast<RecSettingsState*>(cs->lpCreateParams);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(st));
            if (st) { st->hWnd = hWnd; ApplyRecSettingsWindowFrame(hWnd, st->isLight); }
            return 0;
        }
        case WM_ERASEBKGND: return 1;
        case WM_SIZE:
            if (st && st->pHwndRT && wParam != SIZE_MINIMIZED) {
                UINT nw = LOWORD(lParam), nh = HIWORD(lParam);
                if (nw > 0 && nh > 0) st->pHwndRT->Resize(D2D1::SizeU(nw, nh));
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        case WM_PAINT: PaintRecSettingsWindow(hWnd, st); return 0;
        case WM_MOUSEWHEEL:
            if (st) {
                st->scrollY = std::clamp(st->scrollY - (GET_WHEEL_DELTA_WPARAM(wParam) / 3), 0, st->maxScrollY);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        case WM_MOUSEMOVE:
            if (st) {
                POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
                int newHov = 0;
                for (const auto& it : st->hits) if (PtInRect(&it.rc, pt)) { newHov = it.id; break; }
                if (newHov != st->hoveredId) { st->hoveredId = newHov; InvalidateRect(hWnd, nullptr, FALSE); }
            }
            return 0;
        case WM_LBUTTONUP: {
            if (!st) return 0;
            POINT pt{ static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            bool isVi = (ResolveEffectiveLanguageCode(st->settings.language) == L"vi");
            UiStrings ui = GetUiStrings(st->settings.language);

            for (const auto& hit : st->hits) {
                if (!PtInRect(&hit.rc, pt)) continue;
                int id = hit.id;
                bool changed = false;

                if (id >= 1000 && id <= 1003) {
                    st->activeTab = hit.intVal;
                    st->scrollY = 0;
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
                if (id == 2000) {
                    std::vector<std::pair<int, std::wstring>> opts = {
                        { 1,  isVi ? L"Windows 11 Blur (Mặc định)" : L"Windows 11 Blur (Default)" },
                        { 2,  L"Liquid Glass" },
                        { 3,  L"Windows 11 Mica" },
                        { 4,  L"Fluent Solid" },
                        { 5,  L"Classic Win11" },
                        { 6,  L"Midnight OLED" },
                        { 7,  L"Nord Frost" },
                        { 8,  L"Catppuccin Mocha" },
                        { 9,  L"Dracula Velvet" },
                        { 10, L"Rosé Pine" },
                        { 11, L"Emerald Slate" },
                        { 12, L"Warm Paper" },
                        { 13, isVi ? L"Tự tùy chỉnh" : L"Custom" }
                    };
                    static const wchar_t* kKeys[] = {
                        L"", L"windows_blur", L"liquid_glass", L"mica_alt", L"fluent_solid",
                        L"classic_win11", L"midnight_oled", L"nord_frost", L"catppuccin_mocha",
                        L"dracula_velvet", L"rose_pine", L"emerald_slate", L"warm_paper", L"custom"
                    };
                    int curIdx = 1;
                    for (int k = 1; k <= 13; ++k) if (st->settings.themePreset == kKeys[k]) curIdx = k;
                    int sel = ShowRecComboFlyout(hWnd, hit.rc, opts, curIdx, st->isLight, st->dpi);
                    if (sel >= 1 && sel <= 13) {
                        Wh_SetStringValue(L"Ov_themePreset", kKeys[sel]);
                        if (sel != 13) {
                            Wh_SetStringValue(L"Ov_customBgColor", L"__NONE__");
                            Wh_SetStringValue(L"Ov_customBorderColor", L"__NONE__");
                            Wh_SetIntValue(L"Ov_customCornerRadius", -1);
                        }
                        changed = true;
                    }
                } else if (id == 2100) {
                    std::vector<std::pair<int, std::wstring>> opts = {
                        { 1, isVi ? L"Tự động theo Windows" : L"Auto (Follow Windows)" },
                        { 2, isVi ? L"Tối (Dark)" : L"Dark" },
                        { 3, isVi ? L"Sáng (Light)" : L"Light" }
                    };
                    int curIdx = (st->settings.colorMode == ColorMode::Dark) ? 2 : ((st->settings.colorMode == ColorMode::Light) ? 3 : 1);
                    int sel = ShowRecComboFlyout(hWnd, hit.rc, opts, curIdx, st->isLight, st->dpi);
                    if (sel >= 1 && sel <= 3) {
                        static const wchar_t* kC[] = { L"", L"auto", L"dark", L"light" };
                        Wh_SetStringValue(L"Ov_colorMode", kC[sel]);
                        changed = true;
                    }
                } else if (id == 2201) {
                    COLORREF initRgb = RGB(30, 30, 36);
                    TryParseHexColor(st->settings.customBgColor, &initRgb);
                    COLORREF picked = 0;
                    if (PickRecColorViaWinUI3(hWnd, initRgb, st->isLight, isVi, st->dpi, &picked)) {
                        Wh_SetStringValue(L"Ov_customBgColor", FormatHexColor(picked).c_str());
                        changed = true;
                    }
                } else if (id == 2202) {
                    Wh_SetStringValue(L"Ov_customBgColor", L"__NONE__");
                    changed = true;
                } else if (id == 2203) {
                    COLORREF initRgb = RGB(80, 90, 110);
                    TryParseHexColor(st->settings.customBorderColor, &initRgb);
                    COLORREF picked = 0;
                    if (PickRecColorViaWinUI3(hWnd, initRgb, st->isLight, isVi, st->dpi, &picked)) {
                        Wh_SetStringValue(L"Ov_customBorderColor", FormatHexColor(picked).c_str());
                        changed = true;
                    }
                } else if (id == 2204) {
                    Wh_SetStringValue(L"Ov_customBorderColor", L"__NONE__");
                    changed = true;
                } else if (id == 2210 || id == 2211 || id == 2212) {
                    int cur = (st->settings.customCornerRadius < 0) ? 14 : st->settings.customCornerRadius;
                    int next = (id == 2212) ? -1 : std::clamp(cur + (id == 2211 ? 2 : -2), 0, 28);
                    Wh_SetIntValue(L"Ov_customCornerRadius", next);
                    changed = true;
                } else if (id == 2301) {
                    RECT monRect = GetCaptureTargetRect(st->settings.captureMonitorMode);
                    int hwW = static_cast<int>(monRect.right - monRect.left) & ~1;
                    int hwH = static_cast<int>(monRect.bottom - monRect.top) & ~1;
                    std::vector<std::pair<int, std::wstring>> opts;
                    wchar_t natLbl[96] = {};
                    swprintf_s(natLbl, L"%s (%d\u00D7%d)", ui.mResNativePrefix, hwW, hwH);
                    opts.push_back({ 0, natLbl });
                    static constexpr struct { int h; const wchar_t* tag; } kResP[] = {
                        { 2160, L"4K" }, { 1440, L"2K" }, { 1080, L"Full HD" }, { 900, L"HD+" }, { 720, L"HD" }, { 480, L"SD" }
                    };
                    for (const auto& pr : kResP) {
                        if (pr.h < hwH) {
                            int pw = 0, ph = 0;
                            ComputeCappedOutputSize(hwW, hwH, pr.h, pw, ph);
                            wchar_t lbl[96] = {};
                            swprintf_s(lbl, L"%dp %s (%d\u00D7%d)", ph, pr.tag, pw, ph);
                            opts.push_back({ pr.h, lbl });
                        }
                    }
                    int curRes = (st->settings.targetResHeight >= hwH) ? 0 : st->settings.targetResHeight;
                    // Use id+1 so 0 means cancelled
                    std::vector<std::pair<int, std::wstring>> flyOpts;
                    for (auto& o : opts) flyOpts.push_back({ o.first + 1, o.second });
                    int sel = ShowRecComboFlyout(hWnd, hit.rc, flyOpts, curRes + 1, st->isLight, st->dpi);
                    if (sel >= 1) { Wh_SetIntValue(L"Ov_targetResHeight", sel - 1); changed = true; }
                } else if (id == 2302) {
                    int hwHz = GetMonitorRefreshRateHz(st->settings.captureMonitorMode);
                    int effFps = ResolveEffectiveFps(st->settings.fps, st->settings.captureMonitorMode);
                    static constexpr int kFpsVals[] = { 30, 60, 75, 90, 120, 144, 165, 240 };
                    std::vector<std::pair<int, std::wstring>> opts;
                    bool hasExact = false;
                    for (int f : kFpsVals) {
                        if (f <= hwHz || f == 30) {
                            if (f == hwHz) hasExact = true;
                            wchar_t lbl[64] = {};
                            if (f == hwHz) swprintf_s(lbl, ui.mFpsCurrentFmt, f, hwHz);
                            else swprintf_s(lbl, L"%d FPS", f);
                            opts.push_back({ f, lbl });
                        }
                    }
                    if (!hasExact && hwHz > 30 && hwHz <= 360) {
                        wchar_t lbl[64] = {};
                        swprintf_s(lbl, ui.mFpsCurrentFmt, hwHz, hwHz);
                        opts.push_back({ hwHz, lbl });
                    }
                    int sel = ShowRecComboFlyout(hWnd, hit.rc, opts, effFps, st->isLight, st->dpi);
                    if (sel >= 30 && sel <= 360) { Wh_SetIntValue(L"Ov_fps", sel); changed = true; }
                } else if (id == 2303) {
                    std::vector<std::pair<int, std::wstring>> opts = {
                        { 6, L"6 Mbps" }, { 12, L"12 Mbps" }, { 20, L"20 Mbps" }, { 35, L"35 Mbps" }
                    };
                    int sel = ShowRecComboFlyout(hWnd, hit.rc, opts, st->settings.videoBitrateMbps, st->isLight, st->dpi);
                    if (sel >= 2 && sel <= 50) { Wh_SetIntValue(L"Ov_videoBitrateMbps", sel); changed = true; }
                } else if (id == 2310) {
                    auto mons = EnumerateConnectedMonitors();
                    std::vector<std::pair<int, std::wstring>> opts;
                    opts.push_back({ 1, ui.mMonCursor });
                    int curId = 1;
                    int selIdx = ParseMonitorIndexMode(st->settings.captureMonitorMode);
                    for (size_t i = 0; i < mons.size() && i < 8; ++i) {
                        wchar_t base[32] = {}, lbl[96] = {};
                        swprintf_s(base, ui.mMonItemFmt, mons[i].displayNumber);
                        swprintf_s(lbl, L"%s (%d\u00D7%d \u2022 %dHz)%s", base, mons[i].width, mons[i].height, mons[i].refreshHz, mons[i].isPrimary ? L" \u2605" : L"");
                        int optId = 10 + static_cast<int>(i);
                        opts.push_back({ optId, lbl });
                        if (selIdx == static_cast<int>(i) || (st->settings.captureMonitorMode == L"primary" && i == 0)) curId = optId;
                    }
                    if (mons.size() > 1) {
                        opts.push_back({ 2, ui.mMonAll });
                        if (st->settings.captureMonitorMode == L"all") curId = 2;
                    }
                    opts.push_back({ 3, ui.layoutTitle });
                    if (st->settings.captureMonitorMode == L"custom") curId = 3;

                    int sel = ShowRecComboFlyout(hWnd, hit.rc, opts, curId, st->isLight, st->dpi);
                    if (sel == 1) { Wh_SetStringValue(L"Ov_captureMonitor", L"cursor"); changed = true; }
                    else if (sel == 2) { Wh_SetStringValue(L"Ov_captureMonitor", L"all"); changed = true; }
                    else if (sel == 3) { Wh_SetStringValue(L"Ov_captureMonitor", L"custom"); changed = true; TriggerLayoutArrangerInExplorer(); }
                    else if (sel >= 10 && sel < 18) {
                        wchar_t mv[16] = {};
                        swprintf_s(mv, L"mon:%d", sel - 10);
                        Wh_SetStringValue(L"Ov_captureMonitor", mv);
                        changed = true;
                    }
                } else if (id == 2311) {
                    TriggerLayoutArrangerInExplorer();
                    return 0;
                } else if (id == 2401) {
                    Wh_SetIntValue(L"Ov_recordSystemAudio", st->settings.recordSystemAudio ? 0 : 1); changed = true;
                } else if (id == 2402) {
                    Wh_SetIntValue(L"Ov_recordMicrophone", st->settings.recordMicrophone ? 0 : 1); changed = true;
                } else if (id == 2403) {
                    Wh_SetIntValue(L"Ov_recordCursor", st->settings.recordCursor ? 0 : 1); changed = true;
                } else if (id == 2404) {
                    Wh_SetIntValue(L"Ov_openFolderOnFinish", st->settings.openFolderOnFinish ? 0 : 1); changed = true;
                } else if (id == 2405) {
                    Wh_SetIntValue(L"Ov_showFloatingHud", st->settings.showFloatingHud ? 0 : 1); changed = true;
                } else if (id == 2410) {
                    OpenRecordingsFolderInExplorer();
                    return 0;
                } else if (id >= 2600 && id <= 2621) {
                    Wh_SetStringValue(L"Ov_language", hit.strVal.c_str()); changed = true;
                }

                if (changed) {
                    LoadSettings(false);
                    st->settings = GetSettingsCopy();
                    st->isLight = (st->settings.colorMode == ColorMode::Light) ? true
                                : ((st->settings.colorMode == ColorMode::Dark) ? false : IsSystemAppLightTheme());
                    bool viNow = (ResolveEffectiveLanguageCode(st->settings.language) == L"vi");
                    SetWindowTextW(hWnd, viNow ? L"Cài đặt Quay Màn Hình" : L"Screen Recorder Settings");
                    ApplyRecSettingsWindowFrame(hWnd, st->isLight);
                    InvalidateRect(hWnd, nullptr, FALSE);
                    BroadcastReloadToExplorerRecorder();
                }
                break;
            }
            return 0;
        }
        case WM_CLOSE: DestroyWindow(hWnd); return 0;
        case WM_NCDESTROY: {
            HWND expected = hWnd;
            g_hRecWin11SettingsWnd.compare_exchange_strong(expected, nullptr);
            if (st && st->pHwndRT) { st->pHwndRT->Release(); st->pHwndRT = nullptr; }
            delete st;
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
            return 0;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static void ShowOrFocusRecSettingsWindow() {
    if (g_unloading.load() || !g_recWin11SettingsRegistered) return;
    HWND hExisting = g_hRecWin11SettingsWnd.load();
    if (hExisting && IsWindow(hExisting)) {
        if (IsIconic(hExisting)) ShowWindow(hExisting, SW_RESTORE);
        ShowWindow(hExisting, SW_SHOW);
        BringWindowToTop(hExisting);
        SetForegroundWindow(hExisting);
        return;
    }

    LoadSettings(false);
    auto* st = new RecSettingsState();
    st->settings = GetSettingsCopy();

    POINT curPt{ 0, 0 };
    GetCursorPos(&curPt);
    HMONITOR hMon = MonitorFromPoint(curPt, MONITOR_DEFAULTTOPRIMARY);
    UINT dpi = 96;
    if (HDC hdc = GetDC(nullptr)) { dpi = static_cast<UINT>(GetDeviceCaps(hdc, LOGPIXELSX)); ReleaseDC(nullptr, hdc); }
    st->dpi = dpi;
    st->isLight = (st->settings.colorMode == ColorMode::Light) ? true
                : ((st->settings.colorMode == ColorMode::Dark) ? false : IsSystemAppLightTheme());

    int winW = DpiScale(880, dpi), winH = DpiScale(600, dpi);
    MONITORINFO mi{ sizeof(mi) };
    int posX = 120, posY = 90;
    if (GetMonitorInfoW(hMon, &mi)) {
        posX = static_cast<int>(mi.rcWork.left) + (static_cast<int>(mi.rcWork.right - mi.rcWork.left) - winW) / 2;
        posY = static_cast<int>(mi.rcWork.top)  + (static_cast<int>(mi.rcWork.bottom - mi.rcWork.top) - winH) / 2;
    }

    bool isVi = (ResolveEffectiveLanguageCode(st->settings.language) == L"vi");
    HWND hWnd = CreateWindowExW(
        WS_EX_APPWINDOW, kRecWin11SettingsClass,
        isVi ? L"Cài đặt Quay Màn Hình" : L"Screen Recorder Settings",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
        posX, posY, winW, winH, nullptr, nullptr, GetModInstanceHandle(), st);
    if (!hWnd) { delete st; return; }
    g_hRecWin11SettingsWnd.store(hWnd);
    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);
    BringWindowToTop(hWnd);
    SetForegroundWindow(hWnd);
}

static LRESULT CALLBACK RecSettingsHostWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == g_msgOpenSettings && g_msgOpenSettings != 0) {
        ShowOrFocusRecSettingsWindow();
        return 1;
    }
    if (uMsg == WM_CLOSE) {
        if (HWND hSet = g_hRecWin11SettingsWnd.load(); hSet && IsWindow(hSet)) DestroyWindow(hSet);
        DestroyWindow(hWnd);
        return 0;
    }
    if (uMsg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI RecSettingsHostThreadProc(LPVOID param) {
    HANDLE hReadyEvent = static_cast<HANDLE>(param);
    MSG initMsg{};
    PeekMessageW(&initMsg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr, reinterpret_cast<void**>(&g_pD2DFactory));
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_pDWriteFactory));

    HINSTANCE hInst = GetModInstanceHandle();
    WNDCLASSEXW wcSet{ sizeof(wcSet) };
    wcSet.style = CS_HREDRAW | CS_VREDRAW;
    wcSet.lpfnWndProc = RecWin11SettingsWndProc;
    wcSet.hInstance = hInst;
    wcSet.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcSet.lpszClassName = kRecWin11SettingsClass;
    if (RegisterClassExW(&wcSet) || GetLastError() == ERROR_CLASS_ALREADY_EXISTS) g_recWin11SettingsRegistered = true;

    WNDCLASSEXW wcHost{ sizeof(wcHost) };
    wcHost.lpfnWndProc = RecSettingsHostWndProc;
    wcHost.hInstance = hInst;
    wcHost.lpszClassName = kRecSettingsHostClass;
    if (RegisterClassExW(&wcHost) || GetLastError() == ERROR_CLASS_ALREADY_EXISTS) g_recSettingsHostRegistered = true;

    HWND hHost = nullptr;
    if (g_recSettingsHostRegistered) {
        hHost = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kRecSettingsHostClass, L"",
            WS_POPUP, -32000, -32000, 0, 0, nullptr, nullptr, hInst, nullptr);
    }
    if (hHost) {
        if (g_msgOpenSettings) ChangeWindowMessageFilterEx(hHost, g_msgOpenSettings, MSGFLT_ALLOW, nullptr);
        g_hRecSettingsHostWnd.store(hHost);
        Wh_Log(L"[ScreenRecorder] WinUI 3 Settings IPC host ready (HWND=%p, PID=%lu)", hHost, GetCurrentProcessId());
    } else {
        Wh_Log(L"[ScreenRecorder] Settings IPC host CreateWindowExW failed (err=%lu)", GetLastError());
    }
    if (hReadyEvent) SetEvent(hReadyEvent);

    if (hHost) {
        MSG msg{};
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        HWND expected = hHost;
        g_hRecSettingsHostWnd.compare_exchange_strong(expected, nullptr);
    }

    if (g_recWin11SettingsRegistered) { UnregisterClassW(kRecWin11SettingsClass, hInst); g_recWin11SettingsRegistered = false; }
    if (g_recSettingsHostRegistered)  { UnregisterClassW(kRecSettingsHostClass, hInst);  g_recSettingsHostRegistered = false; }
    if (g_pDWriteFactory) { g_pDWriteFactory->Release(); g_pDWriteFactory = nullptr; }
    if (g_pD2DFactory)    { g_pD2DFactory->Release();    g_pD2DFactory = nullptr; }
    CoUninitialize();
    return 0;
}

static void StopRecSettingsHostCleanly() {
    if (HWND hSet = g_hRecWin11SettingsWnd.load(); hSet && IsWindow(hSet)) PostMessageW(hSet, WM_CLOSE, 0, 0);
    if (HWND hHost = g_hRecSettingsHostWnd.load(); hHost && IsWindow(hHost)) PostMessageW(hHost, WM_CLOSE, 0, 0);
    if (g_recSettingsHostThreadId != 0) PostThreadMessageW(g_recSettingsHostThreadId, WM_QUIT, 0, 0);
    if (g_hRecSettingsHostThread) {
        WaitForSingleObject(g_hRecSettingsHostThread, 2000);
        CloseHandle(g_hRecSettingsHostThread);
        g_hRecSettingsHostThread = nullptr;
        g_recSettingsHostThreadId = 0;
    }
}

static bool IsExplorerProcess() {
    wchar_t exePath[MAX_PATH] = {};
    if (GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath)) == 0) return false;
    const wchar_t* exeName = PathFindFileNameW(exePath);
    return exeName && _wcsicmp(exeName, L"explorer.exe") == 0;
}

static BOOL WhTool_ModInit() {
    g_unloading.store(false);
    LoadSettings(false);
    g_msgOpenSettings       = RegisterWindowMessageW(L"Windhawk_OpenRecSettings_" WH_MOD_ID);
    g_msgReloadSettings     = RegisterWindowMessageW(L"Windhawk_ReloadRecSettings_" WH_MOD_ID);
    g_msgOpenLayoutArranger = RegisterWindowMessageW(L"Windhawk_OpenRecLayout_" WH_MOD_ID);

    HANDLE hReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hRecSettingsHostThread = CreateThread(nullptr, 0, RecSettingsHostThreadProc, hReady, 0, &g_recSettingsHostThreadId);
    if (!g_hRecSettingsHostThread) {
        if (hReady) CloseHandle(hReady);
        return FALSE;
    }
    if (hReady) { WaitForSingleObject(hReady, 3000); CloseHandle(hReady); }
    return TRUE;
}

static void WhTool_ModSettingsChanged() {
    LoadSettings(true);
    if (HWND hSet = g_hRecWin11SettingsWnd.load(); hSet && IsWindow(hSet) && g_msgReloadSettings != 0) {
        PostMessageW(hSet, g_msgReloadSettings, 0, 0);
    }
}

static void WhTool_ModUninit() {
    g_unloading.store(true);
    StopRecSettingsHostCleanly();
}

static void WINAPI RecToolEntryPoint_Hook() {
    ExitThread(0);
}

// ============================================================================
// Windhawk Dual-Process Entry Points (explorer.exe + windhawk.exe -tool-mod)
// ============================================================================

BOOL Wh_ModInit() {
    if (IsExplorerProcess()) {
        g_isExplorerProcess = true;
        Wh_Log(L"[ScreenRecorder] Initializing Native Windows Screen Recorder v2.0.4 (explorer.exe)");
        g_unloading.store(false);
        g_msgOpenSettings       = RegisterWindowMessageW(L"Windhawk_OpenRecSettings_" WH_MOD_ID);
        g_msgReloadSettings     = RegisterWindowMessageW(L"Windhawk_ReloadRecSettings_" WH_MOD_ID);
        g_msgOpenLayoutArranger = RegisterWindowMessageW(L"Windhawk_OpenRecLayout_" WH_MOD_ID);
        LoadSettings(false);
        g_uiThread = std::thread(UiThreadProc);
        return TRUE;
    }

    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) return FALSE;

    bool isExcluded = false, isToolModProcess = false, isCurrentToolModProcess = false;
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return FALSE;
    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 || wcscmp(argv[i], L"-service-start") == 0 || wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true; break;
        }
    }
    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) isCurrentToolModProcess = true;
            break;
        }
    }
    LocalFree(argv);
    if (isExcluded) return FALSE;

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex || GetLastError() == ERROR_ALREADY_EXISTS) ExitProcess(1);
        if (!WhTool_ModInit()) ExitProcess(1);

        auto* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        auto* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);
        void* entryPoint = reinterpret_cast<BYTE*>(dosHeader) + ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(RecToolEntryPoint_Hook), nullptr);
        return TRUE;
    }

    if (isToolModProcess) return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isExplorerProcess || !g_isToolModProcessLauncher) return;

    WCHAR currentProcessPath[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath));
    if (len == 0 || len == ARRAYSIZE(currentProcessPath)) return;

    WCHAR commandLine[MAX_PATH + 64];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) kernelModule = GetModuleHandleW(L"kernel32.dll");
    if (!kernelModule) return;

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, WINBOOL,
        DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    auto pCreateProcessInternalW = reinterpret_cast<CreateProcessInternalW_t>(GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!pCreateProcessInternalW) return;

    STARTUPINFOW si{};
    si.cb = sizeof(STARTUPINFOW);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi{};
    if (pCreateProcessInternalW(nullptr, currentProcessPath, commandLine, nullptr, nullptr, FALSE,
                                NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

void Wh_ModSettingsChanged() {
    if (g_isExplorerProcess) {
        LoadSettings(true);
        HWND hwnd = g_msgHwnd.load();
        if (hwnd) PostMessageW(hwnd, WM_APP_RELOAD_SETTINGS, 0, 0);
        return;
    }
    if (g_isToolModProcessLauncher) return;
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorerProcess) {
        g_unloading.store(true);
        StopRecordingSync();
        if (HWND hMenu = g_activeMenuHwnd.load()) {
            if (IsWindow(hMenu)) PostMessageW(hMenu, WM_CLOSE, 0, 0);
        }
        HWND hwnd = g_msgHwnd.load();
        if (hwnd) PostMessageW(hwnd, WM_APP_SHUTDOWN, 0, 0);
        if (g_uiThread.joinable()) g_uiThread.join();
        return;
    }
    if (g_isToolModProcessLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}
