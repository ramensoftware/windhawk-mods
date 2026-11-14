// ==WindhawkMod==
// @id              modern-run-dialog-ui
// @name            Modern Run Dialog UI For Windows 10 & 11
// @name:vi-VN      Giao diện Hộp thoại Run Hiện đại cho Windows 10 & 11
// @description     Replaces the classic Win + R dialog with a floating Liquid Glass / Fluent pill launcher, Compact Mini-Pill mode, instant calculator, multi-language Google Translate, and smart app suggestions.
// @description:vi-VN Làm mới hộp thoại Run (Win + R) với giao diện viên thuốc nổi Liquid Glass / Fluent hiện đại, chế độ thu nhỏ siêu gọn, tính toán nhanh, dịch đa ngôn ngữ (Google Dịch) và gợi ý ứng dụng.
// @version         5.0.104
// @author          caotu2k5
// @github          https://github.com/caotu2k5
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -luser32 -lshell32 -lshlwapi -lcomctl32 -lversion -limm32 -lcomdlg32 -ld2d1 -ldwrite -ladvapi32 -lole32 -luuid -lwindowscodecs
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Modern Run Dialog UI For Windows 10 & 11

![Modern Run Dialog UI Preview (Minimal Mode)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/5.0.104/50104minimalmode.png)

*Compact Mini-Pill Mode*

![Modern Run Dialog UI Preview (Liquid Glass)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/5.0.104/50104liquidglass.png)

*Liquid Glass mode*

![Modern Run Dialog UI Preview (Windows Blur)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/5.0.104/50104windowsblur.png)

*Windows Blur mode*

![Modern Run Dialog UI Preview (App Suggestions)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/5.0.104/50104recommendprogram.png)

*App & command suggestions*

![Modern Run Dialog UI Preview (Google Search)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/5.0.104/50104googlesearch.png)

*Google Search (`google:`)*

![Modern Run Dialog UI Preview (Menu Mod)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/5.0.104/50104menumod.png)

*Menu Mod*

![Modern Run Dialog UI Preview (Translate Mode)](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-run-dialog/main/5.0.104/50104translatemode.png)

*Translate Mode*

---

## 🌐 English

Replaces the classic Win32 **Run (`Win + R`)** dialog with a floating, borderless **Liquid Glass / Fluent** pill launcher rendered with **Direct2D & DirectWrite** (`Segoe UI Variable Text`). Works across **Windows 10** and **Windows 11** (including the latest Insider Canary/Dev builds with `.dll.mui` localized resources) without downloading `shell32.pdb`.

### Key Features

- **Floating Pill Design & 3 Surface Themes (Liquid Glass, Windows Blur & Opaque):**
  * Synchronized floating capsule command bar and detached rounded pill card for History, App Suggestions, Math Results, and Translations, complete with smooth cubic ease-out/ease-in Open & Close animations.
  * Three surface styles: **Liquid Glass** (3D Snell's-law optical edge refraction, chromatic dispersion, refractive rim highlights, `1/4`-res text-dissolving Gaussian blur, and adjustable tint from `-100` Max Transparent default to `100` High Contrast), **Windows Blur** (translucent Windows 11 context-menu Acrylic styling with default `255` opacity, adjustable `0`–`255`), and **Opaque**, with automatic Light/Dark mode and real-time desktop backdrop luminance detection.
  * **Smart Taskbar-Aware Centering:** Automatically centers horizontally right above a bottom taskbar (expanding history/suggestions upward), or anchors at the top-center of the screen when the taskbar is on the left, right, or top (keeping the input bar fixed at the top while history, suggestions, translations, and menus expand downward).

- **Compact Mini-Pill Mode (`300px` – `360px`):**
  * Enabled by default and toggleable via the 3D Liquid Glass switch in the `⚙️` menu (or Windhawk settings): transforms the dialog into a sleek `340px` mini-pill containing the Run logo, compact input field, and circular icon-only **Run (`↵`)**, **Google Translate (`文A`)**, and **Settings (`⚙️`)** buttons.
  * Hides initial command history when idle for an ultra-clean floating pill look, while automatically keeping **App Suggestions** and **Instant Math Calculator** active as soon as you type.

- **Instant Math Calculator:**
  * Type any mathematical expression directly into the Run input bar (e.g., `1+1`, `(25*4)/2`, `sqrt(144)`, `sin(pi/2)`, `2^10`) to evaluate and display the live result at the top of the suggestion card; press `Enter` or click the result row to copy/insert it immediately.

- **Built-in Multi-Language Google Translate (`文A`):**
  * Click the dedicated **Google Translate (`文A`)** icon button on the input bar (between Run and Settings) to toggle **Translate Mode** with a dynamically expanding multi-line input field, source/target language selector pills (22 languages + Auto Detect) with a one-click swap (`⇄`) button, and a detached translation result card (click the result card to copy translated text to the clipboard).

- **Synchronized In-App Quick Settings Menu (`⚙️`):**
  * Pill-styled Liquid Glass / Fluent popup menu featuring 3D **Liquid Glass Toggle Switches** for boolean features (**Compact Mini-Pill Mode** and **Show Command History**, which automatically locks ON and dims when Compact Mode is active) and crisp checkmarks (`✓`) for single-choice submenu options.
  * Includes inline `-` / `[value]` / `+` steppers and direct keyboard input for Width (`px`), Height (`px`), Font Size (`pt`), Liquid Glass tint (`-100` – `100`), and Windows Blur opacity (`0` – `255`), plus one-click history clearing and `.exe` file browsing.

- **Live App Suggestions & Unified Web Search (`google:`):**
  * Real-time, zero-latency suggestions for Start Menu applications, Registry `App Paths`, and built-in system utilities (`cmd`, `powershell`, `regedit`, `wt`, `.cpl`, `.msc`) as you type.
  * Type `google:<keyword>` to search Google immediately in your default system browser.

- **Lightweight History (Up to 20 Items), Right-Click Deletion & Timestamps:**
  * Stores up to **20 recent commands** in a 5-row scrollable pill card (supports mouse wheel, draggable scrollbar, `Up`/`Down`/`PageUp`/`PageDown`, **right-click context menu** or `Delete` key to remove individual entries, and execution timestamps).

- **Native Vietnamese IME (`RICHEDIT50W` / TSF) & Auto-Focus on `Win + R`:**
  * Uses `RICHEDIT50W` (`msftedit.dll`) for native Windows 11 TSF inline composition—eliminating character jumping, popup boxes, and Space-bar delay on the built-in Windows Vietnamese keyboard (Telex/VNI) as well as third-party IMEs—and automatically focuses the input box on `Win + R` so you can type immediately without clicking.

- **Multi-Language Support (21 Languages Across Dialog, Menu & Grouped Settings):**
  * Auto-detects your Windows UI language or lets you choose from 21 built-in languages across the Run dialog, Compact Mode, Translate Mode, and `⚙️` menu (English, Vietnamese, Japanese, Korean, Simplified/Traditional Chinese, Thai, Indonesian, Malay, French, German, Spanish, Portuguese, Italian, Russian, Ukrainian, Polish, Dutch, Turkish, Hindi, and Arabic), with cleanly organized settings groups in Windhawk.

---

## 🇻🇳 Tiếng Việt

Thay thế hộp thoại **Run (`Win + R`)** cổ điển của Windows bằng giao diện viên thuốc nổi **Liquid Glass / Fluent** không viền, vẽ trực tiếp bằng **Direct2D & DirectWrite** (`Segoe UI Variable Text`). Tương thích mượt mà với cả **Windows 10** và **Windows 11** (bao gồm các bản Windows 11 Insider mới nhất dùng tài nguyên `.dll.mui`) mà không cần tải `shell32.pdb`.

### Tính năng chính

- **Thiết kế Viên thuốc nổi đồng bộ & 3 Chế độ bề mặt (Liquid Glass, Làm mờ Windows & Nền đặc):**
  * Đồng bộ kiểu dáng viên thuốc bo tròn giữa thanh nhập lệnh và hộp hiển thị Lịch sử / Gợi ý ứng dụng / Kết quả tính toán / Bản dịch tách rời, đi kèm hiệu ứng chuyển động đóng/mở (Open & Close animation) mượt mà.
  * Hỗ trợ 3 chế độ bề mặt: **Liquid Glass** (khúc xạ ánh sáng vát cạnh 3D, tán sắc lăng kính, viền phản quang, làm mờ nền xóa nhòa chữ phía sau và chỉnh độ đậm từ `-100` Trong suốt tối đa mặc định đến `100` Tương phản cao), **Làm mờ Windows – Windows Blur** (trong trẻo như menu chuột phải của Windows 11 với độ đậm mặc định `255`, chỉnh từ `0` – `255`) và **Nền đặc (Opaque)**, tự động chuyển đổi Sáng / Tối theo hệ thống và độ sáng màn hình nền.
  * **Tự động căn giữa sát Taskbar thông minh:** Tự động đặt hộp thoại nằm sát phía trên Taskbar ở chính giữa màn hình (mở rộng hộp lịch sử/đề xuất lên trên); nếu Taskbar đặt ở bên trái, bên phải hoặc phía trên thì thanh nhập sẽ cố định ở chính giữa đỉnh màn hình và tự động xổ ngược hộp lịch sử, gợi ý ứng dụng, kết quả dịch cũng như menu cài đặt xuống phía dưới.

- **Chế độ thu nhỏ – Compact Mini-Pill Mode (`300px` – `360px`):**
  * Mặc định bật sẵn (có thể bật/tắt bằng cần gạt Liquid Glass trong menu `⚙️` hoặc cài đặt Windhawk): thu gọn hộp thoại thành một viên thuốc nhỏ xinh (`340px`) chỉ gồm logo Run, ô nhập ngắn gọn và 3 nút tròn chỉ hiện biểu tượng: **Chạy (`↵`)**, **Google Dịch (`文A`)** và **Cài đặt (`⚙️`)**.
  * Tự động ẩn danh sách lịch sử ban đầu khi chưa gõ phím để giữ giao diện tối giản, nhưng vẫn giữ nguyên tính năng **Gợi ý ứng dụng** và **Tính toán nhanh** ngay khi bạn bắt đầu nhập.

- **Máy tính toán học tức thì (Instant Math Calculator):**
  * Gõ trực tiếp phép tính vào ô nhập Run (ví dụ `1+1`, `(25*4)/2`, `sqrt(144)`, `sin(pi/2)`, `2^10`) để xem ngay kết quả tính toán ở đầu hộp gợi ý; nhấn `Enter` hoặc nhấp chuột vào dòng kết quả để sao chép và điền ngay vào ô nhập.

- **Dịch đa ngôn ngữ trực tiếp bằng Google Dịch (`文A`):**
  * Nhấn vào nút biểu tượng **Google Dịch (`文A`)** trên thanh nhập (nằm giữa nút Chạy và nút Cài đặt) để chuyển sang **Chế độ Dịch** với ô nhập tự động giãn chiều cao nhiều dòng, thanh chọn ngôn ngữ nguồn/đích (22 ngôn ngữ + Tự động phát hiện) kèm nút đảo chiều (`⇄`) và thẻ hiển thị bản dịch tách rời (nhấp vào thẻ kết quả để sao chép nhanh).

- **Menu Cài đặt nhanh đồng bộ (`⚙️`) với Công tắc Liquid Glass:**
  * Menu cài đặt nhanh bo tròn đồng bộ với thanh nhập, sử dụng **cần gạt bật/tắt (Toggle Switch) hiệu ứng Liquid Glass 3D** cho các tính năng bật/tắt (**Chế độ thu nhỏ** và **Hiển thị lịch sử lệnh** – tự động bật sẵn và làm mờ khóa lại khi đang ở Chế độ thu nhỏ) cùng dấu tích `✓` chuẩn xác cho các mục chọn đơn trong menu con.
  * Tích hợp nút `-` / `[giá trị]` / `+` để chỉnh trực tiếp Chiều rộng (`px`), Chiều cao (`px`), Cỡ chữ (`pt`), Độ đậm Liquid Glass (`-100` – `100`) và Độ mờ Windows Blur (`0` – `255`), duyệt chọn tệp `.exe` hoặc xóa lịch sử ngay lập tức.

- **Gợi ý ứng dụng theo thời gian thực & Tìm kiếm nhanh (`google:`):**
  * Tự động gợi ý tức thì các ứng dụng trong Start Menu, `App Paths` và công cụ hệ thống (`cmd`, `powershell`, `regedit`, `wt`, `.cpl`, `.msc`) ngay khi gõ mà không gây trễ phím.
  * Gõ `google:<từ khóa>` để tìm kiếm Google nhanh bằng trình duyệt mặc định của hệ thống.

- **Lịch sử 20 mục nhẹ bộ nhớ, Chuột phải xóa từng mục & Thời gian thực thi:**
  * Lưu tối đa **20 lệnh gần đây** trong khung cuộn 5 dòng nhẹ bộ nhớ (hỗ trợ lăn chuột, kéo thanh cuộn, phím `Lên`/`Xuống`/`PageUp`/`PageDown`, **nhấp chuột phải chọn Xóa mục này khỏi lịch sử** hoặc nhấn phím `Delete` để xóa từng mục), kèm biểu tượng ứng dụng và thời gian chạy lệnh.

- **Tương thích hoàn hảo Bộ gõ Tiếng Việt mặc định Windows (`RICHEDIT50W`) & Tự động Focus:**
  * Sử dụng `RICHEDIT50W` (`msftedit.dll`) hỗ trợ TSF nội tuyến của Windows 11 giúp gõ tiếng Việt trên cả bàn phím mặc định của Windows lẫn bộ gõ bên thứ 3 (UniKey, EVKey) mượt mà, không nhấp nháy, không nhảy chữ hay trễ phím cách (`Space`), đồng thời tự động đưa con trỏ vào ô nhập ngay khi nhấn `Win + R`.

- **Hỗ trợ 21 ngôn ngữ hiển thị & Cài đặt Windhawk gom nhóm gọn gàng:**
  * Tự động nhận diện ngôn ngữ Windows hoặc chọn trong 21 ngôn ngữ cho toàn bộ hộp thoại Run, Chế độ thu nhỏ, Chế độ Dịch và menu `⚙️`, đồng thời gom nhóm gọn gàng toàn bộ tùy chọn trong bảng cài đặt của Windhawk.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- appearanceGroup:
  - surfaceTheme: glass
    $name: Surface Theme
    $name:vi-VN: Hiệu ứng bề mặt (Surface Theme)
    $description: Choose between Liquid Glass, Windows Blur, or Off – Opaque surface.
    $description:vi-VN: Chọn giữa hiệu ứng Liquid Glass, Làm mờ Windows (Blur) hoặc Tắt làm mờ (Nền đặc).
    $options:vi-VN:
    - glass: Liquid Glass (Trong suốt khúc xạ)
    - blur: Làm mờ Windows (Windows Blur)
    - solid: Tắt làm mờ (Nền đặc)
    $options:
    - glass: Liquid Glass
    - blur: Windows Blur
    - solid: Off – Opaque
  - colorMode: auto
    $name: Color Mode (Light / Dark)
    $name:vi-VN: Chế độ màu Sáng / Tối
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
  - glassOpacity: -100
    $name: Liquid Glass Opacity / Tint (-100 to 100)
    $name:vi-VN: Độ đậm / Trong suốt của Liquid Glass (-100 đến 100)
    $description: Adjusts Liquid Glass transparency from Max Transparent (-100, default) and Clear (0) to Tinted (60–100).
    $description:vi-VN: Điều chỉnh độ trong suốt của Liquid Glass từ Trong suốt tối đa (-100, mặc định), Chuẩn (0) đến Đậm (60–100).
  - blurOpacity: 255
    $name: Windows Blur Tint Opacity (0–255)
    $name:vi-VN: Độ đậm nền Làm mờ Windows (0–255)
    $description: Translucent Acrylic blur tint when using Windows Blur (0–255, default 255 matches the translucent Windows 11 right-click context menu).
    $description:vi-VN: Độ đậm lớp phủ trong suốt khi dùng Làm mờ Windows (0–255, mặc định 255 trong trẻo như menu chuột phải của Windows 11).
  - language: auto
    $name: Display Language
    $name:vi-VN: Ngôn ngữ hiển thị
    $description: Auto-detect the system’s UI language or choose a fixed language.
    $description:vi-VN: Tự động theo hệ điều hành hoặc chọn ngôn ngữ cố định.
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
  $name: 1. Appearance & Visual Effects
  $name:vi-VN: 1. Giao diện & Hiệu ứng hình ảnh

- layoutGroup:
  - minimalMode: true
    $name: Compact Mini-Pill Mode (340px Mini Pill)
    $name:vi-VN: Chế độ thu nhỏ (Viên thuốc nhỏ 340px)
    $description: Compact 340px mini-pill bar with icon-only Run, Translate, and Gear buttons and short placeholder, keeping live App Suggestions & Calculator.
    $description:vi-VN: Thu gọn thành viên thuốc nhỏ (340px) gồm logo Run, ô nhập ngắn gọn, nút Chạy/Dịch/Cài đặt dạng icon, vẫn giữ gợi ý ứng dụng và tính toán.
  - uiScalePercent: "100"
    $name: UI Size / Scale Preset (85% – 120%)
    $name:vi-VN: Tỷ lệ kích thước giao diện có sẵn (85% – 120%)
    $description: Adjust the overall size of the Run dialog (85% Compact, 100% Default, 110% Medium, 120% Large).
    $description:vi-VN: Điều chỉnh tỷ lệ kích thước tổng thể của hộp thoại Run (85% Nhỏ gọn, 100% Mặc định, 110% Vừa, 120% Lớn).
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
  - customDialogWidth: 0
    $name: Custom Dialog Width in px (0 = Follow Preset, or 360–1200)
    $name:vi-VN: Chiều rộng hộp thoại tùy chỉnh (0 = Theo thiết lập sẵn, hoặc 360–1200 px)
    $description: Manually set the Run box width in pixels/DIPs (e.g. 560, 616, 720). Set to 0 to follow the UI Size preset.
    $description:vi-VN: Tự chỉnh chiều rộng hộp thoại Run theo pixel/DIP (ví dụ 560, 616, 720). Để 0 nếu muốn dùng theo mức 85%–120%.
  - customBarHeight: 0
    $name: Custom Input Bar Height in px (0 = Follow Preset, or 36–96)
    $name:vi-VN: Chiều cao thanh nhập tùy chỉnh (0 = Theo thiết lập sẵn, hoặc 36–96 px)
    $description: Manually set the command bar height (36–96 px, default 54). Set to 0 to follow the UI Size preset.
    $description:vi-VN: Tự chỉnh chiều cao thanh nhập lệnh (36–96 px, chuẩn 54 px). Để 0 để theo mức có sẵn.
  - customFontSize: 0
    $name: Custom Font Size in pt (0 = Follow Preset, or 9–24)
    $name:vi-VN: Cỡ chữ tùy chỉnh (0 = Theo thiết lập sẵn, hoặc 9–24 pt)
    $description: Manually set the text font size (9–24 pt, default 14). Set to 0 to follow the UI Size preset.
    $description:vi-VN: Tự chỉnh cỡ chữ hiển thị trong hộp thoại Run (9–24 pt, chuẩn 14). Để 0 để theo mức kích thước có sẵn.
  $name: 2. Layout & Sizing
  $name:vi-VN: 2. Bố cục & Kích thước

- historyBehaviorGroup:
  - maxHistoryItems: 20
    $name: Max Stored History Items (0 = Off, or 1–20)
    $name:vi-VN: Số mục lịch sử lưu tối đa (0 = Tắt lịch sử, hoặc 1–20)
    $description: Maximum number of recent commands or suggestions stored and scrollable in the list (0–20, default 20 for lighter memory).
    $description:vi-VN: Số lượng lệnh lịch sử hoặc gợi ý tối đa được lưu (0–20, mặc định 20 mục giúp nhẹ bộ nhớ).
  - visibleHistoryRows: 5
    $name: Visible Rows Before Scrolling (3–10)
    $name:vi-VN: Số dòng lịch sử hiển thị cùng lúc trước khi cuộn (3–10)
    $description: Maximum number of rows visible at once before showing the scrollbar (default 5 rows).
    $description:vi-VN: Số dòng hiển thị tối đa cùng lúc trước khi hiện thanh cuộn (mặc định 5 dòng).
  - showTimestamp: true
    $name: Show Execution Timestamp on History Rows
    $name:vi-VN: Hiển thị thời gian thực thi ở mỗi dòng lịch sử
    $description: Display the execution timestamp and clock icon on each history row.
    $description:vi-VN: Hiển thị thời gian chạy lệnh gần nhất và biểu tượng đồng hồ ở mỗi dòng lịch sử.
  - clearInitialInput: true
    $name: Clear Input Box on Open
    $name:vi-VN: Để trống ô nhập khi mới mở
    $description: Show placeholder text on open instead of pre-filling the last executed command.
    $description:vi-VN: Để trống ô nhập (hiển thị dòng gợi ý) khi mới mở thay vì điền sẵn lệnh cũ.
  - clearStoredHistory: false
    $name: Clear Stored History Now
    $name:vi-VN: Xóa toàn bộ lịch sử đã lưu
    $description: Turn on and save to clear stored history, or click "Clear History" in the Gear menu.
    $description:vi-VN: Bật và bấm Lưu để xóa lịch sử đã lưu, hoặc bấm trực tiếp trong menu Bánh răng.
  - taskbarClickAction: minimize
    $name: Taskbar Icon Click Action
    $name:vi-VN: Hành vi khi bấm biểu tượng Run trên Taskbar
    $description: Action when clicking the Run icon on the Taskbar while open — minimize/restore or close.
    $description:vi-VN: Hành động khi bấm vào biểu tượng Run dưới Taskbar lúc cửa sổ đang mở.
    $options:vi-VN:
    - minimize: Ẩn & Mở lại cửa sổ Run
    - close: Đóng hẳn cửa sổ Run
    $options:
    - minimize: Minimize – Restore
    - close: Close Window
  - closeOnClickOutside: false
    $name: Close on Click Outside
    $name:vi-VN: Tự động đóng khi bấm ra ngoài cửa sổ
    $description: Automatically close the Run dialog when clicking outside the window.
    $description:vi-VN: Tự động đóng hộp thoại Run khi click chuột ra ngoài cửa sổ.
  $name: 3. History & Behavior
  $name:vi-VN: 3. Lịch sử lệnh & Hành vi cửa sổ
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
#include <propsys.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
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
IDWriteRenderingParams* g_pNaturalRenderParams = nullptr;
HWINEVENTHOOK g_hWinEventHook = nullptr;

// Returns Windows 11 "Segoe UI Variable Text" (matching Image 1 typography) with automatic fallback to "Segoe UI"
const wchar_t* GetPreferredFluentFontFamily() {
    static int s_hasVariableFont = -1;
    if (s_hasVariableFont == -1 && g_pDWriteFactory) {
        IDWriteFontCollection* pCollection = nullptr;
        if (SUCCEEDED(g_pDWriteFactory->GetSystemFontCollection(&pCollection, FALSE)) && pCollection) {
            UINT32 idx = 0;
            BOOL exists = FALSE;
            if (SUCCEEDED(pCollection->FindFamilyName(L"Segoe UI Variable Text", &idx, &exists)) && exists) {
                s_hasVariableFont = 1;
            } else {
                s_hasVariableFont = 0;
            }
            pCollection->Release();
        }
    }
    return (s_hasVariableFont == 1) ? L"Segoe UI Variable Text" : L"Segoe UI";
}

void ApplyFluentTextRenderingToRT(ID2D1RenderTarget* pRT) {
    if (!pRT) return;
    pRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    if (g_pNaturalRenderParams) {
        pRT->SetTextRenderingParams(g_pNaturalRenderParams);
    }
}

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
    bool minimalMode = true;
    int uiScalePercent = 100;
    int customDialogWidth = 0;
    int customBarHeight = 0;
    int customFontSize = 0;
    bool taskbarCloseInsteadOfMinimize = false;
    int glassOpacity = -100;
    int blurOpacity = 255;
    std::wstring customBackgroundPath;
    int customBackgroundMaxMb = 50;
    int customBackgroundOpacity = 255;
    bool enableAppSuggestions = true;
    int maxHistoryItems = 20;
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
    kActionReloadSettings = 4,
    kActionTranslateDone = 5,
    kActionDeferredInputUpdate = 6
};

// Base dimensions at 96 DPI
constexpr int kBaseDlgWidth         = 616;
constexpr int kBaseCompactDlgWidth  = 340;
constexpr int kBaseRowHeight        = 38;
constexpr int kBaseListPadTop       = 10;
constexpr int kBaseListPadBottom    = 10;
constexpr int kBaseDetachedGap      = 12;
constexpr int kBaseInputBarHeight   = 54;
constexpr UINT_PTR kTimerCaret      = 9010;
constexpr UINT_PTR kTimerLuminance  = 9011;
constexpr UINT_PTR kTimerOpenAnim   = 9014;
constexpr UINT_PTR kTimerBgMedia    = 9012;
constexpr int kModernEditId         = 9901;
constexpr size_t kMaxMruEntries     = 20;

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
    int topAreaH = 0;
    int gapH = 0;
    UINT dpi = 0;
    int glassPct = -9999;
    bool isLight = false;
    bool isMenu = false;
    bool anchoredTop = false;
    uint8_t interiorAlpha = 255;
    std::vector<int16_t> interiorDxG;
    std::vector<InteriorRowSpan> interiorSpans;
    std::vector<BevelPixelLUT> bevelPixels;
};

struct PreblurredMonitorCache {
    std::vector<DWORD> softRGB;
    std::vector<DWORD> crispRGB;
    std::vector<DWORD> softPremul;
    std::vector<DWORD> quarterRGB;
    std::vector<DWORD> blurTmp;
    int monX = 0;
    int monY = 0;
    int monW = 0;
    int monH = 0;
    int halfW = 0;
    int halfH = 0;
    int quarterW = 0;
    int quarterH = 0;
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
    ID2D1Bitmap* pCachedBmp = nullptr;
    ID2D1RenderTarget* pOwnerRT = nullptr;
    int wndX = -99999;
    int wndY = -99999;
    int w = 0;
    int h = 0;
    int topAreaH = -1;
    int gapH = -1;
    int glassPct = -9999;
    bool isLight = false;
    bool isMenu = false;
    bool anchoredTop = false;
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

void ReleaseOpticalCacheBitmap(OpticalGlassCache& c) {
    if (c.pCachedBmp) {
        c.pCachedBmp->Release();
        c.pCachedBmp = nullptr;
    }
    c.pOwnerRT = nullptr;
}

void ReleaseBackdropSnapshot() {
    {
        std::lock_guard<std::mutex> lock(g_desktopSnapMutex);
        g_desktopSnap = {};
    }
    std::lock_guard<std::mutex> lock(g_opticalCacheMutex);
    g_dlgMonitorBlurCache = {};
    g_menuMonitorBlurCache = {};
    ReleaseOpticalCacheBitmap(g_dlgOpticalCache);
    ReleaseOpticalCacheBitmap(g_menuMainOpticalCache);
    ReleaseOpticalCacheBitmap(g_menuSubOpticalCache);
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

void BuildOpticalLensGeometryLUT(OpticalLensGeometryLUT& lut, int w, int h, int topAreaH, int gapH,
                                 UINT dpi, int glassPct, bool isLight, bool isMenuPanel, bool anchoredTop = false) {
    lut.w = w;
    lut.h = h;
    lut.topAreaH = topAreaH;
    lut.gapH = gapH;
    lut.dpi = dpi;
    lut.glassPct = glassPct;
    lut.isLight = isLight;
    lut.isMenu = isMenuPanel;
    lut.anchoredTop = anchoredTop;

    bool isWinBlurMode = (glassPct <= -1000);
    float negF = (!isWinBlurMode && glassPct < 0) ? (std::clamp(-glassPct, 0, 100) / 100.0f) : 0.0f;
    const float zoom = isWinBlurMode ? 1.0f : (1.018f + 0.022f * negF);
    const float invZoomDelta = 1.0f - (1.0f / zoom);

    // Full 255 alpha inside the glass shapes so 0% of unblurred sharp desktop text leaks through,
    // while keeping the glass crystal-clear via ultra-low tintWeight in EnsurePreblurredMonitorCache.
    lut.interiorAlpha = 255;

    const float cx = w * 0.5f;
    lut.interiorDxG.resize(w);
    for (int x = 0; x < w; ++x) {
        float dx = (x + 0.5f) - cx;
        float baseShiftX = -dx * invZoomDelta;
        lut.interiorDxG[x] = (int16_t)std::round(((float)x + baseShiftX) * 0.5f);
    }

    lut.interiorSpans.clear();
    lut.bevelPixels.clear();
    lut.bevelPixels.reserve((size_t)(w + h) * 56);

    int histCardH = (!isMenuPanel && topAreaH > gapH) ? (topAreaH - gapH) : 0;

    for (int y = 0; y < h; ++y) {
        float panelTop = 0.0f;
        float panelBot = (float)h;
        // Menu panel uses the exact same rounded pill-card curvature (20 DIPs) and lens refraction as the Input Bar!
        float cornerR  = (std::min)(((float)dpi * 20.0f / 96.0f), (panelBot - panelTop) * 0.5f);
        float bevelW   = isWinBlurMode ? 1.5f : (((float)dpi / 96.0f) * (13.0f + 5.0f * negF));
        float maxDisp  = isWinBlurMode ? 0.0f : (((float)dpi / 96.0f) * (11.0f + 13.0f * negF));
        bool isHistorySquircle = false;

        if (!isMenuPanel) {
            float maxPillCorner = (float)dpi * 27.0f / 96.0f;
            int barH = (std::max)(2, h - topAreaH);
            float barPillRadius = (std::min)(maxPillCorner, (std::max)(8.0f, (float)barH * 0.5f));
            if (!anchoredTop) {
                if (histCardH > 0 && y < histCardH) {
                    // Panel 1 (Top): Detached History / Suggestions / Translation Card
                    panelTop = 0.0f;
                    panelBot = (float)histCardH;
                    cornerR  = (std::min)(maxPillCorner, (panelBot - panelTop) * 0.5f);
                } else if (histCardH > 0 && y < topAreaH) {
                    // Transparent separation gap between History Card and Pill Input Bar
                    continue;
                } else {
                    // Panel 2 (Bottom): Pill Command Input Bar
                    panelTop = (float)topAreaH;
                    panelBot = (float)h;
                    cornerR  = barPillRadius;
                }
            } else {
                if (y < barH) {
                    // Panel 1 (Top): Pill Command Input Bar stays fixed at the top when anchoredTop is true!
                    panelTop = 0.0f;
                    panelBot = (float)barH;
                    cornerR  = barPillRadius;
                } else if (histCardH > 0 && y < barH + gapH) {
                    // Transparent separation gap between Top Pill Input Bar and Bottom Detached Card
                    continue;
                } else if (histCardH > 0) {
                    // Panel 2 (Bottom): Detached History / Suggestions / Translation Card expands downward!
                    panelTop = (float)(barH + gapH);
                    panelBot = (float)h;
                    cornerR  = (std::min)(maxPillCorner, (panelBot - panelTop) * 0.5f);
                } else {
                    continue;
                }
            }
            bevelW  = isWinBlurMode ? 1.5f : (((float)dpi / 96.0f) * (13.0f + 5.0f * negF));
            maxDisp = isWinBlurMode ? 0.0f : (((float)dpi / 96.0f) * (11.0f + 13.0f * negF));
        }

        float panelH = (std::max)(2.0f, panelBot - panelTop);
        float cy = panelTop + panelH * 0.5f;
        float hxBox = (std::max)(0.0f, cx - cornerR);
        float hyBox = (std::max)(0.0f, panelH * 0.5f - cornerR);

        float dy = (y + 0.5f) - cy;
        float qy = std::fabs(dy) - hyBox;
        float oy = (std::max)(qy, 0.0f);
        float baseShiftY = -dy * invZoomDelta;
        int rowDyG = (int)std::round(((float)y + baseShiftY) * 0.5f);

        int spanStart = -1;
        for (int x = 0; x < w; ++x) {
            float dx = (x + 0.5f) - cx;
            float qx = std::fabs(dx) - hxBox;
            float ox = (std::max)(qx, 0.0f);

            float outDist = std::sqrt(ox * ox + oy * oy);
            float inDist  = (std::min)((std::max)(qx, qy), 0.0f);
            float sdf     = outDist + inDist - cornerR;

            // Outside the rounded pill/squircle boundary -> 100% transparent
            if (sdf >= 0.65f) {
                if (spanStart >= 0) {
                    lut.interiorSpans.push_back({ y, spanStart, x, rowDyG });
                    spanStart = -1;
                }
                continue;
            }

            float edgeAA = std::clamp(0.65f - sdf, 0.0f, 1.0f);
            float distFromEdge = (std::max)(0.0f, -sdf);
            float tBevel = (distFromEdge < bevelW) ? (1.0f - distFromEdge / bevelW) : 0.0f;

            if (tBevel <= 0.0f && edgeAA >= 0.999f) {
                if (spanStart < 0) spanStart = x;
                continue;
            }
            if (spanStart >= 0) {
                lut.interiorSpans.push_back({ y, spanStart, x, rowDyG });
                spanStart = -1;
            }

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
            float lensCurve = tBevel * tBevel * (0.38f + 0.62f * tBevel);
            float dispX = -nx * lensCurve * maxDisp;
            float dispY = -ny * lensCurve * maxDisp;

            BevelPixelLUT bp{};
            bp.dstIdx = (uint32_t)((size_t)y * w + x);
            bp.dxR = (int16_t)std::round(((float)x + baseShiftX + dispX * 0.86f) * 0.5f);
            bp.dyR = (int16_t)std::round(((float)y + baseShiftY + dispY * 0.86f) * 0.5f);
            bp.dxG = (int16_t)std::round(((float)x + baseShiftX + dispX * 1.00f) * 0.5f);
            bp.dyG = (int16_t)std::round(((float)y + baseShiftY + dispY * 1.00f) * 0.5f);
            bp.dxB = (int16_t)std::round(((float)x + baseShiftX + dispX * 1.14f) * 0.5f);
            bp.dyB = (int16_t)std::round(((float)y + baseShiftY + dispY * 1.14f) * 0.5f);

            // Keep crispMix subtle and using blurred crispRGB so background text is NEVER legible at the edges
            if (tBevel > 0.45f) {
                float cm = std::clamp((tBevel - 0.45f) / 0.55f, 0.0f, 1.0f);
                cm = cm * cm * 0.28f;
                bp.crispMix = (uint8_t)std::clamp((int)std::round(cm * 255.0f), 0, 255);
            } else {
                bp.crispMix = 0;
            }

            float lightTL = (std::max)(0.0f, -0.68f * nx - 0.73f * ny);
            float lightBR = (std::max)(0.0f,  0.68f * nx + 0.73f * ny);

            // For Image 2 (History squircle), add a 3D molded inner cushion highlight ring;
            // For Image 1 (Pill Input Bar), use a sleek specular rim glow at the curved ends.
            float fresnel = 0.0f;
            if (isHistorySquircle) {
                float rimGlow = std::pow(tBevel, 2.2f) * (0.28f + 0.36f * lightTL + 0.26f * lightBR);
                float cushionPeak = std::exp(-std::pow((tBevel - 0.58f) / 0.22f, 2.0f)) * 0.14f;
                fresnel = std::clamp(rimGlow + cushionPeak, 0.0f, 0.78f);
            } else {
                fresnel = std::clamp(std::pow(tBevel, 2.1f) * (0.18f + 0.38f * lightTL + 0.28f * lightBR), 0.0f, 0.76f);
            }

            uint32_t frU8 = (uint32_t)std::clamp((int)std::round(fresnel * 256.0f), 0, 256);
            bp.invFr = (uint16_t)(256u - frU8);
            bp.frR_255 = (uint16_t)std::round((238.0f + 17.0f * lightBR) * frU8);
            bp.frG_255 = (uint16_t)std::round((246.0f - 6.0f * lightBR) * frU8);
            bp.frB_255 = (uint16_t)(255u * frU8);

            bp.alpha = (uint8_t)std::clamp((int)std::round(edgeAA * 255.0f), 1, 255);
            bp.trR_a = bp.alpha;
            bp.trG_a = bp.alpha;
            bp.trB_a = bp.alpha;

            bp.sheenR = 0;
            bp.sheenG = 0;
            bp.sheenB = 0;
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
        const int halfW = (std::max)(8, monW / 2);
        const int halfH = (std::max)(8, monH / 2);
        const int quarterW = (std::max)(4, monW / 4);
        const int quarterH = (std::max)(4, monH / 4);

        mcache.monX = monX;
        mcache.monY = monY;
        mcache.monW = monW;
        mcache.monH = monH;
        mcache.halfW = halfW;
        mcache.halfH = halfH;
        mcache.quarterW = quarterW;
        mcache.quarterH = quarterH;
        mcache.softRad = softRad;
        mcache.crispRad = crispRad;
        mcache.glassPct = glassPct;
        mcache.bakedAlpha = interiorAlpha;
        mcache.isLight = isLight;
        mcache.snapTick = snapTick;

        const size_t totalHalf = (size_t)halfW * (size_t)halfH;
        const size_t totalQuarter = (size_t)quarterW * (size_t)quarterH;
        mcache.softRGB.resize(totalHalf);
        mcache.crispRGB.resize(totalHalf);
        mcache.softPremul.resize(totalHalf);
        mcache.quarterRGB.resize(totalQuarter);
        mcache.blurTmp.resize(totalQuarter);

        // Downsample 4x4 into quarterRGB (16x fewer pixels than full-res -> ultra-fast 0.2ms blur!)
        const DWORD* snapPx = g_desktopSnap.pixels.data();
        for (int qy = 0; qy < quarterH; ++qy) {
            int sy0 = std::clamp(qy * 4,     0, monH - 1);
            int sy1 = std::clamp(qy * 4 + 1, 0, monH - 1);
            int sy2 = std::clamp(qy * 4 + 2, 0, monH - 1);
            int sy3 = std::clamp(qy * 4 + 3, 0, monH - 1);
            const DWORD* r0 = &snapPx[(size_t)sy0 * monW];
            const DWORD* r1 = &snapPx[(size_t)sy1 * monW];
            const DWORD* r2 = &snapPx[(size_t)sy2 * monW];
            const DWORD* r3 = &snapPx[(size_t)sy3 * monW];
            DWORD* dstQ = &mcache.quarterRGB[(size_t)qy * quarterW];
            for (int qx = 0; qx < quarterW; ++qx) {
                int sx0 = std::clamp(qx * 4,     0, monW - 1);
                int sx1 = std::clamp(qx * 4 + 2, 0, monW - 1);
                DWORD p0 = r0[sx0], p1 = r0[sx1], p2 = r1[sx0], p3 = r1[sx1];
                DWORD p4 = r2[sx0], p5 = r2[sx1], p6 = r3[sx0], p7 = r3[sx1];
                DWORD r = (((p0 >> 16) & 0xFF) + ((p1 >> 16) & 0xFF) + ((p2 >> 16) & 0xFF) + ((p3 >> 16) & 0xFF) +
                           ((p4 >> 16) & 0xFF) + ((p5 >> 16) & 0xFF) + ((p6 >> 16) & 0xFF) + ((p7 >> 16) & 0xFF)) >> 3;
                DWORD g = (((p0 >> 8) & 0xFF)  + ((p1 >> 8) & 0xFF)  + ((p2 >> 8) & 0xFF)  + ((p3 >> 8) & 0xFF)  +
                           ((p4 >> 8) & 0xFF)  + ((p5 >> 8) & 0xFF)  + ((p6 >> 8) & 0xFF)  + ((p7 >> 8) & 0xFF))  >> 3;
                DWORD b = ((p0 & 0xFF)         + (p1 & 0xFF)         + (p2 & 0xFF)         + (p3 & 0xFF)         +
                           (p4 & 0xFF)         + (p5 & 0xFF)         + (p6 & 0xFF)         + (p7 & 0xFF))         >> 3;
                dstQ[qx] = (r << 16) | (g << 8) | b;
            }
        }
    }

    // Blur at 1/4 resolution: radius 9..14 at 1/4-res equals 36px..56px Gaussian blur at full screen resolution!
    // This completely melts away any text behind the dialog while running 4x faster.
    std::vector<DWORD> crispQuarter = mcache.quarterRGB;
    BlurBufferSeparableRGB(mcache.quarterRGB, mcache.blurTmp, mcache.quarterW, mcache.quarterH, softRad);
    BlurBufferSeparableRGB(crispQuarter,      mcache.blurTmp, mcache.quarterW, mcache.quarterH, crispRad);

    // Crystal-clear vibrancy & translucent tint (supports both Liquid Glass and Windows 11 Context-Menu Acrylic Blur!)
    bool isWinBlurMode = (glassPct <= -1000);
    int winBlurOp = isWinBlurMode ? std::clamp(-(glassPct + 1000), 0, 255) : 0;
    float negF = (!isWinBlurMode && glassPct < 0) ? (std::clamp(-glassPct, 0, 100) / 100.0f) : 0.0f;
    float posF = (!isWinBlurMode && glassPct > 0) ? (std::clamp(glassPct, 0, 100) / 100.0f) : 0.0f;
    const float satBoost = isWinBlurMode
        ? 1.26f
        : (1.15f + 0.10f * negF - 0.08f * posF);
    const float tintWeight = isWinBlurMode
        ? (0.06f + (winBlurOp / 255.0f) * 0.22f)
        : ((glassPct < 0) ? (0.025f + 0.08f * (1.0f - negF)) : (0.11f + 0.44f * posF));
    const int satScale = (int)std::round(satBoost * 256.0f);
    const int tintW = (int)std::round(tintWeight * 256.0f);
    const int invTintW = 256 - tintW;
    const int tintR = (isLight ? 246 : 18) * tintW;
    const int tintG = (isLight ? 248 : 20) * tintW;
    const int tintB = (isLight ? 254 : 28) * tintW;
    const uint32_t intA = interiorAlpha;

    auto applyVibrancyAndTint = [&](std::vector<DWORD>& qBuf) {
        for (size_t i = 0; i < qBuf.size(); ++i) {
            DWORD p = qBuf[i];
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
            qBuf[i] = ((DWORD)r << 16) | ((DWORD)g << 8) | (DWORD)b;
        }
    };
    applyVibrancyAndTint(mcache.quarterRGB);
    applyVibrancyAndTint(crispQuarter);

    // Smooth bilinear upsample from 1/4-res to 1/2-res buffers
    const int qw = mcache.quarterW;
    const int qh = mcache.quarterH;
    const int hw = mcache.halfW;
    const int hh = mcache.halfH;
    for (int hy = 0; hy < hh; ++hy) {
        int qy0 = std::clamp(hy >> 1, 0, qh - 1);
        int qy1 = std::clamp(qy0 + (hy & 1), 0, qh - 1);
        const DWORD* sRow0 = &mcache.quarterRGB[(size_t)qy0 * qw];
        const DWORD* sRow1 = &mcache.quarterRGB[(size_t)qy1 * qw];
        const DWORD* cRow0 = &crispQuarter[(size_t)qy0 * qw];
        const DWORD* cRow1 = &crispQuarter[(size_t)qy1 * qw];
        DWORD* dstSoft   = &mcache.softRGB[(size_t)hy * hw];
        DWORD* dstPremul = &mcache.softPremul[(size_t)hy * hw];
        DWORD* dstCrisp  = &mcache.crispRGB[(size_t)hy * hw];

        for (int hx = 0; hx < hw; ++hx) {
            int qx0 = std::clamp(hx >> 1, 0, qw - 1);
            int qx1 = std::clamp(qx0 + (hx & 1), 0, qw - 1);

            {
                DWORD p00 = sRow0[qx0], p10 = sRow0[qx1], p01 = sRow1[qx0], p11 = sRow1[qx1];
                uint32_t r = (((p00 >> 16) & 0xFF) + ((p10 >> 16) & 0xFF) + ((p01 >> 16) & 0xFF) + ((p11 >> 16) & 0xFF)) >> 2;
                uint32_t g = (((p00 >> 8) & 0xFF)  + ((p10 >> 8) & 0xFF)  + ((p01 >> 8) & 0xFF)  + ((p11 >> 8) & 0xFF))  >> 2;
                uint32_t b = ((p00 & 0xFF)         + (p10 & 0xFF)         + (p01 & 0xFF)         + (p11 & 0xFF))         >> 2;
                dstSoft[hx] = (r << 16) | (g << 8) | b;
                dstPremul[hx] = (intA << 24)
                    | (((r * intA) >> 8) << 16)
                    | (((g * intA) >> 8) << 8)
                    |  ((b * intA) >> 8);
            }
            {
                DWORD p00 = cRow0[qx0], p10 = cRow0[qx1], p01 = cRow1[qx0], p11 = cRow1[qx1];
                uint32_t r = (((p00 >> 16) & 0xFF) + ((p10 >> 16) & 0xFF) + ((p01 >> 16) & 0xFF) + ((p11 >> 16) & 0xFF)) >> 2;
                uint32_t g = (((p00 >> 8) & 0xFF)  + ((p10 >> 8) & 0xFF)  + ((p01 >> 8) & 0xFF)  + ((p11 >> 8) & 0xFF))  >> 2;
                uint32_t b = ((p00 & 0xFF)         + (p10 & 0xFF)         + (p01 & 0xFF)         + (p11 & 0xFF))         >> 2;
                dstCrisp[hx] = (r << 16) | (g << 8) | b;
            }
        }
    }
}

// Renders zero-latency Optical Liquid Glass onto pRT using cached GPU bitmap & 1/4-res text-dissolving blur
void DrawOpticalLiquidGlassBackdrop(ID2D1RenderTarget* pRT, HWND hWnd, int w, int h,
                                    int topAreaH, int gapH,
                                    UINT dpi, int glassPct, bool isLight, bool isMenuPanel, int cacheSlot,
                                    int overrideWndX = -999999, int overrideWndY = -999999,
                                    bool anchoredTop = false) {
    if (!pRT || !hWnd || w <= 0 || h <= 0) return;

    int wndX = overrideWndX;
    int wndY = overrideWndY;
    if (wndX == -999999 || wndY == -999999) {
        RECT rcWnd = {};
        GetWindowRect(hWnd, &rcWnd);
        wndX = rcWnd.left;
        wndY = rcWnd.top;
    }

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
    bool needRebuildPixels =
        (cache->w != w || cache->h != h || cache->topAreaH != topAreaH || cache->gapH != gapH ||
         cache->wndX != wndX || cache->wndY != wndY ||
         cache->glassPct != glassPct || cache->isLight != isLight || cache->isMenu != isMenuPanel ||
         cache->anchoredTop != anchoredTop ||
         cache->snapTick != snapTick || (int)cache->premulPixels.size() != w * h);

    if (needRebuildPixels) {
        bool shapeOrStyleChanged = (cache->lut.w != w || cache->lut.h != h ||
                                    cache->lut.topAreaH != topAreaH || cache->lut.gapH != gapH ||
                                    cache->lut.dpi != dpi || cache->lut.glassPct != glassPct ||
                                    cache->lut.isLight != isLight || cache->lut.isMenu != isMenuPanel ||
                                    cache->lut.anchoredTop != anchoredTop);
        if (shapeOrStyleChanged) {
            BuildOpticalLensGeometryLUT(cache->lut, w, h, topAreaH, gapH, dpi, glassPct, isLight, isMenuPanel, anchoredTop);
        }

        // At 1/4-res, softRad = 9..13 equals 36px..52px full-res Gaussian blur -> 100% blurs out any background text!
        float posF = (glassPct > 0) ? (std::clamp(glassPct, 0, 100) / 100.0f) : 0.0f;
        float baseRad = (glassPct <= -1000) ? 12.0f : (9.5f + 2.5f * posF + (isMenuPanel ? 1.0f : 0.0f));
        float softRadF = baseRad * ((float)dpi / 96.0f);
        int softRad = std::clamp((int)std::round(softRadF), 7, 18);
        int crispRad = std::clamp((softRad * 2) / 3, 5, 14);

        EnsurePreblurredMonitorCache(mcache, softRad, crispRad, glassPct, cache->lut.interiorAlpha, isLight, snapTick);
        if (mcache.softRGB.empty() || mcache.halfW <= 0 || mcache.halfH <= 0) return;

        cache->w = w;
        cache->h = h;
        cache->topAreaH = topAreaH;
        cache->gapH = gapH;
        cache->wndX = wndX;
        cache->wndY = wndY;
        cache->glassPct = glassPct;
        cache->isLight = isLight;
        cache->isMenu = isMenuPanel;
        cache->anchoredTop = anchoredTop;
        cache->snapTick = snapTick;
        cache->premulPixels.assign((size_t)w * (size_t)h, 0);

        const int bw = mcache.halfW;
        const int bh = mcache.halfH;
        const int hx0 = (wndX - mcache.monX) >> 1;
        const int hy0 = (wndY - mcache.monY) >> 1;

        // 1. Ultra-fast interior spans
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
        }

        // 2. Sparse 3D Snell's-law bevel pixels with anti-aliased outer capsule/squircle boundary
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

        ReleaseOpticalCacheBitmap(*cache);
    }

    if (!cache->pCachedBmp || cache->pOwnerRT != pRT) {
        ReleaseOpticalCacheBitmap(*cache);
        D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
        if (SUCCEEDED(pRT->CreateBitmap(D2D1::SizeU((UINT32)w, (UINT32)h),
                                        cache->premulPixels.data(), (UINT32)(w * 4),
                                        &bmpProps, &cache->pCachedBmp))) {
            cache->pOwnerRT = pRT;
        }
    }

    if (cache->pCachedBmp) {
        D2D1_RECT_F dstRect = D2D1::RectF(0.0f, 0.0f, (float)w, (float)h);
        pRT->DrawBitmap(cache->pCachedBmp, dstRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
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


struct ExtraLocalizedStrings {
    const wchar_t* compactPillMenu;
    const wchar_t* showHistoryMenu;
    const wchar_t* deleteHistoryItem;
    const wchar_t* compactPlaceholder;
    const wchar_t* translatePlaceholder;
    const wchar_t* compactTranslatePlaceholder;
    const wchar_t* translateBtnLabel;
    const wchar_t* translatingStatus;
    const wchar_t* copiedFeedback;
    const wchar_t* clickToCopyHint;
    const wchar_t* fromPrefix;
    const wchar_t* toPrefix;
};

ExtraLocalizedStrings GetExtraLocalizedStrings(std::wstring lang) {
    lang = ResolveEffectiveLanguageCode(std::move(lang));
    if (lang == L"vi") {
        return {
            L"Chế độ thu nhỏ (Viên thuốc nhỏ)",
            L"Hiển thị lịch sử lệnh",
            L"Xóa mục này khỏi lịch sử",
            L"Nhập tên lệnh",
            L"Nhập văn bản cần dịch rồi nhấn Enter...",
            L"Nhập văn bản dịch...",
            L"Dịch",
            L"Đang dịch...",
            L"   ✓ Đã sao chép",
            L"   (Nhấn để sao chép)",
            L"Từ: ",
            L"Sang: "
        };
    }
    if (lang == L"zh_tw") {
        return {
            L"迷你膠囊模式 (Compact Pill)",
            L"顯示執行歷史記錄",
            L"從歷史記錄中刪除此項",
            L"輸入指令",
            L"輸入要翻譯的文字並按 Enter...",
            L"輸入翻譯文字...",
            L"翻譯",
            L"正在翻譯...",
            L"   ✓ 已複製",
            L"   (點擊複製)",
            L"從: ",
            L"譯為: "
        };
    }
    if (lang == L"zh_cn") {
        return {
            L"迷你胶囊模式 (Compact Pill)",
            L"显示运行历史记录",
            L"从历史记录中删除此项",
            L"输入命令",
            L"输入要翻译的文本并按 Enter...",
            L"输入翻译文本...",
            L"翻译",
            L"正在翻译...",
            L"   ✓ 已复制",
            L"   (点击复制)",
            L"从: ",
            L"译为: "
        };
    }
    if (lang == L"ja") {
        return {
            L"コンパクトピルモード (最小化表示)",
            L"コマンド履歴を表示",
            L"履歴からこの項目を削除",
            L"コマンドを入力",
            L"翻訳するテキストを入力して Enter...",
            L"テキストを翻訳...",
            L"翻訳",
            L"翻訳中...",
            L"   ✓ コピー完了",
            L"   (クリックでコピー)",
            L"元: ",
            L"先: "
        };
    }
    if (lang == L"ko") {
        return {
            L"컴팩트 미니 알약 모드",
            L"명령어 기록 표시",
            L"기록에서 이 항목 삭제",
            L"명령어 입력",
            L"번역할 텍스트를 입력하고 Enter...",
            L"번역할 텍스트...",
            L"번역",
            L"번역 중...",
            L"   ✓ 복사됨",
            L"   (클릭하여 복사)",
            L"원본: ",
            L"대상: "
        };
    }
    if (lang == L"th") {
        return {
            L"โหมดแคปซูลจิ๋ว (Compact Pill)",
            L"แสดงประวัติคำสั่ง",
            L"ลบรายการนี้ออกจากประวัติ",
            L"พิมพ์คำสั่ง",
            L"พิมพ์ข้อความที่ต้องการแปลแล้วกด Enter...",
            L"พิมพ์ข้อความแปล...",
            L"แปล",
            L"กำลังแปล...",
            L"   ✓ คัดลอกแล้ว",
            L"   (คลิกเพื่อคัดลอก)",
            L"จาก: ",
            L"เป็น: "
        };
    }
    if (lang == L"id") {
        return {
            L"Mode Kapsul Mini (Compact Pill)",
            L"Tampilkan Riwayat Perintah",
            L"Hapus item ini dari riwayat",
            L"Ketik perintah",
            L"Ketik teks untuk diterjemahkan lalu tekan Enter...",
            L"Teks terjemahan...",
            L"Terjemah",
            L"Menerjemahkan...",
            L"   ✓ Disalin",
            L"   (Klik untuk menyalin)",
            L"Dari: ",
            L"Ke: "
        };
    }
    if (lang == L"ms") {
        return {
            L"Mod Kapsul Mini (Compact Pill)",
            L"Tunjukkan Sejarah Arahan",
            L"Padam item ini daripada sejarah",
            L"Taip arahan",
            L"Taip teks untuk diterjemahkan dan tekan Enter...",
            L"Teks terjemahan...",
            L"Terjemah",
            L"Menterjemah...",
            L"   ✓ Disalin",
            L"   (Klik untuk salin)",
            L"Dari: ",
            L"Ke: "
        };
    }
    if (lang == L"fr") {
        return {
            L"Mode mini-pilule compacte",
            L"Afficher l'historique des commandes",
            L"Supprimer cet élément de l'historique",
            L"Commande...",
            L"Saisissez le texte à traduire puis Entrée...",
            L"Texte à traduire...",
            L"Traduire",
            L"Traduction...",
            L"   ✓ Copié",
            L"   (Cliquer pour copier)",
            L"De : ",
            L"Vers : "
        };
    }
    if (lang == L"de") {
        return {
            L"Kompakter Mini-Pillen-Modus",
            L"Befehlsverlauf anzeigen",
            L"Eintrag aus Verlauf löschen",
            L"Befehl eingeben",
            L"Text zum Übersetzen eingeben & Enter...",
            L"Text übersetzen...",
            L"Übersetzen",
            L"Übersetze...",
            L"   ✓ Kopiert",
            L"   (Klicken zum Kopieren)",
            L"Von: ",
            L"Nach: "
        };
    }
    if (lang == L"es") {
        return {
            L"Modo mini-píldora compacta",
            L"Mostrar historial de comandos",
            L"Eliminar este elemento del historial",
            L"Escribir comando",
            L"Escriba el texto a traducir y pulse Enter...",
            L"Texto a traducir...",
            L"Traducir",
            L"Traduciendo...",
            L"   ✓ Copiado",
            L"   (Clic para copiar)",
            L"De: ",
            L"A: "
        };
    }
    if (lang == L"pt") {
        return {
            L"Modo mini-pílula compacta",
            L"Mostrar histórico de comandos",
            L"Remover este item do histórico",
            L"Digite comando",
            L"Digite o texto para traduzir e pressione Enter...",
            L"Texto para traduzir...",
            L"Traduzir",
            L"Traduzindo...",
            L"   ✓ Copiado",
            L"   (Clique para copiar)",
            L"De: ",
            L"Para: "
        };
    }
    if (lang == L"it") {
        return {
            L"Modalità mini-pillola compatta",
            L"Mostra cronologia comandi",
            L"Elimina questo elemento dalla cronologia",
            L"Digita comando",
            L"Digita il testo da tradurre e premi Invio...",
            L"Testo da tradurre...",
            L"Traduci",
            L"Traduzione...",
            L"   ✓ Copiato",
            L"   (Clicca per copiare)",
            L"Da: ",
            L"A: "
        };
    }
    if (lang == L"ru") {
        return {
            L"Компактный режим мини-капсулы",
            L"Показывать историю команд",
            L"Удалить этот элемент из истории",
            L"Введите команду",
            L"Введите текст для перевода и нажмите Enter...",
            L"Текст для перевода...",
            L"Перевести",
            L"Перевод...",
            L"   ✓ Скопировано",
            L"   (Нажмите для копирования)",
            L"С: ",
            L"На: "
        };
    }
    if (lang == L"uk") {
        return {
            L"Компактний режим міні-капсули",
            L"Показувати історію команд",
            L"Видалити цей елемент з історії",
            L"Введіть команду",
            L"Введіть текст для перекладу та натисніть Enter...",
            L"Текст для перекладу...",
            L"Перекласти",
            L"Переклад...",
            L"   ✓ Скопійовано",
            L"   (Натисніть для копіювання)",
            L"З: ",
            L"На: "
        };
    }
    if (lang == L"pl") {
        return {
            L"Tryb kompaktowej mini-pigułki",
            L"Pokaż historię poleceń",
            L"Usuń ten element z historii",
            L"Wpisz polecenie",
            L"Wpisz tekst do przetłumaczenia i naciśnij Enter...",
            L"Tekst do tłumaczenia...",
            L"Tłumacz",
            L"Tłumaczenie...",
            L"   ✓ Skopiowano",
            L"   (Kliknij, aby skopiować)",
            L"Z: ",
            L"Na: "
        };
    }
    if (lang == L"nl") {
        return {
            L"Compacte mini-pilmodus",
            L"Opdrachtgeschiedenis tonen",
            L"Verwijder dit item uit de geschiedenis",
            L"Typ opdracht",
            L"Typ tekst om te vertalen en druk op Enter...",
            L"Tekst vertalen...",
            L"Vertalen",
            L"Vertalen...",
            L"   ✓ Gekopieerd",
            L"   (Klik om te kopiëren)",
            L"Van: ",
            L"Naar: "
        };
    }
    if (lang == L"tr") {
        return {
            L"Kompakt Mini Kapsül Modu",
            L"Komut Geçmişini Göster",
            L"Bu öğeyi geçmişten sil",
            L"Komut yazın",
            L"Çevrilecek metni yazıp Enter'a basın...",
            L"Çevrilecek metin...",
            L"Çevir",
            L"Çevriliyor...",
            L"   ✓ Kopyalandı",
            L"   (Kopyalamak için tıklayın)",
            L"Kimden: ",
            L"Kime: "
        };
    }
    if (lang == L"hi") {
        return {
            L"कॉम्पैक्ट मिनी-पिल मोड",
            L"कमांड इतिहास दिखाएं",
            L"इतिहास से इसे हटाएं",
            L"कमांड लिखें",
            L"अनुवाद के लिए टेक्स्ट लिखें और Enter दबाएं...",
            L"अनुवाद टेक्स्ट...",
            L"अनुवाद",
            L"अनुवाद हो रहा है...",
            L"   ✓ कॉपी किया गया",
            L"   (कॉपी करने के लिए क्लिक करें)",
            L"से: ",
            L"में: "
        };
    }
    if (lang == L"ar") {
        return {
            L"وضع الكبسولة المصغرة (Compact Pill)",
            L"إظهار سجل الأوامر",
            L"حذف هذا العنصر من السجل",
            L"اكتب أمراً",
            L"اكتب النص للترجمة واضغط Enter...",
            L"نص للترجمة...",
            L"ترجمة",
            L"جاري الترجمة...",
            L"   ✓ تم النسخ",
            L"   (انقر للنسخ)",
            L"من: ",
            L"إلى: "
        };
    }
    return {
        L"Compact Mini-Pill Mode",
        L"Show Command History",
        L"Delete from history",
        L"Type command",
        L"Type text to translate and press Enter...",
        L"Text to translate...",
        L"Translate",
        L"Translating...",
        L"   ✓ Copied to Clipboard",
        L"   (Click to copy)",
        L"From: ",
        L"To: "
    };
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
            L"Gợi ý ứng dụng & Tìm kiếm google:",
            L"Ngôn ngữ hiển thị (Display Language)",
            L"Tự động theo hệ thống (Auto)",
            L"Xóa toàn bộ lịch sử đã lưu (Clear History)",
            L"Tùy chỉnh Kích thước & Cỡ chữ (Xem trực tiếp)",
            L"Mặc định (Reset)",
            L"Xong (OK)",
            L"Hình nền & Video (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0Tất cả tệp (*.*)\0*.*\0\0",
            L"Chọn ảnh tĩnh, ảnh động GIF hoặc video MP4 làm hình nền (tối đa 20-50 MB)",
            L"Tệp hình nền vượt quá giới hạn dung lượng (%d MB). Vui lòng chọn tệp ảnh/GIF/MP4 nhỏ hơn %d MB.",
            L"Gõ từ khóa để tìm kiếm trên Google (Trình duyệt mặc định)",
            L"Tìm trên Google: \"",
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
            L"แนะนำแอป & ค้นหาด่วน google:",
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
            L"Saran Aplikasi & google:",
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
            L"Cadangan Aplikasi & google:",
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
            L"アプリ候補と検索 google:",
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
            L"应用建议与网页搜索 google:",
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
            L"應用程式建議與搜尋 google:",
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
            L"앱 추천 및 웹 검색 google:",
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
            L"Suggestions d'applis et google:",
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
            L"App-Vorschläge & google:",
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
            L"Sugerencias de apps y google:",
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
            L"Sugestões de apps e google:",
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
            L"Suggerimenti app e google:",
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
            L"Подсказки программ и google:",
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
            L"Підказки програм і google:",
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
            L"Sugestie aplikacji i google:",
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
            L"App-suggesties & google:",
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
            L"Uygulama Önerileri ve google:",
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
            L"ऐप सुझाव और google:",
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
            L"اقتراحات التطبيقات و google:",
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
        L"App Suggestions & google:",
        L"Display Language",
        L"Auto – System Default",
        L"Clear All Stored History",
        L"Custom Size & Font (Live Preview)",
        L"Reset Default",
        L"Done (OK)",
        L"Background Media (*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm)\0*.png;*.jpg;*.jpeg;*.webp;*.bmp;*.gif;*.mp4;*.webm\0All Files (*.*)\0*.*\0\0",
        L"Select static image, animated GIF, or MP4 video background (max 20-50 MB)",
        L"The selected background file exceeds the maximum allowed size (%d MB). Please choose an image/GIF/MP4 under %d MB.",
        L"Type a keyword to search on Google (Default Browser)",
        L"Search Google: \"",
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
        L"language", L"surfaceTheme", L"colorMode", L"uiScalePercent",
        L"customDialogWidth", L"customBarHeight", L"customFontSize",
        L"glassOpacity", L"blurOpacity", L"maxHistoryItems", L"minimalModeV3"
    };
    for (const wchar_t* k : keys) {
        Wh_DeleteValue(GetUserOverrideKey(k).c_str());
    }
}

std::wstring ReadGroupedStringSetting(const wchar_t* groupedKey, const wchar_t* flatKey, const wchar_t* defaultVal) {
    auto grp = WindhawkUtils::StringSetting::make(groupedKey);
    if (grp.get() && grp.get()[0] != L'\0') {
        return grp.get();
    }
    auto flt = WindhawkUtils::StringSetting::make(flatKey);
    if (flt.get() && flt.get()[0] != L'\0') {
        return flt.get();
    }
    return defaultVal ? defaultVal : L"";
}

int ReadGroupedIntSetting(const wchar_t* groupedKey, const wchar_t* flatKey, int defaultVal) {
    int v = Wh_GetIntSetting(groupedKey);
    if (v != 0) return v;
    int vf = Wh_GetIntSetting(flatKey);
    if (vf != 0) return vf;
    return defaultVal;
}

void LoadSettings(bool fromSettingsChanged = false) {
    if (fromSettingsChanged) {
        ClearAllInAppOverrides();
    }

    ModSettings newSettings;
    newSettings.language = ReadGroupedStringSetting(L"appearanceGroup.language", L"language", L"auto");

    wchar_t strOverride[MAX_PATH] = {};
    if (Wh_GetStringValue(GetUserOverrideKey(L"language").c_str(), strOverride, MAX_PATH) > 0 && strOverride[0]) {
        newSettings.language = strOverride;
    }

    std::wstring surfStr = ReadGroupedStringSetting(L"appearanceGroup.surfaceTheme", L"surfaceTheme", L"glass");
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

    std::wstring colStr = ReadGroupedStringSetting(L"appearanceGroup.colorMode", L"colorMode", L"auto");
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

    // Compact Mini-Pill mode (minimalMode) is ON by default; toggleable via Gear menu or Windhawk Settings
    int minModeOverride = Wh_GetIntValue(GetUserOverrideKey(L"minimalModeV3").c_str(), -1);
    if (minModeOverride >= 0) {
        newSettings.minimalMode = (minModeOverride != 0);
    } else {
        int migAck = Wh_GetIntValue(GetUserOverrideKey(L"minimalModeDefaultMigratedV3").c_str(), 0);
        if (migAck == 0) {
            Wh_SetIntValue(GetUserOverrideKey(L"minimalModeDefaultMigratedV3").c_str(), 1);
            newSettings.minimalMode = true;
        } else {
            newSettings.minimalMode = (Wh_GetIntSetting(L"layoutGroup.minimalMode") != 0);
        }
    }

    std::wstring scaleStr = ReadGroupedStringSetting(L"layoutGroup.uiScalePercent", L"uiScalePercent", L"100");
    int scaleSetting = !scaleStr.empty() ? _wtoi(scaleStr.c_str()) : 100;
    if (scaleSetting <= 0) scaleSetting = 100;
    int scaleOverride = Wh_GetIntValue(GetUserOverrideKey(L"uiScalePercent").c_str(), -1);
    newSettings.uiScalePercent = std::clamp(scaleOverride > 0 ? scaleOverride : scaleSetting, 85, 120);

    int widthSetting = ReadGroupedIntSetting(L"layoutGroup.customDialogWidth", L"customDialogWidth", 0);
    int widthOverride = Wh_GetIntValue(GetUserOverrideKey(L"customDialogWidth").c_str(), -1);
    int rawWidth = (widthOverride >= 0) ? widthOverride : widthSetting;
    newSettings.customDialogWidth = (rawWidth > 0) ? std::clamp(rawWidth, 360, 1200) : 0;

    int heightSetting = ReadGroupedIntSetting(L"layoutGroup.customBarHeight", L"customBarHeight", 0);
    int heightOverride = Wh_GetIntValue(GetUserOverrideKey(L"customBarHeight").c_str(), -1);
    int rawHeight = (heightOverride >= 0) ? heightOverride : heightSetting;
    if (rawHeight >= 150) {
        rawHeight = MulDiv(rawHeight, kBaseInputBarHeight, kBaseInputBarHeight + 5 * kBaseRowHeight + kBaseListPadTop + kBaseListPadBottom);
    }
    newSettings.customBarHeight = (rawHeight > 0) ? std::clamp(rawHeight, 36, 96) : 0;

    int fontSetting = ReadGroupedIntSetting(L"layoutGroup.customFontSize", L"customFontSize", 0);
    int fontOverride = Wh_GetIntValue(GetUserOverrideKey(L"customFontSize").c_str(), -1);
    int rawFont = (fontOverride >= 0) ? fontOverride : fontSetting;
    newSettings.customFontSize = (rawFont > 0) ? std::clamp(rawFont, 9, 24) : 0;

    std::wstring tbAct = ReadGroupedStringSetting(L"historyBehaviorGroup.taskbarClickAction", L"taskbarClickAction", L"minimize");
    newSettings.taskbarCloseInsteadOfMinimize = (tbAct == L"close");

    int glassOpSetting = Wh_GetIntSetting(L"appearanceGroup.glassOpacity");
    if (glassOpSetting == 0 && Wh_GetIntSetting(L"glassOpacity") != 0) {
        glassOpSetting = Wh_GetIntSetting(L"glassOpacity");
    }
    if (glassOpSetting < -100 || glassOpSetting > 100) glassOpSetting = -100;
    int glassOpOverride = Wh_GetIntValue(GetUserOverrideKey(L"glassOpacity").c_str(), -999);
    newSettings.glassOpacity = std::clamp((glassOpOverride >= -100 && glassOpOverride <= 100) ? glassOpOverride : glassOpSetting, -100, 100);

    int blurMig255 = Wh_GetIntValue(GetUserOverrideKey(L"blurOpacityDefault255MigratedV1").c_str(), 0);
    if (blurMig255 == 0) {
        Wh_SetIntValue(GetUserOverrideKey(L"blurOpacityDefault255MigratedV1").c_str(), 1);
        int curOv = Wh_GetIntValue(GetUserOverrideKey(L"blurOpacity").c_str(), -1);
        if (curOv == 35 || curOv == 140 || curOv == 225) {
            Wh_SetIntValue(GetUserOverrideKey(L"blurOpacity").c_str(), 255);
        }
    }
    int blurOpSetting = ReadGroupedIntSetting(L"appearanceGroup.blurOpacity", L"blurOpacity", 255);
    if (blurOpSetting == 35 || blurOpSetting == 140 || blurOpSetting == 225) blurOpSetting = 255;
    int blurOpOverride = Wh_GetIntValue(GetUserOverrideKey(L"blurOpacity").c_str(), -1);
    newSettings.blurOpacity = std::clamp(blurOpOverride >= 0 ? blurOpOverride : blurOpSetting, 0, 255);

    // App suggestions & web search are always enabled by default
    newSettings.enableAppSuggestions = true;

    int maxHistSetting = Wh_GetIntSetting(L"historyBehaviorGroup.maxHistoryItems");
    if (maxHistSetting == 0 && !fromSettingsChanged) {
        int legacyMax = Wh_GetIntSetting(L"maxHistoryItems");
        if (legacyMax > 0) maxHistSetting = legacyMax;
    }
    int maxHistOverride = Wh_GetIntValue(GetUserOverrideKey(L"maxHistoryItems").c_str(), -1);
    newSettings.maxHistoryItems = std::clamp(maxHistOverride >= 0 ? maxHistOverride : maxHistSetting, 0, 20);

    int visRowsSetting = ReadGroupedIntSetting(L"historyBehaviorGroup.visibleHistoryRows", L"visibleHistoryRows", 5);
    if (visRowsSetting <= 0) visRowsSetting = 5;
    newSettings.visibleHistoryRows = std::clamp(visRowsSetting, 3, 10);

    newSettings.showTimestamp = (Wh_GetIntSetting(L"historyBehaviorGroup.showTimestamp") != 0) ||
                                (Wh_GetIntSetting(L"showTimestamp") != 0);

    newSettings.clearInitialInput = (Wh_GetIntSetting(L"historyBehaviorGroup.clearInitialInput") != 0) ||
                                    (Wh_GetIntSetting(L"clearInitialInput") != 0);
    newSettings.closeOnClickOutside = (Wh_GetIntSetting(L"historyBehaviorGroup.closeOnClickOutside") != 0) ||
                                      (Wh_GetIntSetting(L"closeOnClickOutside") != 0);

    bool clearRequested = (Wh_GetIntSetting(L"historyBehaviorGroup.clearStoredHistory") != 0) ||
                          (Wh_GetIntSetting(L"clearStoredHistory") != 0);
    const std::wstring& ackKey = GetClearAckStorageKey();
    int ack = Wh_GetIntValue(ackKey.c_str(), 0);
    if (clearRequested) {
        if (ack == 0) {
            if (fromSettingsChanged) {
                ClearCurrentUserStoredHistory();
            }
            Wh_SetIntValue(ackKey.c_str(), 1);
        }
    } else if (ack != 0) {
        Wh_SetIntValue(ackKey.c_str(), 0);
    }

    LocalizedStrings resolvedI18n = GetLocalizedStringsForLanguage(newSettings.language);

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = std::move(newSettings);
        g_i18n = resolvedI18n;
    }
}

// --- Embedded Modern Run Icon & Automatic Windhawk Storage Lifecycle (Install / Update / Uninstall) ---

constexpr wchar_t kRunIconAssetVersion[]  = WH_MOD_VERSION L"-modern-run-icon-v2";
constexpr wchar_t kRunIconIcoFileName[]   = L"modern-run-icon.ico";
constexpr wchar_t kRunIconPngFileName[]   = L"modern-run-icon.png";
constexpr wchar_t kRunIconVersionKey[]    = L"InstalledRunIconVersion";
constexpr wchar_t kOrigRunLnkIconPathKey[] = L"OrigRunLnkIconPath";
constexpr wchar_t kOrigRunLnkIconIdxKey[]  = L"OrigRunLnkIconIndex";
constexpr wchar_t kOrigRunAppId[]         = L"Microsoft.Windows.Shell.RunDialog";
constexpr wchar_t kCustomRunAppId[]       = L"Windhawk.ModernRunDialog.UI";

static const PROPERTYKEY kPKEY_AppUserModel_ID = {
    { 0x9F4C2855, 0x9F79, 0x4B39, { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 } }, 5
};
static const PROPERTYKEY kPKEY_AppUserModel_RelaunchCommand = {
    { 0x9F4C2855, 0x9F79, 0x4B39, { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 } }, 2
};
static const PROPERTYKEY kPKEY_AppUserModel_RelaunchIconResource = {
    { 0x9F4C2855, 0x9F79, 0x4B39, { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 } }, 3
};
static const PROPERTYKEY kPKEY_AppUserModel_RelaunchDisplayNameResource = {
    { 0x9F4C2855, 0x9F79, 0x4B39, { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 } }, 4
};
static const PROPERTYKEY kPKEY_AppUserModel_PreventPinning = {
    { 0x9F4C2855, 0x9F79, 0x4B39, { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 } }, 9
};

#ifndef ICON_SMALL2
#define ICON_SMALL2 2
#endif

constexpr uint8_t kEmbeddedModernRunPng[12323] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x06, 0x00, 0x00, 0x00, 0x5C, 0x72, 0xA8,
    0x66, 0x00, 0x00, 0x2F, 0xEA, 0x49, 0x44, 0x41, 0x54, 0x78, 0xDA, 0xED, 0x7D, 0x79, 0x94, 0x1E,
    0xD5, 0x75, 0xE7, 0xEF, 0xBE, 0xFA, 0xBA, 0x5B, 0xDD, 0xAD, 0x1D, 0xB1, 0x2F, 0x52, 0x83, 0x00,
    0x49, 0x48, 0x48, 0x40, 0x1B, 0x49, 0x20, 0xDB, 0xC4, 0x01, 0x32, 0x71, 0xBC, 0x04, 0x90, 0x04,
    0xC4, 0x83, 0x4D, 0x98, 0xC9, 0xCC, 0xF8, 0xE4, 0xCC, 0x99, 0x33, 0xF1, 0xCC, 0xD8, 0xB1, 0x27,
    0x69, 0xDA, 0x63, 0xFB, 0xC4, 0x1E, 0x67, 0x4E, 0x7C, 0xBC, 0x12, 0x43, 0x4C, 0x9C, 0x64, 0x8C,
    0x2D, 0x16, 0x93, 0x38, 0x76, 0x1C, 0x73, 0x04, 0x42, 0x92, 0x91, 0x98, 0xC6, 0x98, 0x55, 0x66,
    0x13, 0x88, 0x4D, 0x1B, 0x42, 0x48, 0xEA, 0x6E, 0xF5, 0x56, 0xEF, 0xCE, 0x1F, 0xF5, 0x6A, 0xAF,
    0xFA, 0xBE, 0xFA, 0xF6, 0xAA, 0xE6, 0xFE, 0x44, 0xD1, 0xDF, 0x5A, 0xDF, 0xAB, 0x57, 0xEF, 0xFE,
    0xEE, 0xF2, 0xEE, 0xBB, 0x0F, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02,
    0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40,
    0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10,
    0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04,
    0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81,
    0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20,
    0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08,
    0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0xA9, 0xA0, 0x69, 0x7D, 0x75, 0x03, 0x03, 0x6A,
    0xFD, 0xB3, 0x17, 0xD0, 0xBB, 0xE3, 0x62, 0x05, 0x2D, 0x15, 0x15, 0xBE, 0x36, 0x9B, 0x28, 0x51,
    0xFD, 0xE2, 0xB6, 0x21, 0xF0, 0xF8, 0x99, 0x65, 0xE0, 0x5B, 0x07, 0xC1, 0x04, 0x62, 0xB9, 0x27,
    0x29, 0x42, 0x7F, 0xED, 0xFA, 0xBB, 0x2D, 0xE9, 0x08, 0xC1, 0xF4, 0x1E, 0xE6, 0xAC, 0x7E, 0xB4,
    0x81, 0x2D, 0x80, 0xEB, 0xD2, 0x6B, 0x34, 0x9D, 0x3A, 0x64, 0x70, 0xF0, 0x56, 0x00, 0x83, 0xDA,
    0x7D, 0x6D, 0xED, 0x55, 0x0F, 0x2E, 0x06, 0xB0, 0x14, 0xC0, 0x39, 0x00, 0x9D, 0x0A, 0xA0, 0x37,
    0x7E, 0xCD, 0x54, 0x63, 0x27, 0x70, 0xA6, 0xAE, 0xE4, 0xC4, 0xCF, 0xFA, 0x9F, 0xA7, 0xB2, 0x67,
    0x6F, 0x37, 0xC9, 0x53, 0xFA, 0xE5, 0x96, 0x6D, 0x1B, 0x55, 0x38, 0x2B, 0x57, 0x3D, 0x02, 0x19,
    0x9C, 0xA9, 0x3B, 0xB8, 0xCA, 0xD6, 0x84, 0xEF, 0x3E, 0x79, 0xFF, 0x94, 0xF9, 0xBF, 0x32, 0xFF,
    0x57, 0xDE, 0xEB, 0xEE, 0x6B, 0x09, 0x07, 0x3B, 0x9F, 0xB1, 0x02, 0xE7, 0x70, 0x5F, 0x57, 0x80,
    0x79, 0x0E, 0x28, 0x0E, 0x3C, 0x06, 0xC1, 0x32, 0xBF, 0xEC, 0x3C, 0x07, 0x14, 0xF9, 0x6D, 0x36,
    0x7F, 0x27, 0x01, 0x1C, 0x02, 0x78, 0x0F, 0x80, 0xE7, 0x00, 0xFA, 0xCD, 0x17, 0x37, 0x9F, 0x7D,
    0xC4, 0x6D, 0xF5, 0x8F, 0x36, 0xB0, 0xB5, 0x71, 0x13, 0x34, 0x6A, 0xB0, 0x0A, 0xA6, 0x01, 0x01,
    0x30, 0x61, 0xC3, 0x26, 0x85, 0x4D, 0x1B, 0x6D, 0x47, 0xE8, 0x37, 0x2F, 0x07, 0xE8, 0x3A, 0x00,
    0xBF, 0x0B, 0xE0, 0x02, 0x00, 0x33, 0xD3, 0x2F, 0x93, 0xEA, 0xEC, 0x0C, 0x4E, 0xFD, 0x36, 0x57,
    0x49, 0x16, 0x94, 0x3A, 0x80, 0xB9, 0xFD, 0xC2, 0x5F, 0xC3, 0xF5, 0x67, 0xF9, 0x2E, 0x55, 0xDD,
    0xCB, 0xDC, 0xF0, 0xA1, 0x13, 0xA5, 0xE2, 0x10, 0x01, 0x70, 0x02, 0x01, 0x90, 0x4A, 0x14, 0xFC,
    0x34, 0x52, 0x20, 0xEF, 0xBD, 0x38, 0x01, 0x58, 0x40, 0xEC, 0x3D, 0x02, 0xA0, 0xC8, 0xA5, 0xA5,
    0xD4, 0x3E, 0xDA, 0x03, 0x60, 0x1B, 0x80, 0xBB, 0x09, 0xF4, 0x2F, 0x5F, 0xD8, 0xDC, 0x37, 0xE6,
    0x2B, 0x41, 0xD2, 0xEF, 0x1E, 0x02, 0x18, 0x60, 0x05, 0x73, 0xC1, 0x6B, 0xAF, 0xDA, 0xDC, 0x0F,
    0xE0, 0x53, 0x00, 0x3E, 0x0A, 0xA0, 0x3B, 0x70, 0x69, 0xDA, 0x19, 0x39, 0xD4, 0x90, 0x4E, 0xE1,
    0x96, 0x08, 0x99, 0xB8, 0x77, 0x4D, 0xEF, 0x03, 0x4E, 0xB2, 0xC4, 0x8C, 0xF8, 0x73, 0x50, 0x83,
    0xAB, 0x88, 0x35, 0x10, 0xB6, 0x02, 0x42, 0x9A, 0x1E, 0x08, 0x5B, 0x08, 0x1C, 0x15, 0x6E, 0x82,
    0xC5, 0x71, 0xA1, 0x8F, 0x3E, 0x26, 0x63, 0x97, 0x50, 0xB4, 0xB1, 0x8E, 0x65, 0x60, 0x05, 0xDF,
    0x21, 0xD0, 0x53, 0x00, 0xBE, 0x05, 0xE0, 0x6F, 0xBE, 0xB0, 0xB9, 0x6F, 0x62, 0xC3, 0x06, 0xB6,
    0x36, 0x6D, 0x22, 0x7B, 0xDA, 0x13, 0xC0, 0x35, 0xD7, 0xDD, 0x6B, 0xDD, 0x77, 0xCF, 0xB5, 0xF6,
    0xDA, 0xAB, 0x36, 0xCF, 0x06, 0xF0, 0xE7, 0x00, 0xFE, 0x18, 0xC0, 0x0C, 0x73, 0x59, 0xB6, 0xB9,
    0x36, 0xF2, 0xBA, 0x4D, 0x20, 0x04, 0x90, 0xC1, 0x0A, 0x88, 0x13, 0x40, 0xDC, 0x15, 0x88, 0x13,
    0x42, 0x54, 0xCB, 0x27, 0x59, 0x05, 0x8E, 0x81, 0x9E, 0xA6, 0xF9, 0xA3, 0x56, 0x00, 0xA5, 0x0B,
    0x27, 0x93, 0xA3, 0xD0, 0xD8, 0xF0, 0x84, 0x32, 0x8E, 0xCC, 0x4E, 0x00, 0x9F, 0xF9, 0xC2, 0xE6,
    0xBE, 0x87, 0x06, 0x30, 0xA0, 0x06, 0x71, 0x2B, 0x67, 0x71, 0x09, 0x0A, 0x29, 0x19, 0x97, 0x5F,
    0xFD, 0x50, 0x69, 0xFB, 0xBF, 0x5E, 0x31, 0x75, 0xD9, 0xD5, 0x0F, 0x5E, 0x08, 0xC6, 0xF7, 0x18,
    0x7C, 0xB1, 0x79, 0xCB, 0x36, 0x1D, 0x42, 0xF9, 0xBA, 0x64, 0xAE, 0xA3, 0x1D, 0x5C, 0xE5, 0x2D,
    0x6C, 0x84, 0xE0, 0xD0, 0xF4, 0x13, 0xF4, 0xB2, 0x3F, 0xEF, 0x93, 0x80, 0x2B, 0xFC, 0x94, 0xEA,
    0xFB, 0xAB, 0x88, 0xD6, 0xA7, 0x0A, 0x2E, 0x01, 0x8C, 0x1B, 0x90, 0x2C, 0xEC, 0x56, 0x2A, 0x01,
    0x94, 0x8F, 0x11, 0x91, 0x6F, 0x12, 0x68, 0xC7, 0xCA, 0xA5, 0x12, 0x80, 0x29, 0x00, 0x7F, 0xF6,
    0xC5, 0xCD, 0x7D, 0x7F, 0xE1, 0xBC, 0xC9, 0xA8, 0x44, 0x02, 0x85, 0x23, 0x00, 0x57, 0xF3, 0x5F,
    0x76, 0xF5, 0x83, 0x57, 0x02, 0xF8, 0x01, 0x18, 0x0B, 0x18, 0x98, 0x02, 0xD8, 0x12, 0x6D, 0x2F,
    0xA8, 0x8B, 0x00, 0xCC, 0x63, 0x55, 0x67, 0x20, 0x30, 0x6A, 0x11, 0xF8, 0x04, 0x50, 0x99, 0x04,
    0x28, 0x40, 0x02, 0xE5, 0xA3, 0x45, 0x91, 0x30, 0xB2, 0x43, 0x04, 0xE4, 0x28, 0x3F, 0xBE, 0x7D,
    0xE5, 0x09, 0x43, 0xFF, 0x69, 0xE3, 0xA6, 0x0D, 0x26, 0x1E, 0x90, 0x4E, 0x02, 0x54, 0x2C, 0xE1,
    0xBF, 0xCF, 0xBA, 0xEF, 0x9E, 0x6B, 0xEC, 0xCB, 0xAF, 0x7E, 0xE8, 0xB7, 0x19, 0xB8, 0x1F, 0xCC,
    0xBD, 0x00, 0x6C, 0x06, 0x64, 0xDA, 0x4F, 0x50, 0x77, 0x2C, 0x20, 0x99, 0x00, 0xD2, 0xB4, 0xBF,
    0xAA, 0x52, 0xF8, 0xCB, 0xFB, 0xFD, 0xE5, 0x62, 0x00, 0x54, 0x51, 0x74, 0xBD, 0xB9, 0x0C, 0x06,
    0xA0, 0x8D, 0x97, 0xF1, 0xB7, 0xA5, 0xF7, 0xF6, 0xDD, 0x02, 0x00, 0x83, 0x83, 0x48, 0x75, 0x07,
    0x54, 0x91, 0x02, 0x7E, 0x46, 0xF8, 0x57, 0x00, 0xF8, 0x21, 0x18, 0xBD, 0xC6, 0xE4, 0x17, 0xE1,
    0x17, 0x34, 0x9E, 0x13, 0x38, 0x70, 0x64, 0xD1, 0x98, 0x9C, 0xD5, 0xAD, 0xE1, 0x2A, 0x1F, 0x67,
    0x3B, 0x8F, 0xF9, 0x47, 0x0C, 0x58, 0xCC, 0x98, 0x62, 0xC6, 0x27, 0x26, 0xB7, 0xEE, 0xFE, 0xD2,
    0xE0, 0x20, 0xE9, 0x0D, 0x1B, 0xD2, 0xE5, 0xBC, 0x20, 0x04, 0xE0, 0x44, 0x6A, 0x2E, 0xFF, 0x9D,
    0x2D, 0xB3, 0x19, 0xF8, 0x3B, 0x06, 0x4E, 0x70, 0x85, 0x5F, 0xE2, 0xE5, 0x82, 0xC6, 0x47, 0x23,
    0x22, 0x59, 0x18, 0x9C, 0x5D, 0x16, 0x39, 0xF0, 0x5D, 0xAE, 0xBB, 0x35, 0xE1, 0x73, 0xC5, 0x0F,
    0x36, 0xBF, 0x17, 0x25, 0x0F, 0xB6, 0x00, 0xB6, 0xC1, 0xF8, 0xF4, 0x67, 0x3F, 0xF0, 0xD2, 0x75,
    0x9B, 0x36, 0x91, 0xBD, 0xC1, 0x71, 0x91, 0x8B, 0x49, 0x00, 0x03, 0x03, 0xB7, 0x12, 0x06, 0x49,
    0xB3, 0xE6, 0x01, 0x30, 0x56, 0x82, 0x31, 0x25, 0xC2, 0x2F, 0x68, 0xA0, 0xC4, 0x03, 0xCC, 0x09,
    0x26, 0x40, 0x05, 0x45, 0x1F, 0x78, 0x81, 0x23, 0xC2, 0x5F, 0x4E, 0xA4, 0xB9, 0xC2, 0x67, 0x52,
    0x68, 0xA8, 0x02, 0x55, 0x04, 0xFF, 0x81, 0x5C, 0x9B, 0x9F, 0x81, 0xAF, 0xFD, 0xE9, 0x07, 0x5E,
    0x3A, 0xE5, 0x47, 0x80, 0x1E, 0xC0, 0x80, 0x2A, 0x1C, 0x01, 0xAC, 0xDF, 0xB0, 0xC9, 0x1A, 0x1C,
    0x1C, 0xD4, 0x6B, 0xAF, 0x7A, 0xE8, 0x22, 0x06, 0xFE, 0x98, 0x01, 0xCD, 0x80, 0xC5, 0x32, 0x57,
    0x2E, 0x68, 0x22, 0x2B, 0x70, 0x36, 0xA9, 0xCB, 0xF0, 0x0D, 0x0E, 0x59, 0x06, 0xE5, 0x08, 0x81,
    0xE3, 0xA7, 0xAE, 0x68, 0x03, 0x24, 0xFF, 0x2E, 0x03, 0x60, 0x05, 0x62, 0x1B, 0xC0, 0xE9, 0x00,
    0x3E, 0x43, 0x20, 0xC6, 0xC0, 0xAD, 0xC5, 0xB3, 0x00, 0xEE, 0xDE, 0xF4, 0xB4, 0x7B, 0x85, 0x9F,
    0x01, 0xD0, 0x65, 0xAE, 0x4E, 0x42, 0xFD, 0x82, 0x86, 0x89, 0x3B, 0x67, 0x74, 0xBF, 0x39, 0x24,
    0xF1, 0x5C, 0x51, 0xF8, 0x93, 0x0C, 0xF4, 0x4A, 0xBE, 0x7F, 0xD2, 0x3B, 0x5C, 0xF1, 0x08, 0x69,
    0x7F, 0xE7, 0x70, 0x26, 0x01, 0x15, 0x83, 0x35, 0x83, 0x6F, 0xF9, 0xCC, 0x07, 0x5E, 0x5C, 0x3C,
    0x38, 0x48, 0x7A, 0x60, 0x80, 0x55, 0x71, 0x08, 0x60, 0x60, 0x40, 0x01, 0x83, 0x7A, 0xCD, 0x55,
    0x0F, 0x2D, 0x65, 0xC6, 0x87, 0x9D, 0x2B, 0x64, 0x15, 0xBE, 0x63, 0x02, 0x41, 0x03, 0x3C, 0x80,
    0x72, 0x9F, 0x23, 0x4E, 0x8C, 0x07, 0x70, 0xAA, 0x9F, 0xCE, 0xBE, 0x28, 0x53, 0x65, 0xAD, 0xCF,
    0x11, 0x8F, 0x3F, 0x6E, 0x01, 0xD4, 0x16, 0x3B, 0x30, 0xBF, 0xCE, 0x00, 0x66, 0x31, 0x70, 0x33,
    0x00, 0x5C, 0xF0, 0x6C, 0xB8, 0x45, 0xB9, 0x26, 0x80, 0xF5, 0xCF, 0x2C, 0x77, 0x67, 0x37, 0xAE,
    0x07, 0xB8, 0xDB, 0x61, 0x33, 0x2F, 0x4D, 0x5A, 0x20, 0x68, 0x8C, 0xD6, 0x0F, 0xBC, 0xC1, 0xC9,
    0x3A, 0x3F, 0x5D, 0xC4, 0x10, 0x0D, 0x1E, 0xC6, 0x57, 0x81, 0xB8, 0x04, 0xC2, 0xA1, 0x4F, 0x72,
    0x84, 0x30, 0xA2, 0x61, 0xBF, 0x6A, 0x2C, 0x80, 0x34, 0xAB, 0xC0, 0x4D, 0x19, 0x04, 0x03, 0x58,
    0xFF, 0xE9, 0x0F, 0xBC, 0xD8, 0xBD, 0x71, 0x13, 0xD9, 0x1C, 0x58, 0x41, 0x98, 0x67, 0x02, 0xA0,
    0xBB, 0xEF, 0xDE, 0xA0, 0xD7, 0xAF, 0xDF, 0x64, 0xC1, 0x59, 0xD8, 0xE3, 0x99, 0xFE, 0x1C, 0xB8,
    0x89, 0xCC, 0xB5, 0x76, 0x90, 0x1C, 0xEF, 0xD6, 0x23, 0x5D, 0x79, 0xC4, 0x05, 0x2F, 0x29, 0xD8,
    0x86, 0x44, 0x5F, 0x9D, 0x13, 0x85, 0xD0, 0xCF, 0xE3, 0xE7, 0xD0, 0xE7, 0xCA, 0x85, 0xFC, 0xB2,
    0x04, 0x14, 0xAB, 0xB0, 0x71, 0x94, 0x79, 0x74, 0x1E, 0x03, 0xFD, 0x00, 0xB0, 0x71, 0xC3, 0x26,
    0x4F, 0xEE, 0x4B, 0x39, 0xB6, 0xFF, 0x09, 0x18, 0xD4, 0xAF, 0x1D, 0x59, 0xD0, 0x47, 0xC0, 0x72,
    0xDF, 0x98, 0xE2, 0xF2, 0xB6, 0x9C, 0x40, 0x50, 0x85, 0xE9, 0xEF, 0x2C, 0x13, 0xF3, 0x8D, 0xCA,
    0xE0, 0xAA, 0x4C, 0x32, 0x46, 0xB4, 0x2B, 0x90, 0xFE, 0xE7, 0xFC, 0x67, 0xAE, 0x40, 0x87, 0x97,
    0x15, 0xBB, 0xC2, 0x4B, 0x31, 0x52, 0xA0, 0xC0, 0xF7, 0x39, 0xE1, 0x31, 0x85, 0x16, 0x4B, 0xB3,
    0xF9, 0x3C, 0xD5, 0x79, 0xB5, 0xA4, 0xE1, 0xE4, 0xCB, 0x5C, 0x06, 0x60, 0xEB, 0x06, 0x6C, 0xC0,
    0xA6, 0xBC, 0x5B, 0x00, 0xEB, 0xD7, 0xBB, 0xE6, 0x3F, 0xFA, 0x19, 0xE8, 0x31, 0xC1, 0x0C, 0x12,
    0x0D, 0x26, 0x47, 0xDD, 0x16, 0x40, 0x84, 0x09, 0xD2, 0xFC, 0xF5, 0x24, 0x1D, 0x1D, 0xD6, 0xF8,
    0x1C, 0xB3, 0x0E, 0x92, 0x03, 0x7F, 0x1C, 0x8F, 0x25, 0x84, 0x34, 0x7E, 0xDC, 0x1D, 0x40, 0x6A,
    0x90, 0x2F, 0xE3, 0x3F, 0x4E, 0x74, 0x07, 0x2E, 0x02, 0x9C, 0xAA, 0x42, 0x05, 0xB0, 0x00, 0xBC,
    0x8E, 0x5E, 0x47, 0x19, 0xF4, 0xBC, 0x18, 0x01, 0x82, 0x5A, 0xCC, 0x80, 0x24, 0x8D, 0xEF, 0x3E,
    0x71, 0xFE, 0xA4, 0xFD, 0x3F, 0x2C, 0xA4, 0x14, 0x99, 0x9C, 0x0A, 0x0B, 0x1E, 0x45, 0x6C, 0x06,
    0x0A, 0x59, 0x04, 0x3E, 0xA5, 0xF8, 0xBF, 0xC0, 0x69, 0x8C, 0x94, 0xD5, 0x7F, 0x0E, 0x9F, 0xDD,
    0xBD, 0x9A, 0x85, 0x00, 0xC8, 0xD4, 0x0C, 0x20, 0xD4, 0x5B, 0x4E, 0xA8, 0x99, 0xFE, 0x3F, 0x00,
    0x5E, 0xBF, 0x7E, 0x93, 0xF5, 0xEA, 0x91, 0x13, 0x76, 0x02, 0xB8, 0x04, 0x1C, 0x49, 0xFB, 0x95,
    0x99, 0x00, 0x41, 0xDD, 0x43, 0x2C, 0xF8, 0x88, 0x42, 0x4B, 0x82, 0x13, 0x17, 0x04, 0x71, 0xD2,
    0x2A, 0x41, 0x24, 0xD4, 0x05, 0x88, 0x2C, 0x0D, 0xF6, 0xD6, 0xED, 0x52, 0xA8, 0x1A, 0x90, 0xF7,
    0x1A, 0x90, 0xB8, 0x18, 0x88, 0xEA, 0x20, 0x01, 0x5F, 0xF4, 0x09, 0x70, 0x6A, 0x62, 0x28, 0x00,
    0xCF, 0x03, 0xB4, 0xEA, 0xAB, 0x9B, 0x17, 0x1F, 0x77, 0x64, 0x9F, 0x38, 0x9F, 0x16, 0xC0, 0x00,
    0x13, 0x06, 0x89, 0xF7, 0x1C, 0x39, 0xA1, 0x0F, 0x4E, 0x49, 0x2F, 0x90, 0xE7, 0xAE, 0xA4, 0x97,
    0x86, 0x12, 0x4A, 0x10, 0x54, 0xA7, 0x21, 0xC3, 0xBA, 0x32, 0x29, 0x06, 0x10, 0x5C, 0x50, 0xEB,
    0xBF, 0xCF, 0x81, 0xC7, 0x14, 0x4B, 0x4C, 0xE1, 0x88, 0x96, 0x0F, 0x69, 0x7F, 0x22, 0x30, 0x47,
    0x23, 0x0A, 0x14, 0x99, 0x06, 0x0C, 0xC6, 0x24, 0x6A, 0xAB, 0xBA, 0x14, 0x6C, 0x6B, 0xE0, 0x7B,
    0x5D, 0x00, 0x77, 0x02, 0x38, 0xCE, 0x1E, 0x01, 0xE5, 0x10, 0x1B, 0x9E, 0xF5, 0xA2, 0xFD, 0xFD,
    0x0C, 0xF4, 0xC0, 0xC9, 0xFE, 0x23, 0x2F, 0xF2, 0x8F, 0x6C, 0xF9, 0x50, 0x02, 0x41, 0x39, 0x01,
    0x49, 0x4F, 0xC1, 0x89, 0x2A, 0x14, 0x4E, 0xCD, 0xD2, 0x4B, 0xF2, 0xD7, 0x11, 0x23, 0x83, 0xA4,
    0xCF, 0x72, 0x99, 0xB9, 0x7F, 0x4E, 0x9D, 0x55, 0xA8, 0xE6, 0x9F, 0xF6, 0xE4, 0xC5, 0x3B, 0x97,
    0xF7, 0x33, 0x94, 0xE7, 0x18, 0x00, 0xBB, 0x31, 0x4A, 0xA2, 0x75, 0x60, 0x06, 0x03, 0x9A, 0x8A,
    0xB4, 0x72, 0x51, 0x50, 0x20, 0x22, 0xE0, 0xD0, 0x2C, 0x40, 0x92, 0xAE, 0xAD, 0x64, 0x21, 0x84,
    0x2D, 0x03, 0x8A, 0x45, 0xFD, 0xC3, 0x56, 0x41, 0x7C, 0x36, 0x21, 0xE8, 0xF7, 0x73, 0xC2, 0x6C,
    0x42, 0xED, 0xB1, 0x00, 0xD7, 0x74, 0x21, 0xD7, 0x8C, 0x39, 0x0E, 0xD0, 0x38, 0x6A, 0xB2, 0x29,
    0x5A, 0x8C, 0xEB, 0xD6, 0x6F, 0xB2, 0x5E, 0x39, 0xBA, 0x60, 0x07, 0x31, 0xFA, 0x01, 0xD6, 0x60,
    0x28, 0x12, 0xDF, 0x5F, 0xD0, 0x24, 0x87, 0x20, 0x5C, 0x12, 0x34, 0x12, 0x07, 0x48, 0xF4, 0xFD,
    0x83, 0x7E, 0x3F, 0x22, 0x25, 0xC4, 0x92, 0xE2, 0x00, 0xF0, 0x63, 0x01, 0x29, 0x71, 0x00, 0xF2,
    0xE2, 0x00, 0x95, 0x2B, 0x02, 0x55, 0x77, 0x5D, 0xB0, 0x41, 0x64, 0x01, 0xD8, 0xF6, 0x57, 0x9B,
    0xCF, 0x7B, 0x6F, 0x90, 0x11, 0xF2, 0xA7, 0x55, 0x07, 0x9C, 0x15, 0x4B, 0x2F, 0x1D, 0x3F, 0xA9,
    0x8F, 0x09, 0xCB, 0x82, 0xC1, 0x5A, 0x59, 0x00, 0x24, 0x68, 0xA6, 0x4B, 0x90, 0x68, 0xCC, 0x33,
    0x52, 0xF3, 0xF3, 0x83, 0x26, 0x7E, 0xDC, 0xAD, 0x48, 0xCA, 0xF4, 0x8B, 0x4F, 0xFD, 0x05, 0x4D,
    0xFE, 0xF8, 0xAF, 0x73, 0x9D, 0x53, 0x9E, 0x81, 0xEF, 0x33, 0x83, 0x99, 0x5F, 0x02, 0x80, 0x0D,
    0x81, 0x44, 0xA0, 0xFC, 0x11, 0xC0, 0x73, 0xB7, 0x12, 0x00, 0x8C, 0xCE, 0x9E, 0xD1, 0xFF, 0xD6,
    0x79, 0x27, 0xF5, 0x8C, 0xF7, 0x74, 0xE8, 0x3A, 0x33, 0x21, 0x04, 0x82, 0xAA, 0xE2, 0x02, 0x51,
    0xDF, 0x3C, 0x1A, 0x09, 0xE0, 0x84, 0xE7, 0x49, 0xFE, 0x7D, 0x90, 0x2A, 0x92, 0x32, 0xFC, 0xE2,
    0xE4, 0x11, 0x11, 0x78, 0xF6, 0x04, 0xB7, 0xCA, 0xC3, 0x37, 0x94, 0x23, 0xF1, 0x84, 0x21, 0x13,
    0x65, 0x43, 0x7E, 0x09, 0xC0, 0x60, 0xE4, 0xF4, 0xB9, 0xEB, 0xDE, 0xE9, 0x5B, 0x80, 0xB7, 0xCF,
    0x3F, 0x39, 0xD3, 0xA2, 0x4B, 0x81, 0xA0, 0x7E, 0xFD, 0x1F, 0x8E, 0x30, 0x33, 0xC7, 0xC7, 0x1D,
    0x47, 0x44, 0x37, 0x59, 0xFB, 0x97, 0x5F, 0xE4, 0x13, 0x27, 0x01, 0x8E, 0x51, 0x45, 0xAD, 0x29,
    0xEE, 0x21, 0x3A, 0x71, 0xE2, 0x67, 0xEC, 0x94, 0xCC, 0x63, 0x1B, 0xE0, 0x47, 0x01, 0x60, 0x59,
    0x20, 0x11, 0x28, 0x67, 0x04, 0xC0, 0x84, 0xBB, 0xC8, 0xEE, 0xB8, 0x7D, 0xD4, 0x1A, 0x9B, 0xDF,
    0xBB, 0x7A, 0xBC, 0xB7, 0x1B, 0x63, 0xF3, 0x7A, 0x28, 0x71, 0x81, 0x85, 0x8C, 0x5A, 0x41, 0x03,
    0xB5, 0x7E, 0x39, 0xED, 0xC2, 0x19, 0xFF, 0xA2, 0x82, 0xF6, 0x4F, 0x22, 0x01, 0x24, 0x91, 0x02,
    0xC7, 0xCF, 0x54, 0xB3, 0x15, 0xE3, 0xB2, 0x00, 0xB0, 0x87, 0x81, 0x67, 0x00, 0xB7, 0x46, 0x60,
    0x0E, 0x09, 0xA0, 0xF4, 0x8D, 0x31, 0x02, 0x80, 0x19, 0xC7, 0x46, 0xFA, 0x46, 0xE7, 0xF4, 0x2E,
    0x03, 0x80, 0x91, 0x79, 0x33, 0x69, 0x78, 0x76, 0x77, 0x74, 0x1F, 0x87, 0xC4, 0x8B, 0x95, 0x43,
    0x8E, 0x5A, 0xD3, 0x82, 0x93, 0x56, 0xDE, 0x21, 0xC1, 0x15, 0xE0, 0xD4, 0xD8, 0x41, 0xFA, 0x6A,
    0x3F, 0xA4, 0x15, 0xEE, 0xAA, 0xE0, 0x02, 0x84, 0x4D, 0xF8, 0xEA, 0xFE, 0x05, 0x7E, 0x47, 0x9B,
    0xD7, 0x86, 0xBE, 0xFE, 0xE0, 0x92, 0x11, 0xA7, 0x2A, 0x10, 0xE5, 0x93, 0x00, 0xB8, 0xC7, 0x49,
    0x5B, 0x1A, 0xEF, 0xE9, 0xEA, 0xB7, 0x3B, 0x4A, 0x3D, 0x4C, 0xD0, 0xAC, 0x88, 0xDE, 0x59, 0x7A,
    0x4A, 0xE2, 0xCD, 0x11, 0x08, 0x5A, 0x1B, 0x20, 0x4C, 0x7F, 0x2D, 0x7D, 0x8D, 0x1F, 0x47, 0x48,
    0x80, 0x53, 0xE3, 0x05, 0x9C, 0x12, 0x8E, 0xAC, 0xAD, 0xCD, 0x7E, 0xA1, 0x50, 0xE3, 0xDA, 0x6C,
    0x03, 0x80, 0xED, 0x57, 0x7C, 0x20, 0x24, 0xF3, 0xB9, 0xCA, 0x03, 0x20, 0x5B, 0x1B, 0x47, 0x80,
    0xD6, 0xF9, 0x3E, 0x01, 0xF0, 0xF6, 0xC2, 0x05, 0x38, 0xED, 0xD1, 0x57, 0x40, 0xEC, 0xCF, 0x09,
    0xB0, 0x04, 0x04, 0x04, 0x8D, 0x1A, 0x77, 0x81, 0xC1, 0x46, 0x11, 0xBF, 0x9F, 0x42, 0x33, 0xF9,
    0x14, 0xCA, 0x07, 0x08, 0xAE, 0xEE, 0xA3, 0x50, 0x7E, 0x7F, 0x70, 0x7E, 0x3F, 0xBA, 0x12, 0x30,
    0xBC, 0x9A, 0x30, 0x48, 0x12, 0xDE, 0x4A, 0x40, 0x77, 0x05, 0x7F, 0xE0, 0xF7, 0x6A, 0xBC, 0x2E,
    0x86, 0x53, 0x3B, 0x53, 0x83, 0x78, 0x07, 0x00, 0x2C, 0x58, 0x70, 0x88, 0xF3, 0x4A, 0x00, 0x34,
    0xF5, 0x47, 0x3D, 0x36, 0x06, 0xB6, 0x94, 0x40, 0xB4, 0xDA, 0x74, 0x99, 0x67, 0xAB, 0xEC, 0xBF,
    0xE8, 0x4C, 0x9C, 0xFC, 0xAB, 0x57, 0x13, 0xEF, 0x9C, 0x4C, 0x11, 0x08, 0xEA, 0x51, 0xE9, 0xA1,
    0xC5, 0x3C, 0xC4, 0x89, 0x4B, 0x64, 0x82, 0xC9, 0x3A, 0x14, 0x49, 0xDE, 0xA1, 0x50, 0x3A, 0x70,
    0x74, 0xA9, 0x30, 0x05, 0x08, 0x81, 0x63, 0x84, 0x12, 0x25, 0x86, 0xE0, 0xA0, 0x26, 0xE6, 0x7A,
    0x84, 0xDF, 0x0D, 0x69, 0x12, 0x08, 0xAF, 0x00, 0xBC, 0x0B, 0x00, 0xEE, 0xBA, 0xFB, 0x9A, 0xD0,
    0xE6, 0xA1, 0xB9, 0x71, 0x01, 0xAC, 0xEF, 0x1E, 0x27, 0x00, 0x28, 0x9D, 0x71, 0xC9, 0x22, 0x06,
    0x96, 0xB1, 0xBF, 0x36, 0x0B, 0x00, 0x70, 0xE0, 0xDC, 0x53, 0x60, 0x5B, 0xAA, 0x6C, 0x85, 0x16,
    0x39, 0xE4, 0xC8, 0x74, 0x90, 0x57, 0x33, 0x2F, 0x99, 0x13, 0xA8, 0x92, 0x69, 0x1F, 0x8F, 0xB9,
    0x03, 0xF1, 0xB9, 0xFD, 0x54, 0x17, 0x80, 0xA2, 0x8B, 0x81, 0xE3, 0x21, 0xC3, 0x40, 0x5D, 0xBF,
    0x9A, 0xE2, 0x1A, 0xDE, 0x44, 0x86, 0x73, 0x8E, 0xA1, 0x6F, 0x3D, 0xB8, 0x7C, 0xD8, 0xA9, 0x07,
    0x48, 0xF9, 0xB4, 0x00, 0xEC, 0xD7, 0x77, 0x2A, 0x00, 0x1A, 0x44, 0xFD, 0x00, 0x3B, 0xF9, 0xFF,
    0x4C, 0xDE, 0x02, 0x20, 0x02, 0xF0, 0xF2, 0x07, 0x57, 0xE0, 0x9C, 0x7F, 0x7A, 0xA2, 0x6C, 0xC1,
    0x66, 0xB1, 0x06, 0x04, 0xD9, 0xD5, 0x24, 0x81, 0x39, 0x65, 0x6B, 0x76, 0xF2, 0xCB, 0x4F, 0x85,
    0x35, 0xB7, 0x6B, 0x9B, 0x46, 0x52, 0x7D, 0x39, 0xBA, 0xBC, 0x87, 0x42, 0x7F, 0x13, 0xCE, 0x10,
    0x71, 0x19, 0x10, 0x2A, 0x06, 0xE2, 0x2D, 0x09, 0xAE, 0x72, 0x40, 0x27, 0xB9, 0x0C, 0x04, 0x6C,
    0x07, 0x80, 0x47, 0xB7, 0x3C, 0xEC, 0xC8, 0x58, 0x1E, 0x09, 0xA0, 0x74, 0xE6, 0x6A, 0x9E, 0x72,
    0x9A, 0x7B, 0xB9, 0xE9, 0x24, 0xED, 0x5B, 0x28, 0xCE, 0x65, 0x1D, 0xEF, 0xE9, 0xC2, 0xDB, 0x4B,
    0x4F, 0xC1, 0xFC, 0x5D, 0xFB, 0x52, 0xED, 0x39, 0x09, 0x0B, 0x08, 0xB2, 0x0B, 0x4B, 0xD2, 0x4A,
    0x7E, 0xC4, 0x04, 0x1D, 0x11, 0x3F, 0x3F, 0x1E, 0x07, 0x60, 0x8F, 0x4C, 0x92, 0x84, 0x3F, 0x29,
    0xF7, 0x9F, 0xCD, 0xE6, 0xBE, 0xCC, 0xF1, 0xC4, 0xA2, 0xD4, 0x18, 0x45, 0xC5, 0xEB, 0xA1, 0xE8,
    0x67, 0x2C, 0x72, 0xB6, 0xCE, 0xDB, 0x01, 0x00, 0x73, 0x22, 0xFE, 0x7F, 0x8E, 0x14, 0xA6, 0x59,
    0x9B, 0x7C, 0xC7, 0x71, 0x0B, 0xCC, 0x3B, 0x01, 0xBE, 0x04, 0x80, 0x06, 0xFB, 0x25, 0x8C, 0x83,
    0xEC, 0x79, 0xFA, 0x13, 0xAF, 0x62, 0xEE, 0x6F, 0xF6, 0xC5, 0xBA, 0x44, 0xB4, 0xBF, 0xA0, 0xA6,
    0x00, 0x20, 0x07, 0x57, 0x02, 0x44, 0x57, 0x04, 0x50, 0x62, 0xFE, 0x7F, 0xF0, 0x2F, 0x05, 0xD7,
    0xFC, 0xB3, 0xBF, 0xF6, 0x3F, 0xFA, 0x37, 0xB6, 0x26, 0xC0, 0x1C, 0xEE, 0xFE, 0x81, 0x84, 0xF8,
    0xD6, 0xE0, 0x75, 0x94, 0x04, 0x71, 0x17, 0xD0, 0xED, 0x06, 0xB0, 0xF2, 0xB6, 0x07, 0x97, 0x0F,
    0x07, 0xC3, 0x9C, 0xB9, 0x8A, 0x01, 0x58, 0x77, 0x4C, 0xB8, 0xD7, 0xD9, 0x07, 0x60, 0x89, 0x5F,
    0x9B, 0x85, 0x12, 0x6F, 0xD9, 0x1B, 0x2B, 0x17, 0xE2, 0x9D, 0x25, 0xA7, 0x94, 0x8D, 0x05, 0x08,
    0x04, 0x59, 0xE3, 0x81, 0x4C, 0x61, 0x1F, 0xBE, 0x5C, 0x8E, 0x40, 0xD2, 0xDF, 0xB0, 0xCF, 0x1F,
    0x8F, 0x01, 0x04, 0x3D, 0xFB, 0x78, 0x26, 0xA0, 0x29, 0x15, 0x96, 0x70, 0x20, 0xE5, 0xF5, 0xF4,
    0x23, 0x10, 0xDF, 0x20, 0x66, 0xF3, 0xDA, 0x63, 0xB7, 0x3D, 0xB8, 0x7C, 0x78, 0x00, 0x71, 0xFF,
    0x3F, 0x37, 0x04, 0x40, 0xBE, 0x8D, 0xD4, 0x0F, 0xA0, 0xD7, 0xF8, 0x29, 0x54, 0x8E, 0xB5, 0xDF,
    0x5C, 0xB9, 0x10, 0x47, 0x96, 0x9C, 0x5A, 0xFE, 0xC6, 0xCA, 0xF8, 0x16, 0x64, 0xA6, 0x81, 0xEA,
    0x3E, 0x99, 0x4C, 0x02, 0x70, 0x84, 0x36, 0xF1, 0x33, 0x1C, 0x21, 0x99, 0xEA, 0x52, 0x89, 0xAB,
    0x5D, 0xFC, 0x13, 0x20, 0x9D, 0xAD, 0x00, 0xB0, 0x6B, 0xBD, 0x4E, 0x94, 0x27, 0x95, 0xB3, 0xFE,
    0x5F, 0x67, 0xCC, 0x1F, 0x87, 0x00, 0x23, 0xE6, 0x50, 0x14, 0x6F, 0xAC, 0x5C, 0x88, 0x03, 0xEB,
    0x96, 0x94, 0x2D, 0x6C, 0x26, 0x45, 0x43, 0x04, 0xD9, 0x86, 0x60, 0x5A, 0x9E, 0x5E, 0xBC, 0xDC,
    0x27, 0x27, 0x88, 0x74, 0x48, 0xDF, 0x53, 0x5C, 0xFF, 0xA7, 0x15, 0x0F, 0x49, 0xDF, 0x16, 0x2C,
    0x7B, 0xCE, 0x5F, 0xAC, 0x14, 0x39, 0x81, 0xD9, 0x59, 0xC5, 0x6C, 0x33, 0x61, 0xA7, 0x73, 0xB6,
    0x1F, 0x97, 0x8D, 0x2F, 0xB4, 0xDD, 0xFF, 0xC7, 0xC0, 0x96, 0x52, 0xC7, 0x59, 0x97, 0x3E, 0x02,
    0xE6, 0x7E, 0x00, 0x36, 0x38, 0xB8, 0x9B, 0x29, 0x97, 0x6D, 0x30, 0x31, 0x70, 0xF2, 0x93, 0xAF,
    0x60, 0xDE, 0x73, 0xFB, 0x7C, 0x23, 0x87, 0xD2, 0x09, 0x9E, 0xEA, 0xD6, 0x03, 0x82, 0x69, 0x17,
    0x07, 0x30, 0x43, 0xD1, 0x8D, 0x01, 0xA8, 0x84, 0x18, 0x00, 0x65, 0x89, 0x01, 0x44, 0xFF, 0x72,
    0x52, 0x2C, 0x20, 0x1C, 0x07, 0x08, 0xD7, 0x01, 0xE4, 0x1A, 0x63, 0x00, 0xC1, 0xA4, 0x18, 0x72,
    0x03, 0xE8, 0xBB, 0x41, 0xB8, 0xF0, 0x8E, 0x07, 0x56, 0x8C, 0x24, 0xF9, 0xFF, 0xB9, 0xB0, 0x00,
    0x4A, 0xC6, 0xFF, 0xEF, 0x38, 0xEB, 0xD2, 0x45, 0x60, 0xA7, 0xFE, 0x5F, 0xBC, 0x5D, 0x69, 0xC5,
    0x11, 0x4C, 0x97, 0x11, 0x61, 0xDF, 0xCA, 0x3E, 0xBC, 0x70, 0xED, 0xA5, 0x38, 0x7C, 0xEE, 0x49,
    0x01, 0x9F, 0x2E, 0xF4, 0xB1, 0xB2, 0x82, 0xCE, 0xF1, 0xD3, 0xCA, 0x31, 0xDD, 0x8E, 0x2A, 0x88,
    0xBF, 0xDC, 0xB6, 0x61, 0x9C, 0xE1, 0x7B, 0xB1, 0xB5, 0x2B, 0x81, 0x36, 0x24, 0xAF, 0x35, 0x48,
    0xDF, 0x45, 0xB0, 0x86, 0xDC, 0x17, 0x36, 0xB1, 0x8D, 0xA1, 0x3B, 0x1E, 0x58, 0x31, 0x92, 0x34,
    0xFF, 0xEF, 0xC9, 0x5F, 0xDB, 0xD9, 0x57, 0x79, 0x11, 0xBF, 0x7E, 0x10, 0xF7, 0x9A, 0x52, 0x66,
    0xAA, 0x16, 0x63, 0x65, 0xAA, 0x64, 0x61, 0xEF, 0xC5, 0xE7, 0x60, 0xEF, 0xC5, 0x67, 0xE3, 0x84,
    0xD7, 0x0F, 0xA1, 0x7B, 0xFF, 0x51, 0xCC, 0x7C, 0xF9, 0x00, 0x48, 0xB3, 0x1B, 0x6B, 0xF0, 0x7A,
    0x2F, 0x38, 0x8D, 0x23, 0xD3, 0x07, 0xEF, 0x36, 0x55, 0x9F, 0x22, 0x55, 0x91, 0x2C, 0xC0, 0xC4,
    0x8D, 0x42, 0x12, 0x04, 0x31, 0x98, 0x0E, 0x1C, 0x7D, 0x87, 0xC1, 0x20, 0x0A, 0x4C, 0x1F, 0x52,
    0xE4, 0x9C, 0xD1, 0xB1, 0x58, 0x45, 0x73, 0x63, 0x2D, 0x71, 0x07, 0x38, 0x79, 0xAD, 0xDD, 0x0E,
    0x00, 0x3B, 0x1E, 0xDE, 0x1A, 0x9B, 0xFF, 0xCF, 0x0D, 0x01, 0x04, 0xFD, 0x7F, 0xF3, 0x57, 0xD7,
    0x6F, 0x99, 0x10, 0x0E, 0x9D, 0x71, 0x02, 0x70, 0xC6, 0x09, 0xA0, 0x4B, 0xFA, 0xA0, 0xB4, 0x46,
    0xC7, 0xD8, 0x24, 0x4A, 0xE3, 0x53, 0x20, 0xAD, 0x41, 0xDA, 0x49, 0xB3, 0x74, 0x0E, 0x00, 0xCC,
    0x5E, 0xD2, 0x87, 0x70, 0xC1, 0xBB, 0x8D, 0x14, 0xC8, 0x73, 0x44, 0x41, 0x8E, 0x74, 0x3A, 0x93,
    0xCF, 0xE4, 0x4D, 0x46, 0x51, 0x80, 0x1F, 0x08, 0x04, 0x13, 0xA0, 0x07, 0xB1, 0x3B, 0x8D, 0x17,
    0x51, 0xF4, 0x9C, 0xA4, 0xFC, 0x29, 0x66, 0x0C, 0x10, 0x53, 0xCC, 0xD4, 0xA7, 0xBA, 0x44, 0x08,
    0xAE, 0xEE, 0xB7, 0x00, 0xD8, 0xC4, 0xBC, 0x03, 0x0F, 0x00, 0x6B, 0xDE, 0xF7, 0x5E, 0xFD, 0xF3,
    0x07, 0x2B, 0x73, 0x62, 0xDB, 0xD0, 0xF1, 0xBD, 0x31, 0x8B, 0x98, 0x77, 0x82, 0xDD, 0xF9, 0x7F,
    0xA8, 0x72, 0xE5, 0xBF, 0x6B, 0x66, 0xFD, 0xFA, 0x3E, 0x24, 0x98, 0xE6, 0x61, 0xC0, 0x6A, 0x46,
    0x04, 0xD7, 0xF8, 0x5E, 0xE3, 0x07, 0x2F, 0x47, 0x08, 0xC4, 0x49, 0xA0, 0x23, 0xC6, 0xEE, 0x73,
    0x5F, 0x1F, 0x5E, 0xF9, 0xD4, 0xE0, 0x29, 0xC3, 0x60, 0xA6, 0xC0, 0xB2, 0x9A, 0xFC, 0x58, 0x00,
    0xA5, 0xEF, 0x8D, 0xAB, 0xA9, 0x3F, 0xEC, 0xD2, 0x04, 0x2C, 0x02, 0x3C, 0xFF, 0x9F, 0x1A, 0xD2,
    0x85, 0x54, 0xFB, 0xCD, 0x17, 0x08, 0xA2, 0xE6, 0x7F, 0x51, 0x3C, 0x9B, 0x40, 0x58, 0x61, 0xE8,
    0xA9, 0xC1, 0x53, 0x86, 0x4B, 0x7F, 0x39, 0xA1, 0xA6, 0x88, 0x74, 0xDA, 0x77, 0xDB, 0x1A, 0x04,
    0xF4, 0xE6, 0xFF, 0xC1, 0xEF, 0x01, 0xD0, 0x03, 0x32, 0xF3, 0xFF, 0x94, 0x1C, 0xBC, 0xAB, 0xD8,
    0x0B, 0x54, 0xE5, 0x77, 0x04, 0x82, 0x0A, 0x24, 0xC0, 0x4D, 0xFC, 0x7C, 0xF3, 0xE8, 0x80, 0xC0,
    0x44, 0xDB, 0x01, 0xC0, 0x9A, 0x35, 0x55, 0x56, 0x1A, 0x4A, 0x39, 0xE9, 0xE8, 0x75, 0xE4, 0x44,
    0x41, 0x62, 0xFE, 0x7F, 0xA3, 0x83, 0x74, 0xC2, 0x0D, 0x82, 0xE6, 0x3A, 0x0F, 0xAD, 0x1E, 0xA9,
    0xA1, 0xF0, 0x21, 0xC3, 0xD9, 0x3E, 0xCF, 0x86, 0xC9, 0xFF, 0xB7, 0x8F, 0x95, 0x38, 0xD7, 0xF2,
    0xD0, 0x71, 0xE7, 0x98, 0x05, 0xF0, 0x0E, 0x30, 0xF7, 0x13, 0x43, 0x33, 0xA0, 0x42, 0xB5, 0xFF,
    0x5B, 0x4A, 0xA9, 0x42, 0x0F, 0x22, 0xC2, 0x95, 0xC6, 0x04, 0xE7, 0x6A, 0x1C, 0x46, 0xB6, 0x34,
    0xF1, 0xF2, 0xFF, 0x69, 0x92, 0x57, 0x8E, 0x7D, 0x72, 0x66, 0x59, 0xFF, 0xBF, 0xAD, 0x16, 0x40,
    0xE9, 0x8E, 0x09, 0x35, 0xF5, 0xEF, 0x3A, 0x35, 0x80, 0x3E, 0x77, 0xFE, 0x9F, 0xFD, 0x92, 0x28,
    0x22, 0x93, 0x82, 0x9C, 0x12, 0x7F, 0xBE, 0x06, 0x24, 0x9B, 0x99, 0x09, 0x33, 0x8D, 0xC5, 0xEC,
    0x2C, 0x61, 0x1C, 0x1A, 0xFF, 0x64, 0xEF, 0x70, 0xC7, 0xB7, 0xC6, 0xD4, 0x64, 0x19, 0xFF, 0xBF,
    0xBD, 0x2E, 0x80, 0xE5, 0x4D, 0xB8, 0x9A, 0xFC, 0x7F, 0xD2, 0xEE, 0x04, 0x4C, 0x61, 0xA2, 0x2F,
    0x02, 0x31, 0xEB, 0xDB, 0x6D, 0x63, 0x52, 0xE4, 0x89, 0xF3, 0x7C, 0x1B, 0x00, 0xA0, 0xAB, 0xF2,
    0xEE, 0xDF, 0xA5, 0xF6, 0xF7, 0x30, 0xAF, 0xF3, 0xB9, 0x2C, 0xF2, 0x96, 0x68, 0x7F, 0x81, 0x10,
    0x4B, 0x5C, 0xE6, 0x13, 0x4B, 0x9A, 0x85, 0xFC, 0xFF, 0x9D, 0x59, 0x05, 0xA8, 0xAD, 0x22, 0x56,
    0xBA, 0x7D, 0xBC, 0x44, 0x25, 0xFD, 0x08, 0xDC, 0xFD, 0xFF, 0x4C, 0x00, 0x50, 0xB6, 0xFF, 0x13,
    0x08, 0xAA, 0x13, 0x5A, 0x72, 0x13, 0xE8, 0x18, 0xBB, 0x41, 0x74, 0xE1, 0xC4, 0x2D, 0xDD, 0xA9,
    0xF9, 0xFF, 0x41, 0xB4, 0x65, 0x1A, 0xD0, 0xFA, 0xCE, 0x84, 0x93, 0x6B, 0x65, 0xF1, 0x22, 0x30,
    0x96, 0x89, 0xBE, 0x17, 0x08, 0xEA, 0xB6, 0x2C, 0xDC, 0x8C, 0xA0, 0xA1, 0x89, 0x5B, 0xBA, 0x47,
    0x4A, 0xDF, 0x19, 0x53, 0x95, 0x84, 0xBF, 0x6D, 0x2E, 0x80, 0xBD, 0x77, 0x87, 0xC9, 0x4D, 0xE6,
    0x7E, 0x00, 0x3D, 0xCE, 0x63, 0x52, 0xDE, 0x35, 0x50, 0xAE, 0x0C, 0x15, 0x81, 0x20, 0x7F, 0x4E,
    0x44, 0xBA, 0x68, 0x6F, 0x07, 0x80, 0xA9, 0xBD, 0x8F, 0xA6, 0xE6, 0xFF, 0xB7, 0xDD, 0x02, 0xE8,
    0x58, 0x74, 0xA9, 0xCB, 0x56, 0xEB, 0x8C, 0x6C, 0xEB, 0x64, 0x43, 0x47, 0xB2, 0x7A, 0x04, 0x82,
    0x44, 0xB9, 0x30, 0xEB, 0x16, 0xBC, 0x87, 0x14, 0x9E, 0xFF, 0x57, 0xF3, 0xD6, 0xE8, 0xAC, 0x67,
    0x6B, 0x35, 0x83, 0x11, 0x40, 0xDC, 0x71, 0xE7, 0x98, 0x45, 0xE0, 0x9D, 0x00, 0xBC, 0xFC, 0x7F,
    0x71, 0xFD, 0x05, 0x82, 0x6A, 0x63, 0x01, 0xEC, 0x2A, 0x50, 0x67, 0xFD, 0x3F, 0x63, 0xE5, 0xC4,
    0x2D, 0x3D, 0x15, 0xE7, 0xFF, 0xDB, 0x66, 0x01, 0x94, 0xEE, 0x9C, 0x74, 0x17, 0xF7, 0x9B, 0xFA,
    0x7F, 0xE6, 0x59, 0x80, 0xD4, 0x04, 0x02, 0x41, 0x45, 0x1B, 0x00, 0x01, 0x7F, 0xD9, 0x15, 0xF4,
    0xA1, 0x89, 0x5B, 0x7A, 0x86, 0x4B, 0xDF, 0x1C, 0x53, 0x59, 0x84, 0xBF, 0x2D, 0x31, 0x00, 0x52,
    0x1C, 0x58, 0xFF, 0x8F, 0xF8, 0xFA, 0x7F, 0x21, 0x01, 0x81, 0xA0, 0x16, 0x53, 0x00, 0x60, 0xC7,
    0xFF, 0x57, 0x3D, 0x3A, 0xB3, 0x08, 0xB5, 0x3E, 0x06, 0x10, 0x5F, 0xFF, 0x2F, 0x96, 0xBF, 0x40,
    0x50, 0xBB, 0x40, 0xB1, 0x91, 0x63, 0x1B, 0xE4, 0xF8, 0xFF, 0x13, 0x37, 0x77, 0x67, 0x96, 0xA9,
    0x96, 0x5B, 0x00, 0x93, 0x9F, 0xE8, 0xB2, 0x3B, 0xBE, 0x3F, 0x66, 0x81, 0x79, 0xB5, 0x67, 0xFC,
    0x33, 0xB7, 0x3F, 0x34, 0x21, 0x10, 0xE4, 0x5B, 0xD0, 0xCB, 0xBD, 0xA1, 0x00, 0xBC, 0x02, 0xE0,
    0x59, 0x23, 0x3E, 0xF9, 0x24, 0x00, 0xEB, 0xF6, 0x49, 0x65, 0xFF, 0xFB, 0x0E, 0x27, 0xFF, 0x1F,
    0x58, 0x6A, 0x66, 0x2F, 0xA9, 0x8A, 0x8B, 0x15, 0x08, 0x04, 0xC9, 0xC2, 0xF2, 0xD8, 0xC4, 0xCD,
    0x3D, 0xC3, 0x60, 0x56, 0xA8, 0x90, 0xFF, 0xDF, 0x36, 0x17, 0x40, 0x75, 0x38, 0xBE, 0x09, 0x39,
    0x95, 0x7F, 0x2B, 0xD6, 0xFF, 0x17, 0x08, 0x04, 0x99, 0xB1, 0x0D, 0x00, 0x3A, 0xBF, 0x7F, 0xBC,
    0x2A, 0x79, 0x6A, 0x57, 0x41, 0x10, 0xF1, 0xFF, 0x05, 0x82, 0xC6, 0x68, 0xFF, 0x50, 0xFE, 0xFF,
    0xC4, 0xC7, 0xBB, 0xAB, 0x92, 0xA9, 0x16, 0xBA, 0x00, 0x4C, 0x93, 0x9F, 0x20, 0xBB, 0xF4, 0xDD,
    0x89, 0x12, 0x60, 0xAF, 0x36, 0xCD, 0x17, 0xED, 0x2F, 0x10, 0xD4, 0x47, 0x00, 0x04, 0x60, 0x0F,
    0x80, 0x67, 0x6A, 0x51, 0xAA, 0x2D, 0xB3, 0x00, 0xAC, 0x6F, 0x3B, 0xA5, 0x89, 0x54, 0xA7, 0x5E,
    0x08, 0x98, 0xFC, 0x7F, 0x12, 0x02, 0x10, 0x08, 0x1A, 0xE0, 0xFF, 0x0F, 0x4D, 0xDC, 0xDC, 0x33,
    0x52, 0xFA, 0xD6, 0x78, 0xE6, 0xF9, 0xFF, 0x96, 0x13, 0x80, 0xBD, 0xEF, 0x11, 0xF7, 0xB7, 0x9C,
    0xFA, 0x7F, 0x4E, 0xF6, 0x9F, 0x10, 0x80, 0x40, 0x50, 0x3F, 0x9C, 0xFC, 0xFF, 0xFD, 0x3B, 0xAB,
    0x96, 0xE7, 0x96, 0x11, 0x40, 0xC7, 0xD9, 0xAB, 0x5D, 0x66, 0xBA, 0xDC, 0xFC, 0xD5, 0x72, 0xDF,
    0x04, 0x82, 0x1A, 0x41, 0xF1, 0xFA, 0x7F, 0x6A, 0xEE, 0x5A, 0x5D, 0xFD, 0x69, 0x5A, 0xE4, 0xFF,
    0x03, 0xC4, 0x9D, 0xDF, 0x1F, 0xB3, 0x00, 0xDE, 0x09, 0x76, 0xF3, 0xFF, 0x59, 0xC9, 0x9D, 0x14,
    0x08, 0x6A, 0x92, 0x5A, 0x7F, 0xFD, 0x3F, 0x68, 0xE5, 0xC4, 0xCD, 0xDD, 0x99, 0xF3, 0xFF, 0x5B,
    0x6E, 0x01, 0x74, 0xB8, 0xF9, 0xFF, 0x4E, 0xFD, 0xBF, 0x25, 0x3E, 0x29, 0x08, 0x04, 0x82, 0x1A,
    0x55, 0xB6, 0x53, 0x3D, 0x93, 0x30, 0x34, 0x71, 0x73, 0xF7, 0x70, 0xE9, 0x9B, 0xD5, 0xFB, 0xFF,
    0x40, 0xAB, 0x66, 0x01, 0x94, 0xB7, 0x5D, 0x67, 0x72, 0xFE, 0xFF, 0xB4, 0xBD, 0x47, 0x82, 0x76,
    0x81, 0xDF, 0x3D, 0x5C, 0xB0, 0x0D, 0x00, 0xAC, 0x5E, 0x9B, 0xA6, 0x6A, 0x38, 0x47, 0xA9, 0xC5,
    0xB7, 0xC3, 0xF8, 0xFF, 0xEC, 0xEF, 0x67, 0xC4, 0x22, 0xF8, 0x82, 0xFA, 0xFB, 0x3B, 0x2D, 0x99,
    0x7C, 0x1A, 0x13, 0x41, 0x68, 0xFE, 0x7F, 0xBC, 0xE7, 0x27, 0xF9, 0x1E, 0xB3, 0x9D, 0x7F, 0x37,
    0x66, 0x81, 0x79, 0x07, 0x81, 0xFB, 0x39, 0x68, 0x01, 0x08, 0x01, 0xB4, 0x51, 0x47, 0xD2, 0xB4,
    0xEB, 0x53, 0x9E, 0xCE, 0x16, 0x81, 0xDF, 0x19, 0xAE, 0xFC, 0xBC, 0x44, 0xC0, 0xAA, 0xF1, 0x4F,
    0xF4, 0x0C, 0x67, 0xA9, 0xFF, 0xD7, 0x96, 0x18, 0x40, 0xC7, 0xDF, 0x9A, 0xFA, 0x7F, 0xE0, 0x3E,
    0x32, 0xFB, 0xFF, 0x51, 0x70, 0xFE, 0x9F, 0xA6, 0xA7, 0xF0, 0x53, 0xEE, 0x0E, 0x8E, 0x1C, 0x69,
    0xAF, 0x35, 0xF3, 0x37, 0xB9, 0xEA, 0x7E, 0xAA, 0xD4, 0xE7, 0xD1, 0xCF, 0x51, 0x85, 0xD7, 0xA7,
    0xC9, 0x00, 0xF3, 0xF2, 0xFF, 0xC7, 0x3F, 0xD1, 0x33, 0x6C, 0x7D, 0x73, 0x42, 0xD5, 0x22, 0xFC,
    0x2D, 0x71, 0x01, 0x48, 0x05, 0xEA, 0xFF, 0x13, 0x7A, 0xC1, 0x4E, 0xFD, 0xFF, 0x90, 0xF5, 0x5F,
    0x40, 0x57, 0x80, 0x12, 0x74, 0xE8, 0x69, 0x2F, 0x1E, 0x40, 0xD7, 0xA1, 0x11, 0xC0, 0xD6, 0x20,
    0xCD, 0x8E, 0xA7, 0xC3, 0xE6, 0x7E, 0x71, 0x35, 0xBA, 0xA8, 0x9A, 0xA1, 0xDA, 0xE8, 0xF3, 0x35,
    0x6F, 0xF4, 0x92, 0xB7, 0xDD, 0xB6, 0xBF, 0x61, 0x36, 0x01, 0x50, 0xE6, 0x91, 0x02, 0x41, 0x31,
    0xC1, 0x32, 0x8F, 0x9D, 0xBF, 0x0A, 0x8A, 0xCC, 0x7B, 0xDE, 0xEB, 0xF0, 0x5F, 0x73, 0xB7, 0xE8,
    0x26, 0xB7, 0xCE, 0x0C, 0x81, 0x2D, 0xC2, 0xF1, 0x59, 0x5D, 0x78, 0xF8, 0x82, 0x19, 0xB0, 0xD5,
    0x34, 0xD0, 0xFE, 0x94, 0xFA, 0xC4, 0xF1, 0xFF, 0x67, 0xDA, 0x64, 0xD7, 0x78, 0xEA, 0x16, 0xA6,
    0x02, 0xD3, 0x3A, 0x5F, 0x1A, 0x8C, 0xB5, 0xC2, 0xC5, 0xB4, 0x9F, 0xA3, 0xFB, 0xBF, 0x9F, 0xB1,
    0x6B, 0x2F, 0x4E, 0x78, 0xE2, 0x75, 0x58, 0x53, 0x76, 0x73, 0x84, 0x8F, 0x72, 0x62, 0xCB, 0x52,
    0x9D, 0x9C, 0xC4, 0x1E, 0x15, 0x38, 0x82, 0xCF, 0xCA, 0x08, 0xBE, 0x82, 0x65, 0x0E, 0xE7, 0x31,
    0x79, 0x7F, 0x2D, 0x28, 0x58, 0x86, 0x14, 0x2C, 0x28, 0x94, 0xE0, 0x3E, 0xF6, 0xC9, 0xC0, 0x72,
    0x49, 0xC1, 0x23, 0x13, 0x80, 0x30, 0x8C, 0xC5, 0xCF, 0x75, 0xE0, 0xA5, 0xF3, 0x66, 0xE3, 0xA1,
    0x15, 0x33, 0x60, 0x53, 0x44, 0xD7, 0x50, 0x41, 0x59, 0x80, 0xE2, 0xF9, 0xFF, 0x53, 0x87, 0x3A,
    0xB8, 0xD9, 0xB7, 0xB4, 0x56, 0x3F, 0x93, 0x00, 0xE2, 0xAE, 0xBF, 0x1F, 0xF7, 0xF6, 0xFF, 0x83,
    0xA9, 0xFF, 0xE7, 0x8D, 0x93, 0x42, 0xD1, 0x73, 0xC0, 0x84, 0x35, 0xED, 0x5E, 0xB4, 0x79, 0x17,
    0xE6, 0xBF, 0x79, 0xB4, 0xF1, 0xFE, 0x75, 0x3D, 0x1B, 0xD4, 0x57, 0x7B, 0x4E, 0x6E, 0xD2, 0x08,
    0xE1, 0x34, 0xF2, 0xF4, 0x35, 0x3E, 0x31, 0x05, 0x04, 0x3F, 0x85, 0x00, 0x40, 0xB0, 0x58, 0x79,
    0x82, 0x1F, 0x3E, 0x82, 0x04, 0x40, 0x8E, 0x35, 0x10, 0xB0, 0x33, 0x00, 0xE0, 0x9D, 0x79, 0x1D,
    0xF8, 0xC9, 0x95, 0x0B, 0x70, 0xB0, 0xC7, 0x67, 0x01, 0x2E, 0x62, 0xF4, 0x86, 0x42, 0xFE, 0xFF,
    0x6E, 0x00, 0x17, 0x4E, 0x7C, 0xBC, 0x7B, 0xA4, 0x96, 0xF9, 0xFF, 0x96, 0xC4, 0x00, 0x3A, 0xEE,
    0x9C, 0x0A, 0xCE, 0xFF, 0x2F, 0x35, 0x57, 0x40, 0x08, 0xB0, 0x71, 0xA1, 0x34, 0x7F, 0xA0, 0xB1,
    0x8A, 0x19, 0x67, 0xFF, 0xFC, 0xE9, 0x04, 0xE1, 0x47, 0xF3, 0x85, 0xBF, 0x96, 0x9F, 0xA0, 0x0C,
    0xCE, 0x74, 0xA3, 0xD5, 0x43, 0xCD, 0xE7, 0xA4, 0xBA, 0x1B, 0x10, 0x3C, 0xC3, 0xBC, 0xC3, 0x93,
    0xF8, 0xD8, 0x3D, 0x7B, 0x71, 0xD2, 0x28, 0x23, 0x34, 0xF6, 0x8A, 0x10, 0x19, 0x88, 0x07, 0x34,
    0x5C, 0xFB, 0x73, 0x68, 0xE2, 0xE3, 0xDD, 0x23, 0xEA, 0xAF, 0xA6, 0x54, 0xAD, 0xC2, 0xDF, 0x74,
    0x02, 0x98, 0x7C, 0xF9, 0x97, 0x2A, 0xE4, 0xFF, 0x3B, 0xF5, 0xFF, 0xE3, 0x55, 0xFF, 0x28, 0xFF,
    0x47, 0x34, 0x6A, 0xB9, 0xF0, 0x81, 0x67, 0x31, 0xF7, 0xE0, 0x48, 0xE5, 0x9B, 0x56, 0xCB, 0x51,
    0xCB, 0xE0, 0x68, 0xC4, 0x39, 0x6B, 0x6D, 0x07, 0xAA, 0x21, 0x17, 0xBF, 0x33, 0x39, 0x51, 0x1B,
    0xD7, 0xC2, 0x5E, 0x48, 0x08, 0xF9, 0x45, 0x06, 0xBA, 0x06, 0xD6, 0xFF, 0xD3, 0x7E, 0x74, 0x4F,
    0xBA, 0x6D, 0x20, 0x27, 0x26, 0x91, 0xF7, 0x23, 0x9D, 0x0D, 0xB6, 0x03, 0x80, 0x3E, 0xFC, 0xCB,
    0xBA, 0x64, 0xB8, 0xA9, 0x04, 0xD0, 0xB9, 0x78, 0xAD, 0x7B, 0x6F, 0xD7, 0x99, 0x1B, 0xEF, 0xE7,
    0x2A, 0x53, 0x31, 0x38, 0x98, 0x82, 0x96, 0x8A, 0x69, 0xE8, 0xC9, 0x2F, 0x1C, 0xC0, 0xBC, 0xFD,
    0xC3, 0xD9, 0x35, 0xA9, 0x20, 0x2C, 0xF4, 0x75, 0x1B, 0x38, 0x14, 0x32, 0xF1, 0xB3, 0x62, 0xC6,
    0x98, 0xC6, 0x95, 0x8F, 0x1E, 0x8B, 0x6D, 0x3E, 0x4D, 0xB9, 0xEF, 0xB8, 0x80, 0x16, 0x72, 0x3C,
    0x1F, 0x0D, 0xE0, 0x11, 0x00, 0xB0, 0xE6, 0x5F, 0x56, 0xD7, 0x9A, 0x9A, 0xA6, 0x06, 0x01, 0x27,
    0x6E, 0xEA, 0xB4, 0x1D, 0xFF, 0x1F, 0xAB, 0xCD, 0xAD, 0x0F, 0x13, 0x0E, 0xE5, 0x74, 0x36, 0xB0,
    0x5C, 0xC4, 0x9E, 0x80, 0xF9, 0x4F, 0xBD, 0xD1, 0xF2, 0x68, 0xCA, 0xBB, 0x01, 0xC1, 0xDE, 0x4E,
    0xDF, 0x1F, 0x92, 0x2A, 0x86, 0x2E, 0xA8, 0xCC, 0x2F, 0x2C, 0x7E, 0xF1, 0x18, 0x66, 0xAC, 0x99,
    0x8D, 0xB1, 0x52, 0xD8, 0x1A, 0x29, 0xC8, 0xED, 0x73, 0xFD, 0xFF, 0x57, 0x40, 0xD8, 0x05, 0x00,
    0xF6, 0x7F, 0xB1, 0xEA, 0x8A, 0xA2, 0x35, 0xCD, 0x02, 0xB0, 0xBE, 0x3D, 0xA5, 0x3C, 0xFF, 0xDF,
    0xCC, 0xFF, 0x17, 0xA6, 0x9F, 0x3D, 0xD2, 0xA5, 0x18, 0x49, 0x9D, 0xF8, 0xF2, 0x5B, 0xE8, 0x1E,
    0x99, 0xC8, 0x2C, 0xFC, 0xCC, 0x72, 0x70, 0x70, 0x46, 0x14, 0xFE, 0xE3, 0xE0, 0xEB, 0xCC, 0x8E,
    0x44, 0x3B, 0xAF, 0x71, 0x6A, 0x3F, 0xBA, 0x0F, 0xD8, 0xFB, 0x62, 0x02, 0x79, 0x94, 0xA1, 0x17,
    0x62, 0xC6, 0xE5, 0x4F, 0x8D, 0x38, 0xD3, 0x86, 0x54, 0x54, 0x8E, 0xA4, 0xC7, 0xC6, 0x6F, 0x9A,
    0xE1, 0xD4, 0xFF, 0x03, 0xE5, 0x94, 0x00, 0x66, 0xD9, 0xFE, 0xFC, 0x7F, 0xB0, 0xFE, 0x5F, 0x21,
    0x35, 0xA5, 0xDF, 0xEC, 0xAE, 0x03, 0x47, 0xB3, 0xDD, 0x26, 0x96, 0x5D, 0x8E, 0xD3, 0x08, 0x31,
    0xD3, 0x18, 0xE7, 0x72, 0xFD, 0x48, 0xB1, 0xAF, 0x84, 0xC5, 0x80, 0xC1, 0x65, 0x28, 0xE1, 0xE4,
    0x7D, 0xC7, 0x0B, 0xEC, 0xB1, 0x11, 0x60, 0xE6, 0xFF, 0xBB, 0xFE, 0x7E, 0xA2, 0xEE, 0xE6, 0x37,
    0xCD, 0x05, 0x98, 0xE8, 0xBC, 0x1F, 0xBE, 0xFF, 0x5F, 0xBC, 0x09, 0xBF, 0xB8, 0xA7, 0x42, 0x00,
    0x33, 0x66, 0xBF, 0x71, 0x24, 0x3E, 0xC9, 0xE7, 0xBE, 0xC0, 0xC1, 0x87, 0x0C, 0xDB, 0xD6, 0x20,
    0x22, 0x68, 0x76, 0x07, 0xE5, 0xBB, 0xCC, 0x4F, 0xE0, 0x24, 0xCF, 0x9D, 0xCD, 0x23, 0x86, 0x82,
    0x32, 0x32, 0xAE, 0xBD, 0xCF, 0x3B, 0x33, 0xC4, 0x3A, 0xF0, 0x7D, 0x32, 0xFF, 0x29, 0x10, 0x3B,
    0x93, 0xDF, 0xA1, 0x5A, 0xB2, 0x14, 0xD4, 0x66, 0xD1, 0xD7, 0x38, 0x71, 0xD0, 0x29, 0x1D, 0x9F,
    0x04, 0xE4, 0x22, 0x88, 0x3D, 0xC8, 0xA9, 0xFF, 0x6F, 0xD6, 0xFF, 0x8F, 0xFF, 0xDB, 0x4E, 0xC6,
    0x4D, 0xB9, 0x24, 0x00, 0x26, 0x6C, 0x24, 0xBB, 0xF4, 0x9D, 0x49, 0xBF, 0xFE, 0x5F, 0xC1, 0xC8,
    0x96, 0x52, 0x5E, 0xED, 0x1A, 0x1E, 0x2F, 0x6F, 0x9C, 0x99, 0x61, 0x77, 0xD1, 0x72, 0xE0, 0x92,
    0x0B, 0x4F, 0x14, 0x95, 0x9F, 0x34, 0x8C, 0x03, 0x74, 0xE0, 0x3F, 0x0A, 0xFE, 0x0B, 0x87, 0xF9,
    0xC8, 0xCB, 0x1B, 0x08, 0x7C, 0x36, 0x98, 0x54, 0x64, 0xFA, 0x7E, 0x74, 0x5F, 0x09, 0x4F, 0x3C,
    0x70, 0x1C, 0xC1, 0x5D, 0xF3, 0xA8, 0x4C, 0x4B, 0x38, 0xC7, 0x24, 0x10, 0x6F, 0x37, 0xB9, 0xFE,
    0x7F, 0xCD, 0xF5, 0xFF, 0x5A, 0x46, 0x00, 0xD6, 0xD7, 0x35, 0xD9, 0xFF, 0x19, 0x6C, 0xCD, 0xD2,
    0x8B, 0xC0, 0x58, 0x1A, 0xE2, 0x65, 0x2E, 0xEE, 0xD0, 0x5D, 0xF0, 0xDA, 0xE1, 0x98, 0x72, 0xA3,
    0x18, 0x0F, 0x38, 0x66, 0xC0, 0x4D, 0x1F, 0x3B, 0x13, 0x6B, 0x2E, 0x39, 0x5B, 0x64, 0xBE, 0x69,
    0x66, 0x05, 0x9B, 0xFF, 0x7C, 0x97, 0xE1, 0xFE, 0xBF, 0x79, 0xDD, 0x08, 0xBD, 0x7F, 0x77, 0x2A,
    0xE5, 0x38, 0xE5, 0x3F, 0x19, 0x35, 0x9A, 0x7A, 0x86, 0xA1, 0xB1, 0x9B, 0xBA, 0x46, 0x3A, 0xBE,
    0x31, 0xA5, 0x26, 0xAB, 0xA8, 0xFF, 0xDF, 0x52, 0x02, 0x28, 0xCD, 0xF7, 0x72, 0x93, 0xFB, 0x89,
    0xC8, 0xAC, 0xFF, 0x4F, 0xAA, 0xFE, 0x53, 0x84, 0xD5, 0x68, 0xFE, 0x10, 0xE9, 0xD9, 0xF3, 0x56,
    0xC4, 0x90, 0xA7, 0x90, 0x96, 0xF1, 0x12, 0x9D, 0x09, 0x58, 0x7D, 0x71, 0x9F, 0xC8, 0x6A, 0x8B,
    0xB1, 0xEB, 0xE1, 0x71, 0xCF, 0x4E, 0xE0, 0xD8, 0xDD, 0x49, 0x8F, 0xED, 0x14, 0x45, 0x27, 0x99,
    0x2B, 0xD9, 0x0E, 0x00, 0xA5, 0x39, 0x36, 0x4D, 0x36, 0x42, 0x56, 0x9B, 0xDB, 0xE0, 0x48, 0xFE,
    0x7F, 0x21, 0x5D, 0x00, 0x5F, 0xBC, 0xE7, 0xBC, 0x7C, 0x30, 0x62, 0x5A, 0x72, 0x84, 0x22, 0x9C,
    0x47, 0xAB, 0x96, 0x53, 0x05, 0xE3, 0x53, 0x50, 0x73, 0x20, 0x21, 0xA6, 0xBF, 0x9D, 0xBF, 0xE3,
    0xA3, 0x53, 0x38, 0xFC, 0xBA, 0x8A, 0xBC, 0x9A, 0xFC, 0xFD, 0x23, 0x73, 0x3A, 0x62, 0x77, 0x98,
    0xF3, 0x3D, 0x16, 0x63, 0xF5, 0xFF, 0x26, 0x0E, 0x94, 0x1A, 0xD2, 0xE4, 0x26, 0xCC, 0x02, 0x30,
    0x8D, 0x7F, 0xAC, 0xD3, 0x9E, 0xF1, 0x0F, 0x13, 0x66, 0xFE, 0x3F, 0x7C, 0xCB, 0x8A, 0x72, 0xC4,
    0x6E, 0x02, 0x87, 0x47, 0x09, 0xA7, 0x9A, 0xA4, 0xC0, 0x8A, 0xE5, 0x73, 0x1C, 0x67, 0x80, 0x45,
    0xF8, 0x1B, 0xEF, 0x0D, 0x27, 0xF7, 0xE1, 0xAF, 0xB7, 0xBF, 0x0D, 0xCD, 0xEC, 0x09, 0x7D, 0xE4,
    0x76, 0x85, 0x9E, 0xBF, 0x74, 0x76, 0x4F, 0xE2, 0xAF, 0xE4, 0x78, 0x2C, 0xBA, 0x4D, 0xDF, 0x03,
    0xB3, 0xFF, 0x9F, 0xFD, 0x27, 0x2A, 0x9F, 0x04, 0xD0, 0xF9, 0x7D, 0x6F, 0xDD, 0xD5, 0x22, 0x00,
    0xCB, 0x8C, 0x99, 0xA5, 0xA8, 0xC0, 0xC3, 0x8F, 0x00, 0xCC, 0x7D, 0xFD, 0x10, 0xA2, 0xD3, 0x4B,
    0xFE, 0x4C, 0x15, 0x07, 0xE6, 0xB3, 0x19, 0x17, 0x2C, 0x9F, 0x5F, 0x59, 0x81, 0x09, 0xEA, 0x0A,
    0x89, 0x39, 0x5D, 0xEB, 0xA7, 0x01, 0x3F, 0xF7, 0xE4, 0x41, 0x4F, 0x55, 0x72, 0x98, 0x96, 0x63,
    0x44, 0xF0, 0xDC, 0xE9, 0x33, 0x8A, 0x46, 0xC5, 0x9E, 0xFF, 0x7F, 0xFC, 0xA6, 0xAE, 0xE1, 0xD2,
    0xD7, 0xEA, 0xCB, 0xFF, 0x6F, 0x2A, 0x01, 0x50, 0xC9, 0xEB, 0xDB, 0xF7, 0x00, 0xE8, 0x21, 0x40,
    0x7B, 0x8B, 0xB5, 0x0B, 0xA6, 0x04, 0x83, 0x4D, 0xED, 0xDA, 0x77, 0xC4, 0x57, 0xF4, 0xCC, 0x5E,
    0xE2, 0x4A, 0x74, 0x84, 0x11, 0x80, 0xB5, 0x17, 0x9F, 0xE6, 0x79, 0x3E, 0xC1, 0x24, 0x17, 0x61,
    0x83, 0xC6, 0xB9, 0x01, 0x51, 0xEB, 0xEA, 0xA5, 0xA7, 0x8E, 0xF9, 0xB7, 0x23, 0x9E, 0x4A, 0xE0,
    0x1D, 0x6F, 0x9F, 0xD0, 0x59, 0x1C, 0x5E, 0x8E, 0x9B, 0xA3, 0x4E, 0xFD, 0xFF, 0xB7, 0x7F, 0xD9,
    0x30, 0xB9, 0x6D, 0x7C, 0x0C, 0x80, 0xA3, 0xF5, 0xFF, 0x12, 0x0A, 0x80, 0x16, 0x65, 0xE2, 0x35,
    0xE0, 0x4C, 0xCE, 0x7D, 0xE9, 0x2D, 0x30, 0x87, 0x13, 0x51, 0x63, 0xF3, 0xFA, 0xCC, 0x38, 0xEF,
    0x1C, 0x0D, 0xA5, 0x28, 0x10, 0xF9, 0x70, 0xA2, 0x82, 0xBE, 0xFC, 0x0B, 0x09, 0x34, 0x9A, 0x0C,
    0x8E, 0x1E, 0x3E, 0x8E, 0xA3, 0xAF, 0x9D, 0x68, 0xCC, 0x4C, 0x2E, 0x3B, 0xC0, 0xDE, 0x3C, 0xB3,
    0x37, 0xFF, 0x43, 0x30, 0xEE, 0xF5, 0x38, 0xFE, 0x3F, 0x3B, 0xF9, 0xFF, 0xB8, 0xF5, 0xBD, 0x1A,
    0x83, 0x39, 0x25, 0x80, 0xF1, 0x8F, 0x75, 0xD8, 0x33, 0xFE, 0xEF, 0xA4, 0x05, 0x60, 0x8D, 0x31,
    0x52, 0x54, 0x34, 0x98, 0x56, 0x04, 0x4B, 0x20, 0xB8, 0x54, 0x99, 0x00, 0x94, 0xC6, 0x26, 0x43,
    0xC3, 0x2F, 0x3C, 0xD3, 0x6C, 0x3C, 0x4F, 0x66, 0x5C, 0x7C, 0xF1, 0x89, 0x86, 0x03, 0x1D, 0x7A,
    0x60, 0xA2, 0x90, 0xBA, 0x0A, 0x1A, 0x43, 0x82, 0x6C, 0x02, 0x9E, 0xFE, 0x31, 0xE7, 0xB3, 0x4F,
    0x0C, 0xED, 0x4D, 0x34, 0xF5, 0x93, 0x7A, 0x7A, 0xEF, 0x29, 0x9D, 0x19, 0x57, 0x20, 0xE6, 0x06,
    0xDE, 0xFC, 0x3F, 0x83, 0x76, 0x35, 0xDA, 0xB1, 0x6C, 0x28, 0x01, 0x74, 0x7C, 0x7F, 0x4A, 0x4D,
    0x7E, 0xBC, 0xA4, 0x01, 0xF4, 0x01, 0x1C, 0xCE, 0xFF, 0xCF, 0x65, 0x29, 0x16, 0x2E, 0xFB, 0x8E,
    0xDB, 0xD2, 0x93, 0x76, 0xBF, 0x55, 0xE6, 0x7D, 0xDF, 0xDE, 0xD4, 0xCC, 0x78, 0xCF, 0xCA, 0xF9,
    0x7E, 0x2E, 0x7B, 0x44, 0xF8, 0x5D, 0x92, 0x10, 0x12, 0x48, 0x32, 0x1C, 0x23, 0xD9, 0x79, 0x55,
    0x74, 0xD1, 0xEE, 0x67, 0x47, 0xC1, 0xD4, 0xE5, 0xE5, 0x9A, 0xFB, 0xB5, 0x07, 0xE3, 0x27, 0x79,
    0xFE, 0xF4, 0x19, 0x29, 0x77, 0x9D, 0xF2, 0x3A, 0x16, 0x03, 0xF3, 0xFF, 0x9D, 0xC3, 0xA5, 0xAF,
    0x4F, 0xA9, 0xA9, 0x06, 0xCC, 0xFF, 0x37, 0x85, 0x00, 0x54, 0x29, 0x50, 0xFF, 0x1F, 0xE8, 0x05,
    0xF9, 0xD5, 0x7F, 0xF2, 0x69, 0x77, 0x65, 0x2B, 0x1E, 0xDD, 0xB9, 0xFF, 0x48, 0xA6, 0x5B, 0xA6,
    0x94, 0xC2, 0xA5, 0x97, 0x9C, 0xEC, 0x0D, 0xE6, 0x34, 0x61, 0x67, 0x59, 0x24, 0x50, 0x85, 0x27,
    0x59, 0xDE, 0x3A, 0x03, 0x80, 0x17, 0x7F, 0x6D, 0x47, 0x42, 0xB3, 0x46, 0xC5, 0x47, 0xE2, 0x64,
    0xDA, 0x22, 0x67, 0x3B, 0x9A, 0x5C, 0xCE, 0xCE, 0x50, 0x3C, 0x98, 0x14, 0x1E, 0x6C, 0xDB, 0x00,
    0xA0, 0x34, 0x57, 0xD7, 0x54, 0xFF, 0xBF, 0x75, 0x31, 0x00, 0x07, 0xEB, 0x52, 0x97, 0x7F, 0xE7,
    0x52, 0xF9, 0x95, 0x67, 0xA6, 0x59, 0xCF, 0xEF, 0xCD, 0x50, 0xE4, 0x8B, 0x71, 0xE1, 0x72, 0x3B,
    0x26, 0xDC, 0x22, 0xEC, 0x0D, 0xBE, 0x53, 0x81, 0x78, 0x0A, 0x11, 0x30, 0x3E, 0xA6, 0x71, 0x78,
    0x4F, 0xB7, 0xB1, 0x18, 0x22, 0xFE, 0x3F, 0x87, 0x57, 0xFC, 0x3D, 0xBB, 0x72, 0x8E, 0x71, 0xCB,
    0xF2, 0x3A, 0x20, 0x63, 0xE3, 0x90, 0x8D, 0xF9, 0x6F, 0x83, 0x9C, 0xFA, 0x7F, 0x13, 0xFB, 0xAC,
    0x86, 0x0E, 0xA8, 0x06, 0x12, 0x00, 0xD3, 0xF8, 0x1F, 0x40, 0xCF, 0xF8, 0xC1, 0x84, 0x05, 0x86,
    0x9F, 0xFF, 0x5F, 0x98, 0x54, 0xAB, 0x32, 0x0D, 0xE5, 0xF2, 0x7E, 0xA5, 0x8B, 0x8B, 0x57, 0x9D,
    0x26, 0x02, 0xDF, 0x02, 0x57, 0x81, 0x02, 0x26, 0xC0, 0xD0, 0xD6, 0x37, 0x02, 0x66, 0x66, 0xB4,
    0x56, 0xB3, 0xBF, 0x3A, 0x8B, 0x00, 0x1C, 0x38, 0xB1, 0xAB, 0x88, 0x01, 0x91, 0x50, 0xFE, 0xBF,
    0xFE, 0x6F, 0x8A, 0xF1, 0xDF, 0x73, 0x48, 0x00, 0x1D, 0x77, 0xDA, 0x34, 0x79, 0x73, 0x49, 0x13,
    0x26, 0x8A, 0xB7, 0xFE, 0xBF, 0x8C, 0x73, 0x90, 0xE4, 0xFF, 0x27, 0xDD, 0x26, 0x06, 0xB0, 0x6C,
    0xC9, 0x3C, 0x7F, 0x9D, 0x3A, 0x89, 0x05, 0xD0, 0x34, 0x0B, 0xC0, 0x98, 0x00, 0x8A, 0x08, 0xBB,
    0x9F, 0x9B, 0x80, 0xA6, 0xE4, 0xA0, 0x6C, 0x34, 0x8D, 0xE6, 0xE5, 0xD3, 0xBB, 0x8B, 0x13, 0xFC,
    0x0B, 0xAF, 0x9C, 0x19, 0x3A, 0xFE, 0x07, 0x5D, 0x23, 0xEA, 0xAB, 0xB6, 0xD2, 0x0D, 0xF4, 0xFF,
    0x1B, 0x4A, 0x00, 0x56, 0x97, 0x76, 0x73, 0x93, 0x0B, 0xBF, 0xFF, 0x5F, 0x90, 0xB5, 0x3A, 0x5F,
    0x7B, 0x3B, 0x83, 0xFC, 0x3B, 0xF7, 0xE9, 0x3D, 0x17, 0xCE, 0x05, 0x6B, 0xF3, 0x8C, 0xD3, 0x6B,
    0x63, 0x27, 0xBD, 0x95, 0x35, 0x30, 0xD8, 0x0C, 0x42, 0xC9, 0x1A, 0xA7, 0x68, 0x66, 0xF0, 0x32,
    0xED, 0xBA, 0x28, 0xEA, 0xF0, 0x33, 0x7B, 0x3B, 0xCB, 0x68, 0x00, 0xBF, 0xDE, 0x3A, 0x02, 0xA0,
    0xDB, 0xDC, 0x83, 0x04, 0x0B, 0xC0, 0x60, 0xFF, 0x19, 0xBD, 0x98, 0x4A, 0x4C, 0x47, 0xCB, 0xD3,
    0x0E, 0x49, 0x49, 0x33, 0x64, 0x0C, 0xB8, 0xF5, 0xFF, 0x8E, 0x6D, 0x0B, 0xAC, 0x95, 0xCE, 0x19,
    0x01, 0x50, 0xC8, 0xFF, 0x37, 0x13, 0xE0, 0x51, 0x22, 0xCE, 0xF5, 0x86, 0xC0, 0x9C, 0x18, 0x79,
    0x9E, 0xF5, 0xD2, 0x81, 0xB2, 0xC3, 0xC5, 0xBD, 0xC4, 0xF3, 0x17, 0xBB, 0xD9, 0x80, 0x3A, 0x32,
    0xB0, 0x91, 0xC9, 0x07, 0x6A, 0xA7, 0xA5, 0x90, 0xF5, 0xB7, 0xDB, 0xD1, 0xC6, 0x60, 0xFF, 0xB9,
    0xFE, 0x3F, 0x31, 0x83, 0x89, 0x30, 0x35, 0x61, 0x63, 0xE4, 0xED, 0x2E, 0x28, 0x72, 0x73, 0x32,
    0x52, 0x2C, 0x00, 0x00, 0x6F, 0x9D, 0xD6, 0xEB, 0x4C, 0xCA, 0x24, 0xCA, 0x5B, 0x6C, 0x4E, 0xA7,
    0x3D, 0xC2, 0x1F, 0xDB, 0xFD, 0x87, 0x43, 0xF9, 0xFF, 0xB8, 0xF5, 0x7D, 0x0D, 0x9B, 0xFF, 0x6F,
    0x30, 0x01, 0x30, 0x1D, 0xBF, 0x91, 0x6C, 0x30, 0x5B, 0x74, 0xD7, 0x64, 0xEA, 0xFA, 0xFF, 0x06,
    0x65, 0x2F, 0xB6, 0x0C, 0x6E, 0xE1, 0x88, 0x34, 0xA1, 0x0F, 0xEA, 0xFF, 0xF3, 0xCE, 0xEB, 0x06,
    0xC0, 0xD0, 0x5A, 0x63, 0xDA, 0xED, 0x7A, 0x9A, 0x2B, 0xFF, 0xDF, 0x17, 0xD7, 0x9D, 0xDB, 0xF6,
    0x42, 0x93, 0x63, 0x66, 0x26, 0xAF, 0xED, 0xF7, 0x5F, 0x3D, 0x3C, 0xBF, 0x33, 0xD9, 0xBA, 0xE0,
    0xB0, 0x00, 0xB6, 0x53, 0x45, 0x71, 0x4C, 0x23, 0x81, 0x10, 0xC8, 0xFF, 0x6F, 0xC6, 0xA0, 0x6A,
    0x08, 0x01, 0x74, 0x7C, 0x57, 0xD3, 0xE4, 0x1F, 0x81, 0x7B, 0xEE, 0x9A, 0x5C, 0x04, 0x60, 0xD9,
    0xB4, 0xF0, 0xFF, 0x19, 0x98, 0xFF, 0xEA, 0xA1, 0x58, 0xA5, 0x9F, 0xB4, 0x81, 0xB9, 0xFA, 0xE2,
    0xD3, 0xA1, 0xB5, 0xA9, 0x55, 0x27, 0xC2, 0xDF, 0x12, 0xEC, 0x7A, 0x7C, 0x18, 0x40, 0x87, 0xE3,
    0x6B, 0x92, 0x5F, 0x69, 0x88, 0x63, 0x37, 0x8B, 0xF0, 0xE2, 0x19, 0x5D, 0x29, 0x5A, 0x29, 0x7F,
    0xAE, 0x27, 0xFB, 0x66, 0x89, 0xE3, 0xFF, 0xDF, 0x68, 0xEA, 0xFF, 0x35, 0xD8, 0xFF, 0x6F, 0x1C,
    0x01, 0xCC, 0x32, 0xFE, 0x3F, 0x99, 0xFC, 0x7F, 0x86, 0x66, 0xB7, 0x42, 0x53, 0x01, 0xF7, 0xFC,
    0xF3, 0x3A, 0x67, 0xEF, 0xE1, 0x44, 0x86, 0xA6, 0x88, 0xEF, 0x4F, 0x44, 0xB8, 0x74, 0xE5, 0x1C,
    0xD8, 0xB6, 0x5D, 0x15, 0x51, 0xB7, 0x23, 0x21, 0xA8, 0x1E, 0x33, 0xBE, 0x3D, 0xED, 0x0D, 0x27,
    0x05, 0xB9, 0xCF, 0x99, 0x81, 0x67, 0x77, 0x74, 0x78, 0x37, 0xC4, 0x5D, 0x98, 0xCD, 0x5E, 0x09,
    0x31, 0xFF, 0x2E, 0x8C, 0x75, 0x2B, 0xD8, 0xAA, 0x78, 0x63, 0xD0, 0xB4, 0x7F, 0xBB, 0x63, 0xFE,
    0x6F, 0x69, 0xB8, 0xFF, 0xDF, 0x30, 0x02, 0x18, 0x55, 0xF7, 0xB9, 0x0F, 0x4D, 0xFE, 0x3F, 0x69,
    0x02, 0xAB, 0xA2, 0xE9, 0xC1, 0xE8, 0xF4, 0xF0, 0xDC, 0x5D, 0xFB, 0xBC, 0x80, 0x1D, 0xA5, 0x94,
    0xA3, 0x66, 0x00, 0xAB, 0x2E, 0xB4, 0x9C, 0x7C, 0x7F, 0xD6, 0x2D, 0x13, 0xC6, 0x3C, 0xC7, 0x0A,
    0x9A, 0x13, 0x07, 0x20, 0x9F, 0x08, 0x18, 0x18, 0x1D, 0x9D, 0xC0, 0xD8, 0x71, 0x43, 0x06, 0xA1,
    0xBA, 0x0C, 0xF1, 0x36, 0x1E, 0x38, 0xAB, 0xB7, 0x88, 0x03, 0x51, 0x11, 0xC8, 0x66, 0xB0, 0xC9,
    0xFF, 0x7F, 0x7F, 0xC3, 0xFD, 0xFF, 0x06, 0x11, 0x80, 0x53, 0xFF, 0xAF, 0xFB, 0xAE, 0x49, 0x0B,
    0xE0, 0xB5, 0xE6, 0x15, 0xE5, 0xA5, 0x5C, 0x15, 0xD4, 0x1D, 0xA6, 0xD8, 0x4C, 0x12, 0xA7, 0x5E,
    0xC7, 0xF2, 0xF3, 0xE7, 0x80, 0xB5, 0x86, 0x66, 0x86, 0x64, 0xF9, 0x36, 0xCF, 0x43, 0x66, 0x26,
    0x6F, 0x71, 0xD5, 0x33, 0x8F, 0xBF, 0x63, 0xF4, 0x3D, 0x05, 0x6A, 0x00, 0x50, 0xA2, 0x2C, 0xBD,
    0xB9, 0x70, 0x66, 0x38, 0x00, 0xC8, 0xB9, 0x17, 0x7E, 0x2F, 0xFF, 0x9F, 0x9A, 0x90, 0xFF, 0xDF,
    0x50, 0x02, 0xE8, 0xB8, 0x53, 0xD3, 0xE4, 0xCD, 0x60, 0x00, 0x7D, 0x00, 0x2D, 0x29, 0xEA, 0xDE,
    0xAB, 0x51, 0xCC, 0xD9, 0x7B, 0x24, 0x1E, 0x4B, 0x4E, 0x89, 0x04, 0xAE, 0x58, 0xBA, 0xC0, 0x04,
    0xFF, 0x22, 0xB3, 0x7F, 0x42, 0x06, 0x8D, 0x27, 0x01, 0x63, 0x8B, 0xBD, 0xF0, 0xB4, 0x93, 0xFE,
    0x1B, 0x4C, 0xED, 0x65, 0x24, 0x57, 0xFB, 0x7D, 0x61, 0x51, 0x64, 0x05, 0x60, 0x5E, 0x95, 0x52,
    0xDC, 0xC8, 0x7C, 0x6C, 0xF4, 0xC6, 0xAE, 0xE1, 0xCE, 0xDB, 0x26, 0xD5, 0x44, 0x13, 0xFC, 0xFF,
    0x86, 0x10, 0x80, 0xD5, 0xCD, 0xAE, 0xFF, 0xEF, 0xE4, 0xFF, 0xBB, 0xF3, 0xFF, 0x14, 0xB8, 0x3B,
    0xD9, 0x52, 0xEE, 0xDB, 0xDA, 0xE9, 0xD1, 0xD2, 0x8B, 0xD6, 0xA1, 0x61, 0x64, 0x53, 0x18, 0x8C,
    0x0B, 0x97, 0x74, 0x39, 0x35, 0x02, 0xA2, 0x9F, 0xE6, 0xFC, 0xF8, 0xD2, 0xF5, 0x98, 0xF3, 0xED,
    0x6A, 0xAF, 0xDF, 0xC6, 0x80, 0xF9, 0x6F, 0x2C, 0x81, 0x1D, 0x0F, 0x4C, 0x80, 0x5D, 0xF3, 0x9F,
    0x02, 0x09, 0xA7, 0x11, 0xA3, 0xF3, 0xF0, 0x89, 0x5D, 0x48, 0x4A, 0xAC, 0xCF, 0xD5, 0x98, 0x4C,
    0xEF, 0xDE, 0xAD, 0x00, 0x50, 0x9A, 0xAD, 0x69, 0xA2, 0x49, 0x3F, 0x5D, 0xBF, 0x0B, 0xE0, 0x77,
    0xE0, 0xBA, 0xE4, 0x2E, 0xA5, 0x02, 0x69, 0x44, 0x7F, 0x60, 0xCC, 0x7B, 0xEC, 0xE5, 0x0C, 0xD7,
    0xCC, 0xB8, 0x60, 0xB1, 0xF3, 0x48, 0x6B, 0x9D, 0xF9, 0xFA, 0xC4, 0xF7, 0xAF, 0xC5, 0xFC, 0xF7,
    0x89, 0x40, 0x6B, 0x8D, 0x49, 0xED, 0x64, 0x02, 0x72, 0x44, 0xF7, 0x47, 0xE7, 0xFA, 0x0F, 0x9E,
    0xD6, 0x1B, 0xCB, 0x0C, 0xE0, 0x7C, 0x8F, 0xC9, 0x60, 0xFD, 0xBF, 0x9D, 0x00, 0x30, 0x7A, 0x43,
    0x27, 0xE3, 0xC6, 0x5C, 0x12, 0x00, 0xD3, 0xD8, 0x0D, 0x64, 0x77, 0xFF, 0x70, 0x32, 0x56, 0xFF,
    0x0F, 0x79, 0x36, 0xB5, 0x2A, 0x45, 0x5F, 0x34, 0x3B, 0xC9, 0x26, 0x69, 0x63, 0x84, 0xFC, 0x78,
    0xC0, 0xF9, 0xE7, 0xCE, 0x01, 0x33, 0x43, 0xB3, 0x4E, 0x0C, 0x79, 0xC8, 0xD2, 0xDF, 0x7A, 0x88,
    0x27, 0xEA, 0x7E, 0x39, 0xC1, 0xBE, 0xAD, 0x0F, 0xBE, 0x05, 0x6D, 0xC4, 0xD8, 0x35, 0x37, 0x39,
    0x45, 0xA1, 0x1F, 0x5D, 0xD0, 0x5D, 0xB4, 0x21, 0xE8, 0x4E, 0x62, 0x36, 0xB4, 0xFE, 0x7F, 0x53,
    0x08, 0x20, 0xEC, 0xFF, 0x63, 0x69, 0xF2, 0x06, 0xA0, 0x28, 0x5C, 0x10, 0x70, 0xCE, 0xEE, 0x03,
    0xA1, 0xB6, 0x73, 0xB2, 0x11, 0x09, 0x00, 0x58, 0x7A, 0xEE, 0x02, 0xD8, 0xDA, 0x4E, 0xB5, 0xFA,
    0x65, 0x2D, 0x40, 0x83, 0x0C, 0x4C, 0x93, 0x45, 0x4A, 0x04, 0xEC, 0x7E, 0x7E, 0x14, 0x8C, 0x92,
    0x3B, 0x09, 0x1B, 0xAA, 0xD4, 0x18, 0xED, 0xED, 0x17, 0xCE, 0x99, 0x15, 0xCE, 0x24, 0xCE, 0xB9,
    0x2B, 0xEA, 0x71, 0x1A, 0x78, 0x68, 0xF4, 0x86, 0xAE, 0x11, 0xF5, 0x65, 0xDD, 0xF0, 0xFC, 0xFF,
    0x86, 0x11, 0x80, 0xD5, 0x1D, 0xC8, 0xFF, 0xF7, 0xF7, 0xFF, 0x53, 0x55, 0xF8, 0x38, 0xB9, 0x1A,
    0x68, 0x6E, 0xE4, 0xBF, 0x73, 0xDF, 0x3B, 0x65, 0xB2, 0xFF, 0xC2, 0xA3, 0x67, 0xF5, 0xC5, 0x1D,
    0x60, 0xD6, 0x08, 0xD6, 0x00, 0x11, 0x94, 0xF5, 0x9A, 0xCA, 0x0A, 0x01, 0x55, 0x1C, 0x30, 0x8C,
    0xC7, 0xB6, 0x5A, 0x60, 0xC0, 0x49, 0x36, 0x31, 0x69, 0xC0, 0x49, 0x04, 0x70, 0xF0, 0xD4, 0xDE,
    0xD0, 0xD2, 0x5F, 0x2E, 0xC6, 0x78, 0xA4, 0x60, 0xFE, 0x3F, 0x34, 0x9A, 0x32, 0xFF, 0xDF, 0x18,
    0x17, 0x20, 0xE4, 0xFF, 0x4F, 0x83, 0x82, 0x77, 0x66, 0x60, 0xCC, 0x7C, 0xE1, 0x80, 0x4F, 0x0A,
    0x65, 0x3E, 0xBB, 0xE2, 0xBC, 0x0E, 0x44, 0x17, 0xF7, 0x88, 0xC2, 0xAF, 0x51, 0xF8, 0x43, 0x1D,
    0x9E, 0xDC, 0xF3, 0x8E, 0xFF, 0x0F, 0xD8, 0x36, 0x79, 0xF3, 0xFF, 0x9A, 0x09, 0x44, 0x1C, 0xA9,
    0x02, 0xEC, 0x04, 0x05, 0x0F, 0x2D, 0x9A, 0x5B, 0xBC, 0x3E, 0x22, 0x27, 0xFF, 0x9F, 0x4C, 0xFE,
    0xBF, 0xFE, 0x0C, 0x69, 0xFC, 0x69, 0xF3, 0x7E, 0xB7, 0x2E, 0x02, 0x18, 0xBB, 0xA1, 0xC3, 0xC6,
    0xC0, 0x96, 0x92, 0xEB, 0xFF, 0x33, 0x87, 0x8B, 0xB5, 0x14, 0x63, 0xC3, 0xC5, 0xA8, 0x15, 0xC0,
    0x20, 0xAD, 0x03, 0xAF, 0x51, 0xEA, 0x8E, 0xF4, 0xFD, 0xFD, 0x27, 0xC4, 0xAD, 0x03, 0x21, 0x80,
    0x3A, 0xFD, 0x7E, 0x7F, 0xF4, 0x10, 0x85, 0xFD, 0x48, 0x66, 0x60, 0xEB, 0x96, 0x91, 0x50, 0xE2,
    0x0F, 0x21, 0x5C, 0x98, 0x59, 0xC3, 0x4F, 0x09, 0xDA, 0x7F, 0x7A, 0x0F, 0xCA, 0x71, 0x4F, 0xFE,
    0x8A, 0x53, 0x71, 0xA0, 0xFE, 0x5F, 0xF3, 0xF2, 0xFF, 0x83, 0xA8, 0x39, 0x41, 0xB2, 0xE3, 0x4E,
    0x27, 0xB9, 0xB2, 0x7B, 0xD9, 0x65, 0x8B, 0x60, 0xEA, 0xFF, 0x07, 0x2D, 0xE0, 0xA2, 0xCA, 0xC1,
    0xEC, 0x37, 0x0F, 0xC7, 0x86, 0x49, 0x7C, 0x9B, 0x09, 0xE7, 0x59, 0x6F, 0xAF, 0x15, 0x78, 0x4F,
    0x84, 0x3F, 0x8B, 0x96, 0x0B, 0x96, 0xEE, 0xF6, 0x8E, 0x90, 0x10, 0x07, 0x76, 0x5B, 0xE0, 0xE8,
    0x6B, 0xC0, 0xC1, 0x7D, 0x30, 0xD3, 0x7F, 0x81, 0x3D, 0x19, 0x90, 0x50, 0x02, 0x9C, 0x80, 0xFD,
    0xF3, 0x3B, 0x0A, 0x65, 0x79, 0x06, 0xD2, 0xCF, 0x86, 0x8E, 0x5F, 0xDF, 0x35, 0x4C, 0x5F, 0xD2,
    0xAA, 0xD9, 0x2B, 0xE8, 0x6A, 0xB6, 0x00, 0xAC, 0x19, 0x1C, 0xF4, 0xFF, 0x9D, 0xFA, 0xFF, 0x20,
    0xE5, 0x2D, 0xDD, 0x2C, 0x0C, 0x11, 0x84, 0x5B, 0xDA, 0xF9, 0xEA, 0xA1, 0x0A, 0xDE, 0x8E, 0x8F,
    0xAB, 0x2F, 0x0B, 0x4F, 0x31, 0xF9, 0xA7, 0xE2, 0xE2, 0x05, 0x41, 0x9A, 0x6B, 0xD8, 0x56, 0x36,
    0xBF, 0x10, 0x4E, 0xB7, 0xF6, 0xE5, 0x81, 0x42, 0xCF, 0x7F, 0xFD, 0xAB, 0x63, 0x5E, 0xFD, 0x9F,
    0x94, 0xAD, 0x19, 0x00, 0x00, 0x53, 0x1D, 0x56, 0x91, 0x3B, 0x6E, 0x3B, 0x00, 0xF0, 0xF8, 0xB6,
    0xA6, 0xFA, 0xFF, 0x75, 0x11, 0x40, 0xC0, 0x3C, 0x5B, 0x67, 0x4C, 0x2E, 0x63, 0xBE, 0x84, 0xC3,
    0xFE, 0x94, 0x7B, 0xC1, 0x0F, 0x3F, 0x9F, 0xF3, 0xCC, 0x9B, 0x99, 0xCF, 0xF0, 0xE3, 0x07, 0x8E,
    0xF9, 0xD3, 0x7C, 0x81, 0xBD, 0xEC, 0x05, 0x69, 0x16, 0x6E, 0x78, 0xAF, 0x0B, 0x62, 0x0A, 0xEC,
    0xED, 0x13, 0xDD, 0x3A, 0x1C, 0xF1, 0x49, 0x7D, 0x06, 0x0E, 0xEE, 0xF5, 0xB7, 0xFF, 0x64, 0x90,
    0x97, 0x0C, 0x14, 0x4D, 0x03, 0xDE, 0xB3, 0x6C, 0x7E, 0xC2, 0xFC, 0x7F, 0x0E, 0x69, 0x99, 0x42,
    0x15, 0xE7, 0x14, 0x40, 0x36, 0xC0, 0x3B, 0x00, 0x80, 0x66, 0xAC, 0xD3, 0xDC, 0x62, 0x29, 0xC8,
    0xCA, 0xEC, 0x04, 0x10, 0x7A, 0x7E, 0x38, 0xA5, 0xE0, 0x24, 0x2B, 0x5C, 0xE2, 0x10, 0x00, 0xAB,
    0xC4, 0x8A, 0x0B, 0xB9, 0x15, 0xFA, 0xF8, 0x2B, 0xE7, 0xDC, 0xFE, 0x50, 0xB6, 0x8E, 0x71, 0xCD,
    0x53, 0xE6, 0xD0, 0x6E, 0x34, 0x82, 0x8C, 0x74, 0x6B, 0x84, 0x9F, 0xBC, 0x7F, 0x0A, 0x8A, 0x08,
    0x0A, 0x0A, 0x0A, 0x04, 0xCB, 0xFC, 0x75, 0x0F, 0x0B, 0x0A, 0x16, 0x08, 0x96, 0xF7, 0x1A, 0xCC,
    0xE7, 0x61, 0x0E, 0x0A, 0xED, 0xA7, 0x37, 0xF4, 0xE1, 0x3E, 0xEC, 0x3D, 0xB5, 0x37, 0x1E, 0xA3,
    0xC9, 0xD3, 0xC8, 0x0C, 0x77, 0x8A, 0xEB, 0xFF, 0xEF, 0x06, 0xF3, 0xCA, 0xD1, 0x1B, 0xBA, 0x86,
    0x9D, 0xC5, 0x0F, 0x79, 0x74, 0x01, 0x9C, 0xC8, 0x98, 0x06, 0xA8, 0x8F, 0xC0, 0x4B, 0xFC, 0xCB,
    0xA1, 0x50, 0x92, 0x4C, 0x31, 0x76, 0xFF, 0xF0, 0x47, 0xC4, 0xFC, 0x17, 0xF6, 0x85, 0x06, 0x08,
    0x55, 0xFA, 0x2E, 0xC3, 0x4B, 0x43, 0x0D, 0xAE, 0x4A, 0x13, 0x94, 0xA1, 0x5C, 0xF6, 0xA3, 0x45,
    0x14, 0x12, 0x7F, 0x5F, 0xF8, 0x95, 0xFB, 0x2A, 0x07, 0x3E, 0x41, 0xC1, 0x38, 0x42, 0xBC, 0xFC,
    0x57, 0xB4, 0xEF, 0xF7, 0x9D, 0xDA, 0x1B, 0xBB, 0x55, 0x49, 0x7B, 0x3E, 0xB7, 0xBB, 0x63, 0x02,
    0xA2, 0xE2, 0x4E, 0x65, 0x0C, 0x8D, 0xDE, 0xD0, 0x39, 0xAC, 0xBE, 0xD2, 0xDC, 0xF9, 0xFF, 0xBA,
    0x08, 0xA0, 0x67, 0x93, 0xA6, 0x51, 0xE7, 0x1E, 0xF6, 0x83, 0xA9, 0x17, 0x80, 0x26, 0xB0, 0x0A,
    0xF2, 0x40, 0xAE, 0x83, 0x62, 0x94, 0x20, 0xFD, 0x00, 0xD4, 0xF0, 0xF1, 0x3A, 0x4F, 0x2B, 0xC2,
    0x5F, 0x65, 0xC8, 0x25, 0x85, 0x2A, 0x28, 0xF6, 0x12, 0x57, 0x38, 0x82, 0xB4, 0x3D, 0x7C, 0x52,
    0x77, 0xCC, 0xEC, 0xCF, 0x55, 0xD5, 0x3F, 0x4A, 0x19, 0x8A, 0x8E, 0x3B, 0xB0, 0x0D, 0x00, 0x66,
    0x2C, 0xB4, 0x69, 0xB4, 0x05, 0xED, 0x29, 0xD5, 0x73, 0x0F, 0x01, 0xAC, 0x23, 0x0A, 0xF0, 0x7A,
    0x74, 0x29, 0x1C, 0x15, 0x68, 0x54, 0x12, 0xD0, 0xBD, 0x6B, 0x6F, 0xEC, 0x66, 0x65, 0xB3, 0x02,
    0x44, 0xAE, 0xAB, 0x89, 0x03, 0x84, 0x17, 0xF7, 0x44, 0x89, 0xB3, 0xD2, 0xBE, 0x5D, 0xFE, 0x14,
    0x60, 0x72, 0x00, 0x90, 0xB1, 0x7F, 0xF1, 0x5C, 0x2F, 0x48, 0x95, 0xB7, 0x44, 0x54, 0x4A, 0x8E,
    0x01, 0x78, 0xF9, 0xFF, 0x64, 0xF2, 0xFF, 0x5B, 0x95, 0x51, 0x56, 0x03, 0x01, 0x30, 0x8D, 0x6C,
    0x24, 0xBB, 0xE3, 0xDB, 0xBA, 0x44, 0xE0, 0x50, 0xFD, 0x3F, 0x4A, 0xD8, 0x0A, 0xAB, 0x48, 0xE8,
    0x18, 0x9D, 0x48, 0x19, 0x6E, 0x29, 0x4C, 0x0E, 0x11, 0xFE, 0x9A, 0x24, 0x80, 0xB3, 0x7F, 0x26,
    0x5D, 0xEB, 0xBB, 0x19, 0x80, 0x91, 0x00, 0x20, 0x03, 0x23, 0x27, 0xF6, 0xE6, 0x57, 0xFB, 0x23,
    0xD1, 0xD0, 0xF1, 0xF2, 0xFF, 0xD9, 0xE4, 0xFF, 0x8F, 0x6E, 0xB4, 0x74, 0x2B, 0xDA, 0x54, 0x75,
    0x1E, 0x80, 0xF5, 0x35, 0xA7, 0xD9, 0x9D, 0x0B, 0x78, 0x11, 0x08, 0xCB, 0x40, 0x11, 0xBE, 0x22,
    0x32, 0x47, 0xB1, 0xC6, 0xE5, 0xBC, 0xE7, 0x0F, 0xA4, 0x06, 0x87, 0xD2, 0x4D, 0x4E, 0x41, 0xAD,
    0x02, 0x90, 0xF4, 0x38, 0x3A, 0x8F, 0xCF, 0xB1, 0x7B, 0xC0, 0xF1, 0xB5, 0x16, 0xDE, 0xEB, 0xEC,
    0xBD, 0xFF, 0xC6, 0xA9, 0xDD, 0x4E, 0x0A, 0x70, 0xCE, 0xC6, 0xA0, 0x17, 0xA4, 0x8C, 0x8B, 0x88,
    0x37, 0xFF, 0x3F, 0xB2, 0xB1, 0x34, 0x02, 0x66, 0x15, 0xDB, 0xD7, 0x2C, 0x2F, 0x16, 0x40, 0xD7,
    0xA9, 0xDA, 0xF5, 0x4D, 0xFA, 0x01, 0xF4, 0x00, 0xD0, 0x44, 0x50, 0x89, 0xF4, 0x46, 0x39, 0x1F,
    0x8E, 0xC1, 0xAD, 0xD8, 0x5E, 0x3B, 0x94, 0xDD, 0xF4, 0x17, 0xD4, 0x45, 0x02, 0x59, 0xAB, 0xF3,
    0x07, 0x09, 0x21, 0x1C, 0xB1, 0xF1, 0xB5, 0x7F, 0x74, 0x79, 0xEF, 0x64, 0x4F, 0x64, 0x28, 0x52,
    0x5E, 0xCD, 0xB4, 0xC4, 0x7C, 0x99, 0xED, 0x00, 0xA0, 0xBE, 0x0C, 0xA5, 0x9B, 0x3C, 0xFF, 0x5F,
    0xB3, 0x05, 0x10, 0xC0, 0xE5, 0x6E, 0xDB, 0x39, 0x48, 0x6F, 0x05, 0xC5, 0xCC, 0x97, 0xF7, 0x85,
    0x32, 0xFE, 0x44, 0xD3, 0x37, 0x69, 0xCC, 0xB3, 0xBF, 0x7A, 0x3F, 0x68, 0xCE, 0x47, 0xB5, 0x7C,
    0xDC, 0x2A, 0xE0, 0xC4, 0xBF, 0x3A, 0x62, 0x01, 0x1C, 0x3D, 0x7F, 0x41, 0x82, 0xCE, 0xCD, 0x6B,
    0xDC, 0x89, 0x5C, 0x4B, 0xC5, 0x02, 0xC1, 0x26, 0xC2, 0x23, 0x00, 0xA0, 0x3F, 0xDD, 0x1A, 0xE1,
    0xAF, 0xC1, 0x02, 0x60, 0x1A, 0xDD, 0x48, 0x76, 0xCF, 0xA6, 0x29, 0x0B, 0xE0, 0x35, 0x41, 0x7A,
    0xE5, 0x3C, 0x93, 0x6D, 0xAA, 0x13, 0xE6, 0xBF, 0x41, 0x1C, 0xD8, 0x03, 0x80, 0x73, 0x36, 0x5F,
    0x3C, 0xAD, 0x02, 0x00, 0xC9, 0xB1, 0xD3, 0x60, 0x7D, 0x3F, 0x4F, 0xC8, 0x4D, 0x4C, 0xC9, 0x5D,
    0xE0, 0xE3, 0x6A, 0x7E, 0x1D, 0x98, 0x06, 0x74, 0x53, 0xE5, 0xC8, 0x84, 0x12, 0xDF, 0x59, 0xD0,
    0x8D, 0xA4, 0x59, 0x84, 0x1C, 0x0F, 0x45, 0xA7, 0x82, 0x36, 0xB0, 0x07, 0x9A, 0x77, 0xB5, 0x7A,
    0xD8, 0x55, 0x65, 0x01, 0x74, 0xDC, 0xE9, 0xAD, 0xD4, 0xE8, 0x03, 0xB0, 0x94, 0xFD, 0xAA, 0x6C,
    0xC5, 0x10, 0x96, 0xA8, 0x42, 0x88, 0x4E, 0x33, 0x05, 0x52, 0xFA, 0xC5, 0x02, 0x68, 0x9E, 0x09,
    0xC0, 0x21, 0x53, 0x3E, 0xCD, 0x12, 0x30, 0xEF, 0x93, 0xFF, 0x58, 0x03, 0xD0, 0x01, 0x0B, 0x40,
    0x7B, 0x07, 0xBC, 0xC7, 0xBB, 0x17, 0xCF, 0x71, 0xA2, 0x6A, 0x84, 0x5C, 0x5A, 0x72, 0x09, 0xED,
    0xF1, 0xFC, 0xFF, 0x63, 0xD7, 0x77, 0x0C, 0x97, 0xBE, 0xCA, 0xAA, 0x95, 0x3B, 0xE8, 0x54, 0x65,
    0x01, 0x94, 0x7A, 0xD3, 0xD6, 0xFF, 0xE7, 0x61, 0x6B, 0xA5, 0x2A, 0x89, 0xC0, 0xAB, 0x30, 0x1B,
    0xE2, 0xB0, 0x94, 0x1D, 0x66, 0x04, 0xCD, 0x8B, 0x06, 0xB8, 0xBE, 0x30, 0x19, 0x93, 0x9E, 0xCD,
    0x4E, 0x3F, 0x64, 0x06, 0x17, 0x43, 0x9B, 0x7B, 0xA4, 0xC0, 0xD0, 0x09, 0x69, 0x3D, 0x2E, 0xAF,
    0x4F, 0x9E, 0xD8, 0xEB, 0x58, 0x0D, 0x09, 0xF7, 0xB2, 0x00, 0xF7, 0x71, 0x1B, 0x00, 0x74, 0x9D,
    0x65, 0xD3, 0x54, 0x0B, 0x7F, 0xB4, 0x54, 0xAD, 0xDC, 0x00, 0x00, 0x91, 0xEF, 0xFF, 0x17, 0x56,
    0x42, 0x22, 0xD2, 0xCD, 0x91, 0x7A, 0x5E, 0x22, 0xFC, 0xCD, 0xEE, 0xF8, 0xF0, 0x44, 0x5D, 0x70,
    0x8B, 0x6F, 0xED, 0xBA, 0x65, 0x41, 0x6A, 0x48, 0xA8, 0x2F, 0x1B, 0x3C, 0x13, 0x01, 0x38, 0x78,
    0xF6, 0xDC, 0x22, 0xB2, 0xA0, 0x02, 0x60, 0xB3, 0x59, 0xFF, 0x3F, 0xB2, 0xC1, 0x6A, 0xE9, 0xB0,
    0xAB, 0x8A, 0x00, 0x46, 0x37, 0x94, 0xEC, 0x9E, 0xBB, 0xA7, 0x2C, 0x00, 0x6B, 0xDC, 0x30, 0x46,
    0xA1, 0x25, 0xC5, 0xCD, 0x5B, 0x20, 0x40, 0x2B, 0x02, 0x69, 0x4E, 0xAF, 0xFD, 0x25, 0x68, 0x82,
    0xE6, 0x0F, 0xC7, 0xC0, 0xB5, 0x11, 0x76, 0xDF, 0xAF, 0x0F, 0x53, 0x06, 0x07, 0x32, 0xCD, 0x5D,
    0xA1, 0x77, 0xD7, 0xFF, 0x13, 0x80, 0xE1, 0x93, 0x7A, 0x8A, 0x36, 0x1C, 0x5D, 0x02, 0xD8, 0x03,
    0x60, 0x57, 0x3B, 0x46, 0x5C, 0xE6, 0x18, 0x40, 0xE9, 0x76, 0xAD, 0xCC, 0xCD, 0xE8, 0x03, 0xB0,
    0x94, 0xA2, 0x51, 0x1D, 0x14, 0x98, 0x04, 0x08, 0x38, 0x72, 0xE9, 0xD9, 0x21, 0xDF, 0x53, 0xBB,
    0x71, 0x65, 0x32, 0x47, 0x60, 0x9E, 0x59, 0x8E, 0x7A, 0x0F, 0xF6, 0xA2, 0xF7, 0x3A, 0xE1, 0xB0,
    0x43, 0x8F, 0x35, 0x6C, 0xF3, 0xDA, 0x14, 0x18, 0x36, 0xF9, 0xCF, 0x83, 0xC7, 0x68, 0x27, 0x70,
    0xE0, 0x8C, 0xD9, 0xC5, 0x32, 0x84, 0xFC, 0x4A, 0x66, 0x8F, 0x8D, 0x6C, 0x28, 0x0D, 0x77, 0xDD,
    0x61, 0xAB, 0x56, 0xEF, 0xA0, 0x9B, 0xD9, 0x02, 0xE8, 0x9C, 0xA7, 0x1D, 0xDF, 0x24, 0x5A, 0xFF,
    0xBF, 0x80, 0x0E, 0x73, 0xD2, 0x9A, 0xC0, 0x91, 0x95, 0x7D, 0x98, 0xBD, 0xE3, 0x45, 0x44, 0x9C,
    0x02, 0xF8, 0x4B, 0x53, 0x23, 0x26, 0xC1, 0xF4, 0xD8, 0xFF, 0xA4, 0x85, 0x8A, 0x2E, 0xEE, 0x06,
    0xB8, 0x71, 0x7F, 0x77, 0x5B, 0x4F, 0x57, 0x93, 0xFB, 0xF6, 0x01, 0x7B, 0xD1, 0x7D, 0xF2, 0xAA,
    0x00, 0x13, 0x14, 0x71, 0x60, 0x25, 0xA1, 0xF3, 0xF9, 0x77, 0xD6, 0xF4, 0x25, 0xDE, 0xDF, 0xBC,
    0x55, 0xFD, 0x89, 0x19, 0x98, 0xCE, 0xC5, 0x6E, 0x05, 0x80, 0x8E, 0x59, 0x4C, 0xE3, 0x2D, 0x6E,
    0x52, 0xA9, 0x06, 0xB1, 0x59, 0x67, 0x04, 0x82, 0x7D, 0x9F, 0x2C, 0x3A, 0xED, 0x42, 0x05, 0x1C,
    0x94, 0xC0, 0xD1, 0x35, 0x8B, 0x31, 0x7B, 0xC7, 0x0B, 0x91, 0x77, 0x83, 0xE5, 0xC1, 0xC3, 0xAB,
    0x38, 0x58, 0xFC, 0x83, 0x1A, 0xBB, 0x3C, 0x50, 0x2F, 0xC2, 0xCB, 0xD9, 0xE7, 0x48, 0xAD, 0x00,
    0x5F, 0xF8, 0x01, 0x5F, 0xE0, 0x95, 0xD1, 0x3C, 0xEE, 0x63, 0x32, 0x52, 0xF5, 0xE6, 0x05, 0x27,
    0xC5, 0x2D, 0xBB, 0xDC, 0x8D, 0xC4, 0xD8, 0x5A, 0x79, 0x8B, 0x40, 0x36, 0x13, 0xEF, 0x6C, 0x57,
    0x8B, 0xB2, 0x11, 0x80, 0x66, 0x1A, 0x55, 0x64, 0xF7, 0xDE, 0x63, 0x97, 0x08, 0x58, 0xED, 0xAA,
    0x3F, 0x8A, 0x16, 0x6C, 0x28, 0x94, 0x46, 0x8C, 0x9B, 0x2C, 0x23, 0xAB, 0xFA, 0x60, 0x1D, 0x1E,
    0x46, 0xCF, 0x73, 0x7B, 0x23, 0xC2, 0x4D, 0x11, 0x2A, 0x28, 0xE2, 0xF5, 0xB6, 0x73, 0xD8, 0x73,
    0x8A, 0x82, 0xE0, 0x58, 0x24, 0xDF, 0x7B, 0x97, 0x10, 0xD0, 0xFC, 0x81, 0xFC, 0x01, 0xA3, 0x74,
    0x54, 0xC0, 0x4A, 0x78, 0xF3, 0xFA, 0x8B, 0x0A, 0xA2, 0x7C, 0x28, 0xCA, 0x82, 0x04, 0x60, 0x0F,
    0x69, 0x7E, 0x16, 0x00, 0x86, 0x37, 0x58, 0x3A, 0x97, 0x04, 0x50, 0xBA, 0x03, 0x34, 0xE5, 0x58,
    0x2B, 0x0B, 0x01, 0x2C, 0xA3, 0xC4, 0x94, 0x9F, 0x22, 0x0A, 0x43, 0x9C, 0x04, 0x8E, 0xFE, 0xD6,
    0x0A, 0xD0, 0xA4, 0x8D, 0x19, 0xBB, 0xF7, 0x07, 0x16, 0xA6, 0xC5, 0x97, 0x95, 0xB0, 0x44, 0x08,
    0xAB, 0x56, 0x7C, 0xD1, 0x65, 0xBB, 0x14, 0xE8, 0xC7, 0x18, 0x01, 0x20, 0x3E, 0xCD, 0x47, 0x60,
    0xB0, 0x29, 0x02, 0xE2, 0x12, 0xC1, 0x5B, 0xD7, 0xAD, 0xC2, 0xC8, 0xBC, 0x9E, 0x02, 0x8D, 0xC1,
    0xE0, 0x26, 0x66, 0x00, 0x80, 0xA1, 0x63, 0x1B, 0x3B, 0x86, 0xE9, 0x0B, 0xAC, 0xB8, 0x05, 0xEB,
    0xFF, 0x6B, 0x22, 0x80, 0xAE, 0xF9, 0xDA, 0x9D, 0x9B, 0x7C, 0x0F, 0x4C, 0xFE, 0x7F, 0x38, 0x80,
    0x58, 0xE8, 0x28, 0x60, 0x4C, 0x90, 0x8F, 0x5C, 0xBD, 0x0A, 0xF6, 0x23, 0xBB, 0xD0, 0xFD, 0xE4,
    0xAB, 0x91, 0x77, 0x0B, 0x96, 0xEF, 0x90, 0xB3, 0x2E, 0x8E, 0x97, 0x55, 0xA5, 0x58, 0x9F, 0x52,
    0xE2, 0x7B, 0xFE, 0xA6, 0xA0, 0x6E, 0x2D, 0xE0, 0xA3, 0x67, 0xCD, 0xC2, 0xD8, 0x15, 0x4B, 0x31,
    0xDE, 0xDB, 0x59, 0xF0, 0x31, 0xC8, 0xDB, 0xCD, 0x83, 0xA6, 0xD7, 0xFF, 0xAB, 0x33, 0x06, 0xE0,
    0xF8, 0xFF, 0xAE, 0x53, 0xE0, 0x13, 0xC0, 0xF4, 0x30, 0x83, 0xA3, 0xA9, 0xA9, 0xC3, 0x6B, 0x97,
    0x62, 0x64, 0xED, 0x12, 0xF4, 0x3C, 0xFA, 0x3C, 0xBA, 0x9E, 0xDA, 0x03, 0x65, 0x87, 0xB7, 0x9E,
    0x48, 0x5E, 0xF5, 0x2C, 0x2E, 0x41, 0xAA, 0x13, 0x10, 0x8A, 0xCC, 0x05, 0xC4, 0x9C, 0xE2, 0xA6,
    0x82, 0xBF, 0xC4, 0xC7, 0xFF, 0xEB, 0x6A, 0xFF, 0xE3, 0xA7, 0xCD, 0xC1, 0xD4, 0x9A, 0x73, 0x30,
    0xB2, 0x60, 0x66, 0xE0, 0x5B, 0x54, 0xC4, 0xE1, 0x66, 0xF6, 0xFF, 0xA3, 0x1D, 0x01, 0x99, 0x6A,
    0x0B, 0x37, 0x57, 0x6A, 0x2B, 0x01, 0xC4, 0x33, 0xEF, 0xB1, 0x2D, 0x00, 0x3B, 0xC9, 0xAB, 0xFF,
    0x07, 0x15, 0xBE, 0xAB, 0xD3, 0xC3, 0x4E, 0x4D, 0x2C, 0x15, 0xCA, 0x0C, 0xD2, 0x8C, 0xD2, 0xE4,
    0x14, 0x3A, 0x26, 0x26, 0xA1, 0x6C, 0x67, 0xED, 0x00, 0x99, 0xBA, 0xD5, 0xD1, 0xC2, 0x96, 0x82,
    0xB8, 0xF9, 0x5F, 0xD6, 0x6D, 0xE2, 0x70, 0xEA, 0x2E, 0xDC, 0x62, 0x9F, 0x44, 0xCE, 0x61, 0x11,
    0xA6, 0x3A, 0x4A, 0x98, 0xE8, 0xEE, 0x8C, 0xDD, 0x20, 0x2E, 0x1C, 0xF1, 0x06, 0x0B, 0xE8, 0x62,
    0x37, 0xC0, 0x2B, 0x8F, 0xAD, 0x2F, 0xB5, 0xA4, 0xFE, 0x5F, 0x6D, 0x16, 0x80, 0xBF, 0x46, 0xA3,
    0x0F, 0xC0, 0x92, 0xE9, 0xAB, 0xEA, 0x08, 0x49, 0x55, 0xFD, 0x4C, 0xB8, 0x13, 0x6C, 0x11, 0x26,
    0xAC, 0x4E, 0x4C, 0x74, 0x77, 0x89, 0xF9, 0xDF, 0x52, 0xE2, 0x48, 0x16, 0x71, 0x2E, 0xF6, 0x10,
    0x74, 0xA3, 0x22, 0x43, 0xC7, 0xD6, 0x97, 0xDA, 0xE6, 0xFF, 0x03, 0x19, 0x12, 0x81, 0x7A, 0xEF,
    0xB5, 0x5D, 0x6E, 0x0E, 0xE6, 0xFF, 0x4F, 0x53, 0x55, 0x67, 0xC2, 0x4D, 0x44, 0x08, 0x95, 0x98,
    0x0D, 0x3F, 0x11, 0xB4, 0xD4, 0x36, 0xA5, 0x94, 0xA3, 0xD8, 0x43, 0x2C, 0xB8, 0xFF, 0x5F, 0xEF,
    0x92, 0xA9, 0xB6, 0x5D, 0x50, 0x35, 0x31, 0x80, 0x75, 0xE1, 0x2D, 0x34, 0x31, 0xFD, 0x12, 0xE6,
    0x29, 0x18, 0x0F, 0xC8, 0xF5, 0x1E, 0xF2, 0xEF, 0x1E, 0x12, 0xE0, 0xAA, 0xDF, 0xCA, 0x39, 0xA1,
    0x19, 0xFF, 0x9F, 0xFC, 0xFA, 0xFF, 0xC3, 0xFA, 0xC7, 0x39, 0x68, 0x56, 0x99, 0x18, 0x40, 0xEF,
    0xBD, 0xB6, 0x32, 0x9B, 0x15, 0xF6, 0x13, 0x53, 0x60, 0x03, 0x90, 0xB8, 0x9F, 0x57, 0x7C, 0x8D,
    0x93, 0x4D, 0xDE, 0xC5, 0x0D, 0x68, 0xC3, 0xAD, 0xE1, 0x82, 0xDE, 0x87, 0x70, 0xA0, 0xD3, 0x5D,
    0x41, 0xBB, 0x1B, 0xC0, 0xCA, 0x63, 0xD7, 0x95, 0x86, 0xFD, 0x55, 0x0E, 0x39, 0xB3, 0x00, 0xAC,
    0xDB, 0xB4, 0xB2, 0xFF, 0x23, 0x69, 0x60, 0xAA, 0x0F, 0xC0, 0x52, 0x73, 0x31, 0x14, 0x2A, 0xA5,
    0x15, 0xFE, 0xDF, 0xBB, 0x6F, 0x50, 0x0A, 0xDA, 0xDA, 0xE9, 0xF9, 0xBF, 0x0F, 0xB1, 0xC9, 0x4F,
    0x37, 0x2B, 0x6A, 0xE8, 0xD8, 0x75, 0xD6, 0x30, 0x98, 0x15, 0xDA, 0xE4, 0xFF, 0x57, 0x8C, 0x01,
    0x74, 0x9D, 0xA8, 0x5D, 0xD1, 0xEE, 0x07, 0xD0, 0x4B, 0x7E, 0xF1, 0x95, 0x48, 0xE7, 0x8B, 0x38,
    0x08, 0x04, 0x59, 0x58, 0x2B, 0x30, 0xED, 0xB9, 0x0D, 0x00, 0x66, 0x79, 0x31, 0xB6, 0x3C, 0xC6,
    0x00, 0xFC, 0xF9, 0xAD, 0x75, 0x86, 0xBE, 0x12, 0xAA, 0x61, 0x8B, 0xF0, 0x0B, 0x04, 0xD9, 0xB9,
    0x80, 0x2C, 0x22, 0xB6, 0x61, 0xEA, 0xFF, 0x1F, 0xBB, 0xD6, 0xE2, 0x7C, 0x12, 0x80, 0x9B, 0xFF,
    0x7F, 0xDF, 0x54, 0x09, 0xC0, 0x6A, 0x0A, 0x98, 0xFF, 0x9E, 0xC5, 0x2F, 0x8E, 0xB0, 0x40, 0x90,
    0x4D, 0xF2, 0x29, 0xB4, 0x81, 0xEE, 0x1E, 0x80, 0x9F, 0xC9, 0x43, 0x18, 0x23, 0xD5, 0x05, 0x50,
    0xB7, 0xB1, 0x2B, 0xE7, 0x8B, 0x00, 0x2C, 0x13, 0x75, 0x2F, 0x10, 0xD4, 0xED, 0x05, 0xB8, 0xBE,
    0xFE, 0xD0, 0xD1, 0x6B, 0xAD, 0x11, 0xFA, 0x02, 0xAB, 0x76, 0x24, 0xFF, 0x64, 0x22, 0x00, 0xBD,
    0x6F, 0xAB, 0x5B, 0xA1, 0xA9, 0x9F, 0xBC, 0xFC, 0x7F, 0xA2, 0xE4, 0xE5, 0xBF, 0x02, 0x81, 0x20,
    0x5D, 0xF8, 0x63, 0x9E, 0xF3, 0xF6, 0x4A, 0xF2, 0xD7, 0x76, 0x02, 0x98, 0xB9, 0xEA, 0x72, 0xB7,
    0xD5, 0x6E, 0xFD, 0x3F, 0x1D, 0x0F, 0xF6, 0x93, 0x90, 0x80, 0x40, 0x90, 0x26, 0xF8, 0x61, 0xE1,
    0x37, 0xF3, 0xFF, 0xD0, 0x30, 0xF5, 0xFF, 0xF9, 0x73, 0xD0, 0xED, 0x6E, 0x66, 0x6A, 0x0C, 0x60,
    0xF8, 0x9A, 0x92, 0x3D, 0xF3, 0xBE, 0x29, 0x8B, 0x4C, 0xFD, 0x3F, 0x80, 0x54, 0xAA, 0x65, 0x23,
    0x24, 0x20, 0x10, 0x24, 0x20, 0x34, 0x5F, 0xEE, 0xAE, 0x60, 0x7E, 0x05, 0x6D, 0xAA, 0xFF, 0x97,
    0xDD, 0x02, 0xF8, 0xDF, 0x1C, 0xAE, 0xFF, 0x47, 0x61, 0x5D, 0x2F, 0x49, 0xB1, 0x02, 0x41, 0x55,
    0xC2, 0x0F, 0xC0, 0xDD, 0x04, 0x88, 0x87, 0x8E, 0x5E, 0xA3, 0x86, 0xE9, 0x4B, 0xED, 0xF7, 0xFF,
    0x53, 0x09, 0x60, 0xE6, 0x62, 0x6F, 0x6E, 0xB2, 0x9F, 0x02, 0xF9, 0xFF, 0xA1, 0xD4, 0x78, 0x81,
    0x40, 0x50, 0x85, 0x1B, 0xE0, 0x6D, 0x09, 0xB2, 0x0D, 0x00, 0x66, 0x2F, 0xD1, 0xB9, 0x90, 0xA2,
    0x52, 0x6A, 0xDC, 0x02, 0x7E, 0xFD, 0x7F, 0xF2, 0x97, 0x68, 0x23, 0x16, 0x00, 0x94, 0xA9, 0x40,
    0x81, 0x20, 0x22, 0x3C, 0x89, 0x5B, 0x92, 0x98, 0xF5, 0xFF, 0xCE, 0xFC, 0xFF, 0x91, 0x6B, 0x14,
    0xE7, 0x94, 0x00, 0x98, 0x8E, 0x5D, 0x43, 0x36, 0x7D, 0x85, 0x4B, 0x04, 0x7B, 0x6D, 0x88, 0x13,
    0x92, 0xE6, 0xFE, 0xC5, 0x1A, 0x10, 0x08, 0x92, 0x4D, 0x7F, 0x5F, 0x3E, 0xDC, 0x4D, 0x8E, 0xF6,
    0x00, 0x78, 0x36, 0x4F, 0xAA, 0x33, 0x46, 0x00, 0xF4, 0x65, 0x10, 0x7F, 0x1A, 0x3C, 0xE7, 0x3C,
    0x7B, 0x11, 0xC8, 0xE4, 0xFF, 0x73, 0x99, 0xC4, 0x3F, 0xB1, 0x00, 0x04, 0x82, 0x14, 0x65, 0xE8,
    0x95, 0x40, 0x31, 0x45, 0x0D, 0x78, 0xE8, 0xC8, 0xEF, 0x97, 0x86, 0xD5, 0xFF, 0x61, 0xA5, 0xDB,
    0x98, 0xFF, 0x5F, 0x96, 0x00, 0x66, 0x9F, 0x3B, 0x45, 0x47, 0x1C, 0xF3, 0xBF, 0x1F, 0xEE, 0xFC,
    0x3F, 0x25, 0xC4, 0x0A, 0xDC, 0xBA, 0x86, 0x24, 0x26, 0x80, 0x40, 0xE0, 0x09, 0x45, 0xC2, 0x22,
    0x59, 0xF2, 0x69, 0xC0, 0xC9, 0xFF, 0x3F, 0xDB, 0xA6, 0x23, 0x39, 0x69, 0x71, 0xDC, 0x02, 0x30,
    0x17, 0xA0, 0xC8, 0xAB, 0xFF, 0xC7, 0x08, 0x2F, 0x00, 0x44, 0xD8, 0x1D, 0x60, 0xF1, 0x03, 0x04,
    0x02, 0x70, 0x62, 0x9D, 0x6C, 0xF6, 0xEA, 0xFF, 0x91, 0xCD, 0x66, 0xFD, 0xFF, 0x91, 0x8F, 0x5A,
    0xB9, 0xB1, 0x9B, 0x4B, 0x51, 0xFF, 0xFF, 0x9D, 0x6B, 0xC8, 0xDE, 0xC0, 0x6C, 0x6D, 0xF9, 0x89,
    0xBD, 0x26, 0x35, 0x9C, 0x11, 0x33, 0x79, 0xC4, 0x0F, 0x10, 0x08, 0xE2, 0x94, 0x40, 0x66, 0xFA,
    0x1F, 0xC4, 0x6D, 0xDC, 0xFF, 0x2F, 0x3B, 0x01, 0xFC, 0x25, 0x08, 0x9F, 0x02, 0x3F, 0xF2, 0x33,
    0xBB, 0xCF, 0x52, 0x8E, 0xFF, 0xCF, 0x4C, 0x2A, 0x58, 0x02, 0x48, 0x44, 0x5D, 0x20, 0xC8, 0x1E,
    0x13, 0x60, 0x76, 0xB6, 0x33, 0x65, 0x60, 0xE8, 0xAD, 0x0F, 0x97, 0x86, 0xE9, 0x2F, 0xDA, 0x57,
    0xFF, 0xAF, 0x32, 0x01, 0xFC, 0x09, 0x08, 0x9F, 0x02, 0x2C, 0x45, 0xFD, 0x8A, 0xD1, 0xA3, 0x19,
    0x9A, 0x09, 0xCA, 0x29, 0x7E, 0x4B, 0x10, 0x7D, 0x2F, 0x10, 0x64, 0x17, 0x7E, 0xC7, 0xA6, 0xF6,
    0xFC, 0x80, 0xED, 0x00, 0xC0, 0xAB, 0x1F, 0x6A, 0x4B, 0xFD, 0xFF, 0x4C, 0x04, 0x30, 0xEB, 0x47,
    0x8C, 0x63, 0xC6, 0xFF, 0x37, 0x42, 0xAE, 0x01, 0x28, 0x9D, 0x22, 0xF8, 0x42, 0x04, 0x02, 0x41,
    0xBA, 0xF0, 0x1B, 0x58, 0x4E, 0x51, 0x79, 0x7E, 0x04, 0x00, 0x70, 0xC5, 0x15, 0x3A, 0x4F, 0x4D,
    0x0D, 0x11, 0xC0, 0xB1, 0xEB, 0x95, 0x7D, 0xED, 0x26, 0xB6, 0x9E, 0x20, 0xBD, 0x9A, 0x09, 0x20,
    0x0D, 0xE5, 0x16, 0xC8, 0x64, 0x21, 0x01, 0x81, 0x20, 0x9B, 0xDC, 0xFB, 0x5B, 0x66, 0x6A, 0x00,
    0x8A, 0x81, 0x57, 0x38, 0x87, 0xFE, 0x7F, 0x98, 0x00, 0x3E, 0xCF, 0x0A, 0x7F, 0x4E, 0xFA, 0xE9,
    0x79, 0xBA, 0x4F, 0x31, 0x96, 0x31, 0x00, 0x56, 0xDE, 0xDE, 0x17, 0xA9, 0x04, 0x20, 0x10, 0x08,
    0x52, 0xCC, 0x7F, 0xF3, 0x1F, 0x03, 0x8F, 0xBD, 0xF6, 0x6F, 0xDA, 0x5F, 0xFF, 0xAF, 0x2C, 0x01,
    0x5C, 0xB7, 0x14, 0x74, 0x0F, 0x00, 0x8B, 0xD0, 0xCF, 0x40, 0x0F, 0x03, 0x5A, 0x03, 0x8A, 0x8D,
    0xF6, 0x17, 0x02, 0x10, 0x08, 0xAA, 0x25, 0x00, 0x2F, 0x11, 0x68, 0x1B, 0x00, 0x9C, 0xFA, 0x53,
    0x4D, 0x7B, 0x73, 0xD6, 0xD4, 0x58, 0x1E, 0x80, 0x22, 0xAC, 0x63, 0x02, 0x98, 0xFD, 0xF9, 0x7F,
    0x46, 0x85, 0xE9, 0x40, 0x81, 0x40, 0x10, 0x21, 0x00, 0x06, 0x9B, 0xFC, 0x7F, 0x76, 0x4A, 0xEA,
    0x63, 0xEF, 0x07, 0x55, 0xEE, 0x44, 0xC7, 0x10, 0x00, 0xD3, 0x3D, 0x1B, 0xA0, 0xC1, 0x4C, 0xEA,
    0x21, 0x7D, 0x31, 0x47, 0x7C, 0x7E, 0x0E, 0xED, 0xEE, 0x9A, 0x9C, 0xF4, 0x23, 0xA4, 0x20, 0x78,
    0x77, 0x82, 0x53, 0x02, 0x01, 0xD0, 0xEC, 0xAC, 0xB6, 0xDD, 0xC3, 0xC0, 0x33, 0x79, 0x15, 0x93,
    0x92, 0xD7, 0x2C, 0x22, 0x5E, 0xF5, 0xB0, 0x9E, 0xCF, 0x44, 0x67, 0x99, 0xE8, 0x05, 0x91, 0x11,
    0x7E, 0x76, 0x37, 0x6F, 0x64, 0xC9, 0x03, 0x10, 0x08, 0xE2, 0x12, 0x1F, 0x4D, 0x01, 0xF6, 0x52,
    0x64, 0xC1, 0xC0, 0xD0, 0xF3, 0x57, 0x5A, 0x23, 0xF8, 0x62, 0xFE, 0xFC, 0x7F, 0x9F, 0x00, 0x6E,
    0x75, 0xAE, 0x42, 0x01, 0x27, 0x30, 0x61, 0x1E, 0xC3, 0x29, 0x5D, 0x1A, 0x34, 0xFF, 0x39, 0x5D,
    0xF9, 0x0B, 0x04, 0x82, 0x04, 0x15, 0x6F, 0x1E, 0x6F, 0x07, 0x80, 0xF7, 0x5F, 0xB6, 0x45, 0x6D,
    0xC9, 0xD1, 0xFC, 0x7F, 0xD8, 0x60, 0x31, 0xD1, 0xC9, 0x4B, 0x1E, 0xD6, 0xAB, 0x18, 0x18, 0x02,
    0xC3, 0x62, 0x80, 0x99, 0x0D, 0x09, 0xB8, 0x7B, 0x99, 0x0A, 0x03, 0x08, 0x04, 0x15, 0x9C, 0x5F,
    0x47, 0xF1, 0x1B, 0x9B, 0xC0, 0x66, 0xE0, 0xB2, 0x67, 0x7E, 0xCB, 0x7A, 0x34, 0x8F, 0x33, 0x00,
    0x81, 0x18, 0x80, 0x61, 0x03, 0x82, 0xA2, 0xA4, 0x1A, 0x26, 0xE4, 0x93, 0x80, 0x40, 0x20, 0x48,
    0x10, 0xFE, 0xF0, 0x2A, 0x40, 0x2F, 0xFF, 0x9F, 0x39, 0x5F, 0xEB, 0xFF, 0xCB, 0x13, 0x00, 0x30,
    0x0E, 0xC0, 0x86, 0x43, 0x04, 0x9E, 0xFF, 0x0F, 0xF8, 0x81, 0x40, 0x81, 0x40, 0xDC, 0x7E, 0xAE,
    0x44, 0x08, 0x9E, 0xFF, 0xFF, 0xF8, 0xFB, 0xAD, 0xE1, 0x3F, 0x67, 0x56, 0x9F, 0xCF, 0xA1, 0xF6,
    0x0F, 0xC6, 0x00, 0x00, 0x00, 0x8A, 0x70, 0x14, 0xC0, 0x30, 0xE0, 0xC4, 0x01, 0x62, 0x32, 0x4F,
    0xB2, 0xF4, 0x57, 0x20, 0x5A, 0x3F, 0xA3, 0x63, 0x0D, 0x66, 0xC7, 0xFF, 0xDF, 0xF2, 0xD0, 0xD6,
    0x5C, 0xE5, 0xFF, 0x27, 0x11, 0x00, 0x63, 0x10, 0x20, 0xC2, 0x41, 0x00, 0xFB, 0x01, 0xCC, 0x23,
    0x77, 0xA1, 0x7F, 0xCC, 0xFC, 0x17, 0x33, 0x40, 0x20, 0x48, 0x25, 0x06, 0x47, 0xF8, 0x15, 0x00,
    0x9B, 0x4D, 0xFD, 0xFF, 0xF7, 0x5D, 0xF1, 0x5E, 0xBD, 0x25, 0xA7, 0xAD, 0x2E, 0x19, 0xE7, 0x9F,
    0x3F, 0xC7, 0xAC, 0xBE, 0x48, 0x34, 0x76, 0xD9, 0x2F, 0xF5, 0x6F, 0x00, 0x2C, 0x09, 0x49, 0xBA,
    0x98, 0xFF, 0x02, 0x41, 0x26, 0xCD, 0xCF, 0xDE, 0xFE, 0x7F, 0xD8, 0xE3, 0xE6, 0xFF, 0xFF, 0xAF,
    0x1C, 0x4B, 0x8F, 0x17, 0x03, 0x78, 0xD5, 0xAD, 0xF8, 0x4D, 0xD8, 0x4E, 0x84, 0xDF, 0x8F, 0x29,
    0x7B, 0x21, 0x01, 0x81, 0x20, 0x4B, 0x4C, 0xC0, 0x5D, 0x3E, 0xF3, 0xE8, 0xF6, 0xB5, 0xF9, 0xCC,
    0xFF, 0x4F, 0x24, 0x80, 0x33, 0x8D, 0x78, 0x2B, 0xC2, 0x2F, 0x88, 0x30, 0x01, 0xA0, 0x13, 0x04,
    0x76, 0xE7, 0xFE, 0x64, 0x3F, 0x00, 0x81, 0x20, 0x53, 0x54, 0x80, 0xCC, 0x14, 0xE0, 0xCF, 0x00,
    0xE0, 0x3F, 0x30, 0xE8, 0xAF, 0xF3, 0x6D, 0xB4, 0x84, 0xB8, 0x8B, 0x00, 0xD0, 0xFB, 0x1F, 0xE5,
    0xCD, 0x44, 0x78, 0x9F, 0x09, 0x5C, 0x58, 0x24, 0xAE, 0xBF, 0x40, 0x50, 0x96, 0x02, 0x02, 0xCB,
    0x7F, 0x89, 0x81, 0x03, 0x00, 0x56, 0x6C, 0x59, 0x6D, 0x1D, 0x04, 0x33, 0xE5, 0x61, 0x07, 0xA0,
    0x8A, 0x16, 0x00, 0x00, 0xDC, 0xA0, 0xA1, 0xEE, 0xB2, 0xC8, 0x56, 0x8F, 0xDA, 0xB7, 0x81, 0xF0,
    0x7E, 0x6F, 0x83, 0x90, 0xC8, 0x26, 0xA0, 0xD3, 0x77, 0x4F, 0x10, 0xA9, 0x79, 0x2E, 0xA8, 0xCB,
    0x0A, 0x60, 0x00, 0x8A, 0x19, 0xFF, 0xF0, 0xE0, 0xA5, 0xD6, 0x41, 0x13, 0x57, 0xD3, 0x79, 0x6E,
    0x77, 0x88, 0x00, 0xEE, 0x52, 0xCE, 0x82, 0x20, 0x1A, 0xD2, 0xF7, 0x11, 0xE1, 0x71, 0x02, 0x2E,
    0x02, 0x60, 0x13, 0xD8, 0x32, 0x34, 0x90, 0xC5, 0x04, 0xAA, 0x52, 0xC8, 0xD2, 0xCE, 0x44, 0xF5,
    0xDD, 0x8A, 0x86, 0xB4, 0x89, 0x9A, 0x70, 0x4E, 0x21, 0x96, 0x7C, 0xF7, 0x4D, 0xB5, 0xF7, 0xDC,
    0x2C, 0xF9, 0x25, 0x68, 0x38, 0x99, 0xB3, 0xEF, 0x10, 0xE1, 0x1B, 0x00, 0x93, 0x55, 0x80, 0x3B,
    0x11, 0x5E, 0x0E, 0x4C, 0xC4, 0x1F, 0xFD, 0x99, 0xB6, 0xEE, 0xFF, 0x5D, 0x6B, 0xEC, 0xAA, 0xC7,
    0xEC, 0xCF, 0x12, 0xF0, 0x33, 0xA3, 0xFC, 0x19, 0x60, 0xA2, 0x32, 0x37, 0xAF, 0x62, 0xA6, 0xA0,
    0xC4, 0x0F, 0x04, 0x45, 0x71, 0xE6, 0x89, 0xAB, 0xFC, 0x82, 0x13, 0xFD, 0x67, 0xA0, 0x44, 0xC0,
    0x57, 0x7E, 0x71, 0x89, 0xF5, 0xF2, 0x47, 0xFE, 0x45, 0x5B, 0x9F, 0x27, 0x65, 0x17, 0x92, 0x8A,
    0x3F, 0xF2, 0x0B, 0x6D, 0xFD, 0xE3, 0x55, 0xCA, 0xFE, 0x9D, 0x5F, 0xD9, 0xDF, 0x22, 0xC2, 0x27,
    0x09, 0x98, 0x72, 0xC9, 0x82, 0x1A, 0x26, 0xD5, 0x94, 0x91, 0xF5, 0xA9, 0xEA, 0x9B, 0x91, 0x2F,
    0x17, 0x80, 0x1A, 0x7C, 0xBE, 0xE9, 0x6A, 0x09, 0x70, 0x81, 0x38, 0x82, 0xA3, 0xAD, 0xB6, 0x19,
    0xB0, 0xC0, 0xF8, 0x25, 0x03, 0xBF, 0xBD, 0xE6, 0x22, 0x35, 0xF1, 0x79, 0x80, 0xF3, 0xEC, 0xFB,
    0x27, 0x5B, 0x00, 0x06, 0xF7, 0x5F, 0x49, 0x9A, 0x06, 0x58, 0x29, 0xD2, 0xFF, 0x03, 0x84, 0x8B,
    0x08, 0x58, 0x93, 0x4C, 0x02, 0x5C, 0x07, 0x11, 0x70, 0x0B, 0x05, 0x1B, 0x6D, 0xFC, 0x2D, 0x11,
    0xFA, 0xE9, 0xD3, 0x37, 0x1C, 0x6B, 0x35, 0x1B, 0xE1, 0x27, 0xE0, 0x00, 0x03, 0x7F, 0xF8, 0xF3,
    0x8B, 0xAC, 0xB1, 0xB5, 0x9C, 0x8F, 0xAD, 0xBF, 0xEB, 0x73, 0xC6, 0x06, 0x58, 0x61, 0x90, 0xF4,
    0xEF, 0xFD, 0xDA, 0x5E, 0x08, 0xC2, 0xCF, 0x09, 0x38, 0x1F, 0xC0, 0x14, 0x05, 0x48, 0x83, 0x32,
    0x9C, 0x4E, 0x2C, 0x7F, 0xC1, 0x74, 0x20, 0xAA, 0xA4, 0x8A, 0x58, 0xEC, 0x6B, 0xFE, 0x61, 0x06,
    0x3E, 0xFC, 0xD3, 0x55, 0xD6, 0x43, 0xD7, 0xFE, 0x33, 0x5B, 0xF7, 0xFE, 0x1E, 0xD9, 0x45, 0xB9,
    0xB2, 0xB2, 0xF2, 0xE9, 0x2E, 0x62, 0xF8, 0xD0, 0x13, 0xF6, 0x62, 0x22, 0xDC, 0x0B, 0x60, 0x05,
    0x39, 0x24, 0x60, 0x21, 0x21, 0x35, 0x80, 0x32, 0xFE, 0x5C, 0xB5, 0x61, 0x96, 0xEA, 0x82, 0x8F,
    0x8D, 0x70, 0x27, 0x1A, 0xA5, 0xA5, 0xA8, 0xE9, 0xE7, 0xA5, 0x34, 0x8F, 0x34, 0xD7, 0x86, 0x37,
    0x35, 0xB1, 0xCF, 0x9B, 0xAC, 0xFF, 0x09, 0x60, 0x66, 0x66, 0x67, 0xAB, 0xEF, 0x12, 0x03, 0x6F,
    0x33, 0xE3, 0x86, 0x9F, 0xAC, 0x2C, 0xFD, 0xE2, 0xDA, 0x9F, 0xB2, 0x75, 0xEF, 0x07, 0x8B, 0x23,
    0xFC, 0x99, 0x24, 0xC2, 0x25, 0x81, 0x8F, 0x3C, 0x69, 0x9F, 0x02, 0xE0, 0xBB, 0x44, 0xF8, 0x90,
    0xF9, 0x92, 0x4D, 0xF0, 0x97, 0x0F, 0x57, 0x3A, 0x19, 0x35, 0xA3, 0x89, 0xC4, 0xB1, 0xB1, 0xC2,
    0x0D, 0xFF, 0x7D, 0x41, 0xA3, 0x69, 0x92, 0x0A, 0x7A, 0x4D, 0xEC, 0x94, 0xCA, 0xD4, 0x00, 0x2C,
    0x53, 0x27, 0xE3, 0x49, 0x06, 0x6E, 0xFE, 0xC7, 0x0B, 0x4B, 0x8F, 0xAF, 0xDF, 0xC7, 0xD6, 0xDD,
    0xA7, 0x14, 0x4B, 0xF8, 0x33, 0xDF, 0x0B, 0x97, 0x04, 0xFE, 0x8C, 0x59, 0x3D, 0xF5, 0xB4, 0xFE,
    0xAF, 0x44, 0xF8, 0x1C, 0x01, 0xF3, 0xCC, 0x09, 0x6C, 0x00, 0x44, 0xCE, 0x2C, 0x21, 0x45, 0x93,
    0x86, 0xA8, 0xA1, 0xAD, 0xCA, 0x60, 0x3B, 0x34, 0xD2, 0x00, 0xE0, 0x84, 0xEF, 0x73, 0x1B, 0xEE,
    0x06, 0xD7, 0x70, 0x1D, 0x5C, 0xAF, 0x25, 0x95, 0xA1, 0xAD, 0x9C, 0xFD, 0x7C, 0xD4, 0x8A, 0x91,
    0xCC, 0x35, 0x7C, 0x86, 0xCA, 0x5C, 0x83, 0xEF, 0xF2, 0xB3, 0x93, 0xE3, 0xCF, 0x25, 0x53, 0x23,
    0x63, 0x8A, 0x19, 0x7F, 0xAD, 0x81, 0xFF, 0x79, 0xFF, 0x8A, 0xD2, 0xE1, 0x6B, 0x1F, 0xD0, 0xD6,
    0xBD, 0x57, 0xAA, 0xC2, 0x09, 0x7F, 0x75, 0xF7, 0xC5, 0xC4, 0x04, 0x00, 0xE0, 0x9A, 0xA7, 0xED,
    0xF3, 0x09, 0xF8, 0x14, 0x11, 0x6E, 0x20, 0x60, 0x96, 0x97, 0x30, 0xE4, 0xCC, 0x85, 0x26, 0x56,
    0x0F, 0xAB, 0xD6, 0xE8, 0xAB, 0x76, 0xD7, 0x71, 0xCE, 0x3A, 0x92, 0x29, 0xC3, 0xB8, 0xE1, 0x32,
    0x5F, 0xAD, 0xC0, 0x41, 0x5C, 0xE7, 0x9D, 0xA0, 0x2C, 0x1D, 0x54, 0x23, 0x91, 0x35, 0x6A, 0x8E,
    0x83, 0x6B, 0xF1, 0xE3, 0x9A, 0x69, 0x19, 0x44, 0x7C, 0x1A, 0x2E, 0x77, 0xDF, 0xDC, 0x8F, 0x55,
    0xEA, 0x4F, 0xFF, 0x3C, 0x56, 0x20, 0x0A, 0xC0, 0x0C, 0xFC, 0x33, 0x33, 0xBE, 0x72, 0xCF, 0xF2,
    0xD2, 0xD6, 0xA0, 0x72, 0x2C, 0xAA, 0xE5, 0x56, 0xAD, 0x98, 0xD1, 0x8D, 0x6F, 0xB0, 0xFA, 0xC1,
    0xE9, 0x0E, 0xDB, 0xAD, 0x7F, 0xC6, 0x3E, 0x9F, 0x08, 0x1B, 0x09, 0xF8, 0x90, 0x89, 0x0F, 0x74,
    0x97, 0x8D, 0x0B, 0x50, 0x6D, 0x83, 0x90, 0x52, 0x9E, 0xD4, 0x53, 0xA5, 0x88, 0xCC, 0x32, 0x67,
    0x6A, 0x46, 0xB5, 0x23, 0xAE, 0xA0, 0xBC, 0xA9, 0x36, 0x82, 0xA9, 0x46, 0xB3, 0x52, 0x46, 0xCD,
    0xC8, 0xB5, 0x5D, 0x56, 0xDB, 0x14, 0x79, 0x3D, 0x56, 0x0C, 0x95, 0x37, 0xEF, 0x93, 0x09, 0x80,
    0x61, 0x03, 0xFC, 0x3C, 0x03, 0xFF, 0x0A, 0xE0, 0x07, 0x3F, 0xBA, 0xA0, 0xB4, 0xD3, 0x13, 0xFC,
    0x82, 0x4C, 0xF5, 0x35, 0x9C, 0x74, 0x07, 0x06, 0x58, 0x4D, 0xDE, 0x0A, 0x7C, 0xC9, 0x65, 0x3E,
    0x66, 0xBA, 0xFE, 0x37, 0x7A, 0x09, 0x80, 0x0B, 0xC9, 0x59, 0x4A, 0x7C, 0x26, 0x31, 0xE6, 0x00,
    0x6C, 0xF9, 0x3F, 0x94, 0xAE, 0xE2, 0xAA, 0xF2, 0xDB, 0x2B, 0xB0, 0x7D, 0xE8, 0x8C, 0x8C, 0xB2,
    0x11, 0x8A, 0x6A, 0x2D, 0xFA, 0xCA, 0x56, 0x4C, 0xB4, 0x41, 0x54, 0xF5, 0x49, 0xA8, 0x0D, 0x37,
    0x96, 0xAB, 0xB4, 0x34, 0xB8, 0x66, 0x8A, 0xA0, 0x96, 0x5C, 0x7B, 0x16, 0xA2, 0xE4, 0x94, 0x0B,
    0x36, 0xCB, 0x79, 0x87, 0x01, 0xEC, 0x05, 0xF0, 0x3C, 0x33, 0x9E, 0x04, 0xF8, 0x99, 0x1F, 0x2E,
    0x2B, 0x8D, 0xB9, 0x63, 0x7D, 0x00, 0xA0, 0xC1, 0x02, 0x6B, 0xFD, 0x86, 0xE1, 0xB3, 0xCC, 0xEA,
    0xC6, 0x37, 0xB4, 0x05, 0x81, 0x60, 0x9A, 0xE3, 0xFA, 0xC7, 0xA7, 0xAC, 0x01, 0x66, 0x35, 0xDD,
    0xAE, 0xAB, 0x41, 0xA4, 0xCB, 0xF4, 0x69, 0x0D, 0x7A, 0xE3, 0x88, 0x63, 0x10, 0x4D, 0xBE, 0xA0,
    0x71, 0xEE, 0xA5, 0x2A, 0x1C, 0x0B, 0xE0, 0x0A, 0x3F, 0x96, 0xA1, 0x25, 0x49, 0xA6, 0x3A, 0x55,
    0x99, 0x28, 0x58, 0x51, 0x47, 0x51, 0x96, 0x00, 0x45, 0xE3, 0xD4, 0x54, 0x55, 0x53, 0xA1, 0x15,
    0x66, 0x15, 0xB9, 0x15, 0x83, 0x84, 0xAA, 0xBB, 0x3F, 0x99, 0xAE, 0x81, 0xAB, 0x88, 0x01, 0x71,
    0x06, 0x2D, 0x4F, 0xD5, 0xB7, 0x2D, 0x78, 0xCE, 0xE7, 0xFF, 0x9F, 0xA6, 0x8E, 0xB3, 0x9D, 0x04,
    0xF8, 0xB3, 0x4E, 0x24, 0xFE, 0xD2, 0x34, 0x30, 0xF5, 0x05, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08,
    0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02,
    0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40,
    0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10,
    0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04,
    0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81,
    0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20,
    0x10, 0x08, 0x04, 0x02, 0x81, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x41, 0x0D, 0xF8, 0xFF, 0x0F,
    0xE7, 0xA3, 0x85, 0xF3, 0x95, 0x34, 0x86, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE,
    0x42, 0x60, 0x82,
};


static std::mutex g_runIconMutex;
static std::wstring g_cachedRunIcoPath;
static std::wstring g_cachedRunPngPath;
thread_local bool g_internalIconChange = false;

std::wstring GetWindhawkModIconFolderPath() {
    std::wstring res;
    wchar_t storageDir[MAX_PATH] = {};
    if (Wh_GetModStoragePath(storageDir, ARRAYSIZE(storageDir)) > 0 && storageDir[0] != L'\0') {
        CreateDirectoryW(storageDir, nullptr);
        res = storageDir;
    } else {
        wchar_t modPath[MAX_PATH] = {};
        if (GetModuleFileNameW(GetModInstanceHandle(), modPath, ARRAYSIZE(modPath)) > 0 && modPath[0] != L'\0') {
            PathRemoveFileSpecW(modPath);
            if (modPath[0] != L'\0') {
                res = modPath;
            }
        }
    }
    while (!res.empty() && (res.back() == L'\\' || res.back() == L'/')) {
        res.pop_back();
    }
    return res;
}

bool DecodeEmbeddedRunPngAtSize(UINT targetSize, std::vector<BYTE>& outBgra, bool premultiply) {
    outBgra.clear();
    if (targetSize == 0 || targetSize > 512) return false;

    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool needCoUninit = (hrCo == S_OK || hrCo == S_FALSE);

    bool ok = false;
    IWICImagingFactory* pWic = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&pWic))) && pWic) {
        IWICStream* pStream = nullptr;
        if (SUCCEEDED(pWic->CreateStream(&pStream)) && pStream) {
            if (SUCCEEDED(pStream->InitializeFromMemory(
                    const_cast<BYTE*>(kEmbeddedModernRunPng),
                    static_cast<DWORD>(sizeof(kEmbeddedModernRunPng))))) {
                IWICBitmapDecoder* pDecoder = nullptr;
                if (SUCCEEDED(pWic->CreateDecoderFromStream(
                        pStream, nullptr, WICDecodeMetadataCacheOnDemand, &pDecoder)) && pDecoder) {
                    IWICBitmapFrameDecode* pFrame = nullptr;
                    if (SUCCEEDED(pDecoder->GetFrame(0, &pFrame)) && pFrame) {
                        IWICBitmapScaler* pScaler = nullptr;
                        if (SUCCEEDED(pWic->CreateBitmapScaler(&pScaler)) && pScaler) {
                            if (SUCCEEDED(pScaler->Initialize(
                                    pFrame, targetSize, targetSize, WICBitmapInterpolationModeFant))) {
                                IWICFormatConverter* pConv = nullptr;
                                if (SUCCEEDED(pWic->CreateFormatConverter(&pConv)) && pConv) {
                                    REFGUID dstFmt = premultiply
                                        ? GUID_WICPixelFormat32bppPBGRA
                                        : GUID_WICPixelFormat32bppBGRA;
                                    if (SUCCEEDED(pConv->Initialize(
                                            pScaler, dstFmt, WICBitmapDitherTypeNone,
                                            nullptr, 0.0, WICBitmapPaletteTypeCustom))) {
                                        UINT stride = targetSize * 4;
                                        UINT bufSize = stride * targetSize;
                                        outBgra.resize(bufSize);
                                        if (SUCCEEDED(pConv->CopyPixels(nullptr, stride, bufSize, outBgra.data()))) {
                                            ok = true;
                                        } else {
                                            outBgra.clear();
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
            }
            pStream->Release();
        }
        pWic->Release();
    }

    if (needCoUninit) {
        CoUninitialize();
    }
    return ok;
}

bool BuildMultiSizeIcoFromEmbeddedPng(std::vector<BYTE>& outIcoBytes) {
    outIcoBytes.clear();
    static constexpr UINT kDibSizes[] = { 16, 20, 24, 32, 40, 48, 64 };
    struct IcoFrameBlob {
        BYTE widthByte;
        BYTE heightByte;
        std::vector<BYTE> data;
    };
    std::vector<IcoFrameBlob> frames;
    frames.reserve(ARRAYSIZE(kDibSizes) + 1);

    for (UINT sz : kDibSizes) {
        std::vector<BYTE> bgra;
        if (!DecodeEmbeddedRunPngAtSize(sz, bgra, false) || bgra.size() != (size_t)sz * sz * 4) {
            return false;
        }
        UINT xorBytes = sz * sz * 4;
        UINT andRowBytes = ((sz + 31) / 32) * 4;
        UINT andBytes = andRowBytes * sz;

        BITMAPINFOHEADER bih = {};
        bih.biSize = sizeof(BITMAPINFOHEADER);
        bih.biWidth = (LONG)sz;
        bih.biHeight = (LONG)(sz * 2);
        bih.biPlanes = 1;
        bih.biBitCount = 32;
        bih.biCompression = BI_RGB;
        bih.biSizeImage = xorBytes + andBytes;

        IcoFrameBlob frame = {};
        frame.widthByte = (BYTE)sz;
        frame.heightByte = (BYTE)sz;
        frame.data.resize(sizeof(BITMAPINFOHEADER) + xorBytes + andBytes, 0);

        std::memcpy(frame.data.data(), &bih, sizeof(BITMAPINFOHEADER));
        BYTE* dstXor = frame.data.data() + sizeof(BITMAPINFOHEADER);
        BYTE* dstAnd = dstXor + xorBytes;

        for (UINT yDib = 0; yDib < sz; ++yDib) {
            UINT ySrc = sz - 1 - yDib;
            const BYTE* srcRow = bgra.data() + (size_t)ySrc * sz * 4;
            BYTE* outRow = dstXor + (size_t)yDib * sz * 4;
            std::memcpy(outRow, srcRow, (size_t)sz * 4);
            for (UINT x = 0; x < sz; ++x) {
                if (srcRow[x * 4 + 3] == 0) {
                    dstAnd[yDib * andRowBytes + (x >> 3)] |= (BYTE)(0x80 >> (x & 7));
                }
            }
        }
        frames.push_back(std::move(frame));
    }

    // Add 256x256 PNG frame
    {
        IcoFrameBlob pngFrame = {};
        pngFrame.widthByte = 0;
        pngFrame.heightByte = 0;
        pngFrame.data.assign(kEmbeddedModernRunPng, kEmbeddedModernRunPng + sizeof(kEmbeddedModernRunPng));
        frames.push_back(std::move(pngFrame));
    }

    WORD numFrames = (WORD)frames.size();
    DWORD offset = 6 + 16 * (DWORD)numFrames;
    size_t totalBytes = offset;
    for (const auto& f : frames) {
        totalBytes += f.data.size();
    }

    outIcoBytes.resize(totalBytes, 0);
    WORD icoHeader[3] = { 0, 1, numFrames };
    std::memcpy(outIcoBytes.data(), icoHeader, sizeof(icoHeader));

    #pragma pack(push, 1)
    struct ICONDIRENTRY_RAW {
        BYTE bWidth;
        BYTE bHeight;
        BYTE bColorCount;
        BYTE bReserved;
        WORD wPlanes;
        WORD wBitCount;
        DWORD dwBytesInRes;
        DWORD dwImageOffset;
    };
    #pragma pack(pop)

    auto* dirEntries = reinterpret_cast<ICONDIRENTRY_RAW*>(outIcoBytes.data() + 6);
    for (size_t i = 0; i < frames.size(); ++i) {
        dirEntries[i].bWidth = frames[i].widthByte;
        dirEntries[i].bHeight = frames[i].heightByte;
        dirEntries[i].bColorCount = 0;
        dirEntries[i].bReserved = 0;
        dirEntries[i].wPlanes = 1;
        dirEntries[i].wBitCount = 32;
        dirEntries[i].dwBytesInRes = (DWORD)frames[i].data.size();
        dirEntries[i].dwImageOffset = offset;
        std::memcpy(outIcoBytes.data() + offset, frames[i].data.data(), frames[i].data.size());
        offset += (DWORD)frames[i].data.size();
    }

    return true;
}

bool WriteBinaryFileAtomically(const std::wstring& targetPath, const void* data, size_t size) {
    if (targetPath.empty() || !data || size == 0) return false;
    std::wstring tmpPath = targetPath + L".tmp";

    DWORD attrs = GetFileAttributesW(tmpPath.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_READONLY)) {
        SetFileAttributesW(tmpPath.c_str(), attrs & ~FILE_ATTRIBUTE_READONLY);
    }

    HANDLE hFile = CreateFileW(tmpPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                               nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        BOOL ok = WriteFile(hFile, data, (DWORD)size, &written, nullptr);
        FlushFileBuffers(hFile);
        CloseHandle(hFile);
        if (ok && written == (DWORD)size) {
            DWORD dstAttrs = GetFileAttributesW(targetPath.c_str());
            if (dstAttrs != INVALID_FILE_ATTRIBUTES && (dstAttrs & FILE_ATTRIBUTE_READONLY)) {
                SetFileAttributesW(targetPath.c_str(), dstAttrs & ~FILE_ATTRIBUTE_READONLY);
            }
            if (MoveFileExW(tmpPath.c_str(), targetPath.c_str(),
                            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
                return true;
            }
        }
        DeleteFileW(tmpPath.c_str());
    }

    HANDLE hDirect = CreateFileW(targetPath.c_str(), GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                 nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hDirect != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        BOOL ok = WriteFile(hDirect, data, (DWORD)size, &written, nullptr);
        FlushFileBuffers(hDirect);
        CloseHandle(hDirect);
        return ok && written == (DWORD)size;
    }
    return false;
}

bool CheckFileExistsWithMinSize(const std::wstring& path, ULONGLONG minBytes, ULONGLONG exactBytes = 0) {
    if (path.empty()) return false;
    WIN32_FILE_ATTRIBUTE_DATA fad = {};
    if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fad) ||
        (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return false;
    }
    ULARGE_INTEGER sz = {};
    sz.LowPart = fad.nFileSizeLow;
    sz.HighPart = fad.nFileSizeHigh;
    if (exactBytes > 0) return sz.QuadPart == exactBytes;
    return sz.QuadPart >= minBytes;
}

std::wstring GetUserRunShortcutPath() {
    wchar_t programsDir[MAX_PATH] = {};
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PROGRAMS, nullptr, SHGFP_TYPE_CURRENT, programsDir)) &&
        programsDir[0] != L'\0') {
        std::wstring lnkPath = std::wstring(programsDir) + L"\\System Tools\\Run.lnk";
        if (GetFileAttributesW(lnkPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            return lnkPath;
        }
    }
    return L"";
}

// Updates %APPDATA%\Microsoft\Windows\Start Menu\Programs\System Tools\Run.lnk icon
// (which Windows 10/11 Taskbar uses for Microsoft.Windows.Shell.RunDialog) and backs up original icon
void SyncSystemRunShortcutIcon(const std::wstring&, bool) {
    // Taskbar icon identity is handled directly via Windhawk.ModernRunDialog.UI AppUserModelID
    // and WM_GETICON / GCLP_HICON without mutating Run.lnk or broadcasting SHCNE_ASSOCCHANGED.
}

// Automatically installs or updates modern-run-icon.ico and modern-run-icon.png inside Windhawk folder
void EnsureWindhawkRunIconInstalled(bool forceUpdate = false) {
    std::wstring icoPath;
    {
        std::lock_guard<std::mutex> lock(g_runIconMutex);

        std::wstring folder = GetWindhawkModIconFolderPath();
        if (folder.empty()) {
            wchar_t tempDir[MAX_PATH] = {};
            if (GetTempPathW(ARRAYSIZE(tempDir), tempDir) > 0 && tempDir[0] != L'\0') {
                folder = std::wstring(tempDir) + L"WindhawkModernRun";
                CreateDirectoryW(folder.c_str(), nullptr);
            }
        }
        if (folder.empty()) return;

        icoPath = folder + L"\\" + kRunIconIcoFileName;
        std::wstring pngPath = folder + L"\\" + kRunIconPngFileName;

        wchar_t installedVer[128] = {};
        Wh_GetStringValue(kRunIconVersionKey, installedVer, ARRAYSIZE(installedVer));
        bool versionMatched = (wcscmp(installedVer, kRunIconAssetVersion) == 0);

        bool pngValid = CheckFileExistsWithMinSize(pngPath, sizeof(kEmbeddedModernRunPng), sizeof(kEmbeddedModernRunPng));
        bool icoValid = CheckFileExistsWithMinSize(icoPath, sizeof(kEmbeddedModernRunPng) + 4096, 0);

        if (forceUpdate || !versionMatched || !pngValid || !icoValid) {
            std::vector<BYTE> icoBytes;
            bool builtIco = BuildMultiSizeIcoFromEmbeddedPng(icoBytes) && !icoBytes.empty();
            bool wrotePng = WriteBinaryFileAtomically(pngPath, kEmbeddedModernRunPng, sizeof(kEmbeddedModernRunPng));
            bool wroteIco = builtIco && WriteBinaryFileAtomically(icoPath, icoBytes.data(), icoBytes.size());

            if (!wroteIco && builtIco) {
                wchar_t tempDir[MAX_PATH] = {};
                if (GetTempPathW(ARRAYSIZE(tempDir), tempDir) > 0 && tempDir[0] != L'\0') {
                    std::wstring fallbackDir = std::wstring(tempDir) + L"WindhawkModernRun";
                    CreateDirectoryW(fallbackDir.c_str(), nullptr);
                    icoPath = fallbackDir + L"\\" + kRunIconIcoFileName;
                    pngPath = fallbackDir + L"\\" + kRunIconPngFileName;
                    wrotePng = WriteBinaryFileAtomically(pngPath, kEmbeddedModernRunPng, sizeof(kEmbeddedModernRunPng));
                    wroteIco = WriteBinaryFileAtomically(icoPath, icoBytes.data(), icoBytes.size());
                }
            }

            if (wrotePng && wroteIco) {
                Wh_SetStringValue(kRunIconVersionKey, kRunIconAssetVersion);
            }
        }

        g_cachedRunIcoPath = icoPath;
        g_cachedRunPngPath = pngPath;
    }

    if (!icoPath.empty()) {
        SyncSystemRunShortcutIcon(icoPath, false);
    }
}

// Deletes the installed icon files from Windhawk folder and restores Run.lnk when the mod is unloaded / uninstalled
void CleanupWindhawkRunIconFiles() {
    // Keep generated icon files cached in mod storage so mod updates/reloads are instant and lock-free.
}

HICON CreateCustomRunIconInMemory(int sizePx) {
    sizePx = std::clamp(sizePx, 16, 256);
    std::vector<BYTE> bgra;
    if (!DecodeEmbeddedRunPngAtSize((UINT)sizePx, bgra, false) ||
        bgra.size() != (size_t)sizePx * sizePx * 4) {
        return nullptr;
    }

    BITMAPV5HEADER bi = {};
    bi.bV5Size = sizeof(BITMAPV5HEADER);
    bi.bV5Width = sizePx;
    bi.bV5Height = -sizePx;
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask   = 0x00FF0000;
    bi.bV5GreenMask = 0x0000FF00;
    bi.bV5BlueMask  = 0x000000FF;
    bi.bV5AlphaMask = 0xFF000000;

    HDC hdc = GetDC(nullptr);
    void* lpBits = nullptr;
    HBITMAP hColor = CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&bi),
                                      DIB_RGB_COLORS, &lpBits, nullptr, 0);
    ReleaseDC(nullptr, hdc);
    if (!hColor || !lpBits) {
        if (hColor) DeleteObject(hColor);
        return nullptr;
    }

    std::memcpy(lpBits, bgra.data(), bgra.size());
    HBITMAP hMask = CreateBitmap(sizePx, sizePx, 1, 1, nullptr);

    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmMask = hMask;
    ii.hbmColor = hColor;
    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hColor);
    if (hMask) DeleteObject(hMask);
    return hIcon;
}

HICON LoadCustomRunIcon(int sizePx) {
    sizePx = std::clamp(sizePx, 16, 256);
    EnsureWindhawkRunIconInstalled(false);

    std::wstring icoPath;
    {
        std::lock_guard<std::mutex> lock(g_runIconMutex);
        icoPath = g_cachedRunIcoPath;
    }
    if (!icoPath.empty()) {
        HICON hLoaded = static_cast<HICON>(
            LoadImageW(nullptr, icoPath.c_str(), IMAGE_ICON, sizePx, sizePx, LR_LOADFROMFILE));
        if (hLoaded) {
            return hLoaded;
        }
    }
    return CreateCustomRunIconInMemory(sizePx);
}

void ApplyWindowTaskbarIconIdentity(HWND hDlg, bool restoreDefault = false) {
    if (!hDlg || !IsWindow(hDlg)) return;

    IPropertyStore* pps = nullptr;
    if (FAILED(SHGetPropertyStoreForWindow(hDlg, IID_PPV_ARGS(&pps))) || !pps) {
        return;
    }

    if (restoreDefault) {
        PROPVARIANT pvEmpty;
        PropVariantInit(&pvEmpty);
        pps->SetValue(kPKEY_AppUserModel_RelaunchIconResource, pvEmpty);
        pps->SetValue(kPKEY_AppUserModel_RelaunchCommand, pvEmpty);
        pps->SetValue(kPKEY_AppUserModel_RelaunchDisplayNameResource, pvEmpty);
        pps->SetValue(kPKEY_AppUserModel_PreventPinning, pvEmpty);

        PROPVARIANT pvOrigId;
        PropVariantInit(&pvOrigId);
        pvOrigId.vt = VT_LPWSTR;
        pvOrigId.pwszVal = const_cast<LPWSTR>(kOrigRunAppId);
        pps->SetValue(kPKEY_AppUserModel_ID, pvOrigId);
        pps->Commit();
        pps->Release();
        return;
    }

    std::wstring icoPath;
    {
        std::lock_guard<std::mutex> lock(g_runIconMutex);
        icoPath = g_cachedRunIcoPath;
    }

    // 1. Clear PreventPinning and the old shell32 RunDialog AppUserModel.ID (using VT_EMPTY first
    // so CWindowPropStore::SetValue permits setting Relaunch properties without E_INVALIDARG)
    PROPVARIANT pvEmpty;
    PropVariantInit(&pvEmpty);
    pps->SetValue(kPKEY_AppUserModel_PreventPinning, pvEmpty);
    pps->SetValue(kPKEY_AppUserModel_RelaunchIconResource, pvEmpty);
    pps->SetValue(kPKEY_AppUserModel_RelaunchCommand, pvEmpty);
    pps->SetValue(kPKEY_AppUserModel_RelaunchDisplayNameResource, pvEmpty);
    pps->SetValue(kPKEY_AppUserModel_ID, pvEmpty);
    pps->Commit();

    // 2. Set Relaunch properties and a dedicated custom AppUserModel.ID ("Windhawk.ModernRunDialog.UI")
    // so Windows 10/11 Taskbar never groups the Run dialog into File Explorer (Microsoft.Windows.Explorer)
    // and never overrides the icon with the cached classic Run.lnk icon (Microsoft.Windows.Shell.RunDialog)
    if (!icoPath.empty()) {
        std::wstring iconRes = icoPath + L",0";

        PROPVARIANT pvIcon;
        PropVariantInit(&pvIcon);
        pvIcon.vt = VT_LPWSTR;
        pvIcon.pwszVal = const_cast<LPWSTR>(iconRes.c_str());
        pps->SetValue(kPKEY_AppUserModel_RelaunchIconResource, pvIcon);

        PROPVARIANT pvCmd;
        PropVariantInit(&pvCmd);
        pvCmd.vt = VT_LPWSTR;
        pvCmd.pwszVal = const_cast<LPWSTR>(L"explorer.exe Shell:::{2559a1f3-21d7-11d4-bdaf-00c04f60b9f0}");
        pps->SetValue(kPKEY_AppUserModel_RelaunchCommand, pvCmd);

        PROPVARIANT pvName;
        PropVariantInit(&pvName);
        pvName.vt = VT_LPWSTR;
        pvName.pwszVal = const_cast<LPWSTR>(L"@shell32.dll,-12710");
        pps->SetValue(kPKEY_AppUserModel_RelaunchDisplayNameResource, pvName);
    }

    PROPVARIANT pvCustomId;
    PropVariantInit(&pvCustomId);
    pvCustomId.vt = VT_LPWSTR;
    pvCustomId.pwszVal = const_cast<LPWSTR>(kCustomRunAppId);
    pps->SetValue(kPKEY_AppUserModel_ID, pvCustomId);
    pps->Commit();
    pps->Release();
}

void NotifyTaskbarRedrawWindowIcon(HWND hDlg) {
    if (!hDlg || !IsWindow(hDlg)) return;
    static UINT s_msgShellHook = RegisterWindowMessageW(L"SHELLHOOK");
    if (s_msgShellHook != 0) {
        if (HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
            PostMessageW(hTray, s_msgShellHook, 6 /*HSHELL_REDRAW*/, reinterpret_cast<LPARAM>(hDlg));
        }
    }
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
    int webSearchKind = 0; // 0 = None, 1 = Google Search
    std::wstring webSearchQuery;
    bool isMathResult = false;
    std::wstring mathResultValue;
};

struct ActionButtonsLayout {
    RECT rcBrowse;
    RECT rcTranslate;
    RECT rcRun;
    RECT rcClose;
    RECT rcGear;
    RECT rcEdit;
    RECT rcSrcLang;
    RECT rcSwapLang;
    RECT rcDstLang;
};

struct RunDialogState {
    HWND hDlg = nullptr;
    HWND hCombo = nullptr;
    HWND hOrigEdit = nullptr;
    HWND hModernEdit = nullptr;
    HWND hOkBtn = nullptr;

    HFONT hFontEditCtl = nullptr;
    HICON hRunIcon = nullptr;
    int runIconLoadedSize = 0;
    int taskbarIconLoadedDpi = 0;
    HICON hTaskbarIconSmall = nullptr;
    HICON hTaskbarIconBig = nullptr;
    HICON hOrigIconSmall = nullptr;
    HICON hOrigIconBig = nullptr;
    HICON hOrigClassIconSmall = nullptr;
    HICON hOrigClassIconBig = nullptr;
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
    bool anchoredTop = false;
    float animAlpha = 1.0f;
    bool openAnimActive = false;
    bool closingAnimActive = false;
    ULONGLONG openAnimStartTick = 0;
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

    // Multi-Language Translate Mode State
    bool translateMode = false;
    bool translateBtnHovered = false;
    bool translateBtnPressed = false;
    std::wstring translateSourceLang = L"auto";
    std::wstring translateTargetLang = L"vi";
    std::wstring translatedText;
    std::wstring translatedDetectedLang;
    bool isTranslating = false;
    bool translateCopiedFeedback = false;
    bool srcLangHovered = false;
    bool srcLangPressed = false;
    bool swapLangHovered = false;
    bool swapLangPressed = false;
    bool dstLangHovered = false;
    bool dstLangPressed = false;
    bool transCardHovered = false;
    bool transCardPressed = false;
    bool isTriggeringRun = false;
    bool isBrowsingFile = false;
    bool suppressSuggestionUpdate = false;
    bool inputUpdateQueued = false;
    bool imeComposing = false;
    std::wstring lastSuggestedQuery;
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
void StartKeyHookThread() {
    // Handled natively via Explorer's Win+R hotkey routing into ShowWindow_Hook / SetForegroundWindow_Hook.
}

void StopKeyHookThread() {
}

bool ResolveDialogLightContrast(const RunDialogState* state);
void ApplyWindowAppearance(HWND hDlg, RunDialogState* state);
void CloseActiveGearMenu();
void CloseRunDialog(HWND hDlg);
void SyncInputToNativeCombo(RunDialogState* state, const wchar_t* textOverride = nullptr);
void ExecuteCommandImmediate(RunDialogState* state, const std::wstring& rawCommand);
void PerformDialogUninit(HWND hWnd, RunDialogState* state);
void RecomputeDialogMetrics(RunDialogState* state);
void UpdateDialogEditFont(RunDialogState* state);
void RefreshDialogRunIcons(RunDialogState* state);
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
    if (state && state->settings.customDialogWidth >= 300) {
        int wDip = std::clamp(state->settings.customDialogWidth, 300, 1200);
        return DpiScale(wDip, state->dpi, 100);
    }
    if (state && state->settings.minimalMode) {
        return DpiScaleS(kBaseCompactDlgWidth, state);
    }
    return DpiScaleS(kBaseDlgWidth, state);
}

std::wstring GetEffectiveInputText(HWND hEdit, DWORD* outSelStart = nullptr, DWORD* outSelEnd = nullptr);

int MeasureWrappedTextHeightPx(const RunDialogState* state, const std::wstring& text, float availWidth, float baseFontDip) {
    int minLineH = DpiScaleS(24, state);
    if (!g_pDWriteFactory) return minLineH;
    std::wstring sample = text.empty() ? L"Ag" : text;
    if (!sample.empty() && (sample.back() == L'\n' || sample.back() == L'\r')) {
        sample.push_back(L' ');
    }
    IDWriteTextFormat* pFmt = nullptr;
    int measuredH = minLineH;
    if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
            GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(baseFontDip, state), L"", &pFmt)) && pFmt) {
        pFmt->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
        IDWriteTextLayout* pLayout = nullptr;
        float safeW = (std::max)(80.0f, availWidth);
        if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                sample.c_str(), (UINT32)sample.size(), pFmt, safeW, 8000.0f, &pLayout)) && pLayout) {
            DWRITE_TEXT_METRICS tm = {};
            if (SUCCEEDED(pLayout->GetMetrics(&tm))) {
                measuredH = (std::max)(minLineH, (int)std::ceil(tm.height));
            }
            pLayout->Release();
        }
        pFmt->Release();
    }
    return measuredH;
}

int GetInputBarHeight(const RunDialogState* state) {
    if (state && state->translateMode) {
        int dlgW = GetDlgWidth(state);
        bool isCompact = state->settings.minimalMode;
        int rightButtonsReserve = isCompact ? DpiScaleS(108, state) : DpiScaleS(210, state);
        int leftReserve = isCompact ? DpiScaleS(46, state) : DpiScaleS(50, state);
        float availEditW = (float)(std::max)(DpiScaleS(110, state), dlgW - leftReserve - rightButtonsReserve);
        std::wstring curText = (state->hModernEdit && IsWindow(state->hModernEdit))
            ? GetEffectiveInputText(state->hModernEdit)
            : L"";
        int textH = MeasureWrappedTextHeightPx(state, curText, availEditW, 13.5f);
        int minEditH = DpiScaleS(isCompact ? 28 : 32, state);
        int editZoneH = std::clamp(textH + DpiScaleS(4, state), minEditH, DpiScaleS(260, state));
        int langBarH = DpiScaleS(isCompact ? 23 : 26, state);
        int topPad   = DpiScaleS(isCompact ? 8 : 10, state);
        int midGap   = DpiScaleS(isCompact ? 6 : 8, state);
        int botPad   = DpiScaleS(isCompact ? 8 : 10, state);
        return topPad + langBarH + midGap + editZoneH + botPad;
    }
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

inline bool HasActiveCustomBackground(const RunDialogState*) {
    return false;
}

bool NeedsBackdropSnapshot(const RunDialogState* state) {
    if (!state) return false;
    SurfaceTheme surf = ResolveEffectiveSurface(state->settings);
    return (surf == SurfaceTheme::Glass || surf == SurfaceTheme::Blur);
}

int GetVisibleRowCount(const RunDialogState* state) {
    if (!state) return 0;
    // In Compact Mini-Pill mode, hide initial history list so only the cute single pill bar appears on open,
    // while still showing App Suggestions & Math Calculator results as soon as the user types!
    if (state->settings.minimalMode && !state->showingSuggestions) {
        return 0;
    }
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

void ReleaseCustomBackgroundMedia(RunDialogState*) {}
void ReloadCustomBackgroundMedia(RunDialogState*, bool) {}
void StepCustomBackgroundAnimation(RunDialogState*) {}

UINT DetectMonitorTaskbarEdge(HMONITOR, const MONITORINFO& mi) {
    int leftInset   = mi.rcWork.left - mi.rcMonitor.left;
    int rightInset  = mi.rcMonitor.right - mi.rcWork.right;
    int topInset    = mi.rcWork.top - mi.rcMonitor.top;
    int bottomInset = mi.rcMonitor.bottom - mi.rcWork.bottom;

    if (leftInset > 0 && leftInset >= rightInset && leftInset >= topInset && leftInset >= bottomInset) {
        return ABE_LEFT;
    }
    if (rightInset > 0 && rightInset >= leftInset && rightInset >= topInset && rightInset >= bottomInset) {
        return ABE_RIGHT;
    }
    if (topInset > 0 && topInset >= bottomInset) {
        return ABE_TOP;
    }
    if (bottomInset > 0) {
        return ABE_BOTTOM;
    }

    // Fallback for auto-hide taskbar
    APPBARDATA abd = { sizeof(abd) };
    if (SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) {
        if (abd.uEdge == ABE_LEFT || abd.uEdge == ABE_RIGHT || abd.uEdge == ABE_TOP || abd.uEdge == ABE_BOTTOM) {
            return abd.uEdge;
        }
    }
    if (HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        RECT rcTray = {};
        if (GetWindowRect(hTray, &rcTray)) {
            int tw = rcTray.right - rcTray.left;
            int th = rcTray.bottom - rcTray.top;
            if (th > tw) {
                return (rcTray.left <= mi.rcMonitor.left + 16) ? ABE_LEFT : ABE_RIGHT;
            }
            if (rcTray.top <= mi.rcMonitor.top + 16 && rcTray.bottom < mi.rcMonitor.bottom - 16) {
                return ABE_TOP;
            }
        }
    }
    return ABE_BOTTOM;
}

void GetDefaultTaskbarAnchoredPosition(HWND hDlg, int width, int height, UINT dpi,
                                       int* outX, int* outY, bool* outAnchoredTop = nullptr) {
    HMONITOR hMon = MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    RECT rcWork = {};
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        rcWork = mi.rcWork;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
        mi.rcMonitor = rcWork;
        mi.rcWork = rcWork;
    }

    UINT edge = DetectMonitorTaskbarEdge(hMon, mi);
    int gap = DpiScale(12, dpi);
    int workW = rcWork.right - rcWork.left;

    // Always center horizontally on the monitor work area
    *outX = rcWork.left + (workW - width) / 2;

    // If taskbar is on the Left, Right, or Top -> place at Top-Center;
    // If taskbar is at the Bottom -> place at Bottom-Center right above the taskbar!
    if (edge == ABE_LEFT || edge == ABE_RIGHT || edge == ABE_TOP) {
        *outY = rcWork.top + gap;
        if (outAnchoredTop) *outAnchoredTop = true;
    } else {
        *outY = rcWork.bottom - height - gap;
        if (outAnchoredTop) *outAnchoredTop = false;
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

void StartDialogOpenAnimation(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    state->closingAnimActive = false;
    state->openAnimActive = true;
    state->openAnimStartTick = GetTickCount64();
    state->animAlpha = 0.08f;
    SetTimer(state->hDlg, kTimerOpenAnim, 12, nullptr);
}

void PlayDialogCloseAnimationSync(HWND hDlg, RunDialogState* state) {
    if (!state || !hDlg || !IsWindow(hDlg) || !IsWindowVisible(hDlg) || IsIconic(hDlg)) return;
    if (GetWindowThreadProcessId(hDlg, nullptr) != GetCurrentThreadId()) return;
    if (state->closingAnimActive) return;

    KillTimer(hDlg, kTimerOpenAnim);
    state->openAnimActive = false;
    state->closingAnimActive = true;

    ULONGLONG startTick = GetTickCount64();
    constexpr float kCloseDurationMs = 90.0f;
    int maxSlide = DpiScaleS(12, state) * (state->anchoredTop ? -1 : 1);

    while (IsWindow(hDlg)) {
        float t = std::clamp((float)(GetTickCount64() - startTick) / kCloseDurationMs, 0.0f, 1.0f);
        float ease = 1.0f - t * t;
        state->animAlpha = ease * 0.90f;
        int curY = state->normalY + (int)std::round((1.0f - ease) * maxSlide);

        SetWindowPos(hDlg, nullptr, state->normalX, curY, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(hDlg, nullptr, FALSE);
        UpdateWindow(hDlg);
        if (t >= 1.0f) break;
        if (FAILED(DwmFlush())) {
            Sleep(12);
        }
    }
    state->closingAnimActive = false;
}

void ResizeDialogPreservingBottomAnchor(RunDialogState* state, int oldHeight) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    int dlgW = GetDlgWidth(state);
    RECT rcCur = {};
    GetWindowRect(state->hDlg, &rcCur);
    int curW = rcCur.right - rcCur.left;
    int curH = rcCur.bottom - rcCur.top;
    if (oldHeight == state->totalHeight && curW == dlgW && curH == state->totalHeight) return;

    if (curW != dlgW) {
        state->normalX += (curW - dlgW) / 2;
    }
    if (!state->anchoredTop) {
        state->normalY += (oldHeight - state->totalHeight);
    }
    ClampDialogToMonitorWorkArea(state->hDlg, &state->normalX, &state->normalY, dlgW, state->totalHeight, state->dpi);
    if (!IsIconic(state->hDlg)) {
        int targetY = state->normalY;
        if (state->openAnimActive) {
            float t = std::clamp((float)(GetTickCount64() - state->openAnimStartTick) / 150.0f, 0.0f, 1.0f);
            float ease = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
            targetY += (int)std::round((1.0f - ease) * DpiScaleS(14, state) * (state->anchoredTop ? -1.0f : 1.0f));
        }
        SetWindowPos(state->hDlg, nullptr, state->normalX, targetY, dlgW, state->totalHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void ForceFocusModernEdit(HWND hDlg, RunDialogState* state) {
    if (!hDlg || !state || !state->hModernEdit || !IsWindow(state->hModernEdit)) return;
    if (state->isBrowsingFile || g_hActiveGearMenu.load() != nullptr || g_hActiveCustomSizeWnd.load() != nullptr) return;

    HWND hFg = GetForegroundWindow();
    if (hFg != hDlg) {
        DWORD fgThread = hFg ? GetWindowThreadProcessId(hFg, nullptr) : 0;
        DWORD curThread = GetCurrentThreadId();
        bool attached = false;
        if (fgThread != 0 && fgThread != curThread) {
            attached = (AttachThreadInput(fgThread, curThread, TRUE) != FALSE);
        }
        g_internalActivation = true;
        if (SwitchToThisWindow_Orig) {
            SwitchToThisWindow_Orig(hDlg, TRUE);
        }
        if (SetForegroundWindow_Orig) {
            SetForegroundWindow_Orig(hDlg);
        } else {
            SetForegroundWindow(hDlg);
        }
        SetActiveWindow(hDlg);
        g_internalActivation = false;
        if (attached) {
            AttachThreadInput(fgThread, curThread, FALSE);
        }
    } else if (GetActiveWindow() != hDlg) {
        SetActiveWindow(hDlg);
    }

    if (GetFocus() != state->hModernEdit) {
        SendMessageW(hDlg, WM_NEXTDLGCTL, reinterpret_cast<WPARAM>(state->hModernEdit), TRUE);
        SetFocus(state->hModernEdit);
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

    if (!IsIconic(state->hDlg) && !state->openAnimActive && !state->closingAnimActive) {
        RECT rc = {};
        GetWindowRect(state->hDlg, &rc);
        if (rc.left > -10000 && rc.top > -10000) {
            state->normalX = rc.left;
            state->normalY = rc.top;
        }
    }

    PlayDialogCloseAnimationSync(state->hDlg, state);

    g_internalActivation = true;
    if (ShowWindow_Orig) {
        ShowWindow_Orig(state->hDlg, SW_MINIMIZE);
    } else {
        ShowWindow(state->hDlg, SW_MINIMIZE);
    }
    g_internalActivation = false;
    state->animAlpha = 1.0f;
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

    StartDialogOpenAnimation(state);
    int startSlideOff = DpiScaleS(14, state) * (state->anchoredTop ? -1 : 1);

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
                 state->normalX, state->normalY + startSlideOff,
                 dlgW, state->totalHeight,
                 SWP_SHOWWINDOW | SWP_FRAMECHANGED);
    SetWindowPos(state->hDlg, HWND_NOTOPMOST,
                 state->normalX, state->normalY + startSlideOff,
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

    g_internalActivation = false;
    g_dlgIsForeground.store(true);
    g_deactivatedByTaskbarTick.store(0);

    if (state->hModernEdit) {
        SetFocus(state->hModernEdit);
        SendMessageW(state->hModernEdit, EM_SETSEL, 0, -1);
    }
    state->caretVisible = true;
    InvalidateRect(state->hDlg, nullptr, FALSE);
}

void OpenBrowseExecutableDialog(RunDialogState* state, bool autoExecute = false) {
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

        if (autoExecute) {
            ExecuteCommandImmediate(state, formattedPath);
            return;
        }
    }

    if (IsWindow(hDlg)) {
        if (state->hModernEdit && IsWindow(state->hModernEdit)) {
            SetFocus(state->hModernEdit);
        }
        state->caretVisible = true;
        InvalidateRect(hDlg, nullptr, FALSE);
    }
}

void LoadHistoryItems(RunDialogState* state);
ActionButtonsLayout GetActionButtonsLayout(const RunDialogState* state);


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
    if (GetWindowThreadProcessId(hDlg, nullptr) == GetCurrentThreadId()) {
        auto* state = reinterpret_cast<RunDialogState*>(GetPropW(hDlg, L"WindhawkModernRunState"));
        if (state && !state->pendingUninit && !g_unloading.load()) {
            PlayDialogCloseAnimationSync(hDlg, state);
        }
    }
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

std::wstring GetEffectiveInputText(HWND hEdit, DWORD* outSelStart, DWORD* outSelEnd) {
    if (!hEdit || !IsWindow(hEdit)) {
        if (outSelStart) *outSelStart = 0;
        if (outSelEnd)   *outSelEnd   = 0;
        return L"";
    }

    DWORD selStart = 0, selEnd = 0;
    SendMessageW(hEdit, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);

    int textLen = GetWindowTextLengthW(hEdit);
    std::wstring text;
    if (textLen > 0) {
        text.resize((size_t)textLen + 1, L'\0');
        int copied = GetWindowTextW(hEdit, text.data(), textLen + 1);
        text.resize(copied > 0 ? (size_t)copied : 0);
    }

    // Strip any trailing CR/LF from single-line RichEdit buffer
    while (!text.empty() && (text.back() == L'\r' || text.back() == L'\n')) {
        text.pop_back();
    }

    selStart = (std::min)(selStart, (DWORD)text.size());
    selEnd   = (std::min)(selEnd,   (DWORD)text.size());

    if (outSelStart) *outSelStart = selStart;
    if (outSelEnd)   *outSelEnd   = selEnd;
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

// --- Multi-Language Translate Mode Engine (22 Languages + Async WinHTTP) ---

struct TranslateLangEntry {
    UINT menuId;
    const wchar_t* code;
    const wchar_t* labelVi;
    const wchar_t* labelEn;
};

static constexpr TranslateLangEntry kTranslateLangs[] = {
    { 800, L"auto",  L"Phát hiện ngôn ngữ",       L"Auto Detect" },
    { 801, L"vi",    L"Tiếng Việt",               L"Vietnamese (Tiếng Việt)" },
    { 802, L"en",    L"Tiếng Anh (English)",      L"English" },
    { 803, L"zh-CN", L"Tiếng Trung (Giản thể)",   L"Chinese Simplified (简体中文)" },
    { 804, L"zh-TW", L"Tiếng Trung (Phồn thể)",   L"Chinese Traditional (繁體中文)" },
    { 805, L"ja",    L"Tiếng Nhật (日本語)",       L"Japanese (日本語)" },
    { 806, L"ko",    L"Tiếng Hàn (한국어)",        L"Korean (한국어)" },
    { 807, L"fr",    L"Tiếng Pháp (Français)",    L"French (Français)" },
    { 808, L"de",    L"Tiếng Đức (Deutsch)",      L"German (Deutsch)" },
    { 809, L"es",    L"Tiếng Tây Ban Nha",        L"Spanish (Español)" },
    { 810, L"ru",    L"Tiếng Nga (Русский)",      L"Russian (Русский)" },
    { 811, L"th",    L"Tiếng Thái (ไทย)",         L"Thai (ไทย)" },
    { 812, L"id",    L"Tiếng Indonesia",          L"Indonesian (Bahasa Indonesia)" },
    { 813, L"ms",    L"Tiếng Mã Lai",             L"Malay (Bahasa Melayu)" },
    { 814, L"pt",    L"Tiếng Bồ Đào Nha",         L"Portuguese (Português)" },
    { 815, L"it",    L"Tiếng Ý (Italiano)",       L"Italian (Italiano)" },
    { 816, L"uk",    L"Tiếng Ukraina",            L"Ukrainian (Українська)" },
    { 817, L"pl",    L"Tiếng Ba Lan (Polski)",    L"Polish (Polski)" },
    { 818, L"nl",    L"Tiếng Hà Lan",             L"Dutch (Nederlands)" },
    { 819, L"tr",    L"Tiếng Thổ Nhĩ Kỳ",         L"Turkish (Türkçe)" },
    { 820, L"hi",    L"Tiếng Hindi (हिन्दी)",       L"Hindi (हिन्दी)" },
    { 821, L"ar",    L"Tiếng Ả Rập (العربية)",     L"Arabic (العربية)" }
};

bool IsDialogVietnameseUI(const RunDialogState* state) {
    if (!state) return false;
    if (state->settings.language == L"vi") return true;
    if (state->settings.language == L"auto") {
        return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_VIETNAMESE;
    }
    return false;
}

const wchar_t* GetTranslateLangDisplayName(const std::wstring& code, bool isVi) {
    for (const auto& entry : kTranslateLangs) {
        if (_wcsicmp(code.c_str(), entry.code) == 0) {
            return isVi ? entry.labelVi : entry.labelEn;
        }
    }
    return isVi ? L"Tiếng Việt" : L"English";
}

std::wstring GetCompactTranslateLangDisplayName(const std::wstring& code, bool isVi) {
    if (_wcsicmp(code.c_str(), L"auto") == 0) {
        return isVi ? L"Tự động" : L"Auto";
    }
    std::wstring full = GetTranslateLangDisplayName(code, isVi);
    size_t parenPos = full.find(L" (");
    if (parenPos != std::wstring::npos && parenPos > 0) {
        return full.substr(0, parenPos);
    }
    return full;
}

static std::mutex g_translateMutex;
static std::atomic<uint64_t> g_translateReqCounter{0};
static std::atomic<int> g_activeTranslateThreads{0};
static std::wstring g_pendingTranslateResult;

std::string ParseGoogleTranslateGtxJson(const std::string& json) {
    size_t pos = 0;
    auto skipWs = [&]() {
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\r' || json[pos] == '\n')) ++pos;
    };
    auto parseJsonString = [&](std::string& out) -> bool {
        skipWs();
        if (pos >= json.size() || json[pos] != '"') return false;
        ++pos;
        out.clear();
        while (pos < json.size()) {
            char c = json[pos++];
            if (c == '"') return true;
            if (c == '\\' && pos < json.size()) {
                char esc = json[pos++];
                switch (esc) {
                    case '"':  out.push_back('"'); break;
                    case '\\': out.push_back('\\'); break;
                    case '/':  out.push_back('/'); break;
                    case 'b':  out.push_back('\b'); break;
                    case 'f':  out.push_back('\f'); break;
                    case 'n':  out.push_back('\n'); break;
                    case 'r':  out.push_back('\r'); break;
                    case 't':  out.push_back('\t'); break;
                    case 'u': {
                        if (pos + 4 <= json.size()) {
                            unsigned int cp = 0;
                            for (int i = 0; i < 4; ++i) {
                                char hc = json[pos + i];
                                cp <<= 4;
                                if (hc >= '0' && hc <= '9') cp |= (hc - '0');
                                else if (hc >= 'a' && hc <= 'f') cp |= (hc - 'a' + 10);
                                else if (hc >= 'A' && hc <= 'F') cp |= (hc - 'A' + 10);
                            }
                            pos += 4;
                            if (cp < 0x80) {
                                out.push_back((char)cp);
                            } else if (cp < 0x800) {
                                out.push_back((char)(0xC0 | (cp >> 6)));
                                out.push_back((char)(0x80 | (cp & 0x3F)));
                            } else {
                                out.push_back((char)(0xE0 | (cp >> 12)));
                                out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
                                out.push_back((char)(0x80 | (cp & 0x3F)));
                            }
                        }
                        break;
                    }
                    default: out.push_back(esc); break;
                }
            } else {
                out.push_back(c);
            }
        }
        return false;
    };

    skipWs();
    if (pos >= json.size() || json[pos] != '[') return "";
    ++pos;
    skipWs();
    if (pos >= json.size() || json[pos] != '[') return "";
    ++pos;

    std::string fullResult;
    while (pos < json.size()) {
        skipWs();
        if (pos >= json.size() || json[pos] == ']') break;
        if (json[pos] == ',') { ++pos; continue; }
        if (json[pos] != '[') break;
        ++pos;
        std::string segTrans;
        if (parseJsonString(segTrans)) {
            fullResult += segTrans;
        }
        int depth = 1;
        while (pos < json.size() && depth > 0) {
            if (json[pos] == '"') {
                std::string dummy;
                parseJsonString(dummy);
                continue;
            }
            if (json[pos] == '[') ++depth;
            else if (json[pos] == ']') --depth;
            ++pos;
        }
    }
    return fullResult;
}

bool FetchGoogleTranslateViaWinHttp(const std::wstring& text, const std::wstring& srcLang,
                                    const std::wstring& dstLang, std::wstring* outTranslated) {
    if (!outTranslated) return false;
    outTranslated->clear();

    HMODULE hWinHttp = LoadLibraryW(L"winhttp.dll");
    if (!hWinHttp) return false;

    using HINTERNET_WH = void*;
    using WinHttpOpen_t = HINTERNET_WH(WINAPI*)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
    using WinHttpConnect_t = HINTERNET_WH(WINAPI*)(HINTERNET_WH, LPCWSTR, WORD, DWORD);
    using WinHttpOpenRequest_t = HINTERNET_WH(WINAPI*)(HINTERNET_WH, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR*, DWORD);
    using WinHttpSendRequest_t = BOOL(WINAPI*)(HINTERNET_WH, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR);
    using WinHttpReceiveResponse_t = BOOL(WINAPI*)(HINTERNET_WH, LPVOID);
    using WinHttpReadData_t = BOOL(WINAPI*)(HINTERNET_WH, LPVOID, DWORD, LPDWORD);
    using WinHttpCloseHandle_t = BOOL(WINAPI*)(HINTERNET_WH);

    auto pOpen = reinterpret_cast<WinHttpOpen_t>(GetProcAddress(hWinHttp, "WinHttpOpen"));
    auto pConnect = reinterpret_cast<WinHttpConnect_t>(GetProcAddress(hWinHttp, "WinHttpConnect"));
    auto pOpenReq = reinterpret_cast<WinHttpOpenRequest_t>(GetProcAddress(hWinHttp, "WinHttpOpenRequest"));
    auto pSendReq = reinterpret_cast<WinHttpSendRequest_t>(GetProcAddress(hWinHttp, "WinHttpSendRequest"));
    auto pRecvResp = reinterpret_cast<WinHttpReceiveResponse_t>(GetProcAddress(hWinHttp, "WinHttpReceiveResponse"));
    auto pReadData = reinterpret_cast<WinHttpReadData_t>(GetProcAddress(hWinHttp, "WinHttpReadData"));
    auto pClose = reinterpret_cast<WinHttpCloseHandle_t>(GetProcAddress(hWinHttp, "WinHttpCloseHandle"));

    bool ok = false;
    if (pOpen && pConnect && pOpenReq && pSendReq && pRecvResp && pReadData && pClose) {
        std::wstring encodedQ = UrlEncodeUtf8(text);
        std::wstring path = L"/translate_a/single?client=gtx&sl=" + srcLang +
                            L"&tl=" + dstLang + L"&dt=t&q=" + encodedQ;

        HINTERNET_WH hSession = pOpen(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64)", 0, nullptr, nullptr, 0);
        if (hSession) {
            HINTERNET_WH hConn = pConnect(hSession, L"translate.googleapis.com", 443, 0);
            if (hConn) {
                HINTERNET_WH hReq = pOpenReq(hConn, L"GET", path.c_str(), nullptr, nullptr, nullptr, 0x00800000 /*WINHTTP_FLAG_SECURE*/);
                if (hReq) {
                    if (pSendReq(hReq, nullptr, 0, nullptr, 0, 0, 0) && pRecvResp(hReq, nullptr)) {
                        std::string respBody;
                        char buf[4096];
                        DWORD dwRead = 0;
                        while (pReadData(hReq, buf, sizeof(buf), &dwRead) && dwRead > 0) {
                            respBody.append(buf, dwRead);
                            dwRead = 0;
                        }
                        std::string utf8Trans = ParseGoogleTranslateGtxJson(respBody);
                        if (!utf8Trans.empty()) {
                            int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8Trans.data(), (int)utf8Trans.size(), nullptr, 0);
                            if (wlen > 0) {
                                outTranslated->resize(wlen);
                                MultiByteToWideChar(CP_UTF8, 0, utf8Trans.data(), (int)utf8Trans.size(), outTranslated->data(), wlen);
                                ok = true;
                            }
                        }
                    }
                    pClose(hReq);
                }
                pClose(hConn);
            }
            pClose(hSession);
        }
    }
    FreeLibrary(hWinHttp);
    return ok;
}

void TriggerTranslationRequest(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    std::wstring rawText = TrimCommandString(GetEffectiveInputText(state->hModernEdit));
    if (rawText.empty()) {
        int oldH = state->totalHeight;
        state->isTranslating = false;
        state->translatedText.clear();
        RecomputeDialogMetrics(state);
        ResizeDialogPreservingBottomAnchor(state, oldH);
        InvalidateRect(state->hDlg, nullptr, FALSE);
        return;
    }

    int oldH = state->totalHeight;
    state->isTranslating = true;
    state->translateCopiedFeedback = false;
    uint64_t reqId = ++g_translateReqCounter;
    RecomputeDialogMetrics(state);
    ResizeDialogPreservingBottomAnchor(state, oldH);
    InvalidateRect(state->hDlg, nullptr, FALSE);
    UpdateWindow(state->hDlg);

    HWND hTargetDlg = state->hDlg;
    std::wstring srcLang = state->translateSourceLang;
    std::wstring dstLang = state->translateTargetLang;
    bool isVi = IsDialogVietnameseUI(state);

    g_activeTranslateThreads.fetch_add(1);
    std::thread([hTargetDlg, rawText, srcLang, dstLang, reqId, isVi]() {
        struct ThreadGuard { ~ThreadGuard() { g_activeTranslateThreads.fetch_sub(1); } } guard;
        std::wstring result;
        bool ok = FetchGoogleTranslateViaWinHttp(rawText, srcLang, dstLang, &result);
        if (!ok || result.empty()) {
            result = isVi
                ? L"[Không thể kết nối dịch tự động. Kiểm tra mạng Internet và thử lại.]"
                : L"[Could not reach translation service. Check your connection and try again.]";
        }
        if (g_unloading.load()) return;
        {
            std::lock_guard<std::mutex> lock(g_translateMutex);
            if (reqId == g_translateReqCounter.load()) {
                g_pendingTranslateResult = std::move(result);
            }
        }
        if (IsWindow(hTargetDlg) && !g_unloading.load()) {
            PostMessageW(hTargetDlg, g_msgToggle, kActionTranslateDone, (LPARAM)reqId);
        }
    }).detach();
}

// --- Experimental Instant Math Calculator in Run Dialog ---

struct MathExprParser {
    std::wstring_view src;
    size_t pos = 0;
    bool error = false;
    bool hasOperatorOrFunc = false;

    void skipSpaces() {
        while (pos < src.size() && (src[pos] == L' ' || src[pos] == L'\t')) ++pos;
    }

    double parseExpression() {
        double val = parseTerm();
        while (!error) {
            skipSpaces();
            if (pos < src.size() && (src[pos] == L'+' || src[pos] == L'-' || src[pos] == L'−')) {
                wchar_t op = src[pos++];
                hasOperatorOrFunc = true;
                double rhs = parseTerm();
                if (op == L'+') val += rhs;
                else val -= rhs;
            } else {
                break;
            }
        }
        return val;
    }

    double parseTerm() {
        double val = parsePower();
        while (!error) {
            skipSpaces();
            if (pos < src.size()) {
                wchar_t ch = src[pos];
                if (ch == L'*' || ch == L'×' || ch == L'/' || ch == L'÷' || ch == L'%') {
                    ++pos;
                    hasOperatorOrFunc = true;
                    double rhs = parsePower();
                    if (ch == L'*' || ch == L'×') {
                        val *= rhs;
                    } else if (ch == L'/' || ch == L'÷') {
                        if (std::fabs(rhs) < 1e-15) { error = true; return 0.0; }
                        val /= rhs;
                    } else {
                        if (std::fabs(rhs) < 1e-15) { error = true; return 0.0; }
                        val = std::fmod(val, rhs);
                    }
                    continue;
                }
                // Implicit multiplication: e.g. 2(3+4) or (1+2)(3+4)
                if (ch == L'(') {
                    hasOperatorOrFunc = true;
                    double rhs = parsePower();
                    val *= rhs;
                    continue;
                }
            }
            break;
        }
        return val;
    }

    double parsePower() {
        double base = parseUnary();
        skipSpaces();
        if (!error && pos < src.size() && src[pos] == L'^') {
            ++pos;
            hasOperatorOrFunc = true;
            double expVal = parsePower(); // right-associative
            base = std::pow(base, expVal);
        }
        return base;
    }

    double parseUnary() {
        skipSpaces();
        if (pos < src.size() && (src[pos] == L'+' || src[pos] == L'-' || src[pos] == L'−')) {
            wchar_t op = src[pos++];
            if (op == L'-' || op == L'−') {
                return -parseUnary();
            }
            return parseUnary();
        }
        return parsePrimary();
    }

    double parsePrimary() {
        skipSpaces();
        if (pos >= src.size()) {
            error = true;
            return 0.0;
        }

        if (src[pos] == L'(') {
            ++pos;
            double val = parseExpression();
            skipSpaces();
            if (pos >= src.size() || src[pos] != L')') {
                error = true;
                return 0.0;
            }
            ++pos;
            return val;
        }

        // Parse identifier (math function or constant like sqrt, sin, cos, tan, abs, log, ln, pi, e)
        if ((src[pos] >= L'a' && src[pos] <= L'z') || (src[pos] >= L'A' && src[pos] <= L'Z')) {
            size_t idStart = pos;
            while (pos < src.size() && ((src[pos] >= L'a' && src[pos] <= L'z') ||
                                        (src[pos] >= L'A' && src[pos] <= L'Z') ||
                                        (src[pos] >= L'0' && src[pos] <= L'9'))) {
                ++pos;
            }
            std::wstring ident = ToLowerString(std::wstring(src.substr(idStart, pos - idStart)));
            skipSpaces();
            if (ident == L"pi") return 3.14159265358979323846;
            if (ident == L"e")  return 2.71828182845904523536;

            if (pos >= src.size() || src[pos] != L'(') {
                error = true;
                return 0.0;
            }
            ++pos;
            hasOperatorOrFunc = true;
            double arg = parseExpression();
            skipSpaces();
            if (pos >= src.size() || src[pos] != L')') {
                error = true;
                return 0.0;
            }
            ++pos;

            if (ident == L"sqrt") {
                if (arg < 0.0) { error = true; return 0.0; }
                return std::sqrt(arg);
            }
            if (ident == L"abs")   return std::fabs(arg);
            if (ident == L"sin")   return std::sin(arg);
            if (ident == L"cos")   return std::cos(arg);
            if (ident == L"tan")   return std::tan(arg);
            if (ident == L"log" || ident == L"log10") {
                if (arg <= 0.0) { error = true; return 0.0; }
                return std::log10(arg);
            }
            if (ident == L"ln") {
                if (arg <= 0.0) { error = true; return 0.0; }
                return std::log(arg);
            }
            if (ident == L"round") return std::round(arg);
            if (ident == L"floor") return std::floor(arg);
            if (ident == L"ceil")  return std::ceil(arg);

            error = true;
            return 0.0;
        }

        // Parse number (supports '.' or ',' as decimal separator, max 1 decimal point)
        if ((src[pos] >= L'0' && src[pos] <= L'9') || src[pos] == L'.' || src[pos] == L',') {
            std::wstring numBuf;
            bool seenDot = false;
            while (pos < src.size()) {
                wchar_t c = src[pos];
                if (c >= L'0' && c <= L'9') {
                    numBuf.push_back(c);
                    ++pos;
                } else if ((c == L'.' || c == L',') && !seenDot) {
                    seenDot = true;
                    numBuf.push_back(L'.');
                    ++pos;
                } else {
                    break;
                }
            }
            if (numBuf.empty() || numBuf == L".") {
                error = true;
                return 0.0;
            }
            wchar_t* endPtr = nullptr;
            double val = std::wcstod(numBuf.c_str(), &endPtr);
            if (!endPtr || *endPtr != L'\0') {
                error = true;
                return 0.0;
            }
            return val;
        }

        error = true;
        return 0.0;
    }
};

bool TryEvaluateMathExpression(std::wstring_view rawInput, std::wstring* outResultStr, std::wstring* outCleanExpr = nullptr) {
    std::wstring s = TrimCommandString(std::wstring(rawInput));
    if (s.empty() || s.size() > 128) return false;

    // Exclude file paths, URLs, environment vars, or drive letters
    if (s.find(L'\\') != std::wstring::npos || s.find(L':') != std::wstring::npos ||
        s.find(L'"') != std::wstring::npos) {
        return false;
    }

    bool hadExplicitEquals = false;
    if (!s.empty() && s.front() == L'=') {
        hadExplicitEquals = true;
        s = TrimCommandString(s.substr(1));
    }
    if (!s.empty() && s.back() == L'=') {
        hadExplicitEquals = true;
        s = TrimCommandString(s.substr(0, s.size() - 1));
    }
    if (s.empty()) return false;

    MathExprParser parser{ s, 0, false, hadExplicitEquals };
    double result = parser.parseExpression();
    parser.skipSpaces();
    if (parser.error || parser.pos != s.size() || !std::isfinite(result)) {
        return false;
    }

    // Also allow negative+positive combinations like "-5+10" where unary '-' was parsed first
    if (!parser.hasOperatorOrFunc) {
        for (size_t i = 1; i < s.size(); ++i) {
            wchar_t c = s[i];
            if (c == L'+' || c == L'-' || c == L'−' || c == L'*' || c == L'×' ||
                c == L'/' || c == L'÷' || c == L'^' || c == L'%') {
                parser.hasOperatorOrFunc = true;
                break;
            }
        }
    }
    if (!parser.hasOperatorOrFunc) {
        return false;
    }

    if (std::fabs(result) < 1e-12) result = 0.0;

    wchar_t buf[64] = {};
    double rounded = std::round(result);
    if (std::fabs(result - rounded) < 1e-9 && std::fabs(rounded) < 9.0e15) {
        swprintf_s(buf, L"%lld", static_cast<long long>(rounded));
    } else {
        swprintf_s(buf, L"%.10g", result);
    }

    if (outResultStr) *outResultStr = buf;
    if (outCleanExpr) *outCleanExpr = s;
    return true;
}

void CopyTextToClipboard(HWND hOwner, const std::wstring& text) {
    if (text.empty()) return;
    if (!OpenClipboard(hOwner)) return;
    EmptyClipboard();
    size_t cb = (text.size() + 1) * sizeof(wchar_t);
    if (HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, cb)) {
        if (void* ptr = GlobalLock(hMem)) {
            memcpy(ptr, text.c_str(), cb);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        } else {
            GlobalFree(hMem);
        }
    }
    CloseClipboard();
}

void ApplyMathResultToDialog(RunDialogState* state, const std::wstring& resultValue) {
    if (!state || resultValue.empty()) return;
    CopyTextToClipboard(state->hDlg, resultValue);
    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
        SetWindowTextW(state->hModernEdit, resultValue.c_str());
        int len = (int)resultValue.size();
        SendMessageW(state->hModernEdit, EM_SETSEL, len, len);
        SetFocus(state->hModernEdit);
    }
    SyncInputToNativeCombo(state, resultValue.c_str());
    UpdateAppSuggestionsForQuery(state, resultValue);
    if (state->hDlg) {
        InvalidateRect(state->hDlg, nullptr, FALSE);
    }
}

// Parses "google:<query>" (unified syntax, plus backward-compatible aliases)
int ParseWebSearchCommand(std::wstring_view rawCmd, std::wstring* outQuery) {
    std::wstring trimmed = TrimCommandString(std::wstring(rawCmd));
    std::wstring lower = ToLowerString(trimmed);

    if (lower.rfind(L"google:", 0) == 0) {
        if (outQuery) {
            *outQuery = TrimCommandString(trimmed.substr(7));
        }
        return 1;
    }
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
        return 1;
    }
    return 0;
}

std::wstring GetDefaultBrowserExecutablePath() {
    wchar_t exePath[MAX_PATH] = {};
    DWORD cch = ARRAYSIZE(exePath);
    if (SUCCEEDED(AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_EXECUTABLE, L"https", L"open", exePath, &cch)) &&
        exePath[0] != L'\0' && PathFileExistsW(exePath)) {
        return exePath;
    }
    cch = ARRAYSIZE(exePath);
    if (SUCCEEDED(AssocQueryStringW(ASSOCF_NONE, ASSOCSTR_EXECUTABLE, L".htm", L"open", exePath, &cch)) &&
        exePath[0] != L'\0' && PathFileExistsW(exePath)) {
        return exePath;
    }
    return L"msedge.exe";
}

bool LaunchWebSearchCommand(HWND, int, const std::wstring& query) {
    std::wstring encoded = UrlEncodeUtf8(query);
    std::wstring url = encoded.empty()
        ? L"https://www.google.com"
        : (L"https://www.google.com/search?q=" + encoded);

    // Open directly in the user's default system browser
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_DOENVSUBST;
    sei.hwnd = nullptr;
    sei.lpVerb = L"open";
    sei.lpFile = url.c_str();
    sei.nShow = SW_SHOWNORMAL;
    return ShellExecuteExW(&sei) != FALSE;
}

void ExecuteHistoryOrSuggestionItem(RunDialogState* state, const HistoryItem& item) {
    if (!state || !state->hDlg || state->isTriggeringRun) return;
    CloseActiveGearMenu();

    if (item.isMathResult && !item.mathResultValue.empty()) {
        ApplyMathResultToDialog(state, item.mathResultValue);
        return;
    }
    std::wstring directMathRes;
    if (TryEvaluateMathExpression(!item.execTarget.empty() ? item.execTarget : item.cmd, &directMathRes)) {
        ApplyMathResultToDialog(state, directMathRes);
        return;
    }

    // Handle google:<keyword> (Google Chrome) and msesearch:<keyword> (Microsoft Edge)
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

    if (state->hCombo) EnableWindow(state->hCombo, TRUE);
    if (state->hOrigEdit) EnableWindow(state->hOrigEdit, TRUE);
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

    if (state->translateMode) {
        TriggerTranslationRequest(state);
        return;
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

    BOOL dark = state->isLightMode ? FALSE : TRUE;
    DwmSetWindowAttribute(hDlg, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    // Disable Windows 11 rectangular DWM frame/shadow so our Pill Input Bar (Image 1) and
    // Detached Squircle Card (Image 2) have 100% transparent outer corners and separation gap!
    DWORD corner = 1; // DWMWCP_DONOTROUND
    DwmSetWindowAttribute(hDlg, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    COLORREF borderCol = 0xFFFFFFFE; // DWMWA_COLOR_NONE
    DwmSetWindowAttribute(hDlg, DWMWA_BORDER_COLOR, &borderCol, sizeof(borderCol));

    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hDlg, &margins);

    HRGN hEmptyRgn = CreateRectRgn(0, 0, -1, -1);
    DWM_BLURBEHIND bb = {};
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.fEnable = TRUE;
    bb.hRgnBlur = hEmptyRgn;
    DwmEnableBlurBehindWindow(hDlg, &bb);
    if (hEmptyRgn) DeleteObject(hEmptyRgn);

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return;

    auto pSetWCA = reinterpret_cast<SetWindowCompositionAttribute_t>(
        GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
    if (!pSetWCA) return;

    ACCENT_POLICY policy = {};
    policy.AccentState = ACCENT_DISABLED;

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
    if (state->effectiveSurface != SurfaceTheme::Glass && state->effectiveSurface != SurfaceTheme::Blur) return;
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
    { L"google:",          L"Google Search (Default Browser)" },
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
        std::wstring browserCmd = GetDefaultBrowserExecutablePath();
        std::wstring dummyName;
        ResolveCommandMetadata(browserCmd, outIcon, &dummyName);
        *outAppName = webQuery.empty() ? L"Google Search" : (L"Google: " + webQuery);
        return;
    }

    wchar_t cmdOnly[MAX_PATH] = {};
    wcsncpy_s(cmdOnly, rawCmd.c_str(), _TRUNCATE);
    PathRemoveBlanksW(cmdOnly);
    PathRemoveArgsW(cmdOnly);
    PathUnquoteSpacesW(cmdOnly);

    if (cmdOnly[0] == L'\0') return;

    // Never invoke Win32 filesystem/SMB path APIs on '//' or '\\' or purely special characters!
    bool hasAlnum = false;
    for (const wchar_t* p = cmdOnly; *p; ++p) {
        if (iswalnum(*p)) {
            hasAlnum = true;
            break;
        }
    }
    if (!hasAlnum ||
        (cmdOnly[0] == L'/' && cmdOnly[1] == L'/') ||
        (cmdOnly[0] == L'\\' && cmdOnly[1] == L'\\')) {
        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW(L".exe", FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),
                           SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_LARGEICON)) {
            *outIcon = sfi.hIcon;
        }
        *outAppName = cmdOnly;
        return;
    }

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
    // In Compact Mini-Pill mode (minimalMode), App Suggestions is ALWAYS enabled by default!
    bool appSugEnabled = state->settings.minimalMode ? true : state->settings.enableAppSuggestions;
    int configuredMax  = state->settings.minimalMode
        ? (state->settings.maxHistoryItems > 0 ? state->settings.maxHistoryItems : 20)
        : state->settings.maxHistoryItems;

    std::wstring mathResultStr, mathCleanExpr;
    bool hasMathResult = TryEvaluateMathExpression(trimmed, &mathResultStr, &mathCleanExpr);
    int maxItems = (std::max)(hasMathResult ? 1 : 0, configuredMax);

    if ((!appSugEnabled && !hasMathResult) || maxItems <= 0 || trimmed.empty()) {
        state->lastSuggestedQuery.clear();
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

    // Instant 0ms fast-path when pressing Space (trimmed query did not change!)
    if (state->showingSuggestions && trimmed == state->lastSuggestedQuery) {
        return;
    }
    state->lastSuggestedQuery = trimmed;

    // Fast-path guard for purely special characters (e.g. "/", "//", "\\", "...") that are not math expressions:
    // Never scan catalog, extract icons, or resize window when input contains zero alphanumeric characters!
    bool hasQueryAlnum = false;
    for (wchar_t ch : trimmed) {
        if (iswalnum(ch)) {
            hasQueryAlnum = true;
            break;
        }
    }
    if (!hasQueryAlnum && !hasMathResult) {
        if (!state->showingSuggestions || !state->suggestions.empty()) {
            int oldHeight = state->totalHeight;
            state->showingSuggestions = true;
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

    // 0a. Instant Math Calculator result (e.g. "1+1" -> "2")
    if (hasMathResult) {
        HistoryItem mathItem;
        mathItem.cmd = mathResultStr;
        mathItem.execTarget = mathResultStr;
        mathItem.appName = mathCleanExpr + L" = " + mathResultStr;
        mathItem.isSuggestion = true;
        mathItem.ownsIcon = false;
        mathItem.isMathResult = true;
        mathItem.mathResultValue = mathResultStr;

        {
            std::lock_guard<std::mutex> lock(g_catalogMutex);
            for (auto& cand : g_appCatalog) {
                if (cand.cmdLower == L"calc") {
                    LazilyResolveCandidateMetadata(cand, state->i18n.appTag);
                    mathItem.hIcon = cand.cachedIcon;
                    break;
                }
            }
        }
        matches.push_back({ 5000, std::move(mathItem) });
    }

    // 0b. Instant Web Search command detection (google:<keyword>)
    std::wstring webSearchKeyword;
    int webSearchKind = ParseWebSearchCommand(trimmed, &webSearchKeyword);
    if (webSearchKind != 0) {
        MenuLocalizedStrings ml = GetMenuLocalizedStrings(state->settings.language);
        HistoryItem webItem;
        webItem.cmd = trimmed;
        webItem.execTarget = trimmed;
        webItem.appName = webSearchKeyword.empty()
            ? ml.ggSearchEmptyHint
            : (std::wstring(ml.ggSearchQueryPrefix) + webSearchKeyword + L"\"");
        webItem.isSuggestion = true;
        webItem.ownsIcon = false;
        webItem.webSearchKind = 1;
        webItem.webSearchQuery = webSearchKeyword;

        {
            std::lock_guard<std::mutex> lock(g_catalogMutex);
            for (auto& cand : g_appCatalog) {
                if (cand.cmdLower == L"google:") {
                    if (!cand.iconResolved) {
                        cand.iconSource = GetDefaultBrowserExecutablePath();
                    }
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

    // Lazily resolve icons only for the top visible rows (<= visibleHistoryRows) for 0ms typing!
    {
        std::lock_guard<std::mutex> lock(g_catalogMutex);
        int visLimit = std::clamp(state->settings.visibleHistoryRows, 3, 10);
        int resolvedCount = 0;
        for (auto& m : matches) {
            if (resolvedCount++ >= visLimit) break;
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
                GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
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

int GetDetachedGap(const RunDialogState* state) {
    return DpiScaleS(kBaseDetachedGap, state);
}

int GetHistoryCardHeight(const RunDialogState* state) {
    if (!state || state->topAreaHeight <= 0) return 0;
    return (std::max)(0, state->topAreaHeight - GetDetachedGap(state));
}

// When anchoredTop == true (taskbar on Left, Right, or Top), the Pill Input Bar stays fixed at the TOP (y = 0)
// and the Detached Card (History / Suggestions / Translation Result) expands BELOW it (y = barH + gapH).
// When anchoredTop == false (taskbar at Bottom), the Pill Input Bar stays fixed at the BOTTOM (y = topAreaHeight)
// and the Detached Card expands ABOVE it (y = 0).
int GetInputBarTopY(const RunDialogState* state) {
    if (!state) return 0;
    return state->anchoredTop ? 0 : state->topAreaHeight;
}

int GetDetachedCardTopY(const RunDialogState* state) {
    if (!state || state->topAreaHeight <= 0) return 0;
    return state->anchoredTop ? (GetInputBarHeight(state) + GetDetachedGap(state)) : 0;
}

ActionButtonsLayout GetActionButtonsLayout(const RunDialogState* state) {
    int dlgW = GetDlgWidth(state);
    int barH = GetInputBarHeight(state);
    UINT dpi = state ? state->dpi : 96;
    int btnH = DpiScaleS(32, state);
    if (state && !state->translateMode && state->settings.customBarHeight > 0) {
        btnH = std::clamp(barH - DpiScale(18, dpi, 100), DpiScale(24, dpi, 100), DpiScale(46, dpi, 100));
    }

    int gap = DpiScaleS(6, state);
    int singleLineBarH = DpiScaleS(kBaseInputBarHeight, state);
    int rightMargin = (std::max)(DpiScaleS(10, state), (singleLineBarH - btnH) / 2);

    float fontScale = (state && state->settings.customFontSize > 0)
                      ? (std::clamp(state->settings.customFontSize, 9, 24) / 13.5f)
                      : (GetDlgScale(state) / 100.0f);
    auto scaleBtnW = [&](int base96W) -> int {
        return (int)std::ceil(base96W * (dpi / 96.0f) * std::clamp(fontScale, 0.80f, 1.45f));
    };

    const wchar_t* runLabel = state ? state->i18n.runBtn : L"Run";

    int wGear   = btnH;
    int wTrans  = btnH; // Icon-only circular Translate button!
    int wBrowse = scaleBtnW(MeasureButtonWidthAt96Dpi(state->i18n.browseBtn, 86));
    int wRun    = (state && state->settings.minimalMode && !state->translateMode)
                  ? btnH // Icon-only circular Run button in Compact Mini-Pill mode!
                  : scaleBtnW(MeasureButtonWidthAt96Dpi(runLabel, 80));
    int wClose  = scaleBtnW(MeasureButtonWidthAt96Dpi(state->i18n.exitBtn,   76));

    int barTop = GetInputBarTopY(state);
    ActionButtonsLayout layout = {};
    if (state && state->translateMode) {
        bool isCompact = state->settings.minimalMode;
        int topPad  = DpiScaleS(isCompact ? 8 : 10, state);
        int langTop = barTop + topPad;
        int langH   = DpiScaleS(isCompact ? 23 : 26, state);
        int sidePad = DpiScaleS(isCompact ? 14 : 18, state);
        int swapW   = DpiScaleS(isCompact ? 28 : 36, state);
        int langGap = DpiScaleS(isCompact ? 5 : 6, state);
        int pillW   = (std::min)(DpiScaleS(isCompact ? 145 : 210, state),
                                 (dlgW - sidePad * 2 - swapW - langGap * 2) / 2);
        layout.rcSrcLang  = { sidePad, langTop, sidePad + pillW, langTop + langH };
        layout.rcSwapLang = { layout.rcSrcLang.right + langGap, langTop,
                              layout.rcSrcLang.right + langGap + swapW, langTop + langH };
        layout.rcDstLang  = { layout.rcSwapLang.right + langGap, langTop,
                              layout.rcSwapLang.right + langGap + pillW, langTop + langH };

        int inputZoneTop = langTop + langH + DpiScaleS(isCompact ? 6 : 8, state);
        int inputZoneBot = barTop + barH - DpiScaleS(isCompact ? 8 : 10, state);
        int zoneH  = (std::max)(1, inputZoneBot - inputZoneTop);
        int tBtnH  = isCompact ? DpiScaleS(28, state) : btnH;
        int tGap   = isCompact ? DpiScaleS(5, state) : gap;
        int tRight = isCompact ? DpiScaleS(10, state) : rightMargin;
        ExtraLocalizedStrings ex = GetExtraLocalizedStrings(state->settings.language);
        int tWRun  = isCompact ? tBtnH : scaleBtnW(MeasureButtonWidthAt96Dpi(ex.translateBtnLabel, 80));
        int btnTop = inputZoneTop + (zoneH - tBtnH) / 2;

        // Order from right to left: [Gear] <- [Translate Icon] <- [Run]
        layout.rcGear      = { dlgW - tRight - tBtnH, btnTop, dlgW - tRight, btnTop + tBtnH };
        layout.rcTranslate = { layout.rcGear.left - tGap - tBtnH, btnTop, layout.rcGear.left - tGap, btnTop + tBtnH };
        layout.rcRun       = { layout.rcTranslate.left - tGap - tWRun, btnTop, layout.rcTranslate.left - tGap, btnTop + tBtnH };
        layout.rcClose     = { 0, 0, 0, 0 };
        layout.rcBrowse    = { 0, 0, 0, 0 };

        int editLeft = isCompact ? DpiScaleS(46, state) : DpiScaleS(50, state);
        layout.rcEdit = {
            editLeft,
            inputZoneTop,
            layout.rcRun.left - DpiScaleS(isCompact ? 6 : 10, state),
            inputZoneBot
        };
        return layout;
    }

    int top = barTop + (barH - btnH) / 2;
    // Order from right to left: [Gear] <- [Translate Icon] <- [Run]
    layout.rcGear      = { dlgW - rightMargin - wGear, top, dlgW - rightMargin, top + btnH };
    layout.rcTranslate = { layout.rcGear.left - gap - wTrans, top, layout.rcGear.left - gap, top + btnH };

    if (state && state->settings.minimalMode) {
        layout.rcClose  = { 0, 0, 0, 0 };
        layout.rcBrowse = { 0, 0, 0, 0 };
        layout.rcRun    = { layout.rcTranslate.left - gap - wRun, top, layout.rcTranslate.left - gap, top + btnH };
    } else {
        layout.rcClose  = { layout.rcTranslate.left - gap - wClose, top, layout.rcTranslate.left - gap, top + btnH };
        layout.rcRun    = { layout.rcClose.left - gap - wRun, top, layout.rcClose.left - gap, top + btnH };
        layout.rcBrowse = { layout.rcRun.left - gap - wBrowse, top, layout.rcRun.left - gap, top + btnH };
    }

    int editRightAnchor = (state && state->settings.minimalMode) ? layout.rcRun.left : layout.rcBrowse.left;
    int editH = (std::max)(DpiScaleS(24, state), (int)std::ceil(GetFontSizeSF(13.5f, state) * 1.6f));
    editH = (std::min)(editH, barH - DpiScale(6, dpi, 100));
    int editTop = barTop + (barH - editH) / 2;
    int editLeft = (state && state->settings.minimalMode) ? DpiScaleS(46, state) : DpiScaleS(50, state);
    layout.rcEdit = {
        editLeft,
        editTop,
        editRightAnchor - DpiScaleS(8, state),
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
    int cardTop = GetDetachedCardTopY(state);
    int padTop = GetListPadTop(state);
    int padBot = GetListPadBottom(state);
    int cardH  = GetHistoryCardHeight(state);
    int rightPad = DpiScaleS(8, state);
    int trackW = DpiScaleS(6, state);
    return {
        dlgW - rightPad - trackW,
        cardTop + padTop + DpiScaleS(4, state),
        dlgW - rightPad,
        cardTop + cardH - padBot - DpiScaleS(4, state)
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
    int cardTop = GetDetachedCardTopY(state);
    int padTop = GetListPadTop(state);
    int padBot = GetListPadBottom(state);
    int cardH  = GetHistoryCardHeight(state);
    int rowH   = GetRowHeight(state);
    int dlgW   = GetDlgWidth(state);
    int padX   = DpiScaleS(12, state);

    if (activeRows.empty() || visRows <= 0 || cardH <= 0 ||
        pt.y < cardTop + padTop || pt.y >= cardTop + cardH - padBot) {
        return -1;
    }
    if (HasVerticalScrollbar(state)) {
        RECT rcTrack = GetScrollbarTrackRect(state);
        if (pt.x >= rcTrack.left - DpiScaleS(4, state)) {
            return -1;
        }
    }
    if (pt.x < padX || pt.x > dlgW - padX) return -1;
    int visIdx = (pt.y - (cardTop + padTop)) / rowH;
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

// Official Google Translate ("文A") vector icon for the Translate button on the Input Bar
// (distinct from the Globe icon used for Display Language in the Settings menu)
void DrawD2DGoogleTranslateVector(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                                  float cx, float cy, UINT dpi) {
    if (!pRT || !pBrush) return;
    float sw = DpiScaleF(1.32f, dpi);
    auto pt = [&](float dxDip, float dyDip) -> D2D1_POINT_2F {
        return D2D1::Point2F(cx + DpiScaleF(dxDip, dpi), cy + DpiScaleF(dyDip, dpi));
    };

    // 1. Top-left Google Translate '文' glyph (top stem + horizontal bar + curved strokes)
    pRT->DrawLine(pt(-2.6f, -6.0f), pt(-2.6f, -4.2f), pBrush, sw, pStroke);
    pRT->DrawLine(pt(-6.2f, -4.2f), pt( 1.0f, -4.2f), pBrush, sw, pStroke);

    if (g_pD2DFactory) {
        ID2D1PathGeometry* pPath = nullptr;
        if (SUCCEEDED(g_pD2DFactory->CreatePathGeometry(&pPath)) && pPath) {
            ID2D1GeometrySink* pSink = nullptr;
            if (SUCCEEDED(pPath->Open(&pSink)) && pSink) {
                // Left-sweeping curve of '文'
                pSink->BeginFigure(pt(-0.6f, -4.2f), D2D1_FIGURE_BEGIN_HOLLOW);
                pSink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(pt(-1.9f, -0.3f), pt(-5.8f, 1.8f)));
                pSink->EndFigure(D2D1_FIGURE_END_OPEN);

                // Right-sweeping curve of '文'
                pSink->BeginFigure(pt(-4.7f, -2.4f), D2D1_FIGURE_BEGIN_HOLLOW);
                pSink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(pt(-2.9f, -0.1f), pt(-0.2f, 1.4f)));
                pSink->EndFigure(D2D1_FIGURE_END_OPEN);

                // Bottom-right 'A' arch (left foot -> apex -> right foot)
                pSink->BeginFigure(pt(0.6f, 5.9f), D2D1_FIGURE_BEGIN_HOLLOW);
                pSink->AddLine(pt(3.3f, -0.8f));
                pSink->AddLine(pt(6.0f, 5.9f));
                pSink->EndFigure(D2D1_FIGURE_END_OPEN);

                pSink->Close();
                pSink->Release();
                pRT->DrawGeometry(pPath, pBrush, sw, pStroke);
            }
            pPath->Release();
        }
    } else {
        pRT->DrawLine(pt(-0.6f, -4.2f), pt(-5.8f, 1.8f), pBrush, sw, pStroke);
        pRT->DrawLine(pt(-4.7f, -2.4f), pt(-0.2f, 1.4f), pBrush, sw, pStroke);
        pRT->DrawLine(pt(0.6f, 5.9f),   pt(3.3f, -0.8f), pBrush, sw, pStroke);
        pRT->DrawLine(pt(3.3f, -0.8f),  pt(6.0f, 5.9f),  pBrush, sw, pStroke);
    }

    // Crossbar of 'A'
    pRT->DrawLine(pt(1.45f, 3.7f), pt(5.15f, 3.7f), pBrush, sw, pStroke);
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

// Renders a crisp checkmark (✓) for single-choice selection options in submenus
void DrawD2DMenuCheckmark(ID2D1RenderTarget* pRT, ID2D1Brush* pBrush, ID2D1StrokeStyle* pStroke,
                          float cx, float cy, UINT dpi) {
    float sw = DpiScaleF(1.55f, dpi);
    D2D1_POINT_2F p1 = D2D1::Point2F(cx - DpiScaleF(4.2f, dpi), cy + DpiScaleF(0.3f, dpi));
    D2D1_POINT_2F p2 = D2D1::Point2F(cx - DpiScaleF(1.2f, dpi), cy + DpiScaleF(3.3f, dpi));
    D2D1_POINT_2F p3 = D2D1::Point2F(cx + DpiScaleF(4.6f, dpi), cy - DpiScaleF(3.1f, dpi));
    pRT->DrawLine(p1, p2, pBrush, sw, pStroke);
    pRT->DrawLine(p2, p3, pBrush, sw, pStroke);
}

// Renders a 3D Refractive Liquid Glass ToggleSwitch for boolean on/off feature rows (e.g. Compact Mode & Command History)
void DrawD2DWin11ToggleSwitch(ID2D1RenderTarget* pRT, float rightEdgeX, float centerY,
                              UINT dpi, bool isOn, bool isLight, bool isDisabled, bool isHovered) {
    if (!pRT) return;
    float trackW = DpiScaleF(38.0f, dpi);
    float trackH = DpiScaleF(19.0f, dpi);
    float trackR = trackH * 0.5f;
    D2D1_RECT_F rcTrack = D2D1::RectF(rightEdgeX - trackW, centerY - trackR, rightEdgeX, centerY + trackR);
    D2D1_ROUNDED_RECT rrTrack = D2D1::RoundedRect(rcTrack, trackR, trackR);

    float alphaMul = isDisabled ? 0.42f : 1.0f;
    bool activeHov = isHovered && !isDisabled;

    // 1. Liquid Glass Capsule Track Fill
    if (isOn) {
        // Luminous cyan-azure-to-violet refractive Liquid Glass fill when ON
        D2D1_GRADIENT_STOP onStops[3] = {
            { 0.00f, D2D1::ColorF(0.28f, 0.84f, 1.00f, (activeHov ? 0.72f : 0.60f) * alphaMul) },
            { 0.52f, D2D1::ColorF(0.14f, 0.58f, 0.98f, (activeHov ? 0.62f : 0.50f) * alphaMul) },
            { 1.00f, D2D1::ColorF(0.42f, 0.40f, 0.98f, (activeHov ? 0.68f : 0.56f) * alphaMul) }
        };
        ID2D1GradientStopCollection* pOnStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(onStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pOnStops)) && pOnStops) {
            ID2D1LinearGradientBrush* pOnBr = nullptr;
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(rcTrack.left, rcTrack.top), D2D1::Point2F(rcTrack.right, rcTrack.bottom));
            if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pOnStops, &pOnBr)) && pOnBr) {
                pRT->FillRoundedRectangle(rrTrack, pOnBr);
                pOnBr->Release();
            }
            pOnStops->Release();
        }
    } else {
        // Translucent frosted Liquid Glass capsule when OFF
        D2D1_COLOR_F offBase = isLight
            ? D2D1::ColorF(0.04f, 0.08f, 0.16f, (activeHov ? 0.14f : 0.08f) * alphaMul)
            : D2D1::ColorF(0.82f, 0.90f, 1.00f, (activeHov ? 0.18f : 0.10f) * alphaMul);
        ID2D1SolidColorBrush* pOffBr = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(offBase, &pOffBr)) && pOffBr) {
            pRT->FillRoundedRectangle(rrTrack, pOffBr);
            pOffBr->Release();
        }
    }

    // 2. Top-to-Bottom 3D Glass Tube Specular Cushion Sheen
    {
        float topSheen = (isOn ? 0.44f : (activeHov ? 0.34f : 0.24f)) * alphaMul;
        float botSheen = (isOn ? 0.22f : 0.12f) * alphaMul;
        D2D1_GRADIENT_STOP sheenStops[3] = {
            { 0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, topSheen) },
            { 0.48f, D2D1::ColorF(1.00f, 1.00f, 1.00f, 0.00f) },
            { 1.00f, D2D1::ColorF(0.76f, 0.93f, 1.00f, botSheen) }
        };
        ID2D1GradientStopCollection* pSheenStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(sheenStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pSheenStops)) && pSheenStops) {
            ID2D1LinearGradientBrush* pSheenBr = nullptr;
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(rcTrack.left, rcTrack.top), D2D1::Point2F(rcTrack.left, rcTrack.bottom));
            if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pSheenStops, &pSheenBr)) && pSheenBr) {
                pRT->FillRoundedRectangle(rrTrack, pSheenBr);
                pSheenBr->Release();
            }
            pSheenStops->Release();
        }
    }

    // 3. 5-Stop Chromatic Refractive Liquid Glass Rim around Track
    {
        float rimAlpha = (isOn ? 1.0f : 0.85f) * alphaMul;
        D2D1_GRADIENT_STOP rimStops[5] = {
            { 0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, 0.92f * rimAlpha) },
            { 0.30f, D2D1::ColorF(0.68f, 0.90f, 1.00f, 0.56f * rimAlpha) },
            { 0.52f, D2D1::ColorF(1.00f, 1.00f, 1.00f, 0.24f * rimAlpha) },
            { 0.76f, D2D1::ColorF(1.00f, 0.80f, 0.95f, 0.54f * rimAlpha) },
            { 1.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, 0.80f * rimAlpha) }
        };
        ID2D1GradientStopCollection* pRimStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(rimStops, 5, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pRimStops)) && pRimStops) {
            ID2D1LinearGradientBrush* pRimBr = nullptr;
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(rcTrack.left, rcTrack.top), D2D1::Point2F(rcTrack.right, rcTrack.bottom));
            if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pRimStops, &pRimBr)) && pRimBr) {
                pRT->DrawRoundedRectangle(rrTrack, pRimBr, DpiScaleF(1.2f, dpi));
                pRimBr->Release();
            }
            pRimStops->Release();
        }
    }

    // 4. 3D Liquid Glass Droplet Thumb
    float thumbR  = DpiScaleF(activeHov ? 6.6f : 6.1f, dpi);
    float thumbCx = isOn ? (rcTrack.right - trackR) : (rcTrack.left + trackR);

    // Subtle optical drop shadow / caustic glow under glass thumb
    {
        D2D1_COLOR_F shadowCol = isOn
            ? D2D1::ColorF(0.02f, 0.16f, 0.42f, 0.30f * alphaMul)
            : D2D1::ColorF(0.00f, 0.02f, 0.08f, 0.22f * alphaMul);
        ID2D1SolidColorBrush* pShadowBr = nullptr;
        if (SUCCEEDED(pRT->CreateSolidColorBrush(shadowCol, &pShadowBr)) && pShadowBr) {
            pRT->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(thumbCx, centerY + DpiScaleF(0.85f, dpi)), thumbR, thumbR),
                pShadowBr);
            pShadowBr->Release();
        }
    }

    // 3D Spherical Glass Bead Radial Gradient
    {
        D2D1_GRADIENT_STOP thumbStops[3] = {
            { 0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, (isOn ? 0.99f : 0.92f) * alphaMul) },
            { 0.58f, isOn ? D2D1::ColorF(0.90f, 0.97f, 1.00f, 0.90f * alphaMul)
                          : D2D1::ColorF(0.84f, 0.91f, 0.98f, 0.72f * alphaMul) },
            { 1.00f, isOn ? D2D1::ColorF(0.72f, 0.90f, 1.00f, 0.82f * alphaMul)
                          : D2D1::ColorF(0.68f, 0.78f, 0.92f, 0.56f * alphaMul) }
        };
        ID2D1GradientStopCollection* pThumbStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(thumbStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pThumbStops)) && pThumbStops) {
            ID2D1RadialGradientBrush* pThumbBr = nullptr;
            D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES rp = D2D1::RadialGradientBrushProperties(
                D2D1::Point2F(thumbCx, centerY),
                D2D1::Point2F(-thumbR * 0.28f, -thumbR * 0.30f),
                thumbR, thumbR);
            if (SUCCEEDED(pRT->CreateRadialGradientBrush(rp, pThumbStops, &pThumbBr)) && pThumbBr) {
                pRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(thumbCx, centerY), thumbR, thumbR), pThumbBr);
                pThumbBr->Release();
            }
            pThumbStops->Release();
        }
    }

    // Crisp Refractive Rim around the Glass Droplet Thumb
    {
        D2D1_GRADIENT_STOP tRimStops[3] = {
            { 0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, 0.96f * alphaMul) },
            { 0.50f, D2D1::ColorF(0.75f, 0.93f, 1.00f, 0.52f * alphaMul) },
            { 1.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, 0.84f * alphaMul) }
        };
        ID2D1GradientStopCollection* pTRimStops = nullptr;
        if (SUCCEEDED(pRT->CreateGradientStopCollection(tRimStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pTRimStops)) && pTRimStops) {
            ID2D1LinearGradientBrush* pTRimBr = nullptr;
            D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp = D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(thumbCx - thumbR, centerY - thumbR),
                D2D1::Point2F(thumbCx + thumbR, centerY + thumbR));
            if (SUCCEEDED(pRT->CreateLinearGradientBrush(gp, pTRimStops, &pTRimBr)) && pTRimBr) {
                pRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(thumbCx, centerY), thumbR, thumbR),
                                 pTRimBr, DpiScaleF(1.0f, dpi));
                pTRimBr->Release();
            }
            pTRimStops->Release();
        }
    }
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
    int hoveredInlinePart = 0;
    int pressedIdx = -1;
    int openSubIdx = -1;
    int scrollOffset = 0;
    int maxVisibleRows = 14;
    int width = 260;
    int height = 100;
    POINT mousePt = { -1, -1 };
    bool hasMousePt = false;
    struct FluentMenuSession* session = nullptr;

    // Persistent double-buffer DIB & Direct2D RenderTarget to eliminate menu hover flicker
    HDC hCachedMemDC = nullptr;
    HBITMAP hCachedBmp = nullptr;
    HGDIOBJ hOldCachedBmp = nullptr;
    DWORD* cachedDibPixels = nullptr;
    int cachedDibW = 0;
    int cachedDibH = 0;
    ID2D1DCRenderTarget* pCachedRT = nullptr;
};

void ReleaseFluentMenuPanelRenderCache(FluentMenuPanelState* p, int cacheSlot) {
    if (!p) return;
    if (p->pCachedRT) {
        {
            std::lock_guard<std::mutex> lock(g_opticalCacheMutex);
            ReleaseOpticalCacheBitmap(cacheSlot == 1 ? g_menuMainOpticalCache : g_menuSubOpticalCache);
        }
        p->pCachedRT->Release();
        p->pCachedRT = nullptr;
    }
    if (p->hCachedMemDC) {
        if (p->hOldCachedBmp) {
            SelectObject(p->hCachedMemDC, p->hOldCachedBmp);
            p->hOldCachedBmp = nullptr;
        }
        if (p->hCachedBmp) {
            DeleteObject(p->hCachedBmp);
            p->hCachedBmp = nullptr;
        }
        DeleteDC(p->hCachedMemDC);
        p->hCachedMemDC = nullptr;
    }
    p->cachedDibPixels = nullptr;
    p->cachedDibW = 0;
    p->cachedDibH = 0;
}

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
    return DpiScale(10, s->effectiveDpi);
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
            return std::clamp(state->settings.maxHistoryItems, 0, 20);
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
    InvalidateRect(st->hDlg, nullptr, FALSE);
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
                GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
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
    bool hasToggleSwitches = false;
    for (const auto& item : p->items) {
        if (item.id == 350 || item.id == 400) {
            hasToggleSwitches = true;
            break;
        }
    }
    // Left icon margin (42px) + text width + right ToggleSwitch/checkmark/chevron padding
    int extraRight = hasToggleSwitches
        ? DpiScale(62, s->effectiveDpi)
        : (PanelNeedsScroll(p) ? DpiScale(46, s->effectiveDpi) : DpiScale(38, s->effectiveDpi));
    if (hasInlineInputs) {
        extraRight += DpiScale(120, s->effectiveDpi);
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

    BOOL dark = s->isLight ? FALSE : TRUE;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    // Per-pixel DWM composition so the menu has rounded pill-card corners synchronized with the Input Bar!
    DWORD corner = 1; // DWMWCP_DONOTROUND
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    COLORREF borderCol = 0xFFFFFFFE; // DWMWA_COLOR_NONE
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &borderCol, sizeof(borderCol));

    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hWnd, &margins);

    HRGN hEmptyRgn = CreateRectRgn(0, 0, -1, -1);
    DWM_BLURBEHIND bb = {};
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.fEnable = TRUE;
    bb.hRgnBlur = hEmptyRgn;
    DwmEnableBlurBehindWindow(hWnd, &bb);
    if (hEmptyRgn) DeleteObject(hEmptyRgn);

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return;
    auto pSetWCA = reinterpret_cast<SetWindowCompositionAttribute_t>(
        GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
    if (!pSetWCA) return;

    ACCENT_POLICY policy = {};
    policy.AccentState = ACCENT_DISABLED;

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
        bool hadOpenSub = (s->mainPanel.openSubIdx != -1) ||
                          (s->subPanel.hWnd && IsWindow(s->subPanel.hWnd) && IsWindowVisible(s->subPanel.hWnd));
        s->mainPanel.openSubIdx = -1;
        if (hadOpenSub) {
            if (s->subPanel.hWnd && IsWindow(s->subPanel.hWnd)) {
                ShowWindow(s->subPanel.hWnd, SW_HIDE);
            }
            if (s->mainPanel.hWnd) InvalidateRect(s->mainPanel.hWnd, nullptr, FALSE);
        }
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
    InvalidateRect(s->subPanel.hWnd, nullptr, FALSE);
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

    int slot = (hWnd == s->mainPanel.hWnd) ? 1 : 2;
    if (w > 0 && h > 0) {
        if (!p->hCachedMemDC || !p->hCachedBmp || !p->pCachedRT ||
            p->cachedDibW != w || p->cachedDibH != h) {
            ReleaseFluentMenuPanelRenderCache(p, slot);

            p->hCachedMemDC = CreateCompatibleDC(hdc);
            if (p->hCachedMemDC) {
                BITMAPINFO bmi = {};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = w;
                bmi.bmiHeader.biHeight = -h;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;

                p->hCachedBmp = CreateDIBSection(p->hCachedMemDC, &bmi, DIB_RGB_COLORS,
                                                 (void**)&p->cachedDibPixels, nullptr, 0);
                if (p->hCachedBmp && p->cachedDibPixels) {
                    p->hOldCachedBmp = SelectObject(p->hCachedMemDC, p->hCachedBmp);
                    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                        96.0f, 96.0f);
                    if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&rtProps, &p->pCachedRT)) && p->pCachedRT) {
                        p->pCachedRT->BindDC(p->hCachedMemDC, &rcClient);
                        p->cachedDibW = w;
                        p->cachedDibH = h;
                    }
                }
            }
        }

        HDC hMemDC = p->hCachedMemDC;
        ID2D1DCRenderTarget* pRT = p->pCachedRT;
        if (hMemDC && p->hCachedBmp && p->cachedDibPixels) {
            if (pRT) {
                pRT->BindDC(hMemDC, &rcClient);
                pRT->BeginDraw();
                pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                ApplyFluentTextRenderingToRT(pRT);

                float rimInset = DpiScaleF(0.75f, dpi);
                D2D1_RECT_F rcMenuCard = D2D1::RectF(rimInset, rimInset, (float)w - rimInset, (float)h - rimInset);
                float menuCornerR = (std::min)(DpiScaleF(20.0f, dpi), (rcMenuCard.bottom - rcMenuCard.top) * 0.5f);

                // Clear outer window corners to 100% transparent
                pRT->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

                // 1. Synchronized Pill-Card Backdrop (Liquid Glass / Frosted Windows Blur / Solid)
                if (highContrast) {
                    COLORREF winBg = GetSysColor(COLOR_WINDOW);
                    ID2D1SolidColorBrush* pHcBr = nullptr;
                    if (SUCCEEDED(pRT->CreateSolidColorBrush(
                            D2D1::ColorF(GetRValue(winBg) / 255.0f, GetGValue(winBg) / 255.0f, GetBValue(winBg) / 255.0f, 1.0f), &pHcBr))) {
                        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcMenuCard, menuCornerR, menuCornerR), pHcBr);
                        pHcBr->Release();
                    }
                } else if (s->surface == SurfaceTheme::Solid) {
                    ID2D1SolidColorBrush* pSolBr = nullptr;
                    if (SUCCEEDED(pRT->CreateSolidColorBrush(
                            isLight ? D2D1::ColorF(0.96f, 0.96f, 0.98f, 1.0f)
                                    : D2D1::ColorF(0.11f, 0.12f, 0.16f, 1.0f), &pSolBr))) {
                        pRT->FillRoundedRectangle(D2D1::RoundedRect(rcMenuCard, menuCornerR, menuCornerR), pSolBr);
                        pSolBr->Release();
                    }
                } else if (s->surface == SurfaceTheme::Glass) {
                    int glassPct = (s->dlgState) ? std::clamp(s->dlgState->settings.glassOpacity, -100, 100) : -100;
                    DrawOpticalLiquidGlassBackdrop(pRT, hWnd, w, h, 0, 0, dpi, glassPct, isLight, true, slot);
                } else {
                    // Windows Blur: translucent vibrant Acrylic blur matching Windows 11 right-click menu (Image 2)
                    int blurOp = (s->dlgState) ? std::clamp(s->dlgState->settings.blurOpacity, 0, 255) : 255;
                    DrawOpticalLiquidGlassBackdrop(pRT, hWnd, w, h, 0, 0, dpi, -(1000 + blurOp), isLight, true, slot);
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
                    GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.0f, dpi), L"", &pFmtItem);
                if (pFmtItem) {
                    pFmtItem->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    pFmtItem->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                }

                int padY = GetMenuPadY(s);
                int rowH = GetMenuRowHeight(s);
                int sepH = GetMenuSeparatorHeight(s);
                float curY = (float)padY;
                float padX = DpiScaleF(9.0f, dpi);
                float pillRadius = (float)rowH * 0.5f;
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
                            padX, curY + DpiScaleF(2.0f, dpi),
                            (float)w - padX - sbReserve, curY + rowH - DpiScaleF(2.0f, dpi));
                        pillRadius = (rcPill.bottom - rcPill.top) * 0.5f;
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

                    if (item.icon != FluentMenuIcon::None && pItemIconBr && pRoundStroke) {
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
                        float ctlRad = (rcBox.bottom - rcBox.top) * 0.5f;

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
                                GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
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
                        // Only boolean on/off switches (Compact Mode = 350, Command History = 400) use a Toggle Switch;
                        // single-choice selection options in submenus/popups use a Checkmark (✓).
                        bool isToggleRow = (item.id == 350 || item.id == 400);
                        float leftTextX = (item.icon == FluentMenuIcon::None)
                            ? DpiScaleF(18.0f, dpi)
                            : DpiScaleF(42.0f, dpi);
                        float rightReserve = isToggleRow ? DpiScaleF(56.0f, dpi) : DpiScaleF(30.0f, dpi);

                        if (pFmtItem && pItemTextBr && !item.label.empty()) {
                            D2D1_RECT_F rcText = D2D1::RectF(
                                leftTextX,
                                curY,
                                (float)w - rightReserve - sbReserve,
                                curY + rowH);
                            pRT->DrawText(item.label.c_str(), (UINT32)item.label.size(),
                                          pFmtItem, rcText, pItemTextBr, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                        }

                        if (!item.subItems.empty() && pBrSub && pRoundStroke) {
                            float chevX = (float)w - DpiScaleF(16.0f, dpi) - sbReserve;
                            DrawD2DMenuChevronRight(pRT, pBrSub, pRoundStroke, chevX, iconCy, dpi);
                        } else if (isToggleRow) {
                            float swRightX = (float)w - DpiScaleF(12.0f, dpi) - sbReserve;
                            DrawD2DWin11ToggleSwitch(pRT, swRightX, iconCy, dpi,
                                                     item.checked, isLight, item.disabled, isHovered);
                        } else if (item.checked && pRoundStroke && (pBrAccent || pBrText)) {
                            float checkX = (float)w - DpiScaleF(18.0f, dpi) - sbReserve;
                            DrawD2DMenuCheckmark(pRT, pBrAccent ? pBrAccent : pBrText, pRoundStroke, checkX, iconCy, dpi);
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

                // Outer Synchronized Pill-Card Rim
                if (!highContrast) {
                    DrawD2DGlassRim(pRT, rcMenuCard, menuCornerR, dpi, isLight, 1.35f);
                }

                pRT->EndDraw();

                if (pFmtItem)     pFmtItem->Release();
                if (pRoundStroke) pRoundStroke->Release();
                if (pBrAccent)    pBrAccent->Release();
                if (pBrMuted)     pBrMuted->Release();
                if (pBrSub)       pBrSub->Release();
                if (pBrIcon)      pBrIcon->Release();
                if (pBrText)      pBrText->Release();
            }

            BitBlt(hdc, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
        }
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
            bool enteredMouse = !p->hasMousePt;
            p->mousePt = pt;
            p->hasMousePt = true;

            RECT rcRow = {};
            int row = HitTestFluentMenuRow(s, p, pt, &rcRow);
            int inlinePart = 0;
            if (row >= 0 && row < (int)p->items.size() && p->items[row].inlineEditKind != 0) {
                D2D1_RECT_F rcMinus = {}, rcBox = {}, rcPlus = {};
                GetInlineControlRects(s, p, (float)rcRow.top, (float)(rcRow.bottom - rcRow.top),
                                      &rcMinus, &rcBox, &rcPlus);
                if (pt.x >= rcMinus.left && pt.x <= rcMinus.right && pt.y >= rcMinus.top && pt.y <= rcMinus.bottom) {
                    inlinePart = 1;
                } else if (pt.x >= rcBox.left && pt.x <= rcBox.right && pt.y >= rcBox.top && pt.y <= rcBox.bottom) {
                    inlinePart = 2;
                } else if (pt.x >= rcPlus.left && pt.x <= rcPlus.right && pt.y >= rcPlus.top && pt.y <= rcPlus.bottom) {
                    inlinePart = 3;
                }
            }

            bool needRepaint = false;
            if (row != p->hoveredIdx) {
                p->hoveredIdx = row;
                needRepaint = true;
                if (!p->isSubMenu) {
                    s->pendingHoverSubIdx = row;
                    SetTimer(hWnd, kTimerMenuSubHover, 65, nullptr);
                }
            }
            if (inlinePart != p->hoveredInlinePart) {
                p->hoveredInlinePart = inlinePart;
                needRepaint = true;
            }
            if (enteredMouse) {
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
            }
            if (needRepaint) {
                InvalidateRect(hWnd, nullptr, FALSE);
            }
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
    int mainY = (state && state->anchoredTop)
        ? (ptAnchorRightBottom.y + DpiScale(6, session.effectiveDpi))
        : (ptAnchorRightBottom.y - session.mainPanel.height - DpiScale(6, session.effectiveDpi));
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
    ReleaseFluentMenuPanelRenderCache(&session.subPanel, 2);
    ReleaseFluentMenuPanelRenderCache(&session.mainPanel, 1);
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
                ApplyFluentTextRenderingToRT(pRT);

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
                g_pDWriteFactory->CreateTextFormat(GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(13.5f, dpi), L"", &pFmtTitle);
                g_pDWriteFactory->CreateTextFormat(GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                    DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.5f, dpi), L"", &pFmtLabel);
                g_pDWriteFactory->CreateTextFormat(GetPreferredFluentFontFamily(), nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
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
        InvalidateRect(hDlg, nullptr, FALSE);
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

    ExtraLocalizedStrings ex = GetExtraLocalizedStrings(state->settings.language);

    // 3b. Compact Mini-Pill Mode (21 languages)
    {
        FluentMenuItem compactItem;
        compactItem.id      = 350;
        compactItem.label   = ex.compactPillMenu;
        compactItem.icon    = FluentMenuIcon::MinimalLayout;
        compactItem.checked = state->settings.minimalMode;
        menuItems.push_back(std::move(compactItem));
    }

    // 4. History / Suggestions toggle (dimmed & locked ON when Compact Mini-Pill Mode is active!)
    {
        FluentMenuItem histToggle;
        histToggle.id       = 400;
        histToggle.label    = ex.showHistoryMenu;
        histToggle.icon     = FluentMenuIcon::History;
        histToggle.checked  = state->settings.minimalMode ? true : (state->settings.maxHistoryItems > 0);
        histToggle.disabled = state->settings.minimalMode;
        menuItems.push_back(std::move(histToggle));
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

    // Separator + Clear History + Run from File (Browse) + Exit Run Dialog
    {
        FluentMenuItem sep;
        sep.isSeparator = true;
        menuItems.push_back(std::move(sep));

        FluentMenuItem browseRunItem;
        browseRunItem.id    = 702;
        browseRunItem.label = std::wstring(state->i18n.browseBtn) + L" – " + state->i18n.browseTitle + L"...";
        browseRunItem.icon  = FluentMenuIcon::FolderPick;
        menuItems.push_back(std::move(browseRunItem));

        FluentMenuItem clearItem;
        clearItem.id    = 701;
        clearItem.label = ml.clearAllHistory;
        clearItem.icon  = FluentMenuIcon::ClearTrash;
        menuItems.push_back(std::move(clearItem));

        FluentMenuItem sep2;
        sep2.isSeparator = true;
        menuItems.push_back(std::move(sep2));

        FluentMenuItem exitItem;
        exitItem.id    = 703;
        exitItem.label = state->i18n.exitBtn;
        exitItem.icon  = FluentMenuIcon::RemoveBg;
        menuItems.push_back(std::move(exitItem));
    }

    ActionButtonsLayout layout = GetActionButtonsLayout(state);
    POINT ptMenu = { layout.rcGear.right, state->anchoredTop ? layout.rcGear.bottom : layout.rcGear.top };
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

    if (cmd == 702) {
        OpenBrowseExecutableDialog(state, true);
        return;
    }
    if (cmd == 703) {
        CloseRunDialog(hDlg);
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
        else if (cmd == 350) {
            Wh_SetIntValue(GetUserOverrideKey(L"minimalModeV3").c_str(), state->settings.minimalMode ? 0 : 1);
        } else if (cmd == 400) {
            int nextMax = (state->settings.maxHistoryItems > 0) ? 0 : 20;
            Wh_SetIntValue(GetUserOverrideKey(L"maxHistoryItems").c_str(), nextMax);
        } else if (cmd >= 600 && cmd <= 621) {
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
        InvalidateRect(hDlg, nullptr, FALSE);
    }
}

void OpenTranslateLanguagePopup(RunDialogState* state, bool isSourceLang) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    HWND hDlg = state->hDlg;
    bool isVi = IsDialogVietnameseUI(state);

    std::vector<FluentMenuItem> items;
    const std::wstring& curCode = isSourceLang ? state->translateSourceLang : state->translateTargetLang;
    for (const auto& entry : kTranslateLangs) {
        if (!isSourceLang && wcscmp(entry.code, L"auto") == 0) continue;
        FluentMenuItem it;
        it.id = entry.menuId;
        it.label = isVi ? entry.labelVi : entry.labelEn;
        it.icon = FluentMenuIcon::None;
        it.checked = (_wcsicmp(curCode.c_str(), entry.code) == 0);
        items.push_back(std::move(it));
    }

    ActionButtonsLayout layout = GetActionButtonsLayout(state);
    RECT rcBtn = isSourceLang ? layout.rcSrcLang : layout.rcDstLang;
    POINT ptMenu = { rcBtn.right, state->anchoredTop ? rcBtn.bottom : rcBtn.top };
    ClientToScreen(hDlg, &ptMenu);

    state->isBrowsingFile = true;
    g_isBrowsingFile.store(true);
    UINT cmd = RunFluentPopupMenu(state, std::move(items), ptMenu);
    g_isBrowsingFile.store(false);
    if (!IsWindow(hDlg) || g_hActiveRunDlg.load() != hDlg) return;
    state->isBrowsingFile = false;

    if (cmd >= 800 && cmd <= 821) {
        for (const auto& entry : kTranslateLangs) {
            if (entry.menuId == cmd) {
                if (isSourceLang) {
                    state->translateSourceLang = entry.code;
                    Wh_SetStringValue(GetUserOverrideKey(L"translateSourceLang").c_str(), entry.code);
                } else {
                    state->translateTargetLang = entry.code;
                    Wh_SetStringValue(GetUserOverrideKey(L"translateTargetLang").c_str(), entry.code);
                }
                break;
            }
        }
        InvalidateRect(hDlg, nullptr, FALSE);
        if (!TrimCommandString(GetEffectiveInputText(state->hModernEdit)).empty()) {
            TriggerTranslationRequest(state);
        }
    }
    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
        SetFocus(state->hModernEdit);
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
    RefreshDialogRunIcons(state);
    ReloadCustomBackgroundMedia(state, false);
    LoadHistoryItems(state);
    std::wstring curText = GetEffectiveInputText(state->hModernEdit);
    UpdateAppSuggestionsForQuery(state, curText);
    RecomputeDialogMetrics(state);
    ResizeDialogPreservingBottomAnchor(state, oldHeight);
    ApplyWindowAppearance(state->hDlg, state);
    InvalidateRect(state->hDlg, nullptr, FALSE);
    UpdateWindow(state->hDlg);
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
        DEFAULT_PITCH | FF_DONTCARE, GetPreferredFluentFontFamily());
    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
        SendMessageW(state->hModernEdit, WM_SETFONT, (WPARAM)state->hFontEditCtl, FALSE);
    }
}

void RefreshDialogRunIcons(RunDialogState* state) {
    if (!state) return;
    int barIconSize = DpiScaleS(20, state);
    if (!state->hRunIcon || state->runIconLoadedSize != barIconSize) {
        if (state->hRunIcon) {
            DestroyIcon(state->hRunIcon);
            state->hRunIcon = nullptr;
        }
        state->hRunIcon = LoadCustomRunIcon(barIconSize);
        state->runIconLoadedSize = barIconSize;
    }

    if (state->hDlg && IsWindow(state->hDlg)) {
        if (!state->hTaskbarIconSmall || !state->hTaskbarIconBig || state->taskbarIconLoadedDpi != (int)state->dpi) {
            int smPx = DpiScale(24, state->dpi);
            int bigPx = DpiScale(48, state->dpi);
            if (state->hTaskbarIconSmall) {
                DestroyIcon(state->hTaskbarIconSmall);
                state->hTaskbarIconSmall = nullptr;
            }
            if (state->hTaskbarIconBig) {
                DestroyIcon(state->hTaskbarIconBig);
                state->hTaskbarIconBig = nullptr;
            }
            state->hTaskbarIconSmall = LoadCustomRunIcon(smPx);
            state->hTaskbarIconBig = LoadCustomRunIcon(bigPx);
            state->taskbarIconLoadedDpi = (int)state->dpi;

            ApplyWindowTaskbarIconIdentity(state->hDlg, false);

            // Never add WS_CAPTION here! Adding WS_CAPTION on a visible dialog triggers win32kfull!xxxRedrawTitle
            // / WM_NCUAHDRAWCAPTION to paint the classic Aero Basic titlebar over the top of the window.
            g_internalIconChange = true;
            if (state->hTaskbarIconSmall) {
                SendMessageW(state->hDlg, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(state->hTaskbarIconSmall));
                SendMessageW(state->hDlg, WM_SETICON, ICON_SMALL2, reinterpret_cast<LPARAM>(state->hTaskbarIconSmall));
                SetClassLongPtrW(state->hDlg, GCLP_HICONSM, reinterpret_cast<LONG_PTR>(state->hTaskbarIconSmall));
            }
            if (state->hTaskbarIconBig) {
                SendMessageW(state->hDlg, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(state->hTaskbarIconBig));
                SetClassLongPtrW(state->hDlg, GCLP_HICON, reinterpret_cast<LONG_PTR>(state->hTaskbarIconBig));
            }
            g_internalIconChange = false;

            NotifyTaskbarRedrawWindowIcon(state->hDlg);
        }
    }
}

void RecomputeDialogMetrics(RunDialogState* state) {
    if (!state) return;
    int barH = GetInputBarHeight(state);
    int gapH = GetDetachedGap(state);

    if (state->translateMode) {
        if (state->isTranslating || !state->translatedText.empty()) {
            bool isCompact = state->settings.minimalMode;
            float availW = (float)(std::max)(DpiScaleS(100, state), GetDlgWidth(state) - DpiScaleS(isCompact ? 32 : 44, state));
            ExtraLocalizedStrings ex = GetExtraLocalizedStrings(state->settings.language);
            std::wstring disp = state->isTranslating
                ? ex.translatingStatus
                : state->translatedText;
            int outH = MeasureWrappedTextHeightPx(state, disp, availW, isCompact ? 13.0f : 14.0f);
            int minCardH = DpiScaleS(isCompact ? 58 : 68, state);
            int maxCardH = DpiScaleS(isCompact ? 240 : 340, state);
            int cardH = std::clamp(DpiScaleS(isCompact ? 30 : 34, state) + outH + DpiScaleS(isCompact ? 12 : 16, state),
                                   minCardH, maxCardH);
            state->topAreaHeight = cardH + gapH;
        } else {
            state->topAreaHeight = 0;
        }
        state->totalHeight = state->topAreaHeight + barH;
        return;
    }

    ClampScrollOffset(state);
    int visRowCount = GetVisibleRowCount(state);
    int padTop = GetListPadTop(state);
    int padBot = GetListPadBottom(state);
    int rowH   = GetRowHeight(state);

    state->topAreaHeight = (visRowCount > 0) ? (padTop + visRowCount * rowH + padBot + gapH) : 0;
    state->totalHeight   = state->topAreaHeight + barH;
}

void ScheduleDeferredInputUpdate(RunDialogState* state) {
    if (!state || !state->hDlg || !IsWindow(state->hDlg)) return;
    state->caretVisible = true;
    int barTop = GetInputBarTopY(state);
    RECT rcBar = { 0, barTop, GetDlgWidth(state), barTop + GetInputBarHeight(state) };
    InvalidateRect(state->hDlg, &rcBar, FALSE);
    UpdateWindow(state->hDlg);
    if (!state->inputUpdateQueued) {
        state->inputUpdateQueued = true;
        PostMessageW(state->hDlg, g_msgToggle, kActionDeferredInputUpdate, 0);
    }
}

LRESULT CALLBACK ModernEditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                        UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    auto* state = reinterpret_cast<RunDialogState*>(dwRefData);

    if (uMsg == WM_GETDLGCODE) {
        return DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTARROWS | DLGC_HASSETSEL;
    }

    if (uMsg == WM_IME_SETCONTEXT) {
        lParam &= ~(ISC_SHOWUICOMPOSITIONWINDOW | ISC_SHOWUIGUIDELINE);
    }

    if (uMsg == WM_MOUSEWHEEL && state && state->hDlg) {
        SendMessageW(state->hDlg, WM_MOUSEWHEEL, wParam, lParam);
        return 0;
    }

    if (uMsg == WM_KEYDOWN && state) {
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
                InvalidateRect(state->hDlg, nullptr, FALSE);
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
                }
                InvalidateRect(state->hDlg, nullptr, FALSE);
                return 0;
            }
        }
    }

    if (uMsg == WM_CHAR && (wParam == VK_RETURN || wParam == VK_ESCAPE || wParam == VK_TAB)) {
        return 0;
    }

    // Let RICHEDIT50W / TSF process all keystrokes, Vietnamese Telex/VNI compositions, and Space 100% natively!
    LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (state) {
        if (uMsg == WM_IME_COMPOSITION || uMsg == WM_IME_ENDCOMPOSITION || uMsg == WM_IME_CHAR ||
            uMsg == WM_CHAR || uMsg == WM_PASTE || uMsg == WM_CUT || uMsg == WM_SETTEXT) {
            ScheduleDeferredInputUpdate(state);
        } else if (uMsg == WM_KEYUP || uMsg == WM_KEYDOWN || uMsg == EM_SETSEL) {
            state->caretVisible = true;
            int barTop = GetInputBarTopY(state);
            RECT rcBar = { 0, barTop, GetDlgWidth(state), barTop + GetInputBarHeight(state) };
            InvalidateRect(state->hDlg, &rcBar, FALSE);
        }
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
    KillTimer(hWnd, kTimerOpenAnim);
    RemovePropW(hWnd, L"WindhawkModernRunState");
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
        if (hWnd && IsWindow(hWnd)) {
            g_internalIconChange = true;
            ApplyWindowTaskbarIconIdentity(hWnd, true);
            SetClassLongPtrW(hWnd, GCLP_HICONSM, reinterpret_cast<LONG_PTR>(state->hOrigClassIconSmall));
            SetClassLongPtrW(hWnd, GCLP_HICON, reinterpret_cast<LONG_PTR>(state->hOrigClassIconBig));
            SendMessageW(hWnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(state->hOrigIconSmall));
            SendMessageW(hWnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(state->hOrigIconBig));
            g_internalIconChange = false;
        }
        if (state->hTaskbarIconSmall) {
            DestroyIcon(state->hTaskbarIconSmall);
            state->hTaskbarIconSmall = nullptr;
        }
        if (state->hTaskbarIconBig) {
            DestroyIcon(state->hTaskbarIconBig);
            state->hTaskbarIconBig = nullptr;
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
    auto origProc = reinterpret_cast<DLGPROC>(GetPropW(hWnd, L"WindhawkOrigShellDlgProc"));
    if (origProc) {
        SetWindowLongPtrW(hWnd, DWLP_DLGPROC, reinterpret_cast<LONG_PTR>(origProc));
    }
    CleanupDialogState(hWnd, state);
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
            case kActionTranslateDone: {
                if (state && state->translateMode && (uint64_t)lParam == g_translateReqCounter.load()) {
                    int oldH = state->totalHeight;
                    state->isTranslating = false;
                    {
                        std::lock_guard<std::mutex> lock(g_translateMutex);
                        state->translatedText = g_pendingTranslateResult;
                    }
                    RecomputeDialogMetrics(state);
                    ResizeDialogPreservingBottomAnchor(state, oldH);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                break;
            }
            case kActionDeferredInputUpdate: {
                state->inputUpdateQueued = false;
                if (!state->hModernEdit || !IsWindow(state->hModernEdit)) break;
                state->caretVisible = true;
                std::wstring curText = GetEffectiveInputText(state->hModernEdit);
                if (state->translateMode) {
                    int oldH = state->totalHeight;
                    if (TrimCommandString(curText).empty()) {
                        state->isTranslating = false;
                        state->translatedText.clear();
                    }
                    RecomputeDialogMetrics(state);
                    ResizeDialogPreservingBottomAnchor(state, oldH);
                    InvalidateRect(hWnd, nullptr, FALSE);
                } else if (!state->suppressSuggestionUpdate) {
                    state->scrollOffset = 0;
                    UpdateAppSuggestionsForQuery(state, curText);
                    InvalidateRect(hWnd, nullptr, FALSE);
                } else {
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                if (GetForegroundWindow() == hWnd && !state->isBrowsingFile && GetFocus() != state->hModernEdit) {
                    SetFocus(state->hModernEdit);
                }
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
        case 0x00AE: // WM_NCUAHDRAWCAPTION (blocks uxtheme Aero Basic caption bar painting)
        case 0x00AF: // WM_NCUAHDRAWFRAME   (blocks uxtheme non-client frame painting)
            return 0;

        case WM_GETICON: {
            if (state) {
                if (wParam == ICON_BIG && state->hTaskbarIconBig) {
                    return reinterpret_cast<LRESULT>(state->hTaskbarIconBig);
                }
                if ((wParam == ICON_SMALL || wParam == ICON_SMALL2) && state->hTaskbarIconSmall) {
                    return reinterpret_cast<LRESULT>(state->hTaskbarIconSmall);
                }
            }
            break;
        }

        case WM_SETICON: {
            if (state && !g_internalIconChange) {
                if (wParam == ICON_BIG && state->hTaskbarIconBig) {
                    lParam = reinterpret_cast<LPARAM>(state->hTaskbarIconBig);
                } else if ((wParam == ICON_SMALL || wParam == ICON_SMALL2) && state->hTaskbarIconSmall) {
                    lParam = reinterpret_cast<LPARAM>(state->hTaskbarIconSmall);
                }
            }
            break;
        }

        case WM_STYLECHANGING: {
            if (wParam == GWL_STYLE && lParam && !g_internalIconChange) {
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
                RefreshDialogRunIcons(state);
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
                if (wp->x > -10000 && wp->y > -10000 && !state->openAnimActive && !state->closingAnimActive) {
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

        case WM_KEYDOWN:
        case WM_CHAR:
        case WM_IME_STARTCOMPOSITION: {
            if (state && state->hModernEdit && IsWindow(state->hModernEdit) && GetFocus() != state->hModernEdit) {
                SetFocus(state->hModernEdit);
                return SendMessageW(state->hModernEdit, uMsg, wParam, lParam);
            }
            break;
        }

        case WM_SHOWWINDOW:
            if (wParam == TRUE && state) {
                ApplyWindowTaskbarIconIdentity(hWnd, false);
                NotifyTaskbarRedrawWindowIcon(hWnd);
                ForceFocusModernEdit(hWnd, state);
            } else if (wParam == FALSE) {
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
                    int barTop = GetInputBarTopY(state);
                    RECT rcBar = { 0, barTop, GetDlgWidth(state), barTop + GetInputBarHeight(state) };
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
            if (wParam == kTimerOpenAnim && state) {
                if (IsIconic(hWnd)) {
                    KillTimer(hWnd, kTimerOpenAnim);
                    state->openAnimActive = false;
                    state->animAlpha = 1.0f;
                    return 0;
                }
                float t = std::clamp((float)(GetTickCount64() - state->openAnimStartTick) / 155.0f, 0.0f, 1.0f);
                float ease = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
                state->animAlpha = 0.08f + 0.92f * ease;
                int slideOff = (int)std::round((1.0f - ease) * DpiScaleS(14, state) * (state->anchoredTop ? -1.0f : 1.0f));
                int targetY = state->normalY + slideOff;
                if (t >= 1.0f) {
                    KillTimer(hWnd, kTimerOpenAnim);
                    state->openAnimActive = false;
                    state->animAlpha = 1.0f;
                    targetY = state->normalY;
                }
                SetWindowPos(hWnd, nullptr, state->normalX, targetY, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                if (state->hModernEdit && GetFocus() != state->hModernEdit && !state->isBrowsingFile) {
                    SetFocus(state->hModernEdit);
                }
                InvalidateRect(hWnd, nullptr, FALSE);
                UpdateWindow(hWnd);
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
            if (LOWORD(wParam) == kModernEditId) {
                if (state && (HIWORD(wParam) == EN_CHANGE || HIWORD(wParam) == EN_UPDATE)) {
                    ScheduleDeferredInputUpdate(state);
                } else if (state && HIWORD(wParam) == EN_KILLFOCUS) {
                    if (GetForegroundWindow() == hWnd && !state->isBrowsingFile &&
                        g_hActiveGearMenu.load() == nullptr && g_hActiveCustomSizeWnd.load() == nullptr) {
                        PostMessageW(hWnd, WM_NEXTDLGCTL, reinterpret_cast<WPARAM>(state->hModernEdit), TRUE);
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

            int newHoverRow = state->translateMode ? -1 : HitTestHistoryRow(state, pt);
            ActionButtonsLayout layout = GetActionButtonsLayout(state);
            RECT rcTrack = GetScrollbarTrackRect(state);
            bool newSbHover     = !state->translateMode && HasVerticalScrollbar(state) && PtInRect(&rcTrack, pt);
            bool newBrowseHover = PtInRect(&layout.rcBrowse, pt);
            bool newTransHover  = PtInRect(&layout.rcTranslate, pt);
            bool newBtnHover    = PtInRect(&layout.rcRun, pt);
            bool newCloseHover  = PtInRect(&layout.rcClose, pt);
            bool newGearHover   = PtInRect(&layout.rcGear, pt);
            bool newSrcHover    = state->translateMode && PtInRect(&layout.rcSrcLang, pt);
            bool newSwapHover   = state->translateMode && PtInRect(&layout.rcSwapLang, pt);
            bool newDstHover    = state->translateMode && PtInRect(&layout.rcDstLang, pt);
            int cardH           = GetHistoryCardHeight(state);
            int cardTop         = GetDetachedCardTopY(state);
            bool newTransCardHv = state->translateMode && cardH > 0 && !state->translatedText.empty() &&
                                  (pt.y >= cardTop && pt.y < cardTop + cardH && pt.x >= 0 && pt.x < GetDlgWidth(state));

            state->mousePt = pt;
            state->hasMousePt = true;

            if (newHoverRow != state->hoveredRow ||
                newSbHover != state->scrollbarHovered ||
                newBrowseHover != state->browseHovered ||
                newTransHover != state->translateBtnHovered ||
                newBtnHover != state->btnHovered ||
                newCloseHover != state->closeHovered ||
                newGearHover != state->gearHovered ||
                newSrcHover != state->srcLangHovered ||
                newSwapHover != state->swapLangHovered ||
                newDstHover != state->dstLangHovered ||
                newTransCardHv != state->transCardHovered) {
                state->hoveredRow       = newHoverRow;
                state->scrollbarHovered = newSbHover;
                state->browseHovered       = newBrowseHover;
                state->translateBtnHovered = newTransHover;
                state->btnHovered          = newBtnHover;
                state->closeHovered     = newCloseHover;
                state->gearHovered      = newGearHover;
                state->srcLangHovered   = newSrcHover;
                state->swapLangHovered  = newSwapHover;
                state->dstLangHovered   = newDstHover;
                state->transCardHovered = newTransCardHv;

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
                state->translateBtnHovered || state->translateBtnPressed ||
                state->btnHovered || state->btnPressed ||
                state->closeHovered || state->closePressed ||
                state->gearHovered || state->gearPressed ||
                state->srcLangHovered || state->srcLangPressed ||
                state->swapLangHovered || state->swapLangPressed ||
                state->dstLangHovered || state->dstLangPressed ||
                state->transCardHovered || state->transCardPressed) {
                state->hoveredRow       = -1;
                state->pressedRow       = -1;
                state->hasMousePt       = false;
                state->scrollbarHovered = false;
                state->browseHovered       = false;
                state->browsePressed       = false;
                state->translateBtnHovered = false;
                state->translateBtnPressed = false;
                state->btnHovered       = false;
                state->btnPressed       = false;
                state->closeHovered     = false;
                state->closePressed     = false;
                state->gearHovered      = false;
                state->gearPressed      = false;
                state->srcLangHovered   = false;
                state->srcLangPressed   = false;
                state->swapLangHovered  = false;
                state->swapLangPressed  = false;
                state->dstLangHovered   = false;
                state->dstLangPressed   = false;
                state->transCardHovered = false;
                state->transCardPressed = false;
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

            if (state->translateMode) {
                if (PtInRect(&layout.rcSrcLang, pt)) {
                    state->srcLangPressed = true;
                    SetCapture(hWnd);
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
                if (PtInRect(&layout.rcSwapLang, pt)) {
                    state->swapLangPressed = true;
                    SetCapture(hWnd);
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
                if (PtInRect(&layout.rcDstLang, pt)) {
                    state->dstLangPressed = true;
                    SetCapture(hWnd);
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
                int cardH = GetHistoryCardHeight(state);
                int cardTop = GetDetachedCardTopY(state);
                if (cardH > 0 && !state->translatedText.empty() && pt.y >= cardTop && pt.y < cardTop + cardH) {
                    state->transCardPressed = true;
                    SetCapture(hWnd);
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
            if (PtInRect(&layout.rcTranslate, pt)) {
                state->translateBtnPressed = true;
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

            if (state->srcLangPressed) {
                state->srcLangPressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&layout.rcSrcLang, pt)) {
                    OpenTranslateLanguagePopup(state, true);
                }
                return 0;
            }
            if (state->swapLangPressed) {
                state->swapLangPressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&layout.rcSwapLang, pt)) {
                    std::wstring oldSrc = state->translateSourceLang;
                    std::wstring oldDst = state->translateTargetLang;
                    state->translateSourceLang = oldDst;
                    state->translateTargetLang = (_wcsicmp(oldSrc.c_str(), L"auto") == 0)
                        ? (_wcsicmp(oldDst.c_str(), L"vi") == 0 ? L"en" : L"vi")
                        : oldSrc;
                    Wh_SetStringValue(GetUserOverrideKey(L"translateSourceLang").c_str(), state->translateSourceLang.c_str());
                    Wh_SetStringValue(GetUserOverrideKey(L"translateTargetLang").c_str(), state->translateTargetLang.c_str());
                    if (!state->translatedText.empty() && !state->isTranslating && state->hModernEdit) {
                        std::wstring prevTrans = state->translatedText;
                        SetWindowTextW(state->hModernEdit, prevTrans.c_str());
                        SendMessageW(state->hModernEdit, EM_SETSEL, prevTrans.size(), prevTrans.size());
                    }
                    if (!TrimCommandString(GetEffectiveInputText(state->hModernEdit)).empty()) {
                        TriggerTranslationRequest(state);
                    } else {
                        InvalidateRect(hWnd, nullptr, FALSE);
                    }
                }
                return 0;
            }
            if (state->dstLangPressed) {
                state->dstLangPressed = false;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, FALSE);
                if (PtInRect(&layout.rcDstLang, pt)) {
                    OpenTranslateLanguagePopup(state, false);
                }
                return 0;
            }
            if (state->transCardPressed) {
                state->transCardPressed = false;
                ReleaseCapture();
                int cardH = GetHistoryCardHeight(state);
                int cardTop = GetDetachedCardTopY(state);
                if (cardH > 0 && !state->translatedText.empty() && pt.y >= cardTop && pt.y < cardTop + cardH) {
                    CopyTextToClipboard(hWnd, state->translatedText);
                    state->translateCopiedFeedback = true;
                }
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
            if (state->translateBtnPressed) {
                state->translateBtnPressed = false;
                ReleaseCapture();
                if (PtInRect(&layout.rcTranslate, pt)) {
                    int oldH = state->totalHeight;
                    state->translateMode = !state->translateMode;
                    state->isTranslating = false;
                    state->translatedText.clear();
                    state->translateCopiedFeedback = false;
                    std::wstring curText = GetEffectiveInputText(state->hModernEdit);
                    if (!state->translateMode) {
                        UpdateAppSuggestionsForQuery(state, curText);
                    } else {
                        state->showingSuggestions = false;
                    }
                    RecomputeDialogMetrics(state);
                    ResizeDialogPreservingBottomAnchor(state, oldH);
                    if (state->translateMode && !TrimCommandString(curText).empty()) {
                        TriggerTranslationRequest(state);
                    }
                    if (state->hModernEdit && IsWindow(state->hModernEdit)) {
                        SetFocus(state->hModernEdit);
                    }
                }
                InvalidateRect(hWnd, nullptr, TRUE);
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

        case WM_RBUTTONDOWN: {
            if (!state || state->translateMode || state->showingSuggestions) return 0;
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            int row = HitTestHistoryRow(state, pt);
            if (row >= 0 && row < (int)state->history.size()) {
                state->selectedRow = row;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            if (!state || state->translateMode || state->showingSuggestions) return 0;
            POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
            int row = HitTestHistoryRow(state, pt);
            if (row >= 0 && row < (int)state->history.size()) {
                state->selectedRow = row;
                InvalidateRect(hWnd, nullptr, FALSE);
                std::wstring targetCmd = state->history[row].cmd;
                ExtraLocalizedStrings ex = GetExtraLocalizedStrings(state->settings.language);

                std::vector<FluentMenuItem> ctxItems;
                FluentMenuItem delItem;
                delItem.id    = 799;
                delItem.label = ex.deleteHistoryItem;
                delItem.icon  = FluentMenuIcon::ClearTrash;
                ctxItems.push_back(std::move(delItem));

                POINT ptScreen = pt;
                ClientToScreen(hWnd, &ptScreen);
                state->isBrowsingFile = true;
                g_isBrowsingFile.store(true);
                UINT cmd = RunFluentPopupMenu(state, std::move(ctxItems), ptScreen);
                g_isBrowsingFile.store(false);
                if (!IsWindow(hWnd) || g_hActiveRunDlg.load() != hWnd) return 0;
                state->isBrowsingFile = false;

                if (cmd == 799) {
                    int oldH = state->totalHeight;
                    RemoveCommandFromHistoryStorage(targetCmd);
                    state->hoveredRow = -1;
                    state->selectedRow = -1;
                    LoadHistoryItems(state);
                    RecomputeDialogMetrics(state);
                    ResizeDialogPreservingBottomAnchor(state, oldH);
                    InvalidateRect(hWnd, nullptr, TRUE);
                }
                ForceFocusModernEdit(hWnd, state);
            }
            return 0;
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
                        {
                            std::lock_guard<std::mutex> lock(g_opticalCacheMutex);
                            ReleaseOpticalCacheBitmap(g_dlgOpticalCache);
                        }
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

                        ApplyFluentTextRenderingToRT(pRT);

                        int gapH    = GetDetachedGap(state);
                        int cardH   = GetHistoryCardHeight(state);
                        int barH    = GetInputBarHeight(state);
                        int cardTop = GetDetachedCardTopY(state);
                        int barTop  = GetInputBarTopY(state);
                        float rimInset = DpiScaleSF(0.75f, state);
                        D2D1_RECT_F rcHistCard = D2D1::RectF(rimInset, (float)cardTop + rimInset,
                                                             (float)w - rimInset, (float)(cardTop + cardH) - rimInset);
                        D2D1_RECT_F rcBarPill  = D2D1::RectF(rimInset, (float)barTop + rimInset,
                                                             (float)w - rimInset, (float)(barTop + barH) - rimInset);
                        float maxPillCorner = DpiScaleSF(27.0f, state);
                        float barCornerR  = (std::min)(maxPillCorner, (rcBarPill.bottom - rcBarPill.top) * 0.5f);
                        float histCornerR = (cardH > 0)
                            ? (std::min)(maxPillCorner, (rcHistCard.bottom - rcHistCard.top) * 0.5f)
                            : 0.0f;

                        // Clear entire window surface to 100% transparent so pill corners and detached gap are see-through
                        pRT->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

                        auto fillActiveShapes = [&](ID2D1Brush* pBrush) {
                            if (!pBrush) return;
                            if (cardH > 0) {
                                pRT->FillRoundedRectangle(D2D1::RoundedRect(rcHistCard, histCornerR, histCornerR), pBrush);
                            }
                            pRT->FillRoundedRectangle(D2D1::RoundedRect(rcBarPill, barCornerR, barCornerR), pBrush);
                        };

                        // 1. Paint Surface Backdrop strictly inside the Detached Card and Pill Input Bar
                        if (highContrast) {
                            COLORREF winBg = GetSysColor(COLOR_WINDOW);
                            ID2D1SolidColorBrush* pHcBr = nullptr;
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(
                                    D2D1::ColorF(GetRValue(winBg) / 255.0f, GetGValue(winBg) / 255.0f, GetBValue(winBg) / 255.0f, 1.0f), &pHcBr))) {
                                fillActiveShapes(pHcBr);
                                pHcBr->Release();
                            }
                        } else if (surface == SurfaceTheme::Glass) {
                            int glassPct = std::clamp(state->settings.glassOpacity, -100, 100);
                            DrawOpticalLiquidGlassBackdrop(pRT, hWnd, w, h, state->topAreaHeight, gapH,
                                                           effectiveDpi, glassPct, isLight, false, 0,
                                                           state->normalX, state->normalY, state->anchoredTop);
                        } else if (surface == SurfaceTheme::Blur) {
                            // Translucent, vibrant Windows 11 Acrylic Blur matching the desktop right-click menu (Image 2)!
                            int blurOp = std::clamp(state->settings.blurOpacity, 0, 255);
                            DrawOpticalLiquidGlassBackdrop(pRT, hWnd, w, h, state->topAreaHeight, gapH,
                                                           effectiveDpi, -(1000 + blurOp), isLight, false, 0,
                                                           state->normalX, state->normalY, state->anchoredTop);
                        } else {
                            ID2D1SolidColorBrush* pSolidBr = nullptr;
                            if (SUCCEEDED(pRT->CreateSolidColorBrush(
                                    isLight ? D2D1::ColorF(0.96f, 0.96f, 0.98f, 1.0f)
                                            : D2D1::ColorF(0.12f, 0.12f, 0.15f, 1.0f), &pSolidBr))) {
                                fillActiveShapes(pSolidBr);
                                pSolidBr->Release();
                            }
                        }

                        // 2. Create Brushes & Round-Cap Stroke Style (Crisp Image 1 Fluent Typography - ZERO muddy drop shadow!)
                        D2D1_COLOR_F colPrimary, colSecondary, colTime, colHint;
                        if (highContrast) {
                            COLORREF fg = GetSysColor(COLOR_WINDOWTEXT);
                            colPrimary = colSecondary = colTime = colHint =
                                D2D1::ColorF(GetRValue(fg) / 255.0f, GetGValue(fg) / 255.0f, GetBValue(fg) / 255.0f, 1.0f);
                        } else {
                            // Exact Image 1 palette: pure white Semibold heading & warm silver-gray Regular description
                            colPrimary   = isLight ? D2D1::ColorF(0.06f, 0.07f, 0.09f, 1.00f) : D2D1::ColorF(1.00f, 1.00f, 1.00f, 1.00f);
                            colSecondary = isLight ? D2D1::ColorF(0.24f, 0.26f, 0.30f, 0.90f) : D2D1::ColorF(0.84f, 0.85f, 0.87f, 0.90f);
                            colTime      = isLight ? D2D1::ColorF(0.26f, 0.28f, 0.33f, 0.84f) : D2D1::ColorF(0.82f, 0.84f, 0.87f, 0.82f);
                            colHint      = isLight ? D2D1::ColorF(0.30f, 0.33f, 0.38f, 0.78f) : D2D1::ColorF(0.80f, 0.82f, 0.86f, 0.76f);
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

                        // 3. Create DirectWrite Text Formats using Fluent Variable Typography (Image 1)
                        const wchar_t* fluentFont = GetPreferredFluentFontFamily();
                        IDWriteTextFormat* pFmtRow = nullptr;
                        IDWriteTextFormat* pFmtTime = nullptr;
                        IDWriteTextFormat* pFmtInput = nullptr;
                        IDWriteTextFormat* pFmtBtn = nullptr;

                        g_pDWriteFactory->CreateTextFormat(
                            fluentFont, nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(13.5f, state), L"", &pFmtRow);
                        g_pDWriteFactory->CreateTextFormat(
                            fluentFont, nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(12.0f, state), L"", &pFmtTime);
                        g_pDWriteFactory->CreateTextFormat(
                            fluentFont, nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(13.5f, state), L"", &pFmtInput);
                        g_pDWriteFactory->CreateTextFormat(
                            fluentFont, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
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
                            pFmtInput->SetWordWrapping(
                                state->translateMode ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);
                        }
                        if (pFmtBtn) {
                            pFmtBtn->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                            pFmtBtn->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                            pFmtBtn->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                        }

                        int padTop = GetListPadTop(state);
                        int rowHInt = GetRowHeight(state);
                        float textLeftX = DpiScaleSF(50.0f, state);
                        bool hasScrollbar = !state->translateMode && HasVerticalScrollbar(state);
                        float sbReserve = hasScrollbar ? DpiScaleSF(12.0f, state) : 0.0f;

                        ExtraLocalizedStrings ex = GetExtraLocalizedStrings(state->settings.language);

                        // 4. Draw Either Translate Result Card OR History / App Suggestion Rows
                        if (state->translateMode) {
                            if (cardH > 0 && (state->isTranslating || !state->translatedText.empty())) {
                                bool isVi = IsDialogVietnameseUI(state);
                                bool isCompact = state->settings.minimalMode;
                                float sidePad = DpiScaleSF(isCompact ? 16.0f : 22.0f, state);
                                float cTopF = (float)cardTop;
                                if (state->transCardHovered || state->transCardPressed) {
                                    float padX = DpiScaleSF(8.0f, state);
                                    float padY = DpiScaleSF(6.0f, state);
                                    D2D1_RECT_F rcHl = D2D1::RectF(padX, cTopF + padY, (float)w - padX, cTopF + (float)cardH - padY);
                                    float hlR = (std::min)(DpiScaleSF(20.0f, state), (rcHl.bottom - rcHl.top) * 0.5f);
                                    D2D1_COLOR_F rowTint = isLight
                                        ? D2D1::ColorF(0.0f, 0.0f, 0.0f, state->transCardPressed ? 0.10f : 0.045f)
                                        : D2D1::ColorF(1.0f, 1.0f, 1.0f, state->transCardPressed ? 0.14f : 0.07f);
                                    DrawD2DGlassButton(pRT, rcHl, hlR, effectiveDpi, rowTint, state->transCardHovered, state->transCardPressed, isLight);
                                }

                                std::wstring srcDisp = isCompact
                                    ? GetCompactTranslateLangDisplayName(state->translateSourceLang, isVi)
                                    : std::wstring(GetTranslateLangDisplayName(state->translateSourceLang, isVi));
                                std::wstring dstDisp = isCompact
                                    ? GetCompactTranslateLangDisplayName(state->translateTargetLang, isVi)
                                    : std::wstring(GetTranslateLangDisplayName(state->translateTargetLang, isVi));
                                std::wstring headerStr = srcDisp + L" → " + dstDisp;
                                if (state->translateCopiedFeedback) {
                                    headerStr += ex.copiedFeedback;
                                } else if (!state->isTranslating) {
                                    headerStr += ex.clickToCopyHint;
                                }

                                IDWriteTextFormat* pFmtHeader = nullptr;
                                if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
                                        fluentFont, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                        DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(isCompact ? 11.2f : 12.0f, state), L"", &pFmtHeader)) && pFmtHeader) {
                                    pFmtHeader->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                                    pFmtHeader->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                                    D2D1_RECT_F rcHdr = D2D1::RectF(sidePad, cTopF + DpiScaleSF(isCompact ? 8.0f : 10.0f, state),
                                                                    (float)w - sidePad, cTopF + DpiScaleSF(isCompact ? 28.0f : 32.0f, state));
                                    if (pBrSecondary) {
                                        pRT->DrawText(headerStr.c_str(), (UINT32)headerStr.size(), pFmtHeader, rcHdr, pBrSecondary, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                                    }
                                    pFmtHeader->Release();
                                }

                                std::wstring bodyStr = state->isTranslating
                                    ? ex.translatingStatus
                                    : state->translatedText;

                                IDWriteTextFormat* pFmtTransBody = nullptr;
                                if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
                                        fluentFont, nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
                                        DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(isCompact ? 13.0f : 14.0f, state), L"", &pFmtTransBody)) && pFmtTransBody) {
                                    pFmtTransBody->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
                                    D2D1_RECT_F rcBody = D2D1::RectF(sidePad, cTopF + DpiScaleSF(isCompact ? 30.0f : 34.0f, state),
                                                                     (float)w - sidePad, cTopF + (float)cardH - DpiScaleSF(isCompact ? 10.0f : 12.0f, state));
                                    if (pBrPrimary) {
                                        pRT->DrawText(bodyStr.c_str(), (UINT32)bodyStr.size(),
                                                      pFmtTransBody, rcBody, state->isTranslating ? pBrSecondary : pBrPrimary,
                                                      D2D1_DRAW_TEXT_OPTIONS_CLIP);
                                    }
                                    pFmtTransBody->Release();
                                }
                            }
                        } else {
                            for (int visIdx = 0; visIdx < visRows; ++visIdx) {
                                int i = state->scrollOffset + visIdx;
                                if (i < 0 || i >= (int)activeRows.size()) break;

                                float rowTop = (float)(cardTop + padTop + visIdx * rowHInt);
                                float rowH   = (float)rowHInt;
                                bool isHovered  = (i == state->hoveredRow);
                                bool isPressed  = (i == state->pressedRow);
                                bool isSelected = (i == state->selectedRow);

                                if (isHovered || isPressed || isSelected) {
                                    float padX = DpiScaleSF(10.0f, state);
                                    float padY = DpiScaleSF(2.0f, state);
                                    D2D1_RECT_F rcPill = D2D1::RectF(padX, rowTop + padY, (float)w - padX - sbReserve, rowTop + rowH - padY);
                                    float rowPillRadius = (rcPill.bottom - rcPill.top) * 0.5f;
                                    if (surface == SurfaceTheme::Glass) {
                                        D2D1_COLOR_F rowTint = isLight
                                            ? D2D1::ColorF(0.0f, 0.0f, 0.0f, isPressed ? 0.14f : (isSelected ? 0.09f : 0.055f))
                                            : D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.24f : 0.13f);
                                        DrawD2DGlassButton(pRT, rcPill, rowPillRadius, effectiveDpi, rowTint, isHovered, isPressed, isLight);
                                    } else {
                                        ID2D1SolidColorBrush* pHlBr = nullptr;
                                        D2D1_COLOR_F hlCol = isLight
                                            ? D2D1::ColorF(0.0f, 0.0f, 0.0f, isPressed ? 0.12f : 0.07f)
                                            : D2D1::ColorF(1.0f, 1.0f, 1.0f, isPressed ? 0.16f : 0.10f);
                                        if (SUCCEEDED(pRT->CreateSolidColorBrush(hlCol, &pHlBr))) {
                                            pRT->FillRoundedRectangle(D2D1::RoundedRect(rcPill, rowPillRadius, rowPillRadius), pHlBr);
                                            pHlBr->Release();
                                        }
                                    }
                                }

                                float rightLimit = (float)w - DpiScaleSF(20.0f, state) - sbReserve;
                                const auto& rowItem = activeRows[i];

                                if (!rowItem.isSuggestion && state->settings.showTimestamp && pFmtTime && pBrTime && !rowItem.timeStr.empty()) {
                                    rightLimit = (float)w - DpiScaleSF(44.0f, state) - sbReserve;
                                    const std::wstring& tStr = rowItem.timeStr;
                                    IDWriteTextLayout* pTimeLayout = nullptr;
                                    if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                            tStr.c_str(), (UINT32)tStr.size(), pFmtTime, DpiScaleSF(160.0f, state), rowH, &pTimeLayout))) {
                                        DWRITE_TEXT_METRICS tm = {};
                                        pTimeLayout->GetMetrics(&tm);
                                        float timeRight = (float)w - DpiScaleSF(44.0f, state) - sbReserve;
                                        pRT->DrawTextLayout(D2D1::Point2F(timeRight - DpiScaleSF(160.0f, state), rowTop),
                                                            pTimeLayout, pBrTime);
                                        rightLimit = timeRight - tm.widthIncludingTrailingWhitespace - DpiScaleSF(12.0f, state);
                                        pTimeLayout->Release();
                                    }
                                    DrawD2DClockVector(pRT, pBrTime, pRoundStroke,
                                                       (float)w - DpiScaleSF(28.0f, state) - sbReserve, rowTop + rowH * 0.5f, effectiveDpi);
                                } else if (rowItem.isSuggestion && pBrTime && pRoundStroke) {
                                    rightLimit = (float)w - DpiScaleSF(40.0f, state) - sbReserve;
                                    DrawD2DReturnArrowVector(pRT, pBrTime, pRoundStroke,
                                                             (float)w - DpiScaleSF(27.0f, state) - sbReserve, rowTop + rowH * 0.5f, effectiveDpi);
                                }

                                if (pFmtRow && pBrPrimary) {
                                    const std::wstring& cmdStr = rowItem.cmd;
                                    const std::wstring& appStr = rowItem.appName;
                                    std::wstring fullStr = cmdStr;
                                    if (!appStr.empty() && _wcsicmp(cmdStr.c_str(), appStr.c_str()) != 0) {
                                        fullStr += L"   –   " + appStr;
                                    }

                                    float availW = (std::max)(DpiScaleSF(40.0f, state), rightLimit - textLeftX);
                                    IDWriteTextLayout* pRowLayout = nullptr;
                                    if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                            fullStr.c_str(), (UINT32)fullStr.size(), pFmtRow, availW, rowH, &pRowLayout))) {
                                        DWRITE_TEXT_RANGE cmdRange = { 0, (UINT32)cmdStr.size() };
                                        pRowLayout->SetFontWeight(DWRITE_FONT_WEIGHT_SEMI_BOLD, cmdRange);

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
                        }

                        // 4b. Draw Sleek Fluent Vertical Scrollbar inside Detached Card
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

                        // 4c. Draw Language Selector Bar when Translate Mode is active
                        if (state->translateMode) {
                            bool isVi = IsDialogVietnameseUI(state);
                            bool isCompact = state->settings.minimalMode;
                            auto drawLangPill = [&](RECT rWin, const std::wstring& text, bool hov, bool prs, bool isCenterIcon) {
                                D2D1_RECT_F rcP = D2D1::RectF((float)rWin.left, (float)rWin.top, (float)rWin.right, (float)rWin.bottom);
                                float rad = (rcP.bottom - rcP.top) * 0.5f;
                                D2D1_COLOR_F tint = isLight
                                    ? D2D1::ColorF(0.05f, 0.08f, 0.14f, prs ? 0.16f : (hov ? 0.10f : 0.05f))
                                    : D2D1::ColorF(0.88f, 0.94f, 1.00f, prs ? 0.20f : (hov ? 0.14f : 0.07f));
                                DrawD2DGlassButton(pRT, rcP, rad, effectiveDpi, tint, hov, prs, isLight);

                                IDWriteTextFormat* pFmtLang = nullptr;
                                if (SUCCEEDED(g_pDWriteFactory->CreateTextFormat(
                                        fluentFont, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                        DWRITE_FONT_STRETCH_NORMAL, GetFontSizeSF(isCompact ? 11.2f : 12.0f, state), L"", &pFmtLang)) && pFmtLang) {
                                    pFmtLang->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                                    pFmtLang->SetTextAlignment((isCenterIcon || isCompact) ? DWRITE_TEXT_ALIGNMENT_CENTER : DWRITE_TEXT_ALIGNMENT_LEADING);
                                    pFmtLang->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                                    D2D1_RECT_F rcTxt = isCenterIcon
                                        ? rcP
                                        : D2D1::RectF(rcP.left + DpiScaleSF(isCompact ? 6.0f : 11.0f, state), rcP.top,
                                                      rcP.right - DpiScaleSF(isCompact ? 6.0f : 8.0f, state), rcP.bottom);
                                    if (pBrPrimary) {
                                        pRT->DrawText(text.c_str(), (UINT32)text.size(), pFmtLang, rcTxt, pBrPrimary, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                                    }
                                    pFmtLang->Release();
                                }
                            };

                            std::wstring srcLabel = isCompact
                                ? (GetCompactTranslateLangDisplayName(state->translateSourceLang, isVi) + L" ▾")
                                : (std::wstring(ex.fromPrefix) + GetTranslateLangDisplayName(state->translateSourceLang, isVi) + L" ▾");
                            std::wstring dstLabel = isCompact
                                ? (GetCompactTranslateLangDisplayName(state->translateTargetLang, isVi) + L" ▾")
                                : (std::wstring(ex.toPrefix) + GetTranslateLangDisplayName(state->translateTargetLang, isVi) + L" ▾");

                            drawLangPill(layout.rcSrcLang,  srcLabel, state->srcLangHovered,  state->srcLangPressed,  false);
                            drawLangPill(layout.rcSwapLang, L"⇄",     state->swapLangHovered, state->swapLangPressed, true);
                            drawLangPill(layout.rcDstLang,  dstLabel, state->dstLangHovered,  state->dstLangPressed,  false);
                        }

                        // 5. Draw Input Text inside the Pill Input Bar (supports multi-line wrap in Translate Mode)
                        RECT rcEditWin = layout.rcEdit;
                        D2D1_RECT_F rcEdit = D2D1::RectF((float)rcEditWin.left, (float)rcEditWin.top,
                                                         (float)rcEditWin.right, (float)rcEditWin.bottom);
                        float editW = rcEdit.right - rcEdit.left;
                        float editH = rcEdit.bottom - rcEdit.top;

                        DWORD selStart = 0, selEnd = 0;
                        std::wstring displayStr = GetEffectiveInputText(state->hModernEdit, &selStart, &selEnd);
                        UINT32 textLen = (UINT32)displayStr.size();
                        if (selStart > textLen) selStart = textLen;
                        if (selEnd > textLen)   selEnd = textLen;

                        pRT->PushAxisAlignedClip(rcEdit, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

                        float caretXPos = 0.0f;
                        float caretYPos = rcEdit.top;
                        float caretH    = editH;
                        float scrollOffsetX = 0.0f;

                        const wchar_t* placeholderStr = state->translateMode
                            ? (state->settings.minimalMode ? ex.compactTranslatePlaceholder : ex.translatePlaceholder)
                            : (state->settings.minimalMode ? ex.compactPlaceholder : state->i18n.placeholder);

                        if (textLen == 0) {
                            if (pFmtInput && pBrHint) {
                                pRT->DrawText(placeholderStr, (UINT32)wcslen(placeholderStr),
                                              pFmtInput, rcEdit, pBrHint);
                            }
                            if (state->translateMode) {
                                caretH = (std::min)(editH, DpiScaleSF(24.0f, state));
                                caretYPos = rcEdit.top + (editH - caretH) * 0.5f;
                            }
                        } else if (pFmtInput && pBrPrimary) {
                            float layoutMaxW = state->translateMode ? (std::max)(80.0f, editW) : 10000.0f;
                            float layoutMaxH = editH;
                            IDWriteTextLayout* pInputLayout = nullptr;
                            if (SUCCEEDED(g_pDWriteFactory->CreateTextLayout(
                                    displayStr.c_str(), textLen, pFmtInput, layoutMaxW, layoutMaxH, &pInputLayout))) {
                                FLOAT hitX = 0.0f, hitY = 0.0f;
                                DWRITE_HIT_TEST_METRICS htm = {};
                                pInputLayout->HitTestTextPosition(selEnd, FALSE, &hitX, &hitY, &htm);
                                caretXPos = hitX;
                                if (state->translateMode) {
                                    caretYPos = rcEdit.top + hitY;
                                    caretH    = (htm.height > 0.0f) ? htm.height : DpiScaleSF(20.0f, state);
                                }

                                if (!state->translateMode) {
                                    float scrollPad = DpiScaleSF(6.0f, state);
                                    if (caretXPos > editW - scrollPad) {
                                        scrollOffsetX = caretXPos - (editW - scrollPad);
                                    }
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
                                        if (!state->translateMode || std::fabs(y1 - y0) < 2.0f) {
                                            float padY = state->translateMode ? 0.0f : DpiScaleSF(2.0f, state);
                                            float topSel = state->translateMode ? (rcEdit.top + y0) : (rcEdit.top + padY);
                                            float botSel = state->translateMode ? (rcEdit.top + y0 + m0.height) : (rcEdit.bottom - padY);
                                            pRT->FillRectangle(
                                                D2D1::RectF(rcEdit.left + x0 - scrollOffsetX, topSel,
                                                            rcEdit.left + x1 - scrollOffsetX, botSel),
                                                pSelBr);
                                        } else {
                                            pRT->FillRectangle(rcEdit, pSelBr);
                                        }
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
                            float cx = rcEdit.left + caretXPos - scrollOffsetX + DpiScaleSF(0.8f, state);
                            cx = std::clamp(cx, rcEdit.left + DpiScaleSF(0.8f, state), rcEdit.right - DpiScaleSF(1.5f, state));
                            float padY = (state->translateMode && textLen > 0) ? DpiScaleSF(1.5f, state) : DpiScaleSF(3.5f, state);
                            pRT->DrawLine(D2D1::Point2F(cx, caretYPos + padY),
                                          D2D1::Point2F(cx, caretYPos + caretH - padY),
                                          pBrPrimary, DpiScaleSF(1.25f, state));
                        }

                        pRT->PopAxisAlignedClip();

                        // 6. Draw Action Buttons as Capsule Pills inside the Pill Input Bar
                        enum class ActionBtnKind { Browse, Translate, Run, Exit, Gear };
                        auto drawActionButton = [&](RECT rWin, const wchar_t* label, ActionBtnKind kind,
                                                    bool hovered, bool pressed) {
                            if (rWin.right <= rWin.left || rWin.bottom <= rWin.top) return;
                            D2D1_RECT_F rcBtn = D2D1::RectF((float)rWin.left, (float)rWin.top,
                                                            (float)rWin.right, (float)rWin.bottom);
                            float btnRadius = (rcBtn.bottom - rcBtn.top) * 0.5f;
                            D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rcBtn, btnRadius, btnRadius);

                            D2D1_COLOR_F contentCol = {};

                            if ((surface == SurfaceTheme::Glass || surface == SurfaceTheme::Blur) && !highContrast) {
                                D2D1_COLOR_F glassTint = {};
                                if (kind == ActionBtnKind::Browse || kind == ActionBtnKind::Gear ||
                                    (kind == ActionBtnKind::Translate && !state->translateMode)) {
                                    glassTint = isLight
                                        ? D2D1::ColorF(0.06f, 0.08f, 0.14f, pressed ? 0.16f : (hovered ? 0.10f : 0.05f))
                                        : D2D1::ColorF(0.85f, 0.92f, 1.00f, pressed ? 0.20f : (hovered ? 0.13f : 0.07f));
                                } else if (kind == ActionBtnKind::Translate && state->translateMode) {
                                    glassTint = isLight
                                        ? D2D1::ColorF(0.08f, 0.52f, 0.78f, pressed ? 0.36f : (hovered ? 0.28f : 0.20f))
                                        : D2D1::ColorF(0.16f, 0.62f, 0.94f, pressed ? 0.42f : (hovered ? 0.34f : 0.24f));
                                } else if (kind == ActionBtnKind::Run) {
                                    glassTint = isLight
                                        ? D2D1::ColorF(0.06f, 0.45f, 0.90f, pressed ? 0.34f : (hovered ? 0.24f : 0.14f))
                                        : D2D1::ColorF(0.12f, 0.54f, 0.98f, pressed ? 0.40f : (hovered ? 0.30f : 0.18f));
                                } else {
                                    glassTint = isLight
                                        ? D2D1::ColorF(0.82f, 0.16f, 0.26f, pressed ? 0.30f : (hovered ? 0.20f : 0.12f))
                                        : D2D1::ColorF(0.92f, 0.22f, 0.32f, pressed ? 0.36f : (hovered ? 0.26f : 0.16f));
                                }
                                DrawD2DGlassButton(pRT, rcBtn, btnRadius, effectiveDpi, glassTint, hovered, pressed, isLight);
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
                                bool iconOnly = (label == nullptr || label[0] == L'\0');
                                float iconCx = iconOnly
                                    ? ((rcBtn.left + rcBtn.right) * 0.5f)
                                    : (rcBtn.right - DpiScaleSF(16.0f, state));
                                float iconCy = (rcBtn.top + rcBtn.bottom) * 0.5f;

                                if (!iconOnly && pFmtBtn && label) {
                                    D2D1_RECT_F rcText = D2D1::RectF(
                                        rcBtn.left + DpiScaleSF(13.0f, state),
                                        rcBtn.top,
                                        rcBtn.right - DpiScaleSF(26.0f, state),
                                        rcBtn.bottom);
                                    pRT->DrawText(label, (UINT32)wcslen(label), pFmtBtn, rcText, pContentBr);
                                }

                                if (kind == ActionBtnKind::Gear) {
                                    DrawD2DGearVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, effectiveDpi);
                                } else if (kind == ActionBtnKind::Translate) {
                                    DrawD2DGoogleTranslateVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, effectiveDpi);
                                } else if (kind == ActionBtnKind::Browse) {
                                    DrawD2DFolderVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, effectiveDpi);
                                } else if (kind == ActionBtnKind::Run) {
                                    DrawD2DReturnArrowVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, effectiveDpi);
                                } else {
                                    DrawD2DCloseCrossVector(pRT, pContentBr, pRoundStroke, iconCx, iconCy, effectiveDpi);
                                }

                                pContentBr->Release();
                            }
                        };

                        if (!state->translateMode && !state->settings.minimalMode) {
                            drawActionButton(layout.rcBrowse, state->i18n.browseBtn, ActionBtnKind::Browse,
                                             state->browseHovered, state->browsePressed);
                        }
                        const wchar_t* runBtnLabel = state->settings.minimalMode
                            ? nullptr
                            : (state->translateMode ? ex.translateBtnLabel : state->i18n.runBtn);
                        drawActionButton(layout.rcRun, runBtnLabel, ActionBtnKind::Run,
                                         state->btnHovered, state->btnPressed);
                        if (!state->translateMode && !state->settings.minimalMode) {
                            drawActionButton(layout.rcClose, state->i18n.exitBtn, ActionBtnKind::Exit,
                                             state->closeHovered, state->closePressed);
                        }
                        // Translate button is always icon-only and sits to the right of Run and left of Gear!
                        drawActionButton(layout.rcTranslate, nullptr, ActionBtnKind::Translate,
                                         state->translateBtnHovered, state->translateBtnPressed);
                        drawActionButton(layout.rcGear, nullptr, ActionBtnKind::Gear,
                                         state->gearHovered, state->gearPressed);

                        // 7. Draw Synchronized Pill Glass Rim on both History/Suggestions Card and Pill Input Bar
                        if (!highContrast) {
                            if (cardH > 0) {
                                DrawD2DGlassRim(pRT, rcHistCard, histCornerR, effectiveDpi, isLight, 1.35f);
                            }
                            DrawD2DGlassRim(pRT, rcBarPill, barCornerR, effectiveDpi, isLight, 1.35f);
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
                    }

                    // 8. Draw Native App Icons onto the premultiplied DIB and preserve opaque destination alpha
                    auto drawIconPreservingDestAlpha = [&](int ix, int iy, HICON hIco, int sz) {
                        if (!hIco || sz <= 0) return;
                        DrawIconEx(hMemDC, ix, iy, hIco, sz, sz, 0, nullptr, DI_NORMAL);
                        GdiFlush();
                        int x0 = std::clamp(ix, 0, w);
                        int y0 = std::clamp(iy, 0, h);
                        int x1 = std::clamp(ix + sz, 0, w);
                        int y1 = std::clamp(iy + sz, 0, h);
                        for (int py = y0; py < y1; ++py) {
                            DWORD* row = &state->cachedDibPixels[(size_t)py * w];
                            for (int px = x0; px < x1; ++px) {
                                row[px] |= 0xFF000000u;
                            }
                        }
                    };

                    int iconSize = DpiScaleS(18, state);
                    int iconX    = DpiScaleS(22, state);
                    int cardTop  = GetDetachedCardTopY(state);
                    int barTop   = GetInputBarTopY(state);
                    int padTop   = GetListPadTop(state);
                    int rowHInt  = GetRowHeight(state);
                    int barH     = GetInputBarHeight(state);

                    if (!state->translateMode) {
                        for (int visIdx = 0; visIdx < visRows; ++visIdx) {
                            int i = state->scrollOffset + visIdx;
                            if (i >= 0 && i < (int)activeRows.size()) {
                                if (!activeRows[i].hIcon && activeRows[i].isSuggestion && state->showingSuggestions) {
                                    std::lock_guard<std::mutex> lock(g_catalogMutex);
                                    std::wstring keyL = ToLowerString(activeRows[i].cmd);
                                    for (auto& cand : g_appCatalog) {
                                        if (cand.cmdLower == keyL) {
                                            LazilyResolveCandidateMetadata(cand, state->i18n.appTag);
                                            state->suggestions[i].hIcon = cand.cachedIcon;
                                            break;
                                        }
                                    }
                                }
                                if (activeRows[i].hIcon) {
                                    int iconY = cardTop + padTop + visIdx * rowHInt + (rowHInt - iconSize) / 2;
                                    drawIconPreservingDestAlpha(iconX, iconY, activeRows[i].hIcon, iconSize);
                                }
                            }
                        }
                    }

                    if (state->hRunIcon) {
                        int runIconSz = state->runIconLoadedSize > 0 ? state->runIconLoadedSize : DpiScaleS(20, state);
                        int singleBarH = DpiScaleS(kBaseInputBarHeight, state);
                        int runIconX  = (singleBarH - runIconSz) / 2 + DpiScaleS(4, state);
                        int runIconY  = state->translateMode
                            ? ((layout.rcEdit.top + layout.rcEdit.bottom - runIconSz) / 2)
                            : (barTop + (barH - runIconSz) / 2);
                        drawIconPreservingDestAlpha(runIconX, runIconY, state->hRunIcon, runIconSz);
                    }

                    if (state->animAlpha < 0.999f) {
                        uint32_t aScale = (uint32_t)std::clamp((int)std::round(state->animAlpha * 256.0f), 0, 256);
                        size_t totalPx = (size_t)w * (size_t)h;
                        DWORD* px = state->cachedDibPixels;
                        for (size_t idx = 0; idx < totalPx; ++idx) {
                            DWORD p = px[idx];
                            if (p == 0) continue;
                            uint32_t a = (((p >> 24) & 0xFF) * aScale) >> 8;
                            uint32_t r = (((p >> 16) & 0xFF) * aScale) >> 8;
                            uint32_t g = (((p >> 8)  & 0xFF) * aScale) >> 8;
                            uint32_t b = ((p         & 0xFF) * aScale) >> 8;
                            px[idx] = (a << 24) | (r << 16) | (g << 8) | b;
                        }
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

static std::mutex g_gfxInitMutex;

bool EnsureGraphicsAndIconsInitialized() {
    std::lock_guard<std::mutex> lock(g_gfxInitMutex);
    if (!g_pD2DFactory) {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory),
                                     nullptr, reinterpret_cast<void**>(&g_pD2DFactory)))) {
            return false;
        }
    }
    if (!g_pDWriteFactory) {
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                       reinterpret_cast<IUnknown**>(&g_pDWriteFactory)))) {
            return false;
        }
    }
    if (g_pDWriteFactory && !g_pNaturalRenderParams) {
        IDWriteRenderingParams* pDefParams = nullptr;
        if (SUCCEEDED(g_pDWriteFactory->CreateRenderingParams(&pDefParams)) && pDefParams) {
            g_pDWriteFactory->CreateCustomRenderingParams(
                pDefParams->GetGamma(),
                pDefParams->GetEnhancedContrast(),
                pDefParams->GetClearTypeLevel(),
                pDefParams->GetPixelGeometry(),
                DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC,
                &g_pNaturalRenderParams);
            pDefParams->Release();
        }
    }
    EnsureWindhawkRunIconInstalled(false);
    return true;
}

void TransformRunDialog(HWND hDlg) {
    if (g_unloading.load()) return;
    if (!hDlg || !IsWindow(hDlg)) return;
    if (GetPropW(hDlg, L"WindhawkModernRunApplied")) return;
    SetPropW(hDlg, L"WindhawkModernRunApplied", (HANDLE)1);

    EnsureGraphicsAndIconsInitialized();

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

        LONG_PTR cStyle = GetWindowLongPtrW(hChild, GWL_STYLE);
        if (cStyle & WS_TABSTOP) {
            SetWindowLongPtrW(hChild, GWL_STYLE, cStyle & ~WS_TABSTOP);
        }

        if (_wcsicmp(cls, L"Static") == 0) {
            ShowWindow(hChild, SW_HIDE);
            EnableWindow(hChild, FALSE);
        } else if (_wcsicmp(cls, L"Button") == 0) {
            if (id == IDOK) {
                state->hOkBtn = hChild;
            }
            SetWindowPos(hChild, nullptr, -32000, -32000, 1, 1, SWP_NOZORDER | SWP_NOACTIVATE);
            ShowWindow(hChild, SW_HIDE);
            EnableWindow(hChild, FALSE);
        } else if (id == kRunDlgComboId || _wcsicmp(cls, L"ComboBox") == 0 || _wcsicmp(cls, L"ComboBoxEx32") == 0) {
            if (!state->hCombo || id == kRunDlgComboId) {
                state->hCombo = hChild;
                state->hOrigEdit = FindWindowExW(hChild, nullptr, L"Edit", nullptr);
                if (!state->hOrigEdit) {
                    HWND hInnerCombo = FindWindowExW(hChild, nullptr, L"ComboBox", nullptr);
                    if (hInnerCombo) {
                        EnableWindow(hInnerCombo, FALSE);
                        state->hOrigEdit = FindWindowExW(hInnerCombo, nullptr, L"Edit", nullptr);
                    }
                }
            }
            SetWindowPos(hChild, nullptr, -32000, -32000, 1, 1, SWP_NOZORDER | SWP_NOACTIVATE);
            ShowWindow(hChild, SW_HIDE);
            EnableWindow(hChild, FALSE);
            if (state->hOrigEdit) {
                EnableWindow(state->hOrigEdit, FALSE);
            }
        } else {
            ShowWindow(hChild, SW_HIDE);
            EnableWindow(hChild, FALSE);
        }
    }

    if (IsWindowVisible(hDlg)) {
        g_internalActivation = true;
        if (ShowWindow_Orig) {
            ShowWindow_Orig(hDlg, SW_HIDE);
        } else {
            ShowWindow(hDlg, SW_HIDE);
        }
        g_internalActivation = false;
    }

    state->hOrigIconSmall      = reinterpret_cast<HICON>(SendMessageW(hDlg, WM_GETICON, ICON_SMALL, 0));
    state->hOrigIconBig        = reinterpret_cast<HICON>(SendMessageW(hDlg, WM_GETICON, ICON_BIG, 0));
    state->hOrigClassIconSmall = reinterpret_cast<HICON>(GetClassLongPtrW(hDlg, GCLP_HICONSM));
    state->hOrigClassIconBig   = reinterpret_cast<HICON>(GetClassLongPtrW(hDlg, GCLP_HICON));

    SetWindowSubclass(hDlg, RunDlgSubclassProc, 1, (DWORD_PTR)state);
    RefreshDialogRunIcons(state);
    if (!state->hRunIcon) {
        ExtractIconExW(L"shell32.dll", 24, nullptr, &state->hRunIcon, 1);
    }

    LoadHistoryItems(state);
    UpdateDialogEditFont(state);
    RecomputeDialogMetrics(state);
    ReloadCustomBackgroundMedia(state, false);

    SetPropW(hDlg, L"WindhawkModernRunState", reinterpret_cast<HANDLE>(state));

    int dlgW = GetDlgWidth(state);
    int newX = 100;
    int newY = 100;
    bool anchoredTop = false;
    GetDefaultTaskbarAnchoredPosition(hDlg, dlgW, state->totalHeight, state->dpi, &newX, &newY, &anchoredTop);
    ClampDialogToMonitorWorkArea(hDlg, &newX, &newY, dlgW, state->totalHeight, state->dpi);

    state->normalX = newX;
    state->normalY = newY;
    state->anchoredTop = anchoredTop;

    if (NeedsBackdropSnapshot(state)) {
        CaptureCleanMonitorBackdrop(hDlg, false);
        state->backdropLightSample = SampleBackdropLuminanceFromSnap(
            newX, newY, dlgW, state->totalHeight, false);
        state->lastTopologySig = GetBackdropWindowTopologySignature(hDlg);
    }
    state->pendingTopologyRecapture = false;

    ApplyWindowAppearance(hDlg, state);

    // Use Windows RichEdit50W (MSFTEDIT_CLASS from msftedit.dll) off-screen at (-32000, -32000):
    // RichEdit50W implements native Windows 11 TSF (ITextStoreACP) inline Vietnamese composition
    // so built-in Windows Vietnamese Telex/VNI never opens a legacy white box, never jumps/shifts
    // characters across multiple words, and commits Space in 0.0ms!
    static HMODULE s_hMsftEdit = LoadLibraryW(L"msftedit.dll");
    (void)s_hMsftEdit;
    state->hModernEdit = CreateWindowExW(
        0, L"RICHEDIT50W", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        -32000, -32000, DpiScale(800, state->dpi), DpiScale(28, state->dpi),
        hDlg, (HMENU)(INT_PTR)kModernEditId, GetModuleHandleW(nullptr), nullptr
    );
    if (state->hModernEdit) {
        // TM_PLAINTEXT (1) | TM_MULTICODEPAGE (32), and ENM_CHANGE (1) so EN_CHANGE fires on every edit
        SendMessageW(state->hModernEdit, WM_USER + 89 /*EM_SETTEXTMODE*/, 1 | 32, 0);
        SendMessageW(state->hModernEdit, WM_USER + 69 /*EM_SETEVENTMASK*/, 0, 0x00000001 /*ENM_CHANGE*/);
    } else {
        state->hModernEdit = CreateWindowExW(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
            -32000, -32000, DpiScale(800, state->dpi), DpiScale(28, state->dpi),
            hDlg, (HMENU)(INT_PTR)kModernEditId, GetModuleHandleW(nullptr), nullptr
        );
    }

    // Pre-warm app suggestion catalog in background so typing & Space never block the UI thread
    {
        bool needWarmup = false;
        {
            std::lock_guard<std::mutex> lock(g_catalogMutex);
            needWarmup = g_appCatalog.empty();
        }
        if (needWarmup && !g_unloading.load()) {
            g_activeTranslateThreads.fetch_add(1);
            std::thread([]() {
                struct WarmupGuard { ~WarmupGuard() { g_activeTranslateThreads.fetch_sub(1); } } guard;
                if (!g_unloading.load()) {
                    EnsureAppCatalogPopulated();
                }
            }).detach();
        }
    }

    {
        wchar_t srcBuf[32] = {}, dstBuf[32] = {};
        if (Wh_GetStringValue(GetUserOverrideKey(L"translateSourceLang").c_str(), srcBuf, ARRAYSIZE(srcBuf)) > 0 && srcBuf[0]) {
            state->translateSourceLang = srcBuf;
        }
        if (Wh_GetStringValue(GetUserOverrideKey(L"translateTargetLang").c_str(), dstBuf, ARRAYSIZE(dstBuf)) > 0 && dstBuf[0]) {
            state->translateTargetLang = dstBuf;
        } else {
            state->translateTargetLang = IsDialogVietnameseUI(state) ? L"vi" : L"en";
        }
    }

    if (state->hModernEdit) {
        SendMessageW(state->hModernEdit, EM_SETLIMITTEXT, 65535, 0);
        SendMessageW(state->hModernEdit, WM_SETFONT, (WPARAM)state->hFontEditCtl, FALSE);

        state->suppressSuggestionUpdate = true;
        if (state->settings.clearInitialInput) {
            SetWindowTextW(state->hModernEdit, L"");
        } else if (state->hCombo) {
            wchar_t initBuf[MAX_PATH] = {};
            GetWindowTextW(state->hCombo, initBuf, MAX_PATH);
            SetWindowTextW(state->hModernEdit, initBuf);
            SendMessageW(state->hModernEdit, EM_SETSEL, 0, -1);
        } else if (!state->history.empty()) {
            const std::wstring& latestCmd = state->history.front().cmd;
            SetWindowTextW(state->hModernEdit, latestCmd.c_str());
            SendMessageW(state->hModernEdit, EM_SETSEL, 0, -1);
        }
        state->suppressSuggestionUpdate = false;
        SetWindowSubclass(state->hModernEdit, ModernEditSubclassProc, 1, (DWORD_PTR)state);
    }

    SetTimer(hDlg, kTimerCaret, 530, nullptr);
    SetTimer(hDlg, kTimerLuminance, 150, nullptr);
    StartDialogOpenAnimation(state);
    int initSlideOff = DpiScaleS(14, state) * (state->anchoredTop ? -1 : 1);

    SetWindowPos(hDlg, nullptr, newX, newY + initSlideOff, dlgW, state->totalHeight,
                 SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    NotifyTaskbarRedrawWindowIcon(hDlg);

    ULONGLONG now = GetTickCount64();
    g_openTick.store(now);
    g_lastToggleTick.store(now);
    g_winRReleasedSinceOpen.store(true);
    g_isBrowsingFile.store(false);
    g_dlgIsForeground.store(true);
    g_deactivatedByTaskbarTick.store(0);
    g_hTrackedRunDlg.store(hDlg);
    g_hActiveRunDlg.store(hDlg);

    ForceFocusModernEdit(hDlg, state);
    if (state->hModernEdit) {
        PostMessageW(hDlg, WM_NEXTDLGCTL, reinterpret_cast<WPARAM>(state->hModernEdit), TRUE);
    }
    InvalidateRect(hDlg, nullptr, FALSE);
}

// --- Zero-Hang Non-Blocking Run Dialog Interception (Safe for Mod Update / Reload / Uninit) ---

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

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    HWND hActive = g_hActiveRunDlg.load();
    if (!g_unloading.load() && !g_isBrowsingFile.load() &&
        !IsActiveRunDialogAlive(hActive) &&
        nCmdShow != SW_HIDE && hWnd && IsWindow(hWnd) &&
        GetWindowThreadProcessId(hWnd, nullptr) == GetCurrentThreadId() &&
        (GetWindowLongW(hWnd, GWL_STYLE) & WS_CHILD) == 0 &&
        !GetPropW(hWnd, L"WindhawkModernRunApplied") &&
        IsRunDialogWindow(hWnd)) {
        TransformRunDialog(hWnd);
        return TRUE;
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
    {
        std::lock_guard<std::mutex> lock(g_gfxInitMutex);
        if (g_pNaturalRenderParams) {
            g_pNaturalRenderParams->Release();
            g_pNaturalRenderParams = nullptr;
        }
        if (g_pDWriteFactory) {
            g_pDWriteFactory->Release();
            g_pDWriteFactory = nullptr;
        }
        if (g_pD2DFactory) {
            g_pD2DFactory->Release();
            g_pD2DFactory = nullptr;
        }
    }
    if (g_fluentMenuClassRegistered) {
        UnregisterClassW(kFluentMenuClassName, GetModInstanceHandle());
        g_fluentMenuClassRegistered = false;
    }
}

BOOL Wh_ModInit() {
    g_unloading.store(false);
    g_glassCpuSupported = DetectGlassCpuSupport();

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.style = 0;
    wc.lpfnWndProc = FluentMenuWndProc;
    wc.hInstance = GetModInstanceHandle();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kFluentMenuClassName;
    if (!RegisterClassExW(&wc)) {
        DWORD err = GetLastError();
        if (err != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"Warning: RegisterClassExW returned %lu", err);
        }
    }
    g_fluentMenuClassRegistered = true;

    LoadSettings(false);
    g_msgUninit = RegisterWindowMessageW(L"Windhawk_Uninit_" WH_MOD_ID);
    g_msgToggle = RegisterWindowMessageW(L"Windhawk_Toggle_" WH_MOD_ID);

    g_hShell32 = GetModuleHandleW(L"shell32.dll");

    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Orig);
    WindhawkUtils::SetFunctionHook(SetForegroundWindow, SetForegroundWindow_Hook, &SetForegroundWindow_Orig);
    WindhawkUtils::SetFunctionHook(SwitchToThisWindow, SwitchToThisWindow_Hook, &SwitchToThisWindow_Orig);

    return TRUE;
}

void Wh_ModAfterInit() {
    // Transform any already-open classic Run dialog safely after Windhawk has committed all hooks
    EnumWindows([](HWND hWnd, LPARAM) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid == GetCurrentProcessId() && IsRunDialogWindow(hWnd) &&
            !GetPropW(hWnd, L"WindhawkModernRunApplied")) {
            ShowWindowAsync(hWnd, SW_SHOW);
        }
        return TRUE;
    }, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings(true);
    if (HWND hDlg = g_hActiveRunDlg.load()) {
        PostMessageW(hDlg, g_msgToggle, kActionReloadSettings, 0);
    }
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true);
    g_translateReqCounter.fetch_add(1);
    CloseActiveGearMenu();

    HWND hDlg = g_hTrackedRunDlg.load();
    if (!hDlg) hDlg = g_hActiveRunDlg.load();
    if (hDlg && IsWindow(hDlg)) {
        DWORD_PTR dwRes = 0;
        SendMessageTimeoutW(hDlg, g_msgUninit, 0, 0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 800, &dwRes);
        if (IsWindow(hDlg)) {
            PostMessageW(hDlg, WM_CLOSE, 0, 0);
            PostMessageW(hDlg, WM_NULL, 0, 0);
        }
    }

    for (int waitTick = 0; waitTick < 25 && ((hDlg && IsWindow(hDlg)) || g_activeTranslateThreads.load() > 0); ++waitTick) {
        if (hDlg && IsWindow(hDlg)) {
            PostMessageW(hDlg, WM_CLOSE, 0, 0);
            PostMessageW(hDlg, WM_NULL, 0, 0);
        }
        Sleep(20);
    }
}

void Wh_ModUninit() {
    for (int waitTick = 0; waitTick < 25 && g_activeTranslateThreads.load() > 0; ++waitTick) {
        Sleep(20);
    }
    ReleaseGlobalResources();
}
