// ==WindhawkMod==
// @id              drag-n-drop-enhanced
// @name            Drag n Drop Enhanced for Windows 10 and 11
// @name:vi-VN      Drag n Drop Enhanced for Windows 10 và 11
// @description     Dual macOS & Windows Fluent Drag & Drop Shelf for Files, Images, Text & URLs — with Taskbar Minimized Status Pill, Shake-to-Summon, Edge-Bump & optional Win+V Clipboard History Replacement for Windows 10 & 11.
// @description:vi-VN Kệ kéo-thả giao diện kép macOS & Windows Fluent cho Tệp, Hình ảnh, Văn bản & URL — có Viên thuốc trạng thái trên Taskbar khi thu nhỏ, Lắc chuột, Chạm cạnh & tùy chọn thay thế Win+V cho Windows 10 & 11.
// @version         2.0.1.0
// @author          caotu2005
// @github          https://github.com/caotu2005
// @include         windhawk.exe
// @compilerOptions -DWIN32_LEAN_AND_MEAN -ld2d1 -ldwrite -ldwmapi -lgdi32 -luser32 -lshell32 -lshlwapi -lole32 -loleaut32 -lwindowscodecs -luuid -lwinmm -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Drag n Drop Enhanced for Windows 10 and 11

A lightweight, native Direct2D + WIC + OLE2 **Drag & Drop Shelf & Optional `Win + V` Clipboard Hub** for **Windows 10 and Windows 11**, featuring **both macOS (Dropover / Yoink / Notch)** and **Windows Fluent** visual themes. Runs as an isolated **Windhawk Tool Mod** (`windhawk.exe`) with zero hooks into `explorer.exe`.

> ℹ️ **Compatibility Note (Smart App Control) / Lưu ý tương thích:**
> - **🌐 English:** Like all Windhawk mods, this mod is compiled locally into an unsigned `.dll` file and loaded into `windhawk.exe`. If Windows **Smart App Control** is active on your PC, Windows will block the compiled `.dll` and the mod will not load.
> - **🇻🇳 Tiếng Việt:** Giống như mọi bản mod trên Windhawk, mod này được biên dịch cục bộ thành tệp `.dll` không có chữ ký số để nạp vào `windhawk.exe`. Nếu máy đang bật **Smart App Control (Kiểm soát Ứng dụng Thông minh)**, Windows sẽ chặn tệp `.dll` này khiến mod không thể hoạt động.

## 📸 Screenshots & Demo / Hình minh họa & Video Demo

**🎬 Demo:**

![Drag n Drop Enhanced Demo](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/dragndrop/videodemo.gif)

**Drop Shelf & Quick Look Image Preview (macOS / Windows Fluent):**

![Drag n Drop Shelf & Quick Look Preview](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/dragndrop/dragndrop.png)

**Top-Center Animated Mini Bar:**

![Top-Center Mini Bar](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/dragndrop/topcenterminibar.png)

**Live Copy / Cut / Paste Alert Island:**

![Copy Paste Alert Island](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/dragndrop/copypaste.png)

---

## 🌐 Features (English)

- **Isolated Tool Mod Architecture:**
  - Runs in a dedicated `windhawk.exe` tool process instead of injecting into `explorer.exe`, keeping the Windows shell stable and preserving staged items across Explorer restarts.
- **Dual Native Themes (macOS & Windows Fluent):**
  - **macOS Style:** Frosted vibrancy glass, macOS System Blue (`#0A84FF`), and left-side **traffic-light dots** (Red = Clear & Close, Yellow = Minimize to Top Mini Bar, Green/Amber = Pin).
  - **Windows Fluent Style:** Adaptive Windows accent color, crisp Fluent corners, and right-side **Windows caption buttons** (Pin, Minimize `—`, Close `✕` with red hover).
  - Mix and match the **Window Button Style** independently (Auto / macOS / Windows).
- **Minimize to Animated Top-Center Mini Bar:**
  - Click **Minimize** when items are staged to collapse the shelf into a compact animated mini bar at the top edge of the work area (automatically avoiding a top taskbar).
  - **Click** to restore the shelf, **drag straight out** to drop all staged items without reopening, or **drop new items directly onto the bar**. Automatically hides once all Drag & Drop items are dragged out.
- **Drag & Drop Anything (Files, Folders, Images, Text & URLs):**
  - **Files & Folders (`CF_HDROP`):** Stage single or multiple files/folders with native shell icons, category filters, reveal-in-Explorer, and inline audio playback.
  - **Browser & App Images (`FileContents` / `PNG` / `CF_DIB`):** Drag images directly from Chrome, Edge, Firefox, Paint, or Word with WIC rounded thumbnails and Quick Look preview (with fullscreen lightbox); dragging out exposes `CF_HDROP` + `PNG` + `CF_DIB` simultaneously.
  - **Selected Text & Web Links (`CF_UNICODETEXT` / `UniformResourceLocatorW`):** Stage text snippets or URLs and drop them into any editor, browser, or chat app.
  - **Live Copy / Cut / Paste Alerts:** Non-blocking toast notifications for Copy, Cut, and Paste actions, respecting both `ExcludeClipboardContentFromMonitorProcessing` and `CanIncludeInClipboardHistory = 0` so protected/password content is never previewed.
- **Optional `Win + V` Clipboard History Replacement (Off by Default):**
  - Enable **Replace `Win + V`** in settings to intercept `Win + V` cleanly and open the **Clipboard History** tab right beside your cursor (with 10–100 configurable history slots and category filters).
  - **Storage Note:** Staged browser/app images, virtual files, and images saved in Clipboard History (while `Replace Win + V` is enabled) are temporarily stored as files inside this mod's Windhawk storage folder (`staged/<UserSID>/`) while they remain on the shelf or in history, and are automatically cleared when removed, unloaded, or on next startup.
- **30 Global Languages:** Automatically detects your Windows display language across 30 major languages (including English, Vietnamese, Chinese, Spanish, French, German, Japanese, Korean, Portuguese, Russian, Arabic, Hindi, Indonesian, Turkish, and more), or select any language manually in Settings.
- **Flexible Triggers:**
  - **Auto-Summon on File Drag**, **Shake-to-Summon** near cursor, **Screen Edge-Bump** while dragging text/images, and an optional **`Alt + Shift + D`** global toggle hotkey (**off by default** so it does not override `Alt + Shift + D` in apps such as Microsoft Word).

---

## 🇻🇳 Tính năng chính (Tiếng Việt)

- **Chạy độc lập dạng Tool Mod:**
  - Chạy trong tiến trình `windhawk.exe` riêng biệt thay vì chèn vào `explorer.exe`, giúp Explorer luôn ổn định và không mất tệp đang ghim khi khởi động lại Explorer.
- **Giao diện kép macOS & Thuần Windows Fluent:**
  - **Phong cách macOS:** Kính mờ Vibrancy, xanh dương macOS (`#0A84FF`) và bộ **nút đèn giao thông bên trái** (Đỏ = Xóa & Đóng, Vàng = Thu gọn lên thanh mini bar, Cam/Ghim = Ghim kệ).
  - **Phong cách Windows Fluent:** Tự động lấy màu nhấn (Accent Color) thật của Windows, bo góc Fluent sắc nét và bộ **nút đóng mở chuẩn Windows bên phải** (Ghim, Thu gọn `—`, Đóng `✕` đỏ khi rê chuột).
  - Có thể chọn riêng kiểu nút đóng/mở (Tự động / Kiểu macOS / Kiểu Windows).
- **Thu gọn thành Thanh Mini Bar Động ở Mép Trên:**
  - Bấm **Thu gọn** khi có mục trên kệ để biến kệ thành thanh trạng thái nhỏ gọn ở giữa mép trên vùng làm việc (tự né Taskbar trên).
  - **Bấm** để mở lại kệ, **kéo thẳng ra ngoài** để thả toàn bộ mục mà không cần mở lại, hoặc **thả thêm tệp/ảnh trực tiếp lên thanh**. Tự động trượt ẩn khi không còn mục nào trong Drag & Drop.
- **Kéo thả mọi nội dung & Thông báo Sao chép / Dán:**
  - Hỗ trợ tệp/thư mục (`CF_HDROP`), hình ảnh từ trình duyệt/ứng dụng (`FileContents` / `PNG` / `CF_DIB`), văn bản, URL, lọc theo loại (`Tất cả / Văn bản / Ảnh / Tệp / Nhạc`), xem trước ảnh toàn màn hình, mở thư mục chứa tệp và phát nhạc trực tiếp.
  - Thông báo nổi khi Sao chép / Cắt / Dán tự động bỏ qua nội dung bảo mật (`ExcludeClipboardContentFromMonitorProcessing` và `CanIncludeInClipboardHistory = 0`).
- **Tùy chọn thay thế `Win + V` (Mặc định tắt):**
  - Bật **Thay thế `Win + V`** trong Cài đặt để dùng tab **Lịch sử Clipboard** tích hợp (lưu 10–100 mục).
  - **Lưu ý lưu trữ:** Hình ảnh kéo từ trình duyệt/ứng dụng, tệp ảo và hình ảnh trong Lịch sử Clipboard (khi bật `Thay thế Win + V`) được lưu tạm dạng tệp trong thư mục lưu trữ riêng của mod (`staged/<UserSID>/`), và tự động xóa khi gỡ mục, tắt mod hoặc lần khởi chạy kế tiếp.
- **Hỗ trợ 30 ngôn ngữ toàn cầu:** Tự động nhận diện ngôn ngữ Windows trên 30 ngôn ngữ thông dụng nhất (mặc định đầy đủ Tiếng Việt, Tiếng Anh cùng Trung, Nhật, Hàn, Pháp, Đức, Tây Ban Nha, Nga, Ả Rập, Hindi, Thái, Indonesia...) hoặc chọn thủ công trong Cài đặt.
- **Gọi kệ linh hoạt:** Tự hiện khi kéo tệp, **Lắc chuột để gọi**, **Chạm mép màn hình (Edge-Bump)**, và tùy chọn phím tắt toàn cục **`Alt + Shift + D`** (**mặc định tắt** để tránh trùng phím chèn ngày trong Microsoft Word).

---

## 🤝 Credits
- Author: [caotu2005](https://github.com/caotu2005)
- Extracted & evolved from the Mini Drop Shelf architecture in **Modern Taskbar for Windows 11 (Dynamic Taskbar)**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Display Language (30 Languages)
  $name:vi-VN: Ngôn ngữ hiển thị (30 ngôn ngữ)
  $options:
  - auto: System Default (Auto-Detect 30 Languages)
  - en: English
  - vi: Tiếng Việt
  - zh_cn: 简体中文
  - zh_tw: 繁體中文
  - es: Español
  - fr: Français
  - de: Deutsch
  - ja: 日本語
  - ko: 한국어
  - pt: Português
  - ru: Русский
  - it: Italiano
  - ar: العربية
  - hi: हिन्दी
  - id: Bahasa Indonesia
  - tr: Türkçe
  - pl: Polski
  - nl: Nederlands
  - th: ไทย
  - uk: Українська
  - sv: Svenska
  - cs: Čeština
  - ro: Română
  - hu: Magyar
  - el: Ελληνικά
  - he: עברית
  - da: Dansk
  - fi: Suomi
  - no: Norsk
  - ms: Bahasa Melayu
  $options:vi-VN:
  - auto: Tự động theo hệ thống (30 ngôn ngữ)
  - en: English
  - vi: Tiếng Việt
  - zh_cn: 简体中文
  - zh_tw: 繁體中文
  - es: Español
  - fr: Français
  - de: Deutsch
  - ja: 日本語
  - ko: 한국어
  - pt: Português
  - ru: Русский
  - it: Italiano
  - ar: العربية
  - hi: हिन्दी
  - id: Bahasa Indonesia
  - tr: Türkçe
  - pl: Polski
  - nl: Nederlands
  - th: ไทย
  - uk: Українська
  - sv: Svenska
  - cs: Čeština
  - ro: Română
  - hu: Magyar
  - el: Ελληνικά
  - he: עברית
  - da: Dansk
  - fi: Suomi
  - no: Norsk
  - ms: Bahasa Melayu

- triggerGroup:
  - triggerMode: both
    $name: Shelf Activation Mode
    $name:vi-VN: Cách kích hoạt kệ kéo-thả
    $options:
    - both: "Auto-Show on Drag + Shake-to-Summon (Recommended)"
    - auto_drag: "Auto-Show on File Drag (Yoink Style)"
    - shake_only: "Shake Mouse While Dragging Only (Dropover Style)"
    $options:vi-VN:
    - both: "Tự hiện khi kéo + Lắc chuột để gọi (Khuyên dùng)"
    - auto_drag: "Tự động hiện khi kéo tệp (Kiểu Yoink)"
    - shake_only: "Chỉ hiện khi lắc chuột lúc kéo (Kiểu Dropover)"
  - shelfPosition: top_center
    $name: Default Shelf Position
    $name:vi-VN: Vị trí hiển thị mặc định của kệ
    $options:
    - top_center: "Top Center (macOS Notch Style)"
    - near_cursor: "Near Cursor (Dropover / Win+V Style)"
    - edge_right: "Right Screen Edge (Yoink Style)"
    - edge_left: "Left Screen Edge"
    - above_taskbar: "Above Taskbar (Bottom Center)"
    $options:vi-VN:
    - top_center: "Giữa mép trên (Kiểu macOS Notch)"
    - near_cursor: "Ngay cạnh con trỏ chuột (Kiểu Dropover)"
    - edge_right: "Cạnh phải màn hình (Kiểu Yoink)"
    - edge_left: "Cạnh trái màn hình"
    - above_taskbar: "Phía trên thanh Taskbar (Giữa mép dưới)"
  - enableEdgeBump: true
    $name: "Summon Shelf by Bumping Screen Edge While Dragging Text/Images"
    $name:vi-VN: "Gọi kệ khi kéo văn bản/hình ảnh chạm mép màn hình (Edge-Bump)"
  - shakeSensitivity: medium
    $name: Shake-to-Summon Sensitivity
    $name:vi-VN: Độ nhạy cử chỉ lắc chuột
    $options:
    - high: "High (Easy to trigger)"
    - medium: "Medium (Balanced)"
    - low: "Low (Deliberate shake)"
    $options:vi-VN:
    - high: "Cao (Lắc nhẹ là hiện)"
    - medium: "Trung bình (Cân bằng)"
    - low: "Thấp (Lắc rõ mới hiện)"
  - enableGlobalHotkey: false
    $name: "Enable Global Toggle Hotkey (Alt + Shift + D; off by default to avoid Word conflict)"
    $name:vi-VN: "Bật phím tắt gọi/ẩn nhanh kệ (Alt + Shift + D; mặc định tắt để tránh trùng phím Word)"
  - replaceWinV: false
    $name: "Replace Windows + V with Built-in Clipboard History & Drag Hub (Win+V Enhanced)"
    $name:vi-VN: "Thay thế Windows + V bằng Lịch sử Clipboard & Kéo-thả tích hợp (Win+V Enhanced)"
  - maxClipboardHistory: 30
    $name: "Clipboard History Limit (10 to 100, Default 30)"
    $name:vi-VN: "Số mục lịch sử Clipboard tối đa (10 đến 100, Mặc định 30)"
  $name: 1. Trigger, Taskbar & Win+V
  $name:vi-VN: 1. Kích hoạt, Taskbar & Thay thế Win+V

- behaviorGroup:
  - autoClearAfterDrop: true
    $name: "Auto-Clear Shelf After Successful Drag-Out (Hold Ctrl or Pin to Keep)"
    $name:vi-VN: "Tự xóa kệ sau khi kéo-thả thành công (Giữ Ctrl hoặc Ghim để giữ lại)"
  - acceptImageDrops: true
    $name: "Accept Dragged Images from Browsers, Apps & Screenshots (PNG/DIB)"
    $name:vi-VN: "Hỗ trợ kéo thả Hình ảnh trực tiếp từ Trình duyệt, Ứng dụng & Ảnh chụp"
  - acceptTextDrops: true
    $name: "Accept Dragged Text & Web Links (URLs)"
    $name:vi-VN: "Hỗ trợ kéo thả Văn bản & Đường dẫn Web (URL)"
  - enableCopyPasteAlert: true
    $name: "Show Live Toast Alert on Copy, Cut & Paste (Files, Images, Text)"
    $name:vi-VN: "Hiện thông báo nhanh khi Sao chép, Cắt & Dán (Tệp, Ảnh, Văn bản)"
  - alertIslandPosition: auto
    $name: "Copy/Paste Alert Island Position"
    $name:vi-VN: "Vị trí thông báo nổi Sao chép / Dán (Alert Island)"
    $options:
    - auto: "Auto Follow Taskbar (Bottom/Left/Right -> Bottom, Top -> Top)"
    - top: "Always Top Center (Below Top Taskbar)"
    - bottom: "Always Bottom Center (Above Bottom Taskbar)"
    $options:vi-VN:
    - auto: "Tự động theo Taskbar (Dưới/Trái/Phải -> Dưới, Trên -> Trên)"
    - top: "Luôn ở mép trên (Tự né Taskbar trên)"
    - bottom: "Luôn ở mép dưới (Tự né Taskbar dưới)"
  - showImageThumbnails: true
    $name: Show Real Image Thumbnails & Native File Icons
    $name:vi-VN: Hiển thị ảnh thu nhỏ thật & biểu tượng tệp gốc
  - hideInFullscreen: true
    $name: Suppress Auto-Show During Fullscreen Games/Videos
    $name:vi-VN: Tạm ẩn khi đang chơi game / xem video toàn màn hình
  - maxStagedItems: 32
    $name: "Maximum Staged Items (8 to 64, Default 32)"
    $name:vi-VN: "Số mục ghim tối đa (8 đến 64, Mặc định 32)"
  $name: 2. Drag & Drop Behavior
  $name:vi-VN: 2. Hành vi Kéo & Thả

- appearanceGroup:
  - uiStyle: macos
    $name: Interface Theme Style
    $name:vi-VN: Phong cách giao diện
    $options:
    - macos: "macOS Style (Rounded Vibrancy & Blue Accent)"
    - windows: "Windows 10/11 Fluent Style (System Accent & Crisp Corners)"
    $options:vi-VN:
    - macos: "Phong cách macOS (Bo tròn mềm mại & Xanh macOS)"
    - windows: "Phong cách thuần Windows 10/11 (Màu nhấn Windows & Fluent)"
  - buttonStyle: auto
    $name: Window Controls Style (Close / Minimize / Pin)
    $name:vi-VN: Kiểu nút Đóng / Thu nhỏ / Ghim
    $options:
    - auto: "Auto Match Interface Style"
    - macos: "macOS Traffic Lights (Left Side)"
    - windows: "Windows Caption Buttons (Right Side)"
    $options:vi-VN:
    - auto: "Tự động theo phong cách giao diện"
    - macos: "Nút đèn giao thông macOS (Bên trái)"
    - windows: "Nút đóng mở chuẩn Windows (Bên phải)"
  - glassStyle: macos_frosted
    $name: Backdrop Surface Material
    $name:vi-VN: Chất liệu nền mặt kính
    $options:
    - macos_frosted: "Frosted Vibrancy / Acrylic (Balanced)"
    - liquid_clear: "Liquid Glass (Crystal Clear)"
    - solid_soft: "Soft Solid Card (High Contrast)"
    $options:vi-VN:
    - macos_frosted: "Kính mờ Vibrancy / Acrylic (Cân bằng)"
    - liquid_clear: "Kính trong suốt Liquid Glass"
    - solid_soft: "Thẻ nền mịn tương phản cao"
  - colorMode: auto
    $name: "Theme Contrast (Auto / Dark / Light)"
    $name:vi-VN: "Tương phản sáng/tối (Tự động / Tối / Sáng)"
    $options:
    - auto: Auto Follow Windows / Backdrop
    - dark: Dark Theme
    - light: Light Theme
    $options:vi-VN:
    - auto: Tự động theo Windows / Nền
    - dark: Giao diện Tối
    - light: Giao diện Sáng
  $name: 3. macOS & Windows Appearance
  $name:vi-VN: 3. Giao diện macOS & Windows
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#ifndef WINVER
#define WINVER 0x0A00
#endif
#define NOMINMAX

#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <sddl.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <ole2.h>
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

extern "C" IMAGE_DOS_HEADER __ImageBase;
static inline HINSTANCE GetModInstanceHandle() {
    return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

// ---------------------------------------------------------------------------
// 1. Settings, Clipboard Format IDs & Global State
// ---------------------------------------------------------------------------
enum class TriggerMode { Both, AutoDrag, ShakeOnly };
enum class ShelfPosition { TopCenter, NearCursor, EdgeRight, EdgeLeft, AboveTaskbar };
enum class ShakeSensitivity { High, Medium, Low };
enum class UiStyle { MacOs, Windows };
enum class ButtonStyle { Auto, MacOs, Windows };
enum class GlassStyle { MacOsFrosted, LiquidClear, SolidSoft };
enum class ColorMode { Auto, Dark, Light };
enum class UiLang : int { Auto, En, Vi, ZhCn, ZhTw, Es, Fr, De, Ja, Ko, Pt, Ru, It, Ar, Hi, Id, Tr, Pl, Nl, Th, Uk, Sv, Cs, Ro, Hu, El, He, Da, Fi, No, Ms };
enum class ActiveTab { Shelf, ClipboardHistory };
enum class CategoryFilter { All, Text, Image, File, Audio };
enum class AlertIslandPosition { Auto, Top, Bottom };
enum class TaskbarEdge { Bottom, Top, Left, Right };

struct ModSettings {
    TriggerMode      triggerMode        = TriggerMode::Both;
    ShelfPosition    shelfPosition      = ShelfPosition::TopCenter;
    bool             enableEdgeBump     = true;
    ShakeSensitivity shakeSensitivity   = ShakeSensitivity::Medium;
    bool             enableGlobalHotkey = false;
    bool             replaceWinV        = false;
    int              maxClipboardHistory = 30;

    bool autoClearAfterDrop  = true;
    bool acceptImageDrops    = true;
    bool acceptTextDrops     = true;
    bool enableCopyPasteAlert = true;
    AlertIslandPosition alertIslandPosition = AlertIslandPosition::Auto;
    bool showImageThumbnails = true;
    bool hideInFullscreen    = true;
    int  maxStagedItems      = 32;

    UiStyle     uiStyle     = UiStyle::MacOs;
    ButtonStyle buttonStyle = ButtonStyle::Auto;
    GlassStyle  glassStyle  = GlassStyle::MacOsFrosted;
    ColorMode   colorMode   = ColorMode::Auto;
    UiLang      lang        = UiLang::Auto;
};

static ModSettings       g_settings;
static std::atomic<bool> g_running{false};
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_settingsDirty{false};
static std::atomic<bool> g_replaceWinVAtomic{false};
static std::atomic<bool> g_enableCopyPasteAlertAtomic{true};
static std::atomic<ULONGLONG> g_lastCutKeyTick{0};
static bool              g_systemLightTheme = false;
static D2D1_COLOR_F      g_winAccentColor = D2D1::ColorF(0.00f, 0.47f, 0.84f, 1.0f);

static HANDLE g_uiThread       = nullptr;
static DWORD  g_uiThreadId     = 0;
static HANDLE g_kbdThread      = nullptr;
static DWORD  g_kbdThreadId    = 0;
static HWND   g_shelfHwnd      = nullptr;
static HWND   g_pillHwnd       = nullptr; // Minimized Top-Center Mini Bar
static HWND   g_alertHwnd      = nullptr; // Copy / Cut / Paste Live Toast Alert
static HWND   g_previewHwnd    = nullptr; // Quick Look Image Preview Popup

// Registered OLE/Clipboard formats
static UINT g_cfPng             = 0;
static UINT g_cfFileDescriptorW = 0;
static UINT g_cfFileContents    = 0;
static UINT g_cfInetUrlW        = 0;
static UINT g_cfExcludeHistory  = 0;
static UINT g_cfCanIncludeHist  = 0;
static UINT g_cfDropEffect      = 0;

static void EnsureRegisteredFormats() {
    if (!g_cfPng)             g_cfPng             = RegisterClipboardFormatW(L"PNG");
    if (!g_cfFileDescriptorW) g_cfFileDescriptorW = RegisterClipboardFormatW(CFSTR_FILEDESCRIPTORW);
    if (!g_cfFileContents)    g_cfFileContents    = RegisterClipboardFormatW(CFSTR_FILECONTENTS);
    if (!g_cfInetUrlW)        g_cfInetUrlW        = RegisterClipboardFormatW(L"UniformResourceLocatorW");
    if (!g_cfExcludeHistory)  g_cfExcludeHistory  = RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");
    if (!g_cfCanIncludeHist)  g_cfCanIncludeHist  = RegisterClipboardFormatW(L"CanIncludeInClipboardHistory");
    if (!g_cfDropEffect)      g_cfDropEffect      = RegisterClipboardFormatW(L"Preferred DropEffect");
}

#define WM_APP_RELOAD_SETTINGS (WM_APP + 201)
#define WM_APP_WINV_SUMMON     (WM_APP + 202)
#define WM_APP_PASTE_ALERT     (WM_APP + 203)

#define IDT_DRAG_POLL      2001
#define IDT_ANIM_FRAME     2002
#define IDT_TOAST_CLEAR    2003
#define IDT_CLIP_DEBOUNCE  2004
#define IDT_PILL_SYNC      2005
#define IDT_PILL_ANIM      2006
#define IDT_ALERT_ANIM     2007
#define IDT_ALERT_EXPIRE   2008

#define HOTKEY_TOGGLE_SHELF 0x4452

static inline int DpiScale(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi), 96);
}

static inline float DpiScaleF(float value, UINT dpi) {
    return value * (static_cast<float>(dpi) / 96.0f);
}

static std::wstring ToLowerStr(std::wstring s) {
    for (wchar_t& ch : s) ch = static_cast<wchar_t>(std::towlower(ch));
    return s;
}

static int ResolveActiveLangIndex(UiLang lang) {
    if (lang != UiLang::Auto) {
        return static_cast<int>(lang) - 1; // 0=En, 1=Vi, 2..29=extra 28 langs
    }
    LANGID lid = GetUserDefaultUILanguage();
    WORD pri = PRIMARYLANGID(lid);
    WORD sub = SUBLANGID(lid);
    switch (pri) {
        case 0x09: return 0;  // English
        case 0x2a: return 1;  // Vietnamese
        case 0x04: return (sub == 0x02 || sub == 0x04) ? 2 : 3; // Chinese Simplified vs Traditional
        case 0x0a: return 4;  // Spanish
        case 0x0c: return 5;  // French
        case 0x07: return 6;  // German
        case 0x11: return 7;  // Japanese
        case 0x12: return 8;  // Korean
        case 0x16: return 9;  // Portuguese
        case 0x19: return 10; // Russian
        case 0x10: return 11; // Italian
        case 0x01: return 12; // Arabic
        case 0x39: return 13; // Hindi
        case 0x21: return 14; // Indonesian
        case 0x1f: return 15; // Turkish
        case 0x15: return 16; // Polish
        case 0x13: return 17; // Dutch
        case 0x1e: return 18; // Thai
        case 0x22: return 19; // Ukrainian
        case 0x1d: return 20; // Swedish
        case 0x05: return 21; // Czech
        case 0x18: return 22; // Romanian
        case 0x0e: return 23; // Hungarian
        case 0x08: return 24; // Greek
        case 0x0d: return 25; // Hebrew
        case 0x06: return 26; // Danish
        case 0x0b: return 27; // Finnish
        case 0x14: return 28; // Norwegian
        case 0x3e: return 29; // Malay
        default:   return 0;  // English fallback
    }
}

static bool ComputeUseVietnamese(UiLang lang) {
    return ResolveActiveLangIndex(lang) == 1;
}

static constexpr size_t kLocStringCount = 48;
static constexpr const wchar_t* kLocEnKeys[kLocStringCount] = {
    L"Folder",
    L"Web Link",
    L" chars",
    L"Added from Clipboard",
    L"Nothing new in Clipboard",
    L"Copied to Clipboard",
    L" item",
    L" items",
    L"Copied Image",
    L"Copied Text",
    L"Copied Files",
    L"Pasted Image",
    L"Pasted Text",
    L"Pasted Files",
    L"+ Drop",
    L"Shelf",
    L"Release to stage",
    L"Drop files, images, text",
    L"Or click paste from Clipboard",
    L"Clipboard",
    L"Drop Shelf",
    L"Clipboard (Win+V)",
    L"All",
    L"Text",
    L"Images",
    L"Files",
    L"Audio",
    L"No items in this category",
    L"Clipboard history is empty",
    L"Drop files, images, or text here",
    L"Click to paste • Drag to drop",
    L"+ Drop to add more",
    L"Drag item to target app",
    L"Drag all (",
    L" items) out",
    L"Image",
    L" file",
    L" files",
    L"Cut Files",
    L"Protected content",
    L"Private",
    L"Cut Text",
    L"Clipboard image",
    L"Cut Image",
    L"Shelf Pinned",
    L"Shelf Unpinned",
    L"Clipboard history cleared",
    L"Shelf cleared"
};

static constexpr const wchar_t* kLocExtraTables[28][kLocStringCount] = {
    // zh_cn (简体中文)
    { L"文件夹", L"网页链接", L" 字符", L"已从剪贴板添加", L"剪贴板无新内容", L"已复制到剪贴板", L" 项", L" 项", L"已复制图片", L"已复制文本", L"已复制文件", L"已粘贴图片", L"已粘贴文本", L"已粘贴文件", L"+ 拖放", L"暂存架", L"松开以暂存", L"拖放文件、图片、文本", L"或点击从剪贴板粘贴", L"剪贴板", L"拖放架", L"剪贴板 (Win+V)", L"全部", L"文本", L"图片", L"文件", L"音频", L"此分类下暂无项目", L"剪贴板历史为空", L"将文件、图片或文本拖放到此处", L"点击粘贴 • 拖动放置", L"+ 拖放更多", L"将项目拖到目标应用", L"拖出全部 (", L" 项)", L"图片", L" 个文件", L" 个文件", L"已剪切文件", L"受保护内容", L"私密", L"已剪切文本", L"剪贴板图片", L"已剪切图片", L"已固定暂存架", L"已取消固定", L"已清空剪贴板历史", L"已清空暂存架" },
    // zh_tw (繁體中文)
    { L"資料夾", L"網頁連結", L" 字元", L"已從剪貼簿新增", L"剪貼簿無新內容", L"已複製到剪貼簿", L" 項", L" 項", L"已複製圖片", L"已複製文字", L"已複製檔案", L"已貼上圖片", L"已貼上文字", L"已貼上檔案", L"+ 拖放", L"暫存架", L"放開以暫存", L"拖放檔案、圖片、文字", L"或點擊從剪貼簿貼上", L"剪貼簿", L"拖放架", L"剪貼簿 (Win+V)", L"全部", L"文字", L"圖片", L"檔案", L"音訊", L"此分類下沒有項目", L"剪貼簿記錄為空", L"將檔案、圖片或文字拖放到此處", L"點擊貼上 • 拖曳放置", L"+ 拖放更多", L"將項目拖至目標應用程式", L"拖出全部 (", L" 項)", L"圖片", L" 個檔案", L" 個檔案", L"已剪下檔案", L"受保護內容", L"私密", L"已剪下文字", L"剪貼簿圖片", L"已剪下圖片", L"已釘選暫存架", L"已取消釘選", L"已清除剪貼簿記錄", L"已清空暫存架" },
    // es (Español)
    { L"Carpeta", L"Enlace web", L" caracteres", L"Añadido del portapapeles", L"Nada nuevo en el portapapeles", L"Copiado al portapapeles", L" elemento", L" elementos", L"Imagen copiada", L"Texto copiado", L"Archivos copiados", L"Imagen pegada", L"Texto pegado", L"Archivos pegados", L"+ Soltar", L"Estante", L"Suelta para guardar", L"Suelta archivos, imágenes o texto", L"O haz clic para pegar del portapapeles", L"Portapapeles", L"Estante", L"Portapapeles (Win+V)", L"Todo", L"Texto", L"Imágenes", L"Archivos", L"Audio", L"No hay elementos en esta categoría", L"El historial del portapapeles está vacío", L"Suelta archivos, imágenes o texto aquí", L"Clic para pegar • Arrastra para soltar", L"+ Soltar más", L"Arrastra al programa de destino", L"Arrastrar todo (", L" elementos)", L"Imagen", L" archivo", L" archivos", L"Archivos cortados", L"Contenido protegido", L"Privado", L"Texto cortado", L"Imagen del portapapeles", L"Imagen cortada", L"Estante fijado", L"Estante desfijado", L"Historial del portapapeles borrado", L"Estante vaciado" },
    // fr (Français)
    { L"Dossier", L"Lien web", L" car.", L"Ajouté depuis le presse-papiers", L"Rien de nouveau dans le presse-papiers", L"Copié dans le presse-papiers", L" élément", L" éléments", L"Image copiée", L"Texte copié", L"Fichiers copiés", L"Image collée", L"Texte collé", L"Fichiers collés", L"+ Déposer", L"Étagère", L"Relâchez pour déposer", L"Déposez fichiers, images ou texte", L"Ou cliquez pour coller du presse-papiers", L"Presse-papiers", L"Étagère", L"Presse-papiers (Win+V)", L"Tout", L"Texte", L"Images", L"Fichiers", L"Audio", L"Aucun élément dans cette catégorie", L"L'historique du presse-papiers est vide", L"Déposez des fichiers, images ou texte ici", L"Cliquez pour coller • Glissez pour déposer", L"+ Déposer plus", L"Glissez vers l'application cible", L"Tout glisser (", L" éléments)", L"Image", L" fichier", L" fichiers", L"Fichiers coupés", L"Contenu protégé", L"Privé", L"Texte coupé", L"Image du presse-papiers", L"Image coupée", L"Étagère épinglée", L"Étagère détachée", L"Historique du presse-papiers effacé", L"Étagère vidée" },
    // de (Deutsch)
    { L"Ordner", L"Weblink", L" Zeichen", L"Aus Zwischenablage hinzugefügt", L"Nichts Neues in der Zwischenablage", L"In Zwischenablage kopiert", L" Element", L" Elemente", L"Bild kopiert", L"Text kopiert", L"Dateien kopiert", L"Bild eingefügt", L"Text eingefügt", L"Dateien eingefügt", L"+ Ablegen", L"Ablage", L"Loslassen zum Ablegen", L"Dateien, Bilder oder Text ablegen", L"Oder aus Zwischenablage einfügen", L"Zwischenablage", L"Ablage", L"Zwischenablage (Win+V)", L"Alle", L"Text", L"Bilder", L"Dateien", L"Audio", L"Keine Elemente in dieser Kategorie", L"Zwischenablageverlauf ist leer", L"Dateien, Bilder oder Text hier ablegen", L"Klicken zum Einfügen • Ziehen zum Ablegen", L"+ Mehr ablegen", L"In Ziel-App ziehen", L"Alle ziehen (", L" Elemente)", L"Bild", L" Datei", L" Dateien", L"Dateien ausgeschnitten", L"Geschützter Inhalt", L"Privat", L"Text ausgeschnitten", L"Zwischenablage-Bild", L"Bild ausgeschnitten", L"Ablage angeheftet", L"Ablage gelöst", L"Zwischenablageverlauf geleert", L"Ablage geleert" },
    // ja (日本語)
    { L"フォルダー", L"Webリンク", L" 文字", L"クリップボードから追加", L"新しい項目はありません", L"クリップボードにコピーしました", L" 件", L" 件", L"画像をコピー", L"テキストをコピー", L"ファイルをコピー", L"画像を貼り付け", L"テキストを貼り付け", L"ファイルを貼り付け", L"+ ドロップ", L"シェルフ", L"離して一時保存", L"ファイル・画像・テキストをドロップ", L"またはクリップボードから貼り付け", L"クリップボード", L"ドロップシェルフ", L"クリップボード (Win+V)", L"すべて", L"テキスト", L"画像", L"ファイル", L"音声", L"このカテゴリに項目はありません", L"クリップボード履歴は空です", L"ここにファイル・画像・テキストをドロップ", L"クリックで貼り付け • ドラッグでドロップ", L"+ さらに追加", L"対象アプリへドラッグ", L"すべてドラッグ (", L" 件)", L"画像", L" 個", L" 個", L"ファイルを切り取り", L"保護されたコンテンツ", L"非公開", L"テキストを切り取り", L"クリップボード画像", L"画像を切り取り", L"シェルフを固定", L"固定を解除", L"クリップボード履歴を削除しました", L"シェルフを空にしました" },
    // ko (한국어)
    { L"폴더", L"웹 링크", L"자", L"클립보드에서 추가됨", L"클립보드에 새 항목 없음", L"클립보드에 복사됨", L"개", L"개", L"이미지 복사됨", L"텍스트 복사됨", L"파일 복사됨", L"이미지 붙여넣기", L"텍스트 붙여넣기", L"파일 붙여넣기", L"+ 놓기", L"선반", L"놓아서 보관", L"파일, 이미지, 텍스트 놓기", L"또는 클립보드에서 붙여넣기", L"클립보드", L"드롭 선반", L"클립보드 (Win+V)", L"전체", L"텍스트", L"이미지", L"파일", L"오디오", L"이 카테고리에 항목이 없습니다", L"클립보드 기록이 비어 있습니다", L"여기에 파일, 이미지 또는 텍스트를 놓으세요", L"클릭하여 붙여넣기 • 드래그하여 놓기", L"+ 더 놓기", L"대상 앱으로 드래그", L"모두 드래그 (", L"개)", L"이미지", L"개 파일", L"개 파일", L"파일 잘라내기", L"보호된 콘텐츠", L"비공개", L"텍스트 잘라내기", L"클립보드 이미지", L"이미지 잘라내기", L"선반 고정됨", L"선반 고정 해제", L"클립보드 기록을 모두 삭제했습니다", L"선반을 모두 비웠습니다" },
    // pt (Português)
    { L"Pasta", L"Link da Web", L" caract.", L"Adicionado da área de transferência", L"Nada novo na área de transferência", L"Copiado para a área de transferência", L" item", L" itens", L"Imagem copiada", L"Texto copiado", L"Arquivos copiados", L"Imagem colada", L"Texto colado", L"Arquivos colados", L"+ Soltar", L"Prateleira", L"Solte para guardar", L"Solte arquivos, imagens ou texto", L"Ou clique para colar da área de transferência", L"Área de transf.", L"Prateleira", L"Área de transf. (Win+V)", L"Tudo", L"Texto", L"Imagens", L"Arquivos", L"Áudio", L"Nenhum item nesta categoria", L"O histórico está vazio", L"Solte arquivos, imagens ou texto aqui", L"Clique para colar • Arraste para soltar", L"+ Soltar mais", L"Arraste para o aplicativo de destino", L"Arrastar tudo (", L" itens)", L"Imagem", L" arquivo", L" arquivos", L"Arquivos recortados", L"Conteúdo protegido", L"Privado", L"Texto recortado", L"Imagem da área de transf.", L"Imagem recortada", L"Prateleira fixada", L"Prateleira desafixada", L"Histórico limpo", L"Prateleira limpa" },
    // ru (Русский)
    { L"Папка", L"Веб-ссылка", L" симв.", L"Добавлено из буфера", L"В буфере нет нового", L"Скопировано в буфер", L" эл.", L" эл.", L"Изображение скопировано", L"Текст скопирован", L"Файлы скопированы", L"Изображение вставлено", L"Текст вставлен", L"Файлы вставлены", L"+ Бросить", L"Полка", L"Отпустите для добавления", L"Перетащите файлы, фото или текст", L"Или вставьте из буфера обмена", L"Буфер обмена", L"Полка", L"Буфер (Win+V)", L"Все", L"Текст", L"Фото", L"Файлы", L"Аудио", L"В этой категории пусто", L"История буфера обмена пуста", L"Перетащите файлы, фото или текст сюда", L"Клик — вставить • Перетащите — бросить", L"+ Добавить ещё", L"Перетащите в нужное окно", L"Перетащить все (", L" эл.)", L"Изображение", L" файл", L" файлов", L"Файлы вырезаны", L"Защищённый контент", L"Приватно", L"Текст вырезан", L"Изображение из буфера", L"Изображение вырезано", L"Полка закреплена", L"Полка откреплена", L"История буфера очищена", L"Полка очищена" },
    // it (Italiano)
    { L"Cartella", L"Link web", L" car.", L"Aggiunto dagli appunti", L"Nessuna novità negli appunti", L"Copiato negli appunti", L" elem.", L" elem.", L"Immagine copiata", L"Testo copiato", L"File copiati", L"Immagine incollata", L"Testo incollato", L"File incollati", L"+ Rilascia", L"Mensola", L"Rilascia per salvare", L"Trascina file, immagini o testo", L"O clicca per incollare dagli appunti", L"Appunti", L"Mensola", L"Appunti (Win+V)", L"Tutto", L"Testo", L"Immagini", L"File", L"Audio", L"Nessun elemento in questa categoria", L"Cronologia appunti vuota", L"Trascina qui file, immagini o testo", L"Clic per incollare • Trascina per rilasciare", L"+ Rilascia altro", L"Trascina nell'app di destinazione", L"Trascina tutto (", L" elem.)", L"Immagine", L" file", L" file", L"File tagliati", L"Contenuto protetto", L"Privato", L"Testo tagliato", L"Immagine appunti", L"Immagine tagliata", L"Mensola fissata", L"Mensola sbloccata", L"Cronologia appunti svuotata", L"Mensola svuotata" },
    // ar (العربية)
    { L"مجلد", L"رابط ويب", L" حرف", L"تمت الإضافة من الحافظة", L"لا جديد في الحافظة", L"تم النسخ إلى الحافظة", L" عنصر", L" عناصر", L"تم نسخ الصورة", L"تم نسخ النص", L"تم نسخ الملفات", L"تم لصق الصورة", L"تم لصق النص", L"تم لصق الملفات", L"+ إفلات", L"الرف", L"أفلت للحفظ", L"أفلت الملفات أو الصور أو النص", L"أو انقر للصق من الحافظة", L"الحافظة", L"رف الإفلات", L"الحافظة (Win+V)", L"الكل", L"نص", L"صور", L"ملفات", L"صوت", L"لا توجد عناصر في هذه الفئة", L"سجل الحافظة فارغ", L"أفلت الملفات أو الصور أو النص هنا", L"انقر للصق • اسحب للإفلات", L"+ إفلات المزيد", L"اسحب إلى التطبيق المطلوب", L"سحب الكل (", L" عناصر)", L"صورة", L" ملف", L" ملفات", L"تم قص الملفات", L"محتوى محمي", L"خاص", L"تم قص النص", L"صورة من الحافظة", L"تم قص الصورة", L"تم تثبيت الرف", L"تم إلغاء التثبيت", L"تم مسح سجل الحافظة", L"تم تفريغ الرف" },
    // hi (हिन्दी)
    { L"फ़ोल्डर", L"वेब लिंक", L" अक्षर", L"क्लिपबोर्ड से जोड़ा गया", L"क्लिपबोर्ड में कुछ नया नहीं", L"क्लिपबोर्ड पर कॉपी किया गया", L" आइटम", L" आइटम", L"छवि कॉपी की गई", L"टेक्स्ट कॉपी किया गया", L"फ़ाइलें कॉपी की गईं", L"छवि पेस्ट की गई", L"टेक्स्ट पेस्ट किया गया", L"फ़ाइलें पेस्ट की गईं", L"+ छोड़ें", L"शेल्फ़", L"रखने के लिए छोड़ें", L"फ़ाइलें, छवियाँ, टेक्स्ट छोड़ें", L"या क्लिपबोर्ड से पेस्ट करें", L"क्लिपबोर्ड", L"ड्रॉप शेल्फ़", L"क्लिपबोर्ड (Win+V)", L"सभी", L"टेक्स्ट", L"छवियाँ", L"फ़ाइलें", L"ऑडियो", L"इस श्रेणी में कोई आइटम नहीं", L"क्लिपबोर्ड इतिहास खाली है", L"फ़ाइलें, छवियाँ या टेक्स्ट यहाँ छोड़ें", L"पेस्ट करने के लिए क्लिक करें • ड्रैग करें", L"+ और जोड़ें", L"लक्ष्य ऐप में ड्रैग करें", L"सभी ड्रैग करें (", L" आइटम)", L"छवि", L" फ़ाइल", L" फ़ाइलें", L"फ़ाइलें कट की गईं", L"सुरक्षित सामग्री", L"निजी", L"टेक्स्ट कट किया गया", L"क्लिपबोर्ड छवि", L"छवि कट की गई", L"शेल्फ़ पिन किया गया", L"शेल्फ़ अनपिन किया गया", L"क्लिपबोर्ड इतिहास साफ़ किया गया", L"शेल्फ़ खाली किया गया" },
    // id (Bahasa Indonesia)
    { L"Folder", L"Tautan Web", L" karakter", L"Ditambahkan dari Clipboard", L"Tidak ada yang baru di Clipboard", L"Disalin ke Clipboard", L" item", L" item", L"Gambar Disalin", L"Teks Disalin", L"File Disalin", L"Gambar Ditempel", L"Teks Ditempel", L"File Ditempel", L"+ Lepas", L"Rak", L"Lepaskan untuk menyimpan", L"Lepas file, gambar, teks", L"Atau klik tempel dari Clipboard", L"Clipboard", L"Rak Lepas", L"Clipboard (Win+V)", L"Semua", L"Teks", L"Gambar", L"File", L"Audio", L"Tidak ada item di kategori ini", L"Riwayat clipboard kosong", L"Lepas file, gambar, atau teks di sini", L"Klik untuk tempel • Seret untuk lepas", L"+ Tambah lagi", L"Seret ke aplikasi tujuan", L"Seret semua (", L" item)", L"Gambar", L" file", L" file", L"File Dipotong", L"Konten terlindungi", L"Privat", L"Teks Dipotong", L"Gambar clipboard", L"Gambar Dipotong", L"Rak Disematkan", L"Sematkan Dilepas", L"Riwayat clipboard dihapus", L"Rak dikosongkan" },
    // tr (Türkçe)
    { L"Klasör", L"Web Bağlantısı", L" karakter", L"Panodan eklendi", L"Panoda yeni öğe yok", L"Panoya kopyalandı", L" öğe", L" öğe", L"Görsel Kopyalandı", L"Metin Kopyalandı", L"Dosyalar Kopyalandı", L"Görsel Yapıştırıldı", L"Metin Yapıştırıldı", L"Dosyalar Yapıştırıldı", L"+ Bırak", L"Raf", L"Eklemek için bırak", L"Dosya, görsel veya metin bırak", L"Veya panodan yapıştır", L"Pano", L"Bırakma Rafı", L"Pano (Win+V)", L"Tümü", L"Metin", L"Görsel", L"Dosya", L"Ses", L"Bu kategoride öğe yok", L"Pano geçmişi boş", L"Dosya, görsel veya metni buraya bırakın", L"Yapıştır için tıkla • Bırakmak için sürükle", L"+ Daha fazla bırak", L"Hedef uygulamaya sürükle", L"Tümünü sürükle (", L" öğe)", L"Görsel", L" dosya", L" dosya", L"Dosyalar Kesildi", L"Korumalı içerik", L"Gizli", L"Metin Kesildi", L"Pano görseli", L"Görsel Kesildi", L"Raf Sabitlendi", L"Sabitleme Kaldırıldı", L"Pano geçmişi temizlendi", L"Raf temizlendi" },
    // pl (Polski)
    { L"Folder", L"Link WWW", L" znaków", L"Dodano ze schowka", L"Brak nowych danych w schowku", L"Skopiowano do schowka", L" elem.", L" elem.", L"Skopiowano obraz", L"Skopiowano tekst", L"Skopiowano pliki", L"Wklejono obraz", L"Wklejono tekst", L"Wklejono pliki", L"+ Upuść", L"Półka", L"Puść, aby dodać", L"Upuść pliki, obrazy lub tekst", L"Lub kliknij, aby wkleić ze schowka", L"Schowek", L"Półka", L"Schowek (Win+V)", L"Wszystko", L"Tekst", L"Obrazy", L"Pliki", L"Audio", L"Brak elementów w tej kategorii", L"Historia schowka jest pusta", L"Upuść pliki, obrazy lub tekst tutaj", L"Kliknij, aby wkleić • Przeciągnij", L"+ Upuść więcej", L"Przeciągnij do aplikacji docelowej", L"Przeciągnij wszystko (", L" elem.)", L"Obraz", L" plik", L" plików", L"Wycięto pliki", L"Chroniona zawartość", L"Prywatne", L"Wycięto tekst", L"Obraz ze schowka", L"Wycięto obraz", L"Półka przypięta", L"Półka odpięta", L"Wyczyszczono historię schowka", L"Półka wyczyszczona" },
    // nl (Nederlands)
    { L"Map", L"Weblink", L" tekens", L"Toegevoegd van klembord", L"Niets nieuws op klembord", L"Gekopieerd naar klembord", L" item", L" items", L"Afbeelding gekopieerd", L"Tekst gekopieerd", L"Bestanden gekopieerd", L"Afbeelding geplakt", L"Tekst geplakt", L"Bestanden geplakt", L"+ Neerzetten", L"Plank", L"Loslaten om te bewaren", L"Sleep bestanden, afbeeldingen of tekst", L"Of klik om van klembord te plakken", L"Klembord", L"Dropplank", L"Klembord (Win+V)", L"Alles", L"Tekst", L"Foto's", L"Bestanden", L"Audio", L"Geen items in deze categorie", L"Klembordgeschiedenis is leeg", L"Sleep bestanden, foto's of tekst hierheen", L"Klik om te plakken • Sleep om neer te zetten", L"+ Meer toevoegen", L"Sleep naar doel-app", L"Alles slepen (", L" items)", L"Afbeelding", L" bestand", L" bestanden", L"Bestanden geknipt", L"Beveiligde inhoud", L"Privé", L"Tekst geknipt", L"Klembordafbeelding", L"Afbeelding geknipt", L"Plank vastgezet", L"Plank losgemaakt", L"Klembordgeschiedenis gewist", L"Plank geleegd" },
    // th (ไทย)
    { L"โฟลเดอร์", L"ลิงก์เว็บ", L" ตัวอักษร", L"เพิ่มจากคลิปบอร์ดแล้ว", L"ไม่มีข้อมูลใหม่ในคลิปบอร์ด", L"คัดลอกไปยังคลิปบอร์ดแล้ว", L" รายการ", L" รายการ", L"คัดลอกรูปภาพแล้ว", L"คัดลอกข้อความแล้ว", L"คัดลอกไฟล์แล้ว", L"วางรูปภาพแล้ว", L"วางข้อความแล้ว", L"วางไฟล์แล้ว", L"+ วาง", L"ชั้นวาง", L"ปล่อยเพื่อพักไฟล์", L"วางไฟล์ รูปภาพ หรือข้อความ", L"หรือคลิกวางจากคลิปบอร์ด", L"คลิปบอร์ด", L"ชั้นวางไฟล์", L"คลิปบอร์ด (Win+V)", L"ทั้งหมด", L"ข้อความ", L"รูปภาพ", L"ไฟล์", L"เสียง", L"ไม่มีรายการในหมวดหมู่นี้", L"ประวัติคลิปบอร์ดว่างเปล่า", L"วางไฟล์ รูปภาพ หรือข้อความที่นี่", L"คลิกเพื่อวาง • ลากเพื่อปล่อย", L"+ เพิ่มอีก", L"ลากไปยังแอปปลายทาง", L"ลากทั้งหมด (", L" รายการ)", L"รูปภาพ", L" ไฟล์", L" ไฟล์", L"ตัดไฟล์แล้ว", L"เนื้อหาที่ได้รับการปกป้อง", L"ส่วนตัว", L"ตัดข้อความแล้ว", L"รูปภาพจากคลิปบอร์ด", L"ตัดรูปภาพแล้ว", L"ปักหมุดชั้นวางแล้ว", L"เลิกปักหมุดแล้ว", L"ล้างประวัติคลิปบอร์ดแล้ว", L"ล้างชั้นวางแล้ว" },
    // uk (Українська)
    { L"Папка", L"Вебпосилання", L" симв.", L"Додано з буфера", L"У буфері немає нового", L"Скопійовано в буфер", L" ел.", L" ел.", L"Зображення скопійовано", L"Текст скопійовано", L"Файли скопійовано", L"Зображення вставлено", L"Текст вставлено", L"Файли вставлено", L"+ Кинути", L"Полиця", L"Відпустіть для додавання", L"Перетягніть файли, фото чи текст", L"Або вставте з буфера обміну", L"Буфер обміну", L"Полиця", L"Буфер (Win+V)", L"Усі", L"Текст", L"Фото", L"Файли", L"Аудіо", L"У цій категорії немає елементів", L"Історія буфера обміну порожня", L"Перетягніть файли, фото чи текст сюди", L"Клік — вставити • Перетягніть — кинути", L"+ Додати ще", L"Перетягніть у потрібну програму", L"Перетягнути все (", L" ел.)", L"Зображення", L" файл", L" файлів", L"Файли вирізано", L"Захищений вміст", L"Приватно", L"Текст вирізано", L"Зображення з буфера", L"Зображення вирізано", L"Полицю закріплено", L"Полицю відкріплено", L"Історію буфера очищено", L"Полицю очищено" },
    // sv (Svenska)
    { L"Mapp", L"Webblänk", L" tecken", L"Tillagd från urklipp", L"Inget nytt i urklipp", L"Kopierad till urklipp", L" objekt", L" objekt", L"Bild kopierad", L"Text kopierad", L"Filer kopierade", L"Bild inklistrad", L"Text inklistrad", L"Filer inklistrade", L"+ Släpp", L"Hylla", L"Släpp för att spara", L"Släpp filer, bilder eller text", L"Eller klistra in från urklipp", L"Urklipp", L"Släpphylla", L"Urklipp (Win+V)", L"Alla", L"Text", L"Bilder", L"Filer", L"Ljud", L"Inga objekt i denna kategori", L"Urklippshistoriken är tom", L"Släpp filer, bilder eller text här", L"Klicka för att klistra in • Dra för att släppa", L"+ Lägg till mer", L"Dra till målappen", L"Dra alla (", L" objekt)", L"Bild", L" fil", L" filer", L"Filer urklippta", L"Skyddat innehåll", L"Privat", L"Text urklippt", L"Urklippsbild", L"Bild urklippt", L"Hylla fäst", L"Hylla lossad", L"Urklippshistoriken rensad", L"Hyllan rensad" },
    // cs (Čeština)
    { L"Složka", L"Webový odkaz", L" znaků", L"Přidáno ze schránky", L"Ve schránce nic nového", L"Zkopírováno do schránky", L" pol.", L" pol.", L"Obrázek zkopírován", L"Text zkopírován", L"Soubory zkopírovány", L"Obrázek vložen", L"Text vložen", L"Soubory vloženy", L"+ Pustit", L"Polička", L"Pusťte pro uložení", L"Přetáhněte soubory, obrázky nebo text", L"Nebo klikněte pro vložení ze schránky", L"Schránka", L"Polička", L"Schránka (Win+V)", L"Vše", L"Text", L"Obrázky", L"Soubory", L"Audio", L"V této kategorii nic není", L"Historie schránky je prázdná", L"Sem přetáhněte soubory, obrázky nebo text", L"Kliknutím vložíte • Přetažením pustíte", L"+ Přidat další", L"Přetáhněte do cílové aplikace", L"Přetáhnout vše (", L" pol.)", L"Obrázek", L" soubor", L" souborů", L"Soubory vyjmuty", L"Chráněný obsah", L"Soukromé", L"Text vyjmut", L"Obrázek ze schránky", L"Obrázek vyjmut", L"Polička připnuta", L"Polička odepnuta", L"Historie schránky vymazána", L"Polička vyprázdněna" },
    // ro (Română)
    { L"Folder", L"Link web", L" caract.", L"Adăugat din clipboard", L"Nimic nou în clipboard", L"Copiat în clipboard", L" elem.", L" elem.", L"Imagine copiată", L"Text copiat", L"Fișiere copiate", L"Imagine lipită", L"Text lipit", L"Fișiere lipite", L"+ Plasează", L"Raft", L"Eliberează pentru salvare", L"Trage fișiere, imagini sau text", L"Sau apasă pentru a lipi din clipboard", L"Clipboard", L"Raft", L"Clipboard (Win+V)", L"Toate", L"Text", L"Imagini", L"Fișiere", L"Audio", L"Niciun element în această categorie", L"Istoricul clipboard este gol", L"Trage fișiere, imagini sau text aici", L"Clic pentru lipire • Trage pentru plasare", L"+ Adaugă mai multe", L"Trage în aplicația țintă", L"Trage toate (", L" elem.)", L"Imagine", L" fișier", L" fișiere", L"Fișiere decupate", L"Conținut protejat", L"Privat", L"Text decupat", L"Imagine din clipboard", L"Imagine decupată", L"Raft fixat", L"Raft eliberat", L"Istoricul clipboard a fost șters", L"Raftul a fost golit" },
    // hu (Magyar)
    { L"Mappa", L"Webhivatkozás", L" karakter", L"Vágólapról hozzáadva", L"Nincs új elem a vágólapon", L"Vágólapra másolva", L" elem", L" elem", L"Kép másolva", L"Szöveg másolva", L"Fájlok másolva", L"Kép beillesztve", L"Szöveg beillesztve", L"Fájlok beillesztve", L"+ Dobás", L"Polc", L"Engedd el a mentéshez", L"Húzz ide fájlokat, képeket, szöveget", L"Vagy illessz be a vágólapról", L"Vágólap", L"Polc", L"Vágólap (Win+V)", L"Mind", L"Szöveg", L"Képek", L"Fájlok", L"Hang", L"Nincs elem ebben a kategóriában", L"A vágólap előzménye üres", L"Húzz ide fájlokat, képeket vagy szöveget", L"Kattintás: beillesztés • Húzás: áthelyezés", L"+ Továbbiak hozzáadása", L"Húzd a célalkalmazásba", L"Összes húzása (", L" elem)", L"Kép", L" fájl", L" fájl", L"Fájlok kivágva", L"Védett tartalom", L"Privát", L"Szöveg kivágva", L"Vágólap kép", L"Kép kivágva", L"Polc rögzítve", L"Rögzítés feloldva", L"Vágólap előzménye törölve", L"Polc kiürítve" },
    // el (Ελληνικά)
    { L"Φάκελος", L"Σύνδεσμος", L" χαρ.", L"Προστέθηκε από το πρόχειρο", L"Τίποτα νέο στο πρόχειρο", L"Αντιγράφηκε στο πρόχειρο", L" στοιχ.", L" στοιχ.", L"Αντιγραφή εικόνας", L"Αντιγραφή κειμένου", L"Αντιγραφή αρχείων", L"Επικόλληση εικόνας", L"Επικόλληση κειμένου", L"Επικόλληση αρχείων", L"+ Απόθεση", L"Ράφι", L"Αφήστε για αποθήκευση", L"Σύρετε αρχεία, εικόνες ή κείμενο", L"Ή επικολλήστε από το πρόχειρο", L"Πρόχειρο", L"Ράφι", L"Πρόχειρο (Win+V)", L"Όλα", L"Κείμενο", L"Εικόνες", L"Αρχεία", L"Ήχος", L"Κανένα στοιχείο στην κατηγορία", L"Το ιστορικό προχείρου είναι κενό", L"Αποθέστε αρχεία, εικόνες ή κείμενο εδώ", L"Κλικ για επικόλληση • Σύρετε για απόθεση", L"+ Προσθήκη κι άλλων", L"Σύρετε στην εφαρμογή προορισμού", L"Σύρετε όλα (", L" στοιχ.)", L"Εικόνα", L" αρχείο", L" αρχεία", L"Αποκοπή αρχείων", L"Προστατευμένο", L"Ιδιωτικό", L"Αποκοπή κειμένου", L"Εικόνα προχείρου", L"Αποκοπή εικόνας", L"Καρφιτσώθηκε", L"Ξεκαρφιτσώθηκε", L"Το ιστορικό προχείρου διαγράφηκε", L"Το ράφι καθαρίστηκε" },
    // he (עברית)
    { L"תיקייה", L"קישור אינטרנט", L" תווים", L"נוסף מהלוח", L"אין תוכן חדש בלוח", L"הועתק ללוח", L" פריט", L" פריטים", L"התמונה הועתקה", L"הטקסט הועתק", L"הקבצים הועתקו", L"התמונה הודבקה", L"הטקסט הודבק", L"הקבצים הודבקו", L"+ שחרר", L"מדף", L"שחרר לשמירה", L"גרור קבצים, תמונות או טקסט", L"או לחץ להדבקה מהלוח", L"לוח", L"מדף גרירה", L"לוח (Win+V)", L"הכל", L"טקסט", L"תמונות", L"קבצים", L"שמע", L"אין פריטים בקטגוריה זו", L"היסטוריית הלוח ריקה", L"גרור לכאן קבצים, תמונות או טקסט", L"לחץ להדבקה • גרור לשחרור", L"+ הוסף עוד", L"גרור לאפליקציית היעד", L"גרור הכל (", L" פריטים)", L"תמונה", L" קובץ", L" קבצים", L"הקבצים נגזרו", L"תוכן מוגן", L"פרטי", L"הטקסט נגזר", L"תמונה מהלוח", L"התמונה נגזרה", L"המדף הוצמד", L"ההצמדה בוטלה", L"היסטוריית הלוח נוקתה", L"המדף נוקה" },
    // da (Dansk)
    { L"Mappe", L"Weblink", L" tegn", L"Tilføjet fra udklipsholder", L"Intet nyt i udklipsholder", L"Kopieret til udklipsholder", L" element", L" elementer", L"Billede kopieret", L"Tekst kopieret", L"Filer kopieret", L"Billede indsat", L"Tekst indsat", L"Filer indsat", L"+ Slip", L"Hylde", L"Slip for at gemme", L"Slip filer, billeder eller tekst", L"Eller klik for at indsætte fra udklipsholder", L"Udklipsholder", L"Sliphylde", L"Udklipsholder (Win+V)", L"Alle", L"Tekst", L"Billeder", L"Filer", L"Lyd", L"Ingen elementer i denne kategori", L"Udklipshistorikken er tom", L"Slip filer, billeder eller tekst her", L"Klik for at indsætte • Træk for at slippe", L"+ Tilføj flere", L"Træk til målappen", L"Træk alle (", L" elementer)", L"Billede", L" fil", L" filer", L"Filer klippet", L"Beskyttet indhold", L"Privat", L"Tekst klippet", L"Udklipsbillede", L"Billede klippet", L"Hylde fastgjort", L"Hylde frigjort", L"Udklipshistorik ryddet", L"Hylde ryddet" },
    // fi (Suomi)
    { L"Kansio", L"Verkkolinkki", L" merkkiä", L"Lisätty leikepöydältä", L"Ei uutta leikepöydällä", L"Kopioitu leikepöydälle", L" kohde", L" kohdetta", L"Kuva kopioitu", L"Teksti kopioitu", L"Tiedostot kopioitu", L"Kuva liitetty", L"Teksti liitetty", L"Tiedostot liitetty", L"+ Pudota", L"Hylly", L"Pudota tallentaaksesi", L"Pudota tiedostoja, kuvia tai tekstiä", L"Tai liitä leikepöydältä", L"Leikepöytä", L"Pudotushylly", L"Leikepöytä (Win+V)", L"Kaikki", L"Teksti", L"Kuvat", L"Tiedostot", L"Ääni", L"Ei kohteita tässä luokassa", L"Leikepöydän historia on tyhjä", L"Pudota tiedostoja, kuvia tai tekstiä tähän", L"Liitä napsauttamalla • Pudota vetämällä", L"+ Lisää kohteita", L"Vedä kohdesovellukseen", L"Vedä kaikki (", L" kohdetta)", L"Kuva", L" tiedosto", L" tiedostoa", L"Tiedostot leikattu", L"Suojattu sisältö", L"Yksityinen", L"Teksti leikattu", L"Leikepöytäkuva", L"Kuva leikattu", L"Hylly kiinnitetty", L"Kiinnitys poistettu", L"Leikepöydän historia tyhjennetty", L"Hylly tyhjennetty" },
    // no (Norsk)
    { L"Mappe", L"Nettlenke", L" tegn", L"Lagt til fra utklippstavle", L"Ingenting nytt på utklippstavlen", L"Kopiert til utklippstavle", L" element", L" elementer", L"Bilde kopiert", L"Tekst kopiert", L"Filer kopiert", L"Bilde limt inn", L"Tekst limt inn", L"Filer limt inn", L"+ Slipp", L"Hylle", L"Slipp for å lagre", L"Slipp filer, bilder eller tekst", L"Eller klikk for å lime inn fra utklippstavle", L"Utklippstavle", L"Slipphylle", L"Utklippstavle (Win+V)", L"Alle", L"Tekst", L"Bilder", L"Filer", L"Lyd", L"Ingen elementer i denne kategorien", L"Utklippshistorikken er tom", L"Slipp filer, bilder eller tekst her", L"Klikk for å lime inn • Dra for å slippe", L"+ Legg til flere", L"Dra til målappen", L"Dra alle (", L" elementer)", L"Bilde", L" fil", L" filer", L"Filer klippet ut", L"Beskyttet innhold", L"Privat", L"Tekst klippet ut", L"Utklippsbilde", L"Bilde klippet ut", L"Hylle festet", L"Hylle løsnet", L"Utklippshistorikk tømt", L"Hylle tømt" },
    // ms (Bahasa Melayu)
    { L"Folder", L"Pautan Web", L" aksara", L"Ditambah daripada Papan Keratan", L"Tiada kandungan baharu", L"Disalin ke Papan Keratan", L" item", L" item", L"Imej Disalin", L"Teks Disalin", L"Fail Disalin", L"Imej Ditampal", L"Teks Ditampal", L"Fail Ditampal", L"+ Lepas", L"Rak", L"Lepaskan untuk simpan", L"Lepas fail, imej, teks", L"Atau klik tampal daripada Papan Keratan", L"Papan Keratan", L"Rak Lepas", L"Papan Keratan (Win+V)", L"Semua", L"Teks", L"Imej", L"Fail", L"Audio", L"Tiada item dalam kategori ini", L"Sejarah papan keratan kosong", L"Lepas fail, imej atau teks di sini", L"Klik untuk tampal • Seret untuk lepas", L"+ Tambah lagi", L"Seret ke aplikasi sasaran", L"Seret semua (", L" item)", L"Imej", L" fail", L" fail", L"Fail Dipotong", L"Kandungan dilindungi", L"Peribadi", L"Teks Dipotong", L"Imej papan keratan", L"Imej Dipotong", L"Rak Disemat", L"Sematan Dibuka", L"Sejarah papan keratan dikosongkan", L"Rak dikosongkan" }
};

static const wchar_t* PickText(bool /*vi*/, const wchar_t* viText, const wchar_t* enText) {
    int idx = ResolveActiveLangIndex(g_settings.lang);
    if (idx == 0) return enText;
    if (idx == 1) return viText;
    if (idx >= 2 && idx < 30 && enText) {
        for (size_t i = 0; i < kLocStringCount; ++i) {
            if (wcscmp(enText, kLocEnKeys[i]) == 0) {
                return kLocExtraTables[idx - 2][i];
            }
        }
    }
    return enText;
}

static bool UseMacTrafficButtons() {
    if (g_settings.buttonStyle == ButtonStyle::MacOs) return true;
    if (g_settings.buttonStyle == ButtonStyle::Windows) return false;
    return (g_settings.uiStyle == UiStyle::MacOs);
}

static void RefreshSystemThemeAndAccent() {
    DWORD val = 0;
    DWORD cb = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme",
                     RRF_RT_REG_DWORD, nullptr, &val, &cb) == ERROR_SUCCESS) {
        g_systemLightTheme = (val != 0);
    } else {
        cb = sizeof(val);
        if (RegGetValueW(HKEY_CURRENT_USER,
                         L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                         L"SystemUsesLightTheme",
                         RRF_RT_REG_DWORD, nullptr, &val, &cb) == ERROR_SUCCESS) {
            g_systemLightTheme = (val != 0);
        } else {
            g_systemLightTheme = false;
        }
    }

    // Read real Windows accent color (ABGR in Registry DWM\AccentColor)
    DWORD abgr = 0;
    cb = sizeof(abgr);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"AccentColor",
                     RRF_RT_REG_DWORD, nullptr, &abgr, &cb) == ERROR_SUCCESS && abgr != 0) {
        float r = static_cast<float>(abgr & 0xFF) / 255.0f;
        float g = static_cast<float>((abgr >> 8) & 0xFF) / 255.0f;
        float b = static_cast<float>((abgr >> 16) & 0xFF) / 255.0f;
        g_winAccentColor = D2D1::ColorF(r, g, b, 1.0f);
    } else {
        DWORD dwmCol = 0;
        BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&dwmCol, &opaque))) {
            float r = static_cast<float>((dwmCol >> 16) & 0xFF) / 255.0f;
            float g = static_cast<float>((dwmCol >> 8) & 0xFF) / 255.0f;
            float b = static_cast<float>(dwmCol & 0xFF) / 255.0f;
            g_winAccentColor = D2D1::ColorF(r, g, b, 1.0f);
        } else {
            g_winAccentColor = D2D1::ColorF(0.00f, 0.47f, 0.84f, 1.0f);
        }
    }
}

static D2D1_COLOR_F ResolveAccentColor(bool isLight) {
    if (g_settings.uiStyle == UiStyle::Windows) {
        if (!isLight) {
            // Slightly brighten Windows accent on dark surfaces for crisp contrast
            return D2D1::ColorF(
                std::min(1.0f, g_winAccentColor.r * 0.78f + 0.24f),
                std::min(1.0f, g_winAccentColor.g * 0.78f + 0.24f),
                std::min(1.0f, g_winAccentColor.b * 0.78f + 0.24f), 1.0f);
        }
        return g_winAccentColor;
    }
    // macOS System Blue (#007AFF light / #0A84FF dark)
    return isLight
        ? D2D1::ColorF(0.00f, 0.478f, 1.00f, 1.0f)
        : D2D1::ColorF(0.04f, 0.518f, 1.00f, 1.0f);
}

static ModSettings ReadSettingsFromWindhawk() {
    auto readStr = [](const wchar_t* name, const wchar_t* defVal) -> std::wstring {
        auto s = WindhawkUtils::StringSetting::make(name);
        return (s.get() && s.get()[0]) ? std::wstring(s.get()) : std::wstring(defVal);
    };

    ModSettings s;
    std::wstring trig = readStr(L"triggerGroup.triggerMode", L"both");
    if (trig == L"auto_drag") s.triggerMode = TriggerMode::AutoDrag;
    else if (trig == L"shake_only") s.triggerMode = TriggerMode::ShakeOnly;
    else s.triggerMode = TriggerMode::Both;

    std::wstring pos = readStr(L"triggerGroup.shelfPosition", L"top_center");
    if (pos == L"near_cursor") s.shelfPosition = ShelfPosition::NearCursor;
    else if (pos == L"edge_right") s.shelfPosition = ShelfPosition::EdgeRight;
    else if (pos == L"edge_left") s.shelfPosition = ShelfPosition::EdgeLeft;
    else if (pos == L"above_taskbar") s.shelfPosition = ShelfPosition::AboveTaskbar;
    else s.shelfPosition = ShelfPosition::TopCenter;

    s.enableEdgeBump = Wh_GetIntSetting(L"triggerGroup.enableEdgeBump") != 0;

    std::wstring sens = readStr(L"triggerGroup.shakeSensitivity", L"medium");
    if (sens == L"high") s.shakeSensitivity = ShakeSensitivity::High;
    else if (sens == L"low") s.shakeSensitivity = ShakeSensitivity::Low;
    else s.shakeSensitivity = ShakeSensitivity::Medium;

    s.enableGlobalHotkey  = Wh_GetIntSetting(L"triggerGroup.enableGlobalHotkey") != 0;
    s.replaceWinV         = Wh_GetIntSetting(L"triggerGroup.replaceWinV") != 0;
    int maxClip           = Wh_GetIntSetting(L"triggerGroup.maxClipboardHistory");
    s.maxClipboardHistory = (maxClip >= 10) ? std::clamp<int>(maxClip, 10, 100) : 30;

    s.autoClearAfterDrop  = Wh_GetIntSetting(L"behaviorGroup.autoClearAfterDrop") != 0;
    s.acceptImageDrops    = Wh_GetIntSetting(L"behaviorGroup.acceptImageDrops") != 0;
    s.acceptTextDrops     = Wh_GetIntSetting(L"behaviorGroup.acceptTextDrops") != 0;
    s.enableCopyPasteAlert = Wh_GetIntSetting(L"behaviorGroup.enableCopyPasteAlert") != 0;
    std::wstring aPos = readStr(L"behaviorGroup.alertIslandPosition", L"auto");
    if (aPos == L"top" || aPos == L"topRight" || aPos == L"topLeft") s.alertIslandPosition = AlertIslandPosition::Top;
    else if (aPos == L"bottom" || aPos == L"bottomRight" || aPos == L"bottomLeft") s.alertIslandPosition = AlertIslandPosition::Bottom;
    else s.alertIslandPosition = AlertIslandPosition::Auto;
    s.showImageThumbnails = Wh_GetIntSetting(L"behaviorGroup.showImageThumbnails") != 0;
    s.hideInFullscreen    = Wh_GetIntSetting(L"behaviorGroup.hideInFullscreen") != 0;

    int maxItems = Wh_GetIntSetting(L"behaviorGroup.maxStagedItems");
    s.maxStagedItems = (maxItems >= 8) ? std::clamp<int>(maxItems, 8, 64) : 32;

    std::wstring uis = readStr(L"appearanceGroup.uiStyle", L"macos");
    s.uiStyle = (uis == L"windows") ? UiStyle::Windows : UiStyle::MacOs;

    std::wstring bts = readStr(L"appearanceGroup.buttonStyle", L"auto");
    if (bts == L"macos") s.buttonStyle = ButtonStyle::MacOs;
    else if (bts == L"windows") s.buttonStyle = ButtonStyle::Windows;
    else s.buttonStyle = ButtonStyle::Auto;

    std::wstring glass = readStr(L"appearanceGroup.glassStyle", L"macos_frosted");
    if (glass == L"liquid_clear") s.glassStyle = GlassStyle::LiquidClear;
    else if (glass == L"solid_soft") s.glassStyle = GlassStyle::SolidSoft;
    else s.glassStyle = GlassStyle::MacOsFrosted;

    std::wstring col = readStr(L"appearanceGroup.colorMode", L"auto");
    if (col == L"dark") s.colorMode = ColorMode::Dark;
    else if (col == L"light") s.colorMode = ColorMode::Light;
    else s.colorMode = ColorMode::Auto;

    std::wstring lang = readStr(L"language", L"");
    if (lang.empty()) lang = readStr(L"appearanceGroup.language", L"auto");
    if (lang == L"en") s.lang = UiLang::En;
    else if (lang == L"vi") s.lang = UiLang::Vi;
    else if (lang == L"zh_cn") s.lang = UiLang::ZhCn;
    else if (lang == L"zh_tw") s.lang = UiLang::ZhTw;
    else if (lang == L"es") s.lang = UiLang::Es;
    else if (lang == L"fr") s.lang = UiLang::Fr;
    else if (lang == L"de") s.lang = UiLang::De;
    else if (lang == L"ja") s.lang = UiLang::Ja;
    else if (lang == L"ko") s.lang = UiLang::Ko;
    else if (lang == L"pt") s.lang = UiLang::Pt;
    else if (lang == L"ru") s.lang = UiLang::Ru;
    else if (lang == L"it") s.lang = UiLang::It;
    else if (lang == L"ar") s.lang = UiLang::Ar;
    else if (lang == L"hi") s.lang = UiLang::Hi;
    else if (lang == L"id") s.lang = UiLang::Id;
    else if (lang == L"tr") s.lang = UiLang::Tr;
    else if (lang == L"pl") s.lang = UiLang::Pl;
    else if (lang == L"nl") s.lang = UiLang::Nl;
    else if (lang == L"th") s.lang = UiLang::Th;
    else if (lang == L"uk") s.lang = UiLang::Uk;
    else if (lang == L"sv") s.lang = UiLang::Sv;
    else if (lang == L"cs") s.lang = UiLang::Cs;
    else if (lang == L"ro") s.lang = UiLang::Ro;
    else if (lang == L"hu") s.lang = UiLang::Hu;
    else if (lang == L"el") s.lang = UiLang::El;
    else if (lang == L"he") s.lang = UiLang::He;
    else if (lang == L"da") s.lang = UiLang::Da;
    else if (lang == L"fi") s.lang = UiLang::Fi;
    else if (lang == L"no") s.lang = UiLang::No;
    else if (lang == L"ms") s.lang = UiLang::Ms;
    else s.lang = UiLang::Auto;

    g_replaceWinVAtomic.store(s.replaceWinV, std::memory_order_relaxed);
    g_enableCopyPasteAlertAtomic.store(s.enableCopyPasteAlert, std::memory_order_relaxed);
    RefreshSystemThemeAndAccent();
    return s;
}

static UINT GetDpiForMonitorOrWindow(HWND hwnd) {
    using GetDpiForWindowFn = UINT(WINAPI*)(HWND);
    static auto fnGetDpiForWindow = reinterpret_cast<GetDpiForWindowFn>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    if (hwnd && fnGetDpiForWindow) {
        UINT d = fnGetDpiForWindow(hwnd);
        if (d >= 96 && d <= 480) return d;
    }
    HDC hdc = GetDC(nullptr);
    UINT dpi = 96;
    if (hdc) {
        dpi = static_cast<UINT>(GetDeviceCaps(hdc, LOGPIXELSX));
        ReleaseDC(nullptr, hdc);
    }
    return (dpi >= 96) ? dpi : 96;
}

static std::wstring FormatFileSizeCompact(uint64_t bytes) {
    wchar_t buf[48] = {};
    if (bytes >= 1024ULL * 1024 * 1024) {
        swprintf_s(buf, L"%.2f GB", static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0));
    } else if (bytes >= 1024ULL * 1024) {
        swprintf_s(buf, L"%.1f MB", static_cast<double>(bytes) / (1024.0 * 1024.0));
    } else if (bytes >= 1024ULL) {
        swprintf_s(buf, L"%.0f KB", static_cast<double>(bytes) / 1024.0);
    } else {
        swprintf_s(buf, L"%llu B", static_cast<unsigned long long>(bytes));
    }
    return buf;
}

static std::wstring GetCurrentUserSidString() {
    std::wstring result;
    HANDLE token = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        alignas(TOKEN_USER) BYTE buf[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
        DWORD len = 0;
        if (GetTokenInformation(token, TokenUser, buf, sizeof(buf), &len)) {
            LPWSTR sid = nullptr;
            if (ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(buf)->User.Sid, &sid)) {
                result = sid;
                LocalFree(sid);
            }
        }
        CloseHandle(token);
    }
    return result;
}

static std::wstring GetTempStagingDir() {
    wchar_t baseDir[MAX_PATH] = {};
    if (Wh_GetModStoragePath(baseDir, ARRAYSIZE(baseDir)) == 0 || !baseDir[0]) {
        return {};
    }
    std::wstring sid = GetCurrentUserSidString();
    if (sid.empty()) return {};

    CreateDirectoryW(baseDir, nullptr);
    std::wstring stagedRoot = std::wstring(baseDir) + L"\\staged";
    CreateDirectoryW(stagedRoot.c_str(), nullptr);

    std::wstring userDir = stagedRoot + L"\\" + sid;
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = FALSE;
    // Protected DACL: full access only to the current user SID and SYSTEM (so Windhawk can remove mod storage on uninstall)
    std::wstring sddl = L"D:P(A;OICI;GA;;;" + sid + L")(A;OICI;GA;;;SY)";
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(
            sddl.c_str(), SDDL_REVISION_1, &sa.lpSecurityDescriptor, nullptr)) {
        CreateDirectoryW(userDir.c_str(), &sa);
        LocalFree(sa.lpSecurityDescriptor);
    } else {
        CreateDirectoryW(userDir.c_str(), nullptr);
    }
    return userDir;
}

static void ClearStagingDirContents() {
    std::wstring dir = GetTempStagingDir();
    if (dir.empty()) return;
    std::wstring pattern = dir + L"\\*";
    WIN32_FIND_DATAW fd{};
    HANDLE hFind = FindFirstFileW(pattern.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) return;
    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
            std::wstring fullPath = dir + L"\\" + fd.cFileName;
            DeleteFileW(fullPath.c_str());
        }
    } while (FindNextFileW(hFind, &fd));
    FindClose(hFind);
}

static std::wstring SanitizeFileName(std::wstring name) {
    for (wchar_t& ch : name) {
        if (ch < 32 || ch == L'<' || ch == L'>' || ch == L':' || ch == L'"' ||
            ch == L'/' || ch == L'\\' || ch == L'|' || ch == L'?' || ch == L'*') {
            ch = L'_';
        }
    }
    while (!name.empty() && (name.back() == L' ' || name.back() == L'.')) name.pop_back();
    if (name.empty()) name = L"dropped_item.bin";
    if (name.size() > 96) name.resize(96);
    return name;
}

// ---------------------------------------------------------------------------
// 2. Low-Level Keyboard Hook Thread for Optional Win + V Replacement
// ---------------------------------------------------------------------------
static HHOOK g_hKbdHook = nullptr;

static LRESULT CALLBACK LowLevelKbdHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    static bool s_lwinDown = false;
    static bool s_rwinDown = false;
    static bool s_vSuppressed = false;

    if (nCode == HC_ACTION && lParam != 0 &&
        g_running.load(std::memory_order_relaxed) &&
        !g_unloading.load(std::memory_order_relaxed)) {
        const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        // Ignore our own injected keys (e.g. 0xE8 Start-cancel mask or Ctrl+V paste)
        if (kb->flags & LLKHF_INJECTED) {
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }

        DWORD vk = kb->vkCode;
        bool isUp = (kb->flags & LLKHF_UP) != 0 || (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (vk == VK_LWIN) s_lwinDown = !isUp;
        else if (vk == VK_RWIN) s_rwinDown = !isUp;

        if (vk == 'V' && g_replaceWinVAtomic.load(std::memory_order_relaxed)) {
            bool winDown   = s_lwinDown || s_rwinDown ||
                             ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) ||
                             ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);
            bool ctrlDown  = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
            bool altDown   = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
            bool shiftDown = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);

            if (!isUp && winDown && !ctrlDown && !altDown && !shiftDown &&
                g_shelfHwnd && IsWindow(g_shelfHwnd)) {
                if (!s_vSuppressed) {
                    s_vSuppressed = true;
                    // Inject unassigned 0xE8 key (PowerToys technique) so releasing Win never triggers Start Menu
                    INPUT dummy[2] = {};
                    dummy[0].type = INPUT_KEYBOARD;
                    dummy[0].ki.wVk = 0xE8;
                    dummy[1].type = INPUT_KEYBOARD;
                    dummy[1].ki.wVk = 0xE8;
                    dummy[1].ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(2, dummy, sizeof(INPUT));

                    PostMessageW(g_shelfHwnd, WM_APP_WINV_SUMMON, 0, 0);
                }
                return 1; // Block Windows' built-in Win+V popup
            } else if (isUp && s_vSuppressed) {
                s_vSuppressed = false;
                return 1;
            }
        }

        // Track Cut (Ctrl+X / Shift+Del) & Paste (Ctrl+V / Shift+Ins) for instant Copy/Cut/Paste toast alerts
        static bool s_pasteKeyHeld = false;
        if (vk == 'V' || vk == VK_INSERT) {
            if (isUp) s_pasteKeyHeld = false;
        }
        if (!isUp && g_enableCopyPasteAlertAtomic.load(std::memory_order_relaxed)) {
            bool winDown   = s_lwinDown || s_rwinDown ||
                             ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) ||
                             ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);
            bool ctrlDown  = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
            bool altDown   = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
            bool shiftDown = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);

            if ((ctrlDown && !altDown && !winDown && vk == 'X') ||
                (shiftDown && !ctrlDown && !altDown && !winDown && vk == VK_DELETE)) {
                g_lastCutKeyTick.store(GetTickCount64(), std::memory_order_relaxed);
            } else if (!s_pasteKeyHeld &&
                       ((ctrlDown && !altDown && !winDown && vk == 'V') ||
                        (shiftDown && !ctrlDown && !altDown && !winDown && vk == VK_INSERT))) {
                s_pasteKeyHeld = true;
                if (g_shelfHwnd && IsWindow(g_shelfHwnd)) {
                    PostMessageW(g_shelfHwnd, WM_APP_PASTE_ALERT, 0, 0);
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static DWORD WINAPI KbdHookThreadProc(LPVOID) {
    MSG msg{};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_hKbdHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKbdHookProc, GetModInstanceHandle(), 0);
    while (g_running.load(std::memory_order_relaxed) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (g_hKbdHook) {
        UnhookWindowsHookEx(g_hKbdHook);
        g_hKbdHook = nullptr;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// 3. WIC Image Decoder / Encoder & Native Shell Icon Extractor
// ---------------------------------------------------------------------------
struct DecodedBgraImage {
    UINT width  = 0;
    UINT height = 0;
    std::vector<DWORD> pixels; // 32bppPBGRA
};

static bool IsSupportedThumbnailExtension(const wchar_t* path) {
    if (!path || !path[0]) return false;
    const wchar_t* ext = PathFindExtensionW(path);
    if (!ext || !ext[0]) return false;
    std::wstring low = ToLowerStr(ext);
    return (low == L".png"  || low == L".jpg"  || low == L".jpeg" ||
            low == L".bmp"  || low == L".gif"  || low == L".webp" ||
            low == L".tif"  || low == L".tiff" || low == L".ico");
}

static bool IsSupportedAudioExtension(const wchar_t* path) {
    if (!path || !path[0]) return false;
    const wchar_t* ext = PathFindExtensionW(path);
    if (!ext || !ext[0]) return false;
    std::wstring low = ToLowerStr(ext);
    return (low == L".mp3"  || low == L".wav"  || low == L".m4a"  ||
            low == L".wma"  || low == L".flac" || low == L".ogg"  ||
            low == L".aac"  || low == L".opus");
}

static std::shared_ptr<DecodedBgraImage> DecodeFileThumbnailWic(const wchar_t* filePath, UINT maxEdge,
                                                                UINT* outOrigW = nullptr, UINT* outOrigH = nullptr) {
    if (!filePath || !filePath[0]) return nullptr;
    IWICImagingFactory* factory = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&factory))) || !factory) {
        return nullptr;
    }

    std::shared_ptr<DecodedBgraImage> result;
    IWICBitmapDecoder* decoder = nullptr;
    if (SUCCEEDED(factory->CreateDecoderFromFilename(filePath, nullptr, GENERIC_READ,
                                                     WICDecodeMetadataCacheOnDemand, &decoder)) && decoder) {
        IWICBitmapFrameDecode* frame = nullptr;
        if (SUCCEEDED(decoder->GetFrame(0, &frame)) && frame) {
            UINT srcW = 0, srcH = 0;
            if (SUCCEEDED(frame->GetSize(&srcW, &srcH)) && srcW > 0 && srcH > 0 && srcW <= 16384 && srcH <= 16384) {
                if (outOrigW) *outOrigW = srcW;
                if (outOrigH) *outOrigH = srcH;
                double scale = (maxEdge > 0)
                    ? std::min(1.0, static_cast<double>(maxEdge) / static_cast<double>(std::max(srcW, srcH)))
                    : 1.0;
                UINT dstW = std::max<UINT>(1u, static_cast<UINT>(std::lround(srcW * scale)));
                UINT dstH = std::max<UINT>(1u, static_cast<UINT>(std::lround(srcH * scale)));

                // Cap decoded pixel buffer for 32-bit windhawk.exe tool process safety (<= 8192px edge, <= 32M pixels)
                if (dstW <= 8192u && dstH <= 8192u &&
                    (static_cast<uint64_t>(dstW) * static_cast<uint64_t>(dstH)) <= 33554432ULL) {
                    IWICBitmapScaler* scaler = nullptr;
                    if (SUCCEEDED(factory->CreateBitmapScaler(&scaler)) && scaler) {
                        if (SUCCEEDED(scaler->Initialize(frame, dstW, dstH, WICBitmapInterpolationModeFant))) {
                            IWICFormatConverter* conv = nullptr;
                            if (SUCCEEDED(factory->CreateFormatConverter(&conv)) && conv) {
                                if (SUCCEEDED(conv->Initialize(scaler, GUID_WICPixelFormat32bppPBGRA,
                                                               WICBitmapDitherTypeNone, nullptr, 0.0,
                                                               WICBitmapPaletteTypeCustom))) {
                                    try {
                                        auto img = std::make_shared<DecodedBgraImage>();
                                        img->width = dstW;
                                        img->height = dstH;
                                        img->pixels.resize(static_cast<size_t>(dstW) * dstH);
                                        if (SUCCEEDED(conv->CopyPixels(nullptr, dstW * 4,
                                                                       static_cast<UINT>(img->pixels.size() * 4),
                                                                       reinterpret_cast<BYTE*>(img->pixels.data())))) {
                                            result = std::move(img);
                                        }
                                    } catch (const std::bad_alloc&) {
                                        result.reset();
                                    }
                                }
                                conv->Release();
                            }
                        }
                        scaler->Release();
                    }
                }
            }
            frame->Release();
        }
        decoder->Release();
    }
    factory->Release();
    return result;
}

static bool EncodeBgraBufferToPngFile(const wchar_t* outPath, UINT w, UINT h, const BYTE* bgraPixels, bool hasAlpha) {
    if (!outPath || !outPath[0] || w == 0 || h == 0 || !bgraPixels) return false;
    IWICImagingFactory* factory = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&factory))) || !factory) {
        return false;
    }

    bool ok = false;
    IWICStream* stream = nullptr;
    if (SUCCEEDED(factory->CreateStream(&stream)) && stream) {
        if (SUCCEEDED(stream->InitializeFromFilename(outPath, GENERIC_WRITE))) {
            IWICBitmapEncoder* encoder = nullptr;
            if (SUCCEEDED(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder)) && encoder) {
                if (SUCCEEDED(encoder->Initialize(stream, WICBitmapEncoderNoCache))) {
                    IWICBitmapFrameEncode* frame = nullptr;
                    if (SUCCEEDED(encoder->CreateNewFrame(&frame, nullptr)) && frame) {
                        if (SUCCEEDED(frame->Initialize(nullptr)) &&
                            SUCCEEDED(frame->SetSize(w, h))) {
                            WICPixelFormatGUID fmt = hasAlpha ? GUID_WICPixelFormat32bppBGRA : GUID_WICPixelFormat32bppBGR;
                            if (SUCCEEDED(frame->SetPixelFormat(&fmt)) &&
                                SUCCEEDED(frame->WritePixels(h, w * 4, w * h * 4, const_cast<BYTE*>(bgraPixels))) &&
                                SUCCEEDED(frame->Commit()) &&
                                SUCCEEDED(encoder->Commit())) {
                                ok = true;
                            }
                        }
                        frame->Release();
                    }
                }
                encoder->Release();
            }
        }
        stream->Release();
    }
    factory->Release();
    return ok;
}

static bool SaveDibMemoryToPngFile(const BYTE* dibBytes, SIZE_T dibSize, const wchar_t* outPath,
                                   UINT& outW, UINT& outH) {
    outW = outH = 0;
    if (!dibBytes || dibSize < sizeof(BITMAPINFOHEADER) || !outPath) return false;
    const auto* bih = reinterpret_cast<const BITMAPINFOHEADER*>(dibBytes);
    if (bih->biSize < sizeof(BITMAPINFOHEADER) || bih->biWidth <= 0 || bih->biHeight == 0) return false;
    if (bih->biWidth > 8192 || std::abs(bih->biHeight) > 8192 || (static_cast<uint64_t>(bih->biWidth) * static_cast<uint64_t>(std::abs(bih->biHeight))) > 33554432ULL) return false;

    UINT w = static_cast<UINT>(bih->biWidth);
    UINT h = static_cast<UINT>(std::abs(bih->biHeight));
    bool topDown = (bih->biHeight < 0);

    if (bih->biBitCount == 32 && (bih->biCompression == BI_RGB || bih->biCompression == BI_BITFIELDS)) {
        SIZE_T headerBytes = bih->biSize;
        if (bih->biCompression == BI_BITFIELDS && bih->biSize == sizeof(BITMAPINFOHEADER)) {
            headerBytes += 12;
        }
        SIZE_T rowBytes = static_cast<SIZE_T>(w) * 4;
        if (headerBytes + rowBytes * h <= dibSize) {
            std::vector<BYTE> pixels(rowBytes * h);
            bool anyNonZeroAlpha = false;
            const BYTE* srcBase = dibBytes + headerBytes;
            for (UINT y = 0; y < h; ++y) {
                UINT srcY = topDown ? y : (h - 1 - y);
                const BYTE* sRow = srcBase + static_cast<SIZE_T>(srcY) * rowBytes;
                BYTE* dRow = pixels.data() + static_cast<SIZE_T>(y) * rowBytes;
                std::memcpy(dRow, sRow, rowBytes);
                for (UINT x = 0; x < w; ++x) {
                    if (dRow[x * 4 + 3] != 0) anyNonZeroAlpha = true;
                }
            }
            if (!anyNonZeroAlpha) {
                for (size_t i = 3; i < pixels.size(); i += 4) pixels[i] = 255;
            }
            if (EncodeBgraBufferToPngFile(outPath, w, h, pixels.data(), anyNonZeroAlpha)) {
                outW = w;
                outH = h;
                return true;
            }
        }
    }

    HDC hScr = GetDC(nullptr);
    HDC hMem = CreateCompatibleDC(hScr);
    ReleaseDC(nullptr, hScr);
    if (!hMem) return false;

    BITMAPINFO dstBmi{};
    dstBmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    dstBmi.bmiHeader.biWidth       = static_cast<LONG>(w);
    dstBmi.bmiHeader.biHeight      = -static_cast<LONG>(h);
    dstBmi.bmiHeader.biPlanes      = 1;
    dstBmi.bmiHeader.biBitCount    = 32;
    dstBmi.bmiHeader.biCompression = BI_RGB;

    void* dstBits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hMem, &dstBmi, DIB_RGB_COLORS, &dstBits, nullptr, 0);
    bool ok = false;
    if (hBmp && dstBits) {
        HGDIOBJ old = SelectObject(hMem, hBmp);
        SIZE_T colorTblBytes = 0;
        if (bih->biBitCount <= 8) {
            DWORD clrUsed = bih->biClrUsed ? bih->biClrUsed : (1u << bih->biBitCount);
            colorTblBytes = static_cast<SIZE_T>(clrUsed) * sizeof(RGBQUAD);
        } else if (bih->biCompression == BI_BITFIELDS && bih->biSize == sizeof(BITMAPINFOHEADER)) {
            colorTblBytes = 12;
        }
        const BYTE* pBits = dibBytes + bih->biSize + colorTblBytes;
        if (pBits < dibBytes + dibSize) {
            int drawn = StretchDIBits(hMem, 0, 0, static_cast<int>(w), static_cast<int>(h),
                                      0, 0, static_cast<int>(w), static_cast<int>(h),
                                      pBits, reinterpret_cast<const BITMAPINFO*>(bih), DIB_RGB_COLORS, SRCCOPY);
            GdiFlush();
            if (drawn > 0) {
                BYTE* bPtr = static_cast<BYTE*>(dstBits);
                for (size_t i = 3; i < static_cast<size_t>(w) * h * 4; i += 4) bPtr[i] = 255;
                if (EncodeBgraBufferToPngFile(outPath, w, h, bPtr, false)) {
                    outW = w;
                    outH = h;
                    ok = true;
                }
            }
        }
        SelectObject(hMem, old);
        DeleteObject(hBmp);
    }
    DeleteDC(hMem);
    return ok;
}

static HGLOBAL LoadFileBytesToHGlobal(const wchar_t* filePath) {
    if (!filePath || !filePath[0]) return nullptr;
    HANDLE hFile = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return nullptr;
    LARGE_INTEGER sz{};
    HGLOBAL hMem = nullptr;
    if (GetFileSizeEx(hFile, &sz) && sz.QuadPart > 0 && sz.QuadPart <= 32LL * 1024 * 1024) {
        hMem = GlobalAlloc(GHND | GMEM_SHARE, static_cast<SIZE_T>(sz.QuadPart));
        if (hMem) {
            void* ptr = GlobalLock(hMem);
            DWORD readBytes = 0;
            if (!ptr || !ReadFile(hFile, ptr, static_cast<DWORD>(sz.QuadPart), &readBytes, nullptr) ||
                readBytes != static_cast<DWORD>(sz.QuadPart)) {
                if (ptr) GlobalUnlock(hMem);
                GlobalFree(hMem);
                hMem = nullptr;
            } else {
                GlobalUnlock(hMem);
            }
        }
    }
    CloseHandle(hFile);
    return hMem;
}

static HGLOBAL BuildDibHGlobalFromImageFile(const wchar_t* filePath) {
    auto img = DecodeFileThumbnailWic(filePath, 0);
    if (!img || img->width == 0 || img->height == 0 || img->pixels.empty()) return nullptr;
    if (img->width > 8192u || img->height > 8192u ||
        (static_cast<uint64_t>(img->width) * static_cast<uint64_t>(img->height)) > 33554432ULL) {
        return nullptr;
    }
    SIZE_T pixelBytes = static_cast<SIZE_T>(img->width) * img->height * 4;
    SIZE_T totalBytes = sizeof(BITMAPINFOHEADER) + pixelBytes;
    HGLOBAL hMem = GlobalAlloc(GHND | GMEM_SHARE, totalBytes);
    if (!hMem) return nullptr;
    BYTE* dst = static_cast<BYTE*>(GlobalLock(hMem));
    if (!dst) { GlobalFree(hMem); return nullptr; }

    auto* bih = reinterpret_cast<BITMAPINFOHEADER*>(dst);
    bih->biSize        = sizeof(BITMAPINFOHEADER);
    bih->biWidth       = static_cast<LONG>(img->width);
    bih->biHeight      = static_cast<LONG>(img->height);
    bih->biPlanes      = 1;
    bih->biBitCount    = 32;
    bih->biCompression = BI_RGB;
    bih->biSizeImage   = static_cast<DWORD>(pixelBytes);

    BYTE* dstPixels = dst + sizeof(BITMAPINFOHEADER);
    SIZE_T rowBytes = static_cast<SIZE_T>(img->width) * 4;
    for (UINT y = 0; y < img->height; ++y) {
        const BYTE* sRow = reinterpret_cast<const BYTE*>(&img->pixels[static_cast<size_t>(img->height - 1 - y) * img->width]);
        BYTE* dRow = dstPixels + static_cast<SIZE_T>(y) * rowBytes;
        for (UINT x = 0; x < img->width; ++x) {
            BYTE b = sRow[x * 4 + 0];
            BYTE g = sRow[x * 4 + 1];
            BYTE r = sRow[x * 4 + 2];
            BYTE a = sRow[x * 4 + 3];
            if (a > 0 && a < 255) {
                dRow[x * 4 + 0] = static_cast<BYTE>(std::min<int>(255, (b * 255) / a));
                dRow[x * 4 + 1] = static_cast<BYTE>(std::min<int>(255, (g * 255) / a));
                dRow[x * 4 + 2] = static_cast<BYTE>(std::min<int>(255, (r * 255) / a));
            } else {
                dRow[x * 4 + 0] = b;
                dRow[x * 4 + 1] = g;
                dRow[x * 4 + 2] = r;
            }
            dRow[x * 4 + 3] = a;
        }
    }
    GlobalUnlock(hMem);
    return hMem;
}

static std::shared_ptr<DecodedBgraImage> ExtractShellIconImage(const wchar_t* filePath) {
    if (!filePath || !filePath[0]) return nullptr;
    SHFILEINFOW sfi{};
    if (!SHGetFileInfoW(filePath, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON) || !sfi.hIcon) {
        return nullptr;
    }

    std::shared_ptr<DecodedBgraImage> result;
    IWICImagingFactory* factory = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&factory))) && factory) {
        IWICBitmap* wicBmp = nullptr;
        if (SUCCEEDED(factory->CreateBitmapFromHICON(sfi.hIcon, &wicBmp)) && wicBmp) {
            UINT w = 0, h = 0;
            if (SUCCEEDED(wicBmp->GetSize(&w, &h)) && w > 0 && h > 0) {
                IWICFormatConverter* conv = nullptr;
                if (SUCCEEDED(factory->CreateFormatConverter(&conv)) && conv) {
                    if (SUCCEEDED(conv->Initialize(wicBmp, GUID_WICPixelFormat32bppPBGRA,
                                                   WICBitmapDitherTypeNone, nullptr, 0.0,
                                                   WICBitmapPaletteTypeCustom))) {
                        auto img = std::make_shared<DecodedBgraImage>();
                        img->width = w;
                        img->height = h;
                        img->pixels.resize(static_cast<size_t>(w) * h);
                        if (SUCCEEDED(conv->CopyPixels(nullptr, w * 4,
                                                       static_cast<UINT>(img->pixels.size() * 4),
                                                       reinterpret_cast<BYTE*>(img->pixels.data())))) {
                            result = std::move(img);
                        }
                    }
                    conv->Release();
                }
            }
            wicBmp->Release();
        }
        factory->Release();
    }
    DestroyIcon(sfi.hIcon);
    return result;
}

static bool DecodeWallpaperToMonitorBuffer(const wchar_t* filePath, int monW, int monH, std::vector<DWORD>& outPixels) {
    if (!filePath || !filePath[0] || monW <= 16 || monH <= 16) return false;
    IWICImagingFactory* factory = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&factory))) || !factory) {
        return false;
    }

    bool ok = false;
    IWICBitmapDecoder* decoder = nullptr;
    if (SUCCEEDED(factory->CreateDecoderFromFilename(filePath, nullptr, GENERIC_READ,
                                                     WICDecodeMetadataCacheOnDemand, &decoder)) && decoder) {
        IWICBitmapFrameDecode* frame = nullptr;
        if (SUCCEEDED(decoder->GetFrame(0, &frame)) && frame) {
            UINT srcW = 0, srcH = 0;
            if (SUCCEEDED(frame->GetSize(&srcW, &srcH)) && srcW > 0 && srcH > 0) {
                double scale = std::max(static_cast<double>(monW) / srcW, static_cast<double>(monH) / srcH);
                UINT scaledW = std::max<UINT>(static_cast<UINT>(monW), static_cast<UINT>(std::ceil(srcW * scale)));
                UINT scaledH = std::max<UINT>(static_cast<UINT>(monH), static_cast<UINT>(std::ceil(srcH * scale)));

                IWICBitmapScaler* scaler = nullptr;
                if (SUCCEEDED(factory->CreateBitmapScaler(&scaler)) && scaler) {
                    if (SUCCEEDED(scaler->Initialize(frame, scaledW, scaledH, WICBitmapInterpolationModeLinear))) {
                        IWICFormatConverter* conv = nullptr;
                        if (SUCCEEDED(factory->CreateFormatConverter(&conv)) && conv) {
                            if (SUCCEEDED(conv->Initialize(scaler, GUID_WICPixelFormat32bppBGR,
                                                           WICBitmapDitherTypeNone, nullptr, 0.0,
                                                           WICBitmapPaletteTypeCustom))) {
                                WICRect crop = {
                                    static_cast<INT>((scaledW - static_cast<UINT>(monW)) / 2),
                                    static_cast<INT>((scaledH - static_cast<UINT>(monH)) / 2),
                                    monW, monH
                                };
                                outPixels.resize(static_cast<size_t>(monW) * monH);
                                if (SUCCEEDED(conv->CopyPixels(&crop, static_cast<UINT>(monW * 4),
                                                               static_cast<UINT>(outPixels.size() * 4),
                                                               reinterpret_cast<BYTE*>(outPixels.data())))) {
                                    ok = true;
                                }
                            }
                            conv->Release();
                        }
                    }
                    scaler->Release();
                }
            }
            frame->Release();
        }
        decoder->Release();
    }
    factory->Release();
    return ok;
}

// ---------------------------------------------------------------------------
// 4. Staged Items & Clipboard History Model (Files, Images, Text, URLs)
// ---------------------------------------------------------------------------
enum class StagedItemKind {
    File,
    Folder,
    ImageBitmap,
    AudioFile,
    TextSnippet,
    UrlLink
};

struct StagedItem {
    uint64_t       id = 0;
    StagedItemKind kind = StagedItemKind::File;
    std::wstring   pathOrText;
    std::wstring   displayName;
    std::wstring   metaText;
    uint64_t       sizeBytes = 0;
    bool           isImageThumb = false;
    bool           isTempFile = false;
    std::shared_ptr<DecodedBgraImage> thumb;
};

static uint64_t                g_nextItemId = 1;
static std::vector<StagedItem> g_stagedItems;
static std::vector<StagedItem> g_clipHistory; // For Win+V replacement tab
static ActiveTab               g_activeTab = ActiveTab::Shelf;
static CategoryFilter          g_activeFilter = CategoryFilter::All;
static uint64_t                g_playingAudioItemId = 0;
static bool                    g_shelfPinned = false;
static bool                    g_shelfMinimized = false;
static HWND                    g_prevForegroundWnd = nullptr;
static ULONGLONG               g_ignoreClipUntilTick = 0;


static bool ItemMatchesFilter(const StagedItem& it, CategoryFilter f) {
    switch (f) {
        case CategoryFilter::Text:
            return it.kind == StagedItemKind::TextSnippet || it.kind == StagedItemKind::UrlLink;
        case CategoryFilter::Image:
            return it.kind == StagedItemKind::ImageBitmap;
        case CategoryFilter::File:
            return it.kind == StagedItemKind::File || it.kind == StagedItemKind::Folder;
        case CategoryFilter::Audio:
            return it.kind == StagedItemKind::AudioFile;
        default:
            return true;
    }
}

static std::vector<int> BuildFilteredIndices(const std::vector<StagedItem>& list, CategoryFilter f) {
    std::vector<int> out;
    out.reserve(list.size());
    for (int i = 0; i < static_cast<int>(list.size()); ++i) {
        if (ItemMatchesFilter(list[i], f)) out.push_back(i);
    }
    return out;
}

static void StopInlineAudioPlayback() {
    if (g_playingAudioItemId != 0) {
        mciSendStringW(L"stop wh_dnd_audio", nullptr, 0, nullptr);
        mciSendStringW(L"close wh_dnd_audio", nullptr, 0, nullptr);
        g_playingAudioItemId = 0;
    }
}

static void RenderShelfFrame(bool rebuildGlass = false);

static void ToggleInlineAudioPlayback(const StagedItem& item) {
    if (item.kind != StagedItemKind::AudioFile || item.pathOrText.empty()) return;
    if (g_playingAudioItemId == item.id) {
        StopInlineAudioPlayback();
        RenderShelfFrame(false);
        return;
    }
    StopInlineAudioPlayback();
    std::wstring cmdOpen = L"open \"" + item.pathOrText + L"\" alias wh_dnd_audio";
    if (mciSendStringW(cmdOpen.c_str(), nullptr, 0, nullptr) == 0) {
        if (mciSendStringW(L"play wh_dnd_audio", nullptr, 0, nullptr) == 0) {
            g_playingAudioItemId = item.id;
            RenderShelfFrame(false);
            return;
        }
        mciSendStringW(L"close wh_dnd_audio", nullptr, 0, nullptr);
    }
    // Fallback for codecs MCI doesn't decode natively (e.g. FLAC/OGG)
    ShellExecuteW(nullptr, L"open", item.pathOrText.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

static std::vector<StagedItem>& GetActiveList() {
    return (g_activeTab == ActiveTab::ClipboardHistory && g_settings.replaceWinV)
        ? g_clipHistory : g_stagedItems;
}

static void EvictThumbGpu(uint64_t itemId);
static void CleanupTempFileIfOwned(const StagedItem& item) {
    EvictThumbGpu(item.id);
    if (item.isTempFile && !item.pathOrText.empty()) {
        DeleteFileW(item.pathOrText.c_str());
    }
}

static void ClearVectorItems(std::vector<StagedItem>& vec) {
    for (const auto& it : vec) CleanupTempFileIfOwned(it);
    vec.clear();
}

static bool LooksLikeUrl(const std::wstring& s) {
    std::wstring low = ToLowerStr(s);
    return (low.rfind(L"http://", 0) == 0 ||
            low.rfind(L"https://", 0) == 0 ||
            low.rfind(L"www.", 0) == 0);
}

static std::wstring MakeCleanSingleLine(const std::wstring& raw, size_t maxChars) {
    std::wstring out;
    out.reserve(maxChars + 4);
    bool prevSpace = true;
    for (wchar_t ch : raw) {
        if (ch == L'\r' || ch == L'\n' || ch == L'\t' || iswspace(ch)) {
            if (!prevSpace) {
                out.push_back(L' ');
                prevSpace = true;
            }
        } else if (ch >= 0x20) {
            out.push_back(ch);
            prevSpace = false;
        }
        if (out.size() >= maxChars) break;
    }
    while (!out.empty() && out.back() == L' ') out.pop_back();
    if (raw.size() > out.size() && !out.empty()) out += L"…";
    return out;
}

static bool BuildFileStagedItem(const std::wstring& filePath, bool isTempOwned, bool forceImageKind, StagedItem& outItem) {
    if (filePath.empty()) return false;
    WIN32_FILE_ATTRIBUTE_DATA fad{};
    if (!GetFileAttributesExW(filePath.c_str(), GetFileExInfoStandard, &fad)) {
        return false;
    }

    outItem = {};
    outItem.id = g_nextItemId++;
    outItem.pathOrText = filePath;
    outItem.isTempFile = isTempOwned;

    const wchar_t* base = PathFindFileNameW(filePath.c_str());
    outItem.displayName = (base && base[0]) ? std::wstring(base) : filePath;

    bool isDir = (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    bool vi = ComputeUseVietnamese(g_settings.lang);

    if (isDir) {
        outItem.kind = StagedItemKind::Folder;
        outItem.sizeBytes = 0;
        outItem.metaText = PickText(vi, L"Thư mục", L"Folder");
    } else {
        bool isImgExt   = IsSupportedThumbnailExtension(filePath.c_str());
        bool isAudioExt = !forceImageKind && IsSupportedAudioExtension(filePath.c_str());
        if (forceImageKind || isImgExt) outItem.kind = StagedItemKind::ImageBitmap;
        else if (isAudioExt)            outItem.kind = StagedItemKind::AudioFile;
        else                            outItem.kind = StagedItemKind::File;
        outItem.sizeBytes = (static_cast<uint64_t>(fad.nFileSizeHigh) << 32) | fad.nFileSizeLow;

        UINT origW = 0, origH = 0;
        if (g_settings.showImageThumbnails && isImgExt) {
            outItem.thumb = DecodeFileThumbnailWic(filePath.c_str(), 72, &origW, &origH);
            outItem.isImageThumb = (outItem.thumb != nullptr);
        }

        const wchar_t* ext = PathFindExtensionW(outItem.displayName.c_str());
        std::wstring extLabel;
        if (ext && ext[0] == L'.' && ext[1]) {
            extLabel = ext + 1;
            for (wchar_t& c : extLabel) c = static_cast<wchar_t>(std::towupper(c));
        }

        if (origW > 0 && origH > 0) {
            outItem.metaText = std::to_wstring(origW) + L"×" + std::to_wstring(origH) + L" · " +
                               FormatFileSizeCompact(outItem.sizeBytes);
        } else {
            outItem.metaText = FormatFileSizeCompact(outItem.sizeBytes);
            if (!extLabel.empty()) outItem.metaText += L" · " + extLabel;
        }
    }

    if (g_settings.showImageThumbnails && !outItem.thumb) {
        outItem.thumb = ExtractShellIconImage(filePath.c_str());
        outItem.isImageThumb = false;
    }
    return true;
}

static bool AddItemToVector(std::vector<StagedItem>& target, StagedItem item, int maxCap, bool insertFront) {
    // Deduplicate
    for (auto it = target.begin(); it != target.end(); ++it) {
        bool samePath = (it->kind == item.kind &&
                         (it->kind == StagedItemKind::File || it->kind == StagedItemKind::Folder ||
                          it->kind == StagedItemKind::AudioFile) &&
                         _wcsicmp(it->pathOrText.c_str(), item.pathOrText.c_str()) == 0);
        bool sameText = (it->kind == item.kind &&
                         (it->kind == StagedItemKind::TextSnippet || it->kind == StagedItemKind::UrlLink) &&
                         it->pathOrText == item.pathOrText);
        if (samePath || sameText) {
            if (insertFront) {
                StagedItem moved = std::move(*it);
                target.erase(it);
                target.insert(target.begin(), std::move(moved));
                CleanupTempFileIfOwned(item);
                return true;
            }
            CleanupTempFileIfOwned(item);
            return false;
        }
    }

    if (insertFront) {
        target.insert(target.begin(), std::move(item));
        while (static_cast<int>(target.size()) > maxCap) {
            CleanupTempFileIfOwned(target.back());
            target.pop_back();
        }
    } else {
        if (static_cast<int>(target.size()) >= maxCap) {
            CleanupTempFileIfOwned(item);
            return false;
        }
        target.push_back(std::move(item));
    }
    return true;
}

static bool AddFileToList(std::vector<StagedItem>& list, const std::wstring& filePath,
                          bool isTempOwned, bool forceImageKind, int maxCap, bool insertFront) {
    StagedItem item;
    if (!BuildFileStagedItem(filePath, isTempOwned, forceImageKind, item)) return false;
    return AddItemToVector(list, std::move(item), maxCap, insertFront);
}

static bool AddRawPngBytesToList(std::vector<StagedItem>& list, const BYTE* pngData, SIZE_T pngSize,
                                 const std::wstring& suggestedName, int maxCap, bool insertFront) {
    if (!g_settings.acceptImageDrops || !pngData || pngSize < 16 || pngSize > 32 * 1024 * 1024) return false;
    std::wstring dir = GetTempStagingDir();
    if (dir.empty()) return false;

    std::wstring fname = suggestedName.empty()
        ? (L"image_" + std::to_wstring(GetTickCount64()) + L"_" + std::to_wstring(g_nextItemId) + L".png")
        : SanitizeFileName(suggestedName);
    std::wstring fullPath = dir + L"\\" + fname;

    HANDLE hFile = CreateFileW(fullPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    BOOL ok = WriteFile(hFile, pngData, static_cast<DWORD>(pngSize), &written, nullptr);
    CloseHandle(hFile);
    if (!ok || written != static_cast<DWORD>(pngSize)) {
        DeleteFileW(fullPath.c_str());
        return false;
    }
    if (!AddFileToList(list, fullPath, true, true, maxCap, insertFront)) {
        DeleteFileW(fullPath.c_str());
        return false;
    }
    return true;
}

static bool AddRawDibToList(std::vector<StagedItem>& list, const BYTE* dibBytes, SIZE_T dibSize,
                            int maxCap, bool insertFront) {
    if (!g_settings.acceptImageDrops || !dibBytes || dibSize < sizeof(BITMAPINFOHEADER)) return false;
    std::wstring dir = GetTempStagingDir();
    if (dir.empty()) return false;

    std::wstring fname = L"image_" + std::to_wstring(GetTickCount64()) + L"_" + std::to_wstring(g_nextItemId) + L".png";
    std::wstring fullPath = dir + L"\\" + fname;
    UINT w = 0, h = 0;
    if (!SaveDibMemoryToPngFile(dibBytes, dibSize, fullPath.c_str(), w, h)) {
        DeleteFileW(fullPath.c_str());
        return false;
    }
    if (!AddFileToList(list, fullPath, true, true, maxCap, insertFront)) {
        DeleteFileW(fullPath.c_str());
        return false;
    }
    return true;
}

static bool AddTextToList(std::vector<StagedItem>& list, const std::wstring& rawText,
                          int maxCap, bool insertFront) {
    if (!g_settings.acceptTextDrops) return false;
    std::wstring trimmed = rawText;
    while (!trimmed.empty() && iswspace(trimmed.front())) trimmed.erase(trimmed.begin());
    while (!trimmed.empty() && iswspace(trimmed.back())) trimmed.pop_back();
    if (trimmed.empty()) return false;

    bool vi = ComputeUseVietnamese(g_settings.lang);
    StagedItem item;
    item.id = g_nextItemId++;
    item.pathOrText = trimmed;
    item.kind = LooksLikeUrl(trimmed) ? StagedItemKind::UrlLink : StagedItemKind::TextSnippet;
    item.displayName = MakeCleanSingleLine(trimmed, 48);
    item.sizeBytes = trimmed.size() * sizeof(wchar_t);
    if (item.kind == StagedItemKind::UrlLink) {
        item.metaText = PickText(vi, L"Liên kết URL", L"Web Link");
    } else {
        item.metaText = std::to_wstring(trimmed.size()) + PickText(vi, L" ký tự", L" chars");
    }
    return AddItemToVector(list, std::move(item), maxCap, insertFront);
}

static std::vector<std::wstring> CollectStagedFilePaths(const std::vector<StagedItem>& items) {
    std::vector<std::wstring> files;
    for (const auto& it : items) {
        if (it.kind == StagedItemKind::File || it.kind == StagedItemKind::Folder ||
            it.kind == StagedItemKind::ImageBitmap || it.kind == StagedItemKind::AudioFile) {
            files.push_back(it.pathOrText);
        }
    }
    return files;
}

static std::wstring CollectPrimaryImagePath(const std::vector<StagedItem>& items) {
    for (const auto& it : items) {
        if (it.kind == StagedItemKind::ImageBitmap && !it.pathOrText.empty()) {
            return it.pathOrText;
        }
    }
    return {};
}

static std::wstring CollectStagedTextJoined(const std::vector<StagedItem>& items, bool& outIsSingleUrl) {
    outIsSingleUrl = false;
    std::wstring joined;
    int txtCount = 0;
    bool allUrl = true;
    for (const auto& it : items) {
        if (it.kind == StagedItemKind::TextSnippet || it.kind == StagedItemKind::UrlLink) {
            if (!joined.empty()) joined += L"\r\n";
            joined += it.pathOrText;
            ++txtCount;
            if (it.kind != StagedItemKind::UrlLink) allUrl = false;
        }
    }
    outIsSingleUrl = (txtCount == 1 && allUrl);
    return joined;
}

// ---------------------------------------------------------------------------
// 5. Frosted Glass Optical Engine (Wallpaper + Underlying Windows, Clean PARGB)
// ---------------------------------------------------------------------------
struct DibCanvas {
    HDC     hdcMem = nullptr;
    HBITMAP hbm    = nullptr;
    HGDIOBJ oldBmp = nullptr;
    void*   bits   = nullptr;
    int     capW   = 0;
    int     capH   = 0;

    bool Ensure(int w, int h) {
        if (w <= capW && h <= capH && hdcMem && hbm) return true;
        int nw = std::max<int>({ w, capW, 64 });
        int nh = std::max<int>({ h, capH, 64 });
        Release();

        HDC hdcScr = GetDC(nullptr);
        hdcMem = CreateCompatibleDC(hdcScr);
        ReleaseDC(nullptr, hdcScr);
        if (!hdcMem) return false;

        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth       = nw;
        bmi.bmiHeader.biHeight      = -nh;
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        hbm = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!hbm) {
            DeleteDC(hdcMem);
            hdcMem = nullptr;
            return false;
        }
        oldBmp = SelectObject(hdcMem, hbm);
        capW = nw;
        capH = nh;
        return true;
    }

    void Release() {
        if (hdcMem) {
            if (oldBmp) { SelectObject(hdcMem, oldBmp); oldBmp = nullptr; }
            if (hbm)    { DeleteObject(hbm); hbm = nullptr; }
            DeleteDC(hdcMem);
            hdcMem = nullptr;
        }
        bits = nullptr;
        capW = 0;
        capH = 0;
    }
};

static void BlurBufferSeparableRGB(std::vector<DWORD>& buf, std::vector<DWORD>& tmp, int w, int h, int radius) {
    if (radius <= 0 || w <= 2 || h <= 2) return;
    radius = std::clamp<int>(radius, 1, std::min(w - 1, h - 1) / 2);
    if (radius <= 0) return;
    const uint32_t diam    = static_cast<uint32_t>(radius * 2 + 1);
    const uint32_t invDiam = (1u << 24) / diam + 1u;

    for (int pass = 0; pass < 3; ++pass) {
        for (int y = 0; y < h; ++y) {
            const DWORD* rowIn  = &buf[static_cast<size_t>(y) * w];
            DWORD*       rowOut = &tmp[static_cast<size_t>(y) * w];
            uint32_t sumR = 0, sumG = 0, sumB = 0;
            for (int k = -radius; k <= radius; ++k) {
                DWORD p = rowIn[std::clamp<int>(k, 0, w - 1)];
                sumR += (p >> 16) & 0xFF;
                sumG += (p >> 8) & 0xFF;
                sumB += p & 0xFF;
            }
            for (int x = 0; x < w; ++x) {
                rowOut[x] = (((sumR * invDiam) >> 24) << 16) |
                            (((sumG * invDiam) >> 24) << 8)  |
                             ((sumB * invDiam) >> 24);
                DWORD pOut = rowIn[std::clamp<int>(x - radius, 0, w - 1)];
                DWORD pIn  = rowIn[std::clamp<int>(x + radius + 1, 0, w - 1)];
                sumR += ((pIn >> 16) & 0xFF) - ((pOut >> 16) & 0xFF);
                sumG += ((pIn >> 8) & 0xFF)  - ((pOut >> 8) & 0xFF);
                sumB += (pIn & 0xFF)         - (pOut & 0xFF);
            }
        }
        for (int x = 0; x < w; ++x) {
            uint32_t sumR = 0, sumG = 0, sumB = 0;
            for (int k = -radius; k <= radius; ++k) {
                DWORD p = tmp[static_cast<size_t>(std::clamp<int>(k, 0, h - 1)) * w + x];
                sumR += (p >> 16) & 0xFF;
                sumG += (p >> 8) & 0xFF;
                sumB += p & 0xFF;
            }
            for (int y = 0; y < h; ++y) {
                buf[static_cast<size_t>(y) * w + x] =
                    (((sumR * invDiam) >> 24) << 16) |
                    (((sumG * invDiam) >> 24) << 8)  |
                     ((sumB * invDiam) >> 24);
                DWORD pOut = tmp[static_cast<size_t>(std::clamp<int>(y - radius, 0, h - 1)) * w + x];
                DWORD pIn  = tmp[static_cast<size_t>(std::clamp<int>(y + radius + 1, 0, h - 1)) * w + x];
                sumR += ((pIn >> 16) & 0xFF) - ((pOut >> 16) & 0xFF);
                sumG += ((pIn >> 8) & 0xFF)  - ((pOut >> 8) & 0xFF);
                sumB += (pIn & 0xFF)         - (pOut & 0xFF);
            }
        }
    }
}

struct CachedWallpaper {
    RECT rcMonitor{};
    uint64_t lastWriteFt = 0;
    ULONGLONG lastStatTick = 0;
    std::wstring pathUsed;
    std::vector<DWORD> pixels;
    bool valid = false;
};
static CachedWallpaper g_wallpaperCache;
static DibCanvas       g_printWndCanvas;

static void EnsureMonitorWallpaperLoaded(const RECT& rcMonitor) {
    int monW = static_cast<int>(rcMonitor.right - rcMonitor.left);
    int monH = static_cast<int>(rcMonitor.bottom - rcMonitor.top);
    if (monW <= 32 || monH <= 32) return;

    ULONGLONG now = GetTickCount64();
    bool sameMon = EqualRect(&rcMonitor, &g_wallpaperCache.rcMonitor) != FALSE;
    if (g_wallpaperCache.valid && sameMon && (now - g_wallpaperCache.lastStatTick < 4000)) {
        return;
    }
    g_wallpaperCache.lastStatTick = now;

    wchar_t candidatePath[MAX_PATH] = {};
    wchar_t appData[MAX_PATH] = {};
    if (ExpandEnvironmentStringsW(L"%APPDATA%\\Microsoft\\Windows\\Themes\\TranscodedWallpaper", appData, MAX_PATH) > 0 &&
        GetFileAttributesW(appData) != INVALID_FILE_ATTRIBUTES) {
        wcscpy_s(candidatePath, appData);
    } else {
        SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, candidatePath, 0);
    }

    uint64_t writeFt = 0;
    WIN32_FILE_ATTRIBUTE_DATA fad{};
    if (candidatePath[0] && GetFileAttributesExW(candidatePath, GetFileExInfoStandard, &fad)) {
        writeFt = (static_cast<uint64_t>(fad.ftLastWriteTime.dwHighDateTime) << 32) | fad.ftLastWriteTime.dwLowDateTime;
    }

    if (g_wallpaperCache.valid && sameMon && writeFt == g_wallpaperCache.lastWriteFt &&
        candidatePath == g_wallpaperCache.pathUsed) {
        return;
    }

    g_wallpaperCache.rcMonitor = rcMonitor;
    g_wallpaperCache.lastWriteFt = writeFt;
    g_wallpaperCache.pathUsed = candidatePath;

    if (!candidatePath[0] || !DecodeWallpaperToMonitorBuffer(candidatePath, monW, monH, g_wallpaperCache.pixels)) {
        COLORREF deskCol = GetSysColor(COLOR_DESKTOP);
        DWORD bgPx = (static_cast<DWORD>(GetRValue(deskCol)) << 16) |
                     (static_cast<DWORD>(GetGValue(deskCol)) << 8)  |
                      static_cast<DWORD>(GetBValue(deskCol));
        g_wallpaperCache.pixels.assign(static_cast<size_t>(monW) * monH, bgPx);
    }
    g_wallpaperCache.valid = true;
}

static bool IsRealBackdropWindow(HWND w) {
    if (!w || w == g_shelfHwnd || w == g_pillHwnd || w == g_alertHwnd || w == g_previewHwnd) return false;
    if (!IsWindowVisible(w) || IsIconic(w)) return false;
    LONG_PTR ex = GetWindowLongPtrW(w, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) return false;

    int cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked != 0) {
        return false;
    }
    wchar_t cls[64] = {};
    if (!GetClassNameW(w, cls, ARRAYSIZE(cls))) return false;
    if (_wcsicmp(cls, L"Progman") == 0 ||
        _wcsicmp(cls, L"WorkerW") == 0 ||
        _wcsicmp(cls, L"SysDragImage") == 0 ||
        _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        return false;
    }
    return true;
}

struct GlassBackdropCache {
    int w = 0, h = 0;
    bool isLight = false;
    bool valid = false;
    RECT screenRect{};
    std::vector<DWORD> pixels;
    ID2D1Bitmap* pBmp = nullptr;
    ID2D1BitmapBrush* pBrush = nullptr;
    ID2D1RenderTarget* pOwner = nullptr;

    void ReleaseGpu() {
        if (pBrush) { pBrush->Release(); pBrush = nullptr; }
        if (pBmp)   { pBmp->Release();   pBmp = nullptr; }
        pOwner = nullptr;
    }
    void ClearAll() {
        ReleaseGpu();
        pixels.clear();
        valid = false;
    }
};
static GlassBackdropCache g_glassCache;
static GlassBackdropCache g_pillGlassCache;
static GlassBackdropCache g_alertGlassCache;

static bool BuildFrostedGlassForSlot(GlassBackdropCache& cache, const RECT& rcTarget,
                                     float cornerRadiusPx, UINT dpi, const ModSettings& s,
                                     bool fastSkipPrintWindow = false) {
    int tw = static_cast<int>(rcTarget.right - rcTarget.left);
    int th = static_cast<int>(rcTarget.bottom - rcTarget.top);
    if (tw <= 16 || th <= 16) return false;

    if (s.glassStyle == GlassStyle::SolidSoft) {
        cache.ReleaseGpu();
        cache.w = tw;
        cache.h = th;
        cache.screenRect = rcTarget;
        cache.isLight = (s.colorMode == ColorMode::Light) ? true
                      : ((s.colorMode == ColorMode::Dark) ? false : g_systemLightTheme);
        cache.pixels.clear();
        cache.valid = true;
        return true;
    }

    int padPx = std::max<int>(16, DpiScale(20, dpi));
    int capX = static_cast<int>(rcTarget.left) - padPx;
    int capY = static_cast<int>(rcTarget.top)  - padPx;
    int capW = tw + padPx * 2;
    int capH = th + padPx * 2;
    std::vector<DWORD> raw(static_cast<size_t>(capW) * capH, 0x181A22u);

    HMONITOR hMon = MonitorFromRect(&rcTarget, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    if (!hMon || !GetMonitorInfoW(hMon, &mi)) return false;

    EnsureMonitorWallpaperLoaded(mi.rcMonitor);
    int monW = static_cast<int>(mi.rcMonitor.right - mi.rcMonitor.left);
    int monH = static_cast<int>(mi.rcMonitor.bottom - mi.rcMonitor.top);
    if (g_wallpaperCache.valid && static_cast<int>(g_wallpaperCache.pixels.size()) == monW * monH) {
        for (int y = 0; y < capH; ++y) {
            int my = std::clamp<int>((capY + y) - static_cast<int>(mi.rcMonitor.top), 0, monH - 1);
            const DWORD* srcRow = &g_wallpaperCache.pixels[static_cast<size_t>(my) * monW];
            DWORD* dstRow = &raw[static_cast<size_t>(y) * capW];
            for (int x = 0; x < capW; ++x) {
                int mx = std::clamp<int>((capX + x) - static_cast<int>(mi.rcMonitor.left), 0, monW - 1);
                dstRow[x] = srcRow[mx];
            }
        }
    }

    RECT rcCap = { capX, capY, capX + capW, capY + capH };
    std::vector<std::pair<HWND, RECT>> overlaps;
    if (!fastSkipPrintWindow)
    for (HWND w = GetTopWindow(nullptr); w != nullptr; w = GetWindow(w, GW_HWNDNEXT)) {
        if (!IsRealBackdropWindow(w)) continue;
        RECT rcW{};
        if (!GetWindowRect(w, &rcW) || rcW.right <= rcW.left || rcW.bottom <= rcW.top) continue;
        RECT rcInter{};
        if (!IntersectRect(&rcInter, &rcW, &rcTarget)) continue;
        overlaps.push_back({ w, rcW });
        if (rcW.left <= rcCap.left && rcW.top <= rcCap.top &&
            rcW.right >= rcCap.right && rcW.bottom >= rcCap.bottom) {
            break;
        }
        if (overlaps.size() >= 2) break;
    }

    for (int idx = static_cast<int>(overlaps.size()) - 1; idx >= 0; --idx) {
        HWND w = overlaps[idx].first;
        const RECT& rcW = overlaps[idx].second;
        int ww = static_cast<int>(rcW.right - rcW.left);
        int wh = static_cast<int>(rcW.bottom - rcW.top);
        if (ww <= 16 || wh <= 16 || ww > 4096 || wh > 2560) continue;
        if (g_printWndCanvas.Ensure(ww, wh)) {
            if (PrintWindow(w, g_printWndCanvas.hdcMem, PW_RENDERFULLCONTENT)) {
                GdiFlush();
                const DWORD* wBits = static_cast<const DWORD*>(g_printWndCanvas.bits);
                RECT rcInter{};
                if (IntersectRect(&rcInter, &rcW, &rcCap)) {
                    for (int sy = static_cast<int>(rcInter.top); sy < static_cast<int>(rcInter.bottom); ++sy) {
                        const DWORD* sRow = &wBits[static_cast<size_t>(sy - rcW.top) * g_printWndCanvas.capW];
                        DWORD* dRow = &raw[static_cast<size_t>(sy - capY) * capW];
                        for (int sx = static_cast<int>(rcInter.left); sx < static_cast<int>(rcInter.right); ++sx) {
                            DWORD px = sRow[sx - rcW.left] & 0x00FFFFFFu;
                            if (px != 0) dRow[sx - capX] = px;
                        }
                    }
                }
            }
        }
    }

    if (static_cast<int64_t>(g_printWndCanvas.capW) * g_printWndCanvas.capH > 512 * 384) {
        g_printWndCanvas.Release();
    }

    bool isLight = g_systemLightTheme;
    if (s.colorMode == ColorMode::Light) isLight = true;
    else if (s.colorMode == ColorMode::Dark) isLight = false;
    else if (s.uiStyle == UiStyle::MacOs) {
        uint64_t lumaSum = 0;
        uint32_t cnt = 0;
        int stepX = std::max<int>(2, tw / 20);
        int stepY = std::max<int>(2, th / 12);
        for (int sy = padPx + 2; sy < padPx + th - 2; sy += stepY) {
            const DWORD* row = &raw[static_cast<size_t>(sy) * capW];
            for (int sx = padPx + 2; sx < padPx + tw - 2; sx += stepX) {
                DWORD px = row[sx];
                lumaSum += (77u * ((px >> 16) & 0xFF) + 150u * ((px >> 8) & 0xFF) + 29u * (px & 0xFF)) >> 8;
                ++cnt;
            }
        }
        if (cnt > 0) isLight = ((lumaSum / cnt) >= 158);
    }

    int hw = std::max<int>(8, capW / 2);
    int hh = std::max<int>(8, capH / 2);
    std::vector<DWORD> halfBuf(static_cast<size_t>(hw) * hh);
    std::vector<DWORD> blurTmp(static_cast<size_t>(hw) * hh);

    for (int hy = 0; hy < hh; ++hy) {
        int sy0 = std::clamp<int>(hy * 2, 0, capH - 1);
        int sy1 = std::clamp<int>(hy * 2 + 1, 0, capH - 1);
        const DWORD* r0 = &raw[static_cast<size_t>(sy0) * capW];
        const DWORD* r1 = &raw[static_cast<size_t>(sy1) * capW];
        DWORD* dst = &halfBuf[static_cast<size_t>(hy) * hw];
        for (int hx = 0; hx < hw; ++hx) {
            int sx0 = std::clamp<int>(hx * 2, 0, capW - 1);
            int sx1 = std::clamp<int>(hx * 2 + 1, 0, capW - 1);
            DWORD p0 = r0[sx0], p1 = r0[sx1], p2 = r1[sx0], p3 = r1[sx1];
            uint32_t r = (((p0 >> 16) & 0xFF) + ((p1 >> 16) & 0xFF) + ((p2 >> 16) & 0xFF) + ((p3 >> 16) & 0xFF)) >> 2;
            uint32_t g = (((p0 >> 8) & 0xFF)  + ((p1 >> 8) & 0xFF)  + ((p2 >> 8) & 0xFF)  + ((p3 >> 8) & 0xFF))  >> 2;
            uint32_t b = ((p0 & 0xFF)         + (p1 & 0xFF)         + (p2 & 0xFF)         + (p3 & 0xFF))         >> 2;
            dst[hx] = (r << 16) | (g << 8) | b;
        }
    }

    bool isLiquid = (s.glassStyle == GlassStyle::LiquidClear);
    int blurRad = std::clamp<int>(static_cast<int>(std::lround((isLiquid ? 8.0f : 14.0f) * (dpi / 96.0f))), 5, 22);
    BlurBufferSeparableRGB(halfBuf, blurTmp, hw, hh, blurRad);

    float satBoost = isLiquid ? 1.18f : (s.uiStyle == UiStyle::Windows ? 1.05f : 1.12f);
    float tintW    = isLiquid ? 0.24f : (s.uiStyle == UiStyle::Windows ? (isLight ? 0.76f : 0.78f) : (isLight ? 0.62f : 0.66f));
    int satScale   = static_cast<int>(std::lround(satBoost * 256.0f));
    int tintI      = static_cast<int>(std::lround(tintW * 256.0f));
    int invTintI   = 256 - tintI;
    int tintR      = (isLight ? 248 : (s.uiStyle == UiStyle::Windows ? 32 : 24)) * tintI;
    int tintG      = (isLight ? 249 : (s.uiStyle == UiStyle::Windows ? 32 : 26)) * tintI;
    int tintB      = (isLight ? 252 : (s.uiStyle == UiStyle::Windows ? 36 : 34)) * tintI;

    for (DWORD& px : halfBuf) {
        int r = (px >> 16) & 0xFF;
        int g = (px >> 8) & 0xFF;
        int b = px & 0xFF;
        int luma = (77 * r + 150 * g + 29 * b) >> 8;
        r = std::clamp<int>(luma + (((r - luma) * satScale) >> 8), 0, 255);
        g = std::clamp<int>(luma + (((g - luma) * satScale) >> 8), 0, 255);
        b = std::clamp<int>(luma + (((b - luma) * satScale) >> 8), 0, 255);
        r = std::clamp<int>((r * invTintI + tintR) >> 8, 0, 255);
        g = std::clamp<int>((g * invTintI + tintG) >> 8, 0, 255);
        b = std::clamp<int>((b * invTintI + tintB) >> 8, 0, 255);
        px = (static_cast<DWORD>(r) << 16) | (static_cast<DWORD>(g) << 8) | static_cast<DWORD>(b);
    }

    auto sampleBilinear = [&](float fx, float fy) -> DWORD {
        fx = std::clamp(fx, 0.0f, static_cast<float>(hw - 1));
        fy = std::clamp(fy, 0.0f, static_cast<float>(hh - 1));
        int x0 = static_cast<int>(fx);
        int y0 = static_cast<int>(fy);
        int x1 = std::min<int>(x0 + 1, hw - 1);
        int y1 = std::min<int>(y0 + 1, hh - 1);
        uint32_t wx = static_cast<uint32_t>((fx - x0) * 256.0f);
        uint32_t wy = static_cast<uint32_t>((fy - y0) * 256.0f);
        uint32_t iwx = 256u - wx;
        uint32_t iwy = 256u - wy;

        DWORD p00 = halfBuf[static_cast<size_t>(y0) * hw + x0];
        DWORD p10 = halfBuf[static_cast<size_t>(y0) * hw + x1];
        DWORD p01 = halfBuf[static_cast<size_t>(y1) * hw + x0];
        DWORD p11 = halfBuf[static_cast<size_t>(y1) * hw + x1];

        auto ch = [&](int shift) -> uint32_t {
            uint32_t c0 = (((p00 >> shift) & 0xFF) * iwx + ((p10 >> shift) & 0xFF) * wx) >> 8;
            uint32_t c1 = (((p01 >> shift) & 0xFF) * iwx + ((p11 >> shift) & 0xFF) * wx) >> 8;
            return (c0 * iwy + c1 * wy) >> 8;
        };
        return (ch(16) << 16) | (ch(8) << 8) | ch(0);
    };

    cache.ReleaseGpu();
    cache.w = tw;
    cache.h = th;
    cache.isLight = isLight;
    cache.screenRect = rcTarget;
    cache.pixels.assign(static_cast<size_t>(tw) * th, 0u);

    const float cx = tw * 0.5f;
    const float cy = th * 0.5f;
    const float cornerR = std::clamp(cornerRadiusPx, 6.0f, std::min(tw, th) * 0.5f);
    const float hxBox = std::max(0.0f, cx - cornerR);
    const float hyBox = std::max(0.0f, cy - cornerR);
    const float bevelW  = isLiquid ? DpiScaleF(10.0f, dpi) : DpiScaleF(5.0f, dpi);
    const float maxDisp = isLiquid ? DpiScaleF(14.0f, dpi) : (s.uiStyle == UiStyle::Windows ? 0.0f : DpiScaleF(3.5f, dpi));

    for (int py = 0; py < th; ++py) {
        DWORD* dstRow = &cache.pixels[static_cast<size_t>(py) * tw];
        float dy = (py + 0.5f) - cy;
        float qy = std::fabs(dy) - hyBox;
        float oy = std::max(qy, 0.0f);

        for (int px = 0; px < tw; ++px) {
            float dx = (px + 0.5f) - cx;
            float qx = std::fabs(dx) - hxBox;
            float ox = std::max(qx, 0.0f);

            float outDist = std::sqrt(ox * ox + oy * oy);
            float inDist  = std::min(std::max(qx, qy), 0.0f);
            float sdf     = outDist + inDist - cornerR;

            float distFromEdge = std::max(0.0f, -sdf);
            float tBevel = (maxDisp > 0.1f && sdf < 0.0f && distFromEdge < bevelW && bevelW > 0.5f)
                ? (1.0f - distFromEdge / bevelW) : 0.0f;

            float dispX = 0.0f, dispY = 0.0f;
            if (tBevel > 0.01f && outDist > 1e-4f) {
                float nx = (dx >= 0.0f ? 1.0f : -1.0f) * (ox / outDist);
                float ny = (dy >= 0.0f ? 1.0f : -1.0f) * (oy / outDist);
                float curve = tBevel * tBevel;
                dispX = -nx * curve * maxDisp;
                dispY = -ny * curve * maxDisp;
            }

            float sx = (padPx + px + dispX) * 0.5f;
            float sy = (padPx + py + dispY) * 0.5f;
            dstRow[px] = 0xFF000000u | sampleBilinear(sx, sy);
        }
    }

    cache.valid = true;
    return true;
}

// ---------------------------------------------------------------------------
// 6. Direct2D & DirectWrite Resources (Win10 & Win11 Font Auto-Fallback)
// ---------------------------------------------------------------------------
static DibCanvas                  g_shelfCanvas;
static DibCanvas                  g_pillCanvas;
static DibCanvas                  g_alertCanvas;
static DibCanvas                  g_previewCanvas;
static ID2D1Factory*              g_pD2DFactory   = nullptr;
static ID2D1DCRenderTarget*       g_pDCRT         = nullptr;
static ID2D1SolidColorBrush*      g_pBrush        = nullptr;
static ID2D1LinearGradientBrush*  g_pRimLight     = nullptr;
static ID2D1LinearGradientBrush*  g_pRimDark      = nullptr;
static ID2D1StrokeStyle*          g_pDashedStroke = nullptr;
static IDWriteFactory*            g_pDWrite       = nullptr;

struct ShelfFonts {
    UINT dpi = 0;
    IDWriteTextFormat* pIcon        = nullptr;
    IDWriteTextFormat* pSmallIcon   = nullptr;
    IDWriteTextFormat* pTinyIcon    = nullptr;
    IDWriteTextFormat* pHeader      = nullptr;
    IDWriteTextFormat* pTitle       = nullptr;
    IDWriteTextFormat* pSub         = nullptr;
    IDWriteTextFormat* pTab         = nullptr;
    IDWriteTextFormat* pDropPrompt  = nullptr;

    void Release() {
        for (auto** pp : { &pIcon, &pSmallIcon, &pTinyIcon, &pHeader, &pTitle, &pSub, &pTab, &pDropPrompt }) {
            if (*pp) { (*pp)->Release(); *pp = nullptr; }
        }
        dpi = 0;
    }
};
static ShelfFonts g_fonts;

static bool IsFontFamilyInstalled(IDWriteFactory* pDWrite, const wchar_t* familyName) {
    if (!pDWrite || !familyName) return false;
    IDWriteFontCollection* pCol = nullptr;
    if (FAILED(pDWrite->GetSystemFontCollection(&pCol)) || !pCol) return false;
    UINT32 idx = 0;
    BOOL exists = FALSE;
    pCol->FindFamilyName(familyName, &idx, &exists);
    pCol->Release();
    return exists != FALSE;
}

struct CachedThumbGpu {
    ID2D1Bitmap*      pBmp   = nullptr;
    ID2D1BitmapBrush* pBrush = nullptr;
    void Release() {
        if (pBrush) { pBrush->Release(); pBrush = nullptr; }
        if (pBmp)   { pBmp->Release();   pBmp   = nullptr; }
    }
};
static std::unordered_map<uint64_t, CachedThumbGpu> g_thumbGpuCache;

static void ReleaseThumbGpuCache() {
    for (auto& kv : g_thumbGpuCache) kv.second.Release();
    g_thumbGpuCache.clear();
}

static void EvictThumbGpu(uint64_t itemId) {
    auto it = g_thumbGpuCache.find(itemId);
    if (it != g_thumbGpuCache.end()) {
        it->second.Release();
        g_thumbGpuCache.erase(it);
    }
}

static void ReleaseRenderResources() {
    ReleaseThumbGpuCache();
    g_glassCache.ClearAll();
    g_pillGlassCache.ClearAll();
    g_alertGlassCache.ClearAll();
    g_fonts.Release();
    if (g_pDashedStroke) { g_pDashedStroke->Release(); g_pDashedStroke = nullptr; }
    if (g_pRimLight)     { g_pRimLight->Release();     g_pRimLight = nullptr; }
    if (g_pRimDark)      { g_pRimDark->Release();      g_pRimDark = nullptr; }
    if (g_pBrush)        { g_pBrush->Release();        g_pBrush = nullptr; }
    if (g_pDCRT)         { g_pDCRT->Release();         g_pDCRT = nullptr; }
    if (g_pDWrite)       { g_pDWrite->Release();       g_pDWrite = nullptr; }
    if (g_pD2DFactory)   { g_pD2DFactory->Release();   g_pD2DFactory = nullptr; }
    g_shelfCanvas.Release();
    g_pillCanvas.Release();
    g_alertCanvas.Release();
    g_previewCanvas.Release();
    g_printWndCanvas.Release();
}

static bool EnsureRenderResources(UINT dpi) {
    if (!g_pD2DFactory) {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory),
                                     nullptr, reinterpret_cast<void**>(&g_pD2DFactory)))) {
            return false;
        }
    }
    if (!g_pDWrite) {
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                       reinterpret_cast<IUnknown**>(&g_pDWrite)))) {
            return false;
        }
    }
    if (!g_pDashedStroke && g_pD2DFactory) {
        D2D1_STROKE_STYLE_PROPERTIES props = D2D1::StrokeStyleProperties(
            D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
            D2D1_LINE_JOIN_ROUND, 10.0f, D2D1_DASH_STYLE_CUSTOM, 0.0f);
        float dashes[2] = { 4.0f, 3.5f };
        g_pD2DFactory->CreateStrokeStyle(props, dashes, 2, &g_pDashedStroke);
    }
    if (!g_pDCRT) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96.0f, 96.0f);
        if (SUCCEEDED(g_pD2DFactory->CreateDCRenderTarget(&props, &g_pDCRT)) && g_pDCRT) {
            g_pDCRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
            g_pDCRT->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &g_pBrush);

            auto makeRim = [&](bool light, ID2D1LinearGradientBrush** ppOut) {
                D2D1_GRADIENT_STOP stops[4] = {
                    { 0.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, light ? 0.85f : 0.42f) },
                    { 0.35f, D2D1::ColorF(0.75f, 0.88f, 1.0f, light ? 0.35f : 0.20f) },
                    { 0.72f, D2D1::ColorF(1.0f, 1.0f, 1.0f, light ? 0.16f : 0.12f) },
                    { 1.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, light ? 0.55f : 0.28f) }
                };
                ID2D1GradientStopCollection* pStops = nullptr;
                if (SUCCEEDED(g_pDCRT->CreateGradientStopCollection(stops, 4, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops))) {
                    g_pDCRT->CreateLinearGradientBrush(
                        D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(200, 100)),
                        pStops, ppOut);
                    pStops->Release();
                }
            };
            makeRim(true, &g_pRimLight);
            makeRim(false, &g_pRimDark);
        }
    }

    if (g_pDWrite && (g_fonts.dpi != dpi || !g_fonts.pTitle)) {
        g_fonts.Release();
        g_fonts.dpi = dpi;

        const wchar_t* iconFont = IsFontFamilyInstalled(g_pDWrite, L"Segoe Fluent Icons")
            ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
        const wchar_t* textFont = IsFontFamilyInstalled(g_pDWrite, L"Segoe UI Variable Text")
            ? L"Segoe UI Variable Text" : L"Segoe UI";

        g_pDWrite->CreateTextFormat(iconFont, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                    DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(17.5f, dpi), L"", &g_fonts.pIcon);
        g_pDWrite->CreateTextFormat(iconFont, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                    DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.0f, dpi), L"", &g_fonts.pSmallIcon);
        g_pDWrite->CreateTextFormat(iconFont, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                    DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(9.0f, dpi), L"", &g_fonts.pTinyIcon);

        g_pDWrite->CreateTextFormat(textFont, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                    DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(11.8f, dpi), L"", &g_fonts.pHeader);
        g_pDWrite->CreateTextFormat(textFont, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                    DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.4f, dpi), L"", &g_fonts.pTitle);
        g_pDWrite->CreateTextFormat(textFont, nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
                                    DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(10.6f, dpi), L"", &g_fonts.pSub);
        g_pDWrite->CreateTextFormat(textFont, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                    DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(11.2f, dpi), L"", &g_fonts.pTab);
        g_pDWrite->CreateTextFormat(textFont, nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                                    DWRITE_FONT_STRETCH_NORMAL, DpiScaleF(12.6f, dpi), L"", &g_fonts.pDropPrompt);

        for (auto* f : { g_fonts.pIcon, g_fonts.pSmallIcon, g_fonts.pTinyIcon, g_fonts.pTab, g_fonts.pDropPrompt }) {
            if (f) {
                f->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            }
        }
        auto setLeadingTrim = [&](IDWriteTextFormat* f) {
            if (!f) return;
            f->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            IDWriteInlineObject* pEll = nullptr;
            if (SUCCEEDED(g_pDWrite->CreateEllipsisTrimmingSign(f, &pEll)) && pEll) {
                DWRITE_TRIMMING trim = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
                f->SetTrimming(&trim, pEll);
                pEll->Release();
            }
        };
        setLeadingTrim(g_fonts.pHeader);
        setLeadingTrim(g_fonts.pTitle);
        setLeadingTrim(g_fonts.pSub);
    }
    return g_pDCRT != nullptr && g_pBrush != nullptr;
}

static bool EnsureGlassBrushForSlot(GlassBackdropCache& cache, ID2D1RenderTarget* pRT) {
    if (!cache.valid || cache.pixels.empty() || !pRT) return false;
    if (cache.pBrush && cache.pOwner == pRT) return true;

    cache.ReleaseGpu();
    D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
    if (SUCCEEDED(pRT->CreateBitmap(
            D2D1::SizeU(static_cast<UINT32>(cache.w), static_cast<UINT32>(cache.h)),
            cache.pixels.data(), static_cast<UINT32>(cache.w * 4),
            &bmpProps, &cache.pBmp)) && cache.pBmp) {
        D2D1_BITMAP_BRUSH_PROPERTIES bp = D2D1::BitmapBrushProperties(
            D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        if (SUCCEEDED(pRT->CreateBitmapBrush(cache.pBmp, bp, &cache.pBrush))) {
            cache.pOwner = pRT;
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// 7. Native OLE Drag & Drop (Files + Browser Virtual Images + PNG/DIB + Text + URLs)
// ---------------------------------------------------------------------------
static void RenderShelfFrame(bool rebuildGlass);
static void RenderTaskbarPill(bool rebuildGlass = false);
static void ShowShelfAnimated(bool summonNearCursor, POINT cursorAnchor);
static void HideShelfAnimated();
static void MinimizeShelfToTaskbar();
static void RestoreShelfFromTaskbar();

class ShelfEnumFormatEtc final : public IEnumFORMATETC {
    LONG m_ref = 1;
    ULONG m_idx = 0;
    std::vector<FORMATETC> m_fmts;
public:
    ShelfEnumFormatEtc(bool hasFiles, bool hasImage, bool hasText, bool hasUrl) {
        EnsureRegisteredFormats();
        if (hasFiles) {
            m_fmts.push_back({ CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL });
        }
        if (hasImage) {
            if (g_cfPng) m_fmts.push_back({ static_cast<CLIPFORMAT>(g_cfPng), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL });
            m_fmts.push_back({ CF_DIB, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL });
        }
        if (hasUrl && g_cfInetUrlW) {
            m_fmts.push_back({ static_cast<CLIPFORMAT>(g_cfInetUrlW), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL });
        }
        if (hasText) {
            m_fmts.push_back({ CF_UNICODETEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL });
            m_fmts.push_back({ CF_TEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL });
        }
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IEnumFORMATETC) {
            *ppv = static_cast<IEnumFORMATETC*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return static_cast<ULONG>(InterlockedIncrement(&m_ref)); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG r = static_cast<ULONG>(InterlockedDecrement(&m_ref));
        if (r == 0) delete this;
        return r;
    }
    HRESULT STDMETHODCALLTYPE Next(ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched) override {
        if (!rgelt) return E_POINTER;
        ULONG fetched = 0;
        while (fetched < celt && m_idx < m_fmts.size()) {
            rgelt[fetched++] = m_fmts[m_idx++];
        }
        if (pceltFetched) *pceltFetched = fetched;
        return (fetched == celt) ? S_OK : S_FALSE;
    }
    HRESULT STDMETHODCALLTYPE Skip(ULONG celt) override {
        m_idx += celt;
        return (m_idx <= m_fmts.size()) ? S_OK : S_FALSE;
    }
    HRESULT STDMETHODCALLTYPE Reset() override { m_idx = 0; return S_OK; }
    HRESULT STDMETHODCALLTYPE Clone(IEnumFORMATETC** ppenum) override {
        if (!ppenum) return E_POINTER;
        auto* c = new ShelfEnumFormatEtc(false, false, false, false);
        c->m_fmts = m_fmts;
        c->m_idx = m_idx;
        *ppenum = c;
        return S_OK;
    }
};

class ShelfDataObject final : public IDataObject {
    LONG m_ref = 1;
    std::vector<std::wstring> m_files;
    std::wstring              m_primaryImagePath;
    std::wstring              m_text;
    bool                      m_isSingleUrl = false;

    static HGLOBAL BuildHDropGlobal(const std::vector<std::wstring>& files) {
        size_t totalChars = 1;
        for (const auto& f : files) totalChars += f.size() + 1;
        SIZE_T cb = sizeof(DROPFILES) + totalChars * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GHND | GMEM_SHARE, cb);
        if (!hMem) return nullptr;
        auto* df = static_cast<DROPFILES*>(GlobalLock(hMem));
        if (!df) { GlobalFree(hMem); return nullptr; }
        df->pFiles = sizeof(DROPFILES);
        df->fWide = TRUE;
        wchar_t* dst = reinterpret_cast<wchar_t*>(reinterpret_cast<BYTE*>(df) + sizeof(DROPFILES));
        for (const auto& f : files) {
            std::memcpy(dst, f.c_str(), (f.size() + 1) * sizeof(wchar_t));
            dst += f.size() + 1;
        }
        *dst = L'\0';
        GlobalUnlock(hMem);
        return hMem;
    }

    static HGLOBAL BuildUnicodeTextGlobal(const std::wstring& txt) {
        SIZE_T cb = (txt.size() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GHND | GMEM_SHARE, cb);
        if (!hMem) return nullptr;
        wchar_t* dst = static_cast<wchar_t*>(GlobalLock(hMem));
        if (!dst) { GlobalFree(hMem); return nullptr; }
        std::memcpy(dst, txt.c_str(), cb);
        GlobalUnlock(hMem);
        return hMem;
    }

    static HGLOBAL BuildAnsiTextGlobal(const std::wstring& txt) {
        int needed = WideCharToMultiByte(CP_ACP, 0, txt.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (needed <= 0) return nullptr;
        HGLOBAL hMem = GlobalAlloc(GHND | GMEM_SHARE, static_cast<SIZE_T>(needed));
        if (!hMem) return nullptr;
        char* dst = static_cast<char*>(GlobalLock(hMem));
        if (!dst) { GlobalFree(hMem); return nullptr; }
        WideCharToMultiByte(CP_ACP, 0, txt.c_str(), -1, dst, needed, nullptr, nullptr);
        GlobalUnlock(hMem);
        return hMem;
    }

public:
    ShelfDataObject(std::vector<std::wstring> files, std::wstring primaryImagePath,
                    std::wstring text, bool isSingleUrl)
        : m_files(std::move(files)), m_primaryImagePath(std::move(primaryImagePath)),
          m_text(std::move(text)), m_isSingleUrl(isSingleUrl) {
        EnsureRegisteredFormats();
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDataObject) {
            *ppv = static_cast<IDataObject*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return static_cast<ULONG>(InterlockedIncrement(&m_ref)); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG r = static_cast<ULONG>(InterlockedDecrement(&m_ref));
        if (r == 0) delete this;
        return r;
    }
    HRESULT STDMETHODCALLTYPE GetData(FORMATETC* pfe, STGMEDIUM* pmed) override {
        if (!pfe || !pmed) return E_POINTER;
        if (pfe->dwAspect != DVASPECT_CONTENT || !(pfe->tymed & TYMED_HGLOBAL)) {
            return DV_E_FORMATETC;
        }
        if (pfe->cfFormat == CF_HDROP && !m_files.empty()) {
            HGLOBAL hMem = BuildHDropGlobal(m_files);
            if (!hMem) return E_OUTOFMEMORY;
            pmed->tymed = TYMED_HGLOBAL;
            pmed->hGlobal = hMem;
            pmed->pUnkForRelease = nullptr;
            return S_OK;
        }
        if (g_cfPng && pfe->cfFormat == g_cfPng && !m_primaryImagePath.empty()) {
            HGLOBAL hMem = LoadFileBytesToHGlobal(m_primaryImagePath.c_str());
            if (!hMem) return DV_E_FORMATETC;
            pmed->tymed = TYMED_HGLOBAL;
            pmed->hGlobal = hMem;
            pmed->pUnkForRelease = nullptr;
            return S_OK;
        }
        if (pfe->cfFormat == CF_DIB && !m_primaryImagePath.empty()) {
            HGLOBAL hMem = BuildDibHGlobalFromImageFile(m_primaryImagePath.c_str());
            if (!hMem) return DV_E_FORMATETC;
            pmed->tymed = TYMED_HGLOBAL;
            pmed->hGlobal = hMem;
            pmed->pUnkForRelease = nullptr;
            return S_OK;
        }
        if (g_cfInetUrlW && pfe->cfFormat == g_cfInetUrlW && m_isSingleUrl && !m_text.empty()) {
            HGLOBAL hMem = BuildUnicodeTextGlobal(m_text);
            if (!hMem) return E_OUTOFMEMORY;
            pmed->tymed = TYMED_HGLOBAL;
            pmed->hGlobal = hMem;
            pmed->pUnkForRelease = nullptr;
            return S_OK;
        }
        if (pfe->cfFormat == CF_UNICODETEXT && !m_text.empty()) {
            HGLOBAL hMem = BuildUnicodeTextGlobal(m_text);
            if (!hMem) return E_OUTOFMEMORY;
            pmed->tymed = TYMED_HGLOBAL;
            pmed->hGlobal = hMem;
            pmed->pUnkForRelease = nullptr;
            return S_OK;
        }
        if (pfe->cfFormat == CF_TEXT && !m_text.empty()) {
            HGLOBAL hMem = BuildAnsiTextGlobal(m_text);
            if (!hMem) return E_OUTOFMEMORY;
            pmed->tymed = TYMED_HGLOBAL;
            pmed->hGlobal = hMem;
            pmed->pUnkForRelease = nullptr;
            return S_OK;
        }
        return DV_E_FORMATETC;
    }
    HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC*, STGMEDIUM*) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC* pfe) override {
        if (!pfe) return E_POINTER;
        if (pfe->dwAspect != DVASPECT_CONTENT || !(pfe->tymed & TYMED_HGLOBAL)) return DV_E_FORMATETC;
        if (pfe->cfFormat == CF_HDROP && !m_files.empty()) return S_OK;
        if (g_cfPng && pfe->cfFormat == g_cfPng && !m_primaryImagePath.empty()) return S_OK;
        if (pfe->cfFormat == CF_DIB && !m_primaryImagePath.empty()) return S_OK;
        if (g_cfInetUrlW && pfe->cfFormat == g_cfInetUrlW && m_isSingleUrl && !m_text.empty()) return S_OK;
        if ((pfe->cfFormat == CF_UNICODETEXT || pfe->cfFormat == CF_TEXT) && !m_text.empty()) return S_OK;
        return DV_E_FORMATETC;
    }
    HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC*, FORMATETC* pfeOut) override {
        if (pfeOut) pfeOut->ptd = nullptr;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE SetData(FORMATETC*, STGMEDIUM*, BOOL) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDir, IEnumFORMATETC** ppEnum) override {
        if (!ppEnum) return E_POINTER;
        if (dwDir == DATADIR_GET) {
            *ppEnum = new ShelfEnumFormatEtc(!m_files.empty(), !m_primaryImagePath.empty(),
                                             !m_text.empty(), m_isSingleUrl && !m_text.empty());
            return S_OK;
        }
        *ppEnum = nullptr;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC*, DWORD, IAdviseSink*, DWORD*) override { return OLE_E_ADVISENOTSUPPORTED; }
    HRESULT STDMETHODCALLTYPE DUnadvise(DWORD) override { return OLE_E_ADVISENOTSUPPORTED; }
    HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA**) override { return OLE_E_ADVISENOTSUPPORTED; }
};

class ShelfDropSource final : public IDropSource {
    LONG m_ref = 1;
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDropSource) {
            *ppv = static_cast<IDropSource*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return static_cast<ULONG>(InterlockedIncrement(&m_ref)); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG r = static_cast<ULONG>(InterlockedDecrement(&m_ref));
        if (r == 0) delete this;
        return r;
    }
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) override {
        if (fEscapePressed) return DRAGDROP_S_CANCEL;
        if (!(grfKeyState & MK_LBUTTON)) return DRAGDROP_S_DROP;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD) override { return DRAGDROP_S_USEDEFAULTCURSORS; }
};

// ---------------------------------------------------------------------------
// 8. Shelf & Taskbar Pill State, Layout, Animations & Gestures
// ---------------------------------------------------------------------------
static bool      g_shelfVisible       = false;
static bool      g_shelfTargetOpen    = false;
static float     g_shelfAnim          = 0.0f;
static ULONGLONG g_lastAnimTick       = 0;
static bool      g_animActive         = false;

static bool      g_hoverDropActive    = false;
static bool      g_pillHoverDrop      = false;
static bool      g_pillHoverMouse     = false;
static float     g_pillAnim           = 0.0f;
static float     g_pillHoverAnim      = 0.0f;
static ULONGLONG g_pillLastAnimTick   = 0;
static bool      g_pillAnimActive     = false;
static bool      g_pillTargetOpen     = false;
static bool      g_draggingOut        = false;
static bool      g_movingShelf        = false;
static bool      g_hasCustomPos       = false;
static POINT     g_customTopLeft      = {};
static bool      g_hasLockedPos       = false;
static POINT     g_lockedTopLeft      = {};
static POINT     g_moveDragOffset     = {};
static POINT     g_summonAnchorPt     = {};
static bool      g_summonedByShake    = false;
static bool      g_openedByHotkey     = false;

static RECT      g_shelfScreenRect    = {};
static UINT      g_shelfDpi           = 96;
static RECT      g_pillScreenRect     = {};
static UINT      g_pillDpi            = 96;
static int       g_scrollOffset       = 0;
static std::wstring g_toastStatusText;

// Interactive hit rectangles (in shelf window client coordinates)
static D2D1_RECT_F g_rcBtnClose{};
static D2D1_RECT_F g_rcBtnMin{};
static D2D1_RECT_F g_rcBtnPin{};
static D2D1_RECT_F g_rcBtnPaste{};
static D2D1_RECT_F g_rcBtnCopy{};
static D2D1_RECT_F g_rcBtnReveal{};
static D2D1_RECT_F g_rcBtnClearHist{};
static D2D1_RECT_F g_rcTabShelf{};
static D2D1_RECT_F g_rcTabClip{};
static D2D1_RECT_F g_rcHeaderDrag{};
static D2D1_RECT_F g_rcFooterDragAll{};

struct RowHitEntry {
    int itemIndex = -1;
    D2D1_RECT_F rcRow{};
    D2D1_RECT_F rcAction{};
    D2D1_RECT_F rcRemove{};
};
static std::array<D2D1_RECT_F, 5> g_rcFilterChips{};
static int g_hoverFilterIdx = -1;
static int g_hoverRowAction = -1;
static std::vector<RowHitEntry> g_rowHits;

static int  g_hoverHeaderBtn = 0; // 1=close, 2=pin, 3=copy, 4=reveal, 5=paste, 6=dragAll, 7=min, 8=tabShelf, 9=tabClip
static int  g_hoverRowIdx    = -1;
static int  g_hoverRowRemove = -1;

static bool PtInRectF(float x, float y, const D2D1_RECT_F& r) {
    return (r.right > r.left) && x >= r.left && x <= r.right && y >= r.top && y <= r.bottom;
}

static void SetShelfToast(const std::wstring& msg) {
    g_toastStatusText = msg;
    if (g_shelfHwnd && IsWindow(g_shelfHwnd)) {
        SetTimer(g_shelfHwnd, IDT_TOAST_CLEAR, 1600, nullptr);
    }
    RenderShelfFrame(false);
}

struct ShakeDetector {
    POINT     lastPt{};
    bool      hasLast = false;
    int       lastDirX = 0;
    int       reversals = 0;
    ULONGLONG firstFlipTick = 0;
    ULONGLONG lastFlipTick = 0;
    ULONGLONG cooldownUntilTick = 0;

    void Reset() {
        hasLast = false;
        lastDirX = 0;
        reversals = 0;
        firstFlipTick = 0;
        lastFlipTick = 0;
    }

    bool Observe(POINT pt, ULONGLONG now, ShakeSensitivity sens) {
        if (now < cooldownUntilTick) return false;
        if (!hasLast) {
            lastPt = pt;
            hasLast = true;
            return false;
        }
        int dx = pt.x - lastPt.x;
        int minStep = (sens == ShakeSensitivity::High) ? 14 : ((sens == ShakeSensitivity::Low) ? 32 : 22);
        int reqFlips = (sens == ShakeSensitivity::High) ? 2 : ((sens == ShakeSensitivity::Low) ? 4 : 3);

        if (std::abs(dx) >= minStep) {
            int dir = (dx > 0) ? 1 : -1;
            lastPt = pt;
            if (lastDirX != 0 && dir != lastDirX) {
                if (reversals == 0 || (now - lastFlipTick) > 360) {
                    reversals = 1;
                    firstFlipTick = now;
                } else {
                    ++reversals;
                }
                lastFlipTick = now;
                if (reversals >= reqFlips && (now - firstFlipTick) <= 650) {
                    Reset();
                    cooldownUntilTick = now + 900;
                    return true;
                }
            }
            lastDirX = dir;
        } else if (reversals > 0 && (now - lastFlipTick) > 420) {
            reversals = 0;
        }
        return false;
    }
};
static ShakeDetector g_shakeDetector;

static bool IsFullscreenAppActiveOnMonitor(HMONITOR hMon) {
    if (!g_settings.hideInFullscreen || !hMon) return false;
    QUERY_USER_NOTIFICATION_STATE quns = QUNS_ACCEPTS_NOTIFICATIONS;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns)) && quns == QUNS_RUNNING_D3D_FULL_SCREEN) {
        HWND hFg = GetForegroundWindow();
        if (!hFg || MonitorFromWindow(hFg, MONITOR_DEFAULTTONEAREST) == hMon) return true;
    }
    return false;
}

static TaskbarEdge DetectTaskbarEdgeOnMonitor(HMONITOR hMon, const MONITORINFO& mi) {
    HWND hTb = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTb && IsWindow(hTb)) {
        HMONITOR hTbMon = MonitorFromWindow(hTb, MONITOR_DEFAULTTONEAREST);
        RECT rcTb{};
        if (hTbMon == hMon && GetWindowRect(hTb, &rcTb)) {
            int tbW = static_cast<int>(rcTb.right - rcTb.left);
            int tbH = static_cast<int>(rcTb.bottom - rcTb.top);
            int monMidX = static_cast<int>(mi.rcMonitor.left + mi.rcMonitor.right) / 2;
            int monMidY = static_cast<int>(mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
            if (tbH > tbW) {
                return (rcTb.left < monMidX) ? TaskbarEdge::Left : TaskbarEdge::Right;
            }
            return (rcTb.top < monMidY) ? TaskbarEdge::Top : TaskbarEdge::Bottom;
        }
    }
    // Fallback using work area vs monitor bounds
    if (mi.rcWork.top > mi.rcMonitor.top + 8)       return TaskbarEdge::Top;
    if (mi.rcWork.left > mi.rcMonitor.left + 8)     return TaskbarEdge::Left;
    if (mi.rcWork.right < mi.rcMonitor.right - 8)   return TaskbarEdge::Right;
    return TaskbarEdge::Bottom;
}

static bool ComputeDesiredShelfRect(RECT& outRect, UINT& outDpi) {
    POINT ptRef{};
    if (g_hasCustomPos) {
        ptRef = g_customTopLeft;
    } else if (g_hasLockedPos) {
        ptRef = g_lockedTopLeft;
    } else if (g_summonedByShake) {
        ptRef = g_summonAnchorPt;
    } else {
        GetCursorPos(&ptRef);
    }

    HMONITOR hMon = MonitorFromPoint(ptRef, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    if (!hMon || !GetMonitorInfoW(hMon, &mi)) {
        hMon = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
        GetMonitorInfoW(hMon, &mi);
    }

    outDpi = GetDpiForMonitorOrWindow(g_shelfHwnd);

    const auto& curList = GetActiveList();
    auto filteredIdx = BuildFilteredIndices(curList, g_activeFilter);
    int totalUnfiltered = static_cast<int>(curList.size());
    int itemCount = static_cast<int>(filteredIdx.size());
    int visibleRows = std::clamp<int>(itemCount, 0, 5);
    bool showTabBar = g_settings.replaceWinV;
    bool showFilterBar = (showTabBar || totalUnfiltered > 0);
    int tabBarH    = showTabBar ? DpiScale(30, outDpi) : 0;
    int filterBarH = showFilterBar ? DpiScale(26, outDpi) : 0;

    int cardW = 0;
    int cardH = 0;
    if (totalUnfiltered == 0 && !showTabBar) {
        cardW = DpiScale(296, outDpi);
        cardH = DpiScale(96, outDpi);
    } else {
        cardW = DpiScale(324, outDpi);
        int headerH = DpiScale(36, outDpi);
        if (itemCount == 0) {
            cardH = headerH + tabBarH + filterBarH + DpiScale(84, outDpi);
        } else {
            int rowH    = DpiScale(44, outDpi);
            int rowGap  = DpiScale(4, outDpi);
            int footerH = DpiScale(38, outDpi);
            int padV    = DpiScale(14, outDpi);
            cardH = headerH + tabBarH + filterBarH + visibleRows * rowH +
                    std::max<int>(0, visibleRows - 1) * rowGap + footerH + padV;
        }
    }

    int pad = DpiScale(14, outDpi);
    int monL = static_cast<int>(mi.rcWork.left);
    int monT = static_cast<int>(mi.rcWork.top);
    int monR = static_cast<int>(mi.rcWork.right);
    int monB = static_cast<int>(mi.rcWork.bottom);

    int x = monL + (monR - monL - cardW) / 2;
    int y = monT + pad;

    if (g_hasCustomPos) {
        x = static_cast<int>(g_customTopLeft.x);
        y = static_cast<int>(g_customTopLeft.y);
    } else if (g_hasLockedPos) {
        x = static_cast<int>(g_lockedTopLeft.x);
        y = static_cast<int>(g_lockedTopLeft.y);
    } else if (g_summonedByShake || g_settings.shelfPosition == ShelfPosition::NearCursor) {
        x = static_cast<int>(ptRef.x) + DpiScale(24, outDpi);
        y = static_cast<int>(ptRef.y) - cardH / 2;
        if (x + cardW > monR - pad) {
            x = static_cast<int>(ptRef.x) - cardW - DpiScale(24, outDpi);
        }
    } else {
        switch (g_settings.shelfPosition) {
            case ShelfPosition::TopCenter:
                x = monL + (monR - monL - cardW) / 2;
                y = monT + DpiScale(8, outDpi);
                break;
            case ShelfPosition::EdgeRight:
                x = monR - cardW - pad;
                y = monT + (monB - monT - cardH) / 2;
                break;
            case ShelfPosition::EdgeLeft:
                x = monL + pad;
                y = monT + (monB - monT - cardH) / 2;
                break;
            case ShelfPosition::AboveTaskbar:
                x = monL + (monR - monL - cardW) / 2;
                y = monB - cardH - DpiScale(12, outDpi);
                break;
            default:
                break;
        }
    }

    x = std::clamp<int>(x, monL + DpiScale(6, outDpi),
                        std::max<int>(monL + DpiScale(6, outDpi), monR - cardW - DpiScale(6, outDpi)));
    y = std::clamp<int>(y, monT + DpiScale(6, outDpi),
                        std::max<int>(monT + DpiScale(6, outDpi), monB - cardH - DpiScale(6, outDpi)));

    outRect = { x, y, x + cardW, y + cardH };
    return true;
}

static bool ComputeTaskbarPillRect(RECT& outRect, UINT& outDpi, bool& outIsVertical) {
    outIsVertical = false;
    POINT ptAnchor = {
        (g_shelfScreenRect.right > g_shelfScreenRect.left) ? (g_shelfScreenRect.left + g_shelfScreenRect.right) / 2 : 0,
        (g_shelfScreenRect.bottom > g_shelfScreenRect.top) ? (g_shelfScreenRect.top + g_shelfScreenRect.bottom) / 2 : 0
    };
    if (ptAnchor.x == 0 && ptAnchor.y == 0) GetCursorPos(&ptAnchor);

    HMONITOR hMon = MonitorFromPoint(ptAnchor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    if (!hMon || !GetMonitorInfoW(hMon, &mi)) {
        hMon = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
        GetMonitorInfoW(hMon, &mi);
    }
    if (IsFullscreenAppActiveOnMonitor(hMon)) return false;

    outDpi = GetDpiForMonitorOrWindow(g_pillHwnd ? g_pillHwnd : g_shelfHwnd);

    // Compact Top-Center Mini Bar (macOS Dynamic Island / Notch Bar style!)
    int baseW = g_stagedItems.empty() ? DpiScale(110, outDpi) : DpiScale(126, outDpi);
    int hoverExtraW = DpiScale(14, outDpi);
    int canvasW = baseW + hoverExtraW;
    int canvasH = DpiScale(34, outDpi);

    int workW = static_cast<int>(mi.rcWork.right - mi.rcWork.left);
    int pillX = static_cast<int>(mi.rcWork.left) + (workW - canvasW) / 2;
    int pillY = static_cast<int>(mi.rcWork.top) + DpiScale(6, outDpi);

    outRect = { pillX, pillY, pillX + canvasW, pillY + canvasH };
    return true;
}

static void StartShelfAnim() {
    if (!g_shelfHwnd || !IsWindow(g_shelfHwnd)) return;
    if (!g_animActive) {
        g_lastAnimTick = GetTickCount64();
        g_animActive = true;
        SetTimer(g_shelfHwnd, IDT_ANIM_FRAME, 12, nullptr);
    }
}

static void ShowShelfAnimated(bool summonNearCursor, POINT cursorAnchor) {
    if (!g_shelfHwnd || !IsWindow(g_shelfHwnd)) return;
    bool alreadyOpen = (g_shelfTargetOpen && g_shelfVisible && g_hasLockedPos);
    g_shelfMinimized = false;
    g_pillTargetOpen = false;
    g_pillAnim = 0.0f;
    if (g_pillHwnd && IsWindowVisible(g_pillHwnd)) {
        ShowWindow(g_pillHwnd, SW_HIDE);
    }
    if (!alreadyOpen) {
        g_hasLockedPos = false;
        g_summonedByShake = summonNearCursor;
        g_summonAnchorPt = cursorAnchor;
        if (summonNearCursor) {
            g_hasCustomPos = false;
    g_hasLockedPos = false;
        }
        RECT rcInit{};
        UINT initDpi = 96;
        if (ComputeDesiredShelfRect(rcInit, initDpi)) {
            g_lockedTopLeft = { rcInit.left, rcInit.top };
            g_hasLockedPos = true;
        }
    }
    g_shelfTargetOpen = true;
    g_shelfVisible = true;
    if (g_shelfAnim < 0.08f) g_shelfAnim = 0.08f;
    RenderShelfFrame(true);
    StartShelfAnim();
}

static void HideShelfAnimated() {
    if (!g_shelfTargetOpen && g_shelfAnim <= 0.01f) return;
    g_shelfTargetOpen = false;
    g_openedByHotkey = false;
    g_hoverDropActive = false;
    g_hoverHeaderBtn = 0;
    g_hoverRowIdx = -1;
    g_hoverRowRemove = -1;
    g_hasLockedPos = false;
    StartShelfAnim();
}

static void StartPillAnim() {
    if (!g_pillHwnd || !IsWindow(g_pillHwnd)) return;
    if (!g_pillAnimActive) {
        g_pillLastAnimTick = GetTickCount64();
        g_pillAnimActive = true;
        SetTimer(g_pillHwnd, IDT_PILL_ANIM, 12, nullptr);
    }
}

static void HideTopMiniBarAnimated() {
    g_shelfMinimized = false;
    g_pillTargetOpen = false;
    g_pillHoverMouse = false;
    g_pillHoverDrop = false;
    StartPillAnim();
}

static void MinimizeShelfToTaskbar() {
    if (g_stagedItems.empty()) {
        HideShelfAnimated();
        HideTopMiniBarAnimated();
        return;
    }
    g_shelfMinimized = true;
    g_pillTargetOpen = true;
    if (g_pillAnim < 0.08f) g_pillAnim = 0.08f;
    HideShelfAnimated();
    RenderTaskbarPill(true);
    StartPillAnim();
}

static void RestoreShelfFromTaskbar() {
    HideTopMiniBarAnimated();
    POINT pt{};
    GetCursorPos(&pt);
    g_openedByHotkey = true;
    ShowShelfAnimated(false, pt);
}

static bool IsClipboardContentProtected() {
    EnsureRegisteredFormats();
    if (g_cfExcludeHistory && IsClipboardFormatAvailable(g_cfExcludeHistory)) {
        return true;
    }
    if (g_cfCanIncludeHist && IsClipboardFormatAvailable(g_cfCanIncludeHist)) {
        if (HANDLE hFlag = GetClipboardData(g_cfCanIncludeHist)) {
            if (const DWORD* pAllow = static_cast<const DWORD*>(GlobalLock(hFlag))) {
                bool disallowed = (*pAllow == 0);
                GlobalUnlock(hFlag);
                if (disallowed) return true;
            }
        }
    }
    return false;
}

static void CaptureClipboardIntoList(std::vector<StagedItem>& target, int maxCap, bool insertFront, bool showToastIfEmpty) {
    EnsureRegisteredFormats();
    bool vi = ComputeUseVietnamese(g_settings.lang);
    if (!OpenClipboard(g_shelfHwnd)) return;

    // Respect Windows privacy / password manager exclusion flags
    if (insertFront && IsClipboardContentProtected()) {
        CloseClipboard();
        return;
    }

    bool added = false;
    if (IsClipboardFormatAvailable(CF_HDROP)) {
        if (HANDLE hMem = GetClipboardData(CF_HDROP)) {
            if (HDROP hDrop = static_cast<HDROP>(GlobalLock(hMem))) {
                UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
                UINT limit = insertFront ? std::min<UINT>(count, 4u) : count;
                for (UINT i = 0; i < limit; ++i) {
                    UINT need = DragQueryFileW(hDrop, i, nullptr, 0);
                    if (need > 0 && need < 4096) {
                        std::wstring fp(need, L'\0');
                        if (DragQueryFileW(hDrop, i, fp.data(), need + 1) > 0) {
                            if (AddFileToList(target, fp, false, false, maxCap, insertFront)) added = true;
                        }
                    }
                }
                GlobalUnlock(hMem);
            }
        }
    } else if (g_settings.acceptImageDrops && g_cfPng && IsClipboardFormatAvailable(g_cfPng)) {
        if (HANDLE hMem = GetClipboardData(g_cfPng)) {
            SIZE_T sz = GlobalSize(hMem);
            if (const BYTE* ptr = static_cast<const BYTE*>(GlobalLock(hMem))) {
                added = AddRawPngBytesToList(target, ptr, sz, L"", maxCap, insertFront);
                GlobalUnlock(hMem);
            }
        }
    } else if (g_settings.acceptImageDrops && IsClipboardFormatAvailable(CF_DIBV5)) {
        if (HANDLE hMem = GetClipboardData(CF_DIBV5)) {
            SIZE_T sz = GlobalSize(hMem);
            if (const BYTE* ptr = static_cast<const BYTE*>(GlobalLock(hMem))) {
                added = AddRawDibToList(target, ptr, sz, maxCap, insertFront);
                GlobalUnlock(hMem);
            }
        }
    } else if (g_settings.acceptImageDrops && IsClipboardFormatAvailable(CF_DIB)) {
        if (HANDLE hMem = GetClipboardData(CF_DIB)) {
            SIZE_T sz = GlobalSize(hMem);
            if (const BYTE* ptr = static_cast<const BYTE*>(GlobalLock(hMem))) {
                added = AddRawDibToList(target, ptr, sz, maxCap, insertFront);
                GlobalUnlock(hMem);
            }
        }
    } else if (g_settings.acceptTextDrops && IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        if (HANDLE hMem = GetClipboardData(CF_UNICODETEXT)) {
            if (const wchar_t* ptr = static_cast<const wchar_t*>(GlobalLock(hMem))) {
                added = AddTextToList(target, ptr, maxCap, insertFront);
                GlobalUnlock(hMem);
            }
        }
    }
    CloseClipboard();

    if (!insertFront) {
        if (added) {
            g_openedByHotkey = false;
            RenderShelfFrame(true);
            SetShelfToast(PickText(vi, L"Đã dán từ Clipboard", L"Added from Clipboard"));
        } else if (showToastIfEmpty) {
            SetShelfToast(PickText(vi, L"Clipboard trống hoặc đã có", L"Nothing new in Clipboard"));
        }
    } else if (added && g_shelfVisible && g_activeTab == ActiveTab::ClipboardHistory) {
        RenderShelfFrame(true);
    }
}

static void TriggerCopyPasteAlert(const std::wstring& icon, const std::wstring& title,
                                  const std::wstring& subtitle, const std::wstring& badge,
                                  bool isCut, bool isPaste);

static void WriteItemsToClipboard(const std::vector<StagedItem>& subset, bool showToast) {
    if (subset.empty()) return;
    EnsureRegisteredFormats();
    bool vi = ComputeUseVietnamese(g_settings.lang);
    auto files = CollectStagedFilePaths(subset);
    std::wstring primaryImg = CollectPrimaryImagePath(subset);
    bool isSingleUrl = false;
    std::wstring txt = CollectStagedTextJoined(subset, isSingleUrl);

    if (!OpenClipboard(g_shelfHwnd)) return;
    g_ignoreClipUntilTick = GetTickCount64() + 500;
    EmptyClipboard();

    if (!files.empty()) {
        size_t totalChars = 1;
        for (const auto& f : files) totalChars += f.size() + 1;
        SIZE_T cb = sizeof(DROPFILES) + totalChars * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GHND | GMEM_SHARE, cb);
        if (hMem) {
            auto* df = static_cast<DROPFILES*>(GlobalLock(hMem));
            if (df) {
                df->pFiles = sizeof(DROPFILES);
                df->fWide = TRUE;
                wchar_t* dst = reinterpret_cast<wchar_t*>(reinterpret_cast<BYTE*>(df) + sizeof(DROPFILES));
                for (const auto& f : files) {
                    std::memcpy(dst, f.c_str(), (f.size() + 1) * sizeof(wchar_t));
                    dst += f.size() + 1;
                }
                *dst = L'\0';
                GlobalUnlock(hMem);
                SetClipboardData(CF_HDROP, hMem);
            } else {
                GlobalFree(hMem);
            }
        }
    }
    if (!primaryImg.empty()) {
        if (g_cfPng) {
            if (HGLOBAL hPng = LoadFileBytesToHGlobal(primaryImg.c_str())) {
                if (!SetClipboardData(g_cfPng, hPng)) GlobalFree(hPng);
            }
        }
        if (HGLOBAL hDib = BuildDibHGlobalFromImageFile(primaryImg.c_str())) {
            if (!SetClipboardData(CF_DIB, hDib)) GlobalFree(hDib);
        }
    }
    if (!txt.empty()) {
        SIZE_T cb = (txt.size() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GHND | GMEM_SHARE, cb);
        if (hMem) {
            wchar_t* dst = static_cast<wchar_t*>(GlobalLock(hMem));
            if (dst) {
                std::memcpy(dst, txt.c_str(), cb);
                GlobalUnlock(hMem);
                SetClipboardData(CF_UNICODETEXT, hMem);
            } else {
                GlobalFree(hMem);
            }
        }
    }
    CloseClipboard();
    if (showToast) {
        SetShelfToast(PickText(vi, L"Đã sao chép vào Clipboard", L"Copied to Clipboard"));
        const auto& first = subset[0];
        std::wstring sub = (subset.size() == 1)
            ? first.displayName
            : (first.displayName + L" +" + std::to_wstring(subset.size() - 1));
        std::wstring badge = std::to_wstring(subset.size()) + PickText(vi, L" mục", subset.size() == 1 ? L" item" : L" items");
        const wchar_t* title =
            (first.kind == StagedItemKind::ImageBitmap) ? PickText(vi, L"Đã sao chép hình ảnh", L"Copied Image") :
            (first.kind == StagedItemKind::TextSnippet || first.kind == StagedItemKind::UrlLink)
                ? PickText(vi, L"Đã sao chép văn bản", L"Copied Text")
                : PickText(vi, L"Đã sao chép tệp", L"Copied Files");
        TriggerCopyPasteAlert(L"\uE8C8", title, sub, badge, false, false);
    }
}

static void PasteHistoryItemToPreviousWindow(int index) {
    if (index < 0 || index >= static_cast<int>(g_clipHistory.size())) return;
    const auto& chosen = g_clipHistory[index];
    bool vi = ComputeUseVietnamese(g_settings.lang);
    const wchar_t* pTitle =
        (chosen.kind == StagedItemKind::ImageBitmap) ? PickText(vi, L"Đã dán hình ảnh", L"Pasted Image") :
        (chosen.kind == StagedItemKind::TextSnippet || chosen.kind == StagedItemKind::UrlLink)
            ? PickText(vi, L"Đã dán văn bản", L"Pasted Text")
            : PickText(vi, L"Đã dán tệp", L"Pasted Files");
    std::wstring pSub = chosen.displayName;
    std::wstring pBadge = chosen.metaText;

    std::vector<StagedItem> single = { chosen };
    WriteItemsToClipboard(single, false);
    HideShelfAnimated();
    TriggerCopyPasteAlert(L"\uE77F", pTitle, pSub, pBadge, false, true);

    if (g_prevForegroundWnd && IsWindow(g_prevForegroundWnd) &&
        g_prevForegroundWnd != g_shelfHwnd && g_prevForegroundWnd != g_pillHwnd) {
        SetForegroundWindow(g_prevForegroundWnd);
    }
    Sleep(35);
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_CONTROL;
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = 'V';
    inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = 'V';          inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_CONTROL;   inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, inputs, sizeof(INPUT));
}

static void RevealItemInExplorer(const std::vector<StagedItem>& list, int index) {
    if (index < 0 || index >= static_cast<int>(list.size())) return;
    const auto& it = list[index];
    if (it.kind == StagedItemKind::File || it.kind == StagedItemKind::Folder ||
        it.kind == StagedItemKind::ImageBitmap) {
        std::wstring arg = L"/select,\"" + it.pathOrText + L"\"";
        ShellExecuteW(nullptr, L"open", L"explorer.exe", arg.c_str(), nullptr, SW_SHOWNORMAL);
    } else if (it.kind == StagedItemKind::UrlLink) {
        ShellExecuteW(nullptr, L"open", it.pathOrText.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}

static void AttachNativeDragImage(IDataObject* pDataObj, const std::shared_ptr<DecodedBgraImage>& img, UINT dpi) {
    if (!pDataObj) return;
    IDragSourceHelper* pHelper = nullptr;
    if (FAILED(CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&pHelper))) || !pHelper) {
        return;
    }

    int sz = DpiScale(64, dpi);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = sz;
    bmi.bmiHeader.biHeight      = -sz;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (hBmp && bits) {
        DWORD* dst = static_cast<DWORD*>(bits);
        std::fill_n(dst, static_cast<size_t>(sz) * sz, 0x00000000u);

        if (img && img->width > 0 && img->height > 0 && !img->pixels.empty()) {
            for (int y = 0; y < sz; ++y) {
                UINT sy = std::min<UINT>(img->height - 1, static_cast<UINT>(y * img->height / sz));
                for (int x = 0; x < sz; ++x) {
                    UINT sx = std::min<UINT>(img->width - 1, static_cast<UINT>(x * img->width / sz));
                    dst[static_cast<size_t>(y) * sz + x] = img->pixels[static_cast<size_t>(sy) * img->width + sx];
                }
            }
        } else {
            for (int y = 6; y < sz - 6; ++y) {
                for (int x = 6; x < sz - 6; ++x) {
                    dst[static_cast<size_t>(y) * sz + x] = 0xCC0A84FFu;
                }
            }
        }

        SHDRAGIMAGE sdi{};
        sdi.sizeDragImage = { sz, sz };
        sdi.ptOffset      = { sz / 2, sz / 2 };
        sdi.hbmpDragImage = hBmp;
        sdi.crColorKey    = 0x00000000;
        if (FAILED(pHelper->InitializeFromBitmap(&sdi, pDataObj))) {
            DeleteObject(hBmp);
        }
    }
    pHelper->Release();
}

static void ExecuteDragOut(std::vector<StagedItem>& sourceList, int singleItemIndex, bool isFromShelfTab) {
    if (sourceList.empty() || g_draggingOut) return;

    std::vector<StagedItem> subset;
    if (singleItemIndex >= 0 && singleItemIndex < static_cast<int>(sourceList.size())) {
        subset.push_back(sourceList[singleItemIndex]);
    } else {
        subset = sourceList;
    }

    auto files = CollectStagedFilePaths(subset);
    std::wstring primaryImg = CollectPrimaryImagePath(subset);
    bool isSingleUrl = false;
    std::wstring txt = CollectStagedTextJoined(subset, isSingleUrl);
    if (files.empty() && primaryImg.empty() && txt.empty()) return;

    g_draggingOut = true;
    auto* pData = new ShelfDataObject(files, primaryImg, txt, isSingleUrl);
    auto* pSrc  = new ShelfDropSource();

    std::shared_ptr<DecodedBgraImage> previewImg;
    for (const auto& it : subset) {
        if (it.thumb) { previewImg = it.thumb; break; }
    }
    AttachNativeDragImage(pData, previewImg, g_shelfDpi);

    DWORD effect = DROPEFFECT_NONE;
    DoDragDrop(pData, pSrc, DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK, &effect);

    pSrc->Release();
    pData->Release();
    g_draggingOut = false;

    bool holdCtrl = ((GetKeyState(VK_CONTROL) & 0x8000) != 0);
    if (effect != DROPEFFECT_NONE) {
        if (isFromShelfTab && g_settings.autoClearAfterDrop && !g_shelfPinned && !holdCtrl) {
            if (singleItemIndex >= 0 && singleItemIndex < static_cast<int>(sourceList.size())) {
                CleanupTempFileIfOwned(sourceList[singleItemIndex]);
                sourceList.erase(sourceList.begin() + singleItemIndex);
            } else {
                ClearVectorItems(sourceList);
            }
            g_scrollOffset = 0;
            if (g_stagedItems.empty()) {
                HideShelfAnimated();
                HideTopMiniBarAnimated();
            } else if (g_shelfMinimized) {
                RenderTaskbarPill(true);
            } else {
                RenderShelfFrame(true);
            }
        } else if (!isFromShelfTab && !g_shelfPinned && !holdCtrl) {
            HideShelfAnimated();
        }
    }
}

static bool ExtractVirtualFilesFromDataObject(IDataObject* pDataObj) {
    if (!pDataObj || !g_cfFileDescriptorW || !g_cfFileContents) return false;
    FORMATETC feDesc = { static_cast<CLIPFORMAT>(g_cfFileDescriptorW), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM medDesc{};
    if (FAILED(pDataObj->GetData(&feDesc, &medDesc)) || medDesc.tymed != TYMED_HGLOBAL || !medDesc.hGlobal) {
        return false;
    }

    bool addedAny = false;
    std::wstring dir = GetTempStagingDir();
    auto* fgd = static_cast<FILEGROUPDESCRIPTORW*>(GlobalLock(medDesc.hGlobal));
    if (fgd && !dir.empty()) {
        UINT count = std::min<UINT>(fgd->cItems, 16u);
        for (UINT i = 0; i < count; ++i) {
            std::wstring rawName = SanitizeFileName(fgd->fgd[i].cFileName);
            std::wstring outPath = dir + L"\\" + std::to_wstring(GetTickCount64()) + L"_" + std::to_wstring(i) + L"_" + rawName;

            FORMATETC feContent = { static_cast<CLIPFORMAT>(g_cfFileContents), nullptr, DVASPECT_CONTENT,
                                    static_cast<LONG>(i), TYMED_ISTREAM | TYMED_HGLOBAL };
            STGMEDIUM medContent{};
            if (SUCCEEDED(pDataObj->GetData(&feContent, &medContent))) {
                bool wrote = false;
                HANDLE hOut = CreateFileW(outPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (hOut != INVALID_HANDLE_VALUE) {
                    if (medContent.tymed == TYMED_ISTREAM && medContent.pstm) {
                        BYTE buf[16384];
                        ULONG cbRead = 0;
                        uint64_t totalWritten = 0;
                        while (totalWritten < 32ULL * 1024 * 1024 &&
                               SUCCEEDED(medContent.pstm->Read(buf, sizeof(buf), &cbRead)) && cbRead > 0) {
                            DWORD cbW = 0;
                            if (!WriteFile(hOut, buf, cbRead, &cbW, nullptr) || cbW != cbRead) break;
                            totalWritten += cbW;
                        }
                        wrote = (totalWritten > 0);
                    } else if (medContent.tymed == TYMED_HGLOBAL && medContent.hGlobal) {
                        SIZE_T sz = GlobalSize(medContent.hGlobal);
                        if (sz > 0 && sz <= 32 * 1024 * 1024) {
                            if (const BYTE* ptr = static_cast<const BYTE*>(GlobalLock(medContent.hGlobal))) {
                                DWORD cbW = 0;
                                wrote = (WriteFile(hOut, ptr, static_cast<DWORD>(sz), &cbW, nullptr) != FALSE && cbW == static_cast<DWORD>(sz));
                                GlobalUnlock(medContent.hGlobal);
                            }
                        }
                    }
                    CloseHandle(hOut);
                }
                ReleaseStgMedium(&medContent);

                if (wrote && AddFileToList(g_stagedItems, outPath, true, IsSupportedThumbnailExtension(outPath.c_str()),
                                           g_settings.maxStagedItems, false)) {
                    addedAny = true;
                } else {
                    DeleteFileW(outPath.c_str());
                }
            }
        }
        GlobalUnlock(medDesc.hGlobal);
    }
    ReleaseStgMedium(&medDesc);
    return addedAny;
}

class ShelfDropTarget final : public IDropTarget {
    LONG m_ref = 1;
    bool m_canAccept = false;
    bool m_isTaskbarPill = false;

    static bool HasSupportedFormat(IDataObject* pDataObj) {
        if (!pDataObj) return false;
        EnsureRegisteredFormats();
        auto has = [&](CLIPFORMAT cf, DWORD tymed = TYMED_HGLOBAL) -> bool {
            FORMATETC fe = { cf, nullptr, DVASPECT_CONTENT, -1, tymed };
            return pDataObj->QueryGetData(&fe) == S_OK;
        };
        if (has(CF_HDROP)) return true;
        if (g_settings.acceptImageDrops) {
            if (g_cfFileDescriptorW && has(static_cast<CLIPFORMAT>(g_cfFileDescriptorW))) return true;
            if (g_cfPng && has(static_cast<CLIPFORMAT>(g_cfPng))) return true;
            if (has(CF_DIBV5) || has(CF_DIB)) return true;
        }
        if (g_settings.acceptTextDrops) {
            if (g_cfInetUrlW && has(static_cast<CLIPFORMAT>(g_cfInetUrlW))) return true;
            if (has(CF_UNICODETEXT) || has(CF_TEXT)) return true;
        }
        return false;
    }

public:
    explicit ShelfDropTarget(bool isTaskbarPill) : m_isTaskbarPill(isTaskbarPill) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDropTarget) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return static_cast<ULONG>(InterlockedIncrement(&m_ref)); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG r = static_cast<ULONG>(InterlockedDecrement(&m_ref));
        if (r == 0) delete this;
        return r;
    }
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj, DWORD, POINTL, DWORD* pdwEffect) override {
        m_canAccept = HasSupportedFormat(pDataObj);
        if (pdwEffect) *pdwEffect = m_canAccept ? DROPEFFECT_COPY : DROPEFFECT_NONE;
        if (m_canAccept) {
            if (m_isTaskbarPill) {
                g_pillHoverDrop = true;
                RenderTaskbarPill(false);
            } else {
                g_hoverDropActive = true;
                RenderShelfFrame(false);
            }
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL, DWORD* pdwEffect) override {
        if (pdwEffect) *pdwEffect = m_canAccept ? DROPEFFECT_COPY : DROPEFFECT_NONE;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragLeave() override {
        m_canAccept = false;
        if (m_isTaskbarPill && g_pillHoverDrop) {
            g_pillHoverDrop = false;
            RenderTaskbarPill(false);
        } else if (!m_isTaskbarPill && g_hoverDropActive) {
            g_hoverDropActive = false;
            RenderShelfFrame(false);
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD, POINTL, DWORD* pdwEffect) override {
        EnsureRegisteredFormats();
        g_hoverDropActive = false;
        g_pillHoverDrop = false;
        if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
        if (!pDataObj || !m_canAccept) {
            if (m_isTaskbarPill) RenderTaskbarPill(false);
            else RenderShelfFrame(false);
            return S_OK;
        }

        bool addedAny = false;
        {
            FORMATETC feDrop = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM med{};
            if (SUCCEEDED(pDataObj->GetData(&feDrop, &med)) && med.tymed == TYMED_HGLOBAL && med.hGlobal) {
                HDROP hDrop = static_cast<HDROP>(med.hGlobal);
                UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
                for (UINT i = 0; i < count; ++i) {
                    UINT need = DragQueryFileW(hDrop, i, nullptr, 0);
                    if (need > 0 && need < 4096) {
                        std::wstring fp(need, L'\0');
                        if (DragQueryFileW(hDrop, i, fp.data(), need + 1) > 0) {
                            if (AddFileToList(g_stagedItems, fp, false, false, g_settings.maxStagedItems, false)) {
                                addedAny = true;
                            }
                        }
                    }
                }
                ReleaseStgMedium(&med);
            }
        }
        if (!addedAny && g_settings.acceptImageDrops) {
            addedAny = ExtractVirtualFilesFromDataObject(pDataObj);
        }
        if (!addedAny && g_settings.acceptImageDrops) {
            if (g_cfPng) {
                FORMATETC fePng = { static_cast<CLIPFORMAT>(g_cfPng), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
                STGMEDIUM med{};
                if (SUCCEEDED(pDataObj->GetData(&fePng, &med)) && med.tymed == TYMED_HGLOBAL && med.hGlobal) {
                    SIZE_T sz = GlobalSize(med.hGlobal);
                    if (const BYTE* ptr = static_cast<const BYTE*>(GlobalLock(med.hGlobal))) {
                        addedAny = AddRawPngBytesToList(g_stagedItems, ptr, sz, L"", g_settings.maxStagedItems, false);
                        GlobalUnlock(med.hGlobal);
                    }
                    ReleaseStgMedium(&med);
                }
            }
            if (!addedAny) {
                for (CLIPFORMAT cfDib : { static_cast<CLIPFORMAT>(CF_DIBV5), static_cast<CLIPFORMAT>(CF_DIB) }) {
                    FORMATETC feDib = { cfDib, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
                    STGMEDIUM med{};
                    if (SUCCEEDED(pDataObj->GetData(&feDib, &med)) && med.tymed == TYMED_HGLOBAL && med.hGlobal) {
                        SIZE_T sz = GlobalSize(med.hGlobal);
                        if (const BYTE* ptr = static_cast<const BYTE*>(GlobalLock(med.hGlobal))) {
                            addedAny = AddRawDibToList(g_stagedItems, ptr, sz, g_settings.maxStagedItems, false);
                            GlobalUnlock(med.hGlobal);
                        }
                        ReleaseStgMedium(&med);
                        if (addedAny) break;
                    }
                }
            }
        }
        if (!addedAny && g_settings.acceptTextDrops) {
            if (g_cfInetUrlW) {
                FORMATETC feUrl = { static_cast<CLIPFORMAT>(g_cfInetUrlW), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
                STGMEDIUM med{};
                if (SUCCEEDED(pDataObj->GetData(&feUrl, &med)) && med.tymed == TYMED_HGLOBAL && med.hGlobal) {
                    if (const wchar_t* pUrl = static_cast<const wchar_t*>(GlobalLock(med.hGlobal))) {
                        addedAny = AddTextToList(g_stagedItems, pUrl, g_settings.maxStagedItems, false);
                        GlobalUnlock(med.hGlobal);
                    }
                    ReleaseStgMedium(&med);
                }
            }
            if (!addedAny) {
                FORMATETC feTxt = { CF_UNICODETEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
                STGMEDIUM med{};
                if (SUCCEEDED(pDataObj->GetData(&feTxt, &med)) && med.tymed == TYMED_HGLOBAL && med.hGlobal) {
                    if (const wchar_t* pTxt = static_cast<const wchar_t*>(GlobalLock(med.hGlobal))) {
                        addedAny = AddTextToList(g_stagedItems, pTxt, g_settings.maxStagedItems, false);
                        GlobalUnlock(med.hGlobal);
                    }
                    ReleaseStgMedium(&med);
                }
            }
            if (!addedAny) {
                FORMATETC feAnsi = { CF_TEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
                STGMEDIUM med{};
                if (SUCCEEDED(pDataObj->GetData(&feAnsi, &med)) && med.tymed == TYMED_HGLOBAL && med.hGlobal) {
                    if (const char* pAnsi = static_cast<const char*>(GlobalLock(med.hGlobal))) {
                        int wlen = MultiByteToWideChar(CP_ACP, 0, pAnsi, -1, nullptr, 0);
                        if (wlen > 1) {
                            std::wstring wStr(static_cast<size_t>(wlen - 1), L'\0');
                            MultiByteToWideChar(CP_ACP, 0, pAnsi, -1, wStr.data(), wlen);
                            addedAny = AddTextToList(g_stagedItems, wStr, g_settings.maxStagedItems, false);
                        }
                        GlobalUnlock(med.hGlobal);
                    }
                    ReleaseStgMedium(&med);
                }
            }
        }

        if (addedAny && pdwEffect) *pdwEffect = DROPEFFECT_COPY;
        g_activeTab = ActiveTab::Shelf;
        if (m_isTaskbarPill) {
            RenderTaskbarPill(true);
        } else {
            g_shelfMinimized = false;
            g_shelfTargetOpen = true;
            g_shelfVisible = true;
            g_openedByHotkey = false;
            RenderShelfFrame(true);
        }
        return S_OK;
    }
};
static ShelfDropTarget* g_shelfDropTarget = nullptr;
static ShelfDropTarget* g_pillDropTarget  = nullptr;

// ---------------------------------------------------------------------------
// 9. Direct2D Renderers (Main Shelf + Minimized Taskbar Status Pill)
// ---------------------------------------------------------------------------
static void DrawThumbOrIcon(const D2D1_RECT_F& rBox, const StagedItem& item, bool isLight,
                            const D2D1_COLOR_F& accent, UINT dpi) {
    float bw = rBox.right - rBox.left;
    float bh = rBox.bottom - rBox.top;
    float rad = DpiScaleF(g_settings.uiStyle == UiStyle::Windows ? 5.0f : 7.0f, dpi);

    if (item.thumb && !item.thumb->pixels.empty() && g_pDCRT) {
        auto& entry = g_thumbGpuCache[item.id];
        if (!entry.pBmp) {
            if (g_thumbGpuCache.size() > 96) ReleaseThumbGpuCache();
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
            if (SUCCEEDED(g_pDCRT->CreateBitmap(
                    D2D1::SizeU(item.thumb->width, item.thumb->height),
                    item.thumb->pixels.data(), item.thumb->width * 4, &props, &entry.pBmp)) && entry.pBmp) {
                if (item.isImageThumb) {
                    D2D1_BITMAP_BRUSH_PROPERTIES bp = D2D1::BitmapBrushProperties(
                        D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    g_pDCRT->CreateBitmapBrush(entry.pBmp, bp, &entry.pBrush);
                }
            }
        }
        auto itEntry = g_thumbGpuCache.find(item.id);
        if (itEntry != g_thumbGpuCache.end() && itEntry->second.pBmp) {
            if (item.isImageThumb && itEntry->second.pBrush) {
                float sc = std::max(bw / static_cast<float>(item.thumb->width),
                                    bh / static_cast<float>(item.thumb->height));
                float ox = rBox.left + (bw - item.thumb->width * sc) * 0.5f;
                float oy = rBox.top  + (bh - item.thumb->height * sc) * 0.5f;
                itEntry->second.pBrush->SetTransform(
                    D2D1::Matrix3x2F::Scale(sc, sc) * D2D1::Matrix3x2F::Translation(ox, oy));
                D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rBox, rad, rad);
                g_pDCRT->FillRoundedRectangle(rr, itEntry->second.pBrush);
                g_pBrush->SetColor(isLight ? D2D1::ColorF(0, 0, 0, 0.14f) : D2D1::ColorF(1, 1, 1, 0.20f));
                g_pDCRT->DrawRoundedRectangle(rr, g_pBrush, 0.8f);
            } else {
                g_pDCRT->DrawBitmap(itEntry->second.pBmp, rBox, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
            return;
        }
    }

    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rBox, rad, rad);
    g_pBrush->SetColor(D2D1::ColorF(accent.r, accent.g, accent.b, 0.16f));
    g_pDCRT->FillRoundedRectangle(rr, g_pBrush);
    g_pBrush->SetColor(accent);
    const wchar_t* glyph = L"\uE8A5";
    if (item.kind == StagedItemKind::Folder)           glyph = L"\uE8B7";
    else if (item.kind == StagedItemKind::ImageBitmap) glyph = L"\uEB9F";
    else if (item.kind == StagedItemKind::AudioFile)   glyph = L"\uEC4F";
    else if (item.kind == StagedItemKind::UrlLink)     glyph = L"\uE71B";
    else if (item.kind == StagedItemKind::TextSnippet) glyph = L"\uE8C8";
    if (g_fonts.pIcon) g_pDCRT->DrawText(glyph, 1, g_fonts.pIcon, rBox, g_pBrush);
}

static void RenderTaskbarPill(bool rebuildGlass) {
    if (!g_pillHwnd || !IsWindow(g_pillHwnd)) return;

    float t = std::clamp(g_pillAnim, 0.0f, 1.0f);
    if (!g_pillTargetOpen && t <= 0.005f) {
        if (IsWindowVisible(g_pillHwnd)) {
            ShowWindow(g_pillHwnd, SW_HIDE);
        }
        return;
    }

    RECT rcTarget{};
    UINT dpi = 96;
    bool isVert = false;
    if (!ComputeTaskbarPillRect(rcTarget, dpi, isVert)) {
        if (IsWindowVisible(g_pillHwnd)) ShowWindow(g_pillHwnd, SW_HIDE);
        return;
    }

    int cw = static_cast<int>(rcTarget.right - rcTarget.left);
    int ch = static_cast<int>(rcTarget.bottom - rcTarget.top);
    if (cw <= 16 || ch <= 16) return;

    if (!EnsureRenderResources(dpi)) return;
    if (!g_pillCanvas.Ensure(cw, ch)) return;

    bool isWinTheme = (g_settings.uiStyle == UiStyle::Windows);
    float inv  = 1.0f - t;
    float ease = 1.0f - inv * inv * inv; // Smooth cubic ease-out

    float hovEase = std::clamp(g_pillHoverAnim, 0.0f, 1.0f);
    float hoverGrowPx = DpiScaleF(12.0f, dpi) * hovEase;
    float barW = std::clamp(static_cast<float>(cw) - DpiScaleF(14.0f, dpi) + hoverGrowPx,
                            DpiScaleF(72.0f, dpi), static_cast<float>(cw) - 2.0f) * (0.78f + 0.22f * ease);
    float barH = DpiScaleF(28.0f, dpi) + DpiScaleF(3.0f, dpi) * hovEase;
    float barL = (cw - barW) * 0.5f;
    // Gentle slide-down from top edge
    float slideOffsetY = -(1.0f - ease) * DpiScaleF(12.0f, dpi);
    float barT = std::max(0.6f, (ch - barH) * 0.5f + slideOffsetY);
    float pillRad = isWinTheme ? DpiScaleF(8.0f, dpi) : (barH * 0.5f);

    if (rebuildGlass || !g_pillGlassCache.valid ||
        g_pillGlassCache.w != cw || g_pillGlassCache.h != ch ||
        !EqualRect(&rcTarget, &g_pillGlassCache.screenRect)) {
        g_pillScreenRect = rcTarget;
        g_pillDpi = dpi;
        BuildFrostedGlassForSlot(g_pillGlassCache, rcTarget, pillRad, dpi, g_settings);
    }

    bool isLight = g_pillGlassCache.valid ? g_pillGlassCache.isLight : g_systemLightTheme;
    bool vi = ComputeUseVietnamese(g_settings.lang);
    D2D1_COLOR_F colAccent = ResolveAccentColor(isLight);
    D2D1_COLOR_F colText   = isLight ? D2D1::ColorF(0.08f, 0.09f, 0.12f, 1.0f)
                                     : D2D1::ColorF(0.96f, 0.97f, 1.00f, 1.0f);

    RECT rcBind = { 0, 0, cw, ch };
    if (SUCCEEDED(g_pDCRT->BindDC(g_pillCanvas.hdcMem, &rcBind))) {
        g_pDCRT->BeginDraw();
        g_pDCRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        g_pDCRT->Clear(D2D1::ColorF(0, 0, 0, 0));

        D2D1_RECT_F rBox = D2D1::RectF(barL, barT, barL + barW, barT + barH);
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rBox, pillRad, pillRad);

        if (g_settings.glassStyle != GlassStyle::SolidSoft &&
            EnsureGlassBrushForSlot(g_pillGlassCache, g_pDCRT) && g_pillGlassCache.pBrush) {
            g_pillGlassCache.pBrush->SetTransform(D2D1::Matrix3x2F::Identity());
            g_pDCRT->FillRoundedRectangle(rr, g_pillGlassCache.pBrush);
        } else {
            g_pBrush->SetColor(isLight ? D2D1::ColorF(0.96f, 0.965f, 0.98f, 0.96f)
                                       : D2D1::ColorF(0.12f, 0.125f, 0.15f, 0.95f));
            g_pDCRT->FillRoundedRectangle(rr, g_pBrush);
        }

        if (g_pillHoverDrop) {
            g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, 0.28f));
            g_pDCRT->FillRoundedRectangle(rr, g_pBrush);
        } else if (hovEase > 0.02f) {
            g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, 0.12f * hovEase));
            g_pDCRT->FillRoundedRectangle(rr, g_pBrush);
        }

        // Crisp rim + specular top highlight
        if (!isWinTheme) {
            ID2D1LinearGradientBrush* pRim = isLight ? g_pRimLight : g_pRimDark;
            if (pRim) {
                pRim->SetStartPoint(D2D1::Point2F(rBox.left, rBox.top));
                pRim->SetEndPoint(D2D1::Point2F(rBox.right, rBox.bottom));
                g_pDCRT->DrawRoundedRectangle(rr, pRim, DpiScaleF(1.0f, dpi));
            }
        }
        g_pBrush->SetColor(g_pillHoverDrop
            ? colAccent
            : (isLight ? D2D1::ColorF(0, 0, 0, 0.16f + 0.12f * hovEase)
                       : D2D1::ColorF(1, 1, 1, 0.20f + 0.14f * hovEase)));
        g_pDCRT->DrawRoundedRectangle(rr, g_pBrush, DpiScaleF(g_pillHoverDrop ? 1.5f : 0.95f, dpi));

        size_t n = g_stagedItems.size();
        bool isClipMin = (n == 0 && g_activeTab == ActiveTab::ClipboardHistory && g_settings.replaceWinV);
        float icSz = std::min(barH - DpiScaleF(8.0f, dpi), DpiScaleF(18.0f, dpi));
        D2D1_RECT_F rIc = D2D1::RectF(rBox.left + DpiScaleF(8.0f, dpi),
                                      rBox.top + (barH - icSz) * 0.5f,
                                      rBox.left + DpiScaleF(8.0f, dpi) + icSz,
                                      rBox.top + (barH + icSz) * 0.5f);
        if (n > 0) {
            DrawThumbOrIcon(rIc, g_stagedItems[0], isLight, colAccent, dpi);
        } else {
            g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, 0.20f));
            g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rIc, DpiScaleF(5.0f, dpi), DpiScaleF(5.0f, dpi)), g_pBrush);
            g_pBrush->SetColor(colAccent);
            if (g_fonts.pTinyIcon) g_pDCRT->DrawText(isClipMin ? L"\uE77F" : L"\uE896", 1, g_fonts.pTinyIcon, rIc, g_pBrush);
        }

        std::wstring label;
        if (g_pillHoverDrop) {
            label = PickText(vi, L"+ Thả thêm", L"+ Drop");
        } else if (n > 0) {
            label = std::to_wstring(n) + PickText(vi, L" mục", (n == 1 ? L" item" : L" items"));
        } else if (isClipMin) {
            label = L"Clipboard";
        } else {
            label = PickText(vi, L"Kệ thả", L"Shelf");
        }
        float txtL = rIc.right + DpiScaleF(6.0f, dpi);
        float txtR = rBox.right - DpiScaleF(g_shelfPinned ? 17.0f : 8.0f, dpi);
        g_pBrush->SetColor(colText);
        if (g_fonts.pHeader) {
            g_pDCRT->DrawText(label.c_str(), static_cast<UINT32>(label.size()),
                              g_fonts.pHeader, D2D1::RectF(txtL, rBox.top, txtR, rBox.bottom),
                              g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        if (g_shelfPinned) {
            float dotR = DpiScaleF(3.0f, dpi);
            g_pBrush->SetColor(D2D1::ColorF(1.00f, 0.74f, 0.18f, 1.0f));
            g_pDCRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rBox.right - DpiScaleF(9.5f, dpi), rBox.top + barH * 0.5f), dotR, dotR), g_pBrush);
        }

        HRESULT hr = g_pDCRT->EndDraw();
        if (SUCCEEDED(hr)) {
            BYTE masterAlpha = static_cast<BYTE>(std::clamp<int>(static_cast<int>(std::lround(ease * 255.0f)), 1, 255));
            HDC hdcScr = GetDC(nullptr);
            POINT ptSrc = { 0, 0 };
            POINT ptDst = { rcTarget.left, rcTarget.top };
            SIZE  szDst = { cw, ch };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, masterAlpha, AC_SRC_ALPHA };
            UpdateLayeredWindow(g_pillHwnd, hdcScr, &ptDst, &szDst, g_pillCanvas.hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);
            ReleaseDC(nullptr, hdcScr);

            static RECT s_lastPillSwp{};
            if (!IsWindowVisible(g_pillHwnd)) {
                s_lastPillSwp = rcTarget;
                SetWindowPos(g_pillHwnd, HWND_TOPMOST, rcTarget.left, rcTarget.top, cw, ch,
                             SWP_NOACTIVATE | SWP_SHOWWINDOW);
            } else if (!EqualRect(&s_lastPillSwp, &rcTarget)) {
                s_lastPillSwp = rcTarget;
                SetWindowPos(g_pillHwnd, HWND_TOPMOST, rcTarget.left, rcTarget.top, cw, ch,
                             SWP_NOACTIVATE | SWP_NOZORDER);
            }
        }
    }
}

static void RenderShelfFrame(bool rebuildGlass) {
    if (!g_shelfHwnd || !IsWindow(g_shelfHwnd)) return;

    float t = std::clamp(g_shelfAnim, 0.0f, 1.0f);
    if (t <= 0.005f && !g_shelfTargetOpen) {
        if (IsWindowVisible(g_shelfHwnd)) {
            ShowWindow(g_shelfHwnd, SW_HIDE);
        }
        g_shelfVisible = false;
        return;
    }

    RECT rcTarget{};
    UINT dpi = 96;
    if (!ComputeDesiredShelfRect(rcTarget, dpi)) return;
    int cw = static_cast<int>(rcTarget.right - rcTarget.left);
    int ch = static_cast<int>(rcTarget.bottom - rcTarget.top);
    if (cw <= 24 || ch <= 24) return;

    if (!EnsureRenderResources(dpi)) return;
    if (!g_shelfCanvas.Ensure(cw, ch)) return;

    bool isWinTheme = (g_settings.uiStyle == UiStyle::Windows);
    float cornerRad = isWinTheme
        ? DpiScaleF(10.0f, dpi)
        : ((g_stagedItems.empty() && !g_settings.replaceWinV && g_settings.shelfPosition == ShelfPosition::TopCenter && !g_hasCustomPos)
            ? DpiScaleF(22.0f, dpi) : DpiScaleF(18.0f, dpi));

    if (rebuildGlass || !g_glassCache.valid ||
        g_glassCache.w != cw || g_glassCache.h != ch ||
        !EqualRect(&rcTarget, &g_glassCache.screenRect)) {
        g_shelfScreenRect = rcTarget;
        g_shelfDpi = dpi;
        BuildFrostedGlassForSlot(g_glassCache, rcTarget, cornerRad, dpi, g_settings, g_movingShelf);
    }

    bool isLight = g_glassCache.valid ? g_glassCache.isLight : g_systemLightTheme;
    bool vi = ComputeUseVietnamese(g_settings.lang);
    const auto& curList = GetActiveList();
    bool isClipTab = (g_activeTab == ActiveTab::ClipboardHistory && g_settings.replaceWinV);
    bool showTabBar = g_settings.replaceWinV;

    float inv  = 1.0f - t;
    float ease = 1.0f - inv * inv * inv;

    float scale = 0.92f + 0.08f * ease;
    float drawW = cw * scale;
    float drawH = ch * scale;
    float ox = (cw - drawW) * 0.5f;
    float oy = (g_settings.shelfPosition == ShelfPosition::TopCenter && !g_hasCustomPos)
        ? 0.0f : ((ch - drawH) * 0.5f);

    g_rcBtnClose = g_rcBtnMin = g_rcBtnPin = g_rcBtnPaste = g_rcBtnCopy = g_rcBtnReveal = g_rcBtnClearHist = {};
    g_rcTabShelf = g_rcTabClip = g_rcHeaderDrag = g_rcFooterDragAll = {};
    g_rcFilterChips = {};
    g_rowHits.clear();

    RECT rcBind = { 0, 0, cw, ch };
    if (SUCCEEDED(g_pDCRT->BindDC(g_shelfCanvas.hdcMem, &rcBind))) {
        g_pDCRT->BeginDraw();
        g_pDCRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        g_pDCRT->Clear(D2D1::ColorF(0, 0, 0, 0));

        D2D1_RECT_F rCard = D2D1::RectF(ox + 0.6f, oy + 0.6f, ox + drawW - 0.6f, oy + drawH - 0.6f);
        float rad = std::min<float>(cornerRad, std::min(drawW, drawH) * 0.48f);
        D2D1_ROUNDED_RECT rrCard = D2D1::RoundedRect(rCard, rad, rad);

        if (g_settings.glassStyle != GlassStyle::SolidSoft &&
            EnsureGlassBrushForSlot(g_glassCache, g_pDCRT) && g_glassCache.pBrush) {
            g_glassCache.pBrush->SetTransform(
                D2D1::Matrix3x2F::Scale(scale, scale) * D2D1::Matrix3x2F::Translation(ox, oy));
            g_pDCRT->FillRoundedRectangle(rrCard, g_glassCache.pBrush);
        } else {
            g_pBrush->SetColor(isLight
                ? D2D1::ColorF(0.965f, 0.970f, 0.985f, 0.97f)
                : (isWinTheme ? D2D1::ColorF(0.125f, 0.125f, 0.135f, 0.97f)
                              : D2D1::ColorF(0.115f, 0.120f, 0.145f, 0.95f)));
            g_pDCRT->FillRoundedRectangle(rrCard, g_pBrush);
        }

        D2D1_COLOR_F colAccent = ResolveAccentColor(isLight);
        D2D1_COLOR_F colText = isLight
            ? D2D1::ColorF(0.08f, 0.09f, 0.12f, 1.0f)
            : D2D1::ColorF(0.96f, 0.97f, 1.00f, 1.0f);
        D2D1_COLOR_F colSub = isLight
            ? D2D1::ColorF(0.32f, 0.35f, 0.42f, 1.0f)
            : D2D1::ColorF(0.68f, 0.72f, 0.80f, 1.0f);
        D2D1_COLOR_F colRowBg = isLight
            ? D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.045f)
            : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.060f);
        D2D1_COLOR_F colRowHov = D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, isLight ? 0.12f : 0.19f);

        if (g_hoverDropActive) {
            g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, 0.16f));
            g_pDCRT->FillRoundedRectangle(rrCard, g_pBrush);
        }

        if (isWinTheme) {
            // Clean Windows 11/10 Fluent border + subtle top inner highlight
            g_pBrush->SetColor(isLight ? D2D1::ColorF(0, 0, 0, 0.15f) : D2D1::ColorF(1, 1, 1, 0.15f));
            g_pDCRT->DrawRoundedRectangle(rrCard, g_pBrush, DpiScaleF(1.0f, dpi));
            g_pBrush->SetColor(isLight ? D2D1::ColorF(1, 1, 1, 0.70f) : D2D1::ColorF(1, 1, 1, 0.18f));
            g_pDCRT->DrawLine(D2D1::Point2F(rCard.left + rad, rCard.top + 1.0f),
                              D2D1::Point2F(rCard.right - rad, rCard.top + 1.0f), g_pBrush, 1.0f);
        } else {
            ID2D1LinearGradientBrush* pRim = isLight ? g_pRimLight : g_pRimDark;
            if (pRim) {
                pRim->SetStartPoint(D2D1::Point2F(rCard.left, rCard.top));
                pRim->SetEndPoint(D2D1::Point2F(rCard.right, rCard.bottom));
                g_pDCRT->DrawRoundedRectangle(rrCard, pRim, DpiScaleF(1.1f, dpi));
            }
            g_pBrush->SetColor(isLight ? D2D1::ColorF(0, 0, 0, 0.12f) : D2D1::ColorF(1, 1, 1, 0.10f));
            g_pDCRT->DrawRoundedRectangle(rrCard, g_pBrush, DpiScaleF(0.75f, dpi));
        }

        // -------------------------------------------------------------------
        // A. COMPACT EMPTY DROP ZONE (when Shelf empty & Win+V tab disabled)
        // -------------------------------------------------------------------
        if (curList.empty() && !showTabBar) {
            float inset = DpiScaleF(8.0f, dpi);
            D2D1_RECT_F rInner = D2D1::RectF(rCard.left + inset, rCard.top + inset,
                                             rCard.right - inset, rCard.bottom - inset);
            float innerRad = std::max(5.0f, rad - inset * 0.65f);
            g_pBrush->SetColor(g_hoverDropActive
                ? colAccent
                : (isLight ? D2D1::ColorF(0, 0, 0, 0.22f) : D2D1::ColorF(1, 1, 1, 0.24f)));
            g_pDCRT->DrawRoundedRectangle(D2D1::RoundedRect(rInner, innerRad, innerRad),
                                          g_pBrush, DpiScaleF(g_hoverDropActive ? 1.8f : 1.3f, dpi), g_pDashedStroke);

            float icSz = DpiScaleF(28.0f, dpi);
            D2D1_RECT_F rIc = D2D1::RectF(rInner.left + DpiScaleF(12.0f, dpi),
                                          (rInner.top + rInner.bottom - icSz) * 0.5f,
                                          rInner.left + DpiScaleF(12.0f, dpi) + icSz,
                                          (rInner.top + rInner.bottom + icSz) * 0.5f);
            g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, g_hoverDropActive ? 0.28f : 0.14f));
            g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rIc, DpiScaleF(isWinTheme ? 6.0f : 8.0f, dpi),
                                                            DpiScaleF(isWinTheme ? 6.0f : 8.0f, dpi)), g_pBrush);
            g_pBrush->SetColor(colAccent);
            if (g_fonts.pIcon) g_pDCRT->DrawText(L"\uE896", 1, g_fonts.pIcon, rIc, g_pBrush);

            float actSz = DpiScaleF(24.0f, dpi);
            float actGap = DpiScaleF(4.0f, dpi);
            float actY = (rInner.top + rInner.bottom - actSz) * 0.5f;

            // Hide to left taskbar pill button
            g_rcBtnMin = D2D1::RectF(rInner.right - DpiScaleF(10.0f, dpi) - actSz, actY,
                                     rInner.right - DpiScaleF(10.0f, dpi), actY + actSz);
            g_pBrush->SetColor(g_hoverHeaderBtn == 7 ? colRowHov : colRowBg);
            g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(g_rcBtnMin, DpiScaleF(6.0f, dpi), DpiScaleF(6.0f, dpi)), g_pBrush);
            g_pBrush->SetColor(g_hoverHeaderBtn == 7 ? colAccent : colText);
            if (g_fonts.pSmallIcon) g_pDCRT->DrawText(L"\uE921", 1, g_fonts.pSmallIcon, g_rcBtnMin, g_pBrush);

            // Quick Paste from Clipboard button
            g_rcBtnPaste = D2D1::RectF(g_rcBtnMin.left - actGap - actSz, actY,
                                       g_rcBtnMin.left - actGap, actY + actSz);
            g_pBrush->SetColor(g_hoverHeaderBtn == 5 ? colRowHov : colRowBg);
            g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(g_rcBtnPaste, DpiScaleF(6.0f, dpi), DpiScaleF(6.0f, dpi)), g_pBrush);
            g_pBrush->SetColor(g_hoverHeaderBtn == 5 ? colAccent : colText);
            if (g_fonts.pSmallIcon) g_pDCRT->DrawText(L"\uE77F", 1, g_fonts.pSmallIcon, g_rcBtnPaste, g_pBrush);

            float txtL = rIc.right + DpiScaleF(10.0f, dpi);
            float txtR = g_rcBtnPaste.left - DpiScaleF(6.0f, dpi);
            float midY = (rInner.top + rInner.bottom) * 0.5f;

            std::wstring mainPrompt = !g_toastStatusText.empty()
                ? g_toastStatusText
                : (g_hoverDropActive
                    ? PickText(vi, L"Thả vào kệ ngay", L"Release to stage")
                    : PickText(vi, L"Thả tệp, ảnh, văn bản", L"Drop files, images, text"));
            std::wstring subPrompt = PickText(vi, L"Hoặc bấm dán từ Clipboard", L"Or click paste from Clipboard");

            g_pBrush->SetColor(!g_toastStatusText.empty() ? colAccent : colText);
            if (g_fonts.pTitle) {
                g_pDCRT->DrawText(mainPrompt.c_str(), static_cast<UINT32>(mainPrompt.size()),
                                  g_fonts.pTitle, D2D1::RectF(txtL, rInner.top + DpiScaleF(14.0f, dpi), txtR, midY + DpiScaleF(2.0f, dpi)),
                                  g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }
            g_pBrush->SetColor(colSub);
            if (g_fonts.pSub) {
                g_pDCRT->DrawText(subPrompt.c_str(), static_cast<UINT32>(subPrompt.size()),
                                  g_fonts.pSub, D2D1::RectF(txtL, midY + DpiScaleF(1.0f, dpi), txtR, rInner.bottom - DpiScaleF(13.0f, dpi)),
                                  g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }
            g_rcHeaderDrag = rCard;
        } else {
            // ---------------------------------------------------------------
            // B. FULL SHELF / WIN+V HUB: Dual macOS or Windows Header + Tabs + List + Footer
            // ---------------------------------------------------------------
            float padX    = DpiScaleF(12.0f, dpi);
            float headerH = DpiScaleF(36.0f, dpi);
            float headerB = rCard.top + headerH;

            g_pBrush->SetColor(isLight ? D2D1::ColorF(0, 0, 0, 0.08f) : D2D1::ColorF(1, 1, 1, 0.09f));
            g_pDCRT->DrawLine(D2D1::Point2F(rCard.left + padX, headerB),
                              D2D1::Point2F(rCard.right - padX, headerB), g_pBrush, 1.0f);

            float leftX  = rCard.left + padX;
            float rightX = rCard.right - padX;
            float btnSz  = DpiScaleF(22.0f, dpi);
            float btnGap = DpiScaleF(4.0f, dpi);
            float btnY   = rCard.top + (headerH - btnSz) * 0.5f;

            auto drawSmallBtn = [&](const D2D1_RECT_F& rB, const wchar_t* glyph, bool hov, bool dangerHov = false, bool activeTint = false) {
                if (dangerHov && hov) {
                    g_pBrush->SetColor(D2D1::ColorF(0.769f, 0.169f, 0.110f, 0.95f)); // Windows red #C42B1C
                } else if (activeTint) {
                    g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, hov ? 0.30f : 0.20f));
                } else {
                    g_pBrush->SetColor(hov ? colRowHov : colRowBg);
                }
                g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rB, DpiScaleF(isWinTheme ? 4.0f : 6.0f, dpi),
                                                                DpiScaleF(isWinTheme ? 4.0f : 6.0f, dpi)), g_pBrush);
                g_pBrush->SetColor((dangerHov && hov) ? D2D1::ColorF(1, 1, 1, 1) : ((hov || activeTint) ? colAccent : colText));
                if (g_fonts.pSmallIcon) g_pDCRT->DrawText(glyph, 1, g_fonts.pSmallIcon, rB, g_pBrush);
            };

            if (UseMacTrafficButtons()) {
                // macOS Traffic Lights on LEFT: Red Close, Yellow Minimize to Taskbar, Green/Amber Pin
                float dotD   = DpiScaleF(12.0f, dpi);
                float dotY   = rCard.top + (headerH - dotD) * 0.5f;
                float dotGap = DpiScaleF(7.0f, dpi);

                // Red = Close & Clear
                g_rcBtnClose = D2D1::RectF(leftX, dotY, leftX + dotD, dotY + dotD);
                g_pBrush->SetColor(D2D1::ColorF(1.00f, 0.373f, 0.337f, 1.0f));
                g_pDCRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(leftX + dotD * 0.5f, dotY + dotD * 0.5f), dotD * 0.5f, dotD * 0.5f), g_pBrush);
                if (g_hoverHeaderBtn == 1 && g_fonts.pTinyIcon) {
                    g_pBrush->SetColor(D2D1::ColorF(0.30f, 0.04f, 0.04f, 0.95f));
                    g_pDCRT->DrawText(L"\uE711", 1, g_fonts.pTinyIcon, g_rcBtnClose, g_pBrush);
                }

                // Yellow = Minimize to Taskbar Status Pill
                float minX = leftX + dotD + dotGap;
                g_rcBtnMin = D2D1::RectF(minX, dotY, minX + dotD, dotY + dotD);
                g_pBrush->SetColor(D2D1::ColorF(1.00f, 0.741f, 0.180f, 1.0f));
                g_pDCRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(minX + dotD * 0.5f, dotY + dotD * 0.5f), dotD * 0.5f, dotD * 0.5f), g_pBrush);
                if (g_hoverHeaderBtn == 7 && g_fonts.pTinyIcon) {
                    g_pBrush->SetColor(D2D1::ColorF(0.32f, 0.18f, 0.0f, 0.95f));
                    g_pDCRT->DrawText(L"\uE921", 1, g_fonts.pTinyIcon, g_rcBtnMin, g_pBrush);
                }

                // Green = Pin Toggle
                float pinX = minX + dotD + dotGap;
                g_rcBtnPin = D2D1::RectF(pinX, dotY, pinX + dotD, dotY + dotD);
                g_pBrush->SetColor(g_shelfPinned
                    ? D2D1::ColorF(0.157f, 0.804f, 0.255f, 1.0f) // #28CD41
                    : D2D1::ColorF(0.157f, 0.804f, 0.255f, 0.42f));
                g_pDCRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(pinX + dotD * 0.5f, dotY + dotD * 0.5f), dotD * 0.5f, dotD * 0.5f), g_pBrush);
                if (g_shelfPinned || g_hoverHeaderBtn == 2) {
                    g_pBrush->SetColor(D2D1::ColorF(0.03f, 0.25f, 0.06f, 0.95f));
                    g_pDCRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(pinX + dotD * 0.5f, dotY + dotD * 0.5f), dotD * 0.20f, dotD * 0.20f), g_pBrush);
                }

                leftX = pinX + dotD + DpiScaleF(10.0f, dpi);

                // Quick actions on RIGHT: Paste, Reveal, Copy
                g_rcBtnCopy   = D2D1::RectF(rightX - btnSz, btnY, rightX, btnY + btnSz);
                g_rcBtnReveal = D2D1::RectF(g_rcBtnCopy.left - btnGap - btnSz, btnY, g_rcBtnCopy.left - btnGap, btnY + btnSz);
                g_rcBtnPaste  = D2D1::RectF(g_rcBtnReveal.left - btnGap - btnSz, btnY, g_rcBtnReveal.left - btnGap, btnY + btnSz);
                drawSmallBtn(g_rcBtnPaste,  L"\uE77F", g_hoverHeaderBtn == 5);
                drawSmallBtn(g_rcBtnReveal, L"\uE8B7", g_hoverHeaderBtn == 4);
                drawSmallBtn(g_rcBtnCopy,   L"\uE8C8", g_hoverHeaderBtn == 3);
                rightX = g_rcBtnPaste.left - DpiScaleF(6.0f, dpi);
                if (!curList.empty()) {
                    g_rcBtnClearHist = D2D1::RectF(g_rcBtnPaste.left - btnGap - btnSz, btnY, g_rcBtnPaste.left - btnGap, btnY + btnSz);
                    drawSmallBtn(g_rcBtnClearHist, L"\uE74D", g_hoverHeaderBtn == 10, true, false);
                    rightX = g_rcBtnClearHist.left - DpiScaleF(6.0f, dpi);
                }
            } else {
                // Windows Native Caption Controls on RIGHT: Close (✕), Minimize (—), Pin (📌) + Paste / Copy
                g_rcBtnClose = D2D1::RectF(rightX - btnSz, btnY, rightX, btnY + btnSz);
                g_rcBtnMin   = D2D1::RectF(g_rcBtnClose.left - btnGap - btnSz, btnY, g_rcBtnClose.left - btnGap, btnY + btnSz);
                g_rcBtnPin   = D2D1::RectF(g_rcBtnMin.left - btnGap - btnSz, btnY, g_rcBtnMin.left - btnGap, btnY + btnSz);
                g_rcBtnCopy  = D2D1::RectF(g_rcBtnPin.left - btnGap - btnSz, btnY, g_rcBtnPin.left - btnGap, btnY + btnSz);
                g_rcBtnPaste = D2D1::RectF(g_rcBtnCopy.left - btnGap - btnSz, btnY, g_rcBtnCopy.left - btnGap, btnY + btnSz);

                drawSmallBtn(g_rcBtnClose, L"\uE8BB", g_hoverHeaderBtn == 1, true, false);
                drawSmallBtn(g_rcBtnMin,   L"\uE921", g_hoverHeaderBtn == 7, false, false);
                drawSmallBtn(g_rcBtnPin,   L"\uE718", g_hoverHeaderBtn == 2, false, g_shelfPinned);
                drawSmallBtn(g_rcBtnCopy,  L"\uE8C8", g_hoverHeaderBtn == 3, false, false);
                drawSmallBtn(g_rcBtnPaste, L"\uE77F", g_hoverHeaderBtn == 5, false, false);
                rightX = g_rcBtnPaste.left - DpiScaleF(6.0f, dpi);
                if (!curList.empty()) {
                    g_rcBtnClearHist = D2D1::RectF(g_rcBtnPaste.left - btnGap - btnSz, btnY, g_rcBtnPaste.left - btnGap, btnY + btnSz);
                    drawSmallBtn(g_rcBtnClearHist, L"\uE74D", g_hoverHeaderBtn == 10, true, false);
                    rightX = g_rcBtnClearHist.left - DpiScaleF(6.0f, dpi);
                }
            }

            // Header title
            std::wstring headerText = g_toastStatusText;
            if (headerText.empty()) {
                uint64_t totalBytes = 0;
                for (const auto& it : curList) totalBytes += it.sizeBytes;
                size_t n = curList.size();
                if (isClipTab) {
                    headerText = PickText(vi, L"Clipboard", L"Clipboard");
                    if (n > 0) headerText += L" (" + std::to_wstring(n) + L")";
                } else {
                    headerText = std::to_wstring(n) + PickText(vi, L" mục", (n == 1 ? L" item" : L" items"));
                    if (totalBytes > 0) headerText += L" · " + FormatFileSizeCompact(totalBytes);
                }
            }
            D2D1_RECT_F rHdrTxt = D2D1::RectF(leftX, rCard.top, rightX, headerB);
            g_rcHeaderDrag = rHdrTxt;
            g_pBrush->SetColor(g_toastStatusText.empty() ? colText : colAccent);
            if (g_fonts.pHeader) {
                g_pDCRT->DrawText(headerText.c_str(), static_cast<UINT32>(headerText.size()),
                                  g_fonts.pHeader, rHdrTxt, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }

            float contentTop = headerB + DpiScaleF(6.0f, dpi);

            // Optional 2-Tab Bar when Replace Win+V is enabled
            if (showTabBar) {
                float tabH = DpiScaleF(24.0f, dpi);
                D2D1_RECT_F rTabWrap = D2D1::RectF(rCard.left + padX, contentTop, rCard.right - padX, contentTop + tabH);
                float tabRad = DpiScaleF(isWinTheme ? 5.0f : 12.0f, dpi);
                g_pBrush->SetColor(colRowBg);
                g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rTabWrap, tabRad, tabRad), g_pBrush);

                float halfW = (rTabWrap.right - rTabWrap.left) * 0.5f;
                g_rcTabShelf = D2D1::RectF(rTabWrap.left + 2.0f, rTabWrap.top + 2.0f, rTabWrap.left + halfW - 1.0f, rTabWrap.bottom - 2.0f);
                g_rcTabClip  = D2D1::RectF(rTabWrap.left + halfW + 1.0f, rTabWrap.top + 2.0f, rTabWrap.right - 2.0f, rTabWrap.bottom - 2.0f);

                auto drawTab = [&](const D2D1_RECT_F& rT, const wchar_t* lbl, bool active, bool hov) {
                    if (active) {
                        g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, isLight ? 0.18f : 0.28f));
                        g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rT, std::max(3.0f, tabRad - 2.0f), std::max(3.0f, tabRad - 2.0f)), g_pBrush);
                    } else if (hov) {
                        g_pBrush->SetColor(colRowHov);
                        g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rT, std::max(3.0f, tabRad - 2.0f), std::max(3.0f, tabRad - 2.0f)), g_pBrush);
                    }
                    g_pBrush->SetColor(active ? colAccent : colSub);
                    if (g_fonts.pTab) g_pDCRT->DrawText(lbl, static_cast<UINT32>(wcslen(lbl)), g_fonts.pTab, rT, g_pBrush);
                };
                drawTab(g_rcTabShelf, PickText(vi, L"Kệ kéo-thả", L"Drop Shelf"), !isClipTab, g_hoverHeaderBtn == 8);
                drawTab(g_rcTabClip,  PickText(vi, L"Clipboard (Win+V)", L"Clipboard (Win+V)"), isClipTab, g_hoverHeaderBtn == 9);
                contentTop = rTabWrap.bottom + DpiScaleF(6.0f, dpi);
            }

            // Category Filter Chips (Tất cả · Văn bản · Ảnh · Tệp · Nhạc)
            {
                const wchar_t* chipLabels[5] = {
                    PickText(vi, L"Tất cả", L"All"),
                    PickText(vi, L"Văn bản", L"Text"),
                    PickText(vi, L"Ảnh", L"Images"),
                    PickText(vi, L"Tệp", L"Files"),
                    PickText(vi, L"Nhạc", L"Audio")
                };
                float chipH   = DpiScaleF(20.0f, dpi);
                float chipGap = DpiScaleF(4.0f, dpi);
                float availW  = (rCard.right - rCard.left) - padX * 2.0f;
                float chipW   = (availW - chipGap * 4.0f) / 5.0f;
                float chipRad = DpiScaleF(isWinTheme ? 5.0f : 10.0f, dpi);
                float cx = rCard.left + padX;
                for (int fi = 0; fi < 5; ++fi) {
                    D2D1_RECT_F rChip = D2D1::RectF(cx, contentTop, cx + chipW, contentTop + chipH);
                    g_rcFilterChips[fi] = rChip;
                    bool isAct = (static_cast<int>(g_activeFilter) == fi);
                    bool isHov = (g_hoverFilterIdx == fi);
                    if (isAct) {
                        g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, isLight ? 0.20f : 0.30f));
                    } else {
                        g_pBrush->SetColor(isHov ? colRowHov : colRowBg);
                    }
                    g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rChip, chipRad, chipRad), g_pBrush);
                    g_pBrush->SetColor(isAct ? colAccent : colSub);
                    if (g_fonts.pTab) {
                        g_pDCRT->DrawText(chipLabels[fi], static_cast<UINT32>(wcslen(chipLabels[fi])),
                                          g_fonts.pTab, rChip, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    }
                    cx += chipW + chipGap;
                }
                contentTop += chipH + DpiScaleF(6.0f, dpi);
            }

            auto filteredIdx = BuildFilteredIndices(curList, g_activeFilter);
            if (filteredIdx.empty()) {
                D2D1_RECT_F rEmpty = D2D1::RectF(rCard.left + padX, contentTop + DpiScaleF(4.0f, dpi),
                                                 rCard.right - padX, rCard.bottom - DpiScaleF(10.0f, dpi));
                g_pBrush->SetColor(g_hoverDropActive ? colAccent : colSub);
                g_pDCRT->DrawRoundedRectangle(D2D1::RoundedRect(rEmpty, DpiScaleF(10.0f, dpi), DpiScaleF(10.0f, dpi)),
                                              g_pBrush, 1.2f, g_pDashedStroke);
                std::wstring emptyMsg = (g_activeFilter != CategoryFilter::All && !curList.empty())
                    ? PickText(vi, L"Không có mục nào trong danh mục này", L"No items in this category")
                    : (isClipTab
                        ? PickText(vi, L"Chưa có mục Clipboard nào", L"Clipboard history is empty")
                        : PickText(vi, L"Thả tệp, ảnh hoặc văn bản vào đây", L"Drop files, images, or text here"));
                g_pBrush->SetColor(colSub);
                if (g_fonts.pTab) {
                    g_pDCRT->DrawText(emptyMsg.c_str(), static_cast<UINT32>(emptyMsg.size()), g_fonts.pTab, rEmpty, g_pBrush);
                }
            } else {
                int totalItems = static_cast<int>(filteredIdx.size());
                int visibleRows = std::min<int>(totalItems, 5);
                g_scrollOffset = std::clamp<int>(g_scrollOffset, 0, std::max<int>(0, totalItems - visibleRows));

                float rowH   = DpiScaleF(44.0f, dpi);
                float rowGap = DpiScaleF(4.0f, dpi);
                float curY   = contentTop;

                for (int r = 0; r < visibleRows; ++r) {
                    int idx = filteredIdx[g_scrollOffset + r];
                    const auto& item = curList[idx];
                    bool isPlayingThis = (item.kind == StagedItemKind::AudioFile && g_playingAudioItemId == item.id);

                    D2D1_RECT_F rRow = D2D1::RectF(rCard.left + padX, curY, rCard.right - padX, curY + rowH);
                    bool isHov = (g_hoverRowIdx == idx);
                    g_pBrush->SetColor(isHov ? colRowHov : colRowBg);
                    g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rRow, DpiScaleF(isWinTheme ? 6.0f : 10.0f, dpi),
                                                                    DpiScaleF(isWinTheme ? 6.0f : 10.0f, dpi)), g_pBrush);

                    float thSz = DpiScaleF(30.0f, dpi);
                    D2D1_RECT_F rThumb = D2D1::RectF(rRow.left + DpiScaleF(7.0f, dpi),
                                                     rRow.top + (rowH - thSz) * 0.5f,
                                                     rRow.left + DpiScaleF(7.0f, dpi) + thSz,
                                                     rRow.top + (rowH + thSz) * 0.5f);
                    DrawThumbOrIcon(rThumb, item, isLight, colAccent, dpi);

                    float rmSz = DpiScaleF(20.0f, dpi);
                    D2D1_RECT_F rRm = D2D1::RectF(rRow.right - DpiScaleF(7.0f, dpi) - rmSz,
                                                  rRow.top + (rowH - rmSz) * 0.5f,
                                                  rRow.right - DpiScaleF(7.0f, dpi),
                                                  rRow.top + (rowH + rmSz) * 0.5f);

                    // Context-aware per-row Quick Action button (Preview Image / Open Folder / Play Audio / Open URL)
                    D2D1_RECT_F rAct{};
                    const wchar_t* actGlyph = nullptr;
                    if (item.kind == StagedItemKind::ImageBitmap)   actGlyph = L"\uE7B3"; // Eye (Preview image)
                    else if (item.kind == StagedItemKind::AudioFile) actGlyph = isPlayingThis ? L"\uE769" : L"\uE768"; // Play/Pause
                    else if (item.kind == StagedItemKind::File || item.kind == StagedItemKind::Folder) actGlyph = L"\uE8B7"; // Folder
                    else if (item.kind == StagedItemKind::UrlLink)   actGlyph = L"\uE8A7"; // Open link

                    float txtRightLimit = rRm.left - DpiScaleF(4.0f, dpi);
                    if (actGlyph) {
                        rAct = D2D1::RectF(rRm.left - DpiScaleF(4.0f, dpi) - rmSz, rRm.top,
                                           rRm.left - DpiScaleF(4.0f, dpi), rRm.bottom);
                        txtRightLimit = rAct.left - DpiScaleF(4.0f, dpi);
                        if (isHov || isPlayingThis || g_hoverRowAction == idx) {
                            g_pBrush->SetColor((g_hoverRowAction == idx || isPlayingThis)
                                ? D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, 0.25f)
                                : (isLight ? D2D1::ColorF(0, 0, 0, 0.08f) : D2D1::ColorF(1, 1, 1, 0.12f)));
                            g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rAct, rmSz * 0.5f, rmSz * 0.5f), g_pBrush);
                            g_pBrush->SetColor((g_hoverRowAction == idx || isPlayingThis) ? colAccent : colText);
                            if (g_fonts.pTinyIcon) g_pDCRT->DrawText(actGlyph, 1, g_fonts.pTinyIcon, rAct, g_pBrush);
                        }
                    }

                    if (isHov || g_hoverRowRemove == idx) {
                        g_pBrush->SetColor(g_hoverRowRemove == idx
                            ? D2D1::ColorF(1.0f, 0.32f, 0.30f, 0.25f)
                            : (isLight ? D2D1::ColorF(0, 0, 0, 0.08f) : D2D1::ColorF(1, 1, 1, 0.12f)));
                        g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rRm, rmSz * 0.5f, rmSz * 0.5f), g_pBrush);
                        g_pBrush->SetColor(g_hoverRowRemove == idx ? D2D1::ColorF(1.0f, 0.35f, 0.32f, 1.0f) : colSub);
                        if (g_fonts.pTinyIcon) g_pDCRT->DrawText(L"\uE711", 1, g_fonts.pTinyIcon, rRm, g_pBrush);
                    }

                    float txtL = rThumb.right + DpiScaleF(9.0f, dpi);
                    float txtR = txtRightLimit;
                    float midY = (rRow.top + rRow.bottom) * 0.5f;

                    g_pBrush->SetColor(isPlayingThis ? colAccent : colText);
                    if (g_fonts.pTitle) {
                        g_pDCRT->DrawText(item.displayName.c_str(), static_cast<UINT32>(item.displayName.size()),
                                          g_fonts.pTitle, D2D1::RectF(txtL, rRow.top + DpiScaleF(5.0f, dpi), txtR, midY + DpiScaleF(1.0f, dpi)),
                                          g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    }
                    g_pBrush->SetColor(colSub);
                    if (g_fonts.pSub) {
                        g_pDCRT->DrawText(item.metaText.c_str(), static_cast<UINT32>(item.metaText.size()),
                                          g_fonts.pSub, D2D1::RectF(txtL, midY, txtR, rRow.bottom - DpiScaleF(4.0f, dpi)),
                                          g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                    }

                    g_rowHits.push_back({ idx, rRow, rAct, rRm });
                    curY += rowH + rowGap;
                }

                float footerH = DpiScaleF(30.0f, dpi);
                D2D1_RECT_F rFoot = D2D1::RectF(rCard.left + padX, rCard.bottom - DpiScaleF(8.0f, dpi) - footerH,
                                                rCard.right - padX, rCard.bottom - DpiScaleF(8.0f, dpi));
                g_rcFooterDragAll = rFoot;

                bool hovDragAll = (g_hoverHeaderBtn == 6);
                float footRad = isWinTheme ? DpiScaleF(6.0f, dpi) : (footerH * 0.5f);
                g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, hovDragAll ? 0.94f : 0.80f));
                g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rFoot, footRad, footRad), g_pBrush);

                std::wstring footLabel;
                if (isClipTab) {
                    footLabel = PickText(vi, L"Bấm để dán nhanh • Kéo để thả", L"Click to paste • Drag to drop");
                } else if (g_hoverDropActive) {
                    footLabel = PickText(vi, L"+ Thả thêm vào kệ", L"+ Drop to add more");
                } else if (totalItems == 1) {
                    footLabel = PickText(vi, L"Kéo mục này ra ứng dụng đích", L"Drag item to target app");
                } else {
                    footLabel = std::wstring(PickText(vi, L"Kéo tất cả (", L"Drag all (")) +
                                std::to_wstring(totalItems) + PickText(vi, L" mục) ra ngoài", L" items) out");
                }
                g_pBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.98f));
                if (g_fonts.pDropPrompt) {
                    g_pDCRT->DrawText(footLabel.c_str(), static_cast<UINT32>(footLabel.size()),
                                      g_fonts.pDropPrompt, rFoot, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                }
            }
        }

        HRESULT hr = g_pDCRT->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET) {
            ReleaseRenderResources();
            return;
        }
        if (SUCCEEDED(hr)) {
            BYTE masterAlpha = static_cast<BYTE>(std::clamp<int>(static_cast<int>(std::lround(ease * 255.0f)), 1, 255));
            HDC hdcScr = GetDC(nullptr);
            POINT ptSrc = { 0, 0 };
            POINT ptDst = { rcTarget.left, rcTarget.top };
            SIZE  szDst = { cw, ch };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, masterAlpha, AC_SRC_ALPHA };
            UpdateLayeredWindow(g_shelfHwnd, hdcScr, &ptDst, &szDst, g_shelfCanvas.hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);
            ReleaseDC(nullptr, hdcScr);

            static RECT s_lastShelfSwp{};
            if (!IsWindowVisible(g_shelfHwnd)) {
                s_lastShelfSwp = rcTarget;
                SetWindowPos(g_shelfHwnd, HWND_TOPMOST, rcTarget.left, rcTarget.top, cw, ch,
                             SWP_NOACTIVATE | SWP_SHOWWINDOW);
            } else if (!EqualRect(&s_lastShelfSwp, &rcTarget)) {
                s_lastShelfSwp = rcTarget;
                SetWindowPos(g_shelfHwnd, HWND_TOPMOST, rcTarget.left, rcTarget.top, cw, ch,
                             SWP_NOACTIVATE | SWP_NOZORDER);
            }
        }
    }
}


// ---------------------------------------------------------------------------
// 9b. Copy / Cut / Paste Live Toast Alert (Top-Center Non-Blocking Pill)
// ---------------------------------------------------------------------------
struct CopyPasteAlertState {
    std::wstring icon;
    std::wstring title;
    std::wstring subtitle;
    std::wstring badge;
    bool         isCut   = false;
    bool         isPaste = false;
    ULONGLONG    expireTick = 0;
};

struct CachedClipAlertInfo {
    DWORD        seq = 0;
    int          kind = 0; // 1=files, 2=text, 3=image
    std::wstring subtitle;
    std::wstring badge;
    bool         valid = false;
};

static CopyPasteAlertState g_cpAlert;
static CachedClipAlertInfo g_lastClipSummary;
static float               g_alertAnim = 0.0f;
static bool                g_alertTargetOpen = false;
static bool                g_alertAnimActive = false;
static ULONGLONG           g_alertLastAnimTick = 0;
static RECT                g_alertScreenRect = {};

static float MeasureTextWidthPx(IDWriteTextFormat* pFmt, const std::wstring& txt) {
    if (!g_pDWrite || !pFmt || txt.empty()) return 0.0f;
    IDWriteTextLayout* pLay = nullptr;
    float w = 0.0f;
    if (SUCCEEDED(g_pDWrite->CreateTextLayout(
            txt.c_str(), static_cast<UINT32>(txt.size()), pFmt, 2048.0f, 128.0f, &pLay)) && pLay) {
        DWRITE_TEXT_METRICS m{};
        if (SUCCEEDED(pLay->GetMetrics(&m))) w = std::ceil(m.widthIncludingTrailingWhitespace);
        pLay->Release();
    }
    return w;
}

static void RenderCopyPasteAlert(bool rebuildGlass) {
    if (!g_alertHwnd || !IsWindow(g_alertHwnd)) return;

    float t = std::clamp(g_alertAnim, 0.0f, 1.0f);
    if (!g_alertTargetOpen && t <= 0.005f) {
        if (IsWindowVisible(g_alertHwnd)) ShowWindow(g_alertHwnd, SW_HIDE);
        return;
    }

    POINT ptRef{};
    GetCursorPos(&ptRef);
    HMONITOR hMon = MonitorFromPoint(ptRef, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    if (!hMon || !GetMonitorInfoW(hMon, &mi)) {
        hMon = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
        GetMonitorInfoW(hMon, &mi);
    }
    if (IsFullscreenAppActiveOnMonitor(hMon)) {
        if (IsWindowVisible(g_alertHwnd)) ShowWindow(g_alertHwnd, SW_HIDE);
        return;
    }

    UINT dpi = GetDpiForMonitorOrWindow(g_alertHwnd);
    if (!EnsureRenderResources(dpi)) return;

    float titleW = MeasureTextWidthPx(g_fonts.pTitle, g_cpAlert.title);
    float subW   = MeasureTextWidthPx(g_fonts.pSub, g_cpAlert.subtitle);
    float badgeW = !g_cpAlert.badge.empty() ? (MeasureTextWidthPx(g_fonts.pTab, g_cpAlert.badge) + DpiScaleF(18.0f, dpi)) : 0.0f;
    float textColW = std::min(DpiScaleF(210.0f, dpi), std::max(titleW, subW));

    int cw = std::clamp<int>(static_cast<int>(std::ceil(DpiScaleF(58.0f, dpi) + textColW +
                                                        (badgeW > 0.0f ? (badgeW + DpiScaleF(14.0f, dpi)) : DpiScaleF(12.0f, dpi)))),
                             DpiScale(196, dpi), DpiScale(380, dpi));
    int ch = DpiScale(42, dpi);

    int workL = static_cast<int>(mi.rcWork.left);
    int workT = static_cast<int>(mi.rcWork.top);
    int workR = static_cast<int>(mi.rcWork.right);
    int workB = static_cast<int>(mi.rcWork.bottom);
    int x = workL + (workR - workL - cw) / 2;

    bool placeAtTop = false;
    if (g_settings.alertIslandPosition == AlertIslandPosition::Top) {
        placeAtTop = true;
    } else if (g_settings.alertIslandPosition == AlertIslandPosition::Bottom) {
        placeAtTop = false;
    } else {
        // Auto: Top taskbar -> Top (below taskbar); Bottom / Left / Right taskbar -> Bottom (above taskbar)
        TaskbarEdge tbEdge = DetectTaskbarEdgeOnMonitor(hMon, mi);
        placeAtTop = (tbEdge == TaskbarEdge::Top);
    }

    int y = placeAtTop
        ? (workT + DpiScale(8, dpi))
        : (workB - ch - DpiScale(10, dpi));

    // Only nudge slightly below the slim Top-Center Drag & Drop Mini Bar if it is actively open
    // (never push the alert into the center of the screen because of the tall Shelf window!)
    if (placeAtTop && g_pillTargetOpen && g_pillHwnd && IsWindowVisible(g_pillHwnd)) {
        int pillBot = static_cast<int>(g_pillScreenRect.bottom);
        if (pillBot > y && pillBot < workT + DpiScale(72, dpi)) {
            y = pillBot + DpiScale(6, dpi);
        }
    }

    RECT rcTarget = { x, y, x + cw, y + ch };
    if (!g_alertCanvas.Ensure(cw, ch)) return;

    bool isWinTheme = (g_settings.uiStyle == UiStyle::Windows);
    float rad = isWinTheme ? DpiScaleF(10.0f, dpi) : (ch * 0.5f);

    if (rebuildGlass || !g_alertGlassCache.valid ||
        g_alertGlassCache.w != cw || g_alertGlassCache.h != ch ||
        !EqualRect(&rcTarget, &g_alertGlassCache.screenRect)) {
        g_alertScreenRect = rcTarget;
        BuildFrostedGlassForSlot(g_alertGlassCache, rcTarget, rad, dpi, g_settings, true);
    }

    bool isLight = g_alertGlassCache.valid ? g_alertGlassCache.isLight : g_systemLightTheme;
    D2D1_COLOR_F colAccent = g_cpAlert.isCut
        ? (isLight ? D2D1::ColorF(0.84f, 0.42f, 0.02f, 1.0f) : D2D1::ColorF(1.00f, 0.68f, 0.24f, 1.0f))
        : (g_cpAlert.isPaste
            ? (isLight ? D2D1::ColorF(0.06f, 0.54f, 0.28f, 1.0f) : D2D1::ColorF(0.28f, 0.90f, 0.54f, 1.0f))
            : ResolveAccentColor(isLight));
    D2D1_COLOR_F colText = isLight ? D2D1::ColorF(0.08f, 0.09f, 0.12f, 1.0f)
                                   : D2D1::ColorF(0.96f, 0.97f, 1.00f, 1.0f);
    D2D1_COLOR_F colSub  = isLight ? D2D1::ColorF(0.32f, 0.35f, 0.42f, 1.0f)
                                   : D2D1::ColorF(0.74f, 0.78f, 0.86f, 1.0f);

    float inv = 1.0f - t;
    float ease = 1.0f - inv * inv * inv;
    float drawW = cw * (0.88f + 0.12f * ease);
    float drawH = ch * (0.88f + 0.12f * ease);
    float ox = (cw - drawW) * 0.5f;
    float oy = (ch - drawH) * 0.5f;

    RECT rcBind = { 0, 0, cw, ch };
    if (SUCCEEDED(g_pDCRT->BindDC(g_alertCanvas.hdcMem, &rcBind))) {
        g_pDCRT->BeginDraw();
        g_pDCRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        g_pDCRT->Clear(D2D1::ColorF(0, 0, 0, 0));

        D2D1_RECT_F rBox = D2D1::RectF(ox + 0.6f, oy + 0.6f, ox + drawW - 0.6f, oy + drawH - 0.6f);
        float drawRad = std::min<float>(rad, drawH * 0.5f);
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rBox, drawRad, drawRad);

        if (g_settings.glassStyle != GlassStyle::SolidSoft &&
            EnsureGlassBrushForSlot(g_alertGlassCache, g_pDCRT) && g_alertGlassCache.pBrush) {
            g_alertGlassCache.pBrush->SetTransform(D2D1::Matrix3x2F::Identity());
            g_pDCRT->FillRoundedRectangle(rr, g_alertGlassCache.pBrush);
        } else {
            g_pBrush->SetColor(isLight ? D2D1::ColorF(0.965f, 0.97f, 0.985f, 0.96f)
                                       : D2D1::ColorF(0.12f, 0.125f, 0.15f, 0.95f));
            g_pDCRT->FillRoundedRectangle(rr, g_pBrush);
        }

        g_pBrush->SetColor(isLight ? D2D1::ColorF(0, 0, 0, 0.14f) : D2D1::ColorF(1, 1, 1, 0.18f));
        g_pDCRT->DrawRoundedRectangle(rr, g_pBrush, DpiScaleF(1.0f, dpi));

        // Icon squircle
        float icSz = DpiScaleF(24.0f, dpi);
        D2D1_RECT_F rIc = D2D1::RectF(rBox.left + DpiScaleF(10.0f, dpi),
                                      rBox.top + (drawH - icSz) * 0.5f,
                                      rBox.left + DpiScaleF(10.0f, dpi) + icSz,
                                      rBox.top + (drawH + icSz) * 0.5f);
        g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, 0.18f));
        g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rIc, DpiScaleF(7.0f, dpi), DpiScaleF(7.0f, dpi)), g_pBrush);
        g_pBrush->SetColor(colAccent);
        if (g_fonts.pSmallIcon && !g_cpAlert.icon.empty()) {
            g_pDCRT->DrawText(g_cpAlert.icon.c_str(), static_cast<UINT32>(g_cpAlert.icon.size()),
                              g_fonts.pSmallIcon, rIc, g_pBrush);
        }

        // Right status badge
        float rightLim = rBox.right - DpiScaleF(10.0f, dpi);
        if (!g_cpAlert.badge.empty() && g_fonts.pTab) {
            float bW = std::min(badgeW, drawW * 0.36f);
            float bH = DpiScaleF(20.0f, dpi);
            D2D1_RECT_F rB = D2D1::RectF(rightLim - bW, rBox.top + (drawH - bH) * 0.5f, rightLim, rBox.top + (drawH + bH) * 0.5f);
            g_pBrush->SetColor(D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, 0.16f));
            g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rB, bH * 0.5f, bH * 0.5f), g_pBrush);
            g_pBrush->SetColor(colAccent);
            g_pDCRT->DrawText(g_cpAlert.badge.c_str(), static_cast<UINT32>(g_cpAlert.badge.size()),
                              g_fonts.pTab, rB, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            rightLim = rB.left - DpiScaleF(8.0f, dpi);
        }

        float txtL = rIc.right + DpiScaleF(9.0f, dpi);
        if (!g_cpAlert.subtitle.empty()) {
            float midY = rBox.top + drawH * 0.5f;
            g_pBrush->SetColor(colText);
            if (g_fonts.pTitle) {
                g_pDCRT->DrawText(g_cpAlert.title.c_str(), static_cast<UINT32>(g_cpAlert.title.size()), g_fonts.pTitle,
                                  D2D1::RectF(txtL, rBox.top + DpiScaleF(3.0f, dpi), rightLim, midY + DpiScaleF(1.0f, dpi)),
                                  g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }
            g_pBrush->SetColor(colSub);
            if (g_fonts.pSub) {
                g_pDCRT->DrawText(g_cpAlert.subtitle.c_str(), static_cast<UINT32>(g_cpAlert.subtitle.size()), g_fonts.pSub,
                                  D2D1::RectF(txtL, midY, rightLim, rBox.bottom - DpiScaleF(3.0f, dpi)),
                                  g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }
        } else {
            g_pBrush->SetColor(colText);
            if (g_fonts.pTitle) {
                g_pDCRT->DrawText(g_cpAlert.title.c_str(), static_cast<UINT32>(g_cpAlert.title.size()), g_fonts.pTitle,
                                  D2D1::RectF(txtL, rBox.top, rightLim, rBox.bottom),
                                  g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }
        }

        HRESULT hr = g_pDCRT->EndDraw();
        if (SUCCEEDED(hr)) {
            int slideY = static_cast<int>(std::lround((1.0f - ease) * DpiScaleF(10.0f, dpi) * (placeAtTop ? -1.0f : 1.0f)));
            int dstX = rcTarget.left;
            int dstY = rcTarget.top + slideY;
            BYTE alpha = static_cast<BYTE>(std::clamp<int>(static_cast<int>(std::lround(ease * 255.0f)), 1, 255));
            HDC hdcScr = GetDC(nullptr);
            POINT ptSrc = { 0, 0 };
            POINT ptDst = { dstX, dstY };
            SIZE  szDst = { cw, ch };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
            UpdateLayeredWindow(g_alertHwnd, hdcScr, &ptDst, &szDst, g_alertCanvas.hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);
            ReleaseDC(nullptr, hdcScr);
            if (!IsWindowVisible(g_alertHwnd)) {
                SetWindowPos(g_alertHwnd, HWND_TOPMOST, dstX, dstY, cw, ch,
                             SWP_NOACTIVATE | SWP_SHOWWINDOW);
            } else {
                SetWindowPos(g_alertHwnd, HWND_TOPMOST, dstX, dstY, cw, ch,
                             SWP_NOACTIVATE | SWP_NOZORDER);
            }
        }
    }
}

static void TriggerCopyPasteAlert(const std::wstring& icon, const std::wstring& title,
                                  const std::wstring& subtitle, const std::wstring& badge,
                                  bool isCut, bool isPaste) {
    if (!g_settings.enableCopyPasteAlert || !g_alertHwnd || !IsWindow(g_alertHwnd)) return;
    g_cpAlert.icon = icon;
    g_cpAlert.title = title;
    g_cpAlert.subtitle = subtitle;
    g_cpAlert.badge = badge;
    g_cpAlert.isCut = isCut;
    g_cpAlert.isPaste = isPaste;
    g_cpAlert.expireTick = GetTickCount64() + 1400ULL;

    g_alertTargetOpen = true;
    if (g_alertAnim < 0.15f) g_alertAnim = 0.15f;
    RenderCopyPasteAlert(true);
    SetTimer(g_alertHwnd, IDT_ALERT_EXPIRE, 1420, nullptr);
    if (!g_alertAnimActive) {
        g_alertLastAnimTick = GetTickCount64();
        g_alertAnimActive = true;
        SetTimer(g_alertHwnd, IDT_ALERT_ANIM, 12, nullptr);
    }
}

static void InspectClipboardForAlert(bool isPaste) {
    if (!g_settings.enableCopyPasteAlert) return;
    EnsureRegisteredFormats();
    bool vi = ComputeUseVietnamese(g_settings.lang);
    DWORD curSeq = GetClipboardSequenceNumber();

    // Never call OpenClipboard on Paste so we never block the target application!
    if (isPaste) {
        if (g_lastClipSummary.valid && g_lastClipSummary.seq == curSeq) {
            const wchar_t* title =
                (g_lastClipSummary.kind == 1) ? PickText(vi, L"Đã dán tệp", L"Pasted Files") :
                (g_lastClipSummary.kind == 2) ? PickText(vi, L"Đã dán văn bản", L"Pasted Text") :
                                                PickText(vi, L"Đã dán hình ảnh", L"Pasted Image");
            TriggerCopyPasteAlert(L"\uE77F", title, g_lastClipSummary.subtitle, g_lastClipSummary.badge, false, true);
            return;
        }
        if (IsClipboardFormatAvailable(CF_HDROP)) {
            TriggerCopyPasteAlert(L"\uE77F", PickText(vi, L"Đã dán tệp", L"Pasted Files"), L"",
                                  PickText(vi, L"Tệp", L"Files"), false, true);
        } else if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
            TriggerCopyPasteAlert(L"\uE77F", PickText(vi, L"Đã dán văn bản", L"Pasted Text"), L"",
                                  PickText(vi, L"Văn bản", L"Text"), false, true);
        } else if (IsClipboardFormatAvailable(CF_DIB) || IsClipboardFormatAvailable(CF_DIBV5) ||
                   (g_cfPng && IsClipboardFormatAvailable(g_cfPng))) {
            TriggerCopyPasteAlert(L"\uE77F", PickText(vi, L"Đã dán hình ảnh", L"Pasted Image"), L"",
                                  PickText(vi, L"Ảnh", L"Image"), false, true);
        }
        return;
    }

    if (!OpenClipboard(g_shelfHwnd)) return;

    bool isProtected = IsClipboardContentProtected();
    ULONGLONG cutTick = g_lastCutKeyTick.exchange(0, std::memory_order_relaxed);
    bool isCut = (cutTick > 0 && (GetTickCount64() - cutTick <= 650));
    if (g_cfDropEffect && IsClipboardFormatAvailable(g_cfDropEffect)) {
        if (HANDLE hDe = GetClipboardData(g_cfDropEffect)) {
            if (const DWORD* pEff = static_cast<const DWORD*>(GlobalLock(hDe))) {
                if ((*pEff & 2 /*DROPEFFECT_MOVE*/) != 0 && (*pEff & 1 /*DROPEFFECT_COPY*/) == 0) isCut = true;
                GlobalUnlock(hDe);
            }
        }
    }

    const wchar_t* icon = isCut ? L"\uE8C6" : L"\uE8C8";

    if (IsClipboardFormatAvailable(CF_HDROP)) {
        if (HANDLE hMem = GetClipboardData(CF_HDROP)) {
            if (HDROP hDrop = static_cast<HDROP>(GlobalLock(hMem))) {
                UINT cnt = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
                if (cnt > 0) {
                    wchar_t fp[MAX_PATH] = {};
                    DragQueryFileW(hDrop, 0, fp, MAX_PATH);
                    std::wstring name = PathFindFileNameW(fp);
                    std::wstring sub = (cnt == 1) ? name : (name + L" +" + std::to_wstring(cnt - 1));
                    std::wstring badge = std::to_wstring(cnt) + PickText(vi, L" tệp", cnt == 1 ? L" file" : L" files");
                    GlobalUnlock(hMem);
                    CloseClipboard();
                    g_lastClipSummary = { curSeq, 1, sub, badge, true };
                    TriggerCopyPasteAlert(icon, isCut ? PickText(vi, L"Đã cắt tệp", L"Cut Files")
                                                      : PickText(vi, L"Đã sao chép tệp", L"Copied Files"),
                                          sub, badge, isCut, false);
                    return;
                }
                GlobalUnlock(hMem);
            }
        }
    }

    if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        if (HANDLE hTxt = GetClipboardData(CF_UNICODETEXT)) {
            SIZE_T cap = GlobalSize(hTxt);
            size_t maxW = (cap >= sizeof(wchar_t)) ? (cap / sizeof(wchar_t)) : 0;
            if (const wchar_t* pStr = static_cast<const wchar_t*>(GlobalLock(hTxt))) {
                size_t len = wcsnlen(pStr, maxW);
                std::wstring preview = isProtected
                    ? PickText(vi, L"Nội dung được bảo mật", L"Protected content")
                    : MakeCleanSingleLine(std::wstring(pStr, std::min<size_t>(len, 128)), 34);
                GlobalUnlock(hTxt);
                CloseClipboard();
                if (len > 0 || isProtected) {
                    std::wstring badge = isProtected
                        ? PickText(vi, L"Bảo mật", L"Private")
                        : (std::to_wstring(len) + PickText(vi, L" ký tự", L" chars"));
                    g_lastClipSummary = { curSeq, 2, preview, badge, true };
                    TriggerCopyPasteAlert(icon, isCut ? PickText(vi, L"Đã cắt văn bản", L"Cut Text")
                                                      : PickText(vi, L"Đã sao chép văn bản", L"Copied Text"),
                                          preview, badge, isCut, false);
                }
                return;
            }
        }
    }

    bool hasImg = IsClipboardFormatAvailable(CF_DIB) || IsClipboardFormatAvailable(CF_DIBV5) ||
                  (g_cfPng && IsClipboardFormatAvailable(g_cfPng));
    if (hasImg) {
        std::wstring dim;
        if (IsClipboardFormatAvailable(CF_DIB)) {
            if (HANDLE hDib = GetClipboardData(CF_DIB)) {
                if (const auto* bih = static_cast<const BITMAPINFOHEADER*>(GlobalLock(hDib))) {
                    int w = std::abs(bih->biWidth), h = std::abs(bih->biHeight);
                    if (w > 0 && h > 0 && w <= 32768 && h <= 32768) {
                        dim = std::to_wstring(w) + L" × " + std::to_wstring(h);
                    }
                    GlobalUnlock(hDib);
                }
            }
        }
        CloseClipboard();
        std::wstring badge = dim.empty() ? PickText(vi, L"Ảnh", L"Image") : dim;
        std::wstring sub   = dim.empty() ? PickText(vi, L"Ảnh trong bảng tạm", L"Clipboard image") : L"";
        g_lastClipSummary = { curSeq, 3, sub, badge, true };
        TriggerCopyPasteAlert(isCut ? L"\uE8C6" : L"\uEB9F",
                              isCut ? PickText(vi, L"Đã cắt hình ảnh", L"Cut Image")
                                    : PickText(vi, L"Đã sao chép hình ảnh", L"Copied Image"),
                              sub, badge, isCut, false);
        return;
    }
    CloseClipboard();
}

static LRESULT CALLBACK CopyPasteAlertWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER) {
        if (wParam == IDT_ALERT_EXPIRE) {
            KillTimer(hwnd, IDT_ALERT_EXPIRE);
            g_alertTargetOpen = false;
            if (!g_alertAnimActive) {
                g_alertLastAnimTick = GetTickCount64();
                g_alertAnimActive = true;
                SetTimer(hwnd, IDT_ALERT_ANIM, 12, nullptr);
            }
            return 0;
        }
        if (wParam == IDT_ALERT_ANIM) {
            ULONGLONG now = GetTickCount64();
            float dt = (g_alertLastAnimTick > 0 && now > g_alertLastAnimTick)
                ? std::clamp(static_cast<float>(now - g_alertLastAnimTick) / 1000.0f, 0.001f, 0.055f)
                : 0.012f;
            g_alertLastAnimTick = now;

            if (g_alertTargetOpen) g_alertAnim = std::min(1.0f, g_alertAnim + dt / 0.130f);
            else                   g_alertAnim = std::max(0.0f, g_alertAnim - dt / 0.130f);

            RenderCopyPasteAlert(false);

            bool done = g_alertTargetOpen ? (g_alertAnim >= 0.999f) : (g_alertAnim <= 0.001f);
            if (done) {
                KillTimer(hwnd, IDT_ALERT_ANIM);
                g_alertAnimActive = false;
            }
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


// ---------------------------------------------------------------------------
// 9c. Quick Look Image Preview Popup (Compact + Fullscreen Lightbox Mode)
// ---------------------------------------------------------------------------
static std::shared_ptr<DecodedBgraImage> g_previewImage;
static std::wstring                      g_previewCaption;
static bool                              g_previewFullscreen = false;
static int                               g_previewHoverBtn   = 0; // 1=fullscreen toggle, 2=close
static D2D1_RECT_F                       g_previewBtnFs{};
static D2D1_RECT_F                       g_previewBtnClose{};
static D2D1_RECT_F                       g_previewImgRect{};

static void CloseImagePreviewPopup() {
    if (g_previewHwnd && IsWindowVisible(g_previewHwnd)) {
        ShowWindow(g_previewHwnd, SW_HIDE);
    }
    g_previewFullscreen = false;
    g_previewHoverBtn = 0;
    g_previewImage.reset();
    g_previewCaption.clear();
}

static void RenderImagePreviewPopup() {
    if (!g_previewHwnd || !IsWindow(g_previewHwnd) || !g_previewImage || g_previewImage->pixels.empty()) return;

    UINT dpi = GetDpiForMonitorOrWindow(g_previewHwnd);
    if (!EnsureRenderResources(dpi)) return;

    POINT ptCenter = { (g_shelfScreenRect.left + g_shelfScreenRect.right) / 2,
                       (g_shelfScreenRect.top + g_shelfScreenRect.bottom) / 2 };
    if (ptCenter.x == 0 && ptCenter.y == 0) GetCursorPos(&ptCenter);
    HMONITOR hMon = MonitorFromPoint(ptCenter, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    if (!hMon || !GetMonitorInfoW(hMon, &mi)) {
        hMon = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
        GetMonitorInfoW(hMon, &mi);
    }

    int monW = static_cast<int>(mi.rcMonitor.right - mi.rcMonitor.left);
    int monH = static_cast<int>(mi.rcMonitor.bottom - mi.rcMonitor.top);

    int cw = 0, ch = 0, px = 0, py = 0;
    int drawW = 0, drawH = 0;
    int pad = DpiScale(12, dpi);
    int capH = DpiScale(30, dpi);

    if (g_previewFullscreen) {
        cw = monW;
        ch = monH;
        px = static_cast<int>(mi.rcMonitor.left);
        py = static_cast<int>(mi.rcMonitor.top);
        int maxW = std::max<int>(DpiScale(240, dpi), cw - DpiScale(84, dpi));
        int maxH = std::max<int>(DpiScale(180, dpi), ch - DpiScale(116, dpi));
        double sc = std::min<double>({ 1.0, static_cast<double>(maxW) / g_previewImage->width,
                                       static_cast<double>(maxH) / g_previewImage->height });
        drawW = std::max<int>(DpiScale(120, dpi), static_cast<int>(std::lround(g_previewImage->width * sc)));
        drawH = std::max<int>(DpiScale(90, dpi),  static_cast<int>(std::lround(g_previewImage->height * sc)));
    } else {
        int maxImgW = DpiScale(480, dpi);
        int maxImgH = DpiScale(360, dpi);
        double sc = std::min<double>({ 1.0, static_cast<double>(maxImgW) / g_previewImage->width,
                                       static_cast<double>(maxImgH) / g_previewImage->height });
        drawW = std::max<int>(DpiScale(160, dpi), static_cast<int>(std::lround(g_previewImage->width * sc)));
        drawH = std::max<int>(DpiScale(100, dpi), static_cast<int>(std::lround(g_previewImage->height * sc)));
        cw = drawW + pad * 2;
        ch = drawH + pad * 2 + capH;

        px = static_cast<int>(g_shelfScreenRect.right) + DpiScale(10, dpi);
        if (px + cw > static_cast<int>(mi.rcWork.right) - DpiScale(8, dpi)) {
            px = static_cast<int>(g_shelfScreenRect.left) - cw - DpiScale(10, dpi);
        }
        px = std::clamp<int>(px, static_cast<int>(mi.rcWork.left) + DpiScale(8, dpi),
                             std::max<int>(static_cast<int>(mi.rcWork.left) + DpiScale(8, dpi),
                                           static_cast<int>(mi.rcWork.right) - cw - DpiScale(8, dpi)));
        py = std::clamp<int>(static_cast<int>(g_shelfScreenRect.top),
                             static_cast<int>(mi.rcWork.top) + DpiScale(8, dpi),
                             std::max<int>(static_cast<int>(mi.rcWork.top) + DpiScale(8, dpi),
                                           static_cast<int>(mi.rcWork.bottom) - ch - DpiScale(8, dpi)));
    }

    if (!g_previewCanvas.Ensure(cw, ch)) return;

    bool isLight = !g_previewFullscreen && (g_glassCache.valid ? g_glassCache.isLight : g_systemLightTheme);
    D2D1_COLOR_F colAccent = ResolveAccentColor(isLight);
    D2D1_COLOR_F colText   = isLight ? D2D1::ColorF(0.10f, 0.12f, 0.16f, 1.0f)
                                     : D2D1::ColorF(0.95f, 0.97f, 1.00f, 1.0f);
    D2D1_COLOR_F colBtnBg  = isLight ? D2D1::ColorF(0, 0, 0, 0.08f) : D2D1::ColorF(1, 1, 1, 0.14f);
    D2D1_COLOR_F colBtnHov = D2D1::ColorF(colAccent.r, colAccent.g, colAccent.b, 0.28f);

    g_previewBtnFs = g_previewBtnClose = g_previewImgRect = {};

    RECT rcBind = { 0, 0, cw, ch };
    if (SUCCEEDED(g_pDCRT->BindDC(g_previewCanvas.hdcMem, &rcBind))) {
        g_pDCRT->BeginDraw();
        g_pDCRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        g_pDCRT->Clear(D2D1::ColorF(0, 0, 0, 0));

        if (g_previewFullscreen) {
            // Fullscreen dark cinema lightbox scrim
            g_pBrush->SetColor(D2D1::ColorF(0.04f, 0.045f, 0.06f, 0.86f));
            g_pDCRT->FillRectangle(D2D1::RectF(0, 0, static_cast<float>(cw), static_cast<float>(ch)), g_pBrush);

            float imgL = (cw - drawW) * 0.5f;
            float imgT = (ch - drawH - DpiScaleF(24.0f, dpi)) * 0.5f;
            g_previewImgRect = D2D1::RectF(imgL, imgT, imgL + drawW, imgT + drawH);

            // Top-right floating Fullscreen Exit & Close pills
            float btnSz = DpiScaleF(34.0f, dpi);
            float gap   = DpiScaleF(8.0f, dpi);
            float topY  = DpiScaleF(18.0f, dpi);
            g_previewBtnClose = D2D1::RectF(cw - DpiScaleF(20.0f, dpi) - btnSz, topY,
                                            cw - DpiScaleF(20.0f, dpi), topY + btnSz);
            g_previewBtnFs    = D2D1::RectF(g_previewBtnClose.left - gap - btnSz, topY,
                                            g_previewBtnClose.left - gap, topY + btnSz);

            auto drawTopBtn = [&](const D2D1_RECT_F& rB, const wchar_t* glyph, bool hov, bool danger) {
                g_pBrush->SetColor((danger && hov) ? D2D1::ColorF(0.82f, 0.18f, 0.15f, 0.92f)
                                                   : (hov ? colBtnHov : D2D1::ColorF(0.16f, 0.17f, 0.21f, 0.82f)));
                g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rB, btnSz * 0.5f, btnSz * 0.5f), g_pBrush);
                g_pBrush->SetColor(D2D1::ColorF(1, 1, 1, 0.25f));
                g_pDCRT->DrawRoundedRectangle(D2D1::RoundedRect(rB, btnSz * 0.5f, btnSz * 0.5f), g_pBrush, 1.0f);
                g_pBrush->SetColor(D2D1::ColorF(1, 1, 1, 0.96f));
                if (g_fonts.pSmallIcon) g_pDCRT->DrawText(glyph, 1, g_fonts.pSmallIcon, rB, g_pBrush);
            };
            drawTopBtn(g_previewBtnFs,    L"\uE73F", g_previewHoverBtn == 1, false);
            drawTopBtn(g_previewBtnClose, L"\uE711", g_previewHoverBtn == 2, true);
        } else {
            float rad = DpiScaleF(g_settings.uiStyle == UiStyle::Windows ? 10.0f : 16.0f, dpi);
            D2D1_RECT_F rCard = D2D1::RectF(0.6f, 0.6f, cw - 0.6f, ch - 0.6f);
            D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rCard, rad, rad);
            g_pBrush->SetColor(isLight ? D2D1::ColorF(0.965f, 0.97f, 0.985f, 0.96f)
                                       : D2D1::ColorF(0.11f, 0.115f, 0.14f, 0.96f));
            g_pDCRT->FillRoundedRectangle(rr, g_pBrush);
            g_pBrush->SetColor(isLight ? D2D1::ColorF(0, 0, 0, 0.18f) : D2D1::ColorF(1, 1, 1, 0.20f));
            g_pDCRT->DrawRoundedRectangle(rr, g_pBrush, DpiScaleF(1.0f, dpi));

            g_previewImgRect = D2D1::RectF(static_cast<float>(pad), static_cast<float>(pad),
                                           static_cast<float>(pad + drawW), static_cast<float>(pad + drawH));

            float btnSz = DpiScaleF(22.0f, dpi);
            float gap   = DpiScaleF(5.0f, dpi);
            float barT  = static_cast<float>(pad + drawH) + (capH - btnSz) * 0.5f + DpiScaleF(2.0f, dpi);
            g_previewBtnClose = D2D1::RectF(cw - pad - btnSz, barT, static_cast<float>(cw - pad), barT + btnSz);
            g_previewBtnFs    = D2D1::RectF(g_previewBtnClose.left - gap - btnSz, barT,
                                            g_previewBtnClose.left - gap, barT + btnSz);

            auto drawBarBtn = [&](const D2D1_RECT_F& rB, const wchar_t* glyph, bool hov, bool danger) {
                g_pBrush->SetColor((danger && hov) ? D2D1::ColorF(0.82f, 0.18f, 0.15f, 0.92f)
                                                   : (hov ? colBtnHov : colBtnBg));
                g_pDCRT->FillRoundedRectangle(D2D1::RoundedRect(rB, DpiScaleF(6.0f, dpi), DpiScaleF(6.0f, dpi)), g_pBrush);
                g_pBrush->SetColor((danger && hov) ? D2D1::ColorF(1, 1, 1, 1) : (hov ? colAccent : colText));
                if (g_fonts.pSmallIcon) g_pDCRT->DrawText(glyph, 1, g_fonts.pSmallIcon, rB, g_pBrush);
            };
            drawBarBtn(g_previewBtnFs,    L"\uE740", g_previewHoverBtn == 1, false);
            drawBarBtn(g_previewBtnClose, L"\uE711", g_previewHoverBtn == 2, true);
        }

        // Draw high-res image
        ID2D1Bitmap* pBmp = nullptr;
        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0f, 96.0f);
        if (SUCCEEDED(g_pDCRT->CreateBitmap(
                D2D1::SizeU(g_previewImage->width, g_previewImage->height),
                g_previewImage->pixels.data(), g_previewImage->width * 4, &props, &pBmp)) && pBmp) {
            g_pDCRT->DrawBitmap(pBmp, g_previewImgRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            pBmp->Release();
        }
        g_pBrush->SetColor(D2D1::ColorF(1, 1, 1, g_previewFullscreen ? 0.18f : 0.10f));
        g_pDCRT->DrawRectangle(g_previewImgRect, g_pBrush, 1.0f);

        // Caption bar
        g_pBrush->SetColor(colText);
        if (g_fonts.pHeader) {
            if (g_previewFullscreen) {
                D2D1_RECT_F rCap = D2D1::RectF(DpiScaleF(24.0f, dpi), g_previewImgRect.bottom + DpiScaleF(10.0f, dpi),
                                               cw - DpiScaleF(24.0f, dpi), static_cast<float>(ch - DpiScale(12, dpi)));
                g_fonts.pHeader->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                g_pDCRT->DrawText(g_previewCaption.c_str(), static_cast<UINT32>(g_previewCaption.size()),
                                  g_fonts.pHeader, rCap, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
                g_fonts.pHeader->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            } else {
                D2D1_RECT_F rCap = D2D1::RectF(static_cast<float>(pad), static_cast<float>(pad + drawH + DpiScale(4, dpi)),
                                               g_previewBtnFs.left - DpiScaleF(8.0f, dpi), static_cast<float>(ch - DpiScale(4, dpi)));
                g_pDCRT->DrawText(g_previewCaption.c_str(), static_cast<UINT32>(g_previewCaption.size()),
                                  g_fonts.pHeader, rCap, g_pBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            }
        }

        if (SUCCEEDED(g_pDCRT->EndDraw())) {
            HDC hdcScr = GetDC(nullptr);
            POINT ptSrc = { 0, 0 };
            POINT ptDst = { px, py };
            SIZE  szDst = { cw, ch };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            UpdateLayeredWindow(g_previewHwnd, hdcScr, &ptDst, &szDst, g_previewCanvas.hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);
            ReleaseDC(nullptr, hdcScr);
            SetWindowPos(g_previewHwnd, HWND_TOPMOST, px, py, cw, ch, SWP_SHOWWINDOW);
            SetForegroundWindow(g_previewHwnd);
        }
    }
}

static void ShowImagePreviewPopup(const StagedItem& item) {
    if (!g_previewHwnd || !IsWindow(g_previewHwnd) || item.pathOrText.empty()) return;

    // Decode at full monitor resolution so fullscreen view stays razor-sharp!
    POINT ptCur{};
    GetCursorPos(&ptCur);
    HMONITOR hMon = MonitorFromPoint(ptCur, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    UINT maxEdge = 1920;
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        maxEdge = static_cast<UINT>(std::max<LONG>(mi.rcMonitor.right - mi.rcMonitor.left,
                                                   mi.rcMonitor.bottom - mi.rcMonitor.top));
        maxEdge = std::clamp<UINT>(maxEdge, 1280u, 3840u);
    }

    UINT origW = 0, origH = 0;
    auto img = DecodeFileThumbnailWic(item.pathOrText.c_str(), maxEdge, &origW, &origH);
    if (!img || img->width == 0 || img->height == 0 || img->pixels.empty()) {
        ShellExecuteW(nullptr, L"open", item.pathOrText.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }
    g_previewImage = std::move(img);
    g_previewCaption = item.displayName;
    if (origW > 0 && origH > 0) {
        g_previewCaption += L"  ·  " + std::to_wstring(origW) + L"×" + std::to_wstring(origH);
    }
    g_previewFullscreen = false;
    g_previewHoverBtn = 0;
    RenderImagePreviewPopup();
}

static LRESULT CALLBACK ImagePreviewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && !g_previewFullscreen) CloseImagePreviewPopup();
            return 0;

        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, g_previewHoverBtn != 0 ? IDC_HAND : IDC_ARROW));
            return TRUE;

        case WM_MOUSEMOVE: {
            float fx = static_cast<float>(GET_X_LPARAM(lParam));
            float fy = static_cast<float>(GET_Y_LPARAM(lParam));
            int hov = 0;
            if (PtInRectF(fx, fy, g_previewBtnFs))         hov = 1;
            else if (PtInRectF(fx, fy, g_previewBtnClose)) hov = 2;
            if (hov != g_previewHoverBtn) {
                g_previewHoverBtn = hov;
                RenderImagePreviewPopup();
            }
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                if (g_previewFullscreen) {
                    g_previewFullscreen = false;
                    RenderImagePreviewPopup();
                } else {
                    CloseImagePreviewPopup();
                }
            } else if (wParam == VK_SPACE) {
                CloseImagePreviewPopup();
            } else if (wParam == 'F' || wParam == VK_F11) {
                g_previewFullscreen = !g_previewFullscreen;
                RenderImagePreviewPopup();
            }
            return 0;

        case WM_LBUTTONDBLCLK: {
            float fx = static_cast<float>(GET_X_LPARAM(lParam));
            float fy = static_cast<float>(GET_Y_LPARAM(lParam));
            if (PtInRectF(fx, fy, g_previewImgRect)) {
                g_previewFullscreen = !g_previewFullscreen;
                RenderImagePreviewPopup();
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            float fx = static_cast<float>(GET_X_LPARAM(lParam));
            float fy = static_cast<float>(GET_Y_LPARAM(lParam));
            if (PtInRectF(fx, fy, g_previewBtnFs)) {
                g_previewFullscreen = !g_previewFullscreen;
                RenderImagePreviewPopup();
            } else if (PtInRectF(fx, fy, g_previewBtnClose)) {
                CloseImagePreviewPopup();
            } else if (g_previewFullscreen && !PtInRectF(fx, fy, g_previewImgRect)) {
                CloseImagePreviewPopup();
            }
            return 0;
        }

        case WM_RBUTTONUP:
            CloseImagePreviewPopup();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// 10. Window Procedures (Taskbar Status Pill + Main Shelf Window)
// ---------------------------------------------------------------------------
static LRESULT CALLBACK TaskbarPillWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static bool  s_tracking = false;
    static bool  s_lDown    = false;
    static bool  s_didDrag  = false;
    static POINT s_downPt   = {};

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_HAND));
            return TRUE;

        case WM_TIMER:
            if (wParam == IDT_PILL_ANIM) {
                ULONGLONG now = GetTickCount64();
                float dt = (g_pillLastAnimTick > 0 && now > g_pillLastAnimTick)
                    ? std::clamp(static_cast<float>(now - g_pillLastAnimTick) / 1000.0f, 0.001f, 0.055f)
                    : 0.012f;
                g_pillLastAnimTick = now;

                if (g_pillTargetOpen) g_pillAnim = std::min(1.0f, g_pillAnim + dt / 0.170f);
                else                  g_pillAnim = std::max(0.0f, g_pillAnim - dt / 0.130f);

                float hovTarget = (g_pillHoverMouse || g_pillHoverDrop) ? 1.0f : 0.0f;
                if (g_pillHoverAnim < hovTarget) g_pillHoverAnim = std::min(hovTarget, g_pillHoverAnim + dt / 0.130f);
                else if (g_pillHoverAnim > hovTarget) g_pillHoverAnim = std::max(hovTarget, g_pillHoverAnim - dt / 0.140f);

                RenderTaskbarPill(false);

                bool openDone = g_pillTargetOpen ? (g_pillAnim >= 0.999f) : (g_pillAnim <= 0.001f);
                bool hovDone  = (std::fabs(g_pillHoverAnim - hovTarget) <= 0.002f);
                if (openDone && hovDone) {
                    KillTimer(hwnd, IDT_PILL_ANIM);
                    g_pillAnimActive = false;
                }
                return 0;
            }
            break;

        case WM_MOUSELEAVE:
            s_tracking = false;
            if (g_pillHoverMouse) {
                g_pillHoverMouse = false;
                StartPillAnim();
            }
            return 0;

        case WM_MOUSEMOVE: {
            if (!s_tracking) {
                TRACKMOUSEEVENT tme{ sizeof(tme), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
                s_tracking = true;
                g_pillHoverMouse = true;
                StartPillAnim();
            }
            if (s_lDown && !s_didDrag && !g_stagedItems.empty()) {
                int dx = std::abs(GET_X_LPARAM(lParam) - s_downPt.x);
                int dy = std::abs(GET_Y_LPARAM(lParam) - s_downPt.y);
                if (dx >= 5 || dy >= 5) {
                    s_lDown = false;
                    s_didDrag = true;
                    if (GetCapture() == hwnd) ReleaseCapture();
                    ExecuteDragOut(g_stagedItems, -1, true);
                    return 0;
                }
            }
            return 0;
        }

        case WM_LBUTTONDOWN:
            s_lDown = true;
            s_didDrag = false;
            s_downPt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            SetCapture(hwnd);
            return 0;

        case WM_LBUTTONUP:
            s_lDown = false;
            if (GetCapture() == hwnd) ReleaseCapture();
            if (!s_didDrag) {
                RestoreShelfFromTaskbar();
            }
            s_didDrag = false;
            return 0;

        case WM_RBUTTONUP:
            ClearVectorItems(g_stagedItems);
            g_shelfPinned = false;
            HideTopMiniBarAnimated();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK ShelfWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static bool  s_trackingMouse  = false;
    static bool  s_lDownDragOut   = false;
    static int   s_dragOutRowIdx  = -1;
    static bool  s_didDragOut     = false;
    static POINT s_downPt         = {};

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_SETCURSOR: {
            bool hand = (g_hoverHeaderBtn != 0 || g_hoverFilterIdx >= 0 || g_hoverRowIdx >= 0 || g_hoverRowAction >= 0 || g_hoverRowRemove >= 0);
            SetCursor(LoadCursorW(nullptr, g_movingShelf ? IDC_SIZEALL : (hand ? IDC_HAND : IDC_ARROW)));
            return TRUE;
        }

        case WM_CLIPBOARDUPDATE: {
            if ((g_settings.replaceWinV || g_settings.enableCopyPasteAlert) &&
                GetTickCount64() >= g_ignoreClipUntilTick) {
                SetTimer(hwnd, IDT_CLIP_DEBOUNCE, 30, nullptr);
            }
            return 0;
        }

        case WM_APP_PASTE_ALERT: {
            static ULONGLONG s_lastPasteAlertTick = 0;
            ULONGLONG now = GetTickCount64();
            if (g_settings.enableCopyPasteAlert && (now - s_lastPasteAlertTick >= 200)) {
                s_lastPasteAlertTick = now;
                InspectClipboardForAlert(true);
            }
            return 0;
        }

        case WM_APP_WINV_SUMMON: {
            if (!g_settings.replaceWinV) return 0;
            g_prevForegroundWnd = GetForegroundWindow();
            if (g_shelfTargetOpen && g_shelfVisible && g_activeTab == ActiveTab::ClipboardHistory) {
                HideShelfAnimated();
            } else {
                g_activeTab = ActiveTab::ClipboardHistory;
                g_scrollOffset = 0;
                POINT pt{};
                GetCursorPos(&pt);
                g_openedByHotkey = true;
                ShowShelfAnimated(true, pt);
            }
            return 0;
        }

        case WM_HOTKEY: {
            if (wParam == HOTKEY_TOGGLE_SHELF) {
                if (g_shelfTargetOpen && g_shelfVisible) {
                    HideShelfAnimated();
                } else {
                    g_prevForegroundWnd = GetForegroundWindow();
                    g_activeTab = ActiveTab::Shelf;
                    POINT pt{};
                    GetCursorPos(&pt);
                    g_openedByHotkey = true;
                    ShowShelfAnimated(g_settings.shelfPosition == ShelfPosition::NearCursor, pt);
                }
            }
            return 0;
        }

        case WM_APP_RELOAD_SETTINGS: {
            ShelfPosition prevPos = g_settings.shelfPosition;
            g_settingsDirty.store(false, std::memory_order_relaxed);
            g_settings = ReadSettingsFromWindhawk();
            if (g_settings.shelfPosition != prevPos) {
                g_hasCustomPos = false;
                g_hasLockedPos = false;
            }
            g_glassCache.ClearAll();
            g_pillGlassCache.ClearAll();
            g_alertGlassCache.ClearAll();
            UnregisterHotKey(hwnd, HOTKEY_TOGGLE_SHELF);
            if (g_settings.enableGlobalHotkey) {
                RegisterHotKey(hwnd, HOTKEY_TOGGLE_SHELF, MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, 'D');
            }
            if (!g_settings.replaceWinV) {
                g_activeTab = ActiveTab::Shelf;
            } else {
                while (static_cast<int>(g_clipHistory.size()) > g_settings.maxClipboardHistory) {
                    CleanupTempFileIfOwned(g_clipHistory.back());
                    g_clipHistory.pop_back();
                }
                if (g_clipHistory.empty()) {
                    CaptureClipboardIntoList(g_clipHistory, g_settings.maxClipboardHistory, true, false);
                }
            }
            if (g_shelfVisible) RenderShelfFrame(true);
            if (g_shelfMinimized) RenderTaskbarPill(true);
            return 0;
        }

        case WM_SETTINGCHANGE:
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED: {
            RefreshSystemThemeAndAccent();
            g_wallpaperCache.lastStatTick = 0;
            if (g_shelfVisible) RenderShelfFrame(true);
            if (g_shelfMinimized) RenderTaskbarPill(true);
            return 0;
        }

        case WM_MOUSELEAVE: {
            s_trackingMouse = false;
            if (g_hoverHeaderBtn != 0 || g_hoverRowIdx != -1 || g_hoverRowRemove != -1) {
                g_hoverHeaderBtn = 0;
                g_hoverRowIdx = -1;
                g_hoverRowRemove = -1;
                RenderShelfFrame(false);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!s_trackingMouse) {
                TRACKMOUSEEVENT tme{ sizeof(tme), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
                s_trackingMouse = true;
            }
            int mx = GET_X_LPARAM(lParam);
            int my = GET_Y_LPARAM(lParam);

            if (g_movingShelf) {
                POINT ptScr{};
                GetCursorPos(&ptScr);
                g_hasCustomPos = true;
                g_customTopLeft = { ptScr.x - g_moveDragOffset.x, ptScr.y - g_moveDragOffset.y };
                RenderShelfFrame(true);
                return 0;
            }

            auto& curList = GetActiveList();
            if (s_lDownDragOut && !curList.empty() && !g_draggingOut) {
                int dx = std::abs(mx - s_downPt.x);
                int dy = std::abs(my - s_downPt.y);
                if (dx >= 5 || dy >= 5) {
                    int dragIdx = s_dragOutRowIdx;
                    s_lDownDragOut = false;
                    s_didDragOut = true;
                    if (GetCapture() == hwnd) ReleaseCapture();
                    ExecuteDragOut(curList, dragIdx, g_activeTab == ActiveTab::Shelf);
                    return 0;
                }
            }

            float fx = static_cast<float>(mx);
            float fy = static_cast<float>(my);
            int hovBtn = 0;
            int hovFlt = -1;
            int hovRow = -1;
            int hovAct = -1;
            int hovRm  = -1;

            if (PtInRectF(fx, fy, g_rcBtnClose))           hovBtn = 1;
            else if (PtInRectF(fx, fy, g_rcBtnMin))        hovBtn = 7;
            else if (PtInRectF(fx, fy, g_rcBtnPin))        hovBtn = 2;
            else if (PtInRectF(fx, fy, g_rcBtnCopy))       hovBtn = 3;
            else if (PtInRectF(fx, fy, g_rcBtnReveal))     hovBtn = 4;
            else if (PtInRectF(fx, fy, g_rcBtnPaste))      hovBtn = 5;
            else if (PtInRectF(fx, fy, g_rcBtnClearHist))  hovBtn = 10;
            else if (PtInRectF(fx, fy, g_rcFooterDragAll)) hovBtn = 6;
            else if (PtInRectF(fx, fy, g_rcTabShelf))      hovBtn = 8;
            else if (PtInRectF(fx, fy, g_rcTabClip))       hovBtn = 9;
            else {
                for (int fi = 0; fi < 5; ++fi) {
                    if (PtInRectF(fx, fy, g_rcFilterChips[fi])) { hovFlt = fi; break; }
                }
                if (hovFlt < 0) {
                    for (const auto& rh : g_rowHits) {
                        if (PtInRectF(fx, fy, rh.rcRemove)) {
                            hovRow = rh.itemIndex;
                            hovRm  = rh.itemIndex;
                            break;
                        }
                        if (PtInRectF(fx, fy, rh.rcAction)) {
                            hovRow = rh.itemIndex;
                            hovAct = rh.itemIndex;
                            break;
                        }
                        if (PtInRectF(fx, fy, rh.rcRow)) {
                            hovRow = rh.itemIndex;
                            break;
                        }
                    }
                }
            }

            if (hovBtn != g_hoverHeaderBtn || hovFlt != g_hoverFilterIdx ||
                hovRow != g_hoverRowIdx || hovAct != g_hoverRowAction || hovRm != g_hoverRowRemove) {
                g_hoverHeaderBtn = hovBtn;
                g_hoverFilterIdx = hovFlt;
                g_hoverRowIdx    = hovRow;
                g_hoverRowAction = hovAct;
                g_hoverRowRemove = hovRm;
                RenderShelfFrame(false);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            float fx = static_cast<float>(GET_X_LPARAM(lParam));
            float fy = static_cast<float>(GET_Y_LPARAM(lParam));
            s_downPt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            s_lDownDragOut = false;
            s_didDragOut = false;
            g_movingShelf = false;

            if (PtInRectF(fx, fy, g_rcBtnClose)) {
                if (g_activeTab == ActiveTab::ClipboardHistory && g_settings.replaceWinV) {
                    HideShelfAnimated();
                } else {
                    ClearVectorItems(g_stagedItems);
                    g_shelfPinned = false;
                    g_hasCustomPos = false;
                    HideShelfAnimated();
                    HideTopMiniBarAnimated();
                }
                return 0;
            }
            if (PtInRectF(fx, fy, g_rcBtnMin)) {
                MinimizeShelfToTaskbar();
                return 0;
            }
            if (PtInRectF(fx, fy, g_rcBtnPin)) {
                g_shelfPinned = !g_shelfPinned;
                bool vi = ComputeUseVietnamese(g_settings.lang);
                SetShelfToast(g_shelfPinned
                    ? PickText(vi, L"Đã ghim kệ (giữ tệp)", L"Shelf Pinned")
                    : PickText(vi, L"Đã bỏ ghim kệ", L"Shelf Unpinned"));
                return 0;
            }
            if (PtInRectF(fx, fy, g_rcTabShelf)) {
                g_activeTab = ActiveTab::Shelf;
                g_scrollOffset = 0;
                RenderShelfFrame(true);
                return 0;
            }
            if (PtInRectF(fx, fy, g_rcTabClip)) {
                g_activeTab = ActiveTab::ClipboardHistory;
                g_scrollOffset = 0;
                RenderShelfFrame(true);
                return 0;
            }
            if (PtInRectF(fx, fy, g_rcBtnPaste)) {
                g_activeTab = ActiveTab::Shelf;
                CaptureClipboardIntoList(g_stagedItems, g_settings.maxStagedItems, false, true);
                return 0;
            }
            if (PtInRectF(fx, fy, g_rcBtnCopy)) {
                WriteItemsToClipboard(GetActiveList(), true);
                return 0;
            }
            if (PtInRectF(fx, fy, g_rcBtnReveal)) {
                const auto& curList = GetActiveList();
                if (!curList.empty()) RevealItemInExplorer(curList, 0);
                return 0;
            }
            if (PtInRectF(fx, fy, g_rcBtnClearHist)) {
                StopInlineAudioPlayback();
                bool vi = ComputeUseVietnamese(g_settings.lang);
                g_scrollOffset = 0;
                if (g_activeTab == ActiveTab::ClipboardHistory && g_settings.replaceWinV) {
                    ClearVectorItems(g_clipHistory);
                    SetShelfToast(PickText(vi, L"Đã xóa toàn bộ lịch sử", L"Clipboard history cleared"));
                } else {
                    ClearVectorItems(g_stagedItems);
                    HideTopMiniBarAnimated();
                    SetShelfToast(PickText(vi, L"Đã xóa toàn bộ kệ", L"Shelf cleared"));
                }
                RenderShelfFrame(true);
                return 0;
            }

            for (int fi = 0; fi < 5; ++fi) {
                if (PtInRectF(fx, fy, g_rcFilterChips[fi])) {
                    g_activeFilter = static_cast<CategoryFilter>(fi);
                    g_scrollOffset = 0;
                    RenderShelfFrame(true);
                    return 0;
                }
            }

            auto& curList = GetActiveList();
            for (const auto& rh : g_rowHits) {
                if (PtInRectF(fx, fy, rh.rcAction)) {
                    if (rh.itemIndex >= 0 && rh.itemIndex < static_cast<int>(curList.size())) {
                        const auto& it = curList[rh.itemIndex];
                        if (it.kind == StagedItemKind::ImageBitmap) {
                            ShowImagePreviewPopup(it);
                        } else if (it.kind == StagedItemKind::AudioFile) {
                            ToggleInlineAudioPlayback(it);
                        } else {
                            RevealItemInExplorer(curList, rh.itemIndex);
                        }
                    }
                    return 0;
                }
                if (PtInRectF(fx, fy, rh.rcRemove)) {
                    if (rh.itemIndex >= 0 && rh.itemIndex < static_cast<int>(curList.size())) {
                        CleanupTempFileIfOwned(curList[rh.itemIndex]);
                        curList.erase(curList.begin() + rh.itemIndex);
                        if (curList.empty() && !g_settings.replaceWinV) {
                            HideShelfAnimated();
                        } else {
                            RenderShelfFrame(true);
                        }
                    }
                    return 0;
                }
                if (PtInRectF(fx, fy, rh.rcRow)) {
                    s_lDownDragOut = true;
                    s_dragOutRowIdx = rh.itemIndex;
                    SetCapture(hwnd);
                    return 0;
                }
            }

            if (PtInRectF(fx, fy, g_rcFooterDragAll) && !curList.empty() && g_activeTab == ActiveTab::Shelf) {
                s_lDownDragOut = true;
                s_dragOutRowIdx = -1;
                SetCapture(hwnd);
                return 0;
            }

            if (PtInRectF(fx, fy, g_rcHeaderDrag)) {
                g_movingShelf = true;
                POINT ptScr{};
                GetCursorPos(&ptScr);
                g_moveDragOffset = { ptScr.x - g_shelfScreenRect.left, ptScr.y - g_shelfScreenRect.top };
                SetCapture(hwnd);
                return 0;
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_movingShelf) {
                g_movingShelf = false;
                if (GetCapture() == hwnd) ReleaseCapture();
                RenderShelfFrame(true);
                return 0;
            }
            if (s_lDownDragOut) {
                int clickedIdx = s_dragOutRowIdx;
                s_lDownDragOut = false;
                if (GetCapture() == hwnd) ReleaseCapture();
                // Single click in Clipboard History tab pastes immediately into previous window (just like Win+V!)
                if (!s_didDragOut && g_activeTab == ActiveTab::ClipboardHistory &&
                    g_settings.replaceWinV && clickedIdx >= 0) {
                    PasteHistoryItemToPreviousWindow(clickedIdx);
                }
            }
            s_didDragOut = false;
            return 0;
        }

        case WM_LBUTTONDBLCLK: {
            float fx = static_cast<float>(GET_X_LPARAM(lParam));
            float fy = static_cast<float>(GET_Y_LPARAM(lParam));
            const auto& curList = GetActiveList();
            for (const auto& rh : g_rowHits) {
                if (PtInRectF(fx, fy, rh.rcRow)) {
                    RevealItemInExplorer(curList, rh.itemIndex);
                    return 0;
                }
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            if (!g_stagedItems.empty()) {
                MinimizeShelfToTaskbar();
            } else {
                HideShelfAnimated();
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            auto filteredIdx = BuildFilteredIndices(GetActiveList(), g_activeFilter);
            if (filteredIdx.size() > 5) {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                int maxScroll = static_cast<int>(filteredIdx.size()) - 5;
                g_scrollOffset = std::clamp<int>(g_scrollOffset + (delta > 0 ? -1 : 1), 0, maxScroll);
                RenderShelfFrame(false);
            }
            return 0;
        }

        case WM_CAPTURECHANGED: {
            g_movingShelf = false;
            s_lDownDragOut = false;
            return 0;
        }

        case WM_TIMER: {
            if (wParam == IDT_CLIP_DEBOUNCE) {
                KillTimer(hwnd, IDT_CLIP_DEBOUNCE);
                if (GetTickCount64() >= g_ignoreClipUntilTick) {
                    if (g_settings.enableCopyPasteAlert) {
                        InspectClipboardForAlert(false);
                    }
                    if (g_settings.replaceWinV) {
                        CaptureClipboardIntoList(g_clipHistory, g_settings.maxClipboardHistory, true, false);
                    }
                }
                return 0;
            }

            if (wParam == IDT_PILL_SYNC) {
                if (g_shelfMinimized) {
                    RenderTaskbarPill(false);
                }
                return 0;
            }

            if (wParam == IDT_TOAST_CLEAR) {
                KillTimer(hwnd, IDT_TOAST_CLEAR);
                g_toastStatusText.clear();
                if (g_shelfVisible) RenderShelfFrame(false);
                return 0;
            }

            if (wParam == IDT_ANIM_FRAME) {
                ULONGLONG now = GetTickCount64();
                float dt = (g_lastAnimTick > 0 && now > g_lastAnimTick)
                    ? std::clamp(static_cast<float>(now - g_lastAnimTick) / 1000.0f, 0.001f, 0.055f)
                    : 0.014f;
                g_lastAnimTick = now;

                if (g_shelfTargetOpen) {
                    g_shelfAnim = std::min(1.0f, g_shelfAnim + dt / 0.165f);
                } else {
                    g_shelfAnim = std::max(0.0f, g_shelfAnim - dt / 0.135f);
                }
                RenderShelfFrame(false);

                bool done = g_shelfTargetOpen ? (g_shelfAnim >= 0.999f) : (g_shelfAnim <= 0.001f);
                if (done) {
                    KillTimer(hwnd, IDT_ANIM_FRAME);
                    g_animActive = false;
                }
                return 0;
            }

            if (wParam == IDT_DRAG_POLL) {
                if (g_settingsDirty.exchange(false, std::memory_order_acq_rel)) {
                    PostMessageW(hwnd, WM_APP_RELOAD_SETTINGS, 0, 0);
                }
                if (g_draggingOut || g_movingShelf) return 0;

                static bool      s_wasLDown = false;
                static POINT     s_lDownStartPt{};
                static ULONGLONG s_emptyReleaseTick = 0;

                bool lDown = ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0);
                if (!lDown) {
                    s_wasLDown = false;
                    g_shakeDetector.Reset();
                    if (g_shelfTargetOpen && g_stagedItems.empty() && !g_hoverDropActive &&
                        !g_openedByHotkey && g_activeTab == ActiveTab::Shelf) {
                        if (s_emptyReleaseTick == 0) s_emptyReleaseTick = GetTickCount64();
                        if (GetTickCount64() - s_emptyReleaseTick > 220) {
                            HideShelfAnimated();
                            s_emptyReleaseTick = 0;
                        }
                    } else {
                        s_emptyReleaseTick = 0;
                    }
                    return 0;
                }

                s_emptyReleaseTick = 0;
                POINT ptCur{};
                GetCursorPos(&ptCur);
                if (!s_wasLDown) {
                    s_wasLDown = true;
                    s_lDownStartPt = ptCur;
                }

                bool sysFileDrag = (FindWindowW(L"SysDragImage", nullptr) != nullptr);
                bool movedFarEnough = (std::abs(ptCur.x - s_lDownStartPt.x) >= 20 ||
                                       std::abs(ptCur.y - s_lDownStartPt.y) >= 20);
                ULONGLONG now = GetTickCount64();

                HMONITOR hMon = MonitorFromPoint(ptCur, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi{ sizeof(mi) };
                if (!hMon || !GetMonitorInfoW(hMon, &mi)) return 0;
                if (IsFullscreenAppActiveOnMonitor(hMon)) return 0;

                bool shakeTriggered = false;
                if (g_settings.triggerMode == TriggerMode::Both ||
                    g_settings.triggerMode == TriggerMode::ShakeOnly) {
                    if (sysFileDrag || g_settings.acceptImageDrops || g_settings.acceptTextDrops) {
                        shakeTriggered = g_shakeDetector.Observe(ptCur, now, g_settings.shakeSensitivity);
                    }
                }

                bool edgeBumped = false;
                if (!g_shelfTargetOpen && g_settings.enableEdgeBump && movedFarEnough &&
                    (sysFileDrag || g_settings.acceptImageDrops || g_settings.acceptTextDrops)) {
                    int monL = static_cast<int>(mi.rcMonitor.left);
                    int monT = static_cast<int>(mi.rcMonitor.top);
                    int monR = static_cast<int>(mi.rcMonitor.right);
                    int midX = (monL + monR) / 2;
                    if (g_settings.shelfPosition == ShelfPosition::TopCenter &&
                        ptCur.y <= monT + 3 && std::abs(ptCur.x - midX) <= 260) {
                        edgeBumped = true;
                    } else if (g_settings.shelfPosition == ShelfPosition::EdgeRight && ptCur.x >= monR - 3) {
                        edgeBumped = true;
                    } else if (g_settings.shelfPosition == ShelfPosition::EdgeLeft && ptCur.x <= monL + 3) {
                        edgeBumped = true;
                    }
                }

                if (!g_shelfTargetOpen && shakeTriggered) {
                    g_activeTab = ActiveTab::Shelf;
                    g_openedByHotkey = false;
                    ShowShelfAnimated(true, ptCur);
                } else if (edgeBumped) {
                    g_activeTab = ActiveTab::Shelf;
                    g_openedByHotkey = false;
                    ShowShelfAnimated(false, ptCur);
                } else if (sysFileDrag &&
                           (g_settings.triggerMode == TriggerMode::Both ||
                            g_settings.triggerMode == TriggerMode::AutoDrag)) {
                    if (!g_shelfTargetOpen && !g_shelfMinimized) {
                        g_activeTab = ActiveTab::Shelf;
                        g_openedByHotkey = false;
                        ShowShelfAnimated(g_settings.shelfPosition == ShelfPosition::NearCursor, ptCur);
                    }
                }
                return 0;
            }
            break;
        }

        case WM_CLOSE: {
            PostQuitMessage(0);
            return 0;
        }

        case WM_DESTROY: {
            RemoveClipboardFormatListener(hwnd);
            UnregisterHotKey(hwnd, HOTKEY_TOGGLE_SHELF);
            KillTimer(hwnd, IDT_DRAG_POLL);
            KillTimer(hwnd, IDT_ANIM_FRAME);
            KillTimer(hwnd, IDT_TOAST_CLEAR);
            KillTimer(hwnd, IDT_CLIP_DEBOUNCE);
            KillTimer(hwnd, IDT_PILL_SYNC);
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// 11. UI Thread & Windhawk Tool Mod Lifecycle
// ---------------------------------------------------------------------------
static void StopThreadWithQuit(HANDLE& hThread, DWORD& threadId) {
    if (!hThread) return;
    while (WaitForSingleObject(hThread, 40) == WAIT_TIMEOUT) {
        if (threadId != 0) PostThreadMessageW(threadId, WM_QUIT, 0, 0);
    }
    CloseHandle(hThread);
    hThread = nullptr;
    threadId = 0;
}

static DWORD WINAPI ShelfUiThreadProc(LPVOID) {
    MSG initMsg{};
    PeekMessageW(&initMsg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HRESULT hrOle = OleInitialize(nullptr);
    EnsureRegisteredFormats();
    ClearStagingDirContents();
    g_settings = ReadSettingsFromWindhawk();
    g_settingsDirty.store(false, std::memory_order_relaxed);

    // Reset runtime UI state cleanly on every init
    g_shelfVisible = false;
    g_shelfTargetOpen = false;
    g_shelfAnim = 0.0f;
    g_animActive = false;
    g_shelfMinimized = false;
    g_pillTargetOpen = false;
    g_pillAnim = 0.0f;
    g_pillAnimActive = false;
    g_alertTargetOpen = false;
    g_alertAnim = 0.0f;
    g_alertAnimActive = false;
    g_previewImage.reset();
    g_previewFullscreen = false;
    g_previewHoverBtn = 0;
    g_draggingOut = false;
    g_movingShelf = false;
    g_hoverDropActive = false;
    g_hasCustomPos = false;

    HINSTANCE hInst = GetModInstanceHandle();
    const wchar_t* kShelfClass   = L"WindhawkDragNDropEnhancedShelfV2";
    const wchar_t* kPillClass    = L"WindhawkDragNDropEnhancedPillV2";
    const wchar_t* kAlertClass   = L"WindhawkDragNDropEnhancedAlertV2";
    const wchar_t* kPreviewClass = L"WindhawkDragNDropEnhancedPreviewV2";

    WNDCLASSEXW wc{ sizeof(wc) };
    wc.style         = CS_DBLCLKS;
    wc.lpfnWndProc   = ShelfWndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kShelfClass;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"Failed to register kShelfClass: %lu", GetLastError());
        if (SUCCEEDED(hrOle)) OleUninitialize();
        return 1;
    }

    WNDCLASSEXW wcPill{ sizeof(wcPill) };
    wcPill.lpfnWndProc   = TaskbarPillWndProc;
    wcPill.hInstance     = hInst;
    wcPill.hCursor       = LoadCursorW(nullptr, IDC_HAND);
    wcPill.lpszClassName = kPillClass;
    if (!RegisterClassExW(&wcPill)) {
        Wh_Log(L"Failed to register kPillClass: %lu", GetLastError());
        UnregisterClassW(kShelfClass, hInst);
        if (SUCCEEDED(hrOle)) OleUninitialize();
        return 1;
    }

    WNDCLASSEXW wcAlert{ sizeof(wcAlert) };
    wcAlert.lpfnWndProc   = CopyPasteAlertWndProc;
    wcAlert.hInstance     = hInst;
    wcAlert.lpszClassName = kAlertClass;
    if (!RegisterClassExW(&wcAlert)) {
        Wh_Log(L"Failed to register kAlertClass: %lu", GetLastError());
        UnregisterClassW(kPillClass, hInst);
        UnregisterClassW(kShelfClass, hInst);
        if (SUCCEEDED(hrOle)) OleUninitialize();
        return 1;
    }

    WNDCLASSEXW wcPrev{ sizeof(wcPrev) };
    wcPrev.style         = CS_DBLCLKS;
    wcPrev.lpfnWndProc   = ImagePreviewWndProc;
    wcPrev.hInstance     = hInst;
    wcPrev.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wcPrev.lpszClassName = kPreviewClass;
    if (!RegisterClassExW(&wcPrev)) {
        Wh_Log(L"Failed to register kPreviewClass: %lu", GetLastError());
        UnregisterClassW(kAlertClass, hInst);
        UnregisterClassW(kPillClass, hInst);
        UnregisterClassW(kShelfClass, hInst);
        if (SUCCEEDED(hrOle)) OleUninitialize();
        return 1;
    }

    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    g_shelfHwnd   = CreateWindowExW(exStyle, kShelfClass, L"Drag n Drop Enhanced Shelf", WS_POPUP,
                                    0, 0, 0, 0, nullptr, nullptr, hInst, nullptr);
    g_pillHwnd    = CreateWindowExW(exStyle, kPillClass,  L"Drag n Drop Enhanced Mini Bar", WS_POPUP,
                                    0, 0, 0, 0, nullptr, nullptr, hInst, nullptr);
    g_alertHwnd   = CreateWindowExW(exStyle | WS_EX_TRANSPARENT, kAlertClass, L"Drag n Drop Copy Paste Alert", WS_POPUP,
                                    0, 0, 0, 0, nullptr, nullptr, hInst, nullptr);
    g_previewHwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, kPreviewClass, L"Image Preview", WS_POPUP,
                                    0, 0, 0, 0, nullptr, nullptr, hInst, nullptr);

    if (g_shelfHwnd) {
        AddClipboardFormatListener(g_shelfHwnd);
        if (g_settings.replaceWinV) {
            CaptureClipboardIntoList(g_clipHistory, g_settings.maxClipboardHistory, true, false);
        }
        if (SUCCEEDED(hrOle)) {
            g_shelfDropTarget = new ShelfDropTarget(false);
            RegisterDragDrop(g_shelfHwnd, g_shelfDropTarget);
        }
        if (g_settings.enableGlobalHotkey) {
            RegisterHotKey(g_shelfHwnd, HOTKEY_TOGGLE_SHELF, MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, 'D');
        }
        SetTimer(g_shelfHwnd, IDT_DRAG_POLL, 35, nullptr);
        SetTimer(g_shelfHwnd, IDT_PILL_SYNC, 1200, nullptr);
    }

    if (g_pillHwnd && SUCCEEDED(hrOle)) {
        g_pillDropTarget = new ShelfDropTarget(true);
        RegisterDragDrop(g_pillHwnd, g_pillDropTarget);
    }

    if (g_settingsDirty.exchange(false, std::memory_order_acq_rel) && g_shelfHwnd) {
        PostMessageW(g_shelfHwnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }

    MSG msg{};
    while (g_running.load(std::memory_order_relaxed) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    StopInlineAudioPlayback();
    if (g_previewHwnd && IsWindow(g_previewHwnd)) {
        DestroyWindow(g_previewHwnd);
        g_previewHwnd = nullptr;
    }
    if (g_alertHwnd && IsWindow(g_alertHwnd)) {
        KillTimer(g_alertHwnd, IDT_ALERT_ANIM);
        KillTimer(g_alertHwnd, IDT_ALERT_EXPIRE);
        DestroyWindow(g_alertHwnd);
        g_alertHwnd = nullptr;
    }
    if (g_pillHwnd && IsWindow(g_pillHwnd)) {
        KillTimer(g_pillHwnd, IDT_PILL_ANIM);
        RevokeDragDrop(g_pillHwnd);
        DestroyWindow(g_pillHwnd);
        g_pillHwnd = nullptr;
    }
    if (g_pillDropTarget) {
        g_pillDropTarget->Release();
        g_pillDropTarget = nullptr;
    }

    if (g_shelfHwnd && IsWindow(g_shelfHwnd)) {
        RevokeDragDrop(g_shelfHwnd);
        DestroyWindow(g_shelfHwnd);
        g_shelfHwnd = nullptr;
    }
    if (g_shelfDropTarget) {
        g_shelfDropTarget->Release();
        g_shelfDropTarget = nullptr;
    }

    ClearVectorItems(g_stagedItems);
    ClearVectorItems(g_clipHistory);
    ClearStagingDirContents();
    g_rowHits.clear();
    ReleaseRenderResources();
    g_wallpaperCache = {};

    UnregisterClassW(kPreviewClass, hInst);
    UnregisterClassW(kAlertClass, hInst);
    UnregisterClassW(kPillClass, hInst);
    UnregisterClassW(kShelfClass, hInst);

    if (SUCCEEDED(hrOle)) OleUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Drag n Drop Enhanced for Windows 10 and 11 v2.0.1.0 (Tool Mod)...");
    g_unloading.store(false, std::memory_order_relaxed);
    g_running.store(true, std::memory_order_relaxed);
    g_settingsDirty.store(false, std::memory_order_relaxed);
    g_settings = ReadSettingsFromWindhawk();

    g_uiThread  = CreateThread(nullptr, 0, ShelfUiThreadProc, nullptr, 0, &g_uiThreadId);
    g_kbdThread = CreateThread(nullptr, 0, KbdHookThreadProc, nullptr, 0, &g_kbdThreadId);
    return g_uiThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    g_settingsDirty.store(true, std::memory_order_release);
    ModSettings fresh = ReadSettingsFromWindhawk();
    g_replaceWinVAtomic.store(fresh.replaceWinV, std::memory_order_relaxed);
    g_enableCopyPasteAlertAtomic.store(fresh.enableCopyPasteAlert, std::memory_order_relaxed);
    if (g_shelfHwnd && IsWindow(g_shelfHwnd)) {
        PostMessageW(g_shelfHwnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninitializing Drag n Drop Enhanced for Windows 10 and 11 v2.0.1.0 (Tool Mod)...");
    g_unloading.store(true, std::memory_order_relaxed);
    g_running.store(false, std::memory_order_relaxed);

    if (g_kbdThreadId != 0) {
        PostThreadMessageW(g_kbdThreadId, WM_QUIT, 0, 0);
    }
    if (g_shelfHwnd && IsWindow(g_shelfHwnd)) {
        PostMessageW(g_shelfHwnd, WM_CLOSE, 0, 0);
    }
    StopThreadWithQuit(g_kbdThread, g_kbdThreadId);
    StopThreadWithQuit(g_uiThread, g_uiThreadId);
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
        LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
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
