// ==WindhawkMod==
// @id              taskbar-notification-bell-badge
// @name            Taskbar Notification Bell Count Badge
// @name:vi-VN      Số lượng thông báo trên chuông Taskbar
// @description     Displays the unread notification count inside a small circular badge next to or below the Windows 11 taskbar notification bell icon
// @description:vi-VN Hiển thị số lượng thông báo trong một vòng tròn nhỏ bên cạnh hoặc bên dưới biểu tượng quả chuông trên thanh Taskbar Windows 11
// @version         1.0.19
// @author          caotu2005
// @github          https://github.com/caotu2005
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lgdiplus -lole32 -loleaut32 -luuid -ldwmapi -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Notification Bell Count Badge

Displays the current number of active Windows notifications inside a compact,
Fluent-style circular (or pill-shaped for 2+ digits) badge positioned **next to
or below** the Windows 11 taskbar notification bell icon—keeping the bell icon
itself completely unobstructed and legible.

![Horizontal Taskbar Preview](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/taskbarbell/v1horizontal.png)

Horizontal Taskbar Preview

![Vertical Taskbar Preview](https://raw.githubusercontent.com/caotu2k5/windhawk-mods-resources/main/taskbarbell/v1vertical.png)

Vertical Taskbar Preview

## Features
- **All-edge taskbar support (Bottom, Top, Left, Right)**:
  - **Top / Bottom (horizontal taskbar)**: positioned snugly to the **right** of
    the bell without overlapping the Show Desktop line (configurable to right or
    below in Settings).
  - **Left / Right (vertical taskbar)**: positioned directly **below** the bell
    clapper.
- **Language-independent Notification Center count**: Reads active toast
  notifications directly from Windows' per-user notification database
  (`wpndatabase.db` via the built-in `winsqlite3.dll`, read-only) and subscribes
  to `WNF_SHEL_NOTIFICATIONS` for instant updates—works on **any Windows display
  language** and continues reflecting remaining notifications even after opening
  Notification Center (with optional `0` display).
- **Runs safely as a dedicated tool mod**: Executes in an isolated
  `windhawk.exe` tool process (no injection into `explorer.exe`), preventing
  duplicate overlays or shell instability.
- **Optional custom hover tooltip (disabled by default)**: Can be enabled in
  Settings to show the active notification count on hover (e.g., `5 active
  notifications` or `5 thông báo hiện có` on Vietnamese Windows) while verifying
  via UI Automation (`UIA_ToolTipControlTypeId`) before hiding Windows' native
  tooltip.
- **Crisp Fluent styling & smart visibility**: Renders a DPI-aware,
  anti-aliased, click-through GDI+ badge (`PixelFormat32bppPARGB`) that adapts
  to your Windows accent color, stays visible next to maximized windows, and
  automatically hides for true fullscreen apps, auto-hidden taskbars, or when
  the count is 0.

## Why a separate mod from Notification Bell Highlighter?
[Notification Bell Highlighter](https://windhawk.net/mods/notification-bell-highlighter)
is designed as a bright, pulsing attention overlay drawn **on top of** the bell
icon and parses the English accessible name (`"N new notification"`), so it only
counts newly arrived notifications (from 1+) on English Windows. This mod takes
a different, minimal Fluent-badge approach: a static, unobstructed count pill
placed beside/below the bell, full multi-language support via `wpndatabase.db` +
WNF, all-edge (Bottom/Top/Left/Right) layout support, and fullscreen/auto-hide
awareness.

## Positioning tip
Set **Preview count** to a number like `5` in Settings to fine-tune **Offset X**,
**Offset Y**, and **Badge diameter**, then set **Preview count** back to `0`.

---

# Số lượng thông báo trên chuông Taskbar (Tiếng Việt)

Hiển thị số lượng thông báo đang có trong Windows Notification Center bên trong
một vòng tròn nhỏ gọn theo phong cách Fluent (tự mở rộng thành dạng viên thuốc
khi có từ 2 chữ số trở lên) đặt **bên cạnh hoặc bên dưới** quả chuông trên thanh
Taskbar Windows 11 mà không che khuất biểu tượng chuông.

## Tính năng chính
- **Hỗ trợ đầy đủ 4 cạnh Taskbar (Dưới, Trên, Trái, Phải)**:
  - **Taskbar Trên / Dưới (ngang)**: tự động đặt gọn **bên phải** quả chuông,
    tách khỏi vạch Show Desktop (có thể đổi sang bên dưới trong Cài đặt).
  - **Taskbar Trái / Phải (dọc)**: tự động đặt sát **bên dưới** quả chuông.
- **Đếm thông báo chính xác trên mọi ngôn ngữ Windows**: Đọc trực tiếp số thông
  báo đang có trong Notification Center từ `wpndatabase.db` (chỉ đọc qua
  `winsqlite3.dll` có sẵn) kết hợp sự kiện `WNF_SHEL_NOTIFICATIONS` để cập nhật
  tức thì, không bị phụ thuộc ngôn ngữ tiếng Anh và vẫn giữ số đếm chính xác kể
  cả sau khi mở xem Notification Center.
- **Chạy độc lập dạng tool mod an toàn**: Chạy trong tiến trình `windhawk.exe`
  riêng biệt, không tiêm vào `explorer.exe` nên tránh hoàn toàn lỗi chồng badge
  hoặc ảnh hưởng độ ổn định của Windows Shell.
- **Chú thích tùy chọn khi rê chuột (tắt mặc định)**: Có thể bật trong Cài đặt
  để hiển thị `<N> thông báo hiện có` khi rê chuột vào chuông/vòng tròn và chỉ
  ẩn đúng cửa sổ tooltip đã xác minh qua UI Automation (`UIA_ToolTipControlTypeId`).
- **Tự động ẩn thông minh**: Vẫn hiển thị khi phóng to cửa sổ và tự ẩn khi chạy
  ứng dụng toàn màn hình (fullscreen), khi ẩn taskbar hoặc khi không có thông
  báo nào.

## Mẹo căn chỉnh nhanh
Đặt tạm **Số đếm xem trước** thành `5` trong mục Cài đặt để xem trước và căn
chỉnh **Dịch ngang/dọc (DIP)** hoặc **Đường kính vòng tròn**, sau đó đặt lại về
`0` để quay về chế độ đếm thực tế.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- countSource: auto
  $name: Notification count source
  $name:vi-VN: Nguồn đếm thông báo
  $description: Choose where the notification count is read from.
  $description:vi-VN: Chọn nguồn dữ liệu để lấy số lượng thông báo.
  $options:
  - auto: Auto (Notification Center database + WNF, recommended)
  - db: Notification Center database only (wpndatabase.db)
  - wnf: Unseen count only (WNF_SHEL_NOTIFICATIONS)
- horizontalPlacement: right
  $name: Badge placement on Top/Bottom taskbar
  $name:vi-VN: Vị trí vòng tròn trên Taskbar Trên/Dưới
  $description: Where to place the badge relative to the bell when the taskbar is horizontal (Top or Bottom).
  $description:vi-VN: Vị trí đặt vòng tròn số so với quả chuông khi taskbar nằm ngang (Trên hoặc Dưới).
  $options:
  - right: Right of the bell (default)
  - below: Below the bell
- hideWhenZero: true
  $name: Hide badge when count is 0
  $name:vi-VN: Ẩn vòng tròn khi không có thông báo
  $description: Automatically hide the circular badge when there are no notifications.
  $description:vi-VN: Tự động ẩn vòng tròn khi số lượng thông báo bằng 0.
- maxCount: 99
  $name: Max displayed count
  $name:vi-VN: Số hiển thị tối đa
  $description: Counts above this value are shown with a plus sign (e.g., 99+).
  $description:vi-VN: Nếu số thông báo vượt quá mức này sẽ hiển thị kèm dấu cộng (ví dụ 99+).
- anchorMode: auto
  $name: Bell anchor mode
  $name:vi-VN: Chế độ neo theo quả chuông
  $description: How to locate the bell horizontally inside NotificationCenterButton.
  $description:vi-VN: Cách xác định tâm ngang của quả chuông trong nút NotificationCenterButton.
  $options:
  - auto: Auto (detect vertical/bell-only vs horizontal Clock + Bell button)
  - center: Center of button (best for vertical taskbar or standalone bell button)
  - right: Right edge of button (standard horizontal Windows 11 Clock + Bell button)
- offsetX: 0
  $name: Horizontal offset (DIP)
  $name:vi-VN: Dịch ngang (DIP)
  $description: Fine-tune horizontal position in pixels at 100% DPI (negative = left, positive = right).
  $description:vi-VN: Tinh chỉnh vị trí theo chiều ngang ở mức 100% DPI (số âm = sang trái, số dương = sang phải).
- offsetY: 0
  $name: Vertical offset (DIP)
  $name:vi-VN: Dịch dọc (DIP)
  $description: Fine-tune vertical position in pixels at 100% DPI (negative = up, positive = down).
  $description:vi-VN: Tinh chỉnh vị trí theo chiều dọc ở mức 100% DPI (số âm = lên trên, số dương = xuống dưới).
- badgeSize: 13
  $name: Badge diameter (DIP)
  $name:vi-VN: Đường kính vòng tròn (DIP)
  $description: Diameter of the circular badge at 100% DPI (widens into a pill for 2+ digits).
  $description:vi-VN: Đường kính của vòng tròn ở mức 100% DPI (tự mở rộng thành hình viên thuốc khi có từ 2 chữ số).
- fontSize: 8
  $name: Font size (pt)
  $name:vi-VN: Cỡ chữ (pt)
  $description: Font size for the notification count number.
  $description:vi-VN: Kích thước chữ của số lượng thông báo.
- fontFamily: Segoe UI Variable Small
  $name: Font family
  $name:vi-VN: Phông chữ
  $description: Font used to render the number (falls back to Segoe UI if unavailable).
  $description:vi-VN: Phông chữ dùng để hiển thị con số (tự động dùng Segoe UI nếu không có).
- useAccentColor: true
  $name: Use Windows accent color
  $name:vi-VN: Dùng màu chủ đạo của Windows
  $description: Fill the badge with your current Windows accent color and auto-select contrasting text.
  $description:vi-VN: Tô nền vòng tròn theo màu Accent hiện tại của Windows và tự chọn màu chữ tương phản.
- badgeColor: "#E81123"
  $name: Custom badge fill color (#RRGGBB)
  $name:vi-VN: Màu nền vòng tròn tùy chỉnh (#RRGGBB)
  $description: Used when "Use Windows accent color" is disabled.
  $description:vi-VN: Áp dụng khi tắt tùy chọn "Dùng màu chủ đạo của Windows".
- textColor: "#FFFFFF"
  $name: Custom text color (#RRGGBB)
  $name:vi-VN: Màu chữ tùy chỉnh (#RRGGBB)
  $description: Used when "Use Windows accent color" is disabled.
  $description:vi-VN: Áp dụng khi tắt tùy chọn "Dùng màu chủ đạo của Windows".
- borderWidth: 1
  $name: Outline thickness (px)
  $name:vi-VN: Độ dày viền ngoài (px)
  $description: Subtle outline around the badge to separate it from the bell/taskbar (0 to disable).
  $description:vi-VN: Viền mỏng quanh vòng tròn giúp nổi bật trên nền chuông/taskbar (đặt 0 để tắt).
- borderColor: "#1A1A1A"
  $name: Outline color (#RRGGBB)
  $name:vi-VN: Màu viền ngoài (#RRGGBB)
  $description: Color of the badge outline when Outline thickness > 0.
  $description:vi-VN: Màu của viền ngoài khi Độ dày viền ngoài > 0.
- customTooltip: false
  $name: Show custom hover tooltip
  $name:vi-VN: Hiện chú thích khi rê chuột
  $description: When hovering over the bell or badge with active notifications, show "<N> active notifications" and temporarily hide Windows' "No new notifications" tooltip.
  $description:vi-VN: Khi rê chuột vào chuông hoặc vòng tròn lúc đang có thông báo, hiển thị "<N> thông báo hiện có" và tạm ẩn chú thích "Không có thông báo mới" của Windows.
- tooltipTheme: auto
  $name: Hover tooltip theme
  $name:vi-VN: Giao diện chú thích khi rê chuột
  $description: Color theme for the custom hover tooltip (Auto follows Windows taskbar Light/Dark mode).
  $description:vi-VN: Màu nền của chú thích khi rê chuột (Tự động theo chế độ Sáng/Tối của Windows).
  $options:
  - auto: Auto (follow Windows Light/Dark mode)
  - dark: Dark
  - light: Light
- previewCount: 0
  $name: Preview count (0 = live)
  $name:vi-VN: Số đếm xem trước (0 = thực tế)
  $description: Set to a positive number (e.g., 5 or 12) to test appearance and alignment, then set back to 0.
  $description:vi-VN: Nhập một số dương (ví dụ 5 hoặc 12) để xem trước và căn chỉnh vị trí, sau đó đặt lại về 0.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <objbase.h>
#include <oleauto.h>
#include <shellapi.h>
#include <uiautomation.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// ============================================================================
// WNF (Windows Notification Facility) definitions
// ============================================================================

struct WNF_STATE_NAME {
    ULONG Data[2];
};

static constexpr WNF_STATE_NAME kWnfShelNotifications = {0xA3BC1035u, 0x0D83063Eu};

using PFN_WNF_USER_CALLBACK = NTSTATUS(NTAPI*)(
    WNF_STATE_NAME StateName,
    ULONG ChangeStamp,
    const void* TypeId,
    void* CallbackContext,
    const void* Buffer,
    ULONG BufferSize);

using PFN_NtQueryWnfStateData = NTSTATUS(NTAPI*)(
    const WNF_STATE_NAME* StateName,
    const void* TypeId,
    const void* ExplicitScope,
    ULONG* ChangeStamp,
    void* Buffer,
    ULONG* BufferSize);

using PFN_RtlSubscribeWnfStateChangeNotification = NTSTATUS(NTAPI*)(
    void** Subscription,
    WNF_STATE_NAME StateName,
    ULONG ChangeStamp,
    PFN_WNF_USER_CALLBACK Callback,
    void* CallbackContext,
    const void* TypeId,
    ULONG SerializationGroup,
    ULONG Flags);

using PFN_RtlUnsubscribeWnfNotificationWaitForCompletion = NTSTATUS(NTAPI*)(
    void* Subscription);

// ============================================================================
// Built-in Windows SQLite (winsqlite3.dll) minimal definitions
// Note: Windows' winsqlite3.dll uses __stdcall (WINAPI) on 32-bit x86.
// ============================================================================

struct sqlite3;
struct sqlite3_stmt;

static constexpr int kSqliteOk = 0;
static constexpr int kSqliteRow = 100;
static constexpr int kSqliteOpenReadonly = 0x00000001;

using PFN_sqlite3_open_v2 = int(WINAPI*)(const char* filename, sqlite3** ppDb, int flags, const char* zVfs);
using PFN_sqlite3_close = int(WINAPI*)(sqlite3* db);
using PFN_sqlite3_busy_timeout = int(WINAPI*)(sqlite3* db, int ms);
using PFN_sqlite3_prepare_v2 = int(WINAPI*)(sqlite3* db, const char* zSql, int nByte, sqlite3_stmt** ppStmt, const char** pzTail);
using PFN_sqlite3_bind_int64 = int(WINAPI*)(sqlite3_stmt* pStmt, int idx, int64_t val);
using PFN_sqlite3_step = int(WINAPI*)(sqlite3_stmt* pStmt);
using PFN_sqlite3_column_int = int(WINAPI*)(sqlite3_stmt* pStmt, int iCol);
using PFN_sqlite3_finalize = int(WINAPI*)(sqlite3_stmt* pStmt);

// ============================================================================
// Constants & Messages
// ============================================================================

static constexpr wchar_t kOverlayClassName[] = L"WhNotificationBellBadgeOverlay";
static constexpr UINT WM_APP_UPDATE_BADGE = WM_APP + 1;
static constexpr UINT WM_APP_RELOAD_SETTINGS = WM_APP + 2;
static constexpr UINT_PTR kSyncTimerId = 1;
static constexpr UINT kSyncIntervalMs = 250;
static constexpr UINT_PTR kHoverTimerId = 2;
static constexpr UINT kHoverIntervalMs = 80;
static constexpr ULONGLONG kHoverShowDelayMs = 250;
static constexpr ULONGLONG kUiaFullSearchCooldownMs = 3000;

// ============================================================================
// Settings & Global State
// ============================================================================

enum class CountSource {
    Auto,
    DbOnly,
    WnfOnly,
};

enum class HorizontalPlacement {
    Right,
    Below,
};

enum class AnchorMode {
    Auto,
    Center,
    Right,
};

enum class TooltipTheme {
    Auto,
    Dark,
    Light,
};

struct ModSettings {
    CountSource countSource = CountSource::Auto;
    HorizontalPlacement horizontalPlacement = HorizontalPlacement::Right;
    bool hideWhenZero = true;
    int maxCount = 99;
    AnchorMode anchorMode = AnchorMode::Auto;
    int offsetX = 0;
    int offsetY = 0;
    int badgeSize = 13;
    int fontSize = 8;
    std::wstring fontFamily = L"Segoe UI Variable Small";
    bool useAccentColor = true;
    COLORREF badgeColor = RGB(0xE8, 0x11, 0x23);
    COLORREF textColor = RGB(0xFF, 0xFF, 0xFF);
    int borderWidth = 1;
    COLORREF borderColor = RGB(0x1A, 0x1A, 0x1A);
    bool customTooltip = false;
    TooltipTheme tooltipTheme = TooltipTheme::Auto;
    int previewCount = 0;
};

static std::mutex g_settingsMutex;
static ModSettings g_settings;

static std::atomic<bool> g_stopping{false};
static std::atomic<int> g_wnfCount{0};
static std::atomic<bool> g_forceDbRefresh{true};
static std::atomic<HWND> g_overlayHwnd{nullptr};

static HANDLE g_workerThread = nullptr;
static DWORD g_workerThreadId = 0;

static PFN_NtQueryWnfStateData g_pNtQueryWnfStateData = nullptr;
static PFN_RtlSubscribeWnfStateChangeNotification g_pRtlSubscribeWnf = nullptr;
static PFN_RtlUnsubscribeWnfNotificationWaitForCompletion g_pRtlUnsubscribeWnf = nullptr;
static void* g_wnfSubscription = nullptr;

// ============================================================================
// Helpers: String, Color & Settings Parsing
// ============================================================================

static std::wstring ToLowerW(const wchar_t* src) {
    if (!src) {
        return {};
    }
    std::wstring out(src);
    if (!out.empty()) {
        CharLowerBuffW(out.data(), static_cast<DWORD>(out.size()));
    }
    return out;
}

static bool ContainsI(const std::wstring& lowerHaystack, const wchar_t* lowerNeedle) {
    return !lowerHaystack.empty() && lowerNeedle && lowerHaystack.find(lowerNeedle) != std::wstring::npos;
}

static bool RectsEqual(const RECT& a, const RECT& b) {
    return a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
}

static COLORREF ParseHexColor(const wchar_t* hex, COLORREF fallback) {
    if (!hex) {
        return fallback;
    }
    std::wstring s(hex);
    if (!s.empty() && s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() != 6) {
        return fallback;
    }
    wchar_t* endPtr = nullptr;
    unsigned long val = wcstoul(s.c_str(), &endPtr, 16);
    if (!endPtr || *endPtr != L'\0') {
        return fallback;
    }
    BYTE r = static_cast<BYTE>((val >> 16) & 0xFF);
    BYTE g = static_cast<BYTE>((val >> 8) & 0xFF);
    BYTE b = static_cast<BYTE>(val & 0xFF);
    return RGB(r, g, b);
}

static COLORREF GetWindowsAccentColor(COLORREF fallback) {
    DWORD abgr = 0;
    DWORD dataSize = sizeof(abgr);
    if (RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\DWM",
            L"AccentColor",
            RRF_RT_REG_DWORD,
            nullptr,
            &abgr,
            &dataSize) == ERROR_SUCCESS) {
        BYTE r = static_cast<BYTE>(abgr & 0xFF);
        BYTE g = static_cast<BYTE>((abgr >> 8) & 0xFF);
        BYTE b = static_cast<BYTE>((abgr >> 16) & 0xFF);
        return RGB(r, g, b);
    }

    DWORD argb = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&argb, &opaque))) {
        BYTE r = static_cast<BYTE>((argb >> 16) & 0xFF);
        BYTE g = static_cast<BYTE>((argb >> 8) & 0xFF);
        BYTE b = static_cast<BYTE>(argb & 0xFF);
        return RGB(r, g, b);
    }

    return fallback;
}

static COLORREF GetContrastingTextColor(COLORREF bg) {
    double r = GetRValue(bg) / 255.0;
    double g = GetGValue(bg) / 255.0;
    double b = GetBValue(bg) / 255.0;
    double luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    return (luminance > 0.58) ? RGB(0x11, 0x11, 0x11) : RGB(0xFF, 0xFF, 0xFF);
}

static std::string WideToUtf8(const std::wstring& w) {
    if (w.empty()) {
        return {};
    }
    int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()), nullptr, 0, nullptr, nullptr);
    if (len <= 0) {
        return {};
    }
    std::string out(static_cast<size_t>(len), '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()), out.data(), len, nullptr, nullptr);
    return out;
}

static void LoadSettings() {
    ModSettings s;

    PCWSTR srcStr = Wh_GetStringSetting(L"countSource");
    if (srcStr) {
        if (wcscmp(srcStr, L"db") == 0) {
            s.countSource = CountSource::DbOnly;
        } else if (wcscmp(srcStr, L"wnf") == 0) {
            s.countSource = CountSource::WnfOnly;
        } else {
            s.countSource = CountSource::Auto;
        }
        Wh_FreeStringSetting(srcStr);
    }

    PCWSTR horizPlaceStr = Wh_GetStringSetting(L"horizontalPlacement");
    if (horizPlaceStr) {
        if (wcscmp(horizPlaceStr, L"below") == 0) {
            s.horizontalPlacement = HorizontalPlacement::Below;
        } else {
            s.horizontalPlacement = HorizontalPlacement::Right;
        }
        Wh_FreeStringSetting(horizPlaceStr);
    }

    s.hideWhenZero = Wh_GetIntSetting(L"hideWhenZero") != 0;
    s.maxCount = std::clamp(Wh_GetIntSetting(L"maxCount"), 9, 999);

    PCWSTR anchorStr = Wh_GetStringSetting(L"anchorMode");
    if (anchorStr) {
        if (wcscmp(anchorStr, L"center") == 0) {
            s.anchorMode = AnchorMode::Center;
        } else if (wcscmp(anchorStr, L"right") == 0) {
            s.anchorMode = AnchorMode::Right;
        } else {
            s.anchorMode = AnchorMode::Auto;
        }
        Wh_FreeStringSetting(anchorStr);
    }

    s.offsetX = std::clamp(Wh_GetIntSetting(L"offsetX"), -200, 200);
    s.offsetY = std::clamp(Wh_GetIntSetting(L"offsetY"), -200, 200);
    s.badgeSize = std::clamp(Wh_GetIntSetting(L"badgeSize"), 8, 36);
    s.fontSize = std::clamp(Wh_GetIntSetting(L"fontSize"), 5, 24);

    PCWSTR fontStr = Wh_GetStringSetting(L"fontFamily");
    if (fontStr) {
        if (fontStr[0] != L'\0') {
            s.fontFamily = fontStr;
        }
        Wh_FreeStringSetting(fontStr);
    }

    s.useAccentColor = Wh_GetIntSetting(L"useAccentColor") != 0;

    PCWSTR badgeColorStr = Wh_GetStringSetting(L"badgeColor");
    if (badgeColorStr) {
        s.badgeColor = ParseHexColor(badgeColorStr, s.badgeColor);
        Wh_FreeStringSetting(badgeColorStr);
    }

    PCWSTR textColorStr = Wh_GetStringSetting(L"textColor");
    if (textColorStr) {
        s.textColor = ParseHexColor(textColorStr, s.textColor);
        Wh_FreeStringSetting(textColorStr);
    }

    s.borderWidth = std::clamp(Wh_GetIntSetting(L"borderWidth"), 0, 4);

    PCWSTR borderColorStr = Wh_GetStringSetting(L"borderColor");
    if (borderColorStr) {
        s.borderColor = ParseHexColor(borderColorStr, s.borderColor);
        Wh_FreeStringSetting(borderColorStr);
    }

    s.customTooltip = Wh_GetIntSetting(L"customTooltip") != 0;

    PCWSTR tipThemeStr = Wh_GetStringSetting(L"tooltipTheme");
    if (tipThemeStr) {
        if (wcscmp(tipThemeStr, L"dark") == 0) {
            s.tooltipTheme = TooltipTheme::Dark;
        } else if (wcscmp(tipThemeStr, L"light") == 0) {
            s.tooltipTheme = TooltipTheme::Light;
        } else {
            s.tooltipTheme = TooltipTheme::Auto;
        }
        Wh_FreeStringSetting(tipThemeStr);
    }
    s.previewCount = std::clamp(Wh_GetIntSetting(L"previewCount"), 0, 9999);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
}

static ModSettings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// ============================================================================
// WNF Notification Count Query & Subscription
// ============================================================================

static bool QueryWnfNotificationCount(int* outCount, ULONG* outChangeStamp = nullptr) {
    if (!g_pNtQueryWnfStateData) {
        return false;
    }
    ULONG changeStamp = 0;
    uint32_t count = 0;
    ULONG bufferSize = sizeof(count);
    NTSTATUS status = g_pNtQueryWnfStateData(
        &kWnfShelNotifications,
        nullptr,
        nullptr,
        &changeStamp,
        &count,
        &bufferSize);

    if (status < 0) {
        return false;
    }

    if (outChangeStamp) {
        *outChangeStamp = changeStamp;
    }
    if (outCount) {
        *outCount = (bufferSize >= sizeof(uint32_t)) ? static_cast<int>(count) : 0;
    }
    return true;
}

static NTSTATUS NTAPI OnWnfNotificationChanged(
    WNF_STATE_NAME /*StateName*/,
    ULONG /*ChangeStamp*/,
    const void* /*TypeId*/,
    void* /*CallbackContext*/,
    const void* Buffer,
    ULONG BufferSize) {
    if (g_stopping.load()) {
        return 0;
    }

    int count = 0;
    if (Buffer && BufferSize >= sizeof(uint32_t)) {
        uint32_t raw = 0;
        memcpy(&raw, Buffer, sizeof(raw));
        count = static_cast<int>(raw);
    } else {
        QueryWnfNotificationCount(&count);
    }

    g_wnfCount.store(count);
    g_forceDbRefresh.store(true);

    HWND hwnd = g_overlayHwnd.load();
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_UPDATE_BADGE, 0, 0);
    }
    return 0;
}

static void InitWnf() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        return;
    }

    g_pNtQueryWnfStateData = reinterpret_cast<PFN_NtQueryWnfStateData>(
        GetProcAddress(hNtdll, "NtQueryWnfStateData"));
    g_pRtlSubscribeWnf = reinterpret_cast<PFN_RtlSubscribeWnfStateChangeNotification>(
        GetProcAddress(hNtdll, "RtlSubscribeWnfStateChangeNotification"));
    g_pRtlUnsubscribeWnf = reinterpret_cast<PFN_RtlUnsubscribeWnfNotificationWaitForCompletion>(
        GetProcAddress(hNtdll, "RtlUnsubscribeWnfNotificationWaitForCompletion"));

    int initialCount = 0;
    ULONG changeStamp = 0;
    if (QueryWnfNotificationCount(&initialCount, &changeStamp)) {
        g_wnfCount.store(initialCount);
        Wh_Log(L"Initial WNF_SHEL_NOTIFICATIONS count: %d", initialCount);
    }

    if (g_pRtlSubscribeWnf) {
        NTSTATUS st = g_pRtlSubscribeWnf(
            &g_wnfSubscription,
            kWnfShelNotifications,
            changeStamp,
            OnWnfNotificationChanged,
            nullptr,
            nullptr,
            0,
            0);
        if (st < 0) {
            Wh_Log(L"RtlSubscribeWnfStateChangeNotification failed: 0x%08X", static_cast<unsigned>(st));
            g_wnfSubscription = nullptr;
        }
    }
}

static void CleanupWnf() {
    if (g_wnfSubscription && g_pRtlUnsubscribeWnf) {
        g_pRtlUnsubscribeWnf(g_wnfSubscription);
        g_wnfSubscription = nullptr;
    }
}

// ============================================================================
// Notification Center Database Reader (wpndatabase.db via winsqlite3.dll)
// ============================================================================

struct WpnDbReader {
    HMODULE hSqlite = nullptr;
    PFN_sqlite3_open_v2 open_v2 = nullptr;
    PFN_sqlite3_close close = nullptr;
    PFN_sqlite3_busy_timeout busy_timeout = nullptr;
    PFN_sqlite3_prepare_v2 prepare_v2 = nullptr;
    PFN_sqlite3_bind_int64 bind_int64 = nullptr;
    PFN_sqlite3_step step = nullptr;
    PFN_sqlite3_column_int column_int = nullptr;
    PFN_sqlite3_finalize finalize = nullptr;

    std::wstring dbPathW;
    std::wstring walPathW;
    std::string dbPathUtf8;

    uint64_t lastDbStamp = 0;
    uint64_t lastWalStamp = 0;
    ULONGLONG lastQueryTick = 0;
    int cachedCount = -1;

    bool Init() {
        wchar_t localAppData[MAX_PATH] = {};
        DWORD len = GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, MAX_PATH);
        if (len == 0 || len >= MAX_PATH) {
            Wh_Log(L"WpnDbReader: LOCALAPPDATA unavailable");
            return false;
        }

        dbPathW = std::wstring(localAppData) + L"\\Microsoft\\Windows\\Notifications\\wpndatabase.db";
        walPathW = dbPathW + L"-wal";
        dbPathUtf8 = WideToUtf8(dbPathW);

        hSqlite = LoadLibraryW(L"winsqlite3.dll");
        if (!hSqlite) {
            Wh_Log(L"WpnDbReader: LoadLibrary(winsqlite3.dll) failed: %lu", GetLastError());
            return false;
        }

        open_v2 = reinterpret_cast<PFN_sqlite3_open_v2>(GetProcAddress(hSqlite, "sqlite3_open_v2"));
        close = reinterpret_cast<PFN_sqlite3_close>(GetProcAddress(hSqlite, "sqlite3_close"));
        busy_timeout = reinterpret_cast<PFN_sqlite3_busy_timeout>(GetProcAddress(hSqlite, "sqlite3_busy_timeout"));
        prepare_v2 = reinterpret_cast<PFN_sqlite3_prepare_v2>(GetProcAddress(hSqlite, "sqlite3_prepare_v2"));
        bind_int64 = reinterpret_cast<PFN_sqlite3_bind_int64>(GetProcAddress(hSqlite, "sqlite3_bind_int64"));
        step = reinterpret_cast<PFN_sqlite3_step>(GetProcAddress(hSqlite, "sqlite3_step"));
        column_int = reinterpret_cast<PFN_sqlite3_column_int>(GetProcAddress(hSqlite, "sqlite3_column_int"));
        finalize = reinterpret_cast<PFN_sqlite3_finalize>(GetProcAddress(hSqlite, "sqlite3_finalize"));

        if (!open_v2 || !close || !prepare_v2 || !step || !column_int || !finalize) {
            Wh_Log(L"WpnDbReader: missing sqlite3 exports in winsqlite3.dll");
            Cleanup();
            return false;
        }

        Wh_Log(L"WpnDbReader initialized: %s", dbPathW.c_str());
        return true;
    }

    void Cleanup() {
        if (hSqlite) {
            FreeLibrary(hSqlite);
            hSqlite = nullptr;
        }
        open_v2 = nullptr;
        close = nullptr;
        busy_timeout = nullptr;
        prepare_v2 = nullptr;
        bind_int64 = nullptr;
        step = nullptr;
        column_int = nullptr;
        finalize = nullptr;
    }

    static uint64_t GetFileStamp(const std::wstring& path) {
        WIN32_FILE_ATTRIBUTE_DATA fad{};
        if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fad)) {
            return 0;
        }
        uint64_t t = (static_cast<uint64_t>(fad.ftLastWriteTime.dwHighDateTime) << 32) |
                     fad.ftLastWriteTime.dwLowDateTime;
        uint64_t sz = (static_cast<uint64_t>(fad.nFileSizeHigh) << 32) | fad.nFileSizeLow;
        return t ^ (sz * 1315423911ULL);
    }

    int GetActiveToastCount(bool forceRefresh) {
        if (!open_v2 || dbPathUtf8.empty()) {
            return -1;
        }

        ULONGLONG nowTick = GetTickCount64();
        uint64_t dbStamp = GetFileStamp(dbPathW);
        uint64_t walStamp = GetFileStamp(walPathW);

        bool changed = forceRefresh ||
                       cachedCount < 0 ||
                       dbStamp != lastDbStamp ||
                       walStamp != lastWalStamp ||
                       (nowTick - lastQueryTick) >= 2000;

        if (!changed) {
            return cachedCount;
        }

        sqlite3* db = nullptr;
        int rc = open_v2(dbPathUtf8.c_str(), &db, kSqliteOpenReadonly, nullptr);
        if (rc != kSqliteOk || !db) {
            if (db) {
                close(db);
            }
            return cachedCount;
        }

        if (busy_timeout) {
            busy_timeout(db, 40);
        }

        FILETIME ftNow{};
        GetSystemTimeAsFileTime(&ftNow);
        int64_t nowFileTime = static_cast<int64_t>(
            (static_cast<uint64_t>(ftNow.dwHighDateTime) << 32) | ftNow.dwLowDateTime);

        int count = -1;
        sqlite3_stmt* stmt = nullptr;

        const char* sqlWithExpiry =
            "SELECT COUNT(*) FROM Notification "
            "WHERE Type = 'toast' AND (ExpiryTime = 0 OR ExpiryTime > ?1);";

        if (prepare_v2(db, sqlWithExpiry, -1, &stmt, nullptr) == kSqliteOk && stmt) {
            if (bind_int64) {
                bind_int64(stmt, 1, nowFileTime);
            }
            if (step(stmt) == kSqliteRow) {
                count = column_int(stmt, 0);
            }
            finalize(stmt);
            stmt = nullptr;
        } else {
            const char* sqlSimple = "SELECT COUNT(*) FROM Notification WHERE Type = 'toast';";
            if (prepare_v2(db, sqlSimple, -1, &stmt, nullptr) == kSqliteOk && stmt) {
                if (step(stmt) == kSqliteRow) {
                    count = column_int(stmt, 0);
                }
                finalize(stmt);
                stmt = nullptr;
            }
        }

        close(db);

        if (count >= 0) {
            if (count != cachedCount) {
                Wh_Log(L"wpndatabase.db active toast count: %d", count);
            }
            cachedCount = count;
            lastDbStamp = dbStamp;
            lastWalStamp = walStamp;
            lastQueryTick = nowTick;
        }
        return cachedCount;
    }
};

// ============================================================================
// UI Automation + Taskbar Edge Fallback: Locate Windows 11 Notification Bell
// ============================================================================

struct TaskbarBellTracker {
    IUIAutomation* uia = nullptr;
    IUIAutomationCondition* condNotificationCenter = nullptr;
    IUIAutomationCondition* condButton = nullptr;
    IUIAutomationCondition* condText = nullptr;
    IUIAutomationCondition* condToolTip = nullptr;
    std::unordered_map<HWND, bool> tooltipPopupCache;
    IUIAutomationElement* cachedButton = nullptr;
    HWND cachedTrayHwnd = nullptr;
    int lastIsVertical = -1;
    bool loggedFallback = false;

    // Rate-limit full UIA tree scans to at most once every 3 seconds.
    ULONGLONG lastFullSearchTick = 0;

    // Cache the inner bell glyph rect so FindAll is only run when the button rect changes.
    bool glyphCacheValid = false;
    bool cachedHasGlyphRect = false;
    RECT cachedGlyphRect = {};
    RECT cachedGlyphButtonRect = {};

    bool Init() {
        HRESULT hr = CoCreateInstance(
            CLSID_CUIAutomation,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_IUIAutomation,
            reinterpret_cast<void**>(&uia));
        if (FAILED(hr) || !uia) {
            Wh_Log(L"CoCreateInstance(CUIAutomation) failed: 0x%08X", static_cast<unsigned>(hr));
            return false;
        }

        VARIANT varId;
        VariantInit(&varId);
        varId.vt = VT_BSTR;
        varId.bstrVal = SysAllocString(L"NotificationCenterButton");
        uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, varId, &condNotificationCenter);
        VariantClear(&varId);

        VARIANT varBtn;
        VariantInit(&varBtn);
        varBtn.vt = VT_I4;
        varBtn.lVal = UIA_ButtonControlTypeId;
        uia->CreatePropertyCondition(UIA_ControlTypePropertyId, varBtn, &condButton);
        VariantClear(&varBtn);

        VARIANT varTxt;
        VariantInit(&varTxt);
        varTxt.vt = VT_I4;
        varTxt.lVal = UIA_TextControlTypeId;
        uia->CreatePropertyCondition(UIA_ControlTypePropertyId, varTxt, &condText);
        VariantClear(&varTxt);

        VARIANT varTip;
        VariantInit(&varTip);
        varTip.vt = VT_I4;
        varTip.lVal = UIA_ToolTipControlTypeId;
        uia->CreatePropertyCondition(UIA_ControlTypePropertyId, varTip, &condToolTip);
        VariantClear(&varTip);

        return true;
    }

    void InvalidateGlyphCache() {
        glyphCacheValid = false;
        cachedHasGlyphRect = false;
        cachedGlyphRect = {};
        cachedGlyphButtonRect = {};
    }

    void ClearCache() {
        if (cachedButton) {
            cachedButton->Release();
            cachedButton = nullptr;
        }
        cachedTrayHwnd = nullptr;
        InvalidateGlyphCache();
    }

    void Cleanup() {
        ClearCache();
        if (condNotificationCenter) {
            condNotificationCenter->Release();
            condNotificationCenter = nullptr;
        }
        if (condButton) {
            condButton->Release();
            condButton = nullptr;
        }
        if (condText) {
            condText->Release();
            condText = nullptr;
        }
        if (condToolTip) {
            condToolTip->Release();
            condToolTip = nullptr;
        }
        tooltipPopupCache.clear();
        if (uia) {
            uia->Release();
            uia = nullptr;
        }
    }

    bool IsPopupHostingToolTip(HWND popupHwnd) {
        if (!popupHwnd || !IsWindow(popupHwnd)) {
            return false;
        }
        auto it = tooltipPopupCache.find(popupHwnd);
        if (it != tooltipPopupCache.end()) {
            return it->second;
        }

        // Prune destroyed HWNDs occasionally to keep the cache bounded.
        if (tooltipPopupCache.size() > 64) {
            for (auto iter = tooltipPopupCache.begin(); iter != tooltipPopupCache.end(); ) {
                if (!IsWindow(iter->first)) {
                    iter = tooltipPopupCache.erase(iter);
                } else {
                    ++iter;
                }
            }
        }

        bool isTooltip = false;
        if (uia && condToolTip) {
            IUIAutomationElement* rootElem = nullptr;
            if (SUCCEEDED(uia->ElementFromHandle(popupHwnd, &rootElem)) && rootElem) {
                IUIAutomationElement* tipElem = nullptr;
                if (SUCCEEDED(rootElem->FindFirst(TreeScope_Subtree, condToolTip, &tipElem)) && tipElem) {
                    isTooltip = true;
                    tipElem->Release();
                }
                rootElem->Release();
            }
        }

        tooltipPopupCache[popupHwnd] = isTooltip;
        return isTooltip;
    }

    static BOOL CALLBACK CollectChildHwndsProc(HWND hwnd, LPARAM lParam) {
        auto* list = reinterpret_cast<std::vector<HWND>*>(lParam);
        wchar_t cls[96] = {};
        if (GetClassNameW(hwnd, cls, 96) > 0) {
            if (wcscmp(cls, L"Windows.UI.Input.InputSite.WindowClass") == 0 ||
                wcscmp(cls, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0) {
                list->push_back(hwnd);
            }
        }
        return TRUE;
    }

    IUIAutomationElement* FindButtonInRoot(IUIAutomationElement* root, bool isVertical, float dpiScale) {
        if (!root) {
            return nullptr;
        }

        if (condNotificationCenter) {
            IUIAutomationElement* exact = nullptr;
            if (SUCCEEDED(root->FindFirst(TreeScope_Descendants, condNotificationCenter, &exact)) && exact) {
                Wh_Log(L"Found NotificationCenterButton via exact AutomationId");
                return exact;
            }
        }

        if (!condButton) {
            return nullptr;
        }

        IUIAutomationElementArray* arr = nullptr;
        if (FAILED(root->FindAll(TreeScope_Descendants, condButton, &arr)) || !arr) {
            return nullptr;
        }

        int count = 0;
        arr->get_Length(&count);

        IUIAutomationElement* bestMatch = nullptr;
        int bestScore = -1;
        long bestEdgePos = -1000000;
        std::wstring bestDebugInfo;
        int minPrimaryDim = static_cast<int>(std::lround(22.0f * dpiScale));

        for (int i = 0; i < count; ++i) {
            IUIAutomationElement* elem = nullptr;
            if (FAILED(arr->GetElement(i, &elem)) || !elem) {
                continue;
            }

            RECT rc{};
            BOOL isOffscreen = FALSE;
            if (FAILED(elem->get_CurrentBoundingRectangle(&rc)) ||
                FAILED(elem->get_CurrentIsOffscreen(&isOffscreen)) ||
                isOffscreen) {
                elem->Release();
                continue;
            }

            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;

            if (w < 12 || h < 12 || (!isVertical && w < minPrimaryDim) || (isVertical && h < minPrimaryDim)) {
                elem->Release();
                continue;
            }

            BSTR autoId = nullptr;
            BSTR clsName = nullptr;
            BSTR name = nullptr;
            elem->get_CurrentAutomationId(&autoId);
            elem->get_CurrentClassName(&clsName);
            elem->get_CurrentName(&name);

            std::wstring autoIdL = ToLowerW(autoId);
            std::wstring clsNameL = ToLowerW(clsName);
            std::wstring nameL = ToLowerW(name);

            bool isExcluded =
                ContainsI(autoIdL, L"showdesktop") ||
                ContainsI(autoIdL, L"controlcenter") ||
                ContainsI(autoIdL, L"peek") ||
                ContainsI(nameL, L"show desktop") ||
                ContainsI(nameL, L"hiển thị màn hình nền");

            bool hasNotifText =
                ContainsI(nameL, L"notification") ||
                ContainsI(nameL, L"thông báo") ||
                ContainsI(nameL, L"thong bao");
            bool hasClockText =
                ContainsI(nameL, L":") ||
                ContainsI(nameL, L"system clock") ||
                ContainsI(nameL, L"đồng hồ");

            int score = 0;
            if (!isExcluded) {
                if (ContainsI(autoIdL, L"notificationcenter")) {
                    score = 120;
                } else if (hasNotifText && !hasClockText) {
                    score = 110;
                } else if (hasNotifText && hasClockText) {
                    score = 95;
                } else if (ContainsI(clsNameL, L"omnibutton")) {
                    score = 75;
                } else if (autoIdL == L"systemtrayicon") {
                    score = 65;
                } else if (hasClockText) {
                    score = 50;
                }
            }

            long edgePos = isVertical ? rc.bottom : rc.right;

            if (score > 0 && (score > bestScore || (score == bestScore && edgePos > bestEdgePos))) {
                if (bestMatch) {
                    bestMatch->Release();
                }
                bestMatch = elem;
                bestMatch->AddRef();
                bestScore = score;
                bestEdgePos = edgePos;
                bestDebugInfo = L"id=" + autoIdL + L" cls=" + clsNameL + L" name=" + nameL;
            }

            if (autoId) SysFreeString(autoId);
            if (clsName) SysFreeString(clsName);
            if (name) SysFreeString(name);
            elem->Release();
        }

        arr->Release();
        if (bestMatch) {
            Wh_Log(L"Found notification button via UIA scan (score=%d, %s)", bestScore, bestDebugInfo.c_str());
        }
        return bestMatch;
    }

    bool FindBellGlyphRect(IUIAutomationElement* button, float dpiScale, RECT* outGlyphRect) {
        if (!button || !condText || !outGlyphRect) {
            return false;
        }

        IUIAutomationElementArray* arr = nullptr;
        if (FAILED(button->FindAll(TreeScope_Descendants, condText, &arr)) || !arr) {
            return false;
        }

        int count = 0;
        arr->get_Length(&count);

        bool found = false;
        RECT bestRect{};
        long bestRight = -1000000;
        int maxGlyphDim = static_cast<int>(std::lround(32.0f * dpiScale));

        for (int i = 0; i < count; ++i) {
            IUIAutomationElement* elem = nullptr;
            if (FAILED(arr->GetElement(i, &elem)) || !elem) {
                continue;
            }

            RECT rc{};
            BOOL isOffscreen = FALSE;
            if (SUCCEEDED(elem->get_CurrentBoundingRectangle(&rc)) &&
                SUCCEEDED(elem->get_CurrentIsOffscreen(&isOffscreen)) &&
                !isOffscreen) {
                int w = rc.right - rc.left;
                int h = rc.bottom - rc.top;
                if (w >= 6 && h >= 6 && w <= maxGlyphDim && h <= maxGlyphDim) {
                    BSTR name = nullptr;
                    elem->get_CurrentName(&name);
                    bool isClockText = false;
                    if (name) {
                        size_t len = wcslen(name);
                        if (len > 2 || wcschr(name, L':') || wcschr(name, L'/')) {
                            isClockText = true;
                        }
                        SysFreeString(name);
                    }
                    if (!isClockText && rc.right > bestRight) {
                        bestRect = rc;
                        bestRight = rc.right;
                        found = true;
                    }
                }
            }
            elem->Release();
        }

        arr->Release();
        if (found) {
            *outGlyphRect = bestRect;
        }
        return found;
    }

    bool GetBellAnchor(
        HWND* outTrayHwnd,
        RECT* outRect,
        RECT* outGlyphRect,
        bool* outHasGlyphRect,
        bool* outIsVertical,
        bool* outUsedFallback) {
        if (outHasGlyphRect) {
            *outHasGlyphRect = false;
        }

        HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (outTrayHwnd) {
            *outTrayHwnd = hTray;
        }
        if (!hTray || !IsWindowVisible(hTray)) {
            ClearCache();
            return false;
        }

        RECT trayRect{};
        if (!GetWindowRect(hTray, &trayRect)) {
            return false;
        }

        HMONITOR hMon = MonitorFromWindow(hTray, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfoW(hMon, &mi)) {
            RECT intersect{};
            if (!IntersectRect(&intersect, &trayRect, &mi.rcMonitor) ||
                (intersect.right - intersect.left) < 12 ||
                (intersect.bottom - intersect.top) < 12) {
                return false;
            }
        }

        int trayW = trayRect.right - trayRect.left;
        int trayH = trayRect.bottom - trayRect.top;
        bool isVertical = (trayH > trayW);
        if (outIsVertical) {
            *outIsVertical = isVertical;
        }

        UINT dpi = GetDpiForWindow(hTray);
        if (dpi == 0) dpi = 96;
        float scale = dpi / 96.0f;

        int orientVal = isVertical ? 1 : 0;
        bool trayOrOrientChanged = (hTray != cachedTrayHwnd || lastIsVertical != orientVal);
        if (trayOrOrientChanged) {
            ClearCache();
            cachedTrayHwnd = hTray;
            lastIsVertical = orientVal;
            lastFullSearchTick = 0;
            loggedFallback = false;
        }

        ULONGLONG nowTick = GetTickCount64();
        if (!cachedButton && uia &&
            (lastFullSearchTick == 0 || (nowTick - lastFullSearchTick) >= kUiaFullSearchCooldownMs)) {
            lastFullSearchTick = nowTick;

            std::vector<HWND> roots;
            EnumChildWindows(hTray, CollectChildHwndsProc, reinterpret_cast<LPARAM>(&roots));
            roots.push_back(hTray);

            for (HWND rootHwnd : roots) {
                IUIAutomationElement* rootElem = nullptr;
                if (SUCCEEDED(uia->ElementFromHandle(rootHwnd, &rootElem)) && rootElem) {
                    cachedButton = FindButtonInRoot(rootElem, isVertical, scale);
                    rootElem->Release();
                    if (cachedButton) {
                        InvalidateGlyphCache();
                        loggedFallback = false;
                        break;
                    }
                }
            }
        }

        if (cachedButton) {
            BOOL isOffscreen = FALSE;
            RECT rc{};
            if (SUCCEEDED(cachedButton->get_CurrentIsOffscreen(&isOffscreen)) &&
                SUCCEEDED(cachedButton->get_CurrentBoundingRectangle(&rc)) &&
                !isOffscreen &&
                (rc.right - rc.left) >= 8 &&
                (rc.bottom - rc.top) >= 8) {
                if (outRect) *outRect = rc;
                if (outUsedFallback) *outUsedFallback = false;

                if (!isVertical && outGlyphRect && outHasGlyphRect) {
                    if (!glyphCacheValid || !RectsEqual(rc, cachedGlyphButtonRect)) {
                        RECT glyphRc{};
                        cachedHasGlyphRect = FindBellGlyphRect(cachedButton, scale, &glyphRc);
                        cachedGlyphRect = glyphRc;
                        cachedGlyphButtonRect = rc;
                        glyphCacheValid = true;
                    }
                    if (cachedHasGlyphRect) {
                        *outGlyphRect = cachedGlyphRect;
                        *outHasGlyphRect = true;
                    }
                }
                return true;
            }
            ClearCache();
        }

        if (!loggedFallback) {
            Wh_Log(L"UIA button not found; using Shell_TrayWnd edge fallback (%dx%d, vertical=%d)",
                   trayW, trayH, isVertical ? 1 : 0);
            loggedFallback = true;
        }

        RECT fb{};
        if (isVertical) {
            fb.left = trayRect.left;
            fb.right = trayRect.right;
            fb.bottom = trayRect.bottom - static_cast<int>(std::lround(4.0f * scale));
            fb.top = fb.bottom - static_cast<int>(std::lround(32.0f * scale));
        } else {
            fb.right = trayRect.right - static_cast<int>(std::lround(16.0f * scale));
            fb.left = fb.right - static_cast<int>(std::lround(32.0f * scale));
            fb.top = trayRect.top;
            fb.bottom = trayRect.bottom;
        }
        if (outRect) *outRect = fb;
        if (outUsedFallback) *outUsedFallback = true;
        return true;
    }
};

// ============================================================================
// True Fullscreen Window Detection (excludes maximized normal windows)
// ============================================================================

static bool IsTrueFullscreenAppCoveringMonitor(HWND hTray) {
    HWND hFg = GetForegroundWindow();
    if (!hFg || hFg == hTray || !IsWindowVisible(hFg)) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hFg, GWL_STYLE);
    if ((style & WS_CAPTION) == WS_CAPTION || (style & WS_THICKFRAME) == WS_THICKFRAME) {
        return false;
    }

    wchar_t cls[64] = {};
    if (GetClassNameW(hFg, cls, 64) > 0) {
        if (wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0 ||
            wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
            wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
            wcscmp(cls, L"ControlCenterWindow") == 0) {
            return false;
        }
    }

    HMONITOR trayMon = MonitorFromWindow(hTray, MONITOR_DEFAULTTONEAREST);
    HMONITOR fgMon = MonitorFromWindow(hFg, MONITOR_DEFAULTTONEAREST);
    if (trayMon != fgMon) {
        return false;
    }

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(trayMon, &mi)) {
        return false;
    }

    RECT fgRect{};
    if (FAILED(DwmGetWindowAttribute(hFg, DWMWA_EXTENDED_FRAME_BOUNDS, &fgRect, sizeof(fgRect)))) {
        if (!GetWindowRect(hFg, &fgRect)) {
            return false;
        }
    }

    return fgRect.left <= mi.rcMonitor.left &&
           fgRect.top <= mi.rcMonitor.top &&
           fgRect.right >= mi.rcMonitor.right &&
           fgRect.bottom >= mi.rcMonitor.bottom;
}

// ============================================================================
// GDI+ Premultiplied ARGB Badge Renderer (PixelFormat32bppPARGB)
// ============================================================================

static void AddRoundedRectPath(Gdiplus::GraphicsPath& path, float x, float y, float w, float h, float radius) {
    float d = radius * 2.0f;
    if (d > w) d = w;
    if (d > h) d = h;
    path.AddArc(x, y, d, d, 180.0f, 90.0f);
    path.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
    path.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
    path.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

static bool RenderBadgeToLayeredWindow(
    HWND hwnd,
    int targetCenterX,
    int targetCenterY,
    int displayCount,
    float dpiScale,
    const ModSettings& s) {
    std::wstring label;
    if (displayCount > s.maxCount) {
        label = std::to_wstring(s.maxCount) + L"+";
    } else {
        label = std::to_wstring(displayCount);
    }

    COLORREF fillRef = s.useAccentColor ? GetWindowsAccentColor(s.badgeColor) : s.badgeColor;
    COLORREF textRef = s.useAccentColor ? GetContrastingTextColor(fillRef) : s.textColor;

    float diameter = std::max(8.0f, s.badgeSize * dpiScale);
    float borderPx = s.borderWidth * dpiScale;
    float fontPx = std::max(6.0f, s.fontSize * (96.0f / 72.0f) * dpiScale);

    Gdiplus::FontFamily primaryFamily(s.fontFamily.c_str());
    Gdiplus::FontFamily fallbackFamily(L"Segoe UI");
    const Gdiplus::FontFamily* activeFamily =
        (primaryFamily.GetLastStatus() == Gdiplus::Ok) ? &primaryFamily : &fallbackFamily;

    Gdiplus::Font font(activeFamily, fontPx, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return false;
    }

    float textWidth = 0.0f;
    {
        Gdiplus::Bitmap measureBmp(1, 1, PixelFormat32bppPARGB);
        Gdiplus::Graphics measureG(&measureBmp);
        measureG.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
        fmt.SetFormatFlags(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
        Gdiplus::RectF bounds;
        measureG.MeasureString(label.c_str(), -1, &font, Gdiplus::PointF(0.0f, 0.0f), &fmt, &bounds);
        textWidth = bounds.Width;
    }

    float horizPad = std::max(4.0f * dpiScale, diameter * 0.30f);
    float badgeW = (label.size() <= 1) ? diameter : std::max(diameter, std::ceil(textWidth + horizPad * 2.0f));
    float badgeH = diameter;

    int margin = static_cast<int>(std::ceil(borderPx)) + 2;
    int bmpW = static_cast<int>(std::ceil(badgeW)) + margin * 2;
    int bmpH = static_cast<int>(std::ceil(badgeH)) + margin * 2;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bmpW;
    bmi.bmiHeader.biHeight = -bmpH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(screenDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBmp || !bits) {
        ReleaseDC(nullptr, screenDc);
        return false;
    }

    memset(bits, 0, static_cast<size_t>(bmpW) * static_cast<size_t>(bmpH) * 4);

    {
        Gdiplus::Bitmap targetBmp(
            bmpW,
            bmpH,
            bmpW * 4,
            PixelFormat32bppPARGB,
            static_cast<BYTE*>(bits));
        Gdiplus::Graphics g(&targetBmp);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));

        float x = static_cast<float>(margin);
        float y = static_cast<float>(margin);
        float radius = badgeH * 0.5f;

        Gdiplus::GraphicsPath path;
        AddRoundedRectPath(path, x, y, badgeW, badgeH, radius);

        Gdiplus::SolidBrush fillBrush(
            Gdiplus::Color(255, GetRValue(fillRef), GetGValue(fillRef), GetBValue(fillRef)));
        g.FillPath(&fillBrush, &path);

        if (borderPx > 0.1f) {
            Gdiplus::Pen borderPen(
                Gdiplus::Color(220, GetRValue(s.borderColor), GetGValue(s.borderColor), GetBValue(s.borderColor)),
                borderPx);
            g.DrawPath(&borderPen, &path);
        }

        Gdiplus::SolidBrush textBrush(
            Gdiplus::Color(255, GetRValue(textRef), GetGValue(textRef), GetBValue(textRef)));

        Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
        fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
        fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        Gdiplus::RectF textRect(x, y - 0.35f * dpiScale, badgeW, badgeH);
        g.DrawString(label.c_str(), -1, &font, textRect, &fmt, &textBrush);
    }

    HDC memDc = CreateCompatibleDC(screenDc);
    HGDIOBJ oldBmp = SelectObject(memDc, hBmp);

    POINT ptDst = {
        targetCenterX - bmpW / 2,
        targetCenterY - bmpH / 2,
    };
    SIZE szWnd = {bmpW, bmpH};
    POINT ptSrc = {0, 0};

    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    BOOL ok = UpdateLayeredWindow(hwnd, screenDc, &ptDst, &szWnd, memDc, &ptSrc, 0, &bf, ULW_ALPHA);

    SelectObject(memDc, oldBmp);
    DeleteDC(memDc);
    DeleteObject(hBmp);
    ReleaseDC(nullptr, screenDc);

    if (ok) {
        SetWindowPos(
            hwnd,
            HWND_TOPMOST,
            ptDst.x,
            ptDst.y,
            bmpW,
            bmpH,
            SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    return ok != FALSE;
}

// ============================================================================
// Overlay Window & Sync Loop
// ============================================================================


static bool SampleTaskbarIsLight(HWND hTray, const RECT& btnRect, bool* outIsLight) {
    if (!hTray || !IsWindow(hTray) || !outIsLight) {
        return false;
    }

    RECT rcTray{};
    if (!GetWindowRect(hTray, &rcTray)) {
        return false;
    }
    int trayW = rcTray.right - rcTray.left;
    int trayH = rcTray.bottom - rcTray.top;
    if (trayW <= 8 || trayH <= 8) {
        return false;
    }

    // Sample a small 16x4 strip near the top/inner margin of the taskbar around btnRect
    // (avoids white clock/bell glyphs and captures the actual rendered taskbar background).
    int sampleW = 16;
    int sampleH = 4;
    int sx = (btnRect.left + btnRect.right) / 2 - sampleW / 2;
    sx = std::clamp(sx, static_cast<int>(rcTray.left + 2), static_cast<int>(rcTray.right - sampleW - 2));
    int sy = rcTray.top + 3;
    if (trayH > trayW) {
        // Vertical taskbar: sample near left interior margin around btnRect.top
        sx = rcTray.left + 3;
        sy = std::clamp(static_cast<int>(btnRect.top + 2),
                        static_cast<int>(rcTray.top + 2),
                        static_cast<int>(rcTray.bottom - sampleH - 2));
    }

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return false;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = sampleW;
    bmi.bmiHeader.biHeight = -sampleH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(screenDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBmp || !bits) {
        ReleaseDC(nullptr, screenDc);
        return false;
    }

    HDC memDc = CreateCompatibleDC(screenDc);
    HGDIOBJ oldBmp = SelectObject(memDc, hBmp);
    BOOL ok = BitBlt(memDc, 0, 0, sampleW, sampleH, screenDc, sx, sy, SRCCOPY);
    SelectObject(memDc, oldBmp);
    DeleteDC(memDc);
    ReleaseDC(nullptr, screenDc);

    if (!ok) {
        DeleteObject(hBmp);
        return false;
    }

    std::vector<int> lums;
    lums.reserve(static_cast<size_t>(sampleW * sampleH));
    const BYTE* px = static_cast<const BYTE*>(bits);
    for (int i = 0; i < sampleW * sampleH; ++i) {
        int b = px[i * 4 + 0];
        int g = px[i * 4 + 1];
        int r = px[i * 4 + 2];
        int lum = (r * 2126 + g * 7152 + b * 722) / 10000;
        lums.push_back(lum);
    }
    DeleteObject(hBmp);

    if (lums.empty()) {
        return false;
    }
    std::nth_element(lums.begin(), lums.begin() + lums.size() / 2, lums.end());
    int medianLum = lums[lums.size() / 2];
    *outIsLight = (medianLum >= 145);
    return true;
}

static bool ResolveTooltipLightTheme(TooltipTheme mode, HWND hTray, const RECT& btnRect) {
    if (mode == TooltipTheme::Dark) {
        return false;
    }
    if (mode == TooltipTheme::Light) {
        return true;
    }

    // 1. Prefer sampling the actual rendered taskbar background so custom themes
    //    and taskbar styling mods match Windows' visible taskbar tooltip 100%.
    bool sampledLight = false;
    if (SampleTaskbarIsLight(hTray, btnRect, &sampledLight)) {
        return sampledLight;
    }

    // 2. Fallback to Windows registry (64-bit view).
    HKEY hKey = nullptr;
    DWORD sysLight = 0;
    DWORD colorPrevalence = 0;
    bool hasSys = false;
    bool hasColorPrev = false;

    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0,
            KEY_READ | KEY_WOW64_64KEY,
            &hKey) == ERROR_SUCCESS &&
        hKey) {
        DWORD type = 0;
        DWORD size = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"SystemUsesLightTheme", nullptr, &type,
                             reinterpret_cast<LPBYTE>(&sysLight), &size) == ERROR_SUCCESS &&
            type == REG_DWORD) {
            hasSys = true;
        }
        type = 0;
        size = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"ColorPrevalence", nullptr, &type,
                             reinterpret_cast<LPBYTE>(&colorPrevalence), &size) == ERROR_SUCCESS &&
            type == REG_DWORD) {
            hasColorPrev = true;
        }
        RegCloseKey(hKey);
    }

    if (hasColorPrev && colorPrevalence != 0) {
        return false;
    }
    return hasSys && (sysLight != 0);
}

static std::wstring FormatHoverTooltipText(int count) {
    LANGID langId = GetUserDefaultUILanguage();
    if (PRIMARYLANGID(langId) == LANG_VIETNAMESE) {
        return std::to_wstring(count) + L" th\x00F4ng b\x00E1o hi\x1EC7n c\x00F3";
    }
    if (count == 1) {
        return L"1 active notification";
    }
    return std::to_wstring(count) + L" active notifications";
}

struct NativeTooltipSearchCtx {
    TaskbarBellTracker* tracker = nullptr;
    DWORD trayPid = 0;
    HWND trayHwnd = nullptr;
    RECT anchorRect{};
    int maxW = 0;
    int maxH = 0;
    int maxDistX = 0;
    int maxDistY = 0;
    bool hidAny = false;
};

static BOOL CALLBACK SuppressNativeTooltipEnumProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<NativeTooltipSearchCtx*>(lParam);
    if (!ctx || !ctx->tracker || !hwnd || hwnd == ctx->trayHwnd || !IsWindowVisible(hwnd)) {
        return TRUE;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != ctx->trayPid) {
        return TRUE;
    }

    wchar_t cls[64] = {};
    if (GetClassNameW(hwnd, cls, 64) == 0) {
        return TRUE;
    }
    if (wcscmp(cls, L"Xaml_WindowedPopupClass") != 0 &&
        wcscmp(cls, L"tooltips_class32") != 0) {
        return TRUE;
    }

    RECT r{};
    if (!GetWindowRect(hwnd, &r)) {
        return TRUE;
    }
    int w = r.right - r.left;
    int h = r.bottom - r.top;
    if (w < 30 || w > ctx->maxW || h < 16 || h > ctx->maxH) {
        return TRUE;
    }

    int dx = 0;
    if (r.right < ctx->anchorRect.left) {
        dx = ctx->anchorRect.left - r.right;
    } else if (r.left > ctx->anchorRect.right) {
        dx = r.left - ctx->anchorRect.right;
    }
    int dy = 0;
    if (r.bottom < ctx->anchorRect.top) {
        dy = ctx->anchorRect.top - r.bottom;
    } else if (r.top > ctx->anchorRect.bottom) {
        dy = r.top - ctx->anchorRect.bottom;
    }

    if (dx <= ctx->maxDistX && dy <= ctx->maxDistY) {
        // Confirm via cached UIA check that this popup actually hosts a ToolTip
        // (never hide context menus or other Explorer popups).
        if (ctx->tracker->IsPopupHostingToolTip(hwnd)) {
            ShowWindowAsync(hwnd, SW_HIDE);
            ctx->hidAny = true;
        }
    }
    return TRUE;
}

static bool SuppressNativeTaskbarTooltip(
    TaskbarBellTracker* tracker,
    HWND hTray,
    const RECT& anchorRect,
    float dpiScale) {
    if (!tracker || !hTray || !IsWindow(hTray)) {
        return false;
    }
    DWORD trayPid = 0;
    GetWindowThreadProcessId(hTray, &trayPid);
    if (trayPid == 0) {
        return false;
    }

    NativeTooltipSearchCtx ctx;
    ctx.tracker = tracker;
    ctx.trayPid = trayPid;
    ctx.trayHwnd = hTray;
    ctx.anchorRect = anchorRect;
    ctx.maxW = static_cast<int>(std::lround(460.0f * dpiScale));
    ctx.maxH = static_cast<int>(std::lround(96.0f * dpiScale));
    ctx.maxDistX = static_cast<int>(std::lround(180.0f * dpiScale));
    ctx.maxDistY = static_cast<int>(std::lround(140.0f * dpiScale));

    EnumWindows(SuppressNativeTooltipEnumProc, reinterpret_cast<LPARAM>(&ctx));
    return ctx.hidAny;
}

static bool RenderTooltipToLayeredWindow(
    HWND tooltipHwnd,
    HWND hTray,
    const RECT& hoverRect,
    int count,
    float dpiScale,
    bool lightTheme) {
    if (!tooltipHwnd || !IsWindow(tooltipHwnd) || !hTray || !IsWindow(hTray)) {
        return false;
    }

    std::wstring text = FormatHoverTooltipText(count);

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return false;
    }

    Gdiplus::FontFamily family(L"Segoe UI Variable Text");
    const Gdiplus::FontFamily* activeFamily = &family;
    Gdiplus::FontFamily fallbackFamily(L"Segoe UI");
    if (family.GetLastStatus() != Gdiplus::Ok) {
        activeFamily = &fallbackFamily;
    }

    float fontPx = 12.0f * dpiScale;
    Gdiplus::Font font(activeFamily, fontPx, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

    Gdiplus::RectF textBounds(0, 0, 0, 0);
    {
        Gdiplus::Graphics measureG(screenDc);
        measureG.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        Gdiplus::StringFormat measureFmt(Gdiplus::StringFormat::GenericTypographic());
        measureFmt.SetFormatFlags(Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
        measureG.MeasureString(text.c_str(), -1, &font, Gdiplus::PointF(0, 0), &measureFmt, &textBounds);
    }

    float padX = 10.0f * dpiScale;
    float padY = 6.0f * dpiScale;
    float boxW = std::ceil(textBounds.Width + padX * 2.0f);
    float boxH = std::ceil(std::max(textBounds.Height + padY * 2.0f, 30.0f * dpiScale));
    int margin = static_cast<int>(std::ceil(5.0f * dpiScale));
    int bmpW = static_cast<int>(boxW) + margin * 2;
    int bmpH = static_cast<int>(boxH) + margin * 2;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bmpW;
    bmi.bmiHeader.biHeight = -bmpH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(screenDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBmp || !bits) {
        ReleaseDC(nullptr, screenDc);
        return false;
    }

    std::memset(bits, 0, static_cast<size_t>(bmpW) * static_cast<size_t>(bmpH) * 4);

    {
        Gdiplus::Bitmap targetBmp(
            bmpW,
            bmpH,
            bmpW * 4,
            PixelFormat32bppPARGB,
            static_cast<BYTE*>(bits));
        Gdiplus::Graphics g(&targetBmp);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));

        float x = static_cast<float>(margin);
        float y = static_cast<float>(margin);
        float radius = 4.5f * dpiScale;

        // Soft Windows 11 flyout drop shadow
        Gdiplus::GraphicsPath shadowPath;
        AddRoundedRectPath(shadowPath, x, y + 2.0f * dpiScale, boxW, boxH, radius);
        Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(lightTheme ? 34 : 96, 0, 0, 0));
        g.FillPath(&shadowBrush, &shadowPath);

        Gdiplus::GraphicsPath boxPath;
        AddRoundedRectPath(boxPath, x, y, boxW, boxH, radius);

        Gdiplus::SolidBrush fillBrush(lightTheme
            ? Gdiplus::Color(252, 249, 249, 249)
            : Gdiplus::Color(252, 44, 44, 44));
        g.FillPath(&fillBrush, &boxPath);

        Gdiplus::Pen borderPen(
            lightTheme
                ? Gdiplus::Color(215, 204, 204, 204)
                : Gdiplus::Color(220, 66, 66, 66),
            std::max(1.0f, dpiScale));
        g.DrawPath(&borderPen, &boxPath);

        Gdiplus::SolidBrush textBrush(lightTheme
            ? Gdiplus::Color(255, 26, 26, 26)
            : Gdiplus::Color(255, 243, 243, 243));
        Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
        fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
        fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        Gdiplus::RectF textRect(x, y - 0.35f * dpiScale, boxW, boxH);
        g.DrawString(text.c_str(), -1, &font, textRect, &fmt, &textBrush);
    }

    RECT rcTray{};
    GetWindowRect(hTray, &rcTray);

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    HMONITOR hMon = MonitorFromWindow(hTray, MONITOR_DEFAULTTONEAREST);
    if (!GetMonitorInfoW(hMon, &mi)) {
        mi.rcMonitor = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    }

    int trayW = rcTray.right - rcTray.left;
    int trayH = rcTray.bottom - rcTray.top;
    int gap = static_cast<int>(std::lround(8.0f * dpiScale));
    int edgePad = static_cast<int>(std::lround(8.0f * dpiScale));

    int dstX = (hoverRect.left + hoverRect.right - bmpW) / 2;
    int dstY = rcTray.top - bmpH - gap + margin;

    if (trayH > trayW) {
        // Vertical taskbar (Left or Right)
        int monMidX = (mi.rcMonitor.left + mi.rcMonitor.right) / 2;
        int trayMidX = (rcTray.left + rcTray.right) / 2;
        dstY = (hoverRect.top + hoverRect.bottom - bmpH) / 2;
        if (trayMidX < monMidX) {
            dstX = rcTray.right + gap - margin;
        } else {
            dstX = rcTray.left - bmpW - gap + margin;
        }
    } else {
        // Horizontal taskbar (Top or Bottom)
        int monMidY = (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
        int trayMidY = (rcTray.top + rcTray.bottom) / 2;
        if (trayMidY < monMidY) {
            dstY = rcTray.bottom + gap - margin;
        } else {
            dstY = rcTray.top - bmpH - gap + margin;
        }
    }

    int minX = mi.rcMonitor.left + edgePad - margin;
    int maxX = mi.rcMonitor.right - edgePad - bmpW + margin;
    int minY = mi.rcMonitor.top + edgePad - margin;
    int maxY = mi.rcMonitor.bottom - edgePad - bmpH + margin;
    if (maxX >= minX) {
        dstX = std::clamp(dstX, minX, maxX);
    }
    if (maxY >= minY) {
        dstY = std::clamp(dstY, minY, maxY);
    }

    HDC memDc = CreateCompatibleDC(screenDc);
    HGDIOBJ oldBmp = SelectObject(memDc, hBmp);

    POINT ptDst = {dstX, dstY};
    SIZE szWnd = {bmpW, bmpH};
    POINT ptSrc = {0, 0};

    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    BOOL ok = UpdateLayeredWindow(tooltipHwnd, screenDc, &ptDst, &szWnd, memDc, &ptSrc, 0, &bf, ULW_ALPHA);

    SelectObject(memDc, oldBmp);
    DeleteDC(memDc);
    DeleteObject(hBmp);
    ReleaseDC(nullptr, screenDc);

    if (ok) {
        SetWindowPos(
            tooltipHwnd,
            HWND_TOPMOST,
            ptDst.x,
            ptDst.y,
            bmpW,
            bmpH,
            SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    return ok != FALSE;
}

struct OverlayState {
    TaskbarBellTracker tracker;
    WpnDbReader dbReader;
    HWND tooltipHwnd = nullptr;
    bool visible = false;
    int lastRenderedCount = -1;
    int lastCenterX = 0;
    int lastCenterY = 0;
    UINT lastDpi = 0;
    bool forceRedraw = true;

    // Custom hover tooltip state (updated via lightweight cursor check, no UIA).
    HWND lastTrayHwnd = nullptr;
    RECT lastBtnRect = {};
    RECT bellHoverRect = {};
    float lastDpiScale = 1.0f;
    bool hoveringBell = false;
    ULONGLONG hoverStartTick = 0;
    bool tooltipVisible = false;
    int lastTooltipCount = -1;
    bool lastTooltipLightTheme = false;
};

static void HideCustomTooltip(OverlayState* state) {
    if (!state) {
        return;
    }
    state->hoveringBell = false;
    state->hoverStartTick = 0;
    if (state->tooltipVisible && state->tooltipHwnd && IsWindow(state->tooltipHwnd)) {
        ShowWindow(state->tooltipHwnd, SW_HIDE);
    }
    state->tooltipVisible = false;
}

static void UpdateHoverTooltip(OverlayState* state) {
    if (!state || g_stopping.load() || !state->tooltipHwnd || !IsWindow(state->tooltipHwnd)) {
        return;
    }

    ModSettings s = GetSettingsSnapshot();
    if (!s.customTooltip || !state->visible || state->lastRenderedCount <= 0 ||
        IsRectEmpty(&state->bellHoverRect) || !state->lastTrayHwnd || !IsWindow(state->lastTrayHwnd)) {
        HideCustomTooltip(state);
        return;
    }

    POINT pt{};
    if (!GetCursorPos(&pt) || !PtInRect(&state->bellHoverRect, pt)) {
        HideCustomTooltip(state);
        return;
    }

    // Temporarily hide Windows' native "No new notifications" tooltip while hovering the bell.
    bool hidNative = SuppressNativeTaskbarTooltip(
        &state->tracker, state->lastTrayHwnd, state->lastBtnRect, state->lastDpiScale);

    ULONGLONG now = GetTickCount64();
    if (!state->hoveringBell) {
        state->hoveringBell = true;
        state->hoverStartTick = now;
    }

    if (!hidNative && (now - state->hoverStartTick) < kHoverShowDelayMs) {
        return;
    }

    bool lightTheme = ResolveTooltipLightTheme(s.tooltipTheme, state->lastTrayHwnd, state->lastBtnRect);
    bool needRender =
        !state->tooltipVisible ||
        state->lastTooltipCount != state->lastRenderedCount ||
        state->lastTooltipLightTheme != lightTheme;

    if (needRender) {
        if (RenderTooltipToLayeredWindow(
                state->tooltipHwnd,
                state->lastTrayHwnd,
                state->bellHoverRect,
                state->lastRenderedCount,
                state->lastDpiScale,
                lightTheme)) {
            state->tooltipVisible = true;
            state->lastTooltipCount = state->lastRenderedCount;
            state->lastTooltipLightTheme = lightTheme;
        }
    } else if (hidNative) {
        // Keep custom tooltip cleanly on top if Windows tried to re-show its native tooltip.
        SetWindowPos(
            state->tooltipHwnd,
            HWND_TOPMOST,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
}

static int ResolveNotificationCount(OverlayState* state, const ModSettings& s) {
    if (s.previewCount > 0) {
        return s.previewCount;
    }

    int wnfCount = g_wnfCount.load();
    int polledWnf = 0;
    if (QueryWnfNotificationCount(&polledWnf)) {
        wnfCount = polledWnf;
        g_wnfCount.store(wnfCount);
    }

    if (s.countSource == CountSource::WnfOnly) {
        return std::max(0, wnfCount);
    }

    bool forceDb = g_forceDbRefresh.exchange(false);
    int dbCount = state->dbReader.GetActiveToastCount(forceDb);

    if (s.countSource == CountSource::DbOnly) {
        return (dbCount >= 0) ? dbCount : std::max(0, wnfCount);
    }

    if (dbCount >= 0) {
        return std::max(dbCount, wnfCount);
    }
    return std::max(0, wnfCount);
}

static void UpdateOverlay(HWND hwnd, OverlayState* state) {
    if (!state || g_stopping.load()) {
        return;
    }

    ModSettings s = GetSettingsSnapshot();
    int displayCount = ResolveNotificationCount(state, s);

    if (displayCount <= 0 && s.hideWhenZero) {
        if (state->visible) {
            ShowWindow(hwnd, SW_HIDE);
            state->visible = false;
        }
        HideCustomTooltip(state);
        return;
    }

    HWND hTray = nullptr;
    RECT btnRect{};
    RECT glyphRect{};
    bool hasGlyphRect = false;
    bool isVertical = false;
    bool usedFallback = false;
    if (!state->tracker.GetBellAnchor(
            &hTray, &btnRect, &glyphRect, &hasGlyphRect, &isVertical, &usedFallback) ||
        IsTrueFullscreenAppCoveringMonitor(hTray)) {
        if (state->visible) {
            ShowWindow(hwnd, SW_HIDE);
            state->visible = false;
        }
        HideCustomTooltip(state);
        return;
    }

    UINT dpi = GetDpiForWindow(hTray);
    if (dpi == 0) {
        dpi = 96;
    }
    float dpiScale = dpi / 96.0f;

    int badgeRadius = static_cast<int>(std::lround((s.badgeSize * 0.5f) * dpiScale));
    int targetCenterX = 0;
    int targetCenterY = 0;

    if (isVertical) {
        // Left / Right vertical taskbar: tuck directly below the bell clapper.
        int bellCenterX = (btnRect.left + btnRect.right) / 2;
        targetCenterX = bellCenterX +
                        static_cast<int>(std::lround((1.0f + s.offsetX) * dpiScale));
        if (usedFallback) {
            targetCenterY = btnRect.bottom - badgeRadius -
                            static_cast<int>(std::lround(10.0f * dpiScale)) +
                            static_cast<int>(std::lround(s.offsetY * dpiScale));
        } else {
            targetCenterY = btnRect.bottom + badgeRadius -
                            static_cast<int>(std::lround(8.0f * dpiScale)) +
                            static_cast<int>(std::lround(s.offsetY * dpiScale));
        }
    } else if (hasGlyphRect && s.anchorMode == AnchorMode::Auto) {
        // Top / Bottom horizontal taskbar with cached UIA bell glyph rect.
        if (s.horizontalPlacement == HorizontalPlacement::Right) {
            targetCenterX = glyphRect.right + badgeRadius -
                            static_cast<int>(std::lround(3.0f * dpiScale)) +
                            static_cast<int>(std::lround(s.offsetX * dpiScale));
            targetCenterY = (glyphRect.top + glyphRect.bottom) / 2 +
                            static_cast<int>(std::lround(s.offsetY * dpiScale));
        } else {
            targetCenterX = (glyphRect.left + glyphRect.right) / 2 +
                            static_cast<int>(std::lround(s.offsetX * dpiScale));
            targetCenterY = glyphRect.bottom + badgeRadius -
                            static_cast<int>(std::lround(1.0f * dpiScale)) +
                            static_cast<int>(std::lround(s.offsetY * dpiScale));
            int maxY = btnRect.bottom - badgeRadius;
            if (targetCenterY > maxY && s.offsetY == 0) {
                targetCenterY = maxY;
            }
        }
    } else {
        // Top / Bottom horizontal taskbar using matched bell/notification button rect.
        int btnW = btnRect.right - btnRect.left;
        int bellCenterX = 0;
        if (s.anchorMode == AnchorMode::Center ||
            (s.anchorMode == AnchorMode::Auto && !usedFallback &&
             btnW <= static_cast<int>(std::lround(56.0f * dpiScale)))) {
            bellCenterX = (btnRect.left + btnRect.right) / 2;
        } else if (usedFallback) {
            bellCenterX = btnRect.right - static_cast<int>(std::lround(18.0f * dpiScale));
        } else {
            bellCenterX = btnRect.right - static_cast<int>(std::lround(15.0f * dpiScale));
        }

        if (s.horizontalPlacement == HorizontalPlacement::Right) {
            targetCenterX = bellCenterX +
                            static_cast<int>(std::lround(5.0f * dpiScale)) +
                            badgeRadius +
                            static_cast<int>(std::lround(s.offsetX * dpiScale));
            targetCenterY = (btnRect.top + btnRect.bottom) / 2 +
                            static_cast<int>(std::lround(s.offsetY * dpiScale));
        } else {
            targetCenterX = bellCenterX + static_cast<int>(std::lround(s.offsetX * dpiScale));
            targetCenterY = btnRect.bottom - badgeRadius -
                            static_cast<int>(std::lround(6.0f * dpiScale)) +
                            static_cast<int>(std::lround(s.offsetY * dpiScale));
        }
    }

    bool geometryChanged =
        targetCenterX != state->lastCenterX ||
        targetCenterY != state->lastCenterY ||
        dpi != state->lastDpi;
    bool contentChanged =
        displayCount != state->lastRenderedCount ||
        state->forceRedraw ||
        !state->visible;

    if (contentChanged) {
        if (RenderBadgeToLayeredWindow(hwnd, targetCenterX, targetCenterY, displayCount, dpiScale, s)) {
            if (!state->visible || displayCount != state->lastRenderedCount) {
                Wh_Log(L"Rendered badge count=%d at (%d, %d) btn=[%ld,%ld,%ld,%ld] dpi=%u vertical=%d glyph=%d fallback=%d",
                       displayCount, targetCenterX, targetCenterY,
                       btnRect.left, btnRect.top, btnRect.right, btnRect.bottom,
                       dpi, isVertical ? 1 : 0, hasGlyphRect ? 1 : 0, usedFallback ? 1 : 0);
            }
            state->visible = true;
            state->lastRenderedCount = displayCount;
            state->lastCenterX = targetCenterX;
            state->lastCenterY = targetCenterY;
            state->lastDpi = dpi;
            state->forceRedraw = false;
        }
    } else if (geometryChanged) {
        RECT curRect{};
        GetWindowRect(hwnd, &curRect);
        int curW = curRect.right - curRect.left;
        int curH = curRect.bottom - curRect.top;
        SetWindowPos(
            hwnd,
            HWND_TOPMOST,
            targetCenterX - curW / 2,
            targetCenterY - curH / 2,
            0,
            0,
            SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        state->lastCenterX = targetCenterX;
        state->lastCenterY = targetCenterY;
        state->lastDpi = dpi;
    } else {
        // Re-raise badge only if Shell_TrayWnd moved above it (e.g., on taskbar click/hover).
        bool trayAboveBadge = false;
        for (HWND prev = GetWindow(hwnd, GW_HWNDPREV); prev; prev = GetWindow(prev, GW_HWNDPREV)) {
            if (prev == hTray) {
                trayAboveBadge = true;
                break;
            }
        }
        if (trayAboveBadge) {
            SetWindowPos(
                hwnd,
                HWND_TOPMOST,
                0,
                0,
                0,
                0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }

    // Update cached hover hit-test rect (bell glyph + badge area, excluding the clock).
    RECT bellArea{};
    int pad = static_cast<int>(std::lround(6.0f * dpiScale));
    if (hasGlyphRect) {
        bellArea.left = glyphRect.left - pad;
        bellArea.right = glyphRect.right + pad;
        bellArea.top = std::min(btnRect.top, glyphRect.top - pad);
        bellArea.bottom = std::max(btnRect.bottom, glyphRect.bottom + pad);
    } else if (isVertical) {
        bellArea = btnRect;
    } else {
        bellArea = btnRect;
        int maxBellW = static_cast<int>(std::lround(38.0f * dpiScale));
        if (bellArea.right - bellArea.left > maxBellW) {
            bellArea.left = bellArea.right - maxBellW;
        }
    }
    RECT badgeRect{};
    if (GetWindowRect(hwnd, &badgeRect)) {
        UnionRect(&state->bellHoverRect, &bellArea, &badgeRect);
    } else {
        state->bellHoverRect = bellArea;
    }
    state->lastTrayHwnd = hTray;
    state->lastBtnRect = btnRect;
    state->lastDpiScale = dpiScale;
    UpdateHoverTooltip(state);
}

static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* state = reinterpret_cast<OverlayState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_NCCREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_TIMER:
            if (wParam == kSyncTimerId) {
                UpdateOverlay(hwnd, state);
            } else if (wParam == kHoverTimerId) {
                UpdateHoverTooltip(state);
            }
            return 0;
        case WM_APP_UPDATE_BADGE:
            UpdateOverlay(hwnd, state);
            return 0;
        case WM_APP_RELOAD_SETTINGS:
            if (state) {
                state->forceRedraw = true;
                g_forceDbRefresh.store(true);
            }
            UpdateOverlay(hwnd, state);
            return 0;
        case WM_SETTINGCHANGE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            if (state) {
                state->forceRedraw = true;
                state->lastTooltipCount = -1;
            }
            UpdateOverlay(hwnd, state);
            return 0;
        case WM_DESTROY:
            if (state) {
                KillTimer(hwnd, kSyncTimerId);
                KillTimer(hwnd, kHoverTimerId);
                PostQuitMessage(0);
            }
            return 0;
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

static DWORD WINAPI WorkerThreadProc(LPVOID /*param*/) {
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool coInit = SUCCEEDED(coHr);

    Gdiplus::GdiplusStartupInput gdipInput;
    ULONG_PTR gdipToken = 0;
    bool gdipInit = (Gdiplus::GdiplusStartup(&gdipToken, &gdipInput, nullptr) == Gdiplus::Ok);

    OverlayState state;
    state.tracker.Init();
    state.dbReader.Init();

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kOverlayClassName;
    ATOM clsAtom = RegisterClassExW(&wc);

    if (clsAtom == 0) {
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        state.dbReader.Cleanup();
        state.tracker.Cleanup();
        if (gdipInit) {
            Gdiplus::GdiplusShutdown(gdipToken);
        }
        if (coInit) {
            CoUninitialize();
        }
        return 0;
    }

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kOverlayClassName,
        L"",
        WS_POPUP,
        0,
        0,
        1,
        1,
        nullptr,
        nullptr,
        wc.hInstance,
        &state);

    if (hwnd) {
        state.tooltipHwnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            kOverlayClassName,
            L"",
            WS_POPUP,
            0,
            0,
            1,
            1,
            nullptr,
            nullptr,
            wc.hInstance,
            nullptr);

        g_overlayHwnd.store(hwnd);
        SetTimer(hwnd, kSyncTimerId, kSyncIntervalMs, nullptr);
        SetTimer(hwnd, kHoverTimerId, kHoverIntervalMs, nullptr);
        UpdateOverlay(hwnd, &state);

        MSG msg;
        while (!g_stopping.load() && GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        g_overlayHwnd.store(nullptr);
        if (state.tooltipHwnd && IsWindow(state.tooltipHwnd)) {
            DestroyWindow(state.tooltipHwnd);
            state.tooltipHwnd = nullptr;
        }
        if (IsWindow(hwnd)) {
            DestroyWindow(hwnd);
        }
    } else {
        Wh_Log(L"Failed to create overlay window: %lu", GetLastError());
    }

    UnregisterClassW(kOverlayClassName, wc.hInstance);

    state.dbReader.Cleanup();
    state.tracker.Cleanup();

    if (gdipInit) {
        Gdiplus::GdiplusShutdown(gdipToken);
    }
    if (coInit) {
        CoUninitialize();
    }
    return 0;
}

// ============================================================================
// Windhawk Tool Mod Entry Points
// ============================================================================

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Taskbar Notification Bell Count Badge v1.0.24 (tool mod)");
    g_stopping.store(false);
    g_forceDbRefresh.store(true);
    LoadSettings();
    InitWnf();

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread: %lu", GetLastError());
        CleanupWnf();
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Notification Bell Count Badge");
    g_stopping.store(true);
    CleanupWnf();

    HWND hwnd = g_overlayHwnd.load();
    if (hwnd) {
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
    }
    if (g_workerThreadId != 0) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    HWND hwnd = g_overlayHwnd.load();
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
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

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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
        DWORD dwCreationFlags, LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
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
