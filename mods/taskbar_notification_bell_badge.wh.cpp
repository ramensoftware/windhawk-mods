// ==WindhawkMod==
// @id              taskbar-notification-bell-badge
// @name            Taskbar Notification Bell Count Badge
// @name:vi-VN      Số lượng thông báo trên chuông Taskbar
// @description     Displays the unread notification count inside a small circular badge next to or below the Windows 11 taskbar notification bell icon
// @description:vi-VN Hiển thị số lượng thông báo trong một vòng tròn nhỏ bên cạnh hoặc bên dưới biểu tượng quả chuông trên thanh Taskbar Windows 11
// @version         1.0.19
// @author          caotu2005
// @github          https://github.com/caotu2005
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lgdiplus -lole32 -loleaut32 -luuid -ldwmapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Notification Bell Count Badge

Displays the current number of active Windows notifications inside a small
circular (or pill-shaped for 2+ digits) badge anchored to the Windows 11
taskbar notification bell icon.

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
- **Accurate Notification Center count**: Reads active toast notifications
  directly from Windows' per-user notification database (`wpndatabase.db` via
  the built-in `winsqlite3.dll`, read-only) and subscribes to
  `WNF_SHEL_NOTIFICATIONS` for instant updates when notifications arrive or are
  cleared.
- **Theme & Taskbar Styler friendly**: Uses UI Automation (`IUIAutomation`) and
  a DPI-aware, anti-aliased, click-through GDI+ layered overlay
  (`PixelFormat32bppPARGB`)—no `Taskbar.View.dll` symbol hooks and no
  `IXamlDiagnostics` conflict with **Windows 11 Taskbar Styler**.
- **Smart visibility**: Stays visible next to maximized windows and
  automatically hides for true fullscreen apps, auto-hidden taskbars, or when
  there are 0 notifications.

## Positioning tip
Set **Preview count** to a number like `5` in Settings to fine-tune **Offset X**,
**Offset Y**, and **Badge diameter**, then set **Preview count** back to `0`.

---

# Số lượng thông báo trên chuông Taskbar (Tiếng Việt)

Hiển thị số lượng thông báo đang có trong Windows Notification Center bên trong
một vòng tròn nhỏ (tự mở rộng thành dạng viên thuốc khi có từ 2 chữ số trở lên)
gắn ngay cạnh hoặc bên dưới biểu tượng quả chuông trên thanh Taskbar Windows 11.

## Tính năng chính
- **Hỗ trợ đầy đủ 4 cạnh Taskbar (Dưới, Trên, Trái, Phải)**:
  - **Taskbar Trên / Dưới (ngang)**: tự động đặt gọn **bên phải** quả chuông,
    tách khỏi vạch Show Desktop (có thể đổi sang bên dưới trong Cài đặt).
  - **Taskbar Trái / Phải (dọc)**: tự động đặt sát **bên dưới** quả chuông.
- **Đếm thông báo chính xác & tức thì**: Đọc trực tiếp số thông báo đang hiện
  trong Notification Center từ `wpndatabase.db` (chỉ đọc qua `winsqlite3.dll` có
  sẵn của Windows) kết hợp đăng ký sự kiện `WNF_SHEL_NOTIFICATIONS` để cập nhật
  ngay lập tức khi có thông báo mới hoặc khi xóa thông báo.
- **Tương thích tốt với Windows 11 Taskbar Styler**: Định vị nút chuông qua
  UI Automation và vẽ bằng lớp phủ GDI+ trong suốt click-through sắc nét ở mọi
  mức DPI, không hook symbol `Taskbar.View.dll` và không xung đột
  `IXamlDiagnostics`.
- **Tự động ẩn thông minh**: Vẫn hiển thị bình thường khi phóng to cửa sổ và tự
  ẩn khi chạy ứng dụng toàn màn hình (fullscreen), khi ẩn taskbar hoặc khi không
  có thông báo nào.

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
#include <uiautomation.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <mutex>
#include <string>
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
// ============================================================================

struct sqlite3;
struct sqlite3_stmt;

static constexpr int kSqliteOk = 0;
static constexpr int kSqliteRow = 100;
static constexpr int kSqliteOpenReadonly = 0x00000001;

using PFN_sqlite3_open_v2 = int (*)(const char* filename, sqlite3** ppDb, int flags, const char* zVfs);
using PFN_sqlite3_close = int (*)(sqlite3* db);
using PFN_sqlite3_busy_timeout = int (*)(sqlite3* db, int ms);
using PFN_sqlite3_prepare_v2 = int (*)(sqlite3* db, const char* zSql, int nByte, sqlite3_stmt** ppStmt, const char** pzTail);
using PFN_sqlite3_bind_int64 = int (*)(sqlite3_stmt* pStmt, int idx, int64_t val);
using PFN_sqlite3_step = int (*)(sqlite3_stmt* pStmt);
using PFN_sqlite3_column_int = int (*)(sqlite3_stmt* pStmt, int iCol);
using PFN_sqlite3_finalize = int (*)(sqlite3_stmt* pStmt);

// ============================================================================
// Constants & Messages
// ============================================================================

static constexpr wchar_t kOverlayClassName[] = L"WhNotificationBellBadgeOverlay";
static constexpr UINT WM_APP_UPDATE_BADGE = WM_APP + 1;
static constexpr UINT WM_APP_RELOAD_SETTINGS = WM_APP + 2;
static constexpr UINT_PTR kSyncTimerId = 1;
static constexpr UINT kSyncIntervalMs = 250;

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
    IUIAutomationElement* cachedButton = nullptr;
    HWND cachedTrayHwnd = nullptr;
    int lastIsVertical = -1;
    bool loggedFallback = false;

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

        return true;
    }

    void ClearCache() {
        if (cachedButton) {
            cachedButton->Release();
            cachedButton = nullptr;
        }
        cachedTrayHwnd = nullptr;
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
        if (uia) {
            uia->Release();
            uia = nullptr;
        }
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
        if (hTray != cachedTrayHwnd || lastIsVertical != orientVal) {
            ClearCache();
            cachedTrayHwnd = hTray;
            lastIsVertical = orientVal;
            loggedFallback = false;
        }

        if (!cachedButton && uia) {
            std::vector<HWND> roots;
            EnumChildWindows(hTray, CollectChildHwndsProc, reinterpret_cast<LPARAM>(&roots));
            roots.push_back(hTray);

            for (HWND rootHwnd : roots) {
                IUIAutomationElement* rootElem = nullptr;
                if (SUCCEEDED(uia->ElementFromHandle(rootHwnd, &rootElem)) && rootElem) {
                    cachedButton = FindButtonInRoot(rootElem, isVertical, scale);
                    rootElem->Release();
                    if (cachedButton) {
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
                    RECT glyphRc{};
                    if (FindBellGlyphRect(cachedButton, scale, &glyphRc)) {
                        *outGlyphRect = glyphRc;
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

struct OverlayState {
    TaskbarBellTracker tracker;
    WpnDbReader dbReader;
    bool visible = false;
    int lastRenderedCount = -1;
    int lastCenterX = 0;
    int lastCenterY = 0;
    UINT lastDpi = 0;
    bool forceRedraw = true;
};

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
        // Top / Bottom horizontal taskbar with exact UIA bell glyph rect.
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
    } else if (state->visible) {
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
            }
            UpdateOverlay(hwnd, state);
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kSyncTimerId);
            PostQuitMessage(0);
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
        g_overlayHwnd.store(hwnd);
        SetTimer(hwnd, kSyncTimerId, kSyncIntervalMs, nullptr);
        UpdateOverlay(hwnd, &state);

        MSG msg;
        while (!g_stopping.load() && GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        g_overlayHwnd.store(nullptr);
        if (IsWindow(hwnd)) {
            DestroyWindow(hwnd);
        }
    } else {
        Wh_Log(L"Failed to create overlay window: %lu", GetLastError());
    }

    if (clsAtom != 0) {
        UnregisterClassW(kOverlayClassName, wc.hInstance);
    }

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
// Windhawk Mod Entry Points
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Notification Bell Count Badge v1.0.19");
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

void Wh_ModUninit() {
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
        WaitForSingleObject(g_workerThread, 3000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    HWND hwnd = g_overlayHwnd.load();
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}