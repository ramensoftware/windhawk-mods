// ==WindhawkMod==
// @id              modern-run-dialog-ui
// @name            Modern Run Dialog UI For Windows 10 & 11
// @name:vi-VN      Giao diện Hộp thoại Run Hiện đại cho Windows 10 & 11
// @description     Replaces the classic Win + R dialog with a modern Liquid Glass / Fluent UI, custom image/video backgrounds, and quick settings.
// @description:vi-VN Làm mới hộp thoại Run (Win + R) với giao diện Liquid Glass / Fluent hiện đại, hỗ trợ hình nền ảnh/video và menu cài đặt nhanh.
// @version         4.0.101
// @author          caotu2k5
// @github          https://github.com/caotu2k5
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -luser32 -lshell32 -lshlwapi -lcomctl32 -lversion -limm32 -lcomdlg32 -ld2d1 -ldwrite -ladvapi32 -lole32 -luuid -lwindowscodecs -lmfplat -lmfreadwrite -lmfuuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Modern Run Dialog UI For Windows 10 & 11

![Modern Run Dialog UI Preview (Liquid Glass)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/4.0.101/40101liquidglass.png)

*Liquid Glass mode*

![Modern Run Dialog UI Preview (Windows Blur)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/4.0.101/40101windowsblur.png)

*Windows Blur mode*

![Modern Run Dialog UI Preview (App Suggestions)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/4.0.101/40101recommendprogram.png)

*App & command suggestions*

![Modern Run Dialog UI Preview (Minimal Mode)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/4.0.101/40101minimalmode.png)

*Minimal Mode*

![Modern Run Dialog UI Preview (Google Search)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/4.0.101/40101googlesearch.png)

*Google Search (`ggsearch:`)*

![Modern Run Dialog UI Preview (Background Change)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/4.0.101/40101backgroundchange.png)

*Background Change*

![Modern Run Dialog UI Preview (Theme Menu)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/4.0.101/40101thememenu.png)

*Theme Menu*

![Modern Run Dialog UI Preview (Resize Menu)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/4.0.101/40101resizemenu.png)

*Resize Menu*

---

## 🌐 English

Replaces the classic Win32 **Run (`Win + R`)** dialog with a borderless **Liquid Glass / Fluent** launcher rendered with **Direct2D & DirectWrite**. Works across **Windows 10** and **Windows 11** (including the latest Insider Canary/Dev builds with `.dll.mui` localized resources) without downloading `shell32.pdb`.

### Key Features

- **Liquid Glass, Windows Blur & Opaque Themes:**
  * Three surface styles (**Liquid Glass** with 3D Snell's-law optical edge refraction, chromatic dispersion, refractive rim highlights, and adjustable tint from `-100` Max Transparent default to `100` High Contrast, **Windows Blur** with `0`–`255` opacity, and **Opaque**) with automatic Light/Dark mode and real-time desktop backdrop luminance detection.
  * Adjustable UI scale presets (`85%` – `120%`), inline `-` / `+` and direct keyboard input for Width (`px`), Height (`px`), and Font Size (`pt`) with live preview, and taskbar-anchored positioning.

- **Minimal Mode (Hide Browse & Exit Buttons):**
  * Toggle **Minimal Mode** with a single click from the main `⚙️` menu (or Windhawk settings) to hide the **Browse** and **Exit** buttons, keeping the standard dialog width while expanding the command input box right up to the **Run** button for a clean, distraction-free layout.

- **Custom Image, GIF & Video Backgrounds:**
  * Set any static image (`.png`, `.jpg`, `.webp`, `.bmp`), animated `.gif`, or `.mp4` / `.webm` video (up to 50 MB, decoded via Windows Media Foundation & WIC) as the dialog background with adjustable media opacity (`20` – `255`, presets `90`, `150`, `215`, `255`) and automatic text contrast adaptation.

- **In-App Quick Settings Menu (`⚙️`):**
  * Built-in Liquid Glass / Fluent popup menu on the command bar with inline `-` / `[value]` / `+` controls to switch surface themes, Liquid Glass tint (`-100` – `100`), Windows Blur opacity (`0` – `255`), Light/Dark mode, UI scale (`85%` – `120%`) & custom dimensions, **Minimal Mode**, custom background media, history preferences, and display language, or clear stored history on the fly without opening Windhawk.

- **Live App Suggestions & Instant Web Search (`ggsearch:` / `msesearch:`):**
  * Real-time suggestions for installed Start Menu applications, Registry `App Paths`, and system utilities (`cmd`, `powershell`, `regedit`, `wt`, `.cpl`, `.msc`) as you type.
  * Type `ggsearch:<query>` to search Google in Chrome (or your default browser) and `msesearch:<query>` to search in Microsoft Edge.

- **Scrollable History (Up to 50 Items) & Execution Timestamps:**
  * Stores up to **50 recent commands** in a compact 5-row scrollable list (supports mouse wheel, draggable scrollbar, `Up`/`Down`/`PageUp`/`PageDown`, and `Delete` to remove individual entries).
  * Shows each command's native icon, application name, and last executed time.

- **Smart Taskbar & `Win + R` Window Behavior:**
  * Pressing `Win + R` or clicking the Taskbar icon brings an unfocused Run dialog back to the foreground in a single action, or minimizes/closes it when already focused.

- **Multi-Language Support (21 Languages Across Dialog & Menu):**
  * Auto-detects your Windows UI language or lets you choose from 21 built-in languages across both the Run dialog and the entire in-app `⚙️` quick settings menu (English, Vietnamese, Japanese, Korean, Simplified/Traditional Chinese, Thai, Indonesian, Malay, French, German, Spanish, Portuguese, Italian, Russian, Ukrainian, Polish, Dutch, Turkish, Hindi, and Arabic).

---

## 🇻🇳 Tiếng Việt

Thay thế hộp thoại **Run (`Win + R`)** cổ điển của Windows bằng giao diện **Liquid Glass / Fluent** không viền, vẽ trực tiếp bằng **Direct2D & DirectWrite**. Tương thích mượt mà với cả **Windows 10** và **Windows 11** (bao gồm các bản Windows 11 Insider mới nhất dùng tài nguyên `.dll.mui`) mà không cần tải `shell32.pdb`.

### Tính năng chính

- **3 Chế độ bề mặt (Liquid Glass, Làm mờ Windows & Nền đặc):**
  * Hỗ trợ hiệu ứng kính **Liquid Glass** (khúc xạ ánh sáng vát cạnh 3D, tán sắc lăng kính, viền phản quang và tùy chỉnh độ trong suốt từ `-100` Trong suốt tối đa mặc định đến `100` Tương phản cao), **Windows Blur** (`0` – `255`) và **Nền đặc**, tự động chuyển đổi giao diện Sáng / Tối theo hệ thống và độ sáng thực tế của màn hình nền.
  * Tùy chỉnh kích thước theo các mức có sẵn (`85%` – `120%`) hoặc nhập số / bấm `-` `+` chỉnh trực tiếp Chiều rộng (`px`), Chiều cao (`px`) và Cỡ chữ (`pt`) ngay trên menu (xem trực tiếp).

- **Chế độ tối giản – Minimal Mode (Ẩn nút Duyệt & Thoát):**
  * Bật/tắt **Chế độ tối giản** chỉ với 1 cú nhấp chuột ngay trên menu chính `⚙️` (hoặc trong cài đặt Windhawk) để ẩn hai nút **Duyệt** và **Thoát**, giữ nguyên chiều rộng gốc của hộp thoại và mở rộng tối đa khung nhập lệnh sát tới nút **Chạy**.

- **Hình nền tùy chỉnh (Ảnh tĩnh, GIF động & Video MP4):**
  * Cho phép chọn ảnh tĩnh (`.png`, `.jpg`, `.webp`, `.bmp`), ảnh động `.gif` hoặc video `.mp4` / `.webm` (tối đa 50 MB) làm nền hộp thoại Run với độ rõ tùy chỉnh (`20` – `255`, có sẵn các mức `90`, `150`, `215`, `255`) và tự động đổi màu chữ theo độ sáng hình nền.

- **Menu Cài đặt nhanh trực tiếp (`⚙️`):**
  * Nút bánh răng trên thanh lệnh mở menu Liquid Glass / Fluent có ô nhập số `-` / `[giá trị]` / `+` trực quan, giúp đổi tức thì giao diện bề mặt, độ đậm kính Liquid Glass (`-100` – `100`), độ mờ Windows Blur (`0` – `255`), chế độ Sáng/Tối, kích thước UI & cỡ chữ, **Chế độ tối giản**, hình nền tùy chỉnh, tùy chọn lịch sử, ngôn ngữ hiển thị hoặc xóa toàn bộ lịch sử ngay trên hộp thoại Run.

- **Gợi ý ứng dụng & Tìm kiếm trình duyệt (`ggsearch:` / `msesearch:`):**
  * Tự động gợi ý theo thời gian thực các ứng dụng trong Start Menu, `App Paths` và các công cụ hệ thống (`cmd`, `powershell`, `regedit`, `wt`, `.cpl`, `.msc`) ngay khi đang gõ.
  * Gõ `ggsearch:<từ khóa>` để tìm nhanh trên Google bằng Chrome (hoặc trình duyệt mặc định) và `msesearch:<từ khóa>` để tìm bằng Microsoft Edge.

- **Lịch sử 50 mục có thanh cuộn & Thời gian chạy lệnh:**
  * Lưu tối đa **50 lệnh gần đây** trong khung nhìn gọn 5 dòng (hỗ trợ lăn chuột, kéo thanh cuộn, phím `Lên`/`Xuống`/`PageUp`/`PageDown` và phím `Delete` để xóa từng lệnh).
  * Hiển thị biểu tượng gốc của phần mềm, tên ứng dụng và thời gian thực thi gần nhất.

- **Tương tác thông minh với `Win + R` và Taskbar:**
  * Khi hộp thoại đang bị che bởi cửa sổ khác, chỉ cần bấm 1 lần vào biểu tượng Run dưới Taskbar hoặc nhấn `Win + R` là hộp thoại nổi ngay lên trên cùng.

- **Hỗ trợ đầy đủ 21 ngôn ngữ hiển thị (Cả Hộp thoại & Menu `⚙️`):**
  * Tự động nhận diện ngôn ngữ Windows hoặc chọn cố định trong 21 ngôn ngữ cho toàn bộ hộp thoại Run lẫn menu cài đặt nhanh `⚙️` (Tiếng Việt, Tiếng Anh, Nhật, Hàn, Trung Giản thể/Phồn thể, Thái, Indonesia, Mã Lai, Pháp, Đức, Tây Ban Nha, Bồ Đào Nha, Ý, Nga, Ukraina, Ba Lan, Hà Lan, Thổ Nhĩ Kỳ, Hindi và Ả Rập).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Display · Language
  $name:vi-VN: Ngôn ngữ hiển thị
  $description: Auto-detect the system’s UI language or choose a fixed language (also changeable via the in-app Gear button).
  $description:vi-VN: Tự động theo hệ điều hành hoặc chọn ngôn ngữ cố định (có thể đổi nhanh qua nút Bánh răng trên hộp thoại).
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
  - pt: Português (Portuguese)
  - it: Italiano (Tiếng Ý)
  - ru: Русский (Tiếng Nga)
  - uk: Українська (Tiếng Ukraina)
  - pl: Polski (Tiếng Ba Lan)
  - nl: Nederlands (Tiếng Hà Lan)
  - tr: Türkçe (Tiếng Thổ Nhĩ Kỳ)
  - hi: हिन्दी (Tiếng Hindi)
  - ar: العربية (Tiếng Ả Rập)
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
- minimalMode: false
  $name: Appearance · Minimal Mode (Hide Browse & Exit)
  $name:vi-VN: Chế độ tối giản (Ẩn nút Duyệt & Thoát)
  $description: Hides the Browse and Exit buttons on the command bar while keeping the standard dialog width.
  $description:vi-VN: Ẩn hai nút Duyệt và Thoát trên thanh nhập lệnh (chỉ giữ nút Chạy và Cài đặt nhanh), giữ nguyên chiều rộng hộp thoại.
- uiScalePercent: "100"
  $name: Appearance · UI Size / Scale Preset (85% – 120%)
  $name:vi-VN: Tỷ lệ kích thước giao diện có sẵn (85% – 120%)
  $description: Adjust the overall size of the Run dialog (85% Compact, 100% Default, 110% Medium, 120% Large). Also adjustable via the Gear button.
  $description:vi-VN: Điều chỉnh tỷ lệ kích thước tổng thể của hộp thoại Run (85% Nhỏ gọn, 100% Mặc định, 110% Vừa, 120% Lớn). Có thể chỉnh trực tiếp qua nút Bánh răng.
  $options:vi-VN:
  - "85": 85% – Nhỏ gọn (Compact)
  - "90": 90% – Hơi nhỏ
  - "100": 100% – Mặc định (Chuẩn)
  - "110": 110% – Lớn vừa (Medium)
  - "120": 120% – Rất lớn (Large)
  $options:
  - "85": 85% – Compact
  - "90": 90% – Slightly Smaller
  - "100": 100% – Default (Standard)
  - "110": 110% – Medium Large
  - "120": 120% – Large
- glassOpacity: -100
  $name: Liquid Glass Opacity / Tint (-100 to 100)
  $name:vi-VN: Độ đậm / Trong suốt của Liquid Glass (-100 đến 100)
  $description: Adjusts Liquid Glass transparency from Max Transparent (-100, default) and Clear (0) to Tinted (60–100) for higher contrast.
  $description:vi-VN: Điều chỉnh độ trong suốt của Liquid Glass từ Trong suốt tối đa (-100, mặc định), Chuẩn (0) đến Đậm tương phản cao (60–100).
- blurOpacity: 0
  $name: Appearance · Blur Opacity (0–255)
  $name:vi-VN: Độ đậm nền Blur (0–255)
  $description: Background tint opacity when using Windows Blur (0–255, default is 0).
  $description:vi-VN: Độ đậm lớp phủ nền khi dùng Làm mờ Windows (0–255, mặc định là 0).
- customBackgroundPath: ""
  $name: Custom Background · File Path (Image / Animated GIF / MP4 Video)
  $name:vi-VN: Hình nền tùy chỉnh · Đường dẫn tệp (Ảnh / GIF động / Video MP4)
  $description: Path to a custom background image (.png, .jpg, .webp), animated .gif, or .mp4/.webm video (or pick directly via the in-app Gear button).
  $description:vi-VN: Đường dẫn tới tệp ảnh tĩnh (.png, .jpg, .webp), ảnh động (.gif) hoặc video (.mp4, .webm) làm hình nền (hoặc bấm nút Bánh răng trên hộp thoại Run để chọn tệp trực tiếp).
- customBackgroundMaxMb: 50
  $name: Custom Background · Max File Size Limit (20–50 MB)
  $name:vi-VN: Hình nền tùy chỉnh · Giới hạn dung lượng tối đa (20–50 MB)
  $description: Maximum allowed file size in megabytes for custom background files (20–50 MB, default is 50 MB).
  $description:vi-VN: Giới hạn dung lượng tối đa cho phép đối với tệp hình nền động/video MP4 (từ 20 đến 50 MB, mặc định 50 MB).
- customBackgroundOpacity: 255
  $name: Custom Background · Media Opacity (20–255)
  $name:vi-VN: Hình nền tùy chỉnh · Độ rõ hình nền (20–255)
  $description: Opacity of the custom background image/video layer (20–255, default is 255 for full original image detail; text contrast adapts automatically to the background).
  $description:vi-VN: Độ rõ của hình nền ảnh/GIF/video (20–255, mặc định 255 để giữ nguyên 100% chi tiết gốc của hình nền; màu chữ tự động đổi theo độ sáng hình nền).
- enableAppSuggestions: true
  $name: Experimental · App Suggestions & Web Search (ggsearch / msesearch)
  $name:vi-VN: Gợi ý ứng dụng & Tìm kiếm trình duyệt (ggsearch / msesearch)
  $description: Suggest installed apps, system commands, and live web search commands (ggsearch:<keyword> for Chrome, msesearch:<keyword> for Edge) while typing.
  $description:vi-VN: Tự động gợi ý ứng dụng, lệnh hệ thống và lệnh tìm kiếm nhanh (ggsearch:<từ khóa> qua Chrome, msesearch:<từ khóa> qua Edge) khi đang gõ.
- maxHistoryItems: 35
  $name: History · Max Stored Items (0–50)
  $name:vi-VN: Số mục lịch sử / gợi ý tối đa (0–50)
  $description: Maximum number of recent commands or suggestions stored and scrollable in the list (0–50, default is 35).
  $description:vi-VN: Tổng số lượng lệnh lịch sử hoặc gợi ý tối đa được lưu và cuộn xem trong danh sách (0–50, mặc định 35).
- visibleHistoryRows: 5
  $name: History · Visible Rows Before Scrolling (3–10)
  $name:vi-VN: Số dòng hiển thị trên khung nhìn (3–10)
  $description: Maximum number of rows visible at once before showing the scrollbar (default is 5 rows as shown in the classic layout).
  $description:vi-VN: Số dòng hiển thị tối đa cùng lúc trước khi hiện thanh cuộn (mặc định là 5 dòng đúng chuẩn giao diện).
- showTimestamp: true
  $name: History · Show Last Executed Time
  $name:vi-VN: Hiển thị thời gian chạy lệnh gần nhất
  $description: Display the execution timestamp and clock icon on each history row — or hide both when disabled.
  $description:vi-VN: Hiển thị thời gian thực thi và biểu tượng đồng hồ ở mỗi dòng lịch sử.
- clearStoredHistory: false
  $name: History · Clear Stored History
  $name:vi-VN: Xóa lịch sử đã lưu trong Mod
  $description: Turn on and save to clear the mod’s displayed history for the current user — or click "Clear History" in the Gear button menu.
  $description:vi-VN: Bật và lưu để xóa lịch sử hiển thị trong mod — hoặc bấm trực tiếp vào nút Bánh răng -> Xóa toàn bộ lịch sử.
- clearInitialInput: true
  $name: Input · Clear Input on Open
  $name:vi-VN: Để trống ô nhập khi mở
  $description: Show placeholder text on open — instead of pre-filling the last command.
  $description:vi-VN: Hiển thị dòng gợi ý khi mới mở thay vì điền sẵn lệnh cũ.
- customDialogWidth: 0
  $name: Appearance · Custom Dialog Width in px (0 = Follow Preset, or 360–1200)
  $name:vi-VN: Chiều rộng hộp thoại tùy chỉnh (0 = Theo thiết lập sẵn, hoặc 360–1200 px)
  $description: Manually set the Run box width in pixels/DIPs (e.g. 495, 560, 616, 720). Set to 0 to follow the 85%–120% UI Size preset.
  $description:vi-VN: Tự chỉnh chiều rộng hộp thoại Run theo pixel/DIP (ví dụ 495, 560, 616, 720). Để 0 nếu muốn dùng theo mức 85%–120% có sẵn.
- customBarHeight: 0
  $name: Appearance · Custom Height in px (0 = Follow Preset, or 36–96 bar / 150–600 total)
  $name:vi-VN: Chiều cao hộp thoại tùy chỉnh (0 = Theo thiết lập sẵn, hoặc 36–96 px thanh lệnh)
  $description: Manually set the command bar height (36–96 px, default 52) or total height (150–600 px), scaling row height proportionally. Set to 0 to follow the UI Size preset.
  $description:vi-VN: Tự chỉnh chiều cao thanh nhập lệnh (36–96 px, chuẩn 52 px) hoặc tổng chiều cao (150–600 px); chiều cao từng dòng lịch sử sẽ tự co giãn tương ứng. Để 0 để theo mức có sẵn.
- customFontSize: 0
  $name: Appearance · Custom Font Size in pt (0 = Follow Preset, or 9–24)
  $name:vi-VN: Cỡ chữ tùy chỉnh (0 = Theo thiết lập sẵn, hoặc 9–24 pt)
  $description: Manually set the text font size (9–24 pt, default 14). Set to 0 to follow the UI Size preset.
  $description:vi-VN: Tự chỉnh cỡ chữ hiển thị trong hộp thoại Run (9–24 pt, chuẩn 14). Để 0 để tự động theo mức kích thước có sẵn.
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
#include <shlobj.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
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

#include <cstdint>
#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
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

// Classic Windows Run Dialog resource & control IDs in shell32.dll
constexpr WORD kRunDialogResourceId = 1003;
constexpr int kRunDlgComboId        = 12298; // 0x300A (IDD_COMMAND)
constexpr int kRunDlgBrowseId       = 12288; // 0x3000 (IDD_BROWSE)

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

extern "C" IMAGE_DOS_HEADER __ImageBase;

inline HINSTANCE GetModInstanceHandle() {
    return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
using ShowWindow_t = decltype(&ShowWindow);
using SetForegroundWindow_t = decltype(&SetForegroundWindow);
using SwitchToThisWindow_t = decltype(&SwitchToThisWindow);
using DialogBoxParamW_t = decltype(&DialogBoxParamW);

ShowWindow_t ShowWindow_Orig = nullptr;
SetForegroundWindow_t SetForegroundWindow_Orig = nullptr;
SwitchToThisWindow_t SwitchToThisWindow_Orig = nullptr;
DialogBoxParamW_t DialogBoxParamW_Orig = nullptr;

HMODULE g_hShell32 = nullptr;
bool g_fluentMenuClassRegistered = false;
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
    const wchar_t* appTag;
};

struct ModSettings {
    std::wstring language = L"auto";
    SurfaceTheme surfaceTheme = SurfaceTheme::Glass;
    ColorMode colorMode = ColorMode::Auto;
    bool minimalMode = false;
    int uiScalePercent = 100;
    int customDialogWidth = 0;
    int customBarHeight = 0;
    int customFontSize = 0;
    bool taskbarCloseInsteadOfMinimize = false;
    int glassOpacity = -100;
    int blurOpacity = 0;
    std::wstring customBackgroundPath;
    int customBackgroundMaxMb = 50;
    int customBackgroundOpacity = 255;
    bool enableAppSuggestions = true;
    int maxHistoryItems = 35;
    int visibleHistoryRows = 5;
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
    L"Programs (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0All Files (*.*)\0*.*\0\0",
    L"Application"
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
constexpr int kBaseDlgWidth         = 616;
constexpr int kBaseRowHeight        = 38;
constexpr int kBaseListPadTop       = 8;
constexpr int kBaseListPadBottom    = 8;
constexpr int kBaseInputBarHeight   = 52;
constexpr UINT_PTR kTimerCaret      = 9010;
constexpr UINT_PTR kTimerLuminance  = 9011;
constexpr UINT_PTR kTimerBgMedia    = 9012;
constexpr int kModernEditId         = 9901;
constexpr size_t kMaxMruEntries     = 50;

int DpiScale(int value, UINT dpi, int uiScalePercent = 100) {
    return MulDiv(value, static_cast<int>(dpi) * std::clamp(uiScalePercent, 80, 140), 96 * 100);
}

float DpiScaleF(float value, UINT dpi, int uiScalePercent = 100) {
    return value * (static_cast<float>(dpi) / 96.0f) * (std::clamp(uiScalePercent, 80, 140) / 100.0f);
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
    return D2D1::ColorF(0.10f, 0.47f, 0.83f, 1.0f);
}

// --- Real-Time iOS 26 Optical Liquid Glass Snapshot, Gaussian Blur & 3D Snell's-Law Lens Refraction Engine ---

struct CleanDesktopSnapshot {
    std::vector<DWORD> pixels;
    int monX = 0;
    int monY = 0;
    int monW = 0;
    int monH = 0;
    ULONGLONG captureTick = 0;
    bool valid = false;
};

static std::mutex g_desktopSnapMutex;
static CleanDesktopSnapshot g_desktopSnap;

struct BevelPixelLUT {
    uint32_t dstIdx;
    int16_t dxR, dyR;
    int16_t dxG, dyG;
    int16_t dxB, dyB;
    uint8_t crispMix;
    uint8_t alpha;
    uint16_t invFr;
    uint16_t frR_255, frG_255, frB_255;
    uint16_t trR_a, trG_a, trB_a;
    uint8_t sheenR, sheenG, sheenB;
};

struct InteriorRowSpan {
    int y;
    int xStart, xEnd;
    int dyG;
};

struct OpticalLensGeometryLUT {
    int w = 0;
    int h = 0;
    UINT dpi = 0;
    int glassPct = -9999;
    bool isLight = false;
    bool isMenu = false;
    uint8_t interiorAlpha = 0;
    std::vector<int16_t> interiorDxG;
    std::vector<uint8_t> sheenMap;
    std::vector<InteriorRowSpan> interiorSpans;
    std::vector<BevelPixelLUT> bevelPixels;
};

struct PreblurredMonitorCache {
    std::vector<DWORD> softRGB;
    std::vector<DWORD> crispRGB;
    std::vector<DWORD> softPremul;
    std::vector<DWORD> blurTmp;
    int monX = 0;
    int monY = 0;
    int monW = 0;
    int monH = 0;
    int halfW = 0;
    int halfH = 0;
    int softRad = -1;
    int crispRad = -1;
    int glassPct = -9999;
    uint8_t bakedAlpha = 0;
    bool isLight = false;
    ULONGLONG snapTick = 0;
};

struct OpticalGlassCache {
    std::vector<DWORD> premulPixels;
    OpticalLensGeometryLUT lut;
    int wndX = -99999;
    int wndY = -99999;
    int w = 0;
    int h = 0;
    int glassPct = -9999;
    bool isLight = false;
    bool isMenu = false;
    ULONGLONG snapTick = 0;
};

static std::mutex g_opticalCacheMutex;
static PreblurredMonitorCache g_dlgMonitorBlurCache;
static PreblurredMonitorCache g_menuMonitorBlurCache;
static OpticalGlassCache g_dlgOpticalCache;
static OpticalGlassCache g_menuMainOpticalCache;
static OpticalGlassCache g_menuSubOpticalCache;

extern std::atomic<HWND> g_hActiveGearMenu;
extern std::atomic<HWND> g_hActiveCustomSizeWnd;

void ReleaseBackdropSnapshot() {
    {
        std::lock_guard<std::mutex> lock(g_desktopSnapMutex);
        g_desktopSnap = {};
    }
    std::lock_guard<std::mutex> lock(g_opticalCacheMutex);
    g_dlgMonitorBlurCache = {};
    g_menuMonitorBlurCache = {};
    g_dlgOpticalCache = {};
    g_menuMainOpticalCache = {};
    g_menuSubOpticalCache = {};
}

void CaptureCleanMonitorBackdrop(HWND hRefWnd, bool forceExcludeVisible = false) {
    HMONITOR hMon = hRefWnd && IsWindow(hRefWnd)
        ? MonitorFromWindow(hRefWnd, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    int monX = 0, monY = 0, monW = GetSystemMetrics(SM_CXSCREEN), monH = GetSystemMetrics(SM_CYSCREEN);
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        monX = mi.rcMonitor.left;
        monY = mi.rcMonitor.top;
        monW = mi.rcMonitor.right - mi.rcMonitor.left;
        monH = mi.rcMonitor.bottom - mi.rcMonitor.top;
    }
    if (monW <= 0 || monH <= 0 || monW > 7680 || monH > 4320) return;

    std::vector<HWND> excludedHwnds;
    auto excludeWndIfVisible = [&](HWND w) {
        if (w && IsWindow(w) && IsWindowVisible(w)) {
            SetWindowDisplayAffinity(w, 0x00000011 /*WDA_EXCLUDEFROMCAPTURE*/);
            excludedHwnds.push_back(w);
        }
    };

    if (forceExcludeVisible) {
        excludeWndIfVisible(hRefWnd);
        excludeWndIfVisible(g_hActiveGearMenu.load());
        excludeWndIfVisible(g_hActiveCustomSizeWnd.load());
        if (!excludedHwnds.empty()) {
            // Double DwmFlush guarantees DWM has committed WDA_EXCLUDEFROMCAPTURE to the desktop surface
            // before BitBlt reads hScreenDC, preventing any self-capture saturation feedback loop.
            DwmFlush();
            DwmFlush();
        }
    }

    HDC hScreenDC = GetDC(nullptr);
    if (hScreenDC) {
        HDC hMemDC = CreateCompatibleDC(hScreenDC);
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = monW;
        bmi.bmiHeader.biHeight = -monH;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        DWORD* dibPixels = nullptr;
        HBITMAP hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, (void**)&dibPixels, nullptr, 0);
        if (hBmp && dibPixels) {
            HGDIOBJ oldBmp = SelectObject(hMemDC, hBmp);
            if (BitBlt(hMemDC, 0, 0, monW, monH, hScreenDC, monX, monY, SRCCOPY)) {
                GdiFlush();
                std::lock_guard<std::mutex> lock(g_desktopSnapMutex);
                g_desktopSnap.monX = monX;
                g_desktopSnap.monY = monY;
                g_desktopSnap.monW = monW;
                g_desktopSnap.monH = monH;
                g_desktopSnap.pixels.assign(dibPixels, dibPixels + (size_t)monW * (size_t)monH);
                g_desktopSnap.captureTick = GetTickCount64();
                g_desktopSnap.valid = true;
            }
            SelectObject(hMemDC, oldBmp);
            DeleteObject(hBmp);
        }
        DeleteDC(hMemDC);
        ReleaseDC(nullptr, hScreenDC);
    }

    for (HWND w : excludedHwnds) {
        if (IsWindow(w)) {
            SetWindowDisplayAffinity(w, 0x00000000 /*WDA_NONE*/);
        }
    }
}

// Ultra-fast 3-pass separable sliding-window box blur using fixed-point reciprocal multiplication
void BlurBufferSeparableRGB(std::vector<DWORD>& buf, std::vector<DWORD>& tmp, int w, int h, int radius) {
    if (radius <= 0 || w <= 2 || h <= 2) return;
    radius = std::clamp(radius, 1, (std::min)(w - 1, h - 1) / 2);
    if (radius <= 0) return;
    const uint32_t diam = (uint32_t)(radius * 2 + 1);
    const uint32_t invDiam = (1u << 24) / diam + 1u;

    for (int pass = 0; pass < 3; ++pass) {
        // Horizontal pass: buf -> tmp
        for (int y = 0; y < h; ++y) {
            const DWORD* rowIn = &buf[(size_t)y * w];
            DWORD* rowOut = &tmp[(size_t)y * w];
            uint32_t sumR = 0, sumG = 0, sumB = 0;
            for (int k = -radius; k <= radius; ++k) {
                int idx = std::clamp(k, 0, w - 1);
                DWORD p = rowIn[idx];
                sumR += (p >> 16) & 0xFF;
                sumG += (p >> 8) & 0xFF;
                sumB += p & 0xFF;
            }
            for (int x = 0; x < w; ++x) {
                DWORD r = (sumR * invDiam) >> 24;
                DWORD g = (sumG * invDiam) >> 24;
                DWORD b = (sumB * invDiam) >> 24;
                rowOut[x] = (r << 16) | (g << 8) | b;

                int outIdx = std::clamp(x - radius, 0, w - 1);
                int inIdx  = std::clamp(x + radius + 1, 0, w - 1);
                DWORD pOut = rowIn[outIdx];
                DWORD pIn  = rowIn[inIdx];
                sumR += ((pIn >> 16) & 0xFF) - ((pOut >> 16) & 0xFF);
                sumG += ((pIn >> 8) & 0xFF)  - ((pOut >> 8) & 0xFF);
                sumB += (pIn & 0xFF)         - (pOut & 0xFF);
            }
        }
        // Vertical pass: tmp -> buf
        for (int x = 0; x < w; ++x) {
            uint32_t sumR = 0, sumG = 0, sumB = 0;
            for (int k = -radius; k <= radius; ++k) {
                int iy = std::clamp(k, 0, h - 1);
                DWORD p = tmp[(size_t)iy * w + x];
                sumR += (p >> 16) & 0xFF;
                sumG += (p >> 8) & 0xFF;
                sumB += p & 0xFF;
            }
            for (int y = 0; y < h; ++y) {
                DWORD r = (sumR * invDiam) >> 24;
                DWORD g = (sumG * invDiam) >> 24;
                DWORD b = (sumB * invDiam) >> 24;
                buf[(size_t)y * w + x] = (r << 16) | (g << 8) | b;

                int outY = std::clamp(y - radius, 0, h - 1);
                int inY  = std::clamp(y + radius + 1, 0, h - 1);
                DWORD pOut = tmp[(size_t)outY * w + x];
                DWORD pIn  = tmp[(size_t)inY * w + x];
                sumR += ((pIn >> 16) & 0xFF) - ((pOut >> 16) & 0xFF);
                sumG += ((pIn >> 8) & 0xFF)  - ((pOut >> 8) & 0xFF);
                sumB += (pIn & 0xFF)         - (pOut & 0xFF);
            }
        }
    }
}

void BuildOpticalLensGeometryLUT(OpticalLensGeometryLUT& lut, int w, int h, UINT dpi, int glassPct, bool isLight, bool isMenuPanel) {
    lut.w = w;
    lut.h = h;
    lut.dpi = dpi;
    lut.glassPct = glassPct;
    lut.isLight = isLight;
    lut.isMenu = isMenuPanel;

    float negF = (glassPct < 0) ? (std::clamp(-glassPct, 0, 100) / 100.0f) : 0.0f;
    const float cornerR = (float)dpi * 8.0f / 96.0f;
    const float cx = w * 0.5f;
    const float cy = h * 0.5f;
    const float hxBox = (std::max)(1.0f, cx - cornerR);
    const float hyBox = (std::max)(1.0f, cy - cornerR);

    const float bevelW = ((float)dpi / 96.0f) * (20.0f + 10.0f * negF);
    const float maxDisp = ((float)dpi / 96.0f) * (16.0f + 20.0f * negF);
    const float zoom = 1.032f + 0.045f * negF;
    const float invZoomDelta = 1.0f - (1.0f / zoom);

    const float interiorOpacity = (glassPct < 0)
        ? std::clamp(0.24f + 0.74f * std::pow(negF, 0.52f), 0.24f, 0.98f)
        : 0.0f;
    lut.interiorAlpha = (uint8_t)std::clamp((int)std::round(interiorOpacity * 255.0f), 0, 255);

    lut.interiorDxG.resize(w);
    for (int x = 0; x < w; ++x) {
        float dx = (x + 0.5f) - cx;
        float baseShiftX = -dx * invZoomDelta;
        lut.interiorDxG[x] = (int16_t)std::round(((float)x + baseShiftX) * 0.5f);
    }

    lut.sheenMap.resize((size_t)w * (size_t)h);
    lut.interiorSpans.clear();
    lut.bevelPixels.clear();
    lut.bevelPixels.reserve((size_t)(w + h) * 48);

    for (int y = 0; y < h; ++y) {
        float dy = (y + 0.5f) - cy;
        float qy = std::fabs(dy) - hyBox;
        float oy = (std::max)(qy, 0.0f);
        float baseShiftY = -dy * invZoomDelta;
        int rowDyG = (int)std::round(((float)y + baseShiftY) * 0.5f);
        float v = (float)y / (float)(std::max)(1, h);

        int spanStart = -1;
        for (int x = 0; x < w; ++x) {
            float u = (float)x / (float)(std::max)(1, w);
            float diag = u * 0.55f + v * 0.45f;
            float sheen = (diag < 0.38f)
                ? ((1.0f - diag / 0.38f) * (12.0f + 8.0f * negF))
                : ((diag > 0.72f) ? (((diag - 0.72f) / 0.28f) * (7.0f + 5.0f * negF)) : 0.0f);
            uint8_t sheenU8 = (uint8_t)std::clamp((int)std::round(sheen), 0, 32);
            lut.sheenMap[(size_t)y * w + x] = sheenU8;

            float dx = (x + 0.5f) - cx;
            float qx = std::fabs(dx) - hxBox;
            float ox = (std::max)(qx, 0.0f);

            float outDist = std::sqrt(ox * ox + oy * oy);
            float inDist  = (std::min)((std::max)(qx, qy), 0.0f);
            float sdf     = outDist + inDist - cornerR;
            float distFromEdge = (std::max)(0.0f, -sdf);

            float tBevel = (distFromEdge < bevelW) ? (1.0f - distFromEdge / bevelW) : 0.0f;
            if (tBevel <= 0.0f) {
                if (spanStart < 0) spanStart = x;
                continue;
            }
            if (spanStart >= 0) {
                lut.interiorSpans.push_back({ y, spanStart, x, rowDyG });
                spanStart = -1;
            }

            float bevelAlpha = std::clamp(tBevel * tBevel * (0.65f + 0.33f * negF), 0.0f, 0.98f);
            float pixelAlpha = (std::max)(interiorOpacity, bevelAlpha);
            if (pixelAlpha <= 0.004f) continue;

            float nx = 0.0f, ny = 0.0f;
            if (outDist > 1e-4f) {
                nx = (dx >= 0.0f ? 1.0f : -1.0f) * (ox / outDist);
                ny = (dy >= 0.0f ? 1.0f : -1.0f) * (oy / outDist);
            } else {
                float maxQ = (std::max)(qx, qy);
                float wx = std::exp(std::clamp((qx - maxQ) * 0.28f, -20.0f, 0.0f));
                float wy = std::exp(std::clamp((qy - maxQ) * 0.28f, -20.0f, 0.0f));
                nx = (dx >= 0.0f ? 1.0f : -1.0f) * wx;
                ny = (dy >= 0.0f ? 1.0f : -1.0f) * wy;
                float nLen = std::sqrt(nx * nx + ny * ny);
                if (nLen > 1e-4f) { nx /= nLen; ny /= nLen; }
            }

            float baseShiftX = -dx * invZoomDelta;
            float lensCurve = tBevel * tBevel * (0.40f + 0.60f * tBevel);
            float dispX = -nx * lensCurve * maxDisp;
            float dispY = -ny * lensCurve * maxDisp;

            BevelPixelLUT bp{};
            bp.dstIdx = (uint32_t)((size_t)y * w + x);
            bp.dxR = (int16_t)std::round(((float)x + baseShiftX + dispX * 0.80f) * 0.5f);
            bp.dyR = (int16_t)std::round(((float)y + baseShiftY + dispY * 0.80f) * 0.5f);
            bp.dxG = (int16_t)std::round(((float)x + baseShiftX + dispX * 1.00f) * 0.5f);
            bp.dyG = (int16_t)std::round(((float)y + baseShiftY + dispY * 1.00f) * 0.5f);
            bp.dxB = (int16_t)std::round(((float)x + baseShiftX + dispX * 1.22f) * 0.5f);
            bp.dyB = (int16_t)std::round(((float)y + baseShiftY + dispY * 1.22f) * 0.5f);

            if (tBevel > 0.30f) {
                float cm = std::clamp((tBevel - 0.30f) / 0.70f, 0.0f, 1.0f);
                cm = cm * cm * (0.42f + 0.25f * negF);
                bp.crispMix = (uint8_t)std::clamp((int)std::round(cm * 255.0f), 0, 255);
            } else {
                bp.crispMix = 0;
            }

            float lightTL = (std::max)(0.0f, -0.66f * nx - 0.75f * ny);
            float lightBR = (std::max)(0.0f,  0.66f * nx + 0.75f * ny);
            float fresnel = std::clamp(std::pow(tBevel, 1.55f) * (0.15f + 0.30f * lightTL + 0.20f * lightBR) * (1.0f + 0.28f * negF), 0.0f, 0.82f);
            uint32_t frU8 = (uint32_t)std::clamp((int)std::round(fresnel * 256.0f), 0, 256);
            bp.invFr = (uint16_t)(256u - frU8);
            bp.frR_255 = (uint16_t)std::round((232.0f + 23.0f * lightBR) * frU8);
            bp.frG_255 = (uint16_t)std::round((244.0f - 10.0f * lightBR) * frU8);
            bp.frB_255 = (uint16_t)(255u * frU8);

            bp.alpha = (uint8_t)std::clamp((int)std::round(pixelAlpha * 255.0f), 1, 255);
            bp.trR_a = bp.alpha;
            bp.trG_a = bp.alpha;
            bp.trB_a = bp.alpha;

            float bevelSheen = sheen * tBevel;
            bp.sheenR = (uint8_t)std::clamp((int)std::round(bevelSheen * 0.92f), 0, 32);
            bp.sheenG = (uint8_t)std::clamp((int)std::round(bevelSheen * 0.97f), 0, 32);
            bp.sheenB = (uint8_t)std::clamp((int)std::round(bevelSheen * 1.02f), 0, 32);
            lut.bevelPixels.push_back(bp);
        }
        if (spanStart >= 0) {
            lut.interiorSpans.push_back({ y, spanStart, w, rowDyG });
        }
    }
}

void EnsurePreblurredMonitorCache(PreblurredMonitorCache& mcache, int softRad, int crispRad,
                                  int glassPct, uint8_t interiorAlpha, bool isLight, ULONGLONG snapTick) {
    if (mcache.snapTick == snapTick && mcache.softRad == softRad && mcache.crispRad == crispRad &&
        mcache.glassPct == glassPct && mcache.bakedAlpha == interiorAlpha && mcache.isLight == isLight &&
        !mcache.softRGB.empty()) {
        return;
    }

    int monX = 0, monY = 0, monW = 0, monH = 0;
    {
        std::lock_guard<std::mutex> snapLock(g_desktopSnapMutex);
        if (!g_desktopSnap.valid || g_desktopSnap.pixels.empty()) return;
        monX = g_desktopSnap.monX;
        monY = g_desktopSnap.monY;
        monW = g_desktopSnap.monW;
        monH = g_desktopSnap.monH;
        const int halfW = (std::max)(4, monW / 2);
        const int halfH = (std::max)(4, monH / 2);

        mcache.monX = monX;
        mcache.monY = monY;
        mcache.monW = monW;
        mcache.monH = monH;
        mcache.halfW = halfW;
        mcache.halfH = halfH;
        mcache.softRad = softRad;
        mcache.crispRad = crispRad;
        mcache.glassPct = glassPct;
        mcache.bakedAlpha = interiorAlpha;
        mcache.isLight = isLight;
        mcache.snapTick = snapTick;

        const size_t totalHalf = (size_t)halfW * (size_t)halfH;
        mcache.softRGB.resize(totalHalf);
        mcache.crispRGB.resize(totalHalf);
        mcache.softPremul.resize(totalHalf);
        mcache.blurTmp.resize(totalHalf);

        const DWORD* snapPx = g_desktopSnap.pixels.data();
        for (int hy = 0; hy < halfH; ++hy) {
            int sy0 = std::clamp(hy * 2, 0, monH - 1);
            int sy1 = std::clamp(sy0 + 1, 0, monH - 1);
            const DWORD* row0 = &snapPx[(size_t)sy0 * monW];
            const DWORD* row1 = &snapPx[(size_t)sy1 * monW];
            DWORD* dstSoft = &mcache.softRGB[(size_t)hy * halfW];
            DWORD* dstCrisp = &mcache.crispRGB[(size_t)hy * halfW];
            for (int hx = 0; hx < halfW; ++hx) {
                int sx0 = std::clamp(hx * 2, 0, monW - 1);
                int sx1 = std::clamp(sx0 + 1, 0, monW - 1);
                DWORD p00 = row0[sx0], p10 = row0[sx1], p01 = row1[sx0], p11 = row1[sx1];
                DWORD r = (((p00 >> 16) & 0xFF) + ((p10 >> 16) & 0xFF) + ((p01 >> 16) & 0xFF) + ((p11 >> 16) & 0xFF)) >> 2;
                DWORD g = (((p00 >> 8) & 0xFF)  + ((p10 >> 8) & 0xFF)  + ((p01 >> 8) & 0xFF)  + ((p11 >> 8) & 0xFF))  >> 2;
                DWORD b = ((p00 & 0xFF)         + (p10 & 0xFF)         + (p01 & 0xFF)         + (p11 & 0xFF))         >> 2;
                DWORD packed = (r << 16) | (g << 8) | b;
                dstSoft[hx] = packed;
                dstCrisp[hx] = packed;
            }
        }
    }

    BlurBufferSeparableRGB(mcache.softRGB, mcache.blurTmp, mcache.halfW, mcache.halfH, softRad);
    BlurBufferSeparableRGB(mcache.crispRGB, mcache.blurTmp, mcache.halfW, mcache.halfH, crispRad);

    // Pre-apply natural iOS 26 vibrancy & subtle glass body tint across the half-res monitor buffers once
    float negF = (glassPct < 0) ? (std::clamp(-glassPct, 0, 100) / 100.0f) : 0.0f;
    float posF = (glassPct > 0) ? (std::clamp(glassPct, 0, 100) / 100.0f) : 0.0f;
    const float satBoost = 1.12f + 0.12f * negF - 0.10f * posF;
    const float tintWeight = (glassPct < 0)
        ? (0.18f * (1.0f - negF) * (1.0f - 0.45f * negF) + 0.02f)
        : (0.20f + 0.48f * posF);
    const int satScale = (int)std::round(satBoost * 256.0f);
    const int tintW = (int)std::round(tintWeight * 256.0f);
    const int invTintW = 256 - tintW;
    const int tintR = (isLight ? 242 : 18) * tintW;
    const int tintG = (isLight ? 245 : 23) * tintW;
    const int tintB = (isLight ? 252 : 34) * tintW;
    const uint32_t intA = interiorAlpha;

    const size_t totalHalf = mcache.softRGB.size();
    for (size_t i = 0; i < totalHalf; ++i) {
        {
            DWORD p = mcache.softRGB[i];
            int r = (p >> 16) & 0xFF;
            int g = (p >> 8)  & 0xFF;
            int b =  p        & 0xFF;
            int luma = (77 * r + 150 * g + 29 * b) >> 8;
            r = std::clamp(luma + (((r - luma) * satScale) >> 8), 0, 255);
            g = std::clamp(luma + (((g - luma) * satScale) >> 8), 0, 255);
            b = std::clamp(luma + (((b - luma) * satScale) >> 8), 0, 255);
            r = std::clamp((r * invTintW + tintR) >> 8, 0, 255);
            g = std::clamp((g * invTintW + tintG) >> 8, 0, 255);
            b = std::clamp((b * invTintW + tintB) >> 8, 0, 255);
            mcache.softRGB[i] = ((DWORD)r << 16) | ((DWORD)g << 8) | (DWORD)b;
            mcache.softPremul[i] = (intA << 24)
                | ((((uint32_t)r * intA) >> 8) << 16)
                | ((((uint32_t)g * intA) >> 8) << 8)
                |  (((uint32_t)b * intA) >> 8);
        }
        {
            DWORD p = mcache.crispRGB[i];
            int r = (p >> 16) & 0xFF;
            int g = (p >> 8)  & 0xFF;
            int b =  p        & 0xFF;
            int luma = (77 * r + 150 * g + 29 * b) >> 8;
            r = std::clamp(luma + (((r - luma) * satScale) >> 8), 0, 255);
            g = std::clamp(luma + (((g - luma) * satScale) >> 8), 0, 255);
            b = std::clamp(luma + (((b - luma) * satScale) >> 8), 0, 255);
            r = std::clamp((r * invTintW + tintR) >> 8, 0, 255);
            g = std::clamp((g * invTintW + tintG) >> 8, 0, 255);
            b = std::clamp((b * invTintW + tintB) >> 8, 0, 255);
            mcache.crispRGB[i] = ((DWORD)r << 16) | ((DWORD)g << 8) | (DWORD)b;
        }
    }
}

// Renders zero-latency iOS 26 Control Center Liquid Glass onto pRT using pre-blurred monitor buffers & LUT
void DrawOpticalLiquidGlassBackdrop(ID2D1RenderTarget* pRT, HWND hWnd, int w, int h,
                                    UINT dpi, int glassPct, bool isLight, bool isMenuPanel, int cacheSlot) {
    if (!pRT || !hWnd || w <= 0 || h <= 0) return;

    RECT rcWnd = {};
    GetWindowRect(hWnd, &rcWnd);
    int wndX = rcWnd.left;
    int wndY = rcWnd.top;

    bool needCapture = false;
    ULONGLONG snapTick = 0;
    {
        std::lock_guard<std::mutex> lock(g_desktopSnapMutex);
        needCapture = (!g_desktopSnap.valid || g_desktopSnap.pixels.empty());
        snapTick = g_desktopSnap.captureTick;
    }
    if (needCapture) {
        CaptureCleanMonitorBackdrop(hWnd, true);
        std::lock_guard<std::mutex> lock(g_desktopSnapMutex);
        if (!g_desktopSnap.valid || g_desktopSnap.pixels.empty()) return;
        snapTick = g_desktopSnap.captureTick;
    }

    OpticalGlassCache* cache = (cacheSlot == 0) ? &g_dlgOpticalCache
                             : ((cacheSlot == 1) ? &g_menuMainOpticalCache : &g_menuSubOpticalCache);
    PreblurredMonitorCache& mcache = isMenuPanel ? g_menuMonitorBlurCache : g_dlgMonitorBlurCache;

    std::lock_guard<std::mutex> cacheLock(g_opticalCacheMutex);
    if (cache->w != w || cache->h != h || cache->wndX != wndX || cache->wndY != wndY ||
        cache->glassPct != glassPct || cache->isLight != isLight || cache->isMenu != isMenuPanel ||
        cache->snapTick != snapTick || (int)cache->premulPixels.size() != w * h) {

        bool shapeOrStyleChanged = (cache->lut.w != w || cache->lut.h != h || cache->lut.dpi != dpi ||
                                    cache->lut.glassPct != glassPct || cache->lut.isLight != isLight ||
                                    cache->lut.isMenu != isMenuPanel);
        if (shapeOrStyleChanged) {
            BuildOpticalLensGeometryLUT(cache->lut, w, h, dpi, glassPct, isLight, isMenuPanel);
        }

        float negF = (glassPct < 0) ? (std::clamp(-glassPct, 0, 100) / 100.0f) : 0.0f;
        float posF = (glassPct > 0) ? (std::clamp(glassPct, 0, 100) / 100.0f) : 0.0f;
        float softRadF = (8.0f - 4.2f * negF + 4.0f * posF + (isMenuPanel ? 1.5f : 0.0f)) * ((float)dpi / 96.0f);
        int softRad = std::clamp((int)std::round(softRadF), 3, 18);
        int crispRad = std::clamp(softRad / 2, 1, 6);

        EnsurePreblurredMonitorCache(mcache, softRad, crispRad, glassPct, cache->lut.interiorAlpha, isLight, snapTick);
        if (mcache.softRGB.empty() || mcache.halfW <= 0 || mcache.halfH <= 0) return;

        cache->w = w;
        cache->h = h;
        cache->wndX = wndX;
        cache->wndY = wndY;
        cache->glassPct = glassPct;
        cache->isLight = isLight;
        cache->isMenu = isMenuPanel;
        cache->snapTick = snapTick;
        cache->premulPixels.resize((size_t)w * (size_t)h);

        const int bw = mcache.halfW;
        const int bh = mcache.halfH;
        const int hx0 = (wndX - mcache.monX) >> 1;
        const int hy0 = (wndY - mcache.monY) >> 1;

        // 1. Ultra-fast interior spans (< 0.19 ms for the entire window!)
        if (cache->lut.interiorAlpha > 0) {
            const int16_t* dxArr = cache->lut.interiorDxG.data();
            const DWORD* premulBase = mcache.softPremul.data();
            for (const auto& sp : cache->lut.interiorSpans) {
                int sy = std::clamp(hy0 + sp.dyG, 0, bh - 1);
                const DWORD* srcRow = &premulBase[(size_t)sy * bw];
                DWORD* dstRow = &cache->premulPixels[(size_t)sp.y * w];
                if (hx0 + dxArr[sp.xStart] >= 0 && hx0 + dxArr[sp.xEnd - 1] < bw) {
                    const DWORD* srcOffsetRow = srcRow + hx0;
                    for (int x = sp.xStart; x < sp.xEnd; ++x) {
                        dstRow[x] = srcOffsetRow[dxArr[x]];
                    }
                } else {
                    for (int x = sp.xStart; x < sp.xEnd; ++x) {
                        dstRow[x] = srcRow[std::clamp(hx0 + dxArr[x], 0, bw - 1)];
                    }
                }
            }
        } else if (shapeOrStyleChanged) {
            std::fill(cache->premulPixels.begin(), cache->premulPixels.end(), 0);
        }

        // 2. Sparse 3D Snell's-law bevel pixels with chromatic dispersion & Fresnel rim cushion
        const DWORD* softData  = mcache.softRGB.data();
        const DWORD* crispData = mcache.crispRGB.data();
        DWORD* outData = cache->premulPixels.data();

        for (const auto& bp : cache->lut.bevelPixels) {
            int sxR = std::clamp(hx0 + bp.dxR, 0, bw - 1);
            int syR = std::clamp(hy0 + bp.dyR, 0, bh - 1);
            int sxG = std::clamp(hx0 + bp.dxG, 0, bw - 1);
            int syG = std::clamp(hy0 + bp.dyG, 0, bh - 1);
            int sxB = std::clamp(hx0 + bp.dxB, 0, bw - 1);
            int syB = std::clamp(hy0 + bp.dyB, 0, bh - 1);

            uint32_t r = (softData[(size_t)syR * bw + sxR] >> 16) & 0xFF;
            uint32_t g = (softData[(size_t)syG * bw + sxG] >> 8)  & 0xFF;
            uint32_t b =  softData[(size_t)syB * bw + sxB]        & 0xFF;

            if (bp.crispMix > 0) {
                uint32_t rC = (crispData[(size_t)syR * bw + sxR] >> 16) & 0xFF;
                uint32_t gC = (crispData[(size_t)syG * bw + sxG] >> 8)  & 0xFF;
                uint32_t bC =  crispData[(size_t)syB * bw + sxB]        & 0xFF;
                uint32_t cm = bp.crispMix, invCm = 256 - cm;
                r = (r * invCm + rC * cm) >> 8;
                g = (g * invCm + gC * cm) >> 8;
                b = (b * invCm + bC * cm) >> 8;
            }

            r = (std::min)(255u, r + bp.sheenR);
            g = (std::min)(255u, g + bp.sheenG);
            b = (std::min)(255u, b + bp.sheenB);

            uint32_t invFr = bp.invFr;
            r = (r * invFr + bp.frR_255) >> 8;
            g = (g * invFr + bp.frG_255) >> 8;
            b = (b * invFr + bp.frB_255) >> 8;

            uint32_t aByte = bp.alpha;
            uint32_t rByte = (std::min)(aByte, (r * bp.trR_a) >> 8);
            uint32_t gByte = (std::min)(aByte, (g * bp.trG_a) >> 8);
            uint32_t bByte = (std::min)(aByte, (b * bp.trB_a) >> 8);
            outData[bp.dstIdx] = (aByte << 24) | (rByte << 16) | (gByte << 8) | bByte;
        }
    }

    D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
    ID2D1Bitmap* pGlassBmp = nullptr;
    if (SUCCEEDED(pRT->CreateBitmap(D2D1::SizeU((UINT32)w, (UINT32)h),
                                    cache->premulPixels.data(), (UINT32)(w * 4),
                                    &bmpProps, &pGlassBmp)) && pGlassBmp) {
        D2D1_RECT_F dstRect = D2D1::RectF(0.0f, 0.0f, (float)w, (float)h);
        pRT->DrawBitmap(pGlassBmp, dstRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        pGlassBmp->Release();
    }
}

// Computes a pure Win32 window-topology signature of visible background windows on the monitor
// (Zero GetPixel calls -> immune to mouse hover, pointer glow, DWM shadows, or open menu popups!)
uint64_t GetBackdropWindowTopologySignature(HWND hDlg) {
    HMONITOR hMon = (hDlg && IsWindow(hDlg))
        ? MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    RECT rcMon = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        rcMon = mi.rcMonitor;
    }

    DWORD selfPid = GetCurrentProcessId();
    HWND hGearMenu = g_hActiveGearMenu.load();
    HWND hSizeWnd  = g_hActiveCustomSizeWnd.load();

    uint64_t hash = 14695981039346656037ULL;
    auto mixHash = [&](uint64_t v) {
        hash ^= v;
        hash *= 1099511628211ULL;
    };

    for (HWND w = GetTopWindow(nullptr); w != nullptr; w = GetWindow(w, GW_HWNDNEXT)) {
        if (w == hDlg || w == hGearMenu || w == hSizeWnd) continue;
        if (!IsWindowVisible(w) || IsIconic(w)) continue;
        if (hDlg && GetWindow(w, GW_OWNER) == hDlg) continue;

        LONG_PTR exStyle = GetWindowLongPtrW(w, GWL_EXSTYLE);
        if (exStyle & WS_EX_TOOLWINDOW) continue;

        DWORD cloaked = 0;
        if (SUCCEEDED(DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked != 0) {
            continue;
        }

        RECT rcW = {};
        if (!GetWindowRect(w, &rcW) || rcW.right <= rcW.left || rcW.bottom <= rcW.top) continue;

        RECT rcInter = {};
        if (!IntersectRect(&rcInter, &rcW, &rcMon)) continue;

        DWORD wndPid = 0;
        GetWindowThreadProcessId(w, &wndPid);
        if (wndPid == selfPid) {
            wchar_t cls[64] = {};
            if (GetClassNameW(w, cls, ARRAYSIZE(cls))) {
                // In explorer.exe, only track real File Explorer folder windows and the Desktop
                if (_wcsicmp(cls, L"CabinetWClass") != 0 &&
                    _wcsicmp(cls, L"ExploreWClass") != 0 &&
                    _wcsicmp(cls, L"Progman") != 0 &&
                    _wcsicmp(cls, L"WorkerW") != 0) {
                    continue;
                }
            }
        }

        mixHash((uint64_t)(uintptr_t)w);
        mixHash((uint64_t)(int64_t)rcW.left);
        mixHash((uint64_t)(int64_t)rcW.top);
        mixHash((uint64_t)(int64_t)rcW.right);
        mixHash((uint64_t)(int64_t)rcW.bottom);
        mixHash((uint64_t)(IsZoomed(w) ? 1 : 0));
    }
    return hash;
}

// Samples true background luminance directly from g_desktopSnap pixels directly behind the dialog
bool SampleBackdropLuminanceFromSnap(int wndX, int wndY, int w, int h, bool prevLight) {
    std::lock_guard<std::mutex> lock(g_desktopSnapMutex);
    if (!g_desktopSnap.valid || g_desktopSnap.pixels.empty() || w <= 0 || h <= 0) {
        return prevLight;
    }
    const int monX = g_desktopSnap.monX;
    const int monY = g_desktopSnap.monY;
    const int monW = g_desktopSnap.monW;
    const int monH = g_desktopSnap.monH;
    const DWORD* snapPx = g_desktopSnap.pixels.data();

    int relX = wndX - monX;
    int relY = wndY - monY;

    auto srgbToLinear = [](BYTE v) -> double {
        double c = v / 255.0;
        return (c <= 0.04045) ? (c / 12.92) : std::pow((c + 0.055) / 1.055, 2.4);
    };

    std::vector<double> samples;
    samples.reserve(256);

    int stepX = (std::max)(1, w / 16);
    int stepY = (std::max)(1, h / 16);

    for (int sy = relY + stepY / 2; sy < relY + h && sy < monH; sy += stepY) {
        if (sy < 0) continue;
        const DWORD* row = &snapPx[(size_t)sy * monW];
        for (int sx = relX + stepX / 2; sx < relX + w && sx < monW; sx += stepX) {
            if (sx < 0) continue;
            DWORD px = row[sx];
            double r = srgbToLinear((px >> 16) & 0xFF);
            double g = srgbToLinear((px >> 8) & 0xFF);
            double b = srgbToLinear(px & 0xFF);
            double lum = 0.2126 * r + 0.7152 * g + 0.0722 * b;
            samples.push_back(lum);
        }
    }

    if (samples.empty()) return prevLight;
    size_t mid = samples.size() / 2;
    std::nth_element(samples.begin(), samples.begin() + mid, samples.end());
    double medianLum = samples[mid];
    double avgLum = 0.0;
    for (double v : samples) avgLum += v;
    avgLum /= (double)samples.size();
    double effectiveLum = medianLum * 0.65 + avgLum * 0.35;

    return prevLight ? (effectiveLum > 0.36) : (effectiveLum > 0.44);
}

bool SampleCustomBgPixelsIsLight(const BYTE* bgPixels, UINT w, UINT h, bool validPrev, bool prevLight) {
    if (!bgPixels || w == 0 || h == 0) return prevLight;
    auto srgbToLinear = [](BYTE v) -> double {
        double c = v / 255.0;
        return (c <= 0.04045) ? (c / 12.92) : std::pow((c + 0.055) / 1.055, 2.4);
    };

    std::vector<double> samples;
    samples.reserve(256);
    UINT stepX = (std::max)(1u, w / 16);
    UINT stepY = (std::max)(1u, h / 16);
    for (UINT y = stepY / 2; y < h && samples.size() < 256; y += stepY) {
        const BYTE* row = bgPixels + (size_t)y * w * 4;
        for (UINT x = stepX / 2; x < w && samples.size() < 256; x += stepX) {
            const BYTE* px = row + (size_t)x * 4;
            BYTE b = px[0], g = px[1], r = px[2], a = px[3];
            if (a > 0 && a < 255) {
                r = (BYTE)(std::min)(255, (int)r * 255 / a);
                g = (BYTE)(std::min)(255, (int)g * 255 / a);
                b = (BYTE)(std::min)(255, (int)b * 255 / a);
            }
            double lum = 0.2126 * srgbToLinear(r) + 0.7152 * srgbToLinear(g) + 0.0722 * srgbToLinear(b);
            samples.push_back(lum);
        }
    }
    if (samples.empty()) return prevLight;
    size_t mid = samples.size() / 2;
    std::nth_element(samples.begin(), samples.begin() + mid, samples.end());
    double medianLum = samples[mid];
    double avgLum = 0.0;
    for (double v : samples) avgLum += v;
    avgLum /= (double)samples.size();
    double effectiveLum = medianLum * 0.65 + avgLum * 0.35;
    if (!validPrev) return effectiveLum > 0.42;
    return prevLight ? (effectiveLum > 0.36) : (effectiveLum > 0.46);
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

std::wstring ResolveEffectiveLanguageCode(std::wstring lang) {
    if (lang == L"auto" || lang.empty()) {
        LANGID lid = GetUserDefaultUILanguage();
        WORD prim = PRIMARYLANGID(lid);
        WORD sub  = SUBLANGID(lid);
        switch (prim) {
            case LANG_VIETNAMESE: return L"vi";
            case LANG_THAI:       return L"th";
            case LANG_INDONESIAN: return L"id";
            case LANG_MALAY:      return L"ms";
            case LANG_JAPANESE:   return L"ja";
            case LANG_CHINESE:
                return (sub == SUBLANG_CHINESE_SIMPLIFIED || sub == SUBLANG_CHINESE_SINGAPORE)
                       ? L"zh_cn" : L"zh_tw";
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
    return lang;
}

struct MenuLocalizedStrings {
    const wchar_t* surfaceThemeMenu;
    const wchar_t* liquidGlass;
    const wchar_t* windowsBlur;
    const wchar_t* opaqueSolid;
    const wchar_t* glassTintRange;
    const wchar_t* glassPresetM100;
    const wchar_t* glassPresetM80;
    const wchar_t* glassPresetM50;
    const wchar_t* glassPreset0;
    const wchar_t* glassPreset15;
    const wchar_t* glassPreset30;
    const wchar_t* glassPreset55;
    const wchar_t* glassPreset80;
    const wchar_t* blurOpacityRange;
    const wchar_t* colorModeMenu;
    const wchar_t* colorAuto;
    const wchar_t* colorDark;
    const wchar_t* colorLight;
    const wchar_t* uiSizeMenu;
    const wchar_t* scale85;
    const wchar_t* scale90;
    const wchar_t* scale100;
    const wchar_t* scale110;
    const wchar_t* scale120;
    const wchar_t* widthPx;
    const wchar_t* heightPx;
    const wchar_t* fontSizePt;
    const wchar_t* resetCustomSize;
    const wchar_t* minimalModeMenu;
    const wchar_t* customBgMenu;
    const wchar_t* selectBgFile;
    const wchar_t* clearCustomBg;
    const wchar_t* mediaOpacityRange;
    const wchar_t* bgOpacity90;
    const wchar_t* bgOpacity150;
    const wchar_t* bgOpacity215;
    const wchar_t* bgOpacity255;
    const wchar_t* historyMenu;
    const wchar_t* maxStoredRange;
    const wchar_t* visibleRowsRange;
    const wchar_t* maxHist10;
    const wchar_t* maxHist30;
    const wchar_t* maxHist35;
    const wchar_t* maxHist50;
    const wchar_t* maxHist0;
    const wchar_t* showTimestamp;
    const wchar_t* appSuggestions;
    const wchar_t* displayLanguageMenu;
    const wchar_t* autoLanguage;
    const wchar_t* clearAllHistory;
    const wchar_t* customSizeDlgTitle;
    const wchar_t* resetDefaultBtn;
    const wchar_t* doneOkBtn;
    const wchar_t* bgBrowseFilter;
    const wchar_t* bgBrowseTitle;
    const wchar_t* bgSizeLimitFmt;
    const wchar_t* ggSearchEmptyHint;
    const wchar_t* ggSearchQueryPrefix;
    const wchar_t* mseSearchEmptyHint;
    const wchar_t* mseSearchQueryPrefix;
};

MenuLocalizedStrings GetMenuLocalizedStrings(std::wstring lang) {
    lang = ResolveEffectiveLanguageCode(std::move(lang));

    if (lang == L"vi") {
        return {
            L"Giao diện bề mặt (Surface Theme)",
            L"Liquid Glass",
            L"Làm mờ Windows (Windows Blur)",
            L"Nền đặc (Opaque)",
            L"Độ đậm Liquid Glass (-100 – 100)",
            L"Kính: Trong suốt tối đa (-100)",
            L"Kính: Trong suốt cực cao (-80)",
            L"Kính: Siêu trong suốt (-50)",
            L"Kính: Trong suốt chuẩn (0)",
            L"Kính: Trong suốt cao – Clear (15)",
            L"Kính: Chuẩn iOS – Balanced (30)",
            L"Kính: Đậm vừa – Medium Tint (55)",
            L"Kính: Tương phản cao – Tinted (80)",
            L"Độ đậm Windows Blur (0 – 255)",
            L"Chế độ Sáng / Tối (Color Mode)",
            L"Tự động theo Windows (Auto)",
            L"Giao diện Tối (Dark Mode)",
            L"Giao diện Sáng (Light Mode)",
            L"Kích thước giao diện (UI Size)",
            L"85% – Nhỏ gọn (Compact)",
            L"90% – Hơi nhỏ",
            L"100% – Mặc định (Chuẩn)",
            L"110% – Lớn vừa (Medium)",
            L"120% – Rất lớn (Large)",
            L"Chiều rộng (Width px)",
            L"Chiều cao (Height px)",
            L"Cỡ chữ (Font Size pt)",
            L"Đặt lại theo tỷ lệ có sẵn (Reset)",
            L"Chế độ tối giản (Ẩn Duyệt & Thoát)",
            L"Hình nền tùy chỉnh (Ảnh / GIF / MP4)",
            L"Chọn tệp hình nền (Ảnh / GIF / MP4 ≤ 50MB)...",
            L"Gỡ bỏ hình nền tùy chỉnh",
            L"Độ rõ hình nền (20 – 255)",
            L"Độ rõ nền: Dịu nhẹ (90)",
            L"Độ rõ nền: Cân bằng (150)",
            L"Độ rõ nền: Rõ nét (215)",
            L"Độ rõ nền: Tối đa (255)",
            L"Lịch sử & Gợi ý (Tối đa 50 mục / Cuộn 5 dòng)",
            L"Số mục lưu tối đa (0 – 50)",
            L"Số dòng hiển thị (3 – 10)",
            L"Giới hạn lưu: 10 mục",
            L"Giới hạn lưu: 30 mục (Hiển thị 5 + Cuộn)",
            L"Giới hạn lưu: 35 mục (Mặc định)",
            L"Giới hạn lưu: 50 mục (Tối đa)",
            L"Tắt danh sách lịch sử (0)",
            L"Hiển thị thời gian thực thi",
            L"Gợi ý ứng dụng & Tìm kiếm ggsearch:/msesearch:",
            L"Ngôn ngữ hiển thị (Display Language)",
            L"Tự động theo hệ thống (Auto)",
            L"Xóa toàn bộ lịch sử đã lưu (Clear History)",
            L"Tùy chỉnh Kích thước & Cỡ chữ (Xem trực tiếp)",
            L"Mặc định (Reset)",
            L"Xong (OK)",
            L"Hình nền & Video (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Tất cả tệp (*.*)\0*.*\0\0",
            L"Chọn ảnh tĩnh, ảnh động GIF hoặc video MP4 làm hình nền (tối đa 20-50 MB)",
            L"Tệp hình nền vượt quá giới hạn dung lượng (%d MB). Vui lòng chọn tệp ảnh/GIF/MP4 nhỏ hơn %d MB.",
            L"Gõ từ khóa để tìm kiếm Google bằng Chrome",
            L"Tìm trên Google (Chrome): \"",
            L"Gõ từ khóa để tìm kiếm bằng Microsoft Edge",
            L"Tìm bằng Microsoft Edge: \""
        };
    }
    if (lang == L"th") {
        return {
            L"รูปแบบพื้นผิว (Surface Theme)",
            L"Liquid Glass",
            L"เบลอแบบ Windows (Windows Blur)",
            L"พื้นหลังทึบ (Opaque)",
            L"ความเข้ม Liquid Glass (-100 – 100)",
            L"กระจก: โปร่งใสสูงสุด (-100)",
            L"กระจก: โปร่งใสสูงพิเศษ (-80)",
            L"กระจก: โปร่งใสพิเศษ (-50)",
            L"กระจก: โปร่งใสมาตรฐาน (0)",
            L"กระจก: ใสคมชัด – Clear (15)",
            L"กระจก: สมดุลแบบ iOS – Balanced (30)",
            L"กระจก: เข้มปานกลาง – Medium Tint (55)",
            L"กระจก: คอนทราสต์สูง – Tinted (80)",
            L"ความทึบแสง Windows Blur (0 – 255)",
            L"โหมดสีสว่าง / มืด (Color Mode)",
            L"อัตโนมัติตาม Windows (Auto)",
            L"โหมดมืด (Dark Mode)",
            L"โหมดสว่าง (Light Mode)",
            L"ขนาดส่วนติดต่อผู้ใช้ (UI Size)",
            L"85% – กะทัดรัด (Compact)",
            L"90% – เล็กลงเล็กน้อย",
            L"100% – ค่าเริ่มต้น (มาตรฐาน)",
            L"110% – ขนาดกลาง (Medium)",
            L"120% – ขนาดใหญ่ (Large)",
            L"ความกว้าง (Width px)",
            L"ความสูงแถบ (Height px)",
            L"ขนาดตัวอักษร (Font Size pt)",
            L"รีเซ็ตขนาดกลับเป็นค่าที่ตั้งไว้",
            L"โหมดมินิมอล (ซ่อนปุ่มเรียกดู & ออก)",
            L"พื้นหลังกำหนดเอง (รูปภาพ / GIF / MP4)",
            L"เลือกไฟล์พื้นหลัง (รูปภาพ / GIF / MP4 ≤ 50MB)...",
            L"ลบพื้นหลังกำหนดเอง",
            L"ความชัดพื้นหลัง (20 – 255)",
            L"ความชัดพื้นหลัง: นุ่มนวล (90)",
            L"ความชัดพื้นหลัง: สมดุล (150)",
            L"ความชัดพื้นหลัง: คมชัด (215)",
            L"ความชัดพื้นหลัง: สูงสุด (255)",
            L"ประวัติ & คำแนะนำ (สูงสุด 50 / เลื่อน 5 แถว)",
            L"จำนวนประวัติสูงสุด (0 – 50)",
            L"จำนวนแถวที่แสดง (3 – 10)",
            L"จำกัดการเก็บ: 10 รายการ",
            L"จำกัดการเก็บ: 30 รายการ (แสดง 5 + เลื่อน)",
            L"จำกัดการเก็บ: 35 รายการ (ค่าเริ่มต้น)",
            L"จำกัดการเก็บ: 50 รายการ (สูงสุด)",
            L"ปิดรายการประวัติ (0)",
            L"แสดงเวลาที่เรียกใช้ล่าสุด",
            L"แนะนำแอป & ค้นหาด่วน ggsearch:/msesearch:",
            L"ภาษาที่แสดงผล (Display Language)",
            L"อัตโนมัติตามระบบ (Auto)",
            L"ล้างประวัติที่บันทึกไว้ทั้งหมด",
            L"กำหนดขนาดและตัวอักษรเอง (ดูตัวอย่างสด)",
            L"ค่าเริ่มต้น (Reset)",
            L"เสร็จสิ้น (OK)",
            L"สื่อพื้นหลัง (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0ไฟล์ทั้งหมด (*.*)\0*.*\0\0",
            L"เลือกรูปภาพ, GIF เคลื่อนไหว หรือวิดีโอ MP4 เป็นพื้นหลัง (สูงสุด 20-50 MB)",
            L"ไฟล์ที่เลือกมีขนาดเกินขีดจำกัด (%d MB) กรุณาเลือกไฟล์ขนาดต่ำกว่า %d MB",
            L"พิมพ์คำค้นหาเพื่อค้นหา Google ใน Chrome",
            L"ค้นหา Google (Chrome): \"",
            L"พิมพ์คำค้นหาเพื่อค้นหาใน Microsoft Edge",
            L"ค้นหาใน Microsoft Edge: \""
        };
    }
    if (lang == L"id") {
        return {
            L"Tema Permukaan (Surface Theme)",
            L"Liquid Glass",
            L"Windows Blur",
            L"Buram Padat (Opaque)",
            L"Kepekatan Liquid Glass (-100 – 100)",
            L"Kaca: Transparan Maksimal (-100)",
            L"Kaca: Ekstra Transparan (-80)",
            L"Kaca: Ultra Transparan (-50)",
            L"Kaca: Bening Standar (0)",
            L"Kaca: Bening – Clear (15)",
            L"Kaca: Seimbang iOS – Balanced (30)",
            L"Kaca: Sedang – Medium Tint (55)",
            L"Kaca: Kontras Tinggi – Tinted (80)",
            L"Opasitas Windows Blur (0 – 255)",
            L"Mode Warna (Terang / Gelap)",
            L"Otomatis Ikuti Windows (Auto)",
            L"Mode Gelap (Dark Mode)",
            L"Mode Terang (Light Mode)",
            L"Ukuran Tampilan (UI Size)",
            L"85% – Ringkas (Compact)",
            L"90% – Sedikit Lebih Kecil",
            L"100% – Default (Standar)",
            L"110% – Sedang (Medium)",
            L"120% – Besar (Large)",
            L"Lebar (Width px)",
            L"Tinggi Bar (Height px)",
            L"Ukuran Font (Font Size pt)",
            L"Atur Ulang Ukuran ke Preset",
            L"Mode Minimalis (Sembunyikan Telusuri & Keluar)",
            L"Latar Belakang Kustom (Gambar / GIF / MP4)",
            L"Pilih File Latar Belakang (Gambar / GIF / MP4 ≤ 50MB)...",
            L"Hapus Latar Belakang Kustom",
            L"Opasitas Media (20 – 255)",
            L"Opasitas Media: Halus (90)",
            L"Opasitas Media: Seimbang (150)",
            L"Opasitas Media: Jelas (215)",
            L"Opasitas Media: Penuh (255)",
            L"Riwayat & Saran (Maks 50 / Gulir 5 Baris)",
            L"Maks Item Tersimpan (0 – 50)",
            L"Baris Terlihat (3 – 10)",
            L"Batas Simpan: 10 Item",
            L"Batas Simpan: 30 Item (Tampil 5 + Gulir)",
            L"Batas Simpan: 35 Item (Default)",
            L"Batas Simpan: 50 Item (Maksimum)",
            L"Nonaktifkan Daftar Riwayat (0)",
            L"Tampilkan Waktu Eksekusi",
            L"Saran Aplikasi & ggsearch:/msesearch:",
            L"Bahasa Tampilan (Display Language)",
            L"Otomatis – Default Sistem",
            L"Hapus Semua Riwayat Tersimpan",
            L"Ukuran & Font Kustom (Pratinjau Langsung)",
            L"Reset Default",
            L"Selesai (OK)",
            L"Media Latar (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Semua File (*.*)\0*.*\0\0",
            L"Pilih gambar, GIF animasi, atau video MP4 sebagai latar (maks 20-50 MB)",
            L"File latar melebihi batas ukuran (%d MB). Pilih file di bawah %d MB.",
            L"Ketik kata kunci untuk mencari di Google (Chrome)",
            L"Cari di Google (Chrome): \"",
            L"Ketik kata kunci untuk mencari di Microsoft Edge",
            L"Cari di Microsoft Edge: \""
        };
    }
    if (lang == L"ms") {
        return {
            L"Tema Permukaan (Surface Theme)",
            L"Liquid Glass",
            L"Kabur Windows (Windows Blur)",
            L"Legap (Opaque)",
            L"Kepekatan Liquid Glass (-100 – 100)",
            L"Kaca: Lutsinar Maksimum (-100)",
            L"Kaca: Lutsinar Ekstrem (-80)",
            L"Kaca: Ultra Lutsinar (-50)",
            L"Kaca: Jernih Lalai (0)",
            L"Kaca: Jernih – Clear (15)",
            L"Kaca: Seimbang iOS – Balanced (30)",
            L"Kaca: Sederhana – Medium Tint (55)",
            L"Kaca: Kontras Tinggi – Tinted (80)",
            L"Kelegapan Windows Blur (0 – 255)",
            L"Mod Warna (Cerah / Gelap)",
            L"Automatik Ikut Windows (Auto)",
            L"Mod Gelap (Dark Mode)",
            L"Mod Cerah (Light Mode)",
            L"Saiz Antara Muka (UI Size)",
            L"85% – Padat (Compact)",
            L"90% – Sedikit Kecil",
            L"100% – Lalai (Standard)",
            L"110% – Sederhana Besar",
            L"120% – Besar (Large)",
            L"Lebar (Width px)",
            L"Tinggi Bar (Height px)",
            L"Saiz Fon (Font Size pt)",
            L"Tetapkan Semula Saiz ke Pratetap",
            L"Mod Minimal (Sembunyikan Semak & Keluar)",
            L"Latar Belakang Tersuai (Imej / GIF / MP4)",
            L"Pilih Fail Latar Belakang (Imej / GIF / MP4 ≤ 50MB)...",
            L"Buang Latar Belakang Tersuai",
            L"Kelegapan Media (20 – 255)",
            L"Kelegapan Media: Halus (90)",
            L"Kelegapan Media: Seimbang (150)",
            L"Kelegapan Media: Jelas (215)",
            L"Kelegapan Media: Penuh (255)",
            L"Sejarah & Cadangan (Sehingga 50 / Tatal 5)",
            L"Item Disimpan Maksimum (0 – 50)",
            L"Baris Kelihatan (3 – 10)",
            L"Had Simpanan: 10 Item",
            L"Had Simpanan: 30 Item (Papar 5 + Tatal)",
            L"Had Simpanan: 35 Item (Lalai)",
            L"Had Simpanan: 50 Item (Maksimum)",
            L"Lumpuhkan Senarai Sejarah (0)",
            L"Tunjukkan Masa Pelaksanaan",
            L"Cadangan Aplikasi & ggsearch:/msesearch:",
            L"Bahasa Paparan (Display Language)",
            L"Automatik – Lalai Sistem",
            L"Padam Semua Sejarah Disimpan",
            L"Saiz & Fon Tersuai (Pratonton Langsung)",
            L"Tetap Semula",
            L"Selesai (OK)",
            L"Media Latar (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Semua Fail (*.*)\0*.*\0\0",
            L"Pilih imej, GIF animasi atau video MP4 sebagai latar belakang (maks 20-50 MB)",
            L"Fail melebihi had saiz (%d MB). Sila pilih fail di bawah %d MB.",
            L"Taip kata kunci untuk mencari di Google (Chrome)",
            L"Cari di Google (Chrome): \"",
            L"Taip kata kunci untuk mencari di Microsoft Edge",
            L"Cari di Microsoft Edge: \""
        };
    }
    if (lang == L"ja") {
        return {
            L"サーフェステーマ (Surface Theme)",
            L"Liquid Glass",
            L"Windows ブラー (Windows Blur)",
            L"不透明 (Opaque)",
            L"Liquid Glass の濃度 (-100 – 100)",
            L"ガラス: 最大透明 (-100)",
            L"ガラス: 超高透明 (-80)",
            L"ガラス: ウルトラ透明 (-50)",
            L"ガラス: 標準クリア (0)",
            L"ガラス: クリア – Clear (15)",
            L"ガラス: iOS バランス – Balanced (30)",
            L"ガラス: ミディアム – Medium Tint (55)",
            L"ガラス: 高コントラスト – Tinted (80)",
            L"Windows ブラーの不透明度 (0 – 255)",
            L"カラーモード (ライト / ダーク)",
            L"自動 – Windows の設定に従う",
            L"ダークモード (Dark Mode)",
            L"ライトモード (Light Mode)",
            L"UI サイズ / スケール",
            L"85% – コンパクト (Compact)",
            L"90% – やや小さめ",
            L"100% – デフォルト (標準)",
            L"110% – やや大きめ (Medium)",
            L"120% – 大 (Large)",
            L"幅 (Width px)",
            L"バーの高さ (Height px)",
            L"フォントサイズ (Font Size pt)",
            L"カスタムサイズをプリセットに戻す",
            L"ミニマルモード (参照・閉じるを隠す)",
            L"カスタム背景 (画像 / GIF / MP4)",
            L"背景ファイルを選択 (画像 / GIF / MP4 ≤ 50MB)...",
            L"カスタム背景をクリア",
            L"背景メディアの不透明度 (20 – 255)",
            L"背景の不透明度: 控えめ (90)",
            L"背景の不透明度: バランス (150)",
            L"背景の不透明度: 鮮明 (215)",
            L"背景の不透明度: 最大 (255)",
            L"履歴と候補 (最大50件 / 5行スクロール)",
            L"最大保存件数 (0 – 50)",
            L"表示行数 (3 – 10)",
            L"最大保存件数: 10件",
            L"最大保存件数: 30件 (5行表示 + スクロール)",
            L"最大保存件数: 35件 (標準)",
            L"最大保存件数: 50件 (最大)",
            L"履歴リストを無効化 (0)",
            L"実行日時を表示",
            L"アプリ候補と検索 ggsearch:/msesearch:",
            L"表示言語 (Display Language)",
            L"自動 – システム既定",
            L"保存された履歴をすべて消去",
            L"カスタムサイズとフォント (ライブプレビュー)",
            L"リセット (Reset)",
            L"完了 (OK)",
            L"背景メディア (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0すべてのファイル (*.*)\0*.*\0\0",
            L"背景用の画像、GIFアニメーション、またはMP4動画を選択 (最大 20-50 MB)",
            L"選択したファイルは上限 (%d MB) を超えています。%d MB 以下のファイルを選択してください。",
            L"キーワードを入力して Chrome で Google 検索",
            L"Google 検索 (Chrome): \"",
            L"キーワードを入力して Microsoft Edge で検索",
            L"Microsoft Edge で検索: \""
        };
    }
    if (lang == L"zh_cn") {
        return {
            L"表面材质主题 (Surface Theme)",
            L"液态玻璃 (Liquid Glass)",
            L"Windows 模糊 (Windows Blur)",
            L"纯色不透明 (Opaque)",
            L"液态玻璃浓度 (-100 – 100)",
            L"玻璃: 极致透明 (-100)",
            L"玻璃: 超高透明 (-80)",
            L"玻璃: 超透明 (-50)",
            L"玻璃: 标准清透 (0)",
            L"玻璃: 清透 – Clear (15)",
            L"玻璃: iOS 均衡 – Balanced (30)",
            L"玻璃: 适中色调 – Medium Tint (55)",
            L"玻璃: 高对比度 – Tinted (80)",
            L"Windows 模糊不透明度 (0 – 255)",
            L"深浅色模式 (Color Mode)",
            L"自动跟随 Windows (Auto)",
            L"深色模式 (Dark Mode)",
            L"浅色模式 (Light Mode)",
            L"界面尺寸与缩放 (UI Size)",
            L"85% – 紧凑 (Compact)",
            L"90% – 稍小",
            L"100% – 默认 (标准)",
            L"110% – 中等偏大 (Medium)",
            L"120% – 大号 (Large)",
            L"窗口宽度 (Width px)",
            L"输入栏高度 (Height px)",
            L"字体大小 (Font Size pt)",
            L"重置为预设比例 (Reset)",
            L"极简模式 (隐藏浏览与退出按钮)",
            L"自定义背景 (图片 / GIF / MP4)",
            L"选择背景文件 (图片 / GIF / MP4 ≤ 50MB)...",
            L"清除自定义背景",
            L"背景媒体清晰度 (20 – 255)",
            L"背景清晰度: 柔和 (90)",
            L"背景清晰度: 均衡 (150)",
            L"背景清晰度: 鲜明 (215)",
            L"背景清晰度: 完全显示 (255)",
            L"历史与建议 (最多 50 项 / 滚动 5 行)",
            L"最大保存数量 (0 – 50)",
            L"可见行数 (3 – 10)",
            L"保存上限: 10 项",
            L"保存上限: 30 项 (显示 5 行 + 滚动)",
            L"保存上限: 35 项 (默认)",
            L"保存上限: 50 项 (最大)",
            L"禁用历史记录列表 (0)",
            L"显示最近执行时间",
            L"应用建议与网页搜索 ggsearch:/msesearch:",
            L"显示语言 (Display Language)",
            L"自动 – 系统默认",
            L"清除所有已保存的历史记录",
            L"自定义尺寸与字体 (实时预览)",
            L"恢复默认 (Reset)",
            L"完成 (OK)",
            L"背景媒体 (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0所有文件 (*.*)\0*.*\0\0",
            L"选择静态图片、动态 GIF 或 MP4 视频作为背景 (最大 20-50 MB)",
            L"所选背景文件超过大小限制 (%d MB)，请选择小于 %d MB 的文件。",
            L"输入关键词使用 Chrome 进行 Google 搜索",
            L"Google 搜索 (Chrome): \"",
            L"输入关键词使用 Microsoft Edge 搜索",
            L"Microsoft Edge 搜索: \""
        };
    }
    if (lang == L"zh_tw") {
        return {
            L"表面材質主題 (Surface Theme)",
            L"液態玻璃 (Liquid Glass)",
            L"Windows 模糊 (Windows Blur)",
            L"純色不透明 (Opaque)",
            L"液態玻璃濃度 (-100 – 100)",
            L"玻璃: 極致透明 (-100)",
            L"玻璃: 超高透明 (-80)",
            L"玻璃: 超透明 (-50)",
            L"玻璃: 標準清透 (0)",
            L"玻璃: 清透 – Clear (15)",
            L"玻璃: iOS 均衡 – Balanced (30)",
            L"玻璃: 適中色調 – Medium Tint (55)",
            L"玻璃: 高對比度 – Tinted (80)",
            L"Windows 模糊不透明度 (0 – 255)",
            L"深淺色模式 (Color Mode)",
            L"自動跟隨 Windows (Auto)",
            L"深色模式 (Dark Mode)",
            L"淺色模式 (Light Mode)",
            L"介面尺寸與縮放 (UI Size)",
            L"85% – 緊湊 (Compact)",
            L"90% – 稍小",
            L"100% – 預設 (標準)",
            L"110% – 中等偏大 (Medium)",
            L"120% – 大型 (Large)",
            L"視窗寬度 (Width px)",
            L"輸入列高度 (Height px)",
            L"字型大小 (Font Size pt)",
            L"重設為預設比例 (Reset)",
            L"極簡模式 (隱藏瀏覽與退出按鈕)",
            L"自訂背景 (圖片 / GIF / MP4)",
            L"選擇背景檔案 (圖片 / GIF / MP4 ≤ 50MB)...",
            L"清除自訂背景",
            L"背景媒體清晰度 (20 – 255)",
            L"背景清晰度: 柔和 (90)",
            L"背景清晰度: 均衡 (150)",
            L"背景清晰度: 鮮明 (215)",
            L"背景清晰度: 完整顯示 (255)",
            L"歷史與建議 (最多 50 項 / 捲動 5 行)",
            L"最大儲存數量 (0 – 50)",
            L"可見行數 (3 – 10)",
            L"儲存上限: 10 項",
            L"儲存上限: 30 項 (顯示 5 行 + 捲動)",
            L"儲存上限: 35 項 (預設)",
            L"儲存上限: 50 項 (最大)",
            L"停用歷史記錄清單 (0)",
            L"顯示最近執行時間",
            L"應用程式建議與搜尋 ggsearch:/msesearch:",
            L"顯示語言 (Display Language)",
            L"自動 – 系統預設",
            L"清除所有已儲存的歷史記錄",
            L"自訂尺寸與字型 (即時預覽)",
            L"恢復預設 (Reset)",
            L"完成 (OK)",
            L"背景媒體 (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0所有檔案 (*.*)\0*.*\0\0",
            L"選擇靜態圖片、動態 GIF 或 MP4 影片作為背景 (最大 20-50 MB)",
            L"所選背景檔案超過大小限制 (%d MB)，請選擇小於 %d MB 的檔案。",
            L"輸入關鍵字使用 Chrome 進行 Google 搜尋",
            L"Google 搜尋 (Chrome): \"",
            L"輸入關鍵字使用 Microsoft Edge 搜尋",
            L"Microsoft Edge 搜尋: \""
        };
    }
    if (lang == L"ko") {
        return {
            L"표면 테마 (Surface Theme)",
            L"Liquid Glass",
            L"Windows 블러 (Windows Blur)",
            L"불투명 (Opaque)",
            L"Liquid Glass 농도 (-100 – 100)",
            L"유리: 최대 투명 (-100)",
            L"유리: 초고투명 (-80)",
            L"유리: 울트라 투명 (-50)",
            L"유리: 기본 클리어 (0)",
            L"유리: 선명함 – Clear (15)",
            L"유리: iOS 밸런스 – Balanced (30)",
            L"유리: 중간 틴트 – Medium Tint (55)",
            L"유리: 고대비 틴트 – Tinted (80)",
            L"Windows 블러 불투명도 (0 – 255)",
            L"색상 모드 (라이트 / 다크)",
            L"자동 – Windows 설정 따름",
            L"다크 모드 (Dark Mode)",
            L"라이트 모드 (Light Mode)",
            L"UI 크기 및 배율 (UI Size)",
            L"85% – 작게 (Compact)",
            L"90% – 약간 작게",
            L"100% – 기본값 (표준)",
            L"110% – 중간 크게 (Medium)",
            L"120% – 크게 (Large)",
            L"창 너비 (Width px)",
            L"입력줄 높이 (Height px)",
            L"글꼴 크기 (Font Size pt)",
            L"사용자 지정 크기를 프리셋으로 초기화",
            L"미니멀 모드 (찾아보기 및 닫기 숨기기)",
            L"사용자 지정 배경 (이미지 / GIF / MP4)",
            L"배경 파일 선택 (이미지 / GIF / MP4 ≤ 50MB)...",
            L"사용자 지정 배경 지우기",
            L"배경 미디어 불투명도 (20 – 255)",
            L"배경 불투명도: 은은함 (90)",
            L"배경 불투명도: 균형 (150)",
            L"배경 불투명도: 선명함 (215)",
            L"배경 불투명도: 최대 (255)",
            L"기록 및 추천 (최대 50개 / 5줄 스크롤)",
            L"최대 저장 개수 (0 – 50)",
            L"표시할 줄 수 (3 – 10)",
            L"최대 저장: 10개",
            L"최대 저장: 30개 (5줄 표시 + 스크롤)",
            L"최대 저장: 35개 (기본값)",
            L"최대 저장: 50개 (최대)",
            L"기록 목록 사용 안 함 (0)",
            L"최근 실행 시간 표시",
            L"앱 추천 및 웹 검색 ggsearch:/msesearch:",
            L"표시 언어 (Display Language)",
            L"자동 – 시스템 기본값",
            L"저장된 모든 기록 지우기",
            L"사용자 지정 크기 및 글꼴 (실시간 미리보기)",
            L"초기화 (Reset)",
            L"완료 (OK)",
            L"배경 미디어 (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0모든 파일 (*.*)\0*.*\0\0",
            L"배경으로 사용할 이미지, 애니메이션 GIF 또는 MP4 비디오 선택 (최대 20-50 MB)",
            L"선택한 파일이 크기 제한(%d MB)을 초과합니다. %d MB 미만의 파일을 선택하세요.",
            L"Chrome에서 Google 검색할 키워드 입력",
            L"Google 검색 (Chrome): \"",
            L"Microsoft Edge에서 검색할 키워드 입력",
            L"Microsoft Edge 검색: \""
        };
    }
    if (lang == L"fr") {
        return {
            L"Thème de surface (Surface Theme)",
            L"Liquid Glass",
            L"Flou Windows (Windows Blur)",
            L"Opaque (Sans flou)",
            L"Teinte Liquid Glass (-100 – 100)",
            L"Verre : Transparence maximale (-100)",
            L"Verre : Transparence extrême (-80)",
            L"Verre : Ultra transparent (-50)",
            L"Verre : Clair par défaut (0)",
            L"Verre : Clair – Clear (15)",
            L"Verre : Équilibré iOS – Balanced (30)",
            L"Verre : Teinte moyenne – Medium (55)",
            L"Verre : Contraste élevé – Tinted (80)",
            L"Opacité du flou Windows (0 – 255)",
            L"Mode de couleur (Clair / Sombre)",
            L"Auto – Suivre Windows",
            L"Mode sombre (Dark Mode)",
            L"Mode clair (Light Mode)",
            L"Taille de l'interface (UI Size)",
            L"85% – Compact",
            L"90% – Légèrement réduit",
            L"100% – Par défaut (Standard)",
            L"110% – Moyen grand",
            L"120% – Grand (Large)",
            L"Largeur (Width px)",
            L"Hauteur de barre (Height px)",
            L"Taille de police (Font Size pt)",
            L"Réinitialiser la taille au préréglage",
            L"Mode minimal (Masquer Parcourir et Quitter)",
            L"Arrière-plan personnalisé (Image / GIF / MP4)",
            L"Choisir un fichier de fond (Image / GIF / MP4 ≤ 50 Mo)...",
            L"Supprimer l'arrière-plan personnalisé",
            L"Opacité du média (20 – 255)",
            L"Opacité : Subtile (90)",
            L"Opacité : Équilibrée (150)",
            L"Opacité : Vive (215)",
            L"Opacité : Maximale (255)",
            L"Historique et suggestions (Max 50 / 5 lignes)",
            L"Éléments max enregistrés (0 – 50)",
            L"Lignes visibles (3 – 10)",
            L"Max enregistré : 10 éléments",
            L"Max enregistré : 30 éléments (5 + défilement)",
            L"Max enregistré : 35 éléments (Par défaut)",
            L"Max enregistré : 50 éléments (Maximum)",
            L"Désactiver l'historique (0)",
            L"Afficher l'heure d'exécution",
            L"Suggestions d'applis et ggsearch:/msesearch:",
            L"Langue d'affichage (Display Language)",
            L"Auto – Langue du système",
            L"Effacer tout l'historique enregistré",
            L"Taille et police personnalisées (Aperçu)",
            L"Réinitialiser",
            L"Terminé (OK)",
            L"Média de fond (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Tous les fichiers (*.*)\0*.*\0\0",
            L"Sélectionner une image, un GIF animé ou une vidéo MP4 (max 20-50 Mo)",
            L"Le fichier dépasse la limite autorisée (%d Mo). Veuillez choisir un fichier de moins de %d Mo.",
            L"Tapez un mot-clé pour rechercher sur Google (Chrome)",
            L"Recherche Google (Chrome) : \"",
            L"Tapez un mot-clé pour rechercher dans Microsoft Edge",
            L"Recherche Microsoft Edge : \""
        };
    }
    if (lang == L"de") {
        return {
            L"Oberflächendesign (Surface Theme)",
            L"Liquid Glass",
            L"Windows-Unschärfe (Windows Blur)",
            L"Undurchsichtig (Opaque)",
            L"Liquid Glass Tönung (-100 – 100)",
            L"Glas: Maximale Transparenz (-100)",
            L"Glas: Extreme Transparenz (-80)",
            L"Glas: Ultratransparent (-50)",
            L"Glas: Standard Klar (0)",
            L"Glas: Klar – Clear (15)",
            L"Glas: iOS Ausgewogen – Balanced (30)",
            L"Glas: Mittlere Tönung – Medium (55)",
            L"Glas: Hoher Kontrast – Tinted (80)",
            L"Windows-Blur Deckkraft (0 – 255)",
            L"Farbmodus (Hell / Dunkel)",
            L"Automatisch – Windows folgen",
            L"Dunkler Modus (Dark Mode)",
            L"Heller Modus (Light Mode)",
            L"UI-Größe / Skalierung",
            L"85% – Kompakt",
            L"90% – Etwas kleiner",
            L"100% – Standard",
            L"110% – Mittelgroß",
            L"120% – Groß (Large)",
            L"Breite (Width px)",
            L"Leistenhöhe (Height px)",
            L"Schriftgröße (Font Size pt)",
            L"Größe auf Voreinstellung zurücksetzen",
            L"Minimalmodus (Durchsuchen & Schließen ausblenden)",
            L"Eigener Hintergrund (Bild / GIF / MP4)",
            L"Hintergrunddatei wählen (Bild / GIF / MP4 ≤ 50MB)...",
            L"Eigenen Hintergrund entfernen",
            L"Medien-Deckkraft (20 – 255)",
            L"Deckkraft: Dezent (90)",
            L"Deckkraft: Ausgewogen (150)",
            L"Deckkraft: Kräftig (215)",
            L"Deckkraft: Voll (255)",
            L"Verlauf & Vorschläge (Bis 50 / 5 Zeilen)",
            L"Max. gespeicherte Einträge (0 – 50)",
            L"Sichtbare Zeilen (3 – 10)",
            L"Max. Einträge: 10",
            L"Max. Einträge: 30 (5 zeigen + Scrollen)",
            L"Max. Einträge: 35 (Standard)",
            L"Max. Einträge: 50 (Maximum)",
            L"Verlaufsliste deaktivieren (0)",
            L"Ausführungszeitpunkt anzeigen",
            L"App-Vorschläge & ggsearch:/msesearch:",
            L"Anzeigesprache (Display Language)",
            L"Automatisch – Systemstandard",
            L"Gesamten Verlauf löschen",
            L"Benutzerdefinierte Größe & Schrift (Live-Vorschau)",
            L"Zurücksetzen",
            L"Fertig (OK)",
            L"Hintergrundmedien (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Alle Dateien (*.*)\0*.*\0\0",
            L"Bild, animiertes GIF oder MP4-Video als Hintergrund wählen (max. 20-50 MB)",
            L"Die ausgewählte Datei überschreitet das Limit (%d MB). Bitte wählen Sie eine Datei unter %d MB.",
            L"Suchbegriff für Google-Suche in Chrome eingeben",
            L"Google-Suche (Chrome): \"",
            L"Suchbegriff für Suche in Microsoft Edge eingeben",
            L"Microsoft Edge-Suche: \""
        };
    }
    if (lang == L"es") {
        return {
            L"Tema de superficie (Surface Theme)",
            L"Liquid Glass",
            L"Desenfoque de Windows (Windows Blur)",
            L"Opaco (Sin desenfoque)",
            L"Tinte de Liquid Glass (-100 – 100)",
            L"Cristal: Transparencia máxima (-100)",
            L"Cristal: Transparencia extrema (-80)",
            L"Cristal: Ultra transparente (-50)",
            L"Cristal: Claro predeterminado (0)",
            L"Cristal: Claro – Clear (15)",
            L"Cristal: Equilibrado iOS – Balanced (30)",
            L"Cristal: Tinte medio – Medium (55)",
            L"Cristal: Alto contraste – Tinted (80)",
            L"Opacidad de Windows Blur (0 – 255)",
            L"Modo de color (Claro / Oscuro)",
            L"Automático – Seguir a Windows",
            L"Modo oscuro (Dark Mode)",
            L"Modo claro (Light Mode)",
            L"Tamaño de interfaz (UI Size)",
            L"85% – Compacto",
            L"90% – Un poco más pequeño",
            L"100% – Predeterminado (Estándar)",
            L"110% – Mediano grande",
            L"120% – Grande (Large)",
            L"Ancho (Width px)",
            L"Altura de barra (Height px)",
            L"Tamaño de fuente (Font Size pt)",
            L"Restablecer tamaño al ajuste predefinido",
            L"Modo mínimo (Ocultar Examinar y Salir)",
            L"Fondo personalizado (Imagen / GIF / MP4)",
            L"Seleccionar archivo de fondo (Imagen / GIF / MP4 ≤ 50MB)...",
            L"Quitar fondo personalizado",
            L"Opacidad del fondo (20 – 255)",
            L"Opacidad: Sutil (90)",
            L"Opacidad: Equilibrada (150)",
            L"Opacidad: Nítida (215)",
            L"Opacidad: Máxima (255)",
            L"Historial y sugerencias (Máx. 50 / 5 filas)",
            L"Máx. elementos guardados (0 – 50)",
            L"Filas visibles (3 – 10)",
            L"Máximo guardado: 10 elementos",
            L"Máximo guardado: 30 elementos (5 + desplazar)",
            L"Máximo guardado: 35 elementos (Predeterminado)",
            L"Máximo guardado: 50 elementos (Máximo)",
            L"Desactivar historial (0)",
            L"Mostrar hora de ejecución",
            L"Sugerencias de apps y ggsearch:/msesearch:",
            L"Idioma de visualización (Display Language)",
            L"Automático – Predeterminado del sistema",
            L"Borrar todo el historial guardado",
            L"Tamaño y fuente personalizados (Vista previa)",
            L"Restablecer",
            L"Listo (OK)",
            L"Medios de fondo (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Todos los archivos (*.*)\0*.*\0\0",
            L"Seleccione imagen, GIF animado o video MP4 de fondo (máx. 20-50 MB)",
            L"El archivo supera el límite permitido (%d MB). Elija un archivo menor de %d MB.",
            L"Escriba una palabra clave para buscar en Google (Chrome)",
            L"Buscar en Google (Chrome): \"",
            L"Escriba una palabra clave para buscar en Microsoft Edge",
            L"Buscar en Microsoft Edge: \""
        };
    }
    if (lang == L"pt") {
        return {
            L"Tema da superfície (Surface Theme)",
            L"Liquid Glass",
            L"Desfoque do Windows (Windows Blur)",
            L"Opaco (Sem desfoque)",
            L"Tom do Liquid Glass (-100 – 100)",
            L"Vidro: Transparência máxima (-100)",
            L"Vidro: Transparência extrema (-80)",
            L"Vidro: Ultra transparente (-50)",
            L"Vidro: Transparente padrão (0)",
            L"Vidro: Claro – Clear (15)",
            L"Vidro: Equilibrado iOS – Balanced (30)",
            L"Vidro: Tom médio – Medium (55)",
            L"Vidro: Alto contraste – Tinted (80)",
            L"Opacidade do Windows Blur (0 – 255)",
            L"Modo de cor (Claro / Escuro)",
            L"Automático – Seguir o Windows",
            L"Modo escuro (Dark Mode)",
            L"Modo claro (Light Mode)",
            L"Tamanho da interface (UI Size)",
            L"85% – Compacto",
            L"90% – Um pouco menor",
            L"100% – Padrão (Standard)",
            L"110% – Médio grande",
            L"120% – Grande (Large)",
            L"Largura (Width px)",
            L"Altura da barra (Height px)",
            L"Tamanho da fonte (Font Size pt)",
            L"Redefinir tamanho para a predefinição",
            L"Modo mínimo (Ocultar Procurar e Sair)",
            L"Plano de fundo personalizado (Imagem / GIF / MP4)",
            L"Selecionar arquivo de fundo (Imagem / GIF / MP4 ≤ 50MB)...",
            L"Remover plano de fundo personalizado",
            L"Opacidade da mídia (20 – 255)",
            L"Opacidade: Sutil (90)",
            L"Opacidade: Equilibrada (150)",
            L"Opacidade: Nítida (215)",
            L"Opacidade: Total (255)",
            L"Histórico e sugestões (Até 50 / Rolar 5)",
            L"Máx. de itens salvos (0 – 50)",
            L"Linhas visíveis (3 – 10)",
            L"Máximo salvo: 10 itens",
            L"Máximo salvo: 30 itens (Mostrar 5 + Rolar)",
            L"Máximo salvo: 35 itens (Padrão)",
            L"Máximo salvo: 50 itens (Máximo)",
            L"Desativar lista de histórico (0)",
            L"Mostrar horário de execução",
            L"Sugestões de apps e ggsearch:/msesearch:",
            L"Idioma de exibição (Display Language)",
            L"Automático – Padrão do sistema",
            L"Limpar todo o histórico salvo",
            L"Tamanho e fonte personalizados (Visualização)",
            L"Redefinir",
            L"Concluído (OK)",
            L"Mídia de fundo (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Todos os arquivos (*.*)\0*.*\0\0",
            L"Selecione imagem, GIF animado ou vídeo MP4 de fundo (máx. 20-50 MB)",
            L"O arquivo selecionado excede o limite (%d MB). Escolha um arquivo abaixo de %d MB.",
            L"Digite uma palavra-chave para pesquisar no Google (Chrome)",
            L"Pesquisar no Google (Chrome): \"",
            L"Digite uma palavra-chave para pesquisar no Microsoft Edge",
            L"Pesquisar no Microsoft Edge: \""
        };
    }
    if (lang == L"it") {
        return {
            L"Tema superficie (Surface Theme)",
            L"Liquid Glass",
            L"Sfocatura Windows (Windows Blur)",
            L"Opaco (Senza sfocatura)",
            L"Tinta Liquid Glass (-100 – 100)",
            L"Vetro: Trasparenza massima (-100)",
            L"Vetro: Trasparenza estrema (-80)",
            L"Vetro: Ultra trasparente (-50)",
            L"Vetro: Trasparente predefinito (0)",
            L"Vetro: Chiaro – Clear (15)",
            L"Vetro: Bilanciato iOS – Balanced (30)",
            L"Vetro: Tinta media – Medium (55)",
            L"Vetro: Alto contrasto – Tinted (80)",
            L"Opacità Windows Blur (0 – 255)",
            L"Modalità colore (Chiaro / Scuro)",
            L"Automatico – Segui Windows",
            L"Modalità scura (Dark Mode)",
            L"Modalità chiara (Light Mode)",
            L"Dimensioni interfaccia (UI Size)",
            L"85% – Compatto",
            L"90% – Leggermente più piccolo",
            L"100% – Predefinito (Standard)",
            L"110% – Medio grande",
            L"120% – Grande (Large)",
            L"Larghezza (Width px)",
            L"Altezza barra (Height px)",
            L"Dimensione carattere (Font Size pt)",
            L"Ripristina dimensioni predefinite",
            L"Modalità minimale (Nascondi Sfoglia ed Esci)",
            L"Sfondo personalizzato (Immagine / GIF / MP4)",
            L"Seleziona file di sfondo (Immagine / GIF / MP4 ≤ 50MB)...",
            L"Rimuovi sfondo personalizzato",
            L"Opacità sfondo (20 – 255)",
            L"Opacità: Leggera (90)",
            L"Opacità: Bilanciata (150)",
            L"Opacità: Vivida (215)",
            L"Opacità: Massima (255)",
            L"Cronologia e suggerimenti (Max 50 / Scorri 5)",
            L"Elementi salvati max (0 – 50)",
            L"Righe visibili (3 – 10)",
            L"Max salvati: 10 elementi",
            L"Max salvati: 30 elementi (Mostra 5 + Scorri)",
            L"Max salvati: 35 elementi (Predefinito)",
            L"Max salvati: 50 elementi (Massimo)",
            L"Disattiva cronologia (0)",
            L"Mostra orario di esecuzione",
            L"Suggerimenti app e ggsearch:/msesearch:",
            L"Lingua di visualizzazione (Display Language)",
            L"Automatico – Predefinita di sistema",
            L"Cancella tutta la cronologia salvata",
            L"Dimensioni e carattere personalizzati (Anteprima)",
            L"Ripristina",
            L"Fatto (OK)",
            L"Media di sfondo (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Tutti i file (*.*)\0*.*\0\0",
            L"Seleziona un'immagine, GIF animata o video MP4 come sfondo (max 20-50 MB)",
            L"Il file supera il limite consentito (%d MB). Scegli un file inferiore a %d MB.",
            L"Digita una parola chiave per cercare su Google (Chrome)",
            L"Cerca su Google (Chrome): \"",
            L"Digita una parola chiave per cercare in Microsoft Edge",
            L"Cerca in Microsoft Edge: \""
        };
    }
    if (lang == L"ru") {
        return {
            L"Тема поверхности (Surface Theme)",
            L"Liquid Glass (Жидкое стекло)",
            L"Размытие Windows (Windows Blur)",
            L"Непрозрачный фон (Opaque)",
            L"Оттенок Liquid Glass (-100 – 100)",
            L"Стекло: Макс. прозрачность (-100)",
            L"Стекло: Экстра прозрачность (-80)",
            L"Стекло: Ультрапрозрачное (-50)",
            L"Стекло: Чистое по умолчанию (0)",
            L"Стекло: Прозрачное – Clear (15)",
            L"Стекло: Баланс iOS – Balanced (30)",
            L"Стекло: Средний тон – Medium (55)",
            L"Стекло: Высокий контраст – Tinted (80)",
            L"Непрозрачность Windows Blur (0 – 255)",
            L"Цветовой режим (Светлый / Тёмный)",
            L"Авто – Как в системе Windows",
            L"Тёмный режим (Dark Mode)",
            L"Светлый режим (Light Mode)",
            L"Размер интерфейса (UI Size)",
            L"85% – Компактный",
            L"90% – Чуть меньше",
            L"100% – По умолчанию (Стандарт)",
            L"110% – Средне-крупный",
            L"120% – Крупный (Large)",
            L"Ширина окна (Width px)",
            L"Высота панели (Height px)",
            L"Размер шрифта (Font Size pt)",
            L"Сбросить размер к пресету",
            L"Минималистичный режим (Скрыть Обзор и Выход)",
            L"Свой фон (Изображение / GIF / MP4)",
            L"Выбрать файл фона (Изображение / GIF / MP4 ≤ 50 МБ)...",
            L"Удалить пользовательский фон",
            L"Прозрачность медиафона (20 – 255)",
            L"Прозрачность фона: Мягкая (90)",
            L"Прозрачность фона: Средняя (150)",
            L"Прозрачность фона: Яркая (215)",
            L"Прозрачность фона: Полная (255)",
            L"История и подсказки (До 50 / Прокрутка 5)",
            L"Макс. число элементов (0 – 50)",
            L"Видимых строк (3 – 10)",
            L"Лимит истории: 10 элементов",
            L"Лимит истории: 30 (Показ 5 + прокрутка)",
            L"Лимит истории: 35 (По умолчанию)",
            L"Лимит истории: 50 (Максимум)",
            L"Отключить список истории (0)",
            L"Показывать время запуска",
            L"Подсказки программ и ggsearch:/msesearch:",
            L"Язык интерфейса (Display Language)",
            L"Авто – Язык системы",
            L"Очистить всю сохранённую историю",
            L"Свой размер и шрифт (Предпросмотр)",
            L"Сбросить (Reset)",
            L"Готово (OK)",
            L"Фоновые медиа (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Все файлы (*.*)\0*.*\0\0",
            L"Выберите изображение, анимацию GIF или видео MP4 для фона (до 20-50 МБ)",
            L"Файл превышает лимит размера (%d МБ). Выберите файл меньше %d МБ.",
            L"Введите запрос для поиска в Google через Chrome",
            L"Поиск Google (Chrome): \"",
            L"Введите запрос для поиска в Microsoft Edge",
            L"Поиск в Microsoft Edge: \""
        };
    }
    if (lang == L"uk") {
        return {
            L"Тема поверхні (Surface Theme)",
            L"Liquid Glass (Рідке скло)",
            L"Розмиття Windows (Windows Blur)",
            L"Непрозорий фон (Opaque)",
            L"Відтінок Liquid Glass (-100 – 100)",
            L"Скло: Макс. прозорість (-100)",
            L"Скло: Екстра прозорість (-80)",
            L"Скло: Ультрапрозоре (-50)",
            L"Скло: Чисте за замовчуванням (0)",
            L"Скло: Прозоре – Clear (15)",
            L"Скло: Баланс iOS – Balanced (30)",
            L"Скло: Середній тон – Medium (55)",
            L"Скло: Високий контраст – Tinted (80)",
            L"Непрозорість Windows Blur (0 – 255)",
            L"Колірний режим (Світлий / Темний)",
            L"Авто – Як у системі Windows",
            L"Темний режим (Dark Mode)",
            L"Світлий режим (Light Mode)",
            L"Розмір інтерфейсу (UI Size)",
            L"85% – Компактний",
            L"90% – Трохи менший",
            L"100% – За замовчуванням (Стандарт)",
            L"110% – Середньо-великий",
            L"120% – Великий (Large)",
            L"Ширина вікна (Width px)",
            L"Висота панелі (Height px)",
            L"Розмір шрифту (Font Size pt)",
            L"Скинути розмір до пресету",
            L"Мінімалістичний режим (Сховати Огляд і Вихід)",
            L"Власний фон (Зображення / GIF / MP4)",
            L"Вибрати файл фону (Зображення / GIF / MP4 ≤ 50 МБ)...",
            L"Видалити власний фон",
            L"Прозорість медіафону (20 – 255)",
            L"Прозорість фону: М'яка (90)",
            L"Прозорість фону: Збалансована (150)",
            L"Прозорість фону: Яскрава (215)",
            L"Прозорість фону: Повна (255)",
            L"Історія та підказки (До 50 / Прокрутка 5)",
            L"Макс. кількість елементів (0 – 50)",
            L"Видимих рядків (3 – 10)",
            L"Ліміт історії: 10 елементів",
            L"Ліміт історії: 30 (Показ 5 + прокрутка)",
            L"Ліміт історії: 35 (За замовчуванням)",
            L"Ліміт історії: 50 (Максимум)",
            L"Вимкнути список історії (0)",
            L"Показувати час виконання",
            L"Підказки програм і ggsearch:/msesearch:",
            L"Мова інтерфейсу (Display Language)",
            L"Авто – Мова системи",
            L"Очистити всю збережену історію",
            L"Власний розмір і шрифт (Перегляд наживо)",
            L"Скинути (Reset)",
            L"Готово (OK)",
            L"Фонові медіа (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Усі файли (*.*)\0*.*\0\0",
            L"Виберіть зображення, анімацію GIF або відео MP4 для фону (до 20-50 МБ)",
            L"Файл перевищує ліміт розміру (%d МБ). Виберіть файл менше %d МБ.",
            L"Введіть запит для пошуку в Google через Chrome",
            L"Пошук Google (Chrome): \"",
            L"Введіть запит для пошуку в Microsoft Edge",
            L"Пошук у Microsoft Edge: \""
        };
    }
    if (lang == L"pl") {
        return {
            L"Motyw powierzchni (Surface Theme)",
            L"Liquid Glass",
            L"Rozmycie Windows (Windows Blur)",
            L"Nieprzezroczysty (Opaque)",
            L"Odcień Liquid Glass (-100 – 100)",
            L"Szkło: Maks. przezroczystość (-100)",
            L"Szkło: Ekstremalna przezroczystość (-80)",
            L"Szkło: Ultraprzezroczyste (-50)",
            L"Szkło: Domyślne czyste (0)",
            L"Szkło: Czyste – Clear (15)",
            L"Szkło: Zrównoważone iOS – Balanced (30)",
            L"Szkło: Średni odcień – Medium (55)",
            L"Szkło: Wysoki kontrast – Tinted (80)",
            L"Nieprzezroczystość Windows Blur (0 – 255)",
            L"Tryb kolorów (Jasny / Ciemny)",
            L"Automatycznie – Zgodnie z Windows",
            L"Tryb ciemny (Dark Mode)",
            L"Tryb jasny (Light Mode)",
            L"Rozmiar interfejsu (UI Size)",
            L"85% – Kompaktowy",
            L"90% – Nieco mniejszy",
            L"100% – Domyślny (Standard)",
            L"110% – Średnio duży",
            L"120% – Duży (Large)",
            L"Szerokość (Width px)",
            L"Wysokość paska (Height px)",
            L"Rozmiar czcionki (Font Size pt)",
            L"Przywróć rozmiar z ustawienia",
            L"Tryb minimalny (Ukryj Przeglądaj i Wyjdź)",
            L"Własne tło (Obraz / GIF / MP4)",
            L"Wybierz plik tła (Obraz / GIF / MP4 ≤ 50MB)...",
            L"Usuń własne tło",
            L"Krycie multimediów tła (20 – 255)",
            L"Krycie tła: Subtelne (90)",
            L"Krycie tła: Zrównoważone (150)",
            L"Krycie tła: Wyraźne (215)",
            L"Krycie tła: Pełne (255)",
            L"Historia i sugestie (Do 50 / Przewijaj 5)",
            L"Maks. zapisanych elementów (0 – 50)",
            L"Widoczne wiersze (3 – 10)",
            L"Limit historii: 10 elementów",
            L"Limit historii: 30 (Pokaż 5 + przewijanie)",
            L"Limit historii: 35 (Domyślnie)",
            L"Limit historii: 50 (Maksimum)",
            L"Wyłącz listę historii (0)",
            L"Pokaż czas wykonania",
            L"Sugestie aplikacji i ggsearch:/msesearch:",
            L"Język wyświetlania (Display Language)",
            L"Automatycznie – Domyślny systemu",
            L"Wyczyść całą zapisaną historię",
            L"Własny rozmiar i czcionka (Podgląd na żywo)",
            L"Resetuj",
            L"Gotowe (OK)",
            L"Multimedia tła (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Wszystkie pliki (*.*)\0*.*\0\0",
            L"Wybierz obraz, animowany GIF lub wideo MP4 jako tło (maks. 20-50 MB)",
            L"Wybrany plik przekracza limit (%d MB). Wybierz plik mniejszy niż %d MB.",
            L"Wpisz słowo kluczowe, aby wyszukać w Google (Chrome)",
            L"Szukaj w Google (Chrome): \"",
            L"Wpisz słowo kluczowe, aby wyszukać w Microsoft Edge",
            L"Szukaj w Microsoft Edge: \""
        };
    }
    if (lang == L"nl") {
        return {
            L"Oppervlaktethema (Surface Theme)",
            L"Liquid Glass",
            L"Windows-vervaging (Windows Blur)",
            L"Ondoorzichtig (Opaque)",
            L"Liquid Glass-tint (-100 – 100)",
            L"Glas: Max. transparant (-100)",
            L"Glas: Extreem transparant (-80)",
            L"Glas: Ultratransparant (-50)",
            L"Glas: Standaard helder (0)",
            L"Glas: Helder – Clear (15)",
            L"Glas: Gebalanceerd iOS – Balanced (30)",
            L"Glas: Gemiddelde tint – Medium (55)",
            L"Glas: Hoog contrast – Tinted (80)",
            L"Windows Blur-dekking (0 – 255)",
            L"Kleurmodus (Licht / Donker)",
            L"Automatisch – Volg Windows",
            L"Donkere modus (Dark Mode)",
            L"Lichte modus (Light Mode)",
            L"UI-grootte / Schaal",
            L"85% – Compact",
            L"90% – Iets kleiner",
            L"100% – Standaard",
            L"110% – Middelgroot",
            L"120% – Groot (Large)",
            L"Breedte (Width px)",
            L"Balkhoogte (Height px)",
            L"Tekengrootte (Font Size pt)",
            L"Grootte herstellen naar voorinstelling",
            L"Minimale modus (Verberg Bladeren & Sluiten)",
            L"Aangepaste achtergrond (Afbeelding / GIF / MP4)",
            L"Kies achtergrondbestand (Afbeelding / GIF / MP4 ≤ 50MB)...",
            L"Aangepaste achtergrond wissen",
            L"Media-dekking (20 – 255)",
            L"Dekking: Subtiel (90)",
            L"Dekking: Gebalanceerd (150)",
            L"Dekking: Helder (215)",
            L"Dekking: Volledig (255)",
            L"Geschiedenis & suggesties (Max 50 / Scroll 5)",
            L"Max. opgeslagen items (0 – 50)",
            L"Zichtbare rijen (3 – 10)",
            L"Max. opgeslagen: 10 items",
            L"Max. opgeslagen: 30 items (Toon 5 + Scroll)",
            L"Max. opgeslagen: 35 items (Standaard)",
            L"Max. opgeslagen: 50 items (Maximum)",
            L"Geschiedenislijst uitschakelen (0)",
            L"Uitvoeringstijd tonen",
            L"App-suggesties & ggsearch:/msesearch:",
            L"Weergavetaal (Display Language)",
            L"Automatisch – Systeemstandaard",
            L"Alle opgeslagen geschiedenis wissen",
            L"Aangepaste grootte & lettertype (Live voorbeeld)",
            L"Standaard (Reset)",
            L"Klaar (OK)",
            L"Achtergrondmedia (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Alle bestanden (*.*)\0*.*\0\0",
            L"Selecteer een afbeelding, geanimeerde GIF of MP4-video als achtergrond (max. 20-50 MB)",
            L"Het geselecteerde bestand overschrijdt de limiet (%d MB). Kies een bestand onder %d MB.",
            L"Typ een trefwoord om met Google in Chrome te zoeken",
            L"Zoeken op Google (Chrome): \"",
            L"Typ een trefwoord om in Microsoft Edge te zoeken",
            L"Zoeken in Microsoft Edge: \""
        };
    }
    if (lang == L"tr") {
        return {
            L"Yüzey Teması (Surface Theme)",
            L"Liquid Glass",
            L"Windows Bulanıklığı (Windows Blur)",
            L"Opak (Bulanıklık Yok)",
            L"Liquid Glass Tonu (-100 – 100)",
            L"Cam: Maksimum Şeffaf (-100)",
            L"Cam: Aşırı Şeffaf (-80)",
            L"Cam: Ultra Şeffaf (-50)",
            L"Cam: Varsayılan Net (0)",
            L"Cam: Net – Clear (15)",
            L"Cam: Dengeli iOS – Balanced (30)",
            L"Cam: Orta Ton – Medium Tint (55)",
            L"Cam: Yüksek Kontrast – Tinted (80)",
            L"Windows Blur Opaklığı (0 – 255)",
            L"Renk Modu (Açık / Koyu)",
            L"Otomatik – Windows'u Takip Et",
            L"Koyu Mod (Dark Mode)",
            L"Açık Mod (Light Mode)",
            L"Arayüz Boyutu / Ölçek",
            L"85% – Kompakt",
            L"90% – Biraz Küçük",
            L"100% – Varsayılan (Standart)",
            L"110% – Orta Büyük",
            L"120% – Büyük (Large)",
            L"Genişlik (Width px)",
            L"Çubuk Yüksekliği (Height px)",
            L"Yazı Tipi Boyutu (Font Size pt)",
            L"Özel Boyutu Ön Ayara Sıfırla",
            L"Minimal Mod (Gözat ve Çıkış'ı Gizle)",
            L"Özel Arka Plan (Resim / GIF / MP4)",
            L"Arka Plan Dosyası Seç (Resim / GIF / MP4 ≤ 50MB)...",
            L"Özel Arka Planı Kaldır",
            L"Medya Opaklığı (20 – 255)",
            L"Medya Opaklığı: Hafif (90)",
            L"Medya Opaklığı: Dengeli (150)",
            L"Medya Opaklığı: Canlı (215)",
            L"Medya Opaklığı: Tam (255)",
            L"Geçmiş ve Öneriler (En Fazla 50 / 5 Satır Kaydır)",
            L"Maks. Kayıtlı Öğe (0 – 50)",
            L"Görünür Satır Sayısı (3 – 10)",
            L"Kayıt Sınırı: 10 Öğe",
            L"Kayıt Sınırı: 30 Öğe (5 Göster + Kaydır)",
            L"Kayıt Sınırı: 35 Öğe (Varsayılan)",
            L"Kayıt Sınırı: 50 Öğe (Maksimum)",
            L"Geçmiş Listesini Kapat (0)",
            L"Çalıştırma Zamanını Göster",
            L"Uygulama Önerileri ve ggsearch:/msesearch:",
            L"Görüntüleme Dili (Display Language)",
            L"Otomatik – Sistem Varsayılanı",
            L"Kayıtlı Tüm Geçmişi Temizle",
            L"Özel Boyut ve Yazı Tipi (Canlı Önizleme)",
            L"Sıfırla (Reset)",
            L"Tamam (OK)",
            L"Arka Plan Medyası (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Tüm Dosyalar (*.*)\0*.*\0\0",
            L"Arka plan için resim, hareketli GIF veya MP4 video seçin (maks. 20-50 MB)",
            L"Seçilen dosya boyut sınırını (%d MB) aşıyor. Lütfen %d MB altında bir dosya seçin.",
            L"Chrome ile Google'da aramak için anahtar kelime yazın",
            L"Google'da Ara (Chrome): \"",
            L"Microsoft Edge ile aramak için anahtar kelime yazın",
            L"Microsoft Edge'de Ara: \""
        };
    }
    if (lang == L"hi") {
        return {
            L"सतह थीम (Surface Theme)",
            L"Liquid Glass",
            L"विंडोज़ ब्लर (Windows Blur)",
            L"अपारदर्शी (Opaque)",
            L"Liquid Glass टिंट (-100 – 100)",
            L"ग्लास: अधिकतम पारदर्शी (-100)",
            L"ग्लास: अत्यंत पारदर्शी (-80)",
            L"ग्लास: अल्ट्रा पारदर्शी (-50)",
            L"ग्लास: डिफ़ॉल्ट क्लियर (0)",
            L"ग्लास: क्लियर – Clear (15)",
            L"ग्लास: iOS संतुलित – Balanced (30)",
            L"ग्लास: मध्यम टिंट – Medium (55)",
            L"ग्लास: उच्च कंट्रास्ट – Tinted (80)",
            L"Windows Blur अपारदर्शिता (0 – 255)",
            L"रंग मोड (लाइट / डार्क)",
            L"ऑटो – विंडोज़ का अनुसरण करें",
            L"डार्क मोड (Dark Mode)",
            L"लाइट मोड (Light Mode)",
            L"UI आकार / स्केल (UI Size)",
            L"85% – कॉम्पैक्ट (Compact)",
            L"90% – थोड़ा छोटा",
            L"100% – डिफ़ॉल्ट (मानक)",
            L"110% – मध्यम बड़ा",
            L"120% – बड़ा (Large)",
            L"चौड़ाई (Width px)",
            L"बार की ऊँचाई (Height px)",
            L"फ़ॉन्ट आकार (Font Size pt)",
            L"कस्टम आकार को प्रीसेट पर रीसेट करें",
            L"मिनिमल मोड (ब्राउज़ और बंद करें छुपाएँ)",
            L"कस्टम पृष्ठभूमि (छवि / GIF / MP4)",
            L"पृष्ठभूमि फ़ाइल चुनें (छवि / GIF / MP4 ≤ 50MB)...",
            L"कस्टम पृष्ठभूमि हटाएँ",
            L"मीडिया अपारदर्शिता (20 – 255)",
            L"मीडिया अपारदर्शिता: हल्का (90)",
            L"मीडिया अपारदर्शिता: संतुलित (150)",
            L"मीडिया अपारदर्शिता: स्पष्ट (215)",
            L"मीडिया अपारदर्शिता: पूर्ण (255)",
            L"इतिहास और सुझाव (अधिकतम 50 / 5 पंक्तियाँ)",
            L"अधिकतम संग्रहीत आइटम (0 – 50)",
            L"दिखने वाली पंक्तियाँ (3 – 10)",
            L"अधिकतम आइटम: 10",
            L"अधिकतम आइटम: 30 (5 दिखाएँ + स्क्रॉल)",
            L"अधिकतम आइटम: 35 (डिफ़ॉल्ट)",
            L"अधिकतम आइटम: 50 (अधिकतम)",
            L"इतिहास सूची बंद करें (0)",
            L"निष्पादन समय दिखाएँ",
            L"ऐप सुझाव और ggsearch:/msesearch:",
            L"प्रदर्शन भाषा (Display Language)",
            L"ऑटो – सिस्टम डिफ़ॉल्ट",
            L"सहेजा गया सभी इतिहास साफ़ करें",
            L"कस्टम आकार और फ़ॉन्ट (लाइव पूर्वावलोकन)",
            L"रीसेट (Reset)",
            L"पूर्ण (OK)",
            L"पृष्ठभूमि मीडिया (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0सभी फ़ाइलें (*.*)\0*.*\0\0",
            L"पृष्ठभूमि के लिए छवि, एनिमेटेड GIF या MP4 वीडियो चुनें (अधिकतम 20-50 MB)",
            L"चयनित फ़ाइल आकार सीमा (%d MB) से अधिक है। कृपया %d MB से कम की फ़ाइल चुनें।",
            L"Chrome में Google खोजने के लिए कीवर्ड टाइप करें",
            L"Google खोज (Chrome): \"",
            L"Microsoft Edge में खोजने के लिए कीवर्ड टाइप करें",
            L"Microsoft Edge खोज: \""
        };
    }
    if (lang == L"ar") {
        return {
            L"سمة السطح (Surface Theme)",
            L"Liquid Glass (الزجاج السائل)",
            L"تمويه ويندوز (Windows Blur)",
            L"معتم (Opaque)",
            L"درجة Liquid Glass (-100 – 100)",
            L"زجاج: أقصى شفافية (-100)",
            L"زجاج: شفافية فائقة جداً (-80)",
            L"زجاج: شفافية فائقة (-50)",
            L"زجاج: صافي افتراضي (0)",
            L"زجاج: صافي – Clear (15)",
            L"زجاج: متوازن iOS – Balanced (30)",
            L"زجاج: متوسط التظليل – Medium (55)",
            L"زجاج: تباين عالٍ – Tinted (80)",
            L"عتامة تمويه ويندوز (0 – 255)",
            L"وضع الألوان (فاتح / داكن)",
            L"تلقائي – اتباع نظام ويندوز",
            L"الوضع الداكن (Dark Mode)",
            L"الوضع الفاتح (Light Mode)",
            L"حجم الواجهة والمقياس (UI Size)",
            L"85% – مدمج (Compact)",
            L"90% – أصغر قليلاً",
            L"100% – افتراضي (قياسي)",
            L"110% – متوسط كبير",
            L"120% – كبير (Large)",
            L"العرض (Width px)",
            L"ارتفاع الشريط (Height px)",
            L"حجم الخط (Font Size pt)",
            L"إعادة تعيين الحجم إلى الإعداد المسبق",
            L"الوضع المبسط (إخفاء استعراض وخروج)",
            L"خلفية مخصصة (صورة / GIF / MP4)",
            L"اختيار ملف الخلفية (صورة / GIF / MP4 ≤ 50MB)...",
            L"إزالة الخلفية المخصصة",
            L"عتامة وسائط الخلفية (20 – 255)",
            L"عتامة الخلفية: خفيفة (90)",
            L"عتامة الخلفية: متوازنة (150)",
            L"عتامة الخلفية: واضحة (215)",
            L"عتامة الخلفية: كاملة (255)",
            L"السجل والاقتراحات (حتى 50 / تمرير 5)",
            L"الحد الأقصى للعناصر المحفوظة (0 – 50)",
            L"الصفوف المرئية (3 – 10)",
            L"الحد الأقصى: 10 عناصر",
            L"الحد الأقصى: 30 عنصراً (عرض 5 + تمرير)",
            L"الحد الأقصى: 35 عنصراً (افتراضي)",
            L"الحد الأقصى: 50 عنصراً (الأقصى)",
            L"تعطيل قائمة السجل (0)",
            L"إظهار وقت التنفيذ",
            L"اقتراحات التطبيقات و ggsearch:/msesearch:",
            L"لغة العرض (Display Language)",
            L"تلقائي – لغة النظام الافتراضية",
            L"مسح كل السجل المحفوظ",
            L"حجم وخط مخصص (معاينة مباشرة)",
            L"إعادة تعيين",
            L"تم (OK)",
            L"وسائط الخلفية (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0كل الملفات (*.*)\0*.*\0\0",
            L"اختر صورة أو GIF متحرك أو فيديو MP4 كخلفية (الحد الأقصى 20-50 ميجابايت)",
            L"الملف المحدد يتجاوز الحد المسموح (%d ميجابايت). يرجى اختيار ملف أقل من %d ميجابايت.",
            L"اكتب كلمة مفتاحية للبحث في Google عبر Chrome",
            L"بحث Google (Chrome): \"",
            L"اكتب كلمة مفتاحية للبحث في Microsoft Edge",
            L"بحث Microsoft Edge: \""
        };
    }

    // Default: English ("en")
    return {
        L"Surface Theme",
        L"Liquid Glass",
        L"Windows Blur",
        L"Off – Opaque",
        L"Liquid Glass Tint (-100 – 100)",
        L"Glass: Max Transparent (-100)",
        L"Glass: Extreme Transparent (-80)",
        L"Glass: Ultra Transparent (-50)",
        L"Glass: Clear Default (0)",
        L"Glass: Clear (15)",
        L"Glass: Balanced iOS (30)",
        L"Glass: Medium Tint (55)",
        L"Glass: High Contrast Tinted (80)",
        L"Windows Blur Opacity (0 – 255)",
        L"Color Mode",
        L"Auto – Follow Windows",
        L"Dark Mode",
        L"Light Mode",
        L"UI Size / Scale",
        L"85% – Compact",
        L"90% – Slightly Smaller",
        L"100% – Default (Standard)",
        L"110% – Medium Large",
        L"120% – Large",
        L"Width (px)",
        L"Bar Height (px)",
        L"Font Size (pt)",
        L"Reset Custom Size to Preset",
        L"Minimal Mode (Hide Browse & Exit)",
        L"Custom Background (Image / GIF / MP4)",
        L"Select Background File (Image / GIF / MP4 ≤ 50MB)...",
        L"Clear Custom Background",
        L"Media Opacity (20 – 255)",
        L"Media Opacity: Subtle (90)",
        L"Media Opacity: Balanced (150)",
        L"Media Opacity: Vivid (215)",
        L"Media Opacity: Full (255)",
        L"History & Suggestions (Up to 50 / Scroll 5)",
        L"Max Stored Items (0 – 50)",
        L"Visible Rows (3 – 10)",
        L"Max Stored Items: 10",
        L"Max Stored Items: 30 (Show 5 + Scroll)",
        L"Max Stored Items: 35 (Default)",
        L"Max Stored Items: 50 (Maximum)",
        L"Disable History List (0)",
        L"Show Execution Timestamp",
        L"App Suggestions & ggsearch:/msesearch:",
        L"Display Language",
        L"Auto – System Default",
        L"Clear All Stored History",
        L"Custom Size & Font (Live Preview)",
        L"Reset Default",
        L"Done (OK)",
        L"Background Media (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0All Files (*.*)\0*.*\0\0",
        L"Select static image, animated GIF, or MP4 video background (max 20-50 MB)",
        L"The selected background file exceeds the maximum allowed size (%d MB). Please choose an image/GIF/MP4 under %d MB.",
        L"Type a keyword to search Google in Chrome",
        L"Search Google in Chrome: \"",
        L"Type a keyword to search in Microsoft Edge",
        L"Search in Microsoft Edge: \""
    };
}

LocalizedStrings GetLocalizedStringsForLanguage(std::wstring lang) {
    lang = ResolveEffectiveLanguageCode(std::move(lang));

    if (lang == L"vi") {
        return {
            L"Nhập tên lệnh hoặc chương trình để chạy",
            L"Duyệt",
            L"Chạy",
            L"Thoát",
            L"Hôm nay",
            L"Chọn chương trình (.exe) để đưa vào ô nhập",
            L"Chương trình (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tất cả tệp (*.*)\0*.*\0\0",
            L"Ứng dụng"
        };
    }
    if (lang == L"th") {
        return {
            L"พิมพ์ชื่อคำสั่งหรือโปรแกรมเพื่อเรียกใช้งาน",
            L"เรียกดู",
            L"เรียกใช้",
            L"ออก",
            L"วันนี้",
            L"เลือกโปรแกรมที่ต้องการเรียกใช้งาน",
            L"โปรแกรม (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0ไฟล์ทั้งหมด (*.*)\0*.*\0\0",
            L"แอปพลิเคชัน"
        };
    }
    if (lang == L"id") {
        return {
            L"Ketik nama perintah atau program untuk dijalankan",
            L"Telusuri",
            L"Jalankan",
            L"Keluar",
            L"Hari ini",
            L"Pilih program yang akan dijalankan",
            L"Program (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Semua File (*.*)\0*.*\0\0",
            L"Aplikasi"
        };
    }
    if (lang == L"ms") {
        return {
            L"Taip nama arahan atau program untuk dijalankan",
            L"Semak",
            L"Jalankan",
            L"Keluar",
            L"Hari ini",
            L"Pilih program untuk dijalankan",
            L"Program (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Semua Fail (*.*)\0*.*\0\0",
            L"Aplikasi"
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
            L"プログラム (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0すべてのファイル (*.*)\0*.*\0\0",
            L"アプリケーション"
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
            L"程序 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0所有文件 (*.*)\0*.*\0\0",
            L"应用程序"
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
            L"程式 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0所有檔案 (*.*)\0*.*\0\0",
            L"應用程式"
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
            L"프로그램 (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0모든 파일 (*.*)\0*.*\0\0",
            L"애플리케이션"
        };
    }
    if (lang == L"fr") {
        return {
            L"Entrez le nom d'une commande",
            L"Parcourir",
            L"Exécuter",
            L"Quitter",
            L"Aujourd'hui",
            L"Sélectionner un programme",
            L"Programmes (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tous les fichiers (*.*)\0*.*\0\0",
            L"Application"
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
            L"Programme (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Alle Dateien (*.*)\0*.*\0\0",
            L"Anwendung"
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
            L"Programas (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Todos los archivos (*.*)\0*.*\0\0",
            L"Aplicación"
        };
    }
    if (lang == L"pt") {
        return {
            L"Digite o nome de um comando ou programa para executar",
            L"Procurar",
            L"Executar",
            L"Sair",
            L"Hoje",
            L"Selecionar um programa executável",
            L"Programas (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Todos os arquivos (*.*)\0*.*\0\0",
            L"Aplicativo"
        };
    }
    if (lang == L"it") {
        return {
            L"Digitare il nome del programma da eseguire",
            L"Sfoglia",
            L"Esegui",
            L"Esci",
            L"Oggi",
            L"Seleziona un programma da eseguire",
            L"Programmi (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tutti i file (*.*)\0*.*\0\0",
            L"Applicazione"
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
            L"Программы (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Все файлы (*.*)\0*.*\0\0",
            L"Приложение"
        };
    }
    if (lang == L"uk") {
        return {
            L"Введіть ім'я команди або програми для виконання",
            L"Огляд",
            L"Виконати",
            L"Вихід",
            L"Сьогодні",
            L"Виберіть програму для виконання",
            L"Програми (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Усі файли (*.*)\0*.*\0\0",
            L"Програма"
        };
    }
    if (lang == L"pl") {
        return {
            L"Wpisz nazwę polecenia lub programu do uruchomienia",
            L"Przeglądaj",
            L"Uruchom",
            L"Wyjdź",
            L"Dzisiaj",
            L"Wybierz program do uruchomienia",
            L"Programy (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Wszystkie pliki (*.*)\0*.*\0\0",
            L"Aplikacja"
        };
    }
    if (lang == L"nl") {
        return {
            L"Typ de naam van een opdracht of programma",
            L"Bladeren",
            L"Uitvoeren",
            L"Sluiten",
            L"Vandaag",
            L"Selecteer een uitvoerbaar programma",
            L"Programma's (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Alle bestanden (*.*)\0*.*\0\0",
            L"Toepassing"
        };
    }
    if (lang == L"tr") {
        return {
            L"Çalıştırmak istediğiniz komutun adını yazın",
            L"Gözat",
            L"Çalıştır",
            L"Çıkış",
            L"Bugün",
            L"Çalıştırılacak bir program seçin",
            L"Programlar (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0Tüm Dosyalar (*.*)\0*.*\0\0",
            L"Uygulama"
        };
    }
    if (lang == L"hi") {
        return {
            L"चलाने के लिए किसी कमांड या प्रोग्राम का नाम टाइप करें",
            L"ब्राउज़",
            L"चलाएँ",
            L"बंद करें",
            L"आज",
            L"चलाने के लिए एक प्रोग्राम चुनें",
            L"प्रोग्राम (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0सभी फ़ाइलें (*.*)\0*.*\0\0",
            L"एप्लिकेशन"
        };
    }
    if (lang == L"ar") {
        return {
            L"اكتب اسم أمر أو برنامج لتشغيله",
            L"استعراض",
            L"تشغيل",
            L"خروج",
            L"اليوم",
            L"اختر برنامجاً لتشغيله",
            L"البرامج (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0كل الملفات (*.*)\0*.*\0\0",
            L"تطبيق"
        };
    }

    return {
        L"Type the name of a command to run",
        L"Browse",
        L"Run",
        L"Exit",
        L"Today",
        L"Select an executable program",
        L"Programs (*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl)\0*.exe;*.bat;*.cmd;*.com;*.msc;*.cpl\0All Files (*.*)\0*.*\0\0",
        L"Application"
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

std::wstring ToLowerString(std::wstring_view sv) {
    std::wstring res(sv);
    for (wchar_t& ch : res) {
        ch = towlower(ch);
    }
    return res;
}

std::wstring NormalizeCmdKey(const std::wstring& cmd) {
    return ToLowerString(TrimCommandString(cmd));
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

const std::wstring& GetCachedUserSidString() {
    static const std::wstring kSid = GetCurrentUserSidString();
    return kSid;
}

const std::wstring& GetHistoryStorageKey() {
    static const std::wstring kKey = L"HistoryTimestamps_" + GetCachedUserSidString();
    return kKey;
}

const std::wstring& GetHiddenMruStorageKey() {
    static const std::wstring kHiddenKey = L"HiddenRunMru_" + GetCachedUserSidString();
    return kHiddenKey;
}

const std::wstring& GetClearAckStorageKey() {
    static const std::wstring kAckKey = L"ClearHistoryAck_" + GetCachedUserSidString();
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

std::wstring GetUserOverrideKey(const wchar_t* name) {
    return std::wstring(L"InAppCfg_") + name + L"_" + GetCachedUserSidString();
}

void ClearAllInAppOverrides() {
    const wchar_t* keys[] = {
        L"language", L"surfaceTheme", L"colorMode", L"minimalMode", L"uiScalePercent",
        L"customDialogWidth", L"customBarHeight", L"customFontSize",
        L"glassOpacity", L"blurOpacity", L"visibleHistoryRows",
        L"maxHistoryItems", L"showTimestamp",
        L"enableAppSuggestions", L"customBackgroundPath", L"customBackgroundOpacity"
    };
    for (const wchar_t* k : keys) {
        Wh_DeleteValue(GetUserOverrideKey(k).c_str());
    }
}

void LoadSettings(bool fromSettingsChanged = false) {
    if (fromSettingsChanged) {
        // When user explicitly saves settings in Windhawk UI, sync Windhawk settings
        // by clearing old in-app overrides except customBackgroundPath if Windhawk's field is empty.
        auto bgSettingCheck = WindhawkUtils::StringSetting::make(L"customBackgroundPath");
        std::wstring savedBg;
        wchar_t bgBuf[MAX_PATH] = {};
        if (Wh_GetStringValue(GetUserOverrideKey(L"customBackgroundPath").c_str(), bgBuf, MAX_PATH) > 0) {
            savedBg = bgBuf;
        }
        ClearAllInAppOverrides();
        if (bgSettingCheck.get()[0] == L'\0' && !savedBg.empty() && savedBg != L"-") {
            Wh_SetStringValue(GetUserOverrideKey(L"customBackgroundPath").c_str(), savedBg.c_str());
        }
    }

    auto langSetting = WindhawkUtils::StringSetting::make(L"language");
    auto surfaceSetting = WindhawkUtils::StringSetting::make(L"surfaceTheme");
    auto colorSetting = WindhawkUtils::StringSetting::make(L"colorMode");
    auto tbAction = WindhawkUtils::StringSetting::make(L"taskbarClickAction");
    auto bgPathSetting = WindhawkUtils::StringSetting::make(L"customBackgroundPath");

    ModSettings newSettings;
    newSettings.language = langSetting.get()[0] ? langSetting.get() : L"auto";

    wchar_t strOverride[MAX_PATH] = {};
    if (Wh_GetStringValue(GetUserOverrideKey(L"language").c_str(), strOverride, MAX_PATH) > 0 && strOverride[0]) {
        newSettings.language = strOverride;
    }

    std::wstring surfStr = surfaceSetting.get()[0] ? surfaceSetting.get() : L"glass";
    if (Wh_GetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), strOverride, MAX_PATH) > 0 && strOverride[0]) {
        surfStr = strOverride;
    }
    if (surfStr == L"blur") {
        newSettings.surfaceTheme = SurfaceTheme::Blur;
    } else if (surfStr == L"solid") {
        newSettings.surfaceTheme = SurfaceTheme::Solid;
    } else {
        newSettings.surfaceTheme = SurfaceTheme::Glass;
    }

    std::wstring colStr = colorSetting.get()[0] ? colorSetting.get() : L"auto";
    if (Wh_GetStringValue(GetUserOverrideKey(L"colorMode").c_str(), strOverride, MAX_PATH) > 0 && strOverride[0]) {
        colStr = strOverride;
    }
    if (colStr == L"light") {
        newSettings.colorMode = ColorMode::Light;
    } else if (colStr == L"dark") {
        newSettings.colorMode = ColorMode::Dark;
    } else {
        newSettings.colorMode = ColorMode::Auto;
    }

    int minModeOverride = Wh_GetIntValue(GetUserOverrideKey(L"minimalMode").c_str(), -1);
    newSettings.minimalMode = (minModeOverride >= 0) ? (minModeOverride != 0) : (Wh_GetIntSetting(L"minimalMode") != 0);

    auto scaleStrSetting = WindhawkUtils::StringSetting::make(L"uiScalePercent");
    int scaleSetting = (scaleStrSetting.get() && scaleStrSetting.get()[0])
                       ? _wtoi(scaleStrSetting.get())
                       : 100;
    if (scaleSetting <= 0) scaleSetting = 100;
    int scaleOverride = Wh_GetIntValue(GetUserOverrideKey(L"uiScalePercent").c_str(), -1);
    newSettings.uiScalePercent = std::clamp(scaleOverride > 0 ? scaleOverride : scaleSetting, 85, 120);

    int widthSetting = Wh_GetIntSetting(L"customDialogWidth");
    int widthOverride = Wh_GetIntValue(GetUserOverrideKey(L"customDialogWidth").c_str(), -1);
    int rawWidth = (widthOverride >= 0) ? widthOverride : widthSetting;
    newSettings.customDialogWidth = (rawWidth > 0) ? std::clamp(rawWidth, 360, 1200) : 0;

    int heightSetting = Wh_GetIntSetting(L"customBarHeight");
    int heightOverride = Wh_GetIntValue(GetUserOverrideKey(L"customBarHeight").c_str(), -1);
    int rawHeight = (heightOverride >= 0) ? heightOverride : heightSetting;
    if (rawHeight >= 150) {
        rawHeight = MulDiv(rawHeight, kBaseInputBarHeight, kBaseInputBarHeight + 5 * kBaseRowHeight + kBaseListPadTop + kBaseListPadBottom);
    }
    newSettings.customBarHeight = (rawHeight > 0) ? std::clamp(rawHeight, 36, 96) : 0;

    int fontSetting = Wh_GetIntSetting(L"customFontSize");
    int fontOverride = Wh_GetIntValue(GetUserOverrideKey(L"customFontSize").c_str(), -1);
    int rawFont = (fontOverride >= 0) ? fontOverride : fontSetting;
    newSettings.customFontSize = (rawFont > 0) ? std::clamp(rawFont, 9, 24) : 0;

    newSettings.taskbarCloseInsteadOfMinimize = (wcscmp(tbAction.get(), L"close") == 0);
    int glassOpSetting = Wh_GetIntSetting(L"glassOpacity");
    if (glassOpSetting < -100 || glassOpSetting > 100) glassOpSetting = -100;
    int glassOpOverride = Wh_GetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), -999);
    newSettings.glassOpacity = std::clamp((glassOpOverride >= -100 && glassOpOverride <= 100) ? glassOpOverride : glassOpSetting, -100, 100);

    int blurOpSetting = std::clamp(Wh_GetIntSetting(L"blurOpacity"), 0, 255);
    int blurOpOverride = Wh_GetIntValue(GetUserOverrideKey(L"blurOpacity").c_str(), -1);
    newSettings.blurOpacity = std::clamp(blurOpOverride >= 0 ? blurOpOverride : blurOpSetting, 0, 255);

    newSettings.customBackgroundPath = bgPathSetting.get() ? TrimCommandString(bgPathSetting.get()) : L"";
    if (Wh_GetStringValue(GetUserOverrideKey(L"customBackgroundPath").c_str(), strOverride, MAX_PATH) > 0 && strOverride[0]) {
        if (wcscmp(strOverride, L"-") == 0) {
            newSettings.customBackgroundPath.clear();
        } else {
            newSettings.customBackgroundPath = TrimCommandString(strOverride);
        }
    }
    if (!newSettings.customBackgroundPath.empty() && newSettings.customBackgroundPath.front() == L'"' && newSettings.customBackgroundPath.back() == L'"') {
        newSettings.customBackgroundPath = newSettings.customBackgroundPath.substr(1, newSettings.customBackgroundPath.size() - 2);
    }

    int maxMbSetting = Wh_GetIntSetting(L"customBackgroundMaxMb");
    if (maxMbSetting <= 0) maxMbSetting = 50;
    newSettings.customBackgroundMaxMb = std::clamp(maxMbSetting, 20, 50);

    int bgOpSetting = Wh_GetIntSetting(L"customBackgroundOpacity");
    if (bgOpSetting <= 0) {
        bgOpSetting = 255;
    }
    int bgOpOverride = Wh_GetIntValue(GetUserOverrideKey(L"customBackgroundOpacity").c_str(), -1);
    newSettings.customBackgroundOpacity = std::clamp(bgOpOverride >= 0 ? bgOpOverride : bgOpSetting, 20, 255);

    int appSugOverride = Wh_GetIntValue(GetUserOverrideKey(L"enableAppSuggestions").c_str(), -1);
    newSettings.enableAppSuggestions = (appSugOverride >= 0) ? (appSugOverride != 0) : (Wh_GetIntSetting(L"enableAppSuggestions") != 0);

    int maxHistSetting = Wh_GetIntSetting(L"maxHistoryItems");
    int maxHistOverride = Wh_GetIntValue(GetUserOverrideKey(L"maxHistoryItems").c_str(), -1);
    newSettings.maxHistoryItems = std::clamp(maxHistOverride >= 0 ? maxHistOverride : maxHistSetting, 0, 50);

    int visRowsSetting = Wh_GetIntSetting(L"visibleHistoryRows");
    if (visRowsSetting <= 0) visRowsSetting = 5;
    int visRowsOverride = Wh_GetIntValue(GetUserOverrideKey(L"visibleHistoryRows").c_str(), -1);
    newSettings.visibleHistoryRows = std::clamp(visRowsOverride >= 3 ? visRowsOverride : visRowsSetting, 3, 10);

    int showTsOverride = Wh_GetIntValue(GetUserOverrideKey(L"showTimestamp").c_str(), -1);
    newSettings.showTimestamp = (showTsOverride >= 0) ? (showTsOverride != 0) : (Wh_GetIntSetting(L"showTimestamp") != 0);

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
    std::wstring execTarget;
    std::wstring appName;
    std::wstring timeStr;
    HICON hIcon = nullptr;
    bool ownsIcon = true;
    bool isSuggestion = false;
    bool isShortcutFile = false;
    int webSearchKind = 0; // 0 = None, 1 = Google Chrome (ggsearch:), 2 = Microsoft Edge (msesearch:)
    std::wstring webSearchQuery;
};

struct ActionButtonsLayout {
    RECT rcBrowse;
    RECT rcRun;
    RECT rcClose;
    RECT rcGear;
    RECT rcEdit;
};

struct BgAnimFrame {
    std::vector<BYTE> pixels; // 32-bit BGRA premultiplied (Width * Height * 4)
    UINT delayMs = 33;
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
    std::vector<HistoryItem> suggestions;
    bool showingSuggestions = false;

    ModSettings settings;
    LocalizedStrings i18n = {};
    SurfaceTheme effectiveSurface = SurfaceTheme::Glass;
    bool isLightMode = false;
    bool backdropLightSample = false;
    bool customBgIsLight = false;
    bool customBgLumValid = false;

    UINT dpi = 96;
    int topAreaHeight = 0;
    int totalHeight = kBaseInputBarHeight;
    int normalX = 100;
    int normalY = 100;
    int scrollOffset = 0;
    int hoveredRow = -1;
    int pressedRow = -1;
    int selectedRow = -1;
    POINT mousePt = { -1, -1 };
    bool hasMousePt = false;
    bool scrollbarHovered = false;
    bool scrollbarDragging = false;
    int scrollbarDragStartY = 0;
    int scrollbarDragStartOffset = 0;
    bool browseHovered = false;
    bool browsePressed = false;
    bool btnHovered = false;
    bool btnPressed = false;
    bool closeHovered = false;
    bool closePressed = false;
    bool gearHovered = false;
    bool gearPressed = false;
    bool caretVisible = true;
    bool isTriggeringRun = false;
    bool isBrowsingFile = false;
    bool suppressSuggestionUpdate = false;
    bool pendingUninit = false;

    // Zero-latency direct window dragging state
    bool windowDragging = false;
    POINT windowDragStartCursor = { 0, 0 };
    int windowDragStartX = 0;
    int windowDragStartY = 0;

    // Real-time background window topology tracking (immune to mouse hover / menu popups)
    uint64_t lastTopologySig = 0;
    bool pendingTopologyRecapture = false;
    ULONGLONG topologyChangeTick = 0;

    // Persistent double-buffer DIB & Direct2D RenderTarget for 0-ms paint overhead
    HDC hCachedMemDC = nullptr;
    HBITMAP hCachedBmp = nullptr;
    HGDIOBJ hOldCachedBmp = nullptr;
    DWORD* cachedDibPixels = nullptr;
    int cachedDibW = 0;
    int cachedDibH = 0;
    ID2D1DCRenderTarget* pCachedRT = nullptr;

    // Custom Background Media State (Static Image / Animated GIF / MP4 Video)
    std::wstring loadedBgPath;
    UINT bgFrameW = 0;
    UINT bgFrameH = 0;
    std::vector<BgAnimFrame> bgFrames;
    size_t bgCurrentFrame = 0;
    IMFSourceReader* pVideoReader = nullptr;
    bool mfStarted = false;
    UINT videoIntervalMs = 33;
    std::vector<BYTE> videoFrameBuffer;
};

const std::vector<HistoryItem>& GetActiveRowItems(const RunDialogState* state) {
    return state->showingSuggestions ? state->suggestions : state->history;
}

// Atomic state shared safely across threads
std::atomic<HWND> g_hActiveRunDlg{nullptr};
std::atomic<HWND> g_hTrackedRunDlg{nullptr};
std::atomic<HWND> g_hFallbackInterceptedDlg{nullptr};
std::atomic<int> g_fallbackDialogRefCount{0};
std::atomic<ULONGLONG> g_openTick{0};
std::atomic<ULONGLONG> g_lastToggleTick{0};
std::atomic<bool> g_winRReleasedSinceOpen{false};
std::atomic<bool> g_isBrowsingFile{false};
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_dlgIsForeground{false};
std::atomic<ULONGLONG> g_deactivatedByTaskbarTick{0};
thread_local bool g_internalActivation = false;

inline bool IsActiveRunDialogAlive(HWND hDlg) {
    return hDlg && IsWindow(hDlg) && (IsWindowVisible(hDlg) || IsIconic(hDlg));
}

bool IsPointOnTaskbar(POINT pt) {
    HWND hHit = WindowFromPoint(pt);
    if (!hHit) return false;
    HWND hRoot = GetAncestor(hHit, GA_ROOT);
    if (!hRoot) hRoot = hHit;
    wchar_t cls[64] = {};
    if (!GetClassNameW(hRoot, cls, ARRAYSIZE(cls))) return false;
    return _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(cls, L"SecondaryTrayWnd") == 0;
}

bool ShouldMinimizeOnTaskbarClick(HWND hWnd, ULONGLONG now) {
    if (IsIconic(hWnd) || !IsWindowVisible(hWnd)) {
        return false;
    }
    POINT pt = {};
    GetCursorPos(&pt);
    RECT rcDlg = {};
    GetWindowRect(hWnd, &rcDlg);
    if (PtInRect(&rcDlg, pt)) {
        return false;
    }
    if (g_dlgIsForeground.load() && GetForegroundWindow() == hWnd) {
        return true;
    }
    ULONGLONG taskbarTick = g_deactivatedByTaskbarTick.load();
    if (taskbarTick != 0 && (now - taskbarTick < 500)) {
        return true;
    }
    return false;
}

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
        if (IsActiveRunDialogAlive(hDlg)) {
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

bool ResolveDialogLightContrast(const RunDialogState* state);
void ApplyWindowAppearance(HWND hDlg, RunDialogState* state);
void CloseActiveGearMenu();
void CloseRunDialog(HWND hDlg);
void SyncInputToNativeCombo(RunDialogState* state, const wchar_t* textOverride = nullptr);
void PerformDialogUninit(HWND hWnd, RunDialogState* state);
void RecomputeDialogMetrics(RunDialogState* state);
void UpdateDialogEditFont(RunDialogState* state);
void UpdateAppSuggestionsForQuery(RunDialogState* state, const std::wstring& query);
void ReleaseCustomBackgroundMedia(RunDialogState* state);
void ReloadCustomBackgroundMedia(RunDialogState* state, bool showErrorIfTooLarge = false);
void StepCustomBackgroundAnimation(RunDialogState* state);
void ApplyReloadedSettingsToDialog(RunDialogState* state);
void OpenGearSettingsMenu(RunDialogState* state);
void CheckAndUpdateBackdropInRealtime(HWND hWnd, RunDialogState* state, bool forceImmediate = false);

int GetDlgScale(const RunDialogState* state) {
    return state ? std::clamp(state->settings.uiScalePercent, 85, 120) : 100;
}

int DpiScaleS(int value, const RunDialogState* state) {
    return DpiScale(value, state ? state->dpi : 96, GetDlgScale(state));
}

float DpiScaleSF(float value, const RunDialogState* state) {
    return DpiScaleF(value, state ? state->dpi : 96, GetDlgScale(state));
}

float GetFontSizeSF(float baseDip, const RunDialogState* state) {
    if (state && state->settings.customFontSize > 0) {
        float customMain = (float)std::clamp(state->settings.customFontSize, 9, 24);
        float scaledDip = baseDip * (customMain / 13.5f);
        return scaledDip * (static_cast<float>(state->dpi) / 96.0f);
    }
    return DpiScaleSF(baseDip, state);
}

int GetDlgWidth(const RunDialogState* state) {
    if (state && state->settings.customDialogWidth >= 360) {
        int wDip = std::clamp(state->settings.customDialogWidth, 360, 1200);
        return DpiScale(wDip, state->dpi, 100);
    }
    return DpiScaleS(kBaseDlgWidth, state);
}

int GetInputBarHeight(const RunDialogState* state) {
    if (state && state->settings.customBarHeight > 0) {
        int barDip = std::clamp(state->settings.customBarHeight, 36, 96);
        return DpiScale(barDip, state->dpi, 100);
    }
    return DpiScaleS(kBaseInputBarHeight, state);
}

int GetRowHeight(const RunDialogState* state) {
    if (state && state->settings.customBarHeight > 0) {
        int barDip = std::clamp(state->settings.customBarHeight, 36, 96);
        int rowDip = std::clamp(MulDiv(kBaseRowHeight, barDip, kBaseInputBarHeight), 26, 72);
        return DpiScale(rowDip, state->dpi, 100);
    }
    return DpiScaleS(kBaseRowHeight, state);
}

int GetListPadTop(const RunDialogState* state) {
    return DpiScaleS(kBaseListPadTop, state);
}

int GetListPadBottom(const RunDialogState* state) {
    return DpiScaleS(kBaseListPadBottom, state);
}

inline bool HasActiveCustomBackground(const RunDialogState* state) {
    if (!state) return false;
    return (state->pVideoReader && !state->videoFrameBuffer.empty()) || !state->bgFrames.empty();
}

bool NeedsBackdropSnapshot(const RunDialogState* state) {
    return state &&
           ResolveEffectiveSurface(state->settings) == SurfaceTheme::Glass &&
           !HasActiveCustomBackground(state);
}

void UpdateCustomBackgroundLuminance(RunDialogState* state) {
    if (!state) return;
    const BYTE* bgPixels = nullptr;
    if (state->pVideoReader && !state->videoFrameBuffer.empty()) {
        bgPixels = state->videoFrameBuffer.data();
    } else if (!state->bgFrames.empty()) {
        size_t idx = state->bgCurrentFrame % state->bgFrames.size();
        bgPixels = state->bgFrames[idx].pixels.data();
    }
    if (bgPixels && state->bgFrameW > 0 && state->bgFrameH > 0) {
        bool newLight = SampleCustomBgPixelsIsLight(
            bgPixels, state->bgFrameW, state->bgFrameH, state->customBgLumValid, state->customBgIsLight);
        state->customBgIsLight = newLight;
        state->customBgLumValid = true;
    } else {
        state->customBgLumValid = false;
    }
}

int GetVisibleRowCount(const RunDialogState* state) {
    if (!state) return 0;
    int totalRows = (int)GetActiveRowItems(state).size();
    int maxVis = std::clamp(state->settings.visibleHistoryRows, 3, 10);
    return (std::min)(totalRows, maxVis);
}

void ClampScrollOffset(RunDialogState* state) {
    if (!state) return;
    int totalRows = (int)GetActiveRowItems(state).size();
    int visRows = GetVisibleRowCount(state);
    int maxScroll = (std::max)(0, totalRows - visRows);
    state->scrollOffset = std::clamp(state->scrollOffset, 0, maxScroll);
}

void EnsureRowVisible(RunDialogState* state, int rowIdx) {
    if (!state || rowIdx < 0) return;
    int visRows = GetVisibleRowCount(state);
    if (visRows <= 0) return;
    if (rowIdx < state->scrollOffset) {
        state->scrollOffset = rowIdx;
    } else if (rowIdx >= state->scrollOffset + visRows) {
        state->scrollOffset = rowIdx - visRows + 1;
    }
    ClampScrollOffset(state);
}

// --- Custom Background Media Engine (WIC Static/Animated GIF + Media Foundation MP4/WebM) ---

void ReleaseCustomBackgroundMedia(RunDialogState* state) {
    if (!state) return;
    if (state->hDlg && IsWindow(state->hDlg)) {
        KillTimer(state->hDlg, kTimerBgMedia);
    }
    if (state->pVideoReader) {
        state->pVideoReader->Release();
        state->pVideoReader = nullptr;
    }
    if (state->mfStarted) {
        MFShutdown();
        state->mfStarted = false;
    }
    state->bgFrames.clear();
    state->bgFrames.shrink_to_fit();
    state->videoFrameBuffer.clear();
    state->videoFrameBuffer.shrink_to_fit();
    state->bgFrameW = 0;
    state->bgFrameH = 0;
    state->bgCurrentFrame = 0;
    state->customBgLumValid = false;
    state->loadedBgPath.clear();
}

bool DecodeNextVideoFrame(RunDialogState* state) {
    if (!state || !state->pVideoReader || state->bgFrameW == 0 || state->bgFrameH == 0) {
        return false;
    }

    for (int attempt = 0; attempt < 2; ++attempt) {
        DWORD streamIndex = 0;
        DWORD flags = 0;
        LONGLONG llTimeStamp = 0;
        IMFSample* pSample = nullptr;

        HRESULT hr = state->pVideoReader->ReadSample(
            (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0, &streamIndex, &flags, &llTimeStamp, &pSample);

        if (FAILED(hr)) {
            if (pSample) pSample->Release();
            return false;
        }

        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
            if (pSample) pSample->Release();
            PROPVARIANT varZero;
            PropVariantInit(&varZero);
            varZero.vt = VT_I8;
            varZero.hVal.QuadPart = 0;
            static constexpr GUID kGuidNull = {};
            state->pVideoReader->SetCurrentPosition(kGuidNull, varZero);
            PropVariantClear(&varZero);
            continue;
        }

        if (pSample) {
            IMFMediaBuffer* pBuffer = nullptr;
            if (SUCCEEDED(pSample->ConvertToContiguousBuffer(&pBuffer)) && pBuffer) {
                BYTE* pData = nullptr;
                DWORD cbMaxLen = 0, cbCurLen = 0;
                if (SUCCEEDED(pBuffer->Lock(&pData, &cbMaxLen, &cbCurLen)) && pData) {
                    size_t rowBytes = (size_t)state->bgFrameW * 4;
                    size_t requiredBytes = rowBytes * (size_t)state->bgFrameH;
                    if (state->videoFrameBuffer.size() != requiredBytes) {
                        state->videoFrameBuffer.resize(requiredBytes);
                    }
                    IMF2DBuffer* p2DBuffer = nullptr;
                    if (SUCCEEDED(pBuffer->QueryInterface(__uuidof(IMF2DBuffer), (void**)&p2DBuffer)) && p2DBuffer) {
                        p2DBuffer->ContiguousCopyTo(state->videoFrameBuffer.data(), (DWORD)requiredBytes);
                        p2DBuffer->Release();
                    } else if (cbCurLen >= requiredBytes) {
                        std::memcpy(state->videoFrameBuffer.data(), pData, requiredBytes);
                    }
                    // Force alpha channel to 255 in case MF RGB32 decoder leaves alpha = 0
                    BYTE* dst = state->videoFrameBuffer.data();
                    for (size_t px = 3; px < requiredBytes; px += 4) {
                        dst[px] = 255;
                    }
                    pBuffer->Unlock();
                }
                pBuffer->Release();
            }
            pSample->Release();
            return !state->videoFrameBuffer.empty();
        }
    }
    return false;
}

void ReloadCustomBackgroundMedia(RunDialogState* state, bool showErrorIfTooLarge) {
    if (!state) return;

    std::wstring path = state->settings.customBackgroundPath;
    if (path == state->loadedBgPath &&
        (!state->bgFrames.empty() || state->pVideoReader != nullptr || path.empty())) {
        return;
    }

    ReleaseCustomBackgroundMedia(state);
    if (path.empty()) {
        if (state->hDlg && IsWindow(state->hDlg)) {
            ApplyWindowAppearance(state->hDlg, state);
            InvalidateRect(state->hDlg, nullptr, TRUE);
        }
        return;
    }

    wchar_t expandedPath[MAX_PATH] = {};
    if (ExpandEnvironmentStringsW(path.c_str(), expandedPath, MAX_PATH) > 0 && expandedPath[0]) {
        path = expandedPath;
    }

    WIN32_FILE_ATTRIBUTE_DATA fad = {};
    if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fad) ||
        (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return;
    }

    ULARGE_INTEGER fileSize = {};
    fileSize.LowPart = fad.nFileSizeLow;
    fileSize.HighPart = fad.nFileSizeHigh;
    ULONGLONG maxBytes = (ULONGLONG)std::clamp(state->settings.customBackgroundMaxMb, 20, 50) * 1024ULL * 1024ULL;
    if (fileSize.QuadPart == 0 || fileSize.QuadPart > maxBytes) {
        if (showErrorIfTooLarge && state->hDlg && IsWindow(state->hDlg)) {
            MenuLocalizedStrings ml = GetMenuLocalizedStrings(state->settings.language);
            wchar_t msg[320] = {};
            swprintf_s(msg, ml.bgSizeLimitFmt,
                       state->settings.customBackgroundMaxMb, state->settings.customBackgroundMaxMb);
            MessageBoxW(state->hDlg, msg, L"Modern Run Dialog UI", MB_ICONWARNING | MB_OK);
        }
        return;
    }

    const wchar_t* ext = PathFindExtensionW(path.c_str());
    bool isVideoFile = ext && (_wcsicmp(ext, L".mp4") == 0 || _wcsicmp(ext, L".webm") == 0 ||
                               _wcsicmp(ext, L".mkv") == 0 || _wcsicmp(ext, L".mov") == 0 ||
                               _wcsicmp(ext, L".wmv") == 0);

    UINT targetW = (UINT)std::clamp(GetDlgWidth(state), 320, 960);
    UINT targetH = (UINT)std::clamp((std::max)(state->totalHeight, DpiScaleS(260, state)), 180, 600);

    if (isVideoFile) {
        if (SUCCEEDED(MFStartup(MF_VERSION, MFSTARTUP_LITE))) {
            state->mfStarted = true;
            IMFAttributes* pAttr = nullptr;
            if (SUCCEEDED(MFCreateAttributes(&pAttr, 2)) && pAttr) {
                pAttr->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
                pAttr->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
            }
            IMFSourceReader* pReader = nullptr;
            if (SUCCEEDED(MFCreateSourceReaderFromURL(path.c_str(), pAttr, &pReader)) && pReader) {
                IMFMediaType* pType = nullptr;
                if (SUCCEEDED(MFCreateMediaType(&pType)) && pType) {
                    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
                    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
                    if (SUCCEEDED(pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pType))) {
                        IMFMediaType* pActual = nullptr;
                        if (SUCCEEDED(pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pActual)) && pActual) {
                            UINT32 vw = 0, vh = 0;
                            MFGetAttributeSize(pActual, MF_MT_FRAME_SIZE, &vw, &vh);
                            UINT32 fpsNum = 30, fpsDen = 1;
                            MFGetAttributeRatio(pActual, MF_MT_FRAME_RATE, &fpsNum, &fpsDen);
                            if (fpsNum > 0 && fpsDen > 0) {
                                UINT interval = (UINT)((1000ULL * fpsDen) / fpsNum);
                                state->videoIntervalMs = std::clamp(interval, 16u, 100u);
                            } else {
                                state->videoIntervalMs = 33;
                            }
                            pActual->Release();
                            if (vw > 0 && vh > 0 && vw <= 3840 && vh <= 2160) {
                                state->bgFrameW = vw;
                                state->bgFrameH = vh;
                                state->pVideoReader = pReader;
                                pReader = nullptr;
                                state->loadedBgPath = state->settings.customBackgroundPath;
                                DecodeNextVideoFrame(state);
                                UpdateCustomBackgroundLuminance(state);
                                if (state->hDlg && IsWindow(state->hDlg)) {
                                    ApplyWindowAppearance(state->hDlg, state);
                                    SetTimer(state->hDlg, kTimerBgMedia, state->videoIntervalMs, nullptr);
                                }
                            }
                        }
                    }
                    pType->Release();
                }
                if (pReader) pReader->Release();
            }
            if (pAttr) pAttr->Release();
            if (!state->pVideoReader) {
                MFShutdown();
                state->mfStarted = false;
            }
        }
        return;
    }

    // Image or Animated GIF via Windows Imaging Component (WIC)
    IWICImagingFactory* pWic = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&pWic))) || !pWic) {
        return;
    }

    IWICBitmapDecoder* pDecoder = nullptr;
    if (SUCCEEDED(pWic->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
                                                  WICDecodeMetadataCacheOnDemand, &pDecoder)) && pDecoder) {
        UINT frameCount = 0;
        pDecoder->GetFrameCount(&frameCount);
        frameCount = (std::min)(frameCount, 90u); // Cap animated GIF frames in RAM

        for (UINT idx = 0; idx < frameCount; ++idx) {
            IWICBitmapFrameDecode* pFrame = nullptr;
            if (FAILED(pDecoder->GetFrame(idx, &pFrame)) || !pFrame) continue;

            UINT fw = 0, fh = 0;
            pFrame->GetSize(&fw, &fh);
            if (fw == 0 || fh == 0) {
                pFrame->Release();
                continue;
            }

            if (state->bgFrameW == 0 || state->bgFrameH == 0) {
                // Scale down high-res images to fit dialog smoothly
                float scale = (std::min)(1.0f, (std::max)((float)targetW / fw, (float)targetH / fh));
                state->bgFrameW = (std::max)(1u, (UINT)std::round(fw * scale));
                state->bgFrameH = (std::max)(1u, (UINT)std::round(fh * scale));
            }

            UINT delayMs = 80;
            IWICMetadataQueryReader* pMeta = nullptr;
            if (SUCCEEDED(pFrame->GetMetadataQueryReader(&pMeta)) && pMeta) {
                PROPVARIANT propVal;
                PropVariantInit(&propVal);
                if (SUCCEEDED(pMeta->GetMetadataByName(L"/grctlext/Delay", &propVal))) {
                    if (propVal.vt == VT_UI2) {
                        delayMs = (std::max)(20u, (UINT)propVal.uiVal * 10u);
                    }
                }
                PropVariantClear(&propVal);
                pMeta->Release();
            }

            IWICBitmapScaler* pScaler = nullptr;
            if (SUCCEEDED(pWic->CreateBitmapScaler(&pScaler)) && pScaler) {
                if (SUCCEEDED(pScaler->Initialize(pFrame, state->bgFrameW, state->bgFrameH,
                                                  WICBitmapInterpolationModeFant))) {
                    IWICFormatConverter* pConv = nullptr;
                    if (SUCCEEDED(pWic->CreateFormatConverter(&pConv)) && pConv) {
                        if (SUCCEEDED(pConv->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA,
                                                        WICBitmapDitherTypeNone, nullptr, 0.0,
                                                        WICBitmapPaletteTypeCustom))) {
                            BgAnimFrame animFrame;
                            animFrame.delayMs = delayMs;
                            UINT stride = state->bgFrameW * 4;
                            UINT bufSize = stride * state->bgFrameH;
                            animFrame.pixels.resize(bufSize);
                            if (SUCCEEDED(pConv->CopyPixels(nullptr, stride, bufSize, animFrame.pixels.data()))) {
                                state->bgFrames.push_back(std::move(animFrame));
                            }
                        }
                        pConv->Release();
                    }
                }
                pScaler->Release();
            }
            pFrame->Release();
        }
        pDecoder->Release();
    }
    pWic->Release();

    if (!state->bgFrames.empty()) {
        state->loadedBgPath = state->settings.customBackgroundPath;
        state->bgCurrentFrame = 0;
        UpdateCustomBackgroundLuminance(state);
        if (state->hDlg && IsWindow(state->hDlg)) {
            ApplyWindowAppearance(state->hDlg, state);
        }
        if (state->bgFrames.size() > 1 && state->hDlg && IsWindow(state->hDlg)) {
            SetTimer(state->hDlg, kTimerBgMedia, state->bgFrames[0].delayMs, nullptr);
        }
    }
}

void StepCustomBackgroundAnimation(RunDialogState* state) {
    if (!state || !state->hDlg || IsIconic(state->hDlg)) return;
    if (state->pVideoReader) {
        if (DecodeNextVideoFrame(state)) {
            bool prevLight = state->customBgIsLight;
            UpdateCustomBackgroundLuminance(state);
            if (state->customBgIsLight != prevLight) {
                state->isLightMode = ResolveDialogLightContrast(state);
            }
            InvalidateRect(state->hDlg, nullptr, FALSE);
        }
    } else if (state->bgFrames.size() > 1) {
        state->bgCurrentFrame = (state->bgCurrentFrame + 1) % state->bgFrames.size();
        bool prevLight = state->customBgIsLight;
        UpdateCustomBackgroundLuminance(state);
        if (state->customBgIsLight != prevLight) {
            state->isLightMode = ResolveDialogLightContrast(state);
        }
        SetTimer(state->hDlg, kTimerBgMedia, state->bgFrames[state->bgCurrentFrame].delayMs, nullptr);
        InvalidateRect(state->hDlg, nullptr, FALSE);
    }
}

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

void ResizeDialogPreservingBottomAnchor(RunDialogState* state, int oldHeight) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    int dlgW = GetDlgWidth(state);
    RECT rcCur = {};
    GetWindowRect(state->hDlg, &rcCur);
    int curW = rcCur.right - rcCur.left;
    int curH = rcCur.bottom - rcCur.top;
    if (oldHeight == state->totalHeight && curW == dlgW && curH == state->totalHeight) return;

    state->normalY += (oldHeight - state->totalHeight);
    ClampDialogToMonitorWorkArea(state->hDlg, &state->normalX, &state->normalY, dlgW, state->totalHeight, state->dpi);
    if (!IsIconic(state->hDlg)) {
        SetWindowPos(state->hDlg, nullptr, state->normalX, state->normalY, dlgW, state->totalHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}

void MinimizeRunDialog(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    CloseActiveGearMenu();
    ULONGLONG now = GetTickCount64();
    if (now - g_lastToggleTick.load() < 250) return;

    if (state->settings.taskbarCloseInsteadOfMinimize) {
        CloseRunDialog(state->hDlg);
        return;
    }

    g_dlgIsForeground.store(false);
    g_deactivatedByTaskbarTick.store(0);

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
    if (NeedsBackdropSnapshot(state)) {
        CaptureCleanMonitorBackdrop(state->hDlg, false);
        state->backdropLightSample = SampleBackdropLuminanceFromSnap(
            state->normalX, state->normalY, dlgW, state->totalHeight, state->backdropLightSample);
        state->lastTopologySig = GetBackdropWindowTopologySignature(state->hDlg);
    }
    state->pendingTopologyRecapture = false;

    g_internalActivation = true;
    if (IsIconic(state->hDlg)) {
        if (ShowWindow_Orig) {
            ShowWindow_Orig(state->hDlg, SW_RESTORE);
        } else {
            ShowWindow(state->hDlg, SW_RESTORE);
        }
    }

    LONG_PTR style = GetWindowLongPtrW(state->hDlg, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_DLGFRAME | WS_BORDER);
    style |= (WS_SYSMENU | WS_MINIMIZEBOX);
    SetWindowLongPtrW(state->hDlg, GWL_STYLE, style);

    SetWindowPos(state->hDlg, HWND_TOPMOST,
                 state->normalX, state->normalY,
                 dlgW, state->totalHeight,
                 SWP_SHOWWINDOW | SWP_FRAMECHANGED);
    SetWindowPos(state->hDlg, HWND_NOTOPMOST,
                 state->normalX, state->normalY,
                 dlgW, state->totalHeight,
                 SWP_SHOWWINDOW);

    ApplyWindowAppearance(state->hDlg, state);

    if (SwitchToThisWindow_Orig) {
        SwitchToThisWindow_Orig(state->hDlg, TRUE);
    }
    if (SetForegroundWindow_Orig) {
        SetForegroundWindow_Orig(state->hDlg);
    } else {
        SetForegroundWindow(state->hDlg);
    }

    g_dlgIsForeground.store(true);
    g_deactivatedByTaskbarTick.store(0);

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

        state->suppressSuggestionUpdate = true;
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            SetWindowTextW(state->hModernEdit, formattedPath.c_str());
            SendMessageW(state->hModernEdit, EM_SETSEL, formattedPath.size(), formattedPath.size());
        }
        state->suppressSuggestionUpdate = false;
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

void LoadHistoryItems(RunDialogState* state);
ActionButtonsLayout GetActionButtonsLayout(const RunDialogState* state);
std::wstring GetEffectiveInputText(HWND hEdit);

void OpenBrowseCustomBackgroundDialog(RunDialogState* state) {
    if (!state || !state->hDlg || state->isBrowsingFile || state->pendingUninit || g_unloading.load()) return;
    HWND hDlg = state->hDlg;
    state->isBrowsingFile = true;
    g_isBrowsingFile.store(true);

    MenuLocalizedStrings ml = GetMenuLocalizedStrings(state->settings.language);

    wchar_t filePath[MAX_PATH] = {};
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hDlg;
    ofn.lpstrFilter = ml.bgBrowseFilter;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = ml.bgBrowseTitle;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;

    BOOL picked = GetOpenFileNameW(&ofn);

    if (g_hActiveRunDlg.load() != hDlg || !IsWindow(hDlg)) {
        g_isBrowsingFile.store(false);
        return;
    }

    if (state->pendingUninit || g_unloading.load()) {
        g_isBrowsingFile.store(false);
        state->isBrowsingFile = false;
        return;
    }

    g_isBrowsingFile.store(false);
    state->isBrowsingFile = false;

    if (picked && filePath[0] != L'\0') {
        WIN32_FILE_ATTRIBUTE_DATA fad = {};
        if (GetFileAttributesExW(filePath, GetFileExInfoStandard, &fad)) {
            ULARGE_INTEGER fileSize = {};
            fileSize.LowPart = fad.nFileSizeLow;
            fileSize.HighPart = fad.nFileSizeHigh;
            int maxMb = std::clamp(state->settings.customBackgroundMaxMb, 20, 50);
            ULONGLONG maxBytes = (ULONGLONG)maxMb * 1024ULL * 1024ULL;
            if (fileSize.QuadPart > maxBytes) {
                wchar_t msg[320] = {};
                swprintf_s(msg, ml.bgSizeLimitFmt, maxMb, maxMb);
                MessageBoxW(hDlg, msg, L"Modern Run Dialog UI", MB_ICONWARNING | MB_OK);
            } else {
                Wh_SetStringValue(GetUserOverrideKey(L"customBackgroundPath").c_str(), filePath);
                LoadSettings(false);
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    state->settings = g_settings;
                }
                ReloadCustomBackgroundMedia(state, true);
            }
        }
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
    CloseActiveGearMenu();
    if (!hDlg || !IsWindow(hDlg)) return;
    HWND expected = hDlg;
    g_hActiveRunDlg.compare_exchange_strong(expected, nullptr);
    HWND hCancel = GetDlgItem(hDlg, IDCANCEL);
    if (hCancel) EnableWindow(hCancel, TRUE);
    if (GetWindowThreadProcessId(hDlg, nullptr) == GetCurrentThreadId()) {
        SendMessageW(hDlg, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)hCancel);
        if (IsWindow(hDlg)) {
            EndDialog(hDlg, IDCANCEL);
            PostMessageW(hDlg, WM_NULL, 0, 0);
        }
    } else {
        PostMessageW(hDlg, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)hCancel);
        PostMessageW(hDlg, WM_CLOSE, 0, 0);
        PostMessageW(hDlg, WM_NULL, 0, 0);
    }
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

std::wstring UrlEncodeUtf8(std::wstring_view input) {
    if (input.empty()) return L"";
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, input.data(), (int)input.size(), nullptr, 0, nullptr, nullptr);
    if (utf8Len <= 0) return L"";
    std::string utf8(utf8Len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, input.data(), (int)input.size(), utf8.data(), utf8Len, nullptr, nullptr);

    std::wstring encoded;
    encoded.reserve(utf8.size() * 3);
    static constexpr wchar_t kHex[] = L"0123456789ABCDEF";
    for (unsigned char c : utf8) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            encoded.push_back((wchar_t)c);
        } else if (c == ' ') {
            encoded.push_back(L'+');
        } else {
            encoded.push_back(L'%');
            encoded.push_back(kHex[(c >> 4) & 0xF]);
            encoded.push_back(kHex[c & 0xF]);
        }
    }
    return encoded;
}

// Parses "ggsearch:<query>" (returns 1) or "msesearch:<query>" (returns 2)
int ParseWebSearchCommand(std::wstring_view rawCmd, std::wstring* outQuery) {
    std::wstring trimmed = TrimCommandString(std::wstring(rawCmd));
    std::wstring lower = ToLowerString(trimmed);

    if (lower.rfind(L"ggsearch:", 0) == 0) {
        if (outQuery) {
            *outQuery = TrimCommandString(trimmed.substr(9));
        }
        return 1;
    }
    if (lower.rfind(L"msesearch:", 0) == 0) {
        if (outQuery) {
            *outQuery = TrimCommandString(trimmed.substr(10));
        }
        return 2;
    }
    return 0;
}

bool LaunchWebSearchCommand(HWND, int searchKind, const std::wstring& query) {
    std::wstring encoded = UrlEncodeUtf8(query);
    std::wstring url = encoded.empty()
        ? L"https://www.google.com"
        : (L"https://www.google.com/search?q=" + encoded);

    const wchar_t* browserExe = (searchKind == 1) ? L"chrome.exe" : L"msedge.exe";

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_NO_UI;
    sei.hwnd = nullptr;
    sei.lpVerb = L"open";
    sei.lpFile = browserExe;
    sei.lpParameters = url.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (ShellExecuteExW(&sei)) {
        return true;
    }

    // Fallback to default system browser if chrome.exe or msedge.exe is not installed
    SHELLEXECUTEINFOW seiFallback = { sizeof(seiFallback) };
    seiFallback.fMask = SEE_MASK_DOENVSUBST;
    seiFallback.hwnd = nullptr;
    seiFallback.lpVerb = L"open";
    seiFallback.lpFile = url.c_str();
    seiFallback.nShow = SW_SHOWNORMAL;
    return ShellExecuteExW(&seiFallback) != FALSE;
}

void ExecuteHistoryOrSuggestionItem(RunDialogState* state, const HistoryItem& item) {
    if (!state || !state->hDlg || state->isTriggeringRun) return;
    CloseActiveGearMenu();

    // Handle ggsearch:<keyword> (Google Chrome) and msesearch:<keyword> (Microsoft Edge)
    std::wstring parsedSearchQuery = item.webSearchQuery;
    int searchKind = item.webSearchKind;
    if (searchKind == 0) {
        searchKind = ParseWebSearchCommand(!item.execTarget.empty() ? item.execTarget : item.cmd, &parsedSearchQuery);
    }
    if (searchKind != 0) {
        std::wstring recordCmd = !item.cmd.empty() ? item.cmd : item.execTarget;
        SaveCommandTimestamp(recordCmd);
        if (LaunchWebSearchCommand( nullptr, searchKind, parsedSearchQuery)) {
            CloseRunDialog(state->hDlg);
            return;
        }
    }

    // For Start Menu shortcut files (.lnk / .appref-ms), launch directly via ShellExecuteExW
    // with hwnd = nullptr so the launched process is never owned by or tied to the closing Run dialog.
    if (item.isShortcutFile && !item.execTarget.empty()) {
        SaveCommandTimestamp(item.cmd);
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_DOENVSUBST;
        sei.hwnd = nullptr;
        sei.lpVerb = nullptr;
        sei.lpFile = item.execTarget.c_str();
        sei.nShow = SW_SHOWNORMAL;
        if (ShellExecuteExW(&sei)) {
            CloseRunDialog(state->hDlg);
            return;
        }
    }

    const std::wstring& targetCmd = !item.execTarget.empty() ? item.execTarget : item.cmd;
    state->isTriggeringRun = true;

    wchar_t rawBuf[MAX_PATH] = {};
    wcsncpy_s(rawBuf, targetCmd.c_str(), _TRUNCATE);
    PathRemoveBlanksW(rawBuf);

    if (rawBuf[0] == L'\0') {
        state->isTriggeringRun = false;
        return;
    }

    SaveCommandTimestamp(item.cmd.empty() ? rawBuf : item.cmd);

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

    if (IsWindow(hDlg)) {
        if (!IsWindowVisible(hDlg) && !IsIconic(hDlg)) {
            HWND expected = hDlg;
            g_hActiveRunDlg.compare_exchange_strong(expected, nullptr);
            EndDialog(hDlg, IDOK);
            PostMessageW(hDlg, WM_NULL, 0, 0);
        } else if (g_hActiveRunDlg.load() == hDlg) {
            state->isTriggeringRun = false;
        }
    }
}

void ExecuteCommandImmediate(RunDialogState* state, const std::wstring& rawCommand) {
    HistoryItem tempItem;
    tempItem.cmd = rawCommand;
    tempItem.execTarget = rawCommand;
    ExecuteHistoryOrSuggestionItem(state, tempItem);
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

    const auto& activeRows = GetActiveRowItems(state);
    if (state->showingSuggestions && state->selectedRow >= 0 && state->selectedRow < (int)activeRows.size()) {
        HistoryItem chosen = activeRows[state->selectedRow];
        ExecuteHistoryOrSuggestionItem(state, chosen);
        return;
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
    // When a custom background image/GIF/video is active, text color always adapts
    // to the background media's luminance rather than the default Windows theme.
    if (state && HasActiveCustomBackground(state) && state->customBgLumValid) {
        return state->customBgIsLight;
    }
    if (state->settings.colorMode == ColorMode::Light) return true;
    if (state->settings.colorMode == ColorMode::Dark) return false;
    if (state->effectiveSurface == SurfaceTheme::Glass) {
        return state ? state->backdropLightSample : false;
    }
    return IsSystemAppLightTheme();
}

void ApplyWindowAppearance(HWND hDlg, RunDialogState* state) {
    if (!hDlg || !state) return;

    state->effectiveSurface = ResolveEffectiveSurface(state->settings);
    if (!NeedsBackdropSnapshot(state)) {
        ReleaseBackdropSnapshot();
    }
    state->isLightMode = ResolveDialogLightContrast(state);
    bool hasCustomBg = HasActiveCustomBackground(state);

    // Always keep DWMWA_USE_IMMERSIVE_DARK_MODE = TRUE when Liquid Glass is active so Windows 11 DWM
    // never injects the opaque milky-white Light-Mode Acrylic exclusion fog!
    BOOL dark = (!hasCustomBg && state->effectiveSurface == SurfaceTheme::Glass)
                ? TRUE
                : (state->isLightMode ? FALSE : TRUE);
    DwmSetWindowAttribute(hDlg, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    DWORD corner = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(hDlg, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    COLORREF borderCol = (!hasCustomBg && state->effectiveSurface == SurfaceTheme::Glass)
        ? 0xFFFFFFFE // DWMWA_COLOR_NONE (Liquid Glass draws its own dual refractive rim)
        : (state->isLightMode ? RGB(204, 208, 216) : RGB(64, 68, 76));
    DwmSetWindowAttribute(hDlg, DWMWA_BORDER_COLOR, &borderCol, sizeof(borderCol));

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return;

    auto pSetWCA = reinterpret_cast<SetWindowCompositionAttribute_t>(
        GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
    if (!pSetWCA) return;

    ACCENT_POLICY policy = {};
    if (hasCustomBg || state->effectiveSurface == SurfaceTheme::Solid || IsHighContrastActive()) {
        // When custom background is active (or Opaque theme), disable DWM blur/glass completely
        // so custom background never mixes with Liquid Glass or Windows Blur!
        policy.AccentState = ACCENT_DISABLED;
    } else if (state->effectiveSurface == SurfaceTheme::Glass) {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        int glassPct = std::clamp(state->settings.glassOpacity, -100, 100);
        DWORD alpha = (glassPct >= 0) ? (DWORD)std::clamp(MulDiv(glassPct, 180, 100), 4, 215) : 2;
        DWORD bgr   = state->isLightMode ? 0x00F4F2F0 : 0x001E1A16;
        policy.GradientColor = (alpha << 24) | bgr;
    } else if (state->effectiveSurface == SurfaceTheme::Blur) {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        DWORD alpha = static_cast<DWORD>(state->settings.blurOpacity) & 0xFF;
        DWORD bgr   = state->isLightMode ? 0x00F6F3F2 : 0x00241E1C;
        policy.GradientColor = (alpha << 24) | bgr;
    }

    // Ensure WS_EX_LAYERED is never left active on the dialog so DWM blur and text/button opacity stay 100% intact
    LONG_PTR exStyle = GetWindowLongPtrW(hDlg, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) {
        SetLayeredWindowAttributes(hDlg, 0, 255, LWA_ALPHA);
        SetWindowLongPtrW(hDlg, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
    }

    WINDOWCOMPOSITIONATTRIBDATA data = {};
    data.Attrib = 19;
    data.pvData = &policy;
    data.cbData = sizeof(policy);
    pSetWCA(hDlg, &data);
}

void CheckAndUpdateBackdropInRealtime(HWND hWnd, RunDialogState* state, bool forceImmediate) {
    if (!state || !hWnd || !IsWindow(hWnd) || !IsWindowVisible(hWnd) || IsIconic(hWnd)) return;
    if (state->effectiveSurface != SurfaceTheme::Glass || HasActiveCustomBackground(state)) return;
    // Never recapture while Gear menu, Custom Size window, File Browse dialog, or mouse drag is active
    if (state->isBrowsingFile || state->windowDragging || state->scrollbarDragging ||
        g_hActiveGearMenu.load() != nullptr || g_hActiveCustomSizeWnd.load() != nullptr) {
        return;
    }
    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 || (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0) {
        return;
    }

    uint64_t curSig = GetBackdropWindowTopologySignature(hWnd);
    ULONGLONG now = GetTickCount64();

    if (forceImmediate) {
        state->lastTopologySig = curSig;
        state->pendingTopologyRecapture = true;
        state->topologyChangeTick = now;
        return;
    }

    if (curSig != state->lastTopologySig) {
        state->lastTopologySig = curSig;
        state->pendingTopologyRecapture = true;
        state->topologyChangeTick = now;
        return;
    }

    if (!state->pendingTopologyRecapture) {
        return;
    }

    // Wait ~180 ms after the window topology change settles so Windows 11 DWM minimize/restore animations finish
    if (now - state->topologyChangeTick < 180) {
        return;
    }

    state->pendingTopologyRecapture = false;
    CaptureCleanMonitorBackdrop(hWnd, true);
    state->lastTopologySig = GetBackdropWindowTopologySignature(hWnd);

    int dlgW = GetDlgWidth(state);
    bool newLight = SampleBackdropLuminanceFromSnap(
        state->normalX, state->normalY, dlgW, state->totalHeight, state->backdropLightSample);

    bool changed = (newLight != state->backdropLightSample);
    state->backdropLightSample = newLight;
    bool prevMode = state->isLightMode;
    state->isLightMode = ResolveDialogLightContrast(state);

    if (changed || state->isLightMode != prevMode) {
        ApplyWindowAppearance(hWnd, state);
    }
    InvalidateRect(hWnd, nullptr, FALSE);
}


struct BuiltinCommandEntry {
    const wchar_t* key;
    const wchar_t* name;
};

static constexpr BuiltinCommandEntry kBuiltinCommands[] = {
    { L"ggsearch:",        L"Google Search in Google Chrome" },
    { L"msesearch:",       L"Web Search in Microsoft Edge" },
    { L"winver",           L"Windows Version" },
    { L"cmd",              L"Command Prompt" },
    { L"powershell",       L"Windows PowerShell" },
    { L"pwsh",             L"PowerShell" },
    { L"wt",               L"Windows Terminal" },
    { L"notepad",          L"Notepad" },
    { L"regedit",          L"Registry Editor" },
    { L"calc",             L"Calculator" },
    { L"mspaint",          L"Paint" },
    { L"taskmgr",          L"Task Manager" },
    { L"msconfig",         L"System Configuration" },
    { L"control",          L"Control Panel" },
    { L"explorer",         L"File Explorer" },
    { L"mstsc",            L"Remote Desktop Connection" },
    { L"cleanmgr",         L"Disk Cleanup" },
    { L"dxdiag",           L"DirectX Diagnostic Tool" },
    { L"charmap",          L"Character Map" },
    { L"magnify",          L"Magnifier" },
    { L"osk",              L"On-Screen Keyboard" },
    { L"snippingtool",     L"Snipping Tool" },
    { L"winword",          L"Microsoft Word" },
    { L"excel",            L"Microsoft Excel" },
    { L"powerpnt",         L"Microsoft PowerPoint" },
    { L"msedge",           L"Microsoft Edge" },
    { L"chrome",           L"Google Chrome" },
    { L"firefox",          L"Mozilla Firefox" },
    { L"code",             L"Visual Studio Code" },
    { L"sysdm.cpl",        L"System Properties" },
    { L"main.cpl",         L"Mouse Properties" },
    { L"ncpa.cpl",         L"Network Connections" },
    { L"appwiz.cpl",       L"Programs and Features" },
    { L"mmsys.cpl",        L"Sound" },
    { L"timedate.cpl",     L"Date and Time" },
    { L"desk.cpl",         L"Display Settings" },
    { L"inetcpl.cpl",      L"Internet Properties" },
    { L"powercfg.cpl",     L"Power Options" },
    { L"firewall.cpl",     L"Windows Defender Firewall" },
    { L"colorcpl",         L"Color Management" },
    { L"services.msc",     L"Services" },
    { L"devmgmt.msc",      L"Device Manager" },
    { L"diskmgmt.msc",     L"Disk Management" },
    { L"eventvwr.msc",     L"Event Viewer" },
    { L"gpedit.msc",       L"Local Group Policy Editor" },
    { L"lusrmgr.msc",      L"Local Users and Groups" },
    { L"perfmon.msc",      L"Performance Monitor" },
    { L"resmon",           L"Resource Monitor" },
    { L"secpol.msc",       L"Local Security Policy" },
    { L"taskschd.msc",     L"Task Scheduler" },
    { L"sndvol",           L"Volume Mixer" },
    { L"mdsched",          L"Windows Memory Diagnostic" },
    { L"mrt",              L"Malicious Software Removal Tool" },
    { L"optionalfeatures", L"Windows Features" },
    { L"shrpubw",          L"Create Shared Folder Wizard" },
    { L"sigverif",         L"File Signature Verification" },
    { L"utilman",          L"Ease of Access" },
    { L"verifier",         L"Driver Verifier Manager" },
    { L"wmimgmt.msc",      L"WMI Management" },
    { L"wsreset",          L"Microsoft Store Reset" },
    { L"write",            L"WordPad" }
};

std::wstring GetFriendlyBuiltinName(const std::wstring& baseLower) {
    for (const auto& entry : kBuiltinCommands) {
        if (baseLower == entry.key) return entry.name;
    }
    if (baseLower.size() > 4 && baseLower.substr(baseLower.size() - 4) == L".exe") {
        std::wstring noExe = baseLower.substr(0, baseLower.size() - 4);
        for (const auto& entry : kBuiltinCommands) {
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

    std::wstring webQuery;
    int webSearchKind = ParseWebSearchCommand(rawCmd, &webQuery);
    if (webSearchKind != 0) {
        std::wstring browserCmd = (webSearchKind == 1) ? L"chrome.exe" : L"msedge.exe";
        std::wstring dummyName;
        ResolveCommandMetadata(browserCmd, outIcon, &dummyName);
        if (webSearchKind == 1) {
            *outAppName = webQuery.empty() ? L"Google Search (Google Chrome)" : (L"Google Search (Chrome): " + webQuery);
        } else {
            *outAppName = webQuery.empty() ? L"Web Search (Microsoft Edge)" : (L"Web Search (Edge): " + webQuery);
        }
        return;
    }

    wchar_t cmdOnly[MAX_PATH] = {};
    wcsncpy_s(cmdOnly, rawCmd.c_str(), _TRUNCATE);
    PathRemoveBlanksW(cmdOnly);
    PathRemoveArgsW(cmdOnly);
    PathUnquoteSpacesW(cmdOnly);

    if (cmdOnly[0] == L'\0') return;

    std::wstring baseName = PathFindFileNameW(cmdOnly);
    std::wstring baseLower = ToLowerString(baseName);

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

// --- PowerToys Run-Style App & Command Suggestion Catalog ---

struct AppSuggestionCandidate {
    std::wstring cmd;           // Short command or shortcut name shown in Semibold
    std::wstring execTarget;    // Command or full .lnk path to execute
    std::wstring subtitle;      // Friendly app name or folder category
    std::wstring iconSource;    // Path for lazy icon/description lookup
    std::wstring cmdLower;
    std::wstring subLower;
    HICON cachedIcon = nullptr;
    bool iconResolved = false;
    bool descResolved = false;
    bool isShortcutFile = false;
    bool isBuiltin = false;
};

std::mutex g_catalogMutex;
std::vector<AppSuggestionCandidate> g_appCatalog;
ULONGLONG g_catalogLastBuildTick = 0;

bool IsNoiseShortcutName(std::wstring_view nameLower) {
    static constexpr std::wstring_view kSkipTokens[] = {
        L"uninstall", L"uninst", L"gỡ cài đặt", L"readme", L"release notes",
        L"help", L"documentation", L"website", L"license", L"changelog"
    };
    for (auto tok : kSkipTokens) {
        if (nameLower.find(tok) != std::wstring_view::npos) {
            return true;
        }
    }
    return false;
}

void ScanShortcutDirectoryRecursive(const std::wstring& dirPath,
                                    const std::wstring& folderLabel,
                                    int depth,
                                    std::vector<AppSuggestionCandidate>& outList) {
    if (depth > 3 || outList.size() >= 450) return;

    std::wstring pattern = dirPath + L"\\*";
    WIN32_FIND_DATAW fd = {};
    HANDLE hFind = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &fd,
                                    FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        if (fd.cFileName[0] == L'.') continue;
        std::wstring fullPath = dirPath + L"\\" + fd.cFileName;

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!(fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
                ScanShortcutDirectoryRecursive(fullPath, fd.cFileName, depth + 1, outList);
            }
            continue;
        }

        const wchar_t* ext = PathFindExtensionW(fd.cFileName);
        if (!ext) continue;
        if (_wcsicmp(ext, L".lnk") != 0 && _wcsicmp(ext, L".appref-ms") != 0) {
            continue;
        }

        std::wstring title(fd.cFileName, ext - fd.cFileName);
        title = TrimCommandString(title);
        if (title.empty()) continue;

        std::wstring titleLower = ToLowerString(title);
        if (IsNoiseShortcutName(titleLower)) continue;

        bool exists = false;
        for (const auto& item : outList) {
            if (item.cmdLower == titleLower || item.subLower == titleLower) {
                exists = true;
                break;
            }
        }
        if (exists) continue;

        AppSuggestionCandidate cand;
        cand.cmd = title;
        cand.execTarget = fullPath;
        cand.subtitle = folderLabel;
        cand.iconSource = fullPath;
        cand.cmdLower = std::move(titleLower);
        cand.subLower = ToLowerString(folderLabel);
        cand.isShortcutFile = true;
        cand.descResolved = true;
        outList.push_back(std::move(cand));
    } while (FindNextFileW(hFind, &fd) && outList.size() < 450);

    FindClose(hFind);
}

void EnumerateAppPathsRegistry(HKEY hRootKey, std::vector<AppSuggestionCandidate>& outList) {
    HKEY hAppPaths = nullptr;
    if (RegOpenKeyExW(hRootKey, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths",
                      0, KEY_READ, &hAppPaths) != ERROR_SUCCESS) {
        return;
    }

    wchar_t subKeyName[MAX_PATH] = {};
    for (DWORD idx = 0; idx < 300; ++idx) {
        DWORD cchName = ARRAYSIZE(subKeyName);
        if (RegEnumKeyExW(hAppPaths, idx, subKeyName, &cchName, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
            break;
        }

        wchar_t targetPath[MAX_PATH] = {};
        DWORD cbTarget = sizeof(targetPath);
        if (RegGetValueW(hAppPaths, subKeyName, nullptr, RRF_RT_REG_SZ, nullptr, targetPath, &cbTarget) != ERROR_SUCCESS) {
            continue;
        }
        PathRemoveBlanksW(targetPath);
        PathUnquoteSpacesW(targetPath);
        if (targetPath[0] == L'\0') continue;

        wchar_t expandedPath[MAX_PATH] = {};
        if (ExpandEnvironmentStringsW(targetPath, expandedPath, MAX_PATH) > 0 && expandedPath[0] != L'\0') {
            wcsncpy_s(targetPath, expandedPath, _TRUNCATE);
        }
        if (!PathFileExistsW(targetPath)) continue;

        std::wstring cmdName = subKeyName;
        const wchar_t* ext = PathFindExtensionW(subKeyName);
        if (ext && _wcsicmp(ext, L".exe") == 0) {
            cmdName.assign(subKeyName, ext - subKeyName);
        }
        std::wstring cmdLower = ToLowerString(cmdName);
        if (cmdLower.empty() || IsNoiseShortcutName(cmdLower)) continue;

        bool duplicate = false;
        for (const auto& existing : outList) {
            if (existing.cmdLower == cmdLower) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) continue;

        AppSuggestionCandidate cand;
        cand.cmd = cmdName;
        cand.execTarget = cmdName;
        cand.subtitle = GetFriendlyBuiltinName(cmdLower);
        cand.iconSource = targetPath;
        cand.cmdLower = std::move(cmdLower);
        cand.subLower = ToLowerString(cand.subtitle);
        cand.isShortcutFile = false;
        cand.descResolved = !cand.subtitle.empty();
        outList.push_back(std::move(cand));
    }

    RegCloseKey(hAppPaths);
}

void EnsureAppCatalogPopulated() {
    std::lock_guard<std::mutex> lock(g_catalogMutex);
    ULONGLONG now = GetTickCount64();
    if (!g_appCatalog.empty() && (now - g_catalogLastBuildTick < 120000)) {
        return;
    }
    if (!g_appCatalog.empty()) {
        return;
    }
    g_catalogLastBuildTick = now;

    // 1. Add built-in Windows commands & CPL/MSC tools
    for (const auto& entry : kBuiltinCommands) {
        AppSuggestionCandidate cand;
        cand.cmd = entry.key;
        cand.execTarget = entry.key;
        cand.subtitle = entry.name;
        cand.cmdLower = ToLowerString(cand.cmd);
        cand.subLower = ToLowerString(cand.subtitle);
        cand.isBuiltin = true;
        cand.descResolved = true;
        g_appCatalog.push_back(std::move(cand));
    }

    // 2. Add Registry App Paths (HKCU + HKLM)
    EnumerateAppPathsRegistry(HKEY_CURRENT_USER, g_appCatalog);
    EnumerateAppPathsRegistry(HKEY_LOCAL_MACHINE, g_appCatalog);

    // 3. Add Start Menu Programs (User + Common)
    wchar_t folderPath[MAX_PATH] = {};
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PROGRAMS, nullptr, SHGFP_TYPE_CURRENT, folderPath))) {
        ScanShortcutDirectoryRecursive(folderPath, L"", 0, g_appCatalog);
    }
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_COMMON_PROGRAMS, nullptr, SHGFP_TYPE_CURRENT, folderPath))) {
        ScanShortcutDirectoryRecursive(folderPath, L"", 0, g_appCatalog);
    }
}

void LazilyResolveCandidateMetadata(AppSuggestionCandidate& cand, const wchar_t* defaultAppTag) {
    if (!cand.descResolved) {
        cand.descResolved = true;
        if (!cand.iconSource.empty()) {
            std::wstring desc = GetExecutableDescription(cand.iconSource.c_str());
            if (!desc.empty()) {
                cand.subtitle = std::move(desc);
                cand.subLower = ToLowerString(cand.subtitle);
            }
        }
    }
    if (cand.subtitle.empty() && defaultAppTag) {
        cand.subtitle = defaultAppTag;
        cand.subLower = ToLowerString(cand.subtitle);
    }

    if (!cand.iconResolved) {
        cand.iconResolved = true;
        if (!cand.iconSource.empty()) {
            SHFILEINFOW sfi = {};
            if (SHGetFileInfoW(cand.iconSource.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON) && sfi.hIcon) {
                cand.cachedIcon = sfi.hIcon;
            }
        }
        if (!cand.cachedIcon) {
            std::wstring dummyName;
            ResolveCommandMetadata(cand.cmd, &cand.cachedIcon, &dummyName);
            if (cand.subtitle.empty() && !dummyName.empty()) {
                cand.subtitle = std::move(dummyName);
                cand.subLower = ToLowerString(cand.subtitle);
            }
        }
    }
}

int ScoreCandidateAgainstQuery(const std::wstring& cmdLower,
                               const std::wstring& subLower,
                               const std::wstring& queryLower,
                               int baseBonus) {
    if (queryLower.empty()) return -1;

    if (cmdLower == queryLower) return 1000 + baseBonus;
    if (cmdLower.find(queryLower) == 0) {
        return 850 + baseBonus - (int)(std::min)(cmdLower.size(), (size_t)60);
    }
    if (subLower == queryLower) return 800 + baseBonus;
    if (subLower.find(queryLower) == 0) {
        return 720 + baseBonus - (int)(std::min)(subLower.size(), (size_t)60);
    }

    // Word boundary match in command or subtitle
    size_t posCmd = cmdLower.find(queryLower);
    if (posCmd != std::wstring::npos) {
        bool wordStart = (posCmd == 0 || cmdLower[posCmd - 1] == L' ' || cmdLower[posCmd - 1] == L'-' || cmdLower[posCmd - 1] == L'_');
        return (wordStart ? 650 : 520) + baseBonus - (int)posCmd;
    }

    size_t posSub = subLower.find(queryLower);
    if (posSub != std::wstring::npos) {
        bool wordStart = (posSub == 0 || subLower[posSub - 1] == L' ' || subLower[posSub - 1] == L'-');
        return (wordStart ? 580 : 420) + baseBonus - (int)posSub;
    }

    return -1;
}

void UpdateAppSuggestionsForQuery(RunDialogState* state, const std::wstring& rawQuery) {
    if (!state) return;

    std::wstring trimmed = TrimCommandString(rawQuery);
    int maxItems = state->settings.maxHistoryItems;

    if (!state->settings.enableAppSuggestions || maxItems <= 0 || trimmed.empty()) {
        if (state->showingSuggestions || !state->suggestions.empty()) {
            int oldHeight = state->totalHeight;
            state->showingSuggestions = false;
            state->suggestions.clear();
            state->hoveredRow = -1;
            state->pressedRow = -1;
            state->selectedRow = -1;
            RecomputeDialogMetrics(state);
            ResizeDialogPreservingBottomAnchor(state, oldHeight);
        }
        return;
    }

    EnsureAppCatalogPopulated();

    std::wstring queryLower = ToLowerString(trimmed);
    struct ScoredMatch {
        int score;
        HistoryItem item;
    };
    std::vector<ScoredMatch> matches;

    // 0. Instant Web Search command detection (ggsearch:<keyword> or msesearch:<keyword>)
    std::wstring webSearchKeyword;
    int webSearchKind = ParseWebSearchCommand(trimmed, &webSearchKeyword);
    if (webSearchKind != 0) {
        MenuLocalizedStrings ml = GetMenuLocalizedStrings(state->settings.language);
        HistoryItem webItem;
        webItem.cmd = trimmed;
        webItem.execTarget = trimmed;
        if (webSearchKind == 1) {
            webItem.appName = webSearchKeyword.empty()
                ? ml.ggSearchEmptyHint
                : (std::wstring(ml.ggSearchQueryPrefix) + webSearchKeyword + L"\"");
        } else {
            webItem.appName = webSearchKeyword.empty()
                ? ml.mseSearchEmptyHint
                : (std::wstring(ml.mseSearchQueryPrefix) + webSearchKeyword + L"\"");
        }
        webItem.isSuggestion = true;
        webItem.ownsIcon = false;
        webItem.webSearchKind = webSearchKind;
        webItem.webSearchQuery = webSearchKeyword;

        // Reuse cached browser icon from catalog if available
        std::wstring targetBrowser = (webSearchKind == 1) ? L"chrome" : L"msedge";
        {
            std::lock_guard<std::mutex> lock(g_catalogMutex);
            for (auto& cand : g_appCatalog) {
                if (cand.cmdLower == targetBrowser) {
                    LazilyResolveCandidateMetadata(cand, state->i18n.appTag);
                    webItem.hIcon = cand.cachedIcon;
                    break;
                }
            }
        }
        matches.push_back({ 2500, std::move(webItem) });
    }

    // 1. Score recent history items first (giving them a slight recency bonus and keeping their timestamp)
    for (size_t i = 0; i < state->history.size(); ++i) {
        const auto& h = state->history[i];
        std::wstring cmdL = ToLowerString(h.cmd);
        std::wstring subL = ToLowerString(h.appName);
        int score = ScoreCandidateAgainstQuery(cmdL, subL, queryLower, 45 - (int)i * 2);
        if (score > 0) {
            HistoryItem copy = h;
            copy.execTarget = h.cmd;
            copy.ownsIcon = false;
            copy.isSuggestion = false;
            matches.push_back({ score, std::move(copy) });
        }
    }

    // 2. Score catalog items (Built-in commands, App Paths, Start Menu apps)
    {
        std::lock_guard<std::mutex> lock(g_catalogMutex);
        for (auto& cand : g_appCatalog) {
            int bonus = cand.isBuiltin ? 25 : (cand.isShortcutFile ? 0 : 15);
            int score = ScoreCandidateAgainstQuery(cand.cmdLower, cand.subLower, queryLower, bonus);
            if (score <= 0) continue;

            bool alreadyInMatches = false;
            for (const auto& m : matches) {
                if (ToLowerString(m.item.cmd) == cand.cmdLower ||
                    (!cand.subtitle.empty() && ToLowerString(m.item.cmd) == cand.subLower)) {
                    alreadyInMatches = true;
                    break;
                }
            }
            if (alreadyInMatches) continue;

            HistoryItem item;
            item.cmd = cand.cmd;
            item.execTarget = cand.execTarget;
            item.appName = cand.subtitle.empty() ? state->i18n.appTag : cand.subtitle;
            item.timeStr.clear();
            item.hIcon = cand.cachedIcon;
            item.ownsIcon = false;
            item.isSuggestion = true;
            item.isShortcutFile = cand.isShortcutFile;
            matches.push_back({ score, std::move(item) });
        }
    }

    std::stable_sort(matches.begin(), matches.end(),
                     [](const ScoredMatch& a, const ScoredMatch& b) { return a.score > b.score; });

    if ((int)matches.size() > maxItems) {
        matches.resize(maxItems);
    }

    // Lazily resolve icons & descriptions only for the top visible matches
    {
        std::lock_guard<std::mutex> lock(g_catalogMutex);
        for (auto& m : matches) {
            if (!m.item.isSuggestion || m.item.hIcon != nullptr) continue;
            std::wstring keyL = ToLowerString(m.item.cmd);
            for (auto& cand : g_appCatalog) {
                if (cand.cmdLower == keyL) {
                    LazilyResolveCandidateMetadata(cand, state->i18n.appTag);
                    m.item.hIcon = cand.cachedIcon;
                    if (!cand.subtitle.empty()) {
                        m.item.appName = cand.subtitle;
                    }
                    break;
                }
            }
        }
    }

    int oldHeight = state->totalHeight;
    state->suggestions.clear();
    for (auto& m : matches) {
        state->suggestions.push_back(std::move(m.item));
    }
    state->showingSuggestions = true;
    state->hoveredRow = -1;
    state->pressedRow = -1;
    state->selectedRow = -1;

    RecomputeDialogMetrics(state);
    ResizeDialogPreservingBottomAnchor(state, oldHeight);
}

void LoadHistoryItems(RunDialogState* state) {
    for (auto& item : state->history) {
        if (item.hIcon && item.ownsIcon) DestroyIcon(item.hIcon);
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
        item.execTarget = cleanText;
        ResolveCommandMetadata(cleanText, &item.hIcon, &item.appName);
        item.ownsIcon = true;
        item.isSuggestion = false;
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

// Measure button label width in DIPs at 96 DPI so buttons fit both text and right-side icon cleanly
int MeasureButtonWidthAt96Dpi(const wchar_t* label, int minWidth96) {
    if (!label || !label[0]) return minWidth96;
    int textWidth96 = (int)wcslen(label) * 7;
    if (g_pDWriteFactory) {
        IDWriteTextFormat* pFmt = nullptr;
        if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
                L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 13.0f, L"", &pFmt))) {
            IDWriteTextLayout* pLayout = nullptr;
            if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                    label, (UINT32)wcslen(label), pFmt, 400.0f, 30.0f, &pLayout))) {
                DWRITE_TEXT_METRICS tm = {};
                if (SUCCEEDED(pLayout->GetMetrics(&tm))) {
                    textWidth96 = (int)std::ceil(tm.widthIncludingTrailingWhitespace);
                }
                pLayout->Release();
            }
            pFmt->Release();
        }
    }
    // 12px left pad + textWidth + 12px gap + 14px icon + 10px right pad = textWidth + 48
    int computed = textWidth96 + 48;
    return std::clamp(computed, minWidth96, 132);
}

ActionButtonsLayout GetActionButtonsLayout(const RunDialogState* state) {
    int dlgW = GetDlgWidth(state);
    int barH = GetInputBarHeight(state);
    UINT dpi = state ? state->dpi : 96;
    int btnH = DpiScaleS(30, state);
    if (state && state->settings.customBarHeight > 0) {
        btnH = std::clamp(barH - DpiScale(18, dpi, 100), DpiScale(24, dpi, 100), DpiScale(44, dpi, 100));
    }
    int top  = state->topAreaHeight + (barH - btnH) / 2;
    int gap  = DpiScaleS(7, state);
    int rightMargin = DpiScaleS(12, state);

    float fontScale = (state && state->settings.customFontSize > 0)
                      ? (std::clamp(state->settings.customFontSize, 9, 24) / 13.5f)
                      : (GetDlgScale(state) / 100.0f);
    auto scaleBtnW = [&](int base96W) -> int {
        return (int)std::ceil(base96W * (dpi / 96.0f) * std::clamp(fontScale, 0.80f, 1.45f));
    };

    int wGear   = btnH;
    int wBrowse = scaleBtnW(MeasureButtonWidthAt96Dpi(state->i18n.browseBtn, 86));
    int wRun    = scaleBtnW(MeasureButtonWidthAt96Dpi(state->i18n.runBtn,    80));
    int wClose  = scaleBtnW(MeasureButtonWidthAt96Dpi(state->i18n.exitBtn,   76));

    ActionButtonsLayout layout = {};
    layout.rcGear = { dlgW - rightMargin - wGear, top, dlgW - rightMargin, top + btnH };
    if (state && state->settings.minimalMode) {
        layout.rcClose  = { 0, 0, 0, 0 };
        layout.rcBrowse = { 0, 0, 0, 0 };
        layout.rcRun    = { layout.rcGear.left - gap - wRun, top, layout.rcGear.left - gap, top + btnH };
    } else {
        layout.rcClose  = { layout.rcGear.left - gap - wClose, top, layout.rcGear.left - gap, top + btnH };
        layout.rcRun    = { layout.rcClose.left - gap - wRun, top, layout.rcClose.left - gap, top + btnH };
        layout.rcBrowse = { layout.rcRun.left - gap - wBrowse, top, layout.rcRun.left - gap, top + btnH };
    }

    int editRightAnchor = (state && state->settings.minimalMode) ? layout.rcRun.left : layout.rcBrowse.left;
    int editH = (std::max)(DpiScaleS(24, state), (int)std::ceil(GetFontSizeSF(13.5f, state) * 1.6f));
    editH = (std::min)(editH, barH - DpiScale(6, dpi, 100));
    int editTop = state->topAreaHeight + (barH - editH) / 2;
    layout.rcEdit = {
        DpiScaleS(46, state),
        editTop,
        editRightAnchor - DpiScaleS(10, state),
        editTop + editH
    };
    return layout;
}

RECT GetBrowseButtonRect(const RunDialogState* state) {
    return GetActionButtonsLayout(state).rcBrowse;
}

RECT GetRunButtonRect(const RunDialogState* state) {
    return GetActionButtonsLayout(state).rcRun;
}

RECT GetCloseButtonRect(const RunDialogState* state) {
    return GetActionButtonsLayout(state).rcClose;
}

RECT GetModernEditRect(const RunDialogState* state) {
    return GetActionButtonsLayout(state).rcEdit;
}

bool HasVerticalScrollbar(const RunDialogState* state) {
    if (!state) return false;
    int totalRows = (int)GetActiveRowItems(state).size();
    int visRows = GetVisibleRowCount(state);
    return totalRows > visRows && visRows > 0;
}

RECT GetScrollbarTrackRect(const RunDialogState* state) {
    if (!HasVerticalScrollbar(state)) return { 0, 0, 0, 0 };
    int dlgW = GetDlgWidth(state);
    int padTop = GetListPadTop(state);
    int padBot = GetListPadBottom(state);
    int rightPad = DpiScaleS(4, state);
    int trackW = DpiScaleS(6, state);
    return {
        dlgW - rightPad - trackW,
        padTop + DpiScaleS(2, state),
        dlgW - rightPad,
        state->topAreaHeight - padBot - DpiScaleS(2, state)
    };
}

RECT GetScrollbarThumbRect(const RunDialogState* state) {
    RECT rcTrack = GetScrollbarTrackRect(state);
    if (rcTrack.right <= rcTrack.left || rcTrack.bottom <= rcTrack.top) return { 0, 0, 0, 0 };

    int totalRows = (int)GetActiveRowItems(state).size();
    int visRows = GetVisibleRowCount(state);
    if (totalRows <= visRows || visRows <= 0) return { 0, 0, 0, 0 };

    int trackH = rcTrack.bottom - rcTrack.top;
    int minThumbH = DpiScaleS(24, state);
    int thumbH = (std::max)(minThumbH, MulDiv(trackH, visRows, totalRows));
    thumbH = (std::min)(thumbH, trackH);

    int maxScroll = totalRows - visRows;
    int availSlide = (std::max)(0, trackH - thumbH);
    int thumbTop = rcTrack.top + (maxScroll > 0 ? MulDiv(availSlide, std::clamp(state->scrollOffset, 0, maxScroll), maxScroll) : 0);
    return { rcTrack.left, thumbTop, rcTrack.right, thumbTop + thumbH };
}

int HitTestHistoryRow(const RunDialogState* state, POINT pt) {
    const auto& activeRows = GetActiveRowItems(state);
    int visRows = GetVisibleRowCount(state);
    int padTop = GetListPadTop(state);
    int padBot = GetListPadBottom(state);
    int rowH   = GetRowHeight(state);
    int dlgW   = GetDlgWidth(state);
    int padX   = DpiScaleS(10, state);

    if (activeRows.empty() || visRows <= 0 || pt.y < padTop || pt.y >= state->topAreaHeight - padBot) {
        return -1;
    }
    if (HasVerticalScrollbar(state)) {
        RECT rcTrack = GetScrollbarTrackRect(state);
        if (pt.x >= rcTrack.left - DpiScaleS(4, state)) {
            return -1;
        }
    }
    if (pt.x < padX || pt.x > dlgW - padX) return -1;
    int visIdx = (pt.y - padTop) / rowH;
    if (visIdx >= 0 && visIdx < visRows) {
        int actualIdx = state->scrollOffset + visIdx;
        if (actualIdx >= 0 && actualIdx < (int)activeRows.size()) {
            return actualIdx;
        }
    }
    return -1;
}

// --- Direct2D Liquid Glass, Vector Icons & DirectWrite Rendering Helpers ---

void DrawD2DGlassRim(ID2D1RenderTarget* pRT, D2D1_RECT_F rc, float radius, UINT dpi, bool isLight, float outerThicknessDip = 1.2f) {
    // Single crisp 5-stop refractive glass rim (no secondary inner border line)
    D2D1_GRADIENT_STOP stops[5] = {
        { 0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.88f : 0.92f) },
        { 0.30f, D2D1::ColorF(185.0f / 255.0f, 228.0f / 255.0f, 1.00f, isLight ? 0.42f : 0.46f) },
        { 0.52f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.22f : 0.18f) },
        { 0.76f, D2D1::ColorF(1.00f, 210.0f / 255.0f, 240.0f / 255.0f, isLight ? 0.44f : 0.48f) },
        { 1.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, isLight ? 0.72f : 0.76f) }
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

    if (isLight) {
        ID2D1SolidColorBrush* pDarkRim = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.12f), &pDarkRim))) {
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rc, radius, radius), pDarkRim, DpiScaleF(0.85f, dpi));
            pDarkRim->Release();
        }
    }
}

void DrawD2DGlassButton(ID2D1RenderTarget* pRT, D2D1_RECT_F rc, float radius, UINT dpi,
                        D2D1_COLOR_F bgTint, bool hovered, bool pressed, bool isLight) {
    D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(rc, radius, radius);

    // Layer 1: Subtle Acrylic/Glass base
    ID2D1SolidColorBrush* pBaseBr = nullptr;
    if (SUCCEEDED(pRT->CreateSolidColorBrush(
            isLight ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.035f) : D2D1::ColorF(0.69f, 0.69f, 0.69f, 0.06f), &pBaseBr))) {
        pRT->FillRoundedRectangle(rrect, pBaseBr);
        pBaseBr->Release();
    }

    // Layer 2: Button background tint (0.28 base opacity in live-update ControlTemplate)
    if (bgTint.a > 0.001f) {
        ID2D1SolidColorBrush* pTintBr = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(bgTint, &pTintBr))) {
            pRT->FillRoundedRectangle(rrect, pTintBr);
            pTintBr->Release();
        }
    }

    // Layer 3: Diagonal highlight sheen (0.16 normal, 0.30 PointerOver, 0.50 Pressed)
    float hlOpacity = pressed ? 0.50f : (hovered ? 0.30f : 0.16f);
    D2D1_GRADIENT_STOP hlStops[3] = {
        { 0.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.00f * hlOpacity) },
        { 0.45f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.00f) },
        { 1.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.533f * hlOpacity) }
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

    // Layer 4: Outer refractive border (1.2 DIP) + inner hairline border
    DrawD2DGlassRim(pRT, rc, radius, dpi, isLight, 1.2f);
}

void DrawD2DPointerSpecularGlow(ID2D1RenderTarget* pRT, POINT mousePt, bool hasMousePt, int w, int h, UINT dpi, bool isLight) {
    // True iOS 26 Liquid Glass 4-Edge Refractive Lens Cushion (Images 2-6):
    // Keeps the interior 100% clear while bending light along the 4 perimeter edges & corners.
    float topBand  = DpiScaleF(15.0f, dpi);
    float botBand  = DpiScaleF(13.0f, dpi);
    float sideBand = DpiScaleF(13.0f, dpi);

    // 1. Top edge refractive cushion (fades to 0.0 within 15 DIPs)
    {
        D2D1_GRADIENT_STOP stops[3] = {
            { 0.00f, D2D1::ColorF(0.92f, 0.97f, 1.00f, isLight ? 0.20f : 0.16f) },
            { 0.45f, D2D1::ColorF(0.88f, 0.95f, 1.00f, isLight ? 0.05f : 0.04f) },
            { 1.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, 0.00f) }
        };
        ID2D1GradientStopCollection* pStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
            ID2D1LinearGradientBrush* pBr = nullptr;
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F(0.0f, topBand));
            if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pStops, &pBr))) {
                pRT->FillRectangle(D2D1::RectF(0.0f, 0.0f, (float)w, (std::min)((float)h, topBand)), pBr);
                pBr->Release();
            }
            pStops->Release();
        }
    }

    // 2. Bottom edge refractive cushion (fades from 0.0 to edge glow within 13 DIPs)
    if ((float)h > botBand) {
        D2D1_GRADIENT_STOP stops[3] = {
            { 0.00f, D2D1::ColorF(0.80f, 0.92f, 1.00f, 0.00f) },
            { 0.55f, D2D1::ColorF(0.80f, 0.92f, 1.00f, isLight ? 0.04f : 0.03f) },
            { 1.00f, D2D1::ColorF(0.85f, 0.94f, 1.00f, isLight ? 0.14f : 0.11f) }
        };
        ID2D1GradientStopCollection* pStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
            ID2D1LinearGradientBrush* pBr = nullptr;
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(0.0f, (float)h - botBand), D2D1::Point2F(0.0f, (float)h));
            if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pStops, &pBr))) {
                pRT->FillRectangle(D2D1::RectF(0.0f, (float)h - botBand, (float)w, (float)h), pBr);
                pBr->Release();
            }
            pStops->Release();
        }
    }

    // 3. Left edge refractive cushion
    if ((float)w > sideBand) {
        D2D1_GRADIENT_STOP stops[2] = {
            { 0.00f, D2D1::ColorF(0.88f, 0.95f, 1.00f, isLight ? 0.13f : 0.10f) },
            { 1.00f, D2D1::ColorF(0.88f, 0.95f, 1.00f, 0.00f) }
        };
        ID2D1GradientStopCollection* pStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
            ID2D1LinearGradientBrush* pBr = nullptr;
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F(sideBand, 0.0f));
            if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pStops, &pBr))) {
                pRT->FillRectangle(D2D1::RectF(0.0f, 0.0f, sideBand, (float)h), pBr);
                pBr->Release();
            }
            pStops->Release();
        }
    }

    // 4. Right edge refractive cushion
    if ((float)w > sideBand) {
        D2D1_GRADIENT_STOP stops[2] = {
            { 0.00f, D2D1::ColorF(0.88f, 0.95f, 1.00f, 0.00f) },
            { 1.00f, D2D1::ColorF(0.88f, 0.95f, 1.00f, isLight ? 0.11f : 0.08f) }
        };
        ID2D1GradientStopCollection* pStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
            ID2D1LinearGradientBrush* pBr = nullptr;
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                D2D1::Point2F((float)w - sideBand, 0.0f), D2D1::Point2F((float)w, 0.0f));
            if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pStops, &pBr))) {
                pRT->FillRectangle(D2D1::RectF((float)w - sideBand, 0.0f, (float)w, (float)h), pBr);
                pBr->Release();
            }
            pStops->Release();
        }
    }

    // 5. Top-left & bottom-right rim glare highlights (matching Image 3 glass cards)
    {
        float glareRx = DpiScaleF(95.0f, dpi);
        float glareRy = DpiScaleF(16.0f, dpi);
        D2D1_GRADIENT_STOP gStops[2] = {
            { 0.0f, D2D1::ColorF(0.92f, 0.98f, 1.00f, 0.24f) },
            { 1.0f, D2D1::ColorF(0.92f, 0.98f, 1.00f, 0.00f) }
        };
        ID2D1GradientStopCollection* pGStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(gStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pGStops))) {
            ID2D1RadialGradientBrush* pGlare = nullptr;
            D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES gp1 = D2D1::RadialGradientBrushProperties(
                D2D1::Point2F((float)w * 0.22f, DpiScaleF(2.0f, dpi)),
                D2D1::Point2F(0.0f, 0.0f), glareRx, glareRy);
            if (SUCCEEDED(pRT->CreateRadialGradientBrush(gp1, pGStops, &pGlare))) {
                pRT->FillRectangle(D2D1::RectF(0.0f, 0.0f, (float)w, DpiScaleF(20.0f, dpi)), pGlare);
                pGlare->Release();
            }
            pGStops->Release();
        }
    }

    // 6. Subtle interactive pointer refraction (only when mouse is inside, low alpha so text stays sharp)
    if (hasMousePt) {
        float rad = DpiScaleF(130.0f, dpi);
        D2D1_GRADIENT_STOP radStops[2] = {
            { 0.0f, D2D1::ColorF(0.90f, 0.96f, 1.00f, 0.06f) },
            { 1.0f, D2D1::ColorF(1.00f, 1.00f, 1.00f, 0.00f) }
        };
        ID2D1GradientStopCollection* pRadStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(radStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pRadStops))) {
            ID2D1RadialGradientBrush* pRadial = nullptr;
            D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES radProps = D2D1::RadialGradientBrushProperties(
                D2D1::Point2F((float)mousePt.x, (float)mousePt.y), D2D1::Point2F(0.0f, 0.0f), rad, rad);
            if (SUCCEEDED(pRT->CreateRadialGradientBrush(radProps, pRadStops, &pRadial))) {
                pRT->FillRectangle(D2D1::RectF(0.0f, 0.0f, (float)w, (float)h), pRadial);
                pRadial->Release();
            }
            pRadStops->Release();
        }
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

// Folder outline icon for the Browse ("Duyệt") button
void DrawD2DFolderVector(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                         float cx, float cy, UINT dpi) {
    float hw = DpiScaleF(5.8f, dpi);
    float hh = DpiScaleF(4.4f, dpi);
    float left   = cx - hw;
    float right  = cx + hw;
    float top    = cy - hh;
    float bottom = cy + hh;
    float tabW   = DpiScaleF(4.4f, dpi);
    float tabH   = DpiScaleF(1.8f, dpi);
    float sw     = DpiScaleF(1.3f, dpi);

    pRT->DrawLine(D2D1::Point2F(left, top), D2D1::Point2F(left + tabW, top), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(left + tabW, top), D2D1::Point2F(left + tabW + DpiScaleF(1.4f, dpi), top + tabH), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(left, top + tabH), D2D1::Point2F(right, top + tabH), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(right, top + tabH), D2D1::Point2F(right, bottom), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(right, bottom), D2D1::Point2F(left, bottom), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(left, bottom), D2D1::Point2F(left, top), pBrush, sw, pStroke);
}

// Return/Enter arrow icon ("↵") for the Run ("Chạy") button
void DrawD2DReturnArrowVector(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                              float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.4f, dpi);
    float rightX = cx + DpiScaleF(4.4f, dpi);
    float leftX  = cx - DpiScaleF(4.4f, dpi);
    float topY   = cy - DpiScaleF(3.8f, dpi);
    float midY   = cy + DpiScaleF(1.4f, dpi);
    float arrowW = DpiScaleF(2.9f, dpi);
    float arrowH = DpiScaleF(2.7f, dpi);

    pRT->DrawLine(D2D1::Point2F(rightX, topY), D2D1::Point2F(rightX, midY), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(rightX, midY), D2D1::Point2F(leftX, midY), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(leftX + arrowW, midY - arrowH), D2D1::Point2F(leftX, midY), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(leftX + arrowW, midY + arrowH), D2D1::Point2F(leftX, midY), pBrush, sw, pStroke);
}

// Close cross icon ("✕") for the Exit ("Thoát") button
void DrawD2DCloseCrossVector(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                             float cx, float cy, UINT dpi) {
    float r  = DpiScaleF(3.8f, dpi);
    float sw = DpiScaleF(1.4f, dpi);
    pRT->DrawLine(D2D1::Point2F(cx - r, cy - r), D2D1::Point2F(cx + r, cy + r), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(cx + r, cy - r), D2D1::Point2F(cx - r, cy + r), pBrush, sw, pStroke);
}

// Crisp 8-tooth Cog/Gear vector icon ("⚙️") for the In-App Settings button next to Exit
void DrawD2DGearVector(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                       float cx, float cy, UINT dpi) {
    float innerR = DpiScaleF(2.2f, dpi);
    float bodyR  = DpiScaleF(4.6f, dpi);
    float toothR = DpiScaleF(6.3f, dpi);
    float swRing = DpiScaleF(1.4f, dpi);
    float swTooth = DpiScaleF(1.55f, dpi);

    for (int i = 0; i < 8; ++i) {
        float angle = i * (3.14159265f / 4.0f);
        float c = std::cos(angle);
        float s = std::sin(angle);
        pRT->DrawLine(
            D2D1::Point2F(cx + c * (bodyR - DpiScaleF(0.4f, dpi)), cy + s * (bodyR - DpiScaleF(0.4f, dpi))),
            D2D1::Point2F(cx + c * toothR, cy + s * toothR),
            pBrush, swTooth, pStroke);
    }
    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), bodyR, bodyR), pBrush, swRing, pStroke);
    pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), innerR, innerR), pBrush, DpiScaleF(1.2f, dpi), pStroke);
}

// --- Modern Windows 11 Fluent Acrylic / Liquid Glass Popup Menu Engine ---

enum class FluentMenuIcon {
    None = 0,
    Surface,
    ColorMode,
    Scale,
    Background,
    History,
    Language,
    ClearTrash,
    FolderPick,
    RemoveBg,
    MinimalLayout
};

void DrawD2DMenuChevronRight(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                             float cx, float cy, UINT dpi) {
    float h = DpiScaleF(3.8f, dpi);
    float w = DpiScaleF(2.2f, dpi);
    float sw = DpiScaleF(1.35f, dpi);
    pRT->DrawLine(D2D1::Point2F(cx - w, cy - h), D2D1::Point2F(cx + w, cy), pBrush, sw, pStroke);
    pRT->DrawLine(D2D1::Point2F(cx + w, cy), D2D1::Point2F(cx - w, cy + h), pBrush, sw, pStroke);
}

void DrawD2DMenuCheckmark(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                          float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.65f, dpi);
    D2D1_POINT_2F p1 = D2D1::Point2F(cx - DpiScaleF(4.4f, dpi), cy + DpiScaleF(0.3f, dpi));
    D2D1_POINT_2F p2 = D2D1::Point2F(cx - DpiScaleF(1.2f, dpi), cy + DpiScaleF(3.5f, dpi));
    D2D1_POINT_2F p3 = D2D1::Point2F(cx + DpiScaleF(4.8f, dpi), cy - DpiScaleF(3.2f, dpi));
    pRT->DrawLine(p1, p2, pBrush, sw, pStroke);
    pRT->DrawLine(p2, p3, pBrush, sw, pStroke);
}

void DrawD2DFluentMenuIcon(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                           FluentMenuIcon icon, float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.3f, dpi);
    switch (icon) {
        case FluentMenuIcon::Surface: {
            // 2x2 Fluent rounded grid icon
            float gap = DpiScaleF(1.1f, dpi);
            float sz  = DpiScaleF(4.8f, dpi);
            float rad = DpiScaleF(1.2f, dpi);
            D2D1_RECT_F tl = D2D1::RectF(cx - gap - sz, cy - gap - sz, cx - gap, cy - gap);
            D2D1_RECT_F tr = D2D1::RectF(cx + gap, cy - gap - sz, cx + gap + sz, cy - gap);
            D2D1_RECT_F bl = D2D1::RectF(cx - gap - sz, cy + gap, cx - gap, cy + gap + sz);
            D2D1_RECT_F br = D2D1::RectF(cx + gap, cy + gap, cx + gap + sz, cy + gap + sz);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(tl, rad, rad), pBrush, sw, pStroke);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(tr, rad, rad), pBrush, sw, pStroke);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(bl, rad, rad), pBrush, sw, pStroke);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(br, rad, rad), pBrush, sw, pStroke);
            break;
        }
        case FluentMenuIcon::ColorMode: {
            // Sun / Contrast theme icon
            float r = DpiScaleF(3.4f, dpi);
            pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), pBrush, sw, pStroke);
            float rayIn  = DpiScaleF(4.8f, dpi);
            float rayOut = DpiScaleF(6.5f, dpi);
            for (int i = 0; i < 8; ++i) {
                float a = i * (3.14159265f / 4.0f);
                float c = std::cos(a), s = std::sin(a);
                pRT->DrawLine(D2D1::Point2F(cx + c * rayIn, cy + s * rayIn),
                              D2D1::Point2F(cx + c * rayOut, cy + s * rayOut),
                              pBrush, DpiScaleF(1.25f, dpi), pStroke);
            }
            break;
        }
        case FluentMenuIcon::Scale: {
            // Dual up/down or diagonal resize arrows (like Windows 11 Sort/Scale icon)
            float offX = DpiScaleF(2.8f, dpi);
            float topY = cy - DpiScaleF(5.5f, dpi);
            float botY = cy + DpiScaleF(5.5f, dpi);
            float aw   = DpiScaleF(2.4f, dpi);
            float ah   = DpiScaleF(2.5f, dpi);
            // Left Up Arrow
            pRT->DrawLine(D2D1::Point2F(cx - offX, botY), D2D1::Point2F(cx - offX, topY), pBrush, sw, pStroke);
            pRT->DrawLine(D2D1::Point2F(cx - offX - aw, topY + ah), D2D1::Point2F(cx - offX, topY), pBrush, sw, pStroke);
            pRT->DrawLine(D2D1::Point2F(cx - offX + aw, topY + ah), D2D1::Point2F(cx - offX, topY), pBrush, sw, pStroke);
            // Right Down Arrow
            pRT->DrawLine(D2D1::Point2F(cx + offX, topY), D2D1::Point2F(cx + offX, botY), pBrush, sw, pStroke);
            pRT->DrawLine(D2D1::Point2F(cx + offX - aw, botY - ah), D2D1::Point2F(cx + offX, botY), pBrush, sw, pStroke);
            pRT->DrawLine(D2D1::Point2F(cx + offX + aw, botY - ah), D2D1::Point2F(cx + offX, botY), pBrush, sw, pStroke);
            break;
        }
        case FluentMenuIcon::Background: {
            // Picture / Media frame with sun dot & mountain contour
            float hw = DpiScaleF(6.2f, dpi);
            float hh = DpiScaleF(5.0f, dpi);
            float rad = DpiScaleF(1.6f, dpi);
            D2D1_RECT_F rc = D2D1::RectF(cx - hw, cy - hh, cx + hw, cy + hh);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rc, rad, rad), pBrush, sw, pStroke);
            pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx - DpiScaleF(2.6f, dpi), cy - DpiScaleF(1.8f, dpi)),
                                           DpiScaleF(1.2f, dpi), DpiScaleF(1.2f, dpi)), pBrush);
            pRT->DrawLine(D2D1::Point2F(cx - hw + DpiScaleF(1.2f, dpi), cy + hh - DpiScaleF(1.0f, dpi)),
                          D2D1::Point2F(cx + DpiScaleF(1.0f, dpi), cy - DpiScaleF(0.4f, dpi)), pBrush, sw, pStroke);
            pRT->DrawLine(D2D1::Point2F(cx + DpiScaleF(1.0f, dpi), cy - DpiScaleF(0.4f, dpi)),
                          D2D1::Point2F(cx + hw - DpiScaleF(1.0f, dpi), cy + hh - DpiScaleF(1.0f, dpi)), pBrush, sw, pStroke);
            break;
        }
        case FluentMenuIcon::History: {
            // Clock / History vector
            DrawD2DClockVector(pRT, pBrush, pStroke, cx, cy, dpi);
            break;
        }
        case FluentMenuIcon::Language: {
            // Globe / Language icon with meridian ellipse and equator
            float r = DpiScaleF(6.0f, dpi);
            pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), pBrush, sw, pStroke);
            pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), DpiScaleF(2.8f, dpi), r), pBrush, DpiScaleF(1.15f, dpi), pStroke);
            pRT->DrawLine(D2D1::Point2F(cx - r, cy), D2D1::Point2F(cx + r, cy), pBrush, DpiScaleF(1.15f, dpi), pStroke);
            break;
        }
        case FluentMenuIcon::ClearTrash: {
            // Fluent Trash / Delete bin icon
            float topY = cy - DpiScaleF(4.2f, dpi);
            float botY = cy + DpiScaleF(5.8f, dpi);
            float hw   = DpiScaleF(4.5f, dpi);
            float lidW = DpiScaleF(5.8f, dpi);
            pRT->DrawLine(D2D1::Point2F(cx - lidW, topY), D2D1::Point2F(cx + lidW, topY), pBrush, sw, pStroke);
            pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(2.0f, dpi), topY - DpiScaleF(1.6f, dpi)),
                          D2D1::Point2F(cx + DpiScaleF(2.0f, dpi), topY - DpiScaleF(1.6f, dpi)), pBrush, sw, pStroke);
            D2D1_RECT_F bodyRc = D2D1::RectF(cx - hw, topY + DpiScaleF(0.4f, dpi), cx + hw, botY);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(bodyRc, DpiScaleF(1.2f, dpi), DpiScaleF(1.2f, dpi)), pBrush, sw, pStroke);
            pRT->DrawLine(D2D1::Point2F(cx - DpiScaleF(1.6f, dpi), topY + DpiScaleF(2.6f, dpi)),
                          D2D1::Point2F(cx - DpiScaleF(1.6f, dpi), botY - DpiScaleF(2.2f, dpi)), pBrush, DpiScaleF(1.15f, dpi), pStroke);
            pRT->DrawLine(D2D1::Point2F(cx + DpiScaleF(1.6f, dpi), topY + DpiScaleF(2.6f, dpi)),
                          D2D1::Point2F(cx + DpiScaleF(1.6f, dpi), botY - DpiScaleF(2.2f, dpi)), pBrush, DpiScaleF(1.15f, dpi), pStroke);
            break;
        }
        case FluentMenuIcon::FolderPick: {
            DrawD2DFolderVector(pRT, pBrush, pStroke, cx, cy, dpi);
            break;
        }
        case FluentMenuIcon::RemoveBg: {
            DrawD2DCloseCrossVector(pRT, pBrush, pStroke, cx, cy, dpi);
            break;
        }
        case FluentMenuIcon::MinimalLayout: {
            // Sleek command bar pill icon with prompt & action button
            float hw = DpiScaleF(6.4f, dpi);
            float hh = DpiScaleF(4.2f, dpi);
            float rad = DpiScaleF(2.0f, dpi);
            D2D1_RECT_F rcBar = D2D1::RectF(cx - hw, cy - hh, cx + hw, cy + hh);
            pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcBar, rad, rad), pBrush, sw, pStroke);
            pRT->DrawLine(D2D1::Point2F(cx - hw + DpiScaleF(2.6f, dpi), cy),
                          D2D1::Point2F(cx + DpiScaleF(0.6f, dpi), cy), pBrush, sw, pStroke);
            D2D1_RECT_F rcBtn = D2D1::RectF(cx + DpiScaleF(2.2f, dpi), cy - DpiScaleF(2.0f, dpi),
                                            cx + hw - DpiScaleF(1.8f, dpi), cy + DpiScaleF(2.0f, dpi));
            pRT->FillRoundedRectangle(D2D1::RoundedRect(rcBtn, DpiScaleF(1.0f, dpi), DpiScaleF(1.0f, dpi)), pBrush);
            break;
        }
        default:
            break;
    }
}

struct FluentMenuItem {
    UINT id = 0;
    std::wstring label;
    FluentMenuIcon icon = FluentMenuIcon::None;
    bool checked = false;
    bool disabled = false;
    bool isSeparator = false;
    std::vector<FluentMenuItem> subItems;
    int inlineEditKind = 0; // 0 = normal, 1 = Width (px), 2 = Height (px), 3 = Font Size (pt)
};

struct FluentMenuPanelState {
    HWND hWnd = nullptr;
    std::vector<FluentMenuItem> items;
    bool isSubMenu = false;
    int hoveredIdx = -1;
    int pressedIdx = -1;
    int openSubIdx = -1;
    int scrollOffset = 0;
    int maxVisibleRows = 14;
    int width = 260;
    int height = 100;
    POINT mousePt = { -1, -1 };
    bool hasMousePt = false;
    struct FluentMenuSession* session = nullptr;
};

struct FluentMenuSession {
    RunDialogState* dlgState = nullptr;
    UINT dpi = 96;
    UINT effectiveDpi = 96;
    bool isLight = false;
    SurfaceTheme surface = SurfaceTheme::Glass;
    FluentMenuPanelState mainPanel;
    FluentMenuPanelState subPanel;
    UINT selectedCmd = 0;
    bool running = true;
    bool mouseReleasedSinceOpen = false;
    bool wasForeground = false;
    int pendingHoverSubIdx = -1;
    int editingInlineKind = 0;      // 0 = none, 1 = Width, 2 = Height, 3 = Font
    bool inlineEditReplace = false; // true right after clicking box so first digit replaces whole number
    std::wstring inlineEditBuf;
    int pressedInlinePart = 0;      // 0 = none, 1 = [-], 2 = [box], 3 = [+]
};

std::atomic<HWND> g_hActiveGearMenu{nullptr};
std::atomic<HWND> g_hActiveCustomSizeWnd{nullptr};
constexpr UINT_PTR kTimerMenuSubHover = 9501;
constexpr UINT_PTR kTimerMenuWatchdog = 9502;
constexpr wchar_t kFluentMenuClassName[] = L"WindhawkModernRunFluentMenuWnd";

void CloseActiveGearMenu() {
    if (HWND hGearMenu = g_hActiveGearMenu.load()) {
        if (IsWindow(hGearMenu)) {
            PostMessageW(hGearMenu, WM_CLOSE, 0, 0);
        }
    }
    if (!g_isBrowsingFile.load()) {
        if (HWND hSizeWnd = g_hActiveCustomSizeWnd.load()) {
            if (IsWindow(hSizeWnd)) {
                PostMessageW(hSizeWnd, WM_CLOSE, 0, 0);
            }
        }
    }
}

int GetMenuRowHeight(const FluentMenuSession* s) {
    return DpiScale(34, s->effectiveDpi);
}

int GetMenuSeparatorHeight(const FluentMenuSession* s) {
    return DpiScale(9, s->effectiveDpi);
}

int GetMenuPadY(const FluentMenuSession* s) {
    return DpiScale(6, s->effectiveDpi);
}

bool PanelNeedsScroll(const FluentMenuPanelState* p) {
    return (int)p->items.size() > p->maxVisibleRows;
}

void ApplyFluentMenuWindowBackdrop(HWND hWnd, const FluentMenuSession* s);

int GetInlineMenuFieldValue(const RunDialogState* state, int kind) {
    if (!state) return (kind == 1) ? kBaseDlgWidth : ((kind == 2) ? kBaseInputBarHeight : 14);
    int scale = GetDlgScale(state);
    switch (kind) {
        case 1:
            return (state->settings.customDialogWidth >= 360)
                   ? std::clamp(state->settings.customDialogWidth, 360, 1200)
                   : MulDiv(kBaseDlgWidth, scale, 100);
        case 2:
            return (state->settings.customBarHeight > 0)
                   ? std::clamp(state->settings.customBarHeight, 36, 96)
                   : MulDiv(kBaseInputBarHeight, scale, 100);
        case 3:
            return (state->settings.customFontSize > 0)
                   ? std::clamp(state->settings.customFontSize, 9, 24)
                   : (int)std::round(13.5f * scale / 100.0f);
        case 4:
            return std::clamp(state->settings.glassOpacity, -100, 100);
        case 5:
            return std::clamp(state->settings.blurOpacity, 0, 255);
        case 6:
            return std::clamp(state->settings.customBackgroundOpacity, 20, 255);
        case 7:
            return std::clamp(state->settings.maxHistoryItems, 0, 50);
        case 8:
            return std::clamp(state->settings.visibleHistoryRows, 3, 10);
        default:
            return 0;
    }
}

void ApplyInlineMenuFieldValue(FluentMenuSession* s, int kind, int val) {
    if (!s || !s->dlgState || !s->dlgState->hDlg || !IsWindow(s->dlgState->hDlg)) return;
    RunDialogState* st = s->dlgState;
    if (kind == 1) {
        val = std::clamp(val, 360, 1200);
        Wh_SetIntValue(GetUserOverrideKey(L"customDialogWidth").c_str(), val);
    } else if (kind == 2) {
        if (val >= 150) {
            val = MulDiv(val, kBaseInputBarHeight,
                         kBaseInputBarHeight + 5 * kBaseRowHeight + kBaseListPadTop + kBaseListPadBottom);
        }
        val = std::clamp(val, 36, 96);
        Wh_SetIntValue(GetUserOverrideKey(L"customBarHeight").c_str(), val);
    } else if (kind == 3) {
        val = std::clamp(val, 9, 24);
        Wh_SetIntValue(GetUserOverrideKey(L"customFontSize").c_str(), val);
    } else if (kind == 4) {
        val = std::clamp(val, -100, 100);
        Wh_SetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), val);
        Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"glass");
    } else if (kind == 5) {
        val = std::clamp(val, 0, 255);
        Wh_SetIntValue(GetUserOverrideKey(L"blurOpacity").c_str(), val);
        Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"blur");
    } else if (kind == 6) {
        val = std::clamp(val, 20, 255);
        Wh_SetIntValue(GetUserOverrideKey(L"customBackgroundOpacity").c_str(), val);
    } else if (kind == 7) {
        val = std::clamp(val, 0, 50);
        Wh_SetIntValue(GetUserOverrideKey(L"maxHistoryItems").c_str(), val);
    } else if (kind == 8) {
        val = std::clamp(val, 3, 10);
        Wh_SetIntValue(GetUserOverrideKey(L"visibleHistoryRows").c_str(), val);
    } else {
        return;
    }

    LoadSettings(false);
    int oldHeight = st->totalHeight;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        st->settings = g_settings;
        st->i18n = g_i18n;
    }
    if (kind == 7) {
        LoadHistoryItems(st);
        std::wstring curText = GetEffectiveInputText(st->hModernEdit);
        UpdateAppSuggestionsForQuery(st, curText);
    }
    UpdateDialogEditFont(st);
    RecomputeDialogMetrics(st);
    ResizeDialogPreservingBottomAnchor(st, oldHeight);
    ApplyWindowAppearance(st->hDlg, st);
    InvalidateRect(st->hDlg, nullptr, TRUE);
    UpdateWindow(st->hDlg);

    if (kind == 4 || kind == 5) {
        s->surface = st->effectiveSurface;
        if (s->mainPanel.hWnd && IsWindow(s->mainPanel.hWnd)) ApplyFluentMenuWindowBackdrop(s->mainPanel.hWnd, s);
        if (s->subPanel.hWnd && IsWindow(s->subPanel.hWnd)) ApplyFluentMenuWindowBackdrop(s->subPanel.hWnd, s);
        for (auto& it : s->subPanel.items) {
            if (it.id == 101) it.checked = (st->settings.surfaceTheme == SurfaceTheme::Glass);
            else if (it.id == 102) it.checked = (st->settings.surfaceTheme == SurfaceTheme::Blur);
            else if (it.id == 103) it.checked = (st->settings.surfaceTheme == SurfaceTheme::Solid);
            else if (kind == 4) {
                if (it.id == 107) it.checked = (val <= -90);
                else if (it.id == 108) it.checked = (val > -90 && val <= -65);
                else if (it.id == 109) it.checked = (val > -65 && val <= -25);
                else if (it.id == 110) it.checked = (val > -25 && val <= 5);
                else if (it.id == 111) it.checked = (val > 5 && val <= 20);
                else if (it.id == 112) it.checked = (val > 20 && val <= 42);
                else if (it.id == 113) it.checked = (val > 42 && val <= 68);
                else if (it.id == 114) it.checked = (val > 68);
            }
        }
    } else if (kind >= 1 && kind <= 3) {
        for (auto& it : s->subPanel.items) {
            if (it.id >= 301 && it.id <= 305) it.checked = false;
        }
    } else if (kind == 6) {
        for (auto& it : s->subPanel.items) {
            if (it.id == 503) it.checked = (val <= 95);
            else if (it.id == 504) it.checked = (val > 95 && val <= 175);
            else if (it.id == 505) it.checked = (val > 175 && val <= 230);
            else if (it.id == 506) it.checked = (val > 230);
        }
    } else if (kind == 7) {
        for (auto& it : s->subPanel.items) {
            if (it.id == 401) it.checked = (val == 10);
            else if (it.id == 402) it.checked = (val == 30);
            else if (it.id == 403) it.checked = (val == 35);
            else if (it.id == 404) it.checked = (val == 50);
            else if (it.id == 405) it.checked = (val == 0);
        }
    }
    if (s->mainPanel.openSubIdx >= 0 && s->mainPanel.openSubIdx < (int)s->mainPanel.items.size()) {
        s->mainPanel.items[s->mainPanel.openSubIdx].subItems = s->subPanel.items;
    }
    if (s->subPanel.hWnd && IsWindow(s->subPanel.hWnd)) InvalidateRect(s->subPanel.hWnd, nullptr, FALSE);
    if (s->mainPanel.hWnd && IsWindow(s->mainPanel.hWnd)) InvalidateRect(s->mainPanel.hWnd, nullptr, FALSE);
}

void CommitInlineMenuEdit(FluentMenuSession* s) {
    if (!s || s->editingInlineKind == 0) return;
    if (!s->inlineEditBuf.empty() && s->inlineEditBuf != L"-") {
        int val = _wtoi(s->inlineEditBuf.c_str());
        ApplyInlineMenuFieldValue(s, s->editingInlineKind, val);
    }
    s->inlineEditReplace = false;
    s->inlineEditBuf = std::to_wstring(GetInlineMenuFieldValue(s->dlgState, s->editingInlineKind));
}

void GetInlineControlRects(const FluentMenuSession* s, const FluentMenuPanelState* p,
                           float rowTop, float rowH,
                           D2D1_RECT_F* outMinus, D2D1_RECT_F* outBox, D2D1_RECT_F* outPlus) {
    UINT dpi = s->effectiveDpi;
    float sbReserve = PanelNeedsScroll(p) ? DpiScaleF(8.0f, dpi) : 0.0f;
    float rightEdge = (float)p->width - DpiScaleF(8.0f, dpi) - sbReserve;
    float btnW = DpiScaleF(24.0f, dpi);
    float boxW = DpiScaleF(66.0f, dpi);
    float gap  = DpiScaleF(4.0f, dpi);
    float ctlH = DpiScaleF(24.0f, dpi);
    float top  = rowTop + (rowH - ctlH) * 0.5f;
    float bot  = top + ctlH;
    if (outPlus)  *outPlus  = D2D1::RectF(rightEdge - btnW, top, rightEdge, bot);
    if (outBox)   *outBox   = D2D1::RectF(rightEdge - btnW - gap - boxW, top, rightEdge - btnW - gap, bot);
    if (outMinus) *outMinus = D2D1::RectF(rightEdge - btnW - gap - boxW - gap - btnW, top,
                                          rightEdge - btnW - gap - boxW - gap, bot);
}

void ComputeFluentPanelMetrics(FluentMenuSession* s, FluentMenuPanelState* p) {
    int rowH = GetMenuRowHeight(s);
    int sepH = GetMenuSeparatorHeight(s);
    int padY = GetMenuPadY(s);

    float maxTextW = DpiScaleF(160.0f, s->effectiveDpi);
    if (g_pDWriteFactory) {
        IDWriteTextFormat* pFmt = nullptr;
        if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
                L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.0f, s->effectiveDpi), L"", &pFmt))) {
            for (const auto& item : p->items) {
                if (item.isSeparator || item.label.empty()) continue;
                IDWriteTextLayout* pLayout = nullptr;
                if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                        item.label.c_str(), (UINT32)item.label.size(), pFmt, 1000.0f, (float)rowH, &pLayout))) {
                    DWRITE_TEXT_METRICS tm = {};
                    if (SUCCEEDED(pLayout->GetMetrics(&tm))) {
                        maxTextW = (std::max)(maxTextW, tm.widthIncludingTrailingWhitespace);
                    }
                    pLayout->Release();
                }
            }
            pFmt->Release();
        }
    }

    bool hasInlineInputs = false;
    for (const auto& item : p->items) {
        if (item.inlineEditKind != 0) {
            hasInlineInputs = true;
            break;
        }
    }
    // Left icon margin (42px) + text width + right chevron/padding (or 140px for inline input controls)
    int extraRight = PanelNeedsScroll(p) ? DpiScale(44, s->effectiveDpi) : DpiScale(38, s->effectiveDpi);
    if (hasInlineInputs) {
        extraRight += DpiScale(136, s->effectiveDpi);
    }
    p->width = DpiScale(42, s->effectiveDpi) + (int)std::ceil(maxTextW) + extraRight;
    p->width = std::clamp(p->width, DpiScale(210, s->effectiveDpi), DpiScale(460, s->effectiveDpi));

    int totalH = padY * 2;
    if (PanelNeedsScroll(p)) {
        totalH += p->maxVisibleRows * rowH;
    } else {
        for (const auto& item : p->items) {
            totalH += item.isSeparator ? sepH : rowH;
        }
    }
    p->height = totalH;
}

int HitTestFluentMenuRow(const FluentMenuSession* s, const FluentMenuPanelState* p, POINT pt, RECT* outRowRect = nullptr) {
    if (pt.x < 0 || pt.x >= p->width || pt.y < 0 || pt.y >= p->height) return -1;
    int padY = GetMenuPadY(s);
    int rowH = GetMenuRowHeight(s);
    int sepH = GetMenuSeparatorHeight(s);

    int curY = padY;
    int startIdx = PanelNeedsScroll(p) ? p->scrollOffset : 0;
    int endIdx = PanelNeedsScroll(p)
        ? (std::min)((int)p->items.size(), startIdx + p->maxVisibleRows)
        : (int)p->items.size();

    for (int i = startIdx; i < endIdx; ++i) {
        int itemH = p->items[i].isSeparator ? sepH : rowH;
        if (pt.y >= curY && pt.y < curY + itemH) {
            if (p->items[i].isSeparator) return -1;
            if (outRowRect) {
                *outRowRect = { 0, curY, p->width, curY + itemH };
            }
            return i;
        }
        curY += itemH;
    }
    return -1;
}

int GetRowTopYInPanel(const FluentMenuSession* s, const FluentMenuPanelState* p, int targetIdx) {
    int padY = GetMenuPadY(s);
    int rowH = GetMenuRowHeight(s);
    int sepH = GetMenuSeparatorHeight(s);
    int curY = padY;
    int startIdx = PanelNeedsScroll(p) ? p->scrollOffset : 0;
    for (int i = startIdx; i < targetIdx && i < (int)p->items.size(); ++i) {
        curY += p->items[i].isSeparator ? sepH : rowH;
    }
    return curY;
}

void ApplyFluentMenuWindowBackdrop(HWND hWnd, const FluentMenuSession* s) {
    if (!hWnd || !s) return;

    BOOL dark = (s->surface == SurfaceTheme::Glass) ? TRUE : (s->isLight ? FALSE : TRUE);
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    DWORD corner = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    COLORREF borderCol = (s->surface == SurfaceTheme::Glass)
        ? 0xFFFFFFFE
        : (s->isLight ? RGB(204, 208, 218) : RGB(62, 66, 80));
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &borderCol, sizeof(borderCol));

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return;
    auto pSetWCA = reinterpret_cast<SetWindowCompositionAttribute_t>(
        GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
    if (!pSetWCA) return;

    ACCENT_POLICY policy = {};
    if (s->surface == SurfaceTheme::Solid || IsHighContrastActive()) {
        policy.AccentState = ACCENT_DISABLED;
    } else if (s->surface == SurfaceTheme::Glass) {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        int glassPct = (s->dlgState) ? std::clamp(s->dlgState->settings.glassOpacity, -100, 100) : -100;
        DWORD alpha = (glassPct >= 0) ? (DWORD)std::clamp(MulDiv(glassPct, 180, 100), 6, 215) : 3;
        DWORD bgr   = 0x001E1A16;
        policy.GradientColor = (alpha << 24) | bgr;
    } else {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.AccentFlags = 2;
        DWORD alpha = s->isLight ? 185 : 162;
        DWORD bgr   = s->isLight ? 0x00F8F5F4 : 0x002A1816;
        policy.GradientColor = (alpha << 24) | bgr;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) {
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
    }

    WINDOWCOMPOSITIONATTRIBDATA data = {};
    data.Attrib = 19;
    data.pvData = &policy;
    data.cbData = sizeof(policy);
    pSetWCA(hWnd, &data);
}

void ShowSubMenuForParentRow(FluentMenuSession* s, int parentIdx) {
    if (!s) return;
    if (parentIdx < 0 || parentIdx >= (int)s->mainPanel.items.size() ||
        s->mainPanel.items[parentIdx].subItems.empty()) {
        s->mainPanel.openSubIdx = -1;
        if (s->subPanel.hWnd && IsWindow(s->subPanel.hWnd)) {
            ShowWindow(s->subPanel.hWnd, SW_HIDE);
        }
        if (s->mainPanel.hWnd) InvalidateRect(s->mainPanel.hWnd, nullptr, FALSE);
        return;
    }

    if (s->mainPanel.openSubIdx == parentIdx && s->subPanel.hWnd && IsWindowVisible(s->subPanel.hWnd)) {
        return;
    }

    s->mainPanel.openSubIdx = parentIdx;
    s->subPanel.items = s->mainPanel.items[parentIdx].subItems;
    s->subPanel.hoveredIdx = -1;
    s->subPanel.pressedIdx = -1;
    s->subPanel.openSubIdx = -1;
    s->subPanel.scrollOffset = 0;
    s->subPanel.maxVisibleRows = (parentIdx == 0) ? 16 : 12;

    // Auto-scroll to checked item if list is scrollable (e.g. Display Language submenu)
    if (PanelNeedsScroll(&s->subPanel)) {
        for (int i = 0; i < (int)s->subPanel.items.size(); ++i) {
            if (s->subPanel.items[i].checked) {
                int maxScroll = (int)s->subPanel.items.size() - s->subPanel.maxVisibleRows;
                s->subPanel.scrollOffset = std::clamp(i - s->subPanel.maxVisibleRows / 2, 0, maxScroll);
                break;
            }
        }
    }

    ComputeFluentPanelMetrics(s, &s->subPanel);

    RECT rcMain = {};
    GetWindowRect(s->mainPanel.hWnd, &rcMain);

    HMONITOR hMon = MonitorFromWindow(s->mainPanel.hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    RECT rcWork = {};
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        rcWork = mi.rcWork;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    }

    int rowTopOffset = GetRowTopYInPanel(s, &s->mainPanel, parentIdx);
    int subX = rcMain.right - DpiScale(2, s->effectiveDpi);
    if (subX + s->subPanel.width > rcWork.right - DpiScale(8, s->effectiveDpi)) {
        subX = rcMain.left - s->subPanel.width + DpiScale(2, s->effectiveDpi);
    }
    int subY = rcMain.top + rowTopOffset - GetMenuPadY(s);
    if (subY + s->subPanel.height > rcWork.bottom - DpiScale(8, s->effectiveDpi)) {
        subY = rcWork.bottom - s->subPanel.height - DpiScale(8, s->effectiveDpi);
    }
    if (subY < rcWork.top + DpiScale(8, s->effectiveDpi)) {
        subY = rcWork.top + DpiScale(8, s->effectiveDpi);
    }

    SetWindowPos(s->subPanel.hWnd, HWND_TOPMOST, subX, subY,
                 s->subPanel.width, s->subPanel.height,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
    ApplyFluentMenuWindowBackdrop(s->subPanel.hWnd, s);
    InvalidateRect(s->subPanel.hWnd, nullptr, TRUE);
    InvalidateRect(s->mainPanel.hWnd, nullptr, FALSE);
}

void PaintFluentMenuPanel(HWND hWnd, FluentMenuPanelState* p) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    if (!p || !p->session || !g_pD2DFactory || !g_pDWriteFactory) {
        EndPaint(hWnd, &ps);
        return;
    }

    FluentMenuSession* s = p->session;
    RECT rcClient = {};
    GetClientRect(hWnd, &rcClient);
    int w = rcClient.right;
    int h = rcClient.bottom;
    UINT dpi = s->effectiveDpi;
    bool isLight = s->isLight;
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

                // 1. Liquid Glass / Acrylic / Solid Backdrop
                if (highContrast) {
                    COLORREF winBg = GetSysColor(COLOR_WINDOW);
                    pRT->Clear(D2D1::ColorF(GetRValue(winBg) / 255.0f, GetGValue(winBg) / 255.0f, GetBValue(winBg) / 255.0f, 1.0f));
                } else if (s->surface == SurfaceTheme::Solid) {
                    pRT->Clear(isLight ? D2D1::ColorF(0.96f, 0.96f, 0.98f, 1.0f)
                                       : D2D1::ColorF(0.11f, 0.12f, 0.16f, 1.0f));
                } else if (s->surface == SurfaceTheme::Glass) {
                    int glassPct = (s->dlgState) ? std::clamp(s->dlgState->settings.glassOpacity, -100, 100) : -100;
                    float menuAlpha = (glassPct >= 0)
                        ? (0.04f + (glassPct / 100.0f) * 0.48f)
                        : 0.01f;
                    pRT->Clear(isLight ? D2D1::ColorF(0.92f, 0.94f, 0.97f, menuAlpha)
                                       : D2D1::ColorF(0.06f, 0.08f, 0.11f, menuAlpha));
                    int slot = (hWnd == s->mainPanel.hWnd) ? 1 : 2;
                    DrawOpticalLiquidGlassBackdrop(pRT, hWnd, w, h, dpi, glassPct, isLight, true, slot);
                } else {
                    pRT->Clear(isLight ? D2D1::ColorF(246.0f / 255.0f, 246.0f / 255.0f, 250.0f / 255.0f, 145.0f / 255.0f)
                                       : D2D1::ColorF(32.0f / 255.0f, 33.0f / 255.0f, 38.0f / 255.0f, 140.0f / 255.0f));
                }

                if (!highContrast && s->surface == SurfaceTheme::Glass) {
                    DrawD2DPointerSpecularGlow(pRT, p->mousePt, p->hasMousePt, w, h, dpi, isLight);
                }

                // 2. Brushes & Text Format
                D2D1_COLOR_F colText   = isLight ? D2D1::ColorF(0.08f, 0.09f, 0.12f, 1.0f) : D2D1::ColorF(0.96f, 0.97f, 0.99f, 1.0f);
                D2D1_COLOR_F colIcon   = isLight ? D2D1::ColorF(0.16f, 0.20f, 0.28f, 0.90f) : D2D1::ColorF(0.84f, 0.90f, 0.98f, 0.92f);
                D2D1_COLOR_F colSub    = isLight ? D2D1::ColorF(0.22f, 0.25f, 0.32f, 0.72f) : D2D1::ColorF(0.82f, 0.86f, 0.94f, 0.72f);
                D2D1_COLOR_F colMuted  = isLight ? D2D1::ColorF(0.40f, 0.42f, 0.46f, 0.42f) : D2D1::ColorF(0.60f, 0.63f, 0.70f, 0.38f);
                D2D1_COLOR_F colAccent = isLight ? D2D1::ColorF(0.07f, 0.43f, 0.80f, 1.0f) : D2D1::ColorF(0.38f, 0.72f, 1.00f, 1.0f);

                ID2D1SolidColorBrush* pBrText = nullptr;
                ID2D1SolidColorBrush* pBrIcon = nullptr;
                ID2D1SolidColorBrush* pBrSub = nullptr;
                ID2D1SolidColorBrush* pBrMuted = nullptr;
                ID2D1SolidColorBrush* pBrAccent = nullptr;
                pRT->CreateSolidColorBrush(colText, &pBrText);
                pRT->CreateSolidColorBrush(colIcon, &pBrIcon);
                pRT->CreateSolidColorBrush(colSub, &pBrSub);
                pRT->CreateSolidColorBrush(colMuted, &pBrMuted);
                pRT->CreateSolidColorBrush(colAccent, &pBrAccent);

                ID2D1StrokeStyle* pRoundStroke = nullptr;
                D2D1_STROKE_STYLE_PROPERTIES strokeProps = D2D1::StrokeStyleProperties(
                    D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND);
                g_pD2DFactory->CreateStrokeStyle(strokeProps, nullptr, 0, &pRoundStroke);

                IDWriteTextFormat* pFmtItem = nullptr;
                g_pDWriteFactory->CreateTextFormat(
                    L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.0f, dpi), L"", &pFmtItem);
                if (pFmtItem) {
                    pFmtItem->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    pFmtItem->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                }

                int padY = GetMenuPadY(s);
                int rowH = GetMenuRowHeight(s);
                int sepH = GetMenuSeparatorHeight(s);
                float curY = (float)padY;
                float padX = DpiScaleF(5.0f, dpi);
                float pillRadius = DpiScaleF(5.5f, dpi);
                bool hasScroll = PanelNeedsScroll(p);
                float sbReserve = hasScroll ? DpiScaleF(8.0f, dpi) : 0.0f;

                int startIdx = hasScroll ? p->scrollOffset : 0;
                int endIdx = hasScroll
                    ? (std::min)((int)p->items.size(), startIdx + p->maxVisibleRows)
                    : (int)p->items.size();

                for (int i = startIdx; i < endIdx; ++i) {
                    const auto& item = p->items[i];
                    if (item.isSeparator) {
                        ID2D1SolidColorBrush* pDivBr = nullptr;
                        D2D1_COLOR_F divCol = isLight ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.09f)
                                                      : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f);
                        if (SUCCEEDED(pRT->CreateSolidColorBrush(divCol, &pDivBr))) {
                            float lineY = curY + sepH * 0.5f;
                            float lineX = DpiScaleF(12.0f, dpi);
                            pRT->DrawLine(D2D1::Point2F(lineX, lineY),
                                          D2D1::Point2F((float)w - lineX, lineY),
                                          pDivBr, DpiScaleF(1.0f, dpi));
                            pDivBr->Release();
                        }
                        curY += sepH;
                        continue;
                    }

                    bool isHovered = (i == p->hoveredIdx || i == p->openSubIdx) && !item.disabled;
                    bool isPressed = (i == p->pressedIdx) && !item.disabled;

                    if (isHovered || isPressed) {
                        D2D1_RECT_F rcPill = D2D1::RectF(
                            padX, curY + DpiScaleF(1.5f, dpi),
                            (float)w - padX - sbReserve, curY + rowH - DpiScaleF(1.5f, dpi));
                        if (s->surface == SurfaceTheme::Glass && !highContrast) {
                            D2D1_COLOR_F rowTint = isLight
                                ? D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.28f : 0.16f)
                                : D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.18f : 0.10f);
                            DrawD2DGlassButton(pRT, rcPill, pillRadius, dpi, rowTint, isHovered, isPressed, isLight);
                        } else {
                            ID2D1SolidColorBrush* pHlBr = nullptr;
                            D2D1_COLOR_F hlCol = isLight
                                ? D2D1::ColorF(0.0f, 0.0f, 0.0f, isPressed ? 0.11f : 0.065f)
                                : D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.15f : 0.095f);
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(hlCol, &pHlBr))) {
                                pRT->FillRoundedRectangle(D2D1::RoundedRect(rcPill, pillRadius, pillRadius), pHlBr);
                                pHlBr->Release();
                            }
                            ID2D1SolidColorBrush* pHlBorder = nullptr;
                            D2D1_COLOR_F hlBCol = isLight
                                ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.06f)
                                : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.08f);
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(hlBCol, &pHlBorder))) {
                                pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcPill, pillRadius, pillRadius),
                                                          pHlBorder, DpiScaleF(0.8f, dpi));
                                pHlBorder->Release();
                            }
                        }
                    }

                    float iconCx = DpiScaleF(22.0f, dpi);
                    float iconCy = curY + rowH * 0.5f;
                    ID2D1Brush* pItemTextBr = item.disabled ? pBrMuted : pBrText;
                    ID2D1Brush* pItemIconBr = item.disabled ? pBrMuted : (item.checked && pBrAccent ? pBrAccent : pBrIcon);

                    if (item.checked && item.icon == FluentMenuIcon::None && pBrAccent && pRoundStroke) {
                        DrawD2DMenuCheckmark(pRT, pBrAccent, pRoundStroke, iconCx, iconCy, dpi);
                    } else if (item.icon != FluentMenuIcon::None && pItemIconBr && pRoundStroke) {
                        DrawD2DFluentMenuIcon(pRT, pItemIconBr, pRoundStroke, item.icon, iconCx, iconCy, dpi);
                    }

                    if (item.inlineEditKind != 0) {
                        D2D1_RECT_F rcMinus = {}, rcBox = {}, rcPlus = {};
                        GetInlineControlRects(s, p, curY, (float)rowH, &rcMinus, &rcBox, &rcPlus);

                        if (pFmtItem && pItemTextBr && !item.label.empty()) {
                            D2D1_RECT_F rcText = D2D1::RectF(
                                DpiScaleF(42.0f, dpi),
                                curY,
                                rcMinus.left - DpiScaleF(6.0f, dpi),
                                curY + rowH);
                            pRT->DrawText(item.label.c_str(), (UINT32)item.label.size(),
                                          pFmtItem, rcText, pItemTextBr, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                        }

                        bool isEditingThis = (s->editingInlineKind == item.inlineEditKind);
                        float ctlRad = DpiScaleF(5.0f, dpi);

                        bool hovMinus = isHovered && p->hasMousePt &&
                                        p->mousePt.x >= rcMinus.left && p->mousePt.x <= rcMinus.right &&
                                        p->mousePt.y >= rcMinus.top && p->mousePt.y <= rcMinus.bottom;
                        bool hovPlus  = isHovered && p->hasMousePt &&
                                        p->mousePt.x >= rcPlus.left && p->mousePt.x <= rcPlus.right &&
                                        p->mousePt.y >= rcPlus.top && p->mousePt.y <= rcPlus.bottom;
                        bool hovBox   = isHovered && p->hasMousePt &&
                                        p->mousePt.x >= rcBox.left && p->mousePt.x <= rcBox.right &&
                                        p->mousePt.y >= rcBox.top && p->mousePt.y <= rcBox.bottom;

                        bool prsMinus = isPressed && (s->pressedInlinePart == 1);
                        bool prsPlus  = isPressed && (s->pressedInlinePart == 3);

                        D2D1_COLOR_F btnTint = isLight
                            ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.08f)
                            : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.14f);
                        DrawD2DGlassButton(pRT, rcMinus, ctlRad, dpi, btnTint, hovMinus, prsMinus, isLight);
                        DrawD2DGlassButton(pRT, rcPlus,  ctlRad, dpi, btnTint, hovPlus,  prsPlus,  isLight);

                        // Input box background & focus border
                        if (s->surface == SurfaceTheme::Glass && !highContrast) {
                            D2D1_COLOR_F boxTint = isEditingThis
                                ? (isLight ? D2D1::ColorF(0.07f, 0.45f, 0.88f, 0.18f) : D2D1::ColorF(0.14f, 0.54f, 0.98f, 0.24f))
                                : (isLight ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.06f) : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f));
                            DrawD2DGlassButton(pRT, rcBox, ctlRad, dpi, boxTint, hovBox || isEditingThis, false, isLight);
                        } else {
                            D2D1_COLOR_F boxFill = isEditingThis
                                ? (isLight ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.88f) : D2D1::ColorF(0.05f, 0.10f, 0.18f, 0.78f))
                                : (isLight ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.58f) : D2D1::ColorF(0.06f, 0.08f, 0.12f, 0.52f));
                            ID2D1SolidColorBrush* pBoxFillBr = nullptr;
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(boxFill, &pBoxFillBr))) {
                                pRT->FillRoundedRectangle(D2D1::RoundedRect(rcBox, ctlRad, ctlRad), pBoxFillBr);
                                pBoxFillBr->Release();
                            }
                            ID2D1Brush* pBoxBorderBr = (isEditingThis || hovBox) ? (ID2D1Brush*)pBrAccent : (ID2D1Brush*)pBrSub;
                            if (pBoxBorderBr) {
                                pRT->DrawRoundedRectangle(D2D1::RoundedRect(rcBox, ctlRad, ctlRad),
                                                          pBoxBorderBr, DpiScaleF(isEditingThis ? 1.6f : 1.0f, dpi));
                            }
                        }

                        IDWriteTextFormat* pFmtCtl = nullptr;
                        if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
                                L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.0f, dpi), L"", &pFmtCtl))) {
                            pFmtCtl->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtCtl->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

                            if (pBrText) {
                                pRT->DrawText(L"−", 1, pFmtCtl, rcMinus, pBrText);
                                pRT->DrawText(L"+", 1, pFmtCtl, rcPlus,  pBrText);

                                wchar_t valStr[32] = {};
                                if (isEditingThis) {
                                    swprintf_s(valStr, L"%s|", s->inlineEditBuf.c_str());
                                } else {
                                    int curVal = GetInlineMenuFieldValue(s->dlgState, item.inlineEditKind);
                                    swprintf_s(valStr, L"%d", curVal);
                                }
                                pRT->DrawText(valStr, (UINT32)wcslen(valStr), pFmtCtl, rcBox,
                                              isEditingThis && pBrAccent ? pBrAccent : pBrText);
                            }
                            pFmtCtl->Release();
                        }
                    } else {
                        if (pFmtItem && pItemTextBr && !item.label.empty()) {
                            D2D1_RECT_F rcText = D2D1::RectF(
                                DpiScaleF(42.0f, dpi),
                                curY,
                                (float)w - DpiScaleF(26.0f, dpi) - sbReserve,
                                curY + rowH);
                            pRT->DrawText(item.label.c_str(), (UINT32)item.label.size(),
                                          pFmtItem, rcText, pItemTextBr, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                        }

                        if (!item.subItems.empty() && pBrSub && pRoundStroke) {
                            float chevX = (float)w - DpiScaleF(16.0f, dpi) - sbReserve;
                            DrawD2DMenuChevronRight(pRT, pBrSub, pRoundStroke, chevX, iconCy, dpi);
                        } else if (item.checked && item.icon != FluentMenuIcon::None && pBrAccent && pRoundStroke) {
                            float chkX = (float)w - DpiScaleF(17.0f, dpi) - sbReserve;
                            DrawD2DMenuCheckmark(pRT, pBrAccent, pRoundStroke, chkX, iconCy, dpi);
                        }
                    }

                    curY += rowH;
                }

                // Scrollbar for long submenus (such as 22 languages)
                if (hasScroll) {
                    float trackW = DpiScaleF(4.5f, dpi);
                    float rightPad = DpiScaleF(4.0f, dpi);
                    float trackTop = (float)padY + DpiScaleF(4.0f, dpi);
                    float trackBot = (float)h - padY - DpiScaleF(4.0f, dpi);
                    float trackH = (std::max)(10.0f, trackBot - trackTop);
                    float thumbH = (std::max)(DpiScaleF(24.0f, dpi),
                                              trackH * ((float)p->maxVisibleRows / (float)p->items.size()));
                    int maxScroll = (std::max)(1, (int)p->items.size() - p->maxVisibleRows);
                    float thumbTop = trackTop + (trackH - thumbH) * ((float)p->scrollOffset / (float)maxScroll);

                    D2D1_RECT_F rcThumb = D2D1::RectF((float)w - rightPad - trackW, thumbTop,
                                                      (float)w - rightPad, thumbTop + thumbH);
                    ID2D1SolidColorBrush* pThumbBr = nullptr;
                    if (SUCCEEDED(pRT->CreateSolidColorBrush(
                            isLight ? D2D1::ColorF(0.2f, 0.24f, 0.3f, 0.42f)
                                    : D2D1::ColorF(0.9f, 0.92f, 0.98f, 0.38f), &pThumbBr))) {
                        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcThumb, trackW * 0.5f, trackW * 0.5f), pThumbBr);
                        pThumbBr->Release();
                    }
                }

                // Outer Rim / Border
                if (s->surface == SurfaceTheme::Glass && !highContrast) {
                    float rimInset = DpiScaleF(0.6f, dpi);
                    D2D1_RECT_F rcRim = D2D1::RectF(rimInset, rimInset, (float)w - rimInset, (float)h - rimInset);
                    DrawD2DGlassRim(pRT, rcRim, DpiScaleF(8.0f, dpi), dpi, isLight, 1.15f);
                }

                pRT->EndDraw();

                if (pFmtItem)     pFmtItem->Release();
                if (pRoundStroke) pRoundStroke->Release();
                if (pBrAccent)    pBrAccent->Release();
                if (pBrMuted)     pBrMuted->Release();
                if (pBrSub)       pBrSub->Release();
                if (pBrIcon)      pBrIcon->Release();
                if (pBrText)      pBrText->Release();
                pRT->Release();
            }

            BitBlt(hdc, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
            SelectObject(hMemDC, oldBmp);
            DeleteObject(hBmp);
        }
        DeleteDC(hMemDC);
    }

    EndPaint(hWnd, &ps);
}

LRESULT CALLBACK FluentMenuWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto* p = reinterpret_cast<FluentMenuPanelState*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    switch (uMsg) {
        case WM_NCCREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            PaintFluentMenuPanel(hWnd, p);
            return 0;

        case WM_MOUSEMOVE: {
            if (!p || !p->session) return 0;
            FluentMenuSession* s = p->session;
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            p->mousePt = pt;
            p->hasMousePt = true;

            int row = HitTestFluentMenuRow(s, p, pt);
            if (row != p->hoveredIdx) {
                p->hoveredIdx = row;
                if (!p->isSubMenu) {
                    s->pendingHoverSubIdx = row;
                    SetTimer(hWnd, kTimerMenuSubHover, 65, nullptr);
                }
            }
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
            TrackMouseEvent(&tme);
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }

        case WM_TIMER: {
            if (wParam == kTimerMenuSubHover && p && p->session && !p->isSubMenu) {
                KillTimer(hWnd, kTimerMenuSubHover);
                FluentMenuSession* s = p->session;
                // Only switch/close submenu if mouse is NOT currently inside the open submenu
                if (s->subPanel.hWnd && IsWindowVisible(s->subPanel.hWnd)) {
                    POINT ptScreen = {};
                    GetCursorPos(&ptScreen);
                    RECT rcSub = {};
                    GetWindowRect(s->subPanel.hWnd, &rcSub);
                    if (PtInRect(&rcSub, ptScreen)) {
                        p->hoveredIdx = p->openSubIdx;
                        InvalidateRect(hWnd, nullptr, FALSE);
                        return 0;
                    }
                }
                ShowSubMenuForParentRow(s, s->pendingHoverSubIdx);
                return 0;
            }
            if (wParam == kTimerMenuWatchdog && p && p->session && !p->isSubMenu) {
                FluentMenuSession* s = p->session;
                HWND hDlg = s->dlgState ? s->dlgState->hDlg : nullptr;
                if (!hDlg || !IsWindow(hDlg) || !IsWindowVisible(hDlg) || IsIconic(hDlg) ||
                    s->dlgState->pendingUninit || g_unloading.load()) {
                    s->running = false;
                    PostMessageW(hWnd, WM_NULL, 0, 0);
                    return 0;
                }

                HWND hFg = GetForegroundWindow();
                if (hFg == hDlg || hFg == s->mainPanel.hWnd || hFg == s->subPanel.hWnd) {
                    s->wasForeground = true;
                } else if (s->wasForeground && hFg != nullptr) {
                    s->running = false;
                    PostMessageW(hWnd, WM_NULL, 0, 0);
                    return 0;
                }

                bool anyMouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
                                    (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 ||
                                    (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
                if (!anyMouseDown) {
                    s->mouseReleasedSinceOpen = true;
                } else if (s->mouseReleasedSinceOpen) {
                    POINT ptCursor = {};
                    GetCursorPos(&ptCursor);
                    RECT rcMain = {}, rcSub = {};
                    GetWindowRect(s->mainPanel.hWnd, &rcMain);
                    bool inMain = PtInRect(&rcMain, ptCursor);
                    bool inSub  = s->subPanel.hWnd && IsWindowVisible(s->subPanel.hWnd) &&
                                  GetWindowRect(s->subPanel.hWnd, &rcSub) && PtInRect(&rcSub, ptCursor);
                    if (!inMain && !inSub) {
                        s->running = false;
                        PostMessageW(hWnd, WM_NULL, 0, 0);
                        return 0;
                    }
                }
                return 0;
            }
            break;
        }

        case WM_MOUSELEAVE: {
            if (!p || !p->session) return 0;
            p->hasMousePt = false;
            p->pressedIdx = -1;
            if (p->isSubMenu) {
                p->hoveredIdx = -1;
            } else {
                // Keep parent item highlighted if mouse entered its open submenu
                POINT ptScreen = {};
                GetCursorPos(&ptScreen);
                RECT rcSub = {};
                if (p->session->subPanel.hWnd && IsWindowVisible(p->session->subPanel.hWnd) &&
                    GetWindowRect(p->session->subPanel.hWnd, &rcSub) && PtInRect(&rcSub, ptScreen)) {
                    p->hoveredIdx = p->openSubIdx;
                } else {
                    p->hoveredIdx = -1;
                }
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }

        case WM_MOUSEWHEEL: {
            if (p && p->session && PanelNeedsScroll(p)) {
                short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                int step = (zDelta > 0) ? -1 : 1;
                if (std::abs(zDelta) >= WHEEL_DELTA * 2) step *= 2;
                int maxScroll = (std::max)(0, (int)p->items.size() - p->maxVisibleRows);
                int newOff = std::clamp(p->scrollOffset + step, 0, maxScroll);
                if (newOff != p->scrollOffset) {
                    p->scrollOffset = newOff;
                    if (p->hasMousePt) {
                        p->hoveredIdx = HitTestFluentMenuRow(p->session, p, p->mousePt);
                    }
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            if (!p || !p->session) return 0;
            FluentMenuSession* s = p->session;
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            RECT rcRow = {};
            int row = HitTestFluentMenuRow(s, p, pt, &rcRow);
            s->pressedInlinePart = 0;
            if (row >= 0 && row < (int)p->items.size() && !p->items[row].disabled) {
                p->pressedIdx = row;
                const auto& item = p->items[row];
                if (item.inlineEditKind != 0) {
                    D2D1_RECT_F rcMinus = {}, rcBox = {}, rcPlus = {};
                    GetInlineControlRects(s, p, (float)rcRow.top, (float)(rcRow.bottom - rcRow.top),
                                          &rcMinus, &rcBox, &rcPlus);
                    if (pt.x >= rcMinus.left && pt.x <= rcMinus.right) s->pressedInlinePart = 1;
                    else if (pt.x >= rcPlus.left && pt.x <= rcPlus.right) s->pressedInlinePart = 3;
                    else s->pressedInlinePart = 2;
                } else if (!p->isSubMenu && !item.subItems.empty()) {
                    ShowSubMenuForParentRow(s, row);
                }
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (!p || !p->session) return 0;
            FluentMenuSession* s = p->session;
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            RECT rcRow = {};
            int row = HitTestFluentMenuRow(s, p, pt, &rcRow);
            int pressedPart = s->pressedInlinePart;
            s->pressedInlinePart = 0;
            p->pressedIdx = -1;
            InvalidateRect(hWnd, nullptr, FALSE);

            if (row >= 0 && row < (int)p->items.size() && !p->items[row].disabled) {
                const auto& item = p->items[row];
                if (item.inlineEditKind != 0) {
                    D2D1_RECT_F rcMinus = {}, rcBox = {}, rcPlus = {};
                    GetInlineControlRects(s, p, (float)rcRow.top, (float)(rcRow.bottom - rcRow.top),
                                          &rcMinus, &rcBox, &rcPlus);
                    int releasePart = 2;
                    if (pt.x >= rcMinus.left && pt.x <= rcMinus.right) releasePart = 1;
                    else if (pt.x >= rcPlus.left && pt.x <= rcPlus.right) releasePart = 3;

                    if (s->editingInlineKind != 0 && s->editingInlineKind != item.inlineEditKind) {
                        CommitInlineMenuEdit(s);
                    }

                    auto getInlineStep = [](int k) -> int {
                        if (k == 1 || k == 5 || k == 6) return 10;
                        if (k == 4 || k == 7) return 5;
                        if (k == 2) return 2;
                        return 1;
                    };
                    if (releasePart == 1 && (pressedPart == 1 || pressedPart == 0)) {
                        if (s->editingInlineKind == item.inlineEditKind) CommitInlineMenuEdit(s);
                        int curVal = GetInlineMenuFieldValue(s->dlgState, item.inlineEditKind);
                        int step = getInlineStep(item.inlineEditKind);
                        if (GetAsyncKeyState(VK_SHIFT) & 0x8000) step *= 2;
                        ApplyInlineMenuFieldValue(s, item.inlineEditKind, curVal - step);
                        s->editingInlineKind = item.inlineEditKind;
                        s->inlineEditBuf = std::to_wstring(GetInlineMenuFieldValue(s->dlgState, item.inlineEditKind));
                        s->inlineEditReplace = true;
                    } else if (releasePart == 3 && (pressedPart == 3 || pressedPart == 0)) {
                        if (s->editingInlineKind == item.inlineEditKind) CommitInlineMenuEdit(s);
                        int curVal = GetInlineMenuFieldValue(s->dlgState, item.inlineEditKind);
                        int step = getInlineStep(item.inlineEditKind);
                        if (GetAsyncKeyState(VK_SHIFT) & 0x8000) step *= 2;
                        ApplyInlineMenuFieldValue(s, item.inlineEditKind, curVal + step);
                        s->editingInlineKind = item.inlineEditKind;
                        s->inlineEditBuf = std::to_wstring(GetInlineMenuFieldValue(s->dlgState, item.inlineEditKind));
                        s->inlineEditReplace = true;
                    } else {
                        s->editingInlineKind = item.inlineEditKind;
                        s->inlineEditBuf = std::to_wstring(GetInlineMenuFieldValue(s->dlgState, item.inlineEditKind));
                        s->inlineEditReplace = true;
                    }
                    // Ensure keyboard focus is on the dialog's edit control so typing from keyboard always reaches GetMessageW
                    if (s->dlgState && s->dlgState->hDlg) {
                        if (GetForegroundWindow() != s->dlgState->hDlg) {
                            SetForegroundWindow(s->dlgState->hDlg);
                        }
                        if (s->dlgState->hModernEdit && IsWindow(s->dlgState->hModernEdit)) {
                            SetFocus(s->dlgState->hModernEdit);
                        } else {
                            SetFocus(s->dlgState->hDlg);
                        }
                    }
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
                if (s->editingInlineKind != 0) {
                    CommitInlineMenuEdit(s);
                    s->editingInlineKind = 0;
                }
                if (!item.subItems.empty()) {
                    ShowSubMenuForParentRow(s, row);
                } else if (item.id != 0) {
                    s->selectedCmd = item.id;
                    s->running = false;
                }
            }
            return 0;
        }

        case WM_KEYDOWN: {
            if (p && p->session && wParam == VK_ESCAPE) {
                p->session->running = false;
                return 0;
            }
            break;
        }

        case WM_ACTIVATEAPP: {
            if (wParam == FALSE && p && p->session) {
                p->session->running = false;
                PostMessageW(hWnd, WM_NULL, 0, 0);
            }
            return 0;
        }

        case WM_CLOSE: {
            if (p && p->session) {
                p->session->running = false;
                PostMessageW(hWnd, WM_NULL, 0, 0);
            }
            return 0;
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

UINT RunFluentPopupMenu(RunDialogState* state, std::vector<FluentMenuItem>&& menuItems, POINT ptAnchorRightBottom) {
    if (!state || !state->hDlg || !g_fluentMenuClassRegistered) return 0;

    FluentMenuSession session;
    session.dlgState = state;
    session.dpi = state->dpi;
    session.effectiveDpi = MulDiv(state->dpi, GetDlgScale(state), 100);
    session.surface = state->effectiveSurface;
    session.isLight = state ? state->isLightMode : false;
    session.mouseReleasedSinceOpen =
        (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0 &&
        (GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0 &&
        (GetAsyncKeyState(VK_MBUTTON) & 0x8000) == 0;
    session.wasForeground = (GetForegroundWindow() == state->hDlg);

    session.mainPanel.items = std::move(menuItems);
    session.mainPanel.isSubMenu = false;
    session.mainPanel.session = &session;
    session.subPanel.isSubMenu = true;
    session.subPanel.session = &session;

    ComputeFluentPanelMetrics(&session, &session.mainPanel);

    HMONITOR hMon = MonitorFromWindow(state->hDlg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    RECT rcWork = {};
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        rcWork = mi.rcWork;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    }

    int mainX = ptAnchorRightBottom.x - session.mainPanel.width;
    int mainY = ptAnchorRightBottom.y - session.mainPanel.height - DpiScale(6, session.effectiveDpi);
    int margin = DpiScale(8, session.effectiveDpi);
    mainX = std::clamp(mainX, (int)rcWork.left + margin, (int)rcWork.right - session.mainPanel.width - margin);
    mainY = std::clamp(mainY, (int)rcWork.top + margin, (int)rcWork.bottom - session.mainPanel.height - margin);

    HWND hMainWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        kFluentMenuClassName, L"",
        WS_POPUP,
        mainX, mainY, session.mainPanel.width, session.mainPanel.height,
        state->hDlg, nullptr, GetModInstanceHandle(), &session.mainPanel);

    HWND hSubWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        kFluentMenuClassName, L"",
        WS_POPUP,
        0, 0, 10, 10,
        state->hDlg, nullptr, GetModInstanceHandle(), &session.subPanel);

    session.mainPanel.hWnd = hMainWnd;
    session.subPanel.hWnd = hSubWnd;
    g_hActiveGearMenu.store(hMainWnd);

    ApplyFluentMenuWindowBackdrop(hMainWnd, &session);
    ApplyFluentMenuWindowBackdrop(hSubWnd, &session);

    ShowWindow(hMainWnd, SW_SHOWNOACTIVATE);
    InvalidateRect(hMainWnd, nullptr, TRUE);
    if (state->hDlg && IsWindow(state->hDlg)) {
        if (GetForegroundWindow() != state->hDlg) {
            SetForegroundWindow(state->hDlg);
        }
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            SetFocus(state->hModernEdit);
        }
    }
    SetTimer(hMainWnd, kTimerMenuWatchdog, 30, nullptr);

    MSG msg = {};
    while (session.running && !g_unloading.load() && !state->pendingUninit &&
           IsWindow(state->hDlg) && IsWindowVisible(state->hDlg) && !IsIconic(state->hDlg) &&
           GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if ((msg.message == g_msgUninit && g_msgUninit != 0) ||
            (msg.message == g_msgToggle && g_msgToggle != 0 && msg.wParam != kActionReloadSettings)) {
            session.running = false;
            if (IsWindow(hSubWnd)) ShowWindow(hSubWnd, SW_HIDE);
            if (IsWindow(hMainWnd)) ShowWindow(hMainWnd, SW_HIDE);
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            break;
        }

        // Close menu when clicking outside both menu panels
        if (msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN ||
            msg.message == WM_MBUTTONDOWN || msg.message == WM_NCLBUTTONDOWN ||
            msg.message == WM_NCRBUTTONDOWN) {
            POINT ptClick = msg.pt;
            RECT rcMain = {}, rcSub = {};
            GetWindowRect(hMainWnd, &rcMain);
            bool inMain = PtInRect(&rcMain, ptClick);
            bool inSub  = IsWindowVisible(hSubWnd) && GetWindowRect(hSubWnd, &rcSub) && PtInRect(&rcSub, ptClick);
            if (!inMain && !inSub) {
                session.running = false;
                if (IsWindow(hSubWnd)) ShowWindow(hSubWnd, SW_HIDE);
                if (IsWindow(hMainWnd)) ShowWindow(hMainWnd, SW_HIDE);
                // Swallow click if it was on the Gear button itself so it toggles closed instead of re-opening
                ActionButtonsLayout layout = GetActionButtonsLayout(state);
                POINT ptClient = ptClick;
                ScreenToClient(state->hDlg, &ptClient);
                if (!PtInRect(&layout.rcGear, ptClient)) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
                break;
            }
        }

        // Route mouse wheel to visible submenu if cursor is inside submenu, else main menu
        if (msg.message == WM_MOUSEWHEEL) {
            POINT ptCursor = {};
            GetCursorPos(&ptCursor);
            RECT rcSub = {};
            if (IsWindowVisible(hSubWnd) && GetWindowRect(hSubWnd, &rcSub) && PtInRect(&rcSub, ptCursor)) {
                SendMessageW(hSubWnd, WM_MOUSEWHEEL, msg.wParam, msg.lParam);
                continue;
            }
        }

        auto tryLiveApplyInlineBuf = [&]() {
            if (session.inlineEditBuf.empty() || session.inlineEditBuf == L"-") return;
            int liveVal = _wtoi(session.inlineEditBuf.c_str());
            int k = session.editingInlineKind;
            if ((k == 1 && liveVal >= 360 && liveVal <= 1200) ||
                (k == 2 && liveVal >= 36 && liveVal <= 96) ||
                (k == 3 && liveVal >= 9 && liveVal <= 24) ||
                (k == 4 && liveVal >= -100 && liveVal <= 100) ||
                (k == 5 && liveVal >= 0 && liveVal <= 255) ||
                (k == 6 && liveVal >= 20 && liveVal <= 255) ||
                (k == 7 && liveVal >= 0 && liveVal <= 50) ||
                (k == 8 && liveVal >= 3 && liveVal <= 10)) {
                ApplyInlineMenuFieldValue(&session, k, liveVal);
            }
        };

        auto handleInlineCharInput = [&](wchar_t ch) {
            if (ch == L'-') {
                if (session.editingInlineKind == 4) {
                    if (session.inlineEditReplace || session.inlineEditBuf.empty()) {
                        session.inlineEditBuf = L"-";
                        session.inlineEditReplace = false;
                    } else if (session.inlineEditBuf[0] == L'-') {
                        session.inlineEditBuf.erase(0, 1);
                        tryLiveApplyInlineBuf();
                    } else {
                        session.inlineEditBuf = L"-" + session.inlineEditBuf;
                        tryLiveApplyInlineBuf();
                    }
                    if (IsWindow(hSubWnd)) InvalidateRect(hSubWnd, nullptr, FALSE);
                }
                return;
            }
            if (ch >= L'0' && ch <= L'9') {
                if (session.inlineEditReplace) {
                    session.inlineEditBuf.clear();
                    session.inlineEditReplace = false;
                }
                size_t maxLen = (!session.inlineEditBuf.empty() && session.inlineEditBuf[0] == L'-') ? 4 : 4;
                if (session.inlineEditBuf.size() < maxLen) {
                    session.inlineEditBuf.push_back(ch);
                }
                tryLiveApplyInlineBuf();
                if (IsWindow(hSubWnd)) InvalidateRect(hSubWnd, nullptr, FALSE);
            }
        };

        // Handle WM_CHAR for IME / Unikey / EVKey injected digits or minus sign when inline box is focused
        if ((msg.message == WM_CHAR || msg.message == WM_IME_CHAR) && session.editingInlineKind != 0) {
            wchar_t ch = (wchar_t)msg.wParam;
            if ((ch >= L'0' && ch <= L'9') || ch == L'-') {
                handleInlineCharInput(ch);
            } else if (ch == VK_BACK) {
                if (session.inlineEditReplace) {
                    session.inlineEditBuf.clear();
                    session.inlineEditReplace = false;
                } else if (!session.inlineEditBuf.empty()) {
                    session.inlineEditBuf.pop_back();
                    tryLiveApplyInlineBuf();
                }
                if (IsWindow(hSubWnd)) InvalidateRect(hSubWnd, nullptr, FALSE);
            }
            continue;
        }

        // Keyboard navigation & inline input box typing
        if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN) {
            if (session.editingInlineKind != 0) {
                if (msg.wParam == VK_PACKET) {
                    TranslateMessage(&msg);
                    continue;
                }
                wchar_t digit = 0;
                if (msg.wParam >= '0' && msg.wParam <= '9' && (GetKeyState(VK_SHIFT) & 0x8000) == 0) {
                    digit = (wchar_t)msg.wParam;
                } else if (msg.wParam >= VK_NUMPAD0 && msg.wParam <= VK_NUMPAD9) {
                    digit = (wchar_t)(L'0' + (msg.wParam - VK_NUMPAD0));
                }
                if (digit != 0) {
                    handleInlineCharInput(digit);
                    continue;
                }
                if (msg.wParam == VK_OEM_MINUS || msg.wParam == VK_SUBTRACT) {
                    handleInlineCharInput(L'-');
                    continue;
                }
                if (msg.wParam == VK_BACK || msg.wParam == VK_DELETE) {
                    if (session.inlineEditReplace) {
                        session.inlineEditBuf.clear();
                        session.inlineEditReplace = false;
                    } else if (!session.inlineEditBuf.empty()) {
                        session.inlineEditBuf.pop_back();
                        tryLiveApplyInlineBuf();
                    }
                    if (IsWindow(hSubWnd)) InvalidateRect(hSubWnd, nullptr, FALSE);
                    continue;
                }
                if (msg.wParam == VK_UP || msg.wParam == VK_DOWN) {
                    CommitInlineMenuEdit(&session);
                    int k = session.editingInlineKind;
                    int curVal = GetInlineMenuFieldValue(session.dlgState, k);
                    int step = (k == 1 || k == 5 || k == 6) ? 10 : ((k == 4 || k == 7) ? 5 : ((k == 2) ? 2 : 1));
                    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) step *= 2;
                    int nextVal = (msg.wParam == VK_UP) ? (curVal + step) : (curVal - step);
                    ApplyInlineMenuFieldValue(&session, k, nextVal);
                    session.inlineEditBuf = std::to_wstring(GetInlineMenuFieldValue(session.dlgState, k));
                    session.inlineEditReplace = true;
                    if (IsWindow(hSubWnd)) InvalidateRect(hSubWnd, nullptr, FALSE);
                    continue;
                }
                if (msg.wParam == VK_RETURN) {
                    CommitInlineMenuEdit(&session);
                    session.inlineEditReplace = true;
                    if (IsWindow(hSubWnd)) InvalidateRect(hSubWnd, nullptr, FALSE);
                    continue;
                }
                if (msg.wParam == VK_TAB) {
                    CommitInlineMenuEdit(&session);
                    std::vector<int> subInlineKinds;
                    for (const auto& it : session.subPanel.items) {
                        if (it.inlineEditKind != 0) subInlineKinds.push_back(it.inlineEditKind);
                    }
                    if (!subInlineKinds.empty()) {
                        auto itPos = std::find(subInlineKinds.begin(), subInlineKinds.end(), session.editingInlineKind);
                        size_t nextIdx = (itPos != subInlineKinds.end())
                                         ? ((std::distance(subInlineKinds.begin(), itPos) + 1) % subInlineKinds.size())
                                         : 0;
                        session.editingInlineKind = subInlineKinds[nextIdx];
                        session.inlineEditBuf = std::to_wstring(GetInlineMenuFieldValue(session.dlgState, session.editingInlineKind));
                        session.inlineEditReplace = true;
                    }
                    if (IsWindow(hSubWnd)) InvalidateRect(hSubWnd, nullptr, FALSE);
                    continue;
                }
                if (msg.wParam == VK_ESCAPE) {
                    session.editingInlineKind = 0;
                    session.inlineEditBuf.clear();
                    if (IsWindow(hSubWnd)) InvalidateRect(hSubWnd, nullptr, FALSE);
                    continue;
                }
            }
            if (msg.wParam == VK_ESCAPE) {
                session.running = false;
                break;
            }
            if (msg.wParam == VK_DOWN || msg.wParam == VK_UP) {
                bool inSub = IsWindowVisible(hSubWnd) && session.subPanel.hoveredIdx >= 0;
                FluentMenuPanelState* activeP = inSub ? &session.subPanel : &session.mainPanel;
                int n = (int)activeP->items.size();
                if (n > 0) {
                    int step = (msg.wParam == VK_DOWN) ? 1 : -1;
                    int cur = activeP->hoveredIdx;
                    for (int tries = 0; tries < n; ++tries) {
                        cur = (cur + step + n) % n;
                        if (!activeP->items[cur].isSeparator && !activeP->items[cur].disabled) {
                            activeP->hoveredIdx = cur;
                            break;
                        }
                    }
                    if (PanelNeedsScroll(activeP) && activeP->hoveredIdx >= 0) {
                        if (activeP->hoveredIdx < activeP->scrollOffset) {
                            activeP->scrollOffset = activeP->hoveredIdx;
                        } else if (activeP->hoveredIdx >= activeP->scrollOffset + activeP->maxVisibleRows) {
                            activeP->scrollOffset = activeP->hoveredIdx - activeP->maxVisibleRows + 1;
                        }
                    }
                    InvalidateRect(activeP->hWnd, nullptr, FALSE);
                }
                continue;
            }
            if (msg.wParam == VK_RIGHT) {
                int idx = session.mainPanel.hoveredIdx;
                if (idx >= 0 && idx < (int)session.mainPanel.items.size() &&
                    !session.mainPanel.items[idx].subItems.empty()) {
                    ShowSubMenuForParentRow(&session, idx);
                    session.subPanel.hoveredIdx = 0;
                    InvalidateRect(hSubWnd, nullptr, FALSE);
                }
                continue;
            }
            if (msg.wParam == VK_LEFT) {
                if (IsWindowVisible(hSubWnd)) {
                    ShowSubMenuForParentRow(&session, -1);
                }
                continue;
            }
            if (msg.wParam == VK_RETURN) {
                if (IsWindowVisible(hSubWnd) && session.subPanel.hoveredIdx >= 0 &&
                    session.subPanel.hoveredIdx < (int)session.subPanel.items.size()) {
                    const auto& subIt = session.subPanel.items[session.subPanel.hoveredIdx];
                    if (!subIt.disabled && subIt.id != 0) {
                        session.selectedCmd = subIt.id;
                        session.running = false;
                        break;
                    }
                } else if (session.mainPanel.hoveredIdx >= 0 &&
                           session.mainPanel.hoveredIdx < (int)session.mainPanel.items.size()) {
                    const auto& mainIt = session.mainPanel.items[session.mainPanel.hoveredIdx];
                    if (!mainIt.subItems.empty()) {
                        ShowSubMenuForParentRow(&session, session.mainPanel.hoveredIdx);
                        session.subPanel.hoveredIdx = 0;
                        InvalidateRect(hSubWnd, nullptr, FALSE);
                    } else if (!mainIt.disabled && mainIt.id != 0) {
                        session.selectedCmd = mainIt.id;
                        session.running = false;
                        break;
                    }
                }
                continue;
            }
        }

        if (msg.message == WM_ACTIVATEAPP && msg.wParam == FALSE) {
            session.running = false;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            break;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (!IsWindow(state->hDlg) || !IsWindowVisible(state->hDlg) || IsIconic(state->hDlg) || state->pendingUninit) {
            session.running = false;
            break;
        }
    }

    if (session.editingInlineKind != 0) {
        CommitInlineMenuEdit(&session);
        session.editingInlineKind = 0;
    }
    g_hActiveGearMenu.store(nullptr);
    if (IsWindow(hMainWnd)) KillTimer(hMainWnd, kTimerMenuWatchdog);
    if (IsWindow(hSubWnd)) DestroyWindow(hSubWnd);
    if (IsWindow(hMainWnd)) DestroyWindow(hMainWnd);

    return session.selectedCmd;
}

struct CustomSizeDialogSession {
    RunDialogState* dlgState = nullptr;
    HWND hWnd = nullptr;
    UINT dpi = 96;
    bool isLight = false;
    SurfaceTheme surface = SurfaceTheme::Glass;
    bool running = true;
    int hoveredBtn = -1;
    int pressedBtn = -1;
    int activeField = 0; // 0 = Width, 1 = Height, 2 = Font
    std::wstring typingBuf;
    POINT mousePt = { -1, -1 };
    bool hasMousePt = false;
    bool mouseReleasedSinceOpen = false;
    bool wasForeground = false;
};

int GetEffectiveCustomWidthDip(const RunDialogState* state) {
    if (!state) return kBaseDlgWidth;
    if (state->settings.customDialogWidth >= 360) return std::clamp(state->settings.customDialogWidth, 360, 1200);
    return MulDiv(kBaseDlgWidth, GetDlgScale(state), 100);
}

int GetEffectiveCustomBarHeightDip(const RunDialogState* state) {
    if (!state) return kBaseInputBarHeight;
    if (state->settings.customBarHeight > 0) return std::clamp(state->settings.customBarHeight, 36, 96);
    return MulDiv(kBaseInputBarHeight, GetDlgScale(state), 100);
}

int GetEffectiveCustomFontPt(const RunDialogState* state) {
    if (!state) return 14;
    if (state->settings.customFontSize > 0) return std::clamp(state->settings.customFontSize, 9, 24);
    return (int)std::round(13.5f * GetDlgScale(state) / 100.0f);
}

void ApplyLiveCustomDimensionChange(RunDialogState* state, int newW, int newH, int newF) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    if (newW >= 0) Wh_SetIntValue(GetUserOverrideKey(L"customDialogWidth").c_str(), newW);
    if (newH >= 0) Wh_SetIntValue(GetUserOverrideKey(L"customBarHeight").c_str(), newH);
    if (newF >= 0) Wh_SetIntValue(GetUserOverrideKey(L"customFontSize").c_str(), newF);
    LoadSettings(false);
    ApplyReloadedSettingsToDialog(state);
}

// Button IDs in CustomSizeDialog:
// Row 0 (Width):  10 = -20, 11 = -5, 12 = Value Box, 13 = +5, 14 = +20
// Row 1 (Height): 20 = -4,  21 = -1, 22 = Value Box, 23 = +1, 24 = +4
// Row 2 (Font):   30 = -2,  31 = -1, 32 = Value Box, 33 = +1, 34 = +2
// Bottom:         40 = Reset, 41 = Done
RECT GetCustomSizeBtnRect(int btnId, int w, int h, UINT dpi) {
    int padX = DpiScale(16, dpi);
    int rowStartY = DpiScale(46, dpi);
    int rowStepY  = DpiScale(42, dpi);
    int btnH      = DpiScale(28, dpi);
    int wSmall    = DpiScale(34, dpi);
    int wVal      = DpiScale(72, dpi);
    int gap       = DpiScale(5, dpi);

    int rightEdge = w - padX;
    int x4 = rightEdge - wSmall;
    int x3 = x4 - gap - wSmall;
    int x2 = x3 - gap - wVal;
    int x1 = x2 - gap - wSmall;
    int x0 = x1 - gap - wSmall;

    if (btnId >= 10 && btnId <= 34) {
        int r = (btnId / 10) - 1;
        int c = btnId % 10;
        int top = rowStartY + r * rowStepY;
        if (c == 0) return { x0, top, x0 + wSmall, top + btnH };
        if (c == 1) return { x1, top, x1 + wSmall, top + btnH };
        if (c == 2) return { x2, top, x2 + wVal,   top + btnH };
        if (c == 3) return { x3, top, x3 + wSmall, top + btnH };
        if (c == 4) return { x4, top, x4 + wSmall, top + btnH };
    }
    int botY = h - DpiScale(42, dpi);
    int botH = DpiScale(30, dpi);
    if (btnId == 40) {
        return { padX, botY, padX + DpiScale(130, dpi), botY + botH };
    }
    if (btnId == 41) {
        return { w - padX - DpiScale(100, dpi), botY, w - padX, botY + botH };
    }
    return { 0, 0, 0, 0 };
}

int HitTestCustomSizeBtn(POINT pt, int w, int h, UINT dpi) {
    const int ids[] = { 10, 11, 12, 13, 14, 20, 21, 22, 23, 24, 30, 31, 32, 33, 34, 40, 41 };
    for (int id : ids) {
        RECT rc = GetCustomSizeBtnRect(id, w, h, dpi);
        if (PtInRect(&rc, pt)) return id;
    }
    return -1;
}

void CommitCustomSizeTypingBuf(CustomSizeDialogSession* s) {
    if (!s || s->typingBuf.empty() || !s->dlgState) return;
    int val = _wtoi(s->typingBuf.c_str());
    s->typingBuf.clear();
    if (val <= 0) return;
    if (s->activeField == 0) {
        ApplyLiveCustomDimensionChange(s->dlgState, std::clamp(val, 360, 1200), -1, -1);
    } else if (s->activeField == 1) {
        if (val >= 150) {
            val = MulDiv(val, kBaseInputBarHeight, kBaseInputBarHeight + 5 * kBaseRowHeight + kBaseListPadTop + kBaseListPadBottom);
        }
        ApplyLiveCustomDimensionChange(s->dlgState, -1, std::clamp(val, 36, 96), -1);
    } else if (s->activeField == 2) {
        ApplyLiveCustomDimensionChange(s->dlgState, -1, -1, std::clamp(val, 9, 24));
    }
}

void PaintCustomSizeDialog(HWND hWnd, CustomSizeDialogSession* s) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    if (!s || !s->dlgState || !g_pD2DFactory || !g_pDWriteFactory) {
        EndPaint(hWnd, &ps);
        return;
    }

    RECT rcClient = {};
    GetClientRect(hWnd, &rcClient);
    int w = rcClient.right;
    int h = rcClient.bottom;
    UINT dpi = s->dpi;
    bool isLight = s->isLight;
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

                if (highContrast) {
                    COLORREF winBg = GetSysColor(COLOR_WINDOW);
                    pRT->Clear(D2D1::ColorF(GetRValue(winBg) / 255.0f, GetGValue(winBg) / 255.0f, GetBValue(winBg) / 255.0f, 1.0f));
                } else if (s->surface == SurfaceTheme::Solid) {
                    pRT->Clear(isLight ? D2D1::ColorF(0.96f, 0.96f, 0.98f, 1.0f)
                                       : D2D1::ColorF(0.11f, 0.12f, 0.16f, 1.0f));
                } else if (s->surface == SurfaceTheme::Glass) {
                    pRT->Clear(isLight ? D2D1::ColorF(0.98f, 0.98f, 1.00f, 0.18f)
                                       : D2D1::ColorF(0.92f, 0.94f, 1.00f, 0.10f));
                    DrawD2DPointerSpecularGlow(pRT, s->mousePt, s->hasMousePt, w, h, dpi, isLight);
                } else {
                    pRT->Clear(isLight ? D2D1::ColorF(246.0f / 255.0f, 246.0f / 255.0f, 250.0f / 255.0f, 150.0f / 255.0f)
                                       : D2D1::ColorF(32.0f / 255.0f, 33.0f / 255.0f, 38.0f / 255.0f, 145.0f / 255.0f));
                }

                D2D1_COLOR_F colText = isLight ? D2D1::ColorF(0.08f, 0.09f, 0.12f, 1.0f) : D2D1::ColorF(0.96f, 0.97f, 0.99f, 1.0f);
                D2D1_COLOR_F colSub  = isLight ? D2D1::ColorF(0.20f, 0.23f, 0.30f, 0.78f) : D2D1::ColorF(0.84f, 0.88f, 0.95f, 0.78f);
                ID2D1SolidColorBrush* pBrText = nullptr;
                ID2D1SolidColorBrush* pBrSub  = nullptr;
                pRT->CreateSolidColorBrush(colText, &pBrText);
                pRT->CreateSolidColorBrush(colSub,  &pBrSub);

                IDWriteTextFormat* pFmtTitle = nullptr;
                IDWriteTextFormat* pFmtLabel = nullptr;
                IDWriteTextFormat* pFmtBtn   = nullptr;
                g_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.5f, dpi), L"", &pFmtTitle);
                g_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.5f, dpi), L"", &pFmtLabel);
                g_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.0f, dpi), L"", &pFmtBtn);

                if (pFmtTitle) pFmtTitle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                if (pFmtLabel) pFmtLabel->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                if (pFmtBtn) {
                    pFmtBtn->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    pFmtBtn->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                }

                MenuLocalizedStrings ml = GetMenuLocalizedStrings(s->dlgState->settings.language);
                float padX = DpiScaleF(16.0f, dpi);
                if (pFmtTitle && pBrText) {
                    const wchar_t* title = ml.customSizeDlgTitle;
                    D2D1_RECT_F rcTitle = D2D1::RectF(padX, DpiScaleF(10.0f, dpi), (float)w - padX, DpiScaleF(38.0f, dpi));
                    pRT->DrawText(title, (UINT32)wcslen(title), pFmtTitle, rcTitle, pBrText);
                }

                const wchar_t* rowLabels[3] = {
                    ml.widthPx,
                    ml.heightPx,
                    ml.fontSizePt
                };

                int curVals[3] = {
                    GetEffectiveCustomWidthDip(s->dlgState),
                    GetEffectiveCustomBarHeightDip(s->dlgState),
                    GetEffectiveCustomFontPt(s->dlgState)
                };
                const wchar_t* stepLabels[3][4] = {
                    { L"-20", L"-5", L"+5", L"+20" },
                    { L"-4",  L"-1", L"+1", L"+4"  },
                    { L"-2",  L"-1", L"+1", L"+2"  }
                };

                auto drawPillBtn = [&](int id, const wchar_t* text, bool isValueBox, bool isPrimary, bool isFocusedField) {
                    RECT rWin = GetCustomSizeBtnRect(id, w, h, dpi);
                    D2D1_RECT_F rc = D2D1::RectF((float)rWin.left, (float)rWin.top, (float)rWin.right, (float)rWin.bottom);
                    float rad = DpiScaleF(6.0f, dpi);
                    bool hov = (s->hoveredBtn == id);
                    bool prs = (s->pressedBtn == id);

                    D2D1_COLOR_F tint = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.08f);
                    if (isPrimary) {
                        tint = D2D1::ColorF(0.10f, 0.48f, 0.88f, prs ? 0.55f : (hov ? 0.44f : 0.34f));
                    } else if (isValueBox && isFocusedField) {
                        tint = D2D1::ColorF(0.14f, 0.52f, 0.92f, 0.24f);
                    } else if (hov || prs) {
                        tint = isLight
                            ? D2D1::ColorF(0.0f, 0.0f, 0.0f, prs ? 0.16f : 0.09f)
                            : D2D1::ColorF(1.0f, 1.0f, 1.0f, prs ? 0.22f : 0.14f);
                    }
                    DrawD2DGlassButton(pRT, rc, rad, dpi, tint, hov || isFocusedField, prs, isLight);
                    if (pFmtBtn && pBrText && text) {
                        pRT->DrawText(text, (UINT32)wcslen(text), pFmtBtn, rc, pBrText);
                    }
                };

                for (int r = 0; r < 3; ++r) {
                    int baseId = (r + 1) * 10;
                    RECT rcFirst = GetCustomSizeBtnRect(baseId, w, h, dpi);
                    D2D1_RECT_F rcLbl = D2D1::RectF(padX, (float)rcFirst.top, (float)rcFirst.left - DpiScaleF(8.0f, dpi), (float)rcFirst.bottom);
                    if (pFmtLabel && pBrSub) {
                        pRT->DrawText(rowLabels[r], (UINT32)wcslen(rowLabels[r]), pFmtLabel, rcLbl, pBrSub);
                    }

                    drawPillBtn(baseId + 0, stepLabels[r][0], false, false, false);
                    drawPillBtn(baseId + 1, stepLabels[r][1], false, false, false);

                    wchar_t valBuf[32] = {};
                    if (s->activeField == r && !s->typingBuf.empty()) {
                        swprintf_s(valBuf, L"%s_", s->typingBuf.c_str());
                    } else {
                        swprintf_s(valBuf, r == 2 ? L"%d pt" : L"%d px", curVals[r]);
                    }
                    drawPillBtn(baseId + 2, valBuf, true, false, s->activeField == r);

                    drawPillBtn(baseId + 3, stepLabels[r][2], false, false, false);
                    drawPillBtn(baseId + 4, stepLabels[r][3], false, false, false);
                }

                drawPillBtn(40, ml.resetDefaultBtn, false, false, false);
                drawPillBtn(41, ml.doneOkBtn, false, true, false);

                if (s->surface == SurfaceTheme::Glass && !highContrast) {
                    float rimInset = DpiScaleF(0.6f, dpi);
                    D2D1_RECT_F rcRim = D2D1::RectF(rimInset, rimInset, (float)w - rimInset, (float)h - rimInset);
                    DrawD2DGlassRim(pRT, rcRim, DpiScaleF(8.0f, dpi), dpi, isLight, 1.4f);
                }

                pRT->EndDraw();
                if (pFmtBtn)   pFmtBtn->Release();
                if (pFmtLabel) pFmtLabel->Release();
                if (pFmtTitle) pFmtTitle->Release();
                if (pBrSub)    pBrSub->Release();
                if (pBrText)   pBrText->Release();
                pRT->Release();
            }
            BitBlt(hdc, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
            SelectObject(hMemDC, oldBmp);
            DeleteObject(hBmp);
        }
        DeleteDC(hMemDC);
    }
    EndPaint(hWnd, &ps);
}

LRESULT CALLBACK CustomSizeDialogWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto* s = reinterpret_cast<CustomSizeDialogSession*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    switch (uMsg) {
        case WM_NCCREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            PaintCustomSizeDialog(hWnd, s);
            return 0;
        case WM_MOUSEMOVE: {
            if (!s) return 0;
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            s->mousePt = pt;
            s->hasMousePt = true;
            RECT rc = {};
            GetClientRect(hWnd, &rc);
            int hit = HitTestCustomSizeBtn(pt, rc.right, rc.bottom, s->dpi);
            if (hit != s->hoveredBtn) {
                s->hoveredBtn = hit;
            }
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
            TrackMouseEvent(&tme);
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        case WM_MOUSELEAVE: {
            if (!s) return 0;
            s->hasMousePt = false;
            s->hoveredBtn = -1;
            s->pressedBtn = -1;
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (!s) return 0;
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            RECT rc = {};
            GetClientRect(hWnd, &rc);
            int hit = HitTestCustomSizeBtn(pt, rc.right, rc.bottom, s->dpi);
            s->pressedBtn = hit;
            SetCapture(hWnd);
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        case WM_LBUTTONUP: {
            if (!s) return 0;
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            RECT rc = {};
            GetClientRect(hWnd, &rc);
            int hit = HitTestCustomSizeBtn(pt, rc.right, rc.bottom, s->dpi);
            int pressed = s->pressedBtn;
            s->pressedBtn = -1;
            ReleaseCapture();

            if (hit == pressed && hit != -1 && s->dlgState) {
                CommitCustomSizeTypingBuf(s);
                int curW = GetEffectiveCustomWidthDip(s->dlgState);
                int curH = GetEffectiveCustomBarHeightDip(s->dlgState);
                int curF = GetEffectiveCustomFontPt(s->dlgState);

                if (hit == 10) { s->activeField = 0; ApplyLiveCustomDimensionChange(s->dlgState, std::clamp(curW - 20, 360, 1200), -1, -1); }
                else if (hit == 11) { s->activeField = 0; ApplyLiveCustomDimensionChange(s->dlgState, std::clamp(curW - 5, 360, 1200), -1, -1); }
                else if (hit == 12) { s->activeField = 0; s->typingBuf.clear(); }
                else if (hit == 13) { s->activeField = 0; ApplyLiveCustomDimensionChange(s->dlgState, std::clamp(curW + 5, 360, 1200), -1, -1); }
                else if (hit == 14) { s->activeField = 0; ApplyLiveCustomDimensionChange(s->dlgState, std::clamp(curW + 20, 360, 1200), -1, -1); }
                else if (hit == 20) { s->activeField = 1; ApplyLiveCustomDimensionChange(s->dlgState, -1, std::clamp(curH - 4, 36, 96), -1); }
                else if (hit == 21) { s->activeField = 1; ApplyLiveCustomDimensionChange(s->dlgState, -1, std::clamp(curH - 1, 36, 96), -1); }
                else if (hit == 22) { s->activeField = 1; s->typingBuf.clear(); }
                else if (hit == 23) { s->activeField = 1; ApplyLiveCustomDimensionChange(s->dlgState, -1, std::clamp(curH + 1, 36, 96), -1); }
                else if (hit == 24) { s->activeField = 1; ApplyLiveCustomDimensionChange(s->dlgState, -1, std::clamp(curH + 4, 36, 96), -1); }
                else if (hit == 30) { s->activeField = 2; ApplyLiveCustomDimensionChange(s->dlgState, -1, -1, std::clamp(curF - 2, 9, 24)); }
                else if (hit == 31) { s->activeField = 2; ApplyLiveCustomDimensionChange(s->dlgState, -1, -1, std::clamp(curF - 1, 9, 24)); }
                else if (hit == 32) { s->activeField = 2; s->typingBuf.clear(); }
                else if (hit == 33) { s->activeField = 2; ApplyLiveCustomDimensionChange(s->dlgState, -1, -1, std::clamp(curF + 1, 9, 24)); }
                else if (hit == 34) { s->activeField = 2; ApplyLiveCustomDimensionChange(s->dlgState, -1, -1, std::clamp(curF + 2, 9, 24)); }
                else if (hit == 40) {
                    Wh_DeleteValue(GetUserOverrideKey(L"customDialogWidth").c_str());
                    Wh_DeleteValue(GetUserOverrideKey(L"customBarHeight").c_str());
                    Wh_DeleteValue(GetUserOverrideKey(L"customFontSize").c_str());
                    LoadSettings(false);
                    ApplyReloadedSettingsToDialog(s->dlgState);
                } else if (hit == 41) {
                    s->running = false;
                }
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }
        case WM_CHAR: {
            if (!s) return 0;
            if (wParam >= L'0' && wParam <= L'9') {
                if (s->typingBuf.size() < 4) {
                    s->typingBuf.push_back((wchar_t)wParam);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
            } else if (wParam == VK_BACK) {
                if (!s->typingBuf.empty()) {
                    s->typingBuf.pop_back();
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
            }
            return 0;
        }
        case WM_KEYDOWN: {
            if (!s) return 0;
            if (wParam == VK_ESCAPE) {
                s->typingBuf.clear();
                s->running = false;
                return 0;
            }
            if (wParam == VK_RETURN) {
                if (!s->typingBuf.empty()) {
                    CommitCustomSizeTypingBuf(s);
                    InvalidateRect(hWnd, nullptr, FALSE);
                } else {
                    s->running = false;
                }
                return 0;
            }
            if (wParam == VK_TAB || wParam == VK_DOWN || wParam == VK_UP) {
                CommitCustomSizeTypingBuf(s);
                int delta = (wParam == VK_UP) ? 2 : 1;
                s->activeField = (s->activeField + delta) % 3;
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            break;
        }
        case WM_TIMER: {
            if (wParam == kTimerMenuWatchdog && s) {
                HWND hDlg = s->dlgState ? s->dlgState->hDlg : nullptr;
                if (!hDlg || !IsWindow(hDlg) || !IsWindowVisible(hDlg) || IsIconic(hDlg) ||
                    s->dlgState->pendingUninit || g_unloading.load()) {
                    s->running = false;
                    PostMessageW(hWnd, WM_NULL, 0, 0);
                    return 0;
                }
                bool anyMouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
                                    (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
                if (!anyMouseDown) {
                    s->mouseReleasedSinceOpen = true;
                } else if (s->mouseReleasedSinceOpen) {
                    POINT ptCursor = {};
                    GetCursorPos(&ptCursor);
                    RECT rcWnd = {}, rcDlg = {};
                    GetWindowRect(hWnd, &rcWnd);
                    GetWindowRect(hDlg, &rcDlg);
                    if (!PtInRect(&rcWnd, ptCursor) && !PtInRect(&rcDlg, ptCursor)) {
                        CommitCustomSizeTypingBuf(s);
                        s->running = false;
                        PostMessageW(hWnd, WM_NULL, 0, 0);
                        return 0;
                    }
                }
            }
            break;
        }
        case WM_ACTIVATEAPP: {
            if (wParam == FALSE && s) {
                CommitCustomSizeTypingBuf(s);
                s->running = false;
                PostMessageW(hWnd, WM_NULL, 0, 0);
            }
            return 0;
        }
        case WM_CLOSE: {
            if (s) {
                CommitCustomSizeTypingBuf(s);
                s->running = false;
                PostMessageW(hWnd, WM_NULL, 0, 0);
            }
            return 0;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void OpenCustomSizeDialog(RunDialogState* state) {
    if (!state || !state->hDlg || state->isBrowsingFile || state->pendingUninit || g_unloading.load()) return;
    HWND hDlg = state->hDlg;
    state->isBrowsingFile = true;
    g_isBrowsingFile.store(true);

    static constexpr wchar_t kCustomSizeClass[] = L"WindhawkModernRunCustomSizeWnd";
    WNDCLASSEXW wc = { sizeof(wc) };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = CustomSizeDialogWndProc;
    wc.hInstance = GetModInstanceHandle();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kCustomSizeClass;
    RegisterClassExW(&wc);

    CustomSizeDialogSession session;
    session.dlgState = state;
    session.dpi = state->dpi;
    session.isLight = state->isLightMode;
    session.surface = state->effectiveSurface;
    session.mouseReleasedSinceOpen = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0;

    int wndW = DpiScale(420, session.dpi);
    int wndH = DpiScale(226, session.dpi);

    RECT rcDlg = {};
    GetWindowRect(hDlg, &rcDlg);
    HMONITOR hMon = MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    RECT rcWork = {};
    if (hMon && GetMonitorInfoW(hMon, &mi)) rcWork = mi.rcWork;
    else SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);

    int posX = rcDlg.right - wndW;
    int posY = rcDlg.top - wndH - DpiScale(8, session.dpi);
    if (posY < rcWork.top + DpiScale(8, session.dpi)) {
        posY = rcDlg.bottom + DpiScale(8, session.dpi);
    }
    posX = std::clamp(posX, (int)rcWork.left + DpiScale(8, session.dpi), (int)rcWork.right - wndW - DpiScale(8, session.dpi));
    posY = std::clamp(posY, (int)rcWork.top + DpiScale(8, session.dpi), (int)rcWork.bottom - wndH - DpiScale(8, session.dpi));

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kCustomSizeClass, L"",
        WS_POPUP,
        posX, posY, wndW, wndH,
        hDlg, nullptr, GetModInstanceHandle(), &session);

    if (hWnd) {
        session.hWnd = hWnd;
        g_hActiveCustomSizeWnd.store(hWnd);

        FluentMenuSession dummyMenuSession;
        dummyMenuSession.isLight = session.isLight;
        dummyMenuSession.surface = session.surface;
        ApplyFluentMenuWindowBackdrop(hWnd, &dummyMenuSession);

        ShowWindow(hWnd, SW_SHOW);
        SetForegroundWindow(hWnd);
        SetFocus(hWnd);
        InvalidateRect(hWnd, nullptr, TRUE);
        SetTimer(hWnd, kTimerMenuWatchdog, 30, nullptr);

        MSG msg = {};
        while (session.running && !g_unloading.load() && !state->pendingUninit &&
               IsWindow(hDlg) && IsWindowVisible(hDlg) && !IsIconic(hDlg) &&
               GetMessageW(&msg, nullptr, 0, 0) > 0) {
            if (msg.message == g_msgUninit && g_msgUninit != 0) {
                session.running = false;
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        g_hActiveCustomSizeWnd.store(nullptr);
        if (IsWindow(hWnd)) {
            KillTimer(hWnd, kTimerMenuWatchdog);
            DestroyWindow(hWnd);
        }
    }

    UnregisterClassW(kCustomSizeClass, GetModInstanceHandle());
    g_isBrowsingFile.store(false);
    state->isBrowsingFile = false;

    if (IsWindow(hDlg) && IsWindowVisible(hDlg) && !IsIconic(hDlg) && !state->pendingUninit && !g_unloading.load()) {
        SetForegroundWindow(hDlg);
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            SetFocus(state->hModernEdit);
        }
        InvalidateRect(hDlg, nullptr, TRUE);
    }
}

void OpenGearSettingsMenu(RunDialogState* state) {
    if (!state || !state->hDlg || state->isBrowsingFile) return;
    HWND hDlg = state->hDlg;
    state->isBrowsingFile = true;
    g_isBrowsingFile.store(true);

    MenuLocalizedStrings ml = GetMenuLocalizedStrings(state->settings.language);

    std::vector<FluentMenuItem> menuItems;

    // 1. Surface Theme Submenu (Liquid Glass -100..100 without % unit + Windows Blur 0..255 inline input)
    {
        int curGlassOp = std::clamp(state->settings.glassOpacity, -100, 100);
        FluentMenuItem sep;
        sep.isSeparator = true;

        FluentMenuItem glassOpItem;
        glassOpItem.label = ml.glassTintRange;
        glassOpItem.icon  = FluentMenuIcon::Surface;
        glassOpItem.inlineEditKind = 4;

        FluentMenuItem blurOpItem;
        blurOpItem.label = ml.blurOpacityRange;
        blurOpItem.icon  = FluentMenuIcon::Surface;
        blurOpItem.inlineEditKind = 5;

        FluentMenuItem item;
        item.label = ml.surfaceThemeMenu;
        item.icon  = FluentMenuIcon::Surface;
        item.subItems = {
            { 101, ml.liquidGlass, FluentMenuIcon::None, state->settings.surfaceTheme == SurfaceTheme::Glass, false, false, {} },
            { 102, ml.windowsBlur, FluentMenuIcon::None, state->settings.surfaceTheme == SurfaceTheme::Blur, false, false, {} },
            { 103, ml.opaqueSolid, FluentMenuIcon::None, state->settings.surfaceTheme == SurfaceTheme::Solid, false, false, {} },
            sep,
            glassOpItem,
            { 107, ml.glassPresetM100, FluentMenuIcon::None, curGlassOp <= -90, false, false, {} },
            { 108, ml.glassPresetM80,  FluentMenuIcon::None, curGlassOp > -90 && curGlassOp <= -65, false, false, {} },
            { 109, ml.glassPresetM50,  FluentMenuIcon::None, curGlassOp > -65 && curGlassOp <= -25, false, false, {} },
            { 110, ml.glassPreset0,    FluentMenuIcon::None, curGlassOp > -25 && curGlassOp <= 5, false, false, {} },
            { 111, ml.glassPreset15,   FluentMenuIcon::None, curGlassOp > 5 && curGlassOp <= 20, false, false, {} },
            { 112, ml.glassPreset30,   FluentMenuIcon::None, curGlassOp > 20 && curGlassOp <= 42, false, false, {} },
            { 113, ml.glassPreset55,   FluentMenuIcon::None, curGlassOp > 42 && curGlassOp <= 68, false, false, {} },
            { 114, ml.glassPreset80,   FluentMenuIcon::None, curGlassOp > 68, false, false, {} },
            sep,
            blurOpItem
        };
        menuItems.push_back(std::move(item));
    }

    // 2. Color Mode Submenu
    {
        FluentMenuItem item;
        item.label = ml.colorModeMenu;
        item.icon  = FluentMenuIcon::ColorMode;
        item.subItems = {
            { 201, ml.colorAuto,  FluentMenuIcon::None, state->settings.colorMode == ColorMode::Auto, false, false, {} },
            { 202, ml.colorDark,  FluentMenuIcon::None, state->settings.colorMode == ColorMode::Dark, false, false, {} },
            { 203, ml.colorLight, FluentMenuIcon::None, state->settings.colorMode == ColorMode::Light, false, false, {} }
        };
        menuItems.push_back(std::move(item));
    }

    // 3. UI Size / Scale Submenu (Presets 85%–120% + Direct Inline Input Boxes for Width / Height / Font Size)
    {
        int curScale = state->settings.uiScalePercent;
        bool hasManualSize = (state->settings.customDialogWidth > 0) ||
                             (state->settings.customBarHeight > 0) ||
                             (state->settings.customFontSize > 0);
        FluentMenuItem sep;
        sep.isSeparator = true;

        FluentMenuItem wItem;
        wItem.label = ml.widthPx;
        wItem.icon  = FluentMenuIcon::Scale;
        wItem.inlineEditKind = 1;

        FluentMenuItem hItem;
        hItem.label = ml.heightPx;
        hItem.icon  = FluentMenuIcon::Scale;
        hItem.inlineEditKind = 2;

        FluentMenuItem fItem;
        fItem.label = ml.fontSizePt;
        fItem.icon  = FluentMenuIcon::Scale;
        fItem.inlineEditKind = 3;

        FluentMenuItem item;
        item.label = ml.uiSizeMenu;
        item.icon  = FluentMenuIcon::Scale;
        item.subItems = {
            { 301, ml.scale85,  FluentMenuIcon::None, !hasManualSize && curScale == 85, false, false, {} },
            { 302, ml.scale90,  FluentMenuIcon::None, !hasManualSize && curScale == 90, false, false, {} },
            { 303, ml.scale100, FluentMenuIcon::None, !hasManualSize && curScale == 100, false, false, {} },
            { 304, ml.scale110, FluentMenuIcon::None, !hasManualSize && curScale == 110, false, false, {} },
            { 305, ml.scale120, FluentMenuIcon::None, !hasManualSize && curScale == 120, false, false, {} },
            sep,
            wItem,
            hItem,
            fItem,
            { 311, ml.resetCustomSize, FluentMenuIcon::RemoveBg, false, false, false, {} }
        };
        menuItems.push_back(std::move(item));
    }

    // 3b. Minimal Mode direct toggle on main menu
    {
        FluentMenuItem minItem;
        minItem.id      = 350;
        minItem.label   = ml.minimalModeMenu;
        minItem.icon    = FluentMenuIcon::MinimalLayout;
        minItem.checked = state->settings.minimalMode;
        menuItems.push_back(std::move(minItem));
    }

    // 4. Custom Background Media Submenu (with inline keyboard input for Media Opacity 20-255)
    {
        int curBgOp = state->settings.customBackgroundOpacity;
        FluentMenuItem sep;
        sep.isSeparator = true;

        FluentMenuItem bgOpItem;
        bgOpItem.label = ml.mediaOpacityRange;
        bgOpItem.icon  = FluentMenuIcon::Background;
        bgOpItem.inlineEditKind = 6;

        FluentMenuItem item;
        item.label = ml.customBgMenu;
        item.icon  = FluentMenuIcon::Background;
        item.subItems = {
            { 501, ml.selectBgFile,  FluentMenuIcon::FolderPick, false, false, false, {} },
            { 502, ml.clearCustomBg, FluentMenuIcon::RemoveBg, false, state->settings.customBackgroundPath.empty(), false, {} },
            sep,
            bgOpItem,
            { 503, ml.bgOpacity90,  FluentMenuIcon::None, curBgOp <= 95, false, false, {} },
            { 504, ml.bgOpacity150, FluentMenuIcon::None, curBgOp > 95 && curBgOp <= 175, false, false, {} },
            { 505, ml.bgOpacity215, FluentMenuIcon::None, curBgOp > 175 && curBgOp <= 230, false, false, {} },
            { 506, ml.bgOpacity255, FluentMenuIcon::None, curBgOp > 230, false, false, {} }
        };
        menuItems.push_back(std::move(item));
    }

    // 5. History Capacity & Scrollable Viewport Submenu (with inline keyboard inputs for Max Items & Visible Rows)
    {
        int curMaxH = state->settings.maxHistoryItems;
        FluentMenuItem sep;
        sep.isSeparator = true;

        FluentMenuItem maxHistItem;
        maxHistItem.label = ml.maxStoredRange;
        maxHistItem.icon  = FluentMenuIcon::History;
        maxHistItem.inlineEditKind = 7;

        FluentMenuItem visRowsItem;
        visRowsItem.label = ml.visibleRowsRange;
        visRowsItem.icon  = FluentMenuIcon::History;
        visRowsItem.inlineEditKind = 8;

        FluentMenuItem item;
        item.label = ml.historyMenu;
        item.icon  = FluentMenuIcon::History;
        item.subItems = {
            maxHistItem,
            visRowsItem,
            sep,
            { 401, ml.maxHist10,      FluentMenuIcon::None, curMaxH == 10, false, false, {} },
            { 402, ml.maxHist30,      FluentMenuIcon::None, curMaxH == 30, false, false, {} },
            { 403, ml.maxHist35,      FluentMenuIcon::None, curMaxH == 35, false, false, {} },
            { 404, ml.maxHist50,      FluentMenuIcon::None, curMaxH == 50, false, false, {} },
            { 405, ml.maxHist0,       FluentMenuIcon::None, curMaxH == 0, false, false, {} },
            sep,
            { 406, ml.showTimestamp,  FluentMenuIcon::None, state->settings.showTimestamp, false, false, {} },
            { 407, ml.appSuggestions, FluentMenuIcon::None, state->settings.enableAppSuggestions, false, false, {} }
        };
        menuItems.push_back(std::move(item));
    }

    // 6. Display Language Submenu
    struct LangOption { UINT id; const wchar_t* code; const wchar_t* label; };
    static constexpr LangOption kLangOpts[] = {
        { 600, L"auto",  nullptr },
        { 601, L"vi",    L"Tiếng Việt (Vietnamese)" },
        { 602, L"en",    L"English" },
        { 603, L"zh_cn", L"简体中文 (Simplified Chinese)" },
        { 604, L"zh_tw", L"繁體中文 (Traditional Chinese)" },
        { 605, L"ja",    L"日本語 (Japanese)" },
        { 606, L"ko",    L"한국어 (Korean)" },
        { 607, L"th",    L"ไทย (Thai)" },
        { 608, L"id",    L"Bahasa Indonesia (Indonesian)" },
        { 609, L"ms",    L"Bahasa Melayu (Malay)" },
        { 610, L"fr",    L"Français (French)" },
        { 611, L"de",    L"Deutsch (German)" },
        { 612, L"es",    L"Español (Spanish)" },
        { 613, L"pt",    L"Português (Portuguese)" },
        { 614, L"it",    L"Italiano (Italian)" },
        { 615, L"ru",    L"Русский (Russian)" },
        { 616, L"uk",    L"Українська (Ukrainian)" },
        { 617, L"pl",    L"Polski (Polish)" },
        { 618, L"nl",    L"Nederlands (Dutch)" },
        { 619, L"tr",    L"Türkçe (Turkish)" },
        { 620, L"hi",    L"हिन्दी (Hindi)" },
        { 621, L"ar",    L"العربية (Arabic)" }
    };
    {
        FluentMenuItem item;
        item.label = ml.displayLanguageMenu;
        item.icon  = FluentMenuIcon::Language;
        for (const auto& opt : kLangOpts) {
            const wchar_t* lbl = opt.label ? opt.label : ml.autoLanguage;
            item.subItems.push_back({ opt.id, lbl, FluentMenuIcon::None, state->settings.language == opt.code, false, false, {} });
        }
        menuItems.push_back(std::move(item));
    }

    // Separator + Clear History Item
    {
        FluentMenuItem sep;
        sep.isSeparator = true;
        menuItems.push_back(std::move(sep));

        FluentMenuItem clearItem;
        clearItem.id    = 701;
        clearItem.label = ml.clearAllHistory;
        clearItem.icon  = FluentMenuIcon::ClearTrash;
        menuItems.push_back(std::move(clearItem));
    }

    ActionButtonsLayout layout = GetActionButtonsLayout(state);
    POINT ptMenu = { layout.rcGear.right, layout.rcGear.top };
    ClientToScreen(hDlg, &ptMenu);

    UINT cmd = RunFluentPopupMenu(state, std::move(menuItems), ptMenu);

    if (g_hActiveRunDlg.load() != hDlg || !IsWindow(hDlg)) {
        g_isBrowsingFile.store(false);
        return;
    }

    if (state->pendingUninit || g_unloading.load() || !IsWindowVisible(hDlg)) {
        g_isBrowsingFile.store(false);
        state->isBrowsingFile = false;
        return;
    }

    g_isBrowsingFile.store(false);
    state->isBrowsingFile = false;

    if (cmd == 501) {
        OpenBrowseCustomBackgroundDialog(state);
        return;
    }
    if (cmd == 310) {
        OpenCustomSizeDialog(state);
        return;
    }

    if (cmd != 0) {
        auto clearManualDimensions = []() {
            Wh_SetIntValue(GetUserOverrideKey(L"customDialogWidth").c_str(), 0);
            Wh_SetIntValue(GetUserOverrideKey(L"customBarHeight").c_str(), 0);
            Wh_SetIntValue(GetUserOverrideKey(L"customFontSize").c_str(), 0);
        };
        if (cmd == 101) Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"glass");
        else if (cmd == 102) Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"blur");
        else if (cmd == 103) Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"solid");
        else if (cmd == 107) { Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"glass"); Wh_SetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), -100); }
        else if (cmd == 108) { Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"glass"); Wh_SetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), -80); }
        else if (cmd == 109) { Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"glass"); Wh_SetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), -50); }
        else if (cmd == 110) { Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"glass"); Wh_SetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), 0); }
        else if (cmd == 111) { Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"glass"); Wh_SetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), 15); }
        else if (cmd == 112) { Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"glass"); Wh_SetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), 30); }
        else if (cmd == 113) { Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"glass"); Wh_SetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), 55); }
        else if (cmd == 114) { Wh_SetStringValue(GetUserOverrideKey(L"surfaceTheme").c_str(), L"glass"); Wh_SetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), 80); }
        else if (cmd == 201) Wh_SetStringValue(GetUserOverrideKey(L"colorMode").c_str(), L"auto");
        else if (cmd == 202) Wh_SetStringValue(GetUserOverrideKey(L"colorMode").c_str(), L"dark");
        else if (cmd == 203) Wh_SetStringValue(GetUserOverrideKey(L"colorMode").c_str(), L"light");
        else if (cmd == 301) { clearManualDimensions(); Wh_SetIntValue(GetUserOverrideKey(L"uiScalePercent").c_str(), 85); }
        else if (cmd == 302) { clearManualDimensions(); Wh_SetIntValue(GetUserOverrideKey(L"uiScalePercent").c_str(), 90); }
        else if (cmd == 303) { clearManualDimensions(); Wh_SetIntValue(GetUserOverrideKey(L"uiScalePercent").c_str(), 100); }
        else if (cmd == 304) { clearManualDimensions(); Wh_SetIntValue(GetUserOverrideKey(L"uiScalePercent").c_str(), 110); }
        else if (cmd == 305) { clearManualDimensions(); Wh_SetIntValue(GetUserOverrideKey(L"uiScalePercent").c_str(), 120); }
        else if (cmd == 311) { clearManualDimensions(); }
        else if (cmd == 350) Wh_SetIntValue(GetUserOverrideKey(L"minimalMode").c_str(), state->settings.minimalMode ? 0 : 1);
        else if (cmd == 401) Wh_SetIntValue(GetUserOverrideKey(L"maxHistoryItems").c_str(), 10);
        else if (cmd == 402) Wh_SetIntValue(GetUserOverrideKey(L"maxHistoryItems").c_str(), 30);
        else if (cmd == 403) Wh_SetIntValue(GetUserOverrideKey(L"maxHistoryItems").c_str(), 35);
        else if (cmd == 404) Wh_SetIntValue(GetUserOverrideKey(L"maxHistoryItems").c_str(), 50);
        else if (cmd == 405) Wh_SetIntValue(GetUserOverrideKey(L"maxHistoryItems").c_str(), 0);
        else if (cmd == 406) Wh_SetIntValue(GetUserOverrideKey(L"showTimestamp").c_str(), state->settings.showTimestamp ? 0 : 1);
        else if (cmd == 407) Wh_SetIntValue(GetUserOverrideKey(L"enableAppSuggestions").c_str(), state->settings.enableAppSuggestions ? 0 : 1);
        else if (cmd == 502) Wh_SetStringValue(GetUserOverrideKey(L"customBackgroundPath").c_str(), L"-");
        else if (cmd == 503) Wh_SetIntValue(GetUserOverrideKey(L"customBackgroundOpacity").c_str(), 90);
        else if (cmd == 504) Wh_SetIntValue(GetUserOverrideKey(L"customBackgroundOpacity").c_str(), 150);
        else if (cmd == 505) Wh_SetIntValue(GetUserOverrideKey(L"customBackgroundOpacity").c_str(), 215);
        else if (cmd == 506) Wh_SetIntValue(GetUserOverrideKey(L"customBackgroundOpacity").c_str(), 255);
        else if (cmd >= 600 && cmd <= 621) {
            for (const auto& opt : kLangOpts) {
                if (opt.id == cmd) {
                    Wh_SetStringValue(GetUserOverrideKey(L"language").c_str(), opt.code);
                    break;
                }
            }
        } else if (cmd == 701) {
            ClearCurrentUserStoredHistory();
        }

        LoadSettings(false);
        ApplyReloadedSettingsToDialog(state);
    }

    if (IsWindow(hDlg) && IsWindowVisible(hDlg) && !IsIconic(hDlg)) {
        if (GetForegroundWindow() == hDlg && !state->closePressed &&
            state->hModernEdit && IsWindow(state->hModernEdit)) {
            SetFocus(state->hModernEdit);
        }
        state->caretVisible = true;
        InvalidateRect(hDlg, nullptr, TRUE);
    }
}

void ApplyReloadedSettingsToDialog(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    int oldHeight = state->totalHeight;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        state->settings = g_settings;
        state->i18n = g_i18n;
    }
    UpdateDialogEditFont(state);
    ReloadCustomBackgroundMedia(state, false);
    LoadHistoryItems(state);
    std::wstring curText = GetEffectiveInputText(state->hModernEdit);
    UpdateAppSuggestionsForQuery(state, curText);
    RecomputeDialogMetrics(state);
    ResizeDialogPreservingBottomAnchor(state, oldHeight);
    ApplyWindowAppearance(state->hDlg, state);
    InvalidateRect(state->hDlg, nullptr, TRUE);
}

void UpdateDialogEditFont(RunDialogState* state) {
    if (!state) return;
    if (state->hFontEditCtl) {
        DeleteObject(state->hFontEditCtl);
    }
    int fontPx = (int)std::round(GetFontSizeSF(13.5f, state));
    state->hFontEditCtl = CreateFontW(
        -fontPx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
        SendMessageW(state->hModernEdit, WM_SETFONT, (WPARAM)state->hFontEditCtl, FALSE);
    }
}

void RecomputeDialogMetrics(RunDialogState* state) {
    if (!state) return;
    ClampScrollOffset(state);
    int visRowCount = GetVisibleRowCount(state);
    int padTop = GetListPadTop(state);
    int padBot = GetListPadBottom(state);
    int rowH   = GetRowHeight(state);
    int barH   = GetInputBarHeight(state);

    state->topAreaHeight = (visRowCount > 0) ? (padTop + visRowCount * rowH + padBot) : 0;
    state->totalHeight   = state->topAreaHeight + barH;
}

LRESULT CALLBACK ModernEditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                        UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto* state = reinterpret_cast<RunDialogState*>(dwRefData);

    if (uMsg == WM_GETDLGCODE) {
        return DLGC_WANTALLKEYS | DLGC_HASSETSEL;
    }

    if (uMsg == WM_MOUSEWHEEL && state && state->hDlg) {
        SendMessageW(state->hDlg, WM_MOUSEWHEEL, wParam, lParam);
        return 0;
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
        if (wParam == VK_TAB) {
            const auto& activeRows = GetActiveRowItems(state);
            if (!activeRows.empty()) {
                int idx = (state->selectedRow >= 0 && state->selectedRow < (int)activeRows.size())
                          ? state->selectedRow : state->scrollOffset;
                idx = std::clamp(idx, 0, (int)activeRows.size() - 1);
                std::wstring fillCmd = activeRows[idx].cmd;
                state->suppressSuggestionUpdate = true;
                SetWindowTextW(hWnd, fillCmd.c_str());
                SendMessageW(hWnd, EM_SETSEL, fillCmd.size(), fillCmd.size());
                state->suppressSuggestionUpdate = false;
                SyncInputToNativeCombo(state, fillCmd.c_str());
                InvalidateRect(state->hDlg, nullptr, FALSE);
                return 0;
            }
        }
        if (wParam == VK_DELETE && !state->showingSuggestions && !state->history.empty()) {
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
                ResizeDialogPreservingBottomAnchor(state, oldHeight);
                InvalidateRect(state->hDlg, nullptr, TRUE);
                return 0;
            }
        }
        if (wParam == VK_UP || wParam == VK_DOWN || wParam == VK_PRIOR || wParam == VK_NEXT) {
            const auto& activeRows = GetActiveRowItems(state);
            if (!activeRows.empty()) {
                int n = (int)activeRows.size();
                int step = (wParam == VK_PRIOR || wParam == VK_NEXT) ? (std::max)(1, GetVisibleRowCount(state) - 1) : 1;
                if (wParam == VK_UP || wParam == VK_PRIOR) {
                    if (state->selectedRow <= 0) {
                        state->selectedRow = (wParam == VK_UP) ? (n - 1) : 0;
                    } else {
                        state->selectedRow = (std::max)(0, state->selectedRow - step);
                    }
                } else {
                    if (state->selectedRow < 0) {
                        state->selectedRow = 0;
                    } else if (state->selectedRow >= n - 1) {
                        state->selectedRow = (wParam == VK_DOWN) ? 0 : (n - 1);
                    } else {
                        state->selectedRow = (std::min)(n - 1, state->selectedRow + step);
                    }
                }
                EnsureRowVisible(state, state->selectedRow);
                if (!state->showingSuggestions) {
                    const auto& cmd = activeRows[state->selectedRow].cmd;
                    state->suppressSuggestionUpdate = true;
                    SetWindowTextW(hWnd, cmd.c_str());
                    SendMessageW(hWnd, EM_SETSEL, cmd.size(), cmd.size());
                    state->suppressSuggestionUpdate = false;
                    SyncInputToNativeCombo(state, cmd.c_str());
                }
                InvalidateRect(state->hDlg, nullptr, FALSE);
                return 0;
            }
        }
    }

    if (uMsg == WM_CHAR && (wParam == VK_RETURN || wParam == VK_ESCAPE || wParam == VK_TAB)) {
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
    CloseActiveGearMenu();
    KillTimer(hWnd, kTimerCaret);
    KillTimer(hWnd, kTimerLuminance);
    KillTimer(hWnd, kTimerBgMedia);
    HWND expectedActive = hWnd;
    g_hActiveRunDlg.compare_exchange_strong(expectedActive, nullptr);
    HWND expectedTracked = hWnd;
    g_hTrackedRunDlg.compare_exchange_strong(expectedTracked, nullptr);
    HWND expectedFallback = hWnd;
    g_hFallbackInterceptedDlg.compare_exchange_strong(expectedFallback, nullptr);
    g_isBrowsingFile.store(false);
    StopKeyHookThread();
    ReleaseBackdropSnapshot();
    RemovePropW(hWnd, L"WindhawkModernRunApplied");
    RemovePropW(hWnd, L"WindhawkOrigShellDlgProc");

    if (state) {
        ReleaseCustomBackgroundMedia(state);
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            RemoveWindowSubclass(state->hModernEdit, ModernEditSubclassProc, 1);
            DestroyWindow(state->hModernEdit);
            state->hModernEdit = nullptr;
        }
        for (auto& item : state->history) {
            if (item.hIcon && item.ownsIcon) DestroyIcon(item.hIcon);
        }
        state->history.clear();
        state->suggestions.clear();
        if (state->hFontEditCtl) {
            DeleteObject(state->hFontEditCtl);
            state->hFontEditCtl = nullptr;
        }
        if (state->pCachedRT) {
            state->pCachedRT->Release();
            state->pCachedRT = nullptr;
        }
        if (state->hCachedMemDC) {
            if (state->hOldCachedBmp) {
                SelectObject(state->hCachedMemDC, state->hOldCachedBmp);
                state->hOldCachedBmp = nullptr;
            }
            if (state->hCachedBmp) {
                DeleteObject(state->hCachedBmp);
                state->hCachedBmp = nullptr;
            }
            DeleteDC(state->hCachedMemDC);
            state->hCachedMemDC = nullptr;
        }
        state->cachedDibPixels = nullptr;
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
    SendMessageW(hWnd, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), (LPARAM)hCancel);
    if (IsWindow(hWnd)) {
        EndDialog(hWnd, IDCANCEL);
        PostMessageW(hWnd, WM_CLOSE, 0, 0);
        PostMessageW(hWnd, WM_NULL, 0, 0);
    }
}

LRESULT CALLBACK RunDlgSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto* state = reinterpret_cast<RunDialogState*>(dwRefData);

    if (uMsg == g_msgUninit && g_msgUninit != 0) {
        if (!state) {
            return 1;
        }
        if (state->isBrowsingFile || state->isTriggeringRun) {
            state->pendingUninit = true;
            CloseActiveGearMenu();
            HWND hPopup = GetWindow(hWnd, GW_ENABLEDPOPUP);
            if (hPopup && hPopup != hWnd && IsWindow(hPopup)) {
                PostMessageW(hPopup, WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), 0);
                PostMessageW(hPopup, WM_CLOSE, 0, 0);
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
                if (IsIconic(hWnd) || !g_dlgIsForeground.load() || GetForegroundWindow() != hWnd) {
                    RestoreRunDialog(state);
                } else {
                    CloseRunDialog(hWnd);
                }
                break;
            case kActionReloadSettings:
                ApplyReloadedSettingsToDialog(state);
                break;
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
                UpdateDialogEditFont(state);
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

        case WM_EXITSIZEMOVE: {
            if (state && !IsIconic(hWnd) && state->effectiveSurface == SurfaceTheme::Glass &&
                state->settings.colorMode == ColorMode::Auto) {
                bool newSample = SampleBackdropLuminanceFromSnap(
                    state->normalX, state->normalY, GetDlgWidth(state), state->totalHeight, state->backdropLightSample);
                if (newSample != state->backdropLightSample) {
                    state->backdropLightSample = newSample;
                    ApplyWindowAppearance(hWnd, state);
                    InvalidateRect(hWnd, nullptr, FALSE);
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
                    if (state->effectiveSurface == SurfaceTheme::Glass) {
                        InvalidateRect(hWnd, nullptr, FALSE);
                        UpdateWindow(hWnd);
                    }
                }
            }
            break;
        }

        case WM_SHOWWINDOW:
            if (wParam == FALSE) {
                CloseActiveGearMenu();
                if (!IsIconic(hWnd)) {
                    HWND expected = hWnd;
                    if (g_hActiveRunDlg.compare_exchange_strong(expected, nullptr)) {
                        StopKeyHookThread();
                    }
                }
            }
            break;

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
                if (!IsIconic(hWnd) && state->effectiveSurface == SurfaceTheme::Glass) {
                    CheckAndUpdateBackdropInRealtime(hWnd, state, false);
                }
                return 0;
            }
            if (wParam == kTimerBgMedia && state) {
                StepCustomBackgroundAnimation(state);
                return 0;
            }
            break;
        }

        case WM_MOUSEWHEEL: {
            if (state && HasVerticalScrollbar(state)) {
                short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                int step = (zDelta > 0) ? -1 : 1;
                if (std::abs(zDelta) >= WHEEL_DELTA * 2) {
                    step *= 2;
                }
                int oldOffset = state->scrollOffset;
                state->scrollOffset += step;
                ClampScrollOffset(state);
                if (state->scrollOffset != oldOffset) {
                    if (state->hasMousePt) {
                        state->hoveredRow = HitTestHistoryRow(state, state->mousePt);
                    }
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
            }
            return 0;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == kModernEditId && (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == EN_UPDATE)) {
                if (state) {
                    state->caretVisible = true;
                    std::wstring curText = GetEffectiveInputText(state->hModernEdit);
                    SyncInputToNativeCombo(state, curText.c_str());
                    if (HIWORD(wParam) == EN_CHANGE && !state->suppressSuggestionUpdate) {
                        state->scrollOffset = 0;
                        UpdateAppSuggestionsForQuery(state, curText);
                        InvalidateRect(hWnd, nullptr, FALSE);
                    } else {
                        RECT rcBar = { 0, state->topAreaHeight, GetDlgWidth(state), state->totalHeight };
                        InvalidateRect(hWnd, &rcBar, FALSE);
                    }
                }
                return 0;
            }
            if (LOWORD(wParam) == IDOK && state && !state->isTriggeringRun) {
                TriggerRunCommand(state);
                return 0;
            }
            break;
        }

        case WM_ACTIVATEAPP: {
            if (wParam == FALSE) {
                CloseActiveGearMenu();
            }
            break;
        }

        case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE) {
                POINT pt = {};
                GetCursorPos(&pt);
                if (g_dlgIsForeground.load() && IsPointOnTaskbar(pt)) {
                    g_deactivatedByTaskbarTick.store(GetTickCount64());
                } else {
                    g_deactivatedByTaskbarTick.store(0);
                }
                g_dlgIsForeground.store(false);
                CloseActiveGearMenu();
                if (state && state->settings.closeOnClickOutside &&
                    IsWindowVisible(hWnd) && !IsIconic(hWnd) && !state->isTriggeringRun &&
                    (!state->isBrowsingFile || g_hActiveGearMenu.load() != nullptr)) {
                    if (GetTickCount64() - g_openTick.load() > 300) {
                        CloseRunDialog(hWnd);
                        return 0;
                    }
                }
            } else {
                g_dlgIsForeground.store(true);
                g_deactivatedByTaskbarTick.store(0);
                if (!IsIconic(hWnd) && IsWindowVisible(hWnd) && state && state->hModernEdit && !state->isBrowsingFile) {
                    SetFocus(state->hModernEdit);
                }
            }
            if (LOWORD(wParam) == WA_INACTIVE && state) {
                state->windowDragging = false;
            }
            if (state && state->effectiveSurface == SurfaceTheme::Glass && IsWindowVisible(hWnd) && !IsIconic(hWnd)) {
                CheckAndUpdateBackdropInRealtime(hWnd, state, false);
            }
            if (IsWindowVisible(hWnd) && !IsIconic(hWnd)) {
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_CAPTURECHANGED: {
            if (state && (HWND)lParam != hWnd) {
                if (state->windowDragging) {
                    state->windowDragging = false;
                    if (state->effectiveSurface == SurfaceTheme::Glass && state->settings.colorMode == ColorMode::Auto) {
                        bool newSample = SampleBackdropLuminanceFromSnap(
                            state->normalX, state->normalY, GetDlgWidth(state), state->totalHeight, state->backdropLightSample);
                        if (newSample != state->backdropLightSample) {
                            state->backdropLightSample = newSample;
                            ApplyWindowAppearance(hWnd, state);
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                    }
                }
                state->scrollbarDragging = false;
            }
            break;
        }

        case WM_MOUSEMOVE: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };

            if (state->windowDragging) {
                if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0) {
                    state->windowDragging = false;
                    ReleaseCapture();
                    return 0;
                }
                POINT ptScreen = {};
                GetCursorPos(&ptScreen);
                int newX = state->windowDragStartX + (ptScreen.x - state->windowDragStartCursor.x);
                int newY = state->windowDragStartY + (ptScreen.y - state->windowDragStartCursor.y);
                if (newX != state->normalX || newY != state->normalY) {
                    state->normalX = newX;
                    state->normalY = newY;
                    SetWindowPos(hWnd, nullptr, newX, newY, 0, 0,
                                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
                    InvalidateRect(hWnd, nullptr, FALSE);
                    UpdateWindow(hWnd);
                }
                return 0;
            }

            if (state->scrollbarDragging && HasVerticalScrollbar(state)) {
                RECT rcTrack = GetScrollbarTrackRect(state);
                RECT rcThumb = GetScrollbarThumbRect(state);
                int thumbH = rcThumb.bottom - rcThumb.top;
                int availSlide = (std::max)(1, static_cast<int>((rcTrack.bottom - rcTrack.top) - thumbH));
                int totalRows = (int)GetActiveRowItems(state).size();
                int visRows = GetVisibleRowCount(state);
                int maxScroll = (std::max)(1, totalRows - visRows);
                int deltaY = pt.y - state->scrollbarDragStartY;
                int newOffset = state->scrollbarDragStartOffset + MulDiv(deltaY, maxScroll, availSlide);
                state->scrollOffset = std::clamp(newOffset, 0, maxScroll);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }

            int newHoverRow = HitTestHistoryRow(state, pt);
            ActionButtonsLayout layout = GetActionButtonsLayout(state);
            RECT rcTrack = GetScrollbarTrackRect(state);
            bool newSbHover     = HasVerticalScrollbar(state) && PtInRect(&rcTrack, pt);
            bool newBrowseHover = PtInRect(&layout.rcBrowse, pt);
            bool newBtnHover    = PtInRect(&layout.rcRun, pt);
            bool newCloseHover  = PtInRect(&layout.rcClose, pt);
            bool newGearHover   = PtInRect(&layout.rcGear, pt);

            bool pointerMoved = (state->effectiveSurface == SurfaceTheme::Glass) &&
                                (pt.x != state->mousePt.x || pt.y != state->mousePt.y);
            state->mousePt = pt;
            state->hasMousePt = true;

            if (newHoverRow != state->hoveredRow ||
                newSbHover != state->scrollbarHovered ||
                newBrowseHover != state->browseHovered ||
                newBtnHover != state->btnHovered ||
                newCloseHover != state->closeHovered ||
                newGearHover != state->gearHovered ||
                pointerMoved) {
                state->hoveredRow       = newHoverRow;
                state->scrollbarHovered = newSbHover;
                state->browseHovered    = newBrowseHover;
                state->btnHovered       = newBtnHover;
                state->closeHovered     = newCloseHover;
                state->gearHovered      = newGearHover;

                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            if (state->hoveredRow != -1 || state->pressedRow != -1 || state->hasMousePt ||
                state->scrollbarHovered ||
                state->browseHovered || state->browsePressed ||
                state->btnHovered || state->btnPressed ||
                state->closeHovered || state->closePressed ||
                state->gearHovered || state->gearPressed) {
                state->hoveredRow       = -1;
                state->pressedRow       = -1;
                state->hasMousePt       = false;
                state->scrollbarHovered = false;
                state->browseHovered    = false;
                state->browsePressed    = false;
                state->btnHovered       = false;
                state->btnPressed       = false;
                state->closeHovered     = false;
                state->closePressed     = false;
                state->gearHovered      = false;
                state->gearPressed      = false;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            ActionButtonsLayout layout = GetActionButtonsLayout(state);

            if (HasVerticalScrollbar(state)) {
                RECT rcTrack = GetScrollbarTrackRect(state);
                RECT rcThumb = GetScrollbarThumbRect(state);
                RECT rcHitTrack = { rcTrack.left - DpiScaleS(4, state), rcTrack.top, rcTrack.right + DpiScaleS(2, state), rcTrack.bottom };
                if (PtInRect(&rcHitTrack, pt)) {
                    if (PtInRect(&rcThumb, pt)) {
                        state->scrollbarDragging = true;
                        state->scrollbarDragStartY = pt.y;
                        state->scrollbarDragStartOffset = state->scrollOffset;
                        SetCapture(hWnd);
                    } else {
                        int visRows = GetVisibleRowCount(state);
                        if (pt.y < rcThumb.top) {
                            state->scrollOffset -= (std::max)(1, visRows - 1);
                        } else {
                            state->scrollOffset += (std::max)(1, visRows - 1);
                        }
                        ClampScrollOffset(state);
                    }
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
            }

            if (PtInRect(&layout.rcBrowse, pt)) {
                state->browsePressed = true;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRect(&layout.rcRun, pt)) {
                state->btnPressed = true;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRect(&layout.rcClose, pt)) {
                state->closePressed = true;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRect(&layout.rcGear, pt)) {
                state->gearPressed = true;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }
            if (PtInRect(&layout.rcEdit, pt) && state->hModernEdit) {
                SetFocus(state->hModernEdit);
                int len = GetWindowTextLengthW(state->hModernEdit);
                SendMessageW(state->hModernEdit, EM_SETSEL, len, len);
                state->caretVisible = true;
                InvalidateRect(hWnd, &layout.rcEdit, FALSE);
                return 0;
            }

            int row = HitTestHistoryRow(state, pt);
            if (row != -1) {
                state->pressedRow = row;
                SetCapture(hWnd);
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }

            state->windowDragging = true;
            GetCursorPos(&state->windowDragStartCursor);
            state->windowDragStartX = state->normalX;
            state->windowDragStartY = state->normalY;
            SetCapture(hWnd);
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            ActionButtonsLayout layout = GetActionButtonsLayout(state);

            if (state->windowDragging) {
                state->windowDragging = false;
                ReleaseCapture();
                if (state->effectiveSurface == SurfaceTheme::Glass && state->settings.colorMode == ColorMode::Auto) {
                    bool newSample = SampleBackdropLuminanceFromSnap(
                        state->normalX, state->normalY, GetDlgWidth(state), state->totalHeight, state->backdropLightSample);
                    if (newSample != state->backdropLightSample) {
                        state->backdropLightSample = newSample;
                        ApplyWindowAppearance(hWnd, state);
                        InvalidateRect(hWnd, nullptr, FALSE);
                    }
                }
                return 0;
            }

            if (state->scrollbarDragging) {
                state->scrollbarDragging = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                return 0;
            }

            if (state->pressedRow != -1) {
                int targetRow = state->pressedRow;
                state->pressedRow = -1;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                int releasedRow = HitTestHistoryRow(state, pt);
                const auto& activeRows = GetActiveRowItems(state);
                if (releasedRow == targetRow && targetRow >= 0 && targetRow < (int)activeRows.size()) {
                    HistoryItem chosen = activeRows[targetRow];
                    ExecuteHistoryOrSuggestionItem(state, chosen);
                }
                return 0;
            }
            if (state->browsePressed) {
                state->browsePressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&layout.rcBrowse, pt)) {
                    OpenBrowseExecutableDialog(state);
                }
                return 0;
            }
            if (state->btnPressed) {
                state->btnPressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&layout.rcRun, pt)) TriggerRunCommand(state);
                return 0;
            }
            if (state->closePressed) {
                state->closePressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&layout.rcClose, pt)) CloseRunDialog(hWnd);
                return 0;
            }
            if (state->gearPressed) {
                state->gearPressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&layout.rcGear, pt)) {
                    OpenGearSettingsMenu(state);
                }
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
            int scalePct = GetDlgScale(state);
            UINT effectiveDpi = MulDiv(dpi, scalePct, 100);
            bool isLight = state->isLightMode;
            SurfaceTheme surface = state->effectiveSurface;
            bool highContrast = IsHighContrastActive();
            const auto& activeRows = GetActiveRowItems(state);
            int visRows = GetVisibleRowCount(state);
            ActionButtonsLayout layout = GetActionButtonsLayout(state);

            if (w > 0 && h > 0) {
                if (!state->hCachedMemDC || !state->hCachedBmp || !state->pCachedRT ||
                    state->cachedDibW != w || state->cachedDibH != h) {
                    if (state->pCachedRT) {
                        state->pCachedRT->Release();
                        state->pCachedRT = nullptr;
                    }
                    if (state->hCachedMemDC) {
                        if (state->hOldCachedBmp) {
                            SelectObject(state->hCachedMemDC, state->hOldCachedBmp);
                            state->hOldCachedBmp = nullptr;
                        }
                        if (state->hCachedBmp) {
                            DeleteObject(state->hCachedBmp);
                            state->hCachedBmp = nullptr;
                        }
                        DeleteDC(state->hCachedMemDC);
                        state->hCachedMemDC = nullptr;
                    }
                    state->cachedDibPixels = nullptr;

                    state->hCachedMemDC = CreateCompatibleDC(hdc);
                    if (state->hCachedMemDC) {
                        BITMAPINFO bmi = {};
                        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                        bmi.bmiHeader.biWidth = w;
                        bmi.bmiHeader.biHeight = -h;
                        bmi.bmiHeader.biPlanes = 1;
                        bmi.bmiHeader.biBitCount = 32;
                        bmi.bmiHeader.biCompression = BI_RGB;

                        state->hCachedBmp = CreateDIBSection(state->hCachedMemDC, &bmi, DIB_RGB_COLORS,
                                                             (void**)&state->cachedDibPixels, nullptr, 0);
                        if (state->hCachedBmp && state->cachedDibPixels) {
                            state->hOldCachedBmp = SelectObject(state->hCachedMemDC, state->hCachedBmp);
                            D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
                                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                                96.0f, 96.0f);
                            if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&rtProps, &state->pCachedRT)) && state->pCachedRT) {
                                state->pCachedRT->BindDC(state->hCachedMemDC, &rcClient);
                                state->cachedDibW = w;
                                state->cachedDibH = h;
                            }
                        }
                    }
                }

                HDC hMemDC = state->hCachedMemDC;
                ID2D1DCRenderTarget* pRT = state->pCachedRT;
                if (hMemDC && state->hCachedBmp && state->cachedDibPixels) {
                    if (pRT) {
                        pRT->BindDC(hMemDC, &rcClient);
                        pRT->BeginDraw();
                        pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                        pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

                        // 1. Check Custom Background Media (Static Image / Animated GIF / MP4 Video)
                        const BYTE* bgPixels = nullptr;
                        if (state->pVideoReader && !state->videoFrameBuffer.empty()) {
                            bgPixels = state->videoFrameBuffer.data();
                        } else if (!state->bgFrames.empty()) {
                            size_t idx = state->bgCurrentFrame % state->bgFrames.size();
                            bgPixels = state->bgFrames[idx].pixels.data();
                        }
                        bool hasCustomBg = (bgPixels != nullptr && state->bgFrameW > 0 && state->bgFrameH > 0 && !highContrast);
                        if (hasCustomBg && state->customBgLumValid) {
                            // Text & UI contrast adapts directly to the custom background media
                            isLight = state->customBgIsLight;
                            state->isLightMode = isLight;
                        }

                        // 1a. Paint Surface Backdrop (Liquid Glass / Windows Blur / Solid / High Contrast)
                        if (highContrast) {
                            COLORREF winBg = GetSysColor(COLOR_WINDOW);
                            pRT->Clear(D2D1::ColorF(GetRValue(winBg) / 255.0f, GetGValue(winBg) / 255.0f, GetBValue(winBg) / 255.0f, 1.0f));
                        } else if (hasCustomBg) {
                            // Solid opaque base under custom background so Liquid Glass / Windows Blur never bleeds through
                            pRT->Clear(isLight ? D2D1::ColorF(0.95f, 0.95f, 0.96f, 1.0f)
                                               : D2D1::ColorF(0.08f, 0.08f, 0.10f, 1.0f));
                        } else if (surface == SurfaceTheme::Glass) {
                            // True iOS 26 Liquid Glass (-100 to 100):
                            // At 0: 0.02f (exact Clear default) + optical curved-edge lens refraction.
                            // Above 0: increases smoked glass tint up to 0.50f.
                            // Below 0 (-1 to -100): renders full iOS 26 Optical Liquid Glass (Gaussian blur +
                            // 3D Snell's-law curved-edge refraction + RGB prism dispersion + vibrancy boost)!
                            int glassPct = std::clamp(state->settings.glassOpacity, -100, 100);
                            float glassAlpha = (glassPct >= 0)
                                ? (0.02f + (glassPct / 100.0f) * 0.48f)
                                : 0.01f;
                            pRT->Clear(isLight ? D2D1::ColorF(0.92f, 0.94f, 0.97f, glassAlpha)
                                               : D2D1::ColorF(0.06f, 0.08f, 0.11f, glassAlpha));
                            DrawOpticalLiquidGlassBackdrop(pRT, hWnd, w, h, effectiveDpi, glassPct, isLight, false, 0);
                        } else if (surface == SurfaceTheme::Blur) {
                            pRT->Clear(isLight ? D2D1::ColorF(0.96f, 0.97f, 0.99f, 0.20f)
                                               : D2D1::ColorF(0.08f, 0.08f, 0.10f, 0.22f));
                        } else {
                            pRT->Clear(isLight ? D2D1::ColorF(0.96f, 0.96f, 0.98f, 1.0f)
                                               : D2D1::ColorF(0.12f, 0.12f, 0.15f, 1.0f));
                        }

                        // 1b. Paint Custom Background Image / Animated GIF / MP4 Video Frame directly (no theme veil wash-out!)
                        if (hasCustomBg) {
                            D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
                                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
                            ID2D1Bitmap* pBgBitmap = nullptr;
                            if (SUCCEEDED(pRT->CreateBitmap(D2D1::SizeU(state->bgFrameW, state->bgFrameH),
                                                            bgPixels, state->bgFrameW * 4, &bmpProps, &pBgBitmap)) && pBgBitmap) {
                                float dstAspect = (float)w / (float)h;
                                float srcAspect = (float)state->bgFrameW / (float)state->bgFrameH;
                                D2D1_RECT_F srcRect = D2D1::RectF(0.0f, 0.0f, (float)state->bgFrameW, (float)state->bgFrameH);
                                if (srcAspect > dstAspect) {
                                    float cropW = state->bgFrameH * dstAspect;
                                    float offsetX = (state->bgFrameW - cropW) * 0.5f;
                                    srcRect = D2D1::RectF(offsetX, 0.0f, offsetX + cropW, (float)state->bgFrameH);
                                } else {
                                    float cropH = state->bgFrameW / dstAspect;
                                    float offsetY = (state->bgFrameH - cropH) * 0.5f;
                                    srcRect = D2D1::RectF(0.0f, offsetY, (float)state->bgFrameW, offsetY + cropH);
                                }
                                float mediaOpacity = std::clamp(state->settings.customBackgroundOpacity, 20, 255) / 255.0f;
                                D2D1_RECT_F dstRect = D2D1::RectF(0.0f, 0.0f, (float)w, (float)h);
                                pRT->DrawBitmap(pBgBitmap, dstRect, mediaOpacity,
                                                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
                                pBgBitmap->Release();
                            }
                        } else if (!highContrast && surface == SurfaceTheme::Glass) {
                            // Liquid Glass 4-stop diagonal sheen + radial specular highlight from live-update
                            DrawD2DPointerSpecularGlow(pRT, state->mousePt, state->hasMousePt, w, h, effectiveDpi, isLight);
                        }

                        // 2. Create Brushes & Round-Cap Stroke Style (adapting to custom background luminance when active)
                        D2D1_COLOR_F colPrimary, colSecondary, colTime, colHint, colTextShadow;
                        if (highContrast) {
                            COLORREF fg = GetSysColor(COLOR_WINDOWTEXT);
                            colPrimary = colSecondary = colTime = colHint =
                                D2D1::ColorF(GetRValue(fg) / 255.0f, GetGValue(fg) / 255.0f, GetBValue(fg) / 255.0f, 1.0f);
                            colTextShadow = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
                        } else if (hasCustomBg) {
                            colPrimary    = isLight ? D2D1::ColorF(0.04f, 0.05f, 0.08f, 1.00f) : D2D1::ColorF(0.99f, 0.99f, 1.00f, 1.00f);
                            colSecondary  = isLight ? D2D1::ColorF(0.10f, 0.12f, 0.16f, 0.88f) : D2D1::ColorF(0.95f, 0.96f, 0.99f, 0.86f);
                            colTime       = isLight ? D2D1::ColorF(0.12f, 0.14f, 0.18f, 0.84f) : D2D1::ColorF(0.93f, 0.95f, 0.98f, 0.82f);
                            colHint       = isLight ? D2D1::ColorF(0.14f, 0.16f, 0.20f, 0.76f) : D2D1::ColorF(0.92f, 0.94f, 0.98f, 0.74f);
                            colTextShadow = isLight ? D2D1::ColorF(1.00f, 1.00f, 1.00f, 0.72f) : D2D1::ColorF(0.00f, 0.00f, 0.00f, 0.78f);
                        } else {
                            colPrimary    = isLight ? D2D1::ColorF(0.06f, 0.07f, 0.10f, 1.00f) : D2D1::ColorF(0.98f, 0.99f, 1.00f, 1.00f);
                            colSecondary  = isLight ? D2D1::ColorF(0.16f, 0.18f, 0.24f, 0.88f) : D2D1::ColorF(0.92f, 0.94f, 0.98f, 0.78f);
                            colTime       = isLight ? D2D1::ColorF(0.18f, 0.20f, 0.26f, 0.82f) : D2D1::ColorF(0.90f, 0.93f, 0.97f, 0.74f);
                            colHint       = isLight ? D2D1::ColorF(0.24f, 0.27f, 0.34f, 0.72f) : D2D1::ColorF(0.88f, 0.91f, 0.96f, 0.68f);
                            colTextShadow = (surface == SurfaceTheme::Glass)
                                ? (isLight ? D2D1::ColorF(0.00f, 0.00f, 0.00f, 0.00f) : D2D1::ColorF(0.00f, 0.00f, 0.00f, 0.62f))
                                : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
                        }

                        ID2D1SolidColorBrush* pBrPrimary = nullptr;
                        ID2D1SolidColorBrush* pBrSecondary = nullptr;
                        ID2D1SolidColorBrush* pBrTime = nullptr;
                        ID2D1SolidColorBrush* pBrHint = nullptr;
                        ID2D1SolidColorBrush* pBrShadow = nullptr;
                        pRT->CreateSolidColorBrush(colPrimary, &pBrPrimary);
                        pRT->CreateSolidColorBrush(colSecondary, &pBrSecondary);
                        pRT->CreateSolidColorBrush(colTime, &pBrTime);
                        pRT->CreateSolidColorBrush(colHint, &pBrHint);
                        pRT->CreateSolidColorBrush(colTextShadow, &pBrShadow);

                        ID2D1StrokeStyle* pRoundStroke = nullptr;
                        D2D1_STROKE_STYLE_PROPERTIES strokeProps = D2D1::StrokeStyleProperties(
                            D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND);
                        g_pD2DFactory->CreateStrokeStyle(strokeProps, nullptr, 0, &pRoundStroke);

                        // 3. Create DirectWrite Text Formats (respecting customFontSize when set)
                        IDWriteTextFormat* pFmtRow = nullptr;
                        IDWriteTextFormat* pFmtTime = nullptr;
                        IDWriteTextFormat* pFmtInput = nullptr;
                        IDWriteTextFormat* pFmtBtn = nullptr;

                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(13.5f, state), L"", &pFmtRow);
                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(12.0f, state), L"", &pFmtTime);
                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(13.5f, state), L"", &pFmtInput);
                        g_pDWriteFactory->CreateTextFormat(
                            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(13.0f, state), L"", &pFmtBtn);

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
                            pFmtBtn->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                            pFmtBtn->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtBtn->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                        }

                        int padTop = GetListPadTop(state);
                        int rowHInt = GetRowHeight(state);
                        float pillRadius = DpiScaleSF(6.5f, state);
                        float textLeftX = DpiScaleSF(46.0f, state);
                        bool hasScrollbar = HasVerticalScrollbar(state);
                        float sbReserve = hasScrollbar ? DpiScaleSF(10.0f, state) : 0.0f;

                        // 4. Draw Visible History or App Suggestion Rows (up to 5 visible rows with scrolling)
                        for (int visIdx = 0; visIdx < visRows; ++visIdx) {
                            int i = state->scrollOffset + visIdx;
                            if (i < 0 || i >= (int)activeRows.size()) break;

                            float rowTop = (float)(padTop + visIdx * rowHInt);
                            float rowH   = (float)rowHInt;
                            bool isHovered  = (i == state->hoveredRow);
                            bool isPressed  = (i == state->pressedRow);
                            bool isSelected = (i == state->selectedRow);

                            if (isHovered || isPressed || isSelected) {
                                float padX = DpiScaleSF(10.0f, state);
                                float padY = DpiScaleSF(2.0f, state);
                                D2D1_RECT_F rcPill = D2D1::RectF(padX, rowTop + padY, (float)w - padX - sbReserve, rowTop + rowH - padY);
                                if (!hasCustomBg && surface == SurfaceTheme::Glass) {
                                    D2D1_COLOR_F rowTint = isLight
                                        ? D2D1::ColorF(0.0f, 0.0f, 0.0f, isPressed ? 0.14f : (isSelected ? 0.09f : 0.055f))
                                        : D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.26f : 0.14f);
                                    DrawD2DGlassButton(pRT, rcPill, pillRadius, effectiveDpi, rowTint, isHovered, isPressed, isLight);
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

                            float rightLimit = (float)w - DpiScaleSF(18.0f, state) - sbReserve;
                            const auto& rowItem = activeRows[i];

                            if (!rowItem.isSuggestion && state->settings.showTimestamp && pFmtTime && pBrTime && !rowItem.timeStr.empty()) {
                                rightLimit = (float)w - DpiScaleSF(42.0f, state) - sbReserve;
                                const std::wstring& tStr = rowItem.timeStr;
                                IDWriteTextLayout* pTimeLayout = nullptr;
                                if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                        tStr.c_str(), (UINT32)tStr.size(), pFmtTime, DpiScaleSF(160.0f, state), rowH, &pTimeLayout))) {
                                    DWRITE_TEXT_METRICS tm = {};
                                    pTimeLayout->GetMetrics(&tm);
                                    float timeRight = (float)w - DpiScaleSF(42.0f, state) - sbReserve;
                                    if ((hasCustomBg || surface == SurfaceTheme::Glass) && pBrShadow) {
                                        float shOff = DpiScaleF(1.0f, dpi);
                                        pRT->DrawTextLayout(D2D1::Point2F(timeRight - DpiScaleSF(160.0f, state) + shOff, rowTop + shOff),
                                                            pTimeLayout, pBrShadow);
                                    }
                                    pRT->DrawTextLayout(D2D1::Point2F(timeRight - DpiScaleSF(160.0f, state), rowTop),
                                                        pTimeLayout, pBrTime);
                                    rightLimit = timeRight - tm.widthIncludingTrailingWhitespace - DpiScaleSF(12.0f, state);
                                    pTimeLayout->Release();
                                }
                                DrawD2DClockVector(pRT, pBrTime, pRoundStroke,
                                                   (float)w - DpiScaleSF(26.0f, state) - sbReserve, rowTop + rowH * 0.5f, effectiveDpi);
                            } else if (rowItem.isSuggestion && pBrTime && pRoundStroke) {
                                rightLimit = (float)w - DpiScaleSF(38.0f, state) - sbReserve;
                                DrawD2DReturnArrowVector(pRT, pBrTime, pRoundStroke,
                                                         (float)w - DpiScaleSF(25.0f, state) - sbReserve, rowTop + rowH * 0.5f, effectiveDpi);
                            }

                            // Single IDWriteTextLayout for "cmd  -  App Name" so Semibold + Regular share the exact same baseline
                            if (pFmtRow && pBrPrimary) {
                                const std::wstring& cmdStr = rowItem.cmd;
                                const std::wstring& appStr = rowItem.appName;
                                std::wstring fullStr = cmdStr;
                                if (!appStr.empty() && _wcsicmp(cmdStr.c_str(), appStr.c_str()) != 0) {
                                    fullStr += L"  -  " + appStr;
                                }

                                float availW = (std::max)(DpiScaleSF(40.0f, state), rightLimit - textLeftX);
                                IDWriteTextLayout* pRowLayout = nullptr;
                                if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                        fullStr.c_str(), (UINT32)fullStr.size(), pFmtRow, availW, rowH, &pRowLayout))) {
                                    DWRITE_TEXT_RANGE cmdRange = { 0, (UINT32)cmdStr.size() };
                                    pRowLayout->SetFontWeight(DWRITE_FONT_WEIGHT_SEMI_BOLD, cmdRange);

                                    if ((hasCustomBg || surface == SurfaceTheme::Glass) && pBrShadow) {
                                        float shOff = DpiScaleF(1.0f, dpi);
                                        pRT->DrawTextLayout(D2D1::Point2F(textLeftX + shOff, rowTop + shOff),
                                                            pRowLayout, pBrShadow, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                                    }
                                    if (fullStr.size() > cmdStr.size() && pBrSecondary) {
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

                        // 4b. Draw Sleek Fluent Vertical Scrollbar when items exceed visible viewport (5 rows)
                        if (hasScrollbar) {
                            RECT rcTrackWin = GetScrollbarTrackRect(state);
                            RECT rcThumbWin = GetScrollbarThumbRect(state);
                            D2D1_RECT_F rcTrack = D2D1::RectF((float)rcTrackWin.left, (float)rcTrackWin.top,
                                                              (float)rcTrackWin.right, (float)rcTrackWin.bottom);
                            D2D1_RECT_F rcThumb = D2D1::RectF((float)rcThumbWin.left, (float)rcThumbWin.top,
                                                              (float)rcThumbWin.right, (float)rcThumbWin.bottom);
                            float sbRadius = (rcTrack.right - rcTrack.left) * 0.5f;

                            ID2D1SolidColorBrush* pTrackBr = nullptr;
                            D2D1_COLOR_F trackCol = isLight
                                ? D2D1::ColorF(0.0f, 0.0f, 0.0f, state->scrollbarHovered ? 0.10f : 0.05f)
                                : D2D1::ColorF(1.0f, 1.0f, 1.0f, state->scrollbarHovered ? 0.12f : 0.06f);
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(trackCol, &pTrackBr))) {
                                pRT->FillRoundedRectangle(D2D1::RoundedRect(rcTrack, sbRadius, sbRadius), pTrackBr);
                                pTrackBr->Release();
                            }

                            ID2D1SolidColorBrush* pThumbBr = nullptr;
                            float thumbAlpha = state->scrollbarDragging ? 0.72f : (state->scrollbarHovered ? 0.55f : 0.36f);
                            D2D1_COLOR_F thumbCol = isLight
                                ? D2D1::ColorF(0.15f, 0.18f, 0.24f, thumbAlpha)
                                : D2D1::ColorF(0.92f, 0.94f, 0.98f, thumbAlpha);
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(thumbCol, &pThumbBr))) {
                                pRT->FillRoundedRectangle(D2D1::RoundedRect(rcThumb, sbRadius, sbRadius), pThumbBr);
                                pThumbBr->Release();
                            }
                        }

                        // Subtle divider line + iOS 26 Liquid Glass input capsule around the command input box
                        if (state->topAreaHeight > 0) {
                            ID2D1SolidColorBrush* pDivBr = nullptr;
                            D2D1_COLOR_F divCol = isLight ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.10f)
                                                          : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f);
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(divCol, &pDivBr))) {
                                float divX = DpiScaleSF(12.0f, state);
                                pRT->DrawLine(D2D1::Point2F(divX, (float)state->topAreaHeight),
                                              D2D1::Point2F((float)w - divX, (float)state->topAreaHeight),
                                              pDivBr, DpiScaleSF(1.0f, state));
                                pDivBr->Release();
                            }
                        }
                        if (!hasCustomBg && surface == SurfaceTheme::Glass && !highContrast) {
                            D2D1_RECT_F rcInputCapsule = D2D1::RectF(
                                DpiScaleSF(10.0f, state),
                                (float)layout.rcEdit.top - DpiScaleSF(2.0f, state),
                                (float)layout.rcEdit.right + DpiScaleSF(6.0f, state),
                                (float)layout.rcEdit.bottom + DpiScaleSF(2.0f, state));
                            D2D1_COLOR_F capTint = isLight
                                ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.045f)
                                : D2D1::ColorF(0.85f, 0.94f, 1.0f, 0.07f);
                            DrawD2DGlassButton(pRT, rcInputCapsule, pillRadius, effectiveDpi, capTint, false, false, isLight);
                        }

                        // 5. Draw Input Box with DirectWrite HitTestTextPosition for caret & selection placement
                        RECT rcEditWin = layout.rcEdit;
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
                                if ((hasCustomBg || surface == SurfaceTheme::Glass) && pBrShadow) {
                                    float shOff = DpiScaleF(1.0f, dpi);
                                    D2D1_RECT_F rcSh = D2D1::RectF(rcEdit.left + shOff, rcEdit.top + shOff, rcEdit.right + shOff, rcEdit.bottom + shOff);
                                    pRT->DrawText(state->i18n.placeholder, (UINT32)wcslen(state->i18n.placeholder),
                                                  pFmtInput, rcSh, pBrShadow);
                                }
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

                                float scrollPad = DpiScaleSF(6.0f, state);
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
                                        float padY = DpiScaleSF(2.0f, state);
                                        pRT->FillRectangle(
                                            D2D1::RectF(rcEdit.left + x0 - scrollOffsetX, rcEdit.top + padY,
                                                        rcEdit.left + x1 - scrollOffsetX, rcEdit.bottom - padY),
                                            pSelBr);
                                        pSelBr->Release();
                                    }
                                }

                                if ((hasCustomBg || surface == SurfaceTheme::Glass) && pBrShadow) {
                                    float shOff = DpiScaleF(1.0f, dpi);
                                    pRT->DrawTextLayout(D2D1::Point2F(rcEdit.left - scrollOffsetX + shOff, rcEdit.top + shOff),
                                                        pInputLayout, pBrShadow);
                                }
                                pRT->DrawTextLayout(D2D1::Point2F(rcEdit.left - scrollOffsetX, rcEdit.top),
                                                    pInputLayout, pBrPrimary);
                                pInputLayout->Release();
                            }
                        }

                        bool editFocused = (GetFocus() == state->hModernEdit) ||
                                           (GetForegroundWindow() == hWnd && !state->isBrowsingFile);
                        if (editFocused && state->caretVisible && pBrPrimary) {
                            float cx = rcEdit.left + caretXPos - scrollOffsetX + DpiScaleSF(0.8f, state);
                            cx = std::clamp(cx, rcEdit.left + DpiScaleSF(0.8f, state), rcEdit.right - DpiScaleSF(1.5f, state));
                            float padY = DpiScaleSF(3.5f, state);
                            pRT->DrawLine(D2D1::Point2F(cx, rcEdit.top + padY),
                                          D2D1::Point2F(cx, rcEdit.bottom - padY),
                                          pBrPrimary, DpiScaleSF(1.25f, state));
                        }

                        pRT->PopAxisAlignedClip();

                        // 6. Draw the 4 Action Buttons: Browse (Folder), Run (Return Arrow), Exit (Close X), and Gear Settings (⚙️)
                        enum class ActionBtnKind { Browse, Run, Exit, Gear };
                        auto drawActionButton = [&](RECT rWin, const wchar_t* label, ActionBtnKind kind,
                                                    bool hovered, bool pressed) {
                            D2D1_RECT_F rcBtn = D2D1::RectF((float)rWin.left, (float)rWin.top,
                                                            (float)rWin.right, (float)rWin.bottom);
                            D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rcBtn, pillRadius, pillRadius);

                            D2D1_COLOR_F contentCol = {};

                            if (!hasCustomBg && surface == SurfaceTheme::Glass && !highContrast) {
                                // True Liquid Glass buttons with subtle crystal tints and refractive rims
                                D2D1_COLOR_F glassTint = {};
                                if (kind == ActionBtnKind::Browse || kind == ActionBtnKind::Gear) {
                                    glassTint = isLight
                                        ? D2D1::ColorF(0.06f, 0.08f, 0.14f, pressed ? 0.16f : (hovered ? 0.10f : 0.05f))
                                        : D2D1::ColorF(0.85f, 0.92f, 1.00f, pressed ? 0.22f : (hovered ? 0.14f : 0.07f));
                                } else if (kind == ActionBtnKind::Run) {
                                    glassTint = isLight
                                        ? D2D1::ColorF(0.06f, 0.45f, 0.90f, pressed ? 0.36f : (hovered ? 0.26f : 0.16f))
                                        : D2D1::ColorF(0.12f, 0.54f, 0.98f, pressed ? 0.44f : (hovered ? 0.34f : 0.22f));
                                } else {
                                    glassTint = isLight
                                        ? D2D1::ColorF(0.82f, 0.16f, 0.26f, pressed ? 0.32f : (hovered ? 0.22f : 0.13f))
                                        : D2D1::ColorF(0.92f, 0.22f, 0.32f, pressed ? 0.40f : (hovered ? 0.30f : 0.18f));
                                }
                                DrawD2DGlassButton(pRT, rcBtn, pillRadius, effectiveDpi, glassTint, hovered, pressed, isLight);
                                contentCol = isLight
                                    ? D2D1::ColorF(0.05f, 0.06f, 0.10f, pressed ? 0.86f : 1.0f)
                                    : D2D1::ColorF(0.99f, 0.99f, 1.00f, pressed ? 0.86f : 1.0f);
                            } else {
                                D2D1_COLOR_F fillCol = {};
                                D2D1_COLOR_F borderCol = {};
                                if (kind == ActionBtnKind::Browse || kind == ActionBtnKind::Gear) {
                                    fillCol = D2D1::ColorF(0.17f, 0.18f, 0.22f, pressed ? 0.95f : (hovered ? 0.91f : 0.86f));
                                    borderCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, hovered ? 0.28f : 0.16f);
                                    contentCol = D2D1::ColorF(0.98f, 0.98f, 0.99f, 1.0f);
                                } else if (kind == ActionBtnKind::Run) {
                                    fillCol = pressed
                                        ? D2D1::ColorF(0.07f, 0.40f, 0.74f, 0.96f)
                                        : (hovered ? D2D1::ColorF(0.14f, 0.52f, 0.89f, 0.98f)
                                                   : D2D1::ColorF(0.09f, 0.47f, 0.83f, 0.95f));
                                    borderCol = D2D1::ColorF(0.38f, 0.68f, 0.98f, hovered ? 0.65f : 0.40f);
                                    contentCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
                                } else {
                                    fillCol = pressed
                                        ? D2D1::ColorF(0.25f, 0.11f, 0.14f, 0.94f)
                                        : (hovered ? D2D1::ColorF(0.39f, 0.16f, 0.21f, 0.96f)
                                                   : D2D1::ColorF(0.31f, 0.13f, 0.17f, 0.90f));
                                    borderCol = D2D1::ColorF(0.66f, 0.28f, 0.34f, hovered ? 0.62f : 0.40f);
                                    contentCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
                                }

                                if (pressed) {
                                    contentCol.a *= 0.86f;
                                }

                                ID2D1SolidColorBrush* pFillBr = nullptr;
                                if (SUCCEEDED(pRT->CreateSolidColorBrush(fillCol, &pFillBr))) {
                                    pRT->FillRoundedRectangle(rr, pFillBr);
                                    pFillBr->Release();
                                }

                                ID2D1SolidColorBrush* pBorderBr = nullptr;
                                if (SUCCEEDED(pRT->CreateSolidColorBrush(borderCol, &pBorderBr))) {
                                    pRT->DrawRoundedRectangle(rr, pBorderBr, DpiScaleSF(1.0f, state));
                                    pBorderBr->Release();
                                }
                            }

                            ID2D1SolidColorBrush* pContentBr = nullptr;
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(contentCol, &pContentBr))) {
                                if (kind == ActionBtnKind::Gear) {
                                    float iconCx = (rcBtn.left + rcBtn.right) * 0.5f;
                                    float iconCy = (rcBtn.top + rcBtn.bottom) * 0.5f;
                                    DrawD2DGearVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, effectiveDpi);
                                } else {
                                    if (pFmtBtn && label) {
                                        D2D1_RECT_F rcText = D2D1::RectF(
                                            rcBtn.left + DpiScaleSF(12.0f, state),
                                            rcBtn.top,
                                            rcBtn.right - DpiScaleSF(26.0f, state),
                                            rcBtn.bottom);
                                        pRT->DrawText(label, (UINT32)wcslen(label), pFmtBtn, rcText, pContentBr);
                                    }

                                    float iconCx = rcBtn.right - DpiScaleSF(15.5f, state);
                                    float iconCy = (rcBtn.top + rcBtn.bottom) * 0.5f;
                                    if (kind == ActionBtnKind::Browse) {
                                        DrawD2DFolderVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, effectiveDpi);
                                    } else if (kind == ActionBtnKind::Run) {
                                        DrawD2DReturnArrowVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, effectiveDpi);
                                    } else {
                                        DrawD2DCloseCrossVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, effectiveDpi);
                                    }
                                }

                                pContentBr->Release();
                            }
                        };

                        if (!state->settings.minimalMode) {
                            drawActionButton(layout.rcBrowse, state->i18n.browseBtn, ActionBtnKind::Browse,
                                             state->browseHovered, state->browsePressed);
                        }
                        drawActionButton(layout.rcRun, state->i18n.runBtn, ActionBtnKind::Run,
                                         state->btnHovered, state->btnPressed);
                        if (!state->settings.minimalMode) {
                            drawActionButton(layout.rcClose, state->i18n.exitBtn, ActionBtnKind::Exit,
                                             state->closeHovered, state->closePressed);
                        }
                        drawActionButton(layout.rcGear, nullptr, ActionBtnKind::Gear,
                                         state->gearHovered, state->gearPressed);

                        // 7. Draw Outer Window Liquid Glass Dual Rim (only when custom background is NOT active)
                        if (!hasCustomBg && surface == SurfaceTheme::Glass && !highContrast) {
                            float rimInset = DpiScaleSF(0.6f, state);
                            D2D1_RECT_F rcWindowRim = D2D1::RectF(rimInset, rimInset,
                                                                  (float)w - rimInset, (float)h - rimInset);
                            DrawD2DGlassRim(pRT, rcWindowRim, DpiScaleSF(8.0f, state), effectiveDpi, isLight, 1.35f);
                        }

                        pRT->EndDraw();

                        if (pFmtBtn)   pFmtBtn->Release();
                        if (pFmtInput) pFmtInput->Release();
                        if (pFmtTime)  pFmtTime->Release();
                        if (pFmtRow)   pFmtRow->Release();
                        if (pRoundStroke) pRoundStroke->Release();
                        if (pBrShadow)    pBrShadow->Release();
                        if (pBrHint)      pBrHint->Release();
                        if (pBrTime)      pBrTime->Release();
                        if (pBrSecondary) pBrSecondary->Release();
                        if (pBrPrimary)   pBrPrimary->Release();
                    }

                    // 8. Draw Native App Icons onto the premultiplied DIB for visible rows only
                    int iconSize = DpiScaleS(18, state);
                    int iconX    = DpiScaleS(20, state);
                    int padTop   = GetListPadTop(state);
                    int rowHInt  = GetRowHeight(state);
                    int barH     = GetInputBarHeight(state);

                    for (int visIdx = 0; visIdx < visRows; ++visIdx) {
                        int i = state->scrollOffset + visIdx;
                        if (i >= 0 && i < (int)activeRows.size() && activeRows[i].hIcon) {
                            int iconY = padTop + visIdx * rowHInt + (rowHInt - iconSize) / 2;
                            DrawIconEx(hMemDC, iconX, iconY, activeRows[i].hIcon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
                        }
                    }

                    if (state->hRunIcon) {
                        int runIconY = state->topAreaHeight + (barH - iconSize) / 2;
                        DrawIconEx(hMemDC, iconX, runIconY, state->hRunIcon, iconSize, iconSize, 0, nullptr, DI_NORMAL);
                    }

                    BitBlt(hdc, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
                }
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
        } else if (id == kRunDlgComboId || _wcsicmp(cls, L"ComboBox") == 0 || _wcsicmp(cls, L"ComboBoxEx32") == 0) {
            if (!state->hCombo || id == kRunDlgComboId) {
                state->hCombo = hChild;
                state->hOrigEdit = FindWindowExW(hChild, nullptr, L"Edit", nullptr);
                if (!state->hOrigEdit) {
                    HWND hInnerCombo = FindWindowExW(hChild, nullptr, L"ComboBox", nullptr);
                    if (hInnerCombo) {
                        state->hOrigEdit = FindWindowExW(hInnerCombo, nullptr, L"Edit", nullptr);
                    }
                }
            }
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
    UpdateDialogEditFont(state);
    RecomputeDialogMetrics(state);
    ReloadCustomBackgroundMedia(state, false);

    int dlgW = GetDlgWidth(state);
    int newX = 100;
    int newY = 100;
    GetDefaultTaskbarAnchoredPosition(hDlg, dlgW, state->totalHeight, state->dpi, &newX, &newY);
    ClampDialogToMonitorWorkArea(hDlg, &newX, &newY, dlgW, state->totalHeight, state->dpi);

    state->normalX = newX;
    state->normalY = newY;

    if (NeedsBackdropSnapshot(state)) {
        CaptureCleanMonitorBackdrop(hDlg, IsWindowVisible(hDlg) != FALSE);
        state->backdropLightSample = SampleBackdropLuminanceFromSnap(
            newX, newY, dlgW, state->totalHeight, false);
        state->lastTopologySig = GetBackdropWindowTopologySignature(hDlg);
    }
    state->pendingTopologyRecapture = false;

    ApplyWindowAppearance(hDlg, state);

    state->hModernEdit = CreateWindowExW(
        0, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        -32000, -32000, DpiScale(400, state->dpi), DpiScale(24, state->dpi),
        hDlg, (HMENU)(INT_PTR)kModernEditId, GetModuleHandleW(nullptr), nullptr
    );

    if (state->hModernEdit) {
        SendMessageW(state->hModernEdit, WM_SETFONT, (WPARAM)state->hFontEditCtl, FALSE);

        state->suppressSuggestionUpdate = true;
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
        state->suppressSuggestionUpdate = false;
        SetWindowSubclass(state->hModernEdit, ModernEditSubclassProc, 1, (DWORD_PTR)state);
    }

    SetWindowSubclass(hDlg, RunDlgSubclassProc, 1, (DWORD_PTR)state);
    SetTimer(hDlg, kTimerCaret, 530, nullptr);
    SetTimer(hDlg, kTimerLuminance, 150, nullptr);

    SetWindowPos(hDlg, nullptr, newX, newY, dlgW, state->totalHeight,
                 SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    ULONGLONG now = GetTickCount64();
    g_openTick.store(now);
    g_lastToggleTick.store(now);
    g_winRReleasedSinceOpen.store((GetAsyncKeyState('R') & 0x8000) == 0);
    g_isBrowsingFile.store(false);
    g_dlgIsForeground.store(true);
    g_deactivatedByTaskbarTick.store(0);
    g_hTrackedRunDlg.store(hDlg);
    g_hActiveRunDlg.store(hDlg);

    StartKeyHookThread();

    if (state->hModernEdit) {
        SetFocus(state->hModernEdit);
    }
    InvalidateRect(hDlg, nullptr, TRUE);
}

// --- Run Dialog Detection & Hooking ---

bool IsRunDialogWindow(HWND hDlg) {
    if (!hDlg || !IsWindow(hDlg)) return false;

    wchar_t dlgCls[32] = {};
    if (!GetClassNameW(hDlg, dlgCls, ARRAYSIZE(dlgCls)) || wcscmp(dlgCls, L"#32770") != 0) {
        return false;
    }

    return GetDlgItem(hDlg, kRunDlgComboId) != nullptr &&
           GetDlgItem(hDlg, kRunDlgBrowseId) != nullptr &&
           GetDlgItem(hDlg, IDOK) != nullptr &&
           GetDlgItem(hDlg, IDCANCEL) != nullptr;
}

bool IsShell32DialogCallback(DLGPROC lpDialogFunc) {
    if (!lpDialogFunc || !g_hShell32) return false;
    HMODULE hMod = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(lpDialogFunc),
            &hMod) || !hMod) {
        return false;
    }
    return hMod == g_hShell32;
}

INT_PTR CALLBACK FallbackRunDlgProcWrapper(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool ShouldInterceptDialogCreation(HINSTANCE, LPCWSTR lpTemplateName, DLGPROC lpDialogFunc) {
    if (g_unloading.load() || !lpDialogFunc || lpDialogFunc == FallbackRunDlgProcWrapper) return false;
    if (g_isBrowsingFile.load()) return false;
    if (HWND hActive = g_hActiveRunDlg.load(); IsActiveRunDialogAlive(hActive)) return false;
    return lpTemplateName && IS_INTRESOURCE(lpTemplateName) &&
           LOWORD(reinterpret_cast<ULONG_PTR>(lpTemplateName)) == kRunDialogResourceId &&
           IsShell32DialogCallback(lpDialogFunc);
}

thread_local DLGPROC tl_pendingShellDlgProc = nullptr;

INT_PTR CALLBACK FallbackRunDlgProcWrapper(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto origProc = reinterpret_cast<DLGPROC>(GetPropW(hDlg, L"WindhawkOrigShellDlgProc"));
    if (!origProc && tl_pendingShellDlgProc) {
        origProc = tl_pendingShellDlgProc;
        tl_pendingShellDlgProc = nullptr;
        SetPropW(hDlg, L"WindhawkOrigShellDlgProc", reinterpret_cast<HANDLE>(origProc));
        g_hFallbackInterceptedDlg.store(hDlg);
    }

    if (uMsg == WM_INITDIALOG) {
        // Hand DWLP_DLGPROC back to the original dialog procedure immediately so
        // the window no longer points to FallbackRunDlgProcWrapper after WM_INITDIALOG.
        if (origProc) {
            SetWindowLongPtrW(hDlg, DWLP_DLGPROC, reinterpret_cast<LONG_PTR>(origProc));
        }

        INT_PTR ret = origProc ? origProc(hDlg, uMsg, wParam, lParam) : FALSE;

        // If the original WM_INITDIALOG handler subclassed or chained DWLP_DLGPROC on top of
        // FallbackRunDlgProcWrapper, ensure DWLP_DLGPROC does not remain stuck on our wrapper.
        if (origProc &&
            reinterpret_cast<DLGPROC>(GetWindowLongPtrW(hDlg, DWLP_DLGPROC)) == FallbackRunDlgProcWrapper) {
            SetWindowLongPtrW(hDlg, DWLP_DLGPROC, reinterpret_cast<LONG_PTR>(origProc));
        }

        if (!g_unloading.load() && IsRunDialogWindow(hDlg)) {
            TransformRunDialog(hDlg);
            if (HWND hModernEdit = GetDlgItem(hDlg, kModernEditId)) {
                SetFocus(hModernEdit);
                return FALSE;
            }
        } else {
            HWND expected = hDlg;
            g_hFallbackInterceptedDlg.compare_exchange_strong(expected, nullptr);
        }
        return ret;
    }

    INT_PTR ret = origProc ? origProc(hDlg, uMsg, wParam, lParam) : FALSE;

    if (uMsg == WM_NCDESTROY) {
        if (origProc) {
            SetWindowLongPtrW(hDlg, DWLP_DLGPROC, reinterpret_cast<LONG_PTR>(origProc));
        }
        RemovePropW(hDlg, L"WindhawkOrigShellDlgProc");
        HWND expected = hDlg;
        g_hFallbackInterceptedDlg.compare_exchange_strong(expected, nullptr);
    }

    return ret;
}

INT_PTR WINAPI DialogBoxParamW_Hook(HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent,
                                    DLGPROC lpDialogFunc, LPARAM dwInitParam) {
    if (!ShouldInterceptDialogCreation(hInstance, lpTemplateName, lpDialogFunc)) {
        return DialogBoxParamW_Orig(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
    }

    g_fallbackDialogRefCount.fetch_add(1);
    DLGPROC prevPending = tl_pendingShellDlgProc;
    tl_pendingShellDlgProc = lpDialogFunc;
    INT_PTR res = DialogBoxParamW_Orig(hInstance, lpTemplateName, hWndParent,
                                       FallbackRunDlgProcWrapper, dwInitParam);
    tl_pendingShellDlgProc = prevPending;
    g_fallbackDialogRefCount.fetch_sub(1);
    return res;
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    HWND hActive = g_hActiveRunDlg.load();
    // Fallback: transform the Run dialog on first show if it wasn't caught by DialogBoxParamW_Hook.
    if (!g_unloading.load() && !g_isBrowsingFile.load() &&
        !IsActiveRunDialogAlive(hActive) &&
        nCmdShow != SW_HIDE && hWnd && IsWindow(hWnd) &&
        GetWindowThreadProcessId(hWnd, nullptr) == GetCurrentThreadId() &&
        (GetWindowLongW(hWnd, GWL_STYLE) & WS_CHILD) == 0 &&
        !GetPropW(hWnd, L"WindhawkModernRunApplied") &&
        IsRunDialogWindow(hWnd)) {
        TransformRunDialog(hWnd);
        hActive = g_hActiveRunDlg.load();
    }
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
            POINT pt = {};
            GetCursorPos(&pt);
            RECT rcDlg = {};
            GetWindowRect(hWnd, &rcDlg);
            if (!PtInRect(&rcDlg, pt)) {
                if (ShouldMinimizeOnTaskbarClick(hWnd, now)) {
                    PostMessageW(hActive, g_msgToggle, kActionMinimize, 0);
                } else {
                    PostMessageW(hActive, g_msgToggle, kActionRestore, 0);
                }
                return;
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
            POINT pt = {};
            GetCursorPos(&pt);
            RECT rcDlg = {};
            GetWindowRect(hWnd, &rcDlg);
            if (!PtInRect(&rcDlg, pt)) {
                if (ShouldMinimizeOnTaskbarClick(hWnd, now)) {
                    PostMessageW(hActive, g_msgToggle, kActionMinimize, 0);
                } else {
                    PostMessageW(hActive, g_msgToggle, kActionRestore, 0);
                }
                return TRUE;
            }
        }
    }
    return SetForegroundWindow_Orig(hWnd);
}

void ReleaseGlobalResources() {
    ReleaseBackdropSnapshot();
    {
        std::lock_guard<std::mutex> lock(g_catalogMutex);
        for (auto& cand : g_appCatalog) {
            if (cand.cachedIcon) {
                DestroyIcon(cand.cachedIcon);
                cand.cachedIcon = nullptr;
            }
        }
        g_appCatalog.clear();
        g_appCatalog.shrink_to_fit();
    }
    if (g_pDWriteFactory) {
        g_pDWriteFactory->Release();
        g_pDWriteFactory = nullptr;
    }
    if (g_pD2DFactory) {
        g_pD2DFactory->Release();
        g_pD2DFactory = nullptr;
    }
    if (g_fluentMenuClassRegistered) {
        UnregisterClassW(kFluentMenuClassName, GetModInstanceHandle());
        g_fluentMenuClassRegistered = false;
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

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = FluentMenuWndProc;
    wc.hInstance = GetModInstanceHandle();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kFluentMenuClassName;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"Failed to register Fluent menu window class (error %lu)", GetLastError());
        ReleaseGlobalResources();
        return FALSE;
    }
    g_fluentMenuClassRegistered = true;

    LoadSettings(false);
    g_msgUninit = RegisterWindowMessageW(L"Windhawk_Uninit_" WH_MOD_ID);
    g_msgToggle = RegisterWindowMessageW(L"Windhawk_Toggle_" WH_MOD_ID);

    g_hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!g_hShell32) {
        Wh_Log(L"Failed to get shell32.dll module handle");
        ReleaseGlobalResources();
        return FALSE;
    }

    // Clean up legacy symbol cache value if present
    Wh_DeleteValue(L"SkipSymbolShell32Sig");

    // Hook DialogBoxParamW to intercept the Run dialog (resource #1003 in shell32), including when
    // shell32 loads the template from a localized .dll.mui instance handle.
    WindhawkUtils::SetFunctionHook(DialogBoxParamW, DialogBoxParamW_Hook, &DialogBoxParamW_Orig);
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
    CloseActiveGearMenu();

    HWND hDlg = g_hTrackedRunDlg.load();
    if (!hDlg) hDlg = g_hActiveRunDlg.load();
    while (hDlg && IsWindow(hDlg) &&
           SendMessageW(hDlg, g_msgUninit, 0, 0) != 1) {
        Sleep(30);
    }

    // If a fallback-intercepted dialog is still open (even if not transformed), close it
    // so its modal DialogBoxParamW loop exits promptly before Wh_ModUninit waits on ref count.
    if (HWND hFallback = g_hFallbackInterceptedDlg.exchange(nullptr)) {
        if (IsWindow(hFallback)) {
            CloseRunDialog(hFallback);
        }
    }

    for (int waitTick = 0; waitTick < 40 && g_fallbackDialogRefCount.load() > 0; ++waitTick) {
        if (hDlg && IsWindow(hDlg)) {
            PostMessageW(hDlg, WM_CLOSE, 0, 0);
            PostMessageW(hDlg, WM_NULL, 0, 0);
        }
        Sleep(25);
    }

    StopKeyHookThread();
}

void Wh_ModUninit() {
    while (g_fallbackDialogRefCount.load() > 0) {
        Sleep(50);
    }
    StopKeyHookThread();
    ReleaseGlobalResources();
}
